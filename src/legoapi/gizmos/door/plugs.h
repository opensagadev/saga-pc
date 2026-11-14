#ifndef SAGA_LEGOAPI_GIZMOS_DOOR_PLUGS_H
#define SAGA_LEGOAPI_GIZMOS_DOOR_PLUGS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "legoapi/actions/movement/carrying.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

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
