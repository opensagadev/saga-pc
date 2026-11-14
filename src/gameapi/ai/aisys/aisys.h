#ifndef SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "editor/edpath.h"
#include "editor/edpath_types.h"
#include "gameapi/ai/aisys/aipath.h"
#include "legoapi/ai/core/gameai.h"
#include "legogame/batman_gameai.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 9 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART == 1 && !defined(SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP)

struct AIPACKET_PC;
struct AISCRIPTPROCESS_PC;
struct AISCRIPT_PC;
struct AISTATE_PC;
struct APIOBJECT_PC;
struct NUGSCN_s;
struct eduimenu_s;
struct gameobject_s;

typedef struct AIPATHNODE_PC AIPATHNODE_PC;
typedef struct AIPATHROUTE_PC AIPATHROUTE_PC;

typedef struct AIPATHCNX_PC {
    u32 traversal_flags[2];          // 0x00
    u32 original_traversal_flags[2]; // 0x08
    u8 node_indices[2];              // 0x10
    i16 rotation;                    // 0x12
    u16 route_mask;                  // 0x14
    u8 open;                         // 0x16
    u8 last_search_checksum;         // 0x17
    f32 distance;                    // 0x18
    f32 horizontal_distance;         // 0x1c
    f32 max_horizontal_distance;     // 0x20
} AIPATHCNX_PC;                      // size 0x24

struct AIPATHROUTE_PC {
    char *name;            // 0x00
    u8 *node_routes;       // 0x04
    u8 *node_directions;   // 0x08
    u8 *exit_nodes;        // 0x0c
    u8 route_count;        // 0x10
    u8 exit_node_count;    // 0x11
    u8 padding_12[2];      // 0x12
    u8 **route_nodes;      // 0x14
    u32 character_mask[4]; // 0x18
}; // size 0x28

typedef struct AIPATHNODELINK_PC {
    u8 node_index;           // 0x0
    u8 unknown_1;            // 0x1
    i16 special_route_index; // 0x2
} AIPATHNODELINK_PC;         // size 0x4
typedef struct AISYS_PC AISYS_PC;

typedef struct AIAREA_PC {
    char name[0x10];   // 0x00
    NUVEC position;    // 0x10
    f32 half_width;    // 0x1c
    f32 height;        // 0x20
    f32 half_depth;    // 0x24
    i16 rotation;      // 0x28
    u8 runtime_flags;  // 0x2a
    u8 game_flags;     // 0x2b
    u8 unknown_2c[12]; // 0x2c
    AISYS_PC *system;  // 0x38
    u8 unknown_3c[4];  // 0x3c
} AIAREA_PC;           // size 0x40 (Android: 0x3c)

typedef struct AIROW_PC {
    AIPATHINFO_PC path_info;       // 0x00
    NUVEC position;                // 0x18
    i32 rotation;                  // 0x24
    AIPATHCNX_PC *next_connection; // 0x28
    u8 next_direction;             // 0x2c
    u8 is_alive;                   // 0x2d
    u8 unknown_2e[2];              // 0x2e

    union {        // 0x30
        u32 flags; // 0x30

        struct {                      // 0x30
            u32 is_clockwise : 1;     // 0x30:0
            u32 is_turning : 1;       // 0x30:1
            u32 flags_bits_2_31 : 30; // 0x30:2
        };
    };
} AIROW_PC; // size 0x34

typedef struct AICREATURE_PC {
    char name[16];            // 0x00
    char script_name[16];     // 0x10
    NUVEC position;           // 0x20
    i32 y_rotation;           // 0x2c
    AIPATHINFO_PC path_info;  // 0x30
    i32 flags;                // 0x48
    u8 set;                   // 0x4c
    u8 unknown_4d[0x1];       // 0x4d
    i16 type;                 // 0x4e
    u8 count;                 // 0x50
    u8 count_across;          // 0x51
    u8 unknown_52[0x6];       // 0x52
    u32 active_mask;          // 0x58
    f32 x_spacing;            // 0x5c
    f32 z_spacing;            // 0x60
    f32 script_params[4];     // 0x64
    AIAREA_PC *activate_area; // 0x74
    void *area;               // 0x78
    void *locator;            // 0x7c
    void *respawn_locator;    // 0x80
    u8 activation_difficulty; // 0x84
    i8 min_respawn_count;     // 0x85
    i8 max_respawn_count;     // 0x86
    u8 activation_type;       // 0x87
    f32 min_respawn_time;     // 0x88
    f32 max_respawn_time;     // 0x8c
    f32 start_stagger;        // 0x90
    f32 view_distance;        // 0x94
    f32 hear_distance;        // 0x98
    f32 max_view_height;      // 0x9c
    f32 min_view_height;      // 0xa0
} AICREATURE_PC;              // size 0xa4
#ifdef __cplusplus
extern "C" {
#endif
    extern const char AIScriptListPathFormat[14];
    extern const char AIScriptFileExtension[5];
    extern const f64 NuAnimDataFloatMax;
    extern i32 new_retreat;
    extern f32 ai_moveradius;
    extern f32 AIMovementInstructionPositiveThreshold;
    extern f32 AIMovementInstructionNegativeThreshold;
    extern const char AIActionRandomName[7];
    extern const char AIActionMinTimeName[8];
    extern const char AIActionMaxTimeName[8];
    extern const char AIActionFramesName[7];
    extern const char AIActionIncrementName[5];
    extern const char AIActionDecrementName[5];
    extern const char AIActionStateName[6];
    extern const char AIPathAnticlockwiseName[14];
    extern const char AIPathClockwiseName[10];
    extern const char AIPathReverseName[8];
    extern const char AIPathRandomName[7];
    extern const char AIPathFacingName[7];
    extern const char AIPathCurrentDistanceName[12];
    extern const char AIPathCurrentPositionName[17];
    extern const char AIPathCanGoOffPathName[16];
    extern const char AIPathFaceName[5];
    extern const char AIPathWaitTimeName[9];
    extern const char AIPathXZRangeCheckName[14];
    extern i32 AIActionDiagnosticMode;
    extern const char AIActionToName[3];
    extern const char AIActionBothWaysName[9];
    extern const char AIActionFromName[5];
    extern const char AIActionOpenName[5];
    extern const char AIActionLengthName[7];
    extern const char AIPathMustReachDestinationName[23];
    extern const char AIPathIgnorePathName[12];
    extern const char AIPathFaceOpponentName[14];
    extern const char AIExpressionRandName[5];
    extern i32 AIPathDefaultChecks;
    void AIAntinodeCullSingleFrame(void);
    extern const char AIScriptKeywordConst[6];
    extern const char AIScriptKeywordParam[6];
    extern const char AIScriptKeywordState[6];
    extern const char AIScriptKeywordEnd[2];
    extern const char AIScriptKeywordReferenceScript[16];
    extern const char AIScriptKeywordActions[8];
    extern const char AIScriptKeywordConditions[11];
    extern const char AIScriptKeywordBase[5];
    extern const char AIScriptKeywordGlobal[7];
    extern const char AIScriptKeywordSource[7];
    extern const char AIScriptKeywordScript[7];
    extern const char AIScriptKeywordLevel[6];
    extern NULISTHDR global_aiscripts;
    void *AISysBufferAlloc(VARIPTR *cursor, VARIPTR *end, u32 size);
    extern VARIPTR *aiscript_load_buffer;
    extern NULISTHDR *aiscript_load_refs;
    extern NULISTHDR *aiscript_load_conditions;
    extern NULISTHDR *aiscript_load_actions;
    extern i32 condition_has_no_goto;
    extern f32 AIPathConnectionMargin;
    extern f32 antinode_time;
    extern f32 antinode_reverse_time;

    extern char *AiLevelPathName;
    extern i32 ai_usepackfile;
    extern u8 AiLoadActivationDifficultyPc;
    extern i8 AiLoadMinimumRespawnCountPc;
    extern i8 AiLoadMaximumRespawnCountPc;
    extern f32 AiLoadMinimumRespawnTimePc;
    extern f32 AiLoadMaximumRespawnTimePc;
    extern i32 (*GlobalCharacterTypeIDFn)(char *);
#ifdef __cplusplus
}
#endif
#undef SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP
#endif
#endif

#if SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART == 2 && !defined(SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP)

typedef f32 (*AICONDITION_EVAL_PC)(AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char *, void *);
typedef void *(*AICONDITION_INIT_PC)(AISYS_PC *, char *, struct AISCRIPT_PC *);

typedef struct AICONDITIONDEF_PC {
    const char *name;            // 0x0
    AICONDITION_EVAL_PC eval_fn; // 0x4
    AICONDITION_INIT_PC init_fn; // 0x8
} AICONDITIONDEF_PC;             // size 0xc

typedef i32 (*AIACTION_EVAL_PC)(AISYS_PC *, struct AISCRIPTPROCESS_PC *, struct AIPACKET_PC *, char **, i32, i32, f32);

typedef struct AIACTIONDEF_PC {
    const char *name;             // 0x0
    AIACTION_EVAL_PC eval_fn;     // 0x4
    u8 replicate_on_first_action; // 0x8
    u8 is_game_action;            // 0x9
    i16 index;                    // 0xa
} AIACTIONDEF_PC;                 // size 0xc
#ifdef __cplusplus
extern "C" {
#endif
    i32 AIScriptSetBaseScriptStateByName(struct AISCRIPTPROCESS_PC *, char *);
    extern struct AISCRIPTPROCESS_PC *AiSysStateDebugee;
    i32 AIScriptSetInterrupt(struct AISCRIPTPROCESS_PC *, u8, u8, char *, f32);
    extern struct AISCRIPTPROCESS_PC *current_processor;
    f32 AIParamToFloat(struct AISCRIPTPROCESS_PC *, char *);
#ifdef __cplusplus
}
#endif
#undef SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP
#endif
#endif

#if SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART == 3 && !defined(SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP)

struct AIPATHNODE_PC {
    char *name;                     // 0x00
    NUVEC position;                 // 0x04
    f32 radius;                     // 0x10
    f32 radius_squared;             // 0x14
    f32 min_height;                 // 0x18
    f32 min_height_offset;          // 0x1c
    f32 max_height;                 // 0x20
    f32 max_height_offset;          // 0x24
    u8 connection_count;            // 0x28
    u8 flags;                       // 0x29
    u8 has_special;                 // 0x2a
    u8 runtime_flags;               // 0x2b
    i16 path_flags;                 // 0x2c
    u8 distance_cache_nodes[2];     // 0x2e
    u8 special_route_index;         // 0x30
    u8 padding_31[3];               // 0x31
    AIPATHCNX_PC **connections;     // 0x34
    f32 distance_cache[2];          // 0x38
    NUSPECIALHANDLE special_handle; // 0x40
    NUVEC special_position;         // 0x4c
    u16 route_membership_mask;      // 0x58
    u16 route_boundary_mask;        // 0x5a
}; // size 0x5c

typedef struct AIPATH_PC {
    char name[0x10];                   // 0x00
    u8 node_count;                     // 0x10
    u8 flags;                          // 0x11
    u16 connection_count;              // 0x12
    u8 route_count;                    // 0x14
    u8 index;                          // 0x15
    u8 unknown_16[2];                  // 0x16
    u8 updated_node_bits[0x20];        // 0x18
    u8 previous_inside_node_bits[32];  // 0x38
    u8 inside_node_bits[0x20];         // 0x58
    u8 search_checksum;                // 0x78
    u8 search_reset_node;              // 0x79
    u8 special_route_count;            // 0x7a
    u8 unknown_7b;                     // 0x7b
    AIPATHNODE_PC *nodes;              // 0x7c
    AIPATHCNX_PC *connections;         // 0x80
    u8 **route_matrix;                 // 0x84
    AIPATHROUTE_PC *routes;            // 0x88
    AIPATHNODELINK_PC *special_routes; // 0x8c
    NUVEC bounds_min;                  // 0x90
    NUVEC bounds_max;                  // 0x9c
} AIPATH_PC;                           // size 0xa8

typedef struct AIPATHSPECIALROUTE_PC {
    u8 path_count;       // 0x0
    u8 unknown_1[3];     // 0x1
    AIPATH_PC **paths;   // 0x4
} AIPATHSPECIALROUTE_PC; // size 0x8

typedef struct AIPATHSYS_PC {
    u8 path_count;                         // 0x0
    u8 unknown_01[3];                      // 0x1
    AIPATH_PC **paths;                     // 0x4
    AIPATH_PC *active_path;                // 0x8
    AIPATHSPECIALROUTE_PC *special_routes; // 0xc
} AIPATHSYS_PC;                            // size 0x10

typedef struct AIANTINODE_PC {
    NUVEC position;                 // 0x00
    f32 radius;                     // 0x0c
    f32 min_y;                      // 0x10
    f32 min_y_offset;               // 0x14
    f32 max_y;                      // 0x18
    f32 max_y_offset;               // 0x1c
    NUSPECIALHANDLE special_handle; // 0x20
    NUVEC special_position;         // 0x2c
    i32 rotation;                   // 0x38
    i32 rotation_offset;            // 0x3c
    f32 base_radius;                // 0x40
    f32 base_height;                // 0x44
    u8 enabled;                     // 0x48
    u8 game_flags;                  // 0x49
    u8 type;                        // 0x4a
    u8 has_special;                 // 0x4b
    u8 unknown_4c_b[0x4];           // 0x4c
    u8 special_type;                // 0x50
    u8 unknown_51[0x7];             // 0x51
} AIANTINODE_PC;                    // size 0x58
#ifdef __cplusplus
extern "C" {
#endif
    extern AI_PARSE_SPEED_PC AIActionParseSpeedFn;
    extern AI_CHARACTER_DISTANCE_PC GetViewRangeFn;
    extern AI_CHARACTER_DISTANCE_PC GetHearDistanceFn;
    extern AI_CHARACTER_DISTANCE_PC GetMaxViewHeightFn;
    extern AI_CHARACTER_DISTANCE_PC GetMinViewHeightFn;
    extern AI_LEVEL_CHARACTER_TYPE_ID_PC SpecialRouteCharacterTypeIDFn;
    extern AIANTINODE_PC dynamic_antinodes[64];
    AIANTINODE_PC *AIAntinodeCreate(NUVEC *, f32);
    AIANTINODE_PC *AIAntinodeCreateSingleFrame(NUVEC *, f32);
    void AIAntinodeDestroy(AIANTINODE_PC *);
    void AIAntinodeMove(AIANTINODE_PC *, NUVEC *, f32, f32, f32);
    f32 AIPathNodeDistanceToPathNode(AIPATH_PC *, i32, i32, i32, u32);
    typedef i32 (*AISYS_SPECIAL_NODE_CALLBACK_PC)(void *, NUSPECIALHANDLE *);
    extern AISYS_SPECIAL_NODE_CALLBACK_PC AISysSpecialNodeCallback;
    i32 WithinConnection(void *, NUVEC *, AIPATH_PC *, AIPATHCNX_PC *, i32, AIPATHCNX_PC *, i32, i32, AIPATHINFO_PC *,
                         f32, i32);
    void AIPathNodeUpdatePos(void *, AIPATH_PC *, AIPATHNODE_PC *);
#ifdef __cplusplus
}
#endif
#undef SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP
#endif
#endif

#if SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART == 4 && !defined(SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP)

typedef struct APIOBJECT_PC *(*APIOBJECT_FROM_ID_PC)(u8);

typedef struct AILOCATORSET_PC {
    char name[0x10];     // 0x00
    i8 locator_count;    // 0x10
    u8 unknown_11[3];    // 0x11
    u8 *locator_entries; // 0x14
    u8 *assigned;        // 0x18
} AILOCATORSET_PC;       // size 0x1c

typedef struct AIGROUP_PC {
    struct APIOBJECT_PC *leader;      // 0x000
    i16 rotation_speed;               // 0x004
    u8 row_count;                     // 0x006
    u8 member_count;                  // 0x007
    u8 count_across;                  // 0x008
    u8 unknown_09[3];                 // 0x009
    struct APIOBJECT_PC *members[16]; // 0x00c
    i32 member_is_alive;              // 0x04c

    union {        // 0x050
        u32 flags; // 0x050

        struct {                      // 0x050
            u32 flags_bits_0_1 : 2;   // 0x050:0
            u32 is_reversed : 1;      // 0x050:2
            u32 flags_bit_3 : 1;      // 0x050:3
            u32 is_row_turning : 1;   // 0x050:4
            u32 flags_bits_5_31 : 27; // 0x050:5
        };
    };

    AIROW_PC rows[4]; // 0x054
    f32 radius;       // 0x124
    f32 x_spacing;    // 0x128
    f32 z_spacing;    // 0x12c
    f32 max_speed;    // 0x130
} AIGROUP_PC;         // size 0x134

struct AISYS_PC {
    void *storage;          // 0x0000
    VARIPTR storage_end;    // 0x0004
    VARIPTR storage_cursor; // 0x0008
    i32 storage_size;       // 0x000c
    AIPATHSYS_PC *path_sys; // 0x0010

    union {                 // 0x0014
        u8 unknown_14[4];   // 0x0014
        i32 creature_count; // 0x0014
    };

    AICREATURE_PC *creatures;      // 0x0018
    NULISTHDR scripts;             // 0x001c
    i32 locator_count;             // 0x0024
    AILOCATOR_PC *locators;        // 0x0028
    i32 locator_set_count;         // 0x002c
    AILOCATORSET_PC *locator_sets; // 0x0030
    i32 area_count;                // 0x0034
    AIAREA_PC *areas;              // 0x0038
    AIGROUP_PC groups[16];         // 0x003c
    i32 antinode_count;            // 0x137c
    AIANTINODE_PC *antinodes;      // 0x1380
    u8 next_area_check;            // 0x1384
    u8 goody_idx;                  // 0x1385

    union {                   // 0x1386
        u8 unknown_1386[0x2]; // 0x1386
        i16 reset_flags;      // 0x1386
    };

    void *game_sys;                // 0x1388
    struct APIOBJECT_PC *player_1; // 0x138c
    struct APIOBJECT_PC *player_2; // 0x1390
}; // size 0x1394

typedef void (*AI_SCRIPT_FIRST_ACTION_PC)(AISYS_PC *, struct AIPACKET_PC *, struct AISCRIPTPROCESS_PC *);

typedef i32 (*AI_GAME_PARAM_TO_FLOAT_PC)(struct AIPACKET_PC *, struct AISCRIPTPROCESS_PC *, char *, f32 *);
#ifdef __cplusplus
extern "C" {
#endif
    void DestroyAIGroup(AIGROUP_PC *);
    void AIScriptLoadAllPakFile(void *, char *, VARIPTR *, VARIPTR *, AISYS_PC *);
    typedef NUVEC *(*AI_ORIGIN_POSITION_PC)(AISYS_PC *, struct AIPACKET_PC *);
    extern AI_ORIGIN_POSITION_PC GetOriginPositionFn;
    f32 AIParamToFloatEx(struct AIPACKET_PC *, struct AISCRIPTPROCESS_PC *, char *);
    void AIScriptProcess(AISYS_PC *, struct APIOBJECT_PC *, struct AIPACKET_PC *, struct AISCRIPTPROCESS_PC *, f32);
    void AISysProcessCharacter(AISYS_PC *, struct APIOBJECT_PC *, struct AIPACKET_PC *, i32, f32, i32, i32);
    void InitFn_GameParamToFloat(AI_GAME_PARAM_TO_FLOAT_PC);
    extern AI_SCRIPT_FIRST_ACTION_PC ScriptProcessFirstTimeActionFn;
    extern struct AIPACKET_PC *current_packet;
    extern AI_GAME_PARAM_TO_FLOAT_PC GameParamToFloatFn;
    extern APIOBJECT_FROM_ID_PC APIOBJECTFromObjIDFn;
    void AISysProcess(AISYS_PC *, struct APIOBJECT_PC *, struct APIOBJECT_PC *);
    typedef f32 (*AI_CHARACTER_GOAL_SPEED_PC)(struct APIOBJECT_PC *);
    void AISysGetCharacterPathPos(AISYS_PC *, struct APIOBJECT_PC *, struct AIPACKET_PC *, i32, i32);
    void AIMoveInstruction(struct AIPACKET_PC *, NUVEC *, f32, AIPATHINFO_PC *, i32, f32);
    void AIFormationFollow(struct AIPACKET_PC *);
    void AISysUpdateCharacterPathPos(AISYS_PC *, struct APIOBJECT_PC *, struct AIPACKET_PC *, i32, f32);
    void InitFn_APIOBJECTFromObjIDFn(APIOBJECT_FROM_ID_PC);
    void AILocatorSet_CheckLocatorsStillAssigned(AISYS_PC *, AILOCATORSET_PC *);
    void AILocatorSet_AssignNearestLocator(AISYS_PC *, AILOCATORSET_PC *, struct APIOBJECT_PC *, f32, NUVEC *, NUVEC *,
                                           f32, i32);
    void AILocatorSet_AssignFurthestLocator(AISYS_PC *, AILOCATORSET_PC *, struct APIOBJECT_PC *, f32, NUVEC *, NUVEC *,
                                            f32, i32);
    void AILocatorSet_AssignRandomLocator(AISYS_PC *, AILOCATORSET_PC *, struct APIOBJECT_PC *, f32, NUVEC *, f32, i32);
    void AISysGetPathPosEx(AISYS_PC *, NUVEC *, AIPATHINFO_PC *, AIPATH_PC *, i32, i32, f32 *);
    void AISysGetPathPos(AISYS_PC *, NUVEC *, AIPATHINFO_PC *, AIPATH_PC *, i32);
    i32 AISysSetLevelPath(AISYS_PC *, char *);
    AILOCATOR_PC *AIPathFindLocator(AISYS_PC *, char *);
    AILOCATORSET_PC *AIPathFindLocatorSet(AISYS_PC *, char *);
    AIAREA_PC *AISysFindArea(AISYS_PC *, char *);
    AIPATH_PC *AISysFindPath(AISYS_PC *, char *);
    AIPATHNODE_PC *AIPathFindNode(AISYS_PC *, AIPATH_PC *, char *);
    void *AIPAthFindPathCnx(AISYS_PC *, AIPATH_PC *, char *, char *, i32 *);
    void AIScriptInitConditions(AISYS_PC *sys);
    typedef struct APIOBJECT_PC *(*GAMEOBJECT_LOOKUP_CALLBACK_PC)(void *, char *);
    extern GAMEOBJECT_LOOKUP_CALLBACK_PC GetNamedGameObjectCallback;
    void AISysCharacterSetPath(struct AIPACKET_PC *, AIPATH_PC *);
    void AISysFindRoute(struct AIPACKET_PC *);
    void AISysCharacterSetPathCnx(struct AIPACKET_PC *, NUVEC *, AIPATHCNX_PC *, i32);
    void LEGO_AISysCreatureInteraction2D(AISYS_PC *, i32, struct APIOBJECT_PC **, i32 *, f32);
    void AISysCreatureAntinodeInteraction(AISYS_PC *, i32, struct APIOBJECT_PC **, i32 *);
    void AIScriptLoadAll(char *, VARIPTR *, VARIPTR *, AISYS_PC *);
#ifdef __cplusplus
}
#endif
typedef i32 AIFINDALTERNATIVESPECIALOBJECT(AISYS_PC *system, NUSPECIALHANDLE *special);
#undef SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP
#endif
#endif

#if SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART == 5 && !defined(SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    struct AISCRIPT_PC *AIScriptFind(AISYS_PC *, const char *, i32, i32, i32);
    struct AISTATE_PC *AIStateFind(const char *, struct AISCRIPT_PC *);
    void AIScriptSetState(struct AISCRIPTPROCESS_PC *, struct AISTATE_PC *);
    void AIScriptProcessorInit(AISYS_PC *, struct AIPACKET_PC *, struct AISCRIPTPROCESS_PC *, AICREATURE_PC *, char *,
                               char *, i32, struct AISCRIPT_PC *, struct AISTATE_PC *);
    extern struct AISCRIPT_PC *aiscript_load_script;
    extern struct AISTATE_PC *aiscript_load_state;
    extern void (*GameAILoadFn)(AISYS_PC *, i32, struct NUGSCN_s *, VARIPTR *, VARIPTR *);
    void AIScriptOpenPakFileParse(struct AISCRIPT_PC **, void *, char *, char *, VARIPTR *, VARIPTR *);
#ifdef __cplusplus
}
#endif
#undef SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART 6
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP
#endif
#endif

#if SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART == 6 && !defined(SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern AI_BIG_JUMP_TO_DESTINATION_PC AIBigJumpToDestinationFn;

    extern AI_RESPAWN_ON_PATH_PC AIRespawnOnPathFn;
#ifdef __cplusplus
}
#endif
#undef SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART 7
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP
#endif
#endif

#if SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART == 7 && !defined(SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif

    extern AI_PREPARING_SPECIAL_MOVE_PC PreparingForSpecialMoveFn;
    extern AI_MID_SPECIAL_MOVE_PC MidSpecialMoveFn;

    void SetSpecialMove(struct gameobject_s *, AIPATHNODE_PC *, AIPATHNODE_PC *, i8);
    i32 TryToTeleportToNextNode(struct gameobject_s *, AIPATHNODE_PC *, i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART 8
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP
#endif
#endif

#if SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART == 8 && !defined(SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP)

typedef struct EDAICREATURE_s { /* Editor creature record; its area/locator references are editor records. */
    NULISTLNK link;
    char name[16];
    char script_name[16];
    NUVEC position;
    i32 angle;
    struct EDAIPATHCHECK_s check;
    u32 valid_positions;
    i16 character_type;
    u8 set;
    u8 group_count;
    u8 across_count;
    u8 unknown_5d[3];
    f32 x_spacing;
    f32 z_spacing;
    u32 flags;
    struct EDAIAREA_s *activation_area;
    f32 script_params[4];
    struct EDAIAREA_s *trigger_area;
    struct EDLOCATOR_s *locator;
    struct EDLOCATOR_s *respawn_locator;
    u8 difficulty;
    i8 min_respawns;
    i8 max_respawns;
    u8 activation;
    f32 min_respawn_time;
    f32 max_respawn_time;
    f32 stagger_start;
    f32 view_distance;
    f32 hear_distance;
    f32 max_view_height;
    f32 min_view_height;
} EDAICREATURE_s;

STATIC_ASSERT(sizeof(EDAICREATURE_s) == 172);
STATIC_ASSERT(offsetof(EDAICREATURE_s, angle) == 0x34);
STATIC_ASSERT(offsetof(EDAICREATURE_s, check.path) == 0x3c);
STATIC_ASSERT(offsetof(EDAICREATURE_s, character_type) == 0x58);
STATIC_ASSERT(offsetof(EDAICREATURE_s, script_params) == 0x70);
#ifdef __cplusplus
extern "C" {
#endif
    void creatureEditor_PathNodeMoved(struct EDAIPATHNODE_s *node);
#ifdef __cplusplus
}
#endif
#undef SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART 9
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP
#endif
#endif

#if SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART == 9 && !defined(SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void locatorEditor_cbCancelSelectLocatorSet(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void pathEditor_cbCancelSelectMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
#ifdef __cplusplus
}
#endif
#undef SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_PART 10
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP
#endif
#endif

#undef SAGA_GAMEAPI_AI_AISYS_AISYS_H_STOP
