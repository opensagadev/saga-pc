#include "saga.h"

u8 __thiscall PC_DEVICE_BACKEND_PC::SupportsMultisamplePc(i32 index) {
    return multisample_tested != 0 && quality[index] != -1;
}
u8 __thiscall PC_DEVICE_BACKEND_PC::HasRequestedModeChangedPc() {
    return requested_windowed != (parameters.windowed == 1) || requested_mode != active_mode ||
           requested_feature != feature634 || requested_multisample != multisample_index ||
           requested_target_count != target_count;
}
void __thiscall PC_WINDOW_OPTIONS_PC::SetAspectModePc(i32 value) {
    aspect_mode = value;
}
void __thiscall PC_WINDOW_OPTIONS_PC::SetShadowsEnabledPc(u8 value) {
    shadowsenabled = value;
}
void __thiscall PC_WINDOW_OPTIONS_PC::SetBloomEnabledPc(u8 value) {
    bloomenabled = value;
}
void __thiscall PC_WINDOW_OPTIONS_PC::SetUseHiresPendingPc(u8 value) {
    usehirespending = value;
}

u8 __thiscall MENU_VIDEO_OPTIONS_PC::ChangedPc() {
    return aspect_mode != PcWindowOptions.aspect_mode || (shadows_enabled == 1) != PcWindowOptions.shadowsenabled ||
           (bloom_enabled == 1) != PcWindowOptions.bloomenabled ||
           (speed_blur_enabled == 1) != PcWindowOptions.speedblurenabled ||
           (hires_enabled == 1) != PcWindowOptions.usehirespending;
}
void __thiscall MENU_VIDEO_OPTIONS_PC::ApplyPc() {
    PcWindowOptions.SetAspectModePc(aspect_mode);
    PcWindowOptions.Save();
    i32 aspect = PcWindowOptions.aspect_mode;
    if (aspect == 3)
        aspect = PCDeviceBackend.aspect_mode;
    MenuAppliedAspectPc = aspect;
    PcWindowOptions.SetShadowsEnabledPc(shadows_enabled == 1);
    PcWindowOptions.SetBloomEnabledPc(bloom_enabled == 1);
    PcWindowOptions.SetSpeedBlurEnabled(speed_blur_enabled == 1);
    PcWindowOptions.SetUseHiresPendingPc(hires_enabled == 1);
    PcWindowOptions.Save();
}
extern "C" u8 __cdecl MenuCanChangeMultisamplePc(void) {
    for (i32 i = 0; i != 5; ++i) {
        if (i != 0 && PCDeviceBackend.multisample_tested != 0 && PCDeviceBackend.quality[i] != -1)
            return PCDeviceBackend.multisample_index != 6;
    }
    return 0;
}
extern "C" u8 __cdecl MenuCanChangeShadowsPc(void) {
    return PCDeviceBackend.requested_multisample == 0 || PcWindowOptions.shadowmethod == 1;
}
extern "C" void __cdecl MenuEnterVideoPc(MENU *) {
    MenuVideoOptionsPc.aspect_mode = PcWindowOptions.aspect_mode;
    MenuVideoOptionsPc.shadows_enabled = PcWindowOptions.shadowsenabled != 0;
    MenuVideoOptionsPc.bloom_enabled = PcWindowOptions.bloomenabled != 0;
    MenuVideoOptionsPc.speed_blur_enabled = PcWindowOptions.speedblurenabled != 0;
    MenuVideoOptionsPc.hires_enabled = PcWindowOptions.usehirespending != 0;
}
extern "C" void __cdecl MenuEnterEffectsPc(MENU *) {
    MenuVideoOptionsPc.aspect_mode = PcWindowOptions.aspect_mode;
    MenuVideoOptionsPc.shadows_enabled = PcWindowOptions.shadowsenabled != 0;
    MenuVideoOptionsPc.bloom_enabled = PcWindowOptions.bloomenabled != 0;
    MenuVideoOptionsPc.speed_blur_enabled = PcWindowOptions.speedblurenabled != 0;
    MenuVideoOptionsPc.hires_enabled = PcWindowOptions.usehirespending != 0;
}
extern "C" void __cdecl MenuExitVideoPc(MENU *) {
}
extern "C" void __cdecl MenuExitEffectsPc(MENU *) {
}
extern "C" void __cdecl MenuApplyVideoPc(MENU *) {
    if (PCDeviceBackend.HasRequestedModeChangedPc()) {
        MenuSFX = MENUSFX_MENUSELECT;
        MenuVideoOptionsPc.ApplyPc();
        PCDeviceBackend.CreateOrResetDevice(1);
        BackupMenu();
        NewMenu(1027, 0, -1);
        return;
    }
    if (MenuVideoOptionsPc.ChangedPc()) {
        MenuSFX = MENUSFX_MENUSELECT;
        MenuVideoOptionsPc.ApplyPc();
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
extern "C" void __cdecl MenuCancelVideoPc(MENU *) {
    PCDeviceBackend.ResetRequestedMode();
    MenuVideoOptionsPc.aspect_mode = PcWindowOptions.aspect_mode;
    MenuVideoOptionsPc.shadows_enabled = PcWindowOptions.shadowsenabled != 0;
    MenuVideoOptionsPc.bloom_enabled = PcWindowOptions.bloomenabled != 0;
    MenuVideoOptionsPc.speed_blur_enabled = PcWindowOptions.speedblurenabled != 0;
    MenuVideoOptionsPc.hires_enabled = PcWindowOptions.usehirespending != 0;
    MenuSFX = MENUSFX_MENUBACK;
    BackupMenu();
}
extern "C" void __cdecl MenuUpdateVideoPc(MENU *menu) {
    if (menu->cancel_pressed) {
        MenuCancelVideoPc(menu);
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
                if (++MenuVideoOptionsPc.aspect_mode > 3)
                    MenuVideoOptionsPc.aspect_mode = 0;
            } else if (menu->left_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                if (--MenuVideoOptionsPc.aspect_mode < 0)
                    MenuVideoOptionsPc.aspect_mode = 3;
            }
            break;
        case 3:
            if (menu->right_pressed || menu->left_pressed || menu->confirm_pressed) {
                if (!PcWindowOptions.option459)
                    MenuSFX = MENUSFX_MENUNOENTRY;
                else {
                    MenuSFX = MENUSFX_MENUMOVE;
                    MenuVideoOptionsPc.hires_enabled = 1 - MenuVideoOptionsPc.hires_enabled;
                }
            }
            break;
        case 4:
            if (menu->confirm_pressed)
                MenuApplyVideoPc(menu);
            break;
    }
}
extern "C" void __cdecl MenuUpdateEffectsPc(MENU *menu) {
    if (menu->cancel_pressed) {
        MenuCancelVideoPc(menu);
        return;
    }
    switch (menu->selected_row) {
        case 0:
            if (menu->right_pressed || menu->left_pressed) {
                if (PcWindowOptions.usehires || !MenuCanChangeMultisamplePc()) {
                    MenuSFX = MENUSFX_MENUNOENTRY;
                    return;
                }
                MenuSFX = MENUSFX_MENUMOVE;
                i32 index = PCDeviceBackend.requested_multisample;
                if (menu->left_pressed)
                    --index;
                else
                    ++index;
                if (index < 0)
                    index = 4;
                else if (index >= 5)
                    index = 0;
                if (!PCDeviceBackend.SupportsMultisamplePc(index)) {
                    do {
                        if (++index >= 5)
                            index = 0;
                    } while (!PCDeviceBackend.SupportsMultisamplePc(index));
                }
                PCDeviceBackend.requested_multisample = index;
            }
            break;
        case 1:
            if (menu->right_pressed || menu->left_pressed || menu->confirm_pressed) {
                if (!MenuCanChangeShadowsPc()) {
                    MenuSFX = MENUSFX_MENUNOENTRY;
                    return;
                }
                MenuSFX = MENUSFX_MENUMOVE;
                MenuVideoOptionsPc.shadows_enabled = 1 - MenuVideoOptionsPc.shadows_enabled;
            }
            break;
        case 2:
            if (menu->right_pressed || menu->left_pressed || menu->confirm_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                MenuVideoOptionsPc.bloom_enabled = 1 - MenuVideoOptionsPc.bloom_enabled;
            }
            break;
        case 3:
            if (menu->right_pressed || menu->left_pressed || menu->confirm_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                MenuVideoOptionsPc.speed_blur_enabled = 1 - MenuVideoOptionsPc.speed_blur_enabled;
            }
            break;
        case 4:
            if (menu->confirm_pressed)
                MenuApplyVideoPc(menu);
            break;
    }
}
extern "C" void __cdecl MenuDrawVideoPc(MENU *menu) {
    char text[256];
    GRAPHICS_DISPLAY_MODE_PC *mode = &PCDeviceBackend.modes[PCDeviceBackend.requested_mode];
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    NuSPrintf(text, MenuVideoResolutionFormatPc, MenuPcStrings[19], mode->width, mode->height);
    DrawMenuEntryEx(menu, text, (u8)MenuA);
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    NuSPrintf(text, MenuVideoRefreshFormatPc, MenuPcStrings[20], mode->refresh_rate);
    DrawMenuEntryEx(menu, text, (u8)MenuA);
    switch (MenuVideoOptionsPc.aspect_mode) {
        case 0:
            if (!MenuPcStringsInitialized)
                MenuPcStringsInit();
            NuSPrintf(text, MenuVideoAspect43FormatPc, MenuPcStrings[22]);
            break;
        case 1:
            if (!MenuPcStringsInitialized)
                MenuPcStringsInit();
            NuSPrintf(text, MenuVideoAspect169FormatPc, MenuPcStrings[22]);
            break;
        case 2:
            if (!MenuPcStringsInitialized)
                MenuPcStringsInit();
            NuSPrintf(text, MenuVideoAspect1610FormatPc, MenuPcStrings[22]);
            break;
        case 3: {
            if (!MenuPcStringsInitialized)
                MenuPcStringsInit();
            const char *automatic = MenuPcStrings[24];
            if (!MenuPcStringsInitialized)
                MenuPcStringsInit();
            NuSPrintf(text, ExtrasNameFormatPc, MenuPcStrings[22], automatic);
            break;
        }
        default:
            if (!MenuPcStringsInitialized)
                MenuPcStringsInit();
            NuSPrintf(text, MenuVideoAspectErrorFormatPc, MenuPcStrings[22]);
            break;
    }
    DrawMenuEntryEx(menu, text, (u8)MenuA);
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    const char *value = MenuPcStrings[MenuVideoOptionsPc.hires_enabled ? 15 : 16];
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    NuSPrintf(text, ExtrasNameFormatPc, MenuPcStrings[23], value);
    DrawMenuEntryEx(menu, text, (u8)((PcWindowOptions.option459 ? 255 : 64) * MenuAlpha));
    if (!PCDeviceBackend.HasRequestedModeChangedPc() && !MenuVideoOptionsPc.ChangedPc()) {
        if (!MenuPcStringsInitialized)
            MenuPcStringsInit();
        NuSPrintf(text, "%s", MenuPcStrings[10]);
        DrawMenuEntryEx(menu, text, (u8)MenuA);
    } else {
        if (!MenuPcStringsInitialized)
            MenuPcStringsInit();
        NuSPrintf(text, "%s", MenuPcStrings[8]);
        DrawMenuEntryEx(menu, text, (u8)((menu_flash ? 255 : 0) * MenuAlpha));
    }
}
extern "C" void __cdecl MenuDrawEffectsPc(MENU *menu) {
    char text[256];
    const char *value;
    if (PcWindowOptions.usehires) {
        if (!MenuPcStringsInitialized)
            MenuPcStringsInit();
        value = MenuPcStrings[PCDeviceBackend.requested_multisample == 6 ? 31 : 16];
        if (!MenuPcStringsInitialized)
            MenuPcStringsInit();
        NuSPrintf(text, ExtrasNameFormatPc, MenuPcStrings[30], value);
        DrawMenuEntryEx(menu, text, (u8)(MenuAlpha * 64.0));
    } else {
        switch (PCDeviceBackend.requested_multisample) {
            case 1:
                if (!MenuPcStringsInitialized)
                    MenuPcStringsInit();
                NuSPrintf(text, MenuEffects2xFormatPc, MenuPcStrings[30]);
                break;
            case 2:
                if (!MenuPcStringsInitialized)
                    MenuPcStringsInit();
                NuSPrintf(text, MenuEffects4xFormatPc, MenuPcStrings[30]);
                break;
            case 3:
                if (!MenuPcStringsInitialized)
                    MenuPcStringsInit();
                NuSPrintf(text, MenuEffects8xFormatPc, MenuPcStrings[30]);
                break;
            case 4:
                if (!MenuPcStringsInitialized)
                    MenuPcStringsInit();
                NuSPrintf(text, MenuEffects16xFormatPc, MenuPcStrings[30]);
                break;
            case 6:
                if (!MenuPcStringsInitialized)
                    MenuPcStringsInit();
                value = MenuPcStrings[31];
                if (!MenuPcStringsInitialized)
                    MenuPcStringsInit();
                NuSPrintf(text, ExtrasNameFormatPc, MenuPcStrings[30], value);
                break;
            default:
                if (!MenuPcStringsInitialized)
                    MenuPcStringsInit();
                value = MenuPcStrings[16];
                if (!MenuPcStringsInitialized)
                    MenuPcStringsInit();
                NuSPrintf(text, ExtrasNameFormatPc, MenuPcStrings[30], value);
                break;
        }
        DrawMenuEntryEx(menu, text, (u8)((MenuCanChangeMultisamplePc() ? 255 : 64) * MenuAlpha));
    }
    if ((PCDeviceBackend.requested_multisample == 0 || PcWindowOptions.shadowmethod == 1) &&
        MenuVideoOptionsPc.shadows_enabled) {
        if (!MenuPcStringsInitialized)
            MenuPcStringsInit();
        value = MenuPcStrings[15];
    } else {
        if (!MenuPcStringsInitialized)
            MenuPcStringsInit();
        value = MenuPcStrings[16];
    }
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    NuSPrintf(text, ExtrasNameFormatPc, MenuPcStrings[32], value);
    DrawMenuEntryEx(
        menu, text,
        (u8)((PCDeviceBackend.requested_multisample == 0 || PcWindowOptions.shadowmethod == 1 ? 255 : 64) * MenuAlpha));
    if (MenuVideoOptionsPc.bloom_enabled) {
        if (!MenuPcStringsInitialized)
            MenuPcStringsInit();
        value = MenuPcStrings[15];
    } else {
        if (!MenuPcStringsInitialized)
            MenuPcStringsInit();
        value = MenuPcStrings[16];
    }
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    NuSPrintf(text, ExtrasNameFormatPc, MenuPcStrings[33], value);
    DrawMenuEntryEx(menu, text, 255);
    if (MenuVideoOptionsPc.speed_blur_enabled) {
        if (!MenuPcStringsInitialized)
            MenuPcStringsInit();
        value = MenuPcStrings[15];
    } else {
        if (!MenuPcStringsInitialized)
            MenuPcStringsInit();
        value = MenuPcStrings[16];
    }
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    NuSPrintf(text, ExtrasNameFormatPc, MenuPcStrings[35], value);
    DrawMenuEntryEx(menu, text, 255);
    if (!PCDeviceBackend.HasRequestedModeChangedPc() && !MenuVideoOptionsPc.ChangedPc()) {
        if (!MenuPcStringsInitialized)
            MenuPcStringsInit();
        NuSPrintf(text, "%s", MenuPcStrings[10]);
        DrawMenuEntryEx(menu, text, (u8)MenuA);
    } else {
        if (!MenuPcStringsInitialized)
            MenuPcStringsInit();
        NuSPrintf(text, "%s", MenuPcStrings[8]);
        DrawMenuEntryEx(menu, text, (u8)((menu_flash ? 255 : 0) * MenuAlpha));
    }
}

void __thiscall PC_DEVICE_BACKEND_PC::RestoreSavedModePc() {
    requested_windowed = saved_windowed;
    if (saved_mode < mode_count)
        requested_mode = saved_mode;
    else
        requested_mode = active_mode;
    requested_feature = saved_feature;
    requested_target_count = saved_target_count;
    requested_multisample = saved_multisample;
    PcWindowOptions.SetAspectModePc(saved_aspect);
    PcWindowOptions.Save();
    i32 aspect = PcWindowOptions.aspect_mode;
    if (aspect == 3)
        aspect = active_aspect_mode;
    MenuAppliedAspectPc = aspect;
    CreateOrResetDevice(1);
}
extern "C" void __cdecl MenuEnterVideoConfirmPc(MENU *menu) {
    MenuVideoConfirmDurationPc = 15.0f;
    MenuVideoConfirmWaitPc = 3;
    menu->selected_row = 1;
}
extern "C" void __cdecl MenuDrawVideoConfirmPc(MENU *menu) {
    char text[256];
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    DrawMenuEntryEx(menu, (char *)MenuPcStrings[25], (u8)MenuA);
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    NuSPrintf(text, MenuVideoCountdownFormatPc, MenuPcStrings[27],
              (i32)(MenuVideoConfirmDurationPc - menu->menu_time) + 1);
    DrawMenuEntryEx(menu, text, (u8)MenuA);
}
extern "C" void __cdecl MenuUpdateVideoConfirmPc(MENU *menu) {
    if (MenuVideoConfirmWaitPc != 0) {
        --MenuVideoConfirmWaitPc;
        menu->menu_time = 0.0f;
        menu->selected_row = 1;
    }
    if (menu->selected_row == 0 && menu->confirm_pressed) {
        MenuSFX = MENUSFX_MENUBACK;
        BackupMenu();
        if (PcWindowOptions.usehirespending != PcWindowOptions.usehires)
            NewMenu(1028, 0, -1);
        return;
    }
    if ((menu->selected_row == 1 && menu->confirm_pressed) || menu->menu_time >= MenuVideoConfirmDurationPc) {
        PCDeviceBackend.RestoreSavedModePc();
        MenuVideoOptionsPc.aspect_mode = PcWindowOptions.aspect_mode;
        MenuVideoOptionsPc.shadows_enabled = PcWindowOptions.shadowsenabled != 0;
        MenuVideoOptionsPc.bloom_enabled = PcWindowOptions.bloomenabled != 0;
        MenuVideoOptionsPc.speed_blur_enabled = PcWindowOptions.speedblurenabled != 0;
        MenuVideoOptionsPc.hires_enabled = PcWindowOptions.usehirespending != 0;
        MenuSFX = MENUSFX_MENUBACK;
        BackupMenu();
    }
}
extern "C" void __cdecl MenuExitVideoConfirmPc(MENU *) {
}
extern "C" void __cdecl MenuDrawNoticePc(char *text, u32 red, u32 green, u32 blue) {
    MenuSmartTextEx(text, 0.0f, 0.15f, 1.0f, MenuTextScalePc, MenuTextScalePc, MenuTextScalePc, 0, red, green, blue,
                    1.5f, 4, NULL, 0, MenuA);
}
extern "C" void __cdecl MenuEnterHiresNoticePc(MENU *menu) {
    menu->selected_row = 0;
}
extern "C" void __cdecl MenuDrawHiresNoticePc(MENU *menu) {
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    MenuDrawNoticePc((char *)MenuPcStrings[28], 255, 255, 255);
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    DrawMenuEntryEx(menu, (char *)MenuPcStrings[13], (u8)MenuA);
}
extern "C" void __cdecl MenuUpdateHiresNoticePc(MENU *menu) {
    if (menu->confirm_pressed || menu->cancel_pressed) {
        MenuSFX = MENUSFX_MENUBACK;
        BackupMenu();
    }
}
extern "C" void __cdecl MenuExitHiresNoticePc(MENU *) {
}

extern "C" void __cdecl MenuEnterQuitPc(MENU *menu) {
    menu->selected_row = 0;
}
extern "C" void __cdecl MenuExitQuitPc(MENU *) {
}
extern "C" void __cdecl MenuDrawQuitPc(MENU *menu) {
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    DrawMenuEntryEx(menu, (char *)MenuPcStrings[3], (u8)MenuA);
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    DrawMenuEntryEx(menu, (char *)MenuPcStrings[1], (u8)MenuA);
}
extern "C" void __cdecl MenuUpdateQuitPc(MENU *menu) {
    if ((menu->selected_row == 0 && menu->confirm_pressed != 0) || menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUBACK;
        BackupMenu();
        return;
    }
    if (menu->selected_row == 1 && menu->confirm_pressed != 0)
        PcQuitToWindows();
}
