#include "legoapi/render/core/screen.h"
#include "nu2api/NuEffectTex_pc.h"
#include "nu2api/PCInput.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d_pc_includes.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nudir_gen.h"
#include "nu2api/nuerror_gen.h"
#include "nu2api/nufmv/pc/Bink/dsplay_bink.h"
#include "nu2api/nugobj.h"
#include "nu2api/nuhgo_dlist.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numotionaccumfilter_gen.h"
#include "nu2api/nuprim_dlist.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nutex_pc.h"
#include "nu2api/nuwater_PC.h"
#include "nu2api/nuwin32/d3dApiCalls.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include "nu2api/pc_fmv_native.h"
#include "legoapi/core/input/gamepads.h"

/* The D3DX matrix type with its sixteen-float constructor (only the parts used here). */
struct D3DXMATRIX { // size 0x40
    FLOAT m11;      // 0x00
    FLOAT m12;      // 0x04
    FLOAT m13;      // 0x08
    FLOAT m14;      // 0x0c
    FLOAT m21;      // 0x10
    FLOAT m22;      // 0x14
    FLOAT m23;      // 0x18
    FLOAT m24;      // 0x1c
    FLOAT m31;      // 0x20
    FLOAT m32;      // 0x24
    FLOAT m33;      // 0x28
    FLOAT m34;      // 0x2c
    FLOAT m41;      // 0x30
    FLOAT m42;      // 0x34
    FLOAT m43;      // 0x38
    FLOAT m44;      // 0x3c

    D3DXMATRIX(FLOAT f11, FLOAT f12, FLOAT f13, FLOAT f14, FLOAT f21, FLOAT f22, FLOAT f23, FLOAT f24, FLOAT f31, FLOAT f32, FLOAT f33,
               FLOAT f34, FLOAT f41, FLOAT f42, FLOAT f43, FLOAT f44) {
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

extern "C" void fun_006df010(void) {
}

i32 PC_DEVICE_BACKEND::IsMultisampled() {
    return multisample_index != 0;
}

u32 PC_DEVICE_BACKEND::GetEnabledOption0() {
    return enabled_option_0;
}

u32 PC_DEVICE_BACKEND::GetEnabledOption1() {
    return enabled_option_1;
}

extern "C" {
    PC_FRAME_STATS PcFrameStats = {0};
    i32 PCFrameCriticalSectionIndex = 0;
    u8 PCFrameLoadingYieldEnabledPC = 0;
}

extern "C" void NuKeyboardCreateWindowState(HWND window, HINSTANCE instance) {
    PCFrameCriticalSectionIndex = PCThreadCriticalSectionCreate_candidate();
    PCDeviceBackend.InitializeAdapter();
    nu_keyboard_owner.Setup(window, instance);
    NuKeyboardWindowStateLeaf();
    PCNativeWarningDebugState.Initialize();
}

extern "C" void PcRendererCleanup(void) {
    NuRndrDwaRelease();
    PcGeometryOriginRelease();
    NuPrimReleaseResources();
    NuDebrisReleaseVertexBuffers();
}

extern "C" void PcRendererShutdown(i32 unused) {
    if (PCDeviceIsAbsent() == 0) {
        PCDeviceSetTexture(0, NULL);
        PCDeviceSetTexture(1, NULL);
        PCDeviceSetTexture(2, NULL);
        PCDeviceSetTexture(3, NULL);
        PCDeviceSetStreamSource(0, NULL, 0, 0);
        PCDeviceSetStreamSource(1, NULL, 0, 0);
        PCDeviceSetStreamSource(2, NULL, 0, 0);
        PCDeviceSetStreamSource(3, NULL, 0, 0);
    }
    PCGraphicsReleaseFramebuffers();
    PCDeviceBackend.ReleaseDefaultPoolResources();
    PC_EFFECT_SURFACE::ReleaseResources();
    PCGraphicsReleaseEffectTextures();
    GameReleaseResources();
    PcGeometryOriginRelease();
    NuPrimReleaseResources();
    NuRndrGeometryBufferRelease();
    DebrisReleaseResources();
    NuDebrisReleaseVertexBuffers();
    NuWaterReleaseVertexBuffer();
    NuGScnReleaseDynamicBuffers();
    NuTexReleaseResources();
    FmvRelease();
    NuFmvTextureRelease();
}

extern "C" void PCDeviceSetMode(u32 width, u32 height, u32 refresh, i32 windowed, i32 reposition_window) {
    PCDeviceBackend.RequestDisplayMode(width, height, refresh);
    PCDeviceBackend.requested_windowed = windowed == 1;
    PCDeviceBackend.CreateOrResetDevice(reposition_window);
}

extern "C" void fun_006df1e0(u32 width, u32 height, u32 refresh) {
    PCDeviceBackend.RequestDisplayMode(width, height, refresh);
}

extern "C" void PCDeviceRequestWindowed(i32 windowed) {
    PCDeviceBackend.requested_windowed = windowed == 1;
}

extern "C" i32 PCDeviceIsMultisampled(void) {
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

extern "C" i32 PCDeviceIsFullscreen(void) {
    return PCDeviceBackend.parameters.windowed == 0;
}

extern "C" u32 PCDeviceGetBackBufferWidth(void) {
    return PCDeviceBackend.parameters.width;
}

extern "C" u32 PcGetRefreshRate(void) {
    return PCDeviceBackend.parameters.refresh_rate;
}

extern "C" u32 PCDeviceGetAspectMode(void) {
    u32 mode = PcWindowOptions.aspect_mode;
    if (mode == PC_ASPECT_AUTO) {
        mode = PCDeviceBackend.active_aspect_mode;
    }
    return mode;
}

extern "C" f32 PcInputAspectRatio(void) {
    f32 value;
    switch (PcWindowOptions.aspect_mode) {
        case PC_ASPECT_4_3:
            value = 1.3333334f;
            break;
        case PC_ASPECT_16_9:
            value = 1.7777778f;
            break;
        case PC_ASPECT_16_10:
            value = 1.6f;
            break;
        case PC_ASPECT_AUTO:
            value = PCDeviceBackend.active_aspect;
            break;
        default:
            value = 1.3333334f;
            break;
    }
    return value;
}

extern "C" u32 PCDeviceGetBackBufferHeight(void) {
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
        x += NuQFntGetLengthScale();
        y = y - NuQFntGetHeightScale() * 0.5;
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

    void PCDebugPrintAligned(u32 colour, f32 x, f32 y, i32 align, const char *format, ...) {
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
        x += NuQFntGetLengthScale();
        y = y - NuQFntGetHeightScale() * 0.5;
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

    void PCDebugDrawFrameStats(f32 x, f32 y) {
        f32 line_height;
        f32 fps;
        NuQFntPushPrintMode(NUQFNT_MODE_2D);
        NuQFntPushCoordinateSystem(NUQFNT_CS_SIXTEENTHS);
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
        fun_006df340(0x80008080, x, y, 0x10, "Scene Gobj drawn %d of %d", PcFrameStats.scene_gobjs_drawn, PcFrameStats.scene_gobjs);
        y += line_height;
        fun_006df340(0x80008080, x, y, 0x10, "Opaque Geoms drawn %d", PcFrameStats.opaque_geoms_drawn);
        y += line_height;
        fun_006df340(0x80008080, x, y, 0x10, "Trans Geoms drawn %d", PcFrameStats.trans_geoms_drawn);
        y += line_height;
        fun_006df340(0x80008080, x, y, 0x10, "Total 3d geoms drawn %d", PcFrameStats.geoms_drawn);
        y += line_height;
        fun_006df340(0x80008080, x, y, 0x10, "Total DIP calls %d", PcFrameStats.indexed_primitive_calls);
        y += line_height;
        fun_006df340(0x80008080, x, y, 0x10, "Total DP calls %d", PcFrameStats.primitive_calls);
        y += line_height;
        fun_006df340(0x80008080, x, y, 0x10, "Blank 3d Gobjs drawn %d", PcFrameStats.blank_gobjs_drawn);
        NuQFntPopPrintMode();
        NuQFntPopCoordinateSystem();
        PcFrameStatsReset();
    }
}

extern "C" void PCDeviceSetWorldIdentity(void) {
    static D3DXMATRIX identity(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    PCDeviceSetTransform(D3DTS_WORLD, (const NUMTX *)&identity);
}

extern "C" {

    void PCDeviceResetState(void) {
        i32 stage;

        NuThreadCriticalSectionBegin(PCFrameCriticalSectionIndex);
        PCDeviceSetRenderState(D3DRS_LIGHTING, 0);
        PCDeviceSetRenderState(D3DRS_ZENABLE, 1);
        PCDeviceSetRenderState(D3DRS_ZWRITEENABLE, 1);
        PCDeviceSetRenderState(D3DRS_ALPHATESTENABLE, 0);
        PCDeviceSetRenderState(D3DRS_SRCBLEND, 2);
        PCDeviceSetRenderState(D3DRS_DESTBLEND, 1);
        PCDeviceSetRenderState(D3DRS_ALPHAREF, 0);
        PCDeviceSetRenderState(D3DRS_ALPHAFUNC, 8);
        PCDeviceSetRenderState(D3DRS_ALPHABLENDENABLE, 0);
        PCDeviceSetRenderState(D3DRS_STENCILENABLE, 0);
        for (stage = 0; stage < 8; ++stage) {
            PCDeviceSetSamplerState(stage, D3DSAMP_MAGFILTER, 2);
            PCDeviceSetSamplerState(stage, D3DSAMP_MINFILTER, 2);
            PCDeviceSetSamplerState(stage, D3DSAMP_MIPFILTER, 2);
        }
        PCDeviceSetTexture(0, NULL);
        PCDeviceSetTexture(1, NULL);
        PCDeviceSetRenderState(D3DRS_CULLMODE, 1);
        PCDeviceSetTextureStageState(1, D3DTSS_COLOROP, 1);
        PCDeviceSetTextureStageState(1, D3DTSS_ALPHAOP, 1);
        NuThreadCriticalSectionEnd(PCFrameCriticalSectionIndex);
    }
}

extern "C" void PCDeviceResetSamplerDefaults(void) {
    u32 stage = 0;
    if (PCDeviceBackend.caps.sampler_count == 0) {
        return;
    }
    do {
        PCDeviceSetSamplerState(stage, D3DSAMP_MAGFILTER, 2);
        PCDeviceSetSamplerState(stage, D3DSAMP_MINFILTER, 2);
        PCDeviceSetSamplerState(stage, D3DSAMP_MIPFILTER, 2);
        PCDeviceSetSamplerState(stage, D3DSAMP_MAXMIPLEVEL, 0);
        ++stage;
    } while (stage != PCDeviceBackend.caps.sampler_count);
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
    PCDeviceSetGammaRamp(ramp);
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
    PCDeviceSetGammaRamp(ramp);
}

extern "C" void PCTextureLock(PC_TEXTURE_RESOURCE *texture, u32 level, const void *rect, u32 flags, PC_LOCKED_RECT *locked) {
    if (texture != NULL) {
        texture->methods->lock_rect(texture, level, locked, rect, flags);
    }
}

extern "C" void PCTextureUnlock(PC_TEXTURE_RESOURCE *texture, u32 level) {
    if (texture != NULL) {
        texture->methods->unlock_rect(texture, level);
    }
}

extern "C" void PCTextureGetSurface(PC_TEXTURE_RESOURCE *texture, u32 level, PC_TEXTURE_RESOURCE **surface) {
    if (texture != NULL) {
        texture->methods->get_surface_level(texture, level, surface);
    }
}

extern "C" u32 PCTextureGetLevelCount(PC_TEXTURE_RESOURCE *texture) {
    return texture->methods->get_level_count(texture);
}

extern "C" void PCTextureCreate(i32 format, u32 width, u32 height, u32 levels, i32 render_target, PC_TEXTURE_RESOURCE **texture) {
    if (render_target == 0) {
        PCDeviceCreateTexture(width, height, levels, 0, format, 1, texture);
    } else {
        PCDeviceCreateTexture(width, height, 1, 1, format, 0, texture);
    }
}

extern "C" PC_TEXTURE_LEVEL_DESC *PCTextureGetLevelDesc(PC_TEXTURE_RESOURCE *texture, u32 level) {
    static PC_TEXTURE_LEVEL_DESC description;
    if (texture != NULL) {
        return FAILED(texture->methods->get_level_desc(texture, level, &description)) ? NULL : &description;
    }
    return NULL;
}

extern "C" u32 PCDeviceGetEnabledOption0(void) {
    return PCDeviceBackend.enabled_option_0;
}

extern "C" u32 PCDeviceGetEnabledOption1(void) {
    return PCDeviceBackend.enabled_option_1;
}

extern "C" void fun_006dfd20(void) {
}

extern "C" i32 PCFrameBeginScene(i32 begin) {
    PCDeviceBeginScene();
    return 0;
}

extern "C" i32 PCFramePresentNative(i32 mode_unused, i32 flag_unused) {
    PCFramePresentationState = 0;
    PCDeviceBackend.Present(NULL, NULL, NULL, NULL);
    PCPresentationPlatformLeaf();
    return 0;
}

extern "C" void PCFrameResetTargets(void) {
    PCDeviceBackend.ResetTargets();
}

extern "C" void PCFrameConfigureTargets(void) {
    PCDeviceBackend.ConfigureCurrentTargets();
}

extern "C" void PCDevicePostEffectBarrier(void) {
    PCDeviceBackend.PostEffectBarrier();
}

extern "C" {
    i32 PcApiCallBufferCount = 0;
}

extern "C" i32 PcApiGetCallBufferCount(void) {
    return PcApiCallBufferCount;
}

extern "C" {
    void *PcApiCallBuffer = NULL;
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
        NuPostEffectResize();
        ResizeStillRender(PS2_REZ_W, PS2_REZ_H);
    }
    PCGraphicsRestoreEffectTextures(reset_available == 1);
    PC_EFFECT_SURFACE::RestoreResources();
    PCGraphicsRestoreFramebuffers();
    NuKeyboardWindowStateLeaf();
    NuRndrGeometryBufferRestore();
    debris_InitializeD3D();
    DebrisRestoreResources();
    GameRestoreResources();
    NuTexRestoreResources();
    NuGScnRestoreDynamicBuffers();
    PCDeviceResetState();
    FmvRestore();
    NuFmvTextureRestore();
    PCGraphicsClearFrameTextures();
}

extern "C" void PCDebugDrawFrameStatsDefault(void) {
    PCDebugDrawFrameStats(0.0f, 160.0f);
}

extern "C" void fun_006dfec0(u32 width, u32 height) {
    PcFreeApiCallBuffer();
    PcApiCallBufferCount = PCDeviceBackend.CaptureScreenshot(width, height, &PcApiCallBuffer);
}

extern "C" void fun_006dff30(void) {
}
