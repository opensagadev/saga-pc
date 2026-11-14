#ifndef SAGA_LEGOAPI_GIZMO_OBJECT_GIZBUILDIT_H
#define SAGA_LEGOAPI_GIZMO_OBJECT_GIZBUILDIT_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/legoapi_types.h (not all of it earlier in the shim). */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#endif

typedef struct GIZBUILDIT_PC {
    char name[16];
    GAMEANIMSET *anim_set;
    GAMEANIMOBJ **anim_objects;
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

#ifndef TYPES_OFF_14
/* These allocation records use the native PC strides, rather than the larger imported views. */
typedef struct registry_buildit_record_pc_s {
    u8 unknown_00[0x10];      // 0x00
    GAMEANIMSET *anim_set;    // 0x10
    u8 unknown_14[0x64];      // 0x14
    i16 blowup_type;          // 0x78
    u16 flags;                // 0x7a
} REGISTRY_BUILDIT_RECORD_PC; // size 0x7c

typedef struct registry_buildit_system_pc_s {
    REGISTRY_BUILDIT_RECORD_PC *buildits; // 0x00
    u16 count;                            // 0x04
    u16 capacity;                         // 0x06
    u16 max_animation_objects;            // 0x08
    u16 padding;                          // 0x0a
    GAMEANIMOBJPOOL *anim_pool;           // 0x0c
    GAMEANIMOBJ **anim_objects;           // 0x10
} REGISTRY_BUILDIT_SYSTEM_PC;             // size 0x14

#endif /* TYPES_OFF_14 */
#endif
