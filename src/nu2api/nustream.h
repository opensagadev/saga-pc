#ifndef SAGA_NU2API_NUSTREAM_H
#define SAGA_NU2API_NUSTREAM_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    void NuDisplayListDrawPrimitive2DPc(void *);
    void NuDisplayListSkinPalettePc(void *);
    void NuDisplayListLightsPc(void *);
    void NuDisplayListTintPc(void *);
    void NuDisplayListFogPc(void *);
    void NuDisplayListLightmapPacketPc(void *);
    void NuDisplayListLegacyLightmapPc(void *);
    void NuDisplayListLegacyLightmapOffsetPc(void *);
    void NuDisplayListTransformPc(void *);
    void NuDisplayListTransformTransposePc(void *);
    void NuDisplayListFaceOnTransformPc(void *);
    void NuDisplayListPacketAdPc(void *);
    void NuDisplayListUpdateLightmapPc(void);
    void NuDisplayListDrawFaceOnPc(void *);
#ifdef __cplusplus
}
#endif

#endif
