/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

typedef struct NUKEYCHAR_s { // size 0x8
    u32 key;                 // 0x0
    u8 normal;               // 0x4
    u8 shifted;              // 0x5
    u8 padding[2];           // 0x6
} NUKEYCHAR;
