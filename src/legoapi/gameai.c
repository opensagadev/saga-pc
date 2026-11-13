#include "saga.h"

STATIC_ASSERT(offsetof(LEVEL_SCRIPT_PROCESS_PC, processor) + offsetof(AISCRIPTPROCESS_PC, flags) == 0x60);
STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);

#include <string.h>
#include <stdio.h>

/* Native PC startup and callback providers; insert in gameai.c before GameAISysSetGame. */
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

extern void(__cdecl *AIPathCnxHelperSysInitFn)(WORLDINFO *);
extern i32 LEGO_AIPATHCNX_VARIANT_FLAG0_PC, LEGO_AIPATHCNX_VARIANT_FLAG1_PC;
extern u32 LEGO_AIPATHCNX_BIGJUMP, LEGO_AIPATHCNX_REQUIRESPERMISSION;
extern u32 LEGO_AIPATHCNX_NO_DESTINATION_CHECK, LEGO_AIPATHCNX_JUMP_NOW, LEGO_AIPATHCNX_DONT_JUMP_NOW;
void __cdecl InitFn_PreparingForSpecialMove(AI_PREPARING_SPECIAL_MOVE_PC);
void __cdecl InitFn_MidSpecialMove(AI_MID_SPECIAL_MOVE_PC);

i32 __cdecl StarWars_PreparingForSpecialMove(AIPACKET_PC *, APIOBJECT_PC *, i32);
i32 __cdecl StarWars_MidSpecialMove(AISYS_PC *, AIPACKET_PC *, APIOBJECT_PC *);
void __cdecl Indy_GameAISysInit(void) {
    LEGO_AIPATHCNX_JUMP = 0x1;
    LEGO_AIPATHCNX_FORGOODIES = 0x100;
    LEGO_AIPATHCNX_FORBADDIES = 0x200;
    LEGO_AIPATHCNX_BIGJUMP = 0x4;
    LEGO_AIPATHCNX_NO_DESTINATION_CHECK = 0x20;
    LEGO_AIPATHCNX_JUMP_NOW = 0x40;
    LEGO_AIPATHCNX_DONT_JUMP_NOW = 0x80;
    LEGO_AIPATHCNX_VARIANT_FLAG0_PC = 0x10;
    InitFn_PreparingForSpecialMove(Indy_PreparingForSpecialMove);
    InitFn_MidSpecialMove(Indy_MidSpecialMove);
    HubInitAIFnPc = LSW_Hub_InitAI;
    HubResetAIFnPc = LSW_Hub_ResetAI;
    HubUpdateAIFnPc = LSW_Hub_UpdateAI;
    AIPathCnxHelperSysInitFn = Indy_AIPathCnxHelperSysInit;
}

AI_CHARACTER_NAME_PC LevelCharacterNameFn = 0, SpecialRouteCharacterNameFn = 0;
AI_GLOBAL_CHARACTER_NAME_PC GlobalCharacterNameFn = 0;
AI_GLOBAL_CHARACTER_HGOBJ_PC GlobalCharacterHGobjFn = 0;
AI_GLOBAL_CHARACTER_RENDER_PC GlobalCharacterRenderFn = 0;
AI_CLEAR_CREATURES_PC ClearAICreaturesFn = 0;
i32 LEGO_AIPATHCNX_VARIANT_FLAG0_PC = 0, LEGO_AIPATHCNX_VARIANT_FLAG1_PC = 0;
typedef struct EQUIVALENTOBJECTGROUP_PC {
    i16 object_count;
    i16 byte_size;
    NUSPECIALHANDLE objects[1];
} EQUIVALENTOBJECTGROUP_PC;
STATIC_ASSERT(offsetof(EQUIVALENTOBJECTGROUP_PC, objects) == 4);
STATIC_ASSERT(offsetof(WORLDINFO, equivalent_groups) == 0x46b0);
STATIC_ASSERT(offsetof(WORLDINFO, equivalent_group_count) == 0x46b4);
i32 __cdecl NuHGobjRndr(NUHGOBJ_EVAL_PC *object, NUMTX *matrix, i32 count, i16 *layers) {
    __declspec(align(16)) NUMTX matrices[256];
    NuHGobjEval(object, 0, NULL, matrices);
    return NuHGobjRndrMtxDwa(object, matrix, count, layers, matrices, NULL, 0);
}

STATIC_ASSERT(offsetof(GAMEOBJECT, special_move_look_position_pc) == 0x1098);
STATIC_ASSERT(offsetof(GAMEOBJECT, launch_origin) == 0x754);
STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);
STATIC_ASSERT(offsetof(GAMEOBJECT, portal_flags) == 0xeec);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_packet) + offsetof(AIPACKET_PC, special_move_node) == 0x44c);
