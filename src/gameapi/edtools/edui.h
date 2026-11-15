/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_GAMEAPI_EDTOOLS_EDUI_H_PART
#define SAGA_GAMEAPI_EDTOOLS_EDUI_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nucore/nucore_plain_types.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_GAMEAPI_EDTOOLS_EDUI_H_PART == 1 && !defined(SAGA_GAMEAPI_EDTOOLS_EDUI_H_STOP)

struct edcam_s;
struct numtl_s;

typedef struct FRAME_PAD_PC {     // size 0xa4
    i32 device;                   // 0x00
    i32 slot;                     // 0x04
    i32 stage;                    // 0x08
    i32 id;                       // 0x0c
    i32 digital_left_x;           // 0x10
    i32 digital_left_y;           // 0x14
    i32 digital_right_x;          // 0x18
    i32 digital_right_y;          // 0x1c
    f32 left_x_normal;            // 0x20
    f32 left_y_normal;            // 0x24
    f32 left_x_unknown;           // 0x28
    f32 left_y_unknown;           // 0x2c
    f32 left_x_unknown2;          // 0x30
    f32 left_y_unknown2;          // 0x34
    f32 right_x_normal;           // 0x38
    f32 right_y_normal;           // 0x3c
    f32 right_x_unknown;          // 0x40
    f32 right_y_unknown;          // 0x44
    f32 right_x_unknown2;         // 0x48
    f32 right_y_unknown2;         // 0x4c
    u8 reserved50[0x20];          // 0x50
    u32 digital_buttons;          // 0x70
    u32 digital_buttons_prev;     // 0x74
    u32 digital_buttons_pressed;  // 0x78
    u32 digital_buttons_released; // 0x7c

    union { // size 0x4
        u32 flags;

        struct {                             // size 0x4
            i32 has_analog_sticks : 1;       // 0x80:0
            i32 has_analog_buttons : 1;      // 0x80:1
            i32 is_valid : 1;                // 0x80:2
            i32 must_have_analog_sticks : 1; // 0x80:3
        };
    }; // 0x80

    u8 reserved84[4];         // 0x84
    u8 deadzone;              // 0x88
    u8 reserved89[7];         // 0x89
    u8 analog_right_x;        // 0x90
    u8 analog_right_y;        // 0x91
    u8 analog_left_x;         // 0x92
    u8 analog_left_y;         // 0x93
    u8 analog_left_pad_right; // 0x94
    u8 analog_left_pad_left;  // 0x95
    u8 analog_left_pad_up;    // 0x96
    u8 analog_left_pad_down;  // 0x97
    u8 reserved98[4];         // 0x98
    u8 analog_l1;             // 0x9c
    u8 analog_r1;             // 0x9d
    u8 analog_l2;             // 0x9e
    u8 analog_r2;             // 0x9f
    u8 unknown_b0;            // 0xa0
    u8 unknown_b1;            // 0xa1
    u8 reserveda2[2];         // 0xa2
} FRAME_PAD_PC;

#ifdef __cplusplus
extern "C" {
#endif
    extern const f64 TerrainScanMargin10;
    extern i32 nu_keyboard_previous_key_index;
    extern HINSTANCE PCInputWindow;
    i32 NuKeyboardPlatformStartup(void);
    void *NuAlloc(i32 size, const char *file, i32 line);
    extern NUKEYCHAR NuKeyChar[65];
    extern const char WorldObjectAnimFormat[7];
    i32 PcConfigureInputGraphicsCaps(void);
#ifdef __cplusplus
}
#endif
struct eduimenu_s;
struct eduiitem_s;
typedef void (*EdUiMenuCallback)(struct eduimenu_s *menu, struct eduimenu_s *parent);
typedef void (*EdUiItemCallback)(struct eduimenu_s *menu, struct eduiitem_s *item, u32 value);
typedef struct ed_module_s ed_module_s;
typedef struct edui_colour_pick_s edui_colour_pick_s;
typedef struct edui_colour_slider_s edui_colour_slider_s;
typedef struct edui_file_pick_s edui_file_pick_s;
typedef struct edui_gradient_node_s edui_gradient_node_s;
typedef struct edui_gradient_pick_s edui_gradient_pick_s;
typedef struct edui_graph_s edui_graph_s;
typedef struct edui_interact_s edui_interact_s;
typedef struct edui_prop_s edui_prop_s;
typedef struct edui_sel_s edui_sel_s;
typedef struct edui_slider_s edui_slider_s;
typedef struct edui_textpicker_s edui_textpicker_s;
typedef struct edui_texture_pick_s edui_texture_pick_s;
typedef struct eduiiattr_s eduiiattr_s;
typedef struct eduiitem_s eduiitem_s;
typedef struct eduimenu_s eduimenu_s;

struct eduiitem_s {              // size 0x48
    struct eduiitem_s *next;     // 0x00
    struct eduiitem_s *previous; // 0x04
    i32 type;                    // 0x08

    union { // size 0x4
        i32 data;
        void *data_ptr;
        u8 unknown_0c[4];
    }; // 0x0c

    u8 unknown_10; // 0x10

    union { // size 0x1
        u8 flags;

        struct {                  // size 0x1
            u8 highlighted : 1;   // 0x11:0
            u8 disabled : 1;      // 0x11:1
            u8 unknown_flags : 6; // 0x11:2
        };
    }; // 0x11

    i8 text_alignment;                                                                                       // 0x12
    u8 unknown_13[5];                                                                                        // 0x13
    i32 selection_group;                                                                                     // 0x18
    i32 x;                                                                                                   // 0x1c
    i32 y;                                                                                                   // 0x20
    char *text;                                                                                              // 0x24
    i32 colours[4];                                                                                          // 0x28
    i32 (*input)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2, u32 arg3);                      // 0x38
    i32 (*process)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, f32 arg2, struct FRAME_PAD_PC *arg3);   // 0x3c
    i32 (*render)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, i32 arg2, i32 arg3, i32 arg4, i32 arg5); // 0x40
    void (*destroy)(struct eduimenu_s *arg0, struct eduiitem_s *arg1);                                       // 0x44
};

struct edui_gradient_node_s {              // size 0x20
    struct edui_gradient_node_s *next;     // 0x00
    struct edui_gradient_node_s *previous; // 0x04
    f32 time;                              // 0x08
    u32 colour;                            // 0x0c
    f32 hue;                               // 0x10
    f32 saturation;                        // 0x14
    f32 value;                             // 0x18
    u32 metadata;                          // 0x1c
};

struct ed_module_s {                                     // size 0x38
    struct ed_module_s *next;                            // 0x00
    struct ed_module_s *previous;                        // 0x04
    char *name;                                          // 0x08
    void (*init)(void);                                  // 0x0c
    void (*close)(void);                                 // 0x10
    void (*activate)(void);                              // 0x14
    void (*deactivate)(void);                            // 0x18
    void (*apply)(void);                                 // 0x1c
    void (*write)(i32 arg0);                             // 0x20
    void (*read)(i32 arg0);                              // 0x24
    u32 block_id;                                        // 0x28
    i32 (*process)(f32 arg0, struct FRAME_PAD_PC *arg1); // 0x2c
    void (*render)(void);                                // 0x30
    void *reserved;                                      // 0x34
};

struct eduiiattr_s { // size 0x10
    u32 background;  // 0x0
    u32 text;        // 0x4
    u32 highlight;   // 0x8
    u32 disabled;    // 0xc
};

#ifdef __cplusplus
extern "C" {
#endif
    extern char *FS_CurrentCursorPos;
    extern char *FS_CurrentPos;
    extern i32 FS_CurrentPosFileNum;
    extern i32 FS_CursorLine;
    extern char FS_FileList[65536];
    extern char *FS_FileListEnd;
    extern i32 FS_NumFiles;
    extern i32 FS_SortMode;
    extern f32 FS_Width;
    extern struct eduiitem_s *edui_last_item;
    struct eduiitem_s *eduiItemGreyPickCreate(u32 data, void *colours, void *callback, char *text);
    extern i32 edui_donotdraw;
    extern struct vufnt_s *edui_font;
    extern u32 eduimenu_outline_colour;
    extern u32 eduimenu_selected_outline_colour;
    struct eduiitem_s *eduiItemColourPickCreate(u32 arg0, void *arg1, void *arg2, char *arg3);
    struct eduiitem_s *eduiItemSelWithClipColourCreate(u32 data, void *colours, i32 selected, i32 group, void *callback, char *text);
    extern struct edcam_s *gp_cam;
    extern i32 ed_init;
    void edmainClose(void);
    extern struct nucamera_s *edinternalcam;
    void eduiInit(void);
    extern char ed_levelfile[256];
    void eduiInitMaterials(void);
    void cbGradChange(void);
    struct eduiitem_s *eduiItemGradPickCreate(u32 data, void *colours, void *callback, char *text);
    struct eduiitem_s *eduiItemSliderCreate(u32 data, void *colours, i32 group, void *callback, f32 minimum, f32 maximum, f32 value,
                                            char *text);
    struct eduiitem_s *eduiItemSelCreate(u32 data, void *colours, i32 selected, i32 group, void *callback, char *text);
    struct eduiitem_s *eduiItemTextPickCreate(u32 data, void *colours, void *callback, char *text);
    struct eduiitem_s *eduiItemSliderCreateInt(u32 data, void *colours, i32 group, void *callback, i32 minimum, i32 maximum, i32 value,
                                               char *text);
    i32 eduiItemSetText(struct eduiitem_s *item, char *text);
    extern char *textrow[4];
    extern i32 numInteracts;
    extern u32 edui_cursor_buttons;
    extern u32 edui_cursor_buttons_db;
    extern f32 edui_cursor_x;
    extern f32 edui_cursor_y;
    extern i32 selection_value;
    extern char FS_ParentEntry[8];
#ifdef __cplusplus
}
#endif

enum { EDUI_CURSOR_PRIMARY = 64 };

enum { EDUI_ITEM_HIGHLIGHTED = 1 };

#undef SAGA_GAMEAPI_EDTOOLS_EDUI_H_PART
#define SAGA_GAMEAPI_EDTOOLS_EDUI_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_EDTOOLS_EDUI_H_STOP
#endif
#endif

#if SAGA_GAMEAPI_EDTOOLS_EDUI_H_PART == 2 && !defined(SAGA_GAMEAPI_EDTOOLS_EDUI_H_STOP)

struct eduimenu_s {                                                     // size 0x44
    struct eduiitem_s *first;                                           // 0x00
    struct eduiitem_s *last;                                            // 0x04
    struct eduiitem_s *selected;                                        // 0x08
    struct eduiitem_s *unknown_0c;                                      // 0x0c
    struct eduiitem_s *unknown_10;                                      // 0x10
    i32 x;                                                              // 0x14
    i32 y;                                                              // 0x18
    i32 width;                                                          // 0x1c
    i32 height;                                                         // 0x20
    i32 unknown_24;                                                     // 0x24
    i32 unknown_28;                                                     // 0x28
    void *font;                                                         // 0x2c
    void (*callback)(struct eduimenu_s *arg0, struct eduimenu_s *arg1); // 0x30
    char *title;                                                        // 0x34

    union { // size 0x4

        struct {                    // size 0x4
            u32 flags : 3;          // 0x38:0
            u32 unknown_flags : 29; // 0x38:3
        };

        struct {                // size 0x4
            u32 : 1;            // 0x38:0
            i32 horizontal : 1; // 0x38:1
            u32 pinned : 1;     // 0x38:2
            u32 : 29;           // 0x38:3
        };
    }; // 0x38
    struct eduimenu_s *child;  // 0x3c
    struct eduimenu_s *parent; // 0x40
};

struct edui_textpicker_s {                                                        // size 0x168
    struct eduiitem_s base;                                                       // 0x000
    i32 (*interact)(struct edui_interact_s *arg0);                                // 0x048
    char value[256];                                                              // 0x04c
    i32 keyboard_column;                                                          // 0x14c
    i32 keyboard_row;                                                             // 0x150
    i32 cursor;                                                                   // 0x154
    i16 editing;                                                                  // 0x158
    i16 max_length;                                                               // 0x15a
    void (*callback)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2); // 0x15c
    char *format;                                                                 // 0x160

    union { // size 0x4
        u32 keyboard_flags;
        u8 keyboard_flag_bits; /* the edfile.c keyboard handlers test the low byte */

        struct {                                                                        // size 0x4
            u32 : 1;                                                                    // 0x164:0
            u32 extended_keyboard : 1; /* five keyboard rows and the copy/paste keys */ // 0x164:1
            u32 : 30;                                                                   // 0x164:2
        };
    }; // 0x164
};

struct edui_texture_pick_s {                                                     // size 0x6c
    struct eduiitem_s base;                                                      // 0x00
    i32 (*interact)(struct edui_interact_s *arg0);                               // 0x48
    f32 uv_x[2];                                                                 // 0x4c
    f32 uv_y[2];                                                                 // 0x54
    void (*changed)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2); // 0x5c
    struct numtl_s *material;                                                    // 0x60
    i32 selected_corner;                                                         // 0x64
    f32 zoom;                                                                    // 0x68
};

struct edui_graph_s {                                                            // size 0xc0
    struct eduiitem_s base;                                                      // 0x00
    i32 (*interact)(struct edui_interact_s *arg0);                               // 0x48
    struct nugraph_s *graph;                                                     // 0x4c
    struct nugraph_s *onion_skins[8];                                            // 0x50
    void (*changed)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2); // 0x70
    f32 cursor_x;                                                                // 0x74
    f32 cursor_y;                                                                // 0x78
    i32 width;                                                                   // 0x7c
    i32 height;                                                                  // 0x80
    f32 x_scale;                                                                 // 0x84
    f32 y_scale;                                                                 // 0x88
    i32 selected_point;                                                          // 0x8c
    char x_label[16];                                                            // 0x90
    char y_label[16];                                                            // 0xa0
    char title[16];                                                              // 0xb0
};

struct edui_slider_s {                                                           // size 0x6c
    struct eduiitem_s base;                                                      // 0x00
    i32 (*interact)(struct edui_interact_s *arg0);                               // 0x48
    void (*changed)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2); // 0x4c
    f32 normalized_value;                                                        // 0x50
    f32 value;                                                                   // 0x54
    f32 minimum;                                                                 // 0x58
    f32 range;                                                                   // 0x5c
    i32 change_timer;                                                            // 0x60
    char *format;                                                                // 0x64
    f32 granularity;                                                             // 0x68
};

struct edui_prop_s {                                                              // size 0x80
    struct eduiitem_s base;                                                       // 0x00
    i32 (*interact)(struct edui_interact_s *arg0);                                // 0x48
    u32 editing : 1;                                                              // 0x4c:0
    u32 dragging_label : 1;                                                       // 0x4c:1
    u32 divider_active : 1;                                                       // 0x4c:2
    u32 button_held : 1;                                                          // 0x4c:3
    u32 button_hover : 1;                                                         // 0x4c:4
    u32 button_latched : 1;                                                       // 0x4c:5
    u32 button_type : 3;                                                          // 0x4c:6
    u32 remaining_property_flags : 23;                                            // 0x4c:9
    i32 button_colour;                                                            // 0x50
    f32 label_width;                                                              // 0x54
    f32 button_size;                                                              // 0x58
    f32 button_x;                                                                 // 0x5c
    f32 button_y;                                                                 // 0x60
    char *property_text;                                                          // 0x64
    u8 unknown_68[4];                                                             // 0x68
    i32 depth;                                                                    // 0x6c
    void (*selected)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2); // 0x70
    void (*button)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);   // 0x74
    void (*changed)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);  // 0x78
    i32 extra_data;                                                               // 0x7c
};

struct edui_colour_slider_s {  // size 0x70
    struct edui_slider_s base; // 0x00
    u8 red;                    // 0x6c
    u8 green;                  // 0x6d
    u8 blue;                   // 0x6e
    u8 unknown_6f;             // 0x6f
};

#ifdef __cplusplus
extern "C" {
#endif
    struct eduiitem_s *eduiMenuAddItem(struct eduimenu_s *menu, struct eduiitem_s *item);
    struct eduimenu_s *eduiMenuCreate(i32 x, i32 y, i32 width, i32 height, void *font, void *callback, char *title);
    void eduiMenuAddItemFirst(struct eduimenu_s *menu, struct eduiitem_s *item);
    struct eduimenu_s *eduiGetActiveMenu(void);
    void eduicbPickCFGCancel(struct eduimenu_s *menu, struct eduimenu_s *arg1);
    void cbgpcfgAdd(struct eduimenu_s *menu, struct eduiitem_s *item, u32 value);
    void cbgpcfgCPPress(struct eduimenu_s *menu, struct eduiitem_s *item, u32 value);
    void cbgpcfgCopy(struct eduimenu_s *menu, struct eduiitem_s *item, u32 value);
    void cbgpcfgDel(struct eduimenu_s *menu, struct eduiitem_s *item, u32 value);
    void cbgpcfgPaste(struct eduimenu_s *menu, struct eduiitem_s *item, u32 value);
    extern struct eduimenu_s *edui_messagemenu;
    void eduiItemSliderSetFmt(struct edui_slider_s *item, char *format);
    void eduiItemSliderSetGranularity(struct edui_slider_s *item, f32 granularity);
    void cbColourPickSel(struct eduimenu_s *menu, struct eduiitem_s *arg1, u32 arg2);
    void cbmcfgCancel(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void eduiMenuFitWidth(struct eduimenu_s *menu, i32 padding);
    void cbMMEditorConfig(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void eduiMenuHighlight(struct eduimenu_s *menu, struct eduiitem_s *item);
    void cbSel(struct eduimenu_s *arg0, struct eduiitem_s *item);
    void eduiMenuRender(struct eduimenu_s *menu);
    void cbSubMenu(struct eduimenu_s *menu, struct eduiitem_s *item);
    extern struct eduimenu_s *default_active_menu;
    i32 eduiMenuProcessAux(struct eduimenu_s *menu, f32 delta_time, struct FRAME_PAD_PC *pad);
    i32 eduiProcessInteracts(struct eduimenu_s *menu, struct FRAME_PAD_PC *pad);
    extern struct eduimenu_s *submenu0;
    void eduiCreateMessageMenu(struct eduimenu_s *parent, char *message, i32 highlighted);
    i32 eduiMenuAttach(struct eduimenu_s *menu, struct eduimenu_s *child);
    i32 eduiMenuProcess(struct eduimenu_s *menu, f32 delta_time, struct FRAME_PAD_PC *pad);
    void eduiItemSliderSetVal(struct edui_slider_s *item, f32 value);
    void eduicbItemSliderDestroy(struct eduimenu_s *arg0, struct eduiitem_s *arg1);
    i32 eduicbProcessColourSlider(struct eduimenu_s *arg0, struct eduiitem_s *arg1, f32 arg2, struct FRAME_PAD_PC *arg3);
    i32 eduicbProcessSel(struct eduimenu_s *arg0, struct eduiitem_s *arg1, f32 arg2, struct FRAME_PAD_PC *arg3);
    i32 eduicbProcessSlider(struct eduimenu_s *arg0, struct eduiitem_s *arg1, f32 arg2, struct FRAME_PAD_PC *arg3);
    i32 eduicbRenderCheck(struct eduimenu_s *arg0, struct eduiitem_s *arg1, i32 arg2, i32 arg3, i32 arg4, i32 arg5);
    i32 eduicbRenderColourSlider(struct eduimenu_s *arg0, struct eduiitem_s *arg1, i32 arg2, i32 arg3, i32 arg4, i32 arg5);
    i32 eduicbRenderSel(struct eduimenu_s *arg0, struct eduiitem_s *arg1, i32 arg2, i32 arg3, i32 arg4, i32 arg5);
    i32 eduicbRenderSlider(struct eduimenu_s *arg0, struct eduiitem_s *arg1, i32 arg2, i32 arg3, i32 arg4, i32 arg5);
    i32 eduicbRenderSliderInt(struct eduimenu_s *arg0, struct eduiitem_s *arg1, i32 arg2, i32 arg3, i32 arg4, i32 arg5);
    void eduicbItemTextPickDestroy(struct eduimenu_s *arg0, struct eduiitem_s *arg1);
    i32 eduicbProcessTextPick(struct eduimenu_s *arg0, struct eduiitem_s *arg1, f32 arg2, struct FRAME_PAD_PC *arg3);
    i32 eduicbRenderTextPick(struct eduimenu_s *arg0, struct eduiitem_s *arg1, i32 arg2, i32 arg3, i32 arg4, i32 arg5);
    void eduiItemTextPickSetFmt(struct edui_textpicker_s *item, char *format);
    void eduicbItemFilePickDestroy(struct eduimenu_s *arg0, struct eduiitem_s *arg1);
    i32 eduicbProcessFilePick(struct eduimenu_s *arg0, struct eduiitem_s *arg1, f32 arg2, struct FRAME_PAD_PC *arg3);
    i32 eduicbRenderFilePick(struct eduimenu_s *arg0, struct eduiitem_s *arg1, i32 arg2, i32 arg3, i32 arg4, i32 arg5);
    void eduiMenuDestroyItems(struct eduimenu_s *menu);
    void eduiMenuDestroy(struct eduimenu_s *menu);
    i32 eduiMenuDetach(struct eduimenu_s *menu);
    i32 eduiMenuProcessInput(struct eduimenu_s *menu, f32 delta_time, struct FRAME_PAD_PC *pad, i32 item_result);
    void eduicbMenuCloseAllexpanders(struct eduimenu_s *menu);
    void cbInteractMenuKeySelect(struct eduimenu_s *menu);
    i32 eduiCheckForPadMenuCancel(struct eduimenu_s *menu, struct FRAME_PAD_PC *pad);
    void eduicbItemGradPickDestroy(struct eduimenu_s *arg0, struct eduiitem_s *arg1);
    i32 eduicbRenderGradPick(struct eduimenu_s *arg0, struct eduiitem_s *arg1, i32 arg2, i32 arg3, i32 arg4, i32 arg5);
    void cbInteractMenuScrollTo(struct eduimenu_s *menu, char *text);
    void eduicbMenuOpenAllexpanders(struct eduimenu_s *menu);
#ifdef __cplusplus
}
#endif
#undef SAGA_GAMEAPI_EDTOOLS_EDUI_H_PART
#define SAGA_GAMEAPI_EDTOOLS_EDUI_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_EDTOOLS_EDUI_H_STOP
#endif
#endif

#if SAGA_GAMEAPI_EDTOOLS_EDUI_H_PART == 3 && !defined(SAGA_GAMEAPI_EDTOOLS_EDUI_H_STOP)

struct edui_gradient_pick_s {                                                    // size 0x70
    struct eduiitem_s base;                                                      // 0x00
    i32 (*interact)(struct edui_interact_s *arg0);                               // 0x48
    struct edui_gradient_node_s *first_stage;                                    // 0x4c
    struct edui_gradient_node_s *selected_stage;                                 // 0x50
    void (*changed)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2); // 0x54
    void (*press)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);   // 0x58
    void (*add)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);     // 0x5c
    void (*remove)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);  // 0x60
    void (*copy)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);    // 0x64
    void (*paste)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);   // 0x68
    i32 change_timer;                                                            // 0x6c
};

struct edui_interact_s {                           // size 0x24
    f32 x;                                         // 0x00
    f32 y;                                         // 0x04
    f32 width;                                     // 0x08
    f32 height;                                    // 0x0c
    struct eduimenu_s *menu;                       // 0x10
    struct eduiitem_s *item;                       // 0x14
    i32 (*callback)(struct edui_interact_s *arg0); // 0x18
    u32 buttons;                                   // 0x1c
    i32 unknown_20;                                // 0x20
};

struct edui_sel_s {                                                               // size 0x54
    struct eduiitem_s base;                                                       // 0x00
    i32 (*interact)(struct edui_interact_s *arg0);                                // 0x48
    void (*selected)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2); // 0x4c
    void (*held)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);     // 0x50
};

struct edui_file_pick_s {                                                        // size 0x2a4
    struct eduiitem_s base;                                                      // 0x000
    i32 (*interact)(struct edui_interact_s *arg0);                               // 0x048
    void (*changed)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2); // 0x04c
    char *format;                                                                // 0x050
    u16 reserved_54;                                                             // 0x054
    u16 reopen_directory;                                                        // 0x056
    char name[64];                                                               // 0x058
    char directory[256];                                                         // 0x098
    char filename[256];                                                          // 0x198
    char filter[8];                                                              // 0x298
    i32 (*compare_entries)(void *arg0, void *arg1);                              // 0x2a0
};

struct edui_colour_pick_s {                        // size 0x74
    struct eduiitem_s base;                        // 0x00
    i32 (*interact)(struct edui_interact_s *arg0); // 0x48
    f32 cursor_x;                                  // 0x4c
    f32 cursor_y;                                  // 0x50
    f32 hue;                                       // 0x54
    f32 saturation;                                // 0x58
    f32 value;                                     // 0x5c
    f32 red;                                       // 0x60
    f32 green;                                     // 0x64
    f32 blue;                                      // 0x68

    union { // size 0x1
        u8 interaction_flags;

        struct {                              // size 0x1
            u8 dragging_colour : 1;           // 0x6c:0
            u8 dragging_saturation : 1;       // 0x6c:1
            u8 confirm : 1;                   // 0x6c:2
            u8 unknown_interaction_flags : 5; // 0x6c:3
        };
    }; // 0x6c

    u8 unknown_6d[3];                                                            // 0x6d
    void (*changed)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2); // 0x70
};

#ifdef __cplusplus
extern "C" {
#endif
    struct edui_gradient_node_s *eduiGradStageAdd(struct edui_gradient_pick_s *item, f32 time, f32 hue, f32 saturation, f32 value);
    struct eduiitem_s *eduiItemGreyGradPickCreate(size_t data, const void *colours, EdUiItemCallback callback, char *text);
    struct eduiitem_s *eduiItemToggleCreate(size_t data, const void *colours, i32 selected, i32 group, EdUiItemCallback callback,
                                            char *text);
    struct eduiitem_s *eduiItemCheckCreate(size_t data, const void *colours, i32 selected, i32 group, EdUiItemCallback callback,
                                           char *text);
    i32 eduicbInteractSel(struct edui_interact_s *arg0);
    void eduiItemFilePickSetFmt(struct edui_file_pick_s *item, char *format);
    extern struct edui_interact_s *eduiInteractLocked;
    extern edui_interact_s eduiInteracts[64];
    eduiitem_s *eduiItemExpanderCreate(u32 data, const void *colours, void *callback, char *text);
    eduiitem_s *eduiItemFilterCreate(u32 data, void *colours, char *text, char *value);
    eduiitem_s *eduiItemPropCreate(u32 data, void *colours, void *selected, void *changed, void *button, i32 button_type, char *text,
                                   char *value);
    eduiitem_s *eduiItemPropCreateEx(u32 data, void *colours, void *selected, void *changed, void *button, i32 button_type, char *text,
                                     char *value, i32 extra_data);
    eduiitem_s *eduiItemSeparatorCreate(u32 data, const void *colours);
    eduimenu_s *eduiGetActiveMenuParent(void);
    extern i32 eduiPropTextPos;
    f32 eduiGetAnalougePadValue(FRAME_PAD_PC *pad);
    i32 eduiGetCameraEnabled(void);
    i32 eduiItemPropSetText(edui_prop_s *item, char *text);
    i32 eduiUsedAlgPad(FRAME_PAD_PC *pad);
    void eduiFlushInteracts(void);
    void eduiGetCursorCoords(f32 *x, f32 *y);
    void eduiGetCursorDelta(f32 *x, f32 *y);
    void eduiItemExpanderAddChild(eduiitem_s *item, eduiitem_s *child);
    void eduiMenuFitOnScreen(eduimenu_s *menu, i32 padding);
    void eduiMenuRemoveItem(eduimenu_s *menu, eduiitem_s *item);
    void eduiMenuSetAttr(eduimenu_s *menu, const u32 *colours);
    void eduiMenuSortItemsByTxt(eduimenu_s *menu);
    void eduiProcessCursor(f32 delta_time, FRAME_PAD_PC *pad);
    void eduiRenderCursor(void);
    void eduiSetActiveMenu(eduimenu_s *menu);
    void eduiSetCursorCoords(f32 x, f32 y);
    void eduiSetDefaultActiveMenu(eduimenu_s *menu);
    void eduiSetUsingMenuFocus(i32 enabled);
#ifdef __cplusplus
}
#endif
#undef SAGA_GAMEAPI_EDTOOLS_EDUI_H_PART
#define SAGA_GAMEAPI_EDTOOLS_EDUI_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_GAMEAPI_EDTOOLS_EDUI_H_STOP
#endif
#endif

#undef SAGA_GAMEAPI_EDTOOLS_EDUI_H_STOP
