#ifndef SAGA_NU2API_NU3D_ANDROID_NUVERTEXFORMAT_ANDROID_H
#define SAGA_NU2API_NU3D_ANDROID_NUVERTEXFORMAT_ANDROID_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: nu2api/nuwin32/d3dCore.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "nu2api/nuwin32/d3dCore.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    PC_TEXTURE_RESOURCE_PC *NuGetVertexDeclaration(u32);
#ifdef __cplusplus
}
#endif

#endif
