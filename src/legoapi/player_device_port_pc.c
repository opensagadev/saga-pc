#include "saga.h"

extern i32 PlayerDevicePortsPc[2];

/* Original source name unknown. Retail places this accessor at 0x421410, away
   from the drop-in code, and PlayersDropInOut calls it rather than inlining
   it, so the definition lives in its own unit. */
i32 __cdecl PlayerDevicePortPc(i32 player_index) {
    if (player_index < 0)
        return -1;
    return PlayerDevicePortsPc[player_index];
}
