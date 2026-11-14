#ifndef SAGA_NU2API_NU3D_NUSHADER_INTERNAL_H
#define SAGA_NU2API_NU3D_NUSHADER_INTERNAL_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "decomp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void NuShaderObjectInit(PC_SHADER_OBJECT_FULL_VIEW *, const u32 *, i32, const char *, i32, const char *, i32, i32);
#ifdef __cplusplus
}
#endif

#endif
