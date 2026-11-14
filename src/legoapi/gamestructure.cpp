#include "saga.h"

extern "C" void DumpAreaData(i32 mode, i32 last_area);
extern "C" void BackDrop_Dump(void);

STATIC_ASSERT(offsetof(AREASAVE_s, area_complete) == 1);
STATIC_ASSERT(offsetof(STATUSCOLLECT_s, completion_points) == 0);
STATIC_ASSERT(offsetof(STATUSCOLLECT_s, flags) == 3);
extern "C" {
    extern void (*PauseGame_ExtraCodeFn)(void);
    extern void (*ResumeGame_ExtraCodeFn)(void);
}

extern "C" void PauseGame(i32 pad) {
    Paused = 1;
    NuPause(1);
    music_man.SetFader(0.0f, 0.5f);
    music_man.PauseTrack(0x10);
    NuSound3StopRumble();
    if (memcard_autosaveenabled && memcard_autosavedisabled) {
        NewMenu(1011, 0, -1);
    } else {
        NewMenu(LEGOMENU_PAUSEMAIN, 0, -1);
    }
    ResetTimer(&PauseTimer, 0.0f);
    GameAudio_PlaySfx(0x36, NULL, 0, 0);
    DoubleScoreTime = 0.0f;
    ResetTimer(&JoinInTimer, 0.0f);
    pause_i_pad = pad;
    PcInput_SetCtrlStringPlayer(pad);
    for (i32 i = 0; i < 8; ++i) {
        if (Player[i]) {
            ((GAMEOBJECT *)Player[i])->hud_icon_timer = 0.0f;
            ((GAMEOBJECT *)Player[i])->input_toggle_hold_time = TOGGLEHOLDTIME;
            ((GAMEOBJECT *)Player[i])->pause_context_state = 0.0f;
        }
    }
    if (PauseGame_ExtraCodeFn) {
        PauseGame_ExtraCodeFn();
    }
}

extern "C" void NetworkSyncPause(void) {
    if (Paused == 0) {
        NeedScreenGrab(1);
    }
    Paused = 1;
    CutInstEndCount = 0;
    NuPause(1);
    NewMenu(LEGOMENU_PAUSESYNC, 0, -1);
    ResetTimer(&PauseTimer, 0.0f);
    music_man.SetFader(0.0f, 0.5f);
    music_man.PauseTrack(0x10);
    NuSound3StopRumble();
    DoubleScoreTime = 0.0f;
    ResetTimer(&JoinInTimer, 0.0f);
    for (i32 i = 0; i < 8; ++i) {
        if (Player[i]) {
            ((GAMEOBJECT *)Player[i])->hud_icon_timer = 0.0f;
            ((GAMEOBJECT *)Player[i])->input_toggle_hold_time = TOGGLEHOLDTIME;
            ((GAMEOBJECT *)Player[i])->pause_context_state = 0.0f;
        }
    }
    if (PauseGame_ExtraCodeFn) {
        PauseGame_ExtraCodeFn();
    }
}

extern "C" {

    void ResumeGame(i32 play_sound) {
        Paused = 0;
        NetPaused = 0;
        NuPause(0);
        MenuRememberCursor(&GameMenu[GameMenuLevel]);
        MenuReset();
        music_man.SetFader(1.0f, 0.5f);
        music_man.ResumeTrack(0x10);
        if (play_sound != 0) {
            GameAudio_PlaySfx(0x37, NULL, 0, 0);
        }
        if (ResumeGame_ExtraCodeFn != NULL) {
            ResumeGame_ExtraCodeFn();
        }
    }
}

void ClearPause(void) {
    Paused = 0;
    NetPaused = 0;
}

extern "C" void NewGameMode() {
    NewMode = 1;
    reset_load = 1;
}

extern "C" {

    void RestoreOptions(void) {
        GameSetSoundVolume(Game_OptionsSave);
        GameSetMusicVolume(Game_OptionsSave);
        if (Game_OptionsSave != NULL) {
            NuVideoSetBrightness((f32)((f64)Game_OptionsSave->brightness / 10.0));
        }
    }

    void InitSuperStory(i32 episode) {
        SuperStory = 1;
        SuperStoryEpisode = episode;
        ResetTimer(&SuperStoryTimer, 0.0f);
        SuperStoryScore = 0;
        NextArea_FreePlay = 0;
        FreePlay = 0;
        Cheats_TurnOff(0);
    }
}

extern "C" i32 InStory(void) {
    if (FreePlay != 0 || ChallengeMode != 0 || Mission_Active(NULL) != NULL || Arcade != 0) {
        return 0;
    }
    return 1;
}

i32 Game_Exit(i32 last_area) {
    if (PcWindowQuit != 0) {
        if (WorldInfo[0].loaded != 0) {
            WorldInfo_Dump(&WorldInfo[0]);
        }
        if (WorldInfo[1].loaded != 0) {
            WorldInfo_Dump(&WorldInfo[1]);
        }
        DumpAreaData(1, last_area);
        if (save_icon_scene != NULL) {
            NuGScnRemove(save_icon_scene);
        }
        if (big_icon_scene != NULL) {
            NuGScnRemove(big_icon_scene);
        }
        if (area_scene != NULL) {
            NuGScnRemove(area_scene);
        }
        if (things_scene != NULL) {
            NuGScnRemove(things_scene);
        }
        if (vehicle_scene != NULL) {
            NuGScnRemove(vehicle_scene);
        }
        BackDrop_Dump();
        return last_area;
    }
    return 0;
}

extern "C" LEVELDATA_PC *CanSaveAndExit(WORLDINFO *world) {
    if (GAMEDEMO == 0 && SuperStory == 0 && world->area != NULL && world->area != HUB_ADATA &&
        (world->area->flags & 0x146) == 0 && Mission_Active(NULL) == NULL && ChallengeMode == 0 && Arcade == 0 &&
        CutScenePlayer_Active() == NULL && Game_AreaSave != NULL &&
        reinterpret_cast<AREASAVE_s *>(Game_AreaSave)[world->area_index].area_complete != 0 &&
        AreaGlobals_PC.field_18 > 0) {
        return Area_FindStatusLevel(world->area, NULL);
    }
    return NULL;
}

extern "C" void AddToCompletionPoints(u32 points) {
    if (CustomiserCompletionSavePc != NULL &&
        ((STATUSCOLLECT_s *)CustomiserCompletionSavePc)->completion_points < HubCompletionPointsPc) {
        ((STATUSCOLLECT_s *)CustomiserCompletionSavePc)->completion_points += points;
        if (((STATUSCOLLECT_s *)CustomiserCompletionSavePc)->completion_points >= HubCompletionPointsPc) {
            ((STATUSCOLLECT_s *)CustomiserCompletionSavePc)->completion_points = (u16)HubCompletionPointsPc;
            if ((((STATUSCOLLECT_s *)CustomiserCompletionSavePc)->flags & SAVE_REWARD_100_PERCENT) == 0) {
                if (Game_100PercentFn != NULL) {
                    Game_100PercentFn();
                }
                ((STATUSCOLLECT_s *)CustomiserCompletionSavePc)->flags |= SAVE_REWARD_100_PERCENT;
            }
        }
    }
}

extern "C" {

    i32 Game_100PercentComplete(void) {
        if (CustomiserCompletionSavePc != NULL && (CustomiserCompletionSavePc->flags & 1) != 0) {
            return 1;
        }
        return 0;
    }
}

extern "C" void AddToGoldBricks() {
    if (CustomiserCompletionSavePc != NULL &&
        ((STATUSCOLLECT_s *)CustomiserCompletionSavePc)->gold_bricks < GOLDBRICKPOINTS) {
        ++((STATUSCOLLECT_s *)CustomiserCompletionSavePc)->gold_bricks;
        if (((STATUSCOLLECT_s *)CustomiserCompletionSavePc)->gold_bricks == GOLDBRICKPOINTS &&
            (((STATUSCOLLECT_s *)CustomiserCompletionSavePc)->flags & SAVE_REWARD_ALL_GOLD_BRICKS) == 0) {
            if (Game_AllGoldBricksFn != NULL) {
                Game_AllGoldBricksFn();
            }
            ((STATUSCOLLECT_s *)CustomiserCompletionSavePc)->flags |= SAVE_REWARD_ALL_GOLD_BRICKS;
        }
    }
}

extern "C" i32 fun_0055c8f0() {
    if (CustomiserCompletionSavePc != NULL && (CustomiserCompletionSavePc->flags & 2) != 0) {
        return 1;
    }
    return 0;
}

extern "C" i32 fun_0055c910() {
    if (memcard_autosaveneeded != 0 || memcard_autosaveinprogress != 0) {
        return 1;
    }
    return 0;
}
