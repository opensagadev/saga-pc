#include "saga.h"

/* Network message pool support object (0x60f2a0-0x60f3a0). */

void NetMessagePoolError(void) {
    theNetSessionManager->error = (i32)0xa0001000;
}

/* Empty varargs trace used by NetworkObjectManager::ImportObjects (0x6118f0);
 * kept out of line so its callers keep the call. */
void NOSTrace(...) {
}
