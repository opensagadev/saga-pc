#pragma once

#include "nu2api/nucore/common.h"

/* NuKeyGet keys: DirectInput keyboard scan codes (dinput.h) */
#define DIK_ESCAPE 0x01
#define DIK_BACK 0x0e
#define DIK_RETURN 0x1c
#define DIK_UP 0xc8
#define DIK_LEFT 0xcb
#define DIK_RIGHT 0xcd
#define DIK_DOWN 0xd0
#define DIK_DELETE 0xd3

#ifdef __cplusplus
extern "C" {
#endif
    void NuKeyboardRead(void);
    char NuKeyToAscii(u32 key, i32 shifted);
    i32 NuKeyboard(i32);
    i32 NuKeyboard_db(i32);
    i32 NuKeyGet(u32 *modifiers);
#ifdef __cplusplus
}
#endif
