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
extern "C" {
    void DerotateMovementVector() {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        const f64 yaw = NuAtan2DA(query->movement.x, query->movement.z);
        query = (TerrainQuery_s *)TerI;
        query->movement_yaw = static_cast<f32>(yaw);
        const i32 pitch =
            NuAtan2DA(-((TerrainQuery_s *)TerI)->movement.y,
                      NuFsqrt(query->movement.x * query->movement.x + query->movement.z * query->movement.z));
        query = (TerrainQuery_s *)TerI;
        query->movement_pitch = static_cast<f32>(pitch);
        ((TerrainQuery_s *)TerI)->movement_length =
            NuFsqrt(query->movement.x * query->movement.x + query->movement.y * query->movement.y +
                    query->movement.z * query->movement.z);
    }
#define TERRAIN_RV_SIN(angle) NuTrigTable[(static_cast<i32>(angle) >> 1) & (NUTRIGTABLE_COUNT - 1)]
#define TERRAIN_RV_COS(angle) NuTrigTable[(static_cast<i32>((angle) + 16384.0f) >> 1) & (NUTRIGTABLE_COUNT - 1)]
    void RotateVec(NUVEC *source, NUVEC *destination) {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        const f32 rotated_z =
            TERRAIN_RV_COS(query->movement_pitch) * source->z + TERRAIN_RV_SIN(query->movement_pitch) * source->y;
        destination->y =
            source->y * TERRAIN_RV_COS(query->movement_pitch) - TERRAIN_RV_SIN(query->movement_pitch) * source->z;
        destination->z =
            TERRAIN_RV_COS(query->movement_yaw) * rotated_z - TERRAIN_RV_SIN(query->movement_yaw) * source->x;
        destination->x =
            TERRAIN_RV_COS(query->movement_yaw) * source->x + TERRAIN_RV_SIN(query->movement_yaw) * rotated_z;
    }
#undef TERRAIN_RV_SIN
#undef TERRAIN_RV_COS
#define TERRAIN_DRT_SIN(angle) NuTrigTable[(static_cast<i32>(angle) >> 1) & (NUTRIGTABLE_COUNT - 1)]
#define TERRAIN_DRT_COS(angle) NuTrigTable[(static_cast<i32>((angle) + 16384.0f) >> 1) & (NUTRIGTABLE_COUNT - 1)]
#define TERRAIN_DRT_VERTEX(index)                                                                                      \
    {                                                                                                                  \
        const f64 relative_z = surface->vertices[index].z - query->local_start.z;                                      \
        const f64 relative_x = surface->vertices[index].x - query->local_start.x;                                      \
        const f32 rotated_z = relative_z * cos_yaw - relative_x * sin_yaw;                                             \
        query->transformed_vertices[index].x = relative_z * sin_yaw + relative_x * cos_yaw;                            \
        query->transformed_vertices[index].y =                                                                         \
            (surface->vertices[index].y - query->local_start.y) * cos_pitch - rotated_z * sin_pitch;                   \
        query->transformed_vertices[index].z =                                                                         \
            (surface->vertices[index].y - query->local_start.y) * sin_pitch + rotated_z * cos_pitch;                   \
    }
    void DeRotateTerrain(TERRAIN_SHAPE *surface) {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;

        const f32 pitch = -query->movement_pitch;
        const f32 sin_pitch = TERRAIN_DRT_SIN(pitch);
        const f32 cos_pitch = TERRAIN_DRT_COS(pitch);
        const f32 yaw = -query->movement_yaw;
        const f32 sin_yaw = TERRAIN_DRT_SIN(yaw);
        const f32 cos_yaw = TERRAIN_DRT_COS(yaw);

        TERRAIN_DRT_VERTEX(0)
        TERRAIN_DRT_VERTEX(1)
        TERRAIN_DRT_VERTEX(2)
        if (surface->normals[1].y < 65536.0f) {
            TERRAIN_DRT_VERTEX(3)
        }
    }
#undef TERRAIN_DRT_VERTEX
#undef TERRAIN_DRT_SIN
#undef TERRAIN_DRT_COS
#define TERRAIN_DRP_SIN(angle) NuTrigTable[(static_cast<i32>(angle) >> 1) & (NUTRIGTABLE_COUNT - 1)]
#define TERRAIN_DRP_COS(angle) NuTrigTable[(static_cast<i32>((angle) + 16384.0f) >> 1) & (NUTRIGTABLE_COUNT - 1)]
    void DeRotatePoint(NUVEC *point) {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;

        const f32 pitch = -query->movement_pitch;
        const f32 sin_pitch = TERRAIN_DRP_SIN(pitch);
        const f32 cos_pitch = TERRAIN_DRP_COS(pitch);
        const f32 yaw = -query->movement_yaw;
        const f32 sin_yaw = TERRAIN_DRP_SIN(yaw);
        const f32 cos_yaw = TERRAIN_DRP_COS(yaw);

        const f64 relative_z = point->z - query->position.z;
        const f64 relative_x = point->x - query->position.x;
        const f32 rotated_z = relative_z * cos_yaw - relative_x * sin_yaw;
        point->x = relative_z * sin_yaw + relative_x * cos_yaw;
        point->z = (query->collision_radius + point->y - query->position.y) * sin_pitch + rotated_z * cos_pitch;
        point->y = (query->collision_radius + point->y - query->position.y) * cos_pitch - rotated_z * sin_pitch;
    }
#undef TERRAIN_DRP_SIN
#undef TERRAIN_DRP_COS
#define TERRAIN_CC_FIRST query->transformed_vertices[first_vertex]
#define TERRAIN_CC_SECOND query->transformed_vertices[second_vertex]
    i32 CheckCylinder(i32 first_vertex, i32 second_vertex, i32 *vertex_mask, i32 remaining_vertex_mask) {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;

        // Reject an edge when both endpoints lie outside the swept sphere's
        // expanded bounds. Once an edge is rejected this way, its endpoint bits
        // can be removed from the later vertex tests as well.
        if ((TERRAIN_CC_FIRST.x > query->collision_radius && TERRAIN_CC_SECOND.x > query->collision_radius) ||
            (TERRAIN_CC_FIRST.x < -query->collision_radius && TERRAIN_CC_SECOND.x < -query->collision_radius) ||
            (TERRAIN_CC_FIRST.y > query->collision_radius && TERRAIN_CC_SECOND.y > query->collision_radius) ||
            (TERRAIN_CC_FIRST.y < -query->collision_radius && TERRAIN_CC_SECOND.y < -query->collision_radius) ||
            (TERRAIN_CC_FIRST.z < -query->collision_radius && TERRAIN_CC_SECOND.z < -query->collision_radius) ||
            (TERRAIN_CC_FIRST.z > query->movement_length + query->collision_radius &&
             TERRAIN_CC_SECOND.z > query->movement_length + query->collision_radius)) {
            *vertex_mask &= remaining_vertex_mask;
            return 0;
        }

        NUVEC edge_direction;
        edge_direction.x = TERRAIN_CC_SECOND.x - TERRAIN_CC_FIRST.x;
        edge_direction.y = TERRAIN_CC_SECOND.y - TERRAIN_CC_FIRST.y;
        const f32 horizontal_length_sq = edge_direction.x * edge_direction.x + edge_direction.y * edge_direction.y;

        // An almost vertical edge has no stable radial cylinder in the XY plane.
        // It can only overlap the sphere at the start of the sweep when it crosses
        // z = 0.
        if (horizontal_length_sq < 1.0e-12f) {
            if ((TERRAIN_CC_FIRST.z < 0.0f && TERRAIN_CC_SECOND.z < 0.0f) ||
                (TERRAIN_CC_FIRST.z > 0.0f && TERRAIN_CC_SECOND.z > 0.0f)) {
                return 0;
            }

            f32 distance = TERRAIN_CC_SECOND.x * TERRAIN_CC_SECOND.x + TERRAIN_CC_SECOND.y * TERRAIN_CC_SECOND.y;
            if (distance <= query->collision_radius_sq) {
                distance = NuFsqrt(distance);
                query = (TerrainQuery_s *)TerI;
                if (distance - query->collision_radius < query->hit_time) {
                    NUVEC local_normal;
                    local_normal.x = -TERRAIN_CC_SECOND.x;
                    local_normal.y = -TERRAIN_CC_SECOND.y;
                    local_normal.z = 0.0f;
                    RotateVec(&local_normal, &local_normal);
                    const NUVEC &surface_normal = query->working_surface->normals[0];
                    if (surface_normal.y * local_normal.y + surface_normal.x * local_normal.x +
                            surface_normal.z * local_normal.z >
                        0.0) {
                        query->hit_time = distance - query->collision_radius;
                        const f32 inverse_distance = TerrainDivideNonzeroPC(1.0f, distance);
                        query->hit_type = TERRAIN_HIT_TYPE_CYLINDER | TERRAIN_HIT_TYPE_SECOND_NORMAL;
                        query->movement_normal.x = -TERRAIN_CC_SECOND.x * inverse_distance;
                        query->movement_normal.y = -TERRAIN_CC_SECOND.y * inverse_distance;
                        query->movement_normal.z = 0.0f;
                        return 1;
                    }
                }
            }
            return 0;
        }

        // Squared radial distance from the movement axis to the infinite line
        // containing the edge. This avoids a square root for the common miss.
        const f32 line_cross = TERRAIN_CC_FIRST.x * edge_direction.y - TERRAIN_CC_FIRST.y * edge_direction.x;
        if (line_cross * line_cross > query->collision_radius_sq * horizontal_length_sq) {
            *vertex_mask &= remaining_vertex_mask;
            return 0;
        }

        const f32 line_distance_sq = TerrainDivideNonzeroPC(line_cross, horizontal_length_sq) * line_cross;
        edge_direction.z = TERRAIN_CC_SECOND.z - TERRAIN_CC_FIRST.z;
        const f32 edge_length = NuFsqrt(edge_direction.x * edge_direction.x + edge_direction.y * edge_direction.y +
                                        edge_direction.z * edge_direction.z);
        const f32 inverse_edge_length = TerrainDivideNonzeroPC(1.0f, edge_length);
        query = (TerrainQuery_s *)TerI;
        edge_direction.x *= inverse_edge_length;
        edge_direction.y *= inverse_edge_length;
        edge_direction.z *= inverse_edge_length;

        f32 horizontal_perpendicular[2];
        horizontal_perpendicular[0] = -edge_direction.y;
        horizontal_perpendicular[1] = edge_direction.x;
        NUVEC line_cross_vector;
        line_cross_vector.x = -TERRAIN_CC_FIRST.x;
        line_cross_vector.y = -TERRAIN_CC_FIRST.y;
        line_cross_vector.z = -TERRAIN_CC_FIRST.z;
        NuVecCross(&line_cross_vector, &line_cross_vector, &edge_direction);

        const f32 closest_height = -TerrainDivideNonzeroPC(
            line_cross_vector.y * horizontal_perpendicular[1] + line_cross_vector.x * horizontal_perpendicular[0],
            horizontal_perpendicular[0] * horizontal_perpendicular[0] +
                horizontal_perpendicular[1] * horizontal_perpendicular[1]);

        // This normal lies in the plane formed by the edge and the movement axis.
        // Its z component tells how far along the sweep the cylinder surface is
        // reached.
        NUVEC cylinder_side;
        cylinder_side.x = edge_direction.x * edge_direction.z;
        cylinder_side.y = -(edge_direction.y * edge_direction.z);
        cylinder_side.z = -edge_direction.x * edge_direction.x - edge_direction.y * edge_direction.y;
        const f32 cylinder_side_z = TerrainDivideNonzeroPC(cylinder_side.z, NuFsqrt(cylinder_side.x * cylinder_side.x +
                                                                                    cylinder_side.y * cylinder_side.y +
                                                                                    cylinder_side.z * cylinder_side.z));
        const f32 height_offset = TerrainDivideNonzeroPC(
            NuFsqrt(((TerrainQuery_s *)TerI)->collision_radius_sq - line_distance_sq), cylinder_side_z);

        f32 hit_distance;
        if (height_offset < 0.0f) {
            hit_distance = height_offset + closest_height;
        } else {
            hit_distance = closest_height - height_offset;
        }
        query = (TerrainQuery_s *)TerI;
        if (hit_distance >= 0.0f && hit_distance <= query->movement_length) {
            const f32 edge_parameter = -TERRAIN_CC_FIRST.x * edge_direction.x + -TERRAIN_CC_FIRST.y * edge_direction.y +
                                       (hit_distance - TERRAIN_CC_FIRST.z) * edge_direction.z;
            if (0.0f < edge_parameter && edge_parameter <= edge_length) {
                if (TerrainDivideNonzeroPC(hit_distance, query->movement_length) < query->hit_time) {
                    query->hit_type = TERRAIN_HIT_TYPE_CYLINDER;
                    query->hit_time = TerrainDivideNonzeroPC(hit_distance, query->movement_length);
                    query->movement_normal.x = -(edge_parameter * edge_direction.x + TERRAIN_CC_FIRST.x);
                    query->movement_normal.y = -(edge_parameter * edge_direction.y + TERRAIN_CC_FIRST.y);
                    query->movement_normal.z = hit_distance - (edge_parameter * edge_direction.z + TERRAIN_CC_FIRST.z);
                    *vertex_mask &= remaining_vertex_mask;
                    return 1;
                }
                *vertex_mask &= remaining_vertex_mask;
                return 0;
            }
        }

        // No forward-time hit was found. Test whether the sphere already overlaps
        // the finite edge at the start of the sweep.
        const f32 negative_start_radius = -query->collision_radius;
        if ((TERRAIN_CC_FIRST.z < negative_start_radius && TERRAIN_CC_SECOND.z < negative_start_radius) ||
            (TERRAIN_CC_FIRST.z > query->collision_radius && TERRAIN_CC_SECOND.z > query->collision_radius)) {
            return 0;
        }

        const f32 closest_parameter = -edge_direction.x * TERRAIN_CC_FIRST.x - TERRAIN_CC_FIRST.y * edge_direction.y -
                                      TERRAIN_CC_FIRST.z * edge_direction.z;
        if (closest_parameter < 0.0f) {
            return 0;
        }
        if (closest_parameter > (TERRAIN_CC_SECOND.x - TERRAIN_CC_FIRST.x) * edge_direction.x +
                                    (TERRAIN_CC_SECOND.y - TERRAIN_CC_FIRST.y) * edge_direction.y +
                                    (TERRAIN_CC_SECOND.z - TERRAIN_CC_FIRST.z) * edge_direction.z) {
            return 0;
        }

        // The normalized direction is no longer needed after the closest
        // parameter has been bounded, so reuse it for the closest point itself.
        edge_direction.x = edge_direction.x * closest_parameter + TERRAIN_CC_FIRST.x;
        edge_direction.y = edge_direction.y * closest_parameter + TERRAIN_CC_FIRST.y;
        edge_direction.z = edge_direction.z * closest_parameter + TERRAIN_CC_FIRST.z;
        f32 distance = edge_direction.x * edge_direction.x + edge_direction.y * edge_direction.y +
                       edge_direction.z * edge_direction.z;
        if (distance < query->collision_radius_sq) {
            distance = NuFsqrt(distance);
            query = (TerrainQuery_s *)TerI;
            if (distance - query->collision_radius < query->hit_time) {
                NUVEC rotated;
                RotateVec(&edge_direction, &rotated);
                const NUVEC &surface_normal = query->working_surface->normals[0];
                if (surface_normal.y * rotated.y + surface_normal.x * rotated.x + surface_normal.z * rotated.z < 0.0) {
                    query->hit_type = TERRAIN_HIT_TYPE_CYLINDER | TERRAIN_HIT_TYPE_SECOND_NORMAL;
                    query->hit_time = distance - query->collision_radius;
                    const f32 inverse_distance = 1.0f / distance;
                    query->movement_normal.x = -edge_direction.x * inverse_distance;
                    query->movement_normal.y = -edge_direction.y * inverse_distance;
                    query->movement_normal.z = -edge_direction.z * inverse_distance;
                    return 1;
                }
            }
        }
        return 0;
    }
#undef TERRAIN_CC_FIRST
#undef TERRAIN_CC_SECOND
    i32 CheckSphere(i32 vertex_index) {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        if (query->transformed_vertices[vertex_index].z < -query->collision_radius ||
            query->transformed_vertices[vertex_index].z > query->movement_length + query->collision_radius) {
            return 0;
        }

        f32 distance_sq = query->transformed_vertices[vertex_index].x * query->transformed_vertices[vertex_index].x +
                          query->transformed_vertices[vertex_index].y * query->transformed_vertices[vertex_index].y;
        if (distance_sq > query->collision_radius_sq) {
            return 0;
        }

        const f32 radial_extent = NuFsqrt(query->collision_radius_sq - distance_sq);
        query = (TerrainQuery_s *)TerI;
        if (query->transformed_vertices[vertex_index].z - radial_extent < 0.0f ||
            query->transformed_vertices[vertex_index].z - radial_extent > query->movement_length) {
            distance_sq += query->transformed_vertices[vertex_index].z * query->transformed_vertices[vertex_index].z;
            if (distance_sq < query->collision_radius_sq) {
                const f32 distance = NuFsqrt(distance_sq);
                query = (TerrainQuery_s *)TerI;
                const f64 overlap_time = distance - query->collision_radius;
                if (overlap_time < query->hit_time) {
                    TERRAIN_SHAPE *surface = query->working_surface;
                    const f32 primary_distance =
                        (query->local_start.x - surface->vertices[vertex_index].x) * surface->normals[0].x +
                        (query->local_start.y - surface->vertices[vertex_index].y) * surface->normals[0].y +
                        (query->local_start.z - surface->vertices[vertex_index].z) * surface->normals[0].z;
                    if (primary_distance > 0.0f) {
                        query->hit_time = overlap_time;
                        const f32 inverse_distance = TerrainDivideNonzeroPC(1.0f, distance);
                        query->hit_type = TERRAIN_HIT_TYPE_VERTEX | TERRAIN_HIT_TYPE_SECOND_NORMAL;
                        query->movement_normal.x = -query->transformed_vertices[vertex_index].x * inverse_distance;
                        query->movement_normal.y = -query->transformed_vertices[vertex_index].y * inverse_distance;
                        query->movement_normal.z = -query->transformed_vertices[vertex_index].z * inverse_distance;
                        return 1;
                    }
                }
            }
            return 0;
        }

        const f32 hit_time = (query->transformed_vertices[vertex_index].z - radial_extent) / query->movement_length;
        if (hit_time < query->hit_time) {
            query->hit_time = hit_time;
            query->hit_type = TERRAIN_HIT_TYPE_VERTEX;
            query->movement_normal.x = -query->transformed_vertices[vertex_index].x;
            query->movement_normal.y = -query->transformed_vertices[vertex_index].y;
            query->movement_normal.z = -radial_extent;
            return 1;
        }
        return 0;
    }
    i32 CheckSphereTer(NUVEC *position, f32 radius) {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        if (position->z < -query->collision_radius - radius ||
            position->z > radius + query->collision_radius + query->movement_length) {
            return 0;
        }

        f32 distance_sq = position->x * position->x + position->y * position->y;
        const f32 combined_radius_sq = (radius + query->collision_radius) * (radius + query->collision_radius);
        if (distance_sq > combined_radius_sq) {
            return 0;
        }

        const f32 radial_extent = NuFsqrt(combined_radius_sq - distance_sq);
        if (position->z - radial_extent < 0.0f ||
            position->z - radial_extent > ((TerrainQuery_s *)TerI)->movement_length) {
            distance_sq += position->z * position->z;
            if (distance_sq < combined_radius_sq) {
                const f32 inverse_distance = TerrainDivideNonzeroPC(1.0f, NuFsqrt(distance_sq));
                query = (TerrainQuery_s *)TerI;
                query->hit_time = 0.0f;
                query->hit_type = TERRAIN_HIT_TYPE_SPHERE | TERRAIN_HIT_TYPE_SECOND_NORMAL;
                query->movement_normal.x = -position->x * inverse_distance;
                query->movement_normal.y = -position->y * inverse_distance;
                query->movement_normal.z = -position->z * inverse_distance;
                return 1;
            }
            return 0;
        }

        query = (TerrainQuery_s *)TerI;
        const f32 hit_time = (position->z - radial_extent) / query->movement_length;
        if (hit_time < query->hit_time) {
            query->hit_time = hit_time;
            query->hit_type = TERRAIN_HIT_TYPE_SPHERE;
            query->movement_normal.x = -position->x;
            query->movement_normal.y = -position->y;
            query->movement_normal.z = -radial_extent;
            return 1;
        }
        return 0;
    }
#define TERRAIN_HP_FACE(start, end, origin, normal, edge_a, edge_b, compare, steep_shift, early_time)                  \
    contact_point.x =                                                                                                  \
        query->local_start.x - surface->normals[normal].x * query->collision_radius - surface->vertices[origin].x;     \
    contact_point.y =                                                                                                  \
        query->local_start.y - surface->normals[normal].y * query->collision_radius - surface->vertices[origin].y;     \
    contact_point.z =                                                                                                  \
        query->local_start.z - surface->normals[normal].z * query->collision_radius - surface->vertices[origin].z;     \
    test_face = 1;                                                                                                     \
    if ((start) > 0.0f && (end) < 0.0f) {                                                                              \
        candidate_time = (start) / ((start) - (end));                                                                  \
        contact_point.x += query->movement.x * candidate_time;                                                         \
        contact_point.y += query->movement.y * candidate_time;                                                         \
        contact_point.z += query->movement.z * candidate_time;                                                         \
    } else if ((start) <= 0.0f && (end) <= 0.0f && (start) >= -query->collision_radius &&                              \
               (end) >= -query->collision_radius) {                                                                    \
        candidate_time = (start);                                                                                      \
        contact_point.x -= (start) * surface->normals[normal].x;                                                       \
        contact_point.y -= (start) * surface->normals[normal].y;                                                       \
        contact_point.z -= (start) * surface->normals[normal].z;                                                       \
    } else {                                                                                                           \
        test_face = 0;                                                                                                 \
    }                                                                                                                  \
    if (test_face != 0 &&                                                                                              \
        InsidePolLines(contact_point.x, contact_point.y, contact_point.z,                                              \
                       surface->vertices[edge_a].x - surface->vertices[origin].x,                                      \
                       surface->vertices[edge_a].y - surface->vertices[origin].y,                                      \
                       surface->vertices[edge_a].z - surface->vertices[origin].z,                                      \
                       surface->vertices[edge_b].x - surface->vertices[origin].x,                                      \
                       surface->vertices[edge_b].y - surface->vertices[origin].y,                                      \
                       surface->vertices[edge_b].z - surface->vertices[origin].z, &surface->normals[normal]) != 0 &&   \
        candidate_time compare query->hit_time) {                                                                      \
        if (early_time) {                                                                                              \
            query->hit_time = candidate_time;                                                                          \
        }                                                                                                              \
        if ((start) > 0.0f) {                                                                                          \
            query->hit_type = TERRAIN_HIT_TYPE_FACE;                                                                   \
        } else {                                                                                                       \
            query->hit_type = TERRAIN_HIT_TYPE_FACE | TERRAIN_HIT_TYPE_SECOND_NORMAL;                                  \
        }                                                                                                              \
        collision_found = 1;                                                                                           \
        if (!(early_time)) {                                                                                           \
            query->hit_time = candidate_time;                                                                          \
        }                                                                                                              \
        query->movement_normal.x = surface->normals[normal].x;                                                         \
        query->movement_normal.y = surface->normals[normal].y;                                                         \
        query->movement_normal.z = surface->normals[normal].z;                                                         \
        query->shape_adjusted = (surface->normal_flags >> (steep_shift)) & 1;                                          \
    }
#define TERRAIN_HP_EDGE(first, second, keep, edge, flags_expr)                                                         \
    if (CheckCylinder(first, second, &vertex_mask, keep) != 0) {                                                       \
        TerrainQuery_s *edge_query = (TerrainQuery_s *)TerI;                                                           \
        edge_query->hit_edge = edge;                                                                                   \
        collision_found |= 1;                                                                                          \
        edge_query->shape_adjusted = (flags_expr) & 1;                                                                 \
    }
#define TERRAIN_HP_VERTEX(index, flags_expr)                                                                           \
    if ((vertex_mask & (1 << (index))) != 0) {                                                                         \
        const i32 vertex_hit = CheckSphere(index);                                                                     \
        query = (TerrainQuery_s *)TerI;                                                                                \
        if (vertex_hit != 0) {                                                                                         \
            collision_found = 1;                                                                                       \
            query->shape_adjusted = (flags_expr) & 1;                                                                  \
        }                                                                                                              \
    }
    i32 HitPoly(f32 primary_start, f32 primary_end, f32 secondary_start, f32 secondary_end, TERRAIN_SHAPE *surface) {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        i32 collision_found = 0;
        i32 test_face;
        f32 candidate_time;
        NUVEC contact_point;

        TERRAIN_HP_FACE(primary_start, primary_end, 0, 0, 1, 2, <=, 0, 0)
        if (surface->normals[1].y < 65536.0f) {
            TERRAIN_HP_FACE(secondary_start, secondary_end, 3, 1, 2, 1, <, 1, 1)
        }

        if (query->collision_radius != 0.0f) {
            query->working_surface = surface;
            i32 vertex_mask = 0xf;
            DeRotateTerrain(surface);

            if (primary_start >= -query->collision_radius) {
                TERRAIN_HP_EDGE(0, 1, 0xc, TERRAIN_HIT_EDGE_0_1, surface->normal_flags)
                TERRAIN_HP_EDGE(2, 0, 0xa, TERRAIN_HIT_EDGE_2_0, surface->normal_flags)
            }
            i32 hit = CheckCylinder(1, 2, &vertex_mask, 0x9);
            query = (TerrainQuery_s *)TerI;
            if (hit != 0) {
                query->hit_edge = TERRAIN_HIT_EDGE_1_2;
                collision_found |= 1;
                query->shape_adjusted = ((surface->normal_flags >> 1) | surface->normal_flags) & 1;
            }

            if (surface->normals[1].y < 65536.0f && secondary_start >= -query->collision_radius) {
                TERRAIN_HP_EDGE(1, 3, 0x5, TERRAIN_HIT_EDGE_1_3, surface->normal_flags >> 1)
                hit = CheckCylinder(3, 2, &vertex_mask, 0x3);
                query = (TerrainQuery_s *)TerI;
                if (hit != 0) {
                    query->hit_edge = TERRAIN_HIT_EDGE_3_2;
                    collision_found |= 1;
                    query->shape_adjusted = (surface->normal_flags >> 1) & 1;
                }
                TERRAIN_HP_VERTEX(3, surface->normal_flags >> 1)
            }

            if (primary_start >= -query->collision_radius) {
                TERRAIN_HP_VERTEX(0, surface->normal_flags)
                TERRAIN_HP_VERTEX(1, (surface->normal_flags >> 1) | surface->normal_flags)
            }
            TERRAIN_HP_VERTEX(2, (surface->normal_flags >> 1) | surface->normal_flags)
        }

        if (collision_found != 0) {
            query->surface = surface;
        }
        query->unclamped_hit_time = query->hit_time;
        if (query->hit_time < 0.0f) {
            query->hit_time = 0.0f;
        }
        return collision_found;
    }
#undef TERRAIN_HP_FACE
#undef TERRAIN_HP_EDGE
#undef TERRAIN_HP_VERTEX
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
#define TERRAIN_HT_DISTANCE(point, normal, vertex, plane)                                                              \
    (((point).y - surface->vertices[vertex].y) * surface->normals[plane].y +                                           \
     ((point).x - surface->vertices[vertex].x) * surface->normals[plane].x +                                           \
     ((point).z - surface->vertices[vertex].z) * surface->normals[plane].z)
    i32 HitTerrain() {
        f32 primary_start_distance = 0.0f;
        f32 secondary_start_distance = 0.0f;
        f32 secondary_end_distance = 0.0f;
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        i16 *scan = static_cast<i16 *>(TerrOverRideScan);
        if (scan == NULL) {
            scan = reinterpret_cast<i16 *>(query->scan_list_storage);
        }

        const f32 radius = query->collision_radius;
        i32 collision_found = 0;
        query->shape_adjusted = 0;
        query->hit_time = 999.9f;
        query->hit_type = TERRAIN_HIT_TYPE_NONE;
        query->terrain_group_index = -1;

        HitWallSpline();

        query = (TerrainQuery_s *)TerI;
        const f64 horizontal_length =
            NuFsqrt(query->movement.x * query->movement.x + query->movement.z * query->movement.z);
        query = (TerrainQuery_s *)TerI;
        query->horizontal_movement_length = horizontal_length;

        for (;;) {
            while (scan[0] > 0) {
                TERRAIN_GROUP *groups = ((TERRAIN_SET_PC *)CurrentTerrain)->groups;
                TERRAIN_SHAPE **entry = reinterpret_cast<TERRAIN_SHAPE **>(scan + 2);
                query->local_start.x = query->position.x - groups[scan[1]].origin.x;
                query->local_start.y = query->position.y - groups[scan[1]].origin.y;
                query->local_start.z = query->position.z - groups[scan[1]].origin.z;
                query->local_end.x = query->movement.x + query->position.x - groups[scan[1]].origin.x;
                query->local_end.y = query->movement.y + query->position.y - groups[scan[1]].origin.y;
                query->local_end.z = query->movement.z + query->position.z - groups[scan[1]].origin.z;

                for (i32 count = scan[0]; count > 0; --count, ++entry) {
                    TERRAIN_SHAPE *surface = *entry;
                    i32 crosses_surface = 0;
                    const f32 primary_end_distance =
                        TERRAIN_HT_DISTANCE(query->local_end, surface, 0, 0) - radius - query->compare_epsilon;
                    if (primary_end_distance < 0.0f) {
                        primary_start_distance = TERRAIN_HT_DISTANCE(query->local_start, surface, 0, 0) - radius;
                        if (primary_start_distance > -radius) {
                            crosses_surface = 1;
                        }
                    }
                    if (surface->normals[1].y < 65536.0f) {
                        secondary_end_distance =
                            TERRAIN_HT_DISTANCE(query->local_end, surface, 3, 1) - radius - query->compare_epsilon;
                        if (secondary_end_distance < 0.0f) {
                            secondary_start_distance = TERRAIN_HT_DISTANCE(query->local_start, surface, 3, 1) - radius;
                            if (secondary_start_distance > -radius) {
                                crosses_surface = 1;
                            }
                        }
                    }

                    if (crosses_surface != 0) {
                        const i32 hit = HitPoly(primary_start_distance, primary_end_distance, secondary_start_distance,
                                                secondary_end_distance, surface);
                        query = (TerrainQuery_s *)TerI;
                        if (hit != 0) {
                            query->terrain_group_index = scan[1];
                            collision_found = 1;
                            if ((query->hit_type & TERRAIN_HIT_TYPE_SECOND_NORMAL) != 0 &&
                                (query->surface->flags & TERRAIN_SHAPE_FLAG_MATERIAL_ALIAS) != 0) {
                                PlatCrush = query->surface->material[0] + 1;
                            }
                        }
                    }
                }
                scan = reinterpret_cast<i16 *>(entry);
            }

            if (scan[0] >= 0) {
                break;
            }
            do {
                scan += abs(scan[0]) * 2 + 2;
            } while (scan[0] < 0);
        }

        for (i32 sphere_index = 0; sphere_index < curSphereter; ++sphere_index) {
            NUVEC sphere_position = SphereData[sphere_index].position;
            sphere_position.y =
                query->inverse_object_scale * sphere_position.y - query->object_scale * query->collision_radius;
            DeRotatePoint(&sphere_position);
            collision_found |= CheckSphereTer(&sphere_position, SphereData[sphere_index].radius);
            query = (TerrainQuery_s *)TerI;
        }

        if (query->hit_type != TERRAIN_HIT_TYPE_NONE && query->terrain_group_index != -1) {
            TERRAIN_GROUP *group = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[query->terrain_group_index];
            if (group->chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY) {
                plathitid = group->scene_index;
            }
        }

        TerrOverRideScan = NULL;
        return collision_found;
    }
#undef TERRAIN_HT_DISTANCE
#pragma auto_inline(off)
    void NewTerrStoreAnyInfo() {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        if (query->surface == NULL || query->terrain_group_index == -1) {
            return;
        }

        if (query->surface->material[0] != 0) {
            TerrPolyInfo.material[0] = query->surface->material[0];
        }
        if (query->surface->material[1] != 0) {
            TerrPolyInfo.material[1] = query->surface->material[1];
        }
        if (query->surface->flags != 0) {
            TerrPolyInfo.flags = query->surface->flags;
        }
        if (query->surface->normal_flags != 0) {
            TerrPolyInfo.normal_flags = query->surface->normal_flags;
        }
    }
#pragma auto_inline(on)
    void TerrainMoveImpactData() {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        switch (query->hit_type) {
            case TERRAIN_HIT_TYPE_FACE:
            case TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_VERTEX:
            case TERRAIN_HIT_TYPE_SPHERE:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_FACE:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_VERTEX:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE:
                if (query->terrain_group_index != -1) {
                    NewTerrStoreAnyInfo();
                    const i32 group_index = query->terrain_group_index;
                    TerrPoly = &TerrPolyInfo;
                    TerrainShadowStatusPc = group_index;
                }
                break;
        }
    }
    void TerrainImpactNorm() {
        TerrainMoveImpactData();

        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        switch (query->hit_type) {
            case TERRAIN_HIT_TYPE_SPHERE:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE:
                terrhitflags |= 4;
            case TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_VERTEX:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_VERTEX: {
                NUVEC *normal = &query->movement_normal;
                RotateVec(normal, normal);
                // Curved hits are produced in collision-height-scaled space and
                // need to be transformed back.
                if ((query->hit_type & TERRAIN_HIT_TYPE_SECOND_NORMAL) == 0) {
                    normal->x *= query->inverse_collision_radius;
                    query->movement_normal.y *= query->inverse_collision_radius;
                    query->movement_normal.z *= query->inverse_collision_radius;
                }
            }
            case TERRAIN_HIT_TYPE_FACE:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_FACE:
                if (query->object_scale == 1.0f) {
                    query->impact_normal.x = query->movement_normal.x;
                    query->impact_normal.y = query->movement_normal.y;
                    query->impact_normal.z = query->movement_normal.z;
                    return;
                }
                {
                    const f32 normal_length =
                        NuFsqrt(query->movement_normal.x * query->movement_normal.x +
                                query->movement_normal.y * query->movement_normal.y * query->inverse_object_scale_sq +
                                query->movement_normal.z * query->movement_normal.z);
                    const f32 inverse_normal_length = normal_length != 0.0f ? 1.0f / normal_length : 0.0f;
                    query = (TerrainQuery_s *)TerI;
                    query->impact_normal.x = query->movement_normal.x * inverse_normal_length;
                    query->impact_normal.y =
                        query->movement_normal.y * query->inverse_object_scale * inverse_normal_length;
                    query->impact_normal.z = query->movement_normal.z * inverse_normal_length;
                }
                break;
        }
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

        switch (hit_type) {
            case TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_VERTEX:
            case TERRAIN_HIT_TYPE_SPHERE:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_VERTEX:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE:
                RotateVec(&impact_normal, &impact_normal);
                if ((hit_type & TERRAIN_HIT_TYPE_SECOND_NORMAL) == 0) {
                    impact_normal.x *= query->inverse_collision_radius;
                    impact_normal.y *= query->inverse_collision_radius;
                    impact_normal.z *= query->inverse_collision_radius;
                }
            case TERRAIN_HIT_TYPE_FACE:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_FACE:
                if (query->object_scale != 1.0f) {
                    const f32 inverse_normal_length = TerrainDivideNonzeroPC(
                        1.0f, NuFsqrt(impact_normal.x * impact_normal.x +
                                      impact_normal.y * impact_normal.y * query->inverse_object_scale_sq +
                                      impact_normal.z * impact_normal.z));
                    query = (TerrainQuery_s *)TerI;
                    impact_normal.x *= inverse_normal_length;
                    impact_normal.y *= query->inverse_object_scale * inverse_normal_length;
                    impact_normal.z *= inverse_normal_length;
                }

                PlatImpactId = ((TERRAIN_SET_PC *)CurrentTerrain)->groups[query->terrain_group_index].scene_index;
                PlatImpactTer = *query->surface;
                PlatImpactNorm = impact_normal;
                break;
        }
    }
    void TerrainImpact(NUVEC *position, NUVEC *movement, u8 *hit_flags) {
        f32 walkable_normal_y = 0.0f;
        TERRAIN_TRACK_SLOT_PC *track;
        TerrainMoveImpactData();

        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
#define TERRAIN_TI_GROUP (((TERRAIN_SET_PC *)CurrentTerrain)->groups[query->terrain_group_index])
#define TERRAIN_TI_PLATFORM_GROUP                                                                                      \
    (query->terrain_group_index != -1 && TERRAIN_TI_GROUP.chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY)
#define TERRAIN_TI_WALL_OVERRIDE                                                                                       \
    (query->terrain_group_index >= 0 &&                                                                                \
     (query->surface->normal_flags & TERRAIN_SURFACE_CLASS_MASK) == TERRAIN_SURFACE_CLASS_WALL_OVERRIDE)
#define TERRAIN_TI_TRACK_WALL()                                                                                        \
    track = CurTrackInfo;                                                                                              \
    if (track == NULL) {                                                                                               \
        track = AllocTerrId();                                                                                         \
        track->id = query->hit_flags;                                                                                  \
        CurTrackInfo = track;                                                                                          \
        track->flags = TERRAIN_TRACK_FLAG_NONE;                                                                        \
        track->platform_index = 0;                                                                                     \
        track->platform_contact_state = TERRAIN_TRACK_CONTACT_NONE;                                                    \
    }                                                                                                                  \
    track->wall_contact_state = TERRAIN_TRACK_CONTACT_ACTIVE;                                                          \
    track->impact_normal = query->impact_normal
        if (query->hit_type != TERRAIN_HIT_TYPE_NONE) {
            TERRAIN_SHAPE *surface = query->surface;
            if (surface != NULL) {
                walkable_normal_y = query->movement_normal.x * surface->normals[0].x +
                                    query->movement_normal.y * surface->normals[0].y +
                                    query->movement_normal.z * surface->normals[0].z;
            } else {
                walkable_normal_y = 0.707f;
            }

            if (TERRAIN_TI_PLATFORM_GROUP) {
                if ((query->hit_type & TERRAIN_HIT_TYPE_CLASS_MASK) > TERRAIN_HIT_TYPE_FACE &&
                    walkable_normal_y < (f64)0.95f) {
                    walkable_normal_y = 0.98f;
                } else {
                    walkable_normal_y = 0.707f;
                }
            } else if (query->shape_adjusted != 0) {
                walkable_normal_y = 1.1f;
            } else {
                walkable_normal_y = -1.1f;
            }

            if (query->terrain_group_index >= 0 &&
                (surface->normal_flags & TERRAIN_SURFACE_CLASS_MASK) == TERRAIN_SURFACE_CLASS_WALL_OVERRIDE) {
                query->position.x += query->movement_normal.x * (f64)0.0015f;
                query->position.z += query->movement_normal.z * (f64)0.0015f;
                walkable_normal_y = 1.1f;
            }
            if (wallover != 0.0f) {
                walkable_normal_y = wallover;
            }
        }

        switch (query->hit_type) {
            case TERRAIN_HIT_TYPE_NONE:
                hit_flags[0] = 0;
                position->x = query->movement.x + query->position.x;
                position->y = (query->movement.y + query->position.y) * query->object_scale -
                              query->collision_radius * query->object_scale;
                position->z = query->movement.z + query->position.z;
                query->flags &= ~TERRAIN_QUERY_FLAG_PREVIOUS_NORMAL;
                break;
            case TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_VERTEX:
            case TERRAIN_HIT_TYPE_SPHERE:
                if (!TERRAIN_TI_WALL_OVERRIDE) {
                    walkable_normal_y = 0.707f;
                }
                // Fall through to the shared first-normal response.
            case TERRAIN_HIT_TYPE_FACE: {
                query->hit_time -= query->separation_epsilon;
                if (query->hit_time < 0.0f) {
                    query->hit_time = 0.0f;
                }

                const f64 travelled_x = query->movement.x * query->hit_time;
                query->position.x += travelled_x;
                const f64 travelled_y = query->movement.y * query->hit_time;
                query->position.y += travelled_y;
                const f64 travelled_z = query->movement.z * query->hit_time;
                query->position.z += travelled_z;
                query->movement.x -= travelled_x;
                query->movement.y -= travelled_y;
                query->movement.z -= travelled_z;

                if (query->impact_normal.y < walkable_normal_y) {
                    hit_flags[0] = 0;
                    terrhitflags |= TERRAIN_IMPACT_RESULT_WALL;
                    if (!TERRAIN_TI_PLATFORM_GROUP) {
                        terrhitflags |= TERRAIN_IMPACT_RESULT_STATIC_WALL;
                    }

                    query->position.x += query->movement_normal.x * (f64)0.0006f;
                    query->position.z += query->movement_normal.z * (f64)0.0006f;
                    query->movement.x += query->movement_normal.x * (f64)0.0006f;
                    query->movement.z += query->movement_normal.z * (f64)0.0006f;
                    movement->x += query->movement_normal.x * (f64)0.0009f;
                    movement->z += query->movement_normal.z * (f64)0.0009f;

                    FullDeflect(&query->movement_normal, &query->movement, &query->movement);
                    FullDeflectSmallY(&query->impact_normal, movement, movement);

                    TERRAIN_TI_TRACK_WALL();
                    TerrWallNorm = query->impact_normal;
                } else {
                    FullDeflect(&query->movement_normal, &query->movement, &query->movement);
                    FullDeflect(&query->impact_normal, movement, movement);

                    terrhitflags |= TERRAIN_IMPACT_RESULT_GROUND;
                    hit_flags[0] = 1;
                    hit_flags[1] = 1;
                    query->position.y += query->movement_normal.y * (f64)0.0003f;

                    if (TERRAIN_TI_PLATFORM_GROUP) {
                        ((TERRAIN_SET_PC *)CurrentTerrain)->platforms[TERRAIN_TI_GROUP.scene_index].flags |=
                            TERRAIN_PLATFORM_FLAG_COLLIDED;
                    }
                }
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
                    if (response_normal.x * query->previous_movement_normal.x +
                            response_normal.y * query->previous_movement_normal.y +
                            response_normal.z * query->previous_movement_normal.z <
                        0.0) {
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

                if (query->impact_normal.y < walkable_normal_y) {
                    const f64 contact_push = (f64)0.0035f - query->unclamped_hit_time * (f64)1.05f;
                    query->position.x += response_normal.x * contact_push;
                    query->position.y += response_normal.y * contact_push;
                    query->position.z += response_normal.z * contact_push;
                    const f64 movement_push = (f64)0.0035f - query->unclamped_hit_time * (f64)0.35f;
                    query->movement.x += response_normal.x * movement_push;
                    query->movement.y += response_normal.y * movement_push;
                    query->movement.z += response_normal.z * movement_push;
                    hit_flags[0] = 1;

                    FullDeflect(&response_normal, &query->movement, &query->movement);
                    FullDeflectSmallY(&query->impact_normal, movement, movement);

                    TERRAIN_TI_TRACK_WALL();
                } else {
                    const f64 contact_push = (f64)0.0035f - query->unclamped_hit_time * (f64)1.05f;
                    query->position.x += response_normal.x * contact_push;
                    query->position.y += response_normal.y * contact_push;
                    query->position.z += response_normal.z * contact_push;
                    query->position.y += response_normal.y * (f64)0.01f;
                    query->movement.y += response_normal.y * (f64)0.01f;

                    FullDeflect(&response_normal, &query->movement, &query->movement);
                    FullDeflect(&query->impact_normal, movement, movement);
                }
                query->flags |= TERRAIN_QUERY_FLAG_PREVIOUS_NORMAL;
                query->previous_movement_normal = query->movement_normal;
                break;
            }
        }

        if (query->hit_type != TERRAIN_HIT_TYPE_NONE && query->hit_type < TERRAIN_HIT_TYPE_SECOND_NORMAL &&
            TERRAIN_TI_PLATFORM_GROUP && query->impact_normal.y >= walkable_normal_y) {
            PlatformConnect(reinterpret_cast<char *>(query->hit_flags), &query->movement, movement,
                            TERRAIN_TI_GROUP.scene_index);
        }

        if (TerImpactData != NULL && *TerImpactDataCount < TerImpactDataMax &&
            query->hit_type != TERRAIN_HIT_TYPE_NONE) {
            TERRAIN_IMPACT_RECORD *records = static_cast<TERRAIN_IMPACT_RECORD *>(TerImpactData);
            i32 *count = TerImpactDataCount;
            records[*count].position.x = query->position.x - query->movement_normal.x * query->collision_radius;
            records[*count].position.y =
                (query->position.y - query->movement_normal.y * query->collision_radius) * query->object_scale;
            records[*count].position.z = query->position.z - query->movement_normal.z * query->collision_radius;
            records[*count].normal = query->movement_normal;
            if (query->surface != NULL) {
                records[*count].material[0] = query->surface->material[0];
                records[*count].material[1] = query->surface->material[1];
                records[*count].flags = query->surface->flags;
                records[*count].normal_flags = query->surface->normal_flags;
            } else if (TerrWallInfo != 0) {
                records[*count].material[0] = TerrWallTab[0];
                records[*count].material[1] = TerrWallTab[1];
                records[*count].flags = 0;
                records[*count].normal_flags = 0;
            }
            ++*count;
        }
#undef TERRAIN_TI_TRACK_WALL
#undef TERRAIN_TI_WALL_OVERRIDE
#undef TERRAIN_TI_PLATFORM_GROUP
#undef TERRAIN_TI_GROUP
    }
    i32 TerrainImpactPlatform(unsigned char *hit_flags) {
        TerrainMoveImpactData();

        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        switch (query->hit_type) {
            case TERRAIN_HIT_TYPE_NONE:
                hit_flags[0] = 0;
                query->position.x += query->movement.x;
                query->position.y += query->movement.y;
                query->position.z += query->movement.z;
                return 0;
            case TERRAIN_HIT_TYPE_FACE:
            case TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_VERTEX:
            case TERRAIN_HIT_TYPE_SPHERE:
                query->hit_time -= query->separation_epsilon;
                if (query->hit_time < 0.0f) {
                    query->hit_time = 0.0f;
                }

                query->position.x += query->movement.x * query->hit_time;
                query->position.y += query->movement.y * query->hit_time;
                query->position.z += query->movement.z * query->hit_time;

                if (query->impact_normal.y < 0.707f) {
                    hit_flags[0] = 0;
                    return 0;
                }

                hit_flags[0] = 1;
                hit_flags[1] = 1;
                query->position.y += query->movement_normal.y * 0.004f;
                return 0;
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_FACE:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_VERTEX:
            case TERRAIN_HIT_TYPE_SECOND_NORMAL | TERRAIN_HIT_TYPE_SPHERE:
                return 1;
        }
        return 1;
    }
    void FullDeflect(NUVEC *normal, NUVEC *movement, NUVEC *result) {
        // Move just far enough out of the surface to retain a small separation.
        const f32 reverse_x = -movement->x;
        const f32 reverse_y = -movement->y;
        const f32 reverse_z = -movement->z;
        const f32 deflection = reverse_y * normal->y + reverse_x * normal->x + reverse_z * normal->z + 0.0003f;
        result->x = normal->x * deflection + movement->x;
        result->y = normal->y * deflection + movement->y;
        result->z = normal->z * deflection + movement->z;
    }
    void FullDeflectSmallY(NUVEC *normal, NUVEC *movement, NUVEC *result) {
        // Move just far enough out of the surface to retain a small separation.
        const f32 reverse_x = -movement->x;
        const f32 reverse_y = -movement->y;
        const f32 reverse_z = -movement->z;
        const f32 deflection = reverse_y * normal->y + reverse_x * normal->x + reverse_z * normal->z + 0.0003f;
        result->x = normal->x * deflection + movement->x;
        result->y = normal->y * deflection * TerrWallDeflectYScale + movement->y;
        result->z = normal->z * deflection + movement->z;
    }
    i32 FullDeflectTest(NUVEC *normal, NUVEC *movement, NUVEC *result) {
        const f32 reverse_x = -movement->x;
        const f32 reverse_y = -movement->y;
        const f32 reverse_z = -movement->z;
        const f32 deflection = reverse_y * normal->y + reverse_x * normal->x + reverse_z * normal->z + 0.0003f;
        result->x = normal->x * deflection + movement->x;
        result->y = normal->y * deflection + movement->y;
        result->z = normal->z * deflection + movement->z;
        return deflection > 0.0f;
    }
    void PlatformConnect(char *track_id, NUVEC *position_delta, NUVEC *movement_delta, i32 platform_index) {
        TERRAIN_TRACK_SLOT_PC *track = CurTrackInfo;
        if (track != NULL) {
            track->flags |= TERRAIN_TRACK_FLAG_CONNECTED;
            track->platform_index = static_cast<i16>(platform_index);
            track->platform_contact_state = TERRAIN_TRACK_CONTACT_ACTIVE;
        } else {
            track = AllocTerrId();
            CurTrackInfo = track;
            if (track != NULL) {
                track->id = track_id;
                track->flags |= TERRAIN_TRACK_FLAG_CONNECTED;
                track->platform_index = static_cast<i16>(platform_index);
                track->platform_contact_state = TERRAIN_TRACK_CONTACT_ACTIVE;
                position_delta->y = 0.0f;
                movement_delta->y = 0.0f;
            }
        }

        TerrainShadowGroupIndexPc = ((TERRAIN_SET_PC *)CurrentTerrain)->platforms[platform_index].terrain_group_index;
    }
    i32 PlatformChecks(i32 count, NUVEC *movement) {
        TerrainQuery_s *old_query = (TerrainQuery_s *)TerI;
        if (old_query->scan_list == NULL || *static_cast<i16 *>(old_query->scan_list) == 0 || CurTrackInfo == NULL ||
            CurTrackInfo->platform_contact_state == 0 || TerConTol == 0.0f)
            return count;
#define TERRAIN_PC_QUERY ((TerrainQuery_s *)TerI)
#define TERRAIN_PC_TRACK_PLATFORM (((TERRAIN_SET_PC *)CurrentTerrain)->platforms[track->platform_index])
#define TERRAIN_PC_TRACK_MATRIX (static_cast<NUMTX *>(TERRAIN_PC_TRACK_PLATFORM.scene_object))
        TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
        i16 *input = static_cast<i16 *>(old_query->scan_list);
        TerI = query;
        i16 *output = reinterpret_cast<i16 *>(query->scan_list_storage);
        while (*input > 0) {
            output[0] = input[0];
            i32 remaining = input[0];
            output[1] = input[1];
            output += 2;
            input += 2;
            for (; remaining > 0; --remaining) {
                *output++ = *input++;
                *output++ = *input++;
            }
        }
        output[0] = 0;
        output[1] = 0;
        query->collision_radius = old_query->collision_radius;
        query->object_scale = old_query->object_scale;
        query->inverse_object_scale_sq = old_query->inverse_object_scale_sq;
        query->inverse_collision_radius = old_query->inverse_collision_radius;
        query->separation_epsilon = old_query->separation_epsilon;
        query->inverse_object_scale = old_query->inverse_object_scale;
        query->compare_epsilon = old_query->compare_epsilon;
        query->collision_radius_sq = old_query->collision_radius_sq;
        query->object_scale_sq = old_query->object_scale_sq;
        TerrainQuery_s *current = query;
        current->platform_position = old_query->position;
        TERRAIN_TRACK_SLOT_PC *track = CurTrackInfo;
        if (track != NULL && (track->flags & TERRAIN_TRACK_FLAG_CONNECTED)) {
            TERRAIN_PLATFORM *platform = &TERRAIN_PC_TRACK_PLATFORM;
            if (platform->flags & 1) {
                NuMtxInvRSS(&tertempmat, reinterpret_cast<NUMTX *>(platform));
                track = CurTrackInfo;
                tertempvec4.x = old_query->position.x - TERRAIN_PC_TRACK_PLATFORM.previous_matrix.m30;
                tertempvec4.y =
                    (old_query->position.y - TERRAIN_PC_QUERY->collision_radius) * TERRAIN_PC_QUERY->object_scale -
                    TERRAIN_PC_TRACK_PLATFORM.previous_matrix.m31;
                tertempvec4.z = old_query->position.z - TERRAIN_PC_TRACK_PLATFORM.previous_matrix.m32;
                tertempvec4.w = 1.0f;
                NuVec4MtxTransformVU0(&tertempvec4, &tertempvec4, &tertempmat);
                track = CurTrackInfo;
                NuVec4MtxTransformVU0(&tertempvec4, &tertempvec4, TERRAIN_PC_TRACK_MATRIX);
                current = TERRAIN_PC_QUERY;
                current->platform_position.x = tertempvec4.x;
                current->platform_position.y =
                    current->inverse_object_scale * tertempvec4.y + current->collision_radius;
                current->platform_position.z = tertempvec4.z;
            } else {
                query->platform_position.x +=
                    TERRAIN_PC_TRACK_MATRIX->m30 - TERRAIN_PC_TRACK_PLATFORM.previous_matrix.m30;
                query->platform_position.y +=
                    (TERRAIN_PC_TRACK_MATRIX->m31 - TERRAIN_PC_TRACK_PLATFORM.previous_matrix.m31) *
                    old_query->inverse_object_scale;
                query->platform_position.z +=
                    TERRAIN_PC_TRACK_MATRIX->m32 - TERRAIN_PC_TRACK_PLATFORM.previous_matrix.m32;
            }
        }
        current->platform_normal = current->platform_position;
        current->movement.x = 0.0f;
        current->movement.z = 0.0f;
        current->position = current->platform_position;
        const f32 tolerance = TerConTol;
        current->position.y += tolerance;
        current->movement.y = tolerance * -2.0;
        DerotateMovementVector();
        f32 saved_epsilon = TERRAIN_PC_QUERY->separation_epsilon;
        TERRAIN_PC_QUERY->separation_epsilon = 0.0f;
        HitTerrain();
        StorePlatImpact();
        TERRAIN_PC_QUERY->separation_epsilon = saved_epsilon;
        TerrainImpactNorm();
        u8 flags[2];
        TerrainImpactPlatform(flags);
        current = TERRAIN_PC_QUERY;
        if (current->hit_type != 0)
            ShadNorm = current->impact_normal;
        current->platform_position.y = current->position.y - current->collision_radius;
        TerI = old_query;
        TERRAIN_FLOAT_BITS_PC absolute_4;
        absolute_4.value = query->platform_normal.y - query->platform_position.y - old_query->collision_radius;
        absolute_4.bits &= 0x7fffffff;
        if (query->hit_type == 0 || absolute_4.value > TerConTol * 0.5 || query->movement_normal.y < 0.707f) {
            NuScratchRelease();
            return count;
        }
        {
            {
                TERRAIN_GROUP *group = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[query->terrain_group_index];
                i32 platform_index = group->scene_index;
                if (movement->y > TerConTol * 0.5) {
                    TERRAIN_PLATFORM *platform = &((TERRAIN_SET_PC *)CurrentTerrain)->platforms[platform_index];
                    if (movement->y > TerConTol * 0.5 + static_cast<NUMTX *>(platform->scene_object)->m31 -
                                          platform->previous_matrix.m31) {
                        NuScratchRelease();
                        return count;
                    }
                }
                if (query->movement_normal.y >= 0.707f && query->terrain_group_index >= 0 && group->chunk_type == 1)
                    ((TERRAIN_SET_PC *)CurrentTerrain)->platforms[group->scene_index].flags |= 2;
                old_query->platform_position.x = query->platform_position.x;
                track = CurTrackInfo;
                if (TERRAIN_PC_TRACK_PLATFORM.flags & 1)
                    old_query->platform_position.y = old_query->collision_radius + query->platform_position.y;
                else
                    old_query->platform_position.y =
                        old_query->collision_radius + query->platform_position.y + (f64)0.0025f;
                const f32 platform_z = query->platform_position.z;
                old_query->platform_position.z = platform_z;
                NUVEC direction;
                direction.x = old_query->platform_position.x - old_query->position.x;
                direction.y = old_query->platform_position.y - old_query->position.y;
                direction.z = platform_z - old_query->position.z;
                f32 distance =
                    NuFsqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
                direction.x = TerrainDivideNonzeroPC(direction.x, distance);
                direction.y = TerrainDivideNonzeroPC(direction.y, distance);
                direction.z = TerrainDivideNonzeroPC(direction.z, distance);
                NUVEC previous_position = TERRAIN_PC_QUERY->position;
                i32 clear = TerrainPlatformMoveCheck(&TERRAIN_PC_QUERY->platform_position,
                                                     &TERRAIN_PC_QUERY->platform_normal, platform_index, 1, 0);
                current = TERRAIN_PC_QUERY;
                if (current->hit_type != 0 && current->terrain_group_index >= 0) {
                    TERRAIN_GROUP *hit_group =
                        &((TERRAIN_SET_PC *)CurrentTerrain)->groups[current->terrain_group_index];
                    if (hit_group->chunk_type == 1)
                        CurTrackInfo->platform_index = hit_group->scene_index;
                    if (current->platform_normal.y * direction.y + current->platform_normal.x * direction.x +
                            current->platform_normal.z * direction.z <
                        TerrainOpposingContactThresholdPc)
                        clear = 1;
                }
                --count;
                if (clear == 0) {
                    if (count > 0) {
                        const f32 travelled = (current->position.x - previous_position.x) * direction.x +
                                              (current->position.y - previous_position.y) * direction.y +
                                              (current->position.z - previous_position.z) * direction.z;
                        if (distance > travelled &&
                            FullDeflectTest(&current->platform_normal, &direction, &current->platform_position)) {
                            const f32 dot = current->platform_position.y * direction.y +
                                            current->platform_position.x * direction.x +
                                            current->platform_position.z * direction.z;
                            if (dot > 0.0f) {
                                const f32 fraction = (distance - travelled) / dot;
                                if (fraction < (f64)0.8f) {
                                    current->platform_position.x =
                                        current->platform_position.x * fraction + current->position.x;
                                    current->platform_position.y =
                                        current->platform_position.y * fraction + current->position.y;
                                    current->platform_position.z =
                                        current->platform_position.z * fraction + current->position.z;
                                }
                            }
                        }
                    }
                } else {
                    TerrPoly = query->surface;
                    current->position = current->platform_position;
                    if (0.0f > movement->y) {
                        FullDeflect(&ShadNorm, &current->movement, &current->movement);
                        FullDeflect(&ShadNorm, movement, movement);
                    } else {
                        current->movement.y *= (f64)0.75f;
                    }
                    current->hit_flags[1] = 1;
                    PlatformConnect(reinterpret_cast<char *>(old_query->hit_flags), &old_query->movement, movement,
                                    ((TERRAIN_SET_PC *)CurrentTerrain)->groups[query->terrain_group_index].scene_index);
                }
                NuScratchRelease();
                TerI = old_query;
                return count;
            }
        }
#undef TERRAIN_PC_TRACK_MATRIX
#undef TERRAIN_PC_TRACK_PLATFORM
#undef TERRAIN_PC_QUERY
    }
    i32 TerrainPlatformEmbedded(NUVEC *movement_delta) {
        i32 transformed_count = 0;
        TerrainQuery_s *old_query = (TerrainQuery_s *)TerI;
        i32 platform_index = ((TERRAIN_SET_PC *)CurrentTerrain)->groups[old_query->terrain_group_index].scene_index;
#define TERRAIN_TPE_QUERY ((TerrainQuery_s *)TerI)
#define TERRAIN_TPE_PLATFORM (((TERRAIN_SET_PC *)CurrentTerrain)->platforms[platform_index])
#define TERRAIN_TPE_PREVIOUS (reinterpret_cast<NUMTX *>(&TERRAIN_TPE_PLATFORM.previous_matrix))
#define TERRAIN_TPE_MATRIX (static_cast<NUMTX *>(TERRAIN_TPE_PLATFORM.scene_object))
#define TERRAIN_TPE_GROUP_MATRIX                                                                                       \
    (static_cast<NUMTX *>(((TERRAIN_SET_PC *)CurrentTerrain)->platforms[group->scene_index].scene_object))
#define TERRAIN_TPE_SURFACE (&TerrainScaledShapeCursorPc[transformed_count])
#define TERRAIN_TPE_LOAD_VERTEX(index)                                                                                 \
    vertices[index].x = source->vertices[index].x;                                                                     \
    vertices[index].y = source->vertices[index].y;                                                                     \
    vertices[index].z = source->vertices[index].z;                                                                     \
    vertices[index].w = 0.0f
#define TERRAIN_TPE_STORE_VERTEX(index)                                                                                \
    surface->vertices[index].x = vertices[index].x;                                                                    \
    surface->vertices[index].z = vertices[index].z;                                                                    \
    surface->vertices[index].y = vertices[index].y
#define TERRAIN_TPE_STORE_SCALED(target, value)                                                                        \
    target.x = value.x;                                                                                                \
    target.z = value.z;                                                                                                \
    target.y = (value.y + group->origin.y) * query->inverse_object_scale - group->origin.y
#define TERRAIN_TPE_EDGES(tip, first, second)                                                                          \
    vertices[0].x = surface->vertices[first].x - surface->vertices[tip].x;                                             \
    vertices[0].y = surface->vertices[first].y - surface->vertices[tip].y;                                             \
    vertices[0].z = surface->vertices[first].z - surface->vertices[tip].z;                                             \
    vertices[1].x = surface->vertices[second].x - surface->vertices[tip].x;                                            \
    vertices[1].y = surface->vertices[second].y - surface->vertices[tip].y;                                            \
    vertices[1].z = surface->vertices[second].z - surface->vertices[tip].z
#define TERRAIN_TPE_CROSS(target, cross_x)                                                                             \
    {                                                                                                                  \
        const NUVEC4 *edge_a = &vertices[0];                                                                           \
        const NUVEC4 *edge_b = &vertices[1];                                                                           \
        NUVEC cross;                                                                                                   \
        cross.x = cross_x;                                                                                             \
        cross.y = edge_b->x * edge_a->z - edge_b->z * edge_a->x;                                                       \
        cross.z = edge_a->x * edge_b->y - edge_b->x * edge_a->y;                                                       \
        target = cross;                                                                                                \
    }
#define TERRAIN_TPE_FIRST_CROSS_X (edge_b->z * edge_a->y - edge_b->y * edge_a->z)
#define TERRAIN_TPE_SECOND_CROSS_X (edge_a->y * edge_b->z - edge_a->z * edge_b->y)
        TERRAIN_PLATFORM *platform = &TERRAIN_TPE_PLATFORM;
        if (platform->scene_object == NULL)
            return 1;
        if (TERRAIN_TPE_PREVIOUS->m30 == TERRAIN_TPE_MATRIX->m30 &&
            TERRAIN_TPE_PREVIOUS->m31 == TERRAIN_TPE_MATRIX->m31 &&
            TERRAIN_TPE_PREVIOUS->m32 == TERRAIN_TPE_MATRIX->m32) {
            if (!(platform->flags & 1))
                return 1;
            if (TERRAIN_TPE_PREVIOUS->m00 == TERRAIN_TPE_MATRIX->m00 &&
                TERRAIN_TPE_PREVIOUS->m01 == TERRAIN_TPE_MATRIX->m01 &&
                TERRAIN_TPE_PREVIOUS->m02 == TERRAIN_TPE_MATRIX->m02 &&
                TERRAIN_TPE_PREVIOUS->m10 == TERRAIN_TPE_MATRIX->m10 &&
                TERRAIN_TPE_PREVIOUS->m11 == TERRAIN_TPE_MATRIX->m11 &&
                TERRAIN_TPE_PREVIOUS->m12 == TERRAIN_TPE_MATRIX->m12 &&
                TERRAIN_TPE_PREVIOUS->m20 == TERRAIN_TPE_MATRIX->m20 &&
                TERRAIN_TPE_PREVIOUS->m21 == TERRAIN_TPE_MATRIX->m21 &&
                TERRAIN_TPE_PREVIOUS->m22 == TERRAIN_TPE_MATRIX->m22)
                return 1;
        }
        i32 saved_sphere = curSphereter;
        curSphereter = 0;
        TerrainScaledShapeCursorPc = static_cast<TERRAIN_SHAPE *>(ScaleTerrainT2);
        TerrainQuery_s *new_query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
        TerI = new_query;
        NUVEC4 *vertices = reinterpret_cast<NUVEC4 *>((reinterpret_cast<u32>(NuScratchAlloc32(0xd0)) + 31) & ~15u);
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        query->collision_radius = old_query->collision_radius;
        query->object_scale = old_query->object_scale;
        query->inverse_object_scale_sq = old_query->inverse_object_scale_sq;
        query->inverse_collision_radius = old_query->inverse_collision_radius;
        query->separation_epsilon = old_query->separation_epsilon;
        query->inverse_object_scale = old_query->inverse_object_scale;
        query->compare_epsilon = old_query->compare_epsilon;
        query->collision_radius_sq = old_query->collision_radius_sq;
        query->object_scale_sq = old_query->object_scale_sq;
        query->object_index = old_query->object_index;
        query->hit_flags = old_query->hit_flags;
        query->scan_result = old_query->scan_result;
        query->radius = old_query->radius;
        query->movement.x = (TERRAIN_TPE_PREVIOUS->m30 - TERRAIN_TPE_MATRIX->m30) * (f64)1.73f;
        query->movement.y =
            (TERRAIN_TPE_PREVIOUS->m31 - TERRAIN_TPE_MATRIX->m31) * (f64)1.73f * query->inverse_object_scale;
        query->movement.z = (TERRAIN_TPE_PREVIOUS->m32 - TERRAIN_TPE_MATRIX->m32) * (f64)1.73f;
        if (TERRAIN_TPE_PLATFORM.flags & 1) {
            NuMtxInvRSS(&tertempmat, TERRAIN_TPE_PREVIOUS);
            query = (TerrainQuery_s *)TerI;
            tertempvec4.x = old_query->position.x - TERRAIN_TPE_PREVIOUS->m30;
            tertempvec4.y =
                (old_query->position.y - query->collision_radius) * query->object_scale - TERRAIN_TPE_PREVIOUS->m31;
            tertempvec4.z = old_query->position.z - TERRAIN_TPE_PREVIOUS->m32;
            tertempvec4.w = 1.0f;
            NuVec4MtxTransformVU0(&tertempvec4, &tertempvec4, &tertempmat);
            NuVec4MtxTransformVU0(&tertempvec4, &tertempvec4, TERRAIN_TPE_MATRIX);
            query = (TerrainQuery_s *)TerI;
            query->movement.x = (old_query->position.x - tertempvec4.x) * (f64)1.3f;
            query->movement.y =
                (old_query->position.y - (query->inverse_object_scale * tertempvec4.y + query->collision_radius)) *
                (f64)1.3f;
            query->movement.z = (old_query->position.z - tertempvec4.z) * (f64)1.3f;
        }
        TERRAIN_SHAPE **cursor = reinterpret_cast<TERRAIN_SHAPE **>(query->scan_list_storage + 4);
        i16 *header = reinterpret_cast<i16 *>(query->scan_list_storage);
        query->movement.x -= old_query->movement_normal.x * (f64)0.01f;
        query->movement.y -= old_query->movement_normal.y * (f64)0.01f;
        query->movement.z -= old_query->movement_normal.z * (f64)0.01f;
        query->position.x = old_query->position.x - query->movement.x * (f64)0.91f;
        query->position.y = old_query->position.y - query->movement.y * (f64)0.91f;
        query->position.z = old_query->position.z - query->movement.z * (f64)0.91f;

        i32 count = 0;
        TERRAIN_GROUP *group = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[old_query->terrain_group_index];
        TERRAIN_SHAPE_BATCH *batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(group->data);
        if (1.0f == query->object_scale) {
            if (TERRAIN_TPE_PLATFORM.flags & 1) {
                while (batch->marker >= 0) {
                    TERRAIN_SHAPE *source = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                    for (i32 remaining = batch->shape_count; remaining > 0; --remaining, ++source) {
                        if (source->material[1] != 0)
                            continue;
                        TERRAIN_TPE_LOAD_VERTEX(0);
                        TERRAIN_TPE_LOAD_VERTEX(1);
                        TERRAIN_TPE_LOAD_VERTEX(2);
                        NuVec4MtxTransformVU0x3(vertices, vertices, TERRAIN_TPE_GROUP_MATRIX);
                        if (source->normals[1].y < 65535.0) {
                            TERRAIN_TPE_LOAD_VERTEX(3);
                            NuVec4MtxTransformVU0(&vertices[3], &vertices[3], TERRAIN_TPE_GROUP_MATRIX);
                        } else {
                            vertices[3] = vertices[2];
                        }
                        TERRAIN_SHAPE *surface = TERRAIN_TPE_SURFACE;
                        surface->packed_metadata = source->packed_metadata;
                        TERRAIN_TPE_STORE_VERTEX(0);
                        TERRAIN_TPE_STORE_VERTEX(1);
                        TERRAIN_TPE_STORE_VERTEX(2);
                        if (source->normals[1].y < 65535.0) {
                            TERRAIN_TPE_STORE_VERTEX(3);
                            TERRAIN_TPE_EDGES(3, 1, 2);
                            TERRAIN_TPE_CROSS(surface->normals[1], TERRAIN_TPE_FIRST_CROSS_X);
                            f32 length = NuFsqrt(surface->normals[1].x * surface->normals[1].x +
                                                 surface->normals[1].y * surface->normals[1].y +
                                                 surface->normals[1].z * surface->normals[1].z);
                            f32 inverse = 0.0f != length ? 1.0f / length : 0.0f;
                            surface = TERRAIN_TPE_SURFACE;
                            surface->normals[1].x *= inverse;
                            surface->normals[1].y *= inverse;
                            surface->normals[1].z *= inverse;
                        } else {
                            surface->normals[1].y = 65536.0f;
                        }
                        TERRAIN_TPE_EDGES(0, 2, 1);
                        TERRAIN_TPE_CROSS(surface->normals[0], TERRAIN_TPE_FIRST_CROSS_X);
                        f32 length = NuFsqrt(surface->normals[0].x * surface->normals[0].x +
                                             surface->normals[0].y * surface->normals[0].y +
                                             surface->normals[0].z * surface->normals[0].z);
                        f32 inverse = 0.0f == length ? 0.0f : 1.0f / length;
                        surface = TERRAIN_TPE_SURFACE;
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
                    TERRAIN_SHAPE *source = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                    for (i32 remaining = batch->shape_count; remaining > 0; --remaining, ++source) {
                        if (source->material[1] != 0)
                            continue;
                        *cursor++ = source;
                        ++count;
                    }
                    batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(source);
                }
            }
        } else if (TERRAIN_TPE_PLATFORM.flags & 1) {
            while (batch->marker >= 0) {
                TERRAIN_SHAPE *source = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                for (i32 remaining = batch->shape_count; remaining > 0; --remaining, ++source) {
                    if (source->material[1] != 0)
                        continue;
                    TERRAIN_TPE_LOAD_VERTEX(0);
                    TERRAIN_TPE_LOAD_VERTEX(1);
                    TERRAIN_TPE_LOAD_VERTEX(2);
                    NuVec4MtxTransformVU0x3(vertices, vertices, TERRAIN_TPE_GROUP_MATRIX);
                    if (source->normals[1].y < 65535.0) {
                        TERRAIN_TPE_LOAD_VERTEX(3);
                        NuVec4MtxTransformVU0(&vertices[3], &vertices[3], TERRAIN_TPE_GROUP_MATRIX);
                    } else {
                        vertices[3] = vertices[2];
                    }
                    TERRAIN_SHAPE *surface = TERRAIN_TPE_SURFACE;
                    surface->packed_metadata = source->packed_metadata;
                    TERRAIN_TPE_STORE_SCALED(surface->vertices[0], vertices[0]);
                    TERRAIN_TPE_STORE_SCALED(surface->vertices[1], vertices[1]);
                    TERRAIN_TPE_STORE_SCALED(surface->vertices[2], vertices[2]);
                    if (source->normals[1].y < 65535.0) {
                        TERRAIN_TPE_STORE_SCALED(surface->vertices[3], vertices[3]);
                        TERRAIN_TPE_EDGES(3, 1, 2);
                        TERRAIN_TPE_CROSS(surface->normals[1], TERRAIN_TPE_SECOND_CROSS_X);
                        f32 length = NuFsqrt(surface->normals[1].x * surface->normals[1].x +
                                             surface->normals[1].y * surface->normals[1].y +
                                             surface->normals[1].z * surface->normals[1].z);
                        f32 inverse = length != 0.0f ? 1.0f / length : 0.0f;
                        surface = TERRAIN_TPE_SURFACE;
                        surface->normals[1].x *= inverse;
                        surface->normals[1].y *= inverse;
                        surface->normals[1].z *= inverse;
                    } else {
                        surface->normals[1].y = 65536.0f;
                    }
                    TERRAIN_TPE_EDGES(0, 2, 1);
                    TERRAIN_TPE_CROSS(surface->normals[0], TERRAIN_TPE_FIRST_CROSS_X);
                    f32 length = NuFsqrt(surface->normals[0].x * surface->normals[0].x +
                                         surface->normals[0].y * surface->normals[0].y +
                                         surface->normals[0].z * surface->normals[0].z);
                    f32 inverse = 0.0f == length ? 0.0f : 1.0f / length;
                    surface = TERRAIN_TPE_SURFACE;
                    surface->normals[0].x = inverse * surface->normals[0].x;
                    surface->normals[0].y = inverse * surface->normals[0].y;
                    surface->normals[0].z = inverse * surface->normals[0].z;
                    *cursor++ = surface;
                    ++count;
                    ++transformed_count;
                }
                batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(source);
            }
        } else {
            while (batch->marker >= 0) {
                TERRAIN_SHAPE *source = reinterpret_cast<TERRAIN_SHAPE *>(batch + 1);
                for (i32 remaining = batch->shape_count; remaining > 0; --remaining, ++source) {
                    if (source->material[1] != 0)
                        continue;
                    TERRAIN_SHAPE *surface = TERRAIN_TPE_SURFACE;
                    surface->packed_metadata = source->packed_metadata;
                    TERRAIN_TPE_STORE_SCALED(surface->vertices[0], source->vertices[0]);
                    TERRAIN_TPE_STORE_SCALED(surface->vertices[1], source->vertices[1]);
                    TERRAIN_TPE_STORE_SCALED(surface->vertices[2], source->vertices[2]);
                    if (source->normals[1].y < 65535.0) {
                        TERRAIN_TPE_STORE_SCALED(surface->vertices[3], source->vertices[3]);
                        f32 length = NuFsqrt(
                            source->normals[1].y * source->normals[1].y * TERRAIN_TPE_QUERY->object_scale_sq +
                            source->normals[1].x * source->normals[1].x + source->normals[1].z * source->normals[1].z);
                        f32 inverse = 0.0f != length ? 1.0f / length : 0.0f;
                        surface = TERRAIN_TPE_SURFACE;
                        surface->normals[1].x = inverse * source->normals[1].x;
                        surface->normals[1].y = TERRAIN_TPE_QUERY->object_scale * source->normals[1].y * inverse;
                        surface->normals[1].z = inverse * source->normals[1].z;
                    } else {
                        surface->normals[1].y = 65536.0f;
                    }
                    f32 length = NuFsqrt(
                        source->normals[0].y * source->normals[0].y * TERRAIN_TPE_QUERY->object_scale_sq +
                        source->normals[0].x * source->normals[0].x + source->normals[0].z * source->normals[0].z);
                    f32 inverse = length != 0.0f ? 1.0f / length : 0.0f;
                    surface = TERRAIN_TPE_SURFACE;
                    surface->normals[0].x = source->normals[0].x * inverse;
                    surface->normals[0].y = TERRAIN_TPE_QUERY->object_scale * source->normals[0].y * inverse;
                    surface->normals[0].z = inverse * source->normals[0].z;
                    *cursor++ = surface;
                    ++count;
                    ++transformed_count;
                }
                batch = reinterpret_cast<TERRAIN_SHAPE_BATCH *>(source);
            }
        }
#undef TERRAIN_TPE_SECOND_CROSS_X
#undef TERRAIN_TPE_FIRST_CROSS_X
#undef TERRAIN_TPE_CROSS
#undef TERRAIN_TPE_EDGES
#undef TERRAIN_TPE_STORE_SCALED
#undef TERRAIN_TPE_STORE_VERTEX
#undef TERRAIN_TPE_LOAD_VERTEX
#undef TERRAIN_TPE_SURFACE
#undef TERRAIN_TPE_GROUP_MATRIX
#undef TERRAIN_TPE_MATRIX
#undef TERRAIN_TPE_PREVIOUS
#undef TERRAIN_TPE_PLATFORM
        if (count != 0) {
            header[0] = count;
            header[1] = old_query->terrain_group_index;
            header = reinterpret_cast<i16 *>(cursor);
        }
        header[0] = 0;
        header[1] = 0;
        u8 flags[2];
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
        new_query->start_position = new_query->position;
        TERRAIN_TPE_QUERY->position.y = new_query->start_position.y + (f64)0.025f;
        TERRAIN_TPE_QUERY->movement.x = 0.0f;
        TERRAIN_TPE_QUERY->movement.y = -0.05f;
        TERRAIN_TPE_QUERY->movement.z = 0.0f;
        DerotateMovementVector();
        TerrOverRideScan = reinterpret_cast<TERRAIN_SHAPE **>(old_query->scan_list_storage);
        HitTerrain();
        const f32 start_y = new_query->start_position.y;
        TERRAIN_TPE_QUERY->position.y = start_y;
        if (TERRAIN_TPE_QUERY->hit_type != TERRAIN_HIT_TYPE_NONE &&
            TERRAIN_TPE_QUERY->hit_type < TERRAIN_HIT_TYPE_SECOND_NORMAL) {
            new_query->start_position.y =
                TERRAIN_TPE_QUERY->hit_time * TERRAIN_TPE_QUERY->movement.y + start_y + (f64)0.001f + (f64)0.025f;
        } else {
            new_query->start_position.y = 2000000.0f;
        }
        TerI = old_query;
        flags[0] = 0;
        if (new_query->start_position.y < 2000000.0) {
            const f32 delta = new_query->start_position.y - new_query->position.y;
            if (!(delta < -0.008f)) {
                if (0.01f > delta) {
                    new_query->position.y = delta + new_query->position.y;
                    flags[0] = 1;
                } else if (new_query->movement.x * new_query->movement.x +
                               new_query->movement.z * new_query->movement.z >
                           (delta - (f64)0.0025f) * (delta - (f64)0.0025f)) {
                    new_query->position.y = delta + new_query->position.y;
                    flags[0] = 1;
                }
            }
        } else {
            new_query->start_position.y = new_query->position.y;
        }
        if (!TerrainPlatformMoveCheck(&new_query->position, NULL, platform_index, 1, 1))
            flags[0] = 0;
        TERRAIN_TPE_QUERY->position = new_query->position;
        if (flags[0] != 0 && ((TERRAIN_SET_PC *)CurrentTerrain)->groups[TerrainShadowGroupIndexPc].chunk_type == 1) {
            TERRAIN_TPE_QUERY->hit_flags[1] = 1;
            PlatformConnect(reinterpret_cast<char *>(old_query->hit_flags), &old_query->movement, movement_delta,
                            ((TERRAIN_SET_PC *)CurrentTerrain)->groups[TerrainShadowGroupIndexPc].scene_index);
        } else if (CurTrackInfo != NULL) {
            CurTrackInfo->flags &= ~TERRAIN_TRACK_FLAG_CONNECTED;
        }
#undef TERRAIN_TPE_QUERY
        NuScratchRelease();
        NuScratchRelease();
        TerrainScaledShapeCursorPc = static_cast<TERRAIN_SHAPE *>(ScaleTerrainT1);
        TerI = old_query;
        curSphereter = saved_sphere;
        return 0;
    }
    i32 TerrainPlatformMoveCheck(NUVEC *position, NUVEC *normal, i32 platform_index, i32 resolve_impact,
                                 i32 keep_embedded_disabled) {
        const i32 group_index = ((TERRAIN_SET_PC *)CurrentTerrain)->platforms[platform_index].terrain_group_index;
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        i16 *entry = static_cast<i16 *>(query->scan_list);
        while (entry[0] > 0) {
            if (entry[1] == group_index) {
                entry[0] = static_cast<i16>(-abs(entry[0]));
            }
            entry += abs(entry[0]) * 2 + 2;
        }
        if (position->y > query->position.y && position->y < query->position.y + (f64)0.01f) {
            query->position.y = position->y;
        }
        const f32 saved_time = query->hit_time;
        const i16 saved_group = query->terrain_group_index;
        const NUVEC saved_movement = query->movement;
        const f32 saved_unclamped_time = query->unclamped_hit_time;
        const u16 saved_hit_type = query->hit_type;
        TERRAIN_SHAPE *saved_surface = query->surface;
        const i16 saved_scan_group = query->scan_group_index;
        const NUVEC saved_movement_normal = query->movement_normal;
        const NUVEC saved_impact_normal = query->impact_normal;
        query->movement.x = position->x - query->position.x;
        query->movement.y = position->y - query->position.y;
        query->movement.z = position->z - query->position.z;
        DerotateMovementVector();
        HitTerrain();
        query = (TerrainQuery_s *)TerI;
        if (resolve_impact != 0 && query->hit_type != TERRAIN_HIT_TYPE_NONE) {
            TerrainImpactNorm();
            query = (TerrainQuery_s *)TerI;
            TerrainImpactPlatform(query->hit_flags);
            if (normal != NULL) {
                *normal = query->movement_normal;
            }
            *position = query->position;
        }
        query->hit_time = saved_time;
        query->unclamped_hit_time = saved_unclamped_time;
        query->movement = saved_movement;
        query->terrain_group_index = saved_group;
        query->surface = saved_surface;
        query->movement_normal = saved_movement_normal;
        query->scan_group_index = saved_scan_group;
        query->impact_normal = saved_impact_normal;
        if (query->hit_type < TERRAIN_HIT_TYPE_SECOND_NORMAL || keep_embedded_disabled == 0) {
            entry = static_cast<i16 *>(query->scan_list);
            while (entry[0] != 0) {
                if (entry[1] == group_index) {
                    entry[0] = static_cast<i16>(abs(entry[0]));
                }
                entry += abs(entry[0]) * 2 + 2;
            }
        }
        if (query->hit_type != TERRAIN_HIT_TYPE_NONE) {
            query->hit_type = static_cast<u8>(saved_hit_type);
            return 0;
        }
        query->hit_type = static_cast<u8>(saved_hit_type);
        return 1;
    }
    i32 TerrShapeSideStep(NUVEC *, NUVEC *, u8 *) {
        TerrainQuery_s *query = (TerrainQuery_s *)TerI;
        i32 push;
        switch (query->hit_type) {
            case TERRAIN_HIT_TYPE_NONE:
                return 1;
            case TERRAIN_HIT_TYPE_CYLINDER:
            case TERRAIN_HIT_TYPE_VERTEX:
                if (query->impact_normal.y < 0.5f) {
                    push = 0;
                } else {
                    push = 1;
                }
                break;
            default:
                if (query->impact_normal.y >= 0.707f || query->impact_normal.y < -0.8f) {
                    return 1;
                }
                push = 0;
                break;
        }

        TERRAIN_AXIS_FREEDOM_SHAPE *shape = TerrShape;
        NUVEC axis;
        axis.x = NuTrigTable[static_cast<i32>(shape->angle + 16384.0) >> 1 & 0x7fff];
        axis.y = 0.0f;
        axis.z = -NuTrigTable[static_cast<i32>(shape->angle) >> 1 & 0x7fff];

        query->start_position.x = query->position.x;
        query->start_position.y = query->position.y;
        query->start_position.z = query->position.z;
        query->start_movement.x = query->movement.x;
        query->start_movement.y = query->movement.y;
        query->start_movement.z = query->movement.z;
        query->movement.y = 0.0f;

        if (push == 1) {
            query->movement.x = query->movement_normal.x * shape->radius * (f64)0.15f;
            query->movement.z = query->movement_normal.z * shape->radius * (f64)0.15f;
            shape->offset.x += query->movement_normal.x * shape->radius * (f64)0.05f;
            shape->offset.z += query->movement_normal.z * shape->radius * (f64)0.05f;
        } else {
            f32 axis_impact = query->impact_normal.x * axis.x + query->impact_normal.z * axis.z;
            if (axis_impact == 0.0f) {
                axis_impact = (shape->offset.x * axis.x + shape->offset.z * axis.z) / shape->radius;
                if (axis_impact == 0.0f) {
                    return 1;
                }
            }
            if (axis_impact < 0.0f) {
                axis_impact = -NuFsqrt(-axis_impact);
            } else {
                axis_impact = NuFsqrt(axis_impact);
            }
            shape = TerrShape;
            query = (TerrainQuery_s *)TerI;
            query->movement.x = axis_impact * axis.x * shape->radius + shape->offset.x;
            query->movement.z = axis_impact * axis.z * shape->radius + shape->offset.z;
        }

        u8 side_step_hit_flags[2];
        NUVEC impact_result;
        do {
            DerotateMovementVector();
            HitTerrain();
            TerrainImpactNorm();
            TerrainImpact(&impact_result, &impact_result, side_step_hit_flags);

            TerrShapeAdjCnt--;
        } while (TerrShapeAdjCnt > 0 && ((TerrainQuery_s *)TerI)->hit_type != TERRAIN_HIT_TYPE_NONE);
        query = (TerrainQuery_s *)TerI;

        if (query->hit_type == TERRAIN_HIT_TYPE_NONE) {
            query->position.x = query->movement.x + query->position.x;
            query->position.y = query->movement.y + query->position.y;
            query->position.z = query->movement.z + query->position.z;
        }

        TerrShape->offset.x += query->start_position.x - query->position.x;
        TerrShape->offset.z += query->start_position.z - query->position.z;
        query->hit_type = TERRAIN_HIT_TYPE_FACE;
        query->movement.x = query->start_movement.x;
        query->movement.y = query->start_movement.y;
        query->movement.z = query->start_movement.z;
        TerrainSideClamp(&axis, &query->position);
        return 0;
    }
    void TerrainSideClamp(NUVEC *axis, NUVEC *position) {
        f32 forward = axis->x * TerrShape->offset.x + TerrShape->offset.z * axis->z;
        f32 side = axis->z * TerrShape->offset.x - axis->x * TerrShape->offset.z;
        TERRAIN_AXIS_FREEDOM_SHAPE *shape = TerrShape;
        if (forward > shape->radius) {
            forward = shape->radius;
        }
        const f32 negative_radius = -shape->radius;
        if (forward < negative_radius) {
            forward = negative_radius;
        }
        if (side > shape->radius * (f64)0.2f) {
            side = shape->radius * (f64)0.2f;
        }
        if (side < negative_radius * (f64)0.2f) {
            side = negative_radius * (f64)0.2f;
        }

        position->x -= axis->x * forward + side * axis->z - shape->offset.x;
        position->z -= forward * axis->z - axis->x * side - shape->offset.z;

        shape->offset.x = axis->x * forward + side * axis->z;
        shape->offset.z = forward * axis->z - side * axis->x;
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
    TERRAIN_TRACK_SLOT_PC *track;
    for (i32 slot_index = 0; slot_index < 64; ++slot_index) {
        if (((TERRAIN_SET_PC *)CurrentTerrain)->track_slots[slot_index].id == hit_flags) {
            track = &((TERRAIN_SET_PC *)CurrentTerrain)->track_slots[slot_index];
            goto found_track;
        }
    }
    track = NULL;
found_track:
    CurTrackInfo = track;
    TerrOverRideScan = 0;

    TerrainQuery_s *query = static_cast<TerrainQuery_s *>(NuScratchAlloc32(sizeof(TerrainQuery_s)));
    query->object_scale = object_scale;
    TerI = query;
    query->object_scale_sq = object_scale * object_scale;
    f32 inverse = object_scale != 0.0f ? 1.0f / object_scale : 0.0f;
    query->inverse_object_scale = inverse;
    query->inverse_object_scale_sq = inverse * inverse;
    query->collision_radius = collision_radius;
    inverse = collision_radius != 0.0f ? 1.0f / collision_radius : 0.0f;
    query->inverse_collision_radius = inverse;
    query->collision_radius_sq = collision_radius * collision_radius;
    TerrainShadowGroupIndexPc = -1;

    TERRAIN_FLOAT_BITS_PC value;
    value.value = position->x;
    query->position.x = value.value;
    query->start_position.x = value.value;
    value.value = collision_radius * object_scale + position->y;
    query->position.y = value.value;
    query->start_position.y = value.value;
    value.value = position->z;
    query->position.z = value.value;
    query->start_position.z = value.value;
    value.value = movement->x;
    query->movement.x = value.value;
    query->start_movement.x = value.value;
    value.value = movement->y;
    query->movement.y = value.value;
    query->start_movement.y = value.value;
    value.value = movement->z;
    query->movement.z = value.value;
    query->start_movement.z = value.value;
    query->flags &= ~1u;
    query->radius = radius;
    query->separation_epsilon = 0.005f;
    query->object_index = static_cast<i16>(object_index);
    query->hit_flags = hit_flags;
    query->compare_epsilon = 0.000005f;
    query->scan_result = 0;
    ScanTerrain(1, terrain_mask, scan_flags != 0 ? 0x40 : 0);

    if (hit_flags[1] != 0) {
        TERRAIN_FLOAT_BITS_PC absolute_x;
        absolute_x.value = movement->x;
        absolute_x.bits &= 0x7fffffff;
        if (absolute_x.value < radius) {
            TERRAIN_FLOAT_BITS_PC absolute_y;
            absolute_y.value = movement->y;
            absolute_y.bits &= 0x7fffffff;
            if (absolute_y.value < radius) {
                TERRAIN_FLOAT_BITS_PC absolute_z;
                absolute_z.value = movement->z;
                absolute_z.bits &= 0x7fffffff;
                if (absolute_z.value < radius && platinrange == 0) {
                    NuScratchRelease();
                    terrhitflags |= 2;
                    ShadNorm.x = 0.0f;
                    ShadNorm.y = 1.0f;
                    ShadNorm.z = 0.0f;
                    TerrShapeAdjCnt = 0;
                    TerImpactData = 0;
                    return;
                }
            }
        }
    }

    query = static_cast<TerrainQuery_s *>(TerI);
    query->position.y = query->inverse_object_scale * query->position.y;
    query->movement.y = query->inverse_object_scale * query->movement.y;
    hit_flags[0] = 0;
    hit_flags[1] = 0;

    i32 scan_count = PlatformChecks(6, movement);
    do {
        DerotateMovementVector();
        HitTerrain();
        TerrainImpactNorm();
        StorePlatImpact();

        query = static_cast<TerrainQuery_s *>(TerI);
        if (query->hit_type > TERRAIN_HIT_TYPE_SECOND_NORMAL && query->terrain_group_index != -1 &&
            ((TERRAIN_SET_PC *)CurrentTerrain)->groups[query->terrain_group_index].chunk_type ==
                TERRAIN_CHUNK_GROUP_SECONDARY) {
            --scan_count;
            NewTerrStoreAnyInfo();
            NUVEC before = query->position;
            i32 embedded = TerrainPlatformEmbedded(movement);
            query = static_cast<TerrainQuery_s *>(TerI);
            if (embedded == 0 &&
                (before.x != query->position.x || before.y != query->position.y || before.z != query->position.z)) {
                goto resolved;
            }
        }

        --scan_count;
        if (query->hit_type != TERRAIN_HIT_TYPE_NONE && query->terrain_group_index >= 0) {
            ShadNorm = query->impact_normal;

            TERRAIN_SHAPE *surface = query->surface;
            f32 wall_limit;
            if (surface != NULL) {
                wall_limit = surface->normals[0].y * query->movement_normal.y +
                             surface->normals[0].x * query->movement_normal.x +
                             surface->normals[0].z * query->movement_normal.z;
            } else {
                wall_limit = 0.707f;
            }

            TERRAIN_GROUP *group = &((TERRAIN_SET_PC *)CurrentTerrain)->groups[query->terrain_group_index];
            if (group->chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY) {
                wall_limit =
                    (query->hit_type & TERRAIN_HIT_TYPE_CLASS_MASK) > TERRAIN_HIT_TYPE_FACE && (f64)0.95f > wall_limit
                        ? 0.98f
                        : 0.707f;
            } else {
                wall_limit = query->shape_adjusted != 0 ? 1.1f : -1.1f;
            }

            if (wallover != 0.0f) {
                wall_limit = wallover;
            }
            if ((query->hit_type & TERRAIN_HIT_TYPE_CLASS_MASK) != TERRAIN_HIT_TYPE_FACE) {
                wall_limit = 0.707f;
            }
            if (surface != NULL && query->terrain_group_index >= 0 &&
                (surface->normal_flags & TERRAIN_SURFACE_CLASS_MASK) == TERRAIN_SURFACE_CLASS_WALL_OVERRIDE) {
                query->position.x += query->movement_normal.x * (f64)0.001f;
                query->position.z += query->movement_normal.z * (f64)0.001f;
                wall_limit = 1.1f;
            }

            if (query->impact_normal.y >= wall_limit && group->chunk_type == TERRAIN_CHUNK_GROUP_SECONDARY) {
                ((TERRAIN_SET_PC *)CurrentTerrain)->platforms[group->scene_index].flags |=
                    TERRAIN_PLATFORM_FLAG_COLLIDED;
            }
        }

        if (TerrShapeAdjCnt != 0) {
            if (TerrShapeSideStep(position, movement, hit_flags) != 0) {
                TerrainImpact(position, movement, hit_flags);
            }
            query = static_cast<TerrainQuery_s *>(TerI);
        } else {
            TerrainImpact(position, movement, hit_flags);
        }

        TerrLastImpact.position.x = query->position.x - query->movement_normal.x * query->collision_radius;
        TerrLastImpact.position.y =
            (query->position.y - query->movement_normal.y * query->collision_radius) * query->object_scale;
        TerrLastImpact.position.z = query->position.z - query->movement_normal.z * query->collision_radius;
        TerrLastImpact.hit_type = static_cast<f32>(static_cast<i32>(query->hit_type));

    resolved:
        if (query->hit_type != TERRAIN_HIT_TYPE_NONE) {
            TerrImpact = query->surface == NULL ? 2 : 1;
            TerrImpactPos.x = query->position.x - query->movement_normal.x * query->collision_radius;
            TerrImpactPos.y =
                (query->position.y - query->movement_normal.y * query->collision_radius) * query->object_scale;
            TerrImpactPos.z = query->position.z - query->movement_normal.z * query->collision_radius;
            TerrImpactNormal = query->impact_normal;
        }
    } while (query->hit_type != TERRAIN_HIT_TYPE_NONE && scan_count > 0 &&
             query->movement_normal.y * query->movement_normal.y + query->movement_normal.x * query->movement_normal.x +
                     query->movement_normal.z * query->movement_normal.z <=
                 1.5);

    if (query->hit_type != TERRAIN_HIT_TYPE_NONE) {
        position->x = query->position.x;
        position->y = query->position.y * query->object_scale - query->collision_radius * query->object_scale;
        position->z = query->position.z;
    } else if (scan_count > 3 && hit_flags[0] == 0 && hit_flags[1] == 0 && embedded_retry != 0) {
        query->position.x = position->x;
        query->position.y = query->inverse_object_scale * position->y + query->collision_radius + (f64)0.003f;
        query->position.z = position->z;
        query->movement.x = 0.0f;
        query->movement.y = -0.007f;
        query->movement.z = 0.0f;
        DerotateMovementVector();
        HitTerrain();
        query = static_cast<TerrainQuery_s *>(TerI);
        if (query->hit_type != TERRAIN_HIT_TYPE_NONE &&
            (query->terrain_group_index < 0 ||
             (query->surface->normal_flags & TERRAIN_SURFACE_CLASS_MASK) != TERRAIN_SURFACE_CLASS_WALL_OVERRIDE)) {
            TerrainImpactNorm();
            query = static_cast<TerrainQuery_s *>(TerI);
            ShadNorm = query->impact_normal;
            query->start_movement.x = movement->x;
            query->start_movement.y = movement->y;
            query->start_movement.z = movement->z;
            TerrainImpact(position, movement, hit_flags);
            position->x = query->position.x;
            position->y = query->position.y * query->object_scale - query->collision_radius * query->object_scale;
            position->z = query->position.z;
            movement->x = query->start_movement.x;
            movement->y = query->start_movement.y;
            movement->z = query->start_movement.z;
        }
    }

    NuScratchRelease();
    TerrShapeAdjCnt = 0;
    TerImpactData = 0;
}

extern "C" i32 NewRayCastScaleY(NUVEC *position, NUVEC *movement, f32 radius, f32 scale_y, i32 scan_flags) {
    return NewRayCastScaleYMask(position, movement, radius, scale_y, scan_flags, 0);
}

extern "C" void __cdecl FullReflect(NUVEC *normal, NUVEC *movement, NUVEC *result) {
    const f32 reverse_x = -movement->x;
    const f32 reverse_y = -movement->y;
    const f32 reverse_z = -movement->z;
    const f32 reflection = reverse_y * normal->y + reverse_x * normal->x + reverse_z * normal->z;
    result->x = normal->x * reflection * 2.0 + movement->x;
    result->y = normal->y * reflection * 2.0 + movement->y;
    result->z = normal->z * reflection * 2.0 + movement->z;
}
