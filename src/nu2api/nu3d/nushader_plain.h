#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/ShaderManager.h"
#include "nu2api/nu3d/nushadermanager_gen.h"
#include "nu2api/nu3d/nushaderobject.h"

#ifdef __cplusplus
extern "C" {
#endif
    void NuShaderManagerInit(VARIPTR *arena, VARIPTR arena_end);
    NuShaderObject *NuShaderManagerRetrieveShader(const PC_SHADER_FILTER_DESC_VIEW *, const PC_SHADER_FILTER_MATERIAL_VIEW *);
    NuShaderObject *NuShaderManagerRetrieveShaderVariant(const PC_SHADER_FILTER_DESC_VIEW *, const PC_SHADER_FILTER_MATERIAL_VIEW *, i32);
    void NuShaderManagerReleaseShader(PC_SHADER_RECORD_VIEW *);
    void NuShaderManagerSetfv(i32 semantic, const f32 *values);

    void NuShaderManagerSetElementfv(i32 semantic, i32 element, const f32 *values);
    extern char g_shaderSaveFolder[256];
    void NuShaderManagerSetShaderSaveFolder(const char *folder);
    void NuShaderManagerSetElementsfv_transpose(i32 semantic, i32 first_element, i32 count, const f32 *values);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void NuShaderManagerBindShader(NuShaderObject *);
#ifdef __cplusplus
}
#endif
