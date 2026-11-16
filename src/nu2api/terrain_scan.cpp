#include "legoapi/legoapi_types.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/core/terrain_internal.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"

// Closes the open scan group: writes its shape count and group index into the header slot and opens the next one.
static __forceinline void TerrainFinishScanGroup(u8 *&group_header, TERRAIN_POLYGON_INFO **&cursor, i32 &group_shape_count, i32 group_index) {
    if (group_shape_count != 0) {
        i16 *header = reinterpret_cast<i16 *>(group_header);
        header[0] = static_cast<i16>(group_shape_count);
        header[1] = static_cast<i16>(group_index);
        group_header = reinterpret_cast<u8 *>(cursor);
        cursor = reinterpret_cast<TERRAIN_POLYGON_INFO **>(group_header + sizeof(TERRAIN_POLYGON_INFO *));
        group_shape_count = 0;
    }
}

// The unit normal of a shape with its y stretched by the object scale.
static __forceinline void TerrainScaleNormal(NUVEC &out, const NUVEC &normal) {
    TerrainQuery_s *query = TerI;
    const f32 inverse_length = TerrainDivideNonzeroPC(
        1.0f, NuFsqrt(normal.x * normal.x + normal.y * normal.y * query->object_scale_sq + normal.z * normal.z));
    query = TerI;
    out.x = normal.x * inverse_length;
    out.y = query->object_scale * normal.y * inverse_length;
    out.z = normal.z * inverse_length;
}

// Appends a candidate shape to the scan list; under a non-unit object scale it appends a scaled copy from ScaleTerrain.
static __forceinline void TerrainStoreScanShape(TERRAIN_POLYGON_INFO **&cursor, i32 &group_shape_count, i32 &scaled_shape_count,
                                  TERRAIN_POLYGON_INFO *source, const TERRAIN_GROUP &group) {
    TerrainQuery_s *query = TerI;
    if (query->object_scale == 1.0f) {
        *cursor++ = source;
        ++group_shape_count;
        return;
    }
    TERRAIN_POLYGON_INFO *scaled = &ScaleTerrain[scaled_shape_count];
    scaled->material[0] = source->material[0];
    scaled->material[1] = source->material[1];
    scaled->flags = source->flags;
    scaled->normal_flags = source->normal_flags;
    for (i32 i = 0; i < 3; i++) {
        scaled->vertices[i].x = source->vertices[i].x;
        scaled->vertices[i].z = source->vertices[i].z;
        scaled->vertices[i].y = (source->vertices[i].y + group.origin.y) * query->inverse_object_scale - group.origin.y;
    }
    if (source->normals[1].y < 65535.0f) {
        scaled->vertices[3].x = source->vertices[3].x;
        scaled->vertices[3].z = source->vertices[3].z;
        scaled->vertices[3].y = (source->vertices[3].y + group.origin.y) * query->inverse_object_scale - group.origin.y;
        TerrainScaleNormal(scaled->normals[1], source->normals[1]);
    } else {
        scaled->normals[1].y = 65536.0f;
    }
    TerrainScaleNormal(scaled->normals[0], source->normals[0]);
    *cursor++ = scaled;
    ++group_shape_count;
    ++scaled_shape_count;
}

// The scan box around the query's sweep; returns the scan radius. Scan result 1 boxes the swept segment, the others a
// sphere around the start position.
static __forceinline f32 TerrainComputeScanBounds(f32 &min_x, f32 &min_y, f32 &min_z, f32 &max_x, f32 &max_y, f32 &max_z) {
    TerrainQuery_s *query = TerI;
    f32 scan_radius;
    if (query->scan_result != 1) {
        scan_radius =
            NuFsqrt(query->movement.x * query->movement.x + query->movement.y * query->movement.y + query->movement.z * query->movement.z);
        query = TerI;
        scan_radius = query->collision_radius + 0.1f + scan_radius;
        max_x = query->position.x + 0.05f + scan_radius;
        max_y = query->position.y + 0.05f + query->object_scale * scan_radius;
        max_z = query->position.z + 0.05f + scan_radius;
        min_x = query->position.x - 0.05f - scan_radius;
        min_y = query->position.y - 0.05f - query->object_scale * scan_radius;
        min_z = query->position.z - 0.05f - scan_radius;
        return scan_radius;
    }
    if (query->movement.x > 0.0f) {
        min_x = query->position.x - 0.02f - query->collision_radius;
        max_x = query->position.x + query->movement.x + 0.02f + query->collision_radius;
    } else {
        min_x = query->position.x + query->movement.x - 0.02f - query->collision_radius;
        max_x = query->position.x + 0.02f + query->collision_radius;
    }
    if (query->movement.y > 0.0f) {
        min_y = query->position.y - 0.02f - query->collision_radius;
        max_y = query->position.y + query->movement.y + 0.02f + query->collision_radius;
    } else {
        min_y = query->position.y + query->movement.y - 0.02f - query->collision_radius;
        max_y = query->position.y + 0.02f + query->collision_radius;
    }
    if (query->movement.z > 0.0f) {
        min_z = query->position.z - 0.02f - query->collision_radius;
        max_z = query->position.z + query->movement.z + 0.02f + query->collision_radius;
    } else {
        min_z = query->position.z + query->movement.z - 0.02f - query->collision_radius;
        max_z = query->position.z + 0.02f + query->collision_radius;
    }
    return NuFsqrt((min_x - max_x) * (min_x - max_x) + (min_y - max_y) * (min_y - max_y) + (min_z - max_z) * (min_z - max_z));
}

extern "C" {
    i32 IgnoreWallSplines = 0;
    i32 WallSplinesOnly = 0;
    TERRAIN_POLYGON_INFO *ScaleTerrainT1 = 0;
}

extern "C" void ScanTerrain(i32 scan_type, i32 terrain_mask, i32 scan_flags) {
    i32 scaled_shape_count = 0;
    if (WallSplinesOnly != 0) {
        ScanWallSplineTerrain(scan_type, terrain_mask, scan_flags);
        return;
    }

    ScaleTerrain = ScaleTerrainT1;
    platinrange = 0;
    TerI->scan_group_index = -1;

    u8 *group_header;
    TERRAIN_POLYGON_INFO **cursor;
    u8 *limit;
    i32 group_shape_count;
    group_header = TerI->scan_list_storage;
    cursor = reinterpret_cast<TERRAIN_POLYGON_INFO **>(group_header + sizeof(TERRAIN_POLYGON_INFO *));
    limit = TerI->scan_list_storage + sizeof(TerI->scan_list_storage) - 12;
    group_shape_count = 0;

    TERRAIN_GROUP *terrain_groups = CurrentTerrain->groups;
    f32 min_x;
    f32 min_y;
    f32 min_z;
    f32 max_x;
    f32 max_y;
    f32 max_z;

    TerrainQuery_s *query = TerI;
    f32 scan_radius = TerrainComputeScanBounds(min_x, min_y, min_z, max_x, max_y, max_z);
    if (query->object_scale > 1.0f) {
        scan_radius += (query->object_scale - 1.0) * query->collision_radius;
    }

    for (i32 cell_index = 0; cell_index < CurrentTerrain->used_cell_count; ++cell_index) {
        const TERRAIN_CELL &cell = CurrentTerrain->cells[cell_index];
        if (!(max_x >= cell.min_x && max_z >= cell.min_z && min_x <= cell.max_x && min_z <= cell.max_z)) {
            continue;
        }

        const i16 *group_indices = CurrentTerrain->group_indices + cell.first_group;
        for (i32 cell_group = 0; cell_group < cell.group_count; ++cell_group, ++group_indices) {
            const i32 group_index = *group_indices;
            TERRAIN_GROUP &group = terrain_groups[group_index];
            if (!(max_x >= group.bounds_min.x && max_y >= group.bounds_min.y && max_z >= group.bounds_min.z &&
                  min_x <= group.bounds_max.x && min_y < group.bounds_max.y && min_z < group.bounds_max.z) ||
                group.chunk_type == -1) {
                continue;
            }

            const f32 group_max_x = max_x - group.origin.x;
            const f32 group_max_y = max_y - group.origin.y;
            const f32 group_max_z = max_z - group.origin.z;
            const f32 group_min_x = min_x - group.origin.x;
            const f32 group_min_y = min_y - group.origin.y;
            const f32 group_min_z = min_z - group.origin.z;

            if (group.scene_index < 0) {
                TerrainSkinAllocate(&group);
                query = TerI;
            }

            TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group.data);
            while (batch->marker >= 0) {
                TERRAIN_POLYGON_INFO *shapes = reinterpret_cast<TERRAIN_POLYGON_INFO *>(batch + 1);
                const i32 shape_count = batch->shape_count;
                if (group_max_x >= batch->min_x && group_min_x < batch->max_x && group_max_z >= batch->min_z &&
                    group_min_z < batch->max_z) {
                    TERRAIN_POLYGON_INFO *candidate = shapes;
                    for (i32 remaining = shape_count; remaining > 0; --remaining, ++candidate) {
                        if (!(group_max_x >= candidate->min_x && group_min_x < candidate->max_x && group_max_z >= candidate->min_z &&
                              group_min_z < candidate->max_z && group_max_y >= candidate->min_y && group_min_y < candidate->max_y) ||
                            reinterpret_cast<u8 *>(cursor) >= limit) {
                            continue;
                        }
                        if (candidate->material[1] != 0 && (candidate->material[1] & terrain_mask) == 0) {
                            continue;
                        }
                        if ((candidate->flags & scan_flags) == 0x40) {
                            continue;
                        }

                        TERRAIN_POLYGON_INFO *shape = candidate;
                        TerrainStoreScanShape(cursor, group_shape_count, scaled_shape_count, shape, group);
                    }
                }
                batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shapes + shape_count);
            }
            TerrainFinishScanGroup(group_header, cursor, group_shape_count, group_index);
        }
    }

    if (curPickInst != 0) {
        for (i32 pick_index = 0; pick_index < curPickInst; ++pick_index) {
            const i32 group_index = CurrentTerrain->max_groups + pick_index;
            TERRAIN_GROUP &group = terrain_groups[group_index];
            const f32 local_max_x = max_x - group.origin.x;
            const f32 local_max_y = max_y - group.origin.y;
            const f32 local_max_z = max_z - group.origin.z;
            const f32 local_min_x = min_x - group.origin.x;
            const f32 local_min_y = min_y - group.origin.y;
            const f32 local_min_z = min_z - group.origin.z;
            if (!(local_max_x >= group.bounds_min.x && local_max_y >= group.bounds_min.y && local_max_z >= group.bounds_min.z &&
                  local_min_x <= group.bounds_max.x && local_min_y < group.bounds_max.y && local_min_z < group.bounds_max.z) ||
                group.chunk_type == -1) {
                continue;
            }

            TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group.data);
            while (batch->marker >= 0) {
                TERRAIN_POLYGON_INFO *shapes = reinterpret_cast<TERRAIN_POLYGON_INFO *>(batch + 1);
                const i32 shape_count = batch->shape_count;
                if (local_max_x >= batch->min_x && local_min_x < batch->max_x && local_max_z >= batch->min_z &&
                    local_min_z < batch->max_z) {
                    TERRAIN_POLYGON_INFO *candidate = shapes;
                    for (i32 remaining = shape_count; remaining > 0; --remaining, ++candidate) {
                        if (!(local_max_x >= candidate->min_x && local_min_x < candidate->max_x && local_max_z >= candidate->min_z &&
                              local_min_z < candidate->max_z && local_max_y >= candidate->min_y && local_min_y < candidate->max_y) ||
                            reinterpret_cast<u8 *>(cursor) >= limit) {
                            continue;
                        }
                        if (candidate->material[1] != 0 && (candidate->material[1] & terrain_mask) == 0) {
                            continue;
                        }
                        if ((candidate->flags & scan_flags) == 0x40) {
                            continue;
                        }

                        TERRAIN_POLYGON_INFO *shape = candidate;
                        TerrainStoreScanShape(cursor, group_shape_count, scaled_shape_count, shape, group);
                    }
                }
                batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shapes + shape_count);
            }
            TerrainFinishScanGroup(group_header, cursor, group_shape_count, CurrentTerrain->max_groups + pick_index);
        }
    }
    TerI->scan_list = group_header;
    min_x -= 0.05f;
    max_x += 0.05f;
    min_y -= 0.05f;
    max_y += 0.05f;
    min_z -= 0.05f;
    max_z += 0.05f;
    if (scan_type != 0) {
        const i16 *groups;
        i32 count;
        if (min_x > CurrentTerrain->platform_query_min_x && CurrentTerrain->platform_query_max_x > max_x &&
            min_z > CurrentTerrain->platform_query_min_z && CurrentTerrain->platform_query_max_z > max_z) {
            groups = CurrentTerrain->active_platform_groups;
            count = CurrentTerrain->active_platform_count;
        } else {
            const TERRAIN_CELL &cell = CurrentTerrain->platform_cell;
            groups = CurrentTerrain->group_indices + cell.first_group;
            count = cell.group_count;
        }
        for (i32 i = 0; i < count; ++i, ++groups) {
            const i32 platform_group_index = *groups;
            TERRAIN_GROUP &group = terrain_groups[platform_group_index];
            if (group.origin.x - group.radius > max_x || group.origin.x + group.radius < min_x || group.origin.z - group.radius > max_z ||
                group.origin.z + group.radius < min_z) {
                continue;
            }
            TERRAIN_PLATFORM &platform = CurrentTerrain->platforms[group.scene_index];
            if (platform.instance_flags != NULL) {
                const u32 visible_mask = (platform.flags & TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED) != 0 ? 2 : 1;
                if ((*platform.instance_flags & visible_mask) == 0) {
                    continue;
                }
            }
            f32 local_min_x;
            f32 local_min_y;
            f32 local_min_z;
            f32 local_max_x;
            f32 local_max_y;
            f32 local_max_z;
            if (platform.scene_object != NULL) {
                if ((CurrentTerrain->platforms[group.scene_index].scene_object)->m30 >
                    CurrentTerrain->platforms[group.scene_index].previous_matrix.m30) {
                    local_min_x = min_x - group.origin.x;
                    local_max_x = ((CurrentTerrain->platforms[group.scene_index].scene_object)->m30 -
                                   CurrentTerrain->platforms[group.scene_index].previous_matrix.m30) +
                                  max_x - group.origin.x;
                } else {
                    local_max_x = max_x - group.origin.x;
                    local_min_x = ((CurrentTerrain->platforms[group.scene_index].scene_object)->m30 -
                                   CurrentTerrain->platforms[group.scene_index].previous_matrix.m30) +
                                  min_x - group.origin.x;
                }
                if ((CurrentTerrain->platforms[group.scene_index].scene_object)->m31 >
                    CurrentTerrain->platforms[group.scene_index].previous_matrix.m31) {
                    local_min_y = min_y - group.origin.y;
                    local_max_y = ((CurrentTerrain->platforms[group.scene_index].scene_object)->m31 -
                                   CurrentTerrain->platforms[group.scene_index].previous_matrix.m31) +
                                  max_y - group.origin.y;
                } else {
                    local_max_y = max_y - group.origin.y;
                    local_min_y = ((CurrentTerrain->platforms[group.scene_index].scene_object)->m31 -
                                   CurrentTerrain->platforms[group.scene_index].previous_matrix.m31) +
                                  min_y - group.origin.y;
                }
                if ((CurrentTerrain->platforms[group.scene_index].scene_object)->m32 >
                    CurrentTerrain->platforms[group.scene_index].previous_matrix.m32) {
                    local_min_z = min_z - group.origin.z;
                    local_max_z = ((CurrentTerrain->platforms[group.scene_index].scene_object)->m32 -
                                   CurrentTerrain->platforms[group.scene_index].previous_matrix.m32) +
                                  max_z - group.origin.z;
                } else {
                    local_max_z = max_z - group.origin.z;
                    local_min_z = ((CurrentTerrain->platforms[group.scene_index].scene_object)->m32 -
                                   CurrentTerrain->platforms[group.scene_index].previous_matrix.m32) +
                                  min_z - group.origin.z;
                }
            } else {
                local_max_x = max_x - group.origin.x;
                local_min_x = min_x - group.origin.x;
                local_max_y = max_y - group.origin.y;
                local_min_y = min_y - group.origin.y;
                local_max_z = max_z - group.origin.z;
                local_min_z = min_z - group.origin.z;
            }
            if ((platform.flags & TERRAIN_PLATFORM_FLAG_ROTATING) != 0) {
                if (group.chunk_type == -1) {
                    continue;
                }
                const f32 y = (min_y + max_y) * 0.5f - group.origin.y;
                const f32 x = (max_x + min_x) * 0.5f - group.origin.x;
                const f32 z = (max_z + min_z) * 0.5f - group.origin.z;
                const f32 reach = group.radius + scan_radius;
                const f32 distance_sq = x * x + y * y + z * z;
                if (!(reach * reach > distance_sq)) {
                    continue;
                }
                TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group.data);
                while (batch->marker >= 0) {
                    TERRAIN_POLYGON_INFO *shapes = reinterpret_cast<TERRAIN_POLYGON_INFO *>(batch + 1);
                    const i32 shape_count = batch->shape_count;
                    TERRAIN_POLYGON_INFO *shape = shapes;
                    for (i32 remaining = shape_count; remaining > 0; --remaining, ++shape) {
                        if (reinterpret_cast<u8 *>(cursor) >= limit) {
                            continue;
                        }
                        if (shape->material[1] != 0 && (shape->material[1] & terrain_mask) == 0) {
                            continue;
                        }
                        if ((shape->flags & scan_flags) == 0x40) {
                            continue;
                        }

                        NUVEC4 vertices[4];
                        vertices[0].x = shape->vertices[0].x;
                        vertices[0].y = shape->vertices[0].y;
                        vertices[0].z = shape->vertices[0].z;
                        vertices[0].w = 0.0f;
                        vertices[1].x = shape->vertices[1].x;
                        vertices[1].y = shape->vertices[1].y;
                        vertices[1].z = shape->vertices[1].z;
                        vertices[1].w = 0.0f;
                        vertices[2].x = shape->vertices[2].x;
                        vertices[2].y = shape->vertices[2].y;
                        vertices[2].z = shape->vertices[2].z;
                        vertices[2].w = 0.0f;
                        NuVec4MtxTransformVU0x3(vertices, vertices, (CurrentTerrain->platforms[group.scene_index].scene_object));
                        if (shape->normals[1].y < 65535.0f) {
                            vertices[3].x = shape->vertices[3].x;
                            vertices[3].y = shape->vertices[3].y;
                            vertices[3].z = shape->vertices[3].z;
                            vertices[3].w = 0.0f;
                            NuVec4MtxTransformVU0(&vertices[3], &vertices[3], (CurrentTerrain->platforms[group.scene_index].scene_object));
                        } else {
                            vertices[3] = vertices[2];
                        }
                        if (!(vertices[0].x > local_min_x || vertices[1].x > local_min_x || vertices[2].x > local_min_x ||
                              vertices[3].x > local_min_x)) {
                            query = TerI;
                            continue;
                        }
                        if (!(vertices[0].x < local_max_x || vertices[1].x < local_max_x || vertices[2].x < local_max_x ||
                              vertices[3].x < local_max_x)) {
                            query = TerI;
                            continue;
                        }
                        if (!(vertices[0].z > local_min_z || vertices[1].z > local_min_z || vertices[2].z > local_min_z ||
                              vertices[3].z > local_min_z)) {
                            query = TerI;
                            continue;
                        }
                        if (!(vertices[0].z < local_max_z || vertices[1].z < local_max_z || vertices[2].z < local_max_z ||
                              vertices[3].z < local_max_z)) {
                            query = TerI;
                            continue;
                        }
                        TERRAIN_POLYGON_INFO *transformed = &ScaleTerrain[scaled_shape_count];
                        transformed->material[0] = shape->material[0];
                        transformed->material[1] = shape->material[1];
                        transformed->flags = shape->flags;
                        transformed->normal_flags = shape->normal_flags;
                        query = TerI;
                        platinrange = 1;
                        transformed->vertices[0].x = vertices[0].x;
                        transformed->vertices[0].z = vertices[0].z;
                        transformed->vertices[0].y = (vertices[0].y + group.origin.y) * query->inverse_object_scale - group.origin.y;
                        transformed->vertices[1].x = vertices[1].x;
                        transformed->vertices[1].z = vertices[1].z;
                        transformed->vertices[1].y = (vertices[1].y + group.origin.y) * query->inverse_object_scale - group.origin.y;
                        transformed->vertices[2].x = vertices[2].x;
                        transformed->vertices[2].z = vertices[2].z;
                        transformed->vertices[2].y = (vertices[2].y + group.origin.y) * query->inverse_object_scale - group.origin.y;
                        if (shape->normals[1].y < 65535.0f) {
                            transformed->vertices[3].x = vertices[3].x;
                            transformed->vertices[3].z = vertices[3].z;
                            transformed->vertices[3].y = (vertices[3].y + group.origin.y) * query->inverse_object_scale - group.origin.y;
                            vertices[0].x = transformed->vertices[1].x - transformed->vertices[3].x;
                            vertices[0].y = transformed->vertices[1].y - transformed->vertices[3].y;
                            vertices[0].z = transformed->vertices[1].z - transformed->vertices[3].z;
                            vertices[1].x = transformed->vertices[2].x - transformed->vertices[3].x;
                            vertices[1].y = transformed->vertices[2].y - transformed->vertices[3].y;
                            vertices[1].z = transformed->vertices[2].z - transformed->vertices[3].z;
                            {
                                NUVEC cross;
                                cross.x = vertices[0].y * vertices[1].z - vertices[0].z * vertices[1].y;
                                cross.y = vertices[0].z * vertices[1].x - vertices[1].z * vertices[0].x;
                                cross.z = vertices[0].x * vertices[1].y - vertices[1].x * vertices[0].y;
                                transformed->normals[1] = cross;
                            }
                            const f32 inverse =
                                TerrainDivideNonzeroPC(1.0f, NuFsqrt(transformed->normals[1].x * transformed->normals[1].x +
                                                                     transformed->normals[1].y * transformed->normals[1].y +
                                                                     transformed->normals[1].z * transformed->normals[1].z));
                            transformed = &ScaleTerrain[scaled_shape_count];
                            transformed->normals[1].x *= inverse;
                            transformed->normals[1].y *= inverse;
                            transformed->normals[1].z *= inverse;
                        } else {
                            transformed->normals[1].y = 65536.0f;
                        }
                        {
                            vertices[0].x = transformed->vertices[2].x - transformed->vertices[0].x;
                            vertices[0].y = transformed->vertices[2].y - transformed->vertices[0].y;
                            vertices[0].z = transformed->vertices[2].z - transformed->vertices[0].z;
                            vertices[1].x = transformed->vertices[1].x - transformed->vertices[0].x;
                            vertices[1].y = transformed->vertices[1].y - transformed->vertices[0].y;
                            vertices[1].z = transformed->vertices[1].z - transformed->vertices[0].z;
                            {
                                NUVEC cross;
                                cross.x = vertices[0].y * vertices[1].z - vertices[0].z * vertices[1].y;
                                cross.y = vertices[0].z * vertices[1].x - vertices[1].z * vertices[0].x;
                                cross.z = vertices[0].x * vertices[1].y - vertices[1].x * vertices[0].y;
                                transformed->normals[0] = cross;
                            }
                            const f32 length = NuFsqrt(transformed->normals[0].x * transformed->normals[0].x +
                                                       transformed->normals[0].y * transformed->normals[0].y +
                                                       transformed->normals[0].z * transformed->normals[0].z);
                            const f32 inverse = length == 0.0f ? 0.0f : 1.0f / length;
                            transformed = &ScaleTerrain[scaled_shape_count];
                            transformed->normals[0].x *= inverse;
                            transformed->normals[0].y *= inverse;
                            transformed->normals[0].z *= inverse;
                        }
                        *cursor++ = transformed;
                        ++group_shape_count;
                        ++scaled_shape_count;
                        query = TerI;
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shapes + shape_count);
                }
            } else {
                if (!(local_max_x >= group.bounds_min.x && local_max_y >= group.bounds_min.y && local_max_z >= group.bounds_min.z &&
                      local_min_x <= group.bounds_max.x && local_min_y < group.bounds_max.y && local_min_z < group.bounds_max.z) ||
                    group.chunk_type == -1) {
                    continue;
                }
                TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group.data);
                while (batch->marker >= 0) {
                    TERRAIN_POLYGON_INFO *shapes = reinterpret_cast<TERRAIN_POLYGON_INFO *>(batch + 1);
                    const i32 shape_count = batch->shape_count;
                    if (local_max_x >= batch->min_x && local_min_x < batch->max_x && local_max_z >= batch->min_z &&
                        local_min_z < batch->max_z) {
                        TERRAIN_POLYGON_INFO *candidate = shapes;
                        for (i32 remaining = shape_count; remaining > 0; --remaining, ++candidate) {
                            if (!(local_max_x >= candidate->min_x && local_min_x < candidate->max_x && local_max_z >= candidate->min_z &&
                                  local_min_z < candidate->max_z && local_max_y >= candidate->min_y && local_min_y < candidate->max_y) ||
                                reinterpret_cast<u8 *>(cursor) >= limit) {
                                continue;
                            }
                            if (candidate->material[1] != 0 && (candidate->material[1] & terrain_mask) == 0) {
                                continue;
                            }
                            if ((candidate->flags & scan_flags) == 0x40) {
                                continue;
                            }

                            platinrange = 1;
                            TERRAIN_POLYGON_INFO *shape = candidate;
                            TerrainStoreScanShape(cursor, group_shape_count, scaled_shape_count, shape, group);
                        }
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shapes + shape_count);
                }
            }
            TerrainFinishScanGroup(group_header, cursor, group_shape_count, platform_group_index);
        }
    }

    i16 *terminator = reinterpret_cast<i16 *>(group_header);
    terminator[0] = 0;
    terminator[1] = 0;
    i32 wall_count = 0;
    min_x -= 0.02f;
    min_z -= 0.02f;
    TERRAIN_SPATIAL_NODE *node = CurrentTerrain->spatial_nodes;
    WallSplCount = wall_count;
    max_x += 0.02f;
    max_z += 0.02f;
    if (IgnoreWallSplines != 0) {
        IgnoreWallSplines = 0;
    } else {
        for (; node != NULL;
             node = reinterpret_cast<TERRAIN_SPATIAL_CHUNK *>(reinterpret_cast<u8 *>(node) - offsetof(TERRAIN_SPATIAL_CHUNK, node))
                        ->previous) {
            const u8 *material_bytes = reinterpret_cast<const u8 *>(&node->unknown_02);
            if (material_bytes[1] != 0 && (material_bytes[1] & terrain_mask) == 0) {
                continue;
            }
            const u32 material_low = material_bytes[0];
            const u32 mask = material_bytes[1];
            for (i32 first = 0; first < node->point_count; first += 16) {
                NUVEC *points = node->points + first;
                i32 end;
                if (points[0].y != 2147483648.0f) {
                    if (points[1].y >= min_x && max_x >= points[0].y && points[3].y >= min_z && max_z >= points[2].y) {
                        end = first + 16;
                        if (end > node->point_count) {
                            end = node->point_count;
                        }
                    } else {
                        end = 0;
                    }
                } else {
                    end = node->point_count;
                }
                TERRAIN_WALL_POINT *entry = WallSplList + wall_count;
                i32 i = first;
                while (i < end) {
                    NUVEC &a = node->points[i];
                    NUVEC &b = node->points[i + 1];
                    ++i;
                    if (!((a.x >= min_x && b.x < max_x) || (b.x >= min_x && a.x <= max_x))) {
                        continue;
                    }
                    if (!((a.z >= min_z && b.z < max_z) || (b.z >= min_z && a.z <= max_z))) {
                        continue;
                    }
                    if (wall_count < 64) {
                        entry->position = a;
                        entry->material[0] = material_low;
                        entry->material[1] = mask;
                        entry->material[2] = 0;
                        entry->material[3] = 0;
                        ++entry;
                        ++wall_count;
                        entry->position = b;
                        entry->material[0] = material_low;
                        entry->material[1] = mask;
                        entry->material[2] = 0;
                        entry->material[3] = 0;
                        ++entry;
                        ++wall_count;
                    }
                }
            }
        }
        WallSplCount = wall_count;
    }
    TerrOverRideScan = NULL;
}

extern "C" void ScanWallSplineTerrain(i32 scan_type, i32 terrain_mask, i32 scan_flags) {
    TerrainQuery_s *query = TerI;
    WallSplinesOnly = 0;
    ScaleTerrain = ScaleTerrainT1;
    i16 *terminator = reinterpret_cast<i16 *>(query->scan_list_storage);
    platinrange = 0;
    query->scan_group_index = -1;
    f32 min_x;
    f32 min_y;
    f32 min_z;
    f32 max_x;
    f32 max_y;
    f32 max_z;
    TerrainComputeScanBounds(min_x, min_y, min_z, max_x, max_y, max_z);
    terminator[0] = 0;
    terminator[1] = 0;
    min_x -= 0.02f;
    min_z -= 0.02f;
    max_x += 0.02f;
    max_z += 0.02f;
    WallSplCount = 0;
    i32 wall_count = 0;
    for (TERRAIN_SPATIAL_NODE *node = CurrentTerrain->spatial_nodes; node != NULL;
         node = reinterpret_cast<TERRAIN_SPATIAL_CHUNK *>(reinterpret_cast<u8 *>(node) - offsetof(TERRAIN_SPATIAL_CHUNK, node))->previous) {
        const u8 *material_bytes = reinterpret_cast<const u8 *>(&node->unknown_02);
        if (material_bytes[1] != 0 && (material_bytes[1] & terrain_mask) == 0) {
            continue;
        }
        const u32 material = material_bytes[0];
        const u32 mask = material_bytes[1];
        for (i32 first = 0; first < node->point_count; first += 16) {
            i32 end;
            if (node->points[first].y != 2147483648.0f) {
                if (node->points[first + 1].y >= min_x && node->points[first].y <= max_x && node->points[first + 3].y >= min_z &&
                    node->points[first + 2].y <= max_z) {
                    end = first + 16;
                    if (end > node->point_count) {
                        end = node->point_count;
                    }
                } else {
                    end = 0;
                }
            } else {
                end = node->point_count;
            }
            TERRAIN_WALL_POINT *entry = WallSplList + wall_count;
            for (i32 i = first; i < end; ++i) {
                NUVEC *a = &node->points[i];
                NUVEC *b = &node->points[i + 1];
                if ((a->x >= min_x && b->x <= max_x) || (b->x >= min_x && a->x <= max_x)) {
                    if ((a->z >= min_z && b->z <= max_z) || (b->z >= min_z && a->z <= max_z)) {
                        if (wall_count < 64) {
                            entry->position = *a;
                            entry->material[0] = material;
                            entry->material[1] = mask;
                            entry->material[2] = 0;
                            entry->material[3] = 0;
                            ++wall_count;
                            ++entry;
                            entry->position = *b;
                            entry->material[0] = material;
                            entry->material[1] = mask;
                            entry->material[2] = 0;
                            entry->material[3] = 0;
                            ++wall_count;
                            ++entry;
                        }
                    }
                }
            }
            WallSplCount = wall_count;
        }
    }
    TerrOverRideScan = NULL;
}
