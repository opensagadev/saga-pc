#include "saga.h"

/* Original source name unknown. Empty on PC (a bare RET). Technos_FindTgt
   calls it for each failed target lookup, and VC8 drops calls to an empty
   body it can see, so the definition lives outside techno.c. */
void __cdecl TechnoTargetDiagnosticPc(char *name) {
}
