/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void *GizTorpMachine_FindNearest(WORLDINFO *, NUVEC *, f32 *);
    i32 PartDraw_Torp(PART_PC *);
#ifdef __cplusplus
}
#endif
typedef struct GIZTORPMACHINESYS_s GIZTORPMACHINESYS_s;
typedef struct GIZTORPMACHINE_s GIZTORPMACHINE_s;

struct GIZTORPMACHINE_s {
    char name[16];
    NUVEC position;
    f32 activation_time;
    u16 y_rotation;
    u8 reserved_0x22[9];

    union {
        u8 flags;

        struct {
            u8 active : 1;
            u8 visible : 1;
            u8 alternate : 1;
            u8 reserved_flags : 5;
        };
    };
};

STATIC_ASSERT(sizeof(struct GIZTORPMACHINE_s) == 44);

struct GIZTORPMACHINESYS_s {
    struct GIZTORPMACHINE_s *machines;
    i32 count;
    f32 scale;
};

STATIC_ASSERT(sizeof(struct GIZTORPMACHINESYS_s) == 12);
#ifdef __cplusplus
extern "C" {
#endif
    void GizTorp_Activate(void *gizmo, i32 active);
    void GizTorp_AddGizmos(struct gizmosys_pc_s *gizmo_sys, i32 type_id, void *world_ptr, void *arg3);
    void GizTorp_Draw(void *world_ptr, void *arg1, f32 arg2);
    char *GizTorp_GetGizmoName(void *gizmo);
    i32 GizTorp_GetMaxGizmos(void *world_ptr);
    i32 GizTorp_GetNumOutputs(void *arg0);
    i32 GizTorp_GetOutput(void *arg0, i32 arg1, i32 arg2);
    char *GizTorp_GetOutputName(GIZMO_PC *arg0, i32 output_index);
    void GizTorp_SetVisibility(void *gizmo, i32 visible);
    void GizTorp_Update(void *world_ptr, void *arg1, f32 arg2);
    void *GizTorps_ReserveBufferSpace(void *world_ptr);
    void GizTorps_Reset(void *world_ptr, void *arg1, void *arg2);
    extern i32 giztorpmachine_gizmotype_id;
#ifdef __cplusplus
}
#endif
