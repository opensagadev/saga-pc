#ifndef SAGA_NU2API_OGGREADER_H
#define SAGA_NU2API_OGGREADER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/fixed_width.h"

#ifdef __cplusplus
extern "C" {
#endif
    struct PC_OGG_CACHE_RECORD_PC;
    extern struct PC_OGG_CACHE_RECORD_PC *PCOggCachePoolPc;
    extern i32 PCOggCriticalSectionPc;
#ifdef __cplusplus
}
#endif

#endif
