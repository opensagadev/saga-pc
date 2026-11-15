#include "gameapi/ai/aisys/aisys.h"
#include "globals.h"
#include "legoapi/actions/combat/fighting.h"
#include "legoapi/actions/movement/climbing.h"
#include "legoapi/actions/movement/jumping.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/animation_ids.h"
#include "legoapi/characters/motion/animlist.h"
#include "legoapi/characters/motion/contexts.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/fx/explosions.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

struct minianimpacket_s {       // size 0x24
    f32 current_time;           // 0x00
    f32 previous_time;          // 0x04
    u8 unknown_08[0xc];         // 0x08
    f32 blend_target_time;      // 0x14
    u8 flags;                   // 0x18
    u8 blending;                // 0x19
    u8 unknown_1a[4];           // 0x1a
    i16 current_animation_id;   // 0x1e
    i16 previous_animation_id;  // 0x20
    i16 requested_animation_id; // 0x22
};

typedef struct beast_fall_land_view_s { // size 0xdb0
    u8 unknown_0[0x7d0];                // 0x000
    MINIANIMPACKET mini_animation;      // 0x7d0
    u8 unknown_7f4[0xdac - 0x7f4];      // 0x7f4
    f32 fall_animation_time;            // 0xdac
} BEAST_FALL_LAND_VIEW;

typedef struct jump_auxiliary_fields_s {
    u8 unknown_000[0x7b1];
    u8 landing_followup;
    u8 unknown_7b2[0x1e];
    MINIANIMPACKET mini_animation;
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
} JUMP_AUXILIARY_FIELDS;

void StartJump(GAMEOBJECT *object, i32 movement_state) {
    if (LEGOCONTEXT_JUMP != -1) {
        u32 attack_jump;
        object->character_context = (i8)LEGOCONTEXT_JUMP;
        object->action_movement_state = (u8)movement_state;
        object->unknown_7aa = LEGOACT_JUMP;
        object->context_timer = 0.0f;
        attack_jump = movement_state == JUMP_KIND_SLAM || movement_state == JUMP_KIND_LUNGE;
        object->jump_start_height = object->collision_min.y;
        object->airborne_action_timer = 0.0f;
        object->jump_sequence = 1;
        // Bits 12 and 15 cleared, bit 16 set for attack jumps (movement states 3 and 4).
        object->context_variant_flags = (object->context_variant_flags & ~0x19000) | ((attack_jump & 1) << 16);
        if ((u8)movement_state != JUMP_KIND_ZIPUP && (u8)movement_state != 7) {
            PlayJumpSfx(object, 0);
        }
    }
    object->unknown_281 = 0;
    object->unknown_1050 = 0;
    object->velocity.y = object->character_data->game_character->jump_speed;
    object->flags_e20 |= 0x100000;
    object->delayed_turn_timer = 0.0f;
    object->airborne_input_timer = 0.0f;
    object->airborne_collision_target = NULL;
    ResetAnimPacket(&object->anim_packet, -1);
    object->pitch_angle = 0;
    object->roll_angle = 0;
    object->flags_107a = 2;
}

void MakeJumpReachHeight(GAMEOBJECT *object, f32 height, i32 force) {
    f32 remaining_height = height - ((f64)object->collision_min.y - object->jump_start_height);
    if (remaining_height > 0.0f) {
        f32 radicand = (f64)object->character_data->game_character->gravity * -2.0 * remaining_height;
        f32 speed = NuFsqrt(radicand);
        if (force != 0 || speed > object->velocity.y) {
            object->velocity.y = speed;
        }
    } else if (force != 0) {
        object->velocity.y = 0.0f;
    }
}

void StartEndOfJump(GAMEOBJECT *object) {
    if (LEGOCONTEXT_JUMP != -1) {
        object->character_context = (i8)LEGOCONTEXT_JUMP;
        object->action_movement_state = 0;
        object->unknown_7aa = LEGOACT_FALL;
        object->jump_sequence = 2;
        object->context_variant_flags = (object->context_variant_flags & 0xfffeafff) | APIOBJECT_JUMP_FALLING;
    }
    object->airborne_collision_target = NULL;
}

static i32 BigJump_JumpAction_Default(GAMEOBJECT *object) {
    CHARACTER_MODEL_LAYERS *model;
    if (LEGOACT_COMBOJUMP != -1) {
        model = object->character_model;
        if (model->model_data_b[LEGOACT_COMBOJUMP] != NULL) {
            return LEGOACT_COMBOJUMP;
        }
    }
    if (LEGOACT_JUMP2 != -1) {
        model = object->character_model;
        if (model->model_data_b[LEGOACT_JUMP2] != NULL) {
            return LEGOACT_JUMP2;
        }
    }
    return LEGOACT_JUMP;
}

static i32 BigJump_LandAction_Default(GAMEOBJECT *object) {
    if (BigJumpLandPreferred != -1 && object->character_model->model_data_b[BigJumpLandPreferred] != NULL) {
        return BigJumpLandPreferred;
    }
    if (BigJumpLandSecondary != -1 && object->character_model->model_data_b[BigJumpLandSecondary] != NULL) {
        return BigJumpLandSecondary;
    }
    return BigJumpLandFallback;
}

f32 HIGHJUMPHEIGHT = 1.14f;
i32 (*BigJump_JumpActionFn)(GAMEOBJECT *) = BigJump_JumpAction_Default;

i32 StartBigJump(GAMEOBJECT *object, NUVEC *destination, i32 mode, f32 height, f32 duration_scale, i32 animation) {
    if (LEGOCONTEXT_BIGJUMP == -1) {
        return 0;
    }
    if ((object->character_data->model_flags & CHARACTER_MODEL_CANNOT_BIG_JUMP) != 0) {
        return 0;
    }
    Player_ClearContext(object, 0);
    Player_ResetContexts(object->ai_player_packet);
    object->big_jump_animation = (u8)animation;
    object->character_context = (i8)LEGOCONTEXT_BIGJUMP;
    object->unknown_7b3 = 1;
    object->unknown_7aa = (i16)BigJump_JumpActionFn(object);
    object->context_duration = AnimDuration((i16)object->id, object->unknown_7aa, 0.0f, 0.0f, 0);
    if (object->context_duration == 0.0f) {
        object->context_duration = 1.0f;
        object->unknown_7aa = LEGOACT_FALL;
    } else {
        ResetAnimPacket(&object->anim_packet, -1);
    }
    object->context_duration *= duration_scale;
    object->external_force = object->position;
    object->context_timer = 0.0f;
    object->launch_origin = *destination;
    object->launch_origin.y -= object->character_bottom * object->origin_scale;
    object->unknown_276 = object->facing_angle = object->movement_facing_angle =
        NuAtan2D(destination->x - object->external_force.x, destination->z - object->external_force.z);
    object->flags_ef0 = (object->flags_ef0 & ~0x2000) | ((mode & 1) << 13);
    PlayJumpSfx(object, 2);
    object->ai_packet.flags |= 0x2000000;
    object->ai_packet.path_target = NULL;
    object->big_jump_height = height;
    if (height < 0.0f) {
        object->big_jump_height = 0.0f;
    }
    object->context_variant_flags &= ~0x100;
    return 1;
}

i32 (*FindSlamOrigin_UseCPosFn)(GAMEOBJECT *) = 0;

void FindSlamOrigin(GAMEOBJECT *object, NUVEC *start, NUVEC *end) {
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
            object->character_model->points_of_interest[joint] == NULL) {
            *end = *start;
        } else {
            *end = object->locator_matrices[joint].translation;
        }
    }
}

i32 ObjLandReady(GAMEOBJECT *object) {
    i32 context = object->character_context;
    if ((U64_PAIR(&CInfo[context].flags) & 0x1004) != 0 &&
        (ZipUpLandCombatRollContext == -1 || context != ZipUpLandCombatRollContext || object->unknown_7b3 >= 1)) {
        if (ZipUpWallShuffleContext != -1 && context == ZipUpWallShuffleContext) {
            return 0;
        }
        return 1;
    }
    return 0;
}

i32 UseFallAnim(GAMEOBJECT *object) {
    if ((U64_PAIR(&CInfo[object->character_context].flags) & 8) != 0 && LEGOACT_FALL != -1 &&
        object->character_model->model_data_b[LEGOACT_FALL] != NULL) {
        return 1;
    }
    return 0;
}

i32 (*IsWearingBackPackFn)(GAMEOBJECT *) = 0;

i32 StartFallLand(GAMEOBJECT *object, i32 action) {
    BEAST_FALL_LAND_VIEW *view = (BEAST_FALL_LAND_VIEW *)object;
    PlayLandSfx(object, 0, 0);
    if (LEGOCONTEXT_LAND_JUMP != -1) {
        if (action != -1 && object->character_model->model_data_b[action] != NULL) {
            object->unknown_7aa = (i16)action;
        } else if (IsWearingBackPackFn != NULL && IsWearingBackPackFn(object) && LEGOACT_BACKPACKFALLLAND != -1 &&
                   object->character_model->model_data_b[LEGOACT_BACKPACKFALLLAND] != NULL) {
            object->unknown_7aa = LEGOACT_BACKPACKFALLLAND;
        } else if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object) && LEGOACT_EXTRA_LAND2 != -1 &&
                   object->character_model->model_data_b[LEGOACT_EXTRA_LAND2] != NULL) {
            object->unknown_7aa = LEGOACT_EXTRA_LAND2;
        } else if (LEGOACT_FALLLAND != -1 && object->character_model->model_data_b[LEGOACT_FALLLAND] != NULL) {
            object->unknown_7aa = LEGOACT_FALLLAND;
        } else if (BigJumpLandSecondary != -1 && object->character_model->model_data_b[BigJumpLandSecondary] != NULL) {
            object->unknown_7aa = BigJumpLandSecondary;
        } else {
            object->unknown_7aa = BigJumpLandFallback;
        }
        if (object->character_model->model_data_b[object->unknown_7aa] != NULL) {
            object->character_context = (i8)LEGOCONTEXT_LAND_JUMP;
            ResetAnimPacket(&object->anim_packet, -1);
            ResetMiniAnimPacket(&view->mini_animation, -1);
            view->fall_animation_time = 0.0f;
            object->context_timer = AnimDuration((i16)object->id, object->unknown_7aa, 0.0f, 0.0f, 1);
            object->flags_e24 = (object->flags_e24 & ~APIOBJECT_FALL_LANDING) | ((object->flags_e24 & APIOBJECT_FALL_LAND) << 1);
            return 1;
        }
    }
    object->flags_e24 &= ~APIOBJECT_FALL_LAND;
    return 0;
}

i32 StartBackFlip(GAMEOBJECT *object) {
    if (JumpBackFlipContext == -1 || JumpBackFlipAction == -1 || object->character_model->model_data_b[JumpBackFlipAction] == NULL) {
        return 0;
    }
    object->character_context = (i8)JumpBackFlipContext;
    ResetAnimPacket(&object->anim_packet, -1);
    object->unknown_7aa = JumpBackFlipAction;
    object->context_timer = AnimDuration((i16)object->id, object->unknown_7aa, 0.0f, 0.0f, 1);
    if (object->context_timer <= 0.0f) {
        object->context_timer = 1.0f;
    }
    object->flags_e20 |= 0x100000;
    object->delayed_turn_timer = 0.0f;
    object->context_variant_flags &= ~APIOBJECT_JUMP_FALLING;
    object->airborne_input_timer = 0.0f;
    PlayJumpSfx(object, 2);
    return 1;
}

void BackFlipCode(GAMEOBJECT *object) {
    if (JumpBackFlipContext == -1 || object->character_context != JumpBackFlipContext) {
        return;
    }
    if ((object->flags_e20 & 0x100000) != 0 && (object->pad->buttons_held & GAMEPAD_JUMP) == 0) {
        object->flags_e20 &= ~0x100000;
    }
    if (object->character_model->model_data_b[object->unknown_7aa] != NULL && CurrentAnim(&object->anim_packet) != object->unknown_7aa) {
        return;
    }
    object->context_timer -= FRAMETIME;
    if (!(object->context_timer <= 0.0f)) {
        return;
    }
    object->character_context = CHARACTER_CONTEXT_NONE;
    if ((object->pad->buttons_held & GAMEPAD_JUMP) != 0 && (object->flags_e20 & 0x100000) == 0) {
        StartBackFlip(object);
    } else {
        object->velocity.x = 0.0f;
        object->velocity.z = 0.0f;
    }
}

i32 Slam_Start(GAMEOBJECT *object, f32 speed) {
    if (LEGOCONTEXT_JUMP != -1) {
        object->character_context = (i8)LEGOCONTEXT_JUMP;
        object->action_movement_state = JUMP_KIND_SLAM;
        object->unknown_7aa = JumpSlamAction;
        object->context_timer = 0.0f;
        if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
            object->portal_flags |= 0x800;
        }
        object->velocity.y = speed;
        object->jump_sequence = 0;
        PlayJumpSfx(object, 4);
        NewRumble(object->pad->bound_pad, 0.5f, 0);
        object->context_variant_flags |= APIOBJECT_JUMP_ATTACK;
        return 1;
    }
    return 0;
}

f32 JumpFailedLandChainTime = 0.0f;
i32 DoubleJump_AlwaysReachJump2Height = 0;
i32 DoubleJump_JediSlam = 0;
i32 (*Jump_PreventJumpFn)(GAMEOBJECT *) = NULL;
void (*Jump_EndOfLandContextFn)(GAMEOBJECT *) = NULL;
i32 (*Slam_GetDebrisFn)(GAMEOBJECT *, i32) = NULL;
i32 (*CanGlideFn)(GAMEOBJECT *) = NULL;
i32 (*UsingExtraActionsFn)(GAMEOBJECT *) = 0;

void JumpCode(GAMEOBJECT *object, i32 jump_pressed, i32 jump_held, u32 animation_set, i32 action_pressed, i32 action_held, i32 hit_effect) {
    JUMP_AUXILIARY_FIELDS *aux = (JUMP_AUXILIARY_FIELDS *)object;
    WORLDINFO *world;
    i32 player_flag;
    i32 flip_kind;
    i32 force_flip;
    if ((object->flags_e24 & APIOBJECT_NO_JUMP) != 0) {
        return;
    }
    force_flip = -1;
    world = WorldInfo_CurrentlyActive();

    player_flag = (object->flags_1fc_word >> 7) & 1;
    flip_kind = 0;
    if (player_flag != 0 || (object->portal_flags & APIOBJECT_ALWAYS_BACK_FLIP) != 0) {
        if (JumpBackFlipAction != -1 && object->character_model->model_data_b[JumpBackFlipAction] != NULL) {
            flip_kind = 1;
        } else if ((animation_set & 2) != 0 && -1 != LEGOACT_FLIP && object->character_model->model_data_b[LEGOACT_FLIP] != NULL) {
            flip_kind = 2;
        }
    }
    if (aux->jump_reentry_timer > 0.0f) {
        aux->jump_reentry_timer -= FRAMETIME;
    }
    if (aux->jump_chain_timer > 0.0f) {
        aux->jump_chain_timer -= FRAMETIME;
    }

    if (object->character_context != CHARACTER_CONTEXT_NONE &&
        (object->character_context == LEGOCONTEXT_LAND_JUMP || object->character_context == LEGOCONTEXT_LAND_JUMP2 ||
         object->character_context == LEGOCONTEXT_LAND_FLIP || object->character_context == LEGOCONTEXT_LAND_COMBOJUMP ||
         object->character_context == LEGOCONTEXT_LAND_LUNGE || object->character_context == LEGOCONTEXT_LAND_SLAM ||
         object->character_context == ZipUpLandCombatRollContext)) {
        if (object->character_context == LEGOCONTEXT_LAND_LUNGE && (object->context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0) {
            const f32 frame = AnimListFrame(object->character_model, object->unknown_7aa, 0);
            if (frame > 0.0f) {
                f32 *time = AnimPlaying(&object->anim_packet, object->unknown_7aa, 1, 0);
                if (time != NULL && *time >= frame) {
                    if ((object->character_data->model_flags & CHARACTER_MODEL_JEDI) != 0) {
                        ComboHitFrame(object, 1);
                    } else {
                        Punch_Hit(object, object->force_target, PUNCHCHARGAP, PUNCHCHARGAP);
                    }
                }
            }
        } else if (object->character_context == ZipUpLandCombatRollContext) {
            const f32 frame = AnimStopFrame(object->character_model, object->unknown_7aa);
            if (frame > 0.0f) {
                f32 *time = AnimPlaying(&object->anim_packet, object->unknown_7aa, 1, 0);
                if (time != NULL && *time >= frame) {
                    object->unknown_7b3 = 1;
                }
            }
        }
        if (object->character_context == ZipUpLandCombatRollContext && object->unknown_7b3 == 1 && aux->landing_followup == 1) {
            goto jump_takeoff;
        }
        if (jump_pressed != 0 && (LEGOCONTEXT_LAND_LUNGE == -1 || object->character_context != LEGOCONTEXT_LAND_LUNGE) &&
            (LEGOCONTEXT_LAND_SLAM == -1 || object->character_context != LEGOCONTEXT_LAND_SLAM) &&
            (ZipUpLandCombatRollContext == -1 || object->character_context != ZipUpLandCombatRollContext)) {
            goto jump_takeoff;
        }
        if (((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 || object->character_context != LEGOCONTEXT_LAND_JUMP ||
             object->unknown_7aa == ACTION_NONE ||
             (object->unknown_7aa != LEGOACT_FALLLAND && object->unknown_7aa != LEGOACT_BACKPACKFALLLAND)) &&
            (LEGOCONTEXT_LAND_LUNGE == -1 || object->character_context != LEGOCONTEXT_LAND_LUNGE) &&
            (LEGOCONTEXT_LAND_SLAM == -1 || object->character_context != LEGOCONTEXT_LAND_SLAM) &&
            ((ZipUpLandCombatRollContext == -1 || object->character_context != ZipUpLandCombatRollContext) ||
             ((object->character_data->game_character->flags_094 & CHARACTER_OPTION_COMBAT_ROLL_CAN_GET_UP) != 0 &&
              !(object->unknown_7b3 == 1 && aux->landing_followup == 0))) &&
            object->pad->input_magnitude > 0.0f && (object->flags_e24 & APIOBJECT_FALL_LANDING) == 0) {
            object->character_context = CHARACTER_CONTEXT_NONE;
            return;
        }
        if (object->character_context == ZipUpLandCombatRollContext) {
            if (0 != jump_pressed) {
                aux->landing_followup = 1;
            } else if (action_pressed != 0 && LEGOACT_COMBATROLL_FIRE != -1 &&
                       object->character_model->model_data_b[LEGOACT_COMBATROLL_FIRE] != NULL) {
                aux->landing_followup = 2;
            }
        }
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f) {
            if (Jump_EndOfLandContextFn != NULL) {
                Jump_EndOfLandContextFn(object);
            }
            if (object->character_context == ZipUpLandCombatRollContext) {
                if (aux->landing_followup == 2) {
                    aux->action_suppressed = 0;
                    object->facing_angle = object->movement_facing_angle = object->unknown_276 = GamePad_InputAngle(object, object->pad);
                    StartQuickShoot(object, LEGOACT_COMBATROLL_FIRE);
                    object->quick_shoot_timer = 0.1f;
                    return;
                }
                if (aux->landing_followup == 1) {
                    goto jump_takeoff;
                }
                object->character_context = CHARACTER_CONTEXT_NONE;
                return;
            }
            if (LEGOCONTEXT_LAND_LUNGE != -1 && object->character_context == LEGOCONTEXT_LAND_LUNGE &&
                (object->context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0 &&
                (object->character_data->model_flags & CHARACTER_MODEL_JEDI) != 0) {
                ComboHitFrame(object, 1);
            }
            if ((object->character_context == LEGOCONTEXT_LAND_LUNGE || object->character_context == LEGOCONTEXT_LAND_SLAM) &&
                action_held != 0) {
                StartHold(object);
                return;
            }
            object->character_context = CHARACTER_CONTEXT_NONE;
        } else if (object->character_context == LEGOCONTEXT_LAND_SLAM) {
            i32 count = ParticlesPerSecond(25.0f, FRAMETIME);
            NUVEC start;
            NUVEC end;
            FindSlamOrigin(object, &start, &end);
            AddGameDebrisRot(world->game_debris_sys, hit_effect, &start, ParticlesPerSecond(20.0f, FRAMETIME), 0, 0);
            while (count > 0) {
                const f32 t = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f));
                NUVEC position;
                position.x = (end.x - start.x) * t + start.x;
                position.y = (end.y - start.y) * t + start.y;
                position.z = (end.z - start.z) * t + start.z;
                AddGameDebrisRot(world->game_debris_sys, hit_effect, &position, 1, 0, 0);
                --count;
            }
        }
    } else {
        if (LEGOCONTEXT_JUMP != -1 && object->character_context == LEGOCONTEXT_JUMP) {
            u32 variant_flags = object->context_variant_flags;
            f32 vertical_speed;
            u8 movement_state;
            i32 restart_jump;
            if ((variant_flags & 0x1000) != 0) {
                object->flags_e24 |= APIOBJECT_BOLTS_PASS;
            }
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
                if (object->jump_variant_timer <= 0.0f) {
                    object->jump_variant_state = 3;
                }
            }
            object->context_timer += FRAMETIME;
            if (movement_state == JUMP_KIND_COMBAT_ROLL && (variant_flags & APIOBJECT_JUMP_FALLING) == 0 &&
                object->character_model->model_data_b[object->unknown_7aa] != NULL && object->context_timer >= object->context_duration &&
                LEGOACT_COMBATROLL_FALL != -1 && object->character_model->model_data_b[LEGOACT_COMBATROLL_FALL] != NULL) {
                variant_flags |= APIOBJECT_JUMP_FALLING;
                object->context_variant_flags = variant_flags;
            }
            if ((movement_state == JUMP_KIND_COMBAT_ROLL || movement_state == JUMP_KIND_COMBO || movement_state == 0) &&
                object->context_timer >= 2.0f && object->position.y >= object->start_position.y && object->unknown_281 == 0) {
                object->character_context = CHARACTER_CONTEXT_NONE;
                aux->jump_reentry_timer = 0.0f;
                aux->fall_animation_timer = 0.2f;
                return;
            }

            if ((movement_state == 5 && (0 != ((object->unknown_281 & 1)) ||
                                         (player_flag == 0 && object->contact_count != 0 && object->jump_variant_state == 3))) ||
                (movement_state != 5 && object->unknown_281 != 0 && object->context_timer >= 0.1f) ||
                ((movement_state == JUMP_KIND_SLAM || movement_state == JUMP_KIND_LUNGE) && object->context_timer >= 2.5f)) {
                restart_jump = 0;
                if (player_flag != 0) {
                    object->portal_flags |= 0x800;
                }
                if (jump_held != 0 && 0 == ((object->flags_e20 & 0x100000)) && object->action_movement_state != JUMP_KIND_LUNGE &&
                    object->action_movement_state != JUMP_KIND_SLAM && object->action_movement_state != 5 &&
                    object->action_movement_state != JUMP_KIND_COMBAT_ROLL) {
                    if ((object->action_movement_state == JUMP_KIND_FLIP && object->pad->input_magnitude > 0.0f &&
                         (0x200000 & object->flags_e20) != 0 &&
                         abs(RotDiff(object->movement_facing_angle, object->current_input_angle)) > 0x6aaa) ||
                        (object->action_movement_state == JUMP_KIND_COMBO && object->pad->input_magnitude > 0.0f &&
                         (0x200000 & object->flags_e20) != 0 &&
                         abs(RotDiff(object->movement_facing_angle, object->current_input_angle)) < 0x1555)) {
                        force_flip = 1;
                    }
                    restart_jump = 1;
                }
                {
                    i32 land_sfx = 0;
                    u8 state = object->action_movement_state;
                    if (state == 3) {
                        if (!(LEGOCONTEXT_LAND_LUNGE != -1 && LEGOACT_LUNGELAND != -1 &&
                              object->character_model->model_data_b[LEGOACT_LUNGELAND] != NULL)) {
                            if ((object->character_data->model_flags & CHARACTER_MODEL_JEDI) == 0) {
                                Punch_Hit(object, object->force_target, PUNCHCHARGAP, PUNCHCHARGAP);
                                object->character_context = CHARACTER_CONTEXT_NONE;
                                aux->jump_reentry_timer = 0.2f;
                                if (object->action_movement_state == 5) {
                                    aux->jump_chain_timer = JumpFailedLandChainTime;
                                }
                            } else {
                                object->character_context = CHARACTER_CONTEXT_NONE;
                                aux->jump_reentry_timer = 0.2f;
                            }
                        } else {
                            NewRumble(object->pad->bound_pad, 0.6f, 0);
                            object->character_context = LEGOCONTEXT_LAND_LUNGE;
                            object->unknown_7aa = LEGOACT_LUNGELAND;
                            object->context_timer = AnimDuration((i16)object->id, object->unknown_7aa, 0.0f, 0.0f, 1);
                            object->context_variant_flags = object->context_variant_flags & (~APIOBJECT_CONTEXT_EVENT_DONE);
                            aux->jump_reentry_timer = 0.0f;
                            if (!restart_jump) {
                                ResetAnimPacket(&object->anim_packet, -1);
                            }
                            if ((object->character_data->model_flags & CHARACTER_MODEL_JEDI) == 0 &&
                                AnimListFrame(object->character_model, object->unknown_7aa, 0) == 0.0f) {
                                Punch_Hit(object, object->force_target, PUNCHCHARGAP, PUNCHCHARGAP);
                            }
                            land_sfx = 1;
                        }
                    } else if (state == 4) {
                        if (LEGOCONTEXT_LAND_SLAM != -1 && LEGOACT_SLAMLAND != -1 &&
                            object->character_model->model_data_b[LEGOACT_SLAMLAND] != NULL) {
                            f32 duration;
                            GameCam_Judder(GameCam, 0.5f, -1, &object->collision_position);
                            NewRumble(object->pad->bound_pad, 0.75f, 0);
                            object->slam_effect = hit_effect;
                            if (Slam_GetDebrisFn != NULL) {
                                object->slam_effect = Slam_GetDebrisFn(object, hit_effect);
                            }
                            object->character_context = LEGOCONTEXT_LAND_SLAM;
                            object->unknown_7aa = LEGOACT_SLAMLAND;
                            duration = AnimDuration((i16)object->id, object->unknown_7aa, 0.0f, 0.0f, 1);
                            object->context_timer = duration <= 0.0f ? 0.75f : duration;
                            aux->jump_reentry_timer = 0.0f;
                            if (!restart_jump) {
                                ResetAnimPacket(&object->anim_packet, -1);
                                ResetMiniAnimPacket(&aux->mini_animation, -1);
                            }
                            land_sfx = 2;
                        } else {
                            object->character_context = CHARACTER_CONTEXT_NONE;
                            aux->jump_reentry_timer = 0.2f;
                        }
                    } else if (state == 2 && LEGOCONTEXT_LAND_COMBOJUMP != -1 && BigJumpLandPreferred != -1 &&
                               object->character_model->model_data_b[BigJumpLandPreferred] != NULL) {
                        if ((object->context_variant_flags & APIOBJECT_JUMP_FLIP) == 0) {
                            object->unknown_276 += 0x8000;
                            object->facing_angle += 0x8000;
                            object->movement_facing_angle += 0x8000;
                            if (!restart_jump) {
                                ResetAnimPacket(&object->anim_packet, -1);
                            }
                        }
                        aux->jump_reentry_timer = 0.0f;
                        if (object->pad->input_magnitude == 0.0f) {
                            object->character_context = LEGOCONTEXT_LAND_COMBOJUMP;
                            if ((object->context_variant_flags & APIOBJECT_JUMP_FLIP) != 0 && -1 != LEGOACT_FLIPLAND &&
                                object->character_model->model_data_b[LEGOACT_FLIPLAND] != NULL) {
                                object->unknown_7aa = LEGOACT_FLIPLAND;
                            } else {
                                object->unknown_7aa = BigJumpLandPreferred;
                            }
                            object->context_timer = AnimDuration((i16)object->id, object->unknown_7aa, 0.0f, 0.0f, 1);
                        } else {
                            object->character_context = CHARACTER_CONTEXT_NONE;
                        }
                    } else if (state == 1 && LEGOCONTEXT_LAND_FLIP != -1 && LEGOACT_FLIPLAND != -1 &&
                               object->character_model->model_data_b[LEGOACT_FLIPLAND] != NULL) {
                        aux->jump_reentry_timer = 0.0f;
                        if (object->pad->input_magnitude == 0.0f) {
                            object->character_context = LEGOCONTEXT_LAND_FLIP;
                            object->unknown_7aa = LEGOACT_FLIPLAND;
                            object->context_timer = AnimDuration((i16)object->id, object->unknown_7aa, 0.0f, 0.0f, 1);
                            if (!restart_jump) {
                                ResetAnimPacket(&object->anim_packet, -1);
                            }
                        } else {
                            object->character_context = CHARACTER_CONTEXT_NONE;
                        }
                    } else if (state == 8) {
                        if (ZipUpLandCombatRollContext != -1 && object->character_model->model_data_b[LEGOACT_COMBATROLL_LAND] != NULL) {
                            NewRumble(object->pad->bound_pad, 0.4f, 0);
                            object->character_context = ZipUpLandCombatRollContext;
                            object->unknown_7b3 = 0;
                            object->unknown_7aa = LEGOACT_COMBATROLL_LAND;
                            object->context_timer = AnimDuration((i16)object->id, object->unknown_7aa, 0.0f, 0.0f, 1);
                            object->context_variant_flags &= ~APIOBJECT_CONTEXT_EVENT_DONE;
                            aux->jump_reentry_timer = 0.0f;
                            if (!restart_jump) {
                                ResetAnimPacket(&object->anim_packet, -1);
                            }
                            land_sfx = 4;
                            aux->landing_followup = 0;
                            if (jump_held != 0 && (object->flags_e20 & 0x100000) == 0) {
                                aux->landing_followup = 1;
                            } else if (action_held != 0 && -1 != LEGOACT_COMBATROLL_FIRE &&
                                       object->character_model->model_data_b[LEGOACT_COMBATROLL_FIRE] != NULL) {
                                aux->landing_followup = 2;
                            }
                        } else {
                            object->character_context = CHARACTER_CONTEXT_NONE;
                            aux->jump_reentry_timer = 0.2f;
                        }
                    } else {
                        i32 land3 = (object->context_variant_flags & APIOBJECT_JUMP_DOUBLE) != 0;
                        if (state == 0 && object->jump_sequence > 1 && LEGOCONTEXT_LAND_JUMP2 != -1 &&
                            ((BigJumpLandSecondary != -1 && object->character_model->model_data_b[BigJumpLandSecondary] != NULL) ||
                             (land3 && LEGOACT_LAND3 != -1 && object->character_model->model_data_b[LEGOACT_LAND3] != NULL))) {
                            aux->jump_reentry_timer = 0.0f;
                            if (object->pad->input_magnitude != 0.0f) {
                                object->character_context = CHARACTER_CONTEXT_NONE;
                            } else if (aux->fall_animation_timer >= 0.2f) {
                                StartFallLand(object, -1);
                            } else {
                                object->character_context = LEGOCONTEXT_LAND_JUMP2;
                                if (land3 && LEGOACT_LAND3 != -1 && object->character_model->model_data_b[LEGOACT_LAND3] != NULL) {
                                    object->unknown_7aa = LEGOACT_LAND3;
                                } else if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object) && LEGOACT_EXTRA_LAND2 != -1 &&
                                           object->character_model->model_data_b[LEGOACT_EXTRA_LAND2] != NULL) {
                                    object->unknown_7aa = LEGOACT_EXTRA_LAND2;
                                } else {
                                    object->unknown_7aa = BigJumpLandSecondary;
                                }
                                object->context_timer = AnimDuration((i16)object->id, object->unknown_7aa, 0.0f, 0.0f, 1);
                                if (!restart_jump) {
                                    ResetAnimPacket(&object->anim_packet, -1);
                                }
                            }
                        } else if ((state == 0 || state == 6 || state == 7 || state == 9) && -1 != LEGOCONTEXT_LAND_JUMP &&
                                   BigJumpLandFallback != -1 && object->character_model->model_data_b[BigJumpLandFallback] != NULL &&
                                   object->pad->input_magnitude == 0.0f) {
                            if (aux->fall_animation_timer >= 0.2f) {
                                StartFallLand(object, -1);
                            } else {
                                object->character_context = LEGOCONTEXT_LAND_JUMP;
                                if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object) && LEGOACT_EXTRA_LAND != -1 &&
                                    object->character_model->model_data_b[LEGOACT_EXTRA_LAND] != NULL) {
                                    object->unknown_7aa = LEGOACT_EXTRA_LAND;
                                } else {
                                    object->unknown_7aa = BigJumpLandFallback;
                                }
                                if (CanMagnetClimbFn != NULL && CanMagnetClimbFn(object) && LEGOACT_MAGNET_JUMP != -1 &&
                                    object->character_model->model_data_b[LEGOACT_MAGNET_JUMP] != NULL) {
                                    object->unknown_7aa = LEGOACT_MAGNET_JUMP;
                                }
                                object->context_timer = AnimDuration((i16)object->id, object->unknown_7aa, 0.0f, 0.0f, 1);
                                aux->jump_reentry_timer = object->jump_sequence > 1 ? 0.0f : 0.2f;
                                if (!restart_jump) {
                                    ResetAnimPacket(&object->anim_packet, -1);
                                }
                            }
                        } else {
                            object->character_context = CHARACTER_CONTEXT_NONE;
                            aux->jump_reentry_timer = 0.2f;
                            if (state == 5) {
                                aux->jump_chain_timer = JumpFailedLandChainTime;
                            }
                        }
                    }
                    PlayLandSfx(object, land_sfx, 0);
                }
                if (restart_jump) {
                    UpdateLastSafePosition(object);
                    goto jump_takeoff;
                }
                return;
            } else {
                PLAYERCHARACTERCONFIG *game_character;
                i32 can_start_second_jump;
                if (action_pressed != 0 && object->velocity.y > -1.25f &&
                    (object->character_data->game_character->hover_fall_mode != 0 ||
                     (object->character_data->model_flags & CHARACTER_MODEL_JEDI) != 0) &&
                    movement_state == 0 && (object->context_variant_flags & APIOBJECT_JUMP_FALLING) == 0 && object->jump_sequence < 2 &&
                    LEGOACT_LUNGE != -1 && object->character_model->model_data_b[LEGOACT_LUNGE] != NULL) {
                    object->force_target = NULL;
                    object->blowup_target = NULL;
                    StartLunge(object, 0.0f, object->bounds_half_height);
                    return;
                }
                game_character = object->character_data->game_character;
                if (DoubleJump_JediSlam != 0 && (animation_set & 0x10) != 0 && action_pressed != 0 && object->velocity.y > -1.25f &&
                    (object->context_variant_flags & APIOBJECT_JUMP_FALLING) == 0 &&
                    ((movement_state == 0 && (object->jump_sequence == 2 || game_character->single_jump_slam)) ||
                     movement_state == JUMP_KIND_FLIP || movement_state == JUMP_KIND_COMBO) &&
                    JumpSlamAction != -1 && object->character_model->model_data_b[JumpSlamAction] != NULL) {
                    if (Slam_Start(object, SLAMJUMPSPEED) != 0) {
                        PlaySabreSfx(NULL, object, NULL, 0);
                        if (object->action_movement_state == JUMP_KIND_FLIP || object->action_movement_state == JUMP_KIND_COMBO) {
                            ResetAnimPacket(&object->anim_packet, -1);
                            if (object->action_movement_state == JUMP_KIND_COMBO &&
                                (object->context_variant_flags & APIOBJECT_JUMP_FLIP) == 0) {
                                object->unknown_276 += 0x8000;
                                object->facing_angle += 0x8000;
                                object->movement_facing_angle += 0x8000;
                            }
                        }
                        return;
                    }
                }
                can_start_second_jump = jump_pressed != 0 && (animation_set & 8) != 0 && object->action_movement_state == 0 &&
                                        object->jump_sequence < 2 && object->velocity.y > -1.25f;
                if (can_start_second_jump) {
                    if (LEGOCONTEXT_DOOMED != -1 && object->character_context == LEGOCONTEXT_DOOMED) {
                        return;
                    }
                    game_character = object->character_data->game_character;
                    if (game_character->double_jump_hover) {
                        object->jump_variant_timer = 0.0f;
                        object->character_context = CHARACTER_CONTEXT_NONE;
                        object->jump_variant_state = 1;
                        FastWeaponOut(object, 1);
                        return;
                    }
                    if ((animation_set & 0x110) == 0 &&
                        (game_character->flags_090 & (CHARACTER_RUNTIME_HIGH_JUMP | CHARACTER_RUNTIME_COMBAT_ROLL)) == 0) {
                        return;
                    }
                    if ((animation_set & 0x100) != 0) {
                        MakeJumpReachHeight(object, HIGHJUMPHEIGHT, 1);
                    } else if (0 != ((animation_set & 0x200))) {
                        MakeJumpReachHeight(object, game_character->jump_height, 0);
                    } else if (DoubleJump_AlwaysReachJump2Height != 0 || object->velocity.y > 0.0f) {
                        MakeJumpReachHeight(object, game_character->second_jump_height, 0);
                    } else {
                        object->velocity.y = 0.0f;
                    }
                    if (CanGlideFn != NULL && CanGlideFn(object) && Glide_Start(object)) {
                        return;
                    }
                    object->jump_sequence++;
                    object->context_variant_flags |= APIOBJECT_JUMP_DOUBLE;
                    if ((animation_set & 0x110) == 0 &&
                        0 == ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_HIGH_JUMP))) {
                        if ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_COMBAT_ROLL) != 0) {
                            object->action_movement_state = JUMP_KIND_COMBAT_ROLL;
                            object->context_variant_flags &= ~APIOBJECT_JUMP_FALLING;
                            object->unknown_7aa = LEGOACT_COMBATROLL_JUMP;
                            object->context_duration = AnimDuration((i16)object->id, object->unknown_7aa, 0.0f, 0.0f, 1);
                        }
                    } else {
                        i16 action;
                        object->action_movement_state = 0;
                        if (LEGOACT_JUMP3 != -1 && object->character_model->model_data_b[LEGOACT_JUMP3] != NULL) {
                            action = LEGOACT_JUMP3;
                        } else if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object) && LEGOACT_EXTRA_JUMP2 != -1 &&
                                   object->character_model->model_data_b[LEGOACT_EXTRA_JUMP2] != NULL) {
                            action = LEGOACT_EXTRA_JUMP2;
                        } else if (LEGOACT_JUMP2 != -1 && object->character_model->model_data_b[LEGOACT_JUMP2] != NULL) {
                            action = LEGOACT_JUMP2;
                        } else {
                            action = LEGOACT_JUMP;
                        }
                        object->unknown_7aa = action;
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
            if ((animation_set & 0x80) == 0 && LEGOCONTEXT_JUMP != -1 && object->fall_acceleration_timer <= 0.0f &&
                (jump_pressed != 0 || ((object->portal_flags & APIOBJECT_ALWAYS_BACK_FLIP) != 0 && flip_kind != 0)) &&
                (object->unknown_281 != 0 || aux->ground_grace_timer > 0.0f) &&
                (U64_PAIR(&CInfo[object->character_context].flags) & 0x1000) != 0 &&
                ((animation_set & 4) == 0 ||
                 (!(aux->jump_chain_timer > 0.0f) && (object->is_underwater == 0 || aux->communication_blend == 0.0f)))) {
            jump_takeoff:;
                if (Jump_PreventJumpFn != NULL && Jump_PreventJumpFn(object)) {
                    return;
                }
                if (LEGOCONTEXT_WEAPONOUT != -1 && object->character_context == LEGOCONTEXT_WEAPONOUT) {
                    FastWeaponOut(object, 0);
                } else if (LEGOCONTEXT_WEAPONIN != -1 && object->character_context == LEGOCONTEXT_WEAPONIN) {
                    FastWeaponIn(object, 0);
                }
                if (object->unknown_281 != 0) {
                    object->flags_ef4 |= 0x20000000;
                }
                if (aux->jump_reentry_timer <= 0.0f) {
                    object->jump_sequence = 0;
                }
                if (flip_kind == 0 ||
                    (!(object->delayed_turn_timer > 0.0f) && force_flip != 1 && (object->portal_flags & APIOBJECT_ALWAYS_BACK_FLIP) == 0)) {
                    if ((animation_set & 4) != 0) {
                        object->action_movement_state = 5;
                    } else {
                        object->action_movement_state = 0;
                    }
                    if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object) && LEGOACT_EXTRA_JUMP != -1 &&
                        object->character_model->model_data_b[LEGOACT_EXTRA_JUMP] != NULL) {
                        object->unknown_7aa = LEGOACT_EXTRA_JUMP;
                    } else {
                        object->unknown_7aa = LEGOACT_JUMP;
                    }
                    if (CanMagnetClimbFn != NULL && CanMagnetClimbFn(object) && LEGOACT_MAGNET_JUMP != -1 &&
                        object->character_model->model_data_b[LEGOACT_MAGNET_JUMP] != NULL) {
                        object->unknown_7aa = LEGOACT_MAGNET_JUMP;
                    }
                    object->jump_start_height = object->collision_min.y;
                    if ((animation_set & 1) == 0) {
                        object->jump_sequence = 0;
                    }
                    object->jump_sequence++;
                    if (object->jump_sequence > 1) {
                        if (UsingExtraActionsFn != NULL && UsingExtraActionsFn(object) && LEGOACT_EXTRA_JUMP2 != -1 &&
                            object->character_model->model_data_b[LEGOACT_EXTRA_JUMP2] != NULL) {
                            object->unknown_7aa = LEGOACT_EXTRA_JUMP2;
                        } else if (LEGOACT_JUMP2 != -1 && object->character_model->model_data_b[LEGOACT_JUMP2] != NULL) {
                            object->unknown_7aa = LEGOACT_JUMP2;
                        } else {
                            object->unknown_7aa = LEGOACT_JUMP;
                        }
                        object->velocity.y = object->character_data->game_character->second_jump_speed;
                        ResetAnimPacket(&object->anim_packet, object->unknown_7aa);
                        object->context_variant_flags &= ~APIOBJECT_JUMP_DOUBLE;
                        if ((animation_set & 0x100) != 0) {
                            MakeJumpReachHeight(object, HIGHJUMPHEIGHT, 1);
                        }
                    } else {
                        if ((animation_set & 0x40) != 0 &&
                            (object->unknown_7aa == ACTION_NONE || object->character_model->model_data_b[object->unknown_7aa] == NULL)) {
                            object->unknown_7aa = LEGOACT_FALL;
                        }
                        object->velocity.y = object->character_data->game_character->jump_speed;
                        if (5 != object->action_movement_state) {
                            ResetAnimPacket(&object->anim_packet, object->unknown_7aa);
                        }
                        if ((animation_set & 0x101) == 0x100) {
                            MakeJumpReachHeight(object, HIGHJUMPHEIGHT, 1);
                        }
                    }
                } else {
                    if (1 == flip_kind && StartBackFlip(object)) {
                        return;
                    }
                    object->action_movement_state = JUMP_KIND_FLIP;
                    object->unknown_7aa = LEGOACT_FLIP;
                    ResetAnimPacket(&object->anim_packet, object->unknown_7aa);
                    object->velocity.y = object->character_data->game_character->second_jump_speed;
                }
                object->character_context = LEGOCONTEXT_JUMP;
                object->context_timer = 0.0f;
                object->context_variant_flags &= ~(APIOBJECT_JUMP_ATTACK | 0x1000u);
                if (object->action_movement_state == JUMP_KIND_FLIP && LEGOACT_FLIP != -1 &&
                    object->character_model->model_data_b[LEGOACT_FLIP] != NULL) {
                    object->unknown_7aa = LEGOACT_FLIP;
                }
                object->flags_e20 |= 0x100000;
                object->unknown_281 = 0;
                object->unknown_1050 = 0;
                object->delayed_turn_timer = 0.0f;
                if ((animation_set & 4) != 0) {
                    object->jump_variant_timer = object->character_data->game_character->jump_hold_duration;
                    if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0) {
                        object->jump_variant_timer = object->jump_variant_timer * 1.5f;
                    }
                    object->jump_variant_state = 1;
                }
                object->airborne_input_timer = 0.0f;
                object->airborne_collision_target = NULL;
                object->context_variant_flags &= ~APIOBJECT_JUMP_FALLING;
                PlayJumpSfx(object, object->jump_sequence > 1);
                if (object->unknown_283 == 1 && object->collision_min.y < object->water_height) {
                    AddWaterSplash(object, &object->collision_position);
                }
                return;
            }
        }
    }
}

i32 (*BigJump_LandActionFn)(GAMEOBJECT *) = BigJump_LandAction_Default;
f32 SLAMJUMPSPEED = 3.0f;
f32 PUNCHCHARGAP = 0.6f;

void (*BigJump_EndOfLandFn)(GAMEOBJECT *) = 0;

void BigJumpCode(GAMEOBJECT *object) {
    WORLDINFO *world;
    f32 fraction;
    f32 height;
    NUVEC origin;
    i32 angle;
    if (LEGOCONTEXT_BIGJUMP == -1 || object->character_context != LEGOCONTEXT_BIGJUMP) {
        return;
    }
    object->respawn_timer = 0.0f;
    if (object->big_jump_animation != 0 && object->contact_count != 0) {
        object->character_context = CHARACTER_CONTEXT_NONE;
        return;
    }
    if (object->unknown_7b3 == 2) {
        object->context_timer += FRAMETIME;
        if (object->context_timer >= object->context_duration) {
            object->character_context = CHARACTER_CONTEXT_NONE;
            if (BigJump_EndOfLandFn != NULL) {
                BigJump_EndOfLandFn(object);
            }
        }
    } else if (object->unknown_7b3 == 1) {
        object->context_timer += FRAMETIME;
        if (object->context_timer >= object->context_duration) {
            if (LEGOACT_FALL != -1 && object->unknown_7aa == LEGOACT_FALL) {
                object->character_context = CHARACTER_CONTEXT_NONE;
            } else {
                world = WorldInfo_CurrentlyActive();
                if (LEGOACT_COMBOJUMP != -1 && object->unknown_7aa == LEGOACT_COMBOJUMP) {
                    object->facing_angle += 0x8000;
                    object->movement_facing_angle += 0x8000;
                    object->unknown_276 += 0x8000;
                    ResetAnimPacket(&object->anim_packet, -1);
                }
                if ((object->context_variant_flags & 0x100) != 0 && object->unknown_281 == 0) {
                    height = GameShadow(NULL, &object->launch_origin, 5.0f, -1);
                    if (height != 2000000.0f && object->launch_origin.y - height >= 0.1f) {
                        StartEndOfJump(object);
                        return;
                    }
                }
                ResetAnimPacket(&object->anim_packet, -1);
                object->unknown_7b3 = 2;
                object->unknown_7aa = (i16)BigJump_LandActionFn(object);
                object->context_duration = AnimDuration((i16)object->id, object->unknown_7aa, 0.0f, 0.0f, 1);
                if (object->context_duration == 0.0f) {
                    object->context_duration = 1.0f;
                }
                object->context_timer = 0.0f;
                if ((object->portal_flags & APIOBJECT_BOSS) != 0 || (object->flags_ef0 & 0x2000) != 0) {
                    GameCam_Judder(GameCam, -0.3f, 0, &object->collision_position);
                    PlayLandSfx(object, 3, 0);
                    NewRumbleAllPlayers(0.6f, 0.0f, 0, 0);
                } else if ((object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                    GameCam_Judder(GameCam, -0.2f, 0, &object->collision_position);
                    PlayLandSfx(object, 3, 0);
                } else {
                    PlayLandSfx(object, 0, 0);
                }
                object->external_force = object->position;
                object->velocity.x = object->velocity.y = object->velocity.z = 0.0f;
                if ((object->flags_ef0 & 0x2000) != 0) {
                    FindSlamOrigin(object, &origin, NULL);
                    AddExplosion(&origin, 2.0f, 0.4f, object, object->slam_effect,
                                 EXPLOSION_HITS_CHARACTERS | EXPLOSION_KNOCKBACK | EXPLOSION_AS_BOLT);
                }
                AISysGetCharacterPathPos(world->ai_sys, (APIOBJECT *)object, &object->ai_packet, 0xff, 1);
            }
        } else {
            fraction = object->context_timer / object->context_duration;
            object->position.x = object->external_force.x + (object->launch_origin.x - object->external_force.x) * fraction;
            object->position.y = object->external_force.y + (object->launch_origin.y - object->external_force.y) * fraction;
            object->position.z = object->external_force.z + (object->launch_origin.z - object->external_force.z) * fraction;
            angle = (i32)(fraction * 32768.0);
            object->position.y += NuTrigTable[(angle >> 1) & 0x7fff] * object->character_data->game_character->second_jump_height * 2.0f *
                                  object->big_jump_height;
            NuVecSub(&object->velocity, &object->position, &object->start_position);
            NuVecScale(&object->velocity, &object->velocity, 1.0f / FRAMETIME);
        }
    }
    if (object->character_context != CHARACTER_CONTEXT_NONE) {
        object->ai_packet.flags |= 0x2000000;
    }
}
