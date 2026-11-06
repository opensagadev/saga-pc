#include "saga.h"

/* Original source name unknown. Empty on PC (a bare RET at 0x5845b0).
   CharConfig_ConfigureAll calls it around its pack lookups, with and without
   the path. VC8 drops calls to an empty body it can see, so the definition
   lives in its own unit. */
void __cdecl CharConfigPakDiagnosticPc() {
}
