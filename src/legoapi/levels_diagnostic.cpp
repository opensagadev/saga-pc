#include "nu2api/nucore/common.h"

/* Original source name unknown. Empty on PC. Levels_ConfigureList
   calls it when the level list cannot be opened or holds no levels. VC8 drops
   calls to an empty body it can see, so the definition lives in its own unit. */
extern "C" void LevelsConfigDiagnostic(char *message) {
}

/* Original source name unknown. Also empty on PC; the linker folded it
   into LevelsConfigDiagnostic. LevelObjects_InitForGame calls it with the name
   table size when the name table overruns the buffer. */
extern "C" void LevelObjectsArenaDiagnostic(i32 size) {
}
