#include "nu2api/nuwin32/d3dCalls.h"
#include <excpt.h>
#include "decomp.h"
#include "gameapi/edtools/edui.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "nu2api/NuEffectTex_pc.h"
#include "nu2api/PCInput.h"
#include "nu2api/huffdecode.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nuvport.h"
#include "nu2api/nucamera_dlist.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufmv/pc/Bink/dsplay_bink.h"
#include "nu2api/nuhgo_dlist.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nuwin32/d3dApiCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

extern "C" {
    extern const CPU_VENDOR_WORDS CpuVendorAmd = {0x68747541u, 0x69746e65u, 0x444d4163u, {0}};
    extern const CPU_VENDOR_WORDS CpuVendorCyrix = {0x69727943u, 0x736e4978u, 0x64616574u, {0}};
    extern const CPU_VENDOR_WORDS CpuVendorNoCpuid = {0x6f4e5f5fu, 0x5550435fu, 0x5f5f4449u, {0}};
    extern const CPU_VENDOR_WORDS CpuVendorIntel = {0x756e6547u, 0x49656e69u, 0x6c65746eu, {0}};
}
extern "C" {
    extern HRESULT PCDeviceLastResult;
}
extern "C" const char *WINAPI DXGetErrorString9A(HRESULT result);
/* IDirect3DSurface9 methods used by the BMP dump; the shared resource table models textures. */
typedef struct PC_SURFACE PC_SURFACE;

typedef struct PC_SURFACE_METHODS {                                                   // size 0x3c
    void *unknown_0000[2];                                                            // 0x00
    ULONG(__stdcall *release)(PC_SURFACE *);                                          // 0x08
    void *unknown_000c[9];                                                            // 0x0c
    HRESULT(__stdcall *get_desc)(PC_SURFACE *, PC_TEXTURE_LEVEL_DESC *);              // 0x30
    HRESULT(__stdcall *lock_rect)(PC_SURFACE *, PC_LOCKED_RECT *, const RECT *, u32); // 0x34
    HRESULT(__stdcall *unlock_rect)(PC_SURFACE *);                                    // 0x38
} PC_SURFACE_METHODS;

struct PC_SURFACE {
    const PC_SURFACE_METHODS *methods;
};

extern "C" void NuPs2GetViewport(NUVPCURRENT *viewport) {
    if (viewport != NULL) {
        viewport->x = PCDeviceBackend.requested_viewport.x;
        viewport->y = PCDeviceBackend.requested_viewport.y;
        viewport->width = PCDeviceBackend.requested_viewport.width;
        viewport->height = PCDeviceBackend.requested_viewport.height;
        memcpy(&viewport->min_z, &PCDeviceBackend.requested_viewport.min_z, sizeof(viewport->min_z));
        memcpy(&viewport->max_z, &PCDeviceBackend.requested_viewport.max_z, sizeof(viewport->max_z));
    }
}

// Platform aspect supplier.
extern "C" void PCDeviceLightEnable(u32 index, BOOL enable) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->light_enable(PCDeviceBackend.device, index, enable);
}

extern "C" void PCDeviceMultiplyTransform(i32 state, const NUMTX *matrix) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->multiply_transform(PCDeviceBackend.device, state, matrix);
}

extern "C" void PCDeviceProcessVertices(u32 source_start, u32 destination_index, u32 count, PC_BUFFER_RESOURCE *destination,
                                        PC_TEXTURE_RESOURCE *declaration, u32 flags) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->process_vertices(PCDeviceBackend.device, source_start, destination_index, count,
                                                                           destination, declaration, flags);
}

extern "C" void PCDeviceSetClipPlane(u32 index, const f32 *plane) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_clip_plane(PCDeviceBackend.device, index, plane);
}

extern "C" void PCDeviceSetClipStatus(const void *status) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_clip_status(PCDeviceBackend.device, status);
}

extern "C" void PCDeviceSetCurrentTexturePalette(u32 palette) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_current_texture_palette(PCDeviceBackend.device, palette);
}

extern "C" void PCDeviceSetCursorPositionEx(i32 x, i32 y, u32 flags) {
    PCDeviceBackend.device->methods->set_cursor_position(PCDeviceBackend.device, x, y, flags);
}

extern "C" void PCDeviceSetCursorProperties(u32 x, u32 y, PC_TEXTURE_RESOURCE *bitmap) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_cursor_properties(PCDeviceBackend.device, x, y, bitmap);
}

extern "C" void PCDeviceSetDepthStencilSurface(PC_TEXTURE_RESOURCE *surface) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_depth_stencil_surface(PCDeviceBackend.device, surface);
}

extern "C" void PCDeviceSetDialogBoxMode(BOOL enable) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_dialog_box_mode(PCDeviceBackend.device, enable);
}

// noinline: the original calls PCDeviceSetFVFPc out of line from PCGraphicsDrawUnitBox; VC8 would inline it there.
extern "C" __declspec(noinline) void PCDeviceSetFVFPc(u32 fvf) {
    PCDeviceStateCache.vertex_declaration = NULL;
    PC_D3D9_DEVICE *device = PCDeviceBackend.device;
    PCDeviceLastResult = device->methods->set_fvf(device, fvf);
}

extern "C" {
    u8 PCDeviceStateCacheEnabled = 1;
}

// noinline: the original calls PCDeviceSetIndices out of line from PC_DEVICE_BACKEND::DrawPostEffectIndexed; VC8
// would inline it there.
extern "C" __declspec(noinline) void PCDeviceSetIndices(PC_BUFFER_RESOURCE *buffer) {
    if (PCDeviceStateCacheEnabled != 0) {
        if (PCDeviceStateCache.index_buffer == buffer) {
            return;
        }
        PCDeviceStateCache.index_buffer = buffer;
    }
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_indices(PCDeviceBackend.device, buffer);
}

extern "C" void PCDeviceSetLight(u32 index, const void *light) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_light(PCDeviceBackend.device, index, light);
}

extern "C" void PCDeviceSetMaterial(const void *material) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_material(PCDeviceBackend.device, material);
}

extern "C" void PCDeviceSetNPatchMode(f32 segments) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_n_patch_mode(PCDeviceBackend.device, segments);
}

extern "C" void PCDeviceSetPaletteEntries(u32 palette, const PALETTEENTRY *entries) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_palette_entries(PCDeviceBackend.device, palette, entries);
}

extern "C" void PCDeviceSetPixelShader(PC_TEXTURE_RESOURCE *shader) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_pixel_shader(PCDeviceBackend.device, shader);
    SAGA_SKIN_CALL(skin_trace_bind, 1, (unsigned)shader, PCDeviceLastResult);
}

extern "C" void PCDeviceSetPixelShaderConstantBPc(u32 start, const BOOL *data, u32 count) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_pixel_shader_constant_b(PCDeviceBackend.device, start, data, count);
}

extern "C" void PCDeviceSetPixelShaderConstantsF(UINT start, const f32 *values, UINT count) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_pixel_shader_constant_f(PCDeviceBackend.device, start, values, count);
    SAGA_SKIN_CALL(skin_trace_upload, 1, start, count, values, PCDeviceLastResult);
}

extern "C" void PCDeviceSetPixelShaderConstantIPc(u32 start, const i32 *data, u32 count) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_pixel_shader_constant_i(PCDeviceBackend.device, start, data, count);
}

// noinline: the original calls PCDeviceSetRenderState out of line from PC_DEVICE_BACKEND::SetColorWriteMask; VC8
// would inline it there.
extern "C" __declspec(noinline) void PCDeviceSetRenderState(i32 state, u32 value) {
    i16 cache_index;

    if (PCDeviceStateCacheEnabled != 0) {
        switch (state) {
            case D3DRS_COLORWRITEENABLE:
                cache_index = 0;
                break;
            case D3DRS_COLORWRITEENABLE1:
                cache_index = 1;
                break;
            case D3DRS_COLORWRITEENABLE2:
                cache_index = 2;
                break;
            case D3DRS_COLORWRITEENABLE3:
                cache_index = 3;
                break;
            case D3DRS_ZENABLE:
                cache_index = 4;
                break;
            case D3DRS_ZWRITEENABLE:
                cache_index = 5;
                break;
            case D3DRS_ZFUNC:
                cache_index = 6;
                break;
            case D3DRS_CULLMODE:
                cache_index = 7;
                break;
            case D3DRS_ALPHATESTENABLE:
                cache_index = 8;
                break;
            case D3DRS_ALPHABLENDENABLE:
                cache_index = 9;
                break;
            case D3DRS_SRCBLEND:
                cache_index = 10;
                break;
            case D3DRS_DESTBLEND:
                cache_index = 11;
                break;
            default:
                goto submit;
        }
        if ((u32)PCDeviceStateCache.render_states[cache_index] == value) {
            return;
        }
        PCDeviceStateCache.render_states[cache_index] = (i32)value;
    }

submit:
    PCDeviceStateCache.state_key = -1;
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_render_state(PCDeviceBackend.device, state, value);
}

// noinline: the original calls PCDeviceSetSamplerState out of line from PC_DEVICE_BACKEND::CreateOrResetDevice; VC8
// would inline it there.
extern "C" __declspec(noinline) void PCDeviceSetSamplerState(u32 stage, i32 state, u32 value) {
    PCDeviceSamplerInvalidation[stage][0] = -1;
    PCDeviceSamplerInvalidation[stage][1] = -1;
    if (PCDeviceStateCacheEnabled != 0 && stage < 8 && state < 14 && state != D3DSAMP_BORDERCOLOR) {
        if ((u32)PCDeviceStateCache.sampler_states[stage][state] == value) {
            return;
        }
        PCDeviceStateCache.sampler_states[stage][state] = (i32)value;
    }
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_sampler_state(PCDeviceBackend.device, stage, state, value);
}

extern "C" void PCDeviceSetScissorRect(const RECT *rect) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_scissor_rect(PCDeviceBackend.device, rect);
}

// noinline: the original calls PCDeviceSetSoftwareVertexProcessing out of line from
// PC_DEVICE_BACKEND::CreateOrResetDevice; VC8 would inline it there.
__declspec(noinline) void PCDeviceSetSoftwareVertexProcessing(BOOL enabled) {
    PC_D3D9_DEVICE *view = PCDeviceBackend.device;
    PCDeviceLastResult = view->methods->set_software_vertex_processing(view, enabled);
}

extern "C" void PCDeviceSetStreamSource(u32 stream, PC_BUFFER_RESOURCE *buffer, u32 offset, u32 stride) {
    if (PCDeviceStateCacheEnabled != 0) {
        if (PCDeviceStateCache.vertex_buffer == buffer && PCDeviceStateCache.stream_stride == stride &&
            PCDeviceStateCache.stream_number == stream && PCDeviceStateCache.stream_offset == offset) {
            return;
        }
        PCDeviceStateCache.vertex_buffer = buffer;
        PCDeviceStateCache.stream_number = stream;
        PCDeviceStateCache.stream_stride = stride;
        PCDeviceStateCache.stream_offset = offset;
    }
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_stream_source(PCDeviceBackend.device, stream, buffer, offset, stride);
}

extern "C" void PCDeviceSetStreamSourceFreq(u32 stream, u32 setting) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_stream_source_freq(PCDeviceBackend.device, stream, setting);
}

extern "C" void PCDeviceSetTexture(u32 stage, void *texture) {
    if (PCDeviceStateCacheEnabled != 0 && stage < 5) {
        if (PCDeviceStateCache.textures[stage] == texture) {
            return;
        }
        PCDeviceStateCache.textures[stage] = texture;
    }
    PC_D3D9_DEVICE *device = PCDeviceBackend.device;
    PCDeviceLastResult = device->methods->set_texture(device, stage, texture);
}

extern "C" void PCDeviceSetTextureStageState(u32 stage, i32 state, u32 value) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_texture_stage_state(PCDeviceBackend.device, stage, state, value);
}

extern "C" HRESULT PCDeviceSetTransform(i32 state, const NUMTX *matrix) {
    HRESULT result = PCDeviceBackend.device->methods->set_transform(PCDeviceBackend.device, state, matrix);
    PCDeviceLastResult = result;
    return result;
}

// noinline: the original calls PCDeviceSetVertexDeclaration out of line from
// PC_DEVICE_BACKEND::DrawPostEffectQuad; VC8 would inline it there.
extern "C" __declspec(noinline) void PCDeviceSetVertexDeclaration(PC_TEXTURE_RESOURCE *declaration) {
    if (PCDeviceStateCacheEnabled != 0) {
        if (PCDeviceStateCache.vertex_declaration == declaration) {
            return;
        }
        PCDeviceStateCache.vertex_declaration = declaration;
    }
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_vertex_declaration(PCDeviceBackend.device, declaration);
}

extern "C" void PCDeviceSetVertexShader(PC_TEXTURE_RESOURCE *shader) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_vertex_shader(PCDeviceBackend.device, shader);
    SAGA_SKIN_CALL(skin_trace_bind, 0, (unsigned)shader, PCDeviceLastResult);
}

extern "C" void PCDeviceSetVertexShaderConstantBPc(u32 start, const BOOL *data, u32 count) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_vertex_shader_constant_b(PCDeviceBackend.device, start, data, count);
}

extern "C" void PCDeviceSetVertexShaderConstantsF(UINT start, const f32 *values, UINT count) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_vertex_shader_constant_f(PCDeviceBackend.device, start, values, count);
    SAGA_SKIN_CALL(skin_trace_upload, 0, start, count, values, PCDeviceLastResult);
}

extern "C" void PCDeviceSetVertexShaderConstantIPc(u32 start, const i32 *data, u32 count) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_vertex_shader_constant_i(PCDeviceBackend.device, start, data, count);
}

// noinline: the original calls NuVpSubmit out of line from PC_DEVICE_BACKEND::ResetViewport; VC8 would inline it
// there.
extern "C" __declspec(noinline) void NuVpSubmit(const PC_VIEWPORT *viewport) {
    if (viewport != NULL) {
        PCDeviceBackend.ApplyViewport(viewport);
    }
}

extern "C" i32 PCDeviceShowCursor(i32 show) {
    if (PCDeviceBackend.device != NULL) {
        NudxFw_D3DBeginCriticalSection();
        PCDeviceLastResult = PCDeviceBackend.device->methods->show_cursor(PCDeviceBackend.device, show);
        NudxFw_D3DEndCriticalSection();
    }
    return 0;
}

extern "C" i32 PCDeviceSetCursorPosition(i32 x, i32 y) {
    PCDeviceBackend.device->methods->set_cursor_position(PCDeviceBackend.device, x, y, 0);
    return 0;
}

extern "C" void PCDeviceStretchRect(PC_TEXTURE_RESOURCE *source, const RECT *source_rect, PC_TEXTURE_RESOURCE *destination,
                                    const RECT *destination_rect, i32 filter) {
    PCDeviceLastResult =
        PCDeviceBackend.device->methods->stretch_rect(PCDeviceBackend.device, source, source_rect, destination, destination_rect, filter);
}

// noinline: the original calls PCDeviceTestCooperativeLevel out of line from PC_DEVICE_BACKEND::CreateOrResetDevice;
// VC8 would inline it there.
extern "C" __declspec(noinline) HRESULT PCDeviceTestCooperativeLevel() {
    PC_D3D9_DEVICE *device = PCDeviceBackend.device;
    return device->methods->test_cooperative_level(device);
}

extern "C" void PCDeviceUpdateSurface(PC_TEXTURE_RESOURCE *source, const RECT *source_rect, PC_TEXTURE_RESOURCE *destination,
                                      const POINT *destination_point) {
    PCDeviceLastResult =
        PCDeviceBackend.device->methods->update_surface(PCDeviceBackend.device, source, source_rect, destination, destination_point);
}

extern "C" void PCDeviceUpdateTexture(PC_TEXTURE_RESOURCE *source, PC_TEXTURE_RESOURCE *destination) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->update_texture(PCDeviceBackend.device, source, destination);
}

extern "C" void PCDeviceValidateDevice(u32 *passes) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->validate_device(PCDeviceBackend.device, passes);
}

extern "C" void PCDeviceCreateTextureFromFile(char *path, PC_TEXTURE_RESOURCE **texture) {
    NudxFw_D3DBeginCriticalSection();
    u32 size = static_cast<u32>(NuFileSize(path));
    if (size > 0) {
        void *data = NuFileLoad(path);
        if (data != NULL) {
            PCDeviceLastResult = D3DXCreateTextureFromFileInMemory(reinterpret_cast<IDirect3DDevice9 *>(PCDeviceBackend.device), data, size,
                                                                   reinterpret_cast<IDirect3DTexture9 **>(texture));
            NuFree(data, ".\\d3dCalls.cpp", 1032);
        }
    }
    NudxFw_D3DEndCriticalSection();
}

extern "C" void PCDeviceCreateRenderToSurface(u32 width, u32 height, i32 format, BOOL depth_stencil, i32 depth_stencil_format,
                                              ID3DXRenderToSurface **render) {
    PCDeviceLastResult =
        D3DXCreateRenderToSurface(reinterpret_cast<IDirect3DDevice9 *>(PCDeviceBackend.device), width, height,
                                  static_cast<D3DFORMAT>(format), depth_stencil, static_cast<D3DFORMAT>(depth_stencil_format), render);
}

extern "C" void PCDeviceCreateFont(i32 height, u32 width, u32 weight, u32 mip_levels, BOOL italic, u32 char_set, u32 output_precision,
                                   u32 quality, u32 pitch_and_family, const char *face_name, ID3DXFont **font) {
    PCDeviceLastResult = D3DXCreateFontA(reinterpret_cast<IDirect3DDevice9 *>(PCDeviceBackend.device), height, width, weight, mip_levels,
                                         italic, char_set, output_precision, quality, pitch_and_family, face_name, font);
}

extern "C" void PCDeviceCreateEffectFromFile(const char *path, const D3DXMACRO *defines, ID3DXInclude *include, u32 flags,
                                             ID3DXEffectPool *pool, ID3DXEffect **effect, ID3DXBuffer **errors) {
    PCDeviceLastResult = D3DXCreateEffectFromFileA(reinterpret_cast<IDirect3DDevice9 *>(PCDeviceBackend.device), path, defines, include,
                                                   flags, pool, effect, errors);
}

extern "C" void PCDeviceCreateTextureFromMemory(const void *data, u32 size, PC_TEXTURE_RESOURCE **texture) {
    PCDeviceLastResult = D3DXCreateTextureFromFileInMemory(reinterpret_cast<IDirect3DDevice9 *>(PCDeviceBackend.device), data, size,
                                                           reinterpret_cast<IDirect3DTexture9 **>(texture));
}

extern "C" void PCDeviceCreateTextureFromFileInMemory(const void *data, u32 size, PC_TEXTURE_RESOURCE **texture) {
    PCDeviceLastResult = D3DXCreateTextureFromFileInMemory(reinterpret_cast<IDirect3DDevice9 *>(PCDeviceBackend.device), data, size,
                                                           reinterpret_cast<IDirect3DTexture9 **>(texture));
}

extern "C" void PCDeviceCreateTextureFromMemoryEx(const void *data, u32 size, u32 width, u32 height, u32 levels, u32 usage, i32 format,
                                                  i32 pool, u32 filter, u32 mip_filter, u32 colour_key, D3DXIMAGE_INFO *information,
                                                  PALETTEENTRY *palette, PC_TEXTURE_RESOURCE **resource) {
    PCDeviceLastResult =
        D3DXCreateTextureFromFileInMemoryEx(reinterpret_cast<IDirect3DDevice9 *>(PCDeviceBackend.device), data, size, width, height, levels,
                                            usage, static_cast<D3DFORMAT>(format), static_cast<D3DPOOL>(pool), filter, mip_filter,
                                            colour_key, information, palette, reinterpret_cast<IDirect3DTexture9 **>(resource));
}

extern "C" void PCDeviceCreateCubeTextureFromMemory(const void *data, u32 size, PC_TEXTURE_RESOURCE **resource) {
    PCDeviceLastResult = D3DXCreateCubeTextureFromFileInMemory(reinterpret_cast<IDirect3DDevice9 *>(PCDeviceBackend.device), data, size,
                                                               reinterpret_cast<IDirect3DCubeTexture9 **>(resource));
}

extern "C" void PCDeviceCreateCubeTextureFromFile(char *path, PC_TEXTURE_RESOURCE **resource) {
    u32 size = static_cast<u32>(NuFileSize(path));
    if (size > 0) {
        void *data = NuFileLoad(path);
        if (data != NULL) {
            PCDeviceLastResult = D3DXCreateCubeTextureFromFileInMemory(reinterpret_cast<IDirect3DDevice9 *>(PCDeviceBackend.device), data,
                                                                       size, reinterpret_cast<IDirect3DCubeTexture9 **>(resource));
            NuFree(data, ".\\d3dCalls.cpp", 1116);
        }
    }
}

extern "C" void PCDeviceCreateEffect(const void *data, u32 size, const D3DXMACRO *defines, ID3DXInclude *include, u32 flags,
                                     ID3DXEffectPool *pool, ID3DXEffect **effect, ID3DXBuffer **errors) {
    PCDeviceLastResult = D3DXCreateEffect(reinterpret_cast<IDirect3DDevice9 *>(PCDeviceBackend.device), data, size, defines, include, flags,
                                          pool, effect, errors);
}

extern "C" void PCDeviceReleaseResource(PC_TEXTURE_RESOURCE *resource) {
    PCDeviceLastResult = static_cast<HRESULT>(resource->methods->release(resource));
}

extern "C" void PCDeviceReleaseTexture(PC_TEXTURE_RESOURCE **texture) {
    if (*texture != NULL) {
        (*texture)->methods->release(*texture);
        *texture = NULL;
    }
}

// noinline: the original calls PCDeviceLockVertexBuffer out of line from
// PC_DEVICE_BACKEND::BuildDefaultPoolResources; VC8 would inline it there.
extern "C" __declspec(noinline) void PCDeviceLockVertexBuffer(u32 offset, u32 size, void **data, u32 flags, PC_BUFFER_RESOURCE *resource) {
    PCDeviceLastResult = resource->methods->lock(resource, offset, size, data, flags);
}

// noinline: the original calls PCDeviceUnlockVertexBuffer out of line from
// PC_DEVICE_BACKEND::BuildDefaultPoolResources; VC8 would inline it there.
extern "C" __declspec(noinline) HRESULT PCDeviceUnlockVertexBuffer(PC_BUFFER_RESOURCE *resource) {
    return resource->methods->unlock(resource);
}

// noinline: the original calls PCDeviceLockIndexBuffer out of line from
// PC_DEVICE_BACKEND::BuildDefaultPoolResources; VC8 would inline it there.
extern "C" __declspec(noinline) void PCDeviceLockIndexBuffer(u32 offset, u32 size, void **data, u32 flags, PC_BUFFER_RESOURCE *resource) {
    PCDeviceLastResult = resource->methods->lock(resource, offset, size, data, flags);
}

// noinline: the original calls PCDeviceUnlockIndexBuffer out of line from
// PC_DEVICE_BACKEND::BuildDefaultPoolResources; VC8 would inline it there.
extern "C" __declspec(noinline) HRESULT PCDeviceUnlockIndexBuffer(PC_BUFFER_RESOURCE *resource) {
    return resource->methods->unlock(resource);
}

extern "C" void PCDeviceSetVertexShaderMatrix(u32 start, const f32 *matrix) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_vertex_shader_constant_f(PCDeviceBackend.device, start, matrix, 4);
}

extern "C" void PCDeviceSetVertexShaderRows(u32 start, const f32 *rows) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_vertex_shader_constant_f(PCDeviceBackend.device, start, rows, 1);
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_vertex_shader_constant_f(PCDeviceBackend.device, start + 1, rows + 3, 1);
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_vertex_shader_constant_f(PCDeviceBackend.device, start + 2, rows + 6, 1);
}

extern "C" void PCDeviceSetGammaRamp(const void *ramp) {
    PCDeviceBackend.device->methods->set_gamma_ramp(PCDeviceBackend.device, 0, 1, ramp);
}

extern "C" void PCDeviceGetGammaRamp(void *ramp) {
    PCDeviceBackend.device->methods->get_gamma_ramp(PCDeviceBackend.device, 0, ramp);
}

extern "C" void PCDeviceToggleStateCache(void) {
    PCDeviceStateCacheEnabled = PCDeviceStateCacheEnabled == 0;
}

/* Copies a render-target surface to system memory and writes it as the first free "<prefix>_pcNNN.bmp".
   No caller. */
extern "C" void PCDeviceSaveSurfaceBmp(const char *prefix, PC_SURFACE *surface) {
    typedef HRESULT(__stdcall * CREATE_TEXTURE)(PC_D3D9_DEVICE *, u32, u32, u32, u32, u32, i32, PC_TEXTURE_RESOURCE **, HANDLE *);
    PC_SURFACE *copy;
    PC_TEXTURE_RESOURCE *texture;
    PC_TEXTURE_LEVEL_DESC desc;
    PC_LOCKED_RECT locked;
    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info_header;
    char path[256];
    PC_D3D9_DEVICE *device;
    NUFILE file;
    u8 *row;
    i32 width;
    i32 height;
    i32 pitch;
    i32 index;
    i32 y;

    NudxFw_D3DBeginCriticalSection();
    surface->methods->get_desc(surface, &desc);
    width = desc.width;
    height = desc.height;
    device = PCDeviceBackend.device;
    PCDeviceLastResult =
        reinterpret_cast<CREATE_TEXTURE>(device->methods->create_texture)(device, width, height, 1, 0, desc.format, 2, &texture, NULL);
    texture->methods->get_surface_level(texture, 0, reinterpret_cast<PC_TEXTURE_RESOURCE **>(&copy));
    index = 0;
    sprintf(path, "%s_pc%03d.bmp", prefix, index++);
    while (NuFileSize(path) > 0) {
        sprintf(path, "%s_pc%03d.bmp", prefix, index++);
    }
    device = PCDeviceBackend.device;
    DXGetErrorString9A(device->methods->get_render_target_data(device, reinterpret_cast<PC_TEXTURE_RESOURCE *>(surface),
                                                               reinterpret_cast<PC_TEXTURE_RESOURCE *>(copy)));
    file_header.bfType = 0x4d42;
    file_header.bfSize = width * height * 4 + 0x36;
    file_header.bfReserved1 = 0;
    file_header.bfReserved2 = 0;
    file_header.bfOffBits = 0x36;
    info_header.biSize = 0x28;
    info_header.biWidth = width;
    info_header.biHeight = height;
    info_header.biPlanes = 1;
    info_header.biBitCount = 0x20;
    info_header.biCompression = 0;
    info_header.biSizeImage = width * height * 4;
    info_header.biXPelsPerMeter = 1;
    info_header.biYPelsPerMeter = 1;
    info_header.biClrUsed = 0;
    info_header.biClrImportant = 0;
    copy->methods->lock_rect(copy, &locked, NULL, 0x10);
    file = NuFileOpen(path, 1);
    NuFileWrite(file, &file_header, sizeof(file_header));
    NuFileWrite(file, &info_header, sizeof(info_header));
    pitch = locked.pitch;
    row = (u8 *)locked.bits + (height - 1) * pitch;
    for (y = 0; y < height; y++) {
        NuFileWrite(file, row, width * 4);
        row -= pitch;
    }
    NuFileClose(file);
    copy->methods->unlock_rect(copy);
    if (texture != NULL) {
        texture->methods->release(texture);
        texture = NULL;
    }
    if (copy != NULL) {
        copy->methods->release(copy);
        copy = NULL;
    }
    NudxFw_D3DEndCriticalSection();
}

extern "C" {
    CPU_VENDOR_WORDS CpuVendorName = {0x3f3f3f3fu, 0x3f3f3f3fu, 0x3f3f3f3fu, {0}};
    u32 CpuVendor = 0;
    u32 CpuHas3dnow = 0;
    i32 PCClipUseSSE = 0;
    u32 CpuHasSse2 = 0;
    u32 CpuExtendedLeafMax = 0;
    u32 CpuExtendedFeatures = 0;
    u32 CpuHasCpuid = 0;
    u32 CpuBasicLeafMax = 0;
    u32 CpuProcessorType = 0;
    u32 CpuFamily = 0;
    u32 CpuModel = 0;
    u32 CpuStepping = 0;
    u8 CpuBrandIndex = 0;
    u32 CpuHasFpu = 0;
    u32 CpuHasVme = 0;
    u32 CpuHasDe = 0;
    u32 CpuHasPse = 0;
    u32 CpuHasTsc = 0;
    u32 CpuHasMsr = 0;
    u32 CpuHasPae = 0;
    u32 CpuHasMce = 0;
    u32 CpuHasCmpxchg8b = 0;
    u32 CpuHasApic = 0;
    u32 CpuHasPge = 0;
    u32 CpuHasMca = 0;
    u32 CpuHasCmov = 0;
    u32 CpuHasPat = 0;
    u32 CpuHasMmx = 0;
    u32 CpuInitialized = 0;
}

/* Detects the CPU with a hand-written inline assembly block; the shifts by
   zero come from its uniform field-extraction pattern. */
extern "C" void CpuDetectFeatures(void) {
    __asm {
        pushfd
        pop eax
        mov ecx, eax
        xor eax, 0x200000
        push eax
        popfd
        pushfd
        pop eax
        xor eax, ecx
        jne has_cpuid
        mov CpuHasCpuid, 0
        mov CpuBasicLeafMax, 0
        mov ebx, CpuVendorNoCpuid.word0
        mov edx, CpuVendorNoCpuid.word1
        mov ecx, CpuVendorNoCpuid.word2
        mov CpuVendorName.word0, ebx
        mov CpuVendorName.word1, edx
        mov CpuVendorName.word2, ecx
        jmp done
    has_cpuid:
        mov CpuHasCpuid, 1
        mov eax, 0
        cpuid
        mov CpuBasicLeafMax, eax
        mov CpuVendorName.word0, ebx
        mov CpuVendorName.word1, edx
        mov CpuVendorName.word2, ecx
        cmp ebx, CpuVendorIntel.word0
        jne not_intel
        cmp edx, CpuVendorIntel.word1
        jne not_intel
        cmp ecx, CpuVendorIntel.word2
        jne not_intel
        mov CpuVendor, 1
        jmp vendor_known
    not_intel:
        cmp ebx, CpuVendorAmd.word0
        jne not_amd
        cmp edx, CpuVendorAmd.word1
        jne not_amd
        cmp ecx, CpuVendorAmd.word2
        jne not_amd
        mov CpuVendor, 2
        jmp vendor_known
    not_amd:
        cmp ebx, CpuVendorCyrix.word0
        jne not_cyrix
        cmp edx, CpuVendorCyrix.word1
        jne not_cyrix
        cmp ecx, CpuVendorCyrix.word2
        jne not_cyrix
        mov CpuVendor, 3
        jmp vendor_known
    not_cyrix:
        mov CpuVendor, 0
    vendor_known:
        cmp eax, 0
        je done
        mov eax, 1
        cpuid
        mov edi, eax
        and edi, 0x3000
        shr edi, 12
        mov CpuProcessorType, edi
        mov edi, eax
        and edi, 0xf00
        shr edi, 8
        mov CpuFamily, edi
        mov edi, eax
        and edi, 0xf0
        shr edi, 4
        mov CpuModel, edi
        mov edi, eax
        and edi, 0xf
        shr edi, 0
        mov CpuStepping, edi
        mov CpuBrandIndex, bl
        mov edi, edx
        and edi, 0x1
        shr edi, 0
        mov CpuHasFpu, edi
        mov edi, edx
        and edi, 0x2
        shr edi, 1
        mov CpuHasVme, edi
        mov edi, edx
        and edi, 0x1
        shr edi, 0
        mov CpuHasFpu, edi
        mov edi, edx
        and edi, 0x4
        shr edi, 2
        mov CpuHasDe, edi
        mov edi, edx
        and edi, 0x8
        shr edi, 3
        mov CpuHasPse, edi
        mov edi, edx
        and edi, 0x10
        shr edi, 4
        mov CpuHasTsc, edi
        mov edi, edx
        and edi, 0x20
        shr edi, 5
        mov CpuHasMsr, edi
        mov edi, edx
        and edi, 0x40
        shr edi, 6
        mov CpuHasPae, edi
        mov edi, edx
        and edi, 0x80
        shr edi, 7
        mov CpuHasMce, edi
        mov edi, edx
        and edi, 0x100
        shr edi, 8
        mov CpuHasCmpxchg8b, edi
        mov edi, edx
        and edi, 0x200
        shr edi, 9
        mov CpuHasApic, edi
        mov edi, edx
        and edi, 0x1000
        shr edi, 12
        mov CpuHasPge, edi
        mov edi, edx
        and edi, 0x2000
        shr edi, 13
        mov CpuHasMca, edi
        mov edi, edx
        and edi, 0x4000
        shr edi, 14
        mov CpuHasCmov, edi
        mov edi, edx
        and edi, 0x8000
        shr edi, 15
        mov CpuHasPat, edi
        mov edi, edx
        and edi, 0x800000
        shr edi, 23
        mov CpuHasMmx, edi
        mov eax, CpuVendor
        cmp eax, 1
        je intel_features
        cmp eax, 2
        je amd_features
        cmp eax, 3
        je cyrix_features
        jmp done
    intel_features:
        mov edi, edx
        and edi, 0x2000000
        shr edi, 25
        mov PCClipUseSSE, edi
        mov edi, edx
        and edi, 0x4000000
        shr edi, 26
        mov CpuHasSse2, edi
        jmp done
    amd_features:
        mov edi, edx
        and edi, 0x2000000
        shr edi, 25
        mov PCClipUseSSE, edi
        mov eax, 0x80000000
        cpuid
        mov CpuExtendedLeafMax, eax
        test eax, eax
        je done
        mov eax, 0x80000001
        cpuid
        mov CpuExtendedFeatures, edx
        mov edi, edx
        and edi, 0x80000000
        shr edi, 31
        mov CpuHas3dnow, edi
        jmp done
    cyrix_features:
        mov eax, 0x80000000
        cpuid
        mov CpuExtendedLeafMax, eax
        cmp eax, 0x80000000
        jl done
        mov eax, 0x80000001
        cmp eax, CpuExtendedLeafMax
        ja done
        cpuid
        mov CpuExtendedFeatures, edx
        jmp done
    done:
    }
    if (CpuVendor == 2 && PCClipUseSSE != 0) {
        // Executing an SSE instruction faults when the OS has not enabled SSE state saving.
        __try {
            __asm orps xmm0, xmm0
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            PCClipUseSSE = 0;
        }
    }
    CpuInitialized = 1;
}

/* Lazy accessors for the detected CPU fields. None has a caller. */
extern "C" u32 CpuGetHasCpuid(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasCpuid;
}

extern "C" u32 CpuGetFamily(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuFamily;
}

extern "C" u32 CpuGetProcessorType(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuProcessorType;
}

extern "C" u32 CpuGetModel(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuModel;
}

extern "C" u32 CpuGetStepping(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuStepping;
}

extern "C" {
    u32 CpuUnsetValue0 = 0;
}

extern "C" u32 CpuGetUnsetValue0(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuUnsetValue0;
}

extern "C" u32 CpuGetBasicLeafMax(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuBasicLeafMax;
}

extern "C" {
    u32 CpuUnsetValue1 = 0;
}

extern "C" u32 CpuGetUnsetValue1(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuUnsetValue1;
}

extern "C" u32 CpuGetHasFpu(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasFpu;
}

extern "C" u32 CpuGetHasMmx(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasMmx;
}

extern "C" u32 CpuGetHasTsc(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasTsc;
}

extern "C" u32 CpuGetHasDe(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasDe;
}

extern "C" u32 CpuGetHasApic(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasApic;
}

extern "C" u32 CpuGetHasPat(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasPat;
}

extern "C" u32 CpuGetHasVme(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasVme;
}

extern "C" u32 CpuGetHasPse(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasPse;
}

extern "C" u32 CpuGetHasMsr(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasMsr;
}

extern "C" u32 CpuGetHasPae(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasPae;
}

extern "C" u32 CpuGetHasMce(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasMce;
}

extern "C" u32 CpuGetHasCmpxchg8b(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasCmpxchg8b;
}

extern "C" u32 CpuGetHasPge(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasPge;
}

extern "C" u32 CpuGetHasMca(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasMca;
}

extern "C" u32 CpuGetHasCmov(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasCmov;
}

extern "C" {
    u32 CpuUnsetFeature0 = 0;
}

extern "C" u32 CpuGetUnsetFeature0(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuUnsetFeature0;
}

extern "C" {
    u32 CpuUnsetFeature1 = 0;
}

extern "C" u32 CpuGetUnsetFeature1(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuUnsetFeature1;
}

extern "C" {
    u32 CpuUnsetFeature2 = 0;
}

extern "C" u32 CpuGetUnsetFeature2(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuUnsetFeature2;
}

extern "C" {
    u32 CpuUnsetFeature3 = 0;
}

extern "C" u32 CpuGetUnsetFeature3(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuUnsetFeature3;
}

extern "C" u32 CpuGetHas3dnow(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHas3dnow;
}

extern "C" i32 CpuGetHasSse(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return PCClipUseSSE;
}

extern "C" u32 CpuGetHasSse2(void) {
    if (CpuInitialized == 0) {
        CpuDetectFeatures();
        CpuInitialized = 1;
    }
    return CpuHasSse2;
}

extern "C" {
    PC_GRAPHICS_QUERIED_GPU PCGraphicsQueriedGpu = {0xffffffffu, 0xffffffffu};
    PC_GRAPHICS_GPU_RECORD PCGraphicsGpuRecords[268] = {
        {32u, 0u, "RIVA TNT", "RIVA TNT"},
        {40u, 0u, "RIVA TNT2/TNT2 Pro", "RIVA TNT2/TNT2 Pro"},
        {41u, 0u, "RIVA TNT2 Ultra", "RIVA TNT2 Ultra"},
        {44u, 0u, "Vanta/Vanta LT", "Vanta/Vanta LT"},
        {45u, 0u, "RIVA TNT2 Model 64/Model 64 Pro", "RIVA TNT2 Model 64/Model 64 Pro"},
        {160u, 0u, "Aladdin TNT2", "Aladdin TNT2"},
        {256u, 0u, "GeForce 256", "GeForce 256"},
        {257u, 0u, "GeForce DDR", "GeForce DDR"},
        {272u, 0u, "GeForce2 MX/MX 400", "GeForce2 MX/MX 400"},
        {273u, 0u, "GeForce2 MX 100/200", "GeForce2 MX 100/200"},
        {416u, 0u, "GeForce2 Integrated GPU", "GeForce2 Integrated GPU"},
        {336u, 0u, "GeForce2 GTS/GeForce2 Pro", "GeForce2 GTS/GeForce2 Pro"},
        {337u, 0u, "GeForce2 Ti", "GeForce2 Ti"},
        {338u, 0u, "GeForce2 Ultra", "GeForce2 Ultra"},
        {259u, 0u, "Quadro", "Quadro"},
        {339u, 0u, "Quadro2 Pro", "Quadro2 Pro"},
        {275u, 0u, "Quadro2 MXR/EX/Go", "Quadro2 MXR/EX/Go"},
        {274u, 0u, "GeForce2 Go", "GeForce2 Go"},
        {368u, 0u, "GeForce4 MX 460", "GeForce4 MX 460"},
        {369u, 0u, "GeForce4 MX 440", "GeForce4 MX 440"},
        {370u, 0u, "GeForce4 MX 420", "GeForce4 MX 420"},
        {371u, 0u, "GeForce4 MX 440-SE", "GeForce4 MX 440-SE"},
        {385u, 0u, "GeForce4 MX 440 with AGP8X", "GeForce4 MX 440 with AGP8X"},
        {386u, 0u, "GeForce4 MX 440SE with AGP8X", "GeForce4 MX 440SE with AGP8X"},
        {387u, 0u, "GeForce4 MX 420 with AGP8X", "GeForce4 MX 420 with AGP8X"},
        {496u, 0u, "GeForce4 MX Integrated GPU", "GeForce4 MX Integrated GPU"},
        {395u, 0u, "Quadro4 380 XGL", "Quadro4 380 XGL"},
        {376u, 0u, "Quadro4 550 XGL", "Quadro4 550 XGL"},
        {392u, 0u, "Quadro4 580 XGL", "Quadro4 580 XGL"},
        {378u, 0u, "Quadro NVS", "Quadro NVS"},
        {394u, 0u, "Quadro NVS with AGP8X", "Quadro NVS with AGP8X"},
        {396u, 0u, "Quadro NVS 50 PCI", "Quadro NVS 50 PCI"},
        {357u, 0u, "NVIDIA Quadro NVS 285", "NVIDIA Quadro NVS 285"},
        {512u, 0u, "GeForce3", "GeForce3"},
        {513u, 0u, "GeForce3 Ti 200", "GeForce3 Ti 200"},
        {514u, 0u, "GeForce3 Ti 500", "GeForce3 Ti 500"},
        {515u, 0u, "Quadro DCC", "Quadro DCC"},
        {594u, 0u, "NV25", "NV25"},
        {595u, 0u, "GeForce4 Ti 4200", "GeForce4 Ti 4200"},
        {640u, 0u, "GeForce4 Ti 4800", "GeForce4 Ti 4800"},
        {641u, 0u, "GeForce4 Ti 4200 with AGP8X", "GeForce4 Ti 4200 with AGP8X"},
        {642u, 0u, "GeForce4 Ti 4800 SE", "GeForce4 Ti 4800 SE"},
        {592u, 0u, "GeForce4 Ti 4600", "GeForce4 Ti 4600"},
        {593u, 0u, "GeForce4 Ti 4400", "GeForce4 Ti 4400"},
        {600u, 0u, "Quadro4 900 XGL", "Quadro4 900 XGL"},
        {601u, 0u, "Quadro4 750 XGL", "Quadro4 750 XGL"},
        {603u, 0u, "Quadro4 700 XGL", "Quadro4 700 XGL"},
        {648u, 0u, "Quadro4 980 XGL", "Quadro4 980 XGL"},
        {649u, 0u, "Quadro4 780 XGL", "Quadro4 780 XGL"},
        {769u, 3u, "GeForce FX 5800 Ultra", "GeForce FX 5800 Ultra"},
        {770u, 3u, "GeForce FX 5800", "GeForce FX 5800"},
        {776u, 3u, "Quadro FX 2000", "Quadro FX 2000"},
        {777u, 3u, "Quadro FX 1000", "Quadro FX 1000"},
        {778u, 3u, "ICE FX 2000", "ICE FX 2000"},
        {785u, 3u, "GeForce FX 5600 Ultra", "GeForce FX 5600 Ultra"},
        {786u, 1u, "GeForce FX 5600", "GeForce FX 5600"},
        {787u, 1u, "NV31", "NV31"},
        {788u, 1u, "GeForce FX 5600SE", "GeForce FX 5600SE"},
        {801u, 1u, "GeForce FX 5200 Ultra", "GeForce FX 5200 Ultra"},
        {802u, 1u, "GeForce FX 5200", "GeForce FX 5200"},
        {803u, 1u, "GeForce FX 5200LE", "GeForce FX 5200LE"},
        {833u, 3u, "GeForce FX 5700 Ultra", "GeForce FX 5700 Ultra"},
        {834u, 3u, "GeForce FX 5700", "GeForce FX 5700"},
        {835u, 1u, "GeForce FX 5700LE", "GeForce FX 5700LE"},
        {836u, 1u, "GeForce FX 5700VE", "GeForce FX 5700VE"},
        {837u, 3u, "NV36", "NV36"},
        {839u, 3u, "GeForce FX Go5700", "GeForce FX Go5700"},
        {840u, 3u, "GeForce FX Go5700", "GeForce FX Go5700"},
        {841u, 3u, "NV36M Pro", "NV36M Pro"},
        {843u, 3u, "NV36MAP", "NV36MAP"},
        {847u, 3u, "NV36GL", "NV36GL"},
        {810u, 3u, "NV34GL", "NV34GL"},
        {815u, 3u, "NV34GL", "NV34GL"},
        {816u, 3u, "GeForce FX 5900 Ultra", "GeForce FX 5900 Ultra"},
        {817u, 3u, "GeForce FX 5900", "GeForce FX 5900"},
        {818u, 3u, "GeForce FX 5900XT", "GeForce FX 5900XT"},
        {819u, 3u, "GeForce FX 5950 Ultra", "GeForce FX 5950 Ultra"},
        {818u, 3u, "GeForce FX 5900ZT", "GeForce FX 5900ZT"},
        {811u, 3u, "Quadro FX 500", "Quadro FX 500"},
        {815u, 3u, "Quadro FX 700", "Quadro FX 700"},
        {809u, 3u, "NV34MAP", "NV34MAP"},
        {824u, 3u, "Quadro FX 3000", "Quadro FX 3000"},
        {372u, 0u, "GeForce4 440 Go", "GeForce4 440 Go"},
        {373u, 0u, "GeForce4 420 Go", "GeForce4 420 Go"},
        {374u, 0u, "GeForce4 420 Go 32M", "GeForce4 420 Go 32M"},
        {375u, 0u, "GeForce4 460 Go", "GeForce4 460 Go"},
        {377u, 0u, "GeForce4 440 Go 64M", "GeForce4 440 Go 64M"},
        {381u, 0u, "GeForce4 410 Go 16M", "GeForce4 410 Go 16M"},
        {389u, 0u, "GeForce4 MX 4000", "GeForce4 MX 4000"},
        {390u, 0u, "GeForce4 448 Go", "GeForce4 448 Go"},
        {391u, 0u, "GeForce4 488 Go", "GeForce4 488 Go"},
        {397u, 0u, "GeForce4 448 Go", "GeForce4 448 Go"},
        {646u, 0u, "GeForce4 4200 Go", "GeForce4 4200 Go"},
        {380u, 0u, "Quadro4 500 GoGL", "Quadro4 500 GoGL"},
        {652u, 0u, "Quadro4 700 GoGL", "Quadro4 700 GoGL"},
        {794u, 1u, "GeForce FX Go5600", "GeForce FX Go5600"},
        {795u, 1u, "GeForce FX Go5650", "GeForce FX Go5650"},
        {804u, 1u, "GeForce FX Go5200", "GeForce FX Go5200"},
        {805u, 1u, "GeForce FX Go5250", "GeForce FX Go5250"},
        {806u, 1u, "GeForce FX 5500", "GeForce FX 5500"},
        {807u, 1u, "GeForce FX 5100", "GeForce FX 5100"},
        {808u, 1u, "GeForce FX Go5200 32M/64M", "GeForce FX Go5200 32M/64M"},
        {812u, 1u, "GeForce FX Go53xx", "GeForce FX Go53xx"},
        {813u, 1u, "GeForce FX Go5100", "GeForce FX Go5100"},
        {796u, 1u, "Quadro FX Go700", "Quadro FX Go700"},
        {844u, 1u, "Quadro FX Go1000", "Quadro FX Go1000"},
        {846u, 1u, "Quadro FX 1100", "Quadro FX 1100"},
        {790u, 3u, "NV31M", "NV31M"},
        {791u, 3u, "NV31M Pro", "NV31M Pro"},
        {797u, 3u, "NV31GLM", "NV31GLM"},
        {798u, 3u, "NV31GLM Pro", "NV31GLM Pro"},
        {799u, 3u, "NV31GLM Pro", "NV31GLM Pro"},
        {334u, 1u, "NVIDIA Quadro FX 540", "NVIDIA Quadro FX 540"},
        {78u, 3u, "NVIDIA Quadro FX 4000", "NVIDIA Quadro FX 4000"},
        {204u, 3u, "NVIDIA Quadro FX Go1400", "NVIDIA Quadro FX Go1400"},
        {205u, 3u, "NVIDIA Quadro FX 1400", "NVIDIA Quadro FX 1400"},
        {206u, 3u, "NVIDIA Quadro FX 3450/4000 SDI", "NVIDIA Quadro FX 3450/4000 SDI"},
        {248u, 3u, "NVIDIA Quadro FX 3400/4400", "NVIDIA Quadro FX 3400/4400"},
        {250u, 3u, "NVIDIA GeForce PCX 5750", "NVIDIA GeForce PCX 5750"},
        {251u, 3u, "NVIDIA GeForce PCX 5900", "NVIDIA GeForce PCX 5900"},
        {252u, 1u, "NVIDIA GeForce PCX 5300", "NVIDIA GeForce PCX 5300"},
        {253u, 3u, "NVIDIA Quadro PCI-E Series", "NVIDIA Quadro PCI-E Series"},
        {254u, 1u, "NVIDIA Quadro FX 1300", "NVIDIA Quadro FX 1300"},
        {255u, 3u, "NVIDIA GeForce PCX 4300", "NVIDIA GeForce PCX 4300"},
        {240u, 67u, "NVIDIA GeForce 6800 Ultra", "NVIDIA GeForce 6800 Ultra"},
        {64u, 67u, "NVIDIA GeForce 6800 Ultra", "NVIDIA GeForce 6800 Ultra"},
        {528u, 67u, "NVIDIA GeForce 6800", "NVIDIA GeForce 6800"},
        {529u, 67u, "NVIDIA GeForce 6800", "NVIDIA GeForce 6800"},
        {530u, 67u, "NVIDIA GeForce 6800 LE", "NVIDIA GeForce 6800 LE"},
        {533u, 67u, "NVIDIA GeForce 6800 GT", "NVIDIA GeForce 6800 GT"},
        {65u, 67u, "NVIDIA GeForce 6800", "NVIDIA GeForce 6800"},
        {66u, 67u, "NVIDIA GeForce 6800 LE", "NVIDIA GeForce 6800 LE"},
        {67u, 67u, "NVIDIA NV40", "NVIDIA GeForce 6800 LE"},
        {69u, 67u, "NVIDIA GeForce 6800 GT", "NVIDIA GeForce 6800 GT"},
        {73u, 67u, "NVIDIA NV40GL", "NVIDIA GeForce 6800 LE"},
        {192u, 67u, "NVIDIA GeForce 6800 ", "NVIDIA GeForce 6800 "},
        {193u, 67u, "NVIDIA GeForce 6800 ", "NVIDIA GeForce 6800 "},
        {194u, 67u, "NVIDIA GeForce 6800 LE", "NVIDIA GeForce 6800 LE"},
        {320u, 67u, "NVIDIA GeForce 6600 GT", "NVIDIA GeForce 6600 GT"},
        {321u, 67u, "NVIDIA GeForce 6600", "NVIDIA GeForce 6600"},
        {322u, 67u, "NVIDIA GeForce 6600 LE", "NVIDIA GeForce 6600 LE"},
        {323u, 67u, "NVIDIA NV43", "NVIDIA NV43"},
        {325u, 67u, "NVIDIA GeForce 6610 XL", "NVIDIA GeForce 6610 XL"},
        {331u, 67u, "NVIDIA NV43", "NVIDIA NV43"},
        {332u, 67u, "NVIDIA NV43GL", "NVIDIA NV43GL"},
        {333u, 67u, "NVIDIA NV43GL", "NVIDIA NV43GL"},
        {335u, 67u, "NVIDIA GeForce 6200", "NVIDIA GeForce 6200"},
        {241u, 67u, "NVIDIA GeForce 6600 GT ", "NVIDIA GeForce 6600 GT "},
        {242u, 67u, "NVIDIA GeForce 6600 ", "NVIDIA GeForce 6600 "},
        {242u, 67u, "NVIDIA GeForce 6200 ", "NVIDIA GeForce 6600 "},
        {249u, 67u, "NVIDIA GeForce 6800 Series GPU", "NVIDIA GeForce 6800 Series GPU"},
        {201u, 67u, "NVIDIA GeForce Go 6800 Ultra", "NVIDIA GeForce Go 6800 Ultra"},
        {200u, 67u, "NVIDIA GeForce Go 6800", "NVIDIA GeForce Go 6800"},
        {324u, 67u, "NVIDIA GeForce Go 6600", "NVIDIA GeForce Go 6600"},
        {326u, 67u, "NVIDIA GeForce Go 6600 TE", "NVIDIA GeForce Go 6600 TE"},
        {327u, 67u, "NVIDIA GeForce Go 6200 TE", "NVIDIA GeForce Go 6200 TE"},
        {328u, 67u, "NVIDIA GeForce Go 6600", "NVIDIA GeForce Go 6600"},
        {329u, 67u, "NVIDIA GeForce Go 6200 TE", "NVIDIA GeForce Go 6200 TE"},
        {352u, 67u, "NVIDIA GeForce Go 6200", "NVIDIA GeForce Go 6200"},
        {353u, 67u, "NVIDIA GeForce 6200 TurboCache(TM)", "NVIDIA GeForce 6200 TurboCache(TM)"},
        {354u, 67u, "NVIDIA GeForce 6200 TurboCache(TM)", "NVIDIA GeForce 6200 TurboCache(TM)"},
        {355u, 67u, "NV44", "NV44"},
        {356u, 67u, "NVIDIA GeForce Go 6290", "NVIDIA GeForce Go 6200"},
        {358u, 67u, "NVIDIA GeForce Go 6400", "NVIDIA GeForce Go 6400"},
        {359u, 67u, "NVIDIA GeForce Go 6200", "NVIDIA GeForce Go 6200"},
        {360u, 67u, "NVIDIA GeForce Go 6400", "NVIDIA GeForce Go 6400"},
        {544u, 67u, "NV44", "NV44"},
        {545u, 67u, "GeForce 6200", "GeForce 6200"},
        {546u, 67u, "NV44", "NV44"},
        {552u, 67u, "NV44M", "NV44M"},
        {361u, 67u, "NV44M", "NV44M"},
        {362u, 67u, "NV44GLM", "NV44GLM"},
        {363u, 67u, "NV44GLM", "NV44GLM"},
        {364u, 67u, "NV44GLM", "NV44GLM"},
        {365u, 67u, "NV44GLM", "NV44GLM"},
        {366u, 67u, "NV44GL", "NV44GL"},
        {367u, 67u, "NV44GLM", "NV44GLM"},
        {248u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {248u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {249u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {249u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {288u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {289u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {290u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {291u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {292u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {293u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {294u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {295u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {296u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {297u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {298u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {299u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {300u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {301u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {302u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {303u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {144u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {145u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {146u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {147u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {148u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {149u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {150u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {151u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {152u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {153u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {156u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {157u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {158u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {159u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {400u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {401u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {402u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {403u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {404u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {413u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {414u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {415u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {656u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {657u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {658u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {659u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {660u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {669u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {670u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {671u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {912u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {913u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {914u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {915u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {916u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {925u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {926u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {927u, 67u, "NVIDIA Other", "NVIDIA Other"},
        {464u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {465u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {466u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {467u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {468u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {469u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {470u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {471u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {472u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {473u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {474u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {475u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {476u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {477u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {478u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {479u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {576u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {577u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {578u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {579u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {580u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {581u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {582u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {583u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {584u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {585u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {586u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {587u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {588u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {589u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {590u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {591u, 3u, "NVIDIA Other", "NVIDIA Other"},
        {4294967295u, 67u, "NVIDIA NEW", "NVIDIA NEW"},
    };
    PC_GRAPHICS_GPU_RECORD PCGraphicsGpuFallback = {4294967295u, 67u, "NVIDIA NEW", "NVIDIA NEW"};
    i32 PCGraphicsGpuSelectionEnabled = 0;
    PC_GRAPHICS_GPU_RECORD *PCGraphicsSelectedGpuRecord = NULL;
}

extern "C" i32 PCGraphicsPrepareVendor() {
    u32 vendor = PCDeviceBackend.identifier.vendor;
    u32 device_id = PCDeviceBackend.identifier.device_id;
    PC_GRAPHICS_GPU_RECORD *selected = NULL;
    PCGraphicsGpuSelectionEnabled = 1;
    PCGraphicsQueriedGpu.vendor_id = vendor;
    PCGraphicsQueriedGpu.device_id = device_id;
    if (vendor == 0x10de && device_id != 0xffffffffu) {
        for (i32 i = 0; PCGraphicsGpuRecords[i].device_id != 0xffffffffu; ++i) {
            if (selected != NULL) {
                break;
            }
            if (PCGraphicsGpuRecords[i].device_id == device_id) {
                selected = &PCGraphicsGpuRecords[i];
            }
        }
        if (selected == NULL) {
            selected = &PCGraphicsGpuFallback;
        }
    }
    PCGraphicsSelectedGpuRecord = selected;
    return 0;
}

extern "C" PC_GRAPHICS_GPU_RECORD *PCGraphicsGetSelectedGpu(void) {
    if (PCGraphicsGpuSelectionEnabled == 0) {
        PCGraphicsPrepareVendor();
    }
    return PCGraphicsSelectedGpuRecord;
}
