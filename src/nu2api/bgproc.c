#include "saga.h"

NULSTHDR *bgproc_procinfo_pool = 0;
volatile u32 nu_current_thread_id = 0;
void __cdecl PcEnableThreadSwap(void) {
}
void __cdecl PcDisableThreadSwap(void) {
}

void(__cdecl *NuThreadDisableThreadSwap)(void) = PcDisableThreadSwap;
void(__cdecl *NuThreadEnableThreadSwap)(void) = PcEnableThreadSwap;

BGPROCINFO *__cdecl bgPostRequestV(bgprocdofn *do_fn, bgprocdofn *ack_fn, i32 first_type, ...) {
    BGPROCINFO *info;
    BGVAR *var;
    u8 *string_end;
    i32 *argument;
    i32 type;

    EnterCriticalSection(&bgproc_critical_section);
    info = (BGPROCINFO *)NuLstAllocTail(bgproc_procinfo_pool);
    if (info != NULL) {
        info->work_started = 0;
        info->unknown_flag_2 = 0;
        var = (BGVAR *)info->data;
        string_end = (u8 *)&info->vars;
        info->vars = var;
        info->do_fn = do_fn;
        info->ack_fn = ack_fn;
        info->var_count = 0;
        type = first_type;
        argument = &first_type;
        while (type != 7) {
            var->type = type;
            switch (type) {
                case 1:
                case 5:
                    var->value.i32_value = *++argument;
                    break;
                case 2:
                    ++argument;
                    var->value.float_value = *(f32 *)argument;
                    break;
                case 3: {
                    char *string = *(char **)++argument;
                    string_end -= (u32)NuStrLen(string) + 1u;
                    var->value.string_value = (char *)string_end;
                    NuStrCpy((char *)string_end, string);
                    break;
                }
                case 4:
                    var->value.u32_value = *(u32 *)++argument;
                    break;
                case 6:
                    var->value.i32_value = *++argument;
                    break;
                default:
                    bgProcDataOversize(type);
                    break;
            }
            type = *++argument;
            ++info->var_count;
            ++var;
        }
    }
    if (info == NULL || (u8 *)var > string_end) {
        ((void(__cdecl *)(void))bgProcDataOversize)();
    }
    LeaveCriticalSection(&bgproc_critical_section);
    if (info != NULL) {
        SetEvent(bgproc_work_event);
    }
    return info;
}

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

extern HANDLE bgproc_thread_handle_pc;

/* Native Windows worker: the callback never returns during process lifetime. */
static DWORD WINAPI bgProcWorkerPc(void *unused) {
    (void)unused;
    WaitForSingleObject(bgproc_work_event, INFINITE);
    EnterCriticalSection(&bgproc_critical_section);
    bgproc_active = (BGPROCINFO *)NuLstGetNext(bgproc_procinfo_pool, NULL);
    LeaveCriticalSection(&bgproc_critical_section);
    for (;;) {
        if (WaitForSingleObject(bgproc_resume_event, 0) == WAIT_TIMEOUT) {
            bgproc_frozen = 1;
            WaitForSingleObject(bgproc_resume_event, INFINITE);
            bgproc_frozen = 0;
        }
        if (bgproc_active != NULL) {
            bgproc_active->work_started = 1;
            if (bgproc_active->do_fn != NULL) {
                bgproc_active->do_fn(bgproc_active);
            }
            if (bgproc_active->ack_fn != NULL) {
                bgproc_active->ack_fn(bgproc_active);
            }
        }
        EnterCriticalSection(&bgproc_critical_section);
        if (bgproc_active != NULL) {
            NuLstFree((NULNKHDR *)bgproc_active);
        }
        bgproc_active = (BGPROCINFO *)NuLstGetNext(bgproc_procinfo_pool, NULL);
        LeaveCriticalSection(&bgproc_critical_section);
        while (bgproc_active == NULL) {
            if (WaitForSingleObject(bgproc_resume_event, 0) == WAIT_TIMEOUT) {
                bgproc_frozen = 1;
                WaitForSingleObject(bgproc_resume_event, INFINITE);
                bgproc_frozen = 0;
            }
            WaitForSingleObject(bgproc_work_event, INFINITE);
            EnterCriticalSection(&bgproc_critical_section);
            bgproc_active = (BGPROCINFO *)NuLstGetNext(bgproc_procinfo_pool, NULL);
            LeaveCriticalSection(&bgproc_critical_section);
        }
    }
}

void __cdecl bgProcInit(void) {
    if (bgproc_initialized == 0) {
        bgproc_initialized = 1;
        bgproc_procinfo_pool = NuLstCreate(0x10, sizeof(BGPROCINFO));
        InitializeCriticalSection(&bgproc_critical_section);
        bgproc_work_event = CreateEventA(NULL, FALSE, FALSE, bgproc_work_event_name);
        bgproc_resume_event = CreateEventA(NULL, TRUE, TRUE, bgproc_resume_event_name);
        bgproc_active = NULL;
        bgproc_thread_handle_pc =
            CreateThread(NULL, 0x10000, bgProcWorkerPc, NULL, CREATE_SUSPENDED, &bgproc_thread_id);
        SetThreadPriority(bgproc_thread_handle_pc, -1);
        ResumeThread(bgproc_thread_handle_pc);
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

BGPROCINFO *__cdecl bgGetProcActive(void) {
    if (bgproc_active != NULL)
        return bgproc_active;
    return (BGPROCINFO *)NuLstGetNext(bgproc_procinfo_pool, NULL);
}

void __cdecl bgprocFrameSleepPc(i32 milliseconds) {
    if (bgproc_procinfo_pool != NULL) {
        if (bgproc_active != NULL || NuLstGetNext(bgproc_procinfo_pool, NULL) != NULL) {
            Sleep(milliseconds);
        }
    }
}
