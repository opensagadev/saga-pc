#include "saga.h"

class FadeSystem : public FADEINFO_PC {};

extern "C" f32 CutSceneCameraFocalLengthPc;
extern "C" f32 __cdecl NuCameraFocalLenToFOV(f32 focal_length);
extern "C" void __cdecl WidescreenCode(i32 widescreen) {
    f32 aspect = PcWindowOptions.aspect_mode == 2 ? 0.625f : 0.5625f;
    if (widescreen != 0) {
        pNuCam->aspect = aspect;
        if (!CutSceneCameraCTRL)
            pNuCam->fov = (f32)((aspect + 0.75) * 0.5);
        else if (CutSceneCameraFocalLengthPc == 0.0f)
            pNuCam->fov = aspect;
        else
            pNuCam->fov = (f32)(NuCameraFocalLenToFOV(CutSceneCameraFocalLengthPc) * (aspect / 0.75));
        SmartTextSetWidescreen(0.75f, 1.0f);
    } else {
        pNuCam->aspect = 0.75f;
        if (!CutSceneCameraCTRL || CutSceneCameraFocalLengthPc == 0.0f)
            pNuCam->fov = 0.75f;
        else
            pNuCam->fov = NuCameraFocalLenToFOV(CutSceneCameraFocalLengthPc);
        SmartTextSetWidescreen(1.0f, 1.0f);
    }
}
typedef struct CUT_BORDER_RECORD_PC {
    u8 unknown_00[0x50];
    u32 flags;
} CUT_BORDER_RECORD_PC;
STATIC_ASSERT(offsetof(CUT_BORDER_RECORD_PC, flags) == 0x50);
extern "C" NUMTL *FadeMtl;
extern "C" void __cdecl DrawCutBorders(i32 widescreen) {
    f32 border_scale;
    i32 height;
    if (!(CutBorderScale > 0.0f))
        return;
    NuRndrBeginScene(1);
    if ((CutStopInfo != NULL && (((CUT_BORDER_RECORD_PC *)CutStopInfo)->flags & 0x4000) != 0) ||
        (WORLD->cutscene_sys != NULL && WORLD->cutscene_sys->count == 1 &&
         (((CUT_BORDER_RECORD_PC *)WORLD->cutscene_sys->records[0])->flags & 0x4000) != 0))
        border_scale = 0.21276596188545227f;
    else
        border_scale = 0.1f;
    if (widescreen != 0)
        border_scale = (f32)((border_scale * 4.0 - 0.5) / 3.0);
    if (border_scale < 0.02f)
        border_scale = 0.02f;
    height = (i32)(NuTrigTable[((i32)(CutBorderScale * 16384.0) >> 1) & 0x7fff] * (border_scale * 3584.0));
    NuRndrRect2di(0, 0, 0x2800, height, 0x80ffffff, FadeMtl);
    NuRndrRect2di(0, 0xe20 - height, 0x2800, height, 0x80ffffff, FadeMtl);
    NuRndrEndScene();
}

extern "C" i32 RenderBackgroundColoursPc[2];
extern "C" void __cdecl NuCameraSet(NUCAMERA *);
extern "C" void __cdecl BackDrop_Draw(f32, i32);
extern "C" void __cdecl NoRender(void) {
    pNuCam->mtx = NuMtxSetIdentityTemplate;
    NuCameraSet(pNuCam);
    NuRndrBeginScene(1);
    if (RenderBackgroundColoursPc[0] == RenderBackgroundColoursPc[1])
        NuRndrClear(0xf00, RenderBackgroundColoursPc[0], 1.0f);
    else
        NuRndrGradClear(0xf00, RenderBackgroundColoursPc[0], RenderBackgroundColoursPc[1], 1.0f);
    if (MainRenderTime != 1.0f)
        BackDrop_Draw((f32)(1.0f - MainRenderTime), 0);
    NuRndrEndScene();
}

extern "C" u8 ScreenGrabNeededPc;
extern "C" i32 ScreenPauseRenderTargetPc;
extern "C" f32 ScreenPauseFadePc;
extern "C" void __cdecl NuBackbufferCopy(i32 texture_id);
extern "C" void __cdecl GrabStillScreen(void) {
    if (ScreenGrabNeededPc != 0) {
        ScreenGrabNeededPc = 0;
        NuRndrBeginScene(1);
        NuBackbufferCopy(ScreenPauseRenderTargetPc);
        NuRndrEndScene();
        ScreenPauseFadePc = 0.0f;
    }
}

extern "C" void NeedScreenGrab(i32 needed) {
    ScreenGrabNeededPc = needed != 0;
}

extern "C" u8 IsGrabbingScreen(void) {
    return ScreenGrabNeededPc;
}

extern "C" NUVPCURRENT *__cdecl NuVpGetCurrentViewport(void);
extern "C" void DrawStillScreen(i32 clear) {
    NuRndrBeginScene(-1);
    NuVpGetCurrentViewport();
    if (clear != 0) {
        NuRndrClear(0x500, 0, 1.0f);
    }
    if (MainRenderTime < 1.0f) {
        const u32 colour = (((i32)(MainRenderTime * 128.0f)) << 24) | 0x00808080u;
        u32 colours[4] = {colour, colour, colour, colour};
        NuRndrGradRectUV2di(0, 0, 0x2800, 0xe00, 0.0f, 1.0f, 1.0f, 0.0f, colours, (NUMTL *)pause_rndr_mtl);
    } else {
        NuRndrRectUV2di(0, 0, 0x2800, 0xe00, 0.0f, 1.0f, 1.0f, 0.0f, 0x80808080u, (NUMTL *)pause_rndr_mtl);
    }
    NuRndrEndScene();
}

extern "C" f32 ScreenPauseFadeSpeedPc;
extern "C" void __cdecl DrawPauseFade(void) {
    if (editor_active != 0 || screendump != 0)
        return;
    if (Paused == 0 && NetPaused == 0)
        ScreenPauseFadePc = MAX(ScreenPauseFadePc - FRAMETIME * ScreenPauseFadeSpeedPc, 0.0f);
    else
        ScreenPauseFadePc = MIN(FRAMETIME * ScreenPauseFadeSpeedPc + ScreenPauseFadePc, 1.0f);
}

extern "C" void __cdecl HandleStillRender(void) {
    GetMenuID();
    if (pause_rndr_on != 0 && FadeSys.pending_type == -1) {
        NuRndrBeginScene(1);
        if (MainRenderTime < 1.0f) {
            if (back_rgba[0] == back_rgba[1])
                NuRndrClear(0xf00, back_rgba[0], 1.0f);
            else
                NuRndrGradClear(0xf00, back_rgba[0], back_rgba[1], 1.0f);
            BackDrop_Draw(1.0f - MainRenderTime, 0);
        } else {
            NuRndrClear(0x300, 0, 1.0f);
        }
        NuRndrEndScene();
    }
    if (LevelCompletionFallbackFlagPC != 2 && pause_rndr_on != 0 && pause_rt != 0 && FadeSys.pending_type == -1)
        DrawStillScreen(clear_screen_onstill);
    if (Paused != 0) {
        if (old_pause_state == 0 && FadeSys.pending_type == -1)
            ScreenGrabNeededPc = 1;
    } else if (old_pause_state != 0) {
        pause_rndr_on = 0;
    }
    old_pause_state = Paused;
    if (waiting_for_level != -1 &&
        (gone_through_door_to_new_level != 0 || cut_waiting_for_new_level != 0 || waiting_for_new_level != 0))
        pause_rndr_on = 1;
    else if (NewLData == CREDITS_LDATA && LastLData == STATUS_LDATA)
        pause_rndr_on = 1;
    else if (NewLData == STATUS_LDATA && WORLD->area_index != -1 && (WORLD->area[WORLD->area_index].flags & 2) != 0)
        pause_rndr_on = 1;
    else if ((NewLData == STATUS_LDATA && GAMEDEMO != 0) || NewLData == CREDITS_LDATA)
        pause_rndr_on = 1;
    else if (NewLData == HUB_LDATA && WORLD->area_index != -1 && (WORLD->area[WORLD->area_index].flags & 2) != 0)
        pause_rndr_on = 1;
    else if (MainRenderTime > 0.0f && MainRenderTime < 1.0f)
        pause_rndr_on = 1;
    else if (Paused != 0 && ScreenGrabNeededPc == 0) {
        if (PauseRenderOffFn != NULL && PauseRenderOffFn() != 0)
            pause_rndr_on = 0;
        else
            pause_rndr_on = 1;
    } else if (LevelCompletionFallbackFlagPC == 0 || LevelCompletionFallbackFlagPC == 1)
        pause_rndr_on = 0;
    else
        pause_rndr_on = 1;
    if (screendump != 0)
        pause_rndr_on = 0;
    LevelCompletionFallbackFlagPC = 0;
    clear_screen_onstill = 1;
}
