typedef struct TERRAIN_DISPLAY_SCENE TERRAIN_DISPLAY_SCENE;
typedef struct TERRAIN_DISPLAY_OBJECT TERRAIN_DISPLAY_OBJECT;
typedef struct TERRAIN_GSCENE TERRAIN_GSCENE;
typedef struct TERRAIN_SCENE_OBJECT TERRAIN_SCENE_OBJECT;
typedef struct TERRAIN_FILE_CHUNK TERRAIN_FILE_CHUNK;
typedef struct TERRAIN_WALL_POINT TERRAIN_WALL_POINT;
typedef struct TERRAIN_SPATIAL_CHUNK TERRAIN_SPATIAL_CHUNK;
typedef struct TERRAIN_SPATIAL_NODE TERRAIN_SPATIAL_NODE;
typedef struct TERRAIN_SHAPE_BATCH TERRAIN_SHAPE_BATCH;
#include "saga.h"

/* Local PC accessed cohorts; historical whole owners remain unclaimed. */
typedef struct TERRAIN_SKIN_STREAM_VIEW_PC TERRAIN_SKIN_STREAM_VIEW_PC;

typedef union TERRAIN_SCAN_WORD_PC {
    struct {
        i16 shape_count;
        i16 terrain_group;
    } header;
    TERRAIN_POLYGON_INFO_PC *shape;
    u32 opaque_bits;
} TERRAIN_SCAN_WORD_PC;

typedef struct TERRAIN_SHADOW_QUERY_VIEW_PC {
    u8 unknown_000[0x148];
    TERRAIN_SCAN_WORD_PC scan_words[512];
} TERRAIN_SHADOW_QUERY_VIEW_PC;

typedef struct TERRAIN_INDEX_LEVEL_PC {
    i16 cache_age;
    u8 unknown_02[2];
    f32 center_x;
    f32 center_z;
    TERRAIN_SCAN_WORD_PC scan_words[512];
} TERRAIN_INDEX_LEVEL_PC;

typedef struct TERRAIN_XZ_BOUNDS_VIEW_PC {
    f32 x;
    f32 y;
    f32 z;
} TERRAIN_XZ_BOUNDS_VIEW_PC;

typedef struct TERRAIN_GROUP_PC {
    NUVEC origin;
    TERRAIN_SKIN_STREAM_VIEW_PC *data;
    TERRAIN_XZ_BOUNDS_VIEW_PC bounds_min;
    TERRAIN_XZ_BOUNDS_VIEW_PC bounds_max;
    u32 platform_flags;
    i32 chunk_type;
    i16 scene_index;
    i16 group_info;
    f32 radius;
} TERRAIN_GROUP_PC;

typedef struct TERRAIN_PREVIOUS_MATRIX_VIEW_PC {
    u8 unknown_00[0x30];
    f32 m30;
    f32 m31;
    f32 m32;
    u8 unknown_3c[4];
} TERRAIN_PREVIOUS_MATRIX_VIEW_PC;

typedef union TERRAIN_SCENE_STATE_VIEW_PC {
    u32 flags;
    u8 low_flags;
} TERRAIN_SCENE_STATE_VIEW_PC;

typedef struct TERRAIN_PLATFORM_PC {
    TERRAIN_PREVIOUS_MATRIX_VIEW_PC previous_matrix;
    void *scene_object;
    u32 unknown_44;
    i16 terrain_group_index;
    i16 scene_object_index;
    u32 flags;
    u8 unknown_50[4];
    f32 bounce_54;
    f32 bounce_58;
    f32 bounce_5c;
    f32 bounce_60;
    f32 bounce_64;
    void *scene_transform;
} TERRAIN_PLATFORM_PC;

typedef struct TERRAIN_CELL_PC {
    i16 first_group;
    i16 group_count;
    f32 min_x;
    f32 min_z;
    f32 max_x;
    f32 max_z;
} TERRAIN_CELL_PC;

typedef struct TERRAIN_SET_PC {
    TERRAIN_GROUP_PC *groups;
    u8 unknown_004[0x60];
    void *field_064;
    TERRAIN_PLATFORM_PC *platforms;
    i16 active_platform_groups[96];
    i32 active_platform_count;
    f32 platform_query_min_x;
    u8 unknown_134[4];
    f32 platform_query_min_z;
    f32 platform_query_max_x;
    u8 unknown_140[4];
    f32 platform_query_max_z;
    struct TERRAIN_SPATIAL_NODE *spatial_nodes;
    i16 group_count;
    u16 file_version;
    i32 group_index_count;
    i32 removed_platform_count;
    i16 removed_platforms[32];
    TERRAIN_TRACK_SLOT_PC track_slots[64];
    TERRAIN_CELL_PC cells[256];
    TERRAIN_CELL_PC platform_cell;
    i32 minimum_height;
    i32 used_cell_count;
    i16 *group_indices;
    TERRAIN_INDEX_LEVEL_PC index_levels[17];
    i32 max_group_indices;
    i32 max_groups;
    i32 max_platforms;
} TERRAIN_SET_PC;

STATIC_ASSERT(sizeof(TERRAIN_SCAN_WORD_PC) == 4);
STATIC_ASSERT(offsetof(TERRAIN_SHADOW_QUERY_VIEW_PC, scan_words) == 0x148);
STATIC_ASSERT(sizeof(TERRAIN_SHADOW_QUERY_VIEW_PC) == 0x948);
STATIC_ASSERT(sizeof(TERRAIN_INDEX_LEVEL_PC) == 0x80c);
STATIC_ASSERT(offsetof(TERRAIN_INDEX_LEVEL_PC, cache_age) == 0);
STATIC_ASSERT(offsetof(TERRAIN_INDEX_LEVEL_PC, center_x) == 4);
STATIC_ASSERT(offsetof(TERRAIN_INDEX_LEVEL_PC, center_z) == 8);
STATIC_ASSERT(offsetof(TERRAIN_INDEX_LEVEL_PC, scan_words) == 0xc);
STATIC_ASSERT(sizeof(TERRAIN_XZ_BOUNDS_VIEW_PC) == 0xc);
STATIC_ASSERT(sizeof(TERRAIN_PREVIOUS_MATRIX_VIEW_PC) == 0x40);
STATIC_ASSERT(offsetof(TERRAIN_PREVIOUS_MATRIX_VIEW_PC, m30) == 0x30);
STATIC_ASSERT(offsetof(TERRAIN_PREVIOUS_MATRIX_VIEW_PC, m32) == 0x38);
STATIC_ASSERT(sizeof(TERRAIN_SCENE_STATE_VIEW_PC) == 4);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, previous_matrix) == 0);
STATIC_ASSERT(offsetof(TERRAIN_CELL_PC, min_x) == 4);
STATIC_ASSERT(offsetof(TERRAIN_CELL_PC, min_z) == 8);
STATIC_ASSERT(offsetof(TERRAIN_CELL_PC, max_x) == 0xc);
STATIC_ASSERT(offsetof(TERRAIN_CELL_PC, max_z) == 0x10);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, active_platform_groups) == 0x6c);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, active_platform_count) == 0x12c);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, platform_query_min_x) == 0x130);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, platform_query_min_z) == 0x138);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, platform_query_max_x) == 0x13c);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, platform_query_max_z) == 0x144);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, used_cell_count) == 0x1bb0);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, index_levels) == 0x1bb8);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, max_group_indices) == 0xa484);
STATIC_ASSERT(sizeof(TERRAIN_GROUP_PC) == 0x38);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, data) == 0xc);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, bounds_min) == 0x10);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, bounds_max) == 0x1c);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, chunk_type) == 0x2c);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, scene_index) == 0x30);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, radius) == 0x34);
STATIC_ASSERT(sizeof(TERRAIN_PLATFORM_PC) == 0x6c);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, scene_object) == 0x40);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, terrain_group_index) == 0x48);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, scene_object_index) == 0x4a);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, flags) == 0x4c);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, bounce_54) == 0x54);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, scene_transform) == 0x68);
STATIC_ASSERT(sizeof(TERRAIN_TRACK_SLOT_PC) == 0x18);
STATIC_ASSERT(offsetof(TERRAIN_TRACK_SLOT_PC, platform_index) == 0x4);
STATIC_ASSERT(sizeof(TERRAIN_CELL_PC) == 0x14);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, platforms) == 0x68);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, group_count) == 0x14c);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, group_index_count) == 0x150);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, track_slots) == 0x198);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, cells) == 0x798);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, platform_cell) == 0x1b98);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, group_indices) == 0x1bb4);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, max_group_indices) == 0xa484);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, max_groups) == 0xa488);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, max_platforms) == 0xa48c);
STATIC_ASSERT(sizeof(TERRAIN_SET_PC) == 0xa490);

typedef TERRAIN_SET_PC TERRSET;
typedef TERRAIN_GROUP_PC TERRAIN_GROUP;
typedef TERRAIN_PLATFORM_PC TERRAIN_PLATFORM;
typedef TERRAIN_CELL_PC TERRAIN_CELL;
typedef TERRAIN_POLYGON_INFO_PC TERRAIN_SHAPE;

struct TERRAIN_SHAPE_BATCH {
    i16 marker, shape_count;
    f32 min_x, max_x, min_z, max_z;
};
struct TERRAIN_SPATIAL_NODE {
    u16 point_count, field_02;
    NUVEC points[1];
};
struct TERRAIN_SPATIAL_CHUNK {
    TERRAIN_SPATIAL_NODE *previous;
    TERRAIN_SPATIAL_NODE node;
};
struct TERRAIN_WALL_POINT {
    NUVEC position;
    u8 material[4];
};
struct TERRAIN_FILE_CHUNK {
    i16 chunk_count;
    u16 version;
    u32 data_word_count;
    NUVEC origin;
    i16 chunk_type;
    u16 field_16;
    u8 unknown_18[0x14];
    u32 platform_flags;
    u16 field_30, group_info;
};
STATIC_ASSERT(sizeof(TERRAIN_SHAPE_BATCH) == 0x14);
STATIC_ASSERT(sizeof(TERRAIN_FILE_CHUNK) == 0x34);
STATIC_ASSERT(offsetof(TERRAIN_FILE_CHUNK, chunk_type) == 0x14);
STATIC_ASSERT(offsetof(TERRAIN_FILE_CHUNK, platform_flags) == 0x2c);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, field_064) == 0x64);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, spatial_nodes) == 0x148);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, file_version) == 0x14e);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, removed_platform_count) == 0x154);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, minimum_height) == 0x1bac);
STATIC_ASSERT(offsetof(TERRAIN_TRACK_SLOT_PC, wall_contact_state) == 0xa);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, platform_flags) == 0x28);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, group_info) == 0x32);
STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, flags) == 0x62);
STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, normal_flags) == 0x63);
enum {
    TERRAIN_CHUNK_GROUP_PRIMARY = 0,
    TERRAIN_CHUNK_GROUP_SECONDARY = 1,
    TERRAIN_CHUNK_SPATIAL_INDEX = 2,
    TERRAIN_CHUNK_CRASH_DATA = 3,
    TERRAIN_INDEX_LEVEL_COUNT = 17,
    TERRAIN_CELL_RECORD_COUNT = 257,
    TERRAIN_TRACK_SLOT_COUNT = 64,
    TERRAIN_GRID_WIDTH = 7,
    TERRAIN_GRID_CELL_COUNT = 49,
    TERRAIN_PLATFORM_CELL = 256,
    TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED = 4,
    TERRAIN_TRACK_CONTACT_NONE = 0,
    TERRAIN_SHAPE_FLAG_MATERIAL_ALIAS = 0x80,
    TERRAIN_SHAPE_FLAG_MATERIAL_HIGH_BIT = 0x40,
    TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP = 1,
    TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP = 2
};
typedef u32 usize;
struct TERRAIN_SCENE_OBJECT {
    u8 pad_0x00[0x44];
    u8 transform[4];
    void *object;
    u8 pad_0x4c[4];
};
STATIC_ASSERT(sizeof(TERRAIN_SCENE_OBJECT) == 0x50);

struct TERRAIN_DISPLAY_OBJECT {
    u8 pad_0x00[0x40];
    u8 embedded_object[0x78];
    u8 transform[8];
    i32 scene_object_index;
    void *object;
    u8 pad_0xc8[8];
};
STATIC_ASSERT(sizeof(TERRAIN_DISPLAY_OBJECT) == 0xd0);
STATIC_ASSERT(offsetof(TERRAIN_DISPLAY_OBJECT, transform) == 0xb8);
STATIC_ASSERT(offsetof(TERRAIN_DISPLAY_OBJECT, scene_object_index) == 0xc0);

struct TERRAIN_DISPLAY_SCENE {
    u8 pad_0x00[0x6c];
    i32 object_count;
    TERRAIN_DISPLAY_OBJECT *objects;
};
STATIC_ASSERT(offsetof(TERRAIN_DISPLAY_SCENE, object_count) == 0x6c);

struct TERRAIN_GSCENE {
    u8 pad_0x00[0x1c];
    i32 object_count;
    TERRAIN_SCENE_OBJECT *objects;
    u8 pad_0x24[0x34];
    void *object_map_present;
    u16 *object_map;
    u8 pad_0x60[0xb0];
    TERRAIN_DISPLAY_SCENE *display_scene;
};
STATIC_ASSERT(offsetof(TERRAIN_GSCENE, object_count) == 0x1c);
STATIC_ASSERT(offsetof(TERRAIN_GSCENE, object_map_present) == 0x58);
STATIC_ASSERT(offsetof(TERRAIN_GSCENE, display_scene) == 0x110);

typedef union TERRAIN_FLOAT_BITS_PC {
    f32 value;
    u32 bits;
} TERRAIN_FLOAT_BITS_PC;

void __cdecl TerrainSetCur(void *terrain) {
    CurrentTerrain = terrain;
}

void *__cdecl TerrainGetCur(void) {
    return CurrentTerrain;
}

i32 __cdecl PlatInstGetHit(i32 index) {
    TERRAIN_SET_PC *terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    if (terrain == NULL || index < 0 || index >= terrain->max_platforms)
        return 0;
    return (terrain->platforms[index].flags >> 1) & 1;
}

#pragma auto_inline(off)
void __cdecl ScanTerrIDRemovePlat(i32 platform_index) {
    i32 i;

    for (i = 0; i < 64; i++) {
        if (((TERRAIN_SET_PC *)CurrentTerrain)->track_slots[i].platform_index == platform_index)
            ((TERRAIN_SET_PC *)CurrentTerrain)->track_slots[i].id = NULL;
    }
}
#pragma auto_inline(on)

#define TERRAIN_NPI_SET ((TERRAIN_SET_PC *)CurrentTerrain)
i32 __cdecl NewPlatInst(void *object, i32 instance) {
    TERRAIN_SET_PC *terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    i32 group_count;
    i32 new_index;
    i32 source_index;
    i32 i;

    if (terrain == NULL)
        return -1;
    if (terrain->group_index_count >= terrain->max_group_indices)
        return -1;
    group_count = terrain->group_count;
    if (group_count >= terrain->max_groups || object == NULL)
        return -1;

    new_index = -1;
    for (i = 0; i < terrain->max_platforms; i++) {
        if (terrain->platforms[i].scene_object == NULL) {
            new_index = i;
            break;
        }
    }
    if (new_index == -1)
        return -1;

    source_index = -1;
    for (i = 0; i < terrain->max_platforms; i++) {
        if (terrain->platforms[i].scene_object != NULL && terrain->platforms[i].scene_object_index == instance) {
            source_index = i;
            break;
        }
    }
    if (source_index == -1)
        return -1;

    TERRAIN_NPI_SET->groups[group_count] =
        TERRAIN_NPI_SET->groups[terrain->platforms[source_index].terrain_group_index];
    TERRAIN_NPI_SET->groups[TERRAIN_NPI_SET->group_count].scene_index = (i16)new_index;
    TERRAIN_NPI_SET->groups[TERRAIN_NPI_SET->group_count].chunk_type = 1;
    TERRAIN_NPI_SET->platforms[new_index].scene_object = object;
    TERRAIN_NPI_SET->platforms[new_index].terrain_group_index = TERRAIN_NPI_SET->group_count;
    TERRAIN_NPI_SET->platforms[new_index].scene_object_index = (i16)instance;
    TERRAIN_NPI_SET->platforms[new_index].scene_transform = NULL;
    TERRAIN_NPI_SET->platforms[new_index].flags ^=
        (TERRAIN_NPI_SET->platforms[source_index].flags ^ TERRAIN_NPI_SET->platforms[new_index].flags) & 1;
    TERRAIN_NPI_SET->group_indices[TERRAIN_NPI_SET->group_index_count] = TERRAIN_NPI_SET->group_count;
    TERRAIN_NPI_SET->group_index_count++;
    TERRAIN_NPI_SET->platforms[new_index].bounce_54 = 0.0f;
    TERRAIN_NPI_SET->platforms[new_index].bounce_58 = 0.0f;
    TERRAIN_NPI_SET->platforms[new_index].bounce_5c = 0.0f;
    TERRAIN_NPI_SET->platforms[new_index].bounce_64 = 0.0f;
    TERRAIN_NPI_SET->platforms[new_index].bounce_60 = 0.0f;
    TERRAIN_NPI_SET->group_count++;
    TERRAIN_NPI_SET->platform_cell.group_count++;
    return new_index;
}
#undef TERRAIN_NPI_SET

#define TERRAIN_DP_SET ((TERRAIN_SET_PC *)CurrentTerrain)
i32 __cdecl DeletePlatinst(i32 platform_index) {
    i32 i;
    i32 cell_group_index;

    if (platform_index < 0 || platform_index >= TERRAIN_DP_SET->max_platforms)
        return 0;
    if (TERRAIN_DP_SET->platforms[platform_index].scene_object == NULL)
        return 0;

    TERRAIN_DP_SET->group_count--;
    TERRAIN_DP_SET->group_index_count--;
    TERRAIN_DP_SET->platforms[TERRAIN_DP_SET->groups[TERRAIN_DP_SET->group_count].scene_index].terrain_group_index =
        TERRAIN_DP_SET->platforms[platform_index].terrain_group_index;
    TERRAIN_DP_SET->groups[TERRAIN_DP_SET->platforms[platform_index].terrain_group_index] =
        TERRAIN_DP_SET->groups[TERRAIN_DP_SET->group_count];
    TERRAIN_DP_SET->groups[TERRAIN_DP_SET->group_count].chunk_type = -1;
    TERRAIN_DP_SET->platforms[platform_index].scene_object = NULL;

    cell_group_index = TERRAIN_DP_SET->platform_cell.first_group;
    for (i = 0; i < TERRAIN_DP_SET->platform_cell.group_count; i++, cell_group_index++) {
        if (TERRAIN_DP_SET->group_indices[cell_group_index] == TERRAIN_DP_SET->group_count) {
            TERRAIN_DP_SET->group_indices[cell_group_index] =
                TERRAIN_DP_SET->group_indices[TERRAIN_DP_SET->group_index_count];
            break;
        }
    }
    TERRAIN_DP_SET->platform_cell.group_count--;
    ScanTerrIDRemovePlat(platform_index);
    return 1;
}
#undef TERRAIN_DP_SET

i32 __cdecl ShadowInfo(void) {
    return ShadPoly != NULL ? ShadPoly->material[0] : -1;
}

i32 __cdecl EShadowInfo(void) {
    return EShadowPolygonPc != NULL ? EShadowPolygonPc->material[1] : -1;
}

i32 __cdecl ShadowRoofInfo(void) {
    return ShadowRoofPolygonPc != NULL ? ShadowRoofPolygonPc->material[0] : -1;
}

i32 __cdecl NewShadowOnPlatform(void) {
    TERRAIN_SET_PC *terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    TERRAIN_GROUP_PC *group;
    if (terrain == NULL || TerrainShadowGroupIndexPc == -1)
        return -1;
    group = &terrain->groups[TerrainShadowGroupIndexPc];
    if (group->chunk_type != 1)
        return -1;
    return group->scene_index;
}

typedef struct TERRAIN_SKIN_BLEND_VIEW_PC {
    NUMTX end_matrix;    /* +00 */
    NUVEC row_amplitude; /* +40 */
    u8 opaque4c_50[4];
    NUVEC translation_amplitude; /* +50 */
} TERRAIN_SKIN_BLEND_VIEW_PC;

typedef struct TERRAIN_SKIN_INFO_VIEW_PC {
    NUMTX *matrix;                               /* +00 */
    const TERRAIN_SKIN_BLEND_VIEW_PC *blend;     /* +04 */
    const NUVEC4 *sample_rows;                   /* +08: sampled row banks */
    f32 scale;                                   /* +0c */
    TERRAIN_SKIN_STREAM_VIEW_PC *original_shape; /* +10 */
    i16 group_index;                             /* +14 */
    i16 pool_slot;                               /* +16 */
    i16 mode;                                    /* +18 */
    i16 vertex_order;                            /* +1a */
} TERRAIN_SKIN_INFO_VIEW_PC;

typedef struct TERRAIN_SKIN_USE_RECORD_PC {
    i32 last_use;
    i16 skin_index;
    u8 unknown_06[2];
} TERRAIN_SKIN_USE_RECORD_PC;
STATIC_ASSERT(sizeof(TERRAIN_SKIN_INFO_VIEW_PC) == 0x1c);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_INFO_VIEW_PC, original_shape) == 0x10);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_INFO_VIEW_PC, group_index) == 0x14);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_INFO_VIEW_PC, pool_slot) == 0x16);
STATIC_ASSERT(sizeof(TERRAIN_SKIN_USE_RECORD_PC) == 8);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_USE_RECORD_PC, last_use) == 0);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_USE_RECORD_PC, skin_index) == 4);

typedef struct TERRAIN_SKIN_BATCH_VIEW_PC {
    i16 kind;
    i16 count;
    f32 min_x, max_x, min_z, max_z;
} TERRAIN_SKIN_BATCH_VIEW_PC;

typedef TERRAIN_POLYGON_INFO_PC TERRAIN_SKIN_POLYGON_VIEW_PC;

/* Raw records remain variable-length. These named views expose the observed
 * 20-byte header followed by count 100-byte polygons; they are not an owner. */
struct TERRAIN_SKIN_STREAM_VIEW_PC {
    TERRAIN_SKIN_BATCH_VIEW_PC header;
    TERRAIN_SKIN_POLYGON_VIEW_PC polygons[1];
};

extern const f32 TerrainSkinBoundsMaxPc;
extern const f32 TerrainSkinBoundsMinPc;

/* Explicit output-pointer interface preserves the verified PC five-word ABI.
 * Android's aggregate return is a source candidate, not a historical PC type. */
NUVEC *TerrainSkin(NUVEC *result, const TERRAIN_SKIN_INFO_VIEW_PC *info, const NUVEC *position, f32 weight, i32 mode) {
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
                if (weight < 0.0f)
                    weight = 0.0f;
            }
            if (weight > 1.0f)
                weight = 1.0f;
            point.x = position->x;
            point.y = position->y;
            rows = info->sample_rows;
            if (rows != NULL)
                weight = (i32)(weight * 8.0 + 0.5) * 0.125;
            else
                weight = (i32)(weight * 10.0 + 0.5) / 10.0;
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
                const TERRAIN_SKIN_BLEND_VIEW_PC *blend = info->blend;
                i32 angle = (i32)(weight * 32768.0);
#define TERRAIN_SKIN_ROW(M, A)                                                                                         \
    matrix.M = (weight * blend->end_matrix.M + start->M * (1.0f - weight)) *                                           \
               (NU_SIN_LUT(angle) * blend->row_amplitude.A + 1.0f)
#define TERRAIN_SKIN_MOVE(M, A)                                                                                        \
    matrix.M =                                                                                                         \
        weight * blend->end_matrix.M + start->M * (1.0f - weight) + NU_SIN_LUT(angle) * blend->translation_amplitude.A
                TERRAIN_SKIN_ROW(m00, x);
                TERRAIN_SKIN_ROW(m01, x);
                TERRAIN_SKIN_ROW(m02, x);
                TERRAIN_SKIN_ROW(m03, x);
                TERRAIN_SKIN_ROW(m10, y);
                TERRAIN_SKIN_ROW(m11, y);
                TERRAIN_SKIN_ROW(m12, y);
                TERRAIN_SKIN_ROW(m13, y);
                TERRAIN_SKIN_ROW(m20, z);
                TERRAIN_SKIN_ROW(m21, z);
                TERRAIN_SKIN_ROW(m22, z);
                TERRAIN_SKIN_ROW(m23, z);
                TERRAIN_SKIN_MOVE(m30, x);
                TERRAIN_SKIN_MOVE(m31, y);
                TERRAIN_SKIN_MOVE(m32, z);
#undef TERRAIN_SKIN_ROW
#undef TERRAIN_SKIN_MOVE
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

#define TERRAIN_SKIN_NORMAL(normal, a, b)                                                                              \
    do {                                                                                                               \
        NUVEC cross;                                                                                                   \
        f32 length, inverse;                                                                                           \
        cross.x = (a).y * (b).z - (a).z * (b).y;                                                                       \
        cross.y = (a).z * (b).x - (a).x * (b).z;                                                                       \
        cross.z = (a).x * (b).y - (a).y * (b).x;                                                                       \
        *(normal) = cross;                                                                                             \
        length = NuFsqrt((normal)->x * (normal)->x + (normal)->y * (normal)->y + (normal)->z * (normal)->z);           \
        inverse = length != 0.0f ? 1.0f / length : 0.0f;                                                               \
        (normal)->x = (normal)->x * inverse;                                                                           \
        (normal)->y = (normal)->y * inverse;                                                                           \
        (normal)->z = (normal)->z * inverse;                                                                           \
    } while (0)
#define TERRAIN_SKIN_BOUND(value, lo, hi)                                                                              \
    do {                                                                                                               \
        if ((value) < (lo))                                                                                            \
            (lo) = (value);                                                                                            \
        if ((value) > (hi))                                                                                            \
            (hi) = (value);                                                                                            \
    } while (0)

/* Valid initialized variable records, vertex_order 0 or 1, and enough output
 * storage for each complete 20-byte header and 100-byte polygon. */
void SkinPlatform(TERRAIN_GROUP_PC *group, TERRAIN_SKIN_STREAM_VIEW_PC *output, const TERRAIN_SKIN_INFO_VIEW_PC *info) {
    TERRAIN_SKIN_STREAM_VIEW_PC *input;
    TERRAIN_SKIN_STREAM_VIEW_PC *cursor;
    TERRAIN_SKIN_POLYGON_VIEW_PC *source;
    TERRAIN_SKIN_POLYGON_VIEW_PC *polygon;
    NUVEC *p;
    NUVEC transformed;
    NUVEC a, b;
    i32 count;
    i32 vertex;
    f32 z;
    f32 min_x, min_y, min_z, max_x, max_y, max_z;
    f32 pmin_x, pmin_y, pmin_z, pmax_x, pmax_y, pmax_z;
    if (CurrentTerrain == NULL)
        return;
    if (group->chunk_type != 0 && group->chunk_type != 1)
        return;
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
            if (source->normals[1].y > 65535.0)
                vertex = 2;
            else
                vertex = 3;
            pmin_x = pmin_y = pmin_z = 123456789.0f;
            *polygon = *source;
            pmax_x = pmax_y = pmax_z = -123456789.0f;
            for (p = &polygon->vertices[vertex]; vertex >= 0; --vertex, --p) {
                *p = *TerrainSkin(&transformed, info, &source->vertices[SkinFlipTab[vertex + info->vertex_order * 4]],
                                  -1.0f, info->mode);
                p->x = p->x - info->matrix->m30;
                p->y = p->y - info->matrix->m31;
                p->z = p->z - info->matrix->m32;
                z = p->z;
                TERRAIN_SKIN_BOUND(p->x, min_x, max_x);
                TERRAIN_SKIN_BOUND(p->y, min_y, max_y);
                TERRAIN_SKIN_BOUND(z, min_z, max_z);
                TERRAIN_SKIN_BOUND(p->x, pmin_x, pmax_x);
                TERRAIN_SKIN_BOUND(p->y, pmin_y, pmax_y);
                TERRAIN_SKIN_BOUND(z, pmin_z, pmax_z);
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
                TERRAIN_SKIN_NORMAL(&polygon->normals[1], a, b);
            }
            a.x = polygon->vertices[2].x - polygon->vertices[0].x;
            a.y = polygon->vertices[2].y - polygon->vertices[0].y;
            a.z = polygon->vertices[2].z - polygon->vertices[0].z;
            b.x = polygon->vertices[1].x - polygon->vertices[0].x;
            b.y = polygon->vertices[1].y - polygon->vertices[0].y;
            b.z = polygon->vertices[1].z - polygon->vertices[0].z;
            TERRAIN_SKIN_NORMAL(&polygon->normals[0], a, b);
            ++source;
            ++polygon;
        }
        cursor->header.min_x = min_x - (f64)0.05f;
        cursor->header.max_x = max_x + (f64)0.05f;
        cursor->header.min_z = min_z - (f64)0.05f;
        cursor->header.max_z = max_z + (f64)0.05f;
        input = (TERRAIN_SKIN_STREAM_VIEW_PC *)source;
        cursor = (TERRAIN_SKIN_STREAM_VIEW_PC *)polygon;
    }
    cursor->header.kind = -1;
    cursor->header.count = -1;
    group->data = output;
}

#undef TERRAIN_SKIN_NORMAL
#undef TERRAIN_SKIN_BOUND

STATIC_ASSERT(sizeof(TERRAIN_SKIN_BATCH_VIEW_PC) == 0x14);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_STREAM_VIEW_PC, polygons) == 0x14);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_INFO_VIEW_PC, matrix) == 0);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_INFO_VIEW_PC, blend) == 4);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_INFO_VIEW_PC, sample_rows) == 8);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_INFO_VIEW_PC, scale) == 0xc);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_INFO_VIEW_PC, mode) == 0x18);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_INFO_VIEW_PC, vertex_order) == 0x1a);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_BLEND_VIEW_PC, row_amplitude) == 0x40);
STATIC_ASSERT(offsetof(TERRAIN_SKIN_BLEND_VIEW_PC, translation_amplitude) == 0x50);

/* Insert after skin-info/use-record declarations; storage root-owned.
 * All seven new views retain BLANK original CSVsize and ctype / zero credit.
 */
extern i32 TerrainSkinInfoCountPc;
extern TERRAIN_SKIN_INFO_VIEW_PC *TerrainSkinInfosPc;
extern u32 TerrainSkinUseEpochPc;
extern TERRAIN_SKIN_USE_RECORD_PC *TerrainSkinUseRecordsPc;
extern i32 TerrainSkinPoolSlotCountPc;
extern i32 TerrainSkinPoolSlotBytesPc;
extern u8 *TerrainSkinPoolBasePc;
/* Keep public CurrentTerrain/TerI as existing void* storage. Cast TerI locally
 * to TERRAIN_SHADOW_QUERY_VIEW_PC* for query.scan_words. Keep existing
 * PLATFORM.scene_object/scene_transform as void*, locally accessed through
 * NUMTX* / TERRAIN_SCENE_STATE_VIEW_PC*. No full scene-owner type transfer.
 */

/* Genuine C draft for insertion into existing nuterrain.c.
 * Qualified initialized skin records and pool storage; not allocation history.
 * Required source-view replacements are listed in the accompanying header file.
 * Source-convention void: original EAX values are ignored, not a recovered type.
 */
void TerrainSkinAllocate(TERRAIN_GROUP_PC *group) {
    i32 selected;
    i32 candidate;
    i32 oldest;
    i16 previous_skin;

    if (-1 - group->scene_index >= TerrainSkinInfoCountPc)
        return;

    ++TerrainSkinUseEpochPc;
    if (group->data != NULL) {
        TerrainSkinUseRecordsPc[TerrainSkinInfosPc[-1 - group->scene_index].pool_slot].last_use =
            (i32)TerrainSkinUseEpochPc;
        return;
    }

    selected = 0;
    if (TerrainSkinUseRecordsPc[0].skin_index != -1) {
        oldest = TerrainSkinUseRecordsPc[0].last_use;
        for (candidate = selected + 1; candidate < TerrainSkinPoolSlotCountPc; ++candidate) {
            if (TerrainSkinUseRecordsPc[candidate].skin_index == -1) {
                selected = candidate;
                break;
            }
            if (TerrainSkinUseRecordsPc[candidate].last_use < oldest) {
                oldest = TerrainSkinUseRecordsPc[candidate].last_use;
                selected = candidate;
            }
        }
    }

    previous_skin = TerrainSkinUseRecordsPc[selected].skin_index;
    if (previous_skin >= 0) {
        ((TERRAIN_SET_PC *)CurrentTerrain)->groups[TerrainSkinInfosPc[previous_skin].group_index].data = NULL;
        for (candidate = 0; candidate < 16; ++candidate)
            ((TERRAIN_SET_PC *)CurrentTerrain)->index_levels[candidate].cache_age = 0;
    }

    group->data = TerrainSkinInfosPc[-1 - group->scene_index].original_shape;
    TerrainSkinInfosPc[-1 - group->scene_index].pool_slot = (i16)selected;
    TerrainSkinUseRecordsPc[selected].skin_index = -1 - group->scene_index;
    SkinPlatform(group, (TERRAIN_SKIN_STREAM_VIEW_PC *)(TerrainSkinPoolBasePc + TerrainSkinPoolSlotBytesPc * selected),
                 &TerrainSkinInfosPc[-(group->scene_index + 1)]);
    TerrainSkinUseRecordsPc[selected].last_use = (i32)TerrainSkinUseEpochPc;
}

extern void *TerI;
extern TERRAIN_POLYGON_INFO_PC *TerrainScaledShapeCursorPc;
extern u16 TerrainPlatformsOffPc;
extern i16 TerrainShadowGroupIndexPc, TerrainShadowFloorIndexPc;
extern TERRAIN_POLYGON_INFO_PC *TerrainCastAuxPolygonPc;
extern f32 TerrainShadowHeightPc;
extern NUVEC TerrainCastAuxNormalPc;
extern u16 TerrainCastAuxValidPc;
extern i16 TerrainCastAuxIndexPc;
extern u16 TerrainWaterCastIndexPc, TerrainRoofValidPc;
extern i16 TerrainRoofIndexPc;
extern NUVEC TerrainRoofNormalPc;
extern const f64 TerrainScanRadiusEpsilon;
extern const f32 TerrainScanTriangleSentinel, TerrainCastLowerSentinel;
/* Existing canonical globals: CurrentTerrain(void*), ShadPoly,
 * EShadowPolygonPc, ShadowRoofPolygonPc, EShadY,
 * GetSurfaceInfoRoofHeightPc, GetSurfaceInfoSurfaceNormalPc,
 * GetSurfaceInfoWaterNormalPc, TerrainSurfaceClassPc.
 * Reuse existing BoltTargetRandomDenominator(f64)/LevelConfigFarDistance(f32).
 */

extern u16 TerrainSurfaceClassPc;

typedef struct TERRAIN_SCAN_WRITER_PC {
    TERRAIN_SCAN_WORD_PC *group_header;
    TERRAIN_SCAN_WORD_PC *cursor;
    TERRAIN_SCAN_WORD_PC *limit;
    i32 shape_count;
} TERRAIN_SCAN_WRITER_PC;

#define TERRAIN_FINISH_GROUP(w, group_index)                                                                           \
    do {                                                                                                               \
        if ((w).shape_count > 0) {                                                                                     \
            (w).group_header->header.shape_count = (i16)(w).shape_count;                                               \
            (w).group_header->header.terrain_group = (i16)(group_index);                                               \
            (w).group_header = (w).cursor;                                                                             \
            (w).cursor++;                                                                                              \
            (w).shape_count = 0;                                                                                       \
        }                                                                                                              \
    } while (0)
#define TERRAIN_CROSS(dst, a, b)                                                                                       \
    do {                                                                                                               \
        (dst).x = (a).y * (b).z - (a).z * (b).y;                                                                       \
        (dst).y = (a).z * (b).x - (b).z * (a).x;                                                                       \
        (dst).z = (a).x * (b).y - (a).y * (b).x;                                                                       \
    } while (0)
#define SHADOW_BOUNDS_OVERLAP(ax, az, bx, bz, lo, hi)                                                                  \
    ((bx) >= (lo).x && (hi).x > (ax) && (bz) >= (lo).z && (hi).z > (az))

#define TERRAIN_ROT_SET ((TERRAIN_SET_PC *)CurrentTerrain)
#define TERRAIN_ROT_PLATFORM(group_ptr) (TERRAIN_ROT_SET->platforms[(group_ptr)->scene_index])
#define TERRAIN_ROT_MATRIX(group_ptr) ((NUMTX *)TERRAIN_ROT_PLATFORM(group_ptr).scene_object)
#define TERRAIN_ROT_GROUP_OVERLAP(lo_x, lo_z, hi_x, hi_z, group_ptr)                                                   \
    ((hi_x) >= (group_ptr)->bounds_min.x && (hi_z) >= (group_ptr)->bounds_min.z &&                                     \
     (group_ptr)->bounds_max.x >= (lo_x) && (group_ptr)->bounds_max.z > (lo_z))
#define TERRAIN_ROT_SHAPE_OVERLAP(shape_ptr)                                                                           \
    (local_max_x >= (shape_ptr)->min_x && (shape_ptr)->max_x > local_min_x && local_max_z >= (shape_ptr)->min_z &&     \
     (shape_ptr)->max_z > local_min_z)
#define TERRAIN_ROT_MATERIAL_OK(shape_ptr)                                                                             \
    ((shape_ptr)->material[1] == 0 || ((shape_ptr)->material[1] & terrain_mask) != 0)
#define TERRAIN_ROT_FINISH_GROUP(group_index_arg)                                                                      \
    do {                                                                                                               \
        if (shape_count != 0) {                                                                                        \
            group_header->header.shape_count = (i16)shape_count;                                                       \
            group_header->header.terrain_group = (i16)(group_index_arg);                                               \
            group_header = cursor;                                                                                     \
            cursor++;                                                                                                  \
            shape_count = 0;                                                                                           \
        }                                                                                                              \
    } while (0)
#define TERRAIN_ROT_LOCAL_BOUNDS(group_ptr)                                                                            \
    do {                                                                                                               \
        local_max_x = max_x - (group_ptr)->origin.x;                                                                   \
        local_max_z = max_z - (group_ptr)->origin.z;                                                                   \
        local_min_x = min_x - (group_ptr)->origin.x;                                                                   \
        local_min_z = min_z - (group_ptr)->origin.z;                                                                   \
    } while (0)

#define TERRAIN_ROT_CROSS(dst, a, b)                                                                                   \
    do {                                                                                                               \
        NUVEC cross;                                                                                                   \
        cross.x = (a)->y * (b)->z - (a)->z * (b)->y;                                                                   \
        cross.y = (a)->z * (b)->x - (a)->x * (b)->z;                                                                   \
        cross.z = (a)->x * (b)->y - (a)->y * (b)->x;                                                                   \
        (dst) = cross;                                                                                                 \
    } while (0)

void NewScanRot(NUVEC *position, i32 terrain_mask) {
    i32 transformed_count = 0;
    NUVEC4 *vertices;
    NUVEC saved_position;
    TERRAIN_GROUP_PC *groups;
    i32 hit_index;
    i32 oldest_index;
    i32 oldest_age;
    i32 i;
    TERRAIN_INDEX_LEVEL_PC *cache;
    TERRAIN_SCAN_WORD_PC *group_header;
    TERRAIN_SCAN_WORD_PC *cursor;
    TERRAIN_SCAN_WORD_PC *limit;
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
        TERRAIN_INDEX_LEVEL_PC *level = &TERRAIN_ROT_SET->index_levels[i];
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
    groups = TERRAIN_ROT_SET->groups;
    if (hit_index >= 0) {
        TERRAIN_SCAN_WORD_PC *entry;

        cache = &TERRAIN_ROT_SET->index_levels[hit_index];
        group_header = ((TERRAIN_SHADOW_QUERY_VIEW_PC *)TerI)->scan_words;
        max_x = saved_position.x + 0.10000000149011612;
        max_z = saved_position.z + 0.10000000149011612;
        min_x = saved_position.x - 0.10000000149011612;
        min_z = saved_position.z - 0.10000000149011612;
        cache->cache_age = 8;
        cursor = group_header + 1;
        limit = cursor + 508;
        shape_count = 0;
        entry = cache->scan_words;
        while (entry->header.shape_count > 0) {
            const i16 count = entry->header.shape_count;
            const i32 group_index = entry->header.terrain_group;
            TERRAIN_SCAN_WORD_PC *shapes = entry + 1;
            TERRAIN_GROUP_PC *group = &groups[group_index];
            f32 local_min_x, local_max_x;
            f32 local_min_z, local_max_z;
            i16 remaining;

            if (TERRAIN_ROT_GROUP_OVERLAP(min_x, min_z, max_x, max_z, group) && group->chunk_type != -1) {
                TERRAIN_ROT_LOCAL_BOUNDS(group);
                for (remaining = count; remaining > 0; --remaining, ++shapes) {
                    TERRAIN_POLYGON_INFO_PC *shape = shapes->shape;
                    if (TERRAIN_ROT_SHAPE_OVERLAP(shape) && cursor < limit && TERRAIN_ROT_MATERIAL_OK(shape)) {
                        (cursor++)->shape = shape;
                        ++shape_count;
                    }
                }
                TERRAIN_ROT_FINISH_GROUP(group_index);
            } else {
                shapes += count;
            }
            entry = shapes;
        }
    } else {
        i32 cell_index;

        cache = &TERRAIN_ROT_SET->index_levels[oldest_index];
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
            group_header = ((TERRAIN_SHADOW_QUERY_VIEW_PC *)TerI)->scan_words;
        }
        cursor = group_header + 1;
        limit = cursor + 508;
        shape_count = 0;
        for (cell_index = 0; cell_index < TERRAIN_ROT_SET->used_cell_count; ++cell_index) {
            const TERRAIN_CELL_PC *cell = &TERRAIN_ROT_SET->cells[cell_index];
            const i16 *group_indices;
            i32 cell_group;

            if (!(max_x >= cell->min_x && cell->max_x >= min_x && max_z >= cell->min_z && cell->max_z >= min_z))
                continue;
            group_indices = TERRAIN_ROT_SET->group_indices + cell->first_group;
            for (cell_group = 0; cell_group < cell->group_count; ++cell_group) {
                TERRAIN_GROUP_PC *group = &groups[group_indices[cell_group]];
                f32 local_min_x, local_max_x;
                f32 local_min_z, local_max_z;
                TERRAIN_SKIN_STREAM_VIEW_PC *batch;

                if (!TERRAIN_ROT_GROUP_OVERLAP(min_x, min_z, max_x, max_z, group) || group->chunk_type == -1)
                    continue;
                TERRAIN_ROT_LOCAL_BOUNDS(group);
                if (group->scene_index < 0)
                    TerrainSkinAllocate(group);
                batch = group->data;
                while (batch->header.kind >= 0) {
                    TERRAIN_POLYGON_INFO_PC *shape = batch->polygons;
                    const i32 count = batch->header.count;

                    if (local_max_x >= batch->header.min_x && batch->header.max_x > local_min_x &&
                        local_max_z >= batch->header.min_z && batch->header.max_z > local_min_z) {
                        i16 remaining;

                        for (remaining = count; remaining > 0; --remaining, ++shape) {
                            if (TERRAIN_ROT_SHAPE_OVERLAP(shape) && cursor < limit && TERRAIN_ROT_MATERIAL_OK(shape)) {
                                (cursor++)->shape = shape;
                                ++shape_count;
                            }
                        }
                    } else {
                        shape += count;
                    }
                    batch = (TERRAIN_SKIN_STREAM_VIEW_PC *)shape;
                }
                TERRAIN_ROT_FINISH_GROUP(group_indices[cell_group]);
            }
        }
        if (cache->cache_age < 2) {
            TERRAIN_SCAN_WORD_PC *entry;

            cache->cache_age = 8;
            cache->center_x = position->x;
            cache->center_z = position->z;
            group_header->header.shape_count = 0;
            group_header->header.terrain_group = 0;
            group_header = ((TERRAIN_SHADOW_QUERY_VIEW_PC *)TerI)->scan_words;
            cursor = group_header + 1;
            limit = cursor + 508;
            shape_count = 0;
            max_x = saved_position.x + 0.10000000149011612;
            max_z = saved_position.z + 0.10000000149011612;
            min_x = saved_position.x - 0.10000000149011612;
            min_z = saved_position.z - 0.10000000149011612;
            entry = cache->scan_words;
            while (entry->header.shape_count > 0) {
                const i16 count = entry->header.shape_count;
                const i32 group_index = entry->header.terrain_group;
                TERRAIN_SCAN_WORD_PC *shapes = entry + 1;
                TERRAIN_GROUP_PC *group = &groups[group_index];
                f32 local_min_x, local_max_x;
                f32 local_min_z, local_max_z;
                i16 remaining;

                if (TERRAIN_ROT_GROUP_OVERLAP(min_x, min_z, max_x, max_z, group) && group->chunk_type != -1) {
                    TERRAIN_ROT_LOCAL_BOUNDS(group);
                    for (remaining = count; remaining > 0; --remaining, ++shapes) {
                        TERRAIN_POLYGON_INFO_PC *shape = shapes->shape;
                        if (TERRAIN_ROT_SHAPE_OVERLAP(shape) && TERRAIN_ROT_MATERIAL_OK(shape) && cursor < limit) {
                            (cursor++)->shape = shape;
                            ++shape_count;
                        }
                    }
                    TERRAIN_ROT_FINISH_GROUP(group_index);
                } else {
                    shapes += count;
                }
                entry = shapes;
            }
        }
    }

    if (TerrainPlatformsOffPc == 0) {
        const i16 *platform_groups;
        i32 platform_count;

        min_x -= NuTimeBarTop;
        max_x += NuTimeBarTop;
        min_z -= NuTimeBarTop;
        max_z += NuTimeBarTop;
        if (min_x > TERRAIN_ROT_SET->platform_query_min_x && max_x < TERRAIN_ROT_SET->platform_query_max_x &&
            min_z > TERRAIN_ROT_SET->platform_query_min_z && max_z < TERRAIN_ROT_SET->platform_query_max_z) {
            platform_groups = TERRAIN_ROT_SET->active_platform_groups;
            platform_count = TERRAIN_ROT_SET->active_platform_count;
        } else {
            platform_groups = TERRAIN_ROT_SET->group_indices + TERRAIN_ROT_SET->platform_cell.first_group;
            platform_count = TERRAIN_ROT_SET->platform_cell.group_count;
        }
        for (; platform_count > 0; --platform_count, ++platform_groups) {
            TERRAIN_GROUP_PC *group = &groups[*platform_groups];
            TERRAIN_PLATFORM_PC *platform;
            f32 local_min_x, local_max_x;
            f32 local_min_z, local_max_z;
            TERRAIN_SKIN_STREAM_VIEW_PC *batch;

            if (group->origin.x - group->radius > max_x || group->radius + group->origin.x < min_x ||
                group->origin.z - group->radius > max_z || group->origin.z + group->radius < min_z)
                continue;
            platform = &TERRAIN_ROT_PLATFORM(group);
            if (platform->scene_object != NULL) {
                if (TERRAIN_ROT_MATRIX(group)->m30 > TERRAIN_ROT_PLATFORM(group).previous_matrix.m30) {
                    local_min_x = min_x - group->origin.x;
                    local_max_x =
                        (TERRAIN_ROT_MATRIX(group)->m30 - TERRAIN_ROT_PLATFORM(group).previous_matrix.m30) * 1.5 +
                        max_x - group->origin.x;
                } else {
                    local_max_x = max_x - group->origin.x;
                    local_min_x =
                        (TERRAIN_ROT_MATRIX(group)->m30 - TERRAIN_ROT_PLATFORM(group).previous_matrix.m30) * 1.5 +
                        min_x - group->origin.x;
                }
                if (TERRAIN_ROT_MATRIX(group)->m32 > TERRAIN_ROT_PLATFORM(group).previous_matrix.m32) {
                    local_min_z = min_z - group->origin.z;
                    local_max_z =
                        (TERRAIN_ROT_MATRIX(group)->m32 - TERRAIN_ROT_PLATFORM(group).previous_matrix.m32) * 1.5 +
                        max_z - group->origin.z;
                } else {
                    local_max_z = max_z - group->origin.z;
                    local_min_z =
                        (TERRAIN_ROT_MATRIX(group)->m32 - TERRAIN_ROT_PLATFORM(group).previous_matrix.m32) * 1.5 +
                        min_z - group->origin.z;
                }
            } else {
                local_max_x = max_x - group->origin.x;
                local_min_x = min_x - group->origin.x;
                local_max_z = max_z - group->origin.z;
                local_min_z = min_z - group->origin.z;
            }
            if ((platform->flags & 1) != 0) {
                f64 dz;
                f64 dx;
                f32 distance_squared;
                f64 reach;

                if (group->chunk_type == -1)
                    continue;
                dz = (min_z + max_z) * NuQFntHalfScale - group->origin.z;
                dx = (max_x + min_x) * NuQFntHalfScale - group->origin.x;
                reach = group->radius + TerrainScanRadiusEpsilon;
                distance_squared = dx * dx + dz * dz;
                if (!(reach * reach > distance_squared))
                    continue;
                if (platform->scene_transform != NULL) {
                    const TERRAIN_SCENE_STATE_VIEW_PC *scene_state = platform->scene_transform;
                    if ((platform->flags & 4) != 0) {
                        const u32 state_flags = scene_state->flags;
                        if ((state_flags & 2) == 0)
                            continue;
                    } else if ((scene_state->low_flags & 1) == 0) {
                        continue;
                    }
                }
                batch = group->data;
                while (batch->header.kind >= 0) {
                    TERRAIN_POLYGON_INFO_PC *shape = batch->polygons;
                    i32 remaining;

                    for (remaining = batch->header.count; remaining > 0; --remaining, ++shape) {
                        TERRAIN_POLYGON_INFO_PC *transformed;
                        f32 length_squared;
                        f32 length;
                        f32 inverse;

                        if (shape->material[1] != 0 && (shape->material[1] & terrain_mask) == 0)
                            continue;
                        if (cursor >= limit)
                            continue;
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
                        NuVec4MtxTransformVU0x3(vertices, vertices, TERRAIN_ROT_MATRIX(group));
                        if (shape->normals[1].y < BoltTargetRandomDenominator) {
                            vertices[3].x = shape->vertices[3].x;
                            vertices[3].y = shape->vertices[3].y;
                            vertices[3].z = shape->vertices[3].z;
                            vertices[3].w = 0.0f;
                            NuVec4MtxTransformVU0(&vertices[3], &vertices[3], TERRAIN_ROT_MATRIX(group));
                        } else {
                            vertices[3] = vertices[2];
                        }
                        if (!(vertices[0].x > local_min_x || vertices[1].x > local_min_x ||
                              vertices[2].x > local_min_x || vertices[3].x > local_min_x))
                            continue;
                        if (!(local_max_x > vertices[0].x || local_max_x > vertices[1].x ||
                              local_max_x > vertices[2].x || local_max_x > vertices[3].x))
                            continue;
                        if (!(vertices[0].z > local_min_z || vertices[1].z > local_min_z ||
                              vertices[2].z > local_min_z || vertices[3].z > local_min_z))
                            continue;
                        if (!(local_max_z > vertices[0].z || local_max_z > vertices[1].z ||
                              local_max_z > vertices[2].z || local_max_z > vertices[3].z))
                            continue;
                        transformed = &TerrainScaledShapeCursorPc[transformed_count];
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
                            TERRAIN_ROT_CROSS(transformed->normals[1], (NUVEC *)&vertices[0], (NUVEC *)&vertices[1]);
                            length_squared = (transformed->normals[1].x * transformed->normals[1].x +
                                              transformed->normals[1].y * transformed->normals[1].y) +
                                             transformed->normals[1].z * transformed->normals[1].z;
                            length = NuFsqrt(length_squared);
                            inverse = length == 0.0f ? 0.0f : 1.0f / length;
                            transformed = &TerrainScaledShapeCursorPc[transformed_count];
                            transformed->normals[1].x *= inverse;
                            transformed->normals[1].y *= inverse;
                            transformed->normals[1].z *= inverse;
                        } else {
                            transformed->normals[1].y = TerrainScanTriangleSentinel;
                        }
                        vertices[0].x = transformed->vertices[2].x - transformed->vertices[0].x;
                        vertices[0].y = transformed->vertices[2].y - transformed->vertices[0].y;
                        vertices[0].z = transformed->vertices[2].z - transformed->vertices[0].z;
                        vertices[1].x = transformed->vertices[1].x - transformed->vertices[0].x;
                        vertices[1].y = transformed->vertices[1].y - transformed->vertices[0].y;
                        vertices[1].z = transformed->vertices[1].z - transformed->vertices[0].z;
                        TERRAIN_ROT_CROSS(transformed->normals[0], (NUVEC *)&vertices[0], (NUVEC *)&vertices[1]);
                        length_squared = (transformed->normals[0].x * transformed->normals[0].x +
                                          transformed->normals[0].y * transformed->normals[0].y) +
                                         transformed->normals[0].z * transformed->normals[0].z;
                        length = NuFsqrt(length_squared);
                        inverse = length == 0.0f ? 0.0f : 1.0f / length;
                        transformed = &TerrainScaledShapeCursorPc[transformed_count];
                        transformed->normals[0].x *= inverse;
                        transformed->normals[0].y *= inverse;
                        transformed->normals[0].z *= inverse;
                        (cursor++)->shape = transformed;
                        ++shape_count;
                        ++transformed_count;
                    }
                    batch = (TERRAIN_SKIN_STREAM_VIEW_PC *)shape;
                }
                TERRAIN_ROT_FINISH_GROUP(*platform_groups);
            } else {
                if (!TERRAIN_ROT_GROUP_OVERLAP(local_min_x, local_min_z, local_max_x, local_max_z, group) ||
                    group->chunk_type == -1)
                    continue;
                if (platform->scene_transform != NULL) {
                    const TERRAIN_SCENE_STATE_VIEW_PC *scene_state = platform->scene_transform;
                    if ((platform->flags & 4) != 0) {
                        const u32 state_flags = scene_state->flags;
                        if ((state_flags & 2) == 0)
                            continue;
                    } else if ((scene_state->low_flags & 1) == 0) {
                        continue;
                    }
                }
                batch = group->data;
                while (batch->header.kind >= 0) {
                    TERRAIN_POLYGON_INFO_PC *shape = batch->polygons;
                    const i32 count = batch->header.count;

                    if (local_max_x >= batch->header.min_x && batch->header.max_x > local_min_x &&
                        local_max_z >= batch->header.min_z && batch->header.max_z > local_min_z) {
                        i16 remaining;

                        for (remaining = count; remaining > 0; --remaining, ++shape) {
                            if (TERRAIN_ROT_SHAPE_OVERLAP(shape) && cursor < limit && TERRAIN_ROT_MATERIAL_OK(shape)) {
                                (cursor++)->shape = shape;
                                ++shape_count;
                            }
                        }
                    } else {
                        shape += count;
                    }
                    batch = (TERRAIN_SKIN_STREAM_VIEW_PC *)shape;
                }
                TERRAIN_ROT_FINISH_GROUP(*platform_groups);
            }
        }
    }
    NuScratchRelease();
    group_header->header.shape_count = 0;
    group_header->header.terrain_group = 0;
}

f32 NewCast(NUVEC *position, f32 height_above, f32 roof_range) {
    i16 group_index;

    TERRAIN_SET_PC *terrain;
    i32 remaining;
    TERRAIN_SHADOW_QUERY_VIEW_PC *query;
    volatile f32 plane_quotient;
    TERRAIN_SCAN_WORD_PC *scan;
    TERRAIN_POLYGON_INFO_PC *ordinary_floor;
    TERRAIN_POLYGON_INFO_PC *ordinary_above;
    TERRAIN_POLYGON_INFO_PC *extended_above;
    TERRAIN_POLYGON_INFO_PC *extended_floor;
    f32 ordinary_floor_y;
    f32 extended_floor_y;
    f32 ordinary_above_y;
    f32 extended_above_y;
    i16 ordinary_above_group;
    i16 extended_above_group;
    NUVEC ordinary_floor_normal;
    NUVEC extended_floor_normal;
    NUVEC ordinary_above_normal;
    NUVEC extended_above_normal;
    i16 shape_count;

    terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    query = (TERRAIN_SHADOW_QUERY_VIEW_PC *)TerI;

    scan = query->scan_words;
    TerrainShadowGroupIndexPc = -1;
    TerrainShadowFloorIndexPc = -1;
    ordinary_above = NULL;
    ordinary_floor = NULL;
    extended_above = NULL;
    extended_floor = NULL;
    ordinary_floor_y = TerrainCastLowerSentinel;
    extended_floor_y = TerrainCastLowerSentinel;
    ordinary_above_y = LevelConfigFarDistance;
    extended_above_y = LevelConfigFarDistance;
    ordinary_above_group = -1;
    extended_above_group = -1;

    /* No artificial normal initialization: original above-normal locals have
     * no entry definition. Release is qualified to first accepted above-plane
     * height strictly below +2000000; exact-sentinel first ties remain held. */
    shape_count = scan->header.shape_count;
    for (;;) {
        f32 z;

        f32 x;

        TERRAIN_SCAN_WORD_PC *surfaces;
        TERRAIN_SCAN_WORD_PC *entry;

        if (shape_count <= 0) {
            if (!(LevelConfigFarDistance > extended_above_y) || !(extended_above_normal.y < 0.0f) ||
                !(roof_range + position->y > extended_above_y)) {
                TerrainCastAuxPolygonPc = NULL;
                TerrainShadowHeightPc = LevelConfigFarDistance;
            } else if (!(ordinary_above_y < LevelConfigFarDistance) || !(ordinary_above_normal.y < 0.0f) ||
                       !(roof_range + position->y > ordinary_above_y) || !(extended_above_y > ordinary_above_y)) {
                TerrainCastAuxPolygonPc = extended_above;
                TerrainShadowHeightPc = extended_above_y;
                TerrainCastAuxNormalPc = extended_above_normal;
                TerrainCastAuxValidPc = 1;
                TerrainCastAuxIndexPc = extended_above_group;
            } else {
                TerrainCastAuxPolygonPc = NULL;
                TerrainShadowHeightPc = LevelConfigFarDistance;
            }
            if (!(extended_above_y < LevelConfigFarDistance) || !(extended_above_normal.y > 0.0f) ||
                !(roof_range + position->y > extended_above_y)) {
                if (extended_floor_y <= TerrainCastLowerSentinel ||
                    (TerrainCastLowerSentinel < ordinary_floor_y && extended_floor_y < ordinary_floor_y)) {
                    TerrainWaterCastIndexPc = 3;
                    EShadY = LevelConfigFarDistance;
                    EShadowPolygonPc = NULL;
                    GetSurfaceInfoWaterNormalPc.y = 1.0f;
                } else {
                    TerrainWaterCastIndexPc = 2;
                    EShadY = extended_floor_y;
                    EShadowPolygonPc = extended_floor;
                    GetSurfaceInfoWaterNormalPc = extended_floor_normal;
                }
            } else if (!(LevelConfigFarDistance > ordinary_above_y) || !(ordinary_above_normal.y < 0.0f) ||
                       !(roof_range + position->y > ordinary_above_y) || !(extended_above_y > ordinary_above_y)) {
                EShadowPolygonPc = extended_above;
                EShadY = extended_above_y;
                GetSurfaceInfoWaterNormalPc = extended_above_normal;
                TerrainWaterCastIndexPc = 1;
                TerrainShadowFloorIndexPc = extended_above_group;
            } else {
                TerrainWaterCastIndexPc = 3;
                EShadY = LevelConfigFarDistance;
                GetSurfaceInfoWaterNormalPc.y = 1.0f;
                EShadowPolygonPc = NULL;
            }
            if (!(ordinary_above_y < LevelConfigFarDistance) || !(ordinary_above_normal.y < 0.0f) ||
                !(ordinary_above_y < roof_range + position->y)) {
                ShadowRoofPolygonPc = NULL;
                GetSurfaceInfoRoofHeightPc = LevelConfigFarDistance;
            } else {
                TerrainRoofValidPc = 1;
                TerrainRoofNormalPc = ordinary_above_normal;
                TerrainRoofIndexPc = ordinary_above_group;
                ShadowRoofPolygonPc = ordinary_above;
                GetSurfaceInfoRoofHeightPc = ordinary_above_y;
            }
            if (!(LevelConfigFarDistance > ordinary_above_y) || !(ordinary_above_normal.y > 0.0f) ||
                !(height_above + position->y > ordinary_above_y)) {
                if (TerrainCastLowerSentinel < ordinary_floor_y) {
                    TerrainSurfaceClassPc = 2;
                    ShadPoly = ordinary_floor;
                    GetSurfaceInfoSurfaceNormalPc = ordinary_floor_normal;
                    position->y = ordinary_floor_y;
                } else {
                    TerrainSurfaceClassPc = 3;
                    position->y = LevelConfigFarDistance;
                    GetSurfaceInfoSurfaceNormalPc.y = 1.0f;
                    ShadPoly = NULL;
                }
            } else {
                position->y = ordinary_above_y;
                TerrainSurfaceClassPc = 1;
                ShadPoly = ordinary_above;
                GetSurfaceInfoSurfaceNormalPc = ordinary_above_normal;
                TerrainShadowGroupIndexPc = ordinary_above_group;
            }
            return 0.0f;
        }
        group_index = scan->header.terrain_group;
        x = position->x - terrain->groups[group_index].origin.x;
        z = position->z - terrain->groups[group_index].origin.z;
        surfaces = scan + 1;
        entry = surfaces;
        for (remaining = shape_count; 0 != remaining; --remaining, ++entry) {
            TERRAIN_POLYGON_INFO_PC *surface;
            f32 normal_y;
            f32 normal_x;
            f32 normal_z;
            f32 plane_y;
            i32 winding;

            surface = entry->shape;
            normal_y = surface->normals[0].y;

#define CAST_LINE(a, b)                                                                                                \
    (((x - surface->vertices[a].x) * (surface->vertices[b].z - surface->vertices[a].z) +                               \
      (z - surface->vertices[a].z) * (surface->vertices[a].x - surface->vertices[b].x)) < 0.0f                         \
         ? 0                                                                                                           \
         : 1)
            if (!(surface->normals[1].y > BoltTargetRandomDenominator)) {
                if (!(normal_y > 0.0f)) {
                    if (normal_y < 0.0f) {
                        if (CAST_LINE(1, 2) == 0) {
                            if (CAST_LINE(1, 3) != 0 && CAST_LINE(3, 2) != 0) {
                                winding = 2;
                                goto second_plane;
                            }
                        } else if (CAST_LINE(0, 1) != 0 && 0 != CAST_LINE(2, 0)) {
                            winding = 2;
                            goto first_plane;
                        }
                    }
                } else {
                    if (CAST_LINE(2, 1) == 0) {
                        if (CAST_LINE(3, 1) != 0 && CAST_LINE(2, 3) != 0) {
                            winding = 1;
                        second_plane:
                            normal_y = surface->normals[1].y;
                            if (0.0f == normal_y)
                                goto first_plane;
                            normal_z = surface->normals[1].z;
                            normal_x = surface->normals[1].x;
                            plane_quotient = (((z - surface->vertices[3].z) * normal_z) +
                                              ((x - surface->vertices[3].x) * normal_x)) /
                                             -normal_y;
                            plane_y =
                                (((terrain->groups[group_index].origin.y + surface->vertices[3].y))) + plane_quotient;
                            goto record_plane;
                        }
                    } else if (CAST_LINE(1, 0) != 0 && CAST_LINE(0, 2) != 0) {
                        winding = 1;
                    first_plane:
                        normal_z = surface->normals[0].z;
                        normal_x = surface->normals[0].x;
                        normal_y = surface->normals[0].y;
                        plane_quotient =
                            ((x - surface->vertices[0].x) * normal_x + (z - surface->vertices[0].z) * normal_z) /
                            -normal_y;
                        plane_y = (terrain->groups[group_index].origin.y + surface->vertices[0].y) + plane_quotient;
                    record_plane:
                        if (surface->material[1] == 0) {
                            if (plane_y <= position->y && ordinary_floor_y < plane_y && winding == 1) {
                                ordinary_floor_normal.x = normal_x;
                                ordinary_floor_normal.y = normal_y;
                                ordinary_floor_normal.z = normal_z;
                                ordinary_floor = surface;
                                ordinary_floor_y = plane_y;
                                TerrainShadowGroupIndexPc = group_index;
                            } else if (position->y < plane_y && ordinary_above_y >= plane_y &&
                                       (plane_y != ordinary_above_y || normal_y < ordinary_above_normal.y)) {
                                ordinary_above_normal.x = normal_x;
                                ordinary_above_normal.y = normal_y;
                                ordinary_above_normal.z = normal_z;
                                ordinary_above = surface;
                                ordinary_above_y = plane_y;
                                ordinary_above_group = group_index;
                            }
                        } else {
                            if (plane_y <= position->y && plane_y > extended_floor_y && winding == 1) {
                                extended_floor_normal.x = normal_x;
                                extended_floor_normal.y = normal_y;
                                extended_floor_normal.z = normal_z;
                                extended_floor = surface;
                                extended_floor_y = plane_y;
                                TerrainShadowFloorIndexPc = group_index;
                            } else if (position->y < plane_y && plane_y <= extended_above_y &&
                                       (plane_y != extended_above_y || normal_y < extended_above_normal.y)) {
                                extended_above_normal.x = normal_x;
                                extended_above_normal.y = normal_y;
                                extended_above_normal.z = normal_z;
                                extended_above = surface;
                                extended_above_y = plane_y;
                                extended_above_group = group_index;
                            }
                        }
                    }
                }
            } else if (!(normal_y > 0.0f)) {
                if (0.0f > normal_y && CAST_LINE(0, 1) != 0 && CAST_LINE(2, 0) != 0 && 0 != (CAST_LINE(1, 2))) {
                    winding = 2;
                    goto first_plane;
                }
            } else {
                if (CAST_LINE(1, 0) != 0 && 0 != (CAST_LINE(0, 2)) && CAST_LINE(2, 1) != 0) {
                    winding = 1;
                    goto first_plane;
                }
            }
#undef CAST_LINE
        }
        scan = surfaces + shape_count;
        shape_count = scan->header.shape_count;
    }
}

/* Append after the integrated kernels; presentation macros stay local. */
#undef TERRAIN_FINISH_GROUP
#undef TERRAIN_CROSS
#undef SHADOW_BOUNDS_OVERLAP

extern i32 TerrainShadowStatusPc;
extern TERRAIN_POLYGON_INFO_PC *TerrainScaleWorkBasePc;

f32 __cdecl NewShadowEx(NUVEC *position, f32 unused, f32 height_above, f32 roof_range, i32 mask) {
    NUVEC local_position;
    (void)unused;
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

    if (CurrentTerrain == NULL)
        return LevelConfigFarDistance;

    TerI = (TERRAIN_SHADOW_QUERY_VIEW_PC *)NuScratchAlloc32(0x948);
    TerrainScaledShapeCursorPc = TerrainScaleWorkBasePc;
    local_position = *position;
    NewScanRot(&local_position, mask);
    (void)NewCast(&local_position, height_above, roof_range);
    NuScratchRelease();
    TerrainPlatformsOffPc = 0;
    return local_position.y;
}

/* Local existing nuterrain.c access views; no new storage definition. */
void PlatOnOff(i32 platform_index, i32 enabled) {
    TERRAIN_SET_PC *terrain;
    i32 group_index;
    terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    if (terrain == NULL || platform_index < 0 || platform_index >= terrain->max_platforms)
        return;
    group_index = terrain->platforms[platform_index].terrain_group_index;
    if (enabled != 0)
        terrain->groups[group_index].chunk_type = 1;
    else
        terrain->groups[group_index].chunk_type = -1;
}

void NewTerrPlatformsOff(void) {
    TerrainPlatformsOffPc = 1;
}

void TerrainDiagnosticNoOpPC(const void *, ...);
extern u8 *TerrainCrashDataPc;
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
    f32 min_x, max_x, min_z, max_z;

    TerrainCrashDataPc = NULL;
    TerrainDiagnosticNoOpPC(buffer);
    strcpy(path, (char *)base_path);
    strcat(path, ".ter");
    i = NuFileLoadBuffer(path, *buffer, 0x7fffffff);
    TerrainDiagnosticNoOpPC(path, i);
    if (i == 0)
        return -1;

    chunk = (TERRAIN_FILE_CHUNK *)((u8 *)*buffer + 2 * *(u32 *)*buffer);
    *buffer += 2;
    group_count = 0;
    for (i = 0; i < chunk->chunk_count; ++i) {
        if (chunk[i].field_16 == 0x1ca)
            TerrainDiagnosticNoOpPC(path);
    }
    terrain->file_version = chunk->version;
    terrain->spatial_nodes = NULL;
    for (i = 0; i < chunk->chunk_count; ++i) {
        switch (chunk[i].chunk_type) {
            case TERRAIN_CHUNK_GROUP_PRIMARY:
            case TERRAIN_CHUNK_GROUP_SECONDARY:
                terrain->groups[first_group].group_info = chunk[i].group_info;
                terrain->groups[first_group].chunk_type = chunk[i].chunk_type;
                terrain->groups[first_group].scene_index = chunk[i].field_16;
                ++group_count;
                terrain->groups[first_group].origin.x = chunk[i].origin.x;
                terrain->groups[first_group].origin.y = chunk[i].origin.y;
                terrain->groups[first_group].origin.z = chunk[i].origin.z;
                terrain->groups[first_group].data = (TERRAIN_SKIN_STREAM_VIEW_PC *)*buffer;
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
                TerrainCrashDataPc = (u8 *)*buffer;
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
            if (block[0].y == 2147483648.0f)
                continue;
            if (first + 15 < node->point_count) {
                min_x = max_x = block[0].x;
                min_z = max_z = block[0].z;
                for (j = first + 1; j <= first + 16; ++j) {
                    if (min_x > node->points[j].x)
                        min_x = node->points[j].x;
                    if (node->points[j].x > max_x)
                        max_x = node->points[j].x;
                    if (min_z > node->points[j].z)
                        min_z = node->points[j].z;
                    if (node->points[j].z > max_z)
                        max_z = node->points[j].z;
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

extern const char TerrainSourcePathPc[];
extern const f32 TerrainSteepNormalYPc;
extern void *ScaleTerrainT2, *TempScanStack, *WallSplList;
extern i32 terraincnt, curSphereter, platinrange, PlatImpactId, TerrShapeAdjCnt;
extern TERRAIN_SHAPE *ShadPoly, *TerrPoly;
extern u8 TerrWallInfo;
extern i16 debug_index;
void *TerrainInitEx(i32 level_num, void *buf, void *buf_end, i32 options, char *path, void *gscn, i32 scene_id,
                    u32 max_group_indices, u32 max_groups, u32 max_platforms) {
    i32 level;
    i32 group_index;
    i32 reset_cell;
    u32 platform_index;
    i32 slot;
    i32 shape_index;
    i32 n;
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
    TERRAIN_SHAPE_BATCH *batch;
    TERRAIN_SHAPE *shape;
    TERRAIN_FLOAT_BITS_PC x, y, z;
    u8 legacy_material;
    const NUVEC *vector;
    f64 world_y;
    f32 length_squared;
    f32 terrain_min_x;
    f32 terrain_min_z;
    f32 terrain_max_x;
    f32 terrain_max_z;
    f32 batch_min_x;
    f32 batch_max_x;
    f32 batch_min_z;
    f32 batch_max_z;
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

    (void)level_num;
    (void)options;
    (void)scene_id;

    cursor = ((VARIPTR *)(buf));
    saved_cursor = *cursor;
    scene = ((TERRAIN_GSCENE *)(gscn));

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
        ScaleTerrainT1 = NuAlloc(0xc800, TerrainSourcePathPc, 466);
    }
    if (ScaleTerrainT2 == NULL) {
        ScaleTerrainT2 = NuAlloc(0xc800, TerrainSourcePathPc, 468);
    }
    if (TempScanStack == NULL) {
        TempScanStack = NuAlloc(0x2000, TerrainSourcePathPc, 470);
    }
    if (WallSplList == NULL) {
        WallSplList = ((TERRAIN_WALL_POINT *)(NuAlloc(0x600, TerrainSourcePathPc, 472)));
    }

    terrain = ((TERRSET *)(cursor->char_ptr));
    cursor->char_ptr += sizeof(TERRSET);

    terrain->groups = ((TERRAIN_GROUP *)(cursor->char_ptr));
    cursor->char_ptr += (sizeof(TERRAIN_GROUP)) * ((max_groups + 0x80));
    terrain->group_indices = ((i16 *)(cursor->char_ptr));
    cursor->char_ptr += max_group_indices * sizeof(i16);
    terrain->platforms = ((TERRAIN_PLATFORM *)(cursor->char_ptr));
    cursor->char_ptr += max_platforms * sizeof(TERRAIN_PLATFORM);

    terrain->max_group_indices = max_group_indices;
    terrain->max_groups = max_groups;
    terrain->max_platforms = max_platforms;

    for (level = 0; level < TERRAIN_INDEX_LEVEL_COUNT; ++level) {
        terrain->index_levels[level].cache_age = 0;
        terrain->index_levels[level].scan_words[0].shape = NULL;
    }

    memset(terrain->groups, 0, max_groups * sizeof(TERRAIN_GROUP));
    for (group_index = 0; group_index < max_groups; ++group_index) {
        terrain->groups[group_index].chunk_type = -1;
    }

    for (reset_cell = 0; reset_cell < TERRAIN_CELL_RECORD_COUNT; ++reset_cell) {
        if (reset_cell < 256)
            terrain->cells[reset_cell].group_count = 0;
        else
            terrain->platform_cell.group_count = 0;
    }

    for (platform_index = 0; platform_index < max_platforms; ++platform_index) {
        platform = &(terrain->platforms[platform_index]);
        platform->flags &= ~TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED;
        platform->scene_object = NULL;
        platform->unknown_44 = 0;
        platform->bounce_54 = 0;
        platform->bounce_58 = 0;
        platform->bounce_5c = 0;
        platform->bounce_60 = 0;
        platform->bounce_64 = 0;
    }

    for (slot = 0; slot < TERRAIN_TRACK_SLOT_COUNT; ++slot) {
        terrain->track_slots[slot].id = NULL;
        terrain->track_slots[slot].wall_contact_state = TERRAIN_TRACK_CONTACT_NONE;
    }

    terrain->field_064 = NULL;
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
    terrain->group_count = ((i16)(group_count));
    if (group_count < 0) {
        *cursor = saved_cursor;
        return NULL;
    }

    kBoundsMinimum = -200000000.0f;
    kBoundsMaximum = 200000000.0f;
    kSteepNormalY = TerrainSteepNormalYPc;

    terrain->minimum_height = INT_MAX;

    for (group_index = 0; group_index < group_count; ++group_index) {
        group = &(terrain->groups[group_index]);
        if (((u32)(group->chunk_type)) > TERRAIN_CHUNK_GROUP_SECONDARY) {
            continue;
        }

        min_x = kBoundsMaximum;
        min_y = kBoundsMaximum;
        min_z = kBoundsMaximum;
        max_x = kBoundsMinimum;
        max_y = kBoundsMinimum;
        max_z = kBoundsMinimum;
        radius_squared = 0.0f;

        batch = ((TERRAIN_SHAPE_BATCH *)(group->data));
        while (batch->marker >= 0) {
            shape = ((TERRAIN_SHAPE *)(batch + 1));
            for (shape_index = 0; shape_index < batch->shape_count; ++shape_index, ++shape) {
                for (n = 0; n < 2; ++n) {

                    x.value = shape->normals[n].x;
                    y.value = shape->normals[n].y;
                    z.value = shape->normals[n].z;
                    if (x.bits == 0xffc00000 || y.bits == 0xffc00000 || z.bits == 0xffc00000) {
                        shape->normals[n].x = 0;
                        shape->normals[n].y = 1;
                        shape->normals[n].z = 0;
                    }
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
                    shape->normal_flags &=
                        ((u8)(~(TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP | TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP)));
                    if (kSteepNormalY >= shape->normals[0].y) {
                        shape->normal_flags |= TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP;
                    }
                    if (kSteepNormalY >= shape->normals[1].y) {
                        shape->normal_flags |= TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP;
                    }
                }

                if (shape->min_x < min_x)
                    min_x = shape->min_x;
                if (shape->max_x > max_x)
                    max_x = shape->max_x;
                if (shape->min_y < min_y)
                    min_y = shape->min_y;
                if (shape->max_y > max_y)
                    max_y = shape->max_y;
                if (min_z > shape->min_z)
                    min_z = shape->min_z;
                if (shape->max_z > max_z)
                    max_z = shape->max_z;

                for (vector_index = 0; vector_index < 4; ++vector_index) {
                    vector = &(shape->vertices[vector_index]);
                    world_y = (f64)group->origin.y + vector->y;
                    if (((f64)(terrain->minimum_height)) > world_y) {
                        terrain->minimum_height = ((i32)(world_y));
                    }

                    length_squared = vector->x * vector->x + vector->y * vector->y + vector->z * vector->z;
                    if (length_squared > radius_squared) {
                        radius_squared = length_squared;
                    }
                }
            }

            batch = ((TERRAIN_SHAPE_BATCH *)(((TERRAIN_SHAPE *)(batch + 1)) + batch->shape_count));
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

    for (group_index = 0; group_index < group_count; ++group_index) {
        group = &(terrain->groups[group_index]);
        if (group->chunk_type != TERRAIN_CHUNK_GROUP_PRIMARY) {
            continue;
        }

        batch = ((TERRAIN_SHAPE_BATCH *)(group->data));
        while (0 <= batch->marker) {
            min_x = group->origin.x + batch->min_x;
            max_x = group->origin.x + batch->max_x;
            min_z = group->origin.z + batch->min_z;
            max_z = group->origin.z + batch->max_z;
            if (min_x < terrain_min_x)
                terrain_min_x = min_x;
            if (max_x > terrain_max_x)
                terrain_max_x = max_x;
            if (min_z < terrain_min_z)
                terrain_min_z = min_z;
            if (max_z > terrain_max_z)
                terrain_max_z = max_z;

            batch = ((TERRAIN_SHAPE_BATCH *)(((TERRAIN_SHAPE *)(batch + 1)) + batch->shape_count));
        }
    }

    terrain->group_index_count = 0;
    for (group_index = 0; group_index < group_count; ++group_index) {
        group = &(terrain->groups[group_index]);
        if (group->chunk_type != TERRAIN_CHUNK_GROUP_PRIMARY) {
            continue;
        }

        min_x = kBoundsMaximum;
        min_z = kBoundsMaximum;
        max_x = kBoundsMinimum;
        max_z = kBoundsMinimum;
        batch = ((TERRAIN_SHAPE_BATCH *)(group->data));
        while (batch->marker >= 0) {
            batch_min_x = group->origin.x + batch->min_x;
            batch_max_x = group->origin.x + batch->max_x;
            batch_min_z = group->origin.z + batch->min_z;
            batch_max_z = group->origin.z + batch->max_z;
            if (batch_min_x < min_x)
                min_x = batch_min_x;
            if (batch_max_x > max_x)
                max_x = batch_max_x;
            if (batch_min_z < min_z)
                min_z = batch_min_z;
            if (batch_max_z > max_z)
                max_z = batch_max_z;

            batch = ((TERRAIN_SHAPE_BATCH *)(((TERRAIN_SHAPE *)(batch + 1)) + batch->shape_count));
        }

        group_min_x[group_index] = min_x;
        group_min_z[group_index] = min_z;
        group_max_x[group_index] = max_x;
        group_max_z[group_index] = max_z;

        x_distance = ((i32)((min_x + max_x) * 0.5f - terrain_min_x));
        x_cell = ((i32)(((f32)(x_distance * TERRAIN_GRID_WIDTH)) / (terrain_max_x - terrain_min_x)));
        if (x_cell < 0)
            x_cell = 0;
        else if (x_cell >= TERRAIN_GRID_WIDTH)
            x_cell = TERRAIN_GRID_WIDTH - 1;

        z_distance = ((i32)((min_z + max_z) * 0.5f - terrain_min_z));
        z_cell = ((i32)(((f32)(z_distance * TERRAIN_GRID_WIDTH)) / (terrain_max_z - terrain_min_z)));
        if (z_cell < 0)
            z_cell = 0;
        else if (z_cell >= TERRAIN_GRID_WIDTH)
            z_cell = TERRAIN_GRID_WIDTH - 1;

        group_cells[group_index] = ((i16)(z_cell * TERRAIN_GRID_WIDTH + x_cell));
    }

    terrain->used_cell_count = 0;
    terrain->group_index_count = 0;
    for (cell_index = 0; cell_index < TERRAIN_GRID_CELL_COUNT; ++cell_index) {
        cell = &(terrain->cells[terrain->used_cell_count]);
        cell->first_group = ((u16)(terrain->group_index_count));

        min_x = kBoundsMaximum;
        min_z = kBoundsMaximum;
        max_x = kBoundsMinimum;
        max_z = kBoundsMinimum;
        for (group_index = 0; group_index < group_count; ++group_index) {
            group = &(terrain->groups[group_index]);
            if (group_cells[group_index] != cell_index || group->chunk_type != TERRAIN_CHUNK_GROUP_PRIMARY) {
                continue;
            }

            if (group_min_x[group_index] < min_x)
                min_x = group_min_x[group_index];
            if (group_min_z[group_index] < min_z)
                min_z = group_min_z[group_index];
            if (group_max_x[group_index] > max_x)
                max_x = group_max_x[group_index];
            if (group_max_z[group_index] > max_z)
                max_z = group_max_z[group_index];

            ++cell->group_count;
            terrain->group_indices[terrain->group_index_count] = ((i16)(group_index));
            ++terrain->group_index_count;
        }

        if (cell->group_count != 0) {
            cell->min_x = min_x;
            cell->min_z = min_z;
            cell->max_x = max_x;
            cell->max_z = max_z;
            ++terrain->used_cell_count;
        }
    }

    platform_cell = &(terrain->platform_cell);
    platform_cell->first_group = ((u16)(terrain->group_index_count));

    platform_count = 0;
    for (group_index = 0; group_index < group_count; ++group_index) {
        group = &(terrain->groups[group_index]);
        if (group->chunk_type != TERRAIN_CHUNK_GROUP_SECONDARY) {
            continue;
        }
        if (platform_count >= terrain->max_platforms) {
            group->chunk_type = -1;
            TerrainDiagnosticNoOpPC(path);
            continue;
        }

        scene_object_index = group->scene_index;
        debug_index = ((i16)(scene_object_index));
        if (scene == NULL) {
            group->chunk_type = -1;
            TerrainDiagnosticNoOpPC(path);
            group->scene_index = ((u16)(platform_count));
            continue;
        }

        if (scene->object_map_present != NULL) {
            scene_object_index = scene->object_map[scene_object_index];
            debug_index = ((i16)(scene_object_index));
        }

        group->scene_index = ((u16)(platform_count));
        display_scene = scene->display_scene;
        if (display_scene == NULL && scene_object_index >= scene->object_count) {
            group->chunk_type = -1;
            TerrainDiagnosticNoOpPC(path);
            continue;
        }

        terrain->group_indices[terrain->group_index_count] = ((i16)(group_index));
        ++terrain->group_index_count;
        ++platform_cell->group_count;

        platform = &(terrain->platforms[((i16)(group->scene_index))]);
        platform->flags = (platform->flags & ~5u) | (1 & group->platform_flags);
        platform->scene_object_index = scene_object_index;
        platform->terrain_group_index = ((i16)(group_index));

        if (display_scene != NULL) {
            display_object = NULL;
            for (object_index = 0; object_index < display_scene->object_count; ++object_index) {
                candidate = &(display_scene->objects[object_index]);
                if (candidate->scene_object_index == scene_object_index) {
                    display_object = candidate;
                    break;
                }
            }

            if (display_object == NULL) {
                group->chunk_type = -1;
                TerrainDiagnosticNoOpPC(path);
                continue;
            }

            if (display_object->object == NULL || display_object->object == ((void *)(-1))) {
                platform->scene_object = display_object->embedded_object;
            } else {
                platform->scene_object = display_object->object;
            }
            platform->scene_transform = display_object->transform;
            platform->flags |= TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED;
        } else {
            scene_object = &(scene->objects[scene_object_index]);
            platform->scene_object = scene_object->object != NULL ? scene_object->object : scene_object;
            platform->scene_transform = scene_object->transform;
        }

        ++platform_count;
    }

    TerrShapeAdjCnt = 0;
    return saved_cursor.void_ptr;
}

void __cdecl TerrainPlatGetMtx(i32 index, NUMTX **previous, NUMTX **current) {
    if (index > -1) {
        *previous = (NUMTX *)&((TERRAIN_SET_PC *)CurrentTerrain)->platforms[index].previous_matrix;
        *current = (NUMTX *)((TERRAIN_SET_PC *)CurrentTerrain)->platforms[index].scene_object;
    }
}

void __cdecl TerrainSetImpactData(void *records, i32 *count, i32 max_records) {
    TerImpactData = records;
    TerImpactDataCount = count;
    *count = 0;
    TerImpactDataMax = max_records;
}

void __cdecl TerrainSetPlatConnectTol(f32 tolerance) {
    TerConTol = tolerance;
}

typedef struct TERRAIN_PLATFORM_LIFECYCLE_PC {
    NUMTX previous_matrix;
    NUMTX *scene_matrix;
    i32 motion_status;
    i16 terrain_group_index, scene_object_index;
    u32 flags;
    i16 bounce_frames;
    u8 unknown_52[2];
    f32 bounce_impulse, bounce_offset, bounce_velocity, bounce_spring, bounce_damping;
    TERRAIN_SCENE_STATE_VIEW_PC *scene_transform;
} TERRAIN_PLATFORM_LIFECYCLE_PC;
STATIC_ASSERT(sizeof(TERRAIN_PLATFORM_LIFECYCLE_PC) == 0x6c);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_LIFECYCLE_PC, previous_matrix) == 0);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_LIFECYCLE_PC, scene_matrix) == 0x40);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_LIFECYCLE_PC, motion_status) == 0x44);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_LIFECYCLE_PC, flags) == 0x4c);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_LIFECYCLE_PC, bounce_frames) == 0x50);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_LIFECYCLE_PC, bounce_impulse) == 0x54);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_LIFECYCLE_PC, bounce_offset) == 0x58);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_LIFECYCLE_PC, bounce_velocity) == 0x5c);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_LIFECYCLE_PC, bounce_spring) == 0x60);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_LIFECYCLE_PC, bounce_damping) == 0x64);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_LIFECYCLE_PC, scene_transform) == 0x68);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, removed_platform_count) == 0x154);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, removed_platforms) == 0x158);

void __cdecl noterraininit(void) {
    terraincnt = 0;
    curSphereter = 0;
    platinrange = 0;
    ShadPoly = NULL;
    TerI = NuScratchAlloc32(0x948);
    NuScratchRelease();
    CurrentTerrain = NULL;
    TerrShapeAdjCnt = 0;
}

void __cdecl TerrSetPlatScanDist(f32 distance) {
    TerrPlatScanDist = distance;
}
void __cdecl TerrainSetWallDeflectYScale(f32 scale) {
    TerrWallDeflectYScale = scale;
}

void __cdecl TerrainPlatformOldUpdate(void) {
    i32 i;
    if (CurrentTerrain == NULL)
        return;
    curPickInst = 0;
    for (i = 0; i < ((TERRAIN_SET_PC *)CurrentTerrain)->removed_platform_count; ++i)
        DeletePlatinst(((TERRAIN_SET_PC *)CurrentTerrain)->removed_platforms[i]);
    ((TERRAIN_SET_PC *)CurrentTerrain)->removed_platform_count = 0;
    curSphereter = 0;
    for (i = 0; i < ((TERRAIN_SET_PC *)CurrentTerrain)->max_platforms; ++i) {
        if (((TERRAIN_PLATFORM_LIFECYCLE_PC *)((TERRAIN_SET_PC *)CurrentTerrain)->platforms)[i].scene_matrix != NULL)
            ((TERRAIN_PLATFORM_LIFECYCLE_PC *)((TERRAIN_SET_PC *)CurrentTerrain)->platforms)[i].previous_matrix =
                *((TERRAIN_PLATFORM_LIFECYCLE_PC *)((TERRAIN_SET_PC *)CurrentTerrain)->platforms)[i].scene_matrix;
    }
}

#define TERRAIN_TPNU_SET ((TERRAIN_SET_PC *)CurrentTerrain)
#define TERRAIN_TPNU_PLATFORM(index) (((TERRAIN_PLATFORM_LIFECYCLE_PC *)TERRAIN_TPNU_SET->platforms)[index])
void __cdecl TerrainPlatformNewUpdate(void) {
    i32 i;
    f32 negative_distance, x, z, offset;
    u32 offset_bits;
    f64 back_x, back_z, product_x, product_z;
    f32 min_x, min_z, max_x, max_z;
    i16 *group_index;
    if (CurrentTerrain != NULL) {
        for (i = 0; i < 64; ++i) {
            if (TERRAIN_TPNU_SET->track_slots[i].id != NULL) {
                if (TERRAIN_TPNU_SET->track_slots[i].platform_contact_state > 0)
                    --TERRAIN_TPNU_SET->track_slots[i].platform_contact_state;
                if (TERRAIN_TPNU_SET->track_slots[i].wall_contact_state > 0)
                    --TERRAIN_TPNU_SET->track_slots[i].wall_contact_state;
            }
        }
        for (i = 0; i < 16; ++i) {
            if (TERRAIN_TPNU_SET->index_levels[i].cache_age > 0)
                --TERRAIN_TPNU_SET->index_levels[i].cache_age;
        }
    }
    for (i = 0; i < PlatCodeCallback; ++i)
        PlatCallback[i].function(PlatCallback[i].argument);
    PlatCodeCallback = 0;
    cntrots = 0;
    if (CurrentTerrain == NULL)
        return;
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
    if (min_x > x)
        min_x = x;
    if (max_x < x)
        max_x = x;
    product_z = global_camera.mtx.m02 * TerrPlatScanDist;
    z = global_camera.mtx.m22 * TerrPlatScanDist - product_z * (f64)0.35f;
    if (min_z > z)
        min_z = z;
    if (max_z < z)
        max_z = z;
    x = global_camera.mtx.m20 * TerrPlatScanDist + product_x * (f64)0.35f;
    if (min_x > x)
        min_x = x;
    if (max_x < x)
        max_x = x;
    z = global_camera.mtx.m22 * TerrPlatScanDist + product_z * (f64)0.35f;
    if (min_z > z)
        min_z = z;
    if (max_z < z)
        max_z = z;
    x = product_x * 0.25 - back_x;
    if (min_x > x)
        min_x = x;
    if (max_x < x)
        max_x = x;
    z = product_z * 0.25 - back_z;
    if (min_z > z)
        min_z = z;
    if (max_z < z)
        max_z = z;
    min_x += global_camera.mtx.m30 - 1.0f;
    max_x += global_camera.mtx.m30 + 1.0f;
    min_z += global_camera.mtx.m32 - 1.0f;
    max_z += global_camera.mtx.m32 + 1.0f;
    TERRAIN_TPNU_SET->platform_query_min_x = min_x;
    TERRAIN_TPNU_SET->platform_query_max_x = max_x;
    TERRAIN_TPNU_SET->platform_query_min_z = min_z;
    TERRAIN_TPNU_SET->platform_query_max_z = max_z;
    TERRAIN_TPNU_SET->active_platform_count = 0;
    group_index = &TERRAIN_TPNU_SET->group_indices[TERRAIN_TPNU_SET->platform_cell.first_group];
    for (i = 0; i < TERRAIN_TPNU_SET->platform_cell.group_count; ++i, ++group_index) {
        TERRAIN_GROUP_PC *group = &TERRAIN_TPNU_SET->groups[*group_index];
        i32 index = group->scene_index;
        TERRAIN_PLATFORM_LIFECYCLE_PC *platform = &TERRAIN_TPNU_PLATFORM(index);
        if (platform->scene_transform != NULL) {
            if ((platform->flags & 4) != 0) {
                if ((platform->scene_transform->flags & 2) == 0)
                    continue;
            } else if ((platform->scene_transform->flags & 1) == 0)
                continue;
        }
        if (group->chunk_type == 0)
            continue;
        if (platform->scene_matrix != NULL) {
            if (platform->bounce_frames != 0) {
                if (platform->bounce_frames > 124)
                    TERRAIN_TPNU_PLATFORM(index).bounce_velocity =
                        (TERRAIN_TPNU_PLATFORM(index).bounce_impulse -
                         TERRAIN_TPNU_PLATFORM(index).bounce_damping * TERRAIN_TPNU_PLATFORM(index).bounce_velocity) +
                        TERRAIN_TPNU_PLATFORM(index).bounce_velocity;
                else
                    TERRAIN_TPNU_PLATFORM(index).bounce_velocity =
                        TERRAIN_TPNU_PLATFORM(index).bounce_velocity -
                        TERRAIN_TPNU_PLATFORM(index).bounce_damping * TERRAIN_TPNU_PLATFORM(index).bounce_velocity;
                offset = TERRAIN_TPNU_PLATFORM(index).bounce_offset;
                offset_bits = *(u32 *)&offset & 0x7fffffff;
                TERRAIN_TPNU_PLATFORM(index).bounce_velocity =
                    (-TERRAIN_TPNU_PLATFORM(index).bounce_offset * *(f32 *)&offset_bits *
                         TERRAIN_TPNU_PLATFORM(index).bounce_spring * 0.5 -
                     TERRAIN_TPNU_PLATFORM(index).bounce_offset * TERRAIN_TPNU_PLATFORM(index).bounce_spring) +
                    TERRAIN_TPNU_PLATFORM(index).bounce_velocity;
                TERRAIN_TPNU_PLATFORM(index).bounce_offset += TERRAIN_TPNU_PLATFORM(index).bounce_velocity;
                --TERRAIN_TPNU_PLATFORM(index).bounce_frames;
                TERRAIN_TPNU_PLATFORM(index).scene_matrix->m31 += TERRAIN_TPNU_PLATFORM(index).bounce_offset;
            }
            group->origin.x = TERRAIN_TPNU_PLATFORM(index).scene_matrix->m30;
            group->origin.y = TERRAIN_TPNU_PLATFORM(index).scene_matrix->m31;
            group->origin.z = TERRAIN_TPNU_PLATFORM(index).scene_matrix->m32;
        }
        if ((TERRAIN_TPNU_PLATFORM(index).flags & 2) != 0)
            TERRAIN_TPNU_PLATFORM(index).bounce_frames = 128;
        TERRAIN_TPNU_PLATFORM(index).flags &= ~2U;
        TERRAIN_TPNU_PLATFORM(index).motion_status = 0;
        if ((TERRAIN_TPNU_PLATFORM(index).flags & 1) != 0) {
            if (group->origin.x - group->radius > max_x || group->radius + group->origin.x < min_x ||
                group->origin.z - group->radius > max_z || group->origin.z + group->radius < min_z)
                continue;
        } else {
            if (group->bounds_min.x + group->origin.x > max_x || group->bounds_max.x + group->origin.x < min_x ||
                group->bounds_min.z + group->origin.z > max_z || group->bounds_max.z + group->origin.z < min_z)
                continue;
        }
        if (TERRAIN_TPNU_SET->active_platform_count < 96) {
            TERRAIN_TPNU_SET->active_platform_groups[TERRAIN_TPNU_SET->active_platform_count] = *group_index;
            ++TERRAIN_TPNU_SET->active_platform_count;
        }
    }
}
#undef TERRAIN_TPNU_PLATFORM
#undef TERRAIN_TPNU_SET

i32 __cdecl FindPlatInst(i32 instance) {
    TERRAIN_SET_PC *terrain;
    i32 index;
    i32 result = -1;
    if (instance != -1) {
        terrain = (TERRAIN_SET_PC *)CurrentTerrain;
        for (index = 0; index < terrain->max_platforms; ++index) {
            if (terrain->platforms[index].scene_object != NULL &&
                terrain->platforms[index].scene_object_index == instance) {
                result = index;
                break;
            }
        }
    }
    return result;
}

void __cdecl PlatInstRotate(i32 platform_index, i32 enabled) {
    TERRAIN_SET_PC *terrain;
    if (platform_index >= 0) {
        terrain = (TERRAIN_SET_PC *)CurrentTerrain;
        if (platform_index < terrain->max_platforms)
            terrain->platforms[platform_index].flags = (terrain->platforms[platform_index].flags & ~1u) | (enabled & 1);
    }
}

typedef struct TERRAIN_PICKUP_SET_PC {
    TERRAIN_GROUP_PC *groups;
    void *shape_data;
    i32 group_count;
    i16 group_for_type[128];
    TERRAIN_PLATFORM_PC platforms[32];
    TERRAIN_SPATIAL_NODE *spatial_nodes;
    u8 unknown_e90[0xa490 - 0xe90];
} TERRAIN_PICKUP_SET_PC;
STATIC_ASSERT(offsetof(TERRAIN_PICKUP_SET_PC, groups) == 0);
STATIC_ASSERT(offsetof(TERRAIN_PICKUP_SET_PC, shape_data) == 4);
STATIC_ASSERT(offsetof(TERRAIN_PICKUP_SET_PC, group_count) == 8);
STATIC_ASSERT(offsetof(TERRAIN_PICKUP_SET_PC, group_for_type) == 0xc);
STATIC_ASSERT(offsetof(TERRAIN_PICKUP_SET_PC, platforms) == 0x10c);
STATIC_ASSERT(offsetof(TERRAIN_PICKUP_SET_PC, spatial_nodes) == 0xe8c);
STATIC_ASSERT(sizeof(TERRAIN_PICKUP_SET_PC) == 0xa490);
STATIC_ASSERT(offsetof(TERRAIN_PREVIOUS_MATRIX_VIEW_PC, m31) == 0x34);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, bounce_58) == 0x58);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, bounce_5c) == 0x5c);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, bounce_60) == 0x60);
STATIC_ASSERT(offsetof(TERRAIN_PLATFORM_PC, bounce_64) == 0x64);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, removed_platforms) == 0x158);
static TERRAIN_PICKUP_SET_PC *PickupTerrainPc = 0;

i32 ReadTerrainPickup(unsigned char *base_path, i16 **buffer, TERRAIN_PICKUP_SET_PC *terrain) {
    char path[100];
    TERRAIN_FILE_CHUNK *chunk, *header;
    i32 group_count;
    i32 i;
    strcpy(path, (char *)base_path);
    strcat(path, ".ter");
    NuFileLoadBuffer(path, *buffer, INT_MAX);
    chunk = (TERRAIN_FILE_CHUNK *)(*buffer + *(i32 *)*buffer);
    *buffer += 2;
    header = chunk;
    group_count = 0;
    TerrainCrashDataPc = NULL;
    for (i = 0; i < header->chunk_count; ++i, ++chunk) {
        switch (chunk->chunk_type) {
            case TERRAIN_CHUNK_GROUP_PRIMARY:
            case TERRAIN_CHUNK_GROUP_SECONDARY:
                terrain->groups[group_count].group_info = chunk->group_info;
                if (terrain->groups[group_count].group_info >= 0 && terrain->groups[group_count].group_info < 128)
                    terrain->group_for_type[terrain->groups[group_count].group_info] = (i16)group_count;
                terrain->groups[group_count].chunk_type = chunk->chunk_type;
                terrain->groups[group_count].scene_index = (i16)chunk->field_16;
                terrain->groups[group_count].origin.x = chunk->origin.x;
                terrain->groups[group_count].origin.y = chunk->origin.y;
                terrain->groups[group_count].origin.z = chunk->origin.z;
                terrain->groups[group_count].data = (TERRAIN_SKIN_STREAM_VIEW_PC *)*buffer;
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
                TerrainCrashDataPc = (u8 *)*buffer;
                break;
        }
        *buffer += chunk->data_word_count;
    }
    return group_count;
}

void terrainpickupinit(char *path, void **buffer) {
    TERRAIN_PICKUP_SET_PC *terrain = (TERRAIN_PICKUP_SET_PC *)*buffer;
    TERRAIN_GROUP_PC *group;
    i32 i;
    *buffer = (u8 *)*buffer + sizeof(TERRAIN_PICKUP_SET_PC);
    terrain->groups = (TERRAIN_GROUP_PC *)*buffer;
    *buffer = (u8 *)*buffer + 32 * sizeof(TERRAIN_GROUP_PC);
    terrain->shape_data = *buffer;
    *buffer = (u8 *)*buffer + 0x1c00;
    memset(terrain->groups, 0, 32 * sizeof(TERRAIN_GROUP_PC));
    group = terrain->groups;
    for (i = 0; i < 32; ++i)
        group[i].chunk_type = -1;
    for (i = 0; i < 32; ++i) {
        terrain->platforms[i].scene_object = NULL;
        terrain->platforms[i].bounce_54 = 0;
        terrain->platforms[i].bounce_58 = 0;
        terrain->platforms[i].bounce_5c = 0;
        terrain->platforms[i].bounce_64 = 0;
        terrain->platforms[i].bounce_60 = 0;
    }
    for (i = 0; i < 128; ++i)
        terrain->group_for_type[i] = -1;
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
        TERRAIN_SHAPE_BATCH *batch;
        f32 min_x, min_y, min_z, max_x, max_y, max_z, radius_squared;
        if (group->chunk_type != TERRAIN_CHUNK_GROUP_PRIMARY && group->chunk_type != TERRAIN_CHUNK_GROUP_SECONDARY)
            continue;
        radius_squared = 0.0f;
        min_x = 200000000.0f;
        min_y = 200000000.0f;
        min_z = 200000000.0f;
        max_x = -200000000.0f;
        max_y = -200000000.0f;
        max_z = -200000000.0f;
        batch = (TERRAIN_SHAPE_BATCH *)group->data;
        while (batch->marker >= 0) {
            TERRAIN_SHAPE *shape = (TERRAIN_SHAPE *)(batch + 1);
            i32 j;
            for (j = batch->shape_count; j > 0; --j, ++shape) {
                i32 k;
                if (shape->material[0] == 0xff) {
                    shape->material[0] = shape->material[1];
                    shape->material[1] = 0;
                    shape->flags |= 0x80;
                }
                if (shape->min_x < min_x)
                    min_x = shape->min_x;
                if (shape->min_y < min_y)
                    min_y = shape->min_y;
                if (shape->min_z < min_z)
                    min_z = shape->min_z;
                if (shape->max_x > max_x)
                    max_x = shape->max_x;
                if (shape->max_y > max_y)
                    max_y = shape->max_y;
                if (shape->max_z > max_z)
                    max_z = shape->max_z;
                for (k = 0; k < 4; ++k) {
                    f32 length_squared = shape->vertices[k].x * shape->vertices[k].x +
                                         shape->vertices[k].y * shape->vertices[k].y +
                                         shape->vertices[k].z * shape->vertices[k].z;
                    if (radius_squared < length_squared)
                        radius_squared = length_squared;
                }
            }
            batch = (TERRAIN_SHAPE_BATCH *)shape;
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
    PickupTerrainPc = terrain;
}

void AddPickupTerr(i32 type, NUVEC *position) {
    TERRAIN_SET_PC *terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    i16 source;
    i32 count;
    if (terrain == NULL)
        return;
    source = PickupTerrainPc->group_for_type[type];
    if (source == -1)
        return;
    count = curPickInst;
    if (count >= 128)
        return;
    terrain->groups[terrain->max_groups + count] = PickupTerrainPc->groups[source];
    terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    terrain->groups[terrain->max_groups + count].origin = *position;
    curPickInst = count + 1;
}

/* The real original caller expands the platform installer; no source-only helper is emitted. */
#define TERRAIN_APTR_SET ((TERRAIN_SET_PC *)CurrentTerrain)
i32 AddPickupTerrRot(i32 type, NUMTX *matrix, NUMTX *previous, i32 rotating) {
    TERRAIN_GROUP_PC *group;
    i16 source;
    i32 index;
    i32 found;
    if (CurrentTerrain == NULL)
        return -1;
    if (previous == NULL && rotating == 0) {
        AddPickupTerr(type, &matrix->translation);
        return -1;
    }
    if (TERRAIN_APTR_SET->group_index_count >= TERRAIN_APTR_SET->max_group_indices)
        return -1;
    if (TERRAIN_APTR_SET->group_count >= TERRAIN_APTR_SET->max_groups)
        return -1;
    if (matrix == NULL)
        return -1;
    if (previous == NULL && rotating != 0)
        return -1;
    source = PickupTerrainPc->group_for_type[type];
    found = -1;
    if (source != -1 && TERRAIN_APTR_SET->removed_platform_count < 32) {
        for (index = 0; index < TERRAIN_APTR_SET->max_platforms; ++index) {
            if (TERRAIN_APTR_SET->platforms[index].scene_object == NULL) {
                found = index;
                break;
            }
        }
    }
    if (found == -1)
        return -1;
    group = &TERRAIN_APTR_SET->groups[TERRAIN_APTR_SET->group_count];
    *group = PickupTerrainPc->groups[source];
    group->scene_index = (i16)found;
    group->chunk_type = 1;
    TERRAIN_APTR_SET->platforms[found].scene_object = matrix;
    if (rotating != 0) {
        memcpy(&TERRAIN_APTR_SET->platforms[found].previous_matrix, previous, sizeof(NUMTX));
        TERRAIN_APTR_SET->platforms[found].flags |= 1;
    } else {
        if (previous != NULL) {
            TERRAIN_APTR_SET->platforms[found].previous_matrix.m30 = previous->m30;
            TERRAIN_APTR_SET->platforms[found].previous_matrix.m31 = previous->m31;
            TERRAIN_APTR_SET->platforms[found].previous_matrix.m32 = previous->m32;
        } else {
            TERRAIN_APTR_SET->platforms[found].previous_matrix.m30 = matrix->m30;
            TERRAIN_APTR_SET->platforms[found].previous_matrix.m31 = matrix->m31;
            TERRAIN_APTR_SET->platforms[found].previous_matrix.m32 = matrix->m32;
        }
        TERRAIN_APTR_SET->platforms[found].flags &= ~1u;
    }
    group->origin.x = matrix->m30;
    group->origin.y = matrix->m31;
    group->origin.z = matrix->m32;
    TERRAIN_APTR_SET->platforms[found].terrain_group_index = TERRAIN_APTR_SET->group_count;
    TERRAIN_APTR_SET->platforms[found].scene_object_index = 0;
    TERRAIN_APTR_SET->platforms[found].scene_transform = NULL;
    TERRAIN_APTR_SET->group_indices[TERRAIN_APTR_SET->group_index_count] = TERRAIN_APTR_SET->group_count;
    ++TERRAIN_APTR_SET->group_index_count;
    TERRAIN_APTR_SET->platforms[found].bounce_54 = 0;
    TERRAIN_APTR_SET->platforms[found].bounce_58 = 0;
    TERRAIN_APTR_SET->platforms[found].bounce_5c = 0;
    TERRAIN_APTR_SET->platforms[found].bounce_64 = 0;
    TERRAIN_APTR_SET->platforms[found].bounce_60 = 0;
    TERRAIN_APTR_SET->platforms[found].flags &= ~2u;
    if (TERRAIN_APTR_SET->active_platform_count < 96) {
        TERRAIN_APTR_SET->active_platform_groups[TERRAIN_APTR_SET->active_platform_count] =
            TERRAIN_APTR_SET->group_count;
        ++TERRAIN_APTR_SET->active_platform_count;
    }
    ++TERRAIN_APTR_SET->group_count;
    ++TERRAIN_APTR_SET->platform_cell.group_count;
    TERRAIN_APTR_SET->removed_platforms[TERRAIN_APTR_SET->removed_platform_count] = (i16)found;
    ++TERRAIN_APTR_SET->removed_platform_count;
    return found;
}
#undef TERRAIN_APTR_SET

#define TERRAIN_NPPI_SET ((TERRAIN_SET_PC *)CurrentTerrain)
i32 NewPlatPickupInst(void *object, i32 type) {
    TERRAIN_GROUP_PC *group;
    i16 source;
    i32 index;
    i32 i;
    if (CurrentTerrain == NULL)
        return -1;
    if (TERRAIN_NPPI_SET->group_index_count >= TERRAIN_NPPI_SET->max_group_indices)
        return -1;
    if (TERRAIN_NPPI_SET->group_count >= TERRAIN_NPPI_SET->max_groups)
        return -1;
    if (object == NULL)
        return -1;
    source = PickupTerrainPc->group_for_type[type];
    if (source == -1)
        return -1;
    index = -1;
    for (i = 0; i < TERRAIN_NPPI_SET->max_platforms; ++i) {
        if (TERRAIN_NPPI_SET->platforms[i].scene_object == NULL) {
            index = i;
            break;
        }
    }
    if (index == -1)
        return -1;
    group = &TERRAIN_NPPI_SET->groups[TERRAIN_NPPI_SET->group_count];
    *group = PickupTerrainPc->groups[source];
    group->chunk_type = 1;
    group->scene_index = (i16)index;
    TERRAIN_NPPI_SET->platforms[index].scene_object = object;
    TERRAIN_NPPI_SET->platforms[index].flags &= ~1u;
    group->origin.x = ((NUMTX *)object)->m30;
    group->origin.y = ((NUMTX *)object)->m31;
    group->origin.z = ((NUMTX *)object)->m32;
    TERRAIN_NPPI_SET->platforms[index].terrain_group_index = TERRAIN_NPPI_SET->group_count;
    TERRAIN_NPPI_SET->platforms[index].scene_object_index = 0;
    TERRAIN_NPPI_SET->platforms[index].scene_transform = NULL;
    TERRAIN_NPPI_SET->group_indices[TERRAIN_NPPI_SET->group_index_count] = TERRAIN_NPPI_SET->group_count;
    TERRAIN_NPPI_SET->group_index_count++;
    TERRAIN_NPPI_SET->platforms[index].bounce_54 = 0;
    TERRAIN_NPPI_SET->platforms[index].bounce_58 = 0;
    TERRAIN_NPPI_SET->platforms[index].bounce_5c = 0;
    TERRAIN_NPPI_SET->platforms[index].bounce_64 = 0;
    TERRAIN_NPPI_SET->platforms[index].bounce_60 = 0;
    TERRAIN_NPPI_SET->platforms[index].flags &= ~2u;
    TERRAIN_NPPI_SET->group_count++;
    TERRAIN_NPPI_SET->platform_cell.group_count++;
    return index;
}
#undef TERRAIN_NPPI_SET

void ReassignPickupInst(i32 index, i32 type) {
    TERRAIN_SET_PC *terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    TERRAIN_GROUP_PC *source, *group;
    i16 source_index;
    if (terrain == NULL || index == -1)
        return;
    source_index = PickupTerrainPc->group_for_type[type];
    if (source_index == -1)
        return;
    group = &terrain->groups[terrain->platforms[index].terrain_group_index];
    source = &PickupTerrainPc->groups[source_index];
    group->data = source->data;
    group->bounds_min = source->bounds_min;
    group->bounds_max = source->bounds_max;
    group->radius = source->radius;
}

/* Native debug geometry uses the canonical 100-byte polygon view. */
STATIC_ASSERT(sizeof(TERRAIN_POLYGON_INFO_PC) == 0x64);
STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, vertices) == 0x18);
STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, normals) == 0x48);
STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, flags) == 0x62);
STATIC_ASSERT(offsetof(TERRAIN_SCENE_STATE_VIEW_PC, flags) == 0);
STATIC_ASSERT(offsetof(TERRAIN_SCENE_STATE_VIEW_PC, low_flags) == 0);

void TerrDrawPc(TERRAIN_POLYGON_INFO_PC *terrain, i16 index) {
    TERRAIN_POLYGON_INFO_PC *record;
    NUVEC *origin;
    i32 colour, i;
    f32 t;
    f64 x, y, z;

    record = terrain;
    colour = (index & 0x80) + ((u8)(index * 64) << 8);
    if (!(record->normals[1].y > 65535.0)) {
        for (i = 0; i < 2; ++i) {
            t = (f32)(i);
            origin = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[index].origin;
            NuRndrLine3dDbg(origin->x + record->vertices[0].x + (record->vertices[1].x - record->vertices[0].x) * t,
                            origin->y + record->vertices[0].y + (record->vertices[1].y - record->vertices[0].y) * t,
                            origin->z + record->vertices[0].z + (record->vertices[1].z - record->vertices[0].z) * t,
                            origin->x + record->vertices[2].x + (record->vertices[3].x - record->vertices[2].x) * t,
                            origin->y + record->vertices[2].y + (record->vertices[3].y - record->vertices[2].y) * t,
                            origin->z + record->vertices[2].z + (record->vertices[3].z - record->vertices[2].z) * t,
                            colour);
        }
        for (i = 0; i < 2; ++i) {
            t = (f32)(i);
            origin = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[index].origin;
            NuRndrLine3dDbg(origin->x + record->vertices[0].x + (record->vertices[2].x - record->vertices[0].x) * t,
                            origin->y + record->vertices[0].y + (record->vertices[2].y - record->vertices[0].y) * t,
                            origin->z + record->vertices[0].z + (record->vertices[2].z - record->vertices[0].z) * t,
                            origin->x + record->vertices[1].x + (record->vertices[3].x - record->vertices[1].x) * t,
                            origin->y + record->vertices[1].y + (record->vertices[3].y - record->vertices[1].y) * t,
                            origin->z + record->vertices[1].z + (record->vertices[3].z - record->vertices[1].z) * t,
                            colour);
        }
    }
    origin = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[index].origin;
    z = (f64)origin->z + record->vertices[0].z;
    y = (f64)origin->y + record->vertices[0].y;
    x = (f64)origin->x + record->vertices[0].x;
    NuRndrLine3dDbg(x, y, z, x + record->normals[0].x * (f64)0.4f, y + record->normals[0].y * (f64)0.4f,
                    z + record->normals[0].z * (f64)0.4f, colour);
    if (record->normals[1].y < 65536.0) {
        origin = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[index].origin;
        z = (f64)origin->z + record->vertices[3].z;
        y = (f64)origin->y + record->vertices[3].y;
        x = (f64)origin->x + record->vertices[3].x;
        NuRndrLine3dDbg(x, y, z, x + record->normals[1].x * (f64)0.4f, y + record->normals[1].y * (f64)0.4f,
                        z + record->normals[1].z * (f64)0.4f, colour);
    }
}

void TerrDrawSituPc(TERRAIN_POLYGON_INFO_PC *terrain, NUVEC *situation) {
    TERRAIN_POLYGON_INFO_PC *record = terrain;
    i32 i;
    f32 t;

    if (record->normals[1].y > 65535.0) {
        for (i = 0; i <= 1; ++i) {
            t = (f32)(i);
            NuRndrLine3dDbg(situation->x + record->vertices[0].x, situation->y + record->vertices[0].y,
                            situation->z + record->vertices[0].z,
                            situation->x + record->vertices[1].x + (record->vertices[2].x - record->vertices[1].x) * t,
                            situation->y + record->vertices[1].y + (record->vertices[2].y - record->vertices[1].y) * t,
                            situation->z + record->vertices[1].z + (record->vertices[2].z - record->vertices[1].z) * t,
                            0x003f7f80);
        }
        for (i = 0; i <= 1; ++i) {
            t = (f32)(i);
            NuRndrLine3dDbg(situation->x + record->vertices[1].x, situation->y + record->vertices[1].y,
                            situation->z + record->vertices[1].z,
                            situation->x + record->vertices[0].x + (record->vertices[2].x - record->vertices[0].x) * t,
                            situation->y + record->vertices[0].y + (record->vertices[2].y - record->vertices[0].y) * t,
                            situation->z + record->vertices[0].z + (record->vertices[2].z - record->vertices[0].z) * t,
                            0x003f7f80);
        }
    } else {
        for (i = 0; i <= 1; ++i) {
            t = (f32)(i);
            NuRndrLine3dDbg(situation->x + record->vertices[0].x + (record->vertices[1].x - record->vertices[0].x) * t,
                            situation->y + record->vertices[0].y + (record->vertices[1].y - record->vertices[0].y) * t,
                            situation->z + record->vertices[0].z + (record->vertices[1].z - record->vertices[0].z) * t,
                            situation->x + record->vertices[2].x + (record->vertices[3].x - record->vertices[2].x) * t,
                            situation->y + record->vertices[2].y + (record->vertices[3].y - record->vertices[2].y) * t,
                            situation->z + record->vertices[2].z + (record->vertices[3].z - record->vertices[2].z) * t,
                            0xffff7f80);
        }
        for (i = 0; i <= 1; ++i) {
            t = (f32)(i);
            NuRndrLine3dDbg(situation->x + record->vertices[0].x + (record->vertices[2].x - record->vertices[0].x) * t,
                            situation->y + record->vertices[0].y + (record->vertices[2].y - record->vertices[0].y) * t,
                            situation->z + record->vertices[0].z + (record->vertices[2].z - record->vertices[0].z) * t,
                            situation->x + record->vertices[1].x + (record->vertices[3].x - record->vertices[1].x) * t,
                            situation->y + record->vertices[1].y + (record->vertices[3].y - record->vertices[1].y) * t,
                            situation->z + record->vertices[1].z + (record->vertices[3].z - record->vertices[1].z) * t,
                            0xffff7f80);
        }
    }
}

void TerrDrawPlatPc(TERRAIN_POLYGON_INFO_PC *terrain, i16 index) {
    TERRAIN_PLATFORM_PC *platform;
    i32 i;
    NUVEC4 points[5], *normal;
    const NUVEC *origin;
    i32 colour;
    f32 t;

#define TERRAIN_TDP_GROUP (((TERRAIN_SET_PC *)CurrentTerrain)->groups[index])
#define TERRAIN_TDP_MATRIX                                                                                             \
    ((NUMTX *)((TERRAIN_SET_PC *)CurrentTerrain)->platforms[TERRAIN_TDP_GROUP.scene_index].scene_object)
#define TERRAIN_TDP_LINE(a, b, c, d)                                                                                   \
    for (i = 0; i < 2; ++i) {                                                                                          \
        t = (f32)i;                                                                                                    \
        origin = &TERRAIN_TDP_GROUP.origin;                                                                            \
        NuRndrLine3dDbg(origin->x + (points[a].x + (points[b].x - points[a].x) * t),                                   \
                        origin->y + (points[a].y + (points[b].y - points[a].y) * t),                                   \
                        origin->z + (points[a].z + (points[b].z - points[a].z) * t),                                   \
                        origin->x + (points[c].x + (points[d].x - points[c].x) * t),                                   \
                        origin->y + (points[c].y + (points[d].y - points[c].y) * t),                                   \
                        origin->z + (points[c].z + (points[d].z - points[c].z) * t), colour);                          \
    }
#define TERRAIN_TDP_FAN(a, c, d)                                                                                       \
    for (i = 0; i < 2; ++i) {                                                                                          \
        t = (f32)i;                                                                                                    \
        origin = &TERRAIN_TDP_GROUP.origin;                                                                            \
        NuRndrLine3dDbg(origin->x + points[a].x, origin->y + points[a].y, origin->z + points[a].z,                     \
                        origin->x + (points[c].x + (points[d].x - points[c].x) * t),                                   \
                        origin->y + (points[c].y + (points[d].y - points[c].y) * t),                                   \
                        origin->z + (points[c].z + (points[d].z - points[c].z) * t), colour);                          \
    }

    platform = &((TERRAIN_SET_PC *)CurrentTerrain)->platforms[TERRAIN_TDP_GROUP.scene_index];
    if (platform->scene_transform != NULL) {
        if (platform->flags & 4) {
            if (!(((TERRAIN_SCENE_STATE_VIEW_PC *)platform->scene_transform)->flags & 2))
                return;
        } else if (!(((TERRAIN_SCENE_STATE_VIEW_PC *)platform->scene_transform)->low_flags & 1))
            return;
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
    colour = (i8)(terrain->flags) < 0 ? 0x0000ffff : 0x00ff0000;

    if (((TERRAIN_SET_PC *)CurrentTerrain)->platforms[TERRAIN_TDP_GROUP.scene_index].flags & 1) {
        NuVec4MtxTransformVU0(&points[0], &points[0], TERRAIN_TDP_MATRIX);
        NuVec4MtxTransformVU0(&points[1], &points[1], TERRAIN_TDP_MATRIX);
        NuVec4MtxTransformVU0(&points[2], &points[2], TERRAIN_TDP_MATRIX);
        NuVec4MtxTransformVU0(&points[3], &points[3], TERRAIN_TDP_MATRIX);
        NuVec4MtxTransformVU0(normal, normal, TERRAIN_TDP_MATRIX);
    }

    if (terrain->normals[1].y > 65535.0) {
        TERRAIN_TDP_FAN(0, 1, 2);
        TERRAIN_TDP_FAN(1, 0, 2);
    } else {
        TERRAIN_TDP_LINE(0, 1, 2, 3);
        TERRAIN_TDP_LINE(0, 2, 1, 3);
    }
    origin = &TERRAIN_TDP_GROUP.origin;
    NuRndrLine3dDbg(origin->x + points[0].x, origin->y + points[0].y, origin->z + points[0].z,
                    origin->x + points[0].x + normal->x, origin->y + points[0].y + normal->y,
                    origin->z + points[0].z + normal->z, (i8)(index & 0xf0) + (i8)(index * 16) * 256);
#undef TERRAIN_TDP_FAN
#undef TERRAIN_TDP_LINE
#undef TERRAIN_TDP_MATRIX
#undef TERRAIN_TDP_GROUP
}

void TerrDrawPlatColPc(TERRAIN_POLYGON_INFO_PC *terrain, i16 index, i32 colour) {
    TERRAIN_PLATFORM_PC *platform;
    NUVEC4 points[4], normals[2];
    const NUVEC *origin;
    i32 i;
    f32 t;

#define TERRAIN_TDP_GROUP (((TERRAIN_SET_PC *)CurrentTerrain)->groups[index])
#define TERRAIN_TDP_MATRIX                                                                                             \
    ((NUMTX *)((TERRAIN_SET_PC *)CurrentTerrain)->platforms[TERRAIN_TDP_GROUP.scene_index].scene_object)
#define TERRAIN_TDP_LINE(a, b, c, d)                                                                                   \
    for (i = 0; i < 2; ++i) {                                                                                          \
        t = (f32)i;                                                                                                    \
        origin = &TERRAIN_TDP_GROUP.origin;                                                                            \
        NuRndrLine3dDbg((origin->x + (points[a].x + (points[b].x - points[a].x) * t)),                                 \
                        (origin->y + (points[a].y + (points[b].y - points[a].y) * t)),                                 \
                        (origin->z + (points[a].z + (points[b].z - points[a].z) * t)),                                 \
                        (origin->x + (points[c].x + (points[d].x - points[c].x) * t)),                                 \
                        (origin->y + (points[c].y + (points[d].y - points[c].y) * t)),                                 \
                        (origin->z + (points[c].z + (points[d].z - points[c].z) * t)), colour);                        \
    }
#define TERRAIN_TDP_FAN(a, c, d)                                                                                       \
    for (i = 0; i < 2; ++i) {                                                                                          \
        t = (f32)i;                                                                                                    \
        origin = &TERRAIN_TDP_GROUP.origin;                                                                            \
        NuRndrLine3dDbg((origin->x + points[a].x), (origin->y + points[a].y), (origin->z + points[a].z),               \
                        (origin->x + (points[c].x + (points[d].x - points[c].x) * t)),                                 \
                        (origin->y + (points[c].y + (points[d].y - points[c].y) * t)),                                 \
                        (origin->z + (points[c].z + (points[d].z - points[c].z) * t)), colour);                        \
    }
#define TERRAIN_TDP_NORMAL(p, n)                                                                                       \
    origin = &TERRAIN_TDP_GROUP.origin;                                                                                \
    NuRndrLine3dDbg((origin->x + points[p].x), (origin->y + points[p].y), (origin->z + points[p].z),                   \
                    (origin->x + points[p].x + normals[n].x), (origin->y + points[p].y + normals[n].y),                \
                    (origin->z + points[p].z + normals[n].z), 0xff)

    platform = &((TERRAIN_SET_PC *)CurrentTerrain)->platforms[TERRAIN_TDP_GROUP.scene_index];
    if (platform->scene_transform != NULL) {
        if (platform->flags & 4) {
            if ((((TERRAIN_SCENE_STATE_VIEW_PC *)platform->scene_transform)->flags & 2) == 0)
                return;
        } else if ((((TERRAIN_SCENE_STATE_VIEW_PC *)platform->scene_transform)->low_flags & 1) == 0)
            return;
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

    if (((TERRAIN_SET_PC *)CurrentTerrain)->platforms[TERRAIN_TDP_GROUP.scene_index].flags & 1) {
        NuVec4MtxTransformVU0(&points[0], &points[0], TERRAIN_TDP_MATRIX);
        NuVec4MtxTransformVU0(&points[1], &points[1], TERRAIN_TDP_MATRIX);
        NuVec4MtxTransformVU0(&points[2], &points[2], TERRAIN_TDP_MATRIX);
        NuVec4MtxTransformVU0(&normals[0], &normals[0], TERRAIN_TDP_MATRIX);
    }

    if (terrain->normals[1].y > 65535.0) {
        TERRAIN_TDP_FAN(0, 1, 2);
        TERRAIN_TDP_FAN(1, 0, 2);
    } else {
        points[3].x = terrain->vertices[3].x;
        points[3].y = terrain->vertices[3].y;
        points[3].z = terrain->vertices[3].z;
        points[3].w = 0.0f;
        NuVec4MtxTransformVU0(&points[3], &points[3], TERRAIN_TDP_MATRIX);
        normals[1].x = terrain->normals[1].x * (f64)0.4f;
        normals[1].y = terrain->normals[1].y * (f64)0.4f;
        normals[1].z = terrain->normals[1].z * (f64)0.4f;
        normals[1].w = 0.0f;
        NuVec4MtxTransformVU0(&normals[0], &normals[0], TERRAIN_TDP_MATRIX);
        TERRAIN_TDP_LINE(0, 1, 2, 3);
        TERRAIN_TDP_LINE(0, 2, 1, 3);
        TERRAIN_TDP_NORMAL(3, 1);
    }
    TERRAIN_TDP_NORMAL(0, 0);
#undef TERRAIN_TDP_NORMAL
#undef TERRAIN_TDP_FAN
#undef TERRAIN_TDP_LINE
#undef TERRAIN_TDP_MATRIX
#undef TERRAIN_TDP_GROUP
}

/* Original query packet field cohort used by the debug consumers. */
typedef struct TERRAIN_DEBUG_QUERY_PC {
    u8 unknown_00[0x6c];
    i16 terrain_group_index;
    u8 unknown_6e[0x22];
    TERRAIN_POLYGON_INFO_PC *surface;
    u8 unknown_94[0xb4];
    TERRAIN_SCAN_WORD_PC scan_words[512];
} TERRAIN_DEBUG_QUERY_PC;
STATIC_ASSERT(offsetof(TERRAIN_DEBUG_QUERY_PC, terrain_group_index) == 0x6c);
STATIC_ASSERT(offsetof(TERRAIN_DEBUG_QUERY_PC, surface) == 0x90);
STATIC_ASSERT(offsetof(TERRAIN_DEBUG_QUERY_PC, scan_words) == 0x148);
STATIC_ASSERT(sizeof(TERRAIN_DEBUG_QUERY_PC) == 0x948);

typedef struct terrain_debug_source_link_pc_s {
    TERRAIN_POLYGON_INFO_PC *source_polygon;
} TERRAIN_DEBUG_SOURCE_LINK_PC;
STATIC_ASSERT(offsetof(TERRAIN_DEBUG_SOURCE_LINK_PC, source_polygon) == 0);
STATIC_ASSERT(sizeof(TERRAIN_DEBUG_SOURCE_LINK_PC) == 4);

void TerrDrawImpactPolPc(void) {
    TERRAIN_DEBUG_QUERY_PC *query = (TERRAIN_DEBUG_QUERY_PC *)TerI;
    TERRAIN_POLYGON_INFO_PC *surface;
    if (query == NULL || query->surface == NULL)
        return;
    surface = query->surface;
    if ((u32)surface >= (u32)TerrainScaledShapeCursorPc && (u32)surface < (u32)TerrainScaledShapeCursorPc + 0xc800)
        surface = ((TERRAIN_DEBUG_SOURCE_LINK_PC *)surface)->source_polygon;
    TerrDrawPc(surface, query->terrain_group_index);
}

void DrawMPlatPc(i32 index) {
    TERRAIN_GROUP_PC *group;
    TERRAIN_SHAPE_BATCH *batch;
    TERRAIN_POLYGON_INFO_PC *shape;
    i32 remaining;
    if (CurrentTerrain == NULL || index < 0)
        return;
    group = &((TERRAIN_SET_PC *)CurrentTerrain)
                 ->groups[((TERRAIN_SET_PC *)CurrentTerrain)->platforms[index].terrain_group_index];
    if (group->chunk_type != 0 && group->chunk_type != 1)
        return;
    batch = (TERRAIN_SHAPE_BATCH *)group->data;
    while (batch->marker >= 0) {
        shape = (TERRAIN_POLYGON_INFO_PC *)(batch + 1);
        for (remaining = batch->shape_count; remaining > 0; --remaining, ++shape)
            TerrDrawPlatPc(shape, ((TERRAIN_SET_PC *)CurrentTerrain)->platforms[index].terrain_group_index);
        batch = (TERRAIN_SHAPE_BATCH *)shape;
    }
}

void DrawMSituPc(i32 index) {
    TERRAIN_GROUP_PC *group = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[index];
    TERRAIN_SHAPE_BATCH *batch;
    TERRAIN_POLYGON_INFO_PC *shape;
    i32 remaining;
    if ((group->chunk_type != 0 && group->chunk_type != 1) || group->data == NULL)
        return;
    batch = (TERRAIN_SHAPE_BATCH *)group->data;
    while (batch->marker >= 0) {
        shape = (TERRAIN_POLYGON_INFO_PC *)(batch + 1);
        for (remaining = batch->shape_count; remaining > 0; --remaining, ++shape)
            TerrDrawPc(shape, (i16)index);
        batch = (TERRAIN_SHAPE_BATCH *)shape;
    }
}

void SphereDrawExPc(NUVEC *centre, f32 radius, f32 vertical_scale, i32 colour) {
    f64 radius_squared = (f64)radius * radius;
    f64 scaled_radius = (f64)radius * vertical_scale;
    f32 lower_radius, upper_radius, lower_y, upper_y;
    i32 band, angle;
    for (band = 0; band < 8; ++band) {
        lower_radius = radius * NuTrigTable[(((band < 4 ? band : 8 - band) * 4096) >> 1) & 0x7fff];
        upper_radius = radius * NuTrigTable[(((band < 3 ? band + 1 : 7 - band) * 4096) >> 1) & 0x7fff];
        lower_y = NuFsqrt(radius_squared - (f64)lower_radius * lower_radius) * vertical_scale;
        upper_y = NuFsqrt(radius_squared - (f64)upper_radius * upper_radius) * vertical_scale;
        if (band > 4)
            lower_y = -lower_y;
        if (band > 3)
            upper_y = -upper_y;
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

void SphereDrawPc(NUVEC *centre, f32 radius, f32 vertical_scale) {
    SphereDrawExPc(centre, radius, vertical_scale, 0x00400000);
}

void DrawWallSplinePc(f32 height) {
    i32 i;
    NUVEC start, end;
    if (WallSplCount == 0)
        return;
    for (i = 0; i < WallSplCount; i += 2) {
        start.x = ((TERRAIN_WALL_POINT *)WallSplList)[i].position.x;
        start.y = height;
        start.z = ((TERRAIN_WALL_POINT *)WallSplList)[i].position.z;
        if (i == WallSplCount) {
            end.x = ((TERRAIN_WALL_POINT *)WallSplList)[i].position.x;
            end.z = ((TERRAIN_WALL_POINT *)WallSplList)[i].position.z;
        } else {
            end.x = ((TERRAIN_WALL_POINT *)WallSplList)[i + 1].position.x;
            end.z = ((TERRAIN_WALL_POINT *)WallSplList)[i + 1].position.z;
        }
        end.y = height;
        NuRndrLine3dDbg(start.x, start.y, start.z, end.x, end.y, end.z, (i32)0x80808080);
    }
}

static i32 TerrainSkinInfoCapacityPc = 0;
static u8 *TerrainSkinArenaEndPc = 0;
static i32 TerrainSkinOriginalGroupCountPc = -1;
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, max_groups) == 0xa488);
STATIC_ASSERT(offsetof(TERRAIN_SET_PC, max_platforms) == 0xa48c);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, origin) == 0);
STATIC_ASSERT(offsetof(TERRAIN_GROUP_PC, scene_index) == 0x30);
STATIC_ASSERT(offsetof(TERRAIN_XZ_BOUNDS_VIEW_PC, y) == 4);
/* Native 680e90: this preparation updates group bounds/radius, leaving
 * the copied polygon and batch bounds intact. */
#define TERRAIN_SKIN_BOUND(value, lo, hi)                                                                              \
    do {                                                                                                               \
        if ((value) < (lo))                                                                                            \
            (lo) = (value);                                                                                            \
        if ((value) > (hi))                                                                                            \
            (hi) = (value);                                                                                            \
    } while (0)
#define TERRAIN_SKIN_PREPARE_NORMAL(normal, a, b)                                                                      \
    do {                                                                                                               \
        NUVEC cross;                                                                                                   \
        f32 inverse;                                                                                                   \
        cross.x = (a).y * (b).z - (a).z * (b).y;                                                                       \
        cross.y = (a).z * (b).x - (a).x * (b).z;                                                                       \
        cross.z = (a).x * (b).y - (a).y * (b).x;                                                                       \
        *(normal) = cross;                                                                                             \
        length_squared = (normal)->x * (normal)->x + (normal)->y * (normal)->y + (normal)->z * (normal)->z;            \
        inverse = length_squared != 0.0f ? 1.0f / NuFsqrt(length_squared) : 0.0f;                                      \
        (normal)->x = (normal)->x * inverse;                                                                           \
        (normal)->y = (normal)->y * inverse;                                                                           \
        (normal)->z = (normal)->z * inverse;                                                                           \
    } while (0)
void TerrainSkinPrepareGroupPc(i32 group_index, TERRAIN_SKIN_STREAM_VIEW_PC *output,
                               const TERRAIN_SKIN_INFO_VIEW_PC *info) {
    TERRAIN_GROUP_PC *group;
    TERRAIN_SKIN_STREAM_VIEW_PC *input;
    TERRAIN_SKIN_POLYGON_VIEW_PC *source;
    TERRAIN_SKIN_POLYGON_VIEW_PC *polygon;
    NUVEC *p;
    NUVEC transformed;
    NUVEC a, b;
    i32 count;
    i32 vertex;
    i32 bytes;
    f32 z, length_squared;
    f32 min_x, min_y, min_z, max_x, max_y, max_z;
    f32 radius_squared;
    if (CurrentTerrain == NULL || group_index < 0)
        return;
    min_x = min_y = min_z = 123456789.0f;
    group = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[group_index];
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
                if (source->normals[1].y > 65535.0)
                    vertex = 2;
                else
                    vertex = 3;
                *polygon = *source;
                for (p = &polygon->vertices[vertex]; vertex >= 0; --vertex, --p) {
                    *p = *TerrainSkin(&transformed, info,
                                      &source->vertices[SkinFlipTab[vertex + info->vertex_order * 4]], -1.0f,
                                      info->mode);
                    p->x = p->x - info->matrix->m30;
                    p->y = p->y - info->matrix->m31;
                    p->z = p->z - info->matrix->m32;
                    z = p->z;
                    TERRAIN_SKIN_BOUND(p->x, min_x, max_x);
                    TERRAIN_SKIN_BOUND(p->y, min_y, max_y);
                    TERRAIN_SKIN_BOUND(z, min_z, max_z);
                    length_squared = p->x * p->x + p->y * p->y + z * z;
                    if (radius_squared < length_squared)
                        radius_squared = length_squared;
                }
                if (source->normals[1].y < 65535.0) {
                    a.x = polygon->vertices[1].x - polygon->vertices[3].x;
                    a.y = polygon->vertices[1].y - polygon->vertices[3].y;
                    a.z = polygon->vertices[1].z - polygon->vertices[3].z;
                    b.x = polygon->vertices[2].x - polygon->vertices[3].x;
                    b.y = polygon->vertices[2].y - polygon->vertices[3].y;
                    b.z = polygon->vertices[2].z - polygon->vertices[3].z;
                    TERRAIN_SKIN_PREPARE_NORMAL(&polygon->normals[1], a, b);
                }
                a.x = polygon->vertices[2].x - polygon->vertices[0].x;
                a.y = polygon->vertices[2].y - polygon->vertices[0].y;
                a.z = polygon->vertices[2].z - polygon->vertices[0].z;
                b.x = polygon->vertices[1].x - polygon->vertices[0].x;
                b.y = polygon->vertices[1].y - polygon->vertices[0].y;
                b.z = polygon->vertices[1].z - polygon->vertices[0].z;
                TERRAIN_SKIN_PREPARE_NORMAL(&polygon->normals[0], a, b);
                ++source;
                ++polygon;
            }
            input = (TERRAIN_SKIN_STREAM_VIEW_PC *)source;
            output = (TERRAIN_SKIN_STREAM_VIEW_PC *)polygon;
        }
        output->header.kind = -1;
        output->header.count = -1;
        bytes = (i32)((u8 *)input - (u8 *)group->data) + 4;
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
    if (bytes > TerrainSkinPoolSlotBytesPc)
        TerrainSkinPoolSlotBytesPc = bytes;
}
#undef TERRAIN_SKIN_PREPARE_NORMAL
#undef TERRAIN_SKIN_BOUND

/* Native 681470: preserve the native ordered error checks and live group
 * count reloads across the genuine skin preparation call. */
#define TERRAIN_SCI_SET ((TERRAIN_SET_PC *)CurrentTerrain)
i32 TerrainSkinCreateInfoPc(NUMTX *matrix, const TERRAIN_SKIN_BLEND_VIEW_PC *blend, const NUVEC4 *sample_rows,
                            f32 scale, i32 source_scene_index, i16 mode, TERRAIN_SET_PC *source_terrain) {
    TERRAIN_SKIN_INFO_VIEW_PC *info;
    i32 count, skin_index, index, found;
    if (CurrentTerrain == NULL || source_terrain == NULL)
        return -1;
    count = TERRAIN_SCI_SET->group_count;
    if (count >= TERRAIN_SCI_SET->max_groups)
        return -3;
    if (matrix == NULL || blend == NULL)
        return -4;
    skin_index = TerrainSkinInfoCountPc;
    if (skin_index >= TerrainSkinInfoCapacityPc)
        return -5;
    found = -1;
    for (index = 0; index < source_terrain->max_platforms; ++index) {
        if (source_terrain->platforms[index].scene_object != NULL &&
            source_terrain->platforms[index].scene_object_index == source_scene_index) {
            found = index;
            break;
        }
    }
    if (found == -1)
        return -6;
    TERRAIN_SCI_SET->groups[count] = source_terrain->groups[source_terrain->platforms[found].terrain_group_index];
    TERRAIN_SCI_SET->groups[TERRAIN_SCI_SET->group_count].chunk_type = 0;
    info = &TerrainSkinInfosPc[skin_index];
    info->matrix = matrix;
    info->blend = blend;
    info->sample_rows = sample_rows;
    info->scale = scale;
    info->mode = mode;
    if (matrix->m20 * matrix->m02 - matrix->m22 * matrix->m00 > 0.0)
        info->vertex_order = 1;
    else
        info->vertex_order = 0;
    TerrainSkinPrepareGroupPc(TERRAIN_SCI_SET->group_count, (TERRAIN_SKIN_STREAM_VIEW_PC *)TerrainSkinPoolBasePc, info);
    skin_index = TerrainSkinInfoCountPc;
    info = &TerrainSkinInfosPc[skin_index];
    info->group_index = TERRAIN_SCI_SET->group_count;
    info->original_shape = TERRAIN_SCI_SET->groups[TERRAIN_SCI_SET->group_count].data;
    TERRAIN_SCI_SET->groups[TERRAIN_SCI_SET->group_count].scene_index = (i16)(-1 - skin_index);
    TERRAIN_SCI_SET->groups[TERRAIN_SCI_SET->group_count].data = NULL;
    ++TERRAIN_SCI_SET->group_count;
    TerrainSkinInfoCountPc = skin_index + 1;
    return TERRAIN_SCI_SET->group_count - 1;
}
#undef TERRAIN_SCI_SET

/* Native 6813e0: caller-owned arena start/end and explicit record capacity.
 * This partitions an existing arena; it does not establish its allocation. */
void TerrainSkinInitArenaPc(TERRAIN_SKIN_INFO_VIEW_PC *base, u8 *end, i32 capacity) {
    TerrainSkinPoolBasePc = (u8 *)(base + capacity);
    TerrainSkinInfosPc = base;
    TerrainSkinInfoCapacityPc = capacity;
    TerrainSkinPoolSlotBytesPc = 0;
    TerrainSkinInfoCountPc = 0;
    TerrainSkinArenaEndPc = end;
    TerrainSkinOriginalGroupCountPc = ((TERRAIN_SET_PC *)CurrentTerrain)->group_count;
}

void TerrainSkinResetArenaPc(void) {
    i32 i;
    if (TerrainSkinOriginalGroupCountPc < 0)
        return;
    ((TERRAIN_SET_PC *)CurrentTerrain)->group_count = (i16)TerrainSkinOriginalGroupCountPc;
    for (i = 0; i < 16; ++i)
        ((TERRAIN_SET_PC *)CurrentTerrain)->index_levels[i].cache_age = 0;
}

/* Native 6816d0: temporary bounds and cell selectors are carved backwards
 * from the supplied end. Enough readable/writable arena is the native domain. */
u8 *TerrainSkinRebuildIndexPc(i32 pool_slots) {
    TERRAIN_SET_PC *terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    TERRAIN_GROUP_PC *group;
    TERRAIN_SKIN_USE_RECORD_PC *use;
    i16 *selectors;
    f32 *min_x, *max_x, *min_z, *max_z;
    f32 world_min_x = 200000000.0f, world_min_y = 200000000.0f;
    f32 world_min_z = 200000000.0f;
    f32 world_max_x = -200000000.0f, world_max_y = -200000000.0f;
    f32 world_max_z = -200000000.0f;
    i32 capacity = terrain->max_groups, i, cell_index;
    selectors = (i16 *)(((u32)TerrainSkinArenaEndPc & ~1u) - (u32)capacity * 2u);
    min_x = (f32 *)(((u32)selectors & ~3u) - (u32)capacity * 4u);
    max_x = min_x - capacity;
    min_z = max_x - capacity;
    max_z = min_z - capacity;
    group = terrain->groups;
    for (i = 0; i < terrain->group_count; ++i, ++group) {
        const i32 chunk_type = group->chunk_type;
        if (chunk_type == 0) {
            if (group->bounds_min.x < world_min_x)
                world_min_x = group->bounds_min.x;
            /* Each native comparison uses the matching minimum or maximum. */
            if (group->bounds_min.y < world_min_y)
                world_min_y = group->bounds_min.y;
            if (group->bounds_min.z < world_min_z)
                world_min_z = group->bounds_min.z;
            if (group->bounds_max.x > world_max_x)
                world_max_x = group->bounds_max.x;
            if (group->bounds_max.y > world_max_y)
                world_max_y = group->bounds_max.y;
            if (group->bounds_max.z > world_max_z)
                world_max_z = group->bounds_max.z;
        }
        if (chunk_type == 1) {
            if (group->bounds_min.x < world_min_x)
                world_min_x = group->origin.x + group->bounds_min.x;
            if (group->bounds_min.y < world_min_y)
                world_min_y = group->origin.y + group->bounds_min.y;
            if (group->bounds_min.z < world_min_z)
                world_min_z = group->origin.z + group->bounds_min.z;
            if (group->bounds_max.x > world_max_x)
                world_max_x = group->origin.x + group->bounds_max.x;
            if (group->bounds_max.y > world_max_y)
                world_max_y = group->origin.y + group->bounds_max.y;
            if (group->bounds_max.z > world_max_z)
                world_max_z = group->origin.z + group->bounds_max.z;
        }
    }
    terrain->group_index_count = 0;
    group = terrain->groups;
    for (i = 0; i < terrain->group_count; ++i, ++group) {
        if (group->chunk_type == 0) {
            i32 x, z;
            min_z[i] = min_x[i] = 200000000.0f;
            max_z[i] = max_x[i] = -200000000.0f;
            if (group->bounds_min.x < min_x[i])
                min_x[i] = group->bounds_min.x;
            if (group->bounds_min.z < min_z[i])
                min_z[i] = group->bounds_min.z;
            if (group->bounds_max.x > max_x[i])
                max_x[i] = group->bounds_max.x;
            if (group->bounds_max.z > max_z[i])
                max_z[i] = group->bounds_max.z;
            x = (i32)((f64)((i32)(((f64)min_x[i] + max_x[i]) * 0.5 - world_min_x) * 7) /
                      ((f64)world_max_x - world_min_x));
            if (x < 0)
                x = 0;
            else if (x > 6)
                x = 6;
            z = (i32)((f64)((i32)(((f64)min_z[i] + max_z[i]) * 0.5 - world_min_z) * 7) /
                      ((f64)world_max_z - world_min_z));
            if (z < 0)
                z = 0;
            else if (z > 6)
                z = 6;
            selectors[i] = (i16)(z * 7 + x);
        }
    }
    /* Native clear runs one past cells[] into the adjacent platform_cell. */
    for (i = 0; i < 257; ++i)
        terrain->cells[i].group_count = 0;
    terrain->used_cell_count = 0;
    for (cell_index = 0; cell_index < 49; ++cell_index) {
        TERRAIN_CELL_PC *cell = &terrain->cells[terrain->used_cell_count];
        f32 cell_min_x = 200000000.0f, cell_min_z = 200000000.0f;
        f32 cell_max_x = -200000000.0f, cell_max_z = -200000000.0f;
        cell->first_group = (i16)terrain->group_index_count;
        for (i = 0; i < terrain->group_count; ++i) {
            if (selectors[i] == cell_index && terrain->groups[i].chunk_type == 0) {
                if (min_x[i] < cell_min_x)
                    cell_min_x = min_x[i];
                if (min_z[i] < cell_min_z)
                    cell_min_z = min_z[i];
                if (max_x[i] > cell_max_x)
                    cell_max_x = max_x[i];
                if (max_z[i] > cell_max_z)
                    cell_max_z = max_z[i];
                ++cell->group_count;
                terrain->group_indices[terrain->group_index_count++] = (i16)i;
            }
        }
        if (cell->group_count != 0) {
            cell->min_x = cell_min_x;
            cell->min_z = cell_min_z;
            cell->max_x = cell_max_x;
            cell->max_z = cell_max_z;
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
            ++group; /* Native cursor advances only on the platform branch. */
        }
    }
    use = (TERRAIN_SKIN_USE_RECORD_PC *)(TerrainSkinPoolBasePc + TerrainSkinPoolSlotBytesPc * pool_slots);
    TerrainSkinUseRecordsPc = use;
    for (i = 0; i < pool_slots; ++i) {
        use[i].last_use = 0;
        use[i].skin_index = -1;
    }
    TerrainSkinPoolSlotCountPc = pool_slots;
    return (u8 *)(use + pool_slots);
}

STATIC_ASSERT(offsetof(TERRAIN_SPATIAL_NODE, point_count) == 0);
STATIC_ASSERT(offsetof(TERRAIN_SPATIAL_NODE, points) == 4);
STATIC_ASSERT(offsetof(TERRAIN_SPATIAL_CHUNK, previous) == 0);
STATIC_ASSERT(offsetof(TERRAIN_SPATIAL_CHUNK, node) == 4);

void TerrainScanWallSplinePc(TERRAIN_SPATIAL_NODE *node) {
    u32 first = 0;
    while ((i32)(node->point_count - 3) >= (i32)first) {
        NUVEC minimum, maximum;
        i32 count = 16;
        i32 i;
        if (node->point_count + 1 < count)
            count = node->point_count + 1;
        maximum = node->points[first];
        minimum = maximum;
        for (i = 1; i < count; ++i) {
            if (node->points[first + i].x < minimum.x)
                minimum.x = node->points[first + i].x;
            if (node->points[first + i].z < minimum.z)
                minimum.z = node->points[first + i].z;
            if (node->points[first + i].x > maximum.x)
                maximum.x = node->points[first + i].x;
            if (node->points[first + i].z > maximum.z)
                maximum.z = node->points[first + i].z;
        }
        node->points[first].y = minimum.x;
        node->points[first + 1].y = maximum.x;
        node->points[first + 2].y = minimum.z;
        node->points[first + 3].y = maximum.z;
    }
    if (first < (u32)(node->point_count + 1))
        node->points[first].y = 2147483648.0f;
}

void TerrainAddWallSplinePc(TERRAIN_SPATIAL_NODE *node, TERRAIN_SET_PC *terrain) {
    TERRAIN_SPATIAL_CHUNK *chunk = (TERRAIN_SPATIAL_CHUNK *)((u8 *)node - offsetof(TERRAIN_SPATIAL_CHUNK, node));
    chunk->previous = terrain->spatial_nodes;
    terrain->spatial_nodes = node;
}

#define TERRAIN_SPATIAL_PREVIOUS(n)                                                                                    \
    (((TERRAIN_SPATIAL_CHUNK *)((u8 *)(n) - offsetof(TERRAIN_SPATIAL_CHUNK, node)))->previous)
void TerrainRemoveWallSplinePc(TERRAIN_SPATIAL_NODE *node, TERRAIN_SET_PC *terrain) {
    TERRAIN_SPATIAL_NODE *current, *last;
    if (terrain == NULL)
        return;
    current = terrain->spatial_nodes;
    if (current == NULL)
        return;
    if (current == node) {
        terrain->spatial_nodes = TERRAIN_SPATIAL_PREVIOUS(node);
        return;
    }
    do {
        last = current;
        current = TERRAIN_SPATIAL_PREVIOUS(current);
        if (current == node) {
            TERRAIN_SPATIAL_PREVIOUS(last) = TERRAIN_SPATIAL_PREVIOUS(node);
            return;
        }
    } while (current != NULL);
}
#undef TERRAIN_SPATIAL_PREVIOUS
