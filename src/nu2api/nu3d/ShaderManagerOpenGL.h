/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_NU2API_NU3D_SHADERMANAGEROPENGL_H_PART
#define SAGA_NU2API_NU3D_SHADERMANAGEROPENGL_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "decomp.h"
#include "nu2api/nu3d/android/nushaderobject.h"
#include "nu2api/nu3d/nushaderobjectbase.h"
#endif

/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NU3D_SHADERMANAGEROPENGL_H_PART == 1 && !defined(SAGA_NU2API_NU3D_SHADERMANAGEROPENGL_H_STOP)

struct PC_SHADER_FILTER_DESC_VIEW;
struct PC_SHADER_FILTER_MATERIAL_VIEW;
struct PC_SHADER_MATERIAL_FILTER_VIEW;
struct PC_TEXTURE_RESOURCE_PC;

#ifdef __cplusplus
#ifdef __cplusplus
extern "C" {
#endif
    class NuShaderObject;
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    void NuShaderObjectKeyGenerate3(u32 *, const struct PC_SHADER_MATERIAL_FILTER_VIEW *, i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_SHADERMANAGEROPENGL_H_PART
#define SAGA_NU2API_NU3D_SHADERMANAGEROPENGL_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_SHADERMANAGEROPENGL_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_SHADERMANAGEROPENGL_H_PART == 2 && !defined(SAGA_NU2API_NU3D_SHADERMANAGEROPENGL_H_STOP)
#ifdef __cplusplus

class NuShaderObject {
  public:
    NuShaderObject() {
        PCShaderObjectCreateHLSLPc(this);
    }

    ~NuShaderObject() {
        PCShaderObjectDestroyHLSLPc(this);
    }

    i32 marker;
    i32 ref_count;
    u32 key;
    u32 unknown_c;
    PC_SHADER_PARAMETER_LOCAL_PC *active_parameters;
    i32 version;
    u32 field_18;
    PC_SHADER_PARAMETER_LOCAL_PC parameters[100];
    struct PC_TEXTURE_RESOURCE_PC *vertex_shader;
    struct PC_TEXTURE_RESOURCE_PC *pixel_shader;
    u32 unknown_4d4;

    void fun_00741250();
    bool fun_00741260() const;
    void fun_00741270();
    void fun_007412a0();
    void fun_007412b0();
    i32 fun_007412c0() const;
    u32 *fun_007412d0();
    void fun_00745470(const u32 *key, i32 index, const char *vertex_source, i32 vertex_length, const char *pixel_source,
                      i32 pixel_length, i32 version);
    i32 fun_007454a0();
};

template <class T> class ShaderManagerTemplate {
  public:
    ShaderManagerTemplate();
    virtual ~ShaderManagerTemplate();
    virtual bool CreateShader(const struct PC_SHADER_MATERIAL_FILTER_VIEW *, bool, PC_SHADER_OBJECT_FULL_VIEW *, i32);
    virtual void AdaptDescriptor(const struct PC_SHADER_FILTER_DESC_VIEW *,
                                 const struct PC_SHADER_FILTER_MATERIAL_VIEW *);
    T slots[400];
    i32 last_allocated;
    PC_SHADER_OBJECT_FULL_VIEW *bound_slot;
    PC_SHADER_OBJECT_FULL_VIEW *AllocateSlot(i32 *index);
    PC_SHADER_OBJECT_FULL_VIEW *Retrieve(const struct PC_SHADER_FILTER_DESC_VIEW *,
                                         const struct PC_SHADER_FILTER_MATERIAL_VIEW *, i32, i32, bool);
    void ReleaseShader(T *slot);
    void DestroyShaders();
    void SetCurrentShader(T *slot);
    T *GetCurrentShader();
    T *GetShader(u32 index);
    void BindShader(T *slot);
    static void SetShininessFactor(f32 shininess);
    static f32 GetShininessFactor();
    T *FindShader(const u32 *key);
};
#endif
#undef SAGA_NU2API_NU3D_SHADERMANAGEROPENGL_H_PART
#define SAGA_NU2API_NU3D_SHADERMANAGEROPENGL_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_SHADERMANAGEROPENGL_H_STOP
#endif
#endif

#undef SAGA_NU2API_NU3D_SHADERMANAGEROPENGL_H_STOP
