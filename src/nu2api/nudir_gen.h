/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern i64 NuTime_TickBase;
    extern i64 NuTime_PerformanceFrequency;
    extern i32 PCThreadCriticalSectionCount;
    extern CRITICAL_SECTION PCThreadCriticalSections[13];
    i32 PCThreadCriticalSectionCreate_candidate(void);
    i64 PCThreadMillisecondsPc(void);
    void PCThreadCriticalSectionDeleteNoOpPc(i32);
    void PcInitializePerformanceTiming(void);
    extern i32 PcStartupCriticalSectionInitialized;
    extern CRITICAL_SECTION PcStartupCriticalSection;
#ifdef __cplusplus
}
#endif
