#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuOcclusionManagerIsEnabled(void);
    void NuOcclusionManagerInit(u32, VARIPTR *, VARIPTR);
    void NuOcclusionManagerBeginFrame(void);
#ifdef __cplusplus
}
#endif
