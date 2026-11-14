#ifndef SAGA_LEGOAPI_AI_CORE_GAMEAI_H_PART
#define SAGA_LEGOAPI_AI_CORE_GAMEAI_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: gameapi/ai/aisys/aiscript_types.h, gameapi/ai/aisys/aisys.h,
 * legoapi/gizmo/base/gizmo.h, legoapi/items/base/apiobject.h, legogame/game.h, nu2api/nucore/common.h,
 * nu2api/nucore/fixed_width.h, nu2api/numath/nuvec.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aiscript_types.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/items/base/apiobject.h"
#include "legogame/game.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"
#endif
/* 4 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_AI_CORE_GAMEAI_H_PART == 1 && !defined(SAGA_LEGOAPI_AI_CORE_GAMEAI_H_STOP)

typedef struct level_script_process_pc_s {
    char name[16];
    AISCRIPTPROCESS_PC processor;
    u8 unknown_d8[4];
} LEVEL_SCRIPT_PROCESS_PC;
#ifdef __cplusplus
extern "C" {
#endif
    extern f32 alert_time;
    extern NUVEC alert_pos;
    void GameAISysSetGame(void);
    void GameAISysInit(void);
    typedef i32 (*AI_PARSE_SPEED_PC)(char *, u8 *);
    typedef f32 (*AI_CHARACTER_DISTANCE_PC)(i32);
    extern i32 ai_fighting;
    typedef u8 (*AI_LEVEL_CHARACTER_TYPE_ID_PC)(char *);
    typedef i32 (*AI_LEVEL_CHARACTER_GLOBAL_ID_PC)(u8);
    extern i32 party_under_cover;
    extern f32 alert_timer;
    void MakeBaddiesForgetAboutParty(i32);
    extern i32 party_cant_be_under_cover;
    extern f32 default_path_heighttol;
    extern i32 party_cant_be_under_cover;
    extern i8 (*adtab)[4];

    extern const char AIControlPlayerRunSpeedName[17];
    void oneAtOnce_MaintainArray(void);
    void oneAtOnce_SetNumAttackers(i32);
    void oneAtOnce_SetAttackersPerRow(i32);
    void oneAtOnce_SetDistPerRow(f32);
    void oneAtOnce_SetInitDistPerRow(f32);
    void GameAIProcess(void);
    extern f32 draw_attention_distance;
    extern f32 GameAiPartyFollowSpacingPc;
    extern i32 drop_in_teleport;
    extern i32 teleport_all_freeplay_modes;
    extern f32 DEFENDTIME;
    extern i32 debug_ignore_players;
    extern i32 WALKSTEALTH;
    extern i32 active_goody_count;
    extern i32 GameAiLosDivisorPc;
    extern f32 timetogetlos;
    extern i32 test_new_interaction;
    extern f32 OpponentSelectionHysteresisPc;
    extern i32 nbaddies_can_see_players;
    extern f32 fakeanimframe[1];
    void AI_Activate(void *gizmo, i32 active);
    void AI_AddGizmos(struct gizmosys_pc_s *gizmo_sys, i32 type_id, void *world_ptr, void *unused);
    char *AI_GetGizmoName(void *gizmo);
    i32 AI_GetMaxGizmos(void *arg0);
    i32 AI_GetNumOutputs(void *arg0);
    i32 AI_GetOutput(void *gizmo, i32 arg1, i32 arg2);
    char *AI_GetOutputName(void *arg0, i32 arg1);
#ifdef __cplusplus
}
#endif

STATIC_ASSERT(sizeof(LEVEL_SCRIPT_PROCESS_PC) == 0xdc);
STATIC_ASSERT(offsetof(LEVEL_SCRIPT_PROCESS_PC, unknown_d8) == 0xd8);
STATIC_ASSERT(offsetof(LEVEL_SCRIPT_PROCESS_PC, processor.params) == 0x24);
STATIC_ASSERT(offsetof(LEVEL_SCRIPT_PROCESS_PC, processor) == 0x10);
#ifdef __cplusplus
extern "C" {
#endif
    i32 CheckPosAIArea(AIAREA_PC *, NUVEC *, f32);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_AI_CORE_GAMEAI_H_PART
#define SAGA_LEGOAPI_AI_CORE_GAMEAI_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_AI_CORE_GAMEAI_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_AI_CORE_GAMEAI_H_PART == 2 && !defined(SAGA_LEGOAPI_AI_CORE_GAMEAI_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    typedef i32 (*AI_BIG_JUMP_TO_DESTINATION_PC)(APIOBJECT_PC *, NUVEC *);
    APIOBJECT_PC *GameAPIOBJECTFromObjID(u8);
    f32 GetCharacterGoalSpeed(APIOBJECT_PC *);
    typedef i32 (*AI_RESPAWN_ON_PATH_PC)(APIOBJECT_PC *);
    APIOBJECT_PC *GetNamedAPIObject(void *, char *);
    NUVEC *GetAICreatureOrigin(AISYS_PC *, AIPACKET_PC *);

    void GameCreatureInteraction2D(AISYS_PC *, i32, APIOBJECT_PC **, i32 *, f32);

    void GameCreatureOpponentSelection(AISYS_PC *, i32, APIOBJECT_PC **, i32, APIOBJECT_PC **, i32, APIOBJECT_PC **,
                                       u64, f32);
    void GameAISysReset(AISYS_PC *);
    void GameAISysStartFrame(AISYS_PC *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_AI_CORE_GAMEAI_H_PART
#define SAGA_LEGOAPI_AI_CORE_GAMEAI_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_AI_CORE_GAMEAI_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_AI_CORE_GAMEAI_H_PART == 3 && !defined(SAGA_LEGOAPI_AI_CORE_GAMEAI_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void AlertSurroundingCreatures(GAMEOBJECT *, NUVEC *);
    i32 oneAtOnce_CanAttack(GAMEOBJECT *, GAMEOBJECT *);
    f32 oneAtOnce_GetHoldRange(GAMEOBJECT *);
    extern GAMEOBJECT *alert_obj;

#ifdef __cplusplus
}
#endif

typedef struct ai_control_system_pc_s AI_CONTROL_SYSTEM_PC;
#ifdef __cplusplus
extern "C" {
#endif
    void AIPathCnxControlSysReset(AI_CONTROL_SYSTEM_PC *);
    void AIPathCnxControlSysUpdate(AI_CONTROL_SYSTEM_PC *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_AI_CORE_GAMEAI_H_PART
#define SAGA_LEGOAPI_AI_CORE_GAMEAI_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_AI_CORE_GAMEAI_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_AI_CORE_GAMEAI_H_PART == 4 && !defined(SAGA_LEGOAPI_AI_CORE_GAMEAI_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    extern ADAPTIVEDIFFICULTY_PC adaptivedifficulty;
#ifdef __cplusplus
}
#endif
#ifndef TYPES_OFF_2

typedef struct OPPONENT_PACKET_PC {
    u8 unknown000[0xd4];                    // 0x000
    APIOBJECT_PC *nearest_opponent_object;  // 0x0d4
    f32 nearest_opponent_metric;            // 0x0d8
    APIOBJECT_PC *pending_nearest_opponent; // 0x0dc
    f32 pending_nearest_metric;             // 0x0e0
    APIOBJECT_PC *opponent_object;          // 0x0e4
    f32 opponent_metric;                    // 0x0e8
    APIOBJECT_PC *pending_opponent;         // 0x0ec
    f32 pending_opponent_metric;            // 0x0f0
    u8 unknown0f4[0xf4];                    // 0x0f4
    u32 flags;                              // 0x1e8
} OPPONENT_PACKET_PC;                       // size 0x1ec

typedef struct OPPONENT_OBJECT_PC {
    u8 unknown000[0x230];           // 0x000
    f32 view_distance;              // 0x230
    u8 unknown234[0xc];             // 0x234
    f32 visibility_range_extension; // 0x240
    u8 unknown244[0x64];            // 0x244
    u64 awareness;                  // 0x2a8
} OPPONENT_OBJECT_PC;               // size 0x2b0

typedef struct OPPONENT_GAME_PC {
    u8 unknown000[0xeb0];       // 0x000
    u64 seen;                   // 0xeb0
    u64 exclusion;              // 0xeb8
    APIOBJECT_PC *alert_target; // 0xec0
    f32 alert_timer;            // 0xec4
} OPPONENT_GAME_PC;             // size 0xec8

typedef struct OPPONENT_SYS_PC {
    u8 unknown000[0x1385]; // 0x0000
    u8 goody_cursor;       // 0x1385
    u16 assignment_flags;  // 0x1386
} OPPONENT_SYS_PC;         // size 0x1388

typedef struct OPPONENT_LOS_PC {
    u32 object_size;       // 0x000
    GAMEOBJECT *objects;   // 0x004
    u64 line_of_sight[64]; // 0x008
} OPPONENT_LOS_PC;         // size 0x208

typedef struct ai_frame_world_pc_s {
    u8 unknown_0000[0x2aec];                // 0x0000
    i32 processor_count;                    // 0x2aec
    LEVEL_SCRIPT_PROCESS_PC processors[32]; // 0x2af0
} AI_FRAME_WORLD_PC;                        // size 0x4670

typedef struct ai_control_slot_pc_s {
    NULISTLNK links;     // 0x00
    u8 unknown_08[0xa0]; // 0x08
} AI_CONTROL_SLOT_PC;    // size 0xa8

typedef struct ai_control_system_pc_s {
    i32 capacity;                    // 0x00
    AI_CONTROL_SLOT_PC *controllers; // 0x04
    NULISTHDR available;             // 0x08
    NULISTHDR active;                // 0x10
} AI_CONTROL_SYSTEM_PC;              // size 0x18

typedef struct EQUIVALENTOBJECTGROUP_PC {
    i16 object_count;           // 0x0
    i16 byte_size;              // 0x2
    NUSPECIALHANDLE objects[1]; // 0x4
} EQUIVALENTOBJECTGROUP_PC;     // size 0x10

typedef struct AIONCEATTACKER_PC {
    GAMEOBJECT *object; // 0x0
    f32 distance;       // 0x4
} AIONCEATTACKER_PC;    // size 0x8

#endif /* TYPES_OFF_2 */
#undef SAGA_LEGOAPI_AI_CORE_GAMEAI_H_PART
#define SAGA_LEGOAPI_AI_CORE_GAMEAI_H_PART 5
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_AI_CORE_GAMEAI_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_AI_CORE_GAMEAI_H_STOP
