#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nushaderobject.h"
#include "nu2api/numath/nuvec4.h"

struct numtl_s;

struct PC_SHADER_PARAMETER_LOCAL;

#ifdef __cplusplus
extern "C" {
#endif
    struct PC_SHADER_PARAMETER_LOCAL *PCShaderAllocateParameter(NuShaderObject *, i32);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

extern "C" void PCShaderObjectCreateHLSLPc(NuShaderObject *);
extern "C" void PCShaderObjectDestroyHLSLPc(NuShaderObject *);
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern const f64 PCSamplerBiasMaximumComparison;
    extern f32 PCShaderWaterThetaStep;
    extern f32 PCShaderWaterAmplitudeScale;
    extern f32 PCShaderWaterTheta;
    extern i32 PCShaderWaterLastFrame;
    extern NUVEC4 PCShaderWaterTable[32];
    extern struct numtl_s *PCShaderWaterPreviousMaterial;
    extern u32 PCShaderWaterInitializationBits;
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void PCShaderObjectCreateHLSLPc(NuShaderObject *);
    void PCShaderObjectDestroyHLSLPc(NuShaderObject *);
    void NuShaderObjectBaseUpdateWaterTable(NuShaderObject *shader, struct numtl_s *material);
#ifdef __cplusplus
}
#endif
