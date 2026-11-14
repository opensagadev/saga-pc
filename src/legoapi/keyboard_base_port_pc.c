#include "saga.h"

/* Original source name unknown. Retail places this with the PC keyboard code
   at 0x6d5040, and PlayersDropInOut calls it rather than folding the constant,
   so the definition lives in its own unit. */
i32 PcInput_GetFirstKeyboardPort(void) {
    return 10;
}

void fun_006d5050(const i8 *order) {
    i8 *destination;
    i32 value;
    PcInputActionOrder[0] = -1;
    if (order != NULL) {
        destination = PcInputActionOrder;
        do {
            value = *order++;
            *destination++ = value;
        } while (value >= 0);
    }
}
