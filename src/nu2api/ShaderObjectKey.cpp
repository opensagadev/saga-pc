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

extern "C" u8 NuShaderKeyHash[16];

void PC_SHADER_MATERIAL_FILTER::Initialize(const NUSHADERMTLDESC *desc, const NUMTL *mtl, i32 input_flags, i32 input_parameter) {
    descriptor = desc;
    material = mtl;
    flags = input_flags;
    parameter = input_parameter;
    variant = input_flags & 3;
    /* Bit 18 of the vertex format is a one-bit field (shr 18, test dl 1). */
    first_path = variant == 0 && (input_flags & 0x10) != 0 && ((u8)(desc->vertex_format >> 18) & 1) == 0 &&
                 (((mtl->attribs.word >> 14) & 3) == 0 || ((mtl->attribs.word >> 14) & 3) == 2);
    second_path = (variant != 0 || (input_flags & 0x10) != 0) && ((u8)(desc->vertex_format >> 18) & 1) == 0 &&
                  (((mtl->attribs.word >> 14) & 3) == 0 || ((mtl->attribs.word >> 14) & 3) == 2);
    deferred_path = variant == 0 && (input_flags & 0x20) != 0 && ((u8)(desc->vertex_format >> 18) & 1) == 0 &&
                    (((mtl->attribs.word >> 14) & 3) == 0 || ((mtl->attribs.word >> 14) & 3) == 2);
    if ((mtl->attribs.word & 0xf) == 2) {
        first_path = 0;
        second_path = 0;
    }
    layer_count = variant == 0;
    if (first_path != 0 || (variant == 0 && (desc->layer_flags & PC_SHADER_LAYER_GLOW) != 0)) {
        ++layer_count;
    }
    if (second_path != 0) {
        ++layer_count;
    }
    if (deferred_path != 0) {
        ++layer_count;
    }
}

bool PC_SHADER_MATERIAL_FILTER::UnfilteredHasLayer(i32 which) const {
    switch (which) {
        case 0:
            return true;
        case 1:
            return (descriptor->layer_flags & PC_SHADER_LAYER_LAYER1) != 0 && descriptor->blend_ops[0] != 0xff;
        case 2:
            return (descriptor->layer_flags & PC_SHADER_LAYER_LAYER2) != 0 && descriptor->blend_ops[1] != 0xff;
        case 3:
            return (descriptor->layer_flags & PC_SHADER_LAYER_LAYER3) != 0 && descriptor->blend_ops[2] != 0xff;
        default:
            return false;
    }
}

bool PC_SHADER_MATERIAL_FILTER::HasDiffuse(i32 which) const {
    if (variant != 0 && descriptor->shadow_set == 0) {
        return false;
    }
    switch (which) {
        case 0:
            return descriptor->diffuse_map_ids[0] > parameter;
        case 1:
            return descriptor->diffuse_map_ids[1] > parameter;
        case 2:
            return descriptor->diffuse_map_ids[2] > parameter;
        case 3:
            return descriptor->diffuse_map_ids[3] > parameter;
        default:
            return false;
    }
}

i32 PC_SHADER_MATERIAL_FILTER::HasTexCoordSets(i32 count) const {
    if (variant != 0 && descriptor->shadow_set == 0) {
        return 0;
    }
    return (i32)((descriptor->vertex_format >> 9) & 3) >= count;
}

bool PC_SHADER_MATERIAL_FILTER::HasLayerVertexFlag8(i32 layer) const {
    if ((variant != 0 && descriptor->shadow_set == 0) || layer != 0) {
        return false;
    }
    return (descriptor->vertex_format >> 8) & 1 ? true : false;
}

bool PC_SHADER_MATERIAL_FILTER::HasLayer(i32 which) const {
    if (variant != 0) {
        return UnfilteredHasLayer(which) && which == 0 && (descriptor->shadow_set & 1) != 0;
    }
    return UnfilteredHasLayer(which);
}

u32 PC_SHADER_MATERIAL_FILTER::GetVertexFlags() const {
    u32 v = descriptor->vertex_format;
    u32 result = 0;
    u32 colour0;
    if ((u8)(v >> 18) & 1) {
        result = 1;
    }
    colour0 = (v >> 16) & 1;
    if ((colour0 || (v & 0x20000)) && !(v & 0x400000)) {
        result |= 2;
    }
    if (HasTexCoordSets(1)) {
        result |= 4;
    }
    if (HasTexCoordSets(2)) {
        result |= 8;
    }
    if (HasTexCoordSets(3)) {
        result |= 0x10;
    }
    if ((colour0 || (v & 0x20000)) && (v & 0x400000)) {
        result |= 0x40;
    }
    if ((u8)(v >> 19) & 1) {
        result |= 0x80;
    }
    if ((u8)(v >> 24) & 1) {
        result |= 0x100;
    }
    if ((u8)(v >> 25) & 1) {
        result |= 0x400;
    }
    if (variant == 0 && (u8)(descriptor->layer_flags >> 31)) {
        result |= 0x800;
    }
    if (HasLayerVertexFlag8(0)) {
        result |= 0x1000;
    }
    return result;
}

const NUSHADERMTLDESC *PC_SHADER_MATERIAL_FILTER::GetDescriptor() const {
    return descriptor;
}

const NUMTL *PC_SHADER_MATERIAL_FILTER::GetMaterial() const {
    return material;
}

bool PC_SHADER_MATERIAL_FILTER::HasBaseLayer() const {
    return true;
}

i32 PC_SHADER_MATERIAL_FILTER::HasSpecular() const {
    if (variant != 0) {
        return 0;
    }
    return descriptor->specular_map_tid > parameter;
}

bool PC_SHADER_MATERIAL_FILTER::HasBaseLighting() const {
    return true;
}

i32 PC_SHADER_MATERIAL_FILTER::HasFirstPath() const {
    return first_path != 0;
}

i32 PC_SHADER_MATERIAL_FILTER::HasDeferredPath() const {
    return deferred_path != 0;
}

i32 PC_SHADER_MATERIAL_FILTER::HasLayerFlag20() const {
    if (variant != 0) {
        return 0;
    }
    return (descriptor->layer_flags & PC_SHADER_LAYER_REFLECTIVITY2) != 0;
}

i32 PC_SHADER_MATERIAL_FILTER::HasLayerFlag1() const {
    if (variant != 0) {
        return 0;
    }
    return (descriptor->layer_flags & PC_SHADER_LAYER_PARALLAX_MAP) != 0;
}

i32 PC_SHADER_MATERIAL_FILTER::HasNormalLayer1() const {
    return variant == 0 && (descriptor->layer_flags & PC_SHADER_LAYER_PARALLAX_MAP) != 0 && descriptor->lightmap_tids[1] > parameter;
}

i32 PC_SHADER_MATERIAL_FILTER::HasLayerFlag0() const {
    if (variant != 0) {
        return 0;
    }
    return (descriptor->layer_flags & PC_SHADER_LAYER_NORMAL_MAP) != 0;
}

i32 PC_SHADER_MATERIAL_FILTER::HasNormalLayer0() const {
    return variant == 0 && (descriptor->layer_flags & PC_SHADER_LAYER_NORMAL_MAP) != 0 && descriptor->lightmap_tids[1] > parameter;
}

i32 PC_SHADER_MATERIAL_FILTER::HasLayerFlag14() const {
    if (variant != 0) {
        return 0;
    }
    return (descriptor->layer_flags & PC_SHADER_LAYER_VTF_NORMAL_MAP) != 0;
}

i32 PC_SHADER_MATERIAL_FILTER::HasLayerFlag13() const {
    if (variant != 0) {
        return 0;
    }
    return (descriptor->layer_flags & PC_SHADER_LAYER_VERTEX_DISPLACEMENT) != 0;
}

i32 PC_SHADER_MATERIAL_FILTER::HasFlag31With23() const {
    return variant == 0 && (descriptor->flags & 0x80000000) != 0 && (descriptor->flags & 0x800000) != 0;
}

i32 PC_SHADER_MATERIAL_FILTER::HasFlag31Without23() const {
    return variant == 0 && (descriptor->flags & 0x80000000) != 0 && (descriptor->flags & 0x800000) == 0;
}

i32 PC_SHADER_MATERIAL_FILTER::HasSecondPath() const {
    return second_path != 0;
}

bool PC_SHADER_MATERIAL_FILTER::HasExtraTexture() const {
    return descriptor->extra_texture > 0;
}

bool PC_SHADER_MATERIAL_FILTER::HasEnvironmentTexture() const {
    return descriptor->environment_texture > 0;
}

/* The key of a program described by flags alone. No caller. */
extern "C" void NuShaderObjectKeyGenerate4(u32 *key, u32 flags, i32) {
    PC_SHADER_KEY_BLOCK block;
    memset(&block, 0, sizeof(block));
    block.bytes[0x14] = 1;
    block.words.program_hash[0] = ShaderProgramHashPC[0];
    block.words.program_hash[1] = ShaderProgramHashPC[1];
    block.bytes[4] = (u8)flags;
    block.bytes[5] = (u8)(flags >> 8);
    block.bytes[6] = (u8)(flags >> 16);
    block.bytes[7] = (u8)(flags >> 24);
    block.bytes[8] = (u8)flags;
    block.bytes[9] = (u8)(flags >> 8);
    block.bytes[0xa] = (u8)(flags >> 16);
    block.bytes[0xb] = (u8)(flags >> 24);
    *key = (i16)CRC16::hashInverse(block.bytes, 104) << 16;
    *key |= (u16)CRC16::hash(block.bytes, 104);
}

extern "C" {
    u8 NuShaderKeyHash[16] = {0};
}

/* Installs the 16-byte hash of the uber shader source that every key mixes in. */
extern "C" void NuShaderObjectKeySetUberShaderHash(const u8 *hash) {
    if (hash == NULL) {
        hash = NuShaderKeyHash;
    }
    for (i32 i = 0; i < 16; i++) {
        reinterpret_cast<u8 *>(ShaderProgramHashPC)[i] = hash[i];
    }
}
