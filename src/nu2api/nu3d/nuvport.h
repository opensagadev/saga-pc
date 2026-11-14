#ifndef SAGA_NU2API_NU3D_NUVPORT_H
#define SAGA_NU2API_NU3D_NUVPORT_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h (not all of it earlier in the shim). */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

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

#endif
