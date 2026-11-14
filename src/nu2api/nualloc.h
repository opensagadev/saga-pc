/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

typedef struct nualloc_record_s NUALLOC_RECORD;
typedef struct nualloc_pool_s NUALLOC_POOL;
#ifdef __cplusplus
extern "C" {
#endif
    extern NUALLOC_RECORD *NuAllocRecordHead;
    extern u32 NuAllocTrackedBytes;
    extern u32 NuAllocHeaderMetadata[3];
    extern u32 NuAllocTailMetadata[3];
#ifdef __cplusplus
}
#endif
