#pragma once

#include "nu2api/nucore/common.h"

typedef struct CHARACTER_CONTEXT_INFO_s { // size 0x10
    const char *name;                     // 0x0
    i32 action;                           // 0x4
    u64 flags;                            // 0x8
} CHARACTER_CONTEXT_INFO;

#ifdef __cplusplus
extern "C" {
#endif
    extern CHARACTER_CONTEXT_INFO *CInfo;
#ifdef __cplusplus
}
#endif
