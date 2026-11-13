/* Shader manager front end (retail unit 0x7412e0-0x741962): the slot table
 * template, Retrieve and the NuShaderManager C entry points. The material
 * filter view and ShaderManagerHLSL live in other units, so calls into them
 * stay real calls here. */
#include "saga.h"

template <>
bool PC_SHADER_MANAGER_VIEW::CreateShader(const PC_SHADER_MATERIAL_FILTER_VIEW *, bool, PC_SHADER_OBJECT_FULL_VIEW *,
                                          i32);
template <>
void PC_SHADER_MANAGER_VIEW::AdaptDescriptor(const PC_SHADER_FILTER_DESC_VIEW *,
                                             const PC_SHADER_FILTER_MATERIAL_VIEW *);

template <> PC_SHADER_OBJECT_FULL_VIEW *__thiscall PC_SHADER_MANAGER_VIEW::AllocateSlot(i32 *index) {
    i32 i;
    for (i = last_allocated + 1; i < 400; ++i) {
        if (slots[i].ref_count <= 0)
            goto found;
    }
    for (i = 0; i < last_allocated; ++i) {
        if (slots[i].ref_count <= 0)
            goto found;
    }
    return NULL;
found:
    last_allocated = i;
    PCShaderNativeUnInit(&slots[i]);
    *index = i;
    return &slots[i];
}

template <> void PC_SHADER_MANAGER_VIEW::ReleaseShader(PC_SHADER_OBJECT_FULL_VIEW *slot) {
    --slot->ref_count;
}

template <> void PC_SHADER_MANAGER_VIEW::DestroyShaders() {
    PC_SHADER_OBJECT_FULL_VIEW *slot = slots;
    for (i32 i = 0; i < 400; ++i, ++slot) {
        if (slot->marker >= 0)
            PCShaderNativeUnInit(slot);
    }
}

template <> void PC_SHADER_MANAGER_VIEW::SetCurrentShader(PC_SHADER_OBJECT_FULL_VIEW *slot) {
    bound_slot = slot;
}

template <> PC_SHADER_OBJECT_FULL_VIEW *PC_SHADER_MANAGER_VIEW::GetCurrentShader() {
    return bound_slot;
}

/* Lets index 400 through, one past the last slot. */
template <> PC_SHADER_OBJECT_FULL_VIEW *PC_SHADER_MANAGER_VIEW::GetShader(u32 index) {
    if (index <= 400)
        return &slots[index];
    return NULL;
}

template <> void PC_SHADER_MANAGER_VIEW::BindShader(PC_SHADER_OBJECT_FULL_VIEW *slot) {
    if (bound_slot == slot)
        return;
    bound_slot = slot;
    if (slot != NULL) {
        PCDeviceSetVertexShaderPc(slot->vertex_shader);
        PCDeviceSetPixelShaderPc(slot->pixel_shader);
    }
}

template <> void PC_SHADER_MANAGER_VIEW::SetShininessFactor(f32 shininess) {
    PCShaderShininessFactorStorage = shininess;
}

template <> f32 PC_SHADER_MANAGER_VIEW::GetShininessFactor() {
    return PCShaderShininessFactorStorage;
}

template <> PC_SHADER_OBJECT_FULL_VIEW *PC_SHADER_MANAGER_VIEW::FindShader(const u32 *key) {
    for (i32 i = 0; i < 400; ++i) {
        if (*key == slots[i].key)
            return &slots[i];
    }
    return NULL;
}

template <>
PC_SHADER_OBJECT_FULL_VIEW *__thiscall PC_SHADER_MANAGER_VIEW::Retrieve(const PC_SHADER_FILTER_DESC_VIEW *desc,
                                                                        const PC_SHADER_FILTER_MATERIAL_VIEW *material,
                                                                        i32 variant, i32 flags, bool pixel_stage) {
    PC_SHADER_MATERIAL_FILTER_VIEW filter;
    u32 key;
    PC_SHADER_OBJECT_FULL_VIEW *shader;
    i32 index;
    AdaptDescriptor(desc, material);
    filter.Initialize(desc, material, variant, flags);
    key = 0;
    NuShaderObjectKeyGenerate3(&key, &filter, pixel_stage != 0);
    for (i32 i = 0; i < 400; ++i) {
        if (key == slots[i].key) {
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

extern "C" void __cdecl NuShaderManagerSetfv(i32 semantic, const f32 *values) {
    ((ShaderManagerHLSL *)PCShaderSlots)->SetUniformValues(semantic, values);
}

extern "C" void __cdecl NuShaderManagerSetElementfv(i32 semantic, i32 element, const f32 *values) {
    ((ShaderManagerHLSL *)PCShaderSlots)->setElementfv(semantic, element, values);
}

extern "C" void __cdecl NuShaderManagerSetElementsfv_transpose(i32 semantic, i32 first_element, i32 count,
                                                               const f32 *values) {
    ((ShaderManagerHLSL *)PCShaderSlots)->setElementsfv_transpose(semantic, first_element, count, values);
}

extern "C" void __cdecl NuShaderManagerSetElementsfv(i32 semantic, i32 first_element, i32 count, const f32 *values) {
    ((ShaderManagerHLSL *)PCShaderSlots)->setElementsfv(semantic, first_element, count, values);
}

extern "C" void __cdecl NuShaderManagerBindShaderPc(PC_SHADER_OBJECT_FULL_VIEW *shader) {
    PC_SHADER_MANAGER_VIEW *manager = reinterpret_cast<PC_SHADER_MANAGER_VIEW *>(PCShaderSlots);
    if (manager->bound_slot != shader) {
        manager->bound_slot = shader;
        if (shader != NULL) {
            PCDeviceSetVertexShaderPc(shader->vertex_shader);
            PCDeviceSetPixelShaderPc(shader->pixel_shader);
        }
    }
}
extern "C" f32 PCShaderShininessFactorStorage;
extern "C" f32 __cdecl NuShaderManagerGetShininessFactor(void) {
    return PCShaderShininessFactorStorage;
}

extern "C" void *__cdecl NuShaderManagerGetInstance(void) {
    return PCShaderSlots;
}

extern "C" void __cdecl NuShaderManagerDestroy(void) {
    reinterpret_cast<PC_SHADER_MANAGER_VIEW *>(PCShaderSlots)->DestroyShaders();
    PCShaderSlots = NULL;
}

extern "C" void __cdecl NuShaderManagerLoadCompiledShaders(void) {
}

extern "C" void __cdecl NuShaderManagerDestroyShaders(void) {
    reinterpret_cast<PC_SHADER_MANAGER_VIEW *>(PCShaderSlots)->DestroyShaders();
}

extern "C" void __cdecl NuShaderManagerSetCurrentShader(PC_SHADER_OBJECT_FULL_VIEW *slot) {
    reinterpret_cast<PC_SHADER_MANAGER_VIEW *>(PCShaderSlots)->SetCurrentShader(slot);
}

extern "C" void __cdecl NuShaderManagerSetShininessFactor(f32 shininess) {
    PC_SHADER_MANAGER_VIEW::SetShininessFactor(shininess);
}
