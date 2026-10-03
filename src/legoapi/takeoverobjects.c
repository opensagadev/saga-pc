#include "saga.h"
#include <string.h>

extern u8 takeoverobjects[0x160];
extern i32 num_takeoverobjects;

void __cdecl ClearTakeOverObjectSys(void) {
    memset(takeoverobjects, 0, sizeof(takeoverobjects));
    num_takeoverobjects = 0;
}
