#pragma once

#include "nu2api/nucore/common.h"

typedef struct menu_s {       // size 0xe0
    f32 menu_time;            // 0x00
    f32 time_04;              // 0x04
    i16 selected_column;      // 0x08
    i16 selected_row;         // 0x0a
    i16 first_column;         // 0x0c
    i16 first_row;            // 0x0e
    i16 last_column;          // 0x10
    i16 last_row;             // 0x12
    i16 menu;                 // 0x14
    i8 previous_menu;         // 0x16
    i8 flags_17;              // 0x17
    f32 transition_time;      // 0x18
    f32 transition_duration;  // 0x1c
    f32 repeat_up_time;       // 0x20
    f32 repeat_down_time;     // 0x24
    f32 repeat_left_time;     // 0x28
    f32 repeat_right_time;    // 0x2c
    u8 repeat_up_count;       // 0x30
    u8 repeat_down_count;     // 0x31
    u8 repeat_left_count;     // 0x32
    u8 repeat_right_count;    // 0x33
    i32 selected_item;        // 0x34
    i32 selected_item_column; // 0x38
    i32 previous_item;        // 0x3c
    u32 buttons_held;         // 0x40
    u32 buttons_pressed;      // 0x44
    i32 column_count;         // 0x48
    i32 row_count;            // 0x4c
    i32 enter_state;          // 0x50
    i32 up_pressed;           // 0x54
    i32 down_pressed;         // 0x58
    i32 left_pressed;         // 0x5c
    i32 right_pressed;        // 0x60
    i32 confirm_pressed;      // 0x64
    i32 cancel_pressed;       // 0x68
    i32 up_held;              // 0x6c
    i32 down_held;            // 0x70
    i32 left_held;            // 0x74
    i32 right_held;           // 0x78
    u32 start_pressed;        // 0x7c
    u32 select_pressed;       // 0x80
    u32 action_04_pressed;    // 0x84
    u32 action_08_pressed;    // 0x88
    i32 draw_item;            // 0x8c
    f32 item_scale;           // 0x90
    f32 draw_x;               // 0x94
    f32 draw_y;               // 0x98
    f32 draw_z;               // 0x9c
    f32 draw_offset;          // 0xa0
    f32 centre_offset;        // 0xa4
    u8 reserved_a8[4];        // 0xa8
    i32 paused;               // 0xac
    i32 state_b0;             // 0xb0
    i32 state_b4;             // 0xb4
    i32 state_b8;             // 0xb8
    i32 state_bc;             // 0xbc
    i32 state_c0;             // 0xc0
    f32 *item_offsets;        // 0xc4
    f32 item_offset_scale;    // 0xc8
    u8 unknown_00cc[4];       // 0xcc
    f32 item_scale_x;         // 0xd0
    f32 item_spacing;         // 0xd4
    i32 input_disabled;       // 0xd8
    i32 state;                // 0xdc
} MENU;

/* Menu ids (menufninfo_s.id, NewMenu, GetMenuID), named after the update or enter function the menu tables
 * (GameMenuInfo, MenuInfo) give each id. */
#define MENU_TITLES 0
#define MENU_NEW_GAME 1
#define MENU_PAUSE_MAIN 2
#define MENU_PAUSE_RESTART 3
#define MENU_OPTIONS 4
#define MENU_EXTRAS 5
#define MENU_PAUSE_EXIT 6
#define MENU_SELECT_LANGUAGE 7
#define MENU_MISSIONS 8
#define MENU_END_MISSION 9
#define MENU_END_CHALLENGE 10
#define MENU_ARCADE_END 11
#define MENU_CUSTOMISER 12
#define MENU_SHOP 13
#define MENU_MINIKIT 14
#define MENU_SELECT_MODE 15
#define MENU_BONUS_MODE 16
#define MENU_FREE_PLAY 17
#define MENU_CREDITS 18
#define MENU_BONUS_WIN 19
#define MENU_BONUS_COMPLETE 20
#define MENU_CONTROL_SETUP 21
#define MENU_SELECT_CONTROLS 22
#define MENU_SAVE 1000
#define MENU_DO_NOT_REMOVE_CARD 1001
#define MENU_INSERT_CARD 1002
#define MENU_NOT_ENOUGH_SPACE 1003
#define MENU_NO_MEMORY_CARD 1004
#define MENU_NO_DATA 1005
#define MENU_FORMAT_CONFIRM 1006
#define MENU_FORMATTING 1007
#define MENU_SAVE_CONFIRM 1008
#define MENU_SAVING 1009
#define MENU_AUTO_SAVE_WARNING 1010
#define MENU_AUTO_SAVE_CANCEL 1011
#define MENU_LOAD 1012
#define MENU_LOAD_CONFIRM 1013
#define MENU_LOADING 1014
#define MENU_DELETING 1015
#define MENU_SAVE_CANCEL 1016
#define MENU_LOAD_CANCEL 1017
#define MENU_FORMAT_CANCEL 1018
#define MENU_CARD_WARNING 1019
#define MENU_FILE_CORRUPT 1020
#define MENU_HEADER_SAVE 1021
#define MENU_START_NEW_GAME 1022
#define MENU_DELETE_CONFIRM 1023
#define MENU_VIDEO 1025
#define MENU_EFFECTS 1026
#define MENU_VIDEO_CONFIRM 1027
#define MENU_HIRES_NOTICE 1028
#define MENU_QUIT 1029

typedef struct menufninfo_s {  // size 0x1c
    i32 id;                    // 0x00
    void (*enter_fn)(MENU *);  // 0x04
    void (*draw_fn)(MENU *);   // 0x08
    void (*update_fn)(MENU *); // 0x0c
    void (*exit_fn)(MENU *);   // 0x10
    i16 memory_x;              // 0x14
    i16 memory_y;              // 0x16
    i8 wrap;                   // 0x18
    i8 confirm_prompt;         // 0x19
    i8 cancel_prompt;          // 0x1a
    u8 unknown_001b;           // 0x1b
} MENUFNINFO;

typedef struct MISSION_MENU_PACKET { // size 0xa
    i16 player_model[2];             // 0x0
    u8 active_player[2];             // 0x4
    u8 customise_other_player[2];    // 0x6
    u8 selected[2];                  // 0x8
} MISSION_MENU_PACKET;

#ifdef __cplusplus
extern "C" {
#endif
    extern char *apitxt_SELECT;
    extern char *apitxt_BACK;
    extern f32 MenuTextScale;
    extern f32 MenuPromptPositionScale;
    extern f32 MENUDY;
    void DrawMenuHeader(void);
    extern i32 g_enableButtonPrompts;
    extern f32 MenuTopY;
    extern u8 MENUNORMALR;
    extern u8 MENUNORMALG;
    extern u8 MENUNORMALB;
    extern f32 ICONX;
    MENU *NewMenu(i32 menu_id, i32 remember, i32 argument);
    extern i32 MenuValidated;
    extern f32 MenuAlpha;
    extern i32 MenuA;
    extern i32 CurrentMenuId;
    extern MENUFNINFO MenuInfo[100];
    extern MENUFNINFO GameMenuInfo[23];
    extern MENU GameMenu[10];
    extern i32 GameMenuLevel;
    extern i32 MenusUsed;
    extern char MenuHeader[64];
    extern u8 MENUHEADERR;
    extern u8 MENUHEADERG;
    extern u8 MENUHEADERB;
    extern i32 MenuFadeEnabled;
    extern const char Menu_ColonSeparator[3];
    extern f32 dme_sy;
    void MenuInitialiseEx(MENUFNINFO *menu_info, i32 menu_id_count, i32 language_count, void (*draw_save_slots_info_fn)(f32, f32, i32, i32),
                          i32 is_fade_enabled, i32 is_shadow_enabled);
    void MenuInitialise(MENUFNINFO *menu_info, i32 menu_id_count, i32 language_count, void (*draw_save_slots_fn)(MENU *, f32),
                        i32 is_fade_enabled, i32 is_shadow_enabled);
    void MenuReset(void);
    extern i32 newgamecam;

    extern i32 MenuResult;
    extern i32 menu_pulsate_angle;
    extern f32 menu_pulsate_speed;
    extern f32 menu_pulsate;
    extern i32 MenuSuppressUpHeld;
    extern i32 MenuSuppressDownHeld;
    extern i32 MenuSuppressLeftHeld;
    extern i32 MenuSuppressRightHeld;
    i32 UpdateMenu(u32, u32, u32, u32, f32, u32, u32, u32, u32);
    extern u8 MENUENTRYR;
    extern u8 MENUENTRYG;
    extern u8 MENUENTRYB;
    extern u8 MENUFLASH0R;
    extern u8 MENUFLASH0G;
    extern u8 MENUFLASH0B;
    extern u8 MENUFLASH1R;
    extern u8 MENUFLASH1G;
    extern u8 MENUFLASH1B;
    void APIMenuDrawMemCardSlots(MENU *, f32);
    void MenuRememberCursor(MENU *);
    void BackupMenu(void);
    void DrawMenuEntryEx(MENU *, char *, u8);
    extern f32 dme_sx;
    extern i32 dme_rgb;
    extern i32 dme_align;
    extern u8 dme_r;
    extern u8 dme_g;
    extern u8 dme_b;
    extern f32 MENUENTRYEXWIDTH;
    void APIMenuDrawGameState(f32, f32, i32, i32);
    extern u8 MENUWARNR;
    extern u8 MENUWARNG;
    extern u8 MENUWARNB;

    void MenuLoadTechnicalStrings(char *, char *, VARIPTR *, VARIPTR);
    extern char *apiGameTitle;
    extern char *apiGameSubTitle;
    extern char *apitxt_CONTINUE;
    extern char *apitxt_GAMEOVERWRITTEN;
    extern char *apitxt_DOYOUWANTTOSTART;
    extern char *apitxt_CONTINUEWITHOUTSAVING;
    extern char *apitxt_GAMEOVERWRITING;
    extern char *apitxt_CONTINUEWITHOUTLOADING;
    extern char *apitxt_COMPLETED;
    extern char *apitxt_VIBRATION;
    extern char *apitxt_PROGRESS;
    extern char *apitxt_TIMEPLAYED;
    extern char *apitxt_CONTROLLERREMOVED;
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 shop_quit;
    void MenuInitSelectMode(MENU *);
    void MenuInitBonusMode(MENU *);
    void MenuDrawSelectMode(MENU *);
    void MenuDrawBonusMode(MENU *);
    void MenuUpdateSelectMode(MENU *);
    void MenuUpdateBonusMode(MENU *);
#ifdef __cplusplus
}
#endif
