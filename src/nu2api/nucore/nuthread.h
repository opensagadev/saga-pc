#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern volatile u32 nu_current_thread_id;
    extern void (*NuThreadDisableThreadSwap)(void);
    extern void (*NuThreadEnableThreadSwap)(void);
    extern HANDLE pc_file_handles[32];
    void NuThreadCriticalSectionBegin(i32);
    void NuThreadCriticalSectionEnd(i32);
#ifdef __cplusplus
}
#endif
