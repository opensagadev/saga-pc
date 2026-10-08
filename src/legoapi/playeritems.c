#include "saga.h"
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_107e) == 0x107e);
STATIC_ASSERT(offsetof(GAMEOBJECT, current_hp) == 0x107f);
STATIC_ASSERT(offsetof(PART_PC, part_id) == 0x13e);
STATIC_ASSERT(offsetof(PART_PC, active) == 0x144);
STATIC_ASSERT(sizeof(PART_PC) == 0x220);
STATIC_ASSERT(offsetof(GAMESAVE, difficulty) == 1);
i32 __cdecl ReleaseHearts(void) {
    GAMEOBJECT *player = (GAMEOBJECT *)Player[0];
    i32 total_hitpoints = 0;
    i32 missing_hitpoints = 0;
    i32 i, hitpoints, chance, adjustment;
    if (player != NULL && player->dead == 0 && player->field_107e > 0) {
        total_hitpoints = player->field_107e;
        missing_hitpoints = total_hitpoints - player->current_hp;
    }
    player = (GAMEOBJECT *)Player[1];
    if (player != NULL && player->dead == 0 && player->field_107e > 0) {
        hitpoints = player->field_107e;
        missing_hitpoints += hitpoints - player->current_hp;
        total_hitpoints += hitpoints;
    }
    if (total_hitpoints == 0)
        return 0;
    if (missing_hitpoints > 0 && MAXPARTS > 0) {
        for (i = 0; i < MAXPARTS; ++i)
            if ((Part[i].active & 1) && Part[i].part_id == 0xcb)
                --missing_hitpoints;
    }
    missing_hitpoints += (Game.difficulty <= 5 ? 8 : 9) - Game.difficulty;
    if (missing_hitpoints > total_hitpoints)
        missing_hitpoints = total_hitpoints;
    else if (missing_hitpoints < 0)
        missing_hitpoints = 0;
    chance = (missing_hitpoints << 16) / total_hitpoints;
    if (Cheat_IsOn(0x2c)) {
        adjustment = adtab[adaptivedifficulty.level][1];
        if (adjustment == 1)
            chance /= 2;
        else if (adjustment == -1)
            chance += chance;
    }
    return chance > qrand();
}

void __cdecl LoseHelmet(GAMEOBJECT *object, i32 unused1, i32 unused2) {
    i32 locator;
    NUVEC velocity;
    ADDPART_PC params;
    (void)unused1;
    (void)unused2;
    if (object->helmet == 0)
        return;
    locator = object->character_data->game_character->helmet_locator;
    if (locator != -1 && object->character_model->points_of_interest[locator] != NULL) {
        velocity.x = 0.0f;
        velocity.y = 1.0f;
        velocity.z = 0.0f;
        NuVecRotateZ(&velocity, &velocity, qrand());
        NuVecRotateX(&velocity, &velocity, qrand());
        params = Default_ADDPART;
        params.matrix = &object->locator_matrices[locator];
        params.velocity = &velocity;
        params.field_14 = 0.1f;
        params.field_18 = 0.1f;
        params.gravity = -5.0f;
        params.special = &WORLD->level_objects[object->helmet + 0xf9].special;
        params.flags = 0x90;
        params.time_step = FRAMETIME;
        params.stop_fn = PartStop_Flickerer;
        params.draw_fn = PartDraw_Flickerer;
        params.impact_fn = PartImpact_Brick;
        params.field_c4 = 1;
        AddPart(&params);
    }
    object->helmet = 0;
}

void __cdecl KeepWeaponOut(GAMEOBJECT *object) {
    object->weapon_scale = 1.0f;
    object->flags_e20 |= 0x10000;
    object->portal_flags |= 0x10;
    object->field_e33 = 0;
}

extern i32 LEGOCONTEXT_WEAPONIN, LEGOCONTEXT_WEAPONOUT;
void __cdecl SetWeaponIn(GAMEOBJECT *object) {
    i8 context = object->character_context;
    if (context != -1 && (context == LEGOCONTEXT_WEAPONIN || context == LEGOCONTEXT_WEAPONOUT))
        object->character_context = -1;
    object->weapon_scale = 0.0f;
    object->flags_e20 &= ~0x10000u;
    object->field_e33 = 0;
}

extern i32 WeaponInOut_NoAIJediSfx;
typedef struct weapon_rate_pc_s {
    u8 unknown_000[0xd40];
    f32 weapon_scale_rate;
} WEAPON_RATE_PC;
STATIC_ASSERT(offsetof(WEAPON_RATE_PC, weapon_scale_rate) == 0xd40);
void __cdecl FastWeaponIn(GAMEOBJECT *object, i32 force_sound) {
    i8 context = object->character_context;
    i32 animation;
    CHARACTERANIM_CONFIG_PC *config;
    if (context != -1 && (context == LEGOCONTEXT_WEAPONIN || context == LEGOCONTEXT_WEAPONOUT))
        object->character_context = -1;
    if (force_sound != 0 && object->weapon_scale == 1.0f && object->field_e33 != 2) {
        animation = CurrentAnim(&object->anim_packet);
        config = animation != -1 ? object->character_model->animation_configs[animation] : NULL;
        if (config == NULL || (config->flags & 0x800) == 0) {
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

extern i32(__cdecl *Fighting_WeaponInActionFn)(GAMEOBJECT *);
extern i32(__cdecl *Fighting_WeaponOutActionFn)(GAMEOBJECT *);
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
    CHARACTERANIM_CONFIG_PC *config;
    if (context != -1 && (context == LEGOCONTEXT_WEAPONIN || context == LEGOCONTEXT_WEAPONOUT))
        object->character_context = -1;
    if (force_sound != 0 && object->weapon_scale == 0.0f && object->field_e33 != 1) {
        animation = CurrentAnim(&object->anim_packet);
        config = animation != -1 ? object->character_model->animation_configs[animation] : NULL;
        if (config == NULL || (config->flags & 0x400) == 0) {
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

void __cdecl AutoWeaponOnOff(GAMEOBJECT *object) {
    i32 animation;
    u32 flags;
    i8 context = object->character_context;
    ANIMPACKET_PC *packet = &object->anim_packet;
    if (context != -1 && (context == LEGOCONTEXT_WEAPONOUT || context == LEGOCONTEXT_WEAPONIN))
        return;
    animation = CurrentAnim(packet);
    if (animation == -1)
        return;
    flags = object->character_model->animation_configs[animation]->flags;
    if ((flags & 0x400) != 0) {
        if (object->weapon_scale > 0.0f) {
            if ((packet->flags & 4) != 0 && packet->blending != 0)
                FastWeaponIn(object, 0);
            else if ((packet->flags & 4) != 0 || object->field_e33 != 2)
                SetWeaponIn(object);
        }
    } else if ((flags & 0x800) != 0 && object->weapon_scale < 1.0f) {
        if ((packet->flags & 4) != 0 && packet->blending != 0)
            FastWeaponOut(object, 0);
        else if ((packet->flags & 4) != 0 || object->field_e33 != 1)
            SetWeaponOut(object);
    }
}

void __cdecl RegenerateHearts(GAMEOBJECT *object) {
    if (object->current_hp > 0 && object->current_hp < object->field_107e && object->vehicle_hit_timer <= 0.0f &&
        object->spawn_protection_timer <= 0.0f && ParticlesPerSecond(1.0f, FRAMETIME) > 0) {
        ++object->current_hp;
        GameAudio_PlaySfx(0x27, &object->collision_position, 0, 0);
    }
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
