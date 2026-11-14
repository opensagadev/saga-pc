#ifndef SAGA_NU2API_NUGAMMA_H
#define SAGA_NU2API_NUGAMMA_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    /* Actual empty separate original source operation; no invented provider. */
    void DisplayGenerateTransformsNoOpPc(NUDISPLAY_LIST_PC *);
    void NuDisplayListCameraPc(void *);
    void NuDisplayListVertexGroupsPc(void *);
    void NuDisplayListVertexOffsetsPc(void *);
#ifdef __cplusplus
}
#endif

#endif
