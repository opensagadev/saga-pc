#include "saga.h"

struct PC_SHADER_FILTER_DESC_VIEW {
    u32 flags;
    i32 diffuse_maps[4];
    u8 unknown_014[0x20];
    i32 surface_threshold, normal_threshold;
    u8 unknown_03c[8];
    u8 blend_ops[3], unknown_047;
    i32 specular_threshold;
    u8 unknown_04c[0x5c];
    u8 texture_units, sampler_indices[6], unknown_0af[0x8d];
    u32 vertex_format;
    u8 unknown_140[0x74];
    u8 override_mode, unknown_1b5[3];
    u32 layer_flags, selectors;
    u8 unknown_1c0[0x24];
    i32 extra_texture, environment_texture;
};
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, diffuse_maps) == 4);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, surface_threshold) == 0x34);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, normal_threshold) == 0x38);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, blend_ops) == 0x44);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, specular_threshold) == 0x48);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, texture_units) == 0xa8);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, vertex_format) == 0x13c);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, override_mode) == 0x1b4);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, layer_flags) == 0x1b8);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, selectors) == 0x1bc);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, extra_texture) == 0x1e4);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_DESC_VIEW, environment_texture) == 0x1e8);
struct PC_SHADER_FILTER_MATERIAL_VIEW {
    u8 unknown_000[0x40];
    u32 blend_flags;
    u8 unknown_044[0xbc];
    i32 texture_id;
};
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_MATERIAL_VIEW, blend_flags) == 0x40);
STATIC_ASSERT(offsetof(PC_SHADER_FILTER_MATERIAL_VIEW, texture_id) == 0x100);
struct PC_SHADER_MATERIAL_FILTER_VIEW {
    const PC_SHADER_FILTER_DESC_VIEW *descriptor;
    const PC_SHADER_FILTER_MATERIAL_VIEW *material;
    i32 flags, variant, first_path, deferred_path, second_path, layer_count, parameter;
    void __thiscall Initialize(const PC_SHADER_FILTER_DESC_VIEW *, const PC_SHADER_FILTER_MATERIAL_VIEW *, i32, i32);
    bool __thiscall UnfilteredHasLayer(i32) const;
    bool __thiscall HasDiffuse(i32) const;
    bool __thiscall HasLayer(i32) const;
    u32 __thiscall GetVertexFlags() const;
};
STATIC_ASSERT(sizeof(PC_SHADER_MATERIAL_FILTER_VIEW) == 0x24);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_FILTER_VIEW, parameter) == 0x20);

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
    block[0x1b] = filter->HasDiffuse(0);
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
