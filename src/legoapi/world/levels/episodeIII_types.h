/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aipath.h"
#include "legoapi/legoapi_types.h"
#endif

struct gizaimessage_s;

typedef struct vadera_s {                            // size 0x30
    struct gizaimessage_s *in_control_room_message;  // 0x00
    struct gizaimessage_s *ceiling_collapse_message; // 0x04
    struct gizaimessage_s *timer_message;            // 0x08
    f32 timer;                                       // 0x0c
    i16 count;                                       // 0x10
    i16 subtitle;                                    // 0x12
    GIZFORCE_PC *forces[4];                          // 0x14
    AILOCATOR *big_jump_locator;                     // 0x24
    i32 collapse_started;                            // 0x28
    u8 reset_flag;                                   // 0x2c
    u8 reserved_2d[3];                               // 0x2d
} VADERA_PC;
