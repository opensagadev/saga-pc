#include "saga.h"

void NuWaterReset(void) {
    nu_water_reset_state = 1;
    nu_water_reset_status = 0;
}

void NuMouseRead(void) {
}

void __cdecl NuRegisterEndFrameCallBackFn(void (*callback)(void)) {
    nuapi_endframe_callbackfn = callback;
}

static f32 nu2api_paused;

void __cdecl NuPause(i32 paused) {
    nu2api_paused = (f32)paused;
}
