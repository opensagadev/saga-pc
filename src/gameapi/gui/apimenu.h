/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

typedef struct menu_s {
    f32 menu_time;
    f32 time_04;
    i16 selected_column;
    i16 selected_row;
    i16 first_column;
    i16 first_row;
    i16 last_column;
    i16 last_row;
    i16 menu;
    i8 previous_menu;
    i8 flags_17;
    f32 transition_time;
    f32 transition_duration;
    f32 repeat_up_time;
    f32 repeat_down_time;
    f32 repeat_left_time;
    f32 repeat_right_time;
    u8 repeat_up_count;
    u8 repeat_down_count;
    u8 repeat_left_count;
    u8 repeat_right_count;
    i32 selected_item;
    i32 selected_item_column;
    i32 previous_item;
    u32 buttons_held;
    u32 buttons_pressed;
    i32 column_count;
    i32 row_count;
    i32 field_50;
    i32 up_pressed;
    i32 down_pressed;
    i32 left_pressed;
    i32 right_pressed;
    i32 confirm_pressed;
    i32 cancel_pressed;
    i32 up_held;
    i32 down_held;
    i32 left_held;
    i32 right_held;
    u32 start_pressed;
    u32 select_pressed;
    u32 action_04_pressed;
    u32 action_08_pressed;
    i32 draw_item;
    f32 item_scale;
    f32 draw_x;
    f32 draw_y;
    f32 draw_z;
    f32 draw_offset;
    f32 centre_offset;
    u8 unknown_00a8[0x34];
    i32 state;
} MENU;

STATIC_ASSERT(sizeof(MENU) == 0xe0);
STATIC_ASSERT(offsetof(MENU, selected_column) == 0x08);
STATIC_ASSERT(offsetof(MENU, menu_time) == 0);
STATIC_ASSERT(offsetof(MENU, time_04) == 4);
STATIC_ASSERT(offsetof(MENU, selected_row) == 0x0a);
STATIC_ASSERT(offsetof(MENU, menu) == 0x14);
STATIC_ASSERT(offsetof(MENU, selected_item) == 0x34);
STATIC_ASSERT(offsetof(MENU, selected_item_column) == 0x38);
STATIC_ASSERT(offsetof(MENU, left_pressed) == 0x5c);
STATIC_ASSERT(offsetof(MENU, right_pressed) == 0x60);
STATIC_ASSERT(offsetof(MENU, confirm_pressed) == 0x64);
STATIC_ASSERT(offsetof(MENU, cancel_pressed) == 0x68);
STATIC_ASSERT(offsetof(MENU, draw_item) == 0x8c);
STATIC_ASSERT(offsetof(MENU, item_scale) == 0x90);
STATIC_ASSERT(offsetof(MENU, draw_y) == 0x98);
STATIC_ASSERT(offsetof(MENU, centre_offset) == 0xa4);

typedef struct menufninfo_s {
    i32 id;
    void (*enter_fn)(MENU *);
    void (*draw_fn)(MENU *);
    void (*update_fn)(MENU *);
    void (*exit_fn)(MENU *);
    i16 memory_x;
    i16 memory_y;
    i8 wrap;
    i8 confirm_prompt;
    i8 cancel_prompt;
    u8 unknown_001b;
} MENUFNINFO;

STATIC_ASSERT(sizeof(MENUFNINFO) == 0x1c);
STATIC_ASSERT(offsetof(MENUFNINFO, enter_fn) == 4);
STATIC_ASSERT(offsetof(MENUFNINFO, exit_fn) == 0x10);
STATIC_ASSERT(offsetof(MENUFNINFO, memory_x) == 0x14);
STATIC_ASSERT(offsetof(MENUFNINFO, wrap) == 0x18);

typedef struct MISSION_MENU_PACKET_PC {
    i16 player_model[2];
    u8 active_player[2];
    u8 customise_other_player[2];
    u8 selected[2];
} MISSION_MENU_PACKET_PC;

STATIC_ASSERT(sizeof(MISSION_MENU_PACKET_PC) == 0xa);
STATIC_ASSERT(offsetof(MISSION_MENU_PACKET_PC, player_model) == 0);
STATIC_ASSERT(offsetof(MISSION_MENU_PACKET_PC, active_player) == 4);
STATIC_ASSERT(offsetof(MISSION_MENU_PACKET_PC, customise_other_player) == 6);
STATIC_ASSERT(offsetof(MISSION_MENU_PACKET_PC, selected) == 8);
#ifdef __cplusplus
extern "C" {
#endif
    extern char *apitxt_SELECT;
    extern char *apitxt_BACK;
    extern f32 MenuTextScalePc;
    extern f32 MenuPromptPositionScalePc;
    extern f32 MENUDY;
    void DrawMenuHeader(void);
    extern i32 g_enableButtonPrompts;
    extern f32 MenuTopYPc;
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
    void MenuInitialiseEx(MENUFNINFO *menu_info, i32 menu_id_count, i32 language_count,
                          void (*draw_save_slots_info_fn)(f32, f32, i32, i32), i32 is_fade_enabled,
                          i32 is_shadow_enabled);
    void MenuInitialise(MENUFNINFO *menu_info, i32 menu_id_count, i32 language_count,
                        void (*draw_save_slots_fn)(MENU *, f32), i32 is_fade_enabled, i32 is_shadow_enabled);
    void MenuReset(void);
    extern i32 newgamecam;

    extern i32 MenuResult;
    extern i32 menu_pulsate_angle;
    extern f32 menu_pulsate_speed;
    extern f32 menu_pulsate;
    extern i32 MenuSuppressUpHeldPc;
    extern i32 MenuSuppressDownHeldPc;
    extern i32 MenuSuppressLeftHeldPc;
    extern i32 MenuSuppressRightHeldPc;
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

struct MENU_s {
    f32 menu_time;
    f32 unk;
    i16 selected_column;
    i16 selected_row;
    i16 first_column;
    i16 first_row;
    i16 last_column;
    i16 last_row;
    i16 menu;
    i8 previous_menu;
    i8 flags_17;
    f32 transition_time;
    f32 transition_duration;
    f32 repeat_up_time;
    f32 repeat_down_time;
    f32 repeat_left_time;
    f32 repeat_right_time;
    u8 repeat_up_count;
    u8 repeat_down_count;
    u8 repeat_left_count;
    u8 repeat_right_count;
    i32 selected_item;
    i32 selected_item_column;
    i32 previous_item;
    u32 buttons_held;
    u32 buttons_pressed;
    i32 column_count;
    i32 row_count;
    i32 field_50;
    i32 up_pressed;
    i32 down_pressed;
    i32 left_pressed;
    i32 right_pressed;
    i32 confirm_pressed;
    i32 cancel_pressed;
    i32 up_held;
    i32 down_held;
    i32 left_held;
    i32 right_held;
    u32 start_pressed;
    u32 select_pressed;
    u32 action_04_pressed;
    u32 action_08_pressed;
    i32 draw_item;
    f32 item_scale;
    f32 draw_x;
    f32 draw_y;
    f32 draw_z;
    f32 field_a0;
    f32 centre_offset;
    u8 filler_a8[4];
    i32 paused;
    i32 field_b0;
    i32 field_b4;
    i32 field_b8;
    i32 field_bc;
    i32 field_c0;
    f32 *item_offsets;
    f32 item_offset_scale;
    f32 field_cc;
    f32 item_scale_x;
    f32 item_spacing;
    i32 input_disabled;
    i32 state;
    i32 queued_item;
    i32 queued_column;
    i32 queued_row;
    u8 move_left;
    u8 move_right;
    u8 move_up;
    u8 move_down;
    u8 close_requested;
    u8 input_activity;
    u8 filler_f2[2];
    f32 horizontal_scroll_distance;
    f32 item_x[400];
    f32 item_y[400];
    f32 item_width[400];
    f32 item_height[400];
    i32 item_column[400];
    i32 item_row[400];
};

STATIC_ASSERT(sizeof(struct MENU_s) == 9848);
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
