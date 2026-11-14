/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aiscript_types.h"
#endif

typedef struct LEVELSCRIPTPROCESS_s LEVELSCRIPTPROCESS_s;

struct LEVELSCRIPTPROCESS_s {     // size 0xdc
    char name[16];                // 0x00
    AISCRIPTPROCESS_PC processor; // 0x10
    u32 unknown_d8;               // 0xd8
};
