/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

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
