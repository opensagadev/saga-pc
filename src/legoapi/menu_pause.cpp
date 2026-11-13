#include "saga.h"

STATIC_ASSERT(sizeof(FADETYPE) == 4);
STATIC_ASSERT(sizeof(FadeSystem) == 0x2c);
STATIC_ASSERT(sizeof(GAMEPAD_INIT_RECORD) == 0x60);
extern "C" {
    void __cdecl StartDropOut(GAMEOBJECT *);
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
    /* The lean angles are signed in retail (loaded with movsx); the shared header has them as u16. */
    i32 secondary = (i16)object->secondary_lean_angle;
    i32 movement = (i16)object->movement_lean_angle;
    i32 tertiary = (i16)object->tertiary_lean_angle;
    ReleaseEat(object);
    if (VehicleArea) {
        ResetPlayerPacket(object->unknown_06c4, object->character_data);
        object->secondary_lean_angle = secondary;
        object->movement_lean_angle = movement;
        object->tertiary_lean_angle = tertiary;
    }
    object->flags_1fc_word &= ~0x80u;
    AICreatureResumeScript(object);
    if (VehicleArea)
        StartDropOut(object);
    if (object->dead || object->character_context == 0x2b)
        PlayerProgress[player].hitpoints = object->field_107e;
    else
        PlayerProgress[player].hitpoints = object->current_hp;
    object->powerup_time = 0.0f;
    if (!VehicleArea)
        GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
    else
        GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
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
