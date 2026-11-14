/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    struct addgizmotype_pc_s *MiniCut_RegisterGizmo(i32);
#ifdef __cplusplus
}
#endif
