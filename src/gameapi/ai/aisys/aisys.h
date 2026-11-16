#pragma once

#include "nu2api/nucore/common.h"

#include "editor/edpath.h"
#include "gameapi/ai/aisys/aipath.h"
#include "legoapi/ai/core/gameai.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/nuvec.h"

struct nuhspecial_s;

struct AIPACKET;
struct AISCRIPTPROCESS;
struct AISCRIPT;
struct AISTATE;
struct APIOBJECT;
struct NUGSCN_s;
struct eduimenu_s;
struct gameobject_s;

typedef struct AIPATHNODE AIPATHNODE;
typedef struct AIPATHROUTE AIPATHROUTE;

/* AIPATHCNX.traversal_flags: the low bits are the game's connection types (LEGO_AIPATHCNX_*, set per game at
 * start-up); the top bits are the AI editor's connection types (aieditor_RegisterPathCnxType) and runtime state. */
#define AIPATHCNX_TOO_LONG 0x08000000        /* longer than the connection's max_horizontal_distance */
#define AIPATHCNX_HIDDEN 0x10000000          /* an end node's special is hidden */
#define AIPATHCNX_LINK_OBSTACLE 0x20000000   /* "Link Obstacle" */
#define AIPATHCNX_PERMANENT_BLOCK 0x40000000 /* "Permanent Block" */
#define AIPATHCNX_TEMPORARY_BLOCK 0x80000000 /* "Temporary Block", also set and cleared by the BlockPath action */

/* AIPATHCNX.traversal_flags, low bits: each game's connection types, the values its GameAISysInit assigns to the
 * LEGO_AIPATHCNX_* variables. The AI code of each game (starwars_gameai.c, batarang.cpp, hub.cpp) tests its own. */
#define STARWARS_AIPATHCNX_JUMP 0x1
#define STARWARS_AIPATHCNX_R2D2GLIDE 0x4
#define STARWARS_AIPATHCNX_FORGOODIES 0x80
#define STARWARS_AIPATHCNX_FORBADDIES 0x100
#define STARWARS_AIPATHCNX_NO_DESTINATION_CHECK 0x200
#define STARWARS_AIPATHCNX_JUMP_NOW 0x400
#define STARWARS_AIPATHCNX_DONT_JUMP_NOW 0x800
#define STARWARS_AIPATHCNX_BIGJUMP 0x1000
#define STARWARS_AIPATHCNX_REQUIRESPERMISSION 0x2000
#define STARWARS_AIPATHCNX_BLOCKAGE 0x40000
#define STARWARS_AIPATHCNX_DONTTOGGLE 0x80000
#define STARWARS_AIPATHCNX_FULLTERRAIN 0x100000

#define BATMAN_AIPATHCNX_JUMP 0x1
#define BATMAN_AIPATHCNX_FORGOODIES 0x80
#define BATMAN_AIPATHCNX_FORBADDIES 0x100
#define BATMAN_AIPATHCNX_NO_DESTINATION_CHECK 0x200
#define BATMAN_AIPATHCNX_JUMP_NOW 0x400
#define BATMAN_AIPATHCNX_DONT_JUMP_NOW 0x800
#define BATMAN_AIPATHCNX_BIGJUMP 0x1000
#define BATMAN_AIPATHCNX_REQUIRESPERMISSION 0x2000
#define BATMAN_AIPATHCNX_BLOCKAGE 0x4000
#define BATMAN_AIPATHCNX_VARIANT_FLAG0 0x100000
#define BATMAN_AIPATHCNX_VARIANT_FLAG1 0x200000
#define BATMAN_AIPATHCNX_WALLSHUFFLE 0x400000

#define INDY_AIPATHCNX_JUMP 0x1
#define INDY_AIPATHCNX_BIGJUMP 0x4
#define INDY_AIPATHCNX_VARIANT_FLAG0 0x10
#define INDY_AIPATHCNX_NO_DESTINATION_CHECK 0x20
#define INDY_AIPATHCNX_JUMP_NOW 0x40
#define INDY_AIPATHCNX_DONT_JUMP_NOW 0x80
#define INDY_AIPATHCNX_FORGOODIES 0x100
#define INDY_AIPATHCNX_FORBADDIES 0x200

typedef struct AIPATHCNX {           // size 0x24
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
} AIPATHCNX;

struct AIPATHROUTE {       // size 0x28
    char *name;            // 0x00
    u8 *node_routes;       // 0x04
    u8 *node_directions;   // 0x08
    u8 *exit_nodes;        // 0x0c
    u8 route_count;        // 0x10
    u8 exit_node_count;    // 0x11
    u8 pad_12[2];          // 0x12
    u8 **route_nodes;      // 0x14
    u32 character_mask[4]; // 0x18
};

typedef struct AIPATHNODELINK { // size 0x4
    u8 node_index;              // 0x0
    u8 unknown_1;               // 0x1
    i16 special_route_index;    // 0x2
} AIPATHNODELINK;
typedef struct AISYS AISYS;

typedef struct AIAREA { // size 0x40
    char name[0x10];    // 0x00
    NUVEC position;     // 0x10
    f32 half_width;     // 0x1c
    f32 height;         // 0x20
    f32 half_depth;     // 0x24
    i16 rotation;       // 0x28
    u8 runtime_flags;   // 0x2a
    u8 game_flags;      // 0x2b
    u8 unknown_2c[12];  // 0x2c
    AISYS *system;      // 0x38
    u8 unknown_3c[4];   // 0x3c
} AIAREA;

typedef struct AIROW {          // size 0x34
    AIPATHINFO path_info;       // 0x00
    NUVEC position;             // 0x18
    i32 rotation;               // 0x24
    AIPATHCNX *next_connection; // 0x28
    u8 next_direction;          // 0x2c
    u8 is_alive;                // 0x2d
    u8 unknown_2e[2];           // 0x2e

    union { // size 0x4
        u32 flags;

        struct {                      // size 0x4
            u32 is_clockwise : 1;     // 0x30:0
            u32 is_turning : 1;       // 0x30:1
            u32 flags_bits_2_31 : 30; // 0x30:2
        };
    }; // 0x30
} AIROW;

typedef struct AICREATURE {   // size 0xa4
    char name[16];            // 0x00
    char script_name[16];     // 0x10
    NUVEC position;           // 0x20
    i32 y_rotation;           // 0x2c
    AIPATHINFO path_info;     // 0x30
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
    AIAREA *activate_area;    // 0x74
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
} AICREATURE;
#ifdef __cplusplus
extern "C" {
#endif
    extern const char AIScriptListPathFormat[14];
    extern const f64 NuAnimDataFloatMax;
    extern i32 new_retreat;
    extern f32 ai_moveradius;
    extern f32 AIMovementInstructionPositiveThreshold;
    extern f32 AIMovementInstructionNegativeThreshold;
    extern const char AIPathRandomName[7];
    extern const char AIPathCurrentPositionName[17];
    extern i32 AIActionDiagnosticMode;
    extern const char AIActionBothWaysName[9];
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
    extern const char AIScriptKeywordActions[8];
    extern const char AIScriptKeywordConditions[11];
    extern const char AIScriptKeywordGlobal[7];
    extern const char AIScriptKeywordSource[7];
    extern const char AIScriptKeywordScript[7];
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
    extern u8 AiLoadActivationDifficulty;
    extern i8 AiLoadMinimumRespawnCount;
    extern i8 AiLoadMaximumRespawnCount;
    extern f32 AiLoadMinimumRespawnTime;
    extern f32 AiLoadMaximumRespawnTime;
    extern i32 (*GlobalCharacterTypeIDFn)(char *);
#ifdef __cplusplus
}
#endif

typedef f32 (*AICONDITION_EVAL)(AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char *, void *);
typedef void *(*AICONDITION_INIT)(AISYS *, char *, struct AISCRIPT *);

typedef struct AICONDITIONDEF { // size 0xc
    const char *name;           // 0x0
    AICONDITION_EVAL eval_fn;   // 0x4
    AICONDITION_INIT init_fn;   // 0x8
} AICONDITIONDEF;

typedef i32 (*AIACTION_EVAL)(AISYS *, struct AISCRIPTPROCESS *, struct AIPACKET *, char **, i32, i32, f32);

typedef struct AIACTIONDEF {      // size 0xc
    const char *name;             // 0x0
    AIACTION_EVAL eval_fn;        // 0x4
    u8 replicate_on_first_action; // 0x8
    u8 is_game_action;            // 0x9
    i16 index;                    // 0xa
} AIACTIONDEF;
#ifdef __cplusplus
extern "C" {
#endif
    i32 AIScriptSetBaseScriptStateByName(struct AISCRIPTPROCESS *, char *);
    extern struct AISCRIPTPROCESS *AiSysStateDebugee;
    i32 AIScriptSetInterrupt(struct AISCRIPTPROCESS *, u8, u8, char *, f32);
    extern struct AISCRIPTPROCESS *current_processor;
    f32 AIParamToFloat(struct AISCRIPTPROCESS *, char *);
#ifdef __cplusplus
}
#endif

struct AIPATHNODE {              // size 0x5c
    char *name;                  // 0x00
    NUVEC position;              // 0x04
    f32 radius;                  // 0x10
    f32 radius_squared;          // 0x14
    f32 min_height;              // 0x18
    f32 min_height_offset;       // 0x1c
    f32 max_height;              // 0x20
    f32 max_height_offset;       // 0x24
    u8 connection_count;         // 0x28
    u8 flags;                    // 0x29
    u8 has_special;              // 0x2a
    u8 runtime_flags;            // 0x2b
    i16 path_flags;              // 0x2c
    u8 distance_cache_nodes[2];  // 0x2e
    u8 special_route_index;      // 0x30
    u8 pad_31[3];                // 0x31
    AIPATHCNX **connections;     // 0x34
    f32 distance_cache[2];       // 0x38
    nuhspecial_s special_handle; // 0x40
    NUVEC special_position;      // 0x4c
    u16 route_membership_mask;   // 0x58
    u16 route_boundary_mask;     // 0x5a
};

typedef struct AIPATH {               // size 0xa8
    char name[0x10];                  // 0x00
    u8 node_count;                    // 0x10
    u8 flags;                         // 0x11
    u16 connection_count;             // 0x12
    u8 route_count;                   // 0x14
    u8 index;                         // 0x15
    u8 unknown_16[2];                 // 0x16
    u8 updated_node_bits[0x20];       // 0x18
    u8 previous_inside_node_bits[32]; // 0x38
    u8 inside_node_bits[0x20];        // 0x58
    u8 search_checksum;               // 0x78
    u8 search_reset_node;             // 0x79
    u8 special_route_count;           // 0x7a
    u8 unknown_7b;                    // 0x7b
    AIPATHNODE *nodes;                // 0x7c
    AIPATHCNX *connections;           // 0x80
    u8 **route_matrix;                // 0x84
    AIPATHROUTE *routes;              // 0x88
    AIPATHNODELINK *special_routes;   // 0x8c
    NUVEC bounds_min;                 // 0x90
    NUVEC bounds_max;                 // 0x9c
} AIPATH;

typedef struct AIPATHSPECIALROUTE { // size 0x8
    u8 path_count;                  // 0x0
    u8 unknown_1[3];                // 0x1
    AIPATH **paths;                 // 0x4
} AIPATHSPECIALROUTE;

typedef struct AIPATHSYS {              // size 0x10
    u8 path_count;                      // 0x0
    u8 unknown_01[3];                   // 0x1
    AIPATH **paths;                     // 0x4
    AIPATH *active_path;                // 0x8
    AIPATHSPECIALROUTE *special_routes; // 0xc
} AIPATHSYS;

typedef struct AIANTINODE {      // size 0x58
    NUVEC position;              // 0x00
    f32 radius;                  // 0x0c
    f32 min_y;                   // 0x10
    f32 min_y_offset;            // 0x14
    f32 max_y;                   // 0x18
    f32 max_y_offset;            // 0x1c
    nuhspecial_s special_handle; // 0x20
    NUVEC special_position;      // 0x2c
    i32 rotation;                // 0x38
    i32 rotation_offset;         // 0x3c
    f32 base_radius;             // 0x40
    f32 base_height;             // 0x44
    u8 enabled;                  // 0x48
    u8 game_flags;               // 0x49
    u8 type;                     // 0x4a
    u8 has_special;              // 0x4b
    u8 unknown_4c_b[0x4];        // 0x4c
    u32 excluded_character_types[2]; // 0x50: one bit per object id
} AIANTINODE;
#ifdef __cplusplus
extern "C" {
#endif
    extern AI_PARSE_SPEED AIActionParseSpeedFn;
    extern AI_CHARACTER_DISTANCE GetViewRangeFn;
    extern AI_CHARACTER_DISTANCE GetHearDistanceFn;
    extern AI_CHARACTER_DISTANCE GetMaxViewHeightFn;
    extern AI_CHARACTER_DISTANCE GetMinViewHeightFn;
    extern AI_LEVEL_CHARACTER_TYPE_ID SpecialRouteCharacterTypeIDFn;
    extern AIANTINODE dynamic_antinodes[64];
    AIANTINODE *AIAntinodeCreate(NUVEC *, f32);
    AIANTINODE *AIAntinodeCreateSingleFrame(NUVEC *, f32);
    void AIAntinodeDestroy(AIANTINODE *);
    void AIAntinodeMove(AIANTINODE *, NUVEC *, f32, f32, f32);
    f32 AIPathNodeDistanceToPathNode(AIPATH *, i32, i32, i32, u32);
    typedef i32 (*AISYS_SPECIAL_NODE_CALLBACK)(void *, struct nuhspecial_s *);
    extern AISYS_SPECIAL_NODE_CALLBACK AISysSpecialNodeCallback;
    i32 WithinConnection(void *, NUVEC *, AIPATH *, AIPATHCNX *, i32, AIPATHCNX *, i32, i32, AIPATHINFO *, f32, i32);
    void AIPathNodeUpdatePos(void *, AIPATH *, AIPATHNODE *);
#ifdef __cplusplus
}
#endif

typedef struct APIOBJECT *(*APIOBJECT_FROM_ID)(u8);

typedef struct AILOCATORSET { // size 0x1c
    char name[0x10];          // 0x00
    i8 locator_count;         // 0x10
    u8 unknown_11[3];         // 0x11
    u8 *locator_entries;      // 0x14
    u8 *assigned;             // 0x18
} AILOCATORSET;

/* AIGROUP.flags, from CreateAIGroup, the group update and the formation moves; 0x10 (the rows keep their own
 * heading off the path) is not named yet. */
#define AIGROUP_IN_USE 0x1    /* CreateAIGroup takes the first group without it */
#define AIGROUP_ALL_DEAD 0x2  /* no member is alive */
#define AIGROUP_REVERSED 0x4  /* the formation is mirrored and led from its last row */
#define AIGROUP_FORMATION 0x8 /* the members move in formation (AIMoveInstruction, FormationMove) */

typedef struct AIGROUP {           // size 0x134
    struct APIOBJECT *leader;      // 0x000
    i16 rotation_speed;            // 0x004
    u8 row_count;                  // 0x006
    u8 member_count;               // 0x007
    u8 count_across;               // 0x008
    u8 unknown_09[3];              // 0x009
    struct APIOBJECT *members[16]; // 0x00c
    i32 member_is_alive;           // 0x04c

    union { // size 0x4
        u32 flags;

        struct {                      // size 0x4
            u32 flags_bits_0_1 : 2;   // 0x050:0
            u32 is_reversed : 1;      // 0x050:2
            u32 flags_bit_3 : 1;      // 0x050:3
            u32 is_row_turning : 1;   // 0x050:4
            u32 flags_bits_5_31 : 27; // 0x050:5
        };
    }; // 0x050

    AIROW rows[4]; // 0x054
    f32 radius;    // 0x124
    f32 x_spacing; // 0x128
    f32 z_spacing; // 0x12c
    f32 max_speed; // 0x130
} AIGROUP;

struct AISYS {              // size 0x1394
    void *storage;          // 0x0000
    VARIPTR storage_end;    // 0x0004
    VARIPTR storage_cursor; // 0x0008
    i32 storage_size;       // 0x000c
    AIPATHSYS *path_sys;    // 0x0010

    union { // size 0x4
        u8 unknown_14[4];
        i32 creature_count;
    }; // 0x0014

    AICREATURE *creatures;      // 0x0018
    NULISTHDR scripts;          // 0x001c
    i32 locator_count;          // 0x0024
    AILOCATOR *locators;        // 0x0028
    i32 locator_set_count;      // 0x002c
    AILOCATORSET *locator_sets; // 0x0030
    i32 area_count;             // 0x0034
    AIAREA *areas;              // 0x0038
    AIGROUP groups[16];         // 0x003c
    i32 antinode_count;         // 0x137c
    AIANTINODE *antinodes;      // 0x1380
    u8 next_area_check;         // 0x1384
    u8 goody_idx;               // 0x1385

    i16 reset_flags; // 0x1386

    void *game_sys;             // 0x1388
    struct APIOBJECT *player_1; // 0x138c
    struct APIOBJECT *player_2; // 0x1390
};

typedef void (*AI_SCRIPT_FIRST_ACTION)(AISYS *, struct AIPACKET *, struct AISCRIPTPROCESS *);

typedef i32 (*AI_GAME_PARAM_TO_FLOAT)(struct AIPACKET *, struct AISCRIPTPROCESS *, char *, f32 *);
#ifdef __cplusplus
extern "C" {
#endif
    void DestroyAIGroup(AIGROUP *);
    void AIScriptLoadAllPakFile(void *, char *, VARIPTR *, VARIPTR *, AISYS *);
    typedef NUVEC *(*AI_ORIGIN_POSITION)(AISYS *, struct AIPACKET *);
    extern AI_ORIGIN_POSITION GetOriginPositionFn;
    f32 AIParamToFloatEx(struct AIPACKET *, struct AISCRIPTPROCESS *, char *);
    void AIScriptProcess(AISYS *, struct APIOBJECT *, struct AIPACKET *, struct AISCRIPTPROCESS *, f32);
    void AISysProcessCharacter(AISYS *, struct APIOBJECT *, struct AIPACKET *, i32, f32, i32, i32);
    void InitFn_GameParamToFloat(AI_GAME_PARAM_TO_FLOAT);
    extern AI_SCRIPT_FIRST_ACTION ScriptProcessFirstTimeActionFn;
    extern struct AIPACKET *current_packet;
    extern AI_GAME_PARAM_TO_FLOAT GameParamToFloatFn;
    extern APIOBJECT_FROM_ID APIOBJECTFromObjIDFn;
    void AISysProcess(AISYS *, struct APIOBJECT *, struct APIOBJECT *);
    typedef f32 (*AI_CHARACTER_GOAL_SPEED)(struct APIOBJECT *);
    void AISysGetCharacterPathPos(AISYS *, struct APIOBJECT *, struct AIPACKET *, i32, i32);
/* AIMoveInstruction's mode, kept in the low three bits of AIPACKET.flags until AISysCharacterMovement runs the
 * movement function of the mode. */
#define AIPACKET_MOVE_MODE 0x7
#define AIMOVE_TO_DESTINATION 1          /* AIMoveToDestination; a formation moves towards it */
#define AIMOVE_RETREAT 2                 /* AIRetreatFromDestination */
#define AIMOVE_CIRCLE 3                  /* AICircle */
#define AIMOVE_WANDER 4                  /* AIWander; a formation wanders */
#define AIMOVE_TO_DESTINATION_ALONE 5    /* AIMoveToDestination, also for a member of a formation */
#define AIMOVE_AVOIDING_CAMERA 6         /* AIMoveToDestinationAvoidingCamera */
#define AIMOVE_DIRECTLY_TO_DESTINATION 7 /* AIMoveDirectlyToDestination */
    void AIMoveInstruction(struct AIPACKET *, NUVEC *, f32, AIPATHINFO *, i32, f32);
    void AIFormationFollow(struct AIPACKET *);
    void AISysUpdateCharacterPathPos(AISYS *, struct APIOBJECT *, struct AIPACKET *, i32, f32);
    void InitFn_APIOBJECTFromObjIDFn(APIOBJECT_FROM_ID);
    void AILocatorSet_CheckLocatorsStillAssigned(AISYS *, AILOCATORSET *);
    void AILocatorSet_AssignNearestLocator(AISYS *, AILOCATORSET *, struct APIOBJECT *, f32, NUVEC *, NUVEC *, f32, i32);
    void AILocatorSet_AssignFurthestLocator(AISYS *, AILOCATORSET *, struct APIOBJECT *, f32, NUVEC *, NUVEC *, f32, i32);
    void AILocatorSet_AssignRandomLocator(AISYS *, AILOCATORSET *, struct APIOBJECT *, f32, NUVEC *, f32, i32);
    void AISysGetPathPosEx(AISYS *, NUVEC *, AIPATHINFO *, AIPATH *, i32, i32, f32 *);
    void AISysGetPathPos(AISYS *, NUVEC *, AIPATHINFO *, AIPATH *, i32);
    i32 AISysSetLevelPath(AISYS *, char *);
    AILOCATOR *AIPathFindLocator(AISYS *, char *);
    AILOCATORSET *AIPathFindLocatorSet(AISYS *, char *);
    AIAREA *AISysFindArea(AISYS *, char *);
    AIPATH *AISysFindPath(AISYS *, char *);
    AIPATHNODE *AIPathFindNode(AISYS *, AIPATH *, char *);
    void *AIPAthFindPathCnx(AISYS *, AIPATH *, char *, char *, i32 *);
    void AIScriptInitConditions(AISYS *sys);
    typedef struct APIOBJECT *(*GAMEOBJECT_LOOKUP_CALLBACK)(void *, char *);
    extern GAMEOBJECT_LOOKUP_CALLBACK GetNamedGameObjectCallback;
    void AISysCharacterSetPath(struct AIPACKET *, AIPATH *);
    void AISysFindRoute(struct AIPACKET *);
    void AISysCharacterSetPathCnx(struct AIPACKET *, NUVEC *, AIPATHCNX *, i32);
    void LEGO_AISysCreatureInteraction2D(AISYS *, i32, struct APIOBJECT **, i32 *, f32);
    void AISysCreatureAntinodeInteraction(AISYS *, i32, struct APIOBJECT **, i32 *);
    void AIScriptLoadAll(char *, VARIPTR *, VARIPTR *, AISYS *);
#ifdef __cplusplus
}
#endif
typedef i32 AIFINDALTERNATIVESPECIALOBJECT(AISYS *system, struct nuhspecial_s *special);

#ifdef __cplusplus
extern "C" {
#endif
    struct AISCRIPT *AIScriptFind(AISYS *, const char *, i32, i32, i32);
    struct AISTATE *AIStateFind(const char *, struct AISCRIPT *);
    void AIScriptSetState(struct AISCRIPTPROCESS *, struct AISTATE *);
    void AIScriptProcessorInit(AISYS *, struct AIPACKET *, struct AISCRIPTPROCESS *, AICREATURE *, char *, char *, i32, struct AISCRIPT *,
                               struct AISTATE *);
    extern struct AISCRIPT *aiscript_load_script;
    extern struct AISTATE *aiscript_load_state;
    extern void (*GameAILoadFn)(AISYS *, i32, struct NUGSCN_s *, VARIPTR *, VARIPTR *);
    void AIScriptOpenPakFileParse(struct AISCRIPT **, void *, char *, char *, VARIPTR *, VARIPTR *);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern AI_BIG_JUMP_TO_DESTINATION AIBigJumpToDestinationFn;

    extern AI_RESPAWN_ON_PATH AIRespawnOnPathFn;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

    void SetSpecialMove(struct gameobject_s *, AIPATHNODE *, AIPATHNODE *, i8);
    i32 TryToTeleportToNextNode(struct gameobject_s *, AIPATHNODE *, i32);
#ifdef __cplusplus
}
#endif

typedef struct EDAICREATURE_s { // size 0xac
    /* Editor creature record; its area/locator references are editor records. */
    NULISTLNK link;                      // 0x00
    char name[16];                       // 0x08
    char script_name[16];                // 0x18
    NUVEC position;                      // 0x28
    i32 angle;                           // 0x34
    struct EDAIPATHCHECK_s check;        // 0x38
    u32 valid_positions;                 // 0x54
    i16 character_type;                  // 0x58
    u8 set;                              // 0x5a
    u8 group_count;                      // 0x5b
    u8 across_count;                     // 0x5c
    u8 unknown_5d[3];                    // 0x5d
    f32 x_spacing;                       // 0x60
    f32 z_spacing;                       // 0x64
    u32 flags;                           // 0x68
    struct EDAIAREA_s *activation_area;  // 0x6c
    f32 script_params[4];                // 0x70
    struct EDAIAREA_s *trigger_area;     // 0x80
    struct EDLOCATOR_s *locator;         // 0x84
    struct EDLOCATOR_s *respawn_locator; // 0x88
    u8 difficulty;                       // 0x8c
    i8 min_respawns;                     // 0x8d
    i8 max_respawns;                     // 0x8e
    u8 activation;                       // 0x8f
    f32 min_respawn_time;                // 0x90
    f32 max_respawn_time;                // 0x94
    f32 stagger_start;                   // 0x98
    f32 view_distance;                   // 0x9c
    f32 hear_distance;                   // 0xa0
    f32 max_view_height;                 // 0xa4
    f32 min_view_height;                 // 0xa8
} EDAICREATURE_s;

#ifdef __cplusplus
extern "C" {
#endif
    void creatureEditor_PathNodeMoved(struct EDAIPATHNODE_s *node);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void locatorEditor_cbCancelSelectLocatorSet(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void pathEditor_cbCancelSelectMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    extern const char NoText[3];
    extern const char YesText[4];
#ifdef __cplusplus
}
#endif
