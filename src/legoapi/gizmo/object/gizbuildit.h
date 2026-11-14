#ifndef SAGA_LEGOAPI_GIZMO_OBJECT_GIZBUILDIT_H
#define SAGA_LEGOAPI_GIZMO_OBJECT_GIZBUILDIT_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

struct gameanimobj_s;
struct gameanimset_s;

typedef struct GIZBUILDIT_PC {
    char name[16];
    struct gameanimset_s *anim_set;
    struct gameanimobj_s **anim_objects;
    struct GIZBUILDIT_PC *linked_buildit;
    NUVEC file_position;
    NUVEC start_position;
    NUVEC position;
    f32 bounds_radius;
    f32 step_timer;
    f32 step_duration;
    u8 unknown_4c[0x20];
    u8 anim_object_count;
    u8 build_state;
    u8 builders_active;
    u8 built_object_count;
    u8 unknown_70;
    u8 progress;
    u16 state_flags;
    u8 unknown_74[6];
    u16 availability_flags;
} GIZBUILDIT_PC;

STATIC_ASSERT(sizeof(GIZBUILDIT_PC) == 0x7c);
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, anim_objects) == 0x14);
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, linked_buildit) == 0x18);
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, start_position) == 0x28);
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, bounds_radius) == 0x40);
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, anim_object_count) == 0x6c);
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, progress) == 0x71);
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, state_flags) == 0x72);
STATIC_ASSERT(offsetof(GIZBUILDIT_PC, availability_flags) == 0x7a);

#endif
