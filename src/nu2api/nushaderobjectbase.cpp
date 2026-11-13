#include "saga.h"

/* Retail unit 0x743990-0x743d50: definitions in retail address order. */

/* Minimum accessed cohort, not a whole NUMTL allocation/size claim. */
typedef struct PC_SHADER_WATER_MATERIAL_VIEW {
    u8 unknown_0000[0x60];
    f32 phase_speed;
    u8 unknown_0064[8];
    f32 amplitude;
} PC_SHADER_WATER_MATERIAL_VIEW;

extern "C" {

    void __cdecl NuShaderObjectBaseCreate(PC_SHADER_OBJECT_FULL_VIEW *shader) {
        shader->marker = -1;
        shader->ref_count = 0;
        shader->unknown_c = 0;
        shader->key = 0;
    }

    void __cdecl NuShaderObjectBaseDestroy(PC_SHADER_OBJECT_FULL_VIEW *shader) {
        shader->ref_count = 0;
    }

    void __cdecl NuShaderObjectBaseInit(PC_SHADER_OBJECT_FULL_VIEW *shader, const u32 *key, i32 index) {
        shader->key = *key;
        shader->marker = index;
    }

    void __cdecl NuShaderObjectBaseUnInit(PC_SHADER_OBJECT_FULL_VIEW *shader) {
        shader->marker = -1;
    }
}

extern "C" void __cdecl NuShaderObjectBaseSetWaterSpeed(f32 speed) {
    PCShaderWaterThetaStep = (f32)((f64)speed * TerrainScanMargin10Pc);
}

extern "C" void __cdecl NuShaderObjectBaseUpdateWaterTable(PC_SHADER_OBJECT_FULL_VIEW *shader,
                                                           PC_SHADER_WATER_MATERIAL_VIEW *material) {
    if (shader->parameters[23].register_index < 0)
        return;
    NUVEC scale = {0.5f, 0.5f, 0.5f};
    if (!(PCShaderWaterInitializationBits & 1))
        PCShaderWaterInitializationBits |= 1;
    if (PCShaderWaterLastFrame != nuapi.frame_count)
        PCShaderWaterTheta = material->phase_speed * PCShaderWaterThetaStep + PCShaderWaterTheta;
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
            NuVecMtxTransformH((NUVEC *)&PCShaderWaterTable[i], &displacement, &inverse);
        }
    }
    NuShaderObjectSetElementsfv(shader, 23, 0, 32, (const f32 *)PCShaderWaterTable);
    PCShaderWaterPreviousMaterial = material;
    PCShaderWaterLastFrame = nuapi.frame_count;
}

void PC_SHADER_PARAMETER_LOCAL_PC::SetTransposedMatrixValues(i32 first_element, i32 count, const f32 *values) {
    i32 vector_count = (i8)register_count - first_element * 4;
    if (count * 4 < vector_count)
        vector_count = count * 4;
    if ((metadata & 0xf0000000u) == 0x10000000u)
        PCDeviceSetVertexShaderConstantsF(register_index + first_element * 4, values, vector_count);
    else
        PCDeviceSetPixelShaderConstantsF(register_index + first_element * 4, values, vector_count);
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
