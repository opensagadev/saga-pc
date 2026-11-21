#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nucore/nutime.h"
#include "nu2api/nudir_gen.h"
#include "nu2api/numainfilter_gen.h"
#include "nu2api/numath/numaths.h"

/* PC-only state for the critical-section wrappers; their higher-level names
 * remain provisional until the original thread initialization path is mapped. */
extern i32 PCThreadCriticalSectionCount;

i64 NuTime_PerformanceFrequency = 0;
CRITICAL_SECTION PCThreadCriticalSections[12] = {0};

i32 PCThreadCriticalSectionCreate_candidate(void) {
    if (PCThreadCriticalSectionCount <= 12) {
        ++PCThreadCriticalSectionCount;
        InitializeCriticalSection(&PCThreadCriticalSections[PCThreadCriticalSectionCount - 1]);
        return PCThreadCriticalSectionCount;
    }

    return 0;
}

// noinline: the original calls NuThreadCriticalSectionBegin out of line from
// PCDisplayListCriticalSectionEnter; VC8 would inline it there.
__declspec(noinline) void NuThreadCriticalSectionBegin(i32 section_index) {
    if (section_index >= 1 && section_index <= 12) {
        EnterCriticalSection(&PCThreadCriticalSections[section_index - 1]);
    }
}

// noinline: the original calls NuThreadCriticalSectionEnd out of line from
// PCDisplayListCriticalSectionLeave; VC8 would inline it there.
__declspec(noinline) void NuThreadCriticalSectionEnd(i32 section_index) {
    if (section_index >= 1 && section_index <= 12) {
        LeaveCriticalSection(&PCThreadCriticalSections[section_index - 1]);
    }
}

void PCDisplayListCriticalSectionEnter(void) {
    NuThreadCriticalSectionBegin(nudlist_field_fc4);
}

void PCDisplayListCriticalSectionLeave(void) {
    NuThreadCriticalSectionEnd(nudlist_field_fc4);
}

void NuDisplayQueueMaterialAdd(NUMTL *material) {
    NuThreadCriticalSectionBegin(nudlist_field_fc4);
    if (PCDisplayMaterialAddCount == 600) {
        NuThreadCriticalSectionEnd(nudlist_field_fc4);
        AnimationDiagnosticNoOp();
        return;
    }
    PCDisplayMaterialAddQueue[PCDisplayMaterialAddCount++] = material;
    NuThreadCriticalSectionEnd(nudlist_field_fc4);
}

i64 PCThreadMilliseconds(void) {
    NuTime time;
    NuTimeGet(&time);
    return (((i64)time.high << 32) | time.low) / 1000;
}

void PCThreadCriticalSectionDeleteNoOp(i32 index) {
}

void NuTimeGetTicksPerSecondPS(u32 *low, u32 *high) {
    *low = (u32)NuTime_PerformanceFrequency;
    *high = (u32)(NuTime_PerformanceFrequency >> 32);
}
