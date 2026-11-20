#include "gameapi/edbits.h"
#include "gameapi/edptl.h"
#include "gameapi/edtools/edanim_internal.h"
#include "gameapi/edtools/edbri_internal.h"
#include "gameapi/edtools/edgra_internal.h"
#include "gameapi/edtools/edpp_internal.h"
#include "gameapi/edtools/edtools_types.h"
#include "gameapi/edtools/edui.h"
#include "globals.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nucore/numem.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/numusic/sfx.h"
#include "nu2api/nustencilshadows_PC.h"
TERRAIN_POLYGON_INFO *TerrainCastAuxPolygon = 0;
f32 EShadY = 0;
u16 TerrainCastAuxValid = 0;
i32 cntrots = 0;
static u8 *TerrainCrashData = 0;
i32 edanim_cam_ax = 0;
i32 platinrange = 0;
struct eduimenu_s *ptlrotmenu = 0;
struct eduimenu_s *ptlstartvelmenu = 0;
i32 edgra_page_on[8] = {0};
i32 edbri_page_used[8] = {0};
i32 freedebkeyptr = 0;
i32 edgra_instance_type = 0;
struct eduimenu_s *ptlemitvelmenu = 0;
NUMTL *edanim_mtl = 0;
struct eduimenu_s *ptlreadoutmenu = 0;
DEBRIS_PARTICLE_CHUNK **freedebchunksglass = 0;
DEBRIS_PARTICLE_CHUNK **freedebchunks = 0;
NUVEC TerrainCastAuxNormal = {0};
struct eduimenu_s *emittimemenu = 0;
TERRAIN_CALLBACK_LIFECYCLE PlatCallback[8] = {0};
i32 terrhitflags = 0;
i16 TerrImpact = 0;
u8 TerrWallInfo = 0;
i16 NuSound3GroupSampleIds[512] = {0};
TERRAIN_SPHERE SphereData[16] = {0};
u16 SfxBits[100] = {0};
TERRAIN_POLYGON_INFO *ShadRoofPoly = 0;
struct eduimenu_s *ptldatamenu = 0;
i32 PlatCrush = 0;
TERRAIN_AXIS_FREEDOM_SHAPE *TerrShape = 0;
struct eduimenu_s *ptlsizemenu = 0;
struct eduimenu_s *ptlcolmenu = 0;
void *edgra_page_terrain[8] = {0};
f32 GetSurfaceInfoRoofHeight = 0;
i32 edbri_cam_ax = 0;
TerrainQuery_s *TerI = 0;
static i16 debug_index = 0;
TERRAIN_SET *CurrentTerrain = 0;
TERRAIN_POLYGON_INFO *EShadPoly = 0;
i32 TerrPolyObj = 0;
u16 TerrainWaterCastIndex = 0;

const f32 TerrainUnusedSpatialBlock = 2147483648.0f;
const f32 TerrainBoundsMinimum = -200000000.0f;
const f32 TerrainBoundsMaximum = 200000000.0f;
const f64 TerrainSkinSampleIndexScale = -8.0;
const f32 TerrainSkinBoundsMin = -123456792.0f;
const f32 TerrainSkinBoundsMax = 123456792.0f;
const f32 TerrainCastLowerSentinel = -2000000.0f;

void TerrainSetCur(void *terrain) {
    CurrentTerrain = terrain;
}

void *TerrainGetCur(void) {
    return CurrentTerrain;
}

void PlatCallbackAdd(void (*function)(void *argument), void *argument) {
    if (PlatCodeCallback < 8) {
        PlatCallback[PlatCodeCallback].function = function;
        PlatCallback[PlatCodeCallback].argument = argument;
        PlatCodeCallback++;
    }
}

void PlatCallbackRemove(void (*function)(void *argument)) {
    i32 i;
    for (i = 0; i < PlatCodeCallback; ++i) {
        if (PlatCallback[i].function == function) {
            PlatCallback[i].function = PlatCallback[PlatCodeCallback - 1].function;
            --PlatCodeCallback;
            PlatCallback[i].argument = PlatCallback[PlatCodeCallback].argument;
            --i;
        }
    }
}

i32 PlatInstGetHit(i32 index) {
    TERRAIN_SET *terrain = CurrentTerrain;
    if (terrain == NULL || index < 0 || index >= terrain->max_platforms) {
        return 0;
    }
    return (terrain->platforms[index].flags >> 1) & 1;
}

void PlatInstCenter(i32 index, NUVEC *position) {
    TERRAIN_SET *terrain = CurrentTerrain;
    if (terrain != NULL && index >= 0 && index < terrain->max_platforms) {
        *position = terrain->groups[terrain->platforms[index].terrain_group_index].origin;
    }
}

void ScanTerrIDRemovePlat(i32 platform_index) {
    i32 i;

    for (i = 0; i < 64; i++) {
        if (CurrentTerrain->track_slots[i].platform_index == platform_index) {
            CurrentTerrain->track_slots[i].id = NULL;
        }
    }
}

i32 NewPlatInst(void *object, i32 instance) {
    TERRAIN_SET *terrain = CurrentTerrain;
    i32 new_index;
    i32 source_index;
    i32 i;

    if (terrain == NULL) {
        return -1;
    }
    if (terrain->group_index_count >= terrain->max_group_indices) {
        return -1;
    }
    if (terrain->group_count >= terrain->max_groups) {
        return -1;
    }
    if (object == NULL) {
        return -1;
    }

    new_index = -1;
    for (i = 0; i < terrain->max_platforms; i++) {
        if (terrain->platforms[i].scene_object == NULL) {
            new_index = i;
            break;
        }
    }
    if (new_index == -1) {
        return -1;
    }

    source_index = -1;
    for (i = 0; i < terrain->max_platforms; i++) {
        if (terrain->platforms[i].scene_object != NULL && terrain->platforms[i].scene_object_index == instance) {
            source_index = i;
            break;
        }
    }
    if (source_index == -1) {
        return -1;
    }

    CurrentTerrain->groups[terrain->group_count] = CurrentTerrain->groups[terrain->platforms[source_index].terrain_group_index];
    CurrentTerrain->groups[CurrentTerrain->group_count].scene_index = (i16)new_index;
    CurrentTerrain->groups[CurrentTerrain->group_count].chunk_type = 1;
    CurrentTerrain->platforms[new_index].scene_object = object;
    CurrentTerrain->platforms[new_index].terrain_group_index = CurrentTerrain->group_count;
    CurrentTerrain->platforms[new_index].scene_object_index = (i16)instance;
    CurrentTerrain->platforms[new_index].instance_flags = NULL;
    CurrentTerrain->platforms[new_index].flags ^=
        (CurrentTerrain->platforms[source_index].flags ^ CurrentTerrain->platforms[new_index].flags) & 1;
    CurrentTerrain->group_indices[CurrentTerrain->group_index_count] = CurrentTerrain->group_count;
    CurrentTerrain->group_index_count++;
    CurrentTerrain->platforms[new_index].bounce_impulse = 0.0f;
    CurrentTerrain->platforms[new_index].bounce_offset = 0.0f;
    CurrentTerrain->platforms[new_index].bounce_velocity = 0.0f;
    CurrentTerrain->platforms[new_index].bounce_damping = 0.0f;
    CurrentTerrain->platforms[new_index].bounce_spring = 0.0f;
    CurrentTerrain->group_count++;
    CurrentTerrain->platform_cell.group_count++;
    return new_index;
}

i32 NewPlatInstMSitu(void *object, i32 source) {
    TERRAIN_SET *terrain = CurrentTerrain;
    i32 new_index;
    i32 i;

    if (terrain == NULL) {
        return -1;
    }
    if (terrain->group_index_count >= terrain->max_group_indices) {
        return -1;
    }
    if (terrain->group_count >= terrain->max_groups) {
        return -1;
    }
    if (object == NULL) {
        return -1;
    }

    new_index = -1;
    for (i = 0; i < terrain->max_platforms; i++) {
        if (terrain->platforms[i].scene_object == NULL) {
            new_index = i;
            break;
        }
    }
    if (new_index == -1) {
        return -1;
    }
    if (source == -1) {
        return -1;
    }

    terrain->groups[terrain->group_count] = terrain->groups[source];
    CurrentTerrain->groups[CurrentTerrain->group_count].scene_index = (i16)new_index;
    CurrentTerrain->groups[CurrentTerrain->group_count].chunk_type = 1;
    CurrentTerrain->platforms[new_index].scene_object = object;
    CurrentTerrain->platforms[new_index].terrain_group_index = CurrentTerrain->group_count;
    CurrentTerrain->platforms[new_index].scene_object_index = source;
    CurrentTerrain->platforms[new_index].instance_flags = NULL;
    CurrentTerrain->platforms[new_index].flags &= ~TERRAIN_PLATFORM_FLAG_ROTATING;
    CurrentTerrain->platforms[new_index].bounce_impulse = 0.0f;
    CurrentTerrain->platforms[new_index].bounce_offset = 0.0f;
    CurrentTerrain->platforms[new_index].bounce_velocity = 0.0f;
    CurrentTerrain->platforms[new_index].bounce_damping = 0.0f;
    CurrentTerrain->platforms[new_index].bounce_spring = 0.0f;
    CurrentTerrain->group_indices[CurrentTerrain->group_index_count] = CurrentTerrain->group_count;
    CurrentTerrain->group_index_count++;
    CurrentTerrain->platform_cell.group_count++;
    CurrentTerrain->group_count++;
    return new_index;
}

i32 NewMSituTerrEx(NUVEC *position, TERRAIN_SKIN_STREAM *data, NUVEC *minimum, NUVEC *maximum, f32 radius) {
    TERRAIN_SET *terrain = CurrentTerrain;

    if (terrain == NULL) {
        return -1;
    }
    if (terrain->group_index_count >= terrain->max_group_indices) {
        return -1;
    }
    if (terrain->group_count >= terrain->max_groups) {
        return -1;
    }

    terrain->groups[terrain->group_count].group_info = -1;
    CurrentTerrain->groups[CurrentTerrain->group_count].scene_index = -1;
    CurrentTerrain->groups[CurrentTerrain->group_count].platform_flags &= ~1;
    CurrentTerrain->groups[CurrentTerrain->group_count].chunk_type = 0;
    CurrentTerrain->groups[CurrentTerrain->group_count].origin = *position;
    CurrentTerrain->groups[CurrentTerrain->group_count].data = data;
    CurrentTerrain->groups[CurrentTerrain->group_count].bounds_min = *minimum;
    CurrentTerrain->groups[CurrentTerrain->group_count].bounds_max = *maximum;
    CurrentTerrain->groups[CurrentTerrain->group_count].radius = radius;
    CurrentTerrain->group_indices[CurrentTerrain->group_index_count] = CurrentTerrain->group_count;
    CurrentTerrain->group_index_count++;
    CurrentTerrain->platform_cell.group_count++;
    CurrentTerrain->group_count++;
    return CurrentTerrain->group_count - 1;
}

i32 DeletePlatinst(i32 platform_index) {
    i32 i;
    i32 cell_group_index;

    if (platform_index < 0 || platform_index >= CurrentTerrain->max_platforms) {
        return 0;
    }
    if (CurrentTerrain->platforms[platform_index].scene_object == NULL) {
        return 0;
    }

    CurrentTerrain->group_count--;
    CurrentTerrain->group_index_count--;
    CurrentTerrain->platforms[CurrentTerrain->groups[CurrentTerrain->group_count].scene_index].terrain_group_index =
        CurrentTerrain->platforms[platform_index].terrain_group_index;
    CurrentTerrain->groups[CurrentTerrain->platforms[platform_index].terrain_group_index] =
        CurrentTerrain->groups[CurrentTerrain->group_count];
    CurrentTerrain->groups[CurrentTerrain->group_count].chunk_type = -1;
    CurrentTerrain->platforms[platform_index].scene_object = NULL;

    cell_group_index = CurrentTerrain->platform_cell.first_group;
    for (i = 0; i < CurrentTerrain->platform_cell.group_count; i++, cell_group_index++) {
        if (CurrentTerrain->group_indices[cell_group_index] == CurrentTerrain->group_count) {
            CurrentTerrain->group_indices[cell_group_index] = CurrentTerrain->group_indices[CurrentTerrain->group_index_count];
            break;
        }
    }
    CurrentTerrain->platform_cell.group_count--;
    ScanTerrIDRemovePlat(platform_index);
    return 1;
}

void UpdatePlatinst(i32 index, void *object) {
    if (index >= 0 && index < CurrentTerrain->max_platforms) {
        CurrentTerrain->platforms[index].scene_object = object;
    }
}

void fun_0067cab0(void) {
}

i32 PlatformCrush(void) {
    return PlatCrush;
}

i32 ShadowInfo(void) {
    return ShadPoly != NULL ? ShadPoly->material[0] : -1;
}

i32 ShadowIntensityInfo(void) {
    return ShadPoly != NULL ? ShadPoly->normal_flags - 8 : -1;
}

void ShadowDir(NUVEC *direction) {
    TERRAIN_POLYGON_INFO *poly = ShadPoly;
    if (poly != NULL) {
        direction->x = poly->vertices[1].x - poly->vertices[0].x;
        direction->y = poly->vertices[1].y - poly->vertices[0].y;
        direction->z = poly->vertices[1].z - poly->vertices[0].z;
    }
}

i32 EShadowInfo(void) {
    return EShadPoly != NULL ? EShadPoly->material[1] : -1;
}

i32 ShadowRoofInfo(void) {
    return ShadRoofPoly != NULL ? ShadRoofPoly->material[0] : -1;
}

i32 NewShadowOnPlatform(void) {
    TERRAIN_SET *terrain = CurrentTerrain;
    TERRAIN_GROUP *group;
    if (terrain == NULL || castnum == -1) {
        return -1;
    }
    group = &terrain->groups[castnum];
    if (group->chunk_type != 1) {
        return -1;
    }
    return group->scene_index;
}

i32 NewShadowOnMSitu(void) {
    if (castnum != -1) {
        return castnum;
    }
    if (TerrPolyObj != -1) {
        return TerrPolyObj;
    }
    return -1;
}

typedef struct TERRAIN_SKIN_BLEND {        // size 0x5c
    NUMTX end_matrix; /* +00 */            // 0x00
    NUVEC row_amplitude; /* +40 */         // 0x40
    u8 opaque4c_50[4];                     // 0x4c
    NUVEC translation_amplitude; /* +50 */ // 0x50
} TERRAIN_SKIN_BLEND;

typedef struct TERRAIN_SKIN_INFO {             // size 0x1c
    NUMTX *matrix; /* +00 */                   // 0x00
    const TERRAIN_SKIN_BLEND *blend; /* +04 */ // 0x04

    union {                        // size 0x4
        const NUVEC4 *sample_rows; // sampled row banks
        NUMTX *static_matrix;      // static platform matrix
    }; // 0x08

    f32 scale; /* +0c */                           // 0x0c
    TERRAIN_SKIN_STREAM *original_shape; /* +10 */ // 0x10
    i16 group_index; /* +14 */                     // 0x14
    i16 pool_slot; /* +16 */                       // 0x16
    i16 mode; /* +18 */                            // 0x18
    i16 vertex_order; /* +1a */                    // 0x1a
} TERRAIN_SKIN_INFO;

typedef struct TERRAIN_SKIN_USE_RECORD { // size 0x8
    i32 last_use;                        // 0x0
    i16 skin_index;                      // 0x4
    u8 unknown_06[2];                    // 0x6
} TERRAIN_SKIN_USE_RECORD;

NUVEC *TerrainSkin(NUVEC *result, const TERRAIN_SKIN_INFO *info, const NUVEC *position, f32 weight, i32 mode) {
    NUVEC4 point;
    NUMTX matrix;
    const NUVEC4 *rows;
    switch (mode) {
        case 1:
            point.x = position->x;
            point.y = position->y;
            point.z = position->z;
            point.w = 1.0f;
            NuVec4MtxTransformVU0(&point, &point, info->matrix);
            break;
        default:
            if (weight < 0.0f) {
                weight = position->z / info->scale;
                if (weight < 0.0f) {
                    weight = 0.0f;
                }
            }
            if (weight > 1.0f) {
                weight = 1.0f;
            }
            point.x = position->x;
            point.y = position->y;
            rows = info->sample_rows;
            if (rows != NULL) {
                weight = (i32)(weight * 8.0 + 0.5) * 0.125;
            } else {
                weight = (i32)(weight * 10.0 + 0.5) / 10.0;
            }
            point.z = position->z - info->scale * weight;
            point.w = 1.0f;
            if (rows != NULL) {
                i32 sample = (i32)(weight * -8.0);
                matrix.m00 = rows[-sample].x;
                matrix.m01 = rows[-sample].y;
                matrix.m02 = rows[-sample].z;
                matrix.m03 = rows[-sample].w;
                matrix.m10 = rows[11 - sample].x;
                matrix.m11 = rows[11 - sample].y;
                matrix.m12 = rows[11 - sample].z;
                matrix.m13 = rows[11 - sample].w;
                matrix.m20 = rows[22 - sample].x;
                matrix.m21 = rows[22 - sample].y;
                matrix.m22 = rows[22 - sample].z;
                matrix.m23 = rows[22 - sample].w;
                matrix.m30 = rows[33 - sample].x;
                matrix.m31 = rows[33 - sample].y;
                matrix.m32 = rows[33 - sample].z;
                matrix.m33 = rows[33 - sample].w;
            } else {
                const NUMTX *start = info->matrix;
                const TERRAIN_SKIN_BLEND *blend = info->blend;
                i32 angle = (i32)(weight * 32768.0);
                matrix.m00 =
                    (weight * blend->end_matrix.m00 + start->m00 * (1.0f - weight)) * (NU_SIN_LUT(angle) * blend->row_amplitude.x + 1.0f);
                matrix.m01 =
                    (weight * blend->end_matrix.m01 + start->m01 * (1.0f - weight)) * (NU_SIN_LUT(angle) * blend->row_amplitude.x + 1.0f);
                matrix.m02 =
                    (weight * blend->end_matrix.m02 + start->m02 * (1.0f - weight)) * (NU_SIN_LUT(angle) * blend->row_amplitude.x + 1.0f);
                matrix.m03 =
                    (weight * blend->end_matrix.m03 + start->m03 * (1.0f - weight)) * (NU_SIN_LUT(angle) * blend->row_amplitude.x + 1.0f);
                matrix.m10 =
                    (weight * blend->end_matrix.m10 + start->m10 * (1.0f - weight)) * (NU_SIN_LUT(angle) * blend->row_amplitude.y + 1.0f);
                matrix.m11 =
                    (weight * blend->end_matrix.m11 + start->m11 * (1.0f - weight)) * (NU_SIN_LUT(angle) * blend->row_amplitude.y + 1.0f);
                matrix.m12 =
                    (weight * blend->end_matrix.m12 + start->m12 * (1.0f - weight)) * (NU_SIN_LUT(angle) * blend->row_amplitude.y + 1.0f);
                matrix.m13 =
                    (weight * blend->end_matrix.m13 + start->m13 * (1.0f - weight)) * (NU_SIN_LUT(angle) * blend->row_amplitude.y + 1.0f);
                matrix.m20 =
                    (weight * blend->end_matrix.m20 + start->m20 * (1.0f - weight)) * (NU_SIN_LUT(angle) * blend->row_amplitude.z + 1.0f);
                matrix.m21 =
                    (weight * blend->end_matrix.m21 + start->m21 * (1.0f - weight)) * (NU_SIN_LUT(angle) * blend->row_amplitude.z + 1.0f);
                matrix.m22 =
                    (weight * blend->end_matrix.m22 + start->m22 * (1.0f - weight)) * (NU_SIN_LUT(angle) * blend->row_amplitude.z + 1.0f);
                matrix.m23 =
                    (weight * blend->end_matrix.m23 + start->m23 * (1.0f - weight)) * (NU_SIN_LUT(angle) * blend->row_amplitude.z + 1.0f);
                matrix.m30 =
                    weight * blend->end_matrix.m30 + start->m30 * (1.0f - weight) + NU_SIN_LUT(angle) * blend->translation_amplitude.x;
                matrix.m31 =
                    weight * blend->end_matrix.m31 + start->m31 * (1.0f - weight) + NU_SIN_LUT(angle) * blend->translation_amplitude.y;
                matrix.m32 =
                    weight * blend->end_matrix.m32 + start->m32 * (1.0f - weight) + NU_SIN_LUT(angle) * blend->translation_amplitude.z;
                matrix.m33 = weight * blend->end_matrix.m33 + start->m33 * (1.0f - weight);
            }
            NuVec4MtxTransformVU0(&point, &point, &matrix);
            break;
    }
    result->x = point.x;
    result->y = point.y;
    result->z = point.z;
    return result;
}

/* Valid initialized variable records, vertex_order 0 or 1, and enough output
 * storage for each complete 20-byte header and 100-byte polygon. */
void SkinPlatform(TERRAIN_GROUP *group, TERRAIN_SKIN_STREAM *output, const TERRAIN_SKIN_INFO *info) {
    TERRAIN_SKIN_STREAM *input;
    TERRAIN_SKIN_STREAM *cursor;
    TERRAIN_POLYGON_INFO *source;
    TERRAIN_POLYGON_INFO *polygon;
    NUVEC *p;
    NUVEC transformed;
    NUVEC a;
    NUVEC b;
    i32 count;
    i32 vertex;
    f32 z;
    f32 min_x;
    f32 min_y;
    f32 min_z;
    f32 max_x;
    f32 max_y;
    f32 max_z;
    f32 pmin_x;
    f32 pmin_y;
    f32 pmin_z;
    f32 pmax_x;
    f32 pmax_y;
    f32 pmax_z;
    if (CurrentTerrain == NULL) {
        return;
    }
    if (group->chunk_type != 0 && group->chunk_type != 1) {
        return;
    }
    input = group->data;
    cursor = output;
    while (input->header.kind >= 0) {
        cursor->header.kind = input->header.kind;
        cursor->header.count = input->header.count;
        min_x = min_y = min_z = 123456789.0f;
        max_x = max_y = max_z = -123456789.0f;
        count = input->header.count;
        source = input->polygons;
        polygon = cursor->polygons;
        for (; count > 0; --count) {
            if (source->normals[1].y > 65535.0) {
                vertex = 2;
            } else {
                vertex = 3;
            }
            pmin_x = pmin_y = pmin_z = 123456789.0f;
            *polygon = *source;
            pmax_x = pmax_y = pmax_z = -123456789.0f;
            for (p = &polygon->vertices[vertex]; vertex >= 0; --vertex, --p) {
                *p = *TerrainSkin(&transformed, info, &source->vertices[SkinFlipTab[vertex + info->vertex_order * 4]], -1.0f, info->mode);
                p->x = p->x - info->matrix->m30;
                p->y = p->y - info->matrix->m31;
                p->z = p->z - info->matrix->m32;
                z = p->z;
                if (p->x < min_x) {
                    min_x = p->x;
                }
                if (p->x > max_x) {
                    max_x = p->x;
                }
                if (p->y < min_y) {
                    min_y = p->y;
                }
                if (p->y > max_y) {
                    max_y = p->y;
                }
                if (z < min_z) {
                    min_z = z;
                }
                if (z > max_z) {
                    max_z = z;
                }
                if (p->x < pmin_x) {
                    pmin_x = p->x;
                }
                if (p->x > pmax_x) {
                    pmax_x = p->x;
                }
                if (p->y < pmin_y) {
                    pmin_y = p->y;
                }
                if (p->y > pmax_y) {
                    pmax_y = p->y;
                }
                if (z < pmin_z) {
                    pmin_z = z;
                }
                if (z > pmax_z) {
                    pmax_z = z;
                }
            }
            polygon->min_x = pmin_x - (f64)0.05f;
            polygon->min_y = pmin_y - (f64)0.05f;
            polygon->min_z = pmin_z - (f64)0.05f;
            polygon->max_x = pmax_x + (f64)0.05f;
            polygon->max_y = pmax_y + (f64)0.05f;
            polygon->max_z = pmax_z + (f64)0.05f;
            if (source->normals[1].y < 65535.0) {
                a.x = polygon->vertices[1].x - polygon->vertices[3].x;
                a.y = polygon->vertices[1].y - polygon->vertices[3].y;
                a.z = polygon->vertices[1].z - polygon->vertices[3].z;
                b.x = polygon->vertices[2].x - polygon->vertices[3].x;
                b.y = polygon->vertices[2].y - polygon->vertices[3].y;
                b.z = polygon->vertices[2].z - polygon->vertices[3].z;
                {
                    NUVEC cross;
                    f32 length;
                    f32 inverse;
                    cross.x = a.y * b.z - a.z * b.y;
                    cross.y = a.z * b.x - a.x * b.z;
                    cross.z = a.x * b.y - a.y * b.x;
                    polygon->normals[1] = cross;
                    length = NuFsqrt(polygon->normals[1].x * polygon->normals[1].x + polygon->normals[1].y * polygon->normals[1].y +
                                     polygon->normals[1].z * polygon->normals[1].z);
                    inverse = length != 0.0f ? 1.0f / length : 0.0f;
                    polygon->normals[1].x = polygon->normals[1].x * inverse;
                    polygon->normals[1].y = polygon->normals[1].y * inverse;
                    polygon->normals[1].z = polygon->normals[1].z * inverse;
                }
            }
            a.x = polygon->vertices[2].x - polygon->vertices[0].x;
            a.y = polygon->vertices[2].y - polygon->vertices[0].y;
            a.z = polygon->vertices[2].z - polygon->vertices[0].z;
            b.x = polygon->vertices[1].x - polygon->vertices[0].x;
            b.y = polygon->vertices[1].y - polygon->vertices[0].y;
            b.z = polygon->vertices[1].z - polygon->vertices[0].z;
            {
                NUVEC cross;
                f32 length;
                f32 inverse;
                cross.x = a.y * b.z - a.z * b.y;
                cross.y = a.z * b.x - a.x * b.z;
                cross.z = a.x * b.y - a.y * b.x;
                polygon->normals[0] = cross;
                length = NuFsqrt(polygon->normals[0].x * polygon->normals[0].x + polygon->normals[0].y * polygon->normals[0].y +
                                 polygon->normals[0].z * polygon->normals[0].z);
                inverse = length != 0.0f ? 1.0f / length : 0.0f;
                polygon->normals[0].x = polygon->normals[0].x * inverse;
                polygon->normals[0].y = polygon->normals[0].y * inverse;
                polygon->normals[0].z = polygon->normals[0].z * inverse;
            }
            ++source;
            ++polygon;
        }
        cursor->header.min_x = min_x - (f64)0.05f;
        cursor->header.max_x = max_x + (f64)0.05f;
        cursor->header.min_z = min_z - (f64)0.05f;
        cursor->header.max_z = max_z + (f64)0.05f;
        input = (TERRAIN_SKIN_STREAM *)source;
        cursor = (TERRAIN_SKIN_STREAM *)polygon;
    }
    cursor->header.kind = -1;
    cursor->header.count = -1;
    group->data = output;
}

NUVEC *TerrainStaticMtx(NUVEC *result, const TERRAIN_SKIN_INFO *info, const NUVEC *position, i32 mode) {
    NUVEC4 point;
    point.x = position->x;
    point.y = position->y;
    point.z = position->z;
    point.w = 1.0f;
    NuVec4MtxTransformVU0(&point, &point, info->static_matrix);
    result->x = point.x;
    result->y = point.y;
    result->z = point.z;
    return result;
}

extern TERRAIN_SKIN_INFO *PlatSkinInfo;
extern TERRAIN_SKIN_USE_RECORD *SkinMemInfo;

/* TerI points at the 0x948-byte TerrainQuery_s query packet.
 * PLATFORM.scene_object is a void*, accessed through NUMTX*;
 * PLATFORM.instance_flags points at the owning instance's flags word.
 */

void TerrainSkinAllocate(TERRAIN_GROUP *group) {
    i32 selected;
    i32 candidate;
    i32 oldest;
    i16 previous_skin;
    i32 original_scene_index = group->scene_index;
    TERRAIN_SKIN_USE_RECORD *records;
    TERRAIN_SKIN_INFO *infos;

    if (-1 - original_scene_index >= PlatSkinCnt) {
        return;
    }

    ++TerrainUpadteCnt;
    if (group->data != NULL) {
        SkinMemInfo[PlatSkinInfo[-1 - original_scene_index].pool_slot].last_use = (i32)TerrainUpadteCnt;
        return;
    }

    records = SkinMemInfo;
    selected = 0;
    if (records[0].skin_index != -1) {
        oldest = records[0].last_use;
        for (candidate = selected + 1; candidate < PlatSkinMaxStore; ++candidate) {
            if (records[candidate].skin_index == -1) {
                selected = candidate;
                break;
            }
            if (records[candidate].last_use < oldest) {
                oldest = records[candidate].last_use;
                selected = candidate;
            }
        }
    }

    previous_skin = records[selected].skin_index;
    infos = PlatSkinInfo;
    if (previous_skin >= 0) {
        CurrentTerrain->groups[infos[previous_skin].group_index].data = NULL;
        for (candidate = 0; candidate < 16; ++candidate) {
            CurrentTerrain->index_levels[candidate].cache_age = 0;
        }
    }

    group->data = infos[-1 - group->scene_index].original_shape;
    infos[-1 - group->scene_index].pool_slot = (i16)selected;
    records[selected].skin_index = -1 - group->scene_index;
    SkinPlatform(group, (TERRAIN_SKIN_STREAM *)(PlatSkinMem + PlatSkinMaxSize * selected), &infos[-(group->scene_index + 1)]);
    SkinMemInfo[selected].last_use = (i32)TerrainUpadteCnt;
}

typedef struct TERRAIN_SCAN_WRITER {
    TERRAIN_SCAN_WORD *group_header;
    TERRAIN_SCAN_WORD *cursor;
    TERRAIN_SCAN_WORD *limit;
    i32 shape_count;
} TERRAIN_SCAN_WRITER;

i32 EShadowRoofInfo(void) {
    return TerrainCastAuxPolygon != NULL ? TerrainCastAuxPolygon->material[1] : -1;
}

void NewScanRot(NUVEC *position, i32 terrain_mask) {
    i32 transformed_count = 0;
    NUVEC4 *vertices;
    NUVEC saved_position;
    TERRAIN_GROUP *groups;
    i32 hit_index;
    i32 oldest_index;
    i32 oldest_age;
    i32 i;
    TERRAIN_INDEX_LEVEL *cache;
    TERRAIN_SCAN_WORD *group_header;
    TERRAIN_SCAN_WORD *cursor;
    TERRAIN_SCAN_WORD *limit;
    i32 shape_count;
    f32 min_x;
    f32 max_x;
    f32 min_z;
    f32 max_z;

    vertices = (NUVEC4 *)(((u32)NuScratchAlloc32(0xd0) + 31) & ~(u32)15);
    saved_position = *position;
    hit_index = -1;
    oldest_index = -1;
    oldest_age = 0x7fffffff;
    for (i = 0; i < 16; ++i) {
        TERRAIN_INDEX_LEVEL *level = &CurrentTerrain->index_levels[i];
        i16 age = level->cache_age;
        if (age > 0) {
            const f64 dx = (saved_position.x + 1.0) - level->center_x;
            if (dx > 0.0 && dx < 2.0) {
                const f64 dz = (saved_position.z + 1.0) - level->center_z;
                if (dz > 0.0 && dz < 2.0) {
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
    groups = CurrentTerrain->groups;
    if (hit_index >= 0) {
        TERRAIN_SCAN_WORD *entry;

        cache = &CurrentTerrain->index_levels[hit_index];
        group_header = TerI->scan_words;
        max_x = saved_position.x + 0.10000000149011612;
        max_z = saved_position.z + 0.10000000149011612;
        min_x = saved_position.x - 0.10000000149011612;
        min_z = saved_position.z - 0.10000000149011612;
        cache->cache_age = 8;
        cursor = group_header + 1;
        limit = cursor + 508;
        shape_count = 0;
        entry = CurrentTerrain->index_levels[hit_index].scan_words;
        while (entry->header.shape_count > 0) {
            const i16 count = entry->header.shape_count;
            const i32 group_index = entry->header.terrain_group;
            TERRAIN_SCAN_WORD *shapes = entry + 1;
            TERRAIN_GROUP *group = &groups[group_index];
            f32 local_min_x;
            f32 local_max_x;
            f32 local_min_z;
            f32 local_max_z;
            i16 remaining;

            if ((max_x >= group->bounds_min.x && max_z >= group->bounds_min.z && group->bounds_max.x >= min_x &&
                 group->bounds_max.z > min_z) &&
                group->chunk_type != -1) {
                local_max_x = max_x - group->origin.x;
                local_max_z = max_z - group->origin.z;
                local_min_x = min_x - group->origin.x;
                local_min_z = min_z - group->origin.z;
                for (remaining = count; remaining > 0; --remaining, ++shapes) {
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
        i32 cell_index;
        TerrainQuery_s *query;

        cache = &CurrentTerrain->index_levels[oldest_index];
        if (cache->cache_age < 2) {
            max_x = saved_position.x + 1.0;
            max_z = saved_position.z + 1.0;
            min_x = saved_position.x - 1.0;
            min_z = saved_position.z - 1.0;
            group_header = cache->scan_words;
        } else {
            max_x = saved_position.x + 0.10000000149011612;
            max_z = saved_position.z + 0.10000000149011612;
            min_x = saved_position.x - 0.10000000149011612;
            min_z = saved_position.z - 0.10000000149011612;
            group_header = TerI->scan_words;
        }
        cursor = group_header + 1;
        limit = cursor + 508;
        shape_count = 0;
        for (cell_index = 0; cell_index < CurrentTerrain->used_cell_count; ++cell_index) {
            const TERRAIN_CELL *cell = &CurrentTerrain->cells[cell_index];
            const i16 *group_indices;
            i32 cell_group;

            if (!(max_x >= cell->min_x && cell->max_x >= min_x && max_z >= cell->min_z && cell->max_z >= min_z)) {
                continue;
            }
            group_indices = CurrentTerrain->group_indices + cell->first_group;
            for (cell_group = 0; cell_group < CurrentTerrain->cells[cell_index].group_count; ++cell_group) {
                TERRAIN_GROUP *group = &groups[group_indices[cell_group]];
                f32 local_min_x;
                f32 local_max_x;
                f32 local_min_z;
                f32 local_max_z;
                TERRAIN_SKIN_STREAM *batch;

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
                batch = group->data;
                while (batch->header.kind >= 0) {
                    TERRAIN_POLYGON_INFO *shape = batch->polygons;
                    const i32 count = batch->header.count;

                    if (local_max_x >= batch->header.min_x && batch->header.max_x > local_min_x && local_max_z >= batch->header.min_z &&
                        batch->header.max_z > local_min_z) {
                        i16 remaining;

                        for (remaining = count; remaining > 0; --remaining, ++shape) {
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
                    batch = (TERRAIN_SKIN_STREAM *)shape;
                }
                if (shape_count != 0) {
                    group_header->header.shape_count = (i16)shape_count;
                    group_header->header.terrain_group = group_indices[cell_group];
                    group_header = cursor;
                    cursor++;
                    shape_count = 0;
                }
            }
        }
        query = TerI;
        if (CurrentTerrain->index_levels[oldest_index].cache_age < 2) {
            TERRAIN_SCAN_WORD *entry;

            CurrentTerrain->index_levels[oldest_index].cache_age = 8;
            CurrentTerrain->index_levels[oldest_index].center_x = position->x;
            CurrentTerrain->index_levels[oldest_index].center_z = position->z;
            group_header->header.shape_count = 0;
            group_header->header.terrain_group = 0;
            group_header = query->scan_words;
            cursor = group_header + 1;
            limit = cursor + 508;
            shape_count = 0;
            max_x = saved_position.x + 0.10000000149011612;
            max_z = saved_position.z + 0.10000000149011612;
            min_x = saved_position.x - 0.10000000149011612;
            min_z = saved_position.z - 0.10000000149011612;
            entry = CurrentTerrain->index_levels[oldest_index].scan_words;
            while (entry->header.shape_count > 0) {
                const i16 count = entry->header.shape_count;
                const i32 group_index = entry->header.terrain_group;
                TERRAIN_SCAN_WORD *shapes = entry + 1;
                TERRAIN_GROUP *group = &groups[group_index];
                f32 local_min_x;
                f32 local_max_x;
                f32 local_min_z;
                f32 local_max_z;
                i16 remaining;

                if ((max_x >= group->bounds_min.x && max_z >= group->bounds_min.z && group->bounds_max.x >= min_x &&
                     group->bounds_max.z > min_z) &&
                    group->chunk_type != -1) {
                    local_max_x = max_x - group->origin.x;
                    local_max_z = max_z - group->origin.z;
                    local_min_x = min_x - group->origin.x;
                    local_min_z = min_z - group->origin.z;
                    for (remaining = count; remaining > 0; --remaining, ++shapes) {
                        TERRAIN_POLYGON_INFO *shape = shapes->shape;
                        if ((local_max_x >= shape->min_x && shape->max_x > local_min_x && local_max_z >= shape->min_z &&
                             shape->max_z > local_min_z) &&
                            TERRAIN_SHAPE_MATERIAL_OK(shape, terrain_mask) && cursor < limit) {
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
    }

    if (NuTerrPlatsOff == 0) {
        const i16 *platform_groups;
        i32 platform_count;

        min_x -= 0.05f;
        max_x += 0.05f;
        min_z -= 0.05f;
        max_z += 0.05f;
        if (min_x > CurrentTerrain->platform_query_min_x && max_x < CurrentTerrain->platform_query_max_x &&
            min_z > CurrentTerrain->platform_query_min_z && max_z < CurrentTerrain->platform_query_max_z) {
            platform_groups = CurrentTerrain->active_platform_groups;
            platform_count = CurrentTerrain->active_platform_count;
        } else {
            platform_groups = CurrentTerrain->group_indices + CurrentTerrain->platform_cell.first_group;
            platform_count = CurrentTerrain->platform_cell.group_count;
        }
        for (; platform_count > 0; --platform_count, ++platform_groups) {
            TERRAIN_GROUP *group = &groups[*platform_groups];
            TERRAIN_PLATFORM *platform;
            f32 local_min_x;
            f32 local_max_x;
            f32 local_min_z;
            f32 local_max_z;
            TERRAIN_SKIN_STREAM *batch;

            if (group->origin.x - group->radius > max_x || group->radius + group->origin.x < min_x ||
                group->origin.z - group->radius > max_z || group->origin.z + group->radius < min_z) {
                continue;
            }
            platform = &TERRAIN_GROUP_PLATFORM(group);
            if (platform->scene_object != NULL) {
                if (TERRAIN_GROUP_MATRIX(group)->m30 > TERRAIN_GROUP_PLATFORM(group).previous_matrix.m30) {
                    local_min_x = min_x - group->origin.x;
                    local_max_x = (TERRAIN_GROUP_MATRIX(group)->m30 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m30) * 1.5 + max_x -
                                  group->origin.x;
                } else {
                    local_max_x = max_x - group->origin.x;
                    local_min_x = (TERRAIN_GROUP_MATRIX(group)->m30 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m30) * 1.5 + min_x -
                                  group->origin.x;
                }
                if (TERRAIN_GROUP_MATRIX(group)->m32 > TERRAIN_GROUP_PLATFORM(group).previous_matrix.m32) {
                    local_min_z = min_z - group->origin.z;
                    local_max_z = (TERRAIN_GROUP_MATRIX(group)->m32 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m32) * 1.5 + max_z -
                                  group->origin.z;
                } else {
                    local_max_z = max_z - group->origin.z;
                    local_min_z = (TERRAIN_GROUP_MATRIX(group)->m32 - TERRAIN_GROUP_PLATFORM(group).previous_matrix.m32) * 1.5 + min_z -
                                  group->origin.z;
                }
            } else {
                local_max_x = max_x - group->origin.x;
                local_min_x = min_x - group->origin.x;
                local_max_z = max_z - group->origin.z;
                local_min_z = min_z - group->origin.z;
            }
            if ((platform->flags & TERRAIN_PLATFORM_FLAG_ROTATING) != 0) {
                f64 dz;
                f64 dx;
                f32 distance_squared;
                f64 reach;

                if (group->chunk_type == -1) {
                    continue;
                }
                dz = (min_z + max_z) * NuQFntHalfScale - group->origin.z;
                dx = (max_x + min_x) * NuQFntHalfScale - group->origin.x;
                reach = group->radius + 9.999999747378752e-6;
                distance_squared = dx * dx + dz * dz;
                if (!(reach * reach > distance_squared)) {
                    continue;
                }
                if (platform->instance_flags != NULL) {
                    const u32 *scene_state = platform->instance_flags;
                    if ((platform->flags & TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED) != 0) {
                        const u32 state_flags = *scene_state;
                        if ((state_flags & 2) == 0) {
                            continue;
                        }
                    } else if ((*scene_state & 1) == 0) {
                        continue;
                    }
                }
                batch = group->data;
                while (batch->header.kind >= 0) {
                    TERRAIN_POLYGON_INFO *shape = batch->polygons;
                    i32 remaining;

                    for (remaining = batch->header.count; remaining > 0; --remaining, ++shape) {
                        TERRAIN_POLYGON_INFO *transformed;
                        f32 length_squared;
                        f32 length;
                        f32 inverse;

                        if (shape->material[1] != 0 && (shape->material[1] & terrain_mask) == 0) {
                            continue;
                        }
                        if (cursor >= limit) {
                            continue;
                        }
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
                        NuVec4MtxTransformVU0x3(vertices, vertices, TERRAIN_GROUP_MATRIX(group));
                        if (shape->normals[1].y < BoltTargetRandomDenominator) {
                            vertices[3].x = shape->vertices[3].x;
                            vertices[3].y = shape->vertices[3].y;
                            vertices[3].z = shape->vertices[3].z;
                            vertices[3].w = 0.0f;
                            NuVec4MtxTransformVU0(&vertices[3], &vertices[3], TERRAIN_GROUP_MATRIX(group));
                        } else {
                            vertices[3] = vertices[2];
                        }
                        if (!(vertices[0].x > local_min_x || vertices[1].x > local_min_x || vertices[2].x > local_min_x ||
                              vertices[3].x > local_min_x)) {
                            continue;
                        }
                        if (!(local_max_x > vertices[0].x || local_max_x > vertices[1].x || local_max_x > vertices[2].x ||
                              local_max_x > vertices[3].x)) {
                            continue;
                        }
                        if (!(vertices[0].z > local_min_z || vertices[1].z > local_min_z || vertices[2].z > local_min_z ||
                              vertices[3].z > local_min_z)) {
                            continue;
                        }
                        if (!(local_max_z > vertices[0].z || local_max_z > vertices[1].z || local_max_z > vertices[2].z ||
                              local_max_z > vertices[3].z)) {
                            continue;
                        }
                        transformed = &ScaleTerrain[transformed_count];
                        transformed->packed_metadata = shape->packed_metadata;
                        transformed->vertices[0].x = vertices[0].x;
                        transformed->vertices[0].y = vertices[0].y;
                        transformed->vertices[0].z = vertices[0].z;
                        transformed->vertices[1].x = vertices[1].x;
                        transformed->vertices[1].y = vertices[1].y;
                        transformed->vertices[1].z = vertices[1].z;
                        transformed->vertices[2].x = vertices[2].x;
                        transformed->vertices[2].y = vertices[2].y;
                        transformed->vertices[2].z = vertices[2].z;
                        if (shape->normals[1].y < BoltTargetRandomDenominator) {
                            transformed->vertices[3].x = vertices[3].x;
                            transformed->vertices[3].y = vertices[3].y;
                            transformed->vertices[3].z = vertices[3].z;
                            vertices[0].x = transformed->vertices[1].x - transformed->vertices[3].x;
                            vertices[0].y = transformed->vertices[1].y - transformed->vertices[3].y;
                            vertices[0].z = transformed->vertices[1].z - transformed->vertices[3].z;
                            vertices[1].x = transformed->vertices[2].x - transformed->vertices[3].x;
                            vertices[1].y = transformed->vertices[2].y - transformed->vertices[3].y;
                            vertices[1].z = transformed->vertices[2].z - transformed->vertices[3].z;
                            {
                                NUVEC cross;
                                cross.x = vertices[0].y * vertices[1].z - vertices[0].z * vertices[1].y;
                                cross.y = vertices[0].z * vertices[1].x - vertices[0].x * vertices[1].z;
                                cross.z = vertices[0].x * vertices[1].y - vertices[0].y * vertices[1].x;
                                transformed->normals[1] = cross;
                            }
                            length_squared = (transformed->normals[1].x * transformed->normals[1].x +
                                              transformed->normals[1].y * transformed->normals[1].y) +
                                             transformed->normals[1].z * transformed->normals[1].z;
                            length = NuFsqrt(length_squared);
                            inverse = length == 0.0f ? 0.0f : 1.0f / length;
                            transformed = &ScaleTerrain[transformed_count];
                            transformed->normals[1].x *= inverse;
                            transformed->normals[1].y *= inverse;
                            transformed->normals[1].z *= inverse;
                        } else {
                            transformed->normals[1].y = 65536.0f;
                        }
                        vertices[0].x = transformed->vertices[2].x - transformed->vertices[0].x;
                        vertices[0].y = transformed->vertices[2].y - transformed->vertices[0].y;
                        vertices[0].z = transformed->vertices[2].z - transformed->vertices[0].z;
                        vertices[1].x = transformed->vertices[1].x - transformed->vertices[0].x;
                        vertices[1].y = transformed->vertices[1].y - transformed->vertices[0].y;
                        vertices[1].z = transformed->vertices[1].z - transformed->vertices[0].z;
                        {
                            NUVEC cross;
                            cross.x = vertices[0].y * vertices[1].z - vertices[0].z * vertices[1].y;
                            cross.y = vertices[0].z * vertices[1].x - vertices[0].x * vertices[1].z;
                            cross.z = vertices[0].x * vertices[1].y - vertices[0].y * vertices[1].x;
                            transformed->normals[0] = cross;
                        }
                        length_squared = (transformed->normals[0].x * transformed->normals[0].x +
                                          transformed->normals[0].y * transformed->normals[0].y) +
                                         transformed->normals[0].z * transformed->normals[0].z;
                        length = NuFsqrt(length_squared);
                        inverse = length == 0.0f ? 0.0f : 1.0f / length;
                        transformed = &ScaleTerrain[transformed_count];
                        transformed->normals[0].x *= inverse;
                        transformed->normals[0].y *= inverse;
                        transformed->normals[0].z *= inverse;
                        (cursor++)->shape = transformed;
                        ++shape_count;
                        ++transformed_count;
                    }
                    batch = (TERRAIN_SKIN_STREAM *)shape;
                }
                if (shape_count != 0) {
                    group_header->header.shape_count = (i16)shape_count;
                    group_header->header.terrain_group = *platform_groups;
                    group_header = cursor;
                    cursor++;
                    shape_count = 0;
                }
            } else {
                if (!(local_max_x >= group->bounds_min.x && local_max_z >= group->bounds_min.z && group->bounds_max.x >= local_min_x &&
                      group->bounds_max.z > local_min_z) ||
                    group->chunk_type == -1) {
                    continue;
                }
                if (platform->instance_flags != NULL) {
                    const u32 *scene_state = platform->instance_flags;
                    if ((platform->flags & TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED) != 0) {
                        const u32 state_flags = *scene_state;
                        if ((state_flags & 2) == 0) {
                            continue;
                        }
                    } else if ((*scene_state & 1) == 0) {
                        continue;
                    }
                }
                batch = group->data;
                while (batch->header.kind >= 0) {
                    TERRAIN_POLYGON_INFO *shape = batch->polygons;
                    const i32 count = batch->header.count;

                    if (local_max_x >= batch->header.min_x && batch->header.max_x > local_min_x && local_max_z >= batch->header.min_z &&
                        batch->header.max_z > local_min_z) {
                        i16 remaining;

                        for (remaining = count; remaining > 0; --remaining, ++shape) {
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
                    batch = (TERRAIN_SKIN_STREAM *)shape;
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
    }
    NuScratchRelease();
    group_header->header.shape_count = 0;
    group_header->header.terrain_group = 0;
}

f32 NewCast(NUVEC *position, f32 height_above, f32 roof_range) {
    TERRAIN_SCAN_WORD *scan;
    TERRAIN_POLYGON_INFO *floor_poly = NULL;
    TERRAIN_POLYGON_INFO *roof_poly = NULL;
    TERRAIN_POLYGON_INFO *efloor_poly = NULL;
    TERRAIN_POLYGON_INFO *eroof_poly = NULL;
    f32 roof_y = 2000000.0f;
    f32 floor_y = -2000000.0f;
    f32 eroof_y = 2000000.0f;
    f32 efloor_y = -2000000.0f;
    i16 roof_group = -1;
    i16 eroof_group = -1;
    NUVEC floor_normal;
    NUVEC efloor_normal;
    NUVEC roof_normal;
    NUVEC eroof_normal;
    TERRAIN_GROUP *groups;

    castnum = -1;
    TerrainShadowFloorIndex = -1;
    scan = TerI->scan_words;
    /* MATCH-EXCEPTION: the original loads the group table once, after the
     * loop's entry test (it hoisted CurrentTerrain->groups itself); the
     * guarded load reproduces that placement. */
    if (scan->header.shape_count > 0) {
        groups = CurrentTerrain->groups;
    }
    while (scan->header.shape_count > 0) {
        i32 group_index = scan->header.terrain_group;
        f32 x = position->x - groups[group_index].origin.x;
        f32 z = position->z - groups[group_index].origin.z;
        TERRAIN_SCAN_WORD *entry = scan + 1;
        i32 i;
        for (i = scan->header.shape_count; i > 0; --i, ++entry) {
            TERRAIN_POLYGON_INFO *surface = entry->shape;
            i32 facing;
            i32 side;
            i32 plane;
            i32 corner;
            f32 quotient;
            f32 y;
            if (surface->normals[1].y > 65535.0) {
                /* Triangle: a single plane. */
                if (surface->normals[0].y > 0.0f) {
                    if (!InsideLineF(x, z, surface->vertices[1].x, surface->vertices[1].z, surface->vertices[0].x,
                                     surface->vertices[0].z) ||
                        !InsideLineF(x, z, surface->vertices[0].x, surface->vertices[0].z, surface->vertices[2].x,
                                     surface->vertices[2].z) ||
                        !InsideLineF(x, z, surface->vertices[2].x, surface->vertices[2].z, surface->vertices[1].x,
                                     surface->vertices[1].z)) {
                        continue;
                    }
                    facing = 1;
                } else if (surface->normals[0].y < 0.0f) {
                    if (!InsideLineF(x, z, surface->vertices[0].x, surface->vertices[0].z, surface->vertices[1].x,
                                     surface->vertices[1].z) ||
                        !InsideLineF(x, z, surface->vertices[2].x, surface->vertices[2].z, surface->vertices[0].x,
                                     surface->vertices[0].z) ||
                        !InsideLineF(x, z, surface->vertices[1].x, surface->vertices[1].z, surface->vertices[2].x,
                                     surface->vertices[2].z)) {
                        continue;
                    }
                    facing = 2;
                } else {
                    continue;
                }
                plane = 0;
                corner = 0;
            } else {
                /* Quad: two planes sharing the 1-2 edge. */
                if (surface->normals[0].y > 0.0f) {
                    side =
                        InsideLineF(x, z, surface->vertices[2].x, surface->vertices[2].z, surface->vertices[1].x, surface->vertices[1].z);
                    if (side != 0) {
                        if (!InsideLineF(x, z, surface->vertices[1].x, surface->vertices[1].z, surface->vertices[0].x,
                                         surface->vertices[0].z) ||
                            !InsideLineF(x, z, surface->vertices[0].x, surface->vertices[0].z, surface->vertices[2].x,
                                         surface->vertices[2].z)) {
                            continue;
                        }
                    } else if (!InsideLineF(x, z, surface->vertices[3].x, surface->vertices[3].z, surface->vertices[1].x,
                                            surface->vertices[1].z) ||
                               !InsideLineF(x, z, surface->vertices[2].x, surface->vertices[2].z, surface->vertices[3].x,
                                            surface->vertices[3].z)) {
                        continue;
                    }
                    facing = 1;
                } else if (surface->normals[0].y < 0.0f) {
                    side =
                        InsideLineF(x, z, surface->vertices[1].x, surface->vertices[1].z, surface->vertices[2].x, surface->vertices[2].z);
                    if (side != 0) {
                        if (!InsideLineF(x, z, surface->vertices[0].x, surface->vertices[0].z, surface->vertices[1].x,
                                         surface->vertices[1].z) ||
                            !InsideLineF(x, z, surface->vertices[2].x, surface->vertices[2].z, surface->vertices[0].x,
                                         surface->vertices[0].z)) {
                            continue;
                        }
                    } else if (!InsideLineF(x, z, surface->vertices[1].x, surface->vertices[1].z, surface->vertices[3].x,
                                            surface->vertices[3].z) ||
                               !InsideLineF(x, z, surface->vertices[3].x, surface->vertices[3].z, surface->vertices[2].x,
                                            surface->vertices[2].z)) {
                        continue;
                    }
                    facing = 2;
                } else {
                    continue;
                }
                if (side == 0 && surface->normals[1].y != 0.0f) {
                    plane = 1;
                    corner = 3;
                } else {
                    plane = 0;
                    corner = 0;
                }
            }
            /* The height of the chosen plane under the point; keep the nearest floor below and roof above. */
            quotient = ((z - surface->vertices[corner].z) * surface->normals[plane].z +
                        (x - surface->vertices[corner].x) * surface->normals[plane].x) /
                       -surface->normals[plane].y;
            y = groups[group_index].origin.y + surface->vertices[corner].y + quotient;
            if (surface->material[1] != 0) {
                if (y <= position->y && y > efloor_y && facing == 1) {
                    efloor_normal = surface->normals[plane];
                    efloor_poly = surface;
                    efloor_y = y;
                    TerrainShadowFloorIndex = group_index;
                } else if (position->y < y && eroof_y >= y && (y != eroof_y || surface->normals[plane].y < eroof_normal.y)) {
                    eroof_normal = surface->normals[plane];
                    eroof_poly = surface;
                    eroof_y = y;
                    eroof_group = group_index;
                }
            } else {
                if (y <= position->y && y > floor_y && facing == 1) {
                    floor_normal = surface->normals[plane];
                    floor_poly = surface;
                    floor_y = y;
                    castnum = group_index;
                } else if (position->y < y && roof_y >= y && (y != roof_y || surface->normals[plane].y < roof_normal.y)) {
                    roof_normal = surface->normals[plane];
                    roof_poly = surface;
                    roof_y = y;
                    roof_group = group_index;
                }
            }
        }
        scan = entry;
    }

    if (eroof_y < 2000000.0 && eroof_normal.y < 0.0f && position->y + roof_range > eroof_y) {
        if (roof_y < 2000000.0 && roof_normal.y < 0.0f && position->y + roof_range > roof_y && roof_y < eroof_y) {
            TerrainCastAuxPolygon = NULL;
            TerrainShadowHeight = 2000000.0f;
        } else {
            TerrainCastAuxValid = 1;
            TerrainShadowHeight = eroof_y;
            TerrainCastAuxPolygon = eroof_poly;
            TerrainCastAuxIndex = eroof_group;
            TerrainCastAuxNormal.x = eroof_normal.x;
            TerrainCastAuxNormal.y = eroof_normal.y;
            TerrainCastAuxNormal.z = eroof_normal.z;
        }
    } else {
        TerrainCastAuxPolygon = NULL;
        TerrainShadowHeight = 2000000.0f;
    }

    if (eroof_y < 2000000.0 && eroof_normal.y > 0.0f && position->y + roof_range > eroof_y) {
        if (roof_y < 2000000.0 && roof_normal.y < 0.0f && position->y + roof_range > roof_y && eroof_y > roof_y) {
            TerrainWaterCastIndex = 3;
            EShadPoly = NULL;
            EShadY = 2000000.0f;
            GetSurfaceInfoWaterNormal.y = 1.0f;
        } else {
            TerrainWaterCastIndex = 1;
            EShadY = eroof_y;
            EShadPoly = eroof_poly;
            TerrainShadowFloorIndex = eroof_group;
            GetSurfaceInfoWaterNormal.x = eroof_normal.x;
            GetSurfaceInfoWaterNormal.y = eroof_normal.y;
            GetSurfaceInfoWaterNormal.z = eroof_normal.z;
        }
    } else if (!(efloor_y > -2000000.0) || (floor_y > -2000000.0 && floor_y > efloor_y)) {
        TerrainWaterCastIndex = 3;
        EShadPoly = NULL;
        EShadY = 2000000.0f;
        GetSurfaceInfoWaterNormal.y = 1.0f;
    } else {
        EShadY = efloor_y;
        TerrainWaterCastIndex = 2;
        GetSurfaceInfoWaterNormal.x = efloor_normal.x;
        EShadPoly = efloor_poly;
        GetSurfaceInfoWaterNormal.y = efloor_normal.y;
        GetSurfaceInfoWaterNormal.z = efloor_normal.z;
    }

    if (roof_y < 2000000.0 && roof_normal.y < 0.0f && position->y + roof_range > roof_y) {
        TerrainRoofValid = 1;
        GetSurfaceInfoRoofHeight = roof_y;
        ShadRoofPoly = roof_poly;
        TerrainRoofIndex = roof_group;
        TerrainRoofNormal.x = roof_normal.x;
        TerrainRoofNormal.y = roof_normal.y;
        TerrainRoofNormal.z = roof_normal.z;
    } else {
        ShadRoofPoly = NULL;
        GetSurfaceInfoRoofHeight = 2000000.0f;
    }

    if (roof_y < 2000000.0 && roof_normal.y > 0.0f && position->y + height_above > roof_y) {
        ShadPoly = roof_poly;
        position->y = roof_y;
        castnum = roof_group;
        ShadNorm.x = roof_normal.x;
        TerrainSurfaceClass = 1;
        ShadNorm.y = roof_normal.y;
        ShadNorm.z = roof_normal.z;
    } else if (floor_y > -2000000.0) {
        ShadNorm.x = floor_normal.x;
        ShadNorm.y = floor_normal.y;
        TerrainSurfaceClass = 2;
        ShadPoly = floor_poly;
        ShadNorm.z = floor_normal.z;
        position->y = floor_y;
    } else {
        position->y = 2000000.0f;
        TerrainSurfaceClass = 3;
        ShadNorm.y = 1.0f;
        ShadPoly = NULL;
    }
    return 0.0f;
}

/* The presentation macros stay local to the kernels above. */

f32 NewShadowEx(NUVEC *position, f32 unused, f32 height_above, f32 roof_range, i32 mask) {
    NUVEC local_position;
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

    if (CurrentTerrain == NULL) {
        return LevelConfigFarDistance;
    }

    TerI = NuScratchAlloc32(0x948);
    ScaleTerrain = ScaleTerrainT1;
    local_position = *position;
    NewScanRot(&local_position, mask);
    (void)NewCast(&local_position, height_above, roof_range);
    NuScratchRelease();
    NuTerrPlatsOff = 0;
    return local_position.y;
}

/* Local existing nuterrain.c access views; no new storage definition. */
void PlatOnOff(i32 platform_index, i32 enabled) {
    TERRAIN_SET *terrain;
    i32 group_index;
    terrain = CurrentTerrain;
    if (terrain == NULL || platform_index < 0 || platform_index >= terrain->max_platforms) {
        return;
    }
    group_index = terrain->platforms[platform_index].terrain_group_index;
    if (enabled != 0) {
        terrain->groups[group_index].chunk_type = 1;
    } else {
        terrain->groups[group_index].chunk_type = -1;
    }
}

void NewTerrPlatformsOff(void) {
    NuTerrPlatsOff = 1;
}

/* Uncalled: the crash data of the terrain file being read. */
u8 *fun_0067bef0(void) {
    return TerrainCrashData;
}

i32 ReadTerrain(unsigned char *base_path, i32 first_group, i16 **buffer, TERRSET *terrain) {
    char path[100];
    TERRAIN_FILE_CHUNK *chunk;
    i32 i;
    i32 group_count;
    i32 first;
    i32 j;
    i32 count;
    TERRAIN_SPATIAL_NODE *node;
    NUVEC *block;
    f32 min_x;
    f32 max_x;
    f32 min_z;
    f32 max_z;

    TerrainCrashData = NULL;
    TerrainDiagnosticNoOpPC(buffer);
    strcpy(path, (char *)base_path);
    strcat(path, ".ter");
    i = NuFileLoadBuffer(path, *buffer, 0x7fffffff);
    TerrainDiagnosticNoOpPC(path, i);
    if (i == 0) {
        return -1;
    }

    chunk = (TERRAIN_FILE_CHUNK *)((u8 *)*buffer + 2 * *(u32 *)*buffer);
    *buffer += 2;
    group_count = 0;
    for (i = 0; i < chunk->chunk_count; ++i) {
        if (chunk[i].scene_index == 0x1ca) {
            TerrainDiagnosticNoOpPC(path);
        }
    }
    terrain->file_version = chunk->version;
    terrain->spatial_nodes = NULL;
    for (i = 0; i < chunk->chunk_count; ++i) {
        switch (chunk[i].chunk_type) {
            case TERRAIN_CHUNK_GROUP_PRIMARY:
            case TERRAIN_CHUNK_GROUP_SECONDARY:
                terrain->groups[first_group].group_info = chunk[i].group_info;
                terrain->groups[first_group].chunk_type = chunk[i].chunk_type;
                terrain->groups[first_group].scene_index = chunk[i].scene_index;
                ++group_count;
                terrain->groups[first_group].origin.x = chunk[i].origin.x;
                terrain->groups[first_group].origin.y = chunk[i].origin.y;
                terrain->groups[first_group].origin.z = chunk[i].origin.z;
                terrain->groups[first_group].data = (TERRAIN_SKIN_STREAM *)*buffer;
                terrain->groups[first_group].platform_flags = chunk[i].platform_flags;
                ++first_group;
                break;
            case TERRAIN_CHUNK_SPATIAL_INDEX:
                if (*(u32 *)*buffer == 0x12345678) {
                    *(TERRAIN_SPATIAL_NODE **)*buffer = terrain->spatial_nodes;
                    terrain->spatial_nodes = (TERRAIN_SPATIAL_NODE *)(*buffer + 2);
                    --terrain->spatial_nodes->point_count;
                }
                break;
            case TERRAIN_CHUNK_CRASH_DATA:
                TerrainCrashData = (u8 *)*buffer;
                break;
        }
        *buffer += chunk[i].data_word_count;
    }

    // Every block covers sixteen terrain cells and therefore seventeen
    // boundary points. The original stores the aggregate X/Z bounds in the
    // otherwise-unused Y components of the first four points.
    for (node = terrain->spatial_nodes; node != NULL;
         node = ((TERRAIN_SPATIAL_CHUNK *)((char *)node - offsetof(TERRAIN_SPATIAL_CHUNK, node)))->previous) {
        for (first = 0; first < node->point_count; first += 16) {
            block = &node->points[first];
            if (block[0].y == 2147483648.0f) {
                continue;
            }
            if (first + 15 < node->point_count) {
                min_x = max_x = block[0].x;
                min_z = max_z = block[0].z;
                for (j = first + 1; j <= first + 16; ++j) {
                    if (min_x > node->points[j].x) {
                        min_x = node->points[j].x;
                    }
                    if (node->points[j].x > max_x) {
                        max_x = node->points[j].x;
                    }
                    if (min_z > node->points[j].z) {
                        min_z = node->points[j].z;
                    }
                    if (node->points[j].z > max_z) {
                        max_z = node->points[j].z;
                    }
                }
                block[0].y = min_x;
                block[1].y = max_x;
                block[2].y = min_z;
                block[3].y = max_z;
            } else {
                block[0].y = 2147483648.0f;
            }
        }
    }
    return group_count;
}

void TerrFlush(void) {
    TerrShapeAdjCnt = 0;
}

/* Normals exported as NaN are replaced by straight up. */
void NuVecCheckForSNANs(NUVEC *v) {
    if (*(u32 *)&v->x == NU_FLOAT_NAN_BITS || *(u32 *)&v->y == NU_FLOAT_NAN_BITS || *(u32 *)&v->z == NU_FLOAT_NAN_BITS) {
        v->x = 0.0f;
        v->y = 1.0f;
        v->z = 0.0f;
    }
}

i32 curPickInst = 0;
i32 PlatImpactId = 0;
i16 castnum = 0;
i32 terraincnt = 0;
NUVEC GetSurfaceInfoWaterNormal = {0};
// Accessed terrain state and literals; full original allocation extents remain unknown.
struct TERRAIN_SKIN_USE_RECORD *SkinMemInfo = 0;
void *ScaleTerrainT2 = 0;

void *TerrainInitEx(i32 level_num, void *buf, void *buf_end, i32 options, char *path, void *gscn, i32 scene_id, u32 max_group_indices,
                    u32 max_groups, u32 max_platforms) {
    i32 level;
    i32 group_index;
    i32 reset_cell;
    i32 platform_index;
    i32 slot;
    i32 shape_index;
    i32 vector_index;
    i32 cell_index;
    i32 object_index;
    VARIPTR *cursor;
    VARIPTR saved_cursor;
    TERRAIN_GSCENE *scene;
    usize scratch;
    i16 *group_cells;
    f32 *group_min_x;
    f32 *group_max_x;
    f32 *group_min_z;
    f32 *group_max_z;
    TERRSET *terrain;
    TERRAIN_PLATFORM *platform;
    i16 group_count;
    f32 kBoundsMinimum;
    f32 kBoundsMaximum;
    f32 kSteepNormalY;
    TERRAIN_GROUP *group;
    f32 min_x;
    f32 min_y;
    f32 min_z;
    f32 max_x;
    f32 max_y;
    f32 max_z;
    f32 radius_squared;
    TERRAIN_SKIN_STREAM *batch;
    TERRAIN_POLYGON_INFO *shape;
    u8 legacy_material;
    f32 length_squared;
    f32 terrain_min_x;
    f32 terrain_min_z;
    f32 terrain_max_x;
    f32 terrain_max_z;
    i32 x_distance;
    i32 x_cell;
    i32 z_distance;
    i32 z_cell;
    TERRAIN_CELL *cell;
    TERRAIN_CELL *platform_cell;
    i32 platform_count;
    u16 scene_object_index;
    TERRAIN_DISPLAY_SCENE *display_scene;
    TERRAIN_DISPLAY_OBJECT *display_object;
    TERRAIN_DISPLAY_OBJECT *candidate;
    TERRAIN_SCENE_OBJECT *scene_object;

    cursor = buf;
    saved_cursor = *cursor;
    scene = gscn;

    scratch = ((usize)(buf_end)) & ~((usize)(1));
    scratch -= max_groups * sizeof(i16);
    group_cells = ((i16 *)(scratch));

    scratch &= ~((usize)(3));
    scratch -= max_groups * sizeof(f32);
    group_min_x = ((f32 *)(scratch));
    scratch -= max_groups * sizeof(f32);
    group_max_x = ((f32 *)(scratch));
    scratch -= max_groups * sizeof(f32);
    group_min_z = ((f32 *)(scratch));
    scratch -= max_groups * sizeof(f32);
    group_max_z = ((f32 *)(scratch));

    if (ScaleTerrainT1 == NULL) {
        ScaleTerrainT1 = NuAlloc(0xc800, "d:\\projects\\game_saga\\gameapi.saga\\gamelib\\terrain.c", 466);
    }
    if (ScaleTerrainT2 == NULL) {
        ScaleTerrainT2 = NuAlloc(0xc800, "d:\\projects\\game_saga\\gameapi.saga\\gamelib\\terrain.c", 468);
    }
    if (TempScanStack == NULL) {
        TempScanStack = NuAlloc(0x2000, "d:\\projects\\game_saga\\gameapi.saga\\gamelib\\terrain.c", 470);
    }
    if (WallSplList == NULL) {
        WallSplList = NuAlloc(0x600, "d:\\projects\\game_saga\\gameapi.saga\\gamelib\\terrain.c", 472);
    }

    terrain = ((TERRSET *)(cursor->char_ptr));
    cursor->char_ptr += sizeof(TERRSET);
    terrain->max_platforms = max_platforms;
    terrain->max_group_indices = max_group_indices;
    terrain->max_groups = max_groups;

    terrain->groups = ((TERRAIN_GROUP *)(cursor->char_ptr));
    cursor->char_ptr += (max_groups + 0x80) * sizeof(TERRAIN_GROUP) / 2 * 2;
    terrain->group_indices = ((i16 *)(cursor->char_ptr));
    cursor->char_ptr += max_group_indices * sizeof(i16) / 2 * 2;
    terrain->platforms = ((TERRAIN_PLATFORM *)(cursor->char_ptr));
    cursor->char_ptr += terrain->max_platforms * sizeof(TERRAIN_PLATFORM) / 2 * 2;

    for (level = 0; level < TERRAIN_INDEX_LEVEL_COUNT; ++level) {
        terrain->index_levels[level].cache_age = 0;
        terrain->index_levels[level].scan_words[0].shape = NULL;
    }

    memset(terrain->groups, 0, terrain->max_groups * sizeof(TERRAIN_GROUP));
    for (group_index = 0; group_index < terrain->max_groups; ++group_index) {
        terrain->groups[group_index].chunk_type = -1;
    }

    /* The platform cell directly follows the 256 grid cells. */
    cell = terrain->cells;
    for (reset_cell = 0; reset_cell < TERRAIN_CELL_RECORD_COUNT; ++reset_cell, ++cell) {
        cell->group_count = 0;
    }

    for (platform_index = 0; platform_index < terrain->max_platforms; ++platform_index) {
        terrain->platforms[platform_index].scene_object = NULL;
        terrain->platforms[platform_index].motion_status = 0;
        terrain->platforms[platform_index].bounce_impulse = 0;
        terrain->platforms[platform_index].bounce_offset = 0;
        terrain->platforms[platform_index].bounce_velocity = 0;
        terrain->platforms[platform_index].bounce_damping = 0;
        terrain->platforms[platform_index].bounce_spring = 0;
        terrain->platforms[platform_index].flags &= ~TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED;
    }

    for (slot = 0; slot < TERRAIN_TRACK_SLOT_COUNT; ++slot) {
        terrain->track_slots[slot].id = NULL;
        terrain->track_slots[slot].wall_contact_state = TERRAIN_TRACK_CONTACT_NONE;
    }

    terrain->user_data = NULL;
    terrain->active_platform_count = 0;
    terrain->removed_platform_count = 0;

    terraincnt = 0;
    curSphereter = 0;
    platinrange = 0;
    ShadPoly = 0;
    TerrPoly = 0;
    TerrWallInfo = 0;
    PlatImpactId = -1;

    group_count = ((i16)(ReadTerrain(((unsigned char *)(path)), 0, ((i16 **)(cursor)), terrain)));
    terrain->group_count = group_count;
    if (group_count < 0) {
        *cursor = saved_cursor;
        return NULL;
    }

    kBoundsMinimum = -200000000.0f;
    kBoundsMaximum = 200000000.0f;
    kSteepNormalY = 0.707f;

    terrain->minimum_height = INT_MAX;

    group = terrain->groups;
    for (group_index = 0; group_index < terrain->group_count; ++group_index, ++group) {
        if (group->chunk_type != TERRAIN_CHUNK_GROUP_PRIMARY && group->chunk_type != TERRAIN_CHUNK_GROUP_SECONDARY) {
            continue;
        }

        min_x = kBoundsMaximum;
        min_y = kBoundsMaximum;
        min_z = kBoundsMaximum;
        max_x = kBoundsMinimum;
        max_y = kBoundsMinimum;
        max_z = kBoundsMinimum;
        radius_squared = 0.0f;

        batch = group->data;
        while (batch->header.kind >= 0) {
            shape = batch->polygons;
            for (shape_index = 0; shape_index < batch->header.count; ++shape_index, ++shape) {
                if (*(u32 *)&shape->normals[0].x == NU_FLOAT_NAN_BITS || *(u32 *)&shape->normals[0].y == NU_FLOAT_NAN_BITS ||
                    *(u32 *)&shape->normals[0].z == NU_FLOAT_NAN_BITS) {
                    shape->normals[0].x = 0;
                    shape->normals[0].z = 0;
                    shape->normals[0].y = 1;
                }
                if (*(u32 *)&shape->normals[1].x == NU_FLOAT_NAN_BITS || *(u32 *)&shape->normals[1].y == NU_FLOAT_NAN_BITS ||
                    *(u32 *)&shape->normals[1].z == NU_FLOAT_NAN_BITS) {
                    shape->normals[1].x = 0;
                    shape->normals[1].z = 0;
                    shape->normals[1].y = 1;
                }

                if (shape->material[1] > 0xef) {
                    legacy_material = shape->material[1];
                    shape->material[1] = 0;
                    shape->normal_flags |= ((u8)(legacy_material * 4 + 0x44));
                }

                if (shape->material[0] == 0xff) {
                    shape->flags |= TERRAIN_SHAPE_FLAG_MATERIAL_ALIAS;
                    shape->material[0] = shape->material[1];
                    shape->material[1] = 0;
                } else if (((i8)(shape->material[0])) < 0) {
                    shape->material[0] &= 0x7f;
                    shape->flags |= TERRAIN_SHAPE_FLAG_MATERIAL_HIGH_BIT;
                }

                if (terrain->file_version == 0) {
                    shape->normal_flags &= ((u8)(~(TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP | TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP)));
                    if (kSteepNormalY >= shape->normals[0].y) {
                        shape->normal_flags |= TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP;
                    }
                    if (kSteepNormalY >= shape->normals[1].y) {
                        shape->normal_flags |= TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP;
                    }
                }

                if (shape->min_x < min_x) {
                    min_x = shape->min_x;
                }
                if (shape->max_x > max_x) {
                    max_x = shape->max_x;
                }
                if (shape->min_y < min_y) {
                    min_y = shape->min_y;
                }
                if (shape->max_y > max_y) {
                    max_y = shape->max_y;
                }
                if (min_z > shape->min_z) {
                    min_z = shape->min_z;
                }
                if (shape->max_z > max_z) {
                    max_z = shape->max_z;
                }

                if (terrain->minimum_height > shape->vertices[0].y + group->origin.y) {
                    terrain->minimum_height = (i32)(shape->vertices[0].y + group->origin.y);
                }
                if (terrain->minimum_height > shape->vertices[1].y + group->origin.y) {
                    terrain->minimum_height = (i32)(shape->vertices[1].y + group->origin.y);
                }
                if (terrain->minimum_height > shape->vertices[2].y + group->origin.y) {
                    terrain->minimum_height = (i32)(shape->vertices[2].y + group->origin.y);
                }
                if (terrain->minimum_height > shape->vertices[3].y + group->origin.y) {
                    terrain->minimum_height = (i32)(shape->vertices[3].y + group->origin.y);
                }
                length_squared = shape->vertices[0].x * shape->vertices[0].x + shape->vertices[0].y * shape->vertices[0].y +
                                 shape->vertices[0].z * shape->vertices[0].z;
                if (length_squared > radius_squared) {
                    radius_squared = length_squared;
                }
                length_squared = shape->vertices[1].x * shape->vertices[1].x + shape->vertices[1].y * shape->vertices[1].y +
                                 shape->vertices[1].z * shape->vertices[1].z;
                if (length_squared > radius_squared) {
                    radius_squared = length_squared;
                }
                length_squared = shape->vertices[2].x * shape->vertices[2].x + shape->vertices[2].y * shape->vertices[2].y +
                                 shape->vertices[2].z * shape->vertices[2].z;
                if (length_squared > radius_squared) {
                    radius_squared = length_squared;
                }
                length_squared = shape->vertices[3].x * shape->vertices[3].x + shape->vertices[3].y * shape->vertices[3].y +
                                 shape->vertices[3].z * shape->vertices[3].z;
                if (length_squared > radius_squared) {
                    radius_squared = length_squared;
                }
            }

            batch = ((TERRAIN_SKIN_STREAM *)(batch->polygons + batch->header.count));
        }

        group->radius = NuFsqrt(radius_squared);
        if (group->chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY) {
            group->bounds_min.x = min_x;
            group->bounds_min.y = min_y;
            group->bounds_min.z = min_z;
            group->bounds_max.x = max_x;
            group->bounds_max.y = max_y;
            group->bounds_max.z = max_z;
        } else {
            group->bounds_min.x = group->origin.x + min_x;
            group->bounds_min.y = group->origin.y + min_y;
            group->bounds_min.z = group->origin.z + min_z;
            group->bounds_max.x = group->origin.x + max_x;
            group->bounds_max.y = group->origin.y + max_y;
            group->bounds_max.z = group->origin.z + max_z;
        }
        ++terraincnt;
    }

    terrain_min_x = kBoundsMaximum;
    terrain_min_z = kBoundsMaximum;
    terrain_max_z = kBoundsMinimum;
    terrain_max_x = kBoundsMinimum;

    for (group_index = 0; group_index < terrain->group_count; ++group_index) {
        group = &terrain->groups[group_index];
        if (group->chunk_type != TERRAIN_CHUNK_GROUP_PRIMARY) {
            continue;
        }

        batch = group->data;
        while (0 <= batch->header.kind) {
            if (batch->header.min_x + group->origin.x < terrain_min_x) {
                terrain_min_x = batch->header.min_x + group->origin.x;
            }
            if (batch->header.max_x + group->origin.x > terrain_max_x) {
                terrain_max_x = batch->header.max_x + group->origin.x;
            }
            if (batch->header.min_z + group->origin.z < terrain_min_z) {
                terrain_min_z = batch->header.min_z + group->origin.z;
            }
            if (batch->header.max_z + group->origin.z > terrain_max_z) {
                terrain_max_z = batch->header.max_z + group->origin.z;
            }

            batch = ((TERRAIN_SKIN_STREAM *)(batch->polygons + batch->header.count));
        }
    }

    terrain->group_index_count = 0;
    for (group_index = 0; group_index < terrain->group_count; ++group_index) {
        group = &terrain->groups[group_index];
        if (group->chunk_type != TERRAIN_CHUNK_GROUP_PRIMARY) {
            continue;
        }

        group_min_x[group_index] = kBoundsMaximum;
        group_min_z[group_index] = kBoundsMaximum;
        group_max_x[group_index] = kBoundsMinimum;
        group_max_z[group_index] = kBoundsMinimum;
        batch = group->data;
        while (batch->header.kind >= 0) {
            if (batch->header.min_x + group->origin.x < group_min_x[group_index]) {
                group_min_x[group_index] = batch->header.min_x + group->origin.x;
            }
            if (batch->header.max_x + group->origin.x > group_max_x[group_index]) {
                group_max_x[group_index] = batch->header.max_x + group->origin.x;
            }
            if (batch->header.min_z + group->origin.z < group_min_z[group_index]) {
                group_min_z[group_index] = batch->header.min_z + group->origin.z;
            }
            if (batch->header.max_z + group->origin.z > group_max_z[group_index]) {
                group_max_z[group_index] = batch->header.max_z + group->origin.z;
            }

            batch = ((TERRAIN_SKIN_STREAM *)(batch->polygons + batch->header.count));
        }

        x_distance = (i32)((group_max_x[group_index] + group_min_x[group_index]) * 0.5f - terrain_min_x);
        x_cell = (i32)((f32)(x_distance * TERRAIN_GRID_WIDTH) / (terrain_max_x - terrain_min_x));
        if (x_cell < 0) {
            x_cell = 0;
        } else if (x_cell > TERRAIN_GRID_WIDTH - 1) {
            x_cell = TERRAIN_GRID_WIDTH - 1;
        }

        z_distance = (i32)((group_max_z[group_index] + group_min_z[group_index]) * 0.5f - terrain_min_z);
        z_cell = (i32)((f32)(z_distance * TERRAIN_GRID_WIDTH) / (terrain_max_z - terrain_min_z));
        if (z_cell < 0) {
            z_cell = 0;
        } else if (z_cell > TERRAIN_GRID_WIDTH - 1) {
            z_cell = TERRAIN_GRID_WIDTH - 1;
        }

        group_cells[group_index] = ((i16)(z_cell * TERRAIN_GRID_WIDTH + x_cell));
    }

    terrain->used_cell_count = 0;
    for (cell_index = 0; cell_index < TERRAIN_GRID_CELL_COUNT; ++cell_index) {
        terrain->cells[terrain->used_cell_count].first_group = ((i16)(terrain->group_index_count));
        min_x = kBoundsMaximum;
        min_z = kBoundsMaximum;
        max_x = kBoundsMinimum;
        max_z = kBoundsMinimum;
        group = terrain->groups;
        for (group_index = 0; group_index < terrain->group_count; ++group_index, ++group) {
            if (group_cells[group_index] != cell_index || group->chunk_type != TERRAIN_CHUNK_GROUP_PRIMARY) {
                continue;
            }

            if (group_min_x[group_index] < min_x) {
                min_x = group_min_x[group_index];
            }
            if (group_min_z[group_index] < min_z) {
                min_z = group_min_z[group_index];
            }
            if (group_max_x[group_index] > max_x) {
                max_x = group_max_x[group_index];
            }
            if (group_max_z[group_index] > max_z) {
                max_z = group_max_z[group_index];
            }

            ++terrain->cells[terrain->used_cell_count].group_count;
            terrain->group_indices[terrain->group_index_count] = ((i16)(group_index));
            ++terrain->group_index_count;
        }

        if (terrain->cells[terrain->used_cell_count].group_count != 0) {
            terrain->cells[terrain->used_cell_count].min_x = min_x;
            terrain->cells[terrain->used_cell_count].min_z = min_z;
            terrain->cells[terrain->used_cell_count].max_x = max_x;
            terrain->cells[terrain->used_cell_count].max_z = max_z;
            ++terrain->used_cell_count;
        }
    }

    platform_cell = &terrain->platform_cell;
    platform_cell->first_group = ((u16)(terrain->group_index_count));

    platform_count = 0;
    for (group_index = 0; group_index < terrain->group_count; ++group_index) {
        group = &terrain->groups[group_index];
        if (group->chunk_type != TERRAIN_CHUNK_GROUP_SECONDARY) {
            continue;
        }
        if (platform_count >= terrain->max_platforms) {
            goto bad_platform;
        }

        debug_index = group->scene_index;
        if (scene != NULL && scene->object_map_present != NULL) {
            debug_index = scene->object_map[(u16)debug_index];
        }
        scene_object_index = (u16)debug_index;
        group->scene_index = ((u16)(platform_count));
        if (scene == NULL) {
            goto bad_platform;
        }

        display_scene = scene->display_scene;
        if (display_scene != NULL) {
            terrain->group_indices[terrain->group_index_count] = ((i16)(group_index));
            ++terrain->group_index_count;
            ++platform_cell->group_count;

            platform = &terrain->platforms[group->scene_index];
            platform->flags = (platform->flags & ~TERRAIN_PLATFORM_FLAG_ROTATING) | (group->platform_flags & 1);
            platform->scene_object_index = scene_object_index;
            platform->terrain_group_index = ((i16)(group_index));
            platform->flags &= ~TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED;

            display_object = NULL;
            for (object_index = 0; object_index < display_scene->object_count; ++object_index) {
                candidate = &display_scene->objects[object_index];
                if (candidate->scene_object_index == scene_object_index) {
                    display_object = candidate;
                    break;
                }
            }
            if (display_object == NULL) {
                goto bad_platform;
            }

            if (display_object->object == NULL || display_object->object == ((void *)(-1))) {
                platform->scene_object = (NUMTX *)display_object->embedded_object;
            } else {
                platform->scene_object = display_object->object;
            }
            platform->instance_flags = &display_object->flags;
            platform->flags |= TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED;
        } else {
            if (scene_object_index >= scene->object_count) {
                goto bad_platform;
            }
            terrain->group_indices[terrain->group_index_count] = ((i16)(group_index));
            ++terrain->group_index_count;
            ++platform_cell->group_count;

            platform = &terrain->platforms[group->scene_index];
            platform->flags = (platform->flags & ~TERRAIN_PLATFORM_FLAG_ROTATING) | (group->platform_flags & 1);
            platform->scene_object_index = scene_object_index;
            platform->terrain_group_index = ((i16)(group_index));
            platform->flags &= ~TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED;

            scene_object = &scene->objects[scene_object_index];
            platform->scene_object = scene_object->object != NULL ? scene_object->object : scene_object;
            platform->instance_flags = &scene_object->flags;
        }
        ++platform_count;
        continue;

    bad_platform:
        group->chunk_type = -1;
        TerrainDiagnosticNoOpPC(path);
    }

    TerrShapeAdjCnt = 0;
    return saved_cursor.void_ptr;
}

void TerrainPlatGetMtx(i32 index, NUMTX **previous, NUMTX **current) {
    if (index > -1) {
        *previous = &CurrentTerrain->platforms[index].previous_matrix;
        *current = CurrentTerrain->platforms[index].scene_object;
    }
}

void TerrainSetImpactData(void *records, i32 *count, i32 max_records) {
    TerImpactData = records;
    TerImpactDataCount = count;
    *count = 0;
    TerImpactDataMax = max_records;
}

void TerrainSetPlatConnectTol(f32 tolerance) {
    TerConTol = tolerance;
}

void noterraininit(void) {
    terraincnt = 0;
    curSphereter = 0;
    platinrange = 0;
    ShadPoly = NULL;
    TerI = NuScratchAlloc32(0x948);
    NuScratchRelease();
    CurrentTerrain = NULL;
    TerrShapeAdjCnt = 0;
}

void TerrSetPlatScanDist(f32 distance) {
    TerrPlatScanDist = distance;
}

void TerrainSetWallDeflectYScale(f32 scale) {
    TerrWallDeflectYScale = scale;
}

void TerrainPlatformOldUpdate(void) {
    i32 i;
    if (CurrentTerrain == NULL) {
        return;
    }
    curPickInst = 0;
    for (i = 0; i < CurrentTerrain->removed_platform_count; ++i) {
        DeletePlatinst(CurrentTerrain->removed_platforms[i]);
    }
    CurrentTerrain->removed_platform_count = 0;
    curSphereter = 0;
    for (i = 0; i < CurrentTerrain->max_platforms; ++i) {
        if (CurrentTerrain->platforms[i].scene_object != NULL) {
            CurrentTerrain->platforms[i].previous_matrix = *CurrentTerrain->platforms[i].scene_object;
        }
    }
}

void TerrainPlatformNewUpdate(void) {
    i32 i;
    f32 negative_distance;
    f32 x;
    f32 z;
    f32 offset;
    u32 offset_bits;
    f64 back_x;
    f64 back_z;
    f64 product_x;
    f64 product_z;
    f32 min_x;
    f32 min_z;
    f32 max_x;
    f32 max_z;
    i16 *group_index;
    if (CurrentTerrain != NULL) {
        for (i = 0; i < 64; ++i) {
            if (CurrentTerrain->track_slots[i].id != NULL) {
                if (CurrentTerrain->track_slots[i].platform_contact_state > 0) {
                    --CurrentTerrain->track_slots[i].platform_contact_state;
                }
                if (CurrentTerrain->track_slots[i].wall_contact_state > 0) {
                    --CurrentTerrain->track_slots[i].wall_contact_state;
                }
            }
        }
        for (i = 0; i < 16; ++i) {
            if (CurrentTerrain->index_levels[i].cache_age > 0) {
                --CurrentTerrain->index_levels[i].cache_age;
            }
        }
    }
    for (i = 0; i < PlatCodeCallback; ++i) {
        PlatCallback[i].function(PlatCallback[i].argument);
    }
    PlatCodeCallback = 0;
    cntrots = 0;
    if (CurrentTerrain == NULL) {
        return;
    }
    negative_distance = -TerrPlatScanDist;
    back_x = global_camera.mtx.m20 * TerrPlatScanDist * 0.25;
    x = global_camera.mtx.m00 * negative_distance * 0.25 - back_x;
    min_x = x;
    max_x = x;
    back_z = global_camera.mtx.m22 * TerrPlatScanDist * 0.25;
    z = global_camera.mtx.m02 * negative_distance * 0.25 - back_z;
    min_z = z;
    max_z = z;
    product_x = global_camera.mtx.m00 * TerrPlatScanDist;
    x = global_camera.mtx.m20 * TerrPlatScanDist - product_x * (f64)0.35f;
    if (min_x > x) {
        min_x = x;
    }
    if (max_x < x) {
        max_x = x;
    }
    product_z = global_camera.mtx.m02 * TerrPlatScanDist;
    z = global_camera.mtx.m22 * TerrPlatScanDist - product_z * (f64)0.35f;
    if (min_z > z) {
        min_z = z;
    }
    if (max_z < z) {
        max_z = z;
    }
    x = global_camera.mtx.m20 * TerrPlatScanDist + product_x * (f64)0.35f;
    if (min_x > x) {
        min_x = x;
    }
    if (max_x < x) {
        max_x = x;
    }
    z = global_camera.mtx.m22 * TerrPlatScanDist + product_z * (f64)0.35f;
    if (min_z > z) {
        min_z = z;
    }
    if (max_z < z) {
        max_z = z;
    }
    x = product_x * 0.25 - back_x;
    if (min_x > x) {
        min_x = x;
    }
    if (max_x < x) {
        max_x = x;
    }
    z = product_z * 0.25 - back_z;
    if (min_z > z) {
        min_z = z;
    }
    if (max_z < z) {
        max_z = z;
    }
    min_x += global_camera.mtx.m30 - 1.0f;
    max_x += global_camera.mtx.m30 + 1.0f;
    min_z += global_camera.mtx.m32 - 1.0f;
    max_z += global_camera.mtx.m32 + 1.0f;
    CurrentTerrain->platform_query_min_x = min_x;
    CurrentTerrain->platform_query_max_x = max_x;
    CurrentTerrain->platform_query_min_z = min_z;
    CurrentTerrain->platform_query_max_z = max_z;
    CurrentTerrain->active_platform_count = 0;
    group_index = &CurrentTerrain->group_indices[CurrentTerrain->platform_cell.first_group];
    for (i = 0; i < CurrentTerrain->platform_cell.group_count; ++i, ++group_index) {
        TERRAIN_GROUP *group = &CurrentTerrain->groups[*group_index];
        i32 index = group->scene_index;
        TERRAIN_PLATFORM *platform = &CurrentTerrain->platforms[index];
        if (platform->instance_flags != NULL) {
            if ((platform->flags & TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED) != 0) {
                if ((*platform->instance_flags & 2) == 0) {
                    continue;
                }
            } else if ((*platform->instance_flags & 1) == 0) {
                continue;
            }
        }
        if (group->chunk_type == 0) {
            continue;
        }
        if (platform->scene_object != NULL) {
            if (platform->bounce_frames != 0) {
                if (platform->bounce_frames > 124) {
                    CurrentTerrain->platforms[index].bounce_velocity =
                        (CurrentTerrain->platforms[index].bounce_impulse -
                         CurrentTerrain->platforms[index].bounce_damping * CurrentTerrain->platforms[index].bounce_velocity) +
                        CurrentTerrain->platforms[index].bounce_velocity;
                } else {
                    CurrentTerrain->platforms[index].bounce_velocity =
                        CurrentTerrain->platforms[index].bounce_velocity -
                        CurrentTerrain->platforms[index].bounce_damping * CurrentTerrain->platforms[index].bounce_velocity;
                }
                offset = CurrentTerrain->platforms[index].bounce_offset;
                offset_bits = *(u32 *)&offset & 0x7fffffff;
                CurrentTerrain->platforms[index].bounce_velocity =
                    (-CurrentTerrain->platforms[index].bounce_offset * *(f32 *)&offset_bits *
                         CurrentTerrain->platforms[index].bounce_spring * 0.5 -
                     CurrentTerrain->platforms[index].bounce_offset * CurrentTerrain->platforms[index].bounce_spring) +
                    CurrentTerrain->platforms[index].bounce_velocity;
                CurrentTerrain->platforms[index].bounce_offset += CurrentTerrain->platforms[index].bounce_velocity;
                --CurrentTerrain->platforms[index].bounce_frames;
                CurrentTerrain->platforms[index].scene_object->m31 += CurrentTerrain->platforms[index].bounce_offset;
            }
            group->origin.x = CurrentTerrain->platforms[index].scene_object->m30;
            group->origin.y = CurrentTerrain->platforms[index].scene_object->m31;
            group->origin.z = CurrentTerrain->platforms[index].scene_object->m32;
        }
        if ((CurrentTerrain->platforms[index].flags & 2) != 0) {
            CurrentTerrain->platforms[index].bounce_frames = 128;
        }
        CurrentTerrain->platforms[index].flags &= ~2U;
        CurrentTerrain->platforms[index].motion_status = 0;
        if ((CurrentTerrain->platforms[index].flags & 1) != 0) {
            if (group->origin.x - group->radius > max_x || group->radius + group->origin.x < min_x ||
                group->origin.z - group->radius > max_z) {
                continue;
            }
            if (group->origin.z + group->radius < min_z) {
                continue;
            }
        } else {
            if (group->bounds_min.x + group->origin.x > max_x || group->bounds_max.x + group->origin.x < min_x ||
                group->bounds_min.z + group->origin.z > max_z || group->bounds_max.z + group->origin.z < min_z) {
                continue;
            }
        }
        if (CurrentTerrain->active_platform_count < 96) {
            CurrentTerrain->active_platform_groups[CurrentTerrain->active_platform_count] = *group_index;
            ++CurrentTerrain->active_platform_count;
        }
    }
}

NUVEC TerCrossProduct(NUVEC *a, NUVEC *b) {
    NUVEC result;
    result.x = a->y * b->z - a->z * b->y;
    result.y = a->z * b->x - b->z * a->x;
    result.z = a->x * b->y - b->x * a->y;
    return result;
}

i32 FindPlatInst(i32 instance) {
    TERRAIN_SET *terrain;
    i32 index;
    i32 result = -1;
    if (instance != -1) {
        terrain = CurrentTerrain;
        for (index = 0; index < terrain->max_platforms; ++index) {
            if (terrain->platforms[index].scene_object != NULL && terrain->platforms[index].scene_object_index == instance) {
                result = index;
                break;
            }
        }
    }
    return result;
}

void PlatInstRotate(i32 platform_index, i32 enabled) {
    TERRAIN_SET *terrain;
    if (platform_index >= 0) {
        terrain = CurrentTerrain;
        if (platform_index < terrain->max_platforms) {
            terrain->platforms[platform_index].flags =
                (terrain->platforms[platform_index].flags & ~TERRAIN_PLATFORM_FLAG_ROTATING) | (enabled & 1);
        }
    }
}

typedef struct TERRAIN_PICKUP_SET {      // size 0xa490
    TERRAIN_GROUP *groups;               // 0x0000
    void *shape_data;                    // 0x0004
    i32 group_count;                     // 0x0008
    i16 group_for_type[128];             // 0x000c
    TERRAIN_PLATFORM platforms[32];      // 0x010c
    TERRAIN_SPATIAL_NODE *spatial_nodes; // 0x0e8c
    u8 unknown_e90[0xa490 - 0xe90];      // 0x0e90
} TERRAIN_PICKUP_SET;

static TERRAIN_PICKUP_SET *PickupTerr = 0;
i32 edanim_sound_mode = 0;
i32 edanim_sound_type = 0;
f32 TerrainShadowHeight = 0;
u16 TerrainRoofValid = 0;
i32 edpp_page_used[8] = {0};
struct eduimenu_s *namemenu = 0;
TERRAIN_POLYGON_INFO *ScaleTerrain = 0;
struct eduimenu_s *ptlcutoffmenu = 0;
struct eduimenu_s *ptlvaremitmenu = 0;
u8 TerrWallTab[4] = {0};
i32 freeDmaDebType = 0;
i32 PlatSkinMaxStore = 0;
u8 *PlatSkinMem = 0;
EDPP_PARTICLE edpp_ptls[512] = {0};
TERRAIN_WALL_POINT *WallSplList = 0;
u16 NuTerrPlatsOff = 0;
struct eduimenu_s *edptl_damage_menu = 0;
i32 edbri_cam_ay = 0;
i32 plathitid = 0;
i32 edanim_cam_ay = 0;
u8 debris_object_switches[128] = {0};
TERRAIN_POLYGON_INFO *ShadPoly = 0;
i16 TerrainShadowFloorIndex = 0;
i32 edgra_cam_ax = 0;
DEBRIS_CHUNK_CONTROL *debris_chunk_control_stack[2] = {0};
u32 TerrainUpadteCnt = 0;
i32 PlatSkinMaxSize = 0;
i32 edgra_page_calculate_done[8] = {0};
i32 edpp_cam_ay = 0;
NUVEC TerrainRoofNormal = {0};
NUGSCN *edbri_page_scene[8] = {0};

i32 ReadTerrainPickup(unsigned char *base_path, i16 **buffer, TERRAIN_PICKUP_SET *terrain) {
    char path[100];
    TERRAIN_FILE_CHUNK *chunk;
    TERRAIN_FILE_CHUNK *header;
    i32 group_count;
    i32 i;
    strcpy(path, (char *)base_path);
    strcat(path, ".ter");
    NuFileLoadBuffer(path, *buffer, INT_MAX);
    chunk = (TERRAIN_FILE_CHUNK *)(*buffer + *(i32 *)*buffer);
    *buffer += 2;
    header = chunk;
    group_count = 0;
    TerrainCrashData = NULL;
    for (i = 0; i < header->chunk_count; ++i, ++chunk) {
        switch (chunk->chunk_type) {
            case TERRAIN_CHUNK_GROUP_PRIMARY:
            case TERRAIN_CHUNK_GROUP_SECONDARY:
                terrain->groups[group_count].group_info = chunk->group_info;
                if (terrain->groups[group_count].group_info >= 0 && terrain->groups[group_count].group_info < 128) {
                    terrain->group_for_type[terrain->groups[group_count].group_info] = (i16)group_count;
                }
                terrain->groups[group_count].chunk_type = chunk->chunk_type;
                terrain->groups[group_count].scene_index = (i16)chunk->scene_index;
                terrain->groups[group_count].origin.x = chunk->origin.x;
                terrain->groups[group_count].origin.y = chunk->origin.y;
                terrain->groups[group_count].origin.z = chunk->origin.z;
                terrain->groups[group_count].data = (TERRAIN_SKIN_STREAM *)*buffer;
                terrain->groups[group_count].platform_flags = chunk->platform_flags;
                ++group_count;
                break;
            case TERRAIN_CHUNK_SPATIAL_INDEX:
                if (*(u32 *)*buffer == 0x12345678) {
                    *(TERRAIN_SPATIAL_NODE **)*buffer = terrain->spatial_nodes;
                    terrain->spatial_nodes = (TERRAIN_SPATIAL_NODE *)(*buffer + 2);
                    --terrain->spatial_nodes->point_count;
                }
                break;
            case TERRAIN_CHUNK_CRASH_DATA:
                TerrainCrashData = (u8 *)*buffer;
                break;
        }
        *buffer += chunk->data_word_count;
    }
    return group_count;
}

void terrainpickupinit(char *path, void **buffer) {
    TERRAIN_PICKUP_SET *terrain = *buffer;
    TERRAIN_GROUP *group;
    i32 i;
    *buffer = (u8 *)*buffer + sizeof(TERRAIN_PICKUP_SET);
    terrain->groups = *buffer;
    *buffer = (u8 *)*buffer + 32 * sizeof(TERRAIN_GROUP);
    terrain->shape_data = *buffer;
    *buffer = (u8 *)*buffer + 0x1c00;
    memset(terrain->groups, 0, 32 * sizeof(TERRAIN_GROUP));
    group = terrain->groups;
    for (i = 0; i < 32; ++i) {
        group[i].chunk_type = -1;
    }
    for (i = 0; i < 32; ++i) {
        terrain->platforms[i].scene_object = NULL;
        terrain->platforms[i].bounce_impulse = 0;
        terrain->platforms[i].bounce_offset = 0;
        terrain->platforms[i].bounce_velocity = 0;
        terrain->platforms[i].bounce_damping = 0;
        terrain->platforms[i].bounce_spring = 0;
    }
    for (i = 0; i < 128; ++i) {
        terrain->group_for_type[i] = -1;
    }
    curPickInst = 0;
    terraincnt = 0;
    curSphereter = 0;
    platinrange = 0;
    ShadPoly = NULL;
    TerrPoly = NULL;
    TerrWallInfo = 0;
    PlatImpactId = -1;
    terrain->group_count = ReadTerrainPickup((unsigned char *)path, (i16 **)buffer, terrain);
    group = terrain->groups;
    for (i = 0; i < 32; ++i, ++group) {
        TERRAIN_SKIN_STREAM *batch;
        f32 min_x;
        f32 min_y;
        f32 min_z;
        f32 max_x;
        f32 max_y;
        f32 max_z;
        f32 radius_squared;
        if (group->chunk_type != TERRAIN_CHUNK_GROUP_PRIMARY && group->chunk_type != TERRAIN_CHUNK_GROUP_SECONDARY) {
            continue;
        }
        radius_squared = 0.0f;
        min_x = 200000000.0f;
        min_y = 200000000.0f;
        min_z = 200000000.0f;
        max_x = -200000000.0f;
        max_y = -200000000.0f;
        max_z = -200000000.0f;
        batch = group->data;
        while (batch->header.kind >= 0) {
            TERRAIN_POLYGON_INFO *shape = batch->polygons;
            i32 j;
            for (j = batch->header.count; j > 0; --j, ++shape) {
                i32 k;
                if (shape->material[0] == 0xff) {
                    shape->material[0] = shape->material[1];
                    shape->material[1] = 0;
                    shape->flags |= TERRAIN_SHAPE_FLAG_MATERIAL_ALIAS;
                }
                if (shape->min_x < min_x) {
                    min_x = shape->min_x;
                }
                if (shape->min_y < min_y) {
                    min_y = shape->min_y;
                }
                if (shape->min_z < min_z) {
                    min_z = shape->min_z;
                }
                if (shape->max_x > max_x) {
                    max_x = shape->max_x;
                }
                if (shape->max_y > max_y) {
                    max_y = shape->max_y;
                }
                if (shape->max_z > max_z) {
                    max_z = shape->max_z;
                }
                for (k = 0; k < 4; ++k) {
                    f32 length_squared = shape->vertices[k].x * shape->vertices[k].x + shape->vertices[k].y * shape->vertices[k].y +
                                         shape->vertices[k].z * shape->vertices[k].z;
                    if (radius_squared < length_squared) {
                        radius_squared = length_squared;
                    }
                }
            }
            batch = (TERRAIN_SKIN_STREAM *)shape;
        }
        group->radius = NuFsqrt(radius_squared);
        ++terraincnt;
        group->chunk_type = 0;
        group->bounds_min.x = min_x;
        group->bounds_min.y = min_y;
        group->bounds_min.z = min_z;
        group->bounds_max.x = max_x;
        group->bounds_max.y = max_y;
        group->bounds_max.z = max_z;
    }
    group = terrain->groups;
    for (i = 0; i < 32; ++i) {
        group[i].scene_index = (i16)i;
        terrain->platforms[i].scene_object = NULL;
    }
    PickupTerr = terrain;
}

void AddPickupTerr(i32 type, NUVEC *position) {
    TERRAIN_SET *terrain = CurrentTerrain;
    i16 source;
    i32 count;
    if (terrain == NULL) {
        return;
    }
    source = PickupTerr->group_for_type[type];
    if (source == -1) {
        return;
    }
    count = curPickInst;
    if (count >= 128) {
        return;
    }
    terrain->groups[terrain->max_groups + count] = PickupTerr->groups[source];
    terrain = CurrentTerrain;
    terrain->groups[terrain->max_groups + count].origin = *position;
    curPickInst = count + 1;
}

/* The real original caller expands the platform installer; no source-only helper is emitted. */

i32 AddPickupTerrRot(i32 type, NUMTX *matrix, NUMTX *previous, i32 rotating) {
    TERRAIN_GROUP *group;
    i16 source;
    i32 index;
    i32 found;
    if (CurrentTerrain == NULL) {
        return -1;
    }
    if (previous == NULL && rotating == 0) {
        AddPickupTerr(type, &matrix->translation);
        return -1;
    }
    if (CurrentTerrain->group_index_count >= CurrentTerrain->max_group_indices) {
        return -1;
    }
    if (CurrentTerrain->group_count >= CurrentTerrain->max_groups) {
        return -1;
    }
    if (matrix == NULL) {
        return -1;
    }
    if (previous == NULL && rotating != 0) {
        return -1;
    }
    source = PickupTerr->group_for_type[type];
    found = -1;
    if (source != -1 && CurrentTerrain->removed_platform_count < 32) {
        for (index = 0; index < CurrentTerrain->max_platforms; ++index) {
            if (CurrentTerrain->platforms[index].scene_object == NULL) {
                found = index;
                break;
            }
        }
    }
    if (found == -1) {
        return -1;
    }
    group = &CurrentTerrain->groups[CurrentTerrain->group_count];
    *group = PickupTerr->groups[source];
    group->scene_index = (i16)found;
    group->chunk_type = 1;
    CurrentTerrain->platforms[found].scene_object = matrix;
    if (rotating != 0) {
        memcpy(&CurrentTerrain->platforms[found].previous_matrix, previous, sizeof(NUMTX));
        CurrentTerrain->platforms[found].flags |= TERRAIN_PLATFORM_FLAG_ROTATING;
    } else {
        if (previous != NULL) {
            CurrentTerrain->platforms[found].previous_matrix.m30 = previous->m30;
            CurrentTerrain->platforms[found].previous_matrix.m31 = previous->m31;
            CurrentTerrain->platforms[found].previous_matrix.m32 = previous->m32;
        } else {
            CurrentTerrain->platforms[found].previous_matrix.m30 = matrix->m30;
            CurrentTerrain->platforms[found].previous_matrix.m31 = matrix->m31;
            CurrentTerrain->platforms[found].previous_matrix.m32 = matrix->m32;
        }
        CurrentTerrain->platforms[found].flags &= ~TERRAIN_PLATFORM_FLAG_ROTATING;
    }
    group->origin.x = matrix->m30;
    group->origin.y = matrix->m31;
    group->origin.z = matrix->m32;
    CurrentTerrain->platforms[found].terrain_group_index = CurrentTerrain->group_count;
    CurrentTerrain->platforms[found].scene_object_index = 0;
    CurrentTerrain->platforms[found].instance_flags = NULL;
    CurrentTerrain->group_indices[CurrentTerrain->group_index_count] = CurrentTerrain->group_count;
    ++CurrentTerrain->group_index_count;
    CurrentTerrain->platforms[found].bounce_impulse = 0;
    CurrentTerrain->platforms[found].bounce_offset = 0;
    CurrentTerrain->platforms[found].bounce_velocity = 0;
    CurrentTerrain->platforms[found].bounce_damping = 0;
    CurrentTerrain->platforms[found].bounce_spring = 0;
    CurrentTerrain->platforms[found].flags &= ~TERRAIN_PLATFORM_FLAG_COLLIDED;
    if (CurrentTerrain->active_platform_count < 96) {
        CurrentTerrain->active_platform_groups[CurrentTerrain->active_platform_count] = CurrentTerrain->group_count;
        ++CurrentTerrain->active_platform_count;
    }
    ++CurrentTerrain->group_count;
    ++CurrentTerrain->platform_cell.group_count;
    CurrentTerrain->removed_platforms[CurrentTerrain->removed_platform_count] = (i16)found;
    ++CurrentTerrain->removed_platform_count;
    return found;
}

i32 NewPlatPickupInst(void *object, i32 type) {
    TERRAIN_GROUP *group;
    i16 source;
    i32 index;
    i32 i;
    if (CurrentTerrain == NULL) {
        return -1;
    }
    if (CurrentTerrain->group_index_count >= CurrentTerrain->max_group_indices) {
        return -1;
    }
    if (CurrentTerrain->group_count >= CurrentTerrain->max_groups) {
        return -1;
    }
    if (object == NULL) {
        return -1;
    }
    source = PickupTerr->group_for_type[type];
    if (source == -1) {
        return -1;
    }
    index = -1;
    for (i = 0; i < CurrentTerrain->max_platforms; ++i) {
        if (CurrentTerrain->platforms[i].scene_object == NULL) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        return -1;
    }
    group = &CurrentTerrain->groups[CurrentTerrain->group_count];
    *group = PickupTerr->groups[source];
    group->chunk_type = 1;
    group->scene_index = (i16)index;
    CurrentTerrain->platforms[index].scene_object = object;
    CurrentTerrain->platforms[index].flags &= ~TERRAIN_PLATFORM_FLAG_ROTATING;
    group->origin.x = ((NUMTX *)object)->m30;
    group->origin.y = ((NUMTX *)object)->m31;
    group->origin.z = ((NUMTX *)object)->m32;
    CurrentTerrain->platforms[index].terrain_group_index = CurrentTerrain->group_count;
    CurrentTerrain->platforms[index].scene_object_index = 0;
    CurrentTerrain->platforms[index].instance_flags = NULL;
    CurrentTerrain->group_indices[CurrentTerrain->group_index_count] = CurrentTerrain->group_count;
    CurrentTerrain->group_index_count++;
    CurrentTerrain->platforms[index].bounce_impulse = 0;
    CurrentTerrain->platforms[index].bounce_offset = 0;
    CurrentTerrain->platforms[index].bounce_velocity = 0;
    CurrentTerrain->platforms[index].bounce_damping = 0;
    CurrentTerrain->platforms[index].bounce_spring = 0;
    CurrentTerrain->platforms[index].flags &= ~TERRAIN_PLATFORM_FLAG_COLLIDED;
    CurrentTerrain->group_count++;
    CurrentTerrain->platform_cell.group_count++;
    return index;
}

void ReassignPickupInst(i32 index, i32 type) {
    TERRAIN_SET *terrain = CurrentTerrain;
    TERRAIN_GROUP *source;
    TERRAIN_GROUP *group;
    i16 source_index;
    if (terrain == NULL || index == -1) {
        return;
    }
    source_index = PickupTerr->group_for_type[type];
    if (source_index == -1) {
        return;
    }
    group = &terrain->groups[terrain->platforms[index].terrain_group_index];
    source = &PickupTerr->groups[source_index];
    group->data = source->data;
    group->bounds_min = source->bounds_min;
    group->bounds_max = source->bounds_max;
    group->radius = source->radius;
}

/* Debug geometry uses the 100-byte polygon view. */

void TerrDraw(TERRAIN_POLYGON_INFO *terrain, i16 index) {
    TERRAIN_POLYGON_INFO *record;
    NUVEC *origin;
    i32 i;
    f32 t;
    f64 x;
    f64 y;
    f64 z;
    u8 blue = index & 0x80;
    u8 green = index << 6;

    record = terrain;
    if (!(record->normals[1].y > 65535.0)) {
        for (i = 0; i <= 1; ++i) {
            t = (f32)(i);
            origin = &CurrentTerrain->groups[index].origin;
            NuRndrLine3dDbg(origin->x + record->vertices[0].x + (record->vertices[1].x - record->vertices[0].x) * t,
                            origin->y + record->vertices[0].y + (record->vertices[1].y - record->vertices[0].y) * t,
                            origin->z + record->vertices[0].z + (record->vertices[1].z - record->vertices[0].z) * t,
                            origin->x + record->vertices[2].x + (record->vertices[3].x - record->vertices[2].x) * t,
                            origin->y + record->vertices[2].y + (record->vertices[3].y - record->vertices[2].y) * t,
                            origin->z + record->vertices[2].z + (record->vertices[3].z - record->vertices[2].z) * t, (green << 8) + blue);
        }
        for (i = 0; i <= 1; ++i) {
            t = (f32)(i);
            origin = &CurrentTerrain->groups[index].origin;
            NuRndrLine3dDbg(origin->x + record->vertices[0].x + (record->vertices[2].x - record->vertices[0].x) * t,
                            origin->y + record->vertices[0].y + (record->vertices[2].y - record->vertices[0].y) * t,
                            origin->z + record->vertices[0].z + (record->vertices[2].z - record->vertices[0].z) * t,
                            origin->x + record->vertices[1].x + (record->vertices[3].x - record->vertices[1].x) * t,
                            origin->y + record->vertices[1].y + (record->vertices[3].y - record->vertices[1].y) * t,
                            origin->z + record->vertices[1].z + (record->vertices[3].z - record->vertices[1].z) * t, (green << 8) + blue);
        }
    }
    origin = &CurrentTerrain->groups[index].origin;
    z = (f64)origin->z + record->vertices[0].z;
    y = (f64)origin->y + record->vertices[0].y;
    x = (f64)origin->x + record->vertices[0].x;
    NuRndrLine3dDbg(x, y, z, x + record->normals[0].x * (f64)0.4f, y + record->normals[0].y * (f64)0.4f,
                    z + record->normals[0].z * (f64)0.4f, (green << 8) + blue);
    if (record->normals[1].y < 65536.0) {
        origin = &CurrentTerrain->groups[index].origin;
        z = (f64)origin->z + record->vertices[3].z;
        y = (f64)origin->y + record->vertices[3].y;
        x = (f64)origin->x + record->vertices[3].x;
        NuRndrLine3dDbg(x, y, z, x + record->normals[1].x * (f64)0.4f, y + record->normals[1].y * (f64)0.4f,
                        z + record->normals[1].z * (f64)0.4f, (green << 8) + blue);
    }
}

void TerrDrawSitu(TERRAIN_POLYGON_INFO *terrain, NUVEC *situation) {
    TERRAIN_POLYGON_INFO *record = terrain;
    i32 i;
    f32 t;

    if (record->normals[1].y > 65535.0) {
        for (i = 0; i <= 1; ++i) {
            t = (f32)(i);
            NuRndrLine3dDbg(situation->x + record->vertices[0].x, situation->y + record->vertices[0].y,
                            situation->z + record->vertices[0].z,
                            situation->x + record->vertices[1].x + (record->vertices[2].x - record->vertices[1].x) * t,
                            situation->y + record->vertices[1].y + (record->vertices[2].y - record->vertices[1].y) * t,
                            situation->z + record->vertices[1].z + (record->vertices[2].z - record->vertices[1].z) * t, 0x003f7f80);
        }
        for (i = 0; i <= 1; ++i) {
            t = (f32)(i);
            NuRndrLine3dDbg(situation->x + record->vertices[1].x, situation->y + record->vertices[1].y,
                            situation->z + record->vertices[1].z,
                            situation->x + record->vertices[0].x + (record->vertices[2].x - record->vertices[0].x) * t,
                            situation->y + record->vertices[0].y + (record->vertices[2].y - record->vertices[0].y) * t,
                            situation->z + record->vertices[0].z + (record->vertices[2].z - record->vertices[0].z) * t, 0x003f7f80);
        }
    } else {
        for (i = 0; i <= 1; ++i) {
            t = (f32)(i);
            NuRndrLine3dDbg(situation->x + record->vertices[0].x + (record->vertices[1].x - record->vertices[0].x) * t,
                            situation->y + record->vertices[0].y + (record->vertices[1].y - record->vertices[0].y) * t,
                            situation->z + record->vertices[0].z + (record->vertices[1].z - record->vertices[0].z) * t,
                            situation->x + record->vertices[2].x + (record->vertices[3].x - record->vertices[2].x) * t,
                            situation->y + record->vertices[2].y + (record->vertices[3].y - record->vertices[2].y) * t,
                            situation->z + record->vertices[2].z + (record->vertices[3].z - record->vertices[2].z) * t, 0xffff7f80);
        }
        for (i = 0; i <= 1; ++i) {
            t = (f32)(i);
            NuRndrLine3dDbg(situation->x + record->vertices[0].x + (record->vertices[2].x - record->vertices[0].x) * t,
                            situation->y + record->vertices[0].y + (record->vertices[2].y - record->vertices[0].y) * t,
                            situation->z + record->vertices[0].z + (record->vertices[2].z - record->vertices[0].z) * t,
                            situation->x + record->vertices[1].x + (record->vertices[3].x - record->vertices[1].x) * t,
                            situation->y + record->vertices[1].y + (record->vertices[3].y - record->vertices[1].y) * t,
                            situation->z + record->vertices[1].z + (record->vertices[3].z - record->vertices[1].z) * t, 0xffff7f80);
        }
    }
}

/* Debug lines between the a..b and c..d edges of a platform shape, at both edge ends (a == b fans from one corner). */
static __forceinline void TerrDrawEdgeLines(const NUVEC4 *points, i16 index, i32 a, i32 b, i32 c, i32 d, i32 colour) {
    i32 i;
    for (i = 0; i <= 1; ++i) {
        f32 t = (f32)i;
        const NUVEC *origin = &CurrentTerrain->groups[index].origin;
        NuRndrLine3dDbg(
            origin->x + points[a].x + (points[b].x - points[a].x) * t, origin->y + points[a].y + (points[b].y - points[a].y) * t,
            origin->z + points[a].z + (points[b].z - points[a].z) * t, origin->x + points[c].x + (points[d].x - points[c].x) * t,
            origin->y + points[c].y + (points[d].y - points[c].y) * t, origin->z + points[c].z + (points[d].z - points[c].z) * t, colour);
    }
}

void TerrDrawPlat(TERRAIN_POLYGON_INFO *terrain, i16 index) {
    TERRAIN_PLATFORM *platform;
    i32 i;
    NUVEC4 points[5];
    NUVEC4 *normal;
    const NUVEC *origin;
    i32 colour;
    f32 t;
    i8 blue = (i8)(index & 0xf0);
    i8 green = (i8)(index << 4);

    platform = &CurrentTerrain->platforms[CurrentTerrain->groups[index].scene_index];
    if (platform->instance_flags != NULL) {
        if (platform->flags & TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED) {
            if (!(*platform->instance_flags & 2)) {
                return;
            }
        } else if (!(*platform->instance_flags & 1)) {
            return;
        }
    }

    for (i = 0; i < 4; ++i) {
        points[i].x = terrain->vertices[i].x;
        points[i].y = terrain->vertices[i].y;
        points[i].z = terrain->vertices[i].z;
        points[i].w = 0.0f;
    }
    normal = &points[4];
    normal->x = terrain->normals[0].x * (f64)0.4f;
    normal->y = terrain->normals[0].y * (f64)0.4f;
    normal->z = terrain->normals[0].z * (f64)0.4f;
    normal->w = 0.0f;
    colour = (terrain->flags & 0x80) ? 0x0000ffff : 0x00ff0000;

    if (CurrentTerrain->platforms[CurrentTerrain->groups[index].scene_index].flags & TERRAIN_PLATFORM_FLAG_ROTATING) {
        NuVec4MtxTransformVU0(&points[0], &points[0], (CurrentTerrain->platforms[CurrentTerrain->groups[index].scene_index].scene_object));
        NuVec4MtxTransformVU0(&points[1], &points[1], (CurrentTerrain->platforms[CurrentTerrain->groups[index].scene_index].scene_object));
        NuVec4MtxTransformVU0(&points[2], &points[2], (CurrentTerrain->platforms[CurrentTerrain->groups[index].scene_index].scene_object));
        NuVec4MtxTransformVU0(&points[3], &points[3], (CurrentTerrain->platforms[CurrentTerrain->groups[index].scene_index].scene_object));
        NuVec4MtxTransformVU0(normal, normal, (CurrentTerrain->platforms[CurrentTerrain->groups[index].scene_index].scene_object));
    }

    if (terrain->normals[1].y > 65535.0) {
        TerrDrawEdgeLines(points, index, 0, 0, 1, 2, colour);
        TerrDrawEdgeLines(points, index, 1, 1, 0, 2, colour);
    } else {
        TerrDrawEdgeLines(points, index, 0, 1, 2, 3, colour);
        TerrDrawEdgeLines(points, index, 0, 2, 1, 3, colour);
    }
    origin = &CurrentTerrain->groups[index].origin;
    NuRndrLine3dDbg(origin->x + points[0].x, origin->y + points[0].y, origin->z + points[0].z, origin->x + points[0].x + normal->x,
                    origin->y + points[0].y + normal->y, origin->z + points[0].z + normal->z, (green << 8) + blue);
}

void TerrDrawPlatCol(TERRAIN_POLYGON_INFO *terrain, i16 index, i32 colour) {
    TERRAIN_PLATFORM *platform;
    NUVEC4 points[4];
    NUVEC4 normals[2];
    const NUVEC *origin;
    i32 i;
    f32 t;

    platform = &CurrentTerrain->platforms[CurrentTerrain->groups[index].scene_index];
    if (platform->instance_flags != NULL) {
        if (platform->flags & TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED) {
            if ((*platform->instance_flags & 2) == 0) {
                return;
            }
        } else if ((*platform->instance_flags & 1) == 0) {
            return;
        }
    }

    for (i = 0; i < 3; ++i) {
        points[i].x = terrain->vertices[i].x;
        points[i].y = terrain->vertices[i].y;
        points[i].z = terrain->vertices[i].z;
        points[i].w = 0.0f;
    }
    normals[0].x = terrain->normals[0].x * (f64)0.4f;
    normals[0].y = terrain->normals[0].y * (f64)0.4f;
    normals[0].z = terrain->normals[0].z * (f64)0.4f;
    normals[0].w = 0.0f;

    if (CurrentTerrain->platforms[CurrentTerrain->groups[index].scene_index].flags & TERRAIN_PLATFORM_FLAG_ROTATING) {
        NuVec4MtxTransformVU0(&points[0], &points[0], (CurrentTerrain->platforms[CurrentTerrain->groups[index].scene_index].scene_object));
        NuVec4MtxTransformVU0(&points[1], &points[1], (CurrentTerrain->platforms[CurrentTerrain->groups[index].scene_index].scene_object));
        NuVec4MtxTransformVU0(&points[2], &points[2], (CurrentTerrain->platforms[CurrentTerrain->groups[index].scene_index].scene_object));
        NuVec4MtxTransformVU0(&normals[0], &normals[0],
                              (CurrentTerrain->platforms[CurrentTerrain->groups[index].scene_index].scene_object));
    }

    if (terrain->normals[1].y > 65535.0) {
        TerrDrawEdgeLines(points, index, 0, 0, 1, 2, colour);
        TerrDrawEdgeLines(points, index, 1, 1, 0, 2, colour);
    } else {
        points[3].x = terrain->vertices[3].x;
        points[3].y = terrain->vertices[3].y;
        points[3].z = terrain->vertices[3].z;
        points[3].w = 0.0f;
        NuVec4MtxTransformVU0(&points[3], &points[3], (CurrentTerrain->platforms[CurrentTerrain->groups[index].scene_index].scene_object));
        normals[1].x = terrain->normals[1].x * (f64)0.4f;
        normals[1].y = terrain->normals[1].y * (f64)0.4f;
        normals[1].z = terrain->normals[1].z * (f64)0.4f;
        normals[1].w = 0.0f;
        NuVec4MtxTransformVU0(&normals[0], &normals[0],
                              (CurrentTerrain->platforms[CurrentTerrain->groups[index].scene_index].scene_object));
        TerrDrawEdgeLines(points, index, 0, 1, 2, 3, colour);
        TerrDrawEdgeLines(points, index, 0, 2, 1, 3, colour);
        origin = &CurrentTerrain->groups[index].origin;
        NuRndrLine3dDbg(origin->x + points[3].x, origin->y + points[3].y, origin->z + points[3].z, origin->x + points[3].x + normals[1].x,
                        origin->y + points[3].y + normals[1].y, origin->z + points[3].z + normals[1].z, 0xff);
    }
    origin = &CurrentTerrain->groups[index].origin;
    NuRndrLine3dDbg(origin->x + points[0].x, origin->y + points[0].y, origin->z + points[0].z, origin->x + points[0].x + normals[0].x,
                    origin->y + points[0].y + normals[0].y, origin->z + points[0].z + normals[0].z, 0xff);
}

typedef struct terrain_debug_source_link_s { // size 0x4
    TERRAIN_POLYGON_INFO *source_polygon;    // 0x0
} TERRAIN_DEBUG_SOURCE_LINK;

void TerrDrawImpactPol(void) {
    TerrainQuery_s *query = TerI;
    TERRAIN_POLYGON_INFO *surface;
    TERRAIN_POLYGON_INFO *scaled;
    if (query == NULL || query->surface == NULL) {
        return;
    }
    surface = query->surface;
    scaled = ScaleTerrain;
    if ((u32)surface >= (u32)scaled && (u32)surface < (u32)scaled + 0xc800) {
        surface = ((TERRAIN_DEBUG_SOURCE_LINK *)surface)->source_polygon;
    }
    TerrDraw(surface, query->terrain_group_index);
}

void DrawMPlat(i32 index) {
    TERRAIN_GROUP *group;
    TERRAIN_SKIN_STREAM *batch;
    TERRAIN_POLYGON_INFO *shape;
    i32 remaining;
    if (CurrentTerrain == NULL || index < 0) {
        return;
    }
    group = &CurrentTerrain->groups[CurrentTerrain->platforms[index].terrain_group_index];
    if (group->chunk_type != 0 && group->chunk_type != 1) {
        return;
    }
    batch = group->data;
    while (batch->header.kind >= 0) {
        shape = batch->polygons;
        for (remaining = batch->header.count; remaining > 0; --remaining, ++shape) {
            TerrDrawPlat(shape, CurrentTerrain->platforms[index].terrain_group_index);
        }
        batch = (TERRAIN_SKIN_STREAM *)shape;
    }
}

void DrawMSitu(i32 index) {
    TERRAIN_GROUP *group = &CurrentTerrain->groups[index];
    TERRAIN_SKIN_STREAM *batch;
    TERRAIN_POLYGON_INFO *shape;
    i32 remaining;
    if ((group->chunk_type != 0 && group->chunk_type != 1) || group->data == NULL) {
        return;
    }
    batch = group->data;
    while (batch->header.kind >= 0) {
        shape = batch->polygons;
        for (remaining = batch->header.count; remaining > 0; --remaining, ++shape) {
            TerrDraw(shape, (i16)index);
        }
        batch = (TERRAIN_SKIN_STREAM *)shape;
    }
}

void SphereDrawEx(NUVEC *centre, f32 radius, f32 vertical_scale, i32 colour) {
    f64 radius_squared = (f64)radius * radius;
    f64 scaled_radius = (f64)radius * vertical_scale;
    f32 lower_radius;
    f32 upper_radius;
    f32 lower_y;
    f32 upper_y;
    i32 band;
    i32 angle;
    for (band = 0; band < 8; ++band) {
        if (band < 4) {
            lower_radius = radius * NuTrigTable[((band * 4096) >> 1) & 0x7fff];
        } else {
            lower_radius = radius * NuTrigTable[(((8 - band) * 4096) >> 1) & 0x7fff];
        }
        if (band < 3) {
            upper_radius = radius * NuTrigTable[(((band + 1) * 4096) >> 1) & 0x7fff];
        } else {
            upper_radius = radius * NuTrigTable[(((7 - band) * 4096) >> 1) & 0x7fff];
        }
        lower_y = NuFsqrt(radius_squared - (f64)lower_radius * lower_radius) * vertical_scale;
        upper_y = NuFsqrt(radius_squared - (f64)upper_radius * upper_radius) * vertical_scale;
        if (band > 4) {
            lower_y = -lower_y;
        }
        if (band > 3) {
            upper_y = -upper_y;
        }
        lower_y += scaled_radius + centre->y;
        upper_y += scaled_radius + centre->y;
        for (angle = 0; angle < 65536; angle += 2048) {
            NuRndrLine3dDbg(centre->x + lower_radius * NuTrigTable[((angle + 16384) >> 1) & 0x7fff], lower_y,
                            centre->z + lower_radius * NuTrigTable[(angle >> 1) & 0x7fff],
                            centre->x + upper_radius * NuTrigTable[((angle + 16384) >> 1) & 0x7fff], upper_y,
                            centre->z + upper_radius * NuTrigTable[(angle >> 1) & 0x7fff], colour);
            NuRndrLine3dDbg(centre->x + lower_radius * NuTrigTable[((angle + 16384) >> 1) & 0x7fff], lower_y,
                            centre->z + lower_radius * NuTrigTable[(angle >> 1) & 0x7fff],
                            centre->x + lower_radius * NuTrigTable[((angle + 2048 + 16384) >> 1) & 0x7fff], lower_y,
                            centre->z + lower_radius * NuTrigTable[((angle + 2048) >> 1) & 0x7fff], colour);
        }
    }
}

void SphereDraw(NUVEC *centre, f32 radius, f32 vertical_scale) {
    SphereDrawEx(centre, radius, vertical_scale, 0x00400000);
}

void DrawWallSpline(f32 height) {
    i32 i;
    NUVEC start;
    NUVEC end;
    if (WallSplCount == 0) {
        return;
    }
    for (i = 0; i < WallSplCount; i += 2) {
        start.x = WallSplList[i].position.x;
        start.y = height;
        start.z = WallSplList[i].position.z;
        if (i == WallSplCount) {
            end.x = WallSplList[i].position.x;
            end.z = WallSplList[i].position.z;
        } else {
            end.x = WallSplList[i + 1].position.x;
            end.z = WallSplList[i + 1].position.z;
        }
        end.y = height;
        NuRndrLine3dDbg(start.x, start.y, start.z, end.x, end.y, end.z, (i32)0x80808080);
    }
}

static i32 TerrainSkinInfoCapacity = 0;
NUVEC4 tertempvec4 = {0};
i32 edgra_nearest = 0;
i32 SfxRefCounts[1601] = {0};
i32 edgra_clump_size = 0;
i32 edpp_snap_enabled = 0;
i32 edanim_nearest = 0;
DEBRIS_CHUNK_CONTROL **freechunkcontrols = 0;
struct eduimenu_s *ptlgravmenu = 0;
i16 TerrainRoofIndex = 0;
i32 edpp_cam_ax = 0;
i32 edpp_nearest = 0;
NUVEC TerrWallNorm = {0};
EDBITS_GAME_SOUND edbitsGameSound[320] = {0};
i32 WallSplCount = 0;
NUVEC edbri_cam_pos = {0};
i32 edanim_particle_type = 0;
i32 edpp_copy_source[8] = {0};
struct eduimenu_s *effectlistmenu = 0;
static u8 *TerrainSkinArenaEnd = 0;
i32 PlatSkinCnt = 0;
f32 TerrWallDeflectYScale = 0;
i16 TerrainCastAuxIndex = 0;
i32 TerrShapeAdjCnt = 0;
struct TERRAIN_SKIN_INFO *PlatSkinInfo = 0;
void *TempScanStack = 0;
i32 PlatCodeCallback = 0;
i32 TerImpactDataMax = 0;
i32 *TerImpactDataCount = 0;
static i32 TerrainSkinOriginalGroupCount = -1;
i32 TerrPlatDis = -1;
f32 TerConTol = 0.1f;
f32 TerrPlatScanDist = 10.0f;
i32 SkinFlipTab[8] = {0, 1, 2, 3, 0, 2, 1, 3};

/* This preparation updates group bounds/radius, leaving the copied polygon
 * and batch bounds intact. */
void TerrainSkinPrepareGroup(i32 group_index, TERRAIN_SKIN_STREAM *output, const TERRAIN_SKIN_INFO *info) {
    TERRAIN_GROUP *group;
    TERRAIN_SKIN_STREAM *input;
    TERRAIN_POLYGON_INFO *source;
    TERRAIN_POLYGON_INFO *polygon;
    NUVEC *p;
    NUVEC transformed;
    NUVEC a;
    NUVEC b;
    i32 count;
    i32 vertex;
    i32 bytes;
    f32 z;
    f32 length_squared;
    f32 min_x;
    f32 min_y;
    f32 min_z;
    f32 max_x;
    f32 max_y;
    f32 max_z;
    f32 radius_squared;
    if (CurrentTerrain == NULL || group_index < 0) {
        return;
    }
    min_x = min_y = min_z = 123456789.0f;
    group = &CurrentTerrain->groups[group_index];
    max_x = max_y = max_z = -123456789.0f;
    radius_squared = 0.0f;
    if (group->chunk_type == 0 || group->chunk_type == 1) {
        input = group->data;
        while (input->header.kind >= 0) {
            output->header.kind = input->header.kind;
            output->header.count = input->header.count;
            count = input->header.count;
            source = input->polygons;
            polygon = output->polygons;
            for (; count > 0; --count) {
                if (source->normals[1].y > 65535.0) {
                    vertex = 2;
                } else {
                    vertex = 3;
                }
                *polygon = *source;
                for (p = &polygon->vertices[vertex]; vertex >= 0; --vertex, --p) {
                    *p = *TerrainSkin(&transformed, info, &source->vertices[SkinFlipTab[vertex + info->vertex_order * 4]], -1.0f,
                                      info->mode);
                    p->x = p->x - info->matrix->m30;
                    p->y = p->y - info->matrix->m31;
                    p->z = p->z - info->matrix->m32;
                    z = p->z;
                    if (p->x < min_x) {
                        min_x = p->x;
                    }
                    if (p->x > max_x) {
                        max_x = p->x;
                    }
                    if (p->y < min_y) {
                        min_y = p->y;
                    }
                    if (p->y > max_y) {
                        max_y = p->y;
                    }
                    if (z < min_z) {
                        min_z = z;
                    }
                    if (z > max_z) {
                        max_z = z;
                    }
                    length_squared = p->x * p->x + p->y * p->y + z * z;
                    if (radius_squared < length_squared) {
                        radius_squared = length_squared;
                    }
                }
                if (source->normals[1].y < 65535.0) {
                    a.x = polygon->vertices[1].x - polygon->vertices[3].x;
                    a.y = polygon->vertices[1].y - polygon->vertices[3].y;
                    a.z = polygon->vertices[1].z - polygon->vertices[3].z;
                    b.x = polygon->vertices[2].x - polygon->vertices[3].x;
                    b.y = polygon->vertices[2].y - polygon->vertices[3].y;
                    b.z = polygon->vertices[2].z - polygon->vertices[3].z;
                    {
                        NUVEC cross;
                        f32 inverse;
                        cross.x = a.y * b.z - a.z * b.y;
                        cross.y = a.z * b.x - a.x * b.z;
                        cross.z = a.x * b.y - a.y * b.x;
                        polygon->normals[1] = cross;
                        length_squared = polygon->normals[1].x * polygon->normals[1].x + polygon->normals[1].y * polygon->normals[1].y +
                                         polygon->normals[1].z * polygon->normals[1].z;
                        inverse = length_squared != 0.0f ? 1.0f / NuFsqrt(length_squared) : 0.0f;
                        polygon->normals[1].x = polygon->normals[1].x * inverse;
                        polygon->normals[1].y = polygon->normals[1].y * inverse;
                        polygon->normals[1].z = polygon->normals[1].z * inverse;
                    }
                }
                a.x = polygon->vertices[2].x - polygon->vertices[0].x;
                a.y = polygon->vertices[2].y - polygon->vertices[0].y;
                a.z = polygon->vertices[2].z - polygon->vertices[0].z;
                b.x = polygon->vertices[1].x - polygon->vertices[0].x;
                b.y = polygon->vertices[1].y - polygon->vertices[0].y;
                b.z = polygon->vertices[1].z - polygon->vertices[0].z;
                {
                    NUVEC cross;
                    f32 inverse;
                    cross.x = a.y * b.z - a.z * b.y;
                    cross.y = a.z * b.x - a.x * b.z;
                    cross.z = a.x * b.y - a.y * b.x;
                    polygon->normals[0] = cross;
                    length_squared = polygon->normals[0].x * polygon->normals[0].x + polygon->normals[0].y * polygon->normals[0].y +
                                     polygon->normals[0].z * polygon->normals[0].z;
                    inverse = length_squared != 0.0f ? 1.0f / NuFsqrt(length_squared) : 0.0f;
                    polygon->normals[0].x = polygon->normals[0].x * inverse;
                    polygon->normals[0].y = polygon->normals[0].y * inverse;
                    polygon->normals[0].z = polygon->normals[0].z * inverse;
                }
                ++source;
                ++polygon;
            }
            input = (TERRAIN_SKIN_STREAM *)source;
            output = (TERRAIN_SKIN_STREAM *)polygon;
        }
        output->header.kind = -1;
        output->header.count = -1;
        bytes = ((u8 *)input - (u8 *)group->data) + 4;
    } else {
        bytes = 0;
    }
    group->origin.x = info->matrix->m30;
    group->origin.y = info->matrix->m31;
    group->origin.z = info->matrix->m32;
    group->bounds_min.x = min_x - (f64)0.1f + group->origin.x;
    group->bounds_min.y = min_y - (f64)0.1f + group->origin.y;
    group->bounds_min.z = min_z - (f64)0.1f + group->origin.z;
    group->bounds_max.x = max_x + (f64)0.1f + group->origin.x;
    group->bounds_max.y = max_y + (f64)0.1f + group->origin.y;
    group->bounds_max.z = max_z + (f64)0.1f + group->origin.z;
    group->radius = NuFsqrt(radius_squared);
    if (bytes > PlatSkinMaxSize) {
        PlatSkinMaxSize = bytes;
    }
}

/* Keeps the ordered error checks and live group count reloads across the
 * skin preparation call. */

i32 TerrainSkinCreateInfo(NUMTX *matrix, const TERRAIN_SKIN_BLEND *blend, const NUVEC4 *sample_rows, f32 scale, i32 source_scene_index,
                          i16 mode, TERRAIN_SET *source_terrain) {
    TERRAIN_SKIN_INFO *info;
    i32 count;
    i32 skin_index;
    i32 index;
    i32 found;
    if (CurrentTerrain == NULL || source_terrain == NULL) {
        return -1;
    }
    count = CurrentTerrain->group_count;
    if (count >= CurrentTerrain->max_groups) {
        return -3;
    }
    if (matrix == NULL || blend == NULL) {
        return -4;
    }
    skin_index = PlatSkinCnt;
    if (skin_index >= TerrainSkinInfoCapacity) {
        return -5;
    }
    found = -1;
    for (index = 0; index < source_terrain->max_platforms; ++index) {
        if (source_terrain->platforms[index].scene_object != NULL &&
            source_terrain->platforms[index].scene_object_index == source_scene_index) {
            found = index;
            break;
        }
    }
    if (found == -1) {
        return -6;
    }
    CurrentTerrain->groups[count] = source_terrain->groups[source_terrain->platforms[found].terrain_group_index];
    CurrentTerrain->groups[CurrentTerrain->group_count].chunk_type = 0;
    info = &PlatSkinInfo[skin_index];
    info->matrix = matrix;
    info->blend = blend;
    info->sample_rows = sample_rows;
    info->scale = scale;
    info->mode = mode;
    if (matrix->m20 * matrix->m02 - matrix->m22 * matrix->m00 > 0.0) {
        info->vertex_order = 1;
    } else {
        info->vertex_order = 0;
    }
    TerrainSkinPrepareGroup(CurrentTerrain->group_count, (TERRAIN_SKIN_STREAM *)PlatSkinMem, info);
    skin_index = PlatSkinCnt;
    info = &PlatSkinInfo[skin_index];
    info->group_index = CurrentTerrain->group_count;
    info->original_shape = CurrentTerrain->groups[CurrentTerrain->group_count].data;
    CurrentTerrain->groups[CurrentTerrain->group_count].scene_index = (i16)(-1 - skin_index);
    CurrentTerrain->groups[CurrentTerrain->group_count].data = NULL;
    ++CurrentTerrain->group_count;
    PlatSkinCnt = skin_index + 1;
    return CurrentTerrain->group_count - 1;
}

i32 TerrainSkinCreateInfoCurrent(NUMTX *matrix, const TERRAIN_SKIN_BLEND *blend, const NUVEC4 *sample_rows, f32 scale,
                                 i32 source_scene_index, i16 mode) {
    return TerrainSkinCreateInfo(matrix, blend, sample_rows, scale, source_scene_index, mode, CurrentTerrain);
}

/* Caller-owned arena start/end and explicit record capacity. This
 * partitions an existing arena. */
void TerrainSkinInitArena(TERRAIN_SKIN_INFO *base, u8 *end, i32 capacity) {
    PlatSkinInfo = base;
    PlatSkinMem = (u8 *)(base + capacity);
    PlatSkinMaxSize = 0;
    TerrainSkinInfoCapacity = capacity;
    PlatSkinCnt = 0;
    TerrainSkinArenaEnd = end;
    TerrainSkinOriginalGroupCount = CurrentTerrain->group_count;
}

void TerrainSkinResetArena(void) {
    i32 i;
    if (TerrainSkinOriginalGroupCount < 0) {
        return;
    }
    CurrentTerrain->group_count = (i16)TerrainSkinOriginalGroupCount;
    for (i = 0; i < 16; ++i) {
        CurrentTerrain->index_levels[i].cache_age = 0;
    }
}

/* Temporary bounds and cell selectors are carved backwards from the
 * supplied end. */
u8 *TerrainSkinRebuildIndex(i32 pool_slots) {
    TERRAIN_SET *terrain = CurrentTerrain;
    TERRAIN_GROUP *group;
    TERRAIN_SKIN_USE_RECORD *use;
    i16 *selectors;
    f32 *min_x;
    f32 *max_x;
    f32 *min_z;
    f32 *max_z;
    f32 world_min_x = 200000000.0f;
    f32 world_min_y = 200000000.0f;
    f32 world_min_z = 200000000.0f;
    f32 world_max_x = -200000000.0f;
    f32 world_max_y = -200000000.0f;
    f32 world_max_z = -200000000.0f;
    i32 capacity = terrain->max_groups;
    i32 i;
    i32 cell_index;
    selectors = (i16 *)(((u32)TerrainSkinArenaEnd & ~1u) - (u32)capacity * 2u);
    min_x = (f32 *)(((u32)selectors & ~3u) - (u32)capacity * 4u);
    max_x = min_x - capacity;
    min_z = max_x - capacity;
    max_z = min_z - capacity;
    group = terrain->groups;
    for (i = 0; i < terrain->group_count; ++i, ++group) {
        const i32 chunk_type = group->chunk_type;
        if (chunk_type == 0) {
            if (group->bounds_min.x < world_min_x) {
                world_min_x = group->bounds_min.x;
            }
            /* Each comparison uses the matching minimum or maximum. */
            if (group->bounds_min.y < world_min_y) {
                world_min_y = group->bounds_min.y;
            }
            if (group->bounds_min.z < world_min_z) {
                world_min_z = group->bounds_min.z;
            }
            if (group->bounds_max.x > world_max_x) {
                world_max_x = group->bounds_max.x;
            }
            if (group->bounds_max.y > world_max_y) {
                world_max_y = group->bounds_max.y;
            }
            if (group->bounds_max.z > world_max_z) {
                world_max_z = group->bounds_max.z;
            }
        }
        if (chunk_type == 1) {
            if (group->bounds_min.x < world_min_x) {
                world_min_x = group->origin.x + group->bounds_min.x;
            }
            if (group->bounds_min.y < world_min_y) {
                world_min_y = group->origin.y + group->bounds_min.y;
            }
            if (group->bounds_min.z < world_min_z) {
                world_min_z = group->origin.z + group->bounds_min.z;
            }
            if (group->bounds_max.x > world_max_x) {
                world_max_x = group->origin.x + group->bounds_max.x;
            }
            if (group->bounds_max.y > world_max_y) {
                world_max_y = group->origin.y + group->bounds_max.y;
            }
            if (group->bounds_max.z > world_max_z) {
                world_max_z = group->origin.z + group->bounds_max.z;
            }
        }
    }
    terrain->group_index_count = 0;
    group = terrain->groups;
    for (i = 0; i < terrain->group_count; ++i, ++group) {
        if (group->chunk_type == 0) {
            i32 x;
            i32 z;
            min_z[i] = min_x[i] = 200000000.0f;
            max_z[i] = max_x[i] = -200000000.0f;
            if (group->bounds_min.x < min_x[i]) {
                min_x[i] = group->bounds_min.x;
            }
            if (group->bounds_min.z < min_z[i]) {
                min_z[i] = group->bounds_min.z;
            }
            if (group->bounds_max.x > max_x[i]) {
                max_x[i] = group->bounds_max.x;
            }
            if (group->bounds_max.z > max_z[i]) {
                max_z[i] = group->bounds_max.z;
            }
            x = (i32)((f64)((i32)(((f64)min_x[i] + max_x[i]) * 0.5 - world_min_x) * 7) / ((f64)world_max_x - world_min_x));
            if (x < 0) {
                x = 0;
            } else if (x > 6) {
                x = 6;
            }
            z = (i32)((f64)((i32)(((f64)min_z[i] + max_z[i]) * 0.5 - world_min_z) * 7) / ((f64)world_max_z - world_min_z));
            if (z < 0) {
                z = 0;
            } else if (z > 6) {
                z = 6;
            }
            selectors[i] = (i16)(z * 7 + x);
        }
    }
    /* The clear runs one past cells[] into the adjacent platform_cell. */
    for (i = 0; i < 257; ++i) {
        terrain->cells[i].group_count = 0;
    }
    terrain->used_cell_count = 0;
    for (cell_index = 0; cell_index < 49; ++cell_index) {
        f32 cell_min_x = 200000000.0f;
        f32 cell_min_z = 200000000.0f;
        f32 cell_max_x = -200000000.0f;
        f32 cell_max_z = -200000000.0f;
        terrain->cells[terrain->used_cell_count].first_group = (i16)terrain->group_index_count;
        group = terrain->groups;
        for (i = 0; i < terrain->group_count; ++i, ++group) {
            if (selectors[i] == cell_index && group->chunk_type == 0) {
                if (min_x[i] < cell_min_x) {
                    cell_min_x = min_x[i];
                }
                if (min_z[i] < cell_min_z) {
                    cell_min_z = min_z[i];
                }
                if (max_x[i] > cell_max_x) {
                    cell_max_x = max_x[i];
                }
                if (max_z[i] > cell_max_z) {
                    cell_max_z = max_z[i];
                }
                ++terrain->cells[terrain->used_cell_count].group_count;
                terrain->group_indices[terrain->group_index_count++] = (i16)i;
            }
        }
        if (terrain->cells[terrain->used_cell_count].group_count != 0) {
            terrain->cells[terrain->used_cell_count].min_x = cell_min_x;
            terrain->cells[terrain->used_cell_count].min_z = cell_min_z;
            terrain->cells[terrain->used_cell_count].max_x = cell_max_x;
            terrain->cells[terrain->used_cell_count].max_z = cell_max_z;
            ++terrain->used_cell_count;
        }
    }
    terrain->platform_cell.first_group = (i16)terrain->group_index_count;
    group = terrain->groups;
    for (i = 0; i < terrain->group_count; ++i) {
        if (group->chunk_type == 1) {
            terrain->group_indices[terrain->group_index_count++] = (i16)i;
            ++terrain->platform_cell.group_count;
            terrain->platforms[group->scene_index].terrain_group_index = (i16)i;
            ++group; /* The cursor advances only on the platform branch. */
        }
    }
    use = (TERRAIN_SKIN_USE_RECORD *)(PlatSkinMem + PlatSkinMaxSize * pool_slots);
    SkinMemInfo = use;
    for (i = 0; i < pool_slots; ++i) {
        use[i].last_use = 0;
        use[i].skin_index = -1;
    }
    PlatSkinMaxStore = pool_slots;
    return (u8 *)(use + pool_slots);
}

TERRAIN_GROUP *TerrainFindInstanceGroup(i32 instance) {
    TERRAIN_SET *terrain = CurrentTerrain;
    i32 index = -1;
    i32 i;
    if (terrain == NULL) {
        return NULL;
    }
    for (i = 0; i < terrain->max_platforms; ++i) {
        if (terrain->platforms[i].scene_object != NULL && terrain->platforms[i].scene_object_index == instance) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        return NULL;
    }
    return &terrain->groups[terrain->platforms[index].terrain_group_index];
}

static const char *errstr[] = {"ERR_UNKNOWN", "ERR_NOTERR",      "ERR_MAXTERLIST", "ERR_MAXTERR",
                               "ERR_INOUT",   "ERR_PLATSKINMAX", "ERR_NOINSTANCE"};

const char *TerrErrorString(i32 error) {
    i32 index = -error;
    if (index >= 7) {
        index = 0;
    }
    return errstr[index];
}

/* A sound table followed by the NULL-terminated sfx name list it is built from. */
typedef struct SOUND_REGISTRY { // size 0xcc
    SoundTable table;           // 0x00
    char **names;               // 0xc8
} SOUND_REGISTRY;

void RegisterSounds(SOUND_REGISTRY *registry) {
    char **name = registry->names;
    memset(registry->table.bits, 0, sizeof(registry->table.bits));
    /* The original tests the advancing cursor rather than the name it points at. */
    while (name != NULL) {
        i32 id = GetSfxId(*name++);
        registry->table.bits[id >> 4] |= 1 << (id & 0xf);
    }
}

void TerrainScanWallSpline(TERRAIN_SPATIAL_NODE *node) {
    u32 first = 0;
    while ((node->point_count - 3) >= (i32)first) {
        NUVEC minimum;
        NUVEC maximum;
        i32 count = 16;
        i32 i;
        if (node->point_count + 1 < count) {
            count = node->point_count + 1;
        }
        maximum = node->points[first];
        minimum = maximum;
        for (i = 1; i < count; ++i) {
            if (node->points[first + i].x < minimum.x) {
                minimum.x = node->points[first + i].x;
            }
            if (node->points[first + i].z < minimum.z) {
                minimum.z = node->points[first + i].z;
            }
            if (node->points[first + i].x > maximum.x) {
                maximum.x = node->points[first + i].x;
            }
            if (node->points[first + i].z > maximum.z) {
                maximum.z = node->points[first + i].z;
            }
        }
        node->points[first].y = minimum.x;
        node->points[first + 1].y = maximum.x;
        node->points[first + 2].y = minimum.z;
        node->points[first + 3].y = maximum.z;
    }
    if (first < (u32)(node->point_count + 1)) {
        node->points[first].y = 2147483648.0f;
    }
}

void TerrainAddWallSpline(TERRAIN_SPATIAL_NODE *node, TERRAIN_SET *terrain) {
    TERRAIN_SPATIAL_CHUNK *chunk = (TERRAIN_SPATIAL_CHUNK *)((u8 *)node - offsetof(TERRAIN_SPATIAL_CHUNK, node));
    chunk->previous = terrain->spatial_nodes;
    terrain->spatial_nodes = node;
}

void TerrainRemoveWallSpline(TERRAIN_SPATIAL_NODE *node, TERRAIN_SET *terrain) {
    TERRAIN_SPATIAL_NODE *current;
    TERRAIN_SPATIAL_NODE *last;
    if (terrain == NULL) {
        return;
    }
    current = terrain->spatial_nodes;
    if (current == NULL) {
        return;
    }
    if (current == node) {
        terrain->spatial_nodes = (((TERRAIN_SPATIAL_CHUNK *)((u8 *)(node)-offsetof(TERRAIN_SPATIAL_CHUNK, node)))->previous);
        return;
    }
    do {
        last = current;
        current = (((TERRAIN_SPATIAL_CHUNK *)((u8 *)(current)-offsetof(TERRAIN_SPATIAL_CHUNK, node)))->previous);
        if (current == node) {
            (((TERRAIN_SPATIAL_CHUNK *)((u8 *)(last)-offsetof(TERRAIN_SPATIAL_CHUNK, node)))->previous) =
                (((TERRAIN_SPATIAL_CHUNK *)((u8 *)(node)-offsetof(TERRAIN_SPATIAL_CHUNK, node)))->previous);
            return;
        }
    } while (current != NULL);
}

void PlatInstBounce(i32 index, f32 impulse, f32 spring, f32 damping) {
    if (index >= 0) {
        if (index < CurrentTerrain->max_platforms) {
            if (CurrentTerrain->platforms[index].bounce_impulse == 0.0f) {
                CurrentTerrain->platforms[index].bounce_offset = 0.0f;
                CurrentTerrain->platforms[index].bounce_velocity = 0.0f;
            }
            CurrentTerrain->platforms[index].bounce_impulse = impulse;
            CurrentTerrain->platforms[index].bounce_spring = spring;
            CurrentTerrain->platforms[index].bounce_damping = damping;
        }
    }
}
