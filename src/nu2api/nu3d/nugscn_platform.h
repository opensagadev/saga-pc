/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nufile/nufile.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuGScnUploadGfxDataFromFilePS(VARIPTR *, VARIPTR, NUFILE);
#ifdef __cplusplus
}
#endif
