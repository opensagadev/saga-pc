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

void GizBuildIt_SetStepTime(GIZBUILDIT_s *buildit_pointer, GAMEOBJECT *player) {
    BUILDIT_RESET_PC *buildit = (BUILDIT_RESET_PC *)buildit_pointer;
    buildit->reset_step_duration = 0.3f;
    if (player != NULL)
        buildit->reset_step_duration = (f64)0.3f / GizBuildItMul(player);
    buildit->reset_step_timer = buildit->reset_step_duration;
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

STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, was_drawn) == 0xc6);
void __cdecl GizBuildIts_Draw(void *world_pointer, void *system_pointer, f32 unused) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    BUILDIT_RESET_SYSTEM_PC *system = (BUILDIT_RESET_SYSTEM_PC *)system_pointer;
    BUILDIT_RESET_PC *buildit;
    GAMEANIMOBJ *object;
    GIZBUILDITANIMDATA_s *data;
    NUMTX matrix;
    f32 reflection_height;
    i32 index;
    (void)unused;
    if (system == NULL)
        return;
    buildit = system->buildits;
    for (index = 0; index < system->count; ++index, ++buildit) {
        if (!(buildit->availability_flags & 2) ||
            (buildit->room_index >= 0 && world->rooms_visible_ptr[buildit->room_index] == 0))
            continue;
        if (buildit->state_flags & 0x400)
            EnableShadowMapRendering(0);
        else
            ResetShadowMapRendering();
        if (buildit->linked_buildit != NULL) {
            if (buildit->anim_set == NULL)
                continue;
            reflection_height = 0.0f;
            if ((buildit->state_flags & 0x100) && buildit->anim_set->objects != NULL &&
                buildit->anim_set->objects->object_data != NULL) {
                data = (GIZBUILDITANIMDATA_s *)buildit->anim_set->objects->object_data;
                reflection_height = buildit->field_50 + data->start_mtx.m31;
            }
            for (object = buildit->anim_set->objects; object != NULL; object = object->next) {
                data = (GIZBUILDITANIMDATA_s *)object->object_data;
                matrix = data->draw_mtx;
                data->was_drawn = (u8)NuSpecialDrawAt(&object->special, &matrix);
                if ((buildit->state_flags & 0x100) && !(object->flags & 2) &&
                    MatrixReflection(&data->draw_mtx, 2, reflection_height, WORLD->current_level->reflection_y,
                                     &matrix))
                    NuSpecialDrawAt(&object->special, &matrix);
            }
        } else {
            ResetShadowMapRendering();
            if (buildit->state_flags & 0x100)
                GameAnimSet_DrawReflection(buildit->anim_set, 2, buildit->field_50, NULL);
        }
    }
    ResetShadowMapRendering();
}

extern f32 BuilditHopSfxWaitPc;
extern f32 GIZBUILDITWOBBLEJUMPHEIGHT;

STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, wobble_axis) == 0xc4);
STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, was_drawn) == 0xc6);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, big_jump_animation) == 0x7b4);
STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);
STATIC_ASSERT(offsetof(GAMEPAD_PC, bound_pad) == 0);
STATIC_ASSERT(offsetof(GAMEPAD_PC, buttons_held) == 4);

static __declspec(noinline) f32 BuildItSafeRatioPc(f32 numerator, f32 denominator) {
    if (numerator != 0.0f && denominator != 0.0f) {
        volatile f32 ratio = numerator / denominator;
        return ratio;
    }
    return 0.0f;
}

static __declspec(noinline) GAMEOBJECT *BuildItFindActiveBuilderPc(BUILDIT_RESET_PC *buildit) {
    i32 index;
    if (LEGOCONTEXT_BUILDIT == -1)
        return NULL;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *player = (GAMEOBJECT *)Player[index];
        if (player != NULL && (player->flags_1fc_word & 0x80) != 0 &&
            player->character_context == LEGOCONTEXT_BUILDIT && player->grapple == buildit)
            return player;
    }
    return NULL;
}

static __declspec(noinline) f32 BuildItAxisWobbleFactorPc(NUVEC *axis) {
    NUVEC normal;
    union {
        f32 value;
        u32 bits;
    } vertical;
    volatile f32 product, factor;
    f32 sine;
    NuVecNorm(&normal, axis);
    vertical.value = NuVecDot(&normal, &v010);
    vertical.bits &= 0x7fffffffu;
    sine = NU_SIN_LUT((f64)vertical.value * 16384.0 + 16384.0);
    product = (1.0f - sine) * vertical.value;
    factor = 1.0f - product;
    return factor;
}

void __cdecl GizBuildIt_Finish(GIZBUILDIT_s *buildit_pointer) {
    BUILDIT_RESET_PC *buildit = (BUILDIT_RESET_PC *)buildit_pointer;
    buildit->build_state = 2;
    if ((buildit->state_flags & 2) != 0) {
        if (buildit->linked_buildit == NULL)
            GameAnimSet_SetVisibility(buildit->anim_set, 0);
        buildit->availability_flags &= (u16)~2u;
    }
    if (GizBuildIt_FinishFn != NULL)
        GizBuildIt_FinishFn((struct GIZBUILDIT_PC *)buildit);
}

void __cdecl GizBuildIts_LateUpdate(void *world_pointer, void *system_pointer, f32 unused) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    BUILDIT_RESET_SYSTEM_PC *system = (BUILDIT_RESET_SYSTEM_PC *)system_pointer;
    BUILDIT_RESET_PC *buildit;
    NUVEC displacement, reward_direction, finish_position, axis, centre, target;
    NUMTX matrix;
    i32 index;
    (void)unused;
    if (system == NULL)
        return;
    if (BuilditHopSfxWaitPc > 0.0f)
        BuilditHopSfxWaitPc -= FRAMETIME;
    buildit = system->buildits;
    for (index = 0; index < system->count; ++index, ++buildit) {
        if ((buildit->availability_flags & 2) == 0 || (buildit->availability_flags & 1) == 0 ||
            buildit->anim_set == NULL || buildit->anim_object_count == 0)
            continue;
        if (buildit->build_state == 1) {
            volatile f32 timer = buildit->reset_step_timer + FRAMETIME;
            f32 progress;
            i32 piece_index;
            buildit->reset_step_timer = timer;
            if (timer >= 0.6f) {
                i32 hearts;
                Hint_SetComplete(LEGOHINT_BUILD);
                GizBuildIt_Finish((GIZBUILDIT_s *)buildit);
                NuVecAdd(&finish_position, &buildit->position, &buildit->effect_position);
                GameAudio_PlaySfx(0x3a, &finish_position, 0, 0);
                NewRumbleAllPlayers(0.75f, 0.1f, 0, 0.0f);
                GameCam_Judder(GameCam, -0.4f, 0, NULL);
                if ((buildit->availability_flags & 0x200) == 0) {
                    buildit->availability_flags |= 0x200;
                    hearts = ReleaseHearts();
                    if ((u16)buildit->field_56 != 0 || hearts != 0) {
                        NuVecRotateX(&reward_direction, &v010, (u16)buildit->field_58);
                        NuVecRotateY(&reward_direction, &reward_direction, (u16)buildit->field_5a);
                        AddPickups((u16)buildit->field_56, hearts, 0, 0, &finish_position, &reward_direction, 2.0f, -1,
                                   1.75f, 2000000.0f, NULL, 1, 0);
                    }
                }
                if (buildit->blowup_type != -1) {
                    GizmoBlowUpTypeBlowUp(world, buildit->blowup_type, &buildit->position);
                    GameAnimSet_SetVisibility(buildit->anim_set, 0);
                }
                progress = 1.0f;
            } else {
                volatile f32 ratio = (f64)timer / (f64)0.6f;
                progress = ratio;
            }
            for (piece_index = 0; piece_index < buildit->anim_object_count; ++piece_index) {
                GAMEANIMOBJ *piece = buildit->anim_objects[piece_index];
                GIZBUILDITANIMDATA_s *data = (GIZBUILDITANIMDATA_s *)piece->object_data;
                f32 sine = NU_SIN_LUT((f64)progress * 32768.0);
                volatile f32 offset = (f64)sine * (f64)0.2f;
                if (buildit->linked_buildit != NULL)
                    finish_position = *(NUVEC *)&data->end_mtx.m30;
                else
                    finish_position = *NuSpecialGetPos(&piece->special);
                if ((buildit->state_flags & 1) != 0) {
                    i32 angle = (i32)((u32)(u16)buildit->wobble_parameter << 16) / 360;
                    displacement.x = 0.0f;
                    displacement.y = 0.0f;
                    displacement.z = offset;
                    NuVecRotateY(&displacement, &displacement, angle);
                } else {
                    displacement.x = 0.0f;
                    displacement.y = offset;
                    displacement.z = 0.0f;
                }
                NuVecAdd(&finish_position, &finish_position, &displacement);
                if (buildit->linked_buildit != NULL)
                    *(NUVEC *)&data->draw_mtx.m30 = finish_position;
                else
                    NuSpecialSetDrawPos((nuhspecial_s *)&piece->special, &finish_position);
            }
        } else if (buildit->build_state == 0) {
            i32 automatic = (buildit->state_flags & 0x40) != 0;
            GAMEOBJECT *builder = NULL;
            f32 buzz_time = 0.0f;
            i32 buzz_frames = 0;
            volatile f32 previous_timer, timer;
            if (!automatic && buildit->builders_active == 0) {
                GizBuildIt_SetStepTime((GIZBUILDIT_s *)buildit, NULL);
                if (buildit->linked_buildit == NULL && buildit->built_object_count < buildit->anim_object_count)
                    NuSpecialSetVisibility(&buildit->anim_objects[buildit->built_object_count]->special, 1);
                goto idle_wobble;
            }
            if (automatic) {
                if (GizBuildit_AutoBuildPosFn != NULL) {
                    u16 facing;
                    if (GizBuildit_AutoBuildPosFn(world, &buildit->start_position, &centre, &facing) != 0) {
                        volatile f32 radius;
                        u16 angle, angle_step;
                        i32 piece_index;
                        if (buildit->reset_step_timer > 4.5f) {
                            angle = (u16)(facing - 0x4000);
                            angle_step = (u16)(i32)BuildItSafeRatioPc(65536.0f, buildit->anim_object_count);
                            radius = 0.5f;
                        } else {
                            volatile f32 phase = (4.5 - (f64)buildit->reset_step_timer) / 4.5;
                            f32 sine = NU_SIN_LUT((f64)phase * 16384.0 + 32768.0 + 16384.0);
                            f64 progress = (f64)sine + 1.0;
                            volatile f32 turns = progress * 5.0;
                            angle = (u16)(i32)((f64)turns * 360.0 * 65536.0 / 360.0 + ((i32)facing - 0x4000));
                            angle_step = (u16)(i32)BuildItSafeRatioPc(65536.0f, buildit->anim_object_count);
                            radius = 0.5 - progress * (f64)0.3f;
                        }
                        for (piece_index = buildit->anim_object_count - 1; piece_index >= buildit->built_object_count;
                             --piece_index) {
                            NUVEC *position;
                            angle = (u16)(angle + angle_step);
                            target = centre;
                            target.x = centre.x + radius * NU_COS_LUT(angle);
                            target.z = centre.z + radius * NU_SIN_LUT(angle);
                            position = NuSpecialGetDrawPos(&buildit->anim_objects[piece_index]->special);
                            target.x = SeekValF(position->x, target.x, 6.0f);
                            target.y = SeekValF(position->y, target.y, 6.0f);
                            target.z = SeekValF(position->z, target.z, 6.0f);
                            if (buildit->linked_buildit != NULL) {
                                GIZBUILDITANIMDATA_s *data =
                                    (GIZBUILDITANIMDATA_s *)buildit->anim_objects[piece_index]->object_data;
                                *(NUVEC *)&data->draw_mtx.m30 = target;
                            } else {
                                NuSpecialSetDrawPos((nuhspecial_s *)&buildit->anim_objects[piece_index]->special,
                                                    &target);
                                NuSpecialUpdate(&buildit->anim_objects[piece_index]->special);
                            }
                        }
                    }
                }
            } else {
                builder = BuildItFindActiveBuilderPc(buildit);
            }
            previous_timer = buildit->reset_step_timer;
            timer = buildit->reset_step_timer - FRAMETIME;
            buildit->reset_step_timer = timer;
            if (timer <= 0.0f) {
                GAMEANIMOBJ *piece = buildit->anim_objects[buildit->built_object_count];
                if (buildit->linked_buildit != NULL) {
                    GIZBUILDITANIMDATA_s *data = (GIZBUILDITANIMDATA_s *)piece->object_data;
                    data->draw_mtx = data->end_mtx;
                    GameAudio_PlaySfx(0x3b, (NUVEC *)&data->draw_mtx.m30, 0, 0);
                } else {
                    NuSpecialSetVisibility(&piece->special, 1);
                    if (!automatic || piece->instance_animation != NULL) {
                        piece->instance_animation->flags &= ~1u;
                        piece->instance_animation->ltime = piece->end_frame;
                        EvalAnim2(&piece->special, piece->instance_animation->ltime);
                    }
                    GameAudio_PlaySfx(0x3b, NuSpecialGetDrawPos(&piece->special), 0, 0);
                    if (automatic)
                        NuSpecialSetDrawPos((nuhspecial_s *)&piece->special, NuSpecialGetPos(&piece->special));
                }
                ++buildit->built_object_count;
                if (buildit->built_object_count == buildit->anim_object_count) {
                    i32 piece_index;
                    buildit->reset_step_timer = 0.0f;
                    buildit->build_state = 1;
                    for (piece_index = 0; piece_index < buildit->anim_object_count; ++piece_index) {
                        GIZBUILDITANIMDATA_s *data =
                            (GIZBUILDITANIMDATA_s *)buildit->anim_objects[piece_index]->object_data;
                        NUVEC *position = (NUVEC *)&data->end_mtx.m30;
                        i32 debris = GizBuilditGDeb[qrand() / 0x2aab];
                        AddGameDebris(world->game_debris_sys, debris, position);
                    }
                    buzz_time = 0.1f;
                } else if (automatic) {
                    buildit->reset_step_duration = 0.3f;
                    buildit->reset_step_timer = 0.3f;
                } else if (buildit->built_object_count < buildit->anim_object_count) {
                    GizBuildIt_SetStepTime((GIZBUILDIT_s *)buildit, builder);
                    buzz_frames = 1;
                }
                if (builder != NULL) {
                    if (buzz_time > 0.0f)
                        NewBuzz(((GAMEPAD_PC *)builder->pad)->bound_pad, buzz_time, 0);
                    else if (buzz_frames != 0)
                        NewBuzzFrames(((GAMEPAD_PC *)builder->pad)->bound_pad, buzz_frames, 0);
                    if (buildit->build_state == 0 &&
                        ((((GAMEPAD_PC *)builder->pad)->buttons_held & GAMEPAD_SPECIAL) != 0 ||
                         (builder->flags_1f8 & 0x40000) != 0)) {
                        builder->flags_e20 ^= 0x4000;
                        if (builder->big_jump_animation < 10)
                            ++builder->big_jump_animation;
                    } else {
                        ReleaseBuildIt(builder, buildit->build_state);
                    }
                }
            } else {
                GAMEANIMOBJ *piece = buildit->anim_objects[buildit->built_object_count];
                if (previous_timer == buildit->reset_step_duration) {
                    NUVEC *position = NuSpecialGetDrawPos(&piece->special);
                    i32 debris = GizBuilditGDeb[qrand() / 0x2aab];
                    AddGameDebris(world->game_debris_sys, debris, position);
                }
                if (automatic)
                    GizMoveAttractoBuildItPiece((GIZBUILDIT_s *)buildit, piece);
                else if (buildit->linked_buildit == NULL)
                    NuSpecialSetVisibility(&piece->special, 0);
            }
        }
    idle_wobble:
        if (buildit->build_state != 0 || (buildit->state_flags & 0x40) != 0 ||
            ((buildit->state_flags & 0x80) != 0 && ShadowMode == 0))
            continue;
        {
            volatile f32 probability =
                (f64)0.1f - (buildit->anim_object_count - buildit->built_object_count) * (f64)0.002f;
            i32 piece_index;
            if (probability < 0.01f)
                probability = 0.01f;
            if ((buildit->availability_flags & 4) == 0)
                probability = (f64)probability * (f64)0.1f;
            for (piece_index = 0; piece_index < buildit->anim_object_count; ++piece_index) {
                GAMEANIMOBJ *piece;
                GIZBUILDITANIMDATA_s *data;
                if (buildit->linked_buildit != NULL && piece_index < buildit->built_object_count)
                    continue;
                piece = buildit->anim_objects[piece_index];
                data = (GIZBUILDITANIMDATA_s *)piece->object_data;
                if (buildit->linked_buildit != NULL)
                    matrix = data->start_mtx;
                else
                    matrix = *NuSpecialGetMtx(&piece->special);
                if (data->wobble_time > 0.0f) {
                    volatile f32 wobble_time = data->wobble_time - FRAMETIME;
                    data->wobble_time = wobble_time;
                    if (wobble_time <= 0.0f && BuilditHopSfxWaitPc <= 0.0f &&
                        ((buildit->availability_flags & 4) != 0 || qrand() <= 0x7fff)) {
                        GameAudio_PlaySfx(0x3c, &buildit->start_position, 0, 0);
                        BuilditHopSfxWaitPc = (f64)qrand() * 1.5259021893143654e-05 * (f64)0.2f + (f64)0.1f;
                    }
                } else if (piece_index >= buildit->built_object_count &&
                           (buildit->linked_buildit != NULL ? data->was_drawn != 0
                                                            : NuSpecialGetOnScreenFn(&piece->special) != 0)) {
                    if ((f64)probability > (f64)qrand() * 1.5259021893143654e-05) {
                        data->wobble_time = 0.2f;
                        data->wobble_axis = (u8)(qrand() / 0x2aab);
                    }
                }
                if (data->wobble_time > 0.0f) {
                    volatile f32 phase = (f64)data->wobble_time / (f64)0.2f;
                    f32 sine = NU_SIN_LUT((1.0f - phase) * 65536.0);
                    i32 angle = (i32)((f64)sine * 3640.0);
                    f32 factor;
                    if ((data->wobble_axis & 1) != 0)
                        angle = -angle;
                    if (data->wobble_axis <= 1) {
                        NuMtxGetXAxis(&matrix, &axis);
                        factor = BuildItAxisWobbleFactorPc(&axis);
                        NuMtxPreRotateX(&matrix, (i32)(angle * factor));
                    } else if (data->wobble_axis <= 3) {
                        NuMtxGetYAxis(&matrix, &axis);
                        factor = BuildItAxisWobbleFactorPc(&axis);
                        NuMtxPreRotateY(&matrix, (i32)(angle * factor));
                    } else {
                        NuMtxGetZAxis(&matrix, &axis);
                        factor = BuildItAxisWobbleFactorPc(&axis);
                        NuMtxPreRotateZ(&matrix, (i32)(angle * factor));
                    }
                    matrix.m31 += (f64)NU_SIN_LUT((f64)phase * 32768.0) * GIZBUILDITWOBBLEJUMPHEIGHT *
                                  buildit->interaction_radius;
                }
                if (buildit->linked_buildit != NULL)
                    data->draw_mtx = matrix;
                else {
                    NuSpecialSetDrawMtx(&piece->special, &matrix);
                    NuSpecialUpdate(&piece->special);
                }
            }
        }
    }
}

STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, anim_set) == 0x10);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, anim_objects) == 0x14);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, linked_buildit) == 0x18);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, start_position) == 0x28);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, reset_step_timer) == 0x44);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, anim_object_count) == 0x6c);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, build_state) == 0x6d);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, state_flags) == 0x72);
STATIC_ASSERT(offsetof(BUILDIT_RESET_PC, availability_flags) == 0x7a);
STATIC_ASSERT(offsetof(WORLDINFO, game_debris_sys) == 0x134);
STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, draw_mtx) == 0x80);
STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, draw_mtx) + offsetof(NUMTX, m30) == 0xb0);
i32 __cdecl GizmoBuildit_GetOutput(GIZMO_PC *gizmo, i32 output_index, i32 force_read) {
    BUILDIT_RESET_PC *buildit = (BUILDIT_RESET_PC *)gizmo->object;
    if (((buildit->availability_flags & 3) == 3 || force_read != 0) && buildit->build_state == 2)
        return 1;
    return 0;
}
void __cdecl GizmoBuildit_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    BUILDIT_RESET_PC *buildit;
    if (gizmo == NULL)
        return;
    buildit = (BUILDIT_RESET_PC *)gizmo->object;
    if (buildit == NULL)
        return;
    GameAnimSet_SetVisibility(buildit->anim_set, visible);
    buildit->availability_flags = (u16)((buildit->availability_flags & ~2u) | (visible != 0 ? 2u : 0u));
}
void __cdecl GizmoBuildit_Activate(GIZMO_PC *gizmo, i32 active) {
    BUILDIT_RESET_PC *buildit;
    NUVEC scatter_position;
    i32 reward_released, index;
    GAMEANIMOBJ *object;
    GIZBUILDITANIMDATA_s *object_data;
    if (gizmo == NULL)
        return;
    buildit = (BUILDIT_RESET_PC *)gizmo->object;
    buildit->availability_flags = (u16)((buildit->availability_flags & ~1u) | (active != 0 ? 1u : 0u));
    if (!(buildit->availability_flags & 1))
        return;
    if (buildit->state_flags & 0x200)
        buildit->availability_flags &= ~0x200u;
    if (buildit->linked_buildit == NULL)
        GameAnimSet_JumpToStart(buildit->anim_set);
    reward_released = (buildit->availability_flags >> 9) & 1;
    GizBuildit_Reset((GIZBUILDIT_s *)buildit, NULL);
    buildit->availability_flags = (u16)((buildit->availability_flags & ~0x200u) | (reward_released != 0 ? 0x200u : 0u));
    if (!(buildit->state_flags & 0x40) || GizBuildit_AutoBuildPosFn == NULL)
        return;
    if (!GizBuildit_AutoBuildPosFn(NULL, &buildit->start_position, &scatter_position, NULL))
        return;
    buildit->reset_step_timer = 5.0f;
    for (index = 0; index < buildit->anim_object_count; ++index) {
        scatter_position.x = (f32)((f64)NuFloatRand((NURAND *)&GAMERAND) / 5.0 - (f64)0.1f + scatter_position.x);
        scatter_position.y = (f32)((f64)NuFloatRand((NURAND *)&GAMERAND) / 5.0 - (f64)0.1f + scatter_position.y);
        scatter_position.z = (f32)((f64)NuFloatRand((NURAND *)&GAMERAND) / 5.0 - (f64)0.1f + scatter_position.z);
        if (buildit->linked_buildit != NULL) {
            object = buildit->anim_objects[index];
            object_data = (GIZBUILDITANIMDATA_s *)object->object_data;
            *(NUVEC *)&object_data->draw_mtx.m30 = scatter_position;
        } else {
            object = buildit->anim_objects[index];
            NuSpecialSetDrawPos((nuhspecial_s *)&object->special, &scatter_position);
        }
        AddGameDebris(WORLD->game_debris_sys, GizBuilditGDeb[qrand() / 0x2aab], &scatter_position);
    }
}
