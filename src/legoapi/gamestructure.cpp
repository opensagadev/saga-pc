#include "saga.h"

void ClearPause(void) {
    Paused = 0;
    NetPaused = 0;
}

extern "C" void __cdecl NuGCutSceneUpdateMtx(NUMTX *matrix) {
    STUBBED_NAMED("NuGCutSceneUpdateMtx: matrix-dependent recalculation"
                  " (retail passes the operand in EDI)");
}

void __cdecl NuDisplayListResetStateCache0(void *cursor) {
    STUBBED_NAMED("NuDisplayListResetStateCache0: 0x4ffaa0 per-frame reset");
}

void __cdecl NuDisplayListRender2D(i32 arg) {
    STUBBED_NAMED("NuDisplayListRender2D: 0x5ddd10 two-dimensional pass");
}

void __cdecl NuDisplayListBeginFrameTail(void) {
    STUBBED_NAMED("NuDisplayListBeginFrameTail: 0x5fd160 tail");
}
