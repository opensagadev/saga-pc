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
