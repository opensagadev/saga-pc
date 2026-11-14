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

bool PC_SHADER_MATERIAL_FILTER_VIEW::HasVertexSkin() const {
    return (descriptor->vertex_format & 0x30000) != 0 && (descriptor->vertex_format & 0x400000) == 0;
}

bool PC_SHADER_MATERIAL_FILTER_VIEW::HasVertexSkinAlt() const {
    return (descriptor->vertex_format & 0x30000) != 0 && (descriptor->vertex_format & 0x400000) != 0;
}
