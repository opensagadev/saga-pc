#include "saga.h"
void __cdecl IncreaseScore(u32 *total, u64 amount, i32 apply_multiplier) {
    if (apply_multiplier != 0 && BonusArea == 0) {
        if (Cheats_CheckFlags(4) != 0)
            amount *= 2;
        if (Cheats_CheckFlags(8) != 0)
            amount *= 4;
        if (Cheats_CheckFlags(0x10) != 0)
            amount *= 6;
        if (Cheats_CheckFlags(0x20) != 0)
            amount *= 8;
        if (Cheats_CheckFlags(0x40) != 0)
            amount *= 10;
    }
    amount += *total;
    if (amount > 4000000000ULL)
        amount = 4000000000ULL;
    *total = (u32)amount;
}
