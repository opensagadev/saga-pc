#ifndef SAGA_GAMEAPI_EDTOOLS_EDPART_INTERNAL_H
#define SAGA_GAMEAPI_EDTOOLS_EDPART_INTERNAL_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/objects/objectsall.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/items/objects/objectsall.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern PARTTYPE_LOOKUP_PC *edpart_nearest_type;
    extern struct parttype_lookup_pc_s *edpart_nearest_type;
#ifdef __cplusplus
}
#endif

#endif
