#include "saga.h"

extern "C" void __cdecl NuShaderObjectKeyGenerate3(u32 *key, const PC_SHADER_MATERIAL_FILTER_VIEW *filter,
                                                   i32 pixel_stage) {
    u8 block[104];
    const PC_SHADER_FILTER_DESC_VIEW *desc = filter->descriptor;
    u32 word;
    memset(block, 0, sizeof(block));
    block[0x14] = 1;
    if (!pixel_stage) {
        memcpy(block + 0xc, ShaderProgramHashPC, 8);
    }
    if (filter->variant == 0) {
        word = desc->layer_flags & 0xfffffe3f;
    } else {
        word = 0x1000;
    }
    for (i32 i = 0; i < 4; ++i) {
        block[4 + i] = (u8)(word >> (8 * i));
        block[8 + i] = (u8)(word >> (8 * i));
    }
    word = filter->GetVertexFlags();
    for (i32 j = 0; j < 4; ++j)
        block[j] = (u8)(word >> (8 * j));
    block[0x21] = filter->first_path != 0;
    block[0x22] = filter->deferred_path != 0;
    block[0x23] = filter->second_path != 0;
    if (filter->variant == 0 && (desc->layer_flags & 3) != 0)
        block[0x15] = desc->sampler_indices[0];
    if (filter->variant == 0 && desc->specular_threshold > filter->parameter)
        block[0x16] = desc->sampler_indices[1];
    block[0x17] = filter->HasDiffuse(0);
    for (i32 layer = 1; layer <= 3; ++layer)
        if (filter->HasDiffuse(layer))
            block[0x17 + layer] = desc->sampler_indices[layer + 1];
    block[0x1b] = filter->variant == 0 && (desc->layer_flags & 0x6000) != 0 ? desc->sampler_indices[5] : 0;
    for (i32 op = 1; op <= 3; ++op)
        if (filter->HasLayer(op))
            block[0x1b + op] = desc->blend_ops[op - 1];
    block[0x1f] = 1;
    block[0x20] = 1;
    if (filter->variant == 0 && desc->normal_threshold > filter->parameter) {
        if (desc->layer_flags & 1)
            block[0x24] = 2;
        else if (desc->layer_flags & 2)
            block[0x24] = 1;
    }
    if (filter->variant == 0 && (desc->layer_flags & 0x100000))
        block[0x25] = desc->surface_threshold > filter->parameter ? 2 : 1;
    block[0x26] = desc->extra_texture > 0;
    block[0x27] = desc->environment_texture > 0;
    if (filter->variant == 0 && (desc->flags & 0x80000000)) {
        block[0x28] = (u8)((desc->selectors >> 2) & 3);
        block[0x2b] = (desc->flags & 0x800000) ? 2 : 1;
    }
    block[0x29] = (desc->vertex_format & 0xc) != 0;
    block[0x2a] = desc->texture_units;
    block[0x2c] = 1;
    if (filter->material != NULL && filter->material->texture_id != 0) {
        NUTEXDIM_PC *texture = NuTextureGetDimensions(filter->material->texture_id);
        if (texture != NULL && texture->resource != NULL) {
            PC_TEXTURE_LEVEL_DESC_PC dimensions;
            texture->resource->methods->get_level_desc(texture->resource, 0, &dimensions);
            if (dimensions.format == 0x31545844)
                block[0x2c] = 0;
        }
    }
    *key = (CRC16::hashInverse(block, 104) << 16) | (CRC16::hash(block, 104) & 0xffff);
}
