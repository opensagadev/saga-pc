/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 PS2_VREZ_W;
    extern i32 PS2_VCNTR_X;
    extern i32 PS2_VCNTR_Y;
    extern i32 PS2_VREZ_H;
    void NuVpGetPosition2(f32 *x, f32 *y);
    void NuVpUpdate(void);
    void NuVpGetSize2(f32 *, f32 *);

    void NuPs2GetViewport(NUVPCURRENT *viewport);
    void NuVpInit(void);
#ifdef __cplusplus
}
#endif
