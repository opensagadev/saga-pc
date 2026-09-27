#include "saga.h"

STATIC_ASSERT(sizeof(CRITICAL_SECTION) == 0x18);

/* PC-only state for the critical-section wrappers; their higher-level names
 * remain provisional until the original thread initialization path is mapped. */
extern i32 PCThreadCriticalSectionCount_candidate;
extern CRITICAL_SECTION PCThreadCriticalSections_candidate[13];
extern i32 PCFrameCriticalSectionIndex_candidate;
STATIC_ASSERT(sizeof(PCThreadCriticalSections_candidate) == 0x138);

i32 __cdecl PCThreadCriticalSectionCreate_candidate(void) {
    if (PCThreadCriticalSectionCount_candidate <= 12) {
        ++PCThreadCriticalSectionCount_candidate;
        InitializeCriticalSection(&PCThreadCriticalSections_candidate[PCThreadCriticalSectionCount_candidate]);
        return PCThreadCriticalSectionCount_candidate;
    }

    return 0;
}

__declspec(noinline) void __cdecl PCThreadCriticalSectionEnter_candidate(i32 section_index) {
    if (section_index >= 1 && section_index <= 12) {
        EnterCriticalSection(&PCThreadCriticalSections_candidate[section_index]);
    }
}

__declspec(noinline) void __cdecl PCThreadCriticalSectionLeave_candidate(i32 section_index) {
    if (section_index >= 1 && section_index <= 12) {
        LeaveCriticalSection(&PCThreadCriticalSections_candidate[section_index]);
    }
}

void __cdecl PCFrameCriticalSectionEnter_candidate(void) {
    PCThreadCriticalSectionEnter_candidate(PCFrameCriticalSectionIndex_candidate);
}

void __cdecl PCFrameCriticalSectionLeave_candidate(void) {
    PCThreadCriticalSectionLeave_candidate(PCFrameCriticalSectionIndex_candidate);
}
