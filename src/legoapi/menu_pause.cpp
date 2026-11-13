#include "saga.h"

STATIC_ASSERT(sizeof(FADETYPE) == 4);
STATIC_ASSERT(sizeof(FadeSystem) == 0x2c);
STATIC_ASSERT(sizeof(GAMEPAD_INIT_RECORD) == 0x60);
extern "C" {
    void __cdecl ResumeGame(i32);
}

extern "C" {
    void __cdecl StartDropOut(GAMEOBJECT *);
    void __cdecl AICreatureResumeScript(GAMEOBJECT *);
}
extern "C" void __cdecl DropOut(i32 player, i32 resume, i32 silent, i32) {
    GAMEOBJECT *object = (GAMEOBJECT *)Player[player];
    if (!object)
        return;
    if (!silent) {
        if (!object->rider)
            NuPadSetStatus((GAMEPAD_INIT_RECORD *)object->pad - GamePadStorage, 0);
        GameAudio_PlaySfx(0x20, &object->collision_position, 0, 0);
    }
    ReleaseTakeOver(object, 1);
    i16 secondary = object->secondary_lean_angle;
    i16 movement = object->movement_lean_angle;
    i16 tertiary = object->tertiary_lean_angle;
    ReleaseEat(object);
    if (VehicleArea) {
        ResetPlayerPacket(object->unknown_06c4, object->character_data);
        object->secondary_lean_angle = secondary;
        object->movement_lean_angle = movement;
        object->tertiary_lean_angle = tertiary;
    }
    object->flags_1fc &= ~0x80u;
    AICreatureResumeScript(object);
    if (VehicleArea)
        StartDropOut(object);
    PlayerProgress[player].hitpoints =
        object->dead || object->character_context == 0x2b ? object->field_107e : object->current_hp;
    object->powerup_time = 0.0f;
    GameCam_Blend(GameCam, VehicleArea ? 0.5f : 1.0f, 0.0f, 1);
    if (resume) {
        ResumeGame(1);
        TextInputSetKeyboardOverridePc(0);
    }
    GAMEOBJECT *other = (GAMEOBJECT *)Player[player == 0];
    if (other) {
        NUVEC direction;
        NuVecSub(&direction, &other->collision_position, &GameCam->position);
        if (NewRayCastScaleYMask(&GameCam->position, &direction, 0.0f, 1.0f, 0, 0x1f))
            GameCam_Reset(GameCam);
    }
}

extern "C" {
    extern void(__cdecl *PauseGame_ExtraCodeFn)(void);
}
extern "C" void __cdecl PauseGame(i32 pad) {
    Paused = 1;
    NuPause(1);
    music_man.SetFader(0.0f, 0.5f);
    music_man.PauseTrack(0x10);
    NuSound3StopRumble();
    if (memcard_autosaveenabled && memcard_autosavedisabled)
        NewMenu(1011, 0, -1);
    else
        NewMenu(LEGOMENU_PAUSEMAIN, 0, -1);
    ResetTimer(&PauseTimer, 0.0f);
    GameAudio_PlaySfx(0x36, NULL, 0, 0);
    DoubleScoreTime = 0.0f;
    ResetTimer(&JoinInTimer, 0.0f);
    pause_i_pad = pad;
    TextInputSetLastDevicePc(pad);
    for (i32 i = 0; i < 8; ++i) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[i];
        if (object) {
            object->hud_icon_timer = 0.0f;
            object->input_toggle_hold_time = TOGGLEHOLDTIME;
            object->pause_context_state = 0.0f;
        }
    }
    if (PauseGame_ExtraCodeFn)
        PauseGame_ExtraCodeFn();
}

extern "C" void __cdecl PadOutPause(i32 index, WORLDINFO *world) {
    if (Paused)
        return;
    i32 menu = GetMenuID();
    if (!((GAMEPAD_RUNTIME_INPUT_PC *)GamePadStorage)[index].input_ready ||
        (((GAMEPAD_RUNTIME_INPUT_PC *)GamePadStorage)[index].bound_pad->flags & 4))
        return;
    if ((LEGOMENU_NEWGAME != -1 && menu == LEGOMENU_NEWGAME) || (LEGOMENU_CREDITS != -1 && menu == LEGOMENU_CREDITS))
        return;
    if (NewMode || NewLData || ((FADEINFO_PC *)&FadeSys)->fade != 0.0f)
        return;
    if (GamePads_IgnoreInputFn && GamePads_IgnoreInputFn())
        return;
    if (MiniCutCam)
        return;
    if (world->area && (world->area->flags & 2))
        return;
    if (world->current_level->flags & 0xe0)
        return;
    if (GetMenuID() != -1)
        return;
    if (CUTSTOPGAME && !CutScene_IsSkippable((CUTINFO *)CutStopInfo))
        return;
    if (!CutSceneWaiting)
        PauseGame(index);
}

/* Replace/move the two unused static menu.c bodies with these PC-authoritative external bodies. */

/* Native autosave-cancel child1011 reached by PauseGame; save1000 child already implemented. */
extern "C" {
    extern u8 MenuAutoSaveCancelFirstPc;
    extern i32 SaveloadPendingPc;
    extern u32 SaveloadStartedTickPc;
    i32 __cdecl saveload_getinfo(void);

    void __cdecl MenuDrawAutoSaveCancel(MENU *menu) {
        if (!MenuASCancelFinished && !memcard_savefailed) {
            menu->draw_y = MENUBOTY;
            DrawMenuEntryEx(menu, apitxt_OK, (u8)MenuA);
        }
    }

    void __cdecl MenuUpdateAutoSaveCancel(MENU *menu) {
        if (MenuASCancelFinished) {
            MenuASCancelFinished = 0;
            BackupMenu();
            return;
        }
        if (memcard_savefailed) {
            if (MenuAutoSaveCancelFirstPc) {
                MenuAutoSaveCancelFirstPc = 0;
                memcard_autosavecanceldelay = 5.0f;
                g_enableButtonPrompts = 0;
                return;
            }
            if (memcard_autosavecanceldelay > 0.0f)
                return;
            g_enableButtonPrompts = 1;
            NewMenu(1000, -1, -1);
            memcard_autosavecanceldelay = 5.0f;
            MenuASCancelFinished = 1;
            MenuAutoSaveCancelFirstPc = 1;
        }
        if (menu->confirm_pressed || menu->cancel_pressed) {
            MenuSFX = MENUSFX_MENUSELECT;
            MenuASCancelFinished = 1;
        }
    }

    void __cdecl saveloadASCallEachFrame(void) {
        if (SaveloadPendingPc && (i32)(GetTickCount() - SaveloadStartedTickPc) > 1000) {
            SaveloadPendingPc = 0;
            saveload_getinfo();
            saveload_status = 1;
        }
    }
}
