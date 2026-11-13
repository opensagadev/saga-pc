#include "saga.h"
#include <string.h>

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
    if (variant != 0)
        return UnfilteredHasLayer(which) && which == 0 && (descriptor->override_mode & 1) != 0;
    return UnfilteredHasLayer(which);
}
#define SHADER_VERTEX_FORMAT_HAS_UV_SETS(format, count) ((i8)(((format) >> 9) & 3) >= (count))

u32 __thiscall PC_SHADER_MATERIAL_FILTER_VIEW::GetVertexFlags() const {
    u32 v = descriptor->vertex_format;
    u32 result = 0;
    u32 colour0;
    if ((u8)(v >> 18) & 1)
        result = 1;
    colour0 = (v >> 16) & 1;
    if ((colour0 || (v & 0x20000)) && !(v & 0x400000))
        result |= 2;
    if ((variant == 0 || descriptor->override_mode != 0) && SHADER_VERTEX_FORMAT_HAS_UV_SETS(v, 1))
        result |= 4;
    if ((variant == 0 || descriptor->override_mode != 0) && SHADER_VERTEX_FORMAT_HAS_UV_SETS(v, 2))
        result |= 8;
    if ((variant == 0 || descriptor->override_mode != 0) && SHADER_VERTEX_FORMAT_HAS_UV_SETS(v, 3))
        result |= 0x10;
    if ((colour0 || (v & 0x20000)) && (v & 0x400000))
        result |= 0x40;
    if ((u8)(v >> 19) & 1)
        result |= 0x80;
    if ((u8)(v >> 24) & 1)
        result |= 0x100;
    if ((u8)(v >> 25) & 1)
        result |= 0x400;
    if (variant == 0 && (u8)(descriptor->layer_flags >> 31))
        result |= 0x800;
    if ((variant == 0 || descriptor->override_mode != 0) && ((u8)(v >> 8) & 1))
        result |= 0x1000;
    return result;
}
const PC_SHADER_FILTER_DESC_VIEW *PC_SHADER_MATERIAL_FILTER_VIEW::GetDescriptor() const {
    return descriptor;
}
const PC_SHADER_FILTER_MATERIAL_VIEW *PC_SHADER_MATERIAL_FILTER_VIEW::GetMaterial() const {
    return material;
}
bool PC_SHADER_MATERIAL_FILTER_VIEW::HasBaseLayer() const {
    return true;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasSpecular() const {
    if (variant != 0)
        return 0;
    return descriptor->specular_threshold > parameter;
}
bool PC_SHADER_MATERIAL_FILTER_VIEW::HasBaseLighting() const {
    return true;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasFirstPath() const {
    return first_path != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasDeferredPath() const {
    return deferred_path != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag20() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 0x100000) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag1() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 2) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasNormalLayer1() const {
    return variant == 0 && (descriptor->layer_flags & 2) != 0 && descriptor->normal_threshold > parameter;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag0() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 1) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasNormalLayer0() const {
    return variant == 0 && (descriptor->layer_flags & 1) != 0 && descriptor->normal_threshold > parameter;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag14() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 0x4000) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag13() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 0x2000) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasFlag31With23() const {
    return variant == 0 && (descriptor->flags & 0x80000000) != 0 && (descriptor->flags & 0x800000) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasFlag31Without23() const {
    return variant == 0 && (descriptor->flags & 0x80000000) != 0 && (descriptor->flags & 0x800000) == 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasSecondPath() const {
    return second_path != 0;
}
bool PC_SHADER_MATERIAL_FILTER_VIEW::HasExtraTexture() const {
    return descriptor->extra_texture > 0;
}
bool PC_SHADER_MATERIAL_FILTER_VIEW::HasEnvironmentTexture() const {
    return descriptor->environment_texture > 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasSurfaceLayerAbove() const {
    return variant == 0 && (descriptor->layer_flags & 0x100000) != 0 && descriptor->surface_threshold > parameter;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasSurfaceLayerAtOrBelow() const {
    return variant == 0 && (descriptor->layer_flags & 0x100000) != 0 && descriptor->surface_threshold <= parameter;
}

#undef SHADER_VERTEX_FORMAT_HAS_UV_SETS

bool PC_SHADER_MATERIAL_FILTER_VIEW::HasVertexSkin() const {
    return (descriptor->vertex_format & 0x30000) != 0 && (descriptor->vertex_format & 0x400000) == 0;
}
bool PC_SHADER_MATERIAL_FILTER_VIEW::HasVertexSkinAlt() const {
    return (descriptor->vertex_format & 0x30000) != 0 && (descriptor->vertex_format & 0x400000) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasAnisoFlip() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 0x2000000) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasPhongLighting() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 8) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag2() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 0x4) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag5() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 0x20) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlags5And15() const {
    return variant == 0 && (descriptor->layer_flags & 0x20) != 0 && (descriptor->layer_flags & 0x8000) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag15() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 0x8000) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag10() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 0x400) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag21() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 0x200000) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag4() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 0x10) != 0;
}
bool PC_SHADER_MATERIAL_FILTER_VIEW::HasNoExtraLayer() const {
    return false;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag17() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 0x20000) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag19() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 0x80000) != 0;
}
bool PC_SHADER_MATERIAL_FILTER_VIEW::HasNoExtraLighting() const {
    return false;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag18() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 0x40000) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag30() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 0x40000000) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag9() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 0x200) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag29() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 0x20000000) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasUnfilteredLayerFlag22() const {
    return (descriptor->layer_flags & 0x400000) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::IsVariantOrLayerFlag12() const {
    return variant != 0 || (descriptor->layer_flags & 0x1000) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag28() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 0x10000000) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasLayerFlag27() const {
    if (variant != 0)
        return 0;
    return (descriptor->layer_flags & 0x8000000) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasSelector0() const {
    return (descriptor->selectors & 1) != 0;
}
i32 PC_SHADER_MATERIAL_FILTER_VIEW::HasSelector1() const {
    return (descriptor->selectors & 2) != 0;
}

void ShaderManagerHLSLPc::AdaptDescriptor(const PC_SHADER_FILTER_DESC_VIEW *input,
                                          const PC_SHADER_FILTER_MATERIAL_VIEW *input_material) {
    PC_SHADER_FILTER_DESC_VIEW *desc = const_cast<PC_SHADER_FILTER_DESC_VIEW *>(input);
    PC_SHADER_FILTER_MATERIAL_VIEW *material = const_cast<PC_SHADER_FILTER_MATERIAL_VIEW *>(input_material);
    if ((desc->vertex_format & 0x4000000) != 0) {
        desc->layer_flags |= 0x80000000;
        desc->flags |= 0x200000;
    }
    i32 version = compiler->dictionary.profile;
    if (PcWindowOptions.usehires == 0)
        version = 1;
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
            if ((desc->flags & 0x10000) == 0) {
                if ((desc->flags & 0x20) != 0) {
                    desc->flags &= ~8U;
                    desc->layer_flags &= ~8U;
                }
            } else {
                desc->flags &= ~0x20U;
                desc->layer_flags &= ~0x20U;
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

void ShaderManagerHLSL::SetUniformValues(i32 semantic, const f32 *values) {
    ShaderUniformRecordPc *uniform = &ShaderUniformRecordsPc[semantic];
    if (compiler->dictionary.profile != 4) {
        if (uniform->field_18 != 0 || PCShaderCurrentObjectPc() == NULL) {
            if (uniform->value_count <= 4)
                memcpy(uniform->values, values, uniform->value_count * 16);
            else
                PCShaderCurrentObjectPc();
        }
        if ((uniform->field_18 == 0 || uniform->field_18 == 2) && PCShaderCurrentObjectPc() != NULL) {
            PC_SHADER_OBJECT_FULL_VIEW *bound = (PC_SHADER_OBJECT_FULL_VIEW *)PCShaderCurrentObjectPc();
            if (bound->parameters[semantic].register_index >= 0)
                NuShaderObjectSetElementsfv((PC_SHADER_OBJECT_FULL_VIEW *)PCShaderCurrentObjectPc(), semantic, 0, 1,
                                            values);
        }
        return;
    }
    NUMTX transposed;
    const f32 *submitted;
    if (uniform->field_c == 4) {
        NuMtxTranspose(&transposed, (NUMTX *)values);
        submitted = (const f32 *)&transposed;
    } else {
        submitted = values;
    }
    switch (uniform->stage_mask & 3) {
        case 3:
            PCDeviceSetVertexShaderConstantsF(uniform->register_index, submitted, uniform->value_count);
            PCDeviceSetPixelShaderConstantsF(uniform->register_index, submitted, uniform->value_count);
            break;
        case 2:
            PCDeviceSetPixelShaderConstantsF(uniform->register_index, submitted, uniform->value_count);
            break;
        case 1:
            PCDeviceSetVertexShaderConstantsF(uniform->register_index, submitted, uniform->value_count);
            break;
    }
}

#include <new>
extern char uberShader2[158576];

template <> PC_SHADER_MANAGER_VIEW::ShaderManagerTemplate() {
    for (i32 i = 0; i < 400; ++i)
        PCShaderObjectCreateHLSLPc(&slots[i]);
    bound_slot = NULL;
    last_allocated = -1;
    ++slots[0].ref_count;
}

template <> PC_SHADER_MANAGER_VIEW::~ShaderManagerTemplate() {
    for (i32 i = 399; i >= 0; --i)
        PCShaderObjectDestroyHLSLPc(&slots[i]);
}

template <>
bool PC_SHADER_MANAGER_VIEW::CreateShader(const PC_SHADER_MATERIAL_FILTER_VIEW *, bool, PC_SHADER_OBJECT_FULL_VIEW *,
                                          i32) {
    return false;
}

template <>
void PC_SHADER_MANAGER_VIEW::AdaptDescriptor(const PC_SHADER_FILTER_DESC_VIEW *,
                                             const PC_SHADER_FILTER_MATERIAL_VIEW *) {
}

ShaderManagerHLSL::ShaderManagerHLSL(VirtualStackAllocator &allocator)
    : vertex_source(0x10000, true), pixel_source(0x10000, true) {
    void *memory = allocator.cursor;
    allocator.cursor += sizeof(HLSLShaderBuilder);
    compiler = new (memory) HLSLShaderBuilder(allocator);
    compiler->SetSaveDirectory("\\genShaders");
    compiler->SetIncludeDirectories("./shaders;../nu2api/shaderBuilder;C:\\projects\\nu2api\\shaderBuilder");
    compiler->LoadSource(uberShader2, sizeof(uberShader2) - 1);
    compiler->SetProfile(PCDeviceHardwareVertexProcessingPC, 8, 2);
    compiler->dictionary.AddRegisterBlock("skinMatrix", 8);
    compiler->dictionary.AddRegisterBlock("blendShapes", 8);
    compiler->SetForceMode(PcIntelShaderFallback != 0);
}

ShaderManagerHLSL::~ShaderManagerHLSL() {
    for (i32 i = 0; i < 400; ++i) {
        if (slots[i].marker >= 0)
            PCShaderNativeUnInit(&slots[i]);
    }
}

// for saga.h: NuShaderObject::unknown_4d4 is a bitfield (retail stores each path bit with xor/and/xor).
struct NuShaderObjectPathsPc {
    u32 base : 1;
    u32 first : 1;
    u32 layer : 1;
    u32 second : 1;
    u32 unknown : 28;
};
bool ShaderManagerHLSL::CreateShader(const PC_SHADER_MATERIAL_FILTER_VIEW *filter, bool pixel_stage,
                                     PC_SHADER_OBJECT_FULL_VIEW *shader, i32 index) {
    u32 key = 0;
    NuShaderObjectKeyGenerate3(&key, filter, pixel_stage != 0);
    vertex_source.length = 0;
    pixel_source.length = 0;
    compiler->Generate(const_cast<PC_SHADER_MATERIAL_FILTER_VIEW *>(filter), &vertex_source, &pixel_source);
    PCShaderObjectInitHLSLPc(shader, &key, index, reinterpret_cast<const char *>(vertex_source.data),
                             vertex_source.length, reinterpret_cast<const char *>(pixel_source.data),
                             pixel_source.length, compiler->dictionary.profile);
    NuShaderObjectPathsPc *paths = reinterpret_cast<NuShaderObjectPathsPc *>(&shader->unknown_4d4);
    paths->base = filter->variant == 0;
    paths->first = filter->first_path != 0;
    paths->layer = filter->variant != 0 ? false : (bool)((filter->descriptor->layer_flags >> 16) & 1);
    paths->second = filter->second_path != 0;
    return true;
}

extern "C" void __cdecl NuShaderManagerInit(VARIPTR *arena, VARIPTR arena_end) {
    VirtualStackAllocator allocator;
    allocator.setExternalMemoryPool(arena->void_ptr, arena_end.addr - arena->addr);
    void *memory = allocator.cursor;
    allocator.cursor += sizeof(ShaderManagerHLSL);
    PCShaderSlots = reinterpret_cast<PC_SHADER_SLOT_VIEW *>(new (memory) ShaderManagerHLSL(allocator));
    arena->addr += allocator.cursor - allocator.base;
}

void ShaderManagerHLSL::setElementfv(i32 semantic, i32 element, const f32 *values) {
    ShaderUniformRecordPc *uniform = &ShaderUniformRecordsPc[semantic];
    if (compiler->dictionary.profile != 4) {
        if (uniform->field_18 != 0 || PCShaderCurrentObjectPc() == NULL) {
            if (uniform->value_count <= 4)
                memcpy(uniform->values, values, uniform->value_count * 16);
            else
                PCShaderCurrentObjectPc();
        }
        if ((uniform->field_18 == 0 || uniform->field_18 == 2) && PCShaderCurrentObjectPc() != NULL) {
            PC_SHADER_OBJECT_FULL_VIEW *bound = (PC_SHADER_OBJECT_FULL_VIEW *)PCShaderCurrentObjectPc();
            if (bound->parameters[semantic].register_index >= 0)
                NuShaderObjectSetElementsfv((PC_SHADER_OBJECT_FULL_VIEW *)PCShaderCurrentObjectPc(), semantic, element,
                                            1, values);
        }
        return;
    }
    NUMTX transposed;
    const f32 *submitted;
    if (uniform->field_c == 4) {
        NuMtxTranspose(&transposed, (NUMTX *)values);
        submitted = (const f32 *)&transposed;
    } else {
        submitted = values;
    }
    switch (uniform->stage_mask & 3) {
        case 3:
            PCDeviceSetVertexShaderConstantsF(uniform->register_index + element, submitted, uniform->field_c);
            PCDeviceSetPixelShaderConstantsF(uniform->register_index + element, submitted, uniform->field_c);
            break;
        case 2:
            PCDeviceSetPixelShaderConstantsF(uniform->register_index + element, submitted, uniform->field_c);
            break;
        case 1:
            PCDeviceSetVertexShaderConstantsF(uniform->register_index + element, submitted, uniform->field_c);
            break;
    }
}
void ShaderManagerHLSL::setElementsfv_transpose(i32 semantic, i32 first_element, i32 count, const f32 *values) {
    ShaderUniformRecordPc *uniform = &ShaderUniformRecordsPc[semantic];
    if (compiler->dictionary.profile != 4) {
        if (uniform->field_18 != 0 || PCShaderCurrentObjectPc() == NULL) {
            if (uniform->value_count <= 4)
                memcpy(uniform->values, values, uniform->value_count * 16);
            else
                PCShaderCurrentObjectPc();
        }
        if ((uniform->field_18 == 0 || uniform->field_18 == 2) && PCShaderCurrentObjectPc() != NULL) {
            PC_SHADER_OBJECT_FULL_VIEW *bound = (PC_SHADER_OBJECT_FULL_VIEW *)PCShaderCurrentObjectPc();
            if (bound->parameters[semantic].register_index >= 0)
                NuShaderObjectSetElementsfv_transpose((PC_SHADER_OBJECT_FULL_VIEW *)PCShaderCurrentObjectPc(), semantic,
                                                      first_element, count, values);
        }
        return;
    }
    switch (uniform->stage_mask & 3) {
        case 3:
            PCDeviceSetVertexShaderConstantsF(uniform->register_index + first_element, values,
                                              uniform->field_c * count);
            PCDeviceSetPixelShaderConstantsF(uniform->register_index + first_element, values, uniform->field_c * count);
            break;
        case 2:
            PCDeviceSetPixelShaderConstantsF(uniform->register_index + first_element, values, uniform->field_c * count);
            break;
        case 1:
            PCDeviceSetVertexShaderConstantsF(uniform->register_index + first_element, values,
                                              uniform->field_c * count);
            break;
    }
}

void ShaderManagerHLSL::setElementsfv(i32 semantic, i32 first_element, i32 count, const f32 *values) {
    ShaderUniformRecordPc *uniform = &ShaderUniformRecordsPc[semantic];
    if (compiler->dictionary.profile != 4) {
        if (uniform->field_18 != 0 || PCShaderCurrentObjectPc() == NULL) {
            if (uniform->value_count <= 4)
                memcpy(uniform->values, values, uniform->value_count * 16);
            else
                PCShaderCurrentObjectPc();
        }
        if ((uniform->field_18 == 0 || uniform->field_18 == 2) && PCShaderCurrentObjectPc() != NULL) {
            PC_SHADER_OBJECT_FULL_VIEW *bound = (PC_SHADER_OBJECT_FULL_VIEW *)PCShaderCurrentObjectPc();
            if (bound->parameters[semantic].register_index >= 0)
                NuShaderObjectSetElementsfv((PC_SHADER_OBJECT_FULL_VIEW *)PCShaderCurrentObjectPc(), semantic,
                                            first_element, count, values);
        }
        return;
    }
    NUMTX transposed[16];
    const f32 *submitted;
    if (uniform->field_c == 4) {
        const NUMTX *source = (const NUMTX *)values;
        for (i32 i = 0; i < count; ++i)
            NuMtxTranspose(&transposed[i], (NUMTX *)source++);
        first_element *= 4;
        submitted = (const f32 *)transposed;
    } else {
        submitted = values;
    }
    switch (uniform->stage_mask & 3) {
        case 3:
            PCDeviceSetVertexShaderConstantsF(uniform->register_index + first_element, submitted,
                                              uniform->field_c * count);
            PCDeviceSetPixelShaderConstantsF(uniform->register_index + first_element, submitted,
                                             uniform->field_c * count);
            break;
        case 2:
            PCDeviceSetPixelShaderConstantsF(uniform->register_index + first_element, submitted,
                                             uniform->field_c * count);
            break;
        case 1:
            PCDeviceSetVertexShaderConstantsF(uniform->register_index + first_element, submitted,
                                              uniform->field_c * count);
            break;
    }
}
