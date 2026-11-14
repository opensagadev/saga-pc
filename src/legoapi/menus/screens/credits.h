#ifndef SAGA_LEGOAPI_MENUS_SCREENS_CREDITS_H
#define SAGA_LEGOAPI_MENUS_SCREENS_CREDITS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/world/world.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/world/world.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/world/world.h"
#endif

typedef struct credit_record_pc_s CREDIT_RECORD_PC;
#ifdef __cplusplus
extern "C" {
#endif
    extern CREDIT_RECORD_PC *CreditList;

    void Credits_Load(WORLDINFO *, VARIPTR *, VARIPTR *);
#ifdef __cplusplus
}
#endif

#endif
