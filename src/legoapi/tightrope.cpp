#include "saga.h"

typedef struct TIGHTROPE_MOTION_PC {
    char name[16];
    NUVEC start, end;
    u16 start_x_rotation, end_x_rotation, start_y_rotation, end_y_rotation;
    u8 start_model, end_model;
    i8 start_visible;
    u8 reserved_33;
    NUVEC direction;
    f32 horizontal_length;
    u16 rotation;
    u8 enabled, available;
} TIGHTROPE_MOTION_PC;
typedef struct WORLD_TIGHTROPE_VIEW_PC {
    u8 unknown_0000[0x5058];
    TIGHTROPE_MOTION_PC *tightropes;
    i32 tightrope_count;
} WORLD_TIGHTROPE_VIEW_PC;
typedef struct TIGHTROPE_OBJECT_AUX_PC {
    u8 unknown_0000[0xd28];
    f32 jump_reentry_timer;
} TIGHTROPE_OBJECT_AUX_PC;
STATIC_ASSERT(sizeof(TIGHTROPE_MOTION_PC) == 0x48);
STATIC_ASSERT(offsetof(TIGHTROPE_MOTION_PC, start) == 0x10);
STATIC_ASSERT(offsetof(TIGHTROPE_MOTION_PC, end) == 0x1c);
STATIC_ASSERT(offsetof(TIGHTROPE_MOTION_PC, direction) == 0x34);
STATIC_ASSERT(offsetof(TIGHTROPE_MOTION_PC, horizontal_length) == 0x40);
STATIC_ASSERT(offsetof(TIGHTROPE_MOTION_PC, rotation) == 0x44);
STATIC_ASSERT(offsetof(TIGHTROPE_MOTION_PC, enabled) == 0x46);
STATIC_ASSERT(offsetof(TIGHTROPE_MOTION_PC, available) == 0x47);
STATIC_ASSERT(offsetof(WORLD_TIGHTROPE_VIEW_PC, tightropes) == 0x5058);
STATIC_ASSERT(offsetof(WORLD_TIGHTROPE_VIEW_PC, tightrope_count) == 0x505c);
STATIC_ASSERT(offsetof(TIGHTROPE_OBJECT_AUX_PC, jump_reentry_timer) == 0xd28);
STATIC_ASSERT(offsetof(GAMEOBJECT, launch_origin) == 0x754);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_position_offset) == 0x760);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7a4) == 0x7a4);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7a8) == 0x7a8);
extern "C" TIGHTROPE_MOTION_PC *__cdecl TightRope_InRange(GAMEOBJECT *object, WORLDINFO *world, NUVEC *position) {
    TIGHTROPE_MOTION_PC *rope = ((WORLD_TIGHTROPE_VIEW_PC *)world)->tightropes;
    f32 width = 3.0f * object->bounds_radius;
    NUVEC origin = object->collision_position;
    for (i32 index = 0; index < ((WORLD_TIGHTROPE_VIEW_PC *)world)->tightrope_count; ++index, ++rope) {
        if (rope->enabled == 0 || rope->available == 0) {
            continue;
        }
        NUVEC offset;
        offset.x = origin.x - rope->start.x;
        offset.z = origin.z - rope->start.z;
        NuVecRotateY(&offset, &offset, -rope->rotation);
        if (!(offset.z >= 0.0f && rope->horizontal_length >= offset.z && offset.x >= -width && width >= offset.x)) {
            continue;
        }
        offset.x = 0.0f;
        offset.y = (rope->end.y - rope->start.y) * (offset.z / rope->horizontal_length) + rope->start.y;
        if (object->bounds_half_height > (f32)fabs((f64)offset.y - origin.y)) {
            if (position != NULL) {
                f32 margin = (object->character_data->game_character->flags_090 & 0x10000000) != 0
                                 ? object->bounds_half_height
                                 : object->bounds_radius;
                if (offset.z > rope->horizontal_length - margin) {
                    offset.z = rope->horizontal_length - margin;
                } else if (margin > offset.z) {
                    offset.z = margin;
                }
                NuVecRotateY(position, &offset, rope->rotation);
                position->x += rope->start.x;
                position->z += rope->start.z;
            }
            return rope;
        }
    }
    return NULL;
}

static i32 TightRopeSetPositionPc(TIGHTROPE_MOTION_PC *rope, NUVEC *position, GAMEOBJECT *object) {
    f32 margin;
    i32 result = 1;
    NuVecSub(position, position, &rope->start);
    position->z = NuVecDot(position, &rope->direction);
    margin = object == NULL
                 ? (f64)0.1f
                 : ((object->character_data->game_character->flags_090 & 0x10000000) != 0 ? object->bounds_half_height
                                                                                          : object->bounds_radius);
    if (position->z > rope->horizontal_length - margin) {
        position->z = rope->horizontal_length - margin;
        result = 0;
    } else if (margin > position->z) {
        position->z = margin;
        result = 0;
    }
    NuVecScale(position, &rope->direction, position->z);
    NuVecAdd(position, position, &rope->start);
    return result;
}
static i32 TightRope_MoveUpdate(GAMEOBJECT *object, i32 jumping) {
    if (!(object->pad->input_magnitude > 0.0f) || object->field_7aa == 0x8f) {
        if (jumping == 0) {
            object->field_7aa = 0x88;
        }
        return 1;
    }
    NUVEC previous = object->launch_origin;
    TIGHTROPE_MOTION_PC *rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
    u16 rotation = rope->rotation;
    f32 input = PushingTowardsAngle(GamePad_InputAngle(object, object->pad), rotation);
    f32 direction;
    f32 speed;
    if (input > NuTrigTable[0x3000]) {
        if (jumping != 0) {
            speed = 0.6f;
        } else {
            rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
            object->field_7aa = 0x89;
            object->movement_facing_angle = rope->rotation;
            speed = AnimSpeed(object->character_model, 0x89);
        }
        direction = 1.0f;
    } else if (-NuTrigTable[0x3000] > input) {
        if (jumping != 0) {
            speed = 0.6f;
        } else {
            rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
            object->field_7aa = 0x89;
            object->movement_facing_angle = rope->rotation + 0x8000;
            speed = AnimSpeed(object->character_model, 0x89);
        }
        direction = -1.0f;
    } else {
        if (jumping == 0) {
            object->field_7aa = 0x88;
        }
        return 1;
    }
    NUVEC movement;
    rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
    NuVecScale(&movement, &rope->direction, speed * direction * FRAMETIME);
    NuVecAdd(&object->launch_origin, &object->launch_origin, &movement);
    rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
    i32 result = TightRopeSetPositionPc(rope, &object->launch_origin, object);
    if (jumping == 0 && previous.x == object->launch_origin.x && previous.y == object->launch_origin.y &&
        previous.z == object->launch_origin.z) {
        object->field_7aa = 0x88;
    }
    return result;
}

static i32 TightRope_Attach(GAMEOBJECT *object, WORLDINFO *world) {
    NUVEC position;
    TIGHTROPE_MOTION_PC *rope = TightRope_InRange(object, world, &position);
    if (rope == NULL) {
        return 0;
    }
    object->character_context = 0x44;
    object->grapple = rope;
    object->context_variant_flags = (object->context_variant_flags & ~0x800) |
                                    ((((object->character_data->game_character->flags_090 >> 19) ^ 1) & 1) << 11);
    if (object->character_model->model_data_b[0x8f] != NULL &&
        ((object->character_data->game_character->flags_090 & 0x80000) == 0 ||
         position.y > (object->upper_position.y - object->lower_position.y) * 0.25f + object->lower_position.y)) {
        object->field_7aa = 0x8f;
        object->context_timer = AnimDuration(object->id, 0x8f, 0.0f, 0.0f, 1);
    } else {
        object->field_7aa = 0x88;
    }
    object->launch_origin = position;
    rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
    i32 difference = RotDiff(rope->rotation, object->movement_facing_angle);
    if (difference < 0) {
        difference = -difference;
    }
    rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
    u16 rotation = rope->rotation;
    if (difference > 0x4000) {
        rotation += 0x8000;
    }
    object->movement_facing_angle = rotation;
    object->context_hover_target_y = NuVecXZDist(&object->collision_position, &rope->start, NULL);
    rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
    f32 square = rope->direction.x * rope->direction.x + rope->direction.z * rope->direction.z;
    f32 horizontal_length = NuFsqrt(square);
    rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
    i32 slope = NuAtan2D(rope->direction.y, horizontal_length);
    NuVecRotateX(&position, &v010, -slope);
    rope = ((TIGHTROPE_MOTION_PC *)(object->grapple));
    NuVecRotateY(&position, &position, rope->rotation);
    FindAnglesZX(&position, &object->field_7a4, &object->field_7a8);
    if ((object->character_data->game_character->flags_090 & 0x10000000) != 0) {
        position.x = 0.0f;
        position.y = object->upper_position.y - object->lower_position.y;
        position.z = 0.0f;
        NuVecRotateZ(&object->context_position_offset, &position, object->field_7a8);
        NuVecRotateX(&object->context_position_offset, &object->context_position_offset, object->field_7a4);
        NuVecSub(&object->context_position_offset, &position, &object->context_position_offset);
    } else {
        object->context_position_offset = v000;
    }
    return 1;
}

extern "C" i32 TightRope_SnapTo(GAMEOBJECT *object, NUVEC *position) {
    object->lower_position = *position;
    object->upper_position = object->lower_position;
    object->upper_position.y += object->bounds_half_height;
    object->collision_position = object->lower_position;
    object->lower_position.y -= object->bounds_half_height;
    if (TightRope_Attach(object, WORLD) == 0) {
        return 0;
    }
    object->position.x = object->collision_position.x;
    object->position.z = object->collision_position.z;
    if ((static_cast<PLAYERCHARACTERCONFIG_PC *>(object->character_data->game_character)->flags_090 & 0x80000) != 0) {
        object->position.y = object->launch_origin.y - object->character_bottom * object->origin_scale;
    } else {
        object->position.y = object->launch_origin.y - object->character_top * object->origin_scale;
    }
    u8 origin_flag = (object->field_e24 >> 3) & 1;
    object->field_e24 &= ~8;
    GameObjectOrigin(object);
    object->field_e24 = (object->field_e24 & ~8) | (origin_flag << 3);
    return 1;
}

extern "C" void __cdecl TightRope_MoveCode(GAMEOBJECT *object, i32 jump_pressed) {
    if (object->character_context != 0x44) {
        if (object->field_281 != 0 || !(0.0f >= object->velocity.y) ||
            object->character_model->model_data_b[0x88] == NULL) {
            return;
        }
        if (object->character_context != -1) {
            if (object->character_context != 0 || !(object->context_timer >= (f64)0.1f) ||
                object->action_movement_state == 3 || object->action_movement_state == 4 ||
                object->action_movement_state == 8) {
                return;
            }
        }
        if (!((object->flags_1fc & 0x80) != 0) && (object->flags_ef4 & 0x4000) == 0) {
            return;
        }
        TightRope_Attach(object, WORLD);
        object->big_jump_animation = 0;
        object->external_force.z = 0.0f;
        object->external_force.y = 0.0f;
        return;
    }
    if (object->field_7aa != 0x8f) {
        object->external_force.y += FRAMETIME;
        if (static_cast<u16>(object->field_7aa - 5) <= 1) {
            object->context_timer += FRAMETIME;
            void *rope = object->grapple;
            if (object->field_7aa == 5) {
                if (object->context_timer >= object->context_duration + object->context_duration) {
                    StartEndOfJump(object);
                }
            } else if (object->context_timer >= object->context_duration) {
                object->field_7aa = 5;
            }
            if (0.0f >= object->velocity.y && TightRope_Attach(object, WORLD) == 0 && object->field_281 != 0) {
                object->flags_e24 |= 0x400;
                if (object->field_7aa != 5 || StartFallLand(object, -1) == 0) {
                    if (object->character_model->model_data_b[7] != NULL && object->pad->input_magnitude == 0.0f) {
                        object->character_context = 1;
                        object->field_7aa = 7;
                        object->context_timer = AnimDuration(object->id, 7, 0.0f, 0.0f, 1);
                        ((TIGHTROPE_OBJECT_AUX_PC *)object)->jump_reentry_timer = 0.2f;
                        ResetAnimPacket(&object->anim_packet, -1);
                    } else {
                        object->character_context = -1;
                    }
                }
                object->flags_e24 &= ~0x400;
            }
            if (object->character_context != 0x44 || object->grapple != rope) {
                return;
            }
            TightRope_MoveUpdate(object, 1);
            goto done;
        }
    }
    if (jump_pressed != 0) {
        if ((object->flags_1fc & 0x80) != 0) {
            object->velocity.y = 2.0f;
            object->field_7aa = 6;
            object->context_timer = 0.0f;
            f32 duration = AnimDuration(object->id, 6, 0.0f, 0.0f, 0);
            object->context_duration = duration;
            if (duration <= 0.0f)
                object->context_duration = 1.0f;
            ResetAnimPacket(&object->anim_packet, -1);
        } else {
            StartJump(object, 0);
            object->flags_e24 |= 0x1000;
        }

        return;
    }
    if (object->field_7aa != 0x8f) {
        if (TightRope_MoveUpdate(object, 0) == 0 && object->field_7aa != 0x8f && object->external_force.y >= 0.5f) {
            StartJump(object, 0);
            object->velocity.y = (object->character_data->game_character->flags_090 & 0x80000) != 0 ? 1.2f : 1.8f;
        }
    } else {
        f32 *playing = AnimPlaying(&object->anim_packet, 0x8f, 1, 0);
        if (playing != NULL) {
            f32 frame = AnimListFrame(object->character_model, object->field_7aa, 0);
            if (frame >= 1.0f && *playing >= frame && object->pad->input_magnitude > 0.0f &&
                (f32)fabs((f64)PushingTowardsAngle(GamePad_InputAngle(object, object->pad),
                                                   ((TIGHTROPE_MOTION_PC *)(object->grapple))->rotation)) >
                    NuTrigTable[0x3000]) {
                TightRope_MoveUpdate(object, 0);
            } else {
                object->context_timer -= FRAMETIME;
                if (0.0f >= object->context_timer) {
                    object->field_7aa = 0x88;
                }
            }
        }
    }
done:
    object->context_hover_target_y =
        NuVecXZDist(&object->collision_position, &((TIGHTROPE_MOTION_PC *)(object->grapple))->start, NULL);
}
