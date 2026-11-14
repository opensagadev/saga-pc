#ifndef SAGA_NU2API_NUSHADERMTLDESC_GEN_H
#define SAGA_NU2API_NUSHADERMTLDESC_GEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nucore/common.h (not all of it earlier in the shim). */

#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "nu2api/nucore/common.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void RenderPostClearScreenPc(void);
    u8 NuPrim2DAddCLR(i32 minimum_unused, i32 maximum_unused, f32 value);
#ifdef __cplusplus
}
#endif

#endif
