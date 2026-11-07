#include "saga.h"

/* nuframe's frame-loop members outside the nu3d units. SetProceduralAnimationFn and NuPause are in
 * nu3d_includes.cpp, the water vertex buffer in nu3d_platform_pc.cpp. */

void __cdecl PCFrameSetLoadingYieldEnabled(i32 enabled) {
    PCFrameLoadingYieldEnabledPC = enabled != 0;
}
