#ifndef SAGA_NU2API_NU3D_NUSHADER_PLAIN_H_PART
#define SAGA_NU2API_NU3D_NUSHADER_PLAIN_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: decomp.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "decomp.h"
#endif
/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NU3D_NUSHADER_PLAIN_H_PART == 1 && !defined(SAGA_NU2API_NU3D_NUSHADER_PLAIN_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void NuShaderManagerInit(VARIPTR *arena, VARIPTR arena_end);
    PC_SHADER_OBJECT_FULL_VIEW *NuShaderManagerRetrieveShader(const PC_SHADER_FILTER_DESC_VIEW *,
                                                              const PC_SHADER_FILTER_MATERIAL_VIEW *);
    PC_SHADER_OBJECT_FULL_VIEW *NuShaderManagerRetrieveShaderVariant(const PC_SHADER_FILTER_DESC_VIEW *,
                                                                     const PC_SHADER_FILTER_MATERIAL_VIEW *, i32);
    void NuShaderManagerReleaseShader(PC_SHADER_RECORD_VIEW *);
    void NuShaderManagerSetfv(i32 semantic, const f32 *values);

    void NuShaderManagerSetElementfv(i32 semantic, i32 element, const f32 *values);
    extern char g_shaderSaveFolder[256];
    void NuShaderManagerSetShaderSaveFolder(const char *folder);
    void NuShaderManagerSetElementsfv_transpose(i32 semantic, i32 first_element, i32 count, const f32 *values);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NUSHADER_PLAIN_H_PART
#define SAGA_NU2API_NU3D_NUSHADER_PLAIN_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NUSHADER_PLAIN_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_NUSHADER_PLAIN_H_PART == 2 && !defined(SAGA_NU2API_NU3D_NUSHADER_PLAIN_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void NuShaderManagerBindShader(PC_SHADER_OBJECT_FULL_VIEW *);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NUSHADER_PLAIN_H_PART
#define SAGA_NU2API_NU3D_NUSHADER_PLAIN_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NUSHADER_PLAIN_H_STOP
#endif
#endif

#undef SAGA_NU2API_NU3D_NUSHADER_PLAIN_H_STOP
