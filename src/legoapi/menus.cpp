#include "saga.h"

#include <string.h>

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
#pragma section(".bss$MenuButtonHistoryPc", read, write)
extern "C" {

    void __cdecl MenuRegisterSoundFX(i32 move, i32 select, i32 back, i32 no_entry) {
        MENUSFX_MENUMOVE = move;
        MENUSFX_MENUSELECT = select;
        MENUSFX_MENUBACK = back;
        MENUSFX_MENUNOENTRY = no_entry;
    }
}

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

/* 50b310/71. Existing menu globals and types, no new owner data. */
extern "C" i32 __cdecl MenuInMemoryCard(void) {
    if (GameMenuLevel != -1 && MenuValidated != 0 && MenuInfo[GameMenu[GameMenuLevel].menu].id >= 1000 &&
        MenuInfo[GameMenu[GameMenuLevel].menu].id < 1031)
        return 1;
    return 0;
}
