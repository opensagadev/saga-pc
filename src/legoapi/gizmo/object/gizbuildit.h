/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

struct gameanimobj_s;
struct gameanimset_s;

typedef struct GIZBUILDIT_PC {            // size 0x7c
    char name[16];                        // 0x00
    struct gameanimset_s *anim_set;       // 0x10
    struct gameanimobj_s **anim_objects;  // 0x14
    struct GIZBUILDIT_PC *linked_buildit; // 0x18
    NUVEC file_position;                  // 0x1c
    NUVEC start_position;                 // 0x28
    NUVEC position;                       // 0x34
    f32 bounds_radius;                    // 0x40
    f32 step_timer;                       // 0x44
    f32 step_duration;                    // 0x48
    u8 unknown_4c[0x20];                  // 0x4c
    u8 anim_object_count;                 // 0x6c
    u8 build_state;                       // 0x6d
    u8 builders_active;                   // 0x6e
    u8 built_object_count;                // 0x6f
    u8 unknown_70;                        // 0x70
    u8 progress;                          // 0x71
    u16 state_flags;                      // 0x72
    u8 unknown_74[6];                     // 0x74
    u16 availability_flags;               // 0x7a
} GIZBUILDIT_PC;
