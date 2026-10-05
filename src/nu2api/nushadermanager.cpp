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

void __thiscall PC_SHADER_MATERIAL_FILTER_VIEW::Initialize(const PC_SHADER_FILTER_DESC_VIEW *desc,
                                                           const PC_SHADER_FILTER_MATERIAL_VIEW *mtl, i32 input_flags,
                                                           i32 input_parameter) {
    descriptor = desc;
    material = mtl;
    flags = input_flags;
    parameter = input_parameter;
    variant = input_flags & 3;
    first_path = variant == 0 && (input_flags & 0x10) != 0 && (desc->vertex_format & 0x40000) == 0 &&
                 (((mtl->blend_flags >> 14) & 3) == 0 || ((mtl->blend_flags >> 14) & 3) == 2);
    second_path = (variant != 0 || (input_flags & 0x10) != 0) && (desc->vertex_format & 0x40000) == 0 &&
                  (((mtl->blend_flags >> 14) & 3) == 0 || ((mtl->blend_flags >> 14) & 3) == 2);
    deferred_path = variant == 0 && (input_flags & 0x20) != 0 && (desc->vertex_format & 0x40000) == 0 &&
                    (((mtl->blend_flags >> 14) & 3) == 0 || ((mtl->blend_flags >> 14) & 3) == 2);
    if ((mtl->blend_flags & 0xf) == 2) {
        first_path = 0;
        second_path = 0;
    }
    layer_count = variant == 0;
    if (first_path != 0 || (variant == 0 && (desc->layer_flags & 0x10000) != 0))
        ++layer_count;
    if (second_path != 0)
        ++layer_count;
    if (deferred_path != 0)
        ++layer_count;
}
bool __thiscall PC_SHADER_MATERIAL_FILTER_VIEW::UnfilteredHasLayer(i32 which) const {
    switch (which) {
        case 0:
            return true;
        case 1:
            return (descriptor->layer_flags & 0x40) != 0 && descriptor->blend_ops[0] != 0xff;
        case 2:
            return (descriptor->layer_flags & 0x80) != 0 && descriptor->blend_ops[1] != 0xff;
        case 3:
            return (descriptor->layer_flags & 0x100) != 0 && descriptor->blend_ops[2] != 0xff;
        default:
            return false;
    }
}
bool __thiscall PC_SHADER_MATERIAL_FILTER_VIEW::HasDiffuse(i32 which) const {
    if (variant != 0 && descriptor->override_mode == 0)
        return false;
    switch (which) {
        case 0:
            return descriptor->diffuse_maps[0] > parameter;
        case 1:
            return descriptor->diffuse_maps[1] > parameter;
        case 2:
            return descriptor->diffuse_maps[2] > parameter;
        case 3:
            return descriptor->diffuse_maps[3] > parameter;
        default:
            return false;
    }
}
bool __thiscall PC_SHADER_MATERIAL_FILTER_VIEW::HasLayer(i32 which) const {
    if (variant == 0)
        return UnfilteredHasLayer(which);
    return UnfilteredHasLayer(which) && which == 0 && (descriptor->override_mode & 1) != 0;
}
u32 __thiscall PC_SHADER_MATERIAL_FILTER_VIEW::GetVertexFlags() const {
    u32 v = descriptor->vertex_format;
    bool packed = (v & 0x30000) != 0, extended = (variant == 0 || descriptor->override_mode != 0);
    u32 result = (v & 0x40000) != 0;
    if (packed && (v & 0x400000) == 0)
        result |= 2;
    if (extended && ((v >> 9) & 3) >= 1)
        result |= 4;
    if (extended && ((v >> 9) & 3) >= 2)
        result |= 8;
    if (extended && ((v >> 9) & 3) >= 3)
        result |= 0x10;
    if (packed && (v & 0x400000) != 0)
        result |= 0x40;
    if (v & 0x80000)
        result |= 0x80;
    if (v & 0x1000000)
        result |= 0x100;
    if (v & 0x2000000)
        result |= 0x400;
    if (variant == 0 && (descriptor->layer_flags & 0x80000000))
        result |= 0x800;
    if (extended && (v & 0x100))
        result |= 0x1000;
    return result;
}

struct PC_SHADER_MANAGER_VIEW;
struct PC_SHADER_OBJECT_FULL_VIEW {
    i32 marker;
    i32 ref_count;
    u32 key;
    u8 unknown_c[0x4c0];
    PC_TEXTURE_RESOURCE_PC *vertex_shader;
    PC_TEXTURE_RESOURCE_PC *pixel_shader;
    u32 unknown_4d4;
};
STATIC_ASSERT(sizeof(PC_SHADER_OBJECT_FULL_VIEW) == 0x4d8);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, vertex_shader) == 0x4cc);
STATIC_ASSERT(offsetof(PC_SHADER_OBJECT_FULL_VIEW, pixel_shader) == 0x4d0);
struct PC_SHADER_MANAGER_VTABLE_VIEW {
    void *unknown_0;
    bool(__thiscall *create_shader)(PC_SHADER_MANAGER_VIEW *, const PC_SHADER_MATERIAL_FILTER_VIEW *, bool,
                                    PC_SHADER_OBJECT_FULL_VIEW *, i32);
    void(__thiscall *adapt_descriptor)(PC_SHADER_MANAGER_VIEW *, const PC_SHADER_FILTER_DESC_VIEW *,
                                       const PC_SHADER_FILTER_MATERIAL_VIEW *);
};
STATIC_ASSERT(offsetof(PC_SHADER_MANAGER_VTABLE_VIEW, create_shader) == 4);
STATIC_ASSERT(offsetof(PC_SHADER_MANAGER_VTABLE_VIEW, adapt_descriptor) == 8);
struct PC_SHADER_MANAGER_VIEW {
    PC_SHADER_MANAGER_VTABLE_VIEW *vtable;
    PC_SHADER_OBJECT_FULL_VIEW slots[400];
    i32 last_allocated;
    PC_SHADER_OBJECT_FULL_VIEW *bound_slot;
    PC_SHADER_OBJECT_FULL_VIEW *__thiscall AllocateSlot(i32 *index);
    PC_SHADER_OBJECT_FULL_VIEW *__thiscall Retrieve(const PC_SHADER_FILTER_DESC_VIEW *,
                                                    const PC_SHADER_FILTER_MATERIAL_VIEW *, i32, i32, bool);
};
STATIC_ASSERT(offsetof(PC_SHADER_MANAGER_VIEW, slots) == 4);
STATIC_ASSERT(offsetof(PC_SHADER_MANAGER_VIEW, last_allocated) == 0x79184);
STATIC_ASSERT(offsetof(PC_SHADER_MANAGER_VIEW, bound_slot) == 0x79188);

PC_SHADER_OBJECT_FULL_VIEW *__thiscall PC_SHADER_MANAGER_VIEW::AllocateSlot(i32 *index) {
    i32 i;
    for (i = last_allocated + 1; i < 400; ++i) {
        if (slots[i].ref_count <= 0)
            break;
    }
    if (i >= 400) {
        for (i = 0; i < last_allocated; ++i) {
            if (slots[i].ref_count <= 0)
                break;
        }
        if (i >= last_allocated)
            return NULL;
    }
    last_allocated = i;
    PCShaderNativeUnInit(&slots[i]);
    *index = i;
    return &slots[i];
}
PC_SHADER_OBJECT_FULL_VIEW *__thiscall PC_SHADER_MANAGER_VIEW::Retrieve(const PC_SHADER_FILTER_DESC_VIEW *desc,
                                                                        const PC_SHADER_FILTER_MATERIAL_VIEW *material,
                                                                        i32 variant, i32 flags, bool pixel_stage) {
    PC_SHADER_MATERIAL_FILTER_VIEW filter;
    u32 key = 0;
    PC_SHADER_OBJECT_FULL_VIEW *shader;
    i32 index;
    vtable->adapt_descriptor(this, desc, material);
    filter.Initialize(desc, material, variant, flags);
    NuShaderObjectKeyGenerate3(&key, &filter, pixel_stage != 0);
    for (i32 i = 0; i < 400; ++i) {
        if (slots[i].key == key) {
            shader = &slots[i];
            if (shader != NULL) {
                ++shader->ref_count;
                return shader;
            }
            break;
        }
    }
    shader = AllocateSlot(&index);
    if (vtable->create_shader(this, &filter, pixel_stage, shader, index)) {
        ++shader->ref_count;
        return shader;
    }
    return NULL;
}
extern "C" PC_SHADER_OBJECT_FULL_VIEW *__cdecl NuShaderManagerRetrieveShader(
    const PC_SHADER_FILTER_DESC_VIEW *desc, const PC_SHADER_FILTER_MATERIAL_VIEW *material) {
    return reinterpret_cast<PC_SHADER_MANAGER_VIEW *>(PCShaderSlots)->Retrieve(desc, material, 0, 0, false);
}
extern "C" PC_SHADER_OBJECT_FULL_VIEW *__cdecl NuShaderManagerRetrieveShaderVariant(
    const PC_SHADER_FILTER_DESC_VIEW *desc, const PC_SHADER_FILTER_MATERIAL_VIEW *material, i32 variant) {
    return reinterpret_cast<PC_SHADER_MANAGER_VIEW *>(PCShaderSlots)->Retrieve(desc, material, variant, 0, false);
}

extern "C" void *__cdecl PCShaderCurrentObjectPc(void) {
    return reinterpret_cast<PC_SHADER_MANAGER_VIEW *>(PCShaderSlots)->bound_slot;
}
