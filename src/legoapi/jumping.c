#include "saga.h"

/* Retail unit 0x561ba0-0x564310 (legoapi/jumping.c): definitions in retail address order. */
extern i32 LEGOCONTEXT_JUMP;
extern i16 LEGOACT_JUMP;
extern i32 LEGOCONTEXT_BIGJUMP;
extern i16 LEGOACT_COMBOJUMP;
extern i16 LEGOACT_JUMP2;
extern i16 LEGOACT_FALL;
f32 __cdecl AnimDuration(i32, i32, f32, f32, i32);
void __cdecl PlayJumpSfx(GAMEOBJECT *, i32);
void __cdecl ResetAnimPacket(ANIMPACKET_PC *, i32);
extern i32 ZipUpLandCombatRollContextPc, ZipUpWallShuffleContextPc;
extern i16 BigJumpLandPreferredPc, BigJumpLandSecondaryPc, BigJumpLandFallbackPc;
extern i32(__cdecl *FindSlamOrigin_UseCPosFn)(GAMEOBJECT *);
extern i16 LEGOACT_FALL, LEGOACT_COMBOJUMP;
extern void(__cdecl *BigJump_EndOfLandFn)(GAMEOBJECT *);
extern i32(__cdecl *BigJump_LandActionFn)(GAMEOBJECT *);
void __cdecl PlayLandSfx(GAMEOBJECT *, i32, i32);
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
typedef struct BALLOON_HEIGHT_VIEW_PC {
    u8 unknown[0x778];
    f32 height;
} BALLOON_HEIGHT_VIEW_PC;
STATIC_ASSERT(offsetof(BALLOON_HEIGHT_VIEW_PC, height) == 0x778);
void __cdecl UpdateLastSafePosition(GAMEOBJECT *object);
void __cdecl StartQuickShoot(GAMEOBJECT *, i32);
void __cdecl FastWeaponOut(GAMEOBJECT *, i32);
void __cdecl StartQuickShoot(GAMEOBJECT *object, i32 action);
extern f32 PUNCHGAP, PUNCHCHARGAP;
void __cdecl StartHold(GAMEOBJECT *);
void __cdecl Punch_Hit(GAMEOBJECT *, GAMEOBJECT *, f32, f32);
void __cdecl PlaySabreSfx(char *, GAMEOBJECT *, NUVEC *, i32);
void __cdecl StartHold(GAMEOBJECT *object);
extern i32 JumpBackFlipContextPc;
extern i16 JumpBackFlipActionPc, JumpSlamActionPc;
i32 __cdecl Glide_Start(GAMEOBJECT *object);
void __cdecl StartLunge(GAMEOBJECT *object, f32 speed, f32 height);
typedef struct jump_auxiliary_fields_pc_s {
    u8 unknown_000[0x7b1];
    u8 landing_followup;
    u8 unknown_7b2[0x1e];
    MINIANIMPACKET_PC mini_animation;
    u8 unknown_7f4[0x530];
    f32 ground_grace_timer;
    f32 jump_reentry_timer;
    u8 unknown_d2c[0xc];
    f32 communication_blend;
    u8 unknown_d3c[0x2c];
    f32 jump_chain_timer;
    u8 unknown_d6c[0x40];
    f32 fall_animation_timer;
    u8 unknown_db0[0x8b];
    u8 action_suppressed;
} JUMP_AUXILIARY_FIELDS_PC;
void __cdecl ComboHitFrame(GAMEOBJECT *, i32);
void __cdecl StartLunge(GAMEOBJECT *, f32, f32);
extern i32 LEGOCONTEXT_DOOMED;
extern i32 LEGOCONTEXT_WEAPONIN;
extern i32 LEGOCONTEXT_WEAPONOUT;
extern i32 LEGOCONTEXT_LAND_JUMP2;
extern i32 LEGOCONTEXT_LAND_FLIP;
extern i32 LEGOCONTEXT_LAND_COMBOJUMP;
extern i32 LEGOCONTEXT_LAND_LUNGE;
extern i32 LEGOCONTEXT_LAND_SLAM;
extern i32 ZipUpLandCombatRollContextPc;
extern i16 BigJumpLandFallbackPc;
extern i16 LEGOACT_JUMP3;
extern i16 BigJumpLandSecondaryPc;
extern i16 LEGOACT_LAND3;
extern i16 BigJumpLandPreferredPc;
extern i16 LEGOACT_FALLLAND;
extern i16 LEGOACT_BACKPACKFALLLAND;
extern i16 LEGOACT_FLIP;
extern i16 LEGOACT_FLIPLAND;
extern i16 JumpBackFlipActionPc;
extern i16 LEGOACT_MAGNET_JUMP;
extern i16 LEGOACT_LUNGE;
extern i16 LEGOACT_LUNGELAND;
extern i16 JumpSlamActionPc;
extern i16 LEGOACT_SLAMLAND;
extern i16 LEGOACT_COMBATROLL_JUMP;
extern i16 LEGOACT_COMBATROLL_FALL;
extern i16 LEGOACT_COMBATROLL_LAND;
extern i16 LEGOACT_COMBATROLL_FIRE;
extern i16 LEGOACT_EXTRA_JUMP;
extern i16 LEGOACT_EXTRA_JUMP2;
extern i16 LEGOACT_EXTRA_LAND;
extern i16 LEGOACT_EXTRA_LAND2;
extern f32 SLAMJUMPSPEED;
extern f32 JumpFailedLandChainTimePc;
extern i32 DoubleJump_AlwaysReachJump2Height;
extern i32 DoubleJump_JediSlam;
extern i32(__cdecl *Jump_PreventJumpFn)(GAMEOBJECT *);
extern void(__cdecl *Jump_EndOfLandContextFn)(GAMEOBJECT *);
extern i32(__cdecl *Slam_GetDebrisFn)(GAMEOBJECT *, i32);
extern i32(__cdecl *CanGlideFn)(GAMEOBJECT *);

static i32 __cdecl BigJump_JumpAction_Default(GAMEOBJECT *object);
static i32 __cdecl BigJump_LandAction_Default(GAMEOBJECT *object);
void __cdecl MakeJumpReachHeight(GAMEOBJECT *object, f32 height, i32 force);

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

static i32 __cdecl BigJump_LandAction_Default(GAMEOBJECT *object) {
    if (BigJumpLandPreferredPc != -1 && object->character_model->model_data_b[BigJumpLandPreferredPc] != NULL)
        return BigJumpLandPreferredPc;
    if (BigJumpLandSecondaryPc != -1 && object->character_model->model_data_b[BigJumpLandSecondaryPc] != NULL)
        return BigJumpLandSecondaryPc;
    return BigJumpLandFallbackPc;
}
i32(__cdecl *BigJump_JumpActionFn)(GAMEOBJECT *) = BigJump_JumpAction_Default;

i32 __cdecl StartBigJump(GAMEOBJECT *object, NUVEC *destination, i32 mode, f32 height, f32 duration_scale,
                         i32 animation) {
    u16 angle;
    if (LEGOCONTEXT_BIGJUMP == -1 || (object->character_data->model_flags & 0x200000) != 0)
        return 0;
    Player_ClearContext(object, 0);
    Player_ResetContexts(object->ai_player_packet_pc);
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

void __cdecl FindSlamOrigin(GAMEOBJECT *object, NUVEC *start, NUVEC *end) {
    i32 joint;
    if (FindSlamOrigin_UseCPosFn != NULL && FindSlamOrigin_UseCPosFn(object)) {
        start->x = object->collision_position.x;
        start->z = object->collision_position.z;
    } else {
        joint = object->character_data->game_character->streak_joints[0][0];
        if (joint == -1 || object->origin_joint_ready == 0 || object->character_model == NULL ||
            object->character_model->points_of_interest[joint] == NULL) {
            start->x = object->position.x;
            start->z = object->position.z;
        } else {
            start->x = object->locator_matrices[joint].m30;
            start->z = object->locator_matrices[joint].m32;
        }
    }
    start->y = object->floor_height;
    if (end != NULL) {
        joint = object->character_data->game_character->streak_joints[0][1];
        if (joint == -1 || object->origin_joint_ready == 0 || object->character_model == NULL ||
            object->character_model->points_of_interest[joint] == NULL)
            *end = *start;
        else
            *end = object->locator_matrices[joint].translation;
    }
}

i32 __cdecl ObjLandReady(GAMEOBJECT *object) {
    i32 context = object->character_context;
    if ((U64_PAIR(&CInfo[context].flags) & 0x1004) != 0 &&
        (ZipUpLandCombatRollContextPc == -1 || ZipUpLandCombatRollContextPc != context || object->field_7b3 >= 1)) {
        if (ZipUpWallShuffleContextPc != -1 && ZipUpWallShuffleContextPc == context)
            return 0;
        return 1;
    }
    return 0;
}

i32 __cdecl UseFallAnim(GAMEOBJECT *object) {
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 8) != 0 && LEGOACT_FALL != -1 &&
        object->character_model->model_data_b[LEGOACT_FALL] != NULL)
        return 1;
    return 0;
}

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

i32 __cdecl StartBackFlip(GAMEOBJECT *object) {
    if (JumpBackFlipContextPc == -1 || JumpBackFlipActionPc == -1 ||
        object->character_model->model_data_b[JumpBackFlipActionPc] == NULL)
        return 0;
    object->character_context = (i8)JumpBackFlipContextPc;
    ResetAnimPacket(&object->anim_packet, -1);
    object->field_7aa = JumpBackFlipActionPc;
    object->context_timer = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
    if (object->context_timer <= 0.0f)
        object->context_timer = 1.0f;
    object->flags_e20 |= 0x100000;
    object->delayed_turn_timer = 0.0f;
    object->context_variant_flags &= ~0x8000;
    object->airborne_input_timer = 0.0f;
    PlayJumpSfx(object, 2);
    return 1;
}

void __cdecl BackFlipCode(GAMEOBJECT *object) {
    if (JumpBackFlipContextPc == -1 || object->character_context != JumpBackFlipContextPc)
        return;
    if ((object->flags_e20 & 0x100000) != 0 && (object->pad->buttons_held & GAMEPAD_JUMP) == 0)
        object->flags_e20 &= ~0x100000;
    if (object->character_model->model_data_b[object->field_7aa] != NULL &&
        CurrentAnim(&object->anim_packet) != object->field_7aa)
        return;
    object->context_timer -= FRAMETIME;
    if (!(object->context_timer <= 0.0f))
        return;
    object->character_context = -1;
    if ((object->pad->buttons_held & GAMEPAD_JUMP) != 0 && (object->flags_e20 & 0x100000) == 0)
        StartBackFlip(object);
    else {
        object->velocity.x = 0.0f;
        object->velocity.z = 0.0f;
    }
}

/* Native 5624d0: ordinary cdecl(object,vertical_speed), returning genuine success. */
i32 __cdecl Slam_Start(GAMEOBJECT *object, f32 speed) {
    if (LEGOCONTEXT_JUMP != -1) {
        object->character_context = (i8)LEGOCONTEXT_JUMP;
        object->action_movement_state = 4;
        object->field_7aa = JumpSlamActionPc;
        object->context_timer = 0.0f;
        if ((object->flags_1fc & 0x80) != 0)
            object->portal_flags |= 0x800;
        object->velocity.y = speed;
        object->jump_sequence = 0;
        PlayJumpSfx(object, 4);
        NewRumble(object->pad->bound_pad, 0.5f, 0);
        object->context_variant_flags |= 0x10000;
        return 1;
    }
    return 0;
}

void __cdecl JumpCode(GAMEOBJECT *object, i32 jump_pressed, i32 jump_held, u32 animation_set, i32 action_pressed,
                      i32 action_held, i32 hit_effect) {
    JUMP_AUXILIARY_FIELDS_PC *aux = (JUMP_AUXILIARY_FIELDS_PC *)object;
    WORLDINFO *world;
    i32 player_flag;
    i32 flip_kind;
    i32 force_flip;
    if ((object->flags_e24 & 0x1000) != 0) {
        return;
    }
    force_flip = -1;
    world = WorldInfo_CurrentlyActive();

    player_flag = (object->flags_1fc_word >> 7) & 1;
    flip_kind = 0;
    if (player_flag != 0 || (object->portal_flags & 0x100) != 0) {
        if (JumpBackFlipActionPc != -1 && object->character_model->model_data_b[JumpBackFlipActionPc] != NULL)
            flip_kind = 1;
        else if ((animation_set & 2) != 0 && -1 != LEGOACT_FLIP &&
                 object->character_model->model_data_b[LEGOACT_FLIP] != NULL)
            flip_kind = 2;
    }
    if (aux->jump_reentry_timer > 0.0f)
        aux->jump_reentry_timer -= FRAMETIME;
    if (aux->jump_chain_timer > 0.0f)
        aux->jump_chain_timer -= FRAMETIME;

    if (object->character_context != -1 &&
        (object->character_context == LEGOCONTEXT_LAND_JUMP || object->character_context == LEGOCONTEXT_LAND_JUMP2 ||
         object->character_context == LEGOCONTEXT_LAND_FLIP ||
         object->character_context == LEGOCONTEXT_LAND_COMBOJUMP ||
         object->character_context == LEGOCONTEXT_LAND_LUNGE || object->character_context == LEGOCONTEXT_LAND_SLAM ||
         object->character_context == ZipUpLandCombatRollContextPc)) {
        if (object->character_context == LEGOCONTEXT_LAND_LUNGE && (object->context_variant_flags & 0x40) == 0) {
            const f32 frame = AnimListFrame(object->character_model, object->field_7aa, 0);
            if (frame > 0.0f) {
                f32 *time = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
                if (time != NULL && *time >= frame) {
                    if ((object->character_data->model_flags & 8) != 0)
                        ComboHitFrame(object, 1);
                    else
                        Punch_Hit(object, object->force_target, PUNCHCHARGAP, PUNCHCHARGAP);
                }
            }
        } else if (object->character_context == ZipUpLandCombatRollContextPc) {
            const f32 frame = AnimStopFrame(object->character_model, object->field_7aa);
            if (frame > 0.0f) {
                f32 *time = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
                if (time != NULL && frame <= *time)
                    object->field_7b3 = 1;
            }
        }
        if (object->character_context == ZipUpLandCombatRollContextPc && object->field_7b3 == 1 &&
            aux->landing_followup == 1)
            goto jump_takeoff;
        if (jump_pressed != 0 &&
            (LEGOCONTEXT_LAND_LUNGE == -1 || object->character_context != LEGOCONTEXT_LAND_LUNGE) &&
            (LEGOCONTEXT_LAND_SLAM == -1 || object->character_context != LEGOCONTEXT_LAND_SLAM) &&
            (ZipUpLandCombatRollContextPc == -1 || object->character_context != ZipUpLandCombatRollContextPc))
            goto jump_takeoff;
        if ((LEGOCONTEXT_LAND_LUNGE == -1 || object->character_context != LEGOCONTEXT_LAND_LUNGE) &&
            (LEGOCONTEXT_LAND_SLAM == -1 || object->character_context != LEGOCONTEXT_LAND_SLAM) &&
            ((object->flags_1fc & 0x80) != 0 || object->character_context != LEGOCONTEXT_LAND_JUMP ||
             object->field_7aa == -1 ||
             (object->field_7aa != LEGOACT_FALLLAND && object->field_7aa != LEGOACT_BACKPACKFALLLAND)) &&
            ((ZipUpLandCombatRollContextPc == -1 || object->character_context != ZipUpLandCombatRollContextPc) ||
             ((object->character_data->game_character->flags_094_bytes[2] & 8) != 0 &&
              !(object->field_7b3 == 1 && aux->landing_followup == 0))) &&
            0.0f < object->pad->input_magnitude && (object->flags_e24 & 0x800) == 0) {
            object->character_context = -1;
            return;
        }
        if (object->character_context == ZipUpLandCombatRollContextPc) {
            if (0 != jump_pressed)
                aux->landing_followup = 1;
            else if (action_pressed != 0 && LEGOACT_COMBATROLL_FIRE != -1 &&
                     object->character_model->model_data_b[LEGOACT_COMBATROLL_FIRE] != NULL)
                aux->landing_followup = 2;
        }
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f) {
            if (Jump_EndOfLandContextFn != NULL)
                Jump_EndOfLandContextFn(object);
            if (object->character_context == ZipUpLandCombatRollContextPc) {
                if (aux->landing_followup == 2) {
                    u16 angle;
                    aux->action_suppressed = 0;
                    angle = GamePad_InputAngle(object, object->pad);
                    object->field_0x276 = angle;
                    object->movement_facing_angle = angle;
                    object->facing_angle = angle;
                    StartQuickShoot(object, LEGOACT_COMBATROLL_FIRE);
                    object->quick_shoot_timer = 0.1f;
                    return;
                }
                if (aux->landing_followup == 1)
                    goto jump_takeoff;
                object->character_context = -1;
                return;
            }
            if (LEGOCONTEXT_LAND_LUNGE != -1 && object->character_context == LEGOCONTEXT_LAND_LUNGE &&
                (object->context_variant_flags & 0x40) == 0 && (object->character_data->model_flags & 8) != 0)
                ComboHitFrame(object, 1);
            if ((object->character_context == LEGOCONTEXT_LAND_LUNGE ||
                 object->character_context == LEGOCONTEXT_LAND_SLAM) &&
                action_held != 0) {
                StartHold(object);
                return;
            }
            object->character_context = -1;
        } else if (object->character_context == LEGOCONTEXT_LAND_SLAM) {
            i32 count = ParticlesPerSecond(25.0f, FRAMETIME);
            NUVEC start, end;
            FindSlamOrigin(object, &start, &end);
            AddGameDebrisRot(world->game_debris_sys, hit_effect, &start, ParticlesPerSecond(20.0f, FRAMETIME), 0, 0);
            while (count > 0) {
                const f32 t = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f));
                NUVEC position;
                position.x = (end.x - start.x) * t + start.x;
                position.z = (end.z - start.z) * t + start.z;
                position.y = (end.y - start.y) * t + start.y;
                AddGameDebrisRot(world->game_debris_sys, hit_effect, &position, 1, 0, 0);
                --count;
            }
        }
    } else {
        if (LEGOCONTEXT_JUMP == -1)
            return;
        if (object->character_context == LEGOCONTEXT_JUMP) {
            u32 variant_flags = object->context_variant_flags;
            f32 vertical_speed;
            u8 movement_state;
            i32 restart_jump;
            if ((variant_flags & 0x1000) != 0)
                object->flags_e24 |= 0x8000;
            if ((object->flags_e20 & 0x100000) != 0 && jump_held == 0) {
                object->flags_e20 &= ~0x100000u;
            }

            vertical_speed = object->velocity.y;
            if (vertical_speed < 0.0f) {
                object->airborne_collision_target = NULL;
            }
            movement_state = object->action_movement_state;
            if (movement_state == 5 && object->jump_variant_state == 2 && object->jump_variant_timer > 0.0f) {
                object->jump_variant_timer -= FRAMETIME;
                if (object->jump_variant_timer <= 0.0f)
                    object->jump_variant_state = 3;
            }
            object->context_timer += FRAMETIME;
            if (movement_state == 8 && (variant_flags & 0x8000) == 0 &&
                object->character_model->model_data_b[object->field_7aa] != NULL &&
                object->context_timer >= object->context_duration && LEGOACT_COMBATROLL_FALL != -1 &&
                object->character_model->model_data_b[LEGOACT_COMBATROLL_FALL] != NULL) {
                variant_flags |= 0x8000;
                object->context_variant_flags = variant_flags;
            }
            if ((movement_state == 8 || movement_state == 2 || movement_state == 0) && object->context_timer >= 2.0f &&
                object->position.y >= object->start_position.y && object->field_281 == 0) {
                object->character_context = -1;
                aux->jump_reentry_timer = 0.0f;
                aux->fall_animation_timer = 0.2f;
                return;
            }

            if ((movement_state == 5 &&
                 (0 != ((object->field_281 & 1)) ||
                  (player_flag == 0 && object->contact_count != 0 && object->jump_variant_state == 3))) ||
                (movement_state != 5 && object->field_281 != 0 && object->context_timer >= 0.1f) ||
                ((movement_state == 4 || movement_state == 3) && object->context_timer >= 2.5f)) {
                restart_jump = 0;
                if (player_flag != 0)
                    object->portal_flags |= 0x800;
                if (jump_held != 0 && 0 == ((object->flags_e20 & 0x100000)) && object->action_movement_state != 3 &&
                    object->action_movement_state != 4 && object->action_movement_state != 5 &&
                    object->action_movement_state != 8) {
                    if ((object->action_movement_state == 1 && 0.0f < object->pad->input_magnitude &&
                         (0x200000 & object->flags_e20) != 0 &&
                         abs(RotDiff(object->movement_facing_angle, object->current_input_angle)) > 0x6aaa) ||
                        (object->action_movement_state == 2 && 0.0f < object->pad->input_magnitude &&
                         (0x200000 & object->flags_e20) != 0 &&
                         abs(RotDiff(object->movement_facing_angle, object->current_input_angle)) < 0x1555))
                        force_flip = 1;
                    restart_jump = 1;
                }
                {
                    if (object->action_movement_state == 3) {
                        if (!(LEGOCONTEXT_LAND_LUNGE != -1 && LEGOACT_LUNGELAND != -1 &&
                              object->character_model->model_data_b[LEGOACT_LUNGELAND] != NULL)) {
                            if ((object->character_data->model_flags & 8) == 0) {
                                Punch_Hit(object, object->force_target, PUNCHCHARGAP, PUNCHCHARGAP);
                                object->character_context = -1;
                                aux->jump_reentry_timer = 0.2f;
                                if (object->action_movement_state == 5)
                                    aux->jump_chain_timer = JumpFailedLandChainTimePc;
                            } else {
                                object->character_context = -1;
                                aux->jump_reentry_timer = 0.2f;
                            }
                            PlayLandSfx(object, 0, 0);
                        } else {
                            NewRumble(object->pad->bound_pad, 0.6f, 0);
                            object->character_context = LEGOCONTEXT_LAND_LUNGE;
                            object->field_7aa = LEGOACT_LUNGELAND;
                            object->context_timer = AnimDuration((i16)object->id, LEGOACT_LUNGELAND, 0.0f, 0.0f, 1);
                            object->context_variant_flags = object->context_variant_flags & (~0x40u);
                            aux->jump_reentry_timer = 0.0f;
                            if (!restart_jump)
                                ResetAnimPacket(&object->anim_packet, -1);
                            if ((object->character_data->model_flags & 8) == 0 &&
                                AnimListFrame(object->character_model, object->field_7aa, 0) == 0.0f)
                                Punch_Hit(object, object->force_target, PUNCHCHARGAP, PUNCHCHARGAP);
                            PlayLandSfx(object, 1, 0);
                        }
                    } else if (object->action_movement_state == 4) {
                        if (LEGOCONTEXT_LAND_SLAM != -1 && LEGOACT_SLAMLAND != -1 &&
                            object->character_model->model_data_b[LEGOACT_SLAMLAND] != NULL) {
                            f32 duration;
                            GameCam_Judder(GameCam, 0.5f, -1, &object->collision_position);
                            NewRumble(object->pad->bound_pad, 0.75f, 0);
                            object->slam_effect = hit_effect;
                            if (Slam_GetDebrisFn != NULL)
                                object->slam_effect = Slam_GetDebrisFn(object, hit_effect);
                            object->character_context = LEGOCONTEXT_LAND_SLAM;
                            object->field_7aa = LEGOACT_SLAMLAND;
                            duration = AnimDuration((i16)object->id, LEGOACT_SLAMLAND, 0.0f, 0.0f, 1);
                            object->context_timer = duration <= 0.0f ? 0.75f : duration;
                            aux->jump_reentry_timer = 0.0f;
                            if (!restart_jump) {
                                ResetAnimPacket(&object->anim_packet, -1);
                                ResetMiniAnimPacket(&aux->mini_animation, -1);
                            }
                            PlayLandSfx(object, 2, 0);
                        } else {
                            object->character_context = -1;
                            aux->jump_reentry_timer = 0.2f;
                            PlayLandSfx(object, 0, 0);
                        }
                    } else if (object->action_movement_state == 2 && LEGOCONTEXT_LAND_COMBOJUMP != -1 &&
                               BigJumpLandPreferredPc != -1 &&
                               object->character_model->model_data_b[BigJumpLandPreferredPc] != NULL) {
                        if ((object->context_variant_flags & 0x2000) == 0) {
                            object->field_0x276 += 0x8000;
                            object->facing_angle += 0x8000;
                            object->movement_facing_angle += 0x8000;
                            if (!restart_jump)
                                ResetAnimPacket(&object->anim_packet, -1);
                        }
                        aux->jump_reentry_timer = 0.0f;
                        if (object->pad->input_magnitude == 0.0f) {
                            object->character_context = LEGOCONTEXT_LAND_COMBOJUMP;
                            object->field_7aa = (object->context_variant_flags & 0x2000) != 0 &&
                                                        -1 != LEGOACT_FLIPLAND &&
                                                        object->character_model->model_data_b[LEGOACT_FLIPLAND] != NULL
                                                    ? LEGOACT_FLIPLAND
                                                    : BigJumpLandPreferredPc;
                            object->context_timer = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
                        } else {
                            object->character_context = -1;
                        }
                        PlayLandSfx(object, 0, 0);
                    } else if (object->action_movement_state == 1 && LEGOCONTEXT_LAND_FLIP != -1 &&
                               LEGOACT_FLIPLAND != -1 &&
                               object->character_model->model_data_b[LEGOACT_FLIPLAND] != NULL) {
                        aux->jump_reentry_timer = 0.0f;
                        if (object->pad->input_magnitude == 0.0f) {
                            object->character_context = LEGOCONTEXT_LAND_FLIP;
                            object->field_7aa = LEGOACT_FLIPLAND;
                            object->context_timer = AnimDuration((i16)object->id, LEGOACT_FLIPLAND, 0.0f, 0.0f, 1);
                            if (!restart_jump)
                                ResetAnimPacket(&object->anim_packet, -1);
                        } else {
                            object->character_context = -1;
                        }
                        PlayLandSfx(object, 0, 0);
                    } else if (object->action_movement_state == 8) {
                        if (ZipUpLandCombatRollContextPc != -1 &&
                            object->character_model->model_data_b[LEGOACT_COMBATROLL_LAND] != NULL) {
                            NewRumble(object->pad->bound_pad, 0.4f, 0);
                            object->character_context = ZipUpLandCombatRollContextPc;
                            object->field_7b3 = 0;
                            object->field_7aa = LEGOACT_COMBATROLL_LAND;
                            object->context_timer =
                                AnimDuration((i16)object->id, LEGOACT_COMBATROLL_LAND, 0.0f, 0.0f, 1);
                            object->context_variant_flags &= ~0x40u;
                            aux->jump_reentry_timer = 0.0f;
                            if (!restart_jump)
                                ResetAnimPacket(&object->anim_packet, -1);
                            aux->landing_followup = 0;
                            if (jump_held != 0 && (object->flags_e20 & 0x100000) == 0)
                                aux->landing_followup = 1;
                            else if (action_held != 0 && -1 != LEGOACT_COMBATROLL_FIRE &&
                                     object->character_model->model_data_b[LEGOACT_COMBATROLL_FIRE] != NULL)
                                aux->landing_followup = 2;
                            PlayLandSfx(object, 4, 0);
                        } else {
                            object->character_context = -1;
                            aux->jump_reentry_timer = 0.2f;
                            PlayLandSfx(object, 0, 0);
                        }
                    } else {
                        i32 second_landing = 0;
                        if (object->action_movement_state == 0 && object->jump_sequence > 1 &&
                            LEGOCONTEXT_LAND_JUMP2 != -1) {
                            i16 action = BigJumpLandSecondaryPc;
                            if ((object->context_variant_flags & 0x4000) != 0 &&
                                (action == -1 || object->character_model->model_data_b[action] == NULL))
                                action = LEGOACT_LAND3;
                            second_landing = action != -1 && object->character_model->model_data_b[action] != NULL;
                        }
                        if (second_landing) {
                            aux->jump_reentry_timer = 0.0f;
                            if (object->pad->input_magnitude != 0.0f) {
                                object->character_context = -1;
                            } else if (aux->fall_animation_timer >= 0.2f) {
                                StartFallLand(object, -1);
                            } else {
                                object->character_context = LEGOCONTEXT_LAND_JUMP2;
                                if ((object->context_variant_flags & 0x4000) != 0 && LEGOACT_LAND3 != -1 &&
                                    object->character_model->model_data_b[LEGOACT_LAND3] != NULL)
                                    object->field_7aa = LEGOACT_LAND3;
                                else if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object) &&
                                         LEGOACT_EXTRA_LAND2 != -1 &&
                                         object->character_model->model_data_b[LEGOACT_EXTRA_LAND2] != NULL)
                                    object->field_7aa = LEGOACT_EXTRA_LAND2;
                                else
                                    object->field_7aa = BigJumpLandSecondaryPc;
                                object->context_timer = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
                                if (!restart_jump)
                                    ResetAnimPacket(&object->anim_packet, -1);
                            }
                        } else if ((object->action_movement_state == 0 || object->action_movement_state == 6 ||
                                    object->action_movement_state == 7 || object->action_movement_state == 9) &&
                                   -1 != LEGOCONTEXT_LAND_JUMP && BigJumpLandFallbackPc != -1 &&
                                   object->character_model->model_data_b[BigJumpLandFallbackPc] != NULL &&
                                   object->pad->input_magnitude == 0.0f) {
                            if (aux->fall_animation_timer >= 0.2f) {
                                StartFallLand(object, -1);
                            } else {
                                object->character_context = LEGOCONTEXT_LAND_JUMP;
                                if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object) &&
                                    LEGOACT_EXTRA_LAND != -1 &&
                                    object->character_model->model_data_b[LEGOACT_EXTRA_LAND] != NULL)
                                    object->field_7aa = LEGOACT_EXTRA_LAND;
                                else
                                    object->field_7aa = BigJumpLandFallbackPc;
                                if (CanMagnetClimbFn != NULL && CanMagnetClimbFn(object) && LEGOACT_MAGNET_JUMP != -1 &&
                                    object->character_model->model_data_b[LEGOACT_MAGNET_JUMP] != NULL)
                                    object->field_7aa = LEGOACT_MAGNET_JUMP;
                                object->context_timer = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
                                aux->jump_reentry_timer = object->jump_sequence > 1 ? 0.0f : 0.2f;
                                if (!restart_jump)
                                    ResetAnimPacket(&object->anim_packet, -1);
                            }
                        } else {
                            object->character_context = -1;
                            aux->jump_reentry_timer = 0.2f;
                            if (object->action_movement_state == 5)
                                aux->jump_chain_timer = JumpFailedLandChainTimePc;
                        }
                        PlayLandSfx(object, 0, 0);
                    }
                }
                if (restart_jump) {
                    UpdateLastSafePosition(object);
                    goto jump_takeoff;
                }
                return;
            } else {
                PLAYERCHARACTERCONFIG_PC *game_character;
                i32 can_start_second_jump;
                if (action_pressed != 0 && object->velocity.y > -1.25f &&
                    (object->character_data->game_character->hover_fall_mode != 0 ||
                     (object->character_data->model_flags & 8) != 0) &&
                    movement_state == 0 && (object->context_variant_flags & 0x8000) == 0 && object->jump_sequence < 2 &&
                    LEGOACT_LUNGE != -1 && object->character_model->model_data_b[LEGOACT_LUNGE] != NULL) {
                    object->force_target = NULL;
                    object->blowup_target = NULL;
                    StartLunge(object, 0.0f, object->bounds_half_height);
                    return;
                }
                game_character = object->character_data->game_character;
                if (DoubleJump_JediSlam != 0 && (animation_set & 0x10) != 0 && action_pressed != 0 &&
                    object->velocity.y > -1.25f && (object->context_variant_flags & 0x8000) == 0 &&
                    ((movement_state == 0 &&
                      (object->jump_sequence == 2 || (game_character->flags_098[0] & 0x20) != 0)) ||
                     movement_state == 1 || movement_state == 2) &&
                    JumpSlamActionPc != -1 && object->character_model->model_data_b[JumpSlamActionPc] != NULL) {
                    if (Slam_Start(object, SLAMJUMPSPEED) != 0) {
                        PlaySabreSfx(NULL, object, NULL, 0);
                        if (object->action_movement_state == 1 || object->action_movement_state == 2) {
                            ResetAnimPacket(&object->anim_packet, -1);
                            if (object->action_movement_state == 2 && (object->context_variant_flags & 0x2000) == 0) {
                                object->field_0x276 += 0x8000;
                                object->facing_angle += 0x8000;
                                object->movement_facing_angle += 0x8000;
                            }
                        }
                        return;
                    }
                }
                can_start_second_jump = jump_pressed != 0 && (animation_set & 8) != 0 &&
                                        object->action_movement_state == 0 && object->jump_sequence < 2 &&
                                        object->velocity.y > -1.25f;
                if (can_start_second_jump) {
                    if (LEGOCONTEXT_DOOMED != -1 && object->character_context == LEGOCONTEXT_DOOMED)
                        return;
                    game_character = object->character_data->game_character;
                    if ((game_character->flags_094_bytes[1] & 0x20) != 0) {
                        object->jump_variant_timer = 0.0f;
                        object->character_context = -1;
                        object->jump_variant_state = 1;
                        FastWeaponOut(object, 1);
                        return;
                    }
                    if ((animation_set & 0x110) == 0 && (game_character->flags_090 & 0x40400000) == 0)
                        return;
                    if ((animation_set & 0x100) != 0) {
                        MakeJumpReachHeight(object, HIGHJUMPHEIGHT, 1);
                    } else if (0 != ((animation_set & 0x200))) {
                        MakeJumpReachHeight(object, game_character->jump_height, 0);
                    } else if (DoubleJump_AlwaysReachJump2Height != 0 || object->velocity.y > 0.0f) {
                        MakeJumpReachHeight(object, game_character->second_jump_height, 0);
                    } else {
                        object->velocity.y = 0.0f;
                    }
                    if (CanGlideFn != NULL && CanGlideFn(object) && Glide_Start(object))
                        return;
                    object->jump_sequence++;
                    object->context_variant_flags |= 0x4000;
                    game_character = object->character_data->game_character;
                    if ((animation_set & 0x110) == 0 && 0 == ((game_character->flags_090 & 0x400000))) {
                        if ((game_character->flags_090 & 0x40000000) != 0) {
                            object->action_movement_state = 8;
                            object->context_variant_flags &= ~0x8000u;
                            object->field_7aa = LEGOACT_COMBATROLL_JUMP;
                            object->context_duration =
                                AnimDuration((i16)object->id, LEGOACT_COMBATROLL_JUMP, 0.0f, 0.0f, 1);
                        }
                    } else {
                        i16 action;
                        object->action_movement_state = 0;
                        if (LEGOACT_JUMP3 != -1 && object->character_model->model_data_b[LEGOACT_JUMP3] != NULL)
                            action = LEGOACT_JUMP3;
                        else if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object) &&
                                 LEGOACT_EXTRA_JUMP2 != -1 &&
                                 object->character_model->model_data_b[LEGOACT_EXTRA_JUMP2] != NULL)
                            action = LEGOACT_EXTRA_JUMP2;
                        else if (LEGOACT_JUMP2 != -1 && object->character_model->model_data_b[LEGOACT_JUMP2] != NULL)
                            action = LEGOACT_JUMP2;
                        else
                            action = LEGOACT_JUMP;
                        object->field_7aa = action;
                    }
                    object->context_timer = 0.0f;
                    object->flags_e20 |= 0x100000;
                    PlayJumpSfx(object, 1);
                    if ((animation_set & 0x10) == 0) {
                        object->context_variant_flags |= 0x1000;
                    }
                    return;
                }
                return;
            }
        } else {
            if ((animation_set & 0x80) == 0 && object->fall_acceleration_timer <= 0.0f &&
                (jump_pressed != 0 || ((object->portal_flags & 0x100) != 0 && flip_kind != 0)) &&
                (object->field_281 != 0 || aux->ground_grace_timer > 0.0f) &&
                (U64_PAIR(&CInfo[object->character_context].flags) & 0x1000) != 0 &&
                ((animation_set & 4) == 0 || (!(aux->jump_chain_timer > 0.0f) &&
                                              (object->is_underwater == 0 || aux->communication_blend == 0.0f)))) {
            jump_takeoff:;
                if (Jump_PreventJumpFn != NULL && Jump_PreventJumpFn(object))
                    return;
                if (LEGOCONTEXT_WEAPONOUT != -1 && object->character_context == LEGOCONTEXT_WEAPONOUT)
                    FastWeaponOut(object, 0);
                else if (LEGOCONTEXT_WEAPONIN != -1 && object->character_context == LEGOCONTEXT_WEAPONIN)
                    FastWeaponIn(object, 0);
                if (object->field_281 != 0)
                    object->flags_ef4 |= 0x20000000;
                if (aux->jump_reentry_timer <= 0.0f)
                    object->jump_sequence = 0;
                if (flip_kind == 0 ||
                    (!(object->delayed_turn_timer > 0.0f) && force_flip != 1 && (object->portal_flags & 0x100) == 0)) {
                    object->action_movement_state = (animation_set & 4) != 0 ? 5 : 0;
                    if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object) && LEGOACT_EXTRA_JUMP != -1 &&
                        object->character_model->model_data_b[LEGOACT_EXTRA_JUMP] != NULL)
                        object->field_7aa = LEGOACT_EXTRA_JUMP;
                    else
                        object->field_7aa = LEGOACT_JUMP;
                    if (CanMagnetClimbFn != NULL && CanMagnetClimbFn(object) && LEGOACT_MAGNET_JUMP != -1 &&
                        object->character_model->model_data_b[LEGOACT_MAGNET_JUMP] != NULL)
                        object->field_7aa = LEGOACT_MAGNET_JUMP;
                    object->jump_start_height = object->collision_min.y;
                    if ((animation_set & 1) == 0)
                        object->jump_sequence = 1;
                    else
                        ++object->jump_sequence;
                    if ((animation_set & 1) != 0 && object->jump_sequence > 1) {
                        if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object) && LEGOACT_EXTRA_JUMP2 != -1 &&
                            object->character_model->model_data_b[LEGOACT_EXTRA_JUMP2] != NULL)
                            object->field_7aa = LEGOACT_EXTRA_JUMP2;
                        else if (LEGOACT_JUMP2 != -1 && object->character_model->model_data_b[LEGOACT_JUMP2] != NULL)
                            object->field_7aa = LEGOACT_JUMP2;
                        else
                            object->field_7aa = LEGOACT_JUMP;
                        object->velocity.y = object->character_data->game_character->second_jump_speed;
                        ResetAnimPacket(&object->anim_packet, object->field_7aa);
                        object->context_variant_flags &= ~0x4000u;
                        if ((animation_set & 0x100) != 0)
                            MakeJumpReachHeight(object, HIGHJUMPHEIGHT, 1);
                    } else {
                        if ((animation_set & 0x40) != 0 &&
                            (object->field_7aa == -1 ||
                             object->character_model->model_data_b[object->field_7aa] == NULL))
                            object->field_7aa = LEGOACT_FALL;
                        object->velocity.y = object->character_data->game_character->jump_speed;
                        if (5 != object->action_movement_state)
                            ResetAnimPacket(&object->anim_packet, object->field_7aa);
                        if ((animation_set & 0x101) == 0x100)
                            MakeJumpReachHeight(object, HIGHJUMPHEIGHT, 1);
                    }
                } else {
                    if (1 == flip_kind && StartBackFlip(object))
                        return;
                    object->action_movement_state = 1;
                    object->field_7aa = LEGOACT_FLIP;
                    ResetAnimPacket(&object->anim_packet, LEGOACT_FLIP);
                    object->velocity.y = object->character_data->game_character->second_jump_speed;
                }
                object->context_timer = 0.0f;
                object->context_variant_flags &= ~(0x10000u | 0x1000u);
                object->character_context = LEGOCONTEXT_JUMP;
                if (object->action_movement_state == 1 && LEGOACT_FLIP != -1 &&
                    object->character_model->model_data_b[LEGOACT_FLIP] != NULL)
                    object->field_7aa = LEGOACT_FLIP;
                object->flags_e20 |= 0x100000;
                object->field_281 = 0;
                object->field_1050 = 0;
                object->delayed_turn_timer = 0.0f;
                if ((animation_set & 4) != 0) {
                    object->jump_variant_timer = object->character_data->game_character->jump_hold_duration;
                    if ((object->flags_1fc & 0x80) == 0)
                        object->jump_variant_timer = object->jump_variant_timer * 1.5f;
                    object->jump_variant_state = 1;
                }
                object->airborne_input_timer = 0.0f;
                object->airborne_collision_target = NULL;
                object->context_variant_flags &= ~0x8000u;
                PlayJumpSfx(object, object->jump_sequence > 1);
                if (object->field_283 == 1 && object->collision_min.y < object->water_height)
                    AddWaterSplash(object, &object->collision_position);
                return;
            }
        }
    }
}
#pragma data_seg(".data$BigJump_LandActionFn")
i32(__cdecl *BigJump_LandActionFn)(GAMEOBJECT *) = BigJump_LandAction_Default;
#pragma data_seg()

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
                NewRumbleAllPlayers(0.6f, 0.0f, 0, 0);
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

void __cdecl SetBallooningHeight(GAMEOBJECT *object, f32 height) {
    if (object->character_context == 0x5d)
        ((BALLOON_HEIGHT_VIEW_PC *)object)->height = height;
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
