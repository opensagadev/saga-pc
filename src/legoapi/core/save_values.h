#ifndef SAGA_LEGOAPI_CORE_SAVE_VALUES_H
#define SAGA_LEGOAPI_CORE_SAVE_VALUES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/fixed_width.h"

typedef struct LEVELSAVE_s LEVELSAVE_s;

struct LEVELSAVE_s {
    char minikit_names[10][8];
    u8 minikit_count;
    u8 reserved_0x51[2];
    u8 arcade_flags;
};

STATIC_ASSERT(sizeof(struct LEVELSAVE_s) == 84);
STATIC_ASSERT(offsetof(struct LEVELSAVE_s, minikit_count) == 0x50);

enum { SAVE_INCOMPLETE = 0 };

enum { SAVE_REWARD_100_PERCENT = 1 };

enum { SAVE_REWARD_ALL_GOLD_BRICKS = 2 };

enum { SAVE_CHARACTER_AVAILABLE = 1 };

enum { SAVE_CHARACTER_UNLOCKED = 2 };

#endif
