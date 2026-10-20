#include "saga.h"

extern i16 id_JUMBOHOMINGDROID;
void __cdecl UpdateCharacterIdle(GAMEOBJECT *);

STATIC_ASSERT(offsetof(GAMEOBJECT, anim_packet) == 8);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, requested_animation) == 0x3a);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7aa) == 0x7aa);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(offsetof(GAMEOBJECT, previous_movement_angle) == 0x1060);
STATIC_ASSERT(offsetof(GAMEPAD_PC, runtime_flags) == 0x5a);
STATIC_ASSERT(offsetof(GAMEPAD_PC, input_magnitude) == 0x28);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, game_character) == 0x24);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, action_animations) == 0xc);

void __cdecl Animate_DEFAULT(GAMEOBJECT *object) {
    object->anim_packet.requested_animation = 1;
}

void __cdecl Animate_CANNON(GAMEOBJECT *object) {
    object->anim_packet.requested_animation =
        (CInfo[object->character_context].flags & 0x10) != 0 ? object->field_7aa : 1;
}

void __cdecl Animate_SPEEDERBIKE(GAMEOBJECT *object) {
    object->anim_packet.requested_animation =
        (CInfo[object->character_context].flags & 0x10) != 0 ? object->field_7aa : 1;
}

void __cdecl Animate_WALKER(GAMEOBJECT *object) {
    GAMEPAD_PC *pad = object->pad;
    if ((CInfo[object->character_context].flags & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
        return;
    }
    object->anim_packet.requested_animation = 1;
    if ((pad->runtime_flags & 2) == 0 && pad->input_magnitude > 0.0f)
        object->anim_packet.requested_animation = 0;
}

void __cdecl Animate_HOVERDROID(GAMEOBJECT *object) {
    if ((CInfo[object->character_context].flags & 0x10) != 0)
        object->anim_packet.requested_animation = object->field_7aa;
    else if (object->character_context == 30 && object->character_model->action_animations[5] != NULL)
        object->anim_packet.requested_animation = 5;
    else
        object->anim_packet.requested_animation = 1;
}

void __cdecl Animate_VEHICLE(GAMEOBJECT *object) {
    if ((CInfo[object->character_context].flags & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
        return;
    }
    if ((object->character_data->game_character->flags_090 & 0x100000) != 0) {
        object->anim_packet.requested_animation = 3;
        return;
    }
    if ((object->id == (u16)id_STAP || object->id == (u16)id_STAP2 || object->id == (u16)id_JUMBOHOMINGDROID) &&
        object->pad->input_magnitude > 0.0f && object->character_model->action_animations[0] != NULL) {
        object->anim_packet.requested_animation = 0;
        return;
    }
    object->anim_packet.requested_animation = 1;
}

void __cdecl Animate_ATAT(GAMEOBJECT *object) {
    GAMEPAD_PC *pad = object->pad;
    if ((CInfo[object->character_context].flags & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
    } else {
        object->anim_packet.requested_animation = 1;
        if ((object->flags_1fc & 0x80) != 0 && object->character_model->action_animations[15] != NULL)
            object->anim_packet.requested_animation = 15;
        if ((pad->runtime_flags & 2) == 0 && pad->input_magnitude > 0.0f)
            object->anim_packet.requested_animation = 0;
    }
    if (object->anim_packet.requested_animation == 1) {
        i32 turn = RotDiff(object->previous_movement_angle, object->supporting_platform_id);
        if (turn < 0 && object->character_model->action_animations[38] != NULL) {
            object->anim_packet.requested_animation = 38;
            return;
        }
        if (turn > 0 && object->character_model->action_animations[79] != NULL)
            object->anim_packet.requested_animation = 79;
    }
}

void __cdecl Animate_REPUBLICGUNSHIP(GAMEOBJECT *object) {
    object->anim_packet.requested_animation = 1;
    if (object->character_context == 35)
        object->anim_packet.requested_animation = 44;
    else if (object->character_context == 36)
        object->anim_packet.requested_animation = 45;
    UpdateCharacterIdle(object);
}

void __cdecl Animate_BARMAN(GAMEOBJECT *object) {
    object->anim_packet.requested_animation = 1;
    if ((object->pad->runtime_flags & 2) == 0 && object->pad->input_magnitude > 0.0f)
        object->anim_packet.requested_animation = 0;
    UpdateCharacterIdle(object);
}

typedef struct default_animation_timers_pc_s {
    u8 unknown_0000[0xd24];
    f32 ground_contact_grace;
    u8 unknown_0d28[0x78];
    f32 nearby_floor_distance;
    u8 unknown_0da4[8];
    f32 fall_animation_timer;
} DEFAULT_ANIMATION_TIMERS_PC;
STATIC_ASSERT(offsetof(DEFAULT_ANIMATION_TIMERS_PC, ground_contact_grace) == 0xd24);
STATIC_ASSERT(offsetof(DEFAULT_ANIMATION_TIMERS_PC, nearby_floor_distance) == 0xda0);
STATIC_ASSERT(offsetof(DEFAULT_ANIMATION_TIMERS_PC, fall_animation_timer) == 0xdac);
STATIC_ASSERT(sizeof(DEFAULT_ANIMATION_TIMERS_PC) == 0xdb0);
extern f32 JumpAnimHoldTimePc;
i32 __cdecl UseFallAnim(GAMEOBJECT *);

static void SetDefaultCharacterAnimPc(GAMEOBJECT *object) {
    DEFAULT_ANIMATION_TIMERS_PC *timers = (DEFAULT_ANIMATION_TIMERS_PC *)object;
    object->anim_packet.requested_animation = 5;
    if (object->character_context == 0x2b)
        return;
    if (object->field_281 != 0 || timers->ground_contact_grace > 0.0f ||
        object->character_model->model_data_b[5] == NULL ||
        (timers->fall_animation_timer < (f64)0.2f && timers->nearby_floor_distance != 2000000.0 &&
         timers->nearby_floor_distance < 0.25f && object->velocity.y < 0.0f)) {
        if (!(object->character_data->game_character->field_028 > 0.0f) || object->field_281 != 0 ||
            object->character_model->model_data_b[5] == NULL)
            object->anim_packet.requested_animation = (i16)GetDefaultIdle(object);
    }
}

static void JumpAnimCode(GAMEOBJECT *object) {
    DEFAULT_ANIMATION_TIMERS_PC *timers = (DEFAULT_ANIMATION_TIMERS_PC *)object;
    if ((object->context_variant_flags & 0x8000) != 0) {
        if (object->action_movement_state == 8 && object->character_model->model_data_b[0xb5] != NULL) {
            object->anim_packet.requested_animation = 0xb5;
            return;
        }
        if (object->character_model->model_data_b[5] != NULL) {
            object->anim_packet.requested_animation = 5;
            return;
        }
    }
    object->anim_packet.requested_animation = object->field_7aa;
    if ((object->context_variant_flags & 0x8000) != 0 || object->field_7aa == 0x49)
        return;
    if (object->action_movement_state != 0 && object->action_movement_state != 1 &&
        object->action_movement_state != 6 && object->action_movement_state != 7 && object->action_movement_state != 9)
        return;
    if (object->anim_packet.blending == 0 && object->anim_packet.animation_index == object->field_7aa &&
        (object->anim_packet.flags & 1) != 0) {
        if (JumpAnimHoldTimePc > 0.0f)
            object->airborne_input_timer += FRAMETIME;
        else
            object->context_variant_flags |= 0x8000;
    } else
        object->airborne_input_timer = 0.0f;
    if (JumpAnimHoldTimePc > 0.0f && object->airborne_input_timer >= JumpAnimHoldTimePc &&
        (timers->nearby_floor_distance == 2000000.0 || timers->nearby_floor_distance > 0.35f))
        object->context_variant_flags |= 0x8000;
}

static void MoveAnim_Manage(GAMEOBJECT *object, f32 speed, i32 allow_tiptoe, i32 weapon_variant) {
    PLAYERCHARACTERCONFIG_PC *runtime = object->character_data->game_character;
    i32 attempt;
    if (allow_tiptoe != 0 && speed <= ((f64)runtime->tiptoe_speed + runtime->walk_speed) * 0.5) {
        object->anim_packet.requested_animation = weapon_variant != 0 &&
                                                          (object->weapon_scale == 1.0f || object->field_e33 == 1) &&
                                                          object->character_model->model_data_b[0x3f] != NULL
                                                      ? 0x3f
                                                      : 4;
    } else if (speed <= ((f64)runtime->walk_speed + runtime->run_speed) * 0.5) {
        if (weapon_variant != 0 && (object->weapon_scale == 1.0f || object->field_e33 == 1) &&
            object->character_model->model_data_b[0x40] != NULL)
            object->anim_packet.requested_animation = 0x40;
        else if (object->character_model->model_data_b[0x50] != NULL && (object->flags_ef0 & 0x4000) != 0)
            object->anim_packet.requested_animation = 0x50;
        else
            object->anim_packet.requested_animation = 0;
    } else
        object->anim_packet.requested_animation = weapon_variant != 0 &&
                                                          (object->weapon_scale == 1.0f || object->field_e33 == 1) &&
                                                          object->character_model->model_data_b[0x17] != NULL
                                                      ? 0x17
                                                      : 3;
    if (object->suit != NULL && (object->suit->flags & 0x40) != 0 &&
        !(object->character_context == 0x43 && object->context_variant == 5)) {
        if (object->anim_packet.requested_animation == 4 && object->character_model->model_data_b[0xc6] != NULL)
            object->anim_packet.requested_animation = 0xc6;
        else if (object->anim_packet.requested_animation == 0 && object->character_model->model_data_b[0xc7] != NULL)
            object->anim_packet.requested_animation = 0xc7;
        else if (object->anim_packet.requested_animation == 3 && object->character_model->model_data_b[0xc8] != NULL)
            object->anim_packet.requested_animation = 0xc8;
    }
    for (attempt = 0;
         object->character_model->model_data_b[object->anim_packet.requested_animation] == NULL && attempt < 3;
         ++attempt) {
        if (object->anim_packet.requested_animation == 4)
            object->anim_packet.requested_animation = 0;
        else if (object->anim_packet.requested_animation == 0)
            object->anim_packet.requested_animation = 3;
        else if (object->anim_packet.requested_animation == 3)
            object->anim_packet.requested_animation = 0;
    }
}

static void MoveAnim_Check(GAMEOBJECT *object) {
    i32 walk = object->character_model->model_data_b[0] != NULL;
    i32 run = object->character_model->model_data_b[3] != NULL;
    if (walk) {
        if (!run || object->pad->input_magnitude <= ((f64)object->character_data->game_character->walk_speed +
                                                     object->character_data->game_character->run_speed) *
                                                        0.5)
            object->anim_packet.requested_animation = 0;
        else
            object->anim_packet.requested_animation = 3;
    } else if (run)
        object->anim_packet.requested_animation = 3;
}

static void SelfDestructCode(GAMEOBJECT *object, i32 pressed) {
    struct EXPLOSION *explosion;
    if (((i8)(object->flags_1fc)) >= 0 || object->dead != 0 || object->character_context == 0x0b ||
        object->character_context == 0x16 || object->character_context == 0x2b || pressed == 0)
        return;
    InstantKillParts(object, 1, 0.0f);
    explosion = Detonate(&object->collision_position, 0);
    if (explosion != NULL && Arcade != 0 && ((u8)(object->field_280)) <= 1) {
        explosion->field_0x24 |= 0x10000;
        explosion->object = object;
    }
    KillPlayer(object, 2, 1, NULL);
    if (object->camera_screen_position.z > 0.0f)
        AddFancyMessage(TTab[tCHEAT_SELFDESTRUCT], object->camera_screen_position.x, object->camera_screen_position.y,
                        0.25f, 1.0f, 1, 0);
}

/* Same original motion owner; merge with the genuine static default/Manage helpers. */
typedef struct CHARACTER_FALL_ANIMATION_READER_PC {
    u8 unknown_000[0xdac];
    f32 fall_animation_timer;
} CHARACTER_FALL_ANIMATION_READER_PC;
STATIC_ASSERT(offsetof(CHARACTER_FALL_ANIMATION_READER_PC, fall_animation_timer) == 0xdac);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_target_position) == 0xeac);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);
STATIC_ASSERT(offsetof(GAMEOBJECT, weapon_scale) == 0xd3c);

void __cdecl Animate_ASTROMECH(GAMEOBJECT *object) {
    CHARACTER_FALL_ANIMATION_READER_PC *state = (CHARACTER_FALL_ANIMATION_READER_PC *)object;
    i16 animation;
    if ((CInfo[object->character_context].flags & 0x10) != 0)
        object->anim_packet.requested_animation = object->field_7aa;
    else if (object->context_target_position != NULL)
        object->anim_packet.requested_animation = object->character_model->action_animations[43] != NULL  ? 43
                                                  : object->character_model->action_animations[5] != NULL ? 5
                                                                                                          : 1;
    else {
        SetDefaultCharacterAnimPc(object);
        if (object->character_context == 0)
            object->anim_packet.requested_animation = object->pad->input_magnitude > 0.0f ? 37 : 36;
        else if (UseFallAnim(object))
            object->anim_packet.requested_animation = 5;
        else if (object->anim_packet.requested_animation != 5 && (object->pad->runtime_flags & 2) == 0 &&
                 object->pad->input_magnitude > 0.0f)
            object->anim_packet.requested_animation = 0;
    }
    UpdateCharacterIdle(object);
    animation = object->anim_packet.requested_animation;
    if (animation == 5 ||
        ((object->character_data->model_flags & 0x20) != 0 && (animation == 40 || animation == 75 || animation == 76)))
        state->fall_animation_timer += FRAMETIME;
    else
        state->fall_animation_timer = 0.0f;
}

void __cdecl Animate_DROIDEKA(GAMEOBJECT *object) {
    CHARACTER_FALL_ANIMATION_READER_PC *state = (CHARACTER_FALL_ANIMATION_READER_PC *)object;
    i16 animation;
    if ((CInfo[object->character_context].flags & 0x10) != 0)
        object->anim_packet.requested_animation = object->field_7aa;
    else {
        SetDefaultCharacterAnimPc(object);
        if (UseFallAnim(object) || (object->character_context == -1 && object->field_281 == 0))
            object->anim_packet.requested_animation = 5;
        else if ((object->pad->runtime_flags & 2) == 0 && object->pad->input_magnitude > 0.0f) {
            PLAYERCHARACTERCONFIG_PC *character = object->character_data->game_character;
            object->anim_packet.requested_animation =
                ((f64)character->tiptoe_speed + (f64)character->walk_speed) * 0.5 >= object->pad->input_magnitude ? 4
                                                                                                                  : 0;
        }
    }
    animation = object->anim_packet.requested_animation;
    if (animation == 5 ||
        ((object->character_data->model_flags & 0x20) != 0 && (animation == 40 || animation == 75 || animation == 76)))
        state->fall_animation_timer += FRAMETIME;
    else
        state->fall_animation_timer = 0.0f;
}

void __cdecl Animate_GEONOSIAN(GAMEOBJECT *object) {
    CHARACTER_FALL_ANIMATION_READER_PC *state = (CHARACTER_FALL_ANIMATION_READER_PC *)object;
    i16 animation;
    if ((CInfo[object->character_context].flags & 0x10) != 0)
        object->anim_packet.requested_animation = object->field_7aa;
    else {
        SetDefaultCharacterAnimPc(object);
        if (UseFallAnim(object))
            object->anim_packet.requested_animation = 5;
        else if (object->character_context == -1 && object->jump_variant_state == 1)
            object->anim_packet.requested_animation = object->pad->input_magnitude > 0.0f ? 37 : 15;
        else if (object->anim_packet.requested_animation != 5 && (object->pad->runtime_flags & 2) == 0 &&
                 object->pad->input_magnitude > 0.0f)
            object->anim_packet.requested_animation = 0;
    }
    UpdateCharacterIdle(object);
    animation = object->anim_packet.requested_animation;
    if (animation == 5 ||
        ((object->character_data->model_flags & 0x20) != 0 && (animation == 40 || animation == 75 || animation == 76)))
        state->fall_animation_timer += FRAMETIME;
    else
        state->fall_animation_timer = 0.0f;
}

void __cdecl Animate_BATTLEDROID(GAMEOBJECT *object) {
    CHARACTER_FALL_ANIMATION_READER_PC *state = (CHARACTER_FALL_ANIMATION_READER_PC *)object;
    i16 animation;
    if ((CInfo[object->character_context].flags & 0x10) != 0)
        object->anim_packet.requested_animation = object->field_7aa;
    else if (object->context_target_position != NULL)
        object->anim_packet.requested_animation = object->character_model->action_animations[43] != NULL  ? 43
                                                  : object->character_model->action_animations[5] != NULL ? 5
                                                                                                          : 1;
    else {
        SetDefaultCharacterAnimPc(object);
        if (UseFallAnim(object))
            object->anim_packet.requested_animation = 5;
        else if ((object->pad->runtime_flags & 2) == 0 && object->pad->input_magnitude > 0.0f)
            MoveAnim_Manage(object, object->pad->input_magnitude,
                            (object->character_data->game_character->flags_090 & 8) == 0, 0);
    }
    animation = object->anim_packet.requested_animation;
    if (animation == 5 ||
        ((object->character_data->model_flags & 0x20) != 0 && (animation == 40 || animation == 75 || animation == 76)))
        state->fall_animation_timer += FRAMETIME;
    else
        state->fall_animation_timer = 0.0f;
}

void __cdecl Animate_SUPERBATTLEDROID(GAMEOBJECT *object) {
    CHARACTER_FALL_ANIMATION_READER_PC *state = (CHARACTER_FALL_ANIMATION_READER_PC *)object;
    i16 animation;
    if ((CInfo[object->character_context].flags & 0x10) != 0)
        object->anim_packet.requested_animation = object->field_7aa;
    else if (object->context_target_position != NULL)
        object->anim_packet.requested_animation = object->character_model->action_animations[43] != NULL  ? 43
                                                  : object->character_model->action_animations[5] != NULL ? 5
                                                                                                          : 1;
    else {
        SetDefaultCharacterAnimPc(object);
        if (UseFallAnim(object))
            object->anim_packet.requested_animation = 5;
        else if (object->anim_packet.requested_animation != 5) {
            if ((object->pad->runtime_flags & 2) == 0 && object->pad->input_magnitude > 0.0f) {
                PLAYERCHARACTERCONFIG_PC *character = object->character_data->game_character;
                if (((f64)character->walk_speed + (f64)character->run_speed) * 0.5 >= object->pad->input_magnitude)
                    object->anim_packet.requested_animation =
                        (object->flags_e20 & 0x10000) != 0 || object->weapon_scale > 0.0f ? 64 : 0;
                else
                    object->anim_packet.requested_animation =
                        (object->flags_e20 & 0x10000) != 0 || object->weapon_scale > 0.0f ? 23 : 3;
            } else if (object->character_model->action_animations[25] != NULL && object->weapon_scale >= 0.5f)
                object->anim_packet.requested_animation = 25;
        }
    }
    UpdateCharacterIdle(object);
    animation = object->anim_packet.requested_animation;
    if (animation == 5 ||
        ((object->character_data->model_flags & 0x20) != 0 && (animation == 40 || animation == 75 || animation == 76)))
        state->fall_animation_timer += FRAMETIME;
    else
        state->fall_animation_timer = 0.0f;
}

void PeriscodeCode(GAMEOBJECT *object) {
    f32 previous;
    f32 target;
    if ((object->character_data->model_flags & 0x40) == 0)
        return;

    previous = object->communicate_blend;
    target = 0.0f;
    if (object->field_283 == 9 && object->dead == 0 && WORLD->level_objects[0x19].active != 0) {
        i32 locator = object->character_data->game_character->thingy_locator;
        if (locator != -1 && object->character_model->points_of_interest[locator] != NULL &&
            object->water_height > object->locator_matrices[locator].m31) {
            if (object->roof_height_pc == 2000000.0f) {
                target = 1.0f;
            } else {
                f32 clearance = object->roof_height_pc - object->water_height;
                if (!(clearance > 0.0f) || !(clearance < 0.26f))
                    target = 1.0f;
            }
        }
    }
    object->communicate_blend = SeekLinearF(previous, target, 5.0f * FRAMETIME);
    if (previous == 0.0f && object->communicate_blend > 0.0f)
        PlaySfx("drd_r2_scope_up", &object->upper_position);
    else if (previous == 1.0f && object->communicate_blend < 1.0f)
        PlaySfx("drd_r2_scope_down", &object->upper_position);

    if (object->field_db0 > 0.0f) {
        object->field_db0 -= FRAMETIME;
        if (!(object->field_db0 <= 0.0f))
            return;
        if (object->dead == 0)
            PlaySfx(((char *)(object->id == (u16)id_R2Q5 ? "R2Q501" : "R2D201")), &object->collision_position);
    }
    object->field_db0 = qrand() * 1.5259022e-05f * 10.0f + 20.0f;
}

i32 __cdecl SetProtocolDroidFallAnim(GAMEOBJECT *object);
STATIC_ASSERT(offsetof(GAMEOBJECT, saved_hitpoints) == 0xe39);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e24) == 0xe24);

void __cdecl Animate_BEAST(GAMEOBJECT *object) {
    CHARACTER_FALL_ANIMATION_READER_PC *state = (CHARACTER_FALL_ANIMATION_READER_PC *)object;
    GAMEPAD_PC *pad = object->pad;
    i16 animation;
    if ((CInfo[object->character_context].flags & 0x10) != 0)
        object->anim_packet.requested_animation = object->field_7aa;
    else {
        SetDefaultCharacterAnimPc(object);
        if (UseFallAnim(object))
            object->anim_packet.requested_animation = 5;
        else if (object->anim_packet.requested_animation != 5 && (object->pad->runtime_flags & 2) == 0 &&
                 pad->input_magnitude > 0.0f) {
            MoveAnim_Check(object);
            if (object->anim_packet.requested_animation == 0 && (object->flags_e24 & 1) != 0)
                object->anim_packet.requested_animation = 64;
        }
        if (object->anim_packet.requested_animation == 5 && object->character_model->action_animations[5] == NULL)
            object->anim_packet.requested_animation = 1;
    }
    UpdateCharacterIdle(object);
    animation = object->anim_packet.requested_animation;
    if (animation == 5 ||
        ((object->character_data->model_flags & 0x20) != 0 && (animation == 40 || animation == 75 || animation == 76)))
        state->fall_animation_timer += FRAMETIME;
    else
        state->fall_animation_timer = 0.0f;
}

void __cdecl Animate_CRITTER(GAMEOBJECT *object) {
    if ((CInfo[object->character_context].flags & 0x10) != 0) {
        object->anim_packet.requested_animation = object->field_7aa;
        return;
    }
    if (object->character_context == 30 && object->character_model->action_animations[5] != NULL) {
        object->anim_packet.requested_animation = 5;
        return;
    }
    SetDefaultCharacterAnimPc(object);
    if (UseFallAnim(object)) {
        object->anim_packet.requested_animation = 5;
        return;
    }
    if (object->anim_packet.requested_animation != 5 && (object->pad->runtime_flags & 2) == 0 &&
        object->pad->input_magnitude > 0.0f)
        MoveAnim_Check(object);
}

void __cdecl Animate_PROTOCOL(GAMEOBJECT *object) {
    CHARACTER_FALL_ANIMATION_READER_PC *state = (CHARACTER_FALL_ANIMATION_READER_PC *)object;
    i32 falling = 0;
    i16 animation;
    if ((CInfo[object->character_context].flags & 0x10) != 0)
        object->anim_packet.requested_animation = object->field_7aa;
    else if (object->context_target_position != NULL) {
        switch (object->saved_hitpoints) {
            case 1:
                object->anim_packet.requested_animation = 76;
                break;
            case 2:
                object->anim_packet.requested_animation = 75;
                break;
            case 3:
                object->anim_packet.requested_animation = 40;
                break;
            default:
                object->anim_packet.requested_animation = 5;
                break;
        }
    } else {
        SetDefaultCharacterAnimPc(object);
        if (object->anim_packet.requested_animation == 5) {
            object->anim_packet.requested_animation = (i16)SetProtocolDroidFallAnim(object);
            falling = 1;
        } else {
            switch (object->saved_hitpoints) {
                case 1:
                    object->anim_packet.requested_animation = 8;
                    break;
                case 2:
                    object->anim_packet.requested_animation = 20;
                    break;
                case 3:
                    object->anim_packet.requested_animation = 15;
                    break;
                default:
                    object->anim_packet.requested_animation = 1;
                    break;
            }
        }
        if (UseFallAnim(object))
            object->anim_packet.requested_animation = 5;
        else if (!falling && (object->pad->runtime_flags & 2) == 0 && object->pad->input_magnitude > 0.0f) {
            switch (object->saved_hitpoints) {
                case 1:
                    object->anim_packet.requested_animation = 29;
                    break;
                case 2:
                    object->anim_packet.requested_animation = 23;
                    break;
                case 3:
                    object->anim_packet.requested_animation = 3;
                    break;
                default:
                    object->anim_packet.requested_animation = 0;
                    break;
            }
        }
    }
    if (object->anim_packet.requested_animation == 5) {
        switch (object->saved_hitpoints) {
            case 1:
                object->anim_packet.requested_animation = 76;
                break;
            case 2:
                object->anim_packet.requested_animation = 75;
                break;
            case 3:
                object->anim_packet.requested_animation = 40;
                break;
            default:
                object->anim_packet.requested_animation = 5;
                break;
        }
    }
    UpdateCharacterIdle(object);
    animation = object->anim_packet.requested_animation;
    if (animation == 5 ||
        ((object->character_data->model_flags & 0x20) != 0 && (animation == 40 || animation == 75 || animation == 76)))
        state->fall_animation_timer += FRAMETIME;
    else
        state->fall_animation_timer = 0.0f;
}

STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, weapon_model) == 0xec);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, hover_fall_mode) == 0x116);

/* PC EAX tail helper shared by both callbacks; AA retains this sound block inline. */
static void AnimationWeaponEffectPc(GAMEOBJECT *object) {
    char *name;
    i32 sound;
    if (object->id == (u16)id_IMPERIALGUARD || !(object->weapon_scale > 0.0f))
        return;
    if (object->id == (u16)id_BODYGUARD)
        name = "Grv_GuardWeaponLp";
    else if ((object->character_data->model_flags & 4) != 0)
        name = "SaberLoopB";
    else
        name = "SaberLoopJ";
    sound = GetSfxId(name);
    PlaySfxByIdAndSetVolume(sound, &object->collision_position, object->weapon_scale);
}

void __cdecl Animate_WEIRDO(GAMEOBJECT *object) {
    CHARACTER_FALL_ANIMATION_READER_PC *state = (CHARACTER_FALL_ANIMATION_READER_PC *)object;
    i16 animation;
    i16 weapon;
    if ((CInfo[object->character_context].flags & 0x10) != 0)
        object->anim_packet.requested_animation = object->field_7aa;
    else {
        SetDefaultCharacterAnimPc(object);
        if (object->character_context == 0)
            JumpAnimCode(object);
        else if (UseFallAnim(object))
            object->anim_packet.requested_animation = 5;
        else if (object->character_context == 27 && (object->flags_e20 & 0x580) == 0)
            object->anim_packet.requested_animation = object->character_model->action_animations[39] != NULL ? 39 : 11;
        else if (object->character_context == 27 || object->character_context == 29 ||
                 object->character_context == 18 || object->character_context == 8)
            object->anim_packet.requested_animation = ((object->flags_e20 & 0x10000) != 0 || object->field_e33 == 1) &&
                                                              object->character_model->action_animations[39] != NULL
                                                          ? 39
                                                          : 11;
        else if (object->anim_packet.requested_animation != 5) {
            if ((object->pad->runtime_flags & 2) == 0 && object->pad->input_magnitude > 0.0f)
                MoveAnim_Manage(object, object->pad->input_magnitude, 1, 1);
            else if ((object->flags_e20 & 0x10000) != 0 || object->field_e33 == 1) {
                if (object->character_data->game_character->hover_fall_mode == 0 &&
                    (object->character_data->model_flags & 0x80) != 0) {
                    if (object->character_model->action_animations[118] != NULL)
                        object->anim_packet.requested_animation = 118;
                } else if (object->character_model->action_animations[25] != NULL)
                    object->anim_packet.requested_animation = 25;
            }
        }
        if (object->character_data->game_character->hover_fall_mode == 0 &&
            (object->character_data->model_flags & 0x80) != 0) {
            if (object->anim_packet.requested_animation == 23)
                object->anim_packet.requested_animation = 115;
            else if (object->anim_packet.requested_animation == 64)
                object->anim_packet.requested_animation = 114;
        }
    }
    UpdateCharacterIdle(object);
    animation = object->anim_packet.requested_animation;
    if (animation == 5 ||
        ((object->character_data->model_flags & 0x20) != 0 && (animation == 40 || animation == 75 || animation == 76)))
        state->fall_animation_timer += FRAMETIME;
    else
        state->fall_animation_timer = 0.0f;
    weapon = object->character_data->game_character->weapon_model;
    if (weapon == 101 || weapon == 103 || weapon == 105 || weapon == 107)
        AnimationWeaponEffectPc(object);
}

void __cdecl Animate_JEDI(GAMEOBJECT *object) {
    CHARACTER_FALL_ANIMATION_READER_PC *state = (CHARACTER_FALL_ANIMATION_READER_PC *)object;
    i16 animation;
    if ((object->flags_e20 & 0x1000000) != 0)
        object->anim_packet.requested_animation = ((object->flags_e20 & 0x10000) != 0 || object->field_e33 == 1) &&
                                                          object->character_model->action_animations[39] != NULL
                                                      ? 39
                                                      : 11;
    else if ((CInfo[object->character_context].flags & 0x10) != 0)
        object->anim_packet.requested_animation = object->field_7aa;
    else {
        SetDefaultCharacterAnimPc(object);
        if (object->character_context == 0)
            JumpAnimCode(object);
        else if (UseFallAnim(object))
            object->anim_packet.requested_animation = 5;
        else if (object->character_context == 27 && (object->flags_e20 & 0x580) == 0)
            object->anim_packet.requested_animation = object->character_model->action_animations[39] != NULL ? 39 : 11;
        else if (object->character_context == 27 || object->character_context == 29 ||
                 object->character_context == 18 || object->character_context == 8)
            object->anim_packet.requested_animation = ((object->flags_e20 & 0x10000) != 0 || object->field_e33 == 1) &&
                                                              object->character_model->action_animations[39] != NULL
                                                          ? 39
                                                          : 11;
        else if (object->anim_packet.requested_animation != 5) {
            if ((object->pad->runtime_flags & 2) == 0 && object->pad->input_magnitude > 0.0f) {
                if (object->id == (u16)id_YODA || object->id == (u16)id_YODAGHOST)
                    object->anim_packet.requested_animation =
                        (object->weapon_scale == 1.0f || object->field_e33 == 1) &&
                                object->character_model->action_animations[63] != NULL
                            ? 63
                            : 4;
                else
                    MoveAnim_Manage(object, object->pad->input_magnitude,
                                    (object->character_data->game_character->flags_090 & 8) == 0, 1);
            } else if (((object->flags_e20 & 0x10000) != 0 || object->field_e33 == 1) &&
                       object->character_model->action_animations[25] != NULL)
                object->anim_packet.requested_animation = 25;
        }
        if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object)) {
            switch (object->anim_packet.requested_animation) {
                case 0:
                    object->anim_packet.requested_animation = 114;
                    break;
                case 1:
                    object->anim_packet.requested_animation = 117;
                    break;
                case 3:
                    object->anim_packet.requested_animation = 115;
                    break;
                case 4:
                    object->anim_packet.requested_animation = 113;
                    break;
                case 5:
                    object->anim_packet.requested_animation = 116;
                    break;
                default:
                    break;
            }
        }
    }
    UpdateCharacterIdle(object);
    animation = object->anim_packet.requested_animation;
    if (animation == 5 ||
        ((object->character_data->model_flags & 0x20) != 0 && (animation == 40 || animation == 75 || animation == 76)))
        state->fall_animation_timer += FRAMETIME;
    else
        state->fall_animation_timer = 0.0f;
    AnimationWeaponEffectPc(object);
}

typedef struct default_animation_mini_request_pc_s {
    u8 unknown_0000[0x7d0];
    DRAW_ATTACHMENTS_MINI_PC packet;
} DEFAULT_ANIMATION_MINI_REQUEST_PC;
STATIC_ASSERT(offsetof(DEFAULT_ANIMATION_MINI_REQUEST_PC, packet.requested_animation) == 0x7f2);
STATIC_ASSERT(sizeof(DEFAULT_ANIMATION_MINI_REQUEST_PC) == 0x7f4);
extern i16 DefaultAnimationJabbaIdPc, DefaultAnimationLeiaSlaveIdPc;
extern i16 DefaultAnimationLamaSuIdPc, DefaultAnimationTaunWeIdPc;

void __cdecl Animate_CHARACTER(GAMEOBJECT *object) {
    DEFAULT_ANIMATION_TIMERS_PC *timers = (DEFAULT_ANIMATION_TIMERS_PC *)object;
    DEFAULT_ANIMATION_MINI_REQUEST_PC *mini = (DEFAULT_ANIMATION_MINI_REQUEST_PC *)object;
    i16 animation, model_id;
    if (object->id == (u16)DefaultAnimationJabbaIdPc && WORLD->current_level == HUB_LDATA) {
        object->anim_packet.requested_animation = 1;
        if (object->pad->input_magnitude > 0.0f && object->character_model->model_data_b[0] != NULL)
            object->anim_packet.requested_animation = 0;
        else if (object->character_model->model_data_b[15] != NULL && HubJabbaAwakePc > 0.0f)
            object->anim_packet.requested_animation = 15;
        return;
    }
    if ((CInfo[object->character_context].flags & 0x10) != 0)
        object->anim_packet.requested_animation = object->field_7aa;
    else if (object->context_target_position != NULL) {
        if (object->character_model->model_data_b[43] != NULL)
            object->anim_packet.requested_animation = 43;
        else
            object->anim_packet.requested_animation = object->character_model->model_data_b[5] != NULL ? 5 : 1;
    } else if (object->character_context == 8)
        object->anim_packet.requested_animation = 11;
    else {
        SetDefaultCharacterAnimPc(object);
        if (object->character_context == 0)
            JumpAnimCode(object);
        else if (UseFallAnim(object))
            object->anim_packet.requested_animation = 5;
        else if (object->character_context == -1 && object->jump_variant_state == 1)
            object->anim_packet.requested_animation = object->pad->input_magnitude > 0.0f ? 37 : 15;
        else if (object->anim_packet.requested_animation != 5) {
            if ((object->pad->runtime_flags & 2) == 0 && object->pad->input_magnitude > 0.0f) {
                i32 tiptoe =
                    (object->flags_1fc & 0x80) == 0 && object->character_data->game_character->weapon_action == 1
                        ? 0
                        : (object->character_data->game_character->flags_090 & 8) == 0;
                MoveAnim_Manage(object, object->pad->input_magnitude, tiptoe, 1);
            } else if (((object->flags_e20 & 0x10000) != 0 || object->field_e33 == 1) &&
                       object->character_model->model_data_b[25] != NULL)
                object->anim_packet.requested_animation = 25;
        }
    }
    if ((object->flags_1fc & 0x80) != 0 && (object->pad->buttons_held & GAMEPAD_SPECIAL) != 0) {
        if (object->id == (u16)id_GAMORREANGUARD) {
            if (object->anim_packet.requested_animation == 25)
                object->anim_packet.requested_animation = 15;
        } else if (object->id == (u16)DefaultAnimationLeiaSlaveIdPc) {
            if (object->anim_packet.requested_animation == 1)
                object->anim_packet.requested_animation = 96;
        } else if ((object->id == (u16)DefaultAnimationLamaSuIdPc || object->id == (u16)DefaultAnimationTaunWeIdPc) &&
                   object->anim_packet.requested_animation == 1)
            object->anim_packet.requested_animation = 15;
    }
    UpdateCharacterIdle(object);
    animation = object->anim_packet.requested_animation;
    if (animation == 5 || ((object->character_data->model_flags & 0x20) != 0 &&
                           (animation == 0x28 || animation == 0x4b || animation == 0x4c)))
        timers->fall_animation_timer += FRAMETIME;
    else
        timers->fall_animation_timer = 0.0f;
    if (object->id == (u16)AreaExtraSourceId7f17c8Pc) {
        model_id = apicharsys->playermodelids[AreaExtraTargetId7f17ccPc];
        if (model_id == -1)
            return;
        mini->packet.requested_animation =
            animation != -1 && apicharsys->models[model_id].model_data_b[animation] != NULL ? animation : 1;
        UpdateMiniAnimPacket(&apicharsys->models[model_id], &mini->packet, (f32)((f64)FRAMETIME * 30.0), 0.0f,
                             FRAMETIME);
    } else if (object->suit != NULL && (object->suit->flags & 2) != 0) {
        model_id = apicharsys->playermodelids[AreaExtraTargetId7f17bcPc];
        if (model_id == -1)
            return;
        mini->packet.requested_animation = object->character_context == 0x4f                                      ? 45
                                           : object->character_context == 0 && object->action_movement_state == 4 ? 33
                                           : object->character_context == 14                                      ? 34
                                                                                                                  : 44;
        UpdateMiniAnimPacket(&apicharsys->models[model_id], &mini->packet, (f32)((f64)FRAMETIME * 30.0), 0.0f,
                             FRAMETIME);
    }
}

extern i16 id_ANAKINSPOD, id_ANAKINSPODGREEN;
extern f32 pod_animtime[2], podanimendframe;
void __cdecl SetPodMergeAnims(ANIMPACKET_PC *, i32);
i32 __cdecl PodRace_InStartCountdown(WORLDINFO *);
extern AREA_DATA_PC *PODSPRINT_ADATA;
STATIC_ASSERT(offsetof(GAMEOBJECT, field_280) == 0x280);
typedef struct pod_camera_reader_pc_s {
    u8 unknown_0[0x1038];
    f32 camera_shake_strength;
} POD_CAMERA_READER_PC;
STATIC_ASSERT(offsetof(POD_CAMERA_READER_PC, camera_shake_strength) == 0x1038);
STATIC_ASSERT(offsetof(GAMEOBJECT, previous_block_animation) == 0xe10);
STATIC_ASSERT(offsetof(CHARACTERANIM_CONFIG_PC, playback_rate) == 0x18);

void __cdecl Animate_POD(GAMEOBJECT *object) {
    i32 index = (i8)object->field_280;
    f32 delta, time;
    object->anim_packet.requested_animation = 1;
    if ((object->flags_1fc & 0x80) != 0 && index != -1 &&
        (object->id == (u16)id_ANAKINSPOD || object->id == (u16)id_ANAKINSPODGREEN)) {
        delta = (f32)((f64)FRAMETIME * 30.0);
        object->anim_packet.requested_animation = 1;
        if (object->character_model->action_animations[1] != NULL)
            delta = (f32)((f64)object->character_model->animation_configs[1]->playback_rate / 30.0 * delta);
        time = pod_animtime[index] + delta;
        pod_animtime[index] = time;
        if (time >= podanimendframe)
            pod_animtime[index] = (f32)((f64)time - ((f64)podanimendframe - 1.0));
        SetPodMergeAnims(&object->anim_packet, index);
        return;
    }
    if (PodSprint_InStartCountdown(WORLD) > 0.0f || PodRace_InStartCountdown(WORLD))
        object->anim_packet.requested_animation = 1;
    else if (((POD_CAMERA_READER_PC *)object)->camera_shake_strength > 0.0f)
        object->anim_packet.requested_animation = 0x17;
    else if ((object->flags_1fc & 0x80) == 0)
        object->anim_packet.requested_animation = 3;
    else if (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA) {
        if (object->previous_block_animation != -1)
            object->anim_packet.requested_animation = object->previous_block_animation;
    } else
        object->anim_packet.requested_animation = 0;
    if (object->character_model->action_animations[object->anim_packet.requested_animation] == NULL)
        object->anim_packet.requested_animation = 1;
}

f32 __cdecl getPodRoll(i32 index) {
    return pod_roll[index];
}
