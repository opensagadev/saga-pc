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
    u8 unknown_34[4];
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

void __cdecl ScanTerrIDRemovePlat(i32 platform_index) {
    TERRAIN_SET_PC *terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    i32 i;

    for (i = 0; i < 64; i++) {
        if (terrain->track_slots[i].platform_index == platform_index)
            terrain->track_slots[i].id = NULL;
    }
}

i32 __cdecl NewPlatInst(void *object, i32 instance) {
    TERRAIN_SET_PC *terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    TERRAIN_PLATFORM_PC *source_platform;
    TERRAIN_PLATFORM_PC *new_platform;
    TERRAIN_GROUP_PC *new_group;
    i32 new_index;
    i32 source_index;
    i32 i;

    if (terrain == NULL)
        return -1;
    if (terrain->group_index_count >= terrain->max_group_indices)
        return -1;
    new_index = terrain->group_count;
    if (new_index >= terrain->max_groups || object == NULL)
        return -1;
    if (terrain->max_platforms <= 0)
        return -1;

    new_index = -1;
    for (i = 0; i < terrain->max_platforms; i++) {
        if (terrain->platforms[i].scene_object == NULL) {
            new_index = i;
            break;
        }
    }
    if (new_index < 0)
        return -1;

    source_index = -1;
    for (i = 0; i < terrain->max_platforms; i++) {
        if (terrain->platforms[i].scene_object != NULL && terrain->platforms[i].scene_object_index == instance) {
            source_index = i;
            break;
        }
    }
    if (source_index < 0)
        return -1;

    source_platform = &terrain->platforms[source_index];
    new_group = &terrain->groups[terrain->group_count];
    *new_group = terrain->groups[source_platform->terrain_group_index];
    new_group->scene_index = (i16)new_index;
    new_group->chunk_type = 1;

    new_platform = &terrain->platforms[new_index];
    new_platform->scene_object = object;
    new_platform->terrain_group_index = terrain->group_count;
    new_platform->scene_object_index = (i16)instance;
    new_platform->scene_transform = NULL;
    new_platform->flags = (new_platform->flags & ~1u) | (source_platform->flags & 1);
    new_platform->bounce_54 = 0.0f;
    new_platform->bounce_58 = 0.0f;
    new_platform->bounce_5c = 0.0f;
    new_platform->bounce_60 = 0.0f;
    new_platform->bounce_64 = 0.0f;

    terrain->group_indices[terrain->group_index_count] = terrain->group_count;
    terrain->group_index_count++;
    terrain->group_count++;
    terrain->platform_cell.group_count++;
    return new_index;
}

i32 __cdecl DeletePlatinst(i32 platform_index) {
    TERRAIN_SET_PC *terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    TERRAIN_PLATFORM_PC *platform;
    TERRAIN_GROUP_PC *last_group;
    i32 last_group_index;
    i32 group_index;
    i32 cell_count;
    i32 cell_group_index;
    i32 i;

    if (platform_index < 0 || platform_index >= terrain->max_platforms)
        return 0;
    platform = &terrain->platforms[platform_index];
    if (platform->scene_object == NULL)
        return 0;

    terrain->group_count--;
    terrain->group_index_count--;
    last_group_index = terrain->group_count;
    last_group = &terrain->groups[last_group_index];
    group_index = platform->terrain_group_index;
    terrain->platforms[last_group->scene_index].terrain_group_index = (i16)group_index;
    terrain->groups[group_index] = *last_group;
    last_group->chunk_type = -1;
    platform->scene_object = NULL;

    cell_count = terrain->platform_cell.group_count;
    cell_group_index = terrain->platform_cell.first_group;
    for (i = 0; i < cell_count; i++, cell_group_index++) {
        if (terrain->group_indices[cell_group_index] == last_group_index) {
            terrain->group_indices[cell_group_index] = terrain->group_indices[terrain->group_index_count];
            break;
        }
    }
    terrain->platform_cell.group_count--;
    ScanTerrIDRemovePlat(platform_index);
    return 1;
}

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

extern const f64 TerrainSkinSampleCountPc;
extern const f64 TerrainSkinSampleIndexScalePc;
extern const f32 TerrainSkinBoundsMaxPc;
extern const f32 TerrainSkinBoundsMinPc;

/* Explicit output-pointer interface preserves the verified PC five-word ABI.
 * Android's aggregate return is a source candidate, not a historical PC type. */
NUVEC *TerrainSkin(NUVEC *result, const TERRAIN_SKIN_INFO_VIEW_PC *info, const NUVEC *position, f32 weight, i32 mode) {
    NUVEC4 point;
    NUMTX matrix;
    if (mode == 1) {
        point.x = position->x;
        point.y = position->y;
        point.z = position->z;
        point.w = 1.0f;
        NuVec4MtxTransformVU0(&point, &point, info->matrix);
    } else {
        f64 t = weight;
        if (t < 0.0)
            t = (f32)(position->z / info->scale);
        if (t > 1.0)
            t = 1.0;
        if (t < 0.0)
            t = 0.0;
        point.x = position->x;
        point.y = position->y;
        point.w = 1.0f;
        if (info->sample_rows != NULL) {
            i32 sample;
            t = (f32)((i32)(t * TerrainSkinSampleCountPc + 0.5) * 0.125);
            point.z = (f32)(position->z - info->scale * t);
            sample = (i32)(t * TerrainSkinSampleIndexScalePc);
            /* Original computes negative sample; -sample is0..8 for the
             * qualified quantized finite0..1 weights. Four banks stride11. */
            matrix.m00 = info->sample_rows[-sample].x;
            matrix.m01 = info->sample_rows[-sample].y;
            matrix.m02 = info->sample_rows[-sample].z;
            matrix.m03 = info->sample_rows[-sample].w;
            matrix.m10 = info->sample_rows[11 - sample].x;
            matrix.m11 = info->sample_rows[11 - sample].y;
            matrix.m12 = info->sample_rows[11 - sample].z;
            matrix.m13 = info->sample_rows[11 - sample].w;
            matrix.m20 = info->sample_rows[22 - sample].x;
            matrix.m21 = info->sample_rows[22 - sample].y;
            matrix.m22 = info->sample_rows[22 - sample].z;
            matrix.m23 = info->sample_rows[22 - sample].w;
            matrix.m30 = info->sample_rows[33 - sample].x;
            matrix.m31 = info->sample_rows[33 - sample].y;
            matrix.m32 = info->sample_rows[33 - sample].z;
            matrix.m33 = info->sample_rows[33 - sample].w;
        } else {
            const NUMTX *start = info->matrix;
            const TERRAIN_SKIN_BLEND_VIEW_PC *blend = info->blend;
            f64 inverse, wave, sx, sy, sz;
            t = (f32)((i32)(t * 10.0 + 0.5) / 10.0);
            point.z = (f32)(position->z - info->scale * t);
            inverse = 1.0 - t;
            wave = NuTrigTable[((i32)(t * 32768.0) >> 1) & 0x7fff];
            sx = 1.0 + blend->row_amplitude.x * wave;
            sy = 1.0 + blend->row_amplitude.y * wave;
            sz = 1.0 + blend->row_amplitude.z * wave;
#define SKIN_ROW(MEMBER, AMPLITUDE)                                                                                    \
    matrix.MEMBER = (f32)((start->MEMBER * inverse + blend->end_matrix.MEMBER * t) * (AMPLITUDE))
            SKIN_ROW(m00, sx);
            SKIN_ROW(m01, sx);
            SKIN_ROW(m02, sx);
            SKIN_ROW(m03, sx);
            SKIN_ROW(m10, sy);
            SKIN_ROW(m11, sy);
            SKIN_ROW(m12, sy);
            SKIN_ROW(m13, sy);
            SKIN_ROW(m20, sz);
            SKIN_ROW(m21, sz);
            SKIN_ROW(m22, sz);
            SKIN_ROW(m23, sz);
#undef SKIN_ROW
            matrix.m30 =
                (f32)(start->m30 * inverse + blend->end_matrix.m30 * t + blend->translation_amplitude.x * wave);
            matrix.m31 =
                (f32)(start->m31 * inverse + blend->end_matrix.m31 * t + blend->translation_amplitude.y * wave);
            matrix.m32 =
                (f32)(start->m32 * inverse + blend->end_matrix.m32 * t + blend->translation_amplitude.z * wave);
            matrix.m33 = (f32)(start->m33 * inverse + blend->end_matrix.m33 * t);
        }
        NuVec4MtxTransformVU0(&point, &point, &matrix);
    }
    result->x = point.x;
    result->y = point.y;
    result->z = point.z;
    return result;
}

#define TERRAIN_SKIN_NORMAL(normal, a, b)                                                                              \
    do {                                                                                                               \
        f32 length, inverse;                                                                                           \
        (normal)->x = (f32)((f64)(a)->y * (b)->z - (f64)(a)->z * (b)->y);                                              \
        (normal)->y = (f32)((f64)(a)->z * (b)->x - (f64)(a)->x * (b)->z);                                              \
        (normal)->z = (f32)((f64)(a)->x * (b)->y - (f64)(a)->y * (b)->x);                                              \
        length = NuFsqrt(                                                                                              \
            (f32)((f64)(normal)->x * (normal)->x + (f64)(normal)->y * (normal)->y + (f64)(normal)->z * (normal)->z));  \
        inverse = length != 0.0f ? (f32)(1.0 / length) : 0.0f;                                                         \
        (normal)->x = (f32)((f64)(normal)->x * inverse);                                                               \
        (normal)->y = (f32)((f64)(normal)->y * inverse);                                                               \
        (normal)->z = (f32)((f64)(normal)->z * inverse);                                                               \
    } while (0)

/* Valid initialized variable records, vertex_order 0 or 1, and enough output
 * storage for each complete 20-byte header and 100-byte polygon. */
void SkinPlatform(TERRAIN_GROUP_PC *group, TERRAIN_SKIN_STREAM_VIEW_PC *output, const TERRAIN_SKIN_INFO_VIEW_PC *info) {
    TERRAIN_SKIN_STREAM_VIEW_PC *input;
    TERRAIN_SKIN_STREAM_VIEW_PC *output_base = output;
    if (CurrentTerrain == NULL || (group->chunk_type != 0 && group->chunk_type != 1))
        return;
    input = group->data;
    while (input->header.kind >= 0) {
        TERRAIN_SKIN_BATCH_VIEW_PC *header = &output->header;
        TERRAIN_SKIN_POLYGON_VIEW_PC *destination = output->polygons;
        const TERRAIN_SKIN_POLYGON_VIEW_PC *source = input->polygons;
        f32 min_x = TerrainSkinBoundsMaxPc, max_x = TerrainSkinBoundsMinPc;
        f32 min_z = TerrainSkinBoundsMaxPc, max_z = TerrainSkinBoundsMinPc;
        i32 count = input->header.count;
        header->kind = input->header.kind;
        header->count = input->header.count;
        while (count > 0) {
            TERRAIN_SKIN_POLYGON_VIEW_PC *polygon = destination;
            f32 pmin_x = TerrainSkinBoundsMaxPc, pmax_x = TerrainSkinBoundsMinPc;
            f32 pmin_y = TerrainSkinBoundsMaxPc, pmax_y = TerrainSkinBoundsMinPc;
            f32 pmin_z = TerrainSkinBoundsMaxPc, pmax_z = TerrainSkinBoundsMinPc;
            i32 last = source->normals[1].y > BoltTargetRandomDenominator ? 2 : 3;
            i32 vertex;
            *polygon = *source; /* Original REP MOVSD copies25DWORDs. */
            for (vertex = last; vertex >= 0; --vertex) {
                NUVEC transformed;
                i32 selected = SkinFlipTab[vertex + info->vertex_order * 4];
                const NUVEC *position = &source->vertices[selected];
                NUVEC *p = &polygon->vertices[vertex];
                TerrainSkin(&transformed, info, position, -1.0f, info->mode);
                p->x = transformed.x;
                p->y = transformed.y;
                p->z = transformed.z;
                p->x = (f32)(p->x - info->matrix->m30);
                p->y = (f32)(p->y - info->matrix->m31);
                p->z = (f32)(p->z - info->matrix->m32);
#define SKIN_BOUNDS(VALUE, LO, HI)                                                                                     \
    if ((VALUE) < (LO))                                                                                                \
        (LO) = (VALUE);                                                                                                \
    if ((VALUE) > (HI))                                                                                                \
    (HI) = (VALUE)
                SKIN_BOUNDS(p->x, pmin_x, pmax_x);
                SKIN_BOUNDS(p->y, pmin_y, pmax_y);
                SKIN_BOUNDS(p->z, pmin_z, pmax_z);
                SKIN_BOUNDS(p->x, min_x, max_x);
                SKIN_BOUNDS(p->z, min_z, max_z);
#undef SKIN_BOUNDS
            }
            polygon->min_x = pmin_x - NuTimeBarTop;
            polygon->max_x = pmax_x + NuTimeBarTop;
            polygon->min_y = pmin_y - NuTimeBarTop;
            polygon->max_y = pmax_y + NuTimeBarTop;
            polygon->min_z = pmin_z - NuTimeBarTop;
            polygon->max_z = pmax_z + NuTimeBarTop;
            {
                NUVEC a, b;
                if (source->normals[1].y < BoltTargetRandomDenominator) {
                    a.x = polygon->vertices[1].x - polygon->vertices[3].x;
                    a.y = polygon->vertices[1].y - polygon->vertices[3].y;
                    a.z = polygon->vertices[1].z - polygon->vertices[3].z;
                    b.x = polygon->vertices[2].x - polygon->vertices[3].x;
                    b.y = polygon->vertices[2].y - polygon->vertices[3].y;
                    b.z = polygon->vertices[2].z - polygon->vertices[3].z;
                    TERRAIN_SKIN_NORMAL(&polygon->normals[1], &a, &b);
                }
                a.x = polygon->vertices[2].x - polygon->vertices[0].x;
                a.y = polygon->vertices[2].y - polygon->vertices[0].y;
                a.z = polygon->vertices[2].z - polygon->vertices[0].z;
                b.x = polygon->vertices[1].x - polygon->vertices[0].x;
                b.y = polygon->vertices[1].y - polygon->vertices[0].y;
                b.z = polygon->vertices[1].z - polygon->vertices[0].z;
                TERRAIN_SKIN_NORMAL(&polygon->normals[0], &a, &b);
            }
            ++source;
            ++destination;
            --count;
        }
        header->min_x = min_x - NuTimeBarTop;
        header->max_x = max_x + NuTimeBarTop;
        header->min_z = min_z - NuTimeBarTop;
        header->max_z = max_z + NuTimeBarTop;
        /* Advance to the next verified variable record after its polygons. */
        input = (TERRAIN_SKIN_STREAM_VIEW_PC *)source;
        output = (TERRAIN_SKIN_STREAM_VIEW_PC *)destination;
    }
    output->header.kind = -1;
    output->header.count = -1;
    group->data = output_base; /* original arg2, not terminal cursor */
}

#undef TERRAIN_SKIN_NORMAL

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
    i32 skin_index = -1 - (i32)group->scene_index;
    i32 selected = 0;
    i32 candidate;
    i32 oldest;
    i16 previous_skin;
    TERRAIN_SKIN_INFO_VIEW_PC *info;

    if (skin_index >= TerrainSkinInfoCountPc)
        return;

    TerrainSkinUseEpochPc += 1u;
    info = &TerrainSkinInfosPc[skin_index];
    if (group->data != NULL) {
        TerrainSkinUseRecordsPc[info->pool_slot].last_use = (i32)TerrainSkinUseEpochPc;
        return;
    }

    if (TerrainSkinUseRecordsPc[0].skin_index != -1) {
        oldest = TerrainSkinUseRecordsPc[0].last_use;
        for (candidate = 1; candidate < TerrainSkinPoolSlotCountPc; ++candidate) {
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
        TERRAIN_SET_PC *terrain = (TERRAIN_SET_PC *)CurrentTerrain;
        terrain->groups[TerrainSkinInfosPc[previous_skin].group_index].data = NULL;
        for (candidate = 0; candidate < 16; ++candidate)
            ((TERRAIN_SET_PC *)CurrentTerrain)->index_levels[candidate].cache_age = 0;
    }

    /* Field accesses are typed. The byte-pool calculation selects a record
     * boundary in caller-supplied storage; it does not bypass aggregate fields.
     * Keep negative scene_index / upper-only original guard unchanged.
     */
    info = &TerrainSkinInfosPc[skin_index];
    group->data = info->original_shape;
    info->pool_slot = (i16)selected;
    TerrainSkinUseRecordsPc[selected].skin_index = (i16)skin_index;
    SkinPlatform(group, (TERRAIN_SKIN_STREAM_VIEW_PC *)(TerrainSkinPoolBasePc + TerrainSkinPoolSlotBytesPc * selected),
                 &TerrainSkinInfosPc[skin_index]);
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
 * EShadowPolygonPc, ShadowRoofPolygonPc, GetSurfaceInfoWaterHeightPc,
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

void NewScanRot(NUVEC *position, i32 terrain_mask) {
    TERRAIN_SET_PC *terrain;
    TERRAIN_SHADOW_QUERY_VIEW_PC *query;
    i32 i, cell_index, cell_group, remaining, p, v;
    void *scratch;
    i32 cache_index;
    i16 oldest_age;
    i32 cache_hit;
    TERRAIN_INDEX_LEVEL_PC *cache;
    i32 fill_cache;
    TERRAIN_SCAN_WRITER_PC writer;
    f64 extent;
    f32 min_x;
    f32 max_x;
    f32 min_z;
    f32 max_z;

    terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    query = (TERRAIN_SHADOW_QUERY_VIEW_PC *)TerI;

    scratch = NuScratchAlloc32(0xd0);
    cache_index = 0;
    oldest_age = terrain->index_levels[0].cache_age;
    cache_hit = 0;
    for (i = 0; i < 16; ++i) {
        TERRAIN_INDEX_LEVEL_PC *cache;
        i16 age;

        cache = &terrain->index_levels[i];
        age = cache->cache_age;
        if (age > 0) {
            f32 dx;

            dx = (position->x + 1.0f) - cache->center_x;
            if (dx > 0.0f && dx < 2.0f) {
                f32 dz;

                dz = (position->z + 1.0f) - cache->center_z;
                if (dz > 0.0f && dz < 2.0f) {
                    cache_index = i;
                    cache_hit = 1;
                    break;
                }
            }
        }
        if (age < oldest_age) {
            oldest_age = age;
            cache_index = i;
        }
    }
    cache = &terrain->index_levels[cache_index];
    fill_cache = !cache_hit && cache->cache_age <= 1;

    writer.group_header = fill_cache ? cache->scan_words : query->scan_words;
    writer.cursor = writer.group_header + 1;
    // Target stops at TerI + 0x93c, leaving the final header slot available.
    writer.limit = writer.group_header + 509;
    writer.shape_count = 0;

    extent = fill_cache ? 1.0 : 0.10000000149011612;
    min_x = position->x - extent;
    max_x = position->x + extent;
    min_z = position->z - extent;
    max_z = position->z + extent;

    if (!cache_hit) {
        for (cell_index = 0; cell_index < terrain->used_cell_count; ++cell_index) {
            const TERRAIN_CELL_PC *cell;
            const i16 *group_indices;

            cell = &terrain->cells[cell_index];
            if (!(max_x >= cell->min_x && cell->max_x >= min_x && max_z >= cell->min_z && cell->max_z >= min_z)) {
                continue;
            }
            group_indices = terrain->group_indices + cell->first_group;
            for (cell_group = 0; cell_group < cell->group_count; ++cell_group) {
                i32 group_index;
                TERRAIN_GROUP_PC *group;
                f32 local_max_x;
                f32 local_min_x;
                f32 local_max_z;
                f32 local_min_z;
                TERRAIN_SKIN_STREAM_VIEW_PC *batch;

                group_index = group_indices[cell_group];
                group = &terrain->groups[group_index];
                if (max_x < group->bounds_min.x || max_z < group->bounds_min.z || group->bounds_max.x < min_x ||
                    group->bounds_max.z <= min_z || group->chunk_type == -1) {
                    continue;
                }
                local_max_x = max_x - group->origin.x;
                local_min_x = min_x - group->origin.x;
                local_max_z = max_z - group->origin.z;
                local_min_z = min_z - group->origin.z;
                if (group->scene_index < 0) {
                    TerrainSkinAllocate(group);
                }
                batch = group->data;
                while (batch->header.kind >= 0) {
                    TERRAIN_POLYGON_INFO_PC *shapes;
                    i32 count;

                    shapes = batch->polygons;
                    count = batch->header.count;
                    if (local_max_x >= batch->header.min_x && batch->header.max_x > local_min_x &&
                        local_max_z >= batch->header.min_z && batch->header.max_z > local_min_z) {
                        TERRAIN_POLYGON_INFO_PC *shape;

                        shape = shapes;
                        for (remaining = count; remaining > 0; --remaining, ++shape) {
                            if (shape->min_x <= local_max_x && local_min_x < shape->max_x &&
                                shape->min_z <= local_max_z && local_min_z < shape->max_z &&
                                writer.cursor < writer.limit &&
                                (shape->material[1] == 0 || (shape->material[1] & terrain_mask) != 0)) {
                                (writer.cursor++)->shape = shape;
                                ++writer.shape_count;
                            }
                        }
                    }
                    batch = (TERRAIN_SKIN_STREAM_VIEW_PC *)(shapes + count);
                }
                TERRAIN_FINISH_GROUP(writer, group_index);
            }
        }
        // Skin allocation can invalidate the cache while scanning groups.
        fill_cache = cache->cache_age <= 1;
    }
    if (fill_cache) {
        writer.group_header->header.shape_count = 0;
        writer.group_header->header.terrain_group = 0;
        cache->center_x = position->x;
        cache->center_z = position->z;
    }
    if (fill_cache || cache_hit) {
        TERRAIN_SCAN_WORD_PC *entry;

        cache->cache_age = 8;
        writer.group_header = query->scan_words;
        writer.cursor = writer.group_header + 1;
        writer.limit = writer.group_header + 509;
        min_x = position->x - 0.10000000149011612;
        max_x = position->x + 0.10000000149011612;
        min_z = position->z - 0.10000000149011612;
        max_z = position->z + 0.10000000149011612;
        entry = cache->scan_words;
        for (;;) {
            i16 count;
            i16 group_index;
            TERRAIN_SCAN_WORD_PC *shapes;
            TERRAIN_GROUP_PC *group;
            f32 local_min_x, local_max_x;
            f32 local_min_z, local_max_z;

            count = entry->header.shape_count;
            if (count <= 0)
                break;
            group_index = entry->header.terrain_group;
            shapes = entry + 1;
            entry = shapes + count;
            group = &terrain->groups[group_index];
            if (!SHADOW_BOUNDS_OVERLAP(min_x, min_z, max_x, max_z, group->bounds_min, group->bounds_max) ||
                group->chunk_type == -1)
                continue;
            local_min_x = min_x - group->origin.x;
            local_max_x = max_x - group->origin.x;
            local_min_z = min_z - group->origin.z;
            local_max_z = max_z - group->origin.z;
            for (i = 0; i < count; ++i) {
                TERRAIN_POLYGON_INFO_PC *shape;

                shape = shapes[i].shape;
                if (local_max_x < shape->min_x || shape->max_x <= local_min_x || local_max_z < shape->min_z ||
                    shape->max_z <= local_min_z)
                    continue;
                if (shape->material[1] != 0 && (shape->material[1] & terrain_mask) == 0)
                    continue;
                if (writer.cursor >= writer.limit)
                    continue;
                (writer.cursor++)->shape = shape;
                ++writer.shape_count;
            }
            TERRAIN_FINISH_GROUP(writer, group_index);
        }
    }

    if (TerrainPlatformsOffPc == 0) {
        i16 *platform_groups;
        i32 platform_count;
        NUVEC4 *vertices;
        i32 transformed_count;

        min_x -= 0.05000000074505806;
        min_z -= 0.05000000074505806;
        max_x += 0.05000000074505806;
        max_z += 0.05000000074505806;
        platform_groups = terrain->active_platform_groups;
        platform_count = terrain->active_platform_count;
        if (!(min_x > terrain->platform_query_min_x && max_x < terrain->platform_query_max_x &&
              min_z > terrain->platform_query_min_z && max_z < terrain->platform_query_max_z)) {
            TERRAIN_CELL_PC *cell;

            cell = &terrain->platform_cell;
            platform_groups = terrain->group_indices + cell->first_group;
            platform_count = cell->group_count;
        }
        vertices = (NUVEC4 *)(((u32)scratch + 31) & ~(u32)15);
        transformed_count = 0;
        for (p = 0; p < platform_count; ++p) {
            i32 group_index;
            TERRAIN_GROUP_PC *group;
            TERRAIN_PLATFORM_PC *platform;
            f32 local_min_x, local_max_x;
            f32 local_min_z, local_max_z;
            NUMTX *matrix;
            TERRAIN_SCENE_STATE_VIEW_PC *scene_state;
            i32 rotating;
            TERRAIN_SKIN_STREAM_VIEW_PC *batch;

            group_index = platform_groups[p];
            group = &terrain->groups[group_index];
            if (group->origin.x - group->radius > max_x || min_x > group->origin.x + group->radius ||
                group->origin.z - group->radius > max_z || min_z > group->origin.z + group->radius)
                continue;
            platform = &terrain->platforms[group->scene_index];
            local_min_x = min_x - group->origin.x;
            local_max_x = max_x - group->origin.x;
            local_min_z = min_z - group->origin.z;
            local_max_z = max_z - group->origin.z;
            matrix = (NUMTX *)platform->scene_object;
            scene_state = (TERRAIN_SCENE_STATE_VIEW_PC *)platform->scene_transform;
            if (matrix != NULL) {
                if (matrix->m30 > platform->previous_matrix.m30)
                    local_max_x = (matrix->m30 - platform->previous_matrix.m30) * 1.5 + max_x - group->origin.x;
                else
                    local_min_x = (matrix->m30 - platform->previous_matrix.m30) * 1.5 + min_x - group->origin.x;
                if (matrix->m32 > platform->previous_matrix.m32)
                    local_max_z = (matrix->m32 - platform->previous_matrix.m32) * 1.5 + max_z - group->origin.z;
                else
                    local_min_z = (matrix->m32 - platform->previous_matrix.m32) * 1.5 + min_z - group->origin.z;
            }
            rotating = (platform->flags & 1) != 0;
            if (rotating) {
                volatile f32 distance_squared;

                if (group->chunk_type == -1)
                    continue;

                distance_squared =
                    (((f64)max_x + min_x) * 0.5 - group->origin.x) * (((f64)max_x + min_x) * 0.5 - group->origin.x) +
                    (((f64)min_z + max_z) * 0.5 - group->origin.z) * (((f64)min_z + max_z) * 0.5 - group->origin.z);
                if (!(((f64)group->radius + TerrainScanRadiusEpsilon) *
                          ((f64)group->radius + TerrainScanRadiusEpsilon) >
                      distance_squared))
                    continue;
            } else if (!SHADOW_BOUNDS_OVERLAP(local_min_x, local_min_z, local_max_x, local_max_z, group->bounds_min,
                                              group->bounds_max) ||
                       group->chunk_type == -1) {
                continue;
            }
            if (platform->scene_transform != NULL &&
                ((platform->flags & 4) != 0 ? (scene_state->flags & 2) == 0 : (scene_state->low_flags & 1) == 0))
                continue;
            batch = group->data;
            while (batch->header.kind >= 0) {
                TERRAIN_POLYGON_INFO_PC *shapes;
                i32 batch_overlaps;

                shapes = batch->polygons;
                batch_overlaps = rotating || (local_max_x >= batch->header.min_x && batch->header.max_x > local_min_x &&
                                              local_max_z >= batch->header.min_z && batch->header.max_z > local_min_z);
                for (i = 0; batch_overlaps && i < batch->header.count; ++i) {
                    TERRAIN_POLYGON_INFO_PC *shape;

                    shape = &shapes[i];
                    if (shape->material[1] != 0 && (shape->material[1] & terrain_mask) == 0)
                        continue;
                    if (writer.cursor >= writer.limit)
                        continue;
                    if (!rotating) {
                        if (local_max_x < shape->min_x || shape->max_x <= local_min_x || local_max_z < shape->min_z ||
                            shape->max_z <= local_min_z)
                            continue;
                    } else {
                        i32 quad;
                        i32 above_x, below_x, above_z, below_z;
                        TERRAIN_POLYGON_INFO_PC *transformed;
                        f32 length;
                        f32 inverse;

                        for (v = 0; v < 3; ++v) {
                            vertices[v].x = shape->vertices[v].x;
                            vertices[v].y = shape->vertices[v].y;
                            vertices[v].z = shape->vertices[v].z;
                            vertices[v].w = 0.0f;
                        }
                        NuVec4MtxTransformVU0x3(vertices, vertices, matrix);
                        quad = shape->normals[1].y < BoltTargetRandomDenominator;
                        if (quad) {
                            vertices[3].x = shape->vertices[3].x;
                            vertices[3].y = shape->vertices[3].y;
                            vertices[3].z = shape->vertices[3].z;
                            vertices[3].w = 0.0f;
                            NuVec4MtxTransformVU0(&vertices[3], &vertices[3], matrix);
                        } else
                            vertices[3] = vertices[2];
                        above_x = 0;
                        below_x = 0;
                        above_z = 0;
                        below_z = 0;
                        for (v = 0; v < 4; ++v) {
                            above_x |= vertices[v].x > local_min_x;
                            below_x |= local_max_x > vertices[v].x;
                            above_z |= vertices[v].z > local_min_z;
                            below_z |= local_max_z > vertices[v].z;
                        }
                        if (!above_x || !below_x || !above_z || !below_z)
                            continue;
                        transformed = &TerrainScaledShapeCursorPc[transformed_count];
                        transformed->packed_metadata = shape->packed_metadata;
                        for (v = 0; v < (quad ? 4 : 3); ++v) {
                            transformed->vertices[v].x = vertices[v].x;
                            transformed->vertices[v].y = vertices[v].y;
                            transformed->vertices[v].z = vertices[v].z;
                        }
                        if (!quad)
                            transformed->normals[1].y = TerrainScanTriangleSentinel;
                        if (quad) {
                            f32 length;
                            f32 inverse;

                            vertices[0].x = transformed->vertices[1].x - transformed->vertices[3].x;
                            vertices[0].y = transformed->vertices[1].y - transformed->vertices[3].y;
                            vertices[0].z = transformed->vertices[1].z - transformed->vertices[3].z;
                            vertices[1].x = transformed->vertices[2].x - transformed->vertices[3].x;
                            vertices[1].y = transformed->vertices[2].y - transformed->vertices[3].y;
                            vertices[1].z = transformed->vertices[2].z - transformed->vertices[3].z;
                            TERRAIN_CROSS(transformed->normals[1], vertices[0], vertices[1]);
                            length = NuFsqrt((transformed->normals[1].x * transformed->normals[1].x +
                                              transformed->normals[1].y * transformed->normals[1].y) +
                                             transformed->normals[1].z * transformed->normals[1].z);
                            inverse = length == 0.0f ? 0.0f : 1.0f / length;
                            transformed->normals[1].x *= inverse;
                            transformed->normals[1].y *= inverse;
                            transformed->normals[1].z *= inverse;
                        }
                        vertices[0].x = transformed->vertices[2].x - transformed->vertices[0].x;
                        vertices[0].y = transformed->vertices[2].y - transformed->vertices[0].y;
                        vertices[0].z = transformed->vertices[2].z - transformed->vertices[0].z;
                        vertices[1].x = transformed->vertices[1].x - transformed->vertices[0].x;
                        vertices[1].y = transformed->vertices[1].y - transformed->vertices[0].y;
                        vertices[1].z = transformed->vertices[1].z - transformed->vertices[0].z;
                        TERRAIN_CROSS(transformed->normals[0], vertices[0], vertices[1]);
                        length = NuFsqrt((transformed->normals[0].x * transformed->normals[0].x +
                                          transformed->normals[0].y * transformed->normals[0].y) +
                                         transformed->normals[0].z * transformed->normals[0].z);
                        inverse = length == 0.0f ? 0.0f : 1.0f / length;
                        transformed->normals[0].x *= inverse;
                        transformed->normals[0].y *= inverse;
                        transformed->normals[0].z *= inverse;
                        shape = transformed;
                        ++transformed_count;
                    }
                    (writer.cursor++)->shape = shape;
                    ++writer.shape_count;
                }
                batch = (TERRAIN_SKIN_STREAM_VIEW_PC *)(shapes + batch->header.count);
            }
            TERRAIN_FINISH_GROUP(writer, group_index);
        }
    }
    NuScratchRelease();
    writer.group_header->header.shape_count = 0;
    writer.group_header->header.terrain_group = 0;
}

f32 NewCast(NUVEC *position, f32 height_above, f32 roof_range) {
    TERRAIN_SET_PC *terrain;
    TERRAIN_SHADOW_QUERY_VIEW_PC *query;
    i32 remaining;
    volatile f32 plane_quotient;
    TERRAIN_SCAN_WORD_PC *scan;
    TERRAIN_POLYGON_INFO_PC *ordinary_above;
    TERRAIN_POLYGON_INFO_PC *ordinary_floor;
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
        i16 group_index;
        f32 x;
        f32 z;
        TERRAIN_SCAN_WORD_PC *surfaces;
        TERRAIN_SCAN_WORD_PC *entry;

        if (shape_count <= 0) {
            if (!(LevelConfigFarDistance > extended_above_y) || !(extended_above_normal.y < 0.0f) ||
                !(roof_range + position->y > extended_above_y)) {
                TerrainCastAuxPolygonPc = NULL;
                TerrainShadowHeightPc = LevelConfigFarDistance;
            } else if (!(LevelConfigFarDistance > ordinary_above_y) || !(ordinary_above_normal.y < 0.0f) ||
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
            if (!(LevelConfigFarDistance > extended_above_y) || !(extended_above_normal.y > 0.0f) ||
                !(roof_range + position->y > extended_above_y)) {
                if (extended_floor_y <= TerrainCastLowerSentinel ||
                    (TerrainCastLowerSentinel < ordinary_floor_y && extended_floor_y < ordinary_floor_y)) {
                    TerrainWaterCastIndexPc = 3;
                    GetSurfaceInfoWaterHeightPc = LevelConfigFarDistance;
                    EShadowPolygonPc = NULL;
                    GetSurfaceInfoWaterNormalPc.y = 1.0f;
                } else {
                    TerrainWaterCastIndexPc = 2;
                    GetSurfaceInfoWaterHeightPc = extended_floor_y;
                    EShadowPolygonPc = extended_floor;
                    GetSurfaceInfoWaterNormalPc = extended_floor_normal;
                }
            } else if (!(LevelConfigFarDistance > ordinary_above_y) || !(ordinary_above_normal.y < 0.0f) ||
                       !(roof_range + position->y > ordinary_above_y) || !(extended_above_y > ordinary_above_y)) {
                EShadowPolygonPc = extended_above;
                GetSurfaceInfoWaterHeightPc = extended_above_y;
                GetSurfaceInfoWaterNormalPc = extended_above_normal;
                TerrainWaterCastIndexPc = 1;
                TerrainShadowFloorIndexPc = extended_above_group;
            } else {
                TerrainWaterCastIndexPc = 3;
                GetSurfaceInfoWaterHeightPc = LevelConfigFarDistance;
                GetSurfaceInfoWaterNormalPc.y = 1.0f;
                EShadowPolygonPc = NULL;
            }
            if (!(LevelConfigFarDistance > ordinary_above_y) || !(ordinary_above_normal.y < 0.0f) ||
                !(roof_range + position->y > ordinary_above_y)) {
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
        for (remaining = shape_count; remaining != 0; --remaining, ++entry) {
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
                        } else if (CAST_LINE(0, 1) != 0 && CAST_LINE(2, 0) != 0) {
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
                            if (normal_y == 0.0f)
                                goto first_plane;
                            normal_z = surface->normals[1].z;
                            normal_x = surface->normals[1].x;
                            plane_quotient =
                                ((x - surface->vertices[3].x) * normal_x + (z - surface->vertices[3].z) * normal_z) /
                                -normal_y;
                            plane_y = (terrain->groups[group_index].origin.y + surface->vertices[3].y) + plane_quotient;
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
                            } else if (position->y < plane_y && plane_y <= ordinary_above_y &&
                                       (plane_y != ordinary_above_y || normal_y < ordinary_above_normal.y)) {
                                ordinary_above_normal.x = normal_x;
                                ordinary_above_normal.y = normal_y;
                                ordinary_above_normal.z = normal_z;
                                ordinary_above = surface;
                                ordinary_above_y = plane_y;
                                ordinary_above_group = group_index;
                            }
                        } else {
                            if (plane_y <= position->y && extended_floor_y < plane_y && winding == 1) {
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
                if (normal_y < 0.0f && CAST_LINE(0, 1) != 0 && CAST_LINE(2, 0) != 0 && CAST_LINE(1, 2) != 0) {
                    winding = 2;
                    goto first_plane;
                }
            } else {
                if (CAST_LINE(1, 0) != 0 && CAST_LINE(0, 2) != 0 && CAST_LINE(2, 1) != 0) {
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
    GetSurfaceInfoWaterHeightPc = LevelConfigFarDistance;
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
    terrain->groups[group_index].chunk_type = enabled != 0 ? 1 : -1;
}

void NewTerrPlatformsOff(void) {
    TerrainPlatformsOffPc = 1;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
void TerrainDiagnosticNoOpPC(const void *, ...);
u8 *TerrainCrashDataPc;
const f32 TerrainUnusedSpatialBlockPc;
i32 ReadTerrain(unsigned char *base_path, i32 first_group, i16 **buffer, TERRSET *terrain) {
    i32 diagnostic_index;
    i32 point;
    char path[100];
    i32 loaded_bytes;
    u8 *file_start;
    TERRAIN_FILE_CHUNK *chunk;
    i16 chunk_count;
    i32 chunk_index;
    i32 group_count;
    u8 *chunk_data;
    f32 origin_x;
    f32 kUnusedBlock;
    i32 point_count;
    TERRAIN_SPATIAL_NODE *block;
    i32 first_point;
    f32 min_x;
    f32 max_x;
    f32 min_z;
    f32 max_z;

    TerrainCrashDataPc = NULL;
    TerrainDiagnosticNoOpPC(buffer);

    strcpy(path, ((char *)(base_path)));
    strcat(path, ".ter");
    loaded_bytes = NuFileLoadBuffer(path, *buffer, 0x7fffffff);
    TerrainDiagnosticNoOpPC(path, loaded_bytes);
    if (loaded_bytes == 0) {
        return -1;
    }

    file_start = ((u8 *)(*buffer));
    chunk = ((TERRAIN_FILE_CHUNK *)(file_start + 2 * *((u32 *)(file_start))));
    chunk_count = chunk->chunk_count;
    for (diagnostic_index = 0; diagnostic_index < chunk_count; ++diagnostic_index) {
        if (chunk[diagnostic_index].field_16 == 0x1ca)
            TerrainDiagnosticNoOpPC(path);
    }
    *buffer = ((i16 *)(file_start + sizeof(u32)));

    if (chunk_count <= 0) {
        terrain->file_version = chunk->version;
        terrain->spatial_nodes = NULL;
        return 0;
    }

    terrain->file_version = chunk->version;
    terrain->spatial_nodes = NULL;

    chunk_index = 0;
    group_count = 0;
    do {
        chunk_data = ((u8 *)(*buffer));

        if (chunk->chunk_type == TERRAIN_CHUNK_SPATIAL_INDEX) {
            TERRAIN_SPATIAL_NODE **link = ((TERRAIN_SPATIAL_NODE **)(chunk_data));
            if (*((u32 *)(link)) == 0x12345678) {
                *link = terrain->spatial_nodes;
                terrain->spatial_nodes = ((TERRAIN_SPATIAL_NODE *)(link + 1));
                --terrain->spatial_nodes->point_count;
            }
        } else if (chunk->chunk_type <= TERRAIN_CHUNK_SPATIAL_INDEX) {
            if (chunk->chunk_type >= TERRAIN_CHUNK_GROUP_PRIMARY) {
                origin_x = chunk->origin.x;
                terrain->groups[first_group].group_info = chunk->group_info;
                terrain->groups[first_group].chunk_type = chunk->chunk_type;
                terrain->groups[first_group].origin.x = origin_x;
                terrain->groups[first_group].origin.y = chunk->origin.y;
                terrain->groups[first_group].scene_index = chunk->field_16;
                terrain->groups[first_group].origin.z = chunk->origin.z;
                terrain->groups[first_group].data = ((TERRAIN_SKIN_STREAM_VIEW_PC *)(chunk_data));
                terrain->groups[first_group].platform_flags = chunk->platform_flags;
                ++group_count;
                ++first_group;
            }
        } else if (chunk->chunk_type == TERRAIN_CHUNK_CRASH_DATA) {
            TerrainCrashDataPc = chunk_data;
        }

        *buffer = ((i16 *)(chunk_data + 2 * chunk->data_word_count));
        ++chunk_index;
        ++chunk;
    } while (chunk_index != chunk_count);

    {
        TERRAIN_SPATIAL_NODE *node = terrain->spatial_nodes;

        // Every block covers sixteen terrain cells and therefore seventeen
        // boundary points.  The original stores the aggregate X/Z bounds in
        // the otherwise-unused Y components of the first four points.
        kUnusedBlock = TerrainUnusedSpatialBlockPc;

        while (node != NULL) {
            point_count = node->point_count;
            if (point_count != 0) {
                block = node;
                first_point = 0;

                do {
                    if (block->points[0].y != kUnusedBlock) {
                        if (first_point + 15 < point_count) {
                            min_x = block->points[0].x;
                            max_x = block->points[0].x;
                            min_z = block->points[0].z;
                            max_z = block->points[0].z;
                            for (point = 1; point < 17; ++point) {
                                min_x = MIN(block->points[point].x, min_x);
                                max_x = MAX(block->points[point].x, max_x);
                                min_z = MIN(block->points[point].z, min_z);
                                max_z = MAX(block->points[point].z, max_z);
                            }
                            block->points[0].y = min_x;
                            block->points[1].y = max_x;
                            block->points[2].y = min_z;
                            block->points[3].y = max_z;
                        } else {
                            block->points[0].y = kUnusedBlock;
                        }
                    }

                    first_point += 16;
                    block = ((TERRAIN_SPATIAL_NODE *)(((u8 *)(block)) + 16 * sizeof(NUVEC)));
                } while (first_point < point_count);
            }

            // The file starts each spatial chunk with a link word followed by
            // the node itself. ReadTerrain replaces that word with the prior
            // list head while linking the chunks.
            node = ((TERRAIN_SPATIAL_CHUNK *)(((char *)(node)) - offsetof(TERRAIN_SPATIAL_CHUNK, node)))->previous;
        }
    }
    return group_count;
}

extern const char TerrainSourcePathPc[];
extern const f32 TerrainBoundsMinimumPc, TerrainBoundsMaximumPc, TerrainSteepNormalYPc;
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
    cursor->char_ptr += (max_groups + 0x80) * sizeof(TERRAIN_GROUP);
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

    kBoundsMinimum = TerrainBoundsMinimumPc;
    kBoundsMaximum = TerrainBoundsMaximumPc;
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
                if (shape->min_z < min_z)
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
    terrain_max_x = kBoundsMinimum;
    terrain_max_z = kBoundsMinimum;

    for (group_index = 0; group_index < group_count; ++group_index) {
        group = &(terrain->groups[group_index]);
        if (group->chunk_type != TERRAIN_CHUNK_GROUP_PRIMARY) {
            continue;
        }

        batch = ((TERRAIN_SHAPE_BATCH *)(group->data));
        while (batch->marker >= 0) {
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
        platform->flags = (platform->flags & ~5u) | (group->platform_flags & 1);
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
    TERRAIN_SET_PC *terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    if (terrain == NULL)
        return;
    curPickInst = 0;
    for (i = 0; i < ((TERRAIN_SET_PC *)CurrentTerrain)->removed_platform_count; ++i)
        DeletePlatinst(((TERRAIN_SET_PC *)CurrentTerrain)->removed_platforms[i]);
    terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    terrain->removed_platform_count = 0;
    curSphereter = 0;
    for (i = 0; i < ((TERRAIN_SET_PC *)CurrentTerrain)->max_platforms; ++i) {
        TERRAIN_PLATFORM_LIFECYCLE_PC *platform =
            &((TERRAIN_PLATFORM_LIFECYCLE_PC *)((TERRAIN_SET_PC *)CurrentTerrain)->platforms)[i];
        if (platform->scene_matrix != NULL)
            platform->previous_matrix = *platform->scene_matrix;
    }
}

void __cdecl TerrainPlatformNewUpdate(void) {
    TERRAIN_SET_PC *terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    i32 i;
    f32 negative_distance, x, z;
    f64 back_x, back_z, product_x, product_z;
    f32 min_x, min_z, max_x, max_z;
    i16 *group_index, *end;
    const NUMTX *camera = &global_camera.mtx;
    if (terrain != NULL) {
        for (i = 0; i < 64; ++i) {
            TERRAIN_TRACK_SLOT_PC *slot = &terrain->track_slots[i];
            if (slot->id != NULL) {
                if (slot->platform_contact_state > 0)
                    --slot->platform_contact_state;
                if (slot->wall_contact_state > 0)
                    --slot->wall_contact_state;
            }
        }
        for (i = 0; i < 16; ++i) {
            if (terrain->index_levels[i].cache_age > 0)
                --terrain->index_levels[i].cache_age;
        }
    }
    for (i = 0; i < PlatCodeCallback; ++i)
        PlatCallback[i].function(PlatCallback[i].argument);
    PlatCodeCallback = 0;
    cntrots = 0;
    terrain = (TERRAIN_SET_PC *)CurrentTerrain;
    if (terrain == NULL)
        return;
    negative_distance = -TerrPlatScanDist;
    back_x = (f64)TerrPlatScanDist * camera->m20 * 0.25;
    back_z = (f64)TerrPlatScanDist * camera->m22 * 0.25;
    product_x = (f64)TerrPlatScanDist * camera->m00;
    product_z = (f64)TerrPlatScanDist * camera->m02;
    min_x = max_x = camera->m00 * negative_distance * 0.25 - back_x;
    min_z = max_z = camera->m02 * negative_distance * 0.25 - back_z;
    x = camera->m20 * (f64)TerrPlatScanDist - product_x * (f64)0.35f;
    z = camera->m22 * (f64)TerrPlatScanDist - product_z * (f64)0.35f;
    if (min_x > x)
        min_x = x;
    if (max_x < x)
        max_x = x;
    if (min_z > z)
        min_z = z;
    if (max_z < z)
        max_z = z;
    x = camera->m20 * (f64)TerrPlatScanDist + product_x * (f64)0.35f;
    z = camera->m22 * (f64)TerrPlatScanDist + product_z * (f64)0.35f;
    if (min_x > x)
        min_x = x;
    if (max_x < x)
        max_x = x;
    if (min_z > z)
        min_z = z;
    if (max_z < z)
        max_z = z;
    x = product_x * 0.25 - back_x;
    z = product_z * 0.25 - back_z;
    if (min_x > x)
        min_x = x;
    if (max_x < x)
        max_x = x;
    if (min_z > z)
        min_z = z;
    if (max_z < z)
        max_z = z;
    min_x += camera->m30 - 1.0;
    max_x += camera->m30 + 1.0;
    min_z += camera->m32 - 1.0;
    max_z += camera->m32 + 1.0;
    terrain->platform_query_min_x = min_x;
    terrain->platform_query_min_z = min_z;
    terrain->platform_query_max_x = max_x;
    terrain->platform_query_max_z = max_z;
    terrain->active_platform_count = 0;
    group_index = terrain->group_indices + terrain->platform_cell.first_group;
    end = group_index + terrain->platform_cell.group_count;
    for (; group_index < end; ++group_index) {
        TERRAIN_GROUP_PC *group = &terrain->groups[*group_index];
        TERRAIN_PLATFORM_LIFECYCLE_PC *platform =
            &((TERRAIN_PLATFORM_LIFECYCLE_PC *)terrain->platforms)[group->scene_index];
        NUMTX *matrix;
        u32 flags;
        if (platform->scene_transform != NULL) {
            u32 visibility = platform->scene_transform->flags;
            if ((platform->flags & 4) != 0) {
                if ((visibility & 2) == 0)
                    continue;
            } else if ((visibility & 1) == 0)
                continue;
        }
        if (group->chunk_type == 0)
            continue;
        matrix = platform->scene_matrix;
        if (matrix != NULL) {
            if (platform->bounce_frames != 0) {
                f32 offset;
                if (platform->bounce_frames > 124)
                    platform->bounce_velocity =
                        (platform->bounce_impulse - platform->bounce_damping * platform->bounce_velocity) +
                        platform->bounce_velocity;
                else
                    platform->bounce_velocity -= platform->bounce_damping * platform->bounce_velocity;
                offset = platform->bounce_offset;
                platform->bounce_velocity +=
                    -offset * fabsf(offset) * platform->bounce_spring * 0.5 - platform->bounce_spring * offset;
                platform->bounce_offset += platform->bounce_velocity;
                --platform->bounce_frames;
                matrix->m31 += platform->bounce_offset;
            }
            group->origin.x = matrix->m30;
            group->origin.y = matrix->m31;
            group->origin.z = matrix->m32;
        }
        flags = platform->flags;
        if ((flags & 2) != 0)
            platform->bounce_frames = 128;
        platform->flags &= ~2U;
        platform->motion_status = 0;
        if ((flags & 1) == 0) {
            if (group->bounds_min.x + group->origin.x > max_x || group->origin.x + group->bounds_max.x < min_x ||
                group->bounds_min.z + group->origin.z > max_z || group->origin.z + group->bounds_max.z < min_z)
                continue;
        } else {
            if (group->origin.x - group->radius > max_x || group->origin.x + group->radius < min_x ||
                group->origin.z - group->radius > max_z || group->origin.z + group->radius < min_z)
                continue;
        }
        if (terrain->active_platform_count < 96)
            terrain->active_platform_groups[terrain->active_platform_count++] = *group_index;
    }
}
