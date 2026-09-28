#include "saga.h"

void ClearPause(void) {
    Paused = 0;
    NetPaused = 0;
}

extern "C" void __cdecl NuGCutSceneUpdateMtx(NUMTX *matrix) {
    STUBBED_NAMED("NuGCutSceneUpdateMtx: matrix-dependent recalculation"
                  " (retail passes the operand in EDI)");
}

void __cdecl NuDisplayListRender2D(i32 arg) {
    STUBBED_NAMED("NuDisplayListRender2D: 0x5ddd10 two-dimensional pass");
}
