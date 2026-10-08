#include "saga.h"

void __cdecl PCShaderNativeUnInit(PC_SHADER_OBJECT_FULL_VIEW *shader) {
    if (shader->vertex_shader != NULL) {
        shader->vertex_shader->methods->release(shader->vertex_shader);
        shader->vertex_shader = NULL;
    }
    if (shader->pixel_shader != NULL) {
        shader->pixel_shader->methods->release(shader->pixel_shader);
        shader->pixel_shader = NULL;
    }
    NuShaderObjectUnInit(shader);
}

void __cdecl PCShaderObjectCreateHLSLPc(PC_SHADER_OBJECT_FULL_VIEW *shader) {
    i32 i;
    NuShaderObjectCreate(shader);
    shader->vertex_shader = NULL;
    shader->pixel_shader = NULL;
    for (i = 0; i < 100; ++i)
        shader->parameters[i].semantic = (u8)i;
}
void __cdecl PCShaderObjectDestroyHLSLPc(PC_SHADER_OBJECT_FULL_VIEW *shader) {
    NuShaderObjectDestroy(shader);
}

PC_SHADER_PARAMETER_LOCAL_PC *__cdecl PCShaderAllocateParameter(PC_SHADER_OBJECT_FULL_VIEW *shader, i32 index) {
    return &shader->parameters[index];
}
void __cdecl PCShaderObjectInitHLSLPc(PC_SHADER_OBJECT_FULL_VIEW *shader, const u32 *key, i32 index,
                                      const char *vertex_source, i32 vertex_length, const char *pixel_source,
                                      i32 pixel_length, i32 version) {
    ID3DXBuffer *vertex_code = NULL;
    ID3DXBuffer *pixel_code = NULL;
    ID3DXBuffer *errors = NULL;
    ID3DXConstantTable *vertex_constants = NULL;
    ID3DXConstantTable *pixel_constants = NULL;
    const char *vertex_profile;
    const char *pixel_profile;
    DWORD flags = 0x200;
    HRESULT result;
    NuShaderObjectInit(shader, key, index, vertex_source, vertex_length, pixel_source, pixel_length, version);
    switch (version) {
        case 1:
            vertex_profile = "vs_1_1";
            pixel_profile = "ps_1_1";
            break;
        case 2:
            vertex_profile = "vs_2_0";
            pixel_profile = "ps_2_0";
            break;
        case 3:
            vertex_profile = "vs_2_a";
            pixel_profile = "ps_2_a";
            break;
        default:
            vertex_profile = "vs_3_0";
            pixel_profile = "ps_3_0";
            break;
    }
    if (version < 2)
        flags = 0x10200;
    result = D3DXCompileShader(vertex_source, vertex_length, NULL, NULL, "vertexProgram", vertex_profile, flags,
                               &vertex_code, &errors, &vertex_constants);
    if (result != 0) {
        errors->methods->get_buffer_pointer(errors);
        if (vertex_code != NULL) {
            vertex_code->methods->release(vertex_code);
            vertex_code = NULL;
        }
        if (errors != NULL) {
            errors->methods->release(errors);
            errors = NULL;
        }
        if (vertex_constants != NULL)
            vertex_constants->methods->release(vertex_constants);
        return;
    }
    if (pixel_source != NULL) {
        result = D3DXCompileShader(pixel_source, pixel_length, NULL, NULL, "fragmentProgram", pixel_profile, flags,
                                   &pixel_code, &errors, &pixel_constants);
        if (result != 0) {
            errors->methods->get_buffer_pointer(errors);
            if (vertex_code != NULL) {
                vertex_code->methods->release(vertex_code);
                vertex_code = NULL;
            }
            if (pixel_code != NULL) {
                pixel_code->methods->release(pixel_code);
                pixel_code = NULL;
            }
            if (errors != NULL) {
                errors->methods->release(errors);
                errors = NULL;
            }
            if (pixel_constants != NULL)
                pixel_constants->methods->release(pixel_constants);
            /* Native failure tail skips releasing vertex_constants. */
            return;
        }
    }
    PCDeviceCreateVertexShader((const DWORD *)vertex_code->methods->get_buffer_pointer(vertex_code),
                               &shader->vertex_shader);
    if (pixel_source != NULL) {
        if (bgProcIsBgThread() && PCFrameLoadingYieldEnabled())
            Sleep(10);
        PCDeviceCreatePixelShader((const DWORD *)pixel_code->methods->get_buffer_pointer(pixel_code),
                                  &shader->pixel_shader);
    }
    PCShaderProbeHLSLConstants(shader, vertex_constants, pixel_constants);
    if (vertex_code != NULL)
        vertex_code->methods->release(vertex_code);
    if (pixel_code != NULL)
        pixel_code->methods->release(pixel_code);
    if (errors != NULL)
        errors->methods->release(errors);
    if (vertex_constants != NULL)
        vertex_constants->methods->release(vertex_constants);
    if (pixel_constants != NULL)
        pixel_constants->methods->release(pixel_constants);
}

void PC_SHADER_PARAMETER_LOCAL_PC::SetScalarValues(i32 first_element, i32 count, const f32 *values) {
    if ((metadata & 0xf0000000U) == 0x10000000U)
        PCDeviceSetVertexShaderConstantsF(register_index + first_element, values, count);
    else
        PCDeviceSetPixelShaderConstantsF(register_index + first_element, values, count);
}
void PC_SHADER_PARAMETER_LOCAL_PC::SetVectorValues(i32 first_element, i32 count, const f32 *values) {
    if ((metadata & 0xf0000000U) == 0x10000000U)
        PCDeviceSetVertexShaderConstantsF(register_index + first_element, values, count);
    else
        PCDeviceSetPixelShaderConstantsF(register_index + first_element, values, count);
}
void PC_SHADER_PARAMETER_LOCAL_PC::SetMatrixValues(i32 first_element, i32 count, const f32 *values) {
    NUMTX transposed[32];
    for (i32 i = 0; i < count; ++i)
        NuMtxTranspose(&transposed[i], (NUMTX *)(values + i * 16));
    i32 vector_count = count * 4;
    i32 remaining = (i8)register_count - first_element * 4;
    if (vector_count > remaining)
        vector_count = remaining;
    if ((metadata & 0xf0000000U) == 0x10000000U)
        PCDeviceSetVertexShaderConstantsF(register_index + first_element * 4, (const f32 *)transposed, vector_count);
    else
        PCDeviceSetPixelShaderConstantsF(register_index + first_element * 4, (const f32 *)transposed, vector_count);
}
extern "C" void __cdecl NuShaderObjectSetElementsfv(PC_SHADER_OBJECT_FULL_VIEW *shader, i32 semantic, i32 first_element,
                                                    i32 count, const f32 *values) {
    PC_SHADER_PARAMETER_LOCAL_PC *parameter = &shader->parameters[semantic];
    if (parameter->register_index < 0)
        return;
    switch (parameter->type_stage & 15) {
        case 1:
            parameter->SetScalarValues(first_element, count, values);
            break;
        case 2:
            parameter->SetVectorValues(first_element, count, values);
            break;
        case 3:
            parameter->SetMatrixValues(first_element, count, values);
            break;
    }
}
