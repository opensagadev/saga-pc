#ifndef SAGA_LEGOAPI_GIZMOS_FX_EDGIZSHADOWMACHINE_H
#define SAGA_LEGOAPI_GIZMOS_FX_EDGIZSHADOWMACHINE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/gizmo/base/gizmo.h, nu2api/numath/nuvec.h (not all of it earlier in the
 * shim). */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "legoapi/gizmo/base/gizmo.h"
#include "nu2api/numath/nuvec.h"
#endif

typedef struct edgizshadow_pc_s {
    NUVEC direction;
    f32 field_0c;
    f32 field_10;
    f32 field_14;
    f32 field_18;
    f32 field_1c;
    f32 field_20;
    f32 field_24;
    f32 field_28;
    f32 field_2c;
    f32 field_30;
    i32 preset;
} EDGIZSHADOW_PC;

STATIC_ASSERT(sizeof(EDGIZSHADOW_PC) == 0x38);
#ifdef __cplusplus
extern "C" {
#endif
    SHADOW_REGISTRATION_PC *EdGizShadowMachine_RegisterGizmo(i32 type_id);
#ifdef __cplusplus
}
#endif

#endif
