#include "saga.h"

/* Original source name unknown. Empty on PC (a bare RET). Technos_FindTgt
   calls it for each failed target lookup, and VC8 drops calls to an empty
   body it can see, so the definition lives outside techno.c. */
void __cdecl TechnoTargetDiagnosticPc(char *name) {
}

/* Original source name unknown. Also empty on PC; the retail linker folded
   it into TechnoTargetDiagnosticPc. Technos_MoveTarget tail-calls it for
   special-object targets. */
void __cdecl TechnoMoveSpecialTargetPc(void) {
}
