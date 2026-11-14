#ifndef SAGA_NU2API_NUWIN32_D3DCALLS_H
#define SAGA_NU2API_NUWIN32_D3DCALLS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nustencilshadows_PC.h, nu2api/nuwin32/d3dCore.h (not all of it earlier in
 * the shim). */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/numtx.h"

#ifndef SAGA_SHIM
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nuwin32/d3dCore.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void PCDeviceSetGammaRampPc(const void *ramp);

    void PCDeviceLockVertexBuffer(u32, u32, void **, u32, PC_BUFFER_RESOURCE_PC *);
    HRESULT PCDeviceUnlockVertexBuffer(PC_BUFFER_RESOURCE_PC *);
    void PCDeviceCreateTextureFromMemory(const void *, u32, PC_TEXTURE_RESOURCE_PC **);
    void PCDeviceCreateTextureFromMemoryEx(const void *, u32, u32, u32, u32, u32, i32, i32, u32, u32, u32,
                                           D3DXIMAGE_INFO *, PALETTEENTRY *, PC_TEXTURE_RESOURCE_PC **);
    void PCDeviceCreateCubeTextureFromMemory(const void *, u32, PC_TEXTURE_RESOURCE_PC **);
    void NuVpSubmitPc(const PC_VIEWPORT_PC *viewport);
    HRESULT PCDeviceSetTransform(i32 state, const NUMTX *matrix);
    extern i32 PCClipUseSSE;
    void PCDeviceSetRenderState(i32, u32);
    void PCDeviceSetSamplerState(u32, i32, u32);
    void PCDeviceSetTexture(u32, void *);
    void PCDeviceSetTextureStageState(u32, i32, u32);
    void PCDeviceReleaseTexture(PC_TEXTURE_RESOURCE_PC **texture);
    void PCDeviceSetFVFPc(u32);
    void PCDeviceCreateCubeTextureFromFile(char *path, PC_TEXTURE_RESOURCE_PC **resource);
#ifdef __cplusplus
}
#endif
typedef struct CPU_VENDOR_WORDS_PC {
    u32 word0;
    u32 word1;
    u32 word2;
    u8 terminator[4];
} CPU_VENDOR_WORDS_PC;

STATIC_ASSERT(sizeof(CPU_VENDOR_WORDS_PC) == 16);
STATIC_ASSERT(offsetof(CPU_VENDOR_WORDS_PC, word1) == 4);
STATIC_ASSERT(offsetof(CPU_VENDOR_WORDS_PC, word2) == 8);
#ifdef __cplusplus
extern "C" {
#endif
    extern const CPU_VENDOR_WORDS_PC CpuVendorAmdPc;
    extern const CPU_VENDOR_WORDS_PC CpuVendorCyrixPc;
    extern CPU_VENDOR_WORDS_PC CpuVendorNamePc;
    extern const CPU_VENDOR_WORDS_PC CpuVendorNoCpuidPc;
    extern const CPU_VENDOR_WORDS_PC CpuVendorIntelPc;
    extern u32 CpuVendorPc;
    extern u32 CpuHas3dnowPc;
    extern u32 CpuHasSse2Pc;
    extern u32 CpuExtendedLeafMaxPc;
    extern u32 CpuExtendedFeaturesPc;
    extern u32 CpuHasCpuidPc;
    extern u32 CpuBasicLeafMaxPc;
    extern u32 CpuProcessorTypePc;
    extern u32 CpuFamilyPc;
    extern u32 CpuModelPc;
    extern u32 CpuSteppingPc;
    extern u8 CpuBrandIndexPc;
    extern u32 CpuHasFpuPc;
    extern u32 CpuHasVmePc;
    extern u32 CpuHasDePc;
    extern u32 CpuHasPsePc;
    extern u32 CpuHasTscPc;
    extern u32 CpuHasMsrPc;
    extern u32 CpuHasPaePc;
    extern u32 CpuHasMcePc;
    extern u32 CpuHasCmpxchg8bPc;
    extern u32 CpuHasApicPc;
    extern u32 CpuHasPgePc;
    extern u32 CpuHasMcaPc;
    extern u32 CpuHasCmovPc;
    extern u32 CpuHasPatPc;
    extern u32 CpuHasMmxPc;
    extern u32 CpuInitializedPc;
    /* Retail CPU fields with accessors that CpuDetectFeaturesPc never writes. */
    extern u32 CpuUnsetValue0Pc;
    extern u32 CpuUnsetValue1Pc;
    extern u32 CpuUnsetFeature0Pc;
    extern u32 CpuUnsetFeature1Pc;
    extern u32 CpuUnsetFeature2Pc;
    extern u32 CpuUnsetFeature3Pc;
    void CpuDetectFeaturesPc(void);
    void PCDeviceSetVertexShaderConstantsF(UINT start, const f32 *values, UINT count);
    void PCDeviceSetPixelShaderConstantsF(UINT start, const f32 *values, UINT count);
    void PCDeviceSetStreamSourcePc(u32, PC_BUFFER_RESOURCE_PC *, u32, u32);
    void PCDeviceSetIndicesPc(PC_BUFFER_RESOURCE_PC *);
    void PCDeviceSetStreamSourceFreqPc(u32, u32);
    void PCDeviceSetVertexDeclarationPc(PC_TEXTURE_RESOURCE_PC *);
    void PCDeviceSetVertexShaderPc(PC_TEXTURE_RESOURCE_PC *);
    void PCDeviceSetPixelShaderPc(PC_TEXTURE_RESOURCE_PC *);
#ifdef __cplusplus
    void PCDeviceStretchRectPc(PC_TEXTURE_RESOURCE_PC *, const RECT *, PC_TEXTURE_RESOURCE_PC *, const RECT *, i32);
#ifdef __cplusplus
}
#endif
#endif

#endif
