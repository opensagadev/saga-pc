#ifndef SAGA_NU2API_NUCORE_ANDROID_BGPROC_ANDROID_H
#define SAGA_NU2API_NUCORE_ANDROID_BGPROC_ANDROID_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/fixed_width.h"

#ifdef __cplusplus
extern "C" {
#endif
    void bgprocFreeze(void);
    void bgprocUnFreeze(void);
    i32 bgprocIsFreezing(void);
#ifdef __cplusplus
}
#endif

#endif
