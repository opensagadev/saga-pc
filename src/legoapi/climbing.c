#include "saga.h"
#include <string.h>
#include <stdio.h>

/* Retail unit 0x58d340-0x58e360 (legoapi/climbing.c): definitions in retail address order. */

extern i32 LEGOCONTEXT_WEAPONIN, LEGOCONTEXT_WEAPONOUT;
extern i32 WeaponInOut_NoAIJediSfx;
typedef struct weapon_rate_pc_s {
    u8 unknown_000[0xd40];
    f32 weapon_scale_rate;
} WEAPON_RATE_PC;
extern i32(__cdecl *Fighting_WeaponInActionFn)(GAMEOBJECT *);
extern i32(__cdecl *Fighting_WeaponOutActionFn)(GAMEOBJECT *);
extern i32 LEGOCONTEXT_COMBO;
extern const f64 PlayerStepInnerActionPc, PlayerStepOuterActionPc, PlayerStepInnerPc, PlayerStepOuterPc;
typedef struct blowup_movement_pc_s {
    u8 unknown_00[0x4c];
    NUVEC mid_position;
    u8 unknown_58[0x50];
    f32 target_radius;
} BLOWUP_MOVEMENT_PC;

i32 __cdecl ObjOpponentStillThere(GAMEOBJECT *object, GAMEOBJECT *opponent, f32 gap) {
    NUVEC forward, difference;
    f32 distance, dot, radius, radius_squared;
    if (object->force_target == NULL)
        return 0;
    NuVecRotateY(&forward, &v001, object->field_0x276);
    distance = NuVecDistSqr(&opponent->position, &object->position, &difference);
    dot = forward.x * difference.x + forward.z * difference.z;
    if ((!(object->context_variant_flags & 0x80) && dot >= 0.0f) ||
        ((object->context_variant_flags & 0x80) && dot <= 0.0f)) {
        radius = object->bounds_radius + opponent->bounds_radius;
        radius += gap;
        radius_squared = radius * radius;
        if (distance < radius_squared)
            return 1;
    }
    return 0;
}

void __cdecl SetWeaponOut(GAMEOBJECT *object) {
    i8 context = object->character_context;
    if (context != -1 && (context == LEGOCONTEXT_WEAPONIN || context == LEGOCONTEXT_WEAPONOUT))
        object->character_context = -1;
    object->weapon_scale = 1.0f;
    object->flags_e20 |= 0x10000;
    object->field_e33 = 0;
}

void __cdecl FastWeaponOut(GAMEOBJECT *object, i32 force_sound) {
    i8 context = object->character_context;
    i32 animation;
    if (context != -1 && (context == LEGOCONTEXT_WEAPONIN || context == LEGOCONTEXT_WEAPONOUT))
        object->character_context = -1;
    if (force_sound != 0 && object->weapon_scale == 0.0f && object->field_e33 != 1) {
        animation = CurrentAnim(&object->anim_packet);
        if (animation == -1 || object->character_model->animation_configs[animation] == NULL ||
            (object->character_model->animation_configs[animation]->flags & 0x400) == 0) {
            if (object->character_data->model_flags & 8) {
                if (object->field_280 != 0xff || WeaponInOut_NoAIJediSfx == 0)
                    GameAudio_PlaySfx(0x3e, &object->collision_position, GameAudio_GetPlrSfxBits(object), 1);
            } else if (object->character_data->model_flags & 0x80)
                GameAudio_PlaySfx(0x43, &object->collision_position, 0, 1);
        }
    }
    ((WEAPON_RATE_PC *)object)->weapon_scale_rate = 5.0f;
    object->field_e33 = 1;
}

void __cdecl SlowWeaponOut(GAMEOBJECT *object) {
    i32 action;
    f32 end, start, finish;
    if (LEGOCONTEXT_WEAPONOUT != -1 && Fighting_WeaponOutActionFn != NULL) {
        action = Fighting_WeaponOutActionFn(object);
        if (action != -1 && object->character_model->model_data_b[action] != NULL) {
            end = NuAnimEndFrame(object->character_model->model_data_b[action]);
            start = AnimListFrame(object->character_model, action, 0);
            if (start < 1.0f)
                start = 1.0f;
            if (start >= 1.0f && start < end) {
                finish = AnimListFrame(object->character_model, action, 1);
                if (end > finish)
                    end = finish;
                if (start < end) {
                    object->field_7aa = (i16)action;
                    object->character_context = (i8)LEGOCONTEXT_WEAPONOUT;
                    object->context_timer = AnimDuration((i16)object->id, (i16)action, 0.0f, 0.0f, 1);
                    object->weapon_scale = 0.0f;
                    return;
                }
            }
        }
    }
    FastWeaponOut(object, 1);
}

void __cdecl WeaponOutCode(GAMEOBJECT *object) {
    ANIMPACKET_PC *packet;
    i16 animation;
    f32 start, end, time;
    if (LEGOCONTEXT_WEAPONOUT == -1 || object->character_context != LEGOCONTEXT_WEAPONOUT)
        return;
    if (object->pad->input_magnitude > 0.0f && (object->character_data->game_character->flags_094 & 0x1000) == 0) {
        if (object->weapon_scale == 0.0f && object->field_e33 != 1)
            FastWeaponOut(object, 0);
        object->character_context = -1;
        return;
    }
    packet = &object->anim_packet;
    animation = packet->blending ? packet->blend_target_animation : packet->animation_index;
    if (animation != object->field_7aa) {
        object->weapon_scale = 0.0f;
        return;
    }
    start = AnimListFrame(object->character_model, object->field_7aa, 0);
    object->context_timer -= FRAMETIME;
    if (object->context_timer <= 0.0f) {
        object->character_context = -1;
        if (object->weapon_scale != 0.0f)
            return;
    } else {
        if (object->weapon_scale != 0.0f || object->field_e33 == 1 || packet->requested_animation != object->field_7aa)
            return;
        time = packet->blending ? packet->blend_target_time : packet->current_time;
        if (!(time >= start))
            return;
    }
    end = AnimListFrame(object->character_model, object->field_7aa, 1);
    object->field_e33 = 1;
    ((WEAPON_RATE_PC *)object)->weapon_scale_rate =
        1.0f / AnimDuration((i16)object->id, object->field_7aa, start, end, 0);
    if ((object->character_data->model_flags & 8) != 0) {
        if (object->field_280 != 0xff || WeaponInOut_NoAIJediSfx == 0)
            GameAudio_PlaySfx(0x3e, &object->collision_position, GameAudio_GetPlrSfxBits(object), 1);
    } else
        GameAudio_PlaySfx(0x43, &object->collision_position, 0, 1);
    ResetCharacterIdle(object, 1, GetDefaultIdle(object));
}

void __cdecl KeepWeaponOut(GAMEOBJECT *object) {
    object->weapon_scale = 1.0f;
    object->flags_e20 |= 0x10000;
    object->portal_flags |= 0x10;
    object->field_e33 = 0;
}

void __cdecl SetWeaponIn(GAMEOBJECT *object) {
    i8 context = object->character_context;
    if (context != -1 && (context == LEGOCONTEXT_WEAPONIN || context == LEGOCONTEXT_WEAPONOUT))
        object->character_context = -1;
    object->weapon_scale = 0.0f;
    object->flags_e20 &= ~0x10000u;
    object->field_e33 = 0;
}

void __cdecl FastWeaponIn(GAMEOBJECT *object, i32 force_sound) {
    i8 context = object->character_context;
    i32 animation;
    if (context != -1 && (context == LEGOCONTEXT_WEAPONIN || context == LEGOCONTEXT_WEAPONOUT))
        object->character_context = -1;
    if (force_sound != 0 && object->weapon_scale == 1.0f && object->field_e33 != 2) {
        animation = CurrentAnim(&object->anim_packet);
        if (animation == -1 || object->character_model->animation_configs[animation] == NULL ||
            (object->character_model->animation_configs[animation]->flags & 0x800) == 0) {
            if (object->character_data->model_flags & 8) {
                if (object->field_280 != 0xff || WeaponInOut_NoAIJediSfx == 0)
                    GameAudio_PlaySfx(0x3d, &object->collision_position, GameAudio_GetPlrSfxBits(object), 1);
            } else if (object->character_data->model_flags & 0x80)
                GameAudio_PlaySfx(0x42, &object->collision_position, 0, 1);
        }
    }
    ((WEAPON_RATE_PC *)object)->weapon_scale_rate = 5.0f;
    object->field_e33 = 2;
}

void __cdecl SlowWeaponIn(GAMEOBJECT *object) {
    i32 action = -1;
    f32 end, start, finish;
    if (LEGOCONTEXT_WEAPONIN != -1) {
        if (Fighting_WeaponInActionFn != NULL)
            action = Fighting_WeaponInActionFn(object);
        /* Native indexes model_data_b with -1 when the callback is absent or returns -1. */
        if (object->character_model->model_data_b[action] != NULL) {
            end = NuAnimEndFrame(object->character_model->model_data_b[action]);
            start = AnimListFrame(object->character_model, action, 0);
            if (start < 1.0f)
                start = 1.0f;
            if (start >= 1.0f && start < end) {
                finish = AnimListFrame(object->character_model, action, 1);
                if (end > finish)
                    end = finish;
                if (start < end) {
                    object->field_7aa = (i16)action;
                    object->character_context = (i8)LEGOCONTEXT_WEAPONIN;
                    object->context_timer = AnimDuration((i16)object->id, (i16)action, 0.0f, 0.0f, 1);
                    object->weapon_scale = 1.0f;
                    return;
                }
            }
        }
    }
    FastWeaponIn(object, 1);
}

void __cdecl WeaponInCode(GAMEOBJECT *object) {
    ANIMPACKET_PC *packet;
    i16 animation;
    f32 start, end, time;
    if (LEGOCONTEXT_WEAPONIN == -1 || object->character_context != LEGOCONTEXT_WEAPONIN)
        return;
    if (object->pad->input_magnitude > 0.0f && (object->character_data->game_character->flags_094 & 0x1000) == 0) {
        if (object->weapon_scale == 1.0f && object->field_e33 != 2)
            FastWeaponIn(object, 0);
        object->character_context = -1;
        return;
    }
    packet = &object->anim_packet;
    animation = packet->blending ? packet->blend_target_animation : packet->animation_index;
    if (animation != object->field_7aa) {
        object->weapon_scale = 1.0f;
        return;
    }
    start = AnimListFrame(object->character_model, object->field_7aa, 0);
    object->context_timer -= FRAMETIME;
    if (object->context_timer <= 0.0f) {
        object->character_context = -1;
        if (object->weapon_scale != 1.0f)
            return;
    } else {
        if (object->weapon_scale != 1.0f || object->field_e33 == 2 || packet->requested_animation != object->field_7aa)
            return;
        time = packet->blending ? packet->blend_target_time : packet->current_time;
        if (!(time >= start))
            return;
    }
    end = AnimListFrame(object->character_model, object->field_7aa, 1);
    object->field_e33 = 2;
    ((WEAPON_RATE_PC *)object)->weapon_scale_rate =
        1.0f / AnimDuration((i16)object->id, object->field_7aa, start, end, 0);
    if ((object->character_data->model_flags & 8) != 0) {
        if (object->field_280 != 0xff || WeaponInOut_NoAIJediSfx == 0)
            GameAudio_PlaySfx(0x3d, &object->collision_position, GameAudio_GetPlrSfxBits(object), 1);
    } else
        GameAudio_PlaySfx(0x42, &object->collision_position, 0, 1);
    ResetCharacterIdle(object, 1, GetDefaultIdle(object));
}

void __cdecl KeepWeaponIn(GAMEOBJECT *object) {
    object->flags_e20 &= ~0x10000u;
    object->weapon_scale = 0.0f;
    object->portal_flags &= ~0x10u;
    object->field_e33 = 0;
}

void __cdecl AutoWeaponOnOff(GAMEOBJECT *object) {
    i32 animation;
    u32 flags;
    i8 context = object->character_context;
    if (context != -1 && (context == LEGOCONTEXT_WEAPONOUT || context == LEGOCONTEXT_WEAPONIN))
        return;
    animation = CurrentAnim(&object->anim_packet);
    if (animation == -1)
        return;
    flags = object->character_model->animation_configs[animation]->flags;
    if ((flags & 0x400) != 0) {
        if (object->weapon_scale > 0.0f) {
            if ((object->anim_packet.flags & 4) != 0) {
                if (object->anim_packet.blending != 0)
                    FastWeaponIn(object, 0);
                else
                    SetWeaponIn(object);
            } else if (object->field_e33 != 2)
                SetWeaponIn(object);
        }
    } else if ((flags & 0x800) != 0 && object->weapon_scale < 1.0f) {
        if ((object->anim_packet.flags & 4) != 0) {
            if (object->anim_packet.blending != 0)
                FastWeaponOut(object, 0);
            else
                SetWeaponOut(object);
        } else if (object->field_e33 != 1)
            SetWeaponOut(object);
    }
}

void __cdecl WeaponScalingCode(GAMEOBJECT *object) {
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

i32 __cdecl NewBlockAction(GAMEOBJECT *object) {
    i32 actions[3];
    i32 count = 0;
    i32 action;
    for (action = 0; count < 3 && action < apicharsys->model_id_capacity; ++action)
        if ((((API_ACTION_INFO_PC *)PlayerBlockActionInfoPc)[action].flags & 8) != 0 &&
            object->character_model->model_data_b[action] != NULL)
            actions[count++] = action;
    if (count == 0)
        return 0;
    if (count > 1) {
        do {
            action = actions[qrand() / (0xffff / count + 1)];
        } while (action == object->previous_block_animation);
    } else
        action = actions[0];
    object->previous_block_animation = (i16)action;
    object->field_7aa = (i16)action;
    return 1;
}

void __cdecl BlockSfx(GAMEOBJECT *object) {
    GAMEOBJECT *target = object->force_target;
    if (target != NULL) {
        if ((U64_PAIR(&CInfo[target->character_context].flags) & 0x4000000) != 0 ||
            ((U64_PAIR(&CInfo[target->character_context].flags) & 0x8000000) != 0 &&
             (target->context_variant_flags & 0x20000) != 0)) {
            GameAudio_PlaySfx(0x3f, &object->collision_position, GameAudio_GetPlrSfxBits(target), 0);
        }
    }
}

__declspec(noinline) i32 __cdecl CanStepBack(GAMEOBJECT *object) {
    i8 context = object->character_context;
    if ((U64_PAIR(&CInfo[context].flags) & 0x10000) != 0)
        return 1;
    if (LEGOCONTEXT_JUMP != -1 && context == LEGOCONTEXT_JUMP && object->action_movement_state == 3)
        return 1;
    if ((object->character_data->game_character->flags_090 & 0x8000) && LEGOCONTEXT_COMBO != 0 &&
        context == LEGOCONTEXT_COMBO)
        return 1;
    return 0;
}

__declspec(noinline) i32 __cdecl StepBackFromTarget(GAMEOBJECT *object) {
    NUVEC *target_position;
    BLOWUP_MOVEMENT_PC *blowup = (BLOWUP_MOVEMENT_PC *)object->blowup_target;
    f32 inner_distance, outer_distance, speed, distance_squared, blend;
    if (blowup != NULL) {
        target_position = &blowup->mid_position;
        if ((LEGOCONTEXT_COMBO != -1 && object->character_context == LEGOCONTEXT_COMBO) ||
            (LEGOCONTEXT_JUMP != -1 && object->character_context == LEGOCONTEXT_JUMP &&
             object->action_movement_state == 3)) {
            inner_distance = object->collision_radius * PlayerStepInnerActionPc + blowup->target_radius;
            outer_distance = object->collision_radius * PlayerStepOuterActionPc + blowup->target_radius;
        } else {
            inner_distance = object->collision_radius * PlayerStepInnerPc + blowup->target_radius;
            outer_distance = object->collision_radius * PlayerStepOuterPc + blowup->target_radius;
        }
    } else {
        if (!(object->character_data->game_character->flags_090 & 0x8000) || object->force_target == NULL)
            return 0;
        if ((LEGOCONTEXT_COMBO == -1 || object->character_context != LEGOCONTEXT_COMBO) &&
            (LEGOCONTEXT_JUMP == -1 || object->character_context != LEGOCONTEXT_JUMP ||
             object->action_movement_state != 3))
            return 0;
        target_position = &object->force_target->collision_position;
        inner_distance = object->bounds_radius * PlayerStepInnerActionPc + object->force_target->bounds_radius;
        outer_distance = object->bounds_radius * PlayerStepOuterActionPc + object->force_target->bounds_radius;
    }
    if (target_position == NULL)
        return 0;
    speed = AnimSpeed(object->character_model, object->field_7aa);
    if (speed == 0.0f)
        return 0;
    distance_squared = NuVecXZDistSqr(&object->collision_position, target_position, NULL);
    if (distance_squared >= outer_distance * outer_distance)
        return 0;
    if (inner_distance * inner_distance >= distance_squared)
        blend = 1.0f;
    else
        blend = 1.0f - (NuFsqrt(distance_squared) - inner_distance) / (outer_distance - inner_distance);
    speed = (-speed - speed) * blend + speed;
    object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1] * speed;
    object->target_velocity.z = speed * NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff];
    return 1;
}
