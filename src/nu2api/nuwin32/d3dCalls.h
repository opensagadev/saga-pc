/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/numtx.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nuwin32/d3dCore.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void PCDeviceSetGammaRampPc(const void *ramp);

    void PCDeviceLockVertexBuffer(u32, u32, void **, u32, PC_BUFFER_RESOURCE *);
    HRESULT PCDeviceUnlockVertexBuffer(PC_BUFFER_RESOURCE *);
    void PCDeviceCreateTextureFromMemory(const void *, u32, PC_TEXTURE_RESOURCE **);
    void PCDeviceCreateTextureFromMemoryEx(const void *, u32, u32, u32, u32, u32, i32, i32, u32, u32, u32, D3DXIMAGE_INFO *, PALETTEENTRY *,
                                           PC_TEXTURE_RESOURCE **);
    void PCDeviceCreateCubeTextureFromMemory(const void *, u32, PC_TEXTURE_RESOURCE **);
    void NuVpSubmit(const PC_VIEWPORT *viewport);
    HRESULT PCDeviceSetTransform(i32 state, const NUMTX *matrix);
    extern i32 PCClipUseSSE;
    void PCDeviceSetRenderState(i32, u32);
    void PCDeviceSetSamplerState(u32, i32, u32);
    void PCDeviceSetTexture(u32, void *);
    void PCDeviceSetTextureStageState(u32, i32, u32);
    void PCDeviceReleaseTexture(PC_TEXTURE_RESOURCE **texture);
    void PCDeviceSetFVFPc(u32);
    void PCDeviceCreateCubeTextureFromFile(char *path, PC_TEXTURE_RESOURCE **resource);
#ifdef __cplusplus
}
#endif
typedef struct CPU_VENDOR_WORDS { // size 0x10
    u32 word0;                    // 0x0
    u32 word1;                    // 0x4
    u32 word2;                    // 0x8
    u8 terminator[4];             // 0xc
} CPU_VENDOR_WORDS;

#ifdef __cplusplus
extern "C" {
#endif
    extern const CPU_VENDOR_WORDS CpuVendorAmd;
    extern const CPU_VENDOR_WORDS CpuVendorCyrix;
    extern CPU_VENDOR_WORDS CpuVendorName;
    extern const CPU_VENDOR_WORDS CpuVendorNoCpuid;
    extern const CPU_VENDOR_WORDS CpuVendorIntel;
    extern u32 CpuVendor;
    extern u32 CpuHas3dnow;
    extern u32 CpuHasSse2;
    extern u32 CpuExtendedLeafMax;
    extern u32 CpuExtendedFeatures;
    extern u32 CpuHasCpuid;
    extern u32 CpuBasicLeafMax;
    extern u32 CpuProcessorType;
    extern u32 CpuFamily;
    extern u32 CpuModel;
    extern u32 CpuStepping;
    extern u8 CpuBrandIndex;
    extern u32 CpuHasFpu;
    extern u32 CpuHasVme;
    extern u32 CpuHasDe;
    extern u32 CpuHasPse;
    extern u32 CpuHasTsc;
    extern u32 CpuHasMsr;
    extern u32 CpuHasPae;
    extern u32 CpuHasMce;
    extern u32 CpuHasCmpxchg8b;
    extern u32 CpuHasApic;
    extern u32 CpuHasPge;
    extern u32 CpuHasMca;
    extern u32 CpuHasCmov;
    extern u32 CpuHasPat;
    extern u32 CpuHasMmx;
    extern u32 CpuInitialized;
    /* Retail CPU fields with accessors that CpuDetectFeatures never writes. */
    extern u32 CpuUnsetValue0;
    extern u32 CpuUnsetValue1;
    extern u32 CpuUnsetFeature0;
    extern u32 CpuUnsetFeature1;
    extern u32 CpuUnsetFeature2;
    extern u32 CpuUnsetFeature3;
    void CpuDetectFeatures(void);
    void PCDeviceSetVertexShaderConstantsF(UINT start, const f32 *values, UINT count);
    void PCDeviceSetPixelShaderConstantsF(UINT start, const f32 *values, UINT count);
    void PCDeviceSetStreamSourcePc(u32, PC_BUFFER_RESOURCE *, u32, u32);
    void PCDeviceSetIndicesPc(PC_BUFFER_RESOURCE *);
    void PCDeviceSetStreamSourceFreqPc(u32, u32);
    void PCDeviceSetVertexDeclarationPc(PC_TEXTURE_RESOURCE *);
    void PCDeviceSetVertexShaderPc(PC_TEXTURE_RESOURCE *);
    void PCDeviceSetPixelShaderPc(PC_TEXTURE_RESOURCE *);
#ifdef __cplusplus
    void PCDeviceStretchRectPc(PC_TEXTURE_RESOURCE *, const RECT *, PC_TEXTURE_RESOURCE *, const RECT *, i32);
#ifdef __cplusplus
}
#endif
#endif
