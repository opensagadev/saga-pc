#ifndef SAGA_NU2API_NU3D_ANDROID_NUOBJECT_ANDROID_H
#define SAGA_NU2API_NU3D_ANDROID_NUOBJECT_ANDROID_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nuobject.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nuobject.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nuobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    NUVEC NuFadeObjGetAngleTerrainValues(NUVEC *);
    void NuFadeObjInit(void);
    PC_NUFADEGROUP *NuFadeObjCreateMtx(NUSPECIALHANDLE *, NUMTX *, i16, f32, f32, i32);
    void NuFadeObjUpdateArray(NUVEC **positions);
    void NuFadeObjUpdate(NUVEC *position);
    void NuFadeObjDraw(void);
    void NuFadeObjInit(void);
    void NuFadeObjSetup(VARIPTR *, VARIPTR, i32, i32);
#ifdef __cplusplus
}
#endif

#endif
