#include "saga.h"

/* Network message pool support object (0x60f2a0-0x60f3a0). */

void NetMessagePoolError(void) {
    theNetSessionManager->field_34 = (i32)0xa0001000;
}
