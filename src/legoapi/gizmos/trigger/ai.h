#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/ai/aisys/aiscript_types.h"

typedef struct LEVELSCRIPTPROCESS_s LEVELSCRIPTPROCESS_s;

struct LEVELSCRIPTPROCESS_s {  // size 0xdc
    char name[16];             // 0x00
    AISCRIPTPROCESS processor; // 0x10
    u32 reserved_d8;           // 0xd8
};
