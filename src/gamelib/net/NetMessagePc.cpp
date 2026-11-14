#include "saga.h"

/* Network message pool support object (0x60f2a0-0x60f3a0). */

void NetMessagePoolError(void) {
    theNetSessionManager->error = (i32)0xa0001000;
}

extern "C" void fun_0060f2e0() {
}

extern "C" void fun_0060f300() {
}

extern "C" void fun_0060f3c0() {
}
