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
    void NuDisplayListRecalculateMaterialStartsPC(NUDISPLAY_LIST_PC *);
    void PCDisplayListCriticalSectionEnterPc(void);
    void PCDisplayListCriticalSectionLeavePc(void);
    void NuRndrInitStartupMaterialsPc(void);
#ifdef __cplusplus
}
#endif
