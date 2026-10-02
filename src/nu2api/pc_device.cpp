#include "saga.h"
#include <string.h>

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
