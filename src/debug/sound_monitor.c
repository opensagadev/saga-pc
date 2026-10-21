/* Debug-only request tuple observer; native tuple writes and copy remain unchanged. */
#define WIN32_LEAN_AND_MEAN
#include "diagnostics.h"
#include <errno.h>

struct sound_tuple_pc {
    const void *source;
    const void *destination;
    int count;
};
static struct sound_tuple_pc sound_expected = {0};
static const void *sound_setter_caller = NULL;
static const char *sound_last_stage = "setter";
static DWORD sound_last_thread = 0, sound_setter_thread = 0;
static volatile LONG sound_lock = 0;
static volatile LONG sound_active_setters = 0, sound_generation = 0;
static volatile LONG sound_baseline_valid = 0, sound_disabled = 0, sound_reported = 0;
static volatile LONG sound_skip_reported[5] = {0};
static volatile LONG sound_ready_reported = 0;

/* Nonblocking observer lock: no native writer waits, no log is emitted while held. */
static int SoundTryLock(void) {
    return InterlockedCompareExchange(&sound_lock, 1, 0) == 0;
}
static void SoundUnlock(void) {
    InterlockedExchange(&sound_lock, 0);
}
static void SoundSkip(int reason, const char *stage, DWORD thread) {
    static const char *const names[5] = {"concurrent-setters-disabled", "no-published-baseline", "setter-active",
                                         "observer-busy", "setter-generation-changed"};
    if (InterlockedExchange(&sound_skip_reported[reason], 1) == 0)
        LOG_INFO("Sound observer unavailable: reason=%s checkpoint=%s thread=%lu; tuple not certified", names[reason],
                 stage, thread);
}

void __cdecl SagaDebugSoundSetterBeginPc(void) {
    int saved_errno = errno;
    DWORD saved_error = GetLastError();
    LONG active = InterlockedIncrement(&sound_active_setters);
    InterlockedExchange(&sound_baseline_valid, 0);
    InterlockedIncrement(&sound_generation);
    if (active != 1)
        InterlockedExchange(&sound_disabled, 1);
    errno = saved_errno;
    SetLastError(saved_error);
}

void __cdecl SagaDebugSoundSetterEndPc(const void *source, int count, const void *destination, const void *caller) {
    int saved_errno = errno;
    DWORD saved_error = GetLastError();
    DWORD thread = GetCurrentThreadId();
    int published = 0;
    if (sound_disabled == 0 && SoundTryLock()) {
        if (sound_disabled == 0) {
            sound_expected.source = source;
            sound_expected.count = count;
            sound_expected.destination = destination;
            sound_setter_caller = caller;
            sound_setter_thread = thread;
            sound_last_stage = "setter";
            sound_last_thread = thread;
            InterlockedExchange(&sound_baseline_valid, 1);
            published = 1;
        }
        SoundUnlock();
    }
    InterlockedIncrement(&sound_generation);
    InterlockedDecrement(&sound_active_setters);
    LOG_INFO("Sound request setter: thread=%lu caller=%p source=%p count=%d (0x%08x) destination=%p baseline=%s",
             thread, caller, source, count, (unsigned)count, destination,
             published ? "published at setter end" : "unavailable; no certified baseline");
    errno = saved_errno;
    SetLastError(saved_error);
}

void __cdecl SagaDebugSoundCheckpointPc(const char *stage, void *volatile const *source, volatile const int *count,
                                        void *volatile const *destination) {
    int saved_errno = errno;
    DWORD saved_error = GetLastError();
    DWORD thread = GetCurrentThreadId(), setter_thread = 0, previous_thread = 0;
    LONG generation = 0;
    int skip = -1, diverged = 0, ready = 0;
    struct sound_tuple_pc observed, expected;
    const void *caller = NULL;
    const char *previous = NULL;
    if (sound_disabled != 0) {
        skip = 0;
        goto done;
    }
    if (!SoundTryLock()) {
        skip = 3;
        goto done;
    }
    if (sound_disabled != 0) {
        skip = 0;
        goto unlock;
    }
    if (sound_active_setters != 0) {
        skip = 2;
        goto unlock;
    }
    if (sound_baseline_valid == 0) {
        skip = 1;
        goto unlock;
    }
    generation = InterlockedCompareExchange(&sound_generation, 0, 0);
    observed.source = *source;
    observed.count = *count;
    observed.destination = *destination;
    expected = sound_expected;
    caller = sound_setter_caller;
    setter_thread = sound_setter_thread;
    previous = sound_last_stage;
    previous_thread = sound_last_thread;
    if (sound_disabled != 0 || sound_active_setters != 0 || sound_baseline_valid == 0 ||
        InterlockedCompareExchange(&sound_generation, 0, 0) != generation) {
        skip = 4;
        goto unlock;
    }
    diverged = observed.source != expected.source || observed.count != expected.count ||
               observed.destination != expected.destination;
    if (diverged)
        diverged = InterlockedExchange(&sound_reported, 1) == 0;
    else {
        sound_last_stage = stage;
        sound_last_thread = thread;
        ready = InterlockedExchange(&sound_ready_reported, 1) == 0;
    }
unlock:
    SoundUnlock();
done:
    if (skip >= 0)
        SoundSkip(skip, stage, thread);
    else if (diverged)
        LOG_WARN("Sound request first observed divergence: previous-check=%s previous-thread=%lu checkpoint=%s "
                 "thread=%lu setter-thread=%lu setter-caller=%p generation=%ld "
                 "expected source=%p count=%d (0x%08x) destination=%p; observed source=%p count=%d (0x%08x) "
                 "destination=%p; observation interval only, writer unknown, native copy unchanged",
                 previous, previous_thread, stage, thread, setter_thread, caller, generation, expected.source,
                 expected.count, (unsigned)expected.count, expected.destination, observed.source, observed.count,
                 (unsigned)observed.count, observed.destination);
    else if (ready)
        LOG_INFO("Sound observer first accepted checkpoint: checkpoint=%s thread=%lu setter-thread=%lu generation=%ld",
                 stage, thread, setter_thread, generation);
    errno = saved_errno;
    SetLastError(saved_error);
}
