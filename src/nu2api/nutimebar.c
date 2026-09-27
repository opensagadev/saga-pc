#include "saga.h"

typedef struct TimeBarSet {
    i32 uses_buffer;
    i32 slot_count;
    i32 *accumulators[2];
    NuTime *start_times;
    i32 *field_14;
    i32 *field_18;
    i32 *colours;
    i32 *toggle_flags;
    const char **slot_names;
    i32 field_28;
} TimeBarSet;

STATIC_ASSERT(sizeof(TimeBarSet) == 0x2c);
STATIC_ASSERT(offsetof(TimeBarSet, slot_count) == 4);
STATIC_ASSERT(offsetof(TimeBarSet, accumulators) == 8);
STATIC_ASSERT(offsetof(TimeBarSet, start_times) == 0x10);
STATIC_ASSERT(offsetof(TimeBarSet, colours) == 0x1c);
STATIC_ASSERT(offsetof(TimeBarSet, toggle_flags) == 0x20);
STATIC_ASSERT(offsetof(TimeBarSet, slot_names) == 0x24);
STATIC_ASSERT(offsetof(TimeBarSet, field_28) == 0x28);

void __cdecl NuTimeGetTicksPS(u32 *low, u32 *high) {
    LARGE_INTEGER counter;
    i64 elapsed;
    QueryPerformanceCounter(&counter);
    if (counter.QuadPart > NuTime_TickBase) {
        elapsed = counter.QuadPart - NuTime_TickBase;
    } else {
        elapsed = counter.QuadPart + (0x7fffffffffffffffLL - NuTime_TickBase);
    }
    *low = (u32)elapsed;
    *high = (u32)(elapsed >> 32);
}

__declspec(noinline) void __cdecl NuTimeGet(NuTime *time) {
    NuTimeGetTicksPS(&time->low, &time->high);
}

void __cdecl NuTimeBarSlotReset(i32 set, i32 slot) {
    TimeBarSet *timebar;
    if (NuTimeBar_Initialised == 0) {
        return;
    }

    if (set == -1 && slot == 0) {
        timebar = (TimeBarSet *)NuTimeBar_SetList[0];
        if (timebar->slot_count > 0) {
            i32 index = 0;
            do {
                timebar->toggle_flags[index] ^= 1;
                timebar->accumulators[timebar->toggle_flags[index] == 0][index] = 0;
                ++index;
            } while (index < timebar->slot_count);
        }
        return;
    }

    timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
    timebar->toggle_flags[slot] ^= 1;
    timebar->accumulators[timebar->toggle_flags[slot] == 0][slot] = 0;
}

void __cdecl _NuTimeBarSlotBegin(i32 set, i32 slot, const char *name) {
    TimeBarSet *timebar;
    if (NuTimeBar_Initialised == 0) {
        return;
    }
    timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
    NuTimeGet(&timebar->start_times[slot]);
    timebar->slot_names[slot] = name;
}
