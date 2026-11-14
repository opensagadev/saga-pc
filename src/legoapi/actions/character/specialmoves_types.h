#ifndef SAGA_LEGOAPI_ACTIONS_CHARACTER_SPECIALMOVES_TYPES_H
#define SAGA_LEGOAPI_ACTIONS_CHARACTER_SPECIALMOVES_TYPES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nucore/common.h (not all of it earlier in the shim). */

#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "nu2api/nucore/common.h"
#endif

typedef struct specialmove_pc_s {
    i16 attacker_animation;
    i16 attacker_id;
    i8 attacker_action_type;
    i8 victim_action_type;
    i16 victim_id;
    i16 victim_animation;
    u16 flags;
    f32 distance_apart;
} SPECIALMOVE_PC;

#endif
