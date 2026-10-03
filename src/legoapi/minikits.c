#include "saga.h"

void __cdecl ResetMinikitCounter(void) {
    minikitCounter_C = 0;
    minikitCounter_A = 0;
}

void __cdecl MiniKit_GameMsg_Update(GAME_MESSAGE_PC *message) {
    i32 angle = (i32)((f64)NuFmod(GlobalTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
    message->parameter_e2 = (u16)angle;
    message->parameter_e0 = (u16)(i32)((f64)NuTrigTable[(u16)angle & 0x7fff] * 1820.0);
}

void __cdecl MiniKit_GameMsg_End(GAME_MESSAGE_PC *message) {
    MiniKitScale = 2.0f;
    if (WorldInfo_CurrentlyActive()->area != NULL && (i16)message->icon != -1) {
        if ((i16)message->icon == MiniKitModelId_PC) {
            ++AreaGlobals_PC.progress_flags;
            if (AreaGlobals_PC.progress_flags > AreaGlobals_PC.area_phase)
                AreaGlobals_PC.progress_flags = AreaGlobals_PC.area_phase;
        } else if ((i16)message->icon == CharKitModelId_PC && AreaGlobals_PC.field_20 < AreaGlobals_PC.field_1c) {
            ++AreaGlobals_PC.field_20;
            if (AreaGlobals_PC.field_20 == AreaGlobals_PC.field_1c && AreaGlobals_PC.field_1c >= 10)
                EndChallenge(2, 1);
        }
    }
    GameAudio_PlaySfx(0x26, NULL, 0, 0);
    NewRumbleAllPlayers(0.6f, 0.0f, 0, 0.0f);
    GameCam_Judder(GameCam, -0.2f, 0, NULL);
}

void __cdecl CollectMinikit(NUVEC *position, char *name, i32 flags) {
    ADD_GAME_MESSAGE_PC message;
    NUVEC target;
    i32 area_id;
    (void)flags;
    if (WORLD->level_objects[0xce].active == 0)
        return;
    area_id = WORLD->area_index;
    target.x = KITPOSX;
    target.y = KITPOSY + PANEL_MINIKITY;
    target.z = 1.0f;
    message = AddGameMsg_Default;
    message.position = position;
    message.target_position = &target;
    message.scale = AreaPickupScale;
    message.target_scale = PANEL_MINIKITSCALE;
    message.flags = 0x2112d;
    message.duration = 1.0f;
    message.icon = 0xce;
    message.special = &WORLD->level_objects[0xce].special;
    message.tick_fn = MiniKit_GameMsg_Update;
    message.end_fn = MiniKit_GameMsg_End;
    message.allow_temporary = 1;
    AddGameMsg(&message);
    DrawMiniKitTime = 2.0f;
    if (area_id != -1 && Game.area_save[area_id].minikit_count < 10) {
        if (NuStrLen(name) <= 8)
            NuStrCpy(NewMiniPiece[AreaGlobals_PC.new_minikit_count].name, name);
        else
            NewMiniPiece[AreaGlobals_PC.new_minikit_count].name[0] = 0;
        NewMiniPiece[AreaGlobals_PC.new_minikit_count].level = (i16)WORLD->level_idx;
        ++AreaGlobals_PC.new_minikit_count;
        ++AreaGlobals_PC.area_phase;
        if (GAMEDEMO != 0) {
            if (AreaGlobals_PC.area_phase == 1)
                Hint_SetHintFromId(0x26d, 0, 0);
            else if (AreaGlobals_PC.area_phase == 10)
                Hint_SetHintFromId(0x26e, 0, 0);
        }
    }
    AddGameDebris(WORLD->game_debris_sys, 0x13, position);
    GameAudio_PlaySfxById(GetSfxId(PickupTypeLiteral758c14), position, 3, 0);
}
