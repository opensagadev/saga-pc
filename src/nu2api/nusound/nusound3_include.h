#ifndef SAGA_NU2API_NUSOUND_NUSOUND3_INCLUDE_H
#define SAGA_NU2API_NUSOUND_NUSOUND3_INCLUDE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
#ifdef __cplusplus
extern "C" {
#endif
    void PCStereoUnknownToken(...);
#ifdef __cplusplus
}
#endif
#else
#ifdef __cplusplus
extern "C" {
#endif
    void PCStereoUnknownToken(void);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    void NuSound3AddRumble(void *, f32, i32, f32, f32);
    void NuSound3ClearLoopHold(NUVEC *, i32);
#ifdef __cplusplus
}
#endif

#endif
