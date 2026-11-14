#include "saga.h"

/* Empty on PC. Technos_FindTgt calls it for each failed target lookup, and
   VC8 drops calls to an empty body it can see, so the definition lives
   outside techno.c. */
void TechnoTargetDiagnosticPc(char *name) {
}

/* Also empty on PC; the linker folds it into TechnoTargetDiagnosticPc.
   Technos_MoveTarget tail-calls it for special-object targets. */
void TechnoMoveSpecialTargetPc(void) {
}
