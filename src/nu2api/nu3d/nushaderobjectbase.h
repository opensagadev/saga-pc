#ifndef SAGA_NU2API_NU3D_NUSHADEROBJECTBASE_H_PART
#define SAGA_NU2API_NU3D_NUSHADEROBJECTBASE_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: decomp.h, nu2api/nu3d/ShaderManagerOpenGL.h, nu2api/nu3d/android/nushaderobject.h,
 * nu2api/numath/nuvec4.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "decomp.h"
#include "nu2api/nu3d/ShaderManagerOpenGL.h"
#include "nu2api/nu3d/android/nushaderobject.h"
#include "nu2api/numath/nuvec4.h"
#endif
/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_NU2API_NU3D_NUSHADEROBJECTBASE_H_PART == 1 && !defined(SAGA_NU2API_NU3D_NUSHADEROBJECTBASE_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    PC_SHADER_PARAMETER_LOCAL_PC *PCShaderAllocateParameter(PC_SHADER_OBJECT_FULL_VIEW *, i32);
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
    extern struct PC_SHADER_WATER_MATERIAL_VIEW *PCShaderWaterPreviousMaterial;
    extern u32 PCShaderWaterInitializationBits;
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NUSHADEROBJECTBASE_H_PART
#define SAGA_NU2API_NU3D_NUSHADEROBJECTBASE_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NUSHADEROBJECTBASE_H_STOP
#endif
#endif

#if SAGA_NU2API_NU3D_NUSHADEROBJECTBASE_H_PART == 2 && !defined(SAGA_NU2API_NU3D_NUSHADEROBJECTBASE_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    void PCShaderObjectCreateHLSLPc(PC_SHADER_OBJECT_FULL_VIEW *);
    void PCShaderObjectDestroyHLSLPc(PC_SHADER_OBJECT_FULL_VIEW *);
#ifdef __cplusplus
}
#endif
#undef SAGA_NU2API_NU3D_NUSHADEROBJECTBASE_H_PART
#define SAGA_NU2API_NU3D_NUSHADEROBJECTBASE_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_NU2API_NU3D_NUSHADEROBJECTBASE_H_STOP
#endif
#endif

#undef SAGA_NU2API_NU3D_NUSHADEROBJECTBASE_H_STOP
