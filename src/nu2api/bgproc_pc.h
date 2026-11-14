#ifndef SAGA_NU2API_BGPROC_PC_H
#define SAGA_NU2API_BGPROC_PC_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nulst.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern NULSTHDR *bgproc_procinfo_pool;
    extern i32 bgproc_initialized;
    extern CRITICAL_SECTION bgproc_critical_section;
    extern HANDLE bgproc_work_event;
    extern HANDLE bgproc_resume_event;
    extern BGPROCINFO *bgproc_active;
    extern HANDLE bgproc_thread_handle_pc;
    extern DWORD bgproc_thread_id;
    i32 bgprocIsFrozen(void);

    void PcInitializeStartupCriticalSection(void);
#ifdef __cplusplus
}
#endif

#endif
