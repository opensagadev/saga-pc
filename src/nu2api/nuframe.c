#include "saga.h"

extern void __cdecl NuKeyboardRead(void);
extern void __cdecl PCFrameCriticalSectionEnter_candidate(void);
extern void __cdecl PCFrameCriticalSectionLeave_candidate(void);

void __cdecl NuFrameBegin(void) {
    ++nuapi.frame_begin_count;
    NuPortalInit();
    NuWaterReset();
    NuMouseRead();
    NuKeyboardRead();
    NuOcclusionManagerBeginFrame();
}

extern i32 FrameSleepMillisecondsPc;
extern f32 FrameStartupDelayPc;
extern i32 NuApiTextLinePc;
extern f32 NuFrameAccumulatedTimePc;
extern const char *__cdecl PcFrameDelayMessagePc(void);
extern u32 __cdecl PcGetRefreshRatePc(void);
extern void __cdecl bgprocFrameSleepPc(i32 milliseconds);
extern void __cdecl NuFrameEndLeafPc(void);
extern void __cdecl FmvPlatformUpdatePc(void);

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void __cdecl NuApiPrintLinePc(u32 colour, const char *format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    NuQFntPushPrintMode(2);
    NuQFntPushCoordinateSystem(1);
    NuQFntSet(system_qfont);
    NuQFntSetColour(system_qfont, colour);
    NuQFntSetScale(system_qfont, 0.7f, 0.7f);
    NuQFntPrintEx(system_qfont, 0x140, NuApiTextLinePc * 8, 0x10, buffer);
    NuQFntPopPrintMode();
    NuQFntPopCoordinateSystem();
    NuApiTextLinePc += 20;
}

#define NUFRAME_ELAPSED(elapsed, now, start)                                                                           \
    if ((now).low >= (start).low) {                                                                                    \
        (elapsed).low = (now).low - (start).low;                                                                       \
        (elapsed).high = (now).high - (start).high;                                                                    \
    } else {                                                                                                           \
        (elapsed).high = (now).high - (start).high;                                                                    \
        (elapsed).low = (now).low - (start).low;                                                                       \
        (elapsed).high -= 1;                                                                                           \
    }

/* Shows the pending error messages for up to five seconds, or until the jump
 * button is pressed after the first second. */
void __cdecl NuErrorSleep(void) {
    f32 milliseconds = 0.0f;
    i32 waiting = 1;
    i32 index;
    f32 y;
    char *message;
    FRAME_PAD_PC pad;
    NuTime start, now, elapsed, rate;
    memset(&pad, 0, sizeof(pad));
    NuTimeGetTicksPS(&start.low, &start.high);
    if ((i32)start.high < 0) {
        NuFileError();
    }
    while (milliseconds < 5000.0f && waiting != 0) {
        y = 500.0f;
        index = 0;
        NuTimeGetTicksPS(&now.low, &now.high);
        if ((i32)now.high < 0) {
            NuFileError();
        }
        NUFRAME_ELAPSED(elapsed, now, start);
        NuTimeGetTicksPerSecondPS(&rate.low, &rate.high);
        milliseconds =
            (f32)((elapsed.high * NuTime_LowScale + elapsed.low) / (rate.high * NuTime_LowScale + rate.low) * 1000.0);
        if (nuapi.frame_begin_count != 0) {
            NuFileError();
        }
        if (++nuapi.frame_begin_count < 1) {
            NuFileError();
        }
        NuPortalInit();
        NuWaterReset();
        NuMouseRead();
        NuKeyboardRead();
        NuOcclusionManagerBeginFrame();
        NuRndrBeginScene(0);
        NuRndrClear(0xb00, 0xff00ffff, 1.0f);
        NuQFntSet(system_qfont);
        NuQFntPushPrintMode(2);
        NuQFntPushCoordinateSystem(1);
        NuQFntSet(system_qfont);
        while (*(message = NuGetErrN(index++)) != '\0') {
            NuQFntSetColour(system_qfont, 0xff000000);
            NuQFntMove(system_qfont, 500.0f, y, 0.0f);
            NuQFntPrintV(system_qfont, message, NULL);
            y += 150.0f;
        }
        NuQFntPopCoordinateSystem();
        NuQFntPopPrintMode();
        NuRndrEndScene();
        NuPadRead(&pad);
        if ((pad.digital_buttons & 0x40) != 0 && milliseconds > 1000.0f)
            waiting = 0;
        NuFrameEnd();
    }
}

f32 __cdecl NuFrameEnd(void) {
    NuTime elapsed;
    NuTime now;
    f32 refresh_rate;
    f32 target_interval;
    f32 capped_rate;
    f32 capped_interval;
    i32 had_error = NuHasError();

    if (FrameStartupDelayPc > 0.0f) {
        FrameStartupDelayPc -= nuapi.frame_time;
        NuApiTextLinePc = 20;
        NuRndrBeginScene(1);
        if (PcFrameDelayMessagePc() != NULL) {
            NuApiPrintLinePc(0xffffffffu, PcFrameDelayMessagePc());
        }
        NuRndrEndScene();
    }
    NuMtlAnimate(nuapi.frame_time);
    NuTexAnimProcess(nuapi.frame_time);
    NuWindAnimate(nuapi.wind, nuapi.frame_time);
    NuFrameEndLeafPc();
    PCDisplayListCriticalSectionEnterPc();
    PCFrameCriticalSectionEnter_candidate();
    /* DispatchMessage may update the native focus and policy flags. */
    if (SoundVoiceEnabledGate == 0) {
        if (PcNoFocusPolicy != 0) {
            Sleep(1);
            NuWindowsProcessMessages();
        } else {
            NuSound3SuspendForFocusPc();
            while (SoundVoiceEnabledGate == 0) {
                Sleep(10);
                if (NuWindowsProcessMessages() == 0) {
                    break;
                }
            }
            FmvPlatformUpdatePc();
            NuTimeGetTicksPS(&nuapi.time.low, &nuapi.time.high);
            if ((i32)nuapi.time.high < 0) {
                NuFileError();
            }
            NuSound3ResumeAfterFocusPc();
        }
    } else {
        NuWindowsProcessMessages();
        FmvPlatformUpdatePc();
    }
    NuPadRecordEndFrame();
    NuTimeBarSetRender(-1);
    NuRndrSwapScreenEx(1, NuApiEndFrameCallbackPc);
    PCDisplayListCriticalSectionLeavePc();
    PCFrameCriticalSectionLeave_candidate();

    NuTimeGetTicksPS(&now.low, &now.high);
    if ((i32)now.high < 0) {
        NuFileError();
    }
    NUFRAME_ELAPSED(elapsed, now, nuapi.time);
    nuapi.frame_time = NuTimeSeconds(&elapsed);
    NuFrameAccumulatedTimePc += nuapi.frame_time;

    refresh_rate = 85.0 > (f32)PcGetRefreshRatePc() ? 85.0f : (f32)PcGetRefreshRatePc();
    target_interval = 1.0f / refresh_rate;
    capped_rate = 85.0 < (f32)PcGetRefreshRatePc() ? 85.0f : (f32)PcGetRefreshRatePc();
    capped_interval = 1.0f / capped_rate;
    FrameSleepMillisecondsPc = 20;
    if (!(nuapi.frame_time / capped_interval < 1.1f)) {
        FrameSleepMillisecondsPc = 1;
    }
    if (target_interval > nuapi.frame_time) {
        Sleep((DWORD)((target_interval - nuapi.frame_time) * 1000.0));
        NuTimeGetTicksPS(&now.low, &now.high);
        if ((i32)now.high < 0) {
            NuFileError();
        }
        NUFRAME_ELAPSED(elapsed, now, nuapi.time);
        nuapi.frame_time = NuTimeSeconds(&elapsed);
    }
    NuTimeGetTicksPS(&nuapi.time.low, &nuapi.time.high);
    if ((i32)nuapi.time.high < 0) {
        NuFileError();
    }
    NuTimeGetTicksPS(&nuapi.time2.low, &nuapi.time2.high);
    if ((i32)nuapi.time2.high < 0) {
        NuFileError();
    }
    bgprocFrameSleepPc(FrameSleepMillisecondsPc);
    NuPadUpdatePads();
    ++nuapi.frame_count;
    if (nuapi.forced_frame_time != 0.0f) {
        nuapi.frame_time = nuapi.forced_frame_time;
    }
    --nuapi.frame_begin_count;

    if (ShowingError == 0 && had_error != 0) {
        ShowingError = 1;
        NuErrorSleep();
        NuClearError();
        ShowingError = 0;
    }

    nuapi.frame_end_state = 0;
    return nuapi.frame_time;
}

void __cdecl NuWaterSpeed(f32 speed) {
    NuShaderObjectBaseSetWaterSpeedPc(speed);
}

void __cdecl NuWaterClearPc(void) {
    nu_water_flow_state = 0;
    nu_water_reset_status = 0;
}

__declspec(noinline) void __cdecl NuWaterReset(void) {
    nu_water_reset_state = 1;
    nu_water_reset_status = 0;
}

/* Dynamic vertex buffer the PC water renderer creates on first use. */
PC_BUFFER_RESOURCE_PC *NuWaterVertexBufferPc = 0;

/* Releases the water vertex buffer before the D3D device is reset or destroyed. */
void __cdecl NuWaterReleaseVertexBufferPc(void) {
    if (NuWaterVertexBufferPc != NULL) {
        NuWaterVertexBufferPc->methods->release(NuWaterVertexBufferPc);
        NuWaterVertexBufferPc = NULL;
    }
}

void __cdecl SetProceduralAnimationFn(NUJOINTPROCANIMFN_PC callback) {
    JointProcAnimFn = callback;
}

static f32 nu2api_paused = 0;

void __cdecl NuPause(i32 paused) {
    nu2api_paused = (f32)paused;
}

extern void(__cdecl *NuApiEndFrameCallbackPc)(void);
void __cdecl NuApiSetEndFrameCallbackPc(void(__cdecl *callback)(void)) {
    NuApiEndFrameCallbackPc = callback;
}

void __cdecl PCFrameSetLoadingYieldEnabled(i32 enabled) {
    PCFrameLoadingYieldEnabledPC = enabled != 0;
}
