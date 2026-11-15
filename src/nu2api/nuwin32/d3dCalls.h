#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/numtx.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nuwin32/d3dCore.h"

struct PC_TEXTURE_RESOURCE;

/* PCDeviceSetRenderState states: the Direct3D 9 D3DRENDERSTATETYPE values the game sets (d3d9types.h). */
#define D3DRS_ZENABLE 7
#define D3DRS_ZWRITEENABLE 14
#define D3DRS_ALPHATESTENABLE 15
#define D3DRS_SRCBLEND 19
#define D3DRS_DESTBLEND 20
#define D3DRS_CULLMODE 22
#define D3DRS_ZFUNC 23
#define D3DRS_ALPHAREF 24
#define D3DRS_ALPHAFUNC 25
#define D3DRS_ALPHABLENDENABLE 27
#define D3DRS_FOGENABLE 28
#define D3DRS_SPECULARENABLE 29
#define D3DRS_FOGCOLOR 34
#define D3DRS_FOGTABLEMODE 35
#define D3DRS_FOGSTART 36
#define D3DRS_FOGEND 37
#define D3DRS_FOGDENSITY 38
#define D3DRS_STENCILENABLE 52
#define D3DRS_STENCILFAIL 53
#define D3DRS_STENCILZFAIL 54
#define D3DRS_STENCILPASS 55
#define D3DRS_STENCILFUNC 56
#define D3DRS_STENCILREF 57
#define D3DRS_STENCILMASK 58
#define D3DRS_STENCILWRITEMASK 59
#define D3DRS_LIGHTING 137
#define D3DRS_POINTSIZE 154
#define D3DRS_MULTISAMPLEANTIALIAS 161
#define D3DRS_COLORWRITEENABLE 168
#define D3DRS_BLENDOP 171
#define D3DRS_COLORWRITEENABLE1 190
#define D3DRS_COLORWRITEENABLE2 191
#define D3DRS_COLORWRITEENABLE3 192

/* PCDeviceSetSamplerState states (D3DSAMPLERSTATETYPE), PCDeviceSetTextureStageState states
 * (D3DTEXTURESTAGESTATETYPE) and PCDeviceSetTransform states (D3DTRANSFORMSTATETYPE, D3DTS_WORLD is
 * D3DTS_WORLDMATRIX(0)), d3d9types.h. */
#define D3DSAMP_ADDRESSU 1
#define D3DSAMP_ADDRESSV 2
#define D3DSAMP_ADDRESSW 3
#define D3DSAMP_BORDERCOLOR 4
#define D3DSAMP_MAGFILTER 5
#define D3DSAMP_MINFILTER 6
#define D3DSAMP_MIPFILTER 7
#define D3DSAMP_MIPMAPLODBIAS 8
#define D3DSAMP_MAXMIPLEVEL 9
#define D3DTSS_COLOROP 1
#define D3DTSS_COLORARG1 2
#define D3DTSS_COLORARG2 3
#define D3DTSS_ALPHAOP 4
#define D3DTSS_ALPHAARG1 5
#define D3DTSS_ALPHAARG2 6
#define D3DTS_VIEW 2
#define D3DTS_PROJECTION 3
#define D3DTS_WORLD 256

#ifdef __cplusplus
extern "C" {
#endif
    void PCDeviceSetGammaRamp(const void *ramp);

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
    void PCDeviceSetStreamSource(u32, PC_BUFFER_RESOURCE *, u32, u32);
    void PCDeviceSetIndices(PC_BUFFER_RESOURCE *);
    void PCDeviceSetStreamSourceFreq(u32, u32);
    void PCDeviceSetVertexDeclaration(PC_TEXTURE_RESOURCE *);
    void PCDeviceSetVertexShader(PC_TEXTURE_RESOURCE *);
    void PCDeviceSetPixelShader(PC_TEXTURE_RESOURCE *);
#ifdef __cplusplus
    void PCDeviceStretchRect(PC_TEXTURE_RESOURCE *, const RECT *, PC_TEXTURE_RESOURCE *, const RECT *, i32);
    HRESULT PCDeviceUnlockIndexBuffer(PC_BUFFER_RESOURCE *);
    i32 PCDeviceSetCursorPosition(i32 x, i32 y);
    i32 PCDeviceShowCursor(i32 show);
    i32 PCGraphicsPrepareVendor();
    void PCDeviceLockIndexBuffer(u32, u32, void **, u32, PC_BUFFER_RESOURCE *);
    void PCDeviceSetSoftwareVertexProcessing(BOOL enabled);
#ifdef __cplusplus
}
#endif
#endif
