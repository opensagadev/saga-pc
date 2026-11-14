#ifndef SAGA_LEGOAPI_WORLD_LEVELS_EPISODEIII_TYPES_H
#define SAGA_LEGOAPI_WORLD_LEVELS_EPISODEIII_TYPES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aipath.h"
#include "legoapi/legoapi_types.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aipath.h"
#include "legoapi/legoapi_types.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aipath.h"
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
