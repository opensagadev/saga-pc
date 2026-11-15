/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/nustencilshadows_PC.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void *DisplayListBuildLightPacket(NUGLOBALRNDRSTATE *state);
    void *DisplayListBuildKonstPacket(NUGLOBALRNDRSTATE *state);
    void *DisplayListBuildReflectionPacket(NUGLOBALRNDRSTATE *state);
    void *DisplayListBuildFogPacket(NUGLOBALRNDRSTATE *state);
    void *DisplayListBuildVertexGroupPacket(NUGLOBALRNDRSTATE *);
    void NuPtlDmaSlotsExhausted(void);
    void DisplayListSpecialTransformNoOp(nuhspecial_s *special, NUMTX *matrix);
    void NuVpBuildScalingTemplate(void);
    void PCClipScreenUnsupported(void);
    void NuTexBmpUnsupportedDepth(i32 bit_count, const char *path);
    void NuQFntAtlasError(void);

    void PCGraphicsRenderTextureAllocationError(void);
    void NuRndrStateGetTransform(NUMTX *, const NUMTL *);
    void NuRndrStateSetTransform(const NUMTX *);
    void NuRndrInit(VARIPTR *, VARIPTR);
    u8 PcInitializeGeometryBasis(PC_GEOMETRY_BASIS_SOURCE *, NUMTX *, PC_GEOMETRY_MORPH_WEIGHTS *);
    void NuDisplayListReflection(void *);
    void NuDisplayListDrawGeometry(void *);
    void NuDisplayListMaterial(void *);
    void NuDebrisReleaseVertexBuffers(void);
    void NuDisplayListShadowMaterial(void *data);
    void debris_InitializeD3D(void);
#ifdef __cplusplus
}
#endif
