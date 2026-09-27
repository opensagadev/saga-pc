#include "saga.h"

extern void __cdecl NuKeyboardRead(void);

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
    union {
        f32 value;
        u32 bits;
    } forced_frame_time;
    i32 had_error = NuHasError();
    STUBBED_NAMED("NuFrameEnd: PC max-FPS gate, Sleep waits, and message-pump loop");
    STUBBED_NAMED("NuFrameEnd: PC material/texture animation, wind, occlusion, and pad-record updates");
    STUBBED_NAMED("NuFrameEnd: PC time-bar and pad rendering setup");

    NuTimeGet(&nuapi.time);
    STUBBED_NAMED("NuFrameEnd: PC NuRndrSwapScreen(-1); no callback call is visible in this body");
    STUBBED_NAMED("NuFrameEnd: PC post-render pad update and critical-section handling");

    NuTimeGet(&now);
    NuTimeSub(&elapsed, &now, &nuapi.time);
    nuapi.frame_time = NuTimeSeconds(&elapsed);

    STUBBED_NAMED("NuFrameEnd: PC frame-time accumulator at 0x029803e0 and FPS counter loop");
    STUBBED_NAMED("NuFrameEnd: PC FPS pacing Sleep loop and time-finalization helper");
    NuTimeGet(&nuapi.time);
    NuTimeGet(&nuapi.time2);

    forced_frame_time.value = nuapi.forced_frame_time;
    if ((forced_frame_time.bits & 0x7fffffffu) != 0) {
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

__declspec(noinline) i32 __cdecl NuHasError(void) {
    return bHaveErr;
}

__declspec(noinline) void __cdecl NuClearError(void) {
    bHaveErr = 0;
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
