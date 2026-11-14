#include "saga.h"
#include <string.h>

void *SetLevelHack(i32 size) {
    memset(OldLevelHackData, 0, 0x80);
    memset(LevelHackData, 0, 0x80);
    LevelHackSendTimer = 0.0f;
    LevelHackDataSize = size;
    return OldLevelHackData;
}

i32 objInNetWaitContext(GAMEOBJECT *object, i32 context) {
    if (LegoContextNetWaitPc != -1 && object != NULL && object->character_context == LegoContextNetWaitPc &&
        object->field_7aa == context) {
        return 1;
    }
    return 0;
}

void fun_005787d0(GAMEOBJECT *object, i16 value) {
    if (object != NULL) {
        object->character_context = LegoContextNetWaitPc;
        object->field_7aa = value;
    }
}

i32 fun_00578800(i16 high, u16 low) {
    return (high << 16) | low;
}

i32 fun_00578810(i8 a, u8 b, u8 c, u8 d) {
    return (((((a << 8) | b) << 8) | c) << 8) | d;
}

void fun_00578840(u32 value, u16 *high, u16 *low) {
    *high = (value >> 16);
    *low = value;
}

void fun_00578860(i32 value, u8 *a, u8 *b, u8 *c, u8 *d) {
    *a = ((u32)value >> 26) & 0x3f;
    *b = (value >> 16);
    *c = (value >> 8);
    *d = value;
}
