/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

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
