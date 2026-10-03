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
