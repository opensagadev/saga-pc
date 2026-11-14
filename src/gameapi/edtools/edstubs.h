#ifndef SAGA_GAMEAPI_EDTOOLS_EDSTUBS_H
#define SAGA_GAMEAPI_EDTOOLS_EDSTUBS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/fixed_width.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 *edbits_editor_enabled;
    extern i32 edbits_editmode;
    extern void *edbits_base_terrain;
#ifdef __cplusplus
}
#endif

#endif
