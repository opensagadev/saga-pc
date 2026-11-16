#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/ai/aisys/gameaiscript.h"
#include "gamelib/util/Utilities.h"
#include "globals.h"
#include "legoapi/actions/movement/climbing.h"
#include "legoapi/actions/movement/jumping.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/animation_ids.h"
#include "legoapi/characters/motion/contexts.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/light/surfaces.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "legogame/game.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>

extern i32 LEGO_AIPATHCNX_VARIANT_FLAG0;
extern i32 LEGO_AIPATHCNX_VARIANT_FLAG1;

static __inline f32 ClimbingFabs(f32 value) {
    u32 bits = *(u32 *)&value & 0x7fffffff;
    return *(f32 *)&bits;
}

static i32 LedgeTerrain_Attach(GAMEOBJECT *object, u16 facing, NUVEC *position, u16 *wall_angle) {
    i8 surface;
    i32 difference;
    u16 inward_angle;
    f32 distance;
    f32 height;
    f32 height_difference;
    if (object->contact_count == 0) {
        return 0;
    }
    surface = (i8)object->contact_material[0];
    if (surface >= 0 ? surface < 32 && (TerSurface[surface].flags &
                                        (TERSURFACE_DEADLY | TERSURFACE_HURTS | TERSURFACE_CONVEYOR | TERSURFACE_SLIDE)) != 0
                     : 0) { /* the original surface-flag macro shape */
        return 0;
    }
    if (object->contact_position.y <= object->collision_position.y - (f64)0.05f ||
        ClimbingFabs(object->contact_normal.y) > NuTrigTable[0x3000]) {
        return 0;
    }
    *wall_angle = NuAtan2D(object->contact_normal.x, object->contact_normal.z);
    difference = abs(RotDiff(facing, *wall_angle));
    if (difference < 0x4000) {
        return 0;
    }
    inward_angle = *wall_angle + 0x8000;
    distance = object->bounds_radius + LEDGETERRAINLOOKAHEAD;
    position->x = NuTrigTable[inward_angle >> 1] * distance + object->position.x;
    position->y = object->upper_position.y;
    position->z = NuTrigTable[((inward_angle + 0x4000) >> 1) & 0x7fff] * distance + object->position.z;
    height = GameShadow(NULL, position, 5.0f, -1);
    if (height == 2000000.0f || ShadNorm.y < NuTrigTable[0x2555]) {
        return 0;
    }
    height_difference = position->y - height;
    if (height_difference > (f64)0.025f || height_difference < (f64)-0.1f) {
        return 0;
    }
    if (object->roof_height != 2000000.0f && height > object->roof_height) {
        return 0;
    }
    position->y = height;
    return 1;
}

void LedgeTerrain_MoveCode(GAMEOBJECT *object) {
    NUVEC position;
    u16 wall_angle;
    i16 animation;
    if (LEGOCONTEXT_LEDGETERRAIN == -1) {
        return;
    }
    if (object->player.character_context == LEGOCONTEXT_LEDGETERRAIN) {
        f32 travelled;
        u16 input;
        u16 sideways;
        f32 push;
        f32 speed;
        NUVEC offset;
        f32 anim_speed;
        f32 magnitude;
        if ((object->player.pad->buttons_pressed & GAMEPAD_JUMP) != 0) {
            f32 height;
            StartJump(object, 0);
            object->player.flags_e24 |= APIOBJECT_NO_JUMP;
            height = object->player.external_force.y + (f64)0.1f - object->player.jump_start_height;
            if (height > 0.0f) {
                object->velocity.y = NuFsqrt(object->character_data->game_character->gravity * -2.0 * height);
            }
            return;
        }
        if ((object->ground_flags & GROUND_NEAR) != 0) {
            object->player.character_context = CHARACTER_CONTEXT_NONE;
            return;
        }
        if (object->player.context_phase != 0 || object->player.context_animation != LEGOACT_LEDGE_GRAB ||
            AnimPlaying(&object->anim_packet, object->player.context_animation, 1, 0) != NULL) {
            object->player.context_timer = FRAMETIME + object->player.context_timer;
        }
        if (object->player.context_phase == 0 && object->player.context_timer >= object->player.context_hover_target_y) {
            object->player.context_phase = 1;
        }
        if (LedgeTerrain_Attach(object, object->movement_facing_angle, &position, &wall_angle) != 0) {
            f32 x = object->player.external_force.x;
            f32 z = object->player.external_force.z;
            object->movement_facing_angle = wall_angle + 0x8000;
            object->player.launch_origin = position;
            object->player.external_force.x = object->position.x;
            object->player.external_force.y = position.y;
            object->player.external_force.z = object->position.z;
            object->player.context_duration = 0.25f;
            x = object->player.external_force.x - x;
            z = object->player.external_force.z - z;
            travelled = NuFsqrt(x * x + z * z);
        } else {
            object->player.context_duration -= FRAMETIME;
            if (object->player.context_duration <= 0.0f) {
                object->player.character_context = CHARACTER_CONTEXT_NONE;
                return;
            }
            travelled = 0.0f;
        }
        if (object->player.context_phase == 0) {
            return;
        }
        if (object->player.pad->input_magnitude > 0.0f) {
            input = GamePad_InputAngle(object, object->player.pad);
            sideways = object->movement_facing_angle + 0x4000;
            push = PushingTowardsAngle(input, sideways);
            if (push > NuTrigTable[0x3555]) {
                if (LEGOACT_LEDGE_RIGHT != -1 && object->character_model->model_data_b[LEGOACT_LEDGE_RIGHT] != NULL) {
                    object->player.context_animation = LEGOACT_LEDGE_RIGHT;
                    anim_speed = AnimSpeed(object->character_model, LEGOACT_LEDGE_RIGHT);
                    *(u32 *)&speed = *(u32 *)&anim_speed & 0x7fffffff;
                } else {
                    if (LedgeTerrain_CheckAnims != 0) {
                        return;
                    }
                    speed = 0.5f;
                }
            } else if (push < -NuTrigTable[0x3555]) {
                if (LEGOACT_LEDGE_LEFT != -1 && object->character_model->model_data_b[LEGOACT_LEDGE_LEFT] != NULL) {
                    object->player.context_animation = LEGOACT_LEDGE_LEFT;
                    anim_speed = AnimSpeed(object->character_model, LEGOACT_LEDGE_LEFT);
                    *(u32 *)&magnitude = *(u32 *)&anim_speed & 0x7fffffff;
                    speed = -magnitude;
                } else {
                    if (LedgeTerrain_CheckAnims != 0) {
                        return;
                    }
                    speed = -0.5f;
                }
            } else {
                object->player.context_animation = LEGOACT_LEDGE_IDLE;
                return;
            }
            if (speed == 0.0f) {
                return;
            }
            if (LedgeTerrain_CheckAnims != 0 && AnimPlaying(&object->anim_packet, object->player.context_animation, 1, 0) == NULL) {
                return;
            }
            if (speed < 0.0f) {
                travelled = -travelled;
            }
            offset.x = NuTrigTable[sideways >> 1] * (FRAMETIME * speed + travelled);
            offset.y = 0.0f;
            offset.z = NuTrigTable[((sideways + 0x4000) >> 1) & 0x7fff] * (FRAMETIME * speed + travelled);
            NuVecAdd(&object->player.external_force, &object->player.external_force, &offset);
            NuVecAdd(&object->player.launch_origin, &object->player.launch_origin, &offset);
        } else {
            object->player.context_animation = LEGOACT_LEDGE_IDLE;
        }
        return;
    }
    if (object->ground_flags != 0 || !(object->velocity.y <= 0.0f)) {
        return;
    }
    if (LedgeTerrain_CheckAnims != 0 && (LEGOACT_LEDGE_IDLE == -1 || object->character_model->model_data_b[LEGOACT_LEDGE_IDLE] == NULL)) {
        return;
    }
    if (object->player.character_context != CHARACTER_CONTEXT_NONE &&
        !(LEGOCONTEXT_CLIMB != -1 && object->player.character_context == LEGOCONTEXT_CLIMB) &&
        !(LEGOCONTEXT_JUMP != -1 && object->player.character_context == LEGOCONTEXT_JUMP && object->player.context_timer >= (f64)0.1f)) {
        return;
    }
    if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0 && (object->flags_ef4 & 0x8000) == 0) {
        return;
    }
    if (!(object->player.pad->input_magnitude > 0.0f)) {
        return;
    }
    if (LedgeTerrain_Attach(object, GamePad_InputAngle(object, object->player.pad), &position, &wall_angle) == 0) {
        return;
    }
    object->player.character_context = (i8)LEGOCONTEXT_LEDGETERRAIN;
    object->velocity = v000;
    object->player.context_timer = 0.0f;
    object->player.context_phase = 0;
    animation = LEGOACT_LEDGE_GRAB;
    if (animation != -1 && object->character_model->model_data_b[animation] != NULL) {
        object->player.context_animation = animation;
        object->player.context_hover_target_y = AnimDuration((i16)object->id, animation, 0.0f, 0.0f, 1);
    } else {
        animation = LEGOACT_LEDGE_IDLE;
        object->player.context_hover_target_y = 0.1f;
        object->player.context_animation = animation;
        ResetAnimPacket(&object->anim_packet, animation);
    }
    object->player.external_force.x = object->position.x;
    object->player.external_force.y = position.y;
    object->player.external_force.z = object->position.z;
    object->movement_facing_angle = wall_angle + 0x8000;
    object->player.launch_origin = position;
    object->player.context_duration = 0.25f;
}

i32 LedgeTerrain_SetTargetMom(GAMEOBJECT *object) {
    i32 angle = object->movement_facing_angle;
    object->target_velocity.x =
        (NuTrigTable[(u32)angle >> 1] * LEDGETERRAINLOOKAHEAD + object->player.external_force.x - object->upper_position.x) * 10.0f;
    object->target_velocity.z = (NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * LEDGETERRAINLOOKAHEAD + object->player.external_force.z -
                                 object->upper_position.z) *
                                10.0f;
    object->target_velocity.y = (object->player.external_force.y - object->upper_position.y) * 10.0f;
    return 1;
}

i32 (*CanMagnetClimbFn)(GAMEOBJECT *) = 0;

i32 CanClimbSurface(GAMEOBJECT *object, i32 surface) {
    if ((u32)surface > 31 || (TerSurface[surface].flags & TERSURFACE_CLIMBABLE) == 0) {
        return 0;
    }
    if (surface == 5 && (CanMagnetClimbFn == NULL || CanMagnetClimbFn(object) == 0)) {
        return 0;
    }
    return 1;
}

void Climb_SetMagnetAction(GAMEOBJECT *object) {
    object->player.context_animation = LEGOACT_IDLE;
    if (object->player.pad->input_magnitude > 0.0f) {
        if (LEGOACT_MAGNET_WALK_METAL != -1) {
            object->player.context_animation = LEGOACT_MAGNET_WALK_METAL;
        } else if (LEGOACT_WALK != -1) {
            object->player.context_animation = LEGOACT_WALK;
        }
    }
}

void Climb_UpdateMagnetRotation(GAMEOBJECT *object) {
    object->movement_mode = 0;
    object->pitch_angle = SeekRot(object->pitch_angle, (u16)(-0x4000 - object->player.saved_pitch), 6.0f);
    object->roll_angle = SeekRot(object->roll_angle, 0, 10.0f);
}

f32 MAGNETOFFSET = 0;

void Climb_SetMagnetDrawOffsetTarget(GAMEOBJECT *object, NUVEC *offset) {
    NuVecScale(offset, &object->player.external_force, MAGNETOFFSET);
    NuVecAdd(offset, offset, &object->player.launch_origin);
    NuVecSub(offset, offset, &object->position);
}

void Climb_MoveCode(GAMEOBJECT *object) {
    if (LEGOCONTEXT_CLIMB == -1) {
        return;
    }
    if (object->player.character_context == LEGOCONTEXT_CLIMB) {
        u16 input_angle;
        if ((object->player.pad->buttons_pressed & GAMEPAD_JUMP) != 0 && object->player.context_variant != 5) {
            StartJump(object, 0);
            object->player.flags_e24 |= APIOBJECT_NO_JUMP;
            object->velocity.x = -NuTrigTable[object->movement_facing_angle >> 1] * object->character_data->game_character->run_speed;
            object->velocity.z =
                -NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] * object->character_data->game_character->run_speed;
            return;
        }
        input_angle = GamePad_InputAngle(object, object->player.pad);
        if ((object->ground_flags & GROUND_NEAR) != 0 && object->jump_animation_state == object->player.context_variant) {
            object->player.character_context = CHARACTER_CONTEXT_NONE;
        } else if ((object->ground_flags & GROUND_NEAR) != 0 && object->jump_animation_state != object->player.context_variant &&
                   object->player.pad->input_magnitude > 0.0f && PushingTowardsAngle(input_angle, object->movement_facing_angle) < 0.0f) {
            object->player.character_context = CHARACTER_CONTEXT_NONE;
        } else if (object->contact_count != 0 && object->contact_material[0] == object->player.context_variant) {
            object->player.context_duration = 0.25f;
            object->movement_facing_angle =
                NuAtan2D(object->contact_position.x - object->position.x, object->contact_position.z - object->position.z);
            object->player.external_force = object->contact_normal;
            object->player.launch_origin = object->contact_position;
            FindAnglesXY(&object->player.external_force, &object->player.saved_pitch, NULL);
        } else {
            object->player.context_duration -= FRAMETIME;
            if (object->player.context_duration <= 0.0f) {
                object->player.character_context = CHARACTER_CONTEXT_NONE;
            }
        }
        if (object->player.character_context != LEGOCONTEXT_CLIMB) {
            return;
        }
        if (object->player.context_variant == 5) {
            Climb_SetMagnetAction(object);
            Climb_SetMagnetDrawOffsetTarget(object, &object->player.context_position_offset);
            object->player.flags_e24 |= APIOBJECT_MAGNET_CLIMB;
            if (object->player.pad->input_magnitude > 0.0f) {
                u16 lean = RotDiff(object->movement_facing_angle, input_angle);
                object->player.tertiary_lean_angle = SeekRot(object->player.tertiary_lean_angle, lean, 9.0f);
            }
        } else {
            if (object->player.pad->input_magnitude > 0.0f) {
                if (abs(RotDiff(object->movement_facing_angle, GamePad_InputAngle(object, object->player.pad))) <= 0x4000) {
                    object->player.context_animation = LEGOACT_CLIMB_UP;
                } else {
                    object->player.context_animation = LEGOACT_CLIMB_DOWN;
                }
                if (object->character_model->model_data_b[object->player.context_animation] == NULL) {
                    object->player.context_animation = LEGOACT_CLIMB_IDLE;
                }
            } else {
                object->player.context_animation = LEGOACT_CLIMB_IDLE;
            }
        }
    } else {
        union {
            f32 value;
            u32 bits;
        } normal_y, normal_y_abs;

        NUVEC normal;
        i32 surface;
        NUVEC forward;
        f32 x;
        f32 z;
        if (object->player.character_context == CHARACTER_CONTEXT_NONE) {
            goto enter_climb;
        }
        if (ZipUpWallShuffleContext != -1 && object->player.character_context == ZipUpWallShuffleContext) {
            goto enter_climb;
        }
        if (LEGOCONTEXT_JUMP != -1 && object->player.character_context == LEGOCONTEXT_JUMP) {
            if (object->player.context_timer >= 1.0f || (object->player.context_timer >= 0.1f && object->velocity.y <= 0.0f)) {
                goto enter_climb;
            }
        }
        if (LEGOCONTEXT_GLIDE != -1 && object->player.character_context == LEGOCONTEXT_GLIDE && object->player.grapple == NULL) {
            goto enter_climb;
        }
        return;
    enter_climb:
        if (!(object->player.pad->input_magnitude > 0.0f) || object->contact_count == 0) {
            return;
        }
        normal_y.value = object->contact_normal.y;
        normal_y_abs.bits = normal_y.bits & 0x7fffffff;
        if (!(normal_y_abs.value < NuTrigTable[0x3000]) || CanClimbSurface(object, (i8)object->contact_material[0]) == 0) {
            return;
        }
        normal = object->contact_normal;
        surface = (i8)object->contact_material[0];
        NuVecRotateY(&forward, &v001, object->movement_facing_angle);
        x = object->contact_position.x - object->position.x;
        z = object->contact_position.z - object->position.z;
        if (!(forward.x * x + forward.z * z > 0.0f)) {
            return;
        }
        object->movement_facing_angle = NuAtan2D(x, z);
        object->velocity.y = 0.0f;
        object->player.character_context = (u8)LEGOCONTEXT_CLIMB;
        object->player.context_variant = (u8)surface;
        object->player.context_duration = 0.25f;
        if (object->player.context_variant == 5) {
            Climb_SetMagnetAction(object);
            Climb_SetMagnetDrawOffsetTarget(object, &object->player.context_position_offset);
        } else {
            object->player.context_animation = LEGOACT_CLIMB_IDLE;
        }
        object->player.external_force = normal;
        object->player.launch_origin = object->contact_position;
        FindAnglesXY(&object->player.external_force, &object->player.saved_pitch, NULL);
        object->velocity.z = 0.0f;
        object->velocity.x = 0.0f;
    }
}

i32 Climb_SetTargetMom(GAMEOBJECT *object, u16 input_angle) {
    union {
        f32 value;
        u32 bits;
    } raw_speed, speed;

    i16 animation = object->player.context_animation;
    u16 angle;
    u16 side_angle;
    f32 lateral;
    speed.value = 0.0f;
    if (animation != ACTION_NONE && (animation == LEGOACT_CLIMB_UP || animation == LEGOACT_CLIMB_DOWN || animation == LEGOACT_CLIMB_LEFT ||
                                     animation == LEGOACT_CLIMB_RIGHT || animation == LEGOACT_MAGNET_WALK_METAL)) {
        if (object->character_model->model_data_b[animation] != NULL) {
            raw_speed.value = AnimSpeed(object->character_model, animation);
            speed.bits = raw_speed.bits & 0x7fffffff;
        }
    }
    if (speed.value == 0.0f) {
        speed.value = 0.5f;
    }
    angle = object->movement_facing_angle;
    object->target_velocity.x = NuTrigTable[angle >> 1] * speed.value;
    object->target_velocity.y = 0.0f;
    object->target_velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * speed.value;
    if (object->player.pad->input_magnitude > 0.0f) {
        side_angle = angle + 0x4000;
        lateral = PushingTowardsAngle(input_angle, side_angle);
        object->target_velocity.x += NuTrigTable[side_angle >> 1] * lateral * speed.value;
        object->target_velocity.z += lateral * NuTrigTable[((side_angle + 0x4000) >> 1) & 0x7fff] * speed.value;
        object->target_velocity.y += PushingTowardsAngle(input_angle, object->movement_facing_angle) * speed.value;
    }
    return 1;
}

void Hang_MoveCode(GAMEOBJECT *object) {
    i16 animation;
    u8 variant;
    if (LEGOCONTEXT_HANG == -1) {
        return;
    }
    if (object->player.character_context == LEGOCONTEXT_HANG) {
        if ((object->player.pad->buttons_pressed & GAMEPAD_JUMP) != 0 && object->player.context_variant != 5) {
            object->velocity.y = 0.0f;
            object->player.character_context = CHARACTER_CONTEXT_NONE;
            return;
        }
        if (object->ground_flags != 0) {
            object->player.character_context = CHARACTER_CONTEXT_NONE;
            return;
        }
        variant = object->player.context_variant;
        if ((u8)object->roof_surface_id != variant) {
            object->player.character_context = CHARACTER_CONTEXT_NONE;
            object->velocity.y = 0.0f;
            return;
        }
        if (!(object->player.pad->input_magnitude > 0.0f) || (animation = LEGOACT_HANG_MOVE) == -1) {
            animation = LEGOACT_HANG_IDLE;
        }
        object->player.context_animation = animation;
        if (object->contact_count != 0 && object->contact_material[0] == variant) {
            object->player.context_duration = 0.25f;
            object->player.external_force = object->contact_normal;
        } else {
            object->player.context_duration -= FRAMETIME;
            if (object->player.context_duration <= 0.0f) {
                object->player.character_context = CHARACTER_CONTEXT_NONE;
            }
        }
        return;
    }
    if (object->ground_flags != 0 || object->contact_count == 0 || CanClimbSurface(object, (i8)object->contact_material[0]) == 0 ||
        !(object->contact_normal.y < -NuTrigTable[0x3000])) {
        return;
    }
    if (object->player.character_context != CHARACTER_CONTEXT_NONE &&
        !(LEGOCONTEXT_CLIMB != -1 && object->player.character_context == LEGOCONTEXT_CLIMB) &&
        !(LEGOCONTEXT_JUMP != -1 && object->player.character_context == LEGOCONTEXT_JUMP && object->player.context_timer >= (f64)0.1f)) {
        return;
    }
    object->player.character_context = (i8)LEGOCONTEXT_HANG;
    object->player.context_duration = 0.25f;
    object->player.context_animation = LEGOACT_HANG_IDLE;
    object->player.context_variant = object->contact_material[0];
    object->player.external_force = object->contact_normal;
}

i32 Hang_SetTargetMom(GAMEOBJECT *object) {
    if (object->player.pad->input_magnitude > 0.0f) {
        f32 speed;
        if (LEGOACT_HANG_MOVE != -1 && object->character_model->model_data_b[LEGOACT_HANG_MOVE] != NULL) {
            speed = AnimSpeed(object->character_model, LEGOACT_HANG_MOVE);
            if (speed < 0.25f) {
                speed = 0.25f;
            }
        } else {
            speed = 0.5f;
        }
        object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1] * speed;
        object->target_velocity.z = NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] * speed;
    } else {
        object->target_velocity.x = object->target_velocity.z = 0.0f;
    }
    object->target_velocity.y = 0.5f;
    return 1;
}

void *CreateClimbObjectSys(VARIPTR *buffer, VARIPTR *end, i32 count) {
    CLIMB_SYSTEM *system = NULL;
    if (count != 0) {
        system = GameBufferAlloc(buffer, end, sizeof(*system));
        if (system != NULL) {
            system->capacity = (u16)count;
            system->objects = GameBufferAlloc(buffer, end, system->capacity * sizeof(CLIMB_SLOT));
        }
    }
    return system;
}

i32 LedgeTerrain_CheckAnims = 1;
f32 LEDGETERRAINLOOKAHEAD = 0.02f;
i32 ClimbNormalProbeCount = 8;

void ClimbObject_FindNormal(CLIMB_SLOT *object) {
    AIPATHNODE *first = &object->path->nodes[object->connection->node_indices[0]];
    AIPATHNODE *second = &object->path->nodes[object->connection->node_indices[1]];
    AIPATHNODE *node = first->position.y < second->position.y ? first : second;
    NUVEC displacement;
    NUVEC origin;
    i32 index;
    for (index = 0; index < ClimbNormalProbeCount; ++index) {
        displacement.x = (f32)(node->radius + 0.5);
        displacement.y = 0;
        displacement.z = 0;
        NuVecRotateY(&displacement, &displacement, (index * 65536) / 4);
        origin.x = node->position.x;
        origin.y = (f32)((node->max_height + node->min_height) * 0.5);
        origin.z = node->position.z;
        if (NewRayCastScaleYMask(&origin, &displacement, 0.0f, 1.0f, 0,
                                 GetSurfaceInfoDefaultMask.word | GetSurfaceInfoRuntimeMask.word | 0x5f) != 0 &&
            (TerSurface[(i8)NewRayCastGetImpactTerrainType()].flags & TERSURFACE_CLIMBABLE) != 0) {
            object->flags |= 1;
            object->normal = ShadNorm;
            object->impact_x = origin.x + displacement.x;
            object->impact_z = origin.z + displacement.z;
            return;
        }
    }
}

void InitClimbObjectSys(WORLDINFO *world) {
    i32 index;
    i32 connection_index;
    AIPATH *path;
    AIPATHCNX *connection;
    CLIMB_SLOT *object;
    if (world == NULL || world->ai_sys == NULL || world->ai_sys->path_sys == NULL || world->climb_objects == NULL) {
        return;
    }
    world->climb_objects->count = 0;
    memset(world->climb_objects->objects, 0, world->climb_objects->capacity * sizeof(CLIMB_SLOT));
    for (index = 0; index < world->ai_sys->path_sys->path_count; ++index) {
        path = world->ai_sys->path_sys->paths[index];
        if (path == NULL) {
            continue;
        }
        connection = path->connections;
        for (connection_index = 0; connection_index < path->connection_count; ++connection_index, ++connection) {
            if (world->climb_objects->count < world->climb_objects->capacity &&
                (connection->traversal_flags[0] & (LEGO_AIPATHCNX_VARIANT_FLAG0 | LEGO_AIPATHCNX_VARIANT_FLAG1)) != 0) {
                object = &world->climb_objects->objects[world->climb_objects->count++];
                object->connection = connection;
                object->path = path;
                ClimbObject_FindNormal(object);
            }
        }
    }
}

CLIMB_SLOT *FindClimbObject(CLIMB_SYSTEM *system, AIPATHCNX *connection) {
    i32 index;
    CLIMB_SLOT *object;
    if (system != NULL) {
        object = system->objects;
        for (index = 0; index < system->count; ++index, ++object) {
            if (object->connection != connection) {
                continue;
            }
            if (object->flags & 2) {
                return NULL;
            }
            if (object->flags & 1) {
                return object;
            }
            ClimbObject_FindNormal(object);
            if (object->flags & 1) {
                return object;
            }
            object->flags |= 2;
        }
    }
    return NULL;
}
