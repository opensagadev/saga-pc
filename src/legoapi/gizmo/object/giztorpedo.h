#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"

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

struct GIZTORPMACHINE_s { // size 0x2c
    char name[16];        // 0x00
    NUVEC position;       // 0x10
    f32 activation_time;  // 0x1c
    u16 y_rotation;       // 0x20
    u8 reserved_0x22[9];  // 0x22

    union { // size 0x1
        u8 flags;

        struct {                   // size 0x1
            u8 active : 1;         // 0x2b:0
            u8 visible : 1;        // 0x2b:1
            u8 alternate : 1;      // 0x2b:2
            u8 reserved_flags : 5; // 0x2b:3
        };
    }; // 0x2b
};

struct GIZTORPMACHINESYS_s {           // size 0xc
    struct GIZTORPMACHINE_s *machines; // 0x0
    i32 count;                         // 0x4
    f32 scale;                         // 0x8
};

#ifdef __cplusplus
extern "C" {
#endif
    void GizTorp_Activate(void *gizmo, i32 active);
    void GizTorp_AddGizmos(struct gizmosys_s *gizmo_sys, i32 type_id, void *world_ptr, void *arg3);
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
