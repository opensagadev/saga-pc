#include "saga.h"

void __cdecl NuSpecialGetBounds(NUSPECIALHANDLE *, NUVEC *, NUVEC *);
void __cdecl PushAway(NUVEC *, f32, NUVEC *, NUVEC *, GAMEOBJECT *, GAMEOBJECT *, f32, u32);
extern const f32 PlayerBuildItNegativeLimitPc;

typedef struct buildit_matrices_pc_s {
    NUMTX start_mtx;
    NUMTX end_mtx;
} BUILDIT_MATRICES_PC;
STATIC_ASSERT(offsetof(BUILDIT_MATRICES_PC, start_mtx) == 0);
STATIC_ASSERT(offsetof(BUILDIT_MATRICES_PC, end_mtx) == 0x40);

typedef struct buildit_animation_pc_s {
    u8 unknown_00[4];
    NUSPECIALHANDLE special;
    u8 unknown_10[0x14];
    BUILDIT_MATRICES_PC *object_data;
} BUILDIT_ANIMATION_PC;
STATIC_ASSERT(offsetof(BUILDIT_ANIMATION_PC, special) == 4);
STATIC_ASSERT(offsetof(BUILDIT_ANIMATION_PC, object_data) == 0x24);

typedef struct buildit_movement_pc_s {
    u8 unknown_00[0x10];
    void *anim_set;
    BUILDIT_ANIMATION_PC **anim_objects;
    u8 unknown_18[0x10];
    NUVEC start_position;
    u8 unknown_34[0x38];
    u8 anim_object_count;
    u8 unknown_6d[2];
    u8 built_object_count;
    u8 batch_size;
    u8 unknown_71;
    u8 state_flags;
} BUILDIT_MOVEMENT_PC;
STATIC_ASSERT(offsetof(BUILDIT_MOVEMENT_PC, anim_set) == 0x10);
STATIC_ASSERT(offsetof(BUILDIT_MOVEMENT_PC, anim_objects) == 0x14);
STATIC_ASSERT(offsetof(BUILDIT_MOVEMENT_PC, start_position) == 0x28);
STATIC_ASSERT(offsetof(BUILDIT_MOVEMENT_PC, anim_object_count) == 0x6c);
STATIC_ASSERT(offsetof(BUILDIT_MOVEMENT_PC, built_object_count) == 0x6f);
STATIC_ASSERT(offsetof(BUILDIT_MOVEMENT_PC, batch_size) == 0x70);
STATIC_ASSERT(offsetof(BUILDIT_MOVEMENT_PC, state_flags) == 0x72);

void __cdecl GizGetBuildItPlayerPos(GAMEOBJECT *player, NUVEC *position, NUVEC *target) {
    BUILDIT_MOVEMENT_PC *buildit = (BUILDIT_MOVEMENT_PC *)player->grapple;
    BUILDIT_MATRICES_PC *data;
    i32 count, index, batch_size, batch_count, end;
    NUVEC total;
    f32 dx, dz, inverse_distance;
    if (buildit == NULL || buildit->anim_set == NULL || buildit->anim_object_count == 0)
        return;
    count = buildit->anim_object_count;
    index = buildit->built_object_count;
    if (index >= count)
        index = count - 1;
    if (position != NULL) {
        batch_size = buildit->batch_size;
        index = (index / batch_size) * batch_size;
        batch_count = 0;
        while (batch_count < batch_size && index + batch_count < count)
            ++batch_count;
        total = v000;
        end = index + batch_count;
        for (; index < end; ++index) {
            buildit = (BUILDIT_MOVEMENT_PC *)player->grapple;
            data = buildit->anim_objects[index]->object_data;
            NuVecAdd(&total, &total, (NUVEC *)&data->start_mtx.m30);
        }
        if (batch_count > 1)
            NuVecScale(position, &total, 1.0f / batch_count);
        else
            *position = total;
        buildit = (BUILDIT_MOVEMENT_PC *)player->grapple;
        position->x = (position->x + buildit->start_position.x) * NuQFntHalfScale;
        buildit = (BUILDIT_MOVEMENT_PC *)player->grapple;
        position->z = (position->z + buildit->start_position.z) * NuQFntHalfScale;
        buildit = (BUILDIT_MOVEMENT_PC *)player->grapple;
        data = buildit->anim_objects[buildit->built_object_count]->object_data;
        dx = position->x - data->end_mtx.m30;
        dz = position->z - data->end_mtx.m32;
        inverse_distance = 1.0f / NuFsqrt(dx * dx + dz * dz);
        position->x += dx * inverse_distance * player->bounds_radius;
        position->z += dz * inverse_distance * player->bounds_radius;
    }
    if (target != NULL) {
        buildit = (BUILDIT_MOVEMENT_PC *)player->grapple;
        data = buildit->anim_objects[index]->object_data;
        *target = *(NUVEC *)&data->end_mtx.m30;
    }
}

void __cdecl GizBuildItPushAwayFromStart(GAMEOBJECT *player, void *context) {
    BUILDIT_MOVEMENT_PC *buildit = (BUILDIT_MOVEMENT_PC *)context;
    BUILDIT_MATRICES_PC *data;
    NUVEC minimum = v000, maximum = v000, start_position, centre, radius_centre;
    f32 player_x, player_z, radius, nearest_distance, dx, dz, distance;
    i32 count, nearest, i;
    if (player == NULL || buildit == NULL || buildit->anim_set == NULL || buildit->anim_object_count == 0)
        return;
    count = buildit->anim_object_count;
    start_position = buildit->start_position;
    player_x = player->collision_position.x;
    player_z = player->collision_position.z;
    minimum.x = minimum.z = AIPositionUnsetPc;
    maximum.x = maximum.z = PlayerBuildItNegativeLimitPc;
    if (buildit->state_flags & 0x10) {
        nearest_distance = AIPositionUnsetPc;
        nearest = 0;
        for (i = 0; i < count; ++i) {
            data = buildit->anim_objects[i]->object_data;
            dx = data->start_mtx.m30 - player_x;
            dz = data->start_mtx.m32 - player_z;
            distance = dx * dx + dz * dz;
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest = i;
                centre = *(NUVEC *)&data->start_mtx.m30;
            }
        }
        NuSpecialGetBounds(&buildit->anim_objects[nearest]->special, &minimum, &maximum);
        NuSpecialGetRadius(&buildit->anim_objects[nearest]->special, &radius_centre, &radius);
    } else {
        centre = start_position;
        for (i = 0; i < count; ++i) {
            data = buildit->anim_objects[i]->object_data;
            dx = data->start_mtx.m30 - start_position.x;
            dz = data->start_mtx.m32 - start_position.z;
            maximum.x = maximum.x > dx ? maximum.x : dx;
            maximum.z = maximum.z > dz ? maximum.z : dz;
            minimum.x = minimum.x < dx ? minimum.x : dx;
            minimum.z = minimum.z < dz ? minimum.z : dz;
        }
        radius = NuFsqrt(maximum.x * maximum.x + maximum.z * maximum.z);
    }
    maximum.x += centre.x;
    maximum.z += centre.z;
    minimum.x += centre.x;
    minimum.z += centre.z;
    PushAway(&centre, radius, &minimum, &maximum, player, NULL, 1.0f, 0);
}

typedef struct buildit_reset_matrices_pc_s {
    NUMTX start_mtx, end_mtx, draw_mtx;
} BUILDIT_RESET_MATRICES_PC;
STATIC_ASSERT(offsetof(BUILDIT_RESET_MATRICES_PC, start_mtx) == 0);
STATIC_ASSERT(offsetof(BUILDIT_RESET_MATRICES_PC, end_mtx) == 0x40);
STATIC_ASSERT(offsetof(BUILDIT_RESET_MATRICES_PC, draw_mtx) == 0x80);
typedef struct buildit_reset_pc_s {
    char name[16];
    GAMEANIMSET *anim_set;
    GAMEANIMOBJ **anim_objects;
    struct buildit_reset_pc_s *linked_buildit;
    NUVEC file_position, start_position, position;
    f32 bounds_radius;
    f32 reset_step_timer, reset_step_duration;
    f32 interaction_radius, field_50;
    i16 wobble_parameter, field_56, field_58, field_5a;
    NUVEC effect_position;
    f32 activation_radius;
    u8 anim_object_count, build_state, builders_active, built_object_count;
    u8 batch_size, progress;
    u16 state_flags;
    u16 linked_rotation;
    i16 room_index;
    i16 blowup_type;
    u16 availability_flags;
} BUILDIT_RESET_PC;
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, anim_set) == 0x10);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, anim_objects) == 0x14);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, linked_buildit) == 0x18);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, reset_step_timer) == 0x44);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, reset_step_duration) == 0x48);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, anim_object_count) == 0x6c);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, build_state) == 0x6d);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, builders_active) == 0x6e);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, built_object_count) == 0x6f);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, availability_flags) == 0x7a);
STATIC_ASSERT(sizeof(BUILDIT_RESET_PC) == 0x7c);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, file_position) == 0x1c);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, start_position) == 0x28);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, position) == 0x34);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, bounds_radius) == 0x40);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, wobble_parameter) == 0x54);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, state_flags) == 0x72);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, linked_rotation) == 0x74);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, room_index) == 0x76);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, blowup_type) == 0x78);
STATIC_ASSERT(sizeof(GIZBUILDITANIMDATA_s) == 0xc8);
STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, start_mtx) == 0);
STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, end_mtx) == 0x40);
STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, draw_mtx) == 0x80);
STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, wobble_time) == 0xc0);

i32 __cdecl GizBuildIt_AtEnd(void *buildit_pointer) {
    BUILDIT_RESET_PC *buildit = (BUILDIT_RESET_PC *)buildit_pointer;
    if (buildit != NULL && buildit->build_state == 2)
        return 1;
    return 0;
}

void __cdecl GizBuildIt_SetToStart(void *buildit_pointer, i32 emit_debris, i32 keep_built_pieces) {
    BUILDIT_RESET_PC *buildit = (BUILDIT_RESET_PC *)buildit_pointer;
    i32 restored_count = 0, i;
    f32 progress;
    if (buildit == NULL || buildit->anim_set == NULL)
        return;
    if (keep_built_pieces && buildit->anim_object_count != 0) {
        progress = (f32)buildit->built_object_count / (i32)buildit->anim_object_count;
        if (progress >= 0.5f) {
            if (progress < (f64)0.9f)
                restored_count = buildit->anim_object_count >> 1;
            else
                restored_count = buildit->built_object_count;
        }
    }
    buildit->availability_flags |= 2;
    for (i = 0; i < buildit->anim_object_count; ++i) {
        if (emit_debris && i < buildit->built_object_count) {
            BUILDIT_RESET_MATRICES_PC *data = (BUILDIT_RESET_MATRICES_PC *)buildit->anim_objects[i]->object_data;
            AddGameDebris(WORLD->game_debris_sys, GizBuilditGDeb[qrand() / 0x2aab], (NUVEC *)&data->end_mtx.m30);
        }
    }
    if (buildit->linked_buildit != NULL) {
        for (i = restored_count; i < buildit->anim_object_count; ++i) {
            BUILDIT_RESET_MATRICES_PC *data = (BUILDIT_RESET_MATRICES_PC *)buildit->anim_objects[i]->object_data;
            data->draw_mtx = data->start_mtx;
        }
    } else {
        GameAnimSet_SetVisibility(buildit->anim_set, 1);
        for (i = restored_count; i < buildit->anim_object_count; ++i) {
            GAMEANIMOBJ *object = buildit->anim_objects[i];
            object->instance_animation->flags &= ~1u;
            object->instance_animation->ltime = object->start_frame;
        }
        buildit->reset_step_timer = 0.3f;
        buildit->reset_step_duration = 0.3f;
    }
    buildit->availability_flags &= ~4;
    buildit->built_object_count = (u8)restored_count;
    buildit->builders_active = 0;
    buildit->build_state = 0;
}

void __cdecl ReleaseBuildIt(GAMEOBJECT *player, i32 completed) {
    f32 completion_blend;
    if (LEGOCONTEXT_BUILDIT == -1 || player->character_context != LEGOCONTEXT_BUILDIT)
        return;
    player->character_context = -1;
    completion_blend = completed != 0 ? 0.6f : 0.0f;
    GameCam_Blend(GameCam, 0.5f, completion_blend, 1);
}

void GizBuildit_Reset(GIZBUILDIT_s *buildit_pointer, void *world_ptr) {
    BUILDIT_RESET_PC *buildit = (BUILDIT_RESET_PC *)buildit_pointer;
    buildit->availability_flags = (u16)((buildit->availability_flags & ~0x200u) | 3u);

    buildit->start_position = buildit->file_position;
    buildit->position = buildit->file_position;
    buildit->bounds_radius = NuTrigTable[0x3000];

    if (buildit->anim_set != NULL && buildit->anim_set->objects != NULL) {
        struct gameanimobj_s *object;
        f32 span, span_z;
        NUVEC start_max = {-1000000000.0f, -1000000000.0f, -1000000000.0f};
        NUVEC start_min = {1000000000.0f, 1000000000.0f, 1000000000.0f};
        NUVEC end_min = start_min;
        NUVEC end_max = start_max;

        for (object = buildit->anim_set->objects; object != NULL; object = object->next) {
            NUMTX evaluated;
            GIZBUILDITANIMDATA_s *data = (GIZBUILDITANIMDATA_s *)(object->object_data);
            memset(data, 0, sizeof(*data));

            EvalAnim(&object->special, object->start_frame, &evaluated, 1);
            data->start_mtx = evaluated;
            if (buildit->linked_buildit != NULL) {
                data->start_mtx.m30 -= buildit->linked_buildit->file_position.x;
                data->start_mtx.m31 -= buildit->linked_buildit->file_position.y;
                data->start_mtx.m32 -= buildit->linked_buildit->file_position.z;
                NuMtxRotateY(&data->start_mtx, buildit->linked_rotation);
                data->start_mtx.m30 += buildit->file_position.x;
                data->start_mtx.m31 += buildit->file_position.y;
                data->start_mtx.m32 += buildit->file_position.z;
                data->draw_mtx = data->start_mtx;
            }

            EvalAnim(&object->special, object->end_frame, &evaluated, 1);
            data->end_mtx = evaluated;
            if (buildit->linked_buildit != NULL) {
                data->end_mtx.m30 -= buildit->linked_buildit->file_position.x;
                data->end_mtx.m31 -= buildit->linked_buildit->file_position.y;
                data->end_mtx.m32 -= buildit->linked_buildit->file_position.z;
                NuMtxRotateY(&data->end_mtx, buildit->linked_rotation);
                data->end_mtx.m30 += buildit->file_position.x;
                data->end_mtx.m31 += buildit->file_position.y;
                data->end_mtx.m32 += buildit->file_position.z;
            }

            if (data->start_mtx.m30 < start_min.x)
                start_min.x = data->start_mtx.m30;
            if (data->start_mtx.m30 > start_max.x)
                start_max.x = data->start_mtx.m30;
            if (data->start_mtx.m31 < start_min.y)
                start_min.y = data->start_mtx.m31;
            if (data->start_mtx.m31 > start_max.y)
                start_max.y = data->start_mtx.m31;
            if (data->start_mtx.m32 < start_min.z)
                start_min.z = data->start_mtx.m32;
            if (data->start_mtx.m32 > start_max.z)
                start_max.z = data->start_mtx.m32;

            if (data->end_mtx.m30 < end_min.x)
                end_min.x = data->end_mtx.m30;
            if (data->end_mtx.m30 > end_max.x)
                end_max.x = data->end_mtx.m30;
            if (data->end_mtx.m31 < end_min.y)
                end_min.y = data->end_mtx.m31;
            if (data->end_mtx.m31 > end_max.y)
                end_max.y = data->end_mtx.m31;
            if (data->end_mtx.m32 < end_min.z)
                end_min.z = data->end_mtx.m32;
            if (data->end_mtx.m32 > end_max.z)
                end_max.z = data->end_mtx.m32;

            data->wobble_time = 0.2f;
        }

        buildit->start_position.x = (f32)((f64)(start_min.x + start_max.x) * 0.5);
        buildit->start_position.y = (f32)((f64)(start_min.y + start_max.y) * 0.5);
        buildit->start_position.z = (f32)((f64)(start_min.z + start_max.z) * 0.5);
        span = start_max.x - start_min.x;
        span_z = start_max.z - start_min.z;
        buildit->bounds_radius = span > span_z ? span : span_z;
        buildit->bounds_radius *= NuTrigTable[0x3000];
        buildit->position.x = (f32)((f64)(end_min.x + end_max.x) * 0.5);
        buildit->position.y = (f32)((f64)(end_min.y + end_max.y) * 0.5);
        buildit->position.z = (f32)((f64)(end_min.z + end_max.z) * 0.5);
    }

    if (buildit->wobble_parameter != 0) {
        buildit->state_flags |= GIZBUILDIT_STATE_ROTATING_WOBBLE;
    } else {
        buildit->state_flags &= (u16)(~GIZBUILDIT_STATE_ROTATING_WOBBLE);
    }
    buildit->build_state = GIZBUILDIT_BUILD_IDLE;
    buildit->built_object_count = 0;

    if (world_ptr != NULL) {
        WORLDINFO *world = (WORLDINFO *)(world_ptr);
        buildit->room_index =
            world->current_gscn != NULL ? (i16)(NuPortalWhichRoom(world->current_gscn, &buildit->file_position)) : -1;
    }
}

typedef struct buildit_reset_system_pc_s {
    BUILDIT_RESET_PC *buildits;
    u16 count;
} BUILDIT_RESET_SYSTEM_PC;
STATIC_ASSERT(offsetof(BUILDIT_RESET_SYSTEM_PC, buildits) == 0);
STATIC_ASSERT(offsetof(BUILDIT_RESET_SYSTEM_PC, count) == 4);

typedef struct buildit_reset_progress_pc_s {
    u32 completed[2];
    u32 active[2];
    u32 visible[2];
    u32 reward_released[2];
    u8 built_object_count[64];
} BUILDIT_RESET_PROGRESS_PC;
STATIC_ASSERT(sizeof(BUILDIT_RESET_PROGRESS_PC) == 0x60);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PROGRESS_PC, completed) == 0);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PROGRESS_PC, active) == 8);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PROGRESS_PC, visible) == 0x10);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PROGRESS_PC, reward_released) == 0x18);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PROGRESS_PC, built_object_count) == 0x20);

void __cdecl GizBuildIts_Reset(void *world, void *system_pointer, void *progress_pointer) {
    BUILDIT_RESET_SYSTEM_PC *system = (BUILDIT_RESET_SYSTEM_PC *)system_pointer;
    BUILDIT_RESET_PROGRESS_PC *progress = (BUILDIT_RESET_PROGRESS_PC *)progress_pointer;
    BUILDIT_RESET_PC *buildit = system->buildits;
    i32 index;
    for (index = 0; index < system->count; ++index, ++buildit) {
        GizBuildit_Reset((GIZBUILDIT_s *)buildit, world);
        if (progress != NULL && index < 64) {
            i32 word = index / 32;
            u32 bit = 1u << (index & 31);
            buildit->build_state = (progress->completed[word] & bit) != 0 ? 2 : 0;
            buildit->availability_flags =
                (u16)((buildit->availability_flags & ~2u) | ((progress->visible[word] & bit) != 0 ? 2u : 0u));
            buildit->availability_flags =
                (u16)((buildit->availability_flags & ~1u) | ((progress->active[word] & bit) != 0 ? 1u : 0u));
            buildit->availability_flags = (u16)((buildit->availability_flags & ~0x200u) |
                                                ((progress->reward_released[word] & bit) != 0 ? 0x200u : 0u));
            buildit->built_object_count = progress->built_object_count[index];
        }
    }
}

void GizBuildIt_KillParts(GIZBUILDIT_s *buildit) {
    NUVEC velocity;
    struct gameanimobj_s *object;
    if (buildit == NULL)
        return;
    velocity = v000;
    if (buildit->anim_set == NULL)
        return;
    for (object = buildit->anim_set->objects; object != NULL; object = object->next) {
        struct PART_PC *part;
        ADDPART_PC params = Default_ADDPART;
        NUMTX matrix = ((GIZBUILDITANIMDATA_s *)(object->object_data))->end_mtx;
        params.matrix = &matrix;
        params.velocity = &velocity;
        params.special = &object->special;
        params.stop_fn = PartStop_Flickerer;
        params.draw_fn = PartDraw_Flickerer;
        params.impact_fn = PartImpact_Brick;
        params.field_14 = 0.2f;
        params.field_18 = 0.2f;
        params.gravity = -4.0f;
        params.flags = 0x90;
        params.field_c4 = 1;
        params.time_step = FRAMETIME;
        part = AddPart(&params);
        if (part != NULL) {
            f32 random = ((f32)(qrand())) * 1.5259021893143654e-05f;
            part->time_remaining = random + random + 7.0f;
        }
    }
}

void GizBuildIt_SetStepTime(GIZBUILDIT_s *buildit, GAMEOBJECT *player) {
    buildit->step_duration = 0.3f;
    if (player != NULL) {
        buildit->step_duration /= GizBuildItMul(player);
    }
    buildit->step_timer = buildit->step_duration;
}

f32 __cdecl GizBuildItMul(GAMEOBJECT *player) {
    BUILDIT_MOVEMENT_PC *buildit;
    i32 amount, completed, total;
    if ((player->flags_1fc_word & 0x80) != 0 && Player_HasFastBuild(player))
        return 3.0f;
    buildit = (BUILDIT_MOVEMENT_PC *)player->grapple;
    amount = player->big_jump_animation;
    completed = buildit->built_object_count;
    total = buildit->anim_object_count;
    if ((completed - amount + total) / 2 <= completed)
        amount = total - completed;
    if (amount > 10)
        amount = 10;
    else if (amount < 0)
        amount = 0;
    return (f32)((f64)amount / 10.0 + 1.0);
}

STATIC_ASSERT(sizeof(BUILDIT_RESET_PC) == 0x7c);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, name) == 0x0);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, anim_set) == 0x10);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, anim_objects) == 0x14);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, linked_buildit) == 0x18);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, file_position) == 0x1c);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, start_position) == 0x28);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, position) == 0x34);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, bounds_radius) == 0x40);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, reset_step_timer) == 0x44);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, reset_step_duration) == 0x48);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, interaction_radius) == 0x4c);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, field_50) == 0x50);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, wobble_parameter) == 0x54);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, field_56) == 0x56);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, field_58) == 0x58);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, field_5a) == 0x5a);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, effect_position) == 0x5c);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, activation_radius) == 0x68);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, anim_object_count) == 0x6c);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, build_state) == 0x6d);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, builders_active) == 0x6e);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, built_object_count) == 0x6f);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, batch_size) == 0x70);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, progress) == 0x71);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, state_flags) == 0x72);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, linked_rotation) == 0x74);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, room_index) == 0x76);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, blowup_type) == 0x78);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, availability_flags) == 0x7a);

extern const char GizmoRegistryGizBuildItsNamePc[];

i32 __cdecl GizBuildIts_Load(void *world_pointer, void *system_pointer) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    GIZBUILDITSYS_s *system = (GIZBUILDITSYS_s *)system_pointer;
    BUILDIT_RESET_PC *buildit;
    GAMEANIMOBJ *object;
    u8 version;
    i32 index, linked_index, animation_index = 0, length;
    char linked_name[16];
    char blowup_name[32];
    version = (u8)EdFileReadChar();
    system->count = (u16)EdFileReadShort();
    if (system->count == 0)
        return 1;
    buildit = (BUILDIT_RESET_PC *)system->buildits;
    for (index = 0; index < system->count; ++index, ++buildit) {
        EdFileRead(buildit->name, 16);
        EdFileReadNuVec(&buildit->file_position);
        GizmoFileReadGameAnimSet(buildit->anim_set, world, NULL, version, (char *)GizmoRegistryGizBuildItsNamePc,
                                 buildit->name);
        buildit->interaction_radius = EdFileReadFloat();
        if (version <= 6)
            EdFileReadFloat();
        buildit->field_56 = EdFileReadShort();
        buildit->wobble_parameter = EdFileReadShort();
        buildit->batch_size = (u8)EdFileReadChar();
        buildit->progress = (u8)EdFileReadChar();
        if (version >= 6)
            buildit->field_50 = EdFileReadFloat();
        buildit->blowup_type = -1;
        if (version >= 7) {
            if (version == 7) {
                buildit->blowup_type = EdFileReadShort();
            } else {
                length = (i8)EdFileReadChar();
                if (length != 0) {
                    EdFileRead(blowup_name, length);
                    buildit->blowup_type = (i16)GizmoBlowupGetNameTableId(blowup_name);
                    if (buildit->blowup_type != -1)
                        buildit->availability_flags |= 0x80;
                }
            }
            buildit->field_58 = EdFileReadShort();
            buildit->field_5a = EdFileReadShort();
            EdFileReadNuVec(&buildit->effect_position);
        }
        if (version >= 9)
            buildit->activation_radius = EdFileReadFloat();
        else
            buildit->activation_radius = world->area != NULL && (world->area->flags & 1) ? 12.0f : 1.75f;
        if (version < 4) {
            buildit->state_flags = (i16)(i8)EdFileReadChar();
            if (version >= 2 && EdFileReadChar() != 0)
                buildit->state_flags |= 0x40;
            if (version >= 3 && EdFileReadChar() != 0)
                buildit->state_flags |= 0x80;
        } else {
            buildit->state_flags = (u16)EdFileReadShort();
        }
        if (version > 4) {
            buildit->linked_rotation = (u16)EdFileReadShort();
            if (EdFileReadChar() != 0) {
                EdFileRead(linked_name, 16);
                for (linked_index = 0; linked_index < system->count; ++linked_index) {
                    BUILDIT_RESET_PC *linked = &((BUILDIT_RESET_PC *)system->buildits)[linked_index];
                    if (NuStrICmp(linked->name, linked_name) == 0) {
                        buildit->availability_flags |= 3;
                        buildit->linked_buildit = &((BUILDIT_RESET_PC *)system->buildits)[linked_index];
                        break;
                    }
                }
            }
        }
        buildit->anim_object_count = 0;
        buildit->anim_objects = (GAMEANIMOBJ **)&system->anim_objects[animation_index];
        for (object = buildit->anim_set->objects; object != NULL; object = object->next) {
            if ((object->animation != NULL && object->instance_animation != NULL) || (buildit->state_flags & 0x40)) {
                buildit->anim_objects[buildit->anim_object_count] = object;
                ++buildit->anim_object_count;
                ++animation_index;
            }
        }
        GizBuildit_Reset((GIZBUILDIT_s *)buildit, world);
    }
    return 1;
}

void __cdecl GizBuildIts_EarlyUpdate(void *world, void *pointer, f32 delta_time) {
    GIZBUILDITSYS_s *system = (GIZBUILDITSYS_s *)pointer;
    BUILDIT_RESET_PC *buildit;
    i32 i;
    if (system == NULL)
        return;
    buildit = (BUILDIT_RESET_PC *)system->buildits;
    for (i = 0; i < system->count; ++i, ++buildit) {
        buildit->availability_flags &= ~4;
        buildit->builders_active = 0;
    }
    (void)world;
    (void)delta_time;
}
