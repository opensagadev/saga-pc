#include "gameapi/edtools/edui.h"
#include "nu2api/nu3d/nurndr.h"
#include "gameapi/apisave.h"
#include "gameapi/edmain.h"
#include "gameapi/edtools/edanim_internal.h"
#include "gameapi/edtools/edcam.h"
#include "gameapi/edtools/edfile.h"
#include "gameapi/edtools/edrender.h"
#include "gameapi/edtools/edtoolsall.h"
#include "legoapi/core/config/fileselect.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/render/light/lighting.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuprim.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nucore/nucore_plain_types.h"
#include "nu2api/nucore/nukeyboard.h"
#include "nu2api/nucore/numouse.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile/nufilepak.h"
#include "nu2api/nufile_pc.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nustencilshadows_PC.h"
#include "legoapi/render/core/render.h"
#include "gameapi/part.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/PCInput.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/nucore/nutime.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/nuwin32/d3dApiCalls.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include <string.h>

#ifdef __cplusplus
#endif
extern "C" {
    static eduimenu_s *EduiDeepestChild(eduimenu_s *menu);
    extern struct FILE_DIRECTORY_ENTRY *dir_list;
    extern f32 EduiScaleX;
    extern f32 EduiScaleY;
    void edmainRender(void);
    void eduiRenderInteracts(void);
    i32 eduicbProcessGradPickItem(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD *pad);
    static void EduiClearInteractions(eduimenu_s *menu, eduiitem_s *item);
    extern NUMTL *uimtls[5];
    f32 edui_cursor_dx;
    f32 edui_cursor_dy;
    u32 edui_cursor_buttons_old;
    i32 edui_cursor_locked;
}

typedef struct EDUI_EXPANDER {                                        // size 0x6c
    eduiitem_s base;                                                  // 0x00
    i32 (*interact)(edui_interact_s *interact);                       // 0x48
    eduiitem_s *first_child;                                          // 0x4c
    eduiitem_s *last_child;                                           // 0x50
    u32 open : 1;                                                     // 0x54:0
    u32 hover : 1;                                                    // 0x54:1
    u32 unknown_flags : 30;                                           // 0x54:2
    f32 button_size;                                                  // 0x58
    f32 button_x;                                                     // 0x5c
    f32 button_y;                                                     // 0x60
    i32 depth;                                                        // 0x64
    void (*changed)(eduimenu_s *menu, eduiitem_s *item, u32 buttons); // 0x68
} EDUI_EXPANDER;

extern "C" {
    extern i32 EduiSelectionMaterial;
}

/* A filter item: typing into its value hides every menu item whose text
 * does not contain it; hidden items wait on the filter's own child list. */
typedef struct edui_filter_s { // size 0x8c
    edui_prop_s prop;          // 0x00
    eduiitem_s *first_child;   // 0x80
    i32 reserved_84;           // 0x84
    u32 sorted;                // 0x88
} edui_filter_s;

extern "C" {
    extern char *edui_textrow_default[4];
    extern char *edui_textrow_lower[4];
    static void EduiDrawDisabledItem(eduiitem_s *item, i32 x, i32 y, i32 width, i32 height);
}

/* FILE_DIRECTORY_ENTRY.flags: the entry is a directory (listed as <name>, opened on selection). */
enum FILE_DIRECTORY_FLAG { FILE_DIRECTORY_FLAG_DIRECTORY = 8 };

typedef struct FS_PACKED_ENTRY_HEADER { // size 0x7
    u8 kind;                            // 0x0
    u8 encoded_second;                  // 0x1
    u8 encoded_minute;                  // 0x2
    u8 encoded_hour;                    // 0x3
    u8 encoded_day;                     // 0x4
    u8 encoded_month;                   // 0x5
    u8 encoded_year;                    // 0x6
} FS_PACKED_ENTRY_HEADER;

/* An editor item hosting the file selector; the layout copies the colour
 * picker's, whose cursor and colour defaults the create call still sets. */
typedef struct EDUI_FILE_SELECT {                                     // size 0x64
    eduiitem_s base;                                                  // 0x00
    i32 (*interact)(edui_interact_s *interact);                       // 0x48
    f32 cursor_x;                                                     // 0x4c
    f32 cursor_y;                                                     // 0x50
    f32 hue;                                                          // 0x54
    f32 saturation;                                                   // 0x58
    f32 value;                                                        // 0x5c
    void (*changed)(eduimenu_s *menu, eduiitem_s *item, u32 buttons); // 0x60
} EDUI_FILE_SELECT;

extern "C" NUMTL *uimtls[5];
extern "C" f32 EduiScaleX;
extern "C" f32 EduiScaleY;
extern "C" i32 EduiSelectionMaterial;
static void EduiDrawGradientSpan(i32 x, i32 y, i32 width, i32 height, u32 first_colour, u32 second_colour, f32 start, f32 end);
static void eduiHSVToRGB(f32 hue, f32 saturation, f32 value, f32 &red, f32 &green, f32 &blue);

struct edui_text_selector_s { // size 0x70
    edui_slider_s slider;     // 0x00
    char **options;           // 0x6c
};

struct edui_separator_s {               // size 0x4c
    eduiitem_s base;                    // 0x00
    i32 (*interact)(edui_interact_s *); // 0x48
};

struct EDUI_COLOUR_CLIPBOARD { // size 0x10
    f32 hue;                   // 0x0
    f32 saturation;            // 0x4
    f32 value;                 // 0x8
    f32 gray;                  // 0xc
};

typedef union edui_gradient_colour_u { // size 0x4
    u32 word;
    u8 channel[4];
} EDUI_GRADIENT_COLOUR;

typedef struct edui_colour_pick_flags_s { // size 0x74
    eduiitem_s base;                      // 0x00
    i32 (*interact)(edui_interact_s *);   // 0x48
    f32 cursor_x;                         // 0x4c
    f32 cursor_y;                         // 0x50
    f32 hue;                              // 0x54
    f32 saturation;                       // 0x58
    f32 value;                            // 0x5c
    f32 red;                              // 0x60
    f32 green;                            // 0x64
    f32 blue;                             // 0x68

    union { // size 0x4
        u32 flags;

        struct {                         // size 0x4
            u32 dragging_colour : 1;     // 0x6c:0
            u32 dragging_saturation : 1; // 0x6c:1
            u32 confirm : 1;             // 0x6c:2
            u32 unknown_flags : 29;      // 0x6c:3
        };
    }; // 0x6c

    void (*changed)(eduimenu_s *, eduiitem_s *, u32); // 0x70
} EDUI_COLOUR_PICK_FLAGS;

extern "C" {
    extern i32 show_x_scale_help;
    extern i32 show_y_scale_help;
}
extern "C" i32 EduiRenderWidth;
extern "C" i32 near_clip_at_cursor;

/* PC Maya drag angles are observed through DWORD reads and low-WORD writes.
 * The upper halves remain part of each view and are preserved by updates. */
typedef union EdcamMayaAngleObserved { // size 0x4
    u32 word;

    struct {       // size 0x4
        u16 angle; // 0x0
        u16 upper; // 0x2
    } parts;
} EdcamMayaAngleObserved;

extern "C" {
    edcam_s edcam_default = {{0.0f, 0.0f, 0.0f},
                             0,
                             0,
                             -2.0f,
                             {0.0f, 0.0f, 0.0f},
                             {0.0f, 0.0f, 0.0f},
                             {1.0f, 1.0f, 1.0f},
                             1,
                             1,
                             {0.0f, 0.0f, 0.0f},
                             0,
                             0,
                             {0.0015625f, 0.0015625f, 0.0015625f},
                             2,
                             2,
                             0.0078125f,
                             0.5f,
                             0.2f,
                             0.2f,
                             4.0f,
                             0.15f,
                             0.2f,
                             0.01f,
                             0.1f,
                             {0x3f}};
}
extern "C" {

    /* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. Retail keeps this
     * TU's out-of-line copy at 0x617c20. */
    static f32 EdcamFabs(f32 f) {
        f32 rv;

        *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
        return rv;
    }

    static i32 EdtoolsallVuAngleDifference(i32 from, i32 to) {
        i32 difference = (from - to) & 0xffff;
        if (difference >= 0x8000) {
            difference -= 0x10000;
        }
        return difference;
    }

    static void EdcamVecZero(NUVEC *v) {
        v->x = v->y = v->z = 0.0f;
    }

    void fun_00617ca0(void) {
    }

    static void EduiPrimSetColour(i32 colour) {
        u32 converted;
        if (NuPrimNeedsOverbrightening != 0) {
            converted = ((colour & 0xff) << 16) + ((colour >> 16) & 0xff) + (colour & 0xff00) + (colour & 0xff000000);
        } else {
            converted = ((colour >> 1) & 0x7f00) + ((colour & 0xfe) << 15) + ((colour >> 17) & 0x7f) + (colour & 0xff000000);
        }
        NuPrimStreamBufferPointer->vertex_ptr->colour = converted;
    }

    static void EduiPushCoordinateSystem(i32 system) {
        ++NuPrimCSPos;
        NuPrimSetCoordinateSystem(system);
    }

    void EduiPopCoordinateSystem(void) {
        NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[--NuPrimCSPos]);
    }

    i32 editor_return = 0;

    void edmainSetReturn(i32 result) {
        editor_return = result;
    }
    static struct ed_module_s *ed_curr = 0;
    static i32 ed_module_active = 0;

    ed_module_s *edmainCurrent(void) {
        if (ed_module_active != 0 && ed_curr != NULL) {
            return ed_curr;
        }
        return NULL;
    }
    static struct ed_module_s *ed_list = 0;

    i32 edmainActivate(ed_module_s *module, i32 notify) {
        ed_module_s *entry;
        void (*callback)(void);
        if (module != NULL) {
            for (entry = ed_list; entry != NULL; entry = entry->next) {
                if (entry == module) {
                    ed_curr = module;
                    callback = module->activate;
                    ed_module_active = 1;
                    if (callback != NULL && notify != 0) {
                        callback();
                    }
                    return 1;
                }
            }
        } else if (ed_curr != NULL) {
            callback = ed_curr->deactivate;
            if (callback != NULL && notify != 0) {
                callback();
            }
            ed_curr = NULL;
            ed_module_active = 0;
        }
        return 0;
    }

    void edmainSetCamera(NUMTX *matrix) {
        NUCAMERA *camera = EdMainCurrentCamera;
        camera->mtx = *matrix;
        NuCameraSet(camera);
    }

    NUCAMERA *edmainGetCamera(void) {
        return EdMainCurrentCamera;
    }

    NUCAMERA *edinternalcam = 0;

    void edmainExtCamera(NUCAMERA *camera) {
        EdMainCurrentCamera = camera != NULL ? camera : edinternalcam;
    }

    i32 edmain_cursor_enabled = 0;

    void edmainSetCursorEnabled(i32 enabled) {
        edmain_cursor_enabled = enabled;
        PcEditorCursorEnable(enabled);
    }

    i32 edmainGetCursorEnabled(void) {
        return edmain_cursor_enabled;
    }

    char ed_levelfile[256] = {0};

    void edmainReadConfig(void) {
        NUFILE file;
        i32 block;
        ed_module_s *module;
        if (ed_levelfile[0] != 0) {
            file = NuFileOpen(ed_levelfile, 0);
            if (file != 0) {
                NuFileBeginBlkRead(file, 0x3032554e);
                while ((block = NuFileBeginBlkRead(file, 0)) != 0) {
                    for (module = ed_list; module != NULL; module = module->next) {
                        if (module->block_id == (u32)block) {
                            if (module->read != NULL) {
                                module->read(file);
                            }
                            break;
                        }
                    }
                    NuFileEndBlkRead(file);
                }
                NuFileEndBlkRead(file);
                NuFileClose(file);
            }
        }
    }

    void cbMMRegSel(eduimenu_s *menu, eduiitem_s *selected, u32 buttons) {
        ed_module_s *module = (ed_module_s *)selected->data_ptr;
        ed_curr = module;
        if (module->reserved == NULL) {
            void (*activate)(void) = module->activate;
            ed_module_active = 1;
            if (activate != NULL) {
                activate();
            }
        }
    }

    void cbMMCancel(eduimenu_s *menu, eduimenu_s *parent) {
        edmainSetReturn(1);
    }

    void cbMMReturnToApp(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
        edmainSetReturn(1);
    }
    static struct eduimenu_s *ed_main_menu = 0;

    eduimenu_s *edGetMainMenu(void) {
        return ed_main_menu;
    }

    NUVEC *ed_loc = 0;

    void edmainSetLocVec(NUVEC *location) {
        ed_loc = location;
    }

    /* The pointer is zero-initialized engine/editor storage. */
    NUVEC *edmainQueryLocVec(void) {
        return ed_loc;
    }

    f32 EduiSliderGlobalAcceleration = 0.0f;

    void eduiSetGlobalSliderAccel(f32 acceleration) {
        if (acceleration < 0.0f) {
            acceleration = 0.0f;
        } else if (acceleration > 1.0f) {
            acceleration = 1.0f;
        }
        EduiSliderGlobalAcceleration = acceleration;
    }

    f32 EduiScaleX = 16.0f;

    i32 EduiScaledX(f32 x) {
        return (EduiScaleX * x);
    }

    f32 EduiScaleY = 8.0f;

    i32 EduiScaledY(f32 y) {
        return (EduiScaleY * y);
    }

    static void EduiColourRgbToHsv(f32 red, f32 green, f32 blue, f32 *hue, f32 *saturation, f32 *value) {
        f32 maximum = red > (green > blue ? green : blue) ? red : (green > blue ? green : blue);
        f32 minimum = red < (green < blue ? green : blue) ? red : (green < blue ? green : blue);
        *value = maximum;
        *saturation = maximum != 0.0f ? (maximum - minimum) / maximum : 0.0f;
        if (*saturation != 0.0f) {
            f32 delta = maximum - minimum;
            if (red == maximum) {
                *hue = (green - blue) / delta;
            } else if (green == maximum) {
                *hue = (f32)(2.0 + (blue - red) / delta);
            } else if (blue == maximum) {
                *hue = (f32)(4.0 + (red - green) / delta);
            }
            *hue = (f32)(*hue * 60.0);
            if (*hue < 0.0f) {
                *hue = (f32)(*hue + 360.0);
            }
        } else {
            *hue = 0.0f;
        }
    }
}

static void eduiHSVToRGB(f32 hue, f32 saturation, f32 value, f32 &red, f32 &green, f32 &blue) {
    if (saturation == 0.0f) {
        blue = green = red = value;
    } else {
        if (hue == 360.0f) {
            hue = 0.0f;
        }
        hue /= 60.0;
        i32 sector = static_cast<i32>(NuFloor(hue));
        f32 fraction = hue - sector;
        f32 low = (1.0f - saturation) * value;
        f32 falling = (1.0f - saturation * fraction) * value;
        f32 rising = (1.0f - (1.0f - fraction) * saturation) * value;
        switch (sector) {
            case 0:
                red = value;
                green = rising;
                blue = low;
                break;
            case 1:
                red = falling;
                green = value;
                blue = low;
                break;
            case 2:
                red = low;
                green = value;
                blue = rising;
                break;
            case 3:
                red = low;
                green = falling;
                blue = value;
                break;
            case 4:
                red = rising;
                green = low;
                blue = value;
                break;
            case 5:
                red = value;
                green = low;
                blue = falling;
                break;
        }
    }
}

extern "C" {
    i32 edui_donotdraw = 0;
}

/* The drawing calls of the menu renderers, skipped while edui_donotdraw is set. */
static void EduiQFntSet(VUFNT *font) {
    if (edui_donotdraw == 0) {
        NuQFntSet(font);
    }
}

static void EduiQFntSetColour(VUFNT *font, u32 colour) {
    if (edui_donotdraw == 0) {
        NuQFntSetColour(font, colour);
    }
}

static void EduiPrintAlignedText(VUFNT *font, i32 x, i32 y, i32 alignment, char *format, ...) {
    if (edui_donotdraw == 0) {
        char text[1024];
        va_list args;
        i32 width;
        i32 left;
        NuQFntPushPrintMode(NUQFNT_MODE_2D);
        va_start(args, format);
        NuVSPrintf(text, format, args);
        va_end(args);
        width = (i32)NuQFntPrintLenU(font, text);
        switch (alignment) {
            case EDUI_ALIGN_LEFT:
                left = x;
                break;
            case EDUI_ALIGN_RIGHT:
                left = x - width;
                break;
            case EDUI_ALIGN_CENTRE:
                left = x - width / 2;
                break;
            default:
                left = x;
                break;
        }
        NuQFntMove(font, (f32)left, (f32)y, 0.0f);
        NuQFntPrintU(font, text);
        NuQFntPopPrintMode();
    }
}

/* Prints at a 640x448 position, scaled to the 2D renderer's sub-pixels. */
extern "C" void EduiFntPrintScaled(VUFNT *font, f32 x, f32 y, i32 alignment, char *text) {
    EduiPrintAlignedText(font, (i32)(x * EduiScaleX), (i32)(y * EduiScaleY), alignment, text);
}

static void EduiPrintClippedText(VUFNT *font, i32 x, i32 y, i32 alignment, i32 clip_x, i32 clip_width, char *format, ...) {
    if (edui_donotdraw == 0) {
        char text[1024];
        va_list args;
        NuQFntPushPrintMode(NUQFNT_MODE_2D);
        va_start(args, format);
        NuVSPrintf(text, format, args);
        va_end(args);
        i32 width = (i32)NuQFntPrintLenU(font, text);
        i32 left;
        switch (alignment) {
            case EDUI_ALIGN_LEFT:
                left = x;
                break;
            case EDUI_ALIGN_RIGHT:
                left = x - width;
                break;
            case EDUI_ALIGN_CENTRE:
                left = x - width / 2;
                break;
            default:
                left = x;
                break;
        }
        if (left < clip_x) {
            left = clip_x;
        }
        char *end = text + NuStrLen(text) - 1;
        while (width > clip_width && end > text) {
            *end-- = 0;
            width = (i32)NuQFntPrintLenU(font, text);
        }
        NuQFntMove(font, (f32)left, (f32)y, 0.0f);
        NuQFntPrintU(font, text);
        NuQFntPopPrintMode();
    }
}

extern "C" {

    static void eduiFntPrintClipEx(struct vufnt_s *font, f32 x, f32 y, i32 alignment, f32 clip_x, f32 clip_width, char *format, ...) {
        if (edui_donotdraw == 0) {
            char text[1024];
            va_list args;
            i32 left = (i32)(x * EduiScaleX);
            i32 top = (i32)(y * EduiScaleY);
            i32 clip_left = (i32)(clip_x * EduiScaleX);
            i32 clip_right = (i32)(clip_width * EduiScaleX);
            i32 width;
            i32 text_x;
            char *end;
            NuQFntPushPrintMode(NUQFNT_MODE_2D);
            va_start(args, format);
            NuVSPrintf(text, format, args);
            va_end(args);
            width = (i32)NuQFntPrintLenU(font, text);
            switch (alignment) {
                case EDUI_ALIGN_LEFT:
                    break;
                case EDUI_ALIGN_RIGHT:
                    left -= width;
                    break;
                case EDUI_ALIGN_CENTRE:
                    left -= width / 2;
                    break;
            }
            text_x = left;
            if (left < clip_left) {
                text_x = clip_left;
            }
            end = text + NuStrLen(text) - 1;
            while (width > clip_right && end > text) {
                *end-- = 0;
                width = (i32)NuQFntPrintLenU(font, text);
            }
            NuQFntMove(font, (f32)text_x, (f32)top, 0.0f);
            NuQFntPrintU(font, text);
            NuQFntPopPrintMode();
        }
    }
}

static void EduiRndrGradRect2di(i32 x, i32 y, i32 width, i32 height, u32 *colours, NUMTL *material) {
    if (edui_donotdraw == 0) {
        NuRndrGradRect2di(x, y, width, height, colours, material);
    }
}

static void EduiRndrLine2di(i32 x0, i32 y0, i32 x1, i32 y1, i32 colour, NUMTL *material) {
    if (edui_donotdraw == 0) {
        NuRndrLine2di(x0, y0, x1, y1, colour, material);
    }
}

extern "C" {

    /* Screen-space helpers in 640x448 units; EduiScaleX/EduiScaleY convert to
     * the 16x/8x sub-pixel coordinates of the 2D renderer. */
    static void EduiDrawLine(f32 x0, f32 y0, f32 x1, f32 y1, u32 colour, NUMTL *material) {
        if (edui_donotdraw == 0) {
            NuRndrLine2di((i32)(x0 * EduiScaleX), (i32)(y0 * EduiScaleY), (i32)(x1 * EduiScaleX), (i32)(y1 * EduiScaleY), colour, material);
        }
    }
}

static void EduiRndrRect2di(i32 x, i32 y, i32 width, i32 height, i32 colour, NUMTL *material) {
    if (edui_donotdraw == 0) {
        NuRndrRect2di(x, y, width, height, colour, material);
    }
}

extern "C" {

    static void EduiDrawRect(f32 x, f32 y, f32 width, f32 height, u32 colour, NUMTL *material) {
        if (edui_donotdraw == 0) {
            NuRndrRect2di((i32)(x * EduiScaleX), (i32)(y * EduiScaleY), (i32)(width * EduiScaleX), (i32)(height * EduiScaleY), colour,
                          material);
        }
    }
}

static void EduiRndrTriStrip2di(i32 *positions, f32 *uvs, i32 count, i32 colour, NUMTL *material) {
    if (edui_donotdraw == 0) {
        NuRndrTriStrip2di(positions, uvs, count, colour, material);
    }
}

static void EduiRndrLineStrip2di(i32 *positions, f32 *uvs, i32 count, i32 colour, NUMTL *material) {
    if (edui_donotdraw == 0) {
        NuRndrLineStrip2di(positions, uvs, count, colour, material);
    }
}

static void EduiRndrLineRect2di(i32 x, i32 y, i32 width, i32 height, i32 colour, NUMTL *material) {
    if (edui_donotdraw == 0) {
        NuRndrLineRect2di(x, y, width, height, colour, material);
    }
}

extern "C" {

    static void EduiDrawLineRect(f32 x, f32 y, f32 width, f32 height, u32 colour, NUMTL *material) {
        if (edui_donotdraw == 0) {
            NuRndrLineRect2di((i32)(x * EduiScaleX), (i32)(y * EduiScaleY), (i32)(width * EduiScaleX), (i32)(height * EduiScaleY), colour,
                              material);
        }
    }
}

static void EduiRndrRectUV2di(i32 x, i32 y, i32 width, i32 height, f32 u0, f32 v0, f32 u1, f32 v1, i32 colour, NUMTL *material) {
    if (edui_donotdraw == 0) {
        NuRndrRectUV2di(x, y, width, height, u0, v0, u1, v1, colour, material);
    }
}

extern "C" {
    NUMTL *uimtls[5] = {0};
}
i32 ui_bgmtl = 0;

static void EduiDrawGradientSpan(i32 x, i32 y, i32 width, i32 height, u32 first_colour, u32 second_colour, f32 start, f32 end) {
    u32 colours[4];
    width <<= 4;
    colours[0] = colours[2] = first_colour;
    colours[1] = colours[3] = second_colour;
    x <<= 4;
    i32 left = (i32)(width * start) + x;
    if (edui_donotdraw == 0) {
        i32 right = (i32)(width * end) + x;
        NuRndrGradRect2di(left, y << 3, right - left, height << 3, colours, uimtls[ui_bgmtl]);
    }
}

static void EduiDrawGradientGrid(i32 x, i32 y, i32 width, i32 height, f32 start, f32 end, u32 top_left, u32 top_right, u32 bottom_left,
                                 u32 bottom_right, i32 steps) {
    width <<= 4;
    y <<= 3;
    height <<= 3;
    x <<= 4;
    i32 left = x + (i32)(width * start);
    i32 right = x + (i32)(width * end);
    EDUI_GRADIENT_COLOUR first;
    EDUI_GRADIENT_COLOUR second;
    EDUI_GRADIENT_COLOUR last_first;
    EDUI_GRADIENT_COLOUR last_second;
    first.word = top_left;
    second.word = top_right;
    last_first.word = bottom_left;
    last_second.word = bottom_right;
    EDUI_GRADIENT_COLOUR colours[4];
    colours[0] = first;
    colours[1] = second;
    for (i32 strip = 0; strip < steps; ++strip) {
        colours[2].channel[0] = first.channel[0] + (last_first.channel[0] - first.channel[0]) * strip / steps;
        colours[2].channel[1] = first.channel[1] + (last_first.channel[1] - first.channel[1]) * strip / steps;
        colours[2].channel[2] = first.channel[2] + (last_first.channel[2] - first.channel[2]) * strip / steps;
        colours[2].channel[3] = first.channel[3] + (last_first.channel[3] - first.channel[3]) * strip / steps;
        colours[3].channel[0] = second.channel[0] + (last_second.channel[0] - second.channel[0]) * strip / steps;
        colours[3].channel[1] = second.channel[1] + (last_second.channel[1] - second.channel[1]) * strip / steps;
        colours[3].channel[2] = second.channel[2] + (last_second.channel[2] - second.channel[2]) * strip / steps;
        colours[3].channel[3] = second.channel[3] + (last_second.channel[3] - second.channel[3]) * strip / steps;
        EduiRndrGradRect2di(left, y + strip * height / steps, right - left, height / steps, &colours[0].word, uimtls[ui_bgmtl]);
        colours[0] = colours[2];
        colours[1] = colours[3];
    }
}

extern "C" {

    void eduiInitMaterials(void) {
        uimtls[0] = NuMtlCreate(1);
        uimtls[0]->attribs.cull_mode = NUMTL_CULL_NONE;
        uimtls[0]->attribs.z_mode = NUMTL_ZMODE_OFF;
        uimtls[0]->opacity = 1.0f;
        uimtls[0]->attribs.alpha_mode = NUMTL_ALPHA_OPAQUE;
        NuMtlUpdate((uimtls[0]));
        uimtls[1] = NuMtlCreate(1);
        uimtls[1]->diffuse_r = 0.2f;
        uimtls[1]->diffuse_g = 0.2f;
        uimtls[1]->diffuse_b = 0.2f;
        uimtls[1]->attribs.cull_mode = NUMTL_CULL_NONE;
        uimtls[1]->attribs.z_mode = NUMTL_ZMODE_OFF;
        uimtls[1]->opacity = 0.999f;
        uimtls[1]->attribs.alpha_mode = NUMTL_ALPHA_SUBTRACTIVE;
        NuMtlUpdate((uimtls[1]));
        uimtls[2] = NuMtlCreate(1);
        uimtls[2]->diffuse_r = 0.2f;
        uimtls[2]->diffuse_g = 0.2f;
        uimtls[2]->diffuse_b = 0.2f;
        uimtls[2]->attribs.cull_mode = NUMTL_CULL_NONE;
        uimtls[2]->attribs.z_mode = NUMTL_ZMODE_OFF;
        uimtls[2]->opacity = 0.999f;
        uimtls[2]->attribs.alpha_mode = NUMTL_ALPHA_ADDITIVE;
        NuMtlUpdate((uimtls[2]));
        uimtls[3] = NuMtlCreate(1);
        uimtls[3]->diffuse_r = 0.2f;
        uimtls[3]->diffuse_g = 0.2f;
        uimtls[3]->diffuse_b = 0.2f;
        uimtls[3]->attribs.cull_mode = NUMTL_CULL_NONE;
        uimtls[3]->attribs.z_mode = NUMTL_ZMODE_OFF;
        uimtls[3]->opacity = 0.999f;
        uimtls[3]->attribs.alpha_mode = NUMTL_ALPHA_BLEND;
        NuMtlUpdate((uimtls[3]));
    }

    void eduiSetRenderPlane(i32 plane) {
        i32 i;
        for (i = 0; i < 4; i++) {
            NuMtlSetRenderPlaneUi(uimtls[i], plane);
        }
    }
    struct eduiitem_s *edui_last_item = 0;

    eduiitem_s *eduiMenuAddItem(eduimenu_s *menu, eduiitem_s *item) {
        if (menu->last != NULL) {
            menu->last->next = item;
        } else {
            menu->first = item;
        }
        item->previous = menu->last;
        item->next = NULL;
        menu->last = item;
        menu->first_visible = menu->first;
        edui_last_item = item;
        menu->last_visible = NULL;
        return item;
    }

    void eduiMenuAddItemFirst(eduimenu_s *menu, eduiitem_s *item) {
        if (menu->first) {
            menu->first->previous = item;
        } else {
            menu->last = item;
        }
        item->previous = NULL;
        item->next = menu->first;
        menu->first = item;
        menu->first_visible = item;
        edui_last_item = item;
        menu->last_visible = NULL;
    }

    eduiitem_s *eduiMenuAddItemLast(eduimenu_s *menu, eduiitem_s *item) {
        if (menu->last) {
            menu->last->next = item;
        } else {
            menu->first = item;
        }
        item->next = NULL;
        item->previous = menu->last;
        menu->last = item;
        menu->first_visible = item;
        edui_last_item = item;
        menu->last_visible = NULL;
        return item;
    }

    eduiitem_s *eduiMenuAddItemAfter(eduimenu_s *menu, eduiitem_s *item, eduiitem_s *after) {
        if (after == NULL) {
            eduiMenuAddItemFirst(menu, item);
            return item;
        }
        item->previous = after;
        item->next = after->next;
        after->next = item;
        if (item->next == NULL) {
            menu->last = item;
        } else {
            item->next->previous = item;
        }
        menu->first_visible = menu->first;
        menu->last_visible = NULL;
        edui_last_item = item;
        return item;
    }

    /* The item before the insertion point keeps its old next link. */
    eduiitem_s *eduiMenuAddItemBefore(eduimenu_s *menu, eduiitem_s *item, eduiitem_s *before) {
        if (before == NULL) {
            return eduiMenuAddItemLast(menu, item);
        }
        item->next = before;
        item->previous = before->previous;
        before->previous = item;
        if (item->previous == NULL) {
            menu->first = item;
        }
        menu->first_visible = menu->first;
        menu->last_visible = NULL;
        edui_last_item = item;
        return item;
    }

    i32 eduiMenuItemMoveUp(eduimenu_s *menu, eduiitem_s *item) {
        eduiitem_s *previous;
        eduiitem_s *next;
        if (item == menu->first) {
            return 0;
        }
        previous = item->previous;
        next = item->next;
        item->previous = previous->previous;
        item->next = previous;
        if (item->previous != NULL) {
            item->previous->next = item;
        } else {
            menu->first = item;
        }
        if (next != NULL) {
            next->previous = previous;
        } else {
            menu->last = item->next;
        }
        previous->previous = item;
        previous->next = next;
        menu->first_visible = menu->first;
        menu->last_visible = NULL;
        return 1;
    }

    i32 eduiMenuItemMoveDown(eduimenu_s *menu, eduiitem_s *item) {
        eduiitem_s *next;
        eduiitem_s *previous;
        if (item == menu->last) {
            return 0;
        }
        next = item->next;
        previous = item->previous;
        item->previous = next;
        item->next = next->next;
        if (item->next != NULL) {
            item->next->previous = item;
        } else {
            menu->last = item;
        }
        if (previous != NULL) {
            previous->next = next;
        } else {
            menu->first = next;
        }
        next->previous = previous;
        next->next = item;
        menu->first_visible = menu->first;
        menu->last_visible = NULL;
        return 1;
    }

    void eduiMenuSortItemsByTxt(eduimenu_s *menu) {
        eduiitem_s *first = menu->first;
        if (first != NULL) {
            i32 sorted;
            do {
                eduiitem_s *item = first;
                sorted = 1;
                while (item->next != NULL && item->text != NULL && item->next->text != NULL) {
                    if (item->type != EDUIITEM_FILTER &&
                        (item->next->type == EDUIITEM_FILTER || NuStrICmp(item->text, item->next->text) > 0)) {
                        if (item != menu->last) {
                            eduiitem_s *next = item->next;
                            eduiitem_s *previous = item->previous;
                            item->previous = next;
                            item->next = next->next;
                            if (item->next != NULL) {
                                item->next->previous = item;
                            } else {
                                menu->last = item;
                            }
                            if (previous != NULL) {
                                previous->next = next;
                            } else {
                                menu->first = next;
                            }
                            next->previous = previous;
                            next->next = item;
                            menu->first_visible = menu->first;
                            menu->last_visible = NULL;
                        }
                        if (item == first) {
                            first = item->previous;
                        }
                        sorted = 0;
                    } else {
                        item = item->next;
                    }
                }
            } while (sorted == 0);
        }
    }

    void eduiMenuSelectFirstEntry(eduimenu_s *menu) {
        menu->selected = NULL;
    }

    void eduiMenuRemoveItem(eduimenu_s *menu, eduiitem_s *item) {
        if (menu->first == item) {
            menu->first = item->next;
        }
        if (menu->last == item) {
            menu->last = item->previous;
        }
        if (menu->selected == item) {
            menu->selected = item->next ? item->next : item->previous;
        }
        menu->first_visible = menu->first;
        menu->last_visible = NULL;
        if (item->next) {
            item->next->previous = item->previous;
        }
        if (item->previous) {
            item->previous->next = item->next;
        }
        item->next = NULL;
        item->previous = NULL;
    }

    /* Analog pad rate for the editor: the left stick moves coarsely, the right
     * stick finely, while the cursor button (bit 6) is held. */
    f32 eduiGetAnalougePadValue(FRAME_PAD *pad) {
        f32 value = 0.0f;
        if (pad != NULL && (pad->digital_buttons & PAD_BUTTON_CROSS)) {
            if (pad->analog_left_x > 192) {
                value = (pad->analog_left_y - 128.0) * 0.01f;
            }
            if (pad->analog_left_x < 64) {
                value = (128.0 - pad->analog_left_y) * -0.01f;
            }
            if (pad->analog_right_x > 192) {
                value = (pad->analog_right_y - 128.0) * 0.001f;
            }
            if (pad->analog_right_x < 64) {
                value = (128.0 - pad->analog_right_y) * -0.001f;
            }
        }
        return value;
    }
    struct eduimenu_s *active_menu = 0;

    eduimenu_s *eduiMenuCreate(i32 x, i32 y, i32 width, i32 height, struct vufnt_s *font, void *callback, char *title) {
        u32 bytes;
        eduimenu_s *menu;
        if (title != NULL) {
            bytes = NuStrLen(title) + sizeof(eduimenu_s) + 1;
        } else {
            bytes = sizeof(eduimenu_s) + 1;
        }
        menu = (eduimenu_s *)NuAlloc(bytes, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x3dd);
        if (menu != NULL) {
            memset(menu, 0, bytes);
            menu->x = x;
            menu->y = y;
            menu->width = width;
            menu->height = height;
            menu->drawn_width = -1;
            menu->drawn_height = -1;
            menu->font = font;
            menu->callback = (EdUiMenuCallback)callback;
            menu->flags = 0;
            if (title != NULL) {
                menu->title = (char *)(menu + 1);
                NuStrCpy(menu->title, title);
            }
            active_menu = menu;
        }
        edui_last_item = NULL;
        return menu;
    }

    void eduiMenuSetAttr(eduimenu_s *menu, const u32 *colours) {
        eduiitem_s *item;
        for (item = menu->first; item; item = item->next) {
            item->colours[0] = colours[0];
            item->colours[1] = colours[1];
            item->colours[2] = colours[2];
            item->colours[3] = colours[3];
        }
        if (menu->child) {
            eduiMenuSetAttr(menu->child, colours);
        }
    }

    void eduiMenuSetAttrAlpha(eduimenu_s *menu, const u32 *colours) {
        eduiitem_s *item;
        for (item = menu->first; item; item = item->next) {
            item->colours[0] &= 0x00ffffff;
            item->colours[0] |= colours[0] & 0xff000000;
            item->colours[1] &= 0x00ffffff;
            item->colours[1] |= colours[1] & 0xff000000;
            item->colours[2] &= 0x00ffffff;
            item->colours[2] |= colours[2] & 0xff000000;
            item->colours[3] &= 0x00ffffff;
            item->colours[3] |= colours[3] & 0xff000000;
        }
    }

    void eduiMenuSetDisabled(eduimenu_s *menu, i32 disabled) {
        eduiitem_s *item;
        for (item = menu->first; item; item = item->next) {
            item->disabled = disabled;
        }
        if (menu->child) {
            eduiMenuSetDisabled(menu->child, disabled);
        }
    }

    void eduiMenuHighlight(eduimenu_s *menu, eduiitem_s *item) {
        eduiitem_s *entry;
        if (item->selection_group != 0) {
            for (entry = menu->first; entry != NULL; entry = entry->next) {
                if (entry == item) {
                    if (entry->type != EDUIITEM_TOGGLE) {
                        entry->highlighted = 1;
                    } else {
                        entry->highlighted = 1 - entry->highlighted;
                    }

                } else if (entry->selection_group == item->selection_group) {
                    entry->highlighted = 0;
                }
            }
        }
    }

    i32 eduiItemSetText(eduiitem_s *item, char *text) {
        if (item->text != NULL && NuStrLen(item->text) < NuStrLen(text)) {
            NuFree(item->text, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x516);
            item->text = NULL;
        }
        if (item->text == NULL) {
            item->text = (char *)NuAlloc(NuStrLen(text) + 1, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x51b);
        }
        if (item->text != NULL) {
            NuStrCpy(item->text, text);
            return 1;
        }
        return 0;
    }

    u32 edui_cursor_buttons = 0;
    u32 edui_cursor_buttons_db = 0;

    i32 eduicbInteractSel(edui_interact_s *interact) {
        edui_sel_s *item = (edui_sel_s *)interact->item;
        eduimenu_s *menu = interact->menu;
        if (interact->callback && (edui_cursor_buttons_db & PAD_BUTTON_CROSS)) {
            item->selected(menu, &item->base, edui_cursor_buttons);
            return 0;
        }
        if (interact->callback && (edui_cursor_buttons & PAD_BUTTON_CROSS) && item->held) {
            item->held(menu, &item->base, edui_cursor_buttons);
        }
        return 0;
    }

    static void EduiDrawDisabledItem(eduiitem_s *item, i32 x, i32 y, i32 width, i32 height) {
        if (item->disabled) {
            i32 left = x << 4;
            i32 top = y << 3;
            i32 span = width << 4;
            i32 bottom = top + (height << 3);
            if (!edui_donotdraw) {
                NuRndrLine2di(left, top, left + span / 4, bottom, item->colours[0], uimtls[ui_bgmtl]);
            }
            if (!edui_donotdraw) {
                NuRndrLine2di(left + span / 4, top, left + span / 2, bottom, item->colours[0], uimtls[ui_bgmtl]);
            }
            if (!edui_donotdraw) {
                NuRndrLine2di(left + span / 2, top, left + span * 3 / 4, bottom, item->colours[0], uimtls[ui_bgmtl]);
            }
            if (!edui_donotdraw) {
                NuRndrLine2di(left + span * 3 / 4, top, left + span, bottom, item->colours[0], uimtls[ui_bgmtl]);
            }
        }
    }

    VUFNT *edui_font = 0;
}

i32 eduicbRenderSel(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 unused_height) {
    i32 height = (i32)(NuQFntHeight(edui_font) * 0.15625);
    f64 font_height = NuQFntHeight(edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline(edui_font));
    item->x = x;
    item->y = y;
    EduiRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2 + item->highlighted], uimtls[ui_bgmtl]);
    EduiQFntSet(edui_font);
    EduiQFntSetColour(edui_font, item->colours[item->highlighted]);
    switch (item->text_alignment) {
        case EDUI_ALIGN_LEFT:
            EduiPrintAlignedText(edui_font, x << 4, (y << 3) + baseline, item->text_alignment, item->text);
            break;
        case EDUI_ALIGN_RIGHT:
            EduiPrintAlignedText(edui_font, (x + width) << 4, (y << 3) + baseline, item->text_alignment, item->text);
            break;
        default:
            EduiPrintAlignedText(edui_font, (x * 2 + width) << 3, (y << 3) + baseline, item->text_alignment, item->text);
            break;
    }
    return height;
}

extern "C" {
    EDUI_COLOUR_CLIPBOARD EduiColourClipboard = {0.0f, 0.0f, 0.0f, 0.0f};
}

extern "C" i32 eduicbRenderSelWithClipColour(eduimenu_s *menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 selected) {
    i32 height = (i32)(NuQFntHeight(edui_font) * 1.25) >> 3;
    f64 font_height = NuQFntHeight(edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline(edui_font));
    item->x = x;
    item->y = y;
    EduiRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2 + item->highlighted], uimtls[ui_bgmtl]);
    EduiQFntSet(edui_font);
    EduiQFntSetColour(edui_font, item->colours[item->highlighted]);
    EduiPrintAlignedText(edui_font, (x * 2 + width) << 3, (y << 3) + baseline, 64, item->text);
    f32 red;
    f32 green;
    f32 blue;
    if (item->type == EDUIITEM_GRAD_PICK) {
        eduiHSVToRGB(EduiColourClipboard.hue, EduiColourClipboard.saturation, EduiColourClipboard.value, red, green, blue);
    } else {
        eduiHSVToRGB(0.0f, 0.0f, EduiColourClipboard.gray, red, green, blue);
    }
    i32 colour = ((u32)(blue * 255.0) << 16) + ((u32)(green * 255.0) << 8) + (u32)(red * 255.0) + 0x80000000;
    EduiDrawGradientSpan(x + width - 45, y, 40, height, colour, colour, 0.0f, 1.0f);
    return height;
}

i32 eduicbRenderCheck(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 unused_selected) {
    i32 points[10];
    i32 height = (i32)(NuQFntHeight(edui_font) * 1.25) >> 3;
    i32 baseline = (i32)(NuQFntHeight(edui_font) * 0.125 + NuQFntBaseline(edui_font));
    item->x = x;
    item->y = y;
    EduiRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2], uimtls[ui_bgmtl]);
    EduiQFntSet(edui_font);
    EduiQFntSetColour(edui_font, item->colours[item->highlighted]);
    EduiPrintAlignedText(edui_font, x << 4, (y << 3) + baseline, 16, item->text);
    i32 left = ((x - height + width) << 4) - 16;
    i32 top = (y << 3) + 16;
    i32 right = left + ((height - 3) << 4);
    i32 bottom = top + ((height - 3) << 3);
    i32 middle_y = (top + bottom) >> 1;
    if (item->highlighted) {
        i32 quarter_x = (left + ((left + right) >> 1)) >> 1;
        i32 lower_y = (bottom + middle_y) >> 1;
        points[0] = left;
        points[1] = middle_y;
        points[2] = quarter_x;
        points[3] = lower_y;
        points[4] = quarter_x;
        points[5] = bottom;
        points[6] = right;
        points[7] = top;
        EduiRndrTriStrip2di(points, NULL, 4, item->colours[0], uimtls[0]);
    } else {
        i32 quarter_x = (left + ((left + right) >> 1)) >> 1;
        i32 lower_y = (bottom + middle_y) >> 1;
        points[0] = left;
        points[1] = middle_y;
        points[2] = quarter_x;
        points[3] = lower_y;
        points[4] = right;
        points[5] = top;
        points[6] = quarter_x;
        points[7] = bottom;
        points[8] = left;
        points[9] = middle_y;
        EduiRndrLineStrip2di(points, NULL, 5, item->colours[0], uimtls[0]);
    }
    return height;
}

extern "C" {

    void eduiItemTextPickSetFmt(edui_textpicker_s *item, char *format) {
        if (item->format) {
            if (NuStrLen(item->format) >= NuStrLen(format)) {
                goto copy;
            }
            NuFree(item->format, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x630);
            item->format = (char *)NuAlloc(NuStrLen(format) + 1, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x631);
        } else {
            item->format = (char *)NuAlloc(NuStrLen(format) + 1, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x635);
        }
    copy:
        NuStrCpy(item->format, format);
    }

    /* On-screen keyboard of the text picker: rows 0-3 are the letter rows, rows
     * 4 and 5 the copy/paste row when the picker allows pasting (flag 2). */
    static void EduiTextPickPreviousRow(edui_textpicker_s *picker) {
        if (--picker->keyboard_row < 0) {
            picker->keyboard_row = (picker->keyboard_flag_bits & 2) ? 5 : 3;
        }
    }

    static void EduiTextPickNextRow(edui_textpicker_s *picker) {
        picker->keyboard_row++;
        if (picker->keyboard_flag_bits & 2) {
            if (picker->keyboard_row > 5) {
                picker->keyboard_row = 0;
            }
        } else if (picker->keyboard_row > 3) {
            picker->keyboard_row = 0;
        }
    }

    char *textrow[4] = {0};

    static void EduiTextPickPreviousColumn(edui_textpicker_s *picker) {
        if (picker->keyboard_row >= 4) {
            return;
        }
        if (--picker->keyboard_column < 0) {
            picker->keyboard_column = NuStrLen(textrow[picker->keyboard_row]) - 1;
        }
    }

    static void EduiTextPickNextColumn(edui_textpicker_s *picker) {
        if (picker->keyboard_row >= 4) {
            return;
        }
        if (++picker->keyboard_column >= NuStrLen(textrow[picker->keyboard_row])) {
            picker->keyboard_column = 0;
        }
    }

    char EduiTextCopyBuffer[64] = {0};

    /* Copies the text into the copy buffer when copying is allowed. */
    static void EduiTextPickCopy(edui_textpicker_s *picker) {
        if (picker->keyboard_flag_bits & 2) {
            NuStrNCpy(EduiTextCopyBuffer, picker->value, sizeof(EduiTextCopyBuffer));
            picker->value[sizeof(EduiTextCopyBuffer) - 1] = '\0';
        }
    }

    /* Clears the text and, when pasting is allowed, replaces it with the copy
     * buffer. */
    static void EduiTextPickPaste(edui_textpicker_s *picker) {
        picker->cursor = 0;
        picker->value[0] = '\0';
        if ((picker->keyboard_flag_bits & 2) && EduiTextCopyBuffer[0]) {
            NuStrNCpy(picker->value, EduiTextCopyBuffer, picker->max_length - 1);
            picker->value[picker->max_length - 1] = '\0';
            picker->cursor = NuStrLen(picker->value);
        }
    }

    static void EduiTextPickInsert(edui_textpicker_s *picker, char character) {
        i32 index;
        if (NuStrLen(picker->value) >= picker->max_length) {
            return;
        }
        index = sizeof(picker->value);
        while (index > picker->cursor) {
            --index;
            picker->value[index] = picker->value[index - 1];
        }
        picker->value[picker->cursor++] = character;
    }

    static void EduiTextPickCursorLeft(edui_textpicker_s *picker) {
        picker->cursor = MAX(0, picker->cursor - 1);
    }

    static void EduiTextPickCursorRight(edui_textpicker_s *picker) {
        picker->cursor = ((NuStrLen(picker->value)) < (picker->cursor + 1) ? (NuStrLen(picker->value)) : (picker->cursor + 1));
    }

    static void EduiTextPickDelete(edui_textpicker_s *picker) {
        i32 index;
        if (picker->cursor == 0) {
            return;
        }
        if (picker->value[picker->cursor]) {
            picker->cursor--;
            for (index = picker->cursor; index < 0xff; index++) {
                picker->value[index] = picker->value[index + 1];
            }
        } else {
            picker->value[--picker->cursor] = '\0';
        }
    }

    char *edui_textrow_default[4] = {"1234567890", "QWERTYUIOP", "ASDFGHJKL_", "ZXCVBNM<>?"};
    char *edui_textrow_lower[4] = {"1234567890", "qwertyuiop", "asdfghjkl_", "zxcvbnm<>?"};

    /* Swaps the keyboard letter rows between upper and lower case. */
    static void EduiTextPickToggleCase(void) {
        if (textrow[1] == edui_textrow_default[1]) {
            memcpy(textrow, edui_textrow_lower, sizeof(textrow));
        } else {
            memcpy(textrow, edui_textrow_default, sizeof(textrow));
        }
    }

    i32 fun_00619fe0(void) {
        return 0;
    }

    i32 EduiTextCursorPeriod = 32;
    i32 EduiTextCursorCountdown = 0;
}

static void EduiRenderTextCursor(char *value, i32 position, i32 x, i32 y, u32 colour) {
    char at_cursor[2] = "@";
    if (--EduiTextCursorCountdown <= 0) {
        EduiTextCursorCountdown = EduiTextCursorPeriod;
    }
    if (EduiTextCursorCountdown >= (EduiTextCursorPeriod >> 1)) {
        i32 length = NuStrLen(value);
        i32 offset;
        if (position) {
            char prefix[256];
            NuStrNCpy(prefix, value, position + 1);
            offset = (i32)NuQFntPrintLenU(edui_font, prefix);
        } else {
            offset = 0;
        }
        at_cursor[0] = position < length ? value[position] : 'A';
        i32 width = (i32)NuQFntPrintLenU(edui_font, at_cursor);
        i32 top = y - (i32)NuQFntBaseline(edui_font);
        i32 height = (i32)NuQFntHeight(edui_font);
        EduiRndrRect2di(x + offset, top, width, height, colour, NULL);
    }
}

extern "C" {
    struct FILE_DIRECTORY_ENTRY *dir_list = 0;
    i32 edui_filepick_listsize = 8;

    static void eduiFilePickAllocDirList(void) {
        if (edui_filepick_listsize != 0 && dir_list == NULL) {
            dir_list = (FILE_DIRECTORY_ENTRY *)NuAlloc(edui_filepick_listsize * 0x118,
                                                       "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x821);
        }
    }

    /* The key cascade tests the key pointers instead of the keys, so the year
     * only decides when the two pointers differ. */
    static void eduiFilePickDateKeys(FILE_DIRECTORY_ENTRY *a, FILE_DIRECTORY_ENTRY *b, i32 *key_a, i32 *key_b) {
        *key_a = a->modified.year;
        *key_b = b->modified.year;
        if (key_a == key_b) {
            *key_a = a->modified.month;
            *key_b = b->modified.month;
            if (key_a == key_b) {
                *key_a = a->modified.day;
                *key_b = b->modified.day;
                if (key_a == key_b) {
                    *key_a = a->modified.hour;
                    *key_b = b->modified.hour;
                    if (key_a == key_b) {
                        *key_a = a->modified.minute;
                        *key_b = b->modified.minute;
                        if (key_a == key_b) {
                            *key_a = a->modified.second;
                            *key_b = b->modified.second;
                        }
                    }
                }
            }
        }
    }

    i32 eduiFilePickCompareDate(FILE_DIRECTORY_ENTRY *a, FILE_DIRECTORY_ENTRY *b) {
        i32 key_a;
        i32 key_b;
        eduiFilePickDateKeys(a, b, &key_a, &key_b);
        return key_a - key_b;
    }

    i32 eduiFilePickCompareDateReverse(FILE_DIRECTORY_ENTRY *a, FILE_DIRECTORY_ENTRY *b) {
        i32 key_a;
        i32 key_b;
        eduiFilePickDateKeys(a, b, &key_a, &key_b);
        return key_b - key_a;
    }

    i32 eduiFilePickCompareSize(FILE_DIRECTORY_ENTRY *a, FILE_DIRECTORY_ENTRY *b) {
        i32 size_a = a->size;
        i32 size_b = b->size;
        if (size_a < size_b) {
            return -1;
        }
        return size_a > size_b;
    }

    i32 eduiFilePickCompareSizeReverse(FILE_DIRECTORY_ENTRY *a, FILE_DIRECTORY_ENTRY *b) {
        i32 size_a = a->size;
        i32 size_b = b->size;
        if (size_a < size_b) {
            return 1;
        }
        return size_a > size_b ? -1 : 0;
    }

    i32 eduiFilePickCompareName(FILE_DIRECTORY_ENTRY *a, FILE_DIRECTORY_ENTRY *b) {
        return NuStrCmp(a->name, b->name);
    }

    i32 eduiFilePickCompareNameReverse(FILE_DIRECTORY_ENTRY *a, FILE_DIRECTORY_ENTRY *b) {
        return NuStrCmp(b->name, a->name);
    }

    void eduiItemFilePickSetFmt(edui_file_pick_s *item, char *format) {
        if (item->format) {
            if (NuStrLen(item->format) >= NuStrLen(format)) {
                goto copy;
            }
            NuFree(item->format, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x881);
            item->format = (char *)NuAlloc(NuStrLen(format) + 1, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x882);
        } else {
            item->format = (char *)NuAlloc(NuStrLen(format) + 1, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x886);
        }
    copy:
        NuStrCpy(item->format, format);
    }
}

i32 eduicbRenderFilePick(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 unused_selected) {
    edui_file_pick_s *picker = (edui_file_pick_s *)item;
    i32 height = (i32)(NuQFntHeight(edui_font) * 1.25) >> 3;
    f64 font_height = NuQFntHeight(edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline(edui_font));
    if (picker->reserved_54 != 0) {
        i32 centre;
        EduiRndrRect2di(x << 4, y << 3, width << 4, height << 4, item->colours[2 + item->highlighted], uimtls[ui_bgmtl]);
        EduiQFntSet(edui_font);
        EduiQFntSetColour(edui_font, item->colours[item->highlighted]);
        centre = (x * 2 + width) << 3;
        EduiPrintAlignedText(edui_font, centre, (y << 3) + baseline, 64, item->text);
        EduiPrintAlignedText(edui_font, centre, ((y + height) << 3) + baseline, 64, picker->format, picker->name);
        return height * 2;
    } else {
        char formatted[256];
        EduiRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2 + item->highlighted], uimtls[ui_bgmtl]);
        NuSPrintf(formatted, picker->format, picker->name);
        EduiQFntSet(edui_font);
        EduiQFntSetColour(edui_font, item->colours[item->highlighted]);
        EduiPrintAlignedText(edui_font, (x * 2 + width) << 3, (y << 3) + baseline, 64, "%s %s", item->text, formatted);
        return height;
    }
}

extern "C" void eduiGradStageSetRGB(edui_gradient_node_s *stage, f32 red, f32 green, f32 blue) {
    f32 hue;
    f32 saturation;
    f32 value;
    EduiColourRgbToHsv(red, green, blue, &hue, &saturation, &value);
    stage->colour = (u32)red | ((u32)green << 8) | ((u32)blue << 16) | 0x80000000u;
    stage->hue = hue;
    stage->saturation = saturation;
    stage->value = value;
}

extern "C" void eduiGradStageSetHSV(edui_gradient_node_s *stage, f32 hue, f32 saturation, f32 value) {
    f32 red;
    f32 green;
    f32 blue;
    eduiHSVToRGB(hue, saturation, value, red, green, blue);
    red = (f32)(red * 255.0);
    green = (f32)(green * 255.0);
    blue = (f32)(blue * 255.0);
    stage->colour = (u32)red | ((u32)green << 8) | ((u32)blue << 16) | 0x80000000u;
    stage->hue = hue;
    stage->saturation = saturation;
    stage->value = value;
}

extern "C" {

    edui_gradient_node_s *eduiGradStageAdd(edui_gradient_pick_s *item, f32 time, f32 hue, f32 saturation, f32 value) {
        edui_gradient_node_s *stage = (edui_gradient_node_s *)NuAlloc(sizeof(edui_gradient_node_s),
                                                                      "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0xa37);
        edui_gradient_node_s *previous;
        edui_gradient_node_s *next;
        if (stage) {
            memset(stage, 0, sizeof(edui_gradient_node_s));
            previous = NULL;
            for (next = item->first_stage; next; next = next->next) {
                if (next->time > time) {
                    break;
                }
                previous = next;
            }
            if (previous) {
                previous->next = stage;
            } else {
                item->first_stage = stage;
            }
            if (next) {
                next->previous = stage;
            }
            stage->time = time;
            stage->previous = previous;
            stage->next = next;
            eduiGradStageSetHSV(stage, hue, saturation, value);
            item->selected_stage = stage;
        }
        return stage;
    }
}

extern "C" edui_gradient_node_s *eduiGradStageAddRGB(edui_gradient_pick_s *item, f32 time, f32 red, f32 green, f32 blue) {
    f32 hue;
    f32 saturation;
    f32 value;
    EduiColourRgbToHsv(red, green, blue, &hue, &saturation, &value);
    return eduiGradStageAdd(item, time, hue, saturation, value);
}

extern "C" void eduiGradStageDelete(edui_gradient_pick_s *item, edui_gradient_node_s *stage) {
    if (stage->previous != NULL) {
        stage->previous->next = stage->next;
    } else {
        item->first_stage = stage->next;
    }
    if (stage->next != NULL) {
        stage->next->previous = stage->previous;
    }
    if (stage->next != NULL) {
        item->selected_stage = stage->next;
    } else {
        item->selected_stage = stage->previous;
    }
    NuFree(stage, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0xa70);
}

extern "C" i32 eduiGradPickRead(eduiitem_s *item, EDUI_GRADIENT_STAGE *stages, i32 capacity) {
    edui_gradient_node_s *node = ((edui_gradient_pick_s *)item)->first_stage;
    f32 red;
    f32 green;
    f32 blue;
    i32 count = 0;
    for (; node; node = node->next, ++count) {
        if (count < capacity) {
            EDUI_GRADIENT_STAGE *stage = &stages[count];
            stage->time = node->time;
            stage->hue = node->hue;
            stage->saturation = node->saturation;
            stage->value = node->value;
            eduiHSVToRGB(node->hue, node->saturation, node->value, red, green, blue);
            stage->red = red;
            stage->green = green;
            stage->blue = blue;
            stage->colour = (u32)(red * 255.0) - (((((u32)(blue * -255.0) + 0x8000u) << 8) + (u32)(green * -255.0)) << 8);
            stage->metadata = node->metadata;
        }
    }
    return count;
}

extern "C" void eduicbItemGradPickDestroy(eduimenu_s *unused_menu, eduiitem_s *item) {
    edui_gradient_pick_s *gradient = (edui_gradient_pick_s *)item;
    while (gradient->first_stage != NULL) {
        eduiGradStageDelete(gradient, gradient->first_stage);
    }
    NuFree(gradient, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0xa9b);
}

extern "C" {
    i32 EduiSelectionMaterial = 0;
}

i32 eduicbRenderGradPick(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 unused_selected) {
    edui_gradient_pick_s *gradient = (edui_gradient_pick_s *)item;
    i32 height = (i32)(NuQFntHeight(edui_font) * 1.25) >> 2;
    f64 font_height = NuQFntHeight(edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline(edui_font));
    item->x = x;
    item->y = y;
    EduiRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2 + item->highlighted], uimtls[ui_bgmtl]);
    EduiQFntSet(edui_font);
    EduiQFntSetColour(edui_font, item->colours[item->highlighted]);
    if (gradient->change_timer && gradient->selected_stage) {
        EduiPrintAlignedText(edui_font, (x * 2 + width) << 3, (y << 3) + baseline, 64, "%1.03f", gradient->selected_stage->time);
        --gradient->change_timer;
    } else {
        EduiPrintAlignedText(edui_font, (x * 2 + width) << 3, (y << 3) + baseline, 64, item->text);
    }
    edui_gradient_node_s *first = gradient->first_stage;
    if (first != NULL) {
        f32 stage_time = first->time;
        u32 stage_colour = first->colour;
        edui_gradient_node_s *next = first->next;
        while (next != NULL) {
            EduiDrawGradientSpan(x, y + (height >> 1), width, height >> 1, stage_colour, next->colour, stage_time, next->time);
            stage_time = next->time;
            stage_colour = next->colour;
            next = next->next;
        }
    }
    edui_gradient_node_s *stage = gradient->first_stage;
    if (stage != NULL) {
        f32 span = (f32)width;
        do {
            i32 left = x + (i32)(stage->time * span);
            EduiRndrRect2di((left - 2) << 4, ((y + (height >> 1)) << 3) - 16, 80, ((height >> 1) << 3) + 16, stage->colour, uimtls[0]);
            if (stage == gradient->selected_stage) {
                EduiRndrLineRect2di((left - 2) << 4, ((y + (height >> 1)) << 3) - 16, 80, ((height >> 1) << 3) + 16, 0x80ffffff,
                                    uimtls[EduiSelectionMaterial]);
            } else {
                EduiRndrLineRect2di((left - 2) << 4, ((y + (height >> 1)) << 3) - 16, 80, ((height >> 1) << 3) + 16, 0x80000000,
                                    uimtls[EduiSelectionMaterial]);
            }
            stage = stage->next;
        } while (stage != NULL);
    }
    return height;
}

void eduiItemColourPickSetHSV(edui_colour_pick_s *item, f32 hue, f32 saturation, f32 value) {
    item->hue = hue;
    item->saturation = saturation;
    item->value = value;
    item->cursor_x = (f32)((f64)hue / 360.0);
    item->cursor_y = value;
}

void eduiItemColourPickSetRGB(edui_colour_pick_s *item, f32 red, f32 green, f32 blue) {
    EduiColourRgbToHsv(red, green, blue, &item->hue, &item->saturation, &item->value);
    item->cursor_x = (f32)(item->hue / 360.0);
    item->cursor_y = item->value;
}

extern "C" i32 eduicbProcessColourPick(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD *pad) {
    EDUI_COLOUR_PICK_FLAGS *pick = (EDUI_COLOUR_PICK_FLAGS *)item;
    if (!edui_cursor_buttons) {
        pick->dragging_colour = 0;
        pick->dragging_saturation = 0;
    }
    i32 dx = pad->analog_left_x - 128;
    if (dx > 0) {
        if (dx < 32) {
            dx = 0;
        } else {
            dx = (dx - 32) * 255 / 223;
        }
    } else {
        if (dx > -32) {
            dx = 0;
        } else {
            dx = (dx + 32) * 255 / 223;
        }
    }
    i32 dy = pad->analog_left_y - 128;
    if (dy > 0) {
        if (dy < 32) {
            dy = 0;
        } else {
            dy = (dy - 32) * 255 / 223;
        }
    } else {
        if (dy > -32) {
            dy = 0;
        } else {
            dy = (dy + 32) * 255 / 223;
        }
    }
    pick->cursor_x += dx * (f64)0.001f;
    pick->cursor_y += dy * (f64)0.001f;
    pick->cursor_x = MIN(1.0f, MAX(0.0f, pick->cursor_x));
    pick->cursor_y = MIN(1.0f, MAX(0.0f, pick->cursor_y));
    if (pad->analog_l2) {
        pick->saturation = (f32)MAX(0.0f, pick->saturation - (u32)pad->analog_l2 * delta_time * (f64)0.005f);
        return 1;
    }
    if (pad->analog_r2) {
        pick->saturation = (f32)MIN(1.0f, pick->saturation + (u32)pad->analog_r2 * delta_time * (f64)0.005f);
        return 1;
    }
    pick->hue = (f32)(pick->cursor_x * 360.0);
    pick->value = pick->cursor_y;
    eduiHSVToRGB(pick->hue, pick->saturation, pick->value, pick->red, pick->green, pick->blue);
    if (((pad->digital_buttons_pressed & PAD_BUTTON_CROSS) || pick->confirm) && pick->changed) {
        pick->changed(menu, item, pad->digital_buttons);
    }
    return 0;
}

extern "C" i32 eduicbProcessGreyPick(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD *pad) {
    edui_colour_pick_s *pick = (edui_colour_pick_s *)item;
    i32 left = pad->analog_l2;
    i32 right = pad->analog_r2;
    if (pad->analog_l1) {
        left = pad->analog_l1;
    }
    if (pad->analog_r1) {
        right = pad->analog_r1;
    }
    if (left) {
        f64 value = pick->value - left / 4192.0;
        pick->value = value < 0.0 ? 0.0 : value;
        return 1;
    }
    if (right) {
        f64 value = pick->value + right / 4192.0;
        pick->value = value > 1.0 ? 1.0 : value;
        return 1;
    }
    pick->saturation = 0.0f;
    pick->hue = 0.0f;
    if ((pad->digital_buttons_pressed & PAD_BUTTON_CROSS) && pick->changed) {
        pick->changed(menu, item, pad->digital_buttons);
    }
    return 0;
}

extern "C" {
    f32 edui_cursor_x = 50.0f;
    f32 edui_cursor_y = 50.0f;
}

static i32 EduiColourPickWithinHue(edui_interact_s *interact) {
    return edui_cursor_x >= interact->x && edui_cursor_y >= interact->y && edui_cursor_x < interact->x + interact->width &&
           edui_cursor_y < interact->y + interact->height * 0.75;
}

static i32 EduiColourPickWithinSaturation(edui_interact_s *interact) {
    return edui_cursor_x >= interact->x && edui_cursor_y >= interact->y + interact->height * 0.75 &&
           edui_cursor_x < interact->x + interact->width &&
           edui_cursor_y < interact->y + interact->height * 0.75 + interact->height * 0.125;
}

static i32 EduiColourPickWithinConfirm(edui_interact_s *interact) {
    return edui_cursor_x >= interact->x && edui_cursor_y >= interact->y + interact->height * 0.75 + interact->height * 0.125 &&
           edui_cursor_x < interact->x + interact->width && edui_cursor_y < interact->y + interact->height;
}

extern "C" i32 eduicbInteractColourPick(edui_interact_s *interact) {
    EDUI_COLOUR_PICK_FLAGS *pick = (EDUI_COLOUR_PICK_FLAGS *)interact->item;
    if ((EduiColourPickWithinHue(interact) || pick->dragging_colour) && !pick->dragging_saturation) {
        pick->cursor_x = (edui_cursor_x - interact->x) / interact->width;
        pick->cursor_y = (f32)((edui_cursor_y - interact->y) / (interact->height * 0.75));
        pick->dragging_colour = (edui_cursor_buttons & 0x40) != 0;
    }
    if ((EduiColourPickWithinSaturation(interact) || pick->dragging_saturation) && !pick->dragging_colour) {
        pick->saturation = (edui_cursor_x - interact->x) / interact->width;
        pick->dragging_saturation = (edui_cursor_buttons & 0x40) != 0;
    }
    pick->cursor_x = MIN(1.0f, MAX(0.0f, pick->cursor_x));
    pick->cursor_y = MIN(1.0f, MAX(0.0f, pick->cursor_y));
    pick->saturation = MIN(1.0f, MAX(0.0f, pick->saturation));
    pick->hue = (f32)(pick->cursor_x * 360.0);
    pick->value = pick->cursor_y;
    eduiHSVToRGB(pick->hue, pick->saturation, pick->value, pick->red, pick->green, pick->blue);
    if (EduiColourPickWithinConfirm(interact) && (edui_cursor_buttons_db & 0x40)) {
        pick->dragging_colour = 0;
        pick->dragging_saturation = 0;
        pick->confirm = 1;
    }
    return pick->dragging_colour || pick->dragging_saturation;
}

extern "C" i32 eduicbRenderColourPick(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 unused_selected) {
    edui_colour_pick_s *picker = (edui_colour_pick_s *)item;
    item->x = x;
    item->y = y;
    i32 main_height = width * 3 / 4;
    EduiDrawGradientGrid(x, y, width, main_height, 0.0f, 1.0f / 6.0f, 0x80000000, 0x80000000, 0x800000ff, 0x8000ffff, 8);
    EduiDrawGradientGrid(x, y, width, main_height, 1.0f / 6.0f, 2.0f / 6.0f, 0x80000000, 0x80000000, 0x8000ffff, 0x8000ff00, 8);
    EduiDrawGradientGrid(x, y, width, main_height, 2.0f / 6.0f, 0.5f, 0x80000000, 0x80000000, 0x8000ff00, 0x80ffff00, 8);
    EduiDrawGradientGrid(x, y, width, main_height, 0.5f, 4.0f / 6.0f, 0x80000000, 0x80000000, 0x80ffff00, 0x80ff0000, 8);
    EduiDrawGradientGrid(x, y, width, main_height, 4.0f / 6.0f, 5.0f / 6.0f, 0x80000000, 0x80000000, 0x80ff0000, 0x80ff00ff, 8);
    EduiDrawGradientGrid(x, y, width, main_height, 5.0f / 6.0f, 1.0f, 0x80000000, 0x80000000, 0x80ff00ff, 0x800000ff, 8);
    if (edui_donotdraw == 0) {
        f32 line_height = (f32)main_height;
        f32 line_y = (f32)y;
        NuRndrLine2di(x << 4, (i32)(line_y + (i32)line_height * picker->cursor_y) << 3, ((x + width) << 4) - 16,
                      (i32)(line_y + line_height * picker->cursor_y) << 3, 0x80ffffff, uimtls[0]);
        if (edui_donotdraw == 0) {
            i32 hue_x = (i32)(x + width * picker->cursor_x) << 4;
            NuRndrLine2di(hue_x, y << 3, hue_x, ((y + main_height) << 3) - 8, 0x80ffffff, uimtls[0]);
        }
    }
    i32 bar_height = width / 8;
    y += main_height;
    f32 red;
    f32 green;
    f32 blue;
    eduiHSVToRGB(picker->hue, 1.0f, picker->value, red, green, blue);
    u32 full = (u32)(red * 255.0) - (((((u32)(blue * -255.0) + 0x8000) << 8) + (u32)(green * -255.0)) << 8);
    eduiHSVToRGB(picker->hue, 0.0f, picker->value, red, green, blue);
    u32 grey = (u32)(red * 255.0) - (((((u32)(blue * -255.0) + 0x8000) << 8) + (u32)(green * -255.0)) << 8);
    EduiDrawGradientSpan(x, y, width, bar_height, grey, full, 0.0f, 1.0f);
    if (edui_donotdraw == 0) {
        i32 middle = (y * 2 + (bar_height & ~1)) << 2;
        f32 span = (f32)(width - 2);
        i32 marker = (i32)((f32)(x + 1) + picker->saturation * span) << 4;
        NuRndrLine2di(marker, y << 3, marker, middle - 8, 0x80ffffff, uimtls[0]);
        if (edui_donotdraw == 0) {
            marker = (i32)((f32)x + picker->saturation * span) << 4;
            NuRndrLine2di(marker, y << 3, marker, middle - 8, 0x80ffffff, uimtls[0]);
            if (edui_donotdraw == 0) {
                marker = (i32)((f32)(x - 1) + picker->saturation * span) << 4;
                NuRndrLine2di(marker, y << 3, marker, middle - 8, 0x80ffffff, uimtls[0]);
                if (edui_donotdraw == 0) {
                    marker = (i32)((f32)(x + 1) + picker->saturation * span) << 4;
                    NuRndrLine2di(marker, middle, marker, ((y + bar_height) << 3) - 8, 0x80000000, uimtls[0]);
                    if (edui_donotdraw == 0) {
                        marker = (i32)((f32)x + picker->saturation * span) << 4;
                        NuRndrLine2di(marker, middle, marker, ((y + bar_height) << 3) - 8, 0x80000000, uimtls[0]);
                        if (edui_donotdraw == 0) {
                            marker = (i32)((f32)(x - 1) + picker->saturation * span) << 4;
                            NuRndrLine2di(marker, middle, marker, ((y + bar_height) << 3) - 8, 0x80000000, uimtls[0]);
                        }
                    }
                }
            }
        }
    }
    eduiHSVToRGB(picker->hue, picker->saturation, picker->value, red, green, blue);
    u32 selected = (u32)(red * 255.0) - (((((u32)(blue * -255.0) + 0x8000) << 8) + (u32)(green * -255.0)) << 8);
    EduiDrawGradientSpan(x, y + bar_height, width, bar_height, selected, selected, 0.0f, 1.0f);
    return width;
}

extern "C" i32 eduicbRenderGreyPick(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 unused_selected) {
    edui_colour_pick_s *pick = (edui_colour_pick_s *)item;
    i32 height = width / 4;
    item->x = x;
    item->y = y;
    EduiDrawGradientSpan(x, y, width, height, 0x80000000, 0x80ffffff, 0.0f, 1.0f);
    if (edui_donotdraw == 0) {
        f32 plus_x = (f32)(x + 1);
        i32 marker = (i32)(pick->value * (width - 2) + plus_x) << 4;
        NuRndrLine2di(marker, y << 3, marker, (y + (height >> 1) - 1) << 3, 0x80ffffff, uimtls[0]);
        if (edui_donotdraw == 0) {
            f32 middle_x = (f32)x;
            marker = (i32)(pick->value * (width - 2) + middle_x) << 4;
            NuRndrLine2di(marker, y << 3, marker, (y + (height >> 1) - 1) << 3, 0x80ffffff, uimtls[0]);
            if (edui_donotdraw == 0) {
                f32 minus_x = (f32)(x - 1);
                marker = (i32)(pick->value * (width - 2) + minus_x) << 4;
                NuRndrLine2di(marker, y << 3, marker, (y + (height >> 1) - 1) << 3, 0x80ffffff, uimtls[0]);
                if (edui_donotdraw == 0) {
                    marker = (i32)(pick->value * (width - 2) + plus_x) << 4;
                    NuRndrLine2di(marker, (y + (height >> 1)) << 3, marker, (y + height - 1) << 3, 0x80000000, uimtls[0]);
                    if (edui_donotdraw == 0) {
                        marker = (i32)(pick->value * (width - 2) + middle_x) << 4;
                        NuRndrLine2di(marker, (y + (height >> 1)) << 3, marker, (y + height - 1) << 3, 0x80000000, uimtls[0]);
                        if (edui_donotdraw == 0) {
                            marker = (i32)(pick->value * (width - 2) + minus_x) << 4;
                            NuRndrLine2di(marker, (y + (height >> 1)) << 3, marker, (y + height - 1) << 3, 0x80000000, uimtls[0]);
                        }
                    }
                }
            }
        }
    }
    return height;
}

extern "C" {

    /* The left stick pans the selected texture corner (scaled by the zoom), the
     * shoulder buttons zoom, and the face buttons snap, switch corners or apply. */
    static i32 eduicbProcessTexturePick(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD *pad) {
        edui_texture_pick_s *pick = (edui_texture_pick_s *)item;
        i32 stick_x = pad->analog_left_x - 128;
        i32 stick_y = pad->analog_left_y - 128;
        i32 left;
        i32 right;
        if (stick_x > 0) {
            if (stick_x < 32) {
                stick_x = 0;
            } else {
                stick_x = (stick_x - 32) * 255 / 223;
            }
        } else if (stick_x > -32) {
            stick_x = 0;
        } else {
            stick_x = (stick_x + 32) * 255 / 223;
        }
        if (stick_y > 0) {
            if (stick_y < 32) {
                stick_y = 0;
            } else {
                stick_y = (stick_y - 32) * 255 / 223;
            }
        } else if (stick_y > -32) {
            stick_y = 0;
        } else {
            stick_y = (stick_y + 32) * 255 / 223;
        }
        pick->uv_x[pick->selected_corner] += stick_x / (pick->zoom * 8192.0);
        pick->uv_y[pick->selected_corner] += stick_y / (pick->zoom * 8192.0);
        pick->uv_x[pick->selected_corner] =
            (1.0f < ((0.0f > (pick->uv_x[pick->selected_corner]) ? 0.0f : (pick->uv_x[pick->selected_corner])))
                 ? 1.0f
                 : ((0.0f > (pick->uv_x[pick->selected_corner]) ? 0.0f : (pick->uv_x[pick->selected_corner]))));
        pick->uv_y[pick->selected_corner] =
            (1.0f < ((0.0f > (pick->uv_y[pick->selected_corner]) ? 0.0f : (pick->uv_y[pick->selected_corner])))
                 ? 1.0f
                 : ((0.0f > (pick->uv_y[pick->selected_corner]) ? 0.0f : (pick->uv_y[pick->selected_corner]))));
        left = pad->analog_l2;
        right = pad->analog_r2;
        if (pad->analog_l1) {
            left = pad->analog_l1;
        }
        if (pad->analog_r1) {
            right = pad->analog_r1;
        }
        if (left) {
            pick->zoom = (1.0f > (pick->zoom - left / 4192.0) ? 1.0f : (pick->zoom - left / 4192.0));
            return 1;
        }
        if (right) {
            pick->zoom = (16.0f < (right / 4192.0 + pick->zoom) ? 16.0f : (right / 4192.0 + pick->zoom));
            return 1;
        }
        if (pad->digital_buttons_pressed & PAD_BUTTON_SELECT) {
            // NOLINTNEXTLINE(bugprone-incorrect-roundings): snaps to the 1/256 UV grid.
            pick->uv_x[pick->selected_corner] = (i32)(pick->uv_x[pick->selected_corner] * 256.0 + 0.5) * (1.0 / 256.0);
            // NOLINTNEXTLINE(bugprone-incorrect-roundings): snaps to the 1/256 UV grid.
            pick->uv_y[pick->selected_corner] = (i32)(pick->uv_y[pick->selected_corner] * 256.0 + 0.5) * (1.0 / 256.0);
        }
        if (pad->digital_buttons_pressed & PAD_BUTTON_SQUARE) {
            pick->selected_corner = pick->selected_corner != 1;
        }
        if ((pad->digital_buttons_pressed & PAD_BUTTON_CROSS) && pick->changed) {
            pick->changed(menu, item, pad->digital_buttons);
        }
        return 0;
    }
}

extern "C" i32 eduicbRenderTexturePick(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 unused_selected) {
    edui_texture_pick_s *picker = (edui_texture_pick_s *)item;
    item->x = x;
    item->y = y;
    f32 origin_u = (f32)(picker->uv_x[picker->selected_corner] - 0.5 / picker->zoom);
    f32 origin_v = (f32)(picker->uv_y[picker->selected_corner] - 0.5 / picker->zoom);
    f32 span_u = 1.0f / picker->zoom;
    f32 span_v = 1.0f / picker->zoom;
    origin_u = MIN(1.0f - span_u, MAX(0.0f, origin_u));
    origin_v = MIN(1.0f - span_v, MAX(0.0f, origin_v));
    f32 point_u[2];
    f32 point_v[2];
    for (i32 corner = 0; corner < 2; ++corner) {
        point_u[corner] = (picker->uv_x[corner] - origin_u) / span_u;
        point_u[corner] = MIN(1.0f, MAX(0.0f, point_u[corner]));
        point_v[corner] = (picker->uv_y[corner] - origin_v) / span_v;
        point_v[corner] = MIN(1.0f, MAX(0.0f, point_v[corner]));
    }
    u32 colours[2];
    // MATCH-EXCEPTION: the original stores the second default colour first; an initializer or a chained
    // assignment either reverses the stores or reshuffles the x87 code above.
    colours[1] = 0x807f7f7f;
    colours[0] = 0x807f7f7f;
    colours[picker->selected_corner] = 0x80ffffff;
    EduiRndrRectUV2di(x << 4, y << 3, width << 4, width << 3, origin_u, origin_v, origin_u + span_u, origin_v + span_v, 0x80808080,
                      picker->material);
    for (i32 corner = 0; corner < 2; ++corner) {
        if (edui_donotdraw == 0) {
            u32 colour = colours[corner];
            f32 line_width = (f32)width;
            f32 line_y = (f32)y;
            NuRndrLine2di(x << 4, (i32)(line_y + (i32)line_width * point_v[corner]) << 3, ((x + width) << 4) - 16,
                          (i32)(line_y + line_width * point_v[corner]) << 3, colour, uimtls[0]);
            if (edui_donotdraw == 0) {
                i32 marker_x = (i32)(x + line_width * point_u[corner]) << 4;
                NuRndrLine2di(marker_x, y << 3, marker_x, ((y + width) << 3) - 8, colour, uimtls[0]);
            }
        }
    }
    y += width;
    i32 height = (i32)(NuQFntHeight(edui_font) * 1.25) >> 3;
    f64 font_height = NuQFntHeight(edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline(edui_font));
    EduiRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2 + item->highlighted], uimtls[ui_bgmtl]);
    EduiQFntSet(edui_font);
    EduiQFntSetColour(edui_font, item->colours[item->highlighted]);
    EduiPrintAlignedText(edui_font, (x * 2 + width) << 3, (y << 3) + baseline, 64, "%1.03f,%1.03f",
                         picker->uv_x[picker->selected_corner] * 256.0, picker->uv_y[picker->selected_corner] * 256.0);
    return width + height;
}

extern "C" void eduiItemGraphAddOnionSkin(eduiitem_s *item, nugraph_s *graph) {
    edui_graph_s *picker = (edui_graph_s *)item;
    i32 slot;
    if (item->type == EDUIITEM_GRAPH) {
        for (slot = 0; slot < 8; slot++) {
            if (picker->onion_skins[slot] == NULL) {
                picker->onion_skins[slot] = graph;
                return;
            }
        }
    }
}

extern "C" void eduiItemGraphSetCursor(eduiitem_s *item, f32 x, f32 y) {
    edui_graph_s *picker = (edui_graph_s *)item;
    if (item->type == EDUIITEM_GRAPH) {
        picker->cursor_x = (1.0f < ((0.0f > x ? 0.0f : x)) ? 1.0f : ((0.0f > x ? 0.0f : x)));
        picker->cursor_y = (1.0f < ((0.0f > y ? 0.0f : y)) ? 1.0f : ((0.0f > y ? 0.0f : y)));
    }
}

extern "C" void eduiItemGraphSetLabels(eduiitem_s *item, char *x_label, char *y_label, char *title) {
    edui_graph_s *picker = (edui_graph_s *)item;
    if (item->type == EDUIITEM_GRAPH) {
        if (x_label != NULL) {
            NuStrCpy(picker->x_label, x_label);
        }
        if (y_label != NULL) {
            NuStrCpy(picker->y_label, y_label);
        }
        if (title != NULL) {
            NuStrCpy(picker->title, title);
        }
    }
}

extern "C" {
    i32 show_x_scale_help = 0;
    i32 show_y_scale_help = 0;
}

/* Moves the graph cursor with the left stick, scales the axes with the shoulder buttons and edits points. */
extern "C" i32 eduicbProcessGraph(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD *pad) {
    edui_graph_s *picker = (edui_graph_s *)item;
    i32 stick_x = pad->analog_left_x - 128;
    i32 stick_y = pad->analog_left_y - 128;
    nugraph_s *graph;
    i32 index;
    show_y_scale_help = show_y_scale_help - 1 > 0 ? show_y_scale_help - 1 : 0;
    show_x_scale_help = show_x_scale_help - 1 > 0 ? show_x_scale_help - 1 : 0;
    if (stick_x > 0) {
        if (stick_x < 32) {
            stick_x = 0;
        } else {
            stick_x = (stick_x - 32) * 255 / 223;
        }
    } else if (stick_x > -32) {
        stick_x = 0;
    } else {
        stick_x = (stick_x + 32) * 255 / 223;
    }
    if (stick_y > 0) {
        if (stick_y < 32) {
            stick_y = 0;
        } else {
            stick_y = (stick_y - 32) * 255 / 223;
        }
    } else if (stick_y > -32) {
        stick_y = 0;
    } else {
        stick_y = (stick_y + 32) * 255 / 223;
    }
    picker->cursor_x = stick_x * (1.0 / 8192.0) + picker->cursor_x;
    picker->cursor_y = -stick_y * (1.0 / 8192.0) + picker->cursor_y;
    picker->cursor_x =
        (1.0f < ((0.0f > picker->cursor_x ? 0.0f : picker->cursor_x)) ? 1.0f : ((0.0f > picker->cursor_x ? 0.0f : picker->cursor_x)));
    picker->cursor_y =
        (1.0f < ((0.0f > picker->cursor_y ? 0.0f : picker->cursor_y)) ? 1.0f : ((0.0f > picker->cursor_y ? 0.0f : picker->cursor_y)));
    if (pad->analog_l2 && pad->analog_r2) {
        picker->graph->x_scale = 1.0f;
        show_x_scale_help = 60;
        return 1;
    }
    if (pad->analog_l2 > 55) {
        picker->graph->x_scale -= pad->analog_l2 * 0.02f / 255.0;
        picker->graph->x_scale = picker->graph->x_scale < (f64)0.01f ? 0.01f : picker->graph->x_scale;
        show_x_scale_help = 60;
        return 1;
    }
    if (pad->analog_r2 > 55) {
        picker->graph->x_scale = pad->analog_r2 * 0.02f / 255.0 + picker->graph->x_scale;
        show_x_scale_help = 60;
        return 1;
    }
    if (pad->analog_l1 && pad->analog_r1) {
        picker->graph->y_scale = 1.0f;
        show_y_scale_help = 60;
        return 1;
    }
    if (pad->analog_l1) {
        picker->graph->y_scale -= pad->analog_l1 * 0.02f / 255.0;
        picker->graph->y_scale = picker->graph->y_scale < (f64)0.01f ? 0.01f : picker->graph->y_scale;
        show_y_scale_help = 60;
        return 1;
    }
    if (pad->analog_r1) {
        picker->graph->y_scale = pad->analog_r1 * 0.02f / 255.0 + picker->graph->y_scale;
        show_y_scale_help = 60;
        return 1;
    }
    if ((pad->digital_buttons_pressed & PAD_BUTTON_SQUARE) && picker->selected_point >= 0) {
        nugraphDeletePoint(picker->graph, picker->selected_point);
        nugraphCalcCurve(picker->graph, 100);
        return 1;
    }
    if (pad->digital_buttons_pressed & PAD_BUTTON_CIRCLE) {
        nugraphAddPoint(picker->graph, picker->cursor_x, picker->cursor_y);
        nugraphCalcCurve(picker->graph, 100);
        return 1;
    }
    if ((pad->digital_buttons_pressed & PAD_BUTTON_CROSS) && picker->selected_point >= 0) {
        picker->cursor_x = picker->graph->x[picker->selected_point];
        picker->cursor_y = picker->graph->y[picker->selected_point];
    }
    if ((pad->digital_buttons & PAD_BUTTON_CROSS) && picker->selected_point >= 0) {
        picker->graph->x[picker->selected_point] = picker->cursor_x;
        picker->graph->y[picker->selected_point] = picker->cursor_y;
        index = picker->selected_point;
        graph = picker->graph;
        if (index == 0) {
            graph->x[index] = 0.0f;
        } else if (index == graph->point_count - 1) {
            graph->x[index] = 1.0f;
        } else if (graph->x[index] <= graph->x[index - 1]) {
            graph->x[index] = graph->x[index - 1] + 0.01f;
        } else if (graph->x[index] >= graph->x[index + 1]) {
            graph->x[index] = graph->x[index + 1] - 0.01f;
        }
        nugraphCalcCurve(picker->graph, 100);
        return 1;
    }
    if ((pad->digital_buttons_pressed & PAD_BUTTON_TRIANGLE) && picker->changed) {
        picker->changed(menu, item, pad->digital_buttons_pressed);
    }
    return 0;
}

extern "C" {
    i32 EduiGraphDrawCurve = 1;
    i32 EduiGraphDrawDebug = 0;
    i32 EduiGraphDrawDebugBound = 0;
    i32 EduiGraphDebugIterations = 3;
    i32 EduiGraphDebugBoundIterations = 10;

    void eduiRenderGraphLine(nugraph_s *graph, i32 x, i32 y, i32 width, i32 height, u32 colour) {
        if (EduiGraphDrawCurve != 0) {
            f32 fheight = (f32)height;
            i32 previous_x = 0;
            i32 previous_y = height - (i32)(graph->y[0] * fheight);
            f32 fraction = 0.0f;
            f32 fwidth = (f32)width;
            do {
                i32 current_x = (i32)(fwidth * fraction);
                i32 current_y = height - (i32)(nugraphGetYatX(graph, fraction * graph->x_extent * graph->x_scale, 2) * fheight);
                EduiRndrLine2di((previous_x + x) << 4, (previous_y + y) << 3, (current_x + x) << 4, (current_y + y) << 3, colour,
                                uimtls[0]);
                previous_x = current_x;
                previous_y = current_y;
                fraction += (f64)0.01f;
            } while (fraction <= 1.0f);
        }
        if (EduiGraphDrawDebug != 0) {
            i32 index;
            f32 fwidth = (f32)width;
            f32 fheight = (f32)height;
            for (index = 1; index < 100; ++index) {
                f32 t = index / 100.0;
                f32 bound;
                i32 px;
                i32 top;
                i32 bottom;
                if (EduiGraphDrawDebugBound != 0) {
                    bound = nugraphGetYatX(graph, t, EduiGraphDebugBoundIterations);
                } else {
                    bound = 0.0f;
                }
                px = (i32)(fwidth * t) + x;
                top = y - (i32)(nugraphGetYatX(graph, t, EduiGraphDebugIterations) * fheight) + height;
                bottom = y - (i32)(fheight * bound) + height;
                EduiRndrLine2di(px << 4, top << 3, px << 4, bottom << 3, 0x80000080, uimtls[0]);
            }
        }
    }
}

/* Float fabs by clearing the sign bit. */
static f32 EduiFabs(f32 value) {
    *(u32 *)&value &= 0x7fffffff;
    return value;
}

extern "C" i32 eduicbRenderGraph(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 unused_width, i32 selected) {
    edui_graph_s *picker = (edui_graph_s *)item;
    f32 cursor_x = picker->cursor_x;
    f32 cursor_y = 1.0f - picker->cursor_y;
    item->x = x;
    item->y = y;
    i32 total_width = picker->width;
    i32 graph_width = total_width - 60;
    i32 total_height = picker->height;
    i32 plot_x = x + 30;
    i32 plot_y = y + 30;
    i32 graph_height = total_height - 60;
    EduiRndrRect2di(x << 4, y << 3, total_width << 4, total_height << 3, 0x80000000, uimtls[0]);
    EduiRndrRect2di(plot_x << 4, plot_y << 3, graph_width << 4, graph_height << 3, 0x80100000, uimtls[0]);
    if (picker->graph == NULL) {
        EduiQFntSet(edui_font);
        EduiQFntSetColour(edui_font, 0x80000080);
        EduiPrintAlignedText(edui_font, (x + total_width) << 3, (y + total_height) << 2, 64, (char *)"Invalid Graph Pointer");
        return total_height;
    }
    EduiQFntSet(edui_font);
    EduiQFntSetColour(edui_font, 0x80808080);
    if (picker->x_scale > 0.0f) {
        f32 tick_count = picker->graph->x_extent * picker->graph->x_scale / picker->x_scale;
        i32 label_interval = 1 - (i32)(tick_count / -10.0);
        for (i32 tick = 1; tick <= (i32)tick_count; ++tick) {
            f32 tick_offset = (f32)(graph_width * tick) / tick_count;
            u32 colour;
            if (tick % label_interval == 0) {
                EduiQFntSet(edui_font);
                EduiPrintAlignedText(edui_font, (i32)(plot_x + tick_offset) << 4,
                                     ((plot_y + graph_height) << 3) - 48 + (i32)NuQFntHeight(edui_font), 64, (char *)"%1.01f",
                                     (f32)tick * picker->x_scale);
                colour = 0x80340000;
            } else {
                colour = 0x80260000;
            }
            if (edui_donotdraw == 0) {
                i32 line_x = (i32)(plot_x + tick_offset) << 4;
                NuRndrLine2di(line_x, plot_y << 3, line_x, ((plot_y + graph_height) << 3) - 8, colour, uimtls[0]);
            }
        }
    }
    if (picker->y_scale > 0.0f) {
        f32 tick_count = picker->graph->y_extent * picker->graph->y_scale / picker->y_scale;
        i32 label_interval = 1 - (i32)(tick_count / -10.0);
        for (i32 tick = 1; tick <= (i32)tick_count; ++tick) {
            f32 tick_offset = (f32)(graph_height * tick) / tick_count;
            u32 colour;
            if (tick % label_interval == 0) {
                EduiQFntSet(edui_font);
                EduiPrintAlignedText(edui_font, (plot_x - 2) << 4,
                                     ((i32)((plot_y + graph_height) - tick_offset) << 3) + ((i32)NuQFntHeight(edui_font) >> 1), 32,
                                     (char *)"%1.01f", (f32)tick * picker->y_scale);
                colour = 0x80340000;
            } else {
                colour = 0x80260000;
            }
            if (edui_donotdraw == 0) {
                i32 line_y = (i32)((plot_y + graph_height) - tick_offset) << 3;
                NuRndrLine2di(plot_x << 4, line_y, (plot_x + graph_width - 1) << 4, line_y, colour, uimtls[0]);
            }
        }
    }
    EduiQFntSet(edui_font);
    EduiPrintAlignedText(edui_font, (plot_x + graph_width) << 4, (y << 3) + (i32)NuQFntHeight(edui_font), 32, picker->x_label);
    EduiPrintAlignedText(edui_font, (x + total_width - 3) << 4, (y + total_height) << 3, 32, picker->y_label);
    EduiPrintAlignedText(edui_font, plot_x << 4, (y << 3) + (i32)NuQFntHeight(edui_font), 16, picker->title);
    EduiPrintAlignedText(edui_font, (plot_x - 2) << 4, ((plot_y + graph_height) << 3) - 48 + (i32)NuQFntHeight(edui_font), 32,
                         (char *)"0.0");
    u32 cursor_colour = 0x80303030;
    if (selected) {
        cursor_colour = 0x80707070;
    }
    if (edui_donotdraw == 0) {
        i32 cursor_line_y = (i32)(plot_y + cursor_y * graph_height) << 3;
        NuRndrLine2di(plot_x << 4, cursor_line_y, (plot_x + graph_width - 1) << 4, cursor_line_y, cursor_colour, uimtls[0]);
        if (edui_donotdraw == 0) {
            i32 cursor_line_x = (i32)(plot_x + cursor_x * graph_width) << 4;
            NuRndrLine2di(cursor_line_x, plot_y << 3, cursor_line_x, ((plot_y + graph_height) << 3) - 8, cursor_colour, uimtls[0]);
        }
    }
    picker->selected_point = -1;
    f32 closest = 1.0f;
    for (i32 point = 0; point < picker->graph->point_count; ++point) {
        i32 px = plot_x + (i32)(picker->graph->x[point] * (f32)graph_width);
        i32 py = plot_y - (i32)(picker->graph->y[point] * (f32)graph_height) + graph_height;
        EduiRndrLine2di((px - 10) << 4, py << 3, (px + 10) << 4, py << 3, 0x80505050, uimtls[0]);
        EduiRndrLine2di(px << 4, (py << 3) - 80, px << 4, (py << 3) + 80, 0x80505050, uimtls[0]);
        f32 dx = picker->cursor_x - picker->graph->x[point];
        f32 dy = picker->cursor_y - picker->graph->y[point];
        f32 distance = EduiFabs(dx) + EduiFabs(dy);
        if (distance < closest && distance < 0.05f) {
            picker->selected_point = point;
            closest = distance;
        }
    }
    for (i32 skin = 0; skin < 8; ++skin) {
        if (picker->onion_skins[skin] != NULL) {
            eduiRenderGraphLine(picker->onion_skins[skin], plot_x, plot_y, graph_width, graph_height, 0x80402000);
        }
    }
    eduiRenderGraphLine(picker->graph, plot_x, plot_y, graph_width, graph_height, 0x80800000);
    if (picker->selected_point >= 0 && edui_donotdraw == 0) {
        i32 point = picker->selected_point;
        NuRndrLineRect2di((plot_x + (i32)(picker->graph->x[point] * graph_width) - 6) << 4,
                          (((plot_y + graph_height) - (i32)(picker->graph->y[point] * graph_height)) << 3) - 48, 192, 96, 0x80ffffff,
                          uimtls[EduiSelectionMaterial]);
    }
    if (show_x_scale_help || show_y_scale_help) {
        EduiQFntSet(edui_font);
        i32 help_y = (plot_y << 3) + (i32)NuQFntHeight(edui_font);
        EduiQFntSetColour(edui_font, 0x80808080);
        EduiPrintAlignedText(edui_font, (plot_x + (graph_width >> 1)) << 4, help_y, 64, (char *)"Scaling %c Axis",
                             show_x_scale_help ? 'X' : 'Y');
        help_y += (i32)NuQFntHeight(edui_font);
        EduiPrintAlignedText(edui_font, (plot_x + (graph_width >> 1)) << 4, help_y, 64, (char *)"%1.03f",
                             show_x_scale_help ? picker->graph->x_scale : picker->graph->y_scale);
        NuQFntHeight(edui_font);
    }
    i32 label_height = (i32)(NuQFntHeight(edui_font) * 1.125) >> 3;
    i32 result = total_height + label_height;
    i32 label_row = y + total_height;
    i32 label_baseline = (((label_height << 3) - (i32)NuQFntHeight(edui_font)) >> 1) + (i32)NuQFntBaseline(edui_font);
    EduiRndrRect2di(x << 4, label_row << 3, total_width << 4, label_height << 3, item->colours[2 + item->highlighted], uimtls[ui_bgmtl]);
    EduiQFntSet(edui_font);
    EduiQFntSetColour(edui_font, item->colours[item->highlighted]);
    if (picker->selected_point >= 0) {
        i32 point = picker->selected_point;
        EduiPrintAlignedText(edui_font, (x + 2) << 4, (label_row << 3) + label_baseline, 16, (char *)"Point %d = %1.02f, %1.02f", point,
                             picker->graph->x[point] * picker->graph->x_scale * picker->graph->x_extent,
                             picker->graph->y[point] * picker->graph->y_scale * picker->graph->y_extent);
    }
    EduiPrintAlignedText(edui_font, (x + total_width - 4) << 4, (label_row << 3) + label_baseline, 32,
                         (char *)"Cursor pos = %1.02f, %1.02f", picker->cursor_x * picker->graph->x_scale * picker->graph->x_extent,
                         picker->cursor_y * picker->graph->y_scale * picker->graph->y_extent);
    return result;
}

extern "C" {

    void eduiItemSliderSetValEx(edui_slider_s *item, f32 value, i32 reset_timer, i32 notify) {
        item->normalized_value = (value - item->minimum) / item->range;
        item->value = value;
        if (reset_timer) {
            item->change_timer = 60;
        }
        if (notify && item->changed) {
            item->changed(NULL, &item->base, 0);
        }
    }

    void eduiItemSliderSetVal(edui_slider_s *item, f32 value) {
        eduiItemSliderSetValEx(item, value, 1, 1);
    }

    void eduiItemSliderSetFmt(edui_slider_s *item, char *format) {
        if (item->format) {
            if (NuStrLen(item->format) >= NuStrLen(format)) {
                goto copy;
            }
            NuFree(item->format, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0xfa6);
            item->format = (char *)NuAlloc(NuStrLen(format) + 1, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0xfa7);
        } else {
            item->format = (char *)NuAlloc(NuStrLen(format) + 1, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0xfab);
        }
    copy:
        NuStrCpy(item->format, format);
    }

    void eduiItemSliderSetGranularity(edui_slider_s *item, f32 granularity) {
        item->granularity = granularity;
    }

    i32 eduicbInteractSlider(edui_interact_s *interact) {
        edui_slider_s *slider = (edui_slider_s *)interact->item;
        f32 value = (edui_cursor_x - interact->x) / interact->width;
        value = MIN(MAX(value, 0.0f), 1.0f);
        eduiItemSliderSetVal(slider, value * slider->range);
        return (edui_cursor_buttons >> 6) & 1;
    }

    f32 EduiSliderAcceleration = 1.0f;
    i32 EduiSliderRepeatTimer = 30;
    f32 EduiSliderDynamicScale = 0.0f;
    f32 EduiNominalFrameTime = 1.0f / 60.0f;
}

extern "C" i32 eduicbProcessSlider(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD *pad) {
    edui_slider_s *slider = (edui_slider_s *)item;
    u32 buttons = pad->digital_buttons;
    if ((buttons & PAD_BUTTON_L1) && (buttons & PAD_BUTTON_R1)) {
        if (slider->minimum <= 0.0f && 0.0f <= slider->minimum + slider->range) {
            slider->value = 0.0f;
            slider->normalized_value = (0.0f - slider->minimum) / slider->range;
            slider->change_timer = 60;
            if (slider->changed) {
                slider->changed(menu, item, pad->digital_buttons);
            }
        } else if (slider->minimum <= 1.0f && 1.0f <= slider->minimum + slider->range) {
            slider->value = 1.0f;
            slider->normalized_value = (1.0f - slider->minimum) / slider->range;
            slider->change_timer = 60;
            if (slider->changed) {
                slider->changed(menu, item, pad->digital_buttons);
            }
        }
        EduiSliderRepeatTimer = 30;
    } else if (buttons & PAD_BUTTON_L1) {
        i32 timer = EduiSliderRepeatTimer - 1;
        if (timer < 0) {
            EduiSliderRepeatTimer = 0;
        } else {
            EduiSliderRepeatTimer = timer;
        }
        if (!EduiSliderRepeatTimer || (pad->digital_buttons_pressed & PAD_BUTTON_L1)) {
            f64 step = (f64)slider->granularity * EduiSliderAcceleration;
            slider->value = MAX(slider->minimum, slider->value - step);
            if (slider->value > 0.0f) {
                slider->value += step * 0.5;
            } else {
                slider->value -= step * 0.5;
            }
            slider->value = step * (i32)(slider->value / step);
            slider->normalized_value = (slider->value - slider->minimum) / slider->range;
            slider->change_timer = 60;
            if (slider->changed) {
                slider->changed(menu, item, pad->digital_buttons);
            }
            EduiSliderAcceleration += EduiSliderGlobalAcceleration;
            return 0;
        }
    } else if (buttons & PAD_BUTTON_R1) {
        i32 timer = EduiSliderRepeatTimer - 1;
        if (timer < 0) {
            EduiSliderRepeatTimer = 0;
        } else {
            EduiSliderRepeatTimer = timer;
        }
        if (!EduiSliderRepeatTimer || (pad->digital_buttons_pressed & PAD_BUTTON_R1)) {
            f32 maximum = slider->minimum + slider->range;
            f64 step = (f64)slider->granularity * EduiSliderAcceleration;
            slider->value = MIN(maximum, slider->value + step);
            if (slider->value > 0.0f) {
                slider->value += step * 0.5;
            } else {
                slider->value -= step * 0.5;
            }
            slider->value = step * (i32)(slider->value / step);
            slider->normalized_value = (slider->value - slider->minimum) / slider->range;
            slider->change_timer = 60;
            if (slider->changed) {
                slider->changed(menu, item, pad->digital_buttons);
            }
            EduiSliderAcceleration += EduiSliderGlobalAcceleration;
            return 0;
        }
    } else {
        EduiSliderAcceleration = 1.0f;
        EduiSliderRepeatTimer = 30;
        u8 l2 = pad->analog_l2;
        if (l2) {
            if (buttons & PAD_BUTTON_CIRCLE) {
                EduiSliderDynamicScale *= (f64)1.05f;
                slider->normalized_value =
                    MAX(0.0f, slider->normalized_value - (u32)l2 * delta_time / EduiNominalFrameTime * EduiSliderDynamicScale);
            } else {
                EduiSliderDynamicScale = 1.6276043e-7f;
                slider->normalized_value =
                    (f32)MAX(0.0f, slider->normalized_value - (u32)l2 * delta_time / EduiNominalFrameTime * (f64)0.000016276043f);
            }
            slider->value = slider->normalized_value * slider->range + slider->minimum;
            slider->change_timer = 60;
            if (slider->changed) {
                slider->changed(menu, item, pad->digital_buttons);
            }
            return 0;
        }
        u8 r2 = pad->analog_r2;
        if (r2) {
            if (buttons & PAD_BUTTON_CIRCLE) {
                EduiSliderDynamicScale *= (f64)1.05f;
                slider->normalized_value =
                    MIN(1.0f, slider->normalized_value + (u32)r2 * delta_time / EduiNominalFrameTime * EduiSliderDynamicScale);
            } else {
                EduiSliderDynamicScale = 1.6276043e-7f;
                slider->normalized_value =
                    (f32)MIN(1.0f, slider->normalized_value + (u32)r2 * delta_time / EduiNominalFrameTime * (f64)0.000016276043f);
            }
            slider->value = slider->normalized_value * slider->range + slider->minimum;
            slider->change_timer = 60;
            if (slider->changed) {
                slider->changed(menu, item, pad->digital_buttons);
            }
        } else {
            EduiSliderDynamicScale = 1.6276043e-7f;
        }
    }
    return 0;
}

i32 eduicbRenderSlider(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 unused_selected) {
    edui_slider_s *slider = (edui_slider_s *)item;
    i32 height = (i32)(NuQFntHeight(edui_font) * 1.25) >> 3;
    f64 font_height = NuQFntHeight(edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline(edui_font));
    i32 skip_draw = edui_donotdraw;
    char format[512];
    item->x = x;
    item->y = y;
    if (skip_draw == 0) {
        NuRndrRect2di(x << 4, y << 3, width << 4, height << 4, item->colours[2 + item->highlighted], uimtls[ui_bgmtl]);
        EduiQFntSet(edui_font);
        EduiQFntSetColour(edui_font, item->colours[item->highlighted]);
    }
    NuStrCpy(format, item->text);
    NuStrCat(format, slider->format);
    EduiPrintAlignedText(edui_font, (x * 2 + width) << 3, (y << 3) + baseline, 64, format, slider->value);
    y += height;
    if (slider->normalized_value >= 0.0f && slider->normalized_value <= 1.0f) {
        if (edui_donotdraw == 0) {
            i32 bottom = (y + height - 1) << 3;
            i32 marker = (i32)(slider->normalized_value * (width - 2) + (x + 1)) << 4;
            i32 top = y << 3;
            NuRndrLine2di(marker, top, marker, bottom, item->colours[item->highlighted], uimtls[0]);
            if (edui_donotdraw == 0) {
                marker = (i32)(slider->normalized_value * (width - 2) + x) << 4;
                NuRndrLine2di(marker, top, marker, bottom, item->colours[item->highlighted], uimtls[0]);
                if (edui_donotdraw == 0) {
                    marker = (i32)(slider->normalized_value * (width - 2) + (x - 1)) << 4;
                    NuRndrLine2di(marker, top, marker, bottom, item->colours[item->highlighted], uimtls[0]);
                }
            }
        }
    }
    return height * 2;
}

i32 eduicbRenderSliderInt(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 unused_selected) {
    edui_slider_s *slider = (edui_slider_s *)item;
    i32 height = (i32)(NuQFntHeight(edui_font) * 1.25) >> 3;
    f64 font_height = NuQFntHeight(edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline(edui_font));
    i32 skip_draw = edui_donotdraw;
    char format[512];
    item->x = x;
    item->y = y;
    if (skip_draw == 0) {
        NuRndrRect2di(x << 4, y << 3, width << 4, height << 4, item->colours[2 + item->highlighted], uimtls[0]);
        EduiQFntSet(edui_font);
        EduiQFntSetColour(edui_font, item->colours[item->highlighted]);
    }
    NuStrCpy(format, item->text);
    NuStrCat(format, slider->format);
    EduiPrintAlignedText(edui_font, (x * 2 + width) << 3, (y << 3) + baseline, 64, format, (i32)slider->value);
    y += height;
    if (edui_donotdraw == 0) {
        i32 bottom = (y + height - 1) << 3;
        f32 span = (f32)(width);
        i32 marker = (i32)(span * slider->normalized_value + (x + 1)) << 4;
        i32 top = y << 3;
        NuRndrLine2di(marker, top, marker, bottom, item->colours[item->highlighted], uimtls[0]);
        if (edui_donotdraw == 0) {
            marker = (i32)(span * slider->normalized_value + x) << 4;
            NuRndrLine2di(marker, top, marker, bottom, item->colours[item->highlighted], uimtls[0]);
            if (edui_donotdraw == 0) {
                marker = (i32)(span * slider->normalized_value + (x - 1)) << 4;
                NuRndrLine2di(marker, top, marker, bottom, item->colours[item->highlighted], uimtls[0]);
            }
        }
    }
    return height * 2;
}

extern "C" i32 eduicbRenderNumber(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 unused_selected) {
    edui_slider_s *number = (edui_slider_s *)item;
    i32 height = (i32)(NuQFntHeight(edui_font) * 1.25) >> 3;
    f64 scaled_height = NuQFntHeight(edui_font) * 0.125;
    i32 baseline = (i32)(scaled_height + NuQFntBaseline(edui_font));
    item->x = x;
    item->y = y;
    EduiRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2 + item->highlighted], uimtls[ui_bgmtl]);
    EduiQFntSet(edui_font);
    EduiQFntSetColour(edui_font, item->colours[item->highlighted]);
    char format[512];
    NuStrCpy(format, item->text);
    NuStrCat(format, number->format);
    EduiPrintAlignedText(edui_font, x << 4, (y << 3) + baseline, 16, format, number->value);
    return height;
}

extern "C" i32 eduicbRenderTextSelector(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 unused_selected) {
    edui_text_selector_s *selector = (edui_text_selector_s *)item;
    i32 height = (i32)(NuQFntHeight(edui_font) * 1.25) >> 3;
    f64 scaled_height = NuQFntHeight(edui_font) * 0.125;
    i32 baseline = (i32)(scaled_height + NuQFntBaseline(edui_font));
    item->x = x;
    item->y = y;
    EduiRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2 + item->highlighted], uimtls[ui_bgmtl]);
    EduiQFntSet(edui_font);
    EduiQFntSetColour(edui_font, item->colours[item->highlighted]);
    char format[512];
    NuStrCpy(format, item->text);
    NuStrCat(format, ": %s");
    EduiPrintAlignedText(edui_font, x << 4, (y << 3) + baseline, 16, format, selector->options[(i32)selector->slider.value]);
    return height;
}

extern "C" {

    i32 eduiItemPropSetText(edui_prop_s *item, char *text) {
        if (item->property_text && NuStrLen(item->property_text) < NuStrLen(text)) {
            NuFree(item->property_text, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x1108);
            item->property_text = NULL;
        }
        if (item->property_text == NULL) {
            item->property_text = (char *)NuAlloc(NuStrLen(text) + 1, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x110d);
        }
        if (item->property_text) {
            NuStrCpy(item->property_text, text);
            return 1;
        }
        return 0;
    }

    i32 edui_camera_enabled = 1;

    i32 eduiGetCameraEnabled(void) {
        return edui_camera_enabled;
    }

    void eduiSetCameraEnabled(i32 enabled) {
        edui_camera_enabled = enabled;
    }

    /* Dragging the divider resizes the label column of every property item. */
    static i32 EduiPropDragLabel(edui_prop_s *property, eduimenu_s *menu) {
        eduiitem_s *other;
        if (property->dragging_label) {
            property->label_width = edui_cursor_x - property->base.x;
            if (property->label_width < 5.0f) {
                property->label_width = 5.0f;
            }
            for (other = menu->first; other != NULL; other = other->next) {
                if (other->type == EDUIITEM_PROP && other != &property->base) {
                    ((edui_prop_s *)other)->label_width = property->label_width;
                }
            }
            if (!(edui_cursor_buttons & PAD_BUTTON_CROSS)) {
                property->dragging_label = 0;
            }
        }
        return 0;
    }

    /* Runs the button callback while it is held (or the item is activated). */
    static i32 EduiPropButton(edui_prop_s *property, eduimenu_s *menu, FRAME_PAD *pad) {
        edui_camera_enabled = 1;
        if (property->button_held || (property->base.type == EDUIITEM_PROP && (pad->digital_buttons & PAD_BUTTON_CROSS))) {
            if (property->button) {
                edui_camera_enabled = 0;
                property->button(menu, &property->base, edui_cursor_buttons);
                property->button_held = 0;
            }
            if (!(edui_cursor_buttons & PAD_BUTTON_CROSS)) {
                property->button_held = 0;
            }
        }
        return 0;
    }

    // PC walks 8-byte entries to a zero key.
    NUKEYCHAR NuKeyChar[65] = {
        {0x2, 0x31, 0x21, {0, 0}},  {0x3, 0x32, 0x22, {0, 0}},  {0x4, 0x33, 0xa3, {0, 0}},  {0x5, 0x34, 0x24, {0, 0}},
        {0x6, 0x35, 0x25, {0, 0}},  {0x7, 0x36, 0x5e, {0, 0}},  {0x8, 0x37, 0x26, {0, 0}},  {0x9, 0x38, 0x2a, {0, 0}},
        {0xa, 0x39, 0x28, {0, 0}},  {0xb, 0x30, 0x29, {0, 0}},  {0xc, 0x2d, 0x5f, {0, 0}},  {0xd, 0x3d, 0x2b, {0, 0}},
        {0xf, 0x09, 0x09, {0, 0}},  {0x10, 0x71, 0x51, {0, 0}}, {0x11, 0x77, 0x57, {0, 0}}, {0x12, 0x65, 0x45, {0, 0}},
        {0x13, 0x72, 0x52, {0, 0}}, {0x14, 0x74, 0x54, {0, 0}}, {0x15, 0x79, 0x59, {0, 0}}, {0x16, 0x75, 0x55, {0, 0}},
        {0x17, 0x69, 0x49, {0, 0}}, {0x18, 0x6f, 0x4f, {0, 0}}, {0x19, 0x70, 0x50, {0, 0}}, {0x1a, 0x5b, 0x7b, {0, 0}},
        {0x1b, 0x5d, 0x7d, {0, 0}}, {0x1e, 0x61, 0x41, {0, 0}}, {0x1f, 0x73, 0x53, {0, 0}}, {0x20, 0x64, 0x44, {0, 0}},
        {0x21, 0x66, 0x46, {0, 0}}, {0x22, 0x67, 0x47, {0, 0}}, {0x23, 0x68, 0x48, {0, 0}}, {0x24, 0x6a, 0x4a, {0, 0}},
        {0x25, 0x6b, 0x4b, {0, 0}}, {0x26, 0x6c, 0x4c, {0, 0}}, {0x27, 0x3b, 0x3a, {0, 0}}, {0x28, 0x27, 0x40, {0, 0}},
        {0x2b, 0x5c, 0x3f, {0, 0}}, {0x2c, 0x7a, 0x5a, {0, 0}}, {0x2d, 0x78, 0x58, {0, 0}}, {0x2e, 0x63, 0x43, {0, 0}},
        {0x2f, 0x76, 0x56, {0, 0}}, {0x30, 0x62, 0x42, {0, 0}}, {0x31, 0x6e, 0x4e, {0, 0}}, {0x32, 0x6d, 0x4d, {0, 0}},
        {0x33, 0x2c, 0x3c, {0, 0}}, {0x34, 0x2e, 0x3e, {0, 0}}, {0x35, 0x2f, 0x00, {0, 0}}, {0x37, 0x2a, 0x2a, {0, 0}},
        {0x39, 0x20, 0x20, {0, 0}}, {0x47, 0x37, 0x37, {0, 0}}, {0x48, 0x38, 0x38, {0, 0}}, {0x49, 0x39, 0x39, {0, 0}},
        {0x4a, 0x2d, 0x2d, {0, 0}}, {0x4b, 0x34, 0x34, {0, 0}}, {0x4c, 0x35, 0x35, {0, 0}}, {0x4d, 0x36, 0x36, {0, 0}},
        {0x4e, 0x2b, 0x2b, {0, 0}}, {0x4f, 0x31, 0x31, {0, 0}}, {0x50, 0x32, 0x32, {0, 0}}, {0x51, 0x33, 0x33, {0, 0}},
        {0x52, 0x30, 0x30, {0, 0}}, {0x53, 0x2e, 0x2e, {0, 0}}, {0x8d, 0x3d, 0x3d, {0, 0}}, {0xb5, 0x2f, 0x2f, {0, 0}},
        {0x0, 0x00, 0x00, {0, 0}},
    };

    char NuKeyToAscii(u32 key, i32 shifted) {
        NUKEYCHAR *entry;

        for (entry = NuKeyChar; entry->key != 0; ++entry) {
            if (entry->key == key) {
                if (shifted) {
                    return entry->shifted;
                }
                return entry->normal;
            }
        }
        return 0;
    }

    static i32 EduiPropOverDivider(edui_interact_s *interact) {
        f32 edge = ((edui_prop_s *)interact->item)->label_width + interact->x - 1.0f;
        if (edui_cursor_x >= edge && edui_cursor_y >= interact->y) {
            edge = ((edui_prop_s *)interact->item)->label_width + interact->x + 1.0f;
            if (edui_cursor_x < edge) {
                edge = interact->height + interact->y;
                if (edui_cursor_y < edge) {
                    return 1;
                }
            }
        }
        return 0;
    }

    static i32 EduiPropOverLabel(edui_interact_s *interact) {
        f32 edge;
        if (edui_cursor_x >= interact->x && edui_cursor_y >= interact->y) {
            edge = ((edui_prop_s *)interact->item)->label_width + interact->x - 1.0;
            if (edui_cursor_x < edge) {
                edge = interact->height + interact->y;
                if (edui_cursor_y < edge) {
                    return 1;
                }
            }
        }
        return 0;
    }

    static i32 EduiPropOverValue(edui_interact_s *interact) {
        edui_prop_s *property = (edui_prop_s *)interact->item;
        f32 edge = property->label_width + interact->x + 1.0;
        if (edui_cursor_x >= edge && edui_cursor_y >= interact->y) {
            if (edui_cursor_x < property->button_x) {
                edge = interact->height + interact->y;
                if (edui_cursor_y < edge) {
                    return 1;
                }
            }
        }
        return 0;
    }

    static i32 EduiPropOverButton(edui_prop_s *property) {
        return edui_cursor_x >= property->button_x && edui_cursor_y >= property->button_y * 0.5 &&
               edui_cursor_x < property->button_x + property->button_size &&
               edui_cursor_y < (property->button_y + property->button_size) * 0.5;
    }

    i32 eduiPropTextPos = -1;
    /* Property items: a label, a divider the cursor can drag, an editable value
     * and an optional button (1 arrow, 2 up/down arrows, 3 colour swatch). */
    static char eduiPropTextStore[256] = {0};
    static char eduiPropTextEdit[256] = {0};
    struct edui_interact_s *eduiInteractLocked = 0;

    static i32 eduicbInteractProp(edui_interact_s *interact) {
        eduimenu_s *menu = interact->menu;
        edui_prop_s *property = (edui_prop_s *)interact->item;
        i32 handled = 0;
        if (edui_cursor_buttons & PAD_BUTTON_CROSS) {
            if (!property->dragging_label && !property->button_held) {
                if (!property->editing) {
                    if (EduiPropOverLabel(interact) && property->selected) {
                        property->selected(menu, &property->base, 0);
                    }
                    if (EduiPropOverDivider(interact)) {
                        property->dragging_label = 1;
                        property->divider_active = 1;
                    }
                    if (EduiPropOverValue(interact) && !property->editing) {
                        if (property->selected) {
                            property->selected(menu, &property->base, 0);
                        }
                        NuKeyFlush();
                        property->editing = 1;
                        NuStrCpy(eduiPropTextStore, property->property_text);
                        NuStrCpy(eduiPropTextEdit, property->property_text);
                        eduiPropTextPos = NuStrLen(property->property_text);
                    }
                    if ((EduiPropOverButton(property) || eduiInteractLocked) && property->button) {
                        property->button(menu, &property->base, 0);
                        if (property->button_latched) {
                            handled = 1;
                        }
                    }
                } else if (EduiPropOverValue(interact)) {
                    property->editing = 1;
                    eduiPropTextPos = -1;
                    handled = 1;
                }
            }
        }
        return property->editing || property->dragging_label || property->button_held || handled;
    }

    static void eduicbItemDestroyProp(eduimenu_s *menu, eduiitem_s *item) {
        edui_prop_s *property = (edui_prop_s *)item;
        if (item->text) {
            NuFree(item->text, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x1313);
            item->text = NULL;
        }
        if (property->property_text) {
            NuFree(property->property_text, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x1318);
            property->property_text = NULL;
        }
        NuFree(item, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x131c);
        menu->first_visible = menu->first;
    }
}

extern "C" i32 eduicbProcessSeparator(eduimenu_s *unused_menu, eduiitem_s *unused_item, f32 unused_delta_time, FRAME_PAD *unused_pad) {
    return 0;
}

extern "C" i32 eduicbRenderSeparator(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 unused_selected) {
    NuQFntHeight(edui_font);
    NuQFntBaseline(edui_font);
    i32 skip_draw = edui_donotdraw;
    item->x = x;
    item->y = y;
    if (skip_draw == 0) {
        NuRndrRect2di(x << 4, y << 3, width << 4, 64, item->colours[2 + item->highlighted], uimtls[ui_bgmtl]);
        EduiRndrLine2di((x + 4) << 4, (y << 3) + 32, ((x + width) << 4) - 64, (y << 3) + 32, (i32)0xff000000, uimtls[ui_bgmtl]);
    }
    return 8;
}

extern "C" {

    static void EduiExpanderOpen(EDUI_EXPANDER *expander) {
        if (expander->first_child && !expander->open) {
            expander->last_child->next = expander->base.next;
            if (expander->base.next) {
                expander->base.next->previous = expander->last_child;
            }
            expander->base.next = expander->first_child;
            expander->first_child->previous = &expander->base;
            expander->open = 1;
        }
    }

    void eduicbItemExpanderClose(eduiitem_s *item) {
        EDUI_EXPANDER *expander = (EDUI_EXPANDER *)item;
        eduiitem_s *child;
        for (child = expander->first_child; child; child = child->next) {
            if (child->type == EDUIITEM_EXPANDER) {
                eduicbItemExpanderClose(child);
            }
            if (child == expander->last_child) {
                break;
            }
        }
        if (expander->first_child && expander->open) {
            expander->base.next = expander->last_child->next;
            if (expander->last_child->next) {
                expander->last_child->next->previous = &expander->base;
            }
            expander->first_child->previous = NULL;
            expander->open = 0;
        }
    }

    void eduicbMenuOpenAllexpanders(eduimenu_s *menu) {
        eduiitem_s *item;
        for (item = menu->first; item != NULL; item = item->next) {
            if (item->type == EDUIITEM_EXPANDER) {
                EDUI_EXPANDER *expander = (EDUI_EXPANDER *)item;
                if (expander->first_child != NULL && !expander->open) {
                    expander->last_child->next = expander->base.next;
                    if (expander->base.next != NULL) {
                        expander->base.next->previous = expander->last_child;
                    }
                    expander->base.next = expander->first_child;
                    expander->first_child->previous = &expander->base;
                    expander->open = 1;
                }
            }
        }
    }

    void eduicbMenuCloseAllexpanders(eduimenu_s *menu) {
        eduiitem_s *item;
        for (item = menu->last; item; item = item->previous) {
            if (item->type == EDUIITEM_EXPANDER) {
                eduicbItemExpanderClose(item);
            }
        }
    }

    /* Is the editor cursor over the expander's open/close button? */
    static i32 EduiExpanderOverButton(EDUI_EXPANDER *expander) {
        // MATCH-EXCEPTION: parameter copy, reversed declarations, a local for button_x and mirrored
        // compares; only this form gives the original x87 operand order.
        EDUI_EXPANDER *e = expander;
        f32 bottom = (e->button_y + e->button_size) * 0.5;
        f32 right = e->button_size + e->button_x;
        f32 top = e->button_y * 0.5;
        f32 x = e->button_x;
        if (edui_cursor_x >= x && edui_cursor_y >= top && right > edui_cursor_x && bottom > edui_cursor_y) {
            return 1;
        }
        return 0;
    }

    static i32 eduicbProcessExpander(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD *pad) {
        EDUI_EXPANDER *expander = (EDUI_EXPANDER *)item;
        if (pad->digital_buttons_pressed & PAD_BUTTON_CROSS) {
            if (expander->changed) {
                expander->changed(menu, item, 0);
            }
            if (expander->first_child) {
                if (expander->open) {
                    eduicbItemExpanderClose(item);
                } else {
                    EduiExpanderOpen(expander);
                }
            }
        }
        return 0;
    }

    static i32 eduicbInteractExpander(edui_interact_s *interact) {
        EDUI_EXPANDER *expander = (EDUI_EXPANDER *)interact->item;
        if (edui_cursor_buttons_db & PAD_BUTTON_CROSS) {
            if (expander->changed) {
                expander->changed(interact->menu, &expander->base, 0);
            }
            if (EduiExpanderOverButton(expander) && expander->first_child) {
                if (expander->open) {
                    eduicbItemExpanderClose(&expander->base);
                } else {
                    EduiExpanderOpen(expander);
                }
            }
        }
        return 0;
    }

    static void eduicbItemDestroyExpander(eduimenu_s *menu, eduiitem_s *item) {
        EDUI_EXPANDER *expander = (EDUI_EXPANDER *)item;
        if (expander->last_child) {
            expander->last_child->next = NULL;
        }
        while (expander->first_child) {
            eduiitem_s *next = expander->first_child->next;
            expander->first_child->destroy(menu, expander->first_child);
            expander->first_child = next;
        }
    }

    void eduiIitemExpanderSetDepth(eduiitem_s *item, i32 depth) {
        EDUI_EXPANDER *expander = (EDUI_EXPANDER *)item;
        eduiitem_s *child;
        expander->depth = depth;
        for (child = expander->first_child; child; child = child->next) {
            if (child->type == EDUIITEM_EXPANDER) {
                eduiIitemExpanderSetDepth(child, depth + 1);
            }
            if (child->type == EDUIITEM_PROP) {
                ((edui_prop_s *)child)->depth = depth + 1;
            }
            if (child == expander->last_child) {
                break;
            }
        }
    }

    void eduiItemExpanderAddChild(eduiitem_s *item, eduiitem_s *child) {
        EDUI_EXPANDER *expander = (EDUI_EXPANDER *)item;
        child->previous = expander->last_child;
        if (expander->last_child) {
            expander->last_child->next = child;
        }
        expander->last_child = child;
        if (!expander->first_child) {
            expander->first_child = child;
        }
        child->next = NULL;
        if (child->type == EDUIITEM_EXPANDER) {
            eduiIitemExpanderSetDepth(child, expander->depth + 1);
        }
        if (child->type == EDUIITEM_PROP) {
            ((edui_prop_s *)child)->depth = expander->depth + 1;
        }
    }

    void eduiItemFilterAddChild(eduiitem_s *item, eduiitem_s *child) {
        edui_filter_s *filter = (edui_filter_s *)item;
        child->previous = NULL;
        child->next = filter->first_child;
        if (filter->first_child) {
            filter->first_child->previous = child;
        }
        filter->first_child = child;
    }

    void eduiItemFilterRemoveChild(eduiitem_s *item, eduiitem_s *child) {
        edui_filter_s *filter = (edui_filter_s *)item;
        if (filter->first_child == child) {
            filter->first_child = child->next;
        }
        if (child->next) {
            child->next->previous = child->previous;
        }
        if (child->previous) {
            child->previous->next = child->next;
        }
        child->next = NULL;
        child->previous = NULL;
    }

    static i32 eduicbInteractFilter(edui_interact_s *interact) {
        return eduicbInteractProp(interact);
    }

    static void eduicbItemDestroyFilter(eduimenu_s *menu, eduiitem_s *item) {
        edui_filter_s *filter = (edui_filter_s *)item;
        eduiitem_s *child;
        eduiitem_s *next;
        while (filter->first_child != NULL) {
            child = filter->first_child;
            next = child->next;
            child->destroy(menu, child);
            filter->first_child = next;
        }
        eduicbItemDestroyProp(menu, item);
    }

    i32 EduiRenderWidth = 0;
    u32 EduiItemOutlineColour = 0x80ffffff;
    u32 EduiMarkedItemOutlineColour = 0x0000ff00;

    i32 EduiRenderItem(eduiitem_s *item, eduimenu_s *menu, i32 x, i32 y, i32 width, i32 selected) {
        i32 height;
        EduiRenderWidth = width;
        height = item->render(menu, item, x, y, width, selected);
        if (selected != 0) {
            i32 marked = (item->flags & 4) != 0;
            u32 colour = EduiMarkedItemOutlineColour;
            if (!marked) {
                colour = EduiItemOutlineColour;
            }
            if (!edui_donotdraw) {
                NuRndrLineRect2di(x << 4, (y << 3) + 8, EduiRenderWidth << 4, (height << 3) - 16, colour, uimtls[EduiSelectionMaterial]);
            }
        }
        EduiDrawDisabledItem(item, x, y, width, height);
        return height;
    }

    i32 EduiUsingMenuFocus = 0;

    void eduiSetUsingMenuFocus(i32 enabled) {
        EduiUsingMenuFocus = enabled;
    }

    i32 eduiGetUsingMenuFocus(void) {
        return EduiUsingMenuFocus;
    }
    struct eduimenu_s *default_active_menu = 0;

    void eduiSetDefaultActiveMenu(eduimenu_s *menu) {
        default_active_menu = menu;
    }

    eduimenu_s *eduiGetActiveMenu(void) {
        return active_menu;
    }

    eduimenu_s *eduiGetTopLevelParent(eduimenu_s *menu) {
        if (menu == NULL || menu->parent == NULL) {
            return menu;
        }
        return eduiGetTopLevelParent(menu->parent);
    }

    eduimenu_s *eduiGetActiveMenuParent(void) {
        eduimenu_s *menu = active_menu;
        return eduiGetTopLevelParent(menu);
    }

    /* The menu itself or one of its open children when it holds the focus. */
    static eduimenu_s *EduiMenuChainActive(eduimenu_s *menu) {
        if (menu == NULL) {
            return NULL;
        }
        if (menu == active_menu) {
            return menu;
        }
        return EduiMenuChainActive(menu->child);
    }

    static eduimenu_s *EduiDeepestChild(eduimenu_s *menu) {
        if (menu != NULL) {
            eduimenu_s *child = menu->child;
            if (child == NULL) {
                return menu;
            }
            return EduiDeepestChild(menu->child);
        }
        return NULL;
    }

    /* Without menu focus every menu takes input; with it only the active one. */
    i32 EduiMenuHasFocus(eduimenu_s *menu) {
        return !EduiUsingMenuFocus || menu == active_menu;
    }

    i32 EduiMenuProcessSelected(eduimenu_s *menu, f32 delta_time, FRAME_PAD *pad) {
        if (menu && menu->selected && !menu->selected->disabled && menu->selected->process) {
            return menu->selected->process(menu, menu->selected, delta_time, pad);
        }
        return 0;
    }

    void EduiMenuEnsureSelected(eduimenu_s *menu) {
        if (!menu->selected) {
            menu->selected = menu->first;
        }
    }

    i32 EduiMenuContainsCursor(eduimenu_s *menu) {
        if (edui_cursor_x >= menu->x && edui_cursor_y >= menu->y && edui_cursor_x < menu->x + menu->width &&
            edui_cursor_y < menu->y + menu->height) {
            return 1;
        }
        return 0;
    }

    i32 selection_value = 0;

    void cbSel(eduimenu_s *unused0, eduiitem_s *item) {
        selection_value = item->data;
    }

    void cbGradChange(void) {
    }

    i32 cbInteractMenuTitle(edui_interact_s *interact) {
        static f32 offsetx = 0;
        static f32 offsety = 0;
        i32 result = (edui_cursor_buttons & EDUI_CURSOR_PRIMARY) != 0;
        eduimenu_s *menu = interact->menu;
        if (!(menu->flags & EDUI_MENU_FIXED)) {
            f32 mouse_x = (f32)(NuMouseReadX() * 640.0);
            f32 mouse_y = (f32)(NuMouseReadY() * 448.0);
            if (!eduiInteractLocked) {
                offsetx = mouse_x - menu->x;
                offsety = mouse_y - menu->y;
            }
            menu->x = (i32)(mouse_x - offsetx);
            menu->y = (i32)(mouse_y - offsety);
        }
        return result;
    }

    i32 cbInteractMenuScrollUp(edui_interact_s *interact) {
        eduimenu_s *menu = interact->menu;
        if (menu->first_visible) {
            if (menu->first_visible->previous) {
                menu->first_visible = menu->first_visible->previous;
            }
            menu->selected = menu->first_visible;
        }
        return 0;
    }

    i32 cbInteractMenuScrollDown(edui_interact_s *interact) {
        eduimenu_s *menu = interact->menu;
        if (menu->last_visible) {
            if (menu->last_visible->next) {
                menu->last_visible = menu->last_visible->next;
            }
            menu->selected = menu->last_visible;
        }
        return 0;
    }

    void cbInteractMenuScrollTo(eduimenu_s *menu, char *text) {
        i32 comparison;
        if (menu == NULL || text == NULL || text[0] == 0) {
            return;
        }
        comparison = -1;
        if (menu->first != NULL && menu->first->text != NULL) {
            comparison = NuStrICmp(menu->first->text, text);
        }
        menu->first_visible = menu->first;
        menu->selected = menu->first;
        while (menu->selected->next != NULL && comparison < 0) {
            menu->selected = menu->selected->next;
            if (menu->selected->text != NULL) {
                comparison = NuStrICmp(menu->selected->text, text);
            } else {
                comparison = -1;
            }
        }
        menu->last_visible = menu->selected;
    }

    i32 cbInteractMenuCancelChild(edui_interact_s *interact) {
        eduimenu_s *menu = interact->menu;
        if (menu != NULL) {
            eduimenu_s *child = menu->child;
            if (child != NULL && child->callback != NULL) {
                child->callback(child, menu);
            }
        }
        return 0;
    }

    i32 numInteracts = 0;
    edui_interact_s eduiInteracts[64] = {0};
}

static void EduiAddInteraction(f32 x, f32 y, f32 width, f32 height, eduimenu_s *menu, eduiitem_s *item, i32 (*callback)(edui_interact_s *),
                               u32 buttons, i32 needs_debounce) {
    if (numInteracts < 64 && eduiInteractLocked == NULL) {
        eduiInteracts[numInteracts].x = x;
        eduiInteracts[numInteracts].y = (f32)(y * 0.5);
        eduiInteracts[numInteracts].width = width;
        eduiInteracts[numInteracts].height = (f32)(height * 0.5);
        eduiInteracts[numInteracts].menu = menu;
        eduiInteracts[numInteracts].item = item;
        eduiInteracts[numInteracts].callback = callback;
        eduiInteracts[numInteracts].buttons = buttons;
        eduiInteracts[numInteracts].needs_debounce = needs_debounce;
        ++numInteracts;
    }
}

extern "C" {

    static void EduiClearInteractions(eduimenu_s *menu, eduiitem_s *item) {
        edui_interact_s *interact = eduiInteracts;
        i32 remaining = numInteracts;
        if (remaining > 0) {
            do {
                if (interact->menu == menu && interact->item == item) {
                    interact->menu = NULL;
                    interact->item = NULL;
                }
                ++interact;
            } while (--remaining != 0);
        }
    }

    void eduiFlushInteracts(void) {
        numInteracts = 0;
    }

    /* Drops a locked interaction whose owner went away. */
    void EduiReleaseStaleInteract(void) {
        if (eduiInteractLocked && (!eduiInteractLocked->menu || !eduiInteractLocked->item) && eduiInteractLocked->needs_debounce) {
            eduiInteractLocked = NULL;
        }
    }

    static i32 EduiProcessLockedInteraction(eduimenu_s *menu) {
        eduimenu_s *locked_menu = eduiInteractLocked->menu;
        if (menu && (menu == locked_menu || menu->parent == locked_menu)) {
            eduiInteractLocked = eduiInteractLocked->callback(eduiInteractLocked) ? eduiInteractLocked : NULL;
        }
        return eduiInteractLocked != NULL;
    }

    static void EduiInteractSelectItem(edui_interact_s *interact) {
        if (interact && interact->menu && interact->item && interact->item->type != EDUIITEM_SEPARATOR) {
            interact->menu->selected = interact->item;
        }
    }

    static i32 EduiInteractPressed(edui_interact_s *interact) {
        return (interact->buttons & edui_cursor_buttons_db) != 0;
    }

    static i32 EduiInteractionContainsCursor(edui_interact_s *interact) {
        f32 edge;
        if (edui_cursor_x >= interact->x && edui_cursor_y >= interact->y) {
            edge = interact->width + interact->x;
            if (edui_cursor_x < edge) {
                edge = interact->height + interact->y;
                if (edui_cursor_y < edge) {
                    return 1;
                }
            }
        }
        return 0;
    }

    void eduiRenderInteracts(void) {
        edui_interact_s *hovered;
        edui_interact_s *interact;
        i32 count;
        i32 i;
        u32 colour;
        f32 edge;

        if (numInteracts <= 0) {
            return;
        }

        hovered = NULL;
        EduiPushCoordinateSystem(0);
        NuPrim2DBegin(14, 5, uimtls[0]);
        interact = eduiInteractLocked;
        if (interact != NULL) {
            count = 1;
        } else {
            count = numInteracts;
            interact = eduiInteracts;
        }
        for (i = 0; i < count; ++i, ++interact) {
            colour = NuRandInt();
            EduiPrimSetColour(colour);
            NuPrim2DAddXYZ(interact->x, interact->y, 0.0f);
            EduiPrimSetColour(colour);
            NuPrim2DAddXYZ(interact->x + interact->width, interact->y, 0.0f);
            EduiPrimSetColour(colour);
            NuPrim2DAddXYZ(interact->x + interact->width, interact->y, 0.0f);
            EduiPrimSetColour(colour);
            NuPrim2DAddXYZ(interact->x + interact->width, interact->height + interact->y, 0.0f);
            EduiPrimSetColour(colour);
            NuPrim2DAddXYZ(interact->x + interact->width, interact->height + interact->y, 0.0f);
            EduiPrimSetColour(colour);
            NuPrim2DAddXYZ(interact->x, interact->height + interact->y, 0.0f);
            EduiPrimSetColour(colour);
            NuPrim2DAddXYZ(interact->x, interact->height + interact->y, 0.0f);
            EduiPrimSetColour(colour);
            NuPrim2DAddXYZ(interact->x, interact->y, 0.0f);
            if (edui_cursor_x >= interact->x && edui_cursor_y >= interact->y) {
                edge = interact->x + interact->width;
                if (edge > edui_cursor_x) {
                    edge = interact->height + interact->y;
                    if (edge > edui_cursor_y) {
                        hovered = interact;
                    }
                }
            }
        }
        NuPrim2DEnd();
        if (hovered != NULL) {
            NuPrim2DBegin(16, 5, uimtls[3]);
            colour = (NuRandInt() & 0x00ffffff) | 0x40000000;
            EduiPrimSetColour(colour);
            NuPrim2DAddXYZ(hovered->x, hovered->y, 0.0f);
            EduiPrimSetColour(colour);
            NuPrim2DAddXYZ(hovered->width + hovered->x, hovered->height + hovered->y, 0.0f);
            NuPrim2DEnd();
        }
        NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[--NuPrimCSPos]);
    }

    static i32 bShowCursor = 1;

    void eduiShowCursor(i32 show) {
        bShowCursor = show;
    }

    void eduiGetCursorCoords(f32 *x, f32 *y) {
        *x = edui_cursor_x / 640.0;
        *y = edui_cursor_y / 224.0;
    }

    void eduiSetCursorCoords(f32 x, f32 y) {
        edui_cursor_x = x * 640.0;
        edui_cursor_y = y * 224.0;
    }

    void eduiGetCursorDelta(f32 *x, f32 *y) {
        *x = edui_cursor_dx / 640.0;
        *y = edui_cursor_dy / 224.0;
    }

    /* 1 when the left stick is off centre, 2 for the right stick. */
    i32 eduiUsedAlgPad(FRAME_PAD *pad) {
        if (pad != NULL) {
            if (pad->analog_left_x < 0x40 || pad->analog_left_x > 0xc0 || pad->analog_left_y < 0x40 || pad->analog_left_y > 0xc0) {
                return 1;
            }
            if (pad->analog_right_x < 0x40 || pad->analog_right_x > 0xc0 || pad->analog_right_y < 0x40 || pad->analog_right_y > 0xc0) {
                return 2;
            }
        }
        return 0;
    }

    /* The PC cursor follows the mouse elsewhere; this only keeps it on screen. */
    void EduiClampCursor(void) {
        if (edui_cursor_x < 0.0f) {
            edui_cursor_x = 0.0f;
        } else if (edui_cursor_x > 640.0f) {
            edui_cursor_x = 640.0f;
        }
        if (edui_cursor_y < 0.0f) {
            edui_cursor_y = 0.0f;
        }
        if (edui_cursor_y > 224.0) {
            edui_cursor_y = 224.0f;
        }
    }

    u32 edui_cursor_colour = 0xff000000;

    void eduiSetCursorColour(u32 colour) {
        edui_cursor_colour = colour;
    }

    void eduiRenderCursor(void) {
    }

    void eduiInit(void) {
        eduiInitMaterials();
        eduiFilePickAllocDirList();
    }

    void eduiSetFont(struct vufnt_s *font) {
        edui_font = font;
    }

    f32 edui_font_scale_x = 0.9f;
    f32 edui_font_scale_y = 0.9f;

    void eduiSetFontScale(f32 x, f32 y) {
        edui_font_scale_x = x;
        edui_font_scale_y = y;
    }
}

static void EduiColourSliderSet(edui_slider_s *slider, eduimenu_s *menu, eduiitem_s *item, f32 value, FRAME_PAD *pad) {
    slider->value = value;
    slider->normalized_value = (value - slider->minimum) / slider->range;
    slider->change_timer = 60;
    if (slider->changed) {
        slider->changed(menu, item, pad->digital_buttons);
    }
}

static void EduiColourSliderRound(edui_slider_s *slider, eduimenu_s *menu, eduiitem_s *item, FRAME_PAD *pad, f32 value) {
    f32 step = slider->granularity;
    if (value > 0.0f) {
        value += step * 0.5;
    } else {
        value -= step * 0.5;
    }
    value = (i32)(value / step) * step;
    slider->value = value;
    slider->normalized_value = (value - slider->minimum) / slider->range;
    slider->change_timer = 60;
    if (slider->changed) {
        slider->changed(menu, item, pad->digital_buttons);
    }
}

extern "C" {
    i32 EduiColourSliderRepeatTimer = 30;
    f32 EduiColourSliderDynamicScale = 0.0f;
}

extern "C" i32 eduicbProcessColourSlider(eduimenu_s *menu, eduiitem_s *item, f32 unused_delta_time, FRAME_PAD *pad) {
    edui_slider_s *slider = (edui_slider_s *)item;
    if ((pad->digital_buttons & PAD_BUTTON_L1) && (pad->digital_buttons & PAD_BUTTON_R1)) {
        if (slider && slider->minimum <= 0.0f && slider->minimum + slider->range >= 0.0) {
            EduiColourSliderSet(slider, menu, item, 0.0f, pad);
        }
        EduiColourSliderRepeatTimer = 30;
    } else if ((pad->digital_buttons & PAD_BUTTON_L2) && (pad->digital_buttons & PAD_BUTTON_R2)) {
        if (slider && slider->minimum <= 0.0f && slider->minimum + slider->range >= 255.0) {
            EduiColourSliderSet(slider, menu, item, 255.0f, pad);
        }
        EduiColourSliderRepeatTimer = 30;
    } else if (pad->digital_buttons & PAD_BUTTON_L1) {
        i32 timer = EduiColourSliderRepeatTimer - 1;
        if (timer < 0) {
            EduiColourSliderRepeatTimer = 0;
        } else {
            EduiColourSliderRepeatTimer = timer;
        }
        if (!EduiColourSliderRepeatTimer || (pad->digital_buttons_pressed & PAD_BUTTON_L1)) {
            f32 value = MAX(slider->minimum, slider->value - slider->granularity);
            EduiColourSliderRound(slider, menu, item, pad, value);
        }
    } else if (pad->digital_buttons & PAD_BUTTON_R1) {
        i32 timer = EduiColourSliderRepeatTimer - 1;
        if (timer < 0) {
            EduiColourSliderRepeatTimer = 0;
        } else {
            EduiColourSliderRepeatTimer = timer;
        }
        if (!EduiColourSliderRepeatTimer || (pad->digital_buttons_pressed & PAD_BUTTON_R1)) {
            f32 value = MIN(slider->minimum + slider->range, slider->value + slider->granularity);
            EduiColourSliderRound(slider, menu, item, pad, value);
        }
    } else {
        EduiColourSliderRepeatTimer = 30;
        u8 l2 = pad->analog_l2;
        if (l2) {
            if (pad->analog_circle) {
                EduiColourSliderDynamicScale *= (f64)1.05f;
                slider->normalized_value = MAX(0.0f, slider->normalized_value - (u32)l2 * EduiColourSliderDynamicScale);
            } else {
                EduiColourSliderDynamicScale = 1.6276043e-7f;
                slider->normalized_value = (f32)MAX(0.0f, slider->normalized_value - (u32)l2 * (f64)0.000016276043f);
            }
            slider->value = slider->normalized_value * slider->range + slider->minimum;
            slider->change_timer = 60;
            if (slider->changed) {
                slider->changed(menu, item, pad->digital_buttons);
            }
            return 0;
        }
        u8 r2 = pad->analog_r2;
        if (r2) {
            if (pad->analog_circle) {
                EduiColourSliderDynamicScale *= (f64)1.05f;
                slider->normalized_value = MIN(1.0f, slider->normalized_value + (u32)r2 * EduiColourSliderDynamicScale);
            } else {
                EduiColourSliderDynamicScale = 1.6276043e-7f;
                slider->normalized_value = (f32)MIN(1.0f, slider->normalized_value + (u32)r2 * (f64)0.000016276043f);
            }
            slider->value = slider->normalized_value * slider->range + slider->minimum;
            slider->change_timer = 60;
            if (slider->changed) {
                slider->changed(menu, item, pad->digital_buttons);
            }
        } else {
            EduiColourSliderDynamicScale = 1.6276043e-7f;
        }
    }
    return 0;
}

i32 eduicbRenderColourSlider(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 unused_selected) {
    edui_colour_slider_s *slider = (edui_colour_slider_s *)item;
    i32 height = (i32)(NuQFntHeight(edui_font) * 1.25) >> 3;
    f64 font_height = NuQFntHeight(edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline(edui_font));
    i32 skip_draw = edui_donotdraw;
    item->x = x;
    item->y = y;
    if (skip_draw == 0) {
        NuRndrRect2di(x << 4, y << 3, width << 4, height << 4, item->colours[2 + item->highlighted], uimtls[0]);
        EduiQFntSet(edui_font);
        EduiQFntSetColour(edui_font, item->colours[item->highlighted]);
    }
    EduiPrintAlignedText(edui_font, (x + 64) << 4, (y << 3) + baseline, 64, item->text);
    EduiPrintAlignedText(edui_font, (x + 116) << 4, (y << 3) + baseline, 64, slider->base.format, (i32)slider->base.value);
    u32 colour = slider->red + (slider->green << 8) + (slider->blue << 16);
    y += height;
    EduiDrawGradientSpan(x, y, width, height, 0xff000000, colour, 0.0f, 1.0f);
    if (edui_donotdraw == 0) {
        i32 bottom = (y + height - 1) << 3;
        i32 marker = (i32)(slider->base.normalized_value * width + (x + 1)) << 4;
        i32 top = y << 3;
        NuRndrLine2di(marker, top, marker, bottom, item->colours[item->highlighted], uimtls[0]);
        if (edui_donotdraw == 0) {
            marker = (i32)(slider->base.normalized_value * width + x) << 4;
            NuRndrLine2di(marker, top, marker, bottom, item->colours[item->highlighted], uimtls[0]);
            if (edui_donotdraw == 0) {
                marker = (i32)(slider->base.normalized_value * width + (x - 1)) << 4;
                NuRndrLine2di(marker, top, marker, bottom, item->colours[item->highlighted], uimtls[0]);
            }
        }
    }
    return height * 2;
}

extern "C" {
    edcam_s *gp_cam = &edcam_default;

    void edcamSetSpeedPos(f32 position_x, f32 position_y, f32 position_z) {
        edcam_s *camera = gp_cam;
        camera->position_speed.x = position_x;
        camera->position_speed.y = position_y;
        camera->position_speed.z = position_z;
    }

    void EdCamSetSpeeds(f32 position_x, f32 position_y, f32 position_z, f32 distance) {
        edcam_s *camera = gp_cam;
        camera->position_speed.x = position_x;
        camera->position_speed.y = position_y;
        camera->position_speed.z = position_z;
        camera->distance_speed = distance;
    }

    void edcamSetAutoSpeed(f32 move_base, f32 move_distance_scale, f32 zoom_base, f32 zoom_distance_scale) {
        edcam_s *camera = gp_cam;
        camera->auto_move_base = move_base;
        camera->auto_move_dist_scale = move_distance_scale;
        camera->auto_zoom_base = zoom_base;
        camera->auto_zoom_dist_scale = zoom_distance_scale;
    }

    void edcamSetMouseSensitivity(f32 pitch, f32 yaw, f32 movement) {
        edcam_s *camera = gp_cam;
        camera->mouse_pitch_speed = pitch;
        camera->mouse_yaw_speed = yaw;
        camera->mouse_move_speed = movement;
    }

    void edcamSetAdjustFreedom(i32 position_x, i32 position_y, i32 position_z, i32 pitch, i32 yaw, i32 distance) {
        edcam_s *camera = gp_cam;
        camera->allow_position_x = position_x;
        camera->allow_position_y = position_y;
        camera->allow_position_z = position_z;
        camera->allow_pitch = pitch;
        camera->allow_yaw = yaw;
        camera->allow_distance = distance;
    }

    /* Private Source proposal; append in keyboard_read.c after old source. */
    void do_Pad_flymode_camera(edcam_s *camera, f32 delta_time, FRAME_PAD *pad) {
        f32 frame_scale = (f32)(delta_time / (f64)(1.0f / 60.0f));
        f32 move_speed;
        f32 zoom_speed;
        NUVEC offset;
        NUMTX rotation;
        NUVEC old_opposite;
        NUVEC new_opposite;
        i32 yaw_delta;
        i32 pitch_delta;

        if (camera->auto_move_dist_scale != 0.0f) {
            move_speed = EdcamFabs(camera->distance) * camera->auto_move_dist_scale + camera->auto_move_base;
        } else {
            move_speed = 1.0f;
        }
        if (camera->auto_zoom_dist_scale != 0.0f) {
            zoom_speed = camera->auto_zoom_dist_scale * EdcamFabs(camera->distance) + camera->auto_zoom_base;
        } else {
            zoom_speed = 1.0f;
        }
        EdcamVecZero(&offset);
        rotation = NuMtxSetIdentityTemplate;
        NuMtxRotateX(&rotation, camera->pitch);
        NuMtxRotateY(&rotation, camera->yaw);
        offset.z = -camera->distance;
        NuVecMtxRotate(&offset, &offset, &rotation);
        NuVecAdd(&old_opposite, &camera->position, &offset);

        yaw_delta = (i32)((f64)NuPs2ApplyDeadZone(pad->analog_right_x, 32) * camera->stick_yaw_speed * delta_time * 64.0);
        pitch_delta = (i32)((f64)NuPs2ApplyDeadZone(pad->analog_right_y, 32) * camera->stick_pitch_speed * delta_time * 32.0);
        if (camera->freedoms & EDCAM_FREEDOM_INVERT_PAD_PITCH) {
            pitch_delta = -pitch_delta;
        }
        if (camera->freedoms & EDCAM_FREEDOM_PITCH) {
            camera->pitch += pitch_delta;
            camera->pitch = MIN(0x4000, camera->pitch);
            camera->pitch = MAX(-0x4000, camera->pitch);
        }
        if (camera->freedoms & EDCAM_FREEDOM_YAW) {
            camera->yaw += yaw_delta;
        }

        rotation = NuMtxSetIdentityTemplate;
        NuMtxRotateX(&rotation, camera->pitch);
        NuMtxRotateY(&rotation, camera->yaw);
        offset.y = 0.0f;
        offset.x = 0.0f;
        offset.z = -camera->distance;
        NuVecMtxRotate(&offset, &offset, &rotation);
        NuVecAdd(&new_opposite, &camera->position, &offset);
        NuVecAdd(&camera->position, &camera->position, &new_opposite);
        NuVecSub(&camera->position, &camera->position, &old_opposite);

        EdcamVecZero(&offset);
        offset.x -= (f32)NuPs2ApplyDeadZone(pad->analog_left_x, 32) * delta_time * camera->distance * (f64)0.01f;
        offset.z += (f32)NuPs2ApplyDeadZone(pad->analog_left_y, 32) * delta_time * camera->distance * (f64)0.01f;
        offset.y += (f32)pad->analog_l1 * camera->position_speed.y * move_speed * frame_scale * 0.5;
        offset.y -= (f32)pad->analog_l2 * camera->position_speed.y * move_speed * frame_scale * 0.5;
        if (camera->allow_distance) {
            camera->distance += (f32)pad->analog_r1 * camera->distance_speed * zoom_speed * frame_scale;
            camera->distance -= (f32)pad->analog_r2 * camera->distance_speed * zoom_speed * frame_scale;
            camera->distance = MIN(-camera->minimum_distance, camera->distance);
        }
        NuVecMtxRotate(&offset, &offset, &rotation);
        if (camera->freedoms & EDCAM_FREEDOM_POSITION_X) {
            camera->position.x += offset.x;
        }
        if (camera->freedoms & EDCAM_FREEDOM_POSITION_Y) {
            camera->position.y += offset.y;
        }
        if (camera->freedoms & EDCAM_FREEDOM_POSITION_Z) {
            camera->position.z += offset.z;
        }
    }

    void do_Pad_Standard_camera(edcam_s *camera, f32 delta_time, FRAME_PAD *pad) {
        NUVEC movement;
        f32 frame_scale;
        f32 move_speed;
        f32 zoom_speed;
        i32 pad_yaw_axis;
        i32 pad_pitch_axis;
        NUVEC snap_delta;

        EdcamVecZero(&movement);
        if (!pad->is_valid) {
            return;
        }

        frame_scale = (f32)(delta_time / (f64)(1.0f / 60.0f));
        if (camera->auto_move_dist_scale != 0.0f) {
            move_speed = camera->auto_move_dist_scale * EdcamFabs(camera->distance) + camera->auto_move_base;
        } else {
            move_speed = 1.0f;
        }
        if (camera->auto_zoom_dist_scale != 0.0f) {
            zoom_speed = camera->auto_zoom_dist_scale * EdcamFabs(camera->distance) + camera->auto_zoom_base;
        } else {
            zoom_speed = 1.0f;
        }
        pad_yaw_axis = pad->analog_right_x;
        pad_pitch_axis = pad->analog_right_y;
        if (camera->freedoms & EDCAM_FREEDOM_DISTANCE) {
            f32 zoom = (f32)pad->analog_r1 * camera->distance_speed * zoom_speed;
            if (zoom > 1.0f) {
                zoom = 1.0f;
            }
            camera->distance += zoom * frame_scale;
            zoom = (f32)pad->analog_r2 * camera->distance_speed * zoom_speed;
            if (zoom > 1.0f) {
                zoom = 1.0f;
            }
            camera->distance -= zoom * frame_scale;
            camera->distance = MIN(-camera->minimum_distance, camera->distance);
        }
        if (camera->freedoms & EDCAM_FREEDOM_PITCH) {
            camera->pitch += (i32)((f32)(NuPs2ApplyDeadZone(pad_pitch_axis, 32) * camera->stick_pitch_speed) * delta_time * -64.0);
            camera->pitch = MIN(0x4000, camera->pitch);
            camera->pitch = MAX(-0x4000, camera->pitch);
        }
        if (camera->freedoms & EDCAM_FREEDOM_YAW) {
            camera->yaw += (i32)((f32)(NuPs2ApplyDeadZone(pad_yaw_axis, 32) * camera->stick_yaw_speed) * delta_time * -64.0);
        }
        if (camera->freedoms & EDCAM_FREEDOM_POSITION_Y) {
            movement.y += (f32)pad->analog_l1 * camera->position_speed.y * move_speed * frame_scale * 0.5;
            movement.y -= (f32)pad->analog_l2 * camera->position_speed.y * move_speed * frame_scale * 0.5;
        }
        if (camera->freedoms & EDCAM_FREEDOM_POSITION_Z) {
            movement.z = -(f32)NuPs2ApplyDeadZone(pad->analog_left_y, 32) * camera->position_speed.y * move_speed * frame_scale;
        }
        if (camera->freedoms & EDCAM_FREEDOM_POSITION_X) {
            movement.x = (f32)NuPs2ApplyDeadZone(pad->analog_left_x, 32) * camera->position_speed.y * move_speed * frame_scale;
        }
        NuVecRotateY(&movement, &movement, camera->yaw);
        NuVecAdd(&camera->position, &camera->position, &movement);
        NuVecSub(&snap_delta, &camera->position, &camera->snap_origin);
        snap_delta.x /= camera->snap_step.x;
        snap_delta.y /= camera->snap_step.y;
        snap_delta.z /= camera->snap_step.z;
        camera->snapped_position.x = NuFloor(snap_delta.x) * camera->snap_step.x + camera->snap_origin.x;
        camera->snapped_position.y = NuFloor(snap_delta.y) * camera->snap_step.y + camera->snap_origin.y;
        camera->snapped_position.z = NuFloor(snap_delta.z) * camera->snap_step.z + camera->snap_origin.z;
    }

    void do_maya_mouse_camera(edcam_s *camera) {
        /* The historical mapped name TargetAng denotes the smoothed PC slot. */
        static EdcamMayaAngleObserved TargetAng = {0};
        static EdcamMayaAngleObserved RawTargetAng = {0};
        f32 zoom_scale = camera->auto_zoom_dist_scale != 0.0f
                             ? EdcamFabs(camera->distance) * camera->auto_zoom_dist_scale + camera->auto_zoom_base
                             : 1.0f;
        NUVEC movement;
        f32 mouse_x;
        f32 mouse_y;
        f32 mouse_z;
        u32 buttons;
        f32 minimum;
        NUMTX rotation;

        EdcamVecZero(&movement);
        if (NuKeyboard(0x38) == 0) {
            return;
        }
        mouse_x = NuMouseReadXVel() * camera->mouse_pitch_speed;
        mouse_y = NuMouseReadYVel() * camera->mouse_yaw_speed;
        mouse_z = NuMouseReadZVel() * camera->mouse_move_speed;
        buttons = NuMouseReadButtons();
        if (buttons == 2 || buttons == 5) {
            if (camera->freedoms & EDCAM_FREEDOM_DISTANCE) {
                i32 difference;
                f32 sine;
                if (mouse_x != 0.0f || mouse_y != 0.0f) {
                    RawTargetAng.parts.angle = (u16)NuAtan2D(-mouse_x, -mouse_y);
                }
                difference = EdtoolsallVuAngleDifference(RawTargetAng.word, TargetAng.word);
                if (abs(difference) > 20000) {
                    TargetAng.parts.angle = RawTargetAng.parts.angle;
                } else {
                    TargetAng.parts.angle += difference / 4;
                }
                sine = NuTrigTable[((i32)(TargetAng.word + 0x2000) >> 1) & 0x7fff];
                if (EdcamFabs(sine) > (f64)0.1f) {
                    f32 length_squared;
                    f32 zoom_delta;
                    if (sine > 0.0f) {
                        sine = (f32)(sine - (f64)0.1f);
                    } else {
                        sine = (f32)(sine + (f64)0.1f);
                    }
                    length_squared = mouse_x * mouse_x + mouse_y * mouse_y;
                    zoom_delta = NuFsqrt(length_squared) * sine;
                    camera->distance = camera->distance + camera->distance_speed * zoom_scale * zoom_delta * camera->mouse_move_speed;
                    minimum = -camera->minimum_distance;
                    camera->distance = MIN(minimum, camera->distance);
                }
            }
        } else if (buttons == 1) {
            if (camera->freedoms & EDCAM_FREEDOM_PITCH) {
                camera->pitch += (i32)(mouse_y * -16.0);
                camera->pitch = MIN(0x4000, camera->pitch);
                camera->pitch = MAX(-0x4000, camera->pitch);
            }
            if (camera->freedoms & EDCAM_FREEDOM_YAW) {
                camera->yaw += (i32)(mouse_x * -16.0);
            }
        } else if (buttons == 4 || buttons == 3) {
            movement.x = (f32)(movement.x + mouse_x * 0.0078125);
            movement.y = (f32)(movement.y - mouse_y * 0.0078125);
        }

        camera->distance = camera->distance - camera->distance_speed * zoom_scale * mouse_z;
        minimum = -camera->minimum_distance;
        camera->distance = MIN(minimum, camera->distance);
        rotation = NuMtxSetIdentityTemplate;
        NuMtxRotateX(&rotation, camera->pitch);
        NuMtxRotateY(&rotation, camera->yaw);
        NuVecMtxRotate(&movement, &movement, &rotation);
        if (camera->freedoms & EDCAM_FREEDOM_POSITION_X) {
            camera->position.x += movement.x;
        }
        if (camera->freedoms & EDCAM_FREEDOM_POSITION_Y) {
            camera->position.y += movement.y;
        }
        if (camera->freedoms & EDCAM_FREEDOM_POSITION_Z) {
            camera->position.z += movement.z;
        }
    }

    void do_mouse_flymode_camera(edcam_s *camera, f32 delta_time) {
        f32 zoom_scale;
        f32 mouse_z;
        f32 mouse_x;
        f32 mouse_y;
        u32 buttons;
        NUVEC offset;
        NUVEC old_opposite;
        NUVEC new_opposite;
        NUMTX rotation;
        i32 yaw_delta;

        if (camera->auto_zoom_dist_scale != 0.0f) {
            zoom_scale = EdcamFabs(camera->distance) * camera->auto_zoom_dist_scale + camera->auto_zoom_base;
        } else {
            zoom_scale = 1.0f;
        }
        mouse_y = NuMouseReadYRel();
        mouse_x = NuMouseReadXRel();
        mouse_z = NuMouseReadZRel();
        buttons = NuMouseReadButtons();
        if (camera->freedoms & EDCAM_FREEDOM_DISTANCE) {
            camera->distance -= mouse_z * camera->distance_speed * zoom_scale;
            camera->distance = MIN(-camera->minimum_distance, camera->distance);
        }

        EdcamVecZero(&offset);
        rotation = NuMtxSetIdentityTemplate;
        NuMtxRotateX(&rotation, camera->pitch);
        NuMtxRotateY(&rotation, camera->yaw);
        offset.z = -camera->distance;
        NuVecMtxRotate(&offset, &offset, &rotation);
        NuVecAdd(&old_opposite, &camera->position, &offset);

        yaw_delta = (i32)(mouse_x * 16.0f);
        if (camera->freedoms & EDCAM_FREEDOM_PITCH) {
            camera->pitch += (i32)(mouse_y * 16.0f);
        }
        camera->pitch = MIN(0x4000, camera->pitch);
        camera->pitch = MAX(-0x4000, camera->pitch);
        if (camera->freedoms & EDCAM_FREEDOM_YAW) {
            camera->yaw += yaw_delta;
        }

        rotation = NuMtxSetIdentityTemplate;
        NuMtxRotateX(&rotation, camera->pitch);
        NuMtxRotateY(&rotation, camera->yaw);
        offset.y = 0.0f;
        offset.x = 0.0f;
        offset.z = -camera->distance;
        NuVecMtxRotate(&offset, &offset, &rotation);
        NuVecAdd(&new_opposite, &camera->position, &offset);
        NuVecAdd(&camera->position, &camera->position, &new_opposite);
        NuVecSub(&camera->position, &camera->position, &old_opposite);

        EdcamVecZero(&offset);
        if (buttons == 1) {
            offset.z = -delta_time * camera->distance;
        } else if (buttons == 2) {
            offset.z = delta_time * camera->distance;
        }
        NuVecMtxRotate(&offset, &offset, &rotation);
        if (camera->allow_position_x) {
            camera->position.x += offset.x;
        }
        if (camera->allow_position_y) {
            camera->position.y += offset.y;
        }
        if (camera->allow_position_z) {
            camera->position.z += offset.z;
        }
    }

    i32 PadFlyMode = 0;

    void edcamMoveEx(FRAME_PAD *pad, f32 delta_time) {
        if (edmain_cursor_enabled != 0) {
            if (PadFlyMode == 0 || NuKeyboard(0x38) != 0) {
                do_maya_mouse_camera(gp_cam);
            } else {
                do_mouse_flymode_camera(gp_cam, delta_time);
            }
        }
        if (pad != NULL) {
            if (PadFlyMode == 0) {
                do_Pad_Standard_camera(gp_cam, delta_time, pad);
            } else {
                do_Pad_flymode_camera(gp_cam, delta_time, pad);
            }
        }
    }

    void edcamMove(FRAME_PAD *pad) {
        edcamMoveEx(pad, NuTimeGetFrameTime());
    }

    void edcamSetPos(NUVEC *position) {
        edcam_s *camera = gp_cam;
        camera->position = *position;
        EdcamVecZero(&camera->offset);
    }

    void edcamSetAng(i32 pitch, i32 yaw) {
        edcam_s *camera = gp_cam;
        camera->pitch = pitch;
        camera->yaw = yaw;
    }

    void edcamSetPosAng(NUVEC *position, i32 pitch, i32 yaw) {
        edcam_s *camera = gp_cam;
        camera->position = *position;
        camera->pitch = pitch;
        camera->yaw = yaw;
        EdcamVecZero(&camera->offset);
    }

    void edcamGetPosAng(NUVEC *position, i32 *pitch, i32 *yaw) {
        edcam_s *camera = gp_cam;
        if (position != NULL) {
            *position = camera->position;
        }
        if (pitch != NULL) {
            *pitch = camera->pitch;
        }
        if (yaw != NULL) {
            *yaw = camera->yaw;
        }
    }

    void edcamGetPosAngSnap(NUVEC *position, i32 *pitch, i32 *yaw) {
        edcam_s *camera = gp_cam;
        if (position != NULL) {
            *position = camera->snapped_position;
        }
        if (pitch != NULL) {
            *pitch = camera->snapped_pitch;
        }
        if (yaw != NULL) {
            *yaw = camera->snapped_yaw;
        }
    }

    void edcamSetContoller(i32 invert_pitch) {
        gp_cam->invert_pad_pitch = invert_pitch;
    }

    void edcamSetDist(f32 distance) {
        gp_cam->distance = distance;
    }

    f32 edcamGetDist(void) {
        return gp_cam->distance;
    }

    edcam_s *edcamGetEdCam(void) {
        return gp_cam;
    }

    void edcamSetOffset(NUVEC *offset) {
        gp_cam->offset = *offset;
    }

    void edcamGetOffset(NUVEC *offset) {
        edcam_s *camera = gp_cam;
        *offset = camera->offset;
    }
    struct numtx_s *ed_remap_mtx = 0;

    void edcamMtx(NUMTX *matrix) {
        NUVEC distance = {0.0f, 0.0f, gp_cam->distance};
        NuMtxSetTranslation(matrix, &distance);
        NuMtxRotateX(matrix, gp_cam->pitch);
        NuMtxRotateY(matrix, gp_cam->yaw);
        NuMtxTranslate(matrix, &gp_cam->position);
        NuMtxTranslate(matrix, &gp_cam->offset);
        if (ed_remap_mtx != NULL) {
            NuMtxMul(matrix, matrix, ed_remap_mtx);
            ed_remap_mtx = NULL;
        }
    }

    void edcamSet(void) {
        NUMTX matrix;
        NUCAMERA *camera;
        edcamMtx(&matrix);
        camera = EdMainCurrentCamera;
        camera->mtx = matrix;
        NuCameraSet(camera);
    }

    NUVEC *edcamGetPosPointer(void) {
        return &gp_cam->position;
    }

    void EdFileSwapEndianess16(void *value) {
        u8 *bytes = (u8 *)value;
        u8 temp = bytes[0];
        bytes[0] = bytes[1];
        bytes[1] = temp;
    }

    void EdFileSwapEndianess32(void *value) {
        u8 *bytes = (u8 *)value;
        u8 temp = bytes[0];
        bytes[0] = bytes[3];
        bytes[3] = temp;
        temp = bytes[1];
        bytes[1] = bytes[2];
        bytes[2] = temp;
    }

    void EdFileSetReadWrongEndianess(i32 value) {
    }

    static i32 edfile_media = 0;

    void EdFileSetMedia(i32 media) {
        edfile_media = media;
    }

    static void *edfile_pakfile = 0;

    void EdFileSetPakFile(void *pak) {
        edfile_pakfile = pak;
    }

    char edfile_buffer[0x1000] = {0};
    i32 edfile_buffer_pointer = 0;
    i32 edfile_lock[2] = {0};

    void EdFileResetBuffers(void) {
        memset(edfile_buffer, 0, sizeof(edfile_buffer));
        edfile_buffer_pointer = 0;
        edfile_lock[1] = 0;
        edfile_lock[0] = 0;
    }

    i32 edfile_handle = -1;

    void EdFileFillBuffer(void) {
        if (edfile_handle != -1) {
            memset(edfile_buffer, 0, sizeof(edfile_buffer));
            switch (edfile_media) {
                case 0:
                    return;
                case 1:
                    NuFileRead(edfile_handle, edfile_buffer, sizeof(edfile_buffer));
                    edfile_buffer_pointer = 0;
                    break;
            }
        }
    }

    i32 edfile_write_flag = 0;

    i32 EdFileOpen(char *filepath, i32 mode) {
        i32 item_handle;
        void *item;
        i32 item_size;

        if (edfile_handle != -1) {
            return 0;
        }
        EdFileResetBuffers();
        edfile_handle = -1;
        edfile_write_flag = 0;
        switch (edfile_media) {
            case 0:
                return 0;
            case 1:
                switch (mode) {
                    case 0:
                        if (edfile_pakfile != NULL) {
                            item_handle = NuFilePakGetItem(edfile_pakfile, filepath);
                            if (item_handle != 0) {
                                NuFilePakGetItemInfo(edfile_pakfile, item_handle, &item, &item_size);
                                edfile_handle = NuMemFileOpen(item, item_size, 0);
                                if (edfile_handle > 0) {
                                    EdFileFillBuffer();
                                    return 1;
                                }
                                edfile_handle = -1;
                            }
                        }
                        edfile_handle = NuFileOpen(filepath, 0);
                        if (edfile_handle > 0) {
                            EdFileFillBuffer();
                            return 1;
                        }
                        edfile_handle = -1;
                        return 0;
                    case 1:
                        edfile_write_flag = 1;
                        edfile_handle = NuFileOpen(filepath, 1);
                        if (edfile_handle > 0) {
                            return 1;
                        }
                        edfile_handle = -1;
                        break;
                }
                break;
        }
        return 0;
    }

    void EdFileFlushBuffer(void) {
        if (edfile_handle != -1 && edfile_buffer_pointer != 0) {
            switch (edfile_media) {
                case 0:
                    return;
                case 1:
                    NuFileWrite(edfile_handle, edfile_buffer, edfile_buffer_pointer);
                    edfile_buffer_pointer = 0;
                    break;
            }
        }
    }

    i32 EdFileClose(void) {
        edfile_pakfile = NULL;
        if (edfile_handle == -1) {
            return 0;
        }
        if (edfile_write_flag != 0) {
            EdFileFlushBuffer();
        }
        switch (edfile_media) {
            case 0:
                return 0;
            case 1:
                NuFileClose(edfile_handle);
                edfile_handle = -1;
                edfile_media = 0;
                return 1;
        }
        return 0;
    }

    void EdFileWrite(void *data, i32 len) {
        i32 to_write;

        while (len > 0) {
            to_write = MIN(len, 0x1000 - edfile_buffer_pointer);

            memcpy(edfile_buffer + edfile_buffer_pointer, data, to_write);

            edfile_buffer_pointer += to_write;
            len -= to_write;
            data = (u8 *)data + to_write;

            if (edfile_buffer_pointer == 0x1000) {
                EdFileFlushBuffer();
            }
        }
    }

    i32 edfile_readwrongendianess = 0;

    void EdFileWriteFloat(f32 value) {
        f32 data = value;
        if (edfile_readwrongendianess) {
            EdFileSwapEndianess32(&data);
        }
        EdFileWrite(&data, 4);
    }

    void EdFileWriteInt(i32 value) {
        i32 data = value;
        if (edfile_readwrongendianess) {
            EdFileSwapEndianess32(&data);
        }
        EdFileWrite(&data, 4);
    }

    void EdFileWriteUnsignedInt(u32 value) {
        u32 data = value;
        if (edfile_readwrongendianess) {
            EdFileSwapEndianess32(&data);
        }
        EdFileWrite(&data, 4);
    }

    void EdFileWriteShort(i16 value) {
        i16 data = value;
        if (edfile_readwrongendianess) {
            EdFileSwapEndianess16(&data);
        }
        EdFileWrite(&data, 2);
    }

    void EdFileWriteUnsignedShort(u16 value) {
        u16 data = value;
        if (edfile_readwrongendianess) {
            EdFileSwapEndianess16(&data);
        }
        EdFileWrite(&data, 2);
    }

    void EdFileWriteChar(char value) {
        EdFileWrite(&value, 1);
    }

    void EdFileWriteUnsignedChar(u8 value) {
        EdFileWrite(&value, 1);
    }

    void EdFileWriteNuVec(NUVEC *value) {
        EdFileWriteFloat(value->x);
        EdFileWriteFloat(value->y);
        EdFileWriteFloat(value->z);
    }

    void EdFileRead(void *buffer, i32 length) {
        i32 count;
        while (length > 0) {
            count = sizeof(edfile_buffer) - edfile_buffer_pointer;
            if (length < count) {
                count = length;
            }
            memcpy(buffer, edfile_buffer + edfile_buffer_pointer, count);
            edfile_buffer_pointer += count;
            length -= count;
            buffer = (u8 *)buffer + count;
            if (edfile_buffer_pointer == sizeof(edfile_buffer)) {
                EdFileFillBuffer();
            }
        }
    }

    f32 EdFileReadFloat(void) {
        union {
            f32 value;

            struct {
                u8 first;
                u8 second;
                u8 third;
                u8 fourth;
            } bytes;
        } data;

        u8 temporary;
        EdFileRead(&data.value, sizeof(data.value));
        if (edfile_readwrongendianess != 0) {
            temporary = data.bytes.first;
            data.bytes.first = data.bytes.fourth;
            data.bytes.fourth = temporary;
            temporary = data.bytes.second;
            data.bytes.second = data.bytes.third;
            data.bytes.third = temporary;
        }
        return data.value;
    }

    i32 EdFileReadInt(void) {
        union {
            i32 value;

            struct {
                u8 first;
                u8 second;
                u8 third;
                u8 fourth;
            } bytes;
        } data;

        u8 temporary;
        EdFileRead(&data.value, sizeof(data.value));
        if (edfile_readwrongendianess != 0) {
            temporary = data.bytes.first;
            data.bytes.first = data.bytes.fourth;
            data.bytes.fourth = temporary;
            temporary = data.bytes.second;
            data.bytes.second = data.bytes.third;
            data.bytes.third = temporary;
        }
        return data.value;
    }

    u32 EdFileReadUnsignedInt(void) {
        union {
            u32 value;

            struct {
                u8 first;
                u8 second;
                u8 third;
                u8 fourth;
            } bytes;
        } data;

        u8 temporary;
        EdFileRead(&data.value, sizeof(data.value));
        if (edfile_readwrongendianess != 0) {
            temporary = data.bytes.first;
            data.bytes.first = data.bytes.fourth;
            data.bytes.fourth = temporary;
            temporary = data.bytes.second;
            data.bytes.second = data.bytes.third;
            data.bytes.third = temporary;
        }
        return data.value;
    }

    i16 EdFileReadShort(void) {
        union {
            i16 value;

            struct {
                u8 first;
                u8 second;
            } bytes;
        } data;

        u8 temporary;
        EdFileRead(&data.value, sizeof(data.value));
        if (edfile_readwrongendianess != 0) {
            temporary = data.bytes.first;
            data.bytes.first = data.bytes.second;
            data.bytes.second = temporary;
        }
        return data.value;
    }

    u16 EdFileReadUnsignedShort(void) {
        union {
            u16 value;

            struct {
                u8 first;
                u8 second;
            } bytes;
        } data;

        u8 temporary;
        EdFileRead(&data.value, sizeof(data.value));
        if (edfile_readwrongendianess != 0) {
            temporary = data.bytes.first;
            data.bytes.first = data.bytes.second;
            data.bytes.second = temporary;
        }
        return data.value;
    }

    char EdFileReadChar(void) {
        char character;
        EdFileRead(&character, sizeof(character));
        return character;
    }

    u8 EdFileReadUnsignedChar(void) {
        u8 character;
        EdFileRead(&character, sizeof(character));
        return character;
    }

    void EdFileReadNuVec(NUVEC *out) {
        out->x = EdFileReadFloat();
        out->y = EdFileReadFloat();
        out->z = EdFileReadFloat();
    }

    i32 EdFileWriteMemCard(char *filename, i32 size, void *data) {
        EdFileSetMedia(2);
        if (EdFileOpen(filename, 1)) {
            EdFileWrite(data, size);
            return EdFileClose();
        }
        return 0;
    }

    i32 EdFileReadMemCard(char *filename, i32 size, void *data) {
        EdFileSetMedia(2);
        if (!EdFileOpen(filename, 0)) {
            return 0;
        }
        EdFileRead(data, size);
        return EdFileClose();
    }

    i32 EdFileBackup(char *source, char *destination) {
        i64 remaining = NuFileSize(source);
        if (remaining <= 0) {
            return 0;
        }
        {
            NUFILE output = NuFileOpen(destination, NUFILE_WRITE);
            NUFILE input = NuFileOpen(source, NUFILE_READ);
            if (input > 0 && output > 0) {
                while (remaining > 0) {
                    NuFileRead(input, edfile_buffer, MIN((i32)remaining, 0x1000));
                    NuFileWrite(output, edfile_buffer, MIN((i32)remaining, 0x1000));
                    remaining -= 0x1000;
                }
                NuFileClose(input);
                NuFileClose(output);
                return 1;
            }
            if (input > 0) {
                NuFileClose(input);
            }
            if (output > 0) {
                NuFileClose(output);
            }
        }
        return 0;
    }

    void FS_MakeDateTimeString(const FS_PACKED_ENTRY_HEADER *entry, char *output) {
        NuSPrintf(output, " %.2d/%.2d/%d %.2d:%.2d:%.2d", entry->encoded_day - 'A', entry->encoded_month - 'A', entry->encoded_year + 1915,
                  entry->encoded_hour - 'A', entry->encoded_minute - 'A', entry->encoded_second - 'A');
    }

    void FS_MakeTimeString(const FS_PACKED_ENTRY_HEADER *entry, char *output) {
        NuSPrintf(output, " %.2d:%.2d:%.2d", entry->encoded_hour - 'A', entry->encoded_minute - 'A', entry->encoded_second - 'A');
    }

    void FS_MakeDateString(const FS_PACKED_ENTRY_HEADER *entry, char *output) {
        NuSPrintf(output, " %.2d/%.2d/%d", entry->encoded_day - 'A', entry->encoded_month - 'A', entry->encoded_year + 1915);
    }

    char *FS_CurrentPos = FS_FileList;
    char *FS_CurrentCursorPos = FS_FileList;
    i32 FS_CursorLine = 0;
    i32 FS_CurrentPosFileNum = 0;
    char *FS_FileListEnd = FS_FileList;

    void FS_MoveCursorDown(i32 steps) {
        while (steps > 0) {
            i32 top_length = NuStrLen(FS_CurrentPos) + 1;
            i32 cursor_length = NuStrLen(FS_CurrentCursorPos) + 1;
            char *next = FS_CurrentCursorPos + cursor_length;
            if (next < FS_FileListEnd) {
                if (FS_CursorLine == 13) {
                    char *next_top = FS_CurrentPos + top_length;
                    if (next_top < FS_FileListEnd) {
                        ++FS_CurrentPosFileNum;
                        FS_CurrentPos = next_top;
                    }
                } else {
                    ++FS_CursorLine;
                }
                FS_CurrentCursorPos = next;
            }
            --steps;
        }
    }

    char FS_FileList[65536] = {0};

    i32 FS_PrevNameLen(char *name) {
        char *previous;
        i32 length = 0;
        if (name == FS_FileList) {
            return 0;
        }
        previous = name - 2;
        while (*previous != '\0') {
            if (previous == FS_FileList) {
                return length + 2;
            }
            --previous;
            ++length;
        }
        return length + 1;
    }

    void FS_MoveCursorUp(i32 steps) {
        while (steps > 0) {
            i32 top_length = FS_PrevNameLen(FS_CurrentPos);
            i32 cursor_length = FS_PrevNameLen(FS_CurrentCursorPos);
            if (FS_CurrentCursorPos == FS_CurrentPos) {
                FS_CurrentPos -= top_length;
                if (FS_CurrentPosFileNum != 0) {
                    --FS_CurrentPosFileNum;
                }
            } else {
                --FS_CursorLine;
            }
            FS_CurrentCursorPos -= cursor_length;
            --steps;
        }
    }

    u8 FS_ShowVolumes = 0;
    u8 FS_Active = 0;
    char FS_Path[256] = {0};
    char FS_Filter[256] = "*.nup | *.hgp   ";
    f32 FS_Width = 248.0f;
    i32 FS_NumFiles = 0;
    /* PC file-selector rendering, each string in its own section. */
    char FS_Title[64] = "Title";
    f32 FS_X = 50.0f;
    f32 FS_Y = 40.0f;
    f32 FS_W = 0.0f;
    f32 FS_H = 0.0f;
    char *SortStrings[4] = {"A to Z", "Z to A", "Old to New", "New To Old"};
    i32 FS_SortMode = 3;

    void RenderFileSel3(i32 draw_title) {
        char buffer[256];
        char date_time[64];
        char *text;
        char *entry;
        f32 x;
        f32 y;
        f32 width;
        f32 height;
        f32 baseline;
        f32 full_width;
        f32 box_height;
        f32 box_width;
        f32 text_x;
        f32 line_height;
        f32 path_y;
        f32 list_box_height;
        f32 line_y;
        f32 footer_base;
        f32 footer_y;
        f32 sort_length;
        f64 list_height;
        i32 line;
        if (FS_Active == 0) {
            return;
        }
        x = FS_X;
        y = FS_Y;
        width = FS_Width;
        NuQFntPushPrintMode(NUQFNT_MODE_2D);
        NuQFntPushCoordinateSystem(NUQFNT_CS_SIXTEENTHS);
        NuQFntSet(system_qfont);
        NuQFntSetPointSize(system_qfont, 1.0f, 1.0f);
        height = NuQFntHeight(system_qfont);
        baseline = NuQFntBaseline(system_qfont);
        full_width = width;
        if (FS_NumFiles > 14) {
            full_width = width + 8.0;
        }

        if (draw_title) {
            box_height = height * 17.0 * 0.0625 + 6.0;
        } else {
            box_height = height * 16.0 * 0.0625 + 4.0;
        }
        box_width = full_width + 4.0;
        NuRndrRect2d(x, y, 0.0f, box_width, box_height, 0xff808080, NULL);
        x += 2.0;
        y += 1.0;
        FS_W = box_width;
        FS_H = box_height;
        text_x = (x + 4.0) * 16.0;
        if (draw_title) {
            f32 title_height = height * 0.0625;
            f32 title_y;
            NuRndrRect2d(x, y, 0.0f, full_width, title_height, 0xff404040, NULL);
            NuRndrLine3dDbg(x, y, 0.0f, width + x, y, 0.0f, 0xff808080);
            NuQFntSet(system_qfont);
            NuQFntSetColour(system_qfont, 0x80808080);
            title_y = y * 16.0 + baseline;
            NuQFntMove(system_qfont, text_x, title_y, 0.0f);
            NuQFntPrintU(system_qfont, FS_Title);
            y += title_height + 2.0;
        }

        line_height = height * 0.0625;
        path_y = y * 16.0 + baseline;
        NuRndrRect2d(x, y, 0.0f, full_width, line_height, 0x20202020, NULL);
        if (FS_ShowVolumes) {
            NuStrCpy(buffer, "Volumes:-");
            text = buffer;
        } else {
            f32 available = full_width - 8.0;
            f32 length;
            text = FS_Path;
            length = NuQFntPrintLenU(system_qfont, text) * 0.0625;
            if (length > available) {
                f32 scale = available / length;
                if (scale < 0.5f) {
                    // Too long even at half width: keep the drive and first
                    // directory, then "\....", then the last directory.
                    char *first = NuStrChr(text, '\\');
                    if (first != NULL) {
                        char *last;
                        i32 tail_length;
                        char *second = NuStrChr(first + 1, '\\');
                        if (second == NULL) {
                            second = first;
                        }
                        *second = '\0';
                        NuStrCpy(buffer, text);
                        *second = '\\';
                        NuStrCat(buffer, "\\....");
                        tail_length = NuStrLen(second);
                        if (tail_length != 0 && second[tail_length - 1] == '\\') {
                            second[tail_length - 1] = '\0';
                            last = NuStrRChr(second, '\\');
                            second[tail_length - 1] = '\\';
                        } else {
                            last = NuStrRChr(second, '\\');
                        }
                        if ((u32)last > (u32)second) {
                            NuStrCat(buffer, last);
                            text = buffer;
                        }
                    }
                    length = NuQFntPrintLenU(system_qfont, text) * 0.0625;
                    if (!(length > available)) {
                        goto path_done;
                    }
                }
                NuQFntSetPointSize(system_qfont, available / length, 1.0f);
            }
        path_done:;
        }
        NuQFntSet(system_qfont);
        NuQFntMove(system_qfont, text_x, path_y, 0.0f);
        NuQFntSetColour(system_qfont, 0x80408080);
        NuQFntPrintU(system_qfont, text);
        NuQFntSetPointSize(system_qfont, 1.0f, 1.0f);

        y += line_height + 2.0;
        list_height = height * 14.0;
        list_box_height = list_height * 0.0625;
        NuRndrRect2d(x, y, 0.0f, width, list_box_height, 0x20202020, NULL);
        if (FS_NumFiles > 14) {
            f32 thumb;
            f32 thumb_offset;
            NuRndrRect2d(width + x, y, 0.0f, 8.0f, list_box_height, 0xff646464, NULL);
            thumb = list_box_height / FS_NumFiles * 14.0;
            if (thumb < 5.0f) {
                thumb = 5.0f;
            }
            thumb_offset = (list_box_height - thumb) * FS_CurrentPosFileNum / (FS_NumFiles - 14);
            NuRndrRect2d(width + x, thumb_offset + y, 0.0f, 8.0f, thumb, 0xffc8c8c8, NULL);
        }

        line_y = path_y + 32.0;
        entry = FS_CurrentPos;
        for (line = 0; line < 14; ++line) {
            FS_PACKED_ENTRY_HEADER *header;
            char *name;
            if (entry >= FS_FileListEnd) {
                break;
            }
            line_y += height;
            if (entry == FS_CurrentCursorPos) {
                NuRndrRect2d(x, (line_y - baseline) * 0.0625, 0.0f, width, line_height, 0xff202060, NULL);
            }
            NuQFntSet(system_qfont);
            NuQFntMove(system_qfont, text_x, line_y, 0.0f);
            header = (FS_PACKED_ENTRY_HEADER *)entry;
            if (header->kind == 'V') {
                NuQFntSetColour(system_qfont, 0x80204080);
            } else if (header->kind == 'D') {
                NuQFntSetColour(system_qfont, 0x80804020);
            } else {
                NuQFntSetColour(system_qfont, 0x80408080);
            }
            NuQFntSetPointSize(system_qfont, 1.0f, 1.0f);
            name = (char *)(header + 1);
            NuQFntPrintU(system_qfont, name);
            if (header->encoded_year != '#') {
                f32 length;
                f64 time_x;
                NuQFntSetPointSize(system_qfont, 0.8f, 1.0f);
                NuSPrintf(date_time, " %.2d:%.2d:%.2d", header->encoded_hour - 'A', header->encoded_minute - 'A',
                          header->encoded_second - 'A');
                length = NuQFntPrintLenU(system_qfont, date_time);
                NuQFntSetColour(system_qfont, 0x80208080);
                time_x = (width + x) * 16.0 - length;
                NuQFntMove(system_qfont, time_x, line_y, 0.0f);
                NuQFntPrintU(system_qfont, date_time);
                NuSPrintf(date_time, " %.2d/%.2d/%d", header->encoded_day - 'A', header->encoded_month - 'A', header->encoded_year + 1915);
                length = NuQFntPrintLenU(system_qfont, date_time);
                NuQFntSetColour(system_qfont, 0x80808020);
                NuQFntMove(system_qfont, time_x - length, line_y, 0.0f);
                NuQFntPrintU(system_qfont, date_time);
            }
            entry = name + NuStrLen(name) + 1;
        }

        footer_base = y * 16.0 + baseline;
        footer_y = footer_base + list_height;
        NuQFntSet(system_qfont);
        NuQFntSetPointSize(system_qfont, 1.0f, 1.0f);
        NuQFntSetColour(system_qfont, 0x80808080);
        NuQFntMove(system_qfont, x * 16.0, footer_y, 0.0f);
        NuQFntPrintU(system_qfont, FS_Filter);
        NuQFntSetColour(system_qfont, 0x80000040);
        sort_length = NuQFntPrintLenU(system_qfont, SortStrings[FS_SortMode]);
        NuQFntMove(system_qfont, (full_width + x) * 16.0 - sort_length, footer_y, 0.0f);
        NuQFntPrintU(system_qfont, SortStrings[FS_SortMode]);
        NuQFntPopCoordinateSystem();
        NuQFntPopPrintMode();
    }

    void RenderFileSel2(i32 x, i32 y, i32 *width, i32 *height) {
        FS_X = x;
        FS_Y = (f32)(y * 0.5);
        RenderFileSel3(0);
        *height = (FS_H + FS_H);
        *width = FS_W;
    }

    void RenderFileSel(void) {
        RenderFileSel3(1);
    }

    f32 FS_GetDirTextWidth(void) {
        char date_time[64];
        f32 max_width = 0.0f;
        char *entry;
        for (entry = FS_FileList; entry < FS_FileListEnd;) {
            FS_PACKED_ENTRY_HEADER *header = (FS_PACKED_ENTRY_HEADER *)entry;
            f32 width;
            entry += sizeof(FS_PACKED_ENTRY_HEADER);
            NuQFntSetPointSize(system_qfont, 1.0f, 1.0f);
            width = NuQFntPrintLenU(system_qfont, entry);
            if (header->encoded_year != '#') {
                NuQFntSetPointSize(system_qfont, 0.8f, 1.0f);
                FS_MakeDateTimeString(header, date_time);
                width += NuQFntPrintLenU(system_qfont, date_time);
            }
            if (width > max_width) {
                max_width = width;
            }
            entry += NuStrLen(entry) + 1;
        }
        return max_width;
    }

    char *FS_GetFilterString(char *input, char *output) {
        char *end;
        char *trimmed;
        while (*input == '*' || *input == ' ') {
            ++input;
        }
        end = input;
        while (*end != 0 && *end != '|' && *end != '*') {
            ++end;
        }
        trimmed = end;
        while (trimmed != input && trimmed[-1] == ' ') {
            --trimmed;
        }
        while (input != trimmed) {
            *output++ = *input++;
        }
        *output = 0;
        return end;
    }

    char FileSelFilterBlocks[8][96] = {0};
    char FileSelFilterOutBlocks[8][12] = {0};
    i32 FileSelFilterBlockCount = 0;
    i32 FileSelFilterOutBlockCount = 0;

    i32 FS_FileNameFilter(char *name) {
        char lower[256];
        i32 i;
        i32 slot;
        i32 dots;
        char *retry;
        char *match;
        NuStrCpy(lower, name);
        NuStrToLower(lower);
        for (i = 0; i < FileSelFilterOutBlockCount; ++i) {
            if (NuStrStr(lower, FileSelFilterOutBlocks[i]) != NULL) {
                return 0;
            }
        }
        for (i = 0; i < FileSelFilterBlockCount; ++i) {
            retry = lower;
            do {
                match = retry;
                retry = NULL;
                for (slot = 0; slot < 7; ++slot) {
                    if (FileSelFilterBlocks[i][slot * 12] == 0) {
                        dots = 0;
                        while (*match != 0) {
                            if (*match++ == '.' && ++dots > 1) {
                                break;
                            }
                        }
                        if (dots <= 1) {
                            return 1;
                        }
                        break;
                    }
                    match = NuStrStr(match, &FileSelFilterBlocks[i][slot * 12]);
                    if (match == NULL) {
                        break;
                    }
                    if (slot == 0) {
                        retry = match + 1;
                    }
                }
            } while (retry != NULL);
        }
        return 0;
    }

    void FS_BuildFilterBlocks(char *input) {
        i32 slot;
        FileSelFilterBlockCount = 0;
        slot = 0;
        NuStrToLower(input);
        while (*input != 0) {
            input = FS_GetFilterString(input, &FileSelFilterBlocks[FileSelFilterBlockCount][slot * 12]);
            if (*input == '|') {
                FileSelFilterBlocks[FileSelFilterBlockCount][(slot + 1) * 12] = 0;
                ++FileSelFilterBlockCount;
                slot = 0;
                ++input;
            } else {
                ++slot;
            }
        }
        if (slot != 0) {
            ++FileSelFilterBlockCount;
        }
    }

    void FS_BuildFilterOutBlocks(char *input) {
        FileSelFilterOutBlockCount = 0;
        NuStrToLower(input);
        while (*input != 0) {
            input = FS_GetFilterString(input, FileSelFilterOutBlocks[FileSelFilterOutBlockCount]);
            if (FileSelFilterOutBlocks[FileSelFilterOutBlockCount][0] != 0) {
                ++FileSelFilterOutBlockCount;
            }
            if (*input == '|') {
                ++input;
            }
        }
    }

    void FS_SortStrings(char *start, char *end, i32 mode) {
        char temporary[256];
        char *current;
        char *candidate;
        i32 direction;
        i32 current_length;
        i32 candidate_length;
        if (mode != 0 && mode != 1) {
            u32 current_key;
            u32 candidate_key;
            i32 compare;
            direction = mode == 2 ? 1 : -1;
            for (current = start; current != end;) {
                current_key = (u32)(u8)current[6] - 'A';
                current_key = current_key * 13 + (u8)current[5] - 'A';
                current_key = current_key * 33 + (u8)current[4] - 'A';
                current_key = current_key * 25 + (u8)current[3] - 'A';
                current_key = current_key * 61 + (u8)current[2] - 'A';
                current_key = current_key * 61 + (u8)current[1] - 'A';
                current_length = NuStrLen(current);
                candidate = current + NuStrLen(current) + 1;
                while (candidate != end) {
                    candidate_key = (u32)(u8)candidate[6] - 'A';
                    candidate_key = candidate_key * 13 + (u8)candidate[5] - 'A';
                    candidate_key = candidate_key * 33 + (u8)candidate[4] - 'A';
                    candidate_key = candidate_key * 25 + (u8)candidate[3] - 'A';
                    candidate_key = candidate_key * 61 + (u8)candidate[2] - 'A';
                    candidate_key = candidate_key * 61 + (u8)candidate[1] - 'A';
                    candidate_length = NuStrLen(candidate);
                    compare = candidate_key < current_key ? 1 : (candidate_key > current_key ? -1 : 0);
                    if (compare == direction) {
                        NuStrCpy(temporary, candidate);
                        PCByteCopy(current + candidate_length + 1, current, candidate - current);
                        PCByteCopy(current, temporary, candidate_length + 1);
                        current_key = candidate_key;
                        current_length = candidate_length;
                    }
                    candidate += candidate_length + 1;
                }
                current += current_length + 1;
            }
            return;
        }
        if (mode == 0) {
            direction = 1;
        } else {
            direction = -1;
        }
        for (current = start; current != end;) {
            current_length = NuStrLen(current);
            candidate = current + NuStrLen(current) + 1;
            while (candidate != end) {
                candidate_length = NuStrLen(candidate);
                if (NuStrICmp(current + 7, candidate + 7) == direction) {
                    NuStrCpy(temporary, candidate);
                    PCByteCopy(current + candidate_length + 1, current, candidate - current);
                    PCByteCopy(current, temporary, candidate_length + 1);
                    current_length = candidate_length;
                }
                candidate += candidate_length + 1;
            }
            current += current_length + 1;
        }
    }

    char FS_LastFileName[64] = {0};

    void FS_SetCursorToLastFileName(void) {
        char *entry = FS_FileList;
        i32 index;
        if (FS_LastFileName[0] == '\0') {
            return;
        }
        for (index = 0; index < FS_NumFiles; ++index) {
            if (NuStrICmp(entry + 7, FS_LastFileName) == 0) {
                FS_CurrentCursorPos = entry;
                FS_CurrentPosFileNum = index;
                FS_CurrentPos = entry;
                FS_CursorLine = 0;
                if (index > 0) {
                    i32 file_count = FS_NumFiles;
                    i32 end_index = index + 14;
                    do {
                        i32 previous_length;
                        if (end_index <= file_count) {
                            break;
                        }
                        previous_length = FS_PrevNameLen(entry);
                        if (previous_length == 0) {
                            break;
                        }
                        --FS_CurrentPosFileNum;
                        ++FS_CursorLine;
                        entry -= previous_length;
                        --index;
                        --end_index;
                        FS_CurrentPos = entry;
                    } while (index > 0);
                }
                return;
            }
            entry += NuStrLen(entry) + 1;
        }
    }

    char FileSelVolumeNames[1024] = {0};
    char FS_ParentEntry[8] = "D######";

    void FS_GetDirList(char *path, char *filter, char *filter_out) {
        FILE_DIRECTORY_ENTRY entry;
        FS_PACKED_ENTRY_HEADER header;
        char *next = FS_FileList;
        char *drive;
        char *file_begin;
        i32 directory;
        i32 length;
        FS_BuildFilterBlocks(filter);
        FS_BuildFilterOutBlocks(filter_out);
        FS_CurrentCursorPos = FS_FileList;
        FS_CurrentPos = FS_FileList;
        FS_CursorLine = 0;
        FS_CurrentPosFileNum = 0;
        FS_NumFiles = 0;
        if (FS_ShowVolumes != 0) {
            GetLogicalDriveStringsA(1024, FileSelVolumeNames);
            for (drive = FileSelVolumeNames; *drive != 0; drive += length + 1) {
                length = NuStrLen(drive);
                header.kind = 'V';
                header.encoded_second = 'A';
                header.encoded_minute = 'A';
                header.encoded_hour = 'A';
                header.encoded_day = 'A';
                header.encoded_month = 'A';
                header.encoded_year = '#';
                PCByteCopy(next, &header, 7);
                next += 7;
                NuStrCpy(next, drive);
                next += length;
                *next++ = 0;
                ++FS_NumFiles;
            }
            FS_SortStrings(FS_FileList, next, 0);
        } else {
            directory = NuFileOpenDir(path);
            PCByteCopy(FS_FileList, FS_ParentEntry, 7);
            NuStrCpy(FS_FileList + 7, "..");
            ++FS_NumFiles;
            next = FS_FileList + 10;
            if (directory != 0) {
                while ((length = NuFileReadDir(directory, &entry)) != 0) {
                    if ((entry.name[0] == '.' && (entry.name[1] == 0 || (entry.name[1] == '.' && entry.name[2] == 0))) ||
                        !(entry.flags & 8)) {
                        continue;
                    }
                    if (next + length + 8 >= FS_FileList + sizeof(FS_FileList)) {
                        break;
                    }
                    header.kind = 'D';
                    header.encoded_day = entry.created.day + 'A';
                    header.encoded_hour = entry.created.hour + 'A';
                    header.encoded_minute = entry.created.minute + 'A';
                    header.encoded_second = entry.created.second + 'A';
                    header.encoded_month = entry.created.month + 'A';
                    header.encoded_year = (u8)(entry.created.year - 123);
                    PCByteCopy(next, &header, 7);
                    next += 7;
                    NuStrCpy(next, entry.name);
                    next += length;
                    *next++ = 0;
                    ++FS_NumFiles;
                }
            }
            FS_SortStrings(FS_FileList + 10, next, FS_SortMode);
            if (directory != 0) {
                NuFileCloseDir(directory);
            }
            file_begin = next;
            directory = NuFileOpenDir(path);
            if (directory != 0) {
                while ((length = NuFileReadDir(directory, &entry)) != 0) {
                    if ((entry.flags & 8) || !FS_FileNameFilter(entry.name)) {
                        continue;
                    }
                    if (next + length + 8 >= FS_FileList + sizeof(FS_FileList)) {
                        break;
                    }
                    header.kind = 'F';
                    header.encoded_day = entry.modified.day + 'A';
                    header.encoded_hour = entry.modified.hour + 'A';
                    header.encoded_minute = entry.modified.minute + 'A';
                    header.encoded_second = entry.modified.second + 'A';
                    header.encoded_month = entry.modified.month + 'A';
                    header.encoded_year = (u8)(entry.modified.year - 123);
                    PCByteCopy(next, &header, 7);
                    next += 7;
                    NuStrCpy(next, entry.name);
                    next += length;
                    *next++ = 0;
                    ++FS_NumFiles;
                }
            }
            FS_SortStrings(file_begin, next, FS_SortMode);
            if (directory != 0) {
                NuFileCloseDir(directory);
            }
        }
        FS_FileListEnd = next;
    }

    i32 FS_GetPadWithRepeat(FRAME_PAD *pad, float repeat, float elapsed) {
        static i32 LastPad = 0;
        static float PadRepeat = 0;

        if (pad->digital_buttons != LastPad) {
            LastPad = pad->digital_buttons;
            PadRepeat = 0.5f;
            return LastPad;
        }

        PadRepeat -= elapsed;
        if (PadRepeat < 0.0f) {
            PadRepeat += repeat;
            return LastPad;
        }
        return 0;
    }

    u8 FS_RefreshDir = 0;
    char FS_FilterOut[256] = "_PC. | _360. | _PS3.";
    void (*FS_Callback)(char *arg0, char *arg1) = 0;

    i32 ProcessFileSel3(f32 elapsed, FRAME_PAD *pad) {
        i32 buttons;
        i32 length;
        char *separator;
        f32 width;
        if (FS_Active == 0) {
            return 0;
        }
        if (FS_RefreshDir != 0) {
            FS_GetDirList(FS_Path, FS_Filter, FS_FilterOut);
            NuQFntPushCoordinateSystem(NUQFNT_CS_SIXTEENTHS);
            NuQFntSet(system_qfont);
            NuQFntSetPointSize(system_qfont, 1.0f, 1.0f);
            width = FS_GetDirTextWidth() * 0.0625;
            NuQFntPopCoordinateSystem();
            if (width < 240.0) {
                width = 240.0f;
            }
            FS_Width = width + 8.0;
            FS_GetPadWithRepeat(pad, 0.05f, elapsed);
            if (FS_LastFileName[0] != '\0') {
                FS_SetCursorToLastFileName();
            }
            FS_RefreshDir = 0;
        }
        buttons = FS_GetPadWithRepeat(pad, 0.05f, elapsed);
        if (buttons & PAD_BUTTON_DOWN) {
            FS_MoveCursorDown(1);
        }
        if (buttons & PAD_BUTTON_UP) {
            FS_MoveCursorUp(1);
        }
        if (buttons & PAD_BUTTON_L2) {
            FS_MoveCursorDown(14);
        }
        if (buttons & PAD_BUTTON_L1) {
            FS_MoveCursorUp(14);
        }
        if (buttons & PAD_BUTTON_R2) {
            FS_MoveCursorDown(FS_NumFiles);
        }
        if (buttons & PAD_BUTTON_R1) {
            FS_MoveCursorUp(FS_NumFiles);
        }
        if (buttons & PAD_BUTTON_SQUARE) {
            if (++FS_SortMode == 4) {
                FS_SortMode = 0;
            }
            FS_RefreshDir = 2;
            NuStrCpy(FS_LastFileName, FS_CurrentCursorPos + 7);
        }
        if (buttons & PAD_BUTTON_CIRCLE) {
            length = NuStrLen(FS_Path);
            if (length != 0 && FS_Path[length - 1] == '\\') {
                FS_Path[length - 1] = '\0';
            }
            separator = NuStrRChr(FS_Path, '\\');
            if (separator != NULL) {
                separator[1] = '\0';
            } else {
                NuStrCat(FS_Path, "\\");
                FS_ShowVolumes = 1;
            }
            FS_RefreshDir = 1;
            FS_LastFileName[0] = '\0';
        }
        if (buttons & PAD_BUTTON_CROSS) {
            if (FS_CurrentCursorPos[0] == 'V') {
                FS_ShowVolumes = 0;
                NuStrCpy(FS_Path, FS_CurrentCursorPos + 7);
                FS_LastFileName[0] = '\0';
                FS_RefreshDir = 1;
            } else if (FS_CurrentCursorPos[0] == 'D') {
                if (FS_CurrentCursorPos[7] == '.' && FS_CurrentCursorPos[8] == '.' && FS_CurrentCursorPos[9] == '\0') {
                    length = NuStrLen(FS_Path);
                    if (length != 0 && FS_Path[length - 1] == '\\') {
                        FS_Path[length - 1] = '\0';
                    }
                    separator = NuStrRChr(FS_Path, '\\');
                    if (separator != NULL) {
                        separator[1] = '\0';
                    } else {
                        NuStrCat(FS_Path, "\\");
                        FS_ShowVolumes = 1;
                    }
                } else {
                    length = NuStrLen(FS_Path);
                    if (length != 0 && FS_Path[length - 1] != '\\') {
                        NuStrCat(FS_Path, "\\");
                    }
                    NuStrCat(FS_Path, FS_CurrentCursorPos + 7);
                    NuStrCat(FS_Path, "\\");
                }
                FS_LastFileName[0] = '\0';
                FS_RefreshDir = 1;
            } else {
                FS_Active = 0;
                NuStrCpy(FS_LastFileName, FS_CurrentCursorPos + 7);
                if (FS_Callback != NULL) {
                    FS_Callback(FS_Path, FS_CurrentCursorPos + 7);
                }
                return 1;
            }
        }
        return 0;
    }

    i32 ProcessFileSel2(f32 elapsed, FRAME_PAD *pad) {
        return ProcessFileSel3(elapsed, pad);
    }

    void FileSelKill(void) {
        NuStrCpy(FS_LastFileName, FS_CurrentCursorPos + 7);
        FS_Active = 0;
    }

    void ProcessFileSel(f32 elapsed, FRAME_PAD *pad) {
        ProcessFileSel3(elapsed, pad);
        RenderFileSel();
        if (pad->digital_buttons & PAD_BUTTON_TRIANGLE) {
            FileSelKill();
        }
    }

    void FS_SetFileSelPathFromName(char *name) {
        char file_name[256] = "";
        NuStrCpy(FS_Path, name);
        if (NuStrRChr(FS_Path, '.') != NULL) {
            char *separator = NuStrRChr(FS_Path, '\\');
            if (separator != NULL) {
                NuStrCpy(file_name, separator + 1);
                separator[1] = '\0';
            } else {
                NuStrCpy(file_name, name);
                FS_Path[0] = '\0';
            }
        }
        if (file_name[0] != '\0') {
            ((i32(*)(char *, char *, i32))(&NuFileExtConvert))(FS_LastFileName, file_name, 255);
        } else {
            FS_LastFileName[0] = '\0';
        }
    }

    void FS_OpenFileSel(char *title, char *path, char *filter, char *filter_out, void (*callback)(char *path, char *name)) {
        if (title) {
            NuStrCpy(FS_Title, title);
        }
        if (path) {
            NuStrCpy(FS_Path, path);
        }
        if (filter) {
            NuStrCpy(FS_Filter, filter);
        } else {
            FS_Filter[0] = '\0';
        }
        if (filter_out) {
            NuStrCpy(FS_FilterOut, filter_out);
        } else {
            FS_FilterOut[0] = '\0';
        }
        FS_Active = 1;
        FS_Callback = callback;
        FS_RefreshDir = 2;
    }

    static i32 eduicbProcessFileSel(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD *pad) {
        if (ProcessFileSel3(delta_time, pad) && ((EDUI_FILE_SELECT *)item)->changed) {
            ((EDUI_FILE_SELECT *)item)->changed(menu, item, pad->digital_buttons);
        }
        return 0;
    }

    static i32 eduicbRenderFileSel(eduimenu_s *menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 selected) {
        FS_X = (f32)x;
        FS_Y = (f32)(y * 0.5);
        RenderFileSel3(0);
        menu->height = (i32)(FS_H + FS_H);
        menu->width = (i32)FS_W;
        return menu->height;
    }

    i32 near_clip_at_cursor = 0;
}

extern "C" void cbNearClipAtCursor(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    near_clip_at_cursor = item->flags & EDUI_ITEM_HIGHLIGHTED;
}

extern "C" void EduiDrawCross(NUVEC *position, u8 red, u8 green, u8 blue) {
    i32 colour = 0xff000000 | (blue << 16) | (green << 8) | red;
    NuRndrLine3dDbg(position->x - 0.5, position->y, position->z, position->x + 0.5, position->y, position->z, colour);
    NuRndrLine3dDbg(position->x, position->y - 0.5, position->z, position->x, position->y + 0.5, position->z, colour);
    NuRndrLine3dDbg(position->x, position->y, position->z - 0.5, position->x, position->y, position->z + 0.5, colour);
}

extern "C" void fun_00623a10(void) {
}

extern "C" {
    static edui_colour_pick_s *cp_item = NULL;
    static eduiitem_s *cp_info = NULL;
    static i32 (*cp_process)(eduimenu_s *, eduiitem_s *, f32, FRAME_PAD *) = NULL;
}

i32 cbProcessColourPick(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD *pad) {
    edui_colour_pick_s *picker = cp_item;
    i32 result = cp_process(menu, item, delta_time, pad);
    char text[256];
    if (cp_info->data != 0) {
        NuSPrintf(text, "R:%1.2f G:%1.2f B:%1.2f", picker->red, picker->green, picker->blue);
    } else {
        NuSPrintf(text, "H:%1.2f S:%1.2f V:%1.2f", picker->hue, picker->saturation, picker->value);
    }
    eduiItemSetText(cp_info, text);
    return result;
}

extern "C" {
    static eduiitem_s *cp_paste = NULL;
    static f32 *EduiColourTargetRed = NULL;
    static f32 *EduiColourTargetGreen = NULL;
    static f32 *EduiColourTargetBlue = NULL;
    static f32 EduiColourClipboardRed = 0.5f;
    static f32 EduiColourClipboardGreen = 0.5f;
    static f32 EduiColourClipboardBlue = 0.5f;
}

/* Copies the picker's target colour to the clipboard and tints the Paste button with it. The original calls
   __ftol2_sse for each channel; VC8 inlines the conversions here because the packed value is stored to
   the u32 eduiitem_s::colours, which the original's code suggests was declared i32. */
void cbCopy(eduimenu_s *, eduiitem_s *, u32) {
    EduiColourClipboardRed = *EduiColourTargetRed;
    EduiColourClipboardGreen = *EduiColourTargetGreen;
    EduiColourClipboardBlue = *EduiColourTargetBlue;
    i32 packed = 0x80;
    packed = (packed << 8) | ((i32)(EduiColourClipboardBlue * 255.0) & 0xff);
    packed = (packed << 8) | ((i32)(EduiColourClipboardGreen * 255.0) & 0xff);
    packed = (packed << 8) | ((i32)(EduiColourClipboardRed * 255.0) & 0xff);
    cp_paste->colours[2] = packed;
}

extern "C" {
    static f32 *EduiColourTargetAlpha = NULL;
    static u32 *EduiColourTargetPacked = NULL;
}

void cbPaste(eduimenu_s *, eduiitem_s *, u32) {
    *EduiColourTargetRed = EduiColourClipboardRed;
    *EduiColourTargetGreen = EduiColourClipboardGreen;
    *EduiColourTargetBlue = EduiColourClipboardBlue;
    if (EduiColourTargetPacked != NULL) {
        u32 packed = (u32)(*EduiColourTargetAlpha * 255.0);
        packed = (packed << 8) | ((u32)(*EduiColourTargetBlue * 255.0) & 255);
        packed = (packed << 8) | ((u32)(*EduiColourTargetGreen * 255.0) & 255);
        packed = (packed << 8) | ((u32)(*EduiColourTargetRed * 255.0) & 255);
        *EduiColourTargetPacked = packed;
    }
    eduiItemColourPickSetRGB(cp_item, EduiColourClipboardRed, EduiColourClipboardGreen, EduiColourClipboardBlue);
}

void cbToggleIndicatorMode(eduimenu_s *, eduiitem_s *item, u32) {
    char text[256];
    item->data = 1 - item->data;
    edui_colour_pick_s *picker = cp_item;
    if (item->data != 0) {
        NuSPrintf(text, "R:%1.2f G:%1.2f B:%1.2f", picker->red, picker->green, picker->blue);
    } else {
        NuSPrintf(text, "H:%1.2f S:%1.2f V:%1.2f", picker->hue, picker->saturation, picker->value);
    }
    eduiItemSetText(item, text);
}

/* Draws one line between two vertices, optionally transformed by a matrix
 * (copied to aligned storage when it is not 16-byte aligned). */
static void RndrVtxLine3d(NUMTX *matrix, NURND_VERTEX3D *vertices) {
    __declspec(align(16)) NUVEC first = vertices[0].position;
    __declspec(align(16)) NUVEC second = vertices[1].position;
    if (matrix != NULL) {
        NUMTX matrix_copy;
        NUMTX *transform = matrix;
        if (((size_t)(transform) & 15) != 0) {
            matrix_copy = *transform;
            transform = &matrix_copy;
        }
        NuVecMtxTransform(&first, &first, transform);
        NuVecMtxTransform(&second, &second, transform);
    }
    NuRndrLine3dDbg(first.x, first.y, first.z, second.x, second.y, second.z, vertices[0].colour);
}

/* Camera-facing circle outline around centre. */
void RndrOSphere(NUVEC *centre, f32 radius, i32 colour, i32 segments, NUMTL *) {
    NUMTX matrix = global_camera.mtx;
    NURND_VERTEX3D vertices[2];
    i32 step;
    i32 i;
    matrix.m30 = centre->x;
    matrix.m31 = centre->y;
    matrix.m32 = centre->z;
    vertices[0].position.x = radius;
    vertices[0].position.y = 0.0f;
    vertices[0].position.z = 0.0f;
    vertices[0].colour = colour;
    vertices[1].colour = colour;
    step = 0x10000 / segments;
    NuVecRotateZ(&vertices[1].position, &vertices[0].position, step);
    for (i = 0; i < segments; i++) {
        RndrVtxLine3d(&matrix, vertices);
        vertices[0].position = vertices[1].position;
        NuVecRotateZ(&vertices[1].position, &vertices[1].position, step);
    }
}

/* Circle outline in the XZ plane around centre. */
extern "C" void RndrCircleXZ(NUVEC *centre, f32 radius, i32 colour, i32 segments) {
    NUMTX matrix = NuMtxSetIdentityTemplate;
    NURND_VERTEX3D vertices[2];
    i32 step;
    i32 i;
    matrix.m30 = centre->x;
    matrix.m31 = centre->y;
    matrix.m32 = centre->z;
    vertices[0].position.x = radius;
    vertices[0].position.y = 0.0f;
    vertices[0].position.z = 0.0f;
    vertices[0].colour = colour;
    vertices[1].colour = colour;
    step = 0x10000 / segments;
    NuVecRotateY(&vertices[1].position, &vertices[0].position, step);
    for (i = 0; i < segments; i++) {
        RndrVtxLine3d(&matrix, vertices);
        vertices[0].position = vertices[1].position;
        NuVecRotateY(&vertices[1].position, &vertices[1].position, step);
    }
}

/* Camera-facing square outline of half-size radius around centre. */
void RndrOSquare(NUVEC *centre, f32 radius, i32 colour, NUMTL *) {
    f32 corners[8] = {-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};
    NUMTX matrix = global_camera.mtx;
    NURND_VERTEX3D vertices[2];
    i32 i;
    matrix.m30 = centre->x;
    matrix.m31 = centre->y;
    matrix.m32 = centre->z;
    vertices[0].position.z = 0.0f;
    vertices[0].colour = colour;
    vertices[1].position.z = 0.0f;
    vertices[1].colour = colour;
    for (i = 0; i < 4; i++) {
        vertices[0].position.x = corners[(i & 3) * 2] * radius;
        vertices[0].position.y = corners[(i * 2 + 1) & 7] * radius;
        vertices[1].position.x = corners[(i * 2 + 2) & 7] * radius;
        vertices[1].position.y = corners[(i * 2 + 3) & 7] * radius;
        RndrVtxLine3d(&matrix, vertices);
    }
}

extern "C" void FlushMenuHighlights(eduimenu_s *menu) {
    eduiitem_s *item;
    for (item = menu->first; item != NULL; item = item->next) {
        if (item->type == EDUIITEM_SEL) {
            item->highlighted = 0;
        }
    }
}

extern "C" {

    void cbmcfgCameraSpeed(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
        f32 divisor = 512.0f / ((edui_slider_s *)item)->value;
        edcam_s *cam = gp_cam;
        f32 speed = 0.2f / divisor;
        cam->position_speed.x = speed;
        cam->position_speed.y = speed;
        cam->position_speed.z = speed;
        cam->distance_speed = 1.0f / divisor;
    }

    i32 edmain_auto_speed = 1;

    void cbSetAutoSpeed(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
        edcam_s *cam;
        edmain_auto_speed = item->highlighted;
        cam = gp_cam;
        if (edmain_auto_speed != 0) {
            cam->auto_move_base = 0.15f;
            cam->auto_move_dist_scale = 0.2f;
            cam->auto_zoom_base = 0.01f;
            cam->auto_zoom_dist_scale = 0.1f;
        } else {
            cam->auto_move_base = 0.0f;
            cam->auto_move_dist_scale = 0.0f;
            cam->auto_zoom_base = 0.0f;
            cam->auto_zoom_dist_scale = 0.0f;
        }
    }

    void eduicbItemDestroy(eduimenu_s *menu, eduiitem_s *item) {
        EduiClearInteractions(menu, item);
        if (item->text != NULL) {
            NuFree(item->text, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x29e);
            item->text = NULL;
        }
        NuFree(item, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x2a2);
        menu->first_visible = menu->first;
    }

    void eduiMenuDestroyItems(eduimenu_s *menu) {
        if (menu) {
            eduicbMenuCloseAllexpanders(menu);
            while (menu->first) {
                eduiitem_s *next = menu->first->next;
                menu->first->destroy(menu, menu->first);
                menu->first = next;
            }
            menu->last = NULL;
            menu->selected = NULL;
            menu->first_visible = NULL;
            menu->last_visible = NULL;
        }
    }
    static struct eduimenu_s *processing_menu = 0;

    void eduiMenuDestroy(eduimenu_s *menu) {
        eduimenu_s *parent;
        if (menu != NULL) {
            if (eduiInteractLocked != NULL && eduiInteractLocked->menu == menu) {
                eduiInteractLocked = NULL;
            }
            if (menu == processing_menu) {
                processing_menu = NULL;
            }
            parent = menu->parent;
            if (parent != NULL) {
                if (menu == active_menu) {
                    active_menu = parent;
                }
                parent->child = NULL;
            }
            if (menu == default_active_menu) {
                default_active_menu = NULL;
            }
            if (menu == active_menu) {
                active_menu = NULL;
            }
            if (menu->child != NULL) {
                menu->child->parent = NULL;
            }
            eduiMenuDestroyItems(menu);
            NuFree(menu, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x3d0);
        }
    }

    void eduiMenuFitWidth(eduimenu_s *menu, i32 padding) {
        eduiitem_s *first = menu->first;
        eduiitem_s *item;
        i32 width = 0;
        i32 property_label_width = 0;
        i32 property_value_width = 0;
        i32 item_text_width = 0;
        i32 property_width;
        eduicbMenuOpenAllexpanders(menu);
        NuQFntPushCoordinateSystem(NUQFNT_CS_SIXTEENTHS);
        NuQFntSetScale(edui_font, edui_font_scale_x, edui_font_scale_y);
        if (menu->title != NULL) {
            item_text_width = (i32)NuQFntPrintLenU(edui_font, menu->title) >> 4;
            if (item_text_width > width) {
                width = item_text_width;
            }
        }
        for (item = first; item != NULL; item = item->next) {
            switch (item->type) {
                case EDUIITEM_TEXT_PICK:
                    item_text_width = ((i32)(NuQFntHeight(edui_font) * 1.25) >> 3) * 10;
                    break;
                case EDUIITEM_CHECK:
                case EDUIITEM_TOGGLE:
                    if (item->text != NULL) {
                        i32 icon_width = (i32)(NuQFntHeight(edui_font) * 1.25) >> 3;
                        i32 text_width = (i32)NuQFntPrintLenU(edui_font, item->text) >> 4;
                        item_text_width = icon_width + text_width;
                    }
                    break;
                case EDUIITEM_SLIDER: {
                    edui_slider_s *slider = (edui_slider_s *)item;
                    char format[512];
                    char text[512];
                    NuStrCpy(format, item->text);
                    NuStrCat(format, slider->format);
                    NuSPrintf(text, format, -0.0);
                    item_text_width = (i32)NuQFntPrintLenU(edui_font, text) >> 4;
                    break;
                }
                case EDUIITEM_PROP: {
                    edui_prop_s *property = (edui_prop_s *)item;
                    i32 label_width = item->text != NULL ? (i32)NuQFntPrintLenU(edui_font, item->text) >> 4 : 30;
                    if (label_width > property_label_width) {
                        property_label_width = label_width;
                    }
                    item_text_width = property->property_text != NULL ? (i32)NuQFntPrintLenU(edui_font, property->property_text) >> 4 : 30;
                    if (item_text_width > property_value_width) {
                        property_value_width = item_text_width;
                    }
                    break;
                }
                default:
                    if (item->text != NULL) {
                        item_text_width = (i32)NuQFntPrintLenU(edui_font, item->text) >> 4;
                    }
                    break;
            }
            if (item_text_width > width) {
                width = item_text_width;
            }
        }
        property_width = property_label_width + property_value_width + ((i32)NuQFntHeight(edui_font) >> 3);
        if (width < property_width) {
            width = property_width;
        }
        for (item = menu->first; item != NULL; item = item->next) {
            if (item->type == EDUIITEM_PROP) {
                ((edui_prop_s *)item)->label_width = (f32)(property_label_width + 1);
            }
        }
        menu->width = width + padding;
        NuQFntPopCoordinateSystem();
        eduicbMenuCloseAllexpanders(menu);
    }

    static i32 eduicbProcessSel(eduimenu_s *menu, eduiitem_s *item, f32 unused2, FRAME_PAD *pad) {
        edui_sel_s *selection = ((edui_sel_s *)(item));
        if (pad->digital_buttons_pressed & EDUI_CURSOR_PRIMARY) {
            eduiMenuHighlight(menu, item);
            if (selection->selected) {
                selection->selected(menu, item, pad->digital_buttons);
            }
            return 0;
        }
        if ((pad->digital_buttons & EDUI_CURSOR_PRIMARY) && selection->held) {
            selection->held(menu, item, pad->digital_buttons);
        }
        return 0;
    }

    void eduicbItemTextPickDestroy(eduimenu_s *menu, eduiitem_s *item) {
        edui_textpicker_s *picker = (edui_textpicker_s *)item;
        if (picker->format != NULL) {
            NuFree(picker->format, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x623);
            picker->format = NULL;
        }
        eduicbItemDestroy(menu, item);
    }

    static i32 EduiTextPickKeyboard(edui_textpicker_s *picker, eduimenu_s *menu, FRAME_PAD *pad) {
        u32 modifiers;
        i32 key = NuKeyGet(&modifiers);
        NuStrLen(picker->value);
        if (key != -1) {
            switch (key) {
                case DIK_ESCAPE:
                case DIK_RETURN:
                    if (picker->callback) {
                        picker->callback(menu, &picker->base, pad->digital_buttons);
                    }
                    break;
                case DIK_LEFT:
                    EduiTextPickPreviousColumn(picker);
                    break;
                case DIK_RIGHT:
                    EduiTextPickNextColumn(picker);
                    break;
                case DIK_UP:
                    EduiTextPickPreviousRow(picker);
                    break;
                case DIK_DOWN:
                    EduiTextPickNextRow(picker);
                    break;
                case DIK_BACK:
                case DIK_DELETE:
                    EduiTextPickDelete(picker);
                    break;
                default: {
                    char character = NuKeyToAscii(key, modifiers & 1);
                    if (character) {
                        EduiTextPickInsert(picker, character);
                    }
                } break;
            }
        }
        return 0;
    }

    char *EduiTextCopyHelp = "Copy L2+R2 Paste L1+R1";
}

extern "C" i32 eduicbRenderTextPick(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 unused_selected) {
    edui_textpicker_s *picker = (edui_textpicker_s *)item;
    i32 height = (i32)(NuQFntHeight(edui_font) * 1.25) >> 3;
    f64 font_height = NuQFntHeight(edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline(edui_font));
    item->x = x;
    item->y = y;
    char letter[2] = "0";
    if (picker->editing) {
        EduiRenderWidth = MAX(height * 10, width);
        if (picker->keyboard_flags & 2) {
            EduiRndrRect2di(x << 4, y << 3, EduiRenderWidth << 4, (height * 6) << 3, item->colours[2], uimtls[ui_bgmtl]);
        } else {
            EduiRndrRect2di(x << 4, y << 3, EduiRenderWidth << 4, (height * 5) << 3, item->colours[2], uimtls[ui_bgmtl]);
        }
        i32 row;
        for (row = 0; row < 4; ++row) {
            for (i32 column = 0; column < 10; ++column) {
                if (row == picker->keyboard_row && column == picker->keyboard_column) {
                    EduiQFntSet(edui_font);
                    EduiQFntSetColour(edui_font, item->colours[1]);
                } else {
                    EduiQFntSet(edui_font);
                    EduiQFntSetColour(edui_font, item->colours[0]);
                }
                letter[0] = textrow[row][column];
                EduiPrintAlignedText(edui_font, (x + height * column) << 4, ((y + row * height) << 3) + baseline, 16, letter);
            }
        }
        EduiQFntSet(edui_font);
        EduiQFntSetColour(edui_font, item->colours[1]);
        i32 value_y = ((y + height * row) << 3) + baseline;
        EduiPrintAlignedText(edui_font, x << 4, value_y, 16, picker->value);
        EduiRenderTextCursor(picker->value, picker->cursor, x << 4, value_y, item->colours[1] ^ 0xffffff);
        ++row;
        if (picker->keyboard_flags & 2) {
            EduiQFntSet(edui_font);
            EduiQFntSetColour(edui_font, item->colours[0]);
            EduiPrintAlignedText(edui_font, x << 4, ((y + height * row) << 3) + baseline, 16,
                                 EduiTextCopyBuffer[0] ? EduiTextCopyBuffer : EduiTextCopyHelp);
            return height * 6;
        }
        return height * 5;
    }
    if (picker->keyboard_flags & 1) {
        EduiRndrRect2di(x << 4, y << 3, width << 4, height << 4, item->colours[2 + item->highlighted], uimtls[ui_bgmtl]);
        EduiQFntSet(edui_font);
        EduiQFntSetColour(edui_font, item->colours[item->highlighted]);
        i32 centre = (x * 2 + width) << 3;
        EduiPrintAlignedText(edui_font, centre, (y << 3) + baseline, 64, item->text);
        EduiPrintAlignedText(edui_font, centre, ((y + height) << 3) + baseline, 64, picker->format, picker->value);
        return height * 2;
    }
    EduiRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2 + item->highlighted], uimtls[ui_bgmtl]);
    char formatted[64];
    NuSPrintf(formatted, picker->format, picker->value);
    EduiQFntSet(edui_font);
    EduiQFntSetColour(edui_font, item->colours[item->highlighted]);
    EduiPrintAlignedText(edui_font, x << 4, (y << 3) + baseline, 16, "%s %s", item->text, formatted);
    return height;
}

extern "C" {

    void eduicbItemFilePickDestroy(eduimenu_s *menu, eduiitem_s *item) {
        edui_file_pick_s *picker = (edui_file_pick_s *)item;
        if (picker->format != NULL) {
            NuFree(picker->format, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x946);
            picker->format = NULL;
        }
        eduicbItemDestroy(menu, item);
    }

    void eduicbPickCFGCancel(eduimenu_s *menu, eduimenu_s *child) {
        eduiMenuDestroy(menu);
    }

    eduiitem_s *eduiItemColourPickCreate(u32 data, void *colours, void *callback, char *text) {
        edui_colour_pick_s *item =
            (edui_colour_pick_s *)NuAlloc(sizeof(edui_colour_pick_s), "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0xb6c);
        if (item) {
            memset(item, 0, sizeof(edui_colour_pick_s));
            item->base.data = data;
            item->base.type = EDUIITEM_COLOUR_PICK;
            memcpy(item->base.colours, colours, sizeof(item->base.colours));
            item->base.text_alignment = EDUI_ALIGN_CENTRE;
            item->base.selection_group = 0;
            item->base.process = eduicbProcessColourPick;
            item->base.render = eduicbRenderColourPick;
            item->base.destroy = eduicbItemDestroy;
            item->interact = eduicbInteractColourPick;
            eduiItemSetText(&item->base, text);
            item->cursor_x = 0.5f;
            item->cursor_y = 0.5f;
            item->hue = 180.0f;
            item->value = 0.5f;
            item->saturation = 1.0f;
            item->changed = (void (*)(eduimenu_s *, eduiitem_s *, u32))callback;
            return &item->base;
        }
        return NULL;
    }

    eduiitem_s *eduiItemGreyPickCreate(u32 data, void *colours, void *callback, char *text) {
        eduiitem_s *item = eduiItemColourPickCreate(data, colours, callback, text);
        if (item) {
            item->type = EDUIITEM_GREY_PICK;
            item->process = eduicbProcessGreyPick;
            item->render = eduicbRenderGreyPick;
        }
        return item;
    }

    eduiitem_s *eduiItemTexturePickCreate(u32 data, void *colours, void *callback, char *text) {
        edui_texture_pick_s *item =
            (edui_texture_pick_s *)NuAlloc(sizeof(edui_texture_pick_s), "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0xce1);
        if (item) {
            memset(item, 0, sizeof(edui_texture_pick_s));
            item->base.data = data;
            item->base.type = EDUIITEM_TEXTURE_PICK;
            memcpy(item->base.colours, colours, sizeof(item->base.colours));
            item->base.text_alignment = EDUI_ALIGN_CENTRE;
            item->base.selection_group = 0;
            item->base.process = eduicbProcessTexturePick;
            item->base.render = eduicbRenderTexturePick;
            item->base.destroy = eduicbItemDestroy;
            eduiItemSetText(&item->base, text);
            item->uv_x[0] = 0.4f;
            item->uv_y[0] = 0.4f;
            item->changed = (void (*)(eduimenu_s *, eduiitem_s *, u32))callback;
            item->uv_x[1] = 0.5f;
            item->selected_corner = 0;
            item->uv_y[1] = 0.5f;
            item->zoom = 1.0f;
            return &item->base;
        }
        return NULL;
    }

    eduiitem_s *eduiItemGraphCreate(u32 data, void *colours, void *callback, nugraph_s *graph, i32 width, i32 height) {
        edui_graph_s *item =
            (edui_graph_s *)NuAlloc(sizeof(edui_graph_s), "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0xd9f);
        i32 i;
        if (item) {
            memset(item, 0, sizeof(edui_graph_s));
            item->base.data = data;
            item->base.type = EDUIITEM_GRAPH;
            memcpy(item->base.colours, colours, sizeof(item->base.colours));
            item->cursor_x = 0.5f;
            item->cursor_y = 0.5f;
            item->x_scale = 1.0f;
            item->y_scale = 1.0f;
            item->base.text_alignment = EDUI_ALIGN_CENTRE;
            item->base.selection_group = 0;
            item->base.process = eduicbProcessGraph;
            item->base.render = eduicbRenderGraph;
            item->base.destroy = eduicbItemDestroy;
            item->changed = (void (*)(eduimenu_s *, eduiitem_s *, u32))callback;
            item->graph = graph;
            item->width = width;
            item->height = height;
            item->selected_point = -1;
            for (i = 0; i < 8; i++) {
                item->onion_skins[i] = NULL;
            }
            if (graph->point_count < 2) {
                nugraphInit(graph);
            }
            nugraphCalcCurve(graph, 100);
            return &item->base;
        }
        return NULL;
    }

    void eduicbItemSliderDestroy(eduimenu_s *menu, eduiitem_s *item) {
        edui_slider_s *slider = (edui_slider_s *)item;
        if (slider->format != NULL) {
            NuFree(slider->format, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0xfbc);
            slider->format = NULL;
        }
        eduicbItemDestroy(menu, item);
    }

    eduiitem_s *eduiItemSliderCreate(u32 data, void *colours, i32 group, void *callback, f32 minimum, f32 range, f32 value, char *text) {
        edui_slider_s *item =
            (edui_slider_s *)NuAlloc(sizeof(edui_slider_s), "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0xfd4);
        if (item) {
            memset(item, 0, sizeof(edui_slider_s));
            item->base.data = data;
            item->base.type = EDUIITEM_SLIDER;
            memcpy(item->base.colours, colours, sizeof(item->base.colours));
            item->base.text_alignment = EDUI_ALIGN_CENTRE;
            item->base.selection_group = group;
            item->base.process = eduicbProcessSlider;
            item->base.render = eduicbRenderSlider;
            item->base.destroy = eduicbItemSliderDestroy;
            item->interact = eduicbInteractSlider;
            eduiItemSetText(&item->base, text);
            item->minimum = minimum;
            item->range = range;
            item->granularity = 0.001f;
            item->changed = (void (*)(eduimenu_s *, eduiitem_s *, u32))callback;
            item->format = NULL;
            eduiItemSliderSetFmt(item, "(%1.03f)");
            eduiItemSliderSetVal(item, value);
            return &item->base;
        }
        return NULL;
    }

    eduiitem_s *eduiItemSliderCreateInt(u32 data, void *colours, i32 group, void *callback, i32 minimum, i32 maximum, i32 value,
                                        char *text) {
        edui_slider_s *item = (edui_slider_s *)eduiItemSliderCreate(data, colours, group, callback, minimum, maximum, value, text);
        if (item) {
            eduiItemSliderSetFmt(item, "(%d)");
            item->granularity = 1.0f;
            item->base.render = eduicbRenderSliderInt;
        }
        return &item->base;
    }
}

extern "C" eduiitem_s *eduiItemNumberCreate(u32 data, void *colours, i32 group, void *callback, f32 minimum, f32 range, f32 value,
                                            char *text) {
    eduiitem_s *item = eduiItemSliderCreate(data, colours, group, callback, minimum, range, value, text);
    if (item != NULL) {
        item->render = eduicbRenderNumber;
    }
    return item;
}

extern "C" eduiitem_s *eduiItemTextSelectorCreate(u32 data, void *colours, i32 group, void *callback, i32 count, i32 value, char *text,
                                                  char **options) {
    edui_text_selector_s *item =
        (edui_text_selector_s *)NuAlloc(sizeof(edui_text_selector_s), "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x10e4);
    if (item) {
        memset(item, 0, sizeof(edui_text_selector_s));
        item->slider.base.data = data;
        item->slider.base.type = EDUIITEM_SLIDER;
        memcpy(item->slider.base.colours, colours, sizeof(item->slider.base.colours));
        item->slider.base.text_alignment = EDUI_ALIGN_CENTRE;
        item->slider.base.selection_group = group;
        item->slider.base.process = eduicbProcessSlider;
        item->slider.base.render = eduicbRenderTextSelector;
        item->slider.base.destroy = eduicbItemSliderDestroy;
        item->slider.interact = eduicbInteractSlider;
        eduiItemSetText(&item->slider.base, text);
        item->slider.minimum = 0.0f;
        item->slider.range = (f32)(count - 1);
        item->slider.granularity = 1.0f;
        item->slider.changed = (void (*)(eduimenu_s *, eduiitem_s *, u32))callback;
        item->slider.format = NULL;
        f32 initial = (f32)value;
        item->slider.normalized_value = (initial - item->slider.minimum) / item->slider.range;
        item->slider.value = initial;
        item->slider.change_timer = 60;
        if (item->slider.changed) {
            item->slider.changed(NULL, &item->slider.base, 0);
        }
        item->options = options;
        return &item->slider.base;
    }
    return NULL;
}

extern "C" {

    /* Keyboard editing of the value text in eduiPropTextEdit. */
    static i32 EduiPropKeyboard(edui_prop_s *property, eduimenu_s *menu) {
        u32 modifiers;
        i32 length;
        i32 key;
        char *cursor;
        char *insertion;
        if (!property->editing) {
            return 0;
        }
        length = NuStrLen(eduiPropTextEdit);
        if (eduiPropTextPos < 0) {
            eduiPropTextPos = 0;
        }
        if (eduiPropTextPos > length) {
            eduiPropTextPos = length;
        }
        key = NuKeyGet(&modifiers);
        if (key == -1) {
            return 0;
        }
        switch (key) {
            case DIK_RETURN:
                eduiItemPropSetText(property, eduiPropTextEdit);
                if (property->changed) {
                    property->changed(menu, &property->base, 0);
                }
            case DIK_ESCAPE:
                property->editing = 0;
                eduiPropTextPos = -1;
                return 0;
            case DIK_LEFT:
                if (eduiPropTextPos > 0) {
                    eduiPropTextPos--;
                }
                return 0;
            case DIK_RIGHT:
                if (eduiPropTextPos < length) {
                    eduiPropTextPos++;
                }
                return 0;
            case DIK_BACK:
                if (eduiPropTextPos > 0) {
                    eduiPropTextPos--;
                    for (cursor = &eduiPropTextEdit[eduiPropTextPos]; *cursor; cursor++) {
                        *cursor = cursor[1];
                    }
                }
                return 0;
            case DIK_DELETE:
                if (eduiPropTextPos < length) {
                    for (cursor = &eduiPropTextEdit[eduiPropTextPos]; *cursor; cursor++) {
                        *cursor = cursor[1];
                    }
                }
                return 0;
            default: {
                char letter = NuKeyToAscii(key, modifiers & 1);
                if (letter) {
                    insertion = &eduiPropTextEdit[eduiPropTextPos];
                    eduiPropTextEdit[length + 1] = '\0';
                    for (cursor = &eduiPropTextEdit[length]; cursor > insertion; cursor--) {
                        *cursor = cursor[-1];
                    }
                    eduiPropTextPos++;
                    *cursor = letter;
                }
            }
        }
        return 0;
    }

    static i32 eduicbProcessProp(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD *pad) {
        EduiPropDragLabel((edui_prop_s *)item, menu);
        EduiPropKeyboard((edui_prop_s *)item, menu);
        EduiPropButton((edui_prop_s *)item, menu, pad);
        return 0;
    }

    static i32 eduicbRenderProp(eduimenu_s *menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 height) {
        edui_prop_s *property = (edui_prop_s *)item;
        char prefix[256];
        u32 divider_colours[2];
        u32 button_colours[2];
        i32 points[6];
        f32 draw_x = (f32)x;
        f32 draw_y = (f32)y;
        f32 draw_width = (f32)width;
        f32 row_height;
        f32 baseline;
        f64 indent_width;
        f32 indentation;
        f32 value_x;
        f32 clip_x;
        i32 depth;
        item->x = x;
        item->y = y;
        divider_colours[0] = 0x0ff00000;
        divider_colours[1] = 0xffffffff;
        button_colours[0] = 0xff000000;
        button_colours[1] = 0xffffffff;
        if ((EduiPropOverButton(property) && eduiInteractLocked == NULL) || property->button_held) {
            property->button_hover = 1;
        } else {
            property->button_hover = 0;
        }
        if (edui_donotdraw == 0) {
            NuQFntSet(edui_font);
        }
        switch (item->colour_index) {
            case 1:
                if (edui_donotdraw == 0) {
                    NuQFntSetColour(edui_font, 0xff000060);
                }
                break;
            case 2:
                if (edui_donotdraw == 0) {
                    NuQFntSetColour(edui_font, 0xff006000);
                }
                break;
            case 3:
                if (edui_donotdraw == 0) {
                    NuQFntSetColour(edui_font, 0xff600000);
                }
                break;
            default:
                if (edui_donotdraw == 0) {
                    NuQFntSetColour(edui_font, item->colours[item->highlighted]);
                }
                break;
        }
        row_height = NuQFntHeight(edui_font) * 1.25 * (1.0f / EduiScaleY);
        baseline = (NuQFntHeight(edui_font) * 0.125 + NuQFntBaseline(edui_font)) * (1.0f / EduiScaleY);
        indent_width = row_height - 2.0;
        indentation = property->depth * indent_width;
        if (property->editing) {
            f32 text_width = NuQFntPrintLenU(edui_font, eduiPropTextEdit) * (1.0f / EduiScaleX);
            f32 edit_width = draw_width - property->label_width - 4.0;
            f32 caret_x;
            text_width = (text_width > edit_width ? text_width : edit_width);
            EduiDrawRect(draw_x, draw_y, draw_width, row_height, item->colours[2 + item->highlighted], uimtls[0]);
            EduiDrawRect(draw_x + property->label_width + 2.0, draw_y + 2.0, text_width, row_height - 4.0, 0xffffffff, uimtls[0]);
            for (depth = 0; depth < property->depth; depth++) {
                f32 guide_x = draw_x + (depth + 0.5) * indent_width;
                EduiDrawLine(guide_x, draw_y, guide_x, draw_y + row_height, 0xff000000, uimtls[0]);
            }
            EduiDrawLine(draw_x + property->label_width, draw_y, draw_x + property->label_width, draw_y + row_height,
                         divider_colours[property->divider_active], uimtls[0]);
            eduiFntPrintClipEx(edui_font, draw_x + indentation, draw_y + baseline, 16, draw_x - indentation, property->label_width,
                               item->text);
            EduiFntPrintScaled(edui_font, draw_x + property->label_width + 2.0f, draw_y + baseline, EDUI_ALIGN_LEFT, eduiPropTextEdit);
            NuStrNCpy(prefix, eduiPropTextEdit, eduiPropTextPos + 1);
            prefix[eduiPropTextPos + 1] = '\0';
            caret_x = draw_x + property->label_width + 2.0f + NuQFntPrintLenU(edui_font, prefix) * (1.0f / EduiScaleX);
            EduiDrawLine(caret_x, draw_y, caret_x, draw_y + row_height, 0xff000000, uimtls[0]);
        } else {
            property->button_size = indent_width;
            property->button_x = draw_x + draw_width - (property->button_size + 1.0f);
            property->button_y = draw_y + 1.0f;
            if (menu != NULL && EduiMenuChainActive(menu)) {
                EduiDrawRect(draw_x, draw_y, draw_width, row_height, item->colours[2 + item->highlighted], uimtls[ui_bgmtl]);
            }
            EduiDrawLine(draw_x + property->label_width, draw_y, draw_x + property->label_width, draw_y + row_height,
                         divider_colours[property->divider_active], uimtls[0]);
            for (depth = 0; depth < property->depth; depth++) {
                f32 guide_x = draw_x + (depth + 0.5) * indent_width;
                EduiDrawLine(guide_x, draw_y, guide_x, draw_y + row_height, 0xff000000, uimtls[0]);
            }
            eduiFntPrintClipEx(edui_font, draw_x + indentation, draw_y + baseline, 16, draw_x - indentation, property->label_width,
                               item->text);
            value_x = property->button_x - 1.0;
            clip_x = draw_x + property->label_width;
            eduiFntPrintClipEx(edui_font, value_x, draw_y + baseline, 32, clip_x, value_x - clip_x, property->property_text);
            if (property->button_type == 3 && edui_donotdraw == 0) {
                NuRndrRect2di((i32)(property->button_x * EduiScaleX), (i32)(property->button_y * EduiScaleY),
                              (i32)(property->button_size * EduiScaleX), (i32)(property->button_size * EduiScaleY), property->button_colour,
                              uimtls[0]);
            }
            EduiDrawLineRect(property->button_x, property->button_y, property->button_size, property->button_size, 0xff000000,
                             uimtls[EduiSelectionMaterial]);
            if (property->button_type == 1) {
                points[0] = (i32)((property->button_x + property->button_size * 0.1f) * EduiScaleX);
                points[1] = (i32)((property->button_y + property->button_size * 0.1f) * EduiScaleY);
                points[2] = (i32)((property->button_x + property->button_size * 0.9f) * EduiScaleX);
                points[3] = (i32)((property->button_y + property->button_size * 0.5) * EduiScaleY);
                points[4] = points[0];
                points[5] = (i32)((property->button_y + property->button_size * 0.9f) * EduiScaleY);
                if (edui_donotdraw == 0) {
                    NuRndrTriStrip2di(points, NULL, 3, button_colours[property->button_hover], uimtls[0]);
                }
            }
            if (property->button_type == 2) {
                points[0] = (i32)((property->button_x + property->button_size * 0.1f) * EduiScaleX);
                points[1] = (i32)((property->button_y + property->button_size * 0.45f) * EduiScaleY);
                points[2] = (i32)((property->button_x + property->button_size * 0.5) * EduiScaleX);
                points[3] = (i32)((property->button_y + property->button_size * 0.1f) * EduiScaleY);
                points[4] = (i32)((property->button_x + property->button_size * 0.9f) * EduiScaleX);
                points[5] = points[1];
                if (edui_donotdraw == 0) {
                    NuRndrTriStrip2di(points, NULL, 3, button_colours[property->button_hover], uimtls[0]);
                }
                points[0] = (i32)((property->button_x + property->button_size * 0.1f) * EduiScaleX);
                points[1] = (i32)((property->button_y + property->button_size * 0.55f) * EduiScaleY);
                points[2] = (i32)((property->button_x + property->button_size * 0.5) * EduiScaleX);
                points[3] = (i32)((property->button_y + property->button_size * 0.9f) * EduiScaleY);
                points[4] = (i32)((property->button_x + property->button_size * 0.9f) * EduiScaleX);
                points[5] = points[1];
                if (edui_donotdraw == 0) {
                    NuRndrTriStrip2di(points, NULL, 3, button_colours[property->button_hover], uimtls[0]);
                }
            }
        }
        return (i32)row_height;
    }

    f32 EduiPropLabelWidth = 50.0f;

    eduiitem_s *eduiItemPropCreateEx(u32 data, void *colours, void *selected, void *changed, void *button, i32 button_type, char *text,
                                     char *value, i32 extra_data) {
        edui_prop_s *item = (edui_prop_s *)NuAlloc(sizeof(edui_prop_s), "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x1327);
        if (item) {
            memset(item, 0, sizeof(edui_prop_s));
            item->base.data = data;
            item->base.type = EDUIITEM_PROP;
            memcpy(item->base.colours, colours, sizeof(item->base.colours));
            item->base.highlighted = 0;
            item->base.text_alignment = EDUI_ALIGN_CENTRE;
            item->base.selection_group = 0;
            item->base.process = eduicbProcessProp;
            item->base.render = eduicbRenderProp;
            item->base.destroy = eduicbItemDestroyProp;
            item->interact = eduicbInteractProp;
            eduiItemSetText(&item->base, text);
            eduiItemPropSetText(item, value);
            item->label_width = EduiPropLabelWidth;
            item->button_type = button_type;
            item->selected = (void (*)(eduimenu_s *, eduiitem_s *, u32))selected;
            item->changed = (void (*)(eduimenu_s *, eduiitem_s *, u32))changed;
            item->button = (void (*)(eduimenu_s *, eduiitem_s *, u32))button;
            item->extra_data = extra_data;
            return &item->base;
        }
        return NULL;
    }

    eduiitem_s *eduiItemPropCreate(u32 data, void *colours, void *selected, void *changed, void *button, i32 button_type, char *text,
                                   char *value) {
        return eduiItemPropCreateEx(data, colours, selected, changed, button, button_type, text, value, 0);
    }
}

extern "C" eduiitem_s *eduiItemSeparatorCreate(u32 data, const void *colours) {
    edui_separator_s *item =
        (edui_separator_s *)NuAlloc(sizeof(edui_separator_s), "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x1362);
    if (item != NULL) {
        memset(item, 0, sizeof(*item));
        item->base.data = data;
        item->base.type = EDUIITEM_SEPARATOR;
        memcpy(item->base.colours, colours, sizeof(item->base.colours));
        item->base.highlighted = 0;
        item->base.text_alignment = EDUI_ALIGN_CENTRE;
        item->base.selection_group = 0;
        item->base.process = eduicbProcessSeparator;
        item->base.render = eduicbRenderSeparator;
        item->base.destroy = eduicbItemDestroy;
        item->interact = NULL;
        return &item->base;
    }
    return NULL;
}

extern "C" {

    static i32 eduicbRenderExpander(eduimenu_s *menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 height) {
        EDUI_EXPANDER *expander = (EDUI_EXPANDER *)item;
        u32 button_colours[2];
        f32 item_x = (f32)x;
        f32 item_y = (f32)y;
        f32 item_width = (f32)width;
        f32 row_height;
        f32 baseline;
        f32 text_x;
        i32 depth;
        button_colours[0] = 0xff000000;
        button_colours[1] = 0xffffffff;
        item->x = x;
        item->y = y;
        expander->hover = EduiExpanderOverButton(expander);
        if (edui_donotdraw == 0) {
            NuQFntSet(edui_font);
            if (edui_donotdraw == 0) {
                NuQFntSetColour(edui_font, item->colours[item->highlighted]);
            }
        }
        row_height = NuQFntHeight(edui_font) * 1.25 * (1.0f / EduiScaleY);
        baseline = (NuQFntHeight(edui_font) * 0.125 + NuQFntBaseline(edui_font)) * (1.0f / EduiScaleY);
        if (menu != NULL && EduiMenuChainActive(menu)) {
            EduiDrawRect(item_x, item_y, item_width, row_height, item->colours[2 + item->highlighted], uimtls[ui_bgmtl]);
        }
        for (depth = 0; depth < expander->depth; depth++) {
            f32 guide_x = item_x + ((f32)depth + 0.5) * (row_height - 2.0);
            {
                f32 line_x0 = guide_x;
                f32 line_y0 = item_y;
                f32 line_x1 = guide_x;
                f32 line_y1 = (item_y + row_height);
                EduiDrawLine(line_x0, line_y0, line_x1, line_y1, 0xff000000, uimtls[0]);
            }
        }
        expander->button_size = row_height - 2.0f;
        expander->button_x = item_x + expander->depth * expander->button_size + 1.0f;
        expander->button_y = item_y + 1.0f;
        if (expander->first_child) {
            {
                f32 rect_x = expander->button_x;
                f32 rect_y = expander->button_y;
                f32 rect_width = expander->button_size;
                f32 rect_height = expander->button_size;
                EduiDrawLineRect(rect_x, rect_y, rect_width, rect_height, 0xff000000, uimtls[EduiSelectionMaterial]);
            }
            if (expander->open) {
                {
                    f32 line_x0 = (expander->button_x + 2.0f);
                    f32 line_y0 = (expander->button_y + expander->button_size * 0.5);
                    f32 line_x1 = (expander->button_x + expander->button_size - 2.0f);
                    f32 line_y1 = (expander->button_y + expander->button_size * 0.5);
                    EduiDrawLine(line_x0, line_y0, line_x1, line_y1, button_colours[expander->hover], uimtls[0]);
                }
            } else {
                {
                    f32 line_x0 = (expander->button_x + 2.0f);
                    f32 line_y0 = (expander->button_y + expander->button_size * 0.5);
                    f32 line_x1 = (expander->button_x + expander->button_size - 2.0f);
                    f32 line_y1 = (expander->button_y + expander->button_size * 0.5);
                    EduiDrawLine(line_x0, line_y0, line_x1, line_y1, button_colours[expander->hover], uimtls[0]);
                }
                {
                    f32 line_x0 = (expander->button_x + expander->button_size * 0.5);
                    f32 line_y0 = (expander->button_y + 2.0f);
                    f32 line_x1 = (expander->button_x + expander->button_size * 0.5);
                    f32 line_y1 = (expander->button_y + expander->button_size - 2.0f);
                    EduiDrawLine(line_x0, line_y0, line_x1, line_y1, button_colours[expander->hover], uimtls[0]);
                }
            }
        }
        text_x = expander->button_x + expander->button_size + 2.0f;
        eduiFntPrintClipEx(edui_font, text_x, item_y + baseline, 16, text_x, item_width - (expander->button_size - 2.0f), item->text);
        return (i32)row_height;
    }

    eduiitem_s *eduiItemExpanderCreate(u32 data, const void *colours, void *callback, char *text) {
        EDUI_EXPANDER *item =
            (EDUI_EXPANDER *)NuAlloc(sizeof(EDUI_EXPANDER), "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x1473);
        if (item != NULL) {
            memset(item, 0, sizeof(EDUI_EXPANDER));
            item->base.data = data;
            item->base.type = EDUIITEM_EXPANDER;
            memcpy(item->base.colours, colours, sizeof(item->base.colours));
            item->base.highlighted = 0;
            item->base.text_alignment = EDUI_ALIGN_CENTRE;
            item->base.selection_group = 0;
            item->base.process = eduicbProcessExpander;
            item->base.render = eduicbRenderExpander;
            item->base.destroy = eduicbItemDestroyExpander;
            item->interact = eduicbInteractExpander;
            eduiItemSetText(&item->base, text);
            item->changed = (void (*)(eduimenu_s *, eduiitem_s *, u32))callback;
            return &item->base;
        }
        return NULL;
    }

    static i32 eduicbProcessFilter(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD *pad) {
        edui_filter_s *filter = (edui_filter_s *)item;
        i32 changed = 0;
        char *query;
        eduiitem_s *candidate;
        eduiitem_s *next;
        EduiPropDragLabel(&filter->prop, menu);
        EduiPropKeyboard(&filter->prop, menu);
        EduiPropButton(&filter->prop, menu, pad);
        if (filter->prop.editing) {
            query = eduiPropTextEdit;
        } else {
            query = filter->prop.property_text;
        }
        if (*query) {
            for (candidate = menu->first; candidate != NULL; candidate = next) {
                next = candidate->next;
                if (candidate->type != EDUIITEM_FILTER && candidate->type != EDUIITEM_SEPARATOR && !NuStrIStr(candidate->text, query)) {
                    eduiMenuRemoveItem(menu, candidate);
                    candidate->previous = NULL;
                    candidate->next = filter->first_child;
                    if (filter->first_child != NULL) {
                        filter->first_child->previous = candidate;
                    }
                    filter->first_child = candidate;
                    changed = 1;
                }
            }
            for (candidate = filter->first_child; candidate != NULL; candidate = next) {
                next = candidate->next;
                if (NuStrIStr(candidate->text, query)) {
                    if (filter->first_child == candidate) {
                        filter->first_child = candidate->next;
                    }
                    if (candidate->next != NULL) {
                        candidate->next->previous = candidate->previous;
                    }
                    if (candidate->previous != NULL) {
                        candidate->previous->next = candidate->next;
                    }
                    candidate->next = NULL;
                    candidate->previous = NULL;
                    eduiMenuAddItem(menu, candidate);
                    changed = 1;
                }
            }
        } else {
            for (candidate = filter->first_child; candidate != NULL; candidate = next) {
                next = candidate->next;
                if (filter->first_child == candidate) {
                    filter->first_child = candidate->next;
                }
                if (candidate->next != NULL) {
                    candidate->next->previous = candidate->previous;
                }
                if (candidate->previous != NULL) {
                    candidate->previous->next = candidate->next;
                }
                candidate->next = NULL;
                candidate->previous = NULL;
                eduiMenuAddItem(menu, candidate);
                changed = 1;
            }
        }
        if (changed || !(filter->sorted & 1)) {
            eduiMenuSortItemsByTxt(menu);
            filter->sorted |= 1;
        }
        return 0;
    }

    static i32 eduicbRenderFilter(eduimenu_s *menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 height) {
        return eduicbRenderProp(menu, item, x, y, width, height);
    }

    eduiitem_s *eduiItemFilterCreate(u32 data, void *colours, char *text, char *value) {
        edui_filter_s *item =
            (edui_filter_s *)NuAlloc(sizeof(edui_filter_s), "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x151a);
        if (item) {
            memset(item, 0, sizeof(edui_filter_s));
            item->prop.base.data = data;
            item->prop.base.type = EDUIITEM_FILTER;
            memcpy(item->prop.base.colours, colours, sizeof(item->prop.base.colours));
            item->prop.base.highlighted = 0;
            item->prop.base.text_alignment = EDUI_ALIGN_CENTRE;
            item->prop.base.selection_group = 0;
            item->prop.base.process = eduicbProcessFilter;
            item->prop.base.render = eduicbRenderFilter;
            item->prop.base.destroy = eduicbItemDestroyFilter;
            item->prop.interact = eduicbInteractFilter;
            eduiItemSetText(&item->prop.base, text);
            eduiItemPropSetText(&item->prop, value);
            item->prop.label_width = EduiPropLabelWidth;
            return &item->prop.base;
        }
        return NULL;
    }
    struct eduimenu_s *edui_messagemenu = 0;

    void eduicbCancelMessageMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
        eduiMenuDestroy(edui_messagemenu);
        edui_messagemenu = NULL;
    }

    u32 eduimenu_outline_colour = 0x80404040;
    u32 eduimenu_selected_outline_colour = 0x80ffffff;
}

extern "C" void eduiMenuRender(eduimenu_s *menu) {
    i32 x = menu->x;
    i32 y = menu->y;
    i32 max_item_height = 0;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuQFntPushCoordinateSystem(NUQFNT_CS_SIXTEENTHS);
    NuQFntPushPrintMode(NUQFNT_MODE_2D);
    EduiQFntSet(edui_font);
    NuQFntSetScale(edui_font, edui_font_scale_x, edui_font_scale_y);
    i32 title_height = (i32)(NuQFntHeight(edui_font) * 0.15625);
    f64 font_height = NuQFntHeight(edui_font) * 0.125;
    i32 baseline = (i32)(NuQFntBaseline(edui_font) + font_height);
    if (menu->title != NULL && menu->title[0] != 0) {
        if (menu->child == NULL) {
            EduiAddInteraction((f32)x, (f32)y, (f32)menu->width, (f32)title_height, menu, NULL, cbInteractMenuTitle, 0x40, 0);
        }
        EduiRndrRect2di(x << 4, y << 3, menu->width << 4, title_height << 3, 0x80000000, uimtls[ui_bgmtl]);
        EduiQFntSet(edui_font);
        EduiQFntSetColour(edui_font, 0x807f7f7f);
        EduiPrintClippedText(edui_font, (x + 1) << 4, (y << 3) + baseline, 16, x << 4, menu->width << 4, menu->title);
        y += title_height;
    }
    eduiitem_s *item = menu->first_visible;
    if (item != NULL) {
        i32 find_selected = menu->selected != NULL;
        if (item != menu->first) {
            i32 scroll_height = (i32)(NuQFntHeight(edui_font) * 0.125);
            if (menu->child == NULL) {
                EduiAddInteraction((f32)x, (f32)y, (f32)menu->width, (f32)scroll_height, menu, NULL, cbInteractMenuScrollUp, 0x40, 0);
            }
            if (edui_donotdraw == 0) {
                NuRndrRect2di(x << 4, y << 3, menu->width << 4, scroll_height << 3, 0x80000000, uimtls[ui_bgmtl]);
                if (edui_donotdraw == 0) {
                    i32 centre = (x * 2 + menu->width) << 3;
                    i32 top = (y << 3) + 8;
                    i32 bottom = ((y + scroll_height) << 3) - 16;
                    NuRndrLine2di(centre, top, centre - ((scroll_height - 2) << 4), bottom, 0x80ffffff, uimtls[0]);
                    if (edui_donotdraw == 0) {
                        centre = (x * 2 + menu->width) << 3;
                        NuRndrLine2di(centre, top, centre + ((scroll_height - 2) << 4), bottom, 0x80ffffff, uimtls[0]);
                    }
                }
            }
            y += scroll_height;
        }
        while (item != NULL) {
            menu->last_visible = item;
            if (item == menu->selected) {
                find_selected = 0;
            }
            i32 item_height = EduiRenderItem(item, menu, x, y, menu->width, item == menu->selected && menu->child == NULL);
            if (item_height > max_item_height) {
                max_item_height = item_height;
            }
            if (menu->child == NULL) {
                EduiAddInteraction((f32)x, (f32)y, (f32)menu->width, (f32)item_height, menu, item, ((edui_sel_s *)item)->interact, 0x40, 0);
            }
            item = item->next;
            if (menu->flags & EDUI_MENU_HORIZONTAL) {
                x += menu->width;
            } else {
                y += item_height;
            }
            if (item == NULL) {
                break;
            }
            if (menu->y + menu->height > y) {
                continue;
            }
            i32 scroll_height = (i32)(NuQFntHeight(edui_font) * 0.125);
            if (menu->child == NULL) {
                EduiAddInteraction((f32)x, (f32)y, (f32)menu->width, (f32)scroll_height, menu, NULL, cbInteractMenuScrollDown, 0x40, 0);
            }
            if (edui_donotdraw == 0) {
                NuRndrRect2di(x << 4, y << 3, menu->width << 4, scroll_height << 3, 0x80000000, uimtls[ui_bgmtl]);
                if (edui_donotdraw == 0) {
                    i32 bottom = (y + scroll_height - 2) << 3;
                    i32 top = (y << 3) + 8;
                    i32 centre = (x * 2 + menu->width) << 3;
                    NuRndrLine2di(centre, bottom, centre - ((scroll_height - 2) << 4), top, 0x80ffffff, uimtls[0]);
                    if (edui_donotdraw == 0) {
                        centre = (x * 2 + menu->width) << 3;
                        NuRndrLine2di(centre, bottom, centre + ((scroll_height - 2) << 4), top, 0x80ffffff, uimtls[0]);
                    }
                }
            }
            break;
        }
        if (find_selected && menu->first_visible->next != NULL) {
            menu->first_visible = menu->first_visible->next;
        }
    }
    if ((menu->flags & EDUI_MENU_HORIZONTAL) == 0) {
        x = menu->x + menu->width;
    } else {
        y += max_item_height;
    }
    menu->drawn_width = x - menu->x;
    menu->drawn_height = y - menu->y;
    if (active_menu == menu) {
        EduiRndrLineRect2di((menu->x - 1) << 4, (menu->y << 3) - 8, (menu->drawn_width + 2) << 4, (menu->drawn_height << 3) + 16,
                            eduimenu_selected_outline_colour, uimtls[EduiSelectionMaterial]);
    } else {
        EduiRndrLineRect2di((menu->x - 1) << 4, (menu->y << 3) - 8, (menu->drawn_width + 2) << 4, (menu->drawn_height << 3) + 16,
                            eduimenu_outline_colour, uimtls[EduiSelectionMaterial]);
    }
    if (menu->child != NULL) {
        EduiAddInteraction((f32)menu->x, (f32)menu->y, (f32)menu->drawn_width, (f32)menu->drawn_height, menu, NULL,
                           cbInteractMenuCancelChild, 0x40, 1);
        eduiMenuRender(menu->child);
    }
    NuQFntPopPrintMode();
    NuQFntPopCoordinateSystem();
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}

extern "C" {

    void eduiSetActiveMenu(eduimenu_s *menu) {
        if (EduiUsingMenuFocus != 0) {
            if (menu == NULL) {
                active_menu = EduiDeepestChild(default_active_menu);
            } else {
                active_menu = EduiDeepestChild(menu);
            }
        }
    }

    eduimenu_s *eduiClearActiveMenu(void) {
        if (EduiUsingMenuFocus != 0) {
            active_menu = EduiDeepestChild(default_active_menu);
        }
        return NULL;
    }

    // noinline: the original calls cbInteractMenuKeySelect out of line from eduiMenuProcessInput; VC8 would inline it
    // there.
    __declspec(noinline) void cbInteractMenuKeySelect(eduimenu_s *menu) {
        u32 modifiers;
        char text[2];
        i32 key = NuKeyGet(&modifiers);
        if (key > -1) {
            text[0] = NuKeyToAscii(key, modifiers & 1);
            text[1] = 0;
            cbInteractMenuScrollTo(menu, text);
        }
    }

    static i32 EduiProcessMenuInteract(eduimenu_s *menu, edui_interact_s *interact) {
        i32 result = 0;
        if (interact->menu == menu) {
            if (EduiInteractPressed(interact) && EduiInteractionContainsCursor(interact)) {
                result = 1;
                EduiInteractSelectItem(interact);
                if (interact->needs_debounce != 0) {
                    if ((interact->buttons & edui_cursor_buttons_db) == 0) {
                        return result;
                    }
                } else {
                    if ((interact->buttons & edui_cursor_buttons) == 0) {
                        return result;
                    }
                    if (menu != active_menu) {
                        eduiSetActiveMenu(menu);
                    }
                }
                if (interact->callback != NULL && interact->callback(interact)) {
                    eduiInteractLocked = interact;
                }
            }
        }
        return result;
    }

    i32 eduiProcessInteracts(eduimenu_s *menu, FRAME_PAD *pad) {
        i32 result = 0;
        i32 index;
        edui_interact_s *interact;
        if (edmain_cursor_enabled) {
            if (eduiInteractLocked != NULL) {
                if ((eduiInteractLocked->menu == NULL || eduiInteractLocked->item == NULL) && eduiInteractLocked->needs_debounce) {
                    eduiInteractLocked = NULL;
                }
            }
            if (eduiInteractLocked != NULL) {
                result = EduiProcessLockedInteraction(menu);
            } else {
                interact = &eduiInteracts[numInteracts - 1];
                for (index = 0; index < numInteracts; index++, interact--) {
                    if (EduiProcessMenuInteract(menu, interact)) {
                        return 1;
                    }
                }
            }
        }
        return result;
    }

    /* A stick nudge recentres the cursor, otherwise it follows the mouse; holding
     * key 0x38 locks it, parking the OS cursor where the editor cursor is. */
    void eduiProcessCursorDefault(f32 delta_time, FRAME_PAD *pad) {
        u8 centred = 0;
        if (pad != NULL && eduiUsedAlgPad(pad) && !(pad->digital_buttons & PAD_BUTTON_CROSS)) {
            centred = 1;
            edui_cursor_x = 320.0f;
            edui_cursor_y = 112.0f;
        } else {
            edui_cursor_dx = -NuMouseReadXVel() * 0.1f;
            edui_cursor_dy = -NuMouseReadYVel() * 0.1f;
        }
        if (NuKeyboard(0x38)) {
            if (edui_cursor_locked == 0) {
                PcEditorCursorEnable(0);
                edui_cursor_locked = 1;
            }
        } else if (edui_cursor_locked == 1) {
            PcEditorCursorEnable(1);
            edui_cursor_locked = 0;
        }
        if (edui_cursor_locked) {
            PcSetMousePosition(edui_cursor_x / 640.0, edui_cursor_y / 224.0);
        } else if (!centred) {
            PcGetMousePosition(&edui_cursor_x, &edui_cursor_y);
            edui_cursor_x *= 640.0;
            edui_cursor_y *= 224.0;
        }
        EduiClampCursor();
        edui_cursor_buttons_old = edui_cursor_buttons;
        edui_cursor_buttons = 0;
        edui_cursor_buttons |= NuMouseReadButtons() == 1 ? 0x40 : 0;
        edui_cursor_buttons |= NuMouseReadButtons() == 2 ? 0x10 : 0;
        edui_cursor_buttons_db = (edui_cursor_buttons ^ edui_cursor_buttons_old) & edui_cursor_buttons;
    }

    void EduiUpdateCursor(f32 delta_time, FRAME_PAD *pad) {
        if (pad == NULL || !eduiUsedAlgPad(pad) || (pad->digital_buttons & PAD_BUTTON_CROSS)) {
            edui_cursor_dx = -NuMouseReadXVel() * TerrainScanMargin10;
            edui_cursor_dy = -NuMouseReadYVel() * TerrainScanMargin10;
        } else {
            edui_cursor_x = 320.0f;
            edui_cursor_y = 112.0f;
        }
        if (NuKeyboard(0x38)) {
            edui_cursor_locked = 1;
        } else {
            edui_cursor_locked = 0;
            edui_cursor_x += edui_cursor_dx;
            edui_cursor_y += edui_cursor_dy;
        }
        EduiClampCursor();
        edui_cursor_buttons_old = edui_cursor_buttons;
        edui_cursor_buttons = 0;
        edui_cursor_buttons |= NuMouseReadButtons() == 1 ? 0x40 : 0;
        edui_cursor_buttons |= NuMouseReadButtons() == 2 ? 0x10 : 0;
        edui_cursor_buttons_db = (edui_cursor_buttons ^ edui_cursor_buttons_old) & edui_cursor_buttons;
    }

    void eduiProcessCursor(f32 delta_time, FRAME_PAD *pad) {
        eduiProcessCursorDefault(delta_time, pad);
    }

    eduiitem_s *eduiItemColourSliderCreate(u32 data, void *colours, i32 group, void *callback, f32 minimum, f32 range, f32 value, u8 red,
                                           u8 green, u8 blue, char *text) {
        edui_colour_slider_s *item = (edui_colour_slider_s *)NuAlloc(sizeof(edui_colour_slider_s),
                                                                     "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x1a53);
        if (item) {
            memset(item, 0, sizeof(edui_colour_slider_s));
            item->base.base.data = data;
            item->base.base.type = EDUIITEM_COLOUR_SLIDER;
            memcpy(item->base.base.colours, colours, sizeof(item->base.base.colours));
            item->base.base.text_alignment = EDUI_ALIGN_CENTRE;
            item->base.base.selection_group = group;
            item->base.base.process = eduicbProcessColourSlider;
            item->base.base.render = eduicbRenderColourSlider;
            item->base.base.destroy = eduicbItemSliderDestroy;
            eduiItemSetText(&item->base.base, text);
            item->base.minimum = minimum;
            item->base.range = range;
            item->base.granularity = 1.0f;
            item->base.changed = (void (*)(eduimenu_s *, eduiitem_s *, u32))callback;
            item->base.format = NULL;
            item->red = red;
            item->green = green;
            item->blue = blue;
            eduiItemSliderSetFmt(&item->base, "(%d)");
            eduiItemSliderSetVal(&item->base, value);
            return &item->base.base;
        }
        return NULL;
    }

    eduiitem_s *eduiItemFileSelCreate(u32 data, void *colours, void *callback, char *text) {
        EDUI_FILE_SELECT *item =
            (EDUI_FILE_SELECT *)NuAlloc(sizeof(EDUI_FILE_SELECT), "d:\\projects\\game_saga\\gameapi.saga\\edtools\\fileselect.c", 0x54a);
        if (item) {
            memset(item, 0, sizeof(EDUI_FILE_SELECT));
            item->base.data = data;
            item->base.type = EDUIITEM_FILE_SEL;
            memcpy(item->base.colours, colours, sizeof(item->base.colours));
            item->base.text_alignment = EDUI_ALIGN_CENTRE;
            item->base.selection_group = 0;
            item->base.process = eduicbProcessFileSel;
            item->base.render = eduicbRenderFileSel;
            item->base.destroy = eduicbItemDestroy;
            eduiItemSetText(&item->base, text);
            item->cursor_x = 0.5f;
            item->cursor_y = 0.5f;
            item->hue = 180.0f;
            item->value = 0.5f;
            item->saturation = 1.0f;
            item->changed = (void (*)(eduimenu_s *, eduiitem_s *, u32))callback;
            return &item->base;
        }
        return NULL;
    }
    static struct eduimenu_s *ed_cfg_menu = 0;
    i32 ed_init = 0;

    void edmainClose(void) {
        if (ed_init != 0) {
            NuCameraDestroy(edinternalcam);
            while (ed_list != NULL) {
                void (*close)(void) = ed_list->close;
                if (close != NULL) {
                    close();
                }
                ed_list = ed_list->next;
            }
            if (ed_main_menu != NULL) {
                eduiMenuDestroy(ed_main_menu);
                ed_main_menu = NULL;
            }
            if (ed_cfg_menu != NULL) {
                eduiMenuDestroy(ed_cfg_menu);
                ed_cfg_menu = NULL;
            }
            ed_list = NULL;
            ed_curr = NULL;
            ed_init = 0;
        }
    }

    static f32 edmain_menu_scale = 1.0f;

    void edmainRender(void) {
        if (ed_main_menu != NULL) {
            NuQFntPushPrintMode(NUQFNT_MODE_2D);
            NuQFntPushCoordinateSystem(NUQFNT_CS_SIXTEENTHS);
            NuQFntSet(system_qfont);
            NuQFntSetScale(system_qfont, edmain_menu_scale, edmain_menu_scale);
            eduiMenuRender(ed_main_menu);
            NuQFntPopCoordinateSystem();
            NuQFntPopPrintMode();
        }
    }

    f32 edmainSetMenuScale(f32 scale) {
        f32 previous = edmain_menu_scale;
        edmain_menu_scale = scale;
        if (ed_main_menu != NULL) {
            eduiMenuFitWidth(ed_main_menu, 8);
        }
        return previous;
    }

    void eduiMenuFitOnScreen(eduimenu_s *menu, i32 padding) {
        if (menu->drawn_width < 0) {
            i32 saved = edui_donotdraw;
            edui_donotdraw = 1;
            eduiMenuRender(menu);
            edui_donotdraw = saved;
        }
        if (menu->x + menu->drawn_width > 640 - padding) {
            menu->x -= menu->drawn_width;
        }
        if (menu->x < padding) {
            menu->x = padding;
        }
        if (menu->y + menu->drawn_height > 448 - padding) {
            menu->y -= menu->drawn_height;
        }
        if (menu->y < padding) {
            menu->y = padding;
        }
    }

    i32 eduiMenuAttach(eduimenu_s *menu, eduimenu_s *child) {
        if (child->parent == NULL) {
            eduimenu_s *parent = menu;
            while (parent->child != NULL) {
                parent = parent->child;
            }
            child->parent = parent;
            parent->child = child;
            eduiSetActiveMenu(child);
            return 1;
        }
        return 0;
    }

    i32 eduiMenuDetach(eduimenu_s *menu) {
        if (menu->parent != NULL) {
            if (menu->child != NULL) {
                eduiMenuDetach(menu->child);
            }
            if (menu == active_menu) {
                active_menu = menu->parent;
            }
            menu->parent->child = NULL;
            menu->parent = NULL;
            return 1;
        }
        eduiSetActiveMenu(NULL);
        return 0;
    }

    eduiitem_s *eduiItemSelCreate(u32 data, void *colours, i32 selected, i32 group, void *callback, char *text) {
        edui_sel_s *item = (edui_sel_s *)NuAlloc(sizeof(edui_sel_s), "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x53b);
        if (item) {
            memset(item, 0, sizeof(edui_sel_s));
            item->base.data = data;
            item->base.type = EDUIITEM_SEL;
            memcpy(item->base.colours, colours, sizeof(item->base.colours));
            item->base.selection_group = group;
            item->base.highlighted = selected;
            item->base.text_alignment = EDUI_ALIGN_CENTRE;
            item->base.process = eduicbProcessSel;
            item->base.render = eduicbRenderSel;
            item->base.destroy = eduicbItemDestroy;
            item->interact = eduicbInteractSel;
            item->base.text = NULL;
            eduiItemSetText(&item->base, text ? text : (char *)"<no name>");
            item->selected = (void (*)(eduimenu_s *, eduiitem_s *, u32))callback;
            item->held = NULL;
            return &item->base;
        }
        return NULL;
    }

    eduiitem_s *eduiItemSelWithClipColourCreate(u32 data, void *colours, i32 selected, i32 group, void *callback, char *text) {
        eduiitem_s *item = eduiItemSelCreate(data, colours, selected, group, callback, text);
        if (item) {
            item->render = eduicbRenderSelWithClipColour;
        }
        return item;
    }

    eduiitem_s *eduiItemCheckCreate(size_t data, const void *colours, i32 selected, i32 group, EdUiItemCallback callback, char *text) {
        eduiitem_s *item = eduiItemSelCreate(data, (void *)colours, selected, group, callback, text);
        if (item) {
            item->type = EDUIITEM_CHECK;
            item->process = eduicbProcessSel;
            item->render = eduicbRenderCheck;
        }
        return item;
    }

    eduiitem_s *eduiItemToggleCreate(size_t data, const void *colours, i32 selected, i32 group, EdUiItemCallback callback, char *text) {
        eduiitem_s *item = eduiItemSelCreate(data, (void *)colours, selected, group, callback, text);
        if (item) {
            item->type = EDUIITEM_TOGGLE;
            item->process = eduicbProcessSel;
            item->render = eduicbRenderCheck;
        }
        return item;
    }

    eduiitem_s *eduiItemTextPickCreate(u32 data, void *colours, void *callback, char *text) {
        edui_textpicker_s *item =
            (edui_textpicker_s *)NuAlloc(sizeof(edui_textpicker_s), "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x63d);
        if (item) {
            memset(item, 0, sizeof(edui_textpicker_s));
            item->base.data = data;
            item->base.type = EDUIITEM_TEXT_PICK;
            memcpy(item->base.colours, colours, sizeof(item->base.colours));
            item->base.text_alignment = EDUI_ALIGN_CENTRE;
            item->base.selection_group = 0;
            item->base.process = eduicbProcessTextPick;
            item->base.render = eduicbRenderTextPick;
            item->base.destroy = eduicbItemTextPickDestroy;
            eduiItemSetText(&item->base, text);
            item->max_length = 255;
            item->callback = (void (*)(eduimenu_s *, eduiitem_s *, u32))callback;
            item->format = NULL;
            item->cursor = 0;
            eduiItemTextPickSetFmt(item, "\"%s\"");
            memcpy(textrow, edui_textrow_default, sizeof(textrow));
            return &item->base;
        }
        return NULL;
    }

    static void tpentOnEnter(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
        edui_textpicker_s *source = (edui_textpicker_s *)item->data_ptr;
        NuStrCpy(source->value, ((edui_textpicker_s *)item)->value);
        if (source->callback) {
            source->callback(menu->parent, &source->base, buttons);
        }
        eduiMenuDetach(menu);
        eduiMenuDestroy(menu);
    }

    static void tpentPropOnEnter(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
        edui_prop_s *property = (edui_prop_s *)item->data_ptr;
        eduimenu_s *parent;
        NuStrCpy(property->property_text, ((edui_textpicker_s *)item)->value);
        if (property->changed) {
            property->changed(menu->parent, &property->base, buttons);
        }
        parent = menu->parent;
        eduiMenuDetach(menu);
        eduiMenuDestroy(menu);
        if (parent) {
            eduiSetActiveMenu(parent);
        }
    }

    static eduiiattr_s ed_attr = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};

    edui_textpicker_s *eduiAddTextPickEntEx(eduimenu_s *menu, eduiitem_s *source, EdUiItemCallback callback) {
        struct edui_textpicker_s *picker;
        static eduimenu_s *tpent = 0;
        tpent = eduiMenuCreate(source->x + 10, source->y + 10, 180, 50, menu->font, NULL, ((char *)("Enter Text")));
        tpent->flags |= EDUI_MENU_HORIZONTAL;
        picker = ((edui_textpicker_s *)(eduiItemTextPickCreate(((size_t)(source)), &ed_attr, callback, ((char *)("")))));
        eduiMenuAddItem(tpent, &picker->base);
        eduiMenuAttach(menu, tpent);
        picker->editing = 1;
        eduiMenuFitWidth(tpent, 8);
        picker->cursor = NuStrLen(picker->value);
        return picker;
    }

    /* Opens an entry menu that edits a copy of the picker's text. */
    void eduiAddTextPickEnt(eduimenu_s *menu, eduiitem_s *item) {
        edui_textpicker_s *source = (edui_textpicker_s *)item;
        edui_textpicker_s *picker = eduiAddTextPickEntEx(menu, item, tpentOnEnter);
        if (picker) {
            NuStrCpy(picker->value, source->value);
            picker->cursor = NuStrLen(picker->value);
            memcpy(picker->base.colours, item->colours, sizeof(picker->base.colours));
            picker->max_length = source->max_length;
            picker->extended_keyboard = source->extended_keyboard;
        }
    }

    /* Opens an entry menu that edits a copy of the property's text. */
    void eduiAddPropTextPickEnt(eduimenu_s *menu, eduiitem_s *item) {
        edui_prop_s *source = (edui_prop_s *)item;
        edui_textpicker_s *picker = eduiAddTextPickEntEx(menu, item, tpentPropOnEnter);
        if (picker) {
            NuStrCpy(picker->value, source->property_text);
            picker->cursor = NuStrLen(picker->value);
            memcpy(picker->base.colours, item->colours, sizeof(picker->base.colours));
        }
    }

    static i32 EduiTextPickPad(edui_textpicker_s *picker, eduimenu_s *menu, FRAME_PAD *pad) {
        if (picker == NULL || !picker->editing) {
            if (pad->digital_buttons_pressed & PAD_BUTTON_CROSS) {
                eduiAddTextPickEnt(menu, &picker->base);
            }
            return 0;
        }
        if (pad->digital_buttons_pressed & PAD_BUTTON_DOWN) {
            EduiTextPickNextRow(picker);
        }
        if (pad->digital_buttons_pressed & PAD_BUTTON_UP) {
            EduiTextPickPreviousRow(picker);
        }
        if (pad->digital_buttons_pressed & PAD_BUTTON_LEFT) {
            EduiTextPickPreviousColumn(picker);
        }
        if (pad->digital_buttons_pressed & PAD_BUTTON_RIGHT) {
            EduiTextPickNextColumn(picker);
        }
        if ((pad->digital_buttons_pressed & PAD_BUTTON_TRIANGLE) && picker->callback) {
            picker->callback(menu, &picker->base, pad->digital_buttons);
        }
        if ((pad->digital_buttons & (PAD_BUTTON_L1 | PAD_BUTTON_R1)) == 0xc) {
            EduiTextPickPaste(picker);
        }
        if ((pad->digital_buttons & (PAD_BUTTON_L2 | PAD_BUTTON_R2)) == 3) {
            EduiTextPickCopy(picker);
        }
        if (pad->digital_buttons_pressed & PAD_BUTTON_L1) {
            EduiTextPickCursorLeft(picker);
        }
        if (pad->digital_buttons_pressed & PAD_BUTTON_R1) {
            EduiTextPickCursorRight(picker);
        }
        if (pad->digital_buttons_pressed & PAD_BUTTON_CROSS) {
            EduiTextPickInsert(picker, textrow[picker->keyboard_row][picker->keyboard_column]);
        }
        if (pad->digital_buttons_pressed & PAD_BUTTON_SQUARE) {
            EduiTextPickDelete(picker);
        }
        if (pad->digital_buttons_pressed & PAD_BUTTON_CIRCLE) {
            EduiTextPickToggleCase();
        }
        return 1;
    }

    i32 eduicbProcessTextPick(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD *pad) {
        EduiTextPickKeyboard((edui_textpicker_s *)item, menu, pad);
        EduiTextPickPad((edui_textpicker_s *)item, menu, pad);
        return 0;
    }

    static void eduicbDestroyDirectoryList(eduimenu_s *menu, eduimenu_s *parent) {
        eduiMenuDetach(menu);
        eduiMenuDestroy(menu);
    }

    static void eduicbSelectDirectoryExit(eduimenu_s *menu, eduiitem_s *item, u32 value) {
        eduiMenuDetach(menu);
        eduiMenuDestroy(menu);
    }

    /* File picker directory menu: the picker and menu it was opened for. */
    static eduimenu_s *filepick_menu = 0;
    static edui_file_pick_s *filepick_item = 0;

    /* Steps into a directory (or up for the "<..>" entry 9999), or picks a file. */
    static void eduicbSelectDirectoryEntry(eduimenu_s *menu, eduiitem_s *item, u32 value) {
        if (dir_list == NULL) {
            return;
        }
        if (item->data == 9999) {
            char *separator = NuStrRChr(filepick_item->filename, '/');
            if (separator) {
                *separator = '\0';
            } else {
                filepick_item->filename[0] = '\0';
            }
            filepick_item->reopen_directory = 1;
        }
        if (dir_list[item->data].flags & FILE_DIRECTORY_FLAG_DIRECTORY) {
            if (filepick_item->filename[0]) {
                NuStrCat(filepick_item->filename, "/");
            }
            NuStrCat(filepick_item->filename, dir_list[item->data].name);
            filepick_item->reopen_directory = 1;
        }
        if (!(dir_list[item->data].flags & FILE_DIRECTORY_FLAG_DIRECTORY)) {
            NuStrCpy(filepick_item->name, dir_list[item->data].name);
            if (filepick_item->changed) {
                filepick_item->changed(filepick_menu, &filepick_item->base, value);
            }
        }
        eduiMenuDetach(menu);
        eduiMenuDestroy(menu);
    }

    static i32 dir_size = 0;

    /* Lists the picker's directory (filtered by its filter text) in a menu. */
    static void eduicbAttachDirectoryList(eduimenu_s *menu, edui_file_pick_s *picker) {
        eduimenu_s *directory_menu;
        eduiitem_s *item;
        i32 directory;
        i32 index;
        char text[1024];
        if (dir_list == NULL) {
            return;
        }
        picker->reopen_directory = 0;
        filepick_menu = menu;
        filepick_item = picker;
        directory_menu = eduiMenuCreate(menu->x + 20, menu->y + 20, 250, 300, menu->font, eduicbDestroyDirectoryList, "Directory");
        if (directory_menu == NULL) {
            return;
        }
        NuSPrintf(text, "%s%s", picker->directory, picker->filename);
        directory = NuFileOpenDir(text);
        if (directory) {
            dir_size = 0;
            while (NuFileReadDir(directory, &dir_list[dir_size]) > 0 && dir_size < edui_filepick_listsize) {
                dir_size++;
            }
            if (picker->compare_entries) {
                qsort(dir_list, dir_size, sizeof(FILE_DIRECTORY_ENTRY), (i32(*)(const void *, const void *))picker->compare_entries);
            }
            if (picker->filename[0]) {
                item = eduiItemSelCreate(9999, picker->base.colours, 0, 1, eduicbSelectDirectoryEntry, "<..>");
                item->text_alignment = EDUI_ALIGN_LEFT;
                eduiMenuAddItem(directory_menu, item);
            }
            for (index = 0; index < dir_size; index++) {
                if (dir_list[index].flags & FILE_DIRECTORY_FLAG_DIRECTORY) {
                    NuSPrintf(text, "<%s>", dir_list[index].name);
                    item = eduiItemSelCreate(index, picker->base.colours, 0, 1, eduicbSelectDirectoryEntry, text);
                    item->text_alignment = EDUI_ALIGN_LEFT;
                    eduiMenuAddItem(directory_menu, item);
                }
            }
            for (index = 0; index < dir_size; index++) {
                if (picker->filter[0] && !NuStrIStr(dir_list[index].name, picker->filter)) {
                    continue;
                }
                if (dir_list[index].flags & FILE_DIRECTORY_FLAG_DIRECTORY) {
                    continue;
                }
                NuSPrintf(text, "%s  ( size=%d  date=%d/%d/%d %d:%d )", dir_list[index].name, dir_list[index].size,
                          dir_list[index].modified.day, dir_list[index].modified.month, dir_list[index].modified.year,
                          dir_list[index].modified.hour, dir_list[index].modified.minute);
                item = eduiItemSelCreate(index, picker->base.colours, 0, 1, eduicbSelectDirectoryEntry, text);
                item->text_alignment = EDUI_ALIGN_LEFT;
                eduiMenuAddItem(directory_menu, item);
            }
            if (directory_menu->first == NULL) {
                eduiMenuAddItem(directory_menu,
                                eduiItemSelCreate(dir_size, picker->base.colours, 0, 1, eduicbSelectDirectoryExit, "No Files"));
            }
            NuFileCloseDir(directory);
        }
        eduiMenuFitWidth(directory_menu, 8);
        eduiMenuAttach(menu, directory_menu);
    }

    i32 eduicbProcessFilePick(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD *pad) {
        edui_file_pick_s *picker = (edui_file_pick_s *)item;
        if ((pad->digital_buttons_pressed & PAD_BUTTON_CROSS) || picker->reopen_directory) {
            eduiMenuHighlight(menu, item);
            eduicbAttachDirectoryList(menu, picker);
        }
        return 0;
    }

    eduiitem_s *eduiItemFilePickCreate(u32 data, void *colours, void *callback, char *text) {
        edui_file_pick_s *item =
            (edui_file_pick_s *)NuAlloc(sizeof(edui_file_pick_s), "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0x950);
        if (item) {
            memset(item, 0, sizeof(edui_file_pick_s));
            item->base.data = data;
            item->base.type = EDUIITEM_TEXT_PICK;
            memcpy(item->base.colours, colours, sizeof(item->base.colours));
            item->base.text_alignment = EDUI_ALIGN_CENTRE;
            item->base.selection_group = 0;
            item->base.process = eduicbProcessFilePick;
            item->base.render = eduicbRenderFilePick;
            item->base.destroy = eduicbItemFilePickDestroy;
            eduiItemSetText(&item->base, text);
            item->changed = (void (*)(eduimenu_s *, eduiitem_s *, u32))callback;
            item->format = NULL;
            item->name[0] = 0;
            item->directory[0] = 0;
            item->filename[0] = 0;
            item->compare_entries = NULL;
            eduiItemFilePickSetFmt(item, "\"%s\"");
            return &item->base;
        }
        return NULL;
    }

    void cbgpcfgAdd(eduimenu_s *menu, eduiitem_s *item, u32 value) {
        edui_gradient_pick_s *picker = (edui_gradient_pick_s *)item->data_ptr;
        f32 time;
        f32 hue;
        f32 saturation;
        f32 intensity;
        if (picker->selected_stage == NULL) {
            time = 0.0f;
            hue = 0.0f;
            saturation = 0.0f;
            intensity = 1.0f;
        } else {
            if (picker->selected_stage->next != NULL) {
                time = (picker->selected_stage->time + picker->selected_stage->next->time) * 0.5;
            } else {
                time = (picker->selected_stage->time + 1.0) * 0.5;
            }
            hue = picker->selected_stage->hue;
            saturation = picker->selected_stage->saturation;
            intensity = picker->selected_stage->value;
        }
        picker->selected_stage = eduiGradStageAdd(picker, time, hue, saturation, intensity);
        if (picker->add != NULL) {
            picker->add(menu, item, value);
        }
        eduiMenuDetach(menu);
        eduiMenuDestroy(menu);
    }
}

extern "C" void cbgpcfgDel(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    edui_gradient_pick_s *picker = (edui_gradient_pick_s *)item->data_ptr;
    if (picker->remove != NULL) {
        picker->remove(menu, item, value);
    }
    if (picker->selected_stage != NULL) {
        eduiGradStageDelete(picker, picker->selected_stage);
    }
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

extern "C" void cbgpcfgCopy(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    edui_gradient_pick_s *picker = (edui_gradient_pick_s *)item->data_ptr;
    edui_gradient_node_s *stage = picker->selected_stage;
    if (stage != NULL) {
        i32 type = item->type;
        if (type == EDUIITEM_GRAD_PICK) {
            EduiColourClipboard.hue = stage->hue;
            EduiColourClipboard.saturation = stage->saturation;
            EduiColourClipboard.value = stage->value;
        }
        if (type == EDUIITEM_GREY_GRAD_PICK) {
            EduiColourClipboard.gray = stage->value;
        }
        if (picker->copy != NULL) {
            picker->copy(menu, item, value);
        }
    }
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

extern "C" void cbgpcfgPaste(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    edui_gradient_pick_s *picker = (edui_gradient_pick_s *)item->data_ptr;
    edui_gradient_node_s *stage = picker->selected_stage;
    if (stage != NULL) {
        if (item->type == EDUIITEM_GRAD_PICK) {
            eduiGradStageSetHSV(stage, EduiColourClipboard.hue, EduiColourClipboard.saturation, EduiColourClipboard.value);
        }
        if (item->type == EDUIITEM_GREY_GRAD_PICK) {
            eduiGradStageSetHSV(picker->selected_stage, 0.0f, 0.0f, EduiColourClipboard.gray);
        }
        if (picker->paste != NULL) {
            picker->paste(menu, item, value);
        }
    }
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

extern "C" void cbgpcfgCPPress(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    edui_gradient_pick_s *picker = (edui_gradient_pick_s *)item->data_ptr;
    if (picker->selected_stage != NULL) {
        if (item->type == EDUIITEM_COLOUR_PICK || item->type == EDUIITEM_GREY_PICK) {
            edui_colour_pick_s *colour = (edui_colour_pick_s *)item;
            eduiGradStageSetHSV(picker->selected_stage, colour->hue, colour->saturation, colour->value);
        }
        if (picker->press != NULL) {
            picker->press(menu, item, value);
        }
    }
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

extern "C" {

    static void eduicbProcessGradPick(eduimenu_s *menu, eduiitem_s *item) {
        edui_gradient_pick_s *gradient = (edui_gradient_pick_s *)item;
        eduiitem_s *pick;
        static eduimenu_s *gpcfg = 0;
        gpcfg = eduiMenuCreate(menu->x + 10, menu->y + 40, 180, 250, menu->font, eduicbPickCFGCancel, "GradPick Adjust");
        if (!gpcfg) {
            return;
        }
        switch (item->type) {
            case EDUIITEM_GREY_GRAD_PICK:
                pick = eduiItemColourPickCreate((u32)item, &ed_attr, cbgpcfgCPPress, "Colourpick");
                if (pick) {
                    pick->type = EDUIITEM_GREY_PICK;
                    pick->process = eduicbProcessGreyPick;
                    pick->render = eduicbRenderGreyPick;
                }
                if (gradient->selected_stage) {
                    f32 hue = gradient->selected_stage->hue;
                    f32 saturation = gradient->selected_stage->saturation;
                    f32 value = gradient->selected_stage->value;
                    edui_colour_pick_s *colour = (edui_colour_pick_s *)pick;
                    colour->hue = hue;
                    colour->saturation = saturation;
                    colour->value = value;
                    colour->cursor_x = (f32)(hue / 360.0);
                    colour->cursor_y = value;
                }
                break;
            case EDUIITEM_DATA_GRAD_PICK:
                pick = eduiItemSelCreate((u32)item, &ed_attr, 0, 0, cbgpcfgCPPress, "Edit");
                break;
            default:
                pick = eduiItemColourPickCreate((u32)item, &ed_attr, cbgpcfgCPPress, "Colourpick");
                if (gradient->selected_stage) {
                    f32 hue = gradient->selected_stage->hue;
                    f32 saturation = gradient->selected_stage->saturation;
                    f32 value = gradient->selected_stage->value;
                    edui_colour_pick_s *colour = (edui_colour_pick_s *)pick;
                    colour->hue = hue;
                    colour->saturation = saturation;
                    colour->value = value;
                    colour->cursor_x = (f32)(hue / 360.0);
                    colour->cursor_y = value;
                }
                break;
        }
        eduiMenuAddItem(gpcfg, pick);
        eduiMenuAddItem(gpcfg, eduiItemSelCreate((u32)item, &ed_attr, 0, 0, cbgpcfgAdd, "Add"));
        eduiMenuAddItem(gpcfg, eduiItemSelCreate((u32)item, &ed_attr, 0, 0, cbgpcfgDel, "Del"));
        eduiMenuAddItem(gpcfg, eduiItemSelCreate((u32)item, &ed_attr, 0, 0, cbgpcfgCopy, "Copy"));
        edui_last_item->type = item->type;
        pick = eduiItemSelCreate((u32)item, &ed_attr, 0, 0, cbgpcfgPaste, "Paste");
        if (pick) {
            pick->render = eduicbRenderSelWithClipColour;
        }
        eduiMenuAddItem(gpcfg, pick);
        edui_last_item->type = item->type;
        eduiMenuAttach(menu, gpcfg);
    }

    i32 eduicbProcessGradPickItem(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD *pad) {
        edui_gradient_pick_s *gradient = (edui_gradient_pick_s *)item;
        edui_gradient_node_s *stage;
        f32 lower;
        f32 upper;
        i32 decrease;
        i32 increase;
        if (gradient->changed) {
            gradient->changed(menu, item, pad->digital_buttons);
        }
        if (pad->digital_buttons_pressed & PAD_BUTTON_CROSS) {
            eduicbProcessGradPick(menu, item);
            return 0;
        }
        if (!gradient->selected_stage) {
            gradient->selected_stage = gradient->first_stage;
        }
        stage = gradient->selected_stage;
        if (!stage) {
            return 0;
        }
        if (pad->digital_buttons_pressed & PAD_BUTTON_LEFT) {
            if (stage->previous) {
                gradient->selected_stage = stage->previous;
            }
            return 1;
        }
        if (pad->digital_buttons_pressed & PAD_BUTTON_RIGHT) {
            if (stage->next != NULL) {
                gradient->selected_stage = gradient->selected_stage->next;
            }
            return 1;
        }
        if (pad->digital_buttons & (PAD_BUTTON_RIGHT | PAD_BUTTON_LEFT)) {
            return 1;
        }
        lower = 0.0f;
        upper = 1.0f;
        if (stage->previous) {
            lower = stage->previous->time;
        }
        if (stage->next != NULL) {
            upper = gradient->selected_stage->next->time;
        }
        decrease = pad->analog_l2;
        increase = pad->analog_r2;
        if (pad->analog_l1) {
            decrease = pad->analog_l1;
        }
        if (pad->analog_r1) {
            increase = pad->analog_r1;
        }
        if (decrease) {
            f32 step = (f32)(decrease / 15360.0);
            f64 time = (f64)stage->time - step;
            stage->time = lower > time ? lower : time;
            gradient->change_timer = 60;
        } else if (increase) {
            f32 step = (f32)(increase / 15360.0);
            f64 time = (f64)step + stage->time;
            stage->time = upper < time ? upper : time;
            gradient->change_timer = 60;
        }
        return 0;
    }

    void eduiCreateMessageMenu(eduimenu_s *parent, char *message, i32 highlighted) {
        u32 colours[4] = {0x800000c0u, 0x80ff0000, 0x80808080, 0x80404040};
        if (highlighted == 1) {
            colours[0] = 0x8000c000u;
        }
        edui_messagemenu = eduiMenuCreate(70, 70, 180, 250, parent->font, eduicbCancelMessageMenu, ((char *)("Message")));
        if (!edui_messagemenu) {
            return;
        }
        eduiMenuAddItem(edui_messagemenu, eduiItemSelCreate(1, colours, 0, 0, NULL, message));
        eduiMenuAttach(parent, edui_messagemenu);
        edui_messagemenu->x = parent->x + 10;
        edui_messagemenu->y = parent->y + 40;
    }

    void eduiCreate3LineMessageMenu(eduimenu_s *parent, char *first, char *second, char *third, i32 first_highlight, i32 second_highlight,
                                    i32 third_highlight) {
        eduiiattr_s normal = {0x800000c0, 0x80ff0000, 0x80808080, 0x80404040};
        eduiiattr_s highlighted = {0x8000c000, 0x80ff0000, 0x80808080, 0x80404040};
        edui_messagemenu = eduiMenuCreate(70, 70, 300, 250, parent->font, eduicbCancelMessageMenu, ((char *)("Message")));
        if (edui_messagemenu) {
            if (first) {
                eduiMenuAddItem(edui_messagemenu, eduiItemSelCreate(1, first_highlight ? &highlighted : &normal, 0, 0, NULL, first));
            }
            if (second) {
                eduiMenuAddItem(edui_messagemenu, eduiItemSelCreate(1, second_highlight ? &highlighted : &normal, 0, 0, NULL, second));
            }
            if (third) {
                eduiMenuAddItem(edui_messagemenu, eduiItemSelCreate(1, third_highlight ? &highlighted : &normal, 0, 0, NULL, third));
            }
            eduiMenuAttach(parent, edui_messagemenu);
            edui_messagemenu->x = parent->x + 10;
            edui_messagemenu->y = parent->y + 40;
        }
    }

    i32 eduiCheckForPadMenuCancel(eduimenu_s *menu, FRAME_PAD *pad) {
        struct eduimenu_s *parent;
        if (!pad || !(pad->digital_buttons_pressed & PAD_BUTTON_TRIANGLE)) {
            return 0;
        }

        parent = NULL;
        if (menu->parent != NULL) {
            parent = menu->parent;
        }
        if (EduiUsingMenuFocus == 0 && menu->parent != NULL) {
            eduiMenuDetach(menu);
        }
        if (menu->callback) {
            menu->callback(menu, parent);
        }
        return 1;
    }

    i32 eduiMenuProcessInput(eduimenu_s *menu, f32 delta_time, FRAME_PAD *pad, i32 item_result) {
        static f32 up_rept = 0;
        static f32 down_rept = 0;
        u32 up_button;
        u32 down_button;
        i32 up;
        i32 down;
        i32 to_first;
        i32 to_last;
        i32 page_up;
        cbInteractMenuKeySelect(menu);
        if (!pad || item_result) {
            return 0;
        }
        to_last = 0;
        to_first = 0;
        down = 0;
        up = 0;
        page_up = 0;
        if (menu && menu->selected && menu->selected->input && !menu->selected->disabled && pad->digital_buttons) {
            if (menu->selected->input(menu, menu->selected, pad->digital_buttons, pad->digital_buttons_pressed)) {
                return 0;
            }
        }
        if (menu->horizontal) {
            down_button = PAD_BUTTON_RIGHT;
            up_button = PAD_BUTTON_LEFT;
        } else {
            down_button = PAD_BUTTON_DOWN;
            up_button = PAD_BUTTON_UP;
        }
        if (pad->digital_buttons_pressed & up_button) {
            up_rept = 1.0f / 3.0f;
            up = 1;
        }
        if ((pad->digital_buttons & up_button) && up_rept > 0.0f) {
            up_rept -= delta_time;
            if (up_rept <= 0.0f) {
                up_rept = 1.0f / 6.0f;
                up = 1;
            }
        }
        if (!menu->pinned && !menu->horizontal) {
            if ((pad->digital_buttons & PAD_BUTTON_LEFT) && !(menu->flags & EDUI_MENU_FIXED)) {
                i32 distance = (i32)(delta_time * 1500.0f);
                if (distance < 1) {
                    distance = 1;
                }
                menu->x -= distance;
            }
            if ((pad->digital_buttons & PAD_BUTTON_RIGHT) && !(menu->flags & EDUI_MENU_FIXED)) {
                i32 distance = (i32)(delta_time * 1500.0f);
                if (distance < 1) {
                    distance = 1;
                }
                menu->x += distance;
            }
        }
        if (pad->digital_buttons_pressed & down_button) {
            down_rept = 1.0f / 3.0f;
            down = 1;
        }
        if ((pad->digital_buttons & down_button) && down_rept > 0.0f) {
            down_rept -= delta_time;
            if (down_rept <= 0.0f) {
                down_rept = 1.0f / 6.0f;
                down = 1;
            }
        }
        if (up && (pad->digital_buttons & PAD_BUTTON_L2)) {
            to_first = 1;
        }
        if (down && (pad->digital_buttons & PAD_BUTTON_L2)) {
            to_last = 1;
        }
        if (up && (pad->digital_buttons & PAD_BUTTON_R2)) {
            page_up = 1;
        }
        if (down && (pad->digital_buttons & PAD_BUTTON_R2) && menu->last_visible) {
            eduiitem_s *item;
            i32 count = 1;
            for (item = menu->first_visible; item && item != menu->last_visible; item = item->next) {
                ++count;
            }
            while (count && menu->selected->next) {
                --count;
                menu->selected = menu->selected->next;
            }
            return 0;
        }
        if (page_up && menu->last_visible) {
            eduiitem_s *item;
            i32 count = 1;
            for (item = menu->first_visible; item && item != menu->last_visible; item = item->next) {
                ++count;
            }
            menu->selected = menu->first_visible;
            while (count && menu->selected->previous) {
                --count;
                menu->selected = menu->selected->previous;
                menu->first_visible = menu->selected;
            }
            return 0;
        }
        if (up && menu->selected && menu->selected->previous) {
            do {
                eduiitem_s *item;
                if (menu->selected == menu->first_visible) {
                    menu->first_visible = menu->selected->previous;
                }
                item = menu->selected->previous;
                while (item->type == EDUIITEM_SEPARATOR) {
                    item = item->previous;
                }
                menu->selected = item;
            } while (menu->selected->previous && to_first);
            return 0;
        }
        if (down && menu->selected && menu->selected->next) {
            do {
                eduiitem_s *item = menu->selected->next;
                while (item->type == EDUIITEM_SEPARATOR) {
                    item = item->next;
                }
                menu->selected = item;
            } while (menu->selected->next && to_last);
            if (to_last) {
                eduiitem_s *item;
                i32 count = 1;
                for (item = menu->first_visible; item && item != menu->last_visible; item = item->next) {
                    ++count;
                }
                menu->first_visible = menu->selected;
                while (count && menu->first_visible->previous) {
                    --count;
                    menu->first_visible = menu->first_visible->previous;
                }
            }
            return 0;
        }
        eduiCheckForPadMenuCancel(menu, pad);
        return 0;
    }

    void cbSubMenu(eduimenu_s *menu, eduiitem_s *item) {
        eduiMenuAttach(menu, (eduimenu_s *)item->data_ptr);
    }

    void cbTriggerSubMenu(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
        eduimenu_s *submenu = (eduimenu_s *)item->data_ptr;
        if (submenu != NULL) {
            eduiMenuAttach(menu, submenu);
            submenu->x = (i32)((u32)item->x + 10u);
            submenu->y = 50;
            submenu->y = MIN((i32)((u32)PS2_REZ_H * 2u - (u32)submenu->height), 50);
        }
        item->highlighted = 0;
    }
}

void cbCancelSubMenu(eduimenu_s *menu, eduimenu_s *) {
    eduiMenuDetach(menu);
}

extern "C" {

    void cbCancelSubMenuFromItem(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
        eduiMenuDetach(menu);
    }
}

void cbColourPickSel(eduimenu_s *menu, eduiitem_s *, u32) {
    edui_colour_pick_s *picker = cp_item;
    *EduiColourTargetRed = picker->red;
    *EduiColourTargetGreen = picker->green;
    f32 blue = picker->blue;
    *EduiColourTargetBlue = blue;
    if (EduiColourTargetPacked != NULL) {
        u32 packed = (u32)(*EduiColourTargetAlpha * 255.0);
        packed = (packed << 8) | ((u32)(blue * 255.0) & 255);
        packed = (packed << 8) | ((u32)(*EduiColourTargetGreen * 255.0) & 255);
        packed = (packed << 8) | ((u32)(*EduiColourTargetRed * 255.0) & 255);
        *EduiColourTargetPacked = packed;
    }
    eduiMenuDetach(menu);
}

extern "C" {
    static eduimenu_s *colourmenu = NULL;
    f32 EduiColourTempRed = 0.0f;
    f32 EduiColourTempGreen = 0.0f;
    f32 EduiColourTempBlue = 0.0f;
    f32 EduiColourTempAlpha = 0.0f;
}

extern "C" void AddColourPick(eduimenu_s *parent, eduiitem_s *item, f32 *red, f32 *green, f32 *blue, u32 *rgba) {
    eduimenu_s *picker_menu = colourmenu;
    f32 *target_red = red;
    f32 *target_green = green;
    f32 *target_blue = blue;
    edui_colour_pick_s *picker;
    i32 bottom;
    f32 red_value;
    f32 green_value;
    f32 blue_value;
    char text[256];
    if (picker_menu->parent == NULL) {
        eduimenu_s *last = parent;
        while (last->child != NULL) {
            last = last->child;
        }
        picker_menu->parent = last;
        last->child = picker_menu;
        eduiSetActiveMenu(picker_menu);
    }
    if (item != NULL) {
        picker_menu->x = item->x + 10;
        picker_menu->y = item->y + 10;
    } else {
        picker_menu->x = parent->x + 10;
        picker_menu->y = parent->y + 10;
    }
    bottom = PS2_REZ_H - picker_menu->height - 20;
    picker_menu->y = picker_menu->y <= bottom ? picker_menu->y : bottom;
    EduiColourTargetPacked = rgba;
    EduiColourTargetAlpha = &EduiColourTempAlpha;
    if (rgba != NULL) {
        u8 *channels = (u8 *)rgba;
        EduiColourTempRed = (f32)(channels[0] / 255.0);
        EduiColourTempGreen = (f32)(channels[1] / 255.0);
        EduiColourTempBlue = (f32)(channels[2] / 255.0);
        EduiColourTempAlpha = (f32)(channels[3] / 255.0);
        target_red = &EduiColourTempRed;
        target_green = &EduiColourTempGreen;
        target_blue = &EduiColourTempBlue;
    }
    picker = cp_item;
    blue_value = *target_blue;
    green_value = *target_green;
    EduiColourTargetBlue = target_blue;
    red_value = *target_red;
    EduiColourTargetGreen = target_green;
    EduiColourTargetRed = target_red;
    EduiColourRgbToHsv(red_value, green_value, blue_value, &picker->hue, &picker->saturation, &picker->value);
    picker->cursor_x = (f32)(picker->hue / 360.0);
    picker->cursor_y = picker->value;
    NuSPrintf(text, "R:%1.2f G:%1.2f B:%1.2f", *target_red, *target_green, *target_blue);
    eduiItemSetText(cp_info, text);
}

extern "C" {
    static eduiitem_s *cp_copy = NULL;
    i32 EduiColourPickerInitialized = 0;
}

extern "C" void CreateColourPicker(void) {
    u32 colours[4] = {0x80000000, 0x800000ff, 0x80808080, 0x80f0f0f0};
    if (EduiColourPickerInitialized != 0) {
        return;
    }
    EduiColourPickerInitialized = 1;
    colourmenu = eduiMenuCreate(200, 70, 180, 250, NULL, cbCancelSubMenu, "Pick Colour");
    if (colourmenu != NULL) {
        eduiitem_s *item = eduiItemColourPickCreate(0, colours, cbColourPickSel, "");
        if (colourmenu->last != NULL) {
            colourmenu->last->next = item;
        } else {
            colourmenu->first = item;
        }
        item->previous = colourmenu->last;
        item->next = NULL;
        colourmenu->last = item;
        colourmenu->first_visible = colourmenu->first;
        edui_last_item = item;
        colourmenu->last_visible = NULL;
        cp_item = (edui_colour_pick_s *)item;
        cp_process = item->process;
        item->process = cbProcessColourPick;
        item = eduiItemSelCreate(1, colours, 0, 0, cbToggleIndicatorMode, "R:?? G:?? B:??");
        if (colourmenu->last != NULL) {
            colourmenu->last->next = item;
        } else {
            colourmenu->first = item;
        }
        item->previous = colourmenu->last;
        item->next = NULL;
        colourmenu->last = item;
        colourmenu->first_visible = colourmenu->first;
        edui_last_item = item;
        colourmenu->last_visible = NULL;
        cp_info = item;
        item = eduiItemSelCreate(1, colours, 0, 0, cbCopy, "Copy To Clipboard");
        if (colourmenu->last != NULL) {
            colourmenu->last->next = item;
        } else {
            colourmenu->first = item;
        }
        item->previous = colourmenu->last;
        item->next = NULL;
        colourmenu->last = item;
        colourmenu->first_visible = colourmenu->first;
        edui_last_item = item;
        colourmenu->last_visible = NULL;
        cp_copy = item;
        item = eduiItemSelCreate(1, colours, 0, 0, cbPaste, "Paste From Clipboard");
        if (colourmenu->last != NULL) {
            colourmenu->last->next = item;
        } else {
            colourmenu->first = item;
        }
        item->previous = colourmenu->last;
        item->next = NULL;
        colourmenu->last = item;
        colourmenu->first_visible = colourmenu->first;
        edui_last_item = item;
        colourmenu->last_visible = NULL;
        cp_paste = item;
    }
}

extern "C" {

    /* Adds an editor module to the front of the list. */
    i32 edmainRegister(ed_module_s *module) {
        if (ed_init) {
            edmainClose();
        }
        if (ed_list) {
            ed_list->previous = module;
        }
        module->next = ed_list;
        module->previous = NULL;
        ed_list = module;
        return 1;
    }

    i32 edmain_render_interacts = 0;

    /* Renders the active editor module (the main menu without one) and the
     * cursor's interact boxes. */
    void edmainRenderAll(void) {
        if (edmain_cursor_enabled) {
            eduiFlushInteracts();
        }
        if (ed_module_active && ed_curr) {
            if (ed_curr->render) {
                ed_curr->render();
            }
        } else {
            edmainRender();
        }
        if (edmain_cursor_enabled && edmain_render_interacts) {
            eduiRenderInteracts();
        }
    }

    void cbMMEditorConfig(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
        ed_cfg_menu->x = menu->x + 10;
        ed_cfg_menu->y = menu->y + 10;
        eduiMenuAttach(menu, ed_cfg_menu);
    }

    void cbmcfgCancel(eduimenu_s *menu, eduimenu_s *unused1) {
        eduiMenuDetach(menu);
    }
    struct vufnt_s *ed_fnt = 0;
    static i32 ed_main_menu_h = 320;
    static i32 ed_main_menu_w = 180;
    static i32 ed_main_menu_x = 220;
    static i32 ed_main_menu_y = 50;
    static f32 edmain_default_camera_speed = 1.0f;

    void edmainCreateMenus(void) {
        ed_module_s *module;
        NuQFntPushCoordinateSystem(NUQFNT_CS_SIXTEENTHS);
        NuQFntSetScale(system_qfont, edmain_menu_scale, edmain_menu_scale);
        ed_cfg_menu = eduiMenuCreate(220, 50, 180, 320, ed_fnt, cbmcfgCancel, "Editor Config");
        if (ed_cfg_menu != NULL) {
            eduiMenuAddItem(ed_cfg_menu, eduiItemSliderCreate(0, &ed_attr, 0, cbmcfgCameraSpeed, 1.0f, 20.0f, edmain_default_camera_speed,
                                                              "Camera Speed"));
            eduiMenuAddItem(ed_cfg_menu,
                            eduiItemToggleCreate(0, &ed_attr, edmain_auto_speed, 1, cbSetAutoSpeed, "Proportional Cursor Speed"));
            eduiMenuFitWidth(ed_cfg_menu, 8);
        }
        ed_main_menu = eduiMenuCreate(ed_main_menu_x, ed_main_menu_y, ed_main_menu_w, ed_main_menu_h, ed_fnt, cbMMCancel, "Editor Options");
        if (ed_main_menu != NULL) {
            for (module = ed_list; module != NULL; module = module->next) {
                eduiitem_s *item = eduiMenuAddItem(ed_main_menu, eduiItemSelCreate((u32)module, &ed_attr, 0, 1, cbMMRegSel, module->name));
                if (module == ed_curr) {
                    eduiMenuHighlight(ed_main_menu, item);
                }
            }
            eduiMenuAddItem(ed_main_menu, eduiItemSelCreate(0, &ed_attr, 0, 0, cbMMEditorConfig, "Editor Config"));
            eduiMenuAddItem(ed_main_menu, eduiItemSelCreate(0, &ed_attr, 0, 0, cbMMReturnToApp, "Return To App"));
            eduiMenuFitWidth(ed_main_menu, 8);
        }
        NuQFntPopCoordinateSystem();
    }

    eduiitem_s *eduiItemGradPickCreate(u32 data, void *colours, void *callback, char *text) {
        edui_gradient_pick_s *item = (edui_gradient_pick_s *)NuAlloc(sizeof(edui_gradient_pick_s),
                                                                     "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0xaa2);
        if (item) {
            memset(item, 0, sizeof(edui_gradient_pick_s));
            item->base.data = data;
            item->base.type = EDUIITEM_GRAD_PICK;
            memcpy(item->base.colours, colours, sizeof(item->base.colours));
            item->base.text_alignment = EDUI_ALIGN_CENTRE;
            item->base.selection_group = 0;
            item->base.process = eduicbProcessGradPickItem;
            item->base.render = eduicbRenderGradPick;
            item->base.destroy = eduicbItemGradPickDestroy;
            eduiItemSetText(&item->base, text);
            item->changed = (void (*)(eduimenu_s *, eduiitem_s *, u32))callback;
            item->change_timer = 0;
            return &item->base;
        }
        return NULL;
    }

    eduiitem_s *eduiItemGreyGradPickCreate(size_t data, const void *colours, EdUiItemCallback callback, char *text) {
        eduiitem_s *item = eduiItemGradPickCreate(data, (void *)colours, callback, text);
        if (item) {
            item->type = EDUIITEM_GREY_GRAD_PICK;
        }
        return item;
    }

    eduiitem_s *eduiItemDataGradPickCreate(size_t data, void *colours, EdUiItemCallback callback, EdUiItemCallback press,
                                           EdUiItemCallback add, EdUiItemCallback remove, EdUiItemCallback copy, EdUiItemCallback paste,
                                           char *text) {
        edui_gradient_pick_s *item = (edui_gradient_pick_s *)eduiItemGradPickCreate(data, colours, callback, text);
        if (item) {
            item->base.type = EDUIITEM_DATA_GRAD_PICK;
            item->press = press;
            item->add = add;
            item->remove = remove;
            item->copy = copy;
            item->paste = paste;
        }
        return &item->base;
    }

    i32 eduiMenuProcessAux(eduimenu_s *menu, f32 delta_time, FRAME_PAD *pad) {
        if (EduiMenuHasFocus(menu)) {
            i32 result;
            EduiMenuEnsureSelected(menu);
            result = EduiMenuProcessSelected(menu, delta_time, pad);
            if (EduiMenuHasFocus(menu) && menu == processing_menu) {
                EduiMenuEnsureSelected(menu);
                if (EduiMenuHasFocus(menu)) {
                    eduiMenuProcessInput(menu, delta_time, pad, result);
                }
            }
        }
        return eduiProcessInteracts(menu, pad);
    }

    eduimenu_s *submenu0 = NULL;
    static struct eduimenu_s *submenu1 = 0;
    static struct eduimenu_s *testmenu0 = 0;

    void CreateTestMenu(struct vufnt_s *font) {
        u32 colours[4] = {0x80000000u, 0x80ff0000u, 0x80808080u, 0x80404040u};
        eduiInitMaterials();

        submenu1 = eduiMenuCreate(90, 90, 180, 50, font, NULL, ("SubMenu1"));
        if (submenu1 != NULL) {
            eduiMenuAddItem(submenu1, eduiItemSelCreate(0, colours, 0, 1, (void *)cbSel, ("Sub Sel 0")));
            eduiMenuAddItem(submenu1, eduiItemSelCreate(1, colours, 0, 1, (void *)cbSel, ("Sub Sel 1")));
            eduiMenuAddItem(submenu1, eduiItemSelCreate(2, colours, 0, 1, (void *)cbSel, ("Sub Sel 2")));
        }

        submenu0 = eduiMenuCreate(70, 70, 180, 50, font, NULL, ("SubMenu0"));
        if (submenu0 != NULL) {
            eduiMenuAddItem(submenu0, eduiItemSelCreate(0, colours, 0, 1, (void *)cbSel, ("Sub Sel")));
            eduiMenuAddItem(submenu0, eduiItemSelCreate((u32)(submenu1), colours, 0, 0, (void *)cbSubMenu, ("Sub Menu 1..")));
            eduiMenuAddItem(submenu0, eduiItemSelCreate(2, colours, 0, 1, (void *)cbSel, ("Sub Sel")));
        }

        testmenu0 = eduiMenuCreate(50, 50, 180, 50, font, NULL, ("Main Menu"));
        if (testmenu0 != NULL) {
            edui_gradient_pick_s *gradient;
            edui_gradient_pick_s *grey_gradient;
            eduiMenuAddItem(testmenu0, eduiItemSelCreate((u32)(submenu0), colours, 0, 0, (void *)cbSubMenu, ("Sub Menu 0..")));

            gradient = (edui_gradient_pick_s *)(eduiItemGradPickCreate(1, colours, (void *)cbGradChange, ("GradPick")));
            if (gradient != NULL) {
                eduiGradStageAdd(gradient, 0.0f, 0.0f, 0.0f, 0.0f);
                eduiGradStageAdd(gradient, 1.0f, 0.0f, 0.0f, 1.0f);
            }
            eduiMenuAddItem(testmenu0, (eduiitem_s *)gradient);

            grey_gradient =
                (edui_gradient_pick_s *)(eduiItemGreyGradPickCreate(1, colours, (EdUiItemCallback)cbGradChange, ("GreyGradPick")));
            if (grey_gradient != NULL) {
                eduiGradStageAdd(grey_gradient, 0.0f, 0.0f, 0.0f, 0.0f);
                eduiGradStageAdd(grey_gradient, 1.0f, 0.0f, 0.0f, 1.0f);
            }
            eduiMenuAddItem(testmenu0, (eduiitem_s *)grey_gradient);

            eduiMenuAddItem(testmenu0, eduiItemSelCreate(2, colours, 0, 1, (void *)cbSel, ("Sel")));
        }
    }

    void edmainInit(struct vufnt_s *font, char *configuration_file) {
        ed_module_s *module;
        if (ed_init != 0) {
            edmainClose();
        }
        ed_fnt = font;
        ed_module_active = 0;
        for (module = ed_list; module != NULL; module = module->next) {
            if (module->init != NULL) {
                module->init();
            }
        }
        edinternalcam = NuCameraCreate();
        EdMainCurrentCamera = edinternalcam;
        eduiInitMaterials();
        eduiFilePickAllocDirList();
        edmainCreateMenus();
        if (configuration_file != NULL) {
            NuStrCpy(ed_levelfile, configuration_file);
        } else {
            ed_levelfile[0] = 0;
        }
        edmainReadConfig();
        ed_init = 1;
    }

    void edmainInitEx(struct vufnt_s *font, char *configuration_file, eduiiattr_s *colours, i32 x, i32 y, i32 width, i32 height) {
        ed_attr = *colours;
        ed_main_menu_x = x;
        ed_main_menu_y = y;
        ed_main_menu_w = width;
        ed_main_menu_h = height;
        edmainInit(font, configuration_file);
    }

    i32 eduiMenuProcess(eduimenu_s *menu, f32 delta_time, FRAME_PAD *pad) {
        i32 result;
        processing_menu = menu;
        if (menu->child == NULL) {
            return eduiMenuProcessAux(menu, delta_time, pad);
        }
        result = eduiMenuProcess(menu->child, delta_time, pad);
        if (processing_menu) {
            if (result) {
                eduiSetActiveMenu(menu->child ? menu->child : menu);
            } else {
                result = eduiProcessInteracts(menu, pad);
            }
        }
        return result;
    }

    void TestMenu(FRAME_PAD *pad) {
        eduiMenuProcess(testmenu0, 0.016666668f, pad);
        eduiMenuRender(testmenu0);
    }

    void edmainProcessMainMenu(f32 delta_time, FRAME_PAD *pad) {
        eduiitem_s *item;
        if (ed_main_menu) {
            for (item = ed_main_menu->first; item; item = item->next) {
                struct ed_module_s *module = (ed_module_s *)item->data_ptr;
                if (module) {
                    item->disabled = ((size_t)module->reserved & 1) != 0;
                }
            }
            eduiMenuProcess(ed_main_menu, delta_time, pad);
        }
    }

    i32 edmainProcess(f32 delta_time, FRAME_PAD *pad) {
        i32 result;
        editor_return = 0;
        if (edmain_cursor_enabled) {
            eduiProcessCursorDefault(0.0f, NULL);
        }
        if (ed_module_active && ed_curr) {
            if (ed_curr->process) {
                result = ed_curr->process(delta_time, pad);
                if (result) {
                    if (ed_curr->deactivate) {
                        ed_curr->deactivate();
                    }
                    ed_module_active = 0;
                    if (result == 2) {
                        editor_return = 1;
                    }
                }
                return editor_return;
            }
            ed_module_active = 0;
            return editor_return;
        }
        if (!ed_curr) {
            ed_curr = ed_list;
            eduiSetActiveMenu(ed_main_menu);
            eduiSetDefaultActiveMenu(ed_main_menu);
        }
        edmainProcessMainMenu(delta_time, pad);
        return editor_return;
    }
}
