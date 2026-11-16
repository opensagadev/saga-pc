#pragma once

#include "nu2api/nucore/common.h"

typedef struct nupad_init_state_s { // size 0x8b
    u8 unknown_00[0x78];            // 0x00
    u32 digital_buttons_pressed;    // 0x78
    u8 unknown_7c[0xc];             // 0x7c
    u8 init_type;                   // 0x88
    u8 init_flags;                  // 0x89
    u8 init_port;                   // 0x8a
} NUPAD_INIT_STATE;

#ifdef __cplusplus
extern "C" {
#endif
    void Movies_ConfigureList(char *, VARIPTR *, VARIPTR *);
#ifdef __cplusplus
}
#endif
