#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    void fun_006df670(f32, f32);
    void NuKeyboardCreateWindowState(HWND window, HINSTANCE instance);
    extern i32 PCFrameCriticalSectionIndex;
    i32 PCFrameLoadingYieldEnabled(void);
    extern u8 PCFrameLoadingYieldEnabledPC;
    void PCDeviceResetState(void);

#ifdef __cplusplus
    void PcFreeApiCallBuffer(void);
    void PcRendererCleanup(void);
    void PcRendererShutdown(i32);
    void PcDestroyPlatform(void);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern void *PcApiCallBuffer;
    extern i32 PcApiCallBufferCount;
    i32 PcApiGetCallBufferCount(void);
    void *PcApiGetCallBuffer(void);
    void PCFrameSetLoadingYieldEnabled(i32 enabled);

    extern i32 PCFramePresentationState;
    i32 PCFramePresentNative(i32, i32);
    void PCDeviceResetSamplerDefaultsPc(void);
    f32 PcInputAspectRatioPc(void);
#ifdef __cplusplus
    void PCDevicePostEffectBarrierPc(void);
    extern u32 PcGetRefreshRate(void);
    i32 PCFrameBeginScene(i32 begin);
    void PCDeviceSetWorldIdentityPc(void);
    void PCFrameConfigureTargets(void);
    void PCFrameResetTargets(void);
    void PCGraphicsRecreateResources(i32 reset_available);
#ifdef __cplusplus
}
#endif
#endif
