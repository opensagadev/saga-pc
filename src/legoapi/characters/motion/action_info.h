/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

typedef struct CHARACTER_CONTEXT_INFO_PC_s { // size 0x10
    const char *name;                        // 0x0
    i32 action;                              // 0x4
    u32 flags;                               // 0x8
    i32 parameter;                           // 0xc
} CHARACTER_CONTEXT_INFO;

#ifdef __cplusplus
extern "C" {
#endif
    extern CHARACTER_CONTEXT_INFO *CInfo;
#ifdef __cplusplus
}
#endif
