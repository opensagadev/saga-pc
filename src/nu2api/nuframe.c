#include "saga.h"

extern void __cdecl NuKeyboardRead(void);
extern void __cdecl PCFrameCriticalSectionEnter_candidate(void);
extern void __cdecl PCFrameCriticalSectionLeave_candidate(void);

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

#define NUFRAME_ELAPSED(elapsed, now, start)                                                                           \
    if ((now).low >= (start).low) {                                                                                    \
        (elapsed).low = (now).low - (start).low;                                                                       \
        (elapsed).high = (now).high - (start).high;                                                                    \
    } else {                                                                                                           \
        (elapsed).high = (now).high - (start).high;                                                                    \
        (elapsed).low = (now).low - (start).low;                                                                       \
        (elapsed).high -= 1;                                                                                           \
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

void __cdecl PCFrameSetLoadingYieldEnabled(i32 enabled) {
    PCFrameLoadingYieldEnabledPC = enabled != 0;
}
