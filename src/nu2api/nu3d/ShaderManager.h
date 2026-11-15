#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nushaderobject.h"
#include "nu2api/nu3d/nushaderobjectbase.h"

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

/* PC_SHADER_FILTER_DESC_VIEW.layer_flags, named after the shader definitions each bit selects when the shader source
 * is built (shader_compiler.cpp) and the filter's Has* tests (nushadermanager.cpp). */
#define PC_SHADER_LAYER_NORMAL_MAP 0x1             /* SURFACE_NORMAL_MAP */
#define PC_SHADER_LAYER_PARALLAX_MAP 0x2           /* SURFACE_PARALLAX_MAP */
#define PC_SHADER_LAYER_FRESNEL 0x4                /* FRESNEL_STAGE */
#define PC_SHADER_LAYER_PHONG 0x8                  /* LIGHTING_PHONG */
#define PC_SHADER_LAYER_ANISO 0x10                 /* LIGHTING_ANISO */
#define PC_SHADER_LAYER_ENVMAP_CUBE 0x20           /* ENVMAP_CUBE */
#define PC_SHADER_LAYER_LAYER1 0x40                /* blend layer 1 */
#define PC_SHADER_LAYER_LAYER2 0x80                /* blend layer 2 */
#define PC_SHADER_LAYER_LAYER3 0x100               /* blend layer 3 */
#define PC_SHADER_LAYER_WATER 0x200                /* VERTEX_TRANSFORM_WATER, REFRACTION_WATER */
#define PC_SHADER_LAYER_REFRACTION 0x400           /* REFRACTION_STAGE */
#define PC_SHADER_LAYER_VERTEX_DISPLACEMENT 0x2000 /* VERTEX_FETCH_DISPLACEMENT */
#define PC_SHADER_LAYER_VTF_NORMAL_MAP 0x4000      /* VTF_SURFACE_NORMAL_MAP */
#define PC_SHADER_LAYER_SHINEMAP 0x8000            /* PS2_SHINEMAP */
#define PC_SHADER_LAYER_GLOW 0x10000               /* GLOW_OUTPUT */
#define PC_SHADER_LAYER_GOOCH 0x20000              /* LIGHTING_GOOCH */
#define PC_SHADER_LAYER_ENVMAP_LIGHTING 0x80000    /* LIGHTING_ENVMAP */
#define PC_SHADER_LAYER_REFLECTIVITY2 0x100000     /* REFLECTIVITY_STAGE2 */
#define PC_SHADER_LAYER_DAPPLE 0x200000            /* DAPPLE */
#define PC_SHADER_LAYER_WIND 0x400000              /* WIND */
#define PC_SHADER_LAYER_ANISO_FLIP 0x2000000       /* ANISO_FLIP */
#define PC_SHADER_LAYER_EDGE_LIGHTING 0x8000000    /* EDGE_LIGHTING */
#define PC_SHADER_LAYER_FRACTAL 0x10000000         /* FRACTAL */
#define PC_SHADER_LAYER_REFRACTED_GLASS 0x20000000 /* VERTEX_TRANSFORM_REFRACTED_GLASS, REFRACTION_GLASS */
#define PC_SHADER_LAYER_LEGO 0x40000000            /* LEGO */
#define PC_SHADER_LAYER_PRELIGHT_FX 0x80000000     /* PRELIGHT_FX */

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
extern "C" {
#endif
    void NuShaderObjectKeyGenerate3(u32 *, const struct PC_SHADER_MATERIAL_FILTER_VIEW *, i32);
    void NuShaderObjectKeyGenerate4(u32 *, u32, i32);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

class NuShaderObject { // size 0x4d8
  public:
    NuShaderObject() {
        PCShaderObjectCreateHLSLPc(this);
    }

    ~NuShaderObject() {
        PCShaderObjectDestroyHLSLPc(this);
    }

    i32 marker;                                   // 0x000
    i32 ref_count;                                // 0x004
    u32 key;                                      // 0x008
    u32 unknown_c;                                // 0x00c
    PC_SHADER_PARAMETER_LOCAL *active_parameters; // 0x010
    i32 version;                                  // 0x014
    u32 unknown_18;                               // 0x018
    PC_SHADER_PARAMETER_LOCAL parameters[100];    // 0x01c
    struct PC_TEXTURE_RESOURCE *vertex_shader;    // 0x4cc
    struct PC_TEXTURE_RESOURCE *pixel_shader;     // 0x4d0
    u32 stage_flags;                              // 0x4d4

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
    virtual bool CreateShader(const struct PC_SHADER_MATERIAL_FILTER_VIEW *, bool, NuShaderObject *, i32);
    virtual void AdaptDescriptor(const struct PC_SHADER_FILTER_DESC_VIEW *, const struct PC_SHADER_FILTER_MATERIAL_VIEW *);
    T slots[400];
    i32 last_allocated;
    NuShaderObject *bound_slot;
    NuShaderObject *AllocateSlot(i32 *index);
    NuShaderObject *Retrieve(const struct PC_SHADER_FILTER_DESC_VIEW *, const struct PC_SHADER_FILTER_MATERIAL_VIEW *, i32, i32, bool);
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

/* With T = NuShaderObject (0x4d8 bytes): slots at 0x4, last_allocated at 0x79184, bound_slot at 0x79188, size 0x7918c. */
typedef ShaderManagerTemplate<NuShaderObject> PC_SHADER_MANAGER_VIEW;
#else
/* NuShaderObject as C sees it: the members of the class without its constructor and methods. */
struct NuShaderObject {                           // size 0x4d8
    i32 marker;                                   // 0x000
    i32 ref_count;                                // 0x004
    u32 key;                                      // 0x008
    u32 unknown_c;                                // 0x00c
    PC_SHADER_PARAMETER_LOCAL *active_parameters; // 0x010
    i32 version;                                  // 0x014
    u32 unknown_18;                               // 0x018
    PC_SHADER_PARAMETER_LOCAL parameters[100];    // 0x01c
    struct PC_TEXTURE_RESOURCE *vertex_shader;    // 0x4cc
    struct PC_TEXTURE_RESOURCE *pixel_shader;     // 0x4d0
    u32 stage_flags;                              // 0x4d4
};
#endif
