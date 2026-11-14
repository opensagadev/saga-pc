#ifndef SAGA_NU2API_NU3D_SHADERSEMANTICS_H
#define SAGA_NU2API_NU3D_SHADERSEMANTICS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: nu2api/nu3d/nushader.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "nu2api/nu3d/nushader.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern ShaderUniformRecordPc ShaderUniformRecordsPc[100];
#ifdef __cplusplus
}
#endif

#endif
