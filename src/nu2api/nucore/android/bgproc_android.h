/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    void bgprocFreeze(void);
    void bgprocUnFreeze(void);
    i32 bgprocIsFreezing(void);
#ifdef __cplusplus
}
#endif
