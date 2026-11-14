#ifndef SAGA_LEGOAPI_AI_GAME_AIPATHCNXHELPER_H_PART
#define SAGA_LEGOAPI_AI_GAME_AIPATHCNXHELPER_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: gameapi/ai/aisys/aisys.h, legoapi/items/base/apiobject.h, legoapi/world/world.h,
 * nu2api/nucore/common.h, nu2api/nucore/fixed_width.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#endif
/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_AI_GAME_AIPATHCNXHELPER_H_PART == 1 && !defined(SAGA_LEGOAPI_AI_GAME_AIPATHCNXHELPER_H_STOP)

typedef struct ai_helper_slot_pc_s {
    AIPATHCNX_PC *connection;
    void *target;
    u8 direction;
    u8 type;
    u8 padding[2];
    f32 jump_off_dy;
} AI_HELPER_SLOT_PC;

typedef struct ai_helper_system_pc_s {
    i16 capacity;
    i16 count;
    AI_HELPER_SLOT_PC *helpers;
} AI_HELPER_SYSTEM_PC;

STATIC_ASSERT(sizeof(AI_HELPER_SLOT_PC) == 0x10);
STATIC_ASSERT(offsetof(AI_HELPER_SLOT_PC, connection) == 0);
STATIC_ASSERT(offsetof(AI_HELPER_SLOT_PC, target) == 4);
STATIC_ASSERT(offsetof(AI_HELPER_SLOT_PC, direction) == 8);
STATIC_ASSERT(offsetof(AI_HELPER_SLOT_PC, type) == 9);
STATIC_ASSERT(offsetof(AI_HELPER_SLOT_PC, jump_off_dy) == 0xc);
STATIC_ASSERT(sizeof(AI_HELPER_SYSTEM_PC) == 8);
STATIC_ASSERT(offsetof(AI_HELPER_SYSTEM_PC, count) == 2);
STATIC_ASSERT(offsetof(AI_HELPER_SYSTEM_PC, helpers) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    AI_HELPER_SLOT_PC *AIPathCnxHelperSys_AddHelper(AI_HELPER_SYSTEM_PC *, AIPATHCNX_PC *, u8, void *, u8);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_AI_GAME_AIPATHCNXHELPER_H_PART
#define SAGA_LEGOAPI_AI_GAME_AIPATHCNXHELPER_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_AI_GAME_AIPATHCNXHELPER_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_AI_GAME_AIPATHCNXHELPER_H_PART == 2 && !defined(SAGA_LEGOAPI_AI_GAME_AIPATHCNXHELPER_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void AIPathCnxHelperSysReset(WORLDINFO *, struct ai_helper_system_pc_s *);
#ifdef __cplusplus
}
#endif

/* Native Indy path-connection movement and lookup ABI. */
typedef i32 (*AI_HELPER_FILTER_PC)(AI_HELPER_SLOT_PC *, GAMEOBJECT *, AIPATHCNX_PC *, u8, u8);
#ifdef __cplusplus
extern "C" {
#endif
    AI_HELPER_SLOT_PC *AIPathCnxHelperSys_Find(AI_HELPER_SYSTEM_PC *system, GAMEOBJECT *object,
                                               AIPATHCNX_PC *connection, u8 direction, u8 type,
                                               AI_HELPER_FILTER_PC filter);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_AI_GAME_AIPATHCNXHELPER_H_PART
#define SAGA_LEGOAPI_AI_GAME_AIPATHCNXHELPER_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_AI_GAME_AIPATHCNXHELPER_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_AI_GAME_AIPATHCNXHELPER_H_STOP
