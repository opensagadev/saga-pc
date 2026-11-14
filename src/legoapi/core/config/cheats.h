#ifndef SAGA_LEGOAPI_CORE_CONFIG_CHEATS_H
#define SAGA_LEGOAPI_CORE_CONFIG_CHEATS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/legoapi_types.h (not all of it earlier in the shim). */

#include "legoapi/core/config/cheat.h"

#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#endif

typedef struct cheatsystem_pc_s {
    CHEAT_PC *cheats;
    i32 count;
    u32 flags;
} CHEATSYSTEM_PC;

STATIC_ASSERT(sizeof(CHEATSYSTEM_PC) == 12);
STATIC_ASSERT(offsetof(CHEATSYSTEM_PC, flags) == 8);
#ifdef __cplusplus
extern "C" {
#endif
    void Cheats_FrameUpdate(void);
    extern CHEATSYSTEM_PC *CheatSystemPtr;
    i32 GetAnimDirection(NUINSTANIM *);
#ifdef __cplusplus
}
#endif

#endif
