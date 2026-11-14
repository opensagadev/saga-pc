#ifndef SAGA_GAMEAPI_EDTOOLS_EDBRI_INTERNAL_H
#define SAGA_GAMEAPI_EDTOOLS_EDBRI_INTERNAL_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "gameapi/edbits.h"
#include "nu2api/nucore/fixed_width.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern PC_EDBRIDGE edBridges[64];
    extern i32 edbri_page_used[8];
    void edbriStartPage(i32 page);
    void edbriStartAllPages(void);
#ifdef __cplusplus
}
#endif

#endif
