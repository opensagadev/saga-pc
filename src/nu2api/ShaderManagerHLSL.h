/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "decomp.h"
#include "gamelib/util/CRC16.h"
#include "gamelib/util/gamelib_util_types.h"
#include "nu2api/nu3d/ShaderManagerOpenGL.h"
#include "nu2api/nu3d/ShaderObjectKey.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern u32 CRC16TablePC[256];
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

class ShaderManagerHLSL : public PC_SHADER_MANAGER_VIEW {
  public:
    ShaderCompilerBasePc *compiler;
    ShaderCompilerBufferAllocatorPc vertex_source;
    ShaderCompilerBufferAllocatorPc pixel_source;
    ShaderManagerHLSL(VirtualStackAllocator &);
    void SetUniformValues(i32 semantic, const f32 *values);
    void setElementfv(i32 semantic, i32 element, const f32 *values);
    void setElementsfv(i32 semantic, i32 first_element, i32 count, const f32 *values);
    void setElementsfv_transpose(i32 semantic, i32 first_element, i32 count, const f32 *values);
    void fun_007459a0(i32 flags, i32 selector, const char *vertex_text, i32 vertex_length, const char *pixel_text,
                      i32 pixel_length);
    virtual ~ShaderManagerHLSL();
    virtual bool CreateShader(const PC_SHADER_MATERIAL_FILTER_VIEW *, bool, PC_SHADER_OBJECT_FULL_VIEW *, i32);
    virtual void AdaptDescriptor(const PC_SHADER_FILTER_DESC_VIEW *, const PC_SHADER_FILTER_MATERIAL_VIEW *);
};

typedef ShaderManagerHLSL ShaderManagerHLSLPc;
STATIC_ASSERT(offsetof(ShaderManagerHLSLPc, compiler) == 0x7918c);
STATIC_ASSERT(offsetof(ShaderManagerHLSLPc, vertex_source) == 0x79190);
STATIC_ASSERT(offsetof(ShaderManagerHLSLPc, pixel_source) == 0x791a0);
STATIC_ASSERT(sizeof(ShaderManagerHLSLPc) == 0x791b0);
#endif
