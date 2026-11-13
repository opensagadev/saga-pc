#include "saga.h"

/* Original source name unknown. Native PC entry 0x0053db50 is RET-only: the
   missing sock-end warning was compiled out. SockSysConfigureNuFPar still calls it. */
void __cdecl SockSysMissingSockEndDiagnostic(char *file_name) {
}

/* Original source name unknown. Native PC entry 0x0053db60 is RET-only: the
   missing-camera warning was compiled out. SockSysFindInScene still calls it,
   and VC8 drops calls to an empty body it can see, so the definition lives
   outside socksys.c. */
void __cdecl SockSysMissingCameraDiagnostic(char *name) {
}
