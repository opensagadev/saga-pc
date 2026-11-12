#include "saga.h"

/* Original source name unknown. Empty on PC (a bare RET at 0x58a510). Missions_Configure
   calls it when the configuration file cannot be opened, and VC8 drops calls to an empty
   body it can see, so the definition lives outside mission.c. */
void __cdecl MissionsConfigMissingPc(char *filename) {
}
