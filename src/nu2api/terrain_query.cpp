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
    void NewScan(NUVEC *position, i32 terrain_mask, i32 scan_platforms) {
        NUVEC saved_position = *position;
        position = &saved_position;
        i32 cache_index = 0;
        i32 oldest_age = 0x7fffffff;
        bool cache_hit = false;
        for (i32 i = 0; i < 16; ++i) {
            TERRAIN_INDEX_LEVEL_PC &candidate = ((TERRAIN_SET_PC *)CurrentTerrain)->index_levels[i];
            const i32 age = candidate.cache_age;
            if (age > 0) {
                const f64 dx = ((f64)position->x + 1.0f) - candidate.center_x;
                const f64 dz = ((f64)position->z + 1.0f) - candidate.center_z;
                if (dx > 0.0f && dx < 2.0f && dz > 0.0f && dz < 2.0f) {
                    cache_index = i;
                    cache_hit = true;
                    break;
                }
            }
            if (age < oldest_age) {
                oldest_age = age;
                cache_index = i;
            }
        }

        TERRAIN_INDEX_LEVEL_PC &cache = ((TERRAIN_SET_PC *)CurrentTerrain)->index_levels[cache_index];
        const bool fill_cache = !cache_hit;
        ShadowScanWriter writer;
        writer.group_header =
            fill_cache ? reinterpret_cast<u8 *>(cache.scan_words) : ((TerrainQuery_s *)TerI)->scan_list_storage;
        writer.cursor = reinterpret_cast<TERRAIN_SHAPE **>(writer.group_header + sizeof(TERRAIN_SHAPE *));
        writer.limit = writer.group_header + 0x7f4;
        writer.shape_count = 0;

        const f32 extent = fill_cache ? 1.0f : TerrainScanMargin10Pc;
        f32 min_x = position->x - extent;
        f32 max_x = position->x + extent;
        f32 min_z = position->z - extent;
        f32 max_z = position->z + extent;

        if (!cache_hit) {
            for (i32 cell_index = 0; cell_index < ((TERRAIN_SET_PC *)CurrentTerrain)->used_cell_count; ++cell_index) {
                const TERRAIN_CELL_PC &cell = ((TERRAIN_SET_PC *)CurrentTerrain)->cells[cell_index];
                if (max_x < cell.min_x || cell.max_x < min_x || max_z < cell.min_z || cell.max_z < min_z) {
                    continue;
                }
                const i16 *group_indices = ((TERRAIN_SET_PC *)CurrentTerrain)->group_indices + cell.first_group;
                for (i32 cell_group = 0; cell_group < static_cast<i16>(cell.group_count); ++cell_group) {
                    // Cache geometry independently of the requesting material mask.
                    const i32 group_index = group_indices[cell_group];
                    TERRAIN_GROUP_PC &group = ((TERRAIN_SET_PC *)CurrentTerrain)->groups[group_index];
                    if (group.chunk_type == -1 ||
                        !SHADOW_BOUNDS_OVERLAP(min_x, min_z, max_x, max_z, group.bounds_min, group.bounds_max))
                        continue;
                    const f32 local_min_x = min_x - group.origin.x;
                    const f32 local_max_x = max_x - group.origin.x;
                    const f32 local_min_z = min_z - group.origin.z;
                    const f32 local_max_z = max_z - group.origin.z;
                    if (group.scene_index < 0)
                        TerrainSkinAllocate(&group);
                    TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group.data);
                    while (batch->marker >= 0) {
                        TERRAIN_SHAPE *shape = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                        const i32 count = batch->shape_count;
                        if (local_max_x >= batch->min_x && batch->max_x > local_min_x && local_max_z >= batch->min_z &&
                            batch->max_z > local_min_z) {
                            i32 remaining = count;
                            while (remaining > 0) {
                                if (local_max_x >= shape->min_x && shape->max_x > local_min_x &&
                                    local_max_z >= shape->min_z && shape->max_z > local_min_z &&
                                    reinterpret_cast<u8 *>(writer.cursor + 1) <= writer.limit) {
                                    *writer.cursor++ = shape;
                                    ++writer.shape_count;
                                }
                                ++shape;
                                --remaining;
                            }
                        } else
                            shape += count;
                        batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shape);
                    }
                    SHADOW_FINISH_GROUP(&writer, group_index);
                }
            }
        }

        if (fill_cache) {
            i16 *terminator = reinterpret_cast<i16 *>(writer.group_header);
            terminator[0] = 0;
            terminator[1] = 0;
            cache.center_x = position->x;
            cache.center_z = position->z;
        }

        if (fill_cache || cache_hit) {
            cache.cache_age = 8;
            writer.group_header = ((TerrainQuery_s *)TerI)->scan_list_storage;
            writer.cursor = reinterpret_cast<TERRAIN_SHAPE **>(writer.group_header + sizeof(TERRAIN_SHAPE *));
            writer.limit = writer.group_header + 0x7f4;
            writer.shape_count = 0;
            min_x = position->x - TerrainScanMargin10Pc;
            max_x = position->x + TerrainScanMargin10Pc;
            min_z = position->z - TerrainScanMargin10Pc;
            max_z = position->z + TerrainScanMargin10Pc;

            u8 *entry = reinterpret_cast<u8 *>(cache.scan_words);
            for (;;) {
                const i16 count = reinterpret_cast<i16 *>(entry)[0];
                if (count <= 0) {
                    break;
                }
                const i16 group_index = reinterpret_cast<i16 *>(entry)[1];
                TERRAIN_SHAPE **shapes = reinterpret_cast<TERRAIN_SHAPE **>(entry + sizeof(TERRAIN_SHAPE *));
                entry = reinterpret_cast<u8 *>(shapes + count);
                TERRAIN_GROUP_PC &group = ((TERRAIN_SET_PC *)CurrentTerrain)->groups[group_index];
                if (!SHADOW_BOUNDS_OVERLAP(min_x, min_z, max_x, max_z, group.bounds_min, group.bounds_max) ||
                    group.chunk_type == -1) {
                    continue;
                }

                const f32 local_min_x = min_x - group.origin.x;
                const f32 local_max_x = max_x - group.origin.x;
                const f32 local_min_z = min_z - group.origin.z;
                const f32 local_max_z = max_z - group.origin.z;
                for (i32 i = 0; i < count; ++i) {
                    TERRAIN_SHAPE *shape = shapes[i];
                    if (local_max_x < shape->min_x || shape->max_x <= local_min_x || local_max_z < shape->min_z ||
                        shape->max_z <= local_min_z) {
                        continue;
                    }
                    if (shape->material[1] != 0 && (shape->material[1] & terrain_mask) == 0) {
                        continue;
                    }
                    if (reinterpret_cast<u8 *>(writer.cursor) >= writer.limit) {
                        continue;
                    }
                    *writer.cursor++ = shape;
                    ++writer.shape_count;
                }
                SHADOW_FINISH_GROUP(&writer, group_index);
            }
        }

        if (scan_platforms != 0) {
            min_x -= NuTimeBarTop;
            max_x += NuTimeBarTop;
            min_z -= NuTimeBarTop;
            max_z += NuTimeBarTop;
            // This scan uses the complete platform cell, not NewScanRot's active cache.
            const TERRAIN_CELL_PC &cell = ((TERRAIN_SET_PC *)CurrentTerrain)->cells[TERRAIN_PLATFORM_CELL];
            const i16 *platform_groups = ((TERRAIN_SET_PC *)CurrentTerrain)->group_indices + cell.first_group;
            const i32 platform_count = static_cast<i16>(cell.group_count);

            for (i32 platform_index = 0; platform_index < platform_count; ++platform_index) {
                const i32 group_index = platform_groups[platform_index];
                TERRAIN_GROUP_PC &group = ((TERRAIN_SET_PC *)CurrentTerrain)->groups[group_index];
                TERRAIN_PLATFORM_PC &platform = ((TERRAIN_SET_PC *)CurrentTerrain)->platforms[group.scene_index];
                if (platform.scene_transform != NULL) {
                    const u8 visible_mask = (platform.flags & TERRAIN_PLATFORM_FLAG_DISPLAY_LIST_BACKED) != 0 ? 2 : 1;
                    if ((*static_cast<u8 *>(platform.scene_transform) & visible_mask) == 0) {
                        continue;
                    }
                }

                f32 local_min_x = min_x - group.origin.x;
                f32 local_max_x = max_x - group.origin.x;
                f32 local_min_z = min_z - group.origin.z;
                f32 local_max_z = max_z - group.origin.z;
                NUMTX *matrix = static_cast<NUMTX *>(platform.scene_object);
                if (matrix != NULL) {
                    if (matrix->m30 > platform.previous_matrix.m30)
                        local_max_x = (matrix->m30 - platform.previous_matrix.m30) * 1.5f + max_x - group.origin.x;
                    else
                        local_min_x = (matrix->m30 - platform.previous_matrix.m30) * 1.5f + min_x - group.origin.x;
                    if (matrix->m32 > platform.previous_matrix.m32)
                        local_max_z = (matrix->m32 - platform.previous_matrix.m32) * 1.5f + max_z - group.origin.z;
                    else
                        local_min_z = (matrix->m32 - platform.previous_matrix.m32) * 1.5f + min_z - group.origin.z;
                }
                if (!SHADOW_BOUNDS_OVERLAP(local_min_x, local_min_z, local_max_x, local_max_z, group.bounds_min,
                                           group.bounds_max) ||
                    group.chunk_type == -1) {
                    continue;
                }

                TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group.data);
                while (batch->marker >= 0) {
                    TERRAIN_SHAPE *shapes = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                    if (local_max_x >= batch->min_x && batch->max_x > local_min_x && local_max_z >= batch->min_z &&
                        batch->max_z > local_min_z) {
                        for (i32 shape_index = 0; shape_index < batch->shape_count; ++shape_index) {
                            TERRAIN_SHAPE *shape = &shapes[shape_index];
                            if (local_max_x < shape->min_x || shape->max_x <= local_min_x ||
                                local_max_z < shape->min_z || shape->max_z <= local_min_z) {
                                continue;
                            }
                            if (reinterpret_cast<u8 *>(writer.cursor) >= writer.limit) {
                                continue;
                            }
                            if (shape->material[1] != 0 && (shape->material[1] & terrain_mask) == 0) {
                                continue;
                            }
                            *writer.cursor++ = shape;
                            ++writer.shape_count;
                        }
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shapes + batch->shape_count);
                }
                SHADOW_FINISH_GROUP(&writer, group_index);
            }
        }

        i16 *terminator = reinterpret_cast<i16 *>(writer.group_header);
        terminator[0] = 0;
        terminator[1] = 0;
    }
    void ScanTerrainHandel(i32 terrain_mask, i16 *handle) {
        if (handle == NULL)
            return;
        platinrange = 0;
        TerrainScanWriter writer;
        writer.group_header = ((TerrainQuery_s *)TerI)->scan_list_storage;
        writer.cursor = reinterpret_cast<TERRAIN_SHAPE **>(writer.group_header + sizeof(TERRAIN_SHAPE *));
        writer.limit =
            ((TerrainQuery_s *)TerI)->scan_list_storage + sizeof(((TerrainQuery_s *)TerI)->scan_list_storage) - 12;
        writer.group_shape_count = 0;
        writer.scaled_shape_count = 0;
        ((TerrainQuery_s *)TerI)->scan_group_index = -1;
        TerrainScanBounds bounds;
        f32 scan_radius_sq;
        if (((TerrainQuery_s *)TerI)->scan_result != 1) {
            f32 movement_sq = ((TerrainQuery_s *)TerI)->movement.x * ((TerrainQuery_s *)TerI)->movement.x +
                              ((TerrainQuery_s *)TerI)->movement.y * ((TerrainQuery_s *)TerI)->movement.y +
                              ((TerrainQuery_s *)TerI)->movement.z * ((TerrainQuery_s *)TerI)->movement.z;
            f32 reach = TerrainScanMargin02Pc + ((TerrainQuery_s *)TerI)->collision_radius_sq + movement_sq;
            f32 vertical_reach = ((TerrainQuery_s *)TerI)->object_scale * reach;
            bounds.min_x = ((TerrainQuery_s *)TerI)->position.x - reach;
            bounds.max_x = ((TerrainQuery_s *)TerI)->position.x + reach;
            bounds.min_y = ((TerrainQuery_s *)TerI)->position.y - vertical_reach;
            bounds.max_y = ((TerrainQuery_s *)TerI)->position.y + vertical_reach;
            bounds.min_z = ((TerrainQuery_s *)TerI)->position.z - reach;
            bounds.max_z = ((TerrainQuery_s *)TerI)->position.z + reach;
            scan_radius_sq = reach + 1.0f;
        } else {
            {
                const NUVEC &position = ((TerrainQuery_s *)TerI)->position;
                const NUVEC &movement = ((TerrainQuery_s *)TerI)->movement;
                const f32 radius = ((TerrainQuery_s *)TerI)->collision_radius;

                if (movement.x > 0.0f) {
                    bounds.min_x = position.x - TerrainScanMargin02Pc - radius;
                    bounds.max_x = movement.x + position.x + TerrainScanMargin02Pc + radius;
                } else {
                    bounds.min_x = movement.x + position.x - TerrainScanMargin02Pc - radius;
                    bounds.max_x = position.x + TerrainScanMargin02Pc + radius;
                }
                if (movement.y > 0.0f) {
                    bounds.min_y = position.y - TerrainScanMargin02Pc - radius;
                    bounds.max_y = movement.y + position.y + TerrainScanMargin02Pc + radius;
                } else {
                    bounds.min_y = movement.y + position.y - TerrainScanMargin02Pc - radius;
                    bounds.max_y = position.y + TerrainScanMargin02Pc + radius;
                }
                if (movement.z > 0.0f) {
                    bounds.min_z = position.z - TerrainScanMargin02Pc - radius;
                    bounds.max_z = movement.z + position.z + TerrainScanMargin02Pc + radius;
                } else {
                    bounds.min_z = movement.z + position.z - TerrainScanMargin02Pc - radius;
                    bounds.max_z = position.z + TerrainScanMargin02Pc + radius;
                }
            }
            f32 dx = bounds.min_x - bounds.max_x;
            f32 dy = bounds.min_y - bounds.max_y;
            f32 dz = bounds.min_z - bounds.max_z;
            scan_radius_sq = dx * dx + dy * dy + dz * dz + 1.02f;
        }
        f32 scan_radius = NuFsqrt(scan_radius_sq);
        i32 disabled_platform = TerrPlatDis;
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        while (handle[0] > 0) {
            i32 count = handle[0];
            i32 group_index = handle[1];
            TERRAIN_SHAPE **shapes =
                reinterpret_cast<TERRAIN_SHAPE **>(reinterpret_cast<u8 *>(handle) + sizeof(TERRAIN_SHAPE *));
            handle = reinterpret_cast<i16 *>(shapes + count);
            TERRAIN_GROUP_PC &group = ((TERRAIN_SET_PC *)CurrentTerrain)->groups[group_index];
            TerrainScanBounds local = bounds;
            TERRAIN_PLATFORM_PC *platform =
                group.chunk_type == 1 ? &((TERRAIN_SET_PC *)CurrentTerrain)->platforms[group.scene_index] : NULL;
            NUMTX *matrix = platform != NULL ? static_cast<NUMTX *>(platform->scene_object) : NULL;
            if (matrix == NULL) {
                if (!TERRAIN_BOUNDS_OVERLAP(bounds, group.bounds_min, group.bounds_max))
                    continue;
                local.min_x -= group.origin.x;
                local.max_x -= group.origin.x;
                local.min_y -= group.origin.y;
                local.max_y -= group.origin.y;
                local.min_z -= group.origin.z;
                local.max_z -= group.origin.z;
                TERRAIN_HANDLE_FILTER_SHAPES(&writer, shapes, count, local, terrain_mask, false);
                TERRAIN_FINISH_SCAN_GROUP(&writer, group_index);
                continue;
            }
            if (matrix->m30 > platform->previous_matrix.m30) {
                local.min_x = bounds.min_x - group.origin.x;
                local.max_x = (matrix->m30 - platform->previous_matrix.m30) * 1.5f + bounds.max_x - group.origin.x;
            } else {
                local.min_x = (matrix->m30 - platform->previous_matrix.m30) * 1.5f + bounds.min_x - group.origin.x;
                local.max_x = bounds.max_x - group.origin.x;
            }
            if (matrix->m31 > platform->previous_matrix.m31) {
                local.min_y = bounds.min_y - group.origin.y;
                local.max_y = (matrix->m31 - platform->previous_matrix.m31) * 1.5f + bounds.max_y - group.origin.y;
            } else {
                local.min_y = (matrix->m31 - platform->previous_matrix.m31) * 1.5f + bounds.min_y - group.origin.y;
                local.max_y = bounds.max_y - group.origin.y;
            }
            if (matrix->m32 > platform->previous_matrix.m32) {
                local.min_z = bounds.min_z - group.origin.z;
                local.max_z = (matrix->m32 - platform->previous_matrix.m32) * 1.5f + bounds.max_z - group.origin.z;
            } else {
                local.min_z = (matrix->m32 - platform->previous_matrix.m32) * 1.5f + bounds.min_z - group.origin.z;
                local.max_z = bounds.max_z - group.origin.z;
            }
            if (group.scene_index == disabled_platform)
                continue;
            if ((platform->flags & TERRAIN_PLATFORM_FLAG_ROTATING) == 0) {
                if (!TERRAIN_BOUNDS_OVERLAP(local, group.bounds_min, group.bounds_max))
                    continue;
                TERRAIN_HANDLE_FILTER_SHAPES(&writer, shapes, count, local, terrain_mask, false);
                TERRAIN_FINISH_SCAN_GROUP(&writer, group_index);
                continue;
            }
            f32 dx = (bounds.min_x + bounds.max_x) * 0.5f - group.origin.x;
            f32 dy = ((bounds.min_y + bounds.max_y) * 0.5f - group.origin.y) * query->inverse_object_scale;
            f32 dz = (bounds.min_z + bounds.max_z) * 0.5f - group.origin.z;
            f32 radius = scan_radius + group.radius;
            if (!(radius * radius > dx * dx + dy * dy + dz * dz))
                continue;
            TERRAIN_HANDLE_FILTER_SHAPES(&writer, shapes, count, local, terrain_mask, true);
            TERRAIN_FINISH_SCAN_GROUP(&writer, group_index);
        }
        i16 *terminator = reinterpret_cast<i16 *>(writer.group_header);
        terminator[0] = 0;
        terminator[1] = 0;
        WallSplCount = 0;
        bounds.min_x -= TerrainScanMargin02Pc;
        bounds.max_x += TerrainScanMargin02Pc;
        bounds.min_z -= TerrainScanMargin02Pc;
        bounds.max_z += TerrainScanMargin02Pc;
        TERRAIN_WALL_POINT **walls =
            reinterpret_cast<TERRAIN_WALL_POINT **>(reinterpret_cast<u8 *>(handle) + sizeof(TERRAIN_WALL_POINT *));
        while (*walls != NULL) {
            TERRAIN_WALL_POINT *wall = *walls++;
            if (TERRAIN_HANDLE_WALL_OVERLAP(bounds, wall) && WallSplCount < 64) {
                ((TERRAIN_WALL_POINT *)WallSplList)[WallSplCount] = wall[0];
                ((TERRAIN_WALL_POINT *)WallSplList)[WallSplCount + 1] = wall[1];
                WallSplCount += 2;
            }
        }
        TerrPlatDis = -1;
    }
    void ScanTerrainPlatform(i32 group_index, i32 terrain_mask) {
        void *scratch = NuScratchAlloc32(0xd0);
        TerrainScaledShapeCursorPc = static_cast<TERRAIN_SHAPE *>(TerrainScaleWorkBasePc);
        platinrange = 0;
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        u8 *header = query->scan_list_storage;
        TERRAIN_SHAPE **cursor = reinterpret_cast<TERRAIN_SHAPE **>(header + sizeof(void *));
        u8 *limit = query->scan_list_storage + sizeof(query->scan_list_storage) - sizeof(void *) * 3;
        query->scan_group_index = -1;
        TerrainScanBounds bounds;
        f32 scan_radius;
        if (query->scan_result == 1) {
            f32 radius = query->collision_radius;
            if (!(query->movement.x > 0.0f)) {
                bounds.min_x = (query->movement.x + query->position.x - TerrainScanMargin02Pc) - radius;
                bounds.max_x = query->position.x + TerrainScanMargin02Pc + radius;
            } else {
                bounds.min_x = (query->position.x - TerrainScanMargin02Pc) - radius;
                bounds.max_x = query->position.x + query->movement.x + TerrainScanMargin02Pc + radius;
            }
            if (!(query->movement.y > 0.0f)) {
                bounds.min_y = (query->movement.y + query->position.y - TerrainScanMargin02Pc) - radius;
                bounds.max_y = query->position.y + TerrainScanMargin02Pc + radius;
            } else {
                bounds.min_y = (query->position.y - TerrainScanMargin02Pc) - radius;
                bounds.max_y = query->position.y + query->movement.y + TerrainScanMargin02Pc + radius;
            }
            if (!(query->movement.z > 0.0f)) {
                bounds.min_z = (query->movement.z + query->position.z - TerrainScanMargin02Pc) - radius;
                bounds.max_z = query->position.z + TerrainScanMargin02Pc + radius;
            } else {
                bounds.min_z = (query->position.z - TerrainScanMargin02Pc) - radius;
                bounds.max_z = query->position.z + query->movement.z + TerrainScanMargin02Pc + radius;
            }
            const f32 dx = bounds.min_x - bounds.max_x;
            const f32 dy = bounds.min_y - bounds.max_y;
            const f32 dz = bounds.min_z - bounds.max_z;
            scan_radius = NuFsqrt((radius + 1.02f) + (dx * dx + dy * dy + dz * dz));
        } else {
            const f32 reach = (TerrainScanMargin02Pc + query->collision_radius_sq) +
                              (query->movement.x * query->movement.x + query->movement.y * query->movement.y +
                               query->movement.z * query->movement.z);
            const f32 vertical = query->object_scale * reach;
            bounds.min_x = query->position.x - reach;
            bounds.max_x = query->position.x + reach;
            bounds.min_y = query->position.y - vertical;
            bounds.max_y = query->position.y + vertical;
            bounds.min_z = query->position.z - reach;
            bounds.max_z = query->position.z + reach;
            scan_radius = NuFsqrt(reach + 1.0f);
        }
        query = (TerrainQuery_s *)TerI;
        query->scan_list = header;
        bounds.max_x += NuTimeBarTop;
        bounds.min_y -= NuTimeBarTop;
        bounds.min_x -= NuTimeBarTop;
        bounds.min_z -= NuTimeBarTop;
        bounds.max_z += NuTimeBarTop;
        bounds.max_y += NuTimeBarTop;
        TERRAIN_GROUP_PC *group = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[group_index];
        TERRAIN_PLATFORM_PC *platform = &((TERRAIN_SET_PC *)CurrentTerrain)->platforms[group->scene_index];
        NUMTX *matrix = static_cast<NUMTX *>(platform->scene_object);
        TerrainScanBounds local = bounds;
        if (matrix != NULL) {
            if (platform->previous_matrix.m30 < matrix->m30)
                local.max_x += (matrix->m30 - platform->previous_matrix.m30) * 1.5f;
            else
                local.min_x += (matrix->m30 - platform->previous_matrix.m30) * 1.5f;
            if (platform->previous_matrix.m31 < matrix->m31)
                local.max_y += (matrix->m31 - platform->previous_matrix.m31) * 1.5f;
            else
                local.min_y += (matrix->m31 - platform->previous_matrix.m31) * 1.5f;
            if (!(matrix->m32 > platform->previous_matrix.m32))
                local.min_z += (matrix->m32 - platform->previous_matrix.m32) * 1.5f;
            else
                local.max_z += (matrix->m32 - platform->previous_matrix.m32) * 1.5f;
        }
        local.min_x -= group->origin.x;
        local.max_x -= group->origin.x;
        local.min_y -= group->origin.y;
        local.max_y -= group->origin.y;
        local.min_z -= group->origin.z;
        local.max_z -= group->origin.z;
        i32 count = 0;
        i32 scaled_count = 0;
        if ((platform->flags & TERRAIN_PLATFORM_FLAG_ROTATING) == 0) {
            if (group->bounds_min.x <= local.max_x && group->bounds_min.y <= local.max_y &&
                group->bounds_min.z <= local.max_z && local.min_x <= group->bounds_max.x &&
                local.min_y < group->bounds_max.y && local.min_z < group->bounds_max.z && group->chunk_type != -1) {
                TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
                while (batch->marker >= 0) {
                    i32 shape_count = batch->shape_count;
                    TERRAIN_SHAPE *source = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                    if (local.max_x < batch->min_x || batch->max_x <= local.min_x || local.max_z < batch->min_z ||
                        batch->max_z <= local.min_z) {
                        source += shape_count;
                    } else {
                        for (i32 remaining = shape_count; remaining > 0; --remaining, ++source) {
                            if (!(source->min_x <= local.max_x && local.min_x < source->max_x &&
                                  source->min_z <= local.max_z && local.min_z < source->max_z &&
                                  source->min_y <= local.max_y && local.min_y < source->max_y))
                                continue;
                            platinrange = 1;
                            if (!(reinterpret_cast<u8 *>(cursor) < limit) ||
                                (source->material[1] != 0 && (terrain_mask & source->material[1]) == 0))
                                continue;
                            if (query->object_scale == 1.0f) {
                                *cursor++ = source;
                                ++count;
                            } else {
                                TERRAIN_SHAPE *surface = &TerrainScaledShapeCursorPc[scaled_count];
                                memcpy(surface->material, source->material, 4);
                                surface->vertices[0].x = source->vertices[0].x;
                                surface->vertices[0].z = source->vertices[0].z;
                                const f32 origin_y = group->origin.y;
                                const f32 inverse_scale = query->inverse_object_scale;
                                surface->vertices[0].y = (source->vertices[0].y + origin_y) * inverse_scale - origin_y;
                                surface->vertices[1].x = source->vertices[1].x;
                                surface->vertices[1].z = source->vertices[1].z;
                                surface->vertices[1].y = (source->vertices[1].y + origin_y) * inverse_scale - origin_y;
                                surface->vertices[2].x = source->vertices[2].x;
                                surface->vertices[2].z = source->vertices[2].z;
                                surface->vertices[2].y = (source->vertices[2].y + origin_y) * inverse_scale - origin_y;
                                if (source->normals[1].y < 65535.0f) {
                                    surface->vertices[3].x = source->vertices[3].x;
                                    surface->vertices[3].z = source->vertices[3].z;
                                    surface->vertices[3].y =
                                        (source->vertices[3].y + origin_y) * inverse_scale - origin_y;
                                    const NUVEC *normal = &source->normals[1];
                                    f32 length =
                                        NuFsqrt(normal->x * normal->x + normal->y * normal->y * query->object_scale_sq +
                                                normal->z * normal->z);
                                    f32 inverse = 0.0f;
                                    if (length != 0.0f)
                                        inverse = 1.0f / length;
                                    query = (TerrainQuery_s *)TerI;
                                    surface = &TerrainScaledShapeCursorPc[scaled_count];
                                    surface->normals[1].x = normal->x * inverse;
                                    surface->normals[1].y = normal->y * query->object_scale * inverse;
                                    surface->normals[1].z = inverse * normal->z;
                                } else {
                                    surface->normals[1].y = 65536.0f;
                                }
                                const NUVEC *normal = &source->normals[0];
                                f32 length =
                                    NuFsqrt(normal->x * normal->x + normal->y * normal->y * query->object_scale_sq +
                                            normal->z * normal->z);
                                f32 inverse = 0.0f;
                                if (length != 0.0f)
                                    inverse = 1.0f / length;
                                query = (TerrainQuery_s *)TerI;
                                surface = &TerrainScaledShapeCursorPc[scaled_count];
                                surface->normals[0].x = normal->x * inverse;
                                surface->normals[0].y = normal->y * query->object_scale * inverse;
                                surface->normals[0].z = inverse * normal->z;
                                *cursor++ = surface;
                                ++count;
                                ++scaled_count;
                            }
                        }
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(source);
                }
            }
        } else {
            const f32 x = (bounds.max_x + bounds.min_x) * 0.5f - group->origin.x;
            const f32 y = ((bounds.max_y + bounds.min_y) * 0.5f - group->origin.y) * query->inverse_object_scale;
            const f32 z = (bounds.max_z + bounds.min_z) * 0.5f - group->origin.z;
            if (matrix != NULL && group->chunk_type != -1 &&
                x * x + y * y + z * z < (scan_radius + group->radius) * (scan_radius + group->radius)) {
                NUVEC4 *vertices = reinterpret_cast<NUVEC4 *>((reinterpret_cast<u32>(scratch) + 31) & ~u32(15));
                TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
                while (batch->marker >= 0) {
                    i32 shape_count = batch->shape_count;
                    TERRAIN_SHAPE *source = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                    for (i32 remaining = shape_count; remaining > 0; --remaining, ++source) {
                        platinrange = 1;
                        if (!(reinterpret_cast<u8 *>(cursor) < limit) ||
                            (source->material[1] != 0 && (terrain_mask & source->material[1]) == 0))
                            continue;
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
                        NuVec4MtxTransformVU0x3(
                            vertices, vertices,
                            static_cast<NUMTX *>(
                                ((TERRAIN_SET_PC *)CurrentTerrain)->platforms[group->scene_index].scene_object));
                        if (source->normals[1].y < 65535.0f) {
                            vertices[3].x = source->vertices[3].x;
                            vertices[3].y = source->vertices[3].y;
                            vertices[3].z = source->vertices[3].z;
                            vertices[3].w = 0.0f;
                            NuVec4MtxTransformVU0(
                                &vertices[3], &vertices[3],
                                static_cast<NUMTX *>(
                                    ((TERRAIN_SET_PC *)CurrentTerrain)->platforms[group->scene_index].scene_object));
                        } else {
                            vertices[3] = vertices[2];
                        }
                        if (!(local.min_x < vertices[0].x || local.min_x < vertices[1].x ||
                              local.min_x < vertices[2].x || local.min_x < vertices[3].x) ||
                            !(vertices[0].x < local.max_x || vertices[1].x < local.max_x ||
                              vertices[2].x < local.max_x || vertices[3].x < local.max_x))
                            continue;
                        if (vertices[0].z <= local.min_z && !(local.min_z < vertices[1].z) &&
                            vertices[2].z <= local.min_z && vertices[3].z <= local.min_z)
                            continue;
                        if (!(vertices[0].z < local.max_z || vertices[1].z < local.max_z ||
                              vertices[2].z < local.max_z || vertices[3].z < local.max_z))
                            continue;
                        TERRAIN_SHAPE *surface = &TerrainScaledShapeCursorPc[scaled_count];
                        memcpy(surface->material, source->material, 4);
                        const f32 origin_y = group->origin.y;
                        const f32 inverse_scale = ((TerrainQuery_s *)TerI)->inverse_object_scale;
                        surface->vertices[0].x = vertices[0].x;
                        surface->vertices[0].z = vertices[0].z;
                        surface->vertices[1].x = vertices[1].x;
                        surface->vertices[1].z = vertices[1].z;
                        surface->vertices[2].x = vertices[2].x;
                        surface->vertices[2].z = vertices[2].z;
                        surface->vertices[0].y = (vertices[0].y + origin_y) * inverse_scale - origin_y;
                        surface->vertices[1].y = (vertices[1].y + origin_y) * inverse_scale - origin_y;
                        surface->vertices[2].y = (vertices[2].y + origin_y) * inverse_scale - origin_y;
                        if (source->normals[1].y < 65535.0f) {
                            surface->vertices[3].x = vertices[3].x;
                            surface->vertices[3].z = vertices[3].z;
                            surface->vertices[3].y = (vertices[3].y + origin_y) * inverse_scale - origin_y;
                            vertices[0].x = surface->vertices[1].x - surface->vertices[3].x;
                            vertices[0].y = surface->vertices[1].y - surface->vertices[3].y;
                            vertices[0].z = surface->vertices[1].z - surface->vertices[3].z;
                            vertices[1].x = surface->vertices[2].x - surface->vertices[3].x;
                            vertices[1].y = surface->vertices[2].y - surface->vertices[3].y;
                            vertices[1].z = surface->vertices[2].z - surface->vertices[3].z;
                            surface->normals[1].x = vertices[0].y * vertices[1].z - vertices[0].z * vertices[1].y;
                            surface->normals[1].y = vertices[0].z * vertices[1].x - vertices[0].x * vertices[1].z;
                            surface->normals[1].z = vertices[0].x * vertices[1].y - vertices[0].y * vertices[1].x;
                            surface = &TerrainScaledShapeCursorPc[scaled_count];
                            NUVEC *normal = &surface->normals[1];
                            f32 length = NuFsqrt(normal->x * normal->x + normal->y * normal->y + normal->z * normal->z);
                            f32 inverse = 0.0f;
                            if (length != 0.0f)
                                inverse = 1.0f / length;
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
                        surface->normals[0].x = vertices[0].y * vertices[1].z - vertices[0].z * vertices[1].y;
                        surface->normals[0].y = vertices[0].z * vertices[1].x - vertices[0].x * vertices[1].z;
                        surface->normals[0].z = vertices[0].x * vertices[1].y - vertices[0].y * vertices[1].x;
                        surface = &TerrainScaledShapeCursorPc[scaled_count];
                        NUVEC *normal = &surface->normals[0];
                        f32 length = NuFsqrt(normal->x * normal->x + normal->y * normal->y + normal->z * normal->z);
                        f32 inverse = 0.0f;
                        if (length != 0.0f)
                            inverse = 1.0f / length;
                        surface = &TerrainScaledShapeCursorPc[scaled_count];
                        surface->normals[0].x *= inverse;
                        surface->normals[0].y *= inverse;
                        surface->normals[0].z *= inverse;
                        *cursor++ = surface;
                        ++count;
                        ++scaled_count;
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(source);
                }
            }
        }
        if (count != 0) {
            reinterpret_cast<i16 *>(header)[0] = static_cast<i16>(count);
            reinterpret_cast<i16 *>(header)[1] = static_cast<i16>(group_index);
            header = reinterpret_cast<u8 *>(cursor);
        }
        NuScratchRelease();
        reinterpret_cast<i16 *>(header)[0] = 0;
        reinterpret_cast<i16 *>(header)[1] = 0;
    }
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
static u8 *TempStackPtrPc;
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

i16 *NewScanHandelSubsetPc(i16 *subset, NUVEC *position, NUVEC *movement, f32 radius, i32 terrain_mask) {
    if (subset == NULL)
        return NULL;
    TerrainScanWriter writer;
    writer.group_header = TempStackPtrPc;
    writer.cursor = reinterpret_cast<TERRAIN_SHAPE **>(TempStackPtrPc + 4);
    u8 *scale_base = reinterpret_cast<u8 *>(TerrainScaleWorkBasePc);
    if (reinterpret_cast<usize>(writer.cursor) > reinterpret_cast<usize>(scale_base + 0x63c18))
        return NULL;
    writer.limit = reinterpret_cast<u8 *>(writer.cursor) + 0x7f0;
    if (reinterpret_cast<usize>(writer.limit) > reinterpret_cast<usize>(scale_base + 0x63da8))
        writer.limit = scale_base + 0x63da8;
    writer.group_shape_count = 0;
    writer.scaled_shape_count = 0;
    i16 *result = reinterpret_cast<i16 *>(writer.group_header);
    TerrainScanBounds bounds;
    if (movement->x > 0.0f) {
        bounds.min_x = static_cast<f64>(position->x) - TerrainScanMargin02Pc - radius;
        bounds.max_x = static_cast<f64>(position->x) + movement->x + TerrainScanMargin02Pc + radius;
    } else {
        bounds.min_x = static_cast<f64>(position->x) + movement->x - TerrainScanMargin02Pc - radius;
        bounds.max_x = static_cast<f64>(position->x) + TerrainScanMargin02Pc + radius;
    }
    if (movement->y > 0.0f) {
        bounds.min_y = static_cast<f64>(position->y) - TerrainScanMargin02Pc - radius;
        bounds.max_y = static_cast<f64>(position->y) + movement->y + TerrainScanMargin02Pc + radius;
    } else {
        bounds.min_y = static_cast<f64>(position->y) + movement->y - TerrainScanMargin02Pc - radius;
        bounds.max_y = static_cast<f64>(position->y) + TerrainScanMargin02Pc + radius;
    }
    if (movement->z > 0.0f) {
        bounds.min_z = static_cast<f64>(position->z) - TerrainScanMargin02Pc - radius;
        bounds.max_z = static_cast<f64>(position->z) + movement->z + TerrainScanMargin02Pc + radius;
    } else {
        bounds.min_z = static_cast<f64>(position->z) + movement->z - TerrainScanMargin02Pc - radius;
        bounds.max_z = static_cast<f64>(position->z) + TerrainScanMargin02Pc + radius;
    }
    const f64 dx = static_cast<f64>(bounds.min_x) - bounds.max_x;
    const f64 dy = static_cast<f64>(bounds.min_y) - bounds.max_y;
    const f64 dz = static_cast<f64>(bounds.min_z) - bounds.max_z;
    const f32 diagonal_squared = dx * dx + dy * dy + dz * dz;
    const f32 scan_radius_squared = diagonal_squared + NuTimeBarTop;
    while (subset[0] > 0) {
        const i32 count = subset[0];
        const i32 group_index = subset[1];
        TERRAIN_SHAPE **shapes = reinterpret_cast<TERRAIN_SHAPE **>(subset + 2);
        subset = reinterpret_cast<i16 *>(shapes + count);
        TERRAIN_GROUP_PC &group = static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->groups[group_index];
        TERRAIN_PLATFORM_PC *platform =
            group.chunk_type == 1
                ? &static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->platforms[static_cast<i16>(group.scene_index)]
                : NULL;
        NUMTX *matrix = platform != NULL ? static_cast<NUMTX *>(platform->scene_object) : NULL;
        TerrainScanBounds local;
        if (matrix != NULL) {
            if (matrix->m30 > platform->previous_matrix.m30) {
                local.min_x = bounds.min_x - group.origin.x;
                local.max_x = (static_cast<f64>(matrix->m30) - platform->previous_matrix.m30) * 1.5 + bounds.max_x -
                              group.origin.x;
            } else {
                local.max_x = bounds.max_x - group.origin.x;
                local.min_x = (static_cast<f64>(matrix->m30) - platform->previous_matrix.m30) * 1.5 + bounds.min_x -
                              group.origin.x;
            }
            if (matrix->m31 > platform->previous_matrix.m31) {
                local.min_y = bounds.min_y - group.origin.y;
                local.max_y = (static_cast<f64>(matrix->m31) - platform->previous_matrix.m31) * 1.5 + bounds.max_y -
                              group.origin.y;
            } else {
                local.max_y = bounds.max_y - group.origin.y;
                local.min_y = (static_cast<f64>(matrix->m31) - platform->previous_matrix.m31) * 1.5 + bounds.min_y -
                              group.origin.y;
            }
            if (matrix->m32 > platform->previous_matrix.m32) {
                local.min_z = bounds.min_z - group.origin.z;
                local.max_z = (static_cast<f64>(matrix->m32) - platform->previous_matrix.m32) * 1.5 + bounds.max_z -
                              group.origin.z;
            } else {
                local.max_z = bounds.max_z - group.origin.z;
                local.min_z = (static_cast<f64>(matrix->m32) - platform->previous_matrix.m32) * 1.5 + bounds.min_z -
                              group.origin.z;
            }
        } else {
            if (!TERRAIN_BOUNDS_OVERLAP(bounds, group.bounds_min, group.bounds_max)) {
                subset += 2;
                continue;
            }
            local.min_x = bounds.min_x - group.origin.x;
            local.max_x = bounds.max_x - group.origin.x;
            local.min_y = bounds.min_y - group.origin.y;
            local.max_y = bounds.max_y - group.origin.y;
            local.min_z = bounds.min_z - group.origin.z;
            local.max_z = bounds.max_z - group.origin.z;
        }
        const bool rotating = matrix != NULL && (platform->flags & TERRAIN_PLATFORM_FLAG_ROTATING) != 0;
        if (rotating) {
            const f64 x = (static_cast<f64>(bounds.min_x) + bounds.max_x) * 0.5 - group.origin.x;
            const f64 y = ((static_cast<f64>(bounds.min_y) + bounds.max_y) * 0.5 - group.origin.y) *
                          static_cast<TerrainQuery_s *>(TerI)->inverse_object_scale;
            const f64 z = (static_cast<f64>(bounds.min_z) + bounds.max_z) * 0.5 - group.origin.z;
            const f32 distance_squared = x * x + y * y + z * z;
            const f64 reach = static_cast<f64>(scan_radius_squared) + group.radius;
            if (!(reach * reach > distance_squared))
                continue;
        } else if (matrix != NULL && !TERRAIN_BOUNDS_OVERLAP(local, group.bounds_min, group.bounds_max)) {
            continue;
        }
        TERRAIN_HANDLE_FILTER_SHAPES(&writer, shapes, count, local, terrain_mask, rotating);
        TERRAIN_FINISH_SCAN_GROUP(&writer, group_index);
    }
    TERRAIN_SCAN_WORD_PC *terminator = reinterpret_cast<TERRAIN_SCAN_WORD_PC *>(writer.group_header);
    terminator->header.shape_count = 0;
    terminator->header.terrain_group = 0;
    bounds.min_x -= TerrainScanMargin02Pc;
    bounds.max_x += TerrainScanMargin02Pc;
    bounds.min_z -= TerrainScanMargin02Pc;
    bounds.max_z += TerrainScanMargin02Pc;
    TERRAIN_HANDLE_WALL_PAIR_PC *source = reinterpret_cast<TERRAIN_HANDLE_WALL_PAIR_PC *>(subset + 2);
    TERRAIN_HANDLE_WALL_PAIR_PC *walls = reinterpret_cast<TERRAIN_HANDLE_WALL_PAIR_PC *>(terminator + 1);
    while (static_cast<i32>(reinterpret_cast<usize>(source->first)) > 0) {
        TERRAIN_WALL_POINT *wall = source->first;
        if (TERRAIN_HANDLE_WALL_OVERLAP(bounds, wall) &&
            (wall->material[1] == 0 || (wall->material[1] & terrain_mask) != 0)) {
            walls->first = source->first;
            walls->second = source->first;
            ++walls;
        }
        ++source;
    }
    TERRAIN_SCAN_WORD_PC *end = reinterpret_cast<TERRAIN_SCAN_WORD_PC *>(walls);
    end->header.shape_count = 0;
    end->header.terrain_group = 0;
    TempStackPtrPc = reinterpret_cast<u8 *>(end + 1);
    return result;
}

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

i16 *NewScanHandelFullPc(NUVEC *position, NUVEC *movement, f32 radius, i32 scan_type, i32 terrain_mask) {
    TerrainScanWriter writer;
    writer.group_header = TempStackPtrPc;
    writer.cursor = reinterpret_cast<TERRAIN_SHAPE **>(TempStackPtrPc + 4);
    TerrainScaledShapeCursorPc = static_cast<TERRAIN_SHAPE *>(TerrainScaleWorkBasePc);
    u8 *scale_base = reinterpret_cast<u8 *>(TerrainScaleWorkBasePc);
    if (reinterpret_cast<usize>(writer.cursor) > reinterpret_cast<usize>(scale_base + 0x63c18))
        return NULL;
    writer.limit = reinterpret_cast<u8 *>(writer.cursor) + 0x7f0;
    if (reinterpret_cast<usize>(writer.limit) > reinterpret_cast<usize>(scale_base + 0x63da8))
        writer.limit = scale_base + 0x63da8;
    writer.group_shape_count = 0;
    writer.scaled_shape_count = 0;
    i16 *result = reinterpret_cast<i16 *>(writer.group_header);
    struct HandleWorkspace {
        NUVEC4 vertices[4];
    };
    void *scratch = NuScratchAlloc32(0xd0);
    const usize address = (reinterpret_cast<usize>(scratch) + 0x1f) & ~static_cast<usize>(0xf);
    u8 *workspace_storage = reinterpret_cast<u8 *>(address);
    platinrange = 0;
    TerrainScanBounds bounds;
    if (movement->x > 0.0f) {
        bounds.min_x = static_cast<f64>(position->x) - TerrainScanMargin02Pc - radius;
        bounds.max_x = static_cast<f64>(position->x) + movement->x + TerrainScanMargin02Pc + radius;
    } else {
        bounds.min_x = static_cast<f64>(position->x) + movement->x - TerrainScanMargin02Pc - radius;
        bounds.max_x = static_cast<f64>(position->x) + TerrainScanMargin02Pc + radius;
    }
    if (movement->y > 0.0f) {
        bounds.min_y = static_cast<f64>(position->y) - TerrainScanMargin02Pc - radius;
        bounds.max_y = static_cast<f64>(position->y) + movement->y + TerrainScanMargin02Pc + radius;
    } else {
        bounds.min_y = static_cast<f64>(position->y) + movement->y - TerrainScanMargin02Pc - radius;
        bounds.max_y = static_cast<f64>(position->y) + TerrainScanMargin02Pc + radius;
    }
    if (movement->z > 0.0f) {
        bounds.min_z = static_cast<f64>(position->z) - TerrainScanMargin02Pc - radius;
        bounds.max_z = static_cast<f64>(position->z) + movement->z + TerrainScanMargin02Pc + radius;
    } else {
        bounds.min_z = static_cast<f64>(position->z) + movement->z - TerrainScanMargin02Pc - radius;
        bounds.max_z = static_cast<f64>(position->z) + TerrainScanMargin02Pc + radius;
    }
    const f64 dx = static_cast<f64>(bounds.min_x) - bounds.max_x;
    const f64 dy = static_cast<f64>(bounds.min_y) - bounds.max_y;
    const f64 dz = static_cast<f64>(bounds.min_z) - bounds.max_z;
    const f32 diagonal_squared = dx * dx + dy * dy + dz * dz;
    const f32 scan_radius_squared = diagonal_squared + NuTimeBarTop;
    const f32 scan_radius = NuFsqrt(scan_radius_squared);
    for (i32 cell_index = 0; cell_index < static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->used_cell_count;
         ++cell_index) {
        const TERRAIN_CELL_PC &cell = static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->cells[cell_index];
        if (!(bounds.max_x >= cell.min_x && bounds.max_z >= cell.min_z && cell.max_x >= bounds.min_x &&
              cell.max_z >= bounds.min_z))
            continue;
        const i16 *indices =
            static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->group_indices + static_cast<i16>(cell.first_group);
        for (i32 i = 0;
             i < static_cast<i16>(static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->cells[cell_index].group_count); ++i) {
            const i32 index = indices[i];
            TERRAIN_GROUP_PC &group = static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->groups[index];
            if (!TERRAIN_BOUNDS_OVERLAP(bounds, group.bounds_min, group.bounds_max) || group.chunk_type == -1)
                continue;
            TerrainScanBounds local = bounds;
            local.min_x -= group.origin.x;
            local.max_x -= group.origin.x;
            local.min_y -= group.origin.y;
            local.max_y -= group.origin.y;
            local.min_z -= group.origin.z;
            local.max_z -= group.origin.z;
            if (static_cast<i16>(group.scene_index) < 0)
                TerrainSkinAllocate(&group);
            TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group.data);
            while (batch->marker >= 0) {
                const i32 shape_count = static_cast<i16>(batch->shape_count);
                TERRAIN_SHAPE *shapes = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                if (local.max_x >= batch->min_x && batch->max_x > local.min_x && local.max_z >= batch->min_z &&
                    batch->max_z > local.min_z) {
                    for (i32 j = 0; j < shape_count; ++j) {
                        TERRAIN_SHAPE *shape = &shapes[j];
                        if (local.max_x >= shape->min_x && shape->max_x > local.min_x && local.max_z >= shape->min_z &&
                            shape->max_z > local.min_z && local.max_y >= shape->min_y && shape->max_y > local.min_y &&
                            reinterpret_cast<u8 *>(writer.cursor) < writer.limit &&
                            (shape->material[1] == 0 || (shape->material[1] & terrain_mask) != 0)) {
                            *writer.cursor++ = shape;
                            ++writer.group_shape_count;
                        }
                    }
                }
                batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shapes + shape_count);
            }
            TERRAIN_FINISH_SCAN_GROUP(&writer, index);
        }
    }
    bounds.min_x -= NuTimeBarTop;
    bounds.max_x += NuTimeBarTop;
    bounds.min_y -= NuTimeBarTop;
    bounds.max_y += NuTimeBarTop;
    bounds.min_z -= NuTimeBarTop;
    bounds.max_z += NuTimeBarTop;
    static_cast<TerrainQuery_s *>(TerI)->scan_list = writer.group_header;
    if (scan_type != 0) {
        const TERRAIN_CELL_PC &cell = static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->platform_cell;
        const i16 *indices =
            static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->group_indices + static_cast<i16>(cell.first_group);
        for (i32 i = 0; i < static_cast<i16>(static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->platform_cell.group_count);
             ++i) {
            const i32 index = indices[i];
            TERRAIN_GROUP_PC &group = static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->groups[index];
            TERRAIN_PLATFORM_PC &platform =
                static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->platforms[static_cast<i16>(group.scene_index)];
            const u8 flags = platform.flags;
            TERRAIN_SCENE_STATE_VIEW_PC *scene_state =
                static_cast<TERRAIN_SCENE_STATE_VIEW_PC *>(platform.scene_transform);
            if (scene_state != NULL &&
                ((flags & 4) != 0 ? (scene_state->flags & 2) == 0 : (scene_state->low_flags & 1) == 0))
                continue;
            NUMTX *matrix = static_cast<NUMTX *>(platform.scene_object);
            TerrainScanBounds local;
            if (matrix != NULL) {
                if (matrix->m30 > platform.previous_matrix.m30) {
                    local.min_x = bounds.min_x - group.origin.x;
                    local.max_x = (static_cast<f64>(matrix->m30) - platform.previous_matrix.m30) * 1.5 + bounds.max_x -
                                  group.origin.x;
                } else {
                    local.max_x = bounds.max_x - group.origin.x;
                    local.min_x = (static_cast<f64>(matrix->m30) - platform.previous_matrix.m30) * 1.5 + bounds.min_x -
                                  group.origin.x;
                }
                if (matrix->m31 > platform.previous_matrix.m31) {
                    local.min_y = bounds.min_y - group.origin.y;
                    local.max_y = (static_cast<f64>(matrix->m31) - platform.previous_matrix.m31) * 1.5 + bounds.max_y -
                                  group.origin.y;
                } else {
                    local.max_y = bounds.max_y - group.origin.y;
                    local.min_y = (static_cast<f64>(matrix->m31) - platform.previous_matrix.m31) * 1.5 + bounds.min_y -
                                  group.origin.y;
                }
                if (matrix->m32 > platform.previous_matrix.m32) {
                    local.min_z = bounds.min_z - group.origin.z;
                    local.max_z = (static_cast<f64>(matrix->m32) - platform.previous_matrix.m32) * 1.5 + bounds.max_z -
                                  group.origin.z;
                } else {
                    local.max_z = bounds.max_z - group.origin.z;
                    local.min_z = (static_cast<f64>(matrix->m32) - platform.previous_matrix.m32) * 1.5 + bounds.min_z -
                                  group.origin.z;
                }
            } else {
                local.min_x = bounds.min_x - group.origin.x;
                local.max_x = bounds.max_x - group.origin.x;
                local.min_y = bounds.min_y - group.origin.y;
                local.max_y = bounds.max_y - group.origin.y;
                local.min_z = bounds.min_z - group.origin.z;
                local.max_z = bounds.max_z - group.origin.z;
            }
            if ((flags & TERRAIN_PLATFORM_FLAG_ROTATING) == 0) {
                if (!TERRAIN_BOUNDS_OVERLAP(local, group.bounds_min, group.bounds_max) || group.chunk_type == -1)
                    continue;
                i32 in_range = platinrange;
                TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group.data);
                while (batch->marker >= 0) {
                    const i32 count = static_cast<i16>(batch->shape_count);
                    TERRAIN_SHAPE *shapes = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                    if (local.max_x >= batch->min_x && batch->max_x > local.min_x && local.max_z >= batch->min_z &&
                        batch->max_z > local.min_z) {
                        for (i32 j = 0; j < count; ++j) {
                            TERRAIN_SHAPE *shape = &shapes[j];
                            if (!(local.max_x >= shape->min_x && shape->max_x > local.min_x &&
                                  local.max_z >= shape->min_z && shape->max_z > local.min_z &&
                                  local.max_y >= shape->min_y && shape->max_y > local.min_y))
                                continue;
                            in_range = 1;
                            if (reinterpret_cast<u8 *>(writer.cursor) >= writer.limit ||
                                (shape->material[1] != 0 && (shape->material[1] & terrain_mask) == 0))
                                continue;
                            *writer.cursor++ = shape;
                            ++writer.group_shape_count;
                        }
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shapes + count);
                }
                platinrange = in_range;
            } else {
                const f64 x = (static_cast<f64>(bounds.min_x) + bounds.max_x) * 0.5 - group.origin.x;
                const f64 y = ((static_cast<f64>(bounds.min_y) + bounds.max_y) * 0.5 - group.origin.y) *
                              static_cast<TerrainQuery_s *>(TerI)->inverse_object_scale;
                const f64 z = (static_cast<f64>(bounds.min_z) + bounds.max_z) * 0.5 - group.origin.z;
                const f32 distance_squared = x * x + y * y + z * z;
                const f64 reach = static_cast<f64>(scan_radius) + group.radius;
                if (group.chunk_type == -1 || !(reach * reach > distance_squared))
                    continue;
                TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group.data);
                while (batch->marker >= 0) {
                    const i32 count = static_cast<i16>(batch->shape_count);
                    TERRAIN_SHAPE *shapes = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                    TERRAIN_SHAPE *source_cursor = shapes;
                    for (i32 remaining = count; remaining > 0; --remaining, ++source_cursor) {
                        TERRAIN_SHAPE *shape = source_cursor;
                        platinrange = 1;
                        if (reinterpret_cast<u8 *>(writer.cursor) >= writer.limit ||
                            (shape->material[1] != 0 && (shape->material[1] & terrain_mask) == 0))
                            continue;
                        HandleWorkspace *workspace = reinterpret_cast<HandleWorkspace *>(workspace_storage);
                        NUVEC4 *vertices = workspace->vertices;
                        for (i32 v = 0; v < 3; ++v) {
                            vertices[v].x = shape->vertices[v].x;
                            vertices[v].y = shape->vertices[v].y;
                            vertices[v].z = shape->vertices[v].z;
                            vertices[v].w = 0.0f;
                        }
                        NuVec4MtxTransformVU0x3(vertices, vertices, matrix);
                        const bool quad = shape->normals[1].y < 65535.0;
                        if (quad) {
                            vertices[3].x = shape->vertices[3].x;
                            vertices[3].y = shape->vertices[3].y;
                            vertices[3].z = shape->vertices[3].z;
                            vertices[3].w = 0.0f;
                            NuVec4MtxTransformVU0(&vertices[3], &vertices[3], matrix);
                        } else
                            vertices[3] = vertices[2];
                        if (!(vertices[0].x > local.min_x || vertices[1].x > local.min_x ||
                              vertices[2].x > local.min_x || vertices[3].x > local.min_x) ||
                            !(local.max_x > vertices[0].x || local.max_x > vertices[1].x ||
                              local.max_x > vertices[2].x || local.max_x > vertices[3].x) ||
                            !(vertices[0].z > local.min_z || vertices[1].z > local.min_z ||
                              vertices[2].z > local.min_z || vertices[3].z > local.min_z) ||
                            !(local.max_z > vertices[0].z || local.max_z > vertices[1].z ||
                              local.max_z > vertices[2].z || local.max_z > vertices[3].z))
                            continue;
                        TERRAIN_SHAPE *transformed = &TerrainScaledShapeCursorPc[writer.scaled_shape_count++];
                        memcpy(transformed->material, shape->material, 4);
                        transformed->vertices[0].x = vertices[0].x;
                        transformed->vertices[0].y = vertices[0].y;
                        transformed->vertices[0].z = vertices[0].z;
                        transformed->vertices[1].x = vertices[1].x;
                        transformed->vertices[1].y = vertices[1].y;
                        transformed->vertices[1].z = vertices[1].z;
                        transformed->vertices[2].x = vertices[2].x;
                        transformed->vertices[2].y = vertices[2].y;
                        transformed->vertices[2].z = vertices[2].z;
                        if (shape->normals[1].y < 65535.0) {
                            memcpy(&transformed->vertices[3], &vertices[3], sizeof(NUVEC));
                        }
                        if (!quad)
                            transformed->normals[1].y = 65536.0f;
                        if (quad) {
                            NUVEC a, b;
                            a.x = vertices[1].x - vertices[3].x;
                            b.x = vertices[2].x - vertices[3].x;
                            a.y = vertices[1].y - vertices[3].y;
                            b.y = vertices[2].y - vertices[3].y;
                            a.z = vertices[1].z - vertices[3].z;
                            b.z = vertices[2].z - vertices[3].z;
                            NUVEC *normal_a = reinterpret_cast<NUVEC *>(workspace_storage);
                            NUVEC *normal_b = reinterpret_cast<NUVEC *>(workspace_storage + sizeof(NUVEC4));
                            *normal_a = a;
                            *normal_b = b;
                            transformed->normals[1].x = normal_a->y * normal_b->z - normal_a->z * normal_b->y;
                            transformed->normals[1].y = normal_a->z * normal_b->x - normal_a->x * normal_b->z;
                            transformed->normals[1].z = normal_a->x * normal_b->y - normal_a->y * normal_b->x;
                            NUVEC &normal = transformed->normals[1];
                            const f32 length = NuFsqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
                            const f32 inverse = length == 0.0f ? 0.0f : 1.0f / length;
                            normal.x *= inverse;
                            normal.y *= inverse;
                            normal.z *= inverse;
                        }
                        {
                            NUVEC a, b;
                            a.x = transformed->vertices[2].x - transformed->vertices[0].x;
                            b.x = transformed->vertices[1].x - transformed->vertices[0].x;
                            a.y = transformed->vertices[2].y - transformed->vertices[0].y;
                            b.y = transformed->vertices[1].y - transformed->vertices[0].y;
                            a.z = transformed->vertices[2].z - transformed->vertices[0].z;
                            b.z = transformed->vertices[1].z - transformed->vertices[0].z;
                            NUVEC *normal_a = reinterpret_cast<NUVEC *>(workspace_storage);
                            NUVEC *normal_b = reinterpret_cast<NUVEC *>(workspace_storage + sizeof(NUVEC4));
                            *normal_a = a;
                            *normal_b = b;
                            transformed->normals[0].x = normal_a->y * normal_b->z - normal_a->z * normal_b->y;
                            transformed->normals[0].y = normal_a->z * normal_b->x - normal_a->x * normal_b->z;
                            transformed->normals[0].z = normal_a->x * normal_b->y - normal_a->y * normal_b->x;
                            NUVEC &normal = transformed->normals[0];
                            const f32 length = NuFsqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
                            const f32 inverse = length == 0.0f ? 0.0f : 1.0f / length;
                            normal.x *= inverse;
                            normal.y *= inverse;
                            normal.z *= inverse;
                        }
                        shape = transformed;
                        *writer.cursor++ = shape;
                        ++writer.group_shape_count;
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(shapes + count);
                }
            }
            TERRAIN_FINISH_SCAN_GROUP(&writer, index);
        }
    }
    TERRAIN_HANDLE_END_PC *end_header = reinterpret_cast<TERRAIN_HANDLE_END_PC *>(writer.group_header);
    end_header->terminator.header.shape_count = 0;
    end_header->terminator.header.terrain_group = 0;
    bounds.min_x -= TerrainScanMargin02Pc;
    bounds.max_x += TerrainScanMargin02Pc;
    bounds.min_z -= TerrainScanMargin02Pc;
    bounds.max_z += TerrainScanMargin02Pc;
    for (TERRAIN_SPATIAL_NODE *node = static_cast<TERRAIN_SET_PC *>(CurrentTerrain)->spatial_nodes; node != NULL;
         node = reinterpret_cast<TERRAIN_SPATIAL_CHUNK *>(reinterpret_cast<u8 *>(node) -
                                                          offsetof(TERRAIN_SPATIAL_CHUNK, node))
                    ->previous) {
        const u8 mask = static_cast<u8>(node->field_02 >> 8);
        if (mask != 0 && (mask & terrain_mask) == 0)
            continue;
        for (i32 first = 0; first < node->point_count; first += 16) {
            NUVEC *points = node->points + first;
            i32 end = node->point_count;
            if (points[0].y != 2147483648.0f) {
                if (!(points[1].y >= bounds.min_x && bounds.max_x >= points[0].y && points[3].y >= bounds.min_z &&
                      bounds.max_z >= points[2].y))
                    continue;
                if (first + 16 < end)
                    end = first + 16;
            }
            for (i32 i = first; i < end; ++i) {
                NUVEC &a = node->points[i];
                NUVEC &b = node->points[i + 1];
                if (!((a.x >= bounds.min_x && bounds.max_x >= b.x) || (b.x >= bounds.min_x && bounds.max_x >= a.x)) ||
                    !((a.z >= bounds.min_z && bounds.max_z >= b.z) || (b.z >= bounds.min_z && bounds.max_z >= a.z)) ||
                    WallSplCount >= 64)
                    continue;
                // Native header+4 is read and advanced unchecked; its valid storage is a caller/native-state domain.
                end_header->wall_cursor->position = a;
                end_header->wall_cursor->material[0] = static_cast<u8>(node->field_02);
                end_header->wall_cursor->material[1] = mask;
                end_header->wall_cursor->material[2] = 0;
                end_header->wall_cursor->material[3] = 0;
                ++end_header->wall_cursor;
                end_header->wall_cursor->position = b;
                end_header->wall_cursor->material[0] = static_cast<u8>(node->field_02);
                end_header->wall_cursor->material[1] = mask;
                end_header->wall_cursor->material[2] = 0;
                end_header->wall_cursor->material[3] = 0;
                ++end_header->wall_cursor;
            }
        }
    }
    end_header->wall_end.shape_count = 0;
    end_header->wall_end.terrain_group = 0;
    TempStackPtrPc = reinterpret_cast<u8 *>(end_header + 1);
    NuScratchRelease();
    return result;
}

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
