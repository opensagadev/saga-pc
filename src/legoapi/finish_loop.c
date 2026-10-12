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

void FadeLoop(char *text, i32 direction, float duration, void (*draw)(float)) {
    f32 rate;
    f32 alpha = 0.0f;
    f32 target = 1.0f;
    if (direction != 0) {
        alpha = 1.0f;
        target = 0.0f;
    }
    rate = 10.0f;
    if (duration != 0.0f) {
        rate = 1.0f / duration;
    }
    FRAMETIME = DEFAULTFRAMETIME;
    while (alpha != target) {
        NuFrameBegin();
        alpha = SeekLinearF(alpha, target, rate * FRAMETIME);
        NuRndrBeginScene(-1);
        NuRndrClear(0xb00, 0, 1.0f);
        if (FadeLoop_ObjScene != NULL) {
            FadeLoop_DrawObj(alpha);
        }
        if (text != NULL) {
            SetQFont2D();
            Text3D(text, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0, (u8)(alpha * 0.0f), (u8)(alpha * 191.0f),
                   (u8)(alpha * 255.0f));
        }
        if (draw != NULL) {
            draw(alpha);
        }
        NuRndrEndScene();
        edGraEnableTerrainSwap();
        FRAMETIME = NuFrameEnd();
        edGraDisableTerrainSwap();
    }
    if (direction == 1) {
        FadeLoop_SetObj(NULL, NULL);
        FinishLoop(2);
    }
}

void SetLevelExBlowupFlags(u32 flags) {
    EXBLOWUPFLAGS = flags;
}
