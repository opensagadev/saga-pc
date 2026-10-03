#include "saga.h"

struct PC_SHADER_OBJECT_FULL_VIEW {
    i32 marker;
    i32 ref_count;
    u32 key;
    u8 unknown_c[0x4c0];
    PC_TEXTURE_RESOURCE_PC *vertex_shader;
    PC_TEXTURE_RESOURCE_PC *pixel_shader;
    u32 unknown_4d4;
};
STATIC_ASSERT(sizeof(PC_SHADER_OBJECT_FULL_VIEW) == 0x4d8);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, vertex_shader) == 0x4cc);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, pixel_shader) == 0x4d0);

void __cdecl NuShaderObjectUnInit(PC_SHADER_OBJECT_FULL_VIEW *shader) {
    NuShaderObjectBaseUnInit(shader);
}
