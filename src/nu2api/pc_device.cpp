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
#include "nu2api/nuwin32/d3dCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

extern "C" void PCDeviceReleaseTexture(PC_TEXTURE_RESOURCE **texture) {
    if (*texture != NULL) {
        (*texture)->methods->release(*texture);
        *texture = NULL;
    }
}

extern "C" void fun_006e5c10(void) {
}

void PC_DEVICE_BACKEND::ApplyViewport(const PC_VIEWPORT *viewport) {
    PC_VIEWPORT applied = *viewport;
    if (applied.width >= (u32)PS2_REZ_W) {
        applied.width = (u32)PS2_REZ_W;
    }
    if (applied.height >= (u32)PS2_REZ_H) {
        applied.height = (u32)PS2_REZ_H;
    }
    device->methods->set_viewport(device, &applied);
    requested_viewport.x = viewport->x;
    requested_viewport.y = viewport->y;
    requested_viewport.width = viewport->width;
    requested_viewport.height = viewport->height;
    memcpy(&requested_viewport.min_z, &viewport->min_z, sizeof(f32));
    memcpy(&requested_viewport.max_z, &viewport->max_z, sizeof(f32));
}

// noinline: the original calls NuVpSubmit out of line from PC_DEVICE_BACKEND::ResetViewport; VC8 would inline it
// there.
extern "C" __declspec(noinline) void NuVpSubmit(const PC_VIEWPORT *viewport) {
    if (viewport != NULL) {
        PCDeviceBackend.ApplyViewport(viewport);
    }
}

extern "C" {
    HRESULT PCDeviceLastResult = 0;
}

extern "C" HRESULT PCDeviceSetTransform(i32 state, const NUMTX *matrix) {
    HRESULT result = PCDeviceBackend.device->methods->set_transform(PCDeviceBackend.device, state, matrix);
    PCDeviceLastResult = result;
    return result;
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

// noinline: the original calls PCDeviceCreateVertexBuffer out of line from
// PC_DEVICE_BACKEND::BuildDefaultPoolResources; VC8 would inline it there.
extern "C" __declspec(noinline) void PCDeviceCreateVertexBuffer(u32 size, u32 usage, u32 vertex_format, i32 pool,
                                                                PC_BUFFER_RESOURCE **resource) {
    if (PCDeviceHardwareVertexProcessingPC == 0) {
        usage |= 0x10;
    }
    PCDeviceLastResult =
        PCDeviceBackend.device->methods->create_vertex_buffer(PCDeviceBackend.device, size, usage, vertex_format, pool, resource, NULL);
}

// noinline: the original calls PCDeviceCreateIndexBuffer out of line from
// PC_DEVICE_BACKEND::BuildDefaultPoolResources; VC8 would inline it there.
extern "C" __declspec(noinline) void PCDeviceCreateIndexBuffer(u32 size, u32 usage, i32 format, i32 pool, PC_BUFFER_RESOURCE **resource,
                                                               HANDLE *shared_handle) {
    if (PCDeviceHardwareVertexProcessingPC == 0) {
        usage |= 0x10;
    }
    PCDeviceLastResult =
        PCDeviceBackend.device->methods->create_index_buffer(PCDeviceBackend.device, size, usage, format, pool, resource, shared_handle);
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

i32 PC_DEVICE_BACKEND::ShaderVariant16() {
    i32 variant = 0;
    if (shader_caps & 0x20) {
        variant = 16;
    }
    return variant;
}

i32 PC_DEVICE_BACKEND::ShaderVariant1() {
    i32 variant = 0;
    if (shader_caps & 0x20) {
        variant = 1;
    }
    return variant;
}

extern "C" void PCDeviceCreateVertexDeclaration(const PC_VERTEX_ELEMENT *elements, PC_TEXTURE_RESOURCE **out) {
    PC_D3D9_DEVICE *device = PCDeviceBackend.device;
    PCDeviceLastResult = device->methods->create_vertex_declaration(device, elements, out);
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

/* Clears the frame counters after the stats overlay has shown them. */
extern "C" void PcFrameStatsReset(void) {
    memset(&PcFrameStats, 0, sizeof(PcFrameStats));
}

extern "C" void PCDeviceDrawPrimitiveUPPc(i32 type, u32 count, const void *vertices, u32 stride) {
    ++PcFrameStats.primitive_up_calls;
    PCDeviceStateCache.vertex_buffer = NULL;
    PCDeviceStateCache.index_buffer = NULL;
    PC_D3D9_DEVICE *device = PCDeviceBackend.device;
    PCDeviceLastResult = device->methods->draw_primitive_up(device, type, count, vertices, stride);
}

extern "C" void fun_006e49f0(void) {
}

extern "C" void fun_006e4a00(void) {
}

// noinline: the original calls PCDeviceDrawIndexedPrimitiveUPPc out of line from PCGraphicsDrawUnitBox; VC8 would
// inline it there.
extern "C" __declspec(noinline) void PCDeviceDrawIndexedPrimitiveUPPc(i32 type, u32 minimum_vertex, u32 vertex_count, u32 primitive_count,
                                                                      const void *indices, i32 index_format, const void *vertices,
                                                                      u32 stride) {
    ++PcFrameStats.indexed_primitive_up_calls;
    PCDeviceStateCache.vertex_buffer = NULL;
    PCDeviceStateCache.index_buffer = NULL;
    PC_D3D9_DEVICE *device = PCDeviceBackend.device;
    PCDeviceLastResult = device->methods->draw_indexed_primitive_up(device, type, minimum_vertex, vertex_count, primitive_count, indices,
                                                                    index_format, vertices, stride);
}

// noinline: the original calls PCDeviceSetFVFPc out of line from PCGraphicsDrawUnitBox; VC8 would inline it there.
extern "C" __declspec(noinline) void PCDeviceSetFVFPc(u32 fvf) {
    PCDeviceStateCache.vertex_declaration = NULL;
    PC_D3D9_DEVICE *device = PCDeviceBackend.device;
    PCDeviceLastResult = device->methods->set_fvf(device, fvf);
}

/* Draws the [-1, 1] cube transformed by matrix as 12 untextured triangles (no caller). */
extern "C" void PCGraphicsDrawUnitBox(NUMTX *matrix) {
    NUVEC4 corners[8] = {{-1.0f, -1.0f, -1.0f, 1.0f}, {-1.0f, 1.0f, -1.0f, 1.0f}, {1.0f, 1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, -1.0f, 1.0f},
                         {-1.0f, -1.0f, 1.0f, 1.0f},  {-1.0f, 1.0f, 1.0f, 1.0f},  {1.0f, 1.0f, 1.0f, 1.0f},  {1.0f, -1.0f, 1.0f, 1.0f}};
    for (i32 i = 0; i < 8; ++i) {
        NuVec4MtxTransformH(&corners[i], &corners[i], matrix);
    }
    u16 indices[36] = {0, 1, 3, 3, 1, 2, 4, 0, 7, 7, 0, 3, 3, 2, 7, 7, 2, 6, 6, 2, 5, 5, 2, 1, 5, 1, 4, 4, 1, 0, 4, 7, 5, 5, 7, 6};
    PCDeviceSetFVFPc(0x4002);
    PCDeviceDrawIndexedPrimitiveUPPc(4, 0, 8, 12, indices, 0x65, corners, sizeof(NUVEC4));
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

i32 PC_WINDOW_OPTIONS::GetDeviceWidth() {
    return device_width;
}

i32 PC_WINDOW_OPTIONS::GetDeviceHeight() {
    return device_height;
}

i32 PC_WINDOW_OPTIONS::GetRefresh() {
    return refresh_rate;
}

i32 PC_WINDOW_OPTIONS::GetMultisample() {
    return multisample;
}

i32 PC_WINDOW_OPTIONS::GetBloomMethod() {
    return bloommethod;
}

i32 PC_WINDOW_OPTIONS::GetDesiredBloomMethod() {
    return desiredbloommethod;
}

i32 PC_WINDOW_OPTIONS::GetDesiredShadowMethod() {
    return desiredshadowmethod;
}

u8 PC_WINDOW_OPTIONS::GetSpeedBlurSupported() {
    return speedblursupported;
}

i32 PC_DEVICE_BACKEND::IsDeviceLost() {
    return device == NULL;
}

void PC_DEVICE_BACKEND::SetRequestedFeature(u8 feature) {
    requested_feature = feature;
}

void PC_DEVICE_BACKEND::SetRequestedTargetCount(u16 count) {
    requested_target_count = count;
}

i32 PC_DEVICE_BACKEND::GetDeviceType() {
    return caps.device_type;
}

extern "C" i32 PcDeviceGetHardwareVertexProcessing(void) {
    return PCDeviceHardwareVertexProcessingPC;
}

extern "C" void fun_006e1920(void) {
}

PC_DEVICE_BACKEND::PC_DEVICE_BACKEND() {
    aspect = 1.3333334f;
    active_aspect = 1.3333334f;
    factory = NULL;
    device = NULL;
    adapter = 0;
    device_type = 1;
    creation_flags = 0;
    force_software = 0;
    force_reference = 0;
    multithreaded = 1;
    present_result = 0;
    enabled_option_0 = 1;
    enabled_option_1 = 1;
    mode_count = 0;
    modes = NULL;
    active_mode = 0;
    aspect_mode = PC_ASPECT_4_3;
    active_aspect_mode = PC_ASPECT_4_3;
    active_feature = 0;
    multisample_index = 0;
    multisample_tested = 0;
    shader_flags = 0;
    requested_windowed = 1;
    requested_mode = 0;
    requested_feature = 0;
    requested_multisample = 0;
    requested_target_count = 1;
    screen_uv_declaration = NULL;
    triangle_buffer = NULL;
    position_uv_declaration = NULL;
    quad_buffer = NULL;
    grid_vertex_buffer = NULL;
    grid_index_buffer = NULL;
    grid_rows = 1;
    grid_columns = 1;
    grid_vertex_count = 0;
    grid_triangle_count = 0;
    screen_colour_uv_declaration = NULL;
    back_buffer = NULL;
    target_count = 1;
    current_surface = NULL;
    depth_surface = NULL;
    memset(&identifier, 0, sizeof(identifier));
    memset(&caps, 0, sizeof(caps));
    memset(&parameters, 0, sizeof(parameters));
    requested_viewport.x = requested_viewport.y = requested_viewport.width = requested_viewport.height = 0;
    requested_viewport.min_z = requested_viewport.max_z = 0.0f;
    desktop_mode.width = desktop_mode.height = desktop_mode.refresh_rate = 0;
    desktop_mode.format = 0;
    parameters.windowed = requested_windowed;
    quality[1] = quality[2] = quality[3] = quality[4] = -1;
    quality[0] = 0;
    render_target_flags = 0;
    for (i32 i = 0; i != 4; ++i) {
        textures[i] = 0;
        target_surfaces[i] = NULL;
        target_kinds[i] = -1;
        target_cache[i] = NULL;
    }
}

void PC_DEVICE_BACKEND::InitializeFactory() {
    factory = reinterpret_cast<GRAPHICS_FACTORY *>(Direct3DCreate9(32));
    if (factory == NULL) {
        PCGraphicsFatalDiagnostic(&PCNativeWarningDebugState, "Failed to initialize DirectX");
    }
    factory->methods->get_device_caps(factory, 0, 1, &caps);
    factory->methods->get_adapter_identifier(factory, 0, 0, &identifier);
    factory->methods->get_adapter_display_mode(factory, 0, &desktop_mode);
    aspect_mode = FindAspect(desktop_mode.width, desktop_mode.height);
    aspect = (f32)desktop_mode.width / (f32)desktop_mode.height;
    active_aspect_mode = aspect_mode;
    active_aspect = aspect;
}

i32 PC_DEVICE_BACKEND::FindAspect(u32 width, u32 height) {
    i32 selected = 0;
    f32 ratio = (f32)width / (f32)height;
    f32 closest = 1000.0f;
    f32 distance;
    distance = ratio - 1.3333334f;
    distance = (f32)fabs((f64)distance);
    if (distance < closest) {
        closest = distance;
        selected = 0;
    }
    distance = ratio - 1.7777778f;
    distance = (f32)fabs((f64)distance);
    if (distance < closest) {
        closest = distance;
        selected = 1;
    }
    distance = ratio - 1.6f;
    distance = (f32)fabs((f64)distance);
    if (distance < closest) {
        closest = distance;
        selected = 2;
    }
    return selected;
}

void PC_DEVICE_BACKEND::ReleaseDefaultPoolResources() {
    if (back_buffer != NULL) {
        back_buffer->methods->release(back_buffer);
        back_buffer = NULL;
    }
    if (depth_surface != NULL) {
        depth_surface->methods->release(depth_surface);
        depth_surface = NULL;
    }
    if (quad_buffer != NULL) {
        quad_buffer->methods->release(quad_buffer);
        quad_buffer = NULL;
    }
    if (grid_vertex_buffer != NULL) {
        grid_vertex_buffer->methods->release(grid_vertex_buffer);
        grid_vertex_buffer = NULL;
    }
    if (grid_index_buffer != NULL) {
        grid_index_buffer->methods->release(grid_index_buffer);
        grid_index_buffer = NULL;
    }
    if (triangle_buffer != NULL) {
        triangle_buffer->methods->release(triangle_buffer);
        triangle_buffer = NULL;
    }
    for (i32 i = 0; i != 4; ++i) {
        if (target_surfaces[i] != NULL) {
            target_surfaces[i]->methods->release(target_surfaces[i]);
            target_surfaces[i] = NULL;
        }
        PCGraphicsDestroyRenderTexture(textures[i]);
        textures[i] = NULL;
        target_cache[i] = NULL;
    }
}

void PC_DEVICE_BACKEND::Shutdown() {
    if (modes != NULL) {
        delete modes;
        modes = NULL;
    }
    ReleaseDefaultPoolResources();
    if (position_uv_declaration != NULL) {
        position_uv_declaration->methods->release(position_uv_declaration);
        position_uv_declaration = NULL;
    }
    if (screen_colour_uv_declaration != NULL) {
        screen_colour_uv_declaration->methods->release(screen_colour_uv_declaration);
        screen_colour_uv_declaration = NULL;
    }
    if (device != NULL) {
        device->methods->release(device);
        device = NULL;
    }
    if (factory != NULL) {
        factory->methods->release(factory);
        factory = NULL;
    }
}

PC_DEVICE_BACKEND::~PC_DEVICE_BACKEND() {
    Shutdown();
}

// noinline: the original calls EndScene out of line from PCDeviceEndScene; VC8 would inline it there.
__declspec(noinline) HRESULT PC_DEVICE_BACKEND::EndScene() {
    PC_D3D9_DEVICE *view = device;
    return view->methods->end_scene(view);
}

// noinline: the original calls PCDeviceClear out of line from PC_DEVICE_BACKEND::CreateOrResetDevice; VC8 would
// inline it there.
extern "C" __declspec(noinline) void PCDeviceClear(u32 count, const RECT *rectangles, u32 flags, u32 colour, f32 depth, u32 stencil) {
    PC_D3D9_DEVICE *device = PCDeviceBackend.device;
    PCDeviceLastResult = device->methods->clear(device, count, rectangles, flags, colour, depth, stencil);
}

// noinline: the original calls PCDeviceTestCooperativeLevel out of line from PC_DEVICE_BACKEND::CreateOrResetDevice;
// VC8 would inline it there.
extern "C" __declspec(noinline) HRESULT PCDeviceTestCooperativeLevel() {
    PC_D3D9_DEVICE *device = PCDeviceBackend.device;
    return device->methods->test_cooperative_level(device);
}

PC_VIEWPORT *PC_DEVICE_BACKEND::GetRequestedViewport() {
    return &requested_viewport;
}

extern "C" i32 PCDeviceGetHardwareVertexProcessing(void) {
    return PCDeviceHardwareVertexProcessingPC;
}

extern "C" i32 PCDeviceIsAbsent() {
    return PCDeviceBackend.device == NULL;
}

extern "C" PC_D3D9_DEVICE *PCDeviceGetDevice() {
    return PCDeviceBackend.device;
}

extern "C" GRAPHICS_FACTORY *PCDeviceGetFactory(void) {
    return PCDeviceBackend.factory;
}

extern "C" void fun_006e4590(void) {
}

extern "C" {
    static u32 PCGraphicsMultisampleTypes[5] = {0, 2, 4, 8, 16};
    u8 PCDeviceStateCacheEnabled = 1;
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
}

u32 PC_DEVICE_BACKEND::GetMultisampleType() {
    return PCGraphicsMultisampleTypes[multisample_index];
}
struct PC_TEXTURE_CREATE_DEVICE;

struct PC_TEXTURE_CREATE_METHODS {     // size 0x78
    void *reserved_00[23];             // 0x00
    HRESULT(__stdcall *create_texture) // 0x5c
    (PC_TEXTURE_CREATE_DEVICE *, u32, u32, u32, u32, i32, i32, PC_TEXTURE_RESOURCE **, HANDLE *);
    void *reserved_60[5];                    // 0x60
    HRESULT(__stdcall *create_depth_surface) // 0x74
    (PC_TEXTURE_CREATE_DEVICE *, u32, u32, i32, i32, u32, BOOL, PC_TEXTURE_RESOURCE **, HANDLE *);
};

struct PC_TEXTURE_CREATE_DEVICE {             // size 0x4
    const PC_TEXTURE_CREATE_METHODS *methods; // 0x0
};

extern "C" void PCDeviceCreateDepthSurface(u32 width, u32 height, i32 format, i32 multisample, u32 quality, BOOL discard,
                                           PC_TEXTURE_RESOURCE **surface, HANDLE *shared_handle) {
    PC_TEXTURE_CREATE_DEVICE *device = reinterpret_cast<PC_TEXTURE_CREATE_DEVICE *>(PCDeviceBackend.device);
    PCDeviceLastResult =
        device->methods->create_depth_surface(device, width, height, format, multisample, quality, discard, surface, shared_handle);
}

extern "C" void PCDeviceCreateTexture(u32 width, u32 height, u32 levels, u32 usage, i32 format, i32 pool, PC_TEXTURE_RESOURCE **texture) {
    PC_TEXTURE_CREATE_DEVICE *device = reinterpret_cast<PC_TEXTURE_CREATE_DEVICE *>(PCDeviceBackend.device);
    PCDeviceLastResult = device->methods->create_texture(device, width, height, levels, usage, format, pool, texture, NULL);
}

static i32 PCGraphicsModeCompare(const void *left_ptr, const void *right_ptr) {
    const GRAPHICS_DISPLAY_MODE *left = static_cast<const GRAPHICS_DISPLAY_MODE *>(left_ptr);
    const GRAPHICS_DISPLAY_MODE *right = static_cast<const GRAPHICS_DISPLAY_MODE *>(right_ptr);
    if (left->width == 0 && left->height == 0 && left->refresh_rate == 0) {
        return 1;
    }
    if (right->width == 0 && right->height == 0 && right->refresh_rate == 0) {
        return -1;
    }
    if (left->width < right->width) {
        return -1;
    }
    if (left->width > right->width) {
        return 1;
    }
    if (left->height < right->height) {
        return -1;
    }
    if (left->height > right->height) {
        return 1;
    }
    if (left->refresh_rate < right->refresh_rate) {
        return -1;
    }
    return left->refresh_rate > right->refresh_rate;
}

extern "C" void fun_006e2160(void) {
}

u32 PC_DEVICE_BACKEND::FindMode(u32 width, u32 height, u32 refresh) {
    u32 closest_width = 10000;
    u32 closest_height = 10000;
    u32 closest_refresh = 10000;
    u32 selected = 0;
    for (u32 i = 0; i != mode_count; ++i) {
        if (abs((i32)(modes[i].width - width)) <= abs((i32)(closest_width - width))) {
            selected = i;
            closest_width = modes[i].width;
        }
    }
    for (u32 i = 0; i != mode_count; ++i) {
        if (modes[i].width == closest_width && abs((i32)(modes[i].height - height)) <= abs((i32)(closest_height - height))) {
            selected = i;
            closest_height = modes[i].height;
        }
    }
    if (refresh != 0) {
        for (u32 i = 0; i != mode_count; ++i) {
            if (modes[i].width == closest_width && modes[i].height == closest_height &&
                abs((i32)(modes[i].refresh_rate - refresh)) <= abs((i32)(closest_refresh - refresh))) {
                selected = i;
                closest_refresh = modes[i].refresh_rate;
            }
        }
    }
    if (requested_windowed != 0) {
        if (modes[selected].width != width) {
            modes[selected].width = width;
        }
        if (modes[selected].height != height) {
            modes[selected].height = height;
        }
    }
    return selected;
}

extern "C" {
    void (*PCGraphicsDeviceLostCallback)(GRAPHICS_PRESENT_PARAMETERS *) = NULL;
}

extern "C" {
    i32 PS2_VCNTR_X = 320;
    i32 PS2_VCNTR_Y = 240;
    HWND PCInputInstance = 0;
}

void PC_DEVICE_BACKEND::CreateOrResetDevice(i32 reposition_window) {
    NudxFw_D3DBeginCriticalSection();
    saved_windowed = parameters.windowed == 1;
    saved_mode = active_mode;
    saved_feature = active_feature;
    saved_multisample = multisample_index;
    saved_target_count = target_count;
    saved_aspect = PcWindowOptions.aspect_mode;
    bool rebuild_resources = true;
    factory->methods->get_device_caps(factory, 0, 1, &caps);
    parameters.window = PCInputInstance;
    parameters.windowed = requested_windowed;
    parameters.enable_depth = 1;
    parameters.flags = 2;
    parameters.swap_effect = 1;
    parameters.depth_format = 75;
    if (requested_feature != 0 && (caps.caps2 & 1) == 0) {
        requested_feature = 0;
    }
    active_feature = requested_feature;
    parameters.count = 2;
    parameters.interval = 1;
    if (parameters.windowed != 0 && reposition_window != 0) {
        i32 x = PcWindowOptions.window_x;
        i32 width = PcWindowOptions.client_width;
        i32 height = PcWindowOptions.client_height;
        i32 y = PcWindowOptions.window_y;
        if (x < 0) {
            x = (PCDeviceBackend.desktop_mode.width - width) >> 1;
        }
        if (y < 0) {
            y = (PCDeviceBackend.desktop_mode.height - height) >> 1;
        }
        MoveWindow(PCInputInstance, x, y, width, height, TRUE);
    }
    if (PCDeviceBackend.factory->methods->check_device_type(PCDeviceBackend.factory, 0, 1, modes[requested_mode].format, 21,
                                                            parameters.windowed) < 0) {
        parameters.format = modes[requested_mode].format;
    } else {
        parameters.format = 21;
    }
    parameters.width = modes[requested_mode].width;
    parameters.height = modes[requested_mode].height;
    parameters.refresh_rate = modes[requested_mode].refresh_rate;
    if (parameters.windowed != 0) {
        parameters.refresh_rate = 0;
    }
    creation_flags = 0;
    if (force_software == 0) {
        creation_flags = D3DCREATE_PUREDEVICE;
    }
    if (multithreaded != 0) {
        creation_flags |= D3DCREATE_MULTITHREADED;
    }
    if (force_software == 0) {
        if (caps.vertex_shader_version >= 0xfffe0101) {
            creation_flags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
        } else if ((caps.device_caps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0) {
            creation_flags |= D3DCREATE_MIXED_VERTEXPROCESSING;
        } else {
            creation_flags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }
    } else {
        creation_flags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }
    device_type = 1;
    adapter = 0;
    factory->methods->get_adapter_identifier(factory, 0, 0, &identifier);
    PCGraphicsPrepareVendor();
    if (multisample_tested == 0) {
        for (i32 i = 0; i != 5; ++i) {
            u32 colour_quality = 0;
            u32 depth_quality = 0;
            bool supported = true;
            if (factory->methods->check_multisample(factory, adapter, device_type, parameters.format, parameters.windowed,
                                                    PCGraphicsMultisampleTypes[i], &colour_quality) < 0) {
                supported = false;
            } else if (factory->methods->check_multisample(factory, adapter, device_type, parameters.depth_format, parameters.windowed,
                                                           PCGraphicsMultisampleTypes[i], &depth_quality) < 0) {
                supported = false;
            }
            u32 maximum = colour_quality > depth_quality ? colour_quality : depth_quality;
            quality[i] = maximum != 0 && supported != 0 ? maximum - 1 : -1;
        }
        quality[0] = 0;
        render_target_flags = 0;
        multisample_tested = 1;
    }
    if (multisample_tested != 0 && quality[requested_multisample] != -1) {
        multisample_index = requested_multisample;
    } else if (multisample_tested == 0 || quality[multisample_index] == -1) {
        multisample_index = 0;
    }
    parameters.multisample_type = PCGraphicsMultisampleTypes[multisample_index];
    parameters.multisample_quality = quality[multisample_index] < 0 ? 0 : quality[multisample_index];
    if (device == NULL) {
        SAGA_SKIN_CALL(skin_trace_invalidate, 0);
        PCDeviceLastResult = PCDeviceBackend.factory->methods->create_device(PCDeviceBackend.factory, adapter, device_type,
                                                                             parameters.window, creation_flags, &parameters, &device);
        i32 retries = 15;
        while (PCDeviceLastResult < 0 && retries != 0) {
            Sleep(500);
            --retries;
            PCDeviceLastResult = PCDeviceBackend.factory->methods->create_device(PCDeviceBackend.factory, adapter, device_type,
                                                                                 parameters.window, creation_flags, &parameters, &device);
        }
        if (device == NULL) {
            parameters.width = 640;
            parameters.height = 480;
            parameters.refresh_rate = 60;
            PCDeviceLastResult =
                factory->methods->create_device(factory, adapter, device_type, parameters.window, creation_flags, &parameters, &device);
            if (device == NULL) {
                switch (PCDeviceLastResult) {
                    case static_cast<HRESULT>(0x8876017c):
                        PCGraphicsFatalDiagnostic(&PCNativeWarningDebugState, "Out of Video Memory. Application will now exit");
                        break;
                    case static_cast<HRESULT>(0x88760868):
                        PCGraphicsFatalDiagnostic(&PCNativeWarningDebugState, "Lost d3d device. Application will now exit");
                        break;
                    case static_cast<HRESULT>(0x8876086a):
                        PCGraphicsFatalDiagnostic(&PCNativeWarningDebugState, "d3d device unavailable. Application will now exit");
                        break;
                    default:
                        PCGraphicsFatalDiagnostic(&PCNativeWarningDebugState,
                                                  "Failed to create d3d device. Application will now exit.\nError = 0x%x",
                                                  PCDeviceLastResult);
                        break;
                }
            }
        }
        PS2_REZ_W = parameters.width;
        PS2_REZ_H = parameters.height;
        PS2_VCNTR_X = PS2_REZ_W / 2;
        PS2_VCNTR_Y = PS2_REZ_H / 2;
    } else if (SoundVoiceEnabledGate != 0) {
        HRESULT cooperative = PCDeviceTestCooperativeLevel();
        PC_D3D9_DEVICE *view;
        if (cooperative < 0) {
            switch (cooperative) {
                case static_cast<HRESULT>(0x88760827):
                    SendMessageA(PCInputInstance, WM_CLOSE, 0, 0);
                    break;
                default:
                    SendMessageA(PCInputInstance, WM_CLOSE, 0, 0);
                    break;
                case static_cast<HRESULT>(0x88760869):
                    PCDeviceEvictManagedResources();
                    PcRendererShutdown(0);
                    SAGA_SKIN_CALL(skin_trace_reset_begin, (unsigned)device);
                    view = device;
                    if (view->methods->reset(view, &parameters) >= 0) {
                        SAGA_SKIN_CALL(skin_trace_reset_end, (unsigned)device);
                        PCGraphicsRecreateResources(0);
                        rebuild_resources = false;
                    } else {
                        SAGA_SKIN_CALL(skin_trace_reset_end, (unsigned)device);
                    }
                    break;
                case static_cast<HRESULT>(0x88760868):
                    if (PCGraphicsDeviceLostCallback != NULL) {
                        PCGraphicsDeviceLostCallback(&parameters);
                    }
                    break;
            }
        } else {
            PCDeviceEvictManagedResources();
            PcRendererShutdown(1);
            SAGA_SKIN_CALL(skin_trace_reset_begin, (unsigned)device);
            view = device;
            view->methods->reset(view, &parameters);
            SAGA_SKIN_CALL(skin_trace_reset_end, (unsigned)device);
            PS2_REZ_W = parameters.width;
            PS2_REZ_H = parameters.height;
            PS2_VCNTR_X = PS2_REZ_W / 2;
            PS2_VCNTR_Y = PS2_REZ_H / 2;
            PCGraphicsRecreateResources(1);
            rebuild_resources = false;
        }
    }
    multisample_index = 0;
    for (i32 i = 0; i != 5; ++i) {
        if (PCGraphicsMultisampleTypes[i] == static_cast<u32>(parameters.multisample_type)) {
            multisample_index = i;
            break;
        }
    }
    active_aspect_mode = FindAspect(parameters.width, parameters.height);
    active_aspect = (f32)parameters.width / (f32)parameters.height;
    nuapi.video_aspect = PcWindowOptions.aspect_mode == PC_ASPECT_AUTO ? active_aspect_mode : PcWindowOptions.aspect_mode;
    u32 refresh = parameters.windowed != 0 ? modes[requested_mode].refresh_rate : parameters.refresh_rate;
    active_mode = FindMode(parameters.width, parameters.height, refresh);
    PC_D3D9_DEVICE *view = device;
    view->methods->get_device_caps(view, &caps);
    if (rebuild_resources) {
        BuildDefaultPoolResources();
    }
    ResetTargets();
    PCDeviceClear(0, NULL, 1, 0, 1.0f, 0);
    view = device;
    present_result = view->methods->present(view, NULL, NULL, NULL, NULL);
    for (i32 i = 0; i != 8; ++i) {
        PCDeviceSetSamplerState(i, D3DSAMP_MAGFILTER, 2);
        PCDeviceSetSamplerState(i, D3DSAMP_MINFILTER, 2);
        PCDeviceSetSamplerState(i, D3DSAMP_MIPFILTER, 2);
    }
    if (PCDeviceHardwareVertexProcessingPC == 0) {
        PCDeviceSetSoftwareVertexProcessing(0);
    }
    PCDeviceSetRenderState(D3DRS_MULTISAMPLEANTIALIAS, multisample_index != 0);
    for (i32 i = 0; i != 4; ++i) {
        target_cache[i] = NULL;
    }
    PcWindowOptions.SetDeviceWidth(PS2_REZ_W);
    PcWindowOptions.SetDeviceHeight(PS2_REZ_H);
    if (parameters.windowed == 0) {
        PcWindowOptions.SetRefresh(parameters.refresh_rate);
    }
    PcWindowOptions.SetMultisample(multisample_index);
    PcWindowOptions.Save();
    full_viewport.x = 0;
    full_viewport.y = 0;
    full_viewport.width = PS2_REZ_W;
    full_viewport.height = PS2_REZ_H;
    full_viewport.min_z = 0.0f;
    full_viewport.max_z = 1.0f;
    requested_windowed = parameters.windowed == 1;
    requested_mode = active_mode;
    requested_feature = active_feature;
    requested_multisample = multisample_index;
    requested_target_count = target_count;
    NudxFw_D3DEndCriticalSection();
}

// noinline: the original calls PCDeviceEvictManagedResources out of line from
// PC_DEVICE_BACKEND::CreateOrResetDevice; VC8 would inline it there.
__declspec(noinline) void PCDeviceEvictManagedResources() {
    PC_D3D9_DEVICE *view = PCDeviceBackend.device;
    PCDeviceLastResult = view->methods->evict_managed_resources(view);
}

// noinline: the original calls PCDeviceSetSoftwareVertexProcessing out of line from
// PC_DEVICE_BACKEND::CreateOrResetDevice; VC8 would inline it there.
__declspec(noinline) void PCDeviceSetSoftwareVertexProcessing(BOOL enabled) {
    PC_D3D9_DEVICE *view = PCDeviceBackend.device;
    PCDeviceLastResult = view->methods->set_software_vertex_processing(view, enabled);
}

void PC_DEVICE_BACKEND::Present(const RECT *source, const RECT *destination, HWND window, const RGNDATA *region) {
    PC_D3D9_DEVICE *view = device;
    present_result = view->methods->present(view, source, destination, window, region);
    if (present_result < 0) {
        if (present_result == static_cast<HRESULT>(0x88760868)) {
            if (PCDeviceTestCooperativeLevel() == static_cast<HRESULT>(0x88760869)) {
                CreateOrResetDevice(1);
            }
        } else if (present_result != static_cast<HRESULT>(0x88760827)) {
            /* Closes the window through separate branches for an internal driver error and the rest. */
            SendMessageA(PCInputInstance, WM_CLOSE, 0, 0);
        } else {
            SendMessageA(PCInputInstance, WM_CLOSE, 0, 0);
        }
    }
    for (i32 i = 0; i != 4; ++i) {
        target_cache[i] = reinterpret_cast<PC_TEXTURE_RESOURCE *>(-1);
    }
}

// noinline: the original calls BeginScene out of line from PCDeviceBeginScene; VC8 would inline it there.
__declspec(noinline) void PC_DEVICE_BACKEND::BeginScene() {
    PCGraphicsFrameScratchCount.reset_word = 0;
    PCGraphicsBoundShader = -1;
    PCGraphicsFrameState = -1;
    PC_D3D9_DEVICE *view = device;
    view->methods->begin_scene(view);
    BeginDefaultTargets();
}

static PC_VERTEX_ELEMENT PCGraphicsPositionUvDeclaration[3] = {{0, 0, 2, 0, 0, 0}, {0, 12, 1, 0, 5, 0}, {255, 0, 17, 0, 0, 0}};
static PC_VERTEX_ELEMENT PCGraphicsScreenUvDeclaration[3] = {{0, 0, 1, 0, 9, 0}, {0, 8, 1, 0, 5, 0}, {255, 0, 17, 0, 0, 0}};
static PC_VERTEX_ELEMENT PCGraphicsScreenColourUvDeclaration[4] = {
    {0, 0, 3, 0, 9, 0}, {0, 16, 4, 0, 10, 0}, {0, 20, 1, 0, 5, 0}, {255, 0, 17, 0, 0, 0}};

void PC_DEVICE_BACKEND::InitializeAdapter() {
    u32 total = PCDeviceBackend.factory->methods->get_adapter_mode_count(PCDeviceBackend.factory, 0, 22);
    GRAPHICS_DISPLAY_MODE *all_modes = new GRAPHICS_DISPLAY_MODE[total];
    u32 minimum_width = 640;
    u32 minimum_height = 480;
    if (PcWindowOptions.bloommethod == 2 || PcWindowOptions.shadowmethod == 2) {
        minimum_width = 800;
        minimum_height = 600;
    }
    mode_count = 0;
    for (u32 i = 0; i != total; ++i) {
        factory->methods->enum_adapter_modes(factory, 0, 22, i, &all_modes[i]);
        if (all_modes[i].width >= minimum_width && all_modes[i].height >= minimum_height && all_modes[i].refresh_rate >= 40 &&
            all_modes[i].refresh_rate <= 85) {
            ++mode_count;
        } else {
            memset(&all_modes[i], 0, sizeof(GRAPHICS_DISPLAY_MODE));
        }
    }
    qsort(all_modes, total, sizeof(GRAPHICS_DISPLAY_MODE), PCGraphicsModeCompare);
    modes = new GRAPHICS_DISPLAY_MODE[mode_count];
    memcpy(modes, all_modes, mode_count * sizeof(GRAPHICS_DISPLAY_MODE));
    delete all_modes;
    if (PcWindowOptions.usehires != 0 && PcWindowOptions.multisample != 6) {
        PcWindowOptions.SetMultisample(0);
    }
    u32 selected = FindMode(PcWindowOptions.device_width, PcWindowOptions.device_height, PcWindowOptions.refresh_rate);
    requested_mode = selected < mode_count ? selected : active_mode;
    requested_multisample = PcWindowOptions.multisample;
    CreateOrResetDevice(1);
    PCDeviceSetRenderState(D3DRS_ZENABLE, 1);
    PCDeviceSetRenderState(D3DRS_LIGHTING, 0);
    PCDeviceCreateVertexDeclaration(PCGraphicsPositionUvDeclaration, &position_uv_declaration);
    PCDeviceCreateVertexDeclaration(PCGraphicsScreenUvDeclaration, &screen_uv_declaration);
    PCDeviceCreateVertexDeclaration(PCGraphicsScreenColourUvDeclaration, &screen_colour_uv_declaration);
}

void PC_DEVICE_BACKEND::SetRenderTarget(i32 index, PC_TEXTURE_RESOURCE *surface) {
    u16 limit = multisample_index == 0 && PCDeviceHardwareVertexProcessingPC >= 2 ? (u16)caps.simultaneous_render_targets : 1;
    if (index <= limit - 1 && target_cache[index] != surface) {
        target_cache[index] = surface;
        PC_D3D9_DEVICE *view = device;
        view->methods->set_render_target(view, index, surface);
    }
}

/* Captures the current surface as a BMP image in a new NuAlloc block and returns its size. */
u32 PC_DEVICE_BACKEND::CaptureScreenshot(u32 width, u32 height, void **data) {
    PC_TEXTURE_RESOURCE *target;
    PC_TEXTURE_RESOURCE *copy;
    ID3DXBuffer *buffer;
    u32 size = 0;
    if (width == 0) {
        width = parameters.width;
    }
    if (height == 0) {
        f32 aspect;
        switch (PcWindowOptions.aspect_mode) {
            case PC_ASPECT_4_3:
                aspect = 1.3333334f;
                break;
            case PC_ASPECT_16_9:
                aspect = 1.7777778f;
                break;
            case PC_ASPECT_16_10:
                aspect = 1.6f;
                break;
            case PC_ASPECT_AUTO:
                aspect = active_aspect;
                break;
            default:
                aspect = 1.3333334f;
                break;
        }
        height = (u32)((f32)width / aspect);
    }
    target = NULL;
    copy = NULL;
    // Typing this slot in the header perturbs unrelated C units (episodeIII BoulderSpawn), so it is typed here.
    typedef HRESULT(__stdcall * CREATE_RENDER_TARGET)(PC_D3D9_DEVICE *, u32, u32, i32, i32, u32, BOOL, PC_TEXTURE_RESOURCE **, HANDLE *);
    if (SUCCEEDED(reinterpret_cast<CREATE_RENDER_TARGET>(device->methods->create_render_target)(device, width, height, 0x15, 0, 0, FALSE,
                                                                                                &target, NULL))) {
        if (target != NULL) {
            if (SUCCEEDED(device->methods->create_offscreen_plain_surface(device, width, height, 0x15, 2, &copy, NULL)) && copy != NULL &&
                SUCCEEDED(device->methods->stretch_rect(device, current_surface, NULL, target, NULL, 2)) &&
                SUCCEEDED(device->methods->get_render_target_data(device, target, copy))) {
                buffer = NULL;
                if (SUCCEEDED(
                        D3DXSaveSurfaceToFileInMemory(&buffer, D3DXIFF_BMP, reinterpret_cast<IDirect3DSurface9 *>(copy), NULL, NULL))) {
                    *data = NuAlloc(buffer->methods->get_buffer_size(buffer), ".\\d3dCore.cpp", 0x989);
                    PCByteCopy(*data, buffer->methods->get_buffer_pointer(buffer), buffer->methods->get_buffer_size(buffer));
                    size = buffer->methods->get_buffer_size(buffer);
                }
            }
        }
    }
    if (target != NULL) {
        target->methods->release(target);
        target = NULL;
    }
    if (copy != NULL) {
        copy->methods->release(copy);
        copy = NULL;
    }
    if (buffer != NULL) {
        buffer->methods->release(buffer);
    }
    return size;
}

u8 PC_WINDOW_OPTIONS::IsBloomEnabled() {
    return bloomenabled;
}

u8 PC_WINDOW_OPTIONS::AreShadowsEnabled() {
    return shadowsenabled;
}

u8 PC_WINDOW_OPTIONS::IsSpeedBlurEnabled() {
    return speedblurenabled;
}

u8 PC_WINDOW_OPTIONS::IsHiresEnabled() {
    return usehires;
}

u8 PC_WINDOW_OPTIONS::IsHiresPending() {
    return usehirespending;
}

u8 PC_WINDOW_OPTIONS::fun_00509080() {
    return option459;
}

GRAPHICS_DISPLAY_MODE *PC_DEVICE_BACKEND::GetDisplayMode(i32 index) {
    return &modes[index];
}

i32 PC_DEVICE_BACKEND::GetMultisampleIndex() {
    return multisample_index;
}

u8 PC_DEVICE_BACKEND::SupportsMultisample(i32 index) {
    return multisample_tested != 0 && quality[index] != -1;
}

u8 PC_DEVICE_BACKEND::SupportsAnyMultisample() {
    for (i32 index = 0; index != 5; index++) {
        if (index != 0 && SupportsMultisample(index)) {
            return 1;
        }
    }
    return 0;
}

u32 PC_DEVICE_BACKEND::GetRequestedMode() {
    return requested_mode;
}

i32 PC_DEVICE_BACKEND::GetRequestedMultisample() {
    return requested_multisample;
}

void PC_DEVICE_BACKEND::SetRequestedMultisample(i32 multisample) {
    requested_multisample = multisample;
}

void PC_DEVICE_BACKEND::ApplyAspectMode(i32 mode) {
    PcWindowOptions.SetAspectMode(mode);
    PcWindowOptions.Save();
    if (PcWindowOptions.aspect_mode == PC_ASPECT_AUTO) {
        MenuAppliedAspect = active_aspect_mode;
    } else {
        MenuAppliedAspect = PcWindowOptions.aspect_mode;
    }
}

u8 PC_DEVICE_BACKEND::HasRequestedModeChanged() {
    return requested_windowed != (parameters.windowed == 1) || requested_mode != active_mode || requested_feature != active_feature ||
           requested_multisample != multisample_index || requested_target_count != target_count;
}

void PC_DEVICE_BACKEND::ResetViewport() {
    requested_viewport.x = 0;
    requested_viewport.y = 0;
    requested_viewport.width = PS2_REZ_W;
    requested_viewport.height = PS2_REZ_H;
    requested_viewport.min_z = 0.0f;
    requested_viewport.max_z = 1.0f;
    NuVpSetZRange(0.0f, 1.0f);
    f32 y = (f32)((f32)requested_viewport.y * 16.0);
    f32 x = (f32)((f32)requested_viewport.x * 16.0);
    NuVpSetPosition(x, y);
    NuVpSetSize2(640.0f, 224.0f);
    NuVpUpdate();
    requested_viewport.x = 0;
    requested_viewport.y = 0;
    requested_viewport.width = PS2_REZ_W;
    requested_viewport.height = PS2_REZ_H;
    requested_viewport.min_z = 0.0f;
    requested_viewport.max_z = 1.0f;
    NuVpSubmit(&requested_viewport);
}

void PC_DEVICE_BACKEND::ResetTargets() {
    SetRenderTarget(0, back_buffer);
    current_surface = back_buffer;
    for (i32 i = 1; i != 4; ++i) {
        u16 limit = multisample_index == 0 && PCDeviceHardwareVertexProcessingPC >= 2 ? (u16)caps.simultaneous_render_targets : 1;
        if (i <= limit - 1 && target_cache[i] != NULL) {
            target_cache[i] = NULL;
            PC_D3D9_DEVICE *view = device;
            view->methods->set_render_target(view, i, NULL);
        }
    }
    PC_D3D9_DEVICE *view = device;
    view->methods->set_depth_stencil_surface(view, depth_surface);
    PS2_REZ_W = parameters.width;
    PS2_REZ_H = parameters.height;
    PS2_VCNTR_X = PS2_REZ_W / 2;
    PS2_VCNTR_Y = PS2_REZ_H / 2;
    ResetViewport();
}

void PC_DEVICE_BACKEND::ConfigureCurrentTargets() {
    PC_D3D9_DEVICE *view;
    if (multisample_index != 0) {
        SetRenderTarget(0, back_buffer);
        current_surface = back_buffer;
        view = device;
        view->methods->set_depth_stencil_surface(view, depth_surface);
    } else {
        for (i32 i = 0; i != 4; ++i) {
            if (i != 0) {
                if (i > 0) {
                    if (i <= 2 && ((PcWindowOptions.bloomenabled != 0 && PcWindowOptions.bloommethod == 2) ||
                                   (PcWindowOptions.shadowsenabled != 0 && PcWindowOptions.shadowmethod == 2))) {
                        SetRenderTarget(i, target_surfaces[i]);
                    } else {
                        SetRenderTarget(i, NULL);
                    }
                } else {
                    SetRenderTarget(i, NULL);
                }
            } else if (target_surfaces[0] != NULL) {
                SetRenderTarget(0, target_surfaces[0]);
                current_surface = target_surfaces[0];
            } else {
                SetRenderTarget(0, back_buffer);
                current_surface = back_buffer;
            }
        }
        view = device;
        view->methods->set_depth_stencil_surface(view, depth_surface);
    }
    PS2_REZ_W = parameters.width;
    PS2_REZ_H = parameters.height;
    PS2_VCNTR_X = PS2_REZ_W / 2;
    PS2_VCNTR_Y = PS2_REZ_H / 2;
    ResetViewport();
}

void PC_DEVICE_BACKEND::BeginDefaultTargets() {
    PC_D3D9_DEVICE *view;
    if (multisample_index != 0) {
        SetRenderTarget(0, back_buffer);
        view = device;
        view->methods->set_depth_stencil_surface(view, depth_surface);
        view = device;
        view->methods->clear(view, 0, NULL, 7, 0, 1.0f, 0);
        ConfigureCurrentTargets();
        return;
    }
    SetRenderTarget(0, back_buffer);
    view = device;
    view->methods->set_depth_stencil_surface(view, depth_surface);
    view = device;
    if (depth_surface != NULL) {
        view->methods->clear(view, 0, NULL, 7, 0, 1.0f, 0xc0);
    } else {
        view->methods->clear(view, 0, NULL, 1, 0, 1.0f, 0);
    }
    for (i32 i = 0; i != 4; ++i) {
        switch (i) {
            case 0:
                if (target_surfaces[0] != NULL) {
                    SetRenderTarget(0, target_surfaces[0]);
                } else {
                    SetRenderTarget(0, back_buffer);
                }
                view = device;
                view->methods->set_depth_stencil_surface(view, depth_surface);
                view = device;
                view->methods->clear(view, 0, NULL, 1, 0, 1.0f, 0);
                break;
            case 1:
                if (target_surfaces[1] != NULL) {
                    SetRenderTarget(1, NULL);
                    SetRenderTarget(0, target_surfaces[1]);
                    view = device;
                    view->methods->clear(view, 0, NULL, 1, 0xff000000, 1.0f, 0);
                }
                break;
            case 2:
                if (target_surfaces[2] != NULL) {
                    SetRenderTarget(2, NULL);
                    SetRenderTarget(0, target_surfaces[2]);
                    view = device;
                    view->methods->clear(view, 0, NULL, 1, 0, 1.0f, 0);
                }
                break;
        }
    }
    ConfigureCurrentTargets();
}

struct PC_GRAPHICS_CLIP_VERTEX { // size 0x14
    f32 x;                       // 0x00
    f32 y;                       // 0x04
    f32 z;                       // 0x08
    f32 u;                       // 0x0c
    f32 v;                       // 0x10
};

struct PC_GRAPHICS_SCREEN_VERTEX { // size 0x10
    f32 x;                         // 0x0
    f32 y;                         // 0x4
    f32 u;                         // 0x8
    f32 v;                         // 0xc
};

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

void PC_DEVICE_BACKEND::BuildDefaultPoolResources() {
    if (back_buffer != NULL) {
        back_buffer->methods->release(back_buffer);
        back_buffer = NULL;
    }
    if (depth_surface != NULL) {
        depth_surface->methods->release(depth_surface);
        depth_surface = NULL;
    }
    if (quad_buffer != NULL) {
        quad_buffer->methods->release(quad_buffer);
        quad_buffer = NULL;
    }
    if (grid_vertex_buffer != NULL) {
        grid_vertex_buffer->methods->release(grid_vertex_buffer);
        grid_vertex_buffer = NULL;
    }
    if (grid_index_buffer != NULL) {
        grid_index_buffer->methods->release(grid_index_buffer);
        grid_index_buffer = NULL;
    }
    if (triangle_buffer != NULL) {
        triangle_buffer->methods->release(triangle_buffer);
        triangle_buffer = NULL;
    }
    PC_D3D9_DEVICE *view = device;
    view->methods->get_back_buffer(view, 0, 0, 0, &back_buffer);
    view = device;
    view->methods->get_depth_stencil_surface(view, &depth_surface);
    ConfigureTargetKinds();
    for (i32 i = 0; i != 4; ++i) {
        if (target_surfaces[i] != NULL) {
            target_surfaces[i]->methods->release(target_surfaces[i]);
            target_surfaces[i] = NULL;
        }
        PCGraphicsDestroyRenderTexture(textures[i]);
        textures[i] = NULL;
        target_cache[i] = NULL;
    }
    target_count = requested_target_count;
    if (target_count > 4) {
        target_count = 4;
    }
    for (u32 i = 0; i != target_count; ++i) {
        switch (target_kinds[i]) {
            case -1:
            case 2:
                break;
            case 0:
            case 1:
                textures[i] = PCGraphicsCreateColourTarget(1, 1);
                break;
            case 3:
                textures[i] = PCGraphicsCreateColourTargetFormat(1, 1, 10);
                break;
        }
        if (textures[i] != NULL) {
            PC_TEXTURE_RESOURCE *texture = textures[i]->resource;
            texture->methods->get_surface_level(texture, 0, &target_surfaces[i]);
        }
    }
    PC_GRAPHICS_SCREEN_VERTEX triangle[3] = {{0.0f, 0.0f, 0.0f, 0.0f},
                                             {0.0f, (f32)((f32)parameters.height * 3.0), 0.0f, 3.0f},
                                             {(f32)((f32)parameters.width * 3.0), 0.0f, 3.0f, 0.0f}};
    PC_GRAPHICS_CLIP_VERTEX quad[4] = {
        {-1.0f, 1.0f, 0.0f, 0.0f, 0.0f}, {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 0.0f, 1.0f, 1.0f}};
    PCDeviceCreateVertexBuffer(sizeof(quad), 8, 0, 0, &quad_buffer);
    PCDeviceCreateVertexBuffer(sizeof(triangle), 8, 0, 0, &triangle_buffer);
    void *data = NULL;
    PCDeviceLockVertexBuffer(0, sizeof(quad), &data, 0, quad_buffer);
    PCByteCopy(data, quad, sizeof(quad));
    PCDeviceUnlockVertexBuffer(quad_buffer);
    PCDeviceLockVertexBuffer(0, sizeof(triangle), &data, 0, triangle_buffer);
    PCByteCopy(data, triangle, sizeof(triangle));
    PCDeviceUnlockVertexBuffer(triangle_buffer);
    grid_columns = 16;
    grid_rows = (u32)(((f64)PS2_REZ_H / PS2_REZ_W) * 16.0);
    grid_vertex_count = (i32)((grid_rows + 1) * 17);
    grid_triangle_count = (i32)(grid_rows * 32);
    PCDeviceCreateVertexBuffer((u32)grid_vertex_count * 20, 8, 0, 0, &grid_vertex_buffer);
    PCDeviceCreateIndexBuffer((u32)grid_triangle_count * 6, 8, 101, 0, &grid_index_buffer, NULL);
    PCDeviceLockVertexBuffer(0, (u32)grid_vertex_count * 20, &data, 0, grid_vertex_buffer);
    PC_GRAPHICS_CLIP_VERTEX *vertex = static_cast<PC_GRAPHICS_CLIP_VERTEX *>(data);
    for (u32 row = 0; row != grid_rows + 1; ++row) {
        for (u32 column = 0; column != (u32)grid_columns + 1; ++column) {
            f32 rounded_row = (f32)row;
            f32 rounded_column = (f32)column;
            vertex->x = rounded_column / (f32)(u32)grid_columns * 2.0 - 1.0;
            vertex->y = 1.0 - rounded_row / (f32)grid_rows * 2.0;
            vertex->z = 0.0f;
            vertex->u = rounded_column / (f32)(u32)grid_columns;
            vertex->v = rounded_row / (f32)grid_rows;
            ++vertex;
        }
    }
    PCDeviceUnlockVertexBuffer(grid_vertex_buffer);
    PCDeviceLockIndexBuffer(0, (u32)grid_triangle_count * 6, &data, 0, grid_index_buffer);
    u16 *index = static_cast<u16 *>(data);
    for (u32 row = 0; row != grid_rows; ++row) {
        for (u32 column = 0; column != (u32)grid_columns; ++column) {
            *index++ = (u16)(((u16)grid_columns + 1) * (u16)row + (u16)column);
            *index++ = (u16)(((u16)grid_columns + 1) * (u16)row + (u16)column + 1);
            *index++ = (u16)(((u16)grid_columns + 1) * (u16)(row + 1) + (u16)column);
            *index++ = (u16)(((u16)grid_columns + 1) * (u16)(row + 1) + (u16)column);
            *index++ = (u16)(((u16)grid_columns + 1) * (u16)row + (u16)column + 1);
            *index++ = (u16)(((u16)grid_columns + 1) * (u16)(row + 1) + (u16)column + 1);
        }
    }
    PCDeviceUnlockIndexBuffer(grid_index_buffer);
}

void PC_DEVICE_BACKEND::ConfigureTargetKinds() {
    PcWindowOptions.SetShadowMethod(PcWindowOptions.desiredshadowmethod);
    PcWindowOptions.SetBloomMethod(PcWindowOptions.desiredbloommethod);
    PcWindowOptions.SetSpeedBlurSupported(1);
    if (PcWindowOptions.usehires != 0) {
        PcWindowOptions.SetShadowMethod(2);
    } else {
        PcWindowOptions.SetShadowMethod(1);
    }
    if (PCDeviceHardwareVertexProcessingPC < 2) {
        if (PcWindowOptions.shadowsenabled != 0 && PcWindowOptions.shadowmethod > 1) {
            PcWindowOptions.SetShadowMethod(1);
        }
        if (PcWindowOptions.bloomenabled != 0 && PcWindowOptions.bloommethod > 1) {
            PcWindowOptions.SetBloomMethod(1);
        }
    }
    if (PCDeviceHardwareVertexProcessingPC <= 2 && PcWindowOptions.shadowsenabled != 0 && PcWindowOptions.shadowmethod > 1) {
        PcWindowOptions.SetShadowMethod(1);
    }
    if (multisample_index != 0) {
        if (PcWindowOptions.shadowsenabled != 0 && PcWindowOptions.shadowmethod > 1) {
            PcWindowOptions.SetShadowMethod(0);
        }
        if (PcWindowOptions.bloomenabled != 0 && PcWindowOptions.bloommethod > 1) {
            PcWindowOptions.SetBloomMethod(1);
        }
    }
    shader_flags = 1;
    render_target_flags = 1;
    if (PcWindowOptions.shadowmethod == 2) {
        shader_flags = 0x21;
        render_target_flags = 0x13;
    }
    if (PcWindowOptions.bloommethod != 0) {
        shader_flags |= 4;
    }
    if (PcWindowOptions.bloommethod == 2) {
        render_target_flags |= 2;
    }
    if (PcWindowOptions.speedblursupported != 0) {
        shader_flags |= 0x80;
    }
    i32 count = 0;
    if (multisample_index == 0) {
        target_kinds[count++] = 0;
    }
    if ((render_target_flags & 2) != 0) {
        target_kinds[count++] = 1;
    }
    if ((render_target_flags & 4) != 0) {
        target_kinds[count++] = 2;
    }
    if ((render_target_flags & 0x10) != 0) {
        target_kinds[count++] = 3;
    }
    requested_target_count = (u16)count;
    PcWindowOptions.Save();
}

extern "C" {
    i32 PCGraphicsGpuSelectionEnabled = 0;
    PC_GRAPHICS_GPU_RECORD *PCGraphicsSelectedGpuRecord = NULL;
    i32 MenuAppliedAspect = 0;
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

extern "C" HRESULT PCDeviceCreateVertexShader(const DWORD *tokens, PC_TEXTURE_RESOURCE **shader) {
    HRESULT result = PCDeviceBackend.device->methods->create_vertex_shader(PCDeviceBackend.device, tokens, shader);
    PCDeviceLastResult = result;
    return result;
}

extern "C" void fun_006e48d0(void) {
}

extern "C" HRESULT PCDeviceCreatePixelShader(const DWORD *tokens, PC_TEXTURE_RESOURCE **shader) {
    HRESULT result = PCDeviceBackend.device->methods->create_pixel_shader(PCDeviceBackend.device, tokens, shader);
    PCDeviceLastResult = result;
    return result;
}

void PC_DEVICE_BACKEND::RequestMode(u32 mode) {
    requested_mode = mode < mode_count ? mode : active_mode;
}

void PC_DEVICE_BACKEND::NextResolution() {
    u32 original = requested_mode;
    u32 index = original;
    u32 visited = 0;
    while (visited != mode_count && modes[index].width == modes[original].width && modes[index].height == modes[original].height) {
        ++index;
        if (index == mode_count) {
            index = 0;
        }
        ++visited;
    }
    u32 first = index;
    u32 selected = index;
    u32 reference = index;
    i32 closest = 1000;
    while (index < mode_count && modes[index].width == modes[first].width && modes[index].height == modes[first].height) {
        if (modes[index].refresh_rate == modes[original].refresh_rate) {
            requested_mode = index;
            return;
        }
        i32 difference = abs((i32)(modes[index].refresh_rate - modes[reference].refresh_rate));
        if (difference < closest) {
            closest = difference;
            selected = index;
            reference = index;
        }
        ++index;
    }
    requested_mode = selected;
}

void PC_DEVICE_BACKEND::PreviousResolution() {
    u32 original = requested_mode;
    i32 index = (i32)original;
    u32 visited = 0;
    while (visited < mode_count && modes[index].width == modes[original].width && modes[index].height == modes[original].height) {
        --index;
        if (index < 0) {
            index = (i32)mode_count - 1;
        }
        ++visited;
    }
    i32 first = index;
    i32 selected = index;
    i32 reference = index;
    i32 closest = 1000;
    while (index >= 0 && modes[index].width == modes[first].width && modes[index].height == modes[first].height) {
        if (modes[index].refresh_rate == modes[original].refresh_rate) {
            requested_mode = (u32)index;
            return;
        }
        i32 difference = abs((i32)(modes[index].refresh_rate - modes[reference].refresh_rate));
        if (difference < closest) {
            closest = difference;
            selected = index;
            reference = index;
        }
        --index;
    }
    requested_mode = (u32)selected;
}

void PC_DEVICE_BACKEND::NextRefresh() {
    u32 original = requested_mode;
    u32 index = original;
    for (u32 visited = 0; visited < mode_count; ++visited) {
        if (modes[index].width == modes[original].width && modes[index].height == modes[original].height &&
            modes[index].refresh_rate != modes[original].refresh_rate) {
            requested_mode = index;
            return;
        }
        ++index;
        if (index >= mode_count) {
            index = 0;
        }
    }
}

void PC_DEVICE_BACKEND::PreviousRefresh() {
    u32 original = requested_mode;
    i32 index = (i32)original;
    for (u32 visited = 0; visited < mode_count; ++visited) {
        if (modes[index].width == modes[original].width && modes[index].height == modes[original].height &&
            modes[index].refresh_rate != modes[original].refresh_rate) {
            requested_mode = (u32)index;
            return;
        }
        --index;
        if (index < 0) {
            index = (i32)mode_count - 1;
        }
    }
}

void PC_DEVICE_BACKEND::ResetRequestedMode() {
    requested_windowed = parameters.windowed == 1;
    requested_mode = active_mode;
    requested_feature = active_feature;
    requested_multisample = multisample_index;
}

void PC_DEVICE_BACKEND::SetDepthStencilSurface(PC_EFFECT_SURFACE *target) {
    if (target != NULL && target->surface != NULL) {
        PC_D3D9_DEVICE *view = device;
        view->methods->set_depth_stencil_surface(view, target->surface);
    } else if (multisample_index != 0) {
        /* Keeps a separate multisample branch that binds the same depth surface. */
        PC_D3D9_DEVICE *view = device;
        view->methods->set_depth_stencil_surface(view, depth_surface);
    } else {
        PC_D3D9_DEVICE *view = device;
        view->methods->set_depth_stencil_surface(view, depth_surface);
    }
}

void PC_DEVICE_BACKEND::ClearDepthStencilSurface() {
    PC_D3D9_DEVICE *view = device;
    view->methods->set_depth_stencil_surface(view, NULL);
}

void PC_DEVICE_BACKEND::RestoreDefaultPoolResources() {
    BuildDefaultPoolResources();
}

void PC_DEVICE_BACKEND::SetSurfaceRenderTarget(PC_EFFECT_SURFACE *target, u32 index) {
    if (target == NULL || target->surface == NULL) {
        if (index != 0) {
            SetRenderTarget((i32)index, NULL);
            return;
        }
        if (multisample_index != 0) {
            SetRenderTarget(0, back_buffer);
            PCDeviceSetRenderState(D3DRS_COLORWRITEENABLE, 0);
            current_surface = back_buffer;
        } else {
            SetRenderTarget(0, target_surfaces[0]);
            current_surface = target_surfaces[0];
        }
        PS2_REZ_W = parameters.width;
        PS2_REZ_H = parameters.height;
        PS2_VCNTR_X = PS2_REZ_W / 2;
        PS2_VCNTR_Y = PS2_REZ_H / 2;
        ResetViewport();
    } else {
        SetRenderTarget((i32)index, target->surface);
        if (index == 0) {
            PS2_REZ_W = target->width;
            PS2_REZ_H = target->height;
            PS2_VCNTR_X = PS2_REZ_W / 2;
            PS2_VCNTR_Y = PS2_REZ_H / 2;
            ResetViewport();
        }
    }
}

void PC_DEVICE_BACKEND::CopyBackBuffer(GRAPHICS_RENDER_TEXTURE *texture) {
    PC_EFFECT_SURFACE target;
    target.Attach(texture, 0, false);
    bool resize = PCGraphicsEffectTextureWidth(texture) != PS2_REZ_W || PCGraphicsEffectTextureHeight(texture) != PS2_REZ_H;
    PC_TEXTURE_RESOURCE *source = target_surfaces[0];
    if (multisample_index != 0 || source == NULL) {
        source = back_buffer;
    }
    PC_D3D9_DEVICE *view = device;
    view->methods->stretch_rect(view, source, NULL, target.surface, NULL, resize ? 2 : 1);
}

void PC_DEVICE_BACKEND::CopyCurrentSurface(GRAPHICS_RENDER_TEXTURE *texture) {
    PC_EFFECT_SURFACE target;
    target.Attach(texture, 0, false);
    bool resize = PCGraphicsEffectTextureWidth(texture) != PS2_REZ_W || PCGraphicsEffectTextureHeight(texture) != PS2_REZ_H;
    PC_D3D9_DEVICE *view = device;
    view->methods->stretch_rect(view, current_surface, NULL, target.surface, NULL, resize ? 2 : 1);
}

extern "C" void PCDeviceSetVertexShaderConstantsF(UINT start, const f32 *values, UINT count) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_vertex_shader_constant_f(PCDeviceBackend.device, start, values, count);
    SAGA_SKIN_CALL(skin_trace_upload, 0, start, count, values, PCDeviceLastResult);
}

extern "C" void PCDeviceSetPixelShaderConstantsF(UINT start, const f32 *values, UINT count) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_pixel_shader_constant_f(PCDeviceBackend.device, start, values, count);
    SAGA_SKIN_CALL(skin_trace_upload, 1, start, count, values, PCDeviceLastResult);
}

HRESULT PC_DEVICE_BACKEND::CopySurfaceTarget(PC_EFFECT_SURFACE *target, i32 filter, i32 unused) {
    PC_D3D9_DEVICE *view = device;
    return view->methods->stretch_rect(view, current_surface, NULL, target->surface, NULL, filter);
}

extern "C" HRESULT PCDeviceEndScene(void) {
    return PCDeviceBackend.EndScene();
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

// noinline: the original calls PCDeviceDrawPrimitive out of line from PC_DEVICE_BACKEND::DrawPostEffectQuad; VC8
// would inline it there.
extern "C" __declspec(noinline) void PCDeviceDrawPrimitive(i32 type, u32 start, u32 count) {
    ++PcFrameStats.primitive_calls;
    PCDeviceLastResult = PCDeviceBackend.device->methods->draw_primitive(PCDeviceBackend.device, type, start, count);
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

// noinline: the original calls PCDeviceDrawIndexedPrimitive out of line from
// PC_DEVICE_BACKEND::DrawPostEffectIndexed; VC8 would inline it there.
extern "C" __declspec(noinline) void PCDeviceDrawIndexedPrimitive(i32 type, i32 base_vertex, u32 minimum_vertex, u32 vertex_count,
                                                                  u32 start_index, u32 primitive_count) {
    ++PcFrameStats.indexed_primitive_calls;
    PCDeviceLastResult = PCDeviceBackend.device->methods->draw_indexed_primitive(PCDeviceBackend.device, type, base_vertex, minimum_vertex,
                                                                                 vertex_count, start_index, primitive_count);
}

extern "C" void PCDeviceSetStreamSourceFreq(u32 stream, u32 setting) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_stream_source_freq(PCDeviceBackend.device, stream, setting);
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

extern "C" void PCDeviceSetPixelShader(PC_TEXTURE_RESOURCE *shader) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_pixel_shader(PCDeviceBackend.device, shader);
    SAGA_SKIN_CALL(skin_trace_bind, 1, (unsigned)shader, PCDeviceLastResult);
}

void PC_DEVICE_BACKEND::SetColorWriteMask(u8 colour_enabled, u8 alpha_enabled, i32 target) {
    u32 disabled;
    if (target != 0 && target_count == 1) {
        return;
    }
    disabled = 0;
    if (colour_enabled == 0) {
        disabled = 7;
    }
    if (alpha_enabled == 0) {
        disabled |= 8u;
    }
    switch (target) {
        case 0:
            PCDeviceSetRenderState(D3DRS_COLORWRITEENABLE, disabled);
            break;
        case 1:
            PCDeviceSetRenderState(D3DRS_COLORWRITEENABLE1, disabled);
            break;
        case 3:
            PCDeviceSetRenderState(D3DRS_COLORWRITEENABLE2, disabled);
            break;
    }
}

/* Thin IDirect3DDevice9 getters (no caller); each records its result. */
extern "C" void PCDeviceColorFill(PC_TEXTURE_RESOURCE *surface, const RECT *rect, u32 colour) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->color_fill(PCDeviceBackend.device, surface, rect, colour);
}

extern "C" void PCDeviceCreateAdditionalSwapChain(void *parameters, void **swap_chain) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->create_additional_swap_chain(PCDeviceBackend.device, parameters, swap_chain);
}

extern "C" void PCDeviceCreateCubeTexture(u32 edge_length, u32 levels, u32 usage, i32 format, i32 pool, PC_TEXTURE_RESOURCE **texture,
                                          HANDLE *shared) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->create_cube_texture(PCDeviceBackend.device, edge_length, levels, usage, format,
                                                                              pool, texture, shared);
}

extern "C" void PCDeviceCreateOffscreenPlainSurface(u32 width, u32 height, i32 format, i32 pool, PC_TEXTURE_RESOURCE **surface,
                                                    HANDLE *shared) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->create_offscreen_plain_surface(PCDeviceBackend.device, width, height, format,
                                                                                         pool, surface, shared);
}

extern "C" void PCDeviceCreateQuery(i32 type, void **query) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->create_query(PCDeviceBackend.device, type, query);
}

extern "C" void PCDeviceCreateStateBlock(i32 type, void **state_block) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->create_state_block(PCDeviceBackend.device, type, state_block);
}

extern "C" void PCDeviceCreateVolumeTexture(u32 width, u32 height, u32 depth, u32 levels, u32 usage, i32 format, i32 pool,
                                            PC_TEXTURE_RESOURCE **texture) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->create_volume_texture(PCDeviceBackend.device, width, height, depth, levels, usage,
                                                                                format, pool, texture, NULL);
}

extern "C" void PCDeviceEndStateBlock(void **state_block) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->end_state_block(PCDeviceBackend.device, state_block);
}

extern "C" i32 PCDeviceGetAvailableTextureMem(void) {
    PCDeviceLastResult = static_cast<HRESULT>(PCDeviceBackend.device->methods->get_available_texture_mem(PCDeviceBackend.device));
    return 0;
}

extern "C" void PCDeviceGetClipPlane(u32 index, f32 *plane) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_clip_plane(PCDeviceBackend.device, index, plane);
}

extern "C" void PCDeviceGetClipStatus(void *status) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_clip_status(PCDeviceBackend.device, status);
}

extern "C" void PCDeviceGetCreationParameters(void *parameters) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_creation_parameters(PCDeviceBackend.device, parameters);
}

extern "C" void PCDeviceGetCurrentTexturePalette(u32 *palette) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_current_texture_palette(PCDeviceBackend.device, palette);
}

/* Whether the adapter accepts format for the resource type and usage in its
 * current display mode. */
extern "C" HRESULT PCDeviceCheckFormat(i32 format, i32 resource_type, u32 usage) {
    GRAPHICS_CAPS caps;
    GRAPHICS_DISPLAY_MODE mode;
    PCDeviceBackend.device->methods->get_device_caps(PCDeviceBackend.device, &caps);
    PCDeviceBackend.factory->methods->get_adapter_display_mode(PCDeviceBackend.factory, caps.adapter_ordinal, &mode);
    return PCDeviceBackend.factory->methods->check_device_format(PCDeviceBackend.factory, caps.adapter_ordinal, caps.device_type,
                                                                 mode.format, usage, resource_type, format);
}

extern "C" void PCDeviceGetDisplayMode(u32 swap_chain, void *mode) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_display_mode(PCDeviceBackend.device, swap_chain, mode);
}

extern "C" void PCDeviceGetFrontBufferData(u32 swap_chain, PC_TEXTURE_RESOURCE *surface) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_front_buffer_data(PCDeviceBackend.device, swap_chain, surface);
}

extern "C" void PCDeviceGetFVFPc(u32 *fvf) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_fvf(PCDeviceBackend.device, fvf);
}

extern "C" void PCDeviceGetIndices(PC_BUFFER_RESOURCE **indices) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_indices(PCDeviceBackend.device, indices);
}

extern "C" void PCDeviceGetLight(u32 index, void *light) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_light(PCDeviceBackend.device, index, light);
}

extern "C" void PCDeviceGetLightEnable(u32 index, BOOL *enabled) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_light_enable(PCDeviceBackend.device, index, enabled);
}

extern "C" void PCDeviceGetMaterial(void *material) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_material(PCDeviceBackend.device, material);
}

/* N-patches are not used: the mode always reads as 0. */
extern "C" f32 PCDeviceGetNPatchMode(void) {
    return 0.0f;
}

extern "C" i32 PCDeviceGetNumberOfSwapChains(void) {
    PCDeviceLastResult = static_cast<HRESULT>(PCDeviceBackend.device->methods->get_number_of_swap_chains(PCDeviceBackend.device));
    return 0;
}

extern "C" void PCDeviceGetPaletteEntries(u32 palette, PALETTEENTRY *entries) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_palette_entries(PCDeviceBackend.device, palette, entries);
}

extern "C" void PCDeviceGetPixelShader(PC_TEXTURE_RESOURCE **shader) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_pixel_shader(PCDeviceBackend.device, shader);
}

extern "C" void PCDeviceGetPixelShaderConstantBPc(u32 start, BOOL *data, u32 count) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_pixel_shader_constant_b(PCDeviceBackend.device, start, data, count);
}

extern "C" void PCDeviceGetPixelShaderConstantFPc(u32 start, f32 *data, u32 count) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_pixel_shader_constant_f(PCDeviceBackend.device, start, data, count);
}

extern "C" void PCDeviceGetPixelShaderConstantIPc(u32 start, i32 *data, u32 count) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_pixel_shader_constant_i(PCDeviceBackend.device, start, data, count);
}

extern "C" void PCDeviceGetRasterStatus(u32 swap_chain, void *status) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_raster_status(PCDeviceBackend.device, swap_chain, status);
}

extern "C" void PCDeviceGetRenderState(i32 state, u32 *value) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_render_state(PCDeviceBackend.device, state, value);
}

extern "C" void PCDeviceGetRenderTarget(u32 index, PC_TEXTURE_RESOURCE **target) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_render_target(PCDeviceBackend.device, index, target);
}

extern "C" void PCDeviceGetRenderTargetData(PC_TEXTURE_RESOURCE *target, PC_TEXTURE_RESOURCE *destination) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_render_target_data(PCDeviceBackend.device, target, destination);
}

extern "C" void PCDeviceGetSamplerState(u32 sampler, i32 type, u32 *value) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_sampler_state(PCDeviceBackend.device, sampler, type, value);
}

extern "C" void PCDeviceGetScissorRect(RECT *rect) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_scissor_rect(PCDeviceBackend.device, rect);
}

extern "C" i32 PCDeviceGetSoftwareVertexProcessing(void) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_software_vertex_processing(PCDeviceBackend.device);
    return 0;
}

extern "C" void PCDeviceGetStreamSource(u32 stream, PC_BUFFER_RESOURCE **buffer, u32 *offset, u32 *stride) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_stream_source(PCDeviceBackend.device, stream, buffer, offset, stride);
}

extern "C" void PCDeviceGetStreamSourceFreq(u32 stream, u32 *divider) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_stream_source_freq(PCDeviceBackend.device, stream, divider);
}

extern "C" void PCDeviceGetSwapChain(u32 index, void **swap_chain) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_swap_chain(PCDeviceBackend.device, index, swap_chain);
}

extern "C" void PCDeviceGetTexture(u32 stage, void **texture) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_texture(PCDeviceBackend.device, stage, texture);
}

extern "C" void PCDeviceGetTextureStageState(u32 stage, i32 type, u32 *value) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_texture_stage_state(PCDeviceBackend.device, stage, type, value);
}

extern "C" void PCDeviceGetTransform(i32 state, NUMTX *matrix) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_transform(PCDeviceBackend.device, state, matrix);
}

extern "C" void PCDeviceGetVertexDeclaration(PC_TEXTURE_RESOURCE **declaration) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_vertex_declaration(PCDeviceBackend.device, declaration);
}

extern "C" void PCDeviceGetVertexShader(PC_TEXTURE_RESOURCE **shader) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_vertex_shader(PCDeviceBackend.device, shader);
}

extern "C" void PCDeviceGetVertexShaderConstantBPc(u32 start, BOOL *data, u32 count) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_vertex_shader_constant_b(PCDeviceBackend.device, start, data, count);
}

extern "C" void PCDeviceGetVertexShaderConstantFPc(u32 start, f32 *data, u32 count) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_vertex_shader_constant_f(PCDeviceBackend.device, start, data, count);
}

extern "C" void PCDeviceGetVertexShaderConstantIPc(u32 start, i32 *data, u32 count) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->get_vertex_shader_constant_i(PCDeviceBackend.device, start, data, count);
}

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

extern "C" void PCDeviceSetPixelShaderConstantBPc(u32 start, const BOOL *data, u32 count) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_pixel_shader_constant_b(PCDeviceBackend.device, start, data, count);
}

extern "C" void PCDeviceSetPixelShaderConstantIPc(u32 start, const i32 *data, u32 count) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_pixel_shader_constant_i(PCDeviceBackend.device, start, data, count);
}

extern "C" void PCDeviceSetScissorRect(const RECT *rect) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_scissor_rect(PCDeviceBackend.device, rect);
}

extern "C" void PCDeviceSetVertexShaderConstantBPc(u32 start, const BOOL *data, u32 count) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_vertex_shader_constant_b(PCDeviceBackend.device, start, data, count);
}

extern "C" void PCDeviceSetVertexShaderConstantIPc(u32 start, const i32 *data, u32 count) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_vertex_shader_constant_i(PCDeviceBackend.device, start, data, count);
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

void PC_DEVICE_STATE_CACHE::Invalidate() {
    vertex_declaration = reinterpret_cast<PC_TEXTURE_RESOURCE *>(-1);
    index_buffer = reinterpret_cast<PC_BUFFER_RESOURCE *>(-1);
    vertex_buffer = reinterpret_cast<PC_BUFFER_RESOURCE *>(-1);
    stream_stride = (u32)-1;
    stream_number = (u32)-1;
    stream_offset = (u32)-1;
    for (i32 stage = 0; stage < 5; ++stage) {
        textures[stage] = reinterpret_cast<void *>(-1);
    }
    for (i32 index = 0; index < 8 * 14; ++index) {
        sampler_states[0][index] = -1;
    }
    state_key = -1;
    render_states[0] = -1;
    render_states[1] = -1;
    render_states[2] = -1;
    render_states[3] = -1;
    render_states[4] = -1;
    render_states[5] = -1;
    render_states[6] = -1;
    render_states[7] = -1;
    render_states[8] = -1;
    render_states[9] = -1;
    render_states[10] = -1;
    render_states[11] = -1;
}

extern "C" void PCDeviceBeginScene(void) {
    PCDeviceStateCache.Invalidate();
    PCDeviceBackend.BeginScene();
}

extern "C" void fun_006e45d0(void) {
}

// noinline: the original calls DrawPostEffectQuad out of line from PCGraphicsDrawPostEffectQuad; VC8 would inline
// it there.
__declspec(noinline) void PC_DEVICE_BACKEND::DrawPostEffectQuad() {
    PCDeviceSetVertexDeclaration(position_uv_declaration);
    PCDeviceSetStreamSource(0, quad_buffer, 0, 20);
    PCDeviceDrawPrimitive(5, 0, 2);
}

// noinline: the original calls DrawPostEffectIndexed out of line from PCGraphicsDrawPostEffectIndexed; VC8 would
// inline it there.
__declspec(noinline) void PC_DEVICE_BACKEND::DrawPostEffectIndexed() {
    PCDeviceSetVertexDeclaration(position_uv_declaration);
    PCDeviceSetStreamSource(0, grid_vertex_buffer, 0, 20);
    PCDeviceSetIndices(grid_index_buffer);
    PCDeviceDrawIndexedPrimitive(4, 0, 0, grid_vertex_count, 0, grid_triangle_count);
}

void PC_DEVICE_BACKEND::PostEffectBarrier() {
}

u16 PC_DEVICE_BACKEND::GetSimultaneousRenderTargets() {
    if (multisample_index == 0 && PCDeviceHardwareVertexProcessingPC >= 2) {
        return caps.simultaneous_render_targets;
    }
    return 1;
}

void PC_DEVICE_BACKEND::RestoreDepthStencil() {
    device->methods->set_depth_stencil_surface(device, depth_surface);
}

HRESULT PC_DEVICE_BACKEND::CopyBackBufferTo(PC_EFFECT_SURFACE *target, i32 filter) {
    PC_TEXTURE_RESOURCE *source = target_surfaces[0];
    if (multisample_index != 0 || source == NULL) {
        source = back_buffer;
    }
    return device->methods->stretch_rect(device, source, NULL, target->surface, NULL, filter);
}

void PC_DEVICE_BACKEND::SwapTargets() {
    GRAPHICS_RENDER_TEXTURE *texture = textures[0];
    textures[0] = textures[2];
    textures[2] = texture;
    PC_TEXTURE_RESOURCE *surface = target_surfaces[0];
    target_surfaces[0] = target_surfaces[2];
    target_surfaces[2] = surface;
}

void PC_DEVICE_BACKEND::SaveSettings() {
    saved_windowed = parameters.windowed == 1;
    saved_mode = active_mode;
    saved_feature = active_feature;
    saved_multisample = multisample_index;
    saved_target_count = target_count;
    saved_aspect = PcWindowOptions.aspect_mode;
}

void PC_DEVICE_BACKEND::RequestDisplayMode(u32 width, u32 height, u32 refresh) {
    u32 mode = FindMode(width, height, refresh);
    if (mode >= mode_count) {
        mode = active_mode;
    }
    requested_mode = mode;
}

extern "C" void PCGraphicsDrawPostEffectQuad(void) {
    PCDeviceBackend.DrawPostEffectQuad();
}

extern "C" void PCGraphicsDrawPostEffectIndexed(void) {
    PCDeviceBackend.DrawPostEffectIndexed();
}

extern "C" void NuRndrEndFrameAction(void) {
}

extern "C" i32 PCDeviceShowCursor(i32 show) {
    if (PCDeviceBackend.device != NULL) {
        NudxFw_D3DBeginCriticalSection();
        PCDeviceLastResult = PCDeviceBackend.device->methods->show_cursor(PCDeviceBackend.device, show);
        NudxFw_D3DEndCriticalSection();
    }
    return 0;
}

extern "C" void PCDeviceStretchRect(PC_TEXTURE_RESOURCE *source, const RECT *source_rect, PC_TEXTURE_RESOURCE *destination,
                                    const RECT *destination_rect, i32 filter) {
    PCDeviceLastResult =
        PCDeviceBackend.device->methods->stretch_rect(PCDeviceBackend.device, source, source_rect, destination, destination_rect, filter);
}

extern "C" i32 PCDeviceSetCursorPosition(i32 x, i32 y) {
    PCDeviceBackend.device->methods->set_cursor_position(PCDeviceBackend.device, x, y, 0);
    return 0;
}
