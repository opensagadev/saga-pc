/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

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
    i32 field_50;             // 0x50
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
    u8 unknown_00a8[0x34];    // 0xa8
    i32 state;                // 0xdc
} MENU;

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
typedef struct MENU_s MENU_s;

struct MENU_s {                     // size 0x2678
    f32 menu_time;                  // 0x0000
    f32 unk;                        // 0x0004
    i16 selected_column;            // 0x0008
    i16 selected_row;               // 0x000a
    i16 first_column;               // 0x000c
    i16 first_row;                  // 0x000e
    i16 last_column;                // 0x0010
    i16 last_row;                   // 0x0012
    i16 menu;                       // 0x0014
    i8 previous_menu;               // 0x0016
    i8 flags_17;                    // 0x0017
    f32 transition_time;            // 0x0018
    f32 transition_duration;        // 0x001c
    f32 repeat_up_time;             // 0x0020
    f32 repeat_down_time;           // 0x0024
    f32 repeat_left_time;           // 0x0028
    f32 repeat_right_time;          // 0x002c
    u8 repeat_up_count;             // 0x0030
    u8 repeat_down_count;           // 0x0031
    u8 repeat_left_count;           // 0x0032
    u8 repeat_right_count;          // 0x0033
    i32 selected_item;              // 0x0034
    i32 selected_item_column;       // 0x0038
    i32 previous_item;              // 0x003c
    u32 buttons_held;               // 0x0040
    u32 buttons_pressed;            // 0x0044
    i32 column_count;               // 0x0048
    i32 row_count;                  // 0x004c
    i32 field_50;                   // 0x0050
    i32 up_pressed;                 // 0x0054
    i32 down_pressed;               // 0x0058
    i32 left_pressed;               // 0x005c
    i32 right_pressed;              // 0x0060
    i32 confirm_pressed;            // 0x0064
    i32 cancel_pressed;             // 0x0068
    i32 up_held;                    // 0x006c
    i32 down_held;                  // 0x0070
    i32 left_held;                  // 0x0074
    i32 right_held;                 // 0x0078
    u32 start_pressed;              // 0x007c
    u32 select_pressed;             // 0x0080
    u32 action_04_pressed;          // 0x0084
    u32 action_08_pressed;          // 0x0088
    i32 draw_item;                  // 0x008c
    f32 item_scale;                 // 0x0090
    f32 draw_x;                     // 0x0094
    f32 draw_y;                     // 0x0098
    f32 draw_z;                     // 0x009c
    f32 field_a0;                   // 0x00a0
    f32 centre_offset;              // 0x00a4
    u8 pad_a8[4];                   // 0x00a8
    i32 paused;                     // 0x00ac
    i32 field_b0;                   // 0x00b0
    i32 field_b4;                   // 0x00b4
    i32 field_b8;                   // 0x00b8
    i32 field_bc;                   // 0x00bc
    i32 unknown_c0;                 // 0x00c0
    f32 *item_offsets;              // 0x00c4
    f32 item_offset_scale;          // 0x00c8
    f32 field_cc;                   // 0x00cc
    f32 item_scale_x;               // 0x00d0
    f32 item_spacing;               // 0x00d4
    i32 input_disabled;             // 0x00d8
    i32 state;                      // 0x00dc
    i32 queued_item;                // 0x00e0
    i32 queued_column;              // 0x00e4
    i32 queued_row;                 // 0x00e8
    u8 move_left;                   // 0x00ec
    u8 move_right;                  // 0x00ed
    u8 move_up;                     // 0x00ee
    u8 move_down;                   // 0x00ef
    u8 close_requested;             // 0x00f0
    u8 input_activity;              // 0x00f1
    u8 pad_f2[2];                   // 0x00f2
    f32 horizontal_scroll_distance; // 0x00f4
    f32 item_x[400];                // 0x00f8
    f32 item_y[400];                // 0x0738
    f32 item_width[400];            // 0x0d78
    f32 item_height[400];           // 0x13b8
    i32 item_column[400];           // 0x19f8
    i32 item_row[400];              // 0x2038
};

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
