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
    u8 unknown_000[0x10];
    GAMEANIMSET *anim_set;
    GAMEANIMOBJ **anim_objects;
    void *linked_buildit;
    u8 unknown_01c[0x44 - 0x1c];
    f32 reset_step_timer, reset_step_duration;
    u8 unknown_04c[0x6c - 0x4c];
    u8 anim_object_count, build_state, builders_active, built_object_count;
    u8 unknown_070[0xa];
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
