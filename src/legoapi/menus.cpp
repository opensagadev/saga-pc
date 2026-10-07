#include "saga.h"

typedef struct MENU_INPUT_PAD_PC {
    PAD_INPUT_PC *bound_pad;
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
