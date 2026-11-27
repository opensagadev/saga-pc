#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nulst.h"

typedef struct BGPROC_HANDLER {
    struct BGPROC_HANDLER *next;
    void *unknown_04;
    void (*handler)(void);
} BGPROC_HANDLER;

#ifdef __cplusplus
extern "C" {
#endif
    extern NULSTHDR *bgproc_procinfo_pool;
    extern i32 bgproc_initialized;
    extern CRITICAL_SECTION bgproc_critical_section;
    extern HANDLE bgproc_work_event;
    extern HANDLE bgproc_resume_event;
    extern BGPROCINFO *bgproc_active;
    extern HANDLE bgproc_thread_handle;
    extern DWORD bgproc_thread_id;
    i32 bgprocIsFrozen(void);

    void PcInitializeStartupCriticalSection(void);
    extern void bgprocFrameSleep(i32 milliseconds);
    void PcEnableThreadSwap(void);
    void PcDisableThreadSwap(void);
#ifdef __cplusplus
}
#endif
