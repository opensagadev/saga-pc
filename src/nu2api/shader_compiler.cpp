#include "saga.h"
#include <string.h>

void ShaderCompilerBasePc::Prepare(const PC_SHADER_FILTER_DESC_VIEW *descriptor,
                                   ShaderCompilerDictionaryPc *dictionary) {
    PC_SHADER_MATERIAL_FILTER_VIEW filter;
    filter.Initialize(descriptor, NULL, 0, 0);
    Configure(&filter, dictionary);
}
void ShaderCompilerBasePc::Configure(PC_SHADER_MATERIAL_FILTER_VIEW *filter, ShaderCompilerDictionaryPc *definitions) {
    const PC_SHADER_FILTER_DESC_VIEW *descriptor = filter->descriptor;
    const i32 profile = definitions->profile;
    u8 used_uvs[4] = {0, 0, 0, 0};
    const char *uv_functions[4] = {"FUNC_UVSET_0", "FUNC_UVSET_1", "FUNC_UVSET_2", "FUNC_UVSET_3"};
    const char *uv_names[4] = {"varying_uvSet0", "varying_uvSet1", "varying_uvSet2", "varying_uvSet3"};
    const char *vertex_uvs[4] = {"vs_uvSet0", "vs_uvSet1", "vs_uvSet2", "vs_uvSet3"};
    const char *combine[5] = {"DISABLE", "COMBINE_OVER", "COMBINE_ADD", "COMBINE_SUBTRACT", "COMBINE_MULTIPLY"};
    const char *numbers[17] = {"0", "1",  "2",  "3",  "4",  "5",  "6",  "7", "8",
                               "9", "10", "11", "12", "13", "14", "15", "16"};
    const char *version = profile == 4 ? "30" : profile == 3 ? "21" : profile == 2 ? "20" : profile == 1 ? "11" : "0";
    definitions->AddDefinition("VERTEX_SHADER_VERSION", version, 0);
    definitions->AddDefinition("PIXEL_SHADER_VERSION", version, 0);
    definitions->AddDefinition("BUILD_EFFECT", NULL, 0);
    bool nvidia_profile = profile == 4 && PCDeviceBackend.identifier.vendor == 0x10de;
    if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x4000)) {
        used_uvs[descriptor->sampler_indices[5] - 1] = 1;
        definitions->AddDefinition(uv_functions[descriptor->sampler_indices[5] - 1], "UVSET_ANIMATION", 0);
        definitions->AddDefinition("VTF_NORMAL_UVSET", uv_names[descriptor->sampler_indices[5] - 1], 1);
        definitions->AddDefinition("FUNC_fs_computeVTFNormal", "VTF_SURFACE_NORMAL_MAP", 0);
    }
    if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x40000000))
        definitions->AddDefinition("LEGO", NULL, 0);
    if (nvidia_profile) {
        if ((filter->descriptor->layer_flags & 0x400000) && (filter->descriptor->vertex_format & 0x2000000)) {
            definitions->AddDefinition("FUNC_pipePosition",
                                       filter->variant == 0 && (filter->descriptor->layer_flags & 0x40000)
                                           ? "VERTEX_TRANSFORM_PLANT2"
                                           : "VERTEX_TRANSFORM_PLANT",
                                       0);
            definitions->AddDefinition("WIND", NULL, 0);
        }
        if ((filter->descriptor->selectors & 1) && (filter->descriptor->vertex_format & 0x2000000)) {
            definitions->AddDefinition("FUNC_pipePosition", "VERTEX_TRANSFORM_PLANT2", 0);
            definitions->AddDefinition("WIND", NULL, 0);
        }
        if ((filter->descriptor->selectors & 2) && (filter->descriptor->vertex_format & 0x2000000))
            definitions->AddDefinition("WIND", NULL, 0);
        if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x2000)) {
            used_uvs[descriptor->sampler_indices[5] - 1] = 1;
            definitions->AddDefinition(uv_functions[descriptor->sampler_indices[5] - 1], "UVSET_ANIMATION", 0);
            definitions->AddDefinition("VERTEX_FETCH_UVSET", vertex_uvs[descriptor->sampler_indices[5] - 1], 1);
            definitions->AddDefinition("FUNC_pipePosition", "VERTEX_FETCH_DISPLACEMENT", 0);
        }
    } else {
        if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x20000000) &&
            (filter->descriptor->vertex_format & 0x30000) && !(filter->descriptor->vertex_format & 0x400000)) {
            definitions->AddDefinition("FUNC_pipePosition", "VERTEX_TRANSFORM_REFRACTED_GLASS_SKINNING", 0);
            definitions->AddDefinition("FUNC_computeNormal", "VERTEX_TRANSFORM_SKINNING", 0);
            definitions->AddDefinition("FUNC_computeTangent", "VERTEX_TRANSFORM_SKINNING", 0);
        } else if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x20000000)) {
            definitions->AddDefinition("FUNC_pipePosition", "VERTEX_TRANSFORM_REFRACTED_GLASS", 0);
        } else if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x200)) {
            definitions->AddDefinition("FUNC_pipePosition", "VERTEX_TRANSFORM_WATER", 0);
        } else if (filter->descriptor->vertex_format & 0x40000) {
            definitions->AddDefinition("FUNC_pipePosition", "VERTEX_RAW", 0);
        } else if (filter->HasVertexSkin() || filter->HasVertexSkinAlt()) {
            definitions->AddDefinition("FUNC_pipePosition", "VERTEX_TRANSFORM_SKINNING", 0);
            definitions->AddDefinition("FUNC_computeNormal", "VERTEX_TRANSFORM_SKINNING", 0);
            definitions->AddDefinition("FUNC_computeTangent", "VERTEX_TRANSFORM_SKINNING", 0);
        }
    }
    if (filter->descriptor->vertex_format & 0x80000)
        definitions->AddDefinition("VERTEX_GROUP", NULL, 0);
    if (filter->descriptor->vertex_format & 0x1000000)
        definitions->AddDefinition("FUNC_WATERCOEF", "WATERCOEF_AMPLITUDE", 1);
    if (filter->variant == 0 && (filter->descriptor->layer_flags & 2)) {
        used_uvs[descriptor->sampler_indices[0] - 1] = 1;
        definitions->AddDefinition("SURFACE_UVSET", uv_names[descriptor->sampler_indices[0] - 1], 1);
        definitions->AddDefinition("SURFACE_TYPE", "SURFACE_PARALLAX_MAP", 0);
    } else if (filter->variant == 0 && (filter->descriptor->layer_flags & 1)) {
        used_uvs[descriptor->sampler_indices[0] - 1] = 1;
        definitions->AddDefinition("SURFACE_UVSET", uv_names[descriptor->sampler_indices[0] - 1], 1);
        definitions->AddDefinition("SURFACE_TYPE", "SURFACE_NORMAL_MAP", 0);
    } else {
        definitions->AddDefinition("SURFACE_TYPE", "SURFACE_SMOOTH", 0);
    }
    if (filter->variant == 0 && (filter->descriptor->layer_flags & 3) &&
        filter->descriptor->normal_threshold > filter->parameter) {
        definitions->AddDefinition(
            "SURFACE_TYPE2", filter->descriptor->layer_flags & 2 ? "SURFACE_PARALLAX_MAP" : "SURFACE_NORMAL_MAP", 0);
        definitions->AddDefinition("LAYER1_UVSET", uv_names[descriptor->sampler_indices[2] - 1], 1);
    } else {
        definitions->AddDefinition("SURFACE_TYPE2", "SURFACE_SMOOTH", 0);
    }
    if ((filter->variant == 0 || filter->descriptor->override_mode != 0) && (filter->descriptor->vertex_format & 0x100))
        definitions->AddDefinition("LAYER0_COLORSET", "0", 0);
    if (filter->HasLayer(0)) {
        definitions->AddDefinition("COMBINE_OP_0", "COMBINE_BASE", 0);
        if (filter->HasDiffuse(0)) {
            definitions->AddDefinition("LAYER0_UVSET", "varying_uvSet0", 1);
            used_uvs[0] = 1;
            if (filter->descriptor->extra_texture > 0 && filter->descriptor->environment_texture > 0)
                definitions->AddDefinition("TEXTURE_ANIMATION_MAP", NULL, 0);
        } else {
            definitions->AddDefinition("LAYER0_DIFFUSEENABLE", NULL, 0);
        }
    }
    // The native three layer paths are distinct and ordered. Their record indices are fixed 1,2,3.
    if (filter->HasLayer(1) && profile != 1) {
        definitions->AddDefinition("COMBINE_OP_1", combine[descriptor->blend_ops[0]], 1);
        if ((filter->variant == 0 || filter->descriptor->override_mode != 0) &&
            ((filter->descriptor->vertex_format >> 9) & 3) >= 1) {
            definitions->AddDefinition("LAYER1_ALPHA", NULL, 0);
            definitions->AddDefinition("LAYER1_COLORSET", "varying_colorSet1", 1);
        }
        if (filter->HasDiffuse(1)) {
            definitions->AddDefinition("LAYER1_UVSET", uv_names[descriptor->sampler_indices[2] - 1], 1);
            used_uvs[descriptor->sampler_indices[2] - 1] = 1;
        } else
            definitions->AddDefinition("LAYER1_DIFFUSEENABLE", NULL, 0);
    }
    if (filter->HasLayer(2) && profile != 1) {
        definitions->AddDefinition("COMBINE_OP_2", combine[descriptor->blend_ops[1]], 1);
        if ((filter->variant == 0 || filter->descriptor->override_mode != 0) &&
            ((filter->descriptor->vertex_format >> 9) & 3) >= 2) {
            definitions->AddDefinition("LAYER2_ALPHA", NULL, 0);
            definitions->AddDefinition("LAYER1_COLORSET", "varying_colorSet1", 1);
        }
        if (filter->HasDiffuse(2)) {
            definitions->AddDefinition("LAYER2_UVSET", uv_names[descriptor->sampler_indices[3] - 1], 1);
            used_uvs[descriptor->sampler_indices[3] - 1] = 1;
        } else
            definitions->AddDefinition("LAYER2_DIFFUSEENABLE", NULL, 0);
    }
    if (filter->HasLayer(3) && profile != 1) {
        definitions->AddDefinition("COMBINE_OP_3", combine[descriptor->blend_ops[2]], 1);
        if ((filter->variant == 0 || filter->descriptor->override_mode != 0) &&
            ((filter->descriptor->vertex_format >> 9) & 3) >= 3) {
            definitions->AddDefinition("LAYER3_ALPHA", NULL, 0);
            definitions->AddDefinition("LAYER1_COLORSET", "varying_colorSet1", 1);
        }
        if (filter->HasDiffuse(3)) {
            definitions->AddDefinition("LAYER3_UVSET", uv_names[descriptor->sampler_indices[4] - 1], 1);
            used_uvs[descriptor->sampler_indices[4] - 1] = 1;
        } else
            definitions->AddDefinition("LAYER3_DIFFUSEENABLE", NULL, 0);
    }
    if (filter->variant == 0 && !(filter->descriptor->layer_flags & 0x1000)) {
        if (filter->descriptor->layer_flags & 0x20000)
            definitions->AddDefinition("LIGHTING_STAGE", "LIGHTING_GOOCH", 0);
        else if (filter->descriptor->layer_flags & 0x80000)
            definitions->AddDefinition("LIGHTING_STAGE", "LIGHTING_ENVMAP", 0);
        else if (filter->descriptor->layer_flags & 0x10) {
            definitions->AddDefinition("LIGHTING_STAGE", "LIGHTING_ANISO", 0);
            if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x2000000))
                definitions->AddDefinition("ANISO_FLIP", NULL, 0);
        } else {
            definitions->AddDefinition("LIGHTING_STAGE",
                                       filter->HasPhongLighting() ? "LIGHTING_PHONG" : "LIGHTING_LAMBERT", 0);
        }
    } else if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x80000000)) {
        definitions->AddDefinition("PRELIGHT_FX", NULL, 0);
        if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x18))
            definitions->AddDefinition("PRELIGHT_FX_LIVE_SPECULAR", NULL, 0);
        if (filter->variant == 0 && !(filter->descriptor->layer_flags & 0xa0000) &&
            (filter->descriptor->layer_flags & 0x10)) {
            definitions->AddDefinition("LIGHTING_STAGE", "LIGHTING_ANISO", 0);
            if (filter->HasAnisoFlip())
                definitions->AddDefinition("ANISO_FLIP", NULL, 0);
        } else if (filter->variant == 0 && (filter->descriptor->layer_flags & 0xa0000)) {
            definitions->AddDefinition("LIGHTING_STAGE", "DISABLE", 0);
        } else {
            definitions->AddDefinition("LIGHTING_STAGE",
                                       filter->HasPhongLighting() ? "LIGHTING_PHONG" : "LIGHTING_LAMBERT", 0);
        }
    } else {
        definitions->AddDefinition("LIGHTING_STAGE", "DISABLE", 0);
    }
    if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x40000) &&
        !(filter->descriptor->layer_flags & 0x400000) &&
        ((i32)filter->descriptor->flags >= 0 || !(filter->descriptor->flags & 0x800000)))
        definitions->AddDefinition("CAR_PAINT", NULL, 0);
    definitions->AddDefinition("LIGHTING_LIGHTS_COUNT", "3", 0);
    if (filter->variant == 0 && ((i32)filter->descriptor->flags < 0) && descriptor->texture_units > 0) {
        if (profile == 1) {
            u32 selector = (descriptor->selectors >> 2) & 3;
            if (selector == 0) {
                definitions->AddDefinition("LIGHTMAP_UVSET", uv_names[descriptor->texture_units - 1], 1);
                used_uvs[descriptor->texture_units - 1] = (u8)profile;
            } else {
                definitions->AddDefinition("LIGHTMAP_UVSET", uv_names[selector], 1);
                used_uvs[selector] = 1;
            }
        } else {
            definitions->AddDefinition("LIGHTMAP_UVSET", uv_names[descriptor->texture_units - 1], 1);
        }
        definitions->AddDefinition("LIGHTMAP_STAGE",
                                   !PcWindowOptions.usehires ||
                                           (filter->variant == 0 && (i32)filter->descriptor->flags < 0 &&
                                            (filter->descriptor->flags & 0x800000))
                                       ? "LIGHTMAP_SMOOTH"
                                       : "LIGHTMAP_DIRECTIONAL",
                                   0);
    } else {
        definitions->AddDefinition("LIGHTMAP_STAGE", "DISABLE", 0);
    }
    bool environment = false;
    if (filter->variant == 0 && ((filter->descriptor->layer_flags & 0x20) ||
                                 ((filter->descriptor->layer_flags & 0x60000000) && profile != 1))) {
        environment = true;
        definitions->AddDefinition("ENVMAP_STAGE", "ENVMAP_CUBE", 0);
        if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x20000000) && profile != 1)
            definitions->AddDefinition("SPECULAR_STAGE", "SPECULAR_PHONG", 0);
    } else if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x8000)) {
        environment = true;
        definitions->AddDefinition("ENVMAP_STAGE", "PS2_SHINEMAP", 0);
    }
    const bool lighting = filter->variant == 0 && (filter->descriptor->layer_flags & 0x18) != 0;
    if (environment || lighting) {
        if (profile != 1)
            definitions->AddDefinition("REFLECTIVITY_STAGE", NULL, 0);
        if (filter->variant == 0 && filter->descriptor->specular_threshold > filter->parameter) {
            definitions->AddDefinition("SPECULAR_UVSET", uv_names[descriptor->sampler_indices[1] - 1], 1);
            used_uvs[0] = 1;
        } else
            definitions->AddDefinition("SPECULAR_SPECULARENABLE", NULL, 0);
        if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x100000)) {
            definitions->AddDefinition("REFLECTIVITY_STAGE2", NULL, 0);
            if (filter->descriptor->surface_threshold > filter->parameter)
                definitions->AddDefinition("LAYER1_UVSET", uv_names[descriptor->sampler_indices[2] - 1], 1);
            else
                definitions->AddDefinition("SPECULAR_SPECULARENABLE2", NULL, 0);
        }
    }
    if (filter->variant == 0 && (environment || lighting || (filter->descriptor->layer_flags & 0x400))) {
        if (filter->descriptor->layer_flags & 4)
            definitions->AddDefinition("FRESNEL_STAGE", NULL, 0);
        if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x400))
            definitions->AddDefinition("REFRACTION_STAGE",
                                       filter->descriptor->layer_flags & 0x200        ? "REFRACTION_WATER"
                                       : filter->descriptor->layer_flags & 0x20000000 ? "REFRACTION_GLASS"
                                                                                      : "REFRACTION_DEFAULT",
                                       0);
    }
    i32 uv_count = 0;
    for (i32 i = 1; i <= 4; ++i) {
        if (used_uvs[i - 1]) {
            definitions->AddDefinition(uv_functions[i - 1], "UVSET_ANIMATION", 0);
            uv_count = i;
        }
    }
    definitions->AddDefinition("UVSET_COUNT", numbers[uv_count], 0);
    definitions->AddDefinition("NUMTCs", numbers[descriptor->texture_units], 0);
    if (filter->first_path) {
        definitions->AddDefinition("NORMAL_OUTPUT", "1", 1);
        if ((filter->descriptor->vertex_format & 0xc) &&
            !(filter->variant == 0 &&
              ((i32)filter->descriptor->flags < 0 || (filter->descriptor->layer_flags & 0x80000000))))
            definitions->AddDefinition("NORMAL_OUTPUT_NOT_NULL", NULL, 0);
    }
    if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x10000))
        definitions->AddDefinition("GLOW_OUTPUT", "1", 1);
    if (filter->deferred_path)
        definitions->AddDefinition("MOTION_OUTPUT", "2", 1);
    if (filter->second_path)
        definitions->AddDefinition("DEPTHRT_OUTPUT", filter->variant ? "0" : filter->deferred_path ? "3" : "2", 1);
    if (filter->variant)
        definitions->AddDefinition("RENDER_FOR_SHADOW", NULL, 0);
    definitions->AddDefinition("RENDER_TARGET_COUNT", numbers[filter->layer_count < 1 ? 1 : filter->layer_count], 1);
    if (filter->deferred_path && (filter->descriptor->vertex_format & 0x30000))
        definitions->AddDefinition("HACK_MOTION_VECTOR_NULL", NULL, 0);
    definitions->AddDefinition("COLOR_FACTOR", "1.0", 1);
    if (!(filter->descriptor->vertex_format & 0x40000) && filter->variant == 0 && profile != 1)
        definitions->AddDefinition("FOG_STAGE", "FOG_VERTEX_CLAMP_EXPONENTIAL2", 0);
    if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x200000) && profile != 1)
        definitions->AddDefinition("DAPPLE", NULL, 0);
    if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x10000000))
        definitions->AddDefinition("FRACTAL", NULL, 0);
    if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x8000000))
        definitions->AddDefinition("EDGE_LIGHTING", NULL, 0);
    bool swizzle = true;
    if (filter->material != NULL && filter->material->texture_id != 0) {
        NUTEXDIM_PC *texture = NuTextureGetDimensions(filter->material->texture_id);
        if (texture != NULL && texture->resource != NULL) {
            PC_TEXTURE_LEVEL_DESC_PC dimensions;
            texture->resource->methods->get_level_desc(texture->resource, 0, &dimensions);
            if (dimensions.format == 0x31545844)
                swizzle = false;
        }
    }
    definitions->AddDefinition("_WINPC", NULL, 0);
    if (swizzle)
        definitions->AddDefinition("SWIZZLE_NORMAL_MAPS", NULL, 0);
    definitions->AddDefinition("SWIZZLE_PARALLAX_MAPS", NULL, 0);
    definitions->AddDefinition("PACKED_NORMAL", NULL, 0);
    definitions->AddDefinition("PACKED_TANGENT", NULL, 0);
    definitions->AddDefinition("PACKED_TANGENT2", NULL, 0);
    definitions->AddDefinition("PACKED_VECTOR", NULL, 0);
    definitions->AddDefinition("SKIN_WORLD", NULL, 0);
}
