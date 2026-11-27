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

extern "C" {
    HRESULT PCDeviceLastResult = 0;
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

extern "C" void PCDeviceCreateVertexDeclaration(const PC_VERTEX_ELEMENT *elements, PC_TEXTURE_RESOURCE **out) {
    PC_D3D9_DEVICE *device = PCDeviceBackend.device;
    PCDeviceLastResult = device->methods->create_vertex_declaration(device, elements, out);
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

PC_DEVICE_BACKEND::~PC_DEVICE_BACKEND() {
    Shutdown();
}

// noinline: the original calls PCDeviceClear out of line from PC_DEVICE_BACKEND::CreateOrResetDevice; VC8 would
// inline it there.
extern "C" __declspec(noinline) void PCDeviceClear(u32 count, const RECT *rectangles, u32 flags, u32 colour, f32 depth, u32 stencil) {
    PC_D3D9_DEVICE *device = PCDeviceBackend.device;
    PCDeviceLastResult = device->methods->clear(device, count, rectangles, flags, colour, depth, stencil);
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

// noinline: the original calls PCDeviceEvictManagedResources out of line from
// PC_DEVICE_BACKEND::CreateOrResetDevice; VC8 would inline it there.
__declspec(noinline) void PCDeviceEvictManagedResources() {
    PC_D3D9_DEVICE *view = PCDeviceBackend.device;
    PCDeviceLastResult = view->methods->evict_managed_resources(view);
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

extern "C" HRESULT PCDeviceEndScene(void) {
    return PCDeviceBackend.EndScene();
}

// noinline: the original calls PCDeviceDrawPrimitive out of line from PC_DEVICE_BACKEND::DrawPostEffectQuad; VC8
// would inline it there.
extern "C" __declspec(noinline) void PCDeviceDrawPrimitive(i32 type, u32 start, u32 count) {
    ++PcFrameStats.primitive_calls;
    PCDeviceLastResult = PCDeviceBackend.device->methods->draw_primitive(PCDeviceBackend.device, type, start, count);
}

// noinline: the original calls PCDeviceDrawIndexedPrimitive out of line from
// PC_DEVICE_BACKEND::DrawPostEffectIndexed; VC8 would inline it there.
extern "C" __declspec(noinline) void PCDeviceDrawIndexedPrimitive(i32 type, i32 base_vertex, u32 minimum_vertex, u32 vertex_count,
                                                                  u32 start_index, u32 primitive_count) {
    ++PcFrameStats.indexed_primitive_calls;
    PCDeviceLastResult = PCDeviceBackend.device->methods->draw_indexed_primitive(PCDeviceBackend.device, type, base_vertex, minimum_vertex,
                                                                                 vertex_count, start_index, primitive_count);
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

extern "C" void PCDeviceBeginScene(void) {
    PCDeviceStateCache.Invalidate();
    PCDeviceBackend.BeginScene();
}

extern "C" void fun_006e45d0(void) {
}

void PC_DEVICE_BACKEND::PostEffectBarrier() {
}
