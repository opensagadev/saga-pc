/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
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
