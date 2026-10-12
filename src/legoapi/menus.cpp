#include "saga.h"

typedef struct MENU_INPUT_PAD_PC {
    FRAME_PAD_PC *bound_pad;
    u32 held, pressed, alternate_held, alternate_pressed;
    u8 unknown14[0x4c];
} MENU_INPUT_PAD_PC;
STATIC_ASSERT(sizeof(MENU_INPUT_PAD_PC) == 0x60);
STATIC_ASSERT(offsetof(MENU_INPUT_PAD_PC, held) == 4);
STATIC_ASSERT(offsetof(MENU_INPUT_PAD_PC, pressed) == 8);
STATIC_ASSERT(offsetof(MENU_INPUT_PAD_PC, alternate_held) == 0xc);
STATIC_ASSERT(offsetof(MENU_INPUT_PAD_PC, alternate_pressed) == 0x10);
STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
STATIC_ASSERT(offsetof(LEVELDATA_PC, flags) == 0x64);
STATIC_ASSERT(offsetof(GAMESAVE, options_save) + offsetof(OPTIONSSAVE, widescreen) == 0xf);
extern "C" {
    extern i32 MenuKeyboardAlwaysPc, MenuKeyboardSecondaryPc;
    extern u8 MenuKeyboardOverrideActivePc;
    extern i32 MenuKeyboardPendingPc, pause_i_pad, editor_active;
    extern u32 GAMEPAD_MENUSELECT, GAMEPAD_MENUCANCEL, GAMEPAD_SELECT;
    i32 __cdecl UpdateMenu(u32, u32, u32, u32, f32, u32, u32, u32, u32);

    void __cdecl UpdateGameMenu(GAMEPAD_PC *input, i32 check_buttons) {
        MENU_INPUT_PAD_PC *pad = (MENU_INPUT_PAD_PC *)input;
        MENU *menu;
        i32 menu_id = GetMenuID();
        i32 menu_active, active_players, device, both_pads = 0;
        i32 first_active, second_active, previous_item, previous_column, result;
        i16 previous_menu;
        u8 widescreen;
        u32 held = 0, pressed = 0, alternate_held = 0, alternate_pressed = 0;
        if (check_buttons != 0 && WORLD != NULL) {
            STUBBED_NAMED("0x00495332: original button-history/cheat update at0x00493de0");
        }
        menu_active = GetMenuID() != -1;
        if (WORLD != NULL && (WORLD->current_level->flags & 0x400) != 0)
            menu_active = 1;
        if (Hub_InMenu() != 0)
            menu_active = 0;
        if (MenuKeyboardAlwaysPc != 0 || MenuKeyboardSecondaryPc != 0 || menu_active != 0) {
            if (MenuKeyboardOverrideActivePc == 0) {
                active_players = 0;
                device = TextInputLinkedDevicePc(TextInputLastDevicePc());
                if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0)
                    active_players = 1;
                if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0)
                    ++active_players;
                if (active_players < 2 || TextInputIsKeyboardDevicePc(device) != 0)
                    TextInputSetKeyboardOverridePc(1);
                MenuKeyboardOverrideActivePc = 1;
            }
        } else if (MenuKeyboardOverrideActivePc != 0) {
            TextInputSetLastDevicePc(-1);
            TextInputSetKeyboardOverridePc(0);
            MenuKeyboardOverrideActivePc = 0;
            MenuKeyboardPendingPc = 0;
        }
        menu = &GameMenu[GameMenuLevel];
        previous_menu = menu->menu;
        if ((Paused != 0 || NetPaused != 0) && pause_i_pad != -1) {
            pad = (MENU_INPUT_PAD_PC *)&GamePadStorage[pause_i_pad];
        } else if (WORLD == NULL) {
            both_pads = 1;
        } else if (menu_id != -1 && WORLD->current_level == TITLES_LDATA) {
            both_pads = 0;
        } else if (menu_id == 0x12 && PlayerProgress[0].active != 0 && PlayerProgress[1].active != 0) {
            both_pads = 1;
        } else if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0 && Player[1] != NULL &&
                   (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0) {
            both_pads = 1;
        } else if (pad == (MENU_INPUT_PAD_PC *)&GamePadStorage[0]) {
            first_active = 0;
            if (WORLD->current_level == CREDITS_LDATA) {
                first_active = PlayerProgress[0].active;
                second_active = PlayerProgress[1].active;
                if (second_active != 0 && first_active == 0)
                    pad = (MENU_INPUT_PAD_PC *)&GamePadStorage[1];
                if (first_active != 0 && second_active != 0)
                    both_pads = 1;
                else if (first_active == 0 && second_active == 0) {
                    readpads_always = 1;
                    both_pads = 1;
                }
            } else if (Player[0] != NULL && Player[1] != NULL) {
                first_active = (((GAMEOBJECT *)Player[0])->flags_1fc_word & 0x80) != 0 &&
                               (((GAMEOBJECT *)Player[0])->flags_1f8 & 0x40000) == 0;
                second_active = (((GAMEOBJECT *)Player[1])->flags_1fc_word & 0x80) != 0 &&
                                (((GAMEOBJECT *)Player[1])->flags_1f8 & 0x40000) == 0;
                if (second_active != 0 && first_active == 0)
                    pad = (MENU_INPUT_PAD_PC *)((GAMEOBJECT *)Player[1])->pad;
                if (first_active == 0 && second_active == 0) {
                    readpads_always = 1;
                    both_pads = 1;
                }
            } else {
                readpads_always = 1;
                both_pads = 1;
            }
            if (GameMenuLevel != 0) {
                if (both_pads != 0)
                    TextInputSetLastDevicePc(PlayerProgress[0].active != 0 ? 0 : 1);
                else
                    TextInputSetLastDevicePc(first_active != 0 ? 0 : 1);
            }
        }
        if (editor_active == 0) {
            widescreen = Game.options_save.widescreen;
            STUBBED_NAMED("0x004955c7: original loadsaveCallEachFrame at0x005114a0");
            Game.options_save.widescreen = widescreen;
        }
        if (!(((FADEINFO_PC *)&FadeSys)->fade > 0.0f) && NewMode == 0 && NewLData == NULL && editor_active == 0) {
            if (menu_id != 1 || !(GameTimer.time_elapsed < 4.0f)) {
                if (both_pads != 0) {
                    MENU_INPUT_PAD_PC *first = (MENU_INPUT_PAD_PC *)&GamePadStorage[0];
                    MENU_INPUT_PAD_PC *second = (MENU_INPUT_PAD_PC *)&GamePadStorage[1];
                    held = first->held | second->held;
                    pressed = first->pressed | second->pressed;
                    alternate_held = first->alternate_held | second->alternate_held;
                    alternate_pressed = first->alternate_pressed | second->alternate_pressed;
                } else {
                    held = pad->held;
                    pressed = pad->pressed;
                    alternate_held = pad->alternate_held;
                    alternate_pressed = pad->alternate_pressed;
                }
            }
            previous_item = menu->selected_item;
            previous_column = menu->selected_item_column;
            result = UpdateMenu(held, pressed, alternate_held, alternate_pressed, FRAMETIME, GAMEPAD_MENUSELECT,
                                GAMEPAD_MENUCANCEL, GAMEPAD_START, GAMEPAD_SELECT);
            if (MenuSFX == -1 && menu->selected_item != -1 && menu->selected_item_column != -1 && previous_item != -1 &&
                previous_column != -1 &&
                (previous_item != menu->selected_item || previous_column != menu->selected_item_column) &&
                GetMenuID() == menu_id)
                GameAudio_PlaySfx(0x2f, NULL, 0, 0);
            if (result == 1 && Paused != 0) {
                STUBBED_NAMED("0x0049570b: original ResumeGame(1) at0x0055c560");
            }
        }
        if (previous_menu != -1 && NewLData != NULL && gone_through_door_to_new_level == 0)
            reset_area = 1;
    }
}

struct mission_data_pc_s {
    i16 find_character;
    i16 level;
    i16 name_id;
    i16 text_id;
    i32 bounty;
    i32 alternate_bounty;
    u16 time;
    u8 index;
    u8 unknown_13[5];
};
typedef struct MISSION_SYSTEM_READER_PC {
    MISSIONDATA_PC *missions;
    MISSIONDATA_PC *active_mission;
    TIMER timer;
    MISSION_SAVE_READER_PC *save;
    u8 count;
    u8 state;
    u8 flags;
    u8 character_count;
    i16 character_ids[8];
} MISSION_SYSTEM_READER_PC;
STATIC_ASSERT(sizeof(MISSIONDATA_PC) == 0x18);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, find_character) == 0);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, level) == 2);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, name_id) == 4);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, text_id) == 6);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, bounty) == 8);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, alternate_bounty) == 0xc);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, time) == 0x10);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, index) == 0x12);
STATIC_ASSERT(sizeof(MISSION_SAVE_READER_PC) == 0x64);
STATIC_ASSERT(offsetof(MISSION_SAVE_READER_PC, best_times) == 0);
STATIC_ASSERT(offsetof(MISSION_SAVE_READER_PC, completed) == 0x50);
STATIC_ASSERT(sizeof(MISSION_SYSTEM_READER_PC) == 0x30);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, missions) == 0);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, active_mission) == 4);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, timer) == 8);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, save) == 0x18);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, count) == 0x1c);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, state) == 0x1d);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, flags) == 0x1e);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, character_count) == 0x1f);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, character_ids) == 0x20);
STATIC_ASSERT(offsetof(TIMER, time_elapsed) == 0);

STATIC_ASSERT(sizeof(MISSION_MENU_PACKET_PC) == 0xa);
STATIC_ASSERT(offsetof(MISSION_MENU_PACKET_PC, player_model) == 0);
STATIC_ASSERT(offsetof(MISSION_MENU_PACKET_PC, active_player) == 4);
STATIC_ASSERT(offsetof(MISSION_MENU_PACKET_PC, selected) == 8);

extern "C" {
    extern MISSION_MENU_PACKET_PC MissionMenuPacketPc;
    extern TIMER JoinInTimer;
    extern f32 MissionIconGapPc, MissionIconStepPc;
}
static f32 MissionsAlphaPc;
static i32 MissionMenuSelectionPc;
static f32 MissionIconScalePc[20];
static f32 MissionIconTargetXPc[20];
static f32 MissionIconXPc[20];

static void MissionIconPositionsPc(MISSION_SYSTEM_READER_PC *system, i32 initialize) {
    i32 index;
    f32 x;
    for (index = 0; index < system->count; ++index) {
        x = 0.0f;
        if (index < MissionMenuSelectionPc)
            x = -MissionIconGapPc - (MissionMenuSelectionPc - index) * MissionIconStepPc;
        else if (index != MissionMenuSelectionPc)
            x = MissionIconGapPc + (index - MissionMenuSelectionPc) * MissionIconStepPc;
        MissionIconTargetXPc[index] = x;
        if (initialize != 0)
            MissionIconXPc[index] = x;
        else
            MissionIconXPc[index] = SeekValF(MissionIconXPc[index], x, 5.0f);
    }
}

extern "C" {
    void __cdecl EndMissionsMenu(void) {
        LevLock[3] = 1;
        LevTime[3] = 0.6f;
        MenuRememberCursor(&GameMenu[GameMenuLevel]);
        MenuReset();
        ResetTimer(&JoinInTimer, 0.0f);
    }

    void __cdecl MenuInitMissions(MENU *menu) {
        MISSION_SYSTEM_READER_PC *system = (MISSION_SYSTEM_READER_PC *)MissionSys;
        i32 index;
        MissionsAlphaPc = 0.0f;
        if (system == NULL)
            return;
        for (index = 0; index < system->count; ++index)
            MissionIconScalePc[index] = index == MissionMenuSelectionPc ? 1.0f : 0.0f;
        MissionIconPositionsPc(system, 1);
    }

    void __cdecl MenuUpdateMissions(MENU *menu) {
        i32 previous = MissionMenuSelectionPc;
        i32 start = 0, cancel = 0, left = 0, right = 0;
        i32 player, index;
        MISSION_SYSTEM_READER_PC *system;
        if (MissionSys == NULL)
            cancel = 1;
        for (player = 0; player < 2; ++player) {
            MENU_INPUT_PAD_PC *pad;
            u32 pressed, directions;
            if (start != 0 || cancel != 0)
                break;
            if (MissionMenuPacketPc.active_player[player] == 0)
                continue;
            pad = (MENU_INPUT_PAD_PC *)&GamePadStorage[player];
            pressed = pad->pressed;
            if ((pressed & GAMEPAD_MENUSELECT) != 0) {
                if (MissionMenuPacketPc.active_player[0] == 0 || MissionMenuPacketPc.active_player[1] == 0)
                    start = 1;
                else if (MissionMenuPacketPc.selected[player] == 0) {
                    MissionMenuPacketPc.selected[player] = 1;
                    if (MissionMenuPacketPc.selected[0] != 0 && MissionMenuPacketPc.selected[1] != 0)
                        start = 1;
                    else
                        GameAudio_PlaySfx(0x30, NULL, 0, 0);
                }
            } else if ((pressed & GAMEPAD_MENUCANCEL) != 0) {
                if (MissionMenuPacketPc.active_player[0] == 0 || MissionMenuPacketPc.active_player[1] == 0 ||
                    MissionMenuPacketPc.selected[player] == 0)
                    cancel = 1;
                else {
                    MissionMenuPacketPc.selected[player] = 0;
                    GameAudio_PlaySfx(0x31, NULL, 0, 0);
                }
            } else if (MissionMenuPacketPc.selected[0] == 0 && MissionMenuPacketPc.selected[1] == 0) {
                directions = pad->alternate_pressed;
                if (((directions | pressed) & (u32)GAMEPAD_DLEFT) != 0)
                    left = 1;
                else if (((directions | pressed) & (u32)GAMEPAD_DRIGHT) != 0)
                    right = 1;
            }
        }
        if (start != 0) {
            system = (MISSION_SYSTEM_READER_PC *)MissionSys;
            system->state = 1;
            system->active_mission = &system->missions[MissionMenuSelectionPc];
            ResetTimer(&system->timer, 0.0f);
            STUBBED_NAMED("0x00497ab1: MenuUpdateMissions Cheats_TurnOff(0) native56e2e0");
            GameAudio_PlaySfx(0x30, NULL, 0, 0);
            NextArea_FreePlay = 0;
            FreePlay = 0;
            NewLData = &LDataList[system->active_mission->level];
        } else if (cancel != 0) {
            GameAudio_PlaySfx(0x31, NULL, 0, 0);
            EndMissionsMenu();
        } else if (left != 0) {
            if (MissionMenuSelectionPc > 0)
                --MissionMenuSelectionPc;
        } else if (right != 0 && MissionMenuSelectionPc < ((MISSION_SYSTEM_READER_PC *)MissionSys)->count - 1)
            ++MissionMenuSelectionPc;
        if (MissionMenuSelectionPc != previous) {
            GameAudio_PlaySfx(0x2f, NULL, 0, 0);
            MissionsAlphaPc = 0.0f;
        } else {
            MissionsAlphaPc += FRAMETIME * 2.0f;
            if (MissionsAlphaPc > 1.0f)
                MissionsAlphaPc = 1.0f;
        }
        system = (MISSION_SYSTEM_READER_PC *)MissionSys;
        if (system != NULL) {
            for (index = 0; index < system->count; ++index)
                MissionIconScalePc[index] =
                    SeekValF(MissionIconScalePc[index], index == MissionMenuSelectionPc ? 1.0f : 0.0f, 5.0f);
            MissionIconPositionsPc(system, 0);
        }
    }
}

extern "C" {
    extern i16 MissionMenuInstructionTextPc;
    extern f32 MissionIconBaseSizePc, MissionIconSelectedSizePc, MissionIconYPc;
    extern f32 MissionNameYPc, MissionNameScalePc;
    extern f32 MissionBestBrickXPc, MissionBestTimeXPc, MissionBestTimeYPc, MissionBestTimeScalePc;
    extern f32 MissionDropInAlphaPc, MissionPlayerIconSizePc, MissionEpisodeTitleYPc;
}

extern "C" void __cdecl MenuDrawMissions(MENU *menu) {
    f32 alpha, opacity, size, x, absolute_x, highlight, icon_size;
    i32 i, appearance;
    MISSION_SYSTEM_READER_PC *system;
    char time[64], text[64];
    (void)menu;
    if (MenuStopDraw)
        return;
    alpha = MenuAlpha;
    system = (MISSION_SYSTEM_READER_PC *)MissionSys;
    SmartTextEx(TTab[system->missions[MissionMenuSelectionPc].name_id], 0.0f, MissionNameYPc, 1.0f, MissionNameScalePc,
                MissionNameScalePc, MissionNameScalePc, 0, 255, 191, 0, 1.7f, 1, 0, NULL,
                (u32)(i32)(128.0f * MissionsAlphaPc * alpha));
    SmartTextEx(TTab[MissionMenuInstructionTextPc], 0.0f, 0.525f, 1.0f, MissionNameScalePc, MissionNameScalePc,
                MissionNameScalePc, 0, 255, 255, 255, 1.25f, 1, 0, NULL, (u32)(i32)(96.0f * alpha));
    if (Game.mission_save.completed[MissionMenuSelectionPc] != 0) {
        opacity = alpha * MissionsAlphaPc;
        if (opacity > 0.0f) {
            /* Native497188: Hub_DrawImportantBrick(211, x, episode_y+.1, opacity,-1,-1). */
            STUBBED_NAMED("MenuDrawMissions: Hub_DrawImportantBrick at 497188");
            Text_MakeTime(Game.mission_save.best_times[MissionMenuSelectionPc], 0, 1, 1, time);
            NuSPrintf(text, "~0(~2%s~0)", time);
            Text3DEx(text, MissionBestTimeXPc, MissionBestTimeYPc, 1.0f, MissionBestTimeScalePc, MissionBestTimeScalePc,
                     MissionBestTimeScalePc, 0, 0, 255, 0, (u8)(opacity * 128.0f));
        }
    }
    system = (MISSION_SYSTEM_READER_PC *)MissionSys;
    for (i = 0; i < system->count; ++i) {
        opacity = (MissionIconScalePc[i] * 0.3f + 0.7f) * alpha;
        x = MissionIconXPc[i] - 0.0f;
        absolute_x = (f32)fabs((f64)x);
        if (!(absolute_x > 1.0f)) {
            if (absolute_x > 0.5f)
                opacity *= 1.0f - (absolute_x - 0.5f) * 2.0f;
            if (opacity > 0.0f) {
                highlight = i == MissionMenuSelectionPc && menu_flash ? 0.7f : 1.0f;
                icon_size =
                    MissionIconBaseSizePc + MissionIconScalePc[i] * (MissionIconSelectedSizePc - MissionIconBaseSizePc);
                appearance = Game.mission_save.completed[i] != 0 ? 168 : 167;
                DrawCharIcon(system->missions[i].find_character, MissionIconXPc[i], MissionIconYPc, 0.0f, icon_size,
                             appearance, opacity, highlight * opacity, 1, NULL);
            }
        }
        system = (MISSION_SYSTEM_READER_PC *)MissionSys;
    }
    size = MissionMenuPacketPc.active_player[0] ? 1.0f : MissionDropInAlphaPc;
    DrawCharIcon(MissionMenuPacketPc.player_model[0], -ICONX, STATSPOSY, MissionPlayerIconSizePc,
                 MissionPlayerIconSizePc, 166, size, size, 1, NULL);
    size = MissionMenuPacketPc.active_player[1] ? 1.0f : MissionDropInAlphaPc;
    DrawCharIcon(MissionMenuPacketPc.player_model[1], ICONX, STATSPOSY, 0.0f, MissionPlayerIconSizePc, 165, size, size,
                 1, NULL);
    /* Native49751b: actual two-player prompt arguments derived from packet selected BYTEs.
       Genuine void provider is not yet implemented; no fake prompt/font success. */
    STUBBED_NAMED("MenuDrawMissions: DrawPlayerIconPrompts at 49751b");
}

extern "C" {
    extern void(__cdecl *ResumeGame_ExtraCodeFn)(void);
    void __cdecl ResumeGame(i32 play_sound) {
        Paused = 0;
        NetPaused = 0;
        NuPause(0);
        MenuRememberCursor(&GameMenu[GameMenuLevel]);
        MenuReset();
        music_man.SetFader(1.0f, 0.5f);
        music_man.ResumeTrack(0x10);
        if (play_sound != 0)
            GameAudio_PlaySfx(0x37, NULL, 0, 0);
        if (ResumeGame_ExtraCodeFn != NULL)
            ResumeGame_ExtraCodeFn();
    }
}
