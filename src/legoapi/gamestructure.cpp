#include "saga.h"

void ClearPause(void) {
    Paused = 0;
    NetPaused = 0;
}

extern "C" void __cdecl NuGCutSceneUpdateMtx(NUMTX *matrix) {
    STUBBED_NAMED("NuGCutSceneUpdateMtx: matrix-dependent recalculation"
                  " (retail passes the operand in EDI)");
}

extern "C" i32 __cdecl InStory(void) {
    if (FreePlay != 0 || ChallengeMode != 0 || Mission_Active(NULL) != NULL || Arcade != 0) {
        return 0;
    }
    return 1;
}
