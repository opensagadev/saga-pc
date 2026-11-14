#include "saga.h"

/* Empty on PC; the linker folds both bodies to one address. Every call to them is kept, and VC8
   drops calls to an empty body it can see, so the definitions live outside rtl.c. */
void rtlSwapSetEndianess(void *unused) {
}

// Called by InitUI when a debug menu cannot be created.
void RtlUiCreateFailedPc(void) {
}
