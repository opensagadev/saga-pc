#include "saga.h"
#include <math.h>
#include <string.h>
extern "C" {
    void *__cdecl GizmoBufferAlloc(VARIPTR *, VARIPTR *, i32);
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

extern "C" ADDGIZMOTYPE_PC Default_ADDGIZMOTYPE;
extern "C" char *PushBlocksNamePc;

typedef union PUSH_ABSOLUTE_FLOAT_BITS_PC {
    f32 value;
    u32 bits;
} PUSH_ABSOLUTE_FLOAT_BITS_PC;
STATIC_ASSERT(sizeof(PUSH_ABSOLUTE_FLOAT_BITS_PC) == 4);

extern "C" void __cdecl UpdatePushBlocks(void *world_ptr, void *, f32) {
    static f32 snap_time = 0.5f;
    PUSH_RUNTIME_WORLD_VIEW_PC *world = static_cast<PUSH_RUNTIME_WORLD_VIEW_PC *>(world_ptr);
    i32 target_seen = 0;
    if (world == NULL || world->count == 0 || world->blocks == NULL)
        return;
    snap_time = 0.25f;
    for (i32 index = 0; index < world->count; ++index) {
        PUSH_RUNTIME_BLOCK_VIEW_PC *p = &world->blocks[index];
        u32 visible = static_cast<u32>(NuSpecialGetVisibilityFn(&p->special)) & 1u;
        p->packed_state_flags = (p->packed_state_flags & ~0x40000u) | (visible << 18);
        if ((p->packed_state_flags & 0x40000u) == 0)
            continue;
        GizObstacles_AddTrigger(p->position);
        if ((p->packed_state_flags & 0x81u) == 0x81u)
            p->packed_state_flags &= ~0xc0u;
        if ((p->packed_state_flags & 0x80u) != 0)
            p->packed_state_flags |= 0x40u;
        if ((p->packed_state_flags & 0x02000100u) != 0)
            continue;
        if (p->snap_positions != NULL) {
            if ((p->packed_state_flags & 0x200u) != 0) {
                if (p->snap_timer <= snap_time) {
                    f32 fraction = p->snap_timer / snap_time;
                    i32 angle = static_cast<i32>(16384.0 + (static_cast<f64>(fraction) * 16384.0 + 49152.0));
                    fraction = NuTrigTable[(angle >> 1) & 0x7fff];
                    NUVEC *position = p->position;
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
                        NUMTX *matrix = NuSpecialGetInstanceMtx(&p->end_position_specials[j]);
                        NUVEC *target = &p->snap_positions[(p->packed_state_flags >> 15) & 7];
                        f32 z_product = (target->z - p->end_position_origins[j].z) * fraction;
                        f32 x_product = (target->x - p->end_position_origins[j].x) * fraction;
                        matrix->m30 = p->end_position_origins[j].x + x_product;
                        matrix->m32 = z_product + p->end_position_origins[j].z;
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
        if (0 == (p->packed_state_flags & 0x40u) &&
            (2u == (p->packed_state_flags & 3u) || p->position->x != p->snap_origin.x ||
             p->snap_origin.z != p->position->z)) {
            p->target_velocity.x = p->velocity.z = p->target_velocity.z = p->velocity.x = 0.0f;
        }
        if ((p->packed_state_flags & 0x49u) == 0)
            continue;
        if ((p->packed_state_flags & 0x41u) != 0) {
            i32 surface = p->terrain_info[0];
            bool same = p->terrain_info[1] == surface && p->terrain_info[2] == surface && p->terrain_info[3] == surface;
            if (same) {
                if (static_cast<u32>(surface) <= 31u && (TerSurface[surface].flags & 0x20000u) != 0)
                    p->packed_state_flags |= 0x40u;
                else
                    p->packed_state_flags &= ~0x40u;
            }
            if (p->pushing_object != NULL) {
                if ((p->packed_state_flags & 0x40u) == 0)
                    p->target_velocity.z = p->target_velocity.x = 0.0f;
                PUSH_ABSOLUTE_FLOAT_BITS_PC absolute_x, absolute_z;
                absolute_x.value = p->pushing_object->target_velocity.x;
                absolute_x.bits &= 0x7fffffffu;
                absolute_z.value = p->pushing_object->target_velocity.z;
                absolute_z.bits &= 0x7fffffffu;
                if (absolute_x.value > absolute_z.value) {
                    p->target_velocity.x = p->pushing_object->target_velocity.x * FRAMETIME;
                    p->velocity.z = p->target_velocity.z = 0.0f;
                } else {
                    p->velocity.x = p->target_velocity.x = 0.0f;
                    p->target_velocity.z = p->pushing_object->target_velocity.z * FRAMETIME;
                }
            } else if (same) {
                f32 scale = TerSurface[surface].movement_scale;
                p->target_velocity.x *= scale;
                p->target_velocity.z *= scale;
            } else {
                p->target_velocity.x = p->velocity.x = p->target_velocity.z = p->velocity.z = 0.0f;
            }
            p->velocity.x = SeekValF(p->velocity.x, p->target_velocity.x, PushVelocitySeekRatePc);
            p->velocity.z = SeekValF(p->velocity.z, p->target_velocity.z, PushVelocitySeekRatePc);
            f32 stop_speed = (30.0 / (1.0 / static_cast<f64>(FRAMETIME))) * static_cast<f64>(0.00001f);
            if (p->velocity.x == 0.0f) {
                PUSH_ABSOLUTE_FLOAT_BITS_PC absolute_z;
                absolute_z.value = p->velocity.z;
                absolute_z.bits &= 0x7fffffffu;
                if (absolute_z.value <= stop_speed) {
                    p->velocity.z = p->target_velocity.z = 0.0f;
                    p->packed_state_flags &= ~0xc0u;
                }
            } else {
                PUSH_ABSOLUTE_FLOAT_BITS_PC absolute_x;
                absolute_x.value = p->velocity.x;
                absolute_x.bits &= 0x7fffffffu;
                if (absolute_x.value <= stop_speed) {
                    p->target_velocity.x = p->velocity.x = 0.0f;
                    p->packed_state_flags &= ~0xc0u;
                }
            }
        }
        if ((p->packed_state_flags & 8u) != 0) {
            p->target_velocity.x = 0.0f;
            p->target_velocity.y = PushFallingSpeedPc * FRAMETIME;
            p->target_velocity.z = p->velocity.x = 0.0f;
            p->velocity.y = SeekValF(p->velocity.y, p->target_velocity.y, PushVelocitySeekRatePc);
            p->velocity.z = 0.0f;
        } else {
            p->target_velocity.y = 0.0f;
        }
    }
    for (i32 index = 0; index < world->count; ++index) {
        PUSH_RUNTIME_BLOCK_VIEW_PC *p = &world->blocks[index];
        if ((p->packed_state_flags & 0x40000u) != 0 && (p->packed_state_flags & 0x06000100u) == 0 &&
            (p->packed_state_flags & 0x69u) != 0)
            MoveBlocks(reinterpret_cast<WORLDINFO *>(world), reinterpret_cast<PUSH_BLOCK_PC *>(p), index, &p->velocity);
    }
    for (i32 index = 0; index < world->count; ++index) {
        PUSH_RUNTIME_BLOCK_VIEW_PC *p = &world->blocks[index];
        if ((p->packed_state_flags & 0x40000u) == 0 || (p->packed_state_flags & 0x02000000u) != 0)
            continue;
        if (NuSpecialGetVisibilityFn(&p->special) == 0 || (p->packed_state_flags & 0x4du) == 0)
            continue;
        PUSH_ABSOLUTE_FLOAT_BITS_PC absolute_left;
        absolute_left.value = p->bounds_min.x;
        absolute_left.bits &= 0x7fffffffu;
        f32 left = absolute_left.value + PushCornerInsetPc;
        PUSH_ABSOLUTE_FLOAT_BITS_PC absolute_right;
        absolute_right.value = p->bounds_max.x;
        absolute_right.bits &= 0x7fffffffu;
        f32 right = absolute_right.value + PushCornerInsetPc;
        PUSH_ABSOLUTE_FLOAT_BITS_PC absolute_back;
        absolute_back.value = p->bounds_min.z;
        absolute_back.bits &= 0x7fffffffu;
        f32 back = absolute_back.value + PushCornerInsetPc;
        PUSH_ABSOLUTE_FLOAT_BITS_PC absolute_front;
        absolute_front.value = p->bounds_max.z;
        absolute_front.bits &= 0x7fffffffu;
        f32 front = absolute_front.value + PushCornerInsetPc;
        NUVEC corners[4];
        for (i32 j = 0; j < 4; ++j)
            corners[j] = *p->position;
        PUSH_ABSOLUTE_FLOAT_BITS_PC absolute_y;
        absolute_y.value = p->bounds_min.y;
        absolute_y.bits &= 0x7fffffffu;
        f32 y = (corners[0].y - absolute_y.value) - PushGroundOffsetPc;
        for (i32 j = 0; j < 4; ++j)
            corners[j].y = (static_cast<f64>(0.025f)) + (static_cast<f64>(y));
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
        for (i32 j = 0; j < 4; ++j) {
            if (platforms_off)
                NewTerrPlatformsOff();
            heights[j] = GameShadow(NULL, &corners[j], 0.1f, -1);
            p->terrain_info[j] = static_cast<i8>(ShadowInfo());
            p->extra_terrain_info.bytes[j] = static_cast<i8>(EShadowInfo());
        }
        PlatOnOff(p->platform_id, 1);
        BlockInBlock(reinterpret_cast<WORLDINFO *>(world), reinterpret_cast<PUSH_BLOCK_PC *>(p), index,
                     &p->supporting_block);
        if (p->supporting_block != NULL || p->previous_supporting_block != NULL) {
            p->previous_supporting_block = p->supporting_block;
            f32 support[4];
            TerrainBlockOnBlock(reinterpret_cast<WORLDINFO *>(world), reinterpret_cast<PUSH_BLOCK_PC *>(p), corners,
                                support);
            for (i32 j = 0; j < 4; ++j)
                if (support[j] != -10000.0f)
                    heights[j] = support[j];
        } else {
            p->previous_supporting_block = NULL;
        }
        f64 low = static_cast<f64>(p->ground_height) - PushBlockGapEpsilonPc;
        f64 high = static_cast<f64>(p->ground_height) + PushBlockGapEpsilonPc;
        if ((heights[0] < low && heights[1] < low && heights[2] < low && heights[3] < low) ||
            (heights[0] > high && high < heights[1] && heights[2] > high && heights[3] > high)) {
            p->ground_height =
                ((static_cast<f64>(heights[0]) + heights[1]) + heights[2] + heights[3]) * 0.25 + PushBlockGapEpsilonPc;
            p->packed_state_flags |= 8u;
            p->ground_offset = (p->position->y + p->bounds_min.y) - p->ground_height;
        }
        if (p->supporting_block != NULL) {
            p->ground_height =
                p->position->y + reinterpret_cast<PUSH_RUNTIME_BLOCK_VIEW_PC *>(p->supporting_block)->bounds_max.y;
            p->ground_offset = (p->position->y + p->bounds_min.y) - p->ground_height;
        }
        if (p->ground_offset > 0.0f) {
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
    for (i32 index = 0; index < world->count; ++index) {
        PUSH_RUNTIME_BLOCK_VIEW_PC *p = &world->blocks[index];
        if ((p->packed_state_flags & 0x40000u) == 0 || (p->packed_state_flags & 0x02000100u) != 0)
            continue;
        if ((p->packed_state_flags & 0x400u) == 0 && (p->packed_state_flags & 1u) != 0)
            GameAudio_PlaySfx(72, p->position, GameAudio_GetPlrSfxBits(p->pushing_object), 0);
        if ((p->packed_state_flags & 0xc00u) != 0) {
            NUVEC *position = p->position;
            if ((p->packed_state_flags & 0x400u) != 0) {
                position->x = p->snap_origin.x;
                position->z = p->snap_origin.z;
                p->extra_terrain_info.word = p->previous_extra_terrain_info;
                p->packed_state_flags &= ~0xc0u;
                p->target_velocity.x = p->target_velocity.z = p->velocity.x = p->velocity.z = 0.0f;
                NuSpecialUpdate(&p->special);
                for (i32 j = 0; j < p->end_position_count; ++j) {
                    if (0 != NuSpecialExistsFn(&p->end_position_specials[j])) {
                        NUMTX *matrix = NuSpecialGetInstanceMtx(&p->end_position_specials[j]);
                        if (reinterpret_cast<u32>(matrix) != 0xffffffd0u) {
                            matrix->m30 = p->snap_origin.x;
                            matrix->m32 = p->snap_origin.z;
                            NuSpecialUpdate(&p->end_position_specials[j]);
                        }
                    }
                }
                p->packed_state_flags &= ~0x401u;
            }
            if ((p->packed_state_flags & 0x800u) != 0) {
                p->target_velocity.y = p->velocity.y = 0.0f;
                p->packed_state_flags &= ~0x808u;
                p->ground_height = p->settled_height;
                f32 settled_y = p->settled_height - p->bounds_min.y;
                position->y = settled_y;
                p->ground_offset = (settled_y + p->bounds_min.y) - p->ground_height;
                NuSpecialUpdate(&p->special);
            }
        } else if ((p->packed_state_flags & 1u) != 0) {
            if ((p->packed_state_flags & 2u) == 0 && p->pushing_object != NULL)
                GameAudio_PlaySfx(71, &p->pushing_object->collision_position,
                                  GameAudio_GetPlrSfxBits(p->pushing_object), 0);
            if (PushBlockHintIdPc != -1 && p->pushing_object != NULL &&
                (p->pushing_object->flags_1fc_word & 0x81u) == 0x81u)
                Hint_SetComplete(PushBlockHintIdPc);
        }
    }
    for (i32 index = 0; world->count > index; ++index) {
        PUSH_RUNTIME_BLOCK_VIEW_PC *p = &world->blocks[index];
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
        if ((p->packed_state_flags & 0x220u) == 0) {
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

static void *__cdecl PushBlocks_ReserveBufferSpace(void *world_ptr) {
    PUSH_RUNTIME_WORLD_VIEW_PC *world = static_cast<PUSH_RUNTIME_WORLD_VIEW_PC *>(world_ptr);
    PUSH_RUNTIME_LEVEL_VIEW_PC *level = world->current_level;
    world->blocks = NULL;
    world->count = 0;
    if (level->max_push_blocks != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3u) & ~3u;
        world->blocks = static_cast<PUSH_RUNTIME_BLOCK_VIEW_PC *>(world->giz_buffer.void_ptr);
        world->giz_buffer.addr += level->max_push_blocks * sizeof(PUSH_RUNTIME_BLOCK_VIEW_PC);
    }
    return world->blocks;
}

static i32 __cdecl Push_GetMaxGizmos(void *world_ptr) {
    PUSH_RUNTIME_WORLD_VIEW_PC *world = static_cast<PUSH_RUNTIME_WORLD_VIEW_PC *>(world_ptr);
    return world != NULL ? world->current_level->max_push_blocks : 0;
}

static void __cdecl Push_AddGizmos(void *system, i32 type_id, void *world_ptr, void *) {
    PUSH_RUNTIME_WORLD_VIEW_PC *world = static_cast<PUSH_RUNTIME_WORLD_VIEW_PC *>(world_ptr);
    for (i32 index = 0; index < world->count; ++index) {
        if (NuStrLen(world->blocks[index].name) != 0)
            AddGizmo(system, type_id, NULL, &world->blocks[index]);
    }
}

static char *__cdecl GizPush_GetGizmoName(GIZMO_PC *gizmo) {
    if (gizmo == NULL)
        return NULL;
    return static_cast<PUSH_RUNTIME_BLOCK_VIEW_PC *>(gizmo->object)->name;
}

static i32 __cdecl GizPush_GetOutput(GIZMO_PC *gizmo, i32 output_index, i32) {
    PUSH_RUNTIME_BLOCK_VIEW_PC *block = static_cast<PUSH_RUNTIME_BLOCK_VIEW_PC *>(gizmo->object);
    if (block == NULL)
        return 0;
    if (output_index == 0)
        return block->completed_snap_positions != 0;
    return (block->completed_snap_positions & static_cast<u8>(1u << (output_index & 31))) != 0;
}

static i32 __cdecl GizPush_GetNumOutputs(GIZMO_PC *gizmo) {
    return static_cast<PUSH_RUNTIME_BLOCK_VIEW_PC *>(gizmo->object)->output_count;
}

static void __cdecl Push_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    PUSH_RUNTIME_BLOCK_VIEW_PC *block = static_cast<PUSH_RUNTIME_BLOCK_VIEW_PC *>(gizmo->object);
    block->packed_state_flags = (block->packed_state_flags & ~0x40000u) | (visible != 0 ? 0x40000u : 0u);
    NuSpecialSetVisibility(&block->special, visible);
    for (i32 index = 0; index < block->end_position_count; ++index)
        NuSpecialSetVisibility(&block->end_position_specials[index], (block->packed_state_flags >> 18) & 1);
}

typedef struct PUSH_RUNTIME_PROGRESS_VIEW_PC {
    u32 visible_mask, state_mask, position_mask;
    NUVEC positions[16];
    NUVEC end_positions[2][16];
} PUSH_RUNTIME_PROGRESS_VIEW_PC;
STATIC_ASSERT(sizeof(PUSH_RUNTIME_PROGRESS_VIEW_PC) == 0x24c);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_PROGRESS_VIEW_PC, visible_mask) == 0);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_PROGRESS_VIEW_PC, state_mask) == 4);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_PROGRESS_VIEW_PC, position_mask) == 8);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_PROGRESS_VIEW_PC, positions) == 0xc);
STATIC_ASSERT(offsetof(PUSH_RUNTIME_PROGRESS_VIEW_PC, end_positions) == 0xcc);

static void *__cdecl Push_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, 0x24c);
}

static void __cdecl Push_ClearProgress(void *, void *progress_ptr) {
    PUSH_RUNTIME_PROGRESS_VIEW_PC *progress = static_cast<PUSH_RUNTIME_PROGRESS_VIEW_PC *>(progress_ptr);
    if (progress != NULL) {
        progress->state_mask = 0;
        progress->visible_mask = ~0u;
        progress->position_mask = 0;
        NuMemSet128(progress->positions, 0, 0);
    }
}

static i32 __cdecl edpush_Load(void *world_ptr, void *) {
    static i32 version = -1;
    PUSH_RUNTIME_WORLD_VIEW_PC *world = static_cast<PUSH_RUNTIME_WORLD_VIEW_PC *>(world_ptr);
    if (world->count != 0)
        return 0;
    version = EdFileReadInt();
    world->count = EdFileReadInt();
    i32 valid_count = 0;
    char special_name[256];
    for (i32 file_index = 0; file_index < world->count; ++file_index) {
        PUSH_RUNTIME_BLOCK_VIEW_PC *block = &world->blocks[valid_count];
        memset(special_name, 0, sizeof(special_name));
        EdFileRead(special_name, static_cast<i8>(EdFileReadChar()));
        i32 missing = 0;
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
                if (Gizmo_FindNuSpecial(world->current_gscn, &block->end_position_specials[index], special_name, 1,
                                        world->gizmo_sys, const_cast<char *>("PushBlockLinkObject"), block->name) == 0)
                    --block->end_position_count;
            }
        }
        if (missing == 0)
            ++valid_count;
    }
    world->count = valid_count;
    return 1;
}

static void __cdecl Push_Reset(void *world, void *, void *progress) {
    // Native5a6f2a passes (world,progress) to the currently unreconstructed5a4540.
    STUBBED_NAMED("ResetPushProgress_5a4540");
    (void)world;
    (void)progress;
}

static void __cdecl Push_Activate(GIZMO_PC *gizmo, i32 active) {
    if (gizmo == NULL)
        return;
    PUSH_RUNTIME_BLOCK_VIEW_PC *block = static_cast<PUSH_RUNTIME_BLOCK_VIEW_PC *>(gizmo->object);
    block->packed_state_flags = (block->packed_state_flags & ~0x01000000u) | (active != 0 ? 0x01000000u : 0u);
    if (active != 0) {
        PUSH_RUNTIME_WORLD_VIEW_PC *world = reinterpret_cast<PUSH_RUNTIME_WORLD_VIEW_PC *>(WORLD);
        const i32 count = world->count;
        PUSH_RUNTIME_BLOCK_VIEW_PC *candidate = world->blocks;
        i32 index;
        for (index = 0; index < count; ++index, ++candidate)
            if (candidate == block)
                break;
        if (index < count) {
            // Native5a6fa0 passes (world,block,index), not a guessed reset or returned value.
            STUBBED_NAMED("ResetSinglePushBlock_5a4450");
        }
        block->packed_state_flags |= 0x00040000u;
    }
}

static char outputName[13] = "Any Complete";

static char *__cdecl GizPush_GetOutputName(GIZMO_PC *gizmo, i32 output_index) {
    if (output_index < 0)
        return NULL;
    PUSH_RUNTIME_BLOCK_VIEW_PC *block = static_cast<PUSH_RUNTIME_BLOCK_VIEW_PC *>(gizmo->object);
    if (output_index > block->output_count)
        return NULL;
    if (output_index == 0)
        return const_cast<char *>("Any Complete");
    NuIToA(output_index, outputName, 10);
    NuStrCat(outputName, " Complete");
    return outputName;
}

static void __cdecl Push_StoreProgress(void *world_ptr, void *, void *progress_ptr) {
    PUSH_RUNTIME_PROGRESS_VIEW_PC *progress = static_cast<PUSH_RUNTIME_PROGRESS_VIEW_PC *>(progress_ptr);
    if (progress == NULL)
        return;
    // This initialization is the native inline ClearProgress operation, including its zero length.
    progress->state_mask = 0;
    progress->visible_mask = ~0u;
    progress->position_mask = 0;
    NuMemSet128(progress->positions, 0, 0);
    PUSH_RUNTIME_WORLD_VIEW_PC *world = static_cast<PUSH_RUNTIME_WORLD_VIEW_PC *>(world_ptr);
    if (world == NULL || world->blocks == NULL || world->count <= 0)
        return;
    PUSH_RUNTIME_BLOCK_VIEW_PC *block = world->blocks;
    for (i32 index = 0; index < world->count && index < 16; ++index, ++block) {
        const u32 mask = 1u << (index & 31);
        if ((block->packed_state_flags & 0x40000u) == 0)
            progress->visible_mask &= ~mask;
        if ((block->packed_state_flags & 0x02000000u) != 0)
            progress->state_mask |= mask;
        if (NuSpecialExistsFn(&block->special) != 0) {
            NUMTX *matrix = NuSpecialGetInstanceMtx(&block->special);
            progress->positions[index].x = matrix->m30;
            progress->positions[index].y = matrix->m31;
            progress->positions[index].z = matrix->m32;
        } else {
            progress->positions[index] = v000;
        }
        for (i32 end = 0; end < block->end_position_count; ++end) {
            NUSPECIALHANDLE *special = &block->end_position_specials[end];
            if (special != NULL) {
                if (NuSpecialExistsFn(special) != 0) {
                    NUMTX *matrix = NuSpecialGetInstanceMtx(special);
                    progress->end_positions[end][index].x = matrix->m30;
                    progress->end_positions[end][index].y = matrix->m31;
                    progress->end_positions[end][index].z = matrix->m32;
                } else {
                    progress->end_positions[end][index] = v000;
                }
            }
        }
        progress->position_mask |= mask;
    }
}

STATIC_ASSERT(offsetof(BOLT_PC, owner) == 4);
STATIC_ASSERT(offsetof(BOLT_PC, hit_platform) == 0xfa);

static i32 __cdecl Pushblocks_BoltHitPlat(void *world_ptr, void *blocks_ptr, BOLT_PC *bolt, u8 *) {
    PUSH_RUNTIME_WORLD_VIEW_PC *world = static_cast<PUSH_RUNTIME_WORLD_VIEW_PC *>(world_ptr);
    PUSH_RUNTIME_BLOCK_VIEW_PC *block = static_cast<PUSH_RUNTIME_BLOCK_VIEW_PC *>(blocks_ptr);
    if (block != NULL) {
        for (i32 index = 0; index < world->count; ++index, ++block) {
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
                    if (direction.z < 0.0f) {
                        direction.x = -(static_cast<f64>(qrand()) * 0.000015259021893143654);
                        direction.z = -(static_cast<f64>(qrand()) * 0.000015259021893143654);
                    } else {
                        direction.x = static_cast<f64>(qrand()) * 0.000015259021893143654;
                        direction.z = static_cast<f64>(qrand()) * 0.000015259021893143654;
                    }
                }
                PUSH_ABSOLUTE_FLOAT_BITS_PC absolute_x, absolute_z;
                absolute_x.value = direction.x;
                absolute_x.bits &= 0x7fffffffu;
                absolute_z.value = direction.z;
                absolute_z.bits &= 0x7fffffffu;
                if (absolute_x.value > absolute_z.value)
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

i32 pushblock_gizmotype_id;

ADDGIZMOTYPE_PC *__cdecl Push_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC addtype;
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
