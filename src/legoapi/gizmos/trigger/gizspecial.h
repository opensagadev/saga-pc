/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/gizmo/base/gizmo.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 gizspecial_gizmotype_id;
    GIZMO_PC *createGizSpecial(void *, char *);
    struct addgizmotype_pc_s *GizSpecial_RegisterGizmo(i32 type_id);
#ifdef __cplusplus
}
#endif
