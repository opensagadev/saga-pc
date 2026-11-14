#ifndef SAGA_LEGOAPI_CORE_STARTUP_MAIN_H
#define SAGA_LEGOAPI_CORE_STARTUP_MAIN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/fixed_width.h"

#ifdef __cplusplus
extern "C" {
#endif
    void FinishLoop(i32 frames);
#ifdef __cplusplus
}
#endif

#endif
