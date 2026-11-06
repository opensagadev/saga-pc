#include "saga.h"

/* Original source name unknown. Empty on PC (a bare RET). Levels_ConfigureList
   calls it when the level list cannot be opened or holds no levels. VC8 drops
   calls to an empty body it can see, so the definition lives in its own unit. */
void __cdecl LevelsConfigDiagnosticPc(char *message) {
}

/* Original source name unknown. Also empty on PC; the retail linker folded it
   into LevelsConfigDiagnosticPc. LevelObjects_InitForGame calls it with the name
   table size when the name table overruns the buffer. */
void __cdecl LevelObjectsArenaDiagnosticPc(i32 size) {
}
