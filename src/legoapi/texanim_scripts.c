#include "saga.h"

/* Empty on PC. Retail InitTexAnimScripts (utilities unit) still calls it, so the
   body was not visible there: VC8 drops calls to an empty body it can see. */
void __cdecl NuTexAnimScriptMissingPc(char *path) {
    (void)path;
}
