#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nushaderobject.h"
#include "nu2api/nu3d/nushaderobjectbase.h"

/* NUSHADERMTLDESC.layer_flags, named after the shader definitions each bit selects when the shader source
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

/* Shader uniform semantics: the index of a uniform in ShaderUniformRecords, named after its record's vertex or
 * fragment name (without vs_/fs_), as NuShaderManagerSetfv, NuShaderObjectSetElementsfv and the material update take it. */
#define SHADER_UNIFORM_AMBIENT_COLOR 0
#define SHADER_UNIFORM_SCENE_AMBIENT_COLOR 1
#define SHADER_UNIFORM_INCANDESCENT_GLOW 2
#define SHADER_UNIFORM_LIGHT_COLOR0 3
#define SHADER_UNIFORM_LIGHT_COLOR1 4
#define SHADER_UNIFORM_LIGHT_COLOR2 5
#define SHADER_UNIFORM_LIGHT_POSITION0 6
#define SHADER_UNIFORM_LIGHT_POSITION1 7
#define SHADER_UNIFORM_LIGHT_POSITION2 8
#define SHADER_UNIFORM_BITANGENT_FLIP 9
#define SHADER_UNIFORM_SURFACE_PARAMS 10
#define SHADER_UNIFORM_SURFACE_PARAMS2 11
#define SHADER_UNIFORM_SPECULAR_PARAMS 12
#define SHADER_UNIFORM_ENVMAP_PARAMS 13
#define SHADER_UNIFORM_VTF_K_HEIGHT 14
#define SHADER_UNIFORM_VTF_K_NORMAL 15
#define SHADER_UNIFORM_GOOCH_PARAMS 16
#define SHADER_UNIFORM_WORLD 17
#define SHADER_UNIFORM_VIEW_MATRIX 18
#define SHADER_UNIFORM_VIEW_PROJ 19
#define SHADER_UNIFORM_WORLD_VIEW_PROJ 20
#define SHADER_UNIFORM_WORLD_VIEW_MATRIX 21
#define SHADER_UNIFORM_MOTION 22
#define SHADER_UNIFORM_WATER_TABLE 23
#define SHADER_UNIFORM_SKIN_MATRIX 24
#define SHADER_UNIFORM_BLEND_SHAPES_COUNT 25
#define SHADER_UNIFORM_BLEND_SHAPES 26
#define SHADER_UNIFORM_BLEND_SHAPES_0 27
#define SHADER_UNIFORM_BLEND_SHAPES_1 28
#define SHADER_UNIFORM_BLEND_SHAPES_2 29
#define SHADER_UNIFORM_BLEND_SHAPES_3 30
#define SHADER_UNIFORM_BLEND_SHAPES_4 31
#define SHADER_UNIFORM_BLEND_SHAPES_5 32
#define SHADER_UNIFORM_BLEND_SHAPES_6 33
#define SHADER_UNIFORM_BLEND_SHAPES_7 34
#define SHADER_UNIFORM_LIGHT_PARAMS 35
#define SHADER_UNIFORM_LAYER0_SAMPLER 36
#define SHADER_UNIFORM_LAYER1_SAMPLER 37
#define SHADER_UNIFORM_LAYER2_SAMPLER 38
#define SHADER_UNIFORM_LAYER3_SAMPLER 39
#define SHADER_UNIFORM_SPECULAR_SAMPLER 40
#define SHADER_UNIFORM_SPECULAR2_SAMPLER 41
#define SHADER_UNIFORM_SURFACE_SAMPLER 42
#define SHADER_UNIFORM_SURFACE2_SAMPLER 43
#define SHADER_UNIFORM_DETAIL_SURFACE_SAMPLER 44
#define SHADER_UNIFORM_VERTEX_FETCH_SAMPLER 45
#define SHADER_UNIFORM_ENVMAP_SAMPLER_CUBE 46
#define SHADER_UNIFORM_ENVMAP_SAMPLER_SPHERE 47
#define SHADER_UNIFORM_PS2_SHINEMAP_SAMPLER 48
#define SHADER_UNIFORM_VTF_NORMAL_SAMPLER 49
#define SHADER_UNIFORM_FRAGMENT_UNIFORMS_SAMPLER 50
#define SHADER_UNIFORM_LAYER0_DIFFUSE 51
#define SHADER_UNIFORM_LAYER1_DIFFUSE 52
#define SHADER_UNIFORM_LAYER2_DIFFUSE 53
#define SHADER_UNIFORM_LAYER3_DIFFUSE 54
#define SHADER_UNIFORM_LAYER_K_OPACITIES 55
#define SHADER_UNIFORM_K_TINT 56
#define SHADER_UNIFORM_SPECULAR_SPECULAR 57
#define SHADER_UNIFORM_SPECULAR2_SPECULAR 58
#define SHADER_UNIFORM_REFRACTION_COLOR 59
#define SHADER_UNIFORM_REFRACTION_K_INDEX 60
#define SHADER_UNIFORM_BACK_BUFFER_SAMPLER 61
#define SHADER_UNIFORM_SCREEN_SIZE 62
#define SHADER_UNIFORM_TIME 63
#define SHADER_UNIFORM_UV_OFFSET0 64
#define SHADER_UNIFORM_UV_OFFSET1 65
#define SHADER_UNIFORM_UV_OFFSET2 66
#define SHADER_UNIFORM_UV_OFFSET3 67
#define SHADER_UNIFORM_FOG_COLOR 68
#define SHADER_UNIFORM_FOG_PARAMS 69
#define SHADER_UNIFORM_PROJECTION_PARAMS 70
#define SHADER_UNIFORM_FRUSTUM_PARAMS 71
#define SHADER_UNIFORM_LEGO_PARAMS 72
#define SHADER_UNIFORM_PS2_SHINE_MTX 73
#define SHADER_UNIFORM_NOISE_TEX_SIZE 74
#define SHADER_UNIFORM_AVERAGE_LIGHT_COLOR 75
#define SHADER_UNIFORM_AVERAGE_LIGHT_DIR 76
#define SHADER_UNIFORM_LIGHT_ROTATION_MTX 77
#define SHADER_UNIFORM_OFFSET_TABLE 78
#define SHADER_UNIFORM_VERTEX_GROUP_STATES 79
#define SHADER_UNIFORM_FX_ATTRIBUTES 80
#define SHADER_UNIFORM_DAPPLE_LIMIT 81
#define SHADER_UNIFORM_DIFFENVMAP_SAMPLER_CUBE 82
#define SHADER_UNIFORM_CARPAINT_PARAMS 83
#define SHADER_UNIFORM_CARPAINT_TINTS 84
#define SHADER_UNIFORM_WORLD_PARAMS 85
#define SHADER_UNIFORM_WIND_SAMPLER 86
#define SHADER_UNIFORM_WIND_PARAMS 87
#define SHADER_UNIFORM_WORLD_VIEW_INVERSE_TRANSPOSE 88
#define SHADER_UNIFORM_VIEW_INVERSE_TRANSPOSE 89
#define SHADER_UNIFORM_WORLD_CAM_POS 90
#define SHADER_UNIFORM_EDGELIT_PARAMS 91
#define SHADER_UNIFORM_FRACTAL_PARAMS 92
#define SHADER_UNIFORM_PERM_SAMPLER 93
#define SHADER_UNIFORM_PERMGRAD_SAMPLER 94
#define SHADER_UNIFORM_SPECULAR_DIRECTION 95
#define SHADER_UNIFORM_LIGHTMAP_OFFSET 96
#define SHADER_UNIFORM_TEX_ANIM_MAP_SAMPLER 97
#define SHADER_UNIFORM_TEX_ANIM_CURVES_SAMPLER 98
#define SHADER_UNIFORM_SHADOW_CASTING_OBJECT 99

#ifdef __cplusplus
struct PC_SHADER_MATERIAL_FILTER_VIEW {
    const NUSHADERMTLDESC *descriptor;
    const NUMTL *material;
    i32 flags;
    i32 variant;
    i32 first_path;
    i32 deferred_path;
    i32 second_path;
    i32 layer_count;
    i32 parameter;

    PC_SHADER_MATERIAL_FILTER_VIEW() {
    }

    PC_SHADER_MATERIAL_FILTER_VIEW(const NUSHADERMTLDESC *descriptor, i32 flags, i32 parameter);

    PC_SHADER_MATERIAL_FILTER_VIEW(const NUSHADERMTLDESC *desc, const NUMTL *material, i32 flags, i32 variant);
    void Initialize(const NUSHADERMTLDESC *, const NUMTL *, i32, i32);
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
    const NUSHADERMTLDESC *GetDescriptor() const;
    const NUMTL *GetMaterial() const;
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

    // Inline forms of the layer and type predicates (the shader key generator inlines these; the out-of-line ones
    // above reread the descriptor the same way).
    __forceinline u8 LayerBit(i32 bit) const {
        return variant == 0 ? (u8)((descriptor->layer_flags >> bit) & 1) : (u8)0;
    }

    __forceinline bool IsPlainType() const {
        return variant == 0 && (descriptor->flags & 0x80000000) && (descriptor->flags & 0x800000) == 0;
    }

    __forceinline bool IsAlternateType() const {
        return variant == 0 && (descriptor->flags & 0x80000000) && (descriptor->flags & 0x800000) != 0;
    }
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
    i32 GetMarker();
};

template <class T> class ShaderManagerTemplate {
  public:
    ShaderManagerTemplate();
    virtual ~ShaderManagerTemplate();
    virtual bool CreateShader(const struct PC_SHADER_MATERIAL_FILTER_VIEW *, bool, NuShaderObject *, i32);
    virtual void AdaptDescriptor(const NUSHADERMTLDESC *, const NUMTL *);
    T slots[400];
    i32 last_allocated;
    NuShaderObject *bound_slot;
    NuShaderObject *AllocateSlot(i32 *index);
    NuShaderObject *Retrieve(const NUSHADERMTLDESC *, const NUMTL *, i32, i32, bool);
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
