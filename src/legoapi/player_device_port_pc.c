#include "saga.h"

extern i32 PlayerDevicePortsPc[2];

/* Placed away from the drop-in code; PlayersDropInOut calls it rather than
   inlining it, so the definition lives in its own unit. */
i32 PC_CtrlConfigGetPlayerPort(i32 player_index) {
    if (player_index < 0) {
        return -1;
    }
    return PlayerDevicePortsPc[player_index];
}
