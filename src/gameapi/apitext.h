#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/gui/apimenu.h"

struct FRAME_PAD;
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
    extern i32 TextPrintMode;
    extern i32 create_qfont2dlower;
    extern i32 create_qfont2dz;
    extern i32 create_qfont3d;
    extern i32 create_qfont3dz;
    extern struct vufnt_s *QFont2DZ;
    extern struct vufnt_s *QFont3D;
    void UnloadGameFont(i32 unused);
    extern struct vufnt_s *QFont2DLower;
    extern f32 SmartTextDefaultWidth;
    extern f32 SmartTextLongestWidth;
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

/* Private PC contract proposal. */
struct PC_CONTROL_ITEM { // size 0x8
    u8 kind;             // 0x0
    u8 command;          // 0x1
    u8 label;            // 0x2

    union { // size 0x1
        u8 flags;

        struct {                     // size 0x1
            u8 port_mode : 3;        // 0x3:0
            u8 flag_08 : 1;          // 0x3:3
            u8 flag_10 : 1;          // 0x3:4
            u8 unbound_player1 : 1;  // 0x3:5
            u8 unbound_player2 : 1;  // 0x3:6
            u8 unbound_selected : 1; // 0x3:7
        };
    }; // 0x3

    f32 pulse; // 0x4
};

/* PC_CONTROL_ITEM.flags: the bits of the bit-field above (PC_CONTROL_SCREEN sets them from the players' ports). */
#define PC_CONTROL_ITEM_NO_KEYBOARD 0x8       /* not offered on a keyboard port */
#define PC_CONTROL_ITEM_UNBOUND_PLAYER1 0x20  /* not available to player 1's port */
#define PC_CONTROL_ITEM_UNBOUND_PLAYER2 0x40  /* not available to player 2's port */
#define PC_CONTROL_ITEM_UNBOUND_SELECTED 0x80 /* not available to the selected player */

struct PC_CONTROL_ROW {     // size 0xc
    i32 count;              // 0x0
    i32 selected;           // 0x4
    PC_CONTROL_ITEM *items; // 0x8
};

struct PC_CONTROL_TEXT { // size 0x128
    char text[0x100];    // 0x000
    f32 fade;            // 0x100
    f32 x;               // 0x104
    f32 y;               // 0x108
    f32 z;               // 0x10c
    f32 x_scale;         // 0x110
    f32 y_scale;         // 0x114
    f32 wrap_width;      // 0x118
    u32 colour;          // 0x11c
    u32 key;             // 0x120
    i8 alignment;        // 0x124
    u8 refreshed;        // 0x125
    u8 unknown_126[2];   // 0x126
};

struct PC_CONTROL_CONFIG {                  // size 0x14
    struct vufnt_s *font;                   // 0x00
    void (*draw_background)(f32, f32, f32); // 0x04
    PC_CONTROL_ITEM **rows;                 // 0x08
    void (*play_sound)(i32);                // 0x0c
    const char *blocked_keys;               // 0x10
};

/* PC_CONTROL_SCREEN.phase: the fade of PC_CONTROL_SCREEN::Update */
#define PC_CONTROL_CLOSED 0
#define PC_CONTROL_FADE_IN 1
#define PC_CONTROL_OPEN 2
#define PC_CONTROL_FADE_OUT 3

struct PC_CONTROL_SCREEN {                  // size 0xec4
    i32 result;                             // 0x000
    i32 player_count;                       // 0x004
    u8 drop_in;                             // 0x008
    u8 active;                              // 0x009
    u8 initialized;                         // 0x00a
    u8 unknown_0b;                          // 0x00b
    f32 opacity;                            // 0x00c
    f32 pulse_level;                        // 0x010
    f32 fade;                               // 0x014
    i32 phase;                              // 0x018
    i32 row_count;                          // 0x01c
    i32 selected_row;                       // 0x020
    i32 capture_state;                      // 0x024
    i32 release_frames;                     // 0x028
    i32 release_reset;                      // 0x02c
    f32 x_scale;                            // 0x030
    f32 y_scale;                            // 0x034
    i32 selected_player;                    // 0x038
    i32 ports[4];                           // 0x03c
    i32 original_ports[4];                  // 0x04c
    i32 saved_ports[4];                     // 0x05c
    u32 config_hash;                        // 0x06c
    u8 missing_binding;                     // 0x070
    u8 unknown_71[3];                       // 0x071
    void (*draw_background)(f32, f32, f32); // 0x074
    f32 deadzone[4];                        // 0x078
    f32 saved_deadzone[4];                  // 0x088
    f32 keyboard_deadzone;                  // 0x098
    f32 saved_keyboard_deadzone;            // 0x09c
    struct vufnt_s *font;                   // 0x0a0
    PC_CONTROL_ROW rows[4];                 // 0x0a4
    u8 unknown_d4[4];                       // 0x0d4
    PC_CONTROL_TEXT text[12];               // 0x0d8
    f32 elapsed;                            // 0xeb8
    struct numtl_s *material;               // 0xebc
    void (*play_sound)(i32);                // 0xec0
    i32 ItemAllowsBinding(PC_CONTROL_ROW *, i32, i32);
    void AdvanceSelection(PC_CONTROL_ROW *);
    i32 CountPlayerRows(PC_CONTROL_ROW *);
    u32 ScaleAlpha(f32, u32);
    u32 ScaleColour(f32, f32, u32);
    u32 HashString(const char *);
    u8 PortAllowed(i32, i32, i32);
    u8 HasMissingBindings(void);
    void RefreshItems(void);
    void Register(PC_CONTROL_CONFIG *, VARIPTR *, VARIPTR *);
    void Initialize(i32, i32, i32, u8);
    i32 HashConfiguration(void);
    void SaveConfiguration(void);
    i32 WaitForRelease(void);
    i32 Update(struct FRAME_PAD *, struct FRAME_PAD *, f32);
    void DrawDeadzone(f32, f32, f32, f32, f32, f32, f32, f32);
    void QueueText(char *, f32, f32, f32, i32, f32, f32, u32, f32, f32, u32);
    f32 DrawText(char *, f32, f32, f32, i32, f32, f32, u32, i32, f32);
    void DrawQueuedText(f32);
    i32 IsScreenActive(void);
    i32 GetSavedPort(i32);
    void SetPlaySoundFn(void (*)(i32));
    PC_CONTROL_SCREEN *InitDefaults(void);
    void BeginFadeOut(i32);
    void PlaySoundCallback(i32);
    void ResetReleaseFrames(void);
    void DrawBarMeter(f32, f32, f32, f32, f32, f32, f32, f32);
    void ClearTextKeys(void);

    void Draw(void);
};

#ifdef __cplusplus
extern "C" {
#endif
    extern PC_CONTROL_SCREEN PcControlScreen;
    extern PC_CONTROL_ITEM PcControlItems[25];
    extern PC_CONTROL_ITEM *PcControlRows[2];
    extern i16 *PcControlTextIds[45];
    extern char *PcControlText[45];
    extern f32 PcControlBaselineAdjust;
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern u16 TextCrawlRotateX;
    extern u16 TextCrawlRotateY;
    extern u16 TextCrawlRotateZ;
    extern f32 TextCrawlTime;
    extern f32 TextCrawlScaleX;
    extern f32 TextCrawlScaleY;
    extern f32 TextCrawlLineWidth;
    extern f32 TextCrawlLineSpacing;
    extern u32 TextCrawlRed;
    extern u32 TextCrawlGreen;
    extern u8 TextCrawlBlue;
/* The rows of the video and effects option menus (MenuUpdateVideo, MenuUpdateEffects) */
#define MENU_VIDEO_ROW_RESOLUTION 0
#define MENU_VIDEO_ROW_REFRESH 1
#define MENU_VIDEO_ROW_ASPECT 2
#define MENU_VIDEO_ROW_HIRES 3
#define MENU_VIDEO_ROW_APPLY 4
#define MENU_EFFECTS_ROW_MULTISAMPLE 0
#define MENU_EFFECTS_ROW_SHADOWS 1
#define MENU_EFFECTS_ROW_BLOOM 2
#define MENU_EFFECTS_ROW_SPEED_BLUR 3
#define MENU_EFFECTS_ROW_APPLY 4
    void MenuUpdateVideo(MENU *);
    void MenuUpdateEffects(MENU *);
    void MenuApplyVideo(void);
    void MenuCancelVideo(void);
    void MenuUpdateVideoConfirm(MENU *);
    void MenuUpdateHiresNotice(MENU *);
    void MenuUpdateQuit(MENU *);
    char *TextButtonString(i32, const char *, const char *);
    extern f32 PcControlRepeat[2];
    extern i32 PcControlPulseAngle;
    extern u32 PcControlButtons;
    extern u32 PcControlHeld[2];
    extern u32 PcControlPressed[2];
    i32 PcControlWrapText(char *, f32);
    void PcControlFontScale(struct vufnt_s *, f32, f32, f32 *, f32 *);
    i32 PC_CtrlConfigGetPlayerPort(i32 player_index);
    i32 PcControlUpdate(struct FRAME_PAD *, struct FRAME_PAD *, f32);
    void PcControlDraw(void);
    void PcControlInitialize(i32, i32, i32, u8);
#ifdef __cplusplus
}
#endif
