#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    void NuFileError(void);
    void NuFileRetry(void);
    void *NuScratchAlloc32(i32);
    void NuScratchRelease(void);
    void NuMemSet128(void *, u32, i32);
    void NuAllocBadPointer(void);
    void NuAllocReport(const void *message, ...);
    void NuDatFileMissing(char *filepath);
    void NuRDPIVariableError(char *name);
#ifdef __cplusplus
}
#endif
