#include "saga.h"

STATIC_ASSERT(offsetof(MAINFRAMECOUNTERS, warmup_frame) == 0);
STATIC_ASSERT(offsetof(MAINFRAMECOUNTERS, every_frame) == 1);
STATIC_ASSERT(offsetof(MAINFRAMECOUNTERS, alternate_frame) == 2);
STATIC_ASSERT(offsetof(MAINFRAMECOUNTERS, third_frame) == 3);
STATIC_ASSERT(offsetof(MAINFRAMECOUNTERS, fourth_frame) == 4);
STATIC_ASSERT(sizeof(TIMER) == 0x10);
STATIC_ASSERT(offsetof(TIMER, time_elapsed) == 0);
STATIC_ASSERT(offsetof(TIMER, last_time_elapsed) == 4);
STATIC_ASSERT(offsetof(TIMER, time_elapsed_mod_seconds) == 8);
STATIC_ASSERT(offsetof(TIMER, update_count) == 0xc);

extern f32 FRAMETIME;
extern f32 __cdecl NuFmod(f32 value, f32 divisor);

void ResetFrameCounters(void) {
    MainFrameCounters.warmup_frame = 0;
    MainFrameCounters.every_frame = 0xff;
    MainFrameCounters.alternate_frame = 0xff;
    MainFrameCounters.third_frame = 0xff;
    MainFrameCounters.fourth_frame = 0xff;
}

void UpdateFrameCounters(void) {
    if (do_multiframe_update != 0) {
        if ((i8)MainFrameCounters.warmup_frame <= 0) {
            ++MainFrameCounters.warmup_frame;
            return;
        }

        MainFrameCounters.every_frame = 1;
        MainFrameCounters.alternate_frame = (MainFrameCounters.alternate_frame + 1) & 1;
        if (++MainFrameCounters.third_frame == 3) {
            MainFrameCounters.third_frame = 0;
        }
        MainFrameCounters.fourth_frame = (MainFrameCounters.fourth_frame + 1) & 3;
        return;
    }

    ResetFrameCounters();
}

void ResetTimer(TIMER *timer, f32 reset_time) {
    timer->last_time_elapsed = reset_time;
    timer->time_elapsed = reset_time;
    timer->time_elapsed_mod_seconds = NuFmod(reset_time, 1.0f);
    timer->update_count = 0;
}

void UpdateTimer(TIMER *timer) {
    timer->last_time_elapsed = timer->time_elapsed;
    timer->time_elapsed += FRAMETIME;
    timer->time_elapsed_mod_seconds += FRAMETIME;

    while (timer->time_elapsed_mod_seconds >= 1.0f) {
        timer->time_elapsed_mod_seconds -= 1.0f;
    }

    if ((u32)timer->update_count >= 0xffffffffu)
        return;

    timer->update_count++;
}

extern TIMER BonusTimer, OverallGamePlayTimer, ChallengeTimer;
extern f32 global_pulse;

typedef struct GAME_TIMING_MISSION_DATA_PC {
    u8 unknown_00[0x10];
    u16 time;
} GAME_TIMING_MISSION_DATA_PC;
typedef struct GAME_TIMING_MISSION_SYSTEM_PC {
    void *missions;
    GAME_TIMING_MISSION_DATA_PC *active_mission;
    TIMER timer;
} GAME_TIMING_MISSION_SYSTEM_PC;
typedef struct GAME_TIMING_AREA_SAVE_PC {
    u8 unknown_00[8];
    f32 challenge_trial_time;
} GAME_TIMING_AREA_SAVE_PC;
STATIC_ASSERT(offsetof(GAME_TIMING_MISSION_DATA_PC, time) == 0x10);
STATIC_ASSERT(offsetof(GAME_TIMING_MISSION_SYSTEM_PC, active_mission) == 4);
STATIC_ASSERT(offsetof(GAME_TIMING_MISSION_SYSTEM_PC, timer) == 8);
STATIC_ASSERT(sizeof(GAME_TIMING_AREA_SAVE_PC) == 0xc);
STATIC_ASSERT(offsetof(GAME_TIMING_AREA_SAVE_PC, challenge_trial_time) == 8);
STATIC_ASSERT(offsetof(WORLDINFO, area_index) == 0x120);

void __cdecl GameTiming(WORLDINFO *world, f32 *game_time) {
    f32 target = 0.0f;
    f32 pulse;
    GAME_TIMING_MISSION_SYSTEM_PC *mission;
    f32 time_limit;
    /* Existing PC owner names are reversed: Mul is the countdown, Time the fade. */
    if (LevelNameMul > 0.0f) {
        LevelNameMul -= FRAMETIME;
        target = 1.0f;
    }
    LevelNameTime = SeekLinearF(LevelNameTime, target, FRAMETIME + FRAMETIME);
    if (Paused != 0) {
        if (screendump == 0) {
            UpdateTimer(&PauseTimer);
            if (LevelNameMul < 0.5f) {
                LevelNameMul = 0.5f;
            }
        }
        DoubleScoreTime = 0.0f;
    } else {
        if (game_time != NULL) {
            *game_time += FRAMETIME;
        }
        UpdateTimer(&GameTimer);
        UpdateTimer(&LevelTimer);
        UpdateTimer(&AreaTimer);
        UpdateTimer(&JoinInTimer);
        if (CUTSTOPGAME != 0) {
            DoubleScoreTime = 0.0f;
        } else {
            if (world->current_level != NULL && (world->current_level->flags & 2) != 0 && pause_rndr_on == 0 &&
                (Player[0] != NULL || Player[1] != NULL)) {
                UpdateTimer(&GamePlayTimer);
                UpdateTimer(&OverallGamePlayTimer);
                if (BonusWinner == -1) {
                    UpdateTimer(&BonusTimer);
                }
                if ((world->current_level->flags & 0x2000) != 0) {
                    UpdateTimer(&SuperStoryTimer);
                    if (SuperStoryTimer.time_elapsed > 36000.0f) {
                        ResetTimer(&SuperStoryTimer, 36000.0f);
                    }
                }
                if (world->area != NULL && (world->area->flags & 0x10) != 0) {
                    if (ChallengeMode == 1) {
                        UpdateTimer(&ChallengeTimer);
                        if (Game_AreaSave != NULL &&
                            ChallengeTimer.time_elapsed >=
                                ((GAME_TIMING_AREA_SAVE_PC *)Game_AreaSave)[world->area_index].challenge_trial_time) {
                            EndChallenge(3, 1);
                        }
                    } else if (Mission_CurrentState(NULL) == 1) {
                        mission = (GAME_TIMING_MISSION_SYSTEM_PC *)MissionSys;
                        time_limit = (f32)mission->active_mission->time;
                        UpdateTimer(&mission->timer);
                        if (mission->timer.time_elapsed >= time_limit && netclient == 0) {
                            EndMission(MissionSys, 3, 1);
                        }
                    }
                }
            }
            UpdateGameMessages();
            target = 0.0f;
            if (DoubleScore != 0 && GetMenuID() == -1) {
                target = 1.0f;
            }
            DoubleScoreTime = SeekLinearF(DoubleScoreTime, target, FRAMETIME);
        }
    }
    UpdateTimer(&GlobalTimer);
    menu_flash = NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.2f) < 0.1f;
    pulse = NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f);
    game_pulse = NuTrigTable[(u16)(i32)(((f64)pulse + (f64)pulse) * 65536.0) >> 1];
    pulse = NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.5f);
    global_pulse = NuTrigTable[(u16)(i32)(((f64)pulse + (f64)pulse) * 65536.0) >> 1];
    MainRenderTime = SeekLinearF(MainRenderTime, MainRenderTargetTime, FRAMETIME);
    qrand();
}
