#include "saga.h"
#include <string.h>
#include <stdio.h>

STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, force_target) == 0x790);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);

typedef struct PUNCH_BLOWUP_VIEW_PC { // size 0x114
    u8 unknown_00[0x4c];              // 0x000
    NUVEC mid_position;               // 0x04c
    NUVEC bounds_min;                 // 0x058
    NUVEC bounds_max;                 // 0x064
    u8 unknown_70[0x24];              // 0x070
    u32 status_flags;                 // 0x094
    u32 draw_flags;                   // 0x098
    u32 secondary_flags;              // 0x09c
    u8 unknown_a0[8];                 // 0x0a0
    f32 target_scale;                 // 0x0a8
    u8 unknown_ac[0x56];              // 0x0ac
    i16 platform_id;                  // 0x102
    u8 unknown_104[0x10];             // 0x104
} PUNCH_BLOWUP_VIEW_PC;

typedef struct weapon_rate_pc_s {
    u8 unknown_000[0xd40];
    f32 weapon_scale_rate;
} WEAPON_RATE_PC;

typedef struct blowup_movement_pc_s {
    u8 unknown_00[0x4c];
    NUVEC mid_position;
    u8 unknown_58[0x50];
    f32 target_radius;
} BLOWUP_MOVEMENT_PC;

typedef struct HOLD_RUNTIME_VIEW_PC { // size 0xde8
    u8 unknown_000[0xde4];            // 0x000
    f32 hold_timer;                   // 0xde4
} HOLD_RUNTIME_VIEW_PC;

i32 ObjOpponentStillThere(GAMEOBJECT *object, GAMEOBJECT *opponent, f32 gap) {
    NUVEC forward;
    NUVEC difference;
    f32 distance;
    f32 dot;
    f32 radius;
    f32 radius_squared;
    if (object->force_target == NULL) {
        return 0;
    }
    NuVecRotateY(&forward, &v001, object->field_0x276);
    distance = NuVecDistSqr(&opponent->position, &object->position, &difference);
    dot = forward.x * difference.x + forward.z * difference.z;
    if ((!((object->context_variant_flags >> 7) & 1) && dot >= 0.0f) ||
        (((object->context_variant_flags >> 7) & 1) && dot <= 0.0f)) {
        radius = object->bounds_radius + opponent->bounds_radius;
        radius += gap;
        radius_squared = radius * radius;
        if (distance < radius_squared) {
            return 1;
        }
    }
    return 0;
}

void SetWeaponOut(GAMEOBJECT *object) {
    i8 context = object->character_context;
    if (context != -1 && (context == LEGOCONTEXT_WEAPONIN || context == LEGOCONTEXT_WEAPONOUT)) {
        object->character_context = -1;
    }
    object->weapon_scale = 1.0f;
    object->flags_e20 |= 0x10000;
    object->field_e33 = 0;
}

void FastWeaponOut(GAMEOBJECT *object, i32 force_sound) {
    i8 context = object->character_context;
    i32 animation;
    if (context != -1 && (context == LEGOCONTEXT_WEAPONIN || context == LEGOCONTEXT_WEAPONOUT)) {
        object->character_context = -1;
    }
    if (force_sound != 0 && object->weapon_scale == 0.0f && object->field_e33 != 1) {
        animation = CurrentAnim(&object->anim_packet);
        if (animation == -1 || object->character_model->animation_configs[animation] == NULL ||
            (object->character_model->animation_configs[animation]->flags & 0x400) == 0) {
            if (object->character_data->model_flags & 8) {
                if (object->field_280 != 0xff || WeaponInOut_NoAIJediSfx == 0) {
                    GameAudio_PlaySfx(0x3e, &object->collision_position, GameAudio_GetPlrSfxBits(object), 1);
                }
            } else if (object->character_data->model_flags & 0x80) {
                GameAudio_PlaySfx(0x43, &object->collision_position, 0, 1);
            }
        }
    }
    ((WEAPON_RATE_PC *)object)->weapon_scale_rate = 5.0f;
    object->field_e33 = 1;
}

void SlowWeaponOut(GAMEOBJECT *object) {
    i32 action;
    f32 end;
    f32 start;
    f32 finish;
    if (LEGOCONTEXT_WEAPONOUT != -1 && Fighting_WeaponOutActionFn != NULL) {
        action = Fighting_WeaponOutActionFn(object);
        if (action != -1 && object->character_model->model_data_b[action] != NULL) {
            end = NuAnimEndFrame(object->character_model->model_data_b[action]);
            start = AnimListFrame(object->character_model, action, 0);
            if (start < 1.0f) {
                start = 1.0f;
            }
            if (start >= 1.0f && start < end) {
                finish = AnimListFrame(object->character_model, action, 1);
                if (end > finish) {
                    end = finish;
                }
                if (end > start) {
                    object->character_context = (i8)LEGOCONTEXT_WEAPONOUT;
                    object->field_7aa = (i16)action;
                    object->context_timer = AnimDuration((i16)object->id, (i16)action, 0.0f, 0.0f, 1);
                    object->weapon_scale = 0.0f;
                    return;
                }
            }
        }
    }
    FastWeaponOut(object, 1);
}

void WeaponOutCode(GAMEOBJECT *object) {
    f32 start;
    f32 end;
    f32 *time;
    if (LEGOCONTEXT_WEAPONOUT == -1 || object->character_context != LEGOCONTEXT_WEAPONOUT) {
        return;
    }
    if (object->pad->input_magnitude > 0.0f && (object->character_data->game_character->flags_094 & 0x1000) == 0) {
        if (object->weapon_scale == 0.0f && object->field_e33 != 1) {
            FastWeaponOut(object, 0);
        }
        object->character_context = -1;
        return;
    }
    if ((object->anim_packet.blending && object->anim_packet.blend_target_animation != object->field_7aa) ||
        (!object->anim_packet.blending && object->anim_packet.animation_index != object->field_7aa)) {
        object->weapon_scale = 0.0f;
        return;
    }
    start = AnimListFrame(object->character_model, object->field_7aa, 0);
    object->context_timer -= FRAMETIME;
    if (object->context_timer <= 0.0f) {
        object->character_context = -1;
        if (object->weapon_scale != 0.0f) {
            return;
        }
    } else {
        if (object->weapon_scale != 0.0f || object->field_e33 == 1 ||
            object->anim_packet.requested_animation != object->field_7aa) {
            return;
        }
        time =
            object->anim_packet.blending ? &object->anim_packet.blend_target_time : &object->anim_packet.current_time;
        if (!(*time >= start)) {
            return;
        }
    }
    end = AnimListFrame(object->character_model, object->field_7aa, 1);
    object->field_e33 = 1;
    ((WEAPON_RATE_PC *)object)->weapon_scale_rate =
        1.0f / AnimDuration((i16)object->id, object->field_7aa, start, end, 0);
    if ((object->character_data->model_flags & 8) != 0) {
        if (object->field_280 != 0xff || WeaponInOut_NoAIJediSfx == 0) {
            GameAudio_PlaySfx(0x3e, &object->collision_position, GameAudio_GetPlrSfxBits(object), 1);
        }
    } else {
        GameAudio_PlaySfx(0x43, &object->collision_position, 0, 1);
    }
    ResetCharacterIdle(object, 1, GetDefaultIdle(object));
}

void KeepWeaponOut(GAMEOBJECT *object) {
    object->weapon_scale = 1.0f;
    object->flags_e20 |= 0x10000;
    object->portal_flags |= 0x10;
    object->field_e33 = 0;
}

void SetWeaponIn(GAMEOBJECT *object) {
    i8 context = object->character_context;
    if (context != -1 && (context == LEGOCONTEXT_WEAPONIN || context == LEGOCONTEXT_WEAPONOUT)) {
        object->character_context = -1;
    }
    object->weapon_scale = 0.0f;
    object->flags_e20 &= ~0x10000u;
    object->field_e33 = 0;
}

void FastWeaponIn(GAMEOBJECT *object, i32 force_sound) {
    i8 context = object->character_context;
    i32 animation;
    if (context != -1 && (context == LEGOCONTEXT_WEAPONIN || context == LEGOCONTEXT_WEAPONOUT)) {
        object->character_context = -1;
    }
    if (force_sound != 0 && object->weapon_scale == 1.0f && object->field_e33 != 2) {
        animation = CurrentAnim(&object->anim_packet);
        if (animation == -1 || object->character_model->animation_configs[animation] == NULL ||
            (object->character_model->animation_configs[animation]->flags & 0x800) == 0) {
            if (object->character_data->model_flags & 8) {
                if (object->field_280 != 0xff || WeaponInOut_NoAIJediSfx == 0) {
                    GameAudio_PlaySfx(0x3d, &object->collision_position, GameAudio_GetPlrSfxBits(object), 1);
                }
            } else if (object->character_data->model_flags & 0x80) {
                GameAudio_PlaySfx(0x42, &object->collision_position, 0, 1);
            }
        }
    }
    ((WEAPON_RATE_PC *)object)->weapon_scale_rate = 5.0f;
    object->field_e33 = 2;
}

void SlowWeaponIn(GAMEOBJECT *object) {
    i32 action = -1;
    f32 end;
    f32 start;
    f32 finish;
    if (LEGOCONTEXT_WEAPONIN != -1) {
        if (Fighting_WeaponInActionFn != NULL) {
            action = Fighting_WeaponInActionFn(object);
        }
        /* Indexes model_data_b with -1 when the callback is absent or returns -1. */
        if (object->character_model->model_data_b[action] != NULL) {
            end = NuAnimEndFrame(object->character_model->model_data_b[action]);
            start = AnimListFrame(object->character_model, action, 0);
            if (start < 1.0f) {
                start = 1.0f;
            }
            if (start >= 1.0f && start < end) {
                finish = AnimListFrame(object->character_model, action, 1);
                if (end > finish) {
                    end = finish;
                }
                if (end > start) {
                    object->character_context = (i8)LEGOCONTEXT_WEAPONIN;
                    object->field_7aa = (i16)action;
                    object->context_timer = AnimDuration((i16)object->id, (i16)action, 0.0f, 0.0f, 1);
                    object->weapon_scale = 1.0f;
                    return;
                }
            }
        }
    }
    FastWeaponIn(object, 1);
}

void WeaponInCode(GAMEOBJECT *object) {
    f32 start;
    f32 end;
    f32 *time;
    if (LEGOCONTEXT_WEAPONIN == -1 || object->character_context != LEGOCONTEXT_WEAPONIN) {
        return;
    }
    if (object->pad->input_magnitude > 0.0f && (object->character_data->game_character->flags_094 & 0x1000) == 0) {
        if (object->weapon_scale == 1.0f && object->field_e33 != 2) {
            FastWeaponIn(object, 0);
        }
        object->character_context = -1;
        return;
    }
    if ((object->anim_packet.blending && object->anim_packet.blend_target_animation != object->field_7aa) ||
        (!object->anim_packet.blending && object->anim_packet.animation_index != object->field_7aa)) {
        object->weapon_scale = 1.0f;
        return;
    }
    start = AnimListFrame(object->character_model, object->field_7aa, 0);
    object->context_timer -= FRAMETIME;
    if (object->context_timer <= 0.0f) {
        object->character_context = -1;
        if (object->weapon_scale != 1.0f) {
            return;
        }
    } else {
        if (object->weapon_scale != 1.0f || object->field_e33 == 2 ||
            object->anim_packet.requested_animation != object->field_7aa) {
            return;
        }
        time =
            object->anim_packet.blending ? &object->anim_packet.blend_target_time : &object->anim_packet.current_time;
        if (!(*time >= start)) {
            return;
        }
    }
    end = AnimListFrame(object->character_model, object->field_7aa, 1);
    object->field_e33 = 2;
    ((WEAPON_RATE_PC *)object)->weapon_scale_rate =
        1.0f / AnimDuration((i16)object->id, object->field_7aa, start, end, 0);
    if ((object->character_data->model_flags & 8) != 0) {
        if (object->field_280 != 0xff || WeaponInOut_NoAIJediSfx == 0) {
            GameAudio_PlaySfx(0x3d, &object->collision_position, GameAudio_GetPlrSfxBits(object), 1);
        }
    } else {
        GameAudio_PlaySfx(0x42, &object->collision_position, 0, 1);
    }
    ResetCharacterIdle(object, 1, GetDefaultIdle(object));
}

void KeepWeaponIn(GAMEOBJECT *object) {
    object->flags_e20 &= ~0x10000u;
    object->weapon_scale = 0.0f;
    object->portal_flags &= ~0x10u;
    object->field_e33 = 0;
}

void AutoWeaponOnOff(GAMEOBJECT *object) {
    i32 animation;
    u32 flags;
    i8 context = object->character_context;
    if (context != -1 && (context == LEGOCONTEXT_WEAPONOUT || context == LEGOCONTEXT_WEAPONIN)) {
        return;
    }
    animation = CurrentAnim(&object->anim_packet);
    if (animation == -1) {
        return;
    }
    flags = object->character_model->animation_configs[animation]->flags;
    if ((flags & 0x400) != 0) {
        if (object->weapon_scale > 0.0f) {
            if ((object->anim_packet.flags & 4) != 0) {
                if (object->anim_packet.blending != 0) {
                    FastWeaponIn(object, 0);
                } else {
                    SetWeaponIn(object);
                }
            } else if (object->field_e33 != 2) {
                SetWeaponIn(object);
            }
        }
    } else if ((flags & 0x800) != 0 && object->weapon_scale < 1.0f) {
        if ((object->anim_packet.flags & 4) != 0) {
            if (object->anim_packet.blending != 0) {
                FastWeaponOut(object, 0);
            } else {
                SetWeaponOut(object);
            }
        } else if (object->field_e33 != 1) {
            SetWeaponOut(object);
        }
    }
}

void WeaponScalingCode(GAMEOBJECT *object) {
    switch (object->field_e33) {
        case 1:
            object->weapon_scale =
                SeekLinearF(object->weapon_scale, 1.0f, ((WEAPON_RATE_PC *)object)->weapon_scale_rate * FRAMETIME);
            if (object->weapon_scale >= 1.0f) {
                object->field_e33 = 0;
                object->flags_e20 |= 0x10000;
            }
            break;
        case 2:
            object->weapon_scale =
                SeekLinearF(object->weapon_scale, 0.0f, ((WEAPON_RATE_PC *)object)->weapon_scale_rate * FRAMETIME);
            if (object->weapon_scale <= 0.0f) {
                object->field_e33 = 0;
                object->flags_e20 &= ~0x10000;
            }
            break;
    }
}

i32 NewBlockAction(GAMEOBJECT *object) {
    i32 actions[3];
    i32 count = 0;
    i32 action;
    for (action = 0; count < 3 && action < apicharsys->model_id_capacity; ++action) {
        if ((((API_ACTION_INFO_PC *)PlayerBlockActionInfoPc)[action].flags & 8) != 0 &&
            object->character_model->model_data_b[action] != NULL) {
            actions[count++] = action;
        }
    }
    if (count == 0) {
        return 0;
    }
    if (count > 1) {
        do {
            action = actions[qrand() / (0xffff / count + 1)];
        } while (action == object->previous_block_animation);
    } else {
        action = actions[0];
    }
    object->previous_block_animation = (i16)action;
    object->field_7aa = (i16)action;
    return 1;
}

void BlockSfx(GAMEOBJECT *object) {
    GAMEOBJECT *target = object->force_target;
    if (target != NULL) {
        if ((U64_PAIR(&CInfo[target->character_context].flags) & 0x4000000) != 0 ||
            ((U64_PAIR(&CInfo[target->character_context].flags) & 0x8000000) != 0 &&
             (target->context_variant_flags & 0x20000) != 0)) {
            GameAudio_PlaySfx(0x3f, &object->collision_position, GameAudio_GetPlrSfxBits(target), 0);
        }
    }
}

i32 CanStepBack(GAMEOBJECT *object) {
    i8 context = object->character_context;
    if ((U64_PAIR(&CInfo[context].flags) & 0x10000) != 0) {
        return 1;
    }
    if (LEGOCONTEXT_JUMP != -1 && context == LEGOCONTEXT_JUMP && object->action_movement_state == 3) {
        return 1;
    }
    if ((object->character_data->game_character->flags_090 & 0x8000) && LEGOCONTEXT_COMBO != 0 &&
        context == LEGOCONTEXT_COMBO) {
        return 1;
    }
    return 0;
}

i32 StepBackFromTarget(GAMEOBJECT *object) {
    NUVEC *target_position;
    BLOWUP_MOVEMENT_PC *blowup = (BLOWUP_MOVEMENT_PC *)object->blowup_target;
    f32 inner_distance;
    f32 outer_distance;
    f32 speed;
    f32 distance_squared;
    f32 blend;
    if (blowup != NULL) {
        target_position = &blowup->mid_position;
        if ((LEGOCONTEXT_COMBO != -1 && object->character_context == LEGOCONTEXT_COMBO) ||
            (LEGOCONTEXT_JUMP != -1 && object->character_context == LEGOCONTEXT_JUMP &&
             object->action_movement_state == 3)) {
            inner_distance = object->collision_radius * PlayerStepInnerActionPc + blowup->target_radius;
            outer_distance = object->collision_radius * PlayerStepOuterActionPc + blowup->target_radius;
        } else {
            inner_distance = object->collision_radius * 0.75 + blowup->target_radius;
            outer_distance = object->collision_radius * 1.25 + blowup->target_radius;
        }
    } else {
        if (!(object->character_data->game_character->flags_090 & 0x8000) || object->force_target == NULL) {
            return 0;
        }
        if ((LEGOCONTEXT_COMBO == -1 || object->character_context != LEGOCONTEXT_COMBO) &&
            (LEGOCONTEXT_JUMP == -1 || object->character_context != LEGOCONTEXT_JUMP ||
             object->action_movement_state != 3)) {
            return 0;
        }
        target_position = &object->force_target->collision_position;
        inner_distance = object->bounds_radius * PlayerStepInnerActionPc + object->force_target->bounds_radius;
        outer_distance = object->bounds_radius * PlayerStepOuterActionPc + object->force_target->bounds_radius;
    }
    if (target_position == NULL) {
        return 0;
    }
    speed = AnimSpeed(object->character_model, object->field_7aa);
    if (speed == 0.0f) {
        return 0;
    }
    distance_squared = NuVecXZDistSqr(&object->collision_position, target_position, NULL);
    if (distance_squared >= outer_distance * outer_distance) {
        return 0;
    }
    if (inner_distance * inner_distance >= distance_squared) {
        blend = 1.0f;
    } else {
        blend = 1.0f - (NuFsqrt(distance_squared) - inner_distance) / (outer_distance - inner_distance);
    }
    speed = (-speed - speed) * blend + speed;
    object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1] * speed;
    object->target_velocity.z = speed * NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff];
    return 1;
}

void ComboHitFrame(GAMEOBJECT *object, i32 damage) {
    object->context_variant_flags |= 0x40;
    object->sabre_flags |= 5;
    object->sabre_damage = (u8)damage;
    if ((object->flags_1fc & 0x80) != 0 && Player_HasDoubleWeaponDamage(object)) {
        object->sabre_damage = (u8)(object->sabre_damage + object->sabre_damage);
    }
    BlockSfx(object);
}

void StartHold(GAMEOBJECT *object) {
    if (CanStartHoldFn == NULL) {
        return;
    }
    if (CanStartHoldFn(object) == 2) {
        ((HOLD_RUNTIME_VIEW_PC *)object)->hold_timer = 0.0f;
        object->character_context = -1;
        return;
    }
    if (LEGOCONTEXT_HOLD != -1 && NewBlockAction(object)) {
        object->character_context = (i8)LEGOCONTEXT_HOLD;
        object->context_timer = 0.3f;
        PlaySabreSfx(NULL, object, NULL, 1);
        return;
    }
    object->character_context = -1;
}

void HoldCode(GAMEOBJECT *object) {
    if (LEGOCONTEXT_HOLD != -1 && object->character_context == LEGOCONTEXT_HOLD) {
        if (object->context_timer > 0.0f) {
            object->context_timer -= FRAMETIME;
        } else if ((object->pad->buttons_held & GAMEPAD_ACTION) == 0) {
            object->character_context = -1;
        }
    }
}

void fun_0058e480(GAMEOBJECT *object) {
    if (LEGOCONTEXT_HOLD != -1 && object->character_context == LEGOCONTEXT_HOLD) {
        if (object->context_timer > 0.0f) {
            object->context_timer -= FRAMETIME;
        } else if ((object->pad->buttons_held & GAMEPAD_ACTION) == 0) {
            object->character_context = -1;
        }
    }
}

void Punch_Hit(GAMEOBJECT *attacker, GAMEOBJECT *target, f32 unused, f32 gap) {
    NUVEC *hit_position = NULL;
    u64 flags;
    f32 impulse;
    u16 angle;
    i32 damage;
    PUNCH_BLOWUP_VIEW_PC *blowup;
    (void)unused;
    attacker->context_variant_flags |= 0x40;
    if (LEGOCONTEXT_PUNCH != -1 && attacker->character_context == LEGOCONTEXT_PUNCH) {
        if (attacker->character_model->animations[attacker->field_7aa] != NULL) {
            attacker->debris_joint = attacker->character_model->animations[attacker->field_7aa]->locator;
        }
    }
    if (target != NULL) {
        if (!(target->flags_1fc_word & 1) || !(target->flags_1fc_word & 0x1000) || target->dead != 0 ||
            attacker->collision_max.y < target->collision_min.y ||
            attacker->collision_min.y > target->collision_max.y) {
            goto finish;
        }
        if (LEGOCONTEXT_HOLD != -1 && target->character_context == LEGOCONTEXT_HOLD) {
            NewBlockAction(target);
            hit_position = &target->collision_position;
            if (Punch_HitHoldFn != NULL) {
                Punch_HitHoldFn(attacker, target);
            }
        } else {
            if (target->player_controlled) {
                if (LEGOCONTEXT_PUNCH != -1 && target->character_context == LEGOCONTEXT_PUNCH) {
                    goto finish;
                }
                if (LEGOCONTEXT_JUMP != -1 && target->character_context == LEGOCONTEXT_JUMP &&
                    attacker->action_movement_state == 3) {
                    goto finish;
                }
            }
            if (!ObjOpponentStillThere(attacker, target, gap)) {
                goto finish;
            }
            damage = 1;
            if (Punch_GetDamageFn != NULL) {
                damage = Punch_GetDamageFn(attacker, target);
            }
            if (damage != -1 && Cheats_CheckFlags(0x800)) {
                damage *= 2;
            }
            flags = U64_PAIR(&CInfo[target->character_context].flags);
            if (!(flags & 0x80)) {
                if ((flags & 0x04000000) || ((flags & 0x08000000) && (target->context_variant_flags & 0x20000)) ||
                    CannotKill(target)) {
                    NewRumble(attacker->pad->bound_pad, 0.5f, 0);
                    NewRumble(target->pad->bound_pad, 0.5f, 0);
                    if (!(U64_PAIR(&CInfo[target->character_context].flags) & 0x80000000)) {
                        target->movement_facing_angle =
                            NuAtan2D(attacker->collision_position.x - target->collision_position.x,
                                     attacker->collision_position.z - target->collision_position.z);
                    }
                } else {
                    ObjHitObj(attacker, target, damage, (u16)(ObjHitObj_Flags(attacker) | 0x40), 0, 1);
                }
                hit_position = &target->collision_position;
            }
            if (!(target->flags_1fc & 2)) {
                impulse = target->character_data->game_character->run_speed;
                if (attacker->scaled_radius > 1.0f) {
                    impulse /= attacker->scaled_radius;
                }
                angle = attacker->movement_facing_angle;
                if (attacker->context_flags_pc & 0x80) {
                    angle += 0x8000;
                }
                target->velocity.x += NuTrigTable[angle >> 1] * impulse;
                target->velocity.z += NuTrigTable[(angle + 0x4000) >> 1 & 0x7fff] * impulse;
            }
        }
    } else {
        blowup = (PUNCH_BLOWUP_VIEW_PC *)attacker->blowup_target;
        if (blowup == NULL || (blowup->status_flags & 1) || !(blowup->status_flags & 0x4000) ||
            !(blowup->status_flags & 0x800000) || attacker->collision_max.y < blowup->bounds_min.y ||
            attacker->collision_min.y > blowup->bounds_max.y) {
            goto finish;
        }
        hit_position = &blowup->mid_position;
        if (GizmoBlowupBlowup((GIZMOBLOWUP_PC *)blowup, 1, 1, 1, NULL, 1)) {
            NewBuzz(attacker->pad->bound_pad, 0.1f, 0);
        }
        NewRumble(attacker->pad->bound_pad, 0.5f, 0);
    }
    if (hit_position == NULL) {
        goto finish;
    }
    attacker->flags_e24 |= 0x100;
    if (attacker->player_controlled) {
        GameCam_HitJudder();
    }
    goto extra;
finish:
    attacker->flags_e24 &= ~0x100;
extra:
    if (Punch_HitExtraCodeFn != NULL) {
        Punch_HitExtraCodeFn(attacker, hit_position);
    }
}
