#ifndef SAGA_NU2API_NUCORE_NUMEM_H
#define SAGA_NU2API_NUCORE_NUMEM_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/fixed_width.h"

#ifdef __cplusplus
extern "C" {
#endif
    void NuFileError(void);
    void NuFileRetry(void);
    void *NuScratchAlloc32(i32);
    void NuScratchRelease(void);
    void NuMemSet128(void *, u32, i32);
#ifdef __cplusplus
}
#endif

#endif
