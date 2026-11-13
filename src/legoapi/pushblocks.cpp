#include "saga.h"
static f32 PushBlockGapEpsilonPc = 0.00999999978f;

/* Retail unit 0x5a4390-0x5a8070 (legoapi/actions/movement/pushblocks.cpp): definitions in retail address order. */

STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, extra_terrain_info) == 0x14);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, supporting_block) == 0x1c);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, special) == 0x24);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, pushing_object) == 0x30);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, platform_id) == 0x34);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, snap_distance) == 0x40);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, name) == 0x44);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, snap_positions) == 0x54);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, position) == 0x58);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, end_position_specials) == 0x5c);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, end_position_origins) == 0x74);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, velocity) == 0x8c);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, target_velocity) == 0x98);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, snap_origin) == 0xa4);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, bounds_min) == 0xb0);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, bounds_max) == 0xbc);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, packed_state_flags) == 0xc8);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, flags_0ca) == 0xca);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, completed_snap_positions) == 0xd0);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, end_position_count) == 0xd1);
STATIC_ASSERT(offsetof(PUSH_BLOCK_PC, output_count) == 0xd2);
STATIC_ASSERT(offsetof(WORLDINFO, push_blocks) == 0x46bc);
STATIC_ASSERT(offsetof(WORLDINFO, push_block_count) == 0x46c0);
STATIC_ASSERT(offsetof(WORLDINFO, push_block_positions) == 0x46c4);
STATIC_ASSERT(offsetof(WORLDINFO, push_block_position_count) == 0x46c8);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_pushblock_endpos) == 0x100);
STATIC_ASSERT(offsetof(PUSHPROGRESS, end_positions) == 0xcc);
extern "C" {
    extern i32 LEGOCONTEXT_PUSH;
    extern u16 PushBlockFacingTolerancePc;
}
typedef union PUSH_FABS_BITS_PC {
    f32 value;
    u32 bits;
} PUSH_FABS_BITS_PC;
extern "C" {
    void *__cdecl GizmoBufferAlloc(VARIPTR *, VARIPTR *, i32);
    PUSH_BLOCK_PC *__cdecl BlockInBlock(WORLDINFO *, PUSH_BLOCK_PC *, i32, PUSH_BLOCK_PC **);
    void __cdecl MoveBlocks(WORLDINFO *, PUSH_BLOCK_PC *, i32, NUVEC *);
    void __cdecl UpdatePushBlocks(void *, void *, f32);
    extern f32 PushVelocitySeekRatePc;
    extern f32 PushFallingSpeedPc;
    extern f32 PushCornerInsetPc;
    extern f32 PushGroundOffsetPc;
    extern i32 PushBlockHintIdPc;
    void __cdecl KnockPushBlock(PUSH_BLOCK_PC *, NUVEC *);
    char *__cdecl NuIToA(i32, char *, i32);
    extern char *PushBlocksNamePc;
}
extern "C" ADDGIZMOTYPE_PC Default_ADDGIZMOTYPE;
extern "C" char *PushBlocksNamePc;
extern "C" void __cdecl ResetPushProgress(WORLDINFO *, void *);
extern "C" void __cdecl ResetSinglePushBlock(WORLDINFO *, struct pushblock_s *, i32);
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
extern "C" {
    void __cdecl MoveBlocksOverBlock(WORLDINFO *, PUSH_BLOCK_PC *, i32, NUVEC *);
    extern f32 PushRumblePeriodPc;
    extern f32 PushRumbleWindowPc;
}
typedef struct PUSH_RUNTIME_BOUNDS_PC {
    NUVEC min, max;
} PUSH_RUNTIME_BOUNDS_PC;
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
    NUVEC velocity, target_velocity, snap_origin;
    union {
        struct {
            NUVEC bounds_min, bounds_max;
        };
        PUSH_RUNTIME_BOUNDS_PC bounds;
    };
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
extern "C" {
    extern f32 PushBlockOverlapShrinkPc;
    PUSH_BLOCK_PC *__cdecl NearestPushBlock(WORLDINFO *, NUVEC *, f32);
}
STATIC_ASSERT(offsetof(GAMEOBJECT, target_velocity) == 0xf18);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef0) == 0xef0);
STATIC_ASSERT(offsetof(BOLT_PC, owner) == 4);
STATIC_ASSERT(offsetof(BOLT_PC, hit_platform) == 0xfa);
extern "C" {
    extern NUVEC PushBlockMissingSpecialPositionPc;
    extern i32 RunOutOfPosTabSpacePc;
    extern f32 PushInitGroundOffsetPc, PushInitCornerInsetPc;
}

static __inline f32 GizPushFabsPc(f32 value) {
    f32 result;
    *(i32 *)&result = *(i32 *)&value & 0x7fffffff;
    return result;
}

extern "C" void __cdecl KnockPushBlock(PUSH_BLOCK_PC *opaque_block, NUVEC *direction) {
    PUSH_BLOCK_PC *block = reinterpret_cast<PUSH_BLOCK_PC *>(opaque_block);
    if (block != NULL) {
        block->velocity.x = -direction->x;
        block->velocity.y = -direction->y;
        block->velocity.z = -direction->z;
        NuVecScale(&block->velocity, &block->velocity, 0.005f);
        block->packed_state_flags |= 0x80u;
    }
}

extern "C" void __cdecl ResetSinglePushBlock(WORLDINFO *, struct pushblock_s *opaque, i32) {
    PUSH_BLOCK_PC *block = reinterpret_cast<PUSH_BLOCK_PC *>(opaque);
    block->platform_id = FindPlatInst(NuSpecialGetInstanceix(&block->special));
    SetAnimFrame(reinterpret_cast<struct nuhspecial_s *>(&block->special), 1.0f);
    block->packed_state_flags &= 0xfdfc70f2u;
    block->pushing_object = NULL;
    block->completed_snap_positions = 0;
    if (NuSpecialExistsFn(&block->special) != 0) {
        NuSpecialGetBounds(&block->special, &block->bounds_min, &block->bounds_max);
        block->position = reinterpret_cast<NUVEC *>(&NuSpecialGetInstanceMtx(&block->special)->m30);
        if (block->animation_positions > 0 && RunOutOfPosTabSpacePc == 0)
            *block->position = block->snap_positions[0];
    } else {
        // Retail uses this distinct mutable vector, not the canonical v000.
        block->bounds_min = PushBlockMissingSpecialPositionPc;
        block->bounds_max = PushBlockMissingSpecialPositionPc;
        block->position = &PushBlockMissingSpecialPositionPc;
    }
}

extern "C" void __cdecl ResetPushProgress(WORLDINFO *opaque, void *progress_data) {
    WORLDINFO *world = reinterpret_cast<WORLDINFO *>(opaque);
    PUSH_BLOCK_PC *block = world->push_blocks;
    if (block == NULL)
        return;
    PUSHPROGRESS *progress = static_cast<PUSHPROGRESS *>(progress_data);
    for (i32 index = 0; index < world->push_block_count; ++index, ++block) {
        if (progress != NULL && index < 16) {
            const i32 word = index / 32;
            const u32 bit = 1u << (index & 31);
            block->progress_state = (progress->state_mask[word] & bit) != 0;
            block->push_visible = (progress->visible_mask[word] & bit) != 0;
            if (block->push_visible == 0) {
                NuSpecialSetVisibility(&block->special, 0);
                block->push_visible = 0;
                for (i32 output = 0; output < block->end_position_count; ++output)
                    NuSpecialSetVisibility(&block->end_position_specials[output], 0);
            }
            if ((progress->position_mask[word] & bit) == 0)
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

extern "C" PUSH_BLOCK_PC *__cdecl NearestFacingPushBlock(WORLDINFO *world, GAMEOBJECT *object, float range) {
    WORLDINFO *push_world = (WORLDINFO *)world;
    if (push_world == NULL || push_world->push_blocks == NULL || push_world->push_block_count == 0 ||
        LEGOCONTEXT_PUSH == -1 || object->character_context != LEGOCONTEXT_PUSH)
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
    for (i32 i = 0; i < push_world->push_block_count; ++i) {
        PUSH_BLOCK_PC *block = &push_world->push_blocks[i];
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
extern "C" {

    PUSH_BLOCK_PC *__cdecl BlockInBlock(WORLDINFO *opaque_world, PUSH_BLOCK_PC *opaque_block, i32 excluded,
                                        PUSH_BLOCK_PC **support) {
        WORLDINFO *world = (WORLDINFO *)opaque_world;
        PUSH_BLOCK_PC *block = (PUSH_BLOCK_PC *)opaque_block;
        NUVEC *position = block->position;
        i32 i;
        if (support != NULL)
            *support = NULL;
        for (i = 0; i < world->push_block_count; ++i) {
            PUSH_BLOCK_PC *other;
            NUVEC offset, minimum, maximum;
            i32 overlap_x, overlap_y, overlap_z;
            f32 shrink, epsilon;
            if (i == excluded)
                continue;
            other = &world->push_blocks[i];
            if ((other->packed_state_flags & 0x02000000u) != 0)
                continue;
            if (NuSpecialGetVisibilityFn(&other->special) == 0)
                continue;
            offset = *position;
            offset.x -= other->position->x;
            offset.y -= other->position->y;
            offset.z -= other->position->z;
            minimum = other->bounds_min;
            maximum = other->bounds_max;
            minimum.x -= block->bounds_max.x;
            minimum.y -= block->bounds_max.y;
            minimum.z -= block->bounds_max.z;
            maximum.x -= block->bounds_min.x;
            maximum.y -= block->bounds_min.y;
            maximum.z -= block->bounds_min.z;
            shrink = PushBlockOverlapShrinkPc;
            if (shrink > 0.0f) {
                minimum.x += shrink;
                minimum.y += shrink;
                minimum.z += shrink;
                maximum.x -= shrink;
                maximum.y -= shrink;
                maximum.z -= shrink;
            }
            epsilon = PushBlockGapEpsilonPc;
            overlap_x = offset.x > minimum.x + epsilon && offset.x < maximum.x - epsilon;
            overlap_y = offset.y > minimum.y && offset.y < maximum.y;
            overlap_z = offset.z > minimum.z + epsilon && offset.z < maximum.z - epsilon;
            if (support != NULL && overlap_x && overlap_z) {
                f32 gap = offset.y - maximum.y;
                if (gap > 0.0f) {
                    if (*support == NULL) {
                        *support = (PUSH_BLOCK_PC *)other;
                    } else {
                        PUSH_BLOCK_PC *previous = (PUSH_BLOCK_PC *)*support;
                        f32 previous_gap;
                        offset = *position;
                        offset.y -= previous->position->y;
                        maximum = previous->bounds_max;
                        maximum.y -= block->bounds_min.y;
                        if (shrink > 0.0f)
                            maximum.y -= shrink;
                        previous_gap = offset.y - maximum.y;
                        if (previous_gap > gap) {
                            other->supporting_block = (PUSH_BLOCK_PC *)previous;
                            *support = (PUSH_BLOCK_PC *)other;
                        }
                    }
                }
            }
            if (overlap_x && overlap_y && overlap_z)
                return (PUSH_BLOCK_PC *)other;
        }
        return NULL;
    }

    void __cdecl MoveBlocksOverBlock(WORLDINFO *opaque_world, PUSH_BLOCK_PC *opaque_block, i32 excluded, NUVEC *delta) {
        PUSH_RUNTIME_WORLD_VIEW_PC *world = (PUSH_RUNTIME_WORLD_VIEW_PC *)opaque_world;
        PUSH_RUNTIME_BLOCK_VIEW_PC *block = (PUSH_RUNTIME_BLOCK_VIEW_PC *)opaque_block;
        NUVEC *position;
        PUSH_RUNTIME_BOUNDS_PC box;
        i32 index;
        if ((block->packed_state_flags & 0x400u) != 0)
            return;
        position = block->position;
        box = block->bounds;
        box.min.x = position->x + box.min.x;
        box.min.y += position->y;
        box.min.z = position->z + box.min.z;
        box.max.x = position->x + box.max.x;
        box.max.y += position->y;
        box.max.z = position->z + box.max.z;
        for (index = 0; index < world->count; ++index) {
            PUSH_RUNTIME_BLOCK_VIEW_PC *other;
            NUVEC *other_position;
            PUSH_RUNTIME_BOUNDS_PC other_box, test;
            if (index == excluded)
                continue;
            other = &world->blocks[index];
            if ((other->packed_state_flags & 0x04000000u) != 0 || (other->packed_state_flags & 0x00040000u) == 0)
                continue;
            other_position = other->position;
            if (other_position->y <= position->y)
                continue;
            other_box = other->bounds;
            test = box;
            test.min.x -= other_box.max.x;
            test.min.z -= other_box.max.z;
            test.max.x -= other_box.min.x;
            test.max.z -= other_box.min.z;
            if (PushBlockGapEpsilonPc > 0.0f) {
                test.min.x = PushBlockGapEpsilonPc + test.min.x;
                test.min.z += PushBlockGapEpsilonPc;
                test.max.x = test.max.x - PushBlockGapEpsilonPc;
                test.max.z = test.max.z - PushBlockGapEpsilonPc;
            }
            if (!(other_position->x >= test.min.x) || !(other_position->x <= test.max.x) ||
                !(other_position->z >= test.min.z) || !(other_position->z <= test.max.z))
                continue;
            other->snap_origin = *other_position;
            other_position->x += delta->x;
            other_position->y -= delta->y;
            other->ground_offset -= delta->y;
            other_position->z = delta->z + other_position->z;
            other->packed_state_flags |= 4u;
            if (BlockInBlock(opaque_world, (PUSH_BLOCK_PC *)other, index, &other->supporting_block) != NULL)
                other->packed_state_flags |= 0x400u;
            NuSpecialUpdate(&other->special);
        }
    }

    void __cdecl MoveBlocks(WORLDINFO *opaque_world, PUSH_BLOCK_PC *opaque_block, i32 index, NUVEC *velocity) {
        PUSH_RUNTIME_BLOCK_VIEW_PC *block = (PUSH_RUNTIME_BLOCK_VIEW_PC *)opaque_block;
        NUVEC delta;
        PUSH_BLOCK_PC *hit;
        i32 j;
        delta.x = velocity->x;
        delta.y = 0.0f;
        delta.z = velocity->z;
        block->position->x += delta.x;
        block->position->z += delta.z;
        NuSpecialUpdate(&block->special);
        for (j = 0; j < block->end_position_count; ++j) {
            NUMTX *matrix = NuSpecialGetInstanceMtx(&block->end_position_specials[j]);
            NUVEC position = *block->position;
            matrix->m30 = position.x;
            matrix->m32 = position.z;
            NuSpecialUpdate(&block->end_position_specials[j]);
        }
        if (BlockInBlock(opaque_world, opaque_block, index, &block->supporting_block) != NULL) {
            block->packed_state_flags |= 0x400u;
        } else {
            MoveBlocksOverBlock(opaque_world, opaque_block, index, &delta);
            if (block->pushing_object != NULL && (GizPushFabsPc(delta.x) > 0.001f || GizPushFabsPc(delta.z) > 0.001f) &&
                NuFmod(GameTimer.time_elapsed, PushRumblePeriodPc) < PushRumbleWindowPc) {
                NewRumble(block->pushing_object->pad->bound_pad,
                          (f32)((f64)qrand() * 0.000015259021893143654 * (f64)0.05f), 0);
            }
        }
        if ((block->packed_state_flags & 8u) != 0) {
            delta.x = 0.0f;
            delta.y = velocity->y;
            delta.z = 0.0f;
            block->position->y -= delta.y;
            block->ground_offset -= delta.y;
            hit = BlockInBlock(opaque_world, opaque_block, index, &block->supporting_block);
            if (hit != NULL) {
                PUSH_RUNTIME_BLOCK_VIEW_PC *other = (PUSH_RUNTIME_BLOCK_VIEW_PC *)hit;
                block->settled_height = other->position->y + other->bounds_max.y;
                block->packed_state_flags |= 0x800u;
                NuSpecialUpdate(&block->special);
                return;
            }
            MoveBlocksOverBlock(opaque_world, opaque_block, index, &delta);
            NuSpecialUpdate(&block->special);
        }
    }

    i32 __cdecl TerrainBlockOnBlock(WORLDINFO *opaque_world, PUSH_BLOCK_PC *block, NUVEC *points, f32 *heights) {
        i32 i, j;
        if (points == NULL || heights == NULL)
            return 0;
        PUSH_SUPPORT_WORLD_VIEW_PC *world = (PUSH_SUPPORT_WORLD_VIEW_PC *)opaque_world;
        heights[0] = -10000.0f;
        heights[1] = -10000.0f;
        heights[2] = -10000.0f;
        heights[3] = -10000.0f;
        for (i = 0; i < world->count; ++i) {
            PUSH_SUPPORT_BLOCK_VIEW_PC *other = &world->blocks[i];
            NUVEC min, max;
            if ((PUSH_BLOCK_PC *)other == block)
                continue;
            max = min = *other->position;
            min.x -= GizPushFabsPc(other->bounds_min.x);
            min.z -= GizPushFabsPc(other->bounds_min.z);
            max.x += GizPushFabsPc(other->bounds_max.x);
            max.y += GizPushFabsPc(other->bounds_max.y);
            max.z += GizPushFabsPc(other->bounds_max.z);
            for (j = 0; j < 4; ++j) {
                if (points[j].x > min.x && points[j].x < max.x && points[j].z > min.z && points[j].z < max.z)
                    heights[j] = max.y;
            }
        }
        return 1;
    }
}

extern "C" i32 __cdecl OtherBlockInRange(WORLDINFO *opaque_world, PUSH_BLOCK_PC *opaque_block, NUVEC *position,
                                         i32 excluded) {
    WORLDINFO *world = (WORLDINFO *)opaque_world;
    PUSH_BLOCK_PC *block = (PUSH_BLOCK_PC *)opaque_block;
    NUVEC centre;
    f32 radius;
    NuSpecialGetRadius(&block->special, &centre, &radius);
    radius *= radius;
    for (i32 i = 0; i < world->push_block_count; ++i) {
        PUSH_BLOCK_PC *other;
        NUVEC minimum, maximum;
        f32 other_radius, x, z, sum, distance;
        if (i == excluded)
            continue;
        other = &world->push_blocks[i];
        if ((other->packed_state_flags & 0x00040000u) == 0 || (other->packed_state_flags & 0x01000000u) == 0 ||
            (other->packed_state_flags & 0x04000000u) != 0)
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
        if (distance <= radius || distance <= 0.0f)
            return 1;
    }
    return 0;
}
extern "C" {

    i32 __cdecl SnapPosTaken(WORLDINFO *opaque_world, PUSH_BLOCK_PC *block, NUVEC *position, i32 excluded) {
        PUSH_SUPPORT_WORLD_VIEW_PC *world = (PUSH_SUPPORT_WORLD_VIEW_PC *)opaque_world;
        i32 count = world->count;
        i32 i;
        for (i = 0; i < count; ++i) {
            PUSH_SUPPORT_BLOCK_VIEW_PC *other;
            f32 x, y, z;
            if (i == excluded)
                continue;
            other = &world->blocks[i];
            if ((other->packed_state_flags & 0x02000300u) == 0)
                continue;
            x = other->position->x - position->x;
            y = other->position->y - position->y;
            z = other->position->z - position->z;
            if (x * x + y * y + z * z <= (f64)0.002500000176951289f)
                return 1;
        }
        return 0;
    }
}

extern "C" void __cdecl PushSeekComplete(PUSH_BLOCK_PC *opaque_block, i32 index) {
    PUSH_BLOCK_PC *block = opaque_block;
    block->completed_snap_positions |= (u8)(1 << index);
    block->packed_state_flags &= ~0x200u;
    if ((block->packed_state_flags & 0x20000000u) == 0)
        block->packed_state_flags |= 0x02000100u;
    if (block->pushing_object != NULL)
        NewBuzz(block->pushing_object->pad->bound_pad, 0.1f, 0);
}

extern "C" void __cdecl UpdatePushBlocks(void *world_ptr, void *, f32) {
    static f32 snap_time = 0.5f;
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    i32 target_seen = 0;
    if (world == NULL || world->push_block_count == 0 || world->push_blocks == NULL)
        return;
    snap_time = 0.25f;
    for (i32 index = 0; index < world->push_block_count; ++index) {
        PUSH_BLOCK_PC *p = &world->push_blocks[index];
        p->push_visible = NuSpecialGetVisibilityFn(&p->special);
        if (p->push_visible == 0)
            continue;
        GizObstacles_AddTrigger(p->position);
        if (p->flag_000 != 0 && p->flag_007 != 0) {
            p->flag_006 = 0;
            p->flag_007 = 0;
        }
        if (p->flag_007 != 0)
            p->flag_006 = 1;
        if ((p->packed_state_flags & 0x02000100u) != 0)
            continue;
        if (p->snap_positions != NULL) {
            if ((p->packed_state_flags & 0x200u) != 0) {
                if (p->snap_timer <= snap_time) {
                    f32 fraction = p->snap_timer / snap_time;
                    NUVEC *position = p->position;
                    i32 angle = static_cast<i32>(16384.0 + (static_cast<f64>(fraction) * 16384.0 + 49152.0));
                    fraction = NuTrigTable[(angle >> 1) & 0x7fff];
                    position->x =
                        p->snap_origin.x +
                        (p->snap_positions[(p->packed_state_flags >> 15) & 7].x - p->snap_origin.x) * fraction;
                    position->y =
                        p->snap_origin.y +
                        (p->snap_positions[(p->packed_state_flags >> 15) & 7].y - p->snap_origin.y) * fraction;
                    position->z =
                        p->snap_origin.z +
                        (p->snap_positions[(p->packed_state_flags >> 15) & 7].z - p->snap_origin.z) * fraction;
                    NuSpecialUpdate(&p->special);
                    for (i32 j = 0; j < p->end_position_count; ++j) {
                        NUVEC *translation = &NuSpecialGetInstanceMtx(&p->end_position_specials[j])->translation;
                        NUVEC *target = &p->snap_positions[(p->packed_state_flags >> 15) & 7];
                        f32 z_product = (target->z - p->end_position_origins[j].z) * fraction;
                        f32 x_product = (target->x - p->end_position_origins[j].x) * fraction;
                        translation->x = p->end_position_origins[j].x + x_product;
                        translation->z = p->end_position_origins[j].z + z_product;
                        NuSpecialUpdate(&p->end_position_specials[j]);
                    }
                    p->snap_timer += FRAMETIME;
                    continue;
                }
                PushSeekComplete(reinterpret_cast<PUSH_BLOCK_PC *>(p), 7 & ((p->packed_state_flags >> 15)));
            } else {
                for (i32 j = 1; j < static_cast<i32>((p->packed_state_flags >> 12) & 7); ++j) {
                    NUVEC *target = &p->snap_positions[j];
                    NUVEC *position = p->position;
                    f32 x = target->x - position->x;
                    f32 y = target->y - position->y;
                    f32 z = target->z - position->z;
                    f32 y_squared = y * y;

                    if (!(static_cast<f64>(x) * x + y_squared + static_cast<f64>(z) * z <
                          static_cast<f64>(p->snap_distance) * p->snap_distance))
                        continue;
                    if (SnapPosTaken(reinterpret_cast<WORLDINFO *>(world), reinterpret_cast<PUSH_BLOCK_PC *>(p), target,
                                     index) != 0)
                        continue;
                    target_seen = 1;
                    if ((p->packed_state_flags & 0x10000000u) != 0 ||
                        OtherBlockInRange(reinterpret_cast<WORLDINFO *>(world), reinterpret_cast<PUSH_BLOCK_PC *>(p),
                                          target, index) != 0)
                        continue;
                    p->packed_state_flags |= 0x200u;
                    if ((p->packed_state_flags & 0x20000000u) != 0)
                        p->packed_state_flags |= 0x10000000u;
                    p->snap_origin.x = position->x;
                    p->snap_origin.z = position->z;
                    for (i32 k = 0; k < p->end_position_count; ++k)
                        p->end_position_origins[k] = *NuSpecialGetDrawPos(&p->end_position_specials[k]);
                    p->packed_state_flags = (p->packed_state_flags & ~0x38000u) | ((j & 7) << 15);
                    p->snap_timer = FRAMETIME;
                    break;
                }
                if (target_seen == 0) {
                    if ((p->packed_state_flags & 0x20000000u) != 0)
                        p->packed_state_flags &= ~0x10038000u;
                    if ((p->packed_state_flags & 0x40000000u) == 0)
                        p->completed_snap_positions = 0;
                }
            }
        }
        if (p->flag_006 == 0 && (((p->packed_state_flags & 2u) != 0 && p->flag_000 == 0) ||
                                 p->snap_origin.x != p->position->x || p->snap_origin.z != p->position->z)) {
            p->velocity.x = p->velocity.z = p->target_velocity.x = p->target_velocity.z = 0.0f;
        }
        if (p->flag_000 == 0 && (p->packed_state_flags & 8u) == 0 && p->flag_006 == 0)
            continue;
        if (p->flag_000 != 0 || p->flag_006 != 0) {
            i32 surface = p->terrain_info[0];
            if (p->terrain_info[1] == surface && p->terrain_info[2] == surface && p->terrain_info[3] == surface) {
                if (static_cast<u32>(surface) <= 31u && (TerSurface[surface].flags & 0x20000u) != 0)
                    p->packed_state_flags |= 0x40u;
                else
                    p->packed_state_flags &= ~0x40u;
            }
            if (p->pushing_object != NULL) {
                if ((p->packed_state_flags & 0x40u) == 0)
                    p->target_velocity.x = p->target_velocity.z = 0.0f;
                if (GizPushFabsPc(p->pushing_object->target_velocity.x) >
                    GizPushFabsPc(p->pushing_object->target_velocity.z)) {
                    p->target_velocity.x = FRAMETIME * p->pushing_object->target_velocity.x;
                    p->target_velocity.z = p->velocity.z = 0.0f;
                } else {
                    p->target_velocity.x = p->velocity.x = 0.0f;
                    p->target_velocity.z = FRAMETIME * p->pushing_object->target_velocity.z;
                }
            } else if (p->terrain_info[0] == p->terrain_info[1] && p->terrain_info[0] == p->terrain_info[2] &&
                       p->terrain_info[0] == p->terrain_info[3]) {
                f32 scale = TerSurface[p->terrain_info[0]].movement_scale;
                p->target_velocity.x *= scale;
                p->target_velocity.z *= scale;
            } else {
                p->velocity.z = p->target_velocity.z = p->velocity.x = p->target_velocity.x = 0.0f;
            }
            p->velocity.x = SeekValF(p->velocity.x, p->target_velocity.x, PushVelocitySeekRatePc);
            p->velocity.z = SeekValF(p->velocity.z, p->target_velocity.z, PushVelocitySeekRatePc);
            f32 stop_speed = (30.0 / (1.0 / static_cast<f64>(FRAMETIME))) * static_cast<f64>(0.00001f);
            if (p->velocity.x == 0.0f) {
                if (GizPushFabsPc(p->velocity.z) <= stop_speed) {
                    p->target_velocity.z = p->velocity.z = 0.0f;
                    p->packed_state_flags &= ~0xc0u;
                }
            } else {
                if (GizPushFabsPc(p->velocity.x) <= stop_speed) {
                    p->target_velocity.x = p->velocity.x = 0.0f;
                    p->packed_state_flags &= ~0xc0u;
                }
            }
        }
        if ((p->packed_state_flags & 8u) != 0) {
            p->target_velocity.x = 0.0f;
            p->target_velocity.y = PushFallingSpeedPc * FRAMETIME;
            p->velocity.x = p->target_velocity.z = 0.0f;
            p->velocity.y = SeekValF(p->velocity.y, p->target_velocity.y, PushVelocitySeekRatePc);
            p->velocity.z = 0.0f;
        } else {
            p->target_velocity.y = 0.0f;
        }
    }
    for (i32 index = 0; index < world->push_block_count; ++index) {
        PUSH_BLOCK_PC *p = &world->push_blocks[index];
        if ((p->packed_state_flags & 0x40000u) != 0 && (p->packed_state_flags & 0x06000100u) == 0 &&
            (p->packed_state_flags & 0x69u) != 0)
            MoveBlocks(reinterpret_cast<WORLDINFO *>(world), reinterpret_cast<PUSH_BLOCK_PC *>(p), index, &p->velocity);
    }
    for (i32 index = 0; index < world->push_block_count; ++index) {
        PUSH_BLOCK_PC *p = &world->push_blocks[index];
        if ((p->packed_state_flags & 0x40000u) == 0 || (p->packed_state_flags & 0x02000000u) != 0)
            continue;
        if (NuSpecialGetVisibilityFn(&p->special) == 0 || (p->packed_state_flags & 0x4du) == 0)
            continue;
        f32 left = GizPushFabsPc(p->bounds_min.x) + PushCornerInsetPc;
        f32 right = GizPushFabsPc(p->bounds_max.x) + PushCornerInsetPc;
        f32 back = GizPushFabsPc(p->bounds_min.z) + PushCornerInsetPc;
        f32 front = GizPushFabsPc(p->bounds_max.z) + PushCornerInsetPc;
        NUVEC corners[4];
        corners[1] = corners[2] = corners[3] = corners[0] = *p->position;
        corners[0].y = (corners[0].y - GizPushFabsPc(p->bounds_min.y)) - PushGroundOffsetPc;
        corners[1].y = corners[2].y = corners[3].y = corners[0].y;
        for (i32 j = 0; j < 4; ++j)
            corners[j].y += 0.025f;
        corners[0].x -= left;
        corners[0].z -= back;
        corners[1].x += right;
        corners[1].z = corners[1].z - back;
        corners[2].x += right;
        corners[2].z += front;
        corners[3].x -= left;
        corners[3].z += front;
        PlatOnOff(p->platform_id, 0);
        bool platforms_off = p->supporting_block != NULL || p->previous_supporting_block != NULL;
        f32 heights[4];
        if (!platforms_off) {
            heights[0] = GameShadow(NULL, &corners[0], 0.1f, -1);
            p->terrain_info[0] = static_cast<i8>(ShadowInfo());
            p->extra_terrain_info.bytes[0] = static_cast<i8>(EShadowInfo());
            heights[1] = GameShadow(NULL, &corners[1], 0.1f, -1);
            p->terrain_info[1] = static_cast<i8>(ShadowInfo());
            p->extra_terrain_info.bytes[1] = static_cast<i8>(EShadowInfo());
            heights[2] = GameShadow(NULL, &corners[2], 0.1f, -1);
            p->terrain_info[2] = static_cast<i8>(ShadowInfo());
            p->extra_terrain_info.bytes[2] = static_cast<i8>(EShadowInfo());
            heights[3] = GameShadow(NULL, &corners[3], 0.1f, -1);
        } else {
            NewTerrPlatformsOff();
            heights[0] = GameShadow(NULL, &corners[0], 0.1f, -1);
            p->terrain_info[0] = static_cast<i8>(ShadowInfo());
            p->extra_terrain_info.bytes[0] = static_cast<i8>(EShadowInfo());
            NewTerrPlatformsOff();
            heights[1] = GameShadow(NULL, &corners[1], 0.1f, -1);
            p->terrain_info[1] = static_cast<i8>(ShadowInfo());
            p->extra_terrain_info.bytes[1] = static_cast<i8>(EShadowInfo());
            NewTerrPlatformsOff();
            heights[2] = GameShadow(NULL, &corners[2], 0.1f, -1);
            p->terrain_info[2] = static_cast<i8>(ShadowInfo());
            p->extra_terrain_info.bytes[2] = static_cast<i8>(EShadowInfo());
            NewTerrPlatformsOff();
            heights[3] = GameShadow(NULL, &corners[3], 0.1f, -1);
        }
        p->terrain_info[3] = static_cast<i8>(ShadowInfo());
        p->extra_terrain_info.bytes[3] = static_cast<i8>(EShadowInfo());
        PlatOnOff(p->platform_id, 1);
        BlockInBlock(reinterpret_cast<WORLDINFO *>(world), reinterpret_cast<PUSH_BLOCK_PC *>(p), index,
                     &p->supporting_block);
        struct pushblock_s *below = p->supporting_block;
        if (below != NULL || p->previous_supporting_block != NULL) {
            p->previous_supporting_block = below;
            f32 support[4];
            TerrainBlockOnBlock(reinterpret_cast<WORLDINFO *>(world), reinterpret_cast<PUSH_BLOCK_PC *>(p), corners,
                                support);
            for (i32 j = 0; j < 4; ++j)
                if (support[j] != -10000.0f)
                    heights[j] = support[j];
        } else {
            p->previous_supporting_block = NULL;
        }
        NUVEC *position = p->position;
        f32 ground = p->ground_height;
        if ((heights[0] < ground - PushBlockGapEpsilonPc && heights[1] < ground - PushBlockGapEpsilonPc &&
             heights[2] < ground - PushBlockGapEpsilonPc && heights[3] < ground - PushBlockGapEpsilonPc) ||
            (heights[0] > ground + PushBlockGapEpsilonPc && heights[1] > ground + PushBlockGapEpsilonPc &&
             heights[2] > ground + PushBlockGapEpsilonPc && heights[3] > ground + PushBlockGapEpsilonPc)) {
            f32 height = ((heights[0] + heights[1]) + heights[2] + heights[3]) * 0.25 + PushBlockGapEpsilonPc;
            p->ground_height = height;
            p->ground_offset = (position->y + p->bounds_min.y) - height;
            p->packed_state_flags |= 8u;
        }
        if (below != NULL) {
            p->ground_height = below->bounds_max.y + position->y;
            p->ground_offset = (p->bounds_min.y + position->y) - p->ground_height;
        }
        if (p->ground_offset > PushBlockGapEpsilonPc + 0.0) {
            p->packed_state_flags |= 8u;
        } else if ((p->packed_state_flags & 8u) != 0) {
            p->settled_height = p->ground_height;
            p->packed_state_flags = (p->packed_state_flags & ~8u) | 0x800u;
        }
        i32 safe = 0;
        for (i32 j = 0; j < 4; ++j)
            if (p->extra_terrain_info.bytes[j] == 8 || p->terrain_info[j] == 14)
                ++safe;
        if (safe < 4)
            p->packed_state_flags |= 0x400u;
    }
    for (i32 index = 0; index < world->push_block_count; ++index) {
        PUSH_BLOCK_PC *p = &world->push_blocks[index];
        if ((p->packed_state_flags & 0x40000u) == 0 || (p->packed_state_flags & 0x02000100u) != 0)
            continue;
        if ((p->packed_state_flags & 0x400u) == 0 && (p->packed_state_flags & 1u) != 0)
            GameAudio_PlaySfx(72, p->position, GameAudio_GetPlrSfxBits(p->pushing_object), 0);
        const u32 snapped = (p->packed_state_flags >> 10) & 1;
        if (snapped != 0 || (p->packed_state_flags & 0x800u) != 0) {
            NUVEC *position = p->position;
            if (snapped != 0) {
                position->x = p->snap_origin.x;
                position->z = p->snap_origin.z;
                p->extra_terrain_info.word = p->previous_extra_terrain_info;
                p->packed_state_flags &= ~0xc0u;
                p->velocity.z = p->velocity.x = p->target_velocity.z = p->target_velocity.x = 0.0f;
                NuSpecialUpdate(&p->special);
                for (i32 j = 0; j < p->end_position_count; ++j) {
                    if (0 != NuSpecialExistsFn(&p->end_position_specials[j])) {
                        NUVEC *translation = &NuSpecialGetInstanceMtx(&p->end_position_specials[j])->translation;
                        if (translation != NULL) {
                            translation->x = p->snap_origin.x;
                            translation->z = p->snap_origin.z;
                            NuSpecialUpdate(&p->end_position_specials[j]);
                        }
                    }
                }
                p->packed_state_flags &= ~0x401u;
            }
            if ((p->packed_state_flags & 0x800u) != 0) {
                p->velocity.y = p->target_velocity.y = 0.0f;
                p->packed_state_flags &= ~0x808u;
                f32 settled = p->settled_height;
                p->ground_height = settled;
                f32 settled_y = settled - p->bounds_min.y;
                position->y = settled_y;
                p->ground_offset = (settled_y + p->bounds_min.y) - p->ground_height;
                NuSpecialUpdate(&p->special);
            }
        } else if ((p->packed_state_flags & 1u) != 0) {
            if ((p->packed_state_flags & 2u) == 0 && p->pushing_object != NULL)
                GameAudio_PlaySfx(71, &p->pushing_object->collision_position,
                                  GameAudio_GetPlrSfxBits(p->pushing_object), 0);
            if (PushBlockHintIdPc != -1 && p->pushing_object != NULL && (p->pushing_object->flags_1fc_word & 1u) != 0 &&
                (p->pushing_object->flags_1fc_word & 0x80u) != 0)
                Hint_SetComplete(PushBlockHintIdPc);
        }
    }
    for (i32 index = 0; index < world->push_block_count; ++index) {
        PUSH_BLOCK_PC *p = &world->push_blocks[index];
        if ((p->packed_state_flags & 0x40000u) == 0)
            continue;
        if (p->pushing_object != NULL) {
            if (p->platform_id != -1 && (p->packed_state_flags & 9u) != 0) {
                p->pushing_object->flags_ef0 |= 1u;
                AddShoveObject(&p->special, static_cast<i16>(p->platform_id));
            } else {
                p->pushing_object->flags_ef0 &= ~1u;
            }
        }
        if ((p->packed_state_flags & 0x20u) == 0 && (p->packed_state_flags & 0x200u) == 0) {
            i32 safe = 0;
            for (i32 j = 0; j < 4; ++j)
                if (p->extra_terrain_info.bytes[j] == 8 || p->terrain_info[j] == 14)
                    ++safe;
            if (safe == 4) {
                p->previous_extra_terrain_info = p->extra_terrain_info.word;
                p->snap_origin = *p->position;
            }
        }
        p->packed_state_flags = (p->packed_state_flags & ~2u) | ((p->packed_state_flags & 1u) << 1);
        p->packed_state_flags = (p->packed_state_flags & ~0x15u) | ((8u & p->packed_state_flags) << 1);
        p->pushing_object = NULL;
    }
}

extern "C" PUSH_BLOCK_PC *__cdecl NearestPushBlock(WORLDINFO *opaque_world, NUVEC *position, f32 range) {
    WORLDINFO *world = reinterpret_cast<WORLDINFO *>(opaque_world);
    if (world == NULL || position == NULL)
        return NULL;
    NUVEC minimum = {position->x - range, position->y - range, position->z - range};
    NUVEC maximum = {position->x + range, position->y + range, position->z + range};
    PUSH_BLOCK_PC *nearest = NULL;
    f32 nearest_distance = 1000000000.0f;
    PUSH_BLOCK_PC *block = world->push_blocks;
    for (i32 index = 0; index < world->push_block_count; ++index, ++block) {
        if ((block->packed_state_flags & 0x00040000u) == 0 || (block->packed_state_flags & 0x01000000u) == 0 ||
            (block->packed_state_flags & 0x100u) != 0)
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
extern "C" {

    i32 __cdecl Push_UpdateHints(HINT_PC *hint) {
        i32 index;
        (void)hint;
        if (WORLD->area != NULL && WORLD->area == HUB_ADATA)
            return 0;
        if (LEGOACT_PUSH == -1)
            return 0;
        for (index = 0; index < 8; ++index) {
            GAMEOBJECT *object = (GAMEOBJECT *)Player[index];
            if (object != NULL && (object->flags_1fc & 0x80) != 0 &&
                object->character_model->model_data_b[LEGOACT_PUSH] != NULL &&
                NearestPushBlock(WORLD, &object->collision_position, 2.0f) != NULL)
                return 1;
        }
        return 0;
    }
}

static void *__cdecl PushBlocks_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    LEVELDATA_PC *level = world->current_level;
    world->push_blocks = NULL;
    world->push_block_count = 0;
    if (level->max_pushblocks > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3u) & ~3u;
        world->push_blocks = static_cast<PUSH_BLOCK_PC *>(world->giz_buffer.void_ptr);
        world->giz_buffer.addr += level->max_pushblocks * sizeof(PUSH_BLOCK_PC);
    }
    return world->push_blocks;
}

static void __cdecl Push_Reset(void *world, void *, void *progress) {
    ResetPushProgress(static_cast<WORLDINFO *>(world), progress);
}

static void __cdecl Push_Activate(GIZMO_PC *gizmo, i32 active) {
    PUSH_BLOCK_PC *block;
    WORLDINFO *world;
    i32 index;
    if (gizmo != NULL) {
        block = static_cast<PUSH_BLOCK_PC *>(gizmo->object);
        block->push_active = active != 0;
        if (active != 0) {
            world = WORLD;
            for (index = 0; index < world->push_block_count; ++index) {
                if (block == &world->push_blocks[index])
                    break;
            }
            if (index < world->push_block_count)
                ResetSinglePushBlock(world, block, index);
            block->push_visible = 1;
        }
    }
}

static void __cdecl Push_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    PUSH_BLOCK_PC *block = static_cast<PUSH_BLOCK_PC *>(gizmo->object);
    block->push_visible = visible != 0;
    NuSpecialSetVisibility(&block->special, visible);
    for (i32 index = 0; index < block->end_position_count; ++index)
        NuSpecialSetVisibility(&block->end_position_specials[index], block->push_visible);
}

static i32 __cdecl Push_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    return world != NULL ? world->current_level->max_pushblocks : 0;
}

static void __cdecl Push_AddGizmos(void *system, i32 type_id, void *world_ptr, void *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    for (i32 index = 0; index < world->push_block_count; ++index) {
        if (NuStrLen(world->push_blocks[index].name) != 0)
            AddGizmo(system, type_id, NULL, &world->push_blocks[index]);
    }
}

static char *__cdecl GizPush_GetGizmoName(GIZMO_PC *gizmo) {
    if (gizmo != NULL)
        return static_cast<PUSH_BLOCK_PC *>(gizmo->object)->name;
    return NULL;
}

static i32 __cdecl GizPush_GetOutput(GIZMO_PC *gizmo, i32 output_index, i32) {
    PUSH_BLOCK_PC *block = static_cast<PUSH_BLOCK_PC *>(gizmo->object);
    if (block != NULL) {
        if (output_index == 0) {
            if (block->completed_snap_positions == 0)
                return 0;
        } else if ((block->completed_snap_positions & (1 << output_index)) == 0) {
            return 0;
        }
        return 1;
    }
    return 0;
}
static char outputName[13] = "Any Complete";

static char *__cdecl GizPush_GetOutputName(GIZMO_PC *gizmo, i32 output_index) {
    PUSH_BLOCK_PC *block = static_cast<PUSH_BLOCK_PC *>(gizmo->object);
    if (output_index < 0 || output_index > block->output_count)
        return NULL;
    if (output_index == 0)
        return const_cast<char *>("Any Complete");
    NuIToA(output_index, outputName, 10);
    NuStrCat(outputName, " Complete");
    return outputName;
}

static i32 __cdecl GizPush_GetNumOutputs(GIZMO_PC *gizmo) {
    return static_cast<PUSH_BLOCK_PC *>(gizmo->object)->output_count;
}

static void *__cdecl Push_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 0x24c);
}

static void __cdecl Push_ClearProgress(void *, void *progress_ptr) {
    PUSHPROGRESS *progress = static_cast<PUSHPROGRESS *>(progress_ptr);
    if (progress != NULL) {
        // Each mask is a bit array of one word (16 blocks).
        for (i32 i = 0; i < 1; ++i)
            progress->state_mask[i] = 0;
        for (i32 i = 0; i < 1; ++i)
            progress->visible_mask[i] = ~0u;
        for (i32 i = 0; i < 1; ++i)
            progress->position_mask[i] = 0;
        NuMemSet128(progress->positions, 0, 0);
    }
}

static void __cdecl Push_StoreProgress(void *world_ptr, void *, void *progress_ptr) {
    PUSHPROGRESS *progress = static_cast<PUSHPROGRESS *>(progress_ptr);
    if (progress == NULL)
        return;
    Push_ClearProgress(world_ptr, progress);
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world == NULL || world->push_blocks == NULL)
        return;
    PUSH_BLOCK_PC *block = world->push_blocks;
    for (i32 index = 0; index < world->push_block_count && index < 16; ++index, ++block) {
        const i32 word = index / 32;
        const u32 mask = 1u << (index & 31);
        if ((block->packed_state_flags & 0x40000u) == 0)
            progress->visible_mask[word] &= ~mask;
        if ((block->packed_state_flags & 0x02000000u) != 0)
            progress->state_mask[word] |= mask;
        if (NuSpecialExistsFn(&block->special) != 0) {
            progress->positions[index] = NuSpecialGetInstanceMtx(&block->special)->translation;
        } else {
            progress->positions[index] = v000;
        }
        for (i32 end = 0; end < block->end_position_count; ++end) {
            NUSPECIALHANDLE *special = &block->end_position_specials[end];
            if (special != NULL) {
                if (NuSpecialExistsFn(special) != 0) {
                    progress->end_positions[end][index] = NuSpecialGetInstanceMtx(special)->translation;
                } else {
                    progress->end_positions[end][index] = v000;
                }
            }
        }
        progress->position_mask[word] |= mask;
    }
}

static i32 __cdecl edpush_Load(void *world_ptr, void *) {
    static i32 version = -1;
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    if (world->push_block_count == 0) {
        version = EdFileReadInt();
        world->push_block_count = EdFileReadInt();
        i32 valid_count = 0;
        char special_name[256];
        for (i32 file_index = 0; file_index < world->push_block_count; ++file_index) {
            PUSH_BLOCK_PC *block = &world->push_blocks[valid_count];
            i32 missing = 0;
            memset(special_name, 0, sizeof(special_name));
            EdFileRead(special_name, static_cast<i8>(EdFileReadChar()));
            if (Gizmo_FindNuSpecial(world->current_gscn, &block->special, special_name, 1, world->gizmo_sys,
                                    const_cast<char *>("PushBlock"), block->name) == 0) {
                missing = 1;
            } else {
                NuStrCpy(block->name, NuSpecialGetName(&block->special));
            }
            block->snap_distance = EdFileReadFloat();
            block->packed_state_flags &= ~0x04000000u;
            if (EdFileReadChar() != 0)
                block->packed_state_flags |= 0x04000000u;
            block->packed_state_flags &= ~0x08000000u;
            if (EdFileReadChar() != 0)
                block->packed_state_flags |= 0x08000000u;
            block->packed_state_flags &= ~0x00100000u;
            if (EdFileReadChar() != 0)
                block->packed_state_flags |= 0x00100000u;
            block->packed_state_flags &= ~0x00200000u;
            if (EdFileReadChar() != 0)
                block->packed_state_flags |= 0x00200000u;
            if (version >= 4) {
                block->packed_state_flags &= ~0x20000000u;
                if (EdFileReadChar() != 0)
                    block->packed_state_flags |= 0x20000000u;
                block->packed_state_flags &= ~0x40000000u;
                if (EdFileReadChar() != 0)
                    block->packed_state_flags |= 0x40000000u;
            }
            if (version >= 5) {
                block->packed_state_flags &= ~0x00400000u;
                if (EdFileReadChar() != 0)
                    block->packed_state_flags |= 0x00400000u;
                block->packed_state_flags &= ~0x00800000u;
                if (EdFileReadChar() != 0)
                    block->packed_state_flags |= 0x00800000u;
            }
            block->end_position_count = 0;
            if (version >= 3) {
                const u8 count = static_cast<u8>(EdFileReadChar());
                block->end_position_count = count;
                for (i32 index = 0; index < count; ++index) {
                    memset(special_name, 0, sizeof(special_name));
                    EdFileRead(special_name, static_cast<i8>(EdFileReadChar()));
                    special_name[16] = '\0';
                    void *gizmo_sys = world->gizmo_sys;
                    if (Gizmo_FindNuSpecial(world->current_gscn, &block->end_position_specials[index], special_name, 1,
                                            gizmo_sys, const_cast<char *>("PushBlockLinkObject"), block->name) == 0)
                        --block->end_position_count;
                }
            }
            if (missing == 0)
                ++valid_count;
        }
        world->push_block_count = valid_count;
        return 1;
    }
    return 0;
}

static i32 __cdecl Pushblocks_BoltHitPlat(void *world_ptr, void *blocks_ptr, BOLT_PC *bolt, u8 *) {
    WORLDINFO *world = static_cast<WORLDINFO *>(world_ptr);
    PUSH_BLOCK_PC *block = static_cast<PUSH_BLOCK_PC *>(blocks_ptr);
    if (block != NULL) {
        for (i32 index = 0; index < world->push_block_count; ++index, ++block) {
            if ((block->packed_state_flags & 0x40000u) == 0 || (block->packed_state_flags & 0x01000000u) == 0 ||
                (block->packed_state_flags & 0x02000000u) != 0)
                continue;
            i32 platform = FindPlatInst(NuSpecialGetInstanceix(&block->special));
            if (platform == -1 || platform != bolt->hit_platform)
                continue;
            if (bolt->owner != NULL) {
                NUVEC direction;
                NuVecSub(&direction, &static_cast<GAMEOBJECT *>(bolt->owner)->collision_position, block->position);
                if (direction.x == direction.z) {
                    if (direction.x < 0.0f) {
                        direction.x = -(static_cast<f64>(qrand()) * 0.000015259021893143654);
                        direction.z = -(static_cast<f64>(qrand()) * 0.000015259021893143654);
                    } else {
                        direction.x = static_cast<f64>(qrand()) * 0.000015259021893143654;
                        direction.z = static_cast<f64>(qrand()) * 0.000015259021893143654;
                    }
                }
                if (GizPushFabsPc(direction.x) > GizPushFabsPc(direction.z))
                    direction.z = 0.0f;
                else
                    direction.x = 0.0f;
                NuVecNorm(&direction, &direction);
                KnockPushBlock(reinterpret_cast<PUSH_BLOCK_PC *>(block), &direction);
            }
            return 1;
        }
    }
    return 0;
}
i32 pushblock_gizmotype_id = 0;

ADDGIZMOTYPE_PC *__cdecl Push_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC addtype = {0};
    char *name = PushBlocksNamePc;
    addtype = Default_ADDGIZMOTYPE;
    addtype.name = name;
    addtype.prefix = const_cast<char *>("");
    addtype.fns.options = 0x24c;
    addtype.fns.get_max_gizmos_fn = Push_GetMaxGizmos;
    addtype.fns.add_gizmos_fn = reinterpret_cast<void *>(Push_AddGizmos);
    addtype.fns.early_update_fn = NULL;
    addtype.fns.late_update_fn = UpdatePushBlocks;
    addtype.fns.draw_fn = NULL;
    addtype.fns.panel_draw_fn = NULL;
    addtype.fns.get_gizmo_name_fn = reinterpret_cast<char *(__cdecl *)(struct gizmo_pc_s *)>(GizPush_GetGizmoName);
    addtype.fns.get_output_fn = reinterpret_cast<i32(__cdecl *)(struct gizmo_pc_s *, i32, i32)>(GizPush_GetOutput);
    addtype.fns.get_output_name_fn =
        reinterpret_cast<char *(__cdecl *)(struct gizmo_pc_s *, i32)>(GizPush_GetOutputName);
    addtype.fns.get_num_outputs_fn = reinterpret_cast<void *>(GizPush_GetNumOutputs);
    addtype.fns.activate_fn = reinterpret_cast<void(__cdecl *)(struct gizmo_pc_s *, i32)>(Push_Activate);
    addtype.fns.set_visibility_fn = reinterpret_cast<void(__cdecl *)(struct gizmo_pc_s *, i32)>(Push_SetVisibility);
    addtype.fns.get_visibility_fn = NULL;
    addtype.fns.get_pos_fn = NULL;
    addtype.fns.using_special_fn = NULL;
    addtype.fns.bolt_hit_plat_fn = reinterpret_cast<void *>(Pushblocks_BoltHitPlat);
    addtype.fns.get_best_bolt_target_fn = NULL;
    addtype.fns.bolt_hit_fn = NULL;
    addtype.fns.allocate_progress_data_fn = Push_AllocateProgressData;
    addtype.fns.clear_progress_fn = Push_ClearProgress;
    addtype.fns.store_progress_fn = Push_StoreProgress;
    addtype.fns.reset_fn = Push_Reset;
    addtype.fns.reserve_buffer_space_fn = PushBlocks_ReserveBufferSpace;
    addtype.fns.load_fn = edpush_Load;
    addtype.fns.post_load_fn = NULL;
    addtype.fns.add_level_sfx_fn = NULL;
    pushblock_gizmotype_id = type_id;
    return &addtype;
}

extern "C" void __cdecl ResetSinglePushBlockHeight(WORLDINFO *world, struct pushblock_s *opaque, i32 index) {
    PUSH_BLOCK_PC *block = reinterpret_cast<PUSH_BLOCK_PC *>(opaque);
    BlockInBlock(world, reinterpret_cast<PUSH_BLOCK_PC *>(block), index, &block->supporting_block);
    if (block->supporting_block != NULL) {
        PUSH_BLOCK_PC *support = reinterpret_cast<PUSH_BLOCK_PC *>(block->supporting_block);
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
    WORLDINFO *world = reinterpret_cast<WORLDINFO *>(opaque);
    NUMTX evaluated;
    world->push_block_position_count = 0;
    RunOutOfPosTabSpacePc = 0;
    pushposincrease = 0;
    world->push_block_positions = static_cast<NUVEC *>(world->giz_buffer.void_ptr);
    world->giz_buffer.addr =
        (world->giz_buffer.addr + world->current_level->max_pushblock_endpos * sizeof(NUVEC) + 3) & ~3u;
    PUSH_BLOCK_PC *block = world->push_blocks;
    ResetPushProgress(opaque, progress);
    for (i32 index = 0; index < world->push_block_count; ++index, ++block) {
        block->animation_positions = 0;
        NUINSTANIM *animation = static_cast<NUINSTANIM *>(NuSpecialGetInstAnim(&block->special));
        if (animation != NULL) {
            i32 end_frame = static_cast<i32>(NuAnimEndFrameOld(
                static_cast<NUGSCN *>(block->special.scene)->instance_animation_data[animation->animation_index]));
            block->animation_positions = end_frame;
            block->output_count = static_cast<u8>(block->animation_positions);
            i32 required = world->push_block_position_count + block->animation_positions;
            if (required > world->current_level->max_pushblock_endpos) {
                if (RunOutOfPosTabSpacePc != 0)
                    pushposincrease += block->animation_positions;
                else {
                    RunOutOfPosTabSpacePc = 1;
                    pushposincrease += required - world->current_level->max_pushblock_endpos;
                }
            }
            if (block->animation_positions > 0 && RunOutOfPosTabSpacePc == 0) {
                block->snap_positions = &world->push_block_positions[world->push_block_position_count];
                f32 frame = 1.0f;
                for (i32 position = 0; position < static_cast<i32>(block->animation_positions);
                     ++position, frame += 1.0) {
                    EvalAnim(&block->special, frame, &evaluated, 0);
                    block->snap_positions[position] = *reinterpret_cast<NUVEC *>(&evaluated.m30);
                    evaluated = *NuSpecialGetMtx(&block->special);
                    block->snap_positions[position].x += evaluated.m30;
                    block->snap_positions[position].y += evaluated.m31;
                    block->snap_positions[position].z += evaluated.m32;
                    ++world->push_block_position_count;
                }
            }
        }
        ResetSinglePushBlock(opaque, reinterpret_cast<struct pushblock_s *>(block), index);
    }
    block = world->push_blocks;
    for (i32 index = 0; index < world->push_block_count; ++index, ++block) {
        NUVEC centre = *block->position;
        block->velocity = v000;
        block->target_velocity = v000;
        block->snap_origin = centre;
        f32 left = GizPushFabsPc(block->bounds_min.x) + PushInitCornerInsetPc;
        f32 right = GizPushFabsPc(block->bounds_max.x) + PushInitCornerInsetPc;
        f32 back = GizPushFabsPc(block->bounds_min.z) + PushInitCornerInsetPc;
        f32 front = GizPushFabsPc(block->bounds_max.z) + PushInitCornerInsetPc;
        NUVEC corners[4];
        corners[1] = corners[2] = corners[3] = corners[0] = centre;
        corners[0].y = (corners[0].y - GizPushFabsPc(block->bounds_min.y)) - PushInitGroundOffsetPc;
        corners[1].y = corners[2].y = corners[3].y = corners[0].y;
        for (i32 corner = 0; corner < 4; ++corner)
            corners[corner].y += 0.025f;
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
    for (i32 index = 0; index < world->push_block_count; ++index)
        ResetSinglePushBlockHeight(opaque, reinterpret_cast<struct pushblock_s *>(&world->push_blocks[index]), index);
}
