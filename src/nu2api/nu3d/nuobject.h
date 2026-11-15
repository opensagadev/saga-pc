#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nuobject.h"

#ifdef __cplusplus
extern "C" {
#endif
    NUVEC NuFadeObjGetAngleTerrainValues(NUVEC *);
    void NuFadeObjInit(void);
    PC_NUFADEGROUP *NuFadeObjCreateMtx(nuhspecial_s *, NUMTX *, i16, f32, f32, i32);
    void NuFadeObjUpdateArray(NUVEC **positions);
    void NuFadeObjUpdate(NUVEC *position);
    void NuFadeObjDraw(void);
    void NuFadeObjInit(void);
    void NuFadeObjSetup(VARIPTR *, VARIPTR, i32, i32);
#ifdef __cplusplus
}
#endif
