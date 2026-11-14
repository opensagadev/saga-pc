#ifndef SAGA_NU2API_NUCORE_NUMOUSE_H
#define SAGA_NU2API_NUCORE_NUMOUSE_H

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
    void NuMouseRead(void);
    u32 NuMouseReadButtons(void);
    f32 NuMouseReadXVel(void);
    f32 NuMouseReadYVel(void);
    f32 NuMouseReadZVel(void);
    f32 NuMouseReadXRel(void);
    f32 NuMouseReadYRel(void);
    f32 NuMouseReadZRel(void);
#ifdef __cplusplus
}
#endif

#endif
