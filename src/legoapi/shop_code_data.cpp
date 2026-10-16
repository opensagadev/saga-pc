#include "saga.h"

extern "C" {
    i32 ShopCodeCharacterPc = 0;
    i32 ShopCodeValidPc = 0;
    i32 ShopCodeMoveSfxLockPc = 0;
    i32 ShopCodeItemChangedPc = 0;
    f32 ShopCodeRepeatTimerPc = 0;
    f32 ShopCodeRepeatIntervalPc = 0.125f;
    f32 ShopCodeExitPushPc = 0.05f;
    char ShopCodeCharactersPc[37] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    extern const f32 ShopCodeCheatDisplayTimePc = 3.1499998569488525f;
}
