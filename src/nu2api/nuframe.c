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
    i32 had_error = NuHasError();
    STUBBED_NAMED("NuFrameEnd PC timing calls 0x004ffb20 and 0x004f3c30; NUAPI timestamp fields remain unresolved");
    STUBBED_NAMED("NuFrameEnd PC direct calls 0x005d4290, 0x006de050, 0x004f3f50, 0x005f3950, 0x00601330");
    STUBBED_NAMED("NuFrameEnd PC direct calls 0x005d34a0, 0x005ddc80, 0x005c6100, 0x005de1a0, 0x006df300");
    STUBBED_NAMED("NuFrameEnd PC direct calls 0x006dd390, 0x00698fa0, 0x006d3420, 0x004ef790, 0x004f4c80");
    STUBBED_NAMED("NuFrameEnd PC direct calls 0x005f3d60, 0x0060b5b0, 0x006df310, 0x006df270, 0x004ff0c0");
    STUBBED_NAMED("NuFrameEnd PC direct calls 0x004fb130, 0x004f91f0, 0x004feb60; renderer, callbacks, pad and worker "
                  "closure unresolved");

    ++nuapi.frame_count;
    --nuapi.frame_begin_count;

    if (ShowingError == 0 && had_error != 0) {
        ShowingError = 1;
        STUBBED_NAMED("NuFrameEnd PC error dialog 0x004f91f0");
        NuClearError();
        ShowingError = 0;
    }

    nuapi.frame_end_state = 0;
    return nuapi.frame_time;
}

i32 __cdecl NuHasError(void) {
    return bHaveErr;
}

void __cdecl NuClearError(void) {
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
