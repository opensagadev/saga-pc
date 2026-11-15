#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/ai/aisys/aiscript_types.h"

typedef struct LEVELSCRIPTPROCESS_s LEVELSCRIPTPROCESS_s;

struct LEVELSCRIPTPROCESS_s {  // size 0xdc
    char name[16];             // 0x00
    AISCRIPTPROCESS processor; // 0x10
    u32 unknown_d8;            // 0xd8
};

typedef struct {                           // size 0x4670
    u8 opaque[0x2aec];                     // 0x0000
    i32 count;                             // 0x2aec
    struct LEVELSCRIPTPROCESS_s items[32]; // 0x2af0
} ADD_AI_WORLD;
