#pragma once

#include "nu2api/nucore/common.h"

typedef struct nupad_init_state_s { // size 0x8b
    u8 unknown_00[0x88];            // 0x00
    u8 init_type;                   // 0x88
    u8 init_flags;                  // 0x89
    u8 init_port;                   // 0x8a
} NUPAD_INIT_STATE;
