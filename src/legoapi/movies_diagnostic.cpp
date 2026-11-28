#include "nu2api/nucore/common.h"

/* Original source name unknown. Empty on PC (a bare RET). Movies_ConfigureList
   calls it twice; VC8 drops calls to an empty body it can see, so the
   definition lives in its own unit, not in movies.cpp. */
extern "C" void MoviesListNoOp(char *path) {
}
