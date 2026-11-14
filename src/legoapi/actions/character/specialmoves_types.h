#ifndef SAGA_LEGOAPI_ACTIONS_CHARACTER_SPECIALMOVES_TYPES_H
#define SAGA_LEGOAPI_ACTIONS_CHARACTER_SPECIALMOVES_TYPES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

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
