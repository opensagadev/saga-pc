/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_NU2API_NUWIN32_D3DCORE_H_PART
#define SAGA_NU2API_NUWIN32_D3DCORE_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/PCInput.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nuwin32/pcapi.h"
#endif

/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NUWIN32_D3DCORE_H_PART == 1 && !defined(SAGA_NU2API_NUWIN32_D3DCORE_H_STOP)

struct GRAPHICS_FACTORY;
struct GRAPHICS_RENDER_TEXTURE;
struct PC_D3D9_DEVICE_PC;

typedef struct PC_TEXTURE_RESOURCE PC_TEXTURE_RESOURCE;

typedef struct PC_LOCKED_RECT { // size 0x8
    i32 pitch;                  // 0x0
    void *bits;                 // 0x4
} PC_LOCKED_RECT;

typedef struct PC_TEXTURE_RESOURCE_METHODS {                                                        // size 0x54
    void *unknown_0000[2];                                                                          // 0x00
    ULONG(__stdcall *release)(PC_TEXTURE_RESOURCE *);                                               // 0x08
    void *unknown_000c[10];                                                                         // 0x0c
    u32(__stdcall *get_level_count)(PC_TEXTURE_RESOURCE *);                                         // 0x34
    void *unknown_0038[3];                                                                          // 0x38
    HRESULT(__stdcall *get_level_desc)(PC_TEXTURE_RESOURCE *, u32, PC_TEXTURE_LEVEL_DESC *);        // 0x44
    HRESULT(__stdcall *get_surface_level)(PC_TEXTURE_RESOURCE *, u32, PC_TEXTURE_RESOURCE **);      // 0x48
    HRESULT(__stdcall *lock_rect)(PC_TEXTURE_RESOURCE *, u32, PC_LOCKED_RECT *, const void *, u32); // 0x4c
    HRESULT(__stdcall *unlock_rect)(PC_TEXTURE_RESOURCE *, u32);                                    // 0x50
} PC_TEXTURE_RESOURCE_METHODS;

struct PC_TEXTURE_RESOURCE {                    // size 0x4
    const PC_TEXTURE_RESOURCE_METHODS *methods; // 0x0
};

typedef struct PC_VERTEX_ELEMENT { // size 0x8
    u16 stream;                    // 0x0
    u16 offset;                    // 0x2
    u8 type;                       // 0x4
    u8 method;                     // 0x5
    u8 usage;                      // 0x6
    u8 usage_index;                // 0x7
} PC_VERTEX_ELEMENT;

typedef struct GRAPHICS_DISPLAY_MODE { // size 0x10
    u32 width;                         // 0x0
    u32 height;                        // 0x4
    u32 refresh_rate;                  // 0x8
    i32 format;                        // 0xc
} GRAPHICS_DISPLAY_MODE;

typedef struct GRAPHICS_PRESENT_PARAMETERS { // size 0x38
    u32 width;                               // 0x00
    u32 height;                              // 0x04
    i32 format;                              // 0x08
    u32 count;                               // 0x0c
    i32 multisample_type;                    // 0x10
    u32 multisample_quality;                 // 0x14
    i32 swap_effect;                         // 0x18
    HWND window;                             // 0x1c
    BOOL windowed;                           // 0x20
    BOOL enable_depth;                       // 0x24
    i32 depth_format;                        // 0x28
    u32 flags;                               // 0x2c
    u32 refresh_rate;                        // 0x30
    u32 interval;                            // 0x34
} GRAPHICS_PRESENT_PARAMETERS;
#ifdef __cplusplus
extern "C" {
#endif
    extern PC_WINDOW_OPTIONS_PC PcWindowOptions;
#ifdef __cplusplus
}
#endif

typedef struct PC_GRAPHICS_GPU_RECORD { // size 0x10
    u32 device_id;                      // 0x0
    u32 field_04;                       // 0x4
    const char *name0;                  // 0x8
    const char *name1;                  // 0xc
} PC_GRAPHICS_GPU_RECORD;

typedef struct PC_GRAPHICS_QUERIED_GPU { // size 0x8
    u32 vendor_id;                       // 0x0
    u32 device_id;                       // 0x4
} PC_GRAPHICS_QUERIED_GPU;

#ifdef __cplusplus
extern "C" {
#endif
    extern PC_GRAPHICS_GPU_RECORD PCGraphicsGpuRecords[268];
    extern PC_GRAPHICS_GPU_RECORD PCGraphicsGpuFallback;
    extern PC_GRAPHICS_QUERIED_GPU PCGraphicsQueriedGpu;
    extern i32 PCGraphicsGpuSelectionEnabled;
    extern PC_GRAPHICS_GPU_RECORD *PCGraphicsSelectedGpuRecord;
#ifdef __cplusplus
}
#endif
typedef void *PC_DEVICE_TEXTURE_CACHE_PC[5];
typedef i32 PC_DEVICE_SAMPLER_CACHE_PC[8][14];
typedef i32 PC_DEVICE_RENDER_CACHE_PC[12];

typedef struct PC_DEVICE_STATE_CACHE_PC {      // size 0x220
    PC_TEXTURE_RESOURCE *vertex_declaration;   // 0x000
    PC_BUFFER_RESOURCE *index_buffer;          // 0x004
    PC_BUFFER_RESOURCE *vertex_buffer;         // 0x008
    u32 stream_stride;                         // 0x00c
    u32 stream_number;                         // 0x010
    u32 stream_offset;                         // 0x014
    PC_DEVICE_TEXTURE_CACHE_PC textures;       // 0x018
    PC_DEVICE_SAMPLER_CACHE_PC sampler_states; // 0x02c
    i32 state_key;                             // 0x1ec
    PC_DEVICE_RENDER_CACHE_PC render_states;   // 0x1f0
#ifdef __cplusplus
    PC_DEVICE_STATE_CACHE_PC() {
        Invalidate();
    }

    void Invalidate();
#endif
} PC_DEVICE_STATE_CACHE_PC;

#ifdef __cplusplus
extern "C" {
#endif

    /* External COM protocols only; no objects or method tables defined here. */
    typedef struct D3DX_BUFFER_METHODS_PC {                                       // size 0x14
        HRESULT(__stdcall *query_interface)(ID3DXBuffer *, const IID *, void **); // 0x00
        ULONG(__stdcall *add_ref)(ID3DXBuffer *);                                 // 0x04
        ULONG(__stdcall *release)(ID3DXBuffer *);                                 // 0x08
        void *(__stdcall *get_buffer_pointer)(ID3DXBuffer *);                     // 0x0c
        DWORD(__stdcall *get_buffer_size)(ID3DXBuffer *);                         // 0x10
    } D3DX_BUFFER_METHODS_PC;

    struct ID3DXBuffer {
        const D3DX_BUFFER_METHODS_PC *methods;
    };

    extern HWND PCInputInstance;
    extern HRESULT PCDeviceLastResult;
    extern u8 PCDeviceStateCacheEnabled;
    extern PC_DEVICE_STATE_CACHE_PC PCDeviceStateCachePc;
    extern u32 PCDevicePrimitiveUPCallCountPc;
    void PCDeviceDrawIndexedPrimitiveUPPc(i32, u32, u32, u32, const void *, i32, const void *, u32);
    extern u32 PCDeviceIndexedPrimitiveUPCallCountPc;
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

typedef struct PC_DEVICE_BACKEND_PC {       // size 0x700
    struct GRAPHICS_FACTORY *factory;       // 0x000
    struct PC_D3D9_DEVICE_PC *device;       // 0x004
    GRAPHICS_CAPS caps;                     // 0x008
    GRAPHICS_PRESENT_PARAMETERS parameters; // 0x138
    PC_VIEWPORT requested_viewport;         // 0x170
    PC_VIEWPORT full_viewport;              // 0x188
    u32 adapter;                            // 0x1a0
    GRAPHICS_ADAPTER_IDENTIFIER identifier; // 0x1a4
    i32 device_type;                        // 0x5f0
    u32 creation_flags;                     // 0x5f4
    u8 force_software;                      // 0x5f8
    u8 unknown_5f9;                         // 0x5f9
    u8 multithreaded;                       // 0x5fa
    u8 unknown_5fb;                         // 0x5fb
    HRESULT present_result;                 // 0x5fc
    u32 state600;                           // 0x600
    u32 state604;                           // 0x604
    GRAPHICS_DISPLAY_MODE desktop_mode;     // 0x608
    u32 mode_count;                         // 0x618
    GRAPHICS_DISPLAY_MODE *modes;           // 0x61c
    u32 active_mode;                        // 0x620
    u32 aspect_mode;                        // 0x624
    f32 aspect;                             // 0x628
    u32 active_aspect_mode;                 // 0x62c
    f32 active_aspect;                      // 0x630
    u8 feature634;                          // 0x634
    u8 unknown_635[3];                      // 0x635
    i32 multisample_index;                  // 0x638
    u8 multisample_tested;                  // 0x63c
    u8 unknown_63d[3];                      // 0x63d
    i32 quality[5];                         // 0x640

    union { // size 0x4
        u32 shader_flags;
        u8 shader_caps;
    }; // 0x654

    u32 render_target_flags;                     // 0x658
    u8 requested_windowed;                       // 0x65c
    u8 unknown_65d[3];                           // 0x65d
    u32 requested_mode;                          // 0x660
    u8 requested_feature;                        // 0x664
    u8 unknown_665[3];                           // 0x665
    i32 requested_multisample;                   // 0x668
    u16 requested_target_count;                  // 0x66c
    u8 saved_windowed;                           // 0x66e
    u8 unknown_66f;                              // 0x66f
    u32 saved_mode;                              // 0x670
    u8 saved_feature;                            // 0x674
    u8 unknown_675[3];                           // 0x675
    i32 saved_multisample;                       // 0x678
    u16 saved_target_count;                      // 0x67c
    u16 unknown_67e;                             // 0x67e
    i32 saved_aspect;                            // 0x680
    PC_TEXTURE_RESOURCE *declaration684;         // 0x684
    PC_BUFFER_RESOURCE *buffer688;               // 0x688
    PC_TEXTURE_RESOURCE *declaration68c;         // 0x68c
    PC_BUFFER_RESOURCE *buffer690;               // 0x690
    PC_BUFFER_RESOURCE *buffer694;               // 0x694
    PC_BUFFER_RESOURCE *buffer698;               // 0x698
    i32 state69c;                                // 0x69c
    i32 state6a0;                                // 0x6a0
    i32 state6a4;                                // 0x6a4
    i32 state6a8;                                // 0x6a8
    PC_TEXTURE_RESOURCE *declaration6ac;         // 0x6ac
    PC_TEXTURE_RESOURCE *back_buffer;            // 0x6b0
    struct GRAPHICS_RENDER_TEXTURE *textures[4]; // 0x6b4
    PC_TEXTURE_RESOURCE *target_surfaces[4];     // 0x6c4
    i32 target_kinds[4];                         // 0x6d4
    u16 target_count;                            // 0x6e4
    u16 unknown_6e6;                             // 0x6e6
    PC_TEXTURE_RESOURCE *current_surface;        // 0x6e8
    PC_TEXTURE_RESOURCE *depth_surface;          // 0x6ec
    PC_TEXTURE_RESOURCE *target_cache[4];        // 0x6f0

#ifdef __cplusplus
    PC_DEVICE_BACKEND_PC();
    ~PC_DEVICE_BACKEND_PC();
    void Shutdown();
    void DrawPostEffectQuadPc();
    void DrawPostEffectIndexedPc();
    void PostEffectBarrierPc();
    HRESULT EndScene();
    HRESULT CopySurfaceTargetPc(PC_EFFECT_SURFACE *target, i32 filter, i32 unused);
    void SetColorWriteMaskPc(u8 colour_enabled, u8 alpha_enabled, i32 target);
    void ReleaseDefaultPoolResources();
    void ApplyViewport(const PC_VIEWPORT *viewport);
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
    void SetRenderTarget(i32 index, PC_TEXTURE_RESOURCE *surface);
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
    PC_VIEWPORT *GetRequestedViewportPc();
    void SetSurfaceRenderTarget(PC_EFFECT_SURFACE *, u32);
    void SetDepthStencilSurface(PC_EFFECT_SURFACE *);
    void ClearDepthStencilSurface();
    void RestoreDefaultPoolResources();
    void CopyBackBuffer(struct GRAPHICS_RENDER_TEXTURE *);
    void CopyCurrentSurface(struct GRAPHICS_RENDER_TEXTURE *);

    GRAPHICS_DISPLAY_MODE *fun_00509090(i32 index);
    i32 fun_005090a0();
    u8 SupportsMultisamplePc(i32);
    u8 HasRequestedModeChangedPc();
    void RestoreSavedModePc();

    u8 fun_005090e0();
    u32 fun_00509110();
    i32 fun_00509120();
    void fun_00509130(i32 multisample);
    void fun_00509140(i32 mode);
    GRAPHICS_RENDER_TEXTURE *fun_005c2190(u16 index);
    i32 fun_005f7860();
    GRAPHICS_CAPS *fun_005f7870();
    GRAPHICS_PRESENT_PARAMETERS *fun_005f7880();
    u16 fun_005f7890();
    i32 fun_005f78a0();
    void fun_005f78c0();
    u16 fun_006e2140();
    void fun_006e22c0();
    HRESULT fun_006e22e0(PC_EFFECT_SURFACE *target, i32 filter);
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
    GRAPHICS_DISPLAY_MODE *fun_006daee0();
    u32 fun_006dbae0();
    u32 fun_006dbaf0();
    void fun_006dbb00(u8 windowed);
#endif
} PC_DEVICE_BACKEND_PC;

#ifdef __cplusplus
extern "C" {
#endif
    extern PC_DEVICE_BACKEND_PC PCDeviceBackend;
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, parameters) + offsetof(GRAPHICS_PRESENT_PARAMETERS, windowed) == 0x158);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, desktop_mode) + offsetof(GRAPHICS_DISPLAY_MODE, width) == 0x608);
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, desktop_mode) + offsetof(GRAPHICS_DISPLAY_MODE, height) == 0x60c);
#ifdef __cplusplus
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, identifier) + offsetof(GRAPHICS_ADAPTER_IDENTIFIER, vendor) == 0x5cc);
#endif
STATIC_ASSERT(offsetof(PC_DEVICE_BACKEND_PC, caps) + offsetof(GRAPHICS_CAPS, sampler_count) == 0xa0);
#undef SAGA_NU2API_NUWIN32_D3DCORE_H_PART
#define SAGA_NU2API_NUWIN32_D3DCORE_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NUWIN32_D3DCORE_H_STOP
#endif
#endif

#undef SAGA_NU2API_NUWIN32_D3DCORE_H_STOP
