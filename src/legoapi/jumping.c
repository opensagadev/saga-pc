#include "saga.h"
extern i32 LEGOCONTEXT_JUMP;
extern i16 LEGOACT_JUMP;
extern i32 LEGOCONTEXT_BIGJUMP;
extern i16 LEGOACT_COMBOJUMP;
extern i16 LEGOACT_JUMP2;
extern i16 LEGOACT_FALL;
f32 __cdecl AnimDuration(i32, i32, f32, f32, i32);
void __cdecl PlayJumpSfx(GAMEOBJECT *, i32);
void __cdecl ResetAnimPacket(ANIMPACKET_PC *, i16);
void __cdecl StartJump(GAMEOBJECT *object, i32 movement_state) {
    if (LEGOCONTEXT_JUMP != -1) {
        object->character_context = (i8)LEGOCONTEXT_JUMP;
        object->action_movement_state = (u8)movement_state;
        object->context_timer = 0.0f;
        object->field_7aa = LEGOACT_JUMP;
        object->jump_start_height = object->collision_min.y;
        object->context_variant_flags =
            (object->context_variant_flags & ~0x19000) | ((movement_state == 3 || movement_state == 4) ? 0x10000 : 0);
        object->airborne_action_timer = 0.0f;
        object->jump_sequence = 1;
        if ((u8)movement_state != 6 && (u8)movement_state != 7)
            PlayJumpSfx(object, 0);
    }
    object->field_281 = 0;
    object->field_1050 = 0;
    object->flags_e20 |= 0x100000;
    object->velocity.y = object->character_data->game_character->jump_speed;
    object->delayed_turn_timer = 0.0f;
    object->airborne_input_timer = 0.0f;
    object->airborne_collision_target = NULL;
    ResetAnimPacket(&object->anim_packet, -1);
    object->pitch_angle = 0;
    object->roll_angle = 0;
    object->flags_107a = 2;
}

static i32 __cdecl BigJump_JumpAction_Default(GAMEOBJECT *object) {
    CHARACTER_MODEL_LAYERS_PC *model;
    if (LEGOACT_COMBOJUMP != -1) {
        model = (CHARACTER_MODEL_LAYERS_PC *)object->character_model;
        if (model->model_data_b[LEGOACT_COMBOJUMP] != NULL)
            return LEGOACT_COMBOJUMP;
    }
    if (LEGOACT_JUMP2 != -1) {
        model = (CHARACTER_MODEL_LAYERS_PC *)object->character_model;
        if (model->model_data_b[LEGOACT_JUMP2] != NULL)
            return LEGOACT_JUMP2;
    }
    return LEGOACT_JUMP;
}

i32(__cdecl *BigJump_JumpActionFn)(GAMEOBJECT *) = BigJump_JumpAction_Default;

i32 __cdecl StartBigJump(GAMEOBJECT *object, NUVEC *destination, i32 mode, f32 height, f32 duration_scale,
                         i32 animation) {
    u16 angle;
    if (LEGOCONTEXT_BIGJUMP == -1 || (object->character_data->model_flags & 0x200000) != 0)
        return 0;
    STUBBED_NAMED("StartBigJump: Player_ClearContext PC55e4a0");
    STUBBED_NAMED("StartBigJump: Player_ResetContexts embedded packet6c4 PC55e4e0");
    object->big_jump_animation = (u8)animation;
    object->character_context = (i8)LEGOCONTEXT_BIGJUMP;
    object->field_7b3 = 1;
    object->field_7aa = (i16)BigJump_JumpActionFn(object);
    object->context_duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 0);
    if (object->context_duration == 0.0f) {
        object->context_duration = 1.0f;
        object->field_7aa = LEGOACT_FALL;
    } else {
        ResetAnimPacket(&object->anim_packet, -1);
    }
    object->context_duration *= duration_scale;
    object->external_force = object->position;
    object->context_timer = 0.0f;
    object->launch_origin = *destination;
    object->launch_origin.y -= object->character_bottom * object->origin_scale;
    angle = NuAtan2D(destination->x - object->external_force.x, destination->z - object->external_force.z);
    object->movement_facing_angle = angle;
    object->facing_angle = angle;
    object->field_0x276 = angle;
    object->flags_ef0 = (object->flags_ef0 & ~0x2000) | ((mode & 1) << 13);
    PlayJumpSfx(object, 2);
    object->ai_packet.flags |= 0x2000000;
    object->ai_packet.path_target = NULL;
    object->big_jump_height = height < 0.0f ? 0.0f : height;
    object->context_variant_flags &= ~0x100;
    return 1;
}

extern i16 BigJumpLandPreferredPc, BigJumpLandSecondaryPc, BigJumpLandFallbackPc;
static i32 __cdecl BigJump_LandAction_Default(GAMEOBJECT *object) {
    if (BigJumpLandPreferredPc != -1 && object->character_model->model_data_b[BigJumpLandPreferredPc] != NULL)
        return BigJumpLandPreferredPc;
    if (BigJumpLandSecondaryPc != -1 && object->character_model->model_data_b[BigJumpLandSecondaryPc] != NULL)
        return BigJumpLandSecondaryPc;
    return BigJumpLandFallbackPc;
}

#pragma data_seg(".data$BigJump_LandActionFn")
i32(__cdecl *BigJump_LandActionFn)(GAMEOBJECT *) = BigJump_LandAction_Default;
#pragma data_seg()

extern i32 LEGOCONTEXT_JUMP;
extern i16 LEGOACT_FALL;
extern i32(__cdecl *FindSlamOrigin_UseCPosFn)(GAMEOBJECT *);
void __cdecl StartEndOfJump(GAMEOBJECT *object) {
    if (LEGOCONTEXT_JUMP != -1) {
        object->character_context = (i8)LEGOCONTEXT_JUMP;
        object->action_movement_state = 0;
        object->field_7aa = LEGOACT_FALL;
        object->context_variant_flags = (object->context_variant_flags & 0xfffeafff) | 0x8000;
        object->jump_sequence = 2;
    }
    object->airborne_collision_target = NULL;
}

extern i32 LEGOCONTEXT_BIGJUMP;
extern i16 LEGOACT_FALL, LEGOACT_COMBOJUMP;
extern void(__cdecl *BigJump_EndOfLandFn)(GAMEOBJECT *);
extern i32(__cdecl *BigJump_LandActionFn)(GAMEOBJECT *);
void __cdecl StartEndOfJump(GAMEOBJECT *);
void __cdecl PlayLandSfx(GAMEOBJECT *, i32, i32);
void __cdecl FindSlamOrigin(GAMEOBJECT *, NUVEC *, NUVEC *);
void __cdecl BigJumpCode(GAMEOBJECT *object) {
    WORLDINFO *world;
    f32 fraction, height;
    NUVEC origin;
    i32 angle;
    if (LEGOCONTEXT_BIGJUMP == -1 || object->character_context != LEGOCONTEXT_BIGJUMP)
        return;
    object->respawn_timer = 0.0f;
    if (object->big_jump_animation != 0 && object->contact_count != 0) {
        object->character_context = -1;
        return;
    }
    if (object->field_7b3 == 2) {
        object->context_timer += FRAMETIME;
        if (object->context_timer >= object->context_duration) {
            object->character_context = -1;
            if (BigJump_EndOfLandFn != NULL)
                BigJump_EndOfLandFn(object);
        }
    } else if (object->field_7b3 == 1) {
        object->context_timer += FRAMETIME;
        if (object->context_timer >= object->context_duration) {
            if (LEGOACT_FALL != -1 && object->field_7aa == LEGOACT_FALL) {
                object->character_context = -1;
                return;
            }
            world = WorldInfo_CurrentlyActive();
            if (LEGOACT_COMBOJUMP != -1 && object->field_7aa == LEGOACT_COMBOJUMP) {
                object->facing_angle += 0x8000;
                object->movement_facing_angle += 0x8000;
                object->field_0x276 += 0x8000;
                ResetAnimPacket(&object->anim_packet, -1);
            }
            if ((object->context_variant_flags & 0x100) != 0 && object->field_281 == 0) {
                height = GameShadow(NULL, &object->launch_origin, 5.0f, -1);
                if (height != 2000000.0f && object->launch_origin.y - height >= 0.1f) {
                    StartEndOfJump(object);
                    return;
                }
            }
            ResetAnimPacket(&object->anim_packet, -1);
            object->field_7b3 = 2;
            object->field_7aa = (i16)BigJump_LandActionFn(object);
            object->context_duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
            if (object->context_duration == 0.0f)
                object->context_duration = 1.0f;
            object->context_timer = 0.0f;
            if ((object->portal_flags & 0x8000000) != 0 || (object->flags_ef0 & 0x2000) != 0) {
                GameCam_Judder(GameCam, -0.3f, 0, &object->collision_position);
                PlayLandSfx(object, 3, 0);
                NewRumbleAllPlayers(0.6f, 0.0f, 0, 0.0f);
            } else if ((object->flags_1fc_word & 0x80) != 0) {
                GameCam_Judder(GameCam, -0.2f, 0, &object->collision_position);
                PlayLandSfx(object, 3, 0);
            } else {
                PlayLandSfx(object, 0, 0);
            }
            object->external_force = object->position;
            object->velocity.x = object->velocity.y = object->velocity.z = 0.0f;
            if ((object->flags_ef0 & 0x2000) != 0) {
                FindSlamOrigin(object, &origin, NULL);
                AddExplosion(&origin, 2.0f, 0.4f, object, object->slam_effect, 0x19);
            }
            AISysGetCharacterPathPos(world->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, 0xff, 1);
        } else {
            fraction = object->context_timer / object->context_duration;
            object->position.x =
                object->external_force.x + (object->launch_origin.x - object->external_force.x) * fraction;
            object->position.y =
                object->external_force.y + (object->launch_origin.y - object->external_force.y) * fraction;
            object->position.z =
                object->external_force.z + (object->launch_origin.z - object->external_force.z) * fraction;
            angle = (i32)(fraction * 32768.0);
            height = NuTrigTable[(angle >> 1) & 0x7fff] * object->character_data->game_character->second_jump_height;
            object->position.y += (height + height) * object->big_jump_height;
            NuVecSub(&object->velocity, &object->position, &object->start_position);
            NuVecScale(&object->velocity, &object->velocity, 1.0f / FRAMETIME);
        }
    }
    if (object->character_context != -1)
        object->ai_packet.flags |= 0x2000000;
}

void FadeLoop_DrawObj(float alpha) {
    if (FadeLoop_ObjScene != NULL && NuSpecialExistsFn(&FadeLoop_ObjHSpecial)) {
        NUVEC scale = {0.125f, 0.125f, 0.125f};
        NUMTX matrix;
        NuMtxSetScale(&matrix, &scale);
        matrix.m32 = 1.0f;
        NuSpecialDrawAtAlpha(&FadeLoop_ObjHSpecial, &matrix, alpha);
    }
}

struct minianimpacket_pc_s {
    f32 current_time;
    f32 previous_time;
    u8 unknown_08[0xc];
    f32 blend_target_time;
    u8 flags;
    u8 blending;
    u8 unknown_1a[4];
    i16 current_animation_id;
    i16 previous_animation_id;
    i16 requested_animation_id;
};
STATIC_ASSERT(sizeof(MINIANIMPACKET_PC) == 0x24);
STATIC_ASSERT(offsetof(MINIANIMPACKET_PC, current_time) == 0);
STATIC_ASSERT(offsetof(MINIANIMPACKET_PC, previous_time) == 4);
STATIC_ASSERT(offsetof(MINIANIMPACKET_PC, blend_target_time) == 0x14);
STATIC_ASSERT(offsetof(MINIANIMPACKET_PC, flags) == 0x18);
STATIC_ASSERT(offsetof(MINIANIMPACKET_PC, blending) == 0x19);
STATIC_ASSERT(offsetof(MINIANIMPACKET_PC, current_animation_id) == 0x1e);
STATIC_ASSERT(offsetof(MINIANIMPACKET_PC, previous_animation_id) == 0x20);
STATIC_ASSERT(offsetof(MINIANIMPACKET_PC, requested_animation_id) == 0x22);

typedef struct beast_fall_land_view_pc_s {
    u8 unknown_0[0x7d0];
    MINIANIMPACKET_PC mini_animation;
    u8 unknown_7f4[0xdac - 0x7f4];
    f32 fall_animation_time;
} BEAST_FALL_LAND_VIEW_PC;
STATIC_ASSERT(offsetof(BEAST_FALL_LAND_VIEW_PC, mini_animation) == 0x7d0);
STATIC_ASSERT(offsetof(BEAST_FALL_LAND_VIEW_PC, fall_animation_time) == 0xdac);
extern i32 LEGOCONTEXT_LAND_JUMP;
extern i32(__cdecl *IsWearingBackPackFn)(GAMEOBJECT *);
extern i32(__cdecl *UsingExtraActionsFn)(GAMEOBJECT *);
extern i16 LEGOACT_BACKPACKFALLLAND, LEGOACT_EXTRA_LAND2, LEGOACT_FALLLAND;
void __cdecl ResetMiniAnimPacket(MINIANIMPACKET_PC *, i32);
i32 __cdecl StartFallLand(GAMEOBJECT *object, i32 action) {
    i16 selected;
    BEAST_FALL_LAND_VIEW_PC *view = (BEAST_FALL_LAND_VIEW_PC *)object;
    PlayLandSfx(object, 0, 0);
    if (LEGOCONTEXT_LAND_JUMP == -1) {
        object->flags_e24 &= ~0x400;
        return 0;
    }
    if (action != -1 && object->character_model->model_data_b[action] != NULL) {
        selected = (i16)action;
    } else if (IsWearingBackPackFn != NULL && IsWearingBackPackFn(object) && LEGOACT_BACKPACKFALLLAND != -1 &&
               object->character_model->model_data_b[LEGOACT_BACKPACKFALLLAND] != NULL) {
        selected = LEGOACT_BACKPACKFALLLAND;
    } else if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object) && LEGOACT_EXTRA_LAND2 != -1 &&
               object->character_model->model_data_b[LEGOACT_EXTRA_LAND2] != NULL) {
        selected = LEGOACT_EXTRA_LAND2;
    } else if (LEGOACT_FALLLAND != -1 && object->character_model->model_data_b[LEGOACT_FALLLAND] != NULL) {
        selected = LEGOACT_FALLLAND;
    } else if (BigJumpLandSecondaryPc != -1 && object->character_model->model_data_b[BigJumpLandSecondaryPc] != NULL) {
        selected = BigJumpLandSecondaryPc;
    } else {
        selected = BigJumpLandFallbackPc;
    }
    object->field_7aa = selected;
    if (object->character_model->model_data_b[selected] == NULL) {
        object->flags_e24 &= ~0x400;
        return 0;
    }
    object->character_context = (i8)LEGOCONTEXT_LAND_JUMP;
    ResetAnimPacket(&object->anim_packet, -1);
    ResetMiniAnimPacket(&view->mini_animation, -1);
    view->fall_animation_time = 0.0f;
    object->context_timer = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
    object->flags_e24 = (object->flags_e24 & ~0x800) | ((object->flags_e24 & 0x400) << 1);
    return 1;
}

typedef struct BALLOON_HEIGHT_VIEW_PC {
    u8 unknown[0x778];
    f32 height;
} BALLOON_HEIGHT_VIEW_PC;
STATIC_ASSERT(offsetof(BALLOON_HEIGHT_VIEW_PC, height) == 0x778);
void __cdecl SetBallooningHeight(GAMEOBJECT *object, f32 height) {
    if (object->character_context == 0x5d)
        ((BALLOON_HEIGHT_VIEW_PC *)object)->height = height;
}

i32 __cdecl UseFallAnim(GAMEOBJECT *object) {
    if ((CInfo[object->character_context].flags & 8) != 0 && LEGOACT_FALL != -1 &&
        object->character_model->model_data_b[LEGOACT_FALL] != NULL)
        return 1;
    return 0;
}

void __cdecl MakeJumpReachHeight(GAMEOBJECT *object, f32 height, i32 force) {
    f32 remaining_height = height - ((f64)object->collision_min.y - object->jump_start_height);
    if (remaining_height > 0.0f) {
        f32 radicand = (f64)object->character_data->game_character->gravity * -2.0 * remaining_height;
        f32 speed = NuFsqrt(radicand);
        if (force != 0 || speed > object->velocity.y)
            object->velocity.y = speed;
    } else if (force != 0)
        object->velocity.y = 0.0f;
}

void __cdecl StartJetPackFall(GAMEOBJECT *object, i32 fall) {
    object->context_timer = 0.0f;
    object->context_variant_flags &= 0xfffeafff;
    object->jump_variant_state = 0;
    object->character_context = 0;
    object->action_movement_state = 0;
    object->field_7aa = 6;
    object->jump_sequence = 2;
    object->airborne_collision_target = NULL;
    if (fall == 0 && object->character_model->model_data_b[0x49] != NULL) {
        object->context_variant_flags &= ~0x8000;
        object->velocity.y = object->character_data->game_character->jump_speed;
        object->field_7aa = 0x49;
        PlayJumpSfx(object, 0);
    } else
        object->character_context = -1;
    if (object->character_data->game_character->hover_fall_mode == 2)
        PlayJumpSfx(object, 1);
}
