/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/gui/apimenu.h"
#endif

struct FRAME_PAD_PC;
struct language_data_s;
struct numtl_s;
struct vufnt_s;

#ifdef __cplusplus
extern "C" {
#endif
    extern f32 QFONTSCALEY;
    extern u8 text_preset_r[10];
    extern u8 text_preset_g[10];
    extern u8 text_preset_b[10];
    extern u32 current_rgba;
    extern i32 apitext_half_rgb;
    extern i32 followon_line;
    extern i32 TextPrintModePc;
    extern i32 create_qfont2dlower;
    extern i32 create_qfont2dz;
    extern i32 create_qfont3d;
    extern i32 create_qfont3dz;
    extern struct vufnt_s *QFont2DZ;
    extern struct vufnt_s *QFont3D;
    void UnloadGameFont(i32 unused);
    extern struct vufnt_s *QFont2DLower;
    extern f32 SmartTextDefaultWidthPc;
    extern f32 SmartTextLongestWidthPc;
    extern struct numtl_s *MessageMtl;
    extern i32 MessageMtlInit;
    extern struct vufnt_s *QFont2DButtons;
    extern f32 INTROTEXT_Y;
    extern f32 INTROTEXT_SCALE;
    extern f32 TextScaleX;
    extern f32 TextScaleY;
    extern struct language_data_s *Text_LanguageList;
    extern i32 LANGUAGECOUNT;
    extern i32 Text_Language;
    extern void *Text_AppFont;
    extern char *Text_PathEnglish;
    extern char *Text_PathFrench;
    extern char *Text_PathGerman;
    extern char *Text_PathItalian;
    extern char *Text_PathSpanish;
    extern char *Text_PathDanish;
    extern char *Text_PathJapanese;
    extern i32 INTROTEXT_ID;
    extern const char TextPlatformPS2[4];
    extern const char TextPlatformGameCube[3];
    extern const char TextPlatform360[4];
    extern const char TextPlatformWii[4];
    extern void (*buttonmapfn)(char *, char *);
    extern i32 smarttext_fwn;
    extern struct vufnt_s *SmartTextFont;
    extern struct vufnt_s *QFont3DZ;
    extern f32 QFONTSCALEX;
    extern f32 STCOORDSCALE;
    extern const char Text_WibbleCodeword[7];
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

/* Private PC contract proposal. Include after saga.h. */
struct PC_CONTROL_ITEM_PC {
    u8 kind;
    u8 command;
    u8 label;

    union {
        u8 flags;

        struct {
            u8 port_mode : 3;
            u8 flag_08 : 1;
            u8 flag_10 : 1;
            u8 unbound_player1 : 1;
            u8 unbound_player2 : 1;
            u8 unbound_selected : 1;
        };
    };

    f32 pulse;
};

struct PC_CONTROL_ROW_PC {
    i32 count;
    i32 selected;
    PC_CONTROL_ITEM_PC *items;
};

struct PC_CONTROL_TEXT_PC {
    char text[0x100];
    f32 fade;
    f32 x;
    f32 y;
    f32 z;
    f32 x_scale;
    f32 y_scale;
    f32 wrap_width;
    u32 colour;
    u32 key;
    i8 alignment;
    u8 refreshed;
    u8 unknown126[2];
};

struct PC_CONTROL_CONFIG_PC {
    struct vufnt_s *font;
    void (*draw_background)(f32, f32, f32);
    PC_CONTROL_ITEM_PC **rows;
    void (*play_sound)(i32);
    const char *blocked_keys;
};

struct PC_CONTROL_SCREEN_PC {
    i32 result;
    i32 player_count;
    u8 drop_in;
    u8 active;
    u8 initialized;
    u8 unknown0b;
    f32 opacity;
    f32 unknown10;
    f32 fade;
    i32 phase;
    i32 row_count;
    i32 selected_row;
    i32 capture_state;
    i32 release_frames;
    i32 release_reset;
    f32 x_scale;
    f32 y_scale;
    i32 selected_player;
    i32 ports[4];
    i32 original_ports[4];
    i32 saved_ports[4];
    u32 config_hash;
    u8 missing_binding;
    u8 unknown71[3];
    void (*draw_background)(f32, f32, f32);
    f32 deadzone[4];
    f32 saved_deadzone[4];
    f32 keyboard_deadzone;
    f32 saved_keyboard_deadzone;
    struct vufnt_s *font;
    PC_CONTROL_ROW_PC rows[4];
    u8 unknownd4[4];
    PC_CONTROL_TEXT_PC text[12];
    f32 elapsed;
    struct numtl_s *material;
    void (*play_sound)(i32);
    i32 ItemAllowsBinding(PC_CONTROL_ROW_PC *, i32, i32);
    void AdvanceSelection(PC_CONTROL_ROW_PC *);
    i32 CountPlayerRows(PC_CONTROL_ROW_PC *);
    u32 ScaleAlpha(f32, u32);
    u32 ScaleColour(f32, f32, u32);
    u32 HashString(const char *);
    u8 PortAllowed(i32, i32, i32);
    u8 HasMissingBindings(void);
    void RefreshItems(void);
    void Register(PC_CONTROL_CONFIG_PC *, VARIPTR *, VARIPTR *);
    void Initialize(i32, i32, i32, u8);
    i32 HashConfiguration(void);
    void SaveConfiguration(void);
    i32 WaitForRelease(void);
    i32 Update(struct FRAME_PAD_PC *, struct FRAME_PAD_PC *, f32);
    void DrawDeadzone(f32, f32, f32, f32, f32, f32, f32, f32);
    void QueueText(char *, f32, f32, f32, i32, f32, f32, u32, f32, f32, u32);
    f32 DrawText(char *, f32, f32, f32, i32, f32, f32, u32, i32, f32);
    void DrawQueuedText(f32);
    i32 fun_00420160(void);
    i32 fun_00420170(i32);
    void fun_00420180(void (*)(i32));
    PC_CONTROL_SCREEN_PC *fun_00420190(void);
    void fun_00420320(i32);
    void fun_00420420(i32);
    void fun_00420440(void);
    void fun_00420d30(f32, f32, f32, f32, f32, f32, f32, f32);
    void fun_00421020(void);

    void Draw(void);
};

STATIC_ASSERT(sizeof(PC_CONTROL_ITEM_PC) == 8);
STATIC_ASSERT(sizeof(PC_CONTROL_ROW_PC) == 0xc);
STATIC_ASSERT(sizeof(PC_CONTROL_TEXT_PC) == 0x128);
STATIC_ASSERT(offsetof(PC_CONTROL_TEXT_PC, key) == 0x120);
STATIC_ASSERT(offsetof(PC_CONTROL_SCREEN_PC, ports) == 0x3c);
STATIC_ASSERT(offsetof(PC_CONTROL_SCREEN_PC, saved_ports) == 0x5c);
STATIC_ASSERT(offsetof(PC_CONTROL_SCREEN_PC, draw_background) == 0x74);
STATIC_ASSERT(offsetof(PC_CONTROL_SCREEN_PC, keyboard_deadzone) == 0x98);
STATIC_ASSERT(offsetof(PC_CONTROL_SCREEN_PC, rows) == 0xa4);
STATIC_ASSERT(offsetof(PC_CONTROL_SCREEN_PC, text) == 0xd8);
STATIC_ASSERT(offsetof(PC_CONTROL_SCREEN_PC, elapsed) == 0xeb8);
STATIC_ASSERT(offsetof(PC_CONTROL_SCREEN_PC, material) == 0xebc);
STATIC_ASSERT(offsetof(PC_CONTROL_SCREEN_PC, play_sound) == 0xec0);
STATIC_ASSERT(sizeof(PC_CONTROL_SCREEN_PC) == 0xec4);

STATIC_ASSERT(sizeof(PC_CONTROL_CONFIG_PC) == 0x14);
STATIC_ASSERT(offsetof(PC_CONTROL_CONFIG_PC, draw_background) == 4);
STATIC_ASSERT(offsetof(PC_CONTROL_CONFIG_PC, rows) == 8);
STATIC_ASSERT(offsetof(PC_CONTROL_CONFIG_PC, play_sound) == 0xc);
STATIC_ASSERT(offsetof(PC_CONTROL_CONFIG_PC, blocked_keys) == 0x10);
STATIC_ASSERT(offsetof(PC_CONTROL_TEXT_PC, alignment) == 0x124);
STATIC_ASSERT(offsetof(PC_CONTROL_TEXT_PC, refreshed) == 0x125);

#ifdef __cplusplus
extern "C" {
#endif
    extern PC_CONTROL_SCREEN_PC PcControlScreenPc;
    extern PC_CONTROL_ITEM_PC PcControlItemsPc[25];
    extern PC_CONTROL_ITEM_PC *PcControlRowsPc[2];
    extern i16 *PcControlTextIdsPc[45];
    extern char *PcControlTextPc[45];
    extern f32 PcControlBaselineAdjustPc;
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern u16 TextCrawlRotateXPc;
    extern u16 TextCrawlRotateYPc;
    extern u16 TextCrawlRotateZPc;
    extern f32 TextCrawlTimePc;
    extern f32 TextCrawlScaleXPc;
    extern f32 TextCrawlScaleYPc;
    extern f32 TextCrawlLineWidthPc;
    extern f32 TextCrawlLineSpacingPc;
    extern u32 TextCrawlRedPc;
    extern u32 TextCrawlGreenPc;
    extern u8 TextCrawlBluePc;
    struct text_crawl_view_pc_s;
    void MenuUpdateVideoPc(MENU *);
    void MenuUpdateEffectsPc(MENU *);
    void MenuApplyVideoPc(void);
    void MenuCancelVideoPc(void);
    void MenuUpdateVideoConfirmPc(MENU *);
    void MenuUpdateHiresNoticePc(MENU *);
    void MenuUpdateQuitPc(MENU *);
    char *TextButtonStringPc(i32, const char *, const char *);
#ifdef __cplusplus
}
#endif
