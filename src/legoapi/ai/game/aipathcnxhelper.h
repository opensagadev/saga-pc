/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_AI_GAME_AIPATHCNXHELPER_H_PART
#define SAGA_LEGOAPI_AI_GAME_AIPATHCNXHELPER_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_AI_GAME_AIPATHCNXHELPER_H_PART == 1 && !defined(SAGA_LEGOAPI_AI_GAME_AIPATHCNXHELPER_H_STOP)

struct AIPATHCNX;
struct WORLDINFO_s;
struct gameobject_s;

typedef struct ai_helper_slot_s { // size 0x10
    struct AIPATHCNX *connection; // 0x0
    void *target;                 // 0x4
    u8 direction;                 // 0x8
    u8 type;                      // 0x9
    u8 padding[2];                // 0xa
    f32 jump_off_dy;              // 0xc
} AI_HELPER_SLOT;

typedef struct ai_helper_system_s { // size 0x8
    i16 capacity;                   // 0x0
    i16 count;                      // 0x2
    AI_HELPER_SLOT *helpers;        // 0x4
} AI_HELPER_SYSTEM;

#ifdef __cplusplus
extern "C" {
#endif
    AI_HELPER_SLOT *AIPathCnxHelperSys_AddHelper(AI_HELPER_SYSTEM *, struct AIPATHCNX *, u8, void *, u8);
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
    void AIPathCnxHelperSysReset(struct WORLDINFO_s *, struct ai_helper_system_s *);
#ifdef __cplusplus
}
#endif

/* Native Indy path-connection movement and lookup ABI. */
typedef i32 (*AI_HELPER_FILTER)(AI_HELPER_SLOT *, struct gameobject_s *, struct AIPATHCNX *, u8, u8);
#ifdef __cplusplus
extern "C" {
#endif
    AI_HELPER_SLOT *AIPathCnxHelperSys_Find(AI_HELPER_SYSTEM *system, struct gameobject_s *object, struct AIPATHCNX *connection,
                                            u8 direction, u8 type, AI_HELPER_FILTER filter);
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
