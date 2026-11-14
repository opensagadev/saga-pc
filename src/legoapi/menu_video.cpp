#include "saga.h"

u8 MENU_VIDEO_OPTIONS::ChangedPc() {
    if (aspect_mode != PcWindowOptions.aspect_mode) {
        return 1;
    }
    if ((shadows_enabled == 1) != PcWindowOptions.shadowsenabled || (bloom_enabled == 1) != PcWindowOptions.bloomenabled ||
        (speed_blur_enabled == 1) != PcWindowOptions.speedblurenabled) {
        return 1;
    }
    return (hires_enabled == 1) != PcWindowOptions.usehirespending;
}

void MENU_VIDEO_OPTIONS::fun_005091f0() {
    aspect_mode = PcWindowOptions.aspect_mode;
    shadows_enabled = PcWindowOptions.shadowsenabled != 0;
    bloom_enabled = PcWindowOptions.bloomenabled != 0;
    speed_blur_enabled = PcWindowOptions.speedblurenabled != 0;
    hires_enabled = PcWindowOptions.usehirespending != 0;
}

void MENU_VIDEO_OPTIONS::ApplyPc() {
    PcWindowOptions.SetAspectModePc(aspect_mode);
    PcWindowOptions.Save();
    i32 aspect = PcWindowOptions.aspect_mode;
    if (aspect == 3) {
        aspect = PCDeviceBackend.active_aspect_mode;
    }
    MenuAppliedAspect = aspect;
    PcWindowOptions.SetShadowsEnabledPc(shadows_enabled == 1);
    PcWindowOptions.SetBloomEnabledPc(bloom_enabled == 1);
    PcWindowOptions.SetSpeedBlurEnabled(speed_blur_enabled == 1);
    PcWindowOptions.SetUseHiresPendingPc(hires_enabled == 1);
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
    if (PCDeviceBackend.HasRequestedModeChangedPc()) {
        MenuSFX = MENUSFX_MENUSELECT;
        MenuVideoOptions.ApplyPc();
        PCDeviceBackend.CreateOrResetDevice(1);
        BackupMenu();
        NewMenu(1027, 0, -1);
        return;
    }
    if (MenuVideoOptions.ChangedPc()) {
        MenuSFX = MENUSFX_MENUSELECT;
        MenuVideoOptions.ApplyPc();
        if (PcWindowOptions.usehirespending != PcWindowOptions.usehires) {
            BackupMenu();
            NewMenu(1028, 0, -1);
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
        case 0:
            if (menu->right_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                PCDeviceBackend.NextResolution();
            } else if (menu->left_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                PCDeviceBackend.PreviousResolution();
            }
            break;
        case 1:
            if (menu->right_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                PCDeviceBackend.NextRefresh();
            } else if (menu->left_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                PCDeviceBackend.PreviousRefresh();
            }
            break;
        case 2:
            if (menu->right_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                if (++MenuVideoOptions.aspect_mode > 3) {
                    MenuVideoOptions.aspect_mode = 0;
                }
            } else if (menu->left_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                if (--MenuVideoOptions.aspect_mode < 0) {
                    MenuVideoOptions.aspect_mode = 3;
                }
            }
            break;
        case 3:
            if (menu->right_pressed || menu->left_pressed || menu->confirm_pressed) {
                if (!PcWindowOptions.option459) {
                    MenuSFX = MENUSFX_MENUNOENTRY;
                } else {
                    MenuSFX = MENUSFX_MENUMOVE;
                    MenuVideoOptions.hires_enabled = 1 - MenuVideoOptions.hires_enabled;
                }
            }
            break;
        case 4:
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
        case 0:
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
                if (!PCDeviceBackend.SupportsMultisamplePc(index)) {
                    do {
                        if (++index >= 5) {
                            index = 0;
                        }
                    } while (!PCDeviceBackend.SupportsMultisamplePc(index));
                }
                PCDeviceBackend.requested_multisample = index;
            }
            break;
        case 1:
            if (menu->right_pressed || menu->left_pressed || menu->confirm_pressed) {
                if (!MenuCanChangeShadows()) {
                    MenuSFX = MENUSFX_MENUNOENTRY;
                    return;
                }
                MenuSFX = MENUSFX_MENUMOVE;
                MenuVideoOptions.shadows_enabled = 1 - MenuVideoOptions.shadows_enabled;
            }
            break;
        case 2:
            if (menu->right_pressed || menu->left_pressed || menu->confirm_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                MenuVideoOptions.bloom_enabled = 1 - MenuVideoOptions.bloom_enabled;
            }
            break;
        case 3:
            if (menu->right_pressed || menu->left_pressed || menu->confirm_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                MenuVideoOptions.speed_blur_enabled = 1 - MenuVideoOptions.speed_blur_enabled;
            }
            break;
        case 4:
            if (menu->confirm_pressed) {
                MenuApplyVideo();
            }
            break;
    }
}

extern "C" void MenuDrawVideo(MENU *menu) {
    char text[256];
    GRAPHICS_DISPLAY_MODE *mode = &PCDeviceBackend.modes[PCDeviceBackend.requested_mode];
    if (!MenuPcStringsInitialized) {
        MenuPcStringsInit();
    }
    NuSPrintf(text, "%s: %d x %d", MenuPcStrings[19], mode->width, mode->height);
    DrawMenuEntryEx(menu, text, (u8)MenuA);
    if (!MenuPcStringsInitialized) {
        MenuPcStringsInit();
    }
    NuSPrintf(text, "%s: %dHz", MenuPcStrings[20], mode->refresh_rate);
    DrawMenuEntryEx(menu, text, (u8)MenuA);
    switch (MenuVideoOptions.aspect_mode) {
        case 0:
            if (!MenuPcStringsInitialized) {
                MenuPcStringsInit();
            }
            NuSPrintf(text, "%s: 4:3", MenuPcStrings[22]);
            break;
        case 1:
            if (!MenuPcStringsInitialized) {
                MenuPcStringsInit();
            }
            NuSPrintf(text, "%s: 16:9", MenuPcStrings[22]);
            break;
        case 2:
            if (!MenuPcStringsInitialized) {
                MenuPcStringsInit();
            }
            NuSPrintf(text, "%s: 16:10", MenuPcStrings[22]);
            break;
        case 3: {
            if (!MenuPcStringsInitialized) {
                MenuPcStringsInit();
            }
            const char *automatic = MenuPcStrings[24];
            if (!MenuPcStringsInitialized) {
                MenuPcStringsInit();
            }
            NuSPrintf(text, "%s: %s", MenuPcStrings[22], automatic);
            break;
        }
        default:
            if (!MenuPcStringsInitialized) {
                MenuPcStringsInit();
            }
            NuSPrintf(text, "%s: ERROR", MenuPcStrings[22]);
            break;
    }
    DrawMenuEntryEx(menu, text, (u8)MenuA);
    const char *value;
    if (MenuVideoOptions.hires_enabled) {
        if (!MenuPcStringsInitialized) {
            MenuPcStringsInit();
        }
        value = MenuPcStrings[15];
    } else {
        if (!MenuPcStringsInitialized) {
            MenuPcStringsInit();
        }
        value = MenuPcStrings[16];
    }
    if (!MenuPcStringsInitialized) {
        MenuPcStringsInit();
    }
    NuSPrintf(text, "%s: %s", MenuPcStrings[23], value);
    DrawMenuEntryEx(menu, text, (u8)((PcWindowOptions.option459 ? 255 : 64) * MenuAlpha));
    if (!PCDeviceBackend.HasRequestedModeChangedPc() && !MenuVideoOptions.ChangedPc()) {
        if (!MenuPcStringsInitialized) {
            MenuPcStringsInit();
        }
        NuSPrintf(text, "%s", MenuPcStrings[10]);
        DrawMenuEntryEx(menu, text, (u8)MenuA);
    } else {
        if (!MenuPcStringsInitialized) {
            MenuPcStringsInit();
        }
        NuSPrintf(text, "%s", MenuPcStrings[8]);
        DrawMenuEntryEx(menu, text, (u8)((menu_flash ? 255 : 0) * MenuAlpha));
    }
}

extern "C" void MenuDrawEffects(MENU *menu) {
    char text[256];
    const char *value;
    if (PcWindowOptions.usehires) {
        if (PCDeviceBackend.requested_multisample == 6) {
            if (!MenuPcStringsInitialized) {
                MenuPcStringsInit();
            }
            value = MenuPcStrings[31];
            if (!MenuPcStringsInitialized) {
                MenuPcStringsInit();
            }
            NuSPrintf(text, "%s: %s", MenuPcStrings[30], value);
        } else {
            if (!MenuPcStringsInitialized) {
                MenuPcStringsInit();
            }
            value = MenuPcStrings[16];
            if (!MenuPcStringsInitialized) {
                MenuPcStringsInit();
            }
            NuSPrintf(text, "%s: %s", MenuPcStrings[30], value);
        }
        DrawMenuEntryEx(menu, text, (u8)(MenuAlpha * 64.0));
    } else {
        switch (PCDeviceBackend.requested_multisample) {
            case 1:
                if (!MenuPcStringsInitialized) {
                    MenuPcStringsInit();
                }
                NuSPrintf(text, "%s: 2X", MenuPcStrings[30]);
                break;
            case 2:
                if (!MenuPcStringsInitialized) {
                    MenuPcStringsInit();
                }
                NuSPrintf(text, "%s: 4X", MenuPcStrings[30]);
                break;
            case 3:
                if (!MenuPcStringsInitialized) {
                    MenuPcStringsInit();
                }
                NuSPrintf(text, "%s: 8X", MenuPcStrings[30]);
                break;
            case 4:
                if (!MenuPcStringsInitialized) {
                    MenuPcStringsInit();
                }
                NuSPrintf(text, "%s: 16X", MenuPcStrings[30]);
                break;
            case 6:
                if (!MenuPcStringsInitialized) {
                    MenuPcStringsInit();
                }
                value = MenuPcStrings[31];
                if (!MenuPcStringsInitialized) {
                    MenuPcStringsInit();
                }
                NuSPrintf(text, "%s: %s", MenuPcStrings[30], value);
                break;
            default:
                if (!MenuPcStringsInitialized) {
                    MenuPcStringsInit();
                }
                value = MenuPcStrings[16];
                if (!MenuPcStringsInitialized) {
                    MenuPcStringsInit();
                }
                NuSPrintf(text, "%s: %s", MenuPcStrings[30], value);
                break;
        }
        DrawMenuEntryEx(menu, text, (u8)((MenuCanChangeMultisample() ? 255 : 64) * MenuAlpha));
    }
    if (MenuCanChangeShadows() && MenuVideoOptions.shadows_enabled) {
        if (!MenuPcStringsInitialized) {
            MenuPcStringsInit();
        }
        value = MenuPcStrings[15];
    } else {
        if (!MenuPcStringsInitialized) {
            MenuPcStringsInit();
        }
        value = MenuPcStrings[16];
    }
    if (!MenuPcStringsInitialized) {
        MenuPcStringsInit();
    }
    NuSPrintf(text, "%s: %s", MenuPcStrings[32], value);
    DrawMenuEntryEx(menu, text, (u8)((MenuCanChangeShadows() ? 255 : 64) * MenuAlpha));
    if (MenuVideoOptions.bloom_enabled) {
        if (!MenuPcStringsInitialized) {
            MenuPcStringsInit();
        }
        value = MenuPcStrings[15];
    } else {
        if (!MenuPcStringsInitialized) {
            MenuPcStringsInit();
        }
        value = MenuPcStrings[16];
    }
    if (!MenuPcStringsInitialized) {
        MenuPcStringsInit();
    }
    NuSPrintf(text, "%s: %s", MenuPcStrings[33], value);
    DrawMenuEntryEx(menu, text, 255);
    if (MenuVideoOptions.speed_blur_enabled) {
        if (!MenuPcStringsInitialized) {
            MenuPcStringsInit();
        }
        value = MenuPcStrings[15];
    } else {
        if (!MenuPcStringsInitialized) {
            MenuPcStringsInit();
        }
        value = MenuPcStrings[16];
    }
    if (!MenuPcStringsInitialized) {
        MenuPcStringsInit();
    }
    NuSPrintf(text, "%s: %s", MenuPcStrings[35], value);
    DrawMenuEntryEx(menu, text, 255);
    if (!PCDeviceBackend.HasRequestedModeChangedPc() && !MenuVideoOptions.ChangedPc()) {
        if (!MenuPcStringsInitialized) {
            MenuPcStringsInit();
        }
        NuSPrintf(text, "%s", MenuPcStrings[10]);
        DrawMenuEntryEx(menu, text, (u8)MenuA);
    } else {
        if (!MenuPcStringsInitialized) {
            MenuPcStringsInit();
        }
        NuSPrintf(text, "%s", MenuPcStrings[8]);
        DrawMenuEntryEx(menu, text, (u8)((menu_flash ? 255 : 0) * MenuAlpha));
    }
}

void PC_DEVICE_BACKEND_PC::RestoreSavedModePc() {
    requested_windowed = saved_windowed;
    if (saved_mode < mode_count) {
        requested_mode = saved_mode;
    } else {
        requested_mode = active_mode;
    }
    requested_feature = saved_feature;
    requested_multisample = saved_multisample;
    requested_target_count = saved_target_count;
    PcWindowOptions.SetAspectModePc(saved_aspect);
    PcWindowOptions.Save();
    i32 aspect = PcWindowOptions.aspect_mode;
    if (aspect == 3) {
        aspect = active_aspect_mode;
    }
    MenuAppliedAspect = aspect;
    CreateOrResetDevice(1);
}

extern "C" void MenuEnterVideoConfirm(MENU *menu) {
    MenuVideoConfirmDuration = 15.0f;
    MenuVideoConfirmWait = 3;
    menu->selected_row = 1;
}

extern "C" void MenuDrawVideoConfirm(MENU *menu) {
    char text[256];
    if (!MenuPcStringsInitialized) {
        MenuPcStringsInit();
    }
    DrawMenuEntryEx(menu, (char *)MenuPcStrings[25], (u8)MenuA);
    if (!MenuPcStringsInitialized) {
        MenuPcStringsInit();
    }
    NuSPrintf(text, "%s (%ds)", MenuPcStrings[27], (i32)(MenuVideoConfirmDuration - menu->menu_time) + 1);
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
            NewMenu(1028, 0, -1);
        }
        return;
    }
    if ((menu->selected_row == 1 && menu->confirm_pressed) || menu->menu_time >= MenuVideoConfirmDuration) {
        PCDeviceBackend.RestoreSavedModePc();
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

extern "C" void fun_0050f6e0(char *text, u32 red, u32 green, u32 blue) {
    MenuSmartTextEx(text, 0.0f, -0.3f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, red, green, blue, 1.5f, 4, NULL, 0, MenuA);
}

extern "C" void MenuEnterHiresNotice(MENU *menu) {
    menu->selected_row = 0;
}

extern "C" void MenuDrawHiresNotice(MENU *menu) {
    if (!MenuPcStringsInitialized) {
        MenuPcStringsInit();
    }
    MenuDrawNotice((char *)MenuPcStrings[28], 255, 255, 255);
    if (!MenuPcStringsInitialized) {
        MenuPcStringsInit();
    }
    DrawMenuEntryEx(menu, (char *)MenuPcStrings[13], (u8)MenuA);
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

/* Menu 1024 has empty enter, draw, update and exit steps on PC. */
extern "C" void fun_00509470(MENU *) {
}

extern "C" void fun_00509480(MENU *) {
}

extern "C" void fun_00509490(MENU *) {
}

extern "C" void fun_005094a0(MENU *) {
}

extern "C" void MenuDrawQuit(MENU *menu) {
    if (!MenuPcStringsInitialized) {
        MenuPcStringsInit();
    }
    DrawMenuEntryEx(menu, (char *)MenuPcStrings[3], (u8)MenuA);
    if (!MenuPcStringsInitialized) {
        MenuPcStringsInit();
    }
    DrawMenuEntryEx(menu, (char *)MenuPcStrings[1], (u8)MenuA);
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
