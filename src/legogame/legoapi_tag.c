#include "saga.h"
#include <string.h>

/* Retail unit 0x486f20-0x488e82: definitions in retail address order. */

extern i32 __cdecl TagCode(GAMEOBJECT *, GAMEOBJECT *, i32, i32, i32);
extern GAMEOBJECT *player_tag_from;
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, tag_flags) == 0x7c5);
STATIC_ASSERT(offsetof(GAMEOBJECT, tag_cooldown) == 0x7c0);
STATIC_ASSERT(offsetof(GAMEOBJECT, pending_tag_target) == 0x7c8);
STATIC_ASSERT(sizeof(Player) == 32);
#define TAG_CAN_TARGET(source, target)                                                                                 \
    ((target) != NULL && ((target)->flags_1fc_word & 1) != 0 && ((target)->flags_1fc_word & 0x1000) != 0 &&            \
     (target) != (source) && (target)->dead == 0 && ((target)->flags_1fc_word & 0x80) == 0 &&                          \
     ((target)->tag_flags & 2) == 0)
void __cdecl GameObjectOrigin(GAMEOBJECT *object);
/* A copy of the actual 33-DWORD script cohort (script processor plus takeover link), without extending the
   packet. Held as words so the copy is one block move and the local is not treated as a /GS buffer. */
typedef union TAGSCRIPTSTATE_PC {
    u32 words[0x33];
    struct {
        u32 unknown000[0x2c];
        u8 creature_set;
    } fields;
} TAGSCRIPTSTATE_PC;
#define TAG_SCRIPT_STATE(object) (*(TAGSCRIPTSTATE_PC *)&(object)->ai_packet.script_processor)
/* Bitfield view of flags_1fc: bit 7 marks a player-controlled object, bit 8 an AI-driven one. */
typedef struct TAGFLAGS1FC_PC {
    u32 bits_0_6 : 7;
    u32 player : 1;
    u32 ai : 1;
    u32 bits_9_31 : 23;
} TAGFLAGS1FC_PC;
#define TAG_FLAGS(object) ((TAGFLAGS1FC_PC *)&(object)->flags_1fc_word)
/* Defined in render.c, the TU that owns 0x486f20; an in-TU empty body lets the call be dropped. */
void __cdecl TakeOverReleaseDiagnosticPc(char *rider_name, char *object_name);
void __cdecl Tag_NewTransfer(GAMEOBJECT *source, GAMEOBJECT *target);

void __cdecl Tag_SetMode(i32 mode) {
    Tag_Mode = mode;
}

i32 __cdecl Tag_FindGameObject_CYCLICAL(GAMEOBJECT *object) {
    i32 remaining = HIGHGAMEOBJECT - 1;
    i32 index = object->hostility_group;
    GAMEOBJECT *candidate;
    while (remaining > 0) {
        if (++index == HIGHGAMEOBJECT) {
            index = 0;
        }
        --remaining;
        candidate = &Obj[index];
        if (candidate != NULL && candidate != object && (candidate->flags_1fc_word & 1) != 0 &&
            (candidate->flags_1fc_word & 0x1000) != 0 && candidate->dead == 0 && candidate->field_280 != 0xff &&
            (candidate->tag_flags & 2) == 0 && (candidate->flags_ef4 & 2) == 0 &&
            (U64_PAIR(&CInfo[candidate->character_context].flags) & 0x800) == 0) {
            return (i8)candidate->field_280;
        }
    }
    return -1;
}

static GAMEOBJECT *Tag_FindGameObject_TRANSFER(GAMEOBJECT *object) {
    f32 nearest_distance = object->character_context == 0x17 ? 1.44f : 0.48999998f;
    GAMEOBJECT *nearest = NULL;
    i32 count = Tag_Mode == 3 ? HIGHGAMEOBJECT : 8;
    i32 index;
    for (index = 0; index < count; ++index) {
        GAMEOBJECT *candidate = Tag_Mode == 3 ? &Obj[index] : Player[index];
        NUVEC direction;
        f32 distance;
        if (candidate == NULL || (candidate->flags_1fc_word & 1) == 0 || (candidate->flags_1fc_word & 0x1000) == 0 ||
            candidate == object || candidate->dead != 0 || (candidate->tag_flags & 2) != 0) {
            continue;
        }
        if (candidate->character_context == 0x17 || candidate->character_context == 0x3d ||
            (U64_PAIR(&CInfo[candidate->character_context].flags) & 0x8000) != 0 ||
            ((candidate->flags_ef4 & 2) != 0 && object->id != (u16)id_LUKESKYWALKERDAGOBAH)) {
            continue;
        }
        distance = NuVecDistSqr(&object->position, &candidate->position, &direction);
        if (distance < nearest_distance) {
            NuVecRotateY(&direction, &direction, -(i32)object->field_0x276);
            if (direction.z < 0.0f) {
                nearest_distance = distance;
                nearest = candidate;
            }
        }
    }
    return nearest;
}

void __cdecl GetTakeOverPos(GAMEOBJECT *object, NUVEC *position) {
    i32 locator = object->character_data->game_character->ride_locator;
    CHARACTER_MODEL_LAYERS_PC *model = (CHARACTER_MODEL_LAYERS_PC *)object->character_model;
    if (locator != -1 && model->points_of_interest[locator] != NULL) {
        *position = object->locator_matrices[locator].translation;
    } else {
        position->x = object->position.x;
        position->y = object->upper_position.y;
        position->z = object->position.z;
    }
}

/* Appended after tag-draft.c and get-pos.c in the original-TU scratch group. */
static void SetRiderPos(GAMEOBJECT *object, NUVEC *position) {
    if (object->rider != NULL) {
        if (position != NULL) {
            object->position = *position;
        } else {
            GetTakeOverPos(object->rider, &object->position);
        }
        object->pitch_angle = 0;
        object->roll_angle = 0;
        object->field_0x276 = object->rider->facing_angle;
        object->facing_angle = object->rider->facing_angle;
        object->movement_facing_angle = object->rider->facing_angle;
    }
    GameObjectOrigin(object);
}

static void TakeOver_SetAction(GAMEOBJECT *rider, GAMEOBJECT *vehicle) {
    i32 action;
    if (vehicle->id == GameObjectRotationBuggyId) {
        action = 0x70;
    } else if (vehicle->id == (u16)id_GYROCOPTER) {
        action = 0x83;
    } else if (StartupGameFlag1 == 0) {
        goto no_action;
    } else if (vehicle->character_data->game_character->weapon_action == 12) {
        if (CurrentAnim(&vehicle->anim_packet) == 3 || CurrentAnim(&vehicle->anim_packet) == 0x17)
            action = 0xc4;
        else
            action = 0xc3;
    } else if (vehicle->id == (u16)id_BANTHA || vehicle->id == (u16)id_BOMARRMONK) {
        action = 0x70;
    } else if (vehicle->id == (u16)id_DEWBACK) {
        action = 0x83;
    } else if (vehicle->id == (u16)id_LANDSPEEDER || vehicle->id == (u16)id_FLASHSPEEDER) {
        action = 0x84;
    } else if (vehicle->id == (u16)id_TAUNTAUN) {
        action = 0x86;
    } else if (vehicle->id == (u16)id_SPEEDERBIKE) {
        action = 0xc0;
    } else if (vehicle->id == (u16)id_HEAVYREPEATINGCANNON || vehicle->id == (u16)id_BIGGUN) {
        action = 0xc1;
    } else if (vehicle->id == (u16)id_TROOPERCANNON) {
        action = 0xc2;
        if (((CHARACTER_MODEL_LAYERS_PC *)rider->character_model)->action_animations[0xc2] == NULL)
            action = 0xc1;
    } else if (vehicle->id == (u16)id_STAP2) {
        action = 0xc1;
    } else if (vehicle->id == (u16)id_CLONEWALKER) {
        action = 0xc0;
    } else {
        goto no_action;
    }
    if (((CHARACTER_MODEL_LAYERS_PC *)rider->character_model)->action_animations[action] != NULL) {
        rider->field_7aa = (i16)action;
        return;
    }
no_action:
    rider->field_7aa = 0x6c;
}

i32 __cdecl TakeOverYoda(GAMEOBJECT *rider, GAMEOBJECT *vehicle, i32 blend_camera, i32 unused) {
    (void)unused;
    ReleaseForce(rider, 0);
    ReleaseBuildIt(rider, 0);
    ReleasePush(rider);
    Player_ResetContexts(rider->ai_player_packet_pc);
    if ((rider->portal_flags & 0x10) == 0) {
        SetWeaponIn(rider);
    }
    rider->rider = vehicle;
    vehicle->rider = rider;
    rider->takeover_target = NULL;
    vehicle->takeover_target = NULL;
    rider->takeover_owner = vehicle;
    rider->character_context = 0x3b;
    TakeOver_SetAction(rider, rider->rider);
    rider->takeover_timer = 1.0f;
    rider->external_force = rider->position;
    if (blend_camera != 0) {
        GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
    }
    NewBuzz(vehicle->pad->bound_pad, 0.1f, 0);
    GameCam_Judder(GameCam, 0.1f, 0, NULL);
    Hint_SetComplete(0x28c);
    return 1;
}

void __cdecl TakeOver2GetIn(GAMEOBJECT *rider, GAMEOBJECT *vehicle) {
    i32 i;
    GAMEOBJECT *object = Obj;
    i32 count = HIGHGAMEOBJECT;
    CHARACTER_MODEL_LAYERS_PC *model;
    for (i = 0; i < count; ++i, ++object) {
        if ((object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 &&
            object->character_context == 0x3c && object->force_target == vehicle) {
            return;
        }
    }
    if (rider == NULL || (rider->flags_1fc_word & 1) == 0 || vehicle == NULL || (vehicle->flags_1fc_word & 1) == 0 ||
        rider->rider != NULL || vehicle->rider != NULL) {
        return;
    }
    rider->character_context = 0x3c;
    model = (CHARACTER_MODEL_LAYERS_PC *)rider->character_model;
    if (model->action_animations[6] != NULL) {
        ResetAnimPacket(&rider->anim_packet, -1);
        rider->field_7aa = 6;
        rider->context_duration = AnimDuration((i16)rider->id, 6, 0.0f, 0.0f, 1);
    } else {
        rider->field_7aa = 5;
        rider->context_duration = 0.5f;
    }
    if (vehicle->id == (u16)id_ATAT && WORLD->current_level == SPEEDERCHASEA_LDATA) {
        NUVEC position;
        GetTakeOverPos(vehicle, &position);
        rider->context_duration *= 2.0f * SpeederChaseATATInOutMul(&rider->position, &position);
    }
    rider->context_timer = 0.0f;
    rider->external_force = rider->position;
    rider->takeover_start_angle = rider->field_0x276;
    rider->force_target = vehicle;
    rider->takeover_target = NULL;
    vehicle->takeover_target = NULL;
    PlayJumpSfx(rider, 0);
}

i32 __cdecl TagCode(GAMEOBJECT *source, GAMEOBJECT *destination, i32 takeover, i32 blend_camera, i32 unused) {
    GAMEPAD_PC *source_pad;
    GAMEPAD_PC *destination_pad;
    f32 source_da8;
    i32 source_hp, destination_hp;
    u32 source_player, destination_player;
    TAGSCRIPTSTATE_PC source_script, destination_script;
    u32 source_ai, destination_ai;
    void *pointer;
    i32 player_index;
    f32 timer;
    u32 source_flags, saved_flags, mask;
    void(__cdecl * callback)(GAMEOBJECT *);
    (void)unused;
    if (CUTSKIPLOCK == 0) {
        source_pad = source->pad;
        destination_pad = destination->pad;
        source_da8 = source->field_da8;
        source_hp = source->current_hp;
        destination_hp = destination->current_hp;
        source_player = TAG_FLAGS(source)->player;
        destination_player = TAG_FLAGS(destination)->player;
        source_ai = 0;
        destination_ai = 0;
        if (takeover != 0) {
            source_script = TAG_SCRIPT_STATE(source);
            destination_script = TAG_SCRIPT_STATE(destination);
            source_ai = TAG_FLAGS(source)->ai;
            destination_ai = TAG_FLAGS(destination)->ai;
        }
        ResetForceGlow(source->ai_player_packet_pc);
        ResetForceGlow(destination->ai_player_packet_pc);
        destination->pad = source_pad;
        source->pad = destination_pad;
        TAG_FLAGS(source)->player = destination_player;
        TAG_FLAGS(destination)->player = source_player;
        source->field_da8 = destination->field_da8;
        destination->field_da8 = source_da8;
        pointer = source->coin_packet;
        source->coin_packet = destination->coin_packet;
        destination->coin_packet = pointer;
        player_index = (i8)source->field_280;
        source->field_280 = destination->field_280;
        destination->field_280 = player_index;
        timer = source->powerup_time;
        source->powerup_time = destination->powerup_time;
        destination->powerup_time = timer;
        pointer = source->gizforce_los_info;
        source->gizforce_los_info = destination->gizforce_los_info;
        destination->gizforce_los_info = pointer;
        GizForce_ResetLOS(source);
        GizForce_ResetLOS(destination);
        source_flags = source->flags_1f8;
        mask = takeover != 0 ? 0x146fd : 0x46f8;
        saved_flags = source_flags & mask;
        source->flags_1f8 &= ~mask;
        source->flags_1f8 |= destination->flags_1f8 & mask;
        destination->flags_1f8 = (destination->flags_1f8 & ~mask) | (saved_flags & mask);
        callback = source->tag_object_callback;
        source->tag_object_callback = destination->tag_object_callback;
        destination->tag_object_callback = callback;
        timer = source->spawn_protection_timer;
        source->spawn_protection_timer = destination->spawn_protection_timer;
        destination->spawn_protection_timer = timer;
        AICreatureResumeScript(source);
        if (takeover != 0) {
            if (source->field_280 != 0xff) {
                Player[(i8)source->field_280] = source;
            }
            if (destination->field_280 != 0xff) {
                Player[(i8)destination->field_280] = destination;
            }
        } else {
            source->hud_icon_timer = 0.0f;
            source->input_toggle_hold_time = TOGGLEHOLDTIME;
            source->flags_ef0 = (source->flags_ef0 & ~0x180000u) | 0x80;
            source->tag_reset_timer = 0.0f;
            source->pause_context_state = 0.0f;
            destination->hud_icon_timer = 2.0f;
            destination->input_toggle_hold_time = TOGGLEHOLDTIME;
            destination->flags_ef0 |= 0x80;
            NewBuzzFrames(destination->pad->bound_pad, 2, 0);
            if (source->field_280 != 0xff) {
                Player[(i8)source->field_280] = source;
                if (source->rider == NULL) {
                    if (WORLD->current_level != VADERC_LDATA)
                        source->field_107e = DEFAULT_PLAYERHITPOINTS;
                } else {
                    source->field_107e = source->character_data->game_character->hitpoints;
                }
            } else {
                source->field_107e = source->character_data->game_character->hitpoints;
            }
            if (destination->field_280 != 0xff) {
                Player[(i8)destination->field_280] = destination;
                if (destination->rider == NULL) {
                    if (WORLD->current_level != VADERC_LDATA)
                        destination->field_107e = DEFAULT_PLAYERHITPOINTS;
                } else {
                    destination->field_107e = destination->character_data->game_character->hitpoints;
                }
            } else {
                destination->field_107e = destination->character_data->game_character->hitpoints;
            }
            destination->current_hp = (i8)source_hp;
            if (destination->current_hp > destination->field_107e) {
                destination->current_hp = (i8)destination->field_107e;
            }
            if (destination->current_hp == 0 && destination->field_107e > 0) {
                destination->current_hp = 1;
            }
            if (WORLD->current_level == VADERC_LDATA || (source->flags_1fc_word & 0x80) != 0) {
                source->current_hp = destination_hp;
            } else if ((source->character_data->model_flags & 0x20) != 0) {
                source->current_hp = (i8)source->saved_hitpoints;
            } else {
                source->current_hp = (i8)source->field_107e;
            }
        }
        destination->flags_ef0 &= ~0x180000u;
        destination->tag_reset_timer = 0.0f;
        destination->pause_context_state = 0.0f;
        destination->tag_cooldown = 1.0f;
        if (source == player_tag_to && destination != player_tag_from) {
            player_tag_to = NULL;
            player_tag_timer = 0.0f;
            player_tag_from = NULL;
            do_player_tag = 0;
        }
        if (takeover != 0) {
            if (AIScriptSetBaseScriptStateByName(&destination->ai_packet.script_processor,
                                                 (char *)TakeOverState_JustBeenTakenOver) != 0) {
                AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)destination, &destination->ai_packet,
                                &destination->ai_packet.script_processor, FRAMETIME);
            }
            TAG_SCRIPT_STATE(source) = destination_script;
            TAG_SCRIPT_STATE(destination) = source_script;
            source->ai_packet.creature_set = source_script.fields.creature_set;
            destination->ai_packet.creature_set = destination_script.fields.creature_set;
            AICreatureResumeScript(source);
            TAG_FLAGS(source)->ai = destination_ai;
            TAG_FLAGS(destination)->ai = source_ai;
            ReleaseForce(source, 0);
            ReleaseBuildIt(source, 0);
            ReleasePush(source);
            Player_ResetContexts(source->ai_player_packet_pc);
            if ((source->portal_flags & 0x10) == 0) {
                SetWeaponIn(source);
            }
            source->rider = destination;
            destination->rider = source;
            source->takeover_target = NULL;
            destination->takeover_target = NULL;
            source->takeover_owner = destination;
            source->character_context = 0x3b;
            TakeOver_SetAction(source, source->rider);
            source->takeover_timer = 1.0f;
            source->external_force = source->position;
            if (blend_camera != 0) {
                GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
            }
            if (destination->character_data->game_character->field_028 > 0.0f) {
                destination->velocity.y -= (f64)source->character_data->game_character->jump_speed * 0.5;
            }
        }
        return 1;
    }
    return 0;
}

i32 __cdecl TagCharacter(GAMEOBJECT *source, GAMEOBJECT *target, i32 mode) {
    i32 index;
    u32 source_active, target_active;
    i32 result;
    if (!TAG_CAN_TARGET(source, target)) {
        for (index = 0; index < 8; ++index) {
            target = (GAMEOBJECT *)Player[index];
            if (TAG_CAN_TARGET(source, target))
                goto found;
        }
        return 0;
    }
found:
    target_active = target->player_controlled;
    source_active = source->player_controlled;
    result = TagCode(source, target, 0, 0, mode);
    if (result == 2) {
        source->tag_flags = (source->tag_flags & ~8U) | 4;
        source->pending_tag_target = target;
        source->tag_cooldown = 3.0f;
        return 0;
    }
    if (result == 0)
        return 0;
    target->player_controlled = source_active;
    source->player_controlled = target_active;
    return 1;
}

void __cdecl Tag_Check(GAMEOBJECT *object) {
    GAMEOBJECT *target;
    i32 result;
    if (Arcade != 0 || (object->tag_flags & 1) != 0 || object->character_context == 0x51) {
        if ((object->tag_flags & 1) != 0 || object->character_context == 0x51) {
            object->hud_icon_timer = 0.0f;
        }
        object->flags_ef0 &= ~0x180000u;
        object->tag_reset_timer = 0.0f;
        object->pause_context_state = 0.0f;
        if (Arcade == 0) {
            return;
        }
    }
    if (object->field_280 == 0xff) {
        return;
    }
    if ((object->tag_flags & 4) == 0) {
        u32 flags;
        object->tag_player_index = -1;
        flags = object->flags_1fc_word;
        if ((flags & 0x80) != 0 && object->dead == 0) {
            i8 context = object->character_context;
            u64 context_flags = U64_PAIR(&CInfo[context].flags);
            if ((context_flags & 0x800) == 0 && (flags & 0x100) == 0 && object->tag_cooldown <= 0.0f) {
                if (VehicleArea != 0) {
                    if ((object->pad->buttons_pressed & GAMEPAD_TAG) != 0) {
                        object->hud_icon_timer = 2.0f;
                    }
                } else if ((object->pad->buttons_pressed & GAMEPAD_TAG) != 0) {
                    if (CUTSKIPLOCK != 0) {
                        object->hud_icon_timer = 2.0f;
                        object->input_toggle_hold_time = TOGGLEHOLDTIME;
                    } else if (Arcade == 0) {
                        target = NULL;
                        if (Tag_Mode == 3) {
                            if (context == -1 || context == 0x17 || (context_flags & 4) != 0 ||
                                (context_flags & 0x1000000000ull) != 0) {
                                target = Tag_FindGameObject_TRANSFER(object);
                                if (target != NULL && InCollectList_Index((i16)target->id, NULL, 0) == -1) {
                                    GameAudio_PlaySfx(0x32, &object->collision_position, 0, 0);
                                    NewRumble(object->pad->bound_pad, 0.5f, 0.0f);
                                    if (AIScriptSetBaseScriptStateByName(&target->ai_packet.script_processor,
                                                                         (char *)TakeOverState_MapRunAway) != 0) {
                                        AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)target, &target->ai_packet,
                                                        &target->ai_packet.script_processor, FRAMETIME);
                                    }
                                    target = NULL;
                                }
                            }
                        } else {
                            if (Tag_Mode == 1) {
                                if (context == -1 || context == 0x17 || (context_flags & 4) != 0 ||
                                    (context_flags & 0x1000000000ull) != 0) {
                                    target = Tag_FindGameObject_TRANSFER(object);
                                    if (target != NULL && target->field_280 != 0xff) {
                                        object->tag_player_index = (i8)target->field_280;
                                    }
                                }
                            } else {
                                object->tag_player_index = (i8)Tag_FindGameObject_CYCLICAL(object);
                            }
                            if (object->tag_player_index >= 0) {
                                target = Player[object->tag_player_index];
                            } else {
                                target = NULL;
                            }
                        }
                        if (target != NULL) {
                            if ((target->flags_ef4 & 2) != 0) {
                                TakeOver2GetIn(target, object);
                                goto draw_icon;
                            }
                            if ((object->flags_1fc_word & 0x80) != 0 && (target->flags_1fc_word & 0x80) != 0) {
                                if (player_tag_timer > 0.0f && player_tag_to == object && player_tag_from == target) {
                                    do_player_tag = 1;
                                } else {
                                    player_tag_to = target;
                                    player_tag_timer = 0.5f;
                                    player_tag_from = object;
                                }
                                goto draw_icon;
                            }
                            result = TagCode(object, target, 0, 0, 1);
                            if (result == 2) {
                                object->tag_cooldown = 1.0f;
                                object->pending_tag_target = target;
                                object->tag_flags = (object->tag_flags & ~8u) | 4;
                                goto draw_icon;
                            }
                            goto tagged_result;
                        }
                    }
                    if (object->rider == NULL) {
                        object->hud_icon_timer = 2.0f;
                    }
                }
            }
        }
        goto draw_icon;
    } else {
        object->tag_cooldown -= FRAMETIME;
        if (object->tag_cooldown > 0.0f && (target = object->pending_tag_target) != NULL) {
            if ((target->flags_1fc_word & 0x1000) != 0 && (target->flags_1fc_word & 1) != 0) {
                result = (object->flags_ef4 & 2) != 0 ? TakeOverYoda(object, target, 0, 1)
                                                      : TagCode(object, target, (object->tag_flags >> 3) & 1, 0, 1);
                if (result != 0) {
                    goto tagged_result;
                }
            } else {
                object->tag_flags &= ~4u;
                object->pending_tag_target = NULL;
                goto draw_icon;
            }
        }
        object->tag_flags &= ~4u;
        goto draw_icon;
    }
tagged_result:
    if (result == 1) {
        object->tag_flags &= ~4u;
        GameAudio_PlaySfx(0x21, &object->collision_position, 0, 0);
        if (target->model_draw_result != 0) {
            GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
        } else {
            GameCam_Reset(GameCam);
        }
        RememberPlayerIDs(0, Player[0] != NULL ? (i16)((GAMEOBJECT *)Player[0])->id : -1,
                          Player[1] != NULL ? (i16)((GAMEOBJECT *)Player[1])->id : -1);
        if (Tag_Mode == 1 || Tag_Mode == 3) {
            Tag_NewTransfer(object, target);
        }
        if ((object->field_280 == 0 || object->field_280 == 1) && (target->field_280 == 0 || target->field_280 == 1)) {
            ResetTimer(&JoinInTimer, 0.0f);
        }
    }
draw_icon:
    if (Tag_DrawIconFn != NULL) {
        Tag_DrawIconFn(object);
    }
}

void __cdecl ReleaseTakeOver(GAMEOBJECT *target, i32 unused) {
    GAMEOBJECT *rider;
    GAMEOBJECT *object;
    TAGSCRIPTSTATE_PC rider_script, object_script;
    u32 rider_flag, object_flag;
    PLAYERCHARACTERCONFIG_PC *character;
    (void)unused;
    if (target->rider == NULL) {
        return;
    }
    if (target->character_context == 0x3b) {
        rider = target;
        object = target->rider;
    } else {
        rider = target->rider;
        object = target;
    }
    rider_script = TAG_SCRIPT_STATE(rider);
    object_script = TAG_SCRIPT_STATE(object);
    rider_flag = TAG_FLAGS(rider)->ai;
    object_flag = TAG_FLAGS(object)->ai;
    if ((rider->flags_ef4 & 2) == 0 && TagCode(rider, object, 1, 0, 0) == 0) {
        return;
    }
    TakeOverReleaseDiagnosticPc(rider->character_data->name, object->character_data->name);
    TAG_SCRIPT_STATE(rider) = object_script;
    TAG_SCRIPT_STATE(object) = rider_script;
    rider->ai_packet.creature_set = rider_script.fields.creature_set;
    object->ai_packet.creature_set = object_script.fields.creature_set;
    AICreatureResumeScript(object);
    TAG_FLAGS(rider)->ai = object_flag;
    TAG_FLAGS(object)->ai = rider_flag;
    if (rider->rider != NULL) {
        GetTakeOverPos(rider->rider, &rider->position);
        rider->pitch_angle = 0;
        rider->roll_angle = 0;
        rider->field_0x276 = rider->rider->facing_angle;
        rider->facing_angle = rider->rider->facing_angle;
        rider->movement_facing_angle = rider->rider->facing_angle;
    }
    GameObjectOrigin(rider);
    rider->rider = NULL;
    rider->character_context = -1;
    if (object->id == (u16)id_ATAT && WORLD->current_level == SPEEDERCHASEA_LDATA) {
        f32 multiplier = SpeederChaseATATInOutMul(&rider->position, &SpeederChaseATATExitLandPos);
        f32 duration = SpeederChaseATATExitHeight * multiplier;
        f32 speed = SpeederChaseATATExitSpeed * multiplier;
        StartBigJump(rider, &SpeederChaseATATExitLandPos, 0, speed, duration, 0);
        GameCam_Blend(GameCam, 2.0f, 0.0f, 1);
    } else {
        Buck_StartRiderJump(rider, object);
    }
    character = object->character_data->game_character;
    if (character->field_028 > 0.0f) {
        object->velocity.y -= (f64)rider->velocity.y * 0.5;
    } else if ((character->flags_094 & 0x20000) != 0) {
        Buck_Start(object, rider);
    }
    object->rider = NULL;
    rider->flags_e20 &= 0x7fffffff;
    object->flags_e20 &= 0x7fffffff;
    rider->ai_packet.path_info.flags_word &= ~1u;
    object->ai_update_position = object->position;
    if ((i8)rider->field_280 != -1 || (i8)object->field_280 != -1) {
        GameCam_Blend(GameCam, 0.3f, 0.0f, 1);
    }
    if (AIScriptSetBaseScriptStateByName(&rider->ai_packet.script_processor, (char *)TakeOverState_ReleasedTakeOver) !=
        0) {
        AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)rider, &rider->ai_packet, &rider->ai_packet.script_processor,
                        FRAMETIME);
    }
    if (AIScriptSetBaseScriptStateByName(&object->ai_packet.script_processor, (char *)TakeOverState_ReleasedTakeOver) !=
        0) {
        AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, &object->ai_packet.script_processor,
                        FRAMETIME);
    }
}

void __cdecl ReleaseAllTakeOvers(void) {
    i32 index;
    for (index = 0; index < 8; ++index) {
        if (Player[index] != NULL) {
            ReleaseTakeOver(Player[index], 0);
        }
    }
}

void __cdecl TakeOverGameObject2(GAMEOBJECT *rider, GAMEOBJECT *vehicle, i32 blend_camera) {
    i32 result;
    if (rider == NULL || (rider->flags_1fc_word & 1) == 0 || vehicle == NULL || (vehicle->flags_1fc_word & 1) == 0 ||
        rider->rider != NULL || vehicle->rider != NULL) {
        return;
    }
    if ((rider->flags_ef4 & 2) != 0) {
        result = TakeOverYoda(rider, vehicle, blend_camera, 1);
    } else {
        if (WORLD->current_level == SPEEDERCHASEA_LDATA && disable_narrow_socks == 0 &&
            (rider->flags_1fc_word & 0x80) != 0) {
            blend_camera = 1;
        }
        RegisterTakeOverObject(vehicle);
        result = TagCode(rider, vehicle, 1, blend_camera, 1);
    }
    if (result == 2) {
        rider->tag_flags |= 0xc;
        rider->tag_cooldown = 1.0f;
        rider->pending_tag_target = vehicle;
    }
}

__declspec(noinline) void __cdecl TakeOverGameObject(GAMEOBJECT *rider, GAMEOBJECT *vehicle, i32 blend_camera,
                                                     i32 immediate) {
    if (immediate != 0) {
        TakeOverGameObject2(rider, vehicle, blend_camera);
    } else {
        TakeOver2GetIn(rider, vehicle);
    }
}

void __cdecl TakeOverCode(GAMEOBJECT *object, i32 tag_pressed) {
    GAMEOBJECT *target;
    if (object->character_context == 0x3c) {
        f32 fraction;
        NUVEC destination;
        f32 height;
        f64 phase;
        target = object->force_target;
        if (target == NULL) {
            object->character_context = -1;
            return;
        }
        object->context_timer += FRAMETIME;
        if (object->context_timer >= object->context_duration) {
            object->character_context = -1;
            if ((object->flags_1fc_word & 0x80) != 0) {
                if (target->id == (u16)id_GRABCONTROL || target->id == (u16)id_GRABR2CONTROL) {
                    Hint_SetComplete(0x28a);
                } else if (target->character_data->move_fn == Move_BEAST) {
                    if (StartupGameFlag1 != 0)
                        Hint_SetComplete(0x263);
                    else
                        Hint_SetComplete(8);
                } else if (StartupGameFlag1 != 0) {
                    if (object->id == (u16)id_ATAT)
                        Hint_SetComplete(0x28d);
                    else
                        Hint_SetComplete(0x25d);
                } else {
                    Hint_SetComplete(8);
                }
            }
            if (WORLD->grabber != NULL &&
                (object->force_target->id == (u16)id_GRABCONTROL || object->force_target->id == (u16)id_GRABR2CONTROL))
                PlaySfx("env_crane_in", &object->collision_position);
            else
                PlayLandSfx(object, 0, 0);
            NewBuzz(object->pad->bound_pad, 0.1f, 0);
            TakeOverGameObject2(object, object->force_target, 1);
            return;
        }
        fraction = object->context_timer / object->context_duration;
        GetTakeOverPos(target, &destination);
        object->position.x = (destination.x - object->external_force.x) * fraction + object->external_force.x;
        object->position.y = (destination.y - object->external_force.y) * fraction + object->external_force.y;
        object->position.z = (destination.z - object->external_force.z) * fraction + object->external_force.z;
        height = target->bounds_half_height;
        if (object->field_7aa == 6 || object->field_7aa == 0xc9) {
            if (object->character_data->game_character->jump_height > height) {
                height = object->character_data->game_character->jump_height;
            }
        }
        if (target->id == (u16)id_ATAT && WORLD->current_level == SPEEDERCHASEA_LDATA) {
            height *= TakeOverATATJumpScale * SpeederChaseATATInOutMul(&destination, &object->external_force);
        }
        phase = (f64)fraction * 32768.0;
        object->position.y += height * NuTrigTable[((i32)phase >> 1) & 0x7fff];
        object->facing_angle = object->movement_facing_angle = object->field_0x276 =
            (u16)((f64)object->takeover_start_angle +
                  (f64)RotDiff(object->takeover_start_angle, object->force_target->field_0x276) *
                      (1.0 - ((f64)NuTrigTable[((i32)(phase + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5));
        return;
    }
    object->tag_flags &= ~1u;
    target = object->rider;
    if (target != NULL) {
        if (object->character_context == 0x3b) {
            if (!(players_cannot_exit_speeder != 0 && target->id == (u16)id_SPEEDERBIKE)) {
                u32 buttons = target->pad->buttons_pressed;
                if ((object->flags_ef4 & 2) != 0) {
                    buttons &= ~GAMEPAD_JUMP;
                }
                if (target->character_context != 0x2a && (buttons & (GAMEPAD_JUMP | GAMEPAD_TAG)) != 0) {
                    ReleaseTakeOver(object, 1);
                    if ((object->flags_1fc_word & 0x80) != 0) {
                        if (StartupGameFlag1 != 0)
                            Hint_SetComplete(0x25e);
                        else
                            Hint_SetComplete(5);
                    }
                    return;
                }
            }
            SetRiderPos(object, NULL);
            object->takeover_timer = 1.0f;
        } else if (object->character_data->game_character->weapon_action == 12) {
            i32 animation =
                CurrentAnim(&object->anim_packet) == 3 || CurrentAnim(&object->anim_packet) == 0x17 ? 0xc4 : 0xc3;
            CHARACTER_MODEL_LAYERS_PC *model;
            target = object->rider;
            model = (CHARACTER_MODEL_LAYERS_PC *)target->character_model;
            if (model->action_animations[animation] != NULL) {
                target->field_7aa = (i16)animation;
            }
        }
        return;
    }
    if (object->takeover_timer >= 0.0f) {
        object->takeover_timer -= FRAMETIME;
        return;
    }
    if (MiniCutCam != 0 || (object->flags_1fc_word & 0x80) == 0 || object->dead != 0 ||
        !(object->takeover_timer <= 0.0f) || object->character_context == 0x2b ||
        (object->character_context != -1 && (object->character_context != 0 || object->action_movement_state == 3 ||
                                             object->action_movement_state == 4))) {
        return;
    }
    {
        f32 nearest_distance = 9.0f;
        GAMEOBJECT *nearest = NULL;
        GAMEOBJECT *candidate;
        i32 index;
        f32 dx, dz, range, range_squared, distance;
        if (object->field_281 == 0)
            return;
        candidate = Obj;
        for (index = 0; index < HIGHGAMEOBJECT; ++index, ++candidate) {
            if ((candidate->flags_1fc_word & 1) == 0 || (candidate->flags_1fc_word & 0x1000) == 0 ||
                candidate->dead != 0 || candidate->field_280 != 0xff ||
                (U64_PAIR(&CInfo[candidate->character_context].flags) & 0x8000) != 0 || candidate->rider != NULL ||
                (candidate->character_data->game_character->flags_090 & 0x40) == 0 ||
                ((candidate->character_data->model_flags & 0x40000000) != 0 &&
                 (candidate->character_context == 0x17 || candidate->character_context == 0x3e)) ||
                (candidate->tag_flags & 2) != 0) {
                continue;
            }
            if (candidate->id == (u16)id_ATAT && WORLD->current_level == SPEEDERCHASEA_LDATA) {
                if (object->jump_animation_state == 0x13) {
                    nearest = candidate;
                }
                continue;
            }
            if (object->collision_min.y > candidate->collision_max.y ||
                object->collision_max.y < candidate->collision_min.y) {
                continue;
            }
            range = (f64)candidate->bounds_radius * 2.0 + object->bounds_radius;
            dx = candidate->collision_position.x - object->collision_position.x;
            dz = candidate->collision_position.z - object->collision_position.z;
            if (object->facing_direction.x * dx + object->facing_direction.z * dz > 0.0f) {
                distance = dx * dx + dz * dz;
                range_squared = range * range;
                if (distance < range_squared && nearest_distance > distance) {
                    nearest_distance = distance;
                    nearest = candidate;
                }
            }
        }
        if (nearest != NULL) {
            if (tag_pressed != 0 || (nearest->id == (u16)id_ATAT && WORLD->current_level == SPEEDERCHASEA_LDATA)) {
                TakeOver2GetIn(object, nearest);
            } else {
                object->tag_flags |= 1;
                nearest->flags_e20 |= 0x80000000;
            }
        }
    }
}
