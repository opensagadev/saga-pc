#include "saga.h"
#include <string.h>
#include <stdio.h>

STATIC_ASSERT(offsetof(LEVEL_SCRIPT_PROCESS_PC, processor) + offsetof(AISCRIPTPROCESS_PC, flags) == 0x60);
STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);

/* Native PC startup and callback providers; insert in gameai.c before GameAISysSetGame. */
typedef char *(__cdecl *AI_CHARACTER_NAME_PC)(u8);
typedef char *(__cdecl *AI_GLOBAL_CHARACTER_NAME_PC)(i32);
typedef void *(__cdecl *AI_GLOBAL_CHARACTER_HGOBJ_PC)(i32);
typedef void(__cdecl *AI_GLOBAL_CHARACTER_RENDER_PC)(NUVEC *, i16, i32, i32, void *);
typedef void(__cdecl *AI_CLEAR_CREATURES_PC)(void);
typedef APIOBJECT_PC *(__cdecl *AI_GET_NAMED_OBJECT_PC)(void *, char *);
typedef NUVEC *(__cdecl *AI_GET_CREATURE_ORIGIN_PC)(AISYS_PC *, AIPACKET_PC *);

i32 __cdecl StarWars_PreparingForSpecialMove(AIPACKET_PC *, APIOBJECT_PC *, i32);
i32 __cdecl StarWars_MidSpecialMove(AISYS_PC *, AIPACKET_PC *, APIOBJECT_PC *);

AI_CHARACTER_NAME_PC LevelCharacterNameFn = 0, SpecialRouteCharacterNameFn = 0;
AI_GLOBAL_CHARACTER_NAME_PC GlobalCharacterNameFn = 0;
AI_GLOBAL_CHARACTER_HGOBJ_PC GlobalCharacterHGobjFn = 0;
AI_GLOBAL_CHARACTER_RENDER_PC GlobalCharacterRenderFn = 0;
AI_CLEAR_CREATURES_PC ClearAICreaturesFn = 0;

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
