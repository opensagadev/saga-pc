#include "saga.h"

/* Retail unit 0x6df010-0x6dfea0: definitions in retail address order. */

extern "C" {
    void PCDeviceBeginScenePc(void);
    void PCGraphicsReleaseFramebuffers(void);
    void PCGraphicsRestoreFramebuffers(void);
    void NuRndrGeometryBufferReleasePc(void);
    void NuRndrGeometryBufferRestorePc(void);
    void NuDebrisReleaseVertexBuffersPc(void);
    void debris_InitializeD3D(void);
    void NuRndrDwaReleasePc(void);
    void PcGeometryOriginReleasePc(void);
    void NuPrimReleaseResourcesPc(void);
    void NuKeyboardWindowStateLeafPc(void);
    i32 GameReleaseResourcesPc(void);
    i32 GameRestoreResourcesPc(void);
    void DebrisReleaseResourcesPc(void);
    void DebrisRestoreResourcesPc(void);
    void NuWaterReleaseVertexBufferPc(void);
    void NuGScnReleaseDynamicBuffersPc(void);
    void NuGScnRestoreDynamicBuffersPc(void);
    void NuTexReleaseResourcesPc(void);
    void NuTexRestoreResourcesPc(void);
    void FmvReleasePc(void);
    void FmvRestorePc(void);
    void NuFmvTextureReleasePc(void);
    void NuFmvTextureRestorePc(void);
    void ResizeStillRenderPc(i32, i32);
    void PCGraphicsClearFrameTexturesPc(void);
    f32 fun_005d0920(void);
    f32 fun_005d0930(void);
    void PCDeviceSetGammaRampPc(const void *ramp);
}
extern "C" void NuPostEffectResizePc(void);
extern "C" void fun_006d3120(void);
extern "C" u32 dat_02974ccc;
extern "C" u32 dat_02974cd0;
extern "C" u32 dat_02974cd4;
extern "C" u32 dat_02974cd8;
extern "C" u32 dat_02974cec;
extern "C" u32 dat_02974cf0;

/* The D3DX matrix type with its sixteen-float constructor (only the parts used here). */
struct D3DXMATRIX {
    FLOAT m11;
    FLOAT m12;
    FLOAT m13;
    FLOAT m14;
    FLOAT m21;
    FLOAT m22;
    FLOAT m23;
    FLOAT m24;
    FLOAT m31;
    FLOAT m32;
    FLOAT m33;
    FLOAT m34;
    FLOAT m41;
    FLOAT m42;
    FLOAT m43;
    FLOAT m44;

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

extern "C" void fun_006df010(void) {
}

i32 PC_DEVICE_BACKEND_PC::fun_006df050() {
    return multisample_index != 0;
}

u32 PC_DEVICE_BACKEND_PC::fun_006df060() {
    return state600;
}

u32 PC_DEVICE_BACKEND_PC::fun_006df070() {
    return state604;
}

extern "C" void NuKeyboardCreateWindowState(HWND window, HINSTANCE instance) {
    PCFrameCriticalSectionIndex = PCThreadCriticalSectionCreate_candidate();
    PCDeviceBackend.InitializeAdapter();
    nu_keyboard_owner.Setup(window, instance);
    NuKeyboardWindowStateLeafPc();
    PCNativeWarningDebugState.Initialize();
}

extern "C" void PcRendererCleanup(void) {
    NuRndrDwaReleasePc();
    PcGeometryOriginReleasePc();
    NuPrimReleaseResourcesPc();
    NuDebrisReleaseVertexBuffersPc();
}

extern "C" void PcRendererShutdown(i32 unused) {
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

extern "C" void fun_006df1a0(u32 width, u32 height, u32 refresh, i32 windowed, i32 reposition_window) {
    PCDeviceBackend.fun_006e2ce0(width, height, refresh);
    PCDeviceBackend.requested_windowed = windowed == 1;
    PCDeviceBackend.CreateOrResetDevice(reposition_window);
}

extern "C" void fun_006df1e0(u32 width, u32 height, u32 refresh) {
    PCDeviceBackend.fun_006e2ce0(width, height, refresh);
}

extern "C" void fun_006df200(i32 windowed) {
    PCDeviceBackend.requested_windowed = windowed == 1;
}

extern "C" i32 fun_006df210(void) {
    return PCDeviceBackend.multisample_index != 0;
}

extern "C" i32 fun_006df220(void) {
    return 0;
}

extern "C" i32 fun_006df230(void) {
    return 0;
}

extern "C" i32 fun_006df240(void) {
    return 0;
}

extern "C" i32 fun_006df250(void) {
    return PCDeviceBackend.parameters.windowed == 0;
}

extern "C" u32 fun_006df260(void) {
    return PCDeviceBackend.parameters.width;
}

extern "C" u32 PcGetRefreshRatePc(void) {
    return PCDeviceBackend.parameters.refresh_rate;
}

extern "C" u32 fun_006df280(void) {
    u32 mode = PcWindowOptions.aspect_mode;
    if (mode == 3) {
        mode = PCDeviceBackend.active_aspect_mode;
    }
    return mode;
}

extern "C" f32 PcInputAspectRatioPc(void) {
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

extern "C" u32 fun_006df2f0(void) {
    return PCDeviceBackend.parameters.height;
}

extern "C" {

    void NudxFw_D3DBeginCriticalSection(void) {
        NuThreadCriticalSectionBegin(PCFrameCriticalSectionIndex);
    }

    void NudxFw_D3DEndCriticalSection(void) {
        NuThreadCriticalSectionEnd(PCFrameCriticalSectionIndex);
    }

    void PCFrameSetLoadingYieldEnabled(i32 enabled) {
        PCFrameLoadingYieldEnabledPC = enabled != 0;
    }

    i32 PCFrameLoadingYieldEnabled(void) {
        return PCFrameLoadingYieldEnabledPC;
    }

    void fun_006df340(u32 colour, f32 x, f32 y, i32 align, const char *format, ...) {
        va_list args;
        f32 width;
        va_start(args, format);
        width = NuQFntPrintLenV(system_qfont, format, args);
        x += fun_005d0920();
        y = y - fun_005d0930() * 0.5;
        switch (align) {
            case 0x10:
                NuQFntMove(system_qfont, x, y, 0.0f);
                break;
            case 0x20:
                x -= width;
                NuQFntMove(system_qfont, x, y, 0.0f);
                break;
            case 0x40:
                x -= width * 0.5;
                NuQFntMove(system_qfont, x, y, 0.0f);
                break;
        }
        NuQFntSetColour(system_qfont, colour);
        NuQFntPrintV(system_qfont, format, args);
    }

    void fun_006df470(u32 colour, f32 x, f32 y, i32 align, const char *format, ...) {
        va_list args;
        f32 width;
        va_start(args, format);
        width = NuQFntPrintLenV(system_qfont, format, args);
        switch (align) {
            case 0x10:
                NuQFntMove(system_qfont, x, y, 0.0f);
                break;
            case 0x20:
                NuQFntMove(system_qfont, x - width, y, 0.0f);
                break;
            case 0x40:
                NuQFntMove(system_qfont, x - width * 0.5, y, 0.0f);
                break;
        }
        NuQFntSetColour(system_qfont, 0x80000000);
        NuQFntPrintV(system_qfont, format, args);
        x += fun_005d0920();
        y = y - fun_005d0930() * 0.5;
        switch (align) {
            case 0x10:
                NuQFntMove(system_qfont, x, y, 0.0f);
                break;
            case 0x20:
                NuQFntMove(system_qfont, x - width, y, 0.0f);
                break;
            case 0x40:
                NuQFntMove(system_qfont, x - width * 0.5, y, 0.0f);
                break;
        }
        NuQFntSetColour(system_qfont, colour);
        NuQFntPrintV(system_qfont, format, args);
    }

    void fun_006df670(f32 x, f32 y) {
        f32 line_height;
        f32 fps;
        NuQFntPushPrintMode(2);
        NuQFntPushCoordinateSystem(1);
        NuQFntSet(system_qfont);
        NuQFntSetPointSize(system_qfont, 1.0f, 1.0f);
        line_height = NuQFntHeight(system_qfont) * 0.800000011920929;
        if (nuapi.frame_time > 0.0f) {
            fps = 1.0f / nuapi.frame_time;
        } else {
            fps = 0.0f;
        }
        fun_006df340(0x80008080, x, y, 0x10, "FPS %3.1f", fps);
        y += line_height;
        fun_006df340(0x80008080, x, y, 0x10, "Scene Gobj drawn %d of %d", dat_02974cd0, dat_02974ccc);
        y += line_height;
        fun_006df340(0x80008080, x, y, 0x10, "Opaque Geoms drawn %d", dat_02974cec);
        y += line_height;
        fun_006df340(0x80008080, x, y, 0x10, "Trans Geoms drawn %d", dat_02974cf0);
        y += line_height;
        fun_006df340(0x80008080, x, y, 0x10, "Total 3d geoms drawn %d", dat_02974cd8);
        y += line_height;
        fun_006df340(0x80008080, x, y, 0x10, "Total DIP calls %d", PCDeviceIndexedPrimitiveCallCountPc);
        y += line_height;
        fun_006df340(0x80008080, x, y, 0x10, "Total DP calls %d", PCDevicePrimitiveCallCountPc);
        y += line_height;
        fun_006df340(0x80008080, x, y, 0x10, "Blank 3d Gobjs drawn %d", dat_02974cd4);
        NuQFntPopPrintMode();
        NuQFntPopCoordinateSystem();
        fun_006d3120();
    }
}

extern "C" void PCDeviceSetWorldIdentityPc(void) {
    static D3DXMATRIX identity(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                               1.0f);
    PCDeviceSetTransform(0x100, (const NUMTX *)&identity);
}

extern "C" {

    void PCDeviceResetState(void) {
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

extern "C" void PCDeviceResetSamplerDefaultsPc(void) {
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

extern "C" void fun_006dfa70(f32 gamma) {
    u16 ramp[3][256];
    i32 i;
    for (i = 0; i != 256; ++i) {
        f32 level = i / 255.0;
        i32 value = NuPow(level, gamma) * 65535.0;
        if (value >= 0xffff) {
            value = 0xffff;
        }
        value = value <= 0 ? 0 : value;
        ramp[0][i] = value;
        ramp[1][i] = value;
        ramp[2][i] = value;
    }
    PCDeviceSetGammaRampPc(ramp);
}

extern "C" void fun_006dfb30(f32 gamma) {
    u16 ramp[3][256];
    i32 i;
    for (i = 0; i != 256; ++i) {
        f32 level = i / 255.0;
        i32 value;
        level = (level - 0.5) * gamma + 0.5;
        value = (level * 65535.0);
        if (value >= 0xffff) {
            value = 0xffff;
        }
        value = value <= 0 ? 0 : value;
        ramp[0][i] = value;
        ramp[1][i] = value;
        ramp[2][i] = value;
    }
    PCDeviceSetGammaRampPc(ramp);
}

extern "C" void fun_006dfbf0(PC_TEXTURE_RESOURCE_PC *texture, u32 level, const void *rect, u32 flags,
                             PC_LOCKED_RECT_PC *locked) {
    if (texture != NULL) {
        texture->methods->lock_rect(texture, level, locked, rect, flags);
    }
}

extern "C" void fun_006dfc20(PC_TEXTURE_RESOURCE_PC *texture, u32 level) {
    if (texture != NULL) {
        texture->methods->unlock_rect(texture, level);
    }
}

extern "C" void fun_006dfc40(PC_TEXTURE_RESOURCE_PC *texture, u32 level, PC_TEXTURE_RESOURCE_PC **surface) {
    if (texture != NULL) {
        texture->methods->get_surface_level(texture, level, surface);
    }
}

extern "C" u32 fun_006dfc60(PC_TEXTURE_RESOURCE_PC *texture) {
    return texture->methods->get_level_count(texture);
}

extern "C" void fun_006dfc70(i32 format, u32 width, u32 height, u32 levels, i32 render_target,
                             PC_TEXTURE_RESOURCE_PC **texture) {
    if (render_target == 0) {
        PCDeviceCreateTexture(width, height, levels, 0, format, 1, texture);
    } else {
        PCDeviceCreateTexture(width, height, 1, 1, format, 0, texture);
    }
}

extern "C" PC_TEXTURE_LEVEL_DESC_PC *fun_006dfcc0(PC_TEXTURE_RESOURCE_PC *texture, u32 level) {
    static PC_TEXTURE_LEVEL_DESC_PC description;
    if (texture != NULL) {
        return FAILED(texture->methods->get_level_desc(texture, level, &description)) ? NULL : &description;
    }
    return NULL;
}

extern "C" u32 fun_006dfcf0(void) {
    return PCDeviceBackend.state600;
}

extern "C" u32 fun_006dfd00(void) {
    return PCDeviceBackend.state604;
}

extern "C" void fun_006dfd20(void) {
}

extern "C" i32 PCFrameBeginScenePc(i32 begin) {
    (void)begin;
    PCDeviceBeginScenePc();
    return 0;
}

extern "C" i32 PCFramePresentNativePc(i32 mode_unused, i32 flag_unused) {
    PCFramePresentationStatePc = 0;
    PCDeviceBackend.Present(NULL, NULL, NULL, NULL);
    PCPresentationPlatformLeafPc();
    return 0;
}

extern "C" void PCFrameResetTargetsPc(void) {
    PCDeviceBackend.ResetTargets();
}

extern "C" void PCFrameConfigureTargetsPc(void) {
    PCDeviceBackend.ConfigureCurrentTargets();
}

extern "C" void PCDevicePostEffectBarrierPc(void) {
    PCDeviceBackend.PostEffectBarrierPc();
}

extern "C" i32 PcApiGetCallBufferCount(void) {
    return PcApiCallBufferCount;
}

extern "C" void *PcApiGetCallBuffer(void) {
    return PcApiCallBuffer;
}

extern "C" void PcFreeApiCallBuffer(void) {
    if (PcApiCallBuffer != NULL) {
        NuFree(PcApiCallBuffer, ".\\d3dApiCalls.cpp", 0x451);
    }
    PcApiCallBuffer = NULL;
    PcApiCallBufferCount = 0;
}

extern "C" void PcDestroyPlatform(void) {
    PCNativeWarningDebugState.Shutdown();
    nu_keyboard_owner.Shutdown();
    PcRendererShutdown(0);
    PCDeviceBackend.Shutdown();
}

extern "C" void PCGraphicsRecreateResources(i32 reset_available) {
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
    debris_InitializeD3D();
    DebrisRestoreResourcesPc();
    GameRestoreResourcesPc();
    NuTexRestoreResourcesPc();
    NuGScnRestoreDynamicBuffersPc();
    PCDeviceResetState();
    FmvRestorePc();
    NuFmvTextureRestorePc();
    PCGraphicsClearFrameTexturesPc();
}

extern "C" void fun_006dfea0(void) {
    fun_006df670(0.0f, 160.0f);
}

extern "C" void fun_006dfec0(u32 width, u32 height) {
    PcFreeApiCallBuffer();
    PcApiCallBufferCount = PCDeviceBackend.CaptureScreenshotPc(width, height, &PcApiCallBuffer);
}

extern "C" void fun_006dff30(void) {
}
