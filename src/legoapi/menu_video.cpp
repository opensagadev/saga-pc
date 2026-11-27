#include "gameapi/apitext.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nuwin32/Stubs.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/nu2api_windows.h"
#include "nu2api/nuwin32/pcapi.h"

u8 MENU_VIDEO_OPTIONS::Changed() {
    if (aspect_mode != PcWindowOptions.aspect_mode) {
        return 1;
    }
    if ((shadows_enabled == 1) != PcWindowOptions.shadowsenabled || (bloom_enabled == 1) != PcWindowOptions.bloomenabled ||
        (speed_blur_enabled == 1) != PcWindowOptions.speedblurenabled) {
        return 1;
    }
    return (hires_enabled == 1) != PcWindowOptions.usehirespending;
}

void MENU_VIDEO_OPTIONS::LoadFromWindowOptions() {
    aspect_mode = PcWindowOptions.aspect_mode;
    shadows_enabled = PcWindowOptions.shadowsenabled != 0;
    bloom_enabled = PcWindowOptions.bloomenabled != 0;
    speed_blur_enabled = PcWindowOptions.speedblurenabled != 0;
    hires_enabled = PcWindowOptions.usehirespending != 0;
}

void MENU_VIDEO_OPTIONS::Apply() {
    PcWindowOptions.SetAspectMode(aspect_mode);
    PcWindowOptions.Save();
    i32 aspect = PcWindowOptions.aspect_mode;
    if (aspect == PC_ASPECT_AUTO) {
        aspect = PCDeviceBackend.active_aspect_mode;
    }
    MenuAppliedAspect = aspect;
    PcWindowOptions.SetShadowsEnabled(shadows_enabled == 1);
    PcWindowOptions.SetBloomEnabled(bloom_enabled == 1);
    PcWindowOptions.SetSpeedBlurEnabled(speed_blur_enabled == 1);
    PcWindowOptions.SetUseHiresPending(hires_enabled == 1);
    PcWindowOptions.Save();
}

extern "C" bool MenuCanChangeMultisample(void) {
    for (i32 i = 0; i != 5; ++i) {
        if (i != 0 && PCDeviceBackend.multisample_tested != 0 && PCDeviceBackend.quality[i] != -1) {
            if (PCDeviceBackend.multisample_index == 6) {
                break;
            }
            return true;
        }
    }
    return false;
}

extern "C" bool MenuCanChangeShadows(void) {
    return PCDeviceBackend.requested_multisample == 0 || PcWindowOptions.shadowmethod == 1;
}

extern "C" {
    MENU_VIDEO_OPTIONS MenuVideoOptions = {0};
}

extern "C" void MenuEnterVideo(MENU *) {
    MenuVideoOptions.aspect_mode = PcWindowOptions.aspect_mode;
    MenuVideoOptions.shadows_enabled = PcWindowOptions.shadowsenabled != 0;
    MenuVideoOptions.bloom_enabled = PcWindowOptions.bloomenabled != 0;
    MenuVideoOptions.speed_blur_enabled = PcWindowOptions.speedblurenabled != 0;
    MenuVideoOptions.hires_enabled = PcWindowOptions.usehirespending != 0;
}

extern "C" bool fun_00509380(void) {
    return true;
}

extern "C" void MenuEnterEffects(MENU *) {
    MenuVideoOptions.aspect_mode = PcWindowOptions.aspect_mode;
    MenuVideoOptions.shadows_enabled = PcWindowOptions.shadowsenabled != 0;
    MenuVideoOptions.bloom_enabled = PcWindowOptions.bloomenabled != 0;
    MenuVideoOptions.speed_blur_enabled = PcWindowOptions.speedblurenabled != 0;
    MenuVideoOptions.hires_enabled = PcWindowOptions.usehirespending != 0;
}

extern "C" void MenuExitVideo(MENU *) {
}

extern "C" void MenuExitEffects(MENU *) {
}

extern "C" void MenuApplyVideo(void) {
    if (PCDeviceBackend.HasRequestedModeChanged()) {
        MenuSFX = MENUSFX_MENUSELECT;
        MenuVideoOptions.Apply();
        PCDeviceBackend.CreateOrResetDevice(1);
        BackupMenu();
        NewMenu(MENU_VIDEO_CONFIRM, 0, -1);
        return;
    }
    if (MenuVideoOptions.Changed()) {
        MenuSFX = MENUSFX_MENUSELECT;
        MenuVideoOptions.Apply();
        if (PcWindowOptions.usehirespending != PcWindowOptions.usehires) {
            BackupMenu();
            NewMenu(MENU_HIRES_NOTICE, 0, -1);
            return;
        }
    } else {
        MenuSFX = MENUSFX_MENUBACK;
    }
    BackupMenu();
}

extern "C" void MenuCancelVideo(void) {
    PCDeviceBackend.ResetRequestedMode();
    MenuVideoOptions.aspect_mode = PcWindowOptions.aspect_mode;
    MenuVideoOptions.shadows_enabled = PcWindowOptions.shadowsenabled != 0;
    MenuVideoOptions.bloom_enabled = PcWindowOptions.bloomenabled != 0;
    MenuVideoOptions.speed_blur_enabled = PcWindowOptions.speedblurenabled != 0;
    MenuVideoOptions.hires_enabled = PcWindowOptions.usehirespending != 0;
    MenuSFX = MENUSFX_MENUBACK;
    BackupMenu();
}

extern "C" void MenuUpdateVideo(MENU *menu) {
    if (menu->cancel_pressed) {
        MenuCancelVideo();
        return;
    }
    switch (menu->selected_row) {
        case MENU_VIDEO_ROW_RESOLUTION:
            if (menu->right_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                PCDeviceBackend.NextResolution();
            } else if (menu->left_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                PCDeviceBackend.PreviousResolution();
            }
            break;
        case MENU_VIDEO_ROW_REFRESH:
            if (menu->right_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                PCDeviceBackend.NextRefresh();
            } else if (menu->left_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                PCDeviceBackend.PreviousRefresh();
            }
            break;
        case MENU_VIDEO_ROW_ASPECT:
            if (menu->right_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                if (++MenuVideoOptions.aspect_mode > 3) {
                    MenuVideoOptions.aspect_mode = PC_ASPECT_4_3;
                }
            } else if (menu->left_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                if (--MenuVideoOptions.aspect_mode < 0) {
                    MenuVideoOptions.aspect_mode = PC_ASPECT_AUTO;
                }
            }
            break;
        case MENU_VIDEO_ROW_HIRES:
            if (menu->right_pressed || menu->left_pressed || menu->confirm_pressed) {
                if (!PcWindowOptions.option459) {
                    MenuSFX = MENUSFX_MENUNOENTRY;
                } else {
                    MenuSFX = MENUSFX_MENUMOVE;
                    MenuVideoOptions.hires_enabled = 1 - MenuVideoOptions.hires_enabled;
                }
            }
            break;
        case MENU_VIDEO_ROW_APPLY:
            if (menu->confirm_pressed) {
                MenuApplyVideo();
            }
            break;
    }
}

extern "C" void MenuUpdateEffects(MENU *menu) {
    if (menu->cancel_pressed) {
        MenuCancelVideo();
        return;
    }
    switch (menu->selected_row) {
        case MENU_EFFECTS_ROW_MULTISAMPLE:
            if (menu->right_pressed || menu->left_pressed) {
                if (PcWindowOptions.usehires) {
                    MenuSFX = MENUSFX_MENUNOENTRY;
                    return;
                }
                if (!MenuCanChangeMultisample()) {
                    MenuSFX = MENUSFX_MENUNOENTRY;
                    return;
                }
                MenuSFX = MENUSFX_MENUMOVE;
                i32 index = PCDeviceBackend.requested_multisample;
                if (menu->left_pressed) {
                    --index;
                } else {
                    ++index;
                }
                if (index < 0) {
                    index = 4;
                } else if (index >= 5) {
                    index = 0;
                }
                if (!PCDeviceBackend.SupportsMultisample(index)) {
                    do {
                        if (++index >= 5) {
                            index = 0;
                        }
                    } while (!PCDeviceBackend.SupportsMultisample(index));
                }
                PCDeviceBackend.requested_multisample = index;
            }
            break;
        case MENU_EFFECTS_ROW_SHADOWS:
            if (menu->right_pressed || menu->left_pressed || menu->confirm_pressed) {
                if (!MenuCanChangeShadows()) {
                    MenuSFX = MENUSFX_MENUNOENTRY;
                    return;
                }
                MenuSFX = MENUSFX_MENUMOVE;
                MenuVideoOptions.shadows_enabled = 1 - MenuVideoOptions.shadows_enabled;
            }
            break;
        case MENU_EFFECTS_ROW_BLOOM:
            if (menu->right_pressed || menu->left_pressed || menu->confirm_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                MenuVideoOptions.bloom_enabled = 1 - MenuVideoOptions.bloom_enabled;
            }
            break;
        case MENU_EFFECTS_ROW_SPEED_BLUR:
            if (menu->right_pressed || menu->left_pressed || menu->confirm_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                MenuVideoOptions.speed_blur_enabled = 1 - MenuVideoOptions.speed_blur_enabled;
            }
            break;
        case MENU_EFFECTS_ROW_APPLY:
            if (menu->confirm_pressed) {
                MenuApplyVideo();
            }
            break;
    }
}

extern "C" void MenuDrawVideo(MENU *menu) {
    char text[256];
    GRAPHICS_DISPLAY_MODE *mode = &PCDeviceBackend.modes[PCDeviceBackend.requested_mode];
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    NuSPrintf(text, "%s: %d x %d", MenuSettingsStrings[19], mode->width, mode->height);
    DrawMenuEntryEx(menu, text, (u8)MenuA);
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    NuSPrintf(text, "%s: %dHz", MenuSettingsStrings[20], mode->refresh_rate);
    DrawMenuEntryEx(menu, text, (u8)MenuA);
    switch (MenuVideoOptions.aspect_mode) {
        case PC_ASPECT_4_3:
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            NuSPrintf(text, "%s: 4:3", MenuSettingsStrings[22]);
            break;
        case PC_ASPECT_16_9:
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            NuSPrintf(text, "%s: 16:9", MenuSettingsStrings[22]);
            break;
        case PC_ASPECT_16_10:
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            NuSPrintf(text, "%s: 16:10", MenuSettingsStrings[22]);
            break;
        case PC_ASPECT_AUTO: {
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            const char *automatic = MenuSettingsStrings[24];
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            NuSPrintf(text, "%s: %s", MenuSettingsStrings[22], automatic);
            break;
        }
        default:
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            NuSPrintf(text, "%s: ERROR", MenuSettingsStrings[22]);
            break;
    }
    DrawMenuEntryEx(menu, text, (u8)MenuA);
    const char *value;
    if (MenuVideoOptions.hires_enabled) {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        value = MenuSettingsStrings[15];
    } else {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        value = MenuSettingsStrings[16];
    }
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    NuSPrintf(text, "%s: %s", MenuSettingsStrings[23], value);
    DrawMenuEntryEx(menu, text, (u8)((PcWindowOptions.option459 ? 255 : 64) * MenuAlpha));
    if (!PCDeviceBackend.HasRequestedModeChanged() && !MenuVideoOptions.Changed()) {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        NuSPrintf(text, "%s", MenuSettingsStrings[10]);
        DrawMenuEntryEx(menu, text, (u8)MenuA);
    } else {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        NuSPrintf(text, "%s", MenuSettingsStrings[8]);
        DrawMenuEntryEx(menu, text, (u8)((menu_flash ? 255 : 0) * MenuAlpha));
    }
}

extern "C" void MenuDrawEffects(MENU *menu) {
    char text[256];
    const char *value;
    if (PcWindowOptions.usehires) {
        if (PCDeviceBackend.requested_multisample == 6) {
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            value = MenuSettingsStrings[31];
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            NuSPrintf(text, "%s: %s", MenuSettingsStrings[30], value);
        } else {
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            value = MenuSettingsStrings[16];
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            NuSPrintf(text, "%s: %s", MenuSettingsStrings[30], value);
        }
        DrawMenuEntryEx(menu, text, (u8)(MenuAlpha * 64.0));
    } else {
        switch (PCDeviceBackend.requested_multisample) {
            case 1:
                if (!MenuSettingsStringsInitialized) {
                    MenuSettingsStringsInit();
                }
                NuSPrintf(text, "%s: 2X", MenuSettingsStrings[30]);
                break;
            case 2:
                if (!MenuSettingsStringsInitialized) {
                    MenuSettingsStringsInit();
                }
                NuSPrintf(text, "%s: 4X", MenuSettingsStrings[30]);
                break;
            case 3:
                if (!MenuSettingsStringsInitialized) {
                    MenuSettingsStringsInit();
                }
                NuSPrintf(text, "%s: 8X", MenuSettingsStrings[30]);
                break;
            case 4:
                if (!MenuSettingsStringsInitialized) {
                    MenuSettingsStringsInit();
                }
                NuSPrintf(text, "%s: 16X", MenuSettingsStrings[30]);
                break;
            case 6:
                if (!MenuSettingsStringsInitialized) {
                    MenuSettingsStringsInit();
                }
                value = MenuSettingsStrings[31];
                if (!MenuSettingsStringsInitialized) {
                    MenuSettingsStringsInit();
                }
                NuSPrintf(text, "%s: %s", MenuSettingsStrings[30], value);
                break;
            default:
                if (!MenuSettingsStringsInitialized) {
                    MenuSettingsStringsInit();
                }
                value = MenuSettingsStrings[16];
                if (!MenuSettingsStringsInitialized) {
                    MenuSettingsStringsInit();
                }
                NuSPrintf(text, "%s: %s", MenuSettingsStrings[30], value);
                break;
        }
        DrawMenuEntryEx(menu, text, (u8)((MenuCanChangeMultisample() ? 255 : 64) * MenuAlpha));
    }
    if (MenuCanChangeShadows() && MenuVideoOptions.shadows_enabled) {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        value = MenuSettingsStrings[15];
    } else {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        value = MenuSettingsStrings[16];
    }
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    NuSPrintf(text, "%s: %s", MenuSettingsStrings[32], value);
    DrawMenuEntryEx(menu, text, (u8)((MenuCanChangeShadows() ? 255 : 64) * MenuAlpha));
    if (MenuVideoOptions.bloom_enabled) {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        value = MenuSettingsStrings[15];
    } else {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        value = MenuSettingsStrings[16];
    }
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    NuSPrintf(text, "%s: %s", MenuSettingsStrings[33], value);
    DrawMenuEntryEx(menu, text, 255);
    if (MenuVideoOptions.speed_blur_enabled) {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        value = MenuSettingsStrings[15];
    } else {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        value = MenuSettingsStrings[16];
    }
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    NuSPrintf(text, "%s: %s", MenuSettingsStrings[35], value);
    DrawMenuEntryEx(menu, text, 255);
    if (!PCDeviceBackend.HasRequestedModeChanged() && !MenuVideoOptions.Changed()) {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        NuSPrintf(text, "%s", MenuSettingsStrings[10]);
        DrawMenuEntryEx(menu, text, (u8)MenuA);
    } else {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        NuSPrintf(text, "%s", MenuSettingsStrings[8]);
        DrawMenuEntryEx(menu, text, (u8)((menu_flash ? 255 : 0) * MenuAlpha));
    }
}

extern "C" {
    f32 MenuVideoConfirmDuration = 0;
    i32 MenuVideoConfirmWait = 0;
}

extern "C" void MenuEnterVideoConfirm(MENU *menu) {
    MenuVideoConfirmDuration = 15.0f;
    MenuVideoConfirmWait = 3;
    menu->selected_row = 1;
}

extern "C" void MenuDrawVideoConfirm(MENU *menu) {
    char text[256];
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    DrawMenuEntryEx(menu, (char *)MenuSettingsStrings[25], (u8)MenuA);
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    NuSPrintf(text, "%s (%ds)", MenuSettingsStrings[27], (i32)(MenuVideoConfirmDuration - menu->menu_time) + 1);
    DrawMenuEntryEx(menu, text, (u8)MenuA);
}

extern "C" void MenuUpdateVideoConfirm(MENU *menu) {
    if (MenuVideoConfirmWait != 0) {
        --MenuVideoConfirmWait;
        menu->menu_time = 0.0f;
        menu->selected_row = 1;
    }
    if (menu->selected_row == 0 && menu->confirm_pressed) {
        MenuSFX = MENUSFX_MENUBACK;
        BackupMenu();
        if (PcWindowOptions.usehirespending != PcWindowOptions.usehires) {
            NewMenu(MENU_HIRES_NOTICE, 0, -1);
        }
        return;
    }
    if ((menu->selected_row == 1 && menu->confirm_pressed) || menu->menu_time >= MenuVideoConfirmDuration) {
        PCDeviceBackend.RestoreSavedMode();
        MenuVideoOptions.aspect_mode = PcWindowOptions.aspect_mode;
        MenuVideoOptions.shadows_enabled = PcWindowOptions.shadowsenabled != 0;
        MenuVideoOptions.bloom_enabled = PcWindowOptions.bloomenabled != 0;
        MenuVideoOptions.speed_blur_enabled = PcWindowOptions.speedblurenabled != 0;
        MenuVideoOptions.hires_enabled = PcWindowOptions.usehirespending != 0;
        MenuSFX = MENUSFX_MENUBACK;
        BackupMenu();
    }
}

extern "C" void MenuExitVideoConfirm(MENU *) {
}

extern "C" void MenuDrawNotice(char *text, u32 red, u32 green, u32 blue) {
    MenuSmartTextEx(text, 0.0f, 0.15f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, red, green, blue, 1.5f, 4, NULL, 0, MenuA);
}

extern "C" void MenuDrawLowerText(char *text, u32 red, u32 green, u32 blue) {
    MenuSmartTextEx(text, 0.0f, -0.3f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, red, green, blue, 1.5f, 4, NULL, 0, MenuA);
}

extern "C" void MenuEnterHiresNotice(MENU *menu) {
    menu->selected_row = 0;
}

extern "C" void MenuDrawHiresNotice(MENU *menu) {
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    MenuDrawNotice((char *)MenuSettingsStrings[28], 255, 255, 255);
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    DrawMenuEntryEx(menu, (char *)MenuSettingsStrings[13], (u8)MenuA);
}

extern "C" void MenuUpdateHiresNotice(MENU *menu) {
    if (menu->confirm_pressed || menu->cancel_pressed) {
        MenuSFX = MENUSFX_MENUBACK;
        BackupMenu();
    }
}

extern "C" void MenuExitHiresNotice(MENU *) {
}

extern "C" void MenuEnterQuit(MENU *menu) {
    menu->selected_row = 0;
}

extern "C" void MenuExitQuit(MENU *) {
}

/* Menu 1030 has empty enter, draw, update and exit steps on PC. */
extern "C" void fun_00509470(MENU *) {
}

extern "C" void fun_00509480(MENU *) {
}

extern "C" void fun_00509490(MENU *) {
}

extern "C" void fun_005094a0(MENU *) {
}

extern "C" void MenuDrawQuit(MENU *menu) {
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    DrawMenuEntryEx(menu, (char *)MenuSettingsStrings[3], (u8)MenuA);
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    DrawMenuEntryEx(menu, (char *)MenuSettingsStrings[1], (u8)MenuA);
}

extern "C" void MenuUpdateQuit(MENU *menu) {
    if ((menu->selected_row == 0 && menu->confirm_pressed != 0) || menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUBACK;
        BackupMenu();
        return;
    }
    if (menu->selected_row == 1 && menu->confirm_pressed != 0) {
        PcQuitToWindows();
    }
}
