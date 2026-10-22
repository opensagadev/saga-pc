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
        STUBBED_NAMED(0x004f91f0); /* PC error dialog */
        NuClearError();
        ShowingError = 0;
    }

    nuapi.frame_end_state = 0;
    return nuapi.frame_time;
}

void __cdecl NuWaterSpeed(f32 speed) {
    NuShaderObjectBaseSetWaterSpeedPc(speed);
}

__declspec(noinline) void __cdecl NuWaterReset(void) {
    nu_water_reset_state = 1;
    nu_water_reset_status = 0;
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
