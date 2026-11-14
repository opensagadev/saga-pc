#ifndef SAGA_NU2API_NUCORE_NUTHREAD_H
#define SAGA_NU2API_NUCORE_NUTHREAD_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/fixed_width.h"

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

#endif
