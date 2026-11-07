#include "saga.h"

/* Empty on PC (a bare RET). Customiser_Configure calls it when a customiser
   is already configured, and VC8 drops calls to an empty body it can see,
   so the definition lives outside customise.c. */
void __cdecl Customiser_Dump(void) {
}

/* Original source name unknown. Also empty on PC; the retail linker folded it
   into Customiser_Dump. Customiser_LoadAll calls it with the path of each
   scene or texture it has just loaded. */
void __cdecl CustomiserLoadTracePc(char *path) {
}
