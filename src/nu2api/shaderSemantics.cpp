#include "nu2api/nu3d/ShaderManager.h"
#include "nu2api/nu3d/nushader.h"
#include "nu2api/nu3d/shaderSemantics.h"
#include <string.h>

extern "C" {

    ShaderUniformRecord *NuShaderUniformGetByString(const char *name) {
        i32 i;
        for (i = 0; i < 100; ++i) {
            if ((ShaderUniformRecords[i].fragment_name != NULL && strcmp(ShaderUniformRecords[i].fragment_name, name) == 0) ||
                (ShaderUniformRecords[i].vertex_name != NULL && strcmp(ShaderUniformRecords[i].vertex_name, name) == 0)) {
                return &ShaderUniformRecords[i];
            }
        }
        return NULL;
    }
}

i32 PC_SHADER_MATERIAL_FILTER::fun_007442d0() const {
    return (descriptor->vertex_format & 0x40000) != 0;
}

bool PC_SHADER_MATERIAL_FILTER::HasVertexSkin() const {
    return (descriptor->vertex_format & 0x30000) != 0 && (descriptor->vertex_format & 0x400000) == 0;
}

bool PC_SHADER_MATERIAL_FILTER::HasVertexSkinAlt() const {
    return (descriptor->vertex_format & 0x30000) != 0 && (descriptor->vertex_format & 0x400000) != 0;
}

i32 PC_SHADER_MATERIAL_FILTER::fun_00744320() const {
    return (descriptor->vertex_format & 0x80000) != 0;
}

i32 PC_SHADER_MATERIAL_FILTER::fun_00744330() const {
    return (descriptor->vertex_format & 0x1000000) != 0;
}

bool PC_SHADER_MATERIAL_FILTER::fun_00744340() const {
    return descriptor->shadow_set != 0;
}

i32 PC_SHADER_MATERIAL_FILTER::fun_00744350(i32 layer) const {
    if (layer == 0) {
        return descriptor->shadow_set & 1;
    }
    return 0;
}

i32 PC_SHADER_MATERIAL_FILTER::fun_00744370() const {
    return variant == 0;
}

i32 PC_SHADER_MATERIAL_FILTER::fun_00744380() const {
    return (descriptor->vertex_format & 0x2000000) != 0;
}

i32 PC_SHADER_MATERIAL_FILTER::fun_00744390() const {
    if (variant != 0) {
        return 0;
    }
    return (descriptor->layer_flags & PC_SHADER_LAYER_PRELIGHT_FX) != 0;
}

i32 PC_SHADER_MATERIAL_FILTER::fun_007443b0() const {
    if (variant != 0) {
        return 0;
    }
    return (descriptor->layer_flags & PC_SHADER_LAYER_GLOW) != 0;
}
