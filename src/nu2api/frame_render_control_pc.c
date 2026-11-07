#include "saga.h"

#pragma data_seg(".data$NuMainRendererControlPc")
i32 NuMainRendererControlPc = 1;
#pragma data_seg()

#pragma bss_seg(".bss$NuRndrControlStatePc")
i32 NuRndrControlStatePc = 0;
#pragma bss_seg()

void __cdecl NuRndrShadowOnOff_candidate(i32 value) {
    NuRndrControlStatePc = value;
}

/* The shadow-volume pass reads the arena globals that nurndr.c defines as pointers through array
 * declarations, so retail draws from the pointer variables' own addresses (the pass has no caller). */
extern NUVEC4 RendererArenaDataPc[];
extern u32 RendererArenaOffsetsPc[];
extern i32 RendererArenaTypesPc[];
extern i32 RendererArenaVertexCountPc;
extern u32 RendererArenaCountPc;
extern PC_TEXTURE_RESOURCE_PC *RendererVertexDeclarationPc;
extern PC_BUFFER_RESOURCE_PC *RendererQuadVertexBufferPc;

#pragma data_seg(".data$RendererStencilFuncPc")
u32 RendererStencilFuncPc = 2;
#pragma data_seg()
#pragma data_seg(".data$RendererStencilPassPc")
u32 RendererStencilPassPc = 1;
#pragma data_seg()
#pragma data_seg(".data$RendererStencilRefPc")
u32 RendererStencilRefPc = 0x60;
#pragma data_seg()

void __cdecl NuRndrShadowVolumeFinishPc(void) {
    PCDeviceSetVertexShaderPc(NULL);
    PCDeviceSetPixelShaderPc(NULL);
    PCDeviceSetRenderState(27, 1);
    PCDeviceSetRenderState(14, 0);
    PCDeviceSetRenderState(137, 0);
    PCDeviceSetRenderState(29, 0);
    PCDeviceSetTexture(0, NULL);
    PCDeviceSetTextureStageState(0, 1, 2);
    PCDeviceSetTextureStageState(0, 2, 0);
    PCDeviceSetTextureStageState(0, 4, 2);
    PCDeviceSetTextureStageState(0, 5, 0);
    PCDeviceSetTextureStageState(1, 1, 1);
    PCDeviceSetTextureStageState(1, 4, 1);
    PCDeviceSetRenderState(56, RendererStencilFuncPc);
    PCDeviceSetRenderState(55, RendererStencilPassPc);
    PCDeviceSetRenderState(57, RendererStencilRefPc);
    PCDeviceSetRenderState(7, 0);
    PCDeviceSetRenderState(22, 1);
    PCDeviceSetRenderState(171, 1);
    PCDeviceSetRenderState(19, 5);
    PCDeviceSetRenderState(20, 6);
    PCDeviceSetVertexDeclarationPc(PCDeviceBackend.declaration6ac);
    PCDeviceSetStreamSourcePc(0, RendererQuadVertexBufferPc, 0, 0x1c);
    PCDeviceDrawPrimitivePc(6, 0, 2);
}

void __cdecl NuRndrShadowVolumeDrawPc(void) {
    i32 i;
    u32 n;
    PCDeviceSetVertexDeclarationPc(RendererVertexDeclarationPc);
    PCDeviceSetRenderState(14, 0);
    PCDeviceSetRenderState(27, 1);
    PCDeviceSetRenderState(19, 1);
    PCDeviceSetRenderState(20, 2);
    PCDeviceSetRenderState(7, 1);
    PCDeviceSetRenderState(54, 1);
    PCDeviceSetRenderState(52, 1);
    PCDeviceSetRenderState(56, 8);
    PCDeviceSetRenderState(55, 4);
    PCDeviceSetRenderState(57, 1);
    PCDeviceSetRenderState(22, 3);
    for (i = 0; i != RendererArenaVertexCountPc; i += 4) {
        PCDeviceDrawPrimitiveUPPc(6, 2, &RendererArenaDataPc[i], 0x10);
    }
    for (n = 0; n < RendererArenaCountPc; n++) {
        PCDeviceDrawPrimitiveUPPc(6, RendererArenaTypesPc[n] - 2, &RendererArenaDataPc[RendererArenaOffsetsPc[n]],
                                  0x40);
    }
    PCDeviceSetRenderState(22, 2);
    PCDeviceSetRenderState(55, 5);
    for (i = 0; i != RendererArenaVertexCountPc; i += 4) {
        PCDeviceDrawPrimitiveUPPc(6, 2, &RendererArenaDataPc[i], 0x10);
    }
    for (n = 0; n < RendererArenaCountPc; n++) {
        PCDeviceDrawPrimitiveUPPc(6, RendererArenaTypesPc[n] - 2, &RendererArenaDataPc[RendererArenaOffsetsPc[n]],
                                  0x40);
    }
}

void __cdecl PCDeviceSetWorldIdentityPc(void);
void __cdecl NuRndrArenaBuildShadowVolumesPc(i32 unused0, i32 unused1);

/* Renders the queued hierarchy shadow packets as stencil shadow volumes; retail never calls it. */
void __cdecl NuRndrShadowVolumesPc(void) {
    if (PcWindowOptions.shadowsenabled != 0 && PcWindowOptions.shadowmethod == 1 && PCShadowPacketCount != 0) {
        PCDeviceSetVertexShaderPc(NULL);
        PCDeviceSetPixelShaderPc(NULL);
        PCDeviceSetRenderState(15, 0);
        PCDeviceSetRenderState(23, 4);
        PCDeviceSetTexture(0, NULL);
        PCDeviceSetWorldIdentityPc();
        NuRndrArenaBuildShadowVolumesPc(0x80, 0);
        NuRndrShadowVolumeDrawPc();
        NuRndrShadowVolumeFinishPc();
        PCDeviceSetRenderState(7, 1);
        PCDeviceSetRenderState(52, 0);
        PCDeviceSetRenderState(14, 1);
        PCDeviceSetRenderState(27, 0);
        PCDeviceSetRenderState(22, 3);
        RendererArenaVertexCountPc = 0;
        RendererArenaCountPc = 0;
    }
    PCShadowPacketCount = 0;
}
