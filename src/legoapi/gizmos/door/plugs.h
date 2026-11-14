#ifndef SAGA_LEGOAPI_GIZMOS_DOOR_PLUGS_H
#define SAGA_LEGOAPI_GIZMOS_DOOR_PLUGS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/actions/movement/carrying.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    struct carry_plug_pc_s *Plug_FindNearest(struct carry_plug_sys_pc_s *, NUVEC *, f32 *, i32);
    void Plug_MakeDrawMtx(struct carry_plug_pc_s *, NUMTX *);
    extern const char *PlugGizmoNamePc;
    extern const char *PlugOutputNamePc;
    extern i32 plug_gizmotype_id;
    struct addgizmotype_pc_s *Plugs_RegisterGizmo(i32);
#ifdef __cplusplus
}
#endif

#endif
