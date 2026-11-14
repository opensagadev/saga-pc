/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

typedef struct CHARACTER_CONTEXT_INFO_PC_s {
    const char *name;
    i32 action;
    u32 flags;
    i32 parameter;
} CHARACTER_CONTEXT_INFO_PC;

STATIC_ASSERT(sizeof(CHARACTER_CONTEXT_INFO_PC) == 0x10);
STATIC_ASSERT(offsetof(CHARACTER_CONTEXT_INFO_PC, flags) == 8);
STATIC_ASSERT(offsetof(CHARACTER_CONTEXT_INFO_PC, parameter) == 0xc);
#ifdef __cplusplus
extern "C" {
#endif
    extern CHARACTER_CONTEXT_INFO_PC *CInfo;
#ifdef __cplusplus
}
#endif
