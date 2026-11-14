#ifndef SAGA_LEGOAPI_WORLD_LEVELS_EPISODEIII_TYPES_H
#define SAGA_LEGOAPI_WORLD_LEVELS_EPISODEIII_TYPES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/legoapi_types.h (not all of it earlier in the shim). */

#include "gameapi/ai/aisys/aipath.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#endif

typedef struct vadera_pc_s {
    GIZAIMESSAGE *in_control_room_message;
    GIZAIMESSAGE *ceiling_collapse_message;
    GIZAIMESSAGE *timer_message;
    f32 timer;
    i16 count;
    i16 subtitle;
    GIZFORCE_PC *forces[4];
    AILOCATOR_PC *big_jump_locator;
    i32 collapse_started;
    u8 reset_flag;
    u8 reserved_2d[3];
} VADERA_PC;

STATIC_ASSERT(sizeof(VADERA_PC) == 0x30);
STATIC_ASSERT(offsetof(VADERA_PC, big_jump_locator) == 0x24);

#endif
