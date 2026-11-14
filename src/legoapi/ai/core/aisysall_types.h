/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/nuvec.h"
#endif

typedef struct EDAIAREA_s { // size 0x48
    /* Editor area record: a list link ahead of an AIAREA_PC image (rebuild copies it). */
    NULISTLNK link;    // 0x00
    char name[16];     // 0x08
    NUVEC position;    // 0x18
    NUVEC size;        // 0x24
    u16 rotation;      // 0x30
    u8 unknown_32;     // 0x32
    u8 flags;          // 0x33
    u8 unknown_34[20]; // 0x34
} EDAIAREA_s;
