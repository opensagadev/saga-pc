#include "saga.h"

template <>
bool PC_SHADER_MANAGER_VIEW::CreateShader(const PC_SHADER_MATERIAL_FILTER_VIEW *, bool, PC_SHADER_OBJECT_FULL_VIEW *,
                                          i32);
template <>
void PC_SHADER_MANAGER_VIEW::AdaptDescriptor(const PC_SHADER_FILTER_DESC_VIEW *,
                                             const PC_SHADER_FILTER_MATERIAL_VIEW *);

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

template <> PC_SHADER_OBJECT_FULL_VIEW *__thiscall PC_SHADER_MANAGER_VIEW::AllocateSlot(i32 *index) {
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
template <>
PC_SHADER_OBJECT_FULL_VIEW *__thiscall PC_SHADER_MANAGER_VIEW::Retrieve(const PC_SHADER_FILTER_DESC_VIEW *desc,
                                                                        const PC_SHADER_FILTER_MATERIAL_VIEW *material,
                                                                        i32 variant, i32 flags, bool pixel_stage) {
    PC_SHADER_MATERIAL_FILTER_VIEW filter;
    u32 key = 0;
    PC_SHADER_OBJECT_FULL_VIEW *shader;
    i32 index;
    AdaptDescriptor(desc, material);
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
    if (CreateShader(&filter, pixel_stage, shader, index)) {
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

bool PC_SHADER_MATERIAL_FILTER_VIEW::HasVertexSkin() const {
    return (descriptor->vertex_format & 0x30000) != 0 && (descriptor->vertex_format & 0x400000) == 0;
}
bool PC_SHADER_MATERIAL_FILTER_VIEW::HasVertexSkinAlt() const {
    return (descriptor->vertex_format & 0x30000) != 0 && (descriptor->vertex_format & 0x400000) != 0;
}
bool PC_SHADER_MATERIAL_FILTER_VIEW::HasAnisoFlip() const {
    return variant == 0 && (descriptor->layer_flags & 0x2000000) != 0;
}
bool PC_SHADER_MATERIAL_FILTER_VIEW::HasPhongLighting() const {
    return variant == 0 && (descriptor->layer_flags & 8) != 0;
}

void ShaderManagerHLSLPc::AdaptDescriptor(const PC_SHADER_FILTER_DESC_VIEW *input,
                                          const PC_SHADER_FILTER_MATERIAL_VIEW *input_material) {
    PC_SHADER_FILTER_DESC_VIEW *desc = const_cast<PC_SHADER_FILTER_DESC_VIEW *>(input);
    PC_SHADER_FILTER_MATERIAL_VIEW *material = const_cast<PC_SHADER_FILTER_MATERIAL_VIEW *>(input_material);
    if ((desc->vertex_format & 0x4000000) != 0) {
        desc->layer_flags |= 0x80000000;
        desc->flags |= 0x200000;
    }
    i32 version = PcWindowOptions.usehires != 0 ? compiler->dictionary.profile : 1;
    switch (version) {
        case 3:
            desc->flags &= 0xff77dffb;
            desc->layer_flags &= 0xe7fadffb;
            break;
        case 2:
            desc->flags &= 0xff7efffb;
            desc->layer_flags &= 0xbffbfffb;
            if ((desc->flags & 0x20) != 0) {
                desc->flags &= ~8U;
                desc->layer_flags &= ~8U;
            }
            if ((desc->flags & 0x20) != 0) {
                desc->flags &= ~0x20000U;
                desc->layer_flags &= ~0x8000U;
            }
            if ((desc->flags & 0x20) != 0) {
                desc->flags &= ~3U;
                desc->layer_flags &= ~3U;
            }
            if ((desc->flags & 0x20000) != 0 || (desc->layer_flags & 0x8000) != 0) {
                desc->flags &= ~8U;
                desc->layer_flags &= ~8U;
            }
            if ((desc->flags & 3) != 0) {
                desc->flags &= 0xbfffffef;
                desc->layer_flags &= 0xfdffffef;
            }
            if ((desc->flags & 2) != 0) {
                desc->flags = (desc->flags & ~2U) | 1;
                desc->layer_flags = (desc->layer_flags & ~2U) | 1;
            }
            if ((desc->flags & 0x60) == 0x60) {
                desc->flags &= ~1U;
                desc->layer_flags &= ~1U;
            }
            if ((desc->flags & 0x80) != 0) {
                desc->flags &= ~1U;
                desc->layer_flags &= ~1U;
            }
            if ((desc->flags & 0x25) == 0x25) {
                desc->flags &= ~1U;
                desc->layer_flags &= ~1U;
            }
            if ((desc->flags & 1) != 0 && desc->normal_threshold != 0) {
                desc->flags &= ~1U;
                desc->layer_flags &= ~1U;
            }
            desc->flags &= 0xffff9bff;
            desc->layer_flags &= 0xffff9bff;
            if ((desc->layer_flags & 0x4000000) != 0 && (desc->layer_flags & 8) != 0 &&
                (desc->layer_flags & 0x80000000) != 0 && (desc->layer_flags & 1) != 0) {
                desc->flags &= ~8U;
                desc->layer_flags &= ~8U;
            }
            desc->flags &= 0xfdffffef;
            desc->layer_flags &= 0xffefffef;
            if ((desc->flags & 0x49) == 0x49) {
                desc->flags &= ~1U;
                desc->layer_flags &= ~1U;
            }
            if ((desc->flags & 0x100) != 0 && (desc->flags & 8) != 0) {
                desc->flags &= ~8U;
                desc->layer_flags &= ~8U;
            }
            if ((desc->flags & 0x80201000) == 0x80201000) {
                desc->flags &= ~9U;
                desc->layer_flags &= ~9U;
            }
            desc->flags &= 0xfff7ffff;
            desc->layer_flags &= 0xe7feffff;
            break;
        case 1: {
            bool layer = (desc->layer_flags & 0x400) != 0;
            if ((desc->flags & 0x40200) == 0)
                desc->flags &= ~0x400U;
            desc->layer_flags &= ~0x400U;
            if (layer && (desc->layer_flags & 0x400) == 0 && (material->blend_flags & 0x700000) != 0x100000)
                material->blend_flags &= 0x807fffff;
            if ((desc->flags & 0x4000) != 0)
                desc->shininess = (f32)(desc->shininess * (f64)0.4f);
            desc->flags &= 0xffff9ffc;
            desc->layer_flags &= 0xffff9ffc;
            if ((desc->flags & 0x10000) != 0) {
                desc->flags &= ~0x20U;
                desc->layer_flags &= ~0x20U;
            } else if ((desc->flags & 0x20) != 0) {
                desc->flags &= ~8U;
                desc->layer_flags &= ~8U;
            }
            if ((desc->flags & 3) != 0) {
                desc->flags &= ~8U;
                desc->layer_flags &= ~8U;
            }
            desc->flags &= 0xf916fffb;
            desc->layer_flags &= 0x27c8fffb;
            if ((desc->layer_flags & 0x4000000) != 0) {
                desc->flags &= ~0x28U;
                desc->layer_flags &= ~0x28U;
            }
            break;
        }
    }
    if (compiler->dictionary.force_mode) {
        desc->flags &= 0xfffffbd7;
        desc->layer_flags &= 0xfffffbd7;
    }
}
