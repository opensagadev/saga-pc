#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nushaderobject.h"

#ifdef __cplusplus
extern "C" {
#endif
    void NuShaderObjectBaseUnInit(NuShaderObject *);
    void NuShaderObjectUnInit(NuShaderObject *);
    void NuShaderObjectBaseSetWaterSpeed(f32);

#ifdef __cplusplus
}
#endif

typedef struct shader_uniform_record_pc_s { // size 0x5c
    const char *vertex_name;                // 0x00
    const char *fragment_name;              // 0x04
    i32 value_count;                        // 0x08
    i32 field_c;                            // 0x0c
    i32 stage_mask;                         // 0x10
    i32 register_index;                     // 0x14
    i32 unknown_18;                         // 0x18
    f32 values[16];                         // 0x1c
} ShaderUniformRecord;

#ifdef __cplusplus
extern "C" {
#endif
    ShaderUniformRecord *NuShaderUniformGetByString(const char *name);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void NuShaderObjectBaseInit(NuShaderObject *, const u32 *, i32);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void NuShaderObjectSetElementsfv(NuShaderObject *, i32, i32, i32, const f32 *);

    void NuShaderObjectBaseCreate(NuShaderObject *);
    void NuShaderObjectBaseDestroy(NuShaderObject *);
    void NuShaderObjectCreate(NuShaderObject *);
    void NuShaderObjectDestroy(NuShaderObject *);
    void NuShaderObjectSetElementsfv_transpose(NuShaderObject *shader, i32 semantic, i32 first_element, i32 count, const f32 *values);
#ifdef __cplusplus
}
#endif
