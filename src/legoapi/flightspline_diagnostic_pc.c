#include "saga.h"

/* Original source name unknown. The native diagnostic entry is exactly RET;
   exceeding capacity therefore does not stop or truncate the native loader.
   FlightSpline_Init still calls it, and VC8 drops calls to an empty body it
   can see, so the definition lives outside edsplines.c. */
void __cdecl FlightSplineCapacityDiagnosticPc(char *filename) {
}
