#include "saga.h"

STATIC_ASSERT(sizeof(CRITICAL_SECTION) == 0x18);

/* PC-only state for the critical-section wrappers; their higher-level names
 * remain provisional until the original thread initialization path is mapped. */
extern i32 PCThreadCriticalSectionCount;
STATIC_ASSERT(sizeof(PCThreadCriticalSections) == 0x138);

i32 PCThreadCriticalSectionCreate_candidate(void) {
    if (PCThreadCriticalSectionCount <= 12) {
        ++PCThreadCriticalSectionCount;
        InitializeCriticalSection(&PCThreadCriticalSections[PCThreadCriticalSectionCount - 1]);
        return PCThreadCriticalSectionCount;
    }

    return 0;
}

// noinline: the original calls NuThreadCriticalSectionBegin out of line from
// PCDisplayListCriticalSectionEnterPc; VC8 would inline it there.
__declspec(noinline) void NuThreadCriticalSectionBegin(i32 section_index) {
    if (section_index >= 1 && section_index <= 12) {
        EnterCriticalSection(&PCThreadCriticalSections[section_index - 1]);
    }
}

// noinline: the original calls NuThreadCriticalSectionEnd out of line from
// PCDisplayListCriticalSectionLeavePc; VC8 would inline it there.
__declspec(noinline) void NuThreadCriticalSectionEnd(i32 section_index) {
    if (section_index >= 1 && section_index <= 12) {
        LeaveCriticalSection(&PCThreadCriticalSections[section_index - 1]);
    }
}

void PCDisplayListCriticalSectionEnterPc(void) {
    NuThreadCriticalSectionBegin(nudlist_field_fc4);
}

void PCDisplayListCriticalSectionLeavePc(void) {
    NuThreadCriticalSectionEnd(nudlist_field_fc4);
}

void fun_005de1c0(NUMTL *material) {
    NuThreadCriticalSectionBegin(nudlist_field_fc4);
    if (PCDisplayMaterialAddCount == 600) {
        NuThreadCriticalSectionEnd(nudlist_field_fc4);
        AnimationDiagnosticNoOpPc();
        return;
    }
    PCDisplayMaterialAddQueue[PCDisplayMaterialAddCount++] = material;
    NuThreadCriticalSectionEnd(nudlist_field_fc4);
}

i64 PCThreadMillisecondsPc(void) {
    NuTime time;
    NuTimeGet(&time);
    return (((i64)time.high << 32) | time.low) / 1000;
}

void PCThreadCriticalSectionDeleteNoOpPc(i32 index) {
    (void)index;
}

void NuTimeGetTicksPerSecondPS(u32 *low, u32 *high) {
    *low = (u32)NuTime_PerformanceFrequency;
    *high = (u32)(NuTime_PerformanceFrequency >> 32);
}
