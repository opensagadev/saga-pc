#include "nu2api/nucore/common.h"

/* Empty on PC. Missions_Configure calls it when the configuration file
   cannot be opened, and VC8 drops calls to an empty body it can see, so the
   definition lives outside missions.c. */
extern "C" void MissionsConfigMissing(char *filename) {
}
