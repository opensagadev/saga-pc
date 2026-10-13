#include "saga.h"

void __cdecl NuShaderObjectBaseUnInit(PC_SHADER_OBJECT_FULL_VIEW *shader) {
    shader->marker = -1;
}

void __cdecl NuShaderObjectBaseCreate(PC_SHADER_OBJECT_FULL_VIEW *shader) {
    shader->marker = -1;
    shader->ref_count = 0;
    shader->unknown_c = 0;
    shader->key = 0;
}
void __cdecl NuShaderObjectBaseDestroy(PC_SHADER_OBJECT_FULL_VIEW *shader) {
    shader->ref_count = 0;
}

void __cdecl NuShaderObjectBaseInit(PC_SHADER_OBJECT_FULL_VIEW *shader, const u32 *key, i32 index) {
    shader->key = *key;
    shader->marker = index;
}
