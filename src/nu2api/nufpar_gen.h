/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nufile/nufpar.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void NuFreeFPar(NUFPAR_PC *parser);
#ifdef __cplusplus
}
#endif

typedef struct nualloc_manager_trailer_s { // size 0x4
    NUALLOC_MANAGER_BLOCK *previous_block; // 0x0
} NUALLOC_MANAGER_TRAILER;
