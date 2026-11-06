#include "saga.h"

/* Original source name unknown. Empty on PC (a bare RET). SpecialMoves_Configure
   calls it when the configuration file cannot be opened, and VC8 drops calls to
   an empty body it can see, so the definition lives outside specialmoves.c. */
void __cdecl SpecialMovesConfigMissingPc(char *filename) {
}
