#include "globals.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nucore/numem.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include <string.h>
#include <math.h>

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
    TERRAIN_TRACK_FLAG_NONE = 0
};

struct ShadowScanWriter {
    u8 *group_header;
    TERRAIN_POLYGON_INFO **cursor;
    u8 *limit;
    i32 shape_count;
};

extern "C" {

    void NewScan(NUVEC *position, i32 terrain_mask, i32 scan_platforms) {
        NUVEC saved_position = *position;
        i32 hit_index = -1;
        i32 oldest_index = -1;
        i32 oldest_age = 0x7fffffff;
        TERRAIN_SCAN_WORD *group_header;
        TERRAIN_SCAN_WORD *cursor;
        TERRAIN_SCAN_WORD *limit;
        TERRAIN_SCAN_WORD *entry;
        i32 shape_count;
        f32 max_x;
        f32 max_z;
        f32 min_x;
        f32 min_z;
        f32 local_max_x;
        f32 local_max_z;
        f32 local_min_x;
        f32 local_min_z;

        for (i32 i = 0; i < 16; ++i) {
            TERRAIN_INDEX_LEVEL *level = &CurrentTerrain->index_levels[i];
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
            group_header = TerI->scan_words;
            max_x = saved_position.x + 0.1f;
            max_z = saved_position.z + 0.1f;
            CurrentTerrain->index_levels[hit_index].cache_age = 8;
            min_x = saved_position.x - 0.1f;
            min_z = saved_position.z - 0.1f;
            entry = CurrentTerrain->index_levels[hit_index].scan_words;
            cursor = group_header + 1;
            shape_count = 0;
            limit = cursor + 508;
            for (;;) {
                const i16 count = entry->header.shape_count;
                if (count <= 0) {
                    break;
                }
                const i32 group_index = entry->header.terrain_group;
                TERRAIN_SCAN_WORD *shapes = entry + 1;
                TERRAIN_GROUP *group = &CurrentTerrain->groups[group_index];
                if ((max_x >= group->bounds_min.x && max_z >= group->bounds_min.z && group->bounds_max.x >= min_x &&
                     group->bounds_max.z > min_z) &&
                    group->chunk_type != -1) {
                    local_max_x = max_x - group->origin.x;
                    local_max_z = max_z - group->origin.z;
                    local_min_x = min_x - group->origin.x;
                    local_min_z = min_z - group->origin.z;
                    for (i32 remaining = count; remaining > 0; --remaining, ++shapes) {
                        TERRAIN_POLYGON_INFO *shape = shapes->shape;
                        if ((local_max_x >= shape->min_x && shape->max_x > local_min_x && local_max_z >= shape->min_z &&
                             shape->max_z > local_min_z) &&
                            cursor < limit && TERRAIN_SHAPE_MATERIAL_OK(shape, terrain_mask)) {
                            (cursor++)->shape = shape;
                            ++shape_count;
                        }
                    }
                    if (shape_count != 0) {
                        group_header->header.shape_count = (i16)shape_count;
                        group_header->header.terrain_group = (i16)(group_index);
                        group_header = cursor;
                        cursor++;
                        shape_count = 0;
                    }
                } else {
                    shapes += count;
                }
                entry = shapes;
            }
        } else {
            max_x = saved_position.x + 1.0f;
            max_z = saved_position.z + 1.0f;
            group_header = CurrentTerrain->index_levels[oldest_index].scan_words;
            cursor = group_header + 1;
            min_x = saved_position.x - 1.0f;
            limit = cursor + 508;
            min_z = saved_position.z - 1.0f;
            shape_count = 0;
            for (i32 cell_index = 0; cell_index < CurrentTerrain->used_cell_count; ++cell_index) {
                if (!(max_x >= CurrentTerrain->cells[cell_index].min_x && max_z >= CurrentTerrain->cells[cell_index].min_z &&
                      CurrentTerrain->cells[cell_index].max_x >= min_x && CurrentTerrain->cells[cell_index].max_z >= min_z)) {
                    continue;
                }
                i16 *group_indices = CurrentTerrain->group_indices + CurrentTerrain->cells[cell_index].first_group;
                for (i32 cell_group = 0; cell_group < CurrentTerrain->cells[cell_index].group_count; ++cell_group, ++group_indices) {
                    TERRAIN_GROUP *group = &CurrentTerrain->groups[*group_indices];
                    if (!(max_x >= group->bounds_min.x && max_z >= group->bounds_min.z && group->bounds_max.x >= min_x &&
                          group->bounds_max.z > min_z) ||
                        group->chunk_type == -1) {
                        continue;
                    }
                    local_max_x = max_x - group->origin.x;
                    local_max_z = max_z - group->origin.z;
                    local_min_x = min_x - group->origin.x;
                    local_min_z = min_z - group->origin.z;
                    if (group->scene_index < 0) {
                        TerrainSkinAllocate(group);
                    }
                    TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
                    while (batch->marker >= 0) {
                        const i16 count = batch->shape_count;
                        TERRAIN_POLYGON_INFO *shape = reinterpret_cast<TERRAIN_POLYGON_INFO *>(batch + 1);
                        if ((local_max_x >= batch->min_x && batch->max_x > local_min_x && local_max_z >= batch->min_z &&
                             batch->max_z > local_min_z)) {
                            i32 remaining = count;
                            while (remaining > 0) {
                                if ((local_max_x >= shape->min_x && shape->max_x > local_min_x && local_max_z >= shape->min_z &&
                                     shape->max_z > local_min_z) &&
                                    cursor < limit) {
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
                    if (shape_count != 0) {
                        group_header->header.shape_count = (i16)shape_count;
                        group_header->header.terrain_group = *group_indices;
                        group_header = cursor;
                        cursor++;
                        shape_count = 0;
                    }
                }
            }
            CurrentTerrain->index_levels[oldest_index].cache_age = 8;
            CurrentTerrain->index_levels[oldest_index].center_x = position->x;
            CurrentTerrain->index_levels[oldest_index].center_z = position->z;
            group_header->header.shape_count = 0;
            group_header->header.terrain_group = 0;
            shape_count = 0;
            group_header = TerI->scan_words;
            cursor = group_header + 1;
            limit = cursor + 508;
            max_x = saved_position.x + 0.1f;
            max_z = saved_position.z + 0.1f;
            min_x = saved_position.x - 0.1f;
            min_z = saved_position.z - 0.1f;
            entry = CurrentTerrain->index_levels[oldest_index].scan_words;
            for (;;) {
                const i16 count = entry->header.shape_count;
                if (count <= 0) {
                    break;
                }
                const i32 group_index = entry->header.terrain_group;
                TERRAIN_SCAN_WORD *shapes = entry + 1;
                TERRAIN_GROUP *group = &CurrentTerrain->groups[group_index];
                if ((max_x >= group->bounds_min.x && max_z >= group->bounds_min.z && group->bounds_max.x >= min_x &&
                     group->bounds_max.z > min_z) &&
                    group->chunk_type != -1) {
                    local_max_x = max_x - group->origin.x;
                    local_max_z = max_z - group->origin.z;
                    local_min_x = min_x - group->origin.x;
                    local_min_z = min_z - group->origin.z;
                    for (i32 remaining = count; remaining > 0; --remaining, ++shapes) {
                        TERRAIN_POLYGON_INFO *shape = shapes->shape;
                        if ((local_max_x >= shape->min_x && shape->max_x > local_min_x && local_max_z >= shape->min_z &&
                             shape->max_z > local_min_z) &&
                            cursor < limit && TERRAIN_SHAPE_MATERIAL_OK(shape, terrain_mask)) {
                            (cursor++)->shape = shape;
                            ++shape_count;
                        }
                    }
                    if (shape_count != 0) {
                        group_header->header.shape_count = (i16)shape_count;
                        group_header->header.terrain_group = (i16)(group_index);
                        group_header = cursor;
                        cursor++;
                        shape_count = 0;
                    }
                } else {
                    shapes += count;
                }
                entry = shapes;
            }
        }

        min_x = min_x - 0.05f;
        max_x = max_x + 0.05f;
        min_z = min_z - 0.05f;
        max_z = max_z + 0.05f;
        if (scan_platforms != 0) {
            i16 *platform_groups = CurrentTerrain->group_indices + CurrentTerrain->platform_cell.first_group;
            for (i32 platform_index = 0; platform_index < CurrentTerrain->platform_cell.group_count; ++platform_index, ++platform_groups) {
                TERRAIN_GROUP *group = &CurrentTerrain->groups[*platform_groups];
                TERRAIN_PLATFORM *platform = &TERRAIN_GROUP_PLATFORM(group);
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
                    if (TERRAIN_GROUP_MATRIX(group)->m30 > TERRAIN_GROUP_PLATFORM(group).previous_matrix.m30) {
                        local_min_x = min_x - group->origin.x;
                        local_max_x = (f32)((TERRAIN_GROUP_MATRIX(group)->m30 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m30) * 1.5 +
                                            max_x - group->origin.x);
                    } else {
                        local_max_x = max_x - group->origin.x;
                        local_min_x = (f32)((TERRAIN_GROUP_MATRIX(group)->m30 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m30) * 1.5 +
                                            min_x - group->origin.x);
                    }
                    if (TERRAIN_GROUP_MATRIX(group)->m32 > TERRAIN_GROUP_PLATFORM(group).previous_matrix.m32) {
                        local_min_z = min_z - group->origin.z;
                        local_max_z = (f32)((TERRAIN_GROUP_MATRIX(group)->m32 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m32) * 1.5 +
                                            max_z - group->origin.z);
                    } else {
                        local_max_z = max_z - group->origin.z;
                        local_min_z = (f32)((TERRAIN_GROUP_MATRIX(group)->m32 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m32) * 1.5 +
                                            min_z - group->origin.z);
                    }
                } else {
                    local_max_x = max_x - group->origin.x;
                    local_min_x = min_x - group->origin.x;
                    local_max_z = max_z - group->origin.z;
                    local_min_z = min_z - group->origin.z;
                }
                if (!(local_max_x >= group->bounds_min.x && local_max_z >= group->bounds_min.z && group->bounds_max.x >= local_min_x &&
                      group->bounds_max.z > local_min_z) ||
                    group->chunk_type == -1) {
                    continue;
                }
                TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
                while (batch->marker >= 0) {
                    TERRAIN_POLYGON_INFO *shape = reinterpret_cast<TERRAIN_POLYGON_INFO *>(batch + 1);
                    const i32 count = batch->shape_count;
                    if ((local_max_x >= batch->min_x && batch->max_x > local_min_x && local_max_z >= batch->min_z &&
                         batch->max_z > local_min_z)) {
                        for (i32 remaining = count; remaining > 0; --remaining, ++shape) {
                            if ((local_max_x >= shape->min_x && shape->max_x > local_min_x && local_max_z >= shape->min_z &&
                                 shape->max_z > local_min_z) &&
                                cursor < limit && TERRAIN_SHAPE_MATERIAL_OK(shape, terrain_mask)) {
                                (cursor++)->shape = shape;
                                ++shape_count;
                            }
                        }
                    } else {
                        shape += count;
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shape);
                }
                if (shape_count != 0) {
                    group_header->header.shape_count = (i16)shape_count;
                    group_header->header.terrain_group = *platform_groups;
                    group_header = cursor;
                    cursor++;
                    shape_count = 0;
                }
            }
        }
        group_header->header.shape_count = 0;
        group_header->header.terrain_group = 0;
    }

    void ScanTerrainHandel(i32 terrain_mask, i16 *handle) {
        TERRAIN_SCAN_WORD *group_header;
        TERRAIN_SCAN_WORD *cursor;
        TERRAIN_SCAN_WORD *limit;
        i32 shape_count;
        f32 max_x;
        f32 min_x;
        f32 max_z;
        f32 min_z;
        f32 max_y;
        f32 min_y;
        f32 scan_radius;
        TERRAIN_WALL_POINT **walls;
        TERRAIN_WALL_POINT *wall_list;
        i32 wall_count;
        TerrainQuery_s *query;

        if (handle == NULL) {
            return;
        }
        query = TerI;
        group_header = query->scan_words;
        cursor = query->scan_words + 1;
        limit = cursor + 508;
        platinrange = 0;
        query->scan_group_index = -1;
        switch (query->scan_result) {
            default: {
                const f32 movement_sq =
                    query->movement.x * query->movement.x + query->movement.y * query->movement.y + query->movement.z * query->movement.z;
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
                    min_x = query->position.x - 0.02f - query->collision_radius;
                    max_x = query->movement.x + query->position.x + 0.02f + query->collision_radius;
                } else {
                    min_x = query->movement.x + query->position.x - 0.02f - query->collision_radius;
                    max_x = query->position.x + 0.02f + query->collision_radius;
                }
                if (query->movement.y > 0.0f) {
                    min_y = query->position.y - 0.02f - query->collision_radius;
                    max_y = query->movement.y + query->position.y + 0.02f + query->collision_radius;
                } else {
                    min_y = query->movement.y + query->position.y - 0.02f - query->collision_radius;
                    max_y = query->position.y + 0.02f + query->collision_radius;
                }
                if (query->movement.z > 0.0f) {
                    min_z = query->position.z - 0.02f - query->collision_radius;
                    max_z = query->movement.z + query->position.z + 0.02f + query->collision_radius;
                } else {
                    min_z = query->movement.z + query->position.z - 0.02f - query->collision_radius;
                    max_z = query->position.z + 0.02f + query->collision_radius;
                }
                extent_sq = (min_x - max_x) * (min_x - max_x) + (min_y - max_y) * (min_y - max_y) + (min_z - max_z) * (min_z - max_z);
                scan_radius = NuFsqrt((f32)(extent_sq + (f64)1.02f));
                break;
            }
        }
        shape_count = 0;
        while (handle[0] > 0) {
            const i32 group_index = handle[1];
            TERRAIN_SCAN_WORD *shapes = reinterpret_cast<TERRAIN_SCAN_WORD *>(handle) + 1;
            TERRAIN_GROUP *group = &CurrentTerrain->groups[group_index];
            f32 local_min_x;
            f32 local_max_x;
            f32 local_min_z;
            f32 local_max_z;
            f32 local_min_y;
            f32 local_max_y;
            TERRAIN_PLATFORM *platform;
            if (group->chunk_type == 1 && (platform = &TERRAIN_GROUP_PLATFORM(group))->scene_object != NULL) {
                if (TERRAIN_GROUP_MATRIX(group)->m30 > TERRAIN_GROUP_PLATFORM(group).previous_matrix.m30) {
                    local_min_x = min_x - group->origin.x;
                    local_max_x = (f32)((TERRAIN_GROUP_MATRIX(group)->m30 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m30) * 1.5 +
                                        max_x - group->origin.x);
                } else {
                    local_max_x = max_x - group->origin.x;
                    local_min_x = (f32)((TERRAIN_GROUP_MATRIX(group)->m30 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m30) * 1.5 +
                                        min_x - group->origin.x);
                }
                if (TERRAIN_GROUP_MATRIX(group)->m31 > TERRAIN_GROUP_PLATFORM(group).previous_matrix.m31) {
                    local_min_y = min_y - group->origin.y;
                    local_max_y = (f32)((TERRAIN_GROUP_MATRIX(group)->m31 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m31) * 1.5 +
                                        max_y - group->origin.y);
                } else {
                    local_max_y = max_y - group->origin.y;
                    local_min_y = (f32)((TERRAIN_GROUP_MATRIX(group)->m31 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m31) * 1.5 +
                                        min_y - group->origin.y);
                }
                if (TERRAIN_GROUP_MATRIX(group)->m32 > TERRAIN_GROUP_PLATFORM(group).previous_matrix.m32) {
                    local_min_z = min_z - group->origin.z;
                    local_max_z = (f32)((TERRAIN_GROUP_MATRIX(group)->m32 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m32) * 1.5 +
                                        max_z - group->origin.z);
                } else {
                    local_max_z = max_z - group->origin.z;
                    local_min_z = (f32)((TERRAIN_GROUP_MATRIX(group)->m32 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m32) * 1.5 +
                                        min_z - group->origin.z);
                }
                if (group->scene_index == TerrPlatDis) {
                    shapes += handle[0];
                } else if ((platform->flags & TERRAIN_PLATFORM_FLAG_ROTATING) == 0) {
                    if ((local_max_x >= group->bounds_min.x && local_max_y >= group->bounds_min.y && local_max_z >= group->bounds_min.z &&
                         group->bounds_max.x >= local_min_x && group->bounds_max.y > local_min_y && group->bounds_max.z > local_min_z)) {
                        for (i32 remaining = handle[0]; remaining > 0; --remaining, ++shapes) {
                            TERRAIN_POLYGON_INFO *shape = shapes->shape;
                            if ((local_max_x >= shape->min_x && shape->max_x > local_min_x && local_max_z >= shape->min_z &&
                                 shape->max_z > local_min_z && local_max_y >= shape->min_y && shape->max_y > local_min_y) &&
                                cursor < limit && TERRAIN_SHAPE_MATERIAL_OK(shape, terrain_mask)) {
                                (cursor++)->shape = shape;
                                ++shape_count;
                            }
                        }
                        if (shape_count != 0) {
                            group_header->header.shape_count = (i16)shape_count;
                            group_header->header.terrain_group = (i16)(group_index);
                            group_header = cursor;
                            cursor++;
                            shape_count = 0;
                        }
                    } else {
                        shapes += handle[0];
                    }
                } else {
                    const f64 dy = ((min_y + max_y) * NuQFntHalfScale - group->origin.y) * TerI->inverse_object_scale;
                    const f64 dx = (min_x + max_x) * NuQFntHalfScale - group->origin.x;
                    const f64 dz = (min_z + max_z) * NuQFntHalfScale - group->origin.z;
                    const f32 distance_sq = dx * dx + dy * dy + dz * dz;
                    if (distance_sq < (group->radius + scan_radius) * (group->radius + scan_radius)) {
                        for (i32 remaining = handle[0]; remaining > 0; --remaining, ++shapes) {
                            TERRAIN_POLYGON_INFO *shape = shapes->shape;
                            if (cursor < limit && TERRAIN_SHAPE_MATERIAL_OK(shape, terrain_mask) &&
                                ((shape->vertices[0].x > local_min_x || shape->vertices[1].x > local_min_x ||
                                  shape->vertices[2].x > local_min_x || shape->vertices[3].x > local_min_x) &&
                                 (shape->vertices[0].x < local_max_x || shape->vertices[1].x < local_max_x ||
                                  shape->vertices[2].x < local_max_x || shape->vertices[3].x < local_max_x) &&
                                 (shape->vertices[0].z > local_min_z || shape->vertices[1].z > local_min_z ||
                                  shape->vertices[2].z > local_min_z || shape->vertices[3].z > local_min_z) &&
                                 (shape->vertices[0].z < local_max_z || shape->vertices[1].z < local_max_z ||
                                  shape->vertices[2].z < local_max_z || shape->vertices[3].z < local_max_z))) {
                                (cursor++)->shape = shape;
                                ++shape_count;
                            }
                        }
                        if (shape_count != 0) {
                            group_header->header.shape_count = (i16)shape_count;
                            group_header->header.terrain_group = (i16)(group_index);
                            group_header = cursor;
                            cursor++;
                            shape_count = 0;
                        }
                    } else {
                        shapes += handle[0];
                    }
                }
            } else if ((max_x >= group->bounds_min.x && max_y >= group->bounds_min.y && max_z >= group->bounds_min.z &&
                        group->bounds_max.x >= min_x && group->bounds_max.y > min_y && group->bounds_max.z > min_z)) {
                local_max_x = max_x - group->origin.x;
                local_min_x = min_x - group->origin.x;
                local_max_y = max_y - group->origin.y;
                local_min_y = min_y - group->origin.y;
                local_max_z = max_z - group->origin.z;
                local_min_z = min_z - group->origin.z;
                for (i32 remaining = handle[0]; remaining > 0; --remaining, ++shapes) {
                    TERRAIN_POLYGON_INFO *shape = shapes->shape;
                    if ((local_max_x >= shape->min_x && shape->max_x > local_min_x && local_max_z >= shape->min_z &&
                         shape->max_z > local_min_z && local_max_y >= shape->min_y && shape->max_y > local_min_y) &&
                        cursor < limit && TERRAIN_SHAPE_MATERIAL_OK(shape, terrain_mask)) {
                        (cursor++)->shape = shape;
                        ++shape_count;
                    }
                }
                if (shape_count != 0) {
                    group_header->header.shape_count = (i16)shape_count;
                    group_header->header.terrain_group = (i16)(group_index);
                    group_header = cursor;
                    cursor++;
                    shape_count = 0;
                }
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
        min_x = min_x - 0.02f;
        min_z = min_z - 0.02f;
        max_x = max_x + 0.02f;
        max_z = max_z + 0.02f;
        wall_list = WallSplList;
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

    void ScanTerrainPlatform(i32 group_index, i32 terrain_mask) {
        TERRAIN_SCAN_WORD *group_header;
        TERRAIN_SCAN_WORD *cursor;
        TERRAIN_SCAN_WORD *limit;
        i32 shape_count;
        i32 scaled_count;
        f32 max_x;
        f32 min_x;
        f32 max_z;
        f32 min_z;
        f32 max_y;
        f32 min_y;
        f32 scan_radius_sq;
        f32 scan_radius;
        NUVEC4 *vertices;
        TERRAIN_GROUP *group;
        TERRAIN_PLATFORM *platform;
        f32 local_min_x;
        f32 local_max_x;
        f32 local_min_z;
        f32 local_max_z;
        f32 local_min_y;
        f32 local_max_y;

        TerrainQuery_s *query;
        scaled_count = 0;
        vertices = reinterpret_cast<NUVEC4 *>((reinterpret_cast<u32>(NuScratchAlloc32(0xd0)) + 31) & ~u32(15));
        ScaleTerrain = ScaleTerrainT1;
        query = TerI;
        group_header = query->scan_words;
        cursor = reinterpret_cast<TERRAIN_SCAN_WORD *>(query->scan_list_storage) + 1;
        limit = cursor + 508;
        shape_count = 0;
        platinrange = 0;
        query->scan_group_index = -1;
        switch (query->scan_result) {
            default: {
                const f32 movement_sq =
                    query->movement.x * query->movement.x + query->movement.y * query->movement.y + query->movement.z * query->movement.z;
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
                extent_sq = (min_x - max_x) * (min_x - max_x) + (min_y - max_y) * (min_y - max_y) + (min_z - max_z) * (min_z - max_z);
                scan_radius = NuFsqrt((f32)(extent_sq + (query->collision_radius + (f64)1.02f)));
                break;
            }
        }
        query = TerI;
        query->scan_list = group_header;
        min_x = (f32)(min_x - 0.05f);
        max_x = (f32)(max_x + 0.05f);
        min_y = (f32)(min_y - 0.05f);
        max_y = (f32)(max_y + 0.05f);
        min_z = (f32)(min_z - 0.05f);
        max_z = (f32)(max_z + 0.05f);
        group = &CurrentTerrain->groups[group_index];
        platform = &TERRAIN_GROUP_PLATFORM(group);
        if (platform->scene_object != NULL) {
            if (TERRAIN_GROUP_MATRIX(group)->m30 > TERRAIN_GROUP_PLATFORM(group).previous_matrix.m30) {
                local_min_x = min_x - group->origin.x;
                local_max_x = (f32)((TERRAIN_GROUP_MATRIX(group)->m30 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m30) * 1.5 + max_x -
                                    group->origin.x);
            } else {
                local_max_x = max_x - group->origin.x;
                local_min_x = (f32)((TERRAIN_GROUP_MATRIX(group)->m30 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m30) * 1.5 + min_x -
                                    group->origin.x);
            }
            if (TERRAIN_GROUP_MATRIX(group)->m31 > TERRAIN_GROUP_PLATFORM(group).previous_matrix.m31) {
                local_min_y = min_y - group->origin.y;
                local_max_y = (f32)((TERRAIN_GROUP_MATRIX(group)->m31 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m31) * 1.5 + max_y -
                                    group->origin.y);
            } else {
                local_max_y = max_y - group->origin.y;
                local_min_y = (f32)((TERRAIN_GROUP_MATRIX(group)->m31 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m31) * 1.5 + min_y -
                                    group->origin.y);
            }
            if (TERRAIN_GROUP_MATRIX(group)->m32 > TERRAIN_GROUP_PLATFORM(group).previous_matrix.m32) {
                local_min_z = min_z - group->origin.z;
                local_max_z = (f32)((TERRAIN_GROUP_MATRIX(group)->m32 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m32) * 1.5 + max_z -
                                    group->origin.z);
            } else {
                local_max_z = max_z - group->origin.z;
                local_min_z = (f32)((TERRAIN_GROUP_MATRIX(group)->m32 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m32) * 1.5 + min_z -
                                    group->origin.z);
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
            // Reuses the local minimum X slot for the distance, so the vertex test below sees it.
            local_min_x = dx * dx + dy * dy + dz * dz;
            if (group->chunk_type != -1 && local_min_x < (group->radius + scan_radius) * (group->radius + scan_radius)) {
                TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
                while (batch->marker >= 0) {
                    const i32 count = batch->shape_count;
                    TERRAIN_POLYGON_INFO *source = reinterpret_cast<TERRAIN_POLYGON_INFO *>(batch + 1);
                    for (i32 remaining = count; remaining > 0; --remaining, ++source) {
                        platinrange = 1;
                        if (cursor >= limit || !TERRAIN_SHAPE_MATERIAL_OK(source, terrain_mask)) {
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
                        NuVec4MtxTransformVU0x3(vertices, vertices, TERRAIN_GROUP_MATRIX(group));
                        if (source->normals[1].y < 65535.0) {
                            vertices[3].x = source->vertices[3].x;
                            vertices[3].y = source->vertices[3].y;
                            vertices[3].z = source->vertices[3].z;
                            vertices[3].w = 0.0f;
                            NuVec4MtxTransformVU0(&vertices[3], &vertices[3], TERRAIN_GROUP_MATRIX(group));
                        } else {
                            vertices[3] = vertices[2];
                        }
                        if (!((vertices[0].x > local_min_x || vertices[1].x > local_min_x || vertices[2].x > local_min_x ||
                               vertices[3].x > local_min_x) &&
                              (vertices[0].x < local_max_x || vertices[1].x < local_max_x || vertices[2].x < local_max_x ||
                               vertices[3].x < local_max_x) &&
                              (vertices[0].z > local_min_z || vertices[1].z > local_min_z || vertices[2].z > local_min_z ||
                               vertices[3].z > local_min_z) &&
                              (vertices[0].z < local_max_z || vertices[1].z < local_max_z || vertices[2].z < local_max_z ||
                               vertices[3].z < local_max_z))) {
                            continue;
                        }
                        TERRAIN_POLYGON_INFO *surface = &ScaleTerrain[scaled_count];
                        memcpy(surface->material, source->material, 4);
                        query = TerI;
                        surface->vertices[0].x = vertices[0].x;
                        surface->vertices[0].z = vertices[0].z;
                        surface->vertices[0].y = (group->origin.y + vertices[0].y) * query->inverse_object_scale - group->origin.y;
                        surface->vertices[1].x = vertices[1].x;
                        surface->vertices[1].z = vertices[1].z;
                        surface->vertices[1].y = (vertices[1].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                        surface->vertices[2].x = vertices[2].x;
                        surface->vertices[2].z = vertices[2].z;
                        surface->vertices[2].y = (vertices[2].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                        if (source->normals[1].y < 65535.0) {
                            surface->vertices[3].x = vertices[3].x;
                            surface->vertices[3].z = vertices[3].z;
                            surface->vertices[3].y = (vertices[3].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                            vertices[0].x = surface->vertices[1].x - surface->vertices[3].x;
                            vertices[0].y = surface->vertices[1].y - surface->vertices[3].y;
                            vertices[0].z = surface->vertices[1].z - surface->vertices[3].z;
                            vertices[1].x = surface->vertices[2].x - surface->vertices[3].x;
                            vertices[1].y = surface->vertices[2].y - surface->vertices[3].y;
                            vertices[1].z = surface->vertices[2].z - surface->vertices[3].z;
                            {
                                NUVEC *cross_a = &vertices[0].xyz;
                                NUVEC *cross_b = &vertices[1].xyz;
                                NUVEC cross;
                                cross.x = cross_b->z * cross_a->y - cross_b->y * cross_a->z;
                                cross.y = cross_a->z * cross_b->x - cross_b->z * cross_a->x;
                                cross.z = cross_b->y * cross_a->x - cross_b->x * cross_a->y;
                                surface->normals[1] = cross;
                            }
                            const f32 length =
                                NuFsqrt(surface->normals[1].x * surface->normals[1].x + surface->normals[1].y * surface->normals[1].y +
                                        surface->normals[1].z * surface->normals[1].z);
                            const f32 inverse = length != 0.0f ? 1.0f / length : 0.0f;
                            surface = &ScaleTerrain[scaled_count];
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
                        {
                            NUVEC *cross_a = &vertices[0].xyz;
                            NUVEC *cross_b = &vertices[1].xyz;
                            NUVEC cross;
                            cross.x = cross_b->z * cross_a->y - cross_b->y * cross_a->z;
                            cross.y = cross_a->z * cross_b->x - cross_b->z * cross_a->x;
                            cross.z = cross_b->y * cross_a->x - cross_b->x * cross_a->y;
                            surface->normals[0] = cross;
                        }
                        const f32 length =
                            NuFsqrt(surface->normals[0].x * surface->normals[0].x + surface->normals[0].y * surface->normals[0].y +
                                    surface->normals[0].z * surface->normals[0].z);
                        const f32 inverse = length != 0.0f ? 1.0f / length : 0.0f;
                        surface = &ScaleTerrain[scaled_count];
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
            if (local_max_x >= group->bounds_min.x && local_max_y >= group->bounds_min.y && local_max_z >= group->bounds_min.z &&
                local_min_x <= group->bounds_max.x && local_min_y < group->bounds_max.y && local_min_z < group->bounds_max.z &&
                group->chunk_type != -1) {
                TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
                while (batch->marker >= 0) {
                    const i32 count = batch->shape_count;
                    TERRAIN_POLYGON_INFO *source = reinterpret_cast<TERRAIN_POLYGON_INFO *>(batch + 1);
                    if (local_max_x >= batch->min_x && batch->max_x > local_min_x && local_max_z >= batch->min_z &&
                        batch->max_z > local_min_z) {
                        for (i32 remaining = count; remaining > 0; --remaining, ++source) {
                            if (!(local_max_x >= source->min_x && source->max_x > local_min_x && local_max_z >= source->min_z &&
                                  source->max_z > local_min_z && local_max_y >= source->min_y && source->max_y > local_min_y)) {
                                continue;
                            }
                            platinrange = 1;
                            if (cursor >= limit || !TERRAIN_SHAPE_MATERIAL_OK(source, terrain_mask)) {
                                continue;
                            }
                            if (query->object_scale == 1.0f) {
                                (cursor++)->shape = source;
                                ++shape_count;
                            } else {
                                TERRAIN_POLYGON_INFO *surface = &ScaleTerrain[scaled_count];
                                memcpy(surface->material, source->material, 4);
                                surface->vertices[0].x = source->vertices[0].x;
                                surface->vertices[0].z = source->vertices[0].z;
                                surface->vertices[0].y =
                                    (source->vertices[0].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                                surface->vertices[1].x = source->vertices[1].x;
                                surface->vertices[1].z = source->vertices[1].z;
                                surface->vertices[1].y =
                                    (source->vertices[1].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                                surface->vertices[2].x = source->vertices[2].x;
                                surface->vertices[2].z = source->vertices[2].z;
                                surface->vertices[2].y =
                                    (source->vertices[2].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                                if (source->normals[1].y < 65535.0) {
                                    surface->vertices[3].x = source->vertices[3].x;
                                    surface->vertices[3].z = source->vertices[3].z;
                                    surface->vertices[3].y =
                                        (source->vertices[3].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                                    const f32 inverse = TerrainDivideNonzeroPC(
                                        1.0f, NuFsqrt(source->normals[1].x * source->normals[1].x +
                                                      source->normals[1].y * source->normals[1].y * query->object_scale_sq +
                                                      source->normals[1].z * source->normals[1].z));
                                    query = TerI;
                                    surface = &ScaleTerrain[scaled_count];
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
                                query = TerI;
                                surface = &ScaleTerrain[scaled_count];
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

    i32 HitTerrPoly(TERRAIN_POLYGON_INFO *surface, i32 group_index) {
        f32 primary_start = 0.0f;
        f32 secondary_start = 0.0f;
        f32 secondary_end = 0.0f;
        TerrainQuery_s *query = TerI;
        f32 radius = query->collision_radius;
        i32 result = 0;
        query->hit_type = result;
        query->hit_time = 999.9f;
        const f64 horizontal_length = NuFsqrt(query->movement.x * query->movement.x + query->movement.z * query->movement.z);
        query = TerI;
        query->horizontal_movement_length = horizontal_length;
        NUVEC *origin = &CurrentTerrain->groups[group_index].origin;
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
            if (primary_start > -radius) {
                test = 1;
            }
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
                if (secondary_start > -radius) {
                    test = 1;
                }
            }
        }
        if (test != 0 && HitPoly(primary_start, primary_end, secondary_start, secondary_end, surface)) {
            TerI->terrain_group_index = group_index;
            result = 1;
        }
        return result;
    }
}

extern "C" void RayImpact(NUVEC *movement) {
    TerrainMoveImpactData();
    TerrainQuery_s *query = TerI;
    switch (query->hit_type) {
        case TERRAIN_HIT_TYPE_FACE:
        case TERRAIN_HIT_TYPE_CYLINDER:
        case TERRAIN_HIT_TYPE_VERTEX:
        case TERRAIN_HIT_TYPE_SPHERE:
            query->hit_time -= query->separation_epsilon;
            if (query->hit_time < 0.0f) {
                query->hit_time = 0.0f;
            }
            movement->x = query->movement.x * query->hit_time;
            movement->y = query->movement.y * query->hit_time;
            movement->z = query->movement.z * query->hit_time;
            break;
        case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_FACE:
        case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_CYLINDER:
        case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_VERTEX:
        case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE:
            movement->x = 0.0f;
            movement->y = 0.0f;
            movement->z = 0.0f;
            break;
    }
}

extern "C" i32 NewRayCastEx(NUVEC *position, NUVEC *movement, f32 radius, i32 scan_flags) {
    return NewRayCast(position, movement, radius, scan_flags);
}

extern "C" {
    NUVEC ShadNorm = {0};
}

extern "C" i32 NewRayCastScaleYMask(NUVEC *position, NUVEC *movement, f32 radius, f32 scale_y, i32 scan_flags, u32 terrain_mask) {
    plathitid = -1;
    TerrPolyObj = -1;
    TerrPoly = NULL;
    TerrWallInfo = 0;
    castnum = -1;
    if (CurrentTerrain == NULL) {
        return 0;
    }
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    query->object_scale = scale_y;
    TerI = query;
    query->object_scale_sq = scale_y * scale_y;
    TerrOverRideScan = NULL;
    f32 inverse_scale;
    if (scale_y != 0.0f) {
        inverse_scale = 1.0f / scale_y;
    } else {
        inverse_scale = 0.0f;
    }
    query->inverse_object_scale = inverse_scale;
    query->inverse_object_scale_sq = inverse_scale * inverse_scale;
    query->collision_radius = radius;
    if (radius == 0.0f) {
        query->inverse_collision_radius = 0.0f;
    } else {
        query->inverse_collision_radius = 1.0f / radius;
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
    query->separation_epsilon = 0.01f;
    query->compare_epsilon = 0.00001f;
    ScanTerrain(1, terrain_mask, scan_flags != 0 ? 0x40 : 0);
    query = TerI;
    query->position.y *= query->inverse_object_scale;
    query->movement.y = query->movement.y * query->inverse_object_scale;
    DerotateMovementVector();
    HitTerrain();
    query = TerI;
    if (query->hit_type != 0) {
        RayImpact(movement);
        TerrainImpactNorm();
        ShadNorm = TerI->movement_normal;
        movement->y *= query->object_scale;
    }
    NuScratchRelease();
    return TerI->hit_type;
}

extern "C" i32 TerrainPlatId(void) {
    return plathitid;
}

extern "C" f32 NewRayCastGetTOFI(void) {
    return TerI->hit_time;
}

extern "C" i32 NewRayCastGetImpactTerrainType(void) {
    TERRAIN_POLYGON_INFO *surface = TerI->surface;
    return surface ? surface->material[0] : 0;
}

extern "C" void NewTerrHitInfo(u8 *info) {
    info[0] = TerrPolyInfo.material[0];
    info[1] = TerrPolyInfo.material[1];
    info[2] = TerrPolyInfo.flags;
    info[3] = TerrPolyInfo.normal_flags;
}

extern "C" void NewRayCastGetImpactNormal(NUVEC *normal) {
    TerrainQuery_s *query = TerI;
    if (query != NULL) {
        *normal = query->movement_normal;
    }
}

extern "C" f32 NewRayCastGetEmbedDist(void) {
    return TerI->unclamped_hit_time;
}

extern "C" i32 NewRayCastHitWallSpline(void) {
    return TerI->shape_adjusted;
}

extern "C" i32 NewRayCast(NUVEC *position, NUVEC *movement, f32 radius, i32 scan_flags) {
    plathitid = -1;
    TerrPolyObj = -1;
    TerrPoly = NULL;
    TerrWallInfo = 0;
    castnum = -1;
    if (CurrentTerrain == NULL) {
        return 0;
    }
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    TerrOverRideScan = NULL;
    query->inverse_object_scale = 1.0f;
    TerI = query;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    if (radius == 0.0f) {
        query->inverse_collision_radius = 0.0f;
    } else {
        query->inverse_collision_radius = 1.0f / radius;
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
    query->separation_epsilon = 0.01f;
    query->compare_epsilon = 0.00001f;
    ScanTerrain(1, 0, scan_flags != 0 ? 0x40 : 0);
    query = TerI;
    DerotateMovementVector();
    HitTerrain();
    query = TerI;
    if (query->hit_type != 0) {
        RayImpact(movement);
        TerrainImpactNorm();
        query = TerI;
        ShadNorm = query->movement_normal;
    }
    NuScratchRelease();
    return TerI->hit_type;
}

extern "C" i32 NewRayCastMask(NUVEC *position, NUVEC *movement, f32 radius, i32 terrain_mask, i32 scan_flags) {
    plathitid = -1;
    TerrPolyObj = -1;
    TerrPoly = NULL;
    TerrWallInfo = 0;
    castnum = -1;
    if (CurrentTerrain == NULL) {
        return 0;
    }
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    TerrOverRideScan = NULL;
    query->inverse_object_scale = 1.0f;
    TerI = query;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    if (radius == 0.0f) {
        query->inverse_collision_radius = 0.0f;
    } else {
        query->inverse_collision_radius = 1.0f / radius;
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
    query->separation_epsilon = 0.01f;
    query->compare_epsilon = 0.00001f;
    ScanTerrain(1, terrain_mask, scan_flags != 0 ? 0x40 : 0);
    query = TerI;
    DerotateMovementVector();
    HitTerrain();
    query = TerI;
    if (query->hit_type != 0) {
        RayImpact(movement);
        TerrainImpactNorm();
        query = TerI;
        ShadNorm = query->movement_normal;
    }
    NuScratchRelease();
    return TerI->hit_type;
}

extern "C" i32 NewRayCastSet(NUVEC *position, NUVEC *movement, f32 radius, f32 separation_epsilon, f32 compare_epsilon, i32 scan_type,
                             i32 scan_flags) {
    plathitid = -1;
    TerrPolyObj = -1;
    TerrPoly = NULL;
    TerrWallInfo = 0;
    castnum = -1;
    if (CurrentTerrain == NULL) {
        return 0;
    }
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    TerrOverRideScan = NULL;
    query->inverse_object_scale = 1.0f;
    TerI = query;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    if (radius == 0.0f) {
        query->inverse_collision_radius = 0.0f;
    } else {
        query->inverse_collision_radius = 1.0f / radius;
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
    query->compare_epsilon = compare_epsilon;
    ScanTerrain(scan_type, 0, scan_flags != 0 ? 0x40 : 0);
    query = TerI;
    DerotateMovementVector();
    HitTerrain();
    query = TerI;
    if (query->hit_type != 0) {
        RayImpact(movement);
        TerrainImpactNorm();
        query = TerI;
        ShadNorm = query->movement_normal;
    }
    NuScratchRelease();
    return TerI->hit_type;
}

extern "C" i32 NewRayCastSetMask(NUVEC *position, NUVEC *movement, f32 radius, f32 separation_epsilon, f32 compare_epsilon, i32 scan_type,
                                 i32 terrain_mask, i32 scan_flags) {
    plathitid = -1;
    TerrPolyObj = -1;
    TerrPoly = NULL;
    TerrWallInfo = 0;
    castnum = -1;
    if (CurrentTerrain == NULL) {
        return 0;
    }
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    query->inverse_object_scale = 1.0f;
    TerI = query;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    if (radius == 0.0f) {
        query->inverse_collision_radius = 0.0f;
    } else {
        query->inverse_collision_radius = 1.0f / radius;
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
    query->compare_epsilon = compare_epsilon;
    ScanTerrain(scan_type, terrain_mask, scan_flags != 0 ? 0x40 : 0);
    query = TerI;
    DerotateMovementVector();
    HitTerrain();
    query = TerI;
    if (query->hit_type != 0) {
        RayImpact(movement);
        TerrainImpactNorm();
        query = TerI;
        ShadNorm = query->movement_normal;
    }
    NuScratchRelease();
    return TerI->hit_type;
}

extern "C" {
    TERRAIN_POLYGON_INFO *TerrPoly = 0;
    u16 TerrainSurfaceClass = 0;
}
static u8 *TempStackPtr = 0;
extern "C" {
    void *TerrOverRideScan = 0;
}

struct TERRAIN_HANDLE_WALL_PAIR { // size 0x8
    TERRAIN_WALL_POINT *first;    // 0x0
    TERRAIN_WALL_POINT *second;   // 0x4
};

extern "C" void NewScanInit(void) {
    TempStackPtr = static_cast<u8 *>(TempScanStack);
    TerrPlatDis = -1;
}

i16 *NewScanHandelSubset(i16 *subset, NUVEC *position, NUVEC *movement, f32 radius, i32 terrain_mask) {
    TERRAIN_SCAN_WORD *group_header;
    TERRAIN_SCAN_WORD *cursor;
    TERRAIN_SCAN_WORD *limit;
    TERRAIN_SCAN_WORD *result;
    i32 shape_count;
    f32 max_x;
    f32 min_x;
    f32 max_z;
    f32 min_z;
    f32 max_y;
    f32 min_y;
    f32 scan_radius;
    TERRAIN_HANDLE_WALL_PAIR *walls;
    TERRAIN_HANDLE_WALL_PAIR *source;

    if (subset == NULL) {
        return NULL;
    }
    group_header = reinterpret_cast<TERRAIN_SCAN_WORD *>(TempStackPtr);
    cursor = reinterpret_cast<TERRAIN_SCAN_WORD *>(TempStackPtr) + 1;
    result = group_header;
    limit = cursor + 508;
    if (reinterpret_cast<u8 *>(cursor) > reinterpret_cast<u8 *>(ScaleTerrainT1) + 0x63c18) {
        return NULL;
    }
    if (reinterpret_cast<u8 *>(limit) > reinterpret_cast<u8 *>(ScaleTerrainT1) + 0x63da8) {
        limit = reinterpret_cast<TERRAIN_SCAN_WORD *>(reinterpret_cast<u8 *>(ScaleTerrainT1) + 0x63da8);
    }
    if (movement->x > 0.0f) {
        min_x = position->x - 0.02f - radius;
        max_x = position->x + movement->x + 0.02f + radius;
    } else {
        min_x = position->x + movement->x - 0.02f - radius;
        max_x = position->x + 0.02f + radius;
    }
    if (movement->y > 0.0f) {
        min_y = position->y - 0.02f - radius;
        max_y = position->y + movement->y + 0.02f + radius;
    } else {
        min_y = position->y + movement->y - 0.02f - radius;
        max_y = position->y + 0.02f + radius;
    }
    if (movement->z > 0.0f) {
        min_z = position->z - 0.02f - radius;
        max_z = position->z + movement->z + 0.02f + radius;
    } else {
        min_z = position->z + movement->z - 0.02f - radius;
        max_z = position->z + 0.02f + radius;
    }
    shape_count = 0;
    {
        const f32 extent_sq = (min_x - max_x) * (min_x - max_x) + (min_y - max_y) * (min_y - max_y) + (min_z - max_z) * (min_z - max_z);
        // Uses the squared extent plus the margin directly as the sphere radius.
        scan_radius = extent_sq + 0.05f;
    }
    while (subset[0] > 0) {
        const i32 count = subset[0];
        const i32 group_index = subset[1];
        TERRAIN_SCAN_WORD *shapes = reinterpret_cast<TERRAIN_SCAN_WORD *>(subset) + 1;
        TERRAIN_GROUP *group = &CurrentTerrain->groups[group_index];
        f32 local_min_x;
        f32 local_max_x;
        f32 local_min_z;
        f32 local_max_z;
        f32 local_min_y;
        f32 local_max_y;
        TERRAIN_PLATFORM *platform;
        if (group->chunk_type == 1 && (platform = &TERRAIN_GROUP_PLATFORM(group))->scene_object != NULL) {
            if (TERRAIN_GROUP_MATRIX(group)->m30 > TERRAIN_GROUP_PLATFORM(group).previous_matrix.m30) {
                local_min_x = min_x - group->origin.x;
                local_max_x = (f32)((TERRAIN_GROUP_MATRIX(group)->m30 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m30) * 1.5 + max_x -
                                    group->origin.x);
            } else {
                local_max_x = max_x - group->origin.x;
                local_min_x = (f32)((TERRAIN_GROUP_MATRIX(group)->m30 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m30) * 1.5 + min_x -
                                    group->origin.x);
            }
            if (TERRAIN_GROUP_MATRIX(group)->m31 > TERRAIN_GROUP_PLATFORM(group).previous_matrix.m31) {
                local_min_y = min_y - group->origin.y;
                local_max_y = (f32)((TERRAIN_GROUP_MATRIX(group)->m31 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m31) * 1.5 + max_y -
                                    group->origin.y);
            } else {
                local_max_y = max_y - group->origin.y;
                local_min_y = (f32)((TERRAIN_GROUP_MATRIX(group)->m31 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m31) * 1.5 + min_y -
                                    group->origin.y);
            }
            if (TERRAIN_GROUP_MATRIX(group)->m32 > TERRAIN_GROUP_PLATFORM(group).previous_matrix.m32) {
                local_min_z = min_z - group->origin.z;
                local_max_z = (f32)((TERRAIN_GROUP_MATRIX(group)->m32 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m32) * 1.5 + max_z -
                                    group->origin.z);
            } else {
                local_max_z = max_z - group->origin.z;
                local_min_z = (f32)((TERRAIN_GROUP_MATRIX(group)->m32 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m32) * 1.5 + min_z -
                                    group->origin.z);
            }
            if ((platform->flags & TERRAIN_PLATFORM_FLAG_ROTATING) == 0) {
                if ((local_max_x >= group->bounds_min.x && local_max_y >= group->bounds_min.y && local_max_z >= group->bounds_min.z &&
                     group->bounds_max.x >= local_min_x && group->bounds_max.y > local_min_y && group->bounds_max.z > local_min_z)) {
                    for (i32 remaining = count; remaining > 0; --remaining, ++shapes) {
                        TERRAIN_POLYGON_INFO *shape = shapes->shape;
                        if ((local_max_x >= shape->min_x && shape->max_x > local_min_x && local_max_z >= shape->min_z &&
                             shape->max_z > local_min_z && local_max_y >= shape->min_y && shape->max_y > local_min_y) &&
                            cursor < limit && TERRAIN_SHAPE_MATERIAL_OK(shape, terrain_mask)) {
                            (cursor++)->shape = shape;
                            ++shape_count;
                        }
                    }
                    if (shape_count != 0) {
                        group_header->header.shape_count = (i16)shape_count;
                        group_header->header.terrain_group = (i16)(group_index);
                        group_header = cursor;
                        cursor++;
                        shape_count = 0;
                    }
                } else {
                    shapes += count;
                }
            } else {
                const f64 dy = ((min_y + max_y) * 0.5 - group->origin.y) * TerI->inverse_object_scale;
                const f64 dx = (min_x + max_x) * 0.5 - group->origin.x;
                const f64 dz = (min_z + max_z) * 0.5 - group->origin.z;
                const f32 distance_sq = dx * dx + dy * dy + dz * dz;
                if (distance_sq < (group->radius + scan_radius) * (group->radius + scan_radius)) {
                    for (i32 remaining = count; remaining > 0; --remaining, ++shapes) {
                        TERRAIN_POLYGON_INFO *shape = shapes->shape;
                        if (cursor < limit && TERRAIN_SHAPE_MATERIAL_OK(shape, terrain_mask) &&
                            ((shape->vertices[0].x > local_min_x || shape->vertices[1].x > local_min_x ||
                              shape->vertices[2].x > local_min_x || shape->vertices[3].x > local_min_x) &&
                             (shape->vertices[0].x < local_max_x || shape->vertices[1].x < local_max_x ||
                              shape->vertices[2].x < local_max_x || shape->vertices[3].x < local_max_x) &&
                             (shape->vertices[0].z > local_min_z || shape->vertices[1].z > local_min_z ||
                              shape->vertices[2].z > local_min_z || shape->vertices[3].z > local_min_z) &&
                             (shape->vertices[0].z < local_max_z || shape->vertices[1].z < local_max_z ||
                              shape->vertices[2].z < local_max_z || shape->vertices[3].z < local_max_z))) {
                            (cursor++)->shape = shape;
                            ++shape_count;
                        }
                    }
                    if (shape_count != 0) {
                        group_header->header.shape_count = (i16)shape_count;
                        group_header->header.terrain_group = (i16)(group_index);
                        group_header = cursor;
                        cursor++;
                        shape_count = 0;
                    }
                } else {
                    shapes += count;
                }
            }
        } else if ((max_x >= group->bounds_min.x && max_y >= group->bounds_min.y && max_z >= group->bounds_min.z &&
                    group->bounds_max.x >= min_x && group->bounds_max.y > min_y && group->bounds_max.z > min_z)) {
            local_max_x = max_x - group->origin.x;
            local_min_x = min_x - group->origin.x;
            local_max_y = max_y - group->origin.y;
            local_min_y = min_y - group->origin.y;
            local_max_z = max_z - group->origin.z;
            local_min_z = min_z - group->origin.z;
            for (i32 remaining = count; remaining > 0; --remaining, ++shapes) {
                TERRAIN_POLYGON_INFO *shape = shapes->shape;
                if ((local_max_x >= shape->min_x && shape->max_x > local_min_x && local_max_z >= shape->min_z &&
                     shape->max_z > local_min_z && local_max_y >= shape->min_y && shape->max_y > local_min_y) &&
                    cursor < limit && TERRAIN_SHAPE_MATERIAL_OK(shape, terrain_mask)) {
                    (cursor++)->shape = shape;
                    ++shape_count;
                }
            }
            if (shape_count != 0) {
                group_header->header.shape_count = (i16)shape_count;
                group_header->header.terrain_group = (i16)(group_index);
                group_header = cursor;
                cursor++;
                shape_count = 0;
            }
        } else {
            // Skips one extra word after a rejected static group.
            shapes += count + 1;
        }
        subset = reinterpret_cast<i16 *>(shapes);
    }
    group_header->header.shape_count = 0;
    group_header->header.terrain_group = 0;
    min_x = min_x - 0.02f;
    min_z = min_z - 0.02f;
    max_x = max_x + 0.02f;
    max_z = max_z + 0.02f;
    walls = reinterpret_cast<TERRAIN_HANDLE_WALL_PAIR *>(group_header + 1);
    source = reinterpret_cast<TERRAIN_HANDLE_WALL_PAIR *>(subset + 2);
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
    group_header = reinterpret_cast<TERRAIN_SCAN_WORD *>(walls);
    group_header->header.shape_count = 0;
    group_header->header.terrain_group = 0;
    TempStackPtr = reinterpret_cast<u8 *>(group_header + 1);
    return reinterpret_cast<i16 *>(result);
}

extern "C" void fun_0067e980(void) {
}

struct TERRAIN_HANDLE_END {       // size 0x8
    TERRAIN_SCAN_WORD terminator; // 0x0

    union { // size 0x4
        TERRAIN_WALL_POINT *wall_cursor;

        struct {               // size 0x4
            i16 shape_count;   // 0x4
            i16 terrain_group; // 0x6
        } wall_end;
    }; // 0x4
};

i16 *NewScanHandelFull(NUVEC *position, NUVEC *movement, f32 radius, i32 scan_type, i32 terrain_mask) {
    TERRAIN_SCAN_WORD *group_header;
    TERRAIN_SCAN_WORD *cursor;
    TERRAIN_SCAN_WORD *limit;
    TERRAIN_SCAN_WORD *result;
    i32 shape_count;
    i32 scaled_count;
    TerrainScanBounds bounds;
    TerrainScanBounds *const scan = &bounds;
    f32 extent_sq;
    f32 scan_radius;
    NUVEC4 *vertices;
    TERRAIN_HANDLE_END *end_header;

    group_header = reinterpret_cast<TERRAIN_SCAN_WORD *>(TempStackPtr);
    cursor = group_header + 1;
    scaled_count = 0;
    ScaleTerrain = ScaleTerrainT1;
    result = group_header;
    limit = cursor + 508;
    if (reinterpret_cast<u8 *>(cursor) > reinterpret_cast<u8 *>(ScaleTerrainT1) + 0x63c18) {
        return NULL;
    }
    if (reinterpret_cast<u8 *>(limit) > reinterpret_cast<u8 *>(ScaleTerrainT1) + 0x63da8) {
        limit = reinterpret_cast<TERRAIN_SCAN_WORD *>(reinterpret_cast<u8 *>(ScaleTerrainT1) + 0x63da8);
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
    extent_sq = (scan->min_x - scan->max_x) * (scan->min_x - scan->max_x) + (scan->min_y - scan->max_y) * (scan->min_y - scan->max_y) +
                (scan->min_z - scan->max_z) * (scan->min_z - scan->max_z);
    scan_radius = NuFsqrt((f32)(extent_sq + 0.05f));
    for (i32 cell_index = 0; cell_index < CurrentTerrain->used_cell_count; ++cell_index) {
        TERRAIN_CELL *cell = &CurrentTerrain->cells[cell_index];
        if (!(scan->max_x >= cell->min_x && scan->max_z >= cell->min_z && cell->max_x >= scan->min_x && cell->max_z >= scan->min_z)) {
            continue;
        }
        i16 *indices = CurrentTerrain->group_indices + cell->first_group;
        for (i32 i = 0; i < CurrentTerrain->cells[cell_index].group_count; ++i, ++indices) {
            TERRAIN_GROUP *group = &CurrentTerrain->groups[*indices];
            if (scan->max_x >= group->bounds_min.x && scan->max_y >= group->bounds_min.y && scan->max_z >= group->bounds_min.z &&
                scan->min_x <= group->bounds_max.x && scan->min_y < group->bounds_max.y && scan->min_z < group->bounds_max.z &&
                group->chunk_type != -1) {
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
                    TERRAIN_POLYGON_INFO *source = reinterpret_cast<TERRAIN_POLYGON_INFO *>(batch + 1);
                    if (local_max_x >= batch->min_x && batch->max_x > local_min_x && local_max_z >= batch->min_z &&
                        batch->max_z > local_min_z) {
                        while (count-- > 0) {
                            if ((local_max_x >= source->min_x && source->max_x > local_min_x && local_max_z >= source->min_z &&
                                 source->max_z > local_min_z && local_max_y >= source->min_y && source->max_y > local_min_y) &&
                                cursor < limit && TERRAIN_SHAPE_MATERIAL_OK(source, terrain_mask)) {
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
                if (shape_count != 0) {
                    group_header->header.shape_count = (i16)shape_count;
                    group_header->header.terrain_group = *indices;
                    group_header = cursor;
                    cursor++;
                    shape_count = 0;
                }
            }
        }
    }
    TerI->scan_list = group_header;
    scan->min_x = (f32)(scan->min_x - 0.05f);
    scan->max_x = (f32)(scan->max_x + 0.05f);
    scan->min_y = (f32)(scan->min_y - 0.05f);
    scan->max_y = (f32)(scan->max_y + 0.05f);
    scan->min_z = (f32)(scan->min_z - 0.05f);
    scan->max_z = (f32)(scan->max_z + 0.05f);
    if (scan_type != 0) {
        i16 *indices = CurrentTerrain->group_indices + CurrentTerrain->platform_cell.first_group;
        for (i32 i = 0; i < CurrentTerrain->platform_cell.group_count; ++i, ++indices) {
            TERRAIN_GROUP *group = &CurrentTerrain->groups[*indices];
            TERRAIN_PLATFORM *platform = &TERRAIN_GROUP_PLATFORM(group);
            u32 *scene_state = platform->instance_flags;
            if (scene_state == NULL ||
                ((platform->flags & TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED) != 0 ? (*scene_state & 2) : (*scene_state & 1))) {
                f32 local_min_x;
                f32 local_max_x;
                f32 local_min_z;
                f32 local_max_z;
                f32 local_min_y;
                f32 local_max_y;
                if (platform->scene_object != NULL) {
                    if (TERRAIN_GROUP_MATRIX(group)->m30 > TERRAIN_GROUP_PLATFORM(group).previous_matrix.m30) {
                        local_min_x = scan->min_x - group->origin.x;
                        local_max_x = (f32)((TERRAIN_GROUP_MATRIX(group)->m30 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m30) * 1.5 +
                                            scan->max_x - group->origin.x);
                    } else {
                        local_max_x = scan->max_x - group->origin.x;
                        local_min_x = (f32)((TERRAIN_GROUP_MATRIX(group)->m30 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m30) * 1.5 +
                                            scan->min_x - group->origin.x);
                    }
                    if (TERRAIN_GROUP_MATRIX(group)->m31 > TERRAIN_GROUP_PLATFORM(group).previous_matrix.m31) {
                        local_min_y = scan->min_y - group->origin.y;
                        local_max_y = (f32)((TERRAIN_GROUP_MATRIX(group)->m31 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m31) * 1.5 +
                                            scan->max_y - group->origin.y);
                    } else {
                        local_max_y = scan->max_y - group->origin.y;
                        local_min_y = (f32)((TERRAIN_GROUP_MATRIX(group)->m31 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m31) * 1.5 +
                                            scan->min_y - group->origin.y);
                    }
                    if (TERRAIN_GROUP_MATRIX(group)->m32 > TERRAIN_GROUP_PLATFORM(group).previous_matrix.m32) {
                        local_min_z = scan->min_z - group->origin.z;
                        local_max_z = (f32)((TERRAIN_GROUP_MATRIX(group)->m32 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m32) * 1.5 +
                                            scan->max_z - group->origin.z);
                    } else {
                        local_max_z = scan->max_z - group->origin.z;
                        local_min_z = (f32)((TERRAIN_GROUP_MATRIX(group)->m32 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m32) * 1.5 +
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
                    if (local_max_x >= group->bounds_min.x && local_max_y >= group->bounds_min.y && local_max_z >= group->bounds_min.z &&
                        local_min_x <= group->bounds_max.x && local_min_y < group->bounds_max.y && local_min_z < group->bounds_max.z &&
                        group->chunk_type != -1) {
                        TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
                        while (batch->marker >= 0) {
                            i32 count = batch->shape_count;
                            TERRAIN_POLYGON_INFO *source = reinterpret_cast<TERRAIN_POLYGON_INFO *>(batch + 1);
                            if (local_max_x >= batch->min_x && batch->max_x > local_min_x && local_max_z >= batch->min_z &&
                                batch->max_z > local_min_z) {
                                while (count-- > 0) {
                                    if ((local_max_x >= source->min_x && source->max_x > local_min_x && local_max_z >= source->min_z &&
                                         source->max_z > local_min_z && local_max_y >= source->min_y && source->max_y > local_min_y)) {
                                        platinrange = 1;
                                        if (cursor < limit && TERRAIN_SHAPE_MATERIAL_OK(source, terrain_mask)) {
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
                    const f64 dy = ((scan->max_y + scan->min_y) * 0.5 - group->origin.y) * TerI->inverse_object_scale;
                    const f64 dx = (scan->max_x + scan->min_x) * 0.5 - group->origin.x;
                    const f64 dz = (scan->max_z + scan->min_z) * 0.5 - group->origin.z;
                    // Reuses the local minimum Y slot for the distance; Y is not read again here.
                    local_min_y = dx * dx + dy * dy + dz * dz;
                    if (local_min_y < (group->radius + scan_radius) * (group->radius + scan_radius)) {
                        TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
                        while (batch->marker >= 0) {
                            const i32 count = batch->shape_count;
                            TERRAIN_POLYGON_INFO *source = reinterpret_cast<TERRAIN_POLYGON_INFO *>(batch + 1);
                            for (i32 remaining = count; remaining > 0; --remaining, ++source) {
                                platinrange = 1;
                                if (cursor >= limit || !TERRAIN_SHAPE_MATERIAL_OK(source, terrain_mask)) {
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
                                NuVec4MtxTransformVU0x3(vertices, vertices, TERRAIN_GROUP_MATRIX(group));
                                if (source->normals[1].y < 65535.0) {
                                    vertices[3].x = source->vertices[3].x;
                                    vertices[3].y = source->vertices[3].y;
                                    vertices[3].z = source->vertices[3].z;
                                    vertices[3].w = 0.0f;
                                    NuVec4MtxTransformVU0(&vertices[3], &vertices[3], TERRAIN_GROUP_MATRIX(group));
                                } else {
                                    vertices[3] = vertices[2];
                                }
                                if (!((vertices[0].x > local_min_x || vertices[1].x > local_min_x || vertices[2].x > local_min_x ||
                                       vertices[3].x > local_min_x) &&
                                      (vertices[0].x < local_max_x || vertices[1].x < local_max_x || vertices[2].x < local_max_x ||
                                       vertices[3].x < local_max_x) &&
                                      (vertices[0].z > local_min_z || vertices[1].z > local_min_z || vertices[2].z > local_min_z ||
                                       vertices[3].z > local_min_z) &&
                                      (vertices[0].z < local_max_z || vertices[1].z < local_max_z || vertices[2].z < local_max_z ||
                                       vertices[3].z < local_max_z))) {
                                    continue;
                                }
                                TERRAIN_POLYGON_INFO *surface = &ScaleTerrain[scaled_count];
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
                                    {
                                        NUVEC *cross_a = &vertices[0].xyz;
                                        NUVEC *cross_b = &vertices[1].xyz;
                                        NUVEC cross;
                                        cross.x = cross_b->z * cross_a->y - cross_b->y * cross_a->z;
                                        cross.y = cross_a->z * cross_b->x - cross_b->z * cross_a->x;
                                        cross.z = cross_b->y * cross_a->x - cross_b->x * cross_a->y;
                                        surface->normals[1] = cross;
                                    }
                                    const f32 length = NuFsqrt(surface->normals[1].x * surface->normals[1].x +
                                                               surface->normals[1].y * surface->normals[1].y +
                                                               surface->normals[1].z * surface->normals[1].z);
                                    const f32 inverse = length != 0.0f ? 1.0f / length : 0.0f;
                                    surface = &ScaleTerrain[scaled_count];
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
                                {
                                    NUVEC *cross_a = &vertices[0].xyz;
                                    NUVEC *cross_b = &vertices[1].xyz;
                                    NUVEC cross;
                                    cross.x = cross_b->z * cross_a->y - cross_b->y * cross_a->z;
                                    cross.y = cross_a->z * cross_b->x - cross_b->z * cross_a->x;
                                    cross.z = cross_b->y * cross_a->x - cross_b->x * cross_a->y;
                                    surface->normals[0] = cross;
                                }
                                const f32 length =
                                    NuFsqrt(surface->normals[0].x * surface->normals[0].x + surface->normals[0].y * surface->normals[0].y +
                                            surface->normals[0].z * surface->normals[0].z);
                                const f32 inverse = length != 0.0f ? 1.0f / length : 0.0f;
                                surface = &ScaleTerrain[scaled_count];
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
            if (shape_count != 0) {
                group_header->header.shape_count = (i16)shape_count;
                group_header->header.terrain_group = *indices;
                group_header = cursor;
                cursor++;
                shape_count = 0;
            }
        }
    }
    end_header = reinterpret_cast<TERRAIN_HANDLE_END *>(group_header);
    end_header->terminator.header.shape_count = 0;
    end_header->terminator.header.terrain_group = 0;
    scan->min_x = scan->min_x - 0.02f;
    scan->min_z = scan->min_z - 0.02f;
    scan->max_x = scan->max_x + 0.02f;
    scan->max_z = scan->max_z + 0.02f;
    for (TERRAIN_SPATIAL_NODE *node = CurrentTerrain->spatial_nodes; node != NULL;
         node = reinterpret_cast<TERRAIN_SPATIAL_CHUNK *>(reinterpret_cast<u8 *>(node) - offsetof(TERRAIN_SPATIAL_CHUNK, node))->previous) {
        const u8 mask = static_cast<u8>(node->unknown_02 >> 8);
        if (mask != 0 && (mask & terrain_mask) == 0) {
            continue;
        }
        const u8 material = static_cast<u8>(node->unknown_02);
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
                    ((a->z >= scan->min_z && b->z <= scan->max_z) || (b->z >= scan->min_z && a->z <= scan->max_z)) && WallSplCount < 64) {
                    // Header+4 is read and advanced unchecked.
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
    TempStackPtr = reinterpret_cast<u8 *>(end_header + 1);
    NuScratchRelease();
    return reinterpret_cast<i16 *>(result);
}

extern "C" i16 *NewScanHandel(NUVEC *position, NUVEC *movement, f32 radius, i32 scan_type, i16 *subset) {
    if (CurrentTerrain == NULL) {
        return NULL;
    }
    TerrPlatDis = -1;
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    query->inverse_object_scale = 1.0f;
    TerI = query;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    f32 inverse_radius;
    if (radius == 0.0f) {
        inverse_radius = 0.0f;
    } else {
        inverse_radius = 1.0f / radius;
    }
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
    if (subset == NULL) {
        handle = NewScanHandelFull(position, movement, radius, scan_type, 0xff);
    } else {
        handle = NewScanHandelSubset(subset, position, movement, radius, 0xff);
    }
    NuScratchRelease();
    return handle;
}

extern "C" void TerrainSetPlatformDistance(i32 distance) {
    TerrPlatDis = distance;
}

extern "C" i32 NewRayCastSetHandel(NUVEC *position, NUVEC *movement, f32 radius, f32 separation_epsilon, f32 compare_epsilon, i16 *handle,
                                   i32 scan_type) {
    plathitid = -1;
    TerrPolyObj = -1;
    TerrPoly = NULL;
    TerrWallInfo = 0;
    castnum = -1;
    if (CurrentTerrain == NULL) {
        return 0;
    }
    if (handle == NULL) {
        return 0;
    }
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    TerI = query;
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    TerrOverRideScan = NULL;
    query->inverse_object_scale = 1.0f;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    const f32 inverse_radius = radius == 0.0f ? 0.0f : TerrainDivideNonzeroPC(1.0f, radius);
    query = TerI;
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
    query = TerI;
    DerotateMovementVector();
    HitTerrain();
    query = TerI;
    if (query->hit_type != 0) {
        RayImpact(movement);
        TerrainImpactNorm();
        query = TerI;
        ShadNorm = query->movement_normal;
        TerrPoly = query->surface;
    }
    NuScratchRelease();
    return TerI->hit_type;
}

extern "C" i32 NewRayCastPlatForm(NUVEC *position, NUVEC *movement, f32 radius, f32 separation_epsilon, i32 platform_index,
                                  i32 terrain_mask) {
    plathitid = -1;
    TerrPolyObj = -1;
    TerrPoly = NULL;
    TerrWallInfo = 0;
    castnum = -1;
    if (CurrentTerrain == NULL) {
        return 0;
    }
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
    ScanTerrainPlatform(CurrentTerrain->platforms[platform_index].terrain_group_index, terrain_mask);
    query = TerI;
    DerotateMovementVector();
    HitTerrain();
    query = TerI;
    if (query->hit_type != 0) {
        RayImpact(movement);
        TerrainImpactNorm();
        query = TerI;
        ShadNorm = query->movement_normal;
    }
    NuScratchRelease();
    return TerI->hit_type;
}

extern "C" void TerrShowCamTerr(void) {
    plathitid = -1;
    TerrPolyObj = -1;
    TerrPoly = NULL;
    TerrWallInfo = 0;
    PlatCrush = 0;
    terrhitflags = 0;
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    query->inverse_object_scale = 1.0f;
    query->inverse_object_scale_sq = 1.0f;
    TerI = query;
    query->collision_radius = 0.1f;
    query->inverse_collision_radius = 10.0f;
    query->collision_radius_sq = ForcePushApproachInnerSquare;
    query->start_position.x = query->position.x = global_camera.mtx.m30 - 2.0;
    query->start_position.y = query->position.y = global_camera.mtx.m31 - 2.0;
    query->start_position.z = query->position.z = global_camera.mtx.m32 - 2.0;
    query->start_movement.x = query->movement.x = 4.0f;
    query->start_movement.y = query->movement.y = 4.0f;
    query->start_movement.z = query->movement.z = 4.0f;
    query->object_index = 0;
    query->hit_flags = NULL;
    query->scan_result = 0;
    query->radius = 0.0f;
    query->separation_epsilon = 0.01f;
    query->compare_epsilon = 0.00001f;
    ScanTerrain(1, 1, 0);
    NuScratchRelease();
}

extern "C" void DrawHitTerrain(void) {
    if (CurrentTerrain == NULL) {
        return;
    }
    TerI = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    TERRAIN_SCAN_WORD *word = TerI->scan_words;
    for (;;) {
        while (word->header.shape_count > 0) {
            TERRAIN_SCAN_WORD *shape = word + 1;
            for (i32 count = word->header.shape_count; count > 0; --count, ++shape) {
                i16 group = word->header.terrain_group;
                TERRAIN_POLYGON_INFO *surface = shape->shape;
                if (CurrentTerrain->groups[group].chunk_type == 0) {
                    TerrDraw(surface, group);
                } else {
                    TerrDrawPlat(surface, group);
                }
            }
            word = shape;
        }
        if (word->header.shape_count >= 0) {
            break;
        }
        while (word->header.shape_count < 0) {
            word += abs(word->header.shape_count) + 1;
        }
    }
    NuScratchRelease();
}

extern "C" f32 NewShadowHandelEx(NUVEC *position, f32 unused, f32 height_above, f32 height_below, i32 terrain_mask, i16 *handle) {
    if (CurrentTerrain == NULL) {
        return LevelConfigFarDistance;
    }
    if (handle == NULL) {
        return 0.0f;
    }
    TerrainShadowHeight = LevelConfigFarDistance;
    EShadY = LevelConfigFarDistance;
    GetSurfaceInfoWaterNormal.y = 1.0f;
    TerrPolyObj = -1;
    GetSurfaceInfoRoofHeight = LevelConfigFarDistance;
    castnum = -1;
    TerrainShadowFloorIndex = -1;
    ShadNorm.y = 1.0f;
    TerrainCastAuxPolygon = NULL;
    TerrainWaterCastIndex = 0;
    EShadPoly = NULL;
    ShadRoofPoly = NULL;
    TerrainSurfaceClass = 0;
    ShadPoly = NULL;
    TerI = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    TerrainQuery_s *query = TerI;
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
    ScaleTerrain = ScaleTerrainT1;
    NUVEC scan_position = *position;
    ScanTerrainHandel(terrain_mask, handle);
    (void)NewCast(&scan_position, height_above, height_below);
    NuScratchRelease();
    NuTerrPlatsOff = 0;
    return scan_position.y;
}

extern "C" i32 TerrainTrackBack(NUVEC *position, NUVEC *direction, f32 radius, f32 backoff, NUVEC *result) {
    if (CurrentTerrain == NULL) {
        return 0;
    }
    if (TerrPolyObj == -1) {
        return 0;
    }
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    query->inverse_object_scale = 1.0f;
    TerI = query;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    f32 inverse_radius;
    if (radius != 0.0f) {
        inverse_radius = 1.0f / radius;
    } else {
        inverse_radius = 0.0f;
    }
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
    HitTerrPoly(TerrPoly, TerrPolyObj);
    query = TerI;
    switch (query->hit_type) {
        case TERRAIN_HIT_TYPE_FACE:
        case TERRAIN_HIT_TYPE_CYLINDER:
        case TERRAIN_HIT_TYPE_VERTEX:
        case TERRAIN_HIT_TYPE_SPHERE:
            query->hit_time -= backoff;
            if (query->hit_time < 0.0f) {
                query->hit_time = 0.0f;
            }
            result->x = query->movement.x * query->hit_time + query->position.x;
            result->y = query->movement.y * query->hit_time + query->position.y;
            result->z = query->movement.z * query->hit_time + query->position.z;
            break;
        case TERRAIN_HIT_TYPE_NONE:
        case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_FACE:
        case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_CYLINDER:
        case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_VERTEX:
        case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE:
            result->x = query->position.x;
            result->y = query->position.y;
            result->z = query->position.z;
            break;
    }
    NuScratchRelease();
    return TerI->hit_type;
}

extern "C" void RotateTerrain(TERRAIN_POLYGON_INFO *surface) {
    TerrainQuery_s *query = TerI;
    const f32 sin_pitch = NuTrigTable[(static_cast<i32>(query->movement_pitch) >> 1) & (NUTRIGTABLE_COUNT - 1)];
    const f32 cos_pitch = NuTrigTable[(static_cast<i32>(query->movement_pitch + 16384.0) >> 1) & (NUTRIGTABLE_COUNT - 1)];
    const f32 sin_yaw = NuTrigTable[(static_cast<i32>(query->movement_yaw) >> 1) & (NUTRIGTABLE_COUNT - 1)];
    const f32 cos_yaw = NuTrigTable[(static_cast<i32>(query->movement_yaw + 16384.0) >> 1) & (NUTRIGTABLE_COUNT - 1)];
    f32 rotated_z;

    query->transformed_vertices[0].y = surface->vertices[0].y * cos_pitch - surface->vertices[0].z * sin_pitch;
    rotated_z = surface->vertices[0].z * cos_pitch + surface->vertices[0].y * sin_pitch;
    query->transformed_vertices[0].z = rotated_z * cos_yaw - surface->vertices[0].x * sin_yaw;
    query->transformed_vertices[0].x = surface->vertices[0].x * cos_yaw + rotated_z * sin_yaw;
    query->transformed_vertices[1].y = surface->vertices[1].y * cos_pitch - surface->vertices[1].z * sin_pitch;
    rotated_z = surface->vertices[1].z * cos_pitch + surface->vertices[1].y * sin_pitch;
    query->transformed_vertices[1].z = rotated_z * cos_yaw - surface->vertices[1].x * sin_yaw;
    query->transformed_vertices[1].x = surface->vertices[1].x * cos_yaw + rotated_z * sin_yaw;
    query->transformed_vertices[2].y = surface->vertices[2].y * cos_pitch - surface->vertices[2].z * sin_pitch;
    rotated_z = surface->vertices[2].z * cos_pitch + surface->vertices[2].y * sin_pitch;
    query->transformed_vertices[2].z = rotated_z * cos_yaw - surface->vertices[2].x * sin_yaw;
    query->transformed_vertices[2].x = surface->vertices[2].x * cos_yaw + rotated_z * sin_yaw;
    if (surface->normals[1].y < 65536.0f) {
        query->transformed_vertices[3].y = surface->vertices[3].y * cos_pitch - surface->vertices[3].z * sin_pitch;
        rotated_z = surface->vertices[3].z * cos_pitch + surface->vertices[3].y * sin_pitch;
        query->transformed_vertices[3].z = rotated_z * cos_yaw - surface->vertices[3].x * sin_yaw;
        query->transformed_vertices[3].x = surface->vertices[3].x * cos_yaw + rotated_z * sin_yaw;
    }
}

extern "C" void FullDeflectSize(NUVEC *normal, NUVEC *movement, NUVEC *result) {
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

extern "C" void FullReflectTest(NUVEC *normal, NUVEC *movement, NUVEC *result) {
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

extern "C" void CubeImpact(NUMTX *current, NUMTX *previous, NUVEC *normal, f32 scale, NUVEC *impact) {
    NUVEC corners[8] = {
        {-1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f},
        {-1.0f, -1.0f, 1.0f},  {1.0f, -1.0f, 1.0f},  {1.0f, 1.0f, 1.0f},  {-1.0f, 1.0f, 1.0f},
    };
    NUVEC4 transformed[8];
    NUVEC4 old_position;
    f32 closest = 10000.0f;
    i32 selected = 0;
    for (i32 index = 0; index < 8; ++index) {
        transformed[index].x = corners[index].x;
        transformed[index].y = corners[index].y;
        transformed[index].z = corners[index].z;
        transformed[index].w = 0.0f;
        NuVec4MtxTransformVU0(&old_position, &transformed[index], previous);
        NuVec4MtxTransformVU0(&transformed[index], &transformed[index], current);
        const f32 current_distance = normal->x * transformed[index].x + normal->y * transformed[index].y + normal->z * transformed[index].z;
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

extern "C" void TerrainWallSideSlide(NUVEC *movement, void *id, f32 speed, f32 upward_scale, NUANG angle) {
    TERRAIN_SET *terrain = CurrentTerrain;
    if (terrain == NULL) {
        return;
    }
    TERRAIN_TRACK_SLOT *track;
    for (i32 index = 0; index < 64; ++index) {
        if (terrain->track_slots[index].id == id) {
            track = &terrain->track_slots[index];
            goto found;
        }
    }
    track = NULL;
found:
    CurTrackInfo = track;
    if (CurTrackInfo == NULL || CurTrackInfo->wall_contact_state == 0) {
        return;
    }
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
    if (movement->y > 0.0f) {
        movement->y = movement->y * upward_scale;
    }
}

extern "C" void TerrainPolyEdge(NUVEC *first, NUVEC *second) {
    TerrainQuery_s *query = TerI;
    if ((query->hit_type & 0xf) != TERRAIN_HIT_TYPE_CYLINDER) {
        return;
    }

    TERRAIN_GROUP *groups;
    switch (query->hit_edge) {
        case TERRAIN_HIT_EDGE_0_1:
            groups = CurrentTerrain->groups;
            first->x = groups[query->terrain_group_index].origin.x + query->surface->vertices[0].x;
            first->y = groups[query->terrain_group_index].origin.y + query->surface->vertices[0].y;
            first->z = groups[query->terrain_group_index].origin.z + query->surface->vertices[0].z;
            second->x = groups[query->terrain_group_index].origin.x + query->surface->vertices[1].x;
            second->y = groups[query->terrain_group_index].origin.y + query->surface->vertices[1].y;
            second->z = groups[query->terrain_group_index].origin.z + query->surface->vertices[1].z;
            break;

        case TERRAIN_HIT_EDGE_1_2:
            groups = CurrentTerrain->groups;
            first->x = groups[query->terrain_group_index].origin.x + query->surface->vertices[1].x;
            first->y = groups[query->terrain_group_index].origin.y + query->surface->vertices[1].y;
            first->z = groups[query->terrain_group_index].origin.z + query->surface->vertices[1].z;
            second->x = groups[query->terrain_group_index].origin.x + query->surface->vertices[2].x;
            second->y = groups[query->terrain_group_index].origin.y + query->surface->vertices[2].y;
            second->z = groups[query->terrain_group_index].origin.z + query->surface->vertices[2].z;
            break;

        case TERRAIN_HIT_EDGE_2_0:
            groups = CurrentTerrain->groups;
            first->x = groups[query->terrain_group_index].origin.x + query->surface->vertices[2].x;
            first->y = groups[query->terrain_group_index].origin.y + query->surface->vertices[2].y;
            first->z = groups[query->terrain_group_index].origin.z + query->surface->vertices[2].z;
            second->x = groups[query->terrain_group_index].origin.x + query->surface->vertices[0].x;
            second->y = groups[query->terrain_group_index].origin.y + query->surface->vertices[0].y;
            second->z = groups[query->terrain_group_index].origin.z + query->surface->vertices[0].z;
            break;

        case TERRAIN_HIT_EDGE_1_3:
            groups = CurrentTerrain->groups;
            first->x = groups[query->terrain_group_index].origin.x + query->surface->vertices[1].x;
            first->y = groups[query->terrain_group_index].origin.y + query->surface->vertices[1].y;
            first->z = groups[query->terrain_group_index].origin.z + query->surface->vertices[1].z;
            second->x = groups[query->terrain_group_index].origin.x + query->surface->vertices[3].x;
            second->y = groups[query->terrain_group_index].origin.y + query->surface->vertices[3].y;
            second->z = groups[query->terrain_group_index].origin.z + query->surface->vertices[3].z;
            break;

        case TERRAIN_HIT_EDGE_3_2:
            groups = CurrentTerrain->groups;
            first->x = groups[query->terrain_group_index].origin.x + query->surface->vertices[3].x;
            first->y = groups[query->terrain_group_index].origin.y + query->surface->vertices[3].y;
            first->z = groups[query->terrain_group_index].origin.z + query->surface->vertices[3].z;
            second->x = groups[query->terrain_group_index].origin.x + query->surface->vertices[2].x;
            second->y = groups[query->terrain_group_index].origin.y + query->surface->vertices[2].y;
            second->z = groups[query->terrain_group_index].origin.z + query->surface->vertices[2].z;
            break;
        default:
            break;
    }
}

extern "C" void NewTerrAxisFreedom(TERRAIN_AXIS_FREEDOM_SHAPE *shape, NUVEC *position) {
    f64 angle = shape->angle;
    TerrShape = shape;
    TerrShapeAdjCnt = 2;

    NUVEC axis;
    axis.x = NuTrigTable[static_cast<i32>(angle + 16384.0) >> 1 & 0x7fff];
    axis.y = 0.0f;
    axis.z = -NuTrigTable[static_cast<i32>(shape->angle) >> 1 & 0x7fff];
    TerrainSideClamp(&axis, position);
}

extern "C" i32 fun_0067bf00(i32 instance) {
    TERRAIN_SET *terrain = CurrentTerrain;
    i32 index = -1;
    i32 i;
    for (i = 0; i < terrain->max_platforms; ++i) {
        if (terrain->platforms[i].scene_object != NULL && terrain->platforms[i].scene_object_index == instance) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        return 0;
    }
    return 1;
}

extern "C" void fun_0067bee0(NUVEC *v) {
    v->x = 0.0f;
    v->y = 0.0f;
    v->z = 0.0f;
}
