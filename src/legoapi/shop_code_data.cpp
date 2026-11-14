#include "saga.h"

extern "C" {
    i32 ShopCodeCharacter = 0;
    i32 ShopCodeValid = 0;
    i32 ShopCodeMoveSfxLock = 0;
    i32 ShopCodeItemChanged = 0;
    f32 ShopCodeRepeatTimer = 0;
    f32 ShopCodeRepeatInterval = 0.125f;
    f32 ShopNativeSubNormCharPush = 0.02f;
    f32 ShopTopCodeOffset = -0.005f;
    f32 ShopTopExtraOffset = 0.005f;
    f32 ShopTopHintOffset = -0.006f;
    f32 ShopCodeExitPush = 0.05f;
    char ShopCodeCharacters[37] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
}
