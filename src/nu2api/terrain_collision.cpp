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
    i32 HitTerrain() {
        u8 *scan = reinterpret_cast<u8 *>(TerrOverRideScan);
        if (scan == NULL) {
            scan = ((TerrainQuery_s *)TerI)->scan_list_storage;
        }

        ((TerrainQuery_s *)TerI)->hit_time = 999.9f;
        const f32 radius = ((TerrainQuery_s *)TerI)->collision_radius;
        ((TerrainQuery_s *)TerI)->shape_adjusted = 0;
        ((TerrainQuery_s *)TerI)->hit_type = TERRAIN_HIT_TYPE_NONE;
        ((TerrainQuery_s *)TerI)->terrain_group_index = -1;

        HitWallSpline();

        ((TerrainQuery_s *)TerI)->horizontal_movement_length =
            NuFsqrt(((TerrainQuery_s *)TerI)->movement.x * ((TerrainQuery_s *)TerI)->movement.x +
                    ((TerrainQuery_s *)TerI)->movement.z * ((TerrainQuery_s *)TerI)->movement.z);

        i32 collision_found = 0;
        f32 primary_start_distance = 0.0f;
        f32 secondary_start_distance = 0.0f;
        f32 secondary_end_distance = 0.0f;
        const f32 negative_radius = -radius;

        i16 shape_count = *reinterpret_cast<i16 *>(scan);
        for (;;) {
            while (shape_count > 0) {
                // A scan group packs its count and terrain-group index into the first
                // pointer-sized slot, followed by the selected shape pointers.
                TERRAIN_GROUP *group =
                    &((TERRAIN_SET_PC *)CurrentTerrain)->groups[*reinterpret_cast<i16 *>(scan + sizeof(i16))];
                TERRAIN_SHAPE **scan_entries = reinterpret_cast<TERRAIN_SHAPE **>(scan);
                TERRAIN_SHAPE **surfaces = scan_entries + 1;

                TerrainQuery_s *query = (TerrainQuery_s *)TerI;
                query->local_start.x = query->position.x - group->origin.x;
                query->local_start.y = query->position.y - group->origin.y;
                query->local_start.z = query->position.z - group->origin.z;
                query->local_end.y = query->position.y + query->movement.y - group->origin.y;
                query->local_end.x = query->position.x + query->movement.x - group->origin.x;
                query->local_end.z = query->position.z + query->movement.z - group->origin.z;

                const i32 last_shape_index = shape_count - 1;
                i32 shape_index = 0;
                for (;;) {
                    TERRAIN_SHAPE *surface = scan_entries[shape_index + 1];
                    const NUVEC &primary_point = surface->vertices[0];
                    const NUVEC &primary_normal = surface->normals[0];

                    f32 primary_end_distance = (query->local_end.x - primary_point.x) * primary_normal.x +
                                               (query->local_end.y - primary_point.y) * primary_normal.y +
                                               (query->local_end.z - primary_point.z) * primary_normal.z - radius -
                                               query->compare_epsilon;

                    i32 crosses_surface = 0;
                    if (0.0f > primary_end_distance) {
                        primary_start_distance = (query->local_start.x - primary_point.x) * primary_normal.x +
                                                 (query->local_start.y - primary_point.y) * primary_normal.y +
                                                 (query->local_start.z - primary_point.z) * primary_normal.z - radius;
                        crosses_surface = primary_start_distance > negative_radius;
                    }

                    const NUVEC &secondary_point = surface->vertices[3];
                    const NUVEC &secondary_normal = surface->normals[1];
                    if (65536.0f > secondary_normal.y) {
                        secondary_end_distance = (query->local_end.x - secondary_point.x) * secondary_normal.x +
                                                 (query->local_end.y - secondary_point.y) * secondary_normal.y +
                                                 (query->local_end.z - secondary_point.z) * secondary_normal.z -
                                                 radius - query->compare_epsilon;
                        if (0.0f > secondary_end_distance) {
                            secondary_start_distance = (query->local_start.x - secondary_point.x) * secondary_normal.x +
                                                       (query->local_start.y - secondary_point.y) * secondary_normal.y +
                                                       (query->local_start.z - secondary_point.z) * secondary_normal.z -
                                                       radius;
                            if (secondary_start_distance > negative_radius) {
                                crosses_surface = 1;
                            }
                        }
                    }

                    if (crosses_surface != 0 &&
                        HitPoly(primary_start_distance, primary_end_distance, secondary_start_distance,
                                secondary_end_distance, surface) != 0) {
                        query = (TerrainQuery_s *)TerI;
                        query->terrain_group_index = *reinterpret_cast<u16 *>(scan + sizeof(i16));
                        collision_found = 1;
                        TERRAIN_SHAPE *hit_surface = query->surface;
                        if ((query->hit_type & TERRAIN_HIT_TYPE_SECOND_NORMAL) != 0 &&
                            (hit_surface->flags & TERRAIN_SHAPE_FLAG_MATERIAL_ALIAS) != 0) {
                            PlatCrush = hit_surface->material[0] + 1;
                        }
                    }
                    query = (TerrainQuery_s *)TerI;
                    if (shape_index == last_shape_index) {
                        break;
                    }
                    ++shape_index;
                }

                const i16 next_shape_count = *reinterpret_cast<i16 *>(surfaces + shape_index + 1);
                scan = reinterpret_cast<u8 *>(surfaces + shape_count);
                shape_count = next_shape_count;
            }

            if (shape_count == 0) {
                break;
            }
            do {
                scan += (1 - shape_count) * sizeof(TERRAIN_SHAPE *);
                shape_count = *reinterpret_cast<i16 *>(scan);
            } while (shape_count < 0);
        }

        NUVEC sphere_position;
        for (i32 sphere_index = 0; sphere_index < curSphereter; ++sphere_index) {
            const TERRAIN_SPHERE &sphere = SphereData[sphere_index];
            const f32 scaled_collision_radius =
                ((TerrainQuery_s *)TerI)->collision_radius * ((TerrainQuery_s *)TerI)->object_scale;
            sphere_position = sphere.position;
            sphere_position.y *= ((TerrainQuery_s *)TerI)->inverse_object_scale;
            sphere_position.y -= scaled_collision_radius;
            DeRotatePoint(&sphere_position);
            collision_found |= CheckSphereTer(&sphere_position, sphere.radius);
        }

        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        if (query->hit_type != TERRAIN_HIT_TYPE_NONE && query->terrain_group_index != -1) {
            TERRAIN_GROUP *group = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[query->terrain_group_index];
            if (group->chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY) {
                plathitid = group->scene_index;
            }
        }

        TerrOverRideScan = NULL;
        return collision_found;
    }
    void NewTerrStoreAnyInfo() {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        TERRAIN_SHAPE *surface = query->surface;
        if (surface == NULL || query->terrain_group_index == -1) {
            return;
        }

        if (surface->material[0] != 0) {
            TerrPolyInfo.material[0] = surface->material[0];
        }
        if (surface->material[1] != 0) {
            TerrPolyInfo.material[1] = surface->material[1];
        }
        if (surface->flags != 0) {
            TerrPolyInfo.flags = surface->flags;
        }
        if (surface->normal_flags != 0) {
            TerrPolyInfo.normal_flags = surface->normal_flags;
        }
    }
    void TerrainMoveImpactData() {
        const i32 stored_class_mask = (1 << TERRAIN_HIT_TYPE_FACE) | (1 << TERRAIN_HIT_TYPE_CYLINDER) |
                                      (1 << TERRAIN_HIT_TYPE_VERTEX) | (1 << TERRAIN_HIT_TYPE_SPHERE);
        const i32 stored_hit_type_mask = stored_class_mask | (stored_class_mask << TERRAIN_HIT_TYPE_SECOND_NORMAL);
        const u8 maximum_stored_hit_type = TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE;

        if (((TerrainQuery_s *)TerI)->hit_type > maximum_stored_hit_type) {
            return;
        }

        i32 hit_type_flag = 1 << ((TerrainQuery_s *)TerI)->hit_type;
        hit_type_flag &= stored_hit_type_mask;
        if (hit_type_flag == 0 || ((TerrainQuery_s *)TerI)->terrain_group_index == -1) {
            return;
        }

        NewTerrStoreAnyInfo();
        TerrPoly = &TerrPolyInfo;
        TerrainShadowStatusPc = ((TerrainQuery_s *)TerI)->terrain_group_index;
    }
    void TerrainImpactNorm() {
        TerrainMoveImpactData();

        const u8 maximum_supported_hit_type = TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE;
        if (((TerrainQuery_s *)TerI)->hit_type > maximum_supported_hit_type) {
            return;
        }

        const i32 hit_type_flag = 1 << ((TerrainQuery_s *)TerI)->hit_type;
        const i32 sphere_class_mask = 1 << TERRAIN_HIT_TYPE_SPHERE;
        const i32 rotate_and_mark_mask = sphere_class_mask | (sphere_class_mask << TERRAIN_HIT_TYPE_SECOND_NORMAL);
        const i32 rotated_surface_mask = (1 << TERRAIN_HIT_TYPE_CYLINDER) | (1 << TERRAIN_HIT_TYPE_VERTEX);
        const i32 rotate_mask = rotated_surface_mask | (rotated_surface_mask << TERRAIN_HIT_TYPE_SECOND_NORMAL);
        const i32 face_class_mask = 1 << TERRAIN_HIT_TYPE_FACE;
        const i32 direct_normal_mask = face_class_mask | (face_class_mask << TERRAIN_HIT_TYPE_SECOND_NORMAL);

        if ((hit_type_flag & rotate_and_mark_mask) != 0) {
            terrhitflags |= 4;
        }

        const bool rotated_hit = (hit_type_flag & (rotate_and_mark_mask | rotate_mask)) != 0;
        if (rotated_hit) {
            RotateVec(&((TerrainQuery_s *)TerI)->movement_normal, &((TerrainQuery_s *)TerI)->movement_normal);
        } else if ((hit_type_flag & direct_normal_mask) == 0) {
            return;
        }

        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        // Curved hits are produced in collision-height-scaled space and need to
        // be transformed back. Face normals already come from the terrain in
        // object space; the target's direct-face branch deliberately bypasses
        // this scaling before copying the normal below.
        if (rotated_hit && (query->hit_type & TERRAIN_HIT_TYPE_SECOND_NORMAL) == 0) {
            query->movement_normal.x *= query->inverse_collision_radius;
            query->movement_normal.y *= query->inverse_collision_radius;
            query->movement_normal.z *= query->inverse_collision_radius;
        }

        if (query->object_scale == 1.0f) {
            query->impact_normal = query->movement_normal;
            return;
        }

        const f32 normal_length =
            NuFsqrt(query->movement_normal.x * query->movement_normal.x +
                    query->movement_normal.y * query->movement_normal.y * query->inverse_object_scale_sq +
                    query->movement_normal.z * query->movement_normal.z);
        f32 inverse_normal_length = 0.0f;
        if (normal_length != 0.0f) {
            inverse_normal_length = 1.0f / normal_length;
        }

        query = (TerrainQuery_s *)TerI;
        query->impact_normal.x = query->movement_normal.x * inverse_normal_length;
        query->impact_normal.y = query->movement_normal.y * query->inverse_object_scale * inverse_normal_length;
        query->impact_normal.z = query->movement_normal.z * inverse_normal_length;
    }
    void StorePlatImpact() {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        const u8 hit_type = query->hit_type;
        if (hit_type == TERRAIN_HIT_TYPE_NONE || query->terrain_group_index == -1) {
            return;
        }

        TERRAIN_GROUP *group = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[query->terrain_group_index];
        if (group->chunk_type != TERRAIN_CHUNK_GROUP_SECONDARY) {
            return;
        }

        TerrainShadowGroupIndexPc = query->terrain_group_index;
        NUVEC impact_normal = query->movement_normal;

        const u8 maximum_supported_hit_type = TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE;
        if (hit_type > maximum_supported_hit_type) {
            return;
        }

        const i32 hit_type_flag = 1 << hit_type;
        const i32 rotated_class_mask =
            (1 << TERRAIN_HIT_TYPE_CYLINDER) | (1 << TERRAIN_HIT_TYPE_VERTEX) | (1 << TERRAIN_HIT_TYPE_SPHERE);
        const i32 rotated_hit_mask = rotated_class_mask | (rotated_class_mask << TERRAIN_HIT_TYPE_SECOND_NORMAL);
        const i32 face_class_mask = 1 << TERRAIN_HIT_TYPE_FACE;
        const i32 direct_hit_mask = face_class_mask | (face_class_mask << TERRAIN_HIT_TYPE_SECOND_NORMAL);
        if ((hit_type_flag & rotated_hit_mask) != 0) {
            RotateVec(&impact_normal, &impact_normal);
            query = (TerrainQuery_s *)TerI;
            if ((query->hit_type & TERRAIN_HIT_TYPE_SECOND_NORMAL) == 0) {
                impact_normal.x *= query->inverse_collision_radius;
                impact_normal.y *= query->inverse_collision_radius;
                impact_normal.z *= query->inverse_collision_radius;
            }
        } else if ((hit_type_flag & direct_hit_mask) == 0) {
            return;
        }

        const f32 unit_scale = 1.0f;
        if (query->object_scale != unit_scale) {
            const f32 normal_length = NuFsqrt(impact_normal.x * impact_normal.x +
                                              impact_normal.y * impact_normal.y * query->inverse_object_scale_sq +
                                              impact_normal.z * impact_normal.z);
            f32 inverse_normal_length = 0.0f;
            if (normal_length != 0.0f) {
                inverse_normal_length = unit_scale / normal_length;
            }

            query = (TerrainQuery_s *)TerI;
            impact_normal.x *= inverse_normal_length;
            impact_normal.y *= query->inverse_object_scale * inverse_normal_length;
            impact_normal.z *= inverse_normal_length;
        }

        query = (TerrainQuery_s *)TerI;
        PlatImpactId = ((TERRAIN_SET_PC *)CurrentTerrain)->groups[query->terrain_group_index].scene_index;
        PlatImpactNorm = impact_normal;
        PlatImpactTer = *query->surface;
    }
    void TerrainImpact(NUVEC *position, NUVEC *movement, u8 *hit_flags) {
        TerrainMoveImpactData();

        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        u8 hit_type = query->hit_type;
        f32 walkable_normal_y = 0.0f;

        if (hit_type == TERRAIN_HIT_TYPE_NONE) {
            hit_flags[0] = 0;
            position->x = query->position.x + query->movement.x;
            position->y = (query->position.y + query->movement.y) * query->object_scale -
                          query->object_scale * query->collision_radius;
            position->z = query->position.z + query->movement.z;
            query->flags &= ~TERRAIN_QUERY_FLAG_PREVIOUS_NORMAL;
        } else {
            TERRAIN_SHAPE *surface = query->surface;
            f32 surface_alignment = 0.707f;
            if (surface != NULL) {
                surface_alignment = query->movement_normal.x * surface->normals[0].x +
                                    query->movement_normal.y * surface->normals[0].y +
                                    query->movement_normal.z * surface->normals[0].z;
            }

            TERRAIN_GROUP *group = NULL;
            if (query->terrain_group_index != -1) {
                group = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[query->terrain_group_index];
            }
            const bool platform_group = group != NULL && group->chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY;

            if (platform_group) {
                const u8 base_hit_type = hit_type & TERRAIN_HIT_TYPE_CLASS_MASK;
                if (base_hit_type > TERRAIN_HIT_TYPE_FACE && surface_alignment < 0.95f) {
                    walkable_normal_y = 0.98f;
                } else {
                    walkable_normal_y = 0.707f;
                }
            } else {
                walkable_normal_y = query->shape_adjusted != 0 ? 1.1f : -1.1f;
            }

            // A non-negative group index on a hit always comes with a surface.
            const bool wall_override_surface =
                query->terrain_group_index >= 0 &&
                (surface->normal_flags & TERRAIN_SURFACE_CLASS_MASK) == TERRAIN_SURFACE_CLASS_WALL_OVERRIDE;
            if (wall_override_surface) {
                // Original 0x382168 uses the scaled movement normal (+0xa8).
                query->position.x += query->movement_normal.x * 0.0015f;
                query->position.z += query->movement_normal.z * 0.0015f;
                walkable_normal_y = 1.1f;
            }
            if (wallover != 0.0f) {
                walkable_normal_y = wallover;
            }

            switch (hit_type) {
                case TERRAIN_HIT_TYPE_CYLINDER:
                case TERRAIN_HIT_TYPE_VERTEX:
                case TERRAIN_HIT_TYPE_SPHERE:
                    if (!wall_override_surface)
                        walkable_normal_y = 0.707f;
                    // Fall through to the shared first-normal response.
                case TERRAIN_HIT_TYPE_FACE: {

                    query->hit_time -= query->separation_epsilon;
                    if (query->hit_time < 0.0f) {
                        query->hit_time = 0.0f;
                    }

                    const f32 advance = query->hit_time;
                    const f64 travelled_x = (f64)query->movement.x * advance;
                    const f64 travelled_y = (f64)query->movement.y * advance;
                    const f64 travelled_z = (f64)query->movement.z * advance;
                    query->position.x += travelled_x;
                    query->position.y += travelled_y;
                    query->position.z += travelled_z;
                    query->movement.x -= travelled_x;
                    query->movement.y -= travelled_y;
                    query->movement.z -= travelled_z;

                    if (walkable_normal_y > query->impact_normal.y) {
                        hit_flags[0] = 0;
                        terrhitflags |= TERRAIN_IMPACT_RESULT_WALL;
                        if (!platform_group) {
                            terrhitflags |= TERRAIN_IMPACT_RESULT_STATIC_WALL;
                        }

                        query->position.x += query->movement_normal.x * 0.0006f;
                        query->position.z += query->movement_normal.z * 0.0006f;
                        query->movement.x += query->movement_normal.x * 0.0006f;
                        query->movement.z += query->movement_normal.z * 0.0006f;
                        movement->x += query->movement_normal.x * 0.0009f;
                        movement->z += query->movement_normal.z * 0.0009f;

                        FullDeflect(&query->movement_normal, &query->movement, &query->movement);
                        FullDeflectSmallY(&query->impact_normal, movement, movement);

                        query = (TerrainQuery_s *)TerI;
                        if (CurTrackInfo == NULL) {
                            CurTrackInfo = AllocTerrId();
                            CurTrackInfo->flags = TERRAIN_TRACK_FLAG_NONE;
                            CurTrackInfo->id = query->hit_flags;
                            CurTrackInfo->platform_index = 0;
                            CurTrackInfo->platform_contact_state = TERRAIN_TRACK_CONTACT_NONE;
                        }
                        CurTrackInfo->wall_contact_state = TERRAIN_TRACK_CONTACT_ACTIVE;
                        CurTrackInfo->impact_normal = query->impact_normal;
                        TerrWallNorm = query->impact_normal;
                    } else {
                        FullDeflect(&query->movement_normal, &query->movement, &query->movement);
                        FullDeflect(&query->impact_normal, movement, movement);

                        terrhitflags |= TERRAIN_IMPACT_RESULT_GROUND;
                        hit_flags[0] = 1;
                        hit_flags[1] = 1;
                        query->position.y += query->movement_normal.y * 0.0003f;

                        query = (TerrainQuery_s *)TerI;
                        if (query->terrain_group_index != -1) {
                            group = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[query->terrain_group_index];
                            if (group->chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY) {
                                ((TERRAIN_SET_PC *)CurrentTerrain)->platforms[group->scene_index].flags |=
                                    TERRAIN_PLATFORM_FLAG_COLLIDED;
                            }
                        }
                    }

                    query = (TerrainQuery_s *)TerI;
                    query->flags &= ~TERRAIN_QUERY_FLAG_PREVIOUS_NORMAL;

                    break;
                }
                case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_CYLINDER:
                case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_VERTEX:
                case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE:
                    walkable_normal_y = 0.707f;
                    // Fall through to the shared second-normal response.
                case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_FACE: {

                    NUVEC response_normal = query->movement_normal;
                    if ((query->flags & TERRAIN_QUERY_FLAG_PREVIOUS_NORMAL) != 0) {
                        const f32 previous_alignment = response_normal.x * query->previous_movement_normal.x +
                                                       response_normal.y * query->previous_movement_normal.y +
                                                       response_normal.z * query->previous_movement_normal.z;
                        if (previous_alignment < 0.0f) {
                            NUVEC corrected_normal;
                            FullDeflect(&response_normal, &query->previous_movement_normal, &corrected_normal);
                            const f32 correction_alignment = corrected_normal.x * response_normal.x +
                                                             corrected_normal.y * response_normal.y +
                                                             corrected_normal.z * response_normal.z;
                            if (correction_alignment > 0.25f) {
                                response_normal.x = corrected_normal.x / correction_alignment;
                                response_normal.y = corrected_normal.y / correction_alignment;
                                response_normal.z = corrected_normal.z / correction_alignment;
                            }
                        }
                    }

                    query = (TerrainQuery_s *)TerI;
                    const f64 contact_push = (f64)0.0035f - (f64)query->unclamped_hit_time * (f64)1.05f;
                    if (walkable_normal_y > query->impact_normal.y) {
                        const f64 movement_push = (f64)0.0035f - (f64)query->unclamped_hit_time * (f64)0.35f;
                        query->position.x += response_normal.x * contact_push;
                        query->position.y += response_normal.y * contact_push;
                        query->position.z += response_normal.z * contact_push;
                        query->movement.x += response_normal.x * movement_push;
                        query->movement.y += response_normal.y * movement_push;
                        query->movement.z += response_normal.z * movement_push;
                        hit_flags[0] = 1;

                        FullDeflect(&response_normal, &query->movement, &query->movement);
                        FullDeflectSmallY(&query->impact_normal, movement, movement);

                        query = (TerrainQuery_s *)TerI;
                        if (CurTrackInfo == NULL) {
                            CurTrackInfo = AllocTerrId();
                            CurTrackInfo->flags = TERRAIN_TRACK_FLAG_NONE;
                            CurTrackInfo->id = query->hit_flags;
                            CurTrackInfo->platform_index = 0;
                            CurTrackInfo->platform_contact_state = TERRAIN_TRACK_CONTACT_NONE;
                        }
                        CurTrackInfo->wall_contact_state = TERRAIN_TRACK_CONTACT_ACTIVE;
                        CurTrackInfo->impact_normal = query->impact_normal;
                    } else {
                        query->position.x += response_normal.x * contact_push;
                        query->position.y += response_normal.y * contact_push;
                        query->position.y += response_normal.y * 0.01f;
                        query->position.z += response_normal.z * contact_push;
                        query->movement.y += response_normal.y * 0.01f;

                        FullDeflect(&response_normal, &query->movement, &query->movement);
                        FullDeflect(&query->impact_normal, movement, movement);
                    }

                    query = (TerrainQuery_s *)TerI;
                    query->flags |= TERRAIN_QUERY_FLAG_PREVIOUS_NORMAL;
                    query->previous_movement_normal = query->movement_normal;

                    break;
                }
            }
        }

        query = (TerrainQuery_s *)TerI;
        hit_type = query->hit_type;
        if (hit_type >= TERRAIN_HIT_TYPE_FACE && hit_type <= TERRAIN_HIT_TYPE_CLASS_MASK &&
            query->terrain_group_index != -1) {
            TERRAIN_GROUP *group = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[query->terrain_group_index];
            if (group->chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY && query->impact_normal.y >= walkable_normal_y) {
                PlatformConnect(reinterpret_cast<char *>(query->hit_flags), &query->movement, movement,
                                group->scene_index);
            }
        }

        if (TerImpactData == NULL || *TerImpactDataCount >= TerImpactDataMax) {
            return;
        }

        query = (TerrainQuery_s *)TerI;
        if (query->hit_type == TERRAIN_HIT_TYPE_NONE) {
            return;
        }

        TERRAIN_IMPACT_RECORD *records = static_cast<TERRAIN_IMPACT_RECORD *>(TerImpactData);
        TERRAIN_IMPACT_RECORD &record = records[*TerImpactDataCount];
        record.position.x = query->position.x - query->movement_normal.x * query->collision_radius;
        record.position.y =
            (query->position.y - query->movement_normal.y * query->collision_radius) * query->object_scale;
        record.position.z = query->position.z - query->movement_normal.z * query->collision_radius;
        record.normal = query->movement_normal;

        if (query->surface != NULL) {
            record.material[0] = query->surface->material[0];
            record.material[1] = query->surface->material[1];
            record.flags = query->surface->flags;
            record.normal_flags = query->surface->normal_flags;
        } else if (TerrWallInfo != 0) {
            record.material[0] = TerrWallTab[0];
            record.material[1] = TerrWallTab[1];
            record.flags = 0;
            record.normal_flags = 0;
        }
        ++*TerImpactDataCount;
    }
    i32 TerrainImpactPlatform(unsigned char *hit_flags) {
        TerrainMoveImpactData();

        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        if (query->hit_type == TERRAIN_HIT_TYPE_NONE) {
            hit_flags[0] = 0;
            query->position.x += query->movement.x;
            query->position.y += query->movement.y;
            query->position.z += query->movement.z;
            return 0;
        }

        const u8 maximum_platform_hit_type = TERRAIN_HIT_TYPE_SPHERE;
        if (query->hit_type > maximum_platform_hit_type) {
            return 1;
        }

        query->hit_time -= query->separation_epsilon;
        if (query->hit_time < 0.0f) {
            query->hit_time = 0.0f;
        }

        query->position.x += query->movement.x * query->hit_time;
        query->position.y += query->movement.y * query->hit_time;
        query->position.z += query->movement.z * query->hit_time;

        const f32 minimum_walkable_normal_y = 0.707f;
        if (query->impact_normal.y < minimum_walkable_normal_y) {
            hit_flags[0] = 0;
            return 0;
        }

        hit_flags[0] = 1;
        hit_flags[1] = 1;
        query->position.y += query->movement_normal.y * 0.004f;
        return 0;
    }
    void FullDeflect(NUVEC *normal, NUVEC *movement, NUVEC *result) {
        const f32 normal_x = normal->x;
        const f32 normal_y = normal->y;
        const f32 normal_z = normal->z;
        const f32 movement_x = movement->x;
        const f32 movement_y = movement->y;
        const f32 movement_z = movement->z;
        const f32 deflection = -movement_y * normal_y - movement_x * normal_x - movement_z * normal_z + 0.0003f;
        result->x = movement_x + normal_x * deflection;
        result->y = movement_y + normal_y * deflection;
        result->z = movement_z + normal_z * deflection;
    }
    void FullDeflectSmallY(NUVEC *normal, NUVEC *movement, NUVEC *result) {
        // Move just far enough out of the surface to retain a small separation.
        const f32 normal_x = normal->x;
        const f32 normal_y = normal->y;
        const f32 normal_z = normal->z;
        const f32 movement_x = movement->x;
        const f32 movement_y = movement->y;
        const f32 movement_z = movement->z;
        const f32 deflection = -movement_y * normal_y - movement_x * normal_x - movement_z * normal_z + 0.0003f;
        result->x = movement_x + normal_x * deflection;
        result->y = movement_y + normal_y * deflection * TerrWallDeflectYScale;
        result->z = movement_z + normal_z * deflection;
    }
    i32 FullDeflectTest(NUVEC *normal, NUVEC *movement, NUVEC *result) {
        f32 deflection = ((-movement->y * normal->y - movement->x * normal->x) - movement->z * normal->z) + 0.0003f;
        NUVEC output = {normal->x * deflection + movement->x, normal->y * deflection + movement->y,
                        normal->z * deflection + movement->z};
        *result = output;
        return deflection > 0.0f;
    }
    void PlatformConnect(char *track_id, NUVEC *position_delta, NUVEC *movement_delta, i32 platform_index) {
        if (CurTrackInfo == NULL) {
            CurTrackInfo = AllocTerrId();
            if (CurTrackInfo != NULL) {
                CurTrackInfo->flags |= TERRAIN_TRACK_FLAG_CONNECTED;
                CurTrackInfo->platform_index = static_cast<i16>(platform_index);
                CurTrackInfo->id = track_id;
                CurTrackInfo->platform_contact_state = TERRAIN_TRACK_CONTACT_ACTIVE;
                position_delta->y = 0.0f;
                movement_delta->y = 0.0f;
            }
        } else {
            CurTrackInfo->flags |= TERRAIN_TRACK_FLAG_CONNECTED;
            CurTrackInfo->platform_index = static_cast<i16>(platform_index);
            CurTrackInfo->platform_contact_state = TERRAIN_TRACK_CONTACT_ACTIVE;
        }

        TerrainShadowGroupIndexPc = ((TERRAIN_SET_PC *)CurrentTerrain)->platforms[platform_index].terrain_group_index;
    }
    i32 PlatformChecks(i32 count, NUVEC *movement) {
        TerrainQuery_s *old_query = (TerrainQuery_s *)TerI;
        if (old_query->scan_list == NULL || *static_cast<i16 *>(old_query->scan_list) == 0 || CurTrackInfo == NULL ||
            CurTrackInfo->platform_contact_state == 0 || TerConTol == 0.0f)
            return count;
        TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
        TerI = query;
        u8 *input = static_cast<u8 *>(old_query->scan_list);
        u8 *output = query->scan_list_storage;
        while (*reinterpret_cast<i16 *>(input) > 0) {
            i16 *input_header = reinterpret_cast<i16 *>(input);
            i16 *output_header = reinterpret_cast<i16 *>(output);
            i32 shape_count = input_header[0];
            output_header[0] = input_header[0];
            output_header[1] = input_header[1];
            TERRAIN_SHAPE **input_shapes = reinterpret_cast<TERRAIN_SHAPE **>(input) + 1;
            TERRAIN_SHAPE **output_shapes = reinterpret_cast<TERRAIN_SHAPE **>(output) + 1;
            for (i32 i = 0; i < shape_count; ++i) {
                *output_shapes++ = *input_shapes++;
            }
            input = reinterpret_cast<u8 *>(input_shapes);
            output = reinterpret_cast<u8 *>(output_shapes);
        }
        reinterpret_cast<i16 *>(output)[0] = 0;
        reinterpret_cast<i16 *>(output)[1] = 0;
        query->collision_radius = old_query->collision_radius;
        query->object_scale = old_query->object_scale;
        query->inverse_object_scale_sq = old_query->inverse_object_scale_sq;
        query->inverse_collision_radius = old_query->inverse_collision_radius;
        query->separation_epsilon = old_query->separation_epsilon;
        query->inverse_object_scale = old_query->inverse_object_scale;
        query->compare_epsilon = old_query->compare_epsilon;
        query->collision_radius_sq = old_query->collision_radius_sq;
        query->object_scale_sq = old_query->object_scale_sq;
        query->platform_position = old_query->position;
        if (CurTrackInfo != NULL && (CurTrackInfo->flags & TERRAIN_TRACK_FLAG_CONNECTED)) {
            TERRAIN_PLATFORM *platform = &((TERRAIN_SET_PC *)CurrentTerrain)->platforms[CurTrackInfo->platform_index];
            NUMTX *matrix = static_cast<NUMTX *>(platform->scene_object);
            NUMTX *previous = reinterpret_cast<NUMTX *>(&platform->previous_matrix);
            if (platform->flags & 1) {
                NuMtxInvRSS(&tertempmat, previous);
                tertempvec4.x = old_query->position.x - previous->m30;
                tertempvec4.y = (old_query->position.y - query->collision_radius) * query->object_scale - previous->m31;
                tertempvec4.z = old_query->position.z - previous->m32;
                tertempvec4.w = 1.0f;
                NuVec4MtxTransformVU0(&tertempvec4, &tertempvec4, &tertempmat);
                NuVec4MtxTransformVU0(&tertempvec4, &tertempvec4, matrix);
                query->platform_position.x = tertempvec4.x;
                query->platform_position.y = tertempvec4.y * query->inverse_object_scale + query->collision_radius;
                query->platform_position.z = tertempvec4.z;
            } else {
                query->platform_position.x += matrix->m30 - previous->m30;
                query->platform_position.y += (matrix->m31 - previous->m31) * old_query->inverse_object_scale;
                query->platform_position.z += matrix->m32 - previous->m32;
            }
        }
        query->position = query->platform_position;
        query->platform_normal = query->platform_position;
        query->movement.x = 0.0f;
        query->movement.y = TerConTol * -2.0f;
        query->movement.z = 0.0f;
        query->position.y += TerConTol;
        DerotateMovementVector();
        f32 saved_epsilon = ((TerrainQuery_s *)TerI)->separation_epsilon;
        ((TerrainQuery_s *)TerI)->separation_epsilon = 0.0f;
        HitTerrain();
        StorePlatImpact();
        ((TerrainQuery_s *)TerI)->separation_epsilon = saved_epsilon;
        TerrainImpactNorm();
        u8 flags[2];
        TerrainImpactPlatform(flags);
        if (((TerrainQuery_s *)TerI)->hit_type != 0)
            ShadNorm = ((TerrainQuery_s *)TerI)->impact_normal;
        ((TerrainQuery_s *)TerI)->platform_position.y =
            ((TerrainQuery_s *)TerI)->position.y - ((TerrainQuery_s *)TerI)->collision_radius;
        TerI = old_query;
        TERRAIN_FLOAT_BITS_PC absolute_4;
        absolute_4.value = ((query->platform_normal.y - query->platform_position.y) - old_query->collision_radius);
        absolute_4.bits &= 0x7fffffff;

        if (query->hit_type == 0 || absolute_4.value > TerConTol * 0.5f || query->movement_normal.y < 0.707f) {
            NuScratchRelease();
            return count;
        }
        TERRAIN_GROUP *group = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[query->terrain_group_index];
        i32 platform_index = group->scene_index;
        if (movement->y > TerConTol * 0.5f) {
            TERRAIN_PLATFORM *platform = &((TERRAIN_SET_PC *)CurrentTerrain)->platforms[platform_index];
            NUMTX *matrix = static_cast<NUMTX *>(platform->scene_object);
            if (movement->y > (TerConTol * 0.5f + matrix->m31) - platform->previous_matrix.m31) {
                NuScratchRelease();
                return count;
            }
        }
        if (query->movement_normal.y >= 0.707f && query->terrain_group_index >= 0 && group->chunk_type == 1)
            ((TERRAIN_SET_PC *)CurrentTerrain)->platforms[platform_index].flags |= 2;
        old_query->platform_position.x = query->platform_position.x;
        old_query->platform_position.y = query->platform_position.y + old_query->collision_radius;
        if (!(((TERRAIN_SET_PC *)CurrentTerrain)->platforms[CurTrackInfo->platform_index].flags & 1))
            old_query->platform_position.y += 0.0025f;
        old_query->platform_position.z = query->platform_position.z;
        NUVEC direction = {old_query->platform_position.x - old_query->position.x,
                           old_query->platform_position.y - old_query->position.y,
                           old_query->platform_position.z - old_query->position.z};
        f32 distance = NuFsqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
        direction.x = distance == 0.0f || direction.x == 0.0f ? 0.0f : direction.x / distance;
        direction.y = distance == 0.0f || direction.y == 0.0f ? 0.0f : direction.y / distance;
        direction.z = distance == 0.0f || direction.z == 0.0f ? 0.0f : direction.z / distance;
        NUVEC previous_position = ((TerrainQuery_s *)TerI)->position;
        i32 clear = TerrainPlatformMoveCheck(&((TerrainQuery_s *)TerI)->platform_position,
                                             &((TerrainQuery_s *)TerI)->platform_normal, platform_index, 1, 0);
        bool opposing_contact = false;
        if (((TerrainQuery_s *)TerI)->hit_type != 0 && ((TerrainQuery_s *)TerI)->terrain_group_index >= 0) {
            TERRAIN_GROUP *hit_group =
                &((TERRAIN_SET_PC *)CurrentTerrain)->groups[((TerrainQuery_s *)TerI)->terrain_group_index];
            if (hit_group->chunk_type == 1)
                CurTrackInfo->platform_index = hit_group->scene_index;
            f32 dot = direction.x * ((TerrainQuery_s *)TerI)->platform_normal.x +
                      direction.y * ((TerrainQuery_s *)TerI)->platform_normal.y +
                      direction.z * ((TerrainQuery_s *)TerI)->platform_normal.z;
            opposing_contact = dot < TerrainOpposingContactThresholdPc; /* PC qword operand 76e7d8. */
        }
        --count;
        if (opposing_contact || clear != 0) {
            TerrPoly = query->surface;
            ((TerrainQuery_s *)TerI)->position = ((TerrainQuery_s *)TerI)->platform_position;
            if (movement->y < 0.0f) {
                FullDeflect(&ShadNorm, &((TerrainQuery_s *)TerI)->movement, &((TerrainQuery_s *)TerI)->movement);
                FullDeflect(&ShadNorm, movement, movement);
            } else {
                ((TerrainQuery_s *)TerI)->movement.y *= 0.75f;
            }
            ((TerrainQuery_s *)TerI)->hit_flags[1] = 1;
            PlatformConnect(reinterpret_cast<char *>(old_query->hit_flags), &old_query->movement, movement,
                            ((TERRAIN_SET_PC *)CurrentTerrain)->groups[query->terrain_group_index].scene_index);
        } else if (count > 0) {
            f32 travelled = (((TerrainQuery_s *)TerI)->position.x - previous_position.x) * direction.x +
                            (((TerrainQuery_s *)TerI)->position.y - previous_position.y) * direction.y +
                            (((TerrainQuery_s *)TerI)->position.z - previous_position.z) * direction.z;
            if (distance > travelled && FullDeflectTest(&((TerrainQuery_s *)TerI)->platform_normal, &direction,
                                                        &((TerrainQuery_s *)TerI)->platform_position)) {
                f32 dot = direction.x * ((TerrainQuery_s *)TerI)->platform_position.x +
                          direction.y * ((TerrainQuery_s *)TerI)->platform_position.y +
                          direction.z * ((TerrainQuery_s *)TerI)->platform_position.z;
                if (dot > 0.0f) {
                    f32 fraction = (distance - travelled) / dot;
                    if (fraction < 0.8f) {
                        ((TerrainQuery_s *)TerI)->platform_position.x =
                            ((TerrainQuery_s *)TerI)->platform_position.x * fraction +
                            ((TerrainQuery_s *)TerI)->position.x;
                        ((TerrainQuery_s *)TerI)->platform_position.y =
                            ((TerrainQuery_s *)TerI)->platform_position.y * fraction +
                            ((TerrainQuery_s *)TerI)->position.y;
                        ((TerrainQuery_s *)TerI)->platform_position.z =
                            ((TerrainQuery_s *)TerI)->platform_position.z * fraction +
                            ((TerrainQuery_s *)TerI)->position.z;
                    }
                }
            }
        }
        NuScratchRelease();
        TerI = old_query;
        return count;
    }
    i32 TerrainPlatformEmbedded(NUVEC *movement_delta) {
        TerrainQuery_s *old_query = (TerrainQuery_s *)TerI;
        i32 platform_index = ((TERRAIN_SET_PC *)CurrentTerrain)->groups[old_query->terrain_group_index].scene_index;
        TERRAIN_PLATFORM *platform = &((TERRAIN_SET_PC *)CurrentTerrain)->platforms[platform_index];
        NUMTX *matrix = static_cast<NUMTX *>(platform->scene_object);
        if (matrix == NULL)
            return 1;
        NUMTX *previous = reinterpret_cast<NUMTX *>(&platform->previous_matrix);
        if (previous->m30 == matrix->m30 && previous->m31 == matrix->m31 && previous->m32 == matrix->m32) {
            if (!(platform->flags & 1))
                return 1;
            if (previous->m00 == matrix->m00 && previous->m01 == matrix->m01 && previous->m02 == matrix->m02 &&
                previous->m10 == matrix->m10 && previous->m11 == matrix->m11 && previous->m12 == matrix->m12 &&
                previous->m20 == matrix->m20 && previous->m21 == matrix->m21 && previous->m22 == matrix->m22)
                return 1;
        }
        i32 saved_sphere = curSphereter;
        curSphereter = 0;
        TerrainScaledShapeCursorPc = static_cast<TERRAIN_SHAPE *>(ScaleTerrainT2);
        TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
        TerI = query;
        void *scratch = NuScratchAlloc32(0xd0);
        NUVEC4 *vertices = reinterpret_cast<NUVEC4 *>((reinterpret_cast<u32>(scratch) + 31) & ~u32(15));
        query->collision_radius = old_query->collision_radius;
        query->object_index = old_query->object_index;
        query->inverse_object_scale_sq = old_query->inverse_object_scale_sq;
        query->hit_flags = old_query->hit_flags;
        query->inverse_collision_radius = old_query->inverse_collision_radius;
        query->scan_result = old_query->scan_result;
        query->separation_epsilon = old_query->separation_epsilon;
        query->compare_epsilon = old_query->compare_epsilon;
        query->collision_radius_sq = old_query->collision_radius_sq;
        query->object_scale_sq = old_query->object_scale_sq;
        query->inverse_object_scale = old_query->inverse_object_scale;
        query->radius = old_query->radius;
        query->object_scale = old_query->object_scale;
        query->movement.x = (previous->m30 - matrix->m30) * 1.73f;
        query->movement.y = ((previous->m31 - matrix->m31) * 1.73f) * query->inverse_object_scale;
        query->movement.z = (previous->m32 - matrix->m32) * 1.73f;
        if (platform->flags & 1) {
            NuMtxInvRSS(&tertempmat, previous);
            tertempvec4.x = old_query->position.x - previous->m30;
            tertempvec4.y = (old_query->position.y - query->collision_radius) * query->object_scale - previous->m31;
            tertempvec4.z = old_query->position.z - previous->m32;
            tertempvec4.w = 1.0f;
            NuVec4MtxTransformVU0(&tertempvec4, &tertempvec4, &tertempmat);
            NuVec4MtxTransformVU0(&tertempvec4, &tertempvec4, matrix);
            query->movement.x = (old_query->position.x - tertempvec4.x) * 1.3f;
            query->movement.y =
                (old_query->position.y - (tertempvec4.y * query->inverse_object_scale + query->collision_radius)) *
                1.3f;
            query->movement.z = (old_query->position.z - tertempvec4.z) * 1.3f;
        }
        query->movement.x -= 0.01f * old_query->movement_normal.x;
        query->movement.y -= 0.01f * old_query->movement_normal.y;
        query->movement.z -= 0.01f * old_query->movement_normal.z;
        query->position.x = old_query->position.x - query->movement.x * 0.91f;
        query->position.y = old_query->position.y - query->movement.y * 0.91f;
        query->position.z = old_query->position.z - query->movement.z * 0.91f;

        TERRAIN_GROUP *group = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[old_query->terrain_group_index];
        TERRAIN_SHAPE **cursor = reinterpret_cast<TERRAIN_SHAPE **>(query->scan_list_storage + 4);
        i32 count = 0;
        i32 transformed_count = 0;
        TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
        if (query->object_scale == 1.0f) {
            if ((platform->flags & 1)) {
                while (batch->marker > -1) {
                    i32 shape_count = batch->shape_count;
                    TERRAIN_SHAPE *source = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                    for (i32 remaining = shape_count; remaining > 0; --remaining, ++source) {
                        if (source->material[1] != 0)
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
                        bool quad = source->normals[1].y < 65535.0f;
                        if (quad) {
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
                        TERRAIN_SHAPE *surface = &TerrainScaledShapeCursorPc[transformed_count];
                        memcpy(surface->material, source->material, 4);
                        surface->vertices[0].x = vertices[0].x;
                        surface->vertices[0].y = vertices[0].y;
                        surface->vertices[0].z = vertices[0].z;
                        surface->vertices[1].x = vertices[1].x;
                        surface->vertices[1].y = vertices[1].y;
                        surface->vertices[1].z = vertices[1].z;
                        surface->vertices[2].x = vertices[2].x;
                        surface->vertices[2].y = vertices[2].y;
                        surface->vertices[2].z = vertices[2].z;
                        if (quad) {
                            surface->vertices[3].x = vertices[3].x;
                            surface->vertices[3].y = vertices[3].y;
                            surface->vertices[3].z = vertices[3].z;
                        }
                        if (quad) {
                            vertices[0].x = surface->vertices[1].x - surface->vertices[3].x;
                            vertices[0].y = surface->vertices[1].y - surface->vertices[3].y;
                            vertices[0].z = surface->vertices[1].z - surface->vertices[3].z;
                            vertices[1].x = surface->vertices[2].x - surface->vertices[3].x;
                            vertices[1].y = surface->vertices[2].y - surface->vertices[3].y;
                            vertices[1].z = surface->vertices[2].z - surface->vertices[3].z;
                            surface->normals[1].x = vertices[0].y * vertices[1].z - vertices[0].z * vertices[1].y;
                            surface->normals[1].y = vertices[0].z * vertices[1].x - vertices[0].x * vertices[1].z;
                            surface->normals[1].z = vertices[0].x * vertices[1].y - vertices[0].y * vertices[1].x;
                            NUVEC *normal = &surface->normals[1];
                            f32 length = NuFsqrt(normal->x * normal->x + normal->y * normal->y + normal->z * normal->z);
                            f32 inverse = length == 0.0f ? 0.0f : 1.0f / length;
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
                        NUVEC *normal = &surface->normals[0];
                        f32 length = NuFsqrt(normal->x * normal->x + normal->y * normal->y + normal->z * normal->z);
                        f32 inverse = length == 0.0f ? 0.0f : 1.0f / length;
                        surface->normals[0].x *= inverse;
                        surface->normals[0].y *= inverse;
                        surface->normals[0].z *= inverse;
                        *cursor++ = surface;
                        ++count;
                        ++transformed_count;
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(source);
                }
            } else {
                while (batch->marker >= 0) {
                    i32 shape_count = batch->shape_count;
                    TERRAIN_SHAPE *source = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                    for (i32 remaining = shape_count; remaining > 0; --remaining, ++source) {
                        if (source->material[1] != 0)
                            continue;
                        *cursor++ = source;
                        ++count;
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(source);
                }
            }
        } else {
            if ((platform->flags & 1)) {
                while (batch->marker >= 0) {
                    i32 shape_count = batch->shape_count;
                    TERRAIN_SHAPE *source = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                    for (i32 remaining = shape_count; remaining > 0; --remaining, ++source) {
                        if (source->material[1] != 0)
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
                        bool quad = source->normals[1].y < 65535.0f;
                        if (quad) {
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
                        TERRAIN_SHAPE *surface = &TerrainScaledShapeCursorPc[transformed_count];
                        memcpy(surface->material, source->material, 4);
                        surface->vertices[0].x = vertices[0].x;
                        surface->vertices[0].y =
                            (vertices[0].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                        surface->vertices[0].z = vertices[0].z;
                        surface->vertices[1].x = vertices[1].x;
                        surface->vertices[1].y =
                            (vertices[1].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                        surface->vertices[1].z = vertices[1].z;
                        surface->vertices[2].x = vertices[2].x;
                        surface->vertices[2].y =
                            (vertices[2].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                        surface->vertices[2].z = vertices[2].z;
                        if (quad) {
                            surface->vertices[3].x = vertices[3].x;
                            surface->vertices[3].y =
                                (vertices[3].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                            surface->vertices[3].z = vertices[3].z;
                        }
                        if (quad) {
                            vertices[0].x = surface->vertices[1].x - surface->vertices[3].x;
                            vertices[0].y = surface->vertices[1].y - surface->vertices[3].y;
                            vertices[0].z = surface->vertices[1].z - surface->vertices[3].z;
                            vertices[1].x = surface->vertices[2].x - surface->vertices[3].x;
                            vertices[1].y = surface->vertices[2].y - surface->vertices[3].y;
                            vertices[1].z = surface->vertices[2].z - surface->vertices[3].z;
                            surface->normals[1].x = vertices[0].y * vertices[1].z - vertices[0].z * vertices[1].y;
                            surface->normals[1].y = vertices[0].z * vertices[1].x - vertices[0].x * vertices[1].z;
                            surface->normals[1].z = vertices[0].x * vertices[1].y - vertices[0].y * vertices[1].x;
                            NUVEC *normal = &surface->normals[1];
                            f32 length = NuFsqrt(normal->x * normal->x + normal->y * normal->y + normal->z * normal->z);
                            f32 inverse = length == 0.0f ? 0.0f : 1.0f / length;
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
                        NUVEC *normal = &surface->normals[0];
                        f32 length = NuFsqrt(normal->x * normal->x + normal->y * normal->y + normal->z * normal->z);
                        f32 inverse = length == 0.0f ? 0.0f : 1.0f / length;
                        surface->normals[0].x *= inverse;
                        surface->normals[0].y *= inverse;
                        surface->normals[0].z *= inverse;
                        *cursor++ = surface;
                        ++count;
                        ++transformed_count;
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(source);
                }
            } else {
                while (batch->marker >= 0) {
                    i32 shape_count = batch->shape_count;
                    TERRAIN_SHAPE *source = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                    for (i32 remaining = shape_count; 1 <= remaining; --remaining, ++source) {
                        if (source->material[1] != 0)
                            continue;
                        bool quad = source->normals[1].y < 65535.0f;
                        TERRAIN_SHAPE *surface = &TerrainScaledShapeCursorPc[transformed_count];
                        memcpy(surface->material, source->material, 4);
                        surface->vertices[0].x = source->vertices[0].x;
                        surface->vertices[0].y =
                            (source->vertices[0].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                        surface->vertices[0].z = source->vertices[0].z;
                        surface->vertices[1].x = source->vertices[1].x;
                        surface->vertices[1].y =
                            (source->vertices[1].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                        surface->vertices[1].z = source->vertices[1].z;
                        surface->vertices[2].x = source->vertices[2].x;
                        surface->vertices[2].y =
                            (source->vertices[2].y + group->origin.y) * query->inverse_object_scale - group->origin.y;
                        surface->vertices[2].z = source->vertices[2].z;
                        if (quad) {
                            surface->vertices[3].x = source->vertices[3].x;
                            surface->vertices[3].y =
                                (source->vertices[3].y + group->origin.y) * query->inverse_object_scale -
                                group->origin.y;
                            surface->vertices[3].z = source->vertices[3].z;
                        }
                        if (quad) {
                            NUVEC *normal = &source->normals[1];
                            f32 length =
                                NuFsqrt(((normal->y * normal->y) * query->object_scale_sq) + (normal->x * normal->x) +
                                        normal->z * normal->z);
                            f32 inverse = length == 0.0f ? 0.0f : 1.0f / length;
                            surface->normals[1].x = normal->x * inverse;
                            surface->normals[1].y = (normal->y * query->object_scale) * inverse;
                            surface->normals[1].z = normal->z * inverse;
                        } else {
                            surface->normals[1].y = 65536.0f;
                        }
                        NUVEC *normal = &source->normals[0];
                        f32 length = NuFsqrt(normal->x * normal->x + (normal->y * normal->y) * query->object_scale_sq +
                                             normal->z * normal->z);
                        f32 inverse = length == 0.0f ? 0.0f : 1.0f / length;
                        surface->normals[0].x = inverse * normal->x;
                        surface->normals[0].y = (normal->y * query->object_scale) * inverse;
                        surface->normals[0].z = normal->z * inverse;
                        *cursor++ = surface;
                        ++count;
                        ++transformed_count;
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(source);
                }
            }
        }
        i16 *header = reinterpret_cast<i16 *>(query->scan_list_storage);
        if (count != 0) {
            header[0] = count;
            header[1] = old_query->terrain_group_index;
            header = reinterpret_cast<i16 *>(cursor);
        }
        header[0] = 0;
        header[1] = 0;
        u8 flags[2] = {0, 0};
        DerotateMovementVector();
        HitTerrain();
        TerrainImpactNorm();
        if (TerrainImpactPlatform(flags) != 0) {
            NuScratchRelease();
            NuScratchRelease();
            TerrainScaledShapeCursorPc = static_cast<TERRAIN_SHAPE *>(ScaleTerrainT1);
            curSphereter = saved_sphere;
TerI = old_query;
            return 1;
        }
        query->start_position = query->position;
        ((TerrainQuery_s *)TerI)->movement.x = 0.0f;
        ((TerrainQuery_s *)TerI)->movement.y = -0.05f;
        ((TerrainQuery_s *)TerI)->movement.z = 0.0f;
        ((TerrainQuery_s *)TerI)->position.y = query->start_position.y + 0.025f;
        DerotateMovementVector();
        TerrOverRideScan = reinterpret_cast<TERRAIN_SHAPE **>(old_query->scan_list_storage);
        HitTerrain();
        ((TerrainQuery_s *)TerI)->position.y = query->start_position.y;
        flags[0] = 0;
        if (((TerrainQuery_s *)TerI)->hit_type >= 1 && ((TerrainQuery_s *)TerI)->hit_type <= 15) {
            query->start_position.y =
                ((((TerrainQuery_s *)TerI)->movement.y * ((TerrainQuery_s *)TerI)->hit_time + query->start_position.y) +
                 0.001f) +
                0.025f;
            TerI = old_query;
            if (!(query->start_position.y >= 2000000.0f)) {
                f32 delta = query->start_position.y - query->position.y;
                if (!(delta < -0.008f) &&
                    (delta < 0.01f || query->movement.x * query->movement.x + query->movement.z * query->movement.z >
                                          (delta - 0.0025f) * (delta - 0.0025f))) {
                    query->position.y = query->position.y + delta;
                    flags[0] = 1;
                }
            } else {
                query->start_position.y = query->position.y;
            }
        } else {
            TerI = old_query;
            query->start_position.y = query->position.y;
        }
        if (!TerrainPlatformMoveCheck(&query->position, NULL, platform_index, 1, 1))
            flags[0] = 0;
        ((TerrainQuery_s *)TerI)->position = query->position;
        if (flags[0] != 0 && ((TERRAIN_SET_PC *)CurrentTerrain)->groups[TerrainShadowGroupIndexPc].chunk_type == 1) {
            ((TerrainQuery_s *)TerI)->hit_flags[1] = 1;
            PlatformConnect(reinterpret_cast<char *>(old_query->hit_flags), &old_query->movement, movement_delta,
                            ((TERRAIN_SET_PC *)CurrentTerrain)->groups[TerrainShadowGroupIndexPc].scene_index);
        } else if (CurTrackInfo != NULL) {
            CurTrackInfo->flags &= ~TERRAIN_TRACK_FLAG_CONNECTED;
        }
        NuScratchRelease();
        NuScratchRelease();
        TerrainScaledShapeCursorPc = static_cast<TERRAIN_SHAPE *>(ScaleTerrainT1);
        curSphereter = saved_sphere;
        TerI = old_query;
        return 0;
    }
    i32 TerrainPlatformMoveCheck(NUVEC *position, NUVEC *normal, i32 platform_index, i32 resolve_impact,
                                 i32 keep_embedded_disabled) {
        i32 group_index = ((TERRAIN_SET_PC *)CurrentTerrain)->platforms[platform_index].terrain_group_index;
        u8 *entry = static_cast<u8 *>(((TerrainQuery_s *)TerI)->scan_list);
        while (*reinterpret_cast<i16 *>(entry) > 0) {
            i16 *header = reinterpret_cast<i16 *>(entry);
            if (header[1] == group_index)
                header[0] = -header[0];
            i32 count = header[0];
            if (count < 0)
                count = -count;
            entry += 4 + count * 4;
        }
        if (position->y > ((TerrainQuery_s *)TerI)->position.y &&
            ((TerrainQuery_s *)TerI)->position.y + 0.01f > position->y)
            ((TerrainQuery_s *)TerI)->position.y = position->y;
        NUVEC saved_movement = ((TerrainQuery_s *)TerI)->movement;
        TERRAIN_SHAPE *saved_surface = ((TerrainQuery_s *)TerI)->surface;
        i16 saved_group = ((TerrainQuery_s *)TerI)->terrain_group_index;
        i16 saved_scan_group = ((TerrainQuery_s *)TerI)->scan_group_index;
        f32 saved_time = ((TerrainQuery_s *)TerI)->hit_time;
        f32 saved_unclamped_time = ((TerrainQuery_s *)TerI)->unclamped_hit_time;
        NUVEC saved_movement_normal = ((TerrainQuery_s *)TerI)->movement_normal;
        NUVEC saved_impact_normal = ((TerrainQuery_s *)TerI)->impact_normal;
        u8 saved_hit_type = ((TerrainQuery_s *)TerI)->hit_type;
        ((TerrainQuery_s *)TerI)->movement.x = position->x - ((TerrainQuery_s *)TerI)->position.x;
        ((TerrainQuery_s *)TerI)->movement.y = position->y - ((TerrainQuery_s *)TerI)->position.y;
        ((TerrainQuery_s *)TerI)->movement.z = position->z - ((TerrainQuery_s *)TerI)->position.z;
        DerotateMovementVector();
        HitTerrain();
        if (resolve_impact != 0 && ((TerrainQuery_s *)TerI)->hit_type != 0) {
            TerrainImpactNorm();
            TerrainImpactPlatform(((TerrainQuery_s *)TerI)->hit_flags);
            if (normal != NULL)
                *normal = ((TerrainQuery_s *)TerI)->movement_normal;
            *position = ((TerrainQuery_s *)TerI)->position;
        }
        u8 hit_type = ((TerrainQuery_s *)TerI)->hit_type;
        ((TerrainQuery_s *)TerI)->movement = saved_movement;
        ((TerrainQuery_s *)TerI)->surface = saved_surface;
        ((TerrainQuery_s *)TerI)->terrain_group_index = saved_group;
        ((TerrainQuery_s *)TerI)->scan_group_index = saved_scan_group;
        ((TerrainQuery_s *)TerI)->hit_time = saved_time;
        ((TerrainQuery_s *)TerI)->unclamped_hit_time = saved_unclamped_time;
        ((TerrainQuery_s *)TerI)->movement_normal = saved_movement_normal;
        ((TerrainQuery_s *)TerI)->impact_normal = saved_impact_normal;
        if (hit_type <= 15 || keep_embedded_disabled == 0) {
            entry = static_cast<u8 *>(((TerrainQuery_s *)TerI)->scan_list);
            while (*reinterpret_cast<i16 *>(entry) != 0) {
                i16 *header = reinterpret_cast<i16 *>(entry);
                i32 count = header[0];
                if (count < 0)
                    count = -count;
                if (header[1] == group_index)
                    header[0] = count;
                entry += 4 + count * 4;
            }
        }
        ((TerrainQuery_s *)TerI)->hit_type = saved_hit_type;
        return hit_type == 0;
    }
    i32 TerrShapeSideStep(NUVEC *, NUVEC *, u8 *) {
        enum TERRAIN_SIDE_STEP_RESULT {
            TERRAIN_SIDE_STEP_HANDLED = 0,
            TERRAIN_SIDE_STEP_USE_STANDARD_IMPACT = 1,
        };

        const f32 kCylinderFloorThreshold = 0.5f;
        const f32 kMaximumWallNormalY = 0.707f;
        const f32 kMinimumWallNormalY = -0.8f;
        const f32 kMovementPushScale = 0.15f;
        const f32 kShapePushScale = 0.05f;

        i32 result = TERRAIN_SIDE_STEP_USE_STANDARD_IMPACT;
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        const u8 hit_type = query->hit_type;
        if (hit_type == TERRAIN_HIT_TYPE_NONE) {
            return result;
        }

        i32 push_from_collision_normal;
        switch (hit_type) {
            case TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_VERTEX:
                push_from_collision_normal = kCylinderFloorThreshold <= query->impact_normal.y;
                break;
            default:
                if (query->impact_normal.y >= kMaximumWallNormalY || query->impact_normal.y < kMinimumWallNormalY) {
                    return TERRAIN_SIDE_STEP_USE_STANDARD_IMPACT;
                }
                push_from_collision_normal = false;
                break;
        }

        const f32 shape_angle = TerrShape->angle;
        NUVEC axis;
        axis.x = NuTrigTable[static_cast<i32>(shape_angle + 16384.0f) >> 1 & 0x7fff];
        axis.y = 0.0f;
        axis.z = -NuTrigTable[static_cast<i32>(shape_angle) >> 1 & 0x7fff];

        query->start_position.x = query->position.x;
        query->start_position.y = query->position.y;
        query->start_position.z = query->position.z;
        query->start_movement.x = query->movement.x;
        query->start_movement.y = query->movement.y;
        query->start_movement.z = query->movement.z;
        query->movement.y = 0.0f;

        if (push_from_collision_normal == 1) {
            const f32 push_x = query->movement_normal.x * TerrShape->radius;
            const f32 push_z = query->movement_normal.z * TerrShape->radius;
            query->movement.x = push_x * kMovementPushScale;
            query->movement.z = push_z * kMovementPushScale;
            TerrShape->offset.x += push_x * kShapePushScale;
            TerrShape->offset.z += push_z * kShapePushScale;
        } else {
            f32 axis_impact = query->impact_normal.x * axis.x + query->impact_normal.z * axis.z;
            if (axis_impact == 0.0f) {
                axis_impact = (TerrShape->offset.x * axis.x + TerrShape->offset.z * axis.z) / TerrShape->radius;
                if (axis_impact == 0.0f) {
                    return TERRAIN_SIDE_STEP_USE_STANDARD_IMPACT;
                }
            }

            const f32 signed_distance = axis_impact < 0.0f ? -NuFsqrt(-axis_impact) : NuFsqrt(axis_impact);
            query = (TerrainQuery_s *)TerI;
            query->movement.x = axis.x * signed_distance * TerrShape->radius + TerrShape->offset.x;
            query->movement.z = axis.z * signed_distance * TerrShape->radius + TerrShape->offset.z;
        }

        u8 side_step_hit_flags[2];
        NUVEC impact_result;
        do {
            DerotateMovementVector();
            HitTerrain();
            TerrainImpactNorm();
            TerrainImpact(&impact_result, &impact_result, side_step_hit_flags);

            --TerrShapeAdjCnt;
            query = (TerrainQuery_s *)TerI;
        } while (TerrShapeAdjCnt > 0 && query->hit_type != TERRAIN_HIT_TYPE_NONE);

        if (query->hit_type == TERRAIN_HIT_TYPE_NONE) {
            query->position.x += query->movement.x;
            query->position.y += query->movement.y;
            query->position.z += query->movement.z;
        }

        TerrShape->offset.x += query->start_position.x - query->position.x;
        TerrShape->offset.z += query->start_position.z - query->position.z;
        query->hit_type = TERRAIN_HIT_TYPE_FACE;
        query->movement.x = query->start_movement.x;
        query->movement.y = query->start_movement.y;
        query->movement.z = query->start_movement.z;
        TerrainSideClamp(&axis, &query->position);

        result = TERRAIN_SIDE_STEP_HANDLED;
        return result;
    }
    void TerrainSideClamp(NUVEC *axis, NUVEC *position) {
        TERRAIN_AXIS_FREEDOM_SHAPE *shape = TerrShape;
        const f32 shape_x = shape->offset.x;
        const f32 axis_z = axis->z;
        const f32 axis_x = axis->x;
        const f32 shape_z = shape->offset.z;
        const f32 forward_z = shape_z * axis_z;
        f32 clamped_forward = shape_x * axis_x;
        f32 clamped_side = shape_x * axis_z;
        clamped_forward += forward_z;
        clamped_side -= shape_z * axis_x;

        const f32 radius = shape->radius;
        const f32 negative_radius = -radius;
        clamped_forward = MIN(radius, clamped_forward);
        clamped_forward = MAX(negative_radius, clamped_forward);
        const f32 side_radius = radius * 0.2f;
        const f32 negative_side_radius = negative_radius * 0.2f;
        clamped_side = MIN(side_radius, clamped_side);
        clamped_side = MAX(negative_side_radius, clamped_side);

        position->x -= (axis->x * clamped_forward + axis->z * clamped_side) - shape->offset.x;
        position->z -= (axis->z * clamped_forward - axis->x * clamped_side) - shape->offset.z;

        shape->offset.x = axis->x * clamped_forward + axis->z * clamped_side;
        shape->offset.z = axis->z * clamped_forward - axis->x * clamped_side;
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
    i16 InsideLineF(f32 point_u, f32 point_v, f32 line_start_u, f32 line_start_v, f32 line_end_u, f32 line_end_v) {
        if ((point_u - line_start_u) * (line_end_v - line_start_v) +
                (point_v - line_start_v) * (line_start_u - line_end_u) <
            0.0f) {
            return 0;
        }
        return 1;
    }
    TERRAIN_TRACK_SLOT_PC *AllocTerrId() {
        TERRSET *terrain = ((TERRAIN_SET_PC *)CurrentTerrain);
        if (terrain == NULL) {
            return NULL;
        }

        for (i32 slot_index = 0; slot_index < 64; ++slot_index) {
            if (terrain->track_slots[slot_index].id == NULL) {
                return &terrain->track_slots[slot_index];
            }
        }

        return NULL;
    }
}

extern "C" {

    void AddCollisionSphere(NUVEC *position, f32 radius) {
        if (curSphereter < 16) {
            SphereData[curSphereter].position = *position;
            SphereData[curSphereter].radius = radius;
            ++curSphereter;
        }
    }
}

extern "C" {
    extern TERRAIN_LAST_IMPACT_PC TerrLastImpact;
    extern i16 TerrImpact;
    extern NUVEC TerrImpactPos, TerrImpactNormal;
}
extern "C" void NewTerrainScaleYMask(NUVEC *position, NUVEC *movement, u8 *hit_flags, i32 object_index, f32 radius,
                                     f32 collision_radius, f32 object_scale, i32 embedded_retry, i32 scan_flags,
                                     i32 terrain_mask) {
    TerrPolyInfo.material[0] = 0;
    TerrPolyInfo.material[1] = 0;
    TerrPolyInfo.flags = 0;
    TerrPolyInfo.normal_flags = 0;
    TerrImpact = 0;
    plathitid = -1;
    TerrainShadowStatusPc = -1;
    TerrPoly = 0;
    TerrWallInfo = 0;
    PlatCrush = 0;
    terrhitflags = 0;
    TerrainShadowGroupIndexPc = -1;

    if (((TERRAIN_SET_PC *)CurrentTerrain) == NULL) {
        TerImpactData = 0;
        return;
    }

    TerrLastImpact.hit_type = 0.0f;
    CurTrackInfo = NULL;
    for (i32 slot_index = 0; slot_index < 64; ++slot_index) {
        if (((TERRAIN_SET_PC *)CurrentTerrain)->track_slots[slot_index].id == hit_flags) {
            CurTrackInfo = &((TERRAIN_SET_PC *)CurrentTerrain)->track_slots[slot_index];
            break;
        }
    }
    TerrOverRideScan = 0;

    TerI = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(TerI);
    query->object_scale = object_scale;
    query->object_scale_sq = object_scale * object_scale;
    if (object_scale == 0.0f) {
        query->inverse_object_scale = 0.0f;
        query->inverse_object_scale_sq = 0.0f;
    } else {
        query->inverse_object_scale = 1.0f / object_scale;
        query->inverse_object_scale_sq = query->inverse_object_scale * query->inverse_object_scale;
    }
    query->collision_radius = collision_radius;
    query->inverse_collision_radius = collision_radius == 0.0f ? 0.0f : 1.0f / collision_radius;
    query->collision_radius_sq = collision_radius * collision_radius;

    const f32 position_x = position->x;
    const f32 position_y = position->y + collision_radius * object_scale;
    const f32 position_z = position->z;
    query->position.x = position_x;
    query->start_position.x = position_x;
    query->position.y = position_y;
    query->start_position.y = position_y;
    query->position.z = position_z;
    query->start_position.z = position_z;

    const f32 movement_x = movement->x;
    const f32 movement_y = movement->y;
    const f32 movement_z = movement->z;
    query->movement.x = movement_x;
    query->start_movement.x = movement_x;
    query->movement.y = movement_y;
    query->start_movement.y = movement_y;
    query->movement.z = movement_z;
    query->start_movement.z = movement_z;
    query->object_index = static_cast<i16>(object_index);
    query->flags &= ~1u;
    query->hit_flags = hit_flags;
    query->radius = radius;
    query->scan_result = 0;
    query->separation_epsilon = 0.005f;
    query->compare_epsilon = 0.000005f;

    TerrainShadowGroupIndexPc = -1;
    ScanTerrain(1, terrain_mask, scan_flags != 0 ? 0x40 : 0);

    union {
        f32 value;
        u32 bits;
    } absolute_x, absolute_y, absolute_z;
    absolute_x.value = movement->x;
    absolute_y.value = movement->y;
    absolute_z.value = movement->z;
    absolute_x.bits &= 0x7fffffff;
    absolute_y.bits &= 0x7fffffff;
    absolute_z.bits &= 0x7fffffff;
    if (hit_flags[1] != 0 && radius > absolute_x.value && radius > absolute_y.value && radius > absolute_z.value &&
        platinrange == 0) {
        NuScratchRelease();
        TerrShapeAdjCnt = 0;
        ShadNorm.x = 0.0f;
        ShadNorm.y = 1.0f;
        ShadNorm.z = 0.0f;
        terrhitflags |= 2;
        TerImpactData = 0;
        return;
    }

    query->position.y *= query->inverse_object_scale;
    query->movement.y *= query->inverse_object_scale;
    hit_flags[0] = 0;
    hit_flags[1] = 0;

    i32 scan_count = PlatformChecks(6, movement);
    for (;;) {
        DerotateMovementVector();
        HitTerrain();
        TerrainImpactNorm();
        StorePlatImpact();

        query = static_cast<TerrainQuery_s *>(TerI);
        u8 hit_type = query->hit_type;
        bool impact_already_resolved = false;
        if (hit_type > TERRAIN_HIT_TYPE_SECOND_NORMAL && query->terrain_group_index != -1) {
            TERRAIN_GROUP *group = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[query->terrain_group_index];
            if (group->chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY) {
                --scan_count;
                NewTerrStoreAnyInfo();
                NUVEC before = query->position;
                i32 embedded = TerrainPlatformEmbedded(movement);
                query = static_cast<TerrainQuery_s *>(TerI);
                impact_already_resolved =
                    embedded == 0 &&
                    (before.x != query->position.x || before.y != query->position.y || before.z != query->position.z);
                hit_type = query->hit_type;
            }
        }

        if (!impact_already_resolved) {
            if (query->hit_type != TERRAIN_HIT_TYPE_NONE && query->terrain_group_index >= 0) {
                TERRAIN_GROUP *group = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[query->terrain_group_index];
                ShadNorm.x = query->impact_normal.x;
                ShadNorm.y = query->impact_normal.y;
                ShadNorm.z = query->impact_normal.z;

                f32 slope = 0.707f;
                if (query->surface != NULL) {
                    slope = query->movement_normal.x * query->surface->normals[0].x +
                            query->movement_normal.y * query->surface->normals[0].y +
                            query->movement_normal.z * query->surface->normals[0].z;
                }

                const u8 hit_class = query->hit_type & TERRAIN_HIT_TYPE_CLASS_MASK;
                f32 wall_limit;
                if (group->chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY) {
                    wall_limit = hit_class > TERRAIN_HIT_TYPE_FACE && (f64)0.95f > slope ? 0.98f : 0.707f;
                } else {
                    wall_limit = query->shape_adjusted != 0 ? 1.1f : -1.1f;
                }

                if (wallover != 0.0f) {
                    wall_limit = wallover;
                }
                if (hit_class != TERRAIN_HIT_TYPE_FACE) {
                    wall_limit = 0.707f;
                }
                if (query->surface != NULL && (query->surface->normal_flags & TERRAIN_SURFACE_CLASS_MASK) ==
                                                  TERRAIN_SURFACE_CLASS_WALL_OVERRIDE) {
                    query->position.x += query->movement_normal.x * (f64)0.001f;
                    query->position.z += query->movement_normal.z * (f64)0.001f;
                    wall_limit = 1.1f;
                }

                if (query->impact_normal.y >= wall_limit && group->chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY) {
                    ((TERRAIN_SET_PC *)CurrentTerrain)->platforms[group->scene_index].flags |=
                        TERRAIN_PLATFORM_FLAG_COLLIDED;
                }
            }

            if (TerrShapeAdjCnt == 0 || TerrShapeSideStep(position, movement, hit_flags) != 0) {
                TerrainImpact(position, movement, hit_flags);
            }

            query = static_cast<TerrainQuery_s *>(TerI);
            --scan_count;
            TerrLastImpact.position.x = query->position.x - query->movement_normal.x * query->collision_radius;
            TerrLastImpact.position.y =
                (query->position.y - query->movement_normal.y * query->collision_radius) * query->object_scale;
            TerrLastImpact.position.z = query->position.z - query->movement_normal.z * query->collision_radius;
            TerrLastImpact.hit_type = static_cast<f32>(static_cast<u32>(query->hit_type));
            hit_type = query->hit_type;
        }

        if (hit_type == TERRAIN_HIT_TYPE_NONE) {
            if (scan_count > 3 && hit_flags[0] == 0 && hit_flags[1] == 0 && embedded_retry != 0) {
                query->position.x = position->x;
                query->position.y = position->y * query->inverse_object_scale + query->collision_radius + (f64)0.003f;
                query->position.z = position->z;
                query->movement.x = 0.0f;
                query->movement.y = -0.007f;
                query->movement.z = 0.0f;
                DerotateMovementVector();
                HitTerrain();
                query = static_cast<TerrainQuery_s *>(TerI);
                if (query->hit_type != TERRAIN_HIT_TYPE_NONE &&
                    (query->terrain_group_index < 0 || query->surface == NULL ||
                     (query->surface->normal_flags & TERRAIN_SURFACE_CLASS_MASK) !=
                         TERRAIN_SURFACE_CLASS_WALL_OVERRIDE)) {
                    TerrainImpactNorm();
                    ShadNorm.x = query->impact_normal.x;
                    ShadNorm.y = query->impact_normal.y;
                    ShadNorm.z = query->impact_normal.z;
                    query->start_movement.x = movement->x;
                    query->start_movement.y = movement->y;
                    query->start_movement.z = movement->z;
                    TerrainImpact(position, movement, hit_flags);
                    query = static_cast<TerrainQuery_s *>(TerI);
                    position->x = query->position.x;
                    position->y =
                        query->position.y * query->object_scale - query->collision_radius * query->object_scale;
                    position->z = query->position.z;
                    movement->x = query->start_movement.x;
                    movement->y = query->start_movement.y;
                    movement->z = query->start_movement.z;
                }
            }
            break;
        }

        TerrImpact = query->surface == NULL ? 2 : 1;
        TerrImpactPos.x = query->position.x - query->movement_normal.x * query->collision_radius;
        TerrImpactPos.y =
            (query->position.y - query->movement_normal.y * query->collision_radius) * query->object_scale;
        TerrImpactPos.z = query->position.z - query->movement_normal.z * query->collision_radius;
        TerrImpactNormal = query->impact_normal;

        if (scan_count > 0) {
            const f64 normal_mag_sq = static_cast<f64>(query->movement_normal.x) * query->movement_normal.x +
                                      query->movement_normal.y * query->movement_normal.y +
                                      query->movement_normal.z * query->movement_normal.z;
            if (normal_mag_sq <= 1.5) {
                continue;
            }
        }

        position->x = query->position.x;
        position->y = query->position.y * query->object_scale - query->collision_radius * query->object_scale;
        position->z = query->position.z;
        break;
    }

    NuScratchRelease();
    TerrShapeAdjCnt = 0;
    TerImpactData = 0;
}

extern "C" void __cdecl FullReflect(NUVEC *normal, NUVEC *movement, NUVEC *result) {
    f32 reflection = (-movement->y * normal->y + -movement->x * normal->x) + -movement->z * normal->z;
    result->x = movement->x + 2.0 * (normal->x * reflection);
    result->y = movement->y + 2.0 * (normal->y * reflection);
    result->z = movement->z + 2.0 * (normal->z * reflection);
}
