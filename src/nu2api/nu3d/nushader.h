#ifndef SAGA_NU2API_NU3D_NUSHADER_H_PART
#define SAGA_NU2API_NU3D_NUSHADER_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "decomp.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NU3D_NUSHADER_H_PART == 1 && !defined(SAGA_NU2API_NU3D_NUSHADER_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void NuShaderObjectBaseUnInit(PC_SHADER_OBJECT_FULL_VIEW *);
    void NuShaderObjectUnInit(PC_SHADER_OBJECT_FULL_VIEW *);
    void NuShaderObjectBaseSetWaterSpeed(f32);

#ifdef __cplusplus
}
#endif

typedef struct shader_uniform_record_pc_s {
    const char *vertex_name;
    const char *fragment_name;
    i32 value_count;
    i32 field_c;
    i32 stage_mask;
    i32 register_index;
    i32 field_18;
    f32 values[16];
} ShaderUniformRecordPc;

STATIC_ASSERT(sizeof(ShaderUniformRecordPc) == 0x5c);
STATIC_ASSERT(offsetof(ShaderUniformRecordPc, vertex_name) == 0);
STATIC_ASSERT(offsetof(ShaderUniformRecordPc, fragment_name) == 4);
STATIC_ASSERT(offsetof(ShaderUniformRecordPc, value_count) == 8);
STATIC_ASSERT(offsetof(ShaderUniformRecordPc, field_c) == 12);
STATIC_ASSERT(offsetof(ShaderUniformRecordPc, stage_mask) == 16);
STATIC_ASSERT(offsetof(ShaderUniformRecordPc, register_index) == 20);
STATIC_ASSERT(offsetof(ShaderUniformRecordPc, field_18) == 24);
STATIC_ASSERT(offsetof(ShaderUniformRecordPc, values) == 28);
#ifdef __cplusplus
extern "C" {
#endif
    ShaderUniformRecordPc *NuShaderUniformGetByString(const char *name);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NUSHADER_H_PART
#define SAGA_NU2API_NU3D_NUSHADER_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NUSHADER_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_NUSHADER_H_PART == 2 && !defined(SAGA_NU2API_NU3D_NUSHADER_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void NuShaderObjectBaseInit(PC_SHADER_OBJECT_FULL_VIEW *, const u32 *, i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NUSHADER_H_PART
#define SAGA_NU2API_NU3D_NUSHADER_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NUSHADER_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_NUSHADER_H_PART == 3 && !defined(SAGA_NU2API_NU3D_NUSHADER_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void NuShaderObjectSetElementsfv(PC_SHADER_OBJECT_FULL_VIEW *, i32, i32, i32, const f32 *);

    void NuShaderObjectBaseCreate(PC_SHADER_OBJECT_FULL_VIEW *);
    void NuShaderObjectBaseDestroy(PC_SHADER_OBJECT_FULL_VIEW *);
    void NuShaderObjectCreate(PC_SHADER_OBJECT_FULL_VIEW *);
    void NuShaderObjectDestroy(PC_SHADER_OBJECT_FULL_VIEW *);
    void NuShaderObjectSetElementsfv_transpose(PC_SHADER_OBJECT_FULL_VIEW *shader, i32 semantic, i32 first_element,
                                               i32 count, const f32 *values);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NUSHADER_H_PART
#define SAGA_NU2API_NU3D_NUSHADER_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NUSHADER_H_STOP
#endif
#endif

#undef SAGA_NU2API_NU3D_NUSHADER_H_STOP
