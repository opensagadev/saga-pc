#ifndef SAGA_NU2API_NUWIN32_D3DCORE_H_PART
#define SAGA_NU2API_NUWIN32_D3DCORE_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/PCInput.h, nu2api/nu3d/nu3d_includes.h, nu2api/nucore/common.h,
 * nu2api/nucore/fixed_width.h, nu2api/nufmv/pc/Bink/dsplay_bink.h, nu2api/nustencilshadows_PC.h, nu2api/nuwin32/pcapi.h
 * (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "nu2api/PCInput.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/nufmv/pc/Bink/dsplay_bink.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nuwin32/pcapi.h"
#endif
/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NUWIN32_D3DCORE_H_PART == 1 && !defined(SAGA_NU2API_NUWIN32_D3DCORE_H_STOP)
typedef struct PC_TEXTURE_RESOURCE_PC PC_TEXTURE_RESOURCE_PC;

typedef struct PC_LOCKED_RECT_PC {
    i32 pitch;
    void *bits;
} PC_LOCKED_RECT_PC;

STATIC_ASSERT(sizeof(PC_LOCKED_RECT_PC) == 8);

typedef struct PC_TEXTURE_RESOURCE_METHODS_PC {
    void *unknown_0000[2];
    ULONG(__stdcall *release)(PC_TEXTURE_RESOURCE_PC *);
    void *unknown_000c[10];
    u32(__stdcall *get_level_count)(PC_TEXTURE_RESOURCE_PC *);
    void *unknown_0038[3];
    HRESULT(__stdcall *get_level_desc)(PC_TEXTURE_RESOURCE_PC *, u32, PC_TEXTURE_LEVEL_DESC_PC *);
    HRESULT(__stdcall *get_surface_level)(PC_TEXTURE_RESOURCE_PC *, u32, PC_TEXTURE_RESOURCE_PC **);
    HRESULT(__stdcall *lock_rect)(PC_TEXTURE_RESOURCE_PC *, u32, PC_LOCKED_RECT_PC *, const void *, u32);
    HRESULT(__stdcall *unlock_rect)(PC_TEXTURE_RESOURCE_PC *, u32);
} PC_TEXTURE_RESOURCE_METHODS_PC;

struct PC_TEXTURE_RESOURCE_PC {
    const PC_TEXTURE_RESOURCE_METHODS_PC *methods;
};

STATIC_ASSERT(offsetof(PC_TEXTURE_RESOURCE_METHODS_PC, release) == 8);
STATIC_ASSERT(offsetof(PC_TEXTURE_RESOURCE_METHODS_PC, get_level_count) == 0x34);
STATIC_ASSERT(offsetof(PC_TEXTURE_RESOURCE_METHODS_PC, get_level_desc) == 0x44);
STATIC_ASSERT(offsetof(PC_TEXTURE_RESOURCE_METHODS_PC, lock_rect) == 0x4c);
STATIC_ASSERT(offsetof(PC_TEXTURE_RESOURCE_PC, methods) == 0);

typedef struct PC_VERTEX_ELEMENT_PC {
    u16 stream;
    u16 offset;
    u8 type;
    u8 method;
    u8 usage;
    u8 usage_index;
} PC_VERTEX_ELEMENT_PC;

STATIC_ASSERT(sizeof(PC_VERTEX_ELEMENT_PC) == 8);
STATIC_ASSERT(offsetof(PC_VERTEX_ELEMENT_PC, type) == 4);
STATIC_ASSERT(offsetof(PC_VERTEX_ELEMENT_PC, usage) == 6);

typedef struct GRAPHICS_DISPLAY_MODE_PC {
    u32 width;
    u32 height;
    u32 refresh_rate;
    i32 format;
} GRAPHICS_DISPLAY_MODE_PC;

typedef struct GRAPHICS_PRESENT_PARAMETERS_PC {
    u32 width;
    u32 height;
    i32 format;
    u32 count;
    i32 multisample_type;
    u32 multisample_quality;
    i32 swap_effect;
    HWND window;
    BOOL windowed;
    BOOL enable_depth;
    i32 depth_format;
    u32 flags;
    u32 refresh_rate;
    u32 interval;
} GRAPHICS_PRESENT_PARAMETERS_PC;
#ifdef __cplusplus
extern "C" {
#endif
    extern PC_WINDOW_OPTIONS_PC PcWindowOptions;
#ifdef __cplusplus
}
#endif

typedef struct PC_GRAPHICS_GPU_RECORD_PC {
    u32 device_id;
    u32 field_04;
    const char *name0;
    const char *name1;
} PC_GRAPHICS_GPU_RECORD_PC;

typedef struct PC_GRAPHICS_QUERIED_GPU_PC {
    u32 vendor_id;
    u32 device_id;
} PC_GRAPHICS_QUERIED_GPU_PC;

STATIC_ASSERT(sizeof(PC_GRAPHICS_GPU_RECORD_PC) == 16);
STATIC_ASSERT(offsetof(PC_GRAPHICS_GPU_RECORD_PC, device_id) == 0);
STATIC_ASSERT(offsetof(PC_GRAPHICS_GPU_RECORD_PC, field_04) == 4);
STATIC_ASSERT(offsetof(PC_GRAPHICS_GPU_RECORD_PC, name0) == 8);
STATIC_ASSERT(offsetof(PC_GRAPHICS_GPU_RECORD_PC, name1) == 12);
STATIC_ASSERT(sizeof(PC_GRAPHICS_QUERIED_GPU_PC) == 8);
STATIC_ASSERT(offsetof(PC_GRAPHICS_QUERIED_GPU_PC, vendor_id) == 0);
STATIC_ASSERT(offsetof(PC_GRAPHICS_QUERIED_GPU_PC, device_id) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    extern PC_GRAPHICS_GPU_RECORD_PC PCGraphicsGpuRecords[268];
    extern PC_GRAPHICS_GPU_RECORD_PC PCGraphicsGpuFallback;
    extern PC_GRAPHICS_QUERIED_GPU_PC PCGraphicsQueriedGpu;
    extern i32 PCGraphicsGpuSelectionEnabled;
    extern PC_GRAPHICS_GPU_RECORD_PC *PCGraphicsSelectedGpuRecord;
#ifdef __cplusplus
}
#endif
typedef void *PC_DEVICE_TEXTURE_CACHE_PC[5];
typedef i32 PC_DEVICE_SAMPLER_CACHE_PC[8][14];
typedef i32 PC_DEVICE_RENDER_CACHE_PC[12];

typedef struct PC_DEVICE_STATE_CACHE_PC {
    PC_TEXTURE_RESOURCE_PC *vertex_declaration;
    PC_BUFFER_RESOURCE_PC *index_buffer;
    PC_BUFFER_RESOURCE_PC *vertex_buffer;
    u32 stream_stride;
    u32 stream_number;
    u32 stream_offset;
    PC_DEVICE_TEXTURE_CACHE_PC textures;
    PC_DEVICE_SAMPLER_CACHE_PC sampler_states;
    i32 state_key;
    PC_DEVICE_RENDER_CACHE_PC render_states;
#ifdef __cplusplus
    PC_DEVICE_STATE_CACHE_PC() {
        Invalidate();
    }

    void Invalidate();
#endif
} PC_DEVICE_STATE_CACHE_PC;

STATIC_ASSERT(sizeof(PC_DEVICE_STATE_CACHE_PC) == 0x220);
STATIC_ASSERT(offsetof(PC_DEVICE_STATE_CACHE_PC, vertex_declaration) == 0x0);
STATIC_ASSERT(offsetof(PC_DEVICE_STATE_CACHE_PC, index_buffer) == 0x4);
STATIC_ASSERT(offsetof(PC_DEVICE_STATE_CACHE_PC, vertex_buffer) == 0x8);
STATIC_ASSERT(offsetof(PC_DEVICE_STATE_CACHE_PC, stream_stride) == 0xc);
STATIC_ASSERT(offsetof(PC_DEVICE_STATE_CACHE_PC, stream_number) == 0x10);
STATIC_ASSERT(offsetof(PC_DEVICE_STATE_CACHE_PC, stream_offset) == 0x14);
STATIC_ASSERT(offsetof(PC_DEVICE_STATE_CACHE_PC, textures) == 0x18);
STATIC_ASSERT(offsetof(PC_DEVICE_STATE_CACHE_PC, sampler_states) == 0x2c);
STATIC_ASSERT(offsetof(PC_DEVICE_STATE_CACHE_PC, state_key) == 0x1ec);
STATIC_ASSERT(offsetof(PC_DEVICE_STATE_CACHE_PC, render_states) == 0x1f0);

STATIC_ASSERT(sizeof(GRAPHICS_DISPLAY_MODE_PC) == 0x10);
STATIC_ASSERT(sizeof(GRAPHICS_PRESENT_PARAMETERS_PC) == 0x38);
STATIC_ASSERT(offsetof(GRAPHICS_DISPLAY_MODE_PC, width) == 0x0);
STATIC_ASSERT(offsetof(GRAPHICS_DISPLAY_MODE_PC, height) == 0x4);
STATIC_ASSERT(offsetof(GRAPHICS_DISPLAY_MODE_PC, refresh_rate) == 0x8);
STATIC_ASSERT(offsetof(GRAPHICS_DISPLAY_MODE_PC, format) == 0xc);
STATIC_ASSERT(offsetof(GRAPHICS_PRESENT_PARAMETERS_PC, width) == 0x0);
STATIC_ASSERT(offsetof(GRAPHICS_PRESENT_PARAMETERS_PC, height) == 0x4);
STATIC_ASSERT(offsetof(GRAPHICS_PRESENT_PARAMETERS_PC, format) == 0x8);
STATIC_ASSERT(offsetof(GRAPHICS_PRESENT_PARAMETERS_PC, count) == 0xc);
STATIC_ASSERT(offsetof(GRAPHICS_PRESENT_PARAMETERS_PC, multisample_type) == 0x10);
STATIC_ASSERT(offsetof(GRAPHICS_PRESENT_PARAMETERS_PC, multisample_quality) == 0x14);
STATIC_ASSERT(offsetof(GRAPHICS_PRESENT_PARAMETERS_PC, swap_effect) == 0x18);
STATIC_ASSERT(offsetof(GRAPHICS_PRESENT_PARAMETERS_PC, window) == 0x1c);
STATIC_ASSERT(offsetof(GRAPHICS_PRESENT_PARAMETERS_PC, windowed) == 0x20);
STATIC_ASSERT(offsetof(GRAPHICS_PRESENT_PARAMETERS_PC, enable_depth) == 0x24);
STATIC_ASSERT(offsetof(GRAPHICS_PRESENT_PARAMETERS_PC, depth_format) == 0x28);
STATIC_ASSERT(offsetof(GRAPHICS_PRESENT_PARAMETERS_PC, flags) == 0x2c);
STATIC_ASSERT(offsetof(GRAPHICS_PRESENT_PARAMETERS_PC, refresh_rate) == 0x30);
STATIC_ASSERT(offsetof(GRAPHICS_PRESENT_PARAMETERS_PC, interval) == 0x34);
#ifdef __cplusplus
extern "C" {
#endif

    /* External COM protocols only; no objects or method tables defined here. */
    typedef struct D3DX_BUFFER_METHODS_PC {
        HRESULT(__stdcall *query_interface)(ID3DXBuffer *, const IID *, void **);
        ULONG(__stdcall *add_ref)(ID3DXBuffer *);
        ULONG(__stdcall *release)(ID3DXBuffer *);
        void *(__stdcall *get_buffer_pointer)(ID3DXBuffer *);
        DWORD(__stdcall *get_buffer_size)(ID3DXBuffer *);
    } D3DX_BUFFER_METHODS_PC;

    struct ID3DXBuffer {
        const D3DX_BUFFER_METHODS_PC *methods;
    };

    STATIC_ASSERT(offsetof(D3DX_BUFFER_METHODS_PC, release) == 8);
    STATIC_ASSERT(offsetof(D3DX_BUFFER_METHODS_PC, get_buffer_pointer) == 12);
    extern HWND PCInputInstance;
    extern HRESULT PCDeviceLastResult;
    extern u8 PCDeviceStateCacheEnabled;
    extern PC_DEVICE_STATE_CACHE_PC PCDeviceStateCachePc;
    extern u32 PCDevicePrimitiveUPCallCountPc;
    void PCDeviceDrawIndexedPrimitiveUPPc(i32, u32, u32, u32, const void *, i32, const void *, u32);
    extern u32 PCDeviceIndexedPrimitiveUPCallCountPc;
    extern const char PCDeviceTextureCubeSourcePathPc[];
    extern u32 PCDevicePrimitiveCallCountPc;
    extern u32 PCDeviceIndexedPrimitiveCallCountPc;
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NUWIN32_D3DCORE_H_PART
#define SAGA_NU2API_NUWIN32_D3DCORE_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUWIN32_D3DCORE_H_STOP
#endif
#endif

#if SAGA_NU2API_NUWIN32_D3DCORE_H_PART == 2 && !defined(SAGA_NU2API_NUWIN32_D3DCORE_H_STOP)

typedef struct PC_DEVICE_BACKEND_PC {
    GRAPHICS_FACTORY_PC *factory;
    PC_D3D9_DEVICE_PC *device;
    GRAPHICS_CAPS_PC caps;
    GRAPHICS_PRESENT_PARAMETERS_PC parameters;
    PC_VIEWPORT_PC requested_viewport;
    PC_VIEWPORT_PC full_viewport;
    u32 adapter;
    GRAPHICS_ADAPTER_IDENTIFIER_PC identifier;
    i32 device_type;
    u32 creation_flags;
    u8 force_software;
    u8 unknown_5f9;
    u8 multithreaded;
    u8 unknown_5fb;
    HRESULT present_result;
    u32 state600;
    u32 state604;
    GRAPHICS_DISPLAY_MODE_PC desktop_mode;
    u32 mode_count;
    GRAPHICS_DISPLAY_MODE_PC *modes;
    u32 active_mode;
    u32 aspect_mode;
    f32 aspect;
    u32 active_aspect_mode;
    f32 active_aspect;
    u8 feature634;
    u8 unknown_635[3];
    i32 multisample_index;
    u8 multisample_tested;
    u8 unknown_63d[3];
    i32 quality[5];

    union {
        u32 shader_flags;
        u8 shader_caps;
    };

    u32 render_target_flags;
    u8 requested_windowed;
    u8 unknown_65d[3];
    u32 requested_mode;
    u8 requested_feature;
    u8 unknown_665[3];
    i32 requested_multisample;
    u16 requested_target_count;
    u8 saved_windowed;
    u8 unknown_66f;
    u32 saved_mode;
    u8 saved_feature;
    u8 unknown_675[3];
    i32 saved_multisample;
    u16 saved_target_count;
    u16 unknown_67e;
    i32 saved_aspect;
    PC_TEXTURE_RESOURCE_PC *declaration684;
    PC_BUFFER_RESOURCE_PC *buffer688;
    PC_TEXTURE_RESOURCE_PC *declaration68c;
    PC_BUFFER_RESOURCE_PC *buffer690;
    PC_BUFFER_RESOURCE_PC *buffer694;
    PC_BUFFER_RESOURCE_PC *buffer698;
    i32 state69c;
    i32 state6a0;
    i32 state6a4;
    i32 state6a8;
    PC_TEXTURE_RESOURCE_PC *declaration6ac;
    PC_TEXTURE_RESOURCE_PC *back_buffer;
    GRAPHICS_RENDER_TEXTURE_PC *textures[4];
    PC_TEXTURE_RESOURCE_PC *target_surfaces[4];
    i32 target_kinds[4];
    u16 target_count;
    u16 unknown_6e6;
    PC_TEXTURE_RESOURCE_PC *current_surface;
    PC_TEXTURE_RESOURCE_PC *depth_surface;
    PC_TEXTURE_RESOURCE_PC *target_cache[4];

#ifdef __cplusplus
    PC_DEVICE_BACKEND_PC();
    ~PC_DEVICE_BACKEND_PC();
    void Shutdown();
    void DrawPostEffectQuadPc();
    void DrawPostEffectIndexedPc();
    void PostEffectBarrierPc();
    HRESULT EndScene();
    HRESULT CopySurfaceTargetPc(PC_EFFECT_SURFACE_PC *target, i32 filter, i32 unused);
    void SetColorWriteMaskPc(u8 colour_enabled, u8 alpha_enabled, i32 target);
    void ReleaseDefaultPoolResources();
    void ApplyViewport(const PC_VIEWPORT_PC *viewport);
    void InitializeFactory();
    i32 FindAspect(u32 width, u32 height);
    u32 FindMode(u32 width, u32 height, u32 refresh);
    u32 GetMultisampleType();

    void CreateOrResetDevice(i32 reposition_window);
    void InitializeAdapter();
    void Present(const RECT *, const RECT *, HWND, const RGNDATA *);
    void BeginScene();
    void BuildDefaultPoolResources();
    void ConfigureTargetKinds();
    void ResetTargets();
    void SetRenderTarget(i32 index, PC_TEXTURE_RESOURCE_PC *surface);
    u32 CaptureScreenshotPc(u32 width, u32 height, void **data);
    void ResetViewport();
    void ConfigureCurrentTargets();
    void BeginDefaultTargets();
    void RequestMode(u32 mode);
    void NextResolution();
    void PreviousResolution();
    void NextRefresh();
    void PreviousRefresh();
    void ResetRequestedMode();
    PC_VIEWPORT_PC *GetRequestedViewportPc();
    void SetSurfaceRenderTarget(PC_EFFECT_SURFACE_PC *, u32);
    void SetDepthStencilSurface(PC_EFFECT_SURFACE_PC *);
    void ClearDepthStencilSurface();
    void RestoreDefaultPoolResources();
    void CopyBackBuffer(GRAPHICS_RENDER_TEXTURE_PC *);
    void CopyCurrentSurface(GRAPHICS_RENDER_TEXTURE_PC *);

    GRAPHICS_DISPLAY_MODE_PC *fun_00509090(i32 index);
    i32 fun_005090a0();
    u8 SupportsMultisamplePc(i32);
    u8 HasRequestedModeChangedPc();
    void RestoreSavedModePc();

    u8 fun_005090e0();
    u32 fun_00509110();
    i32 fun_00509120();
    void fun_00509130(i32 multisample);
    void fun_00509140(i32 mode);
    i32 fun_005f7860();
    GRAPHICS_CAPS_PC *fun_005f7870();
    GRAPHICS_PRESENT_PARAMETERS_PC *fun_005f7880();
    u16 fun_005f7890();
    i32 fun_005f78a0();
    void fun_005f78c0();
    u16 fun_006e2140();
    void fun_006e22c0();
    HRESULT fun_006e22e0(PC_EFFECT_SURFACE_PC *target, i32 filter);
    void fun_006e2350();
    void fun_006e2af0();
    void fun_006e2ce0(u32 width, u32 height, u32 refresh);
    i32 fun_006df050();
    u32 fun_006df060();
    u32 fun_006df070();
    i32 fun_006e18d0();
    void fun_006e18e0(u8 feature);
    void fun_006e18f0(u16 count);
    i32 fun_006e1900();
    i32 fun_006dc750();
    GRAPHICS_DISPLAY_MODE_PC *fun_006daee0();
    u32 fun_006dbae0();
    u32 fun_006dbaf0();
    void fun_006dbb00(u8 windowed);
#endif
} PC_DEVICE_BACKEND_PC;

STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, factory) == 0x0);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, device) == 0x4);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, caps) == 0x8);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, parameters) == 0x138);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, requested_viewport) == 0x170);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, full_viewport) == 0x188);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, adapter) == 0x1a0);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, identifier) == 0x1a4);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, device_type) == 0x5f0);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, creation_flags) == 0x5f4);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, force_software) == 0x5f8);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, multithreaded) == 0x5fa);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, present_result) == 0x5fc);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, state600) == 0x600);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, state604) == 0x604);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, desktop_mode) == 0x608);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, mode_count) == 0x618);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, modes) == 0x61c);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, active_mode) == 0x620);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, aspect_mode) == 0x624);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, aspect) == 0x628);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, active_aspect_mode) == 0x62c);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, active_aspect) == 0x630);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, feature634) == 0x634);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, multisample_index) == 0x638);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, multisample_tested) == 0x63c);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, quality) == 0x640);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, shader_flags) == 0x654);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, render_target_flags) == 0x658);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, requested_windowed) == 0x65c);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, requested_mode) == 0x660);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, requested_feature) == 0x664);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, requested_multisample) == 0x668);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, requested_target_count) == 0x66c);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, saved_windowed) == 0x66e);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, saved_mode) == 0x670);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, saved_feature) == 0x674);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, saved_multisample) == 0x678);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, saved_target_count) == 0x67c);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, saved_aspect) == 0x680);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, declaration684) == 0x684);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, buffer688) == 0x688);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, declaration68c) == 0x68c);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, buffer690) == 0x690);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, buffer694) == 0x694);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, buffer698) == 0x698);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, state69c) == 0x69c);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, state6a0) == 0x6a0);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, state6a4) == 0x6a4);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, state6a8) == 0x6a8);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, declaration6ac) == 0x6ac);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, back_buffer) == 0x6b0);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, textures) == 0x6b4);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, target_surfaces) == 0x6c4);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, target_kinds) == 0x6d4);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, target_count) == 0x6e4);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, current_surface) == 0x6e8);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, depth_surface) == 0x6ec);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, target_cache) == 0x6f0);
STATIC_ASSERT(sizeof(PC_DEVICE_BACKEND_PC) == 0x700);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, shader_caps) == 0x654);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, unknown_5f9) == 0x5f9);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, unknown_5fb) == 0x5fb);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, unknown_66f) == 0x66f);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, unknown_67e) == 0x67e);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, unknown_6e6) == 0x6e6);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, device) == 4);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, requested_viewport) == 0x170);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, shader_caps) == 0x654);
#ifdef __cplusplus
extern "C" {
#endif
    extern PC_DEVICE_BACKEND_PC PCDeviceBackend;
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, parameters) + offsetof(GRAPHICS_PRESENT_PARAMETERS_PC, windowed) == 0x158);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, desktop_mode) + offsetof(GRAPHICS_DISPLAY_MODE_PC, width) == 0x608);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, desktop_mode) + offsetof(GRAPHICS_DISPLAY_MODE_PC, height) == 0x60c);
#ifdef __cplusplus
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, identifier) + offsetof(GRAPHICS_ADAPTER_IDENTIFIER_PC, vendor) == 0x5cc);
#endif
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, caps) + offsetof(GRAPHICS_CAPS_PC, sampler_count_pc) == 0xa0);
#undef SAGA_NU2API_NUWIN32_D3DCORE_H_PART
#define SAGA_NU2API_NUWIN32_D3DCORE_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUWIN32_D3DCORE_H_STOP
#endif
#endif

#undef SAGA_NU2API_NUWIN32_D3DCORE_H_STOP
