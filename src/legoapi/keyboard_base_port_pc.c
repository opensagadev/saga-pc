#include "saga.h"

/* Original source name unknown. Retail places this with the PC keyboard code
   at 0x6d5040, and PlayersDropInOut calls it rather than folding the constant,
   so the definition lives in its own unit. */
i32 __cdecl NuKeyboardBasePortPc(void) {
    return 10;
}
