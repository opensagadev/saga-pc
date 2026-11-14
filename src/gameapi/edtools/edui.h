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

typedef struct FRAME_PAD_PC {
    i32 device;
    i32 slot;
    i32 stage;
    i32 id;
    i32 digital_left_x;
    i32 digital_left_y;
    i32 digital_right_x;
    i32 digital_right_y;
    f32 left_x_normal;
    f32 left_y_normal;
    f32 left_x_unknown;
    f32 left_y_unknown;
    f32 left_x_unknown2;
    f32 left_y_unknown2;
    f32 right_x_normal;
    f32 right_y_normal;
    f32 right_x_unknown;
    f32 right_y_unknown;
    f32 right_x_unknown2;
    f32 right_y_unknown2;
    u8 reserved50[0x20];
    u32 digital_buttons;
    u32 digital_buttons_prev;
    u32 digital_buttons_pressed;
    u32 digital_buttons_released;

    union {
        u32 flags;

        struct {
            i32 has_analog_sticks : 1;
            i32 has_analog_buttons : 1;
            i32 is_valid : 1;
            i32 must_have_analog_sticks : 1;
        };
    };

    u8 reserved84[4];
    u8 deadzone;
    u8 reserved89[7];
    u8 analog_right_x;
    u8 analog_right_y;
    u8 analog_left_x;
    u8 analog_left_y;
    u8 analog_left_pad_right;
    u8 analog_left_pad_left;
    u8 analog_left_pad_up;
    u8 analog_left_pad_down;
    u8 reserved98[4];
    u8 analog_l1;
    u8 analog_r1;
    u8 analog_l2;
    u8 analog_r2;
    u8 unknown_b0;
    u8 unknown_b1;
    u8 reserveda2[2];
} FRAME_PAD_PC;

STATIC_ASSERT(sizeof(FRAME_PAD_PC) == 0xa4);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, digital_left_x) == 0x10);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, left_x_normal) == 0x20);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, right_x_normal) == 0x38);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, digital_buttons) == 0x70);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, flags) == 0x80);

STATIC_ASSERT(offsetof(FRAME_PAD_PC, deadzone) == 0x88);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_right_x) == 0x90);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_left_pad_left) == 0x95);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_l1) == 0x9c);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, unknown_b0) == 0xa0);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, device) == 0x0);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, slot) == 0x4);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, stage) == 0x8);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, id) == 0xc);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, digital_left_y) == 0x14);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, digital_right_x) == 0x18);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, digital_right_y) == 0x1c);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, left_y_normal) == 0x24);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, left_x_unknown) == 0x28);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, left_y_unknown) == 0x2c);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, left_x_unknown2) == 0x30);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, left_y_unknown2) == 0x34);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, right_y_normal) == 0x3c);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, right_x_unknown) == 0x40);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, right_y_unknown) == 0x44);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, right_x_unknown2) == 0x48);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, right_y_unknown2) == 0x4c);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, digital_buttons_prev) == 0x74);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, digital_buttons_pressed) == 0x78);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, digital_buttons_released) == 0x7c);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_right_y) == 0x91);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_left_x) == 0x92);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_left_y) == 0x93);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_left_pad_right) == 0x94);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_left_pad_up) == 0x96);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_left_pad_down) == 0x97);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_r1) == 0x9d);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_l2) == 0x9e);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_r2) == 0x9f);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, unknown_b1) == 0xa1);
#ifdef __cplusplus
extern "C" {
#endif
    extern const f64 TerrainScanMargin10Pc;
    extern i32 nu_keyboard_previous_key_index;
    extern HINSTANCE PCInputWindow;
    i32 NuKeyboardPlatformStartup(void);
    void *NuAlloc(i32 size, const char *file, i32 line);
    STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_left_x) == 0x92);
    STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_left_y) == 0x93);
    extern NUKEYCHAR NuKeyChar[65];
    extern const char WorldPathSeparator[2];
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

struct eduiitem_s {
    struct eduiitem_s *next;
    struct eduiitem_s *previous;
    i32 type;

    union {
        i32 data;
        void *data_ptr;
        u8 unknown_0c[4];
    };

    u8 unknown_10;

    union {
        u8 flags;

        struct {
            u8 highlighted : 1;
            u8 disabled : 1;
            u8 unknown_flags : 6;
        };
    };

    i8 text_alignment;
    u8 unknown_13[5];
    i32 selection_group;
    i32 x;
    i32 y;
    char *text;
    i32 colours[4];
    i32 (*input)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2, u32 arg3);
    i32 (*process)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, f32 arg2, struct FRAME_PAD_PC *arg3);
    i32 (*render)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, i32 arg2, i32 arg3, i32 arg4, i32 arg5);
    void (*destroy)(struct eduimenu_s *arg0, struct eduiitem_s *arg1);
};

STATIC_ASSERT(sizeof(struct eduiitem_s) == 72);

struct edui_gradient_node_s {
    struct edui_gradient_node_s *next;
    struct edui_gradient_node_s *previous;
    f32 time;
    u32 colour;
    f32 hue;
    f32 saturation;
    f32 value;
    u32 metadata;
};

STATIC_ASSERT(sizeof(struct edui_gradient_node_s) == 32);

struct ed_module_s {
    struct ed_module_s *next;
    struct ed_module_s *previous;
    char *name;
    void (*init)(void);
    void (*close)(void);
    void (*activate)(void);
    void (*deactivate)(void);
    void (*apply)(void);
    void (*write)(i32 arg0);
    void (*read)(i32 arg0);
    u32 block_id;
    i32 (*process)(f32 arg0, struct FRAME_PAD_PC *arg1);
    void (*render)(void);
    void *reserved;
};

STATIC_ASSERT(sizeof(struct ed_module_s) == 56);

struct eduiiattr_s {
    u32 background;
    u32 text;
    u32 highlight;
    u32 disabled;
};

STATIC_ASSERT(sizeof(struct eduiiattr_s) == 16);
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
    extern void *edui_font;
    extern u32 eduimenu_outline_colour;
    extern u32 eduimenu_selected_outline_colour;
    struct eduiitem_s *eduiItemColourPickCreate(u32 arg0, void *arg1, void *arg2, char *arg3);
    struct eduiitem_s *eduiItemSelWithClipColourCreate(u32 data, void *colours, i32 selected, i32 group, void *callback,
                                                       char *text);
    extern struct edcam_s *gp_cam;
    extern i32 ed_init;
    void edmainClose(void);
    extern struct nucamera_s *edinternalcam;
    void eduiInit(void);
    extern char ed_levelfile[256];
    void eduiInitMaterials(void);
    void cbGradChange(void);
    struct eduiitem_s *eduiItemGradPickCreate(u32 data, void *colours, void *callback, char *text);
    struct eduiitem_s *eduiItemSliderCreate(u32 data, void *colours, i32 group, void *callback, f32 minimum,
                                            f32 maximum, f32 value, char *text);
    struct eduiitem_s *eduiItemSelCreate(u32 data, void *colours, i32 selected, i32 group, void *callback, char *text);
    struct eduiitem_s *eduiItemTextPickCreate(u32 data, void *colours, void *callback, char *text);
    struct eduiitem_s *eduiItemSliderCreateInt(u32 data, void *colours, i32 group, void *callback, i32 minimum,
                                               i32 maximum, i32 value, char *text);
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

struct eduimenu_s {
    struct eduiitem_s *first;
    struct eduiitem_s *last;
    struct eduiitem_s *selected;
    struct eduiitem_s *field_0c;
    struct eduiitem_s *field_10;
    i32 x;
    i32 y;
    i32 width;
    i32 height;
    i32 field_24;
    i32 field_28;
    void *font;
    void (*callback)(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    char *title;

    union {
        struct {
            u32 flags : 3;
            u32 unknown_flags : 29;
        };

        struct {
            u32 : 1;
            i32 horizontal : 1;
            u32 pinned : 1;
            u32 : 29;
        };
    };
    struct eduimenu_s *child;
    struct eduimenu_s *parent;
};

STATIC_ASSERT(sizeof(struct eduimenu_s) == 68);

struct edui_textpicker_s {
    struct eduiitem_s base;
    i32 (*interact)(struct edui_interact_s *arg0);
    char value[256];
    i32 keyboard_column;
    i32 keyboard_row;
    i32 cursor;
    i16 editing;
    i16 max_length;
    void (*callback)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    char *format;

    union {
        u32 keyboard_flags;
        u8 keyboard_flag_bits; /* the edfile.c keyboard handlers test the low byte */

        struct {
            u32 : 1;
            u32 extended_keyboard : 1; /* five keyboard rows and the copy/paste keys */
            u32 : 30;
        };
    };
};

STATIC_ASSERT(sizeof(struct edui_textpicker_s) == 360);

struct edui_texture_pick_s {
    struct eduiitem_s base;
    i32 (*interact)(struct edui_interact_s *arg0);
    f32 uv_x[2];
    f32 uv_y[2];
    void (*changed)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    struct numtl_s *material;
    i32 selected_corner;
    f32 zoom;
};

STATIC_ASSERT(sizeof(struct edui_texture_pick_s) == 108);

struct edui_graph_s {
    struct eduiitem_s base;
    i32 (*interact)(struct edui_interact_s *arg0);
    struct nugraph_s *graph;
    struct nugraph_s *onion_skins[8];
    void (*changed)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    f32 cursor_x;
    f32 cursor_y;
    i32 width;
    i32 height;
    f32 x_scale;
    f32 y_scale;
    i32 selected_point;
    char x_label[16];
    char y_label[16];
    char title[16];
};

STATIC_ASSERT(sizeof(struct edui_graph_s) == 192);

struct edui_slider_s {
    struct eduiitem_s base;
    i32 (*interact)(struct edui_interact_s *arg0);
    void (*changed)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    f32 normalized_value;
    f32 value;
    f32 minimum;
    f32 range;
    i32 change_timer;
    char *format;
    f32 granularity;
};

STATIC_ASSERT(sizeof(struct edui_slider_s) == 108);

struct edui_prop_s {
    struct eduiitem_s base;
    i32 (*interact)(struct edui_interact_s *arg0);
    u32 editing : 1;
    u32 dragging_label : 1;
    u32 divider_active : 1;
    u32 button_held : 1;
    u32 button_hover : 1;
    u32 button_latched : 1;
    u32 button_type : 3;
    u32 remaining_property_flags : 23;
    i32 button_colour;
    f32 label_width;
    f32 button_size;
    f32 button_x;
    f32 button_y;
    char *property_text;
    u8 unknown_68[4];
    i32 depth;
    void (*selected)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void (*button)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void (*changed)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    i32 extra_data;
};

STATIC_ASSERT(sizeof(struct edui_prop_s) == 128);

struct edui_colour_slider_s {
    struct edui_slider_s base;
    u8 red;
    u8 green;
    u8 blue;
    u8 unknown_6f;
};

STATIC_ASSERT(sizeof(struct edui_colour_slider_s) == 112);
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
    i32 eduicbProcessColourSlider(struct eduimenu_s *arg0, struct eduiitem_s *arg1, f32 arg2,
                                  struct FRAME_PAD_PC *arg3);
    i32 eduicbProcessSel(struct eduimenu_s *arg0, struct eduiitem_s *arg1, f32 arg2, struct FRAME_PAD_PC *arg3);
    i32 eduicbProcessSlider(struct eduimenu_s *arg0, struct eduiitem_s *arg1, f32 arg2, struct FRAME_PAD_PC *arg3);
    i32 eduicbRenderCheck(struct eduimenu_s *arg0, struct eduiitem_s *arg1, i32 arg2, i32 arg3, i32 arg4, i32 arg5);
    i32 eduicbRenderColourSlider(struct eduimenu_s *arg0, struct eduiitem_s *arg1, i32 arg2, i32 arg3, i32 arg4,
                                 i32 arg5);
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

struct edui_gradient_pick_s {
    struct eduiitem_s base;
    i32 (*interact)(struct edui_interact_s *arg0);
    struct edui_gradient_node_s *first_stage;
    struct edui_gradient_node_s *selected_stage;
    void (*changed)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void (*press)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void (*add)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void (*remove)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void (*copy)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void (*paste)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    i32 change_timer;
};

STATIC_ASSERT(sizeof(struct edui_gradient_pick_s) == 112);

struct edui_interact_s {
    f32 x;
    f32 y;
    f32 width;
    f32 height;
    struct eduimenu_s *menu;
    struct eduiitem_s *item;
    i32 (*callback)(struct edui_interact_s *arg0);
    u32 buttons;
    i32 field_20;
};

STATIC_ASSERT(sizeof(struct edui_interact_s) == 36);

struct edui_sel_s {
    struct eduiitem_s base;
    i32 (*interact)(struct edui_interact_s *arg0);
    void (*selected)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void (*held)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
};

STATIC_ASSERT(sizeof(struct edui_sel_s) == 84);

struct edui_file_pick_s {
    struct eduiitem_s base;
    i32 (*interact)(struct edui_interact_s *arg0);
    void (*changed)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    char *format;
    u16 reserved_54;
    u16 reopen_directory;
    char name[64];
    char directory[256];
    char filename[256];
    char filter[8];
    i32 (*compare_entries)(void *arg0, void *arg1);
};

STATIC_ASSERT(sizeof(struct edui_file_pick_s) == 676);

struct edui_colour_pick_s {
    struct eduiitem_s base;
    i32 (*interact)(struct edui_interact_s *arg0);
    f32 cursor_x;
    f32 cursor_y;
    f32 hue;
    f32 saturation;
    f32 value;
    f32 red;
    f32 green;
    f32 blue;

    union {
        u8 interaction_flags;

        struct {
            u8 dragging_colour : 1;
            u8 dragging_saturation : 1;
            u8 confirm : 1;
            u8 unknown_interaction_flags : 5;
        };
    };

    u8 unknown_6d[3];
    void (*changed)(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
};

STATIC_ASSERT(sizeof(struct edui_colour_pick_s) == 116);
#ifdef __cplusplus
extern "C" {
#endif
    struct edui_gradient_node_s *eduiGradStageAdd(struct edui_gradient_pick_s *item, f32 time, f32 hue, f32 saturation,
                                                  f32 value);
    struct eduiitem_s *eduiItemGreyGradPickCreate(size_t data, const void *colours, EdUiItemCallback callback,
                                                  char *text);
    struct eduiitem_s *eduiItemToggleCreate(size_t data, const void *colours, i32 selected, i32 group,
                                            EdUiItemCallback callback, char *text);
    struct eduiitem_s *eduiItemCheckCreate(size_t data, const void *colours, i32 selected, i32 group,
                                           EdUiItemCallback callback, char *text);
    i32 eduicbInteractSel(struct edui_interact_s *arg0);
    void eduiItemFilePickSetFmt(struct edui_file_pick_s *item, char *format);
    extern struct edui_interact_s *eduiInteractLocked;
    extern edui_interact_s eduiInteracts[64];
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
