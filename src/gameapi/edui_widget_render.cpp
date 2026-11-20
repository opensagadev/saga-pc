#include "gameapi/edtools/edcam.h"
#include "gameapi/edtools/edfile.h"
#include "gameapi/edtools/edrender.h"
#include "gameapi/edtools/edtoolsall.h"
#include "gameapi/edtools/edui.h"
#include "gameapi/part.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuprim.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

i32 ui_bgmtl = 0;
extern "C" NUMTL *uimtls[5];

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

static void EduiRndrRect2di(i32 x, i32 y, i32 width, i32 height, i32 colour, NUMTL *material) {
    if (edui_donotdraw == 0) {
        NuRndrRect2di(x, y, width, height, colour, material);
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

static void EduiRndrLineRect2di(i32 x, i32 y, i32 width, i32 height, i32 colour, NUMTL *material) {
    if (edui_donotdraw == 0) {
        NuRndrLineRect2di(x, y, width, height, colour, material);
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

static void EduiRndrRectUV2di(i32 x, i32 y, i32 width, i32 height, f32 u0, f32 v0, f32 u1, f32 v1, i32 colour, NUMTL *material) {
    if (edui_donotdraw == 0) {
        NuRndrRectUV2di(x, y, width, height, u0, v0, u1, v1, colour, material);
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

extern "C" f32 EduiScaleX;
extern "C" f32 EduiScaleY;

/* Prints at a 640x448 position, scaled to the 2D renderer's sub-pixels. */
extern "C" void EduiFntPrintScaled(VUFNT *font, f32 x, f32 y, i32 alignment, char *text) {
    EduiPrintAlignedText(font, (i32)(x * EduiScaleX), (i32)(y * EduiScaleY), alignment, text);
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

void eduiItemColourPickSetHSV(edui_colour_pick_s *item, f32 hue, f32 saturation, f32 value) {
    item->hue = hue;
    item->saturation = saturation;
    item->value = value;
    item->cursor_x = (f32)((f64)hue / 360.0);
    item->cursor_y = value;
}

extern "C" i32 EduiSelectionMaterial;

static void EduiDrawGradientSpan(i32 x, i32 y, i32 width, i32 height, u32 first_colour, u32 second_colour, f32 start, f32 end);

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

static void eduiHSVToRGB(f32 hue, f32 saturation, f32 value, f32 &red, f32 &green, f32 &blue);

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

extern "C" eduiitem_s *eduiItemNumberCreate(u32 data, void *colours, i32 group, void *callback, f32 minimum, f32 range, f32 value,
                                            char *text) {
    eduiitem_s *item = eduiItemSliderCreate(data, colours, group, callback, minimum, range, value, text);
    if (item != NULL) {
        item->render = eduicbRenderNumber;
    }
    return item;
}

struct edui_text_selector_s { // size 0x70
    edui_slider_s slider;     // 0x00
    char **options;           // 0x6c
};

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

struct edui_separator_s {               // size 0x4c
    eduiitem_s base;                    // 0x00
    i32 (*interact)(edui_interact_s *); // 0x48
};

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

extern "C" void eduicbItemGradPickDestroy(eduimenu_s *unused_menu, eduiitem_s *item) {
    edui_gradient_pick_s *gradient = (edui_gradient_pick_s *)item;
    while (gradient->first_stage != NULL) {
        eduiGradStageDelete(gradient, gradient->first_stage);
    }
    NuFree(gradient, "d:\\projects\\game_saga\\gameapi.saga\\edtools\\edui.cpp", 0xa9b);
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

struct EDUI_COLOUR_CLIPBOARD { // size 0x10
    f32 hue;                   // 0x0
    f32 saturation;            // 0x4
    f32 value;                 // 0x8
    f32 gray;                  // 0xc
};

extern "C" {
    EDUI_COLOUR_CLIPBOARD EduiColourClipboard = {0.0f, 0.0f, 0.0f, 0.0f};
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
    u32 eduimenu_outline_colour = 0x80404040;
    u32 eduimenu_selected_outline_colour = 0x80ffffff;
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
    static eduimenu_s *colourmenu = NULL;
    struct eduimenu_s *edui_messagemenu = 0;
    static edui_colour_pick_s *cp_item = NULL;
    static eduiitem_s *cp_info = NULL;
    static eduiitem_s *cp_copy = NULL;
    static eduiitem_s *cp_paste = NULL;
    static i32 (*cp_process)(eduimenu_s *, eduiitem_s *, f32, FRAME_PAD *) = NULL;
    static f32 *EduiColourTargetRed = NULL;
    static f32 *EduiColourTargetGreen = NULL;
    struct numtx_s *ed_remap_mtx = 0;
    static f32 *EduiColourTargetBlue = NULL;
    static f32 *EduiColourTargetAlpha = NULL;
    static u32 *EduiColourTargetPacked = NULL;
    static f32 EduiColourClipboardRed = 0.5f;
    static f32 EduiColourClipboardGreen = 0.5f;
    static f32 EduiColourClipboardBlue = 0.5f;
    i32 EduiColourPickerInitialized = 0;

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

void eduiItemColourPickSetRGB(edui_colour_pick_s *item, f32 red, f32 green, f32 blue) {
    EduiColourRgbToHsv(red, green, blue, &item->hue, &item->saturation, &item->value);
    item->cursor_x = (f32)(item->hue / 360.0);
    item->cursor_y = item->value;
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

void cbCancelSubMenu(eduimenu_s *menu, eduimenu_s *) {
    eduiMenuDetach(menu);
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
    f32 EduiColourTempRed = 0.0f;
    struct edui_interact_s *eduiInteractLocked = 0;
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

typedef union edui_gradient_colour_u { // size 0x4
    u32 word;
    u8 channel[4];
} EDUI_GRADIENT_COLOUR;

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

extern "C" {
    f32 EduiSliderAcceleration = 1.0f;
    i32 EduiSliderRepeatTimer = 30;
    f32 EduiSliderDynamicScale = 0.0f;
    f32 EduiSliderGlobalAcceleration = 0.0f;
    f32 EduiNominalFrameTime = 1.0f / 60.0f;
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
    extern i32 show_x_scale_help;
    extern i32 show_y_scale_help;
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
    i32 show_x_scale_help = 0;
    i32 show_y_scale_help = 0;
    i32 EduiGraphDrawCurve = 1;
    i32 EduiGraphDrawDebug = 0;
    i32 EduiGraphDrawDebugBound = 0;
    i32 near_clip_at_cursor = 0;
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

extern "C" i32 EduiRenderWidth;

extern "C" {
    char *EduiTextCopyHelp = "Copy L2+R2 Paste L1+R1";
    i32 EduiTextCursorPeriod = 32;
    i32 edui_filepick_listsize = 8;
    i32 EduiTextCursorCountdown = 0;
    char EduiTextCopyBuffer[64] = {0};
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
    i32 EduiColourSliderRepeatTimer = 30;
    f32 EduiColourSliderDynamicScale = 0.0f;
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
        EduiColourSliderRepeatTimer = (i32)((u32)EduiColourSliderRepeatTimer - 1u);
        if (EduiColourSliderRepeatTimer < 0) {
            EduiColourSliderRepeatTimer = 0;
        }
        if (!EduiColourSliderRepeatTimer || (pad->digital_buttons_pressed & PAD_BUTTON_L1)) {
            f32 value = MAX(slider->minimum, slider->value - slider->granularity);
            EduiColourSliderRound(slider, menu, item, pad, value);
        }
    } else if (pad->digital_buttons & PAD_BUTTON_R1) {
        EduiColourSliderRepeatTimer = (i32)((u32)EduiColourSliderRepeatTimer - 1u);
        if (EduiColourSliderRepeatTimer < 0) {
            EduiColourSliderRepeatTimer = 0;
        }
        if (!EduiColourSliderRepeatTimer || (pad->digital_buttons_pressed & PAD_BUTTON_R1)) {
            f32 value = MIN(slider->minimum + slider->range, slider->value + slider->granularity);
            EduiColourSliderRound(slider, menu, item, pad, value);
        }
    } else {
        EduiColourSliderRepeatTimer = 30;
        if (pad->analog_l2) {
            if (pad->analog_circle) {
                EduiColourSliderDynamicScale *= (f64)1.05f;
                slider->normalized_value = MAX(0.0f, slider->normalized_value - (u32)pad->analog_l2 * EduiColourSliderDynamicScale);
            } else {
                EduiColourSliderDynamicScale = 1.6276043e-7f;
                slider->normalized_value = (f32)MAX(0.0f, slider->normalized_value - (u32)pad->analog_l2 * (f64)0.000016276043f);
            }
            slider->value = slider->normalized_value * slider->range + slider->minimum;
            slider->change_timer = 60;
            if (slider->changed) {
                slider->changed(menu, item, pad->digital_buttons);
            }
        } else if (pad->analog_r2) {
            if (pad->analog_circle) {
                EduiColourSliderDynamicScale *= (f64)1.05f;
                slider->normalized_value = MIN(1.0f, slider->normalized_value + (u32)pad->analog_r2 * EduiColourSliderDynamicScale);
            } else {
                EduiColourSliderDynamicScale = 1.6276043e-7f;
                slider->normalized_value = (f32)MIN(1.0f, slider->normalized_value + (u32)pad->analog_r2 * (f64)0.000016276043f);
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

extern "C" edui_gradient_node_s *eduiGradStageAddRGB(edui_gradient_pick_s *item, f32 time, f32 red, f32 green, f32 blue) {
    f32 hue;
    f32 saturation;
    f32 value;
    EduiColourRgbToHsv(red, green, blue, &hue, &saturation, &value);
    return eduiGradStageAdd(item, time, hue, saturation, value);
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

extern "C" i32 eduicbProcessGreyPick(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD *pad) {
    edui_colour_pick_s *pick = (edui_colour_pick_s *)item;
    i32 left = pad->analog_l1 ? pad->analog_l1 : pad->analog_l2;
    i32 right = pad->analog_r1 ? pad->analog_r1 : pad->analog_r2;
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

extern "C" i32 near_clip_at_cursor;

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
