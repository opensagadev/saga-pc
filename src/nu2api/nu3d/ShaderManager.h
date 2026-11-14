/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_NU2API_NU3D_SHADERMANAGER_H_PART
#define SAGA_NU2API_NU3D_SHADERMANAGER_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "decomp.h"
#include "nu2api/nu3d/nushaderobject.h"
#include "nu2api/nu3d/nushaderobjectbase.h"
#endif

/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NU3D_SHADERMANAGER_H_PART == 1 && !defined(SAGA_NU2API_NU3D_SHADERMANAGER_H_STOP)

typedef struct PC_SHADER_FILTER_DESC_VIEW PC_SHADER_FILTER_DESC_VIEW;
typedef struct PC_SHADER_FILTER_MATERIAL_VIEW PC_SHADER_FILTER_MATERIAL_VIEW;

struct PC_SHADER_FILTER_DESC_VIEW {
    u32 flags;
    i32 diffuse_maps[4];
    u8 unknown_014[0x20];
    i32 surface_threshold;
    i32 normal_threshold;
    u8 unknown_03c[8];
    u8 blend_ops[3];
    u8 unknown_047;
    i32 specular_threshold;
    u8 unknown_04c[0x3c];
    f32 shininess;
    u8 unknown_08c[0x1c];
    u8 texture_units;
    u8 sampler_indices[6];
    u8 unknown_0af[0x8d];
    u32 vertex_format;
    u8 unknown_140[0x74];
    u8 override_mode;
    u8 unknown_1b5[3];
    u32 layer_flags;
    u32 selectors;
    u8 unknown_1c0[0x24];
    i32 extra_texture;
    i32 environment_texture;
};

struct PC_SHADER_FILTER_DESC_VIEW;

struct PC_SHADER_FILTER_MATERIAL_VIEW {
    u8 unknown_000[0x40];
    u32 blend_flags;
    u8 unknown_044[0xbc];
    i32 texture_id;
};

struct PC_SHADER_FILTER_MATERIAL_VIEW;
#ifdef __cplusplus
struct PC_SHADER_MATERIAL_FILTER_VIEW {
    const struct PC_SHADER_FILTER_DESC_VIEW *descriptor;
    const struct PC_SHADER_FILTER_MATERIAL_VIEW *material;
    i32 flags;
    i32 variant;
    i32 first_path;
    i32 deferred_path;
    i32 second_path;
    i32 layer_count;
    i32 parameter;

    PC_SHADER_MATERIAL_FILTER_VIEW() {
    }

    PC_SHADER_MATERIAL_FILTER_VIEW(const PC_SHADER_FILTER_DESC_VIEW *descriptor, i32 flags, i32 parameter);

    PC_SHADER_MATERIAL_FILTER_VIEW(const PC_SHADER_FILTER_DESC_VIEW *desc, const PC_SHADER_FILTER_MATERIAL_VIEW *material, i32 flags,
                                   i32 variant);
    void Initialize(const PC_SHADER_FILTER_DESC_VIEW *, const PC_SHADER_FILTER_MATERIAL_VIEW *, i32, i32);
    bool UnfilteredHasLayer(i32) const;
    bool HasDiffuse(i32) const;
    bool HasLayer(i32) const;
    u32 GetVertexFlags() const;
    i32 HasTexCoordSets(i32 count) const;
    bool HasLayerVertexFlag8(i32 layer) const;
    bool HasVertexSkin() const;
    bool HasVertexSkinAlt() const;
    i32 HasAnisoFlip() const;
    i32 HasPhongLighting() const;
    i32 HasLayerFlag2() const;
    i32 HasLayerFlag5() const;
    i32 HasLayerFlags5And15() const;
    i32 HasLayerFlag15() const;
    i32 HasLayerFlag10() const;
    i32 HasLayerFlag21() const;
    i32 HasLayerFlag4() const;
    bool HasNoExtraLayer() const;
    i32 HasLayerFlag17() const;
    i32 HasLayerFlag19() const;
    bool HasNoExtraLighting() const;
    i32 HasLayerFlag18() const;
    i32 HasLayerFlag30() const;
    i32 HasLayerFlag9() const;
    i32 HasLayerFlag29() const;
    i32 HasUnfilteredLayerFlag22() const;
    i32 IsVariantOrLayerFlag12() const;
    bool LacksVariantAndLayerFlag12() const;
    i32 HasLayerFlag28() const;
    i32 HasLayerFlag27() const;
    i32 HasSelector0() const;
    i32 HasSelector1() const;
    const PC_SHADER_FILTER_DESC_VIEW *GetDescriptor() const;
    const PC_SHADER_FILTER_MATERIAL_VIEW *GetMaterial() const;
    bool HasBaseLayer() const;
    i32 HasSpecular() const;
    bool HasBaseLighting() const;
    i32 HasFirstPath() const;
    i32 HasDeferredPath() const;
    i32 HasLayerFlag20() const;
    i32 HasLayerFlag1() const;
    i32 HasNormalLayer1() const;
    i32 HasLayerFlag0() const;
    i32 HasNormalLayer0() const;
    i32 HasLayerFlag14() const;
    i32 HasLayerFlag13() const;
    i32 HasFlag31With23() const;
    i32 HasFlag31Without23() const;
    i32 HasSecondPath() const;
    bool HasExtraTexture() const;
    bool HasEnvironmentTexture() const;
    i32 HasSurfaceLayerAbove() const;
    i32 HasSurfaceLayerAtOrBelow() const;
    i32 fun_007442d0() const;
    i32 fun_00744320() const;
    i32 fun_00744330() const;
    bool fun_00744340() const;
    i32 fun_00744350(i32 layer) const;
    i32 fun_00744370() const;
    i32 fun_00744380() const;
    i32 fun_00744390() const;
    i32 fun_007443b0() const;
};
#endif
typedef struct PC_SHADER_MATERIAL_FILTER_VIEW PC_SHADER_MATERIAL_FILTER_VIEW;

struct PC_SHADER_MATERIAL_FILTER_VIEW;
struct PC_TEXTURE_RESOURCE;

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
    void NuShaderObjectKeyGenerate4(u32 *, u32, i32);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_SHADERMANAGER_H_PART
#define SAGA_NU2API_NU3D_SHADERMANAGER_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_SHADERMANAGER_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_SHADERMANAGER_H_PART == 2 && !defined(SAGA_NU2API_NU3D_SHADERMANAGER_H_STOP)
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
    u32 unknown_18;
    PC_SHADER_PARAMETER_LOCAL_PC parameters[100];
    struct PC_TEXTURE_RESOURCE *vertex_shader;
    struct PC_TEXTURE_RESOURCE *pixel_shader;
    u32 unknown_4d4;

    void fun_00741250();
    bool fun_00741260() const;
    void fun_00741270();
    void fun_007412a0();
    void fun_007412b0();
    i32 fun_007412c0() const;
    u32 *fun_007412d0();
    void fun_00745470(const u32 *key, i32 index, const char *vertex_source, i32 vertex_length, const char *pixel_source, i32 pixel_length,
                      i32 version);
    i32 fun_007454a0();
};

template <class T> class ShaderManagerTemplate {
  public:
    ShaderManagerTemplate();
    virtual ~ShaderManagerTemplate();
    virtual bool CreateShader(const struct PC_SHADER_MATERIAL_FILTER_VIEW *, bool, PC_SHADER_OBJECT_FULL_VIEW *, i32);
    virtual void AdaptDescriptor(const struct PC_SHADER_FILTER_DESC_VIEW *, const struct PC_SHADER_FILTER_MATERIAL_VIEW *);
    T slots[400];
    i32 last_allocated;
    PC_SHADER_OBJECT_FULL_VIEW *bound_slot;
    PC_SHADER_OBJECT_FULL_VIEW *AllocateSlot(i32 *index);
    PC_SHADER_OBJECT_FULL_VIEW *Retrieve(const struct PC_SHADER_FILTER_DESC_VIEW *, const struct PC_SHADER_FILTER_MATERIAL_VIEW *, i32, i32,
                                         bool);
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
#undef SAGA_NU2API_NU3D_SHADERMANAGER_H_PART
#define SAGA_NU2API_NU3D_SHADERMANAGER_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_SHADERMANAGER_H_STOP
#endif
#endif

#undef SAGA_NU2API_NU3D_SHADERMANAGER_H_STOP
