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

struct TERRAIN_POLYGON_INFO_PC {
    f32 min_x, max_x, min_y, max_y, min_z, max_z;
    NUVEC vertices[4];
    NUVEC normals[2];
    union {
        u32 packed_metadata;
        struct {
            u8 material[2];
            u8 flags;
            u8 normal_flags;
        };
    };
};
STATIC_ASSERT(sizeof(TERRAIN_POLYGON_INFO_PC) == 0x64);
STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, min_y) == 8);
STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, max_y) == 0xc);
STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, vertices) == 0x18);
STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, normals) == 0x48);
STATIC_ASSERT(offsetof(TERRAIN_POLYGON_INFO_PC, material) == 0x60);

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
    f32 m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33;
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

typedef struct TERRAIN_TRACK_SLOT_PC {
    void *id;
    i16 platform_index;
    u16 flags;
    i16 platform_contact_state;
    i16 wall_contact_state;
    NUVEC impact_normal;
} TERRAIN_TRACK_SLOT_PC;

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
    TERRAIN_SHAPE *working_surface;
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
    TERRAIN_TRACK_CONTACT_ACTIVE = 1,
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
struct TERRAIN_SPHERE {
    NUVEC position;
    f32 radius;
};
struct TERRAIN_AXIS_FREEDOM_SHAPE {
    NUVEC offset;
    f32 angle, radius;
};
extern "C" {
    extern TERRAIN_SHAPE TerrPolyInfo, PlatImpactTer;
    extern i32 TerrPolyObj, PlatCrush, plathitid, terrhitflags;
    extern TERRAIN_TRACK_SLOT_PC *CurTrackInfo;
    extern TERRAIN_AXIS_FREEDOM_SHAPE *TerrShape;
    extern NUVEC ShadNorm, PlatImpactNorm, TerrWallNorm;
    extern u8 TerrWallTab[4];
    extern void *TerImpactData;
    extern i32 *TerImpactDataCount;
    extern i32 TerImpactDataMax;
    extern f32 wallover, TerConTol, TerrWallDeflectYScale;
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
extern "C" {
    void DerotateMovementVector() {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;

        query->movement_yaw = static_cast<f32>(NuAtan2DA(query->movement.x, query->movement.z));
        const f32 horizontal_length =
            NuFsqrt(query->movement.x * query->movement.x + query->movement.z * query->movement.z);
        query->movement_pitch = static_cast<f32>(NuAtan2DA(-query->movement.y, horizontal_length));
        query->movement_length = NuFsqrt(query->movement.x * query->movement.x + query->movement.y * query->movement.y +
                                         query->movement.z * query->movement.z);
    }
    void RotateVec(NUVEC *source, NUVEC *destination) {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        const f32 pitch = query->movement_pitch;
        const f32 sin_pitch = NuTrigTable[(static_cast<i32>(pitch) >> 1) & (NUTRIGTABLE_COUNT - 1)];
        const f32 cos_pitch = NuTrigTable[(static_cast<i32>(pitch + 16384.0f) >> 1) & (NUTRIGTABLE_COUNT - 1)];

        const f32 rotated_z = source->y * sin_pitch + source->z * cos_pitch;
        destination->y = source->y * cos_pitch - source->z * sin_pitch;

        const f32 yaw = query->movement_yaw;
        const f32 sin_yaw = NuTrigTable[(static_cast<i32>(yaw) >> 1) & (NUTRIGTABLE_COUNT - 1)];
        const f32 cos_yaw = NuTrigTable[(static_cast<i32>(yaw + 16384.0f) >> 1) & (NUTRIGTABLE_COUNT - 1)];

        destination->z = rotated_z * cos_yaw - source->x * sin_yaw;
        destination->x = rotated_z * sin_yaw + source->x * cos_yaw;
    }
    void DeRotateTerrain(TERRAIN_SHAPE *surface) {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;

        const f32 sin_pitch = NuTrigTable[(static_cast<i32>(-query->movement_pitch) >> 1) & 0x7fff];
        const f32 cos_pitch = NuTrigTable[(static_cast<i32>(16384.0f - query->movement_pitch) >> 1) & 0x7fff];
        const f32 sin_yaw = NuTrigTable[(static_cast<i32>(-query->movement_yaw) >> 1) & 0x7fff];
        const f32 cos_yaw = NuTrigTable[(static_cast<i32>(16384.0f - query->movement_yaw) >> 1) & 0x7fff];
        const f32 start_z = query->local_start.z;
        const f32 start_x = query->local_start.x;
        const f32 start_y = query->local_start.y;

        for (i32 vertex_index = 0; vertex_index < 3; ++vertex_index) {
            const NUVEC &vertex = surface->vertices[vertex_index];
            NUVEC &transformed = query->transformed_vertices[vertex_index];
            const f32 relative_z = vertex.z - start_z;
            const f32 relative_x = vertex.x - start_x;
            const f32 rotated_x = relative_z * sin_yaw + relative_x * cos_yaw;
            const f32 rotated_z = relative_z * cos_yaw - relative_x * sin_yaw;

            transformed.x = rotated_x;
            transformed.y = (vertex.y - start_y) * cos_pitch - rotated_z * sin_pitch;
            transformed.z = (vertex.y - start_y) * sin_pitch + rotated_z * cos_pitch;
        }

        if (65536.0f > surface->normals[1].y) {
            const NUVEC &vertex = surface->vertices[3];
            NUVEC &transformed = query->transformed_vertices[3];
            const f32 relative_z = vertex.z - start_z;
            const f32 relative_x = vertex.x - start_x;
            const f32 rotated_x = relative_z * sin_yaw + relative_x * cos_yaw;
            const f32 rotated_z = relative_z * cos_yaw - relative_x * sin_yaw;

            transformed.x = rotated_x;
            transformed.y = (vertex.y - start_y) * cos_pitch - rotated_z * sin_pitch;
            transformed.z = (vertex.y - start_y) * sin_pitch + rotated_z * cos_pitch;
        }
    }
    void DeRotatePoint(NUVEC *point) {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;

        const f32 sin_pitch = NuTrigTable[(static_cast<i32>(-query->movement_pitch) >> 1) & 0x7fff];
        const f32 cos_pitch = NuTrigTable[(static_cast<i32>(16384.0f - query->movement_pitch) >> 1) & 0x7fff];
        const f32 sin_yaw = NuTrigTable[(static_cast<i32>(-query->movement_yaw) >> 1) & 0x7fff];
        const f32 cos_yaw = NuTrigTable[(static_cast<i32>(16384.0f - query->movement_yaw) >> 1) & 0x7fff];

        const f32 relative_z = point->z - query->position.z;
        const f32 relative_x = point->x - query->position.x;

        const f32 rotated_x = relative_z * sin_yaw + relative_x * cos_yaw;
        const f32 rotated_z = relative_z * cos_yaw - relative_x * sin_yaw;

        point->x = rotated_x;
        const f32 relative_y = point->y + query->collision_radius - query->position.y;
        point->z = relative_y * sin_pitch + rotated_z * cos_pitch;
        point->y = relative_y * cos_pitch - rotated_z * sin_pitch;
    }
    i16 InsideLineF(f32 point_u, f32 point_v, f32 line_start_u, f32 line_start_v, f32 line_end_u, f32 line_end_v) {
        if ((point_u - line_start_u) * (line_end_v - line_start_v) +
                (point_v - line_start_v) * (line_start_u - line_end_u) <
            0.0f) {
            return 0;
        }
        return 1;
    }
    i16 InsidePolLines(f32 point_x, f32 point_y, f32 point_z, f32 edge_a_x, f32 edge_a_y, f32 edge_a_z, f32 edge_b_x,
                       f32 edge_b_y, f32 edge_b_z, NUVEC *normal) {
        TERRAIN_FLOAT_BITS_PC absolute_5;
        absolute_5.value = (normal->y);
        absolute_5.bits &= 0x7fffffff;
        TERRAIN_FLOAT_BITS_PC absolute_6;
        absolute_6.value = (normal->x);
        absolute_6.bits &= 0x7fffffff;
        TERRAIN_FLOAT_BITS_PC absolute_7;
        absolute_7.value = (normal->y);
        absolute_7.bits &= 0x7fffffff;
        TERRAIN_FLOAT_BITS_PC absolute_8;
        absolute_8.value = (normal->z);
        absolute_8.bits &= 0x7fffffff;

        // Project onto the plane perpendicular to the dominant normal component.
        // The call order preserves the polygon winding in that projection.
        if (absolute_5.value >= absolute_6.value && absolute_7.value >= absolute_8.value) {
            if (0.0f > normal->y) {
                if (InsideLineF(point_x, point_z, 0.0f, 0.0f, edge_a_x, edge_a_z) == 0 ||
                    InsideLineF(point_x, point_z, edge_b_x, edge_b_z, 0.0f, 0.0f) == 0) {
                    return 0;
                }
                return InsideLineF(point_x, point_z, edge_a_x, edge_a_z, edge_b_x, edge_b_z) != 0;
            }

            if (InsideLineF(point_x, point_z, edge_a_x, edge_a_z, 0.0f, 0.0f) == 0 ||
                InsideLineF(point_x, point_z, 0.0f, 0.0f, edge_b_x, edge_b_z) == 0) {
                return 0;
            }
            return InsideLineF(point_x, point_z, edge_b_x, edge_b_z, edge_a_x, edge_a_z) != 0;
        }
        TERRAIN_FLOAT_BITS_PC absolute_9;
        absolute_9.value = (normal->y);
        absolute_9.bits &= 0x7fffffff;
        TERRAIN_FLOAT_BITS_PC absolute_10;
        absolute_10.value = (normal->x);
        absolute_10.bits &= 0x7fffffff;
        TERRAIN_FLOAT_BITS_PC absolute_11;
        absolute_11.value = (normal->z);
        absolute_11.bits &= 0x7fffffff;
        TERRAIN_FLOAT_BITS_PC absolute_12;
        absolute_12.value = (normal->x);
        absolute_12.bits &= 0x7fffffff;

        if (absolute_9.value > absolute_10.value || absolute_11.value > absolute_12.value) {
            if (0.0f > normal->z) {
                if (InsideLineF(point_y, point_x, 0.0f, 0.0f, edge_a_y, edge_a_x) == 0 ||
                    InsideLineF(point_y, point_x, edge_b_y, edge_b_x, 0.0f, 0.0f) == 0) {
                    return 0;
                }
                return InsideLineF(point_y, point_x, edge_a_y, edge_a_x, edge_b_y, edge_b_x) != 0;
            }

            if (InsideLineF(point_y, point_x, edge_a_y, edge_a_x, 0.0f, 0.0f) == 0 ||
                InsideLineF(point_y, point_x, 0.0f, 0.0f, edge_b_y, edge_b_x) == 0) {
                return 0;
            }
            return InsideLineF(point_y, point_x, edge_b_y, edge_b_x, edge_a_y, edge_a_x) != 0;
        }

        if (0.0f > normal->x) {
            if (InsideLineF(point_y, point_z, edge_a_y, edge_a_z, 0.0f, 0.0f) == 0 ||
                InsideLineF(point_y, point_z, 0.0f, 0.0f, edge_b_y, edge_b_z) == 0) {
                return 0;
            }
            return InsideLineF(point_y, point_z, edge_b_y, edge_b_z, edge_a_y, edge_a_z) != 0;
        }

        if (InsideLineF(point_y, point_z, 0.0f, 0.0f, edge_a_y, edge_a_z) == 0 ||
            InsideLineF(point_y, point_z, edge_b_y, edge_b_z, 0.0f, 0.0f) == 0) {
            return 0;
        }
        return InsideLineF(point_y, point_z, edge_a_y, edge_a_z, edge_b_y, edge_b_z) != 0;
    }
    i32 CheckCylinder(i32 first_vertex, i32 second_vertex, i32 *vertex_mask, i32 remaining_vertex_mask) {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;

        // Reject an edge when both endpoints lie outside the swept sphere's
        // expanded bounds. Once an edge is rejected this way, its endpoint bits
        // can be removed from the later vertex tests as well.
        if ((query->transformed_vertices[first_vertex].x > query->collision_radius &&
             query->transformed_vertices[second_vertex].x > query->collision_radius) ||
            (query->transformed_vertices[first_vertex].x < -query->collision_radius &&
             query->transformed_vertices[second_vertex].x < -query->collision_radius) ||
            (query->transformed_vertices[first_vertex].y > query->collision_radius &&
             query->transformed_vertices[second_vertex].y > query->collision_radius) ||
            (query->transformed_vertices[first_vertex].y < -query->collision_radius &&
             query->transformed_vertices[second_vertex].y < -query->collision_radius) ||
            (query->transformed_vertices[first_vertex].z < -query->collision_radius &&
             query->transformed_vertices[second_vertex].z < -query->collision_radius) ||
            (query->transformed_vertices[first_vertex].z > query->movement_length + query->collision_radius &&
             query->transformed_vertices[second_vertex].z > query->movement_length + query->collision_radius)) {
            *vertex_mask &= remaining_vertex_mask;
            return 0;
        }

        NUVEC edge_direction;
        edge_direction.x = query->transformed_vertices[second_vertex].x - query->transformed_vertices[first_vertex].x;
        edge_direction.y = query->transformed_vertices[second_vertex].y - query->transformed_vertices[first_vertex].y;
        edge_direction.z = query->transformed_vertices[second_vertex].z - query->transformed_vertices[first_vertex].z;
        const f32 horizontal_length_sq = edge_direction.x * edge_direction.x + edge_direction.y * edge_direction.y;

        // An almost vertical edge has no stable radial cylinder in the XY plane.
        // It can only overlap the sphere at the start of the sweep when it crosses
        // z = 0.
        if (horizontal_length_sq < 1.0e-12f) {
            if ((query->transformed_vertices[first_vertex].z < 0.0f &&
                 query->transformed_vertices[second_vertex].z < 0.0f) ||
                (query->transformed_vertices[first_vertex].z > 0.0f &&
                 query->transformed_vertices[second_vertex].z > 0.0f)) {
                return 0;
            }

            const f32 distance_sq =
                query->transformed_vertices[second_vertex].x * query->transformed_vertices[second_vertex].x +
                query->transformed_vertices[second_vertex].y * query->transformed_vertices[second_vertex].y;
            if (distance_sq > query->collision_radius_sq) {
                return 0;
            }

            const f32 distance = NuFsqrt(distance_sq);
            const f32 overlap_time = distance - query->collision_radius;
            if (query->hit_time <= overlap_time) {
                return 0;
            }

            NUVEC local_normal = {
                -query->transformed_vertices[second_vertex].x,
                -query->transformed_vertices[second_vertex].y,
                0.0f,
            };
            RotateVec(&local_normal, &local_normal);
            const NUVEC &surface_normal = query->working_surface->normals[0];
            if (local_normal.x * surface_normal.x + local_normal.y * surface_normal.y +
                    local_normal.z * surface_normal.z <=
                0.0f) {
                return 0;
            }

            query = (TerrainQuery_s *)TerI;
            query->hit_time = overlap_time;
            f32 inverse_distance = 0.0f;
            if (distance != 0.0f) {
                inverse_distance = 1.0f / distance;
            }
            query->hit_type = TERRAIN_HIT_TYPE_CYLINDER | TERRAIN_HIT_TYPE_SECOND_NORMAL;
            query->movement_normal.x = -query->transformed_vertices[second_vertex].x * inverse_distance;
            query->movement_normal.y = -query->transformed_vertices[second_vertex].y * inverse_distance;
            query->movement_normal.z = 0.0f;
            return 1;
        }

        // Squared radial distance from the movement axis to the infinite line
        // containing the edge. This avoids a square root for the common miss.
        const f32 line_cross = query->transformed_vertices[first_vertex].x * edge_direction.y -
                               query->transformed_vertices[first_vertex].y * edge_direction.x;
        if (line_cross * line_cross > query->collision_radius_sq * horizontal_length_sq) {
            *vertex_mask &= remaining_vertex_mask;
            return 0;
        }

        f32 line_distance_sq = 0.0f;
        if (horizontal_length_sq != 0.0f && line_cross != 0.0f) {
            line_distance_sq = line_cross * (line_cross / horizontal_length_sq);
        }

        const f32 edge_length = NuFsqrt(horizontal_length_sq + edge_direction.z * edge_direction.z);
        f32 inverse_edge_length = 0.0f;
        if (edge_length != 0.0f) {
            inverse_edge_length = 1.0f / edge_length;
        }
        edge_direction.x *= inverse_edge_length;
        edge_direction.y *= inverse_edge_length;
        edge_direction.z *= inverse_edge_length;

        NUVEC line_cross_vector = {
            -query->transformed_vertices[first_vertex].x,
            -query->transformed_vertices[first_vertex].y,
            -query->transformed_vertices[first_vertex].z,
        };
        NuVecCross(&line_cross_vector, &line_cross_vector, &edge_direction);

        NUVEC horizontal_perpendicular = {-edge_direction.y, edge_direction.x, 0.0f};
        const f32 horizontal_direction_sq = horizontal_perpendicular.x * horizontal_perpendicular.x +
                                            horizontal_perpendicular.y * horizontal_perpendicular.y;
        const f32 height_numerator =
            line_cross_vector.x * horizontal_perpendicular.x + line_cross_vector.y * horizontal_perpendicular.y;
        f32 negative_closest_height = 0.0f;
        if (horizontal_direction_sq != 0.0f && height_numerator != 0.0f) {
            negative_closest_height = height_numerator / horizontal_direction_sq;
        }
        const f32 closest_height = -negative_closest_height;

        // This normal lies in the plane formed by the edge and the movement axis.
        // Its z component tells how far along the sweep the cylinder surface is
        // reached.
        NUVEC cylinder_side = {
            edge_direction.x * edge_direction.z,
            -edge_direction.y * edge_direction.z,
            -(edge_direction.x * edge_direction.x + edge_direction.y * edge_direction.y),
        };
        const f32 cylinder_side_length = NuFsqrt(cylinder_side.x * cylinder_side.x + cylinder_side.y * cylinder_side.y +
                                                 cylinder_side.z * cylinder_side.z);
        if (cylinder_side_length != 0.0f && cylinder_side.z != 0.0f) {
            cylinder_side.z /= cylinder_side_length;
        } else {
            cylinder_side.z = 0.0f;
        }

        const f32 radial_extent = NuFsqrt(query->collision_radius_sq - line_distance_sq);
        f32 height_offset = 0.0f;
        if (radial_extent != 0.0f && cylinder_side.z != 0.0f) {
            height_offset = radial_extent / cylinder_side.z;
        }

        f32 hit_distance;
        if (height_offset >= 0.0f) {
            hit_distance = closest_height - height_offset;
        } else {
            hit_distance = height_offset - negative_closest_height;
        }
        if (hit_distance >= 0.0f && hit_distance <= query->movement_length) {
            const f32 edge_parameter = (-query->transformed_vertices[first_vertex].y * edge_direction.y -
                                        query->transformed_vertices[first_vertex].x * edge_direction.x) +
                                       (hit_distance - query->transformed_vertices[first_vertex].z) * edge_direction.z;
            if (edge_parameter > 0.0f && edge_parameter <= edge_length) {
                f32 hit_time = 0.0f;
                if (hit_distance != 0.0f && query->movement_length != 0.0f) {
                    hit_time = hit_distance / query->movement_length;
                }
                if (query->hit_time <= hit_time) {
                    *vertex_mask &= remaining_vertex_mask;
                    return 0;
                }

                query->hit_type = TERRAIN_HIT_TYPE_CYLINDER;
                query->hit_time = hit_time;
                query->movement_normal.x =
                    -(query->transformed_vertices[first_vertex].x + edge_direction.x * edge_parameter);
                query->movement_normal.y =
                    -(query->transformed_vertices[first_vertex].y + edge_direction.y * edge_parameter);
                query->movement_normal.z =
                    hit_distance - (edge_parameter * edge_direction.z + query->transformed_vertices[first_vertex].z);
                *vertex_mask &= remaining_vertex_mask;
                return 1;
            }
        }

        // No forward-time hit was found. Test whether the sphere already overlaps
        // the finite edge at the start of the sweep.
        if ((query->transformed_vertices[first_vertex].z < -query->collision_radius &&
             query->transformed_vertices[second_vertex].z < -query->collision_radius) ||
            (query->transformed_vertices[first_vertex].z > query->collision_radius &&
             query->transformed_vertices[second_vertex].z > query->collision_radius)) {
            return 0;
        }

        const f32 closest_parameter = -query->transformed_vertices[first_vertex].x * edge_direction.x -
                                      query->transformed_vertices[first_vertex].y * edge_direction.y -
                                      query->transformed_vertices[first_vertex].z * edge_direction.z;
        const f32 edge_projection =
            (query->transformed_vertices[second_vertex].x - query->transformed_vertices[first_vertex].x) *
                edge_direction.x +
            (query->transformed_vertices[second_vertex].y - query->transformed_vertices[first_vertex].y) *
                edge_direction.y +
            (query->transformed_vertices[second_vertex].z - query->transformed_vertices[first_vertex].z) *
                edge_direction.z;
        if (closest_parameter < 0.0f || closest_parameter > edge_projection) {
            return 0;
        }

        // The normalized direction is no longer needed after the closest
        // parameter has been bounded, so reuse it for the closest point itself.
        edge_direction.x = query->transformed_vertices[first_vertex].x + edge_direction.x * closest_parameter;
        edge_direction.y = query->transformed_vertices[first_vertex].y + edge_direction.y * closest_parameter;
        edge_direction.z = query->transformed_vertices[first_vertex].z + edge_direction.z * closest_parameter;
        const f32 distance_sq = edge_direction.x * edge_direction.x + edge_direction.y * edge_direction.y +
                                edge_direction.z * edge_direction.z;
        if (distance_sq >= query->collision_radius_sq) {
            return 0;
        }

        const f32 distance = NuFsqrt(distance_sq);
        const f32 overlap_time = distance - query->collision_radius;
        if (query->hit_time <= overlap_time) {
            return 0;
        }

        RotateVec(&edge_direction, &horizontal_perpendicular);
        const NUVEC &surface_normal = query->working_surface->normals[0];
        if (horizontal_perpendicular.x * surface_normal.x + horizontal_perpendicular.y * surface_normal.y +
                horizontal_perpendicular.z * surface_normal.z <
            0.0f) {
            query = (TerrainQuery_s *)TerI;
            query->hit_time = overlap_time;
            f32 inverse_distance = 0.0f;
            if (distance != 0.0f) {
                inverse_distance = 1.0f / distance;
            }
            query->hit_type = TERRAIN_HIT_TYPE_CYLINDER | TERRAIN_HIT_TYPE_SECOND_NORMAL;
            query->movement_normal.x = -edge_direction.x * inverse_distance;
            query->movement_normal.y = -edge_direction.y * inverse_distance;
            query->movement_normal.z = -edge_direction.z * inverse_distance;
            return 1;
        }
        return 0;
    }
    i32 CheckSphere(i32 vertex_index) {
        f32 radius = ((TerrainQuery_s *)TerI)->collision_radius;
        if (((TerrainQuery_s *)TerI)->transformed_vertices[vertex_index].z < -radius ||
            ((TerrainQuery_s *)TerI)->transformed_vertices[vertex_index].z >
                ((TerrainQuery_s *)TerI)->movement_length + radius) {
            return 0;
        }

        f32 radial_distance_sq = ((TerrainQuery_s *)TerI)->transformed_vertices[vertex_index].x *
                                     ((TerrainQuery_s *)TerI)->transformed_vertices[vertex_index].x +
                                 ((TerrainQuery_s *)TerI)->transformed_vertices[vertex_index].y *
                                     ((TerrainQuery_s *)TerI)->transformed_vertices[vertex_index].y;
        if (radial_distance_sq > ((TerrainQuery_s *)TerI)->collision_radius_sq) {
            return 0;
        }

        f32 radial_extent = NuFsqrt(((TerrainQuery_s *)TerI)->collision_radius_sq - radial_distance_sq);
        f32 hit_distance = ((TerrainQuery_s *)TerI)->transformed_vertices[vertex_index].z - radial_extent;
        if (0.0f <= hit_distance && ((TerrainQuery_s *)TerI)->movement_length >= hit_distance) {
            f32 hit_time = hit_distance / ((TerrainQuery_s *)TerI)->movement_length;
            if (((TerrainQuery_s *)TerI)->hit_time <= hit_time) {
                return 0;
            }

            ((TerrainQuery_s *)TerI)->hit_time = hit_time;
            ((TerrainQuery_s *)TerI)->hit_type = TERRAIN_HIT_TYPE_VERTEX;
            ((TerrainQuery_s *)TerI)->movement_normal.x =
                -((TerrainQuery_s *)TerI)->transformed_vertices[vertex_index].x;
            ((TerrainQuery_s *)TerI)->movement_normal.y =
                -((TerrainQuery_s *)TerI)->transformed_vertices[vertex_index].y;
            ((TerrainQuery_s *)TerI)->movement_normal.z = -radial_extent;
            return 1;
        }

        f32 distance_sq = radial_distance_sq + ((TerrainQuery_s *)TerI)->transformed_vertices[vertex_index].z *
                                                   ((TerrainQuery_s *)TerI)->transformed_vertices[vertex_index].z;
        if (((TerrainQuery_s *)TerI)->collision_radius_sq <= distance_sq) {
            return 0;
        }

        f32 distance = NuFsqrt(distance_sq);
        f32 overlap_time = distance - ((TerrainQuery_s *)TerI)->collision_radius;
        if (((TerrainQuery_s *)TerI)->hit_time <= overlap_time) {
            return 0;
        }

        TERRAIN_SHAPE *surface = ((TerrainQuery_s *)TerI)->working_surface;
        f32 primary_distance =
            (((TerrainQuery_s *)TerI)->local_start.x - surface->vertices[vertex_index].x) * surface->normals[0].x +
            (((TerrainQuery_s *)TerI)->local_start.y - surface->vertices[vertex_index].y) * surface->normals[0].y +
            (((TerrainQuery_s *)TerI)->local_start.z - surface->vertices[vertex_index].z) * surface->normals[0].z;
        if (0.0f >= primary_distance) {
            return 0;
        }

        ((TerrainQuery_s *)TerI)->hit_time = overlap_time;
        f32 inverse_distance = 0.0f;
        if (distance != 0.0f) {
            inverse_distance = 1.0f / distance;
        }
        ((TerrainQuery_s *)TerI)->hit_type = TERRAIN_HIT_TYPE_VERTEX | TERRAIN_HIT_TYPE_SECOND_NORMAL;
        ((TerrainQuery_s *)TerI)->movement_normal.x =
            -((TerrainQuery_s *)TerI)->transformed_vertices[vertex_index].x * inverse_distance;
        ((TerrainQuery_s *)TerI)->movement_normal.y =
            -((TerrainQuery_s *)TerI)->transformed_vertices[vertex_index].y * inverse_distance;
        ((TerrainQuery_s *)TerI)->movement_normal.z =
            -((TerrainQuery_s *)TerI)->transformed_vertices[vertex_index].z * inverse_distance;
        return 1;
    }
    i32 CheckSphereTer(NUVEC *position, f32 radius) {
        const f32 terrain_radius = ((TerrainQuery_s *)TerI)->collision_radius;
        const f32 combined_radius = radius + terrain_radius;

        if (position->z < -combined_radius ||
            position->z > ((TerrainQuery_s *)TerI)->movement_length + combined_radius) {
            return 0;
        }

        const f32 radial_distance_sq = position->x * position->x + position->y * position->y;
        const f32 combined_radius_sq = combined_radius * combined_radius;
        if (radial_distance_sq > combined_radius_sq) {
            return 0;
        }

        const f32 radial_extent = NuFsqrt(combined_radius_sq - radial_distance_sq);
        const f32 hit_distance = position->z - radial_extent;
        if (hit_distance >= 0.0f && hit_distance <= ((TerrainQuery_s *)TerI)->movement_length) {
            const f32 hit_time = hit_distance / ((TerrainQuery_s *)TerI)->movement_length;
            if (hit_time >= ((TerrainQuery_s *)TerI)->hit_time) {
                return 0;
            }

            ((TerrainQuery_s *)TerI)->hit_time = hit_time;
            ((TerrainQuery_s *)TerI)->hit_type = TERRAIN_HIT_TYPE_SPHERE;
            ((TerrainQuery_s *)TerI)->movement_normal.x = -position->x;
            ((TerrainQuery_s *)TerI)->movement_normal.y = -position->y;
            ((TerrainQuery_s *)TerI)->movement_normal.z = -radial_extent;
            return 1;
        }

        const f32 distance_sq = radial_distance_sq + position->z * position->z;
        if (distance_sq >= combined_radius_sq) {
            return 0;
        }

        const f32 distance = NuFsqrt(distance_sq);
        f32 inverse_distance = 0.0f;
        if (distance != 0.0f) {
            inverse_distance = 1.0f / distance;
        }

        ((TerrainQuery_s *)TerI)->hit_time = 0.0f;
        ((TerrainQuery_s *)TerI)->hit_type = TERRAIN_HIT_TYPE_SPHERE | TERRAIN_HIT_TYPE_SECOND_NORMAL;
        ((TerrainQuery_s *)TerI)->movement_normal.x = -position->x * inverse_distance;
        ((TerrainQuery_s *)TerI)->movement_normal.y = -position->y * inverse_distance;
        ((TerrainQuery_s *)TerI)->movement_normal.z = -position->z * inverse_distance;
        return 1;
    }
    i32 HitPoly(f32 primary_start, f32 primary_end, f32 secondary_start, f32 secondary_end, TERRAIN_SHAPE *surface) {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        const f32 radius = query->collision_radius;
        const f32 no_secondary_normal = 65536.0f;

        i32 collision_found = 0;
        f32 candidate_time = 0.0f;
        NUVEC contact_point;

        const NUVEC &primary_origin = surface->vertices[0];
        NUVEC &primary_normal = surface->normals[0];
        contact_point.x = query->local_start.x - radius * primary_normal.x - primary_origin.x;
        contact_point.y = query->local_start.y - radius * primary_normal.y - primary_origin.y;
        contact_point.z = query->local_start.z - radius * primary_normal.z - primary_origin.z;

        i32 test_primary_face = 0;
        if (primary_start > 0.0f && primary_end < 0.0f) {
            candidate_time = primary_start / (primary_start - primary_end);
            contact_point.x += query->movement.x * candidate_time;
            contact_point.y += query->movement.y * candidate_time;
            contact_point.z += query->movement.z * candidate_time;
            test_primary_face = 1;
        } else if (primary_start <= 0.0f && primary_end <= 0.0f && primary_start >= -radius && primary_end >= -radius) {
            candidate_time = primary_start;
            const f32 projection_distance = -primary_start;
            contact_point.x += primary_normal.x * projection_distance;
            contact_point.y += primary_normal.y * projection_distance;
            contact_point.z += primary_normal.z * projection_distance;
            test_primary_face = 1;
        }

        if (test_primary_face != 0) {
            const NUVEC edge_0_1 = {
                surface->vertices[1].x - primary_origin.x,
                surface->vertices[1].y - primary_origin.y,
                surface->vertices[1].z - primary_origin.z,
            };
            const NUVEC edge_0_2 = {
                surface->vertices[2].x - primary_origin.x,
                surface->vertices[2].y - primary_origin.y,
                surface->vertices[2].z - primary_origin.z,
            };
            if (InsidePolLines(contact_point.x, contact_point.y, contact_point.z, edge_0_1.x, edge_0_1.y, edge_0_1.z,
                               edge_0_2.x, edge_0_2.y, edge_0_2.z, &primary_normal) != 0 &&
                candidate_time <= query->hit_time) {
                query->hit_time = candidate_time;
                query->hit_type = primary_start > 0.0f ? TERRAIN_HIT_TYPE_FACE
                                                       : TERRAIN_HIT_TYPE_FACE | TERRAIN_HIT_TYPE_SECOND_NORMAL;
                query->movement_normal = primary_normal;
                query->shape_adjusted = (surface->normal_flags & TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP) != 0;
                collision_found = 1;
            }
        }

        NUVEC &secondary_normal = surface->normals[1];
        if (secondary_normal.y < no_secondary_normal) {
            const NUVEC &secondary_origin = surface->vertices[3];
            contact_point.x = query->local_start.x - radius * secondary_normal.x - secondary_origin.x;
            contact_point.y = query->local_start.y - radius * secondary_normal.y - secondary_origin.y;
            contact_point.z = query->local_start.z - radius * secondary_normal.z - secondary_origin.z;

            i32 test_secondary_face = 0;
            if (secondary_start > 0.0f && secondary_end < 0.0f) {
                candidate_time = secondary_start / (secondary_start - secondary_end);
                contact_point.x += query->movement.x * candidate_time;
                contact_point.y += query->movement.y * candidate_time;
                contact_point.z += query->movement.z * candidate_time;
                test_secondary_face = 1;
            } else if (secondary_start <= 0.0f && secondary_end <= 0.0f && secondary_start >= -radius &&
                       secondary_end >= -radius) {
                candidate_time = secondary_start;
                const f32 projection_distance = -secondary_start;
                contact_point.x += secondary_normal.x * projection_distance;
                contact_point.y += secondary_normal.y * projection_distance;
                contact_point.z += secondary_normal.z * projection_distance;
                test_secondary_face = 1;
            }

            if (test_secondary_face != 0) {
                const NUVEC edge_3_2 = {
                    surface->vertices[2].x - secondary_origin.x,
                    surface->vertices[2].y - secondary_origin.y,
                    surface->vertices[2].z - secondary_origin.z,
                };
                const NUVEC edge_3_1 = {
                    surface->vertices[1].x - secondary_origin.x,
                    surface->vertices[1].y - secondary_origin.y,
                    surface->vertices[1].z - secondary_origin.z,
                };
                if (InsidePolLines(contact_point.x, contact_point.y, contact_point.z, edge_3_2.x, edge_3_2.y,
                                   edge_3_2.z, edge_3_1.x, edge_3_1.y, edge_3_1.z, &secondary_normal) != 0 &&
                    candidate_time < query->hit_time) {
                    query->hit_time = candidate_time;
                    query->hit_type = secondary_start > 0.0f ? TERRAIN_HIT_TYPE_FACE
                                                             : TERRAIN_HIT_TYPE_FACE | TERRAIN_HIT_TYPE_SECOND_NORMAL;
                    query->movement_normal = secondary_normal;
                    query->shape_adjusted = (surface->normal_flags & TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP) != 0;
                    collision_found = 1;
                }
            }
        }

        if (radius != 0.0f) {
            query->working_surface = surface;
            i32 vertex_mask = 0xf;
            DeRotateTerrain(surface);

            if (primary_start >= -radius) {
                if (CheckCylinder(0, 1, &vertex_mask, 0xc) != 0) {
                    query = (TerrainQuery_s *)TerI;
                    query->hit_edge = TERRAIN_HIT_EDGE_0_1;
                    query->shape_adjusted = (surface->normal_flags & TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP) != 0;
                    collision_found = 1;
                }
                if (CheckCylinder(2, 0, &vertex_mask, 0xa) != 0) {
                    query = (TerrainQuery_s *)TerI;
                    query->hit_edge = TERRAIN_HIT_EDGE_2_0;
                    query->shape_adjusted = (surface->normal_flags & TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP) != 0;
                    collision_found = 1;
                }
            }

            if (CheckCylinder(1, 2, &vertex_mask, 0x9) != 0) {
                query = (TerrainQuery_s *)TerI;
                query->hit_edge = TERRAIN_HIT_EDGE_1_2;
                query->shape_adjusted = (surface->normal_flags & (TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP |
                                                                  TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP)) != 0;
                collision_found = 1;
            }

            if (secondary_normal.y < no_secondary_normal && secondary_start >= -radius) {
                if (CheckCylinder(1, 3, &vertex_mask, 0x5) != 0) {
                    query = (TerrainQuery_s *)TerI;
                    query->hit_edge = TERRAIN_HIT_EDGE_1_3;
                    query->shape_adjusted = (surface->normal_flags & TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP) != 0;
                    collision_found = 1;
                }
                if (CheckCylinder(3, 2, &vertex_mask, 0x3) != 0) {
                    query = (TerrainQuery_s *)TerI;
                    query->hit_edge = TERRAIN_HIT_EDGE_3_2;
                    query->shape_adjusted = (surface->normal_flags & TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP) != 0;
                    collision_found = 1;
                }
                if ((vertex_mask & (1 << 3)) != 0 && CheckSphere(3) != 0) {
                    ((TerrainQuery_s *)TerI)->shape_adjusted =
                        (surface->normal_flags & TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP) != 0;
                    collision_found = 1;
                }
            }

            if (primary_start >= -radius) {
                if ((vertex_mask & (1 << 0)) != 0 && CheckSphere(0) != 0) {
                    ((TerrainQuery_s *)TerI)->shape_adjusted =
                        (surface->normal_flags & TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP) != 0;
                    collision_found = 1;
                }
                if ((vertex_mask & (1 << 1)) != 0 && CheckSphere(1) != 0) {
                    ((TerrainQuery_s *)TerI)->shape_adjusted =
                        (surface->normal_flags &
                         (TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP | TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP)) != 0;
                    collision_found = 1;
                }
            }

            if ((vertex_mask & (1 << 2)) != 0 && CheckSphere(2) != 0) {
                ((TerrainQuery_s *)TerI)->shape_adjusted =
                    (surface->normal_flags &
                     (TERRAIN_SHAPE_NORMAL_FLAG_FIRST_STEEP | TERRAIN_SHAPE_NORMAL_FLAG_SECOND_STEEP)) != 0;
                collision_found = 1;
            }
        }

        query = (TerrainQuery_s *)TerI;
        if (collision_found != 0) {
            query->surface = surface;
        }
        query->unclamped_hit_time = query->hit_time;
        if (query->hit_time < 0.0f) {
            query->hit_time = 0.0f;
        }
        return collision_found;
    }
    i32 HitWallSpline() {
        f32 hit = 0.0f;
        if (WallSplCount == 0)
            return hit;
        for (i32 i = 0; i < WallSplCount; i += 2) {
            const NUVEC &a = ((TERRAIN_WALL_POINT *)WallSplList)[i].position;
            const NUVEC &b = ((TERRAIN_WALL_POINT *)WallSplList)[i + 1].position;
            TerrainQuery_s *query = (TerrainQuery_s *)TerI;
            TERRAIN_FLOAT_BITS_PC absolute_0;
            absolute_0.value = (query->position.x - a.x);
            absolute_0.bits &= 0x7fffffff;
            TERRAIN_FLOAT_BITS_PC absolute_1;
            absolute_1.value = (query->position.z - a.z);
            absolute_1.bits &= 0x7fffffff;

            if (absolute_0.value >= 64.0f || absolute_1.value >= 64.0f)
                continue;
            NUVEC normal = {b.z - a.z, 0.0f, a.x - b.x};
            NuVecNorm(&normal, &normal);
            const f32 radius = query->collision_radius;
            const f32 px = query->position.x;
            const f32 pz = query->position.z;
            const f32 mx = query->movement.x;
            const f32 mz = query->movement.z;
            const f32 end_distance = ((mx + px) - a.x) * normal.x + ((mz + pz) - a.z) * normal.z - radius;
            const f32 start_distance = (px - a.x) * normal.x + (pz - a.z) * normal.z - radius;
            if ((end_distance < 0.0f || start_distance < 0.0f) && start_distance > -radius) {
                f32 time, x, z;
                if (start_distance < 0.0f) {
                    time = start_distance;
                    x = (px - normal.x * start_distance) - normal.x * radius;
                    z = (pz - normal.z * start_distance) - normal.z * radius;
                } else {
                    time = start_distance / (start_distance - end_distance);
                    x = (mx * time + px) - normal.x * radius;
                    z = (mz * time + pz) - normal.z * radius;
                }
                const f32 dx = b.x - a.x;
                const f32 dz = b.z - a.z;
                if ((x - a.x) * dx + dz * (z - a.z) >= -0.00005f && -dx * (x - b.x) - (z - b.z) * dz >= -0.00005f &&
                    time < query->hit_time) {
                    query->shape_adjusted = 4;
                    query->terrain_group_index = -1;
                    query->hit_type = start_distance > 0.0f ? 1 : 0x11;
                    query->surface = NULL;
                    query->movement_normal = normal;
                    query->hit_time = time;
                    TerrWallInfo = 1;
                    TerrWallTab[0] = ((TERRAIN_WALL_POINT *)WallSplList)[i].material[0];
                    TerrWallTab[1] = ((TERRAIN_WALL_POINT *)WallSplList)[i].material[1];
                    hit = 1.0f;
                }
            }
            const f32 dx = a.x - px;
            const f32 dz = a.z - pz;
            const f32 reach = radius + 0.005f + query->horizontal_movement_length;
            const f32 distance_sq = dx * dx + dz * dz;
            TERRAIN_FLOAT_BITS_PC absolute_2;
            absolute_2.value = (dx);
            absolute_2.bits &= 0x7fffffff;
            TERRAIN_FLOAT_BITS_PC absolute_3;
            absolute_3.value = (dz);
            absolute_3.bits &= 0x7fffffff;

            if (absolute_2.value >= 64.0f || absolute_3.value >= 64.0f || distance_sq >= reach * reach)
                continue;
            NUVEC direction = {mx, 0.0f, mz};
            NuVecNorm(&direction, &direction);
            const f32 along = direction.x * dx + direction.z * dz;
            const f32 padded_radius = radius + 0.0005f;
            bool penetrating = false;
            f32 time = 0.0f;
            if (along < 0.0f) {
                if (distance_sq > padded_radius * padded_radius)
                    continue;
                penetrating = true;
            } else {
                const f32 side_x = a.x - (direction.x * along + px);
                const f32 side_z = a.z - (direction.z * along + pz);
                const f32 side_x_sq = side_x * side_x;
                const f32 side_z_sq = side_z * side_z;
                if (side_x_sq + side_z_sq <= padded_radius * padded_radius) {
                    const f32 distance = along - NuFsqrt((radius * radius - side_x_sq) - side_z_sq);
                    const f32 movement_sq = mx * mx + mz * mz;
                    if (distance < -0.0005f || movement_sq < distance * distance) {
                        if (distance_sq > padded_radius * padded_radius)
                            continue;
                        penetrating = true;
                    } else {
                        const f32 length = NuFsqrt(movement_sq);
                        if (length != 0.0f && distance != 0.0f)
                            time = MAX(distance / length, 0.0f);
                        if (time >= query->hit_time)
                            continue;
                        normal.x = direction.x * distance + px - a.x;
                        normal.z = distance * direction.z + pz - a.z;
                        NuVecNorm(&normal, &normal);
                    }
                } else {
                    if (distance_sq > padded_radius * padded_radius)
                        continue;
                    penetrating = true;
                }
            }
            if (penetrating) {
                normal.x = px - a.x;
                normal.z = pz - a.z;
                NuVecNorm(&normal, &normal);
                time = (NuFsqrt(distance_sq) - radius) - 0.0005f;
            }
            query->hit_type = penetrating ? 0x11 : 1;
            query->hit_time = time;
            query->movement_normal = normal;
            query->shape_adjusted = 4;
            query->surface = NULL;
            query->terrain_group_index = -1;
            TerrWallInfo = 1;
            TerrWallTab[0] = ((TERRAIN_WALL_POINT *)WallSplList)[i].material[0];
            TerrWallTab[1] = ((TERRAIN_WALL_POINT *)WallSplList)[i].material[1];
            hit = 1.0f;
        }
        ((TerrainQuery_s *)TerI)->unclamped_hit_time = ((TerrainQuery_s *)TerI)->hit_time;
        if (((TerrainQuery_s *)TerI)->hit_time < 0.0f)
            ((TerrainQuery_s *)TerI)->hit_time = 0.0f;
        return hit;
    }
}
