#include "gameapi/apitext.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nuwin32/Stubs.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/nu2api_windows.h"
#include "nu2api/nuwin32/pcapi.h"
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
#include "nu2api/pc_fmv_native.h"
#include "legoapi/core/input/gamepads.h"
#include "decomp.h"
#include "gameapi/edtools/edui.h"
#include "legoapi/items/collect/bolts.h"
#include "nu2api/huffdecode.h"
#include "nu2api/nu3d/nuvport.h"
#include "nu2api/nucamera_dlist.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/nuvec4.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

extern "C" HRESULT PCDeviceTestCooperativeLevel();

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

extern "C" {
    static u32 PCGraphicsMultisampleTypes[5] = {0, 2, 4, 8, 16};
}

u32 PC_DEVICE_BACKEND::GetMultisampleType() {
    return PCGraphicsMultisampleTypes[multisample_index];
}

u16 PC_DEVICE_BACKEND::GetSimultaneousRenderTargets() {
    if (multisample_index == 0 && PCDeviceHardwareVertexProcessingPC >= 2) {
        return caps.simultaneous_render_targets;
    }
    return 1;
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

// noinline: the original calls EndScene out of line from PCDeviceEndScene; VC8 would inline it there.
__declspec(noinline) HRESULT PC_DEVICE_BACKEND::EndScene() {
    PC_D3D9_DEVICE *view = device;
    return view->methods->end_scene(view);
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

HRESULT PC_DEVICE_BACKEND::CopySurfaceTarget(PC_EFFECT_SURFACE *target, i32 filter, i32 unused) {
    PC_D3D9_DEVICE *view = device;
    return view->methods->stretch_rect(view, current_surface, NULL, target->surface, NULL, filter);
}

void PC_DEVICE_BACKEND::SwapTargets() {
    GRAPHICS_RENDER_TEXTURE *texture = textures[0];
    textures[0] = textures[2];
    textures[2] = texture;
    PC_TEXTURE_RESOURCE *surface = target_surfaces[0];
    target_surfaces[0] = target_surfaces[2];
    target_surfaces[2] = surface;
}

/* Uncalled: draws the screen triangle, optionally with no pixel shader bound. */
void PC_DEVICE_BACKEND::DrawScreenTriangle(u8 without_pixel_shader) {
    PCDeviceSetVertexDeclaration(screen_uv_declaration);
    PCDeviceSetStreamSource(0, triangle_buffer, 0, 16);
    if (without_pixel_shader) {
        PCDeviceSetPixelShader(NULL);
    }
    PCDeviceDrawPrimitive(4, 0, 1);
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

void PC_DEVICE_BACKEND::SaveSettings() {
    saved_windowed = parameters.windowed == 1;
    saved_mode = active_mode;
    saved_feature = active_feature;
    saved_multisample = multisample_index;
    saved_target_count = target_count;
    saved_aspect = PcWindowOptions.aspect_mode;
}

bool PC_DEVICE_BACKEND::HasRequestedModeChanged() {
    return requested_windowed != (parameters.windowed == 1) || requested_mode != active_mode || requested_feature != active_feature ||
           requested_multisample != multisample_index || requested_target_count != target_count;
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

void PC_DEVICE_BACKEND::RequestDisplayMode(u32 width, u32 height, u32 refresh) {
    u32 mode = FindMode(width, height, refresh);
    if (mode >= mode_count) {
        mode = active_mode;
    }
    requested_mode = mode;
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

void PC_DEVICE_BACKEND::RestoreDefaultPoolResources() {
    BuildDefaultPoolResources();
}

extern "C" {
    i32 PS2_VCNTR_X = 320;
    i32 PS2_VCNTR_Y = 240;
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

extern "C" {
    void (*PCGraphicsDeviceLostCallback)(GRAPHICS_PRESENT_PARAMETERS *) = NULL;
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

// noinline: the original calls BeginScene out of line from PCDeviceBeginScene; VC8 would inline it there.
__declspec(noinline) void PC_DEVICE_BACKEND::BeginScene() {
    PCGraphicsFrameScratchCount.reset_word = 0;
    PCGraphicsBoundShader = -1;
    PCGraphicsFrameState = -1;
    PC_D3D9_DEVICE *view = device;
    view->methods->begin_scene(view);
    BeginDefaultTargets();
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

void PC_DEVICE_BACKEND::RestoreSavedMode() {
    requested_windowed = saved_windowed;
    if (saved_mode < mode_count) {
        requested_mode = saved_mode;
    } else {
        requested_mode = active_mode;
    }
    requested_feature = saved_feature;
    requested_multisample = saved_multisample;
    requested_target_count = saved_target_count;
    PcWindowOptions.SetAspectMode(saved_aspect);
    PcWindowOptions.Save();
    i32 aspect = PcWindowOptions.aspect_mode;
    if (aspect == PC_ASPECT_AUTO) {
        aspect = active_aspect_mode;
    }
    MenuAppliedAspect = aspect;
    CreateOrResetDevice(1);
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
    PCByteCopy(modes, all_modes, mode_count * sizeof(GRAPHICS_DISPLAY_MODE));
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
