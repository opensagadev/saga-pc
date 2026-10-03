#include "saga.h"

extern char DoorExitNamePc[];
extern i32 Door_Start;
extern i32 Door_NextSock;

void __cdecl Door_Reset(void) {
    DoorExitNamePc[0] = '\0';
    Door_Start = 0;
    Door_NextSock = -1;
}
