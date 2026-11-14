#ifndef SAGA_NU2API_NU3D_ANDROID_NUDLIST_ANDROID_H
#define SAGA_NU2API_NU3D_ANDROID_NUDLIST_ANDROID_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/numtx.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/numtx.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/numtx.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void *DisplayListCreateFaceonTransformPS(VARIPTR *, NUMTX *, NUMTL *, void *);
#ifdef __cplusplus
}
#endif

#endif
