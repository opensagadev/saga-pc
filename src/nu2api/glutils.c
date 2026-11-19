#include "nu2api/nu3d/glutils.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nualloc.h"
#include "nu2api/nudynamiclight_gen.h"
#include "nu2api/numath/nuvec.h"

NUMTL *CreateAlphaBlendTexture(VARIPTR *buffer, VARIPTR buffer_end, char *name, i32 disable_depth_write, i32 alpha_mode, i32 sort_priority,
                               i32 depth_mode) {
    i32 texture_id = NuTexRead(name, buffer, buffer_end);
    NUMTL *material;
    buffer->addr = (buffer->addr + 15) & ~15u;
    material = NuMtlCreate3D(1);
    material->diffuse.x = 1.0f;
    material->diffuse.y = 1.0f;
    material->diffuse.z = 1.0f;
    material->opacity = 0.999f;
    material->attribs.cull_mode = NUMTL_CULL_NONE;
    material->attribs.z_mode = (u32)depth_mode & 3;
    material->attribs.lighting = NUMTL_LIGHTING_NONE;
    material->attribs.alpha_mode = (u32)alpha_mode & 15;

    if (disable_depth_write != 0) {
        material->disable_depth_write = 1;
    }
    material->texture_id = (i16)texture_id;
    material->sort_priority = (i16)sort_priority;
    NuMtlUpdate(material);
    return material;
}
