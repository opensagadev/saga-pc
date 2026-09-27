#include "saga.h"

void __cdecl FinishLoop(i32 frames) {
    if (FinishLoop_On == 0) {
        return;
    }

    if (finishloop_backdroponly != 0) {
        STUBBED_NAMED("FinishLoop: copy identity into pNuCam->mtx and call NuCameraSet");
        do {
            NuFrameBegin();
            STUBBED_NAMED(0x005d4290); /* NuRndrBeginScene(-1) */
            STUBBED_NAMED(0x005ebd80); /* NuRndrClear(0xb00, 0, 1.0f) */
            STUBBED_NAMED(0x00564af0); /* BackDrop_Draw(1.0f, 1) */
            STUBBED_NAMED(0x005f3950); /* NuRndrEndScene() */
            edGraEnableTerrainSwap();
            STUBBED_NAMED(0x004fd050); /* NuFrameEnd() */
            edGraDisableTerrainSwap();
        } while (--frames > 0);
        finishloop_backdroponly = 0;
        return;
    }

    STUBBED_NAMED("FinishLoop: test FadeSys.pending_type; the other branch calls FadeSys::Draw");
    do {
        NuFrameBegin();
        STUBBED_NAMED(0x005d4290); /* NuRndrBeginScene(-1) */
        STUBBED_NAMED(0x005ebd80); /* NuRndrClear(0xb00, 0, 1.0f) */
        STUBBED_NAMED(0x005f3950); /* NuRndrEndScene() */
        edGraEnableTerrainSwap();
        STUBBED_NAMED(0x004fd050); /* NuFrameEnd() */
        edGraDisableTerrainSwap();
    } while (--frames > 0);
}
