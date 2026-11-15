/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* specialmove_s.flags: the keywords of a specialmove_start block (SpecialMoves_Configure). */
#define SPECIALMOVE_CAN_ALWAYS_DO 0x1
#define SPECIALMOVE_KILL_VICTIM_AT_END 0x2
#define SPECIALMOVE_KILL_VICTIM_AT_RELEASE 0x4
#define SPECIALMOVE_FLASH_VICTIM_AT_ATTACK_END 0x8
#define SPECIALMOVE_THROW_KILL_PARTS_UP 0x10

typedef struct specialmove_s {
    i16 attacker_animation;
    i16 attacker_id;
    i8 attacker_action_type;
    i8 victim_action_type;
    i16 victim_id;
    i16 victim_animation;
    u16 flags;
    f32 distance_apart;
} SPECIALMOVE;
