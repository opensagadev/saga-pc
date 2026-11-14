#include "saga.h"

/* PC timing leaves of nutimebar outside nu3d; the time bars themselves live
 * in nu3d_includes.cpp. */

void NuTimeGetTicksPS(u32 *low, i32 *high) {
    LARGE_INTEGER counter;
    i64 elapsed;
    QueryPerformanceCounter(&counter);
    if (counter.QuadPart > NuTime_TickBase) {
        elapsed = counter.QuadPart - NuTime_TickBase;
    } else {
        elapsed = (counter.QuadPart - NuTime_TickBase) + 0x7fffffffffffffffLL;
    }
    *low = (u32)elapsed;
    *high = (i32)(elapsed >> 32);
}

void PcInitializeStartupCriticalSection(void) {
    if (PcStartupCriticalSectionInitialized == 0) {
        InitializeCriticalSection(&PcStartupCriticalSection);
        PcStartupCriticalSectionInitialized = 1;
    }
}

void PcInitializePerformanceTiming(void) {
    DWORD_PTR process_mask = 1;
    DWORD_PTR system_mask = 1;
    if (!GetProcessAffinityMask(GetCurrentProcess(), &process_mask, &system_mask)) {
        process_mask = 1;
    }
    SetProcessAffinityMask(GetCurrentProcess(), 1);
    QueryPerformanceCounter((LARGE_INTEGER *)&NuTime_TickBase);
    QueryPerformanceFrequency((LARGE_INTEGER *)&NuTime_PerformanceFrequency);
    SetProcessAffinityMask(GetCurrentProcess(), process_mask);
}
