#include "saga.h"

/* Original source name unknown. Empty on PC (a bare RET). Areas_ConfigureList
   calls it when the area list cannot be opened or holds no areas. VC8 drops calls
   to an empty body it can see, so the definition lives in its own unit. */
void __cdecl AreasConfigDiagnosticPc(char *message) {
}
