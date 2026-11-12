#define SAGA_SHADER_PARSER_TYPES
#include "saga.h"
#include <stdio.h>
#include <string.h>

void ShaderCompilerBasePc::Prepare(const PC_SHADER_FILTER_DESC_VIEW *descriptor,
                                   ShaderCompilerDictionaryPc *dictionary) {
    PC_SHADER_MATERIAL_FILTER_VIEW filter;
    filter.Initialize(descriptor, NULL, 0, 0);
    Configure(&filter, dictionary);
}
void ShaderCompilerBasePc::Configure(PC_SHADER_MATERIAL_FILTER_VIEW *filter, ShaderCompilerDictionaryPc *definitions) {
    u8 used_uvs[4] = {0, 0, 0, 0};
    const PC_SHADER_FILTER_DESC_VIEW *descriptor = filter->descriptor;
    const i32 profile = definitions->profile;
    const char *uv_functions[4] = {"FUNC_UVSET_0", "FUNC_UVSET_1", "FUNC_UVSET_2", "FUNC_UVSET_3"};
    const char *uv_names[4] = {"varying_uvSet0", "varying_uvSet1", "varying_uvSet2", "varying_uvSet3"};
    const char *vertex_uvs[4] = {"vs_uvSet0", "vs_uvSet1", "vs_uvSet2", "vs_uvSet3"};
    const char *combine[5] = {"DISABLE", "COMBINE_OVER", "COMBINE_ADD", "COMBINE_SUBTRACT", "COMBINE_MULTIPLY"};
    const char *numbers[17] = {"0", "1",  "2",  "3",  "4",  "5",  "6",  "7", "8",
                               "9", "10", "11", "12", "13", "14", "15", "16"};
    if (profile == 4) {
        definitions->AddDefinition("VERTEX_SHADER_VERSION", "30", 0);
        definitions->AddDefinition("PIXEL_SHADER_VERSION", "30", 0);
    } else if (profile == 3) {
        definitions->AddDefinition("VERTEX_SHADER_VERSION", "21", 0);
        definitions->AddDefinition("PIXEL_SHADER_VERSION", "21", 0);
    } else if (profile == 2) {
        definitions->AddDefinition("VERTEX_SHADER_VERSION", "20", 0);
        definitions->AddDefinition("PIXEL_SHADER_VERSION", "20", 0);
    } else if (profile == 1) {
        definitions->AddDefinition("VERTEX_SHADER_VERSION", "11", 0);
        definitions->AddDefinition("PIXEL_SHADER_VERSION", "11", 0);
    } else {
        definitions->AddDefinition("VERTEX_SHADER_VERSION", "0", 0);
        definitions->AddDefinition("PIXEL_SHADER_VERSION", "0", 0);
    }
    definitions->AddDefinition("BUILD_EFFECT", NULL, 0);
    bool nvidia_profile = profile == 4 && 0x10de == PCDeviceBackend.identifier.vendor;
    if (filter->variant == 0 && (filter->descriptor->layer_flags & 0x4000)) {
        used_uvs[descriptor->sampler_indices[5] - 1] = 1;
        definitions->AddDefinition(uv_functions[descriptor->sampler_indices[5] - 1], "UVSET_ANIMATION", 0);
        definitions->AddDefinition("VTF_NORMAL_UVSET", uv_names[descriptor->sampler_indices[5] - 1], 1);
        definitions->AddDefinition("FUNC_fs_computeVTFNormal", "VTF_SURFACE_NORMAL_MAP", 0);
    }
    if (0 == filter->variant && (filter->descriptor->layer_flags & 0x40000000))
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
    }
    if (!nvidia_profile || filter->variant != 0 || !(filter->descriptor->layer_flags & 0x2000)) {
        if (0 == filter->variant && (filter->descriptor->layer_flags & 0x20000000) &&
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
        if ((0 == filter->variant || filter->descriptor->override_mode != 0) &&
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
            ((filter->descriptor->vertex_format >> 9) & 3) > 2) {
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
    if (filter->variant == 0 && (0x10000000 & filter->descriptor->layer_flags))
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

void ShaderCompilerHLSLPc::Prepare(const PC_SHADER_FILTER_DESC_VIEW *descriptor,
                                   ShaderCompilerDictionaryPc *dictionary) {
    PC_SHADER_MATERIAL_FILTER_VIEW filter;
    filter.Initialize(descriptor, NULL, 0, 0);
    Configure(&filter, dictionary);
}
void ShaderCompilerHLSLPc::Configure(PC_SHADER_MATERIAL_FILTER_VIEW *filter, ShaderCompilerDictionaryPc *dictionary) {
    ShaderCompilerBasePc::Configure(filter, dictionary);
    dictionary->AddDefinition("BUILD_VERTEX_SHADER", NULL, 0);
    dictionary->AddDefinition("BUILD_PIXEL_SHADER", NULL, 0);
    dictionary->AddDefinition("HLSL_LANGUAGE", NULL, 0);
    dictionary->AddDefinition("COLOR_FACTOR", "2.0", 1);
}

ShaderBuilderGen::ShaderBuilderGen(VirtualStackAllocator &parent, i32 mode)
    : unknown_204(0), parsed_source(NULL), dictionary(parent, mode), source_buffer(parent, 0x2d000),
      parser_memory(parent, 0x32000) {
    save_directory[0] = 0;
}
ShaderBuilderGen::~ShaderBuilderGen() {
    parsed_source = NULL;
}

HLSLShaderBuilder::HLSLShaderBuilder(VirtualStackAllocator &allocator) : ShaderCompilerBasePc(allocator, 0) {
}

void ShaderCompilerBasePc::SetIncludeDirectories(const char *paths) {
    strcpy(include_directories, paths);
}
void ShaderCompilerBasePc::SetSaveDirectory(const char *path) {
    strcpy(save_directory, path);
}
void ShaderCompilerBasePc::SetProfile(i32 profile, i32 first, i32 second) {
    dictionary.SetProfile(profile, first, second);
}
void ShaderCompilerBasePc::SetForceMode(bool force) {
    dictionary.force_mode = force;
}

void *ShaderCompilerBasePc::ParseSource(const char *source, i32 length, ShaderCompilerBufferAllocatorPc *buffer,
                                        i32 prefix, VirtualStackAllocator *memory) {
    if (length < 0)
        length = (i32)strlen(source);
    i32 size = prefix + length;
    buffer->length = 0;
    if (buffer->capacity < size)
        buffer->Resize(size);
    buffer->length = size;
    strncpy(reinterpret_cast<char *>(buffer->data + prefix), source, length);
    memset(buffer->data, ' ', prefix);
    for (i32 i = 0; i < prefix; i += 100)
        buffer->data[i] = '\n';
    ShaderCompilerParserPc parser;
    parser.SetIncludeDirectories(include_directories);
    return parser.Parse(buffer, prefix, 1, memory);
}
bool ShaderCompilerBasePc::LoadSource(const char *source, i32 length) {
    parsed_source = ParseSource(source, length, &source_buffer, 0x5000, &parser_memory);
    return parsed_source != NULL;
}
void ShaderCompilerBasePc::Generate(PC_SHADER_MATERIAL_FILTER_VIEW *filter, ShaderCompilerBufferAllocatorPc *vertex,
                                    ShaderCompilerBufferAllocatorPc *pixel) {
    ShaderCompilerDictionaryPc *dict = &dictionary;
    for (i32 i = 0; i < dict->definition_count; ++i)
        dict->definitions[i]->expansion_state = 0;
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
            if (vertex->length + 1 > vertex->capacity)
                vertex->Resize((3 * (vertex->length + 1)) / 2);
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
            if (pixel->length + 1 > pixel->capacity)
                pixel->Resize((3 * (pixel->length + 1)) / 2);
            pixel->data[pixel->length] = 0;
            ++pixel->length;
        }
    }
}

/* Writes data to "<save directory>\<name><suffix>"; no retail caller. */
void ShaderCompilerBasePc::SaveFile(const char *suffix, const char *name, const void *data, i32 size) {
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
bool ShaderNextDirectoryPc(const char *list, i32 *position, char *entry) {
    if (*position >= (i32)strlen(list))
        return false;
    list += *position;
    while (*list != 0 && *list != ';') {
        *entry++ = *list++;
        ++*position;
    }
    *entry = 0;
    if (*list == ';')
        ++*position;
    return true;
}

/* Diagnostics compiled out of retail; varargs keeps the calls. */
static void ShaderBuilderTracePc(...) {
}

void *ShaderCompilerBasePc::ParseFile(const char *name, ShaderCompilerBufferAllocatorPc *buffer, i32 prefix,
                                      VirtualStackAllocator *memory) {
    char directory[0x100];
    char path[0x100];
    i32 position = 0;
    NUFILE file = 0;
    NUDATHDR *dat = NuDatGet();
    i32 size;
    NuDatSet(NULL);
    do {
        if (!ShaderNextDirectoryPc(include_directories, &position, directory))
            break;
        sprintf(path, "%s\\%s", directory, name);
        file = NuFileOpen(path, 0);
    } while (file == 0);
    NuDatSet(dat);
    if (file == 0)
        ShaderBuilderTracePc(path, include_directories);
    size = (i32)NuFileOpenSize(file);
    if (size <= 0) {
        ShaderBuilderTracePc();
        return NULL;
    }
    buffer->length = 0;
    if (buffer->capacity < prefix + size)
        buffer->Resize(prefix + size);
    buffer->length = prefix + size;
    NuFileRead(file, buffer->data + prefix, size);
    NuFileClose(file);
    memset(buffer->data, ' ', prefix);
    for (i32 i = 0; i < prefix; i += 100)
        buffer->data[i] = '\n';
    ShaderCompilerParserPc parser;
    parser.SetIncludeDirectories(directory);
    return parser.Parse(buffer, prefix, 1, memory);
}

bool ShaderCompilerBasePc::LoadFile(const char *name) {
    parsed_source = ParseFile(name, &source_buffer, 0x5000, &parser_memory);
    return parsed_source != NULL;
}

/* Method table of ID3DXEffectCompiler as far as it is called here. */
typedef struct D3DX_EFFECT_COMPILER_METHODS_PC {
    HRESULT(__stdcall *query_interface)(ID3DXEffectCompiler *, const IID *, void **);
    ULONG(__stdcall *add_ref)(ID3DXEffectCompiler *);
    ULONG(__stdcall *release)(ID3DXEffectCompiler *);
    void *unknown_0c[56];
    HRESULT(__stdcall *compile_effect)(ID3DXEffectCompiler *, DWORD, ID3DXBuffer **, ID3DXBuffer **);
} D3DX_EFFECT_COMPILER_METHODS_PC;
struct ID3DXEffectCompiler {
    const D3DX_EFFECT_COMPILER_METHODS_PC *methods;
};

/* Compiles an effect from source into output; returns the compiled size. No retail caller. */
i32 ShaderCompilerBasePc::CompileEffect(const char *source, i32 length, void *output, i32 unused) {
    ID3DXBuffer *effect;
    ID3DXEffectCompiler *compiler;
    i32 size;
    D3DXCreateBuffer(0x80, &effect);
    D3DXCreateEffectCompiler(source, length, NULL, NULL, 0, &compiler, NULL);
    compiler->methods->compile_effect(compiler, 0, &effect, NULL);
    size = effect->methods->get_buffer_size(effect);
    PCByteCopy_candidate(output, effect->methods->get_buffer_pointer(effect), size);
    effect->methods->release(effect);
    if (compiler != NULL)
        compiler->methods->release(compiler);
    return size;
}
/* The material fields from its texture flags on (PC_SHADER_MATERIAL_DISPATCH_VIEW + 0xb4), as the shader
 * builder's parameter defaults read them. */
typedef struct PC_SHADER_DEFAULTS_VIEW_PC {
    u32 texture_flags;
    i32 diffuse_texture_ids[4];
    u32 channel_colours[4];
    f32 parameter_d8;
    f32 parameter_dc;
    f32 parameter_e0;
    f32 parameter_e4;
    i32 height_texture_id;
    i32 normal_texture_id;
    f32 texture_lerp;
    u32 reflection_colour;
    u8 unknown_0f8[0x3];
    u8 flip_sign;
    u8 unknown_0fc[0x1c];
    f32 parameter_118;
    u32 diffuse_colour;
    u32 specular_colour;
    u32 tint_colour;
    u32 shadow_colour;
    f32 parameter_12c;
    f32 parameter_130;
    f32 parameter_134;
    f32 parameter_138;
    f32 parameter_13c;
    f32 parameter_140;
    f32 parameter_144;
    f32 parameter_148;
    f32 parameter_14c;
    f32 parameter_150;
    u8 unknown_154[0x60];
    f32 parameter_1b4;
    f32 parameter_1b8;
    f32 parameter_1bc;
    u8 unknown_1c0[0x10];
    f32 uv_upload_lanes[8];
} PC_SHADER_DEFAULTS_VIEW_PC;
STATIC_ASSERT(offsetof(PC_SHADER_DEFAULTS_VIEW_PC, channel_colours) == 0xc8 - 0xb4);
STATIC_ASSERT(offsetof(PC_SHADER_DEFAULTS_VIEW_PC, texture_lerp) == 0xf0 - 0xb4);
STATIC_ASSERT(offsetof(PC_SHADER_DEFAULTS_VIEW_PC, flip_sign) == 0xfb - 0xb4);
STATIC_ASSERT(offsetof(PC_SHADER_DEFAULTS_VIEW_PC, parameter_118) == 0x118 - 0xb4);
STATIC_ASSERT(offsetof(PC_SHADER_DEFAULTS_VIEW_PC, shadow_colour) == 0x128 - 0xb4);
STATIC_ASSERT(offsetof(PC_SHADER_DEFAULTS_VIEW_PC, parameter_150) == 0x150 - 0xb4);
STATIC_ASSERT(offsetof(PC_SHADER_DEFAULTS_VIEW_PC, parameter_1b4) == 0x1b4 - 0xb4);
STATIC_ASSERT(offsetof(PC_SHADER_DEFAULTS_VIEW_PC, uv_upload_lanes) == 0x1d0 - 0xb4);

static void FormatFloatPc(char *out, f32 value) {
    sprintf(out, "%f", value);
}

static void FormatFloat2Pc(char *out, const f32 *value) {
    sprintf(out, "float2(%f, %f)", value[0], value[1]);
}

static void FormatFloat4Pc(char *out, f32 x, f32 y, f32 z, f32 w) {
    sprintf(out, "float4(%f, %f, %f, %f)", x, y, z, w);
}

static void FormatColourPc(i32 colour, char *out) {
    sprintf(out, "float4(%f, %f, %f, %f)", (f32)(colour & 0xff) / 255.0f, (f32)((colour >> 8) & 0xff) / 255.0f,
            (f32)((colour >> 16) & 0xff) / 255.0f, (f32)((colour >> 24) & 0xff) / 255.0f);
}

/* The default value of shader parameter id as an HLSL literal; ids without one leave out untouched. */
void __stdcall ShaderParameterDefaultPc(const PC_SHADER_DEFAULTS_VIEW_PC *material, i32 id, char *out, i32 unused) {
    switch (id) {
        case 0:
            FormatColourPc(material->diffuse_colour, out);
            break;
        case 2: {
            i32 colour = material->specular_colour;
            FormatFloat4Pc(out, (f32)(colour & 0xff) / 255.0f, (f32)((colour >> 8) & 0xff) / 255.0f,
                           (f32)((colour >> 16) & 0xff) / 255.0f, material->parameter_1b4);
            break;
        }
        case 9:
            FormatFloatPc(out, material->flip_sign ? 1.0f : 0.0f);
            break;
        case 10:
            FormatFloat4Pc(out, material->parameter_134, 0.0f, material->parameter_138, material->parameter_14c);
            break;
        case 11:
            FormatFloat4Pc(out, material->texture_lerp, 0.0f, 0.0f, 0.0f);
            break;
        case 12:
            FormatFloat4Pc(out, material->parameter_130, material->parameter_12c, material->parameter_144,
                           material->parameter_148);
            break;
        case 13:
            FormatFloat4Pc(out, material->parameter_13c, material->parameter_140, 0.0f, 0.0f);
            break;
        case 15:
            FormatFloatPc(out, material->parameter_118);
            break;
        case 51:
            FormatColourPc(material->channel_colours[0], out);
            break;
        case 52:
            FormatColourPc(material->channel_colours[1], out);
            break;
        case 53:
            FormatColourPc(material->channel_colours[2], out);
            break;
        case 54:
            FormatColourPc(material->channel_colours[3], out);
            break;
        case 55:
            FormatFloat4Pc(out, material->parameter_d8, material->parameter_dc, material->parameter_e0,
                           material->parameter_e4);
            break;
        case 57:
            FormatColourPc(material->shadow_colour, out);
            break;
        case 59:
            FormatColourPc(material->tint_colour, out);
            break;
        case 60:
            FormatFloatPc(out, (material->parameter_150 - 1.0) * (f64)0.1f);
            break;
        case 64:
            FormatFloat2Pc(out, &material->uv_upload_lanes[0]);
            break;
        case 65:
            FormatFloat2Pc(out, &material->uv_upload_lanes[2]);
            break;
        case 66:
            FormatFloat2Pc(out, &material->uv_upload_lanes[4]);
            break;
        case 67:
            FormatFloat2Pc(out, &material->uv_upload_lanes[6]);
            break;
        case 16:
            FormatFloat4Pc(out, material->parameter_1b8, material->parameter_1bc, 0.0f, 0.0f);
            break;
    }
}

/* No retail caller. */
bool PC_SHADER_MATERIAL_FILTER_VIEW::LacksVariantAndLayerFlag12() const {
    i32 flagged = variant != 0 || (descriptor->layer_flags & 0x1000) != 0;
    // MATCH-EXCEPTION: the u8 cast reproduces retail's byte test of the 0/1 value; flagged is 0 or 1 anyway.
    return (u8)flagged == 0;
}
