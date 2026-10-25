#include "saga.h"

STATIC_ASSERT(sizeof(FADETYPE) == 4);
STATIC_ASSERT(sizeof(FadeSystem) == 0x2c);
STATIC_ASSERT(sizeof(GAMEPAD_INIT_RECORD) == 0x60);
extern "C" {
    void __cdecl GameDrawMenuEntry(MENU *, char *);
    i32 __cdecl Arcade_BothPlayersActive(void);
    void __cdecl ResumeGame(i32);
    void __cdecl DropOut(i32, i32, i32, i32);
    extern i32 MenuKeyboardPendingPc;
    extern i32 PauseSaveAndExitPc;
    extern i16 TextIdPc880838, TextIdPc880850, TextIdPc880bc4, TextIdPc88090c;
    extern i16 TextIdPc88089c, TextIdPc8808ec, TextIdPc8806f8, TextIdPc880860;
    extern i32 MenuPcStringsInitialized;
    void __cdecl MenuPcStringsInit(void);
}

extern "C" void __cdecl MenuUpdatePauseMain(MENU *menu) {
    i32 aspect = PcWindowOptions.aspect_mode;
    if (aspect == 3)
        aspect = PCDeviceBackend.active_aspect_mode;
    Game.options_save.widescreen = aspect != 0;
    if (!menu->confirm_pressed)
        return;
    i32 both = Arcade_BothPlayersActive();
    if (MenuKeyboardPendingPc)
        both = 0;
    MenuSFX = GameAudio_GetSfxId(0x30);
    i32 item = menu->selected_item;
    if (item == 0) {
        ResumeGame(1);
        return;
    }
    if (item == 1) {
        NewMenu(4, -1, -1);
        return;
    }
    if (item == 2) {
        NewMenu(21, 0, -1);
        return;
    }
    if (item == 3) {
        NewMenu(1025, 0, -1);
        return;
    }
    if (item == 4) {
        NewMenu(1026, 0, -1);
        return;
    }
    i32 next = 5;
    if (!GAMEDEMO) {
        next = 6;
        if (item == 5) {
            if (SuperStory || ChallengeMode || Mission_Active(NULL))
                GameAudio_PlaySfx(0x32, NULL, 0, 0);
            else
                NewMenu(5, -1, -1);
            return;
        }
    }
    if (both) {
        i32 drop_item = next++;
        if (item == drop_item) {
            i32 player = -1;
            for (i32 i = 0; i < 8; ++i) {
                GAMEOBJECT *object = (GAMEOBJECT *)Player[i];
                if (object) {
                    if (object->pad == (GAMEPAD_PC *)&GamePadStorage[pause_i_pad])
                        player = i;
                    object->flags_ef0 &= ~0x200000u;
                }
            }
            if (player != -1)
                DropOut(player, 1, 0, 0);
            return;
        }
    }
    if (item == next)
        NewMenu(6, CanSaveAndExit(WORLD) != NULL ? 2 : 1, -1);
}

extern "C" void __cdecl MenuDrawPauseMain(MENU *menu) {
    i32 both = Arcade_BothPlayersActive();
    if (MenuKeyboardPendingPc)
        both = 0;
    dme_r = 0;
    dme_g = 255;
    dme_b = 0x3f;
    GameDrawMenuEntry(menu, TTab[TextIdPc880838]);
    GameDrawMenuEntry(menu, TTab[TextIdPc880850]);
    GameDrawMenuEntry(menu, TTab[TextIdPc880bc4]);
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    GameDrawMenuEntry(menu, (char *)MenuPcStrings[18]);
    if (!MenuPcStringsInitialized)
        MenuPcStringsInit();
    GameDrawMenuEntry(menu, (char *)MenuPcStrings[29]);
    if (!SuperStory && !ChallengeMode && !Mission_Active(NULL))
        GameDrawMenuEntry(menu, TTab[TextIdPc88090c]);
    else
        DrawMenuEntryEx(menu, TTab[TextIdPc88090c], (u8)(i32)(MenuA * 0.5));
    if (both)
        GameDrawMenuEntry(menu, TTab[TextIdPc88089c]);
    if (WORLD->current_level != HUB_LDATA)
        GameDrawMenuEntry(menu, TTab[GAMEDEMO ? TextIdPc8808ec : TextIdPc8806f8]);
    else
        GameDrawMenuEntry(menu, TTab[TextIdPc880860]);
}

extern "C" void __cdecl MenuUpdatePauseExit(MENU *menu) {
    if (menu->confirm_pressed) {
        LEVELDATA_PC *save_level = CanSaveAndExit(WORLD);
        if (save_level) {
            if (menu->selected_item == 0) {
                NewLevelFromMenu(HUB_LDATA, -1, -1, 1);
                MenuSFX = GameAudio_GetSfxId(0x30);
                return;
            }
            if (menu->selected_item == 1) {
                PauseSaveAndExitPc = 1;
                FADETYPE fade_type = {FADE_TYPE_WIPE};
                NewLData = save_level;
                FadeSys.SetFade(fade_type, 0);
                MenuSFX = GameAudio_GetSfxId(0x30);
                return;
            }
        } else if (menu->selected_item == 0) {
            if (GAMEDEMO) {
                NewLevelFromMenu(TITLES_LDATA, -1, -1, 1);
                MenuSFX = GameAudio_GetSfxId(0x30);
                return;
            }
            save_level = CanSaveAndExit(WORLD);
            if (save_level) {
                NewLData = save_level;
                MenuSFX = GameAudio_GetSfxId(0x30);
                return;
            }
            if (WORLD->current_level != HUB_LDATA) {
                NewLevelFromMenu(HUB_LDATA, -1, -1, 1);
                if (Arcade && pause_i_pad != -1) {
                    i32 other = pause_i_pad == 0;
                    if (PCArcadePointGoals[(i8)ArcadeItem.play] > 0 &&
                        Arcade_Points[pause_i_pad] < Arcade_Points[other])
                        ++Arcade_Score[other];
                }
            } else {
                NuPadSetStatus(0, 0);
                NuPadSetStatus(1, 0);
                NewLevelFromMenu(TITLES_LDATA, -1, -1, 0);
            }
            MenuSFX = GameAudio_GetSfxId(0x30);
            return;
        }
    } else if (!menu->cancel_pressed) {
        return;
    }
    BackupMenu();
    MenuSFX = GameAudio_GetSfxId(0x31);
}

extern "C" {
    void __cdecl StartDropOut(GAMEOBJECT *);
    void __cdecl AICreatureResumeScript(GAMEOBJECT *);
}
extern "C" void __cdecl DropOut(i32 player, i32 resume, i32 silent, i32) {
    GAMEOBJECT *object = (GAMEOBJECT *)Player[player];
    if (!object)
        return;
    if (!silent) {
        if (!object->rider)
            NuPadSetStatus((GAMEPAD_INIT_RECORD *)object->pad - GamePadStorage, 0);
        GameAudio_PlaySfx(0x20, &object->collision_position, 0, 0);
    }
    ReleaseTakeOver(object, 1);
    i16 secondary = object->secondary_lean_angle;
    i16 movement = object->movement_lean_angle;
    i16 tertiary = object->tertiary_lean_angle;
    ReleaseEat(object);
    if (VehicleArea) {
        ResetPlayerPacket(object->unknown_06c4, object->character_data);
        object->secondary_lean_angle = secondary;
        object->movement_lean_angle = movement;
        object->tertiary_lean_angle = tertiary;
    }
    object->flags_1fc &= ~0x80u;
    AICreatureResumeScript(object);
    if (VehicleArea)
        StartDropOut(object);
    PlayerProgress[player].hitpoints =
        object->dead || object->character_context == 0x2b ? object->field_107e : object->current_hp;
    object->powerup_time = 0.0f;
    GameCam_Blend(GameCam, VehicleArea ? 0.5f : 1.0f, 0.0f, 1);
    if (resume) {
        ResumeGame(1);
        TextInputSetKeyboardOverridePc(0);
    }
    GAMEOBJECT *other = (GAMEOBJECT *)Player[player == 0];
    if (other) {
        NUVEC direction;
        NuVecSub(&direction, &other->collision_position, &GameCam->position);
        if (NewRayCastScaleYMask(&GameCam->position, &direction, 0.0f, 1.0f, 0, 0x1f))
            GameCam_Reset(GameCam);
    }
}

extern "C" {
    extern void(__cdecl *PauseGame_ExtraCodeFn)(void);
}
extern "C" void __cdecl PauseGame(i32 pad) {
    Paused = 1;
    NuPause(1);
    music_man.SetFader(0.0f, 0.5f);
    music_man.PauseTrack(0x10);
    NuSound3StopRumble();
    if (memcard_autosaveenabled && memcard_autosavedisabled)
        NewMenu(1011, 0, -1);
    else
        NewMenu(LEGOMENU_PAUSEMAIN, 0, -1);
    ResetTimer(&PauseTimer, 0.0f);
    GameAudio_PlaySfx(0x36, NULL, 0, 0);
    DoubleScoreTime = 0.0f;
    ResetTimer(&JoinInTimer, 0.0f);
    pause_i_pad = pad;
    TextInputSetLastDevicePc(pad);
    for (i32 i = 0; i < 8; ++i) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[i];
        if (object) {
            object->hud_icon_timer = 0.0f;
            object->input_toggle_hold_time = TOGGLEHOLDTIME;
            object->pause_context_state = 0.0f;
        }
    }
    if (PauseGame_ExtraCodeFn)
        PauseGame_ExtraCodeFn();
}

extern "C" void __cdecl PadOutPause(i32 index, WORLDINFO *world) {
    if (Paused)
        return;
    i32 menu = GetMenuID();
    GAMEPAD_RUNTIME_INPUT_PC *pad = &((GAMEPAD_RUNTIME_INPUT_PC *)GamePadStorage)[index];
    if (!pad->input_ready || (pad->bound_pad->flags & 4))
        return;
    if ((LEGOMENU_NEWGAME != -1 && menu == LEGOMENU_NEWGAME) || (LEGOMENU_CREDITS != -1 && menu == LEGOMENU_CREDITS))
        return;
    if (NewMode || NewLData || ((FADEINFO_PC *)&FadeSys)->fade != 0.0f)
        return;
    if (GamePads_IgnoreInputFn && GamePads_IgnoreInputFn())
        return;
    if (MiniCutCam)
        return;
    if (world->area && (world->area->flags & 2))
        return;
    if (world->current_level->flags & 0xe0)
        return;
    if (GetMenuID() != -1)
        return;
    if (CUTSTOPGAME && !CutScene_IsSkippable((CUTINFO *)CutStopInfo))
        return;
    if (!CutSceneWaiting)
        PauseGame(index);
}

/* Replace/move the two unused static menu.c bodies with these PC-authoritative external bodies. */
extern "C" {
    extern i16 TextIdPc880840, TextIdPc880864, TextIdPc880874;
    extern i16 TextIdPc8806fc, TextIdPc880910, TextIdPc8808f0, TextIdPc8807d0, TextIdPc8807d4;
    extern i16 TextIdPc880914, TextIdPc88084c, TextIdPc880794, TextIdPc880700, TextIdPc880754, TextIdPc880890;
}
extern "C" void __cdecl MenuDrawPauseRestart(MENU *menu) {
    if (!MenuStopDraw) {
        SmartTextEx(TTab[TextIdPc880840], PauseMenus_X, (f32)(menu->draw_y - MENUDY * 1.5), 1.0f, MenuTextScalePc,
                    MenuTextScalePc, MenuTextScalePc, PauseMenus_Align, AlternateHudColourBytesPc[0],
                    AlternateHudColourBytesPc[1], AlternateHudRgbPc[0], 1.7f, 1, NULL, 0, MenuA);
    }
    GameDrawMenuEntry(menu, TTab[TextIdPc880864]);
    GameDrawMenuEntry(menu, TTab[TextIdPc880874]);
}
extern "C" void __cdecl MenuDrawPauseExit(MENU *menu) {
    LEVELDATA_PC *save_level = CanSaveAndExit(WORLD);
    f32 spacing = 1.5f;
    char *warning, *title;
    if (save_level) {
        spacing = 2.0f;
        title = TTab[TextIdPc8806fc];
        warning = TTab[TextIdPc880910];
    } else if (GAMEDEMO) {
        title = TTab[TextIdPc8808f0];
        warning = TTab[TextIdPc880910];
    } else if (WORLD->current_level != HUB_LDATA) {
        title = TTab[TextIdPc8806fc];
        if (Arcade) {
            warning = NULL;
            if (pause_i_pad != -1) {
                i32 other = pause_i_pad == 0;
                warning = TTab[PCArcadePointGoals[(i8)ArcadeItem.play] > 0 &&
                                       Arcade_Points[pause_i_pad] < Arcade_Points[other]
                                   ? TextIdPc8807d0
                                   : TextIdPc8807d4];
            }
        } else {
            warning = TTab[SuperStory ? TextIdPc880914 : TextIdPc880910];
        }
    } else {
        title = TTab[TextIdPc88084c];
        warning = TTab[TextIdPc880794];
    }
    if (!MenuStopDraw) {
        SmartTextEx(title, PauseMenus_X, menu->draw_y - spacing * MENUDY, 1.0f, MenuTextScalePc, MenuTextScalePc,
                    MenuTextScalePc, PauseMenus_Align, AlternateHudColourBytesPc[0], AlternateHudColourBytesPc[1],
                    AlternateHudRgbPc[0], 1.7f, 1, NULL, 0, MenuA);
        if (warning && menu->selected_item == menu->first_row) {
            u16 angle = (u16)(i32)(NuFmod(GlobalTimer.time_elapsed_mod_seconds, 0.5f) * 2.0f * 65536.0);
            f32 blend = (f32)(NuTrigTable[angle >> 1] * 0.5 + 0.5);
            i32 red = (i32)(AlternateHudColourBytesPc[2] +
                            (AlternateHudColourBytesPc[5] - AlternateHudColourBytesPc[2]) * blend);
            i32 blue = (i32)(AlternateHudColourBytesPc[3] +
                             (AlternateHudColourBytesPc[6] - AlternateHudColourBytesPc[3]) * blend);
            i32 green = (i32)(AlternateHudRgbPc[1] + (AlternateHudRgbPc[2] - AlternateHudRgbPc[1]) * blend);
            i32 alpha_base = (i32)(AlternateHudColourBytesPc[4] +
                                   (AlternateHudColourBytesPc[7] - AlternateHudColourBytesPc[4]) * blend);
            i32 alpha = (i32)(alpha_base * (MenuA * 0.0078125));
            SmartTextEx(warning, PauseMenus_X, menu->draw_y + (save_level ? 2.0f : 1.0f) * MENUDY + spacing * MENUDY,
                        1.0f, MenuTextScalePc, MenuTextScalePc, MenuTextScalePc, PauseMenus_Align, red, blue, green,
                        1.7f, 1, NULL, 0, alpha);
        }
    }
    if (save_level) {
        GameDrawMenuEntry(menu, TTab[TextIdPc880700]);
        GameDrawMenuEntry(menu, TTab[TextIdPc880754]);
        GameDrawMenuEntry(menu, TTab[TextIdPc880890]);
    } else {
        GameDrawMenuEntry(menu, TTab[TextIdPc880864]);
        GameDrawMenuEntry(menu, TTab[TextIdPc880874]);
    }
}

/* Native autosave-cancel child1011 reached by PauseGame; save1000 child already implemented. */
extern "C" {
    extern u8 MenuAutoSaveCancelFirstPc;
    extern i32 SaveloadPendingPc;
    extern u32 SaveloadStartedTickPc;
    i32 __cdecl saveload_getinfo(void);

    void __cdecl MenuDrawAutoSaveCancel(MENU *menu) {
        if (!MenuASCancelFinished && !memcard_savefailed) {
            menu->draw_y = MENUBOTY;
            DrawMenuEntryEx(menu, apitxt_OK, (u8)MenuA);
        }
    }

    void __cdecl MenuUpdateAutoSaveCancel(MENU *menu) {
        if (MenuASCancelFinished) {
            MenuASCancelFinished = 0;
            BackupMenu();
            return;
        }
        if (memcard_savefailed) {
            if (MenuAutoSaveCancelFirstPc) {
                MenuAutoSaveCancelFirstPc = 0;
                memcard_autosavecanceldelay = 5.0f;
                g_enableButtonPrompts = 0;
                return;
            }
            if (memcard_autosavecanceldelay > 0.0f)
                return;
            g_enableButtonPrompts = 1;
            NewMenu(1000, -1, -1);
            memcard_autosavecanceldelay = 5.0f;
            MenuASCancelFinished = 1;
            MenuAutoSaveCancelFirstPc = 1;
        }
        if (menu->confirm_pressed || menu->cancel_pressed) {
            MenuSFX = MENUSFX_MENUSELECT;
            MenuASCancelFinished = 1;
        }
    }

    void __cdecl saveloadASCallEachFrame(void) {
        if (SaveloadPendingPc && (i32)(GetTickCount() - SaveloadStartedTickPc) > 1000) {
            SaveloadPendingPc = 0;
            saveload_getinfo();
            saveload_status = 1;
        }
    }
}
