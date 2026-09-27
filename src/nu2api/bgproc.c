#include "saga.h"

void __cdecl bgProcInit(void) {
    if (bgproc_initialized == 0) {
        bgproc_initialized = 1;
        STUBBED_NAMED("bgProcInit: NuLstCreate(16, sizeof(BGPROCINFO))");
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
