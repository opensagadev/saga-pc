#include "saga.h"

/* Retail unit 0x4277d0-0x428930: definitions in retail address order. */

void __cdecl PcControlInitializePc(i32, i32, i32, u8);
i32 __cdecl FreePlay_DropInToPlayerPos(GAMEOBJECT *object);
u8 __cdecl PcInputInstallDropInConfigurationPc(i32 port, i32 player_index);
void __cdecl StartDropIn(GAMEOBJECT *object);
STATIC_ASSERT(offsetof(PLAYERPROGRESS, hitpoints) == 8);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_281) == 0x281);
STATIC_ASSERT(offsetof(GAMEOBJECT, camera_screen_position) == 0xc5c);
STATIC_ASSERT(offsetof(NUVEC, y) == 4);
STATIC_ASSERT(offsetof(NUVEC, z) == 8);
i32 __cdecl NuKeyboardBasePortPc(void);
i32 __cdecl PlayerDevicePortPc(i32 player_index);
void __cdecl PlayerSetCurrentHpPc(GAMEOBJECT *object, i8 hitpoints);
#define DROPIN_SCREEN_INSIDE(object)                                                                                   \
    ((object)->camera_screen_position.x > -0.85000002384185791 &&                                                      \
     (object)->camera_screen_position.x < 0.85000002384185791 &&                                                       \
     (object)->camera_screen_position.y > -0.85000002384185791 &&                                                      \
     (object)->camera_screen_position.y < 0.85000002384185791)
#define DROPIN_SCREEN_OUTSIDE(object)                                                                                  \
    ((object)->camera_screen_position.x < -1.0f || (object)->camera_screen_position.x > 1.0f ||                        \
     (object)->camera_screen_position.y < -1.0f || (object)->camera_screen_position.y > 1.0f)
STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_source) == 0xcd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, model_draw_result) == 0x288);
STATIC_ASSERT(offsetof(GAMEOBJECT, tag_flags) == 0x7c5);
STATIC_ASSERT(offsetof(GAMEOBJECT, last_safe_position) == 0x1b4);
STATIC_ASSERT(offsetof(GAMEOBJECT, bounds_half_height) == 0x1e0);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_player_packet_pc) == 0x6c4);
STATIC_ASSERT(offsetof(GAMEOBJECT, respawn_timer) == 0x1d8);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_0x276) == 0x27a);

void __cdecl CheckForPlayersTurnedOff(void) {
    i32 source_index;
    for (source_index = 0; source_index < 2; ++source_index) {
        GAMEOBJECT *source = (GAMEOBJECT *)Player[source_index];
        if (source != NULL && !(source->flags_1fc_word & 0x1000)) {
            i32 target_index;
            for (target_index = 2; target_index < 8; ++target_index) {
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

/* Players whose drop-in flag (0x80) is set. */
i32 __cdecl GetNumLocalPlayers(void) {
    i32 count = 0;
    if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc_word & 0x80))
        count = 1;
    if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc_word & 0x80))
        count++;
    return count < 0 ? 0 : count;
}

i32 __cdecl FreePlay_DropInToPlayerPos(GAMEOBJECT *object) {
    if (WORLD != NULL && WORLD->current_level == FACTORYF_LDATA)
        return 0;
    if (player != NULL && FreePlay != 0 && drop_in_teleport >= 2 && VehicleArea == 0 &&
        (Arcade != 0 || teleport_all_freeplay_modes != 0) && object->takeover_source == NULL &&
        (U64_PAIR(&CInfo[(i8)object->character_context].flags) & 0x8000) == 0 && object->character_context != 0x2b &&
        object->model_draw_result == 0 && (object->tag_flags & 2) == 0) {
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

f32 DropInOutScale(GAMEOBJECT *object) {
    const u8 context = (u8)(object->character_context);
    if (context == CHARACTER_CONTEXT_DROP_IN)
        return NuTrigTable[((i32)(object->context_timer / object->context_duration * 16384.0f) >> 1) & 0x7fff];
    if (context == CHARACTER_CONTEXT_DROP_OUT)
        return NuTrigTable[((i32)(object->context_timer / object->context_duration * 16384.0f + 16384.0f) >> 1) &
                           0x7fff];
    return 1.0f;
}

void __cdecl DropInOutCode(GAMEOBJECT *object) {
    u8 context = (u8)object->character_context;
    GAMEOBJECT *other;
    if (context != 0x23 && context != 0x24)
        return;
    object->context_timer += FRAMETIME;
    if (object->context_timer >= object->context_duration) {
        if (context == 0x24) {
            other = object == Player[0] ? Player[1] : Player[0];
            object->position = other->position;
        }
        object->character_context = -1;
        if ((object->character_data->model_flags & 0x2000) == 0) {
            object->field_281 = 0;
            object->velocity.y = -0.1f;
        }
    } else if (WORLD->current_level == PODSPRINTA_LDATA) {
        other = object == Player[0] ? Player[1] : Player[0];
        object->position = other->position;
    }
}

void StartDropIn(GAMEOBJECT *object) {
    i16 animation;
    if (WORLD->current_level == PODSPRINTA_LDATA || (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA)) {
        return;
    }

    object->character_context = CHARACTER_CONTEXT_DROP_IN;
    animation = (object->character_data->model_flags & 0x2000) == 0 ? 5 : 1;
    object->context_timer = 0.0f;
    object->context_duration = 0.5f;
    object->field_7aa = animation;
}

void StartDropOut(GAMEOBJECT *object) {
    i32 animation;
    if (WORLD->current_level == PODSPRINTA_LDATA || (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA)) {
        return;
    }

    object->character_context = CHARACTER_CONTEXT_DROP_OUT;
    animation = CurrentAnim(&object->anim_packet);
    object->context_timer = 0.0f;
    object->context_duration = 0.5f;
    object->field_7aa = animation;
}

void __cdecl DropOut(i32 player, i32 resume, i32 silent, i32 unused) {
    GAMEOBJECT *object = (GAMEOBJECT *)Player[player];
    i32 secondary;
    i32 movement;
    i32 tertiary;
    GAMEOBJECT *other;
    if (!object)
        return;
    if (!silent) {
        if (!object->rider)
            NuPadSetStatus((GAMEPAD_INIT_RECORD *)object->pad - GamePadStorage, 0);
        GameAudio_PlaySfx(0x20, &object->collision_position, 0, 0);
    }
    ReleaseTakeOver(object, 1);
    /* The lean angles are signed in retail (loaded with movsx); the shared header has them as u16. */
    secondary = (i16)object->secondary_lean_angle;
    movement = (i16)object->movement_lean_angle;
    tertiary = (i16)object->tertiary_lean_angle;
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
    other = (GAMEOBJECT *)Player[player == 0];
    if (other) {
        NUVEC direction;
        NuVecSub(&direction, &other->collision_position, &GameCam->position);
        if (NewRayCastScaleYMask(&GameCam->position, &direction, 0.0f, 1.0f, 0, 0x1f))
            GameCam_Reset(GameCam);
    }
}
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
STATIC_ASSERT(sizeof(DropInPendingPlayerPc) == 1);
STATIC_ASSERT(sizeof(DropInConfigurationPlayerPc) == 1);
STATIC_ASSERT(sizeof(DropInCaptureDelayPc) == 2);
STATIC_ASSERT(sizeof(DropInCapturePortPc) == 4);
STATIC_ASSERT(sizeof(PlayerDevicePortsPc) == 8);

i32 __cdecl PlayersDropInOut(void) {
    i32 force_join = 0;
    i32 changed = 0;
    i32 menu;
    i32 index;
    GAMEOBJECT *object;
    i32 drop_index;
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
                    if (FreePlay_DropInToPlayerPos(object) ||
                        ((GAMEOBJECT *)Player[index])->camera_screen_position.z < 1.0f)
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
        i32 input_join;
        i32 port0;
        i32 port;
        u8 join;
        object = (GAMEOBJECT *)Player[index];
        if (object == NULL)
            continue;
        input_join = 0;
        port0 = NuPadGetPort(0);
        if (port0 >= 0)
            DropInCapturePortPc[0] = (i16)port0;
        port = NuPadGetPort(1);
        if (port >= 0)
            DropInCapturePortPc[1] = (i16)port;
        if (GetMenuID() == -1) {
            i32 kind;
            i32 value;
            i32 off = (object->flags_1fc_word >> 7) & 1;
            if (off == 0 && DropInCaptureDelayPc[index]) {
                --DropInCaptureDelayPc[index];
                if (DropInCapturePortPc[index] >= 0) {
                    port = PcInputSelectPressedDevicePc(~(1u << DropInCapturePortPc[index]), 0x1a, &kind, &value, 1);
                    if (port >= 0)
                        DropInCaptureDelayPc[index] = 5;
                }
            } else if (off == 0) {
                u32 excluded = 0;
                i32 port1;
                port0 = NuPadGetPort(0);
                if (port0 >= 0)
                    excluded = 1u << port0;
                port1 = NuPadGetPort(1);
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
                        input_join = index != 0 ? value == 0x3c : value == 0x3b;
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
        join = VehicleArea != 0 && object->character_context != 0x24;
        if (!join && CUTSTOPGAME == 0 && MiniCutCam != 2 && !(object->tag_flags & 2) &&
            !(object->portal_flags & 0x4000000) && object->character_context != 0x24 &&
            object->character_context != 0x1f) {
            if (DROPIN_SCREEN_INSIDE(object) &&
                (force_join || object->field_281 != 0 || object->id == (u16)id_SPEEDERBIKE ||
                 (object->jump_variant_state == 1 && object->character_data->game_character->field_028 > 0.0f)))
                join = 1;
            else if (force_join)
                join = 1;
        }
        if (!join) {
            i32 target_index;
            for (target_index = 2; target_index < 8; ++target_index) {
                GAMEOBJECT *target = (GAMEOBJECT *)Player[target_index];
                u8 tag;
                if (target == NULL)
                    continue;
                tag = VehicleArea != 0 && object->character_context != 0x24;
                if (!tag && CUTSTOPGAME == 0 && MiniCutCam != 2 && !(target->tag_flags & 2) &&
                    !(target->portal_flags & 0x4000000) && target->character_context != 0x24 &&
                    target->character_context != 0x1f && target->model_draw_result != 0 &&
                    (target->field_281 != 0 ||
                     (target->jump_variant_state == 1 && target->character_data->game_character->field_028 > 0.0f)) &&
                    DROPIN_SCREEN_INSIDE(target))
                    tag = 1;
                if (tag && TagCharacter(object, target, 1)) {
                    object = target;
                    join = 1;
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
        GameAudio_PlaySfx(0x1f, &object->collision_position, 0, 0);
        drop_back_in_timer = 0.0f;
        force_join = 0;
    }
    if (VehicleArea != 0 || CUTSTOPGAME != 0 || MiniCutCam != 0 ||
        (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks == 0) || menu == 0xd || menu == 0xc ||
        menu == 0xe || menu == 0xf || menu == 0x10 || menu == 0x11 || menu == 8)
        return changed;
    drop_index = -1;
    for (index = 0; index < 2; ++index) {
        object = (GAMEOBJECT *)Player[index];
        if (object == NULL || object->dead != 0 || object->character_context == 0x2b ||
            object->character_context == 0x51 || object->character_context == 0x1f)
            continue;
        if (object->flags_ef0 & 0x200000) {
            if (DROPIN_SCREEN_INSIDE(object) && object->camera_screen_position.z < 1.0f) {
                i32 clear_index;
                for (clear_index = 0; clear_index < 8; ++clear_index) {
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
