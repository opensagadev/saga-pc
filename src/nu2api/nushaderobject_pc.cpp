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

void PC_SHADER_PARAMETER_LOCAL_PC::SetTransposedMatrixValues(i32 first_element, i32 count, const f32 *values) {
    i32 vector_count = count * 4;
    i32 remaining = (i8)register_count - first_element * 4;
    if (vector_count >= remaining)
        vector_count = remaining;
    if ((metadata & 0xf0000000u) == 0x10000000u)
        PCDeviceSetVertexShaderConstantsF(register_index + first_element * 4, values, vector_count);
    else
        PCDeviceSetPixelShaderConstantsF(register_index + first_element * 4, values, vector_count);
}
extern "C" void __cdecl NuShaderObjectSetElementsfv_transpose(PC_SHADER_OBJECT_FULL_VIEW *shader, i32 semantic,
                                                              i32 first_element, i32 count, const f32 *values) {
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
            parameter->SetTransposedMatrixValues(first_element, count, values);
            break;
    }
}

/* Verified accessed material cohort; no original allocation extent claim. */
typedef struct PC_SHADER_MATERIAL_DISPATCH_VIEW {
    u8 unknown_0000[0x40];
    u32 sampler_address_modes;
    u8 unknown_0044[0x1c];
    f32 phase_speed;
    f32 phase_scale;
    f32 phase_factor;
    f32 amplitude;
    u8 unknown_0070[0x44];
    u32 texture_flags;
    i32 diffuse_texture_ids[4];
    u32 channel_colours[4];
    f32 parameter_d8;
    f32 parameter_dc;
    f32 parameter_e0;
    f32 parameter_e4;
    i32 height_texture_id;
    i32 normal_texture_id;
    f32 texture_lerp;
    u32 reflection_colour;
    u8 unknown_00f8[0x3];
    u8 flip_sign;
    i32 specular_texture_id;
    i32 bump_texture_id;
    i32 cube_texture_id;
    i32 reflection_texture_id;
    i32 depth_texture_id;
    i32 lighting_texture_id;
    f32 parameter_114;
    f32 parameter_118;
    u32 diffuse_colour;
    u32 specular_colour;
    u32 tint_colour;
    u32 shadow_colour;
    f32 parameter_12c;
    f32 parameter_130;
    f32 parameter_134;
    f32 parameter_138;
    f32 parameter_13c;
    f32 parameter_140;
    f32 parameter_144;
    f32 parameter_148;
    f32 parameter_14c;
    f32 parameter_150;
    f32 parameter_154;
    f32 parameter_158;
    u8 unknown_015c[0x30];
    f32 texture_bias[6];
    u8 unknown_01a4[0x4];
    f32 parameter_1a8;
    f32 parameter_1ac;
    f32 parameter_1b0;
    f32 parameter_1b4;
    f32 parameter_1b8;
    f32 parameter_1bc;
    u8 unknown_01c0[0x10];
    f32 uv_upload_lanes[10];
    u8 unknown_01f8[0x50];
    f32 parameter_248;
    i32 clamp_texture_id;
    u32 palette_colours[4];
    f32 parameter_260;
    f32 parameter_264;
    u8 unknown_0268[0x4];
    u32 phase_flags;
    u8 unknown_0270[0x4];
    f32 parameter_274;
    f32 parameter_278;
    f32 parameter_27c;
    f32 parameter_280;
    f32 parameter_284;
    f32 parameter_288;
    f32 parameter_28c;
    f32 parameter_290;
    f32 parameter_294;
    i32 point_texture_id;
    i32 unfiltered_texture_id;
} PC_SHADER_MATERIAL_DISPATCH_VIEW;
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, sampler_address_modes) == 0x40);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, phase_speed) == 0x60);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, phase_scale) == 0x64);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, phase_factor) == 0x68);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, amplitude) == 0x6c);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, texture_flags) == 0xb4);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, diffuse_texture_ids) == 0xb8);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, channel_colours) == 0xc8);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_d8) == 0xd8);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_dc) == 0xdc);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_e0) == 0xe0);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_e4) == 0xe4);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, height_texture_id) == 0xe8);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, normal_texture_id) == 0xec);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, texture_lerp) == 0xf0);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, reflection_colour) == 0xf4);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, flip_sign) == 0xfb);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, specular_texture_id) == 0xfc);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, bump_texture_id) == 0x100);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, cube_texture_id) == 0x104);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, reflection_texture_id) == 0x108);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, depth_texture_id) == 0x10c);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, lighting_texture_id) == 0x110);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_114) == 0x114);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_118) == 0x118);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, diffuse_colour) == 0x11c);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, specular_colour) == 0x120);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, tint_colour) == 0x124);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, shadow_colour) == 0x128);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_12c) == 0x12c);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_130) == 0x130);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_134) == 0x134);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_138) == 0x138);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_13c) == 0x13c);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_140) == 0x140);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_144) == 0x144);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_148) == 0x148);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_14c) == 0x14c);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_150) == 0x150);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_154) == 0x154);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_158) == 0x158);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, texture_bias) == 0x18c);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_1a8) == 0x1a8);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_1ac) == 0x1ac);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_1b0) == 0x1b0);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_1b4) == 0x1b4);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_1b8) == 0x1b8);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_1bc) == 0x1bc);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, uv_upload_lanes) == 0x1d0);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_248) == 0x248);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, clamp_texture_id) == 0x24c);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, palette_colours) == 0x250);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_260) == 0x260);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_264) == 0x264);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, phase_flags) == 0x26c);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_274) == 0x274);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_278) == 0x278);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_27c) == 0x27c);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_280) == 0x280);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_284) == 0x284);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_288) == 0x288);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_28c) == 0x28c);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_290) == 0x290);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, parameter_294) == 0x294);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, point_texture_id) == 0x298);
STATIC_ASSERT(offsetof(PC_SHADER_MATERIAL_DISPATCH_VIEW, unfiltered_texture_id) == 0x29c);
STATIC_ASSERT(sizeof(PC_SHADER_MATERIAL_DISPATCH_VIEW) == 0x2a0);

void PC_SHADER_PARAMETER_LOCAL_PC::SetScalar(f32 value) {
    if ((metadata & 0xf0000000U) == 0x10000000U)
        PCDeviceSetVertexShaderConstantsF(register_index, &value, 1);
    else
        PCDeviceSetPixelShaderConstantsF(register_index, &value, 1);
}
void PC_SHADER_PARAMETER_LOCAL_PC::SetVector(const f32 *values) {
    if ((metadata & 0xf0000000U) == 0x10000000U)
        PCDeviceSetVertexShaderConstantsF(register_index, values, 1);
    else
        PCDeviceSetPixelShaderConstantsF(register_index, values, 1);
}
void PC_SHADER_PARAMETER_LOCAL_PC::SetVectorElement(i32 index, const f32 *values) {
    if ((metadata & 0xf0000000U) == 0x10000000U)
        PCDeviceSetVertexShaderConstantsF(register_index + index, values, 1);
    else
        PCDeviceSetPixelShaderConstantsF(register_index + index, values, 1);
}
void PC_SHADER_PARAMETER_LOCAL_PC::SetFloat2(const f32 *values) {
    if ((metadata & 0xf0000000U) == 0x10000000U)
        PCDeviceSetVertexShaderConstantsF(register_index, values, 1);
    else
        PCDeviceSetPixelShaderConstantsF(register_index, values, 1);
}
void PC_SHADER_PARAMETER_LOCAL_PC::SetFloat3(const f32 *values) {
    if ((metadata & 0xf0000000U) == 0x10000000U)
        PCDeviceSetVertexShaderConstantsF(register_index, values, 1);
    else
        PCDeviceSetPixelShaderConstantsF(register_index, values, 1);
}
void PC_SHADER_PARAMETER_LOCAL_PC::SetValues(i32 first_element, i32 count, const f32 *values) {
    switch (type_stage & 15) {
        case 1:
            SetScalarValues(first_element, count, values);
            break;
        case 2:
            SetVectorValues(first_element, count, values);
            break;
        case 3:
            SetMatrixValues(first_element, count, values);
            break;
    }
}

extern "C" const f32 PCSamplerBiasMinimum;
extern "C" const f32 PCSamplerBiasMaximum;
extern "C" const f64 PCSamplerBiasMaximumComparison;
extern "C" u32 __cdecl PCShaderSamplerBiasBitsPc(f32 bias) {
    union {
        f32 value;
        u32 bits;
    } result;
    if (bias < PCSamplerBiasMinimum)
        bias = PCSamplerBiasMinimum;
    else if (bias > PCSamplerBiasMaximumComparison)
        bias = PCSamplerBiasMaximum;
    result.value = bias;
    return result.bits;
}

/* Minimum accessed cohort, not a whole NUMTL allocation/size claim. */
typedef struct PC_SHADER_WATER_MATERIAL_VIEW {
    u8 unknown_0000[0x60];
    f32 phase_speed;
    u8 unknown_0064[8];
    f32 amplitude;
} PC_SHADER_WATER_MATERIAL_VIEW;
STATIC_ASSERT(offsetof(PC_SHADER_WATER_MATERIAL_VIEW, phase_speed) == 0x60);
STATIC_ASSERT(offsetof(PC_SHADER_WATER_MATERIAL_VIEW, amplitude) == 0x6c);
extern "C" {
    extern f32 PCShaderWaterThetaStep;
    extern f32 PCShaderWaterAmplitudeScale;
    extern f32 PCShaderWaterTheta;
    extern i32 PCShaderWaterLastFrame;
    extern NUVEC4 PCShaderWaterTable[32];
    extern PC_SHADER_WATER_MATERIAL_VIEW *PCShaderWaterPreviousMaterial;
    extern u32 PCShaderWaterInitializationBits;
}

extern "C" void __cdecl NuShaderObjectBaseUpdateWaterTable(PC_SHADER_OBJECT_FULL_VIEW *shader,
                                                           PC_SHADER_WATER_MATERIAL_VIEW *material) {
    if (shader->parameters[23].register_index < 0)
        return;
    NUVEC scale = {0.5f, 0.5f, 0.5f};
    if (!(PCShaderWaterInitializationBits & 1))
        PCShaderWaterInitializationBits |= 1;
    if (nuapi.frame_count != PCShaderWaterLastFrame)
        PCShaderWaterTheta = material->phase_speed * PCShaderWaterThetaStep + PCShaderWaterTheta;
    if (material != PCShaderWaterPreviousMaterial || nuapi.frame_count != PCShaderWaterLastFrame) {
        NUMTX inverse;
        NuMtxInvR(&inverse, &global_camera.mtx);
        NuMtxScale(&inverse, &scale);
        inverse.m03 = inverse.m13 = inverse.m23 = 0.0f;
        const f32 amplitude = 0.1f * material->amplitude;
        u32 seed = 17;
        NuRandFloatSeeded(&seed);
        for (i32 i = 0; i < 32; ++i) {
            NUVEC displacement;
            f64 phase = (NuRandFloatSeeded(&seed) * (f64)0.4f + (f64)0.8f) * PCShaderWaterTheta;
            f32 anglef = NuRandFloatSeeded(&seed) * 6.283f + phase;
            i32 angle = (i32)(anglef * 10430.3779296875);
            displacement.x = amplitude * NuTrigTable[(angle >> 1) & 0x7fff] * PCShaderWaterAmplitudeScale;
            phase = (NuRandFloatSeeded(&seed) * (f64)0.8f + (f64)0.6f) * PCShaderWaterTheta;
            anglef = NuRandFloatSeeded(&seed) * 5.717f + phase;
            angle = (i32)(anglef * -10430.3779296875);
            displacement.y = amplitude * NuTrigTable[((0x4000 - angle) >> 1) & 0x7fff] * PCShaderWaterAmplitudeScale;
            phase = (NuRandFloatSeeded(&seed) * (f64)0.4f + (f64)0.7f) * PCShaderWaterTheta;
            anglef = NuRandFloatSeeded(&seed) * 6.283f + phase;
            angle = (i32)(anglef * 10430.3779296875);
            displacement.z = amplitude * NuTrigTable[(angle >> 1) & 0x7fff];
            PCShaderWaterTable[i].w = displacement.x / PCShaderWaterAmplitudeScale;
            NuVecMtxTransformH((NUVEC *)&PCShaderWaterTable[i], &displacement, &inverse);
        }
    }
    NuShaderObjectSetElementsfv(shader, 23, 0, 32, (const f32 *)PCShaderWaterTable);
    PCShaderWaterPreviousMaterial = material;
    PCShaderWaterLastFrame = nuapi.frame_count;
}

/* Original caller/helper cohort required; these are logical signatures only.
 * Physical register ABI remains a compiler proof hold until the real dispatcher
 * is compiled in its original TU. No public stack adapters are authorized.
 */
static void *PCShaderMaterialTextureNativePc(PC_SHADER_MATERIAL_DISPATCH_VIEW *material, u8 slot) {
    /* Requires the proposed named diffuse_texture_ids[4] member @b8, reviewed
     * callers slot0/1/2/3; original zero-ID case returnsNULL, else genuine lookup. */
    i32 id = material->diffuse_texture_ids[slot];
    return id == 0 ? NULL : NuTextureGetDimensions(id)->resource;
}
static f32 *PCShaderUnpackColourPc(f32 *output, u32 colour) {
    output[0] = (colour & 255) / 255.0;
    output[1] = ((colour >> 8) & 255) / 255.0;
    output[2] = ((colour >> 16) & 255) / 255.0;
    output[3] = ((colour >> 24) & 255) / 255.0;
    return output;
}
static void PCShaderSamplerWrapPc(i32 u_mode, u32 stage, i32 v_mode) {
    u32 mode;
    switch (u_mode) {
        case 1:
            mode = 3;
            break;
        case 2:
            mode = 2;
            break;
        case 3:
            mode = 4;
            break;
        default:
            mode = 1;
            break;
    }
    PCDeviceSetSamplerState(stage, 1, mode);
    switch (v_mode) {
        case 1:
            mode = 3;
            break;
        case 2:
            mode = 2;
            break;
        case 3:
            mode = 4;
            break;
        default:
            mode = 1;
            break;
    }
    PCDeviceSetSamplerState(stage, 2, mode);
}

extern "C" void __cdecl PCShaderSetMaterialParametersPc(PC_SHADER_OBJECT_FULL_VIEW *shader, NUMTL *original_material) {
    PC_SHADER_MATERIAL_DISPATCH_VIEW *material =
        reinterpret_cast<PC_SHADER_MATERIAL_DISPATCH_VIEW *>(original_material);
    PC_SHADER_PARAMETER_LOCAL_PC *parameter = shader->active_parameters;
    while (parameter != NULL) {
        const i32 semantic = (i8)parameter->semantic;
        NUVEC4 vector;
        switch (semantic) {
            case 0: {
                const f32 *values = PCShaderUnpackColourPc(&vector.x, material->diffuse_colour);
                parameter->SetVector(values);
                break;
            }
            case 2: {
                const f32 *values = PCShaderUnpackColourPc(&vector.x, material->specular_colour);
                vector.w = material->parameter_1b4;
                parameter->SetVector(values);
                break;
            }
            case 51: {
                const f32 *values = PCShaderUnpackColourPc(&vector.x, material->channel_colours[0]);
                parameter->SetVector(values);
                break;
            }
            case 52: {
                const f32 *values = PCShaderUnpackColourPc(&vector.x, material->channel_colours[1]);
                parameter->SetVector(values);
                break;
            }
            case 53: {
                const f32 *values = PCShaderUnpackColourPc(&vector.x, material->channel_colours[2]);
                parameter->SetVector(values);
                break;
            }
            case 54: {
                const f32 *values = PCShaderUnpackColourPc(&vector.x, material->channel_colours[3]);
                parameter->SetVector(values);
                break;
            }
            case 57: {
                const f32 *values = PCShaderUnpackColourPc(&vector.x, material->shadow_colour);
                parameter->SetVector(values);
                break;
            }
            case 58: {
                const f32 *values = PCShaderUnpackColourPc(&vector.x, material->reflection_colour);
                parameter->SetVector(values);
                break;
            }
            case 59: {
                const f32 *values = PCShaderUnpackColourPc(&vector.x, material->tint_colour);
                vector.w = material->parameter_158;
                parameter->SetVector(values);
                break;
            }
            case 9: {
                parameter->SetScalar(material->flip_sign == 0 ? 1.0f : -1.0f);
                break;
            }
            case 10: {
                vector.x = material->parameter_134;
                vector.y = 1.0f;
                vector.z = (f64)0.035f * material->parameter_138;
                vector.w = material->parameter_14c;
                parameter->SetVector(&vector.x);
                break;
            }
            case 11: {
                vector.x = material->texture_lerp;
                vector.y = material->parameter_284 / material->parameter_274;
                vector.z = 0.0f;
                vector.w = 0.0f;
                parameter->SetVector(&vector.x);
                break;
            }
            case 12: {
                vector.x = NuShaderManagerGetShininessFactor() * material->parameter_130;
                vector.y = material->parameter_12c;
                vector.z = material->parameter_144;
                vector.w = material->parameter_148;
                parameter->SetVector(&vector.x);
                break;
            }
            case 13: {
                vector.x = material->parameter_140;
                vector.y = material->parameter_13c;
                vector.z = material->parameter_248;
                vector.w = 0.0f;
                parameter->SetVector(&vector.x);
                break;
            }
            case 14: {
                parameter->SetScalar(material->parameter_114);
                break;
            }
            case 15: {
                parameter->SetScalar(material->parameter_118);
                break;
            }
            case 81: {
                parameter->SetScalar(material->parameter_154);
                break;
            }
            case 16: {
                vector.x = material->parameter_1b8;
                vector.y = material->parameter_1bc;
                vector.z = 0.0f;
                vector.w = 0.0f;
                parameter->SetVector(&vector.x);
                break;
            }
            case 23: {
                NuShaderObjectBaseUpdateWaterTable(shader, reinterpret_cast<PC_SHADER_WATER_MATERIAL_VIEW *>(material));
                break;
            }
            case 36: {
                PCDeviceSetTexture(parameter->register_index, PCShaderMaterialTextureNativePc(material, 0));
                PCShaderSamplerWrapPc((material->sampler_address_modes >> 8) & 3, parameter->register_index,
                                      (material->sampler_address_modes >> 10) & 3);
                PCDeviceSetSamplerState(parameter->register_index, 5, 2);
                PCDeviceSetSamplerState(parameter->register_index, 6, 2);
                PCDeviceSetSamplerState(parameter->register_index, 7, 2);
                const f32 bias = material->texture_bias[0] + PCShaderMaterialSamplerBiasOffsetPc;
                PCDeviceSetSamplerState(parameter->register_index, 8, PCShaderSamplerBiasBitsPc(bias));
                break;
            }
            case 37: {
                PCDeviceSetTexture(parameter->register_index, PCShaderMaterialTextureNativePc(material, 1));
                PCShaderSamplerWrapPc((material->sampler_address_modes >> 8) & 3, parameter->register_index,
                                      (material->sampler_address_modes >> 10) & 3);
                const f32 bias = material->texture_bias[1] + PCShaderMaterialSamplerBiasOffsetPc;
                PCDeviceSetSamplerState(parameter->register_index, 8, PCShaderSamplerBiasBitsPc(bias));
                break;
            }
            case 38: {
                PCDeviceSetTexture(parameter->register_index, PCShaderMaterialTextureNativePc(material, 2));
                PCShaderSamplerWrapPc((material->sampler_address_modes >> 8) & 3, parameter->register_index,
                                      (material->sampler_address_modes >> 10) & 3);
                const f32 bias = material->texture_bias[2] + PCShaderMaterialSamplerBiasOffsetPc;
                PCDeviceSetSamplerState(parameter->register_index, 8, PCShaderSamplerBiasBitsPc(bias));
                break;
            }
            case 39: {
                PCDeviceSetTexture(parameter->register_index, PCShaderMaterialTextureNativePc(material, 3));
                PCShaderSamplerWrapPc((material->sampler_address_modes >> 8) & 3, parameter->register_index,
                                      (material->sampler_address_modes >> 10) & 3);
                const f32 bias = material->texture_bias[3] + PCShaderMaterialSamplerBiasOffsetPc;
                PCDeviceSetSamplerState(parameter->register_index, 8, PCShaderSamplerBiasBitsPc(bias));
                break;
            }
            case 40: {
                const i32 id = material->specular_texture_id;
                PCDeviceSetTexture(parameter->register_index, id == 0 ? NULL : NuTextureGetDimensions(id)->resource);
                PCDeviceSetSamplerState(parameter->register_index, 1, 1);
                PCDeviceSetSamplerState(parameter->register_index, 2, 1);
                const f32 bias = material->texture_bias[4] + PCShaderMaterialSamplerBiasOffsetPc;
                PCDeviceSetSamplerState(parameter->register_index, 8, PCShaderSamplerBiasBitsPc(bias));
                break;
            }
            case 41: {
                const i32 id = material->height_texture_id;
                PCDeviceSetTexture(parameter->register_index, id == 0 ? NULL : NuTextureGetDimensions(id)->resource);
                PCDeviceSetSamplerState(parameter->register_index, 1, 1);
                PCDeviceSetSamplerState(parameter->register_index, 2, 1);
                const f32 bias = material->parameter_1b0 + PCShaderMaterialSamplerBiasOffsetPc;
                PCDeviceSetSamplerState(parameter->register_index, 8, PCShaderSamplerBiasBitsPc(bias));
                break;
            }
            case 42: {
                const i32 id = material->bump_texture_id;
                PCDeviceSetTexture(parameter->register_index, id == 0 ? NULL : NuTextureGetDimensions(id)->resource);
                PCDeviceSetSamplerState(parameter->register_index, 1, 1);
                PCDeviceSetSamplerState(parameter->register_index, 2, 1);
                PCDeviceSetSamplerState(parameter->register_index, 5, 2);
                PCDeviceSetSamplerState(parameter->register_index, 6, 2);
                PCDeviceSetSamplerState(parameter->register_index, 7, 2);
                const f32 bias = material->texture_bias[5] + PCShaderMaterialSamplerBiasOffsetPc;
                PCDeviceSetSamplerState(parameter->register_index, 8, PCShaderSamplerBiasBitsPc(bias));
                break;
            }
            case 43: {
                const i32 id = material->normal_texture_id;
                PCDeviceSetTexture(parameter->register_index, id == 0 ? NULL : NuTextureGetDimensions(id)->resource);
                PCDeviceSetSamplerState(parameter->register_index, 1, 1);
                PCDeviceSetSamplerState(parameter->register_index, 2, 1);
                const f32 bias = material->parameter_1ac + PCShaderMaterialSamplerBiasOffsetPc;
                PCDeviceSetSamplerState(parameter->register_index, 8, PCShaderSamplerBiasBitsPc(bias));
                break;
            }
            case 45: {
                const i32 id = material->depth_texture_id;
                PCDeviceSetTexture(parameter->register_index, id == 0 ? NULL : NuTextureGetDimensions(id)->resource);
                PCDeviceSetSamplerState(parameter->register_index, 1, 1);
                PCDeviceSetSamplerState(parameter->register_index, 2, 1);
                PCDeviceSetSamplerState(parameter->register_index, 5, 1);
                PCDeviceSetSamplerState(parameter->register_index, 6, 1);
                PCDeviceSetSamplerState(parameter->register_index, 7, 0);
                break;
            }
            case 48: {
                const i32 id = material->reflection_texture_id;
                PCDeviceSetTexture(parameter->register_index, id == 0 ? NULL : NuTextureGetDimensions(id)->resource);
                PCDeviceSetSamplerState(parameter->register_index, 1, 3);
                PCDeviceSetSamplerState(parameter->register_index, 2, 3);
                break;
            }
            case 49: {
                const i32 id = material->lighting_texture_id;
                PCDeviceSetTexture(parameter->register_index, id == 0 ? NULL : NuTextureGetDimensions(id)->resource);
                PCDeviceSetSamplerState(parameter->register_index, 1, 1);
                PCDeviceSetSamplerState(parameter->register_index, 2, 1);
                const f32 bias = material->parameter_1a8 + PCShaderMaterialSamplerBiasOffsetPc;
                PCDeviceSetSamplerState(parameter->register_index, 8, PCShaderSamplerBiasBitsPc(bias));
                break;
            }
            case 82: {
                const i32 id = material->clamp_texture_id;
                PCDeviceSetTexture(parameter->register_index, id == 0 ? NULL : NuTextureGetDimensions(id)->resource);
                PCDeviceSetSamplerState(parameter->register_index, 1, 3);
                PCDeviceSetSamplerState(parameter->register_index, 2, 3);
                PCDeviceSetSamplerState(parameter->register_index, 3, 3);
                break;
            }
            case 97: {
                const i32 id = material->point_texture_id;
                PCDeviceSetTexture(parameter->register_index, id == 0 ? NULL : NuTextureGetDimensions(id)->resource);
                PCDeviceSetSamplerState(parameter->register_index, 1, 1);
                PCDeviceSetSamplerState(parameter->register_index, 2, 1);
                PCDeviceSetSamplerState(parameter->register_index, 5, 1);
                PCDeviceSetSamplerState(parameter->register_index, 6, 1);
                PCDeviceSetSamplerState(parameter->register_index, 7, 0);
                break;
            }
            case 98: {
                const i32 id = material->unfiltered_texture_id;
                PCDeviceSetTexture(parameter->register_index, id == 0 ? NULL : NuTextureGetDimensions(id)->resource);
                PCDeviceSetSamplerState(parameter->register_index, 1, 1);
                PCDeviceSetSamplerState(parameter->register_index, 2, 1);
                PCDeviceSetSamplerState(parameter->register_index, 5, 0);
                PCDeviceSetSamplerState(parameter->register_index, 6, 0);
                PCDeviceSetSamplerState(parameter->register_index, 7, 0);
                break;
            }
            case 46: {
                PC_TEXTURE_RESOURCE_PC *texture;
                if (PCDeviceHardwareVertexProcessingPC == 1 && (material->texture_flags & (8u | 0x50000u)))
                    texture = NuTexturePhongCubePc;
                else if (PCDeviceHardwareVertexProcessingPC != 1 && (material->texture_flags & 0x50000u))
                    texture = NuTextureEnvironmentCubePc;
                else
                    texture = material->cube_texture_id > 0
                                  ? NuTextureGetDimensions(material->cube_texture_id)->resource
                                  : NULL;
                PCDeviceSetTexture(parameter->register_index, texture);
                PCDeviceSetSamplerState(parameter->register_index, 1, 3);
                PCDeviceSetSamplerState(parameter->register_index, 2, 3);
                PCDeviceSetSamplerState(parameter->register_index, 3, 3);
                break;
            }
            case 55: {
                vector.x = material->parameter_d8;
                vector.y = material->parameter_dc;
                vector.z = material->parameter_e0;
                vector.w = material->parameter_e4;
                parameter->SetVector(&vector.x);
                break;
            }
            case 60: {
                const f32 value = (material->parameter_150 - 1.0) * (f64)0.1f;
                parameter->SetScalar(value);
                break;
            }
            case 61: {
                GRAPHICS_RENDER_TEXTURE_PC *texture = NuPostEffectGetBackBuffer(nuapi.frame_count);
                PCGraphicsBindEffectTexture(texture, parameter->register_index);
                PCDeviceSetSamplerState(parameter->register_index, 1, 3);
                PCDeviceSetSamplerState(parameter->register_index, 2, 3);
                PCDeviceSetSamplerState(parameter->register_index, 7, 0);
                break;
            }
            case 64: {
                parameter->SetFloat2(&material->uv_upload_lanes[0]);
                break;
            }
            case 65: {
                parameter->SetFloat2(&material->uv_upload_lanes[2]);
                break;
            }
            case 66: {
                parameter->SetFloat2(&material->uv_upload_lanes[4]);
                break;
            }
            case 67: {
                parameter->SetFloat2(&material->uv_upload_lanes[6]);
                break;
            }
            case 72: {
                vector.x = 0.05f;
                vector.y = material->phase_speed * (f64)0.32f;
                vector.z = 0.2f;
                vector.w = 0.8f;
                parameter->SetFloat3(&vector.x);
                break;
            }
            case 80: {
                vector.x =
                    (material->phase_flags & 0x8000u) && material->phase_speed == 0.0f ? 1.0f : material->phase_speed;
                vector.y = material->phase_scale;
                vector.z = material->phase_factor;
                vector.w = (material->phase_scale * (f64)material->phase_factor) * (f64)0.2f;
                parameter->SetVector(&vector.x);
                break;
            }
            case 83: {
                vector.x = material->parameter_260;
                vector.y = material->parameter_264;
                vector.z = 0.0f;
                vector.w = 0.0f;
                parameter->SetVector(&vector.x);
                break;
            }
            case 84: {
                NUVEC4 palette[4];
                for (i32 i = 0; i < 4; ++i) {
                    const f32 *unpacked = PCShaderUnpackColourPc(&vector.x, material->palette_colours[i]);
                    palette[i] = *reinterpret_cast<const NUVEC4 *>(unpacked);
                }
                if ((parameter->metadata & 0xf0000000u) == 0x10000000u)
                    PCDeviceSetVertexShaderConstantsF(parameter->register_index, &palette[0].x, 4);
                else
                    PCDeviceSetPixelShaderConstantsF(parameter->register_index, &palette[0].x, 4);
                break;
            }
            case 86: {
                if (NuWindCurrent(static_cast<NUWIND *>(nuapi.wind)) >= 0) {
                    const i32 id = NuWindCurrent(static_cast<NUWIND *>(nuapi.wind));
                    PCGraphicsBindTextureDimensionsPc(NuTextureGetDimensions(id), parameter->register_index);
                    PCDeviceSetSamplerState(parameter->register_index, 1, 1);
                    PCDeviceSetSamplerState(parameter->register_index, 2, 1);
                    PCDeviceSetSamplerState(parameter->register_index, 3, 1);
                    PCDeviceSetSamplerState(parameter->register_index, 7, 0);
                    PCDeviceSetSamplerState(parameter->register_index, 8, 0);
                }
                break;
            }
            case 87: {
                NUWIND *wind = static_cast<NUWIND *>(nuapi.wind);
                if (wind != NULL) {
                    const f32 parameter24 = wind->parameter_24;
                    const i32 angle0 = (i32)(wind->accumulated_time * 10430.3779296875);
                    const f32 component0 = NuTrigTable[(angle0 >> 1) & 0x7fff] / 10.0;
                    const i32 angle1 = (i32)(wind->scaled_accumulated_time * 10430.3779296875);
                    const f32 component1 = NuTrigTable[(angle1 >> 1) & 0x7fff] / 30.0;
                    vector.x = wind->phase;
                    vector.y = parameter24;
                    vector.z = component0;
                    vector.w = component1;
                    parameter->SetFloat3(&vector.x);
                }
                break;
            }
            case 91: {
                vector.x = 1.0 / material->parameter_290;
                vector.y = material->parameter_288;
                vector.z = material->parameter_28c;
                vector.w = material->parameter_294;
                parameter->SetVector(&vector.x);
                break;
            }
            case 92: {
                vector.x = (f64)0.1f * material->parameter_274;
                vector.y = material->parameter_27c;
                vector.z = material->parameter_280;
                vector.w = material->parameter_278;
                parameter->SetVector(&vector.x);
                break;
            }
        }
        if (shader->version != 4) {
            ShaderUniformRecordPc *uniform = &ShaderUniformRecordsPc[semantic];
            if (uniform->field_18 != 0) {
                switch ((i32)(parameter->metadata << 4) >> 28) {
                    case 1:
                        parameter->SetValues(0, 1, uniform->values);
                        break;
                    case 2:
                        parameter->SetVectorElement(0, uniform->values);
                        break;
                    case 3:
                        parameter->SetMatrixValues(0, 1, uniform->values);
                        break;
                }
            }
        }
        parameter = parameter->next;
    }
}
