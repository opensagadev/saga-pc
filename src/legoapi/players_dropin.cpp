#include "saga.h"
extern "C" void __cdecl PcControlInitializePc(i32, i32, i32, u8);

extern "C" {
    i32 __cdecl FreePlay_DropInToPlayerPos(GAMEOBJECT *object);
    u8 __cdecl PcInputInstallDropInConfigurationPc(i32 port, i32 player_index);
    void __cdecl StartDropIn(GAMEOBJECT *object);
    void __cdecl DropOut(i32 player_index, i32 resume, i32 silent, i32 mode);

#pragma data_seg(".data$DropInPendingPlayerPc")
    i8 DropInPendingPlayerPc = -1;
#pragma data_seg(".data$DropInConfigurationPlayerPc")
    i8 DropInConfigurationPlayerPc = -1;
#pragma data_seg(".data$DropInCaptureDelayPc")
    u8 DropInCaptureDelayPc[2] = {5, 5};
#pragma data_seg(".data$DropInCapturePortPc")
    i16 DropInCapturePortPc[2] = {-1, -1};
#pragma data_seg(".data$DropBackInDelayPc")
    f32 DropBackInDelayPc = 2.0f;
#pragma data_seg(".data$PlayerDevicePortsPc")
    i32 PlayerDevicePortsPc[2] = {-1, -1};
#pragma data_seg()
}

STATIC_ASSERT(sizeof(DropInPendingPlayerPc) == 1);
STATIC_ASSERT(sizeof(DropInConfigurationPlayerPc) == 1);
STATIC_ASSERT(sizeof(DropInCaptureDelayPc) == 2);
STATIC_ASSERT(sizeof(DropInCapturePortPc) == 4);
STATIC_ASSERT(sizeof(PlayerDevicePortsPc) == 8);
STATIC_ASSERT(offsetof(PLAYERPROGRESS, hitpoints) == 8);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_281) == 0x281);
STATIC_ASSERT(offsetof(GAMEOBJECT, camera_screen_position) == 0xc5c);
STATIC_ASSERT(offsetof(NUVEC, y) == 4);
STATIC_ASSERT(offsetof(NUVEC, z) == 8);

extern "C" i32 __cdecl NuKeyboardBasePortPc(void) {
    return 10;
}

extern "C" i32 __cdecl PlayerDevicePortPc(i32 player_index) {
    if (player_index < 0)
        return -1;
    return PlayerDevicePortsPc[player_index];
}

extern "C" void __cdecl PlayerSetCurrentHpPc(GAMEOBJECT *object, i8 hitpoints) {
    object->current_hp = hitpoints;
    if ((i32)hitpoints > (i32)object->field_107e)
        object->current_hp = object->field_107e;
}

extern "C" void __cdecl CheckForPlayersTurnedOff(void) {
    for (i32 source_index = 0; source_index < 2; ++source_index) {
        GAMEOBJECT *source = (GAMEOBJECT *)Player[source_index];
        if (source != NULL && !(source->flags_1fc_word & 0x1000)) {
            for (i32 target_index = 2; target_index < 8; ++target_index) {
                GAMEOBJECT *target = (GAMEOBJECT *)Player[target_index];
                if (target == NULL || (target->tag_flags & 2) || (target->portal_flags & 0x4000000) ||
                    target->character_context == 0x24 || target->character_context == 0x1f)
                    continue;
                if (target->field_281 != 0 ||
                    (target->jump_variant_state == 1 && target->character_data->game_character->field_028 > 0.0f))
                    TagCharacter((GAMEOBJECT *)Player[source_index], target, 1);
            }
        }
    }
}

#define DROPIN_SCREEN_INSIDE(object)                                                                                   \
    ((object)->camera_screen_position.x > -0.85000002384185791 &&                                                      \
     (object)->camera_screen_position.x < 0.85000002384185791 &&                                                       \
     (object)->camera_screen_position.y > -0.85000002384185791 &&                                                      \
     (object)->camera_screen_position.y < 0.85000002384185791)
#define DROPIN_SCREEN_OUTSIDE(object)                                                                                  \
    ((object)->camera_screen_position.x < -1.0f || (object)->camera_screen_position.x > 1.0f ||                        \
     (object)->camera_screen_position.y < -1.0f || (object)->camera_screen_position.y > 1.0f)

extern "C" i32 __cdecl PlayersDropInOut(void) {
    i32 force_join = 0;
    i32 changed = 0;
    i32 menu;
    i32 index;
    GAMEOBJECT *object;
    CheckForPlayersTurnedOff();
    if (newgamecam)
        return 0;
    menu = GetMenuID();
    if (Paused == 0 && menu == -1 && drop_back_in_timer > 0.0f) {
        drop_back_in_timer -= FRAMETIME;
        if (drop_back_in_timer < 0.0f) {
            drop_back_in_timer = 0.0f;
            for (index = 0; index < 8; ++index) {
                object = (GAMEOBJECT *)Player[index];
                if (object == NULL || (object->flags_1fc_word & 0x80))
                    continue;
                if (object->flags_ef0 & 0x200000) {
                    if (FreePlay_DropInToPlayerPos(object)) {
                        force_join = 1;
                        break;
                    }
                    if (!(object->camera_screen_position.z < 1.0f))
                        break;
                    force_join = 1;
                    break;
                }
                if (drop_in_teleport == 3 && FreePlay_DropInToPlayerPos(object)) {
                    force_join = 1;
                    break;
                }
            }
        }
    }
    for (index = 0; index < 2; ++index) {
        object = (GAMEOBJECT *)Player[index];
        if (object == NULL)
            continue;
        i32 input_join = 0;
        i32 port0 = TextInputLinkedDevicePc(0);
        if (port0 >= 0)
            DropInCapturePortPc[0] = (i16)port0;
        i32 port = TextInputLinkedDevicePc(1);
        if (port >= 0)
            DropInCapturePortPc[1] = (i16)port;
        if (GetMenuID() == -1 && !(object->flags_1fc_word & 0x80)) {
            i32 kind;
            i32 value;
            if (DropInCaptureDelayPc[index]) {
                --DropInCaptureDelayPc[index];
                if (DropInCapturePortPc[index] >= 0) {
                    port = PcInputSelectPressedDevicePc(~(1u << DropInCapturePortPc[index]), 0x1a, &kind, &value, 1);
                    if (port >= 0)
                        DropInCaptureDelayPc[index] = 5;
                }
            } else {
                u32 excluded = 0;
                port0 = TextInputLinkedDevicePc(0);
                if (port0 >= 0)
                    excluded = 1u << port0;
                i32 port1 = TextInputLinkedDevicePc(1);
                if (port1 >= 0)
                    excluded |= 1u << port1;
                excluded &= ~(1u << NuKeyboardBasePortPc());
                port = PcInputSelectPressedDevicePc(excluded, 0x1a, &kind, &value, 1);
                if (port >= 0 && port != port0 && port != port1) {
                    DropInCapturePortPc[index] = (i16)port;
                    DropInCaptureDelayPc[index] = 5;
                    input_join = 1;
                    if (TextInputIsKeyboardDevicePc(port)) {
                        port = PcInputMapKeyboardPortPc(port, index);
                        input_join = value == (index == 0 ? 0x3b : 0x3c);
                        if (input_join && !PcInputInstallDropInConfigurationPc(port, index)) {
                            NuPadMapPlayerToPort(index, port);
                            PauseGame(index);
                            PcControlInitializePc(2, index, index, 0);
                            NewMenu(0x15, 0, -1);
                            input_join = 0;
                            DropInConfigurationPlayerPc = (i8)index;
                        }
                    }
                }
            }
        }
        if (index == DropInPendingPlayerPc) {
            input_join = 1;
            port = PlayerDevicePortPc(index);
            DropInPendingPlayerPc = -1;
        }
        if (!((input_join && MiniCutCam == 0) || drop_back_in_timer > 0.0f || force_join || index == -1))
            continue;
        if (input_join)
            NuPadMapPlayerToPort(index, port);
        if (Paused != 0 || menu != -1 || (object->flags_1fc_word & 0x80))
            continue;
        object->model_draw_result = 1;
        bool join = VehicleArea != 0 && object->character_context != 0x24;
        if (!join && CUTSTOPGAME == 0 && MiniCutCam != 2 && !(object->tag_flags & 2) &&
            !(object->portal_flags & 0x4000000) && object->character_context != 0x24 &&
            object->character_context != 0x1f) {
            if (DROPIN_SCREEN_INSIDE(object) &&
                (force_join || object->field_281 != 0 || object->id == id_SPEEDERBIKE ||
                 (object->jump_variant_state == 1 && object->character_data->game_character->field_028 > 0.0f)))
                join = true;
            else if (force_join)
                join = true;
        }
        if (!join) {
            for (i32 target_index = 2; target_index < 8; ++target_index) {
                GAMEOBJECT *target = (GAMEOBJECT *)Player[target_index];
                if (target == NULL)
                    continue;
                bool tag = VehicleArea != 0 && object->character_context != 0x24;
                if (!tag && CUTSTOPGAME == 0 && MiniCutCam != 2 && !(target->tag_flags & 2) &&
                    !(target->portal_flags & 0x4000000) && target->character_context != 0x24 &&
                    target->character_context != 0x1f && target->model_draw_result != 0 &&
                    (target->field_281 != 0 ||
                     (target->jump_variant_state == 1 && target->character_data->game_character->field_028 > 0.0f)) &&
                    DROPIN_SCREEN_INSIDE(target))
                    tag = true;
                if (tag && TagCharacter(object, target, 1)) {
                    object = target;
                    join = true;
                    break;
                }
            }
            if (!join) {
                if (VehicleArea == 0) {
                    if (drop_back_in_timer == 0.0f && !force_join)
                        drop_back_in_timer = DropBackInDelayPc;
                    object->flags_ef0 |= 0x200000;
                }
                continue;
            }
        }
        object->flags_1fc_word |= 0x80;
        object->hud_icon_timer = 2.0f;
        object->input_toggle_hold_time = TOGGLEHOLDTIME;
        NuSound3StopRumble();
        ResetTimer(&JoinInTimer, 0.0f);
        changed = 1;
        if (WORLD->current_level != VADERC_LDATA)
            PlayerSetCurrentHpPc(object, (i8)PlayerProgress[index].hitpoints);
        GizForce_ResetLOS(object);
        object->flags_ef0 &= ~0x180000;
        object->tag_reset_timer = 0.0f;
        object->pause_context_state = 0.0f;
        if (MiniCutCam != 2 && VehicleArea == 0)
            GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
        if (VehicleArea != 0 && object->character_context != 0x24)
            StartDropIn(object);
        object->flags_ef0 |= 0x80;
        GameAudio_PlaySfx(0x1f, &object->position, 0, 0);
        drop_back_in_timer = 0.0f;
        force_join = 0;
    }
    if (VehicleArea != 0 || CUTSTOPGAME != 0 || MiniCutCam != 0 ||
        (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks == 0) || menu == 0xd || menu == 0xc ||
        menu == 0xe || menu == 0xf || menu == 0x10 || menu == 0x11 || menu == 8)
        return changed;
    i32 drop_index = -1;
    for (index = 0; index < 2; ++index) {
        object = (GAMEOBJECT *)Player[index];
        if (object == NULL || object->dead != 0 || object->character_context == 0x2b ||
            object->character_context == 0x51 || object->character_context == 0x1f)
            continue;
        if (object->flags_ef0 & 0x200000) {
            if (DROPIN_SCREEN_INSIDE(object) && object->camera_screen_position.z < 1.0f) {
                for (i32 clear_index = 0; clear_index < 8; ++clear_index) {
                    if (Player[clear_index] != NULL)
                        ((GAMEOBJECT *)Player[clear_index])->flags_ef0 &= ~0x200000;
                }
                continue;
            }
            if (object->field_da8 >= 2.0f)
                continue;
            if ((object->model_draw_result != 0 || DROPIN_SCREEN_OUTSIDE(object)) && !(object->tag_flags & 2) &&
                !(object->portal_flags & 0x4000000))
                drop_back_in_timer = 0.000001f;
        } else if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc_word & 0x80) && Player[1] != NULL &&
                   (((GAMEOBJECT *)Player[1])->flags_1fc_word & 0x80)) {
            if (object->field_da8 >= 2.0f || (object->model_draw_result == 0 && DROPIN_SCREEN_OUTSIDE(object)))
                drop_index = index;
        }
    }
    if (drop_index != -1 && BonusWinner == -1) {
        drop_back_in_timer = DropBackInDelayPc;
        DropOut(drop_index, 0, 1, 0);
    }
    return changed;
}

#undef DROPIN_SCREEN_INSIDE
#undef DROPIN_SCREEN_OUTSIDE

STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_source) == 0xcd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, model_draw_result) == 0x288);
STATIC_ASSERT(offsetof(GAMEOBJECT, tag_flags) == 0x7c5);
STATIC_ASSERT(offsetof(GAMEOBJECT, last_safe_position) == 0x1b4);
STATIC_ASSERT(offsetof(GAMEOBJECT, bounds_half_height) == 0x1e0);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_player_packet_pc) == 0x6c4);
STATIC_ASSERT(offsetof(GAMEOBJECT, respawn_timer) == 0x1d8);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_0x276) == 0x27a);

extern "C" i32 __cdecl FreePlay_DropInToPlayerPos(GAMEOBJECT *object) {
    if ((WORLD == NULL || WORLD->current_level != FACTORYF_LDATA) && player != NULL && FreePlay != 0 &&
        drop_in_teleport >= 2 && VehicleArea == 0 && (Arcade != 0 || teleport_all_freeplay_modes != 0) &&
        object->takeover_source == NULL && (CInfo[(i8)object->character_context].flags & 0x8000) == 0 &&
        object->character_context != 0x2b && object->model_draw_result == 0 && (object->tag_flags & 2) == 0) {
        NUVEC position = player->last_safe_position;
        if (position.x == player->position.x && position.y == player->position.y && position.z == player->position.z) {
            position.z = (f32)((f64)position.z + (f64)0.01f);
        }
        if (NuCameraClipTestSphere(&position, object->bounds_half_height, &NuMtxSetIdentityTemplate) == 0) {
            ResetPlayerMoves(object);
            Player_ClearContext(object, 1);
            Player_ResetContexts(object->ai_player_packet_pc);
            object->start_position = position;
            object->position = position;
            object->velocity = v000;
            object->field_0x276 = player->field_0x276;
            ResetPlayerMoves(object);
            object->respawn_timer = 0.0f;
            InitSurfaceInfo(object);
            SetObjOnSurface(object, 1);
            GameObjectOrigin(object);
            AddGameDebris(WORLD->game_debris_sys, 0x5c, &object->collision_position);
            return 1;
        }
    }
    return 0;
}
