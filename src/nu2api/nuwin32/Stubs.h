/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/menus/screens/movies.h"
#endif

typedef struct MENU_PC_STRING_ENTRY { // size 0x8
    i32 id;                           // 0x0
    const char *text;                 // 0x4
} MENU_PC_STRING_ENTRY;

#ifdef __cplusplus
extern "C" {
#endif
    void MenuPcStringsInit(void);
    extern MENU_PC_STRING_ENTRY dat_00827800[40];
    extern MENU_PC_STRING_ENTRY dat_00827940[40];
    extern MENU_PC_STRING_ENTRY dat_00827a80[40];
    extern MENU_PC_STRING_ENTRY dat_00827bc0[40];
    extern MENU_PC_STRING_ENTRY dat_00827d00[40];
    extern MENU_PC_STRING_ENTRY dat_00827e40[40];
    extern MENU_PC_STRING_ENTRY dat_00827f80[40];
    void PCAnimationEffectCheckpoint(void);
    void NuPadSetMotorsPc(NUPAD_INIT_STATE *, i32, i32);
#ifdef __cplusplus
}
#endif
