#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/ai/aisys/aiscript_types.h"
#include "nu2api/numath/nuvec.h"

struct AIAREA;
struct AIPACKET;
struct AISYS;
struct APIOBJECT;
struct addgizmotype_s;
struct gameobject_s;
struct gizmosys_s;

typedef struct level_script_process_s { // size 0xdc
    char name[16];                      // 0x00
    AISCRIPTPROCESS processor;          // 0x10
    u8 reserved_d8[4];                   // 0xd8
} LEVEL_SCRIPT_PROCESS;
#ifdef __cplusplus
extern "C" {
#endif
    extern f32 alert_time;
    extern NUVEC alert_pos;
    void GameAISysSetGame(void);
    void GameAISysInit(void);
    typedef i32 (*AI_PARSE_SPEED)(char *, u8 *);
    typedef f32 (*AI_CHARACTER_DISTANCE)(i32);
    extern i32 ai_fighting;
    typedef u8 (*AI_LEVEL_CHARACTER_TYPE_ID)(char *);
    typedef i32 (*AI_LEVEL_CHARACTER_GLOBAL_ID)(u8);
    /* The game's hooks into the AI system (InitFn_*). */
    typedef char *(*AI_CHARACTER_NAME)(u8);
    typedef char *(*AI_GLOBAL_CHARACTER_NAME)(i32);
    typedef void *(*AI_GLOBAL_CHARACTER_HGOBJ)(i32);
    typedef void (*AI_GLOBAL_CHARACTER_RENDER)(NUVEC *, i16, i32, i32, void *);
    typedef void (*AI_CLEAR_CREATURES)(void);
    typedef struct APIOBJECT *(*AI_GET_NAMED_OBJECT)(void *, char *);
    typedef NUVEC *(*AI_GET_CREATURE_ORIGIN)(struct AISYS *, struct AIPACKET *);
    void InitFn_LevelCharacterName(AI_CHARACTER_NAME);
    void InitFn_SpecialRouteCharacterName(AI_CHARACTER_NAME);
    void InitFn_GlobalCharacterName(AI_GLOBAL_CHARACTER_NAME);
    void InitFn_GlobalCharacterRender(AI_GLOBAL_CHARACTER_RENDER);
    void InitFn_GlobalCharacterHGobj(AI_GLOBAL_CHARACTER_HGOBJ);
    void InitFn_ClearAICreatures(AI_CLEAR_CREATURES);
    void InitFn_GetNamedAPIObject(AI_GET_NAMED_OBJECT);
    void InitFn_GetAICreatureOrigin(AI_GET_CREATURE_ORIGIN);
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
    extern f32 GameAiPartyFollowSpacing;
    extern i32 drop_in_teleport;
    extern i32 teleport_all_freeplay_modes;
    extern f32 DEFENDTIME;
    extern i32 debug_ignore_players;
    extern i32 WALKSTEALTH;
    extern i32 active_goody_count;
    extern i32 GameAiLosDivisor;
    extern f32 timetogetlos;
    extern i32 test_new_interaction;
    extern f32 OpponentSelectionHysteresis;
    extern i32 nbaddies_can_see_players;
    extern f32 fakeanimframe[1];
    void AI_Activate(void *gizmo, i32 active);
    void AI_AddGizmos(struct gizmosys_s *gizmo_sys, i32 type_id, void *world_ptr, void *unused);
    char *AI_GetGizmoName(void *gizmo);
    i32 AI_GetMaxGizmos(void *arg0);
    i32 AI_GetNumOutputs(void *arg0);
    i32 AI_GetOutput(void *gizmo, i32 arg1, i32 arg2);
    char *AI_GetOutputName(void *arg0, i32 arg1);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 CheckPosAIArea(struct AIAREA *, NUVEC *, f32);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    typedef i32 (*AI_BIG_JUMP_TO_DESTINATION)(struct APIOBJECT *, NUVEC *);
    struct APIOBJECT *GameAPIOBJECTFromObjID(u8);
    f32 GetCharacterGoalSpeed(struct APIOBJECT *);
    typedef i32 (*AI_RESPAWN_ON_PATH)(struct APIOBJECT *);
    struct APIOBJECT *GetNamedAPIObject(void *, char *);
    NUVEC *GetAICreatureOrigin(struct AISYS *, struct AIPACKET *);

    void GameCreatureInteraction2D(struct AISYS *, i32, struct APIOBJECT **, i32 *, f32);

    void GameCreatureOpponentSelection(struct AISYS *, i32, struct APIOBJECT **, i32, struct APIOBJECT **, i32, struct APIOBJECT **, u64,
                                       f32);
    void GameAISysReset(struct AISYS *);
    void GameAISysStartFrame(struct AISYS *);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void AlertSurroundingCreatures(struct gameobject_s *, NUVEC *);
    i32 oneAtOnce_CanAttack(struct gameobject_s *, struct gameobject_s *);
    f32 oneAtOnce_GetHoldRange(struct gameobject_s *);
    extern struct gameobject_s *alert_obj;

#ifdef __cplusplus
}
#endif

typedef struct ai_control_system_s AI_CONTROL_SYSTEM;
#ifdef __cplusplus
extern "C" {
#endif
    void AIPathCnxControlSysReset(AI_CONTROL_SYSTEM *);
    void AIPathCnxControlSysUpdate(AI_CONTROL_SYSTEM *);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void ClearSpecialMove(struct gameobject_s *);
    struct addgizmotype_s *AI_RegisterGizmo(i32);
    u32 GameAI_TotalScore(void);
    i32 CanFightLikeAJedi(struct gameobject_s *);
#ifdef __cplusplus
}
#endif

typedef struct ai_control_system_s { // size 0x18
    i32 capacity;                    // 0x00
    AI_CONTROL_SLOT *controllers;    // 0x04
    NULISTHDR available;             // 0x08
    NULISTHDR active;                // 0x10
} AI_CONTROL_SYSTEM;
