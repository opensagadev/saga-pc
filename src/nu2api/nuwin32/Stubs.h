#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/menus/screens/movies.h"

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
    void NuPadSetDeviceMotors(NUPAD_INIT_STATE *, i32, i32);
    extern u8 *ps2_scratch_free;
    extern u8 NuScratchBackingAccess[0x4000];
#ifdef __cplusplus
}
#endif
