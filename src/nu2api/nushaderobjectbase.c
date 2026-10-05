#include "saga.h"

typedef struct PC_SHADER_PARAMETER_LOCAL_PC {
    u8 unknown_0[6];
    u8 semantic;
    u8 unknown_7[5];
} PC_SHADER_PARAMETER_LOCAL_PC;
STATIC_ASSERT(sizeof(PC_SHADER_PARAMETER_LOCAL_PC) == 12);
STATIC_ASSERT(offsetof(PC_SHADER_PARAMETER_LOCAL_PC, semantic) == 6);
struct PC_SHADER_OBJECT_FULL_VIEW {
    i32 marker;
    i32 ref_count;
    u32 key;
    u32 unknown_c;
    u32 field_10;
    u32 field_14;
    u32 field_18;
    PC_SHADER_PARAMETER_LOCAL_PC parameters[100];
    PC_TEXTURE_RESOURCE_PC *vertex_shader;
    PC_TEXTURE_RESOURCE_PC *pixel_shader;
    u32 stage_flags;
};
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, ref_count) == 4);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, key) == 8);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, unknown_c) == 0xc);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, field_10) == 0x10);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, field_14) == 0x14);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, field_18) == 0x18);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, parameters) == 0x1c);
STATIC_ASSERT(sizeof(PC_SHADER_OBJECT_FULL_VIEW) == 0x4d8);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, vertex_shader) == 0x4cc);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, pixel_shader) == 0x4d0);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, stage_flags) == 0x4d4);

void __cdecl NuShaderObjectBaseUnInit(PC_SHADER_OBJECT_FULL_VIEW *shader) {
    shader->marker = -1;
}

void __cdecl NuShaderObjectBaseCreate(PC_SHADER_OBJECT_FULL_VIEW *shader) {
    shader->marker = -1;
    shader->ref_count = 0;
    shader->key = 0;
    shader->unknown_c = 0;
}
void __cdecl NuShaderObjectBaseDestroy(PC_SHADER_OBJECT_FULL_VIEW *shader) {
    shader->ref_count = 0;
}
