#ifndef SAGA_LEGOAPI_GIZMOS_OBJECT_LEVER_H
#define SAGA_LEGOAPI_GIZMOS_OBJECT_LEVER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 Lever_BeingPulled(void *);
    extern i32 lever_gizmotype_id;
#ifdef __cplusplus
}
#endif
typedef struct GIZMO_LEVER_PC GIZMO_LEVER_PC;

struct GIZMO_LEVER_PC {
    NUMTX matrix;
    NUVEC floor_position;
    NUVEC target_offset;
    char name[16];
    NUVEC position;
    f32 idle_animation_timer;
    f32 pull_progress;
    f32 auto_reset_timer;
    f32 auto_reset_delay;
    f32 target_indicator_scale;
    u16 yaw;
    i16 model_special_index;
    i16 platform_id;
    u16 animation_frame;
    u16 terrain_pitch;
    u16 terrain_roll;

    union {
        u16 flags;

        struct {
            u16 interacting : 1;
            u16 being_pulled : 1;
            u16 goodie : 1;
            u16 baddie : 1;
            u16 visible : 1;
            u16 auto_reset : 1;
            u16 returning : 1;
            u16 enabled : 1;
            u16 hide_base : 1;
            u16 hide_target_indicator : 1;
            u16 unused_10 : 1;
            u16 interaction_finished : 1;
            u16 pulled_down : 1;
            u16 unused_13 : 3;
        };
    };

    u8 model_variant;
    u8 reserved_97[9];
};

STATIC_ASSERT(sizeof(GIZMO_LEVER_PC) == 0xa0);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, floor_position) == 0x40);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, position) == 0x68);
#ifdef __cplusplus
extern "C" {
#endif
    void Levers_InitTerrain(WORLDINFO *);
    void ReleaseLever(GAMEOBJECT *);
    void Lever_GetAbsTargetPos(GIZMO_LEVER_PC *, NUVEC *);
    GIZMO_LEVER_PC *Lever_FindNearest(WORLDINFO *, NUVEC *, GAMEOBJECT *, f32 *);
    i32 Lever_FullyPulledDown(GIZMO_LEVER_PC *);
    void Lever_MoveCode(WORLDINFO *, GAMEOBJECT *);
    struct addgizmotype_pc_s *Levers_RegisterGizmo(i32 type_id);
    extern char *LeverTypeNamePtrPc;
    extern char *LeverOutputDownNamePtrPc;
    extern char *LeverTypeNamePtrPc;
    extern char *LeverOutputDownNamePtrPc;
#ifdef __cplusplus
}
#endif

#endif
