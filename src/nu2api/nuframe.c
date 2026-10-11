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

f32 __cdecl NuFrameEnd(void) {
    NuTime now;
    NuTime elapsed;
    i32 had_error = NuHasError();
    STUBBED_NAMED(0x008004e8); /* PC frame pacing gate and target interval. */
    STUBBED_NAMED(0x007510ac); /* PC Sleep calls inside the frame wait loop. */
    STUBBED_NAMED(0x00601330); /* PC frame-time update helper; identity unconfirmed. */
    STUBBED_NAMED(0x005d34a0); /* PC frame-time update helper; identity unconfirmed. */
    NuWindAnimate(nuapi.wind, nuapi.frame_time);
    STUBBED_NAMED(0x005c6100); /* One-byte PC leaf; identity unresolved. */
    STUBBED_NAMED(0x005de1a0); /* PC update helper with unresolved owner. */
    PCFrameCriticalSectionEnter_candidate();
    /* DispatchMessage may update the native focus and policy flags. */
    if (SoundVoiceEnabledGate != 0) {
        NuWindowsProcessMessages();
        STUBBED_NAMED(0x006d3420);
    } else if (PcNoFocusPolicy != 0) {
        Sleep(1);
        NuWindowsProcessMessages();
    } else {
        STUBBED_NAMED(0x00698fa0);
        while (SoundVoiceEnabledGate == 0) {
            Sleep(10);
            if (NuWindowsProcessMessages() == 0) {
                break;
            }
        }
        STUBBED_NAMED(0x006d3420);
        NuTimeGetTicksPS(&nuapi.time.low, &nuapi.time.high);
        if ((i32)nuapi.time.high < 0) {
            NuFileError();
        }
        STUBBED_NAMED(0x00696b00);
    }
    STUBBED_NAMED(0x004f4c80); /* PC helper immediately before the render boundary. */
    NuTimeBarSetRender(-1);
    NuRndrSwapScreenEx(1, NuApiEndFrameCallbackPc);
    STUBBED_NAMED(0x005de1b0); /* PC post-render update helper. */
    PCFrameCriticalSectionLeave_candidate();

    NuTimeGetTicksPS(&now.low, &now.high);
    elapsed.low = now.low - nuapi.time.low;
    elapsed.high = now.high - nuapi.time.high - (now.low < nuapi.time.low);
    nuapi.frame_time = NuTimeSeconds(&elapsed);

    STUBBED_NAMED(0x029803e0); /* PC accumulated frame time. */
    STUBBED_NAMED(0x006df270); /* PC FPS counter reads. */
    STUBBED_NAMED(0x004ff0c0); /* PC timing finalizer. */
    NuPadUpdatePads();
    NuTimeGetTicksPS(&nuapi.time.low, &nuapi.time.high);
    NuTimeGetTicksPS(&nuapi.time2.low, &nuapi.time2.high);

    if (nuapi.forced_frame_time != 0.0f) {
        nuapi.frame_time = nuapi.forced_frame_time;
    }

    ++nuapi.frame_count;
    --nuapi.frame_begin_count;

    if (ShowingError == 0 && had_error != 0) {
        ShowingError = 1;
        STUBBED_NAMED("NuFrameEnd: PC error-dialog functions at 0x004f91f0 and 0x004feb60");
        NuClearError();
        ShowingError = 0;
    }

    nuapi.frame_end_state = 0;
    return nuapi.frame_time;
}

__declspec(noinline) void __cdecl NuWaterReset(void) {
    nu_water_reset_state = 1;
    nu_water_reset_status = 0;
}

void __cdecl NuRegisterEndFrameCallBackFn(void (*callback)(void)) {
    nuapi_endframe_callbackfn = callback;
}

static f32 nu2api_paused;

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
