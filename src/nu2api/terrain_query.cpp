#include "saga.h"

extern "C" {

#define MIN(a, b) ((a) < (b) ? (a) : (b))
    extern i32 WallSplinesOnly, IgnoreWallSplines, WallSplCount, curPickInst;
    extern void *TerI, *TerrOverRideScan;
    extern TERRAIN_POLYGON_INFO_PC *TerrainScaledShapeCursorPc;
    void __cdecl ScanWallSplineTerrain(i32, i32, i32);
    f32 __cdecl TerrainDivideNonzeroPC(f32, f32);
    void __cdecl TerrainSkinAllocate(TERRAIN_GROUP_PC *);
    enum { TERRAIN_PLATFORM_FLAG_ROTATING = 1 };
}

#include <string.h>
#include <math.h>
#define MAX(a, b) ((a) > (b) ? (a) : (b))
enum {
    NUTRIGTABLE_COUNT = 32768,
    TERRAIN_HIT_TYPE_NONE = 0,
    TERRAIN_HIT_TYPE_FACE = 1,
    TERRAIN_HIT_TYPE_CYLINDER = 2,
    TERRAIN_HIT_TYPE_VERTEX = 3,
    TERRAIN_HIT_TYPE_SPHERE = 4,
    TERRAIN_HIT_TYPE_SECOND_NORMAL = 16,
    TERRAIN_HIT_TYPE_CLASS_MASK = 15,
    TERRAIN_TRACK_FLAG_CONNECTED = 1,
    TERRAIN_TRACK_CONTACT_ACTIVE = 4,
    TERRAIN_QUERY_FLAG_PREVIOUS_NORMAL = 1,
    TERRAIN_SURFACE_CLASS_MASK = 0x7c,
    TERRAIN_SURFACE_CLASS_WALL_OVERRIDE = 4,
    TERRAIN_IMPACT_RESULT_WALL = 1,
    TERRAIN_IMPACT_RESULT_GROUND = 2,
    TERRAIN_IMPACT_RESULT_STATIC_WALL = 16
};
enum {
    TERRAIN_HIT_EDGE_0_1 = 1,
    TERRAIN_HIT_EDGE_2_0 = 3,
    TERRAIN_HIT_EDGE_1_2 = 2,
    TERRAIN_HIT_EDGE_1_3 = 4,
    TERRAIN_HIT_EDGE_3_2 = 5,
    TERRAIN_TRACK_FLAG_NONE = 0,
    TERRAIN_PLATFORM_FLAG_COLLIDED = 2
};
struct TERRAIN_IMPACT_RECORD {
    NUVEC position, normal;
    u8 material[2], flags, normal_flags;
};
typedef TERRAIN_SPHERE_PC TERRAIN_SPHERE;
typedef TERRAIN_AXIS_FREEDOM_SHAPE_PC TERRAIN_AXIS_FREEDOM_SHAPE;
extern "C" {
    extern TERRAIN_SHAPE TerrPolyInfo, PlatImpactTer;
    extern i32 TerrainShadowStatusPc, PlatCrush, plathitid, terrhitflags;
    extern TERRAIN_TRACK_SLOT_PC *CurTrackInfo;
    extern TERRAIN_AXIS_FREEDOM_SHAPE *TerrShape;
    extern NUVEC ShadNorm, PlatImpactNorm, TerrWallNorm;
    extern u8 TerrWallTab[4];
    extern void *TerImpactData;
    extern i32 *TerImpactDataCount;
    extern i32 TerImpactDataMax;
    extern f32 wallover, TerConTol, TerrWallDeflectYScale;
    extern const f64 TerrainOpposingContactThresholdPc;
    extern NUMTX tertempmat;
    extern NUVEC4 tertempvec4;
    extern TERRAIN_SPHERE SphereData[];
    void DerotateMovementVector();
    void RotateVec(NUVEC *source, NUVEC *destination);
    void DeRotateTerrain(TERRAIN_SHAPE *surface);
    void DeRotatePoint(NUVEC *point);
    i32 CheckCylinder(i32 first_vertex, i32 second_vertex, i32 *vertex_mask, i32 remaining_vertex_mask);
    i32 CheckSphere(i32 vertex_index);
    i32 CheckSphereTer(NUVEC *position, f32 radius);
    i32 HitPoly(f32 primary_start, f32 primary_end, f32 secondary_start, f32 secondary_end, TERRAIN_SHAPE *surface);
    i32 HitWallSpline();
    i32 HitTerrain();
    void NewTerrStoreAnyInfo();
    void TerrainMoveImpactData();
    void TerrainImpactNorm();
    void StorePlatImpact();
    void TerrainImpact(NUVEC *position, NUVEC *movement, u8 *hit_flags);
    i32 TerrainImpactPlatform(unsigned char *hit_flags);
    void FullDeflect(NUVEC *normal, NUVEC *movement, NUVEC *result);
    void FullDeflectSmallY(NUVEC *normal, NUVEC *movement, NUVEC *result);
    i32 FullDeflectTest(NUVEC *normal, NUVEC *movement, NUVEC *result);
    void PlatformConnect(char *track_id, NUVEC *position_delta, NUVEC *movement_delta, i32 platform_index);
    i32 PlatformChecks(i32 count, NUVEC *movement);
    i32 TerrainPlatformEmbedded(NUVEC *movement_delta);
    i32 TerrainPlatformMoveCheck(NUVEC *position, NUVEC *normal, i32 platform_index, i32 resolve_impact,
                                 i32 keep_embedded_disabled);
    i32 TerrShapeSideStep(NUVEC *, NUVEC *, u8 *);
    void TerrainSideClamp(NUVEC *axis, NUVEC *position);
    i16 InsidePolLines(f32 point_x, f32 point_y, f32 point_z, f32 edge_a_x, f32 edge_a_y, f32 edge_a_z, f32 edge_b_x,
                       f32 edge_b_y, f32 edge_b_z, NUVEC *normal);
    i16 InsideLineF(f32 point_u, f32 point_v, f32 line_start_u, f32 line_start_v, f32 line_end_u, f32 line_end_v);
    TERRAIN_TRACK_SLOT_PC *AllocTerrId();
}

struct ShadowScanWriter {
    u8 *group_header;
    TERRAIN_SHAPE **cursor;
    u8 *limit;
    i32 shape_count;
};
extern "C" {
    extern i32 TerrPlatDis;
}
#define TERRAIN_BOUNDS_OVERLAP(bounds_arg, minimum_arg, maximum_arg)                                                   \
    ((bounds_arg).max_x >= (minimum_arg).x && (bounds_arg).max_y >= (minimum_arg).y &&                                 \
     (bounds_arg).max_z >= (minimum_arg).z && (maximum_arg).x >= (bounds_arg).min_x &&                                 \
     (maximum_arg).y > (bounds_arg).min_y && (maximum_arg).z > (bounds_arg).min_z)
#define TERRAIN_SHAPE_OVERLAPS(bounds_arg, shape_arg)                                                                  \
    ((bounds_arg).max_x >= (shape_arg).min_x && (shape_arg).max_x > (bounds_arg).min_x &&                              \
     (bounds_arg).max_y >= (shape_arg).min_y && (shape_arg).max_y > (bounds_arg).min_y &&                              \
     (bounds_arg).max_z >= (shape_arg).min_z && (shape_arg).max_z > (bounds_arg).min_z)
#define TERRAIN_HANDLE_VERTICES_OVERLAP(bounds_arg, shape_arg)                                                         \
    (((shape_arg).vertices[0].x > (bounds_arg).min_x || (shape_arg).vertices[1].x > (bounds_arg).min_x ||              \
      (shape_arg).vertices[2].x > (bounds_arg).min_x || (shape_arg).vertices[3].x > (bounds_arg).min_x) &&             \
     ((shape_arg).vertices[0].x < (bounds_arg).max_x || (shape_arg).vertices[1].x < (bounds_arg).max_x ||              \
      (shape_arg).vertices[2].x < (bounds_arg).max_x || (shape_arg).vertices[3].x < (bounds_arg).max_x) &&             \
     ((shape_arg).vertices[0].z > (bounds_arg).min_z || (shape_arg).vertices[1].z > (bounds_arg).min_z ||              \
      (shape_arg).vertices[2].z > (bounds_arg).min_z || (shape_arg).vertices[3].z > (bounds_arg).min_z) &&             \
     ((shape_arg).vertices[0].z < (bounds_arg).max_z || (shape_arg).vertices[1].z < (bounds_arg).max_z ||              \
      (shape_arg).vertices[2].z < (bounds_arg).max_z || (shape_arg).vertices[3].z < (bounds_arg).max_z))
#define TERRAIN_HANDLE_WALL_OVERLAP(bounds_arg, wall_arg)                                                              \
    ((((wall_arg)[0].position.x >= (bounds_arg).min_x && (wall_arg)[1].position.x <= (bounds_arg).max_x) ||            \
      ((wall_arg)[1].position.x >= (bounds_arg).min_x && (wall_arg)[0].position.x <= (bounds_arg).max_x)) &&           \
     (((wall_arg)[0].position.z >= (bounds_arg).min_z && (wall_arg)[1].position.z <= (bounds_arg).max_z) ||            \
      ((wall_arg)[1].position.z >= (bounds_arg).min_z && (wall_arg)[0].position.z <= (bounds_arg).max_z)))
#define SHADOW_BOUNDS_OVERLAP(min_x_arg, min_z_arg, max_x_arg, max_z_arg, minimum_arg, maximum_arg)                    \
    ((max_x_arg) >= (minimum_arg).x && (maximum_arg).x >= (min_x_arg) && (max_z_arg) >= (minimum_arg).z &&             \
     (maximum_arg).z > (min_z_arg))
#define TERRAIN_FINISH_SCAN_GROUP(w_arg, g_arg)                                                                        \
    do {                                                                                                               \
        TerrainScanWriter *w = (w_arg);                                                                                \
        if (w->group_shape_count) {                                                                                    \
            TERRAIN_SCAN_WORD_PC *h = (TERRAIN_SCAN_WORD_PC *)w->group_header;                                         \
            h->header.shape_count = (i16)w->group_shape_count;                                                         \
            h->header.terrain_group = (i16)(g_arg);                                                                    \
            w->group_header = (u8 *)w->cursor;                                                                         \
            w->cursor++;                                                                                               \
            w->group_shape_count = 0;                                                                                  \
        }                                                                                                              \
    } while (0)
#define SHADOW_FINISH_GROUP(w_arg, g_arg)                                                                              \
    do {                                                                                                               \
        ShadowScanWriter *w = (w_arg);                                                                                 \
        if (w->shape_count) {                                                                                          \
            TERRAIN_SCAN_WORD_PC *h = (TERRAIN_SCAN_WORD_PC *)w->group_header;                                         \
            h->header.shape_count = (i16)w->shape_count;                                                               \
            h->header.terrain_group = (i16)(g_arg);                                                                    \
            w->group_header = (u8 *)w->cursor;                                                                         \
            w->cursor++;                                                                                               \
            w->shape_count = 0;                                                                                        \
        }                                                                                                              \
    } while (0)
#define TERRAIN_HANDLE_FILTER_SHAPES(writer_arg, shapes_arg, count_arg, bounds_arg, terrain_mask_arg, rotating_arg)    \
    do {                                                                                                               \
        for (i32 i = 0; i < (count_arg); ++i) {                                                                        \
            TERRAIN_SHAPE *shape = (shapes_arg)[i];                                                                    \
            if (!(rotating_arg) && !TERRAIN_SHAPE_OVERLAPS((bounds_arg), *shape))                                      \
                continue;                                                                                              \
            if (reinterpret_cast<u8 *>((writer_arg)->cursor) >= (writer_arg)->limit)                                   \
                continue;                                                                                              \
            if (shape->material[1] != 0 && (shape->material[1] & (terrain_mask_arg)) == 0)                             \
                continue;                                                                                              \
            if ((rotating_arg) && !TERRAIN_HANDLE_VERTICES_OVERLAP((bounds_arg), *shape))                              \
                continue;                                                                                              \
            *(writer_arg)->cursor++ = shape;                                                                           \
            ++(writer_arg)->group_shape_count;                                                                         \
        }                                                                                                              \
    } while (0)
extern "C" {
#define TERRAIN_NS_SET (static_cast<TERRAIN_SET_PC *>(CurrentTerrain))
#define TERRAIN_NS_PLATFORM(group_ptr) (TERRAIN_NS_SET->platforms[(group_ptr)->scene_index])
#define TERRAIN_NS_MATRIX(group_ptr) (static_cast<NUMTX *>(TERRAIN_NS_PLATFORM(group_ptr).scene_object))
#define TERRAIN_NS_GROUP_OVERLAP(lo_x, lo_z, hi_x, hi_z, group_ptr)                                                    \
    ((hi_x) >= (group_ptr)->bounds_min.x && (hi_z) >= (group_ptr)->bounds_min.z &&                                     \
     (group_ptr)->bounds_max.x >= (lo_x) && (group_ptr)->bounds_max.z > (lo_z))
#define TERRAIN_NS_SHAPE_OVERLAP(shape_ptr)                                                                            \
    (local_max_x >= (shape_ptr)->min_x && (shape_ptr)->max_x > local_min_x && local_max_z >= (shape_ptr)->min_z &&     \
     (shape_ptr)->max_z > local_min_z)
#define TERRAIN_NS_BATCH_OVERLAP(batch_ptr)                                                                            \
    (local_max_x >= (batch_ptr)->min_x && (batch_ptr)->max_x > local_min_x && local_max_z >= (batch_ptr)->min_z &&     \
     (batch_ptr)->max_z > local_min_z)
#define TERRAIN_NS_MATERIAL_OK(shape_ptr)                                                                              \
    ((shape_ptr)->material[1] == 0 || ((shape_ptr)->material[1] & terrain_mask) != 0)
#define TERRAIN_NS_FINISH_GROUP(group_index_arg)                                                                       \
    do {                                                                                                               \
        if (shape_count != 0) {                                                                                        \
            group_header->header.shape_count = (i16)shape_count;                                                       \
            group_header->header.terrain_group = (i16)(group_index_arg);                                               \
            group_header = cursor;                                                                                     \
            cursor++;                                                                                                  \
            shape_count = 0;                                                                                           \
        }                                                                                                              \
    } while (0)
#define TERRAIN_NS_LOCAL_BOUNDS(group_ptr)                                                                             \
    do {                                                                                                               \
        local_max_x = max_x - (group_ptr)->origin.x;                                                                   \
        local_max_z = max_z - (group_ptr)->origin.z;                                                                   \
        local_min_x = min_x - (group_ptr)->origin.x;                                                                   \
        local_min_z = min_z - (group_ptr)->origin.z;                                                                   \
    } while (0)
#define TERRAIN_NS_CACHED_SHAPES(entry_var)                                                                            \
    do {                                                                                                               \
        for (;;) {                                                                                                     \
            const i16 count = (entry_var)->header.shape_count;                                                         \
            if (count <= 0)                                                                                            \
                break;                                                                                                 \
            const i32 group_index = (entry_var)->header.terrain_group;                                                 \
            TERRAIN_SCAN_WORD_PC *shapes = (entry_var) + 1;                                                            \
            TERRAIN_GROUP_PC *group = &TERRAIN_NS_SET->groups[group_index];                                            \
            if (TERRAIN_NS_GROUP_OVERLAP(min_x, min_z, max_x, max_z, group) && group->chunk_type != -1) {              \
                TERRAIN_NS_LOCAL_BOUNDS(group);                                                                        \
                for (i32 remaining = count; remaining > 0; --remaining, ++shapes) {                                    \
                    TERRAIN_SHAPE *shape = shapes->shape;                                                              \
                    if (TERRAIN_NS_SHAPE_OVERLAP(shape) && cursor < limit && TERRAIN_NS_MATERIAL_OK(shape)) {          \
                        (cursor++)->shape = shape;                                                                     \
                        ++shape_count;                                                                                 \
                    }                                                                                                  \
                }                                                                                                      \
                TERRAIN_NS_FINISH_GROUP(group_index);                                                                  \
            } else {                                                                                                   \
                shapes += count;                                                                                       \
            }                                                                                                          \
            (entry_var) = shapes;                                                                                      \
        }                                                                                                              \
    } while (0)
    void NewScan(NUVEC *position, i32 terrain_mask, i32 scan_platforms) {
        NUVEC saved_position = *position;
        i32 hit_index = -1;
        i32 oldest_index = -1;
        i32 oldest_age = 0x7fffffff;
        TERRAIN_SCAN_WORD_PC *group_header;
        TERRAIN_SCAN_WORD_PC *cursor;
        TERRAIN_SCAN_WORD_PC *limit;
        TERRAIN_SCAN_WORD_PC *entry;
        i32 shape_count;
        f32 max_x, max_z, min_x, min_z;
        f32 local_max_x, local_max_z, local_min_x, local_min_z;

        for (i32 i = 0; i < 16; ++i) {
            TERRAIN_INDEX_LEVEL_PC *level = &TERRAIN_NS_SET->index_levels[i];
            const i16 age = level->cache_age;
            if (age > 0) {
                const f64 dx = (saved_position.x + 1.0) - level->center_x;
                if (dx < 2.0) {
                    const f64 dz = (saved_position.z + 1.0) - level->center_z;
                    if (dz < 2.0 && dx > 0.0 && dz > 0.0) {
                        hit_index = i;
                        break;
                    }
                }
            }
            if (age < oldest_age) {
                oldest_age = age;
                oldest_index = i;
            }
        }

        if (hit_index >= 0) {
            group_header = static_cast<TerrainQuery_s *>(TerI)->scan_words;
            max_x = (f32)(saved_position.x + 0.1f);
            max_z = (f32)(saved_position.z + 0.1f);
            TERRAIN_NS_SET->index_levels[hit_index].cache_age = 8;
            min_x = (f32)(saved_position.x - 0.1f);
            min_z = (f32)(saved_position.z - 0.1f);
            entry = TERRAIN_NS_SET->index_levels[hit_index].scan_words;
            cursor = group_header + 1;
            shape_count = 0;
            limit = cursor + 508;
            TERRAIN_NS_CACHED_SHAPES(entry);
        } else {
            max_x = saved_position.x + 1.0f;
            max_z = saved_position.z + 1.0f;
            group_header = TERRAIN_NS_SET->index_levels[oldest_index].scan_words;
            cursor = group_header + 1;
            min_x = saved_position.x - 1.0f;
            limit = cursor + 508;
            min_z = saved_position.z - 1.0f;
            shape_count = 0;
            for (i32 cell_index = 0; cell_index < TERRAIN_NS_SET->used_cell_count; ++cell_index) {
                if (!(max_x >= TERRAIN_NS_SET->cells[cell_index].min_x &&
                      max_z >= TERRAIN_NS_SET->cells[cell_index].min_z &&
                      TERRAIN_NS_SET->cells[cell_index].max_x >= min_x &&
                      TERRAIN_NS_SET->cells[cell_index].max_z >= min_z)) {
                    continue;
                }
                i16 *group_indices = TERRAIN_NS_SET->group_indices + TERRAIN_NS_SET->cells[cell_index].first_group;
                for (i32 cell_group = 0; cell_group < TERRAIN_NS_SET->cells[cell_index].group_count;
                     ++cell_group, ++group_indices) {
                    TERRAIN_GROUP_PC *group = &TERRAIN_NS_SET->groups[*group_indices];
                    if (!TERRAIN_NS_GROUP_OVERLAP(min_x, min_z, max_x, max_z, group) || group->chunk_type == -1) {
                        continue;
                    }
                    TERRAIN_NS_LOCAL_BOUNDS(group);
                    if (group->scene_index < 0) {
                        TerrainSkinAllocate(group);
                    }
                    TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
                    while (batch->marker >= 0) {
                        const i16 count = batch->shape_count;
                        TERRAIN_SHAPE *shape = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                        if (TERRAIN_NS_BATCH_OVERLAP(batch)) {
                            i32 remaining = count;
                            while (remaining > 0) {
                                if (TERRAIN_NS_SHAPE_OVERLAP(shape) && cursor < limit) {
                                    (cursor++)->shape = shape;
                                    ++shape_count;
                                }
                                ++shape;
                                --remaining;
                            }
                        } else {
                            shape += count;
                        }
                        batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shape);
                    }
                    TERRAIN_NS_FINISH_GROUP(*group_indices);
                }
            }
            TERRAIN_NS_SET->index_levels[oldest_index].cache_age = 8;
            TERRAIN_NS_SET->index_levels[oldest_index].center_x = position->x;
            TERRAIN_NS_SET->index_levels[oldest_index].center_z = position->z;
            group_header->header.shape_count = 0;
            group_header->header.terrain_group = 0;
            shape_count = 0;
            group_header = static_cast<TerrainQuery_s *>(TerI)->scan_words;
            cursor = group_header + 1;
            limit = cursor + 508;
            max_x = (f32)(saved_position.x + 0.1f);
            max_z = (f32)(saved_position.z + 0.1f);
            min_x = (f32)(saved_position.x - 0.1f);
            min_z = (f32)(saved_position.z - 0.1f);
            entry = TERRAIN_NS_SET->index_levels[oldest_index].scan_words;
            TERRAIN_NS_CACHED_SHAPES(entry);
        }

        min_x = (f32)(min_x - 0.05f);
        max_x = (f32)(max_x + 0.05f);
        min_z = (f32)(min_z - 0.05f);
        max_z = (f32)(max_z + 0.05f);
        if (scan_platforms != 0) {
            i16 *platform_groups = TERRAIN_NS_SET->group_indices + TERRAIN_NS_SET->platform_cell.first_group;
            for (i32 platform_index = 0; platform_index < TERRAIN_NS_SET->platform_cell.group_count;
                 ++platform_index, ++platform_groups) {
                TERRAIN_GROUP_PC *group = &TERRAIN_NS_SET->groups[*platform_groups];
                TERRAIN_PLATFORM_PC *platform = &TERRAIN_NS_PLATFORM(group);
                if (platform->instance_flags != NULL) {
                    u32 *scene_state = platform->instance_flags;
                    if ((platform->flags & TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED) != 0) {
                        const u32 state_flags = *scene_state;
                        if ((state_flags & 2) == 0) {
                            continue;
                        }
                    } else if ((*scene_state & 1) == 0) {
                        continue;
                    }
                }
                if (platform->scene_object != NULL) {
                    if (TERRAIN_NS_MATRIX(group)->m30 > TERRAIN_NS_PLATFORM(group).previous_matrix.m30) {
                        local_min_x = min_x - group->origin.x;
                        local_max_x =
                            (f32)((TERRAIN_NS_MATRIX(group)->m30 - TERRAIN_NS_PLATFORM(group).previous_matrix.m30) *
                                      1.5 +
                                  max_x - group->origin.x);
                    } else {
                        local_max_x = max_x - group->origin.x;
                        local_min_x =
                            (f32)((TERRAIN_NS_MATRIX(group)->m30 - TERRAIN_NS_PLATFORM(group).previous_matrix.m30) *
                                      1.5 +
                                  min_x - group->origin.x);
                    }
                    if (TERRAIN_NS_MATRIX(group)->m32 > TERRAIN_NS_PLATFORM(group).previous_matrix.m32) {
                        local_min_z = min_z - group->origin.z;
                        local_max_z =
                            (f32)((TERRAIN_NS_MATRIX(group)->m32 - TERRAIN_NS_PLATFORM(group).previous_matrix.m32) *
                                      1.5 +
                                  max_z - group->origin.z);
                    } else {
                        local_max_z = max_z - group->origin.z;
                        local_min_z =
                            (f32)((TERRAIN_NS_MATRIX(group)->m32 - TERRAIN_NS_PLATFORM(group).previous_matrix.m32) *
                                      1.5 +
                                  min_z - group->origin.z);
                    }
                } else {
                    local_max_x = max_x - group->origin.x;
                    local_min_x = min_x - group->origin.x;
                    local_max_z = max_z - group->origin.z;
                    local_min_z = min_z - group->origin.z;
                }
                if (!TERRAIN_NS_GROUP_OVERLAP(local_min_x, local_min_z, local_max_x, local_max_z, group) ||
                    group->chunk_type == -1) {
                    continue;
                }
                TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
                while (batch->marker >= 0) {
                    TERRAIN_SHAPE *shape = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                    const i32 count = batch->shape_count;
                    if (TERRAIN_NS_BATCH_OVERLAP(batch)) {
                        for (i32 remaining = count; remaining > 0; --remaining, ++shape) {
                            if (TERRAIN_NS_SHAPE_OVERLAP(shape) && cursor < limit && TERRAIN_NS_MATERIAL_OK(shape)) {
                                (cursor++)->shape = shape;
                                ++shape_count;
                            }
                        }
                    } else {
                        shape += count;
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shape);
                }
                TERRAIN_NS_FINISH_GROUP(*platform_groups);
            }
        }
        group_header->header.shape_count = 0;
        group_header->header.terrain_group = 0;
    }
#undef TERRAIN_NS_SET
#undef TERRAIN_NS_PLATFORM
#undef TERRAIN_NS_MATRIX
#undef TERRAIN_NS_GROUP_OVERLAP
#undef TERRAIN_NS_SHAPE_OVERLAP
#undef TERRAIN_NS_BATCH_OVERLAP
#undef TERRAIN_NS_MATERIAL_OK
#undef TERRAIN_NS_FINISH_GROUP
#undef TERRAIN_NS_LOCAL_BOUNDS
#undef TERRAIN_NS_CACHED_SHAPES
#define TERRAIN_STH_SET (static_cast<TERRAIN_SET_PC *>(CurrentTerrain))
#define TERRAIN_STH_QUERY (static_cast<TerrainQuery_s *>(TerI))
#define TERRAIN_STH_PLATFORM(group_ptr) (TERRAIN_STH_SET->platforms[(group_ptr)->scene_index])
#define TERRAIN_STH_MATRIX(group_ptr) (static_cast<NUMTX *>(TERRAIN_STH_PLATFORM(group_ptr).scene_object))
#define TERRAIN_STH_GROUP_OVERLAP(lo_x, lo_y, lo_z, hi_x, hi_y, hi_z, group_ptr)                                       \
    ((hi_x) >= (group_ptr)->bounds_min.x && (hi_y) >= (group_ptr)->bounds_min.y &&                                     \
     (hi_z) >= (group_ptr)->bounds_min.z && (group_ptr)->bounds_max.x >= (lo_x) &&                                     \
     (group_ptr)->bounds_max.y > (lo_y) && (group_ptr)->bounds_max.z > (lo_z))
#define TERRAIN_STH_SHAPE_OVERLAP(shape_ptr)                                                                           \
    (local_max_x >= (shape_ptr)->min_x && (shape_ptr)->max_x > local_min_x && local_max_z >= (shape_ptr)->min_z &&     \
     (shape_ptr)->max_z > local_min_z && local_max_y >= (shape_ptr)->min_y && (shape_ptr)->max_y > local_min_y)
#define TERRAIN_STH_MATERIAL_OK(shape_ptr)                                                                             \
    ((shape_ptr)->material[1] == 0 || ((shape_ptr)->material[1] & terrain_mask) != 0)
#define TERRAIN_STH_VERTICES_OVERLAP(shape_ptr)                                                                        \
    (((shape_ptr)->vertices[0].x > local_min_x || (shape_ptr)->vertices[1].x > local_min_x ||                          \
      (shape_ptr)->vertices[2].x > local_min_x || (shape_ptr)->vertices[3].x > local_min_x) &&                         \
     ((shape_ptr)->vertices[0].x < local_max_x || (shape_ptr)->vertices[1].x < local_max_x ||                          \
      (shape_ptr)->vertices[2].x < local_max_x || (shape_ptr)->vertices[3].x < local_max_x) &&                         \
     ((shape_ptr)->vertices[0].z > local_min_z || (shape_ptr)->vertices[1].z > local_min_z ||                          \
      (shape_ptr)->vertices[2].z > local_min_z || (shape_ptr)->vertices[3].z > local_min_z) &&                         \
     ((shape_ptr)->vertices[0].z < local_max_z || (shape_ptr)->vertices[1].z < local_max_z ||                          \
      (shape_ptr)->vertices[2].z < local_max_z || (shape_ptr)->vertices[3].z < local_max_z))
#define TERRAIN_STH_FINISH_GROUP(group_index_arg)                                                                      \
    do {                                                                                                               \
        if (shape_count != 0) {                                                                                        \
            group_header->header.shape_count = (i16)shape_count;                                                       \
            group_header->header.terrain_group = (i16)(group_index_arg);                                               \
            group_header = cursor;                                                                                     \
            cursor++;                                                                                                  \
            shape_count = 0;                                                                                           \
        }                                                                                                              \
    } while (0)
#define TERRAIN_STH_FILTER_SHAPES()                                                                                    \
    do {                                                                                                               \
        for (i32 remaining = handle[0]; remaining > 0; --remaining, ++shapes) {                                        \
            TERRAIN_SHAPE *shape = shapes->shape;                                                                      \
            if (TERRAIN_STH_SHAPE_OVERLAP(shape) && cursor < limit && TERRAIN_STH_MATERIAL_OK(shape)) {                \
                (cursor++)->shape = shape;                                                                             \
                ++shape_count;                                                                                         \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)
    void ScanTerrainHandel(i32 terrain_mask, i16 *handle) {
        TERRAIN_SCAN_WORD_PC *group_header;
        TERRAIN_SCAN_WORD_PC *cursor;
        TERRAIN_SCAN_WORD_PC *limit;
        i32 shape_count;
        f32 max_x, min_x, max_z, min_z, max_y, min_y;
        f32 scan_radius;
        TERRAIN_WALL_POINT **walls;
        TERRAIN_WALL_POINT *wall_list;
        i32 wall_count;
        TerrainQuery_s *query;

        if (handle == NULL) {
            return;
        }
        query = TERRAIN_STH_QUERY;
        group_header = query->scan_words;
        cursor = query->scan_words + 1;
        limit = cursor + 508;
        platinrange = 0;
        query->scan_group_index = -1;
        switch (query->scan_result) {
            default: {
                const f32 movement_sq = query->movement.x * query->movement.x + query->movement.y * query->movement.y +
                                        query->movement.z * query->movement.z;
                const f32 reach = (f32)(0.02f + query->collision_radius_sq + movement_sq);
                max_x = query->position.x + reach;
                max_y = query->position.y + query->object_scale * reach;
                max_z = query->position.z + reach;
                min_x = query->position.x - reach;
                min_y = query->position.y - query->object_scale * reach;
                min_z = query->position.z - reach;
                scan_radius = NuFsqrt((f32)(reach + 1.0));
                break;
            }
            case 1: {
                f32 extent_sq;
                if (query->movement.x > 0.0f) {
                    min_x = (f32)(query->position.x - 0.02f - query->collision_radius);
                    max_x = (f32)(query->movement.x + query->position.x + 0.02f + query->collision_radius);
                } else {
                    min_x = (f32)(query->movement.x + query->position.x - 0.02f - query->collision_radius);
                    max_x = (f32)(query->position.x + 0.02f + query->collision_radius);
                }
                if (query->movement.y > 0.0f) {
                    min_y = (f32)(query->position.y - 0.02f - query->collision_radius);
                    max_y = (f32)(query->movement.y + query->position.y + 0.02f + query->collision_radius);
                } else {
                    min_y = (f32)(query->movement.y + query->position.y - 0.02f - query->collision_radius);
                    max_y = (f32)(query->position.y + 0.02f + query->collision_radius);
                }
                if (query->movement.z > 0.0f) {
                    min_z = (f32)(query->position.z - 0.02f - query->collision_radius);
                    max_z = (f32)(query->movement.z + query->position.z + 0.02f + query->collision_radius);
                } else {
                    min_z = (f32)(query->movement.z + query->position.z - 0.02f - query->collision_radius);
                    max_z = (f32)(query->position.z + 0.02f + query->collision_radius);
                }
                extent_sq = (min_x - max_x) * (min_x - max_x) + (min_y - max_y) * (min_y - max_y) +
                            (min_z - max_z) * (min_z - max_z);
                scan_radius = NuFsqrt((f32)(extent_sq + (f64)1.02f));
                break;
            }
        }
        shape_count = 0;
        while (handle[0] > 0) {
            const i32 group_index = handle[1];
            TERRAIN_SCAN_WORD_PC *shapes = reinterpret_cast<TERRAIN_SCAN_WORD_PC *>(handle) + 1;
            TERRAIN_GROUP_PC *group = &TERRAIN_STH_SET->groups[group_index];
            f32 local_min_x, local_max_x, local_min_z, local_max_z, local_min_y, local_max_y;
            TERRAIN_PLATFORM_PC *platform;
            if (group->chunk_type == 1 && (platform = &TERRAIN_STH_PLATFORM(group))->scene_object != NULL) {
                if (TERRAIN_STH_MATRIX(group)->m30 > TERRAIN_STH_PLATFORM(group).previous_matrix.m30) {
                    local_min_x = min_x - group->origin.x;
                    local_max_x =
                        (f32)((TERRAIN_STH_MATRIX(group)->m30 - TERRAIN_STH_PLATFORM(group).previous_matrix.m30) * 1.5 +
                              max_x - group->origin.x);
                } else {
                    local_max_x = max_x - group->origin.x;
                    local_min_x =
                        (f32)((TERRAIN_STH_MATRIX(group)->m30 - TERRAIN_STH_PLATFORM(group).previous_matrix.m30) * 1.5 +
                              min_x - group->origin.x);
                }
                if (TERRAIN_STH_MATRIX(group)->m31 > TERRAIN_STH_PLATFORM(group).previous_matrix.m31) {
                    local_min_y = min_y - group->origin.y;
                    local_max_y =
                        (f32)((TERRAIN_STH_MATRIX(group)->m31 - TERRAIN_STH_PLATFORM(group).previous_matrix.m31) * 1.5 +
                              max_y - group->origin.y);
                } else {
                    local_max_y = max_y - group->origin.y;
                    local_min_y =
                        (f32)((TERRAIN_STH_MATRIX(group)->m31 - TERRAIN_STH_PLATFORM(group).previous_matrix.m31) * 1.5 +
                              min_y - group->origin.y);
                }
                if (TERRAIN_STH_MATRIX(group)->m32 > TERRAIN_STH_PLATFORM(group).previous_matrix.m32) {
                    local_min_z = min_z - group->origin.z;
                    local_max_z =
                        (f32)((TERRAIN_STH_MATRIX(group)->m32 - TERRAIN_STH_PLATFORM(group).previous_matrix.m32) * 1.5 +
                              max_z - group->origin.z);
                } else {
                    local_max_z = max_z - group->origin.z;
                    local_min_z =
                        (f32)((TERRAIN_STH_MATRIX(group)->m32 - TERRAIN_STH_PLATFORM(group).previous_matrix.m32) * 1.5 +
                              min_z - group->origin.z);
                }
                if (group->scene_index == TerrPlatDis) {
                    shapes += handle[0];
                } else if ((platform->flags & TERRAIN_PLATFORM_FLAG_ROTATING) == 0) {
                    if (TERRAIN_STH_GROUP_OVERLAP(local_min_x, local_min_y, local_min_z, local_max_x, local_max_y,
                                                  local_max_z, group)) {
                        TERRAIN_STH_FILTER_SHAPES();
                        TERRAIN_STH_FINISH_GROUP(group_index);
                    } else {
                        shapes += handle[0];
                    }
                } else {
                    const f64 dy =
                        ((min_y + max_y) * NuQFntHalfScale - group->origin.y) * TERRAIN_STH_QUERY->inverse_object_scale;
                    const f64 dx = (min_x + max_x) * NuQFntHalfScale - group->origin.x;
                    const f64 dz = (min_z + max_z) * NuQFntHalfScale - group->origin.z;
                    const f32 distance_sq = dx * dx + dy * dy + dz * dz;
                    if (distance_sq < (group->radius + scan_radius) * (group->radius + scan_radius)) {
                        for (i32 remaining = handle[0]; remaining > 0; --remaining, ++shapes) {
                            TERRAIN_SHAPE *shape = shapes->shape;
                            if (cursor < limit && TERRAIN_STH_MATERIAL_OK(shape) &&
                                TERRAIN_STH_VERTICES_OVERLAP(shape)) {
                                (cursor++)->shape = shape;
                                ++shape_count;
                            }
                        }
                        TERRAIN_STH_FINISH_GROUP(group_index);
                    } else {
                        shapes += handle[0];
                    }
                }
            } else if (TERRAIN_STH_GROUP_OVERLAP(min_x, min_y, min_z, max_x, max_y, max_z, group)) {
                local_max_x = max_x - group->origin.x;
                local_min_x = min_x - group->origin.x;
                local_max_y = max_y - group->origin.y;
                local_min_y = min_y - group->origin.y;
                local_max_z = max_z - group->origin.z;
                local_min_z = min_z - group->origin.z;
                TERRAIN_STH_FILTER_SHAPES();
                TERRAIN_STH_FINISH_GROUP(group_index);
            } else {
                shapes += handle[0];
            }
            handle = reinterpret_cast<i16 *>(shapes);
        }
        group_header->header.shape_count = 0;
        group_header->header.terrain_group = 0;
        WallSplCount = 0;
        wall_count = 0;
        walls = reinterpret_cast<TERRAIN_WALL_POINT **>(handle) + 1;
        min_x = (f32)(min_x - 0.02f);
        min_z = (f32)(min_z - 0.02f);
        max_x = (f32)(max_x + 0.02f);
        max_z = (f32)(max_z + 0.02f);
        wall_list = static_cast<TERRAIN_WALL_POINT *>(WallSplList);
        while (*walls != NULL) {
            if ((((*walls)[0].position.x >= min_x && (*walls)[1].position.x <= max_x) ||
                 ((*walls)[1].position.x >= min_x && (*walls)[0].position.x <= max_x)) &&
                (((*walls)[0].position.z >= min_z && (*walls)[1].position.z <= max_z) ||
                 ((*walls)[1].position.z >= min_z && (*walls)[0].position.z <= max_z)) &&
                wall_count < 64) {
                wall_list[wall_count++] = (*walls)[0];
                wall_list[wall_count++] = (*walls)[1];
            }
            ++walls;
        }
        WallSplCount = wall_count;
        TerrPlatDis = -1;
    }
#define TERRAIN_STP_CROSS(result_arg, a_arg, b_arg)                                                                    \
    do {                                                                                                               \
        NUVEC *cross_a = (a_arg);                                                                                      \
        NUVEC *cross_b = (b_arg);                                                                                      \
        NUVEC cross;                                                                                                   \
        cross.x = cross_b->z * cross_a->y - cross_b->y * cross_a->z;                                                   \
        cross.y = cross_a->z * cross_b->x - cross_b->z * cross_a->x;                                                   \
        cross.z = cross_b->y * cross_a->x - cross_b->x * cross_a->y;                                                   \
        (result_arg) = cross;                                                                                          \
    } while (0)
    void ScanTerrainPlatform(i32 group_index, i32 terrain_mask) {
        TERRAIN_SCAN_WORD_PC *group_header;
        TERRAIN_SCAN_WORD_PC *cursor;
        TERRAIN_SCAN_WORD_PC *limit;
        i32 shape_count;
        i32 scaled_count;
        f32 max_x, min_x, max_z, min_z, max_y, min_y;
        f32 scan_radius_sq;
        f32 scan_radius;
        NUVEC4 *vertices;
        TERRAIN_GROUP_PC *group;
        TERRAIN_PLATFORM_PC *platform;
        f32 local_min_x, local_max_x, local_min_z, local_max_z, local_min_y, local_max_y;

        TerrainQuery_s *query;
        scaled_count = 0;
        vertices = reinterpret_cast<NUVEC4 *>((reinterpret_cast<u32>(NuScratchAlloc32(0xd0)) + 31) & ~u32(15));
        TerrainScaledShapeCursorPc = static_cast<TERRAIN_SHAPE *>(TerrainScaleWorkBasePc);
        query = TERRAIN_STH_QUERY;
        group_header = query->scan_words;
        cursor = reinterpret_cast<TERRAIN_SCAN_WORD_PC *>(query->scan_list_storage) + 1;
        limit = cursor + 508;
        shape_count = 0;
        platinrange = 0;
        query->scan_group_index = -1;
        switch (query->scan_result) {
            default: {
                const f32 movement_sq = query->movement.x * query->movement.x + query->movement.y * query->movement.y +
                                        query->movement.z * query->movement.z;
                const f32 reach = (f32)(0.02f + query->collision_radius_sq + movement_sq);
                max_x = query->position.x + reach;
                max_y = query->position.y + query->object_scale * reach;
                max_z = query->position.z + reach;
                min_x = query->position.x - reach;
                min_y = query->position.y - query->object_scale * reach;
                min_z = query->position.z - reach;
                scan_radius = NuFsqrt((f32)(reach + 1.0));
                break;
            }
            case 1: {
                f32 extent_sq;
                if (query->movement.x > 0.0f) {
                    min_x = (f32)(query->position.x - 0.02f - query->collision_radius);
                    max_x = (f32)(query->movement.x + query->position.x + 0.02f + query->collision_radius);
                } else {
                    min_x = (f32)(query->movement.x + query->position.x - 0.02f - query->collision_radius);
                    max_x = (f32)(query->position.x + 0.02f + query->collision_radius);
                }
                if (query->movement.y > 0.0f) {
                    min_y = (f32)(query->position.y - 0.02f - query->collision_radius);
                    max_y = (f32)(query->movement.y + query->position.y + 0.02f + query->collision_radius);
                } else {
                    min_y = (f32)(query->movement.y + query->position.y - 0.02f - query->collision_radius);
                    max_y = (f32)(query->position.y + 0.02f + query->collision_radius);
                }
                if (query->movement.z > 0.0f) {
                    min_z = (f32)(query->position.z - 0.02f - query->collision_radius);
                    max_z = (f32)(query->movement.z + query->position.z + 0.02f + query->collision_radius);
                } else {
                    min_z = (f32)(query->movement.z + query->position.z - 0.02f - query->collision_radius);
                    max_z = (f32)(query->position.z + 0.02f + query->collision_radius);
                }
                extent_sq = (min_x - max_x) * (min_x - max_x) + (min_y - max_y) * (min_y - max_y) +
                            (min_z - max_z) * (min_z - max_z);
                scan_radius = NuFsqrt((f32)(extent_sq + (query->collision_radius + (f64)1.02f)));
                break;
            }
        }
        query = TERRAIN_STH_QUERY;
        query->scan_list = group_header;
        min_x = (f32)(min_x - 0.05f);
        max_x = (f32)(max_x + 0.05f);
        min_y = (f32)(min_y - 0.05f);
        max_y = (f32)(max_y + 0.05f);
        min_z = (f32)(min_z - 0.05f);
        max_z = (f32)(max_z + 0.05f);
        group = &TERRAIN_STH_SET->groups[group_index];
        platform = &TERRAIN_STH_PLATFORM(group);
        if (platform->scene_object != NULL) {
            if (TERRAIN_STH_MATRIX(group)->m30 > TERRAIN_STH_PLATFORM(group).previous_matrix.m30) {
                local_min_x = min_x - group->origin.x;
                local_max_x =
                    (f32)((TERRAIN_STH_MATRIX(group)->m30 - TERRAIN_STH_PLATFORM(group).previous_matrix.m30) * 1.5 +
                          max_x - group->origin.x);
            } else {
                local_max_x = max_x - group->origin.x;
                local_min_x =
                    (f32)((TERRAIN_STH_MATRIX(group)->m30 - TERRAIN_STH_PLATFORM(group).previous_matrix.m30) * 1.5 +
                          min_x - group->origin.x);
            }
            if (TERRAIN_STH_MATRIX(group)->m31 > TERRAIN_STH_PLATFORM(group).previous_matrix.m31) {
                local_min_y = min_y - group->origin.y;
                local_max_y =
                    (f32)((TERRAIN_STH_MATRIX(group)->m31 - TERRAIN_STH_PLATFORM(group).previous_matrix.m31) * 1.5 +
                          max_y - group->origin.y);
            } else {
                local_max_y = max_y - group->origin.y;
                local_min_y =
                    (f32)((TERRAIN_STH_MATRIX(group)->m31 - TERRAIN_STH_PLATFORM(group).previous_matrix.m31) * 1.5 +
                          min_y - group->origin.y);
            }
            if (TERRAIN_STH_MATRIX(group)->m32 > TERRAIN_STH_PLATFORM(group).previous_matrix.m32) {
                local_min_z = min_z - group->origin.z;
                local_max_z =
                    (f32)((TERRAIN_STH_MATRIX(group)->m32 - TERRAIN_STH_PLATFORM(group).previous_matrix.m32) * 1.5 +
                          max_z - group->origin.z);
            } else {
                local_max_z = max_z - group->origin.z;
                local_min_z =
                    (f32)((TERRAIN_STH_MATRIX(group)->m32 - TERRAIN_STH_PLATFORM(group).previous_matrix.m32) * 1.5 +
                          min_z - group->origin.z);
            }
        } else {
            local_max_x = max_x - group->origin.x;
            local_min_x = min_x - group->origin.x;
            local_max_y = max_y - group->origin.y;
            local_min_y = min_y - group->origin.y;
            local_max_z = max_z - group->origin.z;
            local_min_z = min_z - group->origin.z;
        }
        if ((platform->flags & TERRAIN_PLATFORM_FLAG_ROTATING) != 0) {
            const f64 dy = ((min_y + max_y) * 0.5 - group->origin.y) * query->inverse_object_scale;
            const f64 dx = (min_x + max_x) * 0.5 - group->origin.x;
            const f64 dz = (min_z + max_z) * 0.5 - group->origin.z;
            // Retail reuses the local minimum X slot for the distance, so the vertex test below sees it.
            local_min_x = dx * dx + dy * dy + dz * dz;
            if (group->chunk_type != -1 &&
                local_min_x < (group->radius + scan_radius) * (group->radius + scan_radius)) {
                TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
                while (batch->marker >= 0) {
                    const i32 count = batch->shape_count;
                    TERRAIN_SHAPE *source = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                    for (i32 remaining = count; remaining > 0; --remaining, ++source) {
                        platinrange = 1;
                        if (cursor >= limit || !TERRAIN_STH_MATERIAL_OK(source)) {
                            continue;
                        }
                        vertices[0].x = source->vertices[0].x;
                        vertices[0].y = source->vertices[0].y;
                        vertices[0].z = source->vertices[0].z;
                        vertices[0].w = 0.0f;
                        vertices[1].x = source->vertices[1].x;
                        vertices[1].y = source->vertices[1].y;
                        vertices[1].z = source->vertices[1].z;
                        vertices[1].w = 0.0f;
                        vertices[2].x = source->vertices[2].x;
                        vertices[2].y = source->vertices[2].y;
                        vertices[2].z = source->vertices[2].z;
                        vertices[2].w = 0.0f;
                        NuVec4MtxTransformVU0x3(vertices, vertices, TERRAIN_STH_MATRIX(group));
                        if (source->normals[1].y < 65535.0) {
                            vertices[3].x = source->vertices[3].x;
                            vertices[3].y = source->vertices[3].y;
                            vertices[3].z = source->vertices[3].z;
                            vertices[3].w = 0.0f;
                            NuVec4MtxTransformVU0(&vertices[3], &vertices[3], TERRAIN_STH_MATRIX(group));
                        } else {
                            vertices[3] = vertices[2];
                        }
                        if (!((vertices[0].x > local_min_x || vertices[1].x > local_min_x ||
                               vertices[2].x > local_min_x || vertices[3].x > local_min_x) &&
                              (vertices[0].x < local_max_x || vertices[1].x < local_max_x ||
                               vertices[2].x < local_max_x || vertices[3].x < local_max_x) &&
                              (vertices[0].z > local_min_z || vertices[1].z > local_min_z ||
                               vertices[2].z > local_min_z || vertices[3].z > local_min_z) &&
                              (vertices[0].z < local_max_z || vertices[1].z < local_max_z ||
                               vertices[2].z < local_max_z || vertices[3].z < local_max_z))) {
                            continue;
                        }
                        TERRAIN_SHAPE *surface = &TerrainScaledShapeCursorPc[scaled_count];
                        memcpy(surface->material, source->material, 4);
                        query = TERRAIN_STH_QUERY;
                        surface->vertices[0].x = vertices[0].x;
                        surface->vertices[0].z = vertices[0].z;
                        surface->vertices[0].y =
                            (group->origin.y + vertices[0].y) * query->inverse_object_scale - group->origin.y;
                        surface->vertices[1].x = vertices[1].x;
                        surface->vertices[1].z = vertices[1].z;
                        surface->vertices[1].y =
                            (vertices[1].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                        surface->vertices[2].x = vertices[2].x;
                        surface->vertices[2].z = vertices[2].z;
                        surface->vertices[2].y =
                            (vertices[2].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                        if (source->normals[1].y < 65535.0) {
                            surface->vertices[3].x = vertices[3].x;
                            surface->vertices[3].z = vertices[3].z;
                            surface->vertices[3].y =
                                (vertices[3].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                            vertices[0].x = surface->vertices[1].x - surface->vertices[3].x;
                            vertices[0].y = surface->vertices[1].y - surface->vertices[3].y;
                            vertices[0].z = surface->vertices[1].z - surface->vertices[3].z;
                            vertices[1].x = surface->vertices[2].x - surface->vertices[3].x;
                            vertices[1].y = surface->vertices[2].y - surface->vertices[3].y;
                            vertices[1].z = surface->vertices[2].z - surface->vertices[3].z;
                            TERRAIN_STP_CROSS(surface->normals[1], reinterpret_cast<NUVEC *>(&vertices[0]),
                                              reinterpret_cast<NUVEC *>(&vertices[1]));
                            const f32 length = NuFsqrt(surface->normals[1].x * surface->normals[1].x +
                                                       surface->normals[1].y * surface->normals[1].y +
                                                       surface->normals[1].z * surface->normals[1].z);
                            const f32 inverse = length != 0.0f ? 1.0f / length : 0.0f;
                            surface = &TerrainScaledShapeCursorPc[scaled_count];
                            surface->normals[1].x *= inverse;
                            surface->normals[1].y *= inverse;
                            surface->normals[1].z *= inverse;
                        } else {
                            surface->normals[1].y = 65536.0f;
                        }
                        vertices[0].x = surface->vertices[2].x - surface->vertices[0].x;
                        vertices[0].y = surface->vertices[2].y - surface->vertices[0].y;
                        vertices[0].z = surface->vertices[2].z - surface->vertices[0].z;
                        vertices[1].x = surface->vertices[1].x - surface->vertices[0].x;
                        vertices[1].y = surface->vertices[1].y - surface->vertices[0].y;
                        vertices[1].z = surface->vertices[1].z - surface->vertices[0].z;
                        TERRAIN_STP_CROSS(surface->normals[0], reinterpret_cast<NUVEC *>(&vertices[0]),
                                          reinterpret_cast<NUVEC *>(&vertices[1]));
                        const f32 length = NuFsqrt(surface->normals[0].x * surface->normals[0].x +
                                                   surface->normals[0].y * surface->normals[0].y +
                                                   surface->normals[0].z * surface->normals[0].z);
                        const f32 inverse = length != 0.0f ? 1.0f / length : 0.0f;
                        surface = &TerrainScaledShapeCursorPc[scaled_count];
                        surface->normals[0].x *= inverse;
                        surface->normals[0].y *= inverse;
                        surface->normals[0].z *= inverse;
                        (cursor++)->shape = surface;
                        ++shape_count;
                        ++scaled_count;
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(source);
                }
                if (shape_count != 0) {
                    group_header->header.shape_count = (i16)shape_count;
                    group_header->header.terrain_group = (i16)group_index;
                    group_header = cursor;
                }
            }
        } else {
            if (local_max_x >= group->bounds_min.x && local_max_y >= group->bounds_min.y &&
                local_max_z >= group->bounds_min.z && local_min_x <= group->bounds_max.x &&
                local_min_y < group->bounds_max.y && local_min_z < group->bounds_max.z && group->chunk_type != -1) {
                TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
                while (batch->marker >= 0) {
                    const i32 count = batch->shape_count;
                    TERRAIN_SHAPE *source = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                    if (local_max_x >= batch->min_x && batch->max_x > local_min_x && local_max_z >= batch->min_z &&
                        batch->max_z > local_min_z) {
                        for (i32 remaining = count; remaining > 0; --remaining, ++source) {
                            if (!TERRAIN_STH_SHAPE_OVERLAP(source)) {
                                continue;
                            }
                            platinrange = 1;
                            if (cursor >= limit || !TERRAIN_STH_MATERIAL_OK(source)) {
                                continue;
                            }
                            if (query->object_scale == 1.0f) {
                                (cursor++)->shape = source;
                                ++shape_count;
                            } else {
                                TERRAIN_SHAPE *surface = &TerrainScaledShapeCursorPc[scaled_count];
                                memcpy(surface->material, source->material, 4);
                                surface->vertices[0].x = source->vertices[0].x;
                                surface->vertices[0].z = source->vertices[0].z;
                                surface->vertices[0].y =
                                    (source->vertices[0].y + group->origin.y) * query->inverse_object_scale -
                                    group->origin.y;
                                surface->vertices[1].x = source->vertices[1].x;
                                surface->vertices[1].z = source->vertices[1].z;
                                surface->vertices[1].y =
                                    (source->vertices[1].y + group->origin.y) * query->inverse_object_scale -
                                    group->origin.y;
                                surface->vertices[2].x = source->vertices[2].x;
                                surface->vertices[2].z = source->vertices[2].z;
                                surface->vertices[2].y =
                                    (source->vertices[2].y + group->origin.y) * query->inverse_object_scale -
                                    group->origin.y;
                                if (source->normals[1].y < 65535.0) {
                                    surface->vertices[3].x = source->vertices[3].x;
                                    surface->vertices[3].z = source->vertices[3].z;
                                    surface->vertices[3].y =
                                        (source->vertices[3].y + group->origin.y) * query->inverse_object_scale -
                                        group->origin.y;
                                    const f32 inverse = TerrainDivideNonzeroPC(
                                        1.0f,
                                        NuFsqrt(source->normals[1].x * source->normals[1].x +
                                                source->normals[1].y * source->normals[1].y * query->object_scale_sq +
                                                source->normals[1].z * source->normals[1].z));
                                    query = TERRAIN_STH_QUERY;
                                    surface = &TerrainScaledShapeCursorPc[scaled_count];
                                    surface->normals[1].x = source->normals[1].x * inverse;
                                    surface->normals[1].y = query->object_scale * source->normals[1].y * inverse;
                                    surface->normals[1].z = inverse * source->normals[1].z;
                                } else {
                                    surface->normals[1].y = 65536.0f;
                                }
                                const f32 inverse = TerrainDivideNonzeroPC(
                                    1.0f, NuFsqrt(source->normals[0].x * source->normals[0].x +
                                                  source->normals[0].y * source->normals[0].y * query->object_scale_sq +
                                                  source->normals[0].z * source->normals[0].z));
                                query = TERRAIN_STH_QUERY;
                                surface = &TerrainScaledShapeCursorPc[scaled_count];
                                surface->normals[0].x = source->normals[0].x * inverse;
                                surface->normals[0].y = query->object_scale * source->normals[0].y * inverse;
                                surface->normals[0].z = inverse * source->normals[0].z;
                                (cursor++)->shape = surface;
                                ++shape_count;
                                ++scaled_count;
                            }
                        }
                    } else {
                        source += count;
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(source);
                }
                if (shape_count != 0) {
                    group_header->header.shape_count = (i16)shape_count;
                    group_header->header.terrain_group = (i16)group_index;
                    group_header = cursor;
                }
            }
        }
        NuScratchRelease();
        group_header->header.shape_count = 0;
        group_header->header.terrain_group = 0;
    }
#undef TERRAIN_STH_SET
#undef TERRAIN_STH_QUERY
#undef TERRAIN_STH_PLATFORM
#undef TERRAIN_STH_MATRIX
#undef TERRAIN_STH_GROUP_OVERLAP
#undef TERRAIN_STH_SHAPE_OVERLAP
#undef TERRAIN_STH_MATERIAL_OK
#undef TERRAIN_STH_VERTICES_OVERLAP
#undef TERRAIN_STH_FINISH_GROUP
#undef TERRAIN_STH_FILTER_SHAPES
#undef TERRAIN_STP_CROSS
    i32 HitTerrPoly(TERRAIN_SHAPE *surface, i32 group_index) {
        f32 primary_start = 0.0f;
        f32 secondary_start = 0.0f;
        f32 secondary_end = 0.0f;
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        f32 radius = query->collision_radius;
        i32 result = 0;
        query->hit_type = result;
        query->hit_time = 999.9f;
        const f64 horizontal_length =
            NuFsqrt(query->movement.x * query->movement.x + query->movement.z * query->movement.z);
        query = (TerrainQuery_s *)TerI;
        query->horizontal_movement_length = horizontal_length;
        NUVEC *origin = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[group_index].origin;
        query->local_start.x = query->position.x - origin->x;
        i32 test = 0;
        query->local_start.y = query->position.y - origin->y;
        query->local_start.z = query->position.z - origin->z;
        query->local_end.x = (query->position.x + query->movement.x) - origin->x;
        query->local_end.y = (query->position.y + query->movement.y) - origin->y;
        const f32 end_z = (query->position.z + query->movement.z) - origin->z;
        query->local_end.z = end_z;
        const f32 primary_end = (((query->local_end.x - surface->vertices[0].x) * surface->normals[0].x +
                                  (query->local_end.y - surface->vertices[0].y) * surface->normals[0].y) +
                                 (end_z - surface->vertices[0].z) * surface->normals[0].z) -
                                radius - query->compare_epsilon;
        if (primary_end < 0.0f) {
            primary_start = ((query->local_start.x - surface->vertices[0].x) * surface->normals[0].x +
                             (query->local_start.y - surface->vertices[0].y) * surface->normals[0].y) +
                            (query->local_start.z - surface->vertices[0].z) * surface->normals[0].z - radius;
            if (primary_start > -radius)
                test = 1;
        }
        if (surface->normals[1].y < 65536.0f) {
            secondary_end = (((query->local_end.x - surface->vertices[3].x) * surface->normals[1].x +
                              (query->local_end.y - surface->vertices[3].y) * surface->normals[1].y) +
                             (end_z - surface->vertices[3].z) * surface->normals[1].z) -
                            radius - query->compare_epsilon;
            if (secondary_end < 0.0f) {
                secondary_start = ((query->local_start.x - surface->vertices[3].x) * surface->normals[1].x +
                                   (query->local_start.y - surface->vertices[3].y) * surface->normals[1].y) +
                                  (query->local_start.z - surface->vertices[3].z) * surface->normals[1].z - radius;
                if (secondary_start > -radius)
                    test = 1;
            }
        }
        if (test != 0 && HitPoly(primary_start, primary_end, secondary_start, secondary_end, surface)) {
            ((TerrainQuery_s *)TerI)->terrain_group_index = group_index;
            result = 1;
        }
        return result;
    }
}

extern "C" void RayImpact(NUVEC *movement) {
    TerrainMoveImpactData();
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(TerI);
    switch (query->hit_type) {
        case 1:
        case 2:
        case 3:
        case 4:
            query->hit_time -= query->separation_epsilon;
            if (query->hit_time < 0.0f)
                query->hit_time = 0.0f;
            movement->x = query->movement.x * query->hit_time;
            movement->y = query->movement.y * query->hit_time;
            movement->z = query->movement.z * query->hit_time;
            break;
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
            movement->x = 0.0f;
            movement->y = 0.0f;
            movement->z = 0.0f;
            break;
    }
}
extern "C" i32 NewRayCastScaleYMask(NUVEC *position, NUVEC *movement, f32 radius, f32 scale_y, i32 scan_flags,
                                    u32 terrain_mask) {
    plathitid = -1;
    TerrainShadowStatusPc = -1;
    TerrPoly = NULL;
    TerrWallInfo = 0;
    TerrainShadowGroupIndexPc = -1;
    if (CurrentTerrain == NULL)
        return 0;
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    query->object_scale = scale_y;
    TerI = query;
    query->object_scale_sq = scale_y * scale_y;
    TerrOverRideScan = NULL;
    f32 inverse_scale;
    if (scale_y != 0.0f)
        inverse_scale = 1.0f / scale_y;
    else
        inverse_scale = 0.0f;
    query->inverse_object_scale = inverse_scale;
    query->inverse_object_scale_sq = inverse_scale * inverse_scale;
    query->collision_radius = radius;
    if (radius == 0.0f)
        query->inverse_collision_radius = 0.0f;
    else
        query->inverse_collision_radius = 1.0f / radius;
    query->collision_radius_sq = radius * radius;
    query->start_position.x = query->position.x = position->x;
    query->start_position.y = query->position.y = position->y;
    query->start_position.z = query->position.z = position->z;
    query->movement = *movement;
    query->start_movement = *movement;
    query->object_index = -1;
    query->hit_flags = NULL;
    query->scan_result = 1;
    query->separation_epsilon = 0.01f;
    query->compare_epsilon = 0.00001f;
    ScanTerrain(1, terrain_mask, scan_flags != 0 ? 0x40 : 0);
    query = static_cast<TerrainQuery_s *>(TerI);
    query->position.y *= query->inverse_object_scale;
    query->movement.y = query->movement.y * query->inverse_object_scale;
    DerotateMovementVector();
    HitTerrain();
    query = static_cast<TerrainQuery_s *>(TerI);
    if (query->hit_type != 0) {
        RayImpact(movement);
        TerrainImpactNorm();
        GetSurfaceInfoSurfaceNormalPc = static_cast<TerrainQuery_s *>(TerI)->movement_normal;
        movement->y *= query->object_scale;
    }
    NuScratchRelease();
    return static_cast<TerrainQuery_s *>(TerI)->hit_type;
}

extern "C" i32 __cdecl TerrainPlatId(void) {
    return plathitid;
}

STATIC_ASSERT(offsetof(TERRAIN_SHAPE, material) == 0x60);
extern "C" f32 __cdecl NewRayCastGetTOFI(void) {
    return static_cast<TerrainQuery_s *>(TerI)->hit_time;
}
extern "C" i32 __cdecl NewRayCastGetImpactTerrainType(void) {
    TERRAIN_SHAPE *surface = static_cast<TerrainQuery_s *>(TerI)->surface;
    return surface ? surface->material[0] : 0;
}

STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, material) == 0x60);
STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, flags) == 0x62);
STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, normal_flags) == 0x63);
extern "C" void __cdecl NewTerrHitInfo(u8 *info) {
    info[0] = TerrPolyInfo.material[0];
    info[1] = TerrPolyInfo.material[1];
    info[2] = TerrPolyInfo.flags;
    info[3] = TerrPolyInfo.normal_flags;
}
extern "C" void __cdecl NewRayCastGetImpactNormal(NUVEC *normal) {
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(TerI);
    if (query != NULL)
        *normal = query->movement_normal;
}

extern "C" f32 __cdecl NewRayCastGetEmbedDist(void) {
    return static_cast<TerrainQuery_s *>(TerI)->unclamped_hit_time;
}
extern "C" i32 __cdecl NewRayCastHitWallSpline(void) {
    return static_cast<TerrainQuery_s *>(TerI)->shape_adjusted;
}
extern "C" i32 NewRayCast(NUVEC *position, NUVEC *movement, f32 radius, i32 scan_flags) {
    plathitid = -1;
    TerrainShadowStatusPc = -1;
    TerrPoly = NULL;
    TerrWallInfo = 0;
    TerrainShadowGroupIndexPc = -1;
    if (CurrentTerrain == NULL)
        return 0;
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    TerrOverRideScan = NULL;
    query->inverse_object_scale = 1.0f;
    TerI = query;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    if (radius == 0.0f)
        query->inverse_collision_radius = 0.0f;
    else
        query->inverse_collision_radius = 1.0f / radius;
    query->collision_radius_sq = radius * radius;
    query->start_position.x = query->position.x = position->x;
    query->start_position.y = query->position.y = position->y;
    query->start_position.z = query->position.z = position->z;
    query->movement = *movement;
    query->start_movement = *movement;
    query->object_index = -1;
    query->hit_flags = NULL;
    query->scan_result = 1;
    query->separation_epsilon = 0.01f;
    query->compare_epsilon = 0.00001f;
    ScanTerrain(1, 0, scan_flags != 0 ? 0x40 : 0);
    query = static_cast<TerrainQuery_s *>(TerI);
    DerotateMovementVector();
    HitTerrain();
    query = static_cast<TerrainQuery_s *>(TerI);
    if (query->hit_type != 0) {
        RayImpact(movement);
        TerrainImpactNorm();
        query = static_cast<TerrainQuery_s *>(TerI);
        GetSurfaceInfoSurfaceNormalPc = query->movement_normal;
    }
    NuScratchRelease();
    return static_cast<TerrainQuery_s *>(TerI)->hit_type;
}

extern "C" i32 NewRayCastMask(NUVEC *position, NUVEC *movement, f32 radius, i32 terrain_mask, i32 scan_flags) {
    plathitid = -1;
    TerrainShadowStatusPc = -1;
    TerrPoly = NULL;
    TerrWallInfo = 0;
    TerrainShadowGroupIndexPc = -1;
    if (CurrentTerrain == NULL)
        return 0;
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    TerrOverRideScan = NULL;
    query->inverse_object_scale = 1.0f;
    TerI = query;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    if (radius == 0.0f)
        query->inverse_collision_radius = 0.0f;
    else
        query->inverse_collision_radius = 1.0f / radius;
    query->collision_radius_sq = radius * radius;
    query->start_position.x = query->position.x = position->x;
    query->start_position.y = query->position.y = position->y;
    query->start_position.z = query->position.z = position->z;
    query->movement = *movement;
    query->start_movement = *movement;
    query->object_index = -1;
    query->hit_flags = NULL;
    query->scan_result = 1;
    query->separation_epsilon = 0.01f;
    query->compare_epsilon = 0.00001f;
    ScanTerrain(1, terrain_mask, scan_flags != 0 ? 0x40 : 0);
    query = static_cast<TerrainQuery_s *>(TerI);
    DerotateMovementVector();
    HitTerrain();
    query = static_cast<TerrainQuery_s *>(TerI);
    if (query->hit_type != 0) {
        RayImpact(movement);
        TerrainImpactNorm();
        query = static_cast<TerrainQuery_s *>(TerI);
        GetSurfaceInfoSurfaceNormalPc = query->movement_normal;
    }
    NuScratchRelease();
    return static_cast<TerrainQuery_s *>(TerI)->hit_type;
}

extern "C" i32 NewRayCastSet(NUVEC *position, NUVEC *movement, f32 radius, f32 separation_epsilon, f32 compare_epsilon,
                             i32 scan_type, i32 scan_flags) {
    plathitid = -1;
    TerrainShadowStatusPc = -1;
    TerrPoly = NULL;
    TerrWallInfo = 0;
    TerrainShadowGroupIndexPc = -1;
    if (CurrentTerrain == NULL)
        return 0;
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    TerrOverRideScan = NULL;
    query->inverse_object_scale = 1.0f;
    TerI = query;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    if (radius == 0.0f)
        query->inverse_collision_radius = 0.0f;
    else
        query->inverse_collision_radius = 1.0f / radius;
    query->collision_radius_sq = radius * radius;
    query->start_position.x = query->position.x = position->x;
    query->start_position.y = query->position.y = position->y;
    query->start_position.z = query->position.z = position->z;
    query->movement = *movement;
    query->start_movement = *movement;
    query->object_index = -1;
    query->hit_flags = NULL;
    query->scan_result = 1;
    query->separation_epsilon = separation_epsilon;
    query->compare_epsilon = compare_epsilon;
    ScanTerrain(scan_type, 0, scan_flags != 0 ? 0x40 : 0);
    query = static_cast<TerrainQuery_s *>(TerI);
    DerotateMovementVector();
    HitTerrain();
    query = static_cast<TerrainQuery_s *>(TerI);
    if (query->hit_type != 0) {
        RayImpact(movement);
        TerrainImpactNorm();
        query = static_cast<TerrainQuery_s *>(TerI);
        GetSurfaceInfoSurfaceNormalPc = query->movement_normal;
    }
    NuScratchRelease();
    return static_cast<TerrainQuery_s *>(TerI)->hit_type;
}

extern "C" i32 NewRayCastSetMask(NUVEC *position, NUVEC *movement, f32 radius, f32 separation_epsilon,
                                 f32 compare_epsilon, i32 scan_type, i32 terrain_mask, i32 scan_flags) {
    plathitid = -1;
    TerrainShadowStatusPc = -1;
    TerrPoly = NULL;
    TerrWallInfo = 0;
    TerrainShadowGroupIndexPc = -1;
    if (CurrentTerrain == NULL)
        return 0;
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    query->inverse_object_scale = 1.0f;
    TerI = query;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    if (radius == 0.0f)
        query->inverse_collision_radius = 0.0f;
    else
        query->inverse_collision_radius = 1.0f / radius;
    query->collision_radius_sq = radius * radius;
    query->start_position.x = query->position.x = position->x;
    query->start_position.y = query->position.y = position->y;
    query->start_position.z = query->position.z = position->z;
    query->movement = *movement;
    query->start_movement = *movement;
    query->object_index = -1;
    query->hit_flags = NULL;
    query->scan_result = 1;
    query->separation_epsilon = separation_epsilon;
    query->compare_epsilon = compare_epsilon;
    ScanTerrain(scan_type, terrain_mask, scan_flags != 0 ? 0x40 : 0);
    query = static_cast<TerrainQuery_s *>(TerI);
    DerotateMovementVector();
    HitTerrain();
    query = static_cast<TerrainQuery_s *>(TerI);
    if (query->hit_type != 0) {
        RayImpact(movement);
        TerrainImpactNorm();
        query = static_cast<TerrainQuery_s *>(TerI);
        GetSurfaceInfoSurfaceNormalPc = query->movement_normal;
    }
    NuScratchRelease();
    return static_cast<TerrainQuery_s *>(TerI)->hit_type;
}

extern "C" {
    extern void *TempScanStack;
}
static u8 *TempStackPtrPc = 0;
struct TERRAIN_HANDLE_WALL_PAIR_PC {
    TERRAIN_WALL_POINT *first;
    TERRAIN_WALL_POINT *second;
};
STATIC_ASSERT(sizeof(TERRAIN_HANDLE_WALL_PAIR_PC) == 8);
STATIC_ASSERT(offsetof(TERRAIN_HANDLE_WALL_PAIR_PC, first) == 0);
STATIC_ASSERT(offsetof(TERRAIN_HANDLE_WALL_PAIR_PC, second) == 4);

extern "C" void NewScanInit(void) {
    TempStackPtrPc = static_cast<u8 *>(TempScanStack);
    TerrPlatDis = -1;
}

#define TERRAIN_NSS_PLATFORM(group_ptr)                                                                                \
    (static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->platforms[(group_ptr)->scene_index])
#define TERRAIN_NSS_MATRIX(group_ptr) (static_cast<NUMTX *>(TERRAIN_NSS_PLATFORM(group_ptr).scene_object))
#define TERRAIN_NSS_GROUP_OVERLAP(lo_x, lo_y, lo_z, hi_x, hi_y, hi_z, group_ptr)                                       \
    ((hi_x) >= (group_ptr)->bounds_min.x && (hi_y) >= (group_ptr)->bounds_min.y &&                                     \
     (hi_z) >= (group_ptr)->bounds_min.z && (group_ptr)->bounds_max.x >= (lo_x) &&                                     \
     (group_ptr)->bounds_max.y > (lo_y) && (group_ptr)->bounds_max.z > (lo_z))
#define TERRAIN_NSS_SHAPE_OVERLAP(shape_ptr)                                                                           \
    (local_max_x >= (shape_ptr)->min_x && (shape_ptr)->max_x > local_min_x && local_max_z >= (shape_ptr)->min_z &&     \
     (shape_ptr)->max_z > local_min_z && local_max_y >= (shape_ptr)->min_y && (shape_ptr)->max_y > local_min_y)
#define TERRAIN_NSS_MATERIAL_OK(shape_ptr)                                                                             \
    ((shape_ptr)->material[1] == 0 || ((shape_ptr)->material[1] & terrain_mask) != 0)
#define TERRAIN_NSS_VERTICES_OVERLAP(shape_ptr)                                                                        \
    (((shape_ptr)->vertices[0].x > local_min_x || (shape_ptr)->vertices[1].x > local_min_x ||                          \
      (shape_ptr)->vertices[2].x > local_min_x || (shape_ptr)->vertices[3].x > local_min_x) &&                         \
     ((shape_ptr)->vertices[0].x < local_max_x || (shape_ptr)->vertices[1].x < local_max_x ||                          \
      (shape_ptr)->vertices[2].x < local_max_x || (shape_ptr)->vertices[3].x < local_max_x) &&                         \
     ((shape_ptr)->vertices[0].z > local_min_z || (shape_ptr)->vertices[1].z > local_min_z ||                          \
      (shape_ptr)->vertices[2].z > local_min_z || (shape_ptr)->vertices[3].z > local_min_z) &&                         \
     ((shape_ptr)->vertices[0].z < local_max_z || (shape_ptr)->vertices[1].z < local_max_z ||                          \
      (shape_ptr)->vertices[2].z < local_max_z || (shape_ptr)->vertices[3].z < local_max_z))
#define TERRAIN_NSS_FINISH_GROUP(group_index_arg)                                                                      \
    do {                                                                                                               \
        if (shape_count != 0) {                                                                                        \
            group_header->header.shape_count = (i16)shape_count;                                                       \
            group_header->header.terrain_group = (i16)(group_index_arg);                                               \
            group_header = cursor;                                                                                     \
            cursor++;                                                                                                  \
            shape_count = 0;                                                                                           \
        }                                                                                                              \
    } while (0)
#define TERRAIN_NSS_FILTER_SHAPES()                                                                                    \
    do {                                                                                                               \
        for (i32 remaining = count; remaining > 0; --remaining, ++shapes) {                                            \
            TERRAIN_SHAPE *shape = shapes->shape;                                                                      \
            if (TERRAIN_NSS_SHAPE_OVERLAP(shape) && cursor < limit && TERRAIN_NSS_MATERIAL_OK(shape)) {                \
                (cursor++)->shape = shape;                                                                             \
                ++shape_count;                                                                                         \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)
i16 *NewScanHandelSubsetPc(i16 *subset, NUVEC *position, NUVEC *movement, f32 radius, i32 terrain_mask) {
    TERRAIN_SCAN_WORD_PC *group_header;
    TERRAIN_SCAN_WORD_PC *cursor;
    TERRAIN_SCAN_WORD_PC *limit;
    TERRAIN_SCAN_WORD_PC *result;
    i32 shape_count;
    f32 max_x, min_x, max_z, min_z, max_y, min_y;
    f32 scan_radius;
    TERRAIN_HANDLE_WALL_PAIR_PC *walls;
    TERRAIN_HANDLE_WALL_PAIR_PC *source;

    if (subset == NULL) {
        return NULL;
    }
    group_header = reinterpret_cast<TERRAIN_SCAN_WORD_PC *>(TempStackPtrPc);
    result = group_header;
    cursor = group_header + 1;
    limit = cursor + 508;
    if (reinterpret_cast<u8 *>(cursor) > reinterpret_cast<u8 *>(TerrainScaleWorkBasePc) + 0x63c18) {
        return NULL;
    }
    if (reinterpret_cast<u8 *>(limit) > reinterpret_cast<u8 *>(TerrainScaleWorkBasePc) + 0x63da8) {
        limit = reinterpret_cast<TERRAIN_SCAN_WORD_PC *>(reinterpret_cast<u8 *>(TerrainScaleWorkBasePc) + 0x63da8);
    }
    if (movement->x > 0.0f) {
        min_x = (f32)(position->x - 0.02f - radius);
        max_x = (f32)(position->x + movement->x + 0.02f + radius);
    } else {
        min_x = (f32)(position->x + movement->x - 0.02f - radius);
        max_x = (f32)(position->x + 0.02f + radius);
    }
    if (movement->y > 0.0f) {
        min_y = (f32)(position->y - 0.02f - radius);
        max_y = (f32)(position->y + movement->y + 0.02f + radius);
    } else {
        min_y = (f32)(position->y + movement->y - 0.02f - radius);
        max_y = (f32)(position->y + 0.02f + radius);
    }
    if (movement->z > 0.0f) {
        min_z = (f32)(position->z - 0.02f - radius);
        max_z = (f32)(position->z + movement->z + 0.02f + radius);
    } else {
        min_z = (f32)(position->z + movement->z - 0.02f - radius);
        max_z = (f32)(position->z + 0.02f + radius);
    }
    shape_count = 0;
    {
        const f32 extent_sq =
            (min_x - max_x) * (min_x - max_x) + (min_y - max_y) * (min_y - max_y) + (min_z - max_z) * (min_z - max_z);
        // Retail uses the squared extent plus the margin directly as the sphere radius.
        scan_radius = (f32)(extent_sq + 0.05f);
    }
    while (subset[0] > 0) {
        const i32 count = subset[0];
        const i32 group_index = subset[1];
        TERRAIN_SCAN_WORD_PC *shapes = reinterpret_cast<TERRAIN_SCAN_WORD_PC *>(subset) + 1;
        TERRAIN_GROUP_PC *group = &static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->groups[group_index];
        f32 local_min_x, local_max_x, local_min_z, local_max_z, local_min_y, local_max_y;
        if (group->chunk_type == 1 && TERRAIN_NSS_PLATFORM(group).scene_object != NULL) {
            TERRAIN_PLATFORM_PC *platform = &TERRAIN_NSS_PLATFORM(group);
            if (TERRAIN_NSS_MATRIX(group)->m30 > TERRAIN_NSS_PLATFORM(group).previous_matrix.m30) {
                local_min_x = min_x - group->origin.x;
                local_max_x =
                    (f32)((TERRAIN_NSS_MATRIX(group)->m30 - TERRAIN_NSS_PLATFORM(group).previous_matrix.m30) * 1.5 +
                          max_x - group->origin.x);
            } else {
                local_max_x = max_x - group->origin.x;
                local_min_x =
                    (f32)((TERRAIN_NSS_MATRIX(group)->m30 - TERRAIN_NSS_PLATFORM(group).previous_matrix.m30) * 1.5 +
                          min_x - group->origin.x);
            }
            if (TERRAIN_NSS_MATRIX(group)->m31 > TERRAIN_NSS_PLATFORM(group).previous_matrix.m31) {
                local_min_y = min_y - group->origin.y;
                local_max_y =
                    (f32)((TERRAIN_NSS_MATRIX(group)->m31 - TERRAIN_NSS_PLATFORM(group).previous_matrix.m31) * 1.5 +
                          max_y - group->origin.y);
            } else {
                local_max_y = max_y - group->origin.y;
                local_min_y =
                    (f32)((TERRAIN_NSS_MATRIX(group)->m31 - TERRAIN_NSS_PLATFORM(group).previous_matrix.m31) * 1.5 +
                          min_y - group->origin.y);
            }
            if (TERRAIN_NSS_MATRIX(group)->m32 > TERRAIN_NSS_PLATFORM(group).previous_matrix.m32) {
                local_min_z = min_z - group->origin.z;
                local_max_z =
                    (f32)((TERRAIN_NSS_MATRIX(group)->m32 - TERRAIN_NSS_PLATFORM(group).previous_matrix.m32) * 1.5 +
                          max_z - group->origin.z);
            } else {
                local_max_z = max_z - group->origin.z;
                local_min_z =
                    (f32)((TERRAIN_NSS_MATRIX(group)->m32 - TERRAIN_NSS_PLATFORM(group).previous_matrix.m32) * 1.5 +
                          min_z - group->origin.z);
            }
            if ((platform->flags & TERRAIN_PLATFORM_FLAG_ROTATING) == 0) {
                if (TERRAIN_NSS_GROUP_OVERLAP(local_min_x, local_min_y, local_min_z, local_max_x, local_max_y,
                                              local_max_z, group)) {
                    TERRAIN_NSS_FILTER_SHAPES();
                    TERRAIN_NSS_FINISH_GROUP(group_index);
                } else {
                    shapes += count;
                }
            } else {
                const f64 dy = ((min_y + max_y) * 0.5 - group->origin.y) *
                               static_cast<TerrainQuery_s *>(TerI)->inverse_object_scale;
                const f64 dx = (min_x + max_x) * 0.5 - group->origin.x;
                const f64 dz = (min_z + max_z) * 0.5 - group->origin.z;
                const f32 distance_sq = dx * dx + dy * dy + dz * dz;
                if (distance_sq < (group->radius + scan_radius) * (group->radius + scan_radius)) {
                    for (i32 remaining = count; remaining > 0; --remaining, ++shapes) {
                        TERRAIN_SHAPE *shape = shapes->shape;
                        if (cursor < limit && TERRAIN_NSS_MATERIAL_OK(shape) && TERRAIN_NSS_VERTICES_OVERLAP(shape)) {
                            (cursor++)->shape = shape;
                            ++shape_count;
                        }
                    }
                    TERRAIN_NSS_FINISH_GROUP(group_index);
                } else {
                    shapes += count;
                }
            }
        } else if (TERRAIN_NSS_GROUP_OVERLAP(min_x, min_y, min_z, max_x, max_y, max_z, group)) {
            local_max_x = max_x - group->origin.x;
            local_min_x = min_x - group->origin.x;
            local_max_y = max_y - group->origin.y;
            local_min_y = min_y - group->origin.y;
            local_max_z = max_z - group->origin.z;
            local_min_z = min_z - group->origin.z;
            TERRAIN_NSS_FILTER_SHAPES();
            TERRAIN_NSS_FINISH_GROUP(group_index);
        } else {
            // Retail skips one extra word after a rejected static group.
            shapes += count + 1;
        }
        subset = reinterpret_cast<i16 *>(shapes);
    }
    group_header->header.shape_count = 0;
    group_header->header.terrain_group = 0;
    min_x = (f32)(min_x - 0.02f);
    min_z = (f32)(min_z - 0.02f);
    max_x = (f32)(max_x + 0.02f);
    max_z = (f32)(max_z + 0.02f);
    walls = reinterpret_cast<TERRAIN_HANDLE_WALL_PAIR_PC *>(group_header + 1);
    source = reinterpret_cast<TERRAIN_HANDLE_WALL_PAIR_PC *>(subset + 2);
    while (static_cast<i32>(reinterpret_cast<usize>(source->first)) > 0) {
        TERRAIN_WALL_POINT *wall = source->first;
        if (((wall[0].position.x >= min_x && wall[1].position.x <= max_x) ||
             (wall[1].position.x >= min_x && wall[0].position.x <= max_x)) &&
            ((wall[0].position.z >= min_z && wall[1].position.z <= max_z) ||
             (wall[1].position.z >= min_z && wall[0].position.z <= max_z)) &&
            (wall->material[1] == 0 || (wall->material[1] & terrain_mask) != 0)) {
            walls->first = wall;
            walls->second = source->first;
            ++walls;
        }
        ++source;
    }
    group_header = reinterpret_cast<TERRAIN_SCAN_WORD_PC *>(walls);
    group_header->header.shape_count = 0;
    group_header->header.terrain_group = 0;
    TempStackPtrPc = reinterpret_cast<u8 *>(group_header + 1);
    return reinterpret_cast<i16 *>(result);
}
#undef TERRAIN_NSS_PLATFORM
#undef TERRAIN_NSS_MATRIX
#undef TERRAIN_NSS_GROUP_OVERLAP
#undef TERRAIN_NSS_SHAPE_OVERLAP
#undef TERRAIN_NSS_MATERIAL_OK
#undef TERRAIN_NSS_VERTICES_OVERLAP
#undef TERRAIN_NSS_FINISH_GROUP
#undef TERRAIN_NSS_FILTER_SHAPES

STATIC_ASSERT(offsetof(TERRAIN_SPATIAL_NODE, point_count) == 0);
STATIC_ASSERT(offsetof(TERRAIN_SPATIAL_CHUNK, previous) == 0);
struct TERRAIN_HANDLE_END_PC {
    TERRAIN_SCAN_WORD_PC terminator;
    union {
        TERRAIN_WALL_POINT *wall_cursor;
        struct {
            i16 shape_count;
            i16 terrain_group;
        } wall_end;
    };
};
STATIC_ASSERT(sizeof(TERRAIN_HANDLE_END_PC) == 8);
STATIC_ASSERT(offsetof(TERRAIN_HANDLE_END_PC, terminator) == 0);
STATIC_ASSERT(offsetof(TERRAIN_HANDLE_END_PC, wall_cursor) == 4);
STATIC_ASSERT(offsetof(TERRAIN_SPATIAL_CHUNK, node) == 4);
STATIC_ASSERT(offsetof(TERRAIN_SPATIAL_NODE, field_02) == 2);
STATIC_ASSERT(offsetof(TERRAIN_SPATIAL_NODE, points) == 4);
STATIC_ASSERT(sizeof(TERRAIN_WALL_POINT) == 16);
STATIC_ASSERT(offsetof(TERRAIN_WALL_POINT, material) == 12);

#define TERRAIN_NSF_SET (static_cast<TERRAIN_SET_PC *>(CurrentTerrain))
#define TERRAIN_NSF_PLATFORM(group_ptr) (TERRAIN_NSF_SET->platforms[(group_ptr)->scene_index])
#define TERRAIN_NSF_MATRIX(group_ptr) (static_cast<NUMTX *>(TERRAIN_NSF_PLATFORM(group_ptr).scene_object))
#define TERRAIN_NSF_SHAPE_OVERLAP(shape_ptr)                                                                           \
    (local_max_x >= (shape_ptr)->min_x && (shape_ptr)->max_x > local_min_x && local_max_z >= (shape_ptr)->min_z &&     \
     (shape_ptr)->max_z > local_min_z && local_max_y >= (shape_ptr)->min_y && (shape_ptr)->max_y > local_min_y)
#define TERRAIN_NSF_MATERIAL_OK(shape_ptr)                                                                             \
    ((shape_ptr)->material[1] == 0 || ((shape_ptr)->material[1] & terrain_mask) != 0)
#define TERRAIN_NSF_FINISH_GROUP(group_index_arg)                                                                      \
    do {                                                                                                               \
        if (shape_count != 0) {                                                                                        \
            group_header->header.shape_count = (i16)shape_count;                                                       \
            group_header->header.terrain_group = (i16)(group_index_arg);                                               \
            group_header = cursor;                                                                                     \
            cursor++;                                                                                                  \
            shape_count = 0;                                                                                           \
        }                                                                                                              \
    } while (0)
#define TERRAIN_NSF_CROSS(result_arg, a_arg, b_arg)                                                                    \
    do {                                                                                                               \
        NUVEC *cross_a = (a_arg);                                                                                      \
        NUVEC *cross_b = (b_arg);                                                                                      \
        NUVEC cross;                                                                                                   \
        cross.x = cross_b->z * cross_a->y - cross_b->y * cross_a->z;                                                   \
        cross.y = cross_a->z * cross_b->x - cross_b->z * cross_a->x;                                                   \
        cross.z = cross_b->y * cross_a->x - cross_b->x * cross_a->y;                                                   \
        (result_arg) = cross;                                                                                          \
    } while (0)
i16 *NewScanHandelFullPc(NUVEC *position, NUVEC *movement, f32 radius, i32 scan_type, i32 terrain_mask) {
    TERRAIN_SCAN_WORD_PC *group_header;
    TERRAIN_SCAN_WORD_PC *cursor;
    TERRAIN_SCAN_WORD_PC *limit;
    TERRAIN_SCAN_WORD_PC *result;
    i32 shape_count;
    i32 scaled_count;
    TerrainScanBounds bounds;
    TerrainScanBounds *const scan = &bounds;
    f32 extent_sq;
    f32 scan_radius;
    NUVEC4 *vertices;
    TERRAIN_HANDLE_END_PC *end_header;

    group_header = reinterpret_cast<TERRAIN_SCAN_WORD_PC *>(TempStackPtrPc);
    cursor = group_header + 1;
    scaled_count = 0;
    TerrainScaledShapeCursorPc = static_cast<TERRAIN_SHAPE *>(TerrainScaleWorkBasePc);
    result = group_header;
    limit = cursor + 508;
    if (reinterpret_cast<u8 *>(cursor) > reinterpret_cast<u8 *>(TerrainScaleWorkBasePc) + 0x63c18) {
        return NULL;
    }
    if (reinterpret_cast<u8 *>(limit) > reinterpret_cast<u8 *>(TerrainScaleWorkBasePc) + 0x63da8) {
        limit = reinterpret_cast<TERRAIN_SCAN_WORD_PC *>(reinterpret_cast<u8 *>(TerrainScaleWorkBasePc) + 0x63da8);
    }
    vertices = reinterpret_cast<NUVEC4 *>((reinterpret_cast<u32>(NuScratchAlloc32(0xd0)) + 31) & ~u32(15));
    shape_count = 0;
    platinrange = 0;
    if (movement->x > 0.0f) {
        scan->min_x = (f32)(position->x - 0.02f - radius);
        scan->max_x = (f32)(position->x + movement->x + 0.02f + radius);
    } else {
        scan->min_x = (f32)(position->x + movement->x - 0.02f - radius);
        scan->max_x = (f32)(position->x + 0.02f + radius);
    }
    if (movement->y > 0.0f) {
        scan->min_y = (f32)(position->y - 0.02f - radius);
        scan->max_y = (f32)(position->y + movement->y + 0.02f + radius);
    } else {
        scan->min_y = (f32)(position->y + movement->y - 0.02f - radius);
        scan->max_y = (f32)(position->y + 0.02f + radius);
    }
    if (movement->z > 0.0f) {
        scan->min_z = (f32)(position->z - 0.02f - radius);
        scan->max_z = (f32)(position->z + movement->z + 0.02f + radius);
    } else {
        scan->min_z = (f32)(position->z + movement->z - 0.02f - radius);
        scan->max_z = (f32)(position->z + 0.02f + radius);
    }
    extent_sq = (scan->min_x - scan->max_x) * (scan->min_x - scan->max_x) +
                (scan->min_y - scan->max_y) * (scan->min_y - scan->max_y) +
                (scan->min_z - scan->max_z) * (scan->min_z - scan->max_z);
    scan_radius = NuFsqrt((f32)(extent_sq + 0.05f));
    for (i32 cell_index = 0; cell_index < TERRAIN_NSF_SET->used_cell_count; ++cell_index) {
        TERRAIN_CELL_PC *cell = &TERRAIN_NSF_SET->cells[cell_index];
        if (!(scan->max_x >= cell->min_x && scan->max_z >= cell->min_z && cell->max_x >= scan->min_x &&
              cell->max_z >= scan->min_z)) {
            continue;
        }
        i16 *indices = TERRAIN_NSF_SET->group_indices + cell->first_group;
        for (i32 i = 0; i < TERRAIN_NSF_SET->cells[cell_index].group_count; ++i, ++indices) {
            TERRAIN_GROUP_PC *group = &TERRAIN_NSF_SET->groups[*indices];
            if (scan->max_x >= group->bounds_min.x && scan->max_y >= group->bounds_min.y &&
                scan->max_z >= group->bounds_min.z && scan->min_x <= group->bounds_max.x &&
                scan->min_y < group->bounds_max.y && scan->min_z < group->bounds_max.z && group->chunk_type != -1) {
                const f32 local_max_x = scan->max_x - group->origin.x;
                const f32 local_max_y = scan->max_y - group->origin.y;
                const f32 local_max_z = scan->max_z - group->origin.z;
                const f32 local_min_x = scan->min_x - group->origin.x;
                const f32 local_min_y = scan->min_y - group->origin.y;
                const f32 local_min_z = scan->min_z - group->origin.z;
                if (group->scene_index < 0) {
                    TerrainSkinAllocate(group);
                }
                TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
                while (batch->marker >= 0) {
                    i32 count = batch->shape_count;
                    TERRAIN_SHAPE *source = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                    if (local_max_x >= batch->min_x && batch->max_x > local_min_x && local_max_z >= batch->min_z &&
                        batch->max_z > local_min_z) {
                        while (count-- > 0) {
                            if (TERRAIN_NSF_SHAPE_OVERLAP(source) && cursor < limit &&
                                TERRAIN_NSF_MATERIAL_OK(source)) {
                                (cursor++)->shape = source;
                                ++shape_count;
                            }
                            ++source;
                        }
                    } else {
                        source += count;
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(source);
                }
                TERRAIN_NSF_FINISH_GROUP(*indices);
            }
        }
    }
    static_cast<TerrainQuery_s *>(TerI)->scan_list = group_header;
    scan->min_x = (f32)(scan->min_x - 0.05f);
    scan->max_x = (f32)(scan->max_x + 0.05f);
    scan->min_y = (f32)(scan->min_y - 0.05f);
    scan->max_y = (f32)(scan->max_y + 0.05f);
    scan->min_z = (f32)(scan->min_z - 0.05f);
    scan->max_z = (f32)(scan->max_z + 0.05f);
    if (scan_type != 0) {
        i16 *indices = TERRAIN_NSF_SET->group_indices + TERRAIN_NSF_SET->platform_cell.first_group;
        for (i32 i = 0; i < TERRAIN_NSF_SET->platform_cell.group_count; ++i, ++indices) {
            TERRAIN_GROUP_PC *group = &TERRAIN_NSF_SET->groups[*indices];
            TERRAIN_PLATFORM_PC *platform = &TERRAIN_NSF_PLATFORM(group);
            u32 *scene_state = platform->instance_flags;
            if (scene_state == NULL || ((platform->flags & 4) != 0 ? (*scene_state & 2) : (*scene_state & 1))) {
                f32 local_min_x, local_max_x, local_min_z, local_max_z, local_min_y, local_max_y;
                if (platform->scene_object != NULL) {
                    if (TERRAIN_NSF_MATRIX(group)->m30 > TERRAIN_NSF_PLATFORM(group).previous_matrix.m30) {
                        local_min_x = scan->min_x - group->origin.x;
                        local_max_x =
                            (f32)((TERRAIN_NSF_MATRIX(group)->m30 - TERRAIN_NSF_PLATFORM(group).previous_matrix.m30) *
                                      1.5 +
                                  scan->max_x - group->origin.x);
                    } else {
                        local_max_x = scan->max_x - group->origin.x;
                        local_min_x =
                            (f32)((TERRAIN_NSF_MATRIX(group)->m30 - TERRAIN_NSF_PLATFORM(group).previous_matrix.m30) *
                                      1.5 +
                                  scan->min_x - group->origin.x);
                    }
                    if (TERRAIN_NSF_MATRIX(group)->m31 > TERRAIN_NSF_PLATFORM(group).previous_matrix.m31) {
                        local_min_y = scan->min_y - group->origin.y;
                        local_max_y =
                            (f32)((TERRAIN_NSF_MATRIX(group)->m31 - TERRAIN_NSF_PLATFORM(group).previous_matrix.m31) *
                                      1.5 +
                                  scan->max_y - group->origin.y);
                    } else {
                        local_max_y = scan->max_y - group->origin.y;
                        local_min_y =
                            (f32)((TERRAIN_NSF_MATRIX(group)->m31 - TERRAIN_NSF_PLATFORM(group).previous_matrix.m31) *
                                      1.5 +
                                  scan->min_y - group->origin.y);
                    }
                    if (TERRAIN_NSF_MATRIX(group)->m32 > TERRAIN_NSF_PLATFORM(group).previous_matrix.m32) {
                        local_min_z = scan->min_z - group->origin.z;
                        local_max_z =
                            (f32)((TERRAIN_NSF_MATRIX(group)->m32 - TERRAIN_NSF_PLATFORM(group).previous_matrix.m32) *
                                      1.5 +
                                  scan->max_z - group->origin.z);
                    } else {
                        local_max_z = scan->max_z - group->origin.z;
                        local_min_z =
                            (f32)((TERRAIN_NSF_MATRIX(group)->m32 - TERRAIN_NSF_PLATFORM(group).previous_matrix.m32) *
                                      1.5 +
                                  scan->min_z - group->origin.z);
                    }
                } else {
                    local_max_x = scan->max_x - group->origin.x;
                    local_min_x = scan->min_x - group->origin.x;
                    local_max_y = scan->max_y - group->origin.y;
                    local_min_y = scan->min_y - group->origin.y;
                    local_max_z = scan->max_z - group->origin.z;
                    local_min_z = scan->min_z - group->origin.z;
                }
                if ((platform->flags & TERRAIN_PLATFORM_FLAG_ROTATING) == 0) {
                    if (local_max_x >= group->bounds_min.x && local_max_y >= group->bounds_min.y &&
                        local_max_z >= group->bounds_min.z && local_min_x <= group->bounds_max.x &&
                        local_min_y < group->bounds_max.y && local_min_z < group->bounds_max.z &&
                        group->chunk_type != -1) {
                        TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
                        while (batch->marker >= 0) {
                            i32 count = batch->shape_count;
                            TERRAIN_SHAPE *source = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                            if (local_max_x >= batch->min_x && batch->max_x > local_min_x &&
                                local_max_z >= batch->min_z && batch->max_z > local_min_z) {
                                while (count-- > 0) {
                                    if (TERRAIN_NSF_SHAPE_OVERLAP(source)) {
                                        platinrange = 1;
                                        if (cursor < limit && TERRAIN_NSF_MATERIAL_OK(source)) {
                                            (cursor++)->shape = source;
                                            ++shape_count;
                                        }
                                    }
                                    ++source;
                                }
                            } else {
                                source += count;
                            }
                            batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(source);
                        }
                    }
                } else if (group->chunk_type != -1) {
                    const f64 dy = ((scan->max_y + scan->min_y) * 0.5 - group->origin.y) *
                                   static_cast<TerrainQuery_s *>(TerI)->inverse_object_scale;
                    const f64 dx = (scan->max_x + scan->min_x) * 0.5 - group->origin.x;
                    const f64 dz = (scan->max_z + scan->min_z) * 0.5 - group->origin.z;
                    // Retail reuses the local minimum Y slot for the distance; Y is not read again here.
                    local_min_y = dx * dx + dy * dy + dz * dz;
                    if (local_min_y < (group->radius + scan_radius) * (group->radius + scan_radius)) {
                        TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
                        while (batch->marker >= 0) {
                            const i32 count = batch->shape_count;
                            TERRAIN_SHAPE *source = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                            for (i32 remaining = count; remaining > 0; --remaining, ++source) {
                                platinrange = 1;
                                if (cursor >= limit || !TERRAIN_NSF_MATERIAL_OK(source)) {
                                    continue;
                                }
                                vertices[0].x = source->vertices[0].x;
                                vertices[0].y = source->vertices[0].y;
                                vertices[0].z = source->vertices[0].z;
                                vertices[0].w = 0.0f;
                                vertices[1].x = source->vertices[1].x;
                                vertices[1].y = source->vertices[1].y;
                                vertices[1].z = source->vertices[1].z;
                                vertices[1].w = 0.0f;
                                vertices[2].x = source->vertices[2].x;
                                vertices[2].y = source->vertices[2].y;
                                vertices[2].z = source->vertices[2].z;
                                vertices[2].w = 0.0f;
                                NuVec4MtxTransformVU0x3(vertices, vertices, TERRAIN_NSF_MATRIX(group));
                                if (source->normals[1].y < 65535.0) {
                                    vertices[3].x = source->vertices[3].x;
                                    vertices[3].y = source->vertices[3].y;
                                    vertices[3].z = source->vertices[3].z;
                                    vertices[3].w = 0.0f;
                                    NuVec4MtxTransformVU0(&vertices[3], &vertices[3], TERRAIN_NSF_MATRIX(group));
                                } else {
                                    vertices[3] = vertices[2];
                                }
                                if (!((vertices[0].x > local_min_x || vertices[1].x > local_min_x ||
                                       vertices[2].x > local_min_x || vertices[3].x > local_min_x) &&
                                      (vertices[0].x < local_max_x || vertices[1].x < local_max_x ||
                                       vertices[2].x < local_max_x || vertices[3].x < local_max_x) &&
                                      (vertices[0].z > local_min_z || vertices[1].z > local_min_z ||
                                       vertices[2].z > local_min_z || vertices[3].z > local_min_z) &&
                                      (vertices[0].z < local_max_z || vertices[1].z < local_max_z ||
                                       vertices[2].z < local_max_z || vertices[3].z < local_max_z))) {
                                    continue;
                                }
                                TERRAIN_SHAPE *surface = &TerrainScaledShapeCursorPc[scaled_count];
                                memcpy(surface->material, source->material, 4);
                                surface->vertices[0].x = vertices[0].x;
                                surface->vertices[0].z = vertices[0].z;
                                surface->vertices[0].y = vertices[0].y;
                                surface->vertices[1].x = vertices[1].x;
                                surface->vertices[1].z = vertices[1].z;
                                surface->vertices[1].y = vertices[1].y;
                                surface->vertices[2].x = vertices[2].x;
                                surface->vertices[2].z = vertices[2].z;
                                surface->vertices[2].y = vertices[2].y;
                                if (source->normals[1].y < 65535.0) {
                                    surface->vertices[3].x = vertices[3].x;
                                    surface->vertices[3].z = vertices[3].z;
                                    surface->vertices[3].y = vertices[3].y;
                                    vertices[0].x = surface->vertices[1].x - surface->vertices[3].x;
                                    vertices[0].y = surface->vertices[1].y - surface->vertices[3].y;
                                    vertices[0].z = surface->vertices[1].z - surface->vertices[3].z;
                                    vertices[1].x = surface->vertices[2].x - surface->vertices[3].x;
                                    vertices[1].y = surface->vertices[2].y - surface->vertices[3].y;
                                    vertices[1].z = surface->vertices[2].z - surface->vertices[3].z;
                                    TERRAIN_NSF_CROSS(surface->normals[1], reinterpret_cast<NUVEC *>(&vertices[0]),
                                                      reinterpret_cast<NUVEC *>(&vertices[1]));
                                    const f32 length = NuFsqrt(surface->normals[1].x * surface->normals[1].x +
                                                               surface->normals[1].y * surface->normals[1].y +
                                                               surface->normals[1].z * surface->normals[1].z);
                                    const f32 inverse = length != 0.0f ? 1.0f / length : 0.0f;
                                    surface = &TerrainScaledShapeCursorPc[scaled_count];
                                    surface->normals[1].x *= inverse;
                                    surface->normals[1].y *= inverse;
                                    surface->normals[1].z *= inverse;
                                } else {
                                    surface->normals[1].y = 65536.0f;
                                }
                                vertices[0].x = surface->vertices[2].x - surface->vertices[0].x;
                                vertices[0].y = surface->vertices[2].y - surface->vertices[0].y;
                                vertices[0].z = surface->vertices[2].z - surface->vertices[0].z;
                                vertices[1].x = surface->vertices[1].x - surface->vertices[0].x;
                                vertices[1].y = surface->vertices[1].y - surface->vertices[0].y;
                                vertices[1].z = surface->vertices[1].z - surface->vertices[0].z;
                                TERRAIN_NSF_CROSS(surface->normals[0], reinterpret_cast<NUVEC *>(&vertices[0]),
                                                  reinterpret_cast<NUVEC *>(&vertices[1]));
                                const f32 length = NuFsqrt(surface->normals[0].x * surface->normals[0].x +
                                                           surface->normals[0].y * surface->normals[0].y +
                                                           surface->normals[0].z * surface->normals[0].z);
                                const f32 inverse = length != 0.0f ? 1.0f / length : 0.0f;
                                surface = &TerrainScaledShapeCursorPc[scaled_count];
                                surface->normals[0].x *= inverse;
                                surface->normals[0].y *= inverse;
                                surface->normals[0].z *= inverse;
                                (cursor++)->shape = surface;
                                ++shape_count;
                                ++scaled_count;
                            }
                            batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(source);
                        }
                    }
                }
            }
            TERRAIN_NSF_FINISH_GROUP(*indices);
        }
    }
    end_header = reinterpret_cast<TERRAIN_HANDLE_END_PC *>(group_header);
    end_header->terminator.header.shape_count = 0;
    end_header->terminator.header.terrain_group = 0;
    scan->min_x = (f32)(scan->min_x - 0.02f);
    scan->min_z = (f32)(scan->min_z - 0.02f);
    scan->max_x = (f32)(scan->max_x + 0.02f);
    scan->max_z = (f32)(scan->max_z + 0.02f);
    for (TERRAIN_SPATIAL_NODE *node = TERRAIN_NSF_SET->spatial_nodes; node != NULL;
         node = reinterpret_cast<TERRAIN_SPATIAL_CHUNK *>(reinterpret_cast<u8 *>(node) -
                                                          offsetof(TERRAIN_SPATIAL_CHUNK, node))
                    ->previous) {
        const u8 mask = static_cast<u8>(node->field_02 >> 8);
        if (mask != 0 && (mask & terrain_mask) == 0) {
            continue;
        }
        const u8 material = static_cast<u8>(node->field_02);
        for (i32 first = 0; first < node->point_count; first += 16) {
            NUVEC *points = &node->points[first];
            i32 end;
            if (points[0].y != 2147483648.0f) {
                if (!(points[1].y >= scan->min_x && points[0].y <= scan->max_x && points[3].y >= scan->min_z &&
                      points[2].y <= scan->max_z)) {
                    continue;
                }
                end = first + 16;
                if (end > node->point_count) {
                    end = node->point_count;
                }
            } else {
                end = node->point_count;
            }
            for (i32 i = first; i < end; ++i) {
                NUVEC *a = &node->points[i];
                NUVEC *b = &node->points[i + 1];
                if (((a->x >= scan->min_x && b->x <= scan->max_x) || (b->x >= scan->min_x && a->x <= scan->max_x)) &&
                    ((a->z >= scan->min_z && b->z <= scan->max_z) || (b->z >= scan->min_z && a->z <= scan->max_z)) &&
                    WallSplCount < 64) {
                    // Native header+4 is read and advanced unchecked; its valid storage is a caller/native-state
                    // domain.
                    end_header->wall_cursor->position = *a;
                    end_header->wall_cursor->material[0] = material;
                    end_header->wall_cursor->material[1] = mask;
                    end_header->wall_cursor->material[2] = 0;
                    end_header->wall_cursor->material[3] = 0;
                    ++end_header->wall_cursor;
                    end_header->wall_cursor->position = *b;
                    end_header->wall_cursor->material[0] = material;
                    end_header->wall_cursor->material[1] = mask;
                    end_header->wall_cursor->material[2] = 0;
                    end_header->wall_cursor->material[3] = 0;
                    ++end_header->wall_cursor;
                }
            }
        }
    }
    end_header->wall_end.shape_count = 0;
    end_header->wall_end.terrain_group = 0;
    TempStackPtrPc = reinterpret_cast<u8 *>(end_header + 1);
    NuScratchRelease();
    return reinterpret_cast<i16 *>(result);
}
#undef TERRAIN_NSF_SET
#undef TERRAIN_NSF_PLATFORM
#undef TERRAIN_NSF_MATRIX
#undef TERRAIN_NSF_SHAPE_OVERLAP
#undef TERRAIN_NSF_MATERIAL_OK
#undef TERRAIN_NSF_FINISH_GROUP
#undef TERRAIN_NSF_CROSS

extern "C" i16 *NewScanHandel(NUVEC *position, NUVEC *movement, f32 radius, i32 scan_type, i16 *subset) {
    if (CurrentTerrain == NULL)
        return NULL;
    TerrPlatDis = -1;
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    query->inverse_object_scale = 1.0f;
    TerI = query;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    f32 inverse_radius;
    if (radius == 0.0f)
        inverse_radius = 0.0f;
    else
        inverse_radius = 1.0f / radius;
    query->inverse_collision_radius = inverse_radius;
    query->collision_radius_sq = radius * radius;
    query->start_position.x = query->position.x = position->x;
    query->start_position.y = query->position.y = position->y;
    query->start_position.z = query->position.z = position->z;
    query->movement = *movement;
    query->start_movement = *movement;
    query->object_index = -1;
    query->hit_flags = NULL;
    query->scan_result = 1;
    i16 *handle;
    if (subset == NULL)
        handle = NewScanHandelFullPc(position, movement, radius, scan_type, 0xff);
    else
        handle = NewScanHandelSubsetPc(subset, position, movement, radius, 0xff);
    NuScratchRelease();
    return handle;
}

extern "C" i32 NewRayCastSetHandel(NUVEC *position, NUVEC *movement, f32 radius, f32 separation_epsilon,
                                   f32 compare_epsilon, i16 *handle, i32 scan_type) {
    plathitid = -1;
    TerrainShadowStatusPc = -1;
    TerrPoly = NULL;
    TerrWallInfo = 0;
    TerrainShadowGroupIndexPc = -1;
    if (CurrentTerrain == NULL)
        return 0;
    if (handle == NULL)
        return 0;
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    TerI = query;
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    TerrOverRideScan = NULL;
    query->inverse_object_scale = 1.0f;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    const f32 inverse_radius = radius == 0.0f ? 0.0f : TerrainDivideNonzeroPC(1.0f, radius);
    query = static_cast<TerrainQuery_s *>(TerI);
    query->inverse_collision_radius = inverse_radius;
    query->collision_radius_sq = radius * radius;
    query->start_position.x = query->position.x = position->x;
    query->start_position.y = query->position.y = position->y;
    query->start_position.z = query->position.z = position->z;
    query->movement = *movement;
    query->start_movement = *movement;
    query->object_index = -1;
    query->hit_flags = NULL;
    query->scan_result = 1;
    query->separation_epsilon = separation_epsilon;
    query->compare_epsilon = compare_epsilon;
    ScanTerrainHandel(scan_type, handle);
    query = static_cast<TerrainQuery_s *>(TerI);
    DerotateMovementVector();
    HitTerrain();
    query = static_cast<TerrainQuery_s *>(TerI);
    if (query->hit_type != 0) {
        RayImpact(movement);
        TerrainImpactNorm();
        query = static_cast<TerrainQuery_s *>(TerI);
        GetSurfaceInfoSurfaceNormalPc = query->movement_normal;
        TerrPoly = query->surface;
    }
    NuScratchRelease();
    return static_cast<TerrainQuery_s *>(TerI)->hit_type;
}

extern "C" i32 NewRayCastPlatForm(NUVEC *position, NUVEC *movement, f32 radius, f32 separation_epsilon,
                                  i32 platform_index, i32 terrain_mask) {
    plathitid = -1;
    TerrainShadowStatusPc = -1;
    TerrPoly = NULL;
    TerrWallInfo = 0;
    TerrainShadowGroupIndexPc = -1;
    if (CurrentTerrain == NULL)
        return 0;
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    query->inverse_object_scale = 1.0f;
    TerI = query;
    query->inverse_object_scale_sq = 1.0f;
    TerrOverRideScan = NULL;
    query->collision_radius = radius;
    if (radius == 0.0f) {
        query->inverse_collision_radius = 0.0f;
    } else {
        f32 inverse_radius = 1.0f / radius;
        query->inverse_collision_radius = inverse_radius;
    }
    query->collision_radius_sq = radius * radius;
    query->start_position.x = query->position.x = position->x;
    query->start_position.y = query->position.y = position->y;
    query->start_position.z = query->position.z = position->z;
    query->movement = *movement;
    query->start_movement = *movement;
    query->object_index = -1;
    query->hit_flags = NULL;
    query->scan_result = 1;
    query->separation_epsilon = separation_epsilon;
    query->compare_epsilon = 0.0f;
    ScanTerrainPlatform(static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->platforms[platform_index].terrain_group_index,
                        terrain_mask);
    query = static_cast<TerrainQuery_s *>(TerI);
    DerotateMovementVector();
    HitTerrain();
    query = static_cast<TerrainQuery_s *>(TerI);
    if (query->hit_type != 0) {
        RayImpact(movement);
        TerrainImpactNorm();
        query = static_cast<TerrainQuery_s *>(TerI);
        GetSurfaceInfoSurfaceNormalPc = query->movement_normal;
    }
    NuScratchRelease();
    return static_cast<TerrainQuery_s *>(TerI)->hit_type;
}

extern "C" {
    extern f32 TerrainShadowHeightPc;
    extern i16 TerrainShadowFloorIndexPc;
    extern u16 TerrainWaterCastIndexPc, TerrainSurfaceClassPc, TerrainPlatformsOffPc;
    extern TERRAIN_SHAPE *TerrainCastAuxPolygonPc;
    extern TERRAIN_SHAPE *TerrainScaleWorkBasePc;
    f32 NewCast(NUVEC *, f32, f32);
}

extern "C" f32 NewShadowHandelEx(NUVEC *position, f32 unused, f32 height_above, f32 height_below, i32 terrain_mask,
                                 i16 *handle) {
    (void)unused;
    if (CurrentTerrain == NULL)
        return LevelConfigFarDistance;
    if (handle == NULL)
        return 0.0f;
    TerrainShadowHeightPc = LevelConfigFarDistance;
    EShadY = LevelConfigFarDistance;
    GetSurfaceInfoWaterNormalPc.y = 1.0f;
    TerrainShadowStatusPc = -1;
    GetSurfaceInfoRoofHeightPc = LevelConfigFarDistance;
    TerrainShadowGroupIndexPc = -1;
    TerrainShadowFloorIndexPc = -1;
    GetSurfaceInfoSurfaceNormalPc.y = 1.0f;
    TerrainCastAuxPolygonPc = NULL;
    TerrainWaterCastIndexPc = 0;
    EShadowPolygonPc = NULL;
    ShadowRoofPolygonPc = NULL;
    TerrainSurfaceClassPc = 0;
    ShadPoly = NULL;
    TerI = NuScratchAlloc32(sizeof(TerrainQuery_s));
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(TerI);
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    query->inverse_object_scale = 1.0f;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = 0.1f;
    query->inverse_collision_radius = 10.0f;
    query->collision_radius_sq = ForcePushApproachInnerSquare;
    query->position.x = position->x;
    query->position.y = static_cast<f64>(position->y) - 20.0;
    query->position.z = position->z;
    query->object_index = -1;
    query->scan_result = 1;
    query->movement.x = query->movement.z = 0.0f;
    query->movement.y = 40.0f;
    TerrainScaledShapeCursorPc = TerrainScaleWorkBasePc;
    NUVEC scan_position = *position;
    ScanTerrainHandel(terrain_mask, handle);
    (void)NewCast(&scan_position, height_above, height_below);
    NuScratchRelease();
    TerrainPlatformsOffPc = 0;
    return scan_position.y;
}

extern "C" i32 TerrainTrackBack(NUVEC *position, NUVEC *direction, f32 radius, f32 backoff, NUVEC *result) {
    if (CurrentTerrain == NULL)
        return 0;
    if (TerrainShadowStatusPc == -1)
        return 0;
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    query->inverse_object_scale = 1.0f;
    TerI = query;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    f32 inverse_radius;
    if (radius != 0.0f)
        inverse_radius = 1.0f / radius;
    else
        inverse_radius = 0.0f;
    query->inverse_collision_radius = inverse_radius;
    query->collision_radius_sq = radius * radius;
    const f64 forward_radius = static_cast<f64>(radius) + 0.5;
    query->position.x = direction->x * forward_radius + position->x;
    query->position.y = direction->y * forward_radius + position->y;
    query->position.z = direction->z * forward_radius + position->z;
    const f64 reverse_radius = static_cast<f64>(radius) + static_cast<f64>(0.8f);
    query->movement.x = -direction->x * reverse_radius;
    query->movement.y = -direction->y * reverse_radius;
    query->movement.z = -direction->z * reverse_radius;
    query->object_index = -1;
    query->hit_flags = NULL;
    DerotateMovementVector();
    HitTerrPoly(TerrPoly, TerrainShadowStatusPc);
    query = static_cast<TerrainQuery_s *>(TerI);
    switch (query->hit_type) {
        case 1:
        case 2:
        case 3:
        case 4:
            query->hit_time -= backoff;
            if (query->hit_time < 0.0f)
                query->hit_time = 0.0f;
            result->x = query->movement.x * query->hit_time + query->position.x;
            result->y = query->movement.y * query->hit_time + query->position.y;
            result->z = query->movement.z * query->hit_time + query->position.z;
            break;
        case 0:
        case 17:
        case 18:
        case 19:
        case 20:
            result->x = query->position.x;
            result->y = query->position.y;
            result->z = query->position.z;
            break;
    }
    NuScratchRelease();
    return static_cast<TerrainQuery_s *>(TerI)->hit_type;
}

STATIC_ASSERT(offsetof(TERRAIN_TRACK_SLOT_PC, id) == 0);
STATIC_ASSERT(offsetof(TERRAIN_TRACK_SLOT_PC, wall_contact_state) == 0xa);
STATIC_ASSERT(offsetof(TERRAIN_TRACK_SLOT_PC, impact_normal) == 0xc);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, track_slots) == 0x198);
STATIC_ASSERT(offsetof(TERRAIN_AXIS_FREEDOM_SHAPE_PC, angle) == 0xc);
STATIC_ASSERT(offsetof(NUMTX, m30) == 0x30);
STATIC_ASSERT(offsetof(NUMTX, m31) == 0x34);
STATIC_ASSERT(offsetof(NUMTX, m32) == 0x38);

extern "C" void RotateTerrainPc(TERRAIN_SHAPE *surface) {
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(TerI);
    const f32 sin_pitch = NuTrigTable[(static_cast<i32>(query->movement_pitch) >> 1) & (NUTRIGTABLE_COUNT - 1)];
    const f32 cos_pitch =
        NuTrigTable[(static_cast<i32>(query->movement_pitch + 16384.0) >> 1) & (NUTRIGTABLE_COUNT - 1)];
    const f32 sin_yaw = NuTrigTable[(static_cast<i32>(query->movement_yaw) >> 1) & (NUTRIGTABLE_COUNT - 1)];
    const f32 cos_yaw = NuTrigTable[(static_cast<i32>(query->movement_yaw + 16384.0) >> 1) & (NUTRIGTABLE_COUNT - 1)];
    f32 rotated_z;
#define TERRAIN_RTP_VERTEX(index)                                                                                      \
    query->transformed_vertices[index].y =                                                                             \
        surface->vertices[index].y * cos_pitch - surface->vertices[index].z * sin_pitch;                               \
    rotated_z = surface->vertices[index].z * cos_pitch + surface->vertices[index].y * sin_pitch;                       \
    query->transformed_vertices[index].z = rotated_z * cos_yaw - surface->vertices[index].x * sin_yaw;                 \
    query->transformed_vertices[index].x = surface->vertices[index].x * cos_yaw + rotated_z * sin_yaw

    TERRAIN_RTP_VERTEX(0);
    TERRAIN_RTP_VERTEX(1);
    TERRAIN_RTP_VERTEX(2);
    if (surface->normals[1].y < 65536.0f) {
        TERRAIN_RTP_VERTEX(3);
    }
#undef TERRAIN_RTP_VERTEX
}

extern "C" void FullDeflectSizePc(NUVEC *normal, NUVEC *movement, NUVEC *result) {
    const f32 reverse_x = -movement->x;
    const f32 reverse_y = -movement->y;
    const f32 reverse_z = -movement->z;
    f32 deflection = reverse_y * normal->y + reverse_x * normal->x + reverse_z * normal->z;
    f32 original_length = (movement->z * movement->z) + (movement->y * movement->y + movement->x * movement->x);
    original_length = NuFsqrt(original_length);
    result->x = (normal->x * deflection) + movement->x;
    result->y = (normal->y * deflection) + movement->y;
    result->z = (normal->z * deflection) + movement->z;
    f32 deflected_length = NuFsqrt(result->x * result->x + result->y * result->y + result->z * result->z);
    if (deflected_length != 0.0f) {
        f32 scale = (original_length * 0.75) / deflected_length + 0.25;
        result->x *= scale;
        result->y *= scale;
        result->z *= scale;
    }
}

extern "C" void FullReflectTestPc(NUVEC *normal, NUVEC *movement, NUVEC *result) {
    const f32 reverse_x = -movement->x;
    const f32 reverse_y = -movement->y;
    const f32 reverse_z = -movement->z;
    f32 deflection = reverse_y * normal->y + reverse_x * normal->x + reverse_z * normal->z;
    f32 original_length = movement->y * movement->y + movement->x * movement->x + movement->z * movement->z;
    original_length = NuFsqrt(original_length);
    result->x = (normal->x * deflection) * 2.0 + movement->x;
    result->y = (normal->y * deflection) * 2.0 + movement->y;
    result->z = (normal->z * deflection) * 2.0 + movement->z;
    f32 deflected_length = NuFsqrt(result->x * result->x + result->y * result->y + result->z * result->z);
    if (deflected_length != 0.0f) {
        f32 scale = original_length / deflected_length;
        result->x *= scale;
        result->y *= scale;
        result->z *= scale;
    }
}

extern "C" void CubeImpactPc(NUMTX *current, NUMTX *previous, NUVEC *normal, f32 scale, NUVEC *impact) {
    NUVEC corners[8] = {
        {-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f},
        {-1.0f, -1.0f, 1.0f},  {1.0f, -1.0f, 1.0f},  {1.0f, 1.0f, 1.0f},  {-1.0f, 1.0f, 1.0f},
    };
    NUVEC4 transformed[8], old_position;
    f32 closest = 10000.0f;
    i32 selected = 0;
    for (i32 index = 0; index < 8; ++index) {
        transformed[index].x = corners[index].x;
        transformed[index].y = corners[index].y;
        transformed[index].z = corners[index].z;
        transformed[index].w = 0.0f;
        NuVec4MtxTransformVU0(&old_position, &transformed[index], previous);
        NuVec4MtxTransformVU0(&transformed[index], &transformed[index], current);
        const f32 current_distance =
            normal->x * transformed[index].x + normal->y * transformed[index].y + normal->z * transformed[index].z;
        const f32 old_distance = normal->x * old_position.x + normal->y * old_position.y + normal->z * old_position.z;
        if (current_distance > old_distance && current_distance < closest) {
            closest = current_distance;
            selected = index;
        }
    }
    impact->x = scale * transformed[selected].x + current->m30;
    impact->y = scale * transformed[selected].y + current->m31;
    impact->z = scale * transformed[selected].z + current->m32;
}

extern "C" void TerrainWallSideSlidePc(NUVEC *movement, void *id, f32 speed, f32 upward_scale, NUANG angle) {
    TERRAIN_SET_PC *terrain = static_cast<TERRAIN_SET_PC *>(CurrentTerrain);
    if (terrain == NULL)
        return;
    TERRAIN_TRACK_SLOT_PC *track;
    for (i32 index = 0; index < 64; ++index) {
        if (terrain->track_slots[index].id == id) {
            track = &terrain->track_slots[index];
            goto found;
        }
    }
    track = NULL;
found:
    CurTrackInfo = track;
    if (CurTrackInfo == NULL || CurTrackInfo->wall_contact_state == 0)
        return;
    if (speed != 0.0f) {
        NUVEC slide = {0.0f, 0.0f, speed};
        NuVecRotateY(&slide, &slide, angle);
        track = CurTrackInfo;
        FullDeflect(&track->impact_normal, &slide, &slide);
        slide.x += speed * track->impact_normal.x * static_cast<f64>(0.4f);
        slide.z += speed * track->impact_normal.z * static_cast<f64>(0.4f);
        movement->x += slide.x;
        movement->z = slide.z + movement->z;
    }
    if (movement->y > 0.0f)
        movement->y = movement->y * upward_scale;
}

#define TERRAIN_TPE_EDGE(first_vertex, second_vertex)                                                                  \
    first->x = groups[query->terrain_group_index].origin.x + query->surface->vertices[first_vertex].x;                 \
    first->y = groups[query->terrain_group_index].origin.y + query->surface->vertices[first_vertex].y;                 \
    first->z = groups[query->terrain_group_index].origin.z + query->surface->vertices[first_vertex].z;                 \
    second->x = groups[query->terrain_group_index].origin.x + query->surface->vertices[second_vertex].x;               \
    second->y = groups[query->terrain_group_index].origin.y + query->surface->vertices[second_vertex].y;               \
    second->z = groups[query->terrain_group_index].origin.z + query->surface->vertices[second_vertex].z
extern "C" void TerrainPolyEdgePc(NUVEC *first, NUVEC *second) {
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(TerI);
    if ((query->hit_type & 0xf) != TERRAIN_HIT_TYPE_CYLINDER)
        return;
    TERRAIN_GROUP *groups;
    switch (query->hit_edge) {
        case 1:
            groups = static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->groups;
            TERRAIN_TPE_EDGE(0, 1);
            break;
        case 2:
            groups = static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->groups;
            TERRAIN_TPE_EDGE(1, 2);
            break;
        case 3:
            groups = static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->groups;
            TERRAIN_TPE_EDGE(2, 0);
            break;
        case 4:
            groups = static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->groups;
            TERRAIN_TPE_EDGE(1, 3);
            break;
        case 5:
            groups = static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->groups;
            TERRAIN_TPE_EDGE(3, 2);
            break;
        default:
            break;
    }
}
#undef TERRAIN_TPE_EDGE

extern "C" void NewTerrAxisFreedomPc(TERRAIN_AXIS_FREEDOM_SHAPE_PC *shape, NUVEC *position) {
    f64 angle = shape->angle;
    TerrShape = shape;
    TerrShapeAdjCnt = 2;

    NUVEC axis;
    axis.x = NuTrigTable[static_cast<i32>(angle + 16384.0) >> 1 & 0x7fff];
    axis.y = 0.0f;
    axis.z = -NuTrigTable[static_cast<i32>(shape->angle) >> 1 & 0x7fff];
    TerrainSideClamp(&axis, position);
}
