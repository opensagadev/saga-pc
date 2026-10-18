/* Debug-only sound request tuple observer; never changes the native request copy. */
#define WIN32_LEAN_AND_MEAN
#include "diagnostics.h"
#include <errno.h>

/* Observe the request-table tuple, never the memory addressed by its pointers.
 * These hooks are Debug support; Matching discards every call and argument. */
struct sound_tuple_pc {
    const void *source;
    const void *destination;
    int count;
};
static struct sound_tuple_pc sound_expected = {0};
static const void *sound_setter_caller = NULL;
static const char *sound_last_stage = "setter";
static volatile LONG sound_owner_thread = 0;
static volatile LONG sound_active_setters = 0;
static volatile LONG sound_generation = 0;
static volatile LONG sound_baseline_valid = 0;
static volatile LONG sound_disabled = 0;
static volatile LONG sound_reported = 0;

void __cdecl SagaDebugSoundSetterBeginPc(void) {
    int saved_errno = errno;
    DWORD saved_error = GetLastError();
    LONG thread = (LONG)GetCurrentThreadId();
    LONG owner = InterlockedCompareExchange(&sound_owner_thread, thread, 0);
    LONG active = InterlockedIncrement(&sound_active_setters);
    if ((owner != 0 && owner != thread) || active != 1)
        InterlockedExchange(&sound_disabled, 1);
    InterlockedIncrement(&sound_generation);
    errno = saved_errno;
    SetLastError(saved_error);
}

void __cdecl SagaDebugSoundSetterEndPc(const void *source, int count, const void *destination, const void *caller) {
    int saved_errno = errno;
    DWORD saved_error = GetLastError();
    if (sound_disabled == 0) {
        sound_expected.source = source;
        sound_expected.count = count;
        sound_expected.destination = destination;
        sound_setter_caller = caller;
        sound_last_stage = "setter";
        InterlockedExchange(&sound_baseline_valid, 1);
    }
    InterlockedIncrement(&sound_generation);
    InterlockedDecrement(&sound_active_setters);
    LOG_INFO("Sound request setter: caller=%p source=%p count=%d (0x%08x) destination=%p monitor=%s", caller, source,
             count, (unsigned)count, destination, sound_disabled ? "unavailable: concurrent setters" : "ready");
    errno = saved_errno;
    SetLastError(saved_error);
}

void __cdecl SagaDebugSoundCheckpointPc(const char *stage, void *volatile const *source, volatile const int *count,
                                        void *volatile const *destination) {
    int saved_errno = errno;
    DWORD saved_error = GetLastError();
    LONG generation;
    struct sound_tuple_pc observed, expected;
    const void *caller;
    const char *previous;
    if (sound_disabled != 0 || sound_baseline_valid == 0 || sound_active_setters != 0 ||
        (LONG)GetCurrentThreadId() != sound_owner_thread)
        goto done;
    generation = InterlockedCompareExchange(&sound_generation, 0, 0);
    observed.source = *source;
    observed.count = *count;
    observed.destination = *destination;
    expected = sound_expected;
    caller = sound_setter_caller;
    previous = sound_last_stage;
    if (sound_disabled != 0 || sound_active_setters != 0 ||
        InterlockedCompareExchange(&sound_generation, 0, 0) != generation)
        goto done;
    if (observed.source != expected.source || observed.count != expected.count ||
        observed.destination != expected.destination) {
        if (InterlockedExchange(&sound_reported, 1) == 0) {
            LOG_WARN("Sound request first observed divergence: previous-check=%s current-check=%s setter-caller=%p "
                     "expected source=%p count=%d (0x%08x) destination=%p; observed source=%p count=%d (0x%08x) "
                     "destination=%p; interval only, writer not identified; native copy unchanged",
                     previous, stage, caller, expected.source, expected.count, (unsigned)expected.count,
                     expected.destination, observed.source, observed.count, (unsigned)observed.count,
                     observed.destination);
        }
    } else {
        sound_last_stage = stage;
    }
done:
    errno = saved_errno;
    SetLastError(saved_error);
}
