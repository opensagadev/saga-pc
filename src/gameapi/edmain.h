/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edtools/edui.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    NUVEC *edmainQueryLocVec(void);
    void cbMMCancel(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void cbMMRegSel(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbMMReturnToApp(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
#ifdef __cplusplus
}
#endif
