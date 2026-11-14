#include "saga.h"

/* Empty on PC: the missing sock-end warning was compiled out.
   SockSysConfigureNuFPar still calls it. */
void SockSysMissingSockEndDiagnostic(char *file_name) {
}

/* Empty on PC: the missing-camera warning was compiled out.
   SockSysFindInScene still calls it, and VC8 drops calls to an empty body it
   can see, so the definition lives outside socksys.c. */
void SockSysMissingCameraDiagnostic(char *name) {
}
