/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

typedef struct NUKEYCHAR_s {
    u32 key;
    u8 normal;
    u8 shifted;
    u8 padding[2];
} NUKEYCHAR;

STATIC_ASSERT(sizeof(NUKEYCHAR) == 8);
STATIC_ASSERT(offsetof(NUKEYCHAR, normal) == 4);
STATIC_ASSERT(offsetof(NUKEYCHAR, shifted) == 5);
