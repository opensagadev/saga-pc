#include "globals.h"
#include "legoapi/actions/character/speederchase.h"
#include "legoapi/actions/character/transform.h"
#include "legoapi/actions/combat/hits.h"
#include "legoapi/actions/movement/carrying.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/animation_ids.h"
#include "legoapi/characters/motion/animlist.h"
#include "legoapi/characters/motion/contexts.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmos/door/plugs.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/menus/core/gamemessages.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/render.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

i32 SuperCarry_TurnBlowupBackOn(GIZMOBLOWUP *blowup, NUVEC *position, u16 angle, i32 keep_matrix) {
    CARRY_PLUG *plug;
    f32 distance;
    blowup->status_flags |= BLOWUP_STATUS_VISIBLE | BLOWUP_STATUS_ACTIVE;
    GizBlowup_Respawn(blowup);
    blowup->status_flags |= 0x10000;
    plug = Plug_FindNearest(WORLD->plug_sys, position, &distance, 1);
    if (plug != NULL && distance < 0.5625f) {
        Plug_MakeDrawMtx(plug, &blowup->transform);
        plug->plugged = 1;
        blowup->status_flags = (blowup->status_flags & ~BLOWUP_STATUS_ACTIVE) | BLOWUP_STATUS_PLUGGING;
        return 1;
    }
    if (!keep_matrix) {
        if (SuperCarry_AlignObject) {
            NuMtxSetRotationY(&blowup->transform, angle);
        } else {
            blowup->transform = NuMtxSetIdentityTemplate;
        }
        NuMtxTranslate(&blowup->transform, position);
    }
    return 0;
}

void SuperCarry_PartImpact(PART *part) {
    f32 amount = NuVecMag(&part->velocity);
    if (amount >= SuperCarryImpactSpeed) {
        amount = 1.0f;
    } else {
        amount /= SuperCarryImpactSpeed;
    }
    if (amount > 0.0f) {
        GameCam_Judder(NULL, (f32)(amount * 0.30000001192092896), qrand() < 0x8000 ? 0 : 2, &part->position);
    }
}

static void SuperCarry_PartKill(PART *part, i32 unused) {
    GIZMOBLOWUP *blowup = part->supercarry_blowup;
    if (blowup->secondary_flags & 4) {
        SuperCarry_TurnBlowupBackOn(blowup, &part->position, part->rotation_y, 0);
    } else {
        GizmoBlowUp_AddEffects(&part->position, blowup, 0,
                               BLOWUP_EFFECT_DEBRIS | BLOWUP_EFFECT_PARTICLES | BLOWUP_EFFECT_PICKUPS | BLOWUP_EFFECT_EXPLOSION, NULL);
    }
}

void SuperCarry_DrawObject(struct gameobject_s *object) {
    NUMTX matrix;
    NUVEC offset;
    struct nuvec_s *translation;
    PLAYERCHARACTERCONFIG *config;
    struct GIZMOBLOWUP *blowup;
    if ((object->player.flags_e20 & 0x800000) != 0 ||
        (LEGOCONTEXT_SUPERCARRY != -1 && object->player.character_context == LEGOCONTEXT_SUPERCARRY &&
         ((object->player.context_phase == SUPERCARRY_PICKUP &&
           (object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) != 0) ||
          (object->player.context_phase == SUPERCARRY_PUTDOWN &&
           (object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0) ||
          object->player.context_phase == SUPERCARRY_IDLE || object->player.context_phase == SUPERCARRY_WALK ||
          object->player.context_phase == SUPERCARRY_JUMP || object->player.context_phase == SUPERCARRY_LAND ||
          (object->player.context_phase == SUPERCARRY_THROW &&
           (object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0) ||
          (object->player.context_phase == SUPERCARRY_BASH &&
           (object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0)))) {
        translation = NULL;
        if (SuperCarry_AlignObject != 0) {
            NuVecRotateY(&offset, &object->player.supercarry.basis[0], object->movement_angle);
            translation = &offset;
            NuMtxSetRotationY(&matrix, object->player.takeover_start_angle);
        } else {
            matrix = NuMtxSetIdentityTemplate;
            *NUMTX_GET_ROW_VEC(&matrix, 0) = object->player.supercarry.basis[0];
            *NUMTX_GET_ROW_VEC(&matrix, 1) = object->player.supercarry.basis[1];
            *NUMTX_GET_ROW_VEC(&matrix, 2) = object->player.supercarry.basis[2];
        }
        if (SuperCarry_KeepObjectLevel != 0) {
            NuMtxRotateY(&matrix, object->movement_angle);
            dco_locatorposonly = 1;
        }
        config = object->character_data->game_character;
        blowup = (GIZMOBLOWUP *)object->player.grapple;
        DrawObjectOnCharacter(WORLD, object, -1, &blowup->type->animated_special, config->hand_locators[0], config->hand_locators[1],
                              object->player.locator_matrices, object->reflection_active, object->layer_mask, &matrix, translation, 1.0f,
                              1.0f);
    }
}

void SuperCarry_GetObjectPos(GAMEOBJECT *object, NUVEC *position, NUVEC *secondary_position) {
    i32 first;
    i32 second;
    NUVEC offset;
    *position = object->collision_position;
    first = object->character_data->game_character->hand_locators[0];
    if (first != -1 && object->character_model->points_of_interest[first] != NULL) {
        second = object->character_data->game_character->hand_locators[1];
        if (second != -1 && object->character_model->points_of_interest[second] != NULL) {
            *position = object->player.locator_matrices[first].translation;
            position->x += (object->player.locator_matrices[second].m30 - position->x) * 0.5f;
            position->y += (object->player.locator_matrices[second].m31 - position->y) * 0.5f;
            position->z += (object->player.locator_matrices[second].m32 - position->z) * 0.5f;
        }
    }
    if (SuperCarry_AlignObject != 0) {
        if (secondary_position != NULL) {
            NuVecRotateY(&offset, &object->player.launch_origin, object->movement_angle);
            NuVecAdd(secondary_position, position, &offset);
        }
        NuVecRotateY(&offset, &object->player.external_force, object->movement_angle);
        NuVecAdd(position, position, &offset);
    }
}

void SuperCarry_Throw(GAMEOBJECT *object, i32 mode) {
    PLAYERCHARACTERCONFIG *config = object->character_data->game_character;
    GIZMOBLOWUP *blowup;
    NUMTX matrix;
    NUVEC position;
    NUVEC offset;
    NUVEC velocity;
    ADDPART params;
    PART *part;
    i32 first = config->hand_locators[0];
    i32 second;
    f32 speed;
    if (first == -1 || object->character_model->points_of_interest[first] == NULL) {
        return;
    }
    if (SuperCarry_KeepObjectLevel) {
        if (SuperCarry_AlignObject) {
            NuMtxSetRotationY(&matrix, object->player.takeover_start_angle);
        } else {
            matrix = NuMtxSetIdentityTemplate;
            *(NUVEC *)&matrix.m00 = object->player.supercarry.basis[0];
            *(NUVEC *)&matrix.m10 = object->player.supercarry.basis[1];
            *(NUVEC *)&matrix.m20 = object->player.supercarry.basis[2];
        }
        NuMtxRotateY(&matrix, object->movement_angle);
        NuMtxTranslate(&matrix, &object->player.locator_matrices[first].translation);
    } else {
        matrix = object->player.locator_matrices[first];
    }
    second = object->character_data->game_character->hand_locators[1];
    if (second != -1 && object->character_model->points_of_interest[second] != NULL) {
        position = matrix.translation;
        if (!SuperCarry_KeepObjectLevel) {
            NUMTX left;
            NUMTX right;
            left = matrix;
            right = object->player.locator_matrices[second];
            left.m30 = left.m31 = left.m32 = 0.0f;
            right.m30 = right.m31 = right.m32 = 0.0f;
            QuatInterpolateRotationMatrix(&matrix, &left, &right, 0.5f);
        }
        matrix.m30 = (f32)((position.x + object->player.locator_matrices[second].m30) * 0.5);
        matrix.m31 = (f32)((position.y + object->player.locator_matrices[second].m31) * 0.5);
        matrix.m32 = (f32)((position.z + object->player.locator_matrices[second].m32) * 0.5);
    }
    if (SuperCarry_KeepObjectLevel) {
        if (SuperCarry_AlignObject) {
            NuVecRotateY(&offset, &object->player.supercarry.basis[0], object->movement_angle);
            NuMtxTranslate(&matrix, &offset);
        }
    } else {
        NUMTX basis_matrix;
        position = matrix.translation;
        *(NUVEC *)&basis_matrix.m00 = object->player.supercarry.basis[0];
        *(NUVEC *)&basis_matrix.m10 = object->player.supercarry.basis[1];
        *(NUVEC *)&basis_matrix.m20 = object->player.supercarry.basis[2];
        NuMtxMulR(&matrix, &basis_matrix, &matrix);
        matrix.translation = position;
    }
    params = Default_ADDPART;
    params.matrix = &matrix;
    speed = mode == 0 ? SUPERCARRY_THROWSPEED_XZ : mode == 1 ? SUPERCARRY_RELEASESPEED_XZ : SUPERCARRY_DROPSPEED_XZ;
    velocity.x = NuTrigTable[object->facing_angle >> 1] * speed;
    velocity.z = NuTrigTable[((object->facing_angle + 0x4000) >> 1) & 0x7fff] * speed;
    velocity.y = mode == 2 ? SUPERCARRY_DROPSPEED_Y : SUPERCARRY_THROWSPEED_Y;
    params.velocity = &velocity;
    blowup = (GIZMOBLOWUP *)object->player.grapple;
    params.special = &blowup->type->animated_special;
    params.start_radius = params.end_radius = (f32)(0.5 * blowup->target_scale);
    params.flags = PART_FLAG_DIE_WHEN_STOPPED | PART_FLAG_KEEP_OFF_SCREEN | PART_FLAG_THROWN;
    if (!(blowup->secondary_flags & 4)) {
        params.flags = PART_FLAG_DIE_WHEN_STOPPED | PART_FLAG_KEEP_OFF_SCREEN | PART_FLAG_NO_BOUNCE | PART_FLAG_THROWN;
    }
    params.owner = object;
    params.time_step = FRAMETIME;
    params.gravity = SUPERCARRY_OBJGRAVITY;
    params.impact_fn = SuperCarry_PartImpact;
    params.fade_time = 3.0f;
    params.kill_fn = SuperCarry_PartKill;
    params.lighting_data = &object->lighting_packet;
    part = AddPart(&params);
    if (part != NULL) {
        part->force_flags = ObjHitObj_Flags(object);
        part->supercarry_blowup = (GIZMOBLOWUP *)object->player.grapple;
        part->rotation_y = object->player.takeover_start_angle + object->movement_facing_angle;
    }
}

u32 (*CanSuperCarryFn)(GAMEOBJECT *) = NULL;

i32 SuperCarry_Possible(GAMEOBJECT *object, i32 require_grounded) {
    if (CanSuperCarryFn == NULL || CanSuperCarryFn(object) == 0) {
        return 0;
    }
    if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0) {
        return 0;
    }
    if (require_grounded != 0 && (object->ground_flags == 0 || ObjLandReady(object) == 0)) {
        return 0;
    }
    return 1;
}

void SuperCarry_Start(struct gameobject_s *object, GIZMOBLOWUP *blowup, i32 immediate) {
    object->player.character_context = ((i8)(LEGOCONTEXT_SUPERCARRY));
    object->player.context_variant_flags &= ~APIOBJECT_CONTEXT_EVENT_DONE;
    object->player.action_movement_state = 0;
    object->player.grapple = blowup;
    if (immediate == 0 && LEGOACT_SUPERCARRY_PICKUP != -1 && object->character_model->model_data_b[LEGOACT_SUPERCARRY_PICKUP] != NULL) {
        object->player.context_phase = SUPERCARRY_PICKUP;
        object->player.context_animation = LEGOACT_SUPERCARRY_PICKUP;
        object->player.context_timer = AnimDuration((i16)object->id, object->player.context_animation, 0.0f, 0.0f, 1);
        if (object->player.context_timer <= 0.0f) {
            object->player.context_timer = 0.5f;
        }
        object->movement_facing_angle =
            NuAtan2D(blowup->mid_position.x - object->collision_position.x, blowup->mid_position.z - object->collision_position.z);
    } else {
        object->player.context_phase = SUPERCARRY_IDLE;
        object->player.context_animation = LEGOACT_SUPERCARRY_IDLE;
        object->player.context_timer = 0.0f;
        if (immediate == 0) {
            GizmoBlowupBlowup(blowup, 0, BLOWUP_HIT_CARRIED, -1, NULL, 1);
        }
    }
    if (SuperCarry_AlignObject != 0) {
        u16 angle;
        i32 difference;
        i32 magnitude;
        NUVEC minimum;
        NUVEC maximum;
        NuSpecialGetBounds(&object->player.carried_blowup->type->animated_special, &minimum, &maximum);
        blowup = (GIZMOBLOWUP *)object->player.grapple;
        angle = NuAtan2D(blowup->transform.m20, blowup->transform.m22);
        difference = RotDiff(object->movement_facing_angle, angle);
        magnitude = abs(difference);
        if (magnitude < 0x2000) {
            object->player.takeover_start_angle = 0;
            object->player.supercarry.basis[0].x = 0.0f;
            object->player.supercarry.basis[0].y = -minimum.y;
            object->player.supercarry.basis[0].z = -minimum.z;
        } else if (magnitude > 0x6000) {
            object->player.takeover_start_angle = 0x8000;
            object->player.supercarry.basis[0].x = 0.0f;
            object->player.supercarry.basis[0].y = -minimum.y;
            object->player.supercarry.basis[0].z = maximum.z;
        } else if (difference < 0) {
            object->player.takeover_start_angle = 0xc000;
            object->player.supercarry.basis[0].x = 0.0f;
            object->player.supercarry.basis[0].y = -minimum.y;
            object->player.supercarry.basis[0].z = maximum.x;
        } else {
            object->player.takeover_start_angle = 0x4000;
            object->player.supercarry.basis[0].x = 0.0f;
            object->player.supercarry.basis[0].y = -minimum.y;
            object->player.supercarry.basis[0].z = -minimum.x;
        }
        object->player.supercarry.basis[1].x = 0.0f;
        object->player.supercarry.basis[1].y = object->player.supercarry.basis[0].y + (maximum.y - minimum.y) * 0.5f;
        object->player.supercarry.basis[1].z = object->player.supercarry.basis[0].z;
        object->movement_facing_angle = angle - object->player.takeover_start_angle;
    } else {
        NUMTX matrix = object->player.carried_blowup->transform;
        NuMtxRotateY(&matrix, 0x8000 - object->movement_facing_angle);
        object->player.supercarry.basis[0] = *NUMTX_GET_ROW_VEC(&matrix, 0);
        object->player.supercarry.basis[1] = *NUMTX_GET_ROW_VEC(&matrix, 1);
        object->player.supercarry.basis[2] = *NUMTX_GET_ROW_VEC(&matrix, 2);
    }
    object->player.context_hover_target_y = 0.5f;
}

i32 SuperCarry_UseActionButton = 0;
i32 SuperCarryEnable = 0;
i32 CombatRollEnable = 0;

void SuperCarry_MoveCode(WORLDINFO *world, struct gameobject_s *object) {
    if (LEGOCONTEXT_SUPERCARRY == -1) {
        return;
    }
    if (object->player.character_context == LEGOCONTEXT_SUPERCARRY) {
        f32 *frame = NULL;
        if (object->character_model->model_data_b[object->player.context_animation] != NULL) {
            frame = AnimPlaying(&object->anim_packet, object->player.context_animation, 1, 0);
            if (frame == NULL) {
                return;
            }
        }
        switch (object->player.context_phase) {
            case SUPERCARRY_PICKUP: {
                i32 event = 0;
                if ((object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0) {
                    GIZMOBLOWUP *blowup = (GIZMOBLOWUP *)object->player.grapple;
                    object->movement_facing_angle = NuAtan2D(blowup->mid_position.x - object->collision_position.x,
                                                             blowup->mid_position.z - object->collision_position.z);
                    if (frame != NULL) {
                        f32 marker = AnimListFrame(object->character_model, object->player.context_animation, 0);
                        if (marker >= 1.0f && *frame >= marker) {
                            event = 1;
                        }
                    }
                }
                object->player.context_timer -= FRAMETIME;
                if (object->player.context_timer <= 0.0f) {
                    object->player.context_phase = SUPERCARRY_IDLE;
                    object->player.context_animation = LEGOACT_SUPERCARRY_IDLE;
                    if ((object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0) {
                        event = 1;
                    }
                }
                if (event) {
                    object->player.context_variant_flags |= APIOBJECT_CONTEXT_EVENT_DONE;
                    GizmoBlowupBlowup((GIZMOBLOWUP *)object->player.grapple, 0, BLOWUP_HIT_CARRIED, -1, NULL, 1);
                    NewBuzzFrames(object->player.pad->bound_pad, 1, 0);
                }
                return;
            }
            case SUPERCARRY_PUTDOWN: {
                i32 event = 0;
                if ((object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0) {
                    if (frame != NULL) {
                        f32 marker = AnimListFrame(object->character_model, object->player.context_animation, 0);
                        if (marker >= 1.0f && *frame >= marker) {
                            event = 1;
                        }
                    } else if (SuperCarry_PutDownDrop != 0) {
                        event = 1;
                    }
                }
                object->player.context_timer -= FRAMETIME;
                if (object->player.context_timer <= 0.0f) {
                    object->player.character_context = CHARACTER_CONTEXT_NONE;
                    if ((object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0) {
                        event = 1;
                    }
                }
                if (event) {
                    struct GIZMOBLOWUP *blowup;
                    NUVEC *drop;
                    object->player.context_variant_flags |= APIOBJECT_CONTEXT_EVENT_DONE;
                    NewBuzzFrames(object->player.pad->bound_pad, 1, 0);
                    blowup = (GIZMOBLOWUP *)object->player.grapple;
                    drop = &object->player.supercarry.drop_position;
                    if (SuperCarry_PutDownDrop != 0) {
                        if (!SuperCarry_TurnBlowupBackOn(blowup, drop, object->player.takeover_start_angle + object->movement_facing_angle,
                                                         1)) {
                            SuperCarry_Throw(object, 2);
                        }
                    } else {
                        SuperCarry_TurnBlowupBackOn(blowup, drop, object->player.takeover_start_angle + object->movement_facing_angle, 0);
                    }
                }
                return;
            }
            case SUPERCARRY_IDLE:
            case SUPERCARRY_WALK: {
                i32 throw_object = 0;
                i32 put_down = 0;
                object->player.context_timer = object->player.context_timer + FRAMETIME;
                if (object->ground_flags != 0) {
                    if (SuperCarry_UseActionButton != 0) {
                        if ((object->player.pad->buttons_pressed & GAMEPAD_ACTION) != 0) {
                            throw_object = 1;
                        } else if ((object->player.pad->buttons_pressed & GAMEPAD_SPECIAL) != 0) {
                            put_down = 1;
                        }
                    } else if ((object->player.pad->buttons_pressed & GAMEPAD_SPECIAL) != 0) {
                        if (object->player.pad->input_magnitude > 0.0f) {
                            throw_object = 1;
                        } else {
                            put_down = 1;
                        }
                    }
                    if (throw_object || put_down) {
                        struct GIZMOBLOWUP *blowup;
                        if ((throw_object || SuperCarry_UseActionButton == 0) && SuperCarryEnable != 0 && LEGOACT_SUPERCARRY_BASH != -1 &&
                            object->character_model->model_data_b[LEGOACT_SUPERCARRY_BASH] != NULL) {
                            objopponent_ignoreaiopponent = 1;
                            object->player.force_target = ObjOpponent(object, 1.0f, PUNCHGAP, 1, 0, 1);
                            if (object->player.force_target == NULL) {
                                objopponent_ignoreaiopponent = 1;
                                object->player.force_target = ObjOpponent(object, 1.0f, PUNCHGAP, 1, 0, 2);
                            }
                            if (object->player.force_target != NULL) {
                                object->player.context_phase = SUPERCARRY_BASH;
                                object->player.context_animation = LEGOACT_SUPERCARRY_BASH;
                                object->player.context_variant_flags &= ~APIOBJECT_CONTEXT_EVENT_DONE;
                                object->player.context_timer =
                                    AnimDuration((i16)object->id, object->player.context_animation, 0.0f, 0.0f, 1);
                                if (object->player.context_timer <= 0.0f) {
                                    object->player.context_timer = 0.5f;
                                }
                                return;
                            }
                        }
                        if (throw_object) {
                            if (LEGOACT_SUPERCARRY_THROW != -1 && object->character_model->model_data_b[LEGOACT_SUPERCARRY_THROW] != NULL) {
                                object->player.context_phase = SUPERCARRY_THROW;
                                object->player.context_animation = LEGOACT_SUPERCARRY_THROW;
                                object->player.context_variant_flags &= ~APIOBJECT_CONTEXT_EVENT_DONE;
                                object->player.context_timer =
                                    AnimDuration((i16)object->id, object->player.context_animation, 0.0f, 0.0f, 1);
                                if (object->player.context_timer <= 0.0f) {
                                    object->player.context_timer = 0.5f;
                                }
                            } else {
                                object->player.character_context = CHARACTER_CONTEXT_NONE;
                                object->player.flags_e20 |= 0x800000;
                                NewRumble(object->player.pad->bound_pad, 0.6f, 0);
                            }
                            return;
                        }
                        blowup = (GIZMOBLOWUP *)object->player.grapple;
                        if (put_down && (blowup->secondary_flags & 4) != 0 && LEGOACT_SUPERCARRY_PUTDOWN != -1 &&
                            object->character_model->model_data_b[LEGOACT_SUPERCARRY_PUTDOWN] != NULL &&
                            object->floor_height != 2000000.0f && object->lower_position.y - object->floor_height < 0.1f) {
                            NUVEC offset;
                            object->player.context_phase = SUPERCARRY_PUTDOWN;
                            object->player.context_animation = LEGOACT_SUPERCARRY_PUTDOWN;
                            object->player.context_variant_flags &= ~APIOBJECT_CONTEXT_EVENT_DONE;
                            object->player.context_timer = AnimDuration((i16)object->id, object->player.context_animation, 0.0f, 0.0f, 1);
                            if (object->player.context_timer <= 0.0f) {
                                object->player.context_timer = 0.5f;
                            }
                            object->player.supercarry.drop_position.x = object->lower_position.x;
                            object->player.supercarry.drop_position.y = object->floor_height;
                            object->player.supercarry.drop_position.z = object->lower_position.z;
                            NuVecRotateY(&offset, &object->player.supercarry.basis[0], object->movement_angle);
                            object->player.supercarry.drop_position.x = object->player.supercarry.drop_position.x + offset.x;
                            object->player.supercarry.drop_position.z = object->player.supercarry.drop_position.z + offset.z;
                            return;
                        }
                    }
                }
                if (CombatRollEnable != 0 && LEGOACT_SUPERCARRY_JUMP != -1 && (object->player.pad->buttons_pressed & GAMEPAD_JUMP) != 0 &&
                    object->ground_flags != 0) {
                    object->player.context_phase = SUPERCARRY_JUMP;
                    object->player.context_animation = LEGOACT_SUPERCARRY_JUMP;
                    object->player.context_timer = 0.0f;
                    object->velocity.y = SUPERCARRY_JUMPSPEED;
                    if (object->character_model->model_data_b[object->player.context_animation] != NULL) {
                        object->player.context_duration = AnimDuration((i16)object->id, object->player.context_animation, 0.0f, 0.0f, 0);
                    } else {
                        object->player.context_duration = (-object->velocity.y / object->character_data->game_character->gravity) * 2.0f;
                    }
                    object->player.context_variant_flags &= ~APIOBJECT_JUMP_FALLING;
                    object->player.context_duration += 0.1f;
                    object->ground_flags = 0;
                    object->terrain_contact_mask = 0;
                    object->player.jump_start_height = object->collision_min.y;
                    ResetAnimPacket(&object->anim_packet, -1);
                }
                if (object->player.pad->input_magnitude > 0.0f) {
                    object->player.context_phase = SUPERCARRY_WALK;
                    object->player.context_animation = LEGOACT_SUPERCARRY_WALK;
                } else {
                    object->player.context_phase = SUPERCARRY_IDLE;
                    object->player.context_animation = LEGOACT_SUPERCARRY_IDLE;
                }
                return;
            }
            case SUPERCARRY_THROW: {
                i32 event;
                if (object->player.context_timer <= 0.0f && (object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) != 0) {
                    object->player.character_context = CHARACTER_CONTEXT_NONE;
                    return;
                }
                event = 0;
                if (frame != NULL && (object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0) {
                    f32 marker = AnimListFrame(object->character_model, object->player.context_animation, 0);
                    if (marker >= 1.0f && *frame >= marker) {
                        event = 1;
                    }
                }
                object->player.context_timer -= FRAMETIME;
                if (object->player.context_timer <= 0.0f) {
                    if ((object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0) {
                        event = 1;
                    } else {
                        object->player.character_context = CHARACTER_CONTEXT_NONE;
                    }
                }
                if (!event) {
                    return;
                }
                object->player.context_variant_flags |= APIOBJECT_CONTEXT_EVENT_DONE;
                object->player.flags_e20 |= 0x800000;
                NewRumble(object->player.pad->bound_pad, 0.6f, 0);
                return;
            }
            case SUPERCARRY_BASH: {
                i32 event;
                if (object->player.context_timer <= 0.0f && (object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) != 0) {
                    object->player.character_context = CHARACTER_CONTEXT_NONE;
                    return;
                }
                event = 0;
                if (frame != NULL && (object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0) {
                    f32 marker = AnimListFrame(object->character_model, object->player.context_animation, 0);
                    if (marker >= 1.0f && *frame >= marker) {
                        event = 1;
                    }
                }
                object->player.context_timer -= FRAMETIME;
                if (object->player.context_timer <= 0.0f) {
                    if ((object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0) {
                        event = 1;
                    } else {
                        object->player.character_context = CHARACTER_CONTEXT_NONE;
                    }
                }
                if (!event) {
                    return;
                }
                object->player.context_variant_flags |= APIOBJECT_CONTEXT_EVENT_DONE;
                if (ObjOpponentStillThere(object, object->player.force_target, PUNCHGAP)) {
                    ObjHitObj(object, object->player.force_target, 1, 0x2000, 0, 1);
                } else if (object->player.character_context != CHARACTER_CONTEXT_NONE && object->player.context_timer > 0.0f) {
                    object->player.context_variant_flags &= ~APIOBJECT_CONTEXT_EVENT_DONE;
                }
                if ((object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) != 0) {
                    struct GIZMOBLOWUP *blowup;
                    NUVEC position;
                    i32 effect;
                    SuperCarry_GetObjectPos(object, &position, NULL);
                    blowup = (GIZMOBLOWUP *)object->player.grapple;
                    effect = BLOWUP_EFFECT_DEBRIS | BLOWUP_EFFECT_PARTICLES;
                    if (blowup->effect_delay > 0.0f) {
                        effect = BLOWUP_EFFECT_DEBRIS | BLOWUP_EFFECT_PARTICLES | BLOWUP_EFFECT_EXPLOSION;
                    }
                    GizmoBlowUp_AddEffects(&position, blowup, 0, effect, NULL);
                    NewRumble(object->player.pad->bound_pad, 0.6f, 0);
                    NewBuzzFrames(object->player.pad->bound_pad, 2, 0);
                }
                return;
            }
            case SUPERCARRY_JUMP: {
                object->player.context_timer += FRAMETIME;
                if (object->player.context_timer >= object->player.context_duration) {
                    object->player.context_variant_flags |= APIOBJECT_JUMP_FALLING;
                }
                if (object->player.context_timer >= 0.1f && object->ground_flags != 0) {
                    i32 fell = (object->player.context_variant_flags >> 15) & 1;
                    if (fell || object->player.pad->input_magnitude == 0.0f) {
                        object->player.context_phase = SUPERCARRY_LAND;
                        if (fell && LEGOACT_SUPERCARRY_FALLLAND != -1 &&
                            object->character_model->model_data_b[LEGOACT_SUPERCARRY_FALLLAND] != NULL) {
                            object->player.context_animation = LEGOACT_SUPERCARRY_FALLLAND;
                        } else {
                            object->player.context_animation = LEGOACT_SUPERCARRY_LAND;
                        }
                        if (object->player.context_animation != ACTION_NONE) {
                            object->player.context_timer = AnimDuration((i16)object->id, object->player.context_animation, 0.0f, 0.0f, 1);
                            if (object->player.context_timer <= 0.0f) {
                                object->player.context_timer = 0.5f;
                            }
                            ResetAnimPacket(&object->anim_packet, -1);
                            object->velocity.x = object->velocity.z = 0.0f;
                            NewBuzzFrames(object->player.pad->bound_pad,
                                          object->player.context_animation == LEGOACT_SUPERCARRY_FALLLAND ? 2 : 1, 0);
                            return;
                        }
                    }
                    if (object->player.pad->input_magnitude > 0.0f) {
                        object->player.context_phase = SUPERCARRY_WALK;
                        object->player.context_animation = LEGOACT_SUPERCARRY_WALK;
                    } else {
                        object->player.context_phase = SUPERCARRY_IDLE;
                        object->player.context_animation = LEGOACT_SUPERCARRY_IDLE;
                    }
                }
                return;
            }
            case SUPERCARRY_LAND:
                object->player.context_timer -= FRAMETIME;
                if (object->player.context_timer <= 0.0f ||
                    ((object->player.context_variant_flags & APIOBJECT_JUMP_FALLING) == 0 && object->player.pad->input_magnitude > 0.0f)) {
                    if (object->player.pad->input_magnitude > 0.0f) {
                        object->player.context_phase = SUPERCARRY_WALK;
                        object->player.context_animation = LEGOACT_SUPERCARRY_WALK;
                    } else {
                        object->player.context_phase = SUPERCARRY_IDLE;
                        object->player.context_animation = LEGOACT_SUPERCARRY_IDLE;
                    }
                }
                return;
            default:
                return;
        }
    } else {
        NUVEC forward;
        struct GIZMOBLOWUP *nearest;
        f32 best;
        struct GIZMOBLOWUP *blowup;
        i32 i;
        i8 player;
        if (CanSuperCarryFn == NULL || CanSuperCarryFn(object) == 0) {
            return;
        }
        if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0) {
            return;
        }
        if (object->ground_flags == 0 || ObjLandReady(object) == 0) {
            return;
        }
        best = 100000.0f;
        NuVecRotateY(&forward, &v001, object->movement_facing_angle);
        blowup = world->gizmo_blowups;
        nearest = NULL;
        if (blowup == NULL) {
            return;
        }
        for (i = 0; i < world->gizmo_blowup_count; ++i, ++blowup) {
            if ((blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) == 0 && (blowup->status_flags & BLOWUP_STATUS_ACTIVE) != 0 &&
                (blowup->status_flags & BLOWUP_STATUS_VISIBLE) != 0 && (blowup->status_flags & BLOWUP_STATUS_DRAWN) != 0 &&
                (blowup->secondary_flags & 1) != 0) {
                NUVEC delta;
                f32 distance;
                f32 radius;
                if ((blowup->draw_flags & BLOWUP_OPTION_SHADOW_MODE_ONLY) != 0 && ShadowMode == 0) {
                    continue;
                }
                if (blowup->platform_id != -1 && object->terrain_platform_id == blowup->platform_id) {
                    continue;
                }
                distance = NuVecDistSqr(&blowup->mid_position, &object->collision_position, &delta);
                if (!(distance < best)) {
                    continue;
                }
                radius = object->bounds_radius + blowup->target_scale + 0.2f;
                if (!(distance < radius * radius)) {
                    continue;
                }
                if (!(delta.x * forward.x + delta.z * forward.z > 0.0f)) {
                    continue;
                }
                nearest = blowup;
                best = distance;
            }
        }
        if (nearest == NULL) {
            return;
        }
        player = object->player_index;
        if (player == 0 || player == 1) {
            ADD_GAME_MESSAGE message = AddGameMsg_Default;
            message.text = LEGOASCII_DOWN != NULL ? LEGOASCII_DOWN : txt_UNKNOWN;
            message.position = &nearest->mid_position;
            message.flags = GAMEMSG_WORLD_POSITION | GAMEMSG_FOLLOW | GAMEMSG_WORLD_SCALE | GAMEMSG_ONE_FRAME;
            message.slot_flags = 4;
            message.scale = 2.0f;
            if (Player[0] != NULL && (Player[0]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) && Player[1] != NULL &&
                (Player[1]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE)) {
                message.slot_flags = (player == 0 ? 8 : 2) | 4;
            }
            message.red = SuperCarryPlayerRgb[player][0];
            message.green = SuperCarryPlayerRgb[player][1];
            message.blue = SuperCarryPlayerRgb[player][2];
            message.alpha =
                ((u8)(80.0 + 48.0 * (f64)NuTrigTable[((u16)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0)) >> 1]));
            AddGameMsg(&message);
        }
        if ((object->player.pad->buttons_held & GAMEPAD_SPECIAL) != 0) {
            SuperCarry_Start(object, nearest, 0);
        }
    }
}

i32 SuperCarry_SetTargetMom(GAMEOBJECT *object, f32 input_speed) {
    f32 speed = 0.0f;
    object->target_velocity.x = 0.0f;
    object->target_velocity.z = 0.0f;
    if (input_speed > 0.0f) {
        if ((object->player.context_phase == SUPERCARRY_WALK || object->player.context_phase == SUPERCARRY_PUTDOWN) &&
            object->player.context_animation != ACTION_NONE &&
            object->character_model->model_data_b[object->player.context_animation] != NULL) {
            speed = AnimSpeed(object->character_model, object->player.context_animation);
        } else if (object->player.context_phase == SUPERCARRY_WALK || object->player.context_phase == SUPERCARRY_JUMP) {
            if (object->player.context_phase == SUPERCARRY_JUMP && LEGOACT_SUPERCARRY_WALK != 0 &&
                object->character_model->model_data_b[LEGOACT_SUPERCARRY_WALK] != NULL) {
                speed = AnimSpeed(object->character_model, LEGOACT_SUPERCARRY_WALK);
            } else {
                speed = NuQFntHalfScale * object->character_data->game_character->run_speed;
            }
        }
        if (speed > 0.0f) {
            object->target_velocity.x = NuTrigTable[object->facing_angle >> 1] * speed;
            object->target_velocity.z = NuTrigTable[((object->facing_angle + 0x4000) >> 1) & 0x7fff] * speed;
        }
    }
    return 0;
}

i32 SuperCarry_YRotation(GAMEOBJECT *object, u16 input_angle) {
    if (object->player.context_phase == SUPERCARRY_PICKUP) {
        object->facing_angle = SeekRot(object->facing_angle, object->movement_facing_angle, 12.0f);
        object->movement_angle = object->facing_angle;
    } else if (object->player.context_phase == SUPERCARRY_BASH) {
        if (object->player.force_target != NULL) {
            object->movement_facing_angle = NuAtan2D(object->player.force_target->collision_position.x - object->collision_position.x,
                                                     object->player.force_target->collision_position.z - object->collision_position.z);
        }
        object->facing_angle = SeekRot(object->facing_angle, object->movement_facing_angle, 8.0f);
        object->movement_angle = object->facing_angle;
    } else if (object->player.context_phase == SUPERCARRY_IDLE || object->player.context_phase == SUPERCARRY_WALK ||
               object->player.context_phase == SUPERCARRY_JUMP) {
        if (object->player.pad->input_magnitude > 0.0f) {
            object->facing_angle = TurnRot(object->facing_angle, input_angle,
                                           (i32)(VehicleTurnPhaseOffset * object->player.context_hover_target_y * 8.0), NULL);
        }
        object->movement_angle = SeekRot(object->movement_angle, object->facing_angle, 10.0f);
        object->movement_facing_angle = object->facing_angle;
    }
    return 1;
}

i32 SuperCarry_Carrying(GAMEOBJECT *object) {
    if (LEGOCONTEXT_SUPERCARRY != -1 && object->player.character_context == LEGOCONTEXT_SUPERCARRY) {
        if (object->player.context_phase == SUPERCARRY_IDLE || object->player.context_phase == SUPERCARRY_WALK ||
            object->player.context_phase == SUPERCARRY_JUMP) {
            return 1;
        }
    }
    return 0;
}

void SuperCarry_Release(GAMEOBJECT *object) {
    if (LEGOCONTEXT_SUPERCARRY != -1 && object->player.character_context == LEGOCONTEXT_SUPERCARRY &&
        (object->player.context_phase == SUPERCARRY_IDLE || object->player.context_phase == SUPERCARRY_WALK ||
         object->player.context_phase == SUPERCARRY_JUMP)) {
        SuperCarry_Throw(object, 1);
        object->player.character_context = CHARACTER_CONTEXT_NONE;
    }
}

i32 SuperCarry_PutDownDrop = 1;
f32 SUPERCARRY_JUMPSPEED = 1.4f;
f32 SuperCarryImpactSpeed = 4.0f;
u8 SuperCarryPlayerRgb[2][3] = {{0, 127, 255}, {0, 255, 0}};
f32 PUNCHGAP = 0.3f;
f32 SUPERCARRY_THROWSPEED_XZ = 3.0f;
f32 SUPERCARRY_THROWSPEED_Y = 3.0f;
f32 SUPERCARRY_RELEASESPEED_XZ = 2.0f;
f32 SUPERCARRY_DROPSPEED_XZ = 0.2f;
f32 SUPERCARRY_DROPSPEED_Y = 1.0f;
f32 SUPERCARRY_OBJGRAVITY = -6.0f;
i32 SuperCarry_KeepObjectLevel = 1;
i32 SuperCarry_AlignObject = 1;
