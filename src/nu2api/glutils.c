#include "saga.h"

NUMTL *CreateAlphaBlendTexture(VARIPTR *buffer, VARIPTR buffer_end, char *name, i32 disable_depth_write, i32 alpha_mode, i32 sort_priority,
                               i32 depth_mode) {
    i32 texture_id = NuTexRead(name, buffer, buffer_end);
    DEBRIS_MATERIAL *material;
    buffer->addr = (buffer->addr + 15) & ~15u;
    material = (DEBRIS_MATERIAL *)NuMtlCreate3D(1);
    material->diffuse.x = 1.0f;
    material->diffuse.y = 1.0f;
    material->diffuse.z = 1.0f;
    material->opacity = 0.999f;
    material->attribs.cull_mode = 2;
    material->attribs.z_mode = (u32)depth_mode & 3;
    material->attribs.unknown_2_1_2 = 2;
    material->attribs.alpha_mode = (u32)alpha_mode & 15;

    if (disable_depth_write != 0) {
        material->secondary.disable_depth_write = 1;
    }
    material->texture_id = (i16)texture_id;
    material->sort_priority = (i16)sort_priority;
    NuMtlUpdate((NUMTL *)material);
    return (NUMTL *)material;
}
