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
typedef union PUSH_FABS_BITS_PC {
    f32 value;
    u32 bits;
} PUSH_FABS_BITS_PC;
extern "C" PUSH_BLOCK_PC *__cdecl NearestFacingPushBlock(WORLDINFO *world, GAMEOBJECT *object, float range) {
    PUSH_BLOCK_WORLD_PC *push_world = (PUSH_BLOCK_WORLD_PC *)world;
    if (push_world == NULL || push_world->blocks == NULL || push_world->count == 0 || LEGOCONTEXT_PUSH == -1 ||
        object->character_context != LEGOCONTEXT_PUSH)
        return NULL;
    f32 max_y = object->upper_position.y;
    u16 facing = object->movement_facing_angle;
    f32 min_y = object->lower_position.y;
    f32 adjust = max_y - min_y;
    adjust *= 0.25;
    max_y -= adjust;
    min_y += adjust;
    i32 mode;
    PUSH_BLOCK_PC *nearest = NULL;
    if (facing == 0 || facing > 0x10000 - PushBlockFacingTolerancePc ||
        (facing > 0 && facing < PushBlockFacingTolerancePc))
        mode = 2;
    else if (facing == 0x4000 ||
             (facing > 0x4000 - PushBlockFacingTolerancePc && facing < 0x4000 + PushBlockFacingTolerancePc))
        mode = 0;
    else if (facing == 0x8000 ||
             (facing > 0x8000 - PushBlockFacingTolerancePc && facing < 0x8000 + PushBlockFacingTolerancePc))
        mode = 3;
    else if (facing == 0xc000 ||
             (facing > 0xc000 - PushBlockFacingTolerancePc && facing < 0xc000 + PushBlockFacingTolerancePc))
        mode = 1;
    else
        return NULL;
    f32 nearest_dist = 1000000000.0f;
    nearest = NULL;
    f32 gap = 0.0f;
    for (i32 i = 0; i < push_world->count; ++i) {
        PUSH_BLOCK_PC *block = &push_world->blocks[i];
        if (NuSpecialExistsFn(&block->special) != 0) {
            if (NuSpecialGetOnScreenFn(&block->special) == 0)
                continue;
            if (NuSpecialGetVisibilityFn(&block->special) == 0)
                continue;
        }
        if ((block->packed_state_flags & 0x6000100) != 0)
            continue;
        NUVEC bmin = block->bounds_min;
        NUVEC bmax = block->bounds_max;
        PUSH_FABS_BITS_PC absolute, value;
        value.value = bmax.y;
        absolute.bits = value.bits & 0x7fffffff;
        f32 top = block->position->y + absolute.value;
        PUSH_FABS_BITS_PC absolute2, value2;
        value2.value = bmin.y;
        absolute2.bits = value2.bits & 0x7fffffff;
        f32 bottom = block->position->y - absolute2.value;
        if (top > max_y && bottom > max_y)
            continue;
        if (min_y > top && min_y > bottom)
            continue;
        NUVEC pos = *block->position;
        f32 dx = pos.x - object->collision_position.x;
        NUVEC delta;
        delta.x = dx;
        delta.y = pos.y - object->collision_position.y;
        f32 dz = pos.z - object->collision_position.z;
        delta.z = dz;
        switch (mode) {
            case 0: {
                PUSH_FABS_BITS_PC a, v;
                f32 bound;
                v.value = bmin.x;
                a.bits = v.bits & 0x7fffffff;
                gap = dx - a.value;
                bound = bmin.z + pos.z - PushBlockGapEpsilonPc;
                if (object->collision_position.z <= bound)
                    continue;
                bound = pos.z + bmax.z - PushBlockGapEpsilonPc;
                if (object->collision_position.z >= bound)
                    continue;
                if ((block->packed_state_flags & 0x200000) != 0)
                    continue;
                if ((block->packed_state_flags & 0x100000) != 0 && (block->packed_state_flags & 0x800000) != 0)
                    continue;
                break;
            }
            case 1: {
                PUSH_FABS_BITS_PC a, v;
                f32 bound;
                v.value = dx;
                a.bits = v.bits & 0x7fffffff;
                gap = a.value - bmax.x;
                bound = bmax.z + pos.z - PushBlockGapEpsilonPc;
                if (object->collision_position.z >= bound)
                    continue;
                bound = pos.z + bmin.z - PushBlockGapEpsilonPc;
                if (object->collision_position.z <= bound)
                    continue;
                if ((block->packed_state_flags & 0x200000) != 0)
                    continue;
                if ((block->packed_state_flags & 0x100000) != 0 && (block->packed_state_flags & 0x400000) != 0)
                    continue;
                break;
            }
            case 2: {
                PUSH_FABS_BITS_PC a, v;
                f32 bound;
                v.value = bmin.z;
                a.bits = v.bits & 0x7fffffff;
                gap = dz - a.value;
                bound = bmax.x + pos.x - PushBlockGapEpsilonPc;
                if (object->collision_position.x >= bound)
                    continue;
                bound = pos.x + bmin.x - PushBlockGapEpsilonPc;
                if (object->collision_position.x <= bound)
                    continue;
                if ((block->packed_state_flags & 0x100000) != 0)
                    continue;
                if ((block->packed_state_flags & 0x200000) != 0 && (block->packed_state_flags & 0x800000) != 0)
                    continue;
                break;
            }
            case 3: {
                PUSH_FABS_BITS_PC a, v;
                f32 bound;
                v.value = dz;
                a.bits = v.bits & 0x7fffffff;
                gap = a.value - bmax.z;
                bound = bmin.x + pos.x - PushBlockGapEpsilonPc;
                if (object->collision_position.x <= bound)
                    continue;
                bound = pos.x + bmax.x - PushBlockGapEpsilonPc;
                if (object->collision_position.x >= bound)
                    continue;
                if ((block->packed_state_flags & 0x100000) != 0)
                    continue;
                if ((block->packed_state_flags & 0x200000) != 0 && (block->packed_state_flags & 0x400000) != 0)
                    continue;
                break;
            }
        }
        if (gap > (object->bounds_radius + PushBlockGapEpsilonPc) * 2.0f)
            continue;
        u16 yaw = NuAtan2D(dx, dz);
        NuVecRotateY(&delta, &delta, 0x4000 - yaw);
        u16 pitch = NuAtan2D(delta.x, delta.y);
        i32 pitch_diff = 0x4000 - pitch;
        u16 pitch_abs = (u16)abs(pitch_diff);
        i32 yaw_diff = (i16)(yaw - object->movement_facing_angle);
        u16 yaw_abs = (u16)abs(yaw_diff);
        if (yaw_abs > 0x2000)
            continue;
        if (pitch_abs > 0x4000)
            continue;
        gap = dx * dx + dz * dz;
        if (gap < range && gap < nearest_dist) {
            nearest_dist = gap;
            nearest = block;
        }
    }
    return nearest;
}
extern "C" i32 __cdecl PushBlock(GAMEOBJECT *object) {
    PUSH_BLOCK_PC *block = NearestFacingPushBlock(WORLD, object, 2.0f);
    if (block != NULL) {
        block->pushing_object = object;
        block->packed_state_flags |= 1;
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
    union {
        u32 packed_state_flags;
        struct {
            u32 unknown_flags_000_011 : 12;
            u32 animation_positions : 3;
            u32 unknown_flags_015_017 : 3;
            u32 push_visible : 1;
            u32 unknown_flags_019_024 : 6;
            u32 progress_state : 1;
            u32 unknown_flags_026_031 : 6;
        };
    };
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
    union {
        u32 packed_state_flags;
        struct {
            u32 unknown_flags_000_011 : 12;
            u32 animation_positions : 3;
            u32 unknown_flags_015_017 : 3;
            u32 push_visible : 1;
            u32 unknown_flags_019_024 : 6;
            u32 progress_state : 1;
            u32 unknown_flags_026_031 : 6;
        };
    };
    u8 unknown_0cc[4];
    u8 completed_snap_positions, end_position_count, output_count, unknown_0d3;
} PUSH_RUNTIME_BLOCK_VIEW_PC;
typedef struct PUSH_RUNTIME_LEVEL_VIEW_PC {
    u8 unknown_000[0xff];
    u8 max_push_blocks;
    u8 max_push_block_positions;
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
    NUVEC *positions;
    i32 position_count;
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

// Full PC runtime views; the Android/shared pushblock_s has a shorter tail.
STATIC_ASSERT(offsetof(PUSH_RUNTIME_LEVEL_VIEW_PC, max_push_block_positions) == 0x100);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, positions) == 0x46c4);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_WORLD_VIEW_PC, position_count) == 0x46c8);
typedef struct PUSH_RESET_PROGRESS_PC {
    u32 visible_mask[1], state_mask[1], position_mask[1];
    NUVEC positions[16];
    NUVEC end_positions[2][16];
} PUSH_RESET_PROGRESS_PC;
STATIC_ASSERT(sizeof(PUSH_RESET_PROGRESS_PC) == 0x24c);
STATIC_ASSERT(offsetof(PUSH_RESET_PROGRESS_PC, positions) == 0xc);
STATIC_ASSERT(offsetof(PUSH_RESET_PROGRESS_PC, end_positions) == 0xcc);
extern "C" {
    extern NUVEC PushBlockMissingSpecialPositionPc;
    extern i32 RunOutOfPosTabSpacePc;
    extern f32 PushInitGroundOffsetPc, PushInitCornerInsetPc;
}

extern "C" void __cdecl ResetSinglePushBlock(WORLDINFO *, struct pushblock_s *opaque, i32) {
    PUSH_RUNTIME_BLOCK_VIEW_PC *block = reinterpret_cast<PUSH_RUNTIME_BLOCK_VIEW_PC *>(opaque);
    block->platform_id = FindPlatInst(NuSpecialGetInstanceix(&block->special));
    SetAnimFrame(reinterpret_cast<struct nuhspecial_s *>(&block->special), 1.0f);
    block->packed_state_flags &= 0xfdfc70f2u;
    block->pushing_object = NULL;
    block->completed_snap_positions = 0;
    if (NuSpecialExistsFn(&block->special) != 0) {
        NuSpecialGetBounds(&block->special, &block->bounds_min, &block->bounds_max);
        block->position = reinterpret_cast<NUVEC *>(&NuSpecialGetInstanceMtx(&block->special)->m30);
        if (block->animation_positions != 0 && RunOutOfPosTabSpacePc == 0)
            *block->position = block->snap_positions[0];
    } else {
        // Retail uses this distinct mutable vector, not the canonical v000.
        block->bounds_min = PushBlockMissingSpecialPositionPc;
        block->bounds_max = PushBlockMissingSpecialPositionPc;
        block->position = &PushBlockMissingSpecialPositionPc;
    }
}

extern "C" void __cdecl ResetPushProgress(WORLDINFO *opaque, void *progress_data) {
    PUSH_RUNTIME_WORLD_VIEW_PC *world = reinterpret_cast<PUSH_RUNTIME_WORLD_VIEW_PC *>(opaque);
    PUSH_RUNTIME_BLOCK_VIEW_PC *block = world->blocks;
    if (block == NULL || world->count <= 0)
        return;
    PUSH_RESET_PROGRESS_PC *progress = static_cast<PUSH_RESET_PROGRESS_PC *>(progress_data);
    for (i32 index = 0; index < world->count; ++index, ++block) {
        if (progress != NULL && index < 16) {
            const u32 bit = 1u << (index & 31);
            block->progress_state = (progress->state_mask[index / 32] & bit) != 0;
            block->push_visible = (progress->visible_mask[index / 32] & bit) != 0;
            if (block->push_visible == 0) {
                NuSpecialSetVisibility(&block->special, 0);
                block->push_visible = 0;
                for (i32 output = 0; output < block->end_position_count; ++output)
                    NuSpecialSetVisibility(&block->end_position_specials[output], 0);
            }
            if ((progress->position_mask[index / 32] & bit) == 0)
                continue;
            void *animation = NuSpecialGetInstAnim(&block->special);
            if (block->push_visible == 0 && animation != NULL) {
                NUMTX evaluated;
                NUMTX *matrix = NuSpecialGetInstanceMtx(&block->special);
                EvalAnim(&block->special, 1.0f, &evaluated, 0);
                *reinterpret_cast<NUVEC *>(&matrix->m30) = *reinterpret_cast<NUVEC *>(&evaluated.m30);
                NuSpecialUpdate(&block->special);
                for (i32 output = 0; output < block->end_position_count; ++output) {
                    NUSPECIALHANDLE *special = &block->end_position_specials[output];
                    if (special != NULL) {
                        matrix = NuSpecialGetInstanceMtx(special);
                        EvalAnim(special, 1.0f, &evaluated, 0);
                        *reinterpret_cast<NUVEC *>(&matrix->m30) = *reinterpret_cast<NUVEC *>(&evaluated.m30);
                        NuSpecialUpdate(special);
                    }
                }
            } else {
                NUMTX *matrix = NuSpecialGetInstanceMtx(&block->special);
                *reinterpret_cast<NUVEC *>(&matrix->m30) = progress->positions[index];
                NuSpecialUpdate(&block->special);
                for (i32 output = 0; output < block->end_position_count; ++output) {
                    NUSPECIALHANDLE *special = &block->end_position_specials[output];
                    if (special != NULL) {
                        matrix = NuSpecialGetInstanceMtx(special);
                        *reinterpret_cast<NUVEC *>(&matrix->m30) = progress->end_positions[output][index];
                        NuSpecialUpdate(special);
                    }
                }
            }
        }
    }
}

extern "C" void __cdecl ResetSinglePushBlockHeight(WORLDINFO *world, struct pushblock_s *opaque, i32 index) {
    PUSH_RUNTIME_BLOCK_VIEW_PC *block = reinterpret_cast<PUSH_RUNTIME_BLOCK_VIEW_PC *>(opaque);
    BlockInBlock(world, reinterpret_cast<PUSH_BLOCK_PC *>(block), index, &block->supporting_block);
    if (block->supporting_block != NULL) {
        PUSH_RUNTIME_BLOCK_VIEW_PC *support = reinterpret_cast<PUSH_RUNTIME_BLOCK_VIEW_PC *>(block->supporting_block);
        block->ground_height = support->position->y + support->bounds_max.y + PushBlockGapEpsilonPc;
    }
    f32 height = block->bounds_min.y + block->position->y;
    height -= block->ground_height;
    block->ground_offset = height;
    if (height > (f64)PushBlockGapEpsilonPc + 0.0) {
        block->position->y -= height;
        block->ground_offset = 0.0f;
    }
}

extern "C" void __cdecl GizmoPushBlockInitAndReset(WORLDINFO *opaque, void *progress) {
    PUSH_RUNTIME_WORLD_VIEW_PC *world = reinterpret_cast<PUSH_RUNTIME_WORLD_VIEW_PC *>(opaque);
    world->position_count = 0;
    RunOutOfPosTabSpacePc = 0;
    pushposincrease = 0;
    world->positions = static_cast<NUVEC *>(world->giz_buffer.void_ptr);
    world->giz_buffer.addr =
        (world->giz_buffer.addr + world->current_level->max_push_block_positions * sizeof(NUVEC) + 3) & ~3u;
    PUSH_RUNTIME_BLOCK_VIEW_PC *block = world->blocks;
    ResetPushProgress(opaque, progress);
    for (i32 index = 0; index < world->count; ++index, ++block) {
        block->animation_positions = 0;
        NUINSTANIM_TIME_VIEW *animation = static_cast<NUINSTANIM_TIME_VIEW *>(NuSpecialGetInstAnim(&block->special));
        if (animation != NULL) {
            block->animation_positions = static_cast<i32>(NuAnimEndFrameOld(
                static_cast<NUGSCN *>(block->special.scene)->instance_animation_data[animation->animation_index]));
            block->output_count = static_cast<u8>(block->animation_positions);
            i32 required = world->position_count + block->animation_positions;
            if (required > world->current_level->max_push_block_positions) {
                if (RunOutOfPosTabSpacePc != 0)
                    pushposincrease += block->animation_positions;
                else {
                    RunOutOfPosTabSpacePc = 1;
                    pushposincrease += required - world->current_level->max_push_block_positions;
                }
            }
            if (block->animation_positions != 0 && RunOutOfPosTabSpacePc == 0) {
                block->snap_positions = &world->positions[world->position_count];
                f32 frame = 1.0f;
                for (i32 position = 0; position < static_cast<i32>(block->animation_positions);
                     ++position, frame += 1.0) {
                    NUMTX evaluated;
                    EvalAnim(&block->special, frame, &evaluated, 0);
                    block->snap_positions[position] = *reinterpret_cast<NUVEC *>(&evaluated.m30);
                    evaluated = *NuSpecialGetMtx(&block->special);
                    block->snap_positions[position].x += evaluated.m30;
                    block->snap_positions[position].y += evaluated.m31;
                    block->snap_positions[position].z += evaluated.m32;
                    ++world->position_count;
                }
            }
        }
        ResetSinglePushBlock(opaque, reinterpret_cast<struct pushblock_s *>(block), index);
    }
    block = world->blocks;
    for (i32 index = 0; index < world->count; ++index, ++block) {
        NUVEC centre = *block->position;
        block->velocity = v000;
        block->target_velocity = v000;
        block->snap_origin = centre;
        f32 left = static_cast<f32>(fabs(static_cast<f64>(block->bounds_min.x))) + PushInitCornerInsetPc;
        f32 right = static_cast<f32>(fabs(static_cast<f64>(block->bounds_max.x))) + PushInitCornerInsetPc;
        f32 back = static_cast<f32>(fabs(static_cast<f64>(block->bounds_min.z))) + PushInitCornerInsetPc;
        f32 front = static_cast<f32>(fabs(static_cast<f64>(block->bounds_max.z))) + PushInitCornerInsetPc;
        f32 base_y = centre.y - static_cast<f32>(fabs(static_cast<f64>(block->bounds_min.y))) - PushInitGroundOffsetPc;
        f32 y = base_y + static_cast<f64>(0.025f);
        NUVEC corners[4] = {centre, centre, centre, centre};
        corners[0].y = corners[1].y = corners[2].y = corners[3].y = y;
        corners[0].x -= left;
        corners[0].z -= back;
        corners[1].x += right;
        corners[1].z -= back;
        corners[2].x += right;
        corners[2].z += front;
        corners[3].x -= left;
        corners[3].z += front;
        PlatOnOff(block->platform_id, 0);
        NewTerrPlatformsOff();
        f32 height0 = GameShadow(NULL, &corners[0], 0.1f, -1);
        block->terrain_info[0] = static_cast<i8>(ShadowInfo());
        block->extra_terrain_info.bytes[0] = static_cast<i8>(EShadowInfo());
        NewTerrPlatformsOff();
        f32 height1 = GameShadow(NULL, &corners[1], 0.1f, -1);
        block->terrain_info[1] = static_cast<i8>(ShadowInfo());
        block->extra_terrain_info.bytes[1] = static_cast<i8>(EShadowInfo());
        NewTerrPlatformsOff();
        f32 height2 = GameShadow(NULL, &corners[2], 0.1f, -1);
        block->terrain_info[2] = static_cast<i8>(ShadowInfo());
        block->extra_terrain_info.bytes[2] = static_cast<i8>(EShadowInfo());
        NewTerrPlatformsOff();
        f32 height3 = GameShadow(NULL, &corners[3], 0.1f, -1);
        block->terrain_info[3] = static_cast<i8>(ShadowInfo());
        block->extra_terrain_info.bytes[3] = static_cast<i8>(EShadowInfo());
        PlatOnOff(block->platform_id, 1);
        if (height1 == height0 && height0 == height3 && height3 == height2) {
            NewTerrPlatformsOff();
            centre.y = GameShadow(NULL, &centre, 5.0f, -1);
            if (centre.y == 2000000.0)
                centre.y = 0.0f;
            block->ground_height = centre.y;
        } else
            block->ground_height = (height3 + (height1 + height0 + height2)) * 0.25;
        // PC has no Android TerrainBlockOnBlock call here.
        block->previous_extra_terrain_info = block->extra_terrain_info.word;
    }
    for (i32 index = 0; index < world->count; ++index)
        ResetSinglePushBlockHeight(opaque, reinterpret_cast<struct pushblock_s *>(&world->blocks[index]), index);
}
