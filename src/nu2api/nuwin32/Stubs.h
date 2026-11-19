#pragma once

#include "nu2api/nucore/common.h"

struct FRAME_PAD;

typedef struct MENU_SETTINGS_STRING_ENTRY { // size 0x8
    i32 id;                                 // 0x0
    const char *text;                       // 0x4
} MENU_SETTINGS_STRING_ENTRY;

#ifdef __cplusplus
extern "C" {
#endif
    void MenuSettingsStringsInit(void);
    extern MENU_SETTINGS_STRING_ENTRY dat_00827800[40];
    extern MENU_SETTINGS_STRING_ENTRY dat_00827940[40];
    extern MENU_SETTINGS_STRING_ENTRY dat_00827a80[40];
    extern MENU_SETTINGS_STRING_ENTRY dat_00827bc0[40];
    extern MENU_SETTINGS_STRING_ENTRY dat_00827d00[40];
    extern MENU_SETTINGS_STRING_ENTRY dat_00827e40[40];
    extern MENU_SETTINGS_STRING_ENTRY dat_00827f80[40];
    void PCAnimationEffectCheckpoint(void);
    void NuPadSetDeviceMotors(struct FRAME_PAD *, i32, i32);
    extern u8 *ps2_scratch_free;
    extern u8 NuScratchBackingAccess[0x4000];
#ifdef __cplusplus
}
#endif
