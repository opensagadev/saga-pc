#include "saga.h"

/* Placed with the PC keyboard code; PlayersDropInOut calls it rather than
   folding the constant, so the definition lives in its own unit. */
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
