#include "saga.h"
#include <string.h>

/* Retail unit 0x744210-0x74431f: definitions in retail address order. */

extern "C" {

    ShaderUniformRecordPc *NuShaderUniformGetByString(const char *name) {
        i32 i;
        for (i = 0; i < 100; ++i) {
            if ((ShaderUniformRecordsPc[i].fragment_name != NULL &&
                 strcmp(ShaderUniformRecordsPc[i].fragment_name, name) == 0) ||
                (ShaderUniformRecordsPc[i].vertex_name != NULL &&
                 strcmp(ShaderUniformRecordsPc[i].vertex_name, name) == 0)) {
                return &ShaderUniformRecordsPc[i];
            }
        }
        return NULL;
    }
}

i32 PC_SHADER_MATERIAL_FILTER_VIEW::fun_007442d0() const {
    return (descriptor->vertex_format & 0x40000) != 0;
}

bool PC_SHADER_MATERIAL_FILTER_VIEW::HasVertexSkin() const {
    return (descriptor->vertex_format & 0x30000) != 0 && (descriptor->vertex_format & 0x400000) == 0;
}

bool PC_SHADER_MATERIAL_FILTER_VIEW::HasVertexSkinAlt() const {
    return (descriptor->vertex_format & 0x30000) != 0 && (descriptor->vertex_format & 0x400000) != 0;
}

i32 PC_SHADER_MATERIAL_FILTER_VIEW::fun_00744320() const {
    return (descriptor->vertex_format & 0x80000) != 0;
}

i32 PC_SHADER_MATERIAL_FILTER_VIEW::fun_00744330() const {
    return (descriptor->vertex_format & 0x1000000) != 0;
}

bool PC_SHADER_MATERIAL_FILTER_VIEW::fun_00744340() const {
    return descriptor->override_mode != 0;
}

i32 PC_SHADER_MATERIAL_FILTER_VIEW::fun_00744350(i32 layer) const {
    if (layer == 0) {
        return descriptor->override_mode & 1;
    }
    return 0;
}

i32 PC_SHADER_MATERIAL_FILTER_VIEW::fun_00744370() const {
    return variant == 0;
}

i32 PC_SHADER_MATERIAL_FILTER_VIEW::fun_00744380() const {
    return (descriptor->vertex_format & 0x2000000) != 0;
}

i32 PC_SHADER_MATERIAL_FILTER_VIEW::fun_00744390() const {
    if (variant != 0) {
        return 0;
    }
    return (descriptor->layer_flags & 0x80000000) != 0;
}

i32 PC_SHADER_MATERIAL_FILTER_VIEW::fun_007443b0() const {
    if (variant != 0) {
        return 0;
    }
    return (descriptor->layer_flags & 0x10000) != 0;
}
