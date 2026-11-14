/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/nuvec.h"
#endif

typedef struct EDAIAREA_s { /* Editor area record: a list link ahead of an AIAREA_PC image (rebuild copies it). */
    NULISTLNK link;
    char name[16];
    NUVEC position;
    NUVEC size;
    u16 rotation;
    u8 unknown_32;
    u8 flags;
    u8 unknown_34[20];
} EDAIAREA_s;

STATIC_ASSERT(sizeof(EDAIAREA_s) == 72);
