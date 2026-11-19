#include "gameapi/edtools/edui.h"
#include "nu2api/nu3d/ShaderManager.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nushader.h"
#include "nu2api/nu3d/nushaderobject.h"
#include "nu2api/nu3d/nushaderobjectbase.h"
#include "nu2api/nualloc.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/nuwin32/d3dCalls.h"

/* Minimum accessed cohort, not a whole NUMTL allocation/size claim. */

extern "C" {

    void NuShaderObjectBaseCreate(NuShaderObject *shader) {
        shader->marker = -1;
        shader->ref_count = 0;
        shader->base_state = 0;
        shader->key = 0;
    }

    void NuShaderObjectBaseDestroy(NuShaderObject *shader) {
        shader->ref_count = 0;
    }

    void NuShaderObjectBaseInit(NuShaderObject *shader, const u32 *key, i32 index) {
        shader->key = *key;
        shader->marker = index;
    }

    void NuShaderObjectBaseUnInit(NuShaderObject *shader) {
        shader->marker = -1;
    }
}

extern "C" {
    f32 PCShaderWaterThetaStep = 0.26666668f;
}

extern "C" void NuShaderObjectBaseSetWaterSpeed(f32 speed) {
    PCShaderWaterThetaStep = (f32)((f64)speed * TerrainScanMargin10);
}

extern "C" {
    f32 PCShaderWaterAmplitudeScale = 4.0f;
    f32 PCShaderWaterTheta = 0.7f;
    i32 PCShaderWaterLastFrame = -1;
    __declspec(align(16)) NUVEC4 PCShaderWaterTable[32] = {0};
    struct numtl_s *PCShaderWaterPreviousMaterial = NULL;
    u32 PCShaderWaterInitializationBits = 0;
}

extern "C" void NuShaderObjectBaseUpdateWaterTable(NuShaderObject *shader, NUMTL *material) {
    if (shader->parameters[23].register_index < 0) {
        return;
    }
    NUVEC scale = {0.5f, 0.5f, 0.5f};
    if (!(PCShaderWaterInitializationBits & 1)) {
        PCShaderWaterInitializationBits |= 1;
    }
    if (PCShaderWaterLastFrame != nuapi.frame_count) {
        PCShaderWaterTheta = material->transform_scale * PCShaderWaterThetaStep + PCShaderWaterTheta;
    }
    if (PCShaderWaterPreviousMaterial != material || PCShaderWaterLastFrame != nuapi.frame_count) {
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
            displacement.x = NuRandFloatSeeded(&seed) * 6.283f + phase;
            i32 angle = (i32)(displacement.x * 10430.3779296875);
            displacement.x = amplitude * NuTrigTable[(angle >> 1) & 0x7fff] * PCShaderWaterAmplitudeScale;
            phase = (NuRandFloatSeeded(&seed) * (f64)0.8f + (f64)0.6f) * PCShaderWaterTheta;
            displacement.y = NuRandFloatSeeded(&seed) * 5.717f + phase;
            angle = (i32)(displacement.y * -10430.3779296875);
            displacement.y = amplitude * NuTrigTable[((0x4000 - angle) >> 1) & 0x7fff] * PCShaderWaterAmplitudeScale;
            phase = (NuRandFloatSeeded(&seed) * (f64)0.4f + (f64)0.7f) * PCShaderWaterTheta;
            displacement.z = NuRandFloatSeeded(&seed) * 6.283f + phase;
            angle = (i32)(displacement.z * 10430.3779296875);
            displacement.z = amplitude * NuTrigTable[(angle >> 1) & 0x7fff];
            PCShaderWaterTable[i].w = displacement.x / PCShaderWaterAmplitudeScale;
            NuVecMtxTransformH(&PCShaderWaterTable[i].xyz, &displacement, &inverse);
        }
    }
    NuShaderObjectSetElementsfv(shader, SHADER_UNIFORM_WATER_TABLE, 0, 32, (const f32 *)PCShaderWaterTable);
    PCShaderWaterPreviousMaterial = material;
    PCShaderWaterLastFrame = nuapi.frame_count;
}

i32 PC_SHADER_PARAMETER_LOCAL::IsBound() {
    return register_index >= 0;
}

void PC_SHADER_PARAMETER_LOCAL::SetTransposedMatrixValues(i32 first_element, i32 count, const f32 *values) {
    i32 vector_count = (i8)register_count - first_element * 4;
    if (count * 4 < vector_count) {
        vector_count = count * 4;
    }
    if ((metadata & 0xf0000000u) == 0x10000000u) {
        PCDeviceSetVertexShaderConstantsF(register_index + first_element * 4, values, vector_count);
    } else {
        PCDeviceSetPixelShaderConstantsF(register_index + first_element * 4, values, vector_count);
    }
}

void PCShaderObjectCreateHLSLPc(NuShaderObject *shader) {
    i32 i;
    NuShaderObjectCreate(shader);
    shader->vertex_shader = NULL;
    shader->pixel_shader = NULL;
    for (i = 0; i < 100; ++i) {
        shader->parameters[i].semantic = (u8)i;
    }
}

void PCShaderObjectDestroyHLSLPc(NuShaderObject *shader) {
    NuShaderObjectDestroy(shader);
}

PC_SHADER_PARAMETER_LOCAL *PCShaderAllocateParameter(NuShaderObject *shader, i32 index) {
    return &shader->parameters[index];
}

extern "C" i32 fun_00743d50(void) {
    return 0;
}

extern "C" void fun_00743de0(void) {
}
