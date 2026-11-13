#include "saga.h"

/* Retail unit 0x6df010-0x6dfea0: definitions in retail address order. */

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
extern "C" void __cdecl NuPostEffectResizePc(void);

/* The D3DX matrix type with its sixteen-float constructor (only the parts used here). */
struct D3DXMATRIX {
    FLOAT m11, m12, m13, m14;
    FLOAT m21, m22, m23, m24;
    FLOAT m31, m32, m33, m34;
    FLOAT m41, m42, m43, m44;

    D3DXMATRIX(FLOAT f11, FLOAT f12, FLOAT f13, FLOAT f14, FLOAT f21, FLOAT f22, FLOAT f23, FLOAT f24, FLOAT f31,
               FLOAT f32, FLOAT f33, FLOAT f34, FLOAT f41, FLOAT f42, FLOAT f43, FLOAT f44) {
        m11 = f11;
        m12 = f12;
        m13 = f13;
        m14 = f14;
        m21 = f21;
        m22 = f22;
        m23 = f23;
        m24 = f24;
        m31 = f31;
        m32 = f32;
        m33 = f33;
        m34 = f34;
        m41 = f41;
        m42 = f42;
        m43 = f43;
        m44 = f44;
    }
};

STATIC_ASSERT(sizeof(D3DXMATRIX) == sizeof(NUMTX));

extern "C" void __cdecl NuKeyboardCreateWindowState(HWND window, HINSTANCE instance) {
    PCFrameCriticalSectionIndex = PCThreadCriticalSectionCreate_candidate();
    PCDeviceBackend.InitializeAdapter();
    nu_keyboard_owner.Setup(window, instance);
    NuKeyboardWindowStateLeafPc();
    PCNativeWarningDebugState.Initialize();
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

extern "C" u32 __cdecl PcGetRefreshRatePc(void) {
    return PCDeviceBackend.parameters.refresh_rate;
}

extern "C" f32 __cdecl PcInputAspectRatioPc(void) {
    f32 value;
    switch (PcWindowOptions.aspect_mode) {
        case 0:
            value = 1.3333333730697632f;
            break;
        case 1:
            value = 1.7777777910232544f;
            break;
        case 2:
            value = 1.600000023841858f;
            break;
        case 3:
            value = PCDeviceBackend.active_aspect;
            break;
        default:
            value = 1.3333333730697632f;
            break;
    }
    return value;
}

extern "C" {

    void __cdecl NudxFw_D3DBeginCriticalSection(void) {
        NuThreadCriticalSectionBegin(PCFrameCriticalSectionIndex);
    }

    void __cdecl NudxFw_D3DEndCriticalSection(void) {
        NuThreadCriticalSectionEnd(PCFrameCriticalSectionIndex);
    }

    void __cdecl PCFrameSetLoadingYieldEnabled(i32 enabled) {
        PCFrameLoadingYieldEnabledPC = enabled != 0;
    }

    i32 __cdecl PCFrameLoadingYieldEnabled(void) {
        return PCFrameLoadingYieldEnabledPC;
    }
}

extern "C" void __cdecl PCDeviceSetWorldIdentityPc(void) {
    static D3DXMATRIX identity(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                               1.0f);
    PCDeviceSetTransform(0x100, (const NUMTX *)&identity);
}

extern "C" {

    void __cdecl PCDeviceResetState(void) {
        i32 stage;

        NuThreadCriticalSectionBegin(PCFrameCriticalSectionIndex);
        PCDeviceSetRenderState(137, 0);
        PCDeviceSetRenderState(7, 1);
        PCDeviceSetRenderState(14, 1);
        PCDeviceSetRenderState(15, 0);
        PCDeviceSetRenderState(19, 2);
        PCDeviceSetRenderState(20, 1);
        PCDeviceSetRenderState(24, 0);
        PCDeviceSetRenderState(25, 8);
        PCDeviceSetRenderState(27, 0);
        PCDeviceSetRenderState(52, 0);
        for (stage = 0; stage < 8; ++stage) {
            PCDeviceSetSamplerState(stage, 5, 2);
            PCDeviceSetSamplerState(stage, 6, 2);
            PCDeviceSetSamplerState(stage, 7, 2);
        }
        PCDeviceSetTexture(0, NULL);
        PCDeviceSetTexture(1, NULL);
        PCDeviceSetRenderState(22, 1);
        PCDeviceSetTextureStageState(1, 1, 1);
        PCDeviceSetTextureStageState(1, 4, 1);
        NuThreadCriticalSectionEnd(PCFrameCriticalSectionIndex);
    }
}

extern "C" void __cdecl PCDeviceResetSamplerDefaultsPc(void) {
    u32 stage = 0;
    if (PCDeviceBackend.caps.sampler_count_pc == 0) {
        return;
    }
    do {
        PCDeviceSetSamplerState(stage, 5, 2);
        PCDeviceSetSamplerState(stage, 6, 2);
        PCDeviceSetSamplerState(stage, 7, 2);
        PCDeviceSetSamplerState(stage, 9, 0);
        ++stage;
    } while (stage != PCDeviceBackend.caps.sampler_count_pc);
}

extern "C" i32 __cdecl PCFrameBeginScenePc(i32 begin) {
    (void)begin;
    PCDeviceBeginScenePc();
    return 0;
}

extern "C" i32 __cdecl PCFramePresentNativePc(i32 mode_unused, i32 flag_unused) {
    PCFramePresentationStatePc = 0;
    PCDeviceBackend.Present(NULL, NULL, NULL, NULL);
    PCPresentationPlatformLeafPc();
    return 0;
}

extern "C" void __cdecl PCFrameResetTargetsPc(void) {
    PCDeviceBackend.ResetTargets();
}

extern "C" void __cdecl PCFrameConfigureTargetsPc(void) {
    PCDeviceBackend.ConfigureCurrentTargets();
}

extern "C" void __cdecl PCDevicePostEffectBarrierPc(void) {
    PCDeviceBackend.PostEffectBarrierPc();
}

extern "C" i32 __cdecl PcApiGetCallBufferCount(void) {
    return PcApiCallBufferCount;
}

extern "C" void *__cdecl PcApiGetCallBuffer(void) {
    return PcApiCallBuffer;
}

extern "C" void __cdecl PcFreeApiCallBuffer(void) {
    if (PcApiCallBuffer != NULL) {
        NuFree(PcApiCallBuffer, ".\\d3dApiCalls.cpp", 0x451);
    }
    PcApiCallBuffer = NULL;
    PcApiCallBufferCount = 0;
}

extern "C" void __cdecl PcDestroyPlatform(void) {
    PCNativeWarningDebugState.Shutdown();
    nu_keyboard_owner.Shutdown();
    PcRendererShutdown(0);
    PCDeviceBackend.Shutdown();
}

extern "C" void __cdecl PCGraphicsRecreateResources(i32 reset_available) {
    PCDeviceBackend.RestoreDefaultPoolResources();
    if (reset_available != 0) {
        NuPostEffectResizePc();
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
