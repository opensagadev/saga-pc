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
        while ((entry_var)->header.shape_count > 0) {                                                                  \
            const i16 count = (entry_var)->header.shape_count;                                                         \
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
        while ((entry_var)->header.shape_count > 0) {                                                                  \
            const i16 count = (entry_var)->header.shape_count;                                                         \
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
        while ((entry_var)->header.shape_count > 0) {                                                                  \
            const i16 count = (entry_var)->header.shape_count;                                                         \
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
        while ((entry_var)->header.shape_count > 0) {                                                                  \
            const i16 count = (entry_var)->header.shape_count;                                                         \
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
        while ((entry_var)->header.shape_count > 0) {                                                                  \
            const i16 count = (entry_var)->header.shape_count;                                                         \
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
        while ((entry_var)->header.shape_count > 0) {                                                                  \
            const i16 count = (entry_var)->header.shape_count;                                                         \
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
        while ((entry_var)->header.shape_count > 0) {                                                                  \
            const i16 count = (entry_var)->header.shape_count;                                                         \
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
        while ((entry_var)->header.shape_count > 0) {                                                                  \
            const i16 count = (entry_var)->header.shape_count;                                                         \
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
        TERRAIN_INDEX_LEVEL_PC *cache;
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

        if (hit_index < 0) {
            cache = &TERRAIN_NS_SET->index_levels[oldest_index];
            max_x = saved_position.x + 1.0f;
            max_z = saved_position.z + 1.0f;
            group_header = cache->scan_words;
            cursor = group_header + 1;
            min_x = saved_position.x - 1.0f;
            limit = cursor + 508;
            min_z = saved_position.z - 1.0f;
            shape_count = 0;
            for (i32 cell_index = 0; cell_index < TERRAIN_NS_SET->used_cell_count; ++cell_index) {
                TERRAIN_CELL_PC *cell = &TERRAIN_NS_SET->cells[cell_index];
                if (!(max_x >= cell->min_x && max_z >= cell->min_z && cell->max_x >= min_x && cell->max_z >= min_z)) {
                    continue;
                }
                i16 *group_indices = TERRAIN_NS_SET->group_indices + cell->first_group;
                for (i32 cell_group = 0; cell_group < cell->group_count; ++cell_group) {
                    TERRAIN_GROUP_PC *group = &TERRAIN_NS_SET->groups[group_indices[cell_group]];
                    if (!TERRAIN_NS_GROUP_OVERLAP(min_x, min_z, max_x, max_z, group) || group->chunk_type == -1) {
                        continue;
                    }
                    TERRAIN_NS_LOCAL_BOUNDS(group);
                    if (group->scene_index < 0) {
                        TerrainSkinAllocate(group);
                    }
                    TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
                    while (batch->marker >= 0) {
                        TERRAIN_SHAPE *shape = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                        const i32 count = batch->shape_count;
                        if (TERRAIN_NS_BATCH_OVERLAP(batch)) {
                            for (i32 remaining = count; remaining > 0; --remaining, ++shape) {
                                if (TERRAIN_NS_SHAPE_OVERLAP(shape) && cursor < limit) {
                                    (cursor++)->shape = shape;
                                    ++shape_count;
                                }
                            }
                        } else {
                            shape += count;
                        }
                        batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shape);
                    }
                    TERRAIN_NS_FINISH_GROUP(group_indices[cell_group]);
                }
            }
            cache->cache_age = 8;
            cache->center_x = position->x;
            cache->center_z = position->z;
            group_header->header.shape_count = 0;
            group_header->header.terrain_group = 0;
            group_header = static_cast<TERRAIN_SHADOW_QUERY_VIEW_PC *>(TerI)->scan_words;
            cursor = group_header + 1;
            limit = cursor + 508;
            max_x = (f32)(saved_position.x + TerrainScanMargin10Pc);
            max_z = (f32)(saved_position.z + TerrainScanMargin10Pc);
            min_x = (f32)(saved_position.x - TerrainScanMargin10Pc);
            min_z = (f32)(saved_position.z - TerrainScanMargin10Pc);
            shape_count = 0;
            entry = cache->scan_words;
            TERRAIN_NS_CACHED_SHAPES(entry);
        } else {
            cache = &TERRAIN_NS_SET->index_levels[hit_index];
            group_header = static_cast<TERRAIN_SHADOW_QUERY_VIEW_PC *>(TerI)->scan_words;
            max_x = (f32)(saved_position.x + TerrainScanMargin10Pc);
            max_z = (f32)(saved_position.z + TerrainScanMargin10Pc);
            cache->cache_age = 8;
            min_x = (f32)(saved_position.x - TerrainScanMargin10Pc);
            min_z = (f32)(saved_position.z - TerrainScanMargin10Pc);
            cursor = group_header + 1;
            limit = cursor + 508;
            shape_count = 0;
            entry = cache->scan_words;
            TERRAIN_NS_CACHED_SHAPES(entry);
        }

        min_x = (f32)(min_x - NuTimeBarTop);
        max_x = (f32)(max_x + NuTimeBarTop);
        min_z = (f32)(min_z - NuTimeBarTop);
        max_z = (f32)(max_z + NuTimeBarTop);
        if (scan_platforms != 0) {
            i16 *platform_groups = TERRAIN_NS_SET->group_indices + TERRAIN_NS_SET->platform_cell.first_group;
            for (i32 platform_index = 0; platform_index < TERRAIN_NS_SET->platform_cell.group_count;
                 ++platform_index, ++platform_groups) {
                TERRAIN_GROUP_PC *group = &TERRAIN_NS_SET->groups[*platform_groups];
                TERRAIN_PLATFORM_PC *platform = &TERRAIN_NS_PLATFORM(group);
                if (platform->scene_transform != NULL) {
                    TERRAIN_SCENE_STATE_VIEW_PC *scene_state =
                        static_cast<TERRAIN_SCENE_STATE_VIEW_PC *>(platform->scene_transform);
                    if ((platform->flags & TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED) != 0) {
                        const u32 state_flags = scene_state->flags;
                        if ((state_flags & 2) == 0) {
                            continue;
                        }
                    } else if ((scene_state->low_flags & 1) == 0) {
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
        for (i32 remaining = count; remaining > 0; --remaining, ++shapes) {                                            \
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
        group_header = reinterpret_cast<TERRAIN_SHADOW_QUERY_VIEW_PC *>(query)->scan_words;
        cursor = group_header + 1;
        limit = cursor + 508;
        platinrange = 0;
        query->scan_group_index = -1;
        shape_count = 0;
        switch (query->scan_result) {
            default: {
                const f32 movement_sq = query->movement.x * query->movement.x + query->movement.y * query->movement.y +
                                        query->movement.z * query->movement.z;
                const f32 reach = (f32)(TerrainScanMargin02Pc + query->collision_radius_sq + movement_sq);
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
                    min_x = (f32)(query->position.x - TerrainScanMargin02Pc - query->collision_radius);
                    max_x =
                        (f32)(query->movement.x + query->position.x + TerrainScanMargin02Pc + query->collision_radius);
                } else {
                    min_x =
                        (f32)(query->movement.x + query->position.x - TerrainScanMargin02Pc - query->collision_radius);
                    max_x = (f32)(query->position.x + TerrainScanMargin02Pc + query->collision_radius);
                }
                if (query->movement.y > 0.0f) {
                    min_y = (f32)(query->position.y - TerrainScanMargin02Pc - query->collision_radius);
                    max_y =
                        (f32)(query->movement.y + query->position.y + TerrainScanMargin02Pc + query->collision_radius);
                } else {
                    min_y =
                        (f32)(query->movement.y + query->position.y - TerrainScanMargin02Pc - query->collision_radius);
                    max_y = (f32)(query->position.y + TerrainScanMargin02Pc + query->collision_radius);
                }
                if (query->movement.z > 0.0f) {
                    min_z = (f32)(query->position.z - TerrainScanMargin02Pc - query->collision_radius);
                    max_z =
                        (f32)(query->movement.z + query->position.z + TerrainScanMargin02Pc + query->collision_radius);
                } else {
                    min_z =
                        (f32)(query->movement.z + query->position.z - TerrainScanMargin02Pc - query->collision_radius);
                    max_z = (f32)(query->position.z + TerrainScanMargin02Pc + query->collision_radius);
                }
                extent_sq = (min_x - max_x) * (min_x - max_x) + (min_y - max_y) * (min_y - max_y) +
                            (min_z - max_z) * (min_z - max_z);
                scan_radius = NuFsqrt((f32)(extent_sq + (f64)1.02f));
                break;
            }
        }
        while (handle[0] > 0) {
            const i32 count = handle[0];
            const i32 group_index = handle[1];
            TERRAIN_SCAN_WORD_PC *shapes = reinterpret_cast<TERRAIN_SCAN_WORD_PC *>(handle) + 1;
            TERRAIN_GROUP_PC *group = &TERRAIN_STH_SET->groups[group_index];
            f32 local_min_x, local_max_x, local_min_z, local_max_z, local_min_y, local_max_y;
            if (group->chunk_type == 1 && TERRAIN_STH_PLATFORM(group).scene_object != NULL) {
                TERRAIN_PLATFORM_PC *platform = &TERRAIN_STH_PLATFORM(group);
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
                    shapes += count;
                } else if ((platform->flags & TERRAIN_PLATFORM_FLAG_ROTATING) == 0) {
                    if (TERRAIN_STH_GROUP_OVERLAP(local_min_x, local_min_y, local_min_z, local_max_x, local_max_y,
                                                  local_max_z, group)) {
                        TERRAIN_STH_FILTER_SHAPES();
                        TERRAIN_STH_FINISH_GROUP(group_index);
                    } else {
                        shapes += count;
                    }
                } else {
                    const f64 dy =
                        ((min_y + max_y) * NuQFntHalfScale - group->origin.y) * TERRAIN_STH_QUERY->inverse_object_scale;
                    const f64 dx = (min_x + max_x) * NuQFntHalfScale - group->origin.x;
                    const f64 dz = (min_z + max_z) * NuQFntHalfScale - group->origin.z;
                    const f32 distance_sq = dx * dx + dy * dy + dz * dz;
                    if (distance_sq < (group->radius + scan_radius) * (group->radius + scan_radius)) {
                        for (i32 remaining = count; remaining > 0; --remaining, ++shapes) {
                            TERRAIN_SHAPE *shape = shapes->shape;
                            if (cursor < limit && TERRAIN_STH_MATERIAL_OK(shape) &&
                                TERRAIN_STH_VERTICES_OVERLAP(shape)) {
                                (cursor++)->shape = shape;
                                ++shape_count;
                            }
                        }
                        TERRAIN_STH_FINISH_GROUP(group_index);
                    } else {
                        shapes += count;
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
                shapes += count;
            }
            handle = reinterpret_cast<i16 *>(shapes);
        }
        group_header->header.shape_count = 0;
        group_header->header.terrain_group = 0;
        WallSplCount = 0;
        wall_count = 0;
        walls = reinterpret_cast<TERRAIN_WALL_POINT **>(handle) + 1;
        min_x = (f32)(min_x - TerrainScanMargin02Pc);
        min_z = (f32)(min_z - TerrainScanMargin02Pc);
        max_x = (f32)(max_x + TerrainScanMargin02Pc);
        max_z = (f32)(max_z + TerrainScanMargin02Pc);
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
        group_header = reinterpret_cast<TERRAIN_SHADOW_QUERY_VIEW_PC *>(query)->scan_words;
        cursor = group_header + 1;
        limit = cursor + 508;
        shape_count = 0;
        platinrange = 0;
        query->scan_group_index = -1;
        switch (query->scan_result) {
            default: {
                const f32 movement_sq = query->movement.x * query->movement.x + query->movement.y * query->movement.y +
                                        query->movement.z * query->movement.z;
                const f32 reach = (f32)(TerrainScanMargin02Pc + query->collision_radius_sq + movement_sq);
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
                    min_x = (f32)(query->position.x - TerrainScanMargin02Pc - query->collision_radius);
                    max_x =
                        (f32)(query->movement.x + query->position.x + TerrainScanMargin02Pc + query->collision_radius);
                } else {
                    min_x =
                        (f32)(query->movement.x + query->position.x - TerrainScanMargin02Pc - query->collision_radius);
                    max_x = (f32)(query->position.x + TerrainScanMargin02Pc + query->collision_radius);
                }
                if (query->movement.y > 0.0f) {
                    min_y = (f32)(query->position.y - TerrainScanMargin02Pc - query->collision_radius);
                    max_y =
                        (f32)(query->movement.y + query->position.y + TerrainScanMargin02Pc + query->collision_radius);
                } else {
                    min_y =
                        (f32)(query->movement.y + query->position.y - TerrainScanMargin02Pc - query->collision_radius);
                    max_y = (f32)(query->position.y + TerrainScanMargin02Pc + query->collision_radius);
                }
                if (query->movement.z > 0.0f) {
                    min_z = (f32)(query->position.z - TerrainScanMargin02Pc - query->collision_radius);
                    max_z =
                        (f32)(query->movement.z + query->position.z + TerrainScanMargin02Pc + query->collision_radius);
                } else {
                    min_z =
                        (f32)(query->movement.z + query->position.z - TerrainScanMargin02Pc - query->collision_radius);
                    max_z = (f32)(query->position.z + TerrainScanMargin02Pc + query->collision_radius);
                }
                extent_sq = (min_x - max_x) * (min_x - max_x) + (min_y - max_y) * (min_y - max_y) +
                            (min_z - max_z) * (min_z - max_z);
                scan_radius = NuFsqrt((f32)(extent_sq + (query->collision_radius + (f64)1.02f)));
                break;
            }
        }
        query = TERRAIN_STH_QUERY;
        query->scan_list = group_header;
        min_x = (f32)(min_x - NuTimeBarTop);
        max_x = (f32)(max_x + NuTimeBarTop);
        min_y = (f32)(min_y - NuTimeBarTop);
        max_y = (f32)(max_y + NuTimeBarTop);
        min_z = (f32)(min_z - NuTimeBarTop);
        max_z = (f32)(max_z + NuTimeBarTop);
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
        f32 radius = ((TerrainQuery_s *)TerI)->collision_radius;
        ((TerrainQuery_s *)TerI)->hit_type = 0;
        ((TerrainQuery_s *)TerI)->hit_time = 999.9f;
        ((TerrainQuery_s *)TerI)->horizontal_movement_length =
            NuFsqrt(((TerrainQuery_s *)TerI)->movement.x * ((TerrainQuery_s *)TerI)->movement.x +
                    ((TerrainQuery_s *)TerI)->movement.z * ((TerrainQuery_s *)TerI)->movement.z);
        NUVEC *origin = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[group_index].origin;
        ((TerrainQuery_s *)TerI)->local_start.x = ((TerrainQuery_s *)TerI)->position.x - origin->x;
        ((TerrainQuery_s *)TerI)->local_start.y = ((TerrainQuery_s *)TerI)->position.y - origin->y;
        ((TerrainQuery_s *)TerI)->local_start.z = ((TerrainQuery_s *)TerI)->position.z - origin->z;
        ((TerrainQuery_s *)TerI)->local_end.x =
            (((TerrainQuery_s *)TerI)->position.x + ((TerrainQuery_s *)TerI)->movement.x) - origin->x;
        ((TerrainQuery_s *)TerI)->local_end.y =
            (((TerrainQuery_s *)TerI)->position.y + ((TerrainQuery_s *)TerI)->movement.y) - origin->y;
        ((TerrainQuery_s *)TerI)->local_end.z =
            (((TerrainQuery_s *)TerI)->position.z + ((TerrainQuery_s *)TerI)->movement.z) - origin->z;
        f32 primary_end = (((((TerrainQuery_s *)TerI)->local_end.x - surface->vertices[0].x) * surface->normals[0].x +
                            (((TerrainQuery_s *)TerI)->local_end.y - surface->vertices[0].y) * surface->normals[0].y) +
                           (((TerrainQuery_s *)TerI)->local_end.z - surface->vertices[0].z) * surface->normals[0].z) -
                          radius - ((TerrainQuery_s *)TerI)->compare_epsilon;
        f32 primary_start = 0.0f;
        bool test = false;
        if (primary_end < 0.0f) {
            primary_start =
                ((((TerrainQuery_s *)TerI)->local_start.x - surface->vertices[0].x) * surface->normals[0].x +
                 (((TerrainQuery_s *)TerI)->local_start.y - surface->vertices[0].y) * surface->normals[0].y) +
                (((TerrainQuery_s *)TerI)->local_start.z - surface->vertices[0].z) * surface->normals[0].z - radius;
            test = primary_start > -radius;
        }
        f32 secondary_start = 0.0f;
        f32 secondary_end = 0.0f;
        if (surface->normals[1].y < 65536.0f) {
            secondary_end =
                (((((TerrainQuery_s *)TerI)->local_end.x - surface->vertices[3].x) * surface->normals[1].x +
                  (((TerrainQuery_s *)TerI)->local_end.y - surface->vertices[3].y) * surface->normals[1].y) +
                 (((TerrainQuery_s *)TerI)->local_end.z - surface->vertices[3].z) * surface->normals[1].z) -
                radius - ((TerrainQuery_s *)TerI)->compare_epsilon;
            if (secondary_end < 0.0f) {
                secondary_start =
                    ((((TerrainQuery_s *)TerI)->local_start.x - surface->vertices[3].x) * surface->normals[1].x +
                     (((TerrainQuery_s *)TerI)->local_start.y - surface->vertices[3].y) * surface->normals[1].y) +
                    (((TerrainQuery_s *)TerI)->local_start.z - surface->vertices[3].z) * surface->normals[1].z - radius;
                if (secondary_start > -radius)
                    test = true;
            }
        }
        if (test && HitPoly(primary_start, primary_end, secondary_start, secondary_end, surface)) {
            ((TerrainQuery_s *)TerI)->terrain_group_index = group_index;
            return 1;
        }
        return 0;
    }
}

STATIC_ASSERT(offsetof(TerrainQuery_s, start_position) == 4);
STATIC_ASSERT(offsetof(TerrainQuery_s, start_movement) == 0x10);
STATIC_ASSERT(offsetof(TerrainQuery_s, object_index) == 0x36);
STATIC_ASSERT(offsetof(TerrainQuery_s, hit_flags) == 0x40);
STATIC_ASSERT(offsetof(TerrainQuery_s, collision_radius_sq) == 0x54);
STATIC_ASSERT(offsetof(TerrainQuery_s, inverse_collision_radius) == 0x58);
STATIC_ASSERT(offsetof(TerrainQuery_s, inverse_object_scale_sq) == 0x68);
STATIC_ASSERT(offsetof(TerrainQuery_s, hit_type) == 0x88);
STATIC_ASSERT(offsetof(TerrainQuery_s, hit_time) == 0x98);
STATIC_ASSERT(offsetof(TerrainQuery_s, separation_epsilon) == 0xa0);
STATIC_ASSERT(offsetof(TerrainQuery_s, compare_epsilon) == 0xa4);
STATIC_ASSERT(offsetof(TerrainQuery_s, movement_normal) == 0xa8);
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
    TerI = NuScratchAlloc32(sizeof(TerrainQuery_s));
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(TerI);
    query->object_scale = scale_y;
    query->object_scale_sq = scale_y * scale_y;
    TerrOverRideScan = NULL;
    query->inverse_object_scale = scale_y == 0.0f ? 1.0f : 1.0f / scale_y;
    query->inverse_object_scale_sq = query->inverse_object_scale * query->inverse_object_scale;
    query->collision_radius = radius;
    query->inverse_collision_radius = radius == 0.0f ? 1.0f : 1.0f / radius;
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
    query->movement.y *= query->inverse_object_scale;
    DerotateMovementVector();
    HitTerrain();
    query = static_cast<TerrainQuery_s *>(TerI);
    if (query->hit_type != 0) {
        RayImpact(movement);
        TerrainImpactNorm();
        query = static_cast<TerrainQuery_s *>(TerI);
        GetSurfaceInfoSurfaceNormalPc = query->movement_normal;
        movement->y *= query->object_scale;
    }
    NuScratchRelease();
    return static_cast<TerrainQuery_s *>(TerI)->hit_type;
}

extern "C" i32 __cdecl TerrainPlatId(void) {
    return plathitid;
}

STATIC_ASSERT(offsetof(TerrainQuery_s, surface) == 0x90);
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
STATIC_ASSERT(offsetof(TerrainQuery_s, movement_normal) == 0xa8);
extern "C" void __cdecl NewTerrHitInfo(u8 *info) {
    info[0] = TerrPolyInfo.material[0];
    info[1] = TerrPolyInfo.material[1];
    info[2] = TerrPolyInfo.flags;
    info[3] = TerrPolyInfo.normal_flags;
}
extern "C" void __cdecl NewRayCastGetImpactNormal(NUVEC *normal) {
    if (TerI != NULL)
        *normal = static_cast<TerrainQuery_s *>(TerI)->movement_normal;
}

STATIC_ASSERT(offsetof(TerrainQuery_s, shape_adjusted) == 0x35);
STATIC_ASSERT(offsetof(TerrainQuery_s, unclamped_hit_time) == 0x9c);
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
    TerI = NuScratchAlloc32(sizeof(TerrainQuery_s));
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(TerI);
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    TerrOverRideScan = NULL;
    query->inverse_object_scale = 1.0f;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    query->inverse_collision_radius = radius == 0.0f ? 0.0f : 1.0f / radius;
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
    TerI = NuScratchAlloc32(sizeof(TerrainQuery_s));
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(TerI);
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    TerrOverRideScan = NULL;
    query->inverse_object_scale = 1.0f;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    query->inverse_collision_radius = radius == 0.0f ? 0.0f : 1.0f / radius;
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
    TerI = NuScratchAlloc32(sizeof(TerrainQuery_s));
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(TerI);
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    TerrOverRideScan = NULL;
    query->inverse_object_scale = 1.0f;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    query->inverse_collision_radius = radius == 0.0f ? 0.0f : 1.0f / radius;
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
    TerI = NuScratchAlloc32(sizeof(TerrainQuery_s));
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(TerI);
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    query->inverse_object_scale = 1.0f;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    query->inverse_collision_radius = radius == 0.0f ? 0.0f : 1.0f / radius;
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
        min_x = (f32)(position->x - TerrainScanMargin02Pc - radius);
        max_x = (f32)(position->x + movement->x + TerrainScanMargin02Pc + radius);
    } else {
        min_x = (f32)(position->x + movement->x - TerrainScanMargin02Pc - radius);
        max_x = (f32)(position->x + TerrainScanMargin02Pc + radius);
    }
    if (movement->y > 0.0f) {
        min_y = (f32)(position->y - TerrainScanMargin02Pc - radius);
        max_y = (f32)(position->y + movement->y + TerrainScanMargin02Pc + radius);
    } else {
        min_y = (f32)(position->y + movement->y - TerrainScanMargin02Pc - radius);
        max_y = (f32)(position->y + TerrainScanMargin02Pc + radius);
    }
    if (movement->z > 0.0f) {
        min_z = (f32)(position->z - TerrainScanMargin02Pc - radius);
        max_z = (f32)(position->z + movement->z + TerrainScanMargin02Pc + radius);
    } else {
        min_z = (f32)(position->z + movement->z - TerrainScanMargin02Pc - radius);
        max_z = (f32)(position->z + TerrainScanMargin02Pc + radius);
    }
    shape_count = 0;
    {
        const f32 extent_sq =
            (min_x - max_x) * (min_x - max_x) + (min_y - max_y) * (min_y - max_y) + (min_z - max_z) * (min_z - max_z);
        // Retail uses the squared extent plus the margin directly as the sphere radius.
        scan_radius = (f32)(extent_sq + NuTimeBarTop);
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
    min_x = (f32)(min_x - TerrainScanMargin02Pc);
    min_z = (f32)(min_z - TerrainScanMargin02Pc);
    max_x = (f32)(max_x + TerrainScanMargin02Pc);
    max_z = (f32)(max_z + TerrainScanMargin02Pc);
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

STATIC_ASSERT(offsetof(TERRAIN_SCENE_STATE_VIEW_PC, flags) == 0);
STATIC_ASSERT(offsetof(TERRAIN_SCENE_STATE_VIEW_PC, low_flags) == 0);
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
        scan->min_x = (f32)(position->x - TerrainScanMargin02Pc - radius);
        scan->max_x = (f32)(position->x + movement->x + TerrainScanMargin02Pc + radius);
    } else {
        scan->min_x = (f32)(position->x + movement->x - TerrainScanMargin02Pc - radius);
        scan->max_x = (f32)(position->x + TerrainScanMargin02Pc + radius);
    }
    if (movement->y > 0.0f) {
        scan->min_y = (f32)(position->y - TerrainScanMargin02Pc - radius);
        scan->max_y = (f32)(position->y + movement->y + TerrainScanMargin02Pc + radius);
    } else {
        scan->min_y = (f32)(position->y + movement->y - TerrainScanMargin02Pc - radius);
        scan->max_y = (f32)(position->y + TerrainScanMargin02Pc + radius);
    }
    if (movement->z > 0.0f) {
        scan->min_z = (f32)(position->z - TerrainScanMargin02Pc - radius);
        scan->max_z = (f32)(position->z + movement->z + TerrainScanMargin02Pc + radius);
    } else {
        scan->min_z = (f32)(position->z + movement->z - TerrainScanMargin02Pc - radius);
        scan->max_z = (f32)(position->z + TerrainScanMargin02Pc + radius);
    }
    {
        const f32 dy = scan->min_y - scan->max_y;
        const f32 dx = scan->min_x - scan->max_x;
        const f32 dz = scan->min_z - scan->max_z;
        extent_sq = dx * dx + dy * dy + dz * dz;
    }
    scan_radius = NuFsqrt((f32)(extent_sq + NuTimeBarTop));
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
                    const i32 count = batch->shape_count;
                    TERRAIN_SHAPE *source = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                    if (local_max_x >= batch->min_x && batch->max_x > local_min_x && local_max_z >= batch->min_z &&
                        batch->max_z > local_min_z) {
                        for (i32 remaining = count; remaining > 0; --remaining, ++source) {
                            if (TERRAIN_NSF_SHAPE_OVERLAP(source) && cursor < limit &&
                                TERRAIN_NSF_MATERIAL_OK(source)) {
                                (cursor++)->shape = source;
                                ++shape_count;
                            }
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
    scan->min_x = (f32)(scan->min_x - NuTimeBarTop);
    scan->max_x = (f32)(scan->max_x + NuTimeBarTop);
    scan->min_y = (f32)(scan->min_y - NuTimeBarTop);
    scan->max_y = (f32)(scan->max_y + NuTimeBarTop);
    scan->min_z = (f32)(scan->min_z - NuTimeBarTop);
    scan->max_z = (f32)(scan->max_z + NuTimeBarTop);
    if (scan_type != 0) {
        i16 *indices = TERRAIN_NSF_SET->group_indices + TERRAIN_NSF_SET->platform_cell.first_group;
        for (i32 i = 0; i < TERRAIN_NSF_SET->platform_cell.group_count; ++i, ++indices) {
            TERRAIN_GROUP_PC *group = &TERRAIN_NSF_SET->groups[*indices];
            TERRAIN_PLATFORM_PC *platform = &TERRAIN_NSF_PLATFORM(group);
            TERRAIN_SCENE_STATE_VIEW_PC *scene_state =
                static_cast<TERRAIN_SCENE_STATE_VIEW_PC *>(platform->scene_transform);
            if (scene_state == NULL ||
                ((platform->flags & 4) != 0 ? (scene_state->flags & 2) != 0 : (scene_state->low_flags & 1) != 0)) {
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
                            const i32 count = batch->shape_count;
                            TERRAIN_SHAPE *source = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                            if (local_max_x >= batch->min_x && batch->max_x > local_min_x &&
                                local_max_z >= batch->min_z && batch->max_z > local_min_z) {
                                for (i32 remaining = count; remaining > 0; --remaining, ++source) {
                                    if (!TERRAIN_NSF_SHAPE_OVERLAP(source)) {
                                        continue;
                                    }
                                    platinrange = 1;
                                    if (cursor < limit && TERRAIN_NSF_MATERIAL_OK(source)) {
                                        (cursor++)->shape = source;
                                        ++shape_count;
                                    }
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
    scan->min_x = (f32)(scan->min_x - TerrainScanMargin02Pc);
    scan->min_z = (f32)(scan->min_z - TerrainScanMargin02Pc);
    scan->max_x = (f32)(scan->max_x + TerrainScanMargin02Pc);
    scan->max_z = (f32)(scan->max_z + TerrainScanMargin02Pc);
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
    TerI = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    static_cast<TerrainQuery_s *>(TerI)->object_scale = 1.0f;
    static_cast<TerrainQuery_s *>(TerI)->object_scale_sq = 1.0f;
    static_cast<TerrainQuery_s *>(TerI)->inverse_object_scale = 1.0f;
    static_cast<TerrainQuery_s *>(TerI)->inverse_object_scale_sq = 1.0f;
    static_cast<TerrainQuery_s *>(TerI)->collision_radius = radius;
    static_cast<TerrainQuery_s *>(TerI)->inverse_collision_radius = radius == 0.0f ? 0.0f : 1.0f / radius;
    static_cast<TerrainQuery_s *>(TerI)->collision_radius_sq = radius * radius;
    static_cast<TerrainQuery_s *>(TerI)->start_position.x = static_cast<TerrainQuery_s *>(TerI)->position.x =
        position->x;
    static_cast<TerrainQuery_s *>(TerI)->start_position.y = static_cast<TerrainQuery_s *>(TerI)->position.y =
        position->y;
    static_cast<TerrainQuery_s *>(TerI)->start_position.z = static_cast<TerrainQuery_s *>(TerI)->position.z =
        position->z;
    static_cast<TerrainQuery_s *>(TerI)->movement = *movement;
    static_cast<TerrainQuery_s *>(TerI)->start_movement = static_cast<TerrainQuery_s *>(TerI)->movement;
    static_cast<TerrainQuery_s *>(TerI)->object_index = -1;
    static_cast<TerrainQuery_s *>(TerI)->hit_flags = NULL;
    static_cast<TerrainQuery_s *>(TerI)->scan_result = 1;
    i16 *handle;
    if (subset != NULL)
        handle = NewScanHandelSubsetPc(subset, position, movement, radius, 0xff);
    else
        handle = NewScanHandelFullPc(position, movement, radius, scan_type, 0xff);
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
    if (CurrentTerrain == NULL || handle == NULL)
        return 0;
    TerI = NuScratchAlloc32(sizeof(TerrainQuery_s));
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(TerI);
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    TerrOverRideScan = NULL;
    query->inverse_object_scale = 1.0f;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    query->inverse_collision_radius = radius == 0.0f ? 0.0f : TerrainDivideNonzeroPC(1.0f, radius);
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
    TerI = NuScratchAlloc32(sizeof(TerrainQuery_s));
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(TerI);
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    TerrOverRideScan = NULL;
    query->inverse_object_scale = 1.0f;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    query->inverse_collision_radius = radius == 0.0f ? 0.0f : 1.0f / radius;
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
    query->movement.x = 0.0f;
    query->movement.z = 0.0f;
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
    if (CurrentTerrain == NULL || TerrainShadowStatusPc == -1)
        return 0;
    TerI = NuScratchAlloc32(sizeof(TerrainQuery_s));
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(TerI);
    query->object_scale = 1.0f;
    query->object_scale_sq = 1.0f;
    query->inverse_object_scale = 1.0f;
    query->inverse_object_scale_sq = 1.0f;
    query->collision_radius = radius;
    query->inverse_collision_radius = radius == 0.0f ? 0.0f : 1.0f / radius;
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
        case 0:
        case 17:
        case 18:
        case 19:
        case 20:
            *result = query->position;
            break;
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
    }
    NuScratchRelease();
    return static_cast<TerrainQuery_s *>(TerI)->hit_type;
}

STATIC_ASSERT(offsetof(TerrainQuery_s, movement_pitch) == 0x44);
STATIC_ASSERT(offsetof(TerrainQuery_s, movement_yaw) == 0x48);
STATIC_ASSERT(offsetof(TerrainQuery_s, transformed_vertices) == 0xfc);
STATIC_ASSERT(offsetof(TerrainQuery_s, hit_type) == 0x88);
STATIC_ASSERT(offsetof(TerrainQuery_s, hit_edge) == 0x89);
STATIC_ASSERT(offsetof(TerrainQuery_s, terrain_group_index) == 0x6c);
STATIC_ASSERT(offsetof(TerrainQuery_s, surface) == 0x90);
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
    const f32 pitch = query->movement_pitch;
    const f32 sin_pitch = NuTrigTable[(static_cast<i32>(pitch) >> 1) & (NUTRIGTABLE_COUNT - 1)];
    const f32 cos_pitch =
        NuTrigTable[(static_cast<i32>(static_cast<f64>(pitch) + 16384.0) >> 1) & (NUTRIGTABLE_COUNT - 1)];
    const f32 yaw = query->movement_yaw;
    const f32 sin_yaw = NuTrigTable[(static_cast<i32>(yaw) >> 1) & (NUTRIGTABLE_COUNT - 1)];
    const f32 cos_yaw = NuTrigTable[(static_cast<i32>(static_cast<f64>(yaw) + 16384.0) >> 1) & (NUTRIGTABLE_COUNT - 1)];

    for (i32 vertex_index = 0; vertex_index < 3; ++vertex_index) {
        const NUVEC &source = surface->vertices[vertex_index];
        NUVEC &destination = query->transformed_vertices[vertex_index];
        const f32 rotated_z = source.y * sin_pitch + source.z * cos_pitch;
        destination.y = source.y * cos_pitch - source.z * sin_pitch;
        destination.z = rotated_z * cos_yaw - source.x * sin_yaw;
        destination.x = rotated_z * sin_yaw + source.x * cos_yaw;
    }
    if (65536.0f > surface->normals[1].y) {
        const NUVEC &source = surface->vertices[3];
        NUVEC &destination = query->transformed_vertices[3];
        const f32 rotated_z = source.y * sin_pitch + source.z * cos_pitch;
        destination.y = source.y * cos_pitch - source.z * sin_pitch;
        destination.z = rotated_z * cos_yaw - source.x * sin_yaw;
        destination.x = rotated_z * sin_yaw + source.x * cos_yaw;
    }
}

extern "C" void FullDeflectSizePc(NUVEC *normal, NUVEC *movement, NUVEC *result) {
    f32 deflection = (-movement->y * normal->y - movement->x * normal->x) - movement->z * normal->z;
    f32 original_length = NuFsqrt(movement->x * movement->x + movement->y * movement->y + movement->z * movement->z);
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
    f32 deflection = (-movement->y * normal->y - movement->x * normal->x) - movement->z * normal->z;
    f32 original_length = NuFsqrt(movement->x * movement->x + movement->y * movement->y + movement->z * movement->z);
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
    if (static_cast<TERRAIN_SET_PC *>(CurrentTerrain) == NULL)
        return;
    TERRAIN_SET_PC *terrain = static_cast<TERRAIN_SET_PC *>(CurrentTerrain);
    TERRAIN_TRACK_SLOT_PC *track = NULL;
    for (i32 index = 0; index < 64; ++index) {
        if (terrain->track_slots[index].id == id) {
            track = &terrain->track_slots[index];
            break;
        }
    }
    CurTrackInfo = track;
    if (CurTrackInfo == NULL || CurTrackInfo->wall_contact_state == 0)
        return;
    if (speed != 0.0f) {
        NUVEC slide = {0.0f, 0.0f, speed};
        NuVecRotateY(&slide, &slide, angle);
        FullDeflect(&CurTrackInfo->impact_normal, &slide, &slide);
        slide.x += (speed * CurTrackInfo->impact_normal.x) * static_cast<f64>(0.4f);
        slide.z += (speed * CurTrackInfo->impact_normal.z) * static_cast<f64>(0.4f);
        movement->z += slide.z;
        movement->x += slide.x;
    }
    if (movement->y > 0.0f)
        movement->y *= upward_scale;
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
    TerrShape = shape;
    const f32 shape_angle = shape->angle;
    TerrShapeAdjCnt = 2;

    NUVEC axis;
    axis.x = NuTrigTable[static_cast<i32>(static_cast<f64>(shape_angle) + 16384.0) >> 1 & 0x7fff];
    axis.y = 0.0f;
    axis.z = -NuTrigTable[static_cast<i32>(shape_angle) >> 1 & 0x7fff];
    TerrainSideClamp(&axis, position);
}
