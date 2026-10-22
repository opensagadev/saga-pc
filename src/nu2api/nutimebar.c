#include "saga.h"

extern NUMTL *NuTimeBar_FrameOutMtl;

__declspec(noinline) void __cdecl NuTimeSub(NuTime *time, NuTime *a, NuTime *b) {
    if (a->low >= b->low) {
        time->low = a->low - b->low;
        time->high = a->high - b->high;
    } else {
        time->low = a->low - b->low;
        time->high = a->high - 1 - b->high;
    }
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
STATIC_ASSERT(offsetof(TimeBarSet, field_14) == 0x14);
STATIC_ASSERT(offsetof(TimeBarSet, field_18) == 0x18);
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
    index = 0;
    do {
        if (NuTimeBar_SetList[index] == NULL) {
            NuTimeBar_SetList[index] = timebar;
            set = index - 1;
            goto found;
        }
    } while (++index < 16);
    AnimationDiagnosticNoOpPc();
    set = 0;
found:
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
    NUMTL *material;
    memset(NuTimeBar_SetList, 0, sizeof(NuTimeBar_SetList));
    CreateTimeBar(buffer, unused_buffer, NULL, 7);
    CreateTimeBar(buffer, unused_buffer, NULL, 16);
    material = NuMtlCreate(1);
    material->attribs.alpha_mode = 2;
    NuTimeBar_FrameOutMtl = material;
    NuMtlUpdatePS(material);
    ((PC_SCENE_MTL_VIEW *)material)->version += 1;
    NuTimeBar_Initialised = 1;
}

void __cdecl NuTimeBarInit(void) {
    VARIPTR unused_buffer;
    unused_buffer.addr = 0;
    NuTimeBarInitEx(NULL, unused_buffer);
}

void __cdecl NuTimeBarSetRenderHorizontal(i32 set) {
    TimeBarSet *timebar;
    f32 name_width = 0.0f;
    f32 vertical_scale;
    i32 slot;

    if (set == -1 && NuTimeBar_EngineEnabled == 0) {
        return;
    }

    {
        if (NuQFntModeStackIndex < 16) {
            NuQFntModeStack[NuQFntModeStackIndex++] = NuQFntMode;
        }
        NuQFntMode = 2;
        if (NuQFntCSModeStackIndex < 16) {
            NuQFntCSModeStack[NuQFntCSModeStackIndex++] = nuqfnt_cs_mode;
        }
        nuqfnt_cs_mode = 1;
        qfnt_rezscale_w = NuTimeBarQFontScale;
        qfnt_rezscale_h = NuTimeBarQFontScale;
        qfnt_offscale_x = 0.0f;
        qfnt_offscale_y = 0.0f;
        qfnt_len_scale = NuTimeBarQFontSize;
        qfnt_height_scale = NuTimeBarQFontSize;
        if (system_qfont != NULL) {
            system_qfont->mode = 1;
            nuqfnt_space_width = system_qfont->space_width;
        }
    }
    if (system_qfont != NULL) {
        system_qfont->platform_data->colour = 0xe0e0e0e0;
        *system_qfont->x_scale = NuTimeBarFontScale;
        *system_qfont->y_scale = (NuQFntMode & 4) != 0 ? NuTimeBarFontScale : NuTimeBarHalfFontScale;
    }
    vertical_scale = (f32)(NuTimeBarVerticalScale / PS2_VREZ_H);
    timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
    for (slot = 0; slot < timebar->slot_count; ++slot) {
        if (timebar->slot_names[slot] != NULL) {
            f32 width = NuQFntPrintLenU(system_qfont, (char *)timebar->slot_names[slot]);
            if (width > name_width) {
                name_width = width;
            }
        }
    }
    {
        if (NuQFntModeStackIndex > 0) {
            NuQFntMode = NuQFntModeStack[--NuQFntModeStackIndex];
        }
        if (NuQFntCSModeStackIndex > 0) {
            NuQFntSetCoordinateSystem(NuQFntCSModeStack[--NuQFntCSModeStackIndex]);
        }
    }
    name_width /= (f32)(PS2_VREZ_W << 4);

    for (slot = 0; slot < timebar->slot_count; ++slot) {
        u32 value = (u32)timebar->accumulators[timebar->toggle_flags[slot]][slot];
        if (value > (u32)timebar->field_14[slot]) {
            timebar->field_14[slot] = (i32)value;
        }
        if (NuTimeBarHorizontalPeakReset == 0) {
            timebar->field_18[slot] = 0;
        }
        value = (u32)timebar->accumulators[timebar->toggle_flags[slot]][slot];
        if (value > (u32)timebar->field_18[slot]) {
            timebar->field_18[slot] = (i32)value;
        }

        if (timebar->field_18[slot] != 0) {
            f32 left = (f32)(name_width + NuTimeBarNameMargin);
            f32 top = (f32)((slot + 1) * NuTimeBarSlotWidth * 2.0 + NuTimeBarHorizontalTop);
            f32 elapsed_frames = (f32)((f64)(u32)timebar->field_18[slot] * NuTime_RefreshRate / NuTime_MicroScale);
            f32 right = left + elapsed_frames * vertical_scale;
            f32 bottom = (f32)(top + NuTimeBarSlotWidth);
            f32 screen_width = (f32)(PS2_VREZ_W << 4);
            f32 screen_height = (f32)(PS2_VREZ_H << 4);
            i32 x = (i32)(left * screen_width);
            i32 y = (i32)(top * screen_height);
            i32 width = (i32)((right - left) * screen_width);
            i32 height = (i32)((bottom - top) * screen_height);
            i32 text_y;

            NuRndrRect2di(x, y, width, height, timebar->colours[slot], NULL);
            {
                if (NuQFntModeStackIndex < 16) {
                    NuQFntModeStack[NuQFntModeStackIndex++] = NuQFntMode;
                }
                NuQFntMode = 2;
                if (NuQFntCSModeStackIndex < 16) {
                    NuQFntCSModeStack[NuQFntCSModeStackIndex++] = nuqfnt_cs_mode;
                }
                nuqfnt_cs_mode = 1;
                qfnt_rezscale_w = NuTimeBarQFontScale;
                qfnt_rezscale_h = NuTimeBarQFontScale;
                qfnt_offscale_x = 0.0f;
                qfnt_offscale_y = 0.0f;
                qfnt_len_scale = NuTimeBarQFontSize;
                qfnt_height_scale = NuTimeBarQFontSize;
                if (system_qfont != NULL) {
                    system_qfont->mode = 1;
                    nuqfnt_space_width = system_qfont->space_width;
                }
            }
            if (system_qfont != NULL) {
                system_qfont->platform_data->colour = 0xe0e0e0e0;
                *system_qfont->x_scale = NuTimeBarFontScale;
                *system_qfont->y_scale = (NuQFntMode & 4) != 0 ? NuTimeBarFontScale : NuTimeBarHalfFontScale;
            }
            text_y = y + height - (i32)(NuQFntHeight(system_qfont) * NuTimeBarQuarter);
            if (timebar->slot_names[slot] != NULL) {
                NuQFntPrintEx(system_qfont, (i32)(NuTimeBarSlotWidth * (PS2_VREZ_W << 4)), text_y, 0x10,
                              timebar->slot_names[slot]);
            }
            if (timebar->field_18[slot] != 0) {
                f32 lines = (f32)((f64)(u32)timebar->field_18[slot] * NuTimeBarLinesPerSecond / NuTime_MicroScale);
                NuQFntPrintEx(system_qfont, x, text_y, 0x10, Text_TimeS, (i32)lines);
            }
            if (timebar->field_14[slot] != 0) {
                f32 lines = (f32)((f64)(u32)timebar->field_14[slot] * NuTimeBarLinesPerSecond / NuTime_MicroScale);
                NuQFntPrintEx(system_qfont, (i32)(NuTimeBarRight * (PS2_VREZ_W << 4)), text_y, 0x20, Text_TimeS,
                              (i32)lines);
            }
            {
                if (NuQFntModeStackIndex > 0) {
                    NuQFntMode = NuQFntModeStack[--NuQFntModeStackIndex];
                }
                if (NuQFntCSModeStackIndex > 0) {
                    NuQFntSetCoordinateSystem(NuQFntCSModeStack[--NuQFntCSModeStackIndex]);
                }
            }
        }
        if (timebar->field_28 != 0) {
            NuTimeBarSlotReset(set, slot);
        }
        if (NuTimeBar_PeakReset != 0) {
            timebar->field_14[slot] = 0;
        }
    }
    NuTimeBar_PeakReset = 0;
    --NuTimeBarHorizontalPeakReset;
    if (NuTimeBarHorizontalPeakReset < 0) {
        NuTimeBarHorizontalPeakReset = NuTimeBarPeakResetInterval;
    }
}

void __cdecl NuTimeBarSetRender(i32 set) {
    TimeBarSet *timebar = (TimeBarSet *)NuTimeBar_SetList[set + 1];
    f32 gpu_frames = 0.0f;
    i32 engine_enabled = NuTimeBar_EngineEnabled;
    i32 slot;

    if (-1 == set) {
        if (NuTimeBar_GpuFrameOutEnabled != 0) {
            u32 gpu_time = (u32)timebar->accumulators[timebar->toggle_flags[1]][1];
            gpu_frames = (f32)(gpu_time * nuapi.fps / NuTime_MicroScale);
        }
        if (engine_enabled == 0 && gpu_frames <= 1.0f) {
            return;
        }
    }

    NuRndrBeginScene(1);
    if (set > -1 || (set == -1 && engine_enabled != 0)) {
        f32 vertical_scale = (f32)(NuTimeBarVerticalScale / PS2_VREZ_H);
        for (slot = 0; slot < timebar->slot_count; ++slot) {
            u32 value = (u32)timebar->accumulators[timebar->toggle_flags[slot]][slot];
            if (value > (u32)timebar->field_14[slot]) {
                timebar->field_14[slot] = (i32)value;
            }
            if (NuTimeBarRenderPeakReset == 0) {
                timebar->field_18[slot] = 0;
            }
            value = (u32)timebar->accumulators[timebar->toggle_flags[slot]][slot];
            if (value > (u32)timebar->field_18[slot]) {
                timebar->field_18[slot] = (i32)value;
            }

            if (timebar->field_18[slot] != 0) {
                f32 left = (f32)((slot + 1) * NuTimeBarSlotWidth * NuTimeBarColumnStep);
                f32 elapsed_frames = (f32)((f64)(u32)timebar->field_18[slot] * NuTime_RefreshRate / NuTime_MicroScale);
                f32 bottom = (f32)(NuTimeBarTop + elapsed_frames * vertical_scale);
                f32 right = (f32)(left + NuTimeBarSlotWidth);
                f32 screen_width = (f32)(PS2_VREZ_W << 4);
                f32 screen_height = (f32)(PS2_VREZ_H << 4);
                i32 x = (i32)(left * screen_width);
                i32 y = (i32)(NuTimeBarTop * screen_height);
                i32 width = (i32)(screen_width * ((right - left)));
                i32 height = (i32)((bottom - NuTimeBarTop) * screen_height);
                i32 text_x;
                i32 text_y;

                NuRndrRect2di(x, y, width, height, timebar->colours[slot], NULL);
                {
                    if (NuQFntModeStackIndex < 16) {
                        NuQFntModeStack[NuQFntModeStackIndex++] = NuQFntMode;
                    }
                    NuQFntMode = 2;
                    if (NuQFntCSModeStackIndex < 16) {
                        NuQFntCSModeStack[NuQFntCSModeStackIndex++] = nuqfnt_cs_mode;
                    }
                    nuqfnt_cs_mode = 1;
                    qfnt_rezscale_w = NuTimeBarQFontScale;
                    qfnt_rezscale_h = NuTimeBarQFontScale;
                    qfnt_offscale_x = 0.0f;
                    qfnt_offscale_y = 0.0f;
                    qfnt_len_scale = NuTimeBarQFontSize;
                    qfnt_height_scale = NuTimeBarQFontSize;
                    if (NULL != system_qfont) {
                        system_qfont->mode = 1;
                        nuqfnt_space_width = system_qfont->space_width;
                    }
                }
                if (system_qfont != NULL) {
                    *system_qfont->x_scale = 1.0f;
                    *system_qfont->y_scale = (NuQFntMode & 4) != 0 ? 1.0f : 0.5f;
                    system_qfont->platform_data->colour = 0xe0e0e0e0;
                }
                text_x = (i32)(PS2_VREZ_W * NuTimeBarSlotWidth * NuQFntHalfScale) -
                         ((slot + 1)) * ((i32)(PS2_VREZ_W * NuTimeBarNegativeSlotWidth)) * 3;
                if (0 != timebar->field_18[slot]) {
                    f32 lines = (f32)((f64)(u32)timebar->field_18[slot] * NuTimeBarLinesPerSecond / NuTime_MicroScale);
                    i32 digits = (i32)lines;
                    i32 digit;
                    if (digits >= 10000) {
                        digits = 9999;
                    }
                    text_y = (i32)(PS2_VREZ_H * NuTimeBarTop) * 16;
                    text_y = text_y + ((i32)NuQFntHeight(system_qfont) * (digits > 999 ? 5 : 4));
                    for (digit = 0; digit < 3; ++digit) {
                        text_y -= (i32)NuQFntHeight(system_qfont);
                        NuQFntPrintEx(system_qfont, text_x * 16, text_y, 0x40, NuTimeBarValueFormat, digits % 10);
                        digits /= 10;
                    }
                    if (digits != 0) {
                        text_y -= (i32)NuQFntHeight(system_qfont);
                        NuQFntPrintEx(system_qfont, text_x * 16, text_y, 0x40, NuTimeBarValueFormat, digits % 10);
                    }
                }
                if (timebar->field_14[slot] != 0) {
                    f32 lines;
                    text_x = (i32)(NuQFntHalfScale * (PS2_VREZ_W * NuTimeBarSlotWidth)) -
                             (i32)(PS2_VREZ_W * NuTimeBarNegativeSlotWidth) * (slot + 1) * 3;
                    text_y = (PS2_VREZ_H << 4) - (i32)NuQFntHeight(system_qfont) * 2;
                    text_y += (i32)NuQFntBaseline(system_qfont);
                    if ((slot & 1) != 0) {
                        text_y -= 320;
                    }
                    lines = (f32)((f64)(u32)timebar->field_14[slot] * NuTimeBarLinesPerSecond / NuTime_MicroScale);
                    NuQFntPrintEx(system_qfont, text_x * 16, text_y, 0x40, NuTimeBarValueFormat, (i32)lines);
                }
                if (NULL != timebar->slot_names[slot]) {
                    text_x = (i32)(PS2_VREZ_W * NuTimeBarSlotWidth * NuQFntHalfScale) -
                             (i32)(PS2_VREZ_W * NuTimeBarNegativeSlotWidth) * (slot + 1) * 3;
                    text_y = (PS2_VREZ_H << 4) - (i32)NuQFntHeight(system_qfont);
                    text_y += (i32)NuQFntBaseline(system_qfont);
                    text_y -= (slot & 1) != 0 ? 640 : 320;
                    NuQFntPrintEx(system_qfont, text_x * 16, text_y, 0x40, timebar->slot_names[slot]);
                }
                {
                    if (NuQFntModeStackIndex > 0) {
                        NuQFntMode = NuQFntModeStack[--NuQFntModeStackIndex];
                    }
                    if (NuQFntCSModeStackIndex > 0) {
                        NuQFntSetCoordinateSystem(NuQFntCSModeStack[--NuQFntCSModeStackIndex]);
                    }
                }
            }
            if (timebar->field_28 != 0) {
                NuTimeBarSlotReset(set, slot);
            }
            if (NuTimeBar_PeakReset != 0) {
                timebar->field_14[slot] = 0;
            }
        }
        NuTimeBar_PeakReset = 0;
        --NuTimeBarRenderPeakReset;
        if (NuTimeBarRenderPeakReset < 0) {
            NuTimeBarRenderPeakReset = NuTimeBarPeakResetInterval;
        }
    }
    if (gpu_frames > 1.0f) {
        NuRndrRect2di(0, 0, 0x3c0, 0xe00, (i32)0x80000064, NuTimeBar_FrameOutMtl);
        NuRndrRect2di(0x2440, 0, 0x3c0, 0xe00, (i32)0x80000064, NuTimeBar_FrameOutMtl);
    }
    NuRndrEndScene();
}

f32 NuTimeGetFrameTime(void) {
    return nuapi.frame_time;
}

void __cdecl PcInitializeStartupCriticalSection(void) {
    if (PcStartupCriticalSectionInitialized == 0) {
        InitializeCriticalSection(&PcStartupCriticalSection);
        PcStartupCriticalSectionInitialized = 1;
    }
}

void __cdecl PcInitializePerformanceTiming(void) {
    DWORD_PTR process_mask = 1;
    DWORD_PTR system_mask = 1;
    if (!GetProcessAffinityMask(GetCurrentProcess(), &process_mask, &system_mask))
        process_mask = 1;
    SetProcessAffinityMask(GetCurrentProcess(), 1);
    QueryPerformanceCounter((LARGE_INTEGER *)&NuTime_TickBase);
    QueryPerformanceFrequency((LARGE_INTEGER *)&NuTime_PerformanceFrequency);
    SetProcessAffinityMask(GetCurrentProcess(), process_mask);
}
