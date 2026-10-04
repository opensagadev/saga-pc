#include "saga.h"

extern f32 CutSceneCameraFocalLengthPc;
f32 __cdecl NuCameraFocalLenToFOV(f32 focal_length);
void __cdecl WidescreenCode(i32 widescreen) {
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
extern i32 RenderBackgroundColoursPc[2];
void __cdecl NuCameraSet(NUCAMERA *);
void __cdecl BackDrop_Draw(f32, i32);
void __cdecl NoRender(void) {
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

extern u8 ScreenGrabNeededPc;
extern i32 ScreenPauseRenderTargetPc;
extern f32 ScreenPauseFadePc;
void __cdecl NuBackbufferCopy(i32 texture_id);
void __cdecl GrabStillScreen(void) {
    if (ScreenGrabNeededPc != 0) {
        ScreenGrabNeededPc = 0;
        NuRndrBeginScene(1);
        NuBackbufferCopy(ScreenPauseRenderTargetPc);
        NuRndrEndScene();
        ScreenPauseFadePc = 0.0f;
    }
}
