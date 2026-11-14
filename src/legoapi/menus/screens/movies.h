#ifndef SAGA_LEGOAPI_MENUS_SCREENS_MOVIES_H
#define SAGA_LEGOAPI_MENUS_SCREENS_MOVIES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

typedef struct nupad_init_state_s {
    u8 unknown_00[0x88];
    u8 init_type;
    u8 init_flags;
    u8 init_port;
} NUPAD_INIT_STATE;

STATIC_ASSERT(offsetof(NUPAD_INIT_STATE, init_type) == 0x88);
STATIC_ASSERT(offsetof(NUPAD_INIT_STATE, init_flags) == 0x89);
STATIC_ASSERT(offsetof(NUPAD_INIT_STATE, init_port) == 0x8a);

#endif
