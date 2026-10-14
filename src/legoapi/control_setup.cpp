#include "saga.h"

i32 PC_CONTROL_SCREEN_PC::ItemAllowsBinding(PC_CONTROL_ROW_PC *row, i32 index, i32 player) {
    PC_CONTROL_ITEM_PC *item = &row->items[index];
    if ((item->flags & 0x10) == 0 && (item->kind == 0 || item->kind == 2)) {
        if (player == 0) {
            if (item->flags & 0x20)
                return 0;
        } else if (player == 1 && (item->flags & 0x40)) {
            return 0;
        }
        return 1;
    }
    return 0;
}

void PC_CONTROL_SCREEN_PC::AdvanceSelection(PC_CONTROL_ROW_PC *row) {
    for (;;) {
        PC_CONTROL_ITEM_PC *item = &row->items[row->selected];
        if (!(drop_in && item->label != 0) && !(selected_player == 0 && (item->flags & 0x20)) &&
            !(selected_player == 1 && (item->flags & 0x40)) && !(item->flags & 0x80))
            return;
        if (++row->selected >= row->count)
            row->selected = 0;
    }
}

/* UI helper names below are reconciled with the separate input API contract before integration. */
extern "C" {
    i32 __cdecl PcInputIsMouseDevicePc(i32);
    i32 __cdecl PcInputIsJoystickDevicePc(i32);
    i32 __cdecl PcInputBindingPresentPc(i32, i32, i32, i32);
    PC_INPUT_CONFIG_PC *__cdecl PcInputFindConfigPc(u8, i32, i32);
    i32(__cdecl *__cdecl PcInputGetCommandFilterPc(void))(i32, i32);
}

u8 PC_CONTROL_SCREEN_PC::PortAllowed(i32 player, i32 port, i32 allow_keyboard_sharing) {
    if (ports[player] == port)
        return 1;
    i32 keyboard_used = 0;
    for (i32 i = 0; i < player_count; ++i) {
        if (i != player) {
            if (TextInputIsKeyboardDevicePc(ports[i]))
                keyboard_used = 1;
            if (ports[i] == port)
                return 0;
        }
    }
    if (!allow_keyboard_sharing && keyboard_used && TextInputIsKeyboardDevicePc(port))
        return 0;
    return 1;
}

u8 PC_CONTROL_SCREEN_PC::HasMissingBindings(void) {
    for (i32 player = 0; player < player_count; ++player) {
        if (ports[player] < 0 || original_ports[player] < 0)
            continue;
        for (i32 row = 0; row < row_count; ++row) {
            for (i32 index = 0; index < rows[row].count; ++index) {
                PC_CONTROL_ITEM_PC *item = &rows[row].items[index];
                if (item->kind != 0 || (player == 0 && (item->flags & 0x20)) || (player == 1 && (item->flags & 0x40)) ||
                    player > 1 || (item->flags & 8))
                    continue;
                if (!PcInputBindingPresentPc(0, item->command, 1, player))
                    return 1;
            }
        }
    }
    return 0;
}

void PC_CONTROL_SCREEN_PC::RefreshItems(void) {
    i32 has_configuration = 0;
    for (i32 i = 0; i < 2; ++i) {
        has_configuration = PcInputFindConfigPc(1, ports[selected_player], i) != NULL;
        if (has_configuration)
            break;
    }
    PC_CONTROL_ROW_PC *row;
    for (i32 r = 0; r < row_count; ++r) {
        row = &rows[r];
        for (i32 i = 0; i < row->count; ++i) {
            PC_CONTROL_ITEM_PC *item = &row->items[i];
            item->flags &= 0x1f;
            if (item->kind == 4) {
                if (ports[selected_player] < 0 && (item->label == 0x28 || item->label == 0x2b))
                    item->flags |= 0x80;
                if (item->label == 0x28 && !has_configuration)
                    item->flags |= 0x80;
                if (item->label == 0x2b && !TextInputIsKeyboardDevicePc(ports[selected_player]))
                    item->flags |= 0x80;
            }
            if (drop_in && item->kind == 4 && item->label != 0)
                item->flags |= 0xe0;
            if (item->kind == 2) {
                if (!PcInputIsJoystickDevicePc(ports[0]))
                    item->flags |= 0x20;
                if (!PcInputIsJoystickDevicePc(ports[1]))
                    item->flags |= 0x40;
            }
            if (item->kind == 0) {
                item->flags = (item->flags & ~0x20) | (ports[0] < 0 ? 0x20 : 0);
                item->flags = (item->flags & ~0x40) | (ports[1] < 0 ? 0x40 : 0);
                item->flags = (item->flags & ~0x80) | (ports[selected_player] < 0 ? 0x80 : 0);
                if ((item->flags & 7) == 1)
                    item->label = 7;
                if ((item->flags & 7) == 2)
                    item->label = 8;
                if ((item->flags & 7) == 3)
                    item->label = 9;
                if ((item->flags & 7) == 4)
                    item->label = 10;
                if ((item->flags & 7) == 1 || (item->flags & 7) == 2) {
                    if (PcInputIsMouseDevicePc(ports[0]))
                        item->flags |= 0x20;
                    if (PcInputIsMouseDevicePc(ports[1]))
                        item->flags |= 0x40;
                }
                if (PcInputIsMouseDevicePc(ports[selected_player])) {
                    if ((item->flags & 7) == 3)
                        item->label = 0x29;
                    if ((item->flags & 7) == 4)
                        item->label = 0x2a;
                }
                i32(__cdecl * filter)(i32, i32) = PcInputGetCommandFilterPc();
                if (filter) {
                    if (!filter(ports[0], item->command))
                        item->flags |= 0x20;
                    if (!filter(ports[1], item->command))
                        item->flags |= 0x40;
                }
                if (item->flags & 8) {
                    if (TextInputIsKeyboardDevicePc(ports[0]))
                        item->flags |= 0x20;
                    if (TextInputIsKeyboardDevicePc(ports[1]))
                        item->flags |= 0x40;
                }
            }
            if (item->kind == 2) {
                i32 player = selected_player;
                if (!player) {
                    if (item->flags & 0x20)
                        player = 1;
                } else if (item->flags & 0x40) {
                    --player;
                }
                item->label = TextInputIsKeyboardDevicePc(ports[player]) ? 0x23 : 0x22;
            }
            if (((item->flags & 0x20) && selected_player == 0) || ((item->flags & 0x40) && selected_player != 0))
                item->flags |= 0x80;
        }
    }
    AdvanceSelection(row);
}

extern "C" {
    void __cdecl PcInputSetBlockedKeysPc(const char *);
    void __cdecl PcInputCopyBindingsPc(i32, i32, i32);
    void __cdecl PcInputResetMouseMotionPc(void);
    void __cdecl PcInputSuppressKeyOnePc(i32);
    i32 __cdecl PcInputAlternateDeadzonePc(i32);
    i32 __cdecl PcInputHashAuxiliaryBindingsPc(void);
    i32 __cdecl PcInputMixConfigurationHashPc(i32, i32);
    void __cdecl PcInputSetDeadzonePc(i32, i32);
    void __cdecl PcInputSetAlternateDeadzonePc(i32, i32);
    void __cdecl PcInputStageAuxiliaryConfigPc(i32, i32, i32);
    void __cdecl PcInputSaveConfigurationPc(void);
    i32 __cdecl PcInputResolveAuxiliaryConflictsPc(i32, i32);
    i32 __cdecl NuMtlSetCurrentRenderPlane(i32);
}

void PC_CONTROL_SCREEN_PC::Register(PC_CONTROL_CONFIG_PC *config, VARIPTR *, VARIPTR *) {
    draw_background = config->draw_background;
    play_sound = config->play_sound;
    initialized = 1;
    font = config->font;
    PcInputSetBlockedKeysPc(config->blocked_keys);
    i32 plane = NuMtlSetCurrentRenderPlane(2);
    material = NuMtlCreate(1);
    material->attribs.alpha_mode = 1;
    material->attribs.cull_mode = 2;
    material->attribs.z_mode = 3;
    NuMtlUpdate(material);
    NuMtlSetCurrentRenderPlane(plane);
    row_count = 0;
    while (config->rows[row_count]) {
        rows[row_count].items = config->rows[row_count];
        i32 count = 0;
        while (rows[row_count].items[count].label != 255) {
            rows[row_count].items[count].pulse = 0.0f;
            ++count;
        }
        rows[row_count].count = count;
        rows[row_count].selected = 0;
        ++row_count;
    }
}

i32 PC_CONTROL_SCREEN_PC::HashConfiguration(void) {
    i32 hash = PcInputHashAuxiliaryBindingsPc();
    hash = PcInputMixConfigurationHashPc(hash, (i32)(i64)keyboard_deadzone);
    for (i32 i = 0; i < 4; ++i)
        hash = PcInputMixConfigurationHashPc(hash, (i32)(i64)deadzone[i]);
    return hash;
}

void PC_CONTROL_SCREEN_PC::SaveConfiguration(void) {
    i32 changed = 0;
    for (i32 i = 0; i < player_count; ++i) {
        i32 port = ports[i];
        if (port < 0)
            continue;
        changed = 1;
        PcInputSetDeadzonePc(port, (i32)deadzone[i]);
        if (TextInputIsKeyboardDevicePc(port))
            PcInputSetAlternateDeadzonePc(port, (i32)keyboard_deadzone);
        PcInputCopyBindingsPc(port, 0, i);
        PcInputStageAuxiliaryConfigPc(port, i, i);
    }
    if (changed)
        PcInputSaveConfigurationPc();
}

i32 PC_CONTROL_SCREEN_PC::WaitForRelease(void) {
    if (release_frames == 0)
        return 1;
    PcInputResetMouseMotionPc();
    i32 kind, value;
    if (PcInputSelectPressedDevicePc(0, 0x1a, &kind, &value, 0) < 0)
        return --release_frames == 0;
    release_frames = release_reset;
    return release_reset == 0;
}

void PC_CONTROL_SCREEN_PC::Initialize(i32 count, i32 player, i32 resolve_bank, u8 is_drop_in) {
    if (!initialized || active)
        return;
    player_count = count > 0 ? count : 2;
    if (player >= 0)
        selected_player = player;
    drop_in = is_drop_in;
    if (drop_in && player_count == 1 && saved_ports[1] >= 0)
        player_count = 2;
    active = 1;
    missing_binding = 0;
    phase = 1;
    for (i32 i = 0; i < 4; ++i) {
        ports[i] = -1;
        original_ports[i] = -1;
    }
    for (i32 i = 0; i < player_count; ++i) {
        ports[i] = TextInputLinkedDevicePc(i);
        original_ports[i] = ports[i];
        if (ports[i] == -1) {
            ports[i] = saved_ports[i];
        } else {
            PcInputCopyBindingsPc(ports[i], 1, i);
            deadzone[i] = (f32)PcInputDeadzonePc(ports[i]);
            if (TextInputIsKeyboardDevicePc(ports[i]))
                keyboard_deadzone = (f32)PcInputAlternateDeadzonePc(ports[i]);
        }
    }
    config_hash = HashConfiguration();
    RefreshItems();
    for (i32 i = 0; i < 12; ++i)
        text[i].key = 0;
    PcInputSuppressKeyOnePc(1);
    if (resolve_bank >= 0)
        PcInputResolveAuxiliaryConflictsPc(resolve_bank, player_count);
    missing_binding = HasMissingBindings();
    release_reset = 3;
    release_frames = 3;
    PcInputResetMouseMotionPc();
}

extern "C" {
    extern PC_CONTROL_SCREEN_PC PcControlScreenPc;
    extern PC_CONTROL_ITEM_PC *PcControlRowsPc[2];
    extern i16 *PcControlTextIdsPc[45];
    extern char *PcControlTextPc[45];
    extern const char PcControlMissingTextPc[];
    extern i8 DropInPendingPlayerPc, DropInConfigurationPlayerPc;
    extern NUMTL *FadeMtl;
    void __cdecl ResumeGame(i32);

    void __cdecl PcControlRegisterPc(PC_CONTROL_CONFIG_PC *config, VARIPTR *buffer, VARIPTR *end) {
        if (!config->rows)
            config->rows = PcControlRowsPc;
        for (i32 i = 0; i < 45; ++i) {
            PcControlTextPc[i] = TTab[*PcControlTextIdsPc[i]];
            if (!PcControlTextPc[i])
                PcControlTextPc[i] = (char *)PcControlMissingTextPc;
        }
        PcControlScreenPc.Register(config, buffer, end);
    }

    void __cdecl PcControlInitializePc(i32 players, i32 player, i32 resolve_bank, u8 drop_in) {
        PcControlScreenPc.Initialize(players, player, resolve_bank, drop_in);
    }

    i32 __cdecl PcControlUpdatePc(FRAME_PAD_PC *first, FRAME_PAD_PC *second, f32 elapsed) {
        return PcControlScreenPc.Update(first, second, elapsed);
    }

    void __cdecl PcControlDrawPc(void) {
        PcControlScreenPc.Draw();
    }

    void __cdecl MenuEnterControlSetupPc(MENU *) {
        PcControlInitializePc(2, -1, -1, 0);
    }

    void __cdecl MenuDrawControlSetupPc(MENU *) {
        PcControlDrawPc();
    }

    void __cdecl MenuUpdateControlSetupPc(MENU *) {
        i32 result = PcControlUpdatePc((FRAME_PAD_PC *)Game_NuPad[0], (FRAME_PAD_PC *)Game_NuPad[1], FRAMETIME);
        if (DropInConfigurationPlayerPc >= 0) {
            if (result == 3) {
                DropInPendingPlayerPc = DropInConfigurationPlayerPc;
                DropInConfigurationPlayerPc = -1;
                ResumeGame(0);
            } else if (result == 2) {
                NuPadMapPlayerToPort(DropInConfigurationPlayerPc, -1);
                DropInConfigurationPlayerPc = -1;
                ResumeGame(0);
            }
        } else if (!result) {
            BackupMenu();
        }
    }

    void __cdecl MenuEnterSelectControlsPc(MENU *) {
        GameAudio_PlaySfx(0x30, NULL, 0, 0);
        PcControlInitializePc(1, -1, -1, 0);
    }

    void __cdecl MenuUpdateSelectControlsPc(MENU *) {
        i32 result = PcControlUpdatePc((FRAME_PAD_PC *)Game_NuPad[0], (FRAME_PAD_PC *)Game_NuPad[1], FRAMETIME);
        if (result == 2) {
            BackupMenu();
        } else if (result == 3) {
            BackupMenu();
            NewMenu(1, saveload_savepresent != 0, -1);
        }
    }

    void __cdecl PcControlPlaySoundPc(i32 sound) {
        switch (sound) {
            case 0:
                GameAudio_PlaySfx(0x31, NULL, 0, 0);
                break;
            case 1:
            case 2:
                GameAudio_PlaySfx(0x30, NULL, 0, 0);
                break;
            case 3:
            case 4:
            case 5:
                GameAudio_PlaySfx(0x2f, NULL, 0, 0);
                break;
            case 6:
                GameAudio_PlaySfx(0x32, NULL, 0, 0);
                break;
        }
    }

    void __cdecl LoadPermProgressDraw(f32, f32, f32 fade) {
        NuRndrRect2di(0, 0, 0x2800, 0xe20, ((i32)(fade * 255.0) << 24) | 0xffffff, FadeMtl);
    }
}

extern "C" void __cdecl MenuExitSelectControlsPc(MENU *) {
}

/* Native421720; append to ControlSetup source after API declarations. */
extern "C" {
    extern u32 PcControlButtonsPc, PcControlHeldPc[2], PcControlPressedPc[2];
    extern i32 PcControlPulseAnglePc;
    extern f32 PcControlRepeatPc[2];
    i32 __cdecl PcInputBindingAvailablePc(i32, i32, i32, i32);
    void __cdecl PcInputAssignBindingPc(i32, i32, i32, i32, i32, i32, i32, u32);
    void __cdecl PcInputClearOtherBankConflictsPc(i32, i32);
    i32 __cdecl PcInputCapturedPortPc(void);
    void __cdecl PcInputSetCapturedPortPc(i32);
    void __cdecl PcInputInstallConfigPc(PC_INPUT_CONFIG_PC *, i32, i32, i32, i32);
}

i32 PC_CONTROL_SCREEN_PC::Update(FRAME_PAD_PC *first, FRAME_PAD_PC *second, f32 dt) {
    i32 device = TextInputLastDevicePc();
    if (device < 0)
        device = 0;
    elapsed = dt;
    active = 1;
    switch (phase) {
        case 0:
            active = 0;
            return 0;
        case 1:
            fade = (f32)(fade + dt);
            if (fade > 1.0f) {
                fade = 1.0f;
                phase = 2;
            }
            opacity = fade;
            break;
        case 4:
            phase = 3;
            return 4;
        case 3:
            fade = (f32)(fade - dt);
            if (fade < 0.0f) {
                PcInputSuppressKeyOnePc(0);
                fade = 0.0f;
                phase = 0;
                active = 0;
                for (i32 i = 0; i < player_count; ++i)
                    saved_ports[i] = ports[i];
                if (result == 3) {
                    if (config_hash != (u32)HashConfiguration())
                        SaveConfiguration();
                    for (i32 i = 0; i < player_count; ++i)
                        if (original_ports[i] >= 0)
                            NuPadMapPlayerToPort(i, ports[i]);
                }
                return result;
            }
            opacity = fade;
            break;
        case 2: {
            FRAME_PAD_PC *pads[2] = {first, second};
            i32 escape = NuKeyboard(1);
            for (i32 i = 0; i < player_count; ++i) {
                PcControlPressedPc[i] = 0;
                if (!pads[i])
                    continue;
                u32 buttons = pads[i]->digital_buttons;
                if (pads[i]->analog_left_y > 0xe0)
                    buttons |= 0x4000;
                if (pads[i]->analog_left_y < 0x20)
                    buttons |= 0x1000;
                if (pads[i]->analog_left_x > 0xe0)
                    buttons |= 0x2000;
                if (pads[i]->analog_left_x < 0x20)
                    buttons |= 0x8000;
                if (buttons == PcControlHeldPc[i] && buttons) {
                    PcControlRepeatPc[i] = (f32)(PcControlRepeatPc[i] - elapsed);
                    if (PcControlRepeatPc[i] <= 0.0f) {
                        PcControlPressedPc[i] = PcControlHeldPc[i];
                        PcControlRepeatPc[i] = (f32)(PcControlRepeatPc[i] + 0.10000000149011612);
                    }
                } else {
                    PcControlHeldPc[i] = buttons;
                    PcControlRepeatPc[i] = 0.5f;
                    PcControlPressedPc[i] = buttons;
                }
            }
            PcControlPulseAnglePc += (i32)(elapsed * 65535.0);
            unknown10 = (f32)((f32)(NuTrigTable[(PcControlPulseAnglePc >> 1) & 0x7fff] + 1.0) * 0.5);
            PcControlButtonsPc = PcControlPressedPc[device];
            if (!WaitForRelease())
                return active;
            PC_CONTROL_ROW_PC *row = &rows[selected_row];
            switch (capture_state) {
                case 2:
                    if (escape) {
                        if (play_sound)
                            play_sound(0);
                        capture_state = 0;
                        release_reset = release_frames = 3;
                        PcInputResetMouseMotionPc();
                    } else {
                        i32 kind, value;
                        i32 port = PcInputSelectPressedDevicePc(0, 0x1a, &kind, &value, 1);
                        port = PcInputMapKeyboardPortPc(port, selected_player);
                        if (port >= 0) {
                            release_reset = release_frames = 3;
                            PcInputResetMouseMotionPc();
                            if (PortAllowed(selected_player, port, 1)) {
                                if (play_sound)
                                    play_sound(2);
                                ports[selected_player] = port;
                                PcInputSelectConfigurationPc(port, selected_player, selected_player);
                                if (TextInputIsKeyboardDevicePc(port))
                                    PcInputResolveAuxiliaryConflictsPc(selected_player, player_count);
                                capture_state = 0;
                                if (!HasMissingBindings())
                                    missing_binding = 0;
                                deadzone[selected_player] = (f32)PcInputDeadzonePc(port);
                                if (TextInputIsKeyboardDevicePc(port))
                                    keyboard_deadzone = (f32)PcInputAlternateDeadzonePc(port);
                                RefreshItems();
                            } else if (play_sound) {
                                play_sound(6);
                            }
                        }
                    }
                    break;
                case 1:
                    if (escape) {
                        if (play_sound)
                            play_sound(0);
                        capture_state = 0;
                        release_reset = release_frames = 3;
                        PcInputResetMouseMotionPc();
                    } else {
                        i32 kind, value;
                        i32 port = PcInputSelectPressedDevicePc(0, 0x13, &kind, &value, 1);
                        port = PcInputMapKeyboardPortPc(port, selected_player);
                        if (port >= 0 && port == ports[selected_player]) {
                            if (PcInputBindingAvailablePc(selected_player, player_count, kind, value)) {
                                PcInputAssignBindingPc(row->items[row->selected].command, port, kind, value, 1, 1,
                                                       selected_player, 0);
                                PcInputResolveAuxiliaryConflictsPc(selected_player, player_count);
                                PcInputClearOtherBankConflictsPc(selected_player, player_count);
                            }
                            capture_state = 0;
                            release_reset = release_frames = 3;
                            PcInputResetMouseMotionPc();
                            if (!HasMissingBindings())
                                missing_binding = 0;
                        }
                    }
                    break;
                case 4:
                    if (PcControlButtonsPc & 0x3000) {
                        keyboard_deadzone = (f32)(keyboard_deadzone + 1.0);
                        i32 sound = 5;
                        if (keyboard_deadzone > 10.0f) {
                            keyboard_deadzone = 10.0f;
                            sound = 6;
                        }
                        if (play_sound)
                            play_sound(sound);
                    }
                    if (PcControlButtonsPc & 0xc000) {
                        keyboard_deadzone = (f32)(keyboard_deadzone - 1.0);
                        i32 sound = 5;
                        if (keyboard_deadzone < 1.0f) {
                            keyboard_deadzone = 1.0f;
                            sound = 6;
                        }
                        if (play_sound)
                            play_sound(sound);
                    }
                    if (escape) {
                        if (play_sound)
                            play_sound(0);
                        keyboard_deadzone = saved_keyboard_deadzone;
                        capture_state = 0;
                        release_reset = release_frames = 3;
                        PcInputResetMouseMotionPc();
                    }
                    if (PcControlButtonsPc & 0x40) {
                        if (play_sound)
                            play_sound(1);
                        capture_state = 0;
                        release_reset = release_frames = 3;
                        PcInputResetMouseMotionPc();
                    }
                    break;
                case 3:
                    if (PcControlButtonsPc & 0x3000) {
                        deadzone[selected_player] = (f32)(deadzone[selected_player] + 5.0);
                        i32 sound = 4;
                        if (deadzone[selected_player] > 100.0) {
                            deadzone[selected_player] = 100.0f;
                            sound = 6;
                        }
                        if (play_sound)
                            play_sound(sound);
                    }
                    if (PcControlButtonsPc & 0xc000) {
                        deadzone[selected_player] = (f32)(deadzone[selected_player] - 5.0);
                        i32 sound = 4;
                        if (deadzone[selected_player] < 16.0f) {
                            deadzone[selected_player] = 16.0f;
                            sound = 6;
                        }
                        if (play_sound)
                            play_sound(sound);
                    }
                    if (escape || (PcControlButtonsPc & 0x20)) {
                        if (play_sound)
                            play_sound(0);
                        deadzone[selected_player] = saved_deadzone[selected_player];
                        capture_state = 0;
                        release_reset = release_frames = 3;
                        PcInputResetMouseMotionPc();
                    }
                    if (PcControlButtonsPc & 0x40) {
                        if (play_sound)
                            play_sound(1);
                        capture_state = 0;
                        release_reset = release_frames = 3;
                        PcInputResetMouseMotionPc();
                    }
                    break;
                case 0: {
                    if (!drop_in) {
                        if (PcControlButtonsPc & 0x2000) {
                            if (play_sound)
                                play_sound(3);
                            if (++selected_player >= player_count)
                                selected_player = 0;
                            RefreshItems();
                            AdvanceSelection(row);
                        }
                        if (PcControlButtonsPc & 0x8000) {
                            if (play_sound)
                                play_sound(3);
                            if (--selected_player < 0)
                                selected_player = player_count - 1;
                            RefreshItems();
                            AdvanceSelection(row);
                        }
                        if (PcControlButtonsPc & 0x4000) {
                            if (play_sound)
                                play_sound(3);
                            if (++row->selected >= row->count)
                                row->selected = 0;
                            AdvanceSelection(row);
                        }
                        if (PcControlButtonsPc & 0x1000) {
                            if (play_sound)
                                play_sound(3);
                            do {
                                if (--row->selected < 0)
                                    row->selected = row->count - 1;
                            } while ((selected_player == 0 && (row->items[row->selected].flags & 0x20)) ||
                                     (selected_player == 1 && (row->items[row->selected].flags & 0x40)) ||
                                     (row->items[row->selected].flags & 0x80));
                        }
                    }
                    if ((PcControlButtonsPc & 0x20) || escape) {
                        if (play_sound)
                            play_sound(0);
                        phase = 3;
                        result = 2;
                    } else if (PcControlButtonsPc & 0x40) {
                        PC_CONTROL_ITEM_PC *item = &row->items[row->selected];
                        if (item->kind == 4) {
                            if (play_sound)
                                play_sound(2);
                            switch (item->label) {
                                case 0:
                                    if (ports[selected_player] == -1) {
                                        if (play_sound)
                                            play_sound(1);
                                        phase = 3;
                                    }
                                    missing_binding = HasMissingBindings();
                                    if (!missing_binding) {
                                        result = 3;
                                        phase = 3;
                                        if (play_sound)
                                            play_sound(1);
                                    } else if (play_sound) {
                                        play_sound(6);
                                    }
                                    break;
                                case 1:
                                    if (play_sound)
                                        play_sound(0);
                                    phase = 3;
                                    result = 2;
                                    break;
                                case 3:
                                    release_reset = release_frames = 3;
                                    PcInputResetMouseMotionPc();
                                    capture_state = 2;
                                    break;
                                case 40: {
                                    i32 key = selected_player;
                                    for (i32 attempt = 0; attempt < 2; ++attempt) {
                                        PC_INPUT_CONFIG_PC *config =
                                            PcInputFindConfigPc(1, ports[selected_player], key);
                                        if (config) {
                                            PcInputInstallConfigPc(config, ports[selected_player], selected_player, 1,
                                                                   selected_player);
                                            PcInputResolveAuxiliaryConflictsPc(selected_player, player_count);
                                            deadzone[selected_player] = (f32)PcInputDeadzonePc(ports[selected_player]);
                                            break;
                                        }
                                        if (++key == 2)
                                            key = 0;
                                    }
                                    break;
                                }
                                case 43: {
                                    i32 player = PcInputCapturedPortPc();
                                    if (player >= 0 && PcInputIsMouseDevicePc(ports[player])) {
                                        ++ports[player];
                                        PcInputSelectConfigurationPc(ports[player], player, player);
                                    }
                                    ++player;
                                    if (player >= player_count) {
                                        player = -1;
                                    } else {
                                        i32 keyboard = TextInputIsKeyboardDevicePc(ports[player]);
                                        if (!keyboard) {
                                            ++player;
                                            if (player >= player_count)
                                                player = -1;
                                            else
                                                keyboard = TextInputIsKeyboardDevicePc(ports[player]);
                                        }
                                        if (player >= 0 && keyboard) {
                                            if (!PcInputIsMouseDevicePc(ports[player]))
                                                --ports[player];
                                            PcInputSelectConfigurationPc(ports[player], player, player);
                                        }
                                    }
                                    PcInputSetCapturedPortPc(player);
                                    RefreshItems();
                                    break;
                                }
                            }
                        } else if (item->kind == 2 || item->kind == 0) {
                            if (!ItemAllowsBinding(row, row->selected, selected_player)) {
                                if (play_sound)
                                    play_sound(6);
                            } else {
                                if (play_sound)
                                    play_sound(2);
                                release_reset = release_frames = 3;
                                PcInputResetMouseMotionPc();
                                if (item->kind == 2) {
                                    if (TextInputIsKeyboardDevicePc(ports[selected_player])) {
                                        capture_state = 4;
                                        saved_keyboard_deadzone = keyboard_deadzone;
                                    } else {
                                        saved_deadzone[selected_player] = deadzone[selected_player];
                                        capture_state = 3;
                                    }
                                } else {
                                    capture_state = 1;
                                }
                            }
                        }
                    }
                    break;
                }
            }
            break;
        }
    }
    for (i32 r = 0; r < row_count; ++r) {
        for (i32 i = 0; i < rows[r].count; ++i) {
            PC_CONTROL_ITEM_PC *item = &rows[r].items[i];
            if (r == selected_row && i == rows[r].selected) {
                item->pulse = (f32)(item->pulse + elapsed * 8.0);
                if (item->pulse > 1.0f)
                    item->pulse = 1.0f;
            } else if (item->pulse > 0.0f) {
                item->pulse = (f32)(item->pulse - elapsed * 8.0);
                if (item->pulse < 0.0f)
                    item->pulse = 0.0f;
            }
        }
    }
    return active;
}

// Private source; sole integration owner installs.
extern char *PcControlTextPc[45];
extern f32 PcControlBaselineAdjustPc;
struct PC_CONTROL_VERTEX_PC {
    f32 x, y, z;
    u32 colour;
    f32 u, v;
};
STATIC_ASSERT(offsetof(PC_CONTROL_VERTEX_PC, colour) == 0xc);
STATIC_ASSERT(sizeof(PC_CONTROL_VERTEX_PC) == 0x18);
extern "C" i32 __cdecl PcControlWrapTextPc(char *, f32);
extern "C" void __cdecl PcControlFontScalePc(VUFNT *, f32, f32, f32 *, f32 *);

static void PcControlPrimColourPc(u32 colour) {
    u32 converted;
    if (NuPrimNeedsOverbrightening) {
        converted = ((colour >> 16) & 0xff) + ((colour & 0xff) << 16) + (colour & 0xff000000) + (colour & 0xff00);
    } else {
        converted =
            ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000);
    }
    ((PC_CONTROL_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour = converted;
}
i32 PC_CONTROL_SCREEN_PC::CountPlayerRows(PC_CONTROL_ROW_PC *row) {
    i32 result = 0;
    for (i32 i = 0; i < row->count; ++i) {
        PC_CONTROL_ITEM_PC *item = &row->items[i];
        if ((item->kind == 0 || item->kind == 2) && (!(item->flags & 0x20) || !(item->flags & 0x40)))
            ++result;
    }
    return result;
}

u32 PC_CONTROL_SCREEN_PC::ScaleAlpha(f32 alpha, u32 colour) {
    f32 scaled = (colour >> 24) * alpha;
    if (scaled > 255.0)
        scaled = 255.0f;
    return ((u32)(i64)scaled << 24) | (colour & 0xffffff);
}

u32 PC_CONTROL_SCREEN_PC::ScaleColour(f32 brightness, f32 alpha, u32 colour) {
    f32 a = (colour >> 24) * alpha;
    f32 r = (colour & 0xff) * brightness;
    f32 g = ((colour >> 8) & 0xff) * brightness;
    f32 b = ((colour >> 16) & 0xff) * brightness;
    if (a >= 255.0)
        a = 255.0f;
    if (r > 255.0)
        r = 255.0f;
    if (g > 255.0)
        g = 255.0f;
    if (b > 255.0)
        b = 255.0f;
    return (((u32)(i32)b & 0xff) << 16) | (((u32)(i32)g & 0xff) << 8) | (((u32)(i32)a & 0xff) << 24) |
           ((u32)(i32)r & 0xff);
}

u32 PC_CONTROL_SCREEN_PC::HashString(const char *string) {
    i32 count = NuStrLen(string);
    if (!count)
        return 0;
    i32 prefix = (u32)string & 3;
    if (prefix > count)
        prefix = count;
    u32 hash = 0;
    for (i32 i = 0; i < prefix; ++i)
        hash = (hash << 8) | (i32)(i8)*string++;
    count -= prefix;
    i32 tail = count & 3;
    count -= tail;
    for (i32 i = 0; i < count; i += 4) {
        hash = (hash << 1) | (hash >> 31);
        hash ^= *(const u32 *)string;
        string += 4;
    }
    if (tail) {
        u32 value = 0;
        for (i32 i = 0; i < tail; ++i)
            value = (value << 8) | (i32)(i8)*string++;
        hash = ((hash << 1) | (hash >> 31)) ^ value;
    }
    return hash;
}

extern "C" void __cdecl PcControlFontScalePc(VUFNT *font, f32 width, f32 height, f32 *sx, f32 *sy) {
    if (!font)
        font = system_qfont;
    if (!font)
        return;
    u16 glyph = NuQFntEncodeUnicodeChar(font, 'M');
    if (glyph != 0xffff) {
        *sx = (f32)(width * 7.0 / font->glyphs[glyph].width);
        *sy = (f32)(height * 18.0 / font->height);
    } else {
        f32 scale = (f32)(height * 18.0 / font->height);
        *sy = scale;
        *sx = scale / height;
    }
}

void PC_CONTROL_SCREEN_PC::QueueText(char *string, f32 x, f32 y, f32 z, i32 alignment, f32 sx, f32 sy, u32 colour,
                                     f32 brightness, f32 wrap, u32 key) {
    char decoded[0x400];
    if (!string || !*string)
        return;
    TextDecode(string, (u8 *)decoded);
    if (!key) {
        key = PcInputMixConfigurationHashPc(HashString(decoded), *(i32 *)&x);
        key = PcInputMixConfigurationHashPc(key, *(i32 *)&y);
    }
    i32 index;
    for (index = 0; index < 12; ++index)
        if (text[index].key == key)
            break;
    if (index == 12) {
        for (index = 0; index < 12; ++index)
            if (!text[index].key)
                break;
        if (index == 12)
            return;
        text[index].fade = 0;
        text[index].key = key;
    }
    PC_CONTROL_TEXT_PC *entry = &text[index];
    NuStrCpy(entry->text, decoded);
    entry->colour = ScaleColour(brightness, 1.0f, colour);
    entry->x = x;
    entry->y = y;
    entry->z = z;
    entry->alignment = (i8)alignment;
    entry->refreshed = 1;
    entry->x_scale = sx;
    entry->y_scale = sy;
    entry->wrap_width = wrap;
}

f32 PC_CONTROL_SCREEN_PC::DrawText(char *string, f32 x, f32 y, f32 z, i32 alignment, f32 sx, f32 sy, u32 colour,
                                   i32 mode, f32 wrap) {
    NuQFntSet(font);
    f32 native_sx = x_scale * sx, native_sy = y_scale * sy;
    PcControlFontScalePc(font, native_sx, native_sy, &native_sx, &native_sy);
    NuQFntSetScale(font, native_sx, native_sy);
    NuQFntSetUVSizePc(font, native_sx, native_sy);
    f32 height = NuQFntHeight(font);
    y += NuQFntBaseline(font);
    y -= PcControlBaselineAdjustPc * height;
    if (wrap != 0.0f) {
        f32 length = NuQFntPrintLenU(font, string);
        if (wrap < length)
            native_sx = wrap * native_sx / length;
    }
    colour = ScaleAlpha(opacity, colour);
    Text3DEx(string, x, y, z, native_sx, native_sy, 1.0f, alignment, (u8)colour, (u8)(colour >> 8), (u8)(colour >> 16),
             colour >> 24);
    return height;
}

void PC_CONTROL_SCREEN_PC::DrawQueuedText(f32 delta) {
    delta += delta;
    for (i32 i = 0; i < 12; ++i) {
        PC_CONTROL_TEXT_PC *entry = &text[i];
        if (!entry->key)
            continue;
        f32 x = entry->x, y = entry->y;
        f32 sx = entry->x_scale, sy = entry->y_scale;
        i32 alignment = entry->alignment;
        if (!entry->refreshed) {
            entry->fade -= delta;
            if (entry->fade < 0.0f) {
                entry->key = 0;
                continue;
            }
        } else {
            entry->refreshed = 0;
            entry->fade += delta;
            if (entry->fade > 1.0f)
                entry->fade = 1.0f;
        }
        u32 colour = ScaleAlpha(entry->fade, entry->colour);
        if (entry->wrap_width > 0.0f) {
            NuQFntSetPointSize(font, x_scale * sx, y_scale * sy);
            f32 spacing = (f32)(NuQFntHeight(font) * 0.800000011920929);
            i32 lines = PcControlWrapTextPc(entry->text, entry->wrap_width);
            y = (f32)(y - lines * spacing * 0.5);
            char *line = entry->text;
            for (i32 j = 0; j < lines; ++j) {
                char *end = NuStrChr(line, '\n');
                if (end)
                    *end = 0;
                DrawText(line, x, y, 0.0f, alignment, sx, sy, colour, 1, 0.0f);
                if (end) {
                    *end = ' ';
                    y += spacing;
                    line = end + 1;
                }
            }
        } else
            DrawText(entry->text, x, y, 0.0f, alignment, sx, sy, colour, 1, 0.0f);
    }
}

void PC_CONTROL_SCREEN_PC::DrawDeadzone(f32 x, f32 y, f32 z, f32 width, f32 height, f32 brightness, f32 alpha,
                                        f32 deadzone_value) {
    u32 colour = ScaleAlpha(alpha * opacity, 0x40c88080);
    y = (f32)(-y - 0.05999999865889549);
    NuPrim2DBegin(13, 5, material);
    PcControlPrimColourPc(colour);
    NuPrim2DAddXYZ(x - width, y - height, z);
    PcControlPrimColourPc(colour);
    NuPrim2DAddXYZ(x - width, y + height, z);
    PcControlPrimColourPc(colour);
    NuPrim2DAddXYZ(x + width, y - height, z);
    PcControlPrimColourPc(colour);
    NuPrim2DAddXYZ(x + width, y + height, z);
    NuPrim2DEnd();
    f32 step = (f32)(width / 20.0), half_height = (f32)(height * 0.5), offset = 0.0f;
    u32 a = (u32)(i32)(alpha * 96.0) << 24;
    NuPrim2DBegin(14, 5, material);
    colour = ScaleAlpha(opacity, a | 0xc8c8c8);
    PcControlPrimColourPc(colour);
    NuPrim2DAddXYZ(x, y, z);
    PcControlPrimColourPc(colour);
    NuPrim2DAddXYZ(x, y, z);
    u32 major = ScaleAlpha(opacity, a | 0xffc864);
    u32 minor = ScaleAlpha(opacity, a | 0xff9678);
    height = (f32)(height * 0.800000011920929);
    for (i32 i = 0; i < 20; ++i) {
        f32 tick = height;
        colour = major;
        if (i & 3) {
            tick = half_height;
            colour = minor;
        }
        PcControlPrimColourPc(colour);
        NuPrim2DAddXYZ(x + offset, y + tick, z);
        PcControlPrimColourPc(colour);
        NuPrim2DAddXYZ(x + offset, y - tick, z);
        PcControlPrimColourPc(colour);
        NuPrim2DAddXYZ(x - offset, y + tick, z);
        PcControlPrimColourPc(colour);
        NuPrim2DAddXYZ(x - offset, y - tick, z);
        offset += step;
    }
    NuPrim2DEnd();
    f32 radius = (f32)(width * deadzone_value / 100.0);
    colour = ScaleColour(brightness, opacity, 0x400000c8);
    NuPrim2DBegin(13, 5, material);
    PcControlPrimColourPc(colour);
    NuPrim2DAddXYZ(x - radius, y - half_height, z);
    PcControlPrimColourPc(colour);
    NuPrim2DAddXYZ(x - radius, y + half_height, z);
    PcControlPrimColourPc(colour);
    NuPrim2DAddXYZ(x + radius, y - half_height, z);
    PcControlPrimColourPc(colour);
    NuPrim2DAddXYZ(x + radius, y + half_height, z);
    NuPrim2DEnd();
}

void PC_CONTROL_SCREEN_PC::Draw(void) {
    if (!active)
        return;
    f32 old_x = TextScaleX_candidate, old_y = TextScaleY_candidate;
    TextScaleX_candidate = TextScaleY_candidate = 1.0f;
    SmartTextSetFWNMode(2);
    if (draw_background)
        draw_background(0.0f, 0.0f, fade);
    f32 left = 0.0f, right = 0.0f;
    if (player_count > 1) {
        left = -0.6499999761581421f;
        right = 0.6499999761581421f;
    }
    f32 primary = selected_player == 0 ? -0.6499999761581421f : 0.6499999761581421f;
    f32 secondary = selected_player == 0 ? 0.6499999761581421f : -0.6499999761581421f;
    NuQFntPushPrintMode(2);
    NuQFntPushCoordinateSystem(3);
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(1);
    i32 port0 = ports[0], port1 = ports[1];
    f32 label_x = player_count == 1 ? 0.3f : 0.0f;
    DrawText(PcControlTextPc[2], 0.0f, 0.9f, 0.0f, 0x40, 2.0f, 2.0f, 0x80ff2080, 2, 0.0f);
    f32 pulse = (f32)(unknown10 + unknown10 * 0.5);
    u32 missing_colour = 0;
    if (missing_binding)
        missing_colour = ScaleColour(pulse, pulse, 0x800000ff);
    u32 selected_colour = ScaleColour(pulse, pulse, 0x80c0c0c0);
    u32 capture_colour = ScaleColour(pulse, pulse, 0x80c0c000);
    for (i32 r = 0; r < row_count; ++r) {
        PC_CONTROL_ROW_PC *row = &rows[r];
        i32 previous_kind = -1;
        u32 player0_colour = selected_player == 0 ? 0x80a0a0a0 : 0x80303030;
        u32 player1_colour = selected_player == 0 ? 0x80303030 : 0x80a0a0a0;
        if (drop_in)
            player0_colour = player1_colour = 0x80a0a0a0;
        if (capture_state == 2) {
            if (selected_player == 0)
                player0_colour = capture_colour;
            else
                player1_colour = capture_colour;
        }
        u32 heading0 = 0x80808020, heading1 = 0x80808020;
        if (!drop_in) {
            if (selected_player == 0)
                heading0 = ScaleColour(pulse, pulse, heading0);
            else
                heading1 = ScaleColour(pulse, pulse, heading1);
        }
        f32 sx, sy;
        PcControlFontScalePc(font, x_scale, y_scale, &sx, &sy);
        NuQFntSetScale(font, sx, sy);
        NuQFntSetUVSizePc(font, sx, sy);
        f32 line_height = NuQFntHeight(font);
        f32 y = (f32)(0.05999999865889549 - CountPlayerRows(row) * line_height * 0.5);
        f32 device_y = (f32)(y - line_height * 1.100000023841858);
        if (port0 >= 0 || port1 >= 0) {
            if (port0 >= 0)
                QueueText(PcInputDeviceNamePc(port0), left, device_y, 0.0f, 0x40, 1.4f, 0.8f, 0x80208020, 1.0f, 0.6f,
                          0);
            if (port1 >= 0 && player_count > 1)
                QueueText(PcInputDeviceNamePc(port1), label_x + 0.6499999761581421f, device_y, 0.0f, 0x40, 1.4f, 0.8f,
                          0x80208020, 1.0f, 0.6f, 0);
            device_y -= line_height * 2.0f;
        }
        if (player_count > 1) {
            DrawText(PcControlTextPc[23], left, device_y, 0.0f, 0x40, 2.0f, 1.5f, heading0, 2, 0.0f);
            DrawText(PcControlTextPc[24], label_x + 0.6499999761581421f, device_y, 0.0f, 0x40, 2.0f, 1.5f, heading1, 2,
                     0.0f);
        }
        PcInputSetMouseFieldPc(1);
        i32 show_empty0 = 1, show_empty1 = 1;
        for (i32 index = 0; index < row->count; ++index) {
            PC_CONTROL_ITEM_PC *item = &row->items[index];
            if ((item->flags & 0x60) == 0x60)
                continue;
            u32 binding0 = player0_colour, binding1 = player1_colour;
            u32 label_colour = item->kind == 4 ? 0x80408040 : 0x80408080;
            i32 capturing = 0;
            f32 brightness = pulse;
            if (capture_state != 2) {
                brightness = (f32)(1.0 + item->pulse * 0.5);
                if (r == selected_row && index == row->selected) {
                    if (selected_player == 0)
                        binding0 = selected_colour;
                    else
                        binding1 = selected_colour;
                    label_colour = selected_colour;
                    if (capture_state == 1) {
                        binding0 = player0_colour;
                        binding1 = player1_colour;
                        if (selected_player == 0)
                            binding0 = capture_colour;
                        else
                            binding1 = capture_colour;
                        label_colour = capture_colour;
                        capturing = 1;
                    }
                }
            }
            if (item->kind == 2) {
                if (capture_state == 3 || capture_state == 4)
                    label_colour = capture_colour;
                for (i32 p = 0; p < player_count; ++p) {
                    i32 excluded = p == 0 ? (item->flags >> 5) & 1 : (item->flags >> 6) & 1;
                    if (ports[p] < 0 || excluded)
                        continue;
                    f32 intensity = 1.0f;
                    if (p == selected_player && r == selected_row && index == row->selected)
                        intensity = brightness;
                    if ((item->flags & 0x80) || p != selected_player)
                        intensity = (f32)(intensity * 0.4000000059604645);
                    f32 x = player_count == 2 ? (p == 0 ? left : right) : (f32)(label_x - 0.6499999761581421);
                    if (!TextInputIsKeyboardDevicePc(ports[p]))
                        DrawDeadzone(x, (f32)(y + line_height * 0.699999988079071 * brightness), 0.0f, 0.25f,
                                     (f32)(brightness * 0.019999999552965164), intensity, intensity, deadzone[p]);
                }
            }
            if (item->kind == 0) {
                if (missing_binding && !capturing) {
                    if (!PcInputBindingPresentPc(0, item->command, 1, 0) && !(item->flags & 8))
                        binding0 = missing_colour;
                    if (!PcInputBindingPresentPc(0, item->command, 1, 1) && (item->flags & 8))
                        binding1 = missing_colour;
                }
                char button[0x40];
                if (port0 >= 0) {
                    if (!(item->flags & 0x20)) {
                        PcInputBindingButtonNamePc(port0, item->command, button, 1, 0, 1, 0, 0);
                        DrawText(button, (f32)(label_x - 0.6499999761581421), y, 0.0f, 0x40, 2.5f, brightness, binding0,
                                 1, 0.45f);
                    }
                } else if (show_empty0) {
                    show_empty0 = 0;
                    DrawText(PcControlTextPc[6], (f32)(label_x - 0.6499999761581421), 0.0f, 0.0f, 0x40, 2.5f, 1.0f,
                             player0_colour, 1, 0.45f);
                }
                if (player_count == 2) {
                    if (port1 >= 0) {
                        if (!(item->flags & 0x40)) {
                            PcInputBindingButtonNamePc(port1, item->command, button, 1, 1, 1, 0, 0);
                            DrawText(button, (f32)(label_x + 0.6499999761581421), y, 0.0f, 0x40, 2.5f, brightness,
                                     binding1, 1, 0.45f);
                        }
                    } else if (show_empty1) {
                        show_empty1 = 0;
                        DrawText(PcControlTextPc[6], (f32)(label_x + 0.6499999761581421), 0.0f, 0.0f, 0x40, 2.5f, 1.0f,
                                 player1_colour, 1, 0.45f);
                    }
                }
            }
            if (previous_kind != 4 && item->kind == 4) {
                label_x = 0.0f;
                y = -0.65f;
            }
            char formatted[0x70];
            char *label = PcControlTextPc[item->label];
            f32 width = 0.7f;
            if (item->label == 43) {
                i32 player = PcInputCapturedPortPc();
                char *who = PcControlTextPc[44];
                if (player == 0)
                    who = PcControlTextPc[23];
                else if (player == 1)
                    who = PcControlTextPc[24];
                NuSPrintf(formatted, "%s (%s)", label, who);
                label = formatted;
                width = 0.6f;
            }
            if (item->flags & 0x80)
                label_colour = ScaleAlpha(0.4f, label_colour);
            y += DrawText(label, label_x, y, 0.0f, 0x40, 2.5f, brightness, label_colour, 1, width);
            previous_kind = item->kind;
        }
        PcInputSetMouseFieldPc(0);
        char *prompt_left = 0, *prompt_right = 0;
        switch (capture_state) {
            case 0:
                if (!drop_in)
                    prompt_right = PcControlTextPc[33];
                if (ports[selected_player] >= 0) {
                    PC_CONTROL_ITEM_PC *item = &row->items[row->selected];
                    if (item->kind == 0) {
                        if (ItemAllowsBinding(row, row->selected, selected_player))
                            prompt_left = PcControlTextPc[25];
                    } else if (item->kind == 2)
                        prompt_left = PcControlTextPc[32];
                    else
                        prompt_left = PcControlTextPc[26];
                }
                break;
            case 1:
                prompt_left = PcControlTextPc[4];
                break;
            case 2:
                prompt_left = PcControlTextPc[5];
                break;
            case 3:
                prompt_left = PcControlTextPc[28];
                prompt_right = PcControlTextPc[29];
                break;
            case 4:
                prompt_left = PcControlTextPc[30];
                prompt_right = PcControlTextPc[31];
                break;
        }
        if (prompt_left)
            QueueText(prompt_left, primary, -0.8f, 0.0f, 0x40, 2.0f, 1.0f, 0x80c0c0c0, 1.0f, 0.54f, 0);
        if (prompt_right)
            QueueText(prompt_right, secondary, -0.8f, 0.0f, 0x40, 2.0f, 1.0f, 0x80c0c0c0, 1.0f, 0.54f, 0);
    }
    DrawQueuedText(elapsed);
    SmartTextSetFWNMode(0);
    i32 restore = NuPrimCoordSystemStack[NuPrimCSPos - 1];
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(restore);
    NuQFntPopCoordinateSystem();
    NuQFntPopPrintMode();
    TextScaleX_candidate = old_x;
    TextScaleY_candidate = old_y;
}

extern "C" i32 __cdecl PcControlWrapTextPc(char *string, f32 max_width) {
    u8 *text = (u8 *)string;
    if (max_width <= 0.0f)
        return -1;
    i32 line_count = 1, line_start = 0, position = 0, previous_break = 0;
    u8 substring[512], decoded[512];
    u16 encoded[512];
    f32 width = 0.0f;
    for (;;) {
        i32 stop = 0;
        width = 0.0f;
        while (max_width > width && !stop) {
            previous_break = position;
            position = SplitTextFindNextWS(text, position);
            i32 length = position - line_start;
            if (length > 511)
                length = 511;
            PCByteCopy_candidate(substring, text + line_start, length);
            substring[length] = 0;
            TextDecode((char *)substring, decoded);
            Text3DStringEncodeFont(decoded, encoded, SmartTextFont ? SmartTextFont : QFont2D);
            width = NuQFntPrintLenW(QFont2D, encoded);
            while (text[position] == ' ')
                ++position;
            if (!text[position] || (text[position] == '\\' && text[position + 1] == 'n'))
                stop = 1;
        }
        if (!text[position])
            break;
        if (text[position] == '\\' && text[position + 1] == 'n') {
            text[position + 1] = ' ';
            previous_break = position + 1;
        }
        if (previous_break == line_start)
            previous_break = position;
        ++line_count;
        text[previous_break - 1] = '\n';
        line_start = previous_break;
    }
    if (max_width <= width) {
        ++line_count;
        text[previous_break - 1] = '\n';
    }
    return line_count;
}
