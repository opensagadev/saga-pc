#define SAGA_SHADER_PARSER_TYPES
#include "gamelib/util/CRC16.h"
#include "gamelib/util/gamelib_util_types.h"
#include "legoapi/items/collect/bolts.h"
#include "nu2api/PCInput.h"
#include "nu2api/nu3d/ShaderManager.h"
#include "nu2api/nu3d/ShaderObjectKey.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include "nu2api/shader_parser.h"
#include <stdio.h>
#include <string.h>

void ShaderCompilerBase::Prepare(const PC_SHADER_FILTER_DESC_VIEW *descriptor, ShaderCompilerDictionary *dictionary) {
    PC_SHADER_MATERIAL_FILTER_VIEW filter;
    filter.Initialize(descriptor, NULL, 0, 0);
    Configure(&filter, dictionary);
}

void ShaderCompilerBase::Configure(PC_SHADER_MATERIAL_FILTER_VIEW *filter, ShaderCompilerDictionary *definitions) {
    u8 used_uvs[4] = {0, 0, 0, 0};
    const PC_SHADER_FILTER_DESC_VIEW *descriptor = filter->descriptor;
    const i32 profile = definitions->profile;
    const char *uv_functions[4] = {"FUNC_UVSET_0", "FUNC_UVSET_1", "FUNC_UVSET_2", "FUNC_UVSET_3"};
    const char *uv_names[4] = {"varying_uvSet0", "varying_uvSet1", "varying_uvSet2", "varying_uvSet3"};
    const char *vertex_uvs[4] = {"vs_uvSet0", "vs_uvSet1", "vs_uvSet2", "vs_uvSet3"};
    const char *combine[5] = {"DISABLE", "COMBINE_OVER", "COMBINE_ADD", "COMBINE_SUBTRACT", "COMBINE_MULTIPLY"};
    const char *numbers[17] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16"};
    if (profile == PC_SHADER_MODEL_3_0) {
        definitions->AddDefinition("VERTEX_SHADER_VERSION", "30", 0);
        definitions->AddDefinition("PIXEL_SHADER_VERSION", "30", 0);
    } else if (profile == PC_SHADER_MODEL_2_X) {
        definitions->AddDefinition("VERTEX_SHADER_VERSION", "21", 0);
        definitions->AddDefinition("PIXEL_SHADER_VERSION", "21", 0);
    } else if (profile == PC_SHADER_MODEL_2_0) {
        definitions->AddDefinition("VERTEX_SHADER_VERSION", "20", 0);
        definitions->AddDefinition("PIXEL_SHADER_VERSION", "20", 0);
    } else if (profile == PC_SHADER_MODEL_1_1) {
        definitions->AddDefinition("VERTEX_SHADER_VERSION", "11", 0);
        definitions->AddDefinition("PIXEL_SHADER_VERSION", "11", 0);
    } else {
        definitions->AddDefinition("VERTEX_SHADER_VERSION", "0", 0);
        definitions->AddDefinition("PIXEL_SHADER_VERSION", "0", 0);
    }
    definitions->AddDefinition("BUILD_EFFECT", NULL, 0);
    bool nvidia_profile = profile == PC_SHADER_MODEL_3_0 && 0x10de == PCDeviceBackend.identifier.vendor;
    if (filter->variant == 0 && (filter->descriptor->layer_flags & PC_SHADER_LAYER_VTF_NORMAL_MAP)) {
        used_uvs[descriptor->sampler_indices[5] - 1] = 1;
        definitions->AddDefinition(uv_functions[descriptor->sampler_indices[5] - 1], "UVSET_ANIMATION", 0);
        definitions->AddDefinition("VTF_NORMAL_UVSET", uv_names[descriptor->sampler_indices[5] - 1], 1);
        definitions->AddDefinition("FUNC_fs_computeVTFNormal", "VTF_SURFACE_NORMAL_MAP", 0);
    }
    if (0 == filter->variant && (filter->descriptor->layer_flags & PC_SHADER_LAYER_LEGO)) {
        definitions->AddDefinition("LEGO", NULL, 0);
    }
    if (nvidia_profile) {
        if ((filter->descriptor->layer_flags & PC_SHADER_LAYER_WIND) && (filter->descriptor->vertex_format & 0x2000000)) {
            definitions->AddDefinition("FUNC_pipePosition",
                                       filter->variant == 0 && (filter->descriptor->layer_flags & 0x40000) ? "VERTEX_TRANSFORM_PLANT2"
                                                                                                           : "VERTEX_TRANSFORM_PLANT",
                                       0);
            definitions->AddDefinition("WIND", NULL, 0);
        }
        if ((filter->descriptor->selectors & 1) && (filter->descriptor->vertex_format & 0x2000000)) {
            definitions->AddDefinition("FUNC_pipePosition", "VERTEX_TRANSFORM_PLANT2", 0);
            definitions->AddDefinition("WIND", NULL, 0);
        }
        if ((filter->descriptor->selectors & 2) && (filter->descriptor->vertex_format & 0x2000000)) {
            definitions->AddDefinition("WIND", NULL, 0);
        }
        if (filter->variant == 0 && (filter->descriptor->layer_flags & PC_SHADER_LAYER_VERTEX_DISPLACEMENT)) {
            used_uvs[descriptor->sampler_indices[5] - 1] = 1;
            definitions->AddDefinition(uv_functions[descriptor->sampler_indices[5] - 1], "UVSET_ANIMATION", 0);
            definitions->AddDefinition("VERTEX_FETCH_UVSET", vertex_uvs[descriptor->sampler_indices[5] - 1], 1);
            definitions->AddDefinition("FUNC_pipePosition", "VERTEX_FETCH_DISPLACEMENT", 0);
        }
    }
    if (!nvidia_profile || filter->variant != 0 || !(filter->descriptor->layer_flags & PC_SHADER_LAYER_VERTEX_DISPLACEMENT)) {
        if (0 == filter->variant && (filter->descriptor->layer_flags & PC_SHADER_LAYER_REFRACTED_GLASS) &&
            (filter->descriptor->vertex_format & 0x30000) && !(filter->descriptor->vertex_format & 0x400000)) {
            definitions->AddDefinition("FUNC_pipePosition", "VERTEX_TRANSFORM_REFRACTED_GLASS_SKINNING", 0);
            definitions->AddDefinition("FUNC_computeNormal", "VERTEX_TRANSFORM_SKINNING", 0);
            definitions->AddDefinition("FUNC_computeTangent", "VERTEX_TRANSFORM_SKINNING", 0);
        } else if (filter->variant == 0 && (filter->descriptor->layer_flags & PC_SHADER_LAYER_REFRACTED_GLASS)) {
            definitions->AddDefinition("FUNC_pipePosition", "VERTEX_TRANSFORM_REFRACTED_GLASS", 0);
        } else if (filter->variant == 0 && (filter->descriptor->layer_flags & PC_SHADER_LAYER_WATER)) {
            definitions->AddDefinition("FUNC_pipePosition", "VERTEX_TRANSFORM_WATER", 0);
        } else if (filter->descriptor->vertex_format & 0x40000) {
            definitions->AddDefinition("FUNC_pipePosition", "VERTEX_RAW", 0);
        } else if (filter->HasVertexSkin() || filter->HasVertexSkinAlt()) {
            definitions->AddDefinition("FUNC_pipePosition", "VERTEX_TRANSFORM_SKINNING", 0);
            definitions->AddDefinition("FUNC_computeNormal", "VERTEX_TRANSFORM_SKINNING", 0);
            definitions->AddDefinition("FUNC_computeTangent", "VERTEX_TRANSFORM_SKINNING", 0);
        }
    }
    if (filter->descriptor->vertex_format & 0x80000) {
        definitions->AddDefinition("VERTEX_GROUP", NULL, 0);
    }
    if (filter->descriptor->vertex_format & 0x1000000) {
        definitions->AddDefinition("FUNC_WATERCOEF", "WATERCOEF_AMPLITUDE", 1);
    }
    if (filter->variant == 0 && (filter->descriptor->layer_flags & PC_SHADER_LAYER_PARALLAX_MAP)) {
        used_uvs[descriptor->sampler_indices[0] - 1] = 1;
        definitions->AddDefinition("SURFACE_UVSET", uv_names[descriptor->sampler_indices[0] - 1], 1);
        definitions->AddDefinition("SURFACE_TYPE", "SURFACE_PARALLAX_MAP", 0);
    } else if (filter->variant == 0 && (filter->descriptor->layer_flags & PC_SHADER_LAYER_NORMAL_MAP)) {
        used_uvs[descriptor->sampler_indices[0] - 1] = 1;
        definitions->AddDefinition("SURFACE_UVSET", uv_names[descriptor->sampler_indices[0] - 1], 1);
        definitions->AddDefinition("SURFACE_TYPE", "SURFACE_NORMAL_MAP", 0);
    } else {
        definitions->AddDefinition("SURFACE_TYPE", "SURFACE_SMOOTH", 0);
    }
    if (filter->variant == 0 && (filter->descriptor->layer_flags & (PC_SHADER_LAYER_NORMAL_MAP | PC_SHADER_LAYER_PARALLAX_MAP)) &&
        filter->descriptor->normal_threshold > filter->parameter) {
        definitions->AddDefinition(
            "SURFACE_TYPE2", filter->descriptor->layer_flags & PC_SHADER_LAYER_PARALLAX_MAP ? "SURFACE_PARALLAX_MAP" : "SURFACE_NORMAL_MAP",
            0);
        definitions->AddDefinition("LAYER1_UVSET", uv_names[descriptor->sampler_indices[2] - 1], 1);
    } else {
        definitions->AddDefinition("SURFACE_TYPE2", "SURFACE_SMOOTH", 0);
    }
    if ((filter->variant == 0 || filter->descriptor->override_mode != 0) && (filter->descriptor->vertex_format & 0x100)) {
        definitions->AddDefinition("LAYER0_COLORSET", "0", 0);
    }
    if (filter->HasLayer(0)) {
        definitions->AddDefinition("COMBINE_OP_0", "COMBINE_BASE", 0);
        if (filter->HasDiffuse(0)) {
            definitions->AddDefinition("LAYER0_UVSET", "varying_uvSet0", 1);
            used_uvs[0] = 1;
            if (filter->descriptor->extra_texture > 0 && filter->descriptor->environment_texture > 0) {
                definitions->AddDefinition("TEXTURE_ANIMATION_MAP", NULL, 0);
            }
        } else {
            definitions->AddDefinition("LAYER0_DIFFUSEENABLE", NULL, 0);
        }
    }
    // The three layer paths are distinct and ordered. Their record indices are fixed 1,2,3.
    if (filter->HasLayer(1) && profile != PC_SHADER_MODEL_1_1) {
        definitions->AddDefinition("COMBINE_OP_1", combine[descriptor->blend_ops[0]], 1);
        if ((filter->variant == 0 || filter->descriptor->override_mode != 0) && ((filter->descriptor->vertex_format >> 9) & 3) >= 1) {
            definitions->AddDefinition("LAYER1_ALPHA", NULL, 0);
            definitions->AddDefinition("LAYER1_COLORSET", "varying_colorSet1", 1);
        }
        if (filter->HasDiffuse(1)) {
            definitions->AddDefinition("LAYER1_UVSET", uv_names[descriptor->sampler_indices[2] - 1], 1);
            used_uvs[descriptor->sampler_indices[2] - 1] = 1;
        } else {
            definitions->AddDefinition("LAYER1_DIFFUSEENABLE", NULL, 0);
        }
    }
    if (filter->HasLayer(2) && profile != PC_SHADER_MODEL_1_1) {
        definitions->AddDefinition("COMBINE_OP_2", combine[descriptor->blend_ops[1]], 1);
        if ((0 == filter->variant || filter->descriptor->override_mode != 0) && ((filter->descriptor->vertex_format >> 9) & 3) >= 2) {
            definitions->AddDefinition("LAYER2_ALPHA", NULL, 0);
            definitions->AddDefinition("LAYER1_COLORSET", "varying_colorSet1", 1);
        }
        if (filter->HasDiffuse(2)) {
            definitions->AddDefinition("LAYER2_UVSET", uv_names[descriptor->sampler_indices[3] - 1], 1);
            used_uvs[descriptor->sampler_indices[3] - 1] = 1;
        } else {
            definitions->AddDefinition("LAYER2_DIFFUSEENABLE", NULL, 0);
        }
    }
    if (filter->HasLayer(3) && profile != PC_SHADER_MODEL_1_1) {
        definitions->AddDefinition("COMBINE_OP_3", combine[descriptor->blend_ops[2]], 1);
        if ((filter->variant == 0 || filter->descriptor->override_mode != 0) && ((filter->descriptor->vertex_format >> 9) & 3) > 2) {
            definitions->AddDefinition("LAYER3_ALPHA", NULL, 0);
            definitions->AddDefinition("LAYER1_COLORSET", "varying_colorSet1", 1);
        }
        if (filter->HasDiffuse(3)) {
            definitions->AddDefinition("LAYER3_UVSET", uv_names[descriptor->sampler_indices[4] - 1], 1);
            used_uvs[descriptor->sampler_indices[4] - 1] = 1;
        } else {
            definitions->AddDefinition("LAYER3_DIFFUSEENABLE", NULL, 0);
        }
    }
    if (filter->variant == 0 && !(filter->descriptor->layer_flags & 0x1000)) {
        if (filter->descriptor->layer_flags & PC_SHADER_LAYER_GOOCH) {
            definitions->AddDefinition("LIGHTING_STAGE", "LIGHTING_GOOCH", 0);
        } else if (filter->descriptor->layer_flags & PC_SHADER_LAYER_ENVMAP_LIGHTING) {
            definitions->AddDefinition("LIGHTING_STAGE", "LIGHTING_ENVMAP", 0);
        } else if (filter->descriptor->layer_flags & PC_SHADER_LAYER_ANISO) {
            definitions->AddDefinition("LIGHTING_STAGE", "LIGHTING_ANISO", 0);
            if (filter->variant == 0 && (filter->descriptor->layer_flags & PC_SHADER_LAYER_ANISO_FLIP)) {
                definitions->AddDefinition("ANISO_FLIP", NULL, 0);
            }
        } else {
            definitions->AddDefinition("LIGHTING_STAGE", filter->HasPhongLighting() ? "LIGHTING_PHONG" : "LIGHTING_LAMBERT", 0);
        }
    } else if (filter->variant == 0 && (filter->descriptor->layer_flags & PC_SHADER_LAYER_PRELIGHT_FX)) {
        definitions->AddDefinition("PRELIGHT_FX", NULL, 0);
        if (filter->variant == 0 && (filter->descriptor->layer_flags & (PC_SHADER_LAYER_PHONG | PC_SHADER_LAYER_ANISO))) {
            definitions->AddDefinition("PRELIGHT_FX_LIVE_SPECULAR", NULL, 0);
        }
        if (filter->variant == 0 && !(filter->descriptor->layer_flags & (PC_SHADER_LAYER_GOOCH | PC_SHADER_LAYER_ENVMAP_LIGHTING)) &&
            (filter->descriptor->layer_flags & PC_SHADER_LAYER_ANISO)) {
            definitions->AddDefinition("LIGHTING_STAGE", "LIGHTING_ANISO", 0);
            if (filter->HasAnisoFlip()) {
                definitions->AddDefinition("ANISO_FLIP", NULL, 0);
            }
        } else if (filter->variant == 0 && (filter->descriptor->layer_flags & (PC_SHADER_LAYER_GOOCH | PC_SHADER_LAYER_ENVMAP_LIGHTING))) {
            definitions->AddDefinition("LIGHTING_STAGE", "DISABLE", 0);
        } else {
            definitions->AddDefinition("LIGHTING_STAGE", filter->HasPhongLighting() ? "LIGHTING_PHONG" : "LIGHTING_LAMBERT", 0);
        }
    } else {
        definitions->AddDefinition("LIGHTING_STAGE", "DISABLE", 0);
    }
    if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x40000) && !(filter->descriptor->layer_flags & PC_SHADER_LAYER_WIND) &&
        ((i32)filter->descriptor->flags >= 0 || !(filter->descriptor->flags & 0x800000))) {
        definitions->AddDefinition("CAR_PAINT", NULL, 0);
    }
    definitions->AddDefinition("LIGHTING_LIGHTS_COUNT", "3", 0);
    if (filter->variant == 0 && ((i32)filter->descriptor->flags < 0) && descriptor->texture_units > 0) {
        if (profile == PC_SHADER_MODEL_1_1) {
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
                                   !PcWindowOptions.usehires || (filter->variant == 0 && (i32)filter->descriptor->flags < 0 &&
                                                                 (filter->descriptor->flags & 0x800000))
                                       ? "LIGHTMAP_SMOOTH"
                                       : "LIGHTMAP_DIRECTIONAL",
                                   0);
    } else {
        definitions->AddDefinition("LIGHTMAP_STAGE", "DISABLE", 0);
    }
    bool environment = false;
    if (filter->variant == 0 && ((filter->descriptor->layer_flags & PC_SHADER_LAYER_ENVMAP_CUBE) ||
                                 ((filter->descriptor->layer_flags & (PC_SHADER_LAYER_REFRACTED_GLASS | PC_SHADER_LAYER_LEGO)) &&
                                  profile != PC_SHADER_MODEL_1_1))) {
        environment = true;
        definitions->AddDefinition("ENVMAP_STAGE", "ENVMAP_CUBE", 0);
        if (filter->variant == 0 && (filter->descriptor->layer_flags & PC_SHADER_LAYER_REFRACTED_GLASS) && profile != PC_SHADER_MODEL_1_1) {
            definitions->AddDefinition("SPECULAR_STAGE", "SPECULAR_PHONG", 0);
        }
    } else if (filter->variant == 0 && (filter->descriptor->layer_flags & PC_SHADER_LAYER_SHINEMAP)) {
        environment = true;
        definitions->AddDefinition("ENVMAP_STAGE", "PS2_SHINEMAP", 0);
    }
    const bool lighting = filter->variant == 0 && (filter->descriptor->layer_flags & (PC_SHADER_LAYER_PHONG | PC_SHADER_LAYER_ANISO)) != 0;
    if (environment || lighting) {
        if (profile != PC_SHADER_MODEL_1_1) {
            definitions->AddDefinition("REFLECTIVITY_STAGE", NULL, 0);
        }
        if (filter->variant == 0 && filter->descriptor->specular_threshold > filter->parameter) {
            definitions->AddDefinition("SPECULAR_UVSET", uv_names[descriptor->sampler_indices[1] - 1], 1);
            used_uvs[0] = 1;
        } else {
            definitions->AddDefinition("SPECULAR_SPECULARENABLE", NULL, 0);
        }
        if (filter->variant == 0 && (filter->descriptor->layer_flags & PC_SHADER_LAYER_REFLECTIVITY2)) {
            definitions->AddDefinition("REFLECTIVITY_STAGE2", NULL, 0);
            if (filter->descriptor->surface_threshold > filter->parameter) {
                definitions->AddDefinition("LAYER1_UVSET", uv_names[descriptor->sampler_indices[2] - 1], 1);
            } else {
                definitions->AddDefinition("SPECULAR_SPECULARENABLE2", NULL, 0);
            }
        }
    }
    if (filter->variant == 0 && (environment || lighting || (filter->descriptor->layer_flags & PC_SHADER_LAYER_REFRACTION))) {
        if (filter->descriptor->layer_flags & PC_SHADER_LAYER_FRESNEL) {
            definitions->AddDefinition("FRESNEL_STAGE", NULL, 0);
        }
        if (filter->variant == 0 && (filter->descriptor->layer_flags & PC_SHADER_LAYER_REFRACTION)) {
            definitions->AddDefinition("REFRACTION_STAGE",
                                       filter->descriptor->layer_flags & PC_SHADER_LAYER_WATER             ? "REFRACTION_WATER"
                                       : filter->descriptor->layer_flags & PC_SHADER_LAYER_REFRACTED_GLASS ? "REFRACTION_GLASS"
                                                                                                           : "REFRACTION_DEFAULT",
                                       0);
        }
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
              ((i32)filter->descriptor->flags < 0 || (filter->descriptor->layer_flags & PC_SHADER_LAYER_PRELIGHT_FX)))) {
            definitions->AddDefinition("NORMAL_OUTPUT_NOT_NULL", NULL, 0);
        }
    }
    if (filter->variant == 0 && (filter->descriptor->layer_flags & PC_SHADER_LAYER_GLOW)) {
        definitions->AddDefinition("GLOW_OUTPUT", "1", 1);
    }
    if (filter->deferred_path) {
        definitions->AddDefinition("MOTION_OUTPUT", "2", 1);
    }
    if (filter->second_path) {
        definitions->AddDefinition("DEPTHRT_OUTPUT", filter->variant ? "0" : filter->deferred_path ? "3" : "2", 1);
    }
    if (filter->variant) {
        definitions->AddDefinition("RENDER_FOR_SHADOW", NULL, 0);
    }
    definitions->AddDefinition("RENDER_TARGET_COUNT", numbers[filter->layer_count < 1 ? 1 : filter->layer_count], 1);
    if (filter->deferred_path && (filter->descriptor->vertex_format & 0x30000)) {
        definitions->AddDefinition("HACK_MOTION_VECTOR_NULL", NULL, 0);
    }
    definitions->AddDefinition("COLOR_FACTOR", "1.0", 1);
    if (!(filter->descriptor->vertex_format & 0x40000) && filter->variant == 0 && profile != PC_SHADER_MODEL_1_1) {
        definitions->AddDefinition("FOG_STAGE", "FOG_VERTEX_CLAMP_EXPONENTIAL2", 0);
    }
    if (filter->variant == 0 && (filter->descriptor->layer_flags & PC_SHADER_LAYER_DAPPLE) && profile != PC_SHADER_MODEL_1_1) {
        definitions->AddDefinition("DAPPLE", NULL, 0);
    }
    if (filter->variant == 0 && (PC_SHADER_LAYER_FRACTAL & filter->descriptor->layer_flags)) {
        definitions->AddDefinition("FRACTAL", NULL, 0);
    }
    if (filter->variant == 0 && (filter->descriptor->layer_flags & PC_SHADER_LAYER_EDGE_LIGHTING)) {
        definitions->AddDefinition("EDGE_LIGHTING", NULL, 0);
    }
    bool swizzle = true;
    if (filter->material != NULL && filter->material->texture_id != 0) {
        NUTEXDIM *texture = NuTextureGetDimensions(filter->material->texture_id);
        if (texture != NULL && texture->resource != NULL) {
            PC_TEXTURE_LEVEL_DESC dimensions;
            texture->resource->methods->get_level_desc(texture->resource, 0, &dimensions);
            if (dimensions.format == 0x31545844) {
                swizzle = false;
            }
        }
    }
    definitions->AddDefinition("_WINPC", NULL, 0);
    if (swizzle) {
        definitions->AddDefinition("SWIZZLE_NORMAL_MAPS", NULL, 0);
    }
    definitions->AddDefinition("SWIZZLE_PARALLAX_MAPS", NULL, 0);
    definitions->AddDefinition("PACKED_NORMAL", NULL, 0);
    definitions->AddDefinition("PACKED_TANGENT", NULL, 0);
    definitions->AddDefinition("PACKED_TANGENT2", NULL, 0);
    definitions->AddDefinition("PACKED_VECTOR", NULL, 0);
    definitions->AddDefinition("SKIN_WORLD", NULL, 0);
}

void ShaderCompilerHLSLPc::Prepare(const PC_SHADER_FILTER_DESC_VIEW *descriptor, ShaderCompilerDictionary *dictionary) {
    PC_SHADER_MATERIAL_FILTER_VIEW filter;
    filter.Initialize(descriptor, NULL, 0, 0);
    Configure(&filter, dictionary);
}

void ShaderCompilerHLSLPc::Configure(PC_SHADER_MATERIAL_FILTER_VIEW *filter, ShaderCompilerDictionary *dictionary) {
    ShaderCompilerBase::Configure(filter, dictionary);
    dictionary->AddDefinition("BUILD_VERTEX_SHADER", NULL, 0);
    dictionary->AddDefinition("BUILD_PIXEL_SHADER", NULL, 0);
    dictionary->AddDefinition("HLSL_LANGUAGE", NULL, 0);
    dictionary->AddDefinition("COLOR_FACTOR", "2.0", 1);
}

ShaderBuilderGen::ShaderBuilderGen(VirtualStackAllocator &parent, i32 mode)
    : unknown_204(0), parsed_source(NULL), dictionary(parent, mode), source_buffer(parent, 0x2d000), parser_memory(parent, 0x32000) {
    save_directory[0] = 0;
}

ShaderBuilderGen::~ShaderBuilderGen() {
    parsed_source = NULL;
}

HLSLShaderBuilder::HLSLShaderBuilder(VirtualStackAllocator &allocator) : ShaderCompilerBase(allocator, 0) {
}

void ShaderCompilerBase::SetIncludeDirectories(const char *paths) {
    strcpy(include_directories, paths);
}

void ShaderCompilerBase::SetSaveDirectory(const char *path) {
    strcpy(save_directory, path);
}

void ShaderCompilerBase::SetProfile(i32 profile, i32 first, i32 second) {
    dictionary.SetProfile(profile, first, second);
}

void ShaderCompilerBase::fun_00745270(const char *name, i32 count) {
    dictionary.AddRegisterBlock(name, count);
}

void ShaderCompilerBase::SetForceMode(bool force) {
    dictionary.force_mode = force;
}

bool ShaderCompilerBase::fun_00745290() {
    return dictionary.force_mode;
}

i32 ShaderCompilerBase::fun_007452a0() {
    return dictionary.profile;
}

void *ShaderCompilerBase::ParseSource(const char *source, i32 length, ShaderCompilerBufferAllocator *buffer, i32 prefix,
                                      VirtualStackAllocator *memory) {
    if (length < 0) {
        length = (i32)strlen(source);
    }
    i32 size = prefix + length;
    buffer->length = 0;
    if (buffer->capacity < size) {
        buffer->Resize(size);
    }
    buffer->length = size;
    strncpy(reinterpret_cast<char *>(buffer->data + prefix), source, length);
    memset(buffer->data, ' ', prefix);
    for (i32 i = 0; i < prefix; i += 100) {
        buffer->data[i] = '\n';
    }
    ShaderCompilerParser parser;
    parser.SetIncludeDirectories(include_directories);
    return parser.Parse(buffer, prefix, 1, memory);
}

bool ShaderCompilerBase::LoadSource(const char *source, i32 length) {
    parsed_source = ParseSource(source, length, &source_buffer, 0x5000, &parser_memory);
    return parsed_source != NULL;
}

void ShaderCompilerBase::Generate(PC_SHADER_MATERIAL_FILTER_VIEW *filter, ShaderCompilerBufferAllocator *vertex,
                                  ShaderCompilerBufferAllocator *pixel) {
    ShaderCompilerDictionary *dict = &dictionary;
    for (i32 i = 0; i < dict->definition_count; ++i) {
        dict->definitions[i]->expansion_state = 0;
    }
    dict->scratch.cursor = dict->scratch.base;
    dict->program_count = 0;
    Configure(filter, dict);
    dict->Expand(parsed_source, NULL);
    dict->Expand(parsed_source, NULL);
    dict->AddDefinition("_SHADERBUILDER_DEBUG", NULL, 0);
    if (vertex != NULL) {
        vertex->length = 0;
        dictionary.output_stage = 0;
        dict->BindOutputBuffer(vertex);
        dict->Expand(parsed_source, vertex);
        if (vertex->length > 0 && vertex->data[vertex->length - 1] != 0) {
            if (vertex->length + 1 > vertex->capacity) {
                vertex->Resize((3 * (vertex->length + 1)) / 2);
            }
            vertex->data[vertex->length] = 0;
            ++vertex->length;
        }
    }
    if (pixel != NULL) {
        pixel->length = 0;
        dictionary.output_stage = 1;
        dict->BindOutputBuffer(pixel);
        dict->Expand(parsed_source, pixel);
        if (pixel->length > 0 && pixel->data[pixel->length - 1] != 0) {
            if (pixel->length + 1 > pixel->capacity) {
                pixel->Resize((3 * (pixel->length + 1)) / 2);
            }
            pixel->data[pixel->length] = 0;
            ++pixel->length;
        }
    }
}

/* Writes data to "<save directory>\<name><suffix>"; no caller. */
void ShaderCompilerBase::SaveFile(const char *suffix, const char *name, const void *data, i32 size) {
    char path[0x100];
    NUFILE file;
    sprintf(path, "%s\\%s%s", save_directory, name, suffix);
    file = NuFileOpen(path, 1);
    if (file != 0) {
        NuFileWrite(file, data, size);
        NuFileClose(file);
    }
}

/* Copies the next ';'-separated entry of list from *position into entry. */
bool ShaderNextDirectory(const char *list, i32 *position, char *entry) {
    if (*position >= (i32)strlen(list)) {
        return false;
    }
    list += *position;
    while (*list != 0 && *list != ';') {
        *entry++ = *list++;
        ++*position;
    }
    *entry = 0;
    if (*list == ';') {
        ++*position;
    }
    return true;
}

/* Diagnostics compiled out; varargs keeps the calls. */
static void ShaderBuilderTrace(...) {
}

void *ShaderCompilerBase::ParseFile(const char *name, ShaderCompilerBufferAllocator *buffer, i32 prefix, VirtualStackAllocator *memory) {
    char directory[0x100];
    char path[0x100];
    i32 position = 0;
    NUFILE file = 0;
    NUDATHDR *dat = NuDatGet();
    i32 size;
    NuDatSet(NULL);
    do {
        if (!ShaderNextDirectory(include_directories, &position, directory)) {
            break;
        }
        sprintf(path, "%s\\%s", directory, name);
        file = NuFileOpen(path, 0);
    } while (file == 0);
    NuDatSet(dat);
    if (file == 0) {
        ShaderBuilderTrace(path, include_directories);
    }
    size = (i32)NuFileOpenSize(file);
    if (size <= 0) {
        ShaderBuilderTrace();
        return NULL;
    }
    buffer->length = 0;
    if (buffer->capacity < prefix + size) {
        buffer->Resize(prefix + size);
    }
    buffer->length = prefix + size;
    NuFileRead(file, buffer->data + prefix, size);
    NuFileClose(file);
    memset(buffer->data, ' ', prefix);
    for (i32 i = 0; i < prefix; i += 100) {
        buffer->data[i] = '\n';
    }
    ShaderCompilerParser parser;
    parser.SetIncludeDirectories(directory);
    return parser.Parse(buffer, prefix, 1, memory);
}

bool ShaderCompilerBase::LoadFile(const char *name) {
    parsed_source = ParseFile(name, &source_buffer, 0x5000, &parser_memory);
    return parsed_source != NULL;
}

/* Method table of ID3DXEffectCompiler as far as it is called here. */
typedef struct D3DX_EFFECT_COMPILER_METHODS {
    HRESULT(__stdcall *query_interface)(ID3DXEffectCompiler *, const IID *, void **);
    ULONG(__stdcall *add_ref)(ID3DXEffectCompiler *);
    ULONG(__stdcall *release)(ID3DXEffectCompiler *);
    void *unknown_0c[56];
    HRESULT(__stdcall *compile_effect)(ID3DXEffectCompiler *, DWORD, ID3DXBuffer **, ID3DXBuffer **);
} D3DX_EFFECT_COMPILER_METHODS;

struct ID3DXEffectCompiler {
    const D3DX_EFFECT_COMPILER_METHODS *methods;
};

/* Compiles an effect from source into output; returns the compiled size. No caller. */
i32 ShaderCompilerBase::CompileEffect(const char *source, i32 length, void *output, i32 unused) {
    ID3DXBuffer *effect;
    ID3DXEffectCompiler *compiler;
    i32 size;
    D3DXCreateBuffer(0x80, &effect);
    D3DXCreateEffectCompiler(source, length, NULL, NULL, 0, &compiler, NULL);
    compiler->methods->compile_effect(compiler, 0, &effect, NULL);
    size = effect->methods->get_buffer_size(effect);
    PCByteCopy(output, effect->methods->get_buffer_pointer(effect), size);
    effect->methods->release(effect);
    if (compiler != NULL) {
        compiler->methods->release(compiler);
    }
    return size;
}

/* The material fields from its texture flags on (PC_SHADER_MATERIAL_DISPATCH_VIEW + 0xb4), as the shader
 * builder's parameter defaults read them. */
typedef struct PC_SHADER_DEFAULTS_VIEW { // size 0x13c
    u32 texture_flags;                   // 0x000
    i32 diffuse_texture_ids[4];          // 0x004
    u32 channel_colours[4];              // 0x014
    f32 parameter_d8;                    // 0x024
    f32 parameter_dc;                    // 0x028
    f32 parameter_e0;                    // 0x02c
    f32 parameter_e4;                    // 0x030
    i32 height_texture_id;               // 0x034
    i32 normal_texture_id;               // 0x038
    f32 texture_lerp;                    // 0x03c
    u32 reflection_colour;               // 0x040
    u8 unknown_0f8[0x3];                 // 0x044
    u8 flip_sign;                        // 0x047
    u8 unknown_0fc[0x1c];                // 0x048
    f32 parameter_118;                   // 0x064
    u32 diffuse_colour;                  // 0x068
    u32 specular_colour;                 // 0x06c
    u32 tint_colour;                     // 0x070
    u32 shadow_colour;                   // 0x074
    f32 parameter_12c;                   // 0x078
    f32 parameter_130;                   // 0x07c
    f32 parameter_134;                   // 0x080
    f32 parameter_138;                   // 0x084
    f32 parameter_13c;                   // 0x088
    f32 parameter_140;                   // 0x08c
    f32 parameter_144;                   // 0x090
    f32 parameter_148;                   // 0x094
    f32 parameter_14c;                   // 0x098
    f32 parameter_150;                   // 0x09c
    u8 unknown_154[0x60];                // 0x0a0
    f32 parameter_1b4;                   // 0x100
    f32 parameter_1b8;                   // 0x104
    f32 parameter_1bc;                   // 0x108
    u8 unknown_1c0[0x10];                // 0x10c
    f32 uv_upload_lanes[8];              // 0x11c
} PC_SHADER_DEFAULTS_VIEW;

static void FormatFloat(char *out, f32 value) {
    sprintf(out, "%f", value);
}

static void FormatFloat2(char *out, const f32 *value) {
    sprintf(out, "float2(%f, %f)", value[0], value[1]);
}

static void FormatFloat4(char *out, f32 x, f32 y, f32 z, f32 w) {
    sprintf(out, "float4(%f, %f, %f, %f)", x, y, z, w);
}

static void FormatColour(i32 colour, char *out) {
    sprintf(out, "float4(%f, %f, %f, %f)", (f32)(colour & 0xff) / 255.0f, (f32)((colour >> 8) & 0xff) / 255.0f,
            (f32)((colour >> 16) & 0xff) / 255.0f, (f32)((colour >> 24) & 0xff) / 255.0f);
}

/* The default value of shader parameter id as an HLSL literal; ids without one leave out untouched. */
void __stdcall ShaderParameterDefault(const PC_SHADER_DEFAULTS_VIEW *material, i32 id, char *out, i32 unused) {
    switch (id) {
        case SHADER_UNIFORM_AMBIENT_COLOR:
            FormatColour(material->diffuse_colour, out);
            break;
        case SHADER_UNIFORM_INCANDESCENT_GLOW: {
            i32 colour = material->specular_colour;
            FormatFloat4(out, (f32)(colour & 0xff) / 255.0f, (f32)((colour >> 8) & 0xff) / 255.0f, (f32)((colour >> 16) & 0xff) / 255.0f,
                         material->parameter_1b4);
            break;
        }
        case SHADER_UNIFORM_BITANGENT_FLIP:
            FormatFloat(out, material->flip_sign ? 1.0f : 0.0f);
            break;
        case SHADER_UNIFORM_SURFACE_PARAMS:
            FormatFloat4(out, material->parameter_134, 0.0f, material->parameter_138, material->parameter_14c);
            break;
        case SHADER_UNIFORM_SURFACE_PARAMS2:
            FormatFloat4(out, material->texture_lerp, 0.0f, 0.0f, 0.0f);
            break;
        case SHADER_UNIFORM_SPECULAR_PARAMS:
            FormatFloat4(out, material->parameter_130, material->parameter_12c, material->parameter_144, material->parameter_148);
            break;
        case SHADER_UNIFORM_ENVMAP_PARAMS:
            FormatFloat4(out, material->parameter_13c, material->parameter_140, 0.0f, 0.0f);
            break;
        case SHADER_UNIFORM_VTF_K_NORMAL:
            FormatFloat(out, material->parameter_118);
            break;
        case SHADER_UNIFORM_LAYER0_DIFFUSE:
            FormatColour(material->channel_colours[0], out);
            break;
        case SHADER_UNIFORM_LAYER1_DIFFUSE:
            FormatColour(material->channel_colours[1], out);
            break;
        case SHADER_UNIFORM_LAYER2_DIFFUSE:
            FormatColour(material->channel_colours[2], out);
            break;
        case SHADER_UNIFORM_LAYER3_DIFFUSE:
            FormatColour(material->channel_colours[3], out);
            break;
        case SHADER_UNIFORM_LAYER_K_OPACITIES:
            FormatFloat4(out, material->parameter_d8, material->parameter_dc, material->parameter_e0, material->parameter_e4);
            break;
        case SHADER_UNIFORM_SPECULAR_SPECULAR:
            FormatColour(material->shadow_colour, out);
            break;
        case SHADER_UNIFORM_REFRACTION_COLOR:
            FormatColour(material->tint_colour, out);
            break;
        case SHADER_UNIFORM_REFRACTION_K_INDEX:
            FormatFloat(out, (material->parameter_150 - 1.0) * (f64)0.1f);
            break;
        case SHADER_UNIFORM_UV_OFFSET0:
            FormatFloat2(out, &material->uv_upload_lanes[0]);
            break;
        case SHADER_UNIFORM_UV_OFFSET1:
            FormatFloat2(out, &material->uv_upload_lanes[2]);
            break;
        case SHADER_UNIFORM_UV_OFFSET2:
            FormatFloat2(out, &material->uv_upload_lanes[4]);
            break;
        case SHADER_UNIFORM_UV_OFFSET3:
            FormatFloat2(out, &material->uv_upload_lanes[6]);
            break;
        case SHADER_UNIFORM_GOOCH_PARAMS:
            FormatFloat4(out, material->parameter_1b8, material->parameter_1bc, 0.0f, 0.0f);
            break;
    }
}

/* No caller. */
bool PC_SHADER_MATERIAL_FILTER_VIEW::LacksVariantAndLayerFlag12() const {
    i32 flagged = variant != 0 || (descriptor->layer_flags & 0x1000) != 0;
    // MATCH-EXCEPTION: the u8 cast gives a byte test of the 0/1 value; flagged is 0 or 1 anyway.
    return (u8)flagged == 0;
}
