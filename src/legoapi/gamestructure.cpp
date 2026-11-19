#include "batman.h"
#include "gameapi/gui/apimenu.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/core/save_values.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/screens/gamestructure.h"
#include "legoapi/misc/buffers.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/render/core/screen.h"
#include "legoapi/world/area.h"
#include "legoapi/world/areas.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "legogame/game.h"
#include "legogame/startup.h"
#include "nu2api/PCInput.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nucore/nuvideo.h"
#include "nu2api/numusic/numusic.h"
#include "nu2api/nusound/nusound.h"
#include "nu2api/nuwin32/nu2api_windows.h"
#include "legoapi/menus/screens/gamestatus_lsw.h"

extern "C" {
    i32 SuperStoryEpisode = -1;
    i32 pause_i_pad = -1;
    i32 reset_load = 1;
    i32 LEGOMENU_PAUSEMAIN = -1;
    i32 LEGOMENU_PAUSESYNC = -1;
    void (*PauseGame_ExtraCodeFn)(void) = NULL;
}

extern "C" {
    TIMER PauseTimer = {0};
    f32 DoubleScoreTime = 0;
}

extern "C" void PauseGame(i32 pad) {
    Paused = 1;
    NuPause(1);
    music_man.SetFader(0.0f, 0.5f);
    music_man.PauseTrack(0x10);
    NuSound3StopRumble();
    if (memcard_autosaveenabled && memcard_autosavedisabled) {
        NewMenu(MENU_AUTO_SAVE_CANCEL, 0, -1);
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
            Player[i]->player.hud_icon_timer = 0.0f;
            Player[i]->player.input_toggle_hold_time = TOGGLEHOLDTIME;
            Player[i]->pause_context_state = 0.0f;
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
            Player[i]->player.hud_icon_timer = 0.0f;
            Player[i]->player.input_toggle_hold_time = TOGGLEHOLDTIME;
            Player[i]->pause_context_state = 0.0f;
        }
    }
    if (PauseGame_ExtraCodeFn) {
        PauseGame_ExtraCodeFn();
    }
}

extern "C" {

    void (*ResumeGame_ExtraCodeFn)(void) = NULL;
    i32 GAMEDEMO = 0;

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

    OPTIONSSAVE *Game_OptionsSave = 0;
    u32 SuperStoryScore = 0;
    i32 NextArea_FreePlay = 0;
    i32 save_paused = 0;
    i32 FreePlay = 0;
    i32 SuperStory = 0;

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

extern "C" {
    i32 Arcade = 0;
    i32 Paused = 0;
    i32 NetPaused = 0;
}

extern "C" i32 InStory(void) {
    if (FreePlay != 0 || ChallengeMode != CHALLENGE_OFF || Mission_Active(NULL) != NULL || Arcade != 0) {
        return 0;
    }
    return 1;
}

extern "C" {
    NUGSCN *save_icon_scene = 0;
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

extern "C" LEVELDATA *CanSaveAndExit(WORLDINFO *world) {
    if (GAMEDEMO == 0 && SuperStory == 0 && world->area != NULL && world->area != HUB_ADATA &&
        (world->area->flags & (AREAFLAG_ENDING_AREA | AREAFLAG_BONUS_AREA | AREAFLAG_HUB_AREA | AREAFLAG_SUPER_BONUS)) == 0 &&
        Mission_Active(NULL) == NULL && ChallengeMode == CHALLENGE_OFF && Arcade == 0 && CutScenePlayer_Active() == NULL &&
        Game_AreaSave != NULL && reinterpret_cast<AREASAVE_s *>(Game_AreaSave)[world->area_index].area_complete != 0 &&
        AreaGlobals.special_pickups_collected > 0) {
        return Area_FindStatusLevel(world->area, NULL);
    }
    return NULL;
}

extern "C" {
    STATUSCOLLECT_s *CustomiserCompletionSave = NULL;
    i32 HubCompletionPoints = 0;
    void (*Game_100PercentFn)(void) = NULL;
}

extern "C" void AddToCompletionPoints(u32 points) {
    if (CustomiserCompletionSave != NULL && CustomiserCompletionSave->completion_points < HubCompletionPoints) {
        CustomiserCompletionSave->completion_points += points;
        if (CustomiserCompletionSave->completion_points >= HubCompletionPoints) {
            CustomiserCompletionSave->completion_points = (u16)HubCompletionPoints;
            if ((CustomiserCompletionSave->flags & SAVE_REWARD_100_PERCENT) == 0) {
                if (Game_100PercentFn != NULL) {
                    Game_100PercentFn();
                }
                CustomiserCompletionSave->flags |= SAVE_REWARD_100_PERCENT;
            }
        }
    }
}

extern "C" {

    i32 Game_100PercentComplete(void) {
        if (CustomiserCompletionSave != NULL && (CustomiserCompletionSave->flags & 1) != 0) {
            return 1;
        }
        return 0;
    }
}

extern "C" {
    i32 GOLDBRICKPOINTS = 0;
    i32 SHOPGOLDBRICKS = 0;
    i32 HubBothTrueJediGoldBricks = 0;
    i32 EpisodeGoldBrickForSuperBonus = 0;
    i32 EpisodeGoldBrickForSuperStory = 0;
    i32 EpisodeGoldBrickForChallenge = 0;
    void (*Game_AllGoldBricksFn)(void) = NULL;
    GAMEOBJECT *Player[8] = {0};
}

extern "C" void AddToGoldBricks() {
    if (CustomiserCompletionSave != NULL && CustomiserCompletionSave->gold_bricks < GOLDBRICKPOINTS) {
        ++CustomiserCompletionSave->gold_bricks;
        if (CustomiserCompletionSave->gold_bricks == GOLDBRICKPOINTS &&
            (CustomiserCompletionSave->flags & SAVE_REWARD_ALL_GOLD_BRICKS) == 0) {
            if (Game_AllGoldBricksFn != NULL) {
                Game_AllGoldBricksFn();
            }
            CustomiserCompletionSave->flags |= SAVE_REWARD_ALL_GOLD_BRICKS;
        }
    }
}

extern "C" i32 fun_0055c8f0() {
    if (CustomiserCompletionSave != NULL && (CustomiserCompletionSave->flags & 2) != 0) {
        return 1;
    }
    return 0;
}

extern "C" i32 Memcard_AutosaveBusy() {
    if (memcard_autosaveneeded != 0 || memcard_autosaveinprogress != 0) {
        return 1;
    }
    return 0;
}
