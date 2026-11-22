#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/core/config/cheat.h"
#include "legoapi/legoapi_types.h"

typedef struct cheatsystem_s { // size 0xc
    CHEAT *cheats;             // 0x0
    i32 count;                 // 0x4
    u32 flags;                 // 0x8
} CHEATSYSTEM;

#ifdef __cplusplus
extern "C" {
#endif
    void Cheats_FrameUpdate(void);
    i32 GetAnimDirection(NUINSTANIM *);
#ifdef __cplusplus
}
#endif
