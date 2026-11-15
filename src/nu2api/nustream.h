#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    void NuDisplayListDrawPrimitive2D(void *);
    void NuDisplayListSkinPalette(void *);
    void NuDisplayListLights(void *);
    void NuDisplayListTint(void *);
    void NuDisplayListFog(void *);
    void NuDisplayListLightmapPacket(void *);
    void NuDisplayListLegacyLightmap(void *);
    void NuDisplayListLegacyLightmapOffset(void *);
    void NuDisplayListTransform(void *);
    void NuDisplayListTransformTranspose(void *);
    void NuDisplayListFaceOnTransform(void *);
    void NuDisplayListPacketAd(void *);
    void NuDisplayListUpdateLightmap(void);
    void NuDisplayListDrawFaceOn(void *);
    void NuDisplayListShadowTransform(void *data);
    void NuDisplayListShadowTransformTranspose(void *data);
#ifdef __cplusplus
}
#endif
