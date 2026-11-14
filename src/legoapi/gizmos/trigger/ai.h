#ifndef SAGA_LEGOAPI_GIZMOS_TRIGGER_AI_H
#define SAGA_LEGOAPI_GIZMOS_TRIGGER_AI_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aiscript_types.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aiscript_types.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aiscript_types.h"
#endif

typedef struct LEVELSCRIPTPROCESS_s LEVELSCRIPTPROCESS_s;

struct LEVELSCRIPTPROCESS_s {
    char name[16];
    AISCRIPTPROCESS_PC processor;
    u32 unknown_d8;
};

STATIC_ASSERT(sizeof(struct LEVELSCRIPTPROCESS_s) == 220);

#endif
