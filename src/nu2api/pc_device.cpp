#include "saga.h"
#include <string.h>

extern "C" void __cdecl PCDeviceReleaseTexture(PC_TEXTURE_RESOURCE_PC **texture) {
    if (*texture != NULL) {
        (*texture)->methods->release(*texture);
        *texture = NULL;
    }
}

void __thiscall PC_DEVICE_BACKEND_PC::ApplyViewport(const PC_VIEWPORT_PC *viewport) {
    PC_VIEWPORT_PC applied = *viewport;
    if (applied.width >= (u32)DebrisGlassTextureWidth)
        applied.width = (u32)DebrisGlassTextureWidth;
    if (applied.height >= (u32)DebrisGlassTextureHeight)
        applied.height = (u32)DebrisGlassTextureHeight;
    device->methods->set_viewport(device, &applied);
    requested_viewport.x = viewport->x;
    requested_viewport.y = viewport->y;
    requested_viewport.width = viewport->width;
    requested_viewport.height = viewport->height;
    memcpy(&requested_viewport.min_z, &viewport->min_z, sizeof(f32));
    memcpy(&requested_viewport.max_z, &viewport->max_z, sizeof(f32));
}

extern "C" void __cdecl NuVpSubmitPc(const PC_VIEWPORT_PC *viewport) {
    if (viewport != NULL)
        PCDeviceBackend.ApplyViewport(viewport);
}

extern "C" HRESULT __cdecl PCDeviceSetTransform(i32 state, const NUMTX *matrix) {
    HRESULT result = PCDeviceBackend.device->methods->set_transform(PCDeviceBackend.device, state, matrix);
    PCDeviceLastResult = result;
    return result;
}

extern "C" void __cdecl PCDeviceSetRenderState(i32 state, u32 value) {
    i16 cache_index;

    if (PCDeviceStateCacheEnabled != 0) {
        switch (state) {
            case 168:
                cache_index = 0;
                break;
            case 190:
                cache_index = 1;
                break;
            case 191:
                cache_index = 2;
                break;
            case 192:
                cache_index = 3;
                break;
            case 7:
                cache_index = 4;
                break;
            case 14:
                cache_index = 5;
                break;
            case 23:
                cache_index = 6;
                break;
            case 22:
                cache_index = 7;
                break;
            case 15:
                cache_index = 8;
                break;
            case 27:
                cache_index = 9;
                break;
            case 19:
                cache_index = 10;
                break;
            case 20:
                cache_index = 11;
                break;
            default:
                goto submit;
        }
        if ((u32)PCDeviceRenderStateCache[cache_index] == value) {
            return;
        }
        PCDeviceRenderStateCache[cache_index] = (i32)value;
    }

submit:
    PCDeviceCacheStateKey = -1;
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_render_state(PCDeviceBackend.device, state, value);
}

extern "C" void __cdecl PCDeviceSetSamplerState(u32 stage, i32 state, u32 value) {
    PCDeviceSamplerInvalidation[stage][0] = -1;
    PCDeviceSamplerInvalidation[stage][1] = -1;
    if (PCDeviceStateCacheEnabled != 0 && stage < 8 && state < 14 && state != 4) {
        if ((u32)PCDeviceSamplerStateCache[stage][state] == value) {
            return;
        }
        PCDeviceSamplerStateCache[stage][state] = (i32)value;
    }
    PCDeviceLastResult =
        PCDeviceBackend.device->methods->set_sampler_state(PCDeviceBackend.device, stage, state, value);
}

extern "C" void __cdecl PCDeviceSetTexture(u32 stage, void *texture) {
    if (PCDeviceStateCacheEnabled != 0 && stage < 5) {
        if (PCDeviceTextureCache[stage] == texture) {
            return;
        }
        PCDeviceTextureCache[stage] = texture;
    }
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_texture(PCDeviceBackend.device, stage, texture);
}

extern "C" void __cdecl PCDeviceSetTextureStageState(u32 stage, i32 state, u32 value) {
    PCDeviceLastResult =
        PCDeviceBackend.device->methods->set_texture_stage_state(PCDeviceBackend.device, stage, state, value);
}

extern "C" void __cdecl PCDeviceCreateTextureFromMemory(const void *data, u32 size, PC_TEXTURE_RESOURCE_PC **texture) {
    PCDeviceLastResult = D3DXCreateTextureFromFileInMemory(reinterpret_cast<IDirect3DDevice9 *>(PCDeviceBackend.device),
                                                           data, size, reinterpret_cast<IDirect3DTexture9 **>(texture));
}

extern "C" void __cdecl PCDeviceCreateVertexBuffer(u32 size, u32 usage, u32 vertex_format, i32 pool,
                                                   PC_BUFFER_RESOURCE_PC **resource) {
    if (PCDeviceHardwareVertexProcessingPC == 0)
        usage |= 0x10;
    PCDeviceLastResult = PCDeviceBackend.device->methods->create_vertex_buffer(PCDeviceBackend.device, size, usage,
                                                                               vertex_format, pool, resource, NULL);
}

extern "C" void __cdecl PCDeviceCreateIndexBuffer(u32 size, u32 usage, i32 format, i32 pool,
                                                  PC_BUFFER_RESOURCE_PC **resource, HANDLE *shared_handle) {
    if (PCDeviceHardwareVertexProcessingPC == 0)
        usage |= 0x10;
    PCDeviceLastResult = PCDeviceBackend.device->methods->create_index_buffer(PCDeviceBackend.device, size, usage,
                                                                              format, pool, resource, shared_handle);
}

extern "C" void __cdecl PCDeviceCreateTextureFromMemoryEx(const void *data, u32 size, u32 width, u32 height, u32 levels,
                                                          u32 usage, i32 format, i32 pool, u32 filter, u32 mip_filter,
                                                          u32 colour_key, D3DXIMAGE_INFO *information,
                                                          PALETTEENTRY *palette, PC_TEXTURE_RESOURCE_PC **resource) {
    PCDeviceLastResult = D3DXCreateTextureFromFileInMemoryEx(
        reinterpret_cast<IDirect3DDevice9 *>(PCDeviceBackend.device), data, size, width, height, levels, usage,
        static_cast<D3DFORMAT>(format), static_cast<D3DPOOL>(pool), filter, mip_filter, colour_key, information,
        palette, reinterpret_cast<IDirect3DTexture9 **>(resource));
}

extern "C" void __cdecl PCDeviceCreateCubeTextureFromMemory(const void *data, u32 size,
                                                            PC_TEXTURE_RESOURCE_PC **resource) {
    PCDeviceLastResult =
        D3DXCreateCubeTextureFromFileInMemory(reinterpret_cast<IDirect3DDevice9 *>(PCDeviceBackend.device), data, size,
                                              reinterpret_cast<IDirect3DCubeTexture9 **>(resource));
}

struct PC_MATERIAL_DEVICE_CAPS_VIEW {
    u8 unknown_000[0x654];
    u8 shader_caps;
    i32 __thiscall ShaderVariant16();
    i32 __thiscall ShaderVariant1();
};
STATIC_ASSERT(offsetof(PC_MATERIAL_DEVICE_CAPS_VIEW, shader_caps) == 0x654);
i32 __thiscall PC_MATERIAL_DEVICE_CAPS_VIEW::ShaderVariant16() {
    return (shader_caps & 0x20) ? 16 : 0;
}
i32 __thiscall PC_MATERIAL_DEVICE_CAPS_VIEW::ShaderVariant1() {
    return (shader_caps & 0x20) ? 1 : 0;
}
extern "C" void __cdecl PCDeviceCreateVertexDeclaration(const PC_VERTEX_ELEMENT_PC *elements,
                                                        PC_TEXTURE_RESOURCE_PC **out) {
    PC_D3D9_DEVICE_PC *device = PCDeviceBackend.device;
    PCDeviceLastResult = device->methods->create_vertex_declaration(device, elements, out);
}

extern "C" void __cdecl PCDeviceLockVertexBuffer(u32 offset, u32 size, void **data, u32 flags,
                                                 PC_BUFFER_RESOURCE_PC *resource) {
    PCDeviceLastResult = resource->methods->lock(resource, offset, size, data, flags);
}
extern "C" HRESULT __cdecl PCDeviceUnlockVertexBuffer(PC_BUFFER_RESOURCE_PC *resource) {
    return resource->methods->unlock(resource);
}

struct PRIMITIVE_D3D_DEVICE_VIEW;
struct PRIMITIVE_D3D_VTABLE_VIEW {
    void *unknown_000[83];
    HRESULT(__stdcall *draw_primitive_up)(PRIMITIVE_D3D_DEVICE_VIEW *, i32, u32, const void *, u32);
};
STATIC_ASSERT(offsetof(PRIMITIVE_D3D_VTABLE_VIEW, draw_primitive_up) == 0x14c);
struct PRIMITIVE_D3D_DEVICE_VIEW {
    PRIMITIVE_D3D_VTABLE_VIEW *methods;
};
extern "C" void __cdecl PCDeviceDrawPrimitiveUPPc(i32 type, u32 count, const void *vertices, u32 stride) {
    ++PCDevicePrimitiveUPCallCountPc;
    PCDeviceVertexBufferCachePc = NULL;
    PCDeviceIndexBufferCachePc = NULL;
    PRIMITIVE_D3D_DEVICE_VIEW *device = reinterpret_cast<PRIMITIVE_D3D_DEVICE_VIEW *>(PCDeviceBackend.device);
    PCDeviceLastResult = device->methods->draw_primitive_up(device, type, count, vertices, stride);
}

extern "C" const char PCDeviceTextureCubeSourcePathPc[];

extern "C" void __cdecl PCDeviceCreateCubeTextureFromFile(char *path, PC_TEXTURE_RESOURCE_PC **resource) {
    u32 size = static_cast<u32>(NuFileSize(path));
    if (size != 0) {
        void *data = NuFileLoad(path);
        if (data != NULL) {
            PCDeviceLastResult =
                D3DXCreateCubeTextureFromFileInMemory(reinterpret_cast<IDirect3DDevice9 *>(PCDeviceBackend.device),
                                                      data, size, reinterpret_cast<IDirect3DCubeTexture9 **>(resource));
            NuFree(data, PCDeviceTextureCubeSourcePathPc, 1116);
        }
    }
}

#include <math.h>
PC_DEVICE_BACKEND_PC::PC_DEVICE_BACKEND_PC() {
    aspect = 1.3333334f;
    active_aspect = 1.3333334f;
    factory = NULL;
    device = NULL;
    adapter = 0;
    device_type = 1;
    creation_flags = 0;
    force_software = 0;
    unknown_5f9 = 0;
    multithreaded = 1;
    present_result = 0;
    state600 = 1;
    state604 = 1;
    mode_count = 0;
    modes = NULL;
    active_mode = 0;
    aspect_mode = 0;
    active_aspect_mode = 0;
    feature634 = 0;
    multisample_index = 0;
    multisample_tested = 0;
    shader_flags = 0;
    requested_windowed = 1;
    requested_mode = 0;
    requested_feature = 0;
    requested_multisample = 0;
    requested_target_count = 1;
    declaration684 = NULL;
    buffer688 = NULL;
    declaration68c = NULL;
    buffer690 = NULL;
    buffer694 = NULL;
    buffer698 = NULL;
    state69c = 1;
    state6a0 = 1;
    state6a4 = 0;
    state6a8 = 0;
    declaration6ac = NULL;
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
void __thiscall PC_DEVICE_BACKEND_PC::InitializeFactory() {
    factory = reinterpret_cast<GRAPHICS_FACTORY_PC *>(Direct3DCreate9(32));
    if (factory == NULL)
        PCGraphicsFatalDiagnostic(&PCNativeWarningDebugState, "Failed to initialize DirectX");
    factory->methods->get_device_caps(factory, 0, 1, &caps);
    factory->methods->get_adapter_identifier(factory, 0, 0, &identifier);
    factory->methods->get_adapter_display_mode(factory, 0, &desktop_mode);
    aspect_mode = FindAspect(desktop_mode.width, desktop_mode.height);
    active_aspect_mode = aspect_mode;
    f32 ratio = (f32)((f64)desktop_mode.width / (f64)desktop_mode.height);
    aspect = ratio;
    active_aspect = ratio;
}
i32 __thiscall PC_DEVICE_BACKEND_PC::FindAspect(u32 width, u32 height) {
    f32 ratio = (f32)((f64)width / (f64)height);
    f32 closest = 1000.0f;
    i32 selected = 0;
    f32 distance = (f32)((f64)ratio - (f64)1.3333334f);
    distance = (f32)fabs((f64)distance);
    if ((f64)distance < 1000.0)
        closest = distance;
    distance = (f32)((f64)ratio - (f64)1.7777778f);
    distance = (f32)fabs((f64)distance);
    if (closest > distance) {
        closest = distance;
        selected = 1;
    }
    distance = (f32)((f64)ratio - (f64)1.6f);
    distance = (f32)fabs((f64)distance);
    if (closest > distance)
        return 2;
    return selected;
}
void __thiscall PC_DEVICE_BACKEND_PC::ReleaseDefaultPoolResources() {
    if (back_buffer != NULL) {
        back_buffer->methods->release(back_buffer);
        back_buffer = NULL;
    }
    if (depth_surface != NULL) {
        depth_surface->methods->release(depth_surface);
        depth_surface = NULL;
    }
    if (buffer690 != NULL) {
        buffer690->methods->release(buffer690);
        buffer690 = NULL;
    }
    if (buffer694 != NULL) {
        buffer694->methods->release(buffer694);
        buffer694 = NULL;
    }
    if (buffer698 != NULL) {
        buffer698->methods->release(buffer698);
        buffer698 = NULL;
    }
    if (buffer688 != NULL) {
        buffer688->methods->release(buffer688);
        buffer688 = NULL;
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
void __thiscall PC_DEVICE_BACKEND_PC::Shutdown() {
    if (modes != NULL) {
        delete modes;
        modes = NULL;
    }
    ReleaseDefaultPoolResources();
    if (declaration68c != NULL) {
        declaration68c->methods->release(declaration68c);
        declaration68c = NULL;
    }
    if (declaration6ac != NULL) {
        declaration6ac->methods->release(declaration6ac);
        declaration6ac = NULL;
    }
    if (device != NULL) {
        PC_TEXTURE_RESOURCE_PC *release_view = reinterpret_cast<PC_TEXTURE_RESOURCE_PC *>(device);
        release_view->methods->release(release_view);
        device = NULL;
    }
    if (factory != NULL) {
        factory->methods->release(factory);
        factory = NULL;
    }
}
PC_DEVICE_BACKEND_PC::~PC_DEVICE_BACKEND_PC() {
    Shutdown();
}

extern "C" PC_DEVICE_BACKEND_PC PCDeviceBackend;
PC_DEVICE_BACKEND_PC PCDeviceBackend;
struct GRAPHICS_DEVICE_PC;
struct GRAPHICS_DEVICE_METHODS_PC {
    void *unknown_0000[3];
    HRESULT(__stdcall *test_cooperative_level)(GRAPHICS_DEVICE_PC *);
    void *get_available_texture_memory;
    HRESULT(__stdcall *evict_managed_resources)(GRAPHICS_DEVICE_PC *);
    void *get_direct3d;
    HRESULT(__stdcall *get_device_caps)(GRAPHICS_DEVICE_PC *, GRAPHICS_CAPS_PC *);
    void *unknown_0020[8];
    HRESULT(__stdcall *reset)(GRAPHICS_DEVICE_PC *, GRAPHICS_PRESENT_PARAMETERS_PC *);
    HRESULT(__stdcall *present)(GRAPHICS_DEVICE_PC *, const RECT *, const RECT *, HWND, const RGNDATA *);
    HRESULT(__stdcall *get_back_buffer)(GRAPHICS_DEVICE_PC *, u32, u32, i32, PC_TEXTURE_RESOURCE_PC **);
    void *unknown_004c[15];
    HRESULT(__stdcall *stretch_rect)
    (GRAPHICS_DEVICE_PC *, PC_TEXTURE_RESOURCE_PC *, const RECT *, PC_TEXTURE_RESOURCE_PC *, const RECT *, i32);
    void *unknown_008c[2];
    HRESULT(__stdcall *set_render_target)(GRAPHICS_DEVICE_PC *, u32, PC_TEXTURE_RESOURCE_PC *);
    void *get_render_target;
    HRESULT(__stdcall *set_depth_stencil_surface)(GRAPHICS_DEVICE_PC *, PC_TEXTURE_RESOURCE_PC *);
    HRESULT(__stdcall *get_depth_stencil_surface)(GRAPHICS_DEVICE_PC *, PC_TEXTURE_RESOURCE_PC **);
    HRESULT(__stdcall *begin_scene)(GRAPHICS_DEVICE_PC *);
    HRESULT(__stdcall *end_scene)(GRAPHICS_DEVICE_PC *);
    HRESULT(__stdcall *clear)(GRAPHICS_DEVICE_PC *, u32, const RECT *, u32, u32, f32, u32);
    void *unknown_00b0[33];
    HRESULT(__stdcall *set_software_vertex_processing)(GRAPHICS_DEVICE_PC *, BOOL);
};
struct GRAPHICS_DEVICE_PC {
    const GRAPHICS_DEVICE_METHODS_PC *methods;
};
STATIC_ASSERT(offsetof(GRAPHICS_DEVICE_METHODS_PC, test_cooperative_level) == 0xc);
STATIC_ASSERT(offsetof(GRAPHICS_DEVICE_METHODS_PC, evict_managed_resources) == 0x14);
STATIC_ASSERT(offsetof(GRAPHICS_DEVICE_METHODS_PC, get_device_caps) == 0x1c);
STATIC_ASSERT(offsetof(GRAPHICS_DEVICE_METHODS_PC, reset) == 0x40);
STATIC_ASSERT(offsetof(GRAPHICS_DEVICE_METHODS_PC, present) == 0x44);
STATIC_ASSERT(offsetof(GRAPHICS_DEVICE_METHODS_PC, get_back_buffer) == 0x48);
STATIC_ASSERT(offsetof(GRAPHICS_DEVICE_METHODS_PC, get_depth_stencil_surface) == 0xa0);
STATIC_ASSERT(offsetof(GRAPHICS_DEVICE_METHODS_PC, set_render_target) == 0x94);
STATIC_ASSERT(offsetof(GRAPHICS_DEVICE_METHODS_PC, set_depth_stencil_surface) == 0x9c);
STATIC_ASSERT(offsetof(GRAPHICS_DEVICE_METHODS_PC, begin_scene) == 0xa4);
STATIC_ASSERT(offsetof(GRAPHICS_DEVICE_METHODS_PC, end_scene) == 0xa8);
STATIC_ASSERT(offsetof(GRAPHICS_DEVICE_METHODS_PC, clear) == 0xac);
STATIC_ASSERT(offsetof(GRAPHICS_DEVICE_METHODS_PC, set_software_vertex_processing) == 0x134);

HRESULT __thiscall PC_DEVICE_BACKEND_PC::EndScene() {
    GRAPHICS_DEVICE_PC *view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    return view->methods->end_scene(view);
}
extern "C" void __cdecl PCDeviceClear(u32 count, const RECT *rectangles, u32 flags, u32 colour, f32 depth,
                                      u32 stencil) {
    GRAPHICS_DEVICE_PC *device = reinterpret_cast<GRAPHICS_DEVICE_PC *>(PCDeviceBackend.device);
    PCDeviceLastResult = device->methods->clear(device, count, rectangles, flags, colour, depth, stencil);
}
extern "C" HRESULT __cdecl PCDeviceTestCooperativeLevel() {
    GRAPHICS_DEVICE_PC *device = reinterpret_cast<GRAPHICS_DEVICE_PC *>(PCDeviceBackend.device);
    return device->methods->test_cooperative_level(device);
}
extern "C" i32 __cdecl PCDeviceIsAbsent() {
    return PCDeviceBackend.device == NULL;
}
extern "C" PC_D3D9_DEVICE_PC *__cdecl PCDeviceGetDevice() {
    return PCDeviceBackend.device;
}

extern "C" {
    static u32 PCGraphicsMultisampleTypes[5] = {0, 2, 4, 8, 16};
}
u32 __thiscall PC_DEVICE_BACKEND_PC::GetMultisampleType() {
    return PCGraphicsMultisampleTypes[multisample_index];
}
struct PC_TEXTURE_CREATE_DEVICE_PC;
struct PC_TEXTURE_CREATE_METHODS_PC {
    void *reserved_00[23];
    HRESULT(__stdcall *create_texture)
    (PC_TEXTURE_CREATE_DEVICE_PC *, u32, u32, u32, u32, i32, i32, PC_TEXTURE_RESOURCE_PC **, HANDLE *);
    void *reserved_60[5];
    HRESULT(__stdcall *create_depth_surface)
    (PC_TEXTURE_CREATE_DEVICE_PC *, u32, u32, i32, i32, u32, BOOL, PC_TEXTURE_RESOURCE_PC **, HANDLE *);
};
struct PC_TEXTURE_CREATE_DEVICE_PC {
    const PC_TEXTURE_CREATE_METHODS_PC *methods;
};
STATIC_ASSERT(offsetof(PC_TEXTURE_CREATE_METHODS_PC, create_texture) == 0x5c);
STATIC_ASSERT(offsetof(PC_TEXTURE_CREATE_METHODS_PC, create_depth_surface) == 0x74);
STATIC_ASSERT(offsetof(PC_TEXTURE_CREATE_DEVICE_PC, methods) == 0);
extern "C" void __cdecl PCDeviceCreateDepthSurface(u32 width, u32 height, i32 format, i32 multisample, u32 quality,
                                                   BOOL discard, PC_TEXTURE_RESOURCE_PC **surface,
                                                   HANDLE *shared_handle) {
    PC_TEXTURE_CREATE_DEVICE_PC *device = reinterpret_cast<PC_TEXTURE_CREATE_DEVICE_PC *>(PCDeviceBackend.device);
    PCDeviceLastResult = device->methods->create_depth_surface(device, width, height, format, multisample, quality,
                                                               discard, surface, shared_handle);
}
extern "C" void __cdecl PCDeviceCreateTexture(u32 width, u32 height, u32 levels, u32 usage, i32 format, i32 pool,
                                              PC_TEXTURE_RESOURCE_PC **texture) {
    PC_TEXTURE_CREATE_DEVICE_PC *device = reinterpret_cast<PC_TEXTURE_CREATE_DEVICE_PC *>(PCDeviceBackend.device);
    PCDeviceLastResult =
        device->methods->create_texture(device, width, height, levels, usage, format, pool, texture, NULL);
}

#include <stdlib.h>
extern "C" {
    extern PC_GRAPHICS_FRAME_SCRATCH_VIEW_PC PCGraphicsFrameScratchCount;
    extern i32 PCGraphicsBoundShader;
    extern i32 PCGraphicsFrameState;
    void __cdecl PCDeviceEvictManagedResources();
    void __cdecl PCDeviceSetSoftwareVertexProcessing(BOOL enabled);
}

static i32 __cdecl PCGraphicsModeCompare(const void *left_ptr, const void *right_ptr) {
    const GRAPHICS_DISPLAY_MODE_PC *left = static_cast<const GRAPHICS_DISPLAY_MODE_PC *>(left_ptr);
    const GRAPHICS_DISPLAY_MODE_PC *right = static_cast<const GRAPHICS_DISPLAY_MODE_PC *>(right_ptr);
    if (left->width == 0 && left->height == 0 && left->refresh_rate == 0)
        return 1;
    if (right->width == 0 && right->height == 0 && right->refresh_rate == 0)
        return -1;
    if (left->width < right->width)
        return -1;
    if (left->width > right->width)
        return 1;
    if (left->height < right->height)
        return -1;
    if (left->height > right->height)
        return 1;
    if (left->refresh_rate < right->refresh_rate)
        return -1;
    return left->refresh_rate > right->refresh_rate;
}
u32 __thiscall PC_DEVICE_BACKEND_PC::FindMode(u32 width, u32 height, u32 refresh) {
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
        if (modes[i].width == closest_width &&
            abs((i32)(modes[i].height - height)) <= abs((i32)(closest_height - height))) {
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
        if (modes[selected].width != width)
            modes[selected].width = width;
        if (modes[selected].height != height)
            modes[selected].height = height;
    }
    return selected;
}

extern "C" {
    i32 __cdecl PCGraphicsPrepareVendor();
    void __cdecl PCGraphicsResetResources(i32 reset_available);
    void __cdecl PCGraphicsRecreateResources(i32 reset_available);
    void __cdecl PCDeviceEvictManagedResources();
    void __cdecl PCDeviceSetSoftwareVertexProcessing(BOOL enabled);
    extern u32 PCGraphicsMultisampleTypes[5];
    extern void(__cdecl *PCGraphicsDeviceLostCallback)(GRAPHICS_PRESENT_PARAMETERS_PC *);
}
void __thiscall PC_DEVICE_BACKEND_PC::CreateOrResetDevice(i32 reposition_window) {
    i32 rebuild_resources = 1;
    PCFrameCriticalSectionEnter_candidate();
    saved_windowed = parameters.windowed == 1;
    saved_mode = active_mode;
    saved_feature = feature634;
    saved_multisample = multisample_index;
    saved_target_count = target_count;
    saved_aspect = PcWindowOptions.aspect_mode;
    factory->methods->get_device_caps(factory, 0, 1, &caps);
    parameters.window = reinterpret_cast<HWND>(PCInputInstance_candidate);
    parameters.windowed = requested_windowed;
    parameters.enable_depth = 1;
    parameters.flags = 2;
    parameters.swap_effect = 1;
    parameters.depth_format = 75;
    if (requested_feature != 0 && (caps.caps2 & 1) == 0)
        requested_feature = 0;
    feature634 = requested_feature;
    parameters.count = 2;
    parameters.interval = 1;
    if (parameters.windowed != 0 && reposition_window != 0) {
        i32 x = PcWindowOptions.window_x;
        i32 y = PcWindowOptions.window_y;
        if (x < 0)
            x = (PCDeviceBackend.desktop_mode.width - PcWindowOptions.client_width) >> 1;
        if (y < 0)
            y = (PCDeviceBackend.desktop_mode.height - PcWindowOptions.client_height) >> 1;
        MoveWindow(reinterpret_cast<HWND>(PCInputInstance_candidate), x, y, PcWindowOptions.client_width,
                   PcWindowOptions.client_height, TRUE);
    }
    if (PCDeviceBackend.factory->methods->check_device_type(PCDeviceBackend.factory, 0, 1, modes[requested_mode].format,
                                                            21, parameters.windowed) < 0)
        parameters.format = modes[requested_mode].format;
    else
        parameters.format = 21;
    parameters.width = modes[requested_mode].width;
    parameters.height = modes[requested_mode].height;
    parameters.refresh_rate = modes[requested_mode].refresh_rate;
    if (parameters.windowed != 0)
        parameters.refresh_rate = 0;
    creation_flags = force_software != 0 ? 0 : 0x10;
    if (multithreaded != 0)
        creation_flags |= 4;
    if (force_software == 0 && caps.vertex_shader_version >= 0xfffe0101)
        creation_flags |= 0x40;
    else if (force_software == 0 && (caps.device_caps & 0x10000) != 0)
        creation_flags |= 0x80;
    else
        creation_flags |= 0x20;
    device_type = 1;
    adapter = 0;
    factory->methods->get_adapter_identifier(factory, 0, 0, &identifier);
    PCGraphicsPrepareVendor();
    if (multisample_tested == 0) {
        for (i32 i = 0; i != 5; ++i) {
            u32 colour_quality = 0;
            u32 depth_quality = 0;
            i32 supported = 1;
            if (factory->methods->check_multisample(factory, adapter, device_type, parameters.format,
                                                    parameters.windowed, PCGraphicsMultisampleTypes[i],
                                                    &colour_quality) < 0)
                supported = 0;
            else if (factory->methods->check_multisample(factory, adapter, device_type, parameters.depth_format,
                                                         parameters.windowed, PCGraphicsMultisampleTypes[i],
                                                         &depth_quality) < 0)
                supported = 0;
            u32 maximum = colour_quality > depth_quality ? colour_quality : depth_quality;
            quality[i] = maximum != 0 && supported != 0 ? maximum - 1 : -1;
        }
        quality[0] = 0;
        render_target_flags = 0;
        multisample_tested = 1;
    }
    if (multisample_tested != 0 && quality[requested_multisample] != -1)
        multisample_index = requested_multisample;
    else if (multisample_tested == 0 || quality[multisample_index] == -1)
        multisample_index = 0;
    parameters.multisample_type = PCGraphicsMultisampleTypes[multisample_index];
    parameters.multisample_quality = quality[multisample_index] < 0 ? 0 : quality[multisample_index];
    if (device == NULL) {
        PCDeviceLastResult = PCDeviceBackend.factory->methods->create_device(
            PCDeviceBackend.factory, adapter, device_type, parameters.window, creation_flags, &parameters, &device);
        for (i32 retries = 15; PCDeviceLastResult < 0 && retries != 0; --retries) {
            Sleep(500);
            PCDeviceLastResult = PCDeviceBackend.factory->methods->create_device(
                PCDeviceBackend.factory, adapter, device_type, parameters.window, creation_flags, &parameters, &device);
        }
        if (device == NULL) {
            parameters.width = 640;
            parameters.height = 480;
            parameters.refresh_rate = 60;
            PCDeviceLastResult = factory->methods->create_device(factory, adapter, device_type, parameters.window,
                                                                 creation_flags, &parameters, &device);
            if (device == NULL) {
                if (PCDeviceLastResult == static_cast<HRESULT>(0x8876017c))
                    PCGraphicsFatalDiagnostic(&PCNativeWarningDebugState,
                                              "Out of Video Memory. Application will now exit");
                else if (PCDeviceLastResult == static_cast<HRESULT>(0x88760868))
                    PCGraphicsFatalDiagnostic(&PCNativeWarningDebugState, "Lost d3d device. Application will now exit");
                else if (PCDeviceLastResult == static_cast<HRESULT>(0x8876086a))
                    PCGraphicsFatalDiagnostic(&PCNativeWarningDebugState,
                                              "d3d device unavailable. Application will now exit");
                else
                    PCGraphicsFatalDiagnostic(&PCNativeWarningDebugState,
                                              "Failed to create d3d device. Application will now exit.\nError = 0x%x",
                                              PCDeviceLastResult);
            }
        }
        PS2_REZ_W = parameters.width;
        PS2_REZ_H = parameters.height;
        PS2_VCNTR_X = PS2_REZ_W / 2;
        PS2_VCNTR_Y = PS2_REZ_H / 2;
    } else if (SoundVoiceEnabledGate != 0) {
        HRESULT cooperative = PCDeviceTestCooperativeLevel();
        GRAPHICS_DEVICE_PC *view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
        if (cooperative < 0) {
            if (cooperative == static_cast<HRESULT>(0x88760869)) {
                PCDeviceEvictManagedResources();
                STUBBED_NAMED("CreateOrResetDevice: ResetResources(0) at PC 0x6e3e6c");
                if (view->methods->reset(view, &parameters) >= 0) {
                    STUBBED_NAMED("CreateOrResetDevice: RecreateResources(0) at PC 0x6e3e8e");
                    rebuild_resources = 0;
                }
            } else if (cooperative == static_cast<HRESULT>(0x88760868)) {
                if (PCGraphicsDeviceLostCallback != NULL)
                    PCGraphicsDeviceLostCallback(&parameters);
            } else
                SendMessageA(reinterpret_cast<HWND>(PCInputInstance_candidate), WM_CLOSE, 0, 0);
        } else {
            PCDeviceEvictManagedResources();
            STUBBED_NAMED("CreateOrResetDevice: ResetResources(1) at PC 0x6e3ebc");
            view->methods->reset(view, &parameters);
            PS2_REZ_W = parameters.width;
            PS2_REZ_H = parameters.height;
            PS2_VCNTR_X = PS2_REZ_W / 2;
            PS2_VCNTR_Y = PS2_REZ_H / 2;
            STUBBED_NAMED("CreateOrResetDevice: RecreateResources(1) at PC 0x6e3f00");
            rebuild_resources = 0;
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
    active_aspect = (f32)((f64)parameters.width / (f64)parameters.height);
    nuapi.video_aspect = PcWindowOptions.aspect_mode == 3 ? active_aspect_mode : PcWindowOptions.aspect_mode;
    u32 refresh = parameters.windowed != 0 ? modes[requested_mode].refresh_rate : parameters.refresh_rate;
    active_mode = FindMode(parameters.width, parameters.height, refresh);
    GRAPHICS_DEVICE_PC *view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    view->methods->get_device_caps(view, &caps);
    if (rebuild_resources != 0)
        BuildDefaultPoolResources();
    ResetTargets();
    PCDeviceClear(0, NULL, 1, 0, 1.0f, 0);
    view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    present_result = view->methods->present(view, NULL, NULL, NULL, NULL);
    for (i32 i = 0; i != 8; ++i) {
        PCDeviceSetSamplerState(i, 5, 2);
        PCDeviceSetSamplerState(i, 6, 2);
        PCDeviceSetSamplerState(i, 7, 2);
    }
    if (PCDeviceHardwareVertexProcessingPC == 0)
        PCDeviceSetSoftwareVertexProcessing(0);
    PCDeviceSetRenderState(161, multisample_index != 0);
    for (i32 i = 0; i != 4; ++i)
        target_cache[i] = NULL;
    PcWindowOptions.SetDeviceWidth(PS2_REZ_W);
    PcWindowOptions.SetDeviceHeight(PS2_REZ_H);
    if (parameters.windowed == 0)
        PcWindowOptions.SetRefresh(parameters.refresh_rate);
    PcWindowOptions.SetMultisample(multisample_index);
    PcWindowOptions.Save();
    full_viewport.x = full_viewport.y = 0;
    full_viewport.width = PS2_REZ_W;
    full_viewport.height = PS2_REZ_H;
    full_viewport.min_z = 0.0f;
    full_viewport.max_z = 1.0f;
    requested_windowed = parameters.windowed == 1;
    requested_mode = active_mode;
    requested_feature = feature634;
    requested_multisample = multisample_index;
    requested_target_count = target_count;
    PCFrameCriticalSectionLeave_candidate();
}

void __cdecl PCDeviceEvictManagedResources() {
    GRAPHICS_DEVICE_PC *view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(PCDeviceBackend.device);
    PCDeviceLastResult = view->methods->evict_managed_resources(view);
}
void __cdecl PCDeviceSetSoftwareVertexProcessing(BOOL enabled) {
    GRAPHICS_DEVICE_PC *view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(PCDeviceBackend.device);
    PCDeviceLastResult = view->methods->set_software_vertex_processing(view, enabled);
}
void __thiscall PC_DEVICE_BACKEND_PC::Present(const RECT *source, const RECT *destination, HWND window,
                                              const RGNDATA *region) {
    GRAPHICS_DEVICE_PC *view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    present_result = view->methods->present(view, source, destination, window, region);
    if (present_result < 0) {
        if (present_result == static_cast<HRESULT>(0x88760868)) {
            if (PCDeviceTestCooperativeLevel() == static_cast<HRESULT>(0x88760869))
                CreateOrResetDevice(1);
        } else
            SendMessageA(reinterpret_cast<HWND>(PCInputInstance_candidate), WM_CLOSE, 0, 0);
    }
    for (i32 i = 0; i != 4; ++i)
        target_cache[i] = reinterpret_cast<PC_TEXTURE_RESOURCE_PC *>(-1);
}
void __thiscall PC_DEVICE_BACKEND_PC::BeginScene() {
    PCGraphicsFrameScratchCount.reset_word = 0;
    PCGraphicsBoundShader = -1;
    PCGraphicsFrameState = -1;
    GRAPHICS_DEVICE_PC *view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    view->methods->begin_scene(view);
    BeginDefaultTargets();
}

STATIC_ASSERT(offsetof(PC_VERTEX_ELEMENT_PC, stream) == 0);
STATIC_ASSERT(offsetof(PC_VERTEX_ELEMENT_PC, offset) == 2);
STATIC_ASSERT(offsetof(PC_VERTEX_ELEMENT_PC, method) == 5);
STATIC_ASSERT(offsetof(PC_VERTEX_ELEMENT_PC, usage_index) == 7);
static PC_VERTEX_ELEMENT_PC PCGraphicsPositionUvDeclaration[3] = {
    {0, 0, 2, 0, 0, 0}, {0, 12, 1, 0, 5, 0}, {255, 0, 17, 0, 0, 0}};
static PC_VERTEX_ELEMENT_PC PCGraphicsScreenUvDeclaration[3] = {
    {0, 0, 1, 0, 9, 0}, {0, 8, 1, 0, 5, 0}, {255, 0, 17, 0, 0, 0}};
static PC_VERTEX_ELEMENT_PC PCGraphicsScreenColourUvDeclaration[4] = {
    {0, 0, 3, 0, 9, 0}, {0, 16, 4, 0, 10, 0}, {0, 20, 1, 0, 5, 0}, {255, 0, 17, 0, 0, 0}};
void __thiscall PC_DEVICE_BACKEND_PC::InitializeAdapter() {
    u32 total = PCDeviceBackend.factory->methods->get_adapter_mode_count(PCDeviceBackend.factory, 0, 22);
    GRAPHICS_DISPLAY_MODE_PC *all_modes = new GRAPHICS_DISPLAY_MODE_PC[total];
    u32 minimum_width = 640;
    u32 minimum_height = 480;
    if (PcWindowOptions.bloommethod == 2 || PcWindowOptions.shadowmethod == 2) {
        minimum_width = 800;
        minimum_height = 600;
    }
    mode_count = 0;
    for (u32 i = 0; i != total; ++i) {
        factory->methods->enum_adapter_modes(factory, 0, 22, i, &all_modes[i]);
        if (all_modes[i].width >= minimum_width && all_modes[i].height >= minimum_height &&
            all_modes[i].refresh_rate >= 40 && all_modes[i].refresh_rate <= 85)
            ++mode_count;
        else
            memset(&all_modes[i], 0, sizeof(GRAPHICS_DISPLAY_MODE_PC));
    }
    qsort(all_modes, total, sizeof(GRAPHICS_DISPLAY_MODE_PC), PCGraphicsModeCompare);
    modes = new GRAPHICS_DISPLAY_MODE_PC[mode_count];
    memcpy(modes, all_modes, mode_count * sizeof(GRAPHICS_DISPLAY_MODE_PC));
    delete all_modes;
    if (PcWindowOptions.usehires != 0 && PcWindowOptions.multisample != 6)
        PcWindowOptions.SetMultisample(0);
    u32 selected = FindMode(PcWindowOptions.device_width, PcWindowOptions.device_height, PcWindowOptions.refresh_rate);
    requested_mode = selected < mode_count ? selected : active_mode;
    requested_multisample = PcWindowOptions.multisample;
    CreateOrResetDevice(1);
    PCDeviceSetRenderState(7, 1);
    PCDeviceSetRenderState(137, 0);
    PCDeviceCreateVertexDeclaration(PCGraphicsPositionUvDeclaration, &declaration68c);
    PCDeviceCreateVertexDeclaration(PCGraphicsScreenUvDeclaration, &declaration684);
    PCDeviceCreateVertexDeclaration(PCGraphicsScreenColourUvDeclaration, &declaration6ac);
}

extern "C" {
    void __cdecl NuVpSetZRange(f32, f32);
    void __cdecl NuVpSetPosition(f32, f32);
    void __cdecl NuVpSetSize2(f32, f32);
}
void __thiscall PC_DEVICE_BACKEND_PC::SetRenderTarget(i32 index, PC_TEXTURE_RESOURCE_PC *surface) {
    u16 limit =
        multisample_index == 0 && PCDeviceHardwareVertexProcessingPC >= 2 ? (u16)caps.simultaneous_render_targets : 1;
    if (index <= (i32)limit - 1 && target_cache[index] != surface) {
        target_cache[index] = surface;
        GRAPHICS_DEVICE_PC *view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
        view->methods->set_render_target(view, index, surface);
    }
}
void __thiscall PC_DEVICE_BACKEND_PC::ResetViewport() {
    requested_viewport.x = 0;
    requested_viewport.y = 0;
    requested_viewport.width = PS2_REZ_W;
    requested_viewport.height = PS2_REZ_H;
    requested_viewport.min_z = 0.0f;
    requested_viewport.max_z = 1.0f;
    NuVpSetZRange(0.0f, 1.0f);
    f32 y = (f32)((f64)requested_viewport.y * 16.0);
    f32 x = (f32)((f64)requested_viewport.x * 16.0);
    NuVpSetPosition(x, y);
    NuVpSetSize2(640.0f, 224.0f);
    NuVpUpdate();
    requested_viewport.x = 0;
    requested_viewport.y = 0;
    requested_viewport.width = PS2_REZ_W;
    requested_viewport.height = PS2_REZ_H;
    requested_viewport.min_z = 0.0f;
    requested_viewport.max_z = 1.0f;
    NuVpSubmitPc(&requested_viewport);
}
void __thiscall PC_DEVICE_BACKEND_PC::ResetTargets() {
    SetRenderTarget(0, back_buffer);
    current_surface = back_buffer;
    for (i32 i = 1; i != 4; ++i) {
        u16 limit = multisample_index == 0 && PCDeviceHardwareVertexProcessingPC >= 2
                        ? (u16)caps.simultaneous_render_targets
                        : 1;
        if (i <= (i32)limit - 1 && target_cache[i] != NULL) {
            target_cache[i] = NULL;
            GRAPHICS_DEVICE_PC *view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
            view->methods->set_render_target(view, i, NULL);
        }
    }
    GRAPHICS_DEVICE_PC *view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    view->methods->set_depth_stencil_surface(view, depth_surface);
    PS2_REZ_W = parameters.width;
    PS2_REZ_H = parameters.height;
    PS2_VCNTR_X = PS2_REZ_W / 2;
    PS2_VCNTR_Y = PS2_REZ_H / 2;
    ResetViewport();
}
void __thiscall PC_DEVICE_BACKEND_PC::ConfigureCurrentTargets() {
    if (multisample_index != 0) {
        SetRenderTarget(0, back_buffer);
        current_surface = back_buffer;
    } else {
        for (i32 i = 0; i != 4; ++i) {
            if (i == 0) {
                if (target_surfaces[0] != NULL) {
                    SetRenderTarget(0, target_surfaces[0]);
                    current_surface = target_surfaces[0];
                } else {
                    SetRenderTarget(0, back_buffer);
                    current_surface = back_buffer;
                }
            } else if (i <= 2 && ((PcWindowOptions.bloomenabled != 0 && PcWindowOptions.bloommethod == 2) ||
                                  (PcWindowOptions.shadowsenabled != 0 && PcWindowOptions.shadowmethod == 2)))
                SetRenderTarget(i, target_surfaces[i]);
            else
                SetRenderTarget(i, NULL);
        }
    }
    GRAPHICS_DEVICE_PC *view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    view->methods->set_depth_stencil_surface(view, depth_surface);
    PS2_REZ_W = parameters.width;
    PS2_REZ_H = parameters.height;
    PS2_VCNTR_X = PS2_REZ_W / 2;
    PS2_VCNTR_Y = PS2_REZ_H / 2;
    ResetViewport();
}
void __thiscall PC_DEVICE_BACKEND_PC::BeginDefaultTargets() {
    GRAPHICS_DEVICE_PC *view;
    if (multisample_index != 0) {
        SetRenderTarget(0, back_buffer);
        view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
        view->methods->set_depth_stencil_surface(view, depth_surface);
        view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
        view->methods->clear(view, 0, NULL, 7, 0, 1.0f, 0);
        ConfigureCurrentTargets();
        return;
    }
    SetRenderTarget(0, back_buffer);
    view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    view->methods->set_depth_stencil_surface(view, depth_surface);
    view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    if (depth_surface != NULL)
        view->methods->clear(view, 0, NULL, 7, 0, 1.0f, 0xc0);
    else
        view->methods->clear(view, 0, NULL, 1, 0, 1.0f, 0);
    for (i32 i = 0; i != 4; ++i) {
        switch (i) {
            case 0:
                SetRenderTarget(0, target_surfaces[0] != NULL ? target_surfaces[0] : back_buffer);
                view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
                view->methods->set_depth_stencil_surface(view, depth_surface);
                view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
                view->methods->clear(view, 0, NULL, 1, 0, 1.0f, 0);
                break;
            case 1:
                if (target_surfaces[1] != NULL) {
                    SetRenderTarget(1, NULL);
                    SetRenderTarget(0, target_surfaces[1]);
                    view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
                    view->methods->clear(view, 0, NULL, 1, 0xff000000, 1.0f, 0);
                }
                break;
            case 2:
                if (target_surfaces[2] != NULL) {
                    SetRenderTarget(2, NULL);
                    SetRenderTarget(0, target_surfaces[2]);
                    view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
                    view->methods->clear(view, 0, NULL, 1, 0, 1.0f, 0);
                }
                break;
        }
    }
    ConfigureCurrentTargets();
}

extern "C" {
    GRAPHICS_RENDER_TEXTURE_PC *__cdecl PCGraphicsCreateColourTarget(i32, i32);
    GRAPHICS_RENDER_TEXTURE_PC *__cdecl PCGraphicsCreateColourTargetFormat(i32, i32, i32);
    void __cdecl PCDeviceLockIndexBuffer(u32, u32, void **, u32, PC_BUFFER_RESOURCE_PC *);
    HRESULT __cdecl PCDeviceUnlockIndexBuffer(PC_BUFFER_RESOURCE_PC *);
}
struct PC_GRAPHICS_CLIP_VERTEX_PC {
    f32 x, y, z, u, v;
};
struct PC_GRAPHICS_SCREEN_VERTEX_PC {
    f32 x, y, u, v;
};
STATIC_ASSERT(sizeof(PC_GRAPHICS_CLIP_VERTEX_PC) == 20);
STATIC_ASSERT(offsetof(PC_GRAPHICS_CLIP_VERTEX_PC, x) == 0);
STATIC_ASSERT(offsetof(PC_GRAPHICS_CLIP_VERTEX_PC, y) == 4);
STATIC_ASSERT(offsetof(PC_GRAPHICS_CLIP_VERTEX_PC, z) == 8);
STATIC_ASSERT(offsetof(PC_GRAPHICS_CLIP_VERTEX_PC, u) == 12);
STATIC_ASSERT(offsetof(PC_GRAPHICS_CLIP_VERTEX_PC, v) == 16);
STATIC_ASSERT(sizeof(PC_GRAPHICS_SCREEN_VERTEX_PC) == 16);
STATIC_ASSERT(offsetof(PC_GRAPHICS_SCREEN_VERTEX_PC, x) == 0);
STATIC_ASSERT(offsetof(PC_GRAPHICS_SCREEN_VERTEX_PC, y) == 4);
STATIC_ASSERT(offsetof(PC_GRAPHICS_SCREEN_VERTEX_PC, u) == 8);
STATIC_ASSERT(offsetof(PC_GRAPHICS_SCREEN_VERTEX_PC, v) == 12);
struct GRAPHICS_TEXTURE_SURFACE_VIEW_PC;
struct GRAPHICS_TEXTURE_SURFACE_METHODS_PC {
    void *unknown_000[18];
    HRESULT(__stdcall *get_surface_level)(GRAPHICS_TEXTURE_SURFACE_VIEW_PC *, u32, PC_TEXTURE_RESOURCE_PC **);
};
struct GRAPHICS_TEXTURE_SURFACE_VIEW_PC {
    const GRAPHICS_TEXTURE_SURFACE_METHODS_PC *methods;
};
STATIC_ASSERT(offsetof(GRAPHICS_TEXTURE_SURFACE_VIEW_PC, methods) == 0);
STATIC_ASSERT(offsetof(GRAPHICS_TEXTURE_SURFACE_METHODS_PC, get_surface_level) == 0x48);
extern "C" void __cdecl PCDeviceLockIndexBuffer(u32 offset, u32 size, void **data, u32 flags,
                                                PC_BUFFER_RESOURCE_PC *resource) {
    PCDeviceLastResult = resource->methods->lock(resource, offset, size, data, flags);
}
extern "C" HRESULT __cdecl PCDeviceUnlockIndexBuffer(PC_BUFFER_RESOURCE_PC *resource) {
    return resource->methods->unlock(resource);
}
void __thiscall PC_DEVICE_BACKEND_PC::BuildDefaultPoolResources() {
    if (back_buffer != NULL) {
        back_buffer->methods->release(back_buffer);
        back_buffer = NULL;
    }
    if (depth_surface != NULL) {
        depth_surface->methods->release(depth_surface);
        depth_surface = NULL;
    }
    if (buffer690 != NULL) {
        buffer690->methods->release(buffer690);
        buffer690 = NULL;
    }
    if (buffer694 != NULL) {
        buffer694->methods->release(buffer694);
        buffer694 = NULL;
    }
    if (buffer698 != NULL) {
        buffer698->methods->release(buffer698);
        buffer698 = NULL;
    }
    if (buffer688 != NULL) {
        buffer688->methods->release(buffer688);
        buffer688 = NULL;
    }
    GRAPHICS_DEVICE_PC *view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    view->methods->get_back_buffer(view, 0, 0, 0, &back_buffer);
    view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
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
    if (target_count > 4)
        target_count = 4;
    for (u32 i = 0; i != target_count; ++i) {
        switch (target_kinds[i]) {
            case 0:
            case 1:
                textures[i] = PCGraphicsCreateColourTarget(1, 1);
                break;
            case 3:
                textures[i] = PCGraphicsCreateColourTargetFormat(1, 1, 10);
                break;
        }
        if (textures[i] != NULL) {
            GRAPHICS_TEXTURE_SURFACE_VIEW_PC *texture =
                reinterpret_cast<GRAPHICS_TEXTURE_SURFACE_VIEW_PC *>(textures[i]->resource);
            texture->methods->get_surface_level(texture, 0, &target_surfaces[i]);
        }
    }
    PC_GRAPHICS_CLIP_VERTEX_PC quad[4] = {{-1.0f, 1.0f, 0.0f, 0.0f, 0.0f},
                                          {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f},
                                          {1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
                                          {1.0f, -1.0f, 0.0f, 1.0f, 1.0f}};
    PC_GRAPHICS_SCREEN_VERTEX_PC triangle[3] = {{0.0f, 0.0f, 0.0f, 0.0f},
                                                {0.0f, (f32)((f64)parameters.height * 3.0), 0.0f, 3.0f},
                                                {(f32)((f64)parameters.width * 3.0), 0.0f, 3.0f, 0.0f}};
    PCDeviceCreateVertexBuffer(sizeof(quad), 8, 0, 0, &buffer690);
    PCDeviceCreateVertexBuffer(sizeof(triangle), 8, 0, 0, &buffer688);
    void *data = NULL;
    PCDeviceLockVertexBuffer(0, sizeof(quad), &data, 0, buffer690);
    memcpy(data, quad, sizeof(quad));
    PCDeviceUnlockVertexBuffer(buffer690);
    PCDeviceLockVertexBuffer(0, sizeof(triangle), &data, 0, buffer688);
    memcpy(data, triangle, sizeof(triangle));
    PCDeviceUnlockVertexBuffer(buffer688);
    state6a0 = 16;
    state69c = (i32)(u32)(((f64)PS2_REZ_H / PS2_REZ_W) * 16.0);
    state6a4 = (i32)(((u32)state69c + 1) * 17);
    state6a8 = (i32)((u32)state69c * 32);
    PCDeviceCreateVertexBuffer((u32)state6a4 * 20, 8, 0, 0, &buffer694);
    PCDeviceCreateIndexBuffer((u32)state6a8 * 6, 8, 101, 0, &buffer698, NULL);
    PCDeviceLockVertexBuffer(0, (u32)state6a4 * 20, &data, 0, buffer694);
    PC_GRAPHICS_CLIP_VERTEX_PC *vertex = static_cast<PC_GRAPHICS_CLIP_VERTEX_PC *>(data);
    for (u32 row = 0; row != (u32)state69c + 1; ++row) {
        for (u32 column = 0; column != (u32)state6a0 + 1; ++column) {
            f32 rounded_row = (f32)row;
            f32 rounded_column = (f32)column;
            vertex->x = (f32)((f64)rounded_column / (u32)state6a0 * 2.0 - 1.0);
            vertex->y = (f32)(1.0 - (f64)rounded_row / (u32)state69c * 2.0);
            vertex->z = 0.0f;
            vertex->u = (f32)((f64)rounded_column / (u32)state6a0);
            vertex->v = (f32)((f64)rounded_row / (u32)state69c);
            ++vertex;
        }
    }
    PCDeviceUnlockVertexBuffer(buffer694);
    PCDeviceLockIndexBuffer(0, (u32)state6a8 * 6, &data, 0, buffer698);
    u16 *index = static_cast<u16 *>(data);
    for (u32 row = 0; row != (u32)state69c; ++row) {
        for (u32 column = 0; column != (u32)state6a0; ++column) {
            u16 lower = (u16)(((u16)state6a0 + 1) * (u16)row + (u16)column);
            u16 upper = (u16)(((u16)state6a0 + 1) * (u16)(row + 1) + (u16)column);
            *index++ = lower;
            *index++ = (u16)(lower + 1);
            *index++ = upper;
            *index++ = upper;
            *index++ = (u16)(lower + 1);
            *index++ = (u16)(upper + 1);
        }
    }
    PCDeviceUnlockIndexBuffer(buffer698);
}

void __thiscall PC_DEVICE_BACKEND_PC::ConfigureTargetKinds() {
    PcWindowOptions.SetShadowMethod(PcWindowOptions.desiredshadowmethod);
    PcWindowOptions.SetBloomMethod(PcWindowOptions.desiredbloommethod);
    PcWindowOptions.SetSpeedBlurSupported(1);
    PcWindowOptions.SetShadowMethod(PcWindowOptions.usehires != 0 ? 2 : 1);
    if (PCDeviceHardwareVertexProcessingPC < 2) {
        if (PcWindowOptions.shadowsenabled != 0 && PcWindowOptions.shadowmethod > 1)
            PcWindowOptions.SetShadowMethod(1);
        if (PcWindowOptions.bloomenabled != 0 && PcWindowOptions.bloommethod > 1)
            PcWindowOptions.SetBloomMethod(1);
    }
    if (PCDeviceHardwareVertexProcessingPC <= 2 && PcWindowOptions.shadowsenabled != 0 &&
        PcWindowOptions.shadowmethod > 1)
        PcWindowOptions.SetShadowMethod(1);
    if (multisample_index != 0) {
        if (PcWindowOptions.shadowsenabled != 0 && PcWindowOptions.shadowmethod > 1)
            PcWindowOptions.SetShadowMethod(0);
        if (PcWindowOptions.bloomenabled != 0 && PcWindowOptions.bloommethod > 1)
            PcWindowOptions.SetBloomMethod(1);
    }
    shader_flags = 1;
    render_target_flags = 1;
    if (PcWindowOptions.shadowmethod == 2) {
        shader_flags = 0x21;
        render_target_flags = 0x13;
    }
    if (PcWindowOptions.bloommethod != 0)
        shader_flags |= 4;
    if (PcWindowOptions.bloommethod == 2)
        render_target_flags |= 2;
    if (PcWindowOptions.speedblursupported != 0)
        shader_flags |= 0x80;
    i32 count = 0;
    if (multisample_index == 0)
        target_kinds[count++] = 0;
    if ((render_target_flags & 2) != 0)
        target_kinds[count++] = 1;
    if ((render_target_flags & 4) != 0)
        target_kinds[count++] = 2;
    if ((render_target_flags & 0x10) != 0)
        target_kinds[count++] = 3;
    requested_target_count = (u16)count;
    PcWindowOptions.Save();
}

extern "C" i32 __cdecl PCGraphicsPrepareVendor() {
    u32 vendor = PCDeviceBackend.identifier.vendor;
    u32 device_id = PCDeviceBackend.identifier.device_id;
    PC_GRAPHICS_GPU_RECORD_PC *selected = NULL;
    PCGraphicsGpuSelectionEnabled = 1;
    PCGraphicsQueriedGpu.vendor_id = vendor;
    PCGraphicsQueriedGpu.device_id = device_id;
    if (vendor == 0x10de && device_id != 0xffffffffu) {
        for (PC_GRAPHICS_GPU_RECORD_PC *record = PCGraphicsGpuRecords; record->device_id != 0xffffffffu; ++record) {
            if (selected != NULL)
                break;
            if (record->device_id == device_id)
                selected = record;
        }
        if (selected == NULL)
            selected = &PCGraphicsGpuFallback;
    }
    PCGraphicsSelectedGpuRecord = selected;
    return 0;
}

extern "C" HRESULT __cdecl PCDeviceCreateVertexShader(const DWORD *tokens, PC_TEXTURE_RESOURCE_PC **shader) {
    HRESULT result = PCDeviceBackend.device->methods->create_vertex_shader(PCDeviceBackend.device, tokens, shader);
    PCDeviceLastResult = result;
    return result;
}
extern "C" HRESULT __cdecl PCDeviceCreatePixelShader(const DWORD *tokens, PC_TEXTURE_RESOURCE_PC **shader) {
    HRESULT result = PCDeviceBackend.device->methods->create_pixel_shader(PCDeviceBackend.device, tokens, shader);
    PCDeviceLastResult = result;
    return result;
}

void __thiscall PC_DEVICE_BACKEND_PC::RequestMode(u32 mode) {
    requested_mode = mode < mode_count ? mode : active_mode;
}

void __thiscall PC_DEVICE_BACKEND_PC::NextResolution() {
    u32 original = requested_mode;
    u32 index = original;
    u32 visited = 0;
    while (visited != mode_count && modes[index].width == modes[original].width &&
           modes[index].height == modes[original].height) {
        ++index;
        if (index == mode_count)
            index = 0;
        ++visited;
    }
    u32 selected = index;
    u32 reference = index;
    i32 closest = 1000;
    while (index < mode_count && modes[index].width == modes[reference].width &&
           modes[index].height == modes[reference].height) {
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

void __thiscall PC_DEVICE_BACKEND_PC::PreviousResolution() {
    u32 original = requested_mode;
    i32 index = (i32)original;
    u32 visited = 0;
    while (visited < mode_count && modes[index].width == modes[original].width &&
           modes[index].height == modes[original].height) {
        --index;
        if (index < 0)
            index = (i32)mode_count - 1;
        ++visited;
    }
    i32 selected = index;
    i32 reference = index;
    i32 closest = 1000;
    while (index >= 0 && modes[index].width == modes[reference].width &&
           modes[index].height == modes[reference].height) {
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

void __thiscall PC_DEVICE_BACKEND_PC::NextRefresh() {
    u32 original = requested_mode;
    u32 index = original;
    for (u32 visited = 0; visited < mode_count; ++visited) {
        if (modes[index].width == modes[original].width && modes[index].height == modes[original].height &&
            modes[index].refresh_rate != modes[original].refresh_rate) {
            requested_mode = index;
            return;
        }
        ++index;
        if (index >= mode_count)
            index = 0;
    }
}

void __thiscall PC_DEVICE_BACKEND_PC::PreviousRefresh() {
    u32 original = requested_mode;
    i32 index = (i32)original;
    for (u32 visited = 0; visited < mode_count; ++visited) {
        if (modes[index].width == modes[original].width && modes[index].height == modes[original].height &&
            modes[index].refresh_rate != modes[original].refresh_rate) {
            requested_mode = (u32)index;
            return;
        }
        --index;
        if (index < 0)
            index = (i32)mode_count - 1;
    }
}

void __thiscall PC_DEVICE_BACKEND_PC::ResetRequestedMode() {
    requested_windowed = parameters.windowed == 1;
    requested_mode = active_mode;
    requested_feature = feature634;
    requested_multisample = multisample_index;
}

STATIC_ASSERT(offsetof(GRAPHICS_DEVICE_METHODS_PC, stretch_rect) == 0x88);

void PC_DEVICE_BACKEND_PC::SetDepthStencilSurface(PC_EFFECT_SURFACE_PC *target) {
    GRAPHICS_DEVICE_PC *view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    if (target != NULL && target->surface != NULL)
        view->methods->set_depth_stencil_surface(view, target->surface);
    else
        view->methods->set_depth_stencil_surface(view, depth_surface);
}
void PC_DEVICE_BACKEND_PC::ClearDepthStencilSurface() {
    GRAPHICS_DEVICE_PC *view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    view->methods->set_depth_stencil_surface(view, NULL);
}
void PC_DEVICE_BACKEND_PC::RestoreDefaultPoolResources() {
    BuildDefaultPoolResources();
}
void PC_DEVICE_BACKEND_PC::SetSurfaceRenderTarget(PC_EFFECT_SURFACE_PC *target, u32 index) {
    GRAPHICS_DEVICE_PC *view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    if (target != NULL && target->surface != NULL) {
        SetRenderTarget((i32)index, target->surface);
        if (index == 0) {
            PS2_REZ_W = target->width;
            PS2_REZ_H = target->height;
            PS2_VCNTR_X = PS2_REZ_W / 2;
            PS2_VCNTR_Y = PS2_REZ_H / 2;
            ResetViewport();
        }
    } else if (index == 0) {
        if (multisample_index != 0) {
            SetRenderTarget(0, back_buffer);
            PCDeviceSetRenderState(168, 0);
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
    } else
        SetRenderTarget((i32)index, NULL);
}
void PC_DEVICE_BACKEND_PC::CopyBackBuffer(GRAPHICS_RENDER_TEXTURE_PC *texture) {
    PC_EFFECT_SURFACE_PC target;
    target.Attach(texture, 0, false);
    bool resize =
        PCGraphicsEffectTextureWidth(texture) != PS2_REZ_W || PCGraphicsEffectTextureHeight(texture) != PS2_REZ_H;
    PC_TEXTURE_RESOURCE_PC *source =
        multisample_index != 0 || target_surfaces[0] == NULL ? back_buffer : target_surfaces[0];
    GRAPHICS_DEVICE_PC *view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    view->methods->stretch_rect(view, source, NULL, target.surface, NULL, resize ? 2 : 1);
}
void PC_DEVICE_BACKEND_PC::CopyCurrentSurface(GRAPHICS_RENDER_TEXTURE_PC *texture) {
    PC_EFFECT_SURFACE_PC target;
    target.Attach(texture, 0, false);
    bool resize =
        PCGraphicsEffectTextureWidth(texture) != PS2_REZ_W || PCGraphicsEffectTextureHeight(texture) != PS2_REZ_H;
    GRAPHICS_DEVICE_PC *view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    view->methods->stretch_rect(view, current_surface, NULL, target.surface, NULL, resize ? 2 : 1);
}

extern "C" void __cdecl PCDeviceSetVertexShaderConstantsF(UINT start, const f32 *values, UINT count) {
    PCDeviceLastResult =
        PCDeviceBackend.device->methods->set_vertex_shader_constant_f(PCDeviceBackend.device, start, values, count);
}
extern "C" void __cdecl PCDeviceSetPixelShaderConstantsF(UINT start, const f32 *values, UINT count) {
    PCDeviceLastResult =
        PCDeviceBackend.device->methods->set_pixel_shader_constant_f(PCDeviceBackend.device, start, values, count);
}

HRESULT __thiscall PC_DEVICE_BACKEND_PC::CopySurfaceTargetPc(PC_EFFECT_SURFACE_PC *target, i32 filter, i32 unused) {
    (void)unused;
    GRAPHICS_DEVICE_PC *view = reinterpret_cast<GRAPHICS_DEVICE_PC *>(device);
    return view->methods->stretch_rect(view, current_surface, NULL, target->surface, NULL, filter);
}
extern "C" HRESULT __cdecl PCDeviceEndScenePc(void) {
    return PCDeviceBackend.EndScene();
}

extern "C" void __cdecl PCDeviceSetStreamSourcePc(u32 stream, PC_BUFFER_RESOURCE_PC *buffer, u32 offset, u32 stride) {
    if (PCDeviceStateCacheEnabled != 0) {
        if (PCDeviceVertexBufferCachePc == buffer && PCDeviceStreamStrideCachePc == stride &&
            PCDeviceStreamNumberCachePc == stream && PCDeviceStreamOffsetCachePc == offset)
            return;
        PCDeviceVertexBufferCachePc = buffer;
        PCDeviceStreamNumberCachePc = stream;
        PCDeviceStreamStrideCachePc = stride;
        PCDeviceStreamOffsetCachePc = offset;
    }
    PCDeviceLastResult =
        PCDeviceBackend.device->methods->set_stream_source(PCDeviceBackend.device, stream, buffer, offset, stride);
}

extern "C" void __cdecl PCDeviceDrawPrimitivePc(i32 type, u32 start, u32 count) {
    ++PCDevicePrimitiveCallCountPc;
    PCDeviceLastResult = PCDeviceBackend.device->methods->draw_primitive(PCDeviceBackend.device, type, start, count);
}

extern "C" void __cdecl PCDeviceSetIndicesPc(PC_BUFFER_RESOURCE_PC *buffer) {
    if (PCDeviceStateCacheEnabled != 0) {
        if (PCDeviceIndexBufferCachePc == buffer)
            return;
        PCDeviceIndexBufferCachePc = buffer;
    }
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_indices(PCDeviceBackend.device, buffer);
}

extern "C" void __cdecl PCDeviceDrawIndexedPrimitivePc(i32 type, i32 base_vertex, u32 minimum_vertex, u32 vertex_count,
                                                       u32 start_index, u32 primitive_count) {
    ++PCDeviceIndexedPrimitiveCallCountPc;
    PCDeviceLastResult = PCDeviceBackend.device->methods->draw_indexed_primitive(
        PCDeviceBackend.device, type, base_vertex, minimum_vertex, vertex_count, start_index, primitive_count);
}

extern "C" void __cdecl PCDeviceSetStreamSourceFreqPc(u32 stream, u32 setting) {
    PCDeviceLastResult =
        PCDeviceBackend.device->methods->set_stream_source_freq(PCDeviceBackend.device, stream, setting);
}

extern "C" void __cdecl PCDeviceSetVertexDeclarationPc(PC_TEXTURE_RESOURCE_PC *declaration) {
    if (PCDeviceStateCacheEnabled != 0) {
        if (PCDeviceVertexDeclarationCachePc == declaration)
            return;
        PCDeviceVertexDeclarationCachePc = declaration;
    }
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_vertex_declaration(PCDeviceBackend.device, declaration);
}

extern "C" void __cdecl PCDeviceSetVertexShaderPc(PC_TEXTURE_RESOURCE_PC *shader) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_vertex_shader(PCDeviceBackend.device, shader);
}

extern "C" void __cdecl PCDeviceSetPixelShaderPc(PC_TEXTURE_RESOURCE_PC *shader) {
    PCDeviceLastResult = PCDeviceBackend.device->methods->set_pixel_shader(PCDeviceBackend.device, shader);
}

void __thiscall PC_DEVICE_BACKEND_PC::SetColorWriteMaskPc(u8 colour_enabled, u8 alpha_enabled, i32 target) {
    u32 disabled;
    if (target != 0 && target_count == 1)
        return;
    disabled = colour_enabled != 0 ? 0u : 7u;
    if (alpha_enabled == 0)
        disabled |= 8u;
    switch (target) {
        case 0:
            PCDeviceSetRenderState(168, disabled);
            break;
        case 1:
            PCDeviceSetRenderState(190, disabled);
            break;
        case 3:
            PCDeviceSetRenderState(191, disabled);
            break;
    }
}

extern "C" void __cdecl NuPs2GetViewport(NUVPCURRENT *viewport) {
    if (viewport != NULL) {
        viewport->x = PCDeviceBackend.requested_viewport.x;
        viewport->y = PCDeviceBackend.requested_viewport.y;
        viewport->width = PCDeviceBackend.requested_viewport.width;
        viewport->height = PCDeviceBackend.requested_viewport.height;
        memcpy(&viewport->min_z, &PCDeviceBackend.requested_viewport.min_z, sizeof(viewport->min_z));
        memcpy(&viewport->max_z, &PCDeviceBackend.requested_viewport.max_z, sizeof(viewport->max_z));
    }
}
