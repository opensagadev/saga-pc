#include "gamelib/util/CRC16.h"
#include "nu2api/nu3d/ShaderManager.h"
#include "nu2api/nu3d/ShaderObjectKey.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "gamelib/util/gamelib_util_types.h"
#include "nu2api/PCInput.h"
#include "nu2api/ShaderManagerHLSL.h"
#include "nu2api/nu3d/nushader.h"
#include "nu2api/nu3d/nushader_plain.h"
#include "nu2api/nu3d/nushadermanager_gen.h"
#include "nu2api/nu3d/nushaderobject.h"
#include "nu2api/nu3d/shaderSemantics.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include "nu2api/nuwin32/pcapi.h"
#include <string.h>
#include <new>
#include "legoapi/items/collect/bolts.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/shader_parser.h"
#include <stdio.h>
#include "nu2api/nucore/nustring.h"

// for the header: NuShaderObject::stage_flags is a bitfield (each path bit is stored with xor/and/xor).
struct NuShaderObjectPaths {
    u32 base : 1;
    u32 first : 1;
    u32 layer : 1;
    u32 second : 1;
    u32 unknown : 28;
};

i32 PC_SHADER_MATERIAL_FILTER::HasSurfaceLayerAbove() const {
    return variant == 0 && (descriptor->layer_flags & PC_SHADER_LAYER_REFLECTIVITY2) != 0 && descriptor->lightmap_tids[0] > parameter;
}

i32 PC_SHADER_MATERIAL_FILTER::HasSurfaceLayerAtOrBelow() const {
    return variant == 0 && (descriptor->layer_flags & PC_SHADER_LAYER_REFLECTIVITY2) != 0 && descriptor->lightmap_tids[0] <= parameter;
}

extern "C" {
    u32 ShaderProgramHashPC[4] = {1482109315U, 2845988928U, 4140572740U, 1211123114U};
}

extern "C" void NuShaderObjectKeyGenerate3(u32 *key, const PC_SHADER_MATERIAL_FILTER *filter, i32 pixel_stage) {
    const NUSHADERMTLDESC *desc = filter->descriptor;
    PC_SHADER_KEY_BLOCK block;
    u64 layer_word;
    u32 layer_low;
    i32 vertex_flags;
    PC_TEXTURE_LEVEL_DESC dimensions;
    memset(&block, 0, sizeof(block));
    block.bytes[0x14] = 1;
    if (pixel_stage) {
        block.words.program_hash[0] = 0;
        block.words.program_hash[1] = 0;
    } else {
        block.words.program_hash[0] = ShaderProgramHashPC[0];
        block.words.program_hash[1] = ShaderProgramHashPC[1];
    }
    if (filter->variant != 0) {
        layer_word = 0x1000;
    } else {
        layer_word = *(const u64 *)&desc->layer_flags & ~(u64)0x1c0;
    }
    layer_low = (u32)layer_word;
    block.bytes[4] = (u8)layer_low;
    block.bytes[5] = (u8)(layer_low >> 8);
    block.bytes[6] = (u8)(layer_low >> 16);
    block.bytes[7] = (u8)(layer_low >> 24);
    block.bytes[8] = (u8)layer_low;
    block.bytes[9] = (u8)(layer_low >> 8);
    block.bytes[0xa] = (u8)(layer_low >> 16);
    block.bytes[0xb] = (u8)(layer_low >> 24);
    vertex_flags = filter->GetVertexFlags();
    block.bytes[0] = (u8)vertex_flags;
    block.bytes[1] = (u8)(vertex_flags >> 8);
    block.bytes[2] = (u8)(vertex_flags >> 16);
    block.bytes[3] = (u8)(vertex_flags >> 24);
    block.bytes[0x21] = filter->first_path != 0;
    block.bytes[0x23] = filter->second_path != 0;
    block.bytes[0x22] = filter->deferred_path != 0;
    block.bytes[0x15] = filter->LayerBit(0) || filter->LayerBit(1) ? desc->sampler_indices[0] : 0;
    block.bytes[0x16] =
        (u8)(filter->variant == 0 ? filter->descriptor->specular_map_tid > filter->parameter : 0) ? desc->sampler_indices[1] : 0;
    block.bytes[0x17] = filter->HasDiffuse(0) != 0;
    block.bytes[0x18] = filter->HasDiffuse(1) ? desc->sampler_indices[2] : 0;
    block.bytes[0x19] = filter->HasDiffuse(2) ? desc->sampler_indices[3] : 0;
    block.bytes[0x1a] = filter->HasDiffuse(3) ? desc->sampler_indices[4] : 0;
    block.bytes[0x1b] = filter->LayerBit(13) || filter->LayerBit(14) ? desc->sampler_indices[5] : 0;
    block.bytes[0x1c] = filter->HasLayer(1) ? desc->blend_ops[0] : 0;
    block.bytes[0x1d] = filter->HasLayer(2) ? desc->blend_ops[1] : 0;
    block.bytes[0x1e] = filter->HasLayer(3) ? desc->blend_ops[2] : 0;
    block.bytes[0x20] = 1;
    block.bytes[0x1f] = 1;
    if (filter->LayerBit(0) && filter->descriptor->lightmap_tids[1] > filter->parameter) {
        block.bytes[0x24] = 2;
    } else {
        block.bytes[0x24] = (filter->LayerBit(1) && filter->descriptor->lightmap_tids[1] > filter->parameter) != 0;
    }
    if (filter->LayerBit(20) && filter->descriptor->lightmap_tids[0] > filter->parameter) {
        block.bytes[0x25] = 2;
    } else {
        block.bytes[0x25] = (filter->LayerBit(20) && filter->descriptor->lightmap_tids[0] <= filter->parameter) != 0;
    }
    block.bytes[0x26] = filter->descriptor->extra_texture > 0;
    block.bytes[0x27] = filter->descriptor->environment_texture > 0;
    block.bytes[0x28] = filter->IsPlainType() || filter->IsAlternateType() ? (u8)((desc->selectors >> 2) & 3) : 0;
    block.bytes[0x29] = 0;
    if (desc->vertex_format & 0xc) {
        block.bytes[0x29] = 1;
    }
    block.bytes[0x2a] = desc->texture_unit_count;
    if (filter->IsPlainType()) {
        block.bytes[0x2b] = 1;
    } else {
        block.bytes[0x2b] = filter->IsAlternateType() != 0 ? 2 : 0;
    }
    block.bytes[0x2c] = 1;
    if (filter->material != NULL && filter->material->shader.normal_map_tid != 0) {
        NUTEXDIM *texture = NuTextureGetDimensions(filter->material->shader.normal_map_tid);
        if (texture != NULL && texture->resource != NULL) {
            texture->resource->methods->get_level_desc(texture->resource, 0, &dimensions);
            if (dimensions.format == 0x31545844) {
                block.bytes[0x2c] = 0;
            }
        }
    }
    *key = (i16)CRC16::hashInverse(block.bytes, 104) << 16;
    *key |= (u16)CRC16::hash(block.bytes, 104);
}

/* Builds the filter view for a descriptor and generates its key. No caller. */
extern "C" void NuShaderObjectKeyGenerate(u32 *key, const NUSHADERMTLDESC *desc, const NUMTL *material, i32 flags, i32 variant,
                                          i32 pixel_stage) {
    PC_SHADER_MATERIAL_FILTER filter;
    filter.Initialize(desc, material, flags, variant);
    NuShaderObjectKeyGenerate3(key, &filter, pixel_stage);
}

bool ShaderCompilerDictionary::GetForceMode() const {
    return force_mode;
}

i32 ShaderCompilerDictionary::GetProfile() const {
    return profile;
}

PC_SHADER_MATERIAL_FILTER::PC_SHADER_MATERIAL_FILTER(const NUSHADERMTLDESC *descriptor, i32 flags, i32 parameter) {
    Initialize(descriptor, NULL, flags, parameter);
}

void ShaderCompilerDictionary::SetForceMode(bool force) {
    force_mode = force;
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

void ShaderCompilerBase::AddDictionaryRegisterBlock(const char *name, i32 count) {
    dictionary.AddRegisterBlock(name, count);
}

void ShaderCompilerBase::SetForceMode(bool force) {
    dictionary.force_mode = force;
}

bool ShaderCompilerBase::GetDictionaryForceMode() {
    return dictionary.force_mode;
}

i32 ShaderCompilerBase::GetDictionaryProfile() {
    return dictionary.profile;
}

void ShaderCompilerBase::Prepare(const NUSHADERMTLDESC *descriptor, ShaderCompilerDictionary *dictionary) {
    PC_SHADER_MATERIAL_FILTER filter;
    filter.Initialize(descriptor, NULL, 0, 0);
    Configure(&filter, dictionary);
}

HLSLShaderBuilder::HLSLShaderBuilder(VirtualStackAllocator &allocator) : ShaderCompilerBase(allocator, 0) {
}

void NuShaderObject::InitHLSL(const u32 *key, i32 index, const char *vertex_source, i32 vertex_length, const char *pixel_source,
                              i32 pixel_length, i32 version) {
    PCShaderObjectInitHLSLPc(this, key, index, vertex_source, vertex_length, pixel_source, pixel_length, version);
}

i32 NuShaderObject::GetMarker() {
    return marker;
}

extern "C" i32 NuShaderManagerGetHardwareVertexProcessing(void) {
    return PCDeviceHardwareVertexProcessingPC;
}

void ShaderManagerHLSLPc::AdaptDescriptor(const NUSHADERMTLDESC *input, const NUMTL *input_material) {
    NUSHADERMTLDESC *desc = const_cast<NUSHADERMTLDESC *>(input);
    NUMTL *material = const_cast<NUMTL *>(input_material);
    if ((desc->vertex_format & 0x4000000) != 0) {
        desc->layer_flags |= PC_SHADER_LAYER_PRELIGHT_FX;
        desc->flags |= 0x200000;
    }
    i32 version = compiler->dictionary.profile;
    if (PcWindowOptions.usehires == 0) {
        version = PC_SHADER_MODEL_1_1;
    }
    switch (version) {
        case PC_SHADER_MODEL_2_X:
            desc->flags &= 0xff77dffb;
            desc->layer_flags &= 0xe7fadffb;
            break;
        case PC_SHADER_MODEL_2_0:
            desc->flags &= 0xff7efffb;
            desc->layer_flags &= 0xbffbfffb;
            if ((desc->flags & 0x20) != 0) {
                desc->flags &= ~8U;
                desc->layer_flags &= ~PC_SHADER_LAYER_PHONG;
            }
            if ((desc->flags & 0x20) != 0) {
                desc->flags &= ~0x20000U;
                desc->layer_flags &= ~PC_SHADER_LAYER_SHINEMAP;
            }
            if ((desc->flags & 0x20) != 0) {
                desc->flags &= ~3U;
                desc->layer_flags &= ~(PC_SHADER_LAYER_NORMAL_MAP | PC_SHADER_LAYER_PARALLAX_MAP);
            }
            if ((desc->flags & 0x20000) != 0 || (desc->layer_flags & PC_SHADER_LAYER_SHINEMAP) != 0) {
                desc->flags &= ~8U;
                desc->layer_flags &= ~PC_SHADER_LAYER_PHONG;
            }
            if ((desc->flags & 3) != 0) {
                desc->flags &= 0xbfffffef;
                desc->layer_flags &= 0xfdffffef;
            }
            if ((desc->flags & 2) != 0) {
                desc->flags = (desc->flags & ~2U) | 1;
                desc->layer_flags = (desc->layer_flags & ~PC_SHADER_LAYER_PARALLAX_MAP) | PC_SHADER_LAYER_NORMAL_MAP;
            }
            if ((desc->flags & 0x60) == 0x60) {
                desc->flags &= ~1U;
                desc->layer_flags &= ~PC_SHADER_LAYER_NORMAL_MAP;
            }
            if ((desc->flags & 0x80) != 0) {
                desc->flags &= ~1U;
                desc->layer_flags &= ~PC_SHADER_LAYER_NORMAL_MAP;
            }
            if ((desc->flags & 0x25) == 0x25) {
                desc->flags &= ~1U;
                desc->layer_flags &= ~PC_SHADER_LAYER_NORMAL_MAP;
            }
            if ((desc->flags & 1) != 0 && desc->lightmap_tids[1] != 0) {
                desc->flags &= ~1U;
                desc->layer_flags &= ~PC_SHADER_LAYER_NORMAL_MAP;
            }
            desc->flags &= 0xffff9bff;
            desc->layer_flags &= 0xffff9bff;
            if ((desc->layer_flags & 0x4000000) != 0 && (desc->layer_flags & PC_SHADER_LAYER_PHONG) != 0 &&
                (desc->layer_flags & PC_SHADER_LAYER_PRELIGHT_FX) != 0 && (desc->layer_flags & PC_SHADER_LAYER_NORMAL_MAP) != 0) {
                desc->flags &= ~8U;
                desc->layer_flags &= ~PC_SHADER_LAYER_PHONG;
            }
            desc->flags &= 0xfdffffef;
            desc->layer_flags &= 0xffefffef;
            if ((desc->flags & 0x49) == 0x49) {
                desc->flags &= ~1U;
                desc->layer_flags &= ~PC_SHADER_LAYER_NORMAL_MAP;
            }
            if ((desc->flags & 0x100) != 0 && (desc->flags & 8) != 0) {
                desc->flags &= ~8U;
                desc->layer_flags &= ~PC_SHADER_LAYER_PHONG;
            }
            if ((desc->flags & 0x80201000) == 0x80201000) {
                desc->flags &= ~9U;
                desc->layer_flags &= ~(PC_SHADER_LAYER_NORMAL_MAP | PC_SHADER_LAYER_PHONG);
            }
            desc->flags &= 0xfff7ffff;
            desc->layer_flags &= 0xe7feffff;
            break;
        case PC_SHADER_MODEL_1_1: {
            bool layer = (desc->layer_flags & PC_SHADER_LAYER_REFRACTION) != 0;
            if ((desc->flags & 0x40200) == 0) {
                desc->flags &= ~0x400U;
            }
            desc->layer_flags &= ~PC_SHADER_LAYER_REFRACTION;
            if (layer && (desc->layer_flags & PC_SHADER_LAYER_REFRACTION) == 0 && (material->attribs.word & 0x700000) != 0x100000) {
                material->attribs.word &= 0x807fffff;
            }
            if ((desc->flags & 0x4000) != 0) {
                desc->shininess = (f32)(desc->shininess * (f64)0.4f);
            }
            desc->flags &= 0xffff9ffc;
            desc->layer_flags &= 0xffff9ffc;
            if ((desc->flags & 0x10000) == 0) {
                if ((desc->flags & 0x20) != 0) {
                    desc->flags &= ~8U;
                    desc->layer_flags &= ~PC_SHADER_LAYER_PHONG;
                }
            } else {
                desc->flags &= ~0x20U;
                desc->layer_flags &= ~PC_SHADER_LAYER_ENVMAP_CUBE;
            }
            if ((desc->flags & 3) != 0) {
                desc->flags &= ~8U;
                desc->layer_flags &= ~PC_SHADER_LAYER_PHONG;
            }
            desc->flags &= 0xf916fffb;
            desc->layer_flags &= 0x27c8fffb;
            if ((desc->layer_flags & 0x4000000) != 0) {
                desc->flags &= ~0x28U;
                desc->layer_flags &= ~(PC_SHADER_LAYER_PHONG | PC_SHADER_LAYER_ENVMAP_CUBE);
            }
            break;
        }
    }
    if (compiler->dictionary.force_mode) {
        desc->flags &= 0xfffffbd7;
        desc->layer_flags &= 0xfffffbd7;
    }
}

bool ShaderManagerHLSL::CreateShader(const PC_SHADER_MATERIAL_FILTER *filter, bool pixel_stage, NuShaderObject *shader, i32 index) {
    u32 key = 0;
    NuShaderObjectKeyGenerate3(&key, filter, pixel_stage != 0);
    vertex_source.length = 0;
    pixel_source.length = 0;
    compiler->Generate(const_cast<PC_SHADER_MATERIAL_FILTER *>(filter), &vertex_source, &pixel_source);
    PCShaderObjectInitHLSLPc(shader, &key, index, vertex_source.data, vertex_source.length, pixel_source.data, pixel_source.length,
                             compiler->dictionary.profile);
    NuShaderObjectPaths *paths = reinterpret_cast<NuShaderObjectPaths *>(&shader->stage_flags);
    paths->base = filter->variant == 0;
    paths->first = filter->first_path != 0;
    paths->layer = filter->variant != 0 ? false : (bool)((filter->descriptor->layer_flags >> 16) & 1);
    paths->second = filter->second_path != 0;
    return true;
}
