#ifndef SAGA_NU2API_NUWIN32_STUBS_H
#define SAGA_NU2API_NUWIN32_STUBS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#include "legoapi/menus/screens/movies.h"

typedef struct MENU_PC_STRING_ENTRY {
    i32 id;
    const char *text;
} MENU_PC_STRING_ENTRY;

STATIC_ASSERT(sizeof(MENU_PC_STRING_ENTRY) == 8);
STATIC_ASSERT(offsetof(MENU_PC_STRING_ENTRY, text) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    void MenuPcStringsInit(void);
    extern MENU_PC_STRING_ENTRY MenuPcStringTable827800[40];
    extern MENU_PC_STRING_ENTRY MenuPcStringTable827940[40];
    extern MENU_PC_STRING_ENTRY MenuPcStringTable827a80[40];
    extern MENU_PC_STRING_ENTRY MenuPcStringTable827bc0[40];
    extern MENU_PC_STRING_ENTRY MenuPcStringTable827d00[40];
    extern MENU_PC_STRING_ENTRY MenuPcStringTable827e40[40];
    extern MENU_PC_STRING_ENTRY MenuPcStringTable827f80[40];
    void PCAnimationEffectCheckpoint(void);
    void NuPadSetMotorsPc(NUPAD_INIT_STATE *, i32, i32);
#ifdef __cplusplus
}
#endif

#endif
