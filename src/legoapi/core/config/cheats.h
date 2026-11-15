/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/core/config/cheat.h"
#include "legoapi/legoapi_types.h"
#endif

typedef struct cheatsystem_s { // size 0xc
    CHEAT *cheats;             // 0x0
    i32 count;                 // 0x4
    u32 flags;                 // 0x8
} CHEATSYSTEM;

#ifdef __cplusplus
extern "C" {
#endif
    void Cheats_FrameUpdate(void);
    extern CHEATSYSTEM *CheatSystemPtr;
    i32 GetAnimDirection(NUINSTANIM *);
#ifdef __cplusplus
}
#endif
