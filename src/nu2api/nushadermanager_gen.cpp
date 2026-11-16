/* Shader manager front end: the slot table template, Retrieve and the
 * NuShaderManager C entry points. The material filter view and
 * ShaderManagerHLSL live in other units, so calls into them stay real calls
 * here. */
#include "nu2api/PCInput.h"
#include "nu2api/ShaderManagerHLSL.h"
#include "nu2api/nu3d/ShaderManager.h"
#include "nu2api/nu3d/nushader_plain.h"
#include "nu2api/nu3d/nushadermanager_gen.h"
#include "nu2api/nu3d/nushaderobject.h"
#include "nu2api/nuwin32/d3dCalls.h"

template <> bool PC_SHADER_MANAGER_VIEW::CreateShader(const PC_SHADER_MATERIAL_FILTER_VIEW *, bool, NuShaderObject *, i32);
template <> void PC_SHADER_MANAGER_VIEW::AdaptDescriptor(const NUSHADERMTLDESC *, const NUMTL *);

template <> NuShaderObject *PC_SHADER_MANAGER_VIEW::AllocateSlot(i32 *index) {
    i32 i;
    for (i = last_allocated + 1; i < 400; ++i) {
        if (slots[i].ref_count <= 0) {
            goto found;
        }
    }
    for (i = 0; i < last_allocated; ++i) {
        if (slots[i].ref_count <= 0) {
            goto found;
        }
    }
    return NULL;
found:
    last_allocated = i;
    PCShaderNativeUnInit(&slots[i]);
    *index = i;
    return &slots[i];
}

template <> void PC_SHADER_MANAGER_VIEW::ReleaseShader(NuShaderObject *slot) {
    --slot->ref_count;
}

template <> void PC_SHADER_MANAGER_VIEW::DestroyShaders() {
    NuShaderObject *slot = slots;
    for (i32 i = 0; i < 400; ++i, ++slot) {
        if (slot->marker >= 0) {
            PCShaderNativeUnInit(slot);
        }
    }
}

template <> void PC_SHADER_MANAGER_VIEW::SetCurrentShader(NuShaderObject *slot) {
    bound_slot = slot;
}

template <> NuShaderObject *PC_SHADER_MANAGER_VIEW::GetCurrentShader() {
    return bound_slot;
}

/* Lets index 400 through, one past the last slot. */
template <> NuShaderObject *PC_SHADER_MANAGER_VIEW::GetShader(u32 index) {
    if (index <= 400) {
        return &slots[index];
    }
    return NULL;
}

template <> void PC_SHADER_MANAGER_VIEW::BindShader(NuShaderObject *slot) {
    if (bound_slot == slot) {
        return;
    }
    bound_slot = slot;
    if (slot != NULL) {
        PCDeviceSetVertexShader(slot->vertex_shader);
        PCDeviceSetPixelShader(slot->pixel_shader);
    }
}

extern "C" {
    char g_shaderSaveFolder[256] = "shaders";
    f32 PCShaderShininessFactorStorage = 1.0f;
}

template <> void PC_SHADER_MANAGER_VIEW::SetShininessFactor(f32 shininess) {
    PCShaderShininessFactorStorage = shininess;
}

template <> f32 PC_SHADER_MANAGER_VIEW::GetShininessFactor() {
    return PCShaderShininessFactorStorage;
}

template <> NuShaderObject *PC_SHADER_MANAGER_VIEW::FindShader(const u32 *key) {
    for (i32 i = 0; i < 400; ++i) {
        if (*key == slots[i].key) {
            return &slots[i];
        }
    }
    return NULL;
}

template <>
NuShaderObject *PC_SHADER_MANAGER_VIEW::Retrieve(const NUSHADERMTLDESC *desc, const NUMTL *material,
                                                 i32 variant, i32 flags, bool pixel_stage) {
    PC_SHADER_MATERIAL_FILTER_VIEW filter;
    u32 key;
    NuShaderObject *shader;
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

extern "C" {
    PC_SHADER_SLOT_VIEW *PCShaderSlots = 0;
}

extern "C" NuShaderObject *NuShaderManagerRetrieveShader(const NUSHADERMTLDESC *desc,
                                                         const NUMTL *material) {
    return reinterpret_cast<PC_SHADER_MANAGER_VIEW *>(PCShaderSlots)->Retrieve(desc, material, 0, 0, false);
}

extern "C" NuShaderObject *NuShaderManagerRetrieveShaderVariant(const NUSHADERMTLDESC *desc,
                                                                const NUMTL *material, i32 variant) {
    return reinterpret_cast<PC_SHADER_MANAGER_VIEW *>(PCShaderSlots)->Retrieve(desc, material, variant, 0, false);
}

extern "C" i32 fun_00741860(void) {
    return PCDeviceHardwareVertexProcessingPC;
}

extern "C" NuShaderObject *PCShaderCurrentObject(void) {
    return reinterpret_cast<PC_SHADER_MANAGER_VIEW *>(PCShaderSlots)->bound_slot;
}

extern "C" void NuShaderManagerSetfv(i32 semantic, const f32 *values) {
    ((ShaderManagerHLSL *)PCShaderSlots)->SetUniformValues(semantic, values);
}

extern "C" void NuShaderManagerSetElementfv(i32 semantic, i32 element, const f32 *values) {
    ((ShaderManagerHLSL *)PCShaderSlots)->setElementfv(semantic, element, values);
}

extern "C" void NuShaderManagerSetElementsfv_transpose(i32 semantic, i32 first_element, i32 count, const f32 *values) {
    ((ShaderManagerHLSL *)PCShaderSlots)->setElementsfv_transpose(semantic, first_element, count, values);
}

extern "C" void NuShaderManagerSetElementsfv(i32 semantic, i32 first_element, i32 count, const f32 *values) {
    ((ShaderManagerHLSL *)PCShaderSlots)->setElementsfv(semantic, first_element, count, values);
}

extern "C" void NuShaderManagerBindShader(NuShaderObject *shader) {
    PC_SHADER_MANAGER_VIEW *manager = reinterpret_cast<PC_SHADER_MANAGER_VIEW *>(PCShaderSlots);
    if (manager->bound_slot != shader) {
        manager->bound_slot = shader;
        if (shader != NULL) {
            PCDeviceSetVertexShader(shader->vertex_shader);
            PCDeviceSetPixelShader(shader->pixel_shader);
        }
    }
}

extern "C" f32 PCShaderShininessFactorStorage;

extern "C" f32 NuShaderManagerGetShininessFactor(void) {
    return PCShaderShininessFactorStorage;
}

extern "C" void *NuShaderManagerGetInstance(void) {
    return PCShaderSlots;
}

extern "C" void NuShaderManagerSetShaderSaveFolder(const char *folder) {
    strcpy(g_shaderSaveFolder, folder);
}

extern "C" void NuShaderManagerDestroy(void) {
    reinterpret_cast<PC_SHADER_MANAGER_VIEW *>(PCShaderSlots)->DestroyShaders();
    PCShaderSlots = NULL;
}

extern "C" void NuShaderManagerLoadCompiledShaders(void) {
}

extern "C" void NuShaderManagerDestroyShaders(void) {
    reinterpret_cast<PC_SHADER_MANAGER_VIEW *>(PCShaderSlots)->DestroyShaders();
}

extern "C" void NuShaderManagerSetCurrentShader(NuShaderObject *slot) {
    reinterpret_cast<PC_SHADER_MANAGER_VIEW *>(PCShaderSlots)->SetCurrentShader(slot);
}

extern "C" void NuShaderManagerSetShininessFactor(f32 shininess) {
    PC_SHADER_MANAGER_VIEW::SetShininessFactor(shininess);
}
