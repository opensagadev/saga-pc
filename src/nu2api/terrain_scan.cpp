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
typedef struct TerrainScanWriter TerrainScanWriter;
struct TerrainScanBounds {
    f32 min_x;
    f32 min_y;
    f32 min_z;
    f32 max_x;
    f32 max_y;
    f32 max_z;
};
struct TerrainScanWriter {
    u8 *group_header;
    TERRAIN_SHAPE **cursor;
    u8 *limit;
    i32 group_shape_count;
    i32 scaled_shape_count;
};
extern "C" void ScanTerrain(i32 scan_type, i32 terrain_mask, i32 scan_flags) {
    if (WallSplinesOnly != 0) {
        ScanWallSplineTerrain(scan_type, terrain_mask, scan_flags);
        return;
    }

    TerrainScaledShapeCursorPc = ((TERRAIN_SHAPE *)(ScaleTerrainT1));
    platinrange = 0;
    ((TerrainQuery_s *)TerI)->scan_group_index = -1;

    TerrainScanWriter writer;
    writer.group_header = ((TerrainQuery_s *)TerI)->scan_list_storage;
    writer.cursor = ((TERRAIN_SHAPE **)(writer.group_header + sizeof(TERRAIN_SHAPE *)));
    writer.limit =
        ((TerrainQuery_s *)TerI)->scan_list_storage + sizeof(((TerrainQuery_s *)TerI)->scan_list_storage) - 12;
    writer.group_shape_count = 0;
    writer.scaled_shape_count = 0;

    TerrainScanBounds bounds;
    f32 scan_radius;
    do {
        TerrainScanBounds &scan_bounds = ((bounds));
        f32 &bounds_radius = ((scan_radius));
        const TerrainQuery_s &scan_query = *((TerrainQuery_s *)TerI);
        if (scan_query.scan_result != 1) {
            const f32 move_length =
                NuFsqrt(scan_query.movement.x * scan_query.movement.x + scan_query.movement.y * scan_query.movement.y +
                        scan_query.movement.z * scan_query.movement.z);
            const f32 reach = scan_query.collision_radius + TerrainScanMargin10Pc + move_length;
            const f32 vertical = reach * scan_query.object_scale;
            scan_bounds.min_x = scan_query.position.x - NuTimeBarTop - reach;
            scan_bounds.max_x = scan_query.position.x + NuTimeBarTop + reach;
            scan_bounds.min_y = scan_query.position.y - NuTimeBarTop - vertical;
            scan_bounds.max_y = scan_query.position.y + NuTimeBarTop + vertical;
            scan_bounds.min_z = scan_query.position.z - NuTimeBarTop - reach;
            scan_bounds.max_z = scan_query.position.z + NuTimeBarTop + reach;
            bounds_radius = reach;
        } else {
            const f32 radius = scan_query.collision_radius;
            if (!(scan_query.movement.x <= 0.0f)) {
                scan_bounds.min_x = scan_query.position.x - TerrainScanMargin02Pc - radius;
                scan_bounds.max_x = scan_query.position.x + scan_query.movement.x + TerrainScanMargin02Pc + radius;
            } else {
                scan_bounds.min_x = scan_query.position.x + scan_query.movement.x - TerrainScanMargin02Pc - radius;
                scan_bounds.max_x = scan_query.position.x + TerrainScanMargin02Pc + radius;
            }
            if (scan_query.movement.y <= 0.0f) {
                scan_bounds.min_y = scan_query.position.y + scan_query.movement.y - TerrainScanMargin02Pc - radius;
                scan_bounds.max_y = scan_query.position.y + TerrainScanMargin02Pc + radius;
            } else {
                scan_bounds.min_y = scan_query.position.y - TerrainScanMargin02Pc - radius;
                scan_bounds.max_y = scan_query.position.y + scan_query.movement.y + TerrainScanMargin02Pc + radius;
            }
            if (scan_query.movement.z <= 0.0f) {
                scan_bounds.min_z = scan_query.position.z + scan_query.movement.z - TerrainScanMargin02Pc - radius;
                scan_bounds.max_z = scan_query.position.z + TerrainScanMargin02Pc + radius;
            } else {
                scan_bounds.min_z = scan_query.position.z - TerrainScanMargin02Pc - radius;
                scan_bounds.max_z = scan_query.position.z + scan_query.movement.z + TerrainScanMargin02Pc + radius;
            }
            const f32 dx = scan_bounds.min_x - scan_bounds.max_x;
            const f32 dy = scan_bounds.min_y - scan_bounds.max_y;
            const f32 dz = scan_bounds.min_z - scan_bounds.max_z;
            bounds_radius = NuFsqrt(dx * dx + dy * dy + dz * dz);
        }
        if (!((false)) && scan_query.object_scale > 1.0f)
            bounds_radius += scan_query.collision_radius * ((scan_query.object_scale - 1.0f));
    } while (0);

    for (i32 cell_index = 0; cell_index < ((TERRAIN_SET_PC *)CurrentTerrain)->used_cell_count; ++cell_index) {
        const TERRAIN_CELL &cell = ((TERRAIN_SET_PC *)CurrentTerrain)->cells[cell_index];
        if (bounds.max_x < cell.min_x || bounds.max_z < cell.min_z || cell.max_x < bounds.min_x ||
            cell.max_z < bounds.min_z) {
            continue;
        }

        const i16 *group_indices = ((TERRAIN_SET_PC *)CurrentTerrain)->group_indices + cell.first_group;
        for (i32 cell_group = 0; cell_group < cell.group_count; ++cell_group) {
            const i32 group_index = group_indices[cell_group];
            TERRAIN_GROUP &group = ((TERRAIN_SET_PC *)CurrentTerrain)->groups[group_index];
            if (!(((bounds)).max_x >= ((group.bounds_min)).x && ((bounds)).max_y >= ((group.bounds_min)).y &&
                  ((bounds)).max_z >= ((group.bounds_min)).z && ((group.bounds_max)).x >= ((bounds)).min_x &&
                  ((group.bounds_max)).y > ((bounds)).min_y && ((group.bounds_max)).z > ((bounds)).min_z) ||
                group.chunk_type == -1) {
                continue;
            }

            if (((i16)(group.scene_index)) < 0) {
                TerrainSkinAllocate(((TERRAIN_GROUP_PC *)(&group)));
            }

            TerrainScanBounds group_bounds = bounds;
            group_bounds.min_x -= group.origin.x;
            group_bounds.max_x -= group.origin.x;
            group_bounds.min_y -= group.origin.y;
            group_bounds.max_y -= group.origin.y;
            group_bounds.min_z -= group.origin.z;
            group_bounds.max_z -= group.origin.z;

            TERRAIN_SHAPE_BATCH *batch = ((TERRAIN_SHAPE_BATCH *)(group.data));
            while (batch->marker >= 0) {
                TERRAIN_SHAPE *shapes = ((TERRAIN_SHAPE *)(batch + 1));
                if (group_bounds.max_x >= batch->min_x && batch->max_x > group_bounds.min_x &&
                    group_bounds.max_z >= batch->min_z && batch->max_z > group_bounds.min_z) {
                    for (i32 shape_index = 0; shape_index < batch->shape_count; ++shape_index) {
                        TERRAIN_SHAPE *candidate = &shapes[shape_index];
                        if (!(((group_bounds)).max_x >= ((*candidate)).min_x &&
                              ((*candidate)).max_x > ((group_bounds)).min_x &&
                              ((group_bounds)).max_y >= ((*candidate)).min_y &&
                              ((*candidate)).max_y > ((group_bounds)).min_y &&
                              ((group_bounds)).max_z >= ((*candidate)).min_z &&
                              ((*candidate)).max_z > ((group_bounds)).min_z) ||
                            ((u8 *)(writer.cursor)) >= writer.limit) {
                            continue;
                        }
                        if (candidate->material[1] != 0 && (candidate->material[1] & terrain_mask) == 0) {
                            continue;
                        }
                        if ((candidate->flags & scan_flags) == 0x40) {
                            continue;
                        }

                        do {
                            TERRAIN_SHAPE *&scale_candidate = ((candidate));
                            const TERRAIN_GROUP &scale_group = ((group));
                            TerrainScanWriter *scale_writer = ((&writer));
                            if (((TerrainQuery_s *)TerI)->object_scale != 1.0f) {
                                TERRAIN_SHAPE *source = scale_candidate;
                                scale_candidate =
                                    &((TERRAIN_SHAPE *)TerrainScaledShapeCursorPc)[scale_writer->scaled_shape_count++];
                                scale_candidate->material[0] = source->material[0];
                                scale_candidate->material[1] = source->material[1];
                                scale_candidate->flags = source->flags;
                                scale_candidate->normal_flags = source->normal_flags;

                                for (i32 vector_index = 0; vector_index < 3; ++vector_index) {
                                    scale_candidate->vertices[vector_index].x = source->vertices[vector_index].x;
                                    scale_candidate->vertices[vector_index].y =
                                        (source->vertices[vector_index].y + scale_group.origin.y) *
                                            ((TerrainQuery_s *)TerI)->inverse_object_scale -
                                        scale_group.origin.y;
                                    scale_candidate->vertices[vector_index].z = source->vertices[vector_index].z;
                                }

                                if (source->normals[1].y < 65535.0f) {
                                    scale_candidate->vertices[3].x = source->vertices[3].x;
                                    scale_candidate->vertices[3].y =
                                        (source->vertices[3].y + scale_group.origin.y) *
                                            ((TerrainQuery_s *)TerI)->inverse_object_scale -
                                        scale_group.origin.y;
                                    scale_candidate->vertices[3].z = source->vertices[3].z;

                                    const f32 length = NuFsqrt(source->normals[1].x * source->normals[1].x +
                                                               source->normals[1].y * source->normals[1].y *
                                                                   ((TerrainQuery_s *)TerI)->object_scale_sq +
                                                               source->normals[1].z * source->normals[1].z);
                                    const f32 inverse_length = TerrainDivideNonzeroPC(1.0f, length);
                                    scale_candidate->normals[1].x = source->normals[1].x * inverse_length;
                                    scale_candidate->normals[1].y =
                                        source->normals[1].y * ((TerrainQuery_s *)TerI)->object_scale * inverse_length;
                                    scale_candidate->normals[1].z = source->normals[1].z * inverse_length;
                                } else {
                                    scale_candidate->normals[1].y = 65536.0f;
                                }

                                const f32 length = NuFsqrt(source->normals[0].x * source->normals[0].x +
                                                           source->normals[0].y * source->normals[0].y *
                                                               ((TerrainQuery_s *)TerI)->object_scale_sq +
                                                           source->normals[0].z * source->normals[0].z);
                                const f32 inverse_length = TerrainDivideNonzeroPC(1.0f, length);
                                scale_candidate->normals[0].x = source->normals[0].x * inverse_length;
                                scale_candidate->normals[0].y =
                                    source->normals[0].y * ((TerrainQuery_s *)TerI)->object_scale * inverse_length;
                                scale_candidate->normals[0].z = source->normals[0].z * inverse_length;
                            }
                        } while (0);
                        *writer.cursor++ = candidate;
                        ++writer.group_shape_count;
                    }
                }
                batch = ((TERRAIN_SHAPE_BATCH *)(shapes + batch->shape_count));
            }
            {
                if ((&writer)->group_shape_count != 0) {

                    i16 *header = ((i16 *)((&writer)->group_header));
                    header[0] = ((i16)((&writer)->group_shape_count));
                    header[1] = ((i16)((group_index)));
                    (&writer)->group_header = ((u8 *)((&writer)->cursor));

                    (&writer)->cursor = ((TERRAIN_SHAPE **)((&writer)->group_header + sizeof(TERRAIN_SHAPE *)));
                    (&writer)->group_shape_count = 0;
                }
            };
        }
    }

    for (i32 pick_index = 0; pick_index < curPickInst; ++pick_index) {
        const i32 group_index = ((TERRAIN_SET_PC *)CurrentTerrain)->max_groups + pick_index;
        TERRAIN_GROUP &group = ((TERRAIN_SET_PC *)CurrentTerrain)->groups[group_index];
        TerrainScanBounds local_bounds = bounds;
        local_bounds.min_x -= group.origin.x;
        local_bounds.max_x -= group.origin.x;
        local_bounds.min_y -= group.origin.y;
        local_bounds.max_y -= group.origin.y;
        local_bounds.min_z -= group.origin.z;
        local_bounds.max_z -= group.origin.z;
        if (!(((local_bounds)).max_x >= ((group.bounds_min)).x && ((local_bounds)).max_y >= ((group.bounds_min)).y &&
              ((local_bounds)).max_z >= ((group.bounds_min)).z && ((group.bounds_max)).x >= ((local_bounds)).min_x &&
              ((group.bounds_max)).y > ((local_bounds)).min_y && ((group.bounds_max)).z > ((local_bounds)).min_z) ||
            group.chunk_type == -1) {
            continue;
        }

        TERRAIN_SHAPE_BATCH *batch = ((TERRAIN_SHAPE_BATCH *)(group.data));
        while (batch->marker >= 0) {
            TERRAIN_SHAPE *shapes = ((TERRAIN_SHAPE *)(batch + 1));
            if (local_bounds.max_x >= batch->min_x && batch->max_x > local_bounds.min_x &&
                local_bounds.max_z >= batch->min_z && batch->max_z > local_bounds.min_z) {
                for (i32 shape_index = 0; shape_index < batch->shape_count; ++shape_index) {
                    TERRAIN_SHAPE *candidate = &shapes[shape_index];
                    if (!(((local_bounds)).max_x >= ((*candidate)).min_x &&
                          ((*candidate)).max_x > ((local_bounds)).min_x &&
                          ((local_bounds)).max_y >= ((*candidate)).min_y &&
                          ((*candidate)).max_y > ((local_bounds)).min_y &&
                          ((local_bounds)).max_z >= ((*candidate)).min_z &&
                          ((*candidate)).max_z > ((local_bounds)).min_z) ||
                        ((u8 *)(writer.cursor)) >= writer.limit) {
                        continue;
                    }
                    if (candidate->material[1] != 0 && (candidate->material[1] & terrain_mask) == 0) {
                        continue;
                    }
                    if ((candidate->flags & scan_flags) == 0x40) {
                        continue;
                    }

                    do {
                        TERRAIN_SHAPE *&scale_candidate = ((candidate));
                        const TERRAIN_GROUP &scale_group = ((group));
                        TerrainScanWriter *scale_writer = ((&writer));
                        if (((TerrainQuery_s *)TerI)->object_scale != 1.0f) {
                            TERRAIN_SHAPE *source = scale_candidate;
                            scale_candidate =
                                &((TERRAIN_SHAPE *)TerrainScaledShapeCursorPc)[scale_writer->scaled_shape_count++];
                            scale_candidate->material[0] = source->material[0];
                            scale_candidate->material[1] = source->material[1];
                            scale_candidate->flags = source->flags;
                            scale_candidate->normal_flags = source->normal_flags;

                            for (i32 vector_index = 0; vector_index < 3; ++vector_index) {
                                scale_candidate->vertices[vector_index].x = source->vertices[vector_index].x;
                                scale_candidate->vertices[vector_index].y =
                                    (source->vertices[vector_index].y + scale_group.origin.y) *
                                        ((TerrainQuery_s *)TerI)->inverse_object_scale -
                                    scale_group.origin.y;
                                scale_candidate->vertices[vector_index].z = source->vertices[vector_index].z;
                            }

                            if (!(source->normals[1].y < 65535.0f)) {
                                scale_candidate->normals[1].y = 65536.0f;
                            } else {
                                scale_candidate->vertices[3].x = source->vertices[3].x;
                                scale_candidate->vertices[3].y = (source->vertices[3].y + scale_group.origin.y) *
                                                                     ((TerrainQuery_s *)TerI)->inverse_object_scale -
                                                                 scale_group.origin.y;
                                scale_candidate->vertices[3].z = source->vertices[3].z;

                                const f32 length = NuFsqrt(source->normals[1].x * source->normals[1].x +
                                                           source->normals[1].y * source->normals[1].y *
                                                               ((TerrainQuery_s *)TerI)->object_scale_sq +
                                                           source->normals[1].z * source->normals[1].z);
                                const f32 inverse_length = TerrainDivideNonzeroPC(1.0f, length);
                                scale_candidate->normals[1].x = source->normals[1].x * inverse_length;
                                scale_candidate->normals[1].y =
                                    source->normals[1].y * ((TerrainQuery_s *)TerI)->object_scale * inverse_length;
                                scale_candidate->normals[1].z = source->normals[1].z * inverse_length;
                            }

                            const f32 length = NuFsqrt(source->normals[0].x * source->normals[0].x +
                                                       source->normals[0].y * source->normals[0].y *
                                                           ((TerrainQuery_s *)TerI)->object_scale_sq +
                                                       source->normals[0].z * source->normals[0].z);
                            const f32 inverse_length = TerrainDivideNonzeroPC(1.0f, length);
                            scale_candidate->normals[0].x = source->normals[0].x * inverse_length;
                            scale_candidate->normals[0].y =
                                source->normals[0].y * ((TerrainQuery_s *)TerI)->object_scale * inverse_length;
                            scale_candidate->normals[0].z = source->normals[0].z * inverse_length;
                        }
                    } while (0);
                    *writer.cursor++ = candidate;
                    ++writer.group_shape_count;
                }
            }
            batch = ((TERRAIN_SHAPE_BATCH *)(shapes + batch->shape_count));
        }
        {
            if ((&writer)->group_shape_count != 0) {

                i16 *header = ((i16 *)((&writer)->group_header));
                header[0] = ((i16)((&writer)->group_shape_count));
                header[1] = ((i16)((group_index)));
                (&writer)->group_header = ((u8 *)((&writer)->cursor));

                (&writer)->cursor = ((TERRAIN_SHAPE **)((&writer)->group_header + sizeof(TERRAIN_SHAPE *)));
                (&writer)->group_shape_count = 0;
            }
        };
    }

    ((TerrainQuery_s *)TerI)->scan_list = writer.group_header;
    if (scan_type != 0) {
        TerrainScanBounds platform_bounds = bounds;
        platform_bounds.min_x -= NuTimeBarTop;
        platform_bounds.min_y -= NuTimeBarTop;
        platform_bounds.min_z -= NuTimeBarTop;
        platform_bounds.max_x += NuTimeBarTop;
        platform_bounds.max_y += NuTimeBarTop;
        platform_bounds.max_z += NuTimeBarTop;
        const i16 *groups = ((TERRAIN_SET_PC *)CurrentTerrain)->active_platform_groups;
        i32 count = ((TERRAIN_SET_PC *)CurrentTerrain)->active_platform_count;
        if (platform_bounds.min_x <= ((TERRAIN_SET_PC *)CurrentTerrain)->platform_query_min_x ||
            platform_bounds.max_x >= ((TERRAIN_SET_PC *)CurrentTerrain)->platform_query_max_x ||
            platform_bounds.min_z <= ((TERRAIN_SET_PC *)CurrentTerrain)->platform_query_min_z ||
            platform_bounds.max_z >= ((TERRAIN_SET_PC *)CurrentTerrain)->platform_query_max_z) {
            const TERRAIN_CELL &cell = ((TERRAIN_SET_PC *)CurrentTerrain)->platform_cell;
            groups = ((TERRAIN_SET_PC *)CurrentTerrain)->group_indices + cell.first_group;
            count = cell.group_count;
        }
        for (i32 i = 0; i < count; ++i) {
            const TERRAIN_GROUP &group = ((TERRAIN_SET_PC *)CurrentTerrain)->groups[groups[i]];
            if (platform_bounds.max_x < group.origin.x - group.radius ||
                group.origin.x + group.radius < platform_bounds.min_x ||
                group.origin.z - group.radius > platform_bounds.max_z ||
                group.origin.z + group.radius < platform_bounds.min_z)
                continue;
            if (group.chunk_type == -1)
                continue;
            const TERRAIN_PLATFORM &platform = ((TERRAIN_SET_PC *)CurrentTerrain)->platforms[group.scene_index];
            if ((platform.flags & TERRAIN_PLATFORM_FLAG_ROTATING) != 0) {
                const f32 x = (platform_bounds.min_x + platform_bounds.max_x) * 0.5f - group.origin.x;
                const f32 y = (platform_bounds.min_y + platform_bounds.max_y) * 0.5f - group.origin.y;
                const f32 z = (platform_bounds.min_z + platform_bounds.max_z) * 0.5f - group.origin.z;
                const f32 reach = scan_radius + group.radius;
                if (!(x * x + y * y + z * z < reach * reach) || platform.scene_object == NULL)
                    continue;
            }
            {
                TERRAIN_GROUP &group = ((TERRAIN_SET_PC *)CurrentTerrain)->groups[(groups[i])];
                TERRAIN_PLATFORM &platform = ((TERRAIN_SET_PC *)CurrentTerrain)->platforms[group.scene_index];
                if (group.chunk_type == -1)
                    continue;
                if ((true) && platform.scene_transform != NULL) {
                    const u8 visible_mask = (platform.flags & TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED) != 0 ? 2 : 1;
                    if ((((TERRAIN_SCENE_STATE_VIEW_PC *)(platform.scene_transform))->low_flags & visible_mask) == 0)
                        continue;
                }
                TerrainScanBounds local = (platform_bounds);
                NUMTX *matrix = ((NUMTX *)(platform.scene_object));
                if (matrix != NULL) {
                    const f32 dx = (matrix->m30 - platform.previous_matrix.m30) * (1.0f);
                    const f32 dy = (matrix->m31 - platform.previous_matrix.m31) * (1.0f);
                    const f32 dz = (matrix->m32 - platform.previous_matrix.m32) * (1.0f);
                    if (dx > 0.0f)
                        local.max_x += dx;
                    else
                        local.min_x += dx;
                    if (dy > 0.0f)
                        local.max_y += dy;
                    else
                        local.min_y += dy;
                    if (dz > 0.0f)
                        local.max_z += dz;
                    else
                        local.min_z += dz;
                }
                local.min_x -= group.origin.x;
                local.max_x -= group.origin.x;
                local.min_y -= group.origin.y;
                local.max_y -= group.origin.y;
                local.min_z -= group.origin.z;
                local.max_z -= group.origin.z;
                const bool rotating = (platform.flags & TERRAIN_PLATFORM_FLAG_ROTATING) != 0;
                if (!rotating &&
                    !(((local)).max_x >= ((group.bounds_min)).x && ((local)).max_y >= ((group.bounds_min)).y &&
                      ((local)).max_z >= ((group.bounds_min)).z && ((group.bounds_max)).x >= ((local)).min_x &&
                      ((group.bounds_max)).y > ((local)).min_y && ((group.bounds_max)).z > ((local)).min_z))
                    continue;
                TERRAIN_SHAPE_BATCH *batch = ((TERRAIN_SHAPE_BATCH *)(group.data));
                while (batch->marker >= 0) {
                    TERRAIN_SHAPE *shapes = ((TERRAIN_SHAPE *)(batch + 1));
                    if (rotating || (local.max_x >= batch->min_x && local.min_x < batch->max_x &&
                                     local.max_z >= batch->min_z && batch->max_z > local.min_z)) {
                        for (i32 i = 0; i < batch->shape_count; ++i) {
                            TERRAIN_SHAPE *shape = &shapes[i];
                            if (!rotating &&
                                !(((local)).max_x >= ((*shape)).min_x && ((*shape)).max_x > ((local)).min_x &&
                                  ((local)).max_y >= ((*shape)).min_y && ((*shape)).max_y > ((local)).min_y &&
                                  ((local)).max_z >= ((*shape)).min_z && ((*shape)).max_z > ((local)).min_z))
                                continue;
                            if (((u8 *)((&writer)->cursor)) >= (&writer)->limit ||
                                (shape->material[1] != 0 && (shape->material[1] & (terrain_mask)) == 0) ||
                                (shape->flags & (scan_flags)) == 0x40)
                                continue;
                            if (rotating) {
                                NUVEC4 vertices[4];
                                const bool quad = shape->normals[1].y < 65535.0f;
                                for (i32 v = 0; v < 3; ++v) {
                                    vertices[v].x = shape->vertices[v].x;
                                    vertices[v].y = shape->vertices[v].y;
                                    vertices[v].z = shape->vertices[v].z;
                                    vertices[v].w = 0.0f;
                                }
                                NuVec4MtxTransformVU0x3(vertices, vertices, matrix);
                                if (quad) {
                                    vertices[3].x = shape->vertices[3].x;
                                    vertices[3].y = shape->vertices[3].y;
                                    vertices[3].z = shape->vertices[3].z;
                                    vertices[3].w = 0.0f;
                                    NuVec4MtxTransformVU0(&vertices[3], &vertices[3], matrix);
                                } else
                                    vertices[3] = vertices[2];
                                bool above_min_x = false, below_max_x = false;
                                bool above_min_z = false, below_max_z = false;
                                for (i32 v = 0; v < 4; ++v) {
                                    above_min_x |= vertices[v].x > local.min_x;
                                    below_max_x |= vertices[v].x < local.max_x;
                                    above_min_z |= vertices[v].z > local.min_z;
                                    below_max_z |= vertices[v].z < local.max_z;
                                }
                                if (!above_min_x || !below_max_x || !above_min_z || !below_max_z)
                                    continue;
                                platinrange = 1;
                                TERRAIN_SHAPE *transformed =
                                    &((TERRAIN_SHAPE *)TerrainScaledShapeCursorPc)[(&writer)->scaled_shape_count++];
                                transformed->material[0] = shape->material[0];
                                transformed->material[1] = shape->material[1];
                                transformed->flags = shape->flags;
                                transformed->normal_flags = shape->normal_flags;
                                for (i32 v = 0; v < (quad ? 4 : 3); ++v) {
                                    transformed->vertices[v].x = vertices[v].x;

                                    transformed->vertices[v].y =
                                        (false) ? vertices[v].y
                                                : (vertices[v].y + group.origin.y) *
                                                          ((TerrainQuery_s *)TerI)->inverse_object_scale -
                                                      group.origin.y;
                                    transformed->vertices[v].z = vertices[v].z;
                                }
                                if (!quad)
                                    transformed->normals[1].y = 65536.0f;
                                for (i32 n = quad ? 1 : 0; n >= 0; --n) {
                                    NUVEC a, b;
                                    NUVEC *v = transformed->vertices;
                                    a.x = v[n ? 1 : 2].x - v[n ? 3 : 0].x;
                                    a.y = v[n ? 1 : 2].y - v[n ? 3 : 0].y;
                                    a.z = v[n ? 1 : 2].z - v[n ? 3 : 0].z;
                                    b.x = v[n ? 2 : 1].x - v[n ? 3 : 0].x;
                                    b.y = v[n ? 2 : 1].y - v[n ? 3 : 0].y;
                                    b.z = v[n ? 2 : 1].z - v[n ? 3 : 0].z;
                                    transformed->normals[n].x = a.y * b.z - a.z * b.y;
                                    transformed->normals[n].y = a.z * b.x - a.x * b.z;
                                    transformed->normals[n].z = a.x * b.y - a.y * b.x;
                                    NUVEC &normal = transformed->normals[n];
                                    const f32 length =
                                        NuFsqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
                                    const f32 inverse = TerrainDivideNonzeroPC(1.0f, length);
                                    normal.x *= inverse;
                                    normal.y *= inverse;
                                    normal.z *= inverse;
                                }
                                shape = transformed;
                            } else {
                                platinrange = 1;
                                if (!(false)) {
                                    do {
                                        TERRAIN_SHAPE *&scale_candidate = ((shape));
                                        const TERRAIN_GROUP &scale_group = ((group));
                                        TerrainScanWriter *scale_writer = (((&writer)));
                                        if (1.0f != ((TerrainQuery_s *)TerI)->object_scale) {
                                            TERRAIN_SHAPE *source = scale_candidate;
                                            scale_candidate =
                                                &((TERRAIN_SHAPE *)
                                                      TerrainScaledShapeCursorPc)[scale_writer->scaled_shape_count++];
                                            scale_candidate->material[0] = *source->material;
                                            scale_candidate->material[1] = source->material[1];
                                            scale_candidate->flags = source->flags;
                                            scale_candidate->normal_flags = source->normal_flags;

                                            for (i32 vector_index = 0; vector_index < 3; ++vector_index) {
                                                scale_candidate->vertices[vector_index].x =
                                                    source->vertices[vector_index].x;
                                                scale_candidate->vertices[vector_index].y =
                                                    (source->vertices[vector_index].y + scale_group.origin.y) *
                                                        ((TerrainQuery_s *)TerI)->inverse_object_scale -
                                                    scale_group.origin.y;
                                                scale_candidate->vertices[vector_index].z =
                                                    source->vertices[vector_index].z;
                                            }

                                            if (source->normals[1].y < 65535.0f) {
                                                scale_candidate->vertices[3].x = source->vertices[3].x;
                                                scale_candidate->vertices[3].y =
                                                    (source->vertices[3].y + scale_group.origin.y) *
                                                        ((TerrainQuery_s *)TerI)->inverse_object_scale -
                                                    scale_group.origin.y;
                                                scale_candidate->vertices[3].z = source->vertices[3].z;

                                                const f32 length =
                                                    NuFsqrt(source->normals[1].x * source->normals[1].x +
                                                            source->normals[1].y * source->normals[1].y *
                                                                ((TerrainQuery_s *)TerI)->object_scale_sq +
                                                            source->normals[1].z * source->normals[1].z);
                                                const f32 inverse_length = TerrainDivideNonzeroPC(1.0f, length);
                                                scale_candidate->normals[1].x = source->normals[1].x * inverse_length;
                                                scale_candidate->normals[1].y = source->normals[1].y *
                                                                                ((TerrainQuery_s *)TerI)->object_scale *
                                                                                inverse_length;
                                                scale_candidate->normals[1].z = source->normals[1].z * inverse_length;
                                            } else {
                                                scale_candidate->normals[1].y = 65536.0f;
                                            }

                                            const f32 length = NuFsqrt(source->normals[0].x * source->normals[0].x +
                                                                       source->normals[0].y * source->normals[0].y *
                                                                           ((TerrainQuery_s *)TerI)->object_scale_sq +
                                                                       source->normals[0].z * source->normals[0].z);
                                            const f32 inverse_length = TerrainDivideNonzeroPC(1.0f, length);
                                            scale_candidate->normals[0].x = source->normals[0].x * inverse_length;
                                            scale_candidate->normals[0].y = source->normals[0].y *
                                                                            ((TerrainQuery_s *)TerI)->object_scale *
                                                                            inverse_length;
                                            scale_candidate->normals[0].z = source->normals[0].z * inverse_length;
                                        }
                                    } while (0);
                                }
                            }
                            *(&writer)->cursor++ = shape;
                            ++(&writer)->group_shape_count;
                        }
                    }
                    batch = ((TERRAIN_SHAPE_BATCH *)(shapes + batch->shape_count));
                }
                {
                    if (((&writer))->group_shape_count != 0) {

                        i16 *header = ((i16 *)(((&writer))->group_header));
                        header[0] = ((i16)(((&writer))->group_shape_count));
                        header[1] = ((i16)(((groups[i]))));
                        ((&writer))->group_header = ((u8 *)(((&writer))->cursor));

                        ((&writer))->cursor = ((TERRAIN_SHAPE **)(((&writer))->group_header + sizeof(TERRAIN_SHAPE *)));
                        ((&writer))->group_shape_count = 0;
                    }
                };
            };
        }
    }

    i16 *terminator = ((i16 *)(writer.group_header));
    terminator[0] = 0;
    terminator[1] = 0;

    WallSplCount = 0;
    if (IgnoreWallSplines != 0) {
        IgnoreWallSplines = 0;
    } else {
        do {
            TerrainScanBounds wall_bounds = ((bounds));
            const i32 wall_mask = ((terrain_mask));
            const bool wall_clear_override = ((false));
            wall_bounds.min_x -= TerrainScanMargin02Pc;
            wall_bounds.min_z -= TerrainScanMargin02Pc;
            wall_bounds.max_x += TerrainScanMargin02Pc;
            wall_bounds.max_z += TerrainScanMargin02Pc;
            WallSplCount = 0;
            for (TERRAIN_SPATIAL_NODE *node = ((TERRAIN_SET_PC *)CurrentTerrain)->spatial_nodes; node != NULL;
                 node = ((TERRAIN_SPATIAL_CHUNK *)(((u8 *)(node)) - offsetof(TERRAIN_SPATIAL_CHUNK, node)))->previous) {
                const u8 mask = node->field_02 >> 8;
                if (mask != 0 && (mask & wall_mask) == 0)
                    continue;
                for (i32 first = 0; first < node->point_count; first += 16) {
                    NUVEC *points = node->points + first;
                    i32 end = node->point_count;
                    if (points[0].y != 2147483648.0f) {
                        if (points[1].y < wall_bounds.min_x || points[0].y > wall_bounds.max_x ||
                            points[3].y < wall_bounds.min_z || points[2].y > wall_bounds.max_z)
                            continue;
                        end = MIN(end, first + 16);
                    }
                    for (i32 i = first; i < end; ++i) {
                        NUVEC &a = node->points[i];
                        NUVEC &b = node->points[i + 1];
                        if ((a.x < wall_bounds.min_x || b.x > wall_bounds.max_x) &&
                            (b.x < wall_bounds.min_x || a.x > wall_bounds.max_x))
                            continue;
                        if ((a.z < wall_bounds.min_z || b.z > wall_bounds.max_z) &&
                            (b.z < wall_bounds.min_z || a.z > wall_bounds.max_z))
                            continue;
                        if (WallSplCount < 64) {
                            ((TERRAIN_WALL_POINT *)WallSplList)[WallSplCount].position = a;
                            ((TERRAIN_WALL_POINT *)WallSplList)[WallSplCount + 1].position = b;
                            for (i32 p = 0; p < 2; ++p) {
                                u8 *material = ((TERRAIN_WALL_POINT *)WallSplList)[WallSplCount + p].material;
                                material[0] = ((u8)(node->field_02));
                                material[1] = mask;
                                material[2] = 0;
                                material[3] = 0;
                            }
                            WallSplCount += 2;
                        }
                    }
                }
            }
            if (wall_clear_override)
                TerrOverRideScan = NULL;
        } while (0);
    }
    TerrOverRideScan = NULL;
}
extern "C" void ScanWallSplineTerrain(i32 unused_first, i32 terrain_mask, i32 unused_third) {
    WallSplinesOnly = 0;
    TerrainScaledShapeCursorPc = ((TERRAIN_SHAPE *)(ScaleTerrainT1));
    platinrange = 0;
    ((TerrainQuery_s *)TerI)->scan_group_index = -1;
    TerrainScanBounds bounds;
    f32 scan_radius;
    do {
        TerrainScanBounds &scan_bounds = ((bounds));
        f32 &bounds_radius = ((scan_radius));
        const TerrainQuery_s &scan_query = *((TerrainQuery_s *)TerI);
        if (scan_query.scan_result != 1) {
            const f32 move_length =
                NuFsqrt(scan_query.movement.x * scan_query.movement.x + scan_query.movement.y * scan_query.movement.y +
                        scan_query.movement.z * scan_query.movement.z);
            const f32 reach = scan_query.collision_radius + TerrainScanMargin10Pc + move_length;
            const f32 vertical = reach * scan_query.object_scale;
            scan_bounds.min_x = scan_query.position.x - NuTimeBarTop - reach;
            scan_bounds.max_x = scan_query.position.x + NuTimeBarTop + reach;
            scan_bounds.min_y = scan_query.position.y - NuTimeBarTop - vertical;
            scan_bounds.max_y = vertical + (scan_query.position.y + NuTimeBarTop);
            scan_bounds.min_z = scan_query.position.z - NuTimeBarTop - reach;
            scan_bounds.max_z = scan_query.position.z + NuTimeBarTop + reach;
            bounds_radius = reach;
        } else {
            const f32 radius = scan_query.collision_radius;
            if (scan_query.movement.x <= 0.0f) {
                scan_bounds.min_x = scan_query.position.x + scan_query.movement.x - TerrainScanMargin02Pc - radius;
                scan_bounds.max_x = scan_query.position.x + TerrainScanMargin02Pc + radius;
            } else {
                scan_bounds.min_x = scan_query.position.x - TerrainScanMargin02Pc - radius;
                scan_bounds.max_x = radius + (scan_query.position.x + scan_query.movement.x + TerrainScanMargin02Pc);
            }
            if (scan_query.movement.y <= 0.0f) {
                scan_bounds.min_y = scan_query.position.y + scan_query.movement.y - TerrainScanMargin02Pc - radius;
                scan_bounds.max_y = scan_query.position.y + TerrainScanMargin02Pc + radius;
            } else {
                scan_bounds.min_y = scan_query.position.y - TerrainScanMargin02Pc - radius;
                scan_bounds.max_y = scan_query.position.y + scan_query.movement.y + TerrainScanMargin02Pc + radius;
            }
            if (0.0f >= scan_query.movement.z) {
                scan_bounds.min_z = scan_query.position.z + scan_query.movement.z - TerrainScanMargin02Pc - radius;
                scan_bounds.max_z = scan_query.position.z + TerrainScanMargin02Pc + radius;
            } else {
                scan_bounds.min_z = scan_query.position.z - TerrainScanMargin02Pc - radius;
                scan_bounds.max_z = scan_query.position.z + scan_query.movement.z + TerrainScanMargin02Pc + radius;
            }
            const f32 dx = scan_bounds.min_x - scan_bounds.max_x;
            const f32 dy = scan_bounds.min_y - scan_bounds.max_y;
            const f32 dz = scan_bounds.min_z - scan_bounds.max_z;
            bounds_radius = NuFsqrt(dx * dx + dy * dy + dz * dz);
        }
        if (!((true)) && scan_query.object_scale > 1.0f)
            bounds_radius += (scan_query.object_scale - 1.0f) * scan_query.collision_radius;
    } while (0);
    i16 *terminator = ((i16 *)(((TerrainQuery_s *)TerI)->scan_list_storage));
    terminator[0] = 0;
    terminator[1] = 0;
    do {
        TerrainScanBounds wall_bounds = ((bounds));
        const i32 wall_mask = ((terrain_mask));
        const bool wall_clear_override = ((true));
        wall_bounds.min_x -= TerrainScanMargin02Pc;
        wall_bounds.min_z -= TerrainScanMargin02Pc;
        wall_bounds.max_x += TerrainScanMargin02Pc;
        wall_bounds.max_z += TerrainScanMargin02Pc;
        WallSplCount = 0;
        for (TERRAIN_SPATIAL_NODE *node = ((TERRAIN_SET_PC *)CurrentTerrain)->spatial_nodes; node != NULL;
             node = ((TERRAIN_SPATIAL_CHUNK *)(((u8 *)(node)) - offsetof(TERRAIN_SPATIAL_CHUNK, node)))->previous) {
            const u8 mask = node->field_02 >> 8;
            if (mask != 0 && (mask & wall_mask) == 0)
                continue;
            for (i32 first = 0; first < node->point_count; first += 16) {
                
                i32 end = node->point_count;
                if ((node->points + first)[0].y != 2147483648.0f) {
                    if ((node->points + first)[1].y < wall_bounds.min_x || (node->points + first)[0].y > wall_bounds.max_x ||
                        (node->points + first)[3].y < wall_bounds.min_z || (node->points + first)[2].y > wall_bounds.max_z)
                        continue;
                    end = MIN(end, first + 16);
                }
                for (i32 i = first; i < end; ++i) {
                    NUVEC &a = node->points[i];
                    NUVEC &b = node->points[1 + i];
                    if ((wall_bounds.min_x > a.x || b.x > wall_bounds.max_x) &&
                        (b.x < wall_bounds.min_x || a.x > wall_bounds.max_x))
                        continue;
                    if ((a.z < wall_bounds.min_z || b.z > wall_bounds.max_z) &&
                        (b.z < wall_bounds.min_z || a.z > wall_bounds.max_z))
                        continue;
                    if (64 > WallSplCount) {
                        ((TERRAIN_WALL_POINT *)WallSplList)[WallSplCount].position = a;
                        ((TERRAIN_WALL_POINT *)WallSplList)[WallSplCount + 1].position = b;
                        for (i32 p = 0; p < 2; ++p) {
                            u8 *material = ((TERRAIN_WALL_POINT *)WallSplList)[WallSplCount + p].material;
                            material[0] = ((u8)(node->field_02));
                            material[1] = mask;
                            material[2] = 0;
                            material[3] = 0;
                        }
                        WallSplCount += 2;
                    }
                }
            }
        }
        if (wall_clear_override)
            TerrOverRideScan = NULL;
    } while (0);
}
