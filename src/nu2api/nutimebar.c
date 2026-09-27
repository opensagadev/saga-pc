#include "saga.h"

__declspec(noinline) void __cdecl NuTimeSub(NuTime *time, NuTime *a, NuTime *b) {
    if (a->low >= b->low) {
        time->low = a->low - b->low;
        time->high = a->high - b->high;
    } else {
        time->low = a->low - b->low;
        time->high = a->high - 1 - b->high;
    }
}

void __cdecl NuTimeGetTicksPerSecondPS(u32 *low, u32 *high) {
    *low = (u32)NuTime_PerformanceFrequency;
    *high = (u32)(NuTime_PerformanceFrequency >> 32);
}

f32 __cdecl NuTimeSeconds(NuTime *time) {
    u32 low;
    u32 high;
    f64 ticks;
    f64 ticks_per_second;

    NuTimeGetTicksPerSecondPS(&low, &high);
    ticks = time->low + time->high * NuTime_LowScale;
    ticks_per_second = low + high * NuTime_LowScale;
    return (f32)(ticks / ticks_per_second);
}

f32 __cdecl NuTimeMilliSeconds(NuTime *time) {
    u32 low;
    u32 high;
    f64 ticks;
    f64 ticks_per_second;

    NuTimeGetTicksPerSecondPS(&low, &high);
    ticks = time->low + time->high * NuTime_LowScale;
    ticks_per_second = low + high * NuTime_LowScale;
    return (f32)(ticks / ticks_per_second * NuScale1000);
}

f32 __cdecl NuTimeMicroSeconds(NuTime *time) {
    u32 low;
    u32 high;
    f64 ticks;
    f64 ticks_per_second;

    NuTimeGetTicksPerSecondPS(&low, &high);
    ticks = time->low + time->high * NuTime_LowScale;
    ticks_per_second = low + high * NuTime_LowScale;
    return (f32)(ticks / ticks_per_second * NuTime_MicroScale);
}

f32 __cdecl NuTimeScanlines(NuTime *time) {
    return (f32)(NuTimeSeconds(time) * NuTime_ScanlineScale * NuTime_RefreshRate);
}

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
        elapsed = (counter.QuadPart - NuTime_TickBase) + 0x7fffffffffffffffLL;
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

void __cdecl NuTimeBarDestroySet(i32 set) {
    TimeBarSet *timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];

    /* Both retail originals free the allocations when this word is nonzero. */
    if (timebar->uses_buffer != 0) {
        NuFree(timebar->accumulators[0], NuTimeBarSource, 0x169);
        NuFree(timebar->accumulators[1], NuTimeBarSource, 0x16a);
        NuFree(timebar->start_times, NuTimeBarSource, 0x16b);
        NuFree(timebar->field_14, NuTimeBarSource, 0x16c);
        NuFree(timebar->field_18, NuTimeBarSource, 0x16d);
        NuFree(timebar->colours, NuTimeBarSource, 0x16e);
        NuFree(timebar->toggle_flags, NuTimeBarSource, 0x16f);
        NuFree((void *)timebar->slot_names, NuTimeBarSource, 0x170);
        NuFree(timebar, NuTimeBarSource, 0x172);
    }
    NuTimeBar_SetList[set + 1] = NULL;
}

u32 __cdecl _NuTimeBarSlotEnd(i32 set, i32 slot) {
    TimeBarSet *timebar;
    NuTime now;
    NuTime delta;
    f32 elapsed;
    i32 accumulator;

    if (NuTimeBar_Initialised == 0) {
        return 0;
    }
    timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
    NuTimeGet(&now);
    NuTimeSub(&delta, &now, &timebar->start_times[slot]);
    elapsed = NuTimeMicroSeconds(&delta);
    accumulator = 1 - timebar->toggle_flags[slot];
    timebar->accumulators[accumulator][slot] += (u32)elapsed;
    return (u32)timebar->accumulators[1 - timebar->toggle_flags[slot]][slot];
}

void __cdecl NuTimeBarSlotSet(i32 set, i32 slot, i32 value) {
    TimeBarSet *timebar;

    if (NuTimeBar_Initialised == 0) {
        return;
    }
    timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
    timebar->accumulators[1 - timebar->toggle_flags[slot]][slot] = value;
}

void __cdecl NuTimeBarSlotSetName(i32 set, i32 slot, const char *name) {
    TimeBarSet *timebar;

    if (NuTimeBar_Initialised == 0) {
        return;
    }
    timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
    timebar->slot_names[slot] = name;
}

void __cdecl NuTimeBarSlotSetEx(i32 set, i32 slot, i32 value, const char *name) {
    TimeBarSet *timebar;

    if (NuTimeBar_Initialised == 0) {
        return;
    }
    if (set == -1 && slot == 6) {
        value = (i32)(u32)((f64)(u32)value * NuTimeBarScanlineMicroseconds);
    }
    timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
    timebar->accumulators[1 - timebar->toggle_flags[slot]][slot] = value;
    timebar->slot_names[slot] = name;
}

i32 __cdecl NuTimeBarSlotLastValueMicroseconds(i32 set, i32 slot) {
    TimeBarSet *timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
    return timebar->accumulators[timebar->toggle_flags[slot]][slot];
}

i32 __cdecl NuTimeBarSlotLastValue(i32 set, i32 slot) {
    u32 elapsed = (u32)NuTimeBarSlotLastValueMicroseconds(set, slot);
    f32 frames = (f32)((f64)elapsed * NuTime_RefreshRate / NuTime_MicroScale);
    return (i32)((f64)frames * NuTimeBarScanlinesPerFrame);
}

void __cdecl NuTimeBarResetPeaks(void) {
    NuTimeBar_PeakReset = 1;
}

void __cdecl NuTimeBarEnable(i32 enabled) {
    NuTimeBar_EngineEnabled = enabled;
}

void __cdecl NuTimeBarIndicateGpuFrameOut(i32 enabled) {
    NuTimeBar_GpuFrameOutEnabled = enabled;
}

static i32 CreateTimeBar(VARIPTR *buffer, VARIPTR unused_buffer, i32 *colours, i32 slot_count) {
    TimeBarSet *timebar;
    i32 index;
    i32 set;
    u32 array_bytes = (u32)slot_count * sizeof(i32);
    (void)unused_buffer;

    if (buffer != NULL) {
        buffer->addr = (buffer->addr + 3) & ~3u;
        timebar = (TimeBarSet *)buffer->void_ptr;
        buffer->addr += sizeof(TimeBarSet);
        memset(timebar, 0, sizeof(TimeBarSet));
        buffer->addr = (buffer->addr + 3) & ~3u;
        timebar->accumulators[0] = (i32 *)buffer->void_ptr;
        buffer->addr += array_bytes * 1;
        buffer->addr = (buffer->addr + 3) & ~3u;
        timebar->accumulators[1] = (i32 *)buffer->void_ptr;
        buffer->addr += array_bytes * 1;
        buffer->addr = (buffer->addr + 3) & ~3u;
        timebar->start_times = (NuTime *)buffer->void_ptr;
        buffer->addr += array_bytes * 2;
        buffer->addr = (buffer->addr + 3) & ~3u;
        timebar->field_14 = (i32 *)buffer->void_ptr;
        buffer->addr += array_bytes * 1;
        buffer->addr = (buffer->addr + 3) & ~3u;
        timebar->field_18 = (i32 *)buffer->void_ptr;
        buffer->addr += array_bytes * 1;
        buffer->addr = (buffer->addr + 3) & ~3u;
        timebar->colours = (i32 *)buffer->void_ptr;
        buffer->addr += array_bytes * 1;
        buffer->addr = (buffer->addr + 3) & ~3u;
        timebar->toggle_flags = (i32 *)buffer->void_ptr;
        buffer->addr += array_bytes * 1;
        buffer->addr = (buffer->addr + 3) & ~3u;
        timebar->slot_names = (const char **)buffer->void_ptr;
        buffer->addr += array_bytes * 1;
        timebar->uses_buffer = 1;
    } else {
        timebar = (TimeBarSet *)NuAlloc(sizeof(TimeBarSet), NuTimeBarSource, 0xf2);
        memset(timebar, 0, sizeof(TimeBarSet));
        timebar->accumulators[0] = (i32 *)NuAlloc(array_bytes * 1, NuTimeBarSource, 0xf5);
        timebar->accumulators[1] = (i32 *)NuAlloc(array_bytes * 1, NuTimeBarSource, 0xf6);
        timebar->start_times = (NuTime *)NuAlloc(array_bytes * 2, NuTimeBarSource, 0xf7);
        timebar->field_14 = (i32 *)NuAlloc(array_bytes * 1, NuTimeBarSource, 0xf8);
        timebar->field_18 = (i32 *)NuAlloc(array_bytes * 1, NuTimeBarSource, 0xf9);
        timebar->colours = (i32 *)NuAlloc(array_bytes * 1, NuTimeBarSource, 0xfa);
        timebar->toggle_flags = (i32 *)NuAlloc(array_bytes * 1, NuTimeBarSource, 0xfb);
        timebar->slot_names = (const char **)NuAlloc(array_bytes * 1, NuTimeBarSource, 0xfc);
        timebar->uses_buffer = 0;
    }
    memset((void *)timebar->accumulators[0], 0, array_bytes);
    memset((void *)timebar->accumulators[1], 0, array_bytes);
    /* PC clears four bytes per slot despite allocating an eight-byte NuTime. */
    memset((void *)timebar->start_times, 0, array_bytes);
    memset((void *)timebar->field_14, 0, array_bytes);
    memset((void *)timebar->field_18, 0, array_bytes);
    memset((void *)timebar->colours, 0, array_bytes);
    memset((void *)timebar->toggle_flags, 0, array_bytes);
    memset((void *)timebar->slot_names, 0, array_bytes);
    set = 0;
    for (index = 0; index < 16; ++index) {
        if (NuTimeBar_SetList[index] == NULL) {
            NuTimeBar_SetList[index] = timebar;
            set = index - 1;
            break;
        }
    }
    if (index == 16) {
        STUBBED_NAMED(0x005c12e0);
    }
    if (colours != NULL) {
        memcpy(timebar->colours, colours, array_bytes);
    } else if (set == -1) {
        memcpy(timebar->colours, NuTimeBar_EngineColours, array_bytes);
    } else {
        memcpy(timebar->colours, NuTimeBar_DefaultColours, array_bytes);
    }
    timebar->slot_count = slot_count;
    timebar->field_28 = set > 0;
    return set;
}

i32 __cdecl NuTimeBarCreateSetEx(VARIPTR *buffer, VARIPTR unused_buffer, i32 *colours) {
    return CreateTimeBar(buffer, unused_buffer, colours, 16);
}

i32 __cdecl NuTimeBarCreateSetEx2(VARIPTR *buffer, VARIPTR unused_buffer, i32 field_28) {
    i32 set = CreateTimeBar(buffer, unused_buffer, NULL, 16);
    TimeBarSet *timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
    timebar->field_28 = field_28;
    return set;
}

i32 __cdecl NuTimeBarCreateSet(i32 *colours) {
    VARIPTR unused_buffer;
    unused_buffer.addr = 0;
    return CreateTimeBar(NULL, unused_buffer, colours, 16);
}

__declspec(noinline) void __cdecl NuTimeBarInitEx(VARIPTR *buffer, VARIPTR unused_buffer) {
    memset(NuTimeBar_SetList, 0, sizeof(NuTimeBar_SetList));
    CreateTimeBar(buffer, unused_buffer, NULL, 7);
    CreateTimeBar(buffer, unused_buffer, NULL, 16);

    STUBBED_NAMED(0x005e4b20); /* PC material creator uses EAX/ECX and one stack argument. */
    STUBBED_NAMED("NuTimeBarInitEx: material attributes at +0x40");
    STUBBED_NAMED(0x00601030);
    STUBBED_NAMED("NuTimeBarInitEx: material reference count at +0x2c0");
    NuTimeBar_Initialised = 1;
}

void __cdecl NuTimeBarInit(void) {
    VARIPTR unused_buffer;
    unused_buffer.addr = 0;
    NuTimeBarInitEx(NULL, unused_buffer);
}
