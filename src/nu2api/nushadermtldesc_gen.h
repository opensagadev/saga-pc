/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    void RenderPostClearScreenPc(void);
    u8 NuPrim2DAddCLR(i32 minimum_unused, i32 maximum_unused, f32 value);
#ifdef __cplusplus
}
#endif
