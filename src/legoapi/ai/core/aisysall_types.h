#ifndef SAGA_LEGOAPI_AI_CORE_AISYSALL_TYPES_H
#define SAGA_LEGOAPI_AI_CORE_AISYSALL_TYPES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: nu2api/numath/nuvec.h (not all of it earlier in the shim). */

#include "nu2api/nucore/nulist.h"

#ifndef SAGA_SHIM
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

#endif
