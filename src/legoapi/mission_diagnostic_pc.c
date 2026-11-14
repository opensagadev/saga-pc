#include "saga.h"

/* Empty on PC. Missions_Configure calls it when the configuration file
   cannot be opened, and VC8 drops calls to an empty body it can see, so the
   definition lives outside missions.c. */
void MissionsConfigMissingPc(char *filename) {
}
