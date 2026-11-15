#pragma once

#include "nu2api/nucore/common.h"

typedef struct LEVELSAVE_s LEVELSAVE_s;

struct LEVELSAVE_s {           // size 0x54
    char minikit_names[10][8]; // 0x00
    u8 minikit_count;          // 0x50
    u8 reserved_0x51[2];       // 0x51
    u8 arcade_flags;           // 0x53
};

enum { SAVE_INCOMPLETE = 0 };

enum { SAVE_REWARD_100_PERCENT = 1 };

enum { SAVE_REWARD_ALL_GOLD_BRICKS = 2 };

enum { SAVE_CHARACTER_AVAILABLE = 1 };

enum { SAVE_CHARACTER_UNLOCKED = 2 };
