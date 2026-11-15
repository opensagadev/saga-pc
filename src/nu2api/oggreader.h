#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    struct PC_OGG_CACHE_RECORD;
    extern struct PC_OGG_CACHE_RECORD *PCOggCachePool;
    extern i32 PCOggCriticalSection;
#ifdef __cplusplus
}
#endif
