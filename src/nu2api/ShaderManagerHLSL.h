#pragma once

#include "nu2api/nucore/common.h"

#include "gamelib/util/CRC16.h"
#include "nu2api/nu3d/ShaderManager.h"
#include "nu2api/nu3d/ShaderObjectKey.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern u32 CRC16TablePC[256];
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

class ShaderManagerHLSL : public PC_SHADER_MANAGER_VIEW { // size 0x791b0
  public:
    ShaderCompilerBasePc *compiler;                // 0x7918c
    ShaderCompilerBufferAllocatorPc vertex_source; // 0x79190
    ShaderCompilerBufferAllocatorPc pixel_source;  // 0x791a0
    ShaderManagerHLSL(VirtualStackAllocator &);
    void SetUniformValues(i32 semantic, const f32 *values);
    void setElementfv(i32 semantic, i32 element, const f32 *values);
    void setElementsfv(i32 semantic, i32 first_element, i32 count, const f32 *values);
    void setElementsfv_transpose(i32 semantic, i32 first_element, i32 count, const f32 *values);
    void fun_007459a0(i32 flags, i32 selector, const char *vertex_text, i32 vertex_length, const char *pixel_text, i32 pixel_length);
    virtual ~ShaderManagerHLSL();
    virtual bool CreateShader(const PC_SHADER_MATERIAL_FILTER_VIEW *, bool, NuShaderObject *, i32);
    virtual void AdaptDescriptor(const PC_SHADER_FILTER_DESC_VIEW *, const PC_SHADER_FILTER_MATERIAL_VIEW *);
};

typedef ShaderManagerHLSL ShaderManagerHLSLPc;
#endif

#ifdef __cplusplus
extern char uberShader2[158576];
#endif
