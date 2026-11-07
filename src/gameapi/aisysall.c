#define _CRT_SECURE_NO_DEPRECATE // NOLINT(readability-identifier-naming): VC8 CRT selector.
#include "saga.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

extern NULISTLNK *__cdecl NuLinkedListGetPrev(NULISTHDR *list, NULISTLNK *node);

extern void __cdecl EdFileWriteUnsignedChar(u8);

extern const char GameAIEveryonePc[];

enum {
    AIPATHINFO_FLAG_ON_PATH_LOCAL = 0x01,
    AIPATHINFO_FLAG_ROUTE_CHECKED_LOCAL = 0x04,
    AIPATH_CONNECTION_FLAG_RESELECT_ROUTE_LOCAL = 0x98000000u
};

extern void(__cdecl *GameAISYSRebuildFromEditorDataFn)(AISYS_PC *, VARIPTR *, VARIPTR *);

extern void(__cdecl *GameAISaveFn)(AIPATHSYS_PC *);

extern void(__cdecl *AIPathNodeMovedFn)(EDAIPATHNODE_s *node);

extern void(__cdecl *AIPathDeletedFn)(EDAIPATH_s *path);

extern void(__cdecl *AiEditorMovePlayersCallbackPc)(NUVEC *);

extern i32 ai_onlyusepackfile;

static const u32 iAISysPathColoursG[5] = {0xffff0000, 0xff00ff00, 0xff00ffff, 0xffff00ff, 0xffffff00};

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, free_creatures) == 0x3719c);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, creatures) == 0x371a4);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, nearest_creature) == 0x371ac);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, selected_creature) == 0x371b0);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, rotation_repeat) == 0x371b4);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, free_areas) == 0x383b8);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, areas) == 0x383c0);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, selected_area) == 0x383c8);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, hovered_area) == 0x383cc);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, locators) == 0x3cbd8);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, locator_sets) == 0x41200);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, current_locator_set) == 0x41208);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, antinode_pool) == 0x4120c);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, free_antinodes) == 0x4380c);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, antinodes) == 0x43814);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, selected_antinode) == 0x4381c);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, nearest_antinode) == 0x43820);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, preview_width) == 0x43824);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, flags) == 0x43828);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, level_directory) == 0x4382c);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, level_file) == 0x43830);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, level_subdirectory) == 0x43834);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, save_scratch) == 0x43838);

STATIC_ASSERT(offsetof(EDAIAREA_s, position) == 0x18);

STATIC_ASSERT(offsetof(EDAIAREA_s, rotation) == 0x30);

STATIC_ASSERT(offsetof(EDAIAREA_s, flags) == 0x33);

STATIC_ASSERT(offsetof(EDAICREATURE_s, angle) == 0x34);

STATIC_ASSERT(offsetof(EDAICREATURE_s, check.path) == 0x3c);

STATIC_ASSERT(offsetof(EDAICREATURE_s, character_type) == 0x58);

STATIC_ASSERT(offsetof(EDAICREATURE_s, script_params) == 0x70);

STATIC_ASSERT(offsetof(EDAICREATURE_s, min_respawns) == 0x8d);

STATIC_ASSERT(offsetof(EDAICREATURE_s, view_distance) == 0x9c);

STATIC_ASSERT(offsetof(EDAICREATURE_s, min_view_height) == 0xa8);

STATIC_ASSERT(offsetof(EDLOCATOR_s, path_check.path) == 0x2c);

extern void __cdecl AiRndrLine3d(NURND_VERTEX3D_PC *, NUMTL *, NUMTX *);
extern const f32 InitOnceHalf;

/* Path-build view of the runtime path system (AIPATHSYS_PC is opaque here). */
typedef struct AI_PATH_BUILD_SYSTEM_PC {
    u8 path_count;
    u8 unknown1;
    u16 special_route_count;
    AIPATH_PC **paths;
    AIPATH_PC *active_path;
    AIPATHSPECIALROUTE_PC *special_routes;
} AI_PATH_BUILD_SYSTEM_PC;
STATIC_ASSERT(sizeof(AI_PATH_BUILD_SYSTEM_PC) == 0x10);
STATIC_ASSERT(offsetof(AI_PATH_BUILD_SYSTEM_PC, special_route_count) == 2);
STATIC_ASSERT(sizeof(((AIPATHROUTE_PC *)0)->character_mask) == 16);
STATIC_ASSERT(offsetof(AIPATHROUTE_PC, character_mask) == 0x18);
STATIC_ASSERT(sizeof(EDAIPATHNODE_s) == 0xb4);
STATIC_ASSERT(offsetof(EDAIPATHNODE_s, connections) == 0x30);
STATIC_ASSERT(offsetof(EDAIPATHNODE_s, index) == 0x90);
STATIC_ASSERT(offsetof(EDAIPATHNODE_s, route_mask) == 0x96);
STATIC_ASSERT(offsetof(EDAIPATHNODE_s, special) == 0x98);
static i32 iterator_count = 0;
static f32 ***distance_tables = NULL;

// Native inlined AISysBufferAlloc blocks retain the failure diagnostic and
// publication after the allocation memset. Size arithmetic remains unsigned32.
#define PATH_BUILD_ALLOCATE(type, destination, input_cursor, input_end, input_size)                                    \
    do {                                                                                                               \
        u32 allocation_size = (input_size);                                                                            \
        VARIPTR *allocation_cursor = (input_cursor);                                                                   \
        VARIPTR *allocation_end = (input_end);                                                                         \
        void *allocation_result = NULL;                                                                                \
        if (allocation_cursor != NULL && allocation_end != NULL) {                                                     \
            if (allocation_cursor->addr + allocation_size < allocation_end->addr) {                                    \
                allocation_result = (void *)((allocation_cursor->addr + 15) & ~15u);                                   \
                allocation_cursor->addr = ((allocation_cursor->addr + 15) & ~15u) + allocation_size;                   \
                memset(allocation_result, 0, allocation_size);                                                         \
            } else {                                                                                                   \
                AIScriptMissingStateDiagnostic();                                                                      \
            }                                                                                                          \
        } else {                                                                                                       \
            AIScriptMissingStateDiagnostic();                                                                          \
        }                                                                                                              \
        destination = (type)allocation_result;                                                                         \
    } while (0)

static __forceinline f32 NuFdiv(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f) {
        return 0.0f;
    }
    return a / b;
}

/* Float fabs by clearing the sign bit; retail inlines it this way. */
static __forceinline f32 AIPathFabs(f32 value) {
    union {
        f32 value;
        u32 bits;
    } storage;
    u32 bits;
    storage.value = value;
    bits = storage.bits;
    bits &= 0x7fffffff;
    storage.bits = bits;
    return storage.value;
}

/* Script message pool (AISYS message system). Records are 0x2c bytes; a
 * process's local messages chain through the word after the record. */
typedef struct AIMESSAGE_PC {
    NULISTLNK links;
    char name[32];
    f32 value;
} AIMESSAGE_PC;

typedef struct AIMESSAGESYS_PC {
    i32 count;
    AIMESSAGE_PC *messages;
    NULISTHDR free_list;
    NULISTHDR active_list;
} AIMESSAGESYS_PC;

typedef struct AILOCALMESSAGE_PC {
    AIMESSAGE_PC message;
    struct AILOCALMESSAGE_PC *next;
} AILOCALMESSAGE_PC;

STATIC_ASSERT(sizeof(AIMESSAGE_PC) == 0x2c);

STATIC_ASSERT(offsetof(AIMESSAGE_PC, value) == 0x28);

STATIC_ASSERT(sizeof(AIMESSAGESYS_PC) == 0x18);

STATIC_ASSERT(offsetof(AIMESSAGESYS_PC, active_list) == 0x10);

STATIC_ASSERT(offsetof(AILOCALMESSAGE_PC, next) == 0x2c);

void AIRetreatFromDestination2(AISYS_PC *, AIPACKET_PC *, APIOBJECT_PC *, i32);

extern const char AIActionFalseName[6];

extern const char AreaConfigKeyword_character[10];

extern const char AIScriptFalse[6];

extern const char GizmoDefaultOutputName[8];

STATIC_ASSERT(offsetof(GAMEOBJECT, force_target) == 0x790);

STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);

STATIC_ASSERT(offsetof(GAMEOBJECT, field_d20) == 0xd20);

STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);

STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, action_data_1) == 0x68);

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, action_data_3) == 0x6c);

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, action_data_4) == 0x70);

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, action_timer) == 0x9c);

extern const char AILocatorIndexedFormat[6];

extern const char AIActionPersonalName[9];

extern const char AIActionIndexedName[8];

extern const char AILocatorPersonalFormat[6];

extern const char AIPathLocatorName[9];

STATIC_ASSERT(offsetof(AIPACKET_PC, antinode_timer) == 0x18c);

STATIC_ASSERT(offsetof(APIOBJECT_PC, ai_collision_priority_pc) == 0x292);

STATIC_ASSERT(offsetof(APIOBJECT_PC, ai_resolved_collision_priority_pc) == 0x294);

STATIC_ASSERT(offsetof(APIOBJECT_PC, collision_link) == 0x298);

typedef struct INTERACTION_ANTINODE_VIEW_PC {
    u8 unknown[0x50];
    u64 excluded_character_types;
} INTERACTION_ANTINODE_VIEW_PC;

STATIC_ASSERT(offsetof(INTERACTION_ANTINODE_VIEW_PC, excluded_character_types) == 0x50);

#define INTERACTION_GEOMETRY(p) ((GAMEOBJECT *)(p))

extern f32 antinode_time;

typedef void(__cdecl *CHECK_ANTINODE_FN_PC)(APIOBJECT_PC *, AIANTINODE_PC *, NUVEC *, f32);

/* The genuine callback definitions and initializer remain in the original aisys owner. */
extern CHECK_ANTINODE_FN_PC checkantinodefns[3];

typedef struct INTERACTION_ANTINODE_SHAPE_VIEW_PC {
    u8 unknown[0x40];
    f32 base_radius;
    f32 base_height;
} INTERACTION_ANTINODE_SHAPE_VIEW_PC;

STATIC_ASSERT(offsetof(INTERACTION_ANTINODE_SHAPE_VIEW_PC, base_radius) == 0x40);

STATIC_ASSERT(offsetof(INTERACTION_ANTINODE_SHAPE_VIEW_PC, base_height) == 0x44);

#define INTERACTION_ANTINODE_SHAPE(a) ((INTERACTION_ANTINODE_SHAPE_VIEW_PC *)(a))

#define INTERACTION_SIN(a) NuTrigTable[((a) >> 1) & 0x7fff]

#define INTERACTION_COS(a) NuTrigTable[(((a) + 0x4000) >> 1) & 0x7fff]

extern f32 antinode_reverse_time;

/* Forward declarations for functions used before their retail position. */

void __cdecl AiSysOnlyUsePakFile(i32 enabled);

static void AISysCheckAntinode_Circle(APIOBJECT_PC *object, AIANTINODE_PC *antinode, NUVEC *difference, f32 radius);

static void AISysCheckAntinode_Ellipse(APIOBJECT_PC *object, AIANTINODE_PC *antinode, NUVEC *difference, f32 radius);
static void AISysCheckAntinode_Rectangle(APIOBJECT_PC *object, AIANTINODE_PC *antinode, NUVEC *difference, f32 radius);

CHECK_ANTINODE_FN_PC checkantinodefns[3] = {AISysCheckAntinode_Circle, AISysCheckAntinode_Ellipse,
                                            AISysCheckAntinode_Rectangle};

typedef struct ai_area_load_pc_s {
    char name[16];
    NUVEC minimum;
    NUVEC maximum;
    u16 flags;
    u8 unknown_2a;
    u8 game_flags;
    u8 unknown_2c[12];
    AISYS_PC *system;
    u8 unknown_3c[4];
} AI_AREA_LOAD_PC;

STATIC_ASSERT(sizeof(AI_AREA_LOAD_PC) == 0x40);

STATIC_ASSERT(offsetof(AI_AREA_LOAD_PC, minimum) == 0x10);

STATIC_ASSERT(offsetof(AI_AREA_LOAD_PC, maximum) == 0x1c);

STATIC_ASSERT(offsetof(AI_AREA_LOAD_PC, flags) == 0x28);

STATIC_ASSERT(offsetof(AI_AREA_LOAD_PC, game_flags) == 0x2b);

STATIC_ASSERT(offsetof(AI_AREA_LOAD_PC, system) == 0x38);

typedef struct ai_creature_load_pc_s {
    char name[16];
    char script_name[16];
    NUVEC position;
    i32 y_rotation;
    AIPATHINFO_PC path_info;
    i32 flags;
    u8 set;
    u8 unknown_4d;
    i16 type;
    u8 count;
    u8 count_across;
    u8 unknown_52[6];
    u32 active_mask;
    f32 x_spacing;
    f32 z_spacing;
    f32 script_params[4];
    AIAREA_PC *activate_area;
    AIAREA_PC *area;
    AILOCATOR_PC *locator;
    AILOCATOR_PC *respawn_locator;
    u8 activation_difficulty;
    i8 min_respawn_count;
    i8 max_respawn_count;
    u8 activation_type;
    f32 min_respawn_time;
    f32 max_respawn_time;
    f32 start_stagger;
    f32 view_distance;
    f32 hear_distance;
    f32 max_view_height;
    f32 min_view_height;
} AI_CREATURE_LOAD_PC;

STATIC_ASSERT(sizeof(AI_CREATURE_LOAD_PC) == 0xa4);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, script_name) == 0x10);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, position) == 0x20);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, y_rotation) == 0x2c);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, path_info) == 0x30);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, flags) == 0x48);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, set) == 0x4c);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, type) == 0x4e);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, count) == 0x50);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, count_across) == 0x51);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, active_mask) == 0x58);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, x_spacing) == 0x5c);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, z_spacing) == 0x60);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, script_params) == 0x64);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, activate_area) == 0x74);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, area) == 0x78);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, locator) == 0x7c);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, respawn_locator) == 0x80);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, activation_difficulty) == 0x84);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, min_respawn_time) == 0x88);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, start_stagger) == 0x90);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, view_distance) == 0x94);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, min_view_height) == 0xa0);

typedef struct ai_antinode_load_pc_s {
    NUVEC position;
    f32 radius;
    f32 min_y;
    f32 min_y_offset;
    f32 max_y;
    f32 max_y_offset;
    NUSPECIALHANDLE special_handle;
    NUVEC special_position;
    i32 flags;
    i32 rotation_offset;
    f32 base_radius;
    f32 base_height;
    u8 enabled;
    u8 game_flags;
    u8 type;
    u8 has_special;
    u8 unknown_4c[12];
} AI_ANTINODE_LOAD_PC;

STATIC_ASSERT(sizeof(AI_ANTINODE_LOAD_PC) == 0x58);

STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, flags) == 0x38);

STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, base_radius) == 0x40);

STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, base_height) == 0x44);

STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, enabled) == 0x48);

typedef struct ai_system_load_pc_s {
    void *storage;
    VARIPTR storage_end;
    VARIPTR storage_cursor;
    i32 storage_size;
    AIPATHSYS_PC *path_sys;
    i32 creature_count;
    AI_CREATURE_LOAD_PC *creatures;
    NULISTHDR scripts;
    i32 locator_count;
    AILOCATOR_PC *locators;
    i32 locator_set_count;
    AILOCATORSET_PC *locator_sets;
    i32 area_count;
    AI_AREA_LOAD_PC *areas;
    AIGROUP_PC groups[16];
    i32 antinode_count;
    AI_ANTINODE_LOAD_PC *antinodes;
    u8 unknown_1384[8];
    APIOBJECT_PC *player_1;
    APIOBJECT_PC *player_2;
    NUGSCN *scene;
} AI_SYSTEM_LOAD_PC;

STATIC_ASSERT(sizeof(AI_SYSTEM_LOAD_PC) == 0x1398);

STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, storage_end) == 4);

STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, storage_cursor) == 8);

STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, storage_size) == 0xc);

STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, path_sys) == 0x10);

STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, creature_count) == 0x14);

STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, creatures) == 0x18);

STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, scripts) == 0x1c);

STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, locators) == 0x28);

STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, locator_sets) == 0x30);

STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, areas) == 0x38);

STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, antinode_count) == 0x137c);

STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, antinodes) == 0x1380);

STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, scene) == 0x1394);

STATIC_ASSERT(offsetof(AI_AREA_LOAD_PC, name) == 0x0);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, name) == 0x0);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, min_respawn_count) == 0x85);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, max_respawn_count) == 0x86);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, activation_type) == 0x87);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, max_respawn_time) == 0x8c);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, hear_distance) == 0x98);

STATIC_ASSERT(offsetof(AI_CREATURE_LOAD_PC, max_view_height) == 0x9c);

STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, position) == 0x0);

STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, radius) == 0xc);

STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, min_y) == 0x10);

STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, min_y_offset) == 0x14);

STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, max_y) == 0x18);

STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, max_y_offset) == 0x1c);

STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, special_handle) == 0x20);

STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, special_position) == 0x2c);

STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, rotation_offset) == 0x3c);

STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, game_flags) == 0x49);

STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, type) == 0x4a);

STATIC_ASSERT(offsetof(AI_ANTINODE_LOAD_PC, has_special) == 0x4b);

STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, storage) == 0x0);

STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, locator_count) == 0x24);

STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, locator_set_count) == 0x2c);

STATIC_ASSERT(offsetof(AI_SYSTEM_LOAD_PC, area_count) == 0x34);

STATIC_ASSERT(offsetof(AIPATHCNX_PC, traversal_flags) == 0x0);

STATIC_ASSERT(offsetof(AIPATHCNX_PC, original_traversal_flags) == 0x8);

STATIC_ASSERT(offsetof(AIPATHCNX_PC, distance) == 0x18);

STATIC_ASSERT(offsetof(AIPATHNODE_PC, name) == 0x0);

STATIC_ASSERT(offsetof(AIPATHNODE_PC, radius) == 0x10);

STATIC_ASSERT(offsetof(AIPATHNODE_PC, radius_squared) == 0x14);

STATIC_ASSERT(offsetof(AIPATHNODE_PC, min_height) == 0x18);

STATIC_ASSERT(offsetof(AIPATHNODE_PC, min_height_offset) == 0x1c);

STATIC_ASSERT(offsetof(AIPATHNODE_PC, max_height) == 0x20);

STATIC_ASSERT(offsetof(AIPATHNODE_PC, max_height_offset) == 0x24);

STATIC_ASSERT(offsetof(AIPATHNODE_PC, connection_count) == 0x28);

STATIC_ASSERT(offsetof(AIPATHNODE_PC, flags) == 0x29);

STATIC_ASSERT(offsetof(AIPATHNODE_PC, has_special) == 0x2a);

STATIC_ASSERT(offsetof(AIPATHNODE_PC, runtime_flags) == 0x2b);

STATIC_ASSERT(offsetof(AIPATHNODE_PC, path_flags) == 0x2c);

STATIC_ASSERT(offsetof(AIPATHNODE_PC, distance_cache_nodes) == 0x2e);

STATIC_ASSERT(offsetof(AIPATHNODE_PC, special_route_index) == 0x30);

STATIC_ASSERT(offsetof(AIPATHNODE_PC, special_position) == 0x4c);

STATIC_ASSERT(offsetof(AIPATHNODE_PC, route_boundary_mask) == 0x5a);

STATIC_ASSERT(offsetof(AIPATHROUTE_PC, name) == 0x0);

STATIC_ASSERT(offsetof(AIPATHROUTE_PC, node_routes) == 0x4);

STATIC_ASSERT(offsetof(AIPATHROUTE_PC, node_directions) == 0x8);

STATIC_ASSERT(offsetof(AIPATHROUTE_PC, exit_nodes) == 0xc);

STATIC_ASSERT(offsetof(AIPATHROUTE_PC, route_count) == 0x10);

STATIC_ASSERT(offsetof(AIPATHROUTE_PC, exit_node_count) == 0x11);

STATIC_ASSERT(offsetof(AIPATHROUTE_PC, route_nodes) == 0x14);

STATIC_ASSERT(offsetof(AIPATHNODELINK_PC, node_index) == 0x0);

STATIC_ASSERT(offsetof(AIPATH_PC, name) == 0x0);

STATIC_ASSERT(offsetof(AIPATH_PC, node_count) == 0x10);

STATIC_ASSERT(offsetof(AIPATH_PC, flags) == 0x11);

STATIC_ASSERT(offsetof(AIPATHSPECIALROUTE_PC, path_count) == 0x0);

extern char *AiLevelPathName;

extern i32 ai_usepackfile;

extern u8 AiLoadActivationDifficultyPc;

extern i8 AiLoadMinimumRespawnCountPc, AiLoadMaximumRespawnCountPc;

extern f32 AiLoadMinimumRespawnTimePc, AiLoadMaximumRespawnTimePc;

static f32 AiLoadStartStaggerPc = 0.0f;

extern i32(__cdecl *GlobalCharacterTypeIDFn)(char *);

extern void(__cdecl *GameAILoadFn)(AISYS_PC *, i32, NUGSCN *, VARIPTR *, VARIPTR *);

void __cdecl AIPathCalcExtents(AIPATH_PC *path);

typedef struct ai_path_system_load_pc_s {
    u8 path_count;
    u8 unknown_01;
    u16 special_route_count;
    AIPATH_PC **paths;
    AIPATH_PC *active_path;
    AIPATHSPECIALROUTE_PC *special_routes;
} AI_PATH_SYSTEM_LOAD_PC;

STATIC_ASSERT(sizeof(AI_PATH_SYSTEM_LOAD_PC) == 0x10);

STATIC_ASSERT(offsetof(AI_PATH_SYSTEM_LOAD_PC, path_count) == 0);

STATIC_ASSERT(offsetof(AI_PATH_SYSTEM_LOAD_PC, special_routes) == 0xc);

STATIC_ASSERT(offsetof(AI_PATH_SYSTEM_LOAD_PC, special_route_count) == 2);

STATIC_ASSERT(offsetof(AI_PATH_SYSTEM_LOAD_PC, paths) == 4);

STATIC_ASSERT(sizeof(AIPATH_PC) == 0xa8);

STATIC_ASSERT(offsetof(AIPATH_PC, connection_count) == 0x12);

STATIC_ASSERT(offsetof(AIPATH_PC, index) == 0x15);

STATIC_ASSERT(offsetof(AIPATH_PC, connections) == 0x80);

STATIC_ASSERT(offsetof(AIPATH_PC, route_matrix) == 0x84);

STATIC_ASSERT(offsetof(AIPATH_PC, bounds_min) == 0x90);

STATIC_ASSERT(offsetof(AIPATH_PC, bounds_max) == 0x9c);

// Native inlined AISysBufferAlloc blocks retain the failure diagnostic and
// publication after the allocation memset. Size arithmetic remains unsigned32.
#define PATH_LOAD_ALLOCATE(type, destination, input_cursor, input_end, input_size)                                     \
    do {                                                                                                               \
        u32 allocation_size = (input_size);                                                                            \
        VARIPTR *allocation_cursor = (input_cursor);                                                                   \
        VARIPTR *allocation_end = (input_end);                                                                         \
        void *allocation_result = NULL;                                                                                \
        if (allocation_cursor != NULL && allocation_end != NULL) {                                                     \
            if (allocation_cursor->addr + allocation_size < allocation_end->addr) {                                    \
                allocation_result = (void *)((allocation_cursor->addr + 15) & ~15u);                                   \
                allocation_cursor->addr = ((allocation_cursor->addr + 15) & ~15u) + allocation_size;                   \
                memset(allocation_result, 0, allocation_size);                                                         \
            } else {                                                                                                   \
                AIScriptMissingStateDiagnostic();                                                                      \
            }                                                                                                          \
        } else {                                                                                                       \
            AIScriptMissingStateDiagnostic();                                                                          \
        }                                                                                                              \
        destination = (type)allocation_result;                                                                         \
    } while (0)

STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1f8) == 0x1f8);

STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);

STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, creature_set) == 0x378);

STATIC_ASSERT(offsetof(GAMEOBJECT, portal_flags) == 0xeec);

STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef4) == 0xef4);

STATIC_ASSERT(offsetof(GAMEOBJECT, field_1010) == 0x1010);

STATIC_ASSERT(sizeof(GAMEOBJECT) == 0x10d8);

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, creature_index) == 0x400);

/* Original AISYS callback family; PC-native seven-argument callback ABI. */
extern i32 __cdecl TagCharacter(GAMEOBJECT *, GAMEOBJECT *, i32);

STATIC_ASSERT(offsetof(AIPACKET_PC, owner) == 0xd0);

STATIC_ASSERT(offsetof(GAMEOBJECT, tag_flags) == 0x7c5);

STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_source) == 0xcd0);

STATIC_ASSERT(offsetof(AISYS_PC, player_1) == 0x138c);

STATIC_ASSERT(offsetof(AISYS_PC, player_2) == 0x1390);

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, locator) == 0xa4);

STATIC_ASSERT(offsetof(AILOCATOR_PC, position) == 0x10);

STATIC_ASSERT(offsetof(APIOBJECT_PC, position) == 0x5c);

typedef struct condition_trigger_process_pc_s {
    u8 unknown_000[0xa0];
    AIAREA_PC *trigger_area;
} CONDITION_TRIGGER_PROCESS_PC;

STATIC_ASSERT(offsetof(CONDITION_TRIGGER_PROCESS_PC, trigger_area) == 0xa0);

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, unknown_a0) == 0xa0);

STATIC_ASSERT(sizeof(AIAREA_PC) == 0x40);

STATIC_ASSERT(offsetof(AIAREA_PC, system) == 0x38);

STATIC_ASSERT(offsetof(AISYS_PC, areas) == 0x38);

STATIC_ASSERT(offsetof(AIPACKET_PC, opponent) == 0xe4);

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_area_mask) == 0x2b0);

STATIC_ASSERT(offsetof(GAMEOBJECT, position) == 0x5c);

extern char *(__cdecl *GlobalCharacterNameFn)(i32);

typedef struct AI_EDITOR_FILE_INFO_PC {
    u32 flags;
    u32 size;
    i8 created_second;
    i8 created_minute;
    i8 created_hour;
    i8 created_day;
    i8 created_month;
    u8 unknown_0d;
    i16 created_year;
    i8 second;
    i8 minute;
    i8 hour;
    i8 day;
    i8 month;
    u8 unknown_15;
    i16 year;
    u8 unknown_18;
    u8 unknown_19[3];
} AI_EDITOR_FILE_INFO_PC;

STATIC_ASSERT(sizeof(AI_EDITOR_FILE_INFO_PC) == 0x1c);

STATIC_ASSERT(offsetof(AI_EDITOR_FILE_INFO_PC, second) == 0x10);

STATIC_ASSERT(offsetof(AI_EDITOR_FILE_INFO_PC, year) == 0x16);

STATIC_ASSERT(offsetof(AI_EDITOR_FILE_INFO_PC, unknown_18) == 0x18);

typedef struct AI_EDITOR_MODE_ENTRY_PC {
    char name[32];
    void(__cdecl *enter)(void);
    eduimenu_s *(__cdecl *process)(FRAME_PAD_PC *);
    void(__cdecl *draw)(i32, i32, f32, f32);
    void(__cdecl *leave)(void);
} AI_EDITOR_MODE_ENTRY_PC;

typedef struct AI_EDITOR_MODE_BANK_PC {
    AI_EDITOR_MODE_ENTRY_PC entries[8];
    i16 count;
    i16 current;
} AI_EDITOR_MODE_BANK_PC;

STATIC_ASSERT(sizeof(AI_EDITOR_MODE_ENTRY_PC) == 48);

STATIC_ASSERT(offsetof(AI_EDITOR_MODE_ENTRY_PC, enter) == 32);

STATIC_ASSERT(offsetof(AI_EDITOR_MODE_ENTRY_PC, process) == 36);

STATIC_ASSERT(offsetof(AI_EDITOR_MODE_ENTRY_PC, draw) == 40);

STATIC_ASSERT(offsetof(AI_EDITOR_MODE_ENTRY_PC, leave) == 44);

STATIC_ASSERT(offsetof(AI_EDITOR_MODE_BANK_PC, count) == 384);

STATIC_ASSERT(offsetof(AI_EDITOR_MODE_BANK_PC, current) == 386);

STATIC_ASSERT(sizeof(AI_EDITOR_MODE_BANK_PC) == 388);

/* AI editor settings block (0x200 bytes, zeroed by the editor initializer). */
typedef struct AIEDITORSETTINGS_PC {
    char path_name[16];     /* restored current path on enter */
    char creature_name[16]; /* restored selected creature on enter */
    char locator_set_name[16];
    char script_name[16];
    i32 locator_rotation;
    i32 character_type;
    f32 height_offset;
    f32 script_params[4];
    u32 script_flags;
    u32 display_options;
    f32 elapsed_time;
    NUMTL *path_material;
    NUMTL *route_material;
    NUMTL *overlay_material;
    void *path_scratch_cursor;
    void *path_scratch_end;
    AI_EDITOR_MODE_BANK_PC modes;
} AIEDITORSETTINGS_PC;

STATIC_ASSERT(offsetof(AIEDITORSETTINGS_PC, locator_rotation) == 0x40);

STATIC_ASSERT(offsetof(AIEDITORSETTINGS_PC, script_params) == 0x4c);

STATIC_ASSERT(offsetof(AIEDITORSETTINGS_PC, display_options) == 0x60);

STATIC_ASSERT(offsetof(AIEDITORSETTINGS_PC, path_scratch_cursor) == 0x74);

STATIC_ASSERT(offsetof(AIEDITORSETTINGS_PC, modes) == 0x7c);

STATIC_ASSERT(sizeof(AIEDITORSETTINGS_PC) == 0x200);

#pragma section(".bss$aieditorsettings", read, write)
__declspec(allocate(".bss$aieditorsettings")) AIEDITORSETTINGS_PC aieditorsettings = {0};

extern const char AiEditorDatedBackupFormatPc[];

AIPATHSYS_PC *__cdecl pathEditorCreateData(VARIPTR *, VARIPTR *, void **, void **);

i32 __cdecl NuFileGetInfo(char *, AI_EDITOR_FILE_INFO_PC *);

i32 __cdecl NuFileCopy(char *, char *);

void __cdecl creatureEditorSaveData(AIPATHSYS_PC *);

static eduiiattr_s AiEditorSaveMessageColoursPc = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};

STATIC_ASSERT(sizeof(AiEditorSaveMessageColoursPc) == 16);

#define REBUILD_ALLOCATE(type, destination, input_cursor, input_end, input_size)                                       \
    do {                                                                                                               \
        u32 allocation_size = (input_size);                                                                            \
        VARIPTR *allocation_cursor = (input_cursor);                                                                   \
        VARIPTR *allocation_end = (input_end);                                                                         \
        void *allocation_result = NULL;                                                                                \
        if (allocation_cursor != NULL && allocation_end != NULL) {                                                     \
            if (allocation_cursor->addr + allocation_size < allocation_end->addr) {                                    \
                allocation_result = (void *)((allocation_cursor->addr + 15) & ~15u);                                   \
                allocation_cursor->addr = ((allocation_cursor->addr + 15) & ~15u) + allocation_size;                   \
                memset(allocation_result, 0, allocation_size);                                                         \
            } else {                                                                                                   \
                AIScriptMissingStateDiagnostic();                                                                      \
            }                                                                                                          \
        } else {                                                                                                       \
            AIScriptMissingStateDiagnostic();                                                                          \
        }                                                                                                              \
        destination = (type)allocation_result;                                                                         \
    } while (0)

/* The Native record has a four-byte mask, a five-word drawing callback,
 * a bounded 64-byte name and a final four-byte options word. */
typedef struct AI_PATH_CONNECTION_TYPE_PC {
    u32 connection_flag;
    void(__cdecl *draw)(NUVEC *, f32, NUVEC *, u32, i32);
    char name[64];
    u32 flags;
} AI_PATH_CONNECTION_TYPE_PC;

STATIC_ASSERT(sizeof(AI_PATH_CONNECTION_TYPE_PC) == 0x4c);

STATIC_ASSERT(offsetof(AI_PATH_CONNECTION_TYPE_PC, draw) == 4);

STATIC_ASSERT(offsetof(AI_PATH_CONNECTION_TYPE_PC, name) == 8);

STATIC_ASSERT(offsetof(AI_PATH_CONNECTION_TYPE_PC, flags) == 0x48);

extern i32 AIPathConnectionTypeCountPc;

extern AI_PATH_CONNECTION_TYPE_PC AIPathConnectionTypesPc[32];

typedef struct AI_EDITOR_SPECIAL_PLATFORM_PC {
    u8 unknown_00[0x4a];
    i16 instance_index;
    u8 unknown_4c[0x20];
} AI_EDITOR_SPECIAL_PLATFORM_PC;

typedef struct AI_EDITOR_SPECIAL_TERRAIN_PC {
    u8 unknown_00[0x68];
    AI_EDITOR_SPECIAL_PLATFORM_PC *platforms;
} AI_EDITOR_SPECIAL_TERRAIN_PC;

STATIC_ASSERT(sizeof(AI_EDITOR_SPECIAL_PLATFORM_PC) == 0x6c);

STATIC_ASSERT(offsetof(AI_EDITOR_SPECIAL_PLATFORM_PC, instance_index) == 0x4a);

STATIC_ASSERT(offsetof(AI_EDITOR_SPECIAL_TERRAIN_PC, platforms) == 0x68);

i16 disable_cylinder_check = 0;

NUCAMERA *__cdecl edmainGetCameraPc(void);

extern i32 near_clip_at_cursor;

f32 AiEditorNearClipOffsetPc = 5.0f;

STATIC_ASSERT(offsetof(edui_slider_s, value) == 0x54);

STATIC_ASSERT(offsetof(eduiitem_s, data) == 0xc);

STATIC_ASSERT(offsetof(eduimenu_s, selected) == 8);

STATIC_ASSERT(offsetof(EDLOCATOR_s, name) == 8);

STATIC_ASSERT(offsetof(edui_textpicker_s, value) == 0x4c);

STATIC_ASSERT(offsetof(edui_textpicker_s, max_length) == 0x15a);

STATIC_ASSERT(offsetof(eduimenu_s, x) == 0x14);

STATIC_ASSERT(offsetof(eduimenu_s, y) == 0x18);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, current_path) == 0x4868);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, paths) == 0x4874);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, current_locator) == 0x3cbe0);

STATIC_ASSERT(offsetof(EDAIPATH_s, name) == 8);

STATIC_ASSERT(offsetof(EDAIPATH_s, nodes) == 0x18);

STATIC_ASSERT(offsetof(EDAIPATH_s, current_node) == 0x20);

STATIC_ASSERT(offsetof(EDAIPATH_s, current_route) == 0x28);

STATIC_ASSERT(offsetof(EDAIPATH_s, flags) == 0x2c);

STATIC_ASSERT(offsetof(EDAIPATH_s, routes) == 0x40);

STATIC_ASSERT(sizeof(EDAIPATHROUTE_s) == 0x20);

STATIC_ASSERT(offsetof(EDAIPATHNODE_s, name) == 8);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, main_menu) == 4);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, free_locator_sets) == 0x411f8);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, locator_set_pool) == 0x3cbe8);

STATIC_ASSERT(offsetof(EDLOCATORSET_s, name) == 8);

STATIC_ASSERT(offsetof(EDLOCATORSET_s, locators) == 0x18);

STATIC_ASSERT(sizeof(EDLOCATORSET_s) == 0x118);

STATIC_ASSERT(offsetof(EDLOCATOR_s, position) == 0x18);

extern const char MenuPcString75f2c0[3];

extern const char MenuPcString75f2c4[4];

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, pending_locator_name) == 0x411e8);

STATIC_ASSERT(offsetof(EDAIPATHROUTE_s, user_mask) == 0x10);

STATIC_ASSERT(offsetof(eduiitem_s, data) == 0x0c);

STATIC_ASSERT(offsetof(eduiitem_s, flags) == 0x11);

extern i32 AiEditorRouteModeIdPc;

STATIC_ASSERT(offsetof(AISYS_PC, scripts) == 0x1c);

STATIC_ASSERT(offsetof(AISCRIPT_PC, name) == 8);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, camera_position) == 0x28);

STATIC_ASSERT(sizeof(EDLOCATOR_s) == 0x48);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, free_locators) == 0x3cbd0);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, nearest_locator) == 0x3cbe4);

void __cdecl cbNearClipAtCursor(eduimenu_s *menu, eduiitem_s *item, u32 buttons);

extern f32 default_path_radius;

extern void __cdecl pathEditor_OnPathCheck(NUVEC *, EDAIPATHCHECK_s *, EDAIPATH_s *, f32);

extern NUVEC edpath_addoffset;

#define PATH_EDITOR_FIND_CONNECTION_PC(result, from, to)                                                               \
    do {                                                                                                               \
        i32 find_index_;                                                                                               \
        (result) = NULL;                                                                                               \
        if ((from) != NULL && (to) != NULL) {                                                                          \
            for (find_index_ = 0; find_index_ < 8; ++find_index_) {                                                    \
                if ((from)->connections[find_index_].node == (to)) {                                                   \
                    (result) = &(from)->connections[find_index_];                                                      \
                    break;                                                                                             \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

extern i32 AiEditorAntinodeModeIdPc;

static const char *AiEditorAntinodeTypeNamesPc[3] = {"Circle", "Ellipse", "Rectangle"};

STATIC_ASSERT(sizeof(AiEditorAntinodeTypeNamesPc) == 12);

STATIC_ASSERT(sizeof(EDANTINODE_s) == 76);

STATIC_ASSERT(offsetof(EDANTINODE_s, radius) == 0x14);

STATIC_ASSERT(offsetof(EDANTINODE_s, special.scene) == 0x20);

STATIC_ASSERT(offsetof(EDANTINODE_s, special.special) == 0x24);

STATIC_ASSERT(offsetof(EDANTINODE_s, special.display_special) == 0x28);

STATIC_ASSERT(offsetof(EDANTINODE_s, base_radius) == 0x40);

STATIC_ASSERT(offsetof(EDANTINODE_s, base_height) == 0x44);

STATIC_ASSERT(offsetof(EDANTINODE_s, game_flags) == 0x48);

STATIC_ASSERT(offsetof(EDANTINODE_s, type) == 0x49);

STATIC_ASSERT(offsetof(EDANTINODE_s, link) == 0);

/* Form2: append these genuine editor-owned mutable scalar definitions in
 * existing src/gameapi/aisys.c before the six-function draft. This TU already
 * owns editor behavior, and both cells have process-static lifetime in Native.
 * Translation-unit placement is an ordinary owner choice; source spelling and
 * initialization are unchanged. Native allocation extents unknown; zero credit.
 * src/data.cpp is retained byte-for-byte from baseline, avoiding its measured
 * constructor/data region displacement. All old aisys complete spans remain a
 * hard measured gate. No section attributes, extra helpers, aliases or bank. */
f32 AiEditorPickHeightTolerancePc = 0.1f;

/* SOURCE DRAFT ONLY. Append to existing src/gameapi/aisys.c, where the
 * existing genuine static set/menu helpers and asserted creature record live.
 * No caller registration/activation, header change, attributes or ABI forcing. */
extern f32 AiEditorPickHeightTolerancePc;

extern void *(__cdecl *GlobalCharacterHGobjFn)(i32);

extern EDAICREATURE_s *__cdecl creatureEditor_GetNearest(i32 use_radius);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, cursor_platform) == 0x3c);

STATIC_ASSERT(offsetof(EDANTINODE_s, position) == 8);

STATIC_ASSERT(offsetof(EDANTINODE_s, lower_height) == 0x18);

STATIC_ASSERT(offsetof(EDANTINODE_s, upper_height) == 0x1c);

STATIC_ASSERT(offsetof(EDANTINODE_s, special_position) == 0x2c);

STATIC_ASSERT(offsetof(EDANTINODE_s, rotation_offset) == 0x38);

STATIC_ASSERT(offsetof(EDANTINODE_s, flags) == 0x3c);

STATIC_ASSERT(offsetof(AIANTINODE_PC, min_y_offset) == 0x14);

STATIC_ASSERT(offsetof(AIANTINODE_PC, max_y_offset) == 0x1c);

STATIC_ASSERT(offsetof(AIANTINODE_PC, rotation) == 0x38);

STATIC_ASSERT(offsetof(AIANTINODE_PC, base_radius) == 0x40);

STATIC_ASSERT(offsetof(AIANTINODE_PC, base_height) == 0x44);

/* Prospective src/gameapi/aisys.c append; existing typed state/shape contracts reused. */
void __cdecl NuRndrBoundingBox(NUVEC *minimum, NUVEC *maximum, NUMTX *matrix, i32 colour);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, ai_system) == 0x64);

/* Actual PC draw-family field views; no Native owning allocation is inferred. */
typedef struct AI_PATH_DRAW_FLAGS_PC {
    NULISTLNK link;
    u8 unknown_08[2];
    u16 flags;
} AI_PATH_DRAW_FLAGS_PC;

STATIC_ASSERT(offsetof(AI_PATH_DRAW_FLAGS_PC, flags) == 0x0a);

STATIC_ASSERT(sizeof(AI_PATH_DRAW_FLAGS_PC) == 0x0c);

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, path_walls) == 0x31b8c);

STATIC_ASSERT(offsetof(EDAIPATH_s, other_node) == 0x24);

STATIC_ASSERT(offsetof(EDAIPATHNODE_s, index) == 0x90);

STATIC_ASSERT(offsetof(EDAIPATHNODE_s, route_mask) == 0x96);

STATIC_ASSERT(offsetof(EDAIPATHNODE_s, shared_node) == 0xb0);

STATIC_ASSERT(offsetof(EDAIPATHNODE_s, connections) == 0x30);

STATIC_ASSERT(sizeof(EDAIPATHCNX_s) == 0x0c);

STATIC_ASSERT(offsetof(EDAIPATHCNX_s, flags) == 4);

STATIC_ASSERT(offsetof(EDAIPATHCNX_s, route_mask) == 8);

STATIC_ASSERT(sizeof(NURND_VERTEX3D_PC) == 0x24);

STATIC_ASSERT(offsetof(NURND_VERTEX3D_PC, colour) == 0x18);

extern i32 AiEditorPathsModeIdPc;

extern i32 AiEditorCreaturesModeIdPc;

extern i32 AiEditorLocatorsModeIdPc;

extern f32 AiEditorUpperHeightOverridePc;

extern f32 AiEditorLowerHeightOverridePc;

extern const u32 AiEditorPathPaletteObservedPc[9];

void AiRndrLine3d(NURND_VERTEX3D_PC *vertices, NUMTL *material, NUMTX *matrix);

STATIC_ASSERT(offsetof(EDAIPATHNODE_s, radius) == 0x24);

STATIC_ASSERT(offsetof(EDAIPATHNODE_s, lower_height) == 0x28);

STATIC_ASSERT(offsetof(EDAIPATHNODE_s, upper_height) == 0x2c);

/* PRIVATE PROPOSAL: append to src/gameapi/aisys.c after existing definitions.
 * No changed registrations; no canonical edits. */
extern void(__cdecl *GlobalCharacterRenderFn)(NUVEC *, i16, i32, i32, void *);

/* Four-byte observed mutable cell; Native owning allocation unknown, zero credit.
 * A distinct storage section preserves all existing owner comparison regions. */

extern u8 default_ngroup, default_nacross;

extern f32 default_xspacing, default_zspacing;

eduiitem_s *creatureEditor_ResetParamsItem = NULL;

typedef struct AI_CREATURE_MODEL_BOUNDS_PC {
    u8 unknown_00[0x1c4];
    f32 min_height;
    f32 height;
    f32 radius;
} AI_CREATURE_MODEL_BOUNDS_PC;

STATIC_ASSERT(offsetof(AI_CREATURE_MODEL_BOUNDS_PC, min_height) == 0x1c4);

STATIC_ASSERT(offsetof(AI_CREATURE_MODEL_BOUNDS_PC, height) == 0x1c8);

STATIC_ASSERT(offsetof(AI_CREATURE_MODEL_BOUNDS_PC, radius) == 0x1cc);

extern i32 aisys_maxnumcreaturesets;

extern void(__cdecl *ClearAICreaturesFn)(void);

i32 geom = 0;

void __cdecl edcamGetPosAng(NUVEC *position, i32 *angle_x, i32 *angle_y);

i32 __cdecl NewRayCastScaleY(NUVEC *position, NUVEC *movement, f32 radius, f32 scale_y, i32 scan_flags);

f32 AiEditorCursorRayScalePc = 1e-17f;

STATIC_ASSERT(sizeof(AIPATHCNX_PC) == 0x24);

STATIC_ASSERT(sizeof(AIPATHNODE_PC) == 0x5c);

STATIC_ASSERT(sizeof(AIPATHINFO_PC) == 0x18);

STATIC_ASSERT(offsetof(AIPATH_PC, nodes) == 0x7c);

STATIC_ASSERT(offsetof(AIPATHNODE_PC, position) == 4);

STATIC_ASSERT(offsetof(AIPATHCNX_PC, node_indices) == 0x10);

STATIC_ASSERT(offsetof(AIPATHCNX_PC, rotation) == 0x12);

STATIC_ASSERT(offsetof(AIPATHCNX_PC, horizontal_distance) == 0x1c);

STATIC_ASSERT(offsetof(EDAIPATHNODE_s, position) == 0x18);

STATIC_ASSERT(offsetof(EDAIPATHCHECK_s, fraction) == 0x10);

STATIC_ASSERT(offsetof(EDAIPATHCHECK_s, width) == 0x14);

STATIC_ASSERT(offsetof(EDAIPATHCHECK_s, angle) == 0x18);

/* Independently observed NUL-terminated Native instruction strings.
 * Separate sections preserve all existing Source data comparison spans. */
#pragma const_seg(".rdata$AiAntinodeRenderNearestPc")
const char AiAntinodeRenderNearestPc[] = "SELECT - Select nearest";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderMovePc")
const char AiAntinodeRenderMovePc[] = "X - Move selected/Adjust size";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderRotateRightPc")
const char AiAntinodeRenderRotateRightPc[] = "LRIGHT - Rotate right";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderRotateLeftPc")
const char AiAntinodeRenderRotateLeftPc[] = "LLEFT - Rotate left";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderDecreaseRadiusPc")
const char AiAntinodeRenderDecreaseRadiusPc[] = "LLEFT - Decrease radius";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderIncreaseRadiusPc")
const char AiAntinodeRenderIncreaseRadiusPc[] = "LRIGHT - Increase radius, %.2f";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderCreatePc")
const char AiAntinodeRenderCreatePc[] = "X - Create antinode";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderSelectPc")
const char AiAntinodeRenderSelectPc[] = "X - Select antinode";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderAdjustLowerPc")
const char AiAntinodeRenderAdjustLowerPc[] = "LDOWN - Adjust lower height";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderAdjustUpperPc")
const char AiAntinodeRenderAdjustUpperPc[] = "LUP - Adjust upper height";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderDecreaseLowerPc")
const char AiAntinodeRenderDecreaseLowerPc[] = "R1 - Decrease lower height";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderIncreaseLowerPc")
const char AiAntinodeRenderIncreaseLowerPc[] = "L1 - Increase lower height";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderDecreaseUpperPc")
const char AiAntinodeRenderDecreaseUpperPc[] = "R1 - Decrease upper height";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderIncreaseUpperPc")
const char AiAntinodeRenderIncreaseUpperPc[] = "L1 - Increase upper height";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderDecreaseZPc")
const char AiAntinodeRenderDecreaseZPc[] = "LDOWN - Decrease Z";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderIncreaseZPc")
const char AiAntinodeRenderIncreaseZPc[] = "LUP - Increase Z, %.2f";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderDecreaseXPc")
const char AiAntinodeRenderDecreaseXPc[] = "LLEFT - Decrease X";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderIncreaseXPc")
const char AiAntinodeRenderIncreaseXPc[] = "LRIGHT - Increase X, %.2f";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderNoPlatformPc")
const char AiAntinodeRenderNoPlatformPc[] = "Not attached to platform";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderPlatformPc")
const char AiAntinodeRenderPlatformPc[] = "Platform=%s";
#pragma const_seg()

#pragma const_seg(".rdata$AiAntinodeRenderTitlePc")
const char AiAntinodeRenderTitlePc[] = "Antinode Editor";
#pragma const_seg()

STATIC_ASSERT(offsetof(AIEDITOR_RENDER_STATE, pad_buttons) == 0xc3938);

STATIC_ASSERT(offsetof(EDANTINODE_s, special) == 0x20);

STATIC_ASSERT(sizeof(NUVEC) == 12);

#pragma section(".data$AiEditorRouteYOffsetPc", read, write)
__declspec(allocate(".data$AiEditorRouteYOffsetPc")) f32 AiEditorRouteYOffsetPc = 0.1f;

extern i32 AiEditorAreasModeIdPc;

void __cdecl routeEditor_Render(i32 x, i32 y, f32 xscale, f32 yscale);

void __cdecl areaEditor_Render(i32 x, i32 y, f32 xscale, f32 yscale);

eduimenu_s *__cdecl locatorEditor_Process(FRAME_PAD_PC *pad);

#define AIEDITOR_REGISTER_PC(id, mode_name, enter_fn, process_fn, draw_fn)                                             \
    do {                                                                                                               \
        AI_EDITOR_MODE_ENTRY_PC *entry_;                                                                               \
        i32 index_ = -1;                                                                                               \
        if (aieditorsettings.modes.count < 8) {                                                                        \
            index_ = aieditorsettings.modes.count++;                                                                   \
            entry_ = &aieditorsettings.modes.entries[index_];                                                          \
            NuStrCpy(entry_->name, mode_name);                                                                         \
            entry_->enter = enter_fn;                                                                                  \
            entry_->process = process_fn;                                                                              \
            entry_->draw = draw_fn;                                                                                    \
            entry_->leave = NULL;                                                                                      \
        } else {                                                                                                       \
            AIScriptMissingStateDiagnostic();                                                                          \
        }                                                                                                              \
        (id) = index_;                                                                                                 \
    } while (0)

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define PI_OVER_4 0.785398f

#define NEG_1_OVER_6 PCPathArcSineCoefficient3

#define NEG_3_OVER_40 PCPathArcSineCoefficient5

#define NEG_5_OVER_112 PCPathArcSineCoefficient7

#define NEG_35_OVER_1152 PCPathArcSineCoefficient9

#define MAX_SHORT_OVER_PI 10430.4f

#define POW2(x) ((x) * (x))

#define POW3(x) ((x) * POW2(x))

#define POW4(x) (POW2(x) * POW2(x))

#define POW5(x) (POW2(x) * POW3(x))

extern const char Text_UnknownText[2];

extern VARIPTR *aiscript_load_buffer;

extern VARIPTR *aiscript_load_end_buffer;

extern AISCRIPT_PC *aiscript_load_script;

extern AISTATE_PC *aiscript_load_state;

extern NULISTHDR *aiscript_load_conditions;

extern NULISTHDR *aiscript_load_actions;

extern NULISTHDR *aiscript_load_refs;

extern NUFPCOMJMP AIScriptStateCommandsPc[5];

extern i32 condition_has_no_goto;

extern const char AIScriptListFilename[11];

extern const char AIScriptListPathFormat[14];

extern const char AIScriptFileExtension[5];

extern const char AIScriptKeywordState[6];

extern const char AIScriptKeywordParam[6];

extern const char AIScriptKeywordConst[6];

extern const char AIScriptKeywordConditionMacro[15];

extern const char AIScriptKeywordActionMacro[12];

extern const char AIScriptKeywordReferenceScript[16];

extern const char AIScriptKeywordDeriveFromScript[17];

extern const char AIScriptKeywordInclude[8];

extern const char AIScriptKeywordEnd[2];

extern const char AIScriptKeywordBase[5];

extern const char AIScriptKeywordScript[7];

extern const char AIScriptKeywordSource[7];

extern const char AIScriptKeywordGlobal[7];

extern const char AIScriptKeywordLevel[6];

void __cdecl xConditions(NUFPAR_PC *parser);

void __cdecl xActions(NUFPAR_PC *parser);

void __cdecl xConditionMacro(NUFPAR_PC *parser);

void __cdecl xActionMacro(NUFPAR_PC *parser);

void __cdecl xRefScript(NUFPAR_PC *parser);

typedef struct aiscript_const_record_pc_s {
    char name[32];
    f32 default_val;
} AISCRIPT_CONST_RECORD_PC;

STATIC_ASSERT(sizeof(AISCRIPT_CONST_RECORD_PC) == 36);

STATIC_ASSERT(offsetof(AISCRIPT_CONST_RECORD_PC, name) == 0);

STATIC_ASSERT(offsetof(AISCRIPT_CONST_RECORD_PC, default_val) == 32);

STATIC_ASSERT(offsetof(AICONDITION_PC, next_state_name) == 0x20);

STATIC_ASSERT(offsetof(NUFPCOMJMP, keyword) == 0);

static void *AIScriptLoadPakPc = 0;

static char *AIScriptLoadPathPc = 0;

static AISCRIPT_CONST_RECORD_PC AIScriptConstantRecordsPc[8] = {0};

static i32 AIScriptConstCursorPc = 0;

static char *AIScriptCopyStringPc(char *text, VARIPTR *buffer, VARIPTR *end);

static void *AIScriptBufferAlloc(VARIPTR *buf, VARIPTR *buf_end, u32 size);

static AISCRIPT_PC *AIScriptLoadPakFilePc(void *pak, char *filename, char *path, VARIPTR *buffer, VARIPTR *end);

/* Set by AiSysOnlyUsePakFile; warns when a package lacks a script file. */
i32 ai_onlyusepackfile = 0;

void __cdecl AIScriptOpenPakFileParse(AISCRIPT_PC **script_ref, void *pak, char *filename, char *path, VARIPTR *buffer,
                                      VARIPTR *end);

void __cdecl xConst(NUFPAR_PC *parser);

void __cdecl xState(NUFPAR_PC *parser);

void __cdecl xParam(NUFPAR_PC *parser);

void __cdecl xDeriveFromScript(NUFPAR_PC *parser);

void __cdecl xInclude(NUFPAR_PC *parser);

static NUFPCOMJMP AIScriptCommandsPc[9] = {
    {(char *)AIScriptKeywordState, xState},
    {(char *)AIScriptKeywordParam, xParam},
    {(char *)AIScriptKeywordConst, xConst},
    {(char *)AIScriptKeywordConditionMacro, xConditionMacro},
    {(char *)AIScriptKeywordActionMacro, xActionMacro},
    {(char *)AIScriptKeywordReferenceScript, xRefScript},
    {(char *)AIScriptKeywordDeriveFromScript, xDeriveFromScript},
    {(char *)AIScriptKeywordInclude, xInclude},
    {NULL, NULL},
};

static const char AIScriptSourcePathFormatPc[] = "%s\\%s";

static const char AIScriptPakPathFormatPc[] = "%s\\ai.pak";

static void *AIScriptBufferAllocOrWarn(VARIPTR *buf, VARIPTR *buf_end, u32 size) {
    void *ret = NULL;

    if (buf != NULL && buf_end != NULL && buf->addr + size < buf_end->addr) {
        ret = (void *)((buf->addr + 15) & ~15u);
        buf->addr = ((buf->addr + 15) & ~15u) + size;
        memset(ret, 0, size);
    } else {
        AIScriptMissingStateDiagnostic();
    }
    return ret;
}

#define AISCRIPT_COPY_NATIVE_STRING(source, destination, buffer, end)                                                  \
    do {                                                                                                               \
        char *text = (source);                                                                                         \
        VARIPTR *text_buffer = (buffer);                                                                               \
        VARIPTR *text_end = (end);                                                                                     \
        u32 length;                                                                                                    \
        (destination) = NULL;                                                                                          \
        if (text != NULL) {                                                                                            \
            length = NuStrLen(text);                                                                                   \
            if (length != 0) {                                                                                         \
                (destination) = (char *)AIScriptBufferAllocOrWarn(text_buffer, text_end, length + 1);                  \
                NuStrCpy((destination), text);                                                                         \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

void AINetReplicateAction(AIACTION_PC *action, AIPACKET_PC *packet);

extern AIACTIONDEF_PC *game_aiactiondefs;

extern AICONDITIONDEF_PC *game_aiconditiondefs;

extern AIACTIONDEF_PC api_aiactiondefs[];

extern AICONDITIONDEF_PC api_aiconditiondefs[];

static i32 ExpressionRequiredNameLookup = 0;

static i32 ExpressionNameLookupFailed = 0;

i32 AiParseExpressionFailed = 0;

enum { EQ = 0, LT = 1, GT = 2, LT_EQ = 3, GT_EQ = 4, NEQ = 5 };

#define AISCRIPT_SET_OWNER_FLAG(packet, params, count, first_time, field, mask, initially_set, set_on_false)           \
    do {                                                                                                               \
        i32 i;                                                                                                         \
        if ((packet) != NULL && (packet)->owner != NULL && (first_time) != 0) {                                        \
            if (initially_set) {                                                                                       \
                (packet)->owner->field |= (mask);                                                                      \
            } else {                                                                                                   \
                (packet)->owner->field &= ~(mask);                                                                     \
            }                                                                                                          \
            for (i = 0; i < (count); i++) {                                                                            \
                if (NuStrICmp((params)[i], AIScriptFalse) == 0) {                                                      \
                    if (set_on_false) {                                                                                \
                        (packet)->owner->field |= (mask);                                                              \
                    } else {                                                                                           \
                        (packet)->owner->field &= ~(mask);                                                             \
                    }                                                                                                  \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

#define AISCRIPT_SET_PACKET_FLAG(packet, params, count, first_time, mask)                                              \
    do {                                                                                                               \
        i32 i;                                                                                                         \
        if ((packet) != NULL && (first_time) != 0) {                                                                   \
            (packet)->flags |= (mask);                                                                                 \
            for (i = 0; i < (count); i++) {                                                                            \
                if (NuStrICmp((params)[i], AIScriptFalse) == 0) {                                                      \
                    (packet)->flags &= ~(mask);                                                                        \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

extern IMAGE_DOS_HEADER __ImageBase;

/* Game-side AI callback registration; retail keeps these in the AI script unit so
   GameAISysInit calls them out of line. */
typedef char *(__cdecl *AI_CHARACTER_NAME_PC)(u8);

typedef char *(__cdecl *AI_GLOBAL_CHARACTER_NAME_PC)(i32);

typedef void *(__cdecl *AI_GLOBAL_CHARACTER_HGOBJ_PC)(i32);

typedef void(__cdecl *AI_GLOBAL_CHARACTER_RENDER_PC)(NUVEC *, i16, i32, i32, void *);

typedef void(__cdecl *AI_CLEAR_CREATURES_PC)(void);

typedef APIOBJECT_PC *(__cdecl *AI_GET_NAMED_OBJECT_PC)(void *, char *);

typedef NUVEC *(__cdecl *AI_GET_CREATURE_ORIGIN_PC)(AISYS_PC *, AIPACKET_PC *);

extern AI_CHARACTER_NAME_PC LevelCharacterNameFn, SpecialRouteCharacterNameFn;

extern AI_GLOBAL_CHARACTER_NAME_PC GlobalCharacterNameFn;

extern AI_GLOBAL_CHARACTER_HGOBJ_PC GlobalCharacterHGobjFn;

extern AI_GLOBAL_CHARACTER_RENDER_PC GlobalCharacterRenderFn;

extern AI_CLEAR_CREATURES_PC ClearAICreaturesFn;

extern AIACTIONDEF_PC api_aiactiondefs[], *game_aiactiondefs;

typedef void(__cdecl *AI_LOAD_PC)(AISYS_PC *, i32, NUGSCN *, VARIPTR *, VARIPTR *);

STATIC_ASSERT(offsetof(AIPACKET_PC, terrain_origin) == 0x14c);

STATIC_ASSERT(offsetof(AIPACKET_PC, creature_index) == 0x138);

struct GIZSPINNER_s *__cdecl GizSpinner_FindBySpecialName(void *world_ptr, char *name);

STATIC_ASSERT(offsetof(AIPACKET_PC, path_info) + offsetof(AIPATHINFO_PC, flags) == 0x166);

STATIC_ASSERT(offsetof(AIPACKET_PC, respawn_count) == 0x1fc);

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, creature_set) == 0xb0);

STATIC_ASSERT(offsetof(AIPACKET_PC, reset_mode) == 0x13e);

STATIC_ASSERT(offsetof(AISCRIPTPROCESS_PC, locator_set) == 0xa8);

STATIC_ASSERT(offsetof(AIAREA_PC, runtime_flags) == 0x2a);

STATIC_ASSERT(offsetof(APIOBJECT_PC, ai) == 4);

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, opponent) == 0x3ac);

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, opponent_metric) == 0x3b0);

STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, flags) == 0x4b0);

STATIC_ASSERT(offsetof(AIPACKET_PC, flags) == 0x1e8);

STATIC_ASSERT(offsetof(GAMEOBJECT, objptr) == 0);

STATIC_ASSERT(offsetof(GAMEOBJECT, field_f10) == 0xf10);

typedef struct condition_trigger_process_reader_pc_s {
    u8 unknown_00[0xa0];
    AIAREA_PC *trigger_area;
} CONDITION_TRIGGER_PROCESS_READER_PC;

STATIC_ASSERT(offsetof(CONDITION_TRIGGER_PROCESS_READER_PC, trigger_area) == 0xa0);

typedef struct condition_trigger_player_reader_pc_s {
    u8 unknown_00[0x2b0];
    u64 membership;
} CONDITION_TRIGGER_PLAYER_READER_PC;

STATIC_ASSERT(offsetof(CONDITION_TRIGGER_PLAYER_READER_PC, membership) == 0x2b0);

STATIC_ASSERT(offsetof(AIPACKET_PC, group) == 0x144);

STATIC_ASSERT(offsetof(GAMEOBJECT, current_hp) == 0x107f);

STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant_flags) == 0x7bc);

STATIC_ASSERT(offsetof(AIPACKET_PC, nearest_opponent) == 0xd4);

STATIC_ASSERT(offsetof(AIPACKET_PC, nearest_opponent_metric) == 0xd8);

STATIC_ASSERT(offsetof(GAMEOBJECT, last_attacker) == 0x10a8);

STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);

STATIC_ASSERT(offsetof(GAMEOBJECT, supporting_platform_id) == 0x27e);

STATIC_ASSERT(offsetof(GAMEOBJECT, field_281) == 0x281);

STATIC_ASSERT(offsetof(GAMEOBJECT, field_282) == 0x282);

STATIC_ASSERT(offsetof(GAMEOBJECT, position) + offsetof(NUVEC, y) == 0x60);

STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_target) == 0xea4);

STATIC_ASSERT(offsetof(GAMEOBJECT, rider) == 0xcd0);

STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);

STATIC_ASSERT(offsetof(APIOBJECT_PC, owner) == 0);

STATIC_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);

STATIC_ASSERT(offsetof(GAMEOBJECT, grapple) == 0x798);

STATIC_ASSERT(offsetof(GAMEOBJECT, field_0x276) == 0x27a);

SOCK_PC *__cdecl FindSock(SOCKSYS_PC *system, char *name);

extern const char AIOverrideAllKeywordPc[9];

static __declspec(noinline) f32 AIPathSafeDivide(f32 numerator, f32 denominator) {
    if (numerator == 0.0f || denominator == 0.0f)
        return 0.0f;
    return numerator / denominator;
}

i16 __cdecl PCPathArcSine(f32 sin) {
    f32 abs;
    f32 sqrt;
    f32 unknown_a;
    f32 unknown_b;
    f32 unknown_c;
    f32 unknown_d;

    union {
        f32 value;
        u32 bits;
    } magnitude, absolute;
    magnitude.value = sin;
    absolute.bits = magnitude.bits & 0x7fffffff;
    abs = absolute.value;
    sqrt = NuFsqrt(1.0f - sin * sin);

    unknown_a = MIN(sqrt, abs);

    unknown_b = MAX(MIN((abs - 0.70710677f) * 3.40282e+38f, 1.0f), -1.0f);

    unknown_c = MIN(sin * 3.40282e+38f, 1.0f);
    unknown_c = MAX(unknown_c, -1.0f);

    unknown_d = unknown_b * unknown_c + unknown_c;

    return (NEG_3_OVER_40 * POW2(unknown_b * unknown_c * unknown_a) * POW3(unknown_b * unknown_c * unknown_a) +
            (unknown_d * PI_OVER_4 - (unknown_b * unknown_c * unknown_a) +
             NEG_1_OVER_6 * (unknown_b * unknown_c * unknown_a) * POW2(unknown_b * unknown_c * unknown_a)) +
            NEG_5_OVER_112 * POW3(unknown_b * unknown_c * unknown_a) * POW4(unknown_b * unknown_c * unknown_a) +
            NEG_35_OVER_1152 * POW4(unknown_b * unknown_c * unknown_a) * POW5(unknown_b * unknown_c * unknown_a)) *
           MAX_SHORT_OVER_PI;
}

void AiRndrLine3d(NURND_VERTEX3D_PC *vertices, NUMTL *unused1, NUMTX *matrix) {
    NUVEC first = vertices[0].position;
    NUVEC second = vertices[1].position;
    NUMTX matrix_copy;
    if (matrix != NULL) {
        if (((size_t)(matrix) & 15) != 0) {
            matrix_copy = *matrix;
            matrix = &matrix_copy;
        }
        NuVecMtxTransform(&first, &first, matrix);
        NuVecMtxTransform(&second, &second, matrix);
    }
    if (AiRndrCullDistance > 0.0f) {
        f32 dx;
        f32 dy;
        f32 dz;
        f32 limit;
        f32 dx2;
        f32 dy2;
        f32 dz2;
        __declspec(align(16)) NUVEC position;
        if (aieditor && (aieditor->flags & 2))
            position = aieditor->camera_position;
        else
            position = *(NUVEC *)&global_camera.mtx.m30;
        dx = position.x - first.x;
        dy = position.y - first.y;
        dz = position.z - first.z;
        limit = AiRndrCullDistance * AiRndrCullDistance;
        dx2 = position.x - second.x;
        dy2 = position.y - second.y;
        dz2 = position.z - second.z;
        if (dx * dx + dy * dy + dz * dz > limit && dx2 * dx2 + dy2 * dy2 + dz2 * dz2 > limit)
            return;
    }
    NuRndrLine3dDbg(first.x, first.y, first.z, second.x, second.y, second.z, vertices[0].colour);
}

void AiRndrLine3dDbg(f32 x0, f32 y0, f32 z0, f32 x1, f32 y1, f32 z1, u32 colour) {
    if (AiRndrCullDistance > 0.0f) {
        f32 dx;
        f32 dy;
        f32 dz;
        f32 limit;
        __declspec(align(16)) NUVEC position;
        if (aieditor && (aieditor->flags & 2))
            position = aieditor->camera_position;
        else
            position = *(NUVEC *)&global_camera.mtx.m30;
        dx = position.x - x0;
        dy = position.y - y0;
        dz = position.z - z0;
        limit = AiRndrCullDistance * AiRndrCullDistance;
        if (!(dx * dx + dy * dy + dz * dz < limit)) {
            dx = position.x - x1;
            dy = position.y - y1;
            dz = position.z - z1;
            if (!(dx * dx + dy * dy + dz * dz < limit))
                return;
        }
    }
    NuRndrLine3dDbg(x0, y0, z0, x1, y1, z1, colour);
}

void __cdecl SetScriptErrorLevel(i32 level) {
    if ((u32)level <= 2)
        AIActionDiagnosticMode = level;
}

static void *AIScriptBufferAlloc(VARIPTR *buf, VARIPTR *buf_end, u32 size) {
    void *ret;

    ret = NULL;
    if (buf != NULL && buf_end != NULL) {
        if (buf_end->addr > buf->addr + size) {
            ret = (void *)((buf->addr + 15) & ~15u);
            buf->addr = (u32)ret + size;
            memset(ret, 0, size);
        }
    }

    return ret;
}

static AICONDITIONDEF_PC *AIConditionFind(char *name) {
    AICONDITIONDEF_PC *definition;

    if (game_aiconditiondefs != NULL) {
        for (definition = game_aiconditiondefs; definition->name != NULL; definition++) {
            if (NuStrICmp(name, definition->name) == 0) {
                return definition;
            }
        }
    }

    for (definition = api_aiconditiondefs; definition->name != NULL; definition++) {
        if (NuStrICmp(name, definition->name) == 0) {
            return definition;
        }
    }

    return NULL;
}

static AIACTIONDEF_PC *AIActionFind(char *name) {
    AIACTIONDEF_PC *definition;

    if (game_aiactiondefs != NULL) {
        for (definition = game_aiactiondefs; definition->name != NULL; definition++) {
            if (NuStrICmp(name, definition->name) == 0) {
                return definition;
            }
        }
    }

    for (definition = api_aiactiondefs; definition->name != NULL; definition++) {
        if (NuStrICmp(name, definition->name) == 0) {
            return definition;
        }
    }

    return NULL;
}

void __cdecl RegisterAIScriptActions(AIACTIONDEF_PC *actions) {
    AIACTIONDEF_PC *def;
    i32 index = 0;
    game_aiactiondefs = actions;
    for (def = actions; def->name != NULL; def++) {
        def->is_game_action = 1;
        def->index = (i16)index;
        index++;
    }
    index = 0;
    for (def = api_aiactiondefs; def->name != NULL; def++) {
        def->is_game_action = 0;
        def->index = (i16)index;
        index++;
    }
}

void __cdecl RegisterAIScriptConditions(AICONDITIONDEF_PC *conditions) {
    game_aiconditiondefs = conditions;
}

AISTATE_PC *__cdecl AIStateFind(const char *name, AISCRIPT_PC *script) {
    AISTATE_PC *state;

    state = (AISTATE_PC *)NuLinkedListGetHead(&script->states);
    while (state != NULL) {
        if (NuStrICmp(name, state->name) == 0) {
            return state;
        }
        state = (AISTATE_PC *)NuLinkedListGetNext(&script->states, &state->link);
    }

    return NULL;
}

AISCRIPT_PC *__cdecl AIScriptFind(AISYS_PC *sys, const char *name, i32 can_use_default, i32 check_level_scripts,
                                  i32 check_global_scripts) {
    AISCRIPT_PC *script;

    if (name != NULL) {
        if (check_level_scripts != 0 && sys != NULL) {
            script = (AISCRIPT_PC *)NuLinkedListGetHead(&sys->scripts);
            while (script != NULL) {
                if (NuStrICmp(name, script->name) == 0) {
                    return script;
                }
                script = (AISCRIPT_PC *)NuLinkedListGetNext(&sys->scripts, &script->link);
            }
        }

        if (check_global_scripts != 0) {
            script = (AISCRIPT_PC *)NuLinkedListGetHead(&global_aiscripts);
            while (script != NULL) {
                if (NuStrICmp(name, script->name) == 0) {
                    return script;
                }
                script = (AISCRIPT_PC *)NuLinkedListGetNext(&global_aiscripts, &script->link);
            }
        }
    }

    if (can_use_default != 0) {
        script = (AISCRIPT_PC *)NuLinkedListGetHead(&global_aiscripts);
        while (script != NULL) {
            if (NuStrICmp(GizmoDefaultOutputName, script->name) == 0) {
                return script;
            }
            script = (AISCRIPT_PC *)NuLinkedListGetNext(&global_aiscripts, &script->link);
        }
    }

    return NULL;
}

static char *AIScriptCopyStringPc(char *text, VARIPTR *buffer, VARIPTR *end) {
    char *copy = NULL;
    u32 length;
    if (text != NULL) {
        length = NuStrLen(text);
        if (length != 0) {
            copy = (char *)AIScriptBufferAlloc(buffer, end, length + 1);
            /* Native copies even after failed allocation; the NULL destination hazard is retained. */
            NuStrCpy(copy, text);
        }
    }
    return copy;
}

char *__cdecl AIScriptNameFromIx(AISYS_PC *system, i32 index) {
    i32 current = 0;
    AISCRIPT_PC *script;
    if (system != NULL) {
        script = (AISCRIPT_PC *)NuLinkedListGetHead(&system->scripts);
        while (script != NULL) {
            if (current == index)
                return script->name;
            script = (AISCRIPT_PC *)NuLinkedListGetNext(&system->scripts, &script->link);
            ++current;
        }
    }
    script = (AISCRIPT_PC *)NuLinkedListGetHead(&global_aiscripts);
    while (script != NULL) {
        AISCRIPT_PC *local = NULL;
        if (system != NULL) {
            local = (AISCRIPT_PC *)NuLinkedListGetHead(&system->scripts);
            while (local != NULL) {
                if (NuStrICmp(script->name, local->name) == 0)
                    break;
                local = (AISCRIPT_PC *)NuLinkedListGetNext(&system->scripts, &local->link);
            }
        }
        if (local == NULL) {
            if (current == index)
                return script->name;
            ++current;
        }
        script = (AISCRIPT_PC *)NuLinkedListGetNext(&global_aiscripts, &script->link);
    }
    return NULL;
}

static i32 AiParseExpressionNameLoopup(char *name, f32 *float_out, i32 *int_out) {
    f32 value;

    ExpressionRequiredNameLookup = 1;
    value = 0.0f;
    ExpressionNameLookupFailed = 1;

    if (GameParamToFloatFn != NULL) {
        if ((*GameParamToFloatFn)(NULL, NULL, name, &value) != 0) {
            ExpressionNameLookupFailed = 0;
        }
    }

    if (float_out != NULL) {
        *float_out = value;
    }

    if (int_out != NULL) {
        *int_out = value;
    }

    return 0;
}

f32 AiParseExpression(char *expr) {
    f32 value;

    ExpressionRequiredNameLookup = 0;
    ExpressionNameLookupFailed = 0;
    AiParseExpressionFailed = 0;

    value = NuRDPFVar(expr, AiParseExpressionNameLoopup);

    if (ExpressionNameLookupFailed != 0) {
        AiParseExpressionFailed = 1;
    }

    return value;
}

static void ConditionsParseFinalise(AICONDITION_PC *cond, NUFPAR_PC *parser) {
    NuFParGetWord(parser);
    if (NuStrICmp(parser->word_buffer_ptr, "goto") == 0) {
        cond->flags &= (u16)~2;
        NuFParGetWord(parser);
        cond->next_state_name =
            AIScriptCopyStringPc(parser->word_buffer_ptr, aiscript_load_buffer, aiscript_load_end_buffer);
        NuFParGetWord(parser);
        if (NuStrICmp(parser->word_buffer_ptr, "KeepBlockedMessages") == 0) {
            cond->flags |= 2;
        }
    } else if (NuStrICmp(parser->word_buffer_ptr, "and") == 0) {
        cond->flags |= 1;
    } else if (condition_has_no_goto == 0) {
        if (AIActionDiagnosticMode == 1) {
            AIScriptMissingStateDiagnosticAlternate(aiscript_load_script->name,
                                                    aiscript_load_state != NULL ? aiscript_load_state->name
                                                                                : (char *)Text_UnknownText,
                                                    parser->line_buffer_ptr);
        } else {
            AIScriptMissingStateDiagnostic(aiscript_load_script->name,
                                           aiscript_load_state != NULL ? aiscript_load_state->name
                                                                       : (char *)Text_UnknownText,
                                           parser->line_buffer_ptr);
        }
    }
}

void __cdecl xEndState(NUFPAR_PC *parser) {
    NuFParPopCom(parser);
}

static NUFPAR_PC *AIScriptOpenParserPc(void *pak, char *filename, char *path) {
    char script_name[64];
    char filepath[128];
    char *extension;
    i32 item;
    i32 size;
    void *data;
    NUFILE file;
    NuStrCpy(script_name, filename);
    extension = NuStrIStr(script_name, (char *)AIScriptFileExtension);
    if (extension == NULL) {
        return NULL;
    }
    *extension = '\0';
    sprintf(filepath, AIScriptSourcePathFormatPc, path, filename);
    if (pak != NULL) {
        item = NuFilePakGetItem(pak, filename);
        if (item != 0) {
            NuFilePakGetItemInfo(pak, item, &data, &size);
            file = NuMemFileOpen(data, size, 0);
            if (file != 0) {
                return NuFParOpen(file);
            }
            AIScriptMissingConditionDiagnostic(filename);
        } else if (AIActionDiagnosticMode == 1) {
            AIScriptMissingConditionDiagnostic(filename);
        } else {
            AIScriptMissingStateDiagnostic(filename);
        }
        if (ai_onlyusepackfile != 0) {
            if (AIActionDiagnosticMode == 1) {
                AIScriptMissingStateDiagnosticAlternate(filename);
            } else {
                AIScriptMissingStateDiagnostic(filename);
            }
        }
    }
    return NuFParCreate(filepath);
}

void __cdecl xState(NUFPAR_PC *parser) {
    VARIPTR *buffer;
    VARIPTR *end_buffer;
    AISTATE_PC *state;
    u32 state_address;
    char *name;

    if (aiscript_load_script == NULL || NuFParGetWord(parser) == 0) {
        return;
    }

    end_buffer = aiscript_load_end_buffer;
    buffer = aiscript_load_buffer;
    state = NULL;
    if (buffer != NULL && end_buffer != NULL && buffer->addr + sizeof(AISTATE_PC) < end_buffer->addr) {
        state_address = (buffer->addr + 15u) & ~15u;
        state = (AISTATE_PC *)state_address;
        buffer->addr = state_address + sizeof(AISTATE_PC);
        memset(state, 0, sizeof(AISTATE_PC));
    } else {
        AIScriptMissingStateDiagnostic();
    }
    aiscript_load_state = state;
    aiscript_load_refs = (NULISTHDR *)((u32)state + offsetof(AISTATE_PC, reference_scripts));
    if (state == NULL) {
        return;
    }
    memset(state, 0, sizeof(AISTATE_PC));
    name = AIScriptCopyStringPc(parser->word_buffer_ptr, buffer, end_buffer);
    aiscript_load_state->name = name;
    if (NuStrICmp(name, AIScriptKeywordBase) == 0) {
        NuLinkedListInsert((NULISTHDR *)((u32)aiscript_load_script + offsetof(AISCRIPT_PC, states)),
                           (NULISTLNK *)aiscript_load_state);
    } else {
        NuLinkedListAppend((NULISTHDR *)((u32)aiscript_load_script + offsetof(AISCRIPT_PC, states)),
                           (NULISTLNK *)aiscript_load_state);
    }

    NuFParPushCom(parser, AIScriptStateCommandsPc);
    while (NuFParGetLine(parser) != 0) {
        NuFParGetWord(parser);
        state_address = (u32)aiscript_load_state;
        aiscript_load_conditions = (NULISTHDR *)(state_address + offsetof(AISTATE_PC, conditions));
        aiscript_load_actions = (NULISTHDR *)(state_address + offsetof(AISTATE_PC, actions));
        NuFParInterpretWord(parser);
    }
    NuFParPopCom(parser);
    aiscript_load_refs = NULL;
    aiscript_load_state = NULL;
    aiscript_load_conditions = NULL;
    aiscript_load_actions = NULL;
}

void __cdecl xParam(NUFPAR_PC *parser) {
    i32 index;

    if (aiscript_load_script == NULL) {
        return;
    }
    index = NuFParGetInt(parser);
    if (index < 0 || index >= 4 || NuFParGetWord(parser) == 0) {
        return;
    }
    aiscript_load_script->params[index].name =
        AIScriptCopyStringPc(parser->word_buffer_ptr, aiscript_load_buffer, aiscript_load_end_buffer);
    aiscript_load_script->params[index].default_value = NuFParGetFloat(parser);
}

void __cdecl xConst(NUFPAR_PC *parser) {
    if (NuStrLen(parser->word_buffer_ptr) >= 31) {
        AIScriptMissingStateDiagnostic(parser->word_buffer_ptr);
    }
    NuFParGetWord(parser);
    NuStrNCpy(AIScriptConstantRecordsPc[AIScriptConstCursorPc].name, parser->word_buffer_ptr, 32);
    AIScriptConstantRecordsPc[AIScriptConstCursorPc].default_val = NuFParGetFloat(parser);
    AIScriptConstCursorPc++;
}

static void AIScriptParseCommandsPc(NUFPAR_PC *parser, VARIPTR *buffer, VARIPTR *end, AISCRIPT_PC *script) {
    if (script == NULL) {
        return;
    }
    aiscript_load_buffer = buffer;
    aiscript_load_end_buffer = end;
    aiscript_load_script = script;
    AIScriptConstCursorPc = 0;
    NuFParPushCom(parser, AIScriptCommandsPc);
    while (NuFParGetLine(parser) != 0) {
        NuFParGetWord(parser);
        aiscript_load_refs = &script->reference_scripts;
        NuFParInterpretWord(parser);
    }
    aiscript_load_buffer = NULL;
    aiscript_load_end_buffer = NULL;
    aiscript_load_script = NULL;
    aiscript_load_refs = NULL;
}

void __cdecl xDeriveFromScript(NUFPAR_PC *parser) {
    i32 done;

    if (aiscript_load_script == NULL || aiscript_load_script->derived_from != NULL) {
        return;
    }
    aiscript_load_script->flags_3c &= ~4u;
    done = 0;
    while (!done && NuFParGetLine(parser) != 0) {
        while (!done && NuFParGetWord(parser) != 0) {
            char *cursor;
            if (NuStrICmp(parser->word_buffer, AIScriptKeywordEnd) == 0) {
                done = 1;
            } else if ((cursor = NuStrIStr(parser->word_buffer, (char *)AIScriptKeywordScript)) != NULL) {
                char *copy;
                AISCRIPT_COPY_NATIVE_STRING(cursor + sizeof("Script"), copy, aiscript_load_buffer,
                                            aiscript_load_end_buffer);
                aiscript_load_script->derived_from = copy;
            } else if (NuStrIStr(parser->word_buffer, (char *)AIScriptKeywordSource) != NULL) {
                if (NuStrIStr(parser->word_buffer, (char *)AIScriptKeywordGlobal) != NULL) {
                    aiscript_load_script->flags_3c &= ~4u;
                } else if (NuStrIStr(parser->word_buffer, (char *)AIScriptKeywordLevel) != NULL) {
                    aiscript_load_script->flags_3c |= 4u;
                }
            }
        }
    }
    if (aiscript_load_script->derived_from != NULL) {
        aiscript_load_script->flags_3c |= 2u;
    }
}

void __cdecl AIScriptCopyConditions(NULISTHDR *src, NULISTHDR *dst, VARIPTR *buf, VARIPTR *buf_end) {
    AICONDITION_PC *src_cond;

    src_cond = (AICONDITION_PC *)NuLinkedListGetHead(src);
    while (src_cond != NULL) {
        AICONDITION_PC *dst_cond;

        dst_cond = (AICONDITION_PC *)AIScriptBufferAllocOrWarn(buf, buf_end, sizeof(AICONDITION_PC));
        if (dst_cond != NULL) {
            memset(dst_cond, 0, sizeof(AICONDITION_PC));
            PCByteCopy_candidate(dst_cond, src_cond, sizeof(AICONDITION_PC));

            dst_cond->link.prev = NULL;
            dst_cond->link.next = NULL;

            AISCRIPT_COPY_NATIVE_STRING(src_cond->argument, dst_cond->argument, buf, buf_end);
            AISCRIPT_COPY_NATIVE_STRING(src_cond->next_state_name, dst_cond->next_state_name, buf, buf_end);

            NuLinkedListAppend(dst, &dst_cond->link);
        }

        src_cond = (AICONDITION_PC *)NuLinkedListGetNext(src, &src_cond->link);
    }
}

void __cdecl AIScriptCopyActions(NULISTHDR *src, NULISTHDR *dst, VARIPTR *buf, VARIPTR *buf_end) {
    AIACTION_PC *src_action;

    src_action = (AIACTION_PC *)NuLinkedListGetHead(src);
    while (src_action != NULL) {
        AIACTION_PC *dst_action;
        i32 i;

        dst_action = (AIACTION_PC *)AIScriptBufferAllocOrWarn(buf, buf_end, sizeof(AIACTION_PC));
        if (dst_action != NULL) {
            dst_action->params =
                (char **)AIScriptBufferAllocOrWarn(buf, buf_end, src_action->param_count * sizeof(char *));
            dst_action->param_count = src_action->param_count;

            for (i = 0; i < dst_action->param_count; i++) {
                char *text = src_action->params[i];
                char *copy = NULL;
                u32 length;

                if (text != NULL) {
                    length = NuStrLen(text);
                    if (length != 0) {
                        copy = (char *)AIScriptBufferAllocOrWarn(buf, buf_end, length + 1);
                        NuStrCpy(copy, text);
                    }
                }
                dst_action->params[i] = copy;
            }

            dst_action->definition = src_action->definition;

            NuLinkedListAppend(dst, &dst_action->link);
        }

        src_action = (AIACTION_PC *)NuLinkedListGetNext(src, &src_action->link);
    }
}

static AISTATE_PC *AIScriptCopyStatePc(AISTATE_PC *source_state, VARIPTR *buffer, VARIPTR *end_buffer) {
    AISTATE_PC *copy;
    AIREFSCRIPT_PC *source_reference;

    copy = (AISTATE_PC *)AIScriptBufferAllocOrWarn(buffer, end_buffer, sizeof(AISTATE_PC));
    if (copy == NULL)
        return NULL;
    copy->name = AIScriptCopyStringPc(source_state->name, buffer, end_buffer);
    AIScriptCopyConditions(&source_state->conditions, &copy->conditions, buffer, end_buffer);
    AIScriptCopyActions(&source_state->actions, &copy->actions, buffer, end_buffer);
    source_reference = (AIREFSCRIPT_PC *)NuLinkedListGetHead(&source_state->reference_scripts);
    while (source_reference != NULL) {
        AIREFSCRIPT_PC *reference_copy;
        reference_copy = (AIREFSCRIPT_PC *)AIScriptBufferAllocOrWarn(buffer, end_buffer, sizeof(AIREFSCRIPT_PC));
        if (reference_copy != NULL) {
            char *reference_string;
            AISCRIPT_COPY_NATIVE_STRING(source_reference->name, reference_string, buffer, end_buffer);
            reference_copy->name = reference_string;
            AISCRIPT_COPY_NATIVE_STRING(source_reference->return_state_name, reference_string, buffer, end_buffer);
            reference_copy->return_state_name = reference_string;
            reference_copy->check_global_scripts = source_reference->check_global_scripts;
            reference_copy->check_level_scripts = source_reference->check_level_scripts;
            AIScriptCopyConditions(&source_reference->conditions, &reference_copy->conditions, buffer, end_buffer);
            NuLinkedListAppend(&copy->reference_scripts, &reference_copy->link);
        }
        source_reference =
            (AIREFSCRIPT_PC *)NuLinkedListGetNext(&source_state->reference_scripts, &source_reference->link);
    }
    return copy;
}

i32 __cdecl AIScriptBuildDerivedScript(AISCRIPT_PC *script, VARIPTR *buffer, VARIPTR *end_buffer, AISYS_PC *sys) {
    AISCRIPT_PC *source_script;
    AISTATE_PC *source_state;
    AISTATE_PC *state;
    AICONDITION_PC *condition;
    i32 source_is_level;
    i32 i;

    if (script == NULL || script->derived_from == NULL || (script->flags_3c & 2) == 0) {
        return 1;
    }

    source_is_level = (script->flags_3c >> 2) & 1;
    source_script = AIScriptFind(sys, script->derived_from, 0, source_is_level, !source_is_level);
    if (source_script != NULL) {
        if (source_script != script) {
            if ((source_script->flags_3c & 2) != 0) {
                return 0;
            }
            source_state = (AISTATE_PC *)NuLinkedListGetHead(&source_script->states);
            while (source_state != NULL) {
                state = (AISTATE_PC *)NuLinkedListGetHead(&script->states);
                while (state != NULL) {
                    if (NuStrICmp(source_state->name, state->name) == 0) {
                        break;
                    }
                    state = (AISTATE_PC *)NuLinkedListGetNext(&script->states, &state->link);
                }
                if (state == NULL) {
                    AISTATE_PC *copy;
                    copy = AIScriptCopyStatePc(source_state, buffer, end_buffer);
                    if (copy != NULL) {
                        NuLinkedListAppend(&script->states, &copy->link);
                    }
                }
                source_state = (AISTATE_PC *)NuLinkedListGetNext(&source_script->states, &source_state->link);
            }

            for (i = 0; i < 4; i++) {
                if (source_script->params[i].name != NULL && script->params[i].name == NULL) {
                    script->params[i].name = AIScriptCopyStringPc(source_script->params[i].name, buffer, end_buffer);
                    script->params[i].default_value = source_script->params[i].default_value;
                }
            }

            state = (AISTATE_PC *)NuLinkedListGetHead(&script->states);
            while (state != NULL) {
                condition = (AICONDITION_PC *)NuLinkedListGetHead(&state->conditions);
                while (condition != NULL) {
                    condition->next_state = AIStateFind(condition->next_state_name, script);
                    if (condition->next_state_name != NULL && condition->next_state == NULL) {
                        if (AIActionDiagnosticMode == 1) {
                            AIScriptMissingStateDiagnosticAlternate(condition->next_state_name, script->name,
                                                                    state->name);
                        } else {
                            AIScriptMissingStateDiagnostic(condition->next_state_name, script->name, state->name);
                        }
                    }
                    condition = (AICONDITION_PC *)NuLinkedListGetNext(&state->conditions, &condition->link);
                }
                state = (AISTATE_PC *)NuLinkedListGetNext(&script->states, &state->link);
            }
        }
    } else if (AIActionDiagnosticMode == 1) {
        AIScriptMissingStateDiagnosticAlternate(script->name, script->derived_from);
    } else {
        AIScriptMissingStateDiagnostic(script->name, script->derived_from);
    }
    script->flags_3c &= ~2u;
    return 1;
}

static void AIScriptFindRefScripts(AISYS_PC *sys, AISCRIPT_PC *script) {
    AIREFSCRIPT_PC *ref;
    AIREFSCRIPT_PC *next;
    AICONDITION_PC *condition;
    AISTATE_PC *state;

    if (sys == NULL || script == NULL) {
        return;
    }

    ref = (AIREFSCRIPT_PC *)NuLinkedListGetHead(&script->reference_scripts);

    while (ref != NULL) {
        next = (AIREFSCRIPT_PC *)NuLinkedListGetNext(&script->reference_scripts, &ref->link);

        if (!(sys->reset_flags & 4) || ref->script == NULL) {
            ref->script = AIScriptFind(sys, ref->name, 1, ref->check_level_scripts, ref->check_global_scripts);
        }

        if (ref->script == NULL || script == ref->script) {
            NuLinkedListRemove(&script->reference_scripts, &ref->link);
            ref = next;

            continue;
        }

        ref->return_state = AIStateFind(ref->return_state_name, script);
        if (ref->return_state == NULL && ref->return_state_name != NULL) {
            if (AIActionDiagnosticMode == 1) {
                AIScriptMissingStateDiagnosticAlternate(ref->return_state_name, script->name);
            } else {
                AIScriptMissingStateDiagnostic(ref->return_state_name, script->name);
            }
        }

        condition = (AICONDITION_PC *)NuLinkedListGetHead(&ref->conditions);

        while (condition != NULL) {
            if (condition->definition->init_fn != NULL) {
                condition->initialized_argument = (*condition->definition->init_fn)(sys, condition->argument, script);
            }

            condition = (AICONDITION_PC *)NuLinkedListGetNext(&ref->conditions, &condition->link);
        }

        ref = next;
    }

    state = (AISTATE_PC *)NuLinkedListGetHead(&script->states);

    while (state != NULL) {
        ref = (AIREFSCRIPT_PC *)NuLinkedListGetHead(&state->reference_scripts);

        while (ref != NULL) {
            next = (AIREFSCRIPT_PC *)NuLinkedListGetNext(&state->reference_scripts, &ref->link);

            if (!(sys->reset_flags & 4) || ref->script == NULL) {
                ref->script = AIScriptFind(sys, ref->name, 1, ref->check_level_scripts, ref->check_global_scripts);
            }

            if (ref->script == NULL || script == ref->script) {
                NuLinkedListRemove(&script->reference_scripts, &ref->link);
            }

            ref->return_state = AIStateFind(ref->return_state_name, script);
            if (ref->return_state == NULL && ref->return_state_name != NULL) {
                if (AIActionDiagnosticMode == 1) {
                    AIScriptMissingStateDiagnosticAlternate(ref->return_state_name, script->name, state->name);
                } else {
                    AIScriptMissingStateDiagnostic(ref->return_state_name, script->name, state->name);
                }
            }

            condition = (AICONDITION_PC *)NuLinkedListGetHead(&ref->conditions);

            while (condition != NULL) {
                if (condition->definition->init_fn != NULL) {
                    condition->initialized_argument =
                        (*condition->definition->init_fn)(sys, condition->argument, script);
                }

                condition = (AICONDITION_PC *)NuLinkedListGetNext(&ref->conditions, &condition->link);
            }

            ref = next;
        }

        state = (AISTATE_PC *)NuLinkedListGetNext(&script->states, &state->link);
    }
}

void __cdecl AIScriptInitConditions(AISYS_PC *sys) {
    AISCRIPT_PC *script;
    AISTATE_PC *state;
    AICONDITION_PC *condition;
    AICONDITION_PC *parameter_condition;

    NuLinkedListGetHead(&global_aiscripts);

    if (sys != NULL) {
        script = (AISCRIPT_PC *)NuLinkedListGetHead(&sys->scripts);

        while (script != NULL) {
            state = (AISTATE_PC *)NuLinkedListGetHead(&script->states);

            while (state != NULL) {
                condition = (AICONDITION_PC *)NuLinkedListGetHead(&state->conditions);

                while (condition != NULL) {
                    if (condition->definition->init_fn != NULL) {
                        LOG_WARN_ONCE_IF((u32)condition->definition->init_fn == (u32)&__ImageBase,
                                         "AI condition init unresolved: name=%.64s def=%p fn=%p sys=%p script=%p "
                                         "state=%p condition=%p arg=%p; build=log-banner",
                                         condition->definition->name, (void *)condition->definition,
                                         (void *)condition->definition->init_fn, (void *)sys, (void *)script,
                                         (void *)state, (void *)condition, (void *)condition->argument);
                        condition->initialized_argument =
                            (*condition->definition->init_fn)(sys, condition->argument, script);
                    }

                    parameter_condition = condition->parameter_condition;
                    if (parameter_condition != NULL && parameter_condition->definition != NULL &&
                        parameter_condition->definition->init_fn != NULL) {
                        condition->parameter_condition->initialized_argument =
                            (*parameter_condition->definition->init_fn)(sys, parameter_condition->argument, script);
                    }

                    condition = (AICONDITION_PC *)NuLinkedListGetNext(&state->conditions, &condition->link);
                }

                state = (AISTATE_PC *)NuLinkedListGetNext(&script->states, &state->link);
            }

            script->base_state = AIStateFind("Base", script);

            AIScriptFindRefScripts(sys, script);

            script = (AISCRIPT_PC *)NuLinkedListGetNext(&sys->scripts, &script->link);
        }
    }

    script = (AISCRIPT_PC *)NuLinkedListGetHead(&global_aiscripts);

    while (script != NULL) {
        state = (AISTATE_PC *)NuLinkedListGetHead(&script->states);

        while (state != NULL) {
            condition = (AICONDITION_PC *)NuLinkedListGetHead(&state->conditions);

            while (condition != NULL) {
                if (condition->definition->init_fn != NULL) {
                    if (sys == NULL) {
                        condition->initialized_argument =
                            (*condition->definition->init_fn)(NULL, condition->argument, script);
                    } else if (!(sys->reset_flags & 4) || condition->initialized_argument == NULL) {
                        condition->initialized_argument =
                            (*condition->definition->init_fn)(sys, condition->argument, script);
                    }
                }

                condition = (AICONDITION_PC *)NuLinkedListGetNext(&state->conditions, &condition->link);
            }

            state = (AISTATE_PC *)NuLinkedListGetNext(&script->states, &state->link);
        }

        script->base_state = AIStateFind("Base", script);

        AIScriptFindRefScripts(sys, script);

        script = (AISCRIPT_PC *)NuLinkedListGetNext(&global_aiscripts, &script->link);
    }
}

static __declspec(noinline) i32 AIScriptNextAction(AISCRIPTPROCESS_PC *processor) {
    AIACTION_PC *action;
    if (processor == NULL)
        return 0;
    if (processor->action_node != NULL)
        action = (AIACTION_PC *)NuLinkedListGetNext(&processor->state->actions, &processor->action_node->link);
    else
        action = (AIACTION_PC *)NuLinkedListGetHead(&processor->state->actions);
    if (action != NULL)
        processor->flags |= 1;
    processor->action_node = action;
    return 1;
}

void __cdecl AIScriptSetState(AISCRIPTPROCESS_PC *processor, AISTATE_PC *state) {
    if (processor != NULL && state != NULL) {
        if ((processor->flags & 4) == 0)
            processor->unknown_b2 = 0;
        processor->flags &= ~4u;
        processor->state = state;
        processor->action_node = NULL;
        processor->next_state = NULL;
        processor->param_stack[0].is_first_time_state = 1;
        AIScriptNextAction(processor);
    }
}

void __cdecl AIScriptForceParamReEval(AISCRIPTPROCESS_PC *processor) {
    i32 index;
    if (processor != NULL) {
        for (index = 0; index < 2; ++index)
            processor->param_stack[index].is_first_time_state = 1;
    }
}

i32 AIScriptSetStateByName(AISCRIPTPROCESS_PC *processor, char *name) {
    AISTATE_PC *state;

    if (name != NULL && processor != NULL && processor->script != NULL) {
        state = AIStateFind(name, processor->script);

        if (state != NULL) {
            AIScriptSetState(processor, state);

            return 1;
        }
    }

    return 0;
}

void __cdecl AIScriptProcessorInit(AISYS_PC *system, AIPACKET_PC *packet, AISCRIPTPROCESS_PC *processor,
                                   AICREATURE_PC *creature, char *script_name, char *start_state_name,
                                   i32 can_use_default, AISCRIPT_PC *script, AISTATE_PC *start_state) {
    i32 i;
    if (processor == NULL)
        return;
    processor->flags &= ~4u;
    processor->unknown_b2 = 0;
    if (script == NULL) {
        script = AIScriptFind(system, script_name, can_use_default, 1, 1);
        processor->local_messages = NULL;
    }
    if (script != NULL) {
        processor->script = script;
        processor->next_state = NULL;
        if (processor->base_script == NULL)
            processor->base_script = script;
        if (creature != NULL) {
            for (i = 0; i < 4; i++)
                processor->params[i] = creature->script_params[i];
            for (i = 0; i < 4; i++) {
                if ((creature->flags & ((i64)2 << i)) == 0)
                    processor->params[i] = script->params[i].default_value;
            }
        } else {
            for (i = 0; i < 4; i++)
                processor->params[i] = script->params[i].default_value;
        }
        if (start_state == NULL && start_state_name != NULL)
            start_state = AIStateFind(start_state_name, script);
        if (start_state != NULL)
            AIScriptSetState(processor, start_state);
        else
            AIScriptSetStateByName(processor, (char *)AIScriptKeywordBase);
    }
    processor->interrupt_timer = 0.0f;
    processor->interrupt_priority = 0;
    processor->interrupt_id = 0;
    processor->interrupt_state = NULL;
    processor->return_to_state = NULL;
}

f32 __cdecl AIParamToFloat(AISCRIPTPROCESS_PC *processor, char *param) {
    char *cursor;
    u32 index;

    if (param == NULL) {
        return 0.0f;
    }

    if (processor != NULL) {
        cursor = NuStrIStr(param, (char *)AIScriptParamPrefix);
        if (cursor != NULL) {
            index = (u32)NuAToI(cursor);
            if (index <= 3) {
                return processor->params[index];
            }
        } else if (processor->script != NULL) {
            for (index = 0; index < 4; ++index) {
                if (processor->script->params[index].name != NULL &&
                    NuStrICmp(processor->script->params[index].name, param) == 0) {
                    return processor->params[index];
                }
            }
        }
    }

    return NuAToF(param);
}

static i32 __cdecl AiEvalExpressionNameLoopup(char *expression, f32 *float_out, i32 *int_out) {
    f32 value;
    i32 got_value;
    got_value = 0;
    value = 0.0f;
    if (NuStrICmp(expression, AIExpressionRandName) == 0) {
        got_value = 1;
        value = NuRandFloat();
    } else if (NuStrICmp(expression, AIScriptFalse) == 0) {
        got_value = 1;
        value = 0.0f;
    } else if (NuStrICmp(expression, AIExpressionTrueName) == 0) {
        got_value = 1;
        value = 1.0f;
    } else if (NuStrICmp(expression, AIExpressionMaxName) == 0) {
        got_value = 1;
        value = FLT_MAX;
    }
    if (got_value == 0) {
        if (GameParamToFloatFn != NULL)
            got_value = GameParamToFloatFn(current_packet, current_processor, expression, &value);
        if (got_value == 0)
            value = AIParamToFloat(current_processor, expression);
    }
    if (float_out != NULL)
        *float_out = value;
    if (int_out != NULL)
        *int_out = (i32)value;
    return 0;
}

f32 __cdecl AIParamToFloatEx(AIPACKET_PC *packet, AISCRIPTPROCESS_PC *processor, char *param) {
    char *cursor;
    f32 result;
    i32 is_number;
    is_number = 1;
    cursor = param;
    while (*cursor != '\0') {
        if (*cursor < -45 || *cursor > '9' || *cursor == '/') {
            is_number = 0;
            break;
        }
        cursor++;
    }
    if (is_number != 0)
        return NuAToF(param);
    current_processor = processor;
    current_packet = packet;
    result = NuRDPFVar(param, AiEvalExpressionNameLoopup);
    current_processor = NULL;
    return result;
}

f32 __cdecl Condition_Timer(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char *argument,
                            void *initialized_argument) {
    (void)sys;
    (void)packet;
    (void)argument;
    (void)initialized_argument;
    return processor->script_timer;
}

f32 __cdecl Condition_Random(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char *argument,
                             void *initialized_argument) {
    (void)sys;
    (void)processor;
    (void)packet;
    (void)argument;
    (void)initialized_argument;
    return NuRandFloat();
}

void *Condition_AlwaysTrueInit(AISYS_PC *sys, char *arg, AISCRIPT_PC *script) {
    f32 value;

    if (arg != NULL && NuStrLen(arg) != 0) {
        value = NuAToF(arg);

        return *(void **)&value;
    }

    value = 1.0f;
    return *(void **)&value;
}

f32 __cdecl Condition_AlwaysTrue(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char *argument,
                                 void *initialized_argument) {
    union {
        void *pointer;
        f32 value;
    } result;
    (void)sys;
    (void)processor;
    (void)packet;
    (void)argument;
    result.pointer = initialized_argument;
    return result.value;
}

f32 __cdecl Condition_LocatorRange(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                   void *argument) {
    AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
    NUVEC delta;
    if (packet != NULL && packet->owner != NULL) {
        if (locator == NULL)
            locator = process->locator;
        if (locator != NULL)
            return NuVecDist(&packet->terrain_origin, &locator->position, &delta);
    }
    return FLT_MAX;
}

f32 __cdecl Condition_LocatorRangeXZPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
    NUVEC delta;
    if (packet != NULL && packet->owner != NULL) {
        if (locator == NULL)
            locator = process->locator;
        if (locator != NULL)
            return NuVecXZDist(&packet->terrain_origin, &locator->position, &delta);
    }
    return FLT_MAX;
}

f32 __cdecl Condition_LocatorRangeYPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
    if (packet != NULL && packet->owner != NULL) {
        if (locator == NULL)
            locator = process->locator;
        if (locator != NULL) {
            f64 difference = (f64)packet->owner->position.y - (f64)locator->position.y;
            return (f32)difference;
        }
    }
    return 0.0f;
}

f32 __cdecl Condition_GotLocator(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                 void *argument) {
    if (process->locator != NULL)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_GotLocatorSetPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    if (process->locator_set != NULL)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_OnPathPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                               void *argument) {
    if (packet != NULL && (packet->path_info.flags & 1) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_PlayerOnPathPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                     void *argument) {
    APIOBJECT_PC *player = system->player_1;
    if (player != NULL && (player->ai->path_info.flags & 1) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_OpponentOnPathPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    (void)system;
    (void)process;
    (void)name;
    (void)argument;
    if (packet != NULL && packet->opponent != NULL &&
        ((((APIOBJECT_PC *)packet->opponent)->ai->path_info.flags & 1) != 0))
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_TimeOffPathPc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                    char *parameter, void *argument) {
    if (packet != NULL)
        return packet->time_off_path;
    return 0.0f;
}

f32 __cdecl Condition_CurrentLocatorIsPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                         void *argument) {
    if (argument != NULL && process->locator == argument)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_InTriggerAreaPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    if (packet != NULL && packet->owner != NULL) {
        GAMEOBJECT *object = packet->owner;
        AIAREA_PC *area = (AIAREA_PC *)argument;
        if (area == NULL)
            area = ((CONDITION_TRIGGER_PROCESS_PC *)process)->trigger_area;
        if (area != NULL && area->system != NULL) {
            i32 index = (i32)(area - area->system->areas);
            i64 mask = (i32)(1u << (index & 31));
            u64 membership = ((u64)object->ai_area_mask[1] << 32) | object->ai_area_mask[0];
            if ((membership & mask) != 0)
                return 1.0f;
        }
    }
    return 0.0f;
}

f32 __cdecl Condition_PlayerRange(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    NUVEC delta;
    if (packet != NULL && packet->owner != NULL && system != NULL && system->player_1 != NULL)
        return NuVecDist(&system->player_1->position, &packet->owner->position, &delta);
    return FLT_MAX;
}

f32 __cdecl Condition_NearestPlayerRangePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                           char *name, void *argument) {
    NUVEC difference;
    f32 distance = FLT_MAX;
    if (packet != NULL && packet->owner != NULL && system != NULL) {
        if (system->player_1 != NULL)
            distance = NuVecDistSqr(&system->player_1->position, &packet->owner->position, &difference);
        if (system->player_2 != NULL) {
            f32 second = NuVecDistSqr(&system->player_2->position, &packet->owner->position, &difference);
            distance = second < distance ? second : distance;
        }
        if ((f64)distance != (f64)FLT_MAX)
            distance = NuFsqrt(distance);
    }
    return distance;
}

f32 __cdecl Condition_NearestPlayerXZRangePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                             char *name, void *argument) {
    NUVEC difference;
    f32 distance = FLT_MAX;
    if (packet != NULL && packet->owner != NULL && system != NULL) {
        if (system->player_1 != NULL)
            distance = NuVecXZDistSqr(&system->player_1->position, &packet->owner->position, &difference);
        if (system->player_2 != NULL) {
            f32 second = NuVecXZDistSqr(&system->player_2->position, &packet->owner->position, &difference);
            distance = second < distance ? second : distance;
        }
        if ((f64)distance != (f64)FLT_MAX)
            distance = NuFsqrt(distance);
    }
    return distance;
}

f32 __cdecl Condition_InLevelNodePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    if (packet != NULL && argument != NULL && system != NULL) {
        if (packet->inside_path_node == (AIPATHNODE_PC *)argument - system->path_sys->active_path->nodes)
            return 1.0f;
    }
    return 0.0f;
}

f32 __cdecl Condition_PlayerInLevelNodePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                          char *name, void *argument) {
    if (system != NULL && system->player_1 != NULL && argument != NULL) {
        if (system->player_1->ai->inside_path_node == (AIPATHNODE_PC *)argument - system->path_sys->active_path->nodes)
            return 1.0f;
    }
    return 0.0f;
}

f32 __cdecl Condition_EitherPlayerInLevelNodePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                char *name, void *argument) {
    if (system != NULL && argument != NULL) {
        if (system->player_1 != NULL &&
            system->player_1->ai->inside_path_node == (AIPATHNODE_PC *)argument - system->path_sys->active_path->nodes)
            return 1.0f;
        if (system->player_2 != NULL &&
            system->player_2->ai->inside_path_node == (AIPATHNODE_PC *)argument - system->path_sys->active_path->nodes)
            return 1.0f;
    }
    return 0.0f;
}

f32 __cdecl Condition_NodeRangePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    AIPATHNODE_PC *node = (AIPATHNODE_PC *)argument;
    if (packet != NULL && packet->owner != NULL && node != NULL) {
        NUVEC difference;
        return NuVecDist(&node->position, &packet->owner->position, &difference);
    }
    return FLT_MAX;
}

f32 __cdecl Condition_GotTriggerAreaPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                       void *argument) {
    if (process->unknown_a0 != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_PlayerInTriggerAreaPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                            char *name, void *argument) {
    if (system != NULL && system->player_1 != NULL && system->areas != NULL) {
        GAMEOBJECT *object = (GAMEOBJECT *)system->player_1;
        AIAREA_PC *area = (AIAREA_PC *)argument;
        if (area == NULL)
            area = ((CONDITION_TRIGGER_PROCESS_PC *)process)->trigger_area;
        if (area != NULL && area->system != NULL) {
            i32 index = (i32)(area - area->system->areas);
            i64 mask = (i32)(1u << (index & 31));
            u64 membership = ((u64)object->ai_area_mask[1] << 32) | object->ai_area_mask[0];
            if ((membership & mask) != 0)
                return 1.0f;
        }
    }
    return 0.0f;
}

f32 __cdecl Condition_Player2InTriggerAreaPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                             char *name, void *argument) {
    if (system != NULL) {
        GAMEOBJECT *object = (GAMEOBJECT *)system->player_2;
        if (object != NULL) {
            AIAREA_PC *area = (AIAREA_PC *)argument;
            if (area == NULL)
                area = ((CONDITION_TRIGGER_PROCESS_READER_PC *)process)->trigger_area;
            if (area != NULL && area->system != NULL) {
                i32 index = (i32)(area - area->system->areas);
                i64 mask = (i32)(1u << index);
                u64 membership = ((CONDITION_TRIGGER_PLAYER_READER_PC *)object)->membership;
                if ((membership & mask) != 0)
                    return 1.0f;
            }
        }
    }
    return 0.0f;
}

f32 __cdecl Condition_EitherPlayerInTriggerAreaPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                                  char *name, void *argument) {
    AIAREA_PC *area;
    GAMEOBJECT *object;
    i32 index;
    i64 mask;
    if (system != NULL) {
        area = (AIAREA_PC *)argument;
        if (area == NULL)
            area = ((CONDITION_TRIGGER_PROCESS_READER_PC *)process)->trigger_area;
        if (area != NULL) {
            if (system->player_1 != NULL && area->system != NULL) {
                object = (GAMEOBJECT *)system->player_1;
                index = (i32)(area - area->system->areas);
                /* Retail sign-extends a 32-bit shift into the 64-bit mask. */
                mask = (i32)(1u << index);
                if ((((CONDITION_TRIGGER_PLAYER_READER_PC *)object)->membership & mask) != 0)
                    return 1.0f;
            }
            if (system->player_2 != NULL && area->system != NULL) {
                object = (GAMEOBJECT *)system->player_2;
                index = (i32)(area - area->system->areas);
                mask = (i32)(1u << index);
                if ((((CONDITION_TRIGGER_PLAYER_READER_PC *)object)->membership & mask) != 0)
                    return 1.0f;
            }
        }
    }
    return 0.0f;
}

f32 __cdecl Condition_BaddyInTriggerAreaPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                           char *name, void *argument) {
    AIAREA_PC *area = (AIAREA_PC *)argument;
    if (area == NULL)
        area = (AIAREA_PC *)process->unknown_a0;
    if (area != NULL && (area->runtime_flags & 4) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_GoodyInTriggerAreaPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                           char *name, void *argument) {
    AIAREA_PC *area = (AIAREA_PC *)argument;
    if (area == NULL)
        area = (AIAREA_PC *)process->unknown_a0;
    if (area != NULL && (area->runtime_flags & 2) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_OpponentInTriggerAreaPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                              char *name, void *argument) {
    if (packet != NULL && packet->opponent != NULL && packet->owner != NULL) {
        GAMEOBJECT *object = packet->opponent;
        AIAREA_PC *area = (AIAREA_PC *)argument;
        if (area == NULL)
            area = ((CONDITION_TRIGGER_PROCESS_PC *)process)->trigger_area;
        if (area != NULL && area->system != NULL) {
            i32 index = (i32)(area - area->system->areas);
            i64 mask = (i32)(1u << (index & 31));
            u64 membership = ((u64)object->ai_area_mask[1] << 32) | object->ai_area_mask[0];
            if ((membership & mask) != 0)
                return 1.0f;
        }
    }
    return 0.0f;
}

f32 __cdecl Condition_GotOpponent(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                  void *argument) {
    if (packet != NULL && packet->opponent != NULL)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_OpponentIsAThreatPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                          char *name, void *argument) {
    (void)system;
    (void)process;
    (void)name;
    (void)argument;
    if (packet != NULL && packet->opponent != NULL && (packet->flags & 0x800u) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_OpponentOnSamePath(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                         void *argument) {
    APIOBJECT_PC *opponent;
    if (packet == NULL || packet->opponent == NULL || packet->path_info.path == NULL)
        return 0.0f;
    opponent = (APIOBJECT_PC *)packet->opponent;
    if (opponent->ai != NULL && packet->path_info.path == opponent->ai->path_info.path)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_OpponentRange(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    if (packet != NULL && packet->opponent != NULL)
        return packet->opponent_metric;
    return FLT_MAX;
}

f32 __cdecl Condition_NearestOpponentRangePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                             char *name, void *argument) {
    (void)system;
    (void)process;
    (void)name;
    (void)argument;
    if (packet != NULL && packet->nearest_opponent != NULL)
        return packet->nearest_opponent_metric;
    return FLT_MAX;
}

f32 __cdecl Condition_YawToOpponentPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    if (packet != NULL && packet->opponent != NULL) {
        NUVEC difference;
        difference.x = packet->opponent->position.x - packet->owner->position.x;
        difference.z = packet->opponent->position.z - packet->owner->position.z;
        /* Retail leaves difference.y unwritten before this in-place rotation. */
        NuVecRotateY(&difference, &difference, -packet->owner->field_0x276);
        return (f32)((f64)NuAtan2D(difference.x, difference.z) * (360.0 / 65536.0));
    }
    return -180.0f;
}

f32 __cdecl Condition_OpponentBelowPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                      void *argument) {
    (void)system;
    (void)process;
    (void)name;
    (void)argument;
    if (packet != NULL && packet->opponent != NULL &&
        (f64)packet->opponent->position.y < (f64)packet->owner->position.y - (f64)0.1f)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_OriginRangePc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    NUVEC *origin;
    (void)process;
    (void)name;
    (void)argument;
    if (packet != NULL) {
        origin = GetOriginPositionFn != NULL ? GetOriginPositionFn(system, packet) : NULL;
        if (origin != NULL)
            return NuVecDist(&packet->terrain_origin, origin, NULL);
        if (system != NULL && packet->creature_index != 0xff)
            return NuVecDist(&packet->terrain_origin, &system->creatures[packet->creature_index].position, NULL);
    }
    return 0.0f;
}

f32 __cdecl Condition_OpponentToOriginPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                         void *argument) {
    if (packet != NULL && packet->opponent != NULL) {
        if (GetOriginPositionFn != NULL) {
            NUVEC *origin = GetOriginPositionFn(system, packet);
            if (origin != NULL)
                return NuVecDist(&packet->opponent->position, origin, NULL);
        }
        if (system != NULL && packet->creature_index != 0xff)
            return NuVecDist(&packet->opponent->position, &system->creatures[packet->creature_index].position, NULL);
    }
    return FLT_MAX;
}

f32 __cdecl Condition_PlayerToOriginPc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                       char *parameter, void *argument) {
    if (packet != NULL && system != NULL && system->player_1 != NULL) {
        NUVEC *origin;
        if (GetOriginPositionFn != NULL && (origin = GetOriginPositionFn(system, packet)) != NULL)
            return NuVecDist(&system->player_1->position, origin, NULL);
        if (packet->creature_index != 0xff)
            return NuVecDist(&system->player_1->position, &system->creatures[packet->creature_index].position, NULL);
    }
    return FLT_MAX;
}

f32 __cdecl Condition_OpponentToLocatorPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                          char *name, void *argument) {
    NUVEC difference;
    if (packet != NULL && packet->opponent != NULL) {
        AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
        if (locator == NULL)
            locator = process->locator;
        if (locator != NULL)
            return NuVecDist(&packet->opponent->position, &locator->position, &difference);
    }
    return FLT_MAX;
}

f32 __cdecl Condition_OpponentToLocatorXZPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                            char *name, void *argument) {
    NUVEC difference;
    if (packet != NULL && packet->opponent != NULL) {
        AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
        if (locator == NULL)
            locator = process->locator;
        if (locator != NULL)
            return NuVecXZDist(&packet->opponent->position, &locator->position, &difference);
    }
    return FLT_MAX;
}

f32 __cdecl Condition_OpponentToLocatorYPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                           char *name, void *argument) {
    if (packet != NULL && packet->opponent != NULL) {
        AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
        if (locator == NULL)
            locator = process->locator;
        if (locator != NULL)
            return packet->opponent->position.y - locator->position.y;
    }
    return FLT_MAX;
}

f32 __cdecl Condition_PlayerToLocatorPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                        void *argument) {
    NUVEC difference;
    if (packet != NULL && system != NULL && system->player_1 != NULL) {
        AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
        if (locator == NULL)
            locator = process->locator;
        if (locator != NULL)
            return NuVecDist(&system->player_1->position, &locator->position, &difference);
    }
    return FLT_MAX;
}

f32 __cdecl Condition_NearestPlayerToLocatorPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet,
                                               char *name, void *argument) {
    NUVEC difference;
    f32 distance = FLT_MAX;
    if (packet != NULL && system != NULL) {
        AILOCATOR_PC *locator = (AILOCATOR_PC *)argument;
        if (locator == NULL)
            locator = process->locator;
        if (locator != NULL) {
            if (system->player_1 != NULL)
                distance = NuVecDistSqr(&system->player_1->position, &locator->position, &difference);
            if (system->player_2 != NULL) {
                f32 second = NuVecDistSqr(&system->player_2->position, &locator->position, &difference);
                if (second < distance)
                    distance = second;
            }
        }
        if ((f64)distance != (f64)FLT_MAX)
            distance = NuFsqrt(distance);
    }
    return distance;
}

f32 __cdecl Condition_PathBlockedPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                                    void *argument) {
    if (packet != NULL && (packet->flags & 0x400000) != 0)
        return 1.0f;
    return 0.0f;
}

f32 __cdecl Condition_InterruptIDPc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                    char *parameter, void *argument) {
    if (processor != NULL)
        return (f32)processor->interrupt_id;
    return -1.0f;
}

void *__cdecl Condition_IAmInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL && GetNamedGameObjectCallback != NULL)
        return GetNamedGameObjectCallback(system, name);
    return NULL;
}

f32 __cdecl Condition_IAmPc(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char *name,
                            void *argument) {
    if (packet != NULL && packet->owner != NULL) {
        if (packet->owner == argument)
            return 1.0f;
        if (packet->owner->character_data != NULL && packet->owner->character_data->name != NULL &&
            NuStrICmp(packet->owner->character_data->name, name) == 0)
            return 1.0f;
    }
    return 0.0f;
}

f32 __cdecl Condition_StuckTimePc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char *parameter,
                                  void *argument) {
    if (packet != NULL && packet->owner != NULL)
        return packet->owner->respawn_timer;
    return 0.0f;
}

f32 __cdecl Condition_ParamPc(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char *parameter,
                              void *argument) {
    return AIParamToFloatEx(packet, processor, parameter);
}

i32 __cdecl Action_SetState(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 count, i32 first_time, f32 elapsed) {
    i32 i;
    if (first_time == 0 || count == 0)
        return 0;
    processor->next_state = AIStateFind(params[0], processor->script);
    processor->flags &= ~4u;
    for (i = 1; i < count; i++) {
        if (NuStrICmp(params[i], "KeepBlockedMessages") == 0)
            processor->flags |= 4;
    }
    if (processor->next_state == NULL) {
        if (AIActionDiagnosticMode == 1)
            AIScriptMissingStateDiagnosticAlternate(params[0], processor->script->name, processor->state->name);
        else
            AIScriptMissingStateDiagnostic(params[0], processor->script->name, processor->state->name);
    }
    return 0;
}

i32 __cdecl Action_ResetTimer(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 count, i32 first_time, f32 elapsed) {
    f32 minimum;
    f32 maximum;
    f32 exact;
    f64 first_term;
    f32 random;
    char *value;
    i32 i;
    if (first_time == 0)
        return 1;
    minimum = maximum = exact = 0.0f;
    for (i = 0; i < count; i++) {
        value = NuStrIStr(params[i], (char *)AIActionMinTimeName);
        if (value != NULL)
            minimum = AIParamToFloatEx(packet, processor, value + 8);
        else if ((value = NuStrIStr(params[i], (char *)AIActionMaxTimeName)) != NULL)
            maximum = AIParamToFloatEx(packet, processor, value + 8);
        else if ((value = NuStrIStr(params[i], (char *)AIActionTimeName)) != NULL)
            exact = AIParamToFloatEx(packet, processor, value + 5);
    }
    if (minimum != 0.0f || maximum != 0.0f) {
        first_term = (1.0 - NuRandFloat()) * minimum;
        random = NuRandFloat();
        processor->script_timer = (f32)(random * maximum + first_term);
    } else
        processor->script_timer = exact;
    return 1;
}

i32 __cdecl Action_Idle(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params, i32 count,
                        i32 first_time, f32 elapsed) {
    f32 minimum;
    f32 maximum;
    i32 frames;
    i32 i;
    char *value;
    minimum = maximum = 0.0f;
    frames = 0;
    if (first_time != 0) {
        for (i = 0; i < count; i++) {
            value = NuStrIStr(params[i], (char *)AIActionMinTimeName);
            if (value != NULL)
                minimum = AIParamToFloatEx(packet, processor, value + NuStrLen(AIActionMinTimeName) + 1);
            else if ((value = NuStrIStr(params[i], (char *)AIActionMaxTimeName)) != NULL)
                maximum = AIParamToFloatEx(packet, processor, value + NuStrLen(AIActionMaxTimeName) + 1);
            else if ((value = NuStrIStr(params[i], (char *)AIActionFramesName)) != NULL)
                frames = (i32)AIParamToFloatEx(packet, processor, value + NuStrLen(AIActionFramesName) + 1);
            else
                processor->action_timer = AIParamToFloatEx(packet, processor, params[i]);
        }
        if (frames != 0) {
            processor->action_data_1 = (u8)(frames < 0 ? 0 : (frames > 255 ? 255 : frames));
        } else if (processor->action_timer == 0.0f && maximum > minimum) {
            processor->action_timer = NuRandFloat() * (maximum - minimum) + minimum;
        }
        return 0;
    }
    if (processor->action_data_1 != 0) {
        processor->action_data_1--;
        return processor->action_data_1 == 0;
    }
    if (processor->action_timer > 0.0f) {
        processor->action_timer -= elapsed;
        if (processor->action_timer <= 0.0f) {
            processor->action_timer = 0.0f;
            return 1;
        }
    }
    return 0;
}

i32 __cdecl Action_SetCircleDirection(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                      char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 i;

    if (packet != NULL && packet->owner != NULL && first_time != 0 && param_count != 0) {
        for (i = 0; i < param_count; i++) {
            if (NuStrICmp(params[i], AIActionClockwiseName) == 0) {
                packet->flags |= 0x200u;
            } else if (NuStrICmp(params[i], AIActionAnticlockwiseName) == 0) {
                packet->flags &= ~0x200u;
            } else if (NuStrICmp(params[i], AIActionReverseName) == 0) {
                packet->flags ^= 0x200u;
            } else if (NuStrICmp(params[i], AIActionRandomName) == 0) {
                if (NuRandFloat() > 0.5f) {
                    packet->flags |= 0x200u;
                } else {
                    packet->flags &= ~0x200u;
                }
            }
        }
    }
    return 1;
}

i32 __cdecl Action_FacePlayer(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 param_count, i32 first_time, f32 elapsed) {
    f32 min_time;
    f32 max_time;
    i32 index;
    char *value;
    f32 remaining_time;

    if (packet == NULL) {
        return 1;
    }
    if (first_time != 0) {
        min_time = 0.0f;
        max_time = 0.0f;
        for (index = 0; index < param_count; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionMinTimeName);
            if (value != NULL) {
                min_time = AIParamToFloatEx(packet, processor, value + 8);
            } else if ((value = NuStrIStr(params[index], (char *)AIActionMaxTimeName)) != NULL) {
                max_time = AIParamToFloatEx(packet, processor, value + 8);
            } else {
                processor->action_timer = AIParamToFloatEx(packet, processor, params[index]);
            }
        }
        if (processor->action_timer == 0.0f && min_time < max_time) {
            processor->action_timer = NuRandFloat() * (max_time - min_time) + min_time;
        }
    }
    remaining_time = processor->action_timer;
    if (sys->player_1 != NULL) {
        packet->movement_look_target = &sys->player_1->position;
    }
    if (remaining_time > 0.0f) {
        remaining_time -= elapsed;
        processor->action_timer = remaining_time;
        if (remaining_time <= 0.0f) {
            processor->action_timer = 0.0f;
            return 1;
        }
    }
    return 0;
}

i32 __cdecl Action_FaceOpponent(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_count, i32 first_time, f32 elapsed) {
    f32 min_time;
    f32 max_time;
    i32 index;
    char *value;
    APIOBJECT_PC *opponent;
    NUVEC direction;

    (void)sys;
    if (packet == NULL) {
        return 1;
    }
    if (first_time != 0) {
        min_time = 0.0f;
        max_time = 0.0f;
        for (index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                continue;
            }
            value = NuStrIStr(params[index], (char *)AIActionMinTimeName);
            if (value != NULL) {
                min_time = AIParamToFloatEx(packet, processor, value + 8);
            } else if ((value = NuStrIStr(params[index], (char *)AIActionMaxTimeName)) != NULL) {
                max_time = AIParamToFloatEx(packet, processor, value + 8);
            } else if ((value = NuStrIStr(params[index], (char *)AIActionFaceOffsetName)) != NULL) {
                processor->action_data_4 = AIParamToFloatEx(packet, processor, value + 11);
            } else if (NuStrICmp(params[index], AIActionNearestOpponentName) == 0) {
                processor->action_data_1 = 1;
            } else {
                processor->action_timer = AIParamToFloatEx(packet, processor, params[index]);
            }
        }
        if (processor->action_timer == 0.0f && min_time < max_time) {
            processor->action_timer = NuRandFloat() * (max_time - min_time) + min_time;
        }
    }

    opponent =
        processor->action_data_1 != 0 ? ((APIOBJECT_PC *)packet->nearest_opponent) : ((APIOBJECT_PC *)packet->opponent);
    if (opponent != NULL && opponent->ai != NULL) {
        if (processor->action_data_4 == 0.0f) {
            packet->movement_look_target = &opponent->position;
        } else {
            direction.x = opponent->position.z - packet->owner->position.z;
            direction.y = 0.0f;
            direction.z = packet->owner->position.x - opponent->position.x;
            NuVecNorm(&direction, &direction);
            processor->action_pos.x = opponent->position.x + direction.x * processor->action_data_4;
            processor->action_pos.y = opponent->position.y;
            processor->action_pos.z = opponent->position.z + direction.z * processor->action_data_4;
            packet->movement_look_target = &processor->action_pos;
        }
    }
    if (processor->action_timer > 0.0f) {
        processor->action_timer -= elapsed;
        if (processor->action_timer <= 0.0f) {
            processor->action_timer = 0.0f;
            return 1;
        }
    }
    return 0;
}

i32 __cdecl Action_IgnoreWallSplines(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                     char **params, i32 count, i32 first_time, f32 elapsed) {
    i32 i;

    if (packet != NULL && first_time != 0) {
        packet->flags |= 0x80u;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], AIScriptFalse) == 0)
                packet->flags &= ~0x80u;
        }
    }
    return 1;
}

i32 __cdecl Action_CheckWallSplines(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 count, i32 first_time, f32 elapsed) {
    i32 i;

    if (packet != NULL && first_time != 0) {
        packet->flags |= 0x100u;
        for (i = 0; i < count; i++) {
            if (NuStrICmp(params[i], AIScriptFalse) == 0)
                packet->flags &= ~0x100u;
        }
        if ((packet->flags & 0x100u) != 0)
            packet->flags |= 0x80u;
    }
    return 1;
}

i32 __cdecl Action_NoTerrain(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 count, i32 first_time, f32 delta_time) {
    (void)sys;
    (void)processor;
    (void)delta_time;
    AISCRIPT_SET_OWNER_FLAG(packet, params, count, first_time, flags_1fc_word, 0x20u, 1, 0);
    return 1;
}

i32 __cdecl Action_NoLosCheck(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 count, i32 first_time, f32 delta_time) {
    (void)sys;
    (void)processor;
    (void)delta_time;
    AISCRIPT_SET_OWNER_FLAG(packet, params, count, first_time, flags_1fc_word, 0x400u, 1, 0);
    return 1;
}

i32 __cdecl Action_FlatTerrain(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 count, i32 first_time, f32 delta_time) {
    (void)sys;
    (void)processor;
    (void)delta_time;
    AISCRIPT_SET_OWNER_FLAG(packet, params, count, first_time, flags_1fc_word, 0x10u, 1, 0);
    return 1;
}

i32 __cdecl Action_ShadowTerrain(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_count, i32 first_time, f32 elapsed) {
    i32 i;

    if (packet != NULL && packet->owner != NULL && first_time != 0) {
        packet->owner->flags_1fc |= 8;
        for (i = 0; i < param_count; i++) {
            if (NuStrICmp(params[i], AIScriptFalse) == 0) {
                packet->owner->flags_1fc &= ~8;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_DontUseShadowTerrain(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                        char **params, i32 count, i32 first_time, f32 delta_time) {
    (void)sys;
    (void)processor;
    (void)delta_time;
    AISCRIPT_SET_OWNER_FLAG(packet, params, count, first_time, flags_1fc_word, 0x40u, 1, 0);
    return 1;
}

i32 __cdecl Action_DontPush(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params, i32 count,
                            i32 first_time, f32 delta_time) {
    (void)sys;
    (void)processor;
    (void)delta_time;
    AISCRIPT_SET_OWNER_FLAG(packet, params, count, first_time, flags_1fc_word, 0x2u, 1, 0);
    return 1;
}

i32 __cdecl Action_CanSeeBehind(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 count, i32 first_time, f32 delta_time) {
    (void)sys;
    (void)processor;
    (void)delta_time;
    AISCRIPT_SET_OWNER_FLAG(packet, params, count, first_time, flags_1fc_word, 0x800u, 1, 0);
    return 1;
}

i32 __cdecl Action_RequiresLOS(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 count, i32 first_time, f32 delta_time) {
    (void)sys;
    (void)processor;
    (void)delta_time;
    AISCRIPT_SET_PACKET_FLAG(packet, params, count, first_time, 0x20000u);
    return 1;
}

i32 __cdecl Action_SetFullPathSearch(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                     i32 count, i32 first_time, f32 delta_time) {
    (void)sys;
    (void)processor;
    (void)delta_time;
    AISCRIPT_SET_OWNER_FLAG(packet, params, count, first_time, flags_1fc_word, 0x8u, 0, 1);
    return 1;
}

i32 __cdecl Action_SetViewDistance(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 count, i32 first_time, f32 elapsed) {
    APIOBJECT_PC *object;

    if (packet != NULL && packet->owner != NULL && first_time != 0) {
        object = (APIOBJECT_PC *)packet->owner;
        if (packet->creature_index != 0xff)
            object->view_distance = system->creatures[packet->creature_index].view_distance;
        else if (GetViewRangeFn != NULL)
            object->view_distance = GetViewRangeFn(object->character_model->model_id);
        else
            object->view_distance = 1.0f;
        if (count != 0 && NuStrICmp(params[0], GizmoDefaultOutputName) != 0)
            object->view_distance = AIParamToFloatEx(packet, processor, params[0]);
    }
    return 1;
}

i32 __cdecl Action_SetMinViewHeight(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 count, i32 first_time, f32 elapsed) {
    APIOBJECT_PC *object;

    if (packet != NULL && packet->owner != NULL && first_time != 0) {
        object = (APIOBJECT_PC *)packet->owner;
        if (packet->creature_index != 0xff)
            object->min_view_height = system->creatures[packet->creature_index].min_view_height;
        else if (GetMinViewHeightFn != NULL)
            object->min_view_height = GetMinViewHeightFn(object->character_model->model_id);
        else
            object->min_view_height = 1.0f;
        if (count != 0 && NuStrICmp(params[0], GizmoDefaultOutputName) != 0)
            object->min_view_height = AIParamToFloatEx(packet, processor, params[0]);
    }
    return 1;
}

i32 __cdecl Action_SetMaxViewHeight(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 count, i32 first_time, f32 elapsed) {
    APIOBJECT_PC *object;

    if (packet != NULL && packet->owner != NULL && first_time != 0) {
        object = (APIOBJECT_PC *)packet->owner;
        if (packet->creature_index != 0xff)
            object->max_view_height = system->creatures[packet->creature_index].max_view_height;
        else if (GetMaxViewHeightFn != NULL)
            object->max_view_height = GetMaxViewHeightFn(object->character_model->model_id);
        else
            object->max_view_height = 1.0f;
        if (count != 0 && NuStrICmp(params[0], GizmoDefaultOutputName) != 0)
            object->max_view_height = AIParamToFloatEx(packet, processor, params[0]);
    }
    return 1;
}

i32 __cdecl Action_SetHearDistance(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                   i32 count, i32 first_time, f32 elapsed) {
    APIOBJECT_PC *object;

    if (packet != NULL && packet->owner != NULL && first_time != 0) {
        object = (APIOBJECT_PC *)packet->owner;
        if (packet->creature_index != 0xff)
            object->hear_distance = system->creatures[packet->creature_index].hear_distance;
        else if (GetHearDistanceFn != NULL)
            object->hear_distance = GetHearDistanceFn(object->character_model->model_id);
        else
            object->hear_distance = 1.0f;
        if (count != 0 && NuStrICmp(params[0], GizmoDefaultOutputName) != 0)
            object->hear_distance = AIParamToFloatEx(packet, processor, params[0]);
    }
    return 1;
}

i32 __cdecl Action_SetMoveRadius(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {

    (void)sys;
    (void)processor;
    (void)packet;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    if (packet != NULL && packet->owner != NULL && param_5 != 0) {
        packet->mover_height =
            ((APIOBJECT_PC *)packet->owner)->collision_radius + ((APIOBJECT_PC *)packet->owner)->collision_radius;
        if (param_4 != 0 && NuStrICmp(params[0], GizmoDefaultOutputName) != 0) {
            packet->mover_height = AIParamToFloatEx(packet, processor, params[0]);
        }
    }
    return 1;
}

i32 __cdecl Action_OverrideAnimation(AISYS_PC *system, AISCRIPTPROCESS_PC *process, AIPACKET_PC *packet, char **params,
                                     i32 count, i32 first_time, f32 delta_time) {
    i32 from = -1;
    i32 to = -1;
    i32 index;
    char *value;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL || first_time == 0)
        return 1;
    for (index = 0; index < count; ++index) {
        if (NuStrICmp(params[index], AIOverrideAllKeywordPc) == 0) {
            from = (u16)apicharsys->model_id_capacity;
            continue;
        }
        value = NuStrIStr(params[index], (char *)"from=");
        if (value != NULL) {
            from = (u16)FindAnimIX(packet->owner->character_data, value + 5);
            continue;
        }
        value = NuStrIStr(params[index], (char *)"to=");
        if (value != NULL) {
            to = (u16)FindAnimIX(packet->owner->character_data, value + 3);
            continue;
        }
        if (process != NULL)
            process->action_timer = AIParamToFloatEx(packet, process, params[0]);
    }
    if ((i16)to == -1)
        from = -1;
    packet->animation_override_from = (i16)from;
    packet->animation_override_to = (i16)to;
    return 1;
}

i32 __cdecl Action_ResetToOrigin(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 param_4, i32 param_5, f32 param_6) {
    NUVEC *origin;

    (void)processor;
    (void)params;
    (void)param_4;
    (void)param_5;
    (void)param_6;
    if (packet == NULL || packet->owner == NULL) {
        return 1;
    }
    origin = GetOriginPositionFn != NULL ? GetOriginPositionFn(sys, packet) : NULL;
    if (origin != NULL) {
        packet->owner->position = *origin;
    } else if (packet->owner != NULL && (packet->owner->flags_1f8 & 0x400) != 0 && packet->creature_index != 0xff) {
        packet->owner->position = sys->creatures[packet->creature_index].position;
    }
    return 1;
}

i32 __cdecl Action_SetIgnoreAntinodes(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    APIOBJECT_PC *object;
    i32 ignore;
    i32 i;
    char *value;

    if (first_time != 0) {
        object = packet != NULL ? (APIOBJECT_PC *)(packet->owner) : NULL;
        ignore = 1;
        for (i = 0; i < param_count; i++) {
            value = NuStrIStr(params[i], (char *)AreaConfigKeyword_character);
            if (value != NULL) {
                if (GetNamedGameObjectCallback != NULL) {
                    object = GetNamedGameObjectCallback(sys, value + 10);
                }
            } else if (NuStrICmp(AIActionFalseName, params[0]) == 0) {
                // The original tests the first parameter, even on later iterations.
                ignore = 0;
            }
        }
        if (object != NULL) {
            object->flags_low = (object->flags_low & ~0x20000u) | (ignore != 0 ? 0x20000u : 0);
        }
    }
    return 1;
}

i32 __cdecl Action_NoShadows(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 count, i32 first_time, f32 delta_time) {
    (void)sys;
    (void)processor;
    (void)delta_time;
    AISCRIPT_SET_OWNER_FLAG(packet, params, count, first_time, flags_1f8, 0x2000u, 1, 0);
    return 1;
}

i32 __cdecl Action_SetParam(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 count, i32 first_time, f32 elapsed) {
    AICREATURE_PC *creature;
    i32 i;
    i32 index;
    u64 flag;
    char *value;
    f32 result;
    f64 previous;
    if (packet != NULL && processor != NULL && processor->script != NULL) {
        creature = packet->creature_index == 0xff ? NULL : &system->creatures[packet->creature_index];
        for (i = 0; i < count - 1; i++) {
            for (index = 0; index < 4; index++) {
                if (NuStrICmp(params[i], processor->script->params[index].name) == 0)
                    break;
            }
            if (index < 4) {
                flag = (u64)2 << index;
                i++;
                if (NuStrICmp(params[i], GizmoDefaultOutputName) == 0) {
                    if (creature != NULL && ((u64)(i64)creature->flags & flag) != 0)
                        processor->params[index] = creature->script_params[index];
                    else
                        processor->params[index] = processor->script->params[index].default_value;
                } else if ((value = NuStrIStr(params[i], (char *)AIActionIncrementName)) != NULL) {
                    result =
                        AIParamToFloatEx(packet, &packet->script_processor, value + NuStrLen(AIActionIncrementName));
                    processor->params[index] += result;
                } else if ((value = NuStrIStr(params[i], (char *)AIActionDecrementName)) != NULL) {
                    previous = processor->params[index];
                    result =
                        AIParamToFloatEx(packet, &packet->script_processor, value + NuStrLen(AIActionDecrementName));
                    processor->params[index] = (f32)(previous - result);
                } else
                    processor->params[index] = AIParamToFloatEx(packet, &packet->script_processor, params[i]);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetReturnToState(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 count, i32 first_time, f32 elapsed) {
    AISTATE_PC *state;
    i32 i;
    char *value;
    if (first_time != 0 && processor != NULL) {
        state = processor->state;
        for (i = 0; i < count; i++) {
            value = NuStrIStr(params[i], (char *)AIActionStateName);
            if (value != NULL) {
                state = AIStateFind(value + 6, processor->script);
                if (state == NULL) {
                    if (AIActionDiagnosticMode == 0)
                        AIScriptMissingStateDiagnostic(params[0], processor->script->name, processor->state->name);
                    else
                        AIScriptMissingStateDiagnosticAlternate(params[0], processor->script->name,
                                                                processor->state->name);
                }
            }
        }
        processor->return_to_state = state;
    }
    return 1;
}

i32 __cdecl Action_ReturnToState(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                 i32 count, i32 first_time, f32 elapsed) {
    if (processor != NULL && processor->return_to_state != NULL) {
        processor->next_state = processor->return_to_state;
        processor->return_to_state = NULL;
    }
    return 1;
}

void __cdecl AiSysUsePackFile(i32 enabled) {
    ai_usepackfile = enabled;
}

void __cdecl AiSysOnlyUsePakFile(i32 enabled) {
    ai_onlyusepackfile = enabled;
}

void __cdecl AiSysSetStateDebugee(AISCRIPTPROCESS_PC *processor) {
    AiSysStateDebugee = processor;
}

void __cdecl InitFn_LevelCharacterTypeID(AI_LEVEL_CHARACTER_TYPE_ID_PC function) {
    LevelCharacterTypeIDFn = function;
    SpecialRouteCharacterTypeIDFn = function;
}

void __cdecl InitFn_SpecialRouteCharacterTypeID(AI_LEVEL_CHARACTER_TYPE_ID_PC function) {
    SpecialRouteCharacterTypeIDFn = function;
}

void __cdecl InitFn_LevelCharacterName(AI_CHARACTER_NAME_PC function) {
    LevelCharacterNameFn = function;
    SpecialRouteCharacterNameFn = function;
}

void __cdecl InitFn_SpecialRouteCharacterName(AI_CHARACTER_NAME_PC function) {
    SpecialRouteCharacterNameFn = function;
}

void __cdecl InitFn_LevelCharacterGlobalID(AI_LEVEL_CHARACTER_GLOBAL_ID_PC function) {
    LevelCharacterGlobalIDFn = function;
}

void __cdecl InitFn_GlobalCharacterTypeID(i32(__cdecl *function)(char *)) {
    GlobalCharacterTypeIDFn = function;
}

void __cdecl InitFn_GlobalCharacterName(AI_GLOBAL_CHARACTER_NAME_PC function) {
    GlobalCharacterNameFn = function;
}

void __cdecl InitFn_GlobalCharacterRender(AI_GLOBAL_CHARACTER_RENDER_PC function) {
    GlobalCharacterRenderFn = function;
}

void __cdecl InitFn_GlobalCharacterHGobj(AI_GLOBAL_CHARACTER_HGOBJ_PC function) {
    GlobalCharacterHGobjFn = function;
}

void __cdecl InitFn_ClearAICreatures(AI_CLEAR_CREATURES_PC function) {
    ClearAICreaturesFn = function;
}

void __cdecl InitFn_GetCharacterGoalSpeedFn(AI_CHARACTER_GOAL_SPEED_PC callback) {
    GetCharacterGoalSpeedFn = callback;
}

void InitFn_FindAlternativeSpecialObjectFn(AIFINDALTERNATIVESPECIALOBJECT *function) {
    AISysSpecialNodeCallback = (AISYS_SPECIAL_NODE_CALLBACK_PC)function;
}

void __cdecl InitFn_APIOBJECTFromObjIDFn(APIOBJECT_FROM_ID_PC callback) {
    APIOBJECTFromObjIDFn = callback;
}

void __cdecl InitFn_GameAILoad(AI_LOAD_PC function) {
    GameAILoadFn = function;
}

void __cdecl InitFn_GameParamToFloat(AI_GAME_PARAM_TO_FLOAT_PC callback) {
    GameParamToFloatFn = callback;
}

void __cdecl InitFn_ScriptProcessFirstTimeAction(AI_SCRIPT_FIRST_ACTION_PC callback) {
    ScriptProcessFirstTimeActionFn = callback;
}

void __cdecl InitFn_AIActionParseSpeed(AI_PARSE_SPEED_PC function) {
    AIActionParseSpeedFn = function;
}

void __cdecl InitFn_AIRespawnOnPath(AI_RESPAWN_ON_PATH_PC function) {
    AIRespawnOnPathFn = function;
}

void __cdecl InitFn_AIBigJumpToDestination(AI_BIG_JUMP_TO_DESTINATION_PC callback) {
    AIBigJumpToDestinationFn = callback;
}

void __cdecl InitFn_GetViewRange(AI_CHARACTER_DISTANCE_PC function) {
    GetViewRangeFn = function;
}

void __cdecl InitFn_GetHearDistance(AI_CHARACTER_DISTANCE_PC function) {
    GetHearDistanceFn = function;
}

void __cdecl InitFn_GlobalGetMaxViewHeight(AI_CHARACTER_DISTANCE_PC function) {
    GetMaxViewHeightFn = function;
}

void __cdecl InitFn_GlobalGetMinViewHeight(AI_CHARACTER_DISTANCE_PC function) {
    GetMinViewHeightFn = function;
}

i32 __cdecl AIScriptSetInterrupt(AISCRIPTPROCESS_PC *processor, u8 priority, u8 id, char *state_name, f32 time) {
    AISTATE_PC *state;
    if (processor->interrupt_priority > priority)
        return 0;
    state = AIStateFind(state_name, processor->script);
    if (state == NULL)
        return 0;
    processor->interrupt_priority = priority;
    processor->interrupt_id = id;
    processor->interrupt_state = state;
    processor->interrupt_timer = time;
    return 1;
}

void __cdecl AIScriptClearInterrupt(AISCRIPTPROCESS_PC *processor, char *state_name) {
    if (processor->interrupt_state != NULL) {
        if (NuStrCmp(state_name, processor->interrupt_state->name) == 0) {
            processor->interrupt_timer = 0.0f;
            processor->interrupt_priority = 0;
            processor->interrupt_id = 0;
            processor->interrupt_state = NULL;
        }
    }
}

__declspec(noinline) void *__cdecl AISysBufferAlloc(VARIPTR *cursor, VARIPTR *end, u32 size) {
    void *result = NULL;

    if (cursor != NULL && end != NULL) {
        if (cursor->addr + size < end->addr) {
            result = (void *)((cursor->addr + 15) & ~15u);
            cursor->addr = ((cursor->addr + 15) & ~15u) + size;
            memset(result, 0, size);
        } else {
            AIScriptMissingStateDiagnostic();
        }
    } else {
        AIScriptMissingStateDiagnostic();
    }
    return result;
}

i32 __cdecl AISysSetLevelPath(AISYS_PC *system, char *name) {
    AIPATH_PC *path = NULL;
    i32 index;

    if (system == NULL || system->path_sys == NULL) {
        return 0;
    }
    if (name == NULL) {
        path = system->path_sys->paths[0];
    } else {
        for (index = 0; index < system->path_sys->path_count; ++index) {
            if (NuStrICmp(system->path_sys->paths[index]->name, name) == 0) {
                path = system->path_sys->paths[index];
                break;
            }
        }
    }
    if (path == NULL || system->path_sys->active_path == path) {
        return 0;
    }
    system->path_sys->active_path = path;
    return 1;
}

void __cdecl AIPathCalcExtents(AIPATH_PC *path) {
    NUVEC delta;
    NUVEC corner;
    i32 index;
    path->bounds_min.x = FLT_MAX;
    path->bounds_min.y = FLT_MAX;
    path->bounds_min.z = FLT_MAX;
    path->bounds_max.x = -FLT_MAX;
    path->bounds_max.y = -FLT_MAX;
    path->bounds_max.z = -FLT_MAX;
    for (index = 0; index < path->node_count; ++index) {
        AIPATHNODE_PC *node = &path->nodes[index];
        corner.x = node->position.x - node->radius;
        corner.y = node->min_height;
        corner.z = node->position.z - node->radius;
        if (path->bounds_min.x > corner.x)
            path->bounds_min.x = corner.x;
        if (path->bounds_min.y > corner.y)
            path->bounds_min.y = corner.y;
        if (path->bounds_min.z > corner.z)
            path->bounds_min.z = corner.z;
        corner.x = node->radius + node->position.x;
        corner.y = node->max_height;
        corner.z = node->radius + node->position.z;
        if (path->bounds_max.x < corner.x)
            path->bounds_max.x = corner.x;
        if (path->bounds_max.y < corner.y)
            path->bounds_max.y = corner.y;
        if (path->bounds_max.z < corner.z)
            path->bounds_max.z = corner.z;
    }
    NuVecSub(&delta, &path->bounds_max, &path->bounds_min);
    NuVecScale(&delta, &delta, 0.5f);
    NuVecSub(&path->bounds_min, &path->bounds_min, &delta);
    NuVecAdd(&path->bounds_max, &path->bounds_max, &delta);
}

__declspec(noinline) i32 __cdecl AIPathCheckExtents(AIPATH_PC *path, NUVEC *position) {
    if (path->bounds_min.x > position->x || path->bounds_min.y > position->y || path->bounds_min.z > position->z ||
        position->x > path->bounds_max.x || position->y > path->bounds_max.y) {
        return 0;
    }
    return position->z <= path->bounds_max.z;
}

static AIPATHSYS_PC *AISysLoadPathsPc(VARIPTR *cursor, i32 version, NUGSCN *scene, VARIPTR *end) {
    i32 count = EdFileReadInt();
    AI_PATH_SYSTEM_LOAD_PC *system = NULL;
    i32 path_index;
    i32 index;
    i32 route_index;
    i32 row;
    i32 length;
    i32 character_count;
    char special_name[256];
    if (count != 0) {
        PATH_LOAD_ALLOCATE(AI_PATH_SYSTEM_LOAD_PC *, system, cursor, end, sizeof(*system));
        if (system != NULL) {
            system->path_count = (u8)count;
            system->paths = (AIPATH_PC **)AISysBufferAlloc(cursor, end, system->path_count * sizeof(AIPATH_PC *));
            for (path_index = 0; path_index < system->path_count; ++path_index) {
                AIPATH_PC *path;
                PATH_LOAD_ALLOCATE(AIPATH_PC *, system->paths[path_index], cursor, end, sizeof(AIPATH_PC));
                path = system->paths[path_index];
                EdFileRead(path->name, 16);
                path->node_count = (u8)EdFileReadChar();
                path->flags = (u8)EdFileReadChar();
                path->index = (u8)path_index;
                path->connection_count = (u16)(version == 1 ? (i16)(i8)EdFileReadChar() : EdFileReadShort());
                if (path->connection_count != 0) {
                    PATH_LOAD_ALLOCATE(AIPATHCNX_PC *, path->connections, cursor, end,
                                       path->connection_count * sizeof(AIPATHCNX_PC));
                    for (index = 0; index < path->connection_count; ++index) {
                        AIPATHCNX_PC *connection = &path->connections[index];
                        connection->node_indices[0] = (u8)EdFileReadChar();
                        connection->node_indices[1] = (u8)EdFileReadChar();
                        if (version > 11) {
                            connection->traversal_flags[0] = EdFileReadInt();
                            connection->traversal_flags[1] = EdFileReadInt();
                        } else if (version > 8) {
                            connection->traversal_flags[0] = (i16)EdFileReadShort();
                            connection->traversal_flags[1] = (i16)EdFileReadShort();
                        } else {
                            connection->traversal_flags[0] = (i8)EdFileReadChar();
                            connection->traversal_flags[1] = (i8)EdFileReadChar();
                        }
                        connection->original_traversal_flags[0] = connection->traversal_flags[0];
                        connection->original_traversal_flags[1] = connection->traversal_flags[1];
                        connection->rotation = EdFileReadShort();
                        connection->route_mask = (u16)EdFileReadShort();
                        connection->distance = EdFileReadFloat();
                        connection->horizontal_distance = EdFileReadFloat();
                        if (connection->horizontal_distance == 0.0f)
                            connection->horizontal_distance = 0.00001f;
                    }
                }
                if (version == 1)
                    EdFileReadChar();
                if (path->node_count != 0) {
                    PATH_LOAD_ALLOCATE(AIPATHNODE_PC *, path->nodes, cursor, end,
                                       path->node_count * sizeof(AIPATHNODE_PC));
                    for (index = 0; index < path->node_count; ++index) {
                        AIPATHNODE_PC *node = &path->nodes[index];
                        length = EdFileReadInt();
                        if (length != 0) {
                            PATH_LOAD_ALLOCATE(char *, node->name, cursor, end, length + 1);
                            EdFileRead(node->name, length);
                        }
                        node->position.x = EdFileReadFloat();
                        node->position.y = EdFileReadFloat();
                        node->position.z = EdFileReadFloat();
                        node->radius = EdFileReadFloat();
                        node->radius_squared = node->radius * node->radius;
                        if (version > 7) {
                            node->min_height = EdFileReadFloat();
                            node->max_height = EdFileReadFloat();
                        } else {
                            node->min_height = node->position.y - default_path_heighttol;
                            node->max_height = node->position.y + default_path_heighttol;
                        }
                        node->min_height_offset = node->min_height - node->position.y;
                        node->max_height_offset = node->max_height - node->position.y;
                        node->connection_count = (u8)EdFileReadChar();
                        node->flags = (u8)EdFileReadChar();
                        EdFileReadChar();
                        node->runtime_flags = (u8)EdFileReadChar() & 0xf9;
                        node->path_flags = EdFileReadShort();
                        if (version >= 19)
                            node->special_route_index = (u8)EdFileReadChar();
                        else {
                            EdFileReadChar();
                            node->special_route_index = 0xff;
                        }
                        length = (i8)EdFileReadChar();
                        if (length != 0) {
                            EdFileRead(special_name, length);
                            node->has_special = (u8)(NuSpecialFind(scene, &node->special_handle, special_name, 1) != 0);
                            node->special_position.x = EdFileReadFloat();
                            node->special_position.y = EdFileReadFloat();
                            node->special_position.z = EdFileReadFloat();
                        }
                        if (path->connections != NULL && node->connection_count != 0) {
                            PATH_LOAD_ALLOCATE(AIPATHCNX_PC **, node->connections, cursor, end,
                                               node->connection_count * sizeof(AIPATHCNX_PC *));
                            for (row = 0; row < node->connection_count; ++row)
                                node->connections[row] = &path->connections[(i16)EdFileReadShort()];
                            if ((node->connection_count & 1) != 0)
                                EdFileReadShort();
                        }
                        if (version >= 5) {
                            node->route_membership_mask = (u16)EdFileReadShort();
                            node->route_boundary_mask = (u16)EdFileReadShort();
                        }
                        node->distance_cache_nodes[0] = 0xff;
                        node->distance_cache_nodes[1] = 0xff;
                    }
                    AIPathCalcExtents(path);
                }
                PATH_LOAD_ALLOCATE(u8 **, path->route_matrix, cursor, end, path->node_count * sizeof(u8 *));
                for (index = 0; index < path->node_count; ++index) {
                    PATH_LOAD_ALLOCATE(u8 *, path->route_matrix[index], cursor, end, path->node_count);
                    EdFileRead(path->route_matrix[index], path->node_count);
                }
                if (version >= 5) {
                    path->route_count = (u8)EdFileReadChar();
                    if (path->route_count != 0) {
                        PATH_LOAD_ALLOCATE(AIPATHROUTE_PC *, path->routes, cursor, end,
                                           path->route_count * sizeof(AIPATHROUTE_PC));
                        for (route_index = 0; route_index < path->route_count; ++route_index) {
                            AIPATHROUTE_PC *route = &path->routes[route_index];
                            length = (i8)EdFileReadChar();
                            if (length != 0) {
                                PATH_LOAD_ALLOCATE(char *, route->name, cursor, end, length);
                                EdFileRead(route->name, length);
                                route->route_count = (u8)EdFileReadChar();
                                route->exit_node_count = (u8)EdFileReadChar();
                                EdFileReadChar();
                                EdFileReadChar();
                                if (path->node_count != 0 && route->route_count != 0) {
                                    PATH_LOAD_ALLOCATE(u8 *, route->node_routes, cursor, end, path->node_count);
                                    EdFileRead(route->node_routes, path->node_count);
                                    PATH_LOAD_ALLOCATE(u8 *, route->node_directions, cursor, end, route->route_count);
                                    EdFileRead(route->node_directions, route->route_count);
                                    PATH_LOAD_ALLOCATE(u8 **, route->route_nodes, cursor, end,
                                                       route->route_count * sizeof(u8 *));
                                    for (row = 0; row < route->route_count; ++row) {
                                        PATH_LOAD_ALLOCATE(u8 *, route->route_nodes[row], cursor, end,
                                                           route->route_count);
                                        EdFileRead(route->route_nodes[row], route->route_count);
                                    }
                                    if (route->exit_node_count != 0) {
                                        PATH_LOAD_ALLOCATE(u8 *, route->exit_nodes, cursor, end,
                                                           route->exit_node_count);
                                        EdFileRead(route->exit_nodes, route->exit_node_count);
                                    }
                                }
                            }
                            character_count = (i8)EdFileReadChar();
                            for (row = 0; row < character_count; ++row) {
                                length = (i8)EdFileReadChar();
                                EdFileRead(special_name, length);
                                if (SpecialRouteCharacterTypeIDFn != NULL) {
                                    u8 type = (u8)SpecialRouteCharacterTypeIDFn(special_name);
                                    if (type < 64) {
                                        u64 mask = (u64)1 << type;
                                        route->character_mask[0] |= (u32)mask;
                                        route->character_mask[1] |= (u32)(mask >> 32);
                                        route->character_mask[2] |= (u32)mask;
                                        route->character_mask[3] |= (u32)(mask >> 32);
                                    } else {
                                        route->character_mask[0] = 0xffffffff;
                                        route->character_mask[1] = 0xffffffff;
                                        route->character_mask[3] |= 0x80000000;
                                    }
                                }
                            }
                        }
                    }
                }
                if (version >= 19) {
                    path->special_route_count = (u8)EdFileReadChar();
                    if (path->special_route_count != 0) {
                        AIPATHNODELINK_PC *link;
                        PATH_LOAD_ALLOCATE(AIPATHNODELINK_PC *, path->special_routes, cursor, end,
                                           path->special_route_count * sizeof(AIPATHNODELINK_PC));
                        link = path->special_routes;
                        for (index = 0; index < path->special_route_count; ++index, ++link) {
                            link->node_index = EdFileReadUnsignedChar();
                            link->special_route_index = EdFileReadShort();
                        }
                    }
                }
            }
            if (version >= 19) {
                system->special_route_count = (u16)EdFileReadShort();
                if (system->special_route_count != 0) {
                    AIPATHSPECIALROUTE_PC *route;
                    system->special_routes = (AIPATHSPECIALROUTE_PC *)AISysBufferAlloc(
                        cursor, end, system->special_route_count * sizeof(AIPATHSPECIALROUTE_PC));
                    route = system->special_routes;
                    for (index = 0; index < system->special_route_count; ++index, ++route) {
                        route->path_count = (u8)EdFileReadChar();
                        if (route->path_count != 0) {
                            PATH_LOAD_ALLOCATE(AIPATH_PC **, route->paths, cursor, end,
                                               route->path_count * sizeof(AIPATH_PC *));
                            for (row = 0; row < route->path_count; ++row)
                                route->paths[row] = system->paths[(i8)EdFileReadChar()];
                        }
                    }
                }
            }
        }
    }
    return (AIPATHSYS_PC *)system;
}

/* Native 0x00517fd0, 105 bytes. */
AIGROUP_PC *__cdecl CreateAIGroup(AISYS_PC *system, i32 count_across, f32 x_spacing, f32 z_spacing, f32 max_speed) {
    AIGROUP_PC *group;
    i32 index;
    if (system == NULL || count_across == 0)
        return NULL;
    for (index = 0; index < 16; ++index) {
        group = &system->groups[index];
        if ((group->flags & 1) == 0) {
            group->flags |= 1;
            group->flags &= ~8u;
            group->x_spacing = x_spacing;
            group->z_spacing = z_spacing;
            group->count_across = (u8)count_across;
            group->max_speed = max_speed;
            return group;
        }
    }
    return NULL;
}

void __cdecl AddToAIGroup(AIGROUP_PC *group, APIOBJECT_PC *object) {
    u8 member_index;
    i32 count_across;
    i32 row_count;
    if (object == NULL || group == NULL || group->member_count >= 16) {
        return;
    }
    member_index = group->member_count;
    count_across = group->count_across;
    row_count = (i32)(AIPathSafeDivide((f32)(member_index + 1), (f32)(count_across)));
    if ((member_index + 1) % group->count_across != 0)
        ++row_count;
    if (row_count > 4) {
        return;
    }
    if (member_index == 0) {
        group->leader = object;
        if (group->x_spacing > 0.0f) {
            group->radius = ((f32)(count_across)-1.0f) * group->x_spacing * 0.5f;
            group->rotation_speed =
                (i32)(AIPathAngularScale *
                      AIPathSafeDivide(group->max_speed, (f32)(group->radius * NuTime_RefreshRate)));
        } else {
            group->radius = 0.0f;
            group->rotation_speed = 100;
        }
    }
    group->row_count = (u8)(row_count);
    object->ai->group = group;
    object->ai->group_member_index = group->member_count;
    object->ai->group_row =
        (i32)(AIPathSafeDivide((f32)((i32)(group->member_count)), (f32)((i32)(group->count_across))));
    object->ai->group_column = group->member_count % group->count_across;
    group->members[group->member_count] = object;
    ++group->member_count;
}

void __cdecl DestroyAIGroup(AIGROUP_PC *group) {
    if (group != NULL)
        memset(group, 0, sizeof(*group));
}

void __cdecl AIScriptSetLevelPath(char *path) {
    AiLevelPathName = path;
}

void __cdecl AISysProcess(AISYS_PC *system, APIOBJECT_PC *player_1, APIOBJECT_PC *player_2) {
    i32 index;
    AILOCATOR_PC *locator;
    NUVEC forward = {0.0f, 0.0f, 1.0f};
    if (system == NULL) {
        return;
    }
    system->player_1 = player_1;
    system->player_2 = player_2;
    for (index = 0; index < system->antinode_count + 64; ++index) {
        i32 antinode_count = system->antinode_count;
        AIANTINODE_PC *antinode =
            index < antinode_count ? &system->antinodes[index] : &dynamic_antinodes[index - antinode_count];
        NUMTX *draw_matrix;
        if (antinode->has_special == 0) {
            continue;
        }
        if (AISysSpecialNodeCallback != NULL && NuSpecialGetVisibilityFn(&antinode->special_handle) == 0) {
            AISysSpecialNodeCallback(system, &antinode->special_handle);
        }
        if (NuSpecialGetVisibilityFn(&antinode->special_handle) == 0) {
            continue;
        }
        draw_matrix = NuSpecialGetDrawMtx(&antinode->special_handle);
        NuVecMtxTransform(&antinode->position, &antinode->special_position, draw_matrix);
        antinode->min_y = antinode->min_y_offset + antinode->position.y;
        antinode->max_y = antinode->max_y_offset + antinode->position.y;
        if (antinode->type != 0) {
            NUVEC rotated;
            NuVecMtxRotate(&rotated, &forward, draw_matrix);
            antinode->rotation = NuAtan2D(rotated.x, rotated.z);
            antinode->rotation = NuAngAdd(antinode->rotation, antinode->rotation_offset);
        }
    }
    locator = system->locators;
    for (index = 0; index < system->locator_count; ++index, ++locator) {
        AIPATH_PC *path = locator->path_info.path;
        AIPATHCNX_PC *connection = locator->path_info.connection;
        if ((path->nodes[connection->node_indices[0]].runtime_flags & 0x02) != 0 ||
            (path->nodes[connection->node_indices[1]].runtime_flags & 0x02) != 0) {
            AIPATHNODE_PC *start = &path->nodes[connection->node_indices[0]];
            AIPATHNODE_PC *end = &path->nodes[connection->node_indices[1]];
            NUVEC delta;
            NUVEC direction;
            NUVEC offset;
            f32 radius;
            f32 direction_x;
            i32 path_angle;
            NuVecSub(&delta, &end->position, &start->position);
            NuVecNorm(&direction, &delta);
            if (locator->path_info.distance > 1.0f) {
                radius = end->radius;
            } else if (locator->path_info.distance < 0.0f) {
                radius = start->radius;
            } else {
                radius =
                    (1.0f - locator->path_info.distance) * start->radius + end->radius * locator->path_info.distance;
            }
            direction_x = direction.x;
            direction.x = direction.z * radius;
            direction.z = -direction_x * radius;
            locator->position = start->position;
            NuVecScale(&offset, &delta, locator->path_info.distance);
            NuVecAdd(&locator->position, &locator->position, &offset);
            NuVecScale(&offset, &direction, locator->path_info.width);
            NuVecAdd(&locator->position, &locator->position, &offset);
            path_angle = (i32)(NuAtan2(delta.x, delta.z) * AIPathAngularScale);
            locator->rotation = NuAngAdd(path_angle, locator->rotation_offset);
        }
    }
    for (index = 0; index < 16; ++index) {
        AIGROUP_PC *group = &system->groups[index];
        if ((group->flags & 1) != 0) {
            if (group->member_is_alive != 0)
                group->flags &= ~2u;
            else
                group->flags |= 2;
        }
    }
}

AIANTINODE_PC *__cdecl AIAntinodeCreate(NUVEC *position, f32 radius) {
    i32 index;
    for (index = 0; index < 64; ++index) {
        AIANTINODE_PC *antinode = &dynamic_antinodes[index];
        if (antinode->enabled == 0) {
            memset(antinode, 0, sizeof(*antinode));
            antinode->enabled = 1;
            antinode->game_flags |= 1;
            antinode->position = *position;
            antinode->radius = radius;
            antinode->min_y = antinode->position.y - default_path_heighttol;
            antinode->max_y = antinode->position.y + default_path_heighttol;
            return antinode;
        }
    }
    return NULL;
}

AIANTINODE_PC *__cdecl AIAntinodeCreateSingleFrame(NUVEC *position, f32 radius) {
    i32 index;
    for (index = 0; index < 64; ++index) {
        AIANTINODE_PC *antinode = &dynamic_antinodes[index];
        if (antinode->enabled == 0) {
            memset(antinode, 0, sizeof(*antinode));
            antinode->enabled = 1;
            antinode->game_flags |= 5;
            antinode->position = *position;
            antinode->radius = radius;
            antinode->min_y = antinode->position.y - default_path_heighttol;
            antinode->max_y = antinode->position.y + default_path_heighttol;
            return antinode;
        }
    }
    return NULL;
}

void __cdecl AIAntinodeDestroy(AIANTINODE_PC *antinode) {
    if ((antinode->game_flags & 1) != 0) {
        memset(antinode, 0, sizeof(*antinode));
    }
}

void __cdecl AIAntinodeCullSingleFrame(void) {
    i32 index;
    for (index = 0; index < 64; ++index) {
        AIANTINODE_PC *antinode = &dynamic_antinodes[index];
        if (antinode->enabled != 0 && (antinode->game_flags & 4) != 0) {
            memset(antinode, 0, sizeof(*antinode));
        }
    }
}

void __cdecl AIAntinodeMove(AIANTINODE_PC *antinode, NUVEC *position, f32 radius, f32 below, f32 above) {
    if (antinode != NULL) {
        antinode->position = *position;
        antinode->radius = radius;
        antinode->min_y = antinode->position.y - below;
        antinode->max_y = antinode->position.y + above;
    }
}

static void AISysCheckAntinode_Circle(APIOBJECT_PC *object, AIANTINODE_PC *antinode, NUVEC *difference, f32 radius) {
    i32 angle;
    i32 turn;
    f32 turn_fraction;
    i32 max_turn;
    f32 squared_distance;
    squared_distance = difference->x * difference->x + difference->z * difference->z;
    if (squared_distance < radius * radius) {
        difference->x = object->position.x - antinode->position.x;
        difference->z = object->position.z - antinode->position.z;
        angle = NuAtan2D(difference->x, difference->z);
        difference->x = object->ai->movement_destination.x - antinode->position.x;
        difference->z = object->ai->movement_destination.z - antinode->position.z;
        turn = NuAngSub(NuAtan2D(difference->x, difference->z), angle);
        if (object->ai->antinode_timer > 0.0f) {
            if (((object->ai->path_info.flags & 3) == 2 || INTERACTION_GEOMETRY(object)->respawn_timer > 1.0f) &&
                object->ai->antinode_timer < antinode_time) {
                object->ai->flags ^= 0x8000;
                object->ai->antinode_timer = antinode_time + antinode_reverse_time;
            }
            if ((object->ai->flags & 0x8000)) {
                if (turn < 0) {
                    turn += 0x10000;
                }
            } else if (turn > 0) {
                turn -= 0x10000;
            }
        } else {
            if (turn > 0) {
                object->ai->flags |= 0x8000;
            } else {
                object->ai->flags &= ~0x8000u;
            }
        }
        if ((antinode->game_flags & 2) == 0 && object->ai->antinode_timer < antinode_time) {
            object->ai->antinode_timer = antinode_time;
        }
        turn_fraction = ai_moveradius / radius;
        max_turn = (i32)((f64)turn_fraction * AIPathAngularScale);
        if (turn > max_turn) {
            turn = max_turn;
        } else if (turn < -max_turn) {
            turn = -max_turn;
        }
        angle = NuAngAdd(angle, turn);
        difference->x = 0.0f;
        difference->y = 0.0f;
        difference->z = radius;
        NuVecRotateY(difference, difference, angle);
        object->ai->movement_position.x = difference->x + antinode->position.x;
        object->ai->movement_position.y = object->ai->movement_destination.y;
        object->ai->movement_position.z = difference->z + antinode->position.z;
        object->flags_low |= 0x40000;
    }
}

static void AISysCheckAntinode_Ellipse(APIOBJECT_PC *object, AIANTINODE_PC *antinode, NUVEC *difference, f32 radius) {
    f32 distance_squared;
    f32 aspect;
    NUVEC local_position;
    NUVEC boundary;
    i32 angle;
    NUVEC local_object;
    NUVEC object_boundary;
    NUVEC local_destination;
    i32 object_angle;
    i32 turn;
    f32 tangent_x;
    f32 tangent_z;
    i32 absolute_turn;
    f32 scale;
    distance_squared = difference->x * difference->x + difference->z * difference->z;
    if (distance_squared < radius * radius) {
        aspect = INTERACTION_ANTINODE_SHAPE(antinode)->base_height / INTERACTION_ANTINODE_SHAPE(antinode)->base_radius;

        difference->y = 0.0f;
        NuVecRotateY(&local_position, difference, -antinode->rotation);
        angle = NuAtan2D(aspect * local_position.x, local_position.z);
        boundary.x = INTERACTION_SIN(angle) * INTERACTION_ANTINODE_SHAPE(antinode)->base_radius;
        boundary.y = 0.0f;
        boundary.z = INTERACTION_COS(angle) * INTERACTION_ANTINODE_SHAPE(antinode)->base_height;
        NuVecNorm(difference, &boundary);
        NuVecScale(difference, difference, object->ai->mover_height);
        NuVecAdd(&boundary, &boundary, difference);
        if (distance_squared < boundary.x * boundary.x + boundary.z * boundary.z) {

            difference->x = object->position.x - antinode->position.x;
            difference->y = 0.0f;
            difference->z = object->position.z - antinode->position.z;
            NuVecRotateY(&local_object, difference, -antinode->rotation);
            object_angle = NuAtan2D(aspect * local_object.x, local_object.z);
            object_boundary.x = INTERACTION_SIN(object_angle) * INTERACTION_ANTINODE_SHAPE(antinode)->base_radius;
            object_boundary.y = 0.0f;
            object_boundary.z = INTERACTION_COS(object_angle) * INTERACTION_ANTINODE_SHAPE(antinode)->base_height;
            NuVecNorm(difference, &object_boundary);
            NuVecScale(difference, difference, object->ai->mover_height);
            NuVecAdd(&object_boundary, &object_boundary, difference);
            difference->x = object->ai->movement_destination.x - antinode->position.x;
            difference->y = 0.0f;
            difference->z = object->ai->movement_destination.z - antinode->position.z;
            NuVecRotateY(&local_destination, difference, -antinode->rotation);
            turn = NuAngSub(NuAtan2D(aspect * local_destination.x, local_destination.z), object_angle);
            if (object->ai->antinode_timer > 0.0f) {
                if (((object->ai->path_info.flags & 3) == 2 || INTERACTION_GEOMETRY(object)->respawn_timer > 1.0f) &&
                    object->ai->antinode_timer < antinode_time) {
                    object->ai->flags ^= 0x8000;
                    object->ai->antinode_timer = antinode_time + antinode_reverse_time;
                }
                if ((object->ai->flags & 0x8000)) {
                    if (turn < 0) {
                        turn += 0x10000;
                    }
                } else if (turn > 0) {
                    turn -= 0x10000;
                }
            } else if (turn > 0) {
                object->ai->flags |= 0x8000;
            } else {
                object->ai->flags &= ~0x8000u;
            }
            if ((antinode->game_flags & 2) == 0 && object->ai->antinode_timer < antinode_time) {
                object->ai->antinode_timer = antinode_time;
            }
            tangent_x = INTERACTION_COS(object_angle) * INTERACTION_ANTINODE_SHAPE(antinode)->base_radius;
            tangent_z = -INTERACTION_SIN(object_angle) * INTERACTION_ANTINODE_SHAPE(antinode)->base_height;
            if (turn < 0) {
                tangent_x = -tangent_x;
                tangent_z = -tangent_z;
            }
            absolute_turn = turn < 0 ? -turn : turn;
            if (absolute_turn < 1820) {
                scale = (f32)(absolute_turn) / 1820.0f;
                tangent_x *= scale;
                tangent_z *= scale;
            }
            object->ai->movement_position.y = 0.0f;
            object->ai->movement_position.x = object_boundary.x + tangent_x;
            object->ai->movement_position.z = object_boundary.z + tangent_z;
            NuVecRotateY(&object->ai->movement_position, &object->ai->movement_position, antinode->rotation);
            NuVecAdd(&object->ai->movement_position, &object->ai->movement_position, &antinode->position);
            object->flags_low |= 0x40000;
        }
    }
}

static void AISysCheckAntinode_Rectangle(APIOBJECT_PC *object, AIANTINODE_PC *antinode, NUVEC *difference, f32 radius) {
    NUVEC local_object;
    NUVEC local_destination;
    i32 object_angle;
    i32 turn;
    i32 turn_offset;
    i32 corner_angle;
    i32 angle;
    f32 tangent_x;
    f32 tangent_z;
    i32 absolute_turn;
    f32 scale;
    difference->x = object->ai->movement_position.x - antinode->position.x;
    if (difference->x > radius || difference->x < -radius) {
        return;
    }
    difference->z = object->ai->movement_position.z - antinode->position.z;
    if (difference->z > radius || difference->z < -radius ||
        INTERACTION_GEOMETRY(object)->collision_min.y > antinode->max_y ||
        antinode->min_y > INTERACTION_GEOMETRY(object)->collision_max.y) {
        return;
    }
    NuVecRotateY(difference, difference, -antinode->rotation);
    if (AIPathFabs(difference->x) < INTERACTION_ANTINODE_SHAPE(antinode)->base_radius + object->ai->mover_height &&
        AIPathFabs(difference->z) < INTERACTION_ANTINODE_SHAPE(antinode)->base_height + object->ai->mover_height) {

        difference->x = object->position.x - antinode->position.x;
        difference->y = 0.0f;
        difference->z = object->position.z - antinode->position.z;
        NuVecRotateY(&local_object, difference, -antinode->rotation);
        object_angle = NuAtan2D(local_object.x, local_object.z);
        difference->x = object->ai->movement_destination.x - antinode->position.x;
        difference->y = 0.0f;
        difference->z = object->ai->movement_destination.z - antinode->position.z;
        NuVecRotateY(&local_destination, difference, -antinode->rotation);
        turn = NuAngSub(NuAtan2D(local_destination.x, local_destination.z), object_angle);
        turn_offset = 0;
        if (object->ai->antinode_timer > 0.0f) {
            if (((object->ai->path_info.flags & 3) == 2 || INTERACTION_GEOMETRY(object)->respawn_timer > 1.0f) &&
                object->ai->antinode_timer < antinode_time) {
                object->ai->flags ^= 0x8000;
                object->ai->antinode_timer = antinode_time + antinode_reverse_time;
            }
            if ((object->ai->flags & 0x8000)) {
                if (turn < 0) {
                    turn_offset = 0x10000;
                }
            } else if (turn > 0) {
                turn_offset = -0x10000;
            }
        } else if (turn > 0) {
            object->ai->flags |= 0x8000;
        } else {
            object->ai->flags &= ~0x8000u;
        }
        if ((antinode->game_flags & 2) == 0 && object->ai->antinode_timer < antinode_time) {
            object->ai->antinode_timer = antinode_time;
        }
        corner_angle = NuAtan2D(INTERACTION_ANTINODE_SHAPE(antinode)->base_radius,
                                INTERACTION_ANTINODE_SHAPE(antinode)->base_height);
        angle = NuAngAdd(object_angle, 0);

        if (angle > 0x8000 - corner_angle) {
            object->ai->movement_position.x =
                (INTERACTION_ANTINODE_SHAPE(antinode)->base_radius + object->ai->mover_height) * INTERACTION_SIN(angle);
            object->ai->movement_position.z =
                -(object->ai->mover_height + INTERACTION_ANTINODE_SHAPE(antinode)->base_height);
            tangent_x = -1.0f;
            tangent_z = 0.0f;
        } else if (angle > corner_angle) {
            object->ai->movement_position.x =
                INTERACTION_ANTINODE_SHAPE(antinode)->base_radius + object->ai->mover_height;
            object->ai->movement_position.z =
                (object->ai->mover_height + INTERACTION_ANTINODE_SHAPE(antinode)->base_height) * INTERACTION_COS(angle);
            tangent_x = 0.0f;
            tangent_z = -1.0f;
        } else if (angle > -corner_angle) {
            object->ai->movement_position.x =
                (INTERACTION_ANTINODE_SHAPE(antinode)->base_radius + object->ai->mover_height) * INTERACTION_SIN(angle);
            object->ai->movement_position.z =
                object->ai->mover_height + INTERACTION_ANTINODE_SHAPE(antinode)->base_height;
            tangent_x = 1.0f;
            tangent_z = 0.0f;
        } else if (angle > corner_angle - 0x8000) {
            object->ai->movement_position.x =
                -(INTERACTION_ANTINODE_SHAPE(antinode)->base_radius + object->ai->mover_height);
            object->ai->movement_position.z =
                (object->ai->mover_height + INTERACTION_ANTINODE_SHAPE(antinode)->base_height) * INTERACTION_COS(angle);
            tangent_x = 0.0f;
            tangent_z = 1.0f;
        } else {
            object->ai->movement_position.x =
                (INTERACTION_ANTINODE_SHAPE(antinode)->base_radius + object->ai->mover_height) * INTERACTION_SIN(angle);
            object->ai->movement_position.z =
                -(object->ai->mover_height + INTERACTION_ANTINODE_SHAPE(antinode)->base_height);
            tangent_x = -1.0f;
            tangent_z = 0.0f;
        }
        if (turn + turn_offset < 0) {
            tangent_x = -tangent_x;
            tangent_z = -tangent_z;
        }
        absolute_turn = turn < 0 ? -turn : turn;
        if (absolute_turn < 1820) {
            scale = (f32)(absolute_turn) / 1820.0f;
            tangent_x *= scale;
            tangent_z *= scale;
        }
        object->ai->movement_position.y = 0.0f;
        object->ai->movement_position.x += tangent_x;
        object->ai->movement_position.z += tangent_z;
        NuVecRotateY(&object->ai->movement_position, &object->ai->movement_position, antinode->rotation);
        NuVecAdd(&object->ai->movement_position, &object->ai->movement_position, &antinode->position);
        object->flags_low |= 0x40000;
    }
}

/* Prospective src/gameapi/aisys.c additions. Existing map row at0x5191f0
 * must be renamed to this genuine private helper; Android-named wrapper is0x519680.
 * No compile or compiler-ABI claim. */
static void AISysDrawBoxPc(NUVEC *minimum, NUVEC *maximum, NUMTX *matrix, u32 colour) {
    NURND_VERTEX3D_PC corners[8];
    i32 i;
    corners[0].position.x = minimum->x;
    corners[0].position.y = minimum->y;
    corners[0].position.z = minimum->z;
    corners[1].position.x = maximum->x;
    corners[1].position.y = minimum->y;
    corners[1].position.z = minimum->z;
    corners[2].position.x = maximum->x;
    corners[2].position.y = minimum->y;
    corners[2].position.z = maximum->z;
    corners[3].position.x = minimum->x;
    corners[3].position.y = minimum->y;
    corners[3].position.z = maximum->z;
    corners[4].position.x = minimum->x;
    corners[4].position.y = maximum->y;
    corners[4].position.z = minimum->z;
    corners[5].position.x = maximum->x;
    corners[5].position.y = maximum->y;
    corners[5].position.z = minimum->z;
    corners[6].position.x = maximum->x;
    corners[6].position.y = maximum->y;
    corners[6].position.z = maximum->z;
    corners[7].position.x = minimum->x;
    corners[7].position.y = maximum->y;
    corners[7].position.z = maximum->z;
    for (i = 0; i < 8; ++i)
        NuVecMtxTransform(&corners[i].position, &corners[i].position, matrix);
    for (i = 0; i < 8; ++i)
        corners[i].colour = colour;
#define DRAW_BOX_EDGE(a, b)                                                                                            \
    AiRndrLine3dDbg(corners[a].position.x, corners[a].position.y, corners[a].position.z, corners[b].position.x,        \
                    corners[b].position.y, corners[b].position.z, colour)
    DRAW_BOX_EDGE(0, 1);
    DRAW_BOX_EDGE(1, 2);
    DRAW_BOX_EDGE(2, 3);
    DRAW_BOX_EDGE(3, 0);
    DRAW_BOX_EDGE(4, 5);
    DRAW_BOX_EDGE(5, 6);
    DRAW_BOX_EDGE(6, 7);
    DRAW_BOX_EDGE(7, 4);
    DRAW_BOX_EDGE(0, 4);
    DRAW_BOX_EDGE(1, 5);
    DRAW_BOX_EDGE(2, 6);
    DRAW_BOX_EDGE(3, 7);
#undef DRAW_BOX_EDGE
}

void __cdecl AISysDrawAntinode_Rectangle(AIANTINODE_PC *node, u32 colour) {
    NUMTX matrix;
    NUVEC minimum;
    NUVEC maximum;
    NuMtxSetTranslation(&matrix, &node->position);
    NuMtxPreRotateY(&matrix, node->rotation);
    minimum.x = -node->base_radius;
    minimum.y = node->min_y_offset;
    minimum.z = -node->base_height;
    maximum.x = node->base_radius;
    maximum.y = node->max_y_offset;
    maximum.z = node->base_height;
    AISysDrawBoxPc(&minimum, &maximum, &matrix, colour);
}

void __cdecl AISysCreatureAntinodeInteraction(AISYS_PC *system, i32 object_count, APIOBJECT_PC **objects, i32 *unused) {
    AIANTINODE_PC *active[192];
    i32 count, index, obstacle;
    AIANTINODE_PC *antinode;
    APIOBJECT_PC *object;
    NUVEC difference;
    f32 radius;
    if (system != NULL) {
        count = 0;
        antinode = system->antinodes;
        for (index = 0; index < system->antinode_count && count < 192; ++index, ++antinode) {
            if (antinode->enabled && (!antinode->has_special || NuSpecialGetVisibilityFn(&antinode->special_handle)))
                active[count++] = antinode;
        }
        antinode = dynamic_antinodes;
        for (index = 0; index < 64 && count < 192; ++index, ++antinode) {
            if (antinode->enabled && (!antinode->has_special || NuSpecialGetVisibilityFn(&antinode->special_handle)))
                active[count++] = antinode;
        }
        if (count != 0) {
            for (index = 0; index < object_count; ++index) {
                object = objects[index];
                {
                    u32 flags = object->flags_low;
                    if (flags & 0x20000)
                        continue;
                    if (flags & 0x80)
                        continue;
                }
                for (obstacle = 0; obstacle < count; ++obstacle) {
                    antinode = active[obstacle];
                    if (NuSpecialExistsFn(&object->collision_special) &&
                        NuSpecialCompare(&object->collision_special, &antinode->special_handle))
                        continue;
                    if (((u64)1 << object->object_id) &
                        ((INTERACTION_ANTINODE_VIEW_PC *)antinode)->excluded_character_types)
                        continue;
                    radius = object->ai->mover_height + antinode->radius;
                    difference.x = object->ai->movement_position.x - antinode->position.x;
                    if (difference.x > radius || difference.x < -radius)
                        continue;
                    difference.z = object->ai->movement_position.z - antinode->position.z;
                    if (difference.z > radius || difference.z < -radius ||
                        INTERACTION_GEOMETRY(object)->collision_min.y > antinode->max_y)
                        continue;
                    /* Native unordered min-height comparison continues to callback. */
                    if (!(antinode->min_y > INTERACTION_GEOMETRY(object)->collision_max.y))
                        checkantinodefns[antinode->type](object, antinode, &difference, radius);
                }
            }
        }
    }
    AIAntinodeCullSingleFrame();
}

void __cdecl AISysCreatureInteraction2D(AISYS_PC *system, i32 object_count, APIOBJECT_PC **objects, i32 *immovable,
                                        f32 delta_time) {
    i32 index, first_index, second_index, first_fixed, second_fixed;
    APIOBJECT_PC *first, *second;
    AIPACKET_PC *first_packet, *second_packet;
    NUVEC difference;
    f32 radius, distance;

    for (index = 0; index < object_count; ++index) {
        first = objects[index];
        first->ai->antinode_timer -= delta_time;
        if (first->ai->antinode_timer < 0.0f)
            first->ai->antinode_timer = 0.0f;
    }
    for (first_index = 0; first_index < object_count - 1; ++first_index) {
        first = objects[first_index];
        for (second_index = first_index + 1; second_index < object_count; ++second_index) {
            second = objects[second_index];
            radius = first->ai->mover_height + second->ai->mover_height;
            if (first->collision_link == second || second->collision_link == first) {
                first_fixed = second_fixed = 1;
            } else {
                first_fixed = immovable[first_index];
                second_fixed = immovable[second_index];
            }
            if (first->flags_low & 0x80)
                first_fixed = 1;
            if (second->flags_low & 0x80)
                second_fixed = 1;
            if (first_fixed && second_fixed)
                continue;
            difference.x = first->ai->movement_position.x - second->ai->movement_position.x;
            if (difference.x > radius || difference.x < -radius)
                continue;
            difference.z = first->ai->movement_position.z - second->ai->movement_position.z;
            if (difference.z > radius || difference.z < -radius)
                continue;
            if (((GAMEOBJECT *)first)->collision_min.y > ((GAMEOBJECT *)second)->collision_max.y ||
                ((GAMEOBJECT *)second)->collision_min.y > ((GAMEOBJECT *)first)->collision_max.y)
                continue;
            distance = NuFsqrt(difference.x * difference.x + difference.z * difference.z);
            if (!(distance < radius))
                continue;
            difference.y = 0.0f;
            radius -= distance;
            if (first_fixed) {
                NuVecScale(&difference, &difference, radius / distance);
                NuVecSub(&second->ai->movement_position, &second->ai->movement_position, &difference);
            } else if (second_fixed) {
                NuVecScale(&difference, &difference, radius / distance);
                NuVecAdd(&first->ai->movement_position, &first->ai->movement_position, &difference);
            } else {
                NuVecScale(&difference, &difference, radius / distance);
                NuVecSub(&second->ai->movement_position, &second->ai->movement_position, &difference);
            }
            first_packet = first->ai;
            second_packet = second->ai;
            if (first_packet->antinode_timer > antinode_time && second_packet->antinode_timer > antinode_time) {
                if (first_packet->antinode_timer > second_packet->antinode_timer) {
                    second_packet->antinode_timer = first_packet->antinode_timer;
                    second->ai->flags ^= (first->ai->flags ^ second->ai->flags) & 0x8000;
                } else {
                    first_packet->antinode_timer = second_packet->antinode_timer;
                    first->ai->flags ^= (second->ai->flags ^ first->ai->flags) & 0x8000;
                }
            } else if (second_packet->antinode_timer > antinode_time) {
                first_packet->antinode_timer = second_packet->antinode_timer;
                first->ai->flags ^= (second->ai->flags ^ first->ai->flags) & 0x8000;
            } else if (first_packet->antinode_timer > 0.0f) {
                second_packet->antinode_timer = first_packet->antinode_timer;
                second->ai->flags ^= (first->ai->flags ^ second->ai->flags) & 0x8000;
            }
            if ((first->motion_flags ^ second->motion_flags) & 1) {
                first->ai->flags |= 0x4000;
                second->ai->flags |= 0x4000;
            }
        }
    }
    AISysCreatureAntinodeInteraction(system, object_count, objects, immovable);
}

void __cdecl LEGO_AISysCreatureInteraction2D(AISYS_PC *system, i32 object_count, APIOBJECT_PC **objects, i32 *immovable,
                                             f32 delta_time) {
    i32 index, first_index, second_index, first_fixed, second_fixed;
    APIOBJECT_PC *first, *second;
    AIPACKET_PC *first_packet, *second_packet;
    NUVEC difference;
    f32 radius, distance;

    for (index = 0; index < object_count; ++index) {
        first = objects[index];
        first->ai->antinode_timer -= delta_time;
        if (first->ai->antinode_timer < 0.0f)
            first->ai->antinode_timer = 0.0f;
    }
    for (first_index = 0; first_index < object_count - 1; ++first_index) {
        first = objects[first_index];
        for (second_index = first_index + 1; second_index < object_count; ++second_index) {
            second = objects[second_index];
            radius = first->ai->mover_height + second->ai->mover_height;
            difference.x = first->ai->movement_position.x - second->ai->movement_position.x;
            if (difference.x > radius || difference.x < -radius)
                continue;
            difference.z = first->ai->movement_position.z - second->ai->movement_position.z;
            if (difference.z > radius || difference.z < -radius)
                continue;
            if (((GAMEOBJECT *)first)->collision_min.y > ((GAMEOBJECT *)second)->collision_max.y ||
                ((GAMEOBJECT *)second)->collision_min.y > ((GAMEOBJECT *)first)->collision_max.y)
                continue;
            if (first->collision_link == second || second->collision_link == first)
                continue;
            if (immovable != NULL) {
                first_fixed = immovable[first_index];
                second_fixed = immovable[second_index];
            } else {
                first_fixed = 0;
                second_fixed = 0;
            }
            if (first->ai->dont_avoid_character == (GAMEOBJECT *)second)
                first_fixed = 1;
            if (second->ai->dont_avoid_character == (GAMEOBJECT *)first)
                second_fixed = 1;
            if (first->flags_low & 0x80)
                first_fixed = 1;
            if (second->flags_low & 0x80)
                second_fixed = 1;
            if (second->ai_collision_priority_pc < first->ai_collision_priority_pc) {
                first_fixed = 1;
                if (second_fixed)
                    continue;
            } else if (second->ai_collision_priority_pc > first->ai_collision_priority_pc) {
                second_fixed = 1;
                if (first_fixed)
                    continue;
            } else {
                continue;
            }
            distance = NuFsqrt(difference.x * difference.x + difference.z * difference.z);
            if (!(distance < radius))
                continue;
            difference.y = 0.0f;
            radius -= distance;
            if (first_fixed) {
                if (first->ai_collision_priority_pc > second->ai_resolved_collision_priority_pc) {
                    NuVecScale(&difference, &difference, AIPathSafeDivide(radius, distance));
                    NuVecSub(&second->ai->movement_position, &second->ai->movement_position, &difference);
                    second->ai_resolved_collision_priority_pc = first->ai_collision_priority_pc;
                }
            } else if (second_fixed) {
                if (second->ai_collision_priority_pc > first->ai_resolved_collision_priority_pc) {
                    NuVecScale(&difference, &difference, AIPathSafeDivide(radius, distance));
                    NuVecAdd(&first->ai->movement_position, &first->ai->movement_position, &difference);
                    first->ai_resolved_collision_priority_pc = second->ai_collision_priority_pc;
                }
            } else {
                AIScriptMissingStateDiagnostic();
                NuVecScale(&difference, &difference, AIPathSafeDivide(radius, distance));
                NuVecSub(&second->ai->movement_position, &second->ai->movement_position, &difference);
            }
            first_packet = first->ai;
            second_packet = second->ai;
            if (first_packet->antinode_timer > antinode_time && second_packet->antinode_timer > antinode_time) {
                if (first_packet->antinode_timer > second_packet->antinode_timer) {
                    second_packet->antinode_timer = first_packet->antinode_timer;
                    second->ai->flags ^= (first->ai->flags ^ second->ai->flags) & 0x8000;
                } else {
                    first_packet->antinode_timer = second_packet->antinode_timer;
                    first->ai->flags ^= (second->ai->flags ^ first->ai->flags) & 0x8000;
                }
            } else if (second_packet->antinode_timer > antinode_time) {
                first_packet->antinode_timer = second_packet->antinode_timer;
                first->ai->flags ^= (second->ai->flags ^ first->ai->flags) & 0x8000;
            } else if (first_packet->antinode_timer > 0.0f) {
                second_packet->antinode_timer = first_packet->antinode_timer;
                second->ai->flags ^= (first->ai->flags ^ second->ai->flags) & 0x8000;
            }
            if ((first->motion_flags ^ second->motion_flags) & 1) {
                first->ai->flags |= 0x4000;
                second->ai->flags |= 0x4000;
            }
        }
    }
    AISysCreatureAntinodeInteraction(system, object_count, objects, immovable);
}

void __cdecl AISysCreatureInteraction3D(AISYS_PC *system, i32 object_count, APIOBJECT_PC **objects, i32 *immovable,
                                        f32 delta_time) {
    i32 index, first_index, second_index, first_fixed, second_fixed;
    APIOBJECT_PC *first, *second;
    NUVEC difference;
    f32 radius, distance;

    for (index = 0; index < object_count; ++index) {
        first = objects[index];
        first->ai->antinode_timer -= delta_time;
        if (first->ai->antinode_timer < 0.0f)
            first->ai->antinode_timer = 0.0f;
    }
    for (first_index = 0; first_index < object_count - 1; ++first_index) {
        first = objects[first_index];
        for (second_index = first_index + 1; second_index < object_count; ++second_index) {
            second = objects[second_index];
            radius = first->ai->mover_height + second->ai->mover_height;
            if (first->collision_link == second || second->collision_link == first) {
                first_fixed = second_fixed = 1;
            } else {
                first_fixed = immovable[first_index];
                second_fixed = immovable[second_index];
            }
            if (first->flags_low & 0x80)
                first_fixed = 1;
            if (second->flags_low & 0x80)
                second_fixed = 1;
            if (first_fixed && second_fixed)
                continue;
            difference.x = first->ai->movement_position.x - second->ai->movement_position.x;
            if (difference.x > radius || difference.x < -radius)
                continue;
            difference.z = first->ai->movement_position.z - second->ai->movement_position.z;
            if (difference.z > radius || difference.z < -radius)
                continue;
            difference.y = first->ai->movement_position.y - second->ai->movement_position.y;
            if (difference.y > radius || difference.y < -radius)
                continue;
            distance = NuFsqrt(difference.x * difference.x + difference.y * difference.y + difference.z * difference.z);
            if (!(distance < radius))
                continue;
            radius -= distance;
            if (first_fixed) {
                NuVecScale(&difference, &difference, radius / distance);
                NuVecSub(&second->ai->movement_position, &second->ai->movement_position, &difference);
            } else if (second_fixed) {
                NuVecScale(&difference, &difference, radius / distance);
                NuVecAdd(&first->ai->movement_position, &first->ai->movement_position, &difference);
            } else {
                NuVecScale(&difference, &difference, radius / distance);
                NuVecSub(&second->ai->movement_position, &second->ai->movement_position, &difference);
            }
            if ((first->motion_flags ^ second->motion_flags) & 1) {
                first->ai->flags |= 0x4000;
                second->ai->flags |= 0x4000;
            }
        }
    }
    AISysCreatureAntinodeInteraction(system, object_count, objects, immovable);
}

static void AISysResetPathSearchConnectionChecks(AIPATH_PC *path) {
    AIPATHNODE_PC *node;
    i32 connection_index;
    if (path == NULL || path->nodes == NULL || path->node_count == 0) {
        return;
    }
    ++path->search_checksum;
    if (path->search_checksum == 0) {
        path->search_checksum = 1;
    }
    ++path->search_reset_node;
    if (path->search_reset_node >= path->node_count) {
        path->search_reset_node = 0;
    }
    node = &path->nodes[path->search_reset_node];
    for (connection_index = 0; connection_index < node->connection_count; ++connection_index) {
        node->connections[connection_index]->last_search_checksum = 0;
    }
}

void __cdecl AISysCharacterSetPath(AIPACKET_PC *packet, AIPATH_PC *path) {
    i32 route;

    if (packet->path_info.path == path) {
        return;
    }
    memset(&packet->path_info, 0, sizeof(packet->path_info));
    packet->path_info.path = path;
    packet->path_info.path_index = 0xff;
    packet->available_routes = 0;
    packet->unknown_124 = 0xffff;
    packet->current_route = 0xff;
    packet->next_route = 0;
    packet->current_path_node = NULL;
    if (path != NULL) {
        for (route = 0; route < path->route_count; route++) {
            if (((path->routes[route].character_mask[0] & packet->route_mask_low) |
                 (path->routes[route].character_mask[1] & packet->route_mask_high)) != 0)
                packet->available_routes |= (u16)((u64)1 << route);
        }
    }
}

f32 __cdecl AIPathNodeDistanceToPathNode(AIPATH_PC *path, i32 start_node, i32 destination_node, i32 route_index,
                                         u32 excluded_route_mask) {
    AIPATHNODE_PC *nodes;
    AIPATHNODE_PC *node;
    AIPATHNODE_PC *destination;
    AIPATHCNX_PC *previous_connection = NULL;
    AIPATHROUTE_PC *route = NULL;
    f32 *cached_distance = NULL;
    f32 distance = 0.0f;
    i32 node_index;

    if (path->route_matrix == NULL) {
        return FLT_MAX;
    }
    if (start_node == destination_node) {
        return 0.0f;
    }
    nodes = path->nodes;
    node = &nodes[start_node];
    destination = &nodes[destination_node];
    if (excluded_route_mask == 0 && route_index == 0xff) {
        if (node->distance_cache_nodes[0] == destination_node) {
            return node->distance_cache[0];
        }
        if (node->distance_cache_nodes[1] == destination_node) {
            return node->distance_cache[1];
        }
        node->distance_cache_nodes[1] = node->distance_cache_nodes[0];
        node->distance_cache_nodes[0] = (u8)destination_node;
        nodes = path->nodes;
        node->distance_cache[1] = node->distance_cache[0];
        cached_distance = &node->distance_cache[0];
        *cached_distance = 0.0f;
    }
    node_index = (i32)(node - nodes);
    if (route_index != 0xff) {
        route = &path->routes[route_index];
        if (((u64)node->route_membership_mask & ((u64)1 << route_index)) == 0) {
            return FLT_MAX;
        }
    }

    while (node != destination) {
        AIPATHCNX_PC *connection;
        i32 connection_index = 0xff;
        if (route != NULL) {
            if (((u64)destination->route_membership_mask & ((u64)1 << route_index)) != 0) {
                if (route->node_routes[node_index] < route->route_count &&
                    route->node_routes[destination_node] < route->route_count) {
                    connection_index =
                        route->route_nodes[route->node_routes[node_index]][route->node_routes[destination_node]];
                } else {
                    AIScriptMissingStateDiagnosticAlternate(route->name, node_index, destination_node);
                    return FLT_MAX;
                }
            } else {
                f32 nearest = FLT_MAX;
                i32 index;
                for (index = 0; index < route->exit_node_count; ++index) {
                    f32 first =
                        route->exit_nodes[index] == start_node
                            ? 0.0f
                            : AIPathNodeDistanceToPathNode(path, start_node, route->exit_nodes[index], route_index, 0);
                    f32 second = route->exit_nodes[index] == destination_node
                                     ? 0.0f
                                     : AIPathNodeDistanceToPathNode(path, route->exit_nodes[index], destination_node,
                                                                    0xff, (u32)((u64)1 << route_index));
                    f32 candidate;
                    if (first != FLT_MAX && second != FLT_MAX) {
                        candidate = second + first;
                    } else {
                        candidate = FLT_MAX;
                    }
                    if (candidate < nearest) {
                        nearest = candidate;
                    }
                }
                if (nearest != FLT_MAX) {
                    distance += nearest;
                    break;
                }
                if (cached_distance != NULL) {
                    *cached_distance = FLT_MAX;
                }
                return FLT_MAX;
            }
        } else {
            connection_index = path->route_matrix[node_index][destination_node];
        }
        if (connection_index == 0xff) {
            if (cached_distance != NULL) {
                *cached_distance = FLT_MAX;
            }
            return FLT_MAX;
        }
        connection = node->connections[connection_index];
        if (connection == NULL) {
            return FLT_MAX;
        }
        if ((connection->route_mask & excluded_route_mask) != 0) {
            if (cached_distance != NULL) {
                *cached_distance = FLT_MAX;
            }
            return FLT_MAX;
        }
        if (connection == previous_connection) {
            break;
        }
        previous_connection = connection;
        node_index =
            node_index == connection->node_indices[0] ? connection->node_indices[1] : connection->node_indices[0];
        distance += connection->distance;
        node = &nodes[node_index];
    }
    if (cached_distance != NULL) {
        *cached_distance = distance;
    }
    return distance;
}

AILOCATOR_PC *__cdecl AIPathFindLocator(AISYS_PC *system, char *name) {
    i32 index;
    if (system != NULL) {
        for (index = 0; index < system->locator_count; ++index) {
            if (NuStrICmp(system->locators[index].name, name) == 0) {
                return &system->locators[index];
            }
        }
    }
    return NULL;
}

AILOCATORSET_PC *__cdecl AIPathFindLocatorSet(AISYS_PC *system, char *name) {
    i32 index;
    if (system != NULL) {
        for (index = 0; index < system->locator_set_count; ++index) {
            if (NuStrICmp(system->locator_sets[index].name, name) == 0) {
                return &system->locator_sets[index];
            }
        }
    }
    return NULL;
}

AIAREA_PC *__cdecl AISysFindArea(AISYS_PC *system, char *name) {
    i32 index;
    if (system != NULL) {
        for (index = 0; index < system->area_count; ++index) {
            if (NuStrICmp(name, system->areas[index].name) == 0) {
                return &system->areas[index];
            }
        }
    }
    return NULL;
}

AIPATH_PC *__cdecl AISysFindPath(AISYS_PC *system, char *name) {
    i32 index;
    if (system != NULL && system->path_sys != NULL) {
        for (index = 0; index < system->path_sys->path_count; ++index) {
            AIPATH_PC *path = system->path_sys->paths[index];
            if (NuStrICmp(path->name, name) == 0) {
                return path;
            }
        }
    }
    return NULL;
}

__declspec(noinline) AIPATHNODE_PC *__cdecl AIPathFindNode(AISYS_PC *system, AIPATH_PC *path, char *name) {
    i32 index;
    if (path == NULL) {
        if (system == NULL || system->path_sys == NULL || system->path_sys->path_count == 0) {
            return NULL;
        }
        path = system->path_sys->active_path;
        if (path == NULL) {
            return NULL;
        }
    }
    for (index = 0; index < path->node_count; ++index) {
        if (path->nodes[index].name != NULL && NuStrICmp(path->nodes[index].name, name) == 0) {
            return &path->nodes[index];
        }
    }
    return NULL;
}

__declspec(noinline) void *__cdecl AIPAthFindPathCnx(AISYS_PC *system, AIPATH_PC *path, char *from_name, char *to_name,
                                                     i32 *direction) {
    AIPATHNODE_PC *from;
    AIPATHNODE_PC *to;
    i32 to_index;
    i32 index;
    if (path == NULL) {
        if (system == NULL || system->path_sys == NULL || system->path_sys->path_count == 0) {
            return NULL;
        }
        path = system->path_sys->active_path;
    }
    if (path == NULL) {
        return NULL;
    }
    from = AIPathFindNode(system, path, from_name);
    to = AIPathFindNode(system, path, to_name);
    if (to == NULL || from == NULL || from == to) {
        return NULL;
    }
    to_index = (i32)(to - path->nodes);
    for (index = 0; index < from->connection_count; ++index) {
        AIPATHCNX_PC *connection = from->connections[index];
        if (connection->node_indices[0] == to_index) {
            *direction = 1;
            return connection;
        }
        if (connection->node_indices[1] == to_index) {
            *direction = 0;
            return connection;
        }
    }
    return NULL;
}

AIPATHCNX_PC *__cdecl AIPathFindPathCnxFromIX(AISYS_PC *system, AIPATH_PC *path, u8 from_index, u8 to_index) {
    AIPATHNODE_PC *node;
    i32 index;
    if (path == NULL) {
        if (system == NULL || system->path_sys == NULL || system->path_sys->path_count == 0)
            return NULL;
        path = system->path_sys->active_path;
    }
    if (path != NULL && from_index < path->node_count) {
        node = &path->nodes[from_index];
        if (from_index != to_index) {
            for (index = 0; index < node->connection_count; ++index) {
                if (node->connections[index]->node_indices[0] == to_index ||
                    node->connections[index]->node_indices[1] == to_index)
                    return node->connections[index];
            }
        }
    }
    return NULL;
}

void __cdecl AIPathNodeUpdatePos(void *context, AIPATH_PC *path, AIPATHNODE_PC *node) {
    i32 node_index;
    i32 connection_index;
    AIPATHCNX_PC *connection;
    AIPATHNODE_PC *other;
    NUVEC direction;
    NUVEC previous_position;
    NUINSTANIM *animation;

    if (node->has_special == 0) {
        return;
    }
    node_index = (i32)(node - path->nodes);
    if ((path->updated_node_bits[node_index / 8] & (1 << (node_index % 8))) != 0) {
        return;
    }
    path->updated_node_bits[node_index / 8] |= 1 << (node_index % 8);

    if ((node->runtime_flags & 0x04) != 0) {
        if (NuSpecialGetVisibilityFn(&node->special_handle) == 0) {
            return;
        }
        node->runtime_flags &= (u8)~0x04;
        for (connection_index = 0; connection_index < node->connection_count; ++connection_index) {
            connection = node->connections[connection_index];
            if (connection->node_indices[0] == node_index)
                other = &path->nodes[connection->node_indices[1]];
            else
                other = &path->nodes[connection->node_indices[0]];
            if ((other->runtime_flags & 0x04) == 0) {
                connection->traversal_flags[0] &= 0xefffffffu;
                connection->traversal_flags[1] &= 0xefffffffu;
            }
        }
    } else if (NuSpecialGetVisibilityFn(&node->special_handle) == 0) {
        if (AISysSpecialNodeCallback == NULL || AISysSpecialNodeCallback(context, &node->special_handle) == 0) {
            node->runtime_flags |= 0x04;
            for (connection_index = 0; connection_index < node->connection_count; ++connection_index) {
                connection = node->connections[connection_index];
                connection->traversal_flags[0] |= 0x10000000u;
                connection->traversal_flags[1] |= 0x10000000u;
            }
            return;
        }
    }

    previous_position = node->position;
    NuVecMtxTransform(&node->position, &node->special_position, NuSpecialGetDrawMtx(&node->special_handle));
    node->min_height = node->position.y + node->min_height_offset;
    node->max_height = node->position.y + node->max_height_offset;

    for (connection_index = 0; connection_index < node->connection_count; ++connection_index) {
        connection = node->connections[connection_index];
        if (connection->node_indices[0] == node_index) {
            other = &path->nodes[connection->node_indices[1]];
            connection->horizontal_distance = NuVecXZDist(&other->position, &node->position, &direction);
        } else {
            other = &path->nodes[connection->node_indices[0]];
            connection->horizontal_distance = NuVecXZDist(&node->position, &other->position, &direction);
        }
        connection->rotation = (i16)(NuAtan2(direction.x, direction.z) * AIPathAngularScale);

        if (connection->max_horizontal_distance != 0.0f) {
            if (connection->horizontal_distance > node->radius + connection->max_horizontal_distance + other->radius) {
                connection->traversal_flags[0] |= 0x08000000u;
                connection->traversal_flags[1] |= 0x08000000u;
            } else {
                connection->traversal_flags[0] &= 0xf7ffffffu;
                connection->traversal_flags[1] &= 0xf7ffffffu;
            }
        }
    }

    if (previous_position.x != node->position.x || previous_position.y != node->position.y ||
        previous_position.z != node->position.z ||
        ((animation = (NUINSTANIM *)NuSpecialGetInstAnim(&node->special_handle)) != NULL &&
         (animation->flags & 1) != 0 && animation->tfactor != 0.0f)) {
        node->runtime_flags |= 0x02;
    } else {
        node->runtime_flags &= (u8)~0x02;
    }
}

__declspec(noinline) i32 __cdecl WithinConnection(void *context, NUVEC *position, AIPATH_PC *path,
                                                  AIPATHCNX_PC *connection, i32 reserved_check,
                                                  AIPATHCNX_PC *previous_connection, i32 route_index,
                                                  i32 reserved_ground, AIPATHINFO_PC *path_info, f32 radius,
                                                  i32 update_once) {
    AIPATHNODE_PC *first;
    AIPATHNODE_PC *second;
    f32 first_radius;
    f32 second_radius;
    i32 narrow;
    NUVEC delta;
    NUVEC local;
    f32 distance_squared;

    (void)reserved_ground;
    (void)reserved_check;

    if (update_once != 0) {
        if (connection->last_search_checksum == path->search_checksum) {
            return 0;
        }
        connection->last_search_checksum = path->search_checksum;
    }

    if (route_index != 0xff && previous_connection != NULL && (((i64)1 << route_index) & connection->route_mask) == 0) {
        AIPATHNODE_PC *shared;
        u8 shared_index;

        if (previous_connection->node_indices[0] == connection->node_indices[0] ||
            previous_connection->node_indices[0] == connection->node_indices[1]) {
            shared_index = previous_connection->node_indices[0];
        } else if (previous_connection->node_indices[1] == connection->node_indices[0] ||
                   previous_connection->node_indices[1] == connection->node_indices[1]) {
            shared_index = previous_connection->node_indices[1];
        } else {
            return 0;
        }
        shared = &path->nodes[shared_index];
        if (shared == NULL || (((i64)1 << route_index) & shared->route_boundary_mask) == 0) {
            return 0;
        }
    }

    first = &path->nodes[connection->node_indices[0]];
    second = &path->nodes[connection->node_indices[1]];
    narrow = 0;
    if (first->radius > radius + AIPathConnectionMargin) {
        first_radius = first->radius - radius;
    } else {
        narrow = 1;
        first_radius = first->radius;
    }
    if (second->radius > radius + AIPathConnectionMargin) {
        second_radius = second->radius - radius;
    } else {
        narrow = 1;
        second_radius = second->radius;
    }

    if (first->has_special != 0 &&
        (path->updated_node_bits[connection->node_indices[0] / 8] & (1 << (connection->node_indices[0] % 8))) == 0) {
        AIPathNodeUpdatePos(context, path, first);
    }
    if (second->has_special != 0 &&
        (path->updated_node_bits[connection->node_indices[1] / 8] & (1 << (connection->node_indices[1] % 8))) == 0) {
        AIPathNodeUpdatePos(context, path, second);
    }

    delta.x = position->x - first->position.x;
    delta.z = position->z - first->position.z;
    NuVecRotateY(&local, &delta, -connection->rotation);

    if (connection->horizontal_distance < AIPathFabs(second_radius - first_radius)) {
        if (second_radius > first_radius) {
            first = second;
            first_radius = second_radius;
        }
        if (position->y < first->min_height || position->y > first->max_height) {
            return 0;
        }
        delta.x = position->x - first->position.x;
        delta.z = position->z - first->position.z;
        distance_squared = delta.x * delta.x + delta.z * delta.z;
        if (!(first_radius * first_radius >= distance_squared)) {
            return 0;
        }
    } else {
        f32 abs_local_x = local.x < 0.0f ? -local.x : local.x;
        i32 intersection_angle;
        NUVEC wall;

        if (abs_local_x > first_radius && abs_local_x > second_radius) {
            return 0;
        }
        if (local.z < -first_radius || local.z > connection->horizontal_distance + second_radius) {
            return 0;
        }

        intersection_angle = PCPathArcSine((second_radius - first_radius) / connection->horizontal_distance);
        wall = local;
        if (wall.x < 0.0f) {
            wall.x = -wall.x;
        }
        NuVecRotateY(&wall, &wall, -intersection_angle);

        if (wall.z < 0.0f) {
            if (position->y < first->min_height || position->y > first->max_height) {
                return 0;
            }
            if (NuVecXZDistSqr(position, &first->position, &delta) <= first_radius * first_radius) {
                if (path_info != NULL) {
                    path_info->narrow_path = narrow;
                    path_info->on_path = 1;
                    path_info->direction = 0;
                    path_info->connection = connection;
                    path_info->distance = local.z / connection->horizontal_distance;
                    path_info->width = local.x;
                }
                return 1;
            }
            return 0;
        } else {
            f32 end_circle =
                NuTrigTable[((intersection_angle + 0x4000) >> 1) & 0x7fff] * connection->horizontal_distance;

            if (wall.z > end_circle) {
                if (position->y < second->min_height || position->y > second->max_height) {
                    return 0;
                }
                if (NuVecXZDistSqr(position, &second->position, &delta) <= second_radius * second_radius) {
                    if (path_info != NULL) {
                        path_info->narrow_path = narrow;
                        path_info->on_path = 1;
                        path_info->direction = 0;
                        path_info->connection = connection;
                        path_info->distance = local.z / connection->horizontal_distance;
                        path_info->width = local.x;
                    }
                    return 1;
                }
                return 0;
            } else {
                f32 minimum;
                f32 maximum;

                if (local.z < first_radius) {
                    minimum = first->min_height;
                    maximum = first->max_height;
                } else if (local.z > connection->horizontal_distance - second_radius) {
                    minimum = second->min_height;
                    maximum = second->max_height;
                } else {
                    f32 fraction =
                        (local.z - first_radius) / (connection->horizontal_distance - (first_radius + second_radius));
                    minimum = second->min_height * fraction + first->min_height * (1.0f - fraction);
                    maximum = second->max_height * fraction + first->max_height * (1.0f - fraction);
                }
                if (minimum > position->y || position->y > maximum || !(first_radius > wall.x)) {
                    return 0;
                }
            }
        }
    }

    if (path_info != NULL) {
        path_info->narrow_path = narrow;
        path_info->on_path = 1;
        path_info->direction = 0;
        path_info->connection = connection;
        path_info->distance = local.z / connection->horizontal_distance;
        path_info->width = local.x;
    }
    return 1;
}

void __cdecl AISysFindRoute(AIPACKET_PC *packet) {
    AIPACKET_PC *path_packet = packet;
    AIPATH_PC *path = (AIPATH_PC *)path_packet->path_info.path;
    AIPATHCNX_PC *connection = (AIPATHCNX_PC *)path_packet->path_info.connection;
    u8 route_count = path->route_count;
    u16 valid_routes;
    u8 route;
    u8 first_route;

    if (route_count == 0 || connection == NULL) {
        return;
    }

    valid_routes = (u16)(path_packet->available_routes & connection->route_mask);
    if (valid_routes == 0) {
        return;
    }

    route = path_packet->next_route;
    if (route >= route_count) {
        route = 0;
    }
    first_route = route;
    do {
        u64 route_mask = (u64)1 << route;
        if ((valid_routes & (u16)route_mask) != 0) {
            path_packet->current_route = route;
            path_packet->next_route = (u8)(route + 1);
            return;
        }
        ++route;
        if (route >= route_count) {
            route = 0;
        }
    } while (route != first_route);
}

__declspec(noinline) void __cdecl AISysCharacterSetPathCnx(AIPACKET_PC *packet, NUVEC *position,
                                                           AIPATHCNX_PC *connection, i32 direction) {
    AIPACKET_PC *path_packet = packet;
    AIPATH_PC *path;
    u32 connection_flags;

    if (connection == NULL || path_packet->owner == NULL) {
        path_packet->path_info.connection = connection;
        path_packet->current_route = 0xff;
        path_packet->next_route = 0;
        return;
    }

    if (path_packet->path_info.connection == connection && path_packet->path_info.direction == direction &&
        (path_packet->path_info.flags & (AIPATHINFO_FLAG_ON_PATH_LOCAL | AIPATHINFO_FLAG_ROUTE_CHECKED_LOCAL)) !=
            AIPATHINFO_FLAG_ON_PATH_LOCAL) {
        return;
    }

    connection_flags = connection->traversal_flags[direction];
    if (connection_flags != 0) {
        if ((connection_flags & AIPATH_CONNECTION_FLAG_RESELECT_ROUTE_LOCAL) != 0) {
            AISysFindRoute(packet);
            return;
        }
        if ((path_packet->capabilities & connection_flags) == 0) {
            return;
        }
    }

    if (path_packet->path_info.connection != connection || path_packet->path_info.direction != direction) {
        AIPATHNODE_PC *node;
        NUVEC delta;
        NUVEC rotated;

        path_packet->path_info.direction = (u8)direction;
        path_packet->path_update_flag = 0;
        path_packet->path_info.connection = connection;
        path = (AIPATH_PC *)path_packet->path_info.path;
        node = &path->nodes[connection->node_indices[0]];
        delta.x = position->x - node->position.x;
        delta.y = position->y - node->position.y;
        delta.z = position->z - node->position.z;
        NuVecRotateY(&rotated, &delta, -(NUANG)connection->rotation);
        path_packet->path_info.distance = rotated.z / connection->horizontal_distance;
        path_packet->path_info.width = rotated.x;
        path_packet->path_info.flags &= (u8)~AIPATHINFO_FLAG_ROUTE_CHECKED_LOCAL;
    }

    if ((path_packet->path_info.flags & (AIPATHINFO_FLAG_ON_PATH_LOCAL | AIPATHINFO_FLAG_ROUTE_CHECKED_LOCAL)) ==
        AIPATHINFO_FLAG_ON_PATH_LOCAL) {
        if (path_packet->current_route != 0xff) {
            u64 route_mask = (u64)1 << path_packet->current_route;
            if ((connection->route_mask & (u16)route_mask) == 0) {
                path_packet->current_route = 0xff;
            }
        }
        if (path_packet->current_route == 0xff) {
            AISysFindRoute(packet);
        }
        path_packet->path_info.flags |= AIPATHINFO_FLAG_ROUTE_CHECKED_LOCAL;
    }
}

void __cdecl AISysGetPathPosEx(AISYS_PC *system, NUVEC *position, AIPATHINFO_PC *info, AIPATH_PC *path, i32 checks,
                               i32 search_all_paths, f32 *nearest_distance_squared) {
    AIPATHCNX_PC *current;
    AIPATHNODE_PC *node;
    i32 index;
    info->flags_word &= 0xfffe;
    if (system == NULL || system->path_sys == NULL || system->path_sys->path_count == 0) {
        return;
    }
    if (search_all_paths != 0) {
        path = info->path;
        if (path == NULL) {
            path = system->path_sys->paths[0];
            info->connection = NULL;
            info->path = path;
            info->next_check = 0;
        }
    } else {
        if (path == NULL) {
            path = system->path_sys->active_path;
        }
        if (info->path != path) {
            info->connection = NULL;
            info->path = path;
            info->next_check = 0;
        }
    }
    AISysResetPathSearchConnectionChecks(path);
    current = info->connection;
    if (current != NULL) {
        if (WithinConnection(system, position, path, current, 1, NULL, 0xff, 0xff, info, 0.0f, 1)) {
            return;
        }
        node = &info->path->nodes[current->node_indices[info->direction]];
        for (index = 0; index < node->connection_count; ++index) {
            AIPATHCNX_PC *candidate = node->connections[index];
            if (candidate != current &&
                WithinConnection(system, position, path, candidate, 1, NULL, 0xff, 0xff, info, 0.0f, 1)) {
                return;
            }
        }
        node = &info->path->nodes[current->node_indices[info->direction == 0]];
        for (index = 0; index < node->connection_count; ++index) {
            AIPATHCNX_PC *candidate = node->connections[index];
            if (candidate != current &&
                WithinConnection(system, position, path, candidate, 1, NULL, 0xff, 0xff, info, 0.0f, 1)) {
                return;
            }
        }
    }
    if (search_all_paths == 0) {
        i32 remaining = info->path->node_count < checks ? info->path->node_count : checks;
        while (remaining != 0) {
            node = &info->path->nodes[info->next_check];
            for (index = 0; index < node->connection_count; ++index) {
                if (WithinConnection(system, position, path, node->connections[index], 1, NULL, 0xff, 0xff, info, 0.0f,
                                     1)) {
                    return;
                }
            }
            info->next_check = (info->next_check + 1) % info->path->node_count;
            --remaining;
        }
    } else {
        AIPATH_PC *saved_path = info->path;
        AIPATHCNX_PC *saved_connection = info->connection;
        u8 saved_direction = info->direction;
        u8 start_path;
        u16 start_node;
        i32 inside_extents = 0;
        if (info->path_index == 0xff) {
            info->path_index = saved_path->index;
        }
        if (info->path_index >= system->path_sys->path_count) {
            info->path_index = 0;
        }
        info->path = system->path_sys->paths[info->path_index];
        if (info->next_check >= info->path->node_count) {
            info->next_check = 0;
        }
        AISysResetPathSearchConnectionChecks(info->path);
        start_path = info->path_index;
        start_node = info->next_check;
        while (checks != 0) {
            if (inside_extents == 0) {
                inside_extents = AIPathCheckExtents(info->path, position);
            }
            if (inside_extents != 0) {
                node = &info->path->nodes[info->next_check];
                for (index = 0; index < node->connection_count; ++index) {
                    AIPATHCNX_PC *candidate = node->connections[index];
                    if (WithinConnection(system, position, info->path, candidate, 1, NULL, 0xff, 0xff, info, 0.0f, 1)) {
                        return;
                    }
                    if (nearest_distance_squared != NULL) {
                        AIPATHCNX_PC *connection = node->connections[index];
                        f32 distance = NuLineToPointDistSqrEx(&info->path->nodes[connection->node_indices[0]].position,
                                                              &info->path->nodes[connection->node_indices[1]].position,
                                                              position, NULL);
                        if (distance < *nearest_distance_squared) {
                            *nearest_distance_squared = distance;
                            saved_connection = candidate;
                            saved_path = info->path;
                        }
                    }
                }
                ++info->next_check;
            }
            if (inside_extents == 0 || info->next_check >= info->path->node_count) {
                info->next_check = 0;
                ++info->path_index;
                if (info->path_index >= system->path_sys->path_count) {
                    info->path_index = 0;
                }
                info->path = system->path_sys->paths[info->path_index];
                info->connection = NULL;
                AISysResetPathSearchConnectionChecks(info->path);
                inside_extents = 0;
            }
            --checks;
            if (info->path_index == start_path && info->next_check == start_node) {
                break;
            }
        }
        info->path = saved_path;
        info->connection = saved_connection;
        info->direction = saved_direction;
    }
}

static AIPATHCNX_PC *GetNextConnection(const AIPACKET_PC *packet, i32 *direction) {
    i32 node_index;
    AIPATHNODE_PC *node;
    u32 destination;
    AIPATHCNX_PC *connection = NULL;
    i32 connection_index;
    u8 route_index;
    AIPATHROUTE_PC *route;
    i32 next_node = 0;
    f32 nearest;
    i32 index;
    f32 first;
    f32 second;
    f32 distance;

    if (packet->goal_path_node == NULL) {
        return NULL;
    }
    if (packet->path_info.connection == NULL) {
        return NULL;
    }
    node_index = packet->path_info.connection->node_indices[packet->path_info.direction == 0];
    node = &packet->path_info.path->nodes[node_index];
    destination = packet->goal_path_node - packet->path_info.path->nodes;
    if (destination > 0xff) {
        return NULL;
    }
    connection_index = packet->path_info.path->route_matrix[node_index][destination];
    if (connection_index < node->connection_count) {
        connection = node->connections[connection_index];
        *direction = connection->node_indices[0] != node_index;
    }
    route_index = packet->current_route;
    if (route_index == 0xff) {
        return connection;
    }
    if ((((u64)1 << route_index) & node->route_membership_mask) == 0) {
        return NULL;
    }
    if ((((u64)1 << route_index) & node->route_boundary_mask) != 0) {
        if (connection == NULL) {
            return NULL;
        }
        if ((((u64)1 << route_index) & connection->route_mask) == 0) {
            return connection;
        }
    }
    route = &packet->path_info.path->routes[packet->current_route];
    connection = NULL;
    if ((((u64)1 << packet->current_route) & packet->path_info.path->nodes[destination].route_membership_mask) != 0) {
        connection_index = route->route_nodes[route->node_routes[node_index]][route->node_routes[destination]];
    } else {
        nearest = FLT_MAX;
        for (index = 0; index < route->exit_node_count; ++index) {
            first = AIPathNodeDistanceToPathNode(packet->path_info.path, node_index, route->exit_nodes[index],
                                                 packet->current_route, 0);
            second = AIPathNodeDistanceToPathNode(packet->path_info.path, route->exit_nodes[index], destination, 0xff,
                                                  (u32)((u64)(1) << packet->current_route));
            if (first != FLT_MAX && second != FLT_MAX) {
                distance = first + second;
            } else {
                distance = FLT_MAX;
            }
            if (distance < nearest) {
                nearest = distance;
                next_node = route->exit_nodes[index];
            }
        }
        if (nearest == FLT_MAX) {
            return NULL;
        }
        connection_index = route->route_nodes[route->node_routes[node_index]][route->node_routes[next_node]];
    }
    if (connection_index < node->connection_count) {
        connection = node->connections[connection_index];
        *direction = connection->node_indices[0] != node_index;
    }
    return connection;
}

static u32 CalculateRightIntersection(APIOBJECT_PC *object, AIPATHCNX_PC *first_connection,
                                      AIPATHCNX_PC *second_connection, i32 first_angle, i32 second_angle,
                                      AIPATHNODE_PC *before, AIPATHNODE_PC *shared, AIPATHNODE_PC *after,
                                      NUVEC *result) {
    f32 clearance;
    f32 before_radius;
    f32 shared_radius;
    f32 after_radius;
    NUVEC first_start;
    NUVEC first_end;
    i32 angle;
    NUVEC second_start;
    NUVEC second_end;
    NUVEC first_origin;
    NUVEC first_direction;
    NUVEC second_origin;
    NUVEC second_direction;
    f32 first_parameter;
    f32 second_parameter;

    clearance = object->collision_radius + AIPathConnectionMargin;
    before_radius = before->radius - clearance;
    if (before_radius < 0.0f) {
        before_radius = 0.0f;
    }
    shared_radius = shared->radius - clearance;
    if (shared_radius < 0.0f) {
        shared_radius = 0.0f;
    }
    after_radius = after->radius - clearance;
    if (after_radius < 0.0f) {
        after_radius = 0.0f;
    }
    first_start.x = before_radius;
    first_start.y = 0.0f;
    first_start.z = 0.0f;
    first_end.x = before_radius;
    first_end.y = 0.0f;
    first_end.z = first_connection->horizontal_distance;
    angle =
        NuAngAdd(first_angle, PCPathArcSine((shared_radius - before_radius) / first_connection->horizontal_distance));
    NuVecRotateY(&first_start, &first_start, angle);
    NuVecRotateY(&first_end, &first_end, angle);
    NuVecAdd(&first_start, &first_start, &before->position);
    NuVecAdd(&first_end, &first_end, &before->position);

    second_start.x = -after_radius;
    second_start.y = 0.0f;
    second_start.z = 0.0f;
    second_end.x = -after_radius;
    second_end.y = 0.0f;
    second_end.z = second_connection->horizontal_distance;
    angle =
        NuAngSub(second_angle, PCPathArcSine((shared_radius - after_radius) / second_connection->horizontal_distance));
    NuVecRotateY(&second_start, &second_start, angle);
    NuVecRotateY(&second_end, &second_end, angle);
    NuVecAdd(&second_start, &second_start, &after->position);
    NuVecAdd(&second_end, &second_end, &after->position);

    first_origin = first_start;

    NuVecSub(&first_direction, &first_end, &first_origin);
    second_origin = second_start;

    NuVecSub(&second_direction, &second_end, &second_origin);

    if (NuLineLineIntersect(&first_origin, &first_direction, &second_origin, &second_direction, &first_parameter,
                            &second_parameter) != 0 &&
        first_parameter >= 0.0f && second_parameter >= 0.0f && first_parameter <= 1.0f && second_parameter <= 1.0f) {
        first_origin.y = shared->position.y * first_parameter + before->position.y * (1.0f - first_parameter);
        NuVecScale(&first_direction, &first_direction, first_parameter);
        NuVecAdd(result, &first_origin, &first_direction);
        return 1;
    }
    return 0;
}

static u32 CalculateIntersection(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object,
                                 AIPATHCNX_PC *current_connection, AIPATHCNX_PC *target_connection) {
    AIPATH_PC *path;
    AIPATHNODE_PC *before;
    AIPATHNODE_PC *shared;
    AIPATHNODE_PC *after;
    i32 first_angle;
    i32 second_angle;
    u32 right;
    u32 left;

    if (packet->intersection_connection == current_connection &&
        packet->intersection_target_connection == target_connection) {
        return ((packet->flags & 0x00000020u) != 0) | ((packet->flags & 0x00000040u) != 0);
    }
    packet->flags &= ~(((0x20u | 0x40u)) << 0);
    packet->intersection_connection = current_connection;
    packet->intersection_target_connection = target_connection;

    path = packet->path_info.path;

    if (current_connection->node_indices[0] == target_connection->node_indices[0]) {
        before = &path->nodes[current_connection->node_indices[1]];
        shared = &path->nodes[current_connection->node_indices[0]];
        after = &path->nodes[target_connection->node_indices[1]];
        first_angle = NuAngAdd(current_connection->rotation, 0x8000);
        second_angle = NuAngAdd(target_connection->rotation, 0x8000);
    } else if (current_connection->node_indices[0] == target_connection->node_indices[1]) {
        before = &path->nodes[current_connection->node_indices[1]];
        shared = &path->nodes[current_connection->node_indices[0]];
        after = &path->nodes[target_connection->node_indices[0]];
        first_angle = NuAngAdd(current_connection->rotation, 0x8000);
        second_angle = target_connection->rotation;
    } else if (current_connection->node_indices[1] == target_connection->node_indices[0]) {
        before = &path->nodes[current_connection->node_indices[0]];
        shared = &path->nodes[current_connection->node_indices[1]];
        after = &path->nodes[target_connection->node_indices[1]];
        first_angle = current_connection->rotation;
        second_angle = NuAngAdd(target_connection->rotation, 0x8000);
    } else if (current_connection->node_indices[1] == target_connection->node_indices[1]) {
        before = &path->nodes[current_connection->node_indices[0]];
        shared = &path->nodes[current_connection->node_indices[1]];
        after = &path->nodes[target_connection->node_indices[0]];
        first_angle = current_connection->rotation;
        second_angle = target_connection->rotation;
    } else {
        return 0;
    }
    if (WithinConnection(system, &after->position, path, current_connection, 1, current_connection,
                         packet->current_route, object->object_id, NULL, 0.0f, 1) != 0) {
        return 0;
    }
    right = CalculateRightIntersection(object, current_connection, target_connection, first_angle, second_angle, before,
                                       shared, after, &packet->right_diversion);
    packet->flags = (packet->flags & ~0x20u) | ((right & 1) << 5);
    left = CalculateRightIntersection(object, target_connection, current_connection, second_angle, first_angle, after,
                                      shared, before, &packet->left_diversion);
    packet->flags = (packet->flags & ~0x40u) | ((left & 1) << 6);
    return ((packet->flags & 0x00000020u) != 0) | ((packet->flags & 0x00000040u) != 0);
}

static i32 RowMoveWander(AIGROUP_PC *group, AIROW_PC *row, AIROW_PC *previous, APIOBJECT_PC *leader) {
    i32 i;
    NUVEC offset;
    NUVEC rotated;
    NUVEC centre;
    i32 node_index;
    i32 first;
    u8 route;
    i32 direction;
    i32 current_angle;
    i32 next_angle;
    f32 distance;
    i32 angle;
    i32 difference;
    i32 step;
    AIPATHNODE_PC *node;
    AIPATHCNX_PC *next;
    AIPATHNODE_PC *other;
    AIPATHNODE_PC *nodes;
    APIOBJECT_PC *object;
    if (row->path_info.connection == NULL)
        return 0;
    node_index = row->path_info.connection->node_indices[row->path_info.direction == 0];
    node = &row->path_info.path->nodes[node_index];
    if (row->next_connection == NULL) {
        if (previous != NULL) {
            if (row->path_info.connection == previous->path_info.connection)
                row->next_connection = previous->next_connection;
            else
                row->next_connection = previous->path_info.connection;
        } else {
            first = NuRandInt() % node->connection_count;
            for (i = 0; i < node->connection_count; ++i) {
                row->next_connection = node->connections[(first + i) % node->connection_count];
                next = row->next_connection;
                if (next != row->path_info.connection) {
                    route = leader->ai->current_route;
                    if (route == 0xff || (((u64)1 << route) & next->route_mask) != 0 ||
                        (((u64)1 << route) & node->route_boundary_mask) != 0) {
                        direction = next->node_indices[0] != node_index;
                        other = &row->path_info.path->nodes[next->node_indices[direction == 0]];
                        if (other->radius > group->radius && next->traversal_flags[direction] == 0)
                            break;
                    }
                }
                row->next_connection = NULL;
            }
            if (row->next_connection == NULL)
                row->next_connection = row->path_info.connection;
        }
        next = row->next_connection;
        if (next != NULL && next->traversal_flags[next->node_indices[0] != node_index] == 0) {
            if (row->path_info.direction != 0)
                current_angle = NuAngAdd(0x8000, row->path_info.connection->rotation);
            else
                current_angle = row->path_info.connection->rotation;
            if (row->path_info.connection->node_indices[row->path_info.direction == 0] ==
                row->next_connection->node_indices[0]) {
                row->next_direction = 0;
                next_angle = row->next_connection->rotation;
            } else {
                row->next_direction = 1;
                next_angle = NuAngAdd(0x8000, row->next_connection->rotation);
            }
            if (NuAngSub(next_angle, current_angle) < 0)
                row->is_clockwise = 1;
            else
                row->is_clockwise = 0;
        }
        if (row->next_connection == NULL)
            return 0;
    }
    offset.y = 0.0f;
    offset.z = 0.0f;
    offset.x = row->is_clockwise ? -group->radius : group->radius;
    NuVecRotateY(&rotated, &offset, row->rotation);
    NuVecAdd(&centre, &rotated, &row->position);
    if (!(row->flags & 2u)) {
        if (row->path_info.connection == row->next_connection) {
            node = &row->path_info.path->nodes[row->path_info.connection->node_indices[row->path_info.direction == 0]];
            distance = NuVecXZDistSqr(&row->position, &node->position, &rotated);
            if (distance < node->radius_squared) {
                for (i = 0; i < group->row_count; ++i) {
                    group->rows[i].next_connection = NULL;
                    group->rows[i].rotation = NuAngAdd(group->rows[i].rotation, 0x8000);
                    group->rows[i].path_info.direction = group->rows[i].path_info.direction == 0;
                }
                group->leader = NULL;
                group->is_reversed = !group->is_reversed;
                if (group->is_reversed) {
                    for (i = group->member_count - 1; i >= 0; --i) {
                        object = group->members[i];
                        if (object != NULL && (object->flags_low & 1) && (object->flags_low & 0x1000) &&
                            object->leader_selection_state == 0) {
                            group->leader = group->members[i];
                            return 1;
                        }
                    }
                } else {
                    for (i = 0; i < group->member_count; ++i) {
                        object = group->members[i];
                        if (object->owner != NULL && (object->flags_low & 1) && (object->flags_low & 0x1000) &&
                            object->leader_selection_state == 0) {
                            group->leader = group->members[i];
                            return 1;
                        }
                    }
                }
                return 1;
            }
        } else {
            nodes = row->path_info.path->nodes;
            distance = NuInfiniteLineToPointDistSqr(&nodes[row->next_connection->node_indices[0]].position,
                                                    &nodes[row->next_connection->node_indices[1]].position, &centre);
            if (distance <= group->radius * group->radius)
                row->flags |= 2u;
        }
    }
    if ((row->flags & 2u)) {
        angle = row->next_connection->rotation;
        if (row->next_direction != 0)
            angle = NuAngAdd(angle, 0x8000);
        difference = NuAngSub(angle, row->rotation);
        step = group->rotation_speed;
        if (difference > step) {
            difference = step;
        } else if (difference < -step) {
            difference = -step;
        } else {
            row->is_turning = 0;
            row->path_info.connection = row->next_connection;
            row->next_connection = NULL;
            row->path_info.direction = row->next_direction;
        }
        row->rotation = NuAngAdd(difference, row->rotation);
        NuVecRotateY(&rotated, &offset, row->rotation);
        NuVecSub(&row->position, &centre, &rotated);
        return 0;
    }
    if (!(group->flags & 0x10u)) {
        angle = row->path_info.connection->rotation;
        if (row->path_info.direction != 0)
            angle = NuAngAdd(angle, 0x8000);
        difference = NuAngSub(angle, row->rotation);
        step = group->rotation_speed;
        if (difference > step)
            difference = step;
        else if (difference < -step)
            difference = -step;
        row->rotation = NuAngAdd(difference, row->rotation);
        rotated.x = rotated.y = rotated.z = 0.0f;
        if (GetCharacterGoalSpeedFn != NULL)
            rotated.z = GetCharacterGoalSpeedFn(leader);
        NuVecRotateY(&rotated, &rotated, row->rotation);
        NuVecAdd(&row->position, &row->position, &rotated);
        row->position.y = leader->position.y;
    }
    return 0;
}

static i32 RowMoveTowards(AIGROUP_PC *group, AIROW_PC *row, AIROW_PC *previous, APIOBJECT_PC *leader) {
    i32 i;
    i32 node_index;
    i32 direction_node;
    i32 connection_index;
    i32 reverse;
    i32 angle;
    i32 difference;
    i32 speed;
    f32 distance;
    NUVEC offset;
    NUVEC rotated;
    NUVEC centre;
    AIPACKET_PC *ai;
    AIPATHCNX_PC *connection = row->path_info.connection;
    AIPATHNODE_PC *nodes;
    AIPATHNODE_PC *node;
    AIPATHNODE_PC *node1;
    APIOBJECT_PC *object;
    NUVEC *position;
    if (connection == NULL)
        return 0;
    node_index = connection->node_indices[row->path_info.direction == 0];
    direction_node = node_index;
    if (row->next_connection == NULL) {
        if (previous != NULL) {
            row->next_connection = row->path_info.connection == previous->path_info.connection
                                       ? previous->next_connection
                                       : previous->path_info.connection;
        } else if (group->leader != NULL) {
            ai = group->leader->ai;
            if (ai->script_destination_path_info.path == row->path_info.path) {
                direction_node = ai->script_destination_path_info.distance > 0.5f
                                     ? ai->script_destination_path_info.connection->node_indices[1]
                                     : ai->script_destination_path_info.connection->node_indices[0];
                connection_index = row->path_info.path->route_matrix[node_index][direction_node];
                if (connection_index != 0xff) {
                    row->next_connection = row->path_info.path->nodes[node_index].connections[connection_index];
                    reverse = row->next_connection->node_indices[0] != node_index;
                    if (row->next_connection->traversal_flags[reverse] != 0)
                        row->next_connection = NULL;
                }
            }
        }
        if (row->next_connection != NULL) {
            reverse = row->next_connection->node_indices[0] != direction_node;
            if (row->next_connection->traversal_flags[reverse] == 0) {
                if (row->path_info.direction != 0)
                    angle = NuAngAdd(0x8000, row->path_info.connection->rotation);
                else
                    angle = row->path_info.connection->rotation;
                if (row->path_info.connection->node_indices[row->path_info.direction == 0] ==
                    row->next_connection->node_indices[0]) {
                    row->next_direction = 0;
                    difference = row->next_connection->rotation;
                } else {
                    row->next_direction = 1;
                    difference = NuAngAdd(0x8000, row->next_connection->rotation);
                }
                if (NuAngSub(difference, angle) < 0)
                    row->flags |= 1;
                else
                    row->flags &= ~1;
            }
        }
    }
    if (row->next_connection != NULL) {
        offset.z = offset.y = 0.0f;
        offset.x = (row->flags & 1) != 0 ? -group->radius : group->radius;
        NuVecRotateY(&rotated, &offset, row->rotation);
        position = &row->position;
        NuVecAdd(&centre, &rotated, position);
        if (!row->is_turning) {
            if (row->path_info.connection == row->next_connection) {
                node =
                    &row->path_info.path->nodes[row->path_info.connection->node_indices[row->path_info.direction == 0]];
                distance = NuVecXZDistSqr(position, &node->position, &rotated);
                if (distance < node->radius_squared) {
                    for (i = 0; i < group->row_count; ++i) {
                        group->rows[i].next_connection = NULL;
                        group->rows[i].rotation = NuAngAdd(group->rows[i].rotation, 0x8000);
                        group->rows[i].path_info.direction = group->rows[i].path_info.direction == 0;
                    }
                    group->is_reversed = !group->is_reversed;
                    group->leader = NULL;
                    if (group->is_reversed) {
                        for (i = group->member_count - 1; i >= 0; --i) {
                            object = group->members[i];
                            if (object != NULL && (object->flags_low & 1) != 0 && (object->flags_low & 0x1000) != 0 &&
                                object->leader_selection_state == 0) {
                                group->leader = group->members[i];
                                break;
                            }
                        }
                    } else {
                        for (i = 0; i < group->member_count; ++i) {
                            object = group->members[i];
                            if (object->owner != NULL && (object->flags_low & 1) != 0 &&
                                (object->flags_low & 0x1000) != 0 && object->leader_selection_state == 0) {
                                group->leader = group->members[i];
                                break;
                            }
                        }
                    }
                    return 1;
                }
            } else {
                nodes = row->path_info.path->nodes;
                node = &nodes[row->next_connection->node_indices[0]];
                node1 = &nodes[row->next_connection->node_indices[1]];
                distance = NuInfiniteLineToPointDistSqr(&node->position, &node1->position, &centre);
                if (distance <= group->radius * group->radius)
                    row->is_turning = 1;
            }
        }
        if (row->is_turning) {
            angle = row->next_connection->rotation;
            if (row->next_direction != 0)
                angle = NuAngAdd(angle, 0x8000);
            difference = NuAngSub(angle, row->rotation);
            speed = group->rotation_speed;
            if (difference > speed) {
                difference = speed;
            } else if (difference < -speed) {
                difference = -speed;
            } else {
                row->is_turning = 0;
                row->path_info.connection = row->next_connection;
                row->path_info.direction = row->next_direction;
                row->next_connection = NULL;
            }
            row->rotation = NuAngAdd(difference, row->rotation);
            NuVecRotateY(&rotated, &offset, row->rotation);
            NuVecSub(position, &centre, &rotated);
            return 0;
        }
        if (!group->is_row_turning) {
            angle = row->path_info.connection->rotation;
            if (row->path_info.direction != 0)
                angle = NuAngAdd(angle, 0x8000);
            difference = NuAngSub(angle, row->rotation);
            speed = group->rotation_speed;
            if (difference > speed)
                difference = speed;
            else if (difference < -speed)
                difference = -speed;
            row->rotation = NuAngAdd(difference, row->rotation);
            rotated.z = rotated.y = rotated.x = 0.0f;
            if (GetCharacterGoalSpeedFn != NULL)
                rotated.z = GetCharacterGoalSpeedFn(leader);
            NuVecRotateY(&rotated, &rotated, row->rotation);
            NuVecAdd(position, position, &rotated);
            row->position.y = leader->position.y;
        }
    } else {
        if (row->path_info.direction != 0) {
            if (row->path_info.distance < 0.0f)
                return 1;
        } else if (row->path_info.distance > 1.0f) {
            return 1;
        }
        angle = row->path_info.connection->rotation;
        if (row->path_info.direction != 0)
            angle = NuAngAdd(angle, 0x8000);
        difference = NuAngSub(angle, row->rotation);
        speed = group->rotation_speed;
        if (difference > speed)
            difference = speed;
        else if (difference < -speed)
            difference = -speed;
        row->rotation = NuAngAdd(difference, row->rotation);
        rotated.z = rotated.y = rotated.x = 0.0f;
        if (GetCharacterGoalSpeedFn != NULL)
            rotated.z = GetCharacterGoalSpeedFn(leader);
        NuVecRotateY(&rotated, &rotated, row->rotation);
        NuVecAdd(&row->position, &row->position, &rotated);
        row->position.y = leader->position.y;
    }
    return 0;
}

static void FormationMove(AIGROUP_PC *group, i32 (*move)(AIGROUP_PC *, AIROW_PC *, AIROW_PC *, APIOBJECT_PC *)) {
    i32 i;
    i32 turning;
    AIROW_PC *previous;
    AIROW_PC *row;
    turning = 0;
    if (move != NULL && (group->flags & 8u)) {
        previous = NULL;
        if ((group->flags & 4u)) {
            for (i = group->row_count - 1; i != -1; --i) {
                row = &group->rows[i];
                if (row->is_alive) {
                    if (move(group, row, previous, group->leader) != 0)
                        break;
                    if ((row->flags & 2u))
                        turning = 1;
                    previous = row;
                }
            }
        } else {
            for (i = 0; i < group->row_count; ++i) {
                row = &group->rows[i];
                if (row->is_alive) {
                    if (move(group, row, previous, group->leader) != 0)
                        break;
                    if ((row->flags & 2u))
                        turning = 1;
                    previous = row;
                }
            }
        }
    }
    group->flags = (group->flags & ~0x10u) | ((u32)turning << 4);
}

static i32 AIMoveFindNearestSpecialRouteNode(AIPATH_PC *path, NUVEC *destination, i32 *nearest_node, f32 *distance) {
    i32 node;
    f32 nearest;
    i32 index;
    u8 candidate_node;
    f32 candidate;

    node = -1;
    nearest = FLT_MAX;

    for (index = 0; index < path->special_route_count; ++index) {
        candidate_node = path->special_routes[index].node_index;
        candidate = NuVecDistSqr(&path->nodes[candidate_node].position, destination, NULL);
        if (candidate < nearest) {
            nearest = candidate;
            node = candidate_node;
        }
    }
    if (node < 0) {
        return 0;
    }
    *nearest_node = node;
    *distance = nearest;
    return 1;
}

static void AIMoveFindDivertNode(AISYS_PC *system, AIPATH_PC *path, AIPACKET_PC *packet, NUVEC *destination) {
    AIPATHNODELINK_PC *links;
    f32 nearest;
    u8 initial_index;
    i32 index;
    f32 distance;
    f32 nearest_2;
    i32 index_2;
    f32 distance_2;

    if (path->special_route_count != 0 && (packet->navigation_flags & 4u) != 0) {
        links = path->special_routes;
        if (packet->divert_node_index >= path->node_count) {
            packet->divert_node_index = 0xff;
        }
        if (packet->divert_search_index >= path->special_route_count) {
            packet->divert_search_index = 0;
        }
        nearest = FLT_MAX;
        if (packet->divert_node_index < path->node_count) {
            nearest = NuVecDistSqr(&path->nodes[packet->divert_node_index].position, destination, NULL);
        }
        initial_index = packet->divert_search_index;
        for (index = 0; index < 4; ++index) {
            if (packet->divert_search_index != packet->divert_node_index) {
                distance = NuVecDistSqr(&path->nodes[links[packet->divert_search_index].node_index].position,
                                        destination, NULL);
                if (distance < nearest) {
                    nearest = distance;
                    packet->divert_node_index = links[packet->divert_search_index].node_index;
                }
            }
            ++packet->divert_search_index;
            if (packet->divert_search_index > path->special_route_count) {
                packet->divert_search_index = 0;
            }
            if (packet->divert_search_index == initial_index) {
                break;
            }
        }
    } else {
        if (packet->divert_node_index >= path->node_count) {
            packet->divert_node_index = 0xff;
        }
        if (packet->divert_search_index >= path->node_count) {
            packet->divert_search_index = 0;
        }
        nearest_2 = FLT_MAX;
        if (packet->divert_node_index < path->node_count) {
            nearest_2 = NuVecDistSqr(&path->nodes[packet->divert_node_index].position, destination, NULL);
        }
        for (index_2 = 0; index_2 < 4; ++index_2) {
            distance_2 = NuVecDistSqr(&path->nodes[packet->divert_search_index].position, destination, NULL);
            if (distance_2 < nearest_2) {
                nearest_2 = distance_2;
                packet->divert_node_index = packet->divert_search_index;
            }
            ++packet->divert_search_index;
            if (packet->divert_search_index > path->node_count) {
                packet->divert_search_index = 0;
            }
        }
    }
}

static i32 AIMoveApplyDivertNode(AIPACKET_PC *packet) {
    AIPATHNODE_PC *node;
    f32 radius;

    node = &packet->path_info.path->nodes[packet->divert_node_index];

    if (node->connection_count == 0) {
        return 0;
    }
    packet->fallback_destination = node->position;
    packet->fallback_stopping_distance = 0.0f;
    radius = node->radius - 1.0f;
    packet->movement_parameter = radius > 1.0f ? radius : 1.0f;
    memset(&packet->fallback_path_info, 0, sizeof(packet->fallback_path_info));
    packet->fallback_path_info.path = packet->path_info.path;
    packet->fallback_path_info.connection = node->connections[0];
    packet->fallback_path_info.direction = node->connections[0]->node_indices[0] == packet->divert_node_index;
    packet->fallback_path_info.distance =
        node->connections[0]->node_indices[0] == packet->divert_node_index ? 0.0f : 1.0f;
    return 1;
}

static i32 AIMoveChooseExitNodePath(AISYS_PC *system, AIPACKET_PC *packet) {
    AIPATHNODE_PC *first;
    AIPATHNODE_PC *second;
    f64 first_distance;
    f32 second_distance;
    AIPATHNODE_PC *node;
    AIPATHSPECIALROUTE_PC *route;
    i32 index;
    f32 nearest;
    AIPATH_PC *selected;
    i32 nearest_node;
    f32 nearest_node_distance;

    first = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[0]];
    second = &packet->path_info.path->nodes[packet->path_info.connection->node_indices[1]];
    first_distance = (f32)(NuVecDistSqr(&packet->owner->position, &first->position, NULL) - first->radius_squared);
    second_distance = NuVecDistSqr(&packet->owner->position, &second->position, NULL) - second->radius_squared;
    node = first_distance < second_distance ? first : second;

    selected = NULL;

    if (node->special_route_index >= packet->path_info.path->special_route_count) {
        return 0;
    }
    route =
        &system->path_sys
             ->special_routes[packet->path_info.path->special_routes[node->special_route_index].special_route_index];
    for (index = 0; index < route->path_count; ++index) {
        if (route->paths[index] == packet->fallback_path_info.path) {
            AISysCharacterSetPath(packet, packet->fallback_path_info.path);
            return 1;
        }
    }
    nearest = FLT_MAX;
    for (index = 0; index < route->path_count; ++index) {
        /* PC ignores success. With no nearest node the output is stale or uninitialized. */
        AIMoveFindNearestSpecialRouteNode(route->paths[index], &packet->fallback_destination, &nearest_node,
                                          &nearest_node_distance);
        if (nearest_node_distance < nearest) {
            nearest = nearest_node_distance;
            selected = route->paths[index];
        }
    }
    if (selected != NULL) {
        AISysCharacterSetPath(packet, selected);
        return 1;
    }
    return 0;
}

static i32 AIMoveAdjustDestinationPath(AISYS_PC *system, AIPACKET_PC *packet) {

    if (packet->path_info.path == packet->fallback_path_info.path && packet->fallback_path_info.connection != NULL) {
        return 0;
    }
    AIMoveFindDivertNode(system, packet->path_info.path, packet, &packet->fallback_destination);
    if (packet->divert_node_index >= packet->path_info.path->node_count) {
        return -1;
    }
    return AIMoveApplyDivertNode(packet) != 0 ? 1 : -1;
}

void AIMoveToDestination(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    AIPATHCNX_PC *previous;
    i32 direction;
    NUVEC saved_destination;
    f32 saved_stopping_distance;
    f32 saved_parameter;
    AIPATHINFO_PC saved_path_info;
    AIPATH_PC *path;
    i32 diverted;
    AIPATHCNX_PC *connection;
    AIPATHCNX_PC *destination_connection;
    NUVEC difference;
    f32 distance_squared;
    AIPATHCNX_PC *target;
    AIPATHCNX_PC *current;
    i32 goal_index;
    f32 current_distance;
    f32 endpoint_distances[2];
    f32 best_distance;
    u32 attempted_routes;
    f32 destination_distance_0;
    f32 destination_distance_1;
    AIPATHNODE_PC *node;
    f32 candidate;
    f32 candidate_2;
    f32 candidate_3;
    f32 candidate_4;
    AIPATHCNX_PC *next;
    u32 flags;
    AIPATHCNX_PC *current_2;
    u32 flags_2;
    AIPATHNODE_PC *shared;
    AIPATHCNX_PC *next_2;
    NUVEC target_2;
    i32 use_left;
    NUVEC offset;
    i32 angle;
    i32 angle_2;
    NUVEC offset_2;

    direction = 0;
    attempted_routes = 0;
    saved_destination = packet->fallback_destination;
    saved_stopping_distance = packet->fallback_stopping_distance;
    saved_parameter = packet->movement_parameter;
    saved_path_info = packet->fallback_path_info;
    path = packet->path_info.path;
    diverted = AIMoveAdjustDestinationPath(system, packet);
    if (diverted < 0) {
        packet->movement_destination = packet->owner->position;
        packet->movement_stopping_distance = 0.0f;
        return;
    }
    path = packet->path_info.path;
    connection = packet->path_info.connection;
    destination_connection = packet->fallback_path_info.connection;

    distance_squared = NuVecDistSqr(&packet->fallback_destination, &object->position, &difference);
    if ((object->supporting_platform_id == -1 || (path->nodes[connection->node_indices[0]].has_special != 0 &&
                                                  path->nodes[connection->node_indices[1]].has_special != 0)) &&
        packet->movement_parameter != 0.0f && difference.y < AIMovementInstructionPositiveThreshold &&
        difference.y > AIMovementInstructionNegativeThreshold &&
        distance_squared < packet->movement_parameter * packet->movement_parameter) {
        if (diverted == 0) {
            packet->movement_destination = object->position;
            packet->movement_stopping_distance = 0.0f;
            return;
        }
        if ((packet->navigation_flags & 4) != 0) {
            packet->fallback_destination = saved_destination;
            packet->fallback_stopping_distance = saved_stopping_distance;
            packet->movement_parameter = saved_parameter;
            packet->fallback_path_info = saved_path_info;
            AIMoveChooseExitNodePath(system, packet);
            AIMoveAdjustDestinationPath(system, packet);
            path = packet->path_info.path;
            connection = packet->path_info.connection;
            destination_connection = packet->fallback_path_info.connection;
        }
    }
    if (path == NULL || connection == NULL || destination_connection == NULL) {
        packet->movement_destination = packet->owner->position;
        packet->movement_stopping_distance = 0.0f;
        return;
    }
    if (packet->path_info.connection != packet->fallback_path_info.connection) {
        if (WithinConnection(system, &packet->terrain_origin, packet->path_info.path,
                             packet->fallback_path_info.connection, checks, packet->path_info.connection,
                             packet->current_route, object->object_id, NULL, object->collision_radius, 0) != 0) {
            target = packet->fallback_path_info.connection;
            current = packet->path_info.connection;
            direction = target->node_indices[0] != current->node_indices[0] &&
                        target->node_indices[0] != current->node_indices[1];
            AISysCharacterSetPathCnx(packet, &object->position, target, direction);
        }
        if (packet->path_info.connection != packet->fallback_path_info.connection) {
            best_distance = FLT_MAX;
            direction = packet->path_info.direction;
            goal_index = destination_connection->node_indices[0];
            current_distance = packet->path_info.distance;
            if (current_distance > 1.0f) {
                current_distance = 1.0f;
            } else if (current_distance < 0.0f) {
                current_distance = 0.0f;
            }
            endpoint_distances[0] = AIPathFabs(current_distance * connection->distance);
            endpoint_distances[1] = AIPathFabs((1.0f - current_distance) * connection->distance);
            destination_distance_0 = AIPathFabs(packet->fallback_path_info.distance * destination_connection->distance);
            destination_distance_1 =
                AIPathFabs((1.0f - packet->fallback_path_info.distance) * destination_connection->distance);
            if (packet->current_route != 0xff) {
                packet->flags |= 0x00100000u;
            }
            for (;;) {
                if (packet->current_route != 0xff &&
                    (((u64)1 << packet->current_route) & (destination_connection->route_mask)) == 0) {
                    node = &path->nodes[destination_connection->node_indices[0]];
                    if ((((u64)1 << packet->current_route) &
                         (node->route_membership_mask & ~node->route_boundary_mask)) != 0) {
                        destination_distance_0 = FLT_MAX;
                    }
                    node = &path->nodes[destination_connection->node_indices[1]];
                    if ((((u64)1 << packet->current_route) &
                         (node->route_membership_mask & ~node->route_boundary_mask)) != 0) {
                        destination_distance_1 = FLT_MAX;
                    }
                }
                if ((connection->traversal_flags[packet->path_info.direction] & 0x40000000) == 0) {
                    {
                        candidate = FLT_MAX;
                        if (destination_distance_0 != FLT_MAX &&
                            endpoint_distances[0 == packet->path_info.direction] != FLT_MAX) {
                            candidate = AIPathNodeDistanceToPathNode(
                                packet->path_info.path, connection->node_indices[packet->path_info.direction == 0],
                                destination_connection->node_indices[0], packet->current_route, 0);
                            if (candidate != FLT_MAX) {
                                candidate +=
                                    endpoint_distances[packet->path_info.direction == 0] + destination_distance_0;
                            }
                        }
                        if (candidate < best_distance) {
                            best_distance = candidate;
                            direction = packet->path_info.direction;
                            goal_index = destination_connection->node_indices[0];
                            packet->flags &= ~0x00100000u;
                        }
                    }
                    {
                        candidate_2 = FLT_MAX;
                        if (destination_distance_1 != FLT_MAX &&
                            endpoint_distances[packet->path_info.direction == 0] != FLT_MAX) {
                            candidate_2 = AIPathNodeDistanceToPathNode(
                                packet->path_info.path, connection->node_indices[packet->path_info.direction == 0],
                                destination_connection->node_indices[1], packet->current_route, 0);
                            if (candidate_2 != FLT_MAX) {
                                candidate_2 +=
                                    endpoint_distances[packet->path_info.direction == 0] + destination_distance_1;
                            }
                        }
                        if (candidate_2 < best_distance) {
                            best_distance = candidate_2;
                            goal_index = destination_connection->node_indices[1];
                            direction = packet->path_info.direction;
                            packet->flags &= ~0x00100000u;
                        }
                    }
                }
                if ((connection->traversal_flags[0 == packet->path_info.direction] & 0x40000000) == 0) {
                    {
                        candidate_3 = FLT_MAX;
                        if (destination_distance_0 != FLT_MAX &&
                            endpoint_distances[packet->path_info.direction] != FLT_MAX) {
                            candidate_3 = AIPathNodeDistanceToPathNode(
                                packet->path_info.path, connection->node_indices[packet->path_info.direction],
                                destination_connection->node_indices[0], packet->current_route, 0);
                            if (candidate_3 != FLT_MAX) {
                                candidate_3 += destination_distance_0 + endpoint_distances[packet->path_info.direction];
                            }
                        }
                        if (candidate_3 < best_distance) {
                            best_distance = candidate_3;
                            direction = packet->path_info.direction == 0;
                            goal_index = destination_connection->node_indices[0];
                            packet->flags &= ~0x00100000u;
                        }
                    }
                    {
                        candidate_4 = FLT_MAX;
                        if (destination_distance_1 != FLT_MAX &&
                            endpoint_distances[packet->path_info.direction] != FLT_MAX) {
                            candidate_4 = AIPathNodeDistanceToPathNode(
                                packet->path_info.path, connection->node_indices[packet->path_info.direction],
                                destination_connection->node_indices[1], packet->current_route, 0);
                            if (candidate_4 != FLT_MAX) {
                                candidate_4 = candidate_4 + (destination_distance_1 +
                                                             endpoint_distances[packet->path_info.direction]);
                            }
                        }
                        if (candidate_4 < best_distance) {
                            best_distance = candidate_4;
                            direction = packet->path_info.direction == 0;
                            goal_index = destination_connection->node_indices[1];
                            packet->flags &= ~0x00100000u;
                        }
                    }
                }
                if ((packet->flags & 0x00100000u) == 0) {
                    break;
                }
                if (packet->current_route != 0xff) {
                    attempted_routes |= 1u << packet->current_route;
                }
                AISysFindRoute(packet);
                if (((attempted_routes >> packet->current_route) & 1) != 0) {
                    return;
                }
            }
            packet->goal_path_node = &path->nodes[goal_index];
            AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
            previous = NULL;
            next = GetNextConnection(packet, &direction);
            if (next != NULL) {
                do {
                    if (next == previous ||
                        WithinConnection(system, &packet->terrain_origin, packet->path_info.path, next, checks,
                                         packet->path_info.connection, packet->current_route, object->object_id, NULL,
                                         object->collision_radius, 0) == 0 ||
                        connection == destination_connection) {
                        break;
                    }
                    AISysCharacterSetPathCnx(packet, &object->position, next, direction);
                    connection = next;
                    previous = next;
                    if (packet->path_info.connection != next || packet->path_info.direction != direction) {
                        break;
                    }
                    next = GetNextConnection(packet, &direction);
                } while (next != NULL);
            } else {
                flags = destination_connection->traversal_flags[0] | destination_connection->traversal_flags[1];
                if ((flags & 0xd8000000) == 0 &&
                    ((flags & 0xdfffffff) == 0 || (packet->capabilities & (flags & 0xdfffffff)) != 0) &&
                    WithinConnection(system, &packet->terrain_origin, packet->path_info.path, destination_connection,
                                     checks, packet->path_info.connection, packet->current_route, object->object_id,
                                     NULL, object->collision_radius, 0) != 0) {
                    AISysCharacterSetPathCnx(packet, &object->position, destination_connection, 0);
                }
            }
        }
    }
    if (PreparingForSpecialMoveFn != NULL) {
        if (connection != destination_connection) {
            packet->flags |= 0x20000000u;
        }
        if (PreparingForSpecialMoveFn(packet, object, checks) != 0) {
            packet->flags |= 0x00080000u;
            packet->flags |= 0x40000000u;
            return;
        }
    }
    current_2 = packet->path_info.connection;
    flags_2 = current_2->traversal_flags[packet->path_info.direction];
    if ((flags_2 & 0xd8000000) != 0) {
        packet->movement_destination = path->nodes[current_2->node_indices[packet->path_info.direction]].position;
        packet->flags |= 0x00400000u;
        packet->movement_stopping_distance = 0.0f;
        packet->frame_state = flags_2;
        return;
    }
    if (current_2 == destination_connection &&
        (packet->current_route == 0xff ||
         (((u64)1 << packet->current_route) & (destination_connection->route_mask)) != 0)) {
        if ((packet->path_info.flags & 1u) == 0 || packet->movement_parameter == 0.0f) {
            packet->movement_stopping_distance = packet->fallback_stopping_distance;
            packet->movement_destination = packet->fallback_destination;
            return;
        }
        if (distance_squared < packet->movement_parameter * packet->movement_parameter) {
            if (diverted == 0) {
                packet->movement_destination = object->position;
                packet->movement_stopping_distance = 0.0f;
                return;
            }
            if ((packet->navigation_flags & 4) != 0) {
                packet->fallback_destination = saved_destination;
                packet->fallback_stopping_distance = saved_stopping_distance;
                packet->movement_parameter = saved_parameter;
                packet->fallback_path_info = saved_path_info;
                AIMoveChooseExitNodePath(system, packet);
                AIMoveAdjustDestinationPath(system, packet);
            }
        }
        NuVecScale(&difference, &difference, packet->movement_parameter / NuFsqrt(distance_squared));
        packet->movement_destination.x = packet->fallback_destination.x - difference.x;
        packet->movement_destination.y = packet->fallback_destination.y;
        packet->movement_destination.z = packet->fallback_destination.z - difference.z;
        packet->movement_stopping_distance =
            packet->fallback_stopping_distance < packet->movement_parameter ? 0.0f : packet->fallback_stopping_distance;
        return;
    }
    shared = NULL;
    if (destination_connection->node_indices[0] == current_2->node_indices[0]) {
        shared = &path->nodes[destination_connection->node_indices[0]];
        direction = 0;
    } else if (current_2->node_indices[0] == destination_connection->node_indices[1]) {
        shared = &path->nodes[destination_connection->node_indices[1]];
        direction = 1;
    } else if (current_2->node_indices[1] == destination_connection->node_indices[0]) {
        shared = &path->nodes[destination_connection->node_indices[0]];
        direction = 0;
    } else if (current_2->node_indices[1] == destination_connection->node_indices[1]) {
        shared = &path->nodes[destination_connection->node_indices[1]];
        direction = 1;
    }

    if (shared != NULL && (packet->current_route == 0xff ||
                           (((u64)1 << packet->current_route) & (destination_connection->route_mask)) != 0 ||
                           (((u64)1 << packet->current_route) & (shared->route_boundary_mask)) != 0)) {
        next_2 = destination_connection;
    } else {
        next_2 = GetNextConnection(packet, &direction);
    }
    if (next_2 != NULL) {
        flags_2 = next_2->traversal_flags[direction];
        if ((flags_2 & 0xdfffffff) != 0) {
            if ((flags_2 & 0xd8000000) != 0) {
                packet->movement_destination = object->position;
                packet->flags |= 0x00400000u;
                packet->movement_stopping_distance = 0.0f;
                packet->frame_state = flags_2;
                return;
            }
            if ((packet->capabilities & flags_2) != 0) {
                packet->navigation_flags |= 1;
            } else {
                packet->flags |= 0x00600000u;
                packet->frame_state = next_2->traversal_flags[direction];
                next_2 = NULL;
            }
        }
        if (next_2 != NULL) {

            if (next_2 == destination_connection) {
                target_2 = packet->fallback_destination;
            } else {
                target_2 = path->nodes[next_2->node_indices[direction]].position;
                packet->flags |= 0x00080000u;
            }
            if (WithinConnection(system, &target_2, packet->path_info.path, current_2, checks,
                                 packet->path_info.connection, packet->current_route, object->object_id, NULL,
                                 object->collision_radius, 0) == 0) {
                if (CalculateIntersection(system, packet, object, current_2, next_2) != 0) {
                    use_left = 0;

                    if ((packet->flags & 0x00000040u) != 0) {
                        NuVecSub(&offset, &packet->left_diversion, &object->position);
                        angle = NuAtan2D(offset.x, offset.z);
                        NuVecSub(&offset, &target_2, &object->position);
                        if (NuAngSub(NuAtan2D(offset.x, offset.z), angle) < 0) {
                            target_2 = packet->left_diversion;
                            packet->flags |= 0x00080000u;
                            use_left = 1;
                        }
                    }
                    if (!use_left && (packet->flags & 0x00000020u) != 0) {
                        NuVecSub(&offset, &packet->right_diversion, &object->position);
                        angle_2 = NuAtan2D(offset.x, offset.z);
                        NuVecSub(&offset, &target_2, &object->position);
                        if (NuAngSub(NuAtan2D(offset.x, offset.z), angle_2) > 0) {
                            target_2 = packet->right_diversion;
                            packet->flags |= 0x00080000u;
                        }
                    }
                } else {
                    target_2 = path->nodes[next_2->node_indices[direction]].position;
                }
            }
            packet->movement_stopping_distance = 0.0f;
            packet->movement_destination = target_2;
            return;
        }
    }
    packet->movement_destination =
        path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]].position;
    packet->movement_stopping_distance = 0.0f;

    if (NuVecDistSqr(&packet->movement_destination, &object->position, &offset_2) < 1.0f) {
        packet->movement_destination = object->position;
    }
    packet->flags |= 0x00400000u;
}

void AIMoveToDestinationAvoidingCamera(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    NUVEC destination;

    destination = packet->fallback_destination;
    AIMoveToDestination(system, packet, object, checks);
    packet->fallback_destination = destination;
}

static void AIRetreatFromDestinationPath(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    NUVEC difference;
    AIPATHCNX_PC *connection;
    AIPATH_PC *path;
    AIPATHCNX_PC *destination_connection;
    f32 parameter;
    f32 destination_a;
    f32 destination_b;
    AIPATHNODE_PC *node;
    f32 best_distance;
    i32 direction;
    f32 partial_distance;
    f32 candidate;
    f32 partial_distance_2;
    f32 candidate_2;
    i32 end_node;
    AIPATHNODE_PC *node_2;
    i32 start;
    i32 index;
    AIPATHCNX_PC *target;
    u8 old_direction;
    AIPATHNODE_PC *node_3;
    i32 index_2;
    AIPATHCNX_PC *target_2;
    i32 angle;
    i32 angle_2;

    connection = packet->path_info.connection;
    if (connection == NULL || packet->fallback_path_info.connection == NULL ||
        packet->path_info.path != packet->fallback_path_info.path) {
        return;
    }
    path = packet->path_info.path;
    destination_connection = packet->fallback_path_info.connection;
    if (connection == destination_connection) {
        AISysCharacterSetPathCnx(packet, &object->position, connection,
                                 packet->path_info.distance < packet->fallback_path_info.distance);
    } else {
        best_distance = 0.0f;
        direction = 0;
        parameter = packet->path_info.distance;
        if (parameter > 1.0f) {
            parameter = 1.0f;
        } else if (parameter < 0.0f) {
            parameter = 0.0f;
        }
        partial_distance = AIPathFabs(parameter * connection->distance);
        partial_distance_2 = AIPathFabs((1.0f - parameter) * connection->distance);
        destination_a = AIPathFabs(destination_connection->distance * packet->fallback_path_info.distance);
        destination_b = AIPathFabs((1.0f - packet->fallback_path_info.distance) * destination_connection->distance);
        if (packet->current_route != 0xff &&
            (((u64)1 << packet->current_route) & (destination_connection->route_mask)) == 0) {
            node = &path->nodes[destination_connection->node_indices[0]];
            if (((u64)1 << packet->current_route) & (node->route_membership_mask & ~node->route_boundary_mask)) {
                destination_a = FLT_MAX;
            }
            node = &path->nodes[destination_connection->node_indices[1]];
            if (((u64)1 << packet->current_route) & (node->route_membership_mask & ~node->route_boundary_mask)) {
                destination_b = FLT_MAX;
            }
        }

        if ((packet->path_info.connection->traversal_flags[1] & 0x40000000u) == 0) {
            candidate =
                AIPathNodeDistanceToPathNode(path, packet->path_info.connection->node_indices[0],
                                             destination_connection->node_indices[0], packet->current_route, 0) +
                partial_distance + destination_a;
            if (candidate > best_distance) {
                best_distance = candidate;
                direction = 1;
            }
            candidate = AIPathNodeDistanceToPathNode(
                            packet->path_info.path, packet->path_info.connection->node_indices[0],
                            packet->fallback_path_info.connection->node_indices[1], packet->current_route, 0) +
                        partial_distance + destination_b;
            if (candidate > best_distance) {
                best_distance = candidate;
                direction = 1;
            }
        }
        if ((packet->path_info.connection->traversal_flags[0] & 0x40000000u) == 0) {
            candidate_2 = AIPathNodeDistanceToPathNode(
                              packet->path_info.path, packet->path_info.connection->node_indices[1],
                              packet->fallback_path_info.connection->node_indices[0], packet->current_route, 0) +
                          partial_distance_2 + destination_a;
            if (candidate_2 > best_distance) {
                best_distance = candidate_2;
                direction = 0;
            }
            candidate_2 = AIPathNodeDistanceToPathNode(
                              packet->path_info.path, packet->path_info.connection->node_indices[1],
                              packet->fallback_path_info.connection->node_indices[1], packet->current_route, 0) +
                          partial_distance_2 + destination_b;
            if (candidate_2 > best_distance) {
                direction = 0;
            }
        }
        AISysCharacterSetPathCnx(packet, &object->position, packet->path_info.connection, direction);
    }

    connection = packet->path_info.connection;
    end_node = connection->node_indices[packet->path_info.direction == 0];
    packet->goal_path_node = &path->nodes[end_node];
    if (packet->movement_target != NULL && packet->movement_target->node_indices[0] != end_node &&
        packet->movement_target->node_indices[1] != end_node) {
        packet->movement_target = NULL;
    }
    if (packet->movement_target == NULL) {
        node_2 = &packet->path_info.path->nodes[end_node];
        start = NuRandInt() % node_2->connection_count;
        connection = packet->path_info.connection;
        for (index = 0; index < node_2->connection_count; ++index) {
            packet->movement_target = node_2->connections[(index + start) % node_2->connection_count];
            target = packet->movement_target;
            if (target != connection &&
                (packet->current_route == 0xff || (((u64)1 << packet->current_route) & (target->route_mask)) != 0 ||
                 (((u64)1 << packet->current_route) & (node_2->route_boundary_mask)) != 0) &&
                target->traversal_flags[target->node_indices[0] != end_node] == 0) {
                break;
            }
            packet->movement_target = NULL;
        }
    }
    if (packet->movement_target == NULL) {
        old_direction = packet->path_info.direction;
        connection = packet->path_info.connection;
        end_node = connection->node_indices[old_direction];
        node_3 = &path->nodes[end_node];
        for (index_2 = 0; index_2 < node_3->connection_count; ++index_2) {
            packet->movement_target = node_3->connections[index_2];
            target_2 = packet->movement_target;
            if (target_2 != connection && target_2->traversal_flags[target_2->node_indices[0] != end_node] == 0) {
                AISysCharacterSetPathCnx(packet, &object->position, connection, old_direction == 0);
                connection = packet->path_info.connection;
                end_node = connection->node_indices[packet->path_info.direction == 0];
                packet->goal_path_node = &path->nodes[end_node];
                break;
            }
            packet->movement_target = NULL;
        }
    }
    if (packet->movement_target != NULL) {
        if (WithinConnection(system, &packet->terrain_origin, packet->path_info.path, packet->movement_target, checks,
                             connection, packet->current_route, object->object_id, NULL, object->collision_radius,
                             0) != 0) {
            AISysCharacterSetPathCnx(packet, &object->position, packet->movement_target,
                                     packet->movement_target->node_indices[0] != end_node);
            packet->goal_path_node =
                &path->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
            packet->movement_destination = packet->goal_path_node->position;
            packet->movement_target = NULL;
            packet->movement_stopping_distance = 0.0f;
            return;
        }

        packet->movement_destination =
            packet->path_info.path->nodes[packet->movement_target->node_indices[packet->movement_target_direction == 0]]
                .position;
        packet->movement_stopping_distance = 0.0f;
        if (CalculateIntersection(system, packet, object, packet->path_info.connection, packet->movement_target) == 0) {
            return;
        }
        if ((packet->flags & 0x00000040u) != 0) {
            NuVecSub(&difference, &packet->left_diversion, &object->position);
            angle = NuAtan2D(difference.x, difference.z);
            NuVecSub(&difference, &packet->movement_destination, &object->position);
            if (NuAngSub(NuAtan2D(difference.x, difference.z), angle) < 0) {
                packet->movement_destination = packet->left_diversion;
                return;
            }
        }
        if ((packet->flags & 0x00000020u) != 0) {
            NuVecSub(&difference, &packet->right_diversion, &object->position);
            angle_2 = NuAtan2D(difference.x, difference.z);
            NuVecSub(&difference, &packet->movement_destination, &object->position);
            if (NuAngSub(NuAtan2D(difference.x, difference.z), angle_2) > 0) {
                packet->movement_destination = packet->right_diversion;
            }
        }
    } else {
        packet->movement_destination = packet->goal_path_node->position;
        packet->movement_stopping_distance = 0.0f;
    }
}

void __cdecl AICircle(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    f32 radius = packet->movement_parameter < packet->fallback_stopping_distance ? packet->fallback_stopping_distance
                                                                                 : packet->movement_parameter;
    NUVEC difference;
    NUVEC circle_position;
    i32 angle;
    (void)system;
    (void)checks;
    NuVecSub(&difference, &object->position, &packet->fallback_destination);
    angle = NuAtan2D(difference.x, difference.z);
    difference.x = 0.0f;
    difference.z = radius;
    NuVecRotateY(&difference, &difference, angle);
    NuVecAdd(&circle_position, &packet->fallback_destination, &difference);
    if ((packet->flags & 0x200) != 0) {
        angle = NuAngSub(angle, 0x4000);
    } else {
        angle = NuAngAdd(angle, 0x4000);
    }
    difference.x = 0.0f;
    difference.y = 0.0f;
    difference.z = 0.5f;
    NuVecRotateY(&difference, &difference, angle);
    NuVecAdd(&packet->movement_destination, &circle_position, &difference);
    packet->flags |= 0x400;
    packet->movement_stopping_distance = 0.0f;
}

void AIWander(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    NUVEC difference;
    i32 node_index;
    i32 node_index_2;
    AIPATHNODE_PC *node;
    i32 start;
    i32 index;
    AIPATHCNX_PC *target;
    AIPATHNODE_PC *node_2;
    f32 distance;
    i32 angle;
    i32 angle_2;
    i32 destination_angle;

    if (packet->movement_target != NULL) {
        node_index = packet->path_info.connection->node_indices[packet->path_info.direction == 0];
        if (packet->movement_target->node_indices[0] != node_index &&
            packet->movement_target->node_indices[1] != node_index) {
            packet->movement_target = NULL;
        }
    }
    if (packet->movement_target == NULL) {
        node_index_2 = packet->path_info.connection->node_indices[packet->path_info.direction == 0];
        node = &packet->path_info.path->nodes[node_index_2];
        start = NuRandInt() % node->connection_count;
        for (index = 0; index < node->connection_count; ++index) {
            packet->movement_target = node->connections[(index + start) % node->connection_count];
            target = packet->movement_target;
            if (target != packet->path_info.connection) {
                if (packet->current_route == 0xff || (((u64)1 << packet->current_route) & (target->route_mask)) != 0 ||
                    (((u64)1 << packet->current_route) & (node->route_boundary_mask)) != 0) {
                    if (target->node_indices[0] == node_index_2)
                        packet->movement_target_direction = 0;
                    else
                        packet->movement_target_direction = 1;
                    if (target->traversal_flags[packet->movement_target_direction] == 0) {
                        break;
                    }
                }
            }
            packet->movement_target = NULL;
        }
    }
    if (packet->movement_target != NULL) {
        packet->flags |= 0x00080000u;
        if (WithinConnection(system, &packet->terrain_origin, packet->path_info.path, packet->movement_target, checks,
                             packet->path_info.connection, packet->current_route, object->object_id, NULL,
                             object->collision_radius, 0) != 0) {
            AISysCharacterSetPathCnx(packet, &object->position, packet->movement_target,
                                     packet->movement_target_direction);
            packet->goal_path_node =
                &packet->path_info.path
                     ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
            packet->movement_destination = packet->goal_path_node->position;
            packet->movement_stopping_distance = 0.0f;
            packet->movement_target = NULL;
        } else {
            packet->movement_destination =
                packet->path_info.path
                    ->nodes[packet->movement_target->node_indices[packet->movement_target_direction == 0]]
                    .position;
            packet->movement_stopping_distance = 0.0f;
            if (CalculateIntersection(system, packet, object, packet->path_info.connection, packet->movement_target) !=
                0) {
                if ((packet->flags & 0x00000040u) != 0) {
                    NuVecSub(&difference, &packet->left_diversion, &object->position);
                    angle = NuAtan2D(difference.x, difference.z);
                    NuVecSub(&difference, &packet->movement_destination, &object->position);
                    destination_angle = NuAtan2D(difference.x, difference.z);
                    if (NuAngSub(destination_angle, angle) < 0) {
                        packet->movement_destination = packet->left_diversion;
                        return;
                    }
                }
                if ((packet->flags & 0x00000020u) != 0) {
                    NuVecSub(&difference, &packet->right_diversion, &object->position);
                    angle_2 = NuAtan2D(difference.x, difference.z);
                    NuVecSub(&difference, &packet->movement_destination, &object->position);
                    destination_angle = NuAtan2D(difference.x, difference.z);
                    if (NuAngSub(destination_angle, angle_2) > 0) {
                        packet->movement_destination = packet->right_diversion;
                    }
                }
            }
        }
    } else {
        node_2 = &packet->path_info.path
                      ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
        distance = NuVecXZDistSqr(&object->position, &node_2->position, &difference);
        if (distance < node_2->radius_squared) {
            AISysCharacterSetPathCnx(packet, &object->position, packet->path_info.connection,
                                     packet->path_info.direction == 0);
            node_2 = &packet->path_info.path
                          ->nodes[packet->path_info.connection->node_indices[packet->path_info.direction == 0]];
            packet->goal_path_node = node_2;
        }
        packet->movement_destination = node_2->position;
        packet->movement_stopping_distance = 0.0f;
    }
}

void __cdecl AIMoveDirectlyToDestination(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    (void)system;
    (void)checks;
    if (packet->movement_parameter > 0.0f) {
        NUVEC delta;
        f32 distance_squared = NuVecDistSqr(&packet->fallback_destination, &object->position, &delta);
        if (packet->movement_parameter * packet->movement_parameter > distance_squared) {
            packet->movement_destination = object->position;
            packet->movement_stopping_distance = 0.0f;
            return;
        }
    }
    packet->movement_destination = packet->fallback_destination;
    packet->movement_stopping_distance = 0.0f;
}

i32 __cdecl AISysGetCharacterWaypoint(AIPACKET_PC *packet, NUVEC *position) {
    i32 direction;
    AIPATHCNX_PC *connection;
    if (packet->path_info.connection != packet->fallback_path_info.connection) {
        connection = GetNextConnection(packet, &direction);
        if (connection != NULL) {
            if (position != NULL)
                *position = packet->path_info.path->nodes[connection->node_indices[direction == 0]].position;
            return 1;
        }
    }
    return 0;
}

void LocaledbitsDrawSolidCircleXY(NUVEC *centre, f32 radius, f32 lower_y, f32 upper_y, u32 colour, i32 unused5,
                                  i32 segments) {
    i32 i;
    f32 previous_x = centre->x;
    f32 previous_z = centre->z + radius;
    for (i = 1; i <= segments; ++i) {
        i32 angle = (i * 65536) / segments;
        f32 next_x = centre->x + radius * NU_SIN_LUT(angle);
        f32 next_z = centre->z + NU_COS_LUT(angle) * radius;
        AiRndrLine3dDbg(previous_x, lower_y, previous_z, next_x, lower_y, next_z, colour);
        AiRndrLine3dDbg(previous_x, upper_y, previous_z, next_x, upper_y, next_z, colour);
        AiRndrLine3dDbg(next_x, lower_y, next_z, next_x, upper_y, next_z, colour);
        previous_x = next_x;
        previous_z = next_z;
    }
}

void LocaledbitsDrawSolidEllipseXY(NUVEC *centre, f32 radius_x, f32 radius_z, i32 rotation, f32 lower_y, f32 upper_y,
                                   u32 colour, i32 unused7, i32 segments) {
    i32 i;
    NURND_VERTEX3D_PC vertices[2];
    NURND_VERTEX3D_PC *first = &(vertices[0]);
    NURND_VERTEX3D_PC *second = &(vertices[1]);
    (*second).position.x = 0.0f;
    (*second).position.y = 0.0f;
    (*second).position.z = radius_z;
    NuVecRotateY(&(*second).position, &(*second).position, rotation);
    NuVecAdd(&(*second).position, &(*second).position, centre);
    (*first).colour = colour;
    (*second).colour = colour;
    for (i = 1; i <= segments; ++i) {
        i32 angle;
        (*first).position = (*second).position;
        (*second).position = *centre;
        angle = (i * 65536) / segments;
        {
            NUVEC offset = {radius_x * NU_SIN_LUT(angle), 0.0f, radius_z * NU_COS_LUT(angle)};
            NuVecRotateY(&offset, &offset, rotation);
            NuVecAdd(&(*second).position, centre, &offset);
            (*first).position.y = lower_y;
            (*second).position.y = lower_y;
            AiRndrLine3dDbg((*first).position.x, (*first).position.y, (*first).position.z, (*second).position.x,
                            (*second).position.y, (*second).position.z, colour);
            (*first).position.y = upper_y;
            (*second).position.y = upper_y;
            AiRndrLine3dDbg((*first).position.x, (*first).position.y, (*first).position.z, (*second).position.x,
                            (*second).position.y, (*second).position.z, colour);
            (*first).position = (*second).position;
            (*first).position.y = lower_y;
            AiRndrLine3dDbg((*first).position.x, (*first).position.y, (*first).position.z, (*second).position.x,
                            (*second).position.y, (*second).position.z, colour);
        }
    }
}

void LocaledbitsDrawCircleXY(NUVEC *centre, f32 radius, u32 colour, i32 unused3, i32 segments) {
    i32 i;
    NUVEC previous = {centre->x, centre->y, centre->z + radius};
    for (i = 1; i <= segments; ++i) {
        i32 angle = (i * 65536) / segments;
        NUVEC next = {centre->x + NU_SIN_LUT(angle) * radius, centre->y, centre->z + NU_COS_LUT(angle) * radius};
        AiRndrLine3dDbg(previous.x, previous.y, previous.z, next.x, next.y, next.z, colour);
        previous = next;
    }
}

void __cdecl ResetAIMessageSys(AIMESSAGESYS_PC *sys) {
    i32 index;
    if (sys != NULL) {
        memset(&sys->free_list, 0, sizeof(sys->free_list));
        memset(&sys->active_list, 0, sizeof(sys->active_list));
        memset(sys->messages, 0, sys->count * sizeof(AIMESSAGE_PC));
        for (index = 0; index < sys->count; ++index)
            NuLinkedListAppend(&sys->free_list, &sys->messages[index].links);
    }
}

void __cdecl ClearAIMessageSys(AIMESSAGESYS_PC *sys) {
    NULISTLNK *link;
    if (sys != NULL) {
        for (link = NuLinkedListGetHead(&sys->active_list); link != NULL;
             link = NuLinkedListGetNext(&sys->active_list, link))
            ((AIMESSAGE_PC *)link)->value = 0.0f;
    }
}

AIMESSAGE_PC *__cdecl CheckAIMessage(AIMESSAGESYS_PC *sys, char *name, AIMESSAGE_PC *message) {
    AIMESSAGE_PC *result = NULL;
    if (sys != NULL) {
        if (message == NULL && name != NULL) {
            for (message = (AIMESSAGE_PC *)NuLinkedListGetHead(&sys->active_list); message != NULL;
                 message = (AIMESSAGE_PC *)NuLinkedListGetNext(&sys->active_list, &message->links)) {
                if (NuStrNICmp(name, message->name, 32) == 0)
                    break;
            }
        }
        result = message;
        if (message == NULL) {
            message = (AIMESSAGE_PC *)NuLinkedListGetHead(&sys->free_list);
            if (message != NULL) {
                NuLinkedListRemove(&sys->free_list, &message->links);
                NuLinkedListAppend(&sys->active_list, &message->links);
                NuStrNCpy(message->name, name, 32);
            }
            result = message;
        }
    }
    return result;
}

AIMESSAGE_PC *__cdecl QueryAIMessage(AIMESSAGESYS_PC *sys, AIMESSAGE_PC *message) {
    if (message == NULL)
        return (AIMESSAGE_PC *)NuLinkedListGetHead(&sys->active_list);
    return (AIMESSAGE_PC *)NuLinkedListGetNext(&sys->active_list, &message->links);
}

void __cdecl RemoveAIMessage(AIMESSAGESYS_PC *sys, char *name, AIMESSAGE_PC *message) {
    if (sys != NULL) {
        if (message == NULL) {
            if (name == NULL)
                return;
            for (message = (AIMESSAGE_PC *)NuLinkedListGetHead(&sys->active_list); message != NULL;
                 message = (AIMESSAGE_PC *)NuLinkedListGetNext(&sys->active_list, &message->links)) {
                if (NuStrNICmp(name, message->name, 32) == 0)
                    break;
            }
        }
        if (message != NULL) {
            NuLinkedListRemove(&sys->active_list, &message->links);
            memset(message, 0, sizeof(*message));
            NuLinkedListAppend(&sys->free_list, &message->links);
        }
    }
}

AILOCALMESSAGE_PC *__cdecl FindLocalAIMessage(AISCRIPTPROCESS_PC *processor, char *name) {
    AILOCALMESSAGE_PC *message;
    for (message = (AILOCALMESSAGE_PC *)processor->local_messages; message != NULL; message = message->next) {
        if (NuStrICmp(message->message.name, name) == 0)
            return message;
    }
    return NULL;
}

void __cdecl AddLocalAIMessage(AISCRIPTPROCESS_PC *processor, AILOCALMESSAGE_PC *message, char *name) {
    NuStrNCpy(message->message.name, name, 32);
    if (processor->local_messages == NULL) {
        processor->local_messages = message;
        message->next = NULL;
    } else {
        message->next = (AILOCALMESSAGE_PC *)processor->local_messages;
        processor->local_messages = message;
    }
}

AILOCALMESSAGE_PC *__cdecl QueryLocalAIMessage(AISCRIPTPROCESS_PC *processor, AILOCALMESSAGE_PC *message) {
    if (message == NULL)
        return (AILOCALMESSAGE_PC *)processor->local_messages;
    return message->next;
}

__declspec(noinline) void __cdecl AILocatorSet_CheckLocatorsStillAssigned(AISYS_PC *system,
                                                                          AILOCATORSET_PC *locator_set) {
    i32 index;
    if (locator_set == NULL || APIOBJECTFromObjIDFn == NULL) {
        return;
    }
    for (index = 0; index < locator_set->locator_count; ++index) {
        u8 assignment = locator_set->assigned[index];
        u8 locator_index = 0xff;
        APIOBJECT_PC *object;
        if (assignment == 0x80 || assignment == 0xff) {
            continue;
        }
        object = APIOBJECTFromObjIDFn(assignment);
        if (object != NULL && object->ai != NULL && object->ai->locator != NULL) {
            locator_index = (u8)(object->ai->locator - system->locators);
        }
        if (locator_index != locator_set->locator_entries[index]) {
            locator_set->assigned[index] = 0xff;
        }
    }
}

void __cdecl AILocatorSet_AssignNearestLocator(AISYS_PC *system, AILOCATORSET_PC *locator_set, APIOBJECT_PC *object,
                                               f32 max_range, NUVEC *position, NUVEC *second_position,
                                               f32 off_screen_radius, i32 ignore_assigned) {
    if (object != NULL && locator_set != NULL && object->ai != NULL && position != NULL) {
        f32 nearest_distance = max_range > 0.0f ? max_range * max_range : FLT_MAX;
        NUVEC difference;
        i32 nearest_index = -1;
        i32 index;
        if (ignore_assigned != 0) {
            AILocatorSet_CheckLocatorsStillAssigned(system, locator_set);
        }
        for (index = 0; index < locator_set->locator_count; ++index) {
            AILOCATOR_PC *locator;
            f32 first_distance;
            if (ignore_assigned != 0 && locator_set->assigned[index] != 0xff) {
                continue;
            }
            locator = &system->locators[locator_set->locator_entries[index]];
            if (off_screen_radius != 0.0f &&
                NuCameraClipTestSphere(&locator->position, off_screen_radius, &NuMtxSetIdentityTemplate) == 0) {
                continue;
            }
            first_distance = NuVecDistSqr(position, &locator->position, &difference);
            if (first_distance < nearest_distance) {
                nearest_distance = first_distance;
                nearest_index = index;
            }
            if (second_position != NULL) {
                f32 second_distance = NuVecDistSqr(second_position, &locator->position, &difference);
                if (second_distance < nearest_distance) {
                    nearest_distance = second_distance;
                    nearest_index = index;
                }
            }
        }
        if (nearest_index != -1) {
            object->ai->locator = &system->locators[locator_set->locator_entries[nearest_index]];
            locator_set->assigned[nearest_index] = object->object_id;
        }
    }
}

void __cdecl AILocatorSet_AssignFurthestLocator(AISYS_PC *system, AILOCATORSET_PC *locator_set, APIOBJECT_PC *object,
                                                f32 max_range, NUVEC *position, NUVEC *second_position,
                                                f32 off_screen_radius, i32 ignore_assigned) {
    if (object != NULL && locator_set != NULL && object->ai != NULL) {
        NUVEC difference;
        f32 furthest_distance = 0.0f;
        i32 furthest_index = -1;
        i32 index;
        if (ignore_assigned != 0) {
            AILocatorSet_CheckLocatorsStillAssigned(system, locator_set);
        }
        for (index = 0; index < locator_set->locator_count; ++index) {
            AILOCATOR_PC *locator;
            f32 distance;
            if (ignore_assigned != 0 && locator_set->assigned[index] != 0xff) {
                continue;
            }
            locator = &system->locators[locator_set->locator_entries[index]];
            if (off_screen_radius != 0.0f &&
                NuCameraClipTestSphere(&locator->position, off_screen_radius, &NuMtxSetIdentityTemplate) == 0) {
                continue;
            }
            distance = NuVecDistSqr(position, &locator->position, &difference);
            if (distance > furthest_distance) {
                if (second_position != NULL) {
                    f32 second_distance = NuVecDistSqr(second_position, &locator->position, &difference);
                    if (!(second_distance > furthest_distance)) {
                        continue;
                    }
                    if (second_distance < distance) {
                        distance = second_distance;
                    }
                }
                if (max_range == 0.0f || max_range * max_range > distance) {
                    furthest_distance = distance;
                    furthest_index = index;
                }
            }
        }
        if (furthest_index != -1) {
            object->ai->locator = &system->locators[locator_set->locator_entries[furthest_index]];
            locator_set->assigned[furthest_index] = object->object_id;
        }
    }
}

void __cdecl AILocatorSet_AssignRandomLocator(AISYS_PC *system, AILOCATORSET_PC *locator_set, APIOBJECT_PC *object,
                                              f32 max_range, NUVEC *position, f32 off_screen_radius,
                                              i32 ignore_assigned) {
    f32 max_distance;
    NUVEC difference;
    i32 candidate_count = 0;
    i32 selected_candidate;
    i32 candidate_index = 0;
    i32 index;
    if (object == NULL || locator_set == NULL || object->ai == NULL) {
        return;
    }
    max_distance = max_range > 0.0f ? max_range * max_range : FLT_MAX;
    if (ignore_assigned != 0) {
        AILocatorSet_CheckLocatorsStillAssigned(system, locator_set);
    }
    for (index = 0; index < locator_set->locator_count; ++index) {
        AILOCATOR_PC *locator;
        if (ignore_assigned != 0 && locator_set->assigned[index] != 0xff) {
            continue;
        }
        locator = &system->locators[locator_set->locator_entries[index]];
        if (off_screen_radius != 0.0f &&
            NuCameraClipTestSphere(&locator->position, off_screen_radius, &NuMtxSetIdentityTemplate) == 0) {
            continue;
        }
        if (NuVecDistSqr(position, &locator->position, &difference) <= max_distance) {
            ++candidate_count;
        }
    }
    if (candidate_count == 0) {
        return;
    }
    selected_candidate = NuRandInt() % candidate_count;
    for (index = 0; index < locator_set->locator_count; ++index) {
        AILOCATOR_PC *locator;
        if (ignore_assigned != 0 && locator_set->assigned[index] != 0xff) {
            continue;
        }
        locator = &system->locators[locator_set->locator_entries[index]];
        if (NuVecDistSqr(&object->position, &locator->position, &difference) <= max_distance) {
            if (candidate_index == selected_candidate) {
                object->ai->locator = locator;
                locator_set->assigned[index] = object->object_id;
                break;
            }
            ++candidate_index;
        }
    }
}

u32 __cdecl AISysGetPathColour(i32 index) {
    return iAISysPathColoursG[index];
}

i32 __cdecl AISysGetPathColourCount(void) {
    return 5;
}

void __cdecl InitFn_GameAISYSRebuildFromEditorData(void(__cdecl *callback)(AISYS_PC *, VARIPTR *, VARIPTR *)) {
    GameAISYSRebuildFromEditorDataFn = callback;
}

void __cdecl InitFn_GameAISave(void(__cdecl *callback)(AIPATHSYS_PC *)) {
    GameAISaveFn = callback;
}

void __cdecl InitFn_AIPathNodeDeleted(void(__cdecl *callback)(struct EDAIPATHNODE_s *)) {
    AIPathNodeDeletedFn = callback;
}

void __cdecl InitFn_AIPathNodeMoved(void(__cdecl *callback)(struct EDAIPATHNODE_s *)) {
    AIPathNodeMovedFn = callback;
}

void __cdecl InitFn_AIPathDeleted(void(__cdecl *callback)(struct EDAIPATH_s *)) {
    AIPathDeletedFn = callback;
}

void __cdecl InitFn_PreparingForSpecialMove(AI_PREPARING_SPECIAL_MOVE_PC function) {
    PreparingForSpecialMoveFn = function;
}

void __cdecl InitFn_MidSpecialMove(AI_MID_SPECIAL_MOVE_PC function) {
    MidSpecialMoveFn = function;
}

void __cdecl InitFn_GetNamedAPIObject(AI_GET_NAMED_OBJECT_PC function) {
    GetNamedGameObjectCallback = function;
}

void __cdecl InitFn_GetAICreatureOrigin(AI_GET_CREATURE_ORIGIN_PC function) {
    GetOriginPositionFn = function;
}

void __cdecl AiEditorChangeModePc(i32 mode) {
    i16 previous = aieditorsettings.modes.current;
    aieditorsettings.modes.current = (i16)mode;
    if (aieditor != NULL && previous != (i16)mode) {
        if (aieditor->current_path != NULL)
            aieditor->current_path->current_node = NULL;
        aieditor->selected_creature = NULL;
        aieditor->selected_area = NULL;
        aieditor->current_locator = NULL;
        aieditor->selected_antinode = NULL;
    }
}

void __cdecl aieditor_Enter(AISYS_PC *system, NUVEC *camera_position, void *context, void *owner, char *level_directory,
                            char *level_file, char *level_subdirectory) {
    i32 index;
    aieditor->flags |= 2;
    aieditor->ai_system = system;
    aieditor->enter_context = context;
    aieditor->enter_owner = owner;
    aieditor->level_directory = level_directory;
    aieditor->level_file = level_file;
    aieditor->level_subdirectory = level_subdirectory;
    if (camera_position != NULL)
        edcamSetPos(camera_position);
    for (index = 0; index < aieditorsettings.modes.count; ++index) {
        if (aieditorsettings.modes.entries[index].enter != NULL)
            aieditorsettings.modes.entries[index].enter();
    }
    aieditorsettings.display_options |= 0x10;
}

i32 __cdecl NuSpecialFindByPlatformID(NUGSCN *scene, struct nuhspecial_s *result, i32 platform_id) {
    i32 count;
    i32 instance_index;
    i32 index;
    NUSPECIALHANDLE candidate;
    result->scene = NULL;
    result->special = NULL;
    result->display_special = NULL;
    if (scene == NULL || CurrentTerrain == NULL || platform_id == -1)
        return 0;
    count = NuGScnNumSpecials(scene);
    instance_index = ((AI_EDITOR_SPECIAL_TERRAIN_PC *)CurrentTerrain)->platforms[platform_id].instance_index;
    for (index = 0; index < count; ++index) {
        NuGScnGetSpecial(&candidate, scene, index);
        if (NuSpecialGetInstanceix(&candidate) == instance_index) {
            result->scene = (NUGSCN *)candidate.scene;
            result->special = candidate.special;
            result->display_special = (struct nudisplay_special_pc_s *)candidate.display_special;
            return 1;
        }
    }
    return 0;
}

void __cdecl AISysSetPathCylinderCheck(i32 enabled) {
    disable_cylinder_check = enabled == 0;
}

void __cdecl aieditor_Render(f32 x_scale, f32 y_scale) {
    NUCAMERA *camera = edmainGetCameraPc();
    void(__cdecl * draw)(i32, i32, f32, f32);
    if (near_clip_at_cursor != 0)
        camera->near_clip =
            NuVecDist(&aieditor->cursor_position, &camera->mtx.translation, NULL) - AiEditorNearClipOffsetPc;
    else
        camera->near_clip = 0.15f;
    edcamSet();
    if (aieditorsettings.display_options & 0x10) {
        edbitsDrawCross(aieditor->cursor_position.x, aieditor->cursor_position.y, aieditor->cursor_position.z, 0.5f, -1,
                        (struct PC_SCENE_MTL_VIEW *)aieditorsettings.path_material);
        edbitsDrawCross(aieditor->camera_position.x, aieditor->camera_position.y, aieditor->camera_position.z, 0.25f,
                        0xff00ff00, (struct PC_SCENE_MTL_VIEW *)aieditorsettings.path_material);
    } else {
        edbitsDrawCross(aieditor->camera_position.x, aieditor->camera_position.y, aieditor->camera_position.z, 0.5f, -1,
                        (struct PC_SCENE_MTL_VIEW *)aieditorsettings.path_material);
    }
    NuRndrRect2di(0x1680, 0x9b0, 0x1180, 0x488, 0x80808080, aieditorsettings.overlay_material);
    NuRndrRect2di(0x1670, 0x8f0, 0x11a0, 0xc0, 0x80000000, aieditorsettings.overlay_material);
    NuRndrLine2di(0x1670, 0x9b0, 0x1670, 0xe40, 0x80000000, aieditorsettings.overlay_material);
    NuRndrLine2di(0x2810, 0x9b0, 0x2810, 0xe40, 0x80000000, aieditorsettings.overlay_material);
    NuRndrLine2di(0x1670, 0xe40, 0x2810, 0xe40, 0x80000000, aieditorsettings.overlay_material);
    NuQFntPushPrintMode(2);
    NuQFntPushCoordinateSystem(1);
    NuQFntSet(system_qfont);
    NuQFntSetColour(system_qfont, 0x80808080);
    NuQFntSetScale(system_qfont, x_scale, y_scale);
    draw = aieditorsettings.modes.entries[aieditorsettings.modes.current].draw;
    if (draw != NULL)
        draw(0x168, 0x136, x_scale, y_scale);
    NuQFntSet(system_qfont);
    NuQFntSetColour(system_qfont, 0x80000000);
    NuQFntSetScale(system_qfont, x_scale, y_scale);
    NuQFntPrintEx(system_qfont, 0x1720, 0xde8, 0x10, "%5.2f", aieditor->camera_position.x);
    NuQFntPrintEx(system_qfont, 0x1b80, 0xde8, 0x10, "%5.2f", aieditor->camera_position.y);
    NuQFntPrintEx(system_qfont, 0x1fe0, 0xde8, 0x10, "%5.2f", aieditor->camera_position.z);
    if (aieditor->main_menu != NULL) {
        NuQFntSetColour(system_qfont, 0x80000000);
        NuQFntSetScale(system_qfont, x_scale, y_scale);
        eduiMenuRender(aieditor->main_menu);
    }
    NuQFntPopPrintMode();
    NuQFntPopCoordinateSystem();
}

/* Native 520e10..520e93: private position argument EDI. */
static __declspec(noinline) EDAIAREA_s *areaEditorCreateArea(NUVEC *position, NUVEC *size, i32 rotation, i32 flags) {
    EDAIAREA_s *area = (EDAIAREA_s *)NuLinkedListGetHead(&aieditor->free_areas);
    if (area != NULL) {
        NuLinkedListRemove(&aieditor->free_areas, &area->link);
        NuLinkedListAppend(&aieditor->areas, &area->link);
        area->position = *position;
        if (size != NULL)
            area->size = *size;
        area->rotation = (u16)rotation;
        area->flags = (u8)flags;
    }
    return area;
}

/* Native 520ea0..520ed6: private area argument ESI. */
static __declspec(noinline) void areaEditorDestroyArea(EDAIAREA_s *area) {
    if (area != NULL) {
        NuLinkedListRemove(&aieditor->areas, &area->link);
        memset(area, 0, sizeof(EDAIAREA_s));
        NuLinkedListAppend(&aieditor->free_areas, &area->link);
    }
}

void __cdecl areaEditorSaveData(void) {
    EDAIAREA_s *area;
    i32 count = 0;
    for (area = (EDAIAREA_s *)NuLinkedListGetHead(&aieditor->areas); area != NULL;
         area = (EDAIAREA_s *)NuLinkedListGetNext(&aieditor->areas, &area->link)) {
        ++count;
    }
    EdFileWriteInt(count);
    for (area = (EDAIAREA_s *)NuLinkedListGetHead(&aieditor->areas); area != NULL;
         area = (EDAIAREA_s *)NuLinkedListGetNext(&aieditor->areas, &area->link)) {
        EdFileWrite(area->name, 16);
        EdFileWriteFloat(area->position.x);
        EdFileWriteFloat(area->position.y);
        EdFileWriteFloat(area->position.z);
        EdFileWriteFloat(area->size.x);
        EdFileWriteFloat(area->size.y);
        EdFileWriteFloat(area->size.z);
        EdFileWriteShort(area->rotation);
        if (aidata_version > 19)
            EdFileWriteChar(area->flags);
        else
            EdFileWriteChar(0);
        EdFileWriteChar(0);
    }
}

/* Native 520fe0..521155. */
static __declspec(noinline) EDAIAREA_s *areaEditorFindHover(i32 check_inside) {
    EDAIAREA_s *area;
    EDAIAREA_s *nearest = NULL;
    f32 best = FLT_MAX;
    f32 distance;
    NUVEC difference;
    for (area = (EDAIAREA_s *)NuLinkedListGetHead(&aieditor->areas); area != NULL;
         area = (EDAIAREA_s *)NuLinkedListGetNext(&aieditor->areas, &area->link)) {
        distance = NuVecXZDistSqr(&aieditor->camera_position, &area->position, &difference);
        if (distance < best) {
            difference.y = aieditor->camera_position.y - area->position.y;
            if (check_inside == 0) {
                best = distance;
                nearest = area;
            } else if (area->flags & 1) {
                if (area->size.x * area->size.x > distance && area->size.y > difference.y && difference.y > -2.0f) {
                    best = distance;
                    nearest = area;
                }
            } else {
                NuVecRotateY(&difference, &difference, -area->rotation);
                if (area->size.x > difference.x && area->size.y > difference.y && area->size.z > difference.z &&
                    -area->size.x < difference.x && difference.y > -2.0f && -area->size.z < difference.z) {
                    best = distance;
                    nearest = area;
                }
            }
        }
    }
    return nearest;
}

static void __cdecl AiEditorRenameAreaPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDAIAREA_s *other;
    (void)menu;
    (void)event;
    if (aieditor->selected_area != NULL) {
        char *name = ((edui_textpicker_s *)item)->value;
        if (strlen(name) != 0) {
            for (other = (EDAIAREA_s *)NuLinkedListGetHead(&aieditor->areas); other != NULL;
                 other = (EDAIAREA_s *)NuLinkedListGetNext(&aieditor->areas, &other->link)) {
                if (NuStrICmp(other->name, name) == 0)
                    return;
            }
            strcpy(aieditor->selected_area->name, name);
        }
    }
}

void __cdecl areaEditor_cbCylinderToggle(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (aieditor->selected_area != NULL) {
        if (item->flags & 1)
            aieditor->selected_area->flags |= 1;
        else
            aieditor->selected_area->flags &= ~1;
    }
}

static void __cdecl AiEditorRenameAreaCancelPc(eduimenu_s *menu, eduimenu_s *child) {
    (void)child;
    eduiMenuDestroy(menu);
}

void DrawAreaBox(NUVEC *position, NUVEC *size, i32 rotation, i32 colour) {
    i32 i;
    f32 local_x[4] = {-size->x, -size->x, size->x, size->x};
    f32 rotated_x[4];
    f32 local_z[4] = {-size->z, size->z, size->z, -size->z};
    f32 rotated_z[4];
    const f32 cosine = NU_COS_LUT(rotation);
    const f32 sine = NU_SIN_LUT(rotation);
    for (i = 0; i < 4; ++i) {
        rotated_x[i] = cosine * local_x[i] + sine * local_z[i];
        rotated_z[i] = cosine * local_z[i] - sine * local_x[i];
    }
#define DRAW_AREA_BOX_EDGE(a, b, y0, y1)                                                                               \
    AiRndrLine3dDbg(position->x + rotated_x[a], y0, position->z + rotated_z[a], position->x + rotated_x[b], y1,        \
                    position->z + rotated_z[b], colour)
    DRAW_AREA_BOX_EDGE(0, 1, position->y, position->y);
    DRAW_AREA_BOX_EDGE(1, 2, position->y, position->y);
    DRAW_AREA_BOX_EDGE(2, 3, position->y, position->y);
    DRAW_AREA_BOX_EDGE(3, 0, position->y, position->y);
    DRAW_AREA_BOX_EDGE(0, 0, position->y, position->y + size->y);
    DRAW_AREA_BOX_EDGE(1, 1, position->y, position->y + size->y);
    DRAW_AREA_BOX_EDGE(2, 2, position->y, position->y + size->y);
    DRAW_AREA_BOX_EDGE(3, 3, position->y, position->y + size->y);
    DRAW_AREA_BOX_EDGE(0, 1, position->y + size->y, position->y + size->y);
    DRAW_AREA_BOX_EDGE(1, 2, position->y + size->y, position->y + size->y);
    DRAW_AREA_BOX_EDGE(2, 3, position->y + size->y, position->y + size->y);
    DRAW_AREA_BOX_EDGE(3, 0, position->y + size->y, position->y + size->y);
#undef DRAW_AREA_BOX_EDGE
}

void __cdecl DrawAreaCylinder(NUVEC *centre, NUVEC *size, i32 colour) {
    LocaledbitsDrawSolidCircleXY(centre, size->x < size->z ? size->x : size->z, centre->y, centre->y + size->y, colour,
                                 0, 16);
}

void __cdecl areaEditorDrawAreas(void) {
    f32 previous_distance = AiRndrCullDistance;
    EDAIAREA_s *area;
    i32 colour;
    AiRndrCullDistance = 0.0f;
    for (area = (EDAIAREA_s *)NuLinkedListGetHead(&aieditor->areas); area != NULL;
         area = (EDAIAREA_s *)NuLinkedListGetNext(&aieditor->areas, &area->link)) {
        if (area == aieditor->selected_area) {
            colour = 0xff0000ff;
            if (area != aieditor->hovered_area)
                colour = 0x800000ff;
        } else {
            colour = area == aieditor->hovered_area ? 0xffffffff : 0;
        }
        if ((area->flags & 1) != 0) {
            f32 radius = area->size.x < area->size.z ? area->size.x : area->size.z;
            LocaledbitsDrawSolidCircleXY(&area->position, radius, area->position.y, area->size.y + area->position.y,
                                         colour, 0, 16);
        } else {
            DrawAreaBox(&area->position, &area->size, (i16)area->rotation, colour);
        }
    }
    AiRndrCullDistance = previous_distance;
}

void __cdecl areaEditor_Enter(void) {
    i32 i;
    NULISTHDR *list = &aieditor->areas;
    list->head = NULL;
    list->tail = NULL;
    for (i = 0; i < 64; ++i)
        NuLinkedListAppend(&aieditor->free_areas, &aieditor->area_pool[i].link);
    if (aieditor->ai_system != NULL) {
        for (i = 0; i < aieditor->ai_system->area_count; ++i) {
            AIAREA_PC *source = &aieditor->ai_system->areas[i];
            EDAIAREA_s *area = areaEditorCreateArea(&source->position, (NUVEC *)&source->half_width, source->rotation,
                                                    (i8)source->game_flags);
            NuStrNCpy(area->name, source->name, 16);
        }
    }
}

static EDLOCATOR_s *AiEditorCreateLocatorPc(const NUVEC *position, i32 direction) {
    EDLOCATOR_s *locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->free_locators);
    if (locator != NULL) {
        NuLinkedListRemove(&aieditor->free_locators, &locator->link);
        NuLinkedListAppend(&aieditor->locators, &locator->link);
        locator->position = *position;
        locator->direction = direction;
    }
    return locator;
}

static EDLOCATORSET_s *AiEditorCreateLocatorSetPc(void) {
    EDLOCATORSET_s *set = (EDLOCATORSET_s *)NuLinkedListGetHead(&aieditor->free_locator_sets);
    if (set != NULL) {
        NuLinkedListRemove(&aieditor->free_locator_sets, &set->link);
        memset(set, 0, sizeof(*set));
        NuLinkedListAppend(&aieditor->locator_sets, &set->link);
    }
    return set;
}

static void AiEditorDestroyLocatorSetPc(EDLOCATORSET_s *set) {
    if (set != NULL) {
        NuLinkedListRemove(&aieditor->locator_sets, &set->link);
        memset(set, 0, sizeof(*set));
        NuLinkedListAppend(&aieditor->free_locator_sets, &set->link);
    }
}

static i32 AiEditorRemoveLocatorFromSetPc(EDLOCATORSET_s *set, EDLOCATOR_s *locator) {
    i32 index;
    if (set == NULL || locator == NULL)
        return 0;
    for (index = 0; index < 64; ++index) {
        if (set->locators[index] == NULL)
            return 0;
        if (set->locators[index] == locator)
            break;
    }
    if (index == 64)
        return 0;
    for (; index < 64; ++index) {
        if (index + 1 < 64)
            set->locators[index] = set->locators[index + 1];
        else
            set->locators[index] = NULL;
    }
    return 1;
}

static i32 AiEditorAddLocatorToSetPc(EDLOCATORSET_s *set, EDLOCATOR_s *locator, EDLOCATOR_s *before) {
    i32 index;
    i32 move;
    if (set == NULL || locator == NULL || set->locators[63] != NULL)
        return 0;
    if (set->locators[0] != NULL && set->locators[0]->path_check.path != locator->path_check.path)
        return 0;
    for (index = 0; index < 64; ++index) {
        if (set->locators[index] == NULL)
            break;
        if (set->locators[index] == locator) {
            AiEditorRemoveLocatorFromSetPc(set, locator);
            break;
        }
    }
    if (before != NULL) {
        for (index = 0; index < 64; ++index) {
            if (set->locators[index] == before) {
                for (move = 62; move >= index; --move)
                    set->locators[move + 1] = set->locators[move];
                set->locators[index + 1] = locator;
                return 1;
            }
        }
    }
    for (index = 0; index < 64; ++index) {
        if (set->locators[index] == NULL) {
            set->locators[index] = locator;
            if (index + 1 < 64)
                set->locators[index + 1] = NULL;
            return 1;
        }
    }
    return 0;
}

/* Native 521c50..521ca4: private name argument EDI. */
static __declspec(noinline) EDLOCATORSET_s *locatorEditor_FindLocatorSet(char *name) {
    EDLOCATORSET_s *set;
    for (set = (EDLOCATORSET_s *)NuLinkedListGetHead(&aieditor->locator_sets); set != NULL;
         set = (EDLOCATORSET_s *)NuLinkedListGetNext(&aieditor->locator_sets, &set->link)) {
        if (NuStrICmp(name, set->name) == 0)
            return set;
    }
    return NULL;
}

void locatorEditorSaveData(AIPATHSYS_PC *path_system) {
    i32 locator_count = 0;
    EDLOCATOR_s *locator;
    for (locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators); locator != NULL;
         locator = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &locator->link)) {
        locator->runtime_index = 0xff;
        if (locator->path_check.path != NULL)
            locator->runtime_index = locator_count++;
    }
    EdFileWriteInt(locator_count);
    for (locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators); locator != NULL;
         locator = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &locator->link)) {
        AIPATH_PC *path;
        i32 connection_index;
        i32 connection_direction;
        i32 index;
        if (locator->path_check.path == NULL)
            continue;
        EdFileWrite(locator->name, 16);
        EdFileWriteFloat(locator->position.x);
        EdFileWriteFloat(locator->position.y);
        EdFileWriteFloat(locator->position.z);
        EdFileWriteShort((u16)locator->direction);
        EdFileWriteChar((u8)locator->path_check.path->draw_index);
        path = path_system->paths[locator->path_check.path->draw_index];
        connection_index = 0;
        connection_direction = 0;
        for (index = 0; index < path->connection_count; ++index) {
            AIPATHCNX_PC *connection = &path->connections[index];
            if (connection->node_indices[0] == locator->path_check.first->index &&
                connection->node_indices[1] == locator->path_check.second->index) {
                connection_index = index;
                connection_direction = abs(locator->path_check.angle) >= 0x4000;
                break;
            }
            if (connection->node_indices[0] == locator->path_check.second->index &&
                connection->node_indices[1] == locator->path_check.first->index) {
                connection_index = index;
                connection_direction = abs(locator->path_check.angle) < 0x4000;
                break;
            }
        }
        EdFileWriteChar(connection_direction);
        EdFileWriteShort(connection_index);
        EdFileWriteFloat(locator->path_check.fraction);
        EdFileWriteFloat(locator->path_check.width);
        if (aidata_version > 14)
            EdFileWriteInt(locator->path_check.angle);
    }
    if (aidata_version > 17) {
        i32 set_count = 0;
        EDLOCATORSET_s *set;
        for (set = (EDLOCATORSET_s *)NuLinkedListGetHead(&aieditor->locator_sets); set != NULL;
             set = (EDLOCATORSET_s *)NuLinkedListGetNext(&aieditor->locator_sets, &set->link))
            ++set_count;
        EdFileWriteInt(set_count);
        if (set_count == 0)
            return;
        for (set = (EDLOCATORSET_s *)NuLinkedListGetHead(&aieditor->locator_sets); set != NULL;
             set = (EDLOCATORSET_s *)NuLinkedListGetNext(&aieditor->locator_sets, &set->link)) {
            i32 member_count = 0;
            i32 index;
            for (index = 0; index < 64 && set->locators[index] != NULL; ++index) {
                if (set->locators[index]->runtime_index != 0xff)
                    ++member_count;
            }
            EdFileWrite(set->name, 16);
            EdFileWriteInt(member_count);
            if (member_count != 0) {
                for (index = 0; index < 64 && set->locators[index] != NULL; ++index) {
                    if (set->locators[index]->runtime_index != 0xff)
                        EdFileWriteChar((u8)set->locators[index]->runtime_index);
                }
            }
        }
    }
}

EDLOCATOR_s *__cdecl locatorEditor_GetNearest(i32 use_width) {
    EDLOCATOR_s *nearest = NULL;
    f32 nearest_distance = FLT_MAX;
    EDLOCATOR_s *locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators);
    while (locator != NULL) {
        NUVEC delta;
        f32 distance = NuVecXZDistSqr(&aieditor->camera_position, &locator->position, &delta);
        if (distance < nearest_distance) {
            if (use_width != 0) {
                NuVecRotateY(&delta, &delta, -locator->direction);
                if (delta.x < aiEditor_LocatorWidth && delta.y < aiEditor_LocatorWidth &&
                    delta.z < aiEditor_LocatorWidth) {
                    f32 negative_width = -aiEditor_LocatorWidth;
                    if (delta.x > negative_width && delta.y > negative_width && delta.z > negative_width) {
                        nearest = locator;
                        nearest_distance = distance;
                    }
                }
            } else {
                nearest = locator;
                nearest_distance = distance;
            }
        }
        locator = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &locator->link);
    }
    return nearest;
}

void __cdecl locatorEditor_PathNodeMoved(EDAIPATHNODE_s *node) {
    EDLOCATOR_s *locator;
    NUVEC difference;
    NUVEC direction;
    NUVEC movement;
    f32 radius;
    f32 normalized_x;
    i32 angle;

    for (locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators); locator != NULL;
         locator = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &locator->link)) {
        if (locator->path_check.first == node || locator->path_check.second == node) {
            NuVecSub(&difference, &locator->path_check.second->position, &locator->path_check.first->position);
            NuVecNorm(&direction, &difference);
            if (locator->path_check.fraction > 1.0f) {
                radius = locator->path_check.second->radius;
            } else if (locator->path_check.fraction < 0.0f) {
                radius = locator->path_check.first->radius;
            } else {
                radius = (1.0f - locator->path_check.fraction) * locator->path_check.first->radius +
                         locator->path_check.second->radius * locator->path_check.fraction;
            }
            normalized_x = direction.x;
            direction.x = direction.z * radius;
            direction.z = -normalized_x * radius;
            locator->position = locator->path_check.first->position;
            NuVecScale(&movement, &difference, locator->path_check.fraction);
            NuVecAdd(&locator->position, &locator->position, &movement);
            NuVecScale(&movement, &direction, locator->path_check.width);
            NuVecAdd(&locator->position, &locator->position, &movement);
            angle = (i32)(NuAtan2(difference.x, difference.z) * 10430.378f);
            locator->direction = NuAngAdd(angle, locator->path_check.angle);
        }
    }
}

static void __cdecl AiEditorRenameLocatorPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDLOCATOR_s *other;
    (void)menu;
    (void)event;
    if (aieditor->current_locator != NULL) {
        char *name = ((edui_textpicker_s *)item)->value;
        if (strlen(name) != 0) {
            for (other = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators); other != NULL;
                 other = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &other->link)) {
                if (NuStrICmp(other->name, name) == 0)
                    return;
            }
            strcpy(aieditor->current_locator->name, name);
        }
    }
}

static void __cdecl AiEditorRenameLocatorCancelPc(eduimenu_s *menu, eduimenu_s *child) {
    (void)child;
    eduiMenuDestroy(menu);
}

static void __cdecl AiEditorRenameLocatorSetPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDLOCATORSET_s *other;
    char *name;
    (void)menu;
    (void)event;
    if (aieditor->current_locator_set == NULL)
        return;
    name = ((edui_textpicker_s *)item)->value;
    if (strlen(name) == 0 || NuStrICmp("NONE", name) == 0)
        return;
    for (other = (EDLOCATORSET_s *)NuLinkedListGetHead(&aieditor->locator_sets); other != NULL;
         other = (EDLOCATORSET_s *)NuLinkedListGetNext(&aieditor->locator_sets, &other->link)) {
        if (NuStrICmp(other->name, name) == 0)
            return;
    }
    strcpy(aieditor->current_locator_set->name, name);
}

static void __cdecl AiEditorSetPendingLocatorNamePc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    AIEDITOR_RENDER_STATE *state = aieditor;
    (void)menu;
    (void)event;
    if (state->current_locator_set != NULL) {
        char *name = ((edui_textpicker_s *)item)->value;
        if (strlen(name) != 0)
            NuStrNCpy(state->pending_locator_name, name, 16);
    }
}

void __cdecl locatorEditor_cbCancelSelectLocatorSet(eduimenu_s *menu, eduimenu_s *child) {
    (void)child;
    eduiMenuDestroy(menu);
}

static EDLOCATOR_s *AiEditorNextLocatorInSetPc(EDLOCATORSET_s *set, EDLOCATOR_s *current) {
    i32 index;
    if (set == NULL)
        return NULL;
    if (current != NULL) {
        for (index = 0; index < 64; ++index) {
            if (set->locators[index] == current && index + 1 < 64 && set->locators[index + 1] != NULL)
                return set->locators[index + 1];
        }
    }
    return set->locators[0];
}

static EDLOCATOR_s *AiEditorPrevLocatorInSetPc(EDLOCATORSET_s *set, EDLOCATOR_s *current) {
    i32 index;
    if (set == NULL)
        return NULL;
    if (current != NULL) {
        for (index = 0; index < 64; ++index) {
            if (set->locators[index] == current) {
                if (index != 0)
                    return set->locators[index - 1];
                break;
            }
        }
        for (index = 63; index >= 0; --index) {
            if (set->locators[index] != NULL)
                return set->locators[index];
        }
    }
    return set->locators[0];
}

void __cdecl DrawLocator(NUVEC *position, f32 width, i32 direction, i32 colour) {
    NUVEC offset;
    NUVEC centre;
    NUVEC arrow;
    NURND_VERTEX3D_PC vertices[2];
    vertices[0].colour = colour;
    vertices[1].colour = colour;
    centre = *position;
    centre.y += 0.25f;
    offset.x = width;
    offset.y = 0.0f;
    offset.z = 0.0f;
    NuVecRotateY(&offset, &offset, direction);
    NuVecAdd(&vertices[0].position, &centre, &offset);
    NuVecSub(&vertices[1].position, &centre, &offset);
    AiRndrLine3d(vertices, NULL, NULL);
    vertices[0].position.x = centre.x;
    vertices[0].position.y = centre.y - width;
    vertices[0].position.z = centre.z;
    vertices[1].position.x = centre.x;
    vertices[1].position.y = centre.y + width;
    vertices[1].position.z = centre.z;
    AiRndrLine3d(vertices, NULL, NULL);
    offset.x = 0.0f;
    offset.y = 0.0f;
    offset.z = width;
    NuVecRotateY(&offset, &offset, direction);
    NuVecAdd(&vertices[0].position, &centre, &offset);
    NuVecSub(&vertices[1].position, &centre, &offset);
    AiRndrLine3d(vertices, NULL, NULL);
    arrow.x = -offset.z * (f64)0.2f;
    arrow.y = 0.0f;
    arrow.z = offset.x * (f64)0.2f;
    NuVecAdd(&vertices[1].position, &centre, &arrow);
    AiRndrLine3d(vertices, NULL, NULL);
    NuVecSub(&vertices[1].position, &centre, &arrow);
    AiRndrLine3d(vertices, NULL, NULL);
}

void locatorEditorDrawLocators(void) {
    AIEDITOR_RENDER_STATE *const *state = &aieditor;
    EDLOCATOR_s *locator = (EDLOCATOR_s *)NuLinkedListGetHead(&(*state)->locators);
    while (locator != NULL) {
        locator->drawn = 0;
        locator = (EDLOCATOR_s *)NuLinkedListGetNext(&(*state)->locators, &locator->link);
    }
    if ((*state)->current_locator_set != NULL) {
        i32 index;
        for (index = 0; index < 64; ++index) {
            i32 colour;
            locator = (*state)->current_locator_set->locators[index];
            if (locator == NULL)
                break;

            if (locator == (*state)->current_locator) {
                colour = locator == (*state)->nearest_locator ? 0xff0000ff : 0x800000ff;
            } else {
                colour = locator == (*state)->nearest_locator ? -1 : 0x32323232;
            }
            DrawLocator(&locator->position, aiEditor_LocatorWidth, locator->direction, colour);
            locator->drawn = 1;
        }
    }
    locator = (EDLOCATOR_s *)NuLinkedListGetHead(&(*state)->locators);
    while (locator != NULL) {
        if (!locator->drawn) {
            i32 colour;
            if ((*state)->current_locator_set != NULL) {
                if (locator == (*state)->current_locator) {
                    colour = locator == (*state)->nearest_locator ? 0x50 : 0x28;
                } else {
                    colour = locator == (*state)->nearest_locator ? 0x64646464 : 0x32323232;
                }
            } else if (locator == (*state)->current_locator) {
                colour = locator == (*state)->nearest_locator ? 0xff0000ff : 0x800000ff;
            } else {
                colour = locator == (*state)->nearest_locator ? -1 : 0x32323232;
            }
            DrawLocator(&locator->position, aiEditor_LocatorWidth, locator->direction, colour);
            locator->drawn = 1;
        }
        locator = (EDLOCATOR_s *)NuLinkedListGetNext(&(*state)->locators, &locator->link);
    }
}

void __cdecl aieditor_ClearAllPathCnxTypes(void) {
    AIPathConnectionTypeCountPc = 0;
    memset(AIPathConnectionTypesPc, 0, sizeof(AIPathConnectionTypesPc));
}

void __cdecl aieditor_RegisterPathCnxType(const char *name, u32 connection_flag,
                                          void(__cdecl *draw)(NUVEC *, f32, NUVEC *, u32, i32), u32 flags) {
    i32 i, count;
    AI_PATH_CONNECTION_TYPE_PC *type;
    if (name != NULL && strlen(name) < 64 && connection_flag != 0 && (count = AIPathConnectionTypeCountPc) < 32) {
        for (i = 0; i < count; ++i) {
            if (AIPathConnectionTypesPc[i].connection_flag & connection_flag)
                AIScriptMissingStateDiagnostic();
        }
        type = &AIPathConnectionTypesPc[count];
        AIPathConnectionTypeCountPc = count + 1;
        strcpy(type->name, name);
        type->connection_flag = connection_flag;
        type->draw = draw;
        type->flags = flags;
        return;
    }
    AIScriptMissingStateDiagnostic();
}

EDAIPATH_s *__cdecl pathEditorAllocPathPc(void) {
    EDAIPATH_s *path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->free_paths);
    if (path != NULL) {
        NuLinkedListRemove(&aieditor->free_paths, &path->link);
        NuLinkedListAppend(&aieditor->paths, &path->link);
    }
    return path;
}

static __declspec(noinline) EDAIPATHNODE_s *pathEditor_CreateNode(EDAIPATH_s *path) {
    EDAIPATHNODE_s *node = NULL;
    if (path != NULL && path->node_count < 254) {
        node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&aieditor->free_path_nodes);
        if (node != NULL) {
            NuLinkedListRemove(&aieditor->free_path_nodes, &node->link);
            NuLinkedListAppend(&path->nodes, &node->link);
            ++path->node_count;
            node->position = aieditor->camera_position;
            node->radius = default_path_radius;
            node->lower_height = -default_path_heighttol;
            node->upper_height = default_path_heighttol;
        }
    }
    return node;
}

/* Native 522a00: private path EAX. */
static EDAIPATHNODE_s *pathEditor_GetNearestNode(EDAIPATH_s *path, i32 require_radius) {
    EDAIPATHNODE_s *nearest = NULL;
    f32 nearest_distance = FLT_MAX;
    EDAIPATHNODE_s *node;
    NUVEC offset;
    f32 distance;
    f32 height;
    f32 upper;
    f32 lower;

    if (path != NULL) {
        for (node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&path->nodes); node != NULL;
             node = (EDAIPATHNODE_s *)NuLinkedListGetNext(&path->nodes, &node->link)) {
            distance = NuVecXZDistSqr(&aieditor->cursor_position, &node->position, &offset);
            if (!(distance < nearest_distance)) {
                continue;
            }
            height = aieditor->cursor_position.y - node->position.y;
            upper = node->upper_height < 0.2f ? 0.2f : node->upper_height;
            lower = node->lower_height > -0.2f ? -0.2f : node->lower_height;
            if (!(height <= upper && height >= lower)) {
                continue;
            }
            if (require_radius && !(node->radius * node->radius > distance)) {
                continue;
            }
            nearest = node;
            nearest_distance = distance;
        }
    }
    return nearest;
}

static __declspec(noinline) EDAIPATHNODE_s *pathEditor_GetNearestPathNode(EDAIPATH_s **nearest_path, i32 require_radius,
                                                                          EDAIPATH_s *exclude) {
    EDAIPATH_s *path;
    EDAIPATH_s *best_path = NULL;
    EDAIPATHNODE_s *best_node = NULL;
    EDAIPATHNODE_s *node;
    NUVEC delta;
    f32 best = FLT_MAX;
    f32 distance;
    for (path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->paths); path != NULL;
         path = (EDAIPATH_s *)NuLinkedListGetNext(&aieditor->paths, &path->link)) {
        if (path == exclude)
            continue;
        node = pathEditor_GetNearestNode(path, require_radius);
        if (node == NULL)
            continue;
        distance = NuVecXZDistSqr(&aieditor->cursor_position, &node->position, &delta);
        if (distance < best) {
            best = distance;
            best_path = path;
            best_node = node;
        }
    }
    if (best_node != NULL && best_path != NULL) {
        if (nearest_path != NULL)
            *nearest_path = best_path;
        return best_node;
    }
    return NULL;
}

static __declspec(noinline) void pathEditor_SelectNearestPath(void) {
    EDAIPATH_s *path;
    EDAIPATHNODE_s *node = pathEditor_GetNearestPathNode(&path, 0, NULL);
    if (node != NULL) {
        aieditor->current_path = path;
        aieditor->current_path->current_node = node;
        edcamSetPos(&aieditor->current_path->current_node->position);
    }
}

static __declspec(noinline) i32 pathEditor_GetNearestRuntimeNode(AIPATH_PC *runtime) {
    i32 nearest = -1;
    i32 index;
    NUVEC delta;
    f32 best = FLT_MAX;
    f32 distance;
    AIPATHNODE_PC *node;
    if (runtime != NULL) {
        for (index = 0; index < runtime->node_count; ++index) {
            node = &runtime->nodes[index];
            distance = NuVecXZDistSqr(&aieditor->cursor_position, &node->position, &delta);
            if (distance < best && distance < node->radius_squared) {
                best = distance;
                nearest = index;
            }
        }
    }
    return nearest;
}

static __declspec(noinline) EDAIPATHCNX_s *pathEditor_FindConnection(EDAIPATHNODE_s *from, EDAIPATHNODE_s *to) {
    i32 index;
    if (from != NULL && to != NULL) {
        for (index = 0; index < 8; ++index) {
            if (from->connections[index].node == to)
                return &from->connections[index];
        }
    }
    return NULL;
}

static __declspec(noinline) i32 pathEditor_ConnectNodes(EDAIPATHNODE_s *node, EDAIPATHNODE_s *other) {
    i32 index;
    i32 other_index;
    if (node != NULL && other != NULL) {
        for (index = 0; index < 8; ++index) {
            if (node->connections[index].node != NULL)
                continue;
            for (other_index = 0; other_index < 8; ++other_index) {
                if (other->connections[other_index].node == NULL) {
                    node->connections[index].node = other;
                    node->connections[index].flags = 0;
                    other->connections[other_index].node = node;
                    other->connections[other_index].flags = 0;
                    return 1;
                }
            }
        }
    }
    return 0;
}

static __declspec(noinline) i32 pathEditor_DisconnectNodes(EDAIPATHNODE_s *node, EDAIPATHNODE_s *other) {
    i32 index;
    i32 other_index;
    if (node != NULL && other != NULL) {
        for (index = 0; index < 8; ++index) {
            if (node->connections[index].node != other)
                continue;
            for (other_index = 0; other_index < 8; ++other_index) {
                if (other->connections[other_index].node == node) {
                    memset(&node->connections[index], 0, sizeof(node->connections[index]));
                    memset(&other->connections[other_index], 0, sizeof(other->connections[other_index]));
                    return 1;
                }
            }
        }
    }
    return 0;
}

static void AiPathDrawSolidEdgePc(EDAIPATHNODE_s *first, EDAIPATHNODE_s *second, u32 colour) {
    NURND_VERTEX3D_PC vertices[2];
    NUVEC direction;
    f32 distance = NuVecXZDist(&first->position, &second->position, &direction);
    if (distance > 0.0f) {
        direction.x /= distance;
        direction.z /= distance;
    }
    if (first->radius == second->radius) {
        vertices[0].position = first->position;
        vertices[0].colour = colour;
        vertices[0].position.x += first->radius * direction.z;
        vertices[0].position.z -= first->radius * direction.x;
        vertices[1].position = second->position;
        vertices[1].colour = colour;
        vertices[1].position.x += second->radius * direction.z;
        vertices[1].position.z -= second->radius * direction.x;
        vertices[0].position.y = first->lower_height + first->position.y;
        vertices[1].position.y = second->lower_height + second->position.y;
        AiRndrLine3d(vertices, NULL, NULL);
        vertices[0].position.y = first->upper_height + first->position.y;
        vertices[1].position.y = second->upper_height + second->position.y;
        AiRndrLine3d(vertices, NULL, NULL);
        vertices[0].position = first->position;
        vertices[0].colour = colour;
        vertices[0].position.x -= first->radius * direction.z;
        vertices[0].position.z += first->radius * direction.x;
        vertices[1].position = second->position;
        vertices[1].colour = colour;
        vertices[1].position.x -= second->radius * direction.z;
        vertices[1].position.z += second->radius * direction.x;
        vertices[0].position.y = first->lower_height + first->position.y;
        vertices[1].position.y = second->lower_height + second->position.y;
        AiRndrLine3d(vertices, NULL, NULL);
        vertices[0].position.y = first->upper_height + first->position.y;
        vertices[1].position.y = second->upper_height + second->position.y;
        AiRndrLine3d(vertices, NULL, NULL);
    } else {
        NUVEC side;
        i16 angle = 0x4000 - PCPathArcSine((second->radius - first->radius) / distance);
        NuVecRotateY(&side, &direction, angle);
        vertices[0].position = first->position;
        vertices[0].colour = colour;
        vertices[0].position.x += first->radius * side.x;
        vertices[0].position.z += first->radius * side.z;
        vertices[1].position = second->position;
        vertices[1].colour = colour;
        vertices[1].position.x += second->radius * side.x;
        vertices[1].position.z += second->radius * side.z;
        vertices[0].position.y = first->lower_height + first->position.y;
        vertices[1].position.y = second->lower_height + second->position.y;
        AiRndrLine3d(vertices, NULL, NULL);
        vertices[0].position.y = first->upper_height + first->position.y;
        vertices[1].position.y = second->upper_height + second->position.y;
        AiRndrLine3d(vertices, NULL, NULL);
        NuVecRotateY(&side, &direction, -angle);
        vertices[0].position = first->position;
        vertices[0].colour = colour;
        vertices[0].position.x += first->radius * side.x;
        vertices[0].position.z += first->radius * side.z;
        vertices[1].position = second->position;
        vertices[1].colour = colour;
        vertices[1].position.x += second->radius * side.x;
        vertices[1].position.z += second->radius * side.z;
        vertices[0].position.y = first->lower_height + first->position.y;
        vertices[1].position.y = second->lower_height + second->position.y;
        AiRndrLine3d(vertices, NULL, NULL);
        vertices[0].position.y = first->upper_height + first->position.y;
        vertices[1].position.y = second->upper_height + second->position.y;
        AiRndrLine3d(vertices, NULL, NULL);
    }
}

static void AiPathDrawWireEdgePc(EDAIPATHNODE_s *first, EDAIPATHNODE_s *second, u32 colour) {
    NUVEC direction, side;
    NURND_VERTEX3D_PC vertices[2];
    f32 distance;
    i16 angle;

    distance = NuVecXZDist(&first->position, &second->position, &direction);
    if (distance > 0.0f) {
        direction.x /= distance;
        direction.z /= distance;
    }
    if (first->radius == second->radius) {
        vertices[0].position = first->position;
        vertices[0].colour = colour;
        vertices[0].position.x += first->radius * direction.z;
        vertices[0].position.y += aiEditor_DrawYOffset;
        vertices[0].position.z -= first->radius * direction.x;
        vertices[1].position = second->position;
        vertices[1].colour = colour;
        vertices[1].position.x += second->radius * direction.z;
        vertices[1].position.y += aiEditor_DrawYOffset;
        vertices[1].position.z -= second->radius * direction.x;
        AiRndrLine3d(vertices, NULL, NULL);
        vertices[0].position = first->position;
        vertices[0].colour = colour;
        vertices[0].position.x -= first->radius * direction.z;
        vertices[0].position.y += aiEditor_DrawYOffset;
        vertices[0].position.z += first->radius * direction.x;
        vertices[1].position = second->position;
        vertices[1].colour = colour;
        vertices[1].position.x -= second->radius * direction.z;
        vertices[1].position.y += aiEditor_DrawYOffset;
        vertices[1].position.z += second->radius * direction.x;
        AiRndrLine3d(vertices, NULL, NULL);
    } else {
        distance = (second->radius - first->radius) / distance;
        angle = 0x4000 - PCPathArcSine(distance);
        NuVecRotateY(&side, &direction, angle);
        vertices[0].position = first->position;
        vertices[0].colour = colour;
        vertices[0].position.x += first->radius * side.x;
        vertices[0].position.y += aiEditor_DrawYOffset;
        vertices[0].position.z += first->radius * side.z;
        vertices[1].position = second->position;
        vertices[1].colour = colour;
        vertices[1].position.x += second->radius * side.x;
        vertices[1].position.y += aiEditor_DrawYOffset;
        vertices[1].position.z += second->radius * side.z;
        AiRndrLine3d(vertices, NULL, NULL);
        NuVecRotateY(&side, &direction, -(i32)angle);
        vertices[0].position = first->position;
        vertices[0].colour = colour;
        vertices[0].position.x += first->radius * side.x;
        vertices[0].position.y += aiEditor_DrawYOffset;
        vertices[0].position.z += first->radius * side.z;
        vertices[1].position = second->position;
        vertices[1].colour = colour;
        vertices[1].position.x += second->radius * side.x;
        vertices[1].position.y += aiEditor_DrawYOffset;
        vertices[1].position.z += second->radius * side.z;
        AiRndrLine3d(vertices, NULL, NULL);
    }
}

static void AiPathDrawCrossPc(NUVEC *position, f32 radius, u32 colour) {
    NURND_VERTEX3D_PC vertices[2];
    vertices[0].colour = colour;
    vertices[1].colour = colour;
    vertices[0].position = *position;
    vertices[1].position = *position;
    vertices[0].position.x += radius;
    vertices[1].position.x -= radius;
    AiRndrLine3d(vertices, NULL, NULL);
    vertices[0].position = *position;
    vertices[1].position = *position;
    vertices[0].position.z += radius;
    vertices[1].position.z -= radius;
    AiRndrLine3d(vertices, NULL, NULL);
}

void __cdecl pathEditorDrawNode(NUVEC *position, f32 radius, f32 lower_height, f32 upper_height, u32 colour,
                                i32 material, i32 segments, i32 solid) {
    NUVEC centre = {position->x, position->y + aiEditor_DrawYOffset, position->z};
    if (solid != 0) {
        LocaledbitsDrawSolidCircleXY(&centre, radius, lower_height, upper_height, colour, material, segments);
    } else {
        LocaledbitsDrawCircleXY(&centre, radius, colour, material, segments);
    }
}

void __cdecl pathEditorDrawConnectionInfo(NUVEC *position, f32 radius, NUVEC *other, u32 flags, i32 colour) {
    i32 i;
    NUVEC start, end, direction;
    f32 length;
    NURND_VERTEX3D_PC vertices[2];
    if (flags == 0) {
        return;
    }
    start.x = position->x;
    start.y = position->y + aiEditor_DrawYOffset;
    start.z = position->z;
    end.x = other->x;
    end.y = other->y + aiEditor_DrawYOffset;
    end.z = other->z;
    for (i = 0; i < AIPathConnectionTypeCountPc; ++i) {
        AI_PATH_CONNECTION_TYPE_PC *type = &AIPathConnectionTypesPc[i];
        if (type->draw != NULL && (type->connection_flag & flags) != 0) {
            type->draw(&start, radius, &end, flags, colour);
            return;
        }
    }
    length = NuVecXZDist(&end, &start, &direction);
    if (length > 0.0f) {
        direction.x /= length;
        direction.z /= length;
    }
    vertices[0].position = start;
    vertices[1].position = end;
    vertices[0].colour = colour;
    vertices[1].colour = colour;
    AiRndrLine3d(vertices, NULL, NULL);
    NuVecScale(&vertices[0].position, &direction, radius * 0.5);
    vertices[0].position.y = 0.0f;
    NuVecAdd(&vertices[0].position, &vertices[0].position, &start);
    vertices[1].position = start;
    vertices[1].position.x += radius * direction.z * 0.5;
    vertices[1].position.z -= radius * direction.x * 0.5;
    AiRndrLine3d(vertices, NULL, NULL);
    vertices[1].position = start;
    vertices[1].position.x -= radius * direction.z * 0.5;
    vertices[1].position.z += radius * direction.x * 0.5;
    AiRndrLine3d(vertices, NULL, NULL);
    if (flags & 0x40000000) {
        NuVecScale(&vertices[0].position, &direction, radius * 0.5);
        vertices[0].position.y = 0.0f;
        NuVecAdd(&vertices[0].position, &vertices[0].position, &start);
        vertices[1].position = vertices[0].position;
        vertices[0].position.x += radius * direction.z * 0.5;
        vertices[0].position.z -= radius * direction.x * 0.5;
        vertices[1].position.x -= radius * direction.z * 0.5;
        vertices[1].position.z += radius * direction.x * 0.5;
        AiRndrLine3d(vertices, NULL, NULL);
    } else if (flags & 0x80000000) {
        NUVEC centre = vertices[0].position;
        vertices[0].colour = colour / 2;
        vertices[1].colour = colour / 2;
        vertices[1].position = centre;
        vertices[1].position.x += radius * direction.z * 0.25;
        vertices[1].position.z -= radius * direction.x * 0.25;
        AiRndrLine3d(vertices, NULL, NULL);
        vertices[1].position = centre;
        vertices[1].position.x -= radius * direction.z * 0.25;
        vertices[1].position.z += radius * direction.x * 0.25;
        AiRndrLine3d(vertices, NULL, NULL);
    } else if (flags & 0x20000000) {
        NUVEC centre = vertices[0].position;
        NuVecScale(&vertices[0].position, &direction, radius);
        vertices[0].position.y = 0.0f;
        NuVecAdd(&vertices[0].position, &vertices[0].position, position);
        vertices[1].position = centre;
        vertices[1].position.x += radius * direction.z * (f64)0.15f;
        vertices[1].position.z -= radius * direction.x * (f64)0.15f;
        AiRndrLine3d(vertices, NULL, NULL);
        vertices[1].position = centre;
        vertices[1].position.x -= radius * direction.z * 0.25;
        vertices[1].position.z += radius * direction.x * 0.25;
        AiRndrLine3d(vertices, NULL, NULL);
        vertices[1].position = centre;
        vertices[1].position.x -= radius * direction.z * (f64)0.35f;
        vertices[1].position.z += radius * direction.x * (f64)0.35f;
        AiRndrLine3d(vertices, NULL, NULL);
    }
}

static void pathEditorDrawPath(EDAIPATH_s *path, i32 path_index) {
    u8 drawn_connections[255][32];
    i16 mode;
    i32 solid;
    u32 active_route = 0;
    u32 colour;
    EDAIPATHNODE_s *node;
    memset(drawn_connections, 0, sizeof(drawn_connections));
    mode = aieditorsettings.modes.current;
    solid = (aieditorsettings.display_options & 0x80) &&
            (mode == AiEditorPathsModeIdPc || mode == AiEditorCreaturesModeIdPc || mode == AiEditorLocatorsModeIdPc);
    if (mode == AiEditorRouteModeIdPc) {
        EDAIPATH_s *selected = aieditor->current_path;
        u8 shift = selected->current_route != NULL
                       ? (u8)((i32)((u32)selected->current_route - (u32)selected - offsetof(EDAIPATH_s, routes)) >> 5)
                       : 0;
        active_route = shift < 64 ? (u32)((u64)1 << shift) : 0;
    }
    colour = path == aieditor->current_path
                 ? 0xffffffff
                 : *(u32 *)((u32)AiEditorPathPaletteObservedPc + 16u + (u32)(path_index % 5) * sizeof(u32));
    if (path != NULL) {
        for (node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&path->nodes); node != NULL;
             node = (EDAIPATHNODE_s *)NuLinkedListGetNext(&path->nodes, &node->link)) {
            i32 first_index;
            i32 segments;
            u32 node_colour;
            i32 slot;
            f32 override_height = AiEditorUpperHeightOverridePc;
            if (override_height != 0.0f)
                node->upper_height = override_height;
            override_height = AiEditorLowerHeightOverridePc;
            if (override_height != 0.0f)
                node->lower_height = override_height;
            first_index = node->index;
            if (path == aieditor->current_path) {
                if (node == path->current_node) {
                    node_colour = 0xff0000ff;
                    segments = 32;
                } else if (node == path->other_node) {
                    node_colour = 0xff00ffff;
                    segments = 32;
                } else {
                    node_colour = colour;
                    segments = 8;
                }
            } else {
                node_colour = colour;
                segments = 8;
            }
            pathEditorDrawNode(&node->position, node->radius, node->lower_height + node->position.y,
                               node->upper_height + node->position.y, node_colour, 0, segments, solid);
            if (node->shared_node != NULL && !(node->shared_node->draw_flags & 1)) {
                AiPathDrawCrossPc(&node->position, node->radius, node_colour);
                ((AI_PATH_DRAW_FLAGS_PC *)node->shared_node)->flags |= 1;
            }
            if (node->route_mask & active_route) {
                if (path == aieditor->current_path && node == path->current_node) {
                    pathEditorDrawNode(&node->position, node->radius * (f64)0.9f, node->lower_height + node->position.y,
                                       node->upper_height + node->position.y, 0xff0000ff, 0, 32, solid);
                } else if (path == aieditor->current_path && node == path->other_node) {
                    pathEditorDrawNode(&node->position, node->radius * (f64)0.9f, node->lower_height + node->position.y,
                                       node->upper_height + node->position.y, 0xff00ffff, 0, 32, solid);
                } else {
                    pathEditorDrawNode(&node->position, node->radius * (f64)0.9f, node->lower_height + node->position.y,
                                       node->upper_height + node->position.y, colour, 0, 8, solid);
                }
            }
            for (slot = 0; slot < 8; ++slot) {
                EDAIPATHNODE_s *other = node->connections[slot].node;
                i32 second_index;
                u32 connection_colour;
                u32 edge_colour;
                u32 route_colour;
                if (other == NULL) {
                    continue;
                }
                second_index = other->index;
                connection_colour = colour;
                if (node == path->current_node && other == path->other_node) {
                    connection_colour = 0xff00ffff;
                }
                pathEditorDrawConnectionInfo(&node->position, node->radius, &other->position,
                                             node->connections[slot].flags, connection_colour);
                if (*((u8 *)((u32)drawn_connections + (u32)first_index * 32u + (u32)(second_index / 8))) &
                    (1u << ((u32)(second_index % 8) & 31u))) {
                    continue;
                }
                *((u8 *)((u32)drawn_connections + (u32)first_index * 32u + (u32)(second_index / 8))) |=
                    1u << ((u32)(second_index % 8) & 31u);
                *((u8 *)((u32)drawn_connections + (u32)second_index * 32u + (u32)(first_index / 8))) |=
                    1u << ((u32)(first_index % 8) & 31u);
                edge_colour = colour;
                if (path == aieditor->current_path) {
                    if (((node == path->current_node && node->connections[slot].node == path->other_node) ||
                         (node == path->other_node && node->connections[slot].node == path->current_node))) {
                        edge_colour = 0xff00ffff;
                    } else if (node == path->current_node || node->connections[slot].node == path->current_node) {
                        edge_colour = 0xff0000ff;
                    }
                }
                if (solid) {
                    AiPathDrawSolidEdgePc(node, node->connections[slot].node, edge_colour);
                } else {
                    AiPathDrawWireEdgePc(node, node->connections[slot].node, edge_colour);
                }
                route_colour = 0xffff0000;
                if ((node == path->current_node && node->connections[slot].node == path->other_node) ||
                    (node == path->other_node && node->connections[slot].node == path->current_node)) {
                    route_colour = 0xff00ffff;
                }
                if (node->connections[slot].route_mask & active_route) {
                    NURND_VERTEX3D_PC route_vertices[2];
                    route_vertices[0].position = node->position;
                    route_vertices[0].colour = route_colour;
                    route_vertices[0].position.y += aiEditor_DrawYOffset + (f64)0.02f;
                    route_vertices[1].position = node->connections[slot].node->position;
                    route_vertices[1].colour = route_colour;
                    route_vertices[1].position.y += aiEditor_DrawYOffset + (f64)0.02f;
                    AiRndrLine3d(route_vertices, NULL, NULL);
                }
            }
        }
    }
}

void pathEditorDrawPaths(void) {
    EDAIPATHWALL_s *wall = (EDAIPATHWALL_s *)NuLinkedListGetHead(&aieditor->path_walls);
    while (wall != NULL) {
        ((AI_PATH_DRAW_FLAGS_PC *)wall)->flags &= 0xfffe;
        wall = (EDAIPATHWALL_s *)NuLinkedListGetNext(&aieditor->path_walls, &wall->link);
    }

    if ((aieditorsettings.display_options & 2)) {
        i32 index = 0;
        EDAIPATH_s *path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->paths);
        while (path != NULL) {
            path->draw_index = (i8)index++;
            path = (EDAIPATH_s *)NuLinkedListGetNext(&aieditor->paths, &path->link);
        }
        if (aieditor->current_path != NULL) {
            pathEditorDrawPath(aieditor->current_path, aieditor->current_path->draw_index);
        }
        path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->paths);
        while (path != NULL) {
            if (path != aieditor->current_path) {
                pathEditorDrawPath(path, path->draw_index);
            }
            path = (EDAIPATH_s *)NuLinkedListGetNext(&aieditor->paths, &path->link);
        }
    } else {
        pathEditorDrawPath(aieditor->current_path, 0);
    }
}

/* Native 5242b0..52431a. */
static __declspec(noinline) void pathEditor_CalcNodeIXs(void) {
    EDAIPATH_s *path;
    EDAIPATHNODE_s *node;
    i32 index;
    for (path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->paths); path != NULL;
         path = (EDAIPATH_s *)NuLinkedListGetNext(&aieditor->paths, &path->link)) {
        index = 0;
        for (node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&path->nodes); node != NULL;
             node = (EDAIPATHNODE_s *)NuLinkedListGetNext(&path->nodes, &node->link)) {
            node->index = (i16)index++;
        }
    }
}

void pathEditor_UpdateNodesOnPlatforms(void) {
    EDAIPATH_s *path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->paths);
    while (path != NULL) {
        EDAIPATHNODE_s *node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&path->nodes);
        while (node != NULL) {
            if (NuSpecialExistsFn(&node->special)) {
                NuVecMtxTransform(&node->position, &node->special_position, NuSpecialGetDrawMtx(&node->special));
            }
            node = (EDAIPATHNODE_s *)NuLinkedListGetNext(&path->nodes, &node->link);
        }
        path = (EDAIPATH_s *)NuLinkedListGetNext(&aieditor->paths, &path->link);
    }
}

/* Native 0x5243b0: geometric outputs only; validation belongs to the caller. */
static void TestPointPathCheck(NUVEC *point, EDAIPATHNODE_s *first, EDAIPATHNODE_s *second, f32 *fraction, f32 *width,
                               i32 *angle, f32 tolerance) {
    NUVEC direction;
    f32 distance;
    NUVEC relative;
    f32 along;
    NUVEC nearest;
    f32 radius;
    NUVEC radial;
    f32 distance_squared;
    NuVecSub(&direction, &second->position, &first->position);
    distance = NuVecMag(&direction);
    NuVecScale(&direction, &direction, 1.0f / distance);
    NuVecSub(&relative, point, &first->position);
    along = NuVecDot(&direction, &relative);
    *fraction = along / distance;
    if (*fraction <= 0.0f) {
        nearest = first->position;
        radius = first->radius;
    } else if (*fraction >= 1.0f) {
        nearest = second->position;
        radius = second->radius;
    } else {
        NuVecScale(&nearest, &direction, along);
        NuVecAdd(&nearest, &nearest, &first->position);
        radius = first->radius * (1.0f - *fraction) + second->radius * *fraction;
    }
    distance_squared = NuVecXZDistSqr(&nearest, point, &radial);
    radius += tolerance;
    *angle = (i32)(NuAtan2(direction.x, direction.z) * AIPathAngularScale);
    NuVecRotateY(&relative, &relative, (i32)(0u - (u32)*angle));
    if (*fraction <= 0.0f) {
        f32 beyond = -*fraction * distance;
        *width = distance_squared - beyond * beyond;
        *width = NuFsqrt(*width) / radius;
    } else if (*fraction >= 1.0f) {
        f32 beyond = (*fraction - 1.0) * distance;
        *width = distance_squared - beyond * beyond;
        *width = NuFsqrt(*width) / radius;
    } else {
        *width = NuFsqrt(distance_squared) / radius;
    }
    if (relative.x < 0.0f)
        *width = -*width;
}

/* Native 0x5245c0: establish a temporary runtime connection, validate it,
 * then calculate editor-space outputs. Ordinary static compiler-private ABI. */
static i32 TestPointWithinPathCheckPc(NUVEC *point, EDAIPATHNODE_s *first, EDAIPATHNODE_s *second, f32 *fraction,
                                      f32 *width, i32 *angle, f32 tolerance) {
    AIPATH_PC path;
    AIPATHCNX_PC connection;
    AIPATHNODE_PC nodes[2];
    AIPATHINFO_PC info;
    NUVEC horizontal;
    memset(&path, 0, sizeof(path));
    memset(&connection, 0, sizeof(connection));
    memset(nodes, 0, sizeof(nodes));
    memset(&info, 0, sizeof(info));
    nodes[0].position = first->position;
    nodes[0].radius = first->radius + tolerance;
    nodes[0].min_height = first->position.y + first->lower_height - tolerance;
    nodes[0].max_height = first->position.y + first->upper_height + tolerance;
    nodes[1].position = second->position;
    nodes[1].radius = second->radius + tolerance;
    nodes[1].min_height = second->position.y + second->lower_height - tolerance;
    nodes[1].max_height = second->position.y + second->upper_height + tolerance;
    path.nodes = nodes;
    path.connections = &connection;
    path.node_count = 2;
    path.connection_count = 1;
    connection.node_indices[0] = 1;
    connection.horizontal_distance = NuVecXZDist(&first->position, &second->position, &horizontal);
    if (connection.horizontal_distance == 0.0f)
        connection.horizontal_distance = 0.0001f;
    connection.rotation = (i16)(NuAtan2(horizontal.x, horizontal.z) * AIPathAngularScale);
    if (!WithinConnection(NULL, point, &path, &connection, 1, NULL, 0xff, 0xff, &info, 0.0f, 0))
        return 0;
    TestPointPathCheck(point, first, second, fraction, width, angle, tolerance);
    return 1;
}

void __cdecl pathEditor_OnPathCheck(NUVEC *point, EDAIPATHCHECK_s *result, EDAIPATH_s *path, f32 tolerance) {
    u8 checked[255][32] = {0};
    EDAIPATHNODE_s *node;
    memset(result, 0, sizeof(*result));
    if (path == NULL)
        return;
    node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&path->nodes);
    while (node != NULL) {
        i32 node_index = node->index;
        i32 slot;
        for (slot = 0; slot < 8; ++slot) {
            EDAIPATHNODE_s *other = node->connections[slot].node;
            i32 other_index;
            if (other == NULL)
                continue;
            other_index = other->index;
            if (!(checked[node_index][other_index / 8] & (1 << (other_index % 8)))) {
                f32 fraction;
                f32 width;
                i32 angle;
                checked[node_index][other_index / 8] |= 1 << (other_index % 8);
                checked[other_index][node_index / 8] |= 1 << (node_index % 8);
                if (TestPointWithinPathCheckPc(point, node, other, &fraction, &width, &angle, tolerance)) {
                    result->fraction = fraction;
                    result->width = width;
                    result->on_path = 1;
                    result->path = path;
                    result->angle = angle;
                    result->first = node;
                    result->second = node->connections[slot].node;
                }
            }
        }
        node = (EDAIPATHNODE_s *)NuLinkedListGetNext(&path->nodes, &node->link);
    }
}

/* Native 524b00..524b62. */
EDAIPATH_s *__cdecl pathEditor_GetPath(char *name) {
    EDAIPATH_s *path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->paths);
    if (name != NULL) {
        for (;;) {
            if (path == NULL)
                break;
            if (NuStrICmp(name, path->name) == 0)
                return path;
            path = (EDAIPATH_s *)NuLinkedListGetNext(&aieditor->paths, &path->link);
        }
    }
    return aieditor->current_path;
}

static void pathEditorDrawRoute(void) {
    AIPATH_PC *path;
    AIPATHCNX_PC *connection;
    AIPATHNODE_PC *node;
    AIPATHNODE_PC *next_node;
    i32 current;
    i32 end;
    i32 next;
    u32 edge;
    NURND_VERTEX3D_PC vertices[2];

    if (aieditor->cached_path_system == NULL)
        return;
    path = aieditor->cached_path_system->paths[0];
    if (path == NULL)
        return;
    if (aieditor->current_path->runtime_start < 0 || aieditor->current_path->runtime_start >= path->node_count ||
        aieditor->current_path->runtime_end < 0 || aieditor->current_path->runtime_end >= path->node_count)
        return;
    end = aieditor->current_path->runtime_end;
    current = aieditor->current_path->runtime_start;
    if (current == end)
        return;
    do {
        edge = path->route_matrix[current][end];
        if (edge == 0xff)
            break;
        connection = path->nodes[current].connections[edge];
        next = connection->node_indices[0];
        if (next == current)
            next = connection->node_indices[1];
        node = &path->nodes[current];
        next_node = &path->nodes[next];
        vertices[0].position = node->position;
        vertices[1].position = next_node->position;
        vertices[0].position.y += AiEditorRouteYOffsetPc;
        vertices[1].position.y += AiEditorRouteYOffsetPc;
        vertices[0].colour = 0xff0000ff;
        vertices[1].colour = 0xff0000ff;
        AiRndrLine3d(vertices, NULL, NULL);
        current = next;
        end = aieditor->current_path->runtime_end;
    } while (current != end);
}

static void pathEditorCalcRouteIterator(AIPATH_PC *path, f32 *distances, u8 *visited, i32 previous, i32 current,
                                        f32 distance, i32 route_mask) {
    AIPATHNODE_PC *node;
    NUVEC difference;
    f32 new_distance;
    i32 index;

    ++iterator_count;
    if (visited[current / 8] & (1 << (current % 8)))
        return;
    visited[current / 8] |= 1 << (current % 8);
    node = &path->nodes[current];
    new_distance = NuVecDist(&path->nodes[previous].position, &node->position, &difference) + distance;
    if (new_distance <= distances[current]) {
        distances[current] = new_distance;
        for (index = 0; index < node->connection_count; ++index) {
            AIPATHCNX_PC *connection = node->connections[index];
            i32 direction = connection->node_indices[0] != current;
            if ((connection->traversal_flags[direction] & 0x40000000) == 0 &&
                (route_mask == 0 || (connection->route_mask & route_mask) != 0)) {
                pathEditorCalcRouteIterator(path, distances, visited, current, connection->node_indices[!direction],
                                            new_distance, route_mask);
            }
        }
    }
    visited[current / 8] &= ~(1 << (current % 8));
}

static f32 **pathEditorCalculateDistanceTable(AIPATH_PC *path, i32 route_mask, VARIPTR *cursor, VARIPTR *end) {
    f32 **table;
    i32 row;

    if (distance_tables == NULL || path == NULL || path->nodes == NULL)
        return NULL;
    table = (f32 **)AISysBufferAlloc(cursor, end, path->node_count * sizeof(f32 *));
    if (table != NULL) {
        memset(table, 0, path->node_count * sizeof(f32 *));
        for (row = 0; row < path->node_count; ++row) {
            f32 *distances = (f32 *)AISysBufferAlloc(cursor, end, path->node_count * sizeof(f32));
            i32 column;
            table[row] = distances;
            for (column = 0; column < path->node_count; ++column)
                distances[column] = FLT_MAX;
        }
        for (row = 0; row < path->node_count; ++row) {
            u8 visited[32];
            memset(visited, 0, sizeof(visited));
            pathEditorCalcRouteIterator(path, table[row], visited, row, row, 0.0f, route_mask);
        }
    }
    return table;
}

static void pathEditorPopulateRouteMatrix(AIPATH_PC *path, i32 route_mask, f32 **distances, u8 **matrix) {
    i32 source;

    if (distance_tables == NULL || path == NULL || path->nodes == NULL || distances == NULL)
        return;
    for (source = 0; source < path->node_count; ++source) {
        AIPATHNODE_PC *node = &path->nodes[source];
        i32 destination;
        for (destination = 0; destination < path->node_count; ++destination) {
            f32 best;
            i32 edge;
            matrix[source][destination] = 0xff;
            if (source == destination)
                continue;
            best = FLT_MAX;
            for (edge = 0; edge < path->nodes[source].connection_count; ++edge) {
                AIPATHCNX_PC *connection = node->connections[edge];
                i32 direction = connection->node_indices[0] != source;
                i32 neighbor;
                f32 distance;
                if (connection->traversal_flags[direction] & 0x40000000)
                    continue;
                if (route_mask != 0 && !(connection->route_mask & route_mask))
                    continue;
                neighbor = connection->node_indices[!direction];
                distance = (f32)(distances[neighbor][destination] + distances[source][neighbor]);
                if (distance < best) {
                    best = distance;
                    matrix[source][destination] = edge;
                }
            }
        }
    }
}

void __cdecl pathEditorSaveData(AIPATHSYS_PC *path_system) {
    AI_PATH_SYSTEM_LOAD_PC *system = (AI_PATH_SYSTEM_LOAD_PC *)path_system;
    i32 path_index;
    i32 index;
    i32 row;
    i32 character;
    i32 character_count;
    u32 length;
    char *text;
    AIPATH_PC *path;
    AIPATHCNX_PC *connection;
    AIPATHNODE_PC *node;
    AIPATHROUTE_PC *route;
    AIPATHNODELINK_PC *link;
    AIPATHSPECIALROUTE_PC *group;
    u32 count_mask_low;
    u64 bit;

    EdFileWriteInt(system->path_count);
    for (path_index = 0; path_index < system->path_count; ++path_index) {
        path = system->paths[path_index];
        EdFileWrite(path->name, 16);
        EdFileWriteChar(path->node_count);
        EdFileWriteChar(path->flags);
        EdFileWriteShort((u16)path->connection_count);
        for (index = 0; index < path->connection_count; ++index) {
            connection = &path->connections[index];
            EdFileWriteChar(connection->node_indices[0]);
            EdFileWriteChar(connection->node_indices[1]);
            if (aidata_version > 11) {
                EdFileWriteInt((i32)connection->traversal_flags[0]);
                EdFileWriteInt((i32)connection->traversal_flags[1]);
            } else if (aidata_version > 8) {
                EdFileWriteShort((u16)connection->traversal_flags[0]);
                EdFileWriteShort((u16)connection->traversal_flags[1]);
            } else {
                EdFileWriteChar((u8)connection->traversal_flags[0]);
                EdFileWriteChar((u8)connection->traversal_flags[1]);
            }
            EdFileWriteShort((u16)connection->rotation);
            EdFileWriteShort((u16)connection->route_mask);
            EdFileWriteFloat(connection->distance);
            EdFileWriteFloat(connection->horizontal_distance);
        }
        for (index = 0; index < path->node_count; ++index) {
            node = &path->nodes[index];
            length = node->name != NULL ? strlen(node->name) + 1 : 0;
            EdFileWriteInt((i32)length);
            if (length != 0)
                EdFileWrite(node->name, (i32)length);
            EdFileWriteFloat(node->position.x);
            EdFileWriteFloat(node->position.y);
            EdFileWriteFloat(node->position.z);
            EdFileWriteFloat(node->radius);
            if (aidata_version > 7) {
                EdFileWriteFloat(node->min_height);
                EdFileWriteFloat(node->max_height);
            }
            EdFileWriteChar(node->connection_count);
            EdFileWriteChar(node->flags);
            EdFileWriteChar(0);
            EdFileWriteChar(node->runtime_flags);
            EdFileWriteShort((u16)node->path_flags);
            if (aidata_version >= 19)
                EdFileWriteChar(node->special_route_index);
            else
                EdFileWriteChar(0);
            if (NuSpecialExistsFn(&node->special_handle) != 0 &&
                (text = NuSpecialGetName(&node->special_handle)) != NULL) {
                length = strlen(text) + 1;
                EdFileWriteChar(length);
                EdFileWrite(text, (i32)length);
                EdFileWriteFloat(node->special_position.x);
                EdFileWriteFloat(node->special_position.y);
                EdFileWriteFloat(node->special_position.z);
            } else {
                EdFileWriteChar(0);
            }
            for (row = 0; row < node->connection_count; ++row)
                EdFileWriteShort(node->connections[row] - path->connections);
            if ((node->connection_count & 1) != 0)
                EdFileWriteShort(0);
            EdFileWriteShort((u16)node->route_membership_mask);
            EdFileWriteShort((u16)node->route_boundary_mask);
        }
        for (row = 0; row < path->node_count; ++row)
            EdFileWrite(path->route_matrix[row], path->node_count);
        EdFileWriteChar(path->route_count);
        for (index = 0; index < path->route_count; ++index) {
            route = &path->routes[index];
            length = strlen(route->name) + 1;
            EdFileWriteChar(length);
            EdFileWrite(route->name, (i32)length);
            EdFileWriteChar(route->route_count);
            EdFileWriteChar(route->exit_node_count);
            EdFileWriteChar(0);
            EdFileWriteChar(0);
            if (route->route_count != 0) {
                EdFileWrite(route->node_routes, path->node_count);
                EdFileWrite(route->node_directions, route->route_count);
                for (row = 0; row < route->route_count; ++row)
                    EdFileWrite(route->route_nodes[row], route->route_count);
                if (route->exit_node_count != 0)
                    EdFileWrite(route->exit_nodes, route->exit_node_count);
            }
            if (SpecialRouteCharacterNameFn != NULL) {
                count_mask_low = route->character_mask[2];
                character_count = 0;
                for (character = 0; character < 64; ++character) {
                    bit = (u64)1 << character;
                    if ((((u32)bit & count_mask_low) | ((u32)(bit >> 32) & route->character_mask[3])) != 0)
                        ++character_count;
                }
                EdFileWriteChar(character_count);
                for (character = 0; character < 63; ++character) {
                    bit = (u64)1 << character;
                    if ((((u32)bit & route->character_mask[2]) | ((u32)(bit >> 32) & route->character_mask[3])) != 0) {
                        text = SpecialRouteCharacterNameFn((u8)character);
                        if (text != NULL) {
                            length = strlen(text) + 1;
                            EdFileWriteChar(length);
                            EdFileWrite(text, (i32)length);
                        } else {
                            EdFileWriteChar(0);
                        }
                    }
                }
                if ((route->character_mask[3] & 0x80000000u) != 0) {
                    EdFileWriteChar(9);
                    EdFileWrite((void *)GameAIEveryonePc, 9);
                }
            } else {
                EdFileWriteChar(0);
            }
        }
        if (aidata_version >= 19) {
            EdFileWriteChar(path->special_route_count);
            link = path->special_routes;
            for (index = 0; index < path->special_route_count; ++index, ++link) {
                EdFileWriteUnsignedChar(link->node_index);
                EdFileWriteShort((u16)link->special_route_index);
            }
        }
    }
    if (aidata_version >= 19) {
        EdFileWriteShort((u16)system->special_route_count);
        group = system->special_routes;
        for (index = 0; index < system->special_route_count; ++index, ++group) {
            EdFileWriteChar(group->path_count);
            for (row = 0; row < group->path_count; ++row)
                EdFileWriteChar(group->paths[row]->index);
        }
    }
}

static void __cdecl pathEditor_cbCancelSelectMenu(eduimenu_s *menu, eduimenu_s *child) {
    (void)child;
    eduiMenuDestroy(menu);
}

static void __cdecl AiEditorRenamePathPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDAIPATH_s *other;
    (void)menu;
    (void)event;
    if (aieditor->current_path != NULL) {
        char *name = ((edui_textpicker_s *)item)->value;
        if (strlen(name) != 0) {
            for (other = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->paths); other != NULL;
                 other = (EDAIPATH_s *)NuLinkedListGetNext(&aieditor->paths, &other->link)) {
                if (NuStrICmp(other->name, name) == 0)
                    return;
            }
            strcpy(aieditor->current_path->name, name);
        }
    }
}

static void __cdecl AiEditorRenamePathCancelPc(eduimenu_s *menu, eduimenu_s *child) {
    (void)child;
    eduiMenuDestroy(menu);
}

void __cdecl pathEditor_cbRenamePathMenu(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    (void)item;
    (void)event;
    if (aieditor->current_path == NULL || (aieditor->current_path->flags & 1))
        return;
    menu = eduiMenuCreate(240, 90, 240, 250, ed_fnt, AiEditorRenamePathCancelPc, "Rename Path");
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu, eduiItemTextPickCreate(0, &AiEditorSaveMessageColoursPc, AiEditorRenamePathPc, "Path Name"));
    strcpy(((edui_textpicker_s *)edui_last_item)->value, aieditor->current_path->name);
    ((edui_textpicker_s *)edui_last_item)->max_length = 15;
    eduiMenuAttach(parent, menu);
    menu->x = parent->x + 10;
    menu->y = parent->y + 40;
}

static void __cdecl AiEditorRenameNodePc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDAIPATH_s *path;
    EDAIPATHNODE_s *node;
    EDAIPATHNODE_s *current;
    char *name;
    (void)menu;
    (void)event;
    if (aieditor->current_path == NULL)
        return;
    path = aieditor->current_path;
    current = path->current_node;
    if (current == NULL)
        return;
    name = ((edui_textpicker_s *)item)->value;
    if (strlen(name) == 0) {
        memset(current->name, 0, 16);
        return;
    }
    for (node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&path->nodes); node != NULL;
         node = (EDAIPATHNODE_s *)NuLinkedListGetNext(&aieditor->current_path->nodes, &node->link)) {
        if (NuStrICmp(node->name, name) == 0)
            return;
    }
    strcpy(aieditor->current_path->current_node->name, name);
}

static void __cdecl AiEditorRenameNodeCancelPc(eduimenu_s *menu, eduimenu_s *child) {
    (void)child;
    eduiMenuDestroy(menu);
}

void __cdecl pathEditor_cbRenameNodeMenu(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    (void)item;
    (void)event;
    if (aieditor->current_path == NULL || aieditor->current_path->current_node == NULL)
        return;
    menu = eduiMenuCreate(240, 90, 240, 250, ed_fnt, AiEditorRenameNodeCancelPc, "Rename Node");
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu, eduiItemTextPickCreate(0, &AiEditorSaveMessageColoursPc, AiEditorRenameNodePc, "Node Name"));
    strcpy(((edui_textpicker_s *)edui_last_item)->value, aieditor->current_path->current_node->name);
    ((edui_textpicker_s *)edui_last_item)->max_length = 15;
    eduiMenuAttach(parent, menu);
    menu->x = parent->x + 10;
    menu->y = parent->y + 40;
}

void __cdecl pathEditor_cbDrawWallsplinesToggle(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    aieditorsettings.display_options = (aieditorsettings.display_options & ~512u) | (((u32)item->flags & 1) << 9);
}

void __cdecl pathEditor_cbCnxFlagsToggle(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    AI_PATH_CONNECTION_TYPE_PC *type = &AIPathConnectionTypesPc[item->data];
    EDAIPATHNODE_s *node;
    EDAIPATHNODE_s *other;
    EDAIPATHCNX_s *connection;
    EDAIPATHCNX_s *reverse;
    i32 index;
    (void)menu;
    (void)event;
    if (aieditor->current_path == NULL)
        return;
    node = aieditor->current_path->current_node;
    other = aieditor->current_path->other_node;
    if (node == NULL || other == NULL)
        return;
    for (index = 0; index < 8; ++index) {
        if (node->connections[index].node == other)
            break;
    }
    if (index == 8)
        return;
    connection = &node->connections[index];
    if (connection == NULL)
        return;
    if (connection->flags & type->connection_flag)
        connection->flags &= ~type->connection_flag;
    else
        connection->flags |= type->connection_flag;
    if (type->flags == 0)
        return;
    reverse = pathEditor_FindConnection(aieditor->current_path->other_node, aieditor->current_path->current_node);
    if (reverse == NULL)
        return;
    if (connection->flags & type->connection_flag)
        reverse->flags |= type->connection_flag;
    else
        reverse->flags &= ~type->connection_flag;
}

void __cdecl pathEditor_cbNodeFlagsToggle(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDAIPATHNODE_s *node = aieditor->current_path->current_node;
    (void)menu;
    (void)event;
    if (node == NULL)
        return;
    if ((u8)item->data & node->flags)
        aieditor->current_path->current_node->flags &= ~(u8)item->data;
    else
        aieditor->current_path->current_node->flags |= (u8)item->data;
    node = aieditor->current_path->current_node;
    if (node->flags & 0x80)
        node->special.display_special = node->special.special = node->special.scene = NULL;
}

static __declspec(noinline) EDAISHAREDPATHNODE_s *pathEditor_AllocSharedNode(void) {
    EDAISHAREDPATHNODE_s *shared = (EDAISHAREDPATHNODE_s *)NuLinkedListGetHead(&aieditor->free_shared_nodes);
    if (shared != NULL) {
        NuLinkedListRemove(&aieditor->free_shared_nodes, &shared->link);
        NuLinkedListAppend(&aieditor->shared_path_nodes, &shared->link);
    }
    return shared;
}

static __declspec(noinline) void pathEditor_DestroySharedNode(EDAISHAREDPATHNODE_s *shared) {
    EDAIPATH_s *path;
    EDAIPATHNODE_s *node;
    if (shared == NULL)
        return;
    for (path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->paths); path != NULL;
         path = (EDAIPATH_s *)NuLinkedListGetNext(&aieditor->paths, &path->link)) {
        for (node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&aieditor->current_path->nodes); node != NULL;
             node = (EDAIPATHNODE_s *)NuLinkedListGetNext(&aieditor->current_path->nodes, &node->link)) {
            if (node->shared_node == shared) {
                node->shared_node = NULL;
                break;
            }
        }
    }
    NuLinkedListRemove(&aieditor->shared_path_nodes, &shared->link);
    NuLinkedListAppend(&aieditor->free_shared_nodes, &shared->link);
}

static __declspec(noinline) void pathEditor_ReleaseSharedNode(EDAIPATHNODE_s *node) {
    if (node != NULL && node->shared_node != NULL) {
        --node->shared_node->reference_count;
        if (node->shared_node->reference_count < 2)
            pathEditor_DestroySharedNode(node->shared_node);
        node->shared_node = NULL;
    }
}

static __declspec(noinline) void pathEditor_SetSharedNode(EDAIPATHNODE_s *node, EDAISHAREDPATHNODE_s *shared) {
    if (shared == NULL || node == NULL)
        return;
    if (node->shared_node != NULL) {
        if (node->shared_node == shared)
            return;
        pathEditor_ReleaseSharedNode(node);
    }
    node->shared_node = shared;
    ++shared->reference_count;
}

void __cdecl pathEditor_Enter(void) {
    EDAISHAREDPATHNODE_s *shared_nodes[64];
    AIPATHSYS_PC *system;
    AIPATH_PC *runtime;
    AIPATHNODE_PC *source;
    AIPATHCNX_PC *cnx;
    EDAIPATH_s *path;
    EDAIPATHNODE_s *node;
    EDAIPATHNODE_s *other;
    EDAISHAREDPATHNODE_s *shared;
    EDAIPATHCNX_s *found;
    NULISTHDR *list;
    i32 node_base;
    i32 path_index;
    i32 index;
    i32 connection;
    i32 direction;

    list = &aieditor->paths;
    list->head = NULL;
    list->tail = NULL;
    for (index = 0; index < 32; ++index)
        NuLinkedListAppend(&aieditor->free_paths, &aieditor->path_storage[index].link);
    list = &aieditor->free_path_nodes;
    list->head = NULL;
    list->tail = NULL;
    for (index = 0; index < 1024; ++index)
        NuLinkedListAppend(&aieditor->free_path_nodes, &aieditor->node_storage[index].link);
    list = &aieditor->free_shared_nodes;
    list->head = NULL;
    list->tail = NULL;
    for (index = 0; index < 64; ++index)
        NuLinkedListAppend(&aieditor->free_shared_nodes, &aieditor->shared_node_storage[index].link);
    memset(shared_nodes, 0, sizeof(shared_nodes));
    system = NULL;
    if (aieditor->ai_system != NULL)
        system = aieditor->ai_system->path_sys;
    if (aieditor->cached_path_system != system) {
        aieditor->cached_path_system = system;
        if (aieditor->cached_path_system != NULL) {
            node_base = 0;
            for (path_index = 0; path_index < aieditor->cached_path_system->path_count; ++path_index) {
                runtime = aieditor->cached_path_system->paths[path_index];
                path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->free_paths);
                if (path != NULL) {
                    NuLinkedListRemove(&aieditor->free_paths, &path->link);
                    NuLinkedListAppend(&aieditor->paths, &path->link);
                    strcpy(path->name, runtime->name);
                    if (NuStrICmp(path->name, "LevelPath") == 0)
                        path->flags |= 1;
                    for (index = 0; index < runtime->route_count; ++index) {
                        path->routes[index].flags |= 1;
                        NuStrCpy(path->routes[index].name, runtime->routes[index].name);
                        path->routes[index].user_mask = *(u64 *)&runtime->routes[index].character_mask[2];
                    }
                    if (runtime->node_count != 0) {
                        for (index = 0; index < runtime->node_count; ++index) {
                            source = &runtime->nodes[index];
                            node = pathEditor_CreateNode(path);
                            if (node == NULL)
                                continue;
                            node->position = source->position;
                            node->radius = source->radius;
                            node->lower_height = source->min_height - source->position.y;
                            node->upper_height = source->max_height - source->position.y;
                            node->flags = source->runtime_flags;
                            node->special = *(struct nuhspecial_s *)&source->special_handle;
                            node->special_position = source->special_position;
                            node->index = (i16)index;
                            if (source->name != NULL)
                                strcpy(node->name, source->name);
                            if (source->special_route_index < runtime->special_route_count) {
                                AIPATHNODELINK_PC *link = &runtime->special_routes[source->special_route_index];
                                if (shared_nodes[link->special_route_index] == NULL) {
                                    shared = (EDAISHAREDPATHNODE_s *)NuLinkedListGetHead(&aieditor->free_shared_nodes);
                                    if (shared != NULL) {
                                        NuLinkedListRemove(&aieditor->free_shared_nodes, &shared->link);
                                        NuLinkedListAppend(&aieditor->shared_path_nodes, &shared->link);
                                    }
                                    shared_nodes[link->special_route_index] = shared;
                                }
                                shared = shared_nodes[link->special_route_index];
                                if (shared != NULL) {
                                    if (node->shared_node != NULL) {
                                        if (node->shared_node == shared)
                                            continue;
                                        --node->shared_node->reference_count;
                                        if (node->shared_node->reference_count < 2)
                                            pathEditor_DestroySharedNode(node->shared_node);
                                        node->shared_node = NULL;
                                    }
                                    node->shared_node = shared;
                                    ++shared->reference_count;
                                }
                            }
                        }
                        for (index = 0; index < runtime->node_count; ++index) {
                            source = &runtime->nodes[index];
                            node = &aieditor->node_storage[node_base + index];
                            for (connection = 0; connection < source->connection_count; ++connection) {
                                cnx = source->connections[connection];
                                direction = cnx->node_indices[0] != index;
                                other = &aieditor->node_storage[node_base + cnx->node_indices[direction == 0]];
                                PATH_EDITOR_FIND_CONNECTION_PC(found, node, other);
                                if (found == NULL) {
                                    pathEditor_ConnectNodes(node, other);
                                    PATH_EDITOR_FIND_CONNECTION_PC(found, node, other);
                                    if (found == NULL)
                                        continue;
                                }
                                found->flags = cnx->original_traversal_flags[direction];
                                if (found->flags & 0x10000000)
                                    found->flags &= ~0x10000000u;
                                found->route_mask |= cnx->route_mask;
                            }
                            node->route_mask = source->route_boundary_mask;
                        }
                    }
                }
                node_base += runtime->node_count;
            }
        }
    }
    if (aieditor->cached_path_system == NULL) {
        path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->free_paths);
        if (path != NULL) {
            NuLinkedListRemove(&aieditor->free_paths, &path->link);
            NuLinkedListAppend(&aieditor->paths, &path->link);
            strcpy(path->name, "LevelPath");
            path->flags |= 1;
        }
    }
    if (strlen(aieditorsettings.path_name) != 0)
        aieditor->current_path = pathEditor_GetPath(aieditorsettings.path_name);
    else
        aieditor->current_path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->paths);
    pathEditor_CalcNodeIXs();
    for (index = 0; index < 16; ++index) {
        aieditor->current_path->current_route = &aieditor->current_path->routes[index];
        if (aieditor->current_path->current_route->flags & 1)
            break;
    }
    if (aieditor->current_path->current_route != NULL && (aieditor->current_path->current_route->flags & 1) == 0)
        aieditor->current_path->current_route = NULL;
}

static void __cdecl AiEditorRenameRoutePc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    i32 index;
    (void)menu;
    (void)event;
    if (aieditor->current_path != NULL && aieditor->current_path->current_route != NULL) {
        char *name = ((edui_textpicker_s *)item)->value;
        if (strlen(name) != 0) {
            for (index = 0; index < 16; ++index) {
                if (NuStrICmp(aieditor->current_path->routes[index].name, name) == 0)
                    return;
            }
            strcpy(aieditor->current_path->current_route->name, name);
        }
    }
}

static void __cdecl AiEditorRenameRouteCancelPc(eduimenu_s *menu, eduimenu_s *child) {
    (void)child;
    eduiMenuDestroy(menu);
}

void __cdecl routeEditor_cbRenameRouteMenu(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    (void)item;
    (void)event;
    if (aieditor->current_path == NULL || aieditor->current_path->current_route == NULL)
        return;
    menu = eduiMenuCreate(240, 90, 240, 250, ed_fnt, AiEditorRenameRouteCancelPc, "Rename Route");
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu,
                    eduiItemTextPickCreate(0, &AiEditorSaveMessageColoursPc, AiEditorRenameRoutePc, "Route Name"));
    strcpy(((edui_textpicker_s *)edui_last_item)->value, aieditor->current_path->current_route->name);
    ((edui_textpicker_s *)edui_last_item)->max_length = 15;
    eduiMenuAttach(parent, menu);
    menu->x = parent->x + 10;
    menu->y = parent->y + 40;
}

static void __cdecl AiEditorRouteUsersCancelPc(eduimenu_s *menu, eduimenu_s *child) {
    (void)child;
    eduiMenuDestroy(menu);
}

static void __cdecl AiEditorSetRouteUsersPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    AIEDITOR_RENDER_STATE *state;
    EDAIPATHROUTE_s *route;
    u64 mask;
    (void)menu;
    (void)event;
    if (item == NULL)
        return;
    state = aieditor;
    if (state->current_path == NULL)
        return;
    route = state->current_path->current_route;
    if (route == NULL || (u32)item->data >= 64)
        return;
    mask = (u64)1 << item->data;
    if ((route->user_mask & mask) != 0) {
        state->current_path->current_route->user_mask &= ~mask;
        item->flags &= ~1;
    } else {
        state->current_path->current_route->user_mask |= mask;
        item->flags |= 1;
    }
}

void __cdecl routeEditor_cbRouteUsers(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    char *name;
    i32 index;
    (void)item;
    (void)event;
    if (SpecialRouteCharacterNameFn == NULL)
        return;
    if (aieditor->current_path == NULL || aieditor->current_path->current_route == NULL)
        return;
    menu = eduiMenuCreate(220, 70, 240, 250, ed_fnt, AiEditorRouteUsersCancelPc, "Select route users");
    if (menu == NULL)
        return;
    index = 0;
    name = SpecialRouteCharacterNameFn((u8)index);
    while (name != NULL) {
        EDAIPATH_s *path;
        u64 mask;
        i32 selected;
        if (index >= 64)
            return;
        path = aieditor->current_path;
        mask = (u64)1 << index;
        selected = (path->current_route->user_mask & mask) != 0;
        eduiMenuAddItem(menu, eduiItemCheckCreate(index, &AiEditorSaveMessageColoursPc, selected, index + 1,
                                                  AiEditorSetRouteUsersPc, name));
        eduiMenuAttach(parent, menu);
        ++index;
        name = SpecialRouteCharacterNameFn((u8)index);
    }
    if (index < 64) {
        i32 selected = (aieditor->current_path->current_route->user_mask & ((u64)1 << 63)) != 0;
        eduiMenuAddItem(menu, eduiItemCheckCreate(63, &AiEditorSaveMessageColoursPc, selected, 64,
                                                  AiEditorSetRouteUsersPc, (char *)GameAIEveryonePc));
    }
}

i32 __cdecl routeEditor_AddToRoute(EDAIPATHNODE_s *node, EDAIPATHNODE_s *other) {
    i32 index;
    i32 slot;
    i32 i;
    u64 mask;
    EDAIPATHCNX_s *connection;
    if (node != NULL && other != NULL && aieditor->current_path->current_route != NULL &&
        (index = aieditor->current_path->current_route - aieditor->current_path->routes) < 16) {
        for (slot = 0; slot < 8; ++slot) {
            if (node->connections[slot].node == other) {
                connection = &node->connections[slot];
                mask = (u64)1 << index;
                if ((connection->route_mask & mask) != 0) {
                    connection->route_mask &= ~(u16)mask;
                    for (i = 0; i < 8; ++i) {
                        if (node->connections[slot].node != NULL && (node->connections[slot].route_mask & mask)) {
                            node->route_mask &= ~(u16)mask;
                            break;
                        }
                    }
                    for (i = 0; i < 8; ++i) {
                        if (other->connections[slot].node != NULL && (other->connections[slot].route_mask & mask)) {
                            other->route_mask &= ~(u16)mask;
                            break;
                        }
                    }
                    return -1;
                }
                connection->route_mask |= (u16)mask;
                return 1;
            }
        }
    }
    return 0;
}

static void pathEditorCreateSpecialRouteData(AIPATH_PC *path, EDAIPATH_s *editor_path, VARIPTR *cursor, VARIPTR *end) {
    i32 route_indices[16] = {0};
    u8 route_members[16][32] = {0};
    i32 slot;
    for (slot = 0; slot < 16; ++slot) {
        if (editor_path->routes[slot].flags & 1) {
            route_indices[slot] = path->route_count;
            path->route_count++;
        }
    }
    if (path->route_count == 0) {
    } else {
        EDAIPATHNODE_s *editor_node;
        i32 route_index;
        PATH_BUILD_ALLOCATE(AIPATHROUTE_PC *, path->routes, cursor, end, path->route_count * sizeof(AIPATHROUTE_PC));
        memset(path->routes, 0, path->route_count * sizeof(AIPATHROUTE_PC));
        for (slot = 0; slot < 16; ++slot) {
            AIPATHROUTE_PC *route;
            i32 name_length;
            u32 *user_mask;
            u32 high_mask;
            if (!(editor_path->routes[slot].flags & 1)) {
                continue;
            }
            route = &path->routes[route_indices[slot]];
            name_length = strlen(editor_path->routes[slot].name);
            if ((name_length != 0)) {
                PATH_BUILD_ALLOCATE(char *, route->name, cursor, end, name_length + 1);
                strcpy(route->name, editor_path->routes[slot].name);
            }
            user_mask = (u32 *)&editor_path->routes[slot].user_mask;
            route->character_mask[0] = user_mask[0];
            high_mask = user_mask[1];
            route->character_mask[1] = high_mask;
            if (high_mask & 0x80000000u) {
                route->character_mask[0] = 0xffffffffu;
                route->character_mask[1] = 0xffffffffu;
            }
            route->character_mask[2] = user_mask[0];
            route->character_mask[3] = user_mask[1];
        }

        for (editor_node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&editor_path->nodes); editor_node != NULL;
             editor_node = (EDAIPATHNODE_s *)NuLinkedListGetNext(&editor_path->nodes, &editor_node->link)) {
            AIPATHNODE_PC *runtime_node = &path->nodes[editor_node->index];
            if (editor_node->route_mask != 0) {
                for (slot = 0; slot < 16; ++slot) {
                    if (editor_node->route_mask & (((u64)1) << slot)) {
                        runtime_node->route_boundary_mask |= ((u64)1) << route_indices[slot];
                    }
                }
            }
            for (slot = 0; slot < 8; ++slot) {
                EDAIPATHCNX_s *editor_connection = &editor_node->connections[slot];
                AIPATHCNX_PC **connections;
                i32 edge;
                if (editor_connection->node == NULL || editor_connection->route_mask == 0) {
                    continue;
                }
                if (runtime_node->connection_count == 0) {
                    continue;
                }
                connections = runtime_node->connections;
                for (edge = 0; edge < runtime_node->connection_count; ++edge) {
                    const i16 other_index = editor_connection->node->index;
                    AIPATHCNX_PC *connection = connections[edge];
                    i32 route_slot;
                    if (connection->node_indices[0] != other_index && connection->node_indices[1] != other_index) {
                        continue;
                    }
                    for (route_slot = 0; route_slot < 16; ++route_slot) {
                        if (editor_connection->route_mask & (1u << route_slot)) {
                            i32 route_index = route_indices[route_slot];
                            u64 bit = ((u64)1) << route_index;
                            AIPATHCNX_PC *const volatile *connection_input = connections;
                            if ((connection_input[edge]->route_mask & bit) == 0) {
                                i32 endpoint;
                                connections[edge]->route_mask |= bit;
                                connections = runtime_node->connections;
                                connection = connections[edge];
                                for (endpoint = 0; endpoint < 2; ++endpoint) {
                                    route_members[route_index][connection->node_indices[endpoint] / 8] |=
                                        1 << (connection->node_indices[endpoint] & 7);
                                }
                            }
                        }
                    }
                }
            }
        }

        for (route_index = 0; route_index < path->route_count; ++route_index) {
            AIPATHROUTE_PC *route = &path->routes[route_index];
            i32 node;
            i32 member_index;
            i32 exit_index;
            u32 source;
            VARIPTR scratch;
            f32 **distances;
            u8 **next_hops;
            i32 next_source;
            for (node = 0; node < path->node_count; ++node) {
                if (route_members[route_index][node >> 3] & (1u << (node & 7))) {
                    ++route->route_count;
                }
                if (path->nodes[node].route_boundary_mask & (((u64)1) << route_indices[route_index])) {
                    ++route->exit_node_count;
                }
            }
            if (path->node_count != 0) {
                PATH_BUILD_ALLOCATE(u8 *, route->node_routes, cursor, end, path->node_count);
                memset(route->node_routes, 0, path->node_count);
            }
            if (route->route_count != 0) {
                PATH_BUILD_ALLOCATE(u8 *, route->node_directions, cursor, end, route->route_count);
                memset(route->node_directions, 0, route->route_count);
            }
            if (route->exit_node_count != 0) {
                PATH_BUILD_ALLOCATE(u8 *, route->exit_nodes, cursor, end, route->exit_node_count);
                memset(route->exit_nodes, 0, route->exit_node_count);
            }
            member_index = 0;
            exit_index = 0;
            for (node = 0; node < path->node_count; ++node) {
                route->node_routes[node] = 0xff;
                if (!(route_members[route_index][node >> 3] & (1u << (node & 7)))) {
                    continue;
                }
                path->nodes[node].route_membership_mask |= ((u64)1) << route_indices[route_index];
                route->node_routes[node] = member_index;
                route->node_directions[member_index] = node;
                if (path->nodes[node].route_boundary_mask & (((u64)1) << route_indices[route_index])) {
                    route->exit_nodes[exit_index++] = node;
                }
                ++member_index;
            }
            if (route->route_count == 0) {
                continue;
            }
            PATH_BUILD_ALLOCATE(u8 **, route->route_nodes, cursor, end, route->route_count * sizeof(u8 *));
            memset(route->route_nodes, 0, route->route_count * sizeof(u8 *));
            for (source = 0; source < route->route_count; ++source) {
                PATH_BUILD_ALLOCATE(u8 *, route->route_nodes[source], cursor, end, route->route_count);
                memset(route->route_nodes[source], 0, route->route_count);
            }
            // The distance and full-node next-hop tables are temporary. Keep their
            // allocations beyond the persistent route data without advancing it.
            scratch = *cursor;
            distances = pathEditorCalculateDistanceTable(path, ((u64)1) << route_index, &scratch, end);
            if (distances == NULL) {
                continue;
            }
            PATH_BUILD_ALLOCATE(u8 **, next_hops, &scratch, end, path->node_count * sizeof(u8 *));
            memset(next_hops, 0, path->node_count * sizeof(u8 *));
            for (next_source = 0; next_source < path->node_count; ++next_source) {
                PATH_BUILD_ALLOCATE(u8 *, next_hops[next_source], &scratch, end, path->node_count);
                memset(next_hops[next_source], 0, path->node_count);
            }
            pathEditorPopulateRouteMatrix(path, (i32)(((u64)1) << route_index), distances, next_hops);
            for (source = 0; source < route->route_count; ++source) {
                i32 destination;
                for (destination = 0; destination < route->route_count; ++destination) {
                    route->route_nodes[source][destination] =
                        next_hops[route->node_directions[source]][route->node_directions[destination]];
                }
            }
        }
    }
}

static void AiEditorClearMainMenuPc(void) {
    if (aieditor->main_menu != NULL) {
        eduiMenuDestroy(aieditor->main_menu);
        aieditor->main_menu = NULL;
    }
}

void __cdecl aieditor_cbDrawAllToggle(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    aieditorsettings.display_options = (aieditorsettings.display_options & ~2u) | (((u32)item->flags & 1) << 1);
}

static void __cdecl AiEditorDisplayBit4Pc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    aieditorsettings.display_options = (aieditorsettings.display_options & ~16u) | (((u32)item->flags & 1) << 4);
}

void __cdecl aieditor_cbSolidPathDisplayToggle(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    aieditorsettings.display_options = (aieditorsettings.display_options & ~128u) | (((u32)item->flags & 1) << 7);
}

static void __cdecl AiEditorDisplayBit8Pc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    aieditorsettings.display_options = (aieditorsettings.display_options & ~256u) | (((u32)item->flags & 1) << 8);
}

static void __cdecl AiEditorDisplayBit6Pc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    aieditorsettings.display_options = (aieditorsettings.display_options & ~64u) | (((u32)item->flags & 1) << 6);
}

void __cdecl aieditor_cbShowCreaturesToggle(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    aieditorsettings.display_options = (aieditorsettings.display_options & ~8u) | (((u32)item->flags & 1) << 3);
}

void __cdecl aieditor_cbShowCreaturesSetToggle(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    aieditorsettings.display_options = (aieditorsettings.display_options & ~1024u) | (((u32)item->flags & 1) << 10);
}

static void __cdecl AiEditorModeCancelPc(eduimenu_s *menu, eduimenu_s *child) {
    (void)child;
    eduiMenuDestroy(menu);
}

static void __cdecl AiEditorModeSelectPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if ((u32)item->data < (u32)(i32)aieditorsettings.modes.count) {
        AiEditorChangeModePc(item->data);
        AiEditorClearMainMenuPc();
    }
}

static void __cdecl AiEditorModeMenuPc(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    i32 index;
    (void)item;
    (void)event;
    menu = eduiMenuCreate(220, 70, 240, 250, ed_fnt, AiEditorModeCancelPc, "Select Editor Mode");
    if (menu == NULL)
        return;
    for (index = 0; index < aieditorsettings.modes.count; ++index) {
        i32 selected = index == aieditorsettings.modes.current;
        eduiMenuAddItem(menu, eduiItemCheckCreate(index, &AiEditorSaveMessageColoursPc, selected, 1,
                                                  AiEditorModeSelectPc, aieditorsettings.modes.entries[index].name));
        if (selected)
            menu->selected = edui_last_item;
        eduiMenuAttach(parent, menu);
    }
}

static void __cdecl AiEditorMessageCloseMenuPc(eduimenu_s *menu, eduimenu_s *child) {
    (void)menu;
    (void)child;
    if (aieditor->main_menu != NULL) {
        eduiMenuDestroy(aieditor->main_menu);
        aieditor->main_menu = NULL;
    }
}

static void __cdecl AiEditorSaveCloseSelectionPc(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    (void)menu;
    (void)item;
    (void)value;
    if (aieditor->main_menu != NULL) {
        eduiMenuDestroy(aieditor->main_menu);
        aieditor->main_menu = NULL;
    }
}

static void __cdecl AiEditorCameraToPlayerPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)item;
    (void)event;
    if (aieditor->ai_system != NULL && aieditor->ai_system->player_1 != NULL)
        edcamSetPos(&aieditor->ai_system->player_1->position);
    AiEditorClearMainMenuPc();
}

void __cdecl InitFn_AIEditorMovePlayers(void(__cdecl *callback)(NUVEC *)) {
    AiEditorMovePlayersCallbackPc = callback;
}

static void __cdecl AiEditorMovePlayersPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)item;
    (void)event;
    if (AiEditorMovePlayersCallbackPc != NULL)
        AiEditorMovePlayersCallbackPc(&aieditor->camera_position);
    AiEditorClearMainMenuPc();
}

eduimenu_s *__cdecl aieditor_AddMessage(char *title, char *message) {
    eduimenu_s *outer = eduiMenuCreate(0xc8, 0x46, 0xf0, 0x10e, ed_fnt, AiEditorMessageCloseMenuPc, title);
    if (outer != NULL) {
        eduimenu_s *inner = eduiMenuCreate(0x78, 0x5a, 0x1b8, 0xfa, ed_fnt, NULL, title);
        if (inner != NULL) {
            eduiitem_s *item =
                eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, AiEditorSaveCloseSelectionPc, message);
            eduiMenuAddItem(inner, item);
            eduiMenuAttach(outer, inner);
        }
    }
    return outer;
}

/* Native 5274f0..5275e0: private position argument EDI. */
static EDANTINODE_s *CreateAntinode(NUVEC *position) {
    EDANTINODE_s *node = (EDANTINODE_s *)NuLinkedListGetHead(&aieditor->free_antinodes);
    if (node != NULL) {
        NuLinkedListRemove(&aieditor->free_antinodes, &node->link);
        NuLinkedListAppend(&aieditor->antinodes, &node->link);
        node->position = *position;
        if (aieditor->selected_antinode != NULL) {
            node->radius = aieditor->selected_antinode->radius;
            node->lower_height = aieditor->selected_antinode->lower_height;
            node->upper_height = aieditor->selected_antinode->upper_height;
            node->type = aieditor->selected_antinode->type;
            node->base_radius = aieditor->selected_antinode->base_radius;
            node->base_height = aieditor->selected_antinode->base_height;
        } else {
            node->radius = 0.25f;
            node->lower_height = -default_path_heighttol;
            node->upper_height = default_path_heighttol;
        }
    }
    return node;
}

static void AiEditorFreeAntinodePc(EDANTINODE_s *node) {
    if (node == NULL)
        return;
    NuLinkedListRemove(&aieditor->antinodes, &node->link);
    memset(node, 0, sizeof(*node));
    NuLinkedListAppend(&aieditor->free_antinodes, &node->link);
}

/* Native 527620..5276d9: private node argument ESI. */
static void antinodeEditor_AntinodeMoved(EDANTINODE_s *node) {
    NUVEC forward = {0.0f, 0.0f, 1.0f};
    NUVEC rotated;
    NUMTX *matrix;
    if ((node->game_flags & 0x80) == 0 && NuSpecialExistsFn(&aieditor->cursor_platform)) {
        node->special = aieditor->cursor_platform;
        matrix = NuSpecialGetDrawMtx(&aieditor->cursor_platform);
        NuVecInvMtxTransform(&node->special_position, &node->position, matrix);
        NuVecMtxRotate(&rotated, &forward, matrix);
        node->rotation_offset = NuAtan2D(rotated.x, rotated.z);
        node->rotation_offset = NuAngSub(node->flags, node->rotation_offset);
    } else {
        node->special.scene = NULL;
        node->special.special = NULL;
        node->special.display_special = NULL;
    }
}

static void __cdecl AiEditorAntinodeGameFlagsPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDANTINODE_s *selected;
    (void)menu;
    (void)event;
    if (aieditor->selected_antinode != NULL) {
        if ((aieditor->selected_antinode->game_flags & (u8)item->data) != 0)
            aieditor->selected_antinode->game_flags &= ~(u8)item->data;
        else
            aieditor->selected_antinode->game_flags |= (u8)item->data;
        selected = aieditor->selected_antinode;
        if ((selected->game_flags & 0x80) != 0) {
            selected->special.scene = NULL;
            selected->special.special = NULL;
            selected->special.display_special = NULL;
        }
    }
}

void antinodeEditor_UpdateAntiNodesOnPlatforms(void) {
    struct EDANTINODE_s *node;
    NULISTHDR *list = &aieditor->antinodes;
    NUVEC rotated;
    NUVEC forward = {0.0f, 0.0f, 1.0f};
    for (node = ((EDANTINODE_s *)(NuLinkedListGetHead(list))); node != NULL;
         node = ((EDANTINODE_s *)(NuLinkedListGetNext(&aieditor->antinodes, &node->link)))) {
        struct numtx_s *matrix;
        if (!NuSpecialExistsFn(&node->special))
            continue;
        matrix = NuSpecialGetDrawMtx(&node->special);
        NuVecMtxTransform(&node->position, &node->special_position, matrix);
        NuVecMtxRotate(&rotated, &forward, matrix);
        node->flags = NuAtan2D(rotated.x, rotated.z);
        node->flags = NuAngAdd(node->flags, node->rotation_offset);
    }
}

static void __cdecl AiEditorAntinodeTypeCancelPc(eduimenu_s *menu, eduimenu_s *child) {
    (void)child;
    eduiMenuDestroy(menu);
}

static void __cdecl AiEditorAntinodeTypeSetterPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDANTINODE_s *antinode = aieditor->selected_antinode;
    i32 previous_type = antinode->type;
    (void)menu;
    (void)event;
    if (antinode != NULL && item != NULL)
        antinode->type = (u8)item->data;
    if (aieditor->selected_antinode->type == 0) {
        if (previous_type != 0) {
            if (aieditor->selected_antinode->base_radius > aieditor->selected_antinode->base_height)
                aieditor->selected_antinode->radius = aieditor->selected_antinode->base_radius;
            else
                aieditor->selected_antinode->radius = aieditor->selected_antinode->base_height;
        }
    } else if (previous_type == 0) {
        aieditor->selected_antinode->base_height = aieditor->selected_antinode->radius;
        aieditor->selected_antinode->base_radius = aieditor->selected_antinode->base_height;
    }
}

static void __cdecl AiEditorAntinodeTypeMenuPc(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    i32 index;
    (void)item;
    (void)event;
    if (aieditor->selected_antinode == NULL || LevelCharacterGlobalIDFn == NULL || GlobalCharacterNameFn == NULL)
        return;
    menu = eduiMenuCreate(220, 70, 240, 250, ed_fnt, AiEditorAntinodeTypeCancelPc, "Select AI Type");
    if (menu == NULL)
        return;
    for (index = 0; index < 3; ++index) {
        i32 selected = aieditor->selected_antinode->type == index;
        eduiMenuAddItem(menu,
                        eduiItemCheckCreate(index, &AiEditorSaveMessageColoursPc, selected, 1,
                                            AiEditorAntinodeTypeSetterPc, (char *)AiEditorAntinodeTypeNamesPc[index]));
        if (selected)
            menu->selected = edui_last_item;
        eduiMenuAttach(parent, menu);
    }
}

/* Native 527960: private node ESI, position ECX. */
static i32 antinodeEditor_InsideEllipse(EDANTINODE_s *node, NUVEC *position) {
    f32 extent;
    NUVEC offset;
    NUVEC rotated;
    f32 distance;
    f32 aspect;
    i32 angle;

    extent = node->base_radius > node->base_height ? node->base_radius : node->base_height;
    offset.x = position->x - node->position.x;
    if (offset.x > extent || offset.x < -extent) {
        return 0;
    }
    offset.z = position->z - node->position.z;
    if (offset.z > extent || offset.z < -extent) {
        return 0;
    }
    distance = offset.x * offset.x + offset.z * offset.z;
    if (!(distance < extent * extent)) {
        return 0;
    }
    aspect = node->base_height / node->base_radius;
    offset.y = 0.0f;
    NuVecRotateY(&rotated, &offset, -node->flags);
    angle = NuAtan2D(aspect * rotated.x, rotated.z);
    offset.x = NU_SIN_LUT(angle) * node->base_radius;
    offset.z = NU_COS_LUT(angle) * node->base_height;
    extent = offset.x * offset.x + offset.z * offset.z;
    if (!(distance < extent)) {
        return 0;
    }
    return 1;
}

/* Native 527ad0: private node ESI, position EDI. */
static i32 antinodeEditor_InsideRectangle(EDANTINODE_s *node, NUVEC *position) {
    f32 diagonal;
    NUVEC offset;

    diagonal = NuFsqrt(node->base_radius * node->base_radius + node->base_height * node->base_height);
    offset.x = position->x - node->position.x;
    if (offset.x > diagonal || offset.x < -diagonal) {
        return 0;
    }
    offset.z = position->z - node->position.z;
    if (offset.z > diagonal || offset.z < -diagonal) {
        return 0;
    }
    NuVecRotateY(&offset, &offset, -node->flags);
    if (!(AIPathFabs(offset.x) < node->base_radius)) {
        return 0;
    }
    if (!(AIPathFabs(offset.z) < node->base_height)) {
        return 0;
    }
    return 1;
}

EDANTINODE_s *__cdecl antinodeEditor_GetNearestAntinode(i32 require_inside) {
    EDANTINODE_s *nearest = NULL;
    f32 nearest_distance = FLT_MAX;
    EDANTINODE_s *node;
    NUVEC offset;
    f32 distance;
    f32 height;
    f32 upper;
    f32 lower;

    for (node = (EDANTINODE_s *)NuLinkedListGetHead(&aieditor->antinodes); node != NULL;
         node = (EDANTINODE_s *)NuLinkedListGetNext(&aieditor->antinodes, &node->link)) {
        distance = NuVecXZDistSqr(&aieditor->camera_position, &node->position, &offset);
        if (!(distance < nearest_distance)) {
            continue;
        }
        height = aieditor->camera_position.y - node->position.y;
        upper = node->upper_height < 0.2f ? 0.2f : node->upper_height;
        lower = node->lower_height > -0.2f ? -0.2f : node->lower_height;
        if (!(height <= upper && height >= lower)) {
            continue;
        }
        if (require_inside) {
            switch (node->type) {
                case 0:
                    if (!(node->radius * node->radius > distance)) {
                        continue;
                    }
                    break;
                case 1:
                    if (!antinodeEditor_InsideEllipse(node, &aieditor->camera_position)) {
                        continue;
                    }
                    break;
                case 2:
                    if (!antinodeEditor_InsideRectangle(node, &aieditor->camera_position)) {
                        continue;
                    }
                    break;
                default:
                    continue;
            }
        }
        nearest = node;
        nearest_distance = distance;
    }
    return nearest;
}

static void __cdecl AiEditorDeleteAntinodeCancelPc(eduimenu_s *menu, eduimenu_s *child) {
    (void)menu;
    (void)child;
    if (aieditor->main_menu != NULL) {
        eduiMenuDestroy(aieditor->main_menu);
        aieditor->main_menu = NULL;
    }
}

static void __cdecl AiEditorDeleteAntinodeAnswerPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (item != NULL && item->data != 0 && aieditor->selected_antinode != NULL &&
        aieditor->selected_antinode == aieditor->nearest_antinode) {
        AiEditorFreeAntinodePc(aieditor->selected_antinode);
        aieditor->selected_antinode = NULL;
    }
    if (aieditor->main_menu != NULL) {
        eduiMenuDestroy(aieditor->main_menu);
        aieditor->main_menu = NULL;
    }
}

eduimenu_s *__cdecl antinodeEditor_DeleteAntinodeMenu(void) {
    eduimenu_s *menu;
    if (aieditor->selected_antinode == NULL || aieditor->selected_antinode != aieditor->nearest_antinode)
        return NULL;
    menu = eduiMenuCreate(200, 70, 240, 270, ed_fnt, AiEditorDeleteAntinodeCancelPc, "Delete Antinode??");
    if (menu != NULL) {
        eduiMenuAddItem(menu, eduiItemSelCreate(0, &AiEditorSaveMessageColoursPc, 0, 0, AiEditorDeleteAntinodeAnswerPc,
                                                (char *)MenuPcString75f2c0));
        eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, AiEditorDeleteAntinodeAnswerPc,
                                                (char *)MenuPcString75f2c4));
    }
    return menu;
}

static void antinodeEditorDrawOnePc(EDANTINODE_s *node, i32 segments, u32 colour, i32 solid) {
    switch (node->type) {
        case 0:
            if (solid)
                LocaledbitsDrawSolidCircleXY(&node->position, node->radius, node->position.y + node->lower_height,
                                             node->position.y + node->upper_height, colour, 0, segments);
            else
                LocaledbitsDrawCircleXY(&node->position, node->radius, colour, 0, segments);
            break;
        case 1:
            if (solid)
                LocaledbitsDrawSolidEllipseXY(&node->position, node->base_radius, node->base_height, node->flags,
                                              node->position.y + node->lower_height,
                                              node->position.y + node->upper_height, colour, 0, segments);
            else
                LocaledbitsDrawSolidEllipseXY(&node->position, node->base_radius, node->base_height, node->flags,
                                              node->position.y, node->position.y, colour, 0, segments);
            break;
        case 2: {
            NUMTX matrix;
            NUVEC minimum;
            NUVEC maximum;
            NuMtxSetTranslation(&matrix, &node->position);
            NuMtxPreRotateY(&matrix, node->flags);
            minimum.x = -node->base_radius;
            minimum.y = node->lower_height;
            minimum.z = -node->base_height;
            maximum.x = node->base_radius;
            maximum.y = node->upper_height;
            maximum.z = node->base_height;
            if (!solid) {
                minimum.y = 0.0f;
                maximum.y = 0.0f;
            }
            NuRndrBoundingBox(&minimum, &maximum, &matrix, colour);
            break;
        }
    }
}

void __cdecl antinodeEditorDrawAntinodes(void) {
    i32 solid = (aieditorsettings.display_options >> 8) & 1;
    EDANTINODE_s *node;
    for (node = (EDANTINODE_s *)NuLinkedListGetHead(&aieditor->antinodes); node != NULL;
         node = (EDANTINODE_s *)NuLinkedListGetNext(&aieditor->antinodes, &node->link)) {
        u32 colour = 0x50505050;
        i32 segments = 8;
        if (aieditorsettings.modes.current == AiEditorAntinodeModeIdPc) {
            if (node == aieditor->selected_antinode) {
                segments = 32;
                if (node == aieditor->nearest_antinode)
                    colour = NuSpecialExistsFn(&node->special) ? 0xffff0064 : 0xffff0000;
                else
                    colour = NuSpecialExistsFn(&node->special) ? 0x80800064 : 0x80800000;
            } else if (node == aieditor->nearest_antinode) {
                colour = 0xffffffff;
                segments = 32;
            } else {
                colour = 0x80640000;
            }
        }
        antinodeEditorDrawOnePc(node, segments, colour, solid);
    }
}

/* Native 539050..5398fa. Input values are deliberately reread after callbacks
 * where the PC reloads them; publication uses the shared selected/nearest slots. */
void __cdecl antinodeEditor_Enter(void) {
    i32 i;
    NULISTHDR *list = &aieditor->antinodes;
    list->tail = list->head = NULL;
    for (i = 0; i < 128; ++i)
        NuLinkedListAppend(&aieditor->free_antinodes, &aieditor->antinode_pool[i].link);
    for (i = 0; i < aieditor->ai_system->antinode_count; ++i) {
        AIANTINODE_PC *source = &aieditor->ai_system->antinodes[i];
        EDANTINODE_s *node = CreateAntinode(&source->position);
        if (node != NULL) {
            node->position = source->position;
            node->radius = source->radius;
            node->lower_height = source->min_y - source->position.y;
            node->upper_height = source->max_y - source->position.y;
            node->game_flags = source->game_flags;
            node->special = *(struct nuhspecial_s *)&source->special_handle;
            node->special_position = source->special_position;
            node->rotation_offset = source->rotation_offset;
            node->flags = source->rotation;
            node->base_radius = source->base_radius;
            node->base_height = source->base_height;
            node->type = source->type;
        }
    }
}

void antinodeEditorSaveData(void) {
    i32 count = 0;
    struct EDANTINODE_s *node;
    if (aidata_version <= 12)
        return;
    for (node = ((EDANTINODE_s *)(NuLinkedListGetHead(&aieditor->antinodes))); node != NULL;
         node = ((EDANTINODE_s *)(NuLinkedListGetNext(&aieditor->antinodes, &node->link))))
        ++count;
    EdFileWriteInt(count);
    if (count == 0)
        return;
    for (node = ((EDANTINODE_s *)(NuLinkedListGetHead(&aieditor->antinodes))); node != NULL;
         node = ((EDANTINODE_s *)(NuLinkedListGetNext(&aieditor->antinodes, &node->link)))) {
        char *name;
        if (node->type == 2)
            node->radius = NuFsqrt(node->base_radius * node->base_radius + node->base_height * node->base_height);
        else if (node->type == 1) {
            if (node->base_radius > node->base_height)
                node->radius = node->base_radius;
            else
                node->radius = node->base_height;
        }
        EdFileWriteFloat(node->position.x);
        EdFileWriteFloat(node->position.y);
        EdFileWriteFloat(node->position.z);
        EdFileWriteFloat(node->radius);
        EdFileWriteFloat(node->lower_height + node->position.y);
        EdFileWriteFloat(node->upper_height + node->position.y);
        if (aidata_version > 14) {
            EdFileWriteInt(node->flags);
            EdFileWriteFloat(node->base_radius);
            EdFileWriteFloat(node->base_height);
            EdFileWriteChar(node->type);
            EdFileWriteChar(0);
            EdFileWriteChar(0);
        } else {
            EdFileWriteChar(0);
            EdFileWriteChar(0);
            EdFileWriteChar(0);
        }
        EdFileWriteChar(node->game_flags);
        if (NuSpecialExistsFn(&node->special) && (name = NuSpecialGetName(&node->special)) != NULL) {
            i32 length = strlen(name) + 1;
            EdFileWriteChar(length);
            EdFileWrite(name, length);
            EdFileWriteFloat(node->special_position.x);
            EdFileWriteFloat(node->special_position.y);
            EdFileWriteFloat(node->special_position.z);
            if (aidata_version > 14)
                EdFileWriteInt(node->rotation_offset);
        } else {
            EdFileWriteChar(0);
        }
    }
}

/* Native 528450..5285ae: private type argument EDI. */
static __declspec(noinline) EDAICREATURE_s *CreateCreature(i32 type, NUVEC *position, i32 angle) {
    EDAICREATURE_s *creature = NULL;
    if (type != -1) {
        creature = (EDAICREATURE_s *)NuLinkedListGetHead(&aieditor->free_creatures);
        if (creature != NULL) {
            NuLinkedListRemove(&aieditor->free_creatures, &creature->link);
            NuLinkedListAppend(&aieditor->creatures, &creature->link);
            creature->character_type = (i16)type;
            creature->set = 0;
            creature->group_count = default_ngroup;
            creature->across_count = default_nacross;
            creature->x_spacing = default_xspacing;
            creature->z_spacing = default_zspacing;
            creature->stagger_start = AiLoadStartStaggerPc;
            if (GetViewRangeFn != NULL)
                creature->view_distance = GetViewRangeFn(type);
            else
                creature->view_distance = 1.0f;
            if (GetHearDistanceFn != NULL)
                creature->hear_distance = GetHearDistanceFn(type);
            else
                creature->hear_distance = 1.0f;
            if (GetMaxViewHeightFn != NULL)
                creature->max_view_height = GetMaxViewHeightFn(type);
            else
                creature->max_view_height = 1.0f;
            if (GetMinViewHeightFn != NULL)
                creature->min_view_height = GetMinViewHeightFn(type);
            else
                creature->min_view_height = 1.0f;
            creature->difficulty = AiLoadActivationDifficultyPc;
            creature->min_respawns = AiLoadMinimumRespawnCountPc;
            creature->max_respawns = AiLoadMaximumRespawnCountPc;
            creature->min_respawn_time = AiLoadMinimumRespawnTimePc;
            creature->max_respawn_time = AiLoadMaximumRespawnTimePc;
            if (position != NULL) {
                creature->position = *position;
                creature->angle = angle;
            }
        }
    }
    return creature;
}

/* Native 5285b0..5285e9: private creature argument ESI. */
static __declspec(noinline) void creatureEditorDestroyCreature(EDAICREATURE_s *creature) {
    if (creature != NULL) {
        NuLinkedListRemove(&aieditor->creatures, &creature->link);
        memset(creature, 0, sizeof(EDAICREATURE_s));
        NuLinkedListAppend(&aieditor->free_creatures, &creature->link);
    }
}

void __cdecl creatureEditorSaveData(AIPATHSYS_PC *system) {
    EDAICREATURE_s *creature;
    i32 count = 0;
    if (GlobalCharacterNameFn == NULL) {
        EdFileWriteInt(0);
        return;
    }
    for (creature = (EDAICREATURE_s *)NuLinkedListGetHead(&aieditor->creatures); creature != NULL;
         creature = (EDAICREATURE_s *)NuLinkedListGetNext(&aieditor->creatures, &creature->link)) {
        if (creature->check.path != NULL && GlobalCharacterNameFn(creature->character_type) != NULL)
            count++;
    }
    EdFileWriteInt(count);
    for (creature = (EDAICREATURE_s *)NuLinkedListGetHead(&aieditor->creatures); creature != NULL;
         creature = (EDAICREATURE_s *)NuLinkedListGetNext(&aieditor->creatures, &creature->link)) {
        char *character_name = GlobalCharacterNameFn(creature->character_type);
        AIPATH_PC *path;
        i32 index, connection_index = 0, direction = 0;
        f64 saved;
        if (creature->check.path == NULL || character_name == NULL)
            continue;
        EdFileWrite(creature->name, 16);
        EdFileWrite(creature->script_name, 16);
        if (aidata_version > 13)
            EdFileWrite(character_name, 32);
        else
            EdFileWrite(character_name, 16);
        EdFileWriteFloat(creature->position.x);
        EdFileWriteFloat(creature->position.y);
        EdFileWriteFloat(creature->position.z);
        EdFileWriteShort((u16)creature->angle);
        if (aidata_version > 15)
            EdFileWriteChar(creature->set);
        EdFileWriteChar(creature->group_count);
        EdFileWriteChar(creature->across_count);
        EdFileWriteInt(creature->valid_positions);
        EdFileWriteFloat(creature->x_spacing);
        EdFileWriteFloat(creature->z_spacing);
        EdFileWriteInt(creature->flags);
        EdFileWriteChar((u8)creature->check.path->draw_index);
        path = system->paths[creature->check.path->draw_index];
        for (index = 0; index < path->connection_count; index++) {
            AIPATHCNX_PC *connection = &path->connections[index];
            i32 first_index = creature->check.first->index;
            if (connection->node_indices[0] == first_index &&
                (u16)connection->node_indices[1] == (u16)creature->check.second->index) {
                i32 angle = creature->check.angle;
                i32 absolute = angle < 0 ? (i32)(0u - (u32)angle) : angle;
                connection_index = index;
                direction = absolute >= 0x4000;
                break;
            }
            if (connection->node_indices[0] == creature->check.second->index &&
                connection->node_indices[1] == first_index) {
                i32 angle = creature->check.angle;
                i32 absolute = angle < 0 ? (i32)(0u - (u32)angle) : angle;
                connection_index = index;
                direction = absolute < 0x4000;
                break;
            }
        }
        EdFileWriteChar(direction);
        EdFileWriteShort(connection_index);
        EdFileWriteFloat(creature->script_params[0]);
        EdFileWriteFloat(creature->script_params[1]);
        EdFileWriteFloat(creature->script_params[2]);
        EdFileWriteFloat(creature->script_params[3]);
        if (creature->trigger_area != NULL) {
            EdFileWriteInt(1);
            EdFileWrite((u8 *)creature->trigger_area + 8, 16);
        } else {
            EdFileWriteInt(0);
        }
        if (creature->locator != NULL) {
            EdFileWriteInt(1);
            EdFileWrite((u8 *)creature->locator + 8, 16);
        } else {
            EdFileWriteInt(0);
        }
        if (aidata_version > 16) {
            if (creature->respawn_locator != NULL) {
                EdFileWriteInt(1);
                EdFileWrite((u8 *)creature->respawn_locator + 8, 16);
            } else {
                EdFileWriteInt(0);
            }
        }
        if (aidata_version > 7) {
            EdFileWriteChar(creature->difficulty);
            EdFileWriteChar(creature->min_respawns);
            EdFileWriteChar(creature->max_respawns);
            EdFileWriteChar(creature->activation);
            EdFileWriteFloat(creature->min_respawn_time);
            EdFileWriteFloat(creature->max_respawn_time);
            if (aidata_version > 9)
                EdFileWriteFloat(creature->stagger_start);
            if (creature->activation == 1)
                EdFileWrite((u8 *)creature->activation_area + 8, 16);
        }
        if (aidata_version > 10) {
            if (GetViewRangeFn != NULL &&
                (saved = creature->view_distance, GetViewRangeFn(creature->character_type) == saved))
                EdFileWriteFloat(0.0f);
            else
                EdFileWriteFloat(creature->view_distance);
            if (GetHearDistanceFn != NULL &&
                (saved = creature->hear_distance, GetHearDistanceFn(creature->character_type) == saved))
                EdFileWriteFloat(0.0f);
            else
                EdFileWriteFloat(creature->hear_distance);
            if (GetMaxViewHeightFn != NULL &&
                (saved = creature->max_view_height, GetMaxViewHeightFn(creature->character_type) == saved))
                EdFileWriteFloat(0.0f);
            else
                EdFileWriteFloat(creature->max_view_height);
            if (GetMinViewHeightFn != NULL &&
                (saved = creature->min_view_height, GetMinViewHeightFn(creature->character_type) == saved))
                EdFileWriteFloat(0.0f);
            else
                EdFileWriteFloat(creature->min_view_height);
            EdFileWriteInt(0);
        }
    }
}

i32 creatureEditor_IsSelectable(EDAICREATURE_s *creature) {
    if ((aieditorsettings.display_options & 0x400) && aieditor->selected_creature != NULL &&
        aieditor->selected_creature->set != creature->set)
        return 0;
    return 1;
}

EDAICREATURE_s *__cdecl creatureEditor_GetNearest(i32 use_radius) {
    EDAICREATURE_s *nearest = NULL;
    f32 nearest_distance = FLT_MAX;
    EDAICREATURE_s *creature;
    if (GlobalCharacterHGobjFn == NULL)
        return NULL;
    for (creature = (EDAICREATURE_s *)NuLinkedListGetHead(&aieditor->creatures); creature != NULL;
         creature = (EDAICREATURE_s *)NuLinkedListGetNext(&aieditor->creatures, &creature->link)) {
        AI_CREATURE_MODEL_BOUNDS_PC *model;
        f32 height, min_height, radius, distance;
        NUVEC delta;
        if (!creatureEditor_IsSelectable(creature))
            continue;
        model = (AI_CREATURE_MODEL_BOUNDS_PC *)GlobalCharacterHGobjFn(creature->character_type);
        if (model != NULL) {
            radius = model->radius;
            min_height = model->min_height;
            height = model->height;
        } else {
            radius = 0.4f;
            min_height = 0.0f;
            height = 0.4f;
        }
        distance = NuVecXZDistSqr(&aieditor->camera_position, &creature->position, &delta);
        if (distance < nearest_distance) {
            if (use_radius) {
                f32 lower = creature->position.y + min_height;
                if (lower > creature->position.y) {
                    height += lower - creature->position.y;
                    lower = creature->position.y;
                }
                if (distance < radius * radius &&
                    aieditor->camera_position.y >= lower - AiEditorPickHeightTolerancePc &&
                    aieditor->camera_position.y <= lower + height + AiEditorPickHeightTolerancePc) {
                    nearest_distance = distance;
                    nearest = creature;
                }
            } else {
                nearest_distance = distance;
                nearest = creature;
            }
        }
    }
    return nearest;
}

static void __cdecl AiEditorRespawnCancelMenuPc(eduimenu_s *menu, eduimenu_s *child) {
    (void)child;
    eduiMenuDestroy(menu);
}

static void __cdecl AiEditorSetActivationPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    aieditor->selected_creature->activation = (u8)item->data;
}

static void __cdecl AiEditorSetAreaActivationPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    NULISTLNK *link;
    i32 index = 0;
    (void)menu;
    (void)event;
    if (item != NULL && aieditor->selected_creature != NULL) {
        for (link = NuLinkedListGetHead(&aieditor->areas); link != NULL;
             link = NuLinkedListGetNext(&aieditor->areas, link), ++index) {
            if (index == item->data) {
                aieditor->selected_creature->activation_area = (EDAIAREA_s *)link;
                aieditor->selected_creature->activation = 1;
                return;
            }
        }
    }
}

static void __cdecl AiEditorActivationDifficultyPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (aieditor->selected_creature != NULL)
        aieditor->selected_creature->difficulty = (u8)(i32)((edui_slider_s *)item)->value;
}

void __cdecl creatureEditor_cbActivationMenu(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    EDAIAREA_s *area;
    i32 index = 0;
    char label[32];
    (void)item;
    (void)event;
    if (aieditor->selected_creature == NULL)
        return;
    menu = eduiMenuCreate(220, 70, 240, 250, ed_fnt, AiEditorRespawnCancelMenuPc, "Activation Condition");
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu,
                    eduiItemSliderCreateInt(1, &AiEditorSaveMessageColoursPc, 0, AiEditorActivationDifficultyPc, 1, 9,
                                            aieditor->selected_creature->difficulty, "Activation Difficulty"));
    eduiMenuAddItem(menu,
                    eduiItemCheckCreate(0, &AiEditorSaveMessageColoursPc, aieditor->selected_creature->activation == 0,
                                        1, AiEditorSetActivationPc, "AUTOMATIC"));
    eduiMenuAddItem(menu,
                    eduiItemCheckCreate(2, &AiEditorSaveMessageColoursPc, aieditor->selected_creature->activation == 2,
                                        1, AiEditorSetActivationPc, "SCRIPT"));
    for (area = (EDAIAREA_s *)NuLinkedListGetHead(&aieditor->areas); area != NULL;
         area = (EDAIAREA_s *)NuLinkedListGetNext(&aieditor->areas, &area->link), ++index) {
        i32 selected = aieditor->selected_creature->activation_area == area;
        sprintf(label, "AREA \"%s\"", area->name);
        eduiMenuAddItem(menu, eduiItemCheckCreate(index, &AiEditorSaveMessageColoursPc, selected, 1,
                                                  AiEditorSetAreaActivationPc, label));
        if (selected != 0)
            menu->selected = edui_last_item;
        eduiMenuAttach(parent, menu);
    }
    eduiMenuAttach(parent, menu);
}

static void __cdecl AiEditorRespawnMinCountPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (aieditor->selected_creature != NULL) {
        aieditor->selected_creature->min_respawns = (i32)((edui_slider_s *)item)->value;
        if (aieditor->selected_creature->min_respawns > aieditor->selected_creature->max_respawns) {
            aieditor->selected_creature->min_respawns = aieditor->selected_creature->max_respawns;
            eduiItemSliderSetVal((edui_slider_s *)item, (f32)aieditor->selected_creature->min_respawns);
        }
    }
}

static void __cdecl AiEditorRespawnMaxCountPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (aieditor->selected_creature != NULL) {
        aieditor->selected_creature->max_respawns = (i32)((edui_slider_s *)item)->value;
        if (aieditor->selected_creature->max_respawns < aieditor->selected_creature->min_respawns) {
            aieditor->selected_creature->max_respawns = aieditor->selected_creature->min_respawns;
            eduiItemSliderSetVal((edui_slider_s *)item, (f32)aieditor->selected_creature->max_respawns);
        }
    }
}

static void __cdecl AiEditorRespawnMinTimePc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (aieditor->selected_creature != NULL) {
        aieditor->selected_creature->min_respawn_time = ((edui_slider_s *)item)->value;
        if (aieditor->selected_creature->min_respawn_time > aieditor->selected_creature->max_respawn_time) {
            aieditor->selected_creature->min_respawn_time = aieditor->selected_creature->max_respawn_time;
            eduiItemSliderSetVal((edui_slider_s *)item, aieditor->selected_creature->min_respawn_time);
        }
    }
}

static void __cdecl AiEditorRespawnMaxTimePc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (aieditor->selected_creature != NULL) {
        aieditor->selected_creature->max_respawn_time = ((edui_slider_s *)item)->value;
        if (aieditor->selected_creature->max_respawn_time < aieditor->selected_creature->min_respawn_time) {
            aieditor->selected_creature->max_respawn_time = aieditor->selected_creature->min_respawn_time;
            eduiItemSliderSetVal((edui_slider_s *)item, aieditor->selected_creature->max_respawn_time);
        }
    }
}

void __cdecl creatureEditor_cb_stagger_start(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (aieditor->selected_creature != NULL)
        aieditor->selected_creature->stagger_start = ((edui_slider_s *)item)->value;
}

void __cdecl creatureEditor_cb_assigntoset(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (aieditor->selected_creature != NULL)
        aieditor->selected_creature->set = (u8)((edui_slider_s *)item)->value;
}

static void __cdecl AiEditorSetRespawnLocatorPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    i32 index = 0;
    EDLOCATOR_s *locator;
    (void)menu;
    (void)event;
    if (item == NULL || aieditor->selected_creature == NULL)
        return;
    if (item->data == -1) {
        /* Native clears the base locator, not the respawn locator. */
        aieditor->selected_creature->locator = NULL;
        return;
    }
    for (locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators); locator != NULL;
         locator = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &locator->link), ++index) {
        if (index == item->data) {
            aieditor->selected_creature->respawn_locator = locator;
            return;
        }
    }
}

static void __cdecl AiEditorSelectRespawnLocatorPc(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu = eduiMenuCreate(220, 70, 240, 250, ed_fnt, AiEditorRespawnCancelMenuPc, "Select Locator");
    EDLOCATOR_s *locator;
    i32 index = 0;
    (void)item;
    (void)event;
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu, eduiItemCheckCreate((size_t)-1, &AiEditorSaveMessageColoursPc,
                                              aieditor->selected_creature->respawn_locator == NULL, 1,
                                              AiEditorSetRespawnLocatorPc, "NONE"));
    for (locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators); locator != NULL;
         locator = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &locator->link), ++index) {
        i32 selected = aieditor->selected_creature->respawn_locator == locator;
        eduiMenuAddItem(menu, eduiItemCheckCreate(index, &AiEditorSaveMessageColoursPc, selected, 1,
                                                  AiEditorSetRespawnLocatorPc, locator->name));
        if (selected)
            menu->selected = edui_last_item;
        eduiMenuAttach(parent, menu);
    }
}

void __cdecl creatureEditor_cbRespawnMenu(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    char label[32];
    (void)item;
    (void)event;
    if (aieditor->selected_creature == NULL)
        return;
    menu = eduiMenuCreate(220, 70, 240, 250, ed_fnt, AiEditorRespawnCancelMenuPc, "Respawn Values");
    if (menu == NULL)
        return;
    if (NuLinkedListGetHead(&aieditor->locators) != NULL) {
        if (aieditor->selected_creature->respawn_locator != NULL) {
            EDLOCATOR_s *locator = (EDLOCATOR_s *)aieditor->selected_creature->respawn_locator;
            sprintf(label, "Respawn Locator \"%s\"", locator->name);
        } else {
            sprintf(label, "Respawn Locator NONE");
        }
        eduiMenuAddItem(
            menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, AiEditorSelectRespawnLocatorPc, label));
    }
    eduiMenuAddItem(menu,
                    eduiItemSliderCreateInt(1, &AiEditorSaveMessageColoursPc, 0, AiEditorRespawnMinCountPc, -1, 33,
                                            (i8)aieditor->selected_creature->min_respawns, "Min Num Respawns"));
    eduiMenuAddItem(menu,
                    eduiItemSliderCreateInt(1, &AiEditorSaveMessageColoursPc, 0, AiEditorRespawnMaxCountPc, -1, 33,
                                            (i8)aieditor->selected_creature->max_respawns, "Max Num Respawns"));
    eduiMenuAddItem(menu,
                    eduiItemSliderCreate(1, &AiEditorSaveMessageColoursPc, 0, AiEditorRespawnMinTimePc, 0.0f, 60.0f,
                                         aieditor->selected_creature->min_respawn_time, "Min Respawn Time"));
    eduiMenuAddItem(menu,
                    eduiItemSliderCreate(1, &AiEditorSaveMessageColoursPc, 0, AiEditorRespawnMaxTimePc, 0.0f, 60.0f,
                                         aieditor->selected_creature->max_respawn_time, "Max Respawn Time"));
    eduiMenuAttach(parent, menu);
}

static void __cdecl AiEditorViewDistancePc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (aieditor->selected_creature != NULL) {
        aieditor->selected_creature->view_distance = ((edui_slider_s *)item)->value;
        if (aieditor->selected_creature->view_distance < aieditor->selected_creature->hear_distance) {
            aieditor->selected_creature->view_distance = aieditor->selected_creature->hear_distance;
            eduiItemSliderSetVal((edui_slider_s *)item, aieditor->selected_creature->view_distance);
        }
    }
}

static void __cdecl AiEditorHearDistancePc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (aieditor->selected_creature != NULL) {
        aieditor->selected_creature->hear_distance = ((edui_slider_s *)item)->value;
        if (aieditor->selected_creature->hear_distance > aieditor->selected_creature->view_distance) {
            aieditor->selected_creature->hear_distance = aieditor->selected_creature->view_distance;
            eduiItemSliderSetVal((edui_slider_s *)item, aieditor->selected_creature->hear_distance);
        }
    }
}

static void __cdecl AiEditorMaxViewHeightPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (aieditor->selected_creature != NULL)
        aieditor->selected_creature->max_view_height = ((edui_slider_s *)item)->value;
}

static void __cdecl AiEditorMinViewHeightPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (aieditor->selected_creature != NULL)
        aieditor->selected_creature->min_view_height = -((edui_slider_s *)item)->value;
}

void __cdecl creatureEditor_cbVisionMenu(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    (void)item;
    (void)event;
    if (aieditor->selected_creature == NULL)
        return;
    menu = eduiMenuCreate(220, 70, 240, 250, ed_fnt, AiEditorRespawnCancelMenuPc, "Vision");
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu, eduiItemSliderCreate(1, &AiEditorSaveMessageColoursPc, 0, AiEditorViewDistancePc, 0.5f, 49.5f,
                                               aieditor->selected_creature->view_distance, "View Distance"));
    eduiMenuAddItem(menu, eduiItemSliderCreate(1, &AiEditorSaveMessageColoursPc, 0, AiEditorHearDistancePc, 0.5f, 49.5f,
                                               aieditor->selected_creature->hear_distance, "Hearing Distance"));
    eduiMenuAddItem(menu, eduiItemSliderCreate(1, &AiEditorSaveMessageColoursPc, 0, AiEditorMaxViewHeightPc, 0.1f,
                                               99.9f, aieditor->selected_creature->max_view_height, "Max View Height"));
    eduiMenuAddItem(menu,
                    eduiItemSliderCreate(1, &AiEditorSaveMessageColoursPc, 0, AiEditorMinViewHeightPc, 0.1f, 99.9f,
                                         -aieditor->selected_creature->min_view_height, "Min View Height -"));
    eduiMenuAttach(parent, menu);
}

static void __cdecl creatureEditor_cbSetType(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (item != NULL) {
        aieditorsettings.character_type = item->data;
    }
    if (aieditor->selected_creature != NULL && aieditor->nearest_creature == aieditor->selected_creature) {
        aieditor->selected_creature->character_type = (i16)aieditorsettings.character_type;
        if (GetViewRangeFn != NULL) {
            aieditor->selected_creature->view_distance = GetViewRangeFn(aieditorsettings.character_type);
        } else {
            aieditor->selected_creature->view_distance = 1.0f;
        }
        if (GetHearDistanceFn != NULL) {
            aieditor->selected_creature->hear_distance = GetHearDistanceFn(aieditorsettings.character_type);
        } else {
            aieditor->selected_creature->hear_distance = 1.0f;
        }
        if (GetMaxViewHeightFn != NULL) {
            aieditor->selected_creature->max_view_height = GetMaxViewHeightFn(aieditorsettings.character_type);
        } else {
            aieditor->selected_creature->max_view_height = 1.0f;
        }
        if (GetMinViewHeightFn != NULL) {
            aieditor->selected_creature->min_view_height = GetMinViewHeightFn(aieditorsettings.character_type);
        } else {
            aieditor->selected_creature->min_view_height = 1.0f;
        }
    }
}

void __cdecl creatureEditor_cbSelectType(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    i32 index;
    i32 type;
    i32 selected;
    (void)item;
    (void)event;
    if (LevelCharacterGlobalIDFn == NULL || GlobalCharacterNameFn == NULL)
        return;
    menu = eduiMenuCreate(220, 70, 240, 250, ed_fnt, AiEditorRespawnCancelMenuPc, "Select AI Type");
    if (menu == NULL)
        return;
    index = 0;
    type = LevelCharacterGlobalIDFn(index);
    while (type != -1) {
        selected = aieditorsettings.character_type == type;
        eduiMenuAddItem(menu, eduiItemCheckCreate(type, &AiEditorSaveMessageColoursPc, selected, 1,
                                                  creatureEditor_cbSetType, GlobalCharacterNameFn(type)));
        if (selected)
            menu->selected = edui_last_item;
        eduiMenuAttach(parent, menu);
        type = LevelCharacterGlobalIDFn(++index);
    }
}

void __cdecl aieditor_SetCurrentScript(char *name, EDAICREATURE_s *creature) {
    AISCRIPT_PC *script;

    if (NuStrICmp(name, aieditorsettings.script_name) != 0)
        strcpy(aieditorsettings.script_name, name);
    if (creature != NULL) {
        aieditorsettings.script_params[0] = creature->script_params[0];
        aieditorsettings.script_params[1] = creature->script_params[1];
        aieditorsettings.script_params[2] = creature->script_params[2];
        aieditorsettings.script_params[3] = creature->script_params[3];
        aieditorsettings.script_flags = creature->flags & 0x1e;
        return;
    }
    script = AIScriptFind(aieditor->ai_system, aieditorsettings.script_name, 1, 1, 1);
    if (script != NULL) {
        aieditorsettings.script_params[0] = script->params[0].default_value;
        aieditorsettings.script_params[1] = script->params[1].default_value;
        aieditorsettings.script_params[2] = script->params[2].default_value;
        aieditorsettings.script_params[3] = script->params[3].default_value;
        aieditorsettings.script_flags = 0;
        return;
    }
    aieditorsettings.script_flags = 0;
    aieditorsettings.script_params[3] = aieditorsettings.script_params[2] = aieditorsettings.script_params[1] =
        aieditorsettings.script_params[0] = 0.0f;
}

void __cdecl creatureEditor_cbSetScript(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    char *name;
    (void)menu;
    (void)event;
    if (item != NULL && aieditor->selected_creature != NULL) {
        name = AIScriptNameFromIx(aieditor->ai_system, item->data);
        if (name != NULL) {
            aieditor_SetCurrentScript(name, NULL);
            if (NuStrICmp(aieditorsettings.script_name, aieditor->selected_creature->script_name) != 0) {
                strcpy(aieditor->selected_creature->script_name, aieditorsettings.script_name);
                aieditor->selected_creature->script_params[0] = aieditorsettings.script_params[0];
                aieditor->selected_creature->script_params[1] = aieditorsettings.script_params[1];
                aieditor->selected_creature->script_params[2] = aieditorsettings.script_params[2];
                aieditor->selected_creature->script_params[3] = aieditorsettings.script_params[3];
                aieditor->selected_creature->flags &= ~0x1e;
                aieditor->selected_creature->flags |= aieditorsettings.script_flags;
            }
        }
    }
}

void __cdecl creatureEditor_cbSelectScript(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    char *name;
    i32 index;
    i32 selected;
    (void)item;
    (void)event;
    menu = eduiMenuCreate(220, 70, 240, 250, ed_fnt, AiEditorRespawnCancelMenuPc, "Select Script");
    if (menu != NULL) {
        index = 0;
        do {
            selected = 0;
            name = AIScriptNameFromIx(aieditor->ai_system, index);
            if (name != NULL && NuStrICmp(name, AIScriptKeywordLevel) != 0) {
                if (aieditor->selected_creature != NULL &&
                    NuStrICmp(aieditor->selected_creature->script_name, name) == 0)
                    selected = 1;
                eduiMenuAddItem(menu, eduiItemCheckCreate(index, &AiEditorSaveMessageColoursPc, selected, 1,
                                                          creatureEditor_cbSetScript, name));
                if (selected)
                    menu->selected = edui_last_item;
                eduiMenuAttach(parent, menu);
            }
            ++index;
        } while (name != NULL);
    }
}

static void __cdecl AiEditorSetTriggerAreaPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDAIAREA_s *area;
    i32 index = 0;
    (void)menu;
    (void)event;
    if (item == NULL || aieditor->selected_creature == NULL)
        return;
    if (item->data == -1) {
        aieditor->selected_creature->trigger_area = NULL;
        return;
    }
    for (area = (EDAIAREA_s *)NuLinkedListGetHead(&aieditor->areas); area != NULL;
         area = (EDAIAREA_s *)NuLinkedListGetNext(&aieditor->areas, &area->link), ++index) {
        if (index == item->data) {
            aieditor->selected_creature->trigger_area = area;
            return;
        }
    }
}

void __cdecl creatureEditor_cbSelectTriggerArea(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu = eduiMenuCreate(220, 70, 240, 250, ed_fnt, AiEditorRespawnCancelMenuPc, "Select Trigger Area");
    EDAIAREA_s *area;
    i32 index = 0;
    i32 selected;
    (void)item;
    (void)event;
    if (menu == NULL)
        return;
    selected = aieditor->selected_creature->trigger_area == NULL;
    eduiMenuAddItem(menu, eduiItemCheckCreate((size_t)-1, &AiEditorSaveMessageColoursPc, selected, 1,
                                              AiEditorSetTriggerAreaPc, "NONE"));
    if (selected)
        menu->selected = edui_last_item;
    eduiMenuAttach(parent, menu);
    for (area = (EDAIAREA_s *)NuLinkedListGetHead(&aieditor->areas); area != NULL;
         area = (EDAIAREA_s *)NuLinkedListGetNext(&aieditor->areas, &area->link), ++index) {
        selected = aieditor->selected_creature->trigger_area == area;
        eduiMenuAddItem(menu, eduiItemCheckCreate(index, &AiEditorSaveMessageColoursPc, selected, 1,
                                                  AiEditorSetTriggerAreaPc, area->name));
        if (selected)
            menu->selected = edui_last_item;
        eduiMenuAttach(parent, menu);
    }
}

static void __cdecl AiEditorSetLocatorPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDLOCATOR_s *locator;
    i32 index = 0;
    (void)menu;
    (void)event;
    if (item == NULL || aieditor->selected_creature == NULL)
        return;
    if (item->data == -1) {
        aieditor->selected_creature->locator = NULL;
        return;
    }
    for (locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators); locator != NULL;
         locator = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &locator->link)) {
        if ((void *)aieditor->selected_creature->check.path == (void *)locator->path_check.path) {
            if (index == item->data) {
                aieditor->selected_creature->locator = locator;
                return;
            }
            ++index;
        }
    }
}

void __cdecl AiEditorSelectLocatorMenuPc(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu = eduiMenuCreate(220, 70, 240, 250, ed_fnt, AiEditorRespawnCancelMenuPc, "Select Locator");
    EDLOCATOR_s *locator;
    i32 index = 0;
    (void)item;
    (void)event;
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu,
                    eduiItemCheckCreate((size_t)-1, &AiEditorSaveMessageColoursPc,
                                        aieditor->selected_creature->locator == NULL, 1, AiEditorSetLocatorPc, "NONE"));
    for (locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators); locator != NULL;
         locator = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &locator->link)) {
        if ((void *)aieditor->selected_creature->check.path == (void *)locator->path_check.path) {
            i32 selected = aieditor->selected_creature->locator == locator;
            eduiMenuAddItem(menu, eduiItemCheckCreate(index, &AiEditorSaveMessageColoursPc, selected, 1,
                                                      AiEditorSetLocatorPc, locator->name));
            if (selected)
                menu->selected = edui_last_item;
            eduiMenuAttach(parent, menu);
            ++index;
        }
    }
}

static void __cdecl AiEditorRenameCreaturePc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDAICREATURE_s *creature;
    (void)menu;
    (void)event;
    if (aieditor->selected_creature != NULL) {
        char *name = ((edui_textpicker_s *)item)->value;
        if (strlen(name) != 0) {
            for (creature = (EDAICREATURE_s *)NuLinkedListGetHead(&aieditor->creatures); creature != NULL;
                 creature = (EDAICREATURE_s *)NuLinkedListGetNext(&aieditor->creatures, &creature->link)) {
                if (NuStrICmp(creature->name, name) == 0)
                    return;
            }
            strcpy(aieditor->selected_creature->name, name);
        }
    }
}

void __cdecl creatureEditor_cbRenameCreatureMenu(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    (void)item;
    (void)event;
    if (aieditor->selected_creature == NULL)
        return;
    menu = eduiMenuCreate(240, 90, 240, 250, ed_fnt, AiEditorRespawnCancelMenuPc, "Rename Creature");
    if (menu == NULL)
        return;
    eduiMenuAddItem(
        menu, eduiItemTextPickCreate(0, &AiEditorSaveMessageColoursPc, AiEditorRenameCreaturePc, "Creature Name"));
    strcpy(((edui_textpicker_s *)edui_last_item)->value, aieditor->selected_creature->name);
    ((edui_textpicker_s *)edui_last_item)->max_length = 15;
    eduiMenuAttach(parent, menu);
    menu->x = parent->x + 10;
    menu->y = parent->y + 40;
}

void __cdecl creatureEditor_cbDeleteCreature(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (item != NULL && item->data != 0 && aieditor->selected_creature != NULL &&
        aieditor->selected_creature == aieditor->nearest_creature) {
        creatureEditorDestroyCreature(aieditor->selected_creature);
        aieditor->selected_creature = NULL;
    }
    AiEditorClearMainMenuPc();
}

void __cdecl pathEditor_cbCancelDeleteCreatureMenu(eduimenu_s *menu, eduimenu_s *child) {
    (void)menu;
    (void)child;
    AiEditorClearMainMenuPc();
}

void __cdecl creatureEditor_cbFlagsToggle(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (aieditor->selected_creature != NULL) {
        if (aieditor->selected_creature->flags & item->data)
            aieditor->selected_creature->flags &= ~item->data;
        else
            aieditor->selected_creature->flags |= item->data;
    }
}

void __cdecl creatureEditor_PathNodeMoved(EDAIPATHNODE_s *node) {
    EDAICREATURE_s *creature;
    NUVEC difference;
    NUVEC direction;
    NUVEC movement;
    f32 radius;
    f32 normalized_x;
    f32 old_y;
    i32 angle;

    for (creature = (EDAICREATURE_s *)NuLinkedListGetHead(&aieditor->creatures); creature != NULL;
         creature = (EDAICREATURE_s *)NuLinkedListGetNext(&aieditor->creatures, &creature->link)) {
        if (creature->check.first == node || creature->check.second == node) {
            NuVecSub(&difference, &creature->check.second->position, &creature->check.first->position);
            NuVecNorm(&direction, &difference);
            if (creature->check.fraction > 1.0f) {
                radius = creature->check.second->radius;
            } else if (creature->check.fraction < 0.0f) {
                radius = creature->check.first->radius;
            } else {
                radius = (1.0f - creature->check.fraction) * creature->check.first->radius +
                         creature->check.second->radius * creature->check.fraction;
            }
            normalized_x = direction.x;
            direction.x = direction.z * radius;
            direction.z = -normalized_x * radius;
            old_y = creature->position.y;
            creature->position = creature->check.first->position;
            NuVecScale(&movement, &difference, creature->check.fraction);
            NuVecAdd(&creature->position, &creature->position, &movement);
            NuVecScale(&movement, &direction, creature->check.width);
            NuVecAdd(&creature->position, &creature->position, &movement);
            if (AIPathFabs(old_y - creature->position.y) > 1.5f) {
                creature->position.y = old_y;
            }
            angle = (i32)(NuAtan2(difference.x, difference.z) * 10430.378f);
            creature->angle = NuAngAdd(angle, creature->check.angle);
        }
    }
}

void __cdecl creatureEditor_PathNodeDeleted(EDAIPATHNODE_s *node) {
    EDAICREATURE_s *creature;
    EDAICREATURE_s *next;
    creature = (EDAICREATURE_s *)NuLinkedListGetHead(&aieditor->creatures);
    while (creature != NULL) {
        next = (EDAICREATURE_s *)NuLinkedListGetNext(&aieditor->creatures, &creature->link);
        if (creature->check.first == node || creature->check.second == node) {
            pathEditor_OnPathCheck(&creature->position, &creature->check, aieditor->current_path, 0.0f);
            if (creature->check.on_path == 0) {
                NuLinkedListRemove(&aieditor->creatures, &creature->link);
                memset(creature, 0, sizeof(*creature));
                NuLinkedListAppend(&aieditor->free_creatures, &creature->link);
                if (creature == aieditor->selected_creature)
                    aieditor->selected_creature = NULL;
            }
        }
        creature = next;
    }
}

void __cdecl creatureEditor_PathDeleted(EDAIPATH_s *path) {
    EDAICREATURE_s *creature;
    EDAICREATURE_s *next;
    creature = (EDAICREATURE_s *)NuLinkedListGetHead(&aieditor->creatures);
    while (creature != NULL) {
        next = (EDAICREATURE_s *)NuLinkedListGetNext(&aieditor->creatures, &creature->link);
        if (creature->check.path == path) {
            NuLinkedListRemove(&aieditor->creatures, &creature->link);
            memset(creature, 0, sizeof(*creature));
            NuLinkedListAppend(&aieditor->free_creatures, &creature->link);
            if (creature == aieditor->selected_creature)
                aieditor->selected_creature = NULL;
        }
        creature = next;
    }
}

i32 creatureEditor_CalculatePos(EDAICREATURE_s *creature, i32 index, NUVEC *position, i32 check_path) {
    i32 column;
    NUVEC offset;
    if (index == 0) {
        *position = creature->position;
        return 1;
    }
    column = index % creature->across_count;
    offset.x = ((column + 1) / 2) * creature->x_spacing;
    if (column & 1)
        offset.x = -offset.x;
    offset.y = 0.0f;
    offset.z = -creature->z_spacing * (index / creature->across_count);
    NuVecRotateY(&offset, &offset, creature->angle);
    NuVecAdd(position, &offset, &creature->position);
    if (check_path) {
        EDAIPATHCHECK_s result;
        pathEditor_OnPathCheck(position, &result, creature->check.path, 0.0f);
        return result.on_path;
    }
    return 1;
}

/* Native 52a710: private creature ESI. */
static void creatureEditor_Updated(EDAICREATURE_s *creature) {
    i32 index;
    NUVEC position;

    creature->valid_positions = 1;
    for (index = 1; index < creature->group_count; index++) {
        if (creatureEditor_CalculatePos(creature, index, &position, 1)) {
            creature->valid_positions |= (u32)((u64)1 << index);
        }
    }
}

void creatureEditor_RenderAllCreatures(void) {
    EDAICREATURE_s *creature;
    if (GlobalCharacterRenderFn == NULL)
        return;
    for (creature = (EDAICREATURE_s *)NuLinkedListGetHead(&aieditor->creatures); creature != NULL;
         creature = (EDAICREATURE_s *)NuLinkedListGetNext(&aieditor->creatures, &creature->link)) {
        i32 group;
        for (group = 0; group < creature->group_count; ++group) {
            NURND_VERTEX3D_PC vertices[2];
            NUVEC position;
            vertices[0].colour = -1;
            vertices[1].colour = -1;
            if (!(creature->valid_positions & ((u64)1 << group)) || !creatureEditor_IsSelectable(creature))
                continue;
            creatureEditor_CalculatePos(creature, group, &position, 0);
            GlobalCharacterRenderFn(&position, creature->angle, creature->character_type, 0, creature);
            if (creature->locator != NULL) {
                vertices[0].position = creature->position;
                vertices[1].position = ((EDLOCATOR_s *)creature->locator)->position;
                AiRndrLine3d(vertices, NULL, NULL);
            }
            if (creature->respawn_locator != NULL) {
                vertices[0].colour = 0x8000;
                vertices[1].colour = 0x8000;
                vertices[0].position = creature->position;
                vertices[1].position = ((EDLOCATOR_s *)creature->respawn_locator)->position;
                vertices[0].position.y += (double)0.1f;
                vertices[1].position.y += (double)0.1f;
                AiRndrLine3d(vertices, NULL, NULL);
            }
        }
    }
}

void creatureEditor_Render(i32 x, i32 y, f32 xscale, f32 yscale) {
    char set_name[16];
    NUVEC displacement;
    EDAICREATURE_s *display, *creature;
    i32 text_x;
    if (GlobalCharacterRenderFn == NULL)
        return;
    text_x = (x + 10) * 16;
    NuQFntPrintEx(system_qfont, text_x, y * 8 - 40, 16, "Creature Editor  (%d placed)",
                  NuLinkedListCheck(&aieditor->creatures));
    NuQFntSetColour(system_qfont, 0x80000000);
    NuQFntSetScale(system_qfont, xscale, yscale);
    display = aieditor->selected_creature;
    if (display == NULL)
        display = aieditor->nearest_creature;
    if (display != NULL) {
        f32 distance = NuVecXZDist(&display->position, &aieditor->camera_position, &displacement);
        NuQFntPrintEx(system_qfont, text_x, y * 8 + 120, 16, "\"%s\", xzrng=%.2f", display->name, distance);
        if (display->set)
            sprintf(set_name, "Set=%d", display->set);
        else
            sprintf(set_name, "Set=NONE");
        if (strlen(display->script_name))
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 240, 16, "Script = \"%s\", %s", display->script_name, set_name);
        else
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 240, 16, "Script = NONE, %s", set_name);
        if (display->trigger_area != NULL)
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 360, 16, "Area = \"%s\"",
                          ((EDLOCATOR_s *)display->trigger_area)->name);
        if (display->locator != NULL)
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 480, 16, "Locator = \"%s\"",
                          ((EDLOCATOR_s *)display->locator)->name);
    }
    NuQFntPrintEx(system_qfont, text_x, y * 8 + 600, 16, "SQR - Options");
    if (aieditor->nearest_creature != NULL) {
        if (aieditor->selected_creature == aieditor->nearest_creature) {
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 720, 16, "X - Move selected");
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 840, 16, "TRI - Delete selected");
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 960, 16, "LLEFT/LRight - Rotate");
        } else {
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 720, 16, "X - Select creature");
        }
    } else {
        NuQFntPrintEx(system_qfont, text_x, y * 8 + 720, 16, "X - Create creature");
        if (aieditor->selected_creature != NULL)
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 840, 16, "TRI - Deselect selected");
        NuQFntPrintEx(system_qfont, text_x, y * 8 + 960, 16, "LLEFT/LRight - Rotate");
    }
    if (aieditor->selected_creature != NULL && aieditor->nearest_locator != NULL)
        NuQFntPrintEx(system_qfont, text_x, y * 8 + 720, 16, "O - Set locator.");
    for (creature = (EDAICREATURE_s *)NuLinkedListGetHead(&aieditor->creatures); creature != NULL;
         creature = (EDAICREATURE_s *)NuLinkedListGetNext(&aieditor->creatures, &creature->link)) {
        i32 group;
        i32 colour = creature == aieditor->selected_creature
                         ? (creature == aieditor->nearest_creature ? 0xff0000ff : 0x800000ff)
                         : (creature == aieditor->nearest_creature ? -1 : 0);
        for (group = 0; group < creature->group_count; ++group, colour = 0) {
            NUVEC position;
            NURND_VERTEX3D_PC line[2];
            if (!(creature->valid_positions & ((u64)1 << group)) || !creatureEditor_IsSelectable(creature))
                continue;
            creatureEditor_CalculatePos(creature, group, &position, 0);
            GlobalCharacterRenderFn(&position, creature->angle, creature->character_type, colour, creature);
            if (creature->locator != NULL) {
                line[0].position = creature->position;
                line[1].position = ((EDLOCATOR_s *)creature->locator)->position;
                line[0].colour = colour;
                line[1].colour = colour;
                AiRndrLine3d(line, NULL, NULL);
            }
            if (creature->respawn_locator != NULL) {
                line[0].position = creature->position;
                line[1].position = ((EDLOCATOR_s *)creature->respawn_locator)->position;
                line[0].position.y += (double)0.1f;
                line[1].position.y += (double)0.1f;
                colour += 0x8000;
                line[0].colour = colour;
                line[1].colour = colour;
                AiRndrLine3d(line, NULL, NULL);
            }
        }
    }
    if (aieditor->nearest_creature == NULL && aieditor->cursor_check.on_path && aieditor->preview_width < 0.5f &&
        aieditorsettings.character_type >= 0) {
        GlobalCharacterRenderFn(&aieditor->camera_position, (u16)aieditorsettings.locator_rotation,
                                aieditorsettings.character_type, 0, NULL);
    }
    pathEditorDrawPaths();
    areaEditorDrawAreas();
    locatorEditorDrawLocators();
    antinodeEditorDrawAntinodes();
}

void __cdecl creatureEditor_Enter(void) {
    AICREATURE_PC *source;
    EDAICREATURE_s *creature;
    EDAIPATH_s *path;
    AISCRIPT_PC *script;
    EDAIAREA_s *area;
    EDLOCATOR_s *locator;
    NULISTHDR *list;
    char *name;
    f32 offset;
    i32 i;
    list = &aieditor->creatures;
    list->head = NULL;
    list->tail = NULL;
    for (i = 0; i < 128; ++i)
        NuLinkedListAppend(&aieditor->free_creatures, &aieditor->creature_pool[i].link);
    if (aieditor->ai_system != NULL) {
        for (i = 0; i < aieditor->ai_system->creature_count; ++i) {
            source = &aieditor->ai_system->creatures[i];
            creature = CreateCreature(source->type, &source->position, source->y_rotation);
            if (creature == NULL)
                continue;
            path = pathEditor_GetPath(source->path_info.path->name);
            offset = 0.0f;
            pathEditor_OnPathCheck(&creature->position, &creature->check, path, offset);
            while (creature->check.on_path == 0) {
                offset = offset + (f64)0.01f;
                pathEditor_OnPathCheck(&creature->position, &creature->check, path, offset);
            }
            creature->check.angle = NuAngSub(creature->angle, creature->check.angle);
            strcpy(creature->name, source->name);
            strcpy(creature->script_name, source->script_name);
            creature->set = source->set;
            creature->group_count = source->count;
            creature->across_count = source->count_across;
            creature->valid_positions = source->active_mask;
            creature->x_spacing = source->x_spacing;
            creature->z_spacing = source->z_spacing;
            creature->flags = source->flags;
            creature->script_params[0] = source->script_params[0];
            creature->script_params[1] = source->script_params[1];
            creature->script_params[2] = source->script_params[2];
            creature->script_params[3] = source->script_params[3];
            name = creature->script_name;
            if (name != NULL) {
                if (aieditor->ai_system != NULL) {
                    for (script = (AISCRIPT_PC *)NuLinkedListGetHead(&aieditor->ai_system->scripts); script != NULL;
                         script = (AISCRIPT_PC *)NuLinkedListGetNext(&aieditor->ai_system->scripts, &script->link)) {
                        if (NuStrICmp(name, script->name) == 0)
                            goto found;
                    }
                }
                for (script = (AISCRIPT_PC *)NuLinkedListGetHead(&global_aiscripts); script != NULL;
                     script = (AISCRIPT_PC *)NuLinkedListGetNext(&global_aiscripts, &script->link)) {
                    if (NuStrICmp(name, script->name) == 0)
                        goto found;
                }
            }
            for (script = (AISCRIPT_PC *)NuLinkedListGetHead(&global_aiscripts); script != NULL;
                 script = (AISCRIPT_PC *)NuLinkedListGetNext(&global_aiscripts, &script->link)) {
                if (NuStrICmp(GizmoDefaultOutputName, script->name) == 0)
                    goto found;
            }
            goto areas;
        found:
            if ((source->flags & 2) == 0)
                creature->script_params[0] = script->params[0].default_value;
            if ((source->flags & 4) == 0)
                creature->script_params[1] = script->params[1].default_value;
            if ((source->flags & 8) == 0)
                creature->script_params[2] = script->params[2].default_value;
            if ((source->flags & 0x10) == 0)
                creature->script_params[3] = script->params[3].default_value;
        areas:
            if (source->area != NULL) {
                for (area = (EDAIAREA_s *)NuLinkedListGetHead(&aieditor->areas); area != NULL;
                     area = (EDAIAREA_s *)NuLinkedListGetNext(&aieditor->areas, &area->link)) {
                    if (NuStrICmp(area->name, (char *)source->area) == 0) {
                        creature->trigger_area = area;
                        break;
                    }
                }
            }
            if (source->locator != NULL) {
                for (locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators); locator != NULL;
                     locator = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &locator->link)) {
                    if (NuStrICmp(locator->name, (char *)source->locator) == 0) {
                        creature->locator = locator;
                        break;
                    }
                }
            }
            if (source->respawn_locator != NULL) {
                for (locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators); locator != NULL;
                     locator = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &locator->link)) {
                    if (NuStrICmp(locator->name, (char *)source->respawn_locator) == 0) {
                        creature->respawn_locator = locator;
                        break;
                    }
                }
            }
            creature->activation = source->activation_type;
            if (source->activation_type == 1) {
                creature->activation = 0;
                if (source->activate_area != NULL) {
                    for (area = (EDAIAREA_s *)NuLinkedListGetHead(&aieditor->areas); area != NULL;
                         area = (EDAIAREA_s *)NuLinkedListGetNext(&aieditor->areas, &area->link)) {
                        if (NuStrICmp(area->name, (char *)source->activate_area) == 0) {
                            creature->activation_area = area;
                            creature->activation = 1;
                            break;
                        }
                    }
                }
            }
            creature->difficulty = source->activation_difficulty;
            creature->min_respawns = source->min_respawn_count;
            creature->max_respawns = source->max_respawn_count;
            creature->min_respawn_time = source->min_respawn_time;
            creature->max_respawn_time = source->max_respawn_time;
            creature->stagger_start = source->start_stagger;
            creature->view_distance = source->view_distance;
            creature->hear_distance = source->hear_distance;
            creature->max_view_height = source->max_view_height;
            creature->min_view_height = source->min_view_height;
        }
    }
    if (aieditorsettings.character_type == -1 && LevelCharacterGlobalIDFn != NULL)
        aieditorsettings.character_type = LevelCharacterGlobalIDFn(0);
    if (ClearAICreaturesFn != NULL)
        ClearAICreaturesFn();
    if (strlen(aieditorsettings.creature_name) != 0) {
        for (creature = (EDAICREATURE_s *)NuLinkedListGetHead(&aieditor->creatures); creature != NULL;
             creature = (EDAICREATURE_s *)NuLinkedListGetNext(&aieditor->creatures, &creature->link)) {
            if (NuStrICmp(creature->name, aieditorsettings.creature_name) == 0) {
                aieditor->selected_creature = creature;
                if (aieditor->selected_creature != NULL)
                    aieditor_SetCurrentScript(aieditor->selected_creature->script_name, aieditor->selected_creature);
                return;
            }
        }
    }
}

void __cdecl AITerrInit(void) {
}

f32 __cdecl AITerrShadow(NUVEC *position, f32 height_above, f32 height_below, i32 terrain_mask) {
    f32 height;
    if (geom == 0)
        height = NewShadow(position, height_above, height_below, terrain_mask);
    return height;
}

i32 __cdecl AITerrShadowOnPlatform(void) {
    i32 platform = -1;
    if (geom == 0)
        platform = NewShadowOnPlatform();
    return platform;
}

void __cdecl xConditions(NUFPAR_PC *parser) {
    i32 complex_idx = 0;

    while (NuFParGetLine(parser) != 0) {
        AICONDITION_PC *cond;
        AICONDITIONDEF_PC *definition;

        NuFParGetWord(parser);

        if (NuStrICmp(parser->word_buffer_ptr, "}") == 0) {
            return;
        }

        if (NuStrICmp(parser->word_buffer_ptr, "IF") == 0) {
            NuFParGetWord(parser);

            definition = AIConditionFind(parser->word_buffer_ptr);
            if (definition != NULL) {

                cond = (AICONDITION_PC *)AIScriptBufferAllocOrWarn(aiscript_load_buffer, aiscript_load_end_buffer,
                                                                   sizeof(AICONDITION_PC));
                if (cond == NULL) {
                    continue;
                }

                memset(cond, 0, sizeof(AICONDITION_PC));

                NuLinkedListAppend(aiscript_load_conditions, &cond->link);

                cond->definition = definition;

                NuFParGetWord(parser);

                if (NuStrICmp(parser->word_buffer_ptr, "=") == 0 || NuStrICmp(parser->word_buffer_ptr, "==") == 0) {
                    cond->comparison = EQ;
                } else if (NuStrICmp(parser->word_buffer_ptr, "<") == 0) {
                    cond->comparison = LT;
                } else if (NuStrICmp(parser->word_buffer_ptr, ">") == 0) {
                    cond->comparison = GT;
                } else if (NuStrICmp(parser->word_buffer_ptr, "<=") == 0) {
                    cond->comparison = LT_EQ;
                } else if (NuStrICmp(parser->word_buffer_ptr, ">=") == 0) {
                    cond->comparison = GT_EQ;
                } else if (NuStrICmp(parser->word_buffer_ptr, "!=") == 0) {
                    cond->comparison = NEQ;
                } else {
                    cond->argument =
                        AIScriptCopyStringPc(parser->word_buffer_ptr, aiscript_load_buffer, aiscript_load_end_buffer);

                    NuFParGetWord(parser);

                    if (NuStrICmp(parser->word_buffer_ptr, "=") == 0 || NuStrICmp(parser->word_buffer_ptr, "==") == 0) {
                        cond->comparison = EQ;
                    } else if (NuStrICmp(parser->word_buffer_ptr, "<") == 0) {
                        cond->comparison = LT;
                    } else if (NuStrICmp(parser->word_buffer_ptr, ">") == 0) {
                        cond->comparison = GT;
                    } else if (NuStrICmp(parser->word_buffer_ptr, "<=") == 0) {
                        cond->comparison = LT_EQ;
                    } else if (NuStrICmp(parser->word_buffer_ptr, ">=") == 0) {
                        cond->comparison = GT_EQ;
                    } else if (NuStrICmp(parser->word_buffer_ptr, "!=") == 0) {
                        cond->comparison = NEQ;
                    }
                }

                NuFParGetWord(parser);

                if (cond->parameter_condition == NULL) {
                    i8 param_index;
                    i32 i;
                    char *param;

                    param = NuStrIStr(parser->word_buffer_ptr, "param");
                    if (param == NULL) {
                        for (i = 0; i < 4; i++) {
                            char *param_name = aiscript_load_script->params[i].name;

                            if (param_name != NULL && NuStrICmp(param_name, parser->word_buffer_ptr) == 0) {
                                cond->param_index = i;
                                cond->flags |= 4;

                                break;
                            }
                        }
                    } else {
                        param_index = NuAToI(param);
                        cond->param_index = param_index;

                        if (param_index >= 0 && param_index < 4) {
                            cond->flags |= 4;
                        }
                    }

                    if (!(cond->flags & 4)) {

                        for (i = 0; i < AIScriptConstCursorPc; i++) {
                            if (NuStrICmp(AIScriptConstantRecordsPc[i].name, parser->word_buffer_ptr) == 0) {
                                cond->param_value = AIScriptConstantRecordsPc[i].default_val;

                                break;
                            }
                        }

                        if (i == AIScriptConstCursorPc) {
                            cond->param_value = AiParseExpression(parser->word_buffer_ptr);

                            if (AiParseExpressionFailed) {
                                cond->flags |= 8;
                                cond->param_index = complex_idx++;
                                cond->complex_expression = AIScriptCopyStringPc(
                                    parser->word_buffer_ptr, aiscript_load_buffer, aiscript_load_end_buffer);
                            }
                        }
                    }
                }

                ConditionsParseFinalise(cond, parser);
            } else if (AIActionDiagnosticMode == 1) {
                AIScriptMissingConditionDiagnostic(parser->word_buffer_ptr, aiscript_load_script->name,
                                                   aiscript_load_state != NULL ? aiscript_load_state->name
                                                                               : (char *)Text_UnknownText);
            } else {
                AIScriptMissingStateDiagnostic(parser->word_buffer_ptr, aiscript_load_script->name,
                                               aiscript_load_state != NULL ? aiscript_load_state->name
                                                                           : (char *)Text_UnknownText);
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "OnMacro") == 0) {
            AICONDITIONMACRO_PC *macro;

            NuFParGetWord(parser);

            macro = (AICONDITIONMACRO_PC *)NuLinkedListGetHead(&aiscript_load_script->condition_macros);
            while (macro != NULL) {
                if (NuStrICmp(macro->name, parser->word_buffer_ptr) == 0) {
                    AICONDITION_PC *macro_last;

                    AIScriptCopyConditions(&macro->conditions, aiscript_load_conditions, aiscript_load_buffer,
                                           aiscript_load_end_buffer);
                    macro_last = (AICONDITION_PC *)NuLinkedListGetTail(aiscript_load_conditions);

                    ConditionsParseFinalise(macro_last, parser);

                    break;
                }

                macro =
                    (AICONDITIONMACRO_PC *)NuLinkedListGetNext(&aiscript_load_script->condition_macros, &macro->link);
            }
            if (macro == NULL) {
                if (AIActionDiagnosticMode == 1) {
                    AIScriptMissingStateDiagnosticAlternate(parser->word_buffer_ptr, aiscript_load_script->name,
                                                            aiscript_load_state != NULL ? aiscript_load_state->name
                                                                                        : (char *)Text_UnknownText);
                } else {
                    AIScriptMissingStateDiagnostic(parser->word_buffer_ptr, aiscript_load_script->name,
                                                   aiscript_load_state != NULL ? aiscript_load_state->name
                                                                               : (char *)Text_UnknownText);
                }
            }
        } else {
            if (NuStrChr(parser->word_buffer_ptr, '{') == NULL) {
                if (AIActionDiagnosticMode == 1) {
                    AIScriptMissingConditionDiagnostic(aiscript_load_script->name, aiscript_load_state != NULL
                                                                                       ? aiscript_load_state->name
                                                                                       : (char *)Text_UnknownText);
                } else {
                    AIScriptMissingStateDiagnostic(aiscript_load_script->name, aiscript_load_state != NULL
                                                                                   ? aiscript_load_state->name
                                                                                   : (char *)Text_UnknownText);
                }
            }
        }
    }
}

void __cdecl xActions(NUFPAR_PC *parser) {
    while (NuFParGetLine(parser) != 0) {
        AIACTION_PC *action;

        NuFParGetWord(parser);

        if (NuStrICmp(parser->word_buffer_ptr, "}") == 0) {
            return;
        }

        if (NuStrICmp(parser->word_buffer_ptr, "DoMacro") == 0) {
            AIACTIONMACRO_PC *macro;

            NuFParGetWord(parser);

            macro = (AIACTIONMACRO_PC *)NuLinkedListGetHead(&aiscript_load_script->action_macros);
            while (macro != NULL) {
                if (NuStrICmp(macro->name, parser->word_buffer_ptr) == 0) {
                    AIScriptCopyActions(&macro->actions, aiscript_load_actions, aiscript_load_buffer,
                                        aiscript_load_end_buffer);

                    break;
                }

                macro = (AIACTIONMACRO_PC *)NuLinkedListGetNext(&aiscript_load_script->action_macros, &macro->link);
            }
            if (macro == NULL) {
                if (AIActionDiagnosticMode == 1) {
                    AIScriptMissingStateDiagnosticAlternate(parser->word_buffer_ptr, aiscript_load_script->name,
                                                            aiscript_load_state != NULL ? aiscript_load_state->name
                                                                                        : (char *)Text_UnknownText);
                } else {
                    AIScriptMissingStateDiagnostic(parser->word_buffer_ptr, aiscript_load_script->name,
                                                   aiscript_load_state != NULL ? aiscript_load_state->name
                                                                               : (char *)Text_UnknownText);
                }
            }
        } else {
            AIACTIONDEF_PC *definition;
            char params[24][128];

            definition = AIActionFind(parser->word_buffer_ptr);
            if (definition != NULL) {
                i32 param_count;
                i32 i;

                action = (AIACTION_PC *)AIScriptBufferAlloc(aiscript_load_buffer, aiscript_load_end_buffer,
                                                            sizeof(AIACTION_PC));
                if (action == NULL) {
                    AIScriptMissingStateDiagnostic();
                    continue;
                }

                memset(action, 0, sizeof(AIACTION_PC));

                NuLinkedListAppend(aiscript_load_actions, &action->link);

                action->definition = definition;

                param_count = 0;

                while (NuFParGetWord(parser) != 0) {
                    if (NuStrCmp(parser->word_buffer_ptr, "\\") == 0) {
                        NuFParGetLine(parser);
                    } else {
                        NuStrCpy(params[param_count++], parser->word_buffer_ptr);
                    }
                }

                if (param_count != 0) {
                    action->params = (char **)AIScriptBufferAlloc(aiscript_load_buffer, aiscript_load_end_buffer,
                                                                  param_count * sizeof(char *));
                    if (action->params != NULL) {
                        action->param_count = param_count;

                        for (i = 0; i < param_count; i++) {
                            action->params[i] =
                                AIScriptCopyStringPc(params[i], aiscript_load_buffer, aiscript_load_end_buffer);
                        }
                    }
                }
            } else {
                if (NuStrChr(parser->word_buffer_ptr, '{') == NULL) {
                    if (AIActionDiagnosticMode == 1) {
                        AIScriptMissingStateDiagnosticAlternate(parser->word_buffer_ptr, aiscript_load_script->name,
                                                                aiscript_load_state != NULL ? aiscript_load_state->name
                                                                                            : (char *)Text_UnknownText);
                    } else {
                        AIScriptMissingStateDiagnostic(parser->word_buffer_ptr, aiscript_load_script->name,
                                                       aiscript_load_state != NULL ? aiscript_load_state->name
                                                                                   : (char *)Text_UnknownText);
                    }
                }
            }
        }
    }
}

void __cdecl AIScriptOpenPakFileParse(AISCRIPT_PC **script_ref, void *pak, char *filename, char *path, VARIPTR *buffer,
                                      VARIPTR *end) {
    NUFPAR_PC *parser = AIScriptOpenParserPc(pak, filename, path);
    char script_name[64];
    char *extension;
    AISCRIPT_PC *script;
    if (parser != NULL) {
        if (*script_ref == NULL) {
            script = NULL;
            if (buffer != NULL && end != NULL && buffer->addr + sizeof(AISCRIPT_PC) < end->addr) {
                script = (AISCRIPT_PC *)((buffer->addr + 15) & ~15u);
                buffer->addr = (u32)script + sizeof(AISCRIPT_PC);
                memset(script, 0, sizeof(AISCRIPT_PC));
            } else {
                AIScriptMissingStateDiagnostic();
            }
            *script_ref = script;
            if (script != NULL) {
                memset(script, 0, sizeof(AISCRIPT_PC));
                NuStrCpy(script_name, filename);
                extension = NuStrIStr(script_name, (char *)AIScriptFileExtension);
                if (extension != NULL) {
                    *extension = '\0';
                }
                (*script_ref)->name = AIScriptCopyStringPc(script_name, buffer, end);
            }
        }
        AIScriptParseCommandsPc(parser, buffer, end, *script_ref);
        NuFParDestroy(parser);
    } else if (AIActionDiagnosticMode == 1) {
        AIScriptMissingStateDiagnosticAlternate(filename);
    } else {
        AIScriptMissingStateDiagnostic(filename);
    }
}

void __cdecl xConditionMacro(NUFPAR_PC *parser) {
    AICONDITIONMACRO_PC *macro;

    if (NuFParGetWord(parser) == 0) {
        return;
    }
    macro = (AICONDITIONMACRO_PC *)AIScriptBufferAllocOrWarn(aiscript_load_buffer, aiscript_load_end_buffer,
                                                             sizeof(AICONDITIONMACRO_PC));
    if (macro != NULL) {
        memset(macro, 0, sizeof(AICONDITIONMACRO_PC));
        macro->name = AIScriptCopyStringPc(parser->word_buffer_ptr, aiscript_load_buffer, aiscript_load_end_buffer);
        NuLinkedListAppend(&aiscript_load_script->condition_macros, &macro->link);
        aiscript_load_conditions = &macro->conditions;
        condition_has_no_goto = 1;
        xConditions(parser);
        aiscript_load_conditions = NULL;
        condition_has_no_goto = 0;
    }
}

void __cdecl xActionMacro(NUFPAR_PC *parser) {
    AIACTIONMACRO_PC *macro;

    if (NuFParGetWord(parser) == 0) {
        return;
    }
    macro = (AIACTIONMACRO_PC *)AIScriptBufferAllocOrWarn(aiscript_load_buffer, aiscript_load_end_buffer,
                                                          sizeof(AIACTIONMACRO_PC));
    if (macro != NULL) {
        memset(macro, 0, sizeof(AIACTIONMACRO_PC));
        macro->name = AIScriptCopyStringPc(parser->word_buffer_ptr, aiscript_load_buffer, aiscript_load_end_buffer);
        NuLinkedListAppend(&aiscript_load_script->action_macros, &macro->link);
        aiscript_load_actions = &macro->actions;
        xActions(parser);
        aiscript_load_actions = NULL;
    }
}

void __cdecl xInclude(NUFPAR_PC *parser) {
    VARIPTR *saved_buffer = aiscript_load_buffer;
    VARIPTR *saved_end = aiscript_load_end_buffer;
    AISCRIPT_PC *saved_script = aiscript_load_script;
    NuFParGetWord(parser);
    NuFParSuspend(parser);
    AIScriptOpenPakFileParse(&aiscript_load_script, AIScriptLoadPakPc, parser->word_buffer_ptr, AIScriptLoadPathPc,
                             aiscript_load_buffer, aiscript_load_end_buffer);
    NuFParResume(parser);
    aiscript_load_buffer = saved_buffer;
    aiscript_load_script = saved_script;
    aiscript_load_end_buffer = saved_end;
}

void __cdecl xRefScript(NUFPAR_PC *parser) {
    AIREFSCRIPT_PC *reference;
    VARIPTR saved_buffer;
    char *copy;
    i32 done;

    if (aiscript_load_refs == NULL) {
        return;
    }
    saved_buffer = *aiscript_load_buffer;
    reference = (AIREFSCRIPT_PC *)AIScriptBufferAllocOrWarn(aiscript_load_buffer, aiscript_load_end_buffer,
                                                            sizeof(AIREFSCRIPT_PC));
    if (reference == NULL) {
        return;
    }
    memset(reference, 0, sizeof(AIREFSCRIPT_PC));
    done = 0;
    while (!done && NuFParGetLine(parser) != 0) {
        while (!done && NuFParGetWord(parser) != 0) {
            char *cursor;
            if (NuStrICmp(parser->word_buffer_ptr, "}") == 0) {
                done = 1;
            } else if ((cursor = NuStrIStr(parser->word_buffer_ptr, "Script")) != NULL) {
                AISCRIPT_COPY_NATIVE_STRING(cursor + sizeof("Script"), copy, aiscript_load_buffer,
                                            aiscript_load_end_buffer);
                reference->name = copy;
            } else if ((cursor = NuStrIStr(parser->word_buffer_ptr, "ReturnState")) != NULL) {
                AISCRIPT_COPY_NATIVE_STRING(cursor + sizeof("ReturnState"), copy, aiscript_load_buffer,
                                            aiscript_load_end_buffer);
                reference->return_state_name = copy;
            } else if (NuStrICmp(parser->word_buffer_ptr, "CONDITIONS") == 0) {
                aiscript_load_conditions = &reference->conditions;
                condition_has_no_goto = 1;
                xConditions(parser);
                aiscript_load_conditions = NULL;
                condition_has_no_goto = 0;
            } else if (NuStrIStr(parser->word_buffer_ptr, "Source") != NULL) {
                if (NuStrIStr(parser->word_buffer_ptr, "Global") != NULL) {
                    reference->check_global_scripts = 1;
                } else if (NuStrIStr(parser->word_buffer_ptr, (char *)AIScriptKeywordLevel) != NULL) {
                    reference->check_level_scripts = 1;
                }
            }
        }
    }
    if (reference->name != NULL) {
        if (!reference->check_global_scripts && !reference->check_level_scripts) {
            reference->check_global_scripts = 1;
            reference->check_level_scripts = 1;
        }
        NuLinkedListAppend(aiscript_load_refs, &reference->link);
    } else {
        AIScriptMissingStateDiagnosticAlternate();
        *aiscript_load_buffer = saved_buffer;
    }
}

static AISCRIPT_PC *AIScriptLoadPakFilePc(void *pak, char *filename, char *path, VARIPTR *buffer, VARIPTR *end) {
    AISCRIPT_PC *script = NULL;
    AISTATE_PC *state;
    AISTATE_PC *later;
    AICONDITION_PC *condition;
    AIScriptLoadPathPc = path;
    AIScriptLoadPakPc = pak;
    AIScriptOpenPakFileParse(&script, pak, filename, path, buffer, end);
    AIScriptLoadPathPc = NULL;
    AIScriptLoadPakPc = NULL;
    if (script != NULL) {
        state = (AISTATE_PC *)NuLinkedListGetHead(&script->states);
        while (state != NULL) {
            condition = (AICONDITION_PC *)NuLinkedListGetHead(&state->conditions);
            while (condition != NULL) {
                condition->next_state = AIStateFind(condition->next_state_name, script);
                if (condition->next_state_name != NULL && condition->next_state == NULL) {
                    if (AIActionDiagnosticMode == 1) {
                        AIScriptMissingStateDiagnosticAlternate(condition->next_state_name, script->name, state->name);
                    } else {
                        AIScriptMissingStateDiagnostic(condition->next_state_name, script->name, state->name);
                    }
                }
                condition = (AICONDITION_PC *)NuLinkedListGetNext(&state->conditions, &condition->link);
            }
            later = (AISTATE_PC *)NuLinkedListGetNext(&script->states, &state->link);
            while (later != NULL) {
                if (NuStrICmp(state->name, later->name) == 0) {
                    if (AIActionDiagnosticMode == 1) {
                        AIScriptMissingStateDiagnosticAlternate(state->name, script->name);
                    } else {
                        AIScriptMissingStateDiagnostic(state->name, script->name);
                    }
                }
                later = (AISTATE_PC *)NuLinkedListGetNext(&script->states, &later->link);
            }
            state = (AISTATE_PC *)NuLinkedListGetNext(&script->states, &state->link);
        }
    }
    return script;
}

void __cdecl AIScriptLoadAllPakFile(void *pak, char *path, VARIPTR *buffer, VARIPTR *end_buffer, AISYS_PC *sys) {
    NULISTHDR *scripts;
    NUFPAR_PC *parser;
    NUFILE file;
    void *memfile;
    i32 memfile_size;
    i32 item_index;
    i32 file_count;
    i32 i;
    i32 done;
    char filepath[0x80];
    char script_files[0x80][0x18];

    scripts = (sys != NULL) ? &sys->scripts : &global_aiscripts;
    file = 0;
    if (pak != NULL) {
        item_index = NuFilePakGetItem(pak, (char *)AIScriptListFilename);
        if (item_index != 0) {
            NuFilePakGetItemInfo(pak, item_index, &memfile, &memfile_size);
            file = NuMemFileOpen(memfile, memfile_size, 0);
            if (file == 0) {
                AIScriptMissingConditionDiagnostic();
            }
        } else if (AIActionDiagnosticMode == 1) {
            AIScriptMissingConditionDiagnostic();
        } else {
            AIScriptMissingStateDiagnostic();
        }
        if (file == 0 && ai_onlyusepackfile == 1) {
            if (AIActionDiagnosticMode == 1) {
                AIScriptMissingStateDiagnosticAlternate();
            } else {
                AIScriptMissingStateDiagnostic();
            }
        }
    }

    if (file == 0) {
        sprintf(filepath, AIScriptListPathFormat, path);
        file = NuFileOpen(filepath, 0);
    }

    file_count = 0;
    if (file != 0) {
        parser = NuFParOpen(file);
        if (parser != NULL) {
            while (NuFParGetLine(parser) != 0) {
                NuFParGetWord(parser);
                if (NuStrLen(parser->word_buffer_ptr) > 0x0f) {
                    AIScriptMissingStateDiagnostic(parser->word_buffer_ptr);
                }
                NuStrCpy(script_files[file_count], parser->word_buffer_ptr);
                NuStrCat(script_files[file_count], AIScriptFileExtension);
                file_count++;
            }
            NuFreeFPar(parser);
        }
        NuFileClose(file);

        for (i = 0; i < file_count; i++) {
            AISCRIPT_PC *script = AIScriptLoadPakFilePc(pak, script_files[i], path, buffer, end_buffer);
            if (script != NULL) {
                NuLinkedListAppend(scripts, &script->link);
                if (sys != NULL) {
                    script->flags_3c |= 1u;
                }
            }
        }
    }

    if (sys == NULL) {
        return;
    }

    do {
        AISCRIPT_PC *script;
        done = 1;
        script = (AISCRIPT_PC *)NuLinkedListGetHead(scripts);
        while (script != NULL) {
            if (AIScriptBuildDerivedScript(script, buffer, end_buffer, sys) == 0) {
                done = 0;
            }
            script = (AISCRIPT_PC *)NuLinkedListGetNext(scripts, &script->link);
        }
    } while (done == 0);
}

void __cdecl AIScriptLoadAll(char *path, VARIPTR *buffer, VARIPTR *end, AISYS_PC *sys) {
    VARIPTR pak_start = *end;
    void *pak = NULL;
    i32 pak_size;
    char filepath[128];
    if (ai_usepackfile != 0) {
        sprintf(filepath, AIScriptPakPathFormatPc, path);
        pak_size = NuFileSize(filepath);
        if (pak_size > 0) {
            pak_start.addr = end->addr - ((pak_size + 16) & ~15u);
            pak = NuFilePakLoad(filepath, &pak_start, *end, 16);
        }
    }
    AIScriptLoadAllPakFile(pak, path, buffer, &pak_start, sys);
}

i32 __cdecl AIScriptSetBaseScriptStateByName(AISCRIPTPROCESS_PC *processor, char *name) {
    AISTATE_PC *state;
    if (name == NULL || processor == NULL || processor->script == NULL)
        return 0;
    state = AIStateFind(name, processor->base_script);
    if (state == NULL)
        return 0;
    if (processor->script == processor->base_script) {
        AIScriptSetState(processor, state);
        return 1;
    }
    AIScriptProcessorInit(NULL, NULL, processor, NULL, NULL, NULL, 0, processor->base_script, state);
    processor->active_ref_count = 0;
    return 1;
}

static __declspec(noinline) void AIScriptFillRemainingComplexOperands(NULISTHDR *conditions, AIPACKET_PC *packet,
                                                                      AISCRIPTPROCESS_PC *processor,
                                                                      AICONDITION_PC *condition, i32 stack_index) {
    AICONDITION_PC *remaining;
    if (processor->param_stack[stack_index].force_complex_eval != 0) {
        if (processor->param_stack[stack_index].is_first_time_state != 0) {
            remaining = (AICONDITION_PC *)NuLinkedListGetNext(conditions, (NULISTLNK *)condition);
            while (remaining != NULL) {
                if ((remaining->flags & 8) != 0)
                    processor->param_stack[stack_index].complex_params[remaining->param_index] =
                        AIParamToFloatEx(packet, processor, remaining->complex_expression);
                remaining = (AICONDITION_PC *)NuLinkedListGetNext(conditions, &remaining->link);
            }
        }
        processor->param_stack[stack_index].force_complex_eval = 0;
    }
}

static __declspec(noinline) AICONDITION_PC *AIScriptProcessStateConditions(AISYS_PC *system, NULISTHDR *conditions,
                                                                           AIPACKET_PC *packet,
                                                                           AISCRIPTPROCESS_PC *processor,
                                                                           i32 stack_index) {
    AICONDITION_PC *condition;
    f32 condition_value = 0.0f;
    f32 comparison_value;
    AICONDITION_EVAL_PC comparison_fn;
    i32 comparison_passed;
    if (processor == NULL || conditions == NULL)
        return NULL;
    condition = (AICONDITION_PC *)NuLinkedListGetHead(conditions);
    while (condition != NULL) {
        comparison_passed = 0;
        if (condition->definition != NULL) {
            if (condition->definition->eval_fn != NULL) {
                LOG_INFO_IF((u32)condition->definition->eval_fn == 0x00400000,
                            "Story condition eval: name=%s def=%p fn=%p sys=%p process=%p condition=%p",
                            condition->definition->name, (void *)condition->definition,
                            (void *)condition->definition->eval_fn, (void *)system, (void *)processor,
                            (void *)condition);
                condition_value = condition->definition->eval_fn(system, processor, packet, condition->argument,
                                                                 condition->initialized_argument);
            }
            if (condition->parameter_condition != NULL && condition->parameter_condition->definition != NULL) {
                comparison_fn = condition->parameter_condition->definition->eval_fn;
                comparison_value = comparison_fn != NULL ? comparison_fn(system, processor, packet, condition->argument,
                                                                         condition->initialized_argument)
                                                         : condition_value;
            } else if ((condition->flags & 8) != 0) {
                if (processor->param_stack[stack_index].is_first_time_state != 0)
                    processor->param_stack[stack_index].complex_params[condition->param_index] =
                        AIParamToFloatEx(packet, processor, condition->complex_expression);
                comparison_value = processor->param_stack[stack_index].complex_params[condition->param_index];
            } else if ((condition->flags & 4) != 0)
                comparison_value = processor->params[condition->param_index];
            else
                comparison_value = condition->param_value;
            switch (condition->comparison) {
                case 0:
                    comparison_passed = condition_value == comparison_value;
                    break;
                case 1:
                    comparison_passed = condition_value < comparison_value;
                    break;
                case 2:
                    comparison_passed = condition_value > comparison_value;
                    break;
                case 3:
                    comparison_passed = condition_value <= comparison_value;
                    break;
                case 4:
                    comparison_passed = condition_value >= comparison_value;
                    break;
                case 5:
                    comparison_passed = condition_value != comparison_value;
                    break;
            }
        }
        if (comparison_passed) {
            if ((condition->flags & 1) == 0) {
                AIScriptFillRemainingComplexOperands(conditions, packet, processor, condition, stack_index);
                processor->param_stack[stack_index].is_first_time_state = 0;
                return condition;
            }
        } else {
            while ((condition->flags & 1) != 0) {
                condition = (AICONDITION_PC *)NuLinkedListGetNext(conditions, &condition->link);
                if (condition == NULL)
                    break;
                if (processor->param_stack[stack_index].is_first_time_state != 0 && (condition->flags & 8) != 0) {
                    processor->param_stack[stack_index].complex_params[condition->param_index] =
                        AIParamToFloatEx(packet, processor, condition->complex_expression);
                    condition->param_value = processor->param_stack[stack_index].complex_params[condition->param_index];
                }
            }
        }
        if (condition != NULL)
            condition = (AICONDITION_PC *)NuLinkedListGetNext(conditions, &condition->link);
    }
    AIScriptFillRemainingComplexOperands(conditions, packet, processor, NULL, stack_index);
    processor->param_stack[stack_index].is_first_time_state = 0;
    return NULL;
}

static __declspec(noinline) i32 AIScriptProcessReferences(AISYS_PC *system, AIPACKET_PC *packet,
                                                          AISCRIPTPROCESS_PC *processor) {
    NULISTHDR *references;
    AIREFSCRIPT_PC *reference;
    AICONDITION_PC *entry_condition;
    if (system == NULL || processor == NULL || processor->script == NULL || processor->active_ref_count >= 4)
        return 0;
    for (reference = (AIREFSCRIPT_PC *)NuLinkedListGetHead(&processor->script->reference_scripts); reference != NULL;
         reference = (AIREFSCRIPT_PC *)NuLinkedListGetNext(&processor->script->reference_scripts, &reference->link)) {
        processor->param_stack[1].is_first_time_state = 1;
        processor->param_stack[1].force_complex_eval = 1;
        if ((NuLinkedListGetHead(&reference->conditions) == NULL ||
             AIScriptProcessStateConditions(system, &reference->conditions, packet, processor, 1) != NULL) &&
            reference->script != NULL) {
            processor->param_stack[0].is_first_time_state = 1;
            processor->param_stack[0].force_complex_eval = 1;
            entry_condition = AIScriptProcessStateConditions(system, &reference->script->base_state->conditions, packet,
                                                             processor, 0);
            if (entry_condition != NULL && entry_condition->next_state != reference->script->base_state)
                goto found;
        }
    }
    if (processor->state == NULL)
        return 0;
    references = &processor->state->reference_scripts;
    for (reference = (AIREFSCRIPT_PC *)NuLinkedListGetHead(references); reference != NULL;
         reference = (AIREFSCRIPT_PC *)NuLinkedListGetNext(references, &reference->link)) {
        processor->param_stack[1].is_first_time_state = 1;
        processor->param_stack[1].force_complex_eval = 1;
        if ((NuLinkedListGetHead(&reference->conditions) == NULL ||
             AIScriptProcessStateConditions(system, &reference->conditions, packet, processor, 1) != NULL) &&
            reference->script != NULL) {
            processor->param_stack[0].is_first_time_state = 1;
            processor->param_stack[0].force_complex_eval = 1;
            entry_condition = AIScriptProcessStateConditions(system, &reference->script->base_state->conditions, packet,
                                                             processor, 0);
            if (entry_condition != NULL && entry_condition->next_state != reference->script->base_state)
                goto found;
        }
    }
    return 0;
found:
    processor->active_refs[processor->active_ref_count] = reference;
    processor->active_ref_count++;
    AIScriptProcessorInit(system, packet, processor, NULL, NULL, NULL, 0, reference->script,
                          entry_condition->next_state);
    return 1;
}

void __cdecl AIScriptProcess(AISYS_PC *system, APIOBJECT_PC *object, AIPACKET_PC *packet, AISCRIPTPROCESS_PC *processor,
                             f32 elapsed) {
    i32 index;
    AIREFSCRIPT_PC *reference;
    AICREATURE_PC *creature;
    AIACTION_PC *action;
    i32 first_time;
    f32 old_interrupt_timer;
    AICONDITION_PC *transition;
    if (processor == NULL || processor->script == NULL)
        return;
    processor->script_timer += elapsed;
    for (index = 0; index < processor->active_ref_count; index++) {
        reference = processor->active_refs[index];
        if (NuLinkedListGetHead(&reference->conditions) != NULL &&
            AIScriptProcessStateConditions(system, &reference->conditions, packet, processor, 1) == NULL) {
            processor->active_ref_count = index;
            if (index != 0)
                AIScriptProcessorInit(system, packet, processor, NULL, NULL, NULL, 0,
                                      processor->active_refs[index - 1]->script, reference->return_state);
            else {
                creature = packet->creature_index != 0xff ? &system->creatures[packet->creature_index] : NULL;
                AIScriptProcessorInit(system, packet, processor, creature, NULL, NULL, 0, processor->base_script,
                                      reference->return_state);
            }
            break;
        }
    }
    if (processor->active_ref_count != 0 && processor->next_state == NULL &&
        processor->script->base_state == processor->state) {
        index = processor->active_ref_count - 1;
        reference = processor->active_refs[index];
        processor->active_ref_count = index;
        if (index != 0)
            AIScriptProcessorInit(system, packet, processor, NULL, NULL, NULL, 0,
                                  processor->active_refs[index - 1]->script, reference->return_state);
        else {
            creature = packet->creature_index != 0xff ? &system->creatures[packet->creature_index] : NULL;
            AIScriptProcessorInit(system, packet, processor, creature, NULL, NULL, 0, processor->base_script,
                                  reference->return_state);
        }
    }
    if (processor->next_state != NULL) {
        AIScriptSetState(processor, processor->next_state);
        processor->next_state = NULL;
    }
    if (processor->state == NULL)
        AIScriptSetState(processor, (AISTATE_PC *)NuLinkedListGetHead(&processor->script->states));
    if (processor->state != NULL) {
        while (processor->action_node != NULL) {
            action = processor->action_node;
            if (action->definition != NULL && action->definition->eval_fn != NULL) {
                first_time = 0;
                if ((processor->flags & 1) != 0) {
                    processor->action_data_1 = 0;
                    processor->action_data_2 = 0;
                    processor->action_data_3 = NULL;
                    processor->action_data_6 = 0;
                    processor->action_data_4 = 0.0f;
                    processor->action_data_5 = 0.0f;
                    processor->action_data_7 = 0;
                    processor->action_timer = 0.0f;
                    processor->action_pos.x = processor->action_pos.y = processor->action_pos.z = 0.0f;
                    memset(&processor->path_info, 0, sizeof(processor->path_info));
                    if (packet != NULL) {
                        packet->goal_path_node = NULL;
                        packet->goal_speed_mode = 0;
                        packet->movement_instruction_parameter = 0.0f;
                        packet->movement_look_target = NULL;
                        if (ScriptProcessFirstTimeActionFn != NULL)
                            ScriptProcessFirstTimeActionFn(system, packet, processor);
                    }
                    processor->flags &= ~1u;
                    first_time = 1;
                    if (action->definition->replicate_on_first_action != 0)
                        AINetReplicateAction(action, packet);
                }
                if (action->definition->eval_fn(system, processor, packet, action->params, action->param_count,
                                                first_time, elapsed) == 0)
                    break;
            } else
                processor->flags &= ~1u;
            if ((processor->flags & 1) != 0)
                continue;
            if (processor->action_node != NULL)
                processor->action_node =
                    (AIACTION_PC *)NuLinkedListGetNext(&processor->state->actions, &processor->action_node->link);
            else
                processor->action_node = (AIACTION_PC *)NuLinkedListGetHead(&processor->state->actions);
            if (processor->action_node != NULL)
                processor->flags |= 1;
        }
    }
    if (AIScriptProcessReferences(system, packet, processor) != 0)
        return;
    old_interrupt_timer = processor->interrupt_timer;
    if (processor->interrupt_timer > 0.0f)
        processor->interrupt_timer -= elapsed;
    if (old_interrupt_timer > 0.0f && processor->interrupt_timer <= 0.0f && processor->interrupt_state != NULL) {
        AIScriptSetState(processor, processor->interrupt_state);
        processor->interrupt_priority = 0;
        processor->interrupt_state = NULL;
        return;
    }
    if (processor->state != NULL) {
        transition = AIScriptProcessStateConditions(system, &processor->state->conditions, packet, processor, 0);
        if (transition != NULL) {
            processor->flags = (processor->flags & ~4u) | ((transition->flags & 2u) << 1);
            AIScriptSetState(processor, transition->next_state);
        }
    }
}

void *__cdecl Condition_LocatorRangeInit(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return AIPathFindLocator(system, name);
    return NULL;
}

void *__cdecl Condition_CurrentLocatorIsInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return name != NULL ? AIPathFindLocator(system, name) : NULL;
}

void *__cdecl Condition_InTriggerAreaInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return AISysFindArea(system, name);
    return NULL;
}

void *__cdecl Condition_InLevelNodeInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (system != NULL && system->path_sys != NULL && system->path_sys->path_count != 0)
        return AIPathFindNode(system, system->path_sys->active_path, name);
    return NULL;
}

void *__cdecl Condition_PlayerInLevelNodeInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (system != NULL && system->path_sys != NULL && system->path_sys->path_count != 0)
        return AIPathFindNode(system, system->path_sys->active_path, name);
    return NULL;
}

void *__cdecl Condition_NodeRangeInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (system != NULL && system->path_sys != NULL && system->path_sys->path_count != 0)
        return AIPathFindNode(system, system->path_sys->active_path, name);
    return NULL;
}

void *__cdecl Condition_PlayerInTriggerAreaInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return name != NULL ? AISysFindArea(system, name) : NULL;
}

void *__cdecl Condition_BaddyInTriggerAreaInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return name != NULL ? AISysFindArea(system, name) : NULL;
}

void *__cdecl Condition_GoodyInTriggerAreaInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return AISysFindArea(system, name);
    return NULL;
}

void *__cdecl Condition_OpponentInTriggerAreaInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    return name != NULL ? AISysFindArea(system, name) : NULL;
}

void *__cdecl Condition_OpponentToLocatorInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return AIPathFindLocator(system, name);
    return NULL;
}

void *__cdecl Condition_PlayerToLocatorInitPc(AISYS_PC *system, char *name, AISCRIPT_PC *script) {
    if (name != NULL)
        return AIPathFindLocator(system, name);
    return NULL;
}

i32 __cdecl Action_FaceLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    char *value;
    AILOCATOR_PC *locator;

    (void)elapsed;
    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL ||
        packet->path_info.path == NULL || packet->path_info.connection == NULL) {
        return 1;
    }
    if (first_time != 0) {
        processor->action_data_3 = processor->locator;
        for (index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                continue;
            }
            value = NuStrIStr(params[index], "name");
            if (value != NULL) {
                processor->action_data_3 = AIPathFindLocator(sys, value + NuStrLen("name") + 1);
                ++index;
            }
        }
    }
    locator = (AILOCATOR_PC *)(processor->action_data_3);
    if (locator != NULL) {
        packet->movement_look_target = &locator->position;
        return 1;
    }
    return 0;
}

i32 __cdecl Action_SetLocator(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 count, i32 first_time, f32 elapsed) {
    char *name;
    char *value;
    char buffer[64];
    i32 personal;
    i32 indexed;
    i32 random_count;
    i32 index;
    APIOBJECT_PC *object;
    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL ||
        packet->path_info.path == NULL || packet->path_info.connection == NULL || first_time == 0)
        return 1;
    processor->locator = NULL;
    name = NULL;
    personal = 0;
    indexed = 0;
    random_count = 0;
    for (index = 0; index < count; index++) {
        value = NuStrIStr(params[index], "name");
        if (value != NULL)
            name = value + 5;
        else if (NuStrIStr(params[index], (char *)AIActionPersonalName) != NULL)
            personal = 1;
        else if (NuStrIStr(params[index], (char *)AIActionIndexedName) != NULL)
            indexed = 1;
        else {
            value = NuStrIStr(params[index], "random");
            if (value != NULL)
                random_count = (i32)AIParamToFloatEx(packet, &packet->script_processor, value + 7);
        }
    }
    if (name != NULL) {
        object = (APIOBJECT_PC *)packet->owner;
        if (indexed != 0 && object->locator_name_index != -1)
            sprintf(buffer, AILocatorIndexedFormat, name, object->locator_name_index);
        else if (personal != 0 && object->character_data != NULL)
            sprintf(buffer, AILocatorPersonalFormat, name, object->character_data->name);
        else if (random_count != 0)
            sprintf(buffer, AILocatorIndexedFormat, name, NuRand(NULL) % random_count);
        else
            sprintf(buffer, name);
        processor->locator = AIPathFindLocator(system, buffer);
    }
    return 1;
}

i32 __cdecl Action_SetRespawnLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                     i32 param_count, i32 first_time, f32 elapsed) {
    AILOCATOR_PC *locators[32];
    i32 count;
    i32 i;
    char *value;

    if (packet != NULL && packet->owner != NULL && ((APIOBJECT_PC *)packet->owner)->owner != NULL && first_time != 0) {
        packet->respawn_locator = processor->locator;
        count = 0;
        for (i = 0; i < param_count; i++) {
            value = NuStrIStr(params[i], "locator");
            if (value != NULL) {
                if (count < 32) {
                    locators[count] = AIPathFindLocator(sys, value + 8);
                    if (locators[count] != NULL) {
                        count++;
                    }
                }
            }
        }
        if (count != 0) {
            packet->respawn_locator = locators[NuRand(NULL) % count];
        }
    }
    return 1;
}

__declspec(noinline) void __cdecl AIPathCnxSetTemporaryBlock(AIPATH_PC *path, char *from_name, char *to_name,
                                                             i32 blocked) {
    AIPATHNODE_PC *from;
    AIPATHNODE_PC *to;
    AIPATHCNX_PC *connection;
    i32 to_index;
    i32 index;
    i32 direction;
    if (from_name == NULL || path == NULL || to_name == NULL)
        return;
    from = AIPathFindNode(NULL, path, from_name);
    to = AIPathFindNode(NULL, path, to_name);
    if (to == NULL || from == NULL)
        return;
    to_index = to - path->nodes;
    for (index = 0; index < from->connection_count; index++) {
        connection = from->connections[index];
        if (connection->node_indices[0] == to_index)
            direction = 1;
        else if (connection->node_indices[1] == to_index)
            direction = 0;
        else
            continue;
        if (blocked != 0)
            connection->traversal_flags[direction] |= 0x80000000u;
        else
            connection->traversal_flags[direction] &= 0x7fffffffu;
        return;
    }
}

i32 __cdecl Action_BlockPath(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                             i32 param_4, i32 param_5, f32 param_6) {
    char *from;
    char *to;
    i32 both_ways;
    i32 blocked;
    i32 index;
    char *value;

    (void)processor;
    (void)packet;
    (void)param_6;
    if (sys != NULL && sys->path_sys != NULL && sys->path_sys->path_count != 0 && param_5 != 0 && param_4 > 0) {
        from = NULL;
        to = NULL;
        both_ways = 0;
        blocked = 1;
        for (index = 0; index < param_4; ++index) {
            value = NuStrIStr(params[index], (char *)AIActionFromName);
            if (value != NULL) {
                from = value + 5;
                continue;
            }
            value = NuStrIStr(params[index], (char *)AIActionToName);
            if (value != NULL) {
                to = value + 3;
            } else if (NuStrICmp(params[index], AIActionBothWaysName) == 0) {
                both_ways = 1;
            } else if (NuStrICmp(params[index], AIActionFalseName) == 0) {
                blocked = 0;
            }
        }
        if (to != NULL && from != NULL) {
            AIPathCnxSetTemporaryBlock(sys->path_sys->active_path, from, to, blocked);
            if (both_ways) {
                AIPathCnxSetTemporaryBlock(sys->path_sys->active_path, to, from, blocked);
            }
        }
    }
    return 1;
}

i32 __cdecl Action_PathConnectionMaxLength(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                           char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 direction;
    char *from;
    char *to;
    f32 length;
    i32 i;
    char *value;
    AIPATHCNX_PC *connection;

    direction = 0;
    if (sys != NULL && first_time != 0) {
        from = NULL;
        to = NULL;
        length = 0.0f;
        for (i = 0; i < param_count; i++) {
            value = NuStrIStr(params[i], (char *)AIActionFromName);
            if (value != NULL) {
                from = value + 5;
            } else if ((value = NuStrIStr(params[i], (char *)AIActionToName)) != NULL) {
                to = value + 3;
            } else if ((value = NuStrIStr(params[i], (char *)AIActionLengthName)) != NULL) {
                length = AIParamToFloatEx(packet, processor, value + 7);
            }
        }
        if (to != NULL && from != NULL) {
            connection = (AIPATHCNX_PC *)(AIPAthFindPathCnx(sys, sys->path_sys->active_path, from, to, &direction));
            if (connection != NULL) {
                connection->max_horizontal_distance = length;
                if (length != 0.0f &&
                    connection->horizontal_distance > length + sys->path_sys->active_path->nodes[0].radius +
                                                          sys->path_sys->active_path->nodes[0].radius) {
                    connection->traversal_flags[0] |= 0x08000000;
                    connection->traversal_flags[1] |= 0x08000000;
                } else {
                    connection->traversal_flags[0] &= ~0x08000000;
                    connection->traversal_flags[1] &= ~0x08000000;
                }
            }
        }
    }
    return 1;
}

i32 __cdecl Action_PathConnectionObstacle(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                          char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 direction;
    char *from;
    char *to;
    i32 open;
    i32 i;
    char *value;
    AIPATHCNX_PC *connection;

    direction = 0;
    if (sys != NULL && first_time != 0) {
        from = NULL;
        to = NULL;
        open = 0;
        for (i = 0; i < param_count; i++) {
            value = NuStrIStr(params[i], (char *)AIActionFromName);
            if (value != NULL) {
                from = value + 5;
            } else if ((value = NuStrIStr(params[i], (char *)AIActionToName)) != NULL) {
                to = value + 3;
            } else if (NuStrICmp(params[i], AIActionOpenName) == 0) {
                open = 1;
            }
        }
        if (to != NULL && from != NULL) {
            connection = (AIPATHCNX_PC *)(AIPAthFindPathCnx(sys, NULL, from, to, &direction));
            if (connection != NULL && (connection->traversal_flags[0] & 0x20000000) != 0) {
                connection->open = open;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_SetInterrupt(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_count, i32 is_first_time, f32 elapsed) {
    u8 priority;
    u8 id;
    char *state_name;
    f32 time;
    char *value;
    i32 i;

    if (is_first_time && processor != NULL && param_count > 0) {
        priority = 0;
        id = 0;
        state_name = NULL;
        time = 0.0f;

        for (i = 0; i < param_count; i++) {
            if ((value = NuStrIStr(params[i], "priority")) != NULL) {
                priority = (i32)(AIParamToFloatEx(packet, processor, value + 9));
            } else if ((value = NuStrIStr(params[i], (char *)AIActionIDName)) != NULL) {
                id = (i32)(AIParamToFloatEx(packet, processor, value + 3));
            } else if ((value = NuStrIStr(params[i], (char *)AIActionStateName)) != NULL) {
                state_name = value + 6;
            } else if ((value = NuStrIStr(params[i], (char *)AIActionTimeName)) != NULL) {
                time = AIParamToFloatEx(packet, processor, value + 5);
            }
        }
        if (state_name != NULL) {
            AIScriptSetInterrupt(processor, priority, id, state_name, time);
        }
    }
    return 1;
}

i32 __cdecl Action_ClearInterrupt(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_count, i32 is_first_time, f32 elapsed) {
    char *state_name;
    i32 i;
    char *value;

    if (is_first_time && processor != NULL && param_count > 0) {
        state_name = NULL;
        for (i = 0; i < param_count; i++) {
            value = NuStrIStr(params[i], (char *)AIActionStateName);
            if (value != NULL) {
                state_name = value + 6;
            }
        }
        if (state_name != NULL) {
            if (processor->interrupt_state != NULL && NuStrCmp(state_name, processor->interrupt_state->name) == 0) {
                processor->interrupt_priority = 0;
                processor->interrupt_id = 0;
                processor->interrupt_timer = 0.0f;
                processor->interrupt_state = NULL;
            }
        }
    }
    return 1;
}

i32 __cdecl Action_NotifyStateChange(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                     char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 enabled;
    i32 i;

    enabled = 1;
    for (i = 0; i < param_count; i++) {
        if (NuStrICmp(params[i], AIScriptFalse) == 0) {
            enabled = 0;
        }
    }
    if (enabled) {
        AiSysStateDebugee = processor;
    } else {
        AiSysStateDebugee = NULL;
    }
    return 1;
}

static AISYS_PC *AISysLoadPrivatePc(NUGSCN *scene, char *directory, void *pak, VARIPTR *buffer, VARIPTR *buffer_end,
                                    i32 storage_size, char *name, char *parameter, char *load_directory) {
    char ai2_path[128];
    char script_path[128];
    char special_name[256];
    char member_path[32];
    char character_name[32];
    char reference_name[16];
    AI_SYSTEM_LOAD_PC *system;
    i32 is_open;
    i32 version;
    i32 index;
    i32 search;
    sprintf(ai2_path, "%sLevels\\%s\\%s\\%s.ai2", AiLevelPathName, load_directory, parameter, name);
    PATH_LOAD_ALLOCATE(AI_SYSTEM_LOAD_PC *, system, buffer, buffer_end, sizeof(AI_SYSTEM_LOAD_PC));
    if (system == NULL)
        return NULL;
    memset(system, 0, sizeof(*system));
    system->storage_size = storage_size;
    system->storage = AISysBufferAlloc(buffer, buffer_end, storage_size);
    system->storage_end = *buffer;
    memset(system->storage, 0, storage_size);
    system->storage_cursor.void_ptr = system->storage;
    EdFileSetMedia(1);
    is_open = 0;
    if (pak != NULL) {
        sprintf(member_path, "%s.ai2", name);
        EdFileSetPakFile(pak);
        is_open = EdFileOpen(member_path, 0);
    }
    if (is_open == 0) {
        EdFileSetPakFile(NULL);
        is_open = EdFileOpen(ai2_path, 0);
    }
    if (is_open != 0) {
        version = EdFileReadInt();
        system->scene = scene;
        system->path_sys = AISysLoadPathsPc(&system->storage_cursor, version, scene, &system->storage_end);
        if (version >= 4) {
            system->area_count = EdFileReadInt();
            if (system->area_count != 0) {
                system->areas = (AI_AREA_LOAD_PC *)AISysBufferAlloc(&system->storage_cursor, &system->storage_end,
                                                                    system->area_count * sizeof(AI_AREA_LOAD_PC));
                for (index = 0; index < system->area_count; ++index) {
                    AI_AREA_LOAD_PC *area = &system->areas[index];
                    EdFileRead(area->name, 16);
                    area->minimum.x = EdFileReadFloat();
                    area->minimum.y = EdFileReadFloat();
                    area->minimum.z = EdFileReadFloat();
                    area->maximum.x = EdFileReadFloat();
                    area->maximum.y = EdFileReadFloat();
                    area->maximum.z = EdFileReadFloat();
                    area->flags = EdFileReadShort();
                    area->system = (AISYS_PC *)system;
                    if (version > 19)
                        area->game_flags = (u8)EdFileReadChar();
                    else
                        EdFileReadChar();
                    EdFileReadChar();
                }
            }
        }
        if (system->path_sys != NULL) {
            if (version >= 6) {
                system->locator_count = EdFileReadInt();
                if (system->locator_count != 0) {
                    system->locators = (AILOCATOR_PC *)AISysBufferAlloc(&system->storage_cursor, &system->storage_end,
                                                                        system->locator_count * sizeof(AILOCATOR_PC));
                    for (index = 0; index < system->locator_count; ++index) {
                        AILOCATOR_PC *locator = &system->locators[index];
                        EdFileRead(locator->name, 16);
                        locator->position.x = EdFileReadFloat();
                        locator->position.y = EdFileReadFloat();
                        locator->position.z = EdFileReadFloat();
                        locator->rotation = (i16)EdFileReadShort();
                        locator->path_info.path = system->path_sys->paths[(u8)EdFileReadChar()];
                        locator->path_info.direction = (u8)EdFileReadChar();
                        locator->path_info.connection = &locator->path_info.path->connections[(u16)EdFileReadShort()];
                        locator->path_info.distance = EdFileReadFloat();
                        locator->path_info.width = EdFileReadFloat();
                        if (version > 14)
                            locator->rotation_offset = EdFileReadInt();
                    }
                }
                if (version > 17) {
                    system->locator_set_count = EdFileReadInt();
                    if (system->locator_set_count != 0) {
                        system->locator_sets =
                            (AILOCATORSET_PC *)AISysBufferAlloc(&system->storage_cursor, &system->storage_end,
                                                                system->locator_set_count * sizeof(AILOCATORSET_PC));
                        for (index = 0; index < system->locator_set_count; ++index) {
                            AILOCATORSET_PC *set = &system->locator_sets[index];
                            EdFileRead(set->name, 16);
                            set->locator_count = (i8)EdFileReadInt();
                            set->locator_entries = (u8 *)AISysBufferAlloc(&system->storage_cursor, &system->storage_end,
                                                                          set->locator_count * sizeof(AILOCATORSET_PC));
                            for (search = 0; search < set->locator_count; ++search)
                                set->locator_entries[search] = (u8)EdFileReadChar();
                            PATH_LOAD_ALLOCATE(u8 *, set->assigned, &system->storage_cursor, &system->storage_end,
                                               set->locator_count);
                            memset(set->assigned, 0, set->locator_count);
                        }
                    }
                }
            }
            system->creature_count = EdFileReadInt();
            if (system->creature_count != 0) {
                system->creatures =
                    (AI_CREATURE_LOAD_PC *)AISysBufferAlloc(&system->storage_cursor, &system->storage_end,
                                                            system->creature_count * sizeof(AI_CREATURE_LOAD_PC));
                for (index = 0; index < system->creature_count; ++index) {
                    AI_CREATURE_LOAD_PC *creature = &system->creatures[index];
                    EdFileRead(creature->name, 16);
                    EdFileRead(creature->script_name, 16);
                    if (strlen(creature->script_name) == 0)
                        strcpy(creature->script_name, "default");
                    if (version > 13)
                        EdFileRead(character_name, 32);
                    else
                        EdFileRead(character_name, 16);
                    if (GlobalCharacterTypeIDFn != NULL)
                        creature->type = (i16)GlobalCharacterTypeIDFn(character_name);
                    creature->position.x = EdFileReadFloat();
                    creature->position.y = EdFileReadFloat();
                    creature->position.z = EdFileReadFloat();
                    creature->y_rotation = (i16)EdFileReadShort();
                    if (version > 15)
                        creature->set = (u8)EdFileReadChar();
                    creature->count = (u8)EdFileReadChar();
                    creature->count_across = (u8)EdFileReadChar();
                    creature->active_mask = EdFileReadInt();
                    creature->x_spacing = EdFileReadFloat();
                    creature->z_spacing = EdFileReadFloat();
                    creature->flags = EdFileReadInt();
                    creature->path_info.path = system->path_sys->paths[(u8)EdFileReadChar()];
                    creature->path_info.direction = (u8)EdFileReadChar();
                    creature->path_info.connection = &creature->path_info.path->connections[(u16)EdFileReadShort()];
                    if (version > 2) {
                        creature->script_params[0] = EdFileReadFloat();
                        creature->script_params[1] = EdFileReadFloat();
                        creature->script_params[2] = EdFileReadFloat();
                        creature->script_params[3] = EdFileReadFloat();
                    }
                    if (version >= 4 && EdFileReadInt() != 0) {
                        EdFileRead(reference_name, 16);
                        for (search = 0; search < system->area_count; ++search) {
                            AI_AREA_LOAD_PC *entry = &system->areas[search];
                            if (NuStrICmp(reference_name, entry->name) == 0) {
                                creature->area = (AIAREA_PC *)entry;
                                break;
                            }
                        }
                    }
                    if (version >= 6 && EdFileReadInt() != 0) {
                        EdFileRead(reference_name, 16);
                        for (search = 0; search < system->locator_count; ++search) {
                            AILOCATOR_PC *entry = &system->locators[search];
                            if (NuStrICmp(reference_name, entry->name) == 0) {
                                creature->locator = entry;
                                break;
                            }
                        }
                    }
                    if (version > 16 && EdFileReadInt() != 0) {
                        EdFileRead(reference_name, 16);
                        for (search = 0; search < system->locator_count; ++search) {
                            AILOCATOR_PC *entry = &system->locators[search];
                            if (NuStrICmp(reference_name, entry->name) == 0) {
                                creature->respawn_locator = entry;
                                break;
                            }
                        }
                    }
                    if (version > 7) {
                        creature->activation_difficulty = (u8)EdFileReadChar();
                        creature->min_respawn_count = EdFileReadChar();
                        creature->max_respawn_count = EdFileReadChar();
                        creature->activation_type = (u8)EdFileReadChar();
                        creature->min_respawn_time = EdFileReadFloat();
                        creature->max_respawn_time = EdFileReadFloat();
                    } else {
                        creature->activation_difficulty = AiLoadActivationDifficultyPc;
                        creature->min_respawn_count = AiLoadMinimumRespawnCountPc;
                        creature->max_respawn_count = AiLoadMaximumRespawnCountPc;
                        creature->min_respawn_time = AiLoadMinimumRespawnTimePc;
                        creature->max_respawn_time = AiLoadMaximumRespawnTimePc;
                    }
                    if (version > 9)
                        creature->start_stagger = EdFileReadFloat();
                    else
                        creature->start_stagger = AiLoadStartStaggerPc;
                    if (creature->activation_type == 1) {
                        EdFileRead(reference_name, 16);
                        for (search = 0; search < system->area_count; ++search) {
                            AI_AREA_LOAD_PC *entry = &system->areas[search];
                            if (NuStrICmp(reference_name, entry->name) == 0) {
                                creature->activate_area = (AIAREA_PC *)entry;
                                break;
                            }
                        }
                    }
                    if (version > 10) {
                        creature->view_distance = EdFileReadFloat();
                        creature->hear_distance = EdFileReadFloat();
                        creature->max_view_height = EdFileReadFloat();
                        creature->min_view_height = EdFileReadFloat();
                        EdFileReadInt();
                        if (GetViewRangeFn != NULL && creature->view_distance == 0.0f)
                            creature->view_distance = GetViewRangeFn(creature->type);
                        if (GetHearDistanceFn != NULL && creature->hear_distance == 0.0f)
                            creature->hear_distance = GetHearDistanceFn(creature->type);
                        if (GetMaxViewHeightFn != NULL && creature->max_view_height == 0.0f)
                            creature->max_view_height = GetMaxViewHeightFn(creature->type);
                        if (GetMinViewHeightFn != NULL && creature->min_view_height == 0.0f)
                            creature->min_view_height = GetMinViewHeightFn(creature->type);
                    } else {
                        if (GetViewRangeFn != NULL)
                            creature->view_distance = GetViewRangeFn(creature->type);
                        else
                            creature->view_distance = 1.0f;
                        if (GetHearDistanceFn != NULL)
                            creature->hear_distance = GetHearDistanceFn(creature->type);
                        else
                            creature->hear_distance = 1.0f;
                        if (GetMaxViewHeightFn != NULL)
                            creature->max_view_height = GetMaxViewHeightFn(creature->type);
                        else
                            creature->max_view_height = 1.0f;
                        if (GetMinViewHeightFn != NULL)
                            creature->min_view_height = GetMinViewHeightFn(creature->type);
                        else
                            creature->min_view_height = 1.0f;
                    }
                }
            }
            if (version > 12) {
                system->antinode_count = EdFileReadInt();
                if (system->antinode_count != 0) {
                    PATH_LOAD_ALLOCATE(AI_ANTINODE_LOAD_PC *, system->antinodes, &system->storage_cursor,
                                       &system->storage_end, system->antinode_count * sizeof(AI_ANTINODE_LOAD_PC));
                    for (index = 0; index < system->antinode_count; ++index) {
                        AI_ANTINODE_LOAD_PC *antinode = &system->antinodes[index];
                        i32 length;
                        antinode->enabled = 1;
                        antinode->position.x = EdFileReadFloat();
                        antinode->position.y = EdFileReadFloat();
                        antinode->position.z = EdFileReadFloat();
                        antinode->radius = EdFileReadFloat();
                        antinode->base_radius = antinode->radius;
                        antinode->base_height = antinode->radius;
                        antinode->min_y = EdFileReadFloat();
                        antinode->max_y = EdFileReadFloat();
                        antinode->min_y_offset = antinode->min_y - antinode->position.y;
                        antinode->max_y_offset = antinode->max_y - antinode->position.y;
                        if (version > 14) {
                            antinode->flags = EdFileReadInt();
                            antinode->base_radius = EdFileReadFloat();
                            antinode->base_height = EdFileReadFloat();
                            antinode->type = (u8)EdFileReadChar();
                        } else {
                            EdFileReadChar();
                        }
                        EdFileReadChar();
                        EdFileReadChar();
                        antinode->game_flags = (u8)EdFileReadChar();
                        length = (i8)EdFileReadChar();
                        if (length != 0) {
                            EdFileRead(special_name, length);
                            antinode->has_special =
                                (u8)(NuSpecialFind(scene, &antinode->special_handle, special_name, 1) != 0);
                            antinode->special_position.x = EdFileReadFloat();
                            antinode->special_position.y = EdFileReadFloat();
                            antinode->special_position.z = EdFileReadFloat();
                            if (version > 14)
                                antinode->rotation_offset = EdFileReadInt();
                        }
                    }
                }
            }
            if (version > 6 && GameAILoadFn != NULL)
                GameAILoadFn((AISYS_PC *)system, version, scene, buffer, buffer_end);
        }
        EdFileClose();
    }
    sprintf(script_path, "%sLevels\\%s\\%s", AiLevelPathName, directory, parameter);
    AIScriptLoadAllPakFile(pak, script_path, buffer, buffer_end, (AISYS_PC *)system);
    AISysSetLevelPath((AISYS_PC *)system, NULL);
    return (AISYS_PC *)system;
}

AISYS_PC *__cdecl AISysLoadEx(VARIPTR *buffer, VARIPTR *buffer_end, i32 storage_size, NUGSCN *scene, char *directory,
                              char *name, char *parameter, char *load_directory) {
    VARIPTR pak_start = *buffer_end;
    void *pak = NULL;
    char path[128];
    if (ai_usepackfile != 0) {
        i32 pak_size;
        sprintf(path, "%sLevels\\%s\\%s\\ai.pak", AiLevelPathName, directory, parameter);
        pak_size = (i32)NuFileSize(path);
        if (pak_size > 0) {
            pak_start.addr = buffer_end->addr - ((pak_size + 16) & ~15u);
            pak = NuFilePakLoad(path, &pak_start, *buffer_end, 16);
        }
    }
    return AISysLoadPrivatePc(scene, directory, pak, buffer, buffer_end, storage_size, name, parameter, load_directory);
}

AISYS_PC *__cdecl AISysLoad(VARIPTR *buffer, VARIPTR *buffer_end, i32 storage_size, NUGSCN *scene, char *directory,
                            char *name, char *parameter) {
    return AISysLoadEx(buffer, buffer_end, storage_size, scene, directory, name, parameter, directory);
}

void __cdecl AISysDrawAntinode_Circle(AIANTINODE_PC *node, u32 colour) {
    LocaledbitsDrawSolidCircleXY(&node->position, node->radius, node->min_y, node->max_y, colour, 0, 16);
}

void __cdecl AISysDrawAntinode_Ellipse(AIANTINODE_PC *node, u32 colour) {
    LocaledbitsDrawSolidEllipseXY(&node->position, node->base_radius, node->base_height, node->rotation, node->min_y,
                                  node->max_y, colour, 0, 16);
}

static u32 CharacterTestPathCnx(AISYS_PC *system, APIOBJECT_PC *object, AIPACKET_PC *packet, AIPATHCNX_PC *connection,
                                i32 direction, f32 *nearest_distance) {
    AIPATH_PC *path;
    u32 flags;
    AIPATHCNX_PC *current;
    AIPATHNODE_PC *shared;
    AIPATHNODE_PC *first;
    AIPATHNODE_PC *second;
    f32 first_radius;
    i32 narrow;
    f32 second_radius;
    NUVEC delta;
    NUVEC local;
    NUVEC radial;
    f32 height_distance;
    f32 distance;
    f32 clearance;
    i32 node_index;
    i32 angle;
    NUVEC wall;
    f32 length;
    f32 denominator;
    f32 numerator;
    f32 fraction;
    f32 height;
    f32 minimum;
    f32 maximum;

    path = packet->path_info.path;
    if (connection->last_search_checksum == path->search_checksum || path->nodes == NULL) {
        return 0;
    }
    connection->last_search_checksum = path->search_checksum;
    if (object->character_data != NULL) {
        flags = connection->traversal_flags[0] | connection->traversal_flags[1];
        if (flags != 0) {
            if (direction != -1) {
                flags = connection->traversal_flags[direction];
            }
            if (flags != 0 && ((packet->capabilities & flags) == 0 || (flags & 0x98000000) != 0)) {
                return 0;
            }
        }
    }
    if ((object->flags_low & 0x80) == 0 && packet->current_route != 0xff &&
        ((u64)connection->route_mask & ((u64)1 << packet->current_route)) == 0) {
        current = packet->path_info.connection;
        if (current == NULL || current == connection) {
            return 0;
        }
        shared = NULL;
        if (current->node_indices[0] == connection->node_indices[0]) {
            shared = &path->nodes[current->node_indices[0]];
        } else if (current->node_indices[0] == connection->node_indices[1]) {
            shared = &path->nodes[current->node_indices[0]];
        } else if (current->node_indices[1] == connection->node_indices[0]) {
            shared = &path->nodes[current->node_indices[1]];
        } else if (current->node_indices[1] == connection->node_indices[1]) {
            shared = &path->nodes[current->node_indices[1]];
        }
        if (shared == NULL || ((u64)shared->route_boundary_mask & ((u64)1 << packet->current_route)) == 0) {
            return 0;
        }
    }
    if (direction == -1) {
        direction = 0;
    }
    first = &path->nodes[connection->node_indices[0]];
    second = &path->nodes[connection->node_indices[1]];
    narrow = 0;
    if (first->radius > AIPathConnectionMargin + object->collision_radius) {
        first_radius = first->radius - object->collision_radius;
    } else {
        first_radius = first->radius;
        narrow = 1;
    }
    if (second->radius > AIPathConnectionMargin + object->collision_radius) {
        second_radius = second->radius - object->collision_radius;
    } else {
        second_radius = second->radius;
        narrow = 1;
    }
    if (first->has_special != 0 &&
        (path->updated_node_bits[connection->node_indices[0] / 8] & (1 << (connection->node_indices[0] % 8))) == 0) {
        AIPathNodeUpdatePos(system, path, first);
    }
    if (second->has_special != 0 &&
        (path->updated_node_bits[connection->node_indices[1] / 8] & (1 << (connection->node_indices[1] % 8))) == 0) {
        AIPathNodeUpdatePos(system, path, second);
    }

    delta.x = object->position.x - first->position.x;
    delta.z = object->position.z - first->position.z;

    NuVecRotateY(&local, &delta, -connection->rotation);

    if (connection->horizontal_distance < AIPathFabs(second_radius - first_radius)) {
        if (second_radius > first_radius) {
            first_radius = second_radius;
            first = second;
        }
        height_distance = AIPathFabs(first->position.y - packet->terrain_origin.y);
        distance = NuVecXZDist(&object->position, &first->position, &radial);

        if (packet->terrain_origin.y < first->min_height || packet->terrain_origin.y > first->max_height) {
            clearance = distance - first_radius;
            clearance = height_distance > clearance ? height_distance : clearance;
        } else if (distance <= first_radius) {
            packet->path_info.narrow_path = narrow;
            packet->path_info.on_path = 1;
            AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
            packet->path_info.distance = local.z / connection->horizontal_distance;
            packet->path_info.width = local.x;
            packet->last_path_position = object->position;
            node_index = first - path->nodes;
            path->inside_node_bits[node_index / 8] |= 1 << (node_index % 8);
            packet->inside_path_node = node_index;
            if (NuVecXZDistSqr(&object->position, &second->position, &radial) <= second_radius * second_radius) {
                node_index = second - path->nodes;
                path->inside_node_bits[node_index / 8] |= 1 << (node_index % 8);
                packet->inside_path_node = node_index;
            }
            return 1;
        } else {
            clearance = distance - first_radius;
        }
        if (clearance < *nearest_distance) {
            *nearest_distance = clearance;
            AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
            packet->path_info.distance = local.z / connection->horizontal_distance;
            packet->path_info.width = local.x;
            if (distance <= first_radius) {
                packet->last_path_position.x = object->position.x;
                packet->last_path_position.z = object->position.z;
            } else {
                packet->last_path_position.x = radial.x * first_radius / distance;
                packet->last_path_position.x += first->position.x;
                packet->last_path_position.z = radial.z * first_radius / distance;
                packet->last_path_position.z += first->position.z;
            }
            packet->last_path_position.y = first->position.y;
            packet->inside_path_node = -1;
        }
        return 0;
    }

    angle = PCPathArcSine((second_radius - first_radius) / connection->horizontal_distance);
    wall = local;
    if (wall.x < 0.0f) {
        wall.x = -wall.x;
    }
    NuVecRotateY(&wall, &wall, -angle);
    length = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * connection->horizontal_distance;
    if (wall.z < 0.0f) {
        height_distance = AIPathFabs(first->position.y - packet->terrain_origin.y);
        distance = NuVecXZDist(&object->position, &first->position, &radial);

        if (packet->terrain_origin.y < first->min_height || packet->terrain_origin.y > first->max_height) {
            clearance = distance - first_radius;
            clearance = height_distance > clearance ? height_distance : clearance;
        } else if (distance <= first_radius) {
            packet->path_info.narrow_path = narrow;
            packet->path_info.on_path = 1;
            AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
            packet->path_info.distance = local.z / connection->horizontal_distance;
            packet->path_info.width = local.x;
            packet->last_path_position = object->position;
            path->inside_node_bits[connection->node_indices[0] / 8] |= 1 << (connection->node_indices[0] % 8);
            packet->inside_path_node = connection->node_indices[0];
            return 1;
        } else {
            clearance = distance - first_radius;
        }
        if (clearance < *nearest_distance) {
            *nearest_distance = clearance;
            AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
            packet->path_info.distance = local.z / connection->horizontal_distance;
            packet->path_info.width = local.x;
            if (distance <= first_radius) {
                packet->last_path_position.x = object->position.x;
                packet->last_path_position.z = object->position.z;
            } else {
                packet->last_path_position.x = radial.x * first_radius / distance;
                packet->last_path_position.x += first->position.x;
                packet->last_path_position.z = radial.z * first_radius / distance;
                packet->last_path_position.z += first->position.z;
            }
            packet->last_path_position.y = first->position.y;
            packet->inside_path_node = -1;
        }
    }
    if (wall.z > length) {
        height_distance = AIPathFabs(second->position.y - packet->terrain_origin.y);
        distance = NuVecXZDist(&object->position, &second->position, &radial);

        if (packet->terrain_origin.y < second->min_height || packet->terrain_origin.y > second->max_height) {
            clearance = distance - second_radius;
            clearance = height_distance > clearance ? height_distance : clearance;
        } else if (distance <= second_radius) {
            packet->path_info.narrow_path = narrow;
            packet->path_info.on_path = 1;
            AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
            packet->path_info.distance = local.z / connection->horizontal_distance;
            packet->path_info.width = local.x;
            packet->last_path_position = object->position;
            path->inside_node_bits[connection->node_indices[1] / 8] |= 1 << (connection->node_indices[1] % 8);
            packet->inside_path_node = connection->node_indices[1];
            return 1;
        } else {
            clearance = distance - second_radius;
        }
        if (clearance < *nearest_distance) {
            *nearest_distance = clearance;
            AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
            packet->path_info.distance = local.z / connection->horizontal_distance;
            packet->path_info.width = local.x;
            if (second_radius < distance) {
                packet->last_path_position.x = radial.x * second_radius / distance;
                packet->last_path_position.x += second->position.x;
                packet->last_path_position.z = radial.z * second_radius / distance;
                packet->last_path_position.z += second->position.z;
            } else {
                packet->last_path_position.x = object->position.x;
                packet->last_path_position.z = object->position.z;
            }
            packet->last_path_position.y = second->position.y;
            packet->inside_path_node = -1;
        }
    } else if (!(wall.z < 0.0f)) {

        if (first_radius > local.z) {
            height = first->position.y;
            minimum = first->min_height;
            maximum = first->max_height;
            if (local.z > connection->horizontal_distance - second_radius) {
                minimum = second->min_height < minimum ? second->min_height : minimum;
                maximum = second->max_height > maximum ? second->max_height : maximum;
            }
        } else if (local.z > connection->horizontal_distance - second_radius) {
            height = second->position.y;
            minimum = second->min_height;
            maximum = second->max_height;
        } else {
            denominator = connection->horizontal_distance - (first_radius + second_radius);
            numerator = local.z - first_radius;
            fraction = AIPathSafeDivide(numerator, denominator);
            height = second->position.y * fraction + first->position.y * (1.0f - fraction);
            minimum = second->min_height * fraction + first->min_height * (1.0f - fraction);
            maximum = second->max_height * fraction + first->max_height * (1.0f - fraction);
        }
        height_distance = AIPathFabs(height - packet->terrain_origin.y);

        if (height_distance > 0.0f && (packet->terrain_origin.y < minimum || packet->terrain_origin.y > maximum)) {
            clearance = wall.x - first_radius;
            clearance = height_distance > clearance ? height_distance : clearance;
        } else if (first_radius > wall.x) {
            packet->path_info.narrow_path = narrow;
            packet->path_info.on_path = 1;
            AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
            packet->path_info.distance = local.z / connection->horizontal_distance;
            packet->path_info.width = local.x;
            packet->last_path_position = object->position;
            packet->inside_path_node = -1;
            if (first_radius > local.z) {
                if (NuVecXZDistSqr(&object->position, &first->position, &radial) <= first_radius * first_radius) {
                    path->inside_node_bits[connection->node_indices[0] / 8] |= 1 << (connection->node_indices[0] % 8);
                    packet->inside_path_node = connection->node_indices[0];
                }
            } else if (local.z > length - second_radius) {
                if (NuVecXZDistSqr(&object->position, &second->position, &radial) <= second_radius * second_radius) {
                    path->inside_node_bits[connection->node_indices[1] / 8] |= 1 << (connection->node_indices[1] % 8);
                    packet->inside_path_node = connection->node_indices[1];
                }
            }
            return 1;
        } else {
            clearance = wall.x - first_radius;
        }
        if (clearance < *nearest_distance) {
            *nearest_distance = clearance;
            AISysCharacterSetPathCnx(packet, &object->position, connection, direction);
            packet->path_info.distance = local.z / connection->horizontal_distance;
            packet->path_info.width = local.x;
            if (wall.x < first_radius) {
                packet->last_path_position.x = object->position.x;
                packet->last_path_position.z = object->position.z;
            } else {
                wall.x = first_radius;
                NuVecRotateY(&wall, &wall, angle);
                if (local.x < 0.0f) {
                    wall.x = -wall.x;
                }
                NuVecRotateY(&wall, &wall, connection->rotation);
                packet->last_path_position.x = wall.x + first->position.x;
                packet->last_path_position.z = first->position.z + wall.z;
            }
            packet->last_path_position.y = first->position.y;
            packet->inside_path_node = -1;
        }
    }
    return 0;
}

void __cdecl AISysGetCharacterPathPos(AISYS_PC *system, APIOBJECT_PC *object, AIPACKET_PC *packet, i32 checks,
                                      i32 ground) {
    f32 nearest_distance_squared;
    u16 previous_flags;
    AIPATH_PC *path;
    AIPATHCNX_PC *current_connection;
    i32 current_direction;
    i32 check_adjacent_connections;
    AIPATHNODE_PC *first_node;
    AIPATHCNX_PC *candidate;
    i32 other_direction;
    AIPATHNODE_PC *second_node;
    i32 remaining_checks;
    AIPATHNODE_PC *node;
    AIPATH_PC *original_path;
    AIPATHCNX_PC *original_connection;
    u8 original_direction;
    i32 position_inside_path_bounds;
    i32 connection_index;

    nearest_distance_squared = 3.402823466e+38f;

    previous_flags = packet->path_info.flags_word;
    packet->path_info.flags_word = (previous_flags & ~(1 | 2 | 8)) | ((previous_flags & 1) << 1);

    if (system == NULL || system->path_sys == NULL || system->path_sys->path_count == 0) {
        packet->path_info.on_path = 0;
        AISysCharacterSetPath(packet, NULL);
        packet->path_info.connection = NULL;
        packet->next_route = 0;
        packet->current_route = 0xff;
        return;
    }

    path = packet->path_info.path;
    if (path == NULL) {
        path = (packet->navigation_flags & 2) != 0 ? system->path_sys->paths[0] : system->path_sys->active_path;
        AISysCharacterSetPath(packet, path);
        packet->path_info.connection = NULL;
        packet->current_route = 0xff;
        packet->next_route = 0;
        packet->path_info.next_check = 0;
        packet->path_info.path_index = 0;
    }
    path = packet->path_info.path;
    AISysResetPathSearchConnectionChecks(path);

    current_connection = packet->path_info.connection;
    if (current_connection != NULL) {
        current_direction = packet->path_info.direction;
        if (CharacterTestPathCnx(system, object, packet, current_connection, current_direction,
                                 &nearest_distance_squared) != 0) {
            return;
        }

        check_adjacent_connections = (object->flags_low & 0x80) != 0 ||
                                     (packet->path_target == NULL && packet->path_update_flag == 0 && ground != 0);
        if (!check_adjacent_connections) {
            return;
        }

        path = packet->path_info.path;
        first_node = &path->nodes[current_connection->node_indices[current_direction]];
        for (connection_index = 0; connection_index < first_node->connection_count; ++connection_index) {
            candidate = first_node->connections[connection_index];
            if (candidate != current_connection &&
                CharacterTestPathCnx(system, object, packet, candidate,
                                     candidate->node_indices[0] != current_connection->node_indices[current_direction],
                                     &nearest_distance_squared) != 0)
                return;
        }

        path = packet->path_info.path;
        other_direction = current_direction == 0;
        second_node = &path->nodes[current_connection->node_indices[other_direction]];
        for (connection_index = 0; connection_index < second_node->connection_count; ++connection_index) {
            candidate = second_node->connections[connection_index];
            if (candidate != current_connection &&
                CharacterTestPathCnx(system, object, packet, candidate,
                                     candidate->node_indices[0] != current_connection->node_indices[other_direction],
                                     &nearest_distance_squared) != 0)
                return;
        }
    }

    path = packet->path_info.path;
    if ((packet->navigation_flags & 2) == 0) {
        remaining_checks = path->node_count < checks ? path->node_count : checks;
        while (remaining_checks-- > 0) {
            node = &path->nodes[packet->path_info.next_check];
            for (connection_index = 0; connection_index < node->connection_count; ++connection_index) {
                if (CharacterTestPathCnx(system, object, packet, node->connections[connection_index], -1,
                                         &nearest_distance_squared) != 0) {
                    return;
                }
            }
            path = packet->path_info.path;
            packet->path_info.next_check = (packet->path_info.next_check + 1) % path->node_count;
        }
        return;
    }

    original_path = packet->path_info.path;
    original_connection = packet->path_info.connection;
    original_direction = packet->path_info.direction;

    if (packet->path_info.path_index == 0xff) {
        packet->path_info.path_index = original_path->index;
    }
    if (packet->path_info.path_index >= system->path_sys->path_count) {
        packet->path_info.path_index = 0;
    }

    path = system->path_sys->paths[packet->path_info.path_index];
    packet->path_info.path = path;
    if (packet->path_info.next_check >= path->node_count) {
        packet->path_info.next_check = 0;
    }
    AISysResetPathSearchConnectionChecks(path);

    position_inside_path_bounds = 0;
    for (remaining_checks = checks; remaining_checks > 0; --remaining_checks) {
        if (!position_inside_path_bounds) {
            position_inside_path_bounds = AIPathCheckExtents(packet->path_info.path, &packet->owner->position);
        }

        if (position_inside_path_bounds) {
            path = packet->path_info.path;
            node = &path->nodes[packet->path_info.next_check];
            for (connection_index = 0; connection_index < node->connection_count; ++connection_index) {
                if (CharacterTestPathCnx(system, object, packet, node->connections[connection_index], -1,
                                         &nearest_distance_squared) != 0) {
                    return;
                }
            }

            ++packet->path_info.next_check;
            path = packet->path_info.path;
            if (packet->path_info.next_check < path->node_count) {
                continue;
            }
        }

        packet->path_info.next_check = 0;
        ++packet->path_info.path_index;
        if (packet->path_info.path_index >= system->path_sys->path_count) {
            packet->path_info.path_index = 0;
        }

        path = system->path_sys->paths[packet->path_info.path_index];
        packet->path_info.path = path;
        packet->path_info.connection = NULL;
        packet->current_route = 0xff;
        packet->next_route = 0;
        AISysResetPathSearchConnectionChecks(packet->path_info.path);
        position_inside_path_bounds = 0;
    }

    packet->path_info.path = original_path;
    packet->path_info.connection = original_connection;
    packet->path_info.direction = original_direction;
}

void __cdecl AISysGetPathPos(AISYS_PC *system, NUVEC *position, AIPATHINFO_PC *info, AIPATH_PC *path, i32 checks) {
    AISysGetPathPosEx(system, position, info, path, checks, 0, NULL);
}

void __cdecl AIMoveInstruction(AIPACKET_PC *packet, NUVEC *destination, f32 stopping_distance, AIPATHINFO_PC *path_info,
                               i32 mode, f32 movement_parameter) {
    AIGROUP_PC *group;
    group = packet->group;
    if (group != NULL && (group->flags & 8u)) {
        switch (mode) {
            case 4:
                if (group->leader == (APIOBJECT_PC *)packet->owner)
                    FormationMove(group, RowMoveWander);
                AIFormationFollow(packet);
                return;
            case 5:
                mode = 1;
                break;
            case 1:
                if (group->leader == (APIOBJECT_PC *)packet->owner)
                    FormationMove(group, RowMoveTowards);
                AIFormationFollow(packet);
                return;
        }
    }
    if (destination != NULL) {
        packet->fallback_destination = *destination;
    }
    if (path_info != NULL) {
        packet->fallback_path_info = *path_info;
    }
    packet->fallback_stopping_distance = stopping_distance;
    packet->flags = (packet->flags & ~7u) | (mode & 7);
    packet->movement_parameter = movement_parameter;
}

void __cdecl AIFormationFollow(AIPACKET_PC *packet) {
    NUVEC offset;
    u8 column;
    f32 column_offset;
    NUVEC destination;
    AIGROUP_PC *group;
    AIROW_PC *row;
    group = packet->group;
    if (packet->group_row < group->row_count) {
        row = &group->rows[packet->group_row];
        column = packet->group_column;
        if ((packet->flags & 0x01000000u) != 0) {
            offset.x = (group->count_across & 1) != 0 ? 0.0f : -(0.5f * group->x_spacing);
        } else {
            column_offset = ((column + 1) / 2) * group->x_spacing;
            offset.x = (column & 1) != 0 ? -column_offset : column_offset;
        }
        if ((group->flags & 4u)) {
            offset.x = -offset.x;
        }
        offset.y = 0.0f;
        offset.z = 0.0f;
        NuVecRotateY(&offset, &offset, row->rotation);
        NuVecAdd(&destination, &offset, &row->position);
        packet->flags |= 8u;
        packet->fallback_destination = destination;
        packet->fallback_path_info = row->path_info;
        packet->fallback_stopping_distance = 0.0f;
        packet->movement_parameter = 0.0f;
        packet->flags = (packet->flags & ~7u) | ((group != NULL && (group->flags & 8u)) ? 1u : 5u);
        packet->script_action_position.x = 0.0f;
        packet->script_action_position.y = 0.0f;
        packet->script_action_position.z = 100.0f;
        NuVecRotateY(&packet->script_action_position, &packet->script_action_position, row->rotation);
        NuVecAdd(&packet->script_action_position, &packet->script_action_position, &row->position);
        packet->movement_look_target = &packet->script_action_position;
    }
}

void AIRetreatFromDestination2(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    NUVEC difference;
    f32 distance;
    NUVEC position;
    NUVEC path_position;
    AIPATHINFO_PC path_info;

    distance = NuVecXZDist(&object->position, &packet->fallback_destination, &difference);
    if (distance != 0.0f) {
        NuVecScale(&difference, &difference, packet->movement_parameter / distance);
    } else {
        difference.x = packet->movement_parameter;
        difference.y = 0.0f;
        difference.z = 0.0f;
    }
    NuVecAdd(&packet->movement_destination, &packet->fallback_destination, &difference);
    packet->movement_stopping_distance = 0.0f;
    distance = NuVecXZDist(&packet->movement_destination, &object->position, &difference);
    if (distance > ai_moveradius) {
        NuVecScale(&difference, &difference, ai_moveradius / distance);
        NuVecAdd(&packet->movement_destination, &object->position, &difference);
    }

    position = object->position;
    path_position = packet->last_path_position;
    path_info = packet->path_info;
    object->position = packet->movement_destination;
    AISysGetCharacterPathPos(system, object, packet, 0, 1);
    packet->movement_destination = packet->last_path_position;
    packet->path_info = path_info;
    object->position = position;
    packet->last_path_position = path_position;
}

void AIRetreatFromDestination(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    NUVEC difference;
    f32 distance;

    if (new_retreat != 0) {
        AIRetreatFromDestination2(system, packet, object, checks);
        return;
    }
    if ((packet->path_info.flags & 1u) != 0) {
        distance = NuVecXZDist(&object->position, &packet->fallback_destination, &difference);
        if (distance != 0.0f) {
            NuVecScale(&difference, &difference, packet->movement_parameter / distance);
        } else {
            difference.x = packet->movement_parameter;
            difference.y = 0.0f;
            difference.z = 0.0f;
        }
        NuVecAdd(&packet->movement_destination, &object->position, &difference);
        packet->movement_stopping_distance = 0.0f;
    } else {
        AIRetreatFromDestinationPath(system, packet, object, checks);
    }
}

void __cdecl AISysCharacterMovement(AISYS_PC *system, AIPACKET_PC *packet, APIOBJECT_PC *object, i32 checks) {
    u32 mode;

    packet->flags &= ~0x20000000u;
    if (MidSpecialMoveFn != NULL && MidSpecialMoveFn(system, packet, object) != 0) {
        packet->flags |= 0x80080000u;
        return;
    }
    packet->movement_destination = object->position;
    packet->movement_stopping_distance = 0.0f;
    if (packet->path_info.path != NULL && packet->path_info.connection != NULL) {
        switch (packet->flags & 7) {
            case 1:
                AIMoveToDestination(system, packet, object, checks);
                break;
            case 2:
                AIRetreatFromDestination(system, packet, object, checks);
                break;
            case 3:
                AICircle(system, packet, object, checks);
                break;
            case 4:
                AIWander(system, packet, object, checks);
                break;
            case 6:
                AIMoveToDestinationAvoidingCamera(system, packet, object, checks);
                break;
            case 7:
                AIMoveDirectlyToDestination(system, packet, object, checks);
                break;
        }
    } else {
        mode = packet->flags & 7;
        if (mode == 1 || mode == 7 || mode == 6) {
            packet->movement_stopping_distance = packet->fallback_stopping_distance;
            packet->movement_destination = packet->fallback_destination;
        }
    }
    packet->flags &= ~7u;
}

AIMESSAGESYS_PC *__cdecl CreateAIMessageSys(VARIPTR *cursor, VARIPTR *end, i32 count) {
    AIMESSAGESYS_PC *sys;
    PATH_LOAD_ALLOCATE(AIMESSAGESYS_PC *, sys, cursor, end, sizeof(AIMESSAGESYS_PC));
    if (sys != NULL) {
        memset(sys, 0, sizeof(*sys));
        sys->messages = (AIMESSAGE_PC *)AISysBufferAlloc(cursor, end, count * sizeof(AIMESSAGE_PC));
        if (sys->messages != NULL) {
            sys->count = count;
            ResetAIMessageSys(sys);
        }
    }
    return sys;
}

void __cdecl SetAIMessage(AIMESSAGESYS_PC *sys, char *name, f32 value, AIMESSAGE_PC *message) {
    message = CheckAIMessage(sys, name, message);
    if (message != NULL)
        message->value = value;
}

f32 __cdecl GetAIMessage(AIMESSAGESYS_PC *sys, char *name, AIMESSAGE_PC *message) {
    message = CheckAIMessage(sys, name, message);
    if (message != NULL)
        return message->value;
    return 0.0f;
}

void __cdecl aieditor_Reset(void) {
    aieditorsettings.character_type = -1;
    aieditorsettings.creature_name[0] = 0;
    aieditorsettings.path_name[0] = 0;
    aieditorsettings.locator_set_name[0] = 0;
    aieditor_SetCurrentScript("default", NULL);
}

i32 __cdecl aieditor_Proc(f32 menu_delta_time, FRAME_PAD_PC *pad, f32 frame_delta_time, u32 exit_buttons) {
    NUVEC movement;
    NUVEC previous_position;
    f32 terrain_height;
    i32 platform;
    f32 ground_height;
    f32 roof_height;
    f32 closest;
    f32 distance;
    i32 hit;
    struct nuhspecial_s *platform_handle;
    eduimenu_s *(__cdecl * process)(FRAME_PAD_PC *);
    aieditor->pad_buttons = pad->digital_buttons;
    aieditor->pad_pressed = pad->digital_buttons_pressed;
    if (aieditor->main_menu != NULL) {
        eduiMenuProcess(aieditor->main_menu, menu_delta_time, pad);
        return 0;
    }
    if (NuStrLen(aieditor->warning) != 0) {
        aieditor->main_menu = aieditor_AddMessage("Warning", aieditor->warning);
        NuStrCpy(aieditor->warning, "");
        if (aieditor->main_menu != NULL)
            return 0;
    }
    aieditorsettings.elapsed_time = NuFmod(aieditorsettings.elapsed_time + frame_delta_time, 1.0f);
    if ((pad->digital_buttons & 0x100) == 0 && (aieditor->flags & 4) == 0)
        edcamMoveEx(pad, frame_delta_time);
    previous_position = aieditor->cursor_position;
    edcamGetPosAng(&aieditor->cursor_position, &aieditor->camera_pitch, &aieditor->camera_yaw);
    NuVecSub(&aieditor->cursor_movement, &aieditor->cursor_position, &previous_position);
    aieditor->flags &= ~4;
    platform_handle = &aieditor->cursor_platform;
    platform_handle->scene = NULL;
    platform_handle->special = NULL;
    platform_handle->display_special = NULL;
    aieditor->camera_position = aieditor->cursor_position;
    terrain_height = AITerrShadow(&aieditor->cursor_position, 0.0f, 5.0f, -1);
    platform = AITerrShadowOnPlatform();
    NuSpecialFindByPlatformID(aieditor->enter_context, &aieditor->cursor_platform, platform);
    if (aieditorsettings.display_options & 0x10) {
        ground_height = EShadY;
        roof_height = GetSurfaceInfoRoofHeightPc;
        closest = 2000000.0f;
        if (terrain_height != 2000000.0) {
            closest = AIPathFabs(aieditor->cursor_position.y - terrain_height);
            aieditor->camera_position.y = terrain_height;
        }
        if (ground_height != 2000000.0) {
            distance = aieditor->cursor_position.y - ground_height;
            if (AIPathFabs(distance) < closest) {
                closest = AIPathFabs(distance);
                aieditor->camera_position.y = ground_height;
            }
        }
        if (roof_height != 2000000.0 && AIPathFabs(aieditor->cursor_position.y - roof_height) < closest)
            aieditor->camera_position.y = roof_height;
        if (disable_cylinder_check == 0 && aieditor->current_path != NULL &&
            aieditor->current_path->current_node != NULL &&
            aieditor->current_path->current_node == aieditor->current_path->nearest_node &&
            aieditor->cursor_position.y > aieditor->camera_position.y &&
            aieditor->camera_position.y == terrain_height) {
            NuVecSub(&movement, &aieditor->camera_position, &aieditor->cursor_position);
            hit = NewRayCastScaleY(&aieditor->cursor_position, &movement, aieditor->current_path->current_node->radius,
                                   AiEditorCursorRayScalePc, 0);
            if (hit != 0 && (hit & 0x10) == 0)
                NuVecAdd(&aieditor->camera_position, &aieditor->cursor_position, &movement);
        }
        aieditor->camera_position.y += aieditorsettings.height_offset;
        if (aieditorsettings.display_options & 0x20)
            aieditor->cursor_position = aieditor->camera_position;
    }
    process = aieditorsettings.modes.entries[aieditorsettings.modes.current].process;
    if (process != NULL)
        aieditor->main_menu = process(pad);
    if (aieditor->main_menu != NULL)
        return 0;
    return (pad->digital_buttons_pressed & exit_buttons) != 0;
}

void __cdecl areaEditor_cbRenameAreaMenu(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    (void)item;
    (void)event;
    if (aieditor->selected_area == NULL)
        return;
    menu = eduiMenuCreate(240, 90, 240, 250, ed_fnt, AiEditorRenameAreaCancelPc, "Rename Area");
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu, eduiItemTextPickCreate(0, &AiEditorSaveMessageColoursPc, AiEditorRenameAreaPc, "Area Name"));
    strcpy(((edui_textpicker_s *)edui_last_item)->value, aieditor->selected_area->name);
    ((edui_textpicker_s *)edui_last_item)->max_length = 15;
    eduiMenuAttach(parent, menu);
    menu->x = parent->x + 10;
    menu->y = parent->y + 40;
}

void __cdecl areaEditor_cbDeleteArea(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDAICREATURE_s *creature;
    (void)menu;
    (void)event;
    if (item != NULL && item->data != 0 && aieditor->selected_area != NULL &&
        aieditor->selected_area == aieditor->hovered_area) {
        for (creature = (EDAICREATURE_s *)NuLinkedListGetHead(&aieditor->creatures); creature != NULL;
             creature = (EDAICREATURE_s *)NuLinkedListGetNext(&aieditor->creatures, &creature->link)) {
            if (creature->trigger_area == aieditor->selected_area)
                creature->trigger_area = NULL;
            if (creature->activation_area == aieditor->selected_area)
                creature->activation_area = NULL;
        }
        areaEditorDestroyArea(aieditor->selected_area);
        aieditor->selected_area = NULL;
    }
    AiEditorClearMainMenuPc();
}

void __cdecl areaEditor_cbCancelDeleteAreaMenu(eduimenu_s *menu, eduimenu_s *child) {
    (void)menu;
    (void)child;
    AiEditorClearMainMenuPc();
}

void __cdecl areaEditor_Render(i32 x, i32 y, f32 xscale, f32 yscale) {
    EDAIAREA_s *focus;
    NUVEC delta;
    f32 range;
    i32 text_x = (x + 10) * 16;
    (void)xscale;
    (void)yscale;

    NuQFntPrintEx(system_qfont, text_x, y * 8 - 40, 16, "Area Editor");
    NuQFntSetColour(system_qfont, 0x80000000);
    if (aieditor->selected_area != NULL)
        focus = aieditor->selected_area;
    else if (aieditor->hovered_area != NULL)
        focus = aieditor->hovered_area;
    else
        focus = NULL;
    if (focus != NULL) {
        range = NuVecXZDist(&focus->position, &aieditor->camera_position, &delta);
        y += 15;
        NuQFntPrintEx(system_qfont, text_x, y * 8, 16, "\"%s\", xzrng=%.2f", focus->name, (f64)range);
    }
    y += 15;
    NuQFntPrintEx(system_qfont, text_x, y * 8, 16, "SQR - Options");
    y += 15;
    if (aieditor->hovered_area != NULL) {
        if (aieditor->selected_area == aieditor->hovered_area) {
            NuQFntPrintEx(system_qfont, text_x, y * 8, 16, AiAntinodeRenderMovePc);
            if (aieditor->pad_buttons & 0x40) {
                if (aieditor->hovered_area->flags & 1) {
                    NuQFntPrintEx(system_qfont, text_x, (y + 15) * 8, 16, "LRIGHT/LLEFT - Adjust Radius");
                } else {
                    y += 15;
                    NuQFntPrintEx(system_qfont, text_x, y * 8, 16, "LRIGHT/LLEFT - Adjust X size");
                    NuQFntPrintEx(system_qfont, text_x, (y + 15) * 8, 16, "LUP/LDOWN - Adjust Z size");
                }
            } else {
                y += 15;
                NuQFntPrintEx(system_qfont, text_x, y * 8, 16, "TRI - Delete selected");
                if (!(aieditor->hovered_area->flags & 1)) {
                    y += 15;
                    NuQFntPrintEx(system_qfont, text_x, y * 8, 16, "LLEFT/LRIGHT - Rotate");
                }
                y += 15;
                NuQFntPrintEx(system_qfont, text_x, y * 8, 16, "LUP - Increase height");
                NuQFntPrintEx(system_qfont, text_x, (y + 15) * 8, 16, "LDOWN - Decrease height");
            }
        } else {
            NuQFntPrintEx(system_qfont, text_x, y * 8, 16, "X - Select area");
            NuQFntPrintEx(system_qfont, text_x, (y + 15) * 8, 16, AiAntinodeRenderNearestPc);
        }
    } else {
        NuQFntPrintEx(system_qfont, text_x, y * 8, 16, "X - Create area");
        NuQFntPrintEx(system_qfont, text_x, (y + 15) * 8, 16, AiAntinodeRenderNearestPc);
    }
    areaEditorDrawAreas();
    pathEditorDrawPaths();
    if (aieditorsettings.display_options & 8)
        creatureEditor_RenderAllCreatures();
    antinodeEditorDrawAntinodes();
    locatorEditorDrawLocators();
}

static void AiEditorDestroyLocatorPc(EDLOCATOR_s *locator) {
    EDLOCATORSET_s *set;
    if (locator == NULL)
        return;
    for (set = (EDLOCATORSET_s *)NuLinkedListGetHead(&aieditor->locator_sets); set != NULL;
         set = (EDLOCATORSET_s *)NuLinkedListGetNext(&aieditor->locator_sets, &set->link)) {
        AiEditorRemoveLocatorFromSetPc(set, locator);
    }
    NuLinkedListRemove(&aieditor->locators, &locator->link);
    memset(locator, 0, sizeof(*locator));
    NuLinkedListAppend(&aieditor->free_locators, &locator->link);
}

void __cdecl locatorEditor_PathNodeDeleted(EDAIPATHNODE_s *node) {
    EDLOCATOR_s *locator;
    EDLOCATOR_s *next;
    locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators);
    while (locator != NULL) {
        next = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &locator->link);
        if (locator->path_check.first == node || locator->path_check.second == node) {
            pathEditor_OnPathCheck(&locator->position, &locator->path_check, aieditor->current_path, 0.0f);
            if (locator->path_check.on_path == 0) {
                AiEditorDestroyLocatorPc(locator);
                if (locator == aieditor->current_locator)
                    aieditor->current_locator = NULL;
            }
        }
        locator = next;
    }
}

void __cdecl locatorEditor_PathDeleted(EDAIPATH_s *path) {
    EDLOCATOR_s *locator;
    EDLOCATOR_s *next;
    locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators);
    while (locator != NULL) {
        next = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &locator->link);
        if (locator->path_check.path == path) {
            AiEditorDestroyLocatorPc(locator);
            if (locator == aieditor->current_locator)
                aieditor->current_locator = NULL;
        }
        locator = next;
    }
}

void __cdecl locatorEditor_cbRenameLocatorMenu(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    (void)item;
    (void)event;
    if (aieditor->current_locator == NULL)
        return;
    menu = eduiMenuCreate(240, 90, 240, 250, ed_fnt, AiEditorRenameLocatorCancelPc, "Rename Locator");
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu,
                    eduiItemTextPickCreate(0, &AiEditorSaveMessageColoursPc, AiEditorRenameLocatorPc, "Locator Name"));
    strcpy(((edui_textpicker_s *)edui_last_item)->value, aieditor->current_locator->name);
    ((edui_textpicker_s *)edui_last_item)->max_length = 15;
    eduiMenuAttach(parent, menu);
    menu->x = parent->x + 10;
    menu->y = parent->y + 40;
}

static void __cdecl AiEditorDeleteLocatorSelectionPc(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDAICREATURE_s *creature;
    (void)menu;
    (void)event;
    if (item != NULL && item->data != 0 && aieditor->current_locator != NULL &&
        aieditor->current_locator == aieditor->nearest_locator) {
        for (creature = (EDAICREATURE_s *)NuLinkedListGetHead(&aieditor->creatures); creature != NULL;
             creature = (EDAICREATURE_s *)NuLinkedListGetNext(&aieditor->creatures, &creature->link)) {
            if (creature->locator == aieditor->current_locator)
                creature->locator = NULL;
        }
        AiEditorDestroyLocatorPc(aieditor->current_locator);
        aieditor->current_locator = NULL;
    }
    if (aieditor->main_menu != NULL) {
        eduiMenuDestroy(aieditor->main_menu);
        aieditor->main_menu = NULL;
    }
}

static void __cdecl AiEditorDeleteLocatorCancelPc(eduimenu_s *menu, eduimenu_s *child) {
    (void)menu;
    (void)child;
    if (aieditor->main_menu != NULL) {
        eduiMenuDestroy(aieditor->main_menu);
        aieditor->main_menu = NULL;
    }
}

void __cdecl locatorEditor_cbCreateLocatorSet(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDLOCATORSET_s *set = AiEditorCreateLocatorSetPc();
    EDLOCATORSET_s *other;
    char name[16];
    i32 index = 0;
    (void)menu;
    (void)item;
    (void)event;
    if (set == NULL)
        return;
    do {
        sprintf(name, "NewSet%d", ++index);
        other = (EDLOCATORSET_s *)NuLinkedListGetHead(&aieditor->locator_sets);
        while (other != NULL) {
            if (NuStrICmp(name, other->name) == 0)
                break;
            other = (EDLOCATORSET_s *)NuLinkedListGetNext(&aieditor->locator_sets, &other->link);
        }
    } while (other != NULL);
    strcpy(set->name, name);
    aieditor->current_locator_set = set;
    aieditor->current_locator = NULL;
    AiEditorClearMainMenuPc();
}

void __cdecl locatorEditor_cbDeleteLocatorSet(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    (void)event;
    if (item == NULL)
        return;
    switch (item->data) {
        case 0:
            menu = eduiMenuCreate(240, 90, 240, 250, ed_fnt, NULL, "Delete current locator set?");
            if (menu != NULL) {
                eduiMenuAddItem(menu, eduiItemSelCreate(2, &AiEditorSaveMessageColoursPc, 0, 0,
                                                        locatorEditor_cbDeleteLocatorSet, (char *)MenuPcString75f2c0));
                eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0,
                                                        locatorEditor_cbDeleteLocatorSet, (char *)MenuPcString75f2c4));
                eduiMenuAttach(parent, menu);
            }
            break;
        case 1:
            if (aieditor->current_locator_set != NULL) {
                AiEditorDestroyLocatorSetPc(aieditor->current_locator_set);
                aieditor->current_locator_set = NULL;
                AiEditorClearMainMenuPc();
            }
            break;
        case 2:
            AiEditorClearMainMenuPc();
            break;
    }
}

void __cdecl locatorEditor_cbEmptyLocatorSet(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    i32 index;
    (void)event;
    if (item == NULL)
        return;
    switch (item->data) {
        case 0:
            menu = eduiMenuCreate(240, 90, 240, 250, ed_fnt, NULL, "Empty current locator set?");
            if (menu != NULL) {
                eduiMenuAddItem(menu, eduiItemSelCreate(2, &AiEditorSaveMessageColoursPc, 0, 0,
                                                        locatorEditor_cbEmptyLocatorSet, (char *)MenuPcString75f2c0));
                eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0,
                                                        locatorEditor_cbEmptyLocatorSet, (char *)MenuPcString75f2c4));
                eduiMenuAttach(parent, menu);
            }
            break;
        case 1:
            if (aieditor->current_locator_set != NULL) {
                for (index = 0; index < 64; ++index)
                    aieditor->current_locator_set->locators[index] = NULL;
            }
            AiEditorClearMainMenuPc();
            break;
        case 2:
            AiEditorClearMainMenuPc();
            break;
    }
}

static void __cdecl AiEditorRenameLocatorSetCancelPc(eduimenu_s *menu, eduimenu_s *child) {
    (void)menu;
    (void)child;
    AiEditorClearMainMenuPc();
}

void __cdecl locatorEditor_cbRenameLocatorSetMenu(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    (void)item;
    (void)event;
    if (aieditor->current_locator_set == NULL)
        return;
    menu = eduiMenuCreate(240, 90, 240, 250, ed_fnt, AiEditorRenameLocatorSetCancelPc, "Rename Locator Set");
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu, eduiItemTextPickCreate(0, &AiEditorSaveMessageColoursPc, AiEditorRenameLocatorSetPc,
                                                 (char *)GizObstacleActionNamePC));
    strcpy(((edui_textpicker_s *)edui_last_item)->value, aieditor->current_locator_set->name);
    ((edui_textpicker_s *)edui_last_item)->max_length = 15;
    eduiMenuAttach(parent, menu);
    menu->x = parent->x + 10;
    menu->y = parent->y + 40;
}

void __cdecl locatorEditor_cbAddLocatorsByNameYesNo(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDLOCATOR_s *locator;
    i32 index;
    (void)menu;
    (void)event;
    if (item != NULL && item->data != 0 && NuStrLen(aieditor->pending_locator_name) != 0 &&
        aieditor->current_locator_set != NULL) {
        locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators);
        while (locator != NULL) {
            if (NuStrNICmp(aieditor->pending_locator_name, locator->name, -1) == 0) {
                for (index = 0; index < 64; ++index) {
                    if (aieditor->current_locator_set->locators[index] == NULL) {
                        AiEditorAddLocatorToSetPc(aieditor->current_locator_set, locator, NULL);
                        break;
                    }
                    if (NuStrICmp(aieditor->current_locator_set->locators[index]->name, locator->name) > 0) {
                        i32 length = NuStrLen(aieditor->current_locator_set->locators[index]->name);
                        if (length >= NuStrLen(locator->name)) {
                            AiEditorAddLocatorToSetPc(aieditor->current_locator_set, locator,
                                                      aieditor->current_locator_set->locators[index]);
                            AiEditorAddLocatorToSetPc(aieditor->current_locator_set,
                                                      aieditor->current_locator_set->locators[index], locator);
                            break;
                        }
                    }
                }
            }
            locator = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &locator->link);
        }
        aieditor->current_locator = aieditor->current_locator_set->locators[0];
        if (aieditor->current_locator != NULL) {
            aieditor->current_path = aieditor->current_locator->path_check.path;
            if (aieditor->current_locator != NULL)
                edcamSetPos(&aieditor->current_locator->position);
        }
    }
    memset(aieditor->pending_locator_name, 0, sizeof(aieditor->pending_locator_name));
    AiEditorClearMainMenuPc();
}

void __cdecl locatorEditor_cbAddLocatorsByName(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    AIEDITOR_RENDER_STATE *state;
    (void)item;
    (void)event;
    if (aieditor->current_locator_set == NULL)
        return;
    menu = eduiMenuCreate(240, 90, 240, 250, ed_fnt, AiEditorRenameLocatorSetCancelPc, "Add Locators By Name");
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu, eduiItemTextPickCreate(0, &AiEditorSaveMessageColoursPc, AiEditorSetPendingLocatorNamePc,
                                                 (char *)GizObstacleActionNamePC));
    state = aieditor;
    if (state->current_locator != NULL)
        strcpy(state->pending_locator_name, state->current_locator->name);
    else
        strcpy(state->pending_locator_name, state->current_locator_set->name);
    NuStrCpy(((edui_textpicker_s *)edui_last_item)->value, aieditor->pending_locator_name);
    ((edui_textpicker_s *)edui_last_item)->max_length = 15;
    eduiMenuAttach(parent, menu);
    menu->x = parent->x + 10;
    menu->y = parent->y + 40;
}

void __cdecl locatorEditor_cbSetLocatorSet(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDLOCATORSET_s *set;
    i32 index;
    (void)menu;
    (void)event;
    if (item != NULL) {
        if (item->data != 0) {
            set = (EDLOCATORSET_s *)NuLinkedListGetHead(&aieditor->locator_sets);
            for (index = 1; set != NULL; ++index) {
                if (index == item->data)
                    break;
                set = (EDLOCATORSET_s *)NuLinkedListGetNext(&aieditor->locator_sets, &set->link);
            }
            if (set != NULL && set != aieditor->current_locator_set) {
                aieditor->current_locator_set = set;
                if (aieditor->current_locator_set != NULL) {
                    aieditor->current_locator = aieditor->current_locator_set->locators[0];
                    if (aieditor->current_locator != NULL) {
                        aieditor->current_path = aieditor->current_locator->path_check.path;
                        if (aieditor->current_locator != NULL)
                            edcamSetPos(&aieditor->current_locator->position);
                    }
                } else {
                    aieditor->current_locator = NULL;
                }
            }
        } else {
            aieditor->current_locator_set = NULL;
        }
    }
    AiEditorClearMainMenuPc();
}

void __cdecl locatorEditor_cbSelectLocatorSet(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    EDLOCATORSET_s *set;
    i32 index;
    (void)item;
    (void)event;
    menu = eduiMenuCreate(220, 70, 240, 250, ed_fnt, locatorEditor_cbCancelSelectLocatorSet, "Select Locator Set");
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu,
                    eduiItemSelCreate(0, &AiEditorSaveMessageColoursPc, 0, 0, locatorEditor_cbSetLocatorSet, "NONE"));
    index = 1;
    set = (EDLOCATORSET_s *)NuLinkedListGetHead(&aieditor->locator_sets);
    while (set != NULL) {
        eduiMenuAddItem(menu, eduiItemSelCreate(index, &AiEditorSaveMessageColoursPc, 0, 0,
                                                locatorEditor_cbSetLocatorSet, set->name));
        ++index;
        set = (EDLOCATORSET_s *)NuLinkedListGetNext(&aieditor->locator_sets, &set->link);
    }
    eduiMenuAttach(parent, menu);
}

void __cdecl locatorEditor_Render(i32 x, i32 y, f32 xscale, f32 yscale) {
    EDLOCATOR_s *display;
    NUVEC delta;
    f32 range;
    i32 text_x = (x + 10) * 16;
    NURND_VERTEX3D_PC vertices[2];
    NUVEC current;
    NUVEC previous;
    NUVEC direction;
    NUVEC midpoint;
    i32 index;

    if (aieditor->current_locator_set != NULL) {
        NuQFntPrintEx(system_qfont, text_x, y * 8 - 40, 16, "Locator Editor: Set=\"%s\"",
                      aieditor->current_locator_set->name);
    } else {
        NuQFntPrintEx(system_qfont, text_x, y * 8 - 40, 16, "Locator Editor: Set=\"NONE\"");
    }
    NuQFntSetColour(system_qfont, 0x80000000);
    NuQFntSetScale(system_qfont, xscale, yscale);
    if (aieditor->current_locator != NULL)
        display = aieditor->current_locator;
    else if (aieditor->nearest_locator != NULL)
        display = aieditor->nearest_locator;
    else
        display = NULL;
    if (display != NULL) {
        range = NuVecXZDist(&display->position, &aieditor->camera_position, &delta);
        NuQFntPrintEx(system_qfont, text_x, (y + 15) * 8, 16, "\"%s\", xzrng=%.2f", display->name, (f64)range);
    }
    NuQFntPrintEx(system_qfont, text_x, (y + 30) * 8, 16, "SQR - Options");
    if (aieditor->nearest_locator != NULL) {
        if (aieditor->current_locator == aieditor->nearest_locator) {
            NuQFntPrintEx(system_qfont, text_x, (y + 45) * 8, 16, "X - Move selected");
            NuQFntPrintEx(system_qfont, text_x, (y + 60) * 8, 16, "TRI - Delete selected");
            NuQFntPrintEx(system_qfont, text_x, (y + 75) * 8, 16, AiAntinodeRenderRotateLeftPc);
            NuQFntPrintEx(system_qfont, text_x, (y + 90) * 8, 16, AiAntinodeRenderRotateRightPc);
        } else {
            NuQFntPrintEx(system_qfont, text_x, (y + 45) * 8, 16, "X - Select locator");
            NuQFntPrintEx(system_qfont, text_x, (y + 60) * 8, 16, AiAntinodeRenderNearestPc);
        }
    } else {
        NuQFntPrintEx(system_qfont, text_x, (y + 45) * 8, 16, "X - Create locator");
        NuQFntPrintEx(system_qfont, text_x, (y + 60) * 8, 16, AiAntinodeRenderNearestPc);
    }
    if (aieditor->nearest_locator == NULL && aieditor->cursor_check.on_path && aieditorsettings.elapsed_time < 0.5f) {
        DrawLocator(&aieditor->camera_position, aiEditor_LocatorWidth, aieditorsettings.locator_rotation, 0);
    }
    areaEditorDrawAreas();
    locatorEditorDrawLocators();
    pathEditorDrawPaths();
    antinodeEditorDrawAntinodes();
    if (aieditorsettings.display_options & 8)
        creatureEditor_RenderAllCreatures();
    if (aieditor->current_locator_set != NULL) {
        vertices[0].colour = 0x32323232;
        vertices[1].colour = 0x32323232;
        if (aieditor->current_locator_set->locators[0] != NULL)
            current = aieditor->current_locator_set->locators[0]->position;
        for (index = 0; index < 64; index++) {
            if (aieditor->current_locator_set->locators[index] == NULL)
                break;
            previous = current;
            current = aieditor->current_locator_set->locators[index]->position;
            if (index != 0) {
                vertices[0].position = current;
                vertices[1].position = previous;
                AiRndrLine3d(vertices, NULL, NULL);
                NuVecSub(&direction, &previous, &current);
                NuVecNorm(&direction, &direction);
                NuVecScale(&direction, &direction, 0.05f);
                NuVecAdd(&midpoint, &current, &previous);
                NuVecScale(&midpoint, &midpoint, 0.5f);
                vertices[0].position = midpoint;
                vertices[0].position.x -= direction.x * 0.5f;
                vertices[0].position.y -= direction.y * 0.5f;
                vertices[0].position.z -= direction.z * 0.5f;
                NuVecRotateY(&vertices[1].position, &direction, 0xe39);
                NuVecAdd(&vertices[1].position, &vertices[1].position, &vertices[0].position);
                AiRndrLine3d(vertices, NULL, NULL);
                NuVecRotateY(&vertices[1].position, &direction, -0xe39);
                NuVecAdd(&vertices[1].position, &vertices[1].position, &vertices[0].position);
                AiRndrLine3d(vertices, NULL, NULL);
            }
        }
    }
}

void __cdecl locatorEditor_Enter(void) {
    AILOCATOR_PC *source;
    AILOCATORSET_PC *source_set;
    EDLOCATOR_s *locator;
    EDLOCATORSET_s *set;
    EDAIPATH_s *path;
    f32 offset;
    i32 rotation;
    i32 i;
    i32 member;
    memset(&aieditor->locators, 0, sizeof(EDLOCATOR_s));
    for (i = 0; i < 256; ++i)
        NuLinkedListAppend(&aieditor->free_locators, &aieditor->locator_pool[i].link);
    memset(&aieditor->locator_sets, 0, sizeof(EDLOCATORSET_s));
    for (i = 0; i < 64; ++i)
        NuLinkedListAppend(&aieditor->free_locator_sets, &aieditor->locator_set_pool[i].link);
    if (aieditor->ai_system != NULL) {
        for (i = 0; i < aieditor->ai_system->locator_count; ++i) {
            source = &aieditor->ai_system->locators[i];
            rotation = source->rotation;
            locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->free_locators);
            if (locator != NULL) {
                NuLinkedListRemove(&aieditor->free_locators, &locator->link);
                NuLinkedListAppend(&aieditor->locators, &locator->link);
                locator->position = source->position;
                locator->direction = rotation;
            }
            strcpy(locator->name, source->name);
            path = pathEditor_GetPath(source->path_info.path->name);
            offset = 0.0f;
            pathEditor_OnPathCheck(&locator->position, &locator->path_check, path, offset);
            while (locator->path_check.on_path == 0) {
                offset = offset + (f64)0.01f;
                pathEditor_OnPathCheck(&locator->position, &locator->path_check, path, offset);
            }
            locator->path_check.angle = NuAngSub(locator->direction, locator->path_check.angle);
        }
        for (i = 0; i < aieditor->ai_system->locator_set_count; ++i) {
            source_set = &aieditor->ai_system->locator_sets[i];
            set = (EDLOCATORSET_s *)NuLinkedListGetHead(&aieditor->free_locator_sets);
            if (set != NULL) {
                NuLinkedListRemove(&aieditor->free_locator_sets, &set->link);
                memset(set, 0, sizeof(EDLOCATORSET_s));
                NuLinkedListAppend(&aieditor->locator_sets, &set->link);
            }
            if (source_set != NULL && set != NULL) {
                strcpy(set->name, source_set->name);
                for (member = 0; member < source_set->locator_count; ++member)
                    set->locators[member] = &aieditor->locator_pool[source_set->locator_entries[member]];
            }
        }
    }
    if (aieditor->current_locator_set != NULL)
        strcpy(aieditorsettings.locator_set_name, aieditor->current_locator_set->name);
    if (strlen(aieditorsettings.locator_set_name) != 0)
        aieditor->current_locator_set = locatorEditor_FindLocatorSet(aieditorsettings.locator_set_name);
    else
        aieditor->current_locator_set = NULL;
}

void __cdecl aieditor_RegisterDefaultPathCnxTypes(void) {
    aieditor_RegisterPathCnxType("Permanent Block", 0x40000000, NULL, 0);
    aieditor_RegisterPathCnxType("Temporary Block", 0x80000000, NULL, 0);
    aieditor_RegisterPathCnxType("Link Obstacle", 0x20000000, NULL, 1);
}

void __cdecl pathEditor_DestroyNode(EDAIPATHNODE_s *node, EDAIPATH_s *path) {
    i32 index;
    if (node == NULL || path == NULL)
        return;
    if (node->shared_node != NULL) {
        --node->shared_node->reference_count;
        if (node->shared_node->reference_count < 2)
            pathEditor_DestroySharedNode(node->shared_node);
        node->shared_node = NULL;
    }
    for (index = 0; index < 8; ++index) {
        if (node->connections[index].node != NULL)
            pathEditor_DisconnectNodes(node, node->connections[index].node);
    }
    NuLinkedListRemove(&path->nodes, &node->link);
    --path->node_count;
    memset(node, 0, sizeof(*node));
    NuLinkedListAppend(&aieditor->free_path_nodes, &node->link);
}

AIPATHSYS_PC *__cdecl pathEditorCreateData(VARIPTR *cursor, VARIPTR *end, void **scratch_cursor, void **scratch_end) {
    VARIPTR scratch;
    EDAIPATH_s *first_editor_path;
    i32 path_count;
    EDAIPATH_s *path_iter;
    AI_PATH_BUILD_SYSTEM_PC *system;
    VARIPTR *scratch_limit;
    EDAISHAREDPATHNODE_s *shared;
    i32 path_index;
    EDAIPATH_s *editor_path;

    iterator_count = 0;
    scratch.void_ptr = *scratch_cursor;
    if (aieditor->current_path == NULL) {
        distance_tables = NULL;
        return NULL;
    }
    first_editor_path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->paths);
    if ((first_editor_path == NULL)) {
        distance_tables = NULL;
        return NULL;
    }

    path_count = 0;
    for (path_iter = first_editor_path; path_iter != NULL;
         path_iter = (EDAIPATH_s *)NuLinkedListGetNext(&aieditor->paths, &path_iter->link)) {
        path_iter->draw_index = path_count++;
    }
    if (path_count == 0) {
        distance_tables = NULL;
        return NULL;
    }
    system = (AI_PATH_BUILD_SYSTEM_PC *)AISysBufferAlloc(cursor, end, sizeof(AI_PATH_BUILD_SYSTEM_PC));
    if (system == NULL) {
        distance_tables = NULL;
        return NULL;
    }
    memset(system, 0, sizeof(*system));
    system->path_count = path_count;
    distance_tables = NULL;
    scratch_limit = (VARIPTR *)scratch_end;
    if (system->path_count != 0) {
        distance_tables = (f32 ***)AISysBufferAlloc(&scratch, scratch_limit, system->path_count * sizeof(f32 **));
        memset(distance_tables, 0, system->path_count * sizeof(f32 **));
    }

    system->special_route_count = 0;
    for (shared = (EDAISHAREDPATHNODE_s *)NuLinkedListGetHead(&aieditor->shared_path_nodes); shared != NULL;
         shared = (EDAISHAREDPATHNODE_s *)NuLinkedListGetNext(&aieditor->shared_path_nodes, &shared->link)) {
        shared->runtime_index = system->special_route_count++;
    }
    if (system->special_route_count != 0) {
        system->special_routes = (AIPATHSPECIALROUTE_PC *)AISysBufferAlloc(
            &scratch, scratch_limit, system->special_route_count * sizeof(AIPATHSPECIALROUTE_PC));
        memset(system->special_routes, 0, system->special_route_count * sizeof(AIPATHSPECIALROUTE_PC));
        for (shared = (EDAISHAREDPATHNODE_s *)NuLinkedListGetHead(&aieditor->shared_path_nodes); shared != NULL;
             shared = (EDAISHAREDPATHNODE_s *)NuLinkedListGetNext(&aieditor->shared_path_nodes, &shared->link)) {
            AIPATHSPECIALROUTE_PC *route = &system->special_routes[shared->runtime_index];
            PATH_BUILD_ALLOCATE(AIPATH_PC **, route->paths, &scratch, scratch_limit,
                                shared->reference_count * sizeof(AIPATH_PC));
            memset(route->paths, 0, shared->reference_count * sizeof(AIPATH_PC));
        }
    }
    system->paths = (AIPATH_PC **)AISysBufferAlloc(cursor, end, system->path_count * sizeof(AIPATH_PC *));
    if (system->paths == NULL) {
        distance_tables = NULL;
        return (AIPATHSYS_PC *)system;
    }
    memset(system->paths, 0, system->path_count * sizeof(AIPATH_PC *));
    path_index = 0;
    for (editor_path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->paths); editor_path != NULL;
         editor_path = (EDAIPATH_s *)NuLinkedListGetNext(&aieditor->paths, &editor_path->link), ++path_index) {
        AIPATH_PC *path;
        EDAIPATHNODE_s *node;
        f32 ***distance_slot;
        system->paths[path_index] = (AIPATH_PC *)AISysBufferAlloc(cursor, end, sizeof(AIPATH_PC));
        path = system->paths[path_index];
        if (path == NULL) {
            continue;
        }
        memset(path, 0, sizeof(*path));
        strcpy(path->name, editor_path->name);
        path->index = path_index;

        for (node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&editor_path->nodes); node != NULL;
             node = (EDAIPATHNODE_s *)NuLinkedListGetNext(&editor_path->nodes, &node->link)) {
            i32 slot;
            node->index = path->node_count++;
            for (slot = 0; slot < 8; ++slot) {
                if (node->connections[slot].node != NULL) {
                    ++path->connection_count;
                }
            }
            if (node->shared_node != NULL) {
                ++path->special_route_count;
            }
        }
        if (path->connection_count != 0) {
            path->connection_count /= 2;
            path->connections =
                (AIPATHCNX_PC *)AISysBufferAlloc(cursor, end, path->connection_count * sizeof(AIPATHCNX_PC));
            if (path->connections != NULL) {
                memset(path->connections, 0, path->connection_count * sizeof(AIPATHCNX_PC));
            }
        }
        if (path->special_route_count != 0) {
            path->special_routes = (AIPATHNODELINK_PC *)AISysBufferAlloc(
                cursor, end, path->special_route_count * sizeof(AIPATHNODELINK_PC));
            memset(path->special_routes, 0, path->special_route_count * sizeof(AIPATHNODELINK_PC));
            path->special_route_count = 0;
        }
        if (path->node_count != 0) {
            path->nodes = (AIPATHNODE_PC *)AISysBufferAlloc(cursor, end, path->node_count * sizeof(AIPATHNODE_PC));
            if (path->nodes != NULL) {
                i32 connection_index;
                memset(path->nodes, 0, path->node_count * sizeof(AIPATHNODE_PC));

                connection_index = 0;
                for (node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&editor_path->nodes); node != NULL;
                     node = (EDAIPATHNODE_s *)NuLinkedListGetNext(&editor_path->nodes, &node->link)) {
                    AIPATHNODE_PC *runtime = &path->nodes[node->index];
                    i32 length = strlen(node->name);
                    const volatile f32 *position_y;
                    i32 slot;
                    if (length != 0) {
                        runtime->name = (char *)AISysBufferAlloc(cursor, end, length + 1);
                        strcpy(runtime->name, node->name);
                    }
                    runtime->position = node->position;
                    runtime->radius = node->radius;
                    runtime->radius_squared = node->radius * node->radius;
                    runtime->runtime_flags = node->flags;
                    runtime->special_handle = *(NUSPECIALHANDLE *)&node->special;
                    if (NuSpecialExistsFn(&runtime->special_handle)) {
                        runtime->has_special = 1;
                    }
                    runtime->special_position = node->special_position;
                    position_y = &runtime->position.y;
                    runtime->min_height = node->lower_height + *position_y;
                    runtime->max_height = node->upper_height + *position_y;
                    runtime->min_height_offset = runtime->min_height - *position_y;
                    runtime->max_height_offset = runtime->max_height - *position_y;
                    for (slot = 0; slot < 8; ++slot) {
                        if ((node->connections[slot].node != NULL)) {
                            ++runtime->connection_count;
                        }
                    }
                    if ((path->connections != NULL)) {
                        if (runtime->connection_count != 0) {
                            i32 next;
                            PATH_BUILD_ALLOCATE(AIPATHCNX_PC **, runtime->connections, cursor, end,
                                                runtime->connection_count * sizeof(AIPATHCNX_PC *));
                            if (runtime->connections != NULL) {
                                memset(runtime->connections, 0, runtime->connection_count * sizeof(AIPATHCNX_PC *));
                            }
                            next = 0;
                            for (slot = 0; slot < 8; ++slot) {
                                EDAIPATHNODE_s *other = node->connections[slot].node;
                                if (other == NULL) {
                                    continue;
                                }
                                if (other->index < node->index) {
                                    AIPATHNODE_PC *other_runtime = &path->nodes[other->index];
                                    i32 reverse;
                                    for (reverse = 0; reverse < other_runtime->connection_count; ++reverse) {
                                        AIPATHCNX_PC *connection = other_runtime->connections[reverse];
                                        if (connection->node_indices[1] == node->index) {
                                            runtime->connections[next] = connection;
                                            runtime->connections[next]->traversal_flags[1] =
                                                node->connections[slot].flags;
                                            runtime->connections[next]->original_traversal_flags[1] =
                                                node->connections[slot].flags;
                                            break;
                                        }
                                    }
                                } else {
                                    NUVEC difference;
                                    AIPATHCNX_PC *connection;
                                    runtime->connections[next] = &path->connections[connection_index++];
                                    runtime->connections[next]->node_indices[0] = node->index;
                                    runtime->connections[next]->node_indices[1] = node->connections[slot].node->index;
                                    runtime->connections[next]->traversal_flags[0] = node->connections[slot].flags;
                                    runtime->connections[next]->original_traversal_flags[0] =
                                        node->connections[slot].flags;
                                    runtime->connections[next]->distance = NuVecDist(
                                        &node->connections[slot].node->position, &node->position, &difference);
                                    runtime->connections[next]->horizontal_distance = NuVecXZDist(
                                        &node->connections[slot].node->position, &node->position, &difference);
                                    connection = runtime->connections[next];
                                    if (connection->horizontal_distance == 0.0f) {
                                        connection->horizontal_distance = 0.0001f;
                                    }
                                    runtime->connections[next]->rotation =
                                        (i16)(NuAtan2(difference.x, difference.z) * AIPathAngularScale);
                                }
                                ++next;
                            }
                        }
                    }
                    runtime->distance_cache_nodes[0] = 0xff;
                    runtime->distance_cache_nodes[1] = 0xff;
                    if (node->shared_node != NULL) {
                        AIPATHSPECIALROUTE_PC *shared_route = &system->special_routes[node->shared_node->runtime_index];
                        AIPATHNODELINK_PC *link =
                            &path->special_routes[runtime->special_route_index = path->special_route_count++];
                        shared_route->paths[shared_route->path_count++] = path;
                        link->node_index = node->index;
                        link->special_route_index = node->shared_node->runtime_index;
                    } else {
                        runtime->special_route_index = 0xff;
                    }
                }
            }
        }
        distance_slot = &distance_tables[path_index];
        *distance_slot = pathEditorCalculateDistanceTable(path, 0, &scratch, scratch_limit);

        if (path->node_count != 0) {
            u8 **route_matrix;
            f32 **distances;
            PATH_BUILD_ALLOCATE(u8 **, path->route_matrix, cursor, end, path->node_count * sizeof(u8 *));
            memset(path->route_matrix, 0, path->node_count * sizeof(u8 *));
            {
                u32 source;
                for (source = 0; source < path->node_count; ++source) {
                    PATH_BUILD_ALLOCATE(u8 *, path->route_matrix[source], cursor, end, path->node_count);
                    memset(path->route_matrix[source], 0, path->node_count);
                }
            }
            route_matrix = path->route_matrix;
            distances = *distance_slot;
            pathEditorPopulateRouteMatrix(path, 0, distances, route_matrix);
            pathEditorCreateSpecialRouteData(path, editor_path, &scratch, scratch_limit);
        }
    }
    if (system->path_count > 1) {
        AIPATHINFO_PC info;
        AIPATH_PC *first_path = system->paths[0];
        u32 info_path_index;
        memset(&info, 0, sizeof(info));
        for (info_path_index = 1; info_path_index < system->path_count; ++info_path_index) {
            AIPATH_PC *path = system->paths[info_path_index];
            i32 node_index;
            for (node_index = 0; node_index < path->node_count; ++node_index) {
                AIPATHNODE_PC *node = &path->nodes[node_index];
                AISysGetPathPosEx(aieditor->ai_system, &node->position, &info, first_path, 0xff, 0, NULL);
                if ((info.flags & 1)) {
                    f64 half = InitOnceHalf;
                    f64 path_distance = info.distance;
                    node->path_flags = info.connection - path->connections;
                    if (path_distance > half || (info.direction != 0 && path_distance < half)) {
                        node->runtime_flags |= 4;
                    }
                    node->runtime_flags |= 1;
                    node->path_flags = info.connection - info.path->connections;
                    path->flags |= 2;
                } else {
                    node->path_flags = -1;
                    node->runtime_flags &= (u8)~1;
                }
            }
            if ((!(path->flags & 2) || distance_tables == NULL || distance_tables[info_path_index] == NULL)) {
                continue;
            }
            for (node_index = 0; node_index < path->node_count; ++node_index) {
                AIPATHNODE_PC *node = &path->nodes[node_index];
                f32 nearest;
                i32 candidate;
                if (node->runtime_flags & 1) {
                    continue;
                }
                nearest = FLT_MAX;
                for (candidate = 0; candidate < path->node_count; ++candidate) {
                    if (&path->nodes[candidate] != node && (path->nodes[candidate].runtime_flags & 1) &&
                        distance_tables[info_path_index][node_index][candidate] < nearest) {
                        node->path_flags = candidate;
                        nearest = distance_tables[info_path_index][node_index][candidate];
                    }
                }
            }
        }
    }
    distance_tables = NULL;
    return (AIPATHSYS_PC *)system;
}

static void __cdecl pathEditor_cbSetCurrentPath(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDAIPATH_s *path;
    i32 index = 0;
    (void)menu;
    (void)event;
    if (item != NULL) {
        for (path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->paths); path != NULL;
             index++, path = (EDAIPATH_s *)NuLinkedListGetNext(&aieditor->paths, &path->link)) {
            if (item->data == index) {
                aieditor->current_path = path;
                aieditor->current_path->nearest_node = pathEditor_GetNearestNode(aieditor->current_path, 0);
                aieditor->current_path->current_node = aieditor->current_path->nearest_node;
                if (aieditor->current_path->nearest_node != NULL) {
                    edcamSetPos(&aieditor->current_path->nearest_node->position);
                }
                break;
            }
        }
    }
    AiEditorClearMainMenuPc();
}

void __cdecl pathEditor_cbCreatePath(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDAIPATH_s *path;
    EDAIPATH_s *existing;
    i32 number;
    char name[16];
    (void)menu;
    (void)item;
    (void)event;
    path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->free_paths);
    if (path != NULL) {
        NuLinkedListRemove(&aieditor->free_paths, &path->link);
        NuLinkedListAppend(&aieditor->paths, &path->link);
    }
    number = 0;
    if (path == NULL)
        return;
    do {
        sprintf(name, "NewPath%d", ++number);
        existing = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->paths);
        while (existing != NULL) {
            if (NuStrICmp(name, existing->name) == 0)
                break;
            existing = (EDAIPATH_s *)NuLinkedListGetNext(&aieditor->paths, &existing->link);
        }
    } while (existing != NULL);
    strcpy(path->name, name);
    path->flags &= ~1;
    aieditor->current_path = path;
    AiEditorClearMainMenuPc();
}

void __cdecl pathEditor_cbSelectPathMenu(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    EDAIPATH_s *path;
    i32 index = 0;
    (void)item;
    (void)event;
    menu = eduiMenuCreate(220, 70, 240, 250, ed_fnt, pathEditor_cbCancelSelectMenu, "Select Path");
    if (menu == NULL)
        return;
    for (path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->paths); path != NULL;
         path = (EDAIPATH_s *)NuLinkedListGetNext(&aieditor->paths, &path->link)) {
        eduiMenuAddItem(menu, eduiItemSelCreate(index, &AiEditorSaveMessageColoursPc, 0, 0, pathEditor_cbSetCurrentPath,
                                                path->name));
        index++;
    }
    eduiMenuAttach(parent, menu);
}

void __cdecl pathEditor_cbSetShareNode(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDAISHAREDPATHNODE_s *shared;
    EDAIPATHNODE_s *matches[16];
    EDAIPATHNODE_s *nearest;
    EDAIPATHNODE_s *node;
    EDAIPATH_s *path;
    i32 path_number = 1;
    i32 match_count = 0;
    i32 index;
    f32 best;
    (void)menu;
    (void)event;
    if (aieditor->current_path != NULL && aieditor->current_path->current_node != NULL) {
        shared = aieditor->current_path->current_node->shared_node;
        for (path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->paths); path != NULL;
             path = (EDAIPATH_s *)NuLinkedListGetNext(&aieditor->paths, &path->link)) {
            if (aieditor->current_path == path)
                continue;
            if (item->data == 0 || path_number == item->data) {
                best = FLT_MAX;
                nearest = NULL;
                for (node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&path->nodes); node != NULL;
                     node = (EDAIPATHNODE_s *)NuLinkedListGetNext(&path->nodes, &node->link)) {
                    NUVEC difference;
                    f32 distance;
                    difference.x = node->position.x - aieditor->current_path->current_node->position.x;
                    difference.z = node->position.z - aieditor->current_path->current_node->position.z;
                    distance = NuFsqrt(difference.x * difference.x + difference.z * difference.z);
                    if (distance < node->radius + aieditor->current_path->current_node->radius) {
                        f32 current_bottom = aieditor->current_path->current_node->position.y +
                                             aieditor->current_path->current_node->lower_height;
                        f32 current_top = aieditor->current_path->current_node->position.y +
                                          aieditor->current_path->current_node->upper_height;
                        f32 node_bottom = node->position.y + node->lower_height;
                        f32 node_top = node->position.y + node->upper_height;
                        if (current_bottom > node_top || current_top < node_bottom)
                            continue;
                        if (distance < best) {
                            best = distance;
                            nearest = node;
                        }
                    }
                }
                if (nearest != NULL && match_count < 16) {
                    matches[match_count++] = nearest;
                    if (shared == NULL)
                        shared = nearest->shared_node;
                }
            }
            ++path_number;
        }
        if (match_count != 0) {
            if (item->data != 0 && !(item->flags & 1)) {
                if (aieditor->current_path->current_node->shared_node != NULL) {
                    for (index = 0; index < match_count; ++index) {
                        node = matches[index];
                        if (node->shared_node == aieditor->current_path->current_node->shared_node)
                            pathEditor_ReleaseSharedNode(node);
                    }
                }
            } else if (shared != NULL || (shared = pathEditor_AllocSharedNode()) != NULL) {
                if (aieditor->current_path->current_node->shared_node == NULL)
                    pathEditor_SetSharedNode(aieditor->current_path->current_node, shared);
                for (index = 0; index < match_count; ++index) {
                    node = matches[index];
                    if (node->shared_node != NULL && node->shared_node != shared)
                        pathEditor_ReleaseSharedNode(node);
                    if (node->shared_node != shared)
                        pathEditor_SetSharedNode(node, shared);
                    node->position = aieditor->current_path->current_node->position;
                    node->radius = aieditor->current_path->current_node->radius;
                    node->lower_height = aieditor->current_path->current_node->lower_height;
                    node->upper_height = aieditor->current_path->current_node->upper_height;
                    node->special = aieditor->current_path->current_node->special;
                    node->special_position = aieditor->current_path->current_node->special_position;
                }
            }
        }
    }
    if (item->data == 0)
        AiEditorClearMainMenuPc();
}

void __cdecl pathEditor_cbShareNodeMenu(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    EDAIPATH_s *path;
    EDAIPATHNODE_s *node;
    i32 row;
    i32 selected;
    (void)item;
    (void)event;
    if (aieditor->current_path == NULL || aieditor->current_path->current_node == NULL)
        return;
    menu = eduiMenuCreate(220, 70, 240, 250, ed_fnt, pathEditor_cbCancelSelectMenu, "Share node with...");
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu,
                    eduiItemSelCreate(0, &AiEditorSaveMessageColoursPc, 0, 1, pathEditor_cbSetShareNode, "All paths"));
    row = 1;
    for (path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->paths); path != NULL;
         path = (EDAIPATH_s *)NuLinkedListGetNext(&aieditor->paths, &path->link)) {
        if (aieditor->current_path == path)
            continue;
        selected = 0;
        if (aieditor->current_path->current_node->shared_node != NULL) {
            for (node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&path->nodes); node != NULL;
                 node = (EDAIPATHNODE_s *)NuLinkedListGetNext(&path->nodes, &node->link)) {
                if (node->shared_node == aieditor->current_path->current_node->shared_node) {
                    selected = 1;
                    break;
                }
            }
        }
        eduiMenuAddItem(menu, eduiItemToggleCreate(row, &AiEditorSaveMessageColoursPc, selected, row + 1,
                                                   pathEditor_cbSetShareNode, path->name));
        ++row;
    }
    eduiMenuAttach(parent, menu);
}

void __cdecl pathEditor_cbDeletePathNode(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDAIPATHNODE_s *node;
    (void)menu;
    (void)event;
    if (item != NULL && item->data != 0 && aieditor->current_path->current_node != NULL &&
        aieditor->current_path->current_node == aieditor->current_path->nearest_node) {
        node = aieditor->current_path->current_node;
        pathEditor_DestroyNode(node, aieditor->current_path);
        creatureEditor_PathNodeDeleted(node);
        locatorEditor_PathNodeDeleted(node);
        if (AIPathNodeMovedFn != NULL)
            AIPathNodeMovedFn(node);
        if (aieditor->current_path->current_node == node)
            aieditor->current_path->current_node = NULL;
    }
    AiEditorClearMainMenuPc();
}

void __cdecl pathEditor_cbCancelDeleteNodeMenu(eduimenu_s *menu, eduimenu_s *child) {
    (void)menu;
    (void)child;
    AiEditorClearMainMenuPc();
}

void __cdecl pathEditor_cbDisconnectPathNode(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (item != NULL && item->data != 0 && aieditor->current_path->current_node != NULL &&
        aieditor->current_path->nearest_node != NULL &&
        aieditor->current_path->current_node != aieditor->current_path->nearest_node) {
        pathEditor_DisconnectNodes(aieditor->current_path->current_node, aieditor->current_path->nearest_node);
        creatureEditor_PathNodeDeleted(aieditor->current_path->current_node);
        locatorEditor_PathNodeDeleted(aieditor->current_path->current_node);
        if (AIPathNodeMovedFn != NULL)
            AIPathNodeMovedFn(aieditor->current_path->current_node);
    }
    AiEditorClearMainMenuPc();
}

void __cdecl pathEditor_cbCancelDisconnectNodeMenu(eduimenu_s *menu, eduimenu_s *child) {
    (void)menu;
    (void)child;
    AiEditorClearMainMenuPc();
}

static __declspec(noinline) void pathEditor_PathNodeMoved(EDAIPATHNODE_s *node) {
    if (!(node->flags & 0x80) && NuSpecialExistsFn(&aieditor->cursor_platform)) {
        node->special = aieditor->cursor_platform;
        NuVecInvMtxTransform(&node->platform_position, &node->position,
                             NuSpecialGetDrawMtx(&aieditor->cursor_platform));
    } else {
        node->special.scene = NULL;
        node->special.special = NULL;
        node->special.display_special = NULL;
    }
    creatureEditor_PathNodeMoved(node);
    locatorEditor_PathNodeMoved(node);
    if (AIPathNodeDeletedFn != NULL)
        AIPathNodeDeletedFn(aieditor->current_path->current_node);
    if (node->shared_node != NULL) {
        struct EDAIPATH_s *path;
        for (path = ((EDAIPATH_s *)(NuLinkedListGetHead(&aieditor->paths))); path != NULL;
             path = ((EDAIPATH_s *)(NuLinkedListGetNext(&aieditor->paths, &path->link)))) {
            struct EDAIPATHNODE_s *other;
            for (other = ((EDAIPATHNODE_s *)(NuLinkedListGetHead(&path->nodes))); other != NULL;
                 other = ((EDAIPATHNODE_s *)(NuLinkedListGetNext(&path->nodes, &other->link)))) {
                if (other != node && other->shared_node == node->shared_node) {
                    other->radius = node->radius;
                    other->lower_height = node->lower_height;
                    other->upper_height = node->upper_height;
                    other->position = node->position;
                    other->special = node->special;
                    other->platform_position = node->platform_position;
                    creatureEditor_PathNodeMoved(other);
                    locatorEditor_PathNodeMoved(other);
                    break;
                }
            }
        }
    }
}

void __cdecl pathEditor_Render(i32 x, i32 y, f32 xscale, f32 yscale) {
    if (aieditor->current_path != NULL) {
        if (aieditorsettings.display_options & 1) {
            NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 - 40, 16, "Show Routes : \"%s\"",
                          aieditor->current_path->name);
            NuQFntSetColour(system_qfont, 0x80000000);
            NuQFntSetScale(system_qfont, xscale, yscale);
            NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 120, 16, "\"%s\"", aieditor->current_path->name);
            NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 240, 16, "SQR - Sub menu");
            NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 360, 16, "SELECT - Goto nearest");
            if (aieditor->current_path->runtime_nearest >= 0) {
                NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 480, 16, "TRI - Set as start of route");
                NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 600, 16, "X - Set as end of route");
            }
        } else {
            NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 - 40, 16, "AI Path Editor: \"%s\"",
                          aieditor->current_path->name);
            NuQFntSetColour(system_qfont, 0x80000000);
            NuQFntSetScale(system_qfont, xscale, yscale);
            if (aieditor->current_path->current_node != NULL) {
                if (strlen(aieditor->current_path->current_node->name) != 0) {
                    NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 120, 16, "\"%s\" %d nodes",
                                  aieditor->current_path->current_node->name, aieditor->current_path->node_count);
                } else {
                    NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 120, 16, "ix=%d, %d nodes",
                                  aieditor->current_path->current_node->index, aieditor->current_path->node_count);
                }
            } else {
                NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 120, 16, "%d nodes",
                              aieditor->current_path->node_count);
            }
            NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 240, 16, "SQR - Sub menu");
            NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 360, 16, AiAntinodeRenderNearestPc);
            if (aieditor->flags & 1) {
                NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 480, 16, "X - Move selected");
                NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 600, 16, "TRI - Delete selected");
                if (aieditor->current_path != NULL && aieditor->current_path->current_node != NULL) {
                    NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 840, 16, AiAntinodeRenderIncreaseRadiusPc,
                                  aieditor->current_path->current_node->radius);
                } else {
                    NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 840, 16, "LRIGHT - Increase radius");
                }
                NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 960, 16, AiAntinodeRenderDecreaseRadiusPc);
            } else if (aieditor->current_path->nearest_node != NULL) {
                NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 480, 16, "X - Select");
                NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 720, 16, "O - Link/unlink to selected");
            } else {
                NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 480, 16, "X - Create");
            }
        }
    }
    if (aieditorsettings.display_options & 1)
        pathEditorDrawRoute();
    pathEditorDrawPaths();
    if (aieditorsettings.display_options & 8)
        creatureEditor_RenderAllCreatures();
    areaEditorDrawAreas();
    locatorEditorDrawLocators();
    antinodeEditorDrawAntinodes();
}

void __cdecl routeEditor_cbCreateRoute(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    EDAIPATH_s *path = aieditor->current_path;
    EDAIPATHROUTE_s *route;
    i32 index;
    i32 attempt;
    i32 number;
    i32 i;
    char name[16];
    (void)menu;
    (void)item;
    (void)event;
    index = path->current_route != NULL ? path->current_route - path->routes : 0;
    for (attempt = 0; attempt < 16; ++attempt) {
        if (++index >= 16)
            index = 0;
        route = &path->routes[index];
        if (!(route->flags & 1))
            goto found;
    }
    return;
found:
    route->flags |= 1;
    route->user_mask = 0;
    aieditor->current_path->current_route = route;
    number = 0;
    do {
        sprintf(name, "route%d", ++number);
        for (i = 0; i < 16; ++i) {
            if ((aieditor->current_path->routes[i].flags & 1) &&
                NuStrICmp(name, aieditor->current_path->routes[i].name) == 0)
                break;
        }
    } while (i < 16);
    strcpy(aieditor->current_path->current_route->name, name);
    AiEditorClearMainMenuPc();
}

void __cdecl routeEditor_cbDeleteRoute(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    EDAIPATHNODE_s *node;
    i32 index;
    i32 i;
    u16 mask;
    (void)event;
    if (item == NULL)
        return;
    switch (item->data) {
        case 0:
            menu = eduiMenuCreate(240, 90, 240, 250, ed_fnt, NULL, "Delete current route??");
            if (menu != NULL) {
                eduiMenuAddItem(
                    menu, eduiItemSelCreate(2, &AiEditorSaveMessageColoursPc, 0, 0, routeEditor_cbDeleteRoute, "No"));
                eduiMenuAddItem(
                    menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, routeEditor_cbDeleteRoute, "Yes"));
                eduiMenuAttach(parent, menu);
            }
            break;
        case 1:
            if (aieditor->current_path->current_route == NULL)
                return;
            index = aieditor->current_path->current_route - aieditor->current_path->routes;
            aieditor->current_path->current_route->flags &= ~1;
            aieditor->current_path->current_route = NULL;
            node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&aieditor->current_path->nodes);
            if (node != NULL) {
                mask = (u16) ~((u64)1 << index);
                do {
                    for (i = 0; i < 8; ++i)
                        node->connections[i].route_mask &= mask;
                    node->route_mask &= mask;
                    node = (EDAIPATHNODE_s *)NuLinkedListGetNext(&aieditor->current_path->nodes, &node->link);
                } while (node != NULL);
            }
            for (i = 0; i < 16; ++i) {
                if (++index >= 16)
                    index = 0;
                if (aieditor->current_path->routes[index].flags & 1)
                    aieditor->current_path->current_route = &aieditor->current_path->routes[index];
            }
            AiEditorClearMainMenuPc();
            break;
        case 2:
            eduiMenuDestroy(parent);
            break;
    }
}

void __cdecl routeEditor_Render(i32 x, i32 y, f32 xscale, f32 yscale) {
    if (aieditor->current_path != NULL) {
        NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 - 40, 16, "Edit Routes (Path = \"%s\")",
                      aieditor->current_path->name);
        NuQFntSetColour(system_qfont, 0x80000000);
        NuQFntSetScale(system_qfont, xscale, yscale);
        if (aieditor->current_path->current_route != NULL) {
            NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 120, 16, "\"%s\"",
                          aieditor->current_path->current_route->name);
        } else {
            NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 120, 16, "NO ROUTES AVAILABLE");
        }
        if (aieditor->current_path->current_node != NULL) {
            NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 240, 16, "SQR - Sub menu");
            NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 360, 16, AiAntinodeRenderNearestPc);
            if (aieditor->current_path->nearest_node != NULL &&
                aieditor->current_path->nearest_node != aieditor->current_path->current_node) {
                NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 480, 16, "X - Select");
            }
            if (!(aieditor->flags & 1) && aieditor->current_path->nearest_node != NULL) {
                NuQFntPrintEx(system_qfont, (x + 10) * 16, y * 8 + 720, 16, "O - Add/remove cnx to route.");
            }
        }
    }
    pathEditorDrawPaths();
    if (aieditorsettings.display_options & 8)
        creatureEditor_RenderAllCreatures();
    areaEditorDrawAreas();
    locatorEditorDrawLocators();
    antinodeEditorDrawAntinodes();
}

void __cdecl antinodeEditor_Render(i32 x, i32 y, f32 xscale, f32 yscale) {
    EDANTINODE_s *display;
    EDANTINODE_s *nearest;
    NUVEC displacement;
    i32 text_x = (x + 10) * 16;

    NuQFntPrintEx(system_qfont, text_x, y * 8 - 40, 16, AiAntinodeRenderTitlePc);
    NuQFntSetColour(system_qfont, 0x80000000);
    NuQFntSetScale(system_qfont, xscale, yscale);
    display = aieditor->selected_antinode;
    if (display == NULL)
        display = aieditor->nearest_antinode;
    if (display != NULL)
        NuVecXZDist(&display->position, &aieditor->camera_position, &displacement);
    nearest = aieditor->nearest_antinode;
    if (nearest != NULL && nearest == aieditor->selected_antinode) {
        char *platform_name = NuSpecialGetName(&display->special);
        if (platform_name != NULL)
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 120, 16, AiAntinodeRenderPlatformPc, platform_name);
        else
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 120, 16, AiAntinodeRenderNoPlatformPc);
        NuQFntPrintEx(system_qfont, text_x, y * 8 + 240, 16, AiAntinodeRenderMovePc);
        NuQFntPrintEx(system_qfont, text_x, y * 8 + 360, 16, "TRI - Delete selected");
        if (aieditor->selected_antinode->type == 0) {
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 480, 16, AiAntinodeRenderIncreaseRadiusPc,
                          aieditor->selected_antinode->radius);
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 600, 16, AiAntinodeRenderDecreaseRadiusPc);
        } else if (aieditor->pad_buttons & 0x40) {
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 480, 16, AiAntinodeRenderIncreaseXPc,
                          aieditor->selected_antinode->base_radius);
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 600, 16, AiAntinodeRenderDecreaseXPc);
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 720, 16, AiAntinodeRenderIncreaseZPc,
                          aieditor->selected_antinode->base_height);
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 840, 16, AiAntinodeRenderDecreaseZPc);
        } else if ((aieditorsettings.display_options & 0x100) && (aieditor->pad_buttons & 0x1000)) {
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 480, 16, AiAntinodeRenderIncreaseUpperPc);
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 600, 16, AiAntinodeRenderDecreaseUpperPc);
        } else if ((aieditorsettings.display_options & 0x100) && (aieditor->pad_buttons & 0x4000)) {
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 480, 16, AiAntinodeRenderIncreaseLowerPc);
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 600, 16, AiAntinodeRenderDecreaseLowerPc);
        } else {
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 480, 16, AiAntinodeRenderRotateLeftPc);
            NuQFntPrintEx(system_qfont, text_x, y * 8 + 600, 16, AiAntinodeRenderRotateRightPc);
            if (aieditorsettings.display_options & 0x100) {
                NuQFntPrintEx(system_qfont, text_x, y * 8 + 720, 16, AiAntinodeRenderAdjustUpperPc);
                NuQFntPrintEx(system_qfont, text_x, y * 8 + 840, 16, AiAntinodeRenderAdjustLowerPc);
            }
        }
    } else {
        NuQFntPrintEx(system_qfont, text_x, y * 8 + 240, 16,
                      nearest != NULL ? AiAntinodeRenderSelectPc : AiAntinodeRenderCreatePc);
        NuQFntPrintEx(system_qfont, text_x, y * 8 + 360, 16, AiAntinodeRenderNearestPc);
    }
    antinodeEditorDrawAntinodes();
    areaEditorDrawAreas();
    pathEditorDrawPaths();
    if (aieditorsettings.display_options & 8)
        creatureEditor_RenderAllCreatures();
    locatorEditorDrawLocators();
}

void __cdecl creatureEditor_cbResetParams(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)item;
    (void)event;
    aieditor_SetCurrentScript(aieditorsettings.script_name, NULL);
    if (aieditor->selected_creature != NULL) {
        aieditor->selected_creature->script_params[0] = aieditorsettings.script_params[0];
        aieditor->selected_creature->script_params[1] = aieditorsettings.script_params[1];
        aieditor->selected_creature->script_params[2] = aieditorsettings.script_params[2];
        aieditor->selected_creature->script_params[3] = aieditorsettings.script_params[3];
        aieditor->selected_creature->flags &= ~0x1e;
    }
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
    creatureEditor_ResetParamsItem = NULL;
}

/* Native 534740..534777: private menu argument ESI. */
static __declspec(noinline) void creatureEditorAddResetParamsItem(eduimenu_s *menu) {
    if (menu != NULL && creatureEditor_ResetParamsItem == NULL) {
        creatureEditor_ResetParamsItem =
            eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc, 1, 1,
                                                       creatureEditor_cbResetParams, "Reset Params"));
    }
}

void __cdecl creatureEditor_cbSetScriptParam(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)event;
    if (item != NULL && aieditor->selected_creature != NULL &&
        aieditorsettings.script_params[item->data] != ((edui_slider_s *)item)->value) {
        aieditorsettings.script_params[item->data] = ((edui_slider_s *)item)->value;
        aieditorsettings.script_flags |= (u32)((u64)2 << item->data);
        aieditor->selected_creature->script_params[item->data] = aieditorsettings.script_params[item->data];
        aieditor->selected_creature->flags &= ~0x1e;
        aieditor->selected_creature->flags |= aieditorsettings.script_flags;
        creatureEditorAddResetParamsItem(menu);
    }
}

void __cdecl creatureEditor_cbScriptParams(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    AISCRIPT_PC *script;
    char text[16];
    i32 index;
    (void)item;
    (void)event;
    if (aieditor->selected_creature == NULL)
        return;
    menu = eduiMenuCreate(220, 70, 240, 250, ed_fnt, AiEditorRespawnCancelMenuPc, "Script Params");
    if (menu == NULL)
        return;
    if (NuLinkedListGetHead(&aieditor->areas) != NULL) {
        if (aieditor->selected_creature->trigger_area != NULL)
            sprintf(text, "Trigger Area \"%s\"", aieditor->selected_creature->trigger_area->name);
        else
            sprintf(text, "Trigger Area NONE");
        eduiMenuAddItem(
            menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, creatureEditor_cbSelectTriggerArea, text));
    }
    if (NuLinkedListGetHead(&aieditor->locators) != NULL) {
        if (aieditor->selected_creature->locator != NULL)
            sprintf(text, "Locator \"%s\"", aieditor->selected_creature->locator->name);
        else
            sprintf(text, "Locator NONE");
        eduiMenuAddItem(menu,
                        eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, AiEditorSelectLocatorMenuPc, text));
    }
    script = AIScriptFind(aieditor->ai_system, aieditor->selected_creature->script_name, 1, 1, 1);
    for (index = 0; index < 4; ++index) {
        if (script != NULL && script->params[index].name != NULL)
            sprintf(text, script->params[index].name);
        else
            sprintf(text, "Param%d", index);
        eduiMenuAddItem(menu,
                        eduiItemSliderCreate(index, &AiEditorSaveMessageColoursPc, 0, creatureEditor_cbSetScriptParam,
                                             0.0f, 100.0f, aieditorsettings.script_params[index], text));
        eduiItemSliderSetGranularity((edui_slider_s *)edui_last_item, 0.1f);
    }
    creatureEditor_ResetParamsItem = NULL;
    if (aieditorsettings.script_flags & 0x1e)
        creatureEditor_ResetParamsItem =
            eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc, 1, 1,
                                                       creatureEditor_cbResetParams, "Reset Params"));
    eduiMenuAttach(parent, menu);
}

void __cdecl creatureEditor_cb_ngroup(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (aieditor->selected_creature != NULL) {
        aieditor->selected_creature->group_count = (u8)((edui_slider_s *)item)->value;
        creatureEditor_Updated(aieditor->selected_creature);
    }
}

void __cdecl creatureEditor_cb_nacross(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (aieditor->selected_creature != NULL) {
        aieditor->selected_creature->across_count = (u8)((edui_slider_s *)item)->value;
        creatureEditor_Updated(aieditor->selected_creature);
    }
}

void __cdecl creatureEditor_cb_xspacing(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (aieditor->selected_creature != NULL) {
        aieditor->selected_creature->x_spacing = ((edui_slider_s *)item)->value;
        creatureEditor_Updated(aieditor->selected_creature);
    }
}

void __cdecl creatureEditor_cb_zspacing(eduimenu_s *menu, eduiitem_s *item, u32 event) {
    (void)menu;
    (void)event;
    if (aieditor->selected_creature != NULL) {
        aieditor->selected_creature->z_spacing = ((edui_slider_s *)item)->value;
        creatureEditor_Updated(aieditor->selected_creature);
    }
}

void __cdecl creatureEditor_cbGroupMenu(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    (void)item;
    (void)event;
    if (aieditor->selected_creature == NULL)
        return;
    menu = eduiMenuCreate(220, 70, 240, 250, ed_fnt, AiEditorRespawnCancelMenuPc, "Group Values");
    if (menu == NULL)
        return;
    eduiMenuAddItem(menu, eduiItemSliderCreateInt(1, &AiEditorSaveMessageColoursPc, 0, creatureEditor_cb_ngroup, 1, 31,
                                                  aieditor->selected_creature->group_count, "Group Size"));
    eduiMenuAddItem(menu,
                    eduiItemSliderCreate(1, &AiEditorSaveMessageColoursPc, 0, creatureEditor_cb_stagger_start, 0.0f,
                                         60.0f, aieditor->selected_creature->stagger_start, "Stagger Time"));
    eduiMenuAddItem(menu, eduiItemSliderCreateInt(1, &AiEditorSaveMessageColoursPc, 0, creatureEditor_cb_nacross, 1, 31,
                                                  aieditor->selected_creature->across_count, "Formation Width"));
    eduiMenuAddItem(menu, eduiItemSliderCreate(1, &AiEditorSaveMessageColoursPc, 0, creatureEditor_cb_xspacing, 0.2f,
                                               4.8f, aieditor->selected_creature->x_spacing, "Formation X Spacing"));
    eduiMenuAddItem(menu, eduiItemSliderCreate(1, &AiEditorSaveMessageColoursPc, 0, creatureEditor_cb_zspacing, 0.2f,
                                               4.8f, aieditor->selected_creature->z_spacing, "Formation Z Spacing"));
    eduiMenuAttach(parent, menu);
}

eduimenu_s *__cdecl creatureEditorDeleteMenu(void) {
    eduimenu_s *menu;
    if (aieditor->selected_creature != NULL && aieditor->selected_creature == aieditor->nearest_creature) {
        menu = eduiMenuCreate(200, 70, 240, 270, ed_fnt, pathEditor_cbCancelDeleteCreatureMenu, "Delete creature??");
        if (menu != NULL) {
            eduiMenuAddItem(
                menu, eduiItemSelCreate(0, &AiEditorSaveMessageColoursPc, 0, 0, creatureEditor_cbDeleteCreature, "No"));
            eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0,
                                                    creatureEditor_cbDeleteCreature, "Yes"));
            return menu;
        }
    }
    return NULL;
}

__declspec(noinline) void __cdecl FollowAPIObject(APIOBJECT_PC *object, APIOBJECT_PC *target, i32 flags,
                                                  f32 movement_parameter) {
    AIPACKET_PC *packet;
    AIPACKET_PC *target_packet;
    AIPATH_PC *path;
    NUVEC *destination;
    i32 movement_mode;
    i32 use_path;
    u8 object_node;
    u8 target_node;
    f32 stopping_distance;
    packet = object->ai;
    target_packet = target->ai;
    if ((flags & 1) != 0) {
        use_path = 0;
        path = packet->path_info.path;
        if (path != NULL && path == target_packet->path_info.path) {
            if (((target_packet->path_info.flags & 1) != 0 ||
                 packet->path_info.connection != target_packet->path_info.connection) &&
                packet->path_info.connection != NULL && target_packet->path_info.connection != NULL &&
                (packet->flags & 0x100000u) == 0) {
                object_node = packet->path_info.connection->node_indices[0];
                target_node = target_packet->path_info.connection->node_indices[0];
                if (path->route_matrix[object_node][target_node] != 0xff || object_node == target_node)
                    use_path = 1;
            }
        }
        if (use_path != 0) {
            destination = &target_packet->last_path_position;
            movement_mode = 1;
        } else {
            destination = &target_packet->terrain_origin;
            movement_mode = 7;
        }
    } else {
        destination = &target_packet->last_path_position;
        movement_mode = 1;
    }
    stopping_distance = (flags & 2) != 0 ? 0.0f : target_packet->mover_height;
    AIMoveInstruction(packet, destination, stopping_distance, &target_packet->path_info, movement_mode,
                      movement_parameter);
}

i32 __cdecl Action_FollowOpponent(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_count, i32 first_time, f32 elapsed) {
    i32 index;
    APIOBJECT_PC *target;

    if (packet == NULL) {
        return 1;
    }

    if (first_time != 0) {
        for (index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                continue;
            }
            if (NuStrICmp(params[index], AIActionIgnoreRadiusName) == 0) {
                processor->action_data_1 |= 2;
            } else if (NuStrICmp(params[index], AIPathCanGoOffPathName) == 0) {
                processor->action_data_1 |= 1;
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[index]);
            }
        }
    }

    target = (APIOBJECT_PC *)(packet->opponent);
    if (target != NULL && target->ai != NULL) {
        FollowAPIObject(((APIOBJECT_PC *)packet->owner), target, processor->action_data_1,
                        packet->movement_instruction_parameter);
    }
    return 0;
}

i32 __cdecl Action_FollowPlayer(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_count, i32 first_time, f32 elapsed) {
    if (packet == NULL) {
        return 1;
    }

    if (first_time != 0) {
        i32 index;
        for (index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                continue;
            }
            if (NuStrICmp(params[index], AIActionIgnoreRadiusName) == 0) {
                processor->action_data_1 |= 2;
            } else if (NuStrICmp(params[index], AIPathCanGoOffPathName) == 0) {
                processor->action_data_1 |= 1;
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[index]);
            }
        }
    }

    if (sys->player_1 != NULL && sys->player_1->ai != NULL) {
        FollowAPIObject(((APIOBJECT_PC *)packet->owner), sys->player_1, processor->action_data_1,
                        packet->movement_instruction_parameter);
    }
    return 0;
}

i32 __cdecl Action_Circle(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                          i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC difference;
    i32 i;
    char *value;
    NUVEC *position;
    AIPATHINFO_PC *path;
    AILOCATOR_PC *locator;
    AICREATURE_PC *creature;

    if (packet == NULL || sys == NULL || sys->player_1 == NULL) {
        return 1;
    }
    if (first_time != 0) {
        for (i = 0; i < param_count; i++) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[i], &packet->goal_speed_mode) != 0) {
                continue;
            }

            if (NuStrICmp(params[i], AIPathAnticlockwiseName) == 0) {
                packet->flags &= ~0x200u;
            } else if (NuStrICmp(params[i], AIPathClockwiseName) == 0) {
                packet->flags |= 0x200u;
            } else if (NuStrICmp(params[i], AIPathReverseName) == 0) {
                packet->flags ^= 0x200u;
            } else if (NuStrICmp(params[i], AIPathFacingName) == 0) {
                processor->action_data_1 |= 1;
            } else if (NuStrICmp(params[i], AIPathCurrentDistanceName) == 0) {
                processor->action_data_1 |= 4;
            } else if ((value = NuStrIStr(params[i], (char *)AIPathLocatorName)) != NULL) {
                processor->action_data_3 = AIPathFindLocator(sys, value + 8);
                if (processor->action_data_3 != NULL) {
                    processor->action_data_1 |= 0x10;
                }
            } else if (NuStrIStr(params[i], (char *)AIPathCurrentPositionName) != NULL) {
                processor->action_data_1 |= 8;
                processor->action_pos = packet->terrain_origin;
            } else if (NuStrICmp(params[i], AIPathOriginName) == 0) {
                if (packet->creature_index != 0xff) {
                    processor->action_data_1 |= 0x20;
                }
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[i]);
            }
        }
        if ((processor->action_data_1 & 0x38) == 0) {
            processor->action_data_1 |= 8;
            processor->action_pos = packet->terrain_origin;
        }
    }
    position = NULL;
    path = NULL;
    if ((processor->action_data_1 & 8) != 0) {
        position = &processor->action_pos;
        path = &packet->path_info;
    } else if ((processor->action_data_1 & 0x10) != 0) {
        locator = (AILOCATOR_PC *)(processor->action_data_3);
        position = &locator->position;
        path = &locator->path_info;
    } else if ((processor->action_data_1 & 0x20) != 0) {
        creature = &sys->creatures[packet->creature_index];
        path = &creature->path_info;
        position = GetOriginPositionFn != NULL ? GetOriginPositionFn(sys, packet) : NULL;
        if (position == NULL) {
            position = &creature->position;
        }
    }
    if (position != NULL && path != NULL) {
        if ((processor->action_data_1 & 4) != 0) {
            packet->movement_instruction_parameter = NuVecXZDist(&packet->terrain_origin, position, &difference);
        }
        AIMoveInstruction(packet, position, 0.0f, path, 3, packet->movement_instruction_parameter);
        if ((processor->action_data_1 & 1) != 0) {
            packet->movement_look_target = position;
        }
    }
    return 0;
}

i32 __cdecl Action_CirclePlayer(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC difference;
    i32 i;
    AIPACKET_PC *target;
    NUVEC *position;

    if (packet == NULL || sys == NULL || sys->player_1 == NULL) {
        return 1;
    }
    if (first_time != 0) {
        for (i = 0; i < param_count; i++) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[i], &packet->goal_speed_mode) != 0) {
                continue;
            }
            if (NuStrICmp(params[i], AIPathAnticlockwiseName) == 0) {
                packet->flags &= ~0x200u;
            } else if (NuStrICmp(params[i], AIPathClockwiseName) == 0) {
                packet->flags |= 0x200u;
            } else if (NuStrICmp(params[i], AIPathReverseName) == 0) {
                packet->flags ^= 0x200u;
            } else if (NuStrICmp(params[i], AIPathFacingName) == 0) {
                processor->action_data_1 |= 1;
            } else if (NuStrICmp(params[i], AIPathCanGoOffPathName) == 0) {
                processor->action_data_1 |= 2;
            } else if (NuStrICmp(params[i], AIPathCurrentDistanceName) == 0) {
                processor->action_data_1 |= 4;
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[i]);
            }
        }
    }
    if (sys->player_1 != NULL) {
        if ((processor->action_data_1 & 4) != 0) {
            packet->movement_instruction_parameter =
                NuVecXZDist(&packet->terrain_origin, &sys->player_1->ai->last_path_position, &difference);
        }
        target = sys->player_1->ai;
        position = (processor->action_data_1 & 2) != 0 ? &target->terrain_origin : &target->last_path_position;
        AIMoveInstruction(packet, position, target->mover_height, &target->path_info, 3,
                          packet->movement_instruction_parameter);
        if ((processor->action_data_1 & 1) != 0) {
            packet->movement_look_target = position;
        }
    }
    return 0;
}

i32 __cdecl Action_CircleOpponent(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC difference;
    i32 i;
    AIPACKET_PC *target;
    NUVEC *position;

    if (packet == NULL) {
        return 1;
    }
    if (first_time != 0) {
        for (i = 0; i < param_count; i++) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[i], &packet->goal_speed_mode) != 0) {
                continue;
            }
            if (NuStrICmp(params[i], AIPathAnticlockwiseName) == 0) {
                packet->flags &= ~0x200u;
            } else if (NuStrICmp(params[i], AIPathClockwiseName) == 0) {
                packet->flags |= 0x200u;
            } else if (NuStrICmp(params[i], AIPathReverseName) == 0) {
                packet->flags ^= 0x200u;
            } else if (NuStrICmp(params[i], AIPathRandomName) == 0) {
                if (NuRandFloat() < 0.5f) {
                    packet->flags &= ~0x200u;
                } else {
                    packet->flags |= 0x200u;
                }
            } else if (NuStrICmp(params[i], AIPathFacingName) == 0) {
                processor->action_data_1 |= 1;
            } else if (NuStrICmp(params[i], AIPathCanGoOffPathName) == 0) {
                processor->action_data_1 |= 2;
            } else if (NuStrICmp(params[i], AIPathCurrentDistanceName) == 0) {
                processor->action_data_1 |= 4;
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[i]);
            }
        }
    }
    if (((APIOBJECT_PC *)packet->opponent) != NULL && ((APIOBJECT_PC *)packet->opponent)->ai != NULL) {
        if ((processor->action_data_1 & 4) != 0) {
            packet->movement_instruction_parameter = NuVecXZDist(
                &packet->terrain_origin, &((APIOBJECT_PC *)packet->opponent)->ai->last_path_position, &difference);
        }
        target = ((APIOBJECT_PC *)packet->opponent)->ai;
        position = (processor->action_data_1 & 2) != 0 ? &target->terrain_origin : &target->last_path_position;
        AIMoveInstruction(packet, position, target->mover_height, &target->path_info, 3,
                          packet->movement_instruction_parameter);
        if ((processor->action_data_1 & 1) != 0) {
            packet->movement_look_target = position;
        }
    }
    return 0;
}

i32 __cdecl Action_MoveAwayFromOpponent(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                        char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 i;
    AIPACKET_PC *target;

    if (packet == NULL) {
        return 1;
    }
    if (first_time != 0) {
        for (i = 0; i < param_count; i++) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[i], &packet->goal_speed_mode) != 0) {
                continue;
            }
            if (NuStrICmp(params[i], AIPathFaceName) == 0) {
                processor->action_data_1 = 1;
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[i]);
            }
        }
    }
    if (((APIOBJECT_PC *)packet->opponent) != NULL && ((APIOBJECT_PC *)packet->opponent)->ai != NULL) {
        target = ((APIOBJECT_PC *)packet->opponent)->ai;
        AIMoveInstruction(packet, &target->last_path_position, target->mover_height, &target->path_info, 2,
                          packet->movement_instruction_parameter);
        if (processor->action_data_1 != 0) {
            packet->movement_look_target = &((APIOBJECT_PC *)packet->opponent)->position;
        }
    }
    return 0;
}

i32 __cdecl Action_MoveAwayFromPlayer(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                      i32 param_count, i32 first_time, f32 elapsed) {
    i32 i;
    AIPACKET_PC *target;

    if (packet == NULL) {
        return 1;
    }
    if (first_time != 0) {
        for (i = 0; i < param_count; i++) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[i], &packet->goal_speed_mode) != 0) {
                continue;
            }
            if (NuStrICmp(params[i], AIPathFaceName) == 0) {
                processor->action_data_1 = 1;
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[i]);
            }
        }
    }
    if (sys->player_1 != NULL) {
        target = sys->player_1->ai;
        AIMoveInstruction(packet, &target->last_path_position, target->mover_height, &target->path_info, 2,
                          packet->movement_instruction_parameter);
        if (processor->action_data_1 != 0) {
            packet->movement_look_target = &sys->player_1->position;
        }
    }
    return 0;
}

i32 __cdecl Action_MoveAwayFromPlayer2(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                       i32 param_count, i32 first_time, f32 elapsed) {
    i32 i;
    AIPACKET_PC *target;

    if (packet == NULL) {
        return 1;
    }
    if (first_time != 0) {
        for (i = 0; i < param_count; i++) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[i], &packet->goal_speed_mode) != 0) {
                continue;
            }
            if (NuStrICmp(params[i], AIPathFaceName) == 0) {
                processor->action_data_1 = 1;
            } else {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[i]);
            }
        }
    }
    if (sys->player_2 != NULL) {
        target = sys->player_2->ai;
        AIMoveInstruction(packet, &target->last_path_position, target->mover_height, &target->path_info, 2,
                          packet->movement_instruction_parameter);
        if (processor->action_data_1 != 0) {
            packet->movement_look_target = &sys->player_2->position;
        }
    }
    return 0;
}

i32 __cdecl Action_RetreatFromOpponent(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                       char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 i;
    AIPACKET_PC *target;

    if (packet == NULL) {
        return 1;
    }
    if (first_time != 0) {
        packet->movement_instruction_parameter = 1.0f;
        for (i = 0; i < param_count; i++) {
            if (AIActionParseSpeedFn == NULL || AIActionParseSpeedFn(params[i], &packet->goal_speed_mode) == 0) {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[i]);
            }
        }
    }
    if (packet->opponent != NULL && ((APIOBJECT_PC *)packet->opponent)->ai != NULL) {
        target = ((APIOBJECT_PC *)packet->opponent)->ai;
        AIMoveInstruction(packet, &target->last_path_position, target->mover_height, &target->path_info, 2,
                          packet->movement_instruction_parameter);
    }
    return 0;
}

i32 __cdecl Action_RetreatFromNearestOpponent(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet,
                                              char **params, i32 param_count, i32 first_time, f32 elapsed) {
    i32 i;
    AIPACKET_PC *target;

    if (packet == NULL) {
        return 1;
    }
    if (first_time != 0) {
        packet->movement_instruction_parameter = 1.0f;
        for (i = 0; i < param_count; i++) {
            if (AIActionParseSpeedFn == NULL || AIActionParseSpeedFn(params[i], &packet->goal_speed_mode) == 0) {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[i]);
            }
        }
    }
    if (packet->nearest_opponent != NULL) {
        target = ((APIOBJECT_PC *)packet->nearest_opponent)->ai;
        AIMoveInstruction(packet, &target->last_path_position, target->mover_height, &target->path_info, 2,
                          packet->movement_instruction_parameter);
    }
    return 0;
}

i32 __cdecl Action_GoToNode(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                            i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC difference;
    i32 index;
    AIPATHNODE_PC *node;
    i32 node_index;
    AIPATHNODE_PC *node_2;
    f32 distance_squared;

    (void)elapsed;

    if (packet == NULL || packet->owner == NULL || packet->path_info.path == NULL ||
        packet->path_info.connection == NULL) {
        return 1;
    }

    if (first_time != 0) {
        if (param_count == 0) {
            return 0;
        }
        for (index = 1; index < param_count; ++index) {
            if (AIActionParseSpeedFn == NULL || AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) == 0) {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[index]);
            }
        }
        processor->action_data_3 = AIPathFindNode(sys, packet->path_info.path, params[0]);
        node = (AIPATHNODE_PC *)(processor->action_data_3);
        if (node != NULL && node->connection_count != 0) {
            node_index = node - packet->path_info.path->nodes;
            processor->path_info.path = packet->path_info.path;
            processor->path_info.connection = node->connections[0];
            processor->path_info.distance =
                node_index == processor->path_info.connection->node_indices[0] ? 0.0f : 1.0f;
            processor->path_info.direction = 0;
            processor->path_info.flags_word |= 1;
            processor->path_info.width = 0.0f;
            AIMoveInstruction(packet, &node->position, 0.0f, &processor->path_info, 1,
                              packet->movement_instruction_parameter);
        } else {
            return 1;
        }
        return 0;
    }

    node_2 = (AIPATHNODE_PC *)(processor->action_data_3);
    if (node_2 == NULL) {
        return 1;
    }
    distance_squared = NuVecXZDistSqr(&packet->terrain_origin, &node_2->position, &difference);
    AIMoveInstruction(packet, &node_2->position, 0.0f, &processor->path_info, 1,
                      packet->movement_instruction_parameter);
    return distance_squared < node_2->radius_squared;
}

i32 __cdecl Action_GoToNodeRandom(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                  i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC difference;
    i32 selected;
    AIPATHNODE_PC *node;
    i32 node_index;
    AIPATHNODE_PC *node_2;
    f32 distance_squared;

    (void)elapsed;

    if (packet == NULL || packet->owner == NULL || packet->path_info.path == NULL ||
        packet->path_info.connection == NULL) {
        return 1;
    }
    if (first_time != 0) {
        if (param_count == 0) {
            return 0;
        }
        selected = (i32)(NuRandFloat() * param_count);
        if (selected >= param_count) {
            selected = param_count - 1;
        }
        processor->action_data_3 = AIPathFindNode(sys, packet->path_info.path, params[selected]);
        node = (AIPATHNODE_PC *)(processor->action_data_3);
        if (node != NULL && node->connection_count != 0) {
            node_index = node - packet->path_info.path->nodes;
            processor->path_info.path = packet->path_info.path;
            processor->path_info.connection = node->connections[0];
            processor->path_info.distance =
                node_index == processor->path_info.connection->node_indices[0] ? 0.0f : 1.0f;
            processor->path_info.direction = 0;
            processor->path_info.flags_word |= 1;
            processor->path_info.width = 0.0f;
            AIMoveInstruction(packet, &node->position, 0.0f, &processor->path_info, 1,
                              packet->movement_instruction_parameter);
        } else {
            return 1;
        }
        return 0;
    }
    node_2 = (AIPATHNODE_PC *)(processor->action_data_3);
    if (node_2 == NULL) {
        return 1;
    }
    distance_squared = NuVecXZDistSqr(&packet->terrain_origin, &node_2->position, &difference);
    AIMoveInstruction(packet, &node_2->position, 0.0f, &processor->path_info, 1,
                      packet->movement_instruction_parameter);
    return distance_squared < node_2->radius_squared;
}

i32 __cdecl Action_GoToOrigin(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC difference;
    AICREATURE_PC *creature;
    NUVEC *origin;
    f32 min_time = 0.0f;
    f32 max_time = 0.0f;
    i32 index;
    char *value;
    f32 distance_squared;
    f32 range;
    f32 remaining_time;

    if (packet == NULL || packet->owner == NULL || packet->path_info.path == NULL ||
        packet->path_info.connection == NULL || (packet->owner->flags_1f8 & 0x400) == 0 ||
        packet->creature_index == 0xff) {
        return 1;
    }
    creature = &sys->creatures[packet->creature_index];
    origin = GetOriginPositionFn != NULL ? GetOriginPositionFn(sys, packet) : NULL;
    if (origin == NULL) {
        origin = &creature->position;
    }

    if (first_time != 0) {
        packet->movement_instruction_parameter = 0.2f;
        if (param_count != 0) {
            for (index = 0; index < param_count; ++index) {
                if (AIActionParseSpeedFn != NULL &&
                    AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                    continue;
                }
                value = NuStrIStr(params[index], (char *)AIPathWaitTimeName);
                if (value != NULL) {
                    processor->action_timer =
                        AIParamToFloatEx(packet, processor, value + NuStrLen(AIPathWaitTimeName) + 1);
                } else if ((value = NuStrIStr(params[index], (char *)AIActionMinTimeName)) != NULL) {
                    min_time = AIParamToFloatEx(packet, processor, value + NuStrLen(AIActionMinTimeName) + 1);
                } else if ((value = NuStrIStr(params[index], (char *)AIActionMaxTimeName)) != NULL) {
                    max_time = AIParamToFloatEx(packet, processor, value + NuStrLen(AIActionMaxTimeName) + 1);
                } else if (NuStrICmp(params[index], AIPathXZRangeCheckName) == 0) {
                    processor->action_data_2 = 1;
                } else if ((value = NuStrIStr(params[index], (char *)AIPathGoalRangeName)) != NULL) {
                    packet->movement_instruction_parameter =
                        AIParamToFloatEx(packet, processor, value + NuStrLen(AIPathGoalRangeName) + 1);
                } else {
                    packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[index]);
                }
            }
        }
        if (processor->action_timer == 0.0f) {
            if (min_time < max_time) {
                processor->action_timer = NuRandFloat() * (max_time - min_time) + min_time;
            } else {
                processor->action_timer = 0.01f;
            }
        }
        AIMoveInstruction(packet, origin, 0.0f, &creature->path_info, 1, packet->movement_instruction_parameter);
        processor->action_pos.x = 0.0f;
        processor->action_pos.y = 0.0f;
        processor->action_pos.z = 1.0f;
        NuVecRotateY(&processor->action_pos, &processor->action_pos, creature->y_rotation);
        NuVecAdd(&processor->action_pos, &processor->action_pos, origin);
        return 0;
    }

    AIMoveInstruction(packet, origin, 0.0f, &creature->path_info, 1, packet->movement_instruction_parameter);
    distance_squared = processor->action_data_2 != 0 ? NuVecXZDistSqr(&packet->terrain_origin, origin, &difference)
                                                     : NuVecDistSqr(&packet->terrain_origin, origin, &difference);
    range = packet->movement_instruction_parameter + ai_moveradius;
    range = elapsed * packet->owner->horizontal_velocity_magnitude + range;
    if (distance_squared < range * range) {
        remaining_time = processor->action_timer;
        packet->movement_look_target = &processor->action_pos;
        if (!(remaining_time > 0.0f)) {
            return 1;
        }
        remaining_time -= elapsed;
        processor->action_timer = remaining_time;
        if (remaining_time < 0.0f) {
            processor->action_timer = 0.0f;
        }
    }
    return 0;
}

i32 __cdecl Action_GoToLocator(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                               i32 param_count, i32 is_first_time, f32 elapsed) {
    AILOCATOR_PC *locator;
    NUVEC distance_vector;
    f32 distance_squared;
    f32 reach_distance;
    i32 random_locator_count = 0;
    i32 use_personal_name = 0;
    i32 use_indexed_name = 0;
    f32 minimum_time = 0.0f;
    f32 maximum_time = 0.0f;
    char locator_name[64];
    i32 index;
    AILOCATOR_PC *locator_2;
    char *value;
    AILOCATOR_PC *locator_3;

    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL ||
        packet->path_info.path == NULL || packet->path_info.connection == NULL) {
        return 1;
    }

    if (is_first_time != 0) {
        processor->action_data_1 = 0;
        processor->action_data_3 = processor->locator;

        if (param_count != 0) {
            for (index = 0; index < param_count; ++index) {
                if (AIActionParseSpeedFn != NULL &&
                    AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                    continue;
                }

                if (NuStrIStr(params[index], "name") != NULL || NuStrIStr(params[index], "teleport") != NULL) {
                    ++index;
                    if (index < param_count) {
                        if (use_indexed_name != 0 && ((APIOBJECT_PC *)packet->owner)->locator_name_index != -1) {
                            sprintf(locator_name, AILocatorIndexedFormat, params[index],
                                    ((APIOBJECT_PC *)packet->owner)->locator_name_index);
                        } else if (use_personal_name != 0 && ((APIOBJECT_PC *)packet->owner)->character_data != NULL) {
                            sprintf(locator_name, AILocatorPersonalFormat, params[index],
                                    ((APIOBJECT_PC *)packet->owner)->character_data->name);
                        } else if (random_locator_count != 0) {
                            sprintf(locator_name, AILocatorIndexedFormat, params[index],
                                    NuRand(NULL) % random_locator_count);
                        } else {
                            sprintf(locator_name, params[index]);
                        }

                        processor->action_data_3 = AIPathFindLocator(sys, locator_name);
                    }
                    if (NuStrIStr(params[index - 1], "teleport") != NULL) {
                        locator_2 = (AILOCATOR_PC *)(processor->action_data_3);
                        if (locator_2 == NULL) {
                            return 1;
                        }
                        packet->owner->position = locator_2->position;
                        return 1;
                    }
                    continue;
                }

                if (NuStrIStr(params[index], (char *)AIActionPersonalName) != NULL) {
                    use_personal_name = 1;
                    continue;
                }
                if (NuStrIStr(params[index], (char *)AIActionIndexedName) != NULL) {
                    use_indexed_name = 1;
                    continue;
                }

                value = NuStrIStr(params[index], "random");
                if (value != NULL) {
                    random_locator_count = AIParamToFloatEx(packet, processor, value + NuStrLen("random") + 1);
                    continue;
                }
                value = NuStrIStr(params[index], (char *)AIPathWaitTimeName);
                if (value != NULL) {
                    processor->action_timer =
                        AIParamToFloatEx(packet, processor, value + NuStrLen(AIPathWaitTimeName) + 1);
                    continue;
                }
                value = NuStrIStr(params[index], (char *)AIActionMinTimeName);
                if (value != NULL) {
                    minimum_time = AIParamToFloatEx(packet, processor, value + NuStrLen(AIActionMinTimeName) + 1);
                    continue;
                }
                value = NuStrIStr(params[index], (char *)AIActionMaxTimeName);
                if (value != NULL) {
                    maximum_time = AIParamToFloatEx(packet, processor, value + NuStrLen(AIActionMaxTimeName) + 1);
                    continue;
                }
                value = NuStrIStr(params[index], (char *)AIPathGoalRangeName);
                if (value != NULL) {
                    packet->movement_instruction_parameter =
                        AIParamToFloatEx(packet, processor, value + NuStrLen(AIPathGoalRangeName) + 1);
                    continue;
                }

                if (NuStrICmp(params[index], "on_ground") == 0) {
                    processor->action_data_1 |= 1u;
                } else if (NuStrICmp(params[index], AIPathXZRangeCheckName) == 0) {
                    processor->action_data_1 |= 2u;
                } else if (NuStrICmp(params[index], AIPathFaceOpponentName) == 0) {
                    processor->action_data_1 |= 4u;
                } else if (NuStrICmp(params[index], AIPathIgnorePathName) == 0) {
                    processor->action_data_1 |= 8u;
                } else if (NuStrICmp(params[index], AIPathMustReachDestinationName) == 0) {
                    processor->action_data_1 |= 16u;
                } else {
                    packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[index]);
                }
            }
        }

        if (processor->action_timer == 0.0f) {
            if (maximum_time > minimum_time) {
                processor->action_timer = NuRandFloat() * (maximum_time - minimum_time) + minimum_time;
            } else {
                processor->action_timer = 0.01f;
            }
        }

        locator_3 = (AILOCATOR_PC *)(processor->action_data_3);
        if (locator_3 == NULL) {
            return 1;
        }

        if ((processor->action_data_1 & 8u) != 0) {
            AIMoveInstruction(packet, &locator_3->position, 0.0f, &locator_3->path_info, 7,
                              packet->movement_instruction_parameter);
        } else {
            AIMoveInstruction(packet, &locator_3->position, 0.0f, &locator_3->path_info, 1,
                              packet->movement_instruction_parameter);
        }

        processor->action_pos.x = 0.0f;
        processor->action_pos.y = 0.0f;
        processor->action_pos.z = 1.0f;
        NuVecRotateY(&processor->action_pos, &processor->action_pos, locator_3->rotation);
        NuVecAdd(&processor->action_pos, &processor->action_pos, &locator_3->position);
        return 0;
    }
    locator = (AILOCATOR_PC *)(processor->action_data_3);
    if (locator == NULL) {
        return 1;
    }

    if ((processor->action_data_1 & 8u) != 0) {
        AIMoveInstruction(packet, &locator->position, 0.0f, &locator->path_info, 7,
                          packet->movement_instruction_parameter);
    } else {
        AIMoveInstruction(packet, &locator->position, 0.0f, &locator->path_info, 1,
                          packet->movement_instruction_parameter);
    }

    if ((processor->action_data_1 & 4u) != 0 && packet->opponent != NULL) {
        packet->movement_look_target = &packet->opponent->position;
    }
    if ((processor->action_data_1 & 1u) != 0 && ((APIOBJECT_PC *)packet->owner)->grounded == 0) {
        return 0;
    }

    distance_squared = (processor->action_data_1 & 2u) != 0
                           ? NuVecXZDistSqr(&packet->terrain_origin, &locator->position, &distance_vector)
                           : NuVecDistSqr(&packet->terrain_origin, &locator->position, &distance_vector);
    reach_distance = packet->movement_instruction_parameter + ai_moveradius;
    reach_distance = elapsed * packet->owner->horizontal_velocity_magnitude + reach_distance;
    if (reach_distance * reach_distance > distance_squared) {
        if ((processor->action_data_1 & 4u) == 0 || packet->opponent == NULL) {
            packet->movement_look_target = &processor->action_pos;
        }
        if (!(processor->action_timer > 0.0f)) {
            return 1;
        }
        processor->action_timer -= elapsed;
        if (processor->action_timer < 0.0f) {
            processor->action_timer = 0.0f;
        }
        return 0;
    } else {
        if ((packet->flags & 0x400000u) != 0 && (processor->action_data_1 & 16u) != 0 &&
            AIBigJumpToDestinationFn != NULL) {
            if (AIBigJumpToDestinationFn(((APIOBJECT_PC *)packet->owner), &locator->position) != 0) {
                return 1;
            }
        }
        return 0;
    }
}

i32 __cdecl Action_FollowPath(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                              i32 param_count, i32 is_first_time, f32 elapsed) {
    f32 completion_time;
    f32 minimum_time;
    f32 maximum_time;
    i32 index;
    char *value;
    f64 first_term;
    f32 random;

    (void)sys;

    if (packet == NULL || packet->owner == NULL || ((APIOBJECT_PC *)packet->owner)->owner == NULL) {
        return 1;
    }
    if (packet->path_info.path == NULL || packet->path_info.connection == NULL) {
        return 0;
    }

    completion_time = 0.0f;
    if (is_first_time != 0) {
        packet->movement_target = NULL;

        minimum_time = 0.0f;
        maximum_time = 0.0f;
        for (index = 0; index < param_count; ++index) {
            if (AIActionParseSpeedFn != NULL && AIActionParseSpeedFn(params[index], &packet->goal_speed_mode) != 0) {
                continue;
            }

            value = NuStrIStr(params[index], (char *)AIActionMinTimeName);
            if (value != NULL) {
                minimum_time = AIParamToFloatEx(packet, processor, value + 8);
                continue;
            }

            value = NuStrIStr(params[index], (char *)AIActionMaxTimeName);
            if (value != NULL) {
                maximum_time = AIParamToFloatEx(packet, processor, value + 8);
                continue;
            }

            processor->action_timer = AIParamToFloatEx(packet, processor, params[index]);
        }

        if (maximum_time > minimum_time) {
            first_term = (1.0 - NuRandFloat()) * minimum_time;
            random = NuRandFloat();
            processor->action_timer = (f32)(random * maximum_time + first_term);
        }
    }

    AIMoveInstruction(packet, NULL, 0.0f, NULL, 4, packet->movement_instruction_parameter);

    if (!(processor->action_timer > completion_time)) {
        return 0;
    }

    processor->action_timer -= elapsed;
    return completion_time >= processor->action_timer;
}

i32 __cdecl Action_MoveAwayFromNode(AISYS_PC *sys, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                                    i32 param_count, i32 first_time, f32 elapsed) {
    NUVEC difference;
    i32 i;
    AIPATHNODE_PC *node;
    i32 index;
    AIPATHNODE_PC *node_2;
    f32 distance;

    if (packet == NULL || packet->owner == NULL || packet->path_info.path == NULL ||
        packet->path_info.connection == NULL) {
        return 1;
    }
    if (first_time != 0) {
        if (param_count == 0) {
            return 0;
        }
        for (i = 1; i < param_count; i++) {
            if (AIActionParseSpeedFn == NULL || AIActionParseSpeedFn(params[i], &packet->goal_speed_mode) == 0) {
                packet->movement_instruction_parameter = AIParamToFloatEx(packet, processor, params[i]);
            }
        }
        node = AIPathFindNode(sys, packet->path_info.path, params[0]);
        processor->action_data_3 = node;
        if (node == NULL || node->connection_count == 0) {
            return 1;
        }
        index = node - packet->path_info.path->nodes;
        AISysCharacterSetPath(packet, packet->path_info.path);
        processor->path_info.connection = node->connections[0];
        processor->path_info.distance = index == processor->path_info.connection->node_indices[0] ? 0.0f : 1.0f;
        processor->path_info.width = 0.0f;
        processor->path_info.direction = 0;
        processor->path_info.flags_word |= 1;
        AIMoveInstruction(packet, &node->position, 0.0f, &processor->path_info, 2,
                          packet->movement_instruction_parameter);
        return 0;
    }
    node_2 = (AIPATHNODE_PC *)(processor->action_data_3);
    if (node_2 != NULL) {
        distance = NuVecXZDistSqr(&packet->terrain_origin, &node_2->position, &difference);
        if (!(node_2->radius_squared > distance)) {
            AIMoveInstruction(packet, &node_2->position, 0.0f, &processor->path_info, 2,
                              packet->movement_instruction_parameter);
            return 0;
        }
    }
    return 1;
}

void __cdecl AISysUpdateCharacterPathPos(AISYS_PC *system, APIOBJECT_PC *object, AIPACKET_PC *packet, i32 ground,
                                         f32 elapsed) {
    i32 should_find_path =
        (packet->flags & 0x02000000) == 0 &&
        (((object->flags_low & 4) == 0 || (packet->path_info.flags & 1) == 0) || object->supporting_platform_id != -1);
    if (should_find_path) {
        if (packet->path_info.path != NULL) {
            if (object->flags_low & 0x80000)
                AISysGetCharacterPathPos(system, object, packet, 0, ground);
            else
                AISysGetCharacterPathPos(system, object, packet, AIPathDefaultChecks, ground);
        } else {
            packet->last_path_position = object->position;
        }
        packet->flags &= ~0x02000000u;
        if ((packet->path_info.flags & 1) == 0) {
            packet->time_off_path += elapsed;
        } else {
            packet->time_off_path = 0.0f;
        }
    } else {
        AIPATH_PC *path = packet->path_info.path;
        if (path != NULL && packet->path_info.connection != NULL) {
            u8 node_index = packet->path_info.connection->node_indices[0];
            AIPATHNODE_PC *first = &path->nodes[node_index];
            AIPATHNODE_PC *second = &path->nodes[packet->path_info.connection->node_indices[1]];
            if (first->has_special != 0 && (path->updated_node_bits[node_index / 8] & (1u << (node_index % 8))) == 0) {
                AIPathNodeUpdatePos(system, path, first);
            }
            if (second->has_special != 0) {
                node_index = packet->path_info.connection->node_indices[1];
                path = packet->path_info.path;
                if ((path->updated_node_bits[node_index / 8] & (1u << (node_index % 8))) == 0) {
                    AIPathNodeUpdatePos(system, path, second);
                }
            }
        }
        path = packet->path_info.path;
        if (packet->inside_path_node != -1 && path != NULL) {
            i16 node_index = packet->inside_path_node;
            path->inside_node_bits[node_index / 8] |= (u8)(1u << (node_index % 8));
        }
        packet->flags &= ~0x02000000u;
        packet->time_off_path = 0.0f;
    }
}

void __cdecl AISysProcessCharacter(AISYS_PC *system, APIOBJECT_PC *object, AIPACKET_PC *packet, i32 ground, f32 elapsed,
                                   i32 use_three_dimensions, i32 process_ai) {
    AISCRIPTPROCESS_PC *primary = &packet->script_processor;
    AISCRIPTPROCESS_PC *processor = packet->alternate_processor;
    f32 clearance;
    NUVEC delta;
    f32 distance;
    f32 stopping_clearance;
    f32 scale;
    if (processor == NULL) {
        processor = primary;
    }
    if ((packet->route_flags & 1) == 0) {
        AISTATE_PC *saved_state = primary->state;
        AISTATE_PC *initial_state =
            primary->script == NULL ? NULL : AIStateFind(AICharacterInitialiseName, primary->script);
        if (initial_state != NULL) {
            AIScriptSetState(primary, initial_state);
            AIScriptProcess(system, object, packet, primary, 0.0f);
            AIScriptSetState(primary, saved_state);
        }
        packet->flags |= 0x00010000u;
    }
    if (process_ai != 0) {
        AISysUpdateCharacterPathPos(system, object, packet, ground, elapsed);
        packet->flags &= 0xfff7fbf7u;
        packet->movement_look_target = NULL;
        if ((object->flags_low & 0x80u) != 0 && (object->flags_low & 0x100u) == 0) {
            packet->path_update_flag = 0;
            packet->path_target = NULL;
            packet->movement_destination = object->position;
            packet->flags &= 0x3f0fffffu;
            packet->navigation_flags &= ~1u;
        } else {
            u32 source;
            if (AIRespawnOnPathFn != NULL && AIRespawnOnPathFn(object) != 0) {
                return;
            }
            NuSpecialClear(&object->collision_special);
            object->flags_low &= ~0x00040000u;
            if (system != NULL && processor->script != NULL) {
                AIScriptProcess(system, object, packet, processor, elapsed);
            }
            packet->flags &= 0x3f0fffffu;
            packet->navigation_flags &= ~1u;
            packet->frame_state = 0;
            if ((packet->flags & 0x10000000u) != 0) {
                switch ((packet->flags >> 26) & 3) {
                    case 1:
                        if (packet->creature_index != 0xff) {
                            AICREATURE_PC *creature = &system->creatures[packet->creature_index];
                            AIMoveInstruction(packet, &creature->position, 0.0f, &creature->path_info, 1, 0.0f);
                        }
                        break;
                    case 2:
                        if (packet->locator != NULL) {
                            AIMoveInstruction(packet, &packet->locator->position, 0.0f, &packet->locator->path_info, 1,
                                              0.0f);
                        }
                        break;
                    default:
                        packet->flags &= ~0x10000000u;
                        break;
                }
            }
            AISysCharacterMovement(system, packet, object, ground);
            source = (packet->flags >> 26) & 3;
            if (source != 0 && packet->path_target == NULL && packet->movement_target_radius > 0.0f &&
                (object->motion_flags & 0x400u) != 0) {
                NUVEC *source_position = NULL;
                switch (source) {
                    case 1:
                        if (packet->creature_index != 0xff) {
                            source_position = &system->creatures[packet->creature_index].position;
                        }
                        break;
                    case 2:
                        if (packet->locator != NULL) {
                            source_position = &packet->locator->position;
                        }
                        break;
                }
                if (source_position != NULL) {
                    f32 distance_squared = NuVecXZDistSqr(source_position, &object->position, &delta);
                    f32 radius = packet->movement_target_radius;
                    if ((packet->flags & 0x10000000u) != 0) {
                        if (distance_squared < radius * radius) {
                            packet->flags &= ~0x10000000u;
                        }
                    } else if (distance_squared > radius * radius) {
                        if (distance_squared >
                            (packet->movement_target_radius + 0.5) * (packet->movement_target_radius + 0.5)) {
                            packet->flags |= 0x10000000u;
                        } else {
                            NUVEC movement_delta;
                            movement_delta.x = packet->movement_destination.x - object->position.x;
                            movement_delta.y = 0.0f;
                            movement_delta.z = packet->movement_destination.z - object->position.z;
                            if (NuVecDot(&movement_delta, &delta) < 0.0f) {
                                packet->movement_destination = object->position;
                                packet->movement_stopping_distance = 0.0f;
                            }
                        }
                    }
                }
            }
        }
    }
    clearance = packet->movement_stopping_distance != 0.0f ? packet->mover_height : 0.0f;
    if (use_three_dimensions != 0) {
        distance = NuVecDist(&packet->movement_destination, &object->position, &delta);
        if (distance > packet->mover_height + clearance + packet->movement_stopping_distance) {
            scale = NuFdiv(packet->mover_height, distance);
            NuVecScale(&delta, &delta, scale);
            NuVecAdd(&packet->movement_position, &object->position, &delta);
        } else {
            stopping_clearance = clearance + packet->movement_stopping_distance;
            scale = NuFdiv(stopping_clearance, distance);
            NuVecScale(&delta, &delta, scale);
            NuVecSub(&packet->movement_position, &packet->movement_destination, &delta);
        }
    } else {
        distance = NuVecXZDist(&packet->movement_destination, &object->position, &delta);
        if (distance > packet->mover_height + clearance + packet->movement_stopping_distance) {
            scale = NuFdiv(packet->mover_height, distance);
            NuVecScale(&delta, &delta, scale);
            NuVecAdd(&packet->movement_position, &object->position, &delta);
        } else {
            stopping_clearance = clearance + packet->movement_stopping_distance;
            scale = NuFdiv(stopping_clearance, distance);
            NuVecScale(&delta, &delta, scale);
            NuVecSub(&packet->movement_position, &packet->movement_destination, &delta);
        }
    }
}

void __cdecl AISYSRebuildFromEditorData(void) {
    AISYS_PC *system;
    EDAIAREA_s *area;
    EDANTINODE_s *node;
    i32 index;
    i32 storage_size;
    if (aieditor->ai_system == NULL)
        return;
    storage_size = aieditor->ai_system->storage_size;
    memset(aieditor->ai_system->storage, 0, storage_size);
    aieditor->ai_system->storage_cursor.void_ptr = aieditor->ai_system->storage;
    aieditor->ai_system->storage_size = storage_size;
    aieditor->ai_system->path_sys =
        pathEditorCreateData(&aieditor->ai_system->storage_cursor, &aieditor->ai_system->storage_end,
                             &aieditorsettings.path_scratch_cursor, &aieditorsettings.path_scratch_end);
    memset(aieditor->ai_system->groups, 0, sizeof(aieditor->ai_system->groups));
    area = (EDAIAREA_s *)NuLinkedListGetHead(&aieditor->areas);
    aieditor->ai_system->area_count = 0;
    for (; area != NULL; area = (EDAIAREA_s *)NuLinkedListGetNext(&aieditor->areas, &area->link))
        ++aieditor->ai_system->area_count;
    if (aieditor->ai_system->area_count != 0) {
        aieditor->ai_system->areas =
            (AIAREA_PC *)AISysBufferAlloc(&aieditor->ai_system->storage_cursor, &aieditor->ai_system->storage_end,
                                          aieditor->ai_system->area_count * sizeof(AIAREA_PC));
        index = 0;
        for (area = (EDAIAREA_s *)NuLinkedListGetHead(&aieditor->areas); area != NULL;
             area = (EDAIAREA_s *)NuLinkedListGetNext(&aieditor->areas, &area->link)) {
            AIAREA_PC *runtime = &aieditor->ai_system->areas[index++];
            memcpy(runtime, &area->name, sizeof(*runtime)); /* the record is an AIAREA_PC image behind its link */
            runtime->system = aieditor->ai_system;
        }
    }
    if (aieditor->ai_system->path_sys != NULL) {
        EDLOCATOR_s *locator;
        EDLOCATORSET_s *set;
        EDAICREATURE_s *creature;
        locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators);
        aieditor->ai_system->locator_count = 0;
        for (; locator != NULL; locator = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &locator->link)) {
            if (locator->path_check.path != NULL)
                ++aieditor->ai_system->locator_count;
        }
        if (aieditor->ai_system->locator_count != 0) {
            aieditor->ai_system->locators = (AILOCATOR_PC *)AISysBufferAlloc(
                &aieditor->ai_system->storage_cursor, &aieditor->ai_system->storage_end,
                aieditor->ai_system->locator_count * sizeof(AILOCATOR_PC));
            index = 0;
            for (locator = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators); locator != NULL;
                 locator = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &locator->link)) {
                AILOCATOR_PC *runtime;
                AIPATH_PC *path;
                i32 connection_index;
                locator->runtime_index = 0xff;
                if (locator->path_check.path == NULL)
                    continue;
                locator->runtime_index = index;
                runtime = &aieditor->ai_system->locators[index++];
                strcpy(runtime->name, locator->name);
                runtime->rotation = locator->direction;
                runtime->locator_flags = locator->path_check.angle;
                runtime->position = locator->position;
                runtime->path_info.path = aieditor->ai_system->path_sys->paths[locator->path_check.path->draw_index];
                runtime->path_info.distance = locator->path_check.fraction;
                runtime->path_info.width = locator->path_check.width;
                if (runtime->path_info.path == NULL)
                    continue;
                for (connection_index = 0; connection_index < runtime->path_info.path->connection_count;
                     ++connection_index) {
                    AIPATHCNX_PC *connection = &runtime->path_info.path->connections[connection_index];
                    i32 first = locator->path_check.first->index;
                    if (connection->node_indices[0] == first &&
                        (u16)connection->node_indices[1] == (u16)locator->path_check.second->index) {
                        runtime->path_info.connection = connection;
                        runtime->path_info.direction = abs(locator->path_check.angle) >= 0x4000;
                        break;
                    }
                    if (connection->node_indices[0] == locator->path_check.second->index &&
                        connection->node_indices[1] == first) {
                        runtime->path_info.connection = connection;
                        runtime->path_info.distance = 1.0f - locator->path_check.fraction;
                        runtime->path_info.width = -locator->path_check.width;
                        runtime->path_info.direction = abs(locator->path_check.angle) < 0x4000;
                        break;
                    }
                }
            }
        }
        aieditor->ai_system->locator_set_count = 0;
        for (set = (EDLOCATORSET_s *)NuLinkedListGetHead(&aieditor->locator_sets); set != NULL;
             set = (EDLOCATORSET_s *)NuLinkedListGetNext(&aieditor->locator_sets, &set->link))
            ++aieditor->ai_system->locator_set_count;
        if (aieditor->ai_system->locator_set_count != 0) {
            aieditor->ai_system->locator_sets = (AILOCATORSET_PC *)AISysBufferAlloc(
                &aieditor->ai_system->storage_cursor, &aieditor->ai_system->storage_end,
                aieditor->ai_system->locator_set_count * 0x3c);
            if (aieditor->ai_system->locator_sets != NULL) {
                index = 0;
                for (set = (EDLOCATORSET_s *)NuLinkedListGetHead(&aieditor->locator_sets); set != NULL;
                     set = (EDLOCATORSET_s *)NuLinkedListGetNext(&aieditor->locator_sets, &set->link)) {
                    AILOCATORSET_PC *runtime = &aieditor->ai_system->locator_sets[index++];
                    i32 member;
                    strcpy(runtime->name, set->name);
                    runtime->locator_count = 0;
                    for (member = 0; member < 64 && set->locators[member] != NULL; ++member) {
                        if (set->locators[member]->runtime_index != 0xff)
                            ++runtime->locator_count;
                    }
                    if (runtime->locator_count != 0) {
                        REBUILD_ALLOCATE(u8 *, runtime->locator_entries, &aieditor->ai_system->storage_cursor,
                                         &aieditor->ai_system->storage_end, runtime->locator_count);
                        for (member = 0; member < runtime->locator_count; ++member) {
                            if (set->locators[member]->runtime_index != 0xff)
                                runtime->locator_entries[member] = set->locators[member]->runtime_index;
                        }
                        REBUILD_ALLOCATE(u8 *, runtime->assigned, &aieditor->ai_system->storage_cursor,
                                         &aieditor->ai_system->storage_end, runtime->locator_count);
                        memset(runtime->assigned, 0, runtime->locator_count);
                    }
                }
            }
        }
        creature = (EDAICREATURE_s *)NuLinkedListGetHead(&aieditor->creatures);
        aieditor->ai_system->creature_count = 0;
        for (; creature != NULL;
             creature = (EDAICREATURE_s *)NuLinkedListGetNext(&aieditor->creatures, &creature->link))
            ++aieditor->ai_system->creature_count;
        if (aieditor->ai_system->creature_count != 0) {
            REBUILD_ALLOCATE(AICREATURE_PC *, aieditor->ai_system->creatures, &aieditor->ai_system->storage_cursor,
                             &aieditor->ai_system->storage_end,
                             aieditor->ai_system->creature_count * sizeof(AICREATURE_PC));
            index = 0;
            for (creature = (EDAICREATURE_s *)NuLinkedListGetHead(&aieditor->creatures); creature != NULL;
                 creature = (EDAICREATURE_s *)NuLinkedListGetNext(&aieditor->creatures, &creature->link)) {
                AICREATURE_PC *runtime = &aieditor->ai_system->creatures[index++];
                AIPATH_PC *path;
                i32 connection_index;
                i32 item;
                strcpy(runtime->name, creature->name);
                strcpy(runtime->script_name, creature->script_name);
                runtime->type = creature->character_type;
                runtime->set = creature->set;
                runtime->count = creature->group_count;
                runtime->count_across = creature->across_count;
                runtime->active_mask = creature->valid_positions;
                runtime->x_spacing = creature->x_spacing;
                runtime->z_spacing = creature->z_spacing;
                runtime->flags = creature->flags;
                runtime->y_rotation = creature->angle;
                runtime->position = creature->position;
                runtime->path_info.path = aieditor->ai_system->path_sys->paths[creature->check.path->draw_index];
                runtime->path_info.distance = creature->check.fraction;
                runtime->path_info.width = creature->check.width;
                for (item = 0; item < 4; ++item)
                    runtime->script_params[item] = creature->script_params[item];
                path = runtime->path_info.path;
                if (path != NULL && path->connection_count != 0) {
                    for (connection_index = 0; connection_index < path->connection_count; ++connection_index) {
                        AIPATHCNX_PC *connection = &path->connections[connection_index];
                        i32 first = creature->check.first->index;
                        if (connection->node_indices[0] == first &&
                            (u16)connection->node_indices[1] == (u16)creature->check.second->index) {
                            runtime->path_info.connection = connection;
                            runtime->path_info.direction = abs(creature->check.angle) >= 0x4000;
                            break;
                        }
                        if (connection->node_indices[0] == creature->check.second->index &&
                            connection->node_indices[1] == first) {
                            runtime->path_info.connection = connection;
                            runtime->path_info.distance = 1.0f - creature->check.fraction;
                            runtime->path_info.width = -creature->check.width;
                            runtime->path_info.direction = abs(creature->check.angle) < 0x4000;
                            break;
                        }
                    }
                }
                if (creature->trigger_area != NULL) {
                    for (item = 0; item < aieditor->ai_system->area_count; ++item) {
                        AIAREA_PC *runtime_area = &aieditor->ai_system->areas[item];
                        if (NuStrICmp(creature->trigger_area->name, runtime_area->name) == 0) {
                            runtime->area = runtime_area;
                            break;
                        }
                    }
                }
                if (creature->locator != NULL) {
                    for (item = 0; item < aieditor->ai_system->locator_count; ++item) {
                        AILOCATOR_PC *runtime_locator = &aieditor->ai_system->locators[item];
                        if (NuStrICmp(creature->locator->name, runtime_locator->name) == 0) {
                            runtime->locator = runtime_locator;
                            break;
                        }
                    }
                }
                if (creature->respawn_locator != NULL) {
                    for (item = 0; item < aieditor->ai_system->locator_count; ++item) {
                        AILOCATOR_PC *runtime_locator = &aieditor->ai_system->locators[item];
                        if (NuStrICmp(creature->respawn_locator->name, runtime_locator->name) == 0) {
                            runtime->respawn_locator = runtime_locator;
                            break;
                        }
                    }
                }
                runtime->activation_difficulty = creature->difficulty;
                runtime->min_respawn_count = creature->min_respawns;
                runtime->max_respawn_count = creature->max_respawns;
                runtime->min_respawn_time = creature->min_respawn_time;
                runtime->max_respawn_time = creature->max_respawn_time;
                runtime->start_stagger = creature->stagger_start;
                runtime->view_distance = creature->view_distance;
                runtime->hear_distance = creature->hear_distance;
                runtime->max_view_height = creature->max_view_height;
                runtime->min_view_height = creature->min_view_height;
                runtime->activation_type = creature->activation;
                if (creature->activation == 1) {
                    runtime->activation_type = 0;
                    if (creature->activation_area != NULL) {
                        for (item = 0; item < aieditor->ai_system->area_count; ++item) {
                            AIAREA_PC *runtime_area = &aieditor->ai_system->areas[item];
                            if (NuStrICmp(creature->activation_area->name, runtime_area->name) == 0) {
                                runtime->activate_area = runtime_area;
                                runtime->activation_type = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (GameAISYSRebuildFromEditorDataFn != NULL) {
            system = aieditor->ai_system;
            GameAISYSRebuildFromEditorDataFn(system, &system->storage_cursor, &system->storage_end);
        }
    }
    aieditor->ai_system->antinode_count = 0;
    for (node = (EDANTINODE_s *)NuLinkedListGetHead(&aieditor->antinodes); node != NULL;
         node = (EDANTINODE_s *)NuLinkedListGetNext(&aieditor->antinodes, &node->link))
        ++aieditor->ai_system->antinode_count;
    if (aieditor->ai_system->antinode_count != 0) {
        REBUILD_ALLOCATE(AIANTINODE_PC *, aieditor->ai_system->antinodes, &aieditor->ai_system->storage_cursor,
                         &aieditor->ai_system->storage_end,
                         aieditor->ai_system->antinode_count * sizeof(AIANTINODE_PC));
        index = 0;
        for (node = (EDANTINODE_s *)NuLinkedListGetHead(&aieditor->antinodes); node != NULL;
             node = (EDANTINODE_s *)NuLinkedListGetNext(&aieditor->antinodes, &node->link)) {
            AIANTINODE_PC *runtime = &aieditor->ai_system->antinodes[index++];
            runtime->enabled = 1;
            runtime->position = node->position;
            runtime->radius = node->radius;
            runtime->game_flags = node->game_flags;
            memcpy(&runtime->special_handle, &node->special, sizeof(runtime->special_handle));
            if (NuSpecialExistsFn(&runtime->special_handle))
                runtime->has_special = 1;
            runtime->special_position = node->special_position;
            runtime->rotation_offset = node->rotation_offset;
            runtime->min_y = node->lower_height + runtime->position.y;
            runtime->max_y = node->upper_height + runtime->position.y;
            runtime->min_y_offset = runtime->min_y - runtime->position.y;
            runtime->max_y_offset = runtime->max_y - runtime->position.y;
            runtime->rotation = node->flags;
            runtime->base_radius = node->base_radius;
            runtime->base_height = node->base_height;
            runtime->type = node->type;
            if (runtime->type == 2)
                runtime->radius =
                    NuFsqrt(runtime->base_radius * runtime->base_radius + runtime->base_height * runtime->base_height);
            else if (runtime->type == 1) {
                if (runtime->base_radius > runtime->base_height)
                    runtime->radius = runtime->base_radius;
                else
                    runtime->radius = runtime->base_height;
            }
        }
    }
    AISysSetLevelPath(aieditor->ai_system, NULL);
}

i32 __cdecl aieditor_Save(void) {
    VARIPTR cursor;
    VARIPTR end;
    AIPATHSYS_PC *paths;
    AI_EDITOR_FILE_INFO_PC info;
    char filename[128];
    char backup[128];
    char *basename;
    char *separator;

    cursor.char_ptr = (char *)aieditor->save_scratch;
    end.char_ptr = cursor.char_ptr + sizeof(aieditor->save_scratch);
    memset(cursor.char_ptr, 0, sizeof(aieditor->save_scratch));
    paths =
        pathEditorCreateData(&cursor, &end, &aieditorsettings.path_scratch_cursor, &aieditorsettings.path_scratch_end);
    if (paths == NULL)
        return 1;
    if (aieditor->level_file == NULL)
        return 1;
    sprintf(filename, "%sLevels\\%s\\%s\\%s.ai2", AiLevelPathName, aieditor->level_directory,
            aieditor->level_subdirectory, aieditor->level_file);
    if (NuFileExists(filename)) {
        if (NuFileGetInfo(filename, &info)) {
            basename = filename;
            separator = NuStrIStr(basename, "\\");
            while (separator != NULL) {
                basename = separator + 1;
                separator = NuStrIStr(basename, "\\");
            }
            sprintf(backup, AiEditorDatedBackupFormatPc, basename, info.year, info.month, info.day, info.hour,
                    info.minute, info.second);
            NuFileCopy(backup, filename);
        }
        sprintf(backup, "%s.bak", filename);
        NuFileCopy(backup, filename);
    }
    EdFileSetMedia(1);
    if (EdFileOpen(filename, 1) == 0)
        return 0;
    EdFileWriteInt(aidata_version);
    pathEditorSaveData(paths);
    areaEditorSaveData();
    locatorEditorSaveData(paths);
    creatureEditorSaveData(paths);
    antinodeEditorSaveData();
    if (GameAISaveFn != NULL)
        GameAISaveFn(paths);
    EdFileClose();
    return 1;
}

void __cdecl aieditor_Leave(void) {
    i32 index;
    edmainGetCameraPc()->near_clip = 0.15f;
    for (index = 0; index < aieditorsettings.modes.count; ++index) {
        if (aieditorsettings.modes.entries[index].leave != NULL)
            aieditorsettings.modes.entries[index].leave();
    }
    if (aieditor->selected_creature != NULL)
        strcpy(aieditorsettings.creature_name, aieditor->selected_creature->name);
    if (aieditor->current_path != NULL)
        strcpy(aieditorsettings.path_name, aieditor->current_path->name);
    if (aieditor->current_locator_set != NULL)
        strcpy(aieditorsettings.locator_set_name, aieditor->current_locator_set->name);
    AISYSRebuildFromEditorData();
    AIScriptInitConditions(aieditor->ai_system);
    aieditor->flags &= ~2u;
}

eduimenu_s *__cdecl areaEditorDeleteMenu(void) {
    eduimenu_s *menu;
    if (aieditor->selected_area != NULL && aieditor->selected_area == aieditor->hovered_area) {
        menu = eduiMenuCreate(200, 70, 240, 270, ed_fnt, areaEditor_cbCancelDeleteAreaMenu, "Delete area??");
        if (menu != NULL) {
            eduiMenuAddItem(menu,
                            eduiItemSelCreate(0, &AiEditorSaveMessageColoursPc, 0, 0, areaEditor_cbDeleteArea, "No"));
            eduiMenuAddItem(menu,
                            eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, areaEditor_cbDeleteArea, "Yes"));
            return menu;
        }
    }
    return NULL;
}

eduimenu_s *__cdecl locatorEditor_DeleteLocatorMenu(void) {
    eduimenu_s *menu;
    if (aieditor->current_locator == NULL || aieditor->current_locator != aieditor->nearest_locator)
        return NULL;
    menu = eduiMenuCreate(200, 70, 240, 270, ed_fnt, AiEditorDeleteLocatorCancelPc, "Delete locator??");
    if (menu != NULL) {
        eduiMenuAddItem(menu, eduiItemSelCreate(0, &AiEditorSaveMessageColoursPc, 0, 0,
                                                AiEditorDeleteLocatorSelectionPc, (char *)MenuPcString75f2c0));
        eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0,
                                                AiEditorDeleteLocatorSelectionPc, (char *)MenuPcString75f2c4));
    }
    return menu;
}

static __declspec(noinline) void pathEditor_DestroyPath(EDAIPATH_s *path) {
    EDAIPATHNODE_s *node;
    if (path == NULL)
        return;
    for (node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&path->nodes); node != NULL;
         node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&path->nodes)) {
        pathEditor_DestroyNode(node, path);
    }
    NuLinkedListRemove(&aieditor->paths, &path->link);
    memset(path, 0, sizeof(*path));
    NuLinkedListAppend(&aieditor->free_paths, &path->link);
}

void __cdecl pathEditor_cbDeletePath(eduimenu_s *parent, eduiitem_s *item, u32 event) {
    eduimenu_s *menu;
    (void)event;
    if (item == NULL)
        return;
    switch (item->data) {
        case 0:
            menu = eduiMenuCreate(240, 90, 240, 250, ed_fnt, NULL, "Delete current path??");
            if (menu != NULL) {
                eduiMenuAddItem(
                    menu, eduiItemSelCreate(2, &AiEditorSaveMessageColoursPc, 0, 0, pathEditor_cbDeletePath, "No"));
                eduiMenuAddItem(
                    menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, pathEditor_cbDeletePath, "Yes"));
                eduiMenuAttach(parent, menu);
            }
            break;
        case 1:
            if (aieditor->current_path == NULL || (aieditor->current_path->flags & 1))
                return;
            creatureEditor_PathDeleted(aieditor->current_path);
            locatorEditor_PathDeleted(aieditor->current_path);
            if (AIPathDeletedFn != NULL)
                AIPathDeletedFn(aieditor->current_path);
            pathEditor_DestroyPath(aieditor->current_path);
            aieditor->current_path = (EDAIPATH_s *)NuLinkedListGetHead(&aieditor->paths);
            AiEditorClearMainMenuPc();
            break;
        case 2:
            AiEditorClearMainMenuPc();
            break;
    }
}

eduimenu_s *__cdecl pathEditor_DeleteNodeMenu(void) {
    eduimenu_s *menu;
    if (aieditor->current_path->current_node != NULL &&
        aieditor->current_path->current_node == aieditor->current_path->nearest_node) {
        menu =
            eduiMenuCreate(200, 70, 240, 270, ed_fnt, pathEditor_cbCancelDeleteNodeMenu, "Delete current path node??");
        if (menu != NULL) {
            eduiMenuAddItem(
                menu, eduiItemSelCreate(0, &AiEditorSaveMessageColoursPc, 0, 0, pathEditor_cbDeletePathNode, "No"));
            eduiMenuAddItem(
                menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, pathEditor_cbDeletePathNode, "Yes"));
            return menu;
        }
    }
    return NULL;
}

eduimenu_s *__cdecl pathEditor_DisconnectNodeMenu(void) {
    eduimenu_s *menu;
    if (aieditor->current_path->current_node != NULL && aieditor->current_path->nearest_node != NULL &&
        aieditor->current_path->current_node != aieditor->current_path->nearest_node) {
        menu = eduiMenuCreate(200, 70, 240, 270, ed_fnt, pathEditor_cbCancelDisconnectNodeMenu,
                              "Disconnect current path node??");
        if (menu != NULL) {
            eduiMenuAddItem(
                menu, eduiItemSelCreate(0, &AiEditorSaveMessageColoursPc, 0, 0, pathEditor_cbDisconnectPathNode, "No"));
            eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0,
                                                    pathEditor_cbDisconnectPathNode, "Yes"));
            return menu;
        }
    }
    return NULL;
}

void __cdecl aieditor_cbSave(eduimenu_s *parent, eduiitem_s *item, u32 value) {
    eduimenu_s *menu;
    char message[128];
    (void)item;
    (void)value;
    if (aieditor_Save() != 0) {
        if (aieditor->main_menu != NULL) {
            eduiMenuDestroy(aieditor->main_menu);
            aieditor->main_menu = NULL;
        }
    } else {
        menu = eduiMenuCreate(0x78, 0x5a, 0x1b8, 0xfa, ed_fnt, NULL, "AI Save FAILED");
        if (menu != NULL) {
            sprintf(message, "Make sure there is a folder called \"%s\" in your level directory",
                    aieditor->level_subdirectory);
            eduiMenuAddItem(
                menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, AiEditorSaveCloseSelectionPc, message));
            eduiMenuAttach(parent, menu);
        }
    }
}

eduimenu_s *__cdecl antinodeEditor_MainMenu(void) {
    eduimenu_s *menu = eduiMenuCreate(200, 70, 240, 270, ed_fnt, AiEditorMessageCloseMenuPc, "Options");
    if (menu == NULL)
        return NULL;
    eduiMenuAddItem(menu, eduiItemSelCreate(AiEditorAntinodeModeIdPc, &AiEditorSaveMessageColoursPc, 0, 0,
                                            AiEditorModeMenuPc, "Select Editor Mode"));
    eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, aieditor_cbSave, "Save AI Data"));
    eduiMenuAddItem(
        menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, AiEditorCameraToPlayerPc, "Go To Player"));
    eduiMenuAddItem(menu,
                    eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, AiEditorMovePlayersPc, "Move Player"));
    if (aieditor->selected_antinode != NULL)
        eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, AiEditorAntinodeTypeMenuPc,
                                                "Select Antinode Type"));
    eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc,
                                               (i32)(aieditorsettings.display_options << 23) >> 31, 1,
                                               AiEditorDisplayBit8Pc, "Solid Antinode Display"));
    eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc,
                                               (i32)(aieditorsettings.display_options << 25) >> 31, 2,
                                               AiEditorDisplayBit6Pc, "Stop Platforms"));
    eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc,
                                               (i32)(aieditorsettings.display_options << 27) >> 31, 3,
                                               AiEditorDisplayBit4Pc, "Snap Height"));
    if (aieditor->selected_antinode != NULL) {
        eduiMenuAddItem(menu, eduiItemToggleCreate(0x80, &AiEditorSaveMessageColoursPc,
                                                   aieditor->selected_antinode->game_flags >> 7, 4,
                                                   AiEditorAntinodeGameFlagsPc, "Never On A Platform"));
        eduiMenuAddItem(menu, eduiItemToggleCreate(2, &AiEditorSaveMessageColoursPc,
                                                   (aieditor->selected_antinode->game_flags >> 1) & 1, 5,
                                                   AiEditorAntinodeGameFlagsPc, "Disbale direction memory"));
    }
    return menu;
}

eduimenu_s *__cdecl antinodeEditor_Process(FRAME_PAD_PC *pad) {
    EDANTINODE_s *first;
    i32 skip_platforms;
    i32 forward;
    f32 step;
    NUVEC *platform_position;
    if (pad->digital_buttons_pressed & 0x80)
        return antinodeEditor_MainMenu();
    if (pad->digital_buttons & 0x40) {
        if (aieditor->nearest_antinode != NULL) {
            if (pad->digital_buttons_pressed & 0x40) {
                aieditor->selected_antinode = aieditor->nearest_antinode;
                edcamSetPos(&aieditor->selected_antinode->position);
            } else if (aieditor->selected_antinode != NULL &&
                       aieditor->selected_antinode == aieditor->nearest_antinode) {
                if ((pad->digital_buttons_pressed & 0x100) &&
                    NuSpecialExistsFn(&aieditor->selected_antinode->special)) {
                    platform_position = NuSpecialGetDrawPos(&aieditor->cursor_platform);
                    if (platform_position != NULL) {
                        aieditor->selected_antinode->position.x = platform_position->x;
                        aieditor->selected_antinode->position.z = platform_position->z;
                        antinodeEditor_AntinodeMoved(aieditor->selected_antinode);
                        edcamSetPos(&aieditor->selected_antinode->position);
                    }
                } else {
                    aieditor->selected_antinode->position = aieditor->camera_position;
                    antinodeEditor_AntinodeMoved(aieditor->selected_antinode);
                    if (aieditor->selected_antinode->type != 0) {
                        if (pad->digital_buttons & 0x8000) {
                            aieditor->selected_antinode->base_radius =
                                aieditor->selected_antinode->base_radius * ((double)0.99f);
                            if (aieditor->selected_antinode->base_radius < ((double)0.05f))
                                aieditor->selected_antinode->base_radius = 0.05f;
                        } else if (pad->digital_buttons & 0x2000) {
                            aieditor->selected_antinode->base_radius =
                                aieditor->selected_antinode->base_radius * ((double)1.01f);
                        } else if (pad->digital_buttons & 0x1000) {
                            aieditor->selected_antinode->base_height =
                                aieditor->selected_antinode->base_height * ((double)1.01f);
                        } else if (pad->digital_buttons & 0x4000) {
                            aieditor->selected_antinode->base_height =
                                aieditor->selected_antinode->base_height * ((double)0.99f);
                            if (aieditor->selected_antinode->base_height < ((double)0.05f))
                                aieditor->selected_antinode->base_height = 0.05f;
                        }
                    }
                }
            }
        } else if (pad->digital_buttons_pressed & 0x40) {
            aieditor->selected_antinode = CreateAntinode(&aieditor->camera_position);
            antinodeEditor_AntinodeMoved(aieditor->selected_antinode);
        }
    } else if (pad->digital_buttons_pressed & 0x10) {
        if (aieditor->selected_antinode != NULL && aieditor->selected_antinode == aieditor->nearest_antinode)
            return antinodeEditor_DeleteAntinodeMenu();
    } else if (pad->digital_buttons_pressed & 0x100) {
        aieditor->selected_antinode = antinodeEditor_GetNearestAntinode(0);
        if (aieditor->selected_antinode != NULL)
            edcamSetPos(&aieditor->selected_antinode->position);
    } else if (pad->digital_buttons & 0x100) {
        if (pad->digital_buttons_pressed & 8) {
            forward = 1;
            skip_platforms = 0;
        } else if (pad->digital_buttons_pressed & 2) {
            forward = 0;
            skip_platforms = 0;
        } else if (pad->digital_buttons_pressed & 4) {
            forward = 1;
            skip_platforms = 1;
        } else if (pad->digital_buttons_pressed & 1) {
            forward = 0;
            skip_platforms = 1;
        } else {
            goto nearest;
        }
        if (forward) {
            if (aieditor->selected_antinode != NULL)
                aieditor->selected_antinode =
                    (EDANTINODE_s *)NuLinkedListGetNext(&aieditor->antinodes, &aieditor->selected_antinode->link);
            if (aieditor->selected_antinode == NULL)
                aieditor->selected_antinode = (EDANTINODE_s *)NuLinkedListGetHead(&aieditor->antinodes);
            if (skip_platforms && aieditor->selected_antinode != NULL &&
                NuSpecialExistsFn(&aieditor->selected_antinode->special)) {
                first = aieditor->selected_antinode;
                do {
                    aieditor->selected_antinode =
                        (EDANTINODE_s *)NuLinkedListGetNext(&aieditor->antinodes, &aieditor->selected_antinode->link);
                    if (aieditor->selected_antinode == NULL) {
                        aieditor->selected_antinode = (EDANTINODE_s *)NuLinkedListGetHead(&aieditor->antinodes);
                        if (aieditor->selected_antinode == NULL)
                            break;
                    }
                    if (aieditor->selected_antinode == first) {
                        aieditor->selected_antinode = NULL;
                        break;
                    }
                } while (NuSpecialExistsFn(&aieditor->selected_antinode->special));
            }
        } else {
            if (aieditor->selected_antinode != NULL)
                aieditor->selected_antinode =
                    (EDANTINODE_s *)NuLinkedListGetPrev(&aieditor->antinodes, &aieditor->selected_antinode->link);
            if (aieditor->selected_antinode == NULL)
                aieditor->selected_antinode = (EDANTINODE_s *)NuLinkedListGetTail(&aieditor->antinodes);
            if (skip_platforms && aieditor->selected_antinode != NULL &&
                NuSpecialExistsFn(&aieditor->selected_antinode->special)) {
                first = aieditor->selected_antinode;
                do {
                    aieditor->selected_antinode =
                        (EDANTINODE_s *)NuLinkedListGetPrev(&aieditor->antinodes, &aieditor->selected_antinode->link);
                    if (aieditor->selected_antinode == NULL) {
                        aieditor->selected_antinode = (EDANTINODE_s *)NuLinkedListGetTail(&aieditor->antinodes);
                        if (aieditor->selected_antinode == NULL)
                            break;
                    }
                    if (aieditor->selected_antinode == first) {
                        aieditor->selected_antinode = NULL;
                        break;
                    }
                } while (NuSpecialExistsFn(&aieditor->selected_antinode->special));
            }
        }
        if (aieditor->selected_antinode != NULL)
            edcamSetPos(&aieditor->selected_antinode->position);
    } else if (pad->digital_buttons & 0x8000) {
        if (aieditor->selected_antinode != NULL && aieditor->selected_antinode == aieditor->nearest_antinode) {
            if (aieditor->selected_antinode->type == 0) {
                aieditor->selected_antinode->radius = aieditor->selected_antinode->radius * ((double)0.99f);
                if (aieditor->selected_antinode->radius < ((double)0.05f))
                    aieditor->selected_antinode->radius = 0.05f;
            } else {
                aieditorsettings.locator_rotation = aieditor->selected_antinode->flags;
                /* Crossed Native pressed bits are intentional. */
                if (pad->digital_buttons_pressed & 0x2000)
                    aieditor->rotation_repeat = 20;
                else {
                    aieditor->rotation_repeat += 20;
                    if (aieditor->rotation_repeat > 600)
                        aieditor->rotation_repeat = 600;
                }
                aieditorsettings.locator_rotation =
                    NuAngSub(aieditorsettings.locator_rotation, aieditor->rotation_repeat);
                if (aieditor->selected_antinode != NULL && aieditor->selected_antinode == aieditor->nearest_antinode)
                    aieditor->selected_antinode->flags = aieditorsettings.locator_rotation;
                antinodeEditor_AntinodeMoved(aieditor->selected_antinode);
            }
        }
    } else if (pad->digital_buttons & 0x2000) {
        if (aieditor->selected_antinode != NULL && aieditor->selected_antinode == aieditor->nearest_antinode) {
            if (aieditor->selected_antinode->type == 0) {
                aieditor->selected_antinode->radius = aieditor->selected_antinode->radius * ((double)1.01f);
            } else {
                aieditorsettings.locator_rotation = aieditor->selected_antinode->flags;
                if (pad->digital_buttons_pressed & 0x8000)
                    aieditor->rotation_repeat = 20;
                else {
                    aieditor->rotation_repeat += 20;
                    if (aieditor->rotation_repeat > 600)
                        aieditor->rotation_repeat = 600;
                }
                aieditorsettings.locator_rotation =
                    NuAngAdd(aieditorsettings.locator_rotation, aieditor->rotation_repeat);
                if (aieditor->selected_antinode != NULL && aieditor->selected_antinode == aieditor->nearest_antinode)
                    aieditor->selected_antinode->flags = aieditorsettings.locator_rotation;
                antinodeEditor_AntinodeMoved(aieditor->selected_antinode);
            }
        }
    } else if (pad->digital_buttons & 0x1000) {
        if ((aieditorsettings.display_options & 0x100) && aieditor->selected_antinode != NULL &&
            aieditor->selected_antinode == aieditor->nearest_antinode) {
            aieditor->flags |= 4;
            step = (aieditor->selected_antinode->upper_height - aieditor->selected_antinode->lower_height) *
                   ((double)0.05f);
            if (pad->digital_buttons & 4)
                aieditor->selected_antinode->upper_height = step + aieditor->selected_antinode->upper_height;
            else if (pad->digital_buttons & 1)
                aieditor->selected_antinode->upper_height -= step;
            if (aieditor->selected_antinode->upper_height < aieditor->selected_antinode->lower_height + ((double)0.01f))
                aieditor->selected_antinode->upper_height = aieditor->selected_antinode->lower_height + ((double)0.01f);
        }
    } else if (pad->digital_buttons & 0x4000) {
        if ((aieditorsettings.display_options & 0x100) && aieditor->selected_antinode != NULL &&
            aieditor->selected_antinode == aieditor->nearest_antinode) {
            aieditor->flags |= 4;
            step = (aieditor->selected_antinode->upper_height - aieditor->selected_antinode->lower_height) *
                   ((double)0.05f);
            if (pad->digital_buttons & 4)
                aieditor->selected_antinode->lower_height += step;
            else if (pad->digital_buttons & 1)
                aieditor->selected_antinode->lower_height -= step;
            if (aieditor->selected_antinode->lower_height > aieditor->selected_antinode->upper_height - ((double)0.01f))
                aieditor->selected_antinode->lower_height = aieditor->selected_antinode->upper_height - ((double)0.01f);
        }
    }
nearest:
    aieditor->nearest_antinode = antinodeEditor_GetNearestAntinode(1);
    return NULL;
}

eduimenu_s *__cdecl creatureEditorOptionsMenu(void) {
    eduimenu_s *menu;
    i32 group;
    menu = eduiMenuCreate(200, 70, 240, 330, ed_fnt, AiEditorMessageCloseMenuPc, "Options");
    group = 1;
    if (menu != NULL) {
        eduiMenuAddItem(menu, eduiItemSelCreate(AiEditorRouteModeIdPc, &AiEditorSaveMessageColoursPc, 0, 0,
                                                AiEditorModeMenuPc, "Select Editor Mode"));
        eduiMenuAddItem(menu,
                        eduiItemSelCreate(group, &AiEditorSaveMessageColoursPc, 0, 0, aieditor_cbSave, "Save AI Data"));
        eduiMenuAddItem(menu, eduiItemSelCreate(group, &AiEditorSaveMessageColoursPc, 0, 0, AiEditorCameraToPlayerPc,
                                                "Go To Player"));
        eduiMenuAddItem(
            menu, eduiItemSelCreate(group, &AiEditorSaveMessageColoursPc, 0, 0, AiEditorMovePlayersPc, "Move Player"));
        eduiMenuAddItem(menu, eduiItemSelCreate(group, &AiEditorSaveMessageColoursPc, 0, 0, creatureEditor_cbSelectType,
                                                "Select Creature Type"));
        if (aieditor->selected_creature != NULL) {
            if (AIScriptNameFromIx(aieditor->ai_system, 0) != NULL)
                eduiMenuAddItem(menu, eduiItemSelCreate(group, &AiEditorSaveMessageColoursPc, 0, 0,
                                                        creatureEditor_cbSelectScript, "Select Script"));
            eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0,
                                                    creatureEditor_cbScriptParams, "Script Params"));
            eduiMenuAddItem(menu, eduiItemSliderCreateInt(1, &AiEditorSaveMessageColoursPc, 0,
                                                          creatureEditor_cb_assigntoset, 0, aisys_maxnumcreaturesets,
                                                          aieditor->selected_creature->set, "Assigned To Set"));
            eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0,
                                                    creatureEditor_cbRenameCreatureMenu, "Rename Creature"));
            eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0,
                                                    creatureEditor_cbActivationMenu, "Activation Conditions"));
            eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0,
                                                    creatureEditor_cbRespawnMenu, "Respawn Values"));
            eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, creatureEditor_cbGroupMenu,
                                                    "Group Values"));
            eduiMenuAddItem(
                menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, creatureEditor_cbVisionMenu, "Vision"));
            eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc,
                                                       aieditor->selected_creature->flags & 1, 1,
                                                       creatureEditor_cbFlagsToggle, "Ignore Wall Splines"));
            group = 2;
        }
        eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc,
                                                   (i32)(aieditorsettings.display_options << 25) >> 31, group,
                                                   AiEditorDisplayBit6Pc, "Stop Platforms"));
        ++group;
        eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc,
                                                   (i32)(aieditorsettings.display_options << 27) >> 31, group,
                                                   AiEditorDisplayBit4Pc, "Snap Height"));
        eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc,
                                                   (i32)(aieditorsettings.display_options << 21) >> 31, group + 1,
                                                   aieditor_cbShowCreaturesSetToggle, "Show Current Set"));
        return menu;
    }
    return NULL;
}

eduimenu_s *__cdecl creatureEditor_Process(FRAME_PAD_PC *pad) {
    EDAICREATURE_s *creature;
    EDLOCATOR_s *locator;
    char base[16];
    char name[16];
    char *separator;
    i32 index = 0;
    i32 set;
    i32 angle;
    i32 quarter;
    i32 remainder;
    if (pad->digital_buttons_pressed & 0x80)
        return creatureEditorOptionsMenu();
    if (pad->digital_buttons & 0x40) {
        if (aieditor->nearest_creature != NULL) {
            if (pad->digital_buttons_pressed & 0x40) {
                aieditor->selected_creature = aieditor->nearest_creature;
                aieditor->current_path = aieditor->selected_creature->check.path;
                aieditorsettings.locator_rotation = aieditor->selected_creature->angle;
                edcamSetPos(&aieditor->selected_creature->position);
                aieditorsettings.character_type = aieditor->selected_creature->character_type;
                aieditor_SetCurrentScript(aieditor->selected_creature->script_name, aieditor->selected_creature);
            } else if (aieditor->selected_creature != NULL) {
                if (aieditor->cursor_check.on_path != 0) {
                    aieditor->selected_creature->position = aieditor->camera_position;
                    aieditor->selected_creature->check = aieditor->cursor_check;
                    goto update_angle;
                }
                edcamSetPos(&aieditor->selected_creature->position);
            }
        } else if ((pad->digital_buttons_pressed & 0x40) && aieditor->cursor_check.on_path != 0) {
            set = 0;
            if (aieditor->selected_creature != NULL) {
                NuStrCpy(base, aieditor->selected_creature->name);
                separator = strrchr(base, '_');
                if (separator != NULL)
                    *separator = 0;
                base[12] = 0;
                set = aieditor->selected_creature->set;
            } else {
                NuStrNCpy(base, GlobalCharacterNameFn(aieditorsettings.character_type), 13);
            }
            aieditor->selected_creature = CreateCreature(aieditorsettings.character_type, &aieditor->camera_position,
                                                         aieditorsettings.locator_rotation);
            if (aieditor->selected_creature != NULL) {
                do {
                    sprintf(name, "%s_%d", base, ++index);
                    for (creature = (EDAICREATURE_s *)NuLinkedListGetHead(&aieditor->creatures); creature != NULL;
                         creature = (EDAICREATURE_s *)NuLinkedListGetNext(&aieditor->creatures, &creature->link)) {
                        if (NuStrICmp(name, creature->name) == 0)
                            break;
                    }
                } while (creature != NULL);
                strcpy(aieditor->selected_creature->name, name);
                if (strlen(aieditorsettings.script_name) != 0)
                    strcpy(aieditor->selected_creature->script_name, aieditorsettings.script_name);
                aieditor->selected_creature->script_params[0] = aieditorsettings.script_params[0];
                aieditor->selected_creature->script_params[1] = aieditorsettings.script_params[1];
                aieditor->selected_creature->script_params[2] = aieditorsettings.script_params[2];
                aieditor->selected_creature->script_params[3] = aieditorsettings.script_params[3];
                aieditor->selected_creature->flags &= ~0x1e;
                aieditor->selected_creature->flags |= aieditorsettings.script_flags;
                aieditor->selected_creature->check.path = aieditor->current_path;
                aieditor->selected_creature->check = aieditor->cursor_check;
                aieditor->selected_creature->check.angle =
                    NuAngSub(aieditor->selected_creature->angle, aieditor->cursor_check.angle);
                aieditor->selected_creature->set = (u8)set;
                goto updated;
            }
        }
    } else if (pad->digital_buttons_pressed & 0x10) {
        if (aieditor->selected_creature != NULL) {
            if (aieditor->selected_creature == aieditor->nearest_creature)
                return creatureEditorDeleteMenu();
            locator = aieditor->nearest_locator;
            if (locator != NULL) {
                if (aieditor->selected_creature->respawn_locator == locator)
                    aieditor->selected_creature->respawn_locator = NULL;
                else if (aieditor->selected_creature->check.path == locator->path_check.path)
                    aieditor->selected_creature->respawn_locator = locator;
                goto done;
            }
        }
        aieditor->selected_creature = NULL;
    } else if (pad->digital_buttons_pressed & 0x20) {
        if (aieditor->selected_creature != NULL && (locator = aieditor->nearest_locator) != NULL) {
            if (aieditor->selected_creature->locator == locator)
                aieditor->selected_creature->locator = NULL;
            else if (aieditor->selected_creature->check.path == locator->path_check.path)
                aieditor->selected_creature->locator = locator;
        }
    } else if ((pad->digital_buttons_pressed & 0x1000) ||
               ((pad->digital_buttons & 0x100) && (pad->digital_buttons_pressed & 8))) {
        if (aieditor->selected_creature != NULL) {
            aieditor->selected_creature =
                (EDAICREATURE_s *)NuLinkedListGetNext(&aieditor->creatures, &aieditor->selected_creature->link);
            if (aieditor->selected_creature != NULL)
                goto selected;
        }
        aieditor->selected_creature = (EDAICREATURE_s *)NuLinkedListGetHead(&aieditor->creatures);
        goto pick;
    } else if ((pad->digital_buttons & 0x100) && (pad->digital_buttons_pressed & 2)) {
        if (aieditor->selected_creature != NULL) {
            aieditor->selected_creature =
                (EDAICREATURE_s *)NuLinkedListGetPrev(&aieditor->creatures, &aieditor->selected_creature->link);
            if (aieditor->selected_creature != NULL)
                goto selected;
        }
        aieditor->selected_creature = (EDAICREATURE_s *)NuLinkedListGetTail(&aieditor->creatures);
        goto pick;
    } else if (pad->digital_buttons_pressed & 0x100) {
        aieditor->selected_creature = creatureEditor_GetNearest(0);
    pick:
        if (aieditor->selected_creature != NULL) {
        selected:
            aieditor->current_path = aieditor->selected_creature->check.path;
            if (aieditor->selected_creature != NULL)
                edcamSetPos(&aieditor->selected_creature->position);
            aieditorsettings.character_type = aieditor->selected_creature->character_type;
            aieditorsettings.character_type = aieditor->selected_creature->character_type;
            aieditor_SetCurrentScript(aieditor->selected_creature->script_name, aieditor->selected_creature);
        }
    } else {
        if (pad->digital_buttons & 0x2000) {
            if (aieditor->selected_creature != NULL && aieditor->selected_creature == aieditor->nearest_creature)
                aieditorsettings.locator_rotation = aieditor->selected_creature->angle;
            if (pad->digital_buttons_pressed & 0x8000) {
                aieditor->rotation_repeat = 20;
            } else {
                aieditor->rotation_repeat += 20;
                if (aieditor->rotation_repeat > 600)
                    aieditor->rotation_repeat = 600;
            }
            angle = NuAngAdd(aieditorsettings.locator_rotation, aieditor->rotation_repeat);
        } else if (pad->digital_buttons & 0x8000) {
            if (aieditor->selected_creature != NULL && aieditor->selected_creature == aieditor->nearest_creature)
                aieditorsettings.locator_rotation = aieditor->selected_creature->angle;
            if (pad->digital_buttons_pressed & 0x2000) {
                aieditor->rotation_repeat = 20;
            } else {
                aieditor->rotation_repeat += 20;
                if (aieditor->rotation_repeat > 600)
                    aieditor->rotation_repeat = 600;
            }
            angle = NuAngSub(aieditorsettings.locator_rotation, aieditor->rotation_repeat);
        } else if (pad->digital_buttons & 0x4000) {
            angle = NuAngSub(aieditorsettings.locator_rotation, aieditor->cursor_check.angle);
            quarter = angle / 0x4000;
            remainder = angle % 0x4000;
            if (remainder > 0x2000)
                ++quarter;
            else if (remainder < -0x2000)
                --quarter;
            angle = NuAngAdd(quarter << 14, aieditor->cursor_check.angle);
        } else {
            goto done;
        }
        aieditorsettings.locator_rotation = angle;
        if (aieditor->selected_creature != NULL && aieditor->selected_creature == aieditor->nearest_creature) {
            aieditor->selected_creature->angle = angle;
        update_angle:
            aieditor->selected_creature->check.angle =
                NuAngSub(aieditor->selected_creature->angle, aieditor->cursor_check.angle);
        updated:
            creatureEditor_Updated(aieditor->selected_creature);
        }
    }
done:
    aieditor->nearest_locator = locatorEditor_GetNearest(1);
    aieditor->nearest_creature = creatureEditor_GetNearest(1);
    return NULL;
}

eduimenu_s *__cdecl areaEditorOptionsMenu(void) {
    eduimenu_s *menu = eduiMenuCreate(200, 70, 240, 270, ed_fnt, AiEditorMessageCloseMenuPc, "Options");
    if (menu != NULL) {
        eduiMenuAddItem(menu, eduiItemSelCreate(AiEditorRouteModeIdPc, &AiEditorSaveMessageColoursPc, 0, 0,
                                                AiEditorModeMenuPc, "Select Editor Mode"));
        eduiMenuAddItem(menu,
                        eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, aieditor_cbSave, "Save AI Data"));
        eduiMenuAddItem(
            menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, AiEditorCameraToPlayerPc, "Go To Player"));
        eduiMenuAddItem(
            menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, AiEditorMovePlayersPc, "Move Player"));
        if (aieditor->selected_area != NULL && AIScriptNameFromIx(aieditor->ai_system, 0) != NULL)
            eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, areaEditor_cbRenameAreaMenu,
                                                    "Rename Area"));
        eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc,
                                                   (i32)(aieditorsettings.display_options << 25) >> 31, 3,
                                                   AiEditorDisplayBit6Pc, "Stop Platforms"));
        eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc,
                                                   (i32)(aieditorsettings.display_options << 27) >> 31, 2,
                                                   AiEditorDisplayBit4Pc, "Snap Height"));
        if (aieditor->selected_area != NULL) {
            eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, NULL, "================="));
            eduiMenuAddItem(menu,
                            eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc, aieditor->selected_area->flags & 1,
                                                 4, areaEditor_cbCylinderToggle, "Cylinder"));
            eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, NULL, "================="));
        }
        return menu;
    }
    return NULL;
}

eduimenu_s *__cdecl areaEditor_Process(FRAME_PAD_PC *pad) {
    EDAIAREA_s *previous;
    EDAIAREA_s *area;
    char name[16];
    i32 index = 0;
    f32 size;
    if (pad->digital_buttons_pressed & 0x80)
        return areaEditorOptionsMenu();
    aieditor->hovered_area = areaEditorFindHover(1);
    if (pad->digital_buttons & 0x40) {
        if (aieditor->hovered_area != NULL) {
            if (pad->digital_buttons_pressed & 0x40) {
                aieditor->selected_area = aieditor->hovered_area;
                aieditorsettings.locator_rotation = (i16)aieditor->selected_area->rotation;
                edcamSetPos(&aieditor->selected_area->position);
            } else {
                if (aieditor->selected_area == NULL)
                    goto done;
                aieditor->selected_area->position = aieditor->camera_position;
            }
            if (aieditor->selected_area == NULL)
                goto done;
            if (aieditor->selected_area->flags & 1) {
                if (pad->digital_buttons & 0x2000)
                    aieditor->selected_area->size.x *= (f64)1.01f;
                else if (pad->digital_buttons & 0x8000)
                    aieditor->selected_area->size.x *= (f64)0.99f;
                aieditor->selected_area->size.z = aieditor->selected_area->size.x;
            } else {
                if (pad->digital_buttons & 0x2000)
                    aieditor->selected_area->size.x *= (f64)1.01f;
                else if (pad->digital_buttons & 0x8000)
                    aieditor->selected_area->size.x *= (f64)0.99f;
                if (pad->digital_buttons & 0x1000)
                    aieditor->selected_area->size.z *= (f64)1.01f;
                else if (pad->digital_buttons & 0x4000)
                    aieditor->selected_area->size.z *= (f64)0.99f;
            }
        } else if (pad->digital_buttons_pressed & 0x40) {
            previous = aieditor->selected_area;
            aieditor->selected_area =
                areaEditorCreateArea(&aieditor->camera_position, NULL, aieditorsettings.locator_rotation,
                                     previous != NULL ? (i8)previous->flags : 0);
            if (aieditor->selected_area == NULL)
                goto done;
            if (previous != NULL) {
                aieditor->selected_area->size.x = previous->size.x;
                aieditor->selected_area->size.y = previous->size.y;
                aieditor->selected_area->size.z = previous->size.z;
            } else {
                aieditor->selected_area->size.z = aieditor->selected_area->size.y = aieditor->selected_area->size.x =
                    1.0f;
            }
            do {
                sprintf(name, "Area%d", ++index);
                for (area = (EDAIAREA_s *)NuLinkedListGetHead(&aieditor->areas); area != NULL;
                     area = (EDAIAREA_s *)NuLinkedListGetNext(&aieditor->areas, &area->link)) {
                    if (NuStrICmp(name, area->name) == 0)
                        break;
                }
            } while (area != NULL);
            strcpy(aieditor->selected_area->name, name);
        }
    } else if (pad->digital_buttons_pressed & 0x10) {
        if (aieditor->selected_area != NULL && aieditor->selected_area == aieditor->hovered_area)
            return areaEditorDeleteMenu();
    } else if (pad->digital_buttons_pressed & 0x100) {
        aieditor->selected_area = areaEditorFindHover(0);
        if (aieditor->selected_area != NULL)
            edcamSetPos(&aieditor->selected_area->position);
    } else if (pad->digital_buttons & 0x2000) {
        if (aieditor->selected_area != NULL && aieditor->selected_area == aieditor->hovered_area)
            aieditorsettings.locator_rotation = (i16)aieditor->selected_area->rotation;
        if (pad->digital_buttons_pressed & 0x8000) {
            aieditor->rotation_repeat = 20;
        } else {
            aieditor->rotation_repeat += 20;
            if (aieditor->rotation_repeat > 600)
                aieditor->rotation_repeat = 600;
        }
        aieditorsettings.locator_rotation = NuAngAdd(aieditorsettings.locator_rotation, aieditor->rotation_repeat);
        if (aieditor->selected_area != NULL && aieditor->selected_area == aieditor->hovered_area)
            aieditor->selected_area->rotation = (u16)aieditorsettings.locator_rotation;
    } else if (pad->digital_buttons & 0x8000) {
        if (aieditor->selected_area != NULL && aieditor->selected_area == aieditor->hovered_area)
            aieditorsettings.locator_rotation = (i16)aieditor->selected_area->rotation;
        if (pad->digital_buttons_pressed & 0x2000) {
            aieditor->rotation_repeat = 20;
        } else {
            aieditor->rotation_repeat += 20;
            if (aieditor->rotation_repeat > 600)
                aieditor->rotation_repeat = 600;
        }
        aieditorsettings.locator_rotation = NuAngSub(aieditorsettings.locator_rotation, aieditor->rotation_repeat);
        if (aieditor->selected_area != NULL && aieditor->selected_area == aieditor->hovered_area)
            aieditor->selected_area->rotation = (u16)aieditorsettings.locator_rotation;
    } else if (pad->digital_buttons & 0x4000) {
        if (aieditor->selected_area != NULL && aieditor->selected_area == aieditor->hovered_area)
            aieditor->selected_area->size.y *= (f64)0.99f;
    } else if (pad->digital_buttons & 0x1000) {
        if (aieditor->selected_area != NULL && aieditor->selected_area == aieditor->hovered_area)
            aieditor->selected_area->size.y *= (f64)1.01f;
    } else if ((pad->digital_buttons & 0x100) && (pad->digital_buttons_pressed & 8)) {
        if (aieditor->selected_area != NULL)
            aieditor->selected_area =
                (EDAIAREA_s *)NuLinkedListGetNext(&aieditor->areas, &aieditor->selected_area->link);
        if (aieditor->selected_area == NULL)
            aieditor->selected_area = (EDAIAREA_s *)NuLinkedListGetHead(&aieditor->areas);
        if (aieditor->selected_area != NULL)
            edcamSetPos(&aieditor->selected_area->position);
    } else if ((pad->digital_buttons & 0x100) && (pad->digital_buttons_pressed & 2)) {
        if (aieditor->selected_area != NULL)
            aieditor->selected_area =
                (EDAIAREA_s *)NuLinkedListGetPrev(&aieditor->areas, &aieditor->selected_area->link);
        if (aieditor->selected_area == NULL)
            aieditor->selected_area = (EDAIAREA_s *)NuLinkedListGetTail(&aieditor->areas);
        if (aieditor->selected_area != NULL)
            edcamSetPos(&aieditor->selected_area->position);
    }
done:
    if (aieditor->selected_area != NULL && (aieditor->selected_area->flags & 1) &&
        aieditor->selected_area->size.x != aieditor->selected_area->size.z) {
        size = aieditor->selected_area->size.z > aieditor->selected_area->size.x ? aieditor->selected_area->size.x
                                                                                 : aieditor->selected_area->size.z;
        aieditor->selected_area->size.x = size;
    }
    return NULL;
}

eduimenu_s *__cdecl locatorEditor_MainMenu(void) {
    eduimenu_s *menu = eduiMenuCreate(200, 70, 240, 270, ed_fnt, AiEditorMessageCloseMenuPc, "Options");
    if (menu == NULL)
        return NULL;
    eduiMenuAddItem(menu, eduiItemSelCreate(AiEditorRouteModeIdPc, &AiEditorSaveMessageColoursPc, 0, 0,
                                            AiEditorModeMenuPc, "Select Editor Mode"));
    eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, aieditor_cbSave, "Save AI Data"));
    eduiMenuAddItem(
        menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, AiEditorCameraToPlayerPc, "Go To Player"));
    eduiMenuAddItem(menu,
                    eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, AiEditorMovePlayersPc, "Move Player"));
    if (aieditor->current_locator != NULL && AIScriptNameFromIx(aieditor->ai_system, 0) != NULL)
        eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0,
                                                locatorEditor_cbRenameLocatorMenu, "Rename Locator"));
    eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, locatorEditor_cbCreateLocatorSet,
                                            "Create Locator Set"));
    if (NuLinkedListGetHead(&aieditor->locator_sets) != NULL) {
        eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0,
                                                locatorEditor_cbSelectLocatorSet, "Select Locator Set"));
        if (aieditor->current_locator_set != NULL) {
            eduiMenuAddItem(menu, eduiItemSelCreate(0, &AiEditorSaveMessageColoursPc, 0, 0,
                                                    locatorEditor_cbDeleteLocatorSet, "Delete Locator Set"));
            eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0,
                                                    locatorEditor_cbRenameLocatorSetMenu, "Rename Locator Set"));
            eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0,
                                                    locatorEditor_cbAddLocatorsByName, "Add Locators To Set By Name"));
            eduiMenuAddItem(menu, eduiItemSelCreate(0, &AiEditorSaveMessageColoursPc, 0, 0,
                                                    locatorEditor_cbEmptyLocatorSet, "Empty Locator Set"));
        }
    }
    eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc,
                                               (i32)(aieditorsettings.display_options << 25) >> 31, 3,
                                               AiEditorDisplayBit6Pc, "Stop Platforms"));
    eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc,
                                               (i32)(aieditorsettings.display_options << 27) >> 31, 2,
                                               AiEditorDisplayBit4Pc, "Snap Height"));
    return menu;
}

eduimenu_s *__cdecl locatorEditor_Process(FRAME_PAD_PC *pad) {
    char base[32];
    i32 angle;
    EDAICREATURE_s *hover_creature;
    EDLOCATOR_s *hover_locator;
    if ((pad->digital_buttons_pressed & 0x80) != 0)
        return locatorEditor_MainMenu();
    if (NuStrLen(aieditor->pending_locator_name) != 0) {
        if (aieditor->current_locator_set != NULL) {
            char title[128];
            eduimenu_s *menu;
            sprintf(title, "Add locators \"%s\" to set \"%s\"?", aieditor->pending_locator_name,
                    aieditor->current_locator_set->name);
            menu = eduiMenuCreate(100, 70, 440, 270, ed_fnt, AiEditorMessageCloseMenuPc, title);
            if (menu != NULL) {
                eduiMenuAddItem(menu,
                                eduiItemSelCreate(0, &AiEditorSaveMessageColoursPc, 0, 0,
                                                  locatorEditor_cbAddLocatorsByNameYesNo, (char *)MenuPcString75f2c0));
                eduiMenuAddItem(menu,
                                eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0,
                                                  locatorEditor_cbAddLocatorsByNameYesNo, (char *)MenuPcString75f2c4));
                return menu;
            }
        }
        memset(aieditor->pending_locator_name, 0, sizeof(aieditor->pending_locator_name));
    }
    if ((pad->digital_buttons & 0x40) != 0) {
        if (aieditor->nearest_locator != NULL) {
            if ((pad->digital_buttons_pressed & 0x40) != 0) {
                aieditor->current_locator = aieditor->nearest_locator;
                aieditor->current_path = aieditor->current_locator->path_check.path;
                aieditorsettings.locator_rotation = aieditor->current_locator->direction;
                edcamSetPos(&aieditor->current_locator->position);
            } else if (aieditor->current_locator != NULL && aieditor->cursor_check.on_path != 0) {
                aieditor->current_locator->position = aieditor->camera_position;
                aieditor->current_locator->path_check = aieditor->cursor_check;
                angle = NuAngSub(aieditor->current_locator->direction, aieditor->cursor_check.angle);
                aieditor->current_locator->path_check.angle = angle;
            } else if (aieditor->current_locator != NULL) {
                edcamSetPos(&aieditor->current_locator->position);
            }
        } else if ((pad->digital_buttons_pressed & 0x40) != 0 && aieditor->cursor_check.on_path != 0) {
            EDLOCATOR_s *previous;
            EDLOCATOR_s *created;
            char *suffix;
            if (aieditor->current_locator_set != NULL && NuStrLen(aieditor->current_locator_set->name) != 0) {
                NuStrCpy(base, aieditor->current_locator_set->name);
            } else if (aieditor->current_locator != NULL) {
                NuStrCpy(base, aieditor->current_locator->name);
            } else {
                NuStrCpy(base, "Locator");
                goto create_locator;
            }
            suffix = strrchr(base, '_');
            if (suffix != NULL)
                *suffix = 0;
        create_locator:
            previous = aieditor->current_locator;
            created = AiEditorCreateLocatorPc(&aieditor->camera_position, aieditorsettings.locator_rotation);
            aieditor->current_locator = created;
            if (aieditor->current_locator != NULL) {
                char name[16];
                i32 index = 0;
                EDLOCATOR_s *other;
                do {
                    sprintf(name, "%s_%d", base, ++index);
                    other = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators);
                    while (other != NULL && NuStrICmp(name, other->name) != 0)
                        other = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &other->link);
                } while (other != NULL);
                strcpy(aieditor->current_locator->name, name);
                aieditor->current_locator->path_check.path = aieditor->current_path;
                aieditor->current_locator->path_check = aieditor->cursor_check;
                angle = NuAngSub(aieditor->current_locator->direction, aieditor->cursor_check.angle);
                aieditor->current_locator->path_check.angle = angle;
                AiEditorAddLocatorToSetPc(aieditor->current_locator_set, aieditor->current_locator,
                                          previous != aieditor->current_locator ? previous : NULL);
            }
        }
    } else if ((pad->digital_buttons_pressed & 0x10) != 0) {
        if (aieditor->current_locator != NULL && aieditor->current_locator == aieditor->nearest_locator)
            return locatorEditor_DeleteLocatorMenu();
    } else if ((pad->digital_buttons & 0x100) != 0) {
        EDLOCATOR_s *next;
        if ((pad->digital_buttons_pressed & 8) != 0) {
            if (aieditor->current_locator_set != NULL) {
                next = AiEditorNextLocatorInSetPc(aieditor->current_locator_set, aieditor->current_locator);
                aieditor->current_locator = next;
            } else {
                if (aieditor->current_locator != NULL) {
                    next = (EDLOCATOR_s *)NuLinkedListGetNext(&aieditor->locators, &aieditor->current_locator->link);
                    aieditor->current_locator = next;
                }
                if (aieditor->current_locator == NULL) {
                    next = (EDLOCATOR_s *)NuLinkedListGetHead(&aieditor->locators);
                    aieditor->current_locator = next;
                }
            }
        } else if ((pad->digital_buttons_pressed & 2) != 0) {
            if (aieditor->current_locator_set != NULL) {
                next = AiEditorPrevLocatorInSetPc(aieditor->current_locator_set, aieditor->current_locator);
                aieditor->current_locator = next;
            } else {
                if (aieditor->current_locator != NULL) {
                    next = (EDLOCATOR_s *)NuLinkedListGetPrev(&aieditor->locators, &aieditor->current_locator->link);
                    aieditor->current_locator = next;
                }
                if (aieditor->current_locator == NULL) {
                    next = (EDLOCATOR_s *)NuLinkedListGetTail(&aieditor->locators);
                    aieditor->current_locator = next;
                }
            }
        } else if ((pad->digital_buttons_pressed & 0x100) != 0) {
            next = locatorEditor_GetNearest(0);
            aieditor->current_locator = next;
        } else {
            goto refresh_hover;
        }
        if (aieditor->current_locator != NULL) {
            aieditor->current_path = aieditor->current_locator->path_check.path;
            edcamSetPos(&aieditor->current_locator->position);
        }
    } else if ((pad->digital_buttons & 0x2000) != 0) {
        if (aieditor->current_locator != NULL && aieditor->current_locator == aieditor->nearest_locator)
            aieditorsettings.locator_rotation = aieditor->current_locator->direction;
        if ((pad->digital_buttons_pressed & 0x8000) != 0)
            aieditor->rotation_repeat = 20;
        else {
            aieditor->rotation_repeat += 20;
            if (aieditor->rotation_repeat > 600)
                aieditor->rotation_repeat = 600;
        }
        aieditorsettings.locator_rotation = NuAngAdd(aieditorsettings.locator_rotation, aieditor->rotation_repeat);
        if (aieditor->current_locator != NULL && aieditor->current_locator == aieditor->nearest_locator) {
            aieditor->current_locator->direction = aieditorsettings.locator_rotation;
            angle = NuAngSub(aieditor->current_locator->direction, aieditor->cursor_check.angle);
            aieditor->current_locator->path_check.angle = angle;
        }
    } else if ((pad->digital_buttons & 0x8000) != 0) {
        if (aieditor->current_locator != NULL && aieditor->current_locator == aieditor->nearest_locator)
            aieditorsettings.locator_rotation = aieditor->current_locator->direction;
        if ((pad->digital_buttons_pressed & 0x2000) != 0)
            aieditor->rotation_repeat = 20;
        else {
            aieditor->rotation_repeat += 20;
            if (aieditor->rotation_repeat > 600)
                aieditor->rotation_repeat = 600;
        }
        aieditorsettings.locator_rotation = NuAngSub(aieditorsettings.locator_rotation, aieditor->rotation_repeat);
        if (aieditor->current_locator != NULL && aieditor->current_locator == aieditor->nearest_locator) {
            aieditor->current_locator->direction = aieditorsettings.locator_rotation;
            angle = NuAngSub(aieditor->current_locator->direction, aieditor->cursor_check.angle);
            aieditor->current_locator->path_check.angle = angle;
        }
    } else if ((pad->digital_buttons & 0x4000) != 0) {
        i32 difference = NuAngSub(aieditorsettings.locator_rotation, aieditor->cursor_check.angle);
        i32 quadrant = difference < 0 ? (difference + 0x3fff) >> 14 : difference >> 14;
        i32 remainder = difference % 0x4000;
        if (remainder > 0x2000)
            ++quadrant;
        else if (remainder < -0x2000)
            --quadrant;
        aieditorsettings.locator_rotation = NuAngAdd(quadrant << 14, aieditor->cursor_check.angle);
        if (aieditor->current_locator != NULL && aieditor->current_locator == aieditor->nearest_locator) {
            aieditor->current_locator->direction = aieditorsettings.locator_rotation;
            angle = NuAngSub(aieditor->current_locator->direction, aieditor->cursor_check.angle);
            aieditor->current_locator->path_check.angle = angle;
        }
    } else if ((pad->digital_buttons & 0x20) != 0 && aieditor->current_locator_set != NULL &&
               (pad->digital_buttons_pressed & 0x20) != 0 && aieditor->nearest_locator != NULL) {
        EDLOCATORSET_s *set = aieditor->current_locator_set;
        EDLOCATOR_s *nearest = aieditor->nearest_locator;
        EDLOCATOR_s *before;
        i32 index;
        for (index = 0; index < 64; ++index) {
            if (set->locators[index] == nearest) {
                AiEditorRemoveLocatorFromSetPc(set, nearest);
                if (aieditor->current_locator == NULL || aieditor->current_locator == aieditor->nearest_locator)
                    goto refresh_hover;
                goto add_nearest;
            }
            if (set->locators[index] == NULL)
                break;
        }
    add_nearest:
        before = aieditor->current_locator != NULL && aieditor->current_locator != aieditor->nearest_locator
                     ? aieditor->current_locator
                     : NULL;
        if (AiEditorAddLocatorToSetPc(aieditor->current_locator_set, aieditor->nearest_locator, before) != 0) {
            aieditor->current_locator = aieditor->nearest_locator;
            aieditor->current_path = aieditor->current_locator->path_check.path;
            aieditorsettings.locator_rotation = aieditor->current_locator->direction;
            edcamSetPos(&aieditor->current_locator->position);
        }
    }
refresh_hover:
    hover_creature = creatureEditor_GetNearest(1);
    aieditor->nearest_creature = hover_creature;
    hover_locator = locatorEditor_GetNearest(1);
    aieditor->nearest_locator = hover_locator;
    return NULL;
}

eduimenu_s *__cdecl pathEditor_MainMenu(void) {
    eduimenu_s *menu = eduiMenuCreate(200, 70, 240, 270, ed_fnt, AiEditorMessageCloseMenuPc, "Options");
    EDAIPATHNODE_s *node;
    EDAIPATHNODE_s *other;
    EDAIPATHCNX_s *connection;
    i32 index;
    i32 row;
    if (menu != NULL) {
        eduiMenuAddItem(menu, eduiItemSelCreate(AiEditorRouteModeIdPc, &AiEditorSaveMessageColoursPc, 0, 0,
                                                AiEditorModeMenuPc, "Select Editor Mode"));
        if (aieditor->current_path != NULL && aieditor->current_path->current_node != NULL)
            eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, pathEditor_cbRenameNodeMenu,
                                                    "Rename Node"));
        eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, pathEditor_cbSelectPathMenu,
                                                "Select Path Network"));
        eduiMenuAddItem(menu,
                        eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, aieditor_cbSave, "Save AI Data"));
        eduiMenuAddItem(
            menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, AiEditorCameraToPlayerPc, "Go To Player"));
        eduiMenuAddItem(
            menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, AiEditorMovePlayersPc, "Move Player"));
        eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, pathEditor_cbCreatePath,
                                                "Create Path Network"));
        if (aieditor->current_path != NULL) {
            if (aieditor->current_path->current_node != NULL)
                eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0,
                                                        pathEditor_cbShareNodeMenu, "Share node with..."));
            if (aieditor->current_path != NULL && !(aieditor->current_path->flags & 1))
                eduiMenuAddItem(menu, eduiItemSelCreate(0, &AiEditorSaveMessageColoursPc, 0, 0, pathEditor_cbDeletePath,
                                                        "Delete Path Network"));
            if (aieditor->current_path != NULL && !(aieditor->current_path->flags & 1))
                eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0,
                                                        pathEditor_cbRenamePathMenu, "Rename Path Network"));
        }
        eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc,
                                                   (i32)(aieditorsettings.display_options << 24) >> 31, 1,
                                                   aieditor_cbSolidPathDisplayToggle, "Solid Path Display"));
        row = 2;
        if (aieditor->current_path != NULL) {
            node = aieditor->current_path->current_node;
            other = aieditor->current_path->other_node;
            connection = NULL;
            if (node != NULL && other != NULL) {
                for (index = 0; index < 8; ++index) {
                    if (node->connections[index].node == other) {
                        connection = &node->connections[index];
                        break;
                    }
                }
                {
                    if (connection != NULL && AIPathConnectionTypeCountPc != 0) {
                        eduiMenuAddItem(
                            menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, NULL, "================="));
                        for (index = 0; index < AIPathConnectionTypeCountPc; ++index) {
                            eduiMenuAddItem(menu, eduiItemToggleCreate(index, &AiEditorSaveMessageColoursPc,
                                                                       (AIPathConnectionTypesPc[index].connection_flag &
                                                                        connection->flags) != 0,
                                                                       row, pathEditor_cbCnxFlagsToggle,
                                                                       AIPathConnectionTypesPc[index].name));
                            ++row;
                        }
                        eduiMenuAddItem(
                            menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, NULL, "================="));
                    }
                }
            }
            if (aieditor->current_path->current_node != NULL) {
                eduiMenuAddItem(menu, eduiItemToggleCreate(0x80, &AiEditorSaveMessageColoursPc,
                                                           aieditor->current_path->current_node->flags >> 7, row,
                                                           pathEditor_cbNodeFlagsToggle, "Never On A Platform"));
                ++row;
            }
        }
        eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc,
                                                   (i32)(aieditorsettings.display_options << 25) >> 31, row,
                                                   AiEditorDisplayBit6Pc, "Stop Platforms"));
        ++row;
        eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc,
                                                   (i32)(aieditorsettings.display_options << 22) >> 31, row,
                                                   pathEditor_cbDrawWallsplinesToggle, "Draw Wallsplines"));
        ++row;
        eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc,
                                                   (i32)(aieditorsettings.display_options << 30) >> 31, row,
                                                   aieditor_cbDrawAllToggle, "Draw All Paths"));
        ++row;
        eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc,
                                                   (i32)(aieditorsettings.display_options << 27) >> 31, row,
                                                   AiEditorDisplayBit4Pc, "Snap Height"));
        ++row;
        eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc,
                                                   (i32)(aieditorsettings.display_options << 28) >> 31, row,
                                                   aieditor_cbShowCreaturesToggle, "Show Creatures"));
        ++row;
        eduiMenuAddItem(menu, eduiItemToggleCreate(1, &AiEditorSaveMessageColoursPc, near_clip_at_cursor, row,
                                                   cbNearClipAtCursor, "Near Clip At Cursor"));
        return menu;
    }
    return NULL;
}

eduimenu_s *__cdecl pathEditor_Process(FRAME_PAD_PC *pad) {
    EDAIPATH_s *path;
    EDAIPATHNODE_s *previous;
    EDAIPATHNODE_s *nearest;
    EDAIPATHNODE_s *node;
    EDAIPATHNODE_s *start;
    NUVEC position;
    f32 step;
    i32 named;
    if (pad->digital_buttons_pressed & 0x80)
        return pathEditor_MainMenu();
    if (aieditor->current_path == NULL)
        return NULL;
    previous = aieditor->current_path->current_node;
    if (!(aieditor->current_path->flags & 1))
        aieditorsettings.display_options &= ~1;
    if (aieditorsettings.display_options & 1) {
        aieditor->current_path->runtime_nearest = (i16)pathEditor_GetNearestRuntimeNode(aieditor->runtime_path);
        if ((pad->digital_buttons_pressed & 0x40) && aieditor->current_path->runtime_nearest >= 0)
            aieditor->current_path->runtime_start = aieditor->current_path->runtime_nearest;
        if ((pad->digital_buttons_pressed & 0x10) && aieditor->current_path->runtime_nearest >= 0)
            aieditor->current_path->runtime_end = aieditor->current_path->runtime_nearest;
        if (pad->digital_buttons_pressed & 0x100) {
            if (aieditor->current_path->runtime_nearest >= 0) {
                aieditor->current_path->runtime_start = aieditor->current_path->runtime_nearest;
                edcamSetPos(&aieditor->runtime_path->nodes[aieditor->current_path->runtime_nearest].position);
            }
            aieditor->current_path->current_node = pathEditor_GetNearestNode(aieditor->current_path, 0);
            if (aieditor->current_path->current_node != NULL)
                edcamSetPos(&aieditor->current_path->current_node->position);
        }
        return NULL;
    }
    if (pad->digital_buttons & 0x40) {
        nearest = aieditor->current_path->nearest_node;
        if (nearest != NULL) {
            if (pad->digital_buttons_pressed & 0x40) {
                aieditor->current_path->current_node = nearest;
                node = aieditor->current_path->current_node;
                if (node != NULL) {
                    position.x = node->position.x;
                    position.y = aieditor->cursor_position.y;
                    position.z = node->position.z;
                    edcamSetPos(&position);
                }
            } else if (aieditor->current_path->current_node != NULL &&
                       nearest == aieditor->current_path->current_node) {
                position = nearest->position;
                nearest->position = aieditor->camera_position;
                pathEditor_PathNodeMoved(aieditor->current_path->current_node);
                locatorEditor_PathNodeMoved(aieditor->current_path->current_node);
                if (AIPathNodeDeletedFn != NULL)
                    AIPathNodeDeletedFn(aieditor->current_path->current_node);
                if (pad->digital_buttons & 0x200) {
                    path = aieditor->current_path;
                    start = path->current_node;
                    position.x = start->position.x - position.x;
                    position.z = start->position.z - position.z;
                    for (node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&path->nodes); node != NULL;
                         node = (EDAIPATHNODE_s *)NuLinkedListGetNext(&aieditor->current_path->nodes, &node->link)) {
                        if (node != aieditor->current_path->current_node) {
                            node->position.x += position.x;
                            node->position.z = position.z + node->position.z;
                            locatorEditor_PathNodeMoved(node);
                            if (AIPathNodeDeletedFn != NULL)
                                AIPathNodeDeletedFn(node);
                        }
                    }
                }
            }
        } else if (pad->digital_buttons_pressed & 0x40) {
            aieditor->current_path->current_node = pathEditor_CreateNode(aieditor->current_path);
            if (previous != NULL) {
                if (aieditor->current_path->current_node != NULL) {
                    aieditor->current_path->current_node->radius = previous->radius;
                    aieditor->current_path->current_node->upper_height = previous->upper_height;
                    aieditor->current_path->current_node->lower_height = previous->lower_height;
                }
                if (!pathEditor_ConnectNodes(aieditor->current_path->current_node, previous)) {
                    pathEditor_DestroyNode(aieditor->current_path->current_node, aieditor->current_path);
                    aieditor->current_path->current_node = previous;
                }
            }
        }
    }
    if (pad->digital_buttons_pressed & 0x20) {
        node = aieditor->current_path->current_node;
        if (node != NULL) {
            nearest = aieditor->current_path->nearest_node;
            if (nearest != NULL && nearest != node) {
                if (pathEditor_FindConnection(node, nearest) != NULL)
                    return pathEditor_DisconnectNodeMenu();
                pathEditor_ConnectNodes(node, nearest);
                position.x = nearest->position.x;
                position.y = aieditor->cursor_position.y;
                position.z = nearest->position.z;
                edcamSetPos(&position);
            }
        }
    }
    if ((pad->digital_buttons_pressed & 0x10) && aieditor->current_path->current_node != NULL &&
        aieditor->current_path->current_node == aieditor->current_path->nearest_node)
        return pathEditor_DeleteNodeMenu();
    if (pad->digital_buttons_pressed & 0x100) {
        if (aieditorsettings.display_options & 2) {
            pathEditor_SelectNearestPath();
        } else {
            aieditor->current_path->current_node = pathEditor_GetNearestNode(aieditor->current_path, 0);
            if (aieditor->current_path->current_node != NULL) {
                if (edpath_addoffset.x != 0.0f || edpath_addoffset.y != 0.0f || edpath_addoffset.z != 0.0f) {
                    NuVecAdd(&aieditor->current_path->current_node->position,
                             &aieditor->current_path->current_node->position, &edpath_addoffset);
                    pathEditor_PathNodeMoved(aieditor->current_path->current_node);
                    locatorEditor_PathNodeMoved(aieditor->current_path->current_node);
                }
                edcamSetPos(&aieditor->current_path->current_node->position);
            }
        }
    }
    aieditor->flags &= ~1;
    if (aieditor->current_path->current_node != NULL &&
        aieditor->current_path->current_node == aieditor->current_path->nearest_node) {
        if (pad->digital_buttons & 0x8000 || pad->digital_buttons & 0x2000) {
            if (pad->digital_buttons & 0x8000)
                aieditor->current_path->current_node->radius =
                    aieditor->current_path->current_node->radius * (f64)0.99f;
            else
                aieditor->current_path->current_node->radius =
                    aieditor->current_path->current_node->radius * (f64)1.01f;
            if (aieditor->current_path->current_node->radius < 0.01f)
                aieditor->current_path->current_node->radius = 0.01f;
            pathEditor_PathNodeMoved(aieditor->current_path->current_node);
            locatorEditor_PathNodeMoved(aieditor->current_path->current_node);
            if (AIPathNodeDeletedFn != NULL)
                AIPathNodeDeletedFn(aieditor->current_path->current_node);
        }
        if (aieditorsettings.display_options & 0x80) {
            if (pad->digital_buttons & 0x1000) {
                aieditor->flags |= 4;
                step = aieditor->current_path->current_node->upper_height * (f64)0.05f;
                if (pad->digital_buttons & 4)
                    aieditor->current_path->current_node->upper_height += step;
                else if (pad->digital_buttons & 1)
                    aieditor->current_path->current_node->upper_height -= step;
                if (aieditor->current_path->current_node->upper_height < 0.01f)
                    aieditor->current_path->current_node->upper_height = 0.01f;
            } else if (pad->digital_buttons & 0x4000) {
                aieditor->flags |= 4;
                step = -aieditor->current_path->current_node->lower_height * (f64)0.05f;
                if (pad->digital_buttons & 4)
                    aieditor->current_path->current_node->lower_height += step;
                else if (pad->digital_buttons & 1)
                    aieditor->current_path->current_node->lower_height -= step;
                if (aieditor->current_path->current_node->lower_height > -0.01f)
                    aieditor->current_path->current_node->lower_height = -0.01f;
            }
        }
        aieditor->flags |= 1;
    }
    named = 0;
    if ((pad->digital_buttons & 0x100) && (pad->digital_buttons_pressed & 8))
        goto next_node;
    if ((pad->digital_buttons & 0x100) && (pad->digital_buttons_pressed & 2))
        goto previous_node;
    if ((pad->digital_buttons & 0x100) && (pad->digital_buttons_pressed & 4)) {
        named = 1;
        goto next_node;
    }
    if ((pad->digital_buttons & 0x100) && (pad->digital_buttons_pressed & 1)) {
        named = 1;
        goto previous_node;
    }
    if (aieditorsettings.display_options & 0x80)
        goto done;
    if (pad->digital_buttons_pressed & 0x1000)
        goto next_node;
    if (!(pad->digital_buttons_pressed & 0x4000))
        goto done;
previous_node:
    if (aieditor->current_path->current_node != NULL)
        aieditor->current_path->current_node = (EDAIPATHNODE_s *)NuLinkedListGetPrev(
            &aieditor->current_path->nodes, &aieditor->current_path->current_node->link);
    if (aieditor->current_path->current_node == NULL)
        aieditor->current_path->current_node = (EDAIPATHNODE_s *)NuLinkedListGetTail(&aieditor->current_path->nodes);
    if (named && aieditor->current_path->current_node != NULL &&
        NuStrLen(aieditor->current_path->current_node->name) == 0) {
        start = aieditor->current_path->current_node;
        do {
            aieditor->current_path->current_node = (EDAIPATHNODE_s *)NuLinkedListGetPrev(
                &aieditor->current_path->nodes, &aieditor->current_path->current_node->link);
            if (aieditor->current_path->current_node == NULL) {
                aieditor->current_path->current_node =
                    (EDAIPATHNODE_s *)NuLinkedListGetTail(&aieditor->current_path->nodes);
                if (aieditor->current_path->current_node == NULL)
                    break;
            }
            if (aieditor->current_path->current_node == start)
                break;
        } while (NuStrLen(aieditor->current_path->current_node->name) == 0);
    }
    goto set_camera;
next_node:
    if (aieditor->current_path->current_node != NULL)
        aieditor->current_path->current_node = (EDAIPATHNODE_s *)NuLinkedListGetNext(
            &aieditor->current_path->nodes, &aieditor->current_path->current_node->link);
    if (aieditor->current_path->current_node == NULL)
        aieditor->current_path->current_node = (EDAIPATHNODE_s *)NuLinkedListGetHead(&aieditor->current_path->nodes);
    if (named && aieditor->current_path->current_node != NULL &&
        NuStrLen(aieditor->current_path->current_node->name) == 0) {
        start = aieditor->current_path->current_node;
        do {
            aieditor->current_path->current_node = (EDAIPATHNODE_s *)NuLinkedListGetNext(
                &aieditor->current_path->nodes, &aieditor->current_path->current_node->link);
            if (aieditor->current_path->current_node == NULL) {
                aieditor->current_path->current_node =
                    (EDAIPATHNODE_s *)NuLinkedListGetHead(&aieditor->current_path->nodes);
                if (aieditor->current_path->current_node == NULL)
                    break;
            }
            if (aieditor->current_path->current_node == start)
                break;
        } while (NuStrLen(aieditor->current_path->current_node->name) == 0);
    }
set_camera:
    if (aieditor->current_path->current_node != NULL)
        edcamSetPos(&aieditor->current_path->current_node->position);
done:
    aieditor->current_path->nearest_node = pathEditor_GetNearestNode(aieditor->current_path, 1);
    return NULL;
}

eduimenu_s *__cdecl routeEditor_MainMenu(void) {
    eduimenu_s *menu = eduiMenuCreate(200, 70, 240, 270, ed_fnt, AiEditorMessageCloseMenuPc, "Options");
    if (menu != NULL) {
        eduiMenuAddItem(menu, eduiItemSelCreate(AiEditorRouteModeIdPc, &AiEditorSaveMessageColoursPc, 0, 0,
                                                AiEditorModeMenuPc, "Select Editor Mode"));
        eduiMenuAddItem(menu,
                        eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, aieditor_cbSave, "Save AI Data"));
        eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, routeEditor_cbCreateRoute,
                                                "Create New Route"));
        if (aieditor->current_path != NULL && aieditor->current_path->current_route != NULL) {
            eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0,
                                                    routeEditor_cbRenameRouteMenu, "Rename Current Route"));
            eduiMenuAddItem(menu, eduiItemSelCreate(0, &AiEditorSaveMessageColoursPc, 0, 0, routeEditor_cbDeleteRoute,
                                                    "Destroy Current Route"));
            eduiMenuAddItem(menu, eduiItemSelCreate(1, &AiEditorSaveMessageColoursPc, 0, 0, routeEditor_cbRouteUsers,
                                                    "Route Users"));
        }
        return menu;
    }
    return NULL;
}

eduimenu_s *__cdecl routeEditor_Process(FRAME_PAD_PC *pad) {
    EDAIPATHNODE_s *nearest;
    EDAIPATHNODE_s *node;
    NUVEC position;
    i32 index;
    i32 i;
    i32 change;
    if (pad->digital_buttons_pressed & 0x80)
        return routeEditor_MainMenu();
    if (aieditor->current_path == NULL)
        return NULL;
    if (pad->digital_buttons_pressed & 0x1000) {
        index = aieditor->current_path->current_route != NULL
                    ? aieditor->current_path->current_route - aieditor->current_path->routes
                    : 0;
        for (i = 0; i < 16; ++i) {
            if (++index >= 16)
                index = 0;
            aieditor->current_path->current_route = &aieditor->current_path->routes[index];
            if (aieditor->current_path->current_route->flags & 1)
                break;
            aieditor->current_path->current_route = NULL;
        }
    } else if (pad->digital_buttons_pressed & 0x4000) {
        index = aieditor->current_path->current_route != NULL
                    ? aieditor->current_path->current_route - aieditor->current_path->routes
                    : 0;
        for (i = 0; i < 16; ++i) {
            if (--index < 0)
                index = 15;
            aieditor->current_path->current_route = &aieditor->current_path->routes[index];
            if (aieditor->current_path->current_route->flags & 1)
                break;
            aieditor->current_path->current_route = NULL;
        }
    }
    if (aieditor->current_path->current_route == NULL)
        return NULL;
    if ((pad->digital_buttons & 0x40) && aieditor->current_path->nearest_node != NULL &&
        (pad->digital_buttons_pressed & 0x40)) {
        aieditor->current_path->current_node = aieditor->current_path->nearest_node;
        node = aieditor->current_path->current_node;
        if (node != NULL) {
            position.x = node->position.x;
            position.y = aieditor->cursor_position.y;
            position.z = node->position.z;
            edcamSetPos(&position);
        }
    }
    if (pad->digital_buttons_pressed & 0x20) {
        node = aieditor->current_path->current_node;
        nearest = aieditor->current_path->nearest_node;
        if (node != NULL && nearest != NULL && nearest != node) {
            change = routeEditor_AddToRoute(node, nearest);
            routeEditor_AddToRoute(nearest, aieditor->current_path->current_node);
            if (change == 1) {
                aieditor->current_path->current_node = nearest;
                node = aieditor->current_path->current_node;
                position.x = node->position.x;
                position.y = aieditor->cursor_position.y;
                position.z = node->position.z;
                edcamSetPos(&position);
            } else if (change == -1) {
                aieditor->current_path->current_node = nearest;
                node = aieditor->current_path->current_node;
                position.x = node->position.x;
                position.y = aieditor->cursor_position.y;
                position.z = node->position.z;
                edcamSetPos(&position);
            }
        }
    }
    if (pad->digital_buttons_pressed & 0x10) {
        node = aieditor->current_path->nearest_node;
        if (node != NULL) {
            index = aieditor->current_path->current_route - aieditor->current_path->routes;
            for (i = 0; i < 8; ++i) {
                if (node->connections[i].node != NULL && (node->connections[i].route_mask & ((u64)1 << index))) {
                    if (node->route_mask & ((u64)1 << index))
                        aieditor->current_path->nearest_node->route_mask &= ~(u16)((u64)1 << index);
                    else
                        aieditor->current_path->nearest_node->route_mask |= (u16)((u64)1 << index);
                    break;
                }
            }
        }
    }
    if (pad->digital_buttons_pressed & 0x100) {
        aieditor->current_path->current_node = pathEditor_GetNearestNode(aieditor->current_path, 0);
        if (aieditor->current_path->current_node != NULL)
            edcamSetPos(&aieditor->current_path->current_node->position);
    }
    aieditor->current_path->nearest_node = pathEditor_GetNearestNode(aieditor->current_path, 1);
    aieditor->flags &= ~1;
    if (aieditor->current_path->current_node != NULL &&
        aieditor->current_path->current_node == aieditor->current_path->nearest_node)
        aieditor->flags |= 1;
    return NULL;
}

void __cdecl aieditor_Init(AIEDITOR_RENDER_STATE *state, void **scratch_cursor, void **scratch_end) {
    aieditor = state;
    memset(&aieditorsettings, 0, sizeof(aieditorsettings));
    aieditorsettings.path_material = NuMtlCreate3D(1);
    aieditorsettings.path_material->diffuse_r = 0.5f;
    aieditorsettings.path_material->diffuse_g = 0.5f;
    aieditorsettings.path_material->diffuse_b = 0.5f;
    aieditorsettings.path_material->attribs.cull_mode = 2;
    aieditorsettings.path_material->attribs.z_mode = 0;
    aieditorsettings.path_material->opacity = 1.0f;
    aieditorsettings.path_material->attribs.alpha_mode = 0;
    NuMtlUpdate(aieditorsettings.path_material);
    aieditorsettings.route_material = NuMtlCreate3D(1);
    aieditorsettings.route_material->diffuse_r = 0.5f;
    aieditorsettings.route_material->diffuse_g = 0.5f;
    aieditorsettings.route_material->diffuse_b = 0.5f;
    aieditorsettings.route_material->attribs.cull_mode = 2;
    aieditorsettings.route_material->attribs.z_mode = 0;
    aieditorsettings.route_material->opacity = 1.0f;
    aieditorsettings.route_material->attribs.alpha_mode = 2;
    NuMtlUpdate(aieditorsettings.route_material);
    aieditorsettings.overlay_material = NuMtlCreate(1);
    aieditorsettings.overlay_material->diffuse_r = 0.5f;
    aieditorsettings.overlay_material->diffuse_g = 0.5f;
    aieditorsettings.overlay_material->diffuse_b = 0.5f;
    aieditorsettings.overlay_material->attribs.cull_mode = 2;
    aieditorsettings.overlay_material->attribs.z_mode = 3;
    aieditorsettings.overlay_material->opacity = 1.0f;
    aieditorsettings.overlay_material->attribs.alpha_mode = 0;
    NuMtlUpdate(aieditorsettings.overlay_material);
    aieditorsettings.height_offset = 0.01f;
    aieditorsettings.script_params[0] = 0.0f;
    aieditorsettings.script_params[1] = 0.0f;
    aieditorsettings.script_params[2] = 0.0f;
    aieditorsettings.script_params[3] = 0.0f;
    aieditorsettings.display_options = (aieditorsettings.display_options & 0xfffffb5c) | 0x318;
    aieditorsettings.character_type = -1;
    aieditorsettings.script_flags = 0;
    aieditorsettings.modes.count = 0;
    memset(aieditorsettings.modes.entries, 0, sizeof(aieditorsettings.modes.entries));
    AIEDITOR_REGISTER_PC(AiEditorPathsModeIdPc, "AIEDITOR_PATHS", pathEditor_Enter, pathEditor_Process,
                         pathEditor_Render);
    AIEDITOR_REGISTER_PC(AiEditorRouteModeIdPc, "AIEDITOR_ROUTES", NULL, routeEditor_Process, routeEditor_Render);
    AIEDITOR_REGISTER_PC(AiEditorAreasModeIdPc, "AIEDITOR_AREAS", areaEditor_Enter, areaEditor_Process,
                         areaEditor_Render);
    AIEDITOR_REGISTER_PC(AiEditorLocatorsModeIdPc, "AIEDITOR_LOCATORS", locatorEditor_Enter, locatorEditor_Process,
                         locatorEditor_Render);
    AIEDITOR_REGISTER_PC(AiEditorCreaturesModeIdPc, "AIEDITOR_CREATURES", creatureEditor_Enter, creatureEditor_Process,
                         creatureEditor_Render);
    AIEDITOR_REGISTER_PC(AiEditorAntinodeModeIdPc, "AIEDITOR_ANTINODES", antinodeEditor_Enter, antinodeEditor_Process,
                         antinodeEditor_Render);
    aieditor_RegisterDefaultPathCnxTypes();
    aieditorsettings.path_scratch_cursor = *scratch_cursor;
    aieditorsettings.path_scratch_end = *scratch_end;
}

/* Empty trace hook called from AINet::Receive (message.cpp). */
void AINetTrace(const char *format, ...) {
}
