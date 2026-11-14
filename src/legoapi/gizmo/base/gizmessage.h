#ifndef SAGA_LEGOAPI_GIZMO_BASE_GIZMESSAGE_H
#define SAGA_LEGOAPI_GIZMO_BASE_GIZMESSAGE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/legoapi_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void ResetGizAIMessageSys(GIZAIMESSAGESYS *sys);
    void ClearGizAIMessageSys(GIZAIMESSAGESYS *sys);
    f32 GetGizAIMessage(GIZAIMESSAGESYS *sys, const char *name, GIZAIMESSAGE *message);
    GIZAIMESSAGE *QueryGizAIMessage(GIZAIMESSAGESYS *sys, GIZAIMESSAGE *message);
    extern char gizaimessage_prefix[5];
    extern const char GizAIMessage_NameFormat[3];
    extern const char GizAIMessage_PrefixedNameFormat[5];
    GIZAIMESSAGESYS *CreateGizAIMessageSys(VARIPTR *cursor, VARIPTR *end, i32 count);
    void GizAIMessage_AddGizmos(struct gizmosys_pc_s *gizmo_sys, i32 arg1, void *arg2, void *arg3);
    char *GizAIMessage_GetGizmoName(GIZMO_PC *gizmo);
    i32 GizAIMessage_GetMaxGizmos(void *arg0);
    i32 GizAIMessage_GetNumOutputs(GIZMO_PC *gizmo);
    i32 GizAIMessage_GetOutput(GIZMO_PC *gizmo, i32 output_index, i32 arg2);
    char *GizAIMessage_GetOutputName(GIZMO_PC *gizmo, i32 output_index);
    extern i32 gizaimessage_gizmotype_id;
#ifdef __cplusplus
}
#endif

#endif
