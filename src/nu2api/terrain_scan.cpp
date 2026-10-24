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

extern "C" {

#define MIN(a, b) ((a) < (b) ? (a) : (b))
    extern i32 WallSplinesOnly, IgnoreWallSplines, WallSplCount, curPickInst;
    extern void *TerI, *TerrOverRideScan;
    extern TERRAIN_POLYGON_INFO_PC *TerrainScaledShapeCursorPc;
    extern const f64 TerrainScanMargin02Pc, TerrainScanMargin10Pc;
    void __cdecl ScanWallSplineTerrain(i32, i32, i32);
    f32 __cdecl TerrainDivideNonzeroPC(f32, f32);
    void __cdecl TerrainSkinAllocate(TERRAIN_GROUP_PC *);
    enum { TERRAIN_PLATFORM_FLAG_ROTATING = 1 };
}
typedef struct TerrainQuery_s TerrainQuery_s;
struct TerrainQuery_s {
    u32 flags;
    NUVEC start_position;
    NUVEC start_movement;
    NUVEC position;
    NUVEC movement;
    u8 scan_result;
    u8 shape_adjusted;
    i16 object_index;
    f32 radius;
    f32 horizontal_movement_length;
    u8 *hit_flags;
    f32 movement_pitch;
    f32 movement_yaw;
    f32 movement_length;
    f32 collision_radius;
    f32 collision_radius_sq;
    f32 inverse_collision_radius;
    f32 object_scale;
    f32 object_scale_sq;
    f32 inverse_object_scale;
    f32 inverse_object_scale_sq;
    i16 terrain_group_index;
    u8 pad_0x06e[2];
    NUVEC local_start;
    NUVEC local_end;
    u8 hit_type;
    u8 hit_edge;
    i16 scan_group_index;
    void *scan_list;
    TERRAIN_SHAPE *surface;
    void *working_surface;
    f32 hit_time;
    f32 unclamped_hit_time;
    f32 separation_epsilon;
    f32 compare_epsilon;
    NUVEC movement_normal;
    NUVEC impact_normal;
    NUVEC platform_position;
    NUVEC platform_normal;
    NUVEC previous_movement_normal;
    u8 pad_0x0e4[0x18];
    NUVEC transformed_vertices[4];
    u8 pad_0x12c[0x1c];
    u8 scan_list_storage[0x800];
};
STATIC_ASSERT(sizeof(TerrainQuery_s) == 0x948);
STATIC_ASSERT(offsetof(TERRAIN_PREVIOUS_MATRIX_VIEW_PC, m31) == 0x34);
STATIC_ASSERT(offsetof(TerrainQuery_s, position) == 0x1c);
STATIC_ASSERT(offsetof(TerrainQuery_s, movement) == 0x28);
STATIC_ASSERT(offsetof(TerrainQuery_s, scan_result) == 0x34);
STATIC_ASSERT(offsetof(TerrainQuery_s, collision_radius) == 0x50);
STATIC_ASSERT(offsetof(TerrainQuery_s, object_scale) == 0x5c);
STATIC_ASSERT(offsetof(TerrainQuery_s, object_scale_sq) == 0x60);
STATIC_ASSERT(offsetof(TerrainQuery_s, inverse_object_scale) == 0x64);
STATIC_ASSERT(offsetof(TerrainQuery_s, scan_group_index) == 0x8a);
STATIC_ASSERT(offsetof(TerrainQuery_s, scan_list) == 0x8c);
STATIC_ASSERT(offsetof(TerrainQuery_s, scan_list_storage) == 0x148);
typedef struct TerrainScanBounds TerrainScanBounds;
struct TerrainScanBounds {
    f32 min_x;
    f32 min_y;
    f32 min_z;
    f32 max_x;
    f32 max_y;
    f32 max_z;
};
#define TERRAIN_QUERY_PC (static_cast<TerrainQuery_s *>(TerI))
#define TERRAIN_SET_CURRENT_PC (static_cast<TERRAIN_SET_PC *>(CurrentTerrain))
#define TERRAIN_PLATFORM_OF_PC(group_arg) (TERRAIN_SET_CURRENT_PC->platforms[(group_arg).scene_index])
#define TERRAIN_PLATFORM_MATRIX_PC(group_arg) (static_cast<NUMTX *>(TERRAIN_PLATFORM_OF_PC(group_arg).scene_object))
#define TERRAIN_CROSS_PRODUCT_PC(result_arg, a_arg, b_arg)                                                             \
    do {                                                                                                               \
        NUVEC cross;                                                                                                   \
        cross.x = (a_arg).y * (b_arg).z - (a_arg).z * (b_arg).y;                                                       \
        cross.y = (a_arg).z * (b_arg).x - (a_arg).x * (b_arg).z;                                                       \
        cross.z = (a_arg).x * (b_arg).y - (a_arg).y * (b_arg).x;                                                       \
        (result_arg) = cross;                                                                                          \
    } while (0)
#define TERRAIN_FINISH_SCAN_GROUP_PC(group_index_arg)                                                                  \
    do {                                                                                                               \
        if (group_shape_count != 0) {                                                                                  \
            i16 *header = reinterpret_cast<i16 *>(group_header);                                                       \
            header[0] = static_cast<i16>(group_shape_count);                                                           \
            header[1] = static_cast<i16>(group_index_arg);                                                             \
            group_header = reinterpret_cast<u8 *>(cursor);                                                             \
            cursor = reinterpret_cast<TERRAIN_SHAPE **>(group_header + sizeof(TERRAIN_SHAPE *));                       \
            group_shape_count = 0;                                                                                     \
        }                                                                                                              \
    } while (0)
#define TERRAIN_SCALE_SCAN_SHAPE(candidate_arg, group_arg)                                                             \
    do {                                                                                                               \
        if (query->object_scale != 1.0f) {                                                                             \
            TERRAIN_SHAPE *source = (candidate_arg);                                                                   \
            TERRAIN_SHAPE *scaled = &TerrainScaledShapeCursorPc[scaled_shape_count];                                   \
            scaled->material[0] = source->material[0];                                                                 \
            scaled->material[1] = source->material[1];                                                                 \
            scaled->flags = source->flags;                                                                             \
            scaled->normal_flags = source->normal_flags;                                                               \
            scaled->vertices[0].x = source->vertices[0].x;                                                             \
            scaled->vertices[0].z = source->vertices[0].z;                                                             \
            scaled->vertices[0].y =                                                                                    \
                (source->vertices[0].y + (group_arg).origin.y) * query->inverse_object_scale - (group_arg).origin.y;   \
            scaled->vertices[1].x = source->vertices[1].x;                                                             \
            scaled->vertices[1].z = source->vertices[1].z;                                                             \
            scaled->vertices[1].y =                                                                                    \
                (source->vertices[1].y + (group_arg).origin.y) * query->inverse_object_scale - (group_arg).origin.y;   \
            scaled->vertices[2].x = source->vertices[2].x;                                                             \
            scaled->vertices[2].z = source->vertices[2].z;                                                             \
            scaled->vertices[2].y =                                                                                    \
                (source->vertices[2].y + (group_arg).origin.y) * query->inverse_object_scale - (group_arg).origin.y;   \
            if (source->normals[1].y < 65535.0f) {                                                                     \
                scaled->vertices[3].x = source->vertices[3].x;                                                         \
                scaled->vertices[3].z = source->vertices[3].z;                                                         \
                scaled->vertices[3].y = (source->vertices[3].y + (group_arg).origin.y) * query->inverse_object_scale - \
                                        (group_arg).origin.y;                                                          \
                const f32 inverse_length = TerrainDivideNonzeroPC(                                                     \
                    1.0f, NuFsqrt(source->normals[1].x * source->normals[1].x +                                        \
                                  source->normals[1].y * source->normals[1].y * query->object_scale_sq +               \
                                  source->normals[1].z * source->normals[1].z));                                       \
                query = TERRAIN_QUERY_PC;                                                                              \
                scaled = &TerrainScaledShapeCursorPc[scaled_shape_count];                                              \
                scaled->normals[1].x = source->normals[1].x * inverse_length;                                          \
                scaled->normals[1].y = query->object_scale * source->normals[1].y * inverse_length;                    \
                scaled->normals[1].z = source->normals[1].z * inverse_length;                                          \
            } else {                                                                                                   \
                scaled->normals[1].y = 65536.0f;                                                                       \
            }                                                                                                          \
            const f32 inverse_length = TerrainDivideNonzeroPC(                                                         \
                1.0f, NuFsqrt(source->normals[0].x * source->normals[0].x +                                            \
                              source->normals[0].y * source->normals[0].y * query->object_scale_sq +                   \
                              source->normals[0].z * source->normals[0].z));                                           \
            query = TERRAIN_QUERY_PC;                                                                                  \
            scaled = &TerrainScaledShapeCursorPc[scaled_shape_count];                                                  \
            scaled->normals[0].x = source->normals[0].x * inverse_length;                                              \
            scaled->normals[0].y = query->object_scale * source->normals[0].y * inverse_length;                        \
            scaled->normals[0].z = source->normals[0].z * inverse_length;                                              \
            ++scaled_shape_count;                                                                                      \
            (candidate_arg) = scaled;                                                                                  \
        }                                                                                                              \
    } while (0)

#define TERRAIN_SCAN_BOUNDS_PC(query_arg)                                                                              \
    if ((query_arg)->scan_result != 1) {                                                                               \
        f32 length_sq = (query_arg)->movement.x * (query_arg)->movement.x +                                            \
                        (query_arg)->movement.y * (query_arg)->movement.y +                                            \
                        (query_arg)->movement.z * (query_arg)->movement.z;                                             \
        scan_radius = NuFsqrt(length_sq);                                                                              \
        (query_arg) = TERRAIN_QUERY_PC;                                                                                \
        scan_radius = (query_arg)->collision_radius + TerrainScanMargin10Pc + scan_radius;                             \
        max_x = (query_arg)->position.x + NuTimeBarTop + scan_radius;                                                  \
        max_y = (query_arg)->position.y + NuTimeBarTop + (query_arg)->object_scale * scan_radius;                      \
        max_z = (query_arg)->position.z + NuTimeBarTop + scan_radius;                                                  \
        min_x = (query_arg)->position.x - NuTimeBarTop - scan_radius;                                                  \
        min_y = (query_arg)->position.y - NuTimeBarTop - (query_arg)->object_scale * scan_radius;                      \
        min_z = (query_arg)->position.z - NuTimeBarTop - scan_radius;                                                  \
    } else {                                                                                                           \
        if ((query_arg)->movement.x > 0.0f) {                                                                          \
            min_x = (query_arg)->position.x - TerrainScanMargin02Pc - (query_arg)->collision_radius;                   \
            max_x = (query_arg)->position.x + (query_arg)->movement.x;                                                 \
        } else {                                                                                                       \
            min_x = (query_arg)->position.x + (query_arg)->movement.x - TerrainScanMargin02Pc -                        \
                    (query_arg)->collision_radius;                                                                     \
            max_x = (query_arg)->position.x;                                                                           \
        }                                                                                                              \
        max_x = max_x + TerrainScanMargin02Pc + (query_arg)->collision_radius;                                         \
        if ((query_arg)->movement.y > 0.0f) {                                                                          \
            min_y = (query_arg)->position.y - TerrainScanMargin02Pc - (query_arg)->collision_radius;                   \
            max_y = (query_arg)->movement.y + (query_arg)->position.y;                                                 \
        } else {                                                                                                       \
            min_y = (query_arg)->movement.y + (query_arg)->position.y - TerrainScanMargin02Pc -                        \
                    (query_arg)->collision_radius;                                                                     \
            max_y = (query_arg)->position.y;                                                                           \
        }                                                                                                              \
        max_y = max_y + TerrainScanMargin02Pc + (query_arg)->collision_radius;                                         \
        if ((query_arg)->movement.z > 0.0f) {                                                                          \
            min_z = (query_arg)->position.z - TerrainScanMargin02Pc - (query_arg)->collision_radius;                   \
            max_z = (query_arg)->movement.z + (query_arg)->position.z;                                                 \
        } else {                                                                                                       \
            min_z = (query_arg)->movement.z + (query_arg)->position.z - TerrainScanMargin02Pc -                        \
                    (query_arg)->collision_radius;                                                                     \
            max_z = (query_arg)->position.z;                                                                           \
        }                                                                                                              \
        max_z = max_z + TerrainScanMargin02Pc + (query_arg)->collision_radius;                                         \
        f32 length_sq =                                                                                                \
            (min_x - max_x) * (min_x - max_x) + (min_y - max_y) * (min_y - max_y) + (min_z - max_z) * (min_z - max_z); \
        scan_radius = NuFsqrt(length_sq);                                                                              \
        (query_arg) = TERRAIN_QUERY_PC;                                                                                \
    }

extern "C" void ScanTerrain(i32 scan_type, i32 terrain_mask, i32 scan_flags) {
    if (WallSplinesOnly != 0) {
        ScanWallSplineTerrain(scan_type, terrain_mask, scan_flags);
        return;
    }

    TerrainScaledShapeCursorPc = ScaleTerrainT1;
    platinrange = 0;
    TERRAIN_QUERY_PC->scan_group_index = -1;

    u8 *group_header;
    TERRAIN_SHAPE **cursor;
    u8 *limit;
    i32 group_shape_count;
    i32 scaled_shape_count;
    group_header = TERRAIN_QUERY_PC->scan_list_storage;
    cursor = reinterpret_cast<TERRAIN_SHAPE **>(group_header + sizeof(TERRAIN_SHAPE *));
    limit = TERRAIN_QUERY_PC->scan_list_storage + sizeof(TERRAIN_QUERY_PC->scan_list_storage) - 12;
    group_shape_count = 0;
    scaled_shape_count = 0;

    TERRAIN_GROUP *terrain_groups = TERRAIN_SET_CURRENT_PC->groups;
    f32 min_x, min_y, min_z, max_x, max_y, max_z, scan_radius;

    TerrainQuery_s *query = TERRAIN_QUERY_PC;
    TERRAIN_SCAN_BOUNDS_PC(query);
    if (1.0f < query->object_scale) {
        scan_radius += (query->object_scale - 1.0) * query->collision_radius;
    }

    for (i32 cell_index = 0; cell_index < TERRAIN_SET_CURRENT_PC->used_cell_count; ++cell_index) {
        const TERRAIN_CELL &cell = TERRAIN_SET_CURRENT_PC->cells[cell_index];
        if (!(max_x >= cell.min_x && max_z >= cell.min_z && cell.max_x >= min_x && cell.max_z >= min_z)) {
            continue;
        }

        const i16 *group_indices = TERRAIN_SET_CURRENT_PC->group_indices + cell.first_group;
        for (i32 cell_group = 0; cell_group < cell.group_count; ++cell_group) {
            const i32 group_index = group_indices[cell_group];
            TERRAIN_GROUP &group = terrain_groups[group_index];
            if (!(max_x >= group.bounds_min.x && max_y >= group.bounds_min.y && max_z >= group.bounds_min.z &&
                  group.bounds_max.x >= min_x && group.bounds_max.y > min_y && group.bounds_max.z > min_z) ||
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
                query = TERRAIN_QUERY_PC;
            }

            TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group.data);
            while (batch->marker >= 0) {
                TERRAIN_SHAPE *shapes = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                const i32 shape_count = batch->shape_count;
                if (group_max_x >= batch->min_x && batch->max_x > group_min_x && group_max_z >= batch->min_z &&
                    batch->max_z > group_min_z) {
                    TERRAIN_SHAPE *candidate = shapes;
                    for (i32 remaining = shape_count; remaining > 0; --remaining, ++candidate) {
                        if (!(group_max_x >= candidate->min_x && candidate->max_x > group_min_x &&
                              group_max_z >= candidate->min_z && candidate->max_z > group_min_z &&
                              group_max_y >= candidate->min_y && candidate->max_y > group_min_y) ||
                            reinterpret_cast<u8 *>(cursor) >= limit) {
                            continue;
                        }
                        if (candidate->material[1] != 0 && (candidate->material[1] & terrain_mask) == 0) {
                            continue;
                        }
                        if ((candidate->flags & scan_flags) == 0x40) {
                            continue;
                        }

                        TERRAIN_SHAPE *shape = candidate;
                        TERRAIN_SCALE_SCAN_SHAPE(shape, group);
                        *cursor++ = shape;
                        ++group_shape_count;
                    }
                }
                batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shapes + shape_count);
            }
            TERRAIN_FINISH_SCAN_GROUP_PC(group_index);
        }
    }

    if (curPickInst != 0) {
        for (i32 pick_index = 0; pick_index < curPickInst; ++pick_index) {
            const i32 group_index = TERRAIN_SET_CURRENT_PC->max_groups + pick_index;
            TERRAIN_GROUP &group = terrain_groups[group_index];
            const f32 local_max_x = max_x - group.origin.x;
            const f32 local_max_y = max_y - group.origin.y;
            const f32 local_max_z = max_z - group.origin.z;
            const f32 local_min_x = min_x - group.origin.x;
            const f32 local_min_y = min_y - group.origin.y;
            const f32 local_min_z = min_z - group.origin.z;
            if (!(local_max_x >= group.bounds_min.x && local_max_y >= group.bounds_min.y &&
                  local_max_z >= group.bounds_min.z && group.bounds_max.x >= local_min_x &&
                  group.bounds_max.y > local_min_y && group.bounds_max.z > local_min_z) ||
                group.chunk_type == -1) {
                continue;
            }

            TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group.data);
            while (batch->marker >= 0) {
                TERRAIN_SHAPE *shapes = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                const i32 shape_count = batch->shape_count;
                if (local_max_x >= batch->min_x && batch->max_x > local_min_x && local_max_z >= batch->min_z &&
                    batch->max_z > local_min_z) {
                    TERRAIN_SHAPE *candidate = shapes;
                    for (i32 remaining = shape_count; remaining > 0; --remaining, ++candidate) {
                        if (!(local_max_x >= candidate->min_x && candidate->max_x > local_min_x &&
                              local_max_z >= candidate->min_z && candidate->max_z > local_min_z &&
                              local_max_y >= candidate->min_y && candidate->max_y > local_min_y) ||
                            reinterpret_cast<u8 *>(cursor) >= limit) {
                            continue;
                        }
                        if (candidate->material[1] != 0 && (candidate->material[1] & terrain_mask) == 0) {
                            continue;
                        }
                        if ((candidate->flags & scan_flags) == 0x40) {
                            continue;
                        }

                        TERRAIN_SHAPE *shape = candidate;
                        TERRAIN_SCALE_SCAN_SHAPE(shape, group);
                        *cursor++ = shape;
                        ++group_shape_count;
                    }
                }
                batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shapes + shape_count);
            }
            TERRAIN_FINISH_SCAN_GROUP_PC(group_index);
        }
    }
    TERRAIN_QUERY_PC->scan_list = group_header;
    min_x -= NuTimeBarTop;
    max_x += NuTimeBarTop;
    min_y -= NuTimeBarTop;
    max_y += NuTimeBarTop;
    min_z -= NuTimeBarTop;
    max_z += NuTimeBarTop;
    if (scan_type != 0) {
        const i16 *groups;
        i32 count;
        if (min_x > TERRAIN_SET_CURRENT_PC->platform_query_min_x &&
            TERRAIN_SET_CURRENT_PC->platform_query_max_x > max_x &&
            min_z > TERRAIN_SET_CURRENT_PC->platform_query_min_z &&
            TERRAIN_SET_CURRENT_PC->platform_query_max_z > max_z) {
            groups = TERRAIN_SET_CURRENT_PC->active_platform_groups;
            count = TERRAIN_SET_CURRENT_PC->active_platform_count;
        } else {
            const TERRAIN_CELL &cell = TERRAIN_SET_CURRENT_PC->platform_cell;
            groups = TERRAIN_SET_CURRENT_PC->group_indices + cell.first_group;
            count = cell.group_count;
        }
        for (i32 i = 0; i < count; ++i) {
            const i32 platform_group_index = groups[i];
            TERRAIN_GROUP &group = terrain_groups[platform_group_index];
            if (group.origin.x - group.radius > max_x || group.origin.x + group.radius < min_x ||
                group.origin.z - group.radius > max_z || group.origin.z + group.radius < min_z) {
                continue;
            }
            TERRAIN_PLATFORM &platform = TERRAIN_SET_CURRENT_PC->platforms[group.scene_index];
            if (platform.scene_transform != NULL) {
                const u32 visible_mask = (platform.flags & TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED) != 0 ? 2 : 1;
                if ((static_cast<TERRAIN_SCENE_STATE_VIEW_PC *>(platform.scene_transform)->flags & visible_mask) == 0) {
                    continue;
                }
            }
            f32 local_min_x, local_min_y, local_min_z, local_max_x, local_max_y, local_max_z;
            if (platform.scene_object != NULL) {
                if (TERRAIN_PLATFORM_MATRIX_PC(group)->m30 > TERRAIN_PLATFORM_OF_PC(group).previous_matrix.m30) {
                    local_min_x = min_x - group.origin.x;
                    local_max_x =
                        (TERRAIN_PLATFORM_MATRIX_PC(group)->m30 - TERRAIN_PLATFORM_OF_PC(group).previous_matrix.m30) +
                        max_x - group.origin.x;
                } else {
                    local_max_x = max_x - group.origin.x;
                    local_min_x =
                        (TERRAIN_PLATFORM_MATRIX_PC(group)->m30 - TERRAIN_PLATFORM_OF_PC(group).previous_matrix.m30) +
                        min_x - group.origin.x;
                }
                if (TERRAIN_PLATFORM_MATRIX_PC(group)->m31 > TERRAIN_PLATFORM_OF_PC(group).previous_matrix.m31) {
                    local_min_y = min_y - group.origin.y;
                    local_max_y =
                        (TERRAIN_PLATFORM_MATRIX_PC(group)->m31 - TERRAIN_PLATFORM_OF_PC(group).previous_matrix.m31) +
                        max_y - group.origin.y;
                } else {
                    local_max_y = max_y - group.origin.y;
                    local_min_y =
                        (TERRAIN_PLATFORM_MATRIX_PC(group)->m31 - TERRAIN_PLATFORM_OF_PC(group).previous_matrix.m31) +
                        min_y - group.origin.y;
                }
                if (TERRAIN_PLATFORM_MATRIX_PC(group)->m32 > TERRAIN_PLATFORM_OF_PC(group).previous_matrix.m32) {
                    local_min_z = min_z - group.origin.z;
                    local_max_z =
                        (TERRAIN_PLATFORM_MATRIX_PC(group)->m32 - TERRAIN_PLATFORM_OF_PC(group).previous_matrix.m32) +
                        max_z - group.origin.z;
                } else {
                    local_max_z = max_z - group.origin.z;
                    local_min_z =
                        (TERRAIN_PLATFORM_MATRIX_PC(group)->m32 - TERRAIN_PLATFORM_OF_PC(group).previous_matrix.m32) +
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
                const f32 y = (min_y + max_y) * NuQFntHalfScale - group.origin.y;
                const f32 x = (max_x + min_x) * NuQFntHalfScale - group.origin.x;
                const f32 z = (max_z + min_z) * NuQFntHalfScale - group.origin.z;
                const f32 reach = group.radius + scan_radius;
                const f32 distance_sq = x * x + y * y + z * z;
                if (!(reach * reach > distance_sq)) {
                    continue;
                }
                TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group.data);
                while (batch->marker >= 0) {
                    TERRAIN_SHAPE *shapes = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                    const i32 shape_count = batch->shape_count;
                    TERRAIN_SHAPE *shape = shapes;
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
                        NuVec4MtxTransformVU0x3(vertices, vertices, TERRAIN_PLATFORM_MATRIX_PC(group));
                        if (shape->normals[1].y < 65535.0f) {
                            vertices[3].x = shape->vertices[3].x;
                            vertices[3].y = shape->vertices[3].y;
                            vertices[3].z = shape->vertices[3].z;
                            vertices[3].w = 0.0f;
                            NuVec4MtxTransformVU0(&vertices[3], &vertices[3], TERRAIN_PLATFORM_MATRIX_PC(group));
                        } else {
                            vertices[3] = vertices[2];
                        }
                        if (!(vertices[0].x > local_min_x || vertices[1].x > local_min_x ||
                              vertices[2].x > local_min_x || vertices[3].x > local_min_x)) {
                            query = TERRAIN_QUERY_PC;
                            continue;
                        }
                        if (!(vertices[0].x < local_max_x || vertices[1].x < local_max_x ||
                              vertices[2].x < local_max_x || vertices[3].x < local_max_x)) {
                            query = TERRAIN_QUERY_PC;
                            continue;
                        }
                        if (!(vertices[0].z > local_min_z || vertices[1].z > local_min_z ||
                              vertices[2].z > local_min_z || vertices[3].z > local_min_z)) {
                            query = TERRAIN_QUERY_PC;
                            continue;
                        }
                        if (!(vertices[0].z < local_max_z || vertices[1].z < local_max_z ||
                              vertices[2].z < local_max_z || vertices[3].z < local_max_z)) {
                            query = TERRAIN_QUERY_PC;
                            continue;
                        }
                        TERRAIN_SHAPE *transformed = &TerrainScaledShapeCursorPc[scaled_shape_count];
                        transformed->material[0] = shape->material[0];
                        transformed->material[1] = shape->material[1];
                        transformed->flags = shape->flags;
                        transformed->normal_flags = shape->normal_flags;
                        query = TERRAIN_QUERY_PC;
                        platinrange = 1;
                        transformed->vertices[0].x = vertices[0].x;
                        transformed->vertices[0].z = vertices[0].z;
                        transformed->vertices[0].y =
                            (vertices[0].y + group.origin.y) * query->inverse_object_scale - group.origin.y;
                        transformed->vertices[1].x = vertices[1].x;
                        transformed->vertices[1].z = vertices[1].z;
                        transformed->vertices[1].y =
                            (vertices[1].y + group.origin.y) * query->inverse_object_scale - group.origin.y;
                        transformed->vertices[2].x = vertices[2].x;
                        transformed->vertices[2].z = vertices[2].z;
                        transformed->vertices[2].y =
                            (vertices[2].y + group.origin.y) * query->inverse_object_scale - group.origin.y;
                        if (shape->normals[1].y < 65535.0f) {
                            transformed->vertices[3].x = vertices[3].x;
                            transformed->vertices[3].z = vertices[3].z;
                            transformed->vertices[3].y =
                                (vertices[3].y + group.origin.y) * query->inverse_object_scale - group.origin.y;
                            NUVEC a, b;
                            a.x = transformed->vertices[1].x - transformed->vertices[3].x;
                            a.y = transformed->vertices[1].y - transformed->vertices[3].y;
                            a.z = transformed->vertices[1].z - transformed->vertices[3].z;
                            b.x = transformed->vertices[2].x - transformed->vertices[3].x;
                            b.y = transformed->vertices[2].y - transformed->vertices[3].y;
                            b.z = transformed->vertices[2].z - transformed->vertices[3].z;
                            TERRAIN_CROSS_PRODUCT_PC(transformed->normals[1], a, b);
                            const f32 inverse = TerrainDivideNonzeroPC(
                                1.0f, NuFsqrt(transformed->normals[1].x * transformed->normals[1].x +
                                              transformed->normals[1].y * transformed->normals[1].y +
                                              transformed->normals[1].z * transformed->normals[1].z));
                            transformed = &TerrainScaledShapeCursorPc[scaled_shape_count];
                            transformed->normals[1].x *= inverse;
                            transformed->normals[1].y *= inverse;
                            transformed->normals[1].z *= inverse;
                        } else {
                            transformed->normals[1].y = 65536.0f;
                        }
                        {
                            NUVEC a, b;
                            a.x = transformed->vertices[2].x - transformed->vertices[0].x;
                            a.y = transformed->vertices[2].y - transformed->vertices[0].y;
                            a.z = transformed->vertices[2].z - transformed->vertices[0].z;
                            b.x = transformed->vertices[1].x - transformed->vertices[0].x;
                            b.y = transformed->vertices[1].y - transformed->vertices[0].y;
                            b.z = transformed->vertices[1].z - transformed->vertices[0].z;
                            TERRAIN_CROSS_PRODUCT_PC(transformed->normals[0], a, b);
                            const f32 length = NuFsqrt(transformed->normals[0].x * transformed->normals[0].x +
                                                       transformed->normals[0].y * transformed->normals[0].y +
                                                       transformed->normals[0].z * transformed->normals[0].z);
                            const f32 inverse = length == 0.0f ? 0.0f : 1.0f / length;
                            transformed = &TerrainScaledShapeCursorPc[scaled_shape_count];
                            transformed->normals[0].x *= inverse;
                            transformed->normals[0].y *= inverse;
                            transformed->normals[0].z *= inverse;
                        }
                        *cursor++ = transformed;
                        ++group_shape_count;
                        ++scaled_shape_count;
                        query = TERRAIN_QUERY_PC;
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shapes + shape_count);
                }
            } else {
                if (!(local_max_x >= group.bounds_min.x && local_max_y >= group.bounds_min.y &&
                      local_max_z >= group.bounds_min.z && group.bounds_max.x >= local_min_x &&
                      group.bounds_max.y > local_min_y && group.bounds_max.z > local_min_z) ||
                    group.chunk_type == -1) {
                    continue;
                }
                TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group.data);
                while (batch->marker >= 0) {
                    TERRAIN_SHAPE *shapes = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                    const i32 shape_count = batch->shape_count;
                    if (local_max_x >= batch->min_x && batch->max_x > local_min_x && local_max_z >= batch->min_z &&
                        batch->max_z > local_min_z) {
                        TERRAIN_SHAPE *candidate = shapes;
                        for (i32 remaining = shape_count; remaining > 0; --remaining, ++candidate) {
                            if (!(local_max_x >= candidate->min_x && candidate->max_x > local_min_x &&
                                  local_max_z >= candidate->min_z && candidate->max_z > local_min_z &&
                                  local_max_y >= candidate->min_y && candidate->max_y > local_min_y) ||
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
                            TERRAIN_SHAPE *shape = candidate;
                            TERRAIN_SCALE_SCAN_SHAPE(shape, group);
                            *cursor++ = shape;
                            ++group_shape_count;
                        }
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shapes + shape_count);
                }
            }
            TERRAIN_FINISH_SCAN_GROUP_PC(platform_group_index);
        }
    }

    i16 *terminator = reinterpret_cast<i16 *>(group_header);
    terminator[0] = 0;
    terminator[1] = 0;
    WallSplCount = 0;
    min_x -= TerrainScanMargin02Pc;
    min_z -= TerrainScanMargin02Pc;
    max_x += TerrainScanMargin02Pc;
    max_z += TerrainScanMargin02Pc;
    if (IgnoreWallSplines != 0) {
        IgnoreWallSplines = 0;
    } else {
        i32 wall_count = 0;
        for (TERRAIN_SPATIAL_NODE *node = TERRAIN_SET_CURRENT_PC->spatial_nodes; node != NULL;
             node = reinterpret_cast<TERRAIN_SPATIAL_CHUNK *>(reinterpret_cast<u8 *>(node) -
                                                              offsetof(TERRAIN_SPATIAL_CHUNK, node))
                        ->previous) {
            const u8 mask = node->field_02 >> 8;
            if (mask != 0 && (mask & terrain_mask) == 0) {
                continue;
            }
            const u8 material_low = static_cast<u8>(node->field_02);
            for (i32 first = 0; first < node->point_count; first += 16) {
                NUVEC *points = node->points + first;
                i32 end = node->point_count;
                if (points[0].y != TerrainUnusedSpatialBlockPc) {
                    if (points[1].y >= min_x && max_x >= points[0].y && points[3].y >= min_z && max_z >= points[2].y) {
                        end = MIN(end, first + 16);
                    } else {
                        end = 0;
                    }
                }
                TERRAIN_WALL_POINT *entry = static_cast<TERRAIN_WALL_POINT *>(WallSplList) + wall_count;
                for (i32 i = first; i < end; ++i) {
                    NUVEC &a = node->points[i];
                    NUVEC &b = node->points[i + 1];
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
    WallSplinesOnly = 0;
    TerrainScaledShapeCursorPc = ((TERRAIN_SHAPE *)(ScaleTerrainT1));
    TerrainQuery_s *query = TERRAIN_QUERY_PC;
    i16 *terminator = reinterpret_cast<i16 *>(query->scan_list_storage);
    platinrange = 0;
    query->scan_group_index = -1;
    f32 min_x, min_y, min_z, max_x, max_y, max_z, scan_radius;
    TERRAIN_SCAN_BOUNDS_PC(query);
    terminator[0] = 0;
    terminator[1] = 0;
    min_x -= TerrainScanMargin02Pc;
    min_z -= TerrainScanMargin02Pc;
    WallSplCount = 0;
    i32 wall_count = 0;
    max_x += TerrainScanMargin02Pc;
    max_z += TerrainScanMargin02Pc;
    for (TERRAIN_SPATIAL_NODE *node = TERRAIN_SET_CURRENT_PC->spatial_nodes; node != NULL;
         node = reinterpret_cast<TERRAIN_SPATIAL_CHUNK *>(reinterpret_cast<u8 *>(node) -
                                                          offsetof(TERRAIN_SPATIAL_CHUNK, node))
                    ->previous) {
        const u8 mask = node->field_02 >> 8;
        if (mask != 0 && (mask & terrain_mask) == 0) {
            continue;
        }
        const u32 material = node->field_02 & 0xff;
        if (node->point_count != 0) {
            for (i32 first = 0; first < node->point_count; first += 16) {
                i32 end;
                if (node->points[first].y != 2147483648.0f) {
                    if (node->points[first + 1].y >= min_x && node->points[first].y <= max_x &&
                        node->points[first + 3].y >= min_z && node->points[first + 2].y <= max_z) {
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
                for (i32 i = first; i < end; ++i) {
                    NUVEC *a = &node->points[i];
                    NUVEC *b = &node->points[i + 1];
                    if ((a->x >= min_x && b->x <= max_x) || (b->x >= min_x && a->x <= max_x)) {
                        if ((a->z >= min_z && b->z <= max_z) || (b->z >= min_z && a->z <= max_z)) {
                            if (wall_count < 64) {
                                TERRAIN_WALL_POINT *entry = &static_cast<TERRAIN_WALL_POINT *>(WallSplList)[wall_count];
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
                            }
                        }
                    }
                }
            }
            WallSplCount = wall_count;
        }
    }
    TerrOverRideScan = NULL;
}
