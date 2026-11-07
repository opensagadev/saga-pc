#include "saga.h"

extern "C" {
    void __cdecl PCDeviceBeginScenePc(void);
    void __cdecl PCGraphicsReleaseFramebuffers(void);
    void __cdecl PCGraphicsRestoreFramebuffers(void);
    void __cdecl NuRndrGeometryBufferReleasePc(void);
    void __cdecl NuRndrGeometryBufferRestorePc(void);
    void __cdecl NuDebrisReleaseVertexBuffersPc(void);
    void __cdecl NuDebrisCreateVertexBuffersPc(void);
    void __cdecl NuRndrDwaReleasePc(void);
    void __cdecl PcGeometryOriginReleasePc(void);
    void __cdecl NuPrimReleaseResourcesPc(void);
    void __cdecl NuKeyboardWindowStateLeafPc(void);
    i32 __cdecl GameReleaseResourcesPc(void);
    i32 __cdecl GameRestoreResourcesPc(void);
    void __cdecl DebrisReleaseResourcesPc(void);
    void __cdecl DebrisRestoreResourcesPc(void);
    void __cdecl NuWaterReleaseVertexBufferPc(void);
    void __cdecl NuGScnReleaseDynamicBuffersPc(void);
    void __cdecl NuGScnRestoreDynamicBuffersPc(void);
    void __cdecl NuTexReleaseResourcesPc(void);
    void __cdecl NuTexRestoreResourcesPc(void);
    void __cdecl FmvReleasePc(void);
    void __cdecl FmvRestorePc(void);
    void __cdecl NuFmvTextureReleasePc(void);
    void __cdecl NuFmvTextureRestorePc(void);
    void __cdecl ResizeStillRenderPc(i32, i32);
    void __cdecl PCGraphicsClearFrameTexturesPc(void);
}

extern "C" void __cdecl PcFreeApiCallBuffer(void) {
    if (PcApiCallBuffer != NULL)
        NuFree(PcApiCallBuffer, ".\\d3dApiCalls.cpp", 0x451);
    PcApiCallBufferCount = 0;
    PcApiCallBuffer = NULL;
}

extern "C" void __cdecl PcRendererCleanup(void) {
    NuRndrDwaReleasePc();
    PcGeometryOriginReleasePc();
    NuPrimReleaseResourcesPc();
    NuDebrisReleaseVertexBuffersPc();
}

extern "C" void __cdecl PcRendererShutdown(i32 unused) {
    if (PCDeviceIsAbsent() == 0) {
        PCDeviceSetTexture(0, NULL);
        PCDeviceSetTexture(1, NULL);
        PCDeviceSetTexture(2, NULL);
        PCDeviceSetTexture(3, NULL);
        PCDeviceSetStreamSourcePc(0, NULL, 0, 0);
        PCDeviceSetStreamSourcePc(1, NULL, 0, 0);
        PCDeviceSetStreamSourcePc(2, NULL, 0, 0);
        PCDeviceSetStreamSourcePc(3, NULL, 0, 0);
    }
    PCGraphicsReleaseFramebuffers();
    PCDeviceBackend.ReleaseDefaultPoolResources();
    PC_EFFECT_SURFACE_PC::ReleaseResources();
    PCGraphicsReleaseEffectTextures();
    GameReleaseResourcesPc();
    PcGeometryOriginReleasePc();
    NuPrimReleaseResourcesPc();
    NuRndrGeometryBufferReleasePc();
    DebrisReleaseResourcesPc();
    NuDebrisReleaseVertexBuffersPc();
    NuWaterReleaseVertexBufferPc();
    NuGScnReleaseDynamicBuffersPc();
    NuTexReleaseResourcesPc();
    FmvReleasePc();
    NuFmvTextureReleasePc();
}

extern "C" void __cdecl PCGraphicsRecreateResources(i32 reset_available) {
    PCDeviceBackend.RestoreDefaultPoolResources();
    if (reset_available != 0) {
        STUBBED_NAMED("void action5c3900 at6dfe33");
        ResizeStillRenderPc(PS2_REZ_W, PS2_REZ_H);
    }
    PCGraphicsRestoreEffectTextures(reset_available == 1);
    PC_EFFECT_SURFACE_PC::RestoreResources();
    PCGraphicsRestoreFramebuffers();
    NuKeyboardWindowStateLeafPc();
    NuRndrGeometryBufferRestorePc();
    NuDebrisCreateVertexBuffersPc();
    DebrisRestoreResourcesPc();
    GameRestoreResourcesPc();
    NuTexRestoreResourcesPc();
    NuGScnRestoreDynamicBuffersPc();
    PCDeviceResetState();
    FmvRestorePc();
    NuFmvTextureRestorePc();
    PCGraphicsClearFrameTexturesPc();
}

extern "C" void __cdecl PcDestroyPlatform(void) {
    PCNativeWarningDebugState.Shutdown();
    nu_keyboard_owner.Shutdown();
    PcRendererShutdown(0);
    PCDeviceBackend.Shutdown();
}

extern "C" void __cdecl PCFrameResetTargetsPc(void) {
    PCDeviceBackend.ResetTargets();
}

extern "C" void __cdecl PCFrameConfigureTargetsPc(void) {
    PCDeviceBackend.ConfigureCurrentTargets();
}

extern "C" i32 __cdecl PCFrameBeginScenePc(void) {
    PCDeviceBeginScenePc();
    return 0;
}
