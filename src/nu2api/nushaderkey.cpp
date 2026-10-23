#include "saga.h"

// The filter predicates below are small PC_SHADER_MATERIAL_FILTER_VIEW helpers that retail inlines here; each one
// rereads filter->descriptor.
#define KEY_LAYER_BIT(filter, bit) ((filter)->variant == 0 && (u8)(((filter)->descriptor->layer_flags >> (bit)) & 1))
#define KEY_TYPE_PLAIN(filter)                                                                                         \
    ((filter)->variant == 0 && (i32)(filter)->descriptor->flags < 0 && ((filter)->descriptor->flags & 0x800000) == 0)
#define KEY_TYPE_ALTERNATE(filter)                                                                                     \
    ((filter)->variant == 0 && (i32)(filter)->descriptor->flags < 0 && ((filter)->descriptor->flags & 0x800000) != 0)

union PC_SHADER_KEY_BLOCK_PC {
    u8 bytes[104];
    struct {
        u32 vertex_flags, layer_flags, layer_flags_copy, program_hash[2];
    } words;
};

extern "C" void __cdecl NuShaderObjectKeyGenerate3(u32 *key, const PC_SHADER_MATERIAL_FILTER_VIEW *filter,
                                                   i32 pixel_stage) {
    const PC_SHADER_FILTER_DESC_VIEW *desc = filter->descriptor;
    PC_SHADER_KEY_BLOCK_PC block;
    u64 layer_word;
    u32 layer_low;
    i32 vertex_flags;
    PC_TEXTURE_LEVEL_DESC_PC dimensions;
    memset(&block, 0, sizeof(block));
    block.bytes[0x14] = 1;
    if (pixel_stage) {
        block.words.program_hash[0] = 0;
        block.words.program_hash[1] = 0;
    } else {
        block.words.program_hash[0] = ShaderProgramHashPC[0];
        block.words.program_hash[1] = ShaderProgramHashPC[1];
    }
    if (filter->variant != 0)
        layer_word = 0x1000;
    else
        layer_word = *(const u64 *)&desc->layer_flags & ~(u64)0x1c0;
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
    block.bytes[0x15] = KEY_LAYER_BIT(filter, 0) || KEY_LAYER_BIT(filter, 1) ? desc->sampler_indices[0] : 0;
    block.bytes[0x16] = filter->variant == 0 && (u8)(filter->descriptor->specular_threshold > filter->parameter)
                            ? desc->sampler_indices[1]
                            : 0;
    block.bytes[0x17] = filter->HasDiffuse(0);
    block.bytes[0x18] = filter->HasDiffuse(1) ? desc->sampler_indices[2] : 0;
    block.bytes[0x19] = filter->HasDiffuse(2) ? desc->sampler_indices[3] : 0;
    block.bytes[0x1a] = filter->HasDiffuse(3) ? desc->sampler_indices[4] : 0;
    block.bytes[0x1b] = KEY_LAYER_BIT(filter, 13) || KEY_LAYER_BIT(filter, 14) ? desc->sampler_indices[5] : 0;
    block.bytes[0x1c] = filter->HasLayer(1) ? desc->blend_ops[0] : 0;
    block.bytes[0x1d] = filter->HasLayer(2) ? desc->blend_ops[1] : 0;
    block.bytes[0x1e] = filter->HasLayer(3) ? desc->blend_ops[2] : 0;
    block.bytes[0x20] = 1;
    block.bytes[0x1f] = 1;
    if (filter->variant == 0 && (filter->descriptor->layer_flags & 1) &&
        filter->descriptor->normal_threshold > filter->parameter)
        block.bytes[0x24] = 2;
    else
        block.bytes[0x24] = (filter->variant == 0 && (filter->descriptor->layer_flags & 2) &&
                             filter->descriptor->normal_threshold > filter->parameter) != 0;
    if (KEY_LAYER_BIT(filter, 20) && filter->descriptor->surface_threshold > filter->parameter)
        block.bytes[0x25] = 2;
    else
        block.bytes[0x25] =
            (KEY_LAYER_BIT(filter, 20) && filter->descriptor->surface_threshold <= filter->parameter) != 0;
    block.bytes[0x26] = filter->descriptor->extra_texture > 0;
    block.bytes[0x27] = filter->descriptor->environment_texture > 0;
    block.bytes[0x28] = KEY_TYPE_PLAIN(filter) || KEY_TYPE_ALTERNATE(filter) ? (u8)((desc->selectors >> 2) & 3) : 0;
    block.bytes[0x29] = 0;
    if (desc->vertex_format & 0xc)
        block.bytes[0x29] = 1;
    block.bytes[0x2a] = desc->texture_units;
    if (KEY_TYPE_PLAIN(filter))
        block.bytes[0x2b] = 1;
    else
        block.bytes[0x2b] = KEY_TYPE_ALTERNATE(filter) ? 2 : 0;
    block.bytes[0x2c] = 1;
    if (filter->material != NULL && filter->material->texture_id != 0) {
        NUTEXDIM_PC *texture = NuTextureGetDimensions(filter->material->texture_id);
        if (texture != NULL && texture->resource != NULL) {
            texture->resource->methods->get_level_desc(texture->resource, 0, &dimensions);
            if (dimensions.format == 0x31545844)
                block.bytes[0x2c] = 0;
        }
    }
    *key = (i16)CRC16::hashInverse(block.bytes, 104) << 16;
    *key |= (u16)CRC16::hash(block.bytes, 104);
}
