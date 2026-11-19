#include "globals.h"
#include "legoapi/characters/motion.h"
#include "legoapi/core/net/netplay.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/level.h"
#include "legogame/startup.h"
#include <string.h>

void *LevelHackData = 0;
i32 LevelHackDataSize = 0;
void *OldLevelHackData = 0;
f32 LevelHackSendTimer = 0;

void *SetLevelHack(i32 size) {
    memset(OldLevelHackData, 0, 0x80);
    memset(LevelHackData, 0, 0x80);
    LevelHackSendTimer = 0.0f;
    LevelHackDataSize = size;
    return OldLevelHackData;
}

i32 objInNetWaitContext(GAMEOBJECT *object, i32 context) {
    if (LegoContextNetWait != -1 && object != NULL && object->player.character_context == LegoContextNetWait &&
        object->player.context_animation == context) {
        return 1;
    }
    return 0;
}

void NetPlay_SetWaitContext(GAMEOBJECT *object, i16 value) {
    if (object != NULL) {
        object->player.character_context = LegoContextNetWait;
        object->player.context_animation = value;
    }
}

i32 NetPlay_Pack16x2(i16 high, u16 low) {
    return (high << 16) | low;
}

i32 NetPlay_Pack8x4(i8 a, u8 b, u8 c, u8 d) {
    return (((((a << 8) | b) << 8) | c) << 8) | d;
}

void NetPlay_Unpack16x2(u32 value, u16 *high, u16 *low) {
    *high = (value >> 16);
    *low = value;
}

void NetPlay_Unpack8x4(i32 value, u8 *a, u8 *b, u8 *c, u8 *d) {
    *a = ((u32)value >> 26) & 0x3f;
    *b = (value >> 16);
    *c = (value >> 8);
    *d = value;
}
