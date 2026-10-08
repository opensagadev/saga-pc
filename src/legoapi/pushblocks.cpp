#include "saga.h"

typedef struct PUSH_BLOCK_PC {
    u8 unknown_000[0x24];
    NUSPECIALHANDLE special;
    GAMEOBJECT *pushing_object;
    u8 unknown_034[0x58 - 0x34];
    NUVEC *position;
    u8 unknown_05c[0xb0 - 0x5c];
    NUVEC bounds_min, bounds_max;
    union {
        u32 packed_state_flags;
        struct {
            u8 runtime_flags_0c8, runtime_flags_0c9, flags_0ca, flags_0cb;
        };
    };
    u8 unknown_0cc[0xd4 - 0xcc];
} PUSH_BLOCK_PC;
typedef struct PUSH_BLOCK_WORLD_PC {
    u8 unknown_000[0x46bc];
    PUSH_BLOCK_PC *blocks;
    i32 count;
} PUSH_BLOCK_WORLD_PC;
STATIC_ASSERT(sizeof(PUSH_BLOCK_PC) == 0xd4);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, special) == 0x24);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, pushing_object) == 0x30);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, position) == 0x58);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, bounds_min) == 0xb0);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, bounds_max) == 0xbc);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, packed_state_flags) == 0xc8);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, runtime_flags_0c8) == 0xc8);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, flags_0ca) == 0xca);
STATIC_ASSERT(offsetof(PUSH_BLOCK_WORLD_PC, blocks) == 0x46bc);
STATIC_ASSERT(offsetof(PUSH_BLOCK_WORLD_PC, count) == 0x46c0);
extern "C" {
    extern i32 LEGOCONTEXT_PUSH;
    extern u16 PushBlockFacingTolerancePc;
    extern f32 PushBlockGapEpsilonPc;
}
extern "C" PUSH_BLOCK_PC *__cdecl NearestFacingPushBlock(WORLDINFO *world, GAMEOBJECT *object, float range) {
    PUSH_BLOCK_PC *nearest = NULL;
    if (world == NULL)
        return nearest;
    i32 count = ((PUSH_BLOCK_WORLD_PC *)world)->count;
    PUSH_BLOCK_PC *blocks = ((PUSH_BLOCK_WORLD_PC *)world)->blocks;
    if (blocks == NULL)
        return nearest;
    if (count == 0)
        return nearest;
    if (LEGOCONTEXT_PUSH == -1)
        return nearest;
    if (object->character_context != LEGOCONTEXT_PUSH)
        return nearest;
    f32 upper = object->upper_position.y;
    u16 facing = object->movement_facing_angle;
    f32 lower = object->lower_position.y;
    f32 height = upper - lower;
    f32 adjust = height * 0.25f;
    f32 max_y = upper - adjust;
    f32 min_y = lower + adjust;
    i32 mode;
    i32 angle = facing;
    i32 tolerance = PushBlockFacingTolerancePc;
    if (facing == 0 || angle > 0x10000 - tolerance || (facing > 0 && facing < PushBlockFacingTolerancePc))
        mode = 2;
    else if (facing == 0x4000 || (angle > 0x4000 - tolerance && angle < 0x4000 + tolerance))
        mode = 0;
    else if (facing == 0x8000 || (angle > 0x8000 - tolerance && angle < 0x8000 + tolerance))
        mode = 3;
    else if (facing == 0xc000 || (angle > 0xc000 - tolerance && angle < 0xc000 + tolerance))
        mode = 1;
    else
        return nearest;
    nearest = NULL;
    if (((PUSH_BLOCK_WORLD_PC *)world)->count <= 0)
        return nearest;
    f32 nearest_dist = 1000000000.0f;
    for (i32 i = 0; i < ((PUSH_BLOCK_WORLD_PC *)world)->count; ++i) {
        PUSH_BLOCK_PC *block = &((PUSH_BLOCK_WORLD_PC *)world)->blocks[i];
        if (NuSpecialExistsFn(&block->special) != 0) {
            if (NuSpecialGetOnScreenFn(&block->special) == 0)
                continue;
            if (NuSpecialGetVisibilityFn(&block->special) == 0)
                continue;
        }
        if ((block->packed_state_flags & 0x6000100) != 0)
            continue;
        f32 max_x = block->bounds_max.x;
        NUVEC *pos = block->position;
        f32 top = pos->y + (f32)fabs((f64)block->bounds_max.y);
        f32 bottom = pos->y - (f32)fabs((f64)block->bounds_min.y);
        f32 min_x = block->bounds_min.x;
        f32 min_z = block->bounds_min.z;
        f32 max_z = block->bounds_max.z;
        if (top > max_y && bottom > max_y)
            continue;
        if (min_y > top) {
            if (min_y > bottom)
                continue;
        }
        f32 dx = pos->x - object->collision_position.x;
        f32 dy = pos->y - object->collision_position.y;
        f32 dz = pos->z - object->collision_position.z;
        NUVEC delta;
        delta.x = dx;
        delta.y = dy;
        delta.z = dz;
        f32 gap;
        if (mode == 2) {
            goto mode2_geometry;
        }
        if (mode == 3) {
            goto mode3_geometry;
        }
        if (mode == 1) {
            goto mode1_geometry;
        }
    mode0_geometry: {
        f32 block_min_z = min_z + pos->z - PushBlockGapEpsilonPc;
        f32 block_max_z = pos->z + max_z - PushBlockGapEpsilonPc;
        if (block_min_z >= object->collision_position.z)
            continue;
        if (object->collision_position.z >= block_max_z)
            continue;
        if ((block->flags_0ca & 0x20) != 0)
            continue;
        if ((block->flags_0ca & 0x90) == 0x90)
            continue;
        gap = dx - (f32)fabs((f64)min_x);
        goto have_gap;
    }
    mode1_geometry: {
        f32 block_max_z = max_z + pos->z - PushBlockGapEpsilonPc;
        f32 block_min_z = pos->z + min_z - PushBlockGapEpsilonPc;
        if (object->collision_position.z >= block_max_z)
            continue;
        if (block_min_z >= object->collision_position.z)
            continue;
        if ((block->flags_0ca & 0x20) != 0)
            continue;
        if ((block->flags_0ca & 0x50) == 0x50)
            continue;
        gap = (f32)fabs((f64)dx) - max_x;
        goto have_gap;
    }
    mode3_geometry: {
        f32 block_min_x = min_x + pos->x - PushBlockGapEpsilonPc;
        f32 block_max_x = pos->x + max_x - PushBlockGapEpsilonPc;
        if (block_min_x >= object->collision_position.x)
            continue;
        if (object->collision_position.x >= block_max_x)
            continue;
        if ((block->flags_0ca & 0x10) != 0)
            continue;
        if ((block->flags_0ca & 0x60) == 0x60)
            continue;
        gap = (f32)fabs((f64)dz) - max_z;
        goto have_gap;
    }
    mode2_geometry: {
        f32 block_max_x = max_x + pos->x - PushBlockGapEpsilonPc;
        f32 block_min_x = pos->x + min_x - PushBlockGapEpsilonPc;
        if (object->collision_position.x >= block_max_x)
            continue;
        if (block_min_x >= object->collision_position.x)
            continue;
        if ((block->flags_0ca & 0x10) != 0)
            continue;
        if ((block->flags_0ca & 0xa0) == 0xa0)
            continue;
        gap = dz - (f32)fabs((f64)min_z);
        goto have_gap;
    }
    have_gap: {
        if (gap > (object->bounds_radius + PushBlockGapEpsilonPc) * 2.0f)
            continue;
        i32 yaw = NuAtan2D(delta.x, delta.z);
        i32 rot = 0x4000 - (yaw & 0xffff);
        NuVecRotateY(&delta, &delta, rot);
        i32 pitch = NuAtan2D(delta.x, delta.y);
        i16 yaw_diff = (i16)(yaw - object->movement_facing_angle);
        i32 yaw_abs = yaw_diff;
        yaw_abs = yaw_abs < 0 ? -yaw_abs : yaw_abs;
        if ((u16)yaw_abs > 0x2000)
            continue;
        i32 pitch_diff = 0x4000 - (u16)pitch;
        pitch_diff = pitch_diff < 0 ? -pitch_diff : pitch_diff;
        if ((u16)pitch_diff > 0x4000)
            continue;
        f32 dist2 = dx * dx + dz * dz;
        if (!(range > dist2))
            continue;
        if (!(nearest_dist > dist2))
            continue;
        nearest_dist = dist2;
        nearest = block;
    }
    }
    return nearest;
}
extern "C" i32 __cdecl PushBlock(GAMEOBJECT *object) {
    PUSH_BLOCK_PC *block = NearestFacingPushBlock(WORLD, object, 2.0f);
    if (block != NULL) {
        block->pushing_object = object;
        block->runtime_flags_0c8 |= 1;
        AlertSurroundingCreatures(object, &object->collision_position);
        return 1;
    }
    object->character_context = -1;
    return 0;
}

typedef struct PUSH_SUPPORT_BLOCK_VIEW_PC {
    u8 unknown_000[0x24];
    NUSPECIALHANDLE special;
    GAMEOBJECT *pushing_object;
    u8 unknown_034[0x58 - 0x34];
    NUVEC *position;
    u8 unknown_05c[0xb0 - 0x5c];
    NUVEC bounds_min, bounds_max;
    u32 packed_state_flags;
    u8 unknown_0cc[4];
    u8 completed_snap_positions;
    u8 end_position_count;
    u8 output_count;
    u8 unknown_0d3;
} PUSH_SUPPORT_BLOCK_VIEW_PC;
typedef struct PUSH_SUPPORT_WORLD_VIEW_PC {
    u8 unknown_000[0x46bc];
    PUSH_SUPPORT_BLOCK_VIEW_PC *blocks;
    i32 count;
} PUSH_SUPPORT_WORLD_VIEW_PC;
STATIC_ASSERT(sizeof(PUSH_SUPPORT_BLOCK_VIEW_PC) == 0xd4);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, special) == 0x24);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, pushing_object) == 0x30);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, position) == 0x58);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, bounds_min) == 0xb0);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, bounds_max) == 0xbc);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, packed_state_flags) == 0xc8);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, completed_snap_positions) == 0xd0);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, end_position_count) == 0xd1);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, output_count) == 0xd2);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_WORLD_VIEW_PC, blocks) == 0x46bc);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_WORLD_VIEW_PC, count) == 0x46c0);

extern "C" void __cdecl PushSeekComplete(PUSH_BLOCK_PC *opaque_block, i32 index) {
    PUSH_SUPPORT_BLOCK_VIEW_PC *block = (PUSH_SUPPORT_BLOCK_VIEW_PC *)opaque_block;
    block->packed_state_flags &= ~0x200u;
    block->completed_snap_positions |= (u8)(1u << (index & 31));
    if ((block->packed_state_flags & 0x20000000u) == 0)
        block->packed_state_flags |= 0x02000100u;
    if (block->pushing_object != NULL)
        NewBuzz(block->pushing_object->pad->bound_pad, 0.1f, 0);
}

extern "C" i32 __cdecl OtherBlockInRange(WORLDINFO *opaque_world, PUSH_BLOCK_PC *opaque_block, NUVEC *position,
                                         i32 excluded) {
    PUSH_SUPPORT_WORLD_VIEW_PC *world = (PUSH_SUPPORT_WORLD_VIEW_PC *)opaque_world;
    PUSH_SUPPORT_BLOCK_VIEW_PC *block = (PUSH_SUPPORT_BLOCK_VIEW_PC *)opaque_block;
    NUVEC centre;
    f32 radius;
    NuSpecialGetRadius(&block->special, &centre, &radius);
    radius *= radius;
    for (i32 i = 0; i < world->count; ++i) {
        PUSH_SUPPORT_BLOCK_VIEW_PC *other;
        NUVEC minimum, maximum;
        f32 other_radius, x, z, sum, distance;
        if (i == excluded)
            continue;
        other = &world->blocks[i];
        if ((other->packed_state_flags & 0x01040000u) != 0x01040000u || (other->packed_state_flags & 0x04000000u) != 0)
            continue;
        if (NuSpecialGetVisibilityFn(&other->special) == 0)
            continue;
        NuSpecialGetBounds(&other->special, &minimum, &maximum);
        NuSpecialGetRadius(&other->special, &centre, &other_radius);
        centre = *other->position;
        x = centre.x - position->x;
        z = centre.z - position->z;
        sum = x * x + z * z;
        distance = sum - other_radius * other_radius;
        if (radius >= distance || 0.0f >= distance)
            return 1;
    }
    return 0;
}

extern "C" {
    PUSH_BLOCK_PC *__cdecl BlockInBlock(WORLDINFO *, PUSH_BLOCK_PC *, i32, PUSH_BLOCK_PC **);
    void __cdecl MoveBlocksOverBlock(WORLDINFO *, PUSH_BLOCK_PC *, i32, NUVEC *);
    void __cdecl MoveBlocks(WORLDINFO *, PUSH_BLOCK_PC *, i32, NUVEC *);
    void __cdecl UpdatePushBlocks(void *, void *, f32);
    extern f32 PushVelocitySeekRatePc;
    extern f32 PushFallingSpeedPc;
    extern f32 PushCornerInsetPc;
    extern f32 PushGroundOffsetPc;
    extern f32 PushRumblePeriodPc;
    extern f32 PushRumbleWindowPc;
    extern f32 PushBlockOverlapShrinkPc;
    extern i32 PushBlockHintIdPc;
    void __cdecl KnockPushBlock(PUSH_BLOCK_PC *, NUVEC *);
    PUSH_BLOCK_PC *__cdecl NearestPushBlock(WORLDINFO *, NUVEC *, f32);
    char *__cdecl NuIToA(i32, char *, i32);
    extern char *PushBlocksNamePc;
}

typedef struct PUSH_RUNTIME_BLOCK_VIEW_PC {
    f32 ground_offset, ground_height, snap_timer, settled_height;
    i8 terrain_info[4];
    union {
        i8 bytes[4];
        i32 word;
    } extra_terrain_info;
    i32 previous_extra_terrain_info;
    PUSH_BLOCK_PC *supporting_block, *previous_supporting_block;
    NUSPECIALHANDLE special;
    GAMEOBJECT *pushing_object;
    i32 platform_id;
    u8 unknown_038[8];
    f32 snap_distance;
    char name[16];
    NUVEC *snap_positions, *position;
    NUSPECIALHANDLE end_position_specials[2];
    NUVEC end_position_origins[2];
    NUVEC velocity, target_velocity, snap_origin, bounds_min, bounds_max;
    u32 packed_state_flags;
    u8 unknown_0cc[4];
    u8 completed_snap_positions, end_position_count, output_count, unknown_0d3;
} PUSH_RUNTIME_BLOCK_VIEW_PC;
typedef struct PUSH_RUNTIME_LEVEL_VIEW_PC {
    u8 unknown_000[0xff];
    u8 max_push_blocks;
} PUSH_RUNTIME_LEVEL_VIEW_PC;
typedef struct PUSH_RUNTIME_WORLD_VIEW_PC {
    u8 unknown_000[0x104];
    VARIPTR giz_buffer;
    u8 unknown_108[0x128 - 0x108];
    PUSH_RUNTIME_LEVEL_VIEW_PC *current_level;
    u8 unknown_12c[0x13c - 0x12c];
    NUGSCN *current_gscn;
    u8 unknown_140[0x2ac8 - 0x140];
    void *gizmo_sys;
    u8 unknown_2acc[0x46bc - 0x2acc];
    PUSH_RUNTIME_BLOCK_VIEW_PC *blocks;
    i32 count;
} PUSH_RUNTIME_WORLD_VIEW_PC;
STATIC_ASSERT(sizeof(PUSH_RUNTIME_BLOCK_VIEW_PC) == 0xd4);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_LEVEL_VIEW_PC, max_push_blocks) == 0xff);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, current_level) == 0x128);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, current_gscn) == 0x13c);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, gizmo_sys) == 0x2ac8);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, blocks) == 0x46bc);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, count) == 0x46c0);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, ground_offset) == 0x0);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, ground_height) == 0x4);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, snap_timer) == 0x8);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, settled_height) == 0xc);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, terrain_info) == 0x10);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, extra_terrain_info) == 0x14);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, previous_extra_terrain_info) == 0x18);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, supporting_block) == 0x1c);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, previous_supporting_block) == 0x20);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, special) == 0x24);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, pushing_object) == 0x30);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, platform_id) == 0x34);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, snap_distance) == 0x40);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, name) == 0x44);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, snap_positions) == 0x54);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, position) == 0x58);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, end_position_specials) == 0x5c);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, end_position_origins) == 0x74);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, velocity) == 0x8c);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, target_velocity) == 0x98);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, snap_origin) == 0xa4);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, bounds_min) == 0xb0);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, bounds_max) == 0xbc);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, packed_state_flags) == 0xc8);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, completed_snap_positions) == 0xd0);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, end_position_count) == 0xd1);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_BLOCK_VIEW_PC, output_count) == 0xd2);
STATIC_ASSERT(offsetof(GAMEOBJECT, target_velocity) == 0xf18);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef0) == 0xef0);

extern "C" void __cdecl KnockPushBlock(PUSH_BLOCK_PC *opaque_block, NUVEC *direction) {
    PUSH_RUNTIME_BLOCK_VIEW_PC *block = reinterpret_cast<PUSH_RUNTIME_BLOCK_VIEW_PC *>(opaque_block);
    if (block != NULL) {
        block->velocity.x = -direction->x;
        block->velocity.y = -direction->y;
        block->velocity.z = -direction->z;
        NuVecScale(&block->velocity, &block->velocity, 0.005f);
        block->packed_state_flags |= 0x80u;
    }
}

extern "C" PUSH_BLOCK_PC *__cdecl NearestPushBlock(WORLDINFO *opaque_world, NUVEC *position, f32 range) {
    PUSH_RUNTIME_WORLD_VIEW_PC *world = reinterpret_cast<PUSH_RUNTIME_WORLD_VIEW_PC *>(opaque_world);
    if (world == NULL || position == NULL)
        return NULL;
    NUVEC minimum = {position->x - range, position->y - range, position->z - range};
    NUVEC maximum = {position->x + range, position->y + range, position->z + range};
    PUSH_RUNTIME_BLOCK_VIEW_PC *nearest = NULL;
    f32 nearest_distance = 1000000000.0f;
    PUSH_RUNTIME_BLOCK_VIEW_PC *block = world->blocks;
    for (i32 index = 0; index < world->count; ++index, ++block) {
        if ((block->packed_state_flags & 0x01040100u) != 0x01040000u)
            continue;
        NUVEC *candidate = block->position;
        if (candidate->x < minimum.x || candidate->x > maximum.x || candidate->z < minimum.z ||
            candidate->z > maximum.z || candidate->y < minimum.y || candidate->y > maximum.y)
            continue;
        f32 distance = NuVecDistSqr(position, candidate, NULL);
        if (distance < nearest_distance) {
            nearest_distance = distance;
            nearest = block;
        }
    }
    return reinterpret_cast<PUSH_BLOCK_PC *>(nearest);
}
