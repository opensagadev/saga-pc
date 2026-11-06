#include "saga.h"

/* Empty on PC; the linker folds both bodies to one address. Retail keeps every call to them, and VC8
   drops calls to an empty body it can see, so the definitions live outside rtl.c. */
void __cdecl rtlSwapSetEndianess(void *unused) {
}

// Called by InitUI when a debug menu cannot be created.
void __cdecl RtlUiCreateFailedPc(void) {
}
