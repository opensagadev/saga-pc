#include "saga.h"

extern "C" {
    i32 ShopCodeCharacterPc;
    i32 ShopCodeValidPc;
    i32 ShopCodeMoveSfxLockPc;
    i32 ShopCodeItemChangedPc;
    f32 ShopCodeRepeatTimerPc;
    f32 ShopCodeRepeatIntervalPc = 0.125f;
    f32 ShopCodeExitPushPc = 0.05f;
    char ShopCodeCharactersPc[37] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    extern const f32 ShopCodeCheatDisplayTimePc = 3.1499998569488525f;
}
