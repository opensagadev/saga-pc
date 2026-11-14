#ifndef SAGA_NU2API_NU3D_NUSHADERMANAGER_GEN_H
#define SAGA_NU2API_NU3D_NUSHADERMANAGER_GEN_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "decomp.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "decomp.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "decomp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern PC_SHADER_SLOT_VIEW *PCShaderSlots;
    PC_SHADER_RECORD_VIEW *PCShaderRecordFromHandle(u32);
    void *PCShaderCurrentObjectPc(void);
    extern f32 PCShaderShininessFactorStorage;
    f32 NuShaderManagerGetShininessFactor(void);
#ifdef __cplusplus
}
#endif

#endif
