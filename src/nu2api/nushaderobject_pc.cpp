#include "gamelib/nuwind/nuwind.h"
#include "nu2api/NuEffectTex_pc.h"
#include "nu2api/PCInput.h"
#include "nu2api/nu3d/ShaderManager.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nupostfilter.h"
#include "nu2api/nu3d/nushader.h"
#include "nu2api/nu3d/nushader_internal.h"
#include "nu2api/nu3d/nushadermanager_gen.h"
#include "nu2api/nu3d/nushaderobject.h"
#include "nu2api/nu3d/shaderSemantics.h"
#include "nu2api/nualloc.h"
#include "nu2api/nucamera_dlist.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nutex_pc.h"
#include "nu2api/nuwin32/d3dApiCalls.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nu3d/nushaderobjectbase.h"

void PC_SHADER_PARAMETER_LOCAL::SetClassValues(i32 first_element, i32 count, const f32 *values) {
    switch ((i32)(metadata << 4) >> 28) {
        case 1:
            SetScalarValues(first_element, count, values);
            break;
        case 2:
            SetVectorValues(first_element, count, values);
            break;
        case 3:
            SetTransposedMatrixValues(first_element, count, values);
            break;
        case 4:
            return;
    }
}

void PCShaderObjectInitHLSLPc(NuShaderObject *shader, const u32 *key, i32 index, const char *vertex_source, i32 vertex_length,
                              const char *pixel_source, i32 pixel_length, i32 version) {
    NuShaderObjectInit(shader, key, index, vertex_source, vertex_length, pixel_source, pixel_length, version);
    ID3DXBuffer *vertex_code = NULL;
    ID3DXBuffer *pixel_code = NULL;
    ID3DXBuffer *errors = NULL;
    ID3DXConstantTable *vertex_constants = NULL;
    ID3DXConstantTable *pixel_constants = NULL;
    const char *vertex_profile;
    const char *pixel_profile;
    DWORD flags;
    HRESULT result;
    switch (version) {
        case PC_SHADER_MODEL_1_1:
            vertex_profile = "vs_1_1";
            pixel_profile = "ps_1_1";
            break;
        case PC_SHADER_MODEL_2_0:
            vertex_profile = "vs_2_0";
            pixel_profile = "ps_2_0";
            break;
        case PC_SHADER_MODEL_2_X:
            vertex_profile = "vs_2_a";
            pixel_profile = "ps_2_a";
            break;
        default:
            vertex_profile = "vs_3_0";
            pixel_profile = "ps_3_0";
            break;
    }
    flags = 0x200;
    if (version < 2) {
        flags = 0x10200;
    }
    result = D3DXCompileShader(vertex_source, vertex_length, NULL, NULL, "vertexProgram", vertex_profile, flags, &vertex_code, &errors,
                               &vertex_constants);
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
        if (vertex_constants != NULL) {
            vertex_constants->methods->release(vertex_constants);
        }
        return;
    }
    if (pixel_source != NULL) {
        result = D3DXCompileShader(pixel_source, pixel_length, NULL, NULL, "fragmentProgram", pixel_profile, flags, &pixel_code, &errors,
                                   &pixel_constants);
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
            if (pixel_constants != NULL) {
                pixel_constants->methods->release(pixel_constants);
            }
            /* Native failure tail skips releasing vertex_constants. */
            return;
        }
    }
    PCDeviceCreateVertexShader((const DWORD *)vertex_code->methods->get_buffer_pointer(vertex_code), &shader->vertex_shader);
    if (pixel_source != NULL) {
        if (bgProcIsBgThread() && PCFrameLoadingYieldEnabled()) {
            Sleep(10);
        }
        PCDeviceCreatePixelShader((const DWORD *)pixel_code->methods->get_buffer_pointer(pixel_code), &shader->pixel_shader);
    }
    PCShaderProbeHLSLConstants(shader, vertex_constants, pixel_constants);
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
    if (vertex_constants != NULL) {
        vertex_constants->methods->release(vertex_constants);
        vertex_constants = NULL;
    }
    if (pixel_constants != NULL) {
        pixel_constants->methods->release(pixel_constants);
        pixel_constants = NULL;
    }
}

void PCShaderNativeUnInit(NuShaderObject *shader) {
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

extern "C" void NuShaderObjectSetElementsfv(NuShaderObject *shader, i32 semantic, i32 first_element, i32 count, const f32 *values) {
    PC_SHADER_PARAMETER_LOCAL *parameter = &shader->parameters[semantic];
    if (parameter->register_index < 0) {
        return;
    }
    switch ((i32)(parameter->metadata << 4) >> 28) {
        case 1:
            parameter->SetScalarValues(first_element, count, values);
            break;
        case 2:
            parameter->SetVectorValues(first_element, count, values);
            break;
        case 3:
            parameter->SetMatrixValues(first_element, count, values);
            break;
        case 4:
            return;
    }
}

extern "C" void NuShaderObjectSetElementsfv_transpose(NuShaderObject *shader, i32 semantic, i32 first_element, i32 count,
                                                      const f32 *values) {
    PC_SHADER_PARAMETER_LOCAL *parameter = &shader->parameters[semantic];
    if (parameter->register_index < 0) {
        return;
    }
    switch ((i32)(parameter->metadata << 4) >> 28) {
        case 1:
            parameter->SetScalarValues(first_element, count, values);
            break;
        case 2:
            parameter->SetVectorValues(first_element, count, values);
            break;
        case 3:
            parameter->SetTransposedMatrixValues(first_element, count, values);
            break;
        case 4:
            return;
    }
}

extern "C" void NushaderobjectNuError(void) {
}

/* Minimum accessed cohort, not a whole NUMTL allocation/size claim. */

extern "C" void fun_00743970(void) {
}
