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
