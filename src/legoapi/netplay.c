#include "saga.h"
#include <string.h>

/* Retail unit 0x578740-0x5787c4: definitions in retail address order. */

void *__cdecl SetLevelHack(i32 size) {
    memset(OldLevelHackData, 0, 0x80);
    memset(LevelHackData, 0, 0x80);
    LevelHackSendTimer = 0.0f;
    LevelHackDataSize = size;
    return OldLevelHackData;
}

i32 __cdecl objInNetWaitContext(GAMEOBJECT *object, i32 context) {
    if (LegoContextNetWaitPc != -1 && object != NULL && object->character_context == LegoContextNetWaitPc &&
        object->field_7aa == context)
        return 1;
    return 0;
}
