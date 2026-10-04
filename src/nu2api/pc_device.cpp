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
    void *unknown_0010[38];
    HRESULT(__stdcall *end_scene)(GRAPHICS_DEVICE_PC *);
    HRESULT(__stdcall *clear)(GRAPHICS_DEVICE_PC *, u32, const RECT *, u32, u32, f32, u32);
};
struct GRAPHICS_DEVICE_PC {
    const GRAPHICS_DEVICE_METHODS_PC *methods;
};
STATIC_ASSERT(offsetof(GRAPHICS_DEVICE_METHODS_PC, test_cooperative_level) == 0xc);
STATIC_ASSERT(offsetof(GRAPHICS_DEVICE_METHODS_PC, end_scene) == 0xa8);
STATIC_ASSERT(offsetof(GRAPHICS_DEVICE_METHODS_PC, clear) == 0xac);

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
