#ifndef SAGA_NU2API_NU3D_PC_INCLUDES_H
#define SAGA_NU2API_NU3D_PC_INCLUDES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/nustencilshadows_PC.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void *DisplayListBuildLightPacket(NUGLOBALRNDRSTATE_PC *state);
    void *DisplayListBuildKonstPacket(NUGLOBALRNDRSTATE_PC *state);
    void *DisplayListBuildReflectionPacket(NUGLOBALRNDRSTATE_PC *state);
    void *DisplayListBuildFogPacket(NUGLOBALRNDRSTATE_PC *state);
    void *DisplayListBuildVertexGroupPacket(NUGLOBALRNDRSTATE_PC *);
    void NuPtlDmaSlotsExhaustedPc(void);
    void DisplayListSpecialTransformNoOpPc(NUSPECIALHANDLE *special, NUMTX *matrix);
    void NuVpBuildScalingTemplatePc(void);
    void PCClipScreenUnsupported(void);
    void NuTexBmpUnsupportedDepthPc(i32 bit_count, const char *path);
    void NuQFntAtlasErrorPc(void);

    void PCGraphicsRenderTextureAllocationError(void);
    void NuRndrStateGetTransformPc(NUMTX *, const NUMTL *);
    void NuRndrStateSetTransformPc(const NUMTX *);
    void NuRndrInit(VARIPTR *, VARIPTR);
    u8 PcInitializeGeometryBasisPc(PC_GEOMETRY_BASIS_SOURCE_PC *, NUMTX *, PC_GEOMETRY_MORPH_WEIGHTS_PC *);
    void NuDisplayListReflectionPc(void *);
    void NuDisplayListDrawGeometryPc(void *);
    void NuDisplayListMaterialPc(void *);
#ifdef __cplusplus
}
#endif

#endif
