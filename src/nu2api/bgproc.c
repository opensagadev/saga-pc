#include "saga.h"

NULSTHDR *bgproc_procinfo_pool;
volatile u32 nu_current_thread_id;
void(__cdecl *NuThreadDisableThreadSwap)(void);
void(__cdecl *NuThreadEnableThreadSwap)(void);

BGPROCINFO *__cdecl bgPostRequest(bgprocdofn *do_fn, bgprocdofn *ack_fn, void *data, i32 data_size) {
    BGPROCINFO *info;

    EnterCriticalSection(&bgproc_critical_section);
    info = (BGPROCINFO *)NuLstAllocTail(bgproc_procinfo_pool);
    if (info != NULL) {
        info->do_fn = do_fn;
        info->ack_fn = ack_fn;
        info->work_started = 0;
        info->unknown_flag_2 = 0;
        info->vars = NULL;
        info->var_count = 0;
        if (data != NULL && data_size != 0) {
            if (data_size > 0x100) {
                bgProcDataOversize(0x100);
            }
            memcpy(info->data, data, data_size);
        }
    }
    LeaveCriticalSection(&bgproc_critical_section);
    if (info != NULL) {
        SetEvent(bgproc_work_event);
    }
    return info;
}

void __cdecl bgProcInit(void) {
    if (bgproc_initialized == 0) {
        bgproc_initialized = 1;
        bgproc_procinfo_pool = NuLstCreate(0x10, sizeof(BGPROCINFO));
        InitializeCriticalSection(&bgproc_critical_section);
        bgproc_work_event = CreateEventA(NULL, FALSE, FALSE, bgproc_work_event_name);
        bgproc_resume_event = CreateEventA(NULL, TRUE, TRUE, bgproc_resume_event_name);
        bgproc_active = NULL;
        STUBBED_NAMED("bgProcInit: create the suspended worker with a 64 KiB stack");
        STUBBED_NAMED("bgProcInit: set worker priority to -1 and resume it");
    }
}

void __cdecl bgprocFreeze(void) {
    bgproc_freeze = 1;
    ResetEvent(bgproc_resume_event);
    SetEvent(bgproc_work_event);
}

void __cdecl bgprocUnFreeze(void) {
    bgproc_freeze = 0;
    SetEvent(bgproc_resume_event);
}

i32 __cdecl bgprocIsFrozen(void) {
    return bgproc_frozen != 0;
}

i32 __cdecl bgprocIsFreezing(void) {
    return bgproc_frozen != 0 || bgproc_freeze != 0;
}

i32 __cdecl bgProcIsBgThread(void) {
    return GetCurrentThreadId() == bgproc_thread_id;
}
