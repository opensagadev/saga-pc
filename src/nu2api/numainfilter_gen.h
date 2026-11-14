#ifndef SAGA_NU2API_NUMAINFILTER_GEN_H
#define SAGA_NU2API_NUMAINFILTER_GEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nu3d/nu3d_includes.h (not all of it earlier in the shim). */

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

#endif
