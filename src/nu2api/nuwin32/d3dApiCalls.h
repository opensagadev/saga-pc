#ifndef SAGA_NU2API_NUWIN32_D3DAPICALLS_H
#define SAGA_NU2API_NUWIN32_D3DAPICALLS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nucore/common.h (not all of it earlier in the shim). */

#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "nu2api/nucore/common.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void fun_006df670(f32, f32);
    void NuKeyboardCreateWindowState(HWND window, HINSTANCE instance);
    extern i32 PCFrameCriticalSectionIndex;
    i32 PCFrameLoadingYieldEnabled(void);
    extern u8 PCFrameLoadingYieldEnabledPC;
    void PCDeviceResetState(void);

#ifdef __cplusplus
    void PcFreeApiCallBuffer(void);
    void PcRendererCleanup(void);
    void PcRendererShutdown(i32);
    void PcDestroyPlatform(void);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern void *PcApiCallBuffer;
    extern i32 PcApiCallBufferCount;
    i32 PcApiGetCallBufferCount(void);
    void *PcApiGetCallBuffer(void);
    void PCFrameSetLoadingYieldEnabled(i32 enabled);

    extern i32 PCFramePresentationStatePc;
    i32 PCFramePresentNativePc(i32, i32);
    void PCDeviceResetSamplerDefaultsPc(void);
    f32 PcInputAspectRatioPc(void);
#ifdef __cplusplus
    void PCDevicePostEffectBarrierPc(void);
#ifdef __cplusplus
}
#endif
#endif

#endif
