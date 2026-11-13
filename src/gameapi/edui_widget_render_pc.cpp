#include "saga.h"

i32 ui_bgmtl = 0;
extern "C" struct PC_SCENE_MTL_VIEW *uimtls[5];
extern "C" i32 __cdecl NuVSPrintf(char *dest, const char *format, va_list args);

static void EduiPrintAlignedTextPc(void *font, i32 x, i32 y, i32 alignment, char *format, ...) {
    if (edui_donotdraw == 0) {
        char text[1024];
        va_list args;
        i32 width;
        i32 left;
        NuQFntPushPrintMode(2);
        va_start(args, format);
        NuVSPrintf(text, format, args);
        va_end(args);
        width = (i32)NuQFntPrintLenU((VUFNT *)font, text);
        switch (alignment) {
            case 16:
                left = x;
                break;
            case 32:
                left = x - width;
                break;
            case 64:
                left = x - width / 2;
                break;
            default:
                left = x;
                break;
        }
        NuQFntMove((VUFNT *)font, (f32)left, (f32)y, 0.0f);
        NuQFntPrintU((VUFNT *)font, text);
        NuQFntPopPrintMode();
    }
}

extern "C" f32 EduiScaleXPc;
extern "C" f32 EduiScaleYPc;

/* Prints at a 640x448 position, scaled to the 2D renderer's sub-pixels. */
extern "C" void __cdecl EduiFntPrintScaledPc(void *font, f32 x, f32 y, i32 alignment, char *text) {
    EduiPrintAlignedTextPc(font, (i32)(x * EduiScaleXPc), (i32)(y * EduiScaleYPc), alignment, text);
}

i32 __cdecl eduicbRenderSel(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 unused_height) {
    i32 height = (i32)(NuQFntHeight((VUFNT *)edui_font) * 0.15625);
    f64 font_height = NuQFntHeight((VUFNT *)edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline((VUFNT *)edui_font));
    item->x = x;
    item->y = y;
    if (edui_donotdraw == 0) {
        NuRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2 + item->highlighted],
                      (NUMTL *)uimtls[ui_bgmtl]);
        if (edui_donotdraw == 0) {
            NuQFntSet((VUFNT *)edui_font);
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, item->colours[item->highlighted]);
        }
    }
    switch (item->text_alignment) {
        case 16:
            EduiPrintAlignedTextPc(edui_font, x << 4, (y << 3) + baseline, item->text_alignment, item->text);
            break;
        case 32:
            EduiPrintAlignedTextPc(edui_font, (x + width) << 4, (y << 3) + baseline, item->text_alignment, item->text);
            break;
        default:
            EduiPrintAlignedTextPc(edui_font, (x * 2 + width) << 3, (y << 3) + baseline, item->text_alignment,
                                   item->text);
            break;
    }
    return height;
}

extern "C" void __cdecl NuRndrLineStrip2di(i32 *positions, f32 *uvs, i32 count, i32 colour, NUMTL *material);
extern "C" void __cdecl NuRndrTriStrip2di(i32 *positions, f32 *uvs, i32 count, i32 colour, NUMTL *material);

i32 __cdecl eduicbRenderCheck(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 unused_selected) {
    i32 points[10];
    i32 height = (i32)(NuQFntHeight((VUFNT *)edui_font) * 1.25) >> 3;
    i32 baseline = (i32)(NuQFntHeight((VUFNT *)edui_font) * 0.125 + NuQFntBaseline((VUFNT *)edui_font));
    item->x = x;
    item->y = y;
    if (edui_donotdraw == 0) {
        NuRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2], (NUMTL *)uimtls[ui_bgmtl]);
        if (edui_donotdraw == 0) {
            NuQFntSet((VUFNT *)edui_font);
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, item->colours[item->highlighted]);
        }
    }
    EduiPrintAlignedTextPc(edui_font, x << 4, (y << 3) + baseline, 16, item->text);
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
        if (edui_donotdraw == 0)
            NuRndrTriStrip2di(points, NULL, 4, item->colours[0], (NUMTL *)uimtls[0]);
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
        if (edui_donotdraw == 0)
            NuRndrLineStrip2di(points, NULL, 5, item->colours[0], (NUMTL *)uimtls[0]);
    }
    return height;
}

i32 __cdecl eduicbRenderSlider(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width,
                               i32 unused_selected) {
    edui_slider_s *slider = (edui_slider_s *)item;
    i32 height = (i32)(NuQFntHeight((VUFNT *)edui_font) * 1.25) >> 3;
    f64 font_height = NuQFntHeight((VUFNT *)edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline((VUFNT *)edui_font));
    i32 skip_draw = edui_donotdraw;
    char format[512];
    item->x = x;
    item->y = y;
    if (skip_draw == 0) {
        NuRndrRect2di(x << 4, y << 3, width << 4, height << 4, item->colours[2 + item->highlighted],
                      (NUMTL *)uimtls[ui_bgmtl]);
        if (edui_donotdraw == 0) {
            NuQFntSet((VUFNT *)edui_font);
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, item->colours[item->highlighted]);
        }
    }
    NuStrCpy(format, item->text);
    NuStrCat(format, slider->format);
    EduiPrintAlignedTextPc(edui_font, (x * 2 + width) << 3, (y << 3) + baseline, 64, format, slider->value);
    if (slider->normalized_value >= 0.0f && slider->normalized_value <= 1.0f) {
        if (edui_donotdraw == 0) {
            i32 top = (y + height) << 3;
            i32 bottom = (y + height * 2 - 1) << 3;
            f32 span = (f32)(width - 2);
            i32 marker = (i32)(slider->normalized_value * span + (x + 1)) << 4;
            NuRndrLine2di(marker, top, marker, bottom, item->colours[item->highlighted], (NUMTL *)uimtls[0]);
            if (edui_donotdraw == 0) {
                marker = (i32)(slider->normalized_value * span + x) << 4;
                NuRndrLine2di(marker, top, marker, bottom, item->colours[item->highlighted], (NUMTL *)uimtls[0]);
                if (edui_donotdraw == 0) {
                    marker = (i32)(slider->normalized_value * span + (x - 1)) << 4;
                    NuRndrLine2di(marker, top, marker, bottom, item->colours[item->highlighted], (NUMTL *)uimtls[0]);
                }
            }
        }
    }
    return height * 2;
}

i32 __cdecl eduicbRenderSliderInt(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width,
                                  i32 unused_selected) {
    edui_slider_s *slider = (edui_slider_s *)item;
    i32 height = (i32)(NuQFntHeight((VUFNT *)edui_font) * 1.25) >> 3;
    f64 font_height = NuQFntHeight((VUFNT *)edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline((VUFNT *)edui_font));
    i32 skip_draw = edui_donotdraw;
    char format[512];
    item->x = x;
    item->y = y;
    if (skip_draw == 0) {
        NuRndrRect2di(x << 4, y << 3, width << 4, height << 4, item->colours[2 + item->highlighted],
                      (NUMTL *)uimtls[0]);
        if (edui_donotdraw == 0) {
            NuQFntSet((VUFNT *)edui_font);
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, item->colours[item->highlighted]);
        }
    }
    NuStrCpy(format, item->text);
    NuStrCat(format, slider->format);
    EduiPrintAlignedTextPc(edui_font, (x * 2 + width) << 3, (y << 3) + baseline, 64, format, (i32)slider->value);
    {
        if (edui_donotdraw == 0) {
            i32 top = (y + height) << 3;
            i32 bottom = (y + height * 2 - 1) << 3;
            f32 span = (f32)(width);
            i32 marker = (i32)(span * slider->normalized_value + (x + 1)) << 4;
            NuRndrLine2di(marker, top, marker, bottom, item->colours[item->highlighted], (NUMTL *)uimtls[0]);
            if (edui_donotdraw == 0) {
                marker = (i32)(span * slider->normalized_value + x) << 4;
                NuRndrLine2di(marker, top, marker, bottom, item->colours[item->highlighted], (NUMTL *)uimtls[0]);
                if (edui_donotdraw == 0) {
                    marker = (i32)(span * slider->normalized_value + (x - 1)) << 4;
                    NuRndrLine2di(marker, top, marker, bottom, item->colours[item->highlighted], (NUMTL *)uimtls[0]);
                }
            }
        }
    }
    return height * 2;
}

i32 __cdecl eduicbRenderFilePick(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width,
                                 i32 unused_selected) {
    edui_file_pick_s *picker = (edui_file_pick_s *)item;
    i32 height = (i32)(NuQFntHeight((VUFNT *)edui_font) * 1.25) >> 3;
    f64 font_height = NuQFntHeight((VUFNT *)edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline((VUFNT *)edui_font));
    if (picker->reserved_54 != 0) {
        i32 centre;
        if (edui_donotdraw == 0) {
            NuRndrRect2di(x << 4, y << 3, width << 4, height << 4, item->colours[2 + item->highlighted],
                          (NUMTL *)uimtls[ui_bgmtl]);
            if (edui_donotdraw == 0) {
                NuQFntSet((VUFNT *)edui_font);
                if (edui_donotdraw == 0)
                    NuQFntSetColour((VUFNT *)edui_font, item->colours[item->highlighted]);
            }
        }
        centre = (x * 2 + width) << 3;
        EduiPrintAlignedTextPc(edui_font, centre, (y << 3) + baseline, 64, item->text);
        EduiPrintAlignedTextPc(edui_font, centre, ((y + height) << 3) + baseline, 64, picker->format, picker->name);
        return height * 2;
    } else {
        char formatted[256];
        if (edui_donotdraw == 0)
            NuRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2 + item->highlighted],
                          (NUMTL *)uimtls[ui_bgmtl]);
        NuSPrintf(formatted, picker->format, picker->name);
        if (edui_donotdraw == 0) {
            NuQFntSet((VUFNT *)edui_font);
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, item->colours[item->highlighted]);
        }
        EduiPrintAlignedTextPc(edui_font, (x * 2 + width) << 3, (y << 3) + baseline, 64, "%s %s", item->text,
                               formatted);
        return height;
    }
}

void __cdecl eduiItemColourPickSetHSV(edui_colour_pick_s *item, f32 hue, f32 saturation, f32 value) {
    item->hue = hue;
    item->saturation = saturation;
    item->value = value;
    item->cursor_x = (f32)((f64)hue / 360.0);
    item->cursor_y = value;
}

extern "C" i32 EduiSelectionMaterialPc;
extern "C" void __cdecl NuRndrGradRect2di(i32 x, i32 y, i32 width, i32 height, u32 *colours, NUMTL *material);
extern "C" void __cdecl NuRndrLineRect2di(i32 x, i32 y, i32 width, i32 height, i32 colour, NUMTL *material);

static void EduiDrawGradientSpanPc(i32 x, i32 y, i32 width, i32 height, u32 first_colour, u32 second_colour, f32 start,
                                   f32 end);

i32 __cdecl eduicbRenderColourSlider(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width,
                                     i32 unused_selected) {
    edui_colour_slider_s *slider = (edui_colour_slider_s *)item;
    i32 height = (i32)(NuQFntHeight((VUFNT *)edui_font) * 1.25) >> 3;
    f64 font_height = NuQFntHeight((VUFNT *)edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline((VUFNT *)edui_font));
    i32 skip_draw = edui_donotdraw;
    item->x = x;
    item->y = y;
    if (skip_draw == 0) {
        NuRndrRect2di(x << 4, y << 3, width << 4, height << 4, item->colours[2 + item->highlighted],
                      (NUMTL *)uimtls[0]);
        if (edui_donotdraw == 0) {
            NuQFntSet((VUFNT *)edui_font);
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, item->colours[item->highlighted]);
        }
    }
    EduiPrintAlignedTextPc(edui_font, (x + 64) << 4, (y << 3) + baseline, 64, item->text);
    EduiPrintAlignedTextPc(edui_font, (x + 116) << 4, (y << 3) + baseline, 64, slider->base.format,
                           (i32)slider->base.value);
    u32 colour = slider->red + (slider->green << 8) + (slider->blue << 16);
    EduiDrawGradientSpanPc(x, y + height, width, height, 0xff000000, colour, 0.0f, 1.0f);
    if (edui_donotdraw == 0) {
        f32 span = (f32)width;
        i32 top = (y + height) << 3;
        i32 bottom = ((y + height * 2) << 3) - 8;
        i32 marker = (i32)(slider->base.normalized_value * span + (x + 1)) << 4;
        NuRndrLine2di(marker, top, marker, bottom, item->colours[item->highlighted], (NUMTL *)uimtls[0]);
        if (edui_donotdraw == 0) {
            marker = (i32)(slider->base.normalized_value * span + x) << 4;
            NuRndrLine2di(marker, top, marker, bottom, item->colours[item->highlighted], (NUMTL *)uimtls[0]);
            if (edui_donotdraw == 0) {
                marker = (i32)(slider->base.normalized_value * span + (x - 1)) << 4;
                NuRndrLine2di(marker, top, marker, bottom, item->colours[item->highlighted], (NUMTL *)uimtls[0]);
            }
        }
    }
    return height * 2;
}

i32 __cdecl eduicbRenderGradPick(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width,
                                 i32 unused_selected) {
    edui_gradient_pick_s *gradient = (edui_gradient_pick_s *)item;
    i32 height = (i32)(NuQFntHeight((VUFNT *)edui_font) * 1.25) >> 2;
    f64 font_height = NuQFntHeight((VUFNT *)edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline((VUFNT *)edui_font));
    item->x = x;
    item->y = y;
    if (edui_donotdraw == 0) {
        NuRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2 + item->highlighted],
                      (NUMTL *)uimtls[ui_bgmtl]);
        if (edui_donotdraw == 0) {
            NuQFntSet((VUFNT *)edui_font);
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, item->colours[item->highlighted]);
        }
    }
    if (gradient->change_timer && gradient->selected_stage) {
        EduiPrintAlignedTextPc(edui_font, (x * 2 + width) << 3, (y << 3) + baseline, 64, "%1.03f",
                               gradient->selected_stage->time);
        --gradient->change_timer;
    } else {
        EduiPrintAlignedTextPc(edui_font, (x * 2 + width) << 3, (y << 3) + baseline, 64, item->text);
    }
    edui_gradient_node_s *first = gradient->first_stage;
    if (first != NULL) {
        f32 stage_time = first->time;
        u32 stage_colour = first->colour;
        edui_gradient_node_s *next = first->next;
        while (next != NULL) {
            EduiDrawGradientSpanPc(x, y + (height >> 1), width, height >> 1, stage_colour, next->colour, stage_time,
                                   next->time);
            stage_time = next->time;
            stage_colour = next->colour;
            next = next->next;
        }
    }
    edui_gradient_node_s *stage = gradient->first_stage;
    if (stage != NULL) {
        f32 span = (f32)width;
        do {
            i32 offset = (i32)(stage->time * span);
            i32 marker_x = (x + offset - 2) << 4;
            i32 marker_y = ((y + (height >> 1)) << 3) - 16;
            i32 marker_height = ((height >> 1) << 3) + 16;
            if (edui_donotdraw == 0)
                NuRndrRect2di(marker_x, marker_y, 80, marker_height, stage->colour, (NUMTL *)uimtls[0]);
            if (stage == gradient->selected_stage) {
                if (edui_donotdraw == 0)
                    NuRndrLineRect2di(marker_x, marker_y, 80, marker_height, 0x80ffffff,
                                      (NUMTL *)uimtls[EduiSelectionMaterialPc]);
            } else {
                if (edui_donotdraw == 0)
                    NuRndrLineRect2di(marker_x, marker_y, 80, marker_height, 0x80000000,
                                      (NUMTL *)uimtls[EduiSelectionMaterialPc]);
            }
            stage = stage->next;
        } while (stage != NULL);
    }
    return height;
}

extern "C" i32 __cdecl eduicbRenderGreyPick(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width,
                                            i32 unused_selected) {
    edui_colour_pick_s *pick = (edui_colour_pick_s *)item;
    i32 height = width / 4;
    item->x = x;
    item->y = y;
    EduiDrawGradientSpanPc(x, y, width, height, 0x80000000, 0x80ffffff, 0.0f, 1.0f);
    if (edui_donotdraw == 0) {
        f32 span = (f32)(width - 2);
        f32 plus_x = (f32)(x + 1);
        i32 top = y << 3;
        i32 middle_y = (y + (height >> 1)) << 3;
        i32 bottom = ((y + height) << 3) - 8;
        i32 marker = (i32)(pick->value * span + plus_x) << 4;
        NuRndrLine2di(marker, top, marker, middle_y - 8, 0x80ffffff, (NUMTL *)uimtls[0]);
        if (edui_donotdraw == 0) {
            f32 middle_x = (f32)x;
            marker = (i32)(pick->value * span + middle_x) << 4;
            NuRndrLine2di(marker, top, marker, middle_y - 8, 0x80ffffff, (NUMTL *)uimtls[0]);
            if (edui_donotdraw == 0) {
                f32 minus_x = (f32)(x - 1);
                marker = (i32)(pick->value * span + minus_x) << 4;
                NuRndrLine2di(marker, top, marker, middle_y - 8, 0x80ffffff, (NUMTL *)uimtls[0]);
                if (edui_donotdraw == 0) {
                    marker = (i32)(pick->value * span + plus_x) << 4;
                    NuRndrLine2di(marker, middle_y, marker, bottom, 0x80000000, (NUMTL *)uimtls[0]);
                    if (edui_donotdraw == 0) {
                        marker = (i32)(pick->value * span + middle_x) << 4;
                        NuRndrLine2di(marker, middle_y, marker, bottom, 0x80000000, (NUMTL *)uimtls[0]);
                        if (edui_donotdraw == 0) {
                            marker = (i32)(pick->value * span + minus_x) << 4;
                            NuRndrLine2di(marker, middle_y, marker, bottom, 0x80000000, (NUMTL *)uimtls[0]);
                        }
                    }
                }
            }
        }
    }
    return height;
}

static void EduiDrawGradientSpanPc(i32 x, i32 y, i32 width, i32 height, u32 first_colour, u32 second_colour, f32 start,
                                   f32 end) {
    u32 colours[4];
    width <<= 4;
    colours[0] = colours[2] = first_colour;
    colours[1] = colours[3] = second_colour;
    x <<= 4;
    i32 left = (i32)(width * start) + x;
    if (edui_donotdraw == 0) {
        i32 right = (i32)(width * end) + x;
        NuRndrGradRect2di(left, y << 3, right - left, height << 3, colours, (NUMTL *)uimtls[ui_bgmtl]);
    }
}

static void eduiHSVToRGB(f32 hue, f32 saturation, f32 value, f32 &red, f32 &green, f32 &blue);

extern "C" void __cdecl eduiGradStageSetHSV(edui_gradient_node_s *stage, f32 hue, f32 saturation, f32 value) {
    f32 red, green, blue;
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
        if (hue == 360.0f)
            hue = 0.0f;
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

extern "C" i32 __cdecl eduicbRenderNumber(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width,
                                          i32 unused_selected) {
    edui_slider_s *number = (edui_slider_s *)item;
    i32 height = (i32)(NuQFntHeight((VUFNT *)edui_font) * 1.25) >> 3;
    f64 scaled_height = NuQFntHeight((VUFNT *)edui_font) * 0.125;
    i32 baseline = (i32)(scaled_height + NuQFntBaseline((VUFNT *)edui_font));
    item->x = x;
    item->y = y;
    if (edui_donotdraw == 0) {
        NuRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2 + item->highlighted],
                      (NUMTL *)uimtls[ui_bgmtl]);
        if (edui_donotdraw == 0) {
            NuQFntSet((VUFNT *)edui_font);
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, item->colours[item->highlighted]);
        }
    }
    char format[512];
    NuStrCpy(format, item->text);
    NuStrCat(format, number->format);
    EduiPrintAlignedTextPc(edui_font, x << 4, (y << 3) + baseline, 16, format, number->value);
    return height;
}

extern "C" eduiitem_s *__cdecl eduiItemNumberCreate(u32 data, void *colours, i32 group, void *callback, f32 minimum,
                                                    f32 range, f32 value, char *text) {
    eduiitem_s *item = eduiItemSliderCreate(data, colours, group, callback, minimum, range, value, text);
    if (item != NULL)
        item->render = eduicbRenderNumber;
    return item;
}

struct EduiTextSelectorViewPc {
    edui_slider_s slider;
    char **options;
};
STATIC_ASSERT(sizeof(EduiTextSelectorViewPc) == 112);
STATIC_ASSERT(offsetof(EduiTextSelectorViewPc, options) == 0x6c);

extern "C" i32 __cdecl eduicbRenderTextSelector(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width,
                                                i32 unused_selected) {
    EduiTextSelectorViewPc *selector = (EduiTextSelectorViewPc *)item;
    i32 height = (i32)(NuQFntHeight((VUFNT *)edui_font) * 1.25) >> 3;
    f64 scaled_height = NuQFntHeight((VUFNT *)edui_font) * 0.125;
    i32 baseline = (i32)(scaled_height + NuQFntBaseline((VUFNT *)edui_font));
    item->x = x;
    item->y = y;
    if (edui_donotdraw == 0) {
        NuRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2 + item->highlighted],
                      (NUMTL *)uimtls[ui_bgmtl]);
        if (edui_donotdraw == 0) {
            NuQFntSet((VUFNT *)edui_font);
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, item->colours[item->highlighted]);
        }
    }
    char format[512];
    NuStrCpy(format, item->text);
    NuStrCat(format, ": %s");
    EduiPrintAlignedTextPc(edui_font, x << 4, (y << 3) + baseline, 16, format,
                           selector->options[(i32)selector->slider.value]);
    return height;
}

extern "C" const char EduiSourcePathPc[52];

extern "C" eduiitem_s *__cdecl eduiItemTextSelectorCreate(u32 data, void *colours, i32 group, void *callback, i32 count,
                                                          i32 value, char *text, char **options) {
    EduiTextSelectorViewPc *item =
        (EduiTextSelectorViewPc *)NuAlloc(sizeof(EduiTextSelectorViewPc), EduiSourcePathPc, 0x10e4);
    if (item) {
        memset(item, 0, sizeof(EduiTextSelectorViewPc));
        item->slider.base.data = data;
        item->slider.base.type = 6;
        memcpy(item->slider.base.colours, colours, sizeof(item->slider.base.colours));
        item->slider.base.text_alignment = 0x40;
        item->slider.base.selection_group = group;
        item->slider.base.process = eduicbProcessSlider;
        item->slider.base.render = eduicbRenderTextSelector;
        item->slider.base.destroy = eduicbItemSliderDestroy;
        item->slider.interact = eduicbInteractSlider;
        eduiItemSetText(&item->slider.base, text);
        item->slider.minimum = 0.0f;
        item->slider.range = (f32)(count - 1);
        item->slider.granularity = 1.0f;
        item->slider.changed = (void(__cdecl *)(eduimenu_s *, eduiitem_s *, u32))callback;
        item->slider.format = NULL;
        f32 initial = (f32)value;
        item->slider.normalized_value = (initial - item->slider.minimum) / item->slider.range;
        item->slider.value = initial;
        item->slider.change_timer = 60;
        if (item->slider.changed)
            item->slider.changed(NULL, &item->slider.base, 0);
        item->options = options;
        return &item->slider.base;
    }
    return NULL;
}

struct EduiSeparatorViewPc {
    eduiitem_s base;
    i32(__cdecl *interact)(edui_interact_s *);
};
STATIC_ASSERT(sizeof(EduiSeparatorViewPc) == 76);
STATIC_ASSERT(offsetof(EduiSeparatorViewPc, interact) == 0x48);

extern "C" i32 __cdecl eduicbProcessSeparator(eduimenu_s *unused_menu, eduiitem_s *unused_item, f32 unused_delta_time,
                                              FRAME_PAD_PC *unused_pad) {
    return 0;
}

extern "C" i32 __cdecl eduicbRenderSeparator(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width,
                                             i32 unused_selected) {
    NuQFntHeight((VUFNT *)edui_font);
    NuQFntBaseline((VUFNT *)edui_font);
    i32 skip_draw = edui_donotdraw;
    item->x = x;
    item->y = y;
    if (skip_draw == 0) {
        NuRndrRect2di(x << 4, y << 3, width << 4, 64, item->colours[2 + item->highlighted], (NUMTL *)uimtls[ui_bgmtl]);
        if (edui_donotdraw == 0)
            NuRndrLine2di((x + 4) << 4, (y << 3) + 32, ((x + width) << 4) - 64, (y << 3) + 32, (i32)0xff000000,
                          (NUMTL *)uimtls[ui_bgmtl]);
    }
    return 8;
}

extern "C" eduiitem_s *__cdecl eduiItemSeparatorCreate(u32 data, const void *colours) {
    EduiSeparatorViewPc *item = (EduiSeparatorViewPc *)NuAlloc(sizeof(EduiSeparatorViewPc), EduiSourcePathPc, 0x1362);
    if (item != NULL) {
        memset(item, 0, sizeof(*item));
        item->base.data = data;
        item->base.type = 18;
        memcpy(item->base.colours, colours, sizeof(item->base.colours));
        item->base.highlighted = 0;
        item->base.text_alignment = 0x40;
        item->base.selection_group = 0;
        item->base.process = eduicbProcessSeparator;
        item->base.render = eduicbRenderSeparator;
        item->base.destroy = eduicbItemDestroy;
        item->interact = NULL;
        return &item->base;
    }
    return NULL;
}

extern "C" void __cdecl eduiGradStageDelete(edui_gradient_pick_s *item, edui_gradient_node_s *stage) {
    if (stage->previous != NULL)
        stage->previous->next = stage->next;
    else
        item->first_stage = stage->next;
    if (stage->next != NULL)
        stage->next->previous = stage->previous;
    if (stage->next != NULL)
        item->selected_stage = stage->next;
    else
        item->selected_stage = stage->previous;
    NuFree(stage, EduiSourcePathPc, 0xa70);
}

extern "C" void __cdecl eduicbItemGradPickDestroy(eduimenu_s *unused_menu, eduiitem_s *item) {
    edui_gradient_pick_s *gradient = (edui_gradient_pick_s *)item;
    while (gradient->first_stage != NULL)
        eduiGradStageDelete(gradient, gradient->first_stage);
    NuFree(gradient, EduiSourcePathPc, 0xa9b);
}

extern "C" void __cdecl cbgpcfgDel(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    edui_gradient_pick_s *picker = (edui_gradient_pick_s *)item->data_ptr;
    if (picker->remove != NULL)
        picker->remove(menu, item, value);
    if (picker->selected_stage != NULL)
        eduiGradStageDelete(picker, picker->selected_stage);
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

struct EDUI_COLOUR_CLIPBOARD_PC {
    f32 hue;
    f32 saturation;
    f32 value;
    f32 gray;
};
STATIC_ASSERT(sizeof(EDUI_COLOUR_CLIPBOARD_PC) == 16);
STATIC_ASSERT(offsetof(EDUI_COLOUR_CLIPBOARD_PC, saturation) == 4);
STATIC_ASSERT(offsetof(EDUI_COLOUR_CLIPBOARD_PC, value) == 8);
STATIC_ASSERT(offsetof(EDUI_COLOUR_CLIPBOARD_PC, gray) == 12);
extern "C" {
    EDUI_COLOUR_CLIPBOARD_PC EduiColourClipboardPc = {0.0f, 0.0f, 0.0f, 0.0f};
}

extern "C" void __cdecl cbgpcfgCopy(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    edui_gradient_pick_s *picker = (edui_gradient_pick_s *)item->data_ptr;
    edui_gradient_node_s *stage = picker->selected_stage;
    if (stage != NULL) {
        i32 type = item->type;
        if (type == 7) {
            EduiColourClipboardPc.hue = stage->hue;
            EduiColourClipboardPc.saturation = stage->saturation;
            EduiColourClipboardPc.value = stage->value;
        }
        if (type == 8)
            EduiColourClipboardPc.gray = stage->value;
        if (picker->copy != NULL)
            picker->copy(menu, item, value);
    }
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

extern "C" void __cdecl cbgpcfgPaste(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    edui_gradient_pick_s *picker = (edui_gradient_pick_s *)item->data_ptr;
    edui_gradient_node_s *stage = picker->selected_stage;
    if (stage != NULL) {
        if (item->type == 7)
            eduiGradStageSetHSV(stage, EduiColourClipboardPc.hue, EduiColourClipboardPc.saturation,
                                EduiColourClipboardPc.value);
        if (item->type == 8)
            eduiGradStageSetHSV(picker->selected_stage, 0.0f, 0.0f, EduiColourClipboardPc.gray);
        if (picker->paste != NULL)
            picker->paste(menu, item, value);
    }
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

extern "C" void __cdecl cbgpcfgCPPress(eduimenu_s *menu, eduiitem_s *item, u32 value) {
    edui_gradient_pick_s *picker = (edui_gradient_pick_s *)item->data_ptr;
    if (picker->selected_stage != NULL) {
        if (item->type == 10 || item->type == 11) {
            edui_colour_pick_s *colour = (edui_colour_pick_s *)item;
            eduiGradStageSetHSV(picker->selected_stage, colour->hue, colour->saturation, colour->value);
        }
        if (picker->press != NULL)
            picker->press(menu, item, value);
    }
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

/* SOURCE ONLY: append to the existing edui_widget_render_pc.cpp. The Native
 * register-argument helpers belong with their real caller in this existing
 * drawing unit. No ABI attributes, alignment forcing or caller activation.
 * New colour cells have unknown Native allocation
 * extents and zero allocation credit. All other state owners are reused. */
extern "C" {
    extern f32 edui_font_scale_x;
    extern f32 edui_font_scale_y;
    extern i32 EduiSelectionMaterialPc;
    i32 __cdecl EduiRenderItemPc(eduiitem_s *, eduimenu_s *, i32, i32, i32, i32);
    u32 eduimenu_outline_colour = 0x80404040;
    u32 eduimenu_selected_outline_colour = 0x80ffffff;
}
STATIC_ASSERT(offsetof(eduimenu_s, x) == 0x14);
STATIC_ASSERT(offsetof(eduimenu_s, child) == 0x3c);
STATIC_ASSERT(offsetof(eduimenu_s, callback) == 0x30);
STATIC_ASSERT(offsetof(eduiitem_s, render) == 0x40);
STATIC_ASSERT(offsetof(edui_sel_s, interact) == 0x48);
STATIC_ASSERT(sizeof(edui_interact_s) == 0x24);
STATIC_ASSERT(offsetof(edui_interact_s, menu) == 0x10);
STATIC_ASSERT(offsetof(edui_interact_s, callback) == 0x18);
STATIC_ASSERT(offsetof(edui_interact_s, field_20) == 0x20);

extern "C" {
    i32 __cdecl cbInteractMenuTitle(edui_interact_s *);
    i32 __cdecl cbInteractMenuScrollUp(edui_interact_s *);
    i32 __cdecl cbInteractMenuScrollDown(edui_interact_s *);
    i32 __cdecl cbInteractMenuCancelChild(edui_interact_s *);
}

static void EduiAddInteractionPc(f32 x, f32 y, f32 width, f32 height, eduimenu_s *menu, eduiitem_s *item,
                                 i32(__cdecl *callback)(edui_interact_s *), u32 buttons, i32 field_20) {
    if (numInteracts < 64 && eduiInteractLocked == NULL) {
        eduiInteracts[numInteracts].x = x;
        eduiInteracts[numInteracts].y = (f32)(y * 0.5);
        eduiInteracts[numInteracts].width = width;
        eduiInteracts[numInteracts].height = (f32)(height * 0.5);
        eduiInteracts[numInteracts].menu = menu;
        eduiInteracts[numInteracts].item = item;
        eduiInteracts[numInteracts].callback = callback;
        eduiInteracts[numInteracts].buttons = buttons;
        eduiInteracts[numInteracts].field_20 = field_20;
        ++numInteracts;
    }
}

static void EduiPrintClippedTextPc(void *font, i32 x, i32 y, i32 alignment, i32 clip_x, i32 clip_width, char *format,
                                   ...) {
    if (edui_donotdraw == 0) {
        char text[1024];
        va_list args;
        NuQFntPushPrintMode(2);
        va_start(args, format);
        NuVSPrintf(text, format, args);
        va_end(args);
        i32 width = (i32)NuQFntPrintLenU((VUFNT *)font, text);
        i32 left;
        switch (alignment) {
            case 16:
                left = x;
                break;
            case 32:
                left = x - width;
                break;
            case 64:
                left = x - width / 2;
                break;
            default:
                left = x;
                break;
        }
        if (left < clip_x)
            left = clip_x;
        char *end = text + NuStrLen(text) - 1;
        while (width > clip_width && end > text) {
            *end-- = 0;
            width = (i32)NuQFntPrintLenU((VUFNT *)font, text);
        }
        NuQFntMove((VUFNT *)font, (f32)left, (f32)y, 0.0f);
        NuQFntPrintU((VUFNT *)font, text);
        NuQFntPopPrintMode();
    }
}

extern "C" void __cdecl eduiMenuRender(eduimenu_s *menu) {
    i32 x = menu->x;
    i32 y = menu->y;
    i32 max_item_height = 0;
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    NuQFntPushCoordinateSystem(1);
    NuQFntPushPrintMode(2);
    if (edui_donotdraw == 0)
        NuQFntSet((VUFNT *)edui_font);
    NuQFntSetScale((VUFNT *)edui_font, edui_font_scale_x, edui_font_scale_y);
    i32 title_height = (i32)(NuQFntHeight((VUFNT *)edui_font) * 0.15625);
    f64 font_height = NuQFntHeight((VUFNT *)edui_font) * 0.125;
    i32 baseline = (i32)(NuQFntBaseline((VUFNT *)edui_font) + font_height);
    if (menu->title != NULL && menu->title[0] != 0) {
        if (menu->child == NULL)
            EduiAddInteractionPc((f32)x, (f32)y, (f32)menu->width, (f32)title_height, menu, NULL, cbInteractMenuTitle,
                                 0x40, 0);
        if (edui_donotdraw == 0) {
            NuRndrRect2di(x << 4, y << 3, menu->width << 4, title_height << 3, 0x80000000, (NUMTL *)uimtls[ui_bgmtl]);
            if (edui_donotdraw == 0) {
                NuQFntSet((VUFNT *)edui_font);
                if (edui_donotdraw == 0)
                    NuQFntSetColour((VUFNT *)edui_font, 0x807f7f7f);
            }
        }
        EduiPrintClippedTextPc(edui_font, (x + 1) << 4, (y << 3) + baseline, 16, x << 4, menu->width << 4, menu->title);
        y += title_height;
    }
    eduiitem_s *item = menu->field_0c;
    if (item != NULL) {
        i32 find_selected = menu->selected != NULL;
        if (item != menu->first) {
            i32 scroll_height = (i32)(NuQFntHeight((VUFNT *)edui_font) * 0.125);
            if (menu->child == NULL)
                EduiAddInteractionPc((f32)x, (f32)y, (f32)menu->width, (f32)scroll_height, menu, NULL,
                                     cbInteractMenuScrollUp, 0x40, 0);
            if (edui_donotdraw == 0) {
                NuRndrRect2di(x << 4, y << 3, menu->width << 4, scroll_height << 3, 0x80000000,
                              (NUMTL *)uimtls[ui_bgmtl]);
                if (edui_donotdraw == 0) {
                    i32 centre = (x * 2 + menu->width) << 3;
                    i32 top = (y << 3) + 8;
                    i32 bottom = ((y + scroll_height) << 3) - 16;
                    NuRndrLine2di(centre, top, centre - ((scroll_height - 2) << 4), bottom, 0x80ffffff,
                                  (NUMTL *)uimtls[0]);
                    if (edui_donotdraw == 0) {
                        centre = (x * 2 + menu->width) << 3;
                        NuRndrLine2di(centre, top, centre + ((scroll_height - 2) << 4), bottom, 0x80ffffff,
                                      (NUMTL *)uimtls[0]);
                    }
                }
            }
            y += scroll_height;
        }
        while (item != NULL) {
            menu->field_10 = item;
            if (item == menu->selected)
                find_selected = 0;
            i32 item_height =
                EduiRenderItemPc(item, menu, x, y, menu->width, item == menu->selected && menu->child == NULL);
            if (item_height > max_item_height)
                max_item_height = item_height;
            if (menu->child == NULL)
                EduiAddInteractionPc((f32)x, (f32)y, (f32)menu->width, (f32)item_height, menu, item,
                                     ((edui_sel_s *)item)->interact, 0x40, 0);
            item = item->next;
            if (menu->flags & 2)
                x += menu->width;
            else
                y += item_height;
            if (item == NULL)
                break;
            if (menu->y + menu->height > y)
                continue;
            i32 scroll_height = (i32)(NuQFntHeight((VUFNT *)edui_font) * 0.125);
            if (menu->child == NULL)
                EduiAddInteractionPc((f32)x, (f32)y, (f32)menu->width, (f32)scroll_height, menu, NULL,
                                     cbInteractMenuScrollDown, 0x40, 0);
            if (edui_donotdraw == 0) {
                NuRndrRect2di(x << 4, y << 3, menu->width << 4, scroll_height << 3, 0x80000000,
                              (NUMTL *)uimtls[ui_bgmtl]);
                if (edui_donotdraw == 0) {
                    i32 bottom = (y + scroll_height - 2) << 3;
                    i32 top = (y << 3) + 8;
                    i32 centre = (x * 2 + menu->width) << 3;
                    NuRndrLine2di(centre, bottom, centre - ((scroll_height - 2) << 4), top, 0x80ffffff,
                                  (NUMTL *)uimtls[0]);
                    if (edui_donotdraw == 0) {
                        centre = (x * 2 + menu->width) << 3;
                        NuRndrLine2di(centre, bottom, centre + ((scroll_height - 2) << 4), top, 0x80ffffff,
                                      (NUMTL *)uimtls[0]);
                    }
                }
            }
            break;
        }
        if (find_selected && menu->field_0c->next != NULL)
            menu->field_0c = menu->field_0c->next;
    }
    if ((menu->flags & 2) == 0)
        x = menu->x + menu->width;
    else
        y += max_item_height;
    menu->field_24 = x - menu->x;
    menu->field_28 = y - menu->y;
    if (active_menu == menu) {
        if (edui_donotdraw == 0)
            NuRndrLineRect2di((menu->x - 1) << 4, (menu->y << 3) - 8, (menu->field_24 + 2) << 4,
                              (menu->field_28 << 3) + 16, eduimenu_selected_outline_colour,
                              (NUMTL *)uimtls[EduiSelectionMaterialPc]);
    } else {
        if (edui_donotdraw == 0)
            NuRndrLineRect2di((menu->x - 1) << 4, (menu->y << 3) - 8, (menu->field_24 + 2) << 4,
                              (menu->field_28 << 3) + 16, eduimenu_outline_colour,
                              (NUMTL *)uimtls[EduiSelectionMaterialPc]);
    }
    if (menu->child != NULL) {
        EduiAddInteractionPc((f32)menu->x, (f32)menu->y, (f32)menu->field_24, (f32)menu->field_28, menu, NULL,
                             cbInteractMenuCancelChild, 0x40, 1);
        eduiMenuRender(menu->child);
    }
    NuQFntPopPrintMode();
    NuQFntPopCoordinateSystem();
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}

STATIC_ASSERT(offsetof(edui_colour_pick_s, cursor_x) == 0x4c);
STATIC_ASSERT(offsetof(edui_colour_pick_s, hue) == 0x54);
STATIC_ASSERT(offsetof(edui_colour_pick_s, saturation) == 0x58);
STATIC_ASSERT(offsetof(edui_colour_pick_s, value) == 0x5c);
STATIC_ASSERT(offsetof(edui_colour_pick_s, red) == 0x60);
STATIC_ASSERT(offsetof(edui_colour_pick_s, green) == 0x64);
STATIC_ASSERT(offsetof(edui_colour_pick_s, blue) == 0x68);

extern "C" {
    static eduimenu_s *colourmenu = NULL;
    static edui_colour_pick_s *cp_item = NULL;
    static eduiitem_s *cp_info = NULL;
    static eduiitem_s *cp_copy = NULL;
    static eduiitem_s *cp_paste = NULL;
    static i32(__cdecl *cp_process)(eduimenu_s *, eduiitem_s *, f32, FRAME_PAD_PC *) = NULL;
    static f32 *EduiColourTargetRedPc = NULL;
    static f32 *EduiColourTargetGreenPc = NULL;
    static f32 *EduiColourTargetBluePc = NULL;
    static f32 *EduiColourTargetAlphaPc = NULL;
    static u32 *EduiColourTargetPackedPc = NULL;
    static f32 EduiColourClipboardRedPc = 0.5f;
    static f32 EduiColourClipboardGreenPc = 0.5f;
    static f32 EduiColourClipboardBluePc = 0.5f;
    i32 EduiColourPickerInitializedPc = 0;

    static void EduiColourRgbToHsvPc(f32 red, f32 green, f32 blue, f32 *hue, f32 *saturation, f32 *value) {
        f32 maximum = red > (green > blue ? green : blue) ? red : (green > blue ? green : blue);
        f32 minimum = red < (green < blue ? green : blue) ? red : (green < blue ? green : blue);
        *value = maximum;
        *saturation = maximum != 0.0f ? (maximum - minimum) / maximum : 0.0f;
        if (*saturation != 0.0f) {
            f32 delta = maximum - minimum;
            if (red == maximum)
                *hue = (green - blue) / delta;
            else if (green == maximum)
                *hue = (f32)(2.0 + (blue - red) / delta);
            else if (blue == maximum)
                *hue = (f32)(4.0 + (red - green) / delta);
            *hue = (f32)(*hue * 60.0);
            if (*hue < 0.0f)
                *hue = (f32)(*hue + 360.0);
        } else {
            *hue = 0.0f;
        }
    }
}

extern "C" void __cdecl eduiGradStageSetRGB(edui_gradient_node_s *stage, f32 red, f32 green, f32 blue) {
    f32 hue, saturation, value;
    EduiColourRgbToHsvPc(red, green, blue, &hue, &saturation, &value);
    stage->colour = (u32)red | ((u32)green << 8) | ((u32)blue << 16) | 0x80000000u;
    stage->hue = hue;
    stage->saturation = saturation;
    stage->value = value;
}

void __cdecl eduiItemColourPickSetRGB(edui_colour_pick_s *item, f32 red, f32 green, f32 blue) {
    EduiColourRgbToHsvPc(red, green, blue, &item->hue, &item->saturation, &item->value);
    item->cursor_x = (f32)(item->hue / 360.0);
    item->cursor_y = item->value;
}

i32 __cdecl cbProcessColourPick(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD_PC *pad) {
    edui_colour_pick_s *picker = cp_item;
    i32 result = cp_process(menu, item, delta_time, pad);
    char text[256];
    if (cp_info->data != 0)
        NuSPrintf(text, "R:%1.2f G:%1.2f B:%1.2f", picker->red, picker->green, picker->blue);
    else
        NuSPrintf(text, "H:%1.2f S:%1.2f V:%1.2f", picker->hue, picker->saturation, picker->value);
    eduiItemSetText(cp_info, text);
    return result;
}

/* Copies the picker's target colour to the clipboard and tints the Paste button with it. Retail calls
   __ftol2_sse for each channel; VC8 inlines the conversions here because the result is stored to the u32
   eduiitem_s::colours (an i32 destination keeps the __ftol2_sse calls), which caps the score. */
void __cdecl cbCopy(eduimenu_s *, eduiitem_s *, u32) {
    EduiColourClipboardRedPc = *EduiColourTargetRedPc;
    EduiColourClipboardGreenPc = *EduiColourTargetGreenPc;
    EduiColourClipboardBluePc = *EduiColourTargetBluePc;
    u32 packed = 0x80;
    packed = (packed << 8) | ((i32)(EduiColourClipboardBluePc * 255.0) & 0xff);
    packed = (packed << 8) | ((i32)(EduiColourClipboardGreenPc * 255.0) & 0xff);
    packed = (packed << 8) | ((i32)(EduiColourClipboardRedPc * 255.0) & 0xff);
    cp_paste->colours[2] = packed;
}

void __cdecl cbPaste(eduimenu_s *, eduiitem_s *, u32) {
    *EduiColourTargetRedPc = EduiColourClipboardRedPc;
    *EduiColourTargetGreenPc = EduiColourClipboardGreenPc;
    *EduiColourTargetBluePc = EduiColourClipboardBluePc;
    if (EduiColourTargetPackedPc != NULL) {
        u32 packed = (u32)(*EduiColourTargetAlphaPc * 255.0);
        packed = (packed << 8) | ((u32)(*EduiColourTargetBluePc * 255.0) & 255);
        packed = (packed << 8) | ((u32)(*EduiColourTargetGreenPc * 255.0) & 255);
        packed = (packed << 8) | ((u32)(*EduiColourTargetRedPc * 255.0) & 255);
        *EduiColourTargetPackedPc = packed;
    }
    eduiItemColourPickSetRGB(cp_item, EduiColourClipboardRedPc, EduiColourClipboardGreenPc, EduiColourClipboardBluePc);
}

void __cdecl cbToggleIndicatorMode(eduimenu_s *, eduiitem_s *item, u32) {
    char text[256];
    item->data = 1 - item->data;
    edui_colour_pick_s *picker = cp_item;
    if (item->data != 0)
        NuSPrintf(text, "R:%1.2f G:%1.2f B:%1.2f", picker->red, picker->green, picker->blue);
    else
        NuSPrintf(text, "H:%1.2f S:%1.2f V:%1.2f", picker->hue, picker->saturation, picker->value);
    eduiItemSetText(item, text);
}

void __cdecl cbCancelSubMenu(eduimenu_s *menu, eduimenu_s *) {
    eduiMenuDetach(menu);
}

void __cdecl cbColourPickSel(eduimenu_s *menu, eduiitem_s *, u32) {
    edui_colour_pick_s *picker = cp_item;
    *EduiColourTargetRedPc = picker->red;
    *EduiColourTargetGreenPc = picker->green;
    f32 blue = picker->blue;
    *EduiColourTargetBluePc = blue;
    if (EduiColourTargetPackedPc != NULL) {
        u32 packed = (u32)(*EduiColourTargetAlphaPc * 255.0);
        packed = (packed << 8) | ((u32)(blue * 255.0) & 255);
        packed = (packed << 8) | ((u32)(*EduiColourTargetGreenPc * 255.0) & 255);
        packed = (packed << 8) | ((u32)(*EduiColourTargetRedPc * 255.0) & 255);
        *EduiColourTargetPackedPc = packed;
    }
    eduiMenuDetach(menu);
}

extern "C" void __cdecl CreateColourPicker(void) {
    u32 colours[4] = {0x80000000, 0x800000ff, 0x80808080, 0x80f0f0f0};
    if (EduiColourPickerInitializedPc != 0)
        return;
    EduiColourPickerInitializedPc = 1;
    colourmenu = eduiMenuCreate(200, 70, 180, 250, NULL, cbCancelSubMenu, "Pick Colour");
    if (colourmenu != NULL) {
        eduiitem_s *item = eduiItemColourPickCreate(0, colours, cbColourPickSel, "");
        if (colourmenu->last != NULL)
            colourmenu->last->next = item;
        else
            colourmenu->first = item;
        item->previous = colourmenu->last;
        item->next = NULL;
        colourmenu->last = item;
        colourmenu->field_0c = colourmenu->first;
        edui_last_item = item;
        colourmenu->field_10 = NULL;
        cp_item = (edui_colour_pick_s *)item;
        cp_process = item->process;
        item->process = cbProcessColourPick;
        item = eduiItemSelCreate(1, colours, 0, 0, cbToggleIndicatorMode, "R:?? G:?? B:??");
        if (colourmenu->last != NULL)
            colourmenu->last->next = item;
        else
            colourmenu->first = item;
        item->previous = colourmenu->last;
        item->next = NULL;
        colourmenu->last = item;
        colourmenu->field_0c = colourmenu->first;
        edui_last_item = item;
        colourmenu->field_10 = NULL;
        cp_info = item;
        item = eduiItemSelCreate(1, colours, 0, 0, cbCopy, "Copy To Clipboard");
        if (colourmenu->last != NULL)
            colourmenu->last->next = item;
        else
            colourmenu->first = item;
        item->previous = colourmenu->last;
        item->next = NULL;
        colourmenu->last = item;
        colourmenu->field_0c = colourmenu->first;
        edui_last_item = item;
        colourmenu->field_10 = NULL;
        cp_copy = item;
        item = eduiItemSelCreate(1, colours, 0, 0, cbPaste, "Paste From Clipboard");
        if (colourmenu->last != NULL)
            colourmenu->last->next = item;
        else
            colourmenu->first = item;
        item->previous = colourmenu->last;
        item->next = NULL;
        colourmenu->last = item;
        colourmenu->field_0c = colourmenu->first;
        edui_last_item = item;
        colourmenu->field_10 = NULL;
        cp_paste = item;
    }
}

extern "C" {
#pragma section(".bss$EduiColourTempRedPc", read, write)
    __declspec(allocate(".bss$EduiColourTempRedPc")) f32 EduiColourTempRedPc = 0.0f;
#pragma section(".bss$EduiColourTempGreenPc", read, write)
    __declspec(allocate(".bss$EduiColourTempGreenPc")) f32 EduiColourTempGreenPc = 0.0f;
#pragma section(".bss$EduiColourTempBluePc", read, write)
    __declspec(allocate(".bss$EduiColourTempBluePc")) f32 EduiColourTempBluePc = 0.0f;
#pragma section(".bss$EduiColourTempAlphaPc", read, write)
    __declspec(allocate(".bss$EduiColourTempAlphaPc")) f32 EduiColourTempAlphaPc = 0.0f;
    void __cdecl eduiSetActiveMenu(eduimenu_s *menu);
}

extern "C" void __cdecl AddColourPick(eduimenu_s *parent, eduiitem_s *item, f32 *red, f32 *green, f32 *blue,
                                      u32 *rgba) {
    eduimenu_s *picker_menu = colourmenu;
    f32 *target_red = red;
    f32 *target_green = green;
    f32 *target_blue = blue;
    edui_colour_pick_s *picker;
    i32 bottom;
    f32 red_value, green_value, blue_value;
    char text[256];
    if (picker_menu->parent == NULL) {
        eduimenu_s *last = parent;
        while (last->child != NULL)
            last = last->child;
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
    EduiColourTargetPackedPc = rgba;
    EduiColourTargetAlphaPc = &EduiColourTempAlphaPc;
    if (rgba != NULL) {
        u8 *channels = (u8 *)rgba;
        EduiColourTempRedPc = (f32)(channels[0] / 255.0);
        EduiColourTempGreenPc = (f32)(channels[1] / 255.0);
        EduiColourTempBluePc = (f32)(channels[2] / 255.0);
        EduiColourTempAlphaPc = (f32)(channels[3] / 255.0);
        target_red = &EduiColourTempRedPc;
        target_green = &EduiColourTempGreenPc;
        target_blue = &EduiColourTempBluePc;
    }
    picker = cp_item;
    blue_value = *target_blue;
    green_value = *target_green;
    EduiColourTargetBluePc = target_blue;
    red_value = *target_red;
    EduiColourTargetGreenPc = target_green;
    EduiColourTargetRedPc = target_red;
    EduiColourRgbToHsvPc(red_value, green_value, blue_value, &picker->hue, &picker->saturation, &picker->value);
    picker->cursor_x = (f32)(picker->hue / 360.0);
    picker->cursor_y = picker->value;
    NuSPrintf(text, "R:%1.2f G:%1.2f B:%1.2f", *target_red, *target_green, *target_blue);
    eduiItemSetText(cp_info, text);
}

STATIC_ASSERT(offsetof(edui_colour_pick_s, cursor_x) == 0x4c);
STATIC_ASSERT(offsetof(edui_colour_pick_s, cursor_y) == 0x50);
STATIC_ASSERT(offsetof(edui_colour_pick_s, hue) == 0x54);
STATIC_ASSERT(offsetof(edui_colour_pick_s, saturation) == 0x58);
STATIC_ASSERT(offsetof(edui_colour_pick_s, value) == 0x5c);

typedef union edui_gradient_colour_pc_u {
    u32 word;
    u8 channel[4];
} EDUI_GRADIENT_COLOUR_PC;
STATIC_ASSERT(sizeof(EDUI_GRADIENT_COLOUR_PC) == 4);

static void EduiDrawGradientGridPc(i32 x, i32 y, i32 width, i32 height, f32 start, f32 end, u32 top_left, u32 top_right,
                                   u32 bottom_left, u32 bottom_right, i32 steps) {
    width <<= 4;
    y <<= 3;
    height <<= 3;
    f32 scaled_width = (f32)width;
    i32 left = (x << 4) + (i32)(scaled_width * start);
    i32 right = (x << 4) + (i32)(scaled_width * end);
    EDUI_GRADIENT_COLOUR_PC first, second, last_first, last_second;
    first.word = top_left;
    second.word = top_right;
    last_first.word = bottom_left;
    last_second.word = bottom_right;
    EDUI_GRADIENT_COLOUR_PC colours[4];
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
        if (edui_donotdraw == 0)
            NuRndrGradRect2di(left, y + strip * height / steps, right - left, height / steps, &colours[0].word,
                              (NUMTL *)uimtls[ui_bgmtl]);
        colours[0] = colours[2];
        colours[1] = colours[3];
    }
}

extern "C" i32 __cdecl eduicbRenderColourPick(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width,
                                              i32 unused_selected) {
    edui_colour_pick_s *picker = (edui_colour_pick_s *)item;
    item->x = x;
    item->y = y;
    i32 main_height = width * 3 / 4;
    EduiDrawGradientGridPc(x, y, width, main_height, 0.0f, 1.0f / 6.0f, 0x80000000, 0x80000000, 0x800000ff, 0x8000ffff,
                           8);
    EduiDrawGradientGridPc(x, y, width, main_height, 1.0f / 6.0f, 2.0f / 6.0f, 0x80000000, 0x80000000, 0x8000ffff,
                           0x8000ff00, 8);
    EduiDrawGradientGridPc(x, y, width, main_height, 2.0f / 6.0f, 0.5f, 0x80000000, 0x80000000, 0x8000ff00, 0x80ffff00,
                           8);
    EduiDrawGradientGridPc(x, y, width, main_height, 0.5f, 4.0f / 6.0f, 0x80000000, 0x80000000, 0x80ffff00, 0x80ff0000,
                           8);
    EduiDrawGradientGridPc(x, y, width, main_height, 4.0f / 6.0f, 5.0f / 6.0f, 0x80000000, 0x80000000, 0x80ff0000,
                           0x80ff00ff, 8);
    EduiDrawGradientGridPc(x, y, width, main_height, 5.0f / 6.0f, 1.0f, 0x80000000, 0x80000000, 0x80ff00ff, 0x800000ff,
                           8);
    if (edui_donotdraw == 0) {
        f32 line_height = (f32)main_height;
        f32 line_y = (f32)y;
        NuRndrLine2di(x << 4, (i32)(line_y + (i32)line_height * picker->cursor_y) << 3, ((x + width) << 4) - 16,
                      (i32)(line_y + line_height * picker->cursor_y) << 3, 0x80ffffff, (NUMTL *)uimtls[0]);
        if (edui_donotdraw == 0) {
            i32 hue_x = (i32)(x + width * picker->cursor_x) << 4;
            NuRndrLine2di(hue_x, y << 3, hue_x, ((y + main_height) << 3) - 8, 0x80ffffff, (NUMTL *)uimtls[0]);
        }
    }
    i32 bar_height = width / 8;
    y += main_height;
    f32 red, green, blue;
    eduiHSVToRGB(picker->hue, 1.0f, picker->value, red, green, blue);
    u32 full = (u32)(red * 255.0) - (((((u32)(blue * -255.0) + 0x8000) << 8) + (u32)(green * -255.0)) << 8);
    eduiHSVToRGB(picker->hue, 0.0f, picker->value, red, green, blue);
    u32 grey = (u32)(red * 255.0) - (((((u32)(blue * -255.0) + 0x8000) << 8) + (u32)(green * -255.0)) << 8);
    EduiDrawGradientSpanPc(x, y, width, bar_height, grey, full, 0.0f, 1.0f);
    if (edui_donotdraw == 0) {
        i32 middle = (y * 2 + (bar_height & ~1)) << 2;
        f32 span = (f32)(width - 2);
        i32 marker = (i32)((f32)(x + 1) + picker->saturation * span) << 4;
        NuRndrLine2di(marker, y << 3, marker, middle - 8, 0x80ffffff, (NUMTL *)uimtls[0]);
        if (edui_donotdraw == 0) {
            marker = (i32)((f32)x + picker->saturation * span) << 4;
            NuRndrLine2di(marker, y << 3, marker, middle - 8, 0x80ffffff, (NUMTL *)uimtls[0]);
            if (edui_donotdraw == 0) {
                marker = (i32)((f32)(x - 1) + picker->saturation * span) << 4;
                NuRndrLine2di(marker, y << 3, marker, middle - 8, 0x80ffffff, (NUMTL *)uimtls[0]);
                if (edui_donotdraw == 0) {
                    marker = (i32)((f32)(x + 1) + picker->saturation * span) << 4;
                    NuRndrLine2di(marker, middle, marker, ((y + bar_height) << 3) - 8, 0x80000000, (NUMTL *)uimtls[0]);
                    if (edui_donotdraw == 0) {
                        marker = (i32)((f32)x + picker->saturation * span) << 4;
                        NuRndrLine2di(marker, middle, marker, ((y + bar_height) << 3) - 8, 0x80000000,
                                      (NUMTL *)uimtls[0]);
                        if (edui_donotdraw == 0) {
                            marker = (i32)((f32)(x - 1) + picker->saturation * span) << 4;
                            NuRndrLine2di(marker, middle, marker, ((y + bar_height) << 3) - 8, 0x80000000,
                                          (NUMTL *)uimtls[0]);
                        }
                    }
                }
            }
        }
    }
    eduiHSVToRGB(picker->hue, picker->saturation, picker->value, red, green, blue);
    u32 selected = (u32)(red * 255.0) - (((((u32)(blue * -255.0) + 0x8000) << 8) + (u32)(green * -255.0)) << 8);
    EduiDrawGradientSpanPc(x, y + bar_height, width, bar_height, selected, selected, 0.0f, 1.0f);
    return width;
}

typedef struct edui_colour_pick_flags_pc_s {
    eduiitem_s base;
    i32(__cdecl *interact)(edui_interact_s *);
    f32 cursor_x, cursor_y, hue, saturation, value, red, green, blue;
    union {
        u32 flags;
        struct {
            u32 dragging_colour : 1;
            u32 dragging_saturation : 1;
            u32 confirm : 1;
            u32 unknown_flags : 29;
        };
    };
    void(__cdecl *changed)(eduimenu_s *, eduiitem_s *, u32);
} EDUI_COLOUR_PICK_FLAGS_PC;
STATIC_ASSERT(sizeof(EDUI_COLOUR_PICK_FLAGS_PC) == 0x74);
STATIC_ASSERT(offsetof(EDUI_COLOUR_PICK_FLAGS_PC, cursor_x) == 0x4c);
STATIC_ASSERT(offsetof(EDUI_COLOUR_PICK_FLAGS_PC, saturation) == 0x58);
STATIC_ASSERT(offsetof(EDUI_COLOUR_PICK_FLAGS_PC, flags) == 0x6c);
STATIC_ASSERT(offsetof(EDUI_COLOUR_PICK_FLAGS_PC, changed) == 0x70);

extern "C" i32 __cdecl eduicbProcessColourPick(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD_PC *pad) {
    EDUI_COLOUR_PICK_FLAGS_PC *pick = (EDUI_COLOUR_PICK_FLAGS_PC *)item;
    if (!edui_cursor_buttons) {
        pick->dragging_colour = 0;
        pick->dragging_saturation = 0;
    }
    i32 dx = pad->analog_left_x - 128;
    if (dx > 0) {
        if (dx < 32)
            dx = 0;
        else
            dx = (dx - 32) * 255 / 223;
    } else {
        if (dx > -32)
            dx = 0;
        else
            dx = (dx + 32) * 255 / 223;
    }
    i32 dy = pad->analog_left_y - 128;
    if (dy > 0) {
        if (dy < 32)
            dy = 0;
        else
            dy = (dy - 32) * 255 / 223;
    } else {
        if (dy > -32)
            dy = 0;
        else
            dy = (dy + 32) * 255 / 223;
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
    if (((pad->digital_buttons_pressed & 0x40) || pick->confirm) && pick->changed)
        pick->changed(menu, item, pad->digital_buttons);
    return 0;
}

STATIC_ASSERT(offsetof(edui_interact_s, x) == 0);
STATIC_ASSERT(offsetof(edui_interact_s, y) == 4);
STATIC_ASSERT(offsetof(edui_interact_s, width) == 8);
STATIC_ASSERT(offsetof(edui_interact_s, height) == 0xc);
STATIC_ASSERT(offsetof(edui_interact_s, item) == 0x14);

static i32 EduiColourPickWithinHuePc(edui_interact_s *interact) {
    return edui_cursor_x >= interact->x && edui_cursor_y >= interact->y &&
           edui_cursor_x < interact->x + interact->width && edui_cursor_y < interact->y + interact->height * 0.75;
}

static i32 EduiColourPickWithinSaturationPc(edui_interact_s *interact) {
    return edui_cursor_x >= interact->x && edui_cursor_y >= interact->y + interact->height * 0.75 &&
           edui_cursor_x < interact->x + interact->width &&
           edui_cursor_y < interact->y + interact->height * 0.75 + interact->height * 0.125;
}

static i32 EduiColourPickWithinConfirmPc(edui_interact_s *interact) {
    return edui_cursor_x >= interact->x &&
           edui_cursor_y >= interact->y + interact->height * 0.75 + interact->height * 0.125 &&
           edui_cursor_x < interact->x + interact->width && edui_cursor_y < interact->y + interact->height;
}

extern "C" i32 __cdecl eduicbInteractColourPick(edui_interact_s *interact) {
    EDUI_COLOUR_PICK_FLAGS_PC *pick = (EDUI_COLOUR_PICK_FLAGS_PC *)interact->item;
    if ((EduiColourPickWithinHuePc(interact) || pick->dragging_colour) && !pick->dragging_saturation) {
        pick->cursor_x = (edui_cursor_x - interact->x) / interact->width;
        pick->cursor_y = (f32)((edui_cursor_y - interact->y) / (interact->height * 0.75));
        pick->dragging_colour = (edui_cursor_buttons & 0x40) != 0;
    }
    if ((EduiColourPickWithinSaturationPc(interact) || pick->dragging_saturation) && !pick->dragging_colour) {
        pick->saturation = (edui_cursor_x - interact->x) / interact->width;
        pick->dragging_saturation = (edui_cursor_buttons & 0x40) != 0;
    }
    pick->cursor_x = MIN(1.0f, MAX(0.0f, pick->cursor_x));
    pick->cursor_y = MIN(1.0f, MAX(0.0f, pick->cursor_y));
    pick->saturation = MIN(1.0f, MAX(0.0f, pick->saturation));
    pick->hue = (f32)(pick->cursor_x * 360.0);
    pick->value = pick->cursor_y;
    eduiHSVToRGB(pick->hue, pick->saturation, pick->value, pick->red, pick->green, pick->blue);
    if (EduiColourPickWithinConfirmPc(interact) && (edui_cursor_buttons_db & 0x40)) {
        pick->dragging_colour = 0;
        pick->dragging_saturation = 0;
        pick->confirm = 1;
    }
    return pick->dragging_colour || pick->dragging_saturation;
}

STATIC_ASSERT(offsetof(edui_texture_pick_s, uv_x) == 0x4c);
STATIC_ASSERT(offsetof(edui_texture_pick_s, uv_y) == 0x54);
STATIC_ASSERT(offsetof(edui_texture_pick_s, material) == 0x60);
STATIC_ASSERT(offsetof(edui_texture_pick_s, selected_corner) == 0x64);
STATIC_ASSERT(offsetof(edui_texture_pick_s, zoom) == 0x68);
STATIC_ASSERT(offsetof(edui_slider_s, changed) == 0x4c);
STATIC_ASSERT(offsetof(edui_slider_s, normalized_value) == 0x50);
STATIC_ASSERT(offsetof(edui_slider_s, value) == 0x54);
STATIC_ASSERT(offsetof(edui_slider_s, minimum) == 0x58);
STATIC_ASSERT(offsetof(edui_slider_s, range) == 0x5c);
STATIC_ASSERT(offsetof(edui_slider_s, change_timer) == 0x60);
STATIC_ASSERT(offsetof(edui_slider_s, granularity) == 0x68);

extern "C" {
#pragma data_seg(".data$EduiSliderAcceleration")
    f32 EduiSliderAccelerationPc = 1.0f;
#pragma data_seg(".data$EduiSliderRepeatTimer")
    i32 EduiSliderRepeatTimerPc = 30;
#pragma data_seg()
#pragma section(".bss$EduiSliderDynamicScale", read, write)
    __declspec(allocate(".bss$EduiSliderDynamicScale")) f32 EduiSliderDynamicScalePc = 0.0f;
#pragma section(".bss$EduiSliderGlobalAcceleration", read, write)
    __declspec(allocate(".bss$EduiSliderGlobalAcceleration")) f32 EduiSliderGlobalAccelerationPc = 0.0f;
#pragma data_seg(".data$EduiNominalFrameTime")
    f32 EduiNominalFrameTimePc = 1.0f / 60.0f;
#pragma data_seg()
#pragma const_seg(".rdata$EduiTextureValueFormat")
    extern const char EduiTextureValueFormatPc[] = "%1.03f,%1.03f";
#pragma const_seg()
}

extern "C" i32 __cdecl eduicbRenderTexturePick(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width,
                                               i32 unused_selected) {
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
    // MATCH-EXCEPTION: retail stores the second default colour first; an initializer or a chained
    // assignment either reverses the stores or reshuffles the x87 code above.
    colours[1] = 0x807f7f7f;
    colours[0] = 0x807f7f7f;
    colours[picker->selected_corner] = 0x80ffffff;
    if (edui_donotdraw == 0)
        NuRndrRectUV2di(x << 4, y << 3, width << 4, width << 3, origin_u, origin_v, origin_u + span_u,
                        origin_v + span_v, 0x80808080, picker->material);
    for (i32 corner = 0; corner < 2; ++corner) {
        if (edui_donotdraw == 0) {
            u32 colour = colours[corner];
            f32 line_width = (f32)width;
            f32 line_y = (f32)y;
            NuRndrLine2di(x << 4, (i32)(line_y + (i32)line_width * point_v[corner]) << 3, ((x + width) << 4) - 16,
                          (i32)(line_y + line_width * point_v[corner]) << 3, colour, (NUMTL *)uimtls[0]);
            if (edui_donotdraw == 0) {
                i32 marker_x = (i32)(x + line_width * point_u[corner]) << 4;
                NuRndrLine2di(marker_x, y << 3, marker_x, ((y + width) << 3) - 8, colour, (NUMTL *)uimtls[0]);
            }
        }
    }
    y += width;
    i32 height = (i32)(NuQFntHeight((VUFNT *)edui_font) * 1.25) >> 3;
    f64 font_height = NuQFntHeight((VUFNT *)edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline((VUFNT *)edui_font));
    if (edui_donotdraw == 0) {
        NuRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2 + item->highlighted],
                      (NUMTL *)uimtls[ui_bgmtl]);
        if (edui_donotdraw == 0) {
            NuQFntSet((VUFNT *)edui_font);
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, item->colours[item->highlighted]);
        }
    }
    EduiPrintAlignedTextPc(edui_font, (x * 2 + width) << 3, (y << 3) + baseline, 64, (char *)EduiTextureValueFormatPc,
                           picker->uv_x[picker->selected_corner] * 256.0,
                           picker->uv_y[picker->selected_corner] * 256.0);
    return width + height;
}

#define EDUI_GRAPH_MIN(a, b) ((a) < (b) ? (a) : (b))
#define EDUI_GRAPH_MAX(a, b) ((a) > (b) ? (a) : (b))

extern "C" void __cdecl eduiItemGraphAddOnionSkin(eduiitem_s *item, nugraph_s *graph) {
    edui_graph_s *picker = (edui_graph_s *)item;
    i32 slot;
    if (item->type == 15) {
        for (slot = 0; slot < 8; slot++) {
            if (picker->onion_skins[slot] == NULL) {
                picker->onion_skins[slot] = graph;
                return;
            }
        }
    }
}

extern "C" void __cdecl eduiItemGraphSetCursor(eduiitem_s *item, f32 x, f32 y) {
    edui_graph_s *picker = (edui_graph_s *)item;
    if (item->type == 15) {
        picker->cursor_x = EDUI_GRAPH_MIN(1.0f, EDUI_GRAPH_MAX(0.0f, x));
        picker->cursor_y = EDUI_GRAPH_MIN(1.0f, EDUI_GRAPH_MAX(0.0f, y));
    }
}

extern "C" void __cdecl eduiItemGraphSetLabels(eduiitem_s *item, char *x_label, char *y_label, char *title) {
    edui_graph_s *picker = (edui_graph_s *)item;
    if (item->type == 15) {
        if (x_label != NULL)
            NuStrCpy(picker->x_label, x_label);
        if (y_label != NULL)
            NuStrCpy(picker->y_label, y_label);
        if (title != NULL)
            NuStrCpy(picker->title, title);
    }
}

extern "C" {
    extern i32 show_x_scale_help;
    extern i32 show_y_scale_help;
    i32 __cdecl nugraphAddPoint(nugraph_s *graph, f32 x, f32 y);
    i32 __cdecl nugraphDeletePoint(nugraph_s *graph, i32 index);
    i32 __cdecl nugraphCalcCurve(nugraph_s *graph, i32 point_count);
}

/* Moves the graph cursor with the left stick, scales the axes with the shoulder buttons and edits points. */
extern "C" i32 __cdecl eduicbProcessGraph(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD_PC *pad) {
    edui_graph_s *picker = (edui_graph_s *)item;
    i32 stick_x = pad->analog_left_x - 128;
    i32 stick_y = pad->analog_left_y - 128;
    nugraph_s *graph;
    i32 index;
    (void)delta_time;
    show_y_scale_help = show_y_scale_help - 1 > 0 ? show_y_scale_help - 1 : 0;
    show_x_scale_help = show_x_scale_help - 1 > 0 ? show_x_scale_help - 1 : 0;
    if (stick_x > 0) {
        if (stick_x < 32)
            stick_x = 0;
        else
            stick_x = (stick_x - 32) * 255 / 223;
    } else if (stick_x > -32) {
        stick_x = 0;
    } else {
        stick_x = (stick_x + 32) * 255 / 223;
    }
    if (stick_y > 0) {
        if (stick_y < 32)
            stick_y = 0;
        else
            stick_y = (stick_y - 32) * 255 / 223;
    } else if (stick_y > -32) {
        stick_y = 0;
    } else {
        stick_y = (stick_y + 32) * 255 / 223;
    }
    picker->cursor_x = stick_x * (1.0 / 8192.0) + picker->cursor_x;
    picker->cursor_y = -stick_y * (1.0 / 8192.0) + picker->cursor_y;
    picker->cursor_x = EDUI_GRAPH_MIN(1.0f, EDUI_GRAPH_MAX(0.0f, picker->cursor_x));
    picker->cursor_y = EDUI_GRAPH_MIN(1.0f, EDUI_GRAPH_MAX(0.0f, picker->cursor_y));
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
    if ((pad->digital_buttons_pressed & 0x80) && picker->selected_point >= 0) {
        nugraphDeletePoint(picker->graph, picker->selected_point);
        nugraphCalcCurve(picker->graph, 100);
        return 1;
    }
    if (pad->digital_buttons_pressed & 0x20) {
        nugraphAddPoint(picker->graph, picker->cursor_x, picker->cursor_y);
        nugraphCalcCurve(picker->graph, 100);
        return 1;
    }
    if ((pad->digital_buttons_pressed & 0x40) && picker->selected_point >= 0) {
        picker->cursor_x = picker->graph->x[picker->selected_point];
        picker->cursor_y = picker->graph->y[picker->selected_point];
    }
    if ((pad->digital_buttons & 0x40) && picker->selected_point >= 0) {
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
    if ((pad->digital_buttons_pressed & 0x10) && picker->changed)
        picker->changed(menu, item, pad->digital_buttons_pressed);
    return 0;
}

extern "C" {
    i32 show_x_scale_help = 0;
    i32 show_y_scale_help = 0;
    i32 EduiGraphDrawCurvePc = 1;
    i32 EduiGraphDrawDebugPc = 0;
    i32 EduiGraphDrawDebugBoundPc = 0;
    i32 EduiGraphDebugIterationsPc = 3;
    i32 EduiGraphDebugBoundIterationsPc = 10;
    f32 __cdecl nugraphGetYatX(nugraph_s *graph, f32 x, i32 iterations);

    void __cdecl eduiRenderGraphLine(nugraph_s *graph, i32 x, i32 y, i32 width, i32 height, u32 colour) {
        if (EduiGraphDrawCurvePc != 0) {
            f32 fheight = (f32)height;
            i32 previous_x = 0;
            i32 previous_y = height - (i32)(graph->y[0] * fheight);
            f32 fraction = 0.0f;
            f32 fwidth = (f32)width;
            do {
                i32 current_x = (i32)(fwidth * fraction);
                i32 current_y =
                    height - (i32)(nugraphGetYatX(graph, fraction * graph->x_extent * graph->x_scale, 2) * fheight);
                if (edui_donotdraw == 0)
                    NuRndrLine2di((previous_x + x) << 4, (previous_y + y) << 3, (current_x + x) << 4,
                                  (current_y + y) << 3, colour, (NUMTL *)uimtls[0]);
                previous_x = current_x;
                previous_y = current_y;
                fraction += (f64)0.01f;
            } while (fraction <= 1.0f);
        }
        if (EduiGraphDrawDebugPc != 0) {
            i32 index;
            f32 fwidth = (f32)width;
            f32 fheight = (f32)height;
            for (index = 1; index < 100; ++index) {
                f32 t = index / 100.0;
                f32 bound;
                i32 px;
                i32 top;
                i32 bottom;
                if (EduiGraphDrawDebugBoundPc != 0)
                    bound = nugraphGetYatX(graph, t, EduiGraphDebugBoundIterationsPc);
                else
                    bound = 0.0f;
                px = (i32)(fwidth * t) + x;
                top = y - (i32)(nugraphGetYatX(graph, t, EduiGraphDebugIterationsPc) * fheight) + height;
                bottom = y - (i32)(fheight * bound) + height;
                if (edui_donotdraw == 0)
                    NuRndrLine2di(px << 4, top << 3, px << 4, bottom << 3, 0x80000080, (NUMTL *)uimtls[0]);
            }
        }
    }
}
STATIC_ASSERT(offsetof(edui_graph_s, graph) == 0x4c);
STATIC_ASSERT(offsetof(edui_graph_s, onion_skins) == 0x50);
STATIC_ASSERT(offsetof(edui_graph_s, cursor_x) == 0x74);
STATIC_ASSERT(offsetof(edui_graph_s, width) == 0x7c);
STATIC_ASSERT(offsetof(edui_graph_s, x_scale) == 0x84);
STATIC_ASSERT(offsetof(edui_graph_s, selected_point) == 0x8c);
STATIC_ASSERT(offsetof(edui_graph_s, x_label) == 0x90);
STATIC_ASSERT(offsetof(nugraph_s, point_count) == 1);
STATIC_ASSERT(offsetof(nugraph_s, y) == 0x24);
STATIC_ASSERT(offsetof(nugraph_s, x_scale) == 0x44);
STATIC_ASSERT(offsetof(nugraph_s, y_extent) == 0x50);

/* Float fabs by clearing the sign bit; retail inlines it this way. */
static __forceinline f32 EduiFabsPc(f32 value) {
    *(u32 *)&value &= 0x7fffffff;
    return value;
}

extern "C" i32 __cdecl eduicbRenderGraph(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 unused_width,
                                         i32 selected) {
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
    if (edui_donotdraw == 0) {
        NuRndrRect2di(x << 4, y << 3, total_width << 4, total_height << 3, 0x80000000, (NUMTL *)uimtls[0]);
        if (edui_donotdraw == 0)
            NuRndrRect2di(plot_x << 4, plot_y << 3, graph_width << 4, graph_height << 3, 0x80100000,
                          (NUMTL *)uimtls[0]);
    }
    if (picker->graph == NULL) {
        if (edui_donotdraw == 0) {
            NuQFntSet((VUFNT *)edui_font);
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, 0x80000080);
        }
        EduiPrintAlignedTextPc(edui_font, (x + total_width) << 3, (y + total_height) << 2, 64,
                               (char *)"Invalid Graph Pointer");
        return total_height;
    }
    if (edui_donotdraw == 0) {
        NuQFntSet((VUFNT *)edui_font);
        if (edui_donotdraw == 0)
            NuQFntSetColour((VUFNT *)edui_font, 0x80808080);
    }
    if (picker->x_scale > 0.0f) {
        f32 tick_count = picker->graph->x_extent * picker->graph->x_scale / picker->x_scale;
        i32 label_interval = 1 - (i32)(tick_count / -10.0);
        for (i32 tick = 1; tick <= (i32)tick_count; ++tick) {
            f32 tick_offset = (f32)(graph_width * tick) / tick_count;
            u32 colour;
            if (tick % label_interval == 0) {
                if (edui_donotdraw == 0)
                    NuQFntSet((VUFNT *)edui_font);
                EduiPrintAlignedTextPc(edui_font, (i32)(plot_x + tick_offset) << 4,
                                       ((plot_y + graph_height) << 3) - 48 + (i32)NuQFntHeight((VUFNT *)edui_font), 64,
                                       (char *)"%1.01f", (f32)tick * picker->x_scale);
                colour = 0x80340000;
            } else {
                colour = 0x80260000;
            }
            if (edui_donotdraw == 0) {
                i32 line_x = (i32)(plot_x + tick_offset) << 4;
                NuRndrLine2di(line_x, plot_y << 3, line_x, ((plot_y + graph_height) << 3) - 8, colour,
                              (NUMTL *)uimtls[0]);
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
                if (edui_donotdraw == 0)
                    NuQFntSet((VUFNT *)edui_font);
                EduiPrintAlignedTextPc(edui_font, (plot_x - 2) << 4,
                                       ((i32)((plot_y + graph_height) - tick_offset) << 3) +
                                           ((i32)NuQFntHeight((VUFNT *)edui_font) >> 1),
                                       32, (char *)"%1.01f", (f32)tick * picker->y_scale);
                colour = 0x80340000;
            } else {
                colour = 0x80260000;
            }
            if (edui_donotdraw == 0) {
                i32 line_y = (i32)((plot_y + graph_height) - tick_offset) << 3;
                NuRndrLine2di(plot_x << 4, line_y, (plot_x + graph_width - 1) << 4, line_y, colour, (NUMTL *)uimtls[0]);
            }
        }
    }
    if (edui_donotdraw == 0)
        NuQFntSet((VUFNT *)edui_font);
    EduiPrintAlignedTextPc(edui_font, (plot_x + graph_width) << 4, (y << 3) + (i32)NuQFntHeight((VUFNT *)edui_font), 32,
                           picker->x_label);
    EduiPrintAlignedTextPc(edui_font, (x + total_width - 3) << 4, (y + total_height) << 3, 32, picker->y_label);
    EduiPrintAlignedTextPc(edui_font, plot_x << 4, (y << 3) + (i32)NuQFntHeight((VUFNT *)edui_font), 16, picker->title);
    EduiPrintAlignedTextPc(edui_font, (plot_x - 2) << 4,
                           ((plot_y + graph_height) << 3) - 48 + (i32)NuQFntHeight((VUFNT *)edui_font), 32,
                           (char *)"0.0");
    u32 cursor_colour = 0x80303030;
    if (selected)
        cursor_colour = 0x80707070;
    if (edui_donotdraw == 0) {
        i32 cursor_line_y = (i32)(plot_y + cursor_y * graph_height) << 3;
        NuRndrLine2di(plot_x << 4, cursor_line_y, (plot_x + graph_width - 1) << 4, cursor_line_y, cursor_colour,
                      (NUMTL *)uimtls[0]);
        if (edui_donotdraw == 0) {
            i32 cursor_line_x = (i32)(plot_x + cursor_x * graph_width) << 4;
            NuRndrLine2di(cursor_line_x, plot_y << 3, cursor_line_x, ((plot_y + graph_height) << 3) - 8, cursor_colour,
                          (NUMTL *)uimtls[0]);
        }
    }
    picker->selected_point = -1;
    f32 closest = 1.0f;
    for (i32 point = 0; point < picker->graph->point_count; ++point) {
        i32 px = plot_x + (i32)(picker->graph->x[point] * (f32)graph_width);
        i32 py = plot_y - (i32)(picker->graph->y[point] * (f32)graph_height) + graph_height;
        if (edui_donotdraw == 0) {
            NuRndrLine2di((px - 10) << 4, py << 3, (px + 10) << 4, py << 3, 0x80505050, (NUMTL *)uimtls[0]);
            if (edui_donotdraw == 0)
                NuRndrLine2di(px << 4, (py << 3) - 80, px << 4, (py << 3) + 80, 0x80505050, (NUMTL *)uimtls[0]);
        }
        f32 dx = picker->cursor_x - picker->graph->x[point];
        f32 dy = picker->cursor_y - picker->graph->y[point];
        f32 distance = EduiFabsPc(dx) + EduiFabsPc(dy);
        if (distance < closest && distance < 0.05f) {
            picker->selected_point = point;
            closest = distance;
        }
    }
    for (i32 skin = 0; skin < 8; ++skin) {
        if (picker->onion_skins[skin] != NULL)
            eduiRenderGraphLine(picker->onion_skins[skin], plot_x, plot_y, graph_width, graph_height, 0x80402000);
    }
    eduiRenderGraphLine(picker->graph, plot_x, plot_y, graph_width, graph_height, 0x80800000);
    if (picker->selected_point >= 0 && edui_donotdraw == 0) {
        i32 point = picker->selected_point;
        NuRndrLineRect2di((plot_x + (i32)(picker->graph->x[point] * graph_width) - 6) << 4,
                          (((plot_y + graph_height) - (i32)(picker->graph->y[point] * graph_height)) << 3) - 48, 192,
                          96, 0x80ffffff, (NUMTL *)uimtls[EduiSelectionMaterialPc]);
    }
    if (show_x_scale_help || show_y_scale_help) {
        if (edui_donotdraw == 0)
            NuQFntSet((VUFNT *)edui_font);
        i32 help_y = (plot_y << 3) + (i32)NuQFntHeight((VUFNT *)edui_font);
        if (edui_donotdraw == 0)
            NuQFntSetColour((VUFNT *)edui_font, 0x80808080);
        EduiPrintAlignedTextPc(edui_font, (plot_x + (graph_width >> 1)) << 4, help_y, 64, (char *)"Scaling %c Axis",
                               show_x_scale_help ? 'X' : 'Y');
        help_y += (i32)NuQFntHeight((VUFNT *)edui_font);
        EduiPrintAlignedTextPc(edui_font, (plot_x + (graph_width >> 1)) << 4, help_y, 64, (char *)"%1.03f",
                               show_x_scale_help ? picker->graph->x_scale : picker->graph->y_scale);
        NuQFntHeight((VUFNT *)edui_font);
    }
    i32 label_height = (i32)(NuQFntHeight((VUFNT *)edui_font) * 1.125) >> 3;
    i32 result = total_height + label_height;
    i32 label_row = y + total_height;
    i32 label_baseline =
        (((label_height << 3) - (i32)NuQFntHeight((VUFNT *)edui_font)) >> 1) + (i32)NuQFntBaseline((VUFNT *)edui_font);
    if (edui_donotdraw == 0) {
        NuRndrRect2di(x << 4, label_row << 3, total_width << 4, label_height << 3, item->colours[2 + item->highlighted],
                      (NUMTL *)uimtls[ui_bgmtl]);
        if (edui_donotdraw == 0) {
            NuQFntSet((VUFNT *)edui_font);
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, item->colours[item->highlighted]);
        }
    }
    if (picker->selected_point >= 0) {
        i32 point = picker->selected_point;
        EduiPrintAlignedTextPc(edui_font, (x + 2) << 4, (label_row << 3) + label_baseline, 16,
                               (char *)"Point %d = %1.02f, %1.02f", point,
                               picker->graph->x[point] * picker->graph->x_scale * picker->graph->x_extent,
                               picker->graph->y[point] * picker->graph->y_scale * picker->graph->y_extent);
    }
    EduiPrintAlignedTextPc(edui_font, (x + total_width - 4) << 4, (label_row << 3) + label_baseline, 32,
                           (char *)"Cursor pos = %1.02f, %1.02f",
                           picker->cursor_x * picker->graph->x_scale * picker->graph->x_extent,
                           picker->cursor_y * picker->graph->y_scale * picker->graph->y_extent);
    return result;
}

extern "C" i32 __cdecl eduicbProcessSlider(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD_PC *pad) {
    edui_slider_s *slider = (edui_slider_s *)item;
    u32 buttons = pad->digital_buttons;
    if ((buttons & 4) && (buttons & 8)) {
        f32 value;
        if (slider->minimum <= 0.0f && 0.0f <= slider->minimum + slider->range)
            value = 0.0f;
        else if (slider->minimum <= 1.0f && 1.0f <= slider->minimum + slider->range)
            value = 1.0f;
        else {
            EduiSliderRepeatTimerPc = 30;
            return 0;
        }
        slider->value = value;
        slider->normalized_value = (value - slider->minimum) / slider->range;
        slider->change_timer = 60;
        if (slider->changed)
            slider->changed(menu, item, pad->digital_buttons);
        EduiSliderRepeatTimerPc = 30;
    } else if (buttons & 4) {
        i32 timer = EduiSliderRepeatTimerPc - 1;
        if (timer < 0)
            EduiSliderRepeatTimerPc = 0;
        else
            EduiSliderRepeatTimerPc = timer;
        if (!EduiSliderRepeatTimerPc || (pad->digital_buttons_pressed & 4)) {
            f64 step = (f64)slider->granularity * EduiSliderAccelerationPc;
            slider->value = MAX(slider->minimum, slider->value - step);
            if (slider->value > 0.0f)
                slider->value += step * 0.5;
            else
                slider->value -= step * 0.5;
            slider->value = step * (i32)(slider->value / step);
            slider->normalized_value = (slider->value - slider->minimum) / slider->range;
            slider->change_timer = 60;
            if (slider->changed)
                slider->changed(menu, item, pad->digital_buttons);
            EduiSliderAccelerationPc += EduiSliderGlobalAccelerationPc;
            return 0;
        }
    } else if (buttons & 8) {
        i32 timer = EduiSliderRepeatTimerPc - 1;
        if (timer < 0)
            EduiSliderRepeatTimerPc = 0;
        else
            EduiSliderRepeatTimerPc = timer;
        if (!EduiSliderRepeatTimerPc || (pad->digital_buttons_pressed & 8)) {
            f32 maximum = slider->minimum + slider->range;
            f64 step = (f64)slider->granularity * EduiSliderAccelerationPc;
            slider->value = MIN(maximum, slider->value + step);
            if (slider->value > 0.0f)
                slider->value += step * 0.5;
            else
                slider->value -= step * 0.5;
            slider->value = step * (i32)(slider->value / step);
            slider->normalized_value = (slider->value - slider->minimum) / slider->range;
            slider->change_timer = 60;
            if (slider->changed)
                slider->changed(menu, item, pad->digital_buttons);
            EduiSliderAccelerationPc += EduiSliderGlobalAccelerationPc;
            return 0;
        }
    } else {
        EduiSliderAccelerationPc = 1.0f;
        EduiSliderRepeatTimerPc = 30;
        u8 l2 = pad->analog_l2;
        if (l2) {
            if (buttons & 0x20) {
                EduiSliderDynamicScalePc *= (f64)1.05f;
                slider->normalized_value =
                    (f32)MAX(0.0f, slider->normalized_value -
                                       (u32)l2 * delta_time / EduiNominalFrameTimePc * EduiSliderDynamicScalePc);
            } else {
                EduiSliderDynamicScalePc = 1.627604291343232e-7f;
                slider->normalized_value =
                    (f32)MAX(0.0f, slider->normalized_value -
                                       (u32)l2 * delta_time / EduiNominalFrameTimePc * (f64)1.627604251552839e-5f);
            }
            slider->value = slider->normalized_value * slider->range + slider->minimum;
            slider->change_timer = 60;
            if (slider->changed)
                slider->changed(menu, item, pad->digital_buttons);
            return 0;
        }
        u8 r2 = pad->analog_r2;
        if (r2) {
            if (buttons & 0x20) {
                EduiSliderDynamicScalePc *= (f64)1.05f;
                slider->normalized_value =
                    (f32)MIN(1.0f, slider->normalized_value +
                                       (u32)r2 * delta_time / EduiNominalFrameTimePc * EduiSliderDynamicScalePc);
            } else {
                EduiSliderDynamicScalePc = 1.627604291343232e-7f;
                slider->normalized_value =
                    (f32)MIN(1.0f, slider->normalized_value +
                                       (u32)r2 * delta_time / EduiNominalFrameTimePc * (f64)1.627604251552839e-5f);
            }
            slider->value = slider->normalized_value * slider->range + slider->minimum;
            slider->change_timer = 60;
            if (slider->changed)
                slider->changed(menu, item, pad->digital_buttons);
        } else {
            EduiSliderDynamicScalePc = 1.627604291343232e-7f;
        }
    }
    return 0;
}

extern "C" i32 EduiRenderWidthPc;

STATIC_ASSERT(offsetof(edui_textpicker_s, value) == 0x4c);
STATIC_ASSERT(offsetof(edui_textpicker_s, keyboard_column) == 0x14c);
STATIC_ASSERT(offsetof(edui_textpicker_s, keyboard_row) == 0x150);
STATIC_ASSERT(offsetof(edui_textpicker_s, cursor) == 0x154);
STATIC_ASSERT(offsetof(edui_textpicker_s, editing) == 0x158);
STATIC_ASSERT(sizeof(((edui_textpicker_s *)0)->editing) == 2);
STATIC_ASSERT(offsetof(edui_textpicker_s, format) == 0x160);
STATIC_ASSERT(offsetof(edui_textpicker_s, keyboard_flags) == 0x164);

extern "C" {
#pragma const_seg(".rdata$EduiTextCopyHelpString")
    extern const char EduiTextCopyHelpStringPc[] = "Copy L2+R2 Paste L1+R1";
#pragma const_seg()
#pragma data_seg(".data$EduiTextCopyHelp")
    char *EduiTextCopyHelpPc = (char *)EduiTextCopyHelpStringPc;
#pragma data_seg(".data$EduiTextCursorPeriod")
    i32 EduiTextCursorPeriodPc = 32;
#pragma data_seg()
#pragma section(".bss$EduiTextCursorCountdown", read, write)
    __declspec(allocate(".bss$EduiTextCursorCountdown")) i32 EduiTextCursorCountdownPc = 0;
#pragma section(".bss$EduiTextCopyBuffer", read, write)
    __declspec(allocate(".bss$EduiTextCopyBuffer")) char EduiTextCopyBufferPc[64] = {0};
}

static void EduiRenderTextCursorPc(char *value, i32 position, i32 x, i32 y, u32 colour) {
    char at_cursor[2] = "@";
    if (--EduiTextCursorCountdownPc <= 0)
        EduiTextCursorCountdownPc = EduiTextCursorPeriodPc;
    if (EduiTextCursorCountdownPc >= (EduiTextCursorPeriodPc >> 1)) {
        i32 length = NuStrLen(value);
        i32 offset;
        if (position) {
            char prefix[256];
            NuStrNCpy(prefix, value, position + 1);
            offset = (i32)NuQFntPrintLenU((VUFNT *)edui_font, prefix);
        } else {
            offset = 0;
        }
        at_cursor[0] = position < length ? value[position] : 'A';
        i32 width = (i32)NuQFntPrintLenU((VUFNT *)edui_font, at_cursor);
        i32 top = y - (i32)NuQFntBaseline((VUFNT *)edui_font);
        i32 height = (i32)NuQFntHeight((VUFNT *)edui_font);
        if (edui_donotdraw == 0)
            NuRndrRect2di(x + offset, top, width, height, colour, NULL);
    }
}

extern "C" i32 __cdecl eduicbRenderTextPick(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width,
                                            i32 unused_selected) {
    edui_textpicker_s *picker = (edui_textpicker_s *)item;
    i32 height = (i32)(NuQFntHeight((VUFNT *)edui_font) * 1.25) >> 3;
    f64 font_height = NuQFntHeight((VUFNT *)edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline((VUFNT *)edui_font));
    item->x = x;
    item->y = y;
    char letter[2] = "0";
    if (picker->editing) {
        EduiRenderWidthPc = MAX(width, height * 10);
        if (picker->keyboard_flags & 2) {
            if (edui_donotdraw == 0)
                NuRndrRect2di(x << 4, y << 3, EduiRenderWidthPc << 4, (height * 6) << 3, item->colours[2],
                              (NUMTL *)uimtls[ui_bgmtl]);
        } else {
            if (edui_donotdraw == 0)
                NuRndrRect2di(x << 4, y << 3, EduiRenderWidthPc << 4, (height * 5) << 3, item->colours[2],
                              (NUMTL *)uimtls[ui_bgmtl]);
        }
        i32 row;
        for (row = 0; row < 4; ++row) {
            for (i32 column = 0; column < 10; ++column) {
                if (row == picker->keyboard_row && column == picker->keyboard_column) {
                    if (edui_donotdraw == 0) {
                        NuQFntSet((VUFNT *)edui_font);
                        if (edui_donotdraw == 0)
                            NuQFntSetColour((VUFNT *)edui_font, item->colours[1]);
                    }
                } else {
                    if (edui_donotdraw == 0) {
                        NuQFntSet((VUFNT *)edui_font);
                        if (edui_donotdraw == 0)
                            NuQFntSetColour((VUFNT *)edui_font, item->colours[0]);
                    }
                }
                letter[0] = textrow[row][column];
                EduiPrintAlignedTextPc(edui_font, (x + height * column) << 4, ((y + row * height) << 3) + baseline, 16,
                                       letter);
            }
        }
        if (edui_donotdraw == 0) {
            NuQFntSet((VUFNT *)edui_font);
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, item->colours[1]);
        }
        i32 value_y = ((y + height * row) << 3) + baseline;
        EduiPrintAlignedTextPc(edui_font, x << 4, value_y, 16, picker->value);
        EduiRenderTextCursorPc(picker->value, picker->cursor, x << 4, value_y, item->colours[1] ^ 0xffffff);
        ++row;
        if (picker->keyboard_flags & 2) {
            if (edui_donotdraw == 0) {
                NuQFntSet((VUFNT *)edui_font);
                if (edui_donotdraw == 0)
                    NuQFntSetColour((VUFNT *)edui_font, item->colours[0]);
            }
            EduiPrintAlignedTextPc(edui_font, x << 4, ((y + height * row) << 3) + baseline, 16,
                                   EduiTextCopyBufferPc[0] ? EduiTextCopyBufferPc : EduiTextCopyHelpPc);
            return height * 6;
        }
        return height * 5;
    }
    if (picker->keyboard_flags & 1) {
        if (edui_donotdraw == 0) {
            NuRndrRect2di(x << 4, y << 3, width << 4, height << 4, item->colours[2 + item->highlighted],
                          (NUMTL *)uimtls[ui_bgmtl]);
            if (edui_donotdraw == 0) {
                NuQFntSet((VUFNT *)edui_font);
                if (edui_donotdraw == 0)
                    NuQFntSetColour((VUFNT *)edui_font, item->colours[item->highlighted]);
            }
        }
        i32 centre = (x * 2 + width) << 3;
        EduiPrintAlignedTextPc(edui_font, centre, (y << 3) + baseline, 64, item->text);
        EduiPrintAlignedTextPc(edui_font, centre, ((y + height) << 3) + baseline, 64, picker->format, picker->value);
        return height * 2;
    }
    if (edui_donotdraw == 0)
        NuRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2 + item->highlighted],
                      (NUMTL *)uimtls[ui_bgmtl]);
    char formatted[64];
    NuSPrintf(formatted, picker->format, picker->value);
    if (edui_donotdraw == 0) {
        NuQFntSet((VUFNT *)edui_font);
        if (edui_donotdraw == 0)
            NuQFntSetColour((VUFNT *)edui_font, item->colours[item->highlighted]);
    }
    EduiPrintAlignedTextPc(edui_font, x << 4, (y << 3) + baseline, 16, "%s %s", item->text, formatted);
    return height;
}

STATIC_ASSERT(offsetof(FRAME_PAD_PC, reserved98) + 1 == 0x99);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_l2) == 0x9e);
STATIC_ASSERT(offsetof(FRAME_PAD_PC, analog_r2) == 0x9f);

extern "C" {
#pragma data_seg(".data$EduiColourSliderRepeatTimer")
    i32 EduiColourSliderRepeatTimerPc = 30;
#pragma data_seg()
#pragma section(".bss$EduiColourSliderDynamicScale", read, write)
    __declspec(allocate(".bss$EduiColourSliderDynamicScale")) f32 EduiColourSliderDynamicScalePc = 0.0f;
}

static void EduiColourSliderSetPc(edui_slider_s *slider, eduimenu_s *menu, eduiitem_s *item, f32 value,
                                  FRAME_PAD_PC *pad) {
    slider->value = value;
    slider->normalized_value = (value - slider->minimum) / slider->range;
    slider->change_timer = 60;
    if (slider->changed)
        slider->changed(menu, item, pad->digital_buttons);
}

static void EduiColourSliderRoundPc(edui_slider_s *slider, eduimenu_s *menu, eduiitem_s *item, FRAME_PAD_PC *pad,
                                    f32 value) {
    f32 step = slider->granularity;
    if (value > 0.0f)
        value += step * 0.5;
    else
        value -= step * 0.5;
    value = (i32)(value / step) * step;
    slider->value = value;
    slider->normalized_value = (value - slider->minimum) / slider->range;
    slider->change_timer = 60;
    if (slider->changed)
        slider->changed(menu, item, pad->digital_buttons);
}

extern "C" i32 __cdecl eduicbProcessColourSlider(eduimenu_s *menu, eduiitem_s *item, f32 unused_delta_time,
                                                 FRAME_PAD_PC *pad) {
    edui_slider_s *slider = (edui_slider_s *)item;
    if ((pad->digital_buttons & 4) && (pad->digital_buttons & 8)) {
        if (slider && slider->minimum <= 0.0f && slider->minimum + slider->range >= 0.0)
            EduiColourSliderSetPc(slider, menu, item, 0.0f, pad);
        EduiColourSliderRepeatTimerPc = 30;
    } else if ((pad->digital_buttons & 1) && (pad->digital_buttons & 2)) {
        if (slider && slider->minimum <= 0.0f && slider->minimum + slider->range >= 255.0)
            EduiColourSliderSetPc(slider, menu, item, 255.0f, pad);
        EduiColourSliderRepeatTimerPc = 30;
    } else if (pad->digital_buttons & 4) {
        EduiColourSliderRepeatTimerPc = (i32)((u32)EduiColourSliderRepeatTimerPc - 1u);
        if (EduiColourSliderRepeatTimerPc < 0)
            EduiColourSliderRepeatTimerPc = 0;
        if (!EduiColourSliderRepeatTimerPc || (pad->digital_buttons_pressed & 4)) {
            f32 value = MAX(slider->minimum, slider->value - slider->granularity);
            EduiColourSliderRoundPc(slider, menu, item, pad, value);
        }
    } else if (pad->digital_buttons & 8) {
        EduiColourSliderRepeatTimerPc = (i32)((u32)EduiColourSliderRepeatTimerPc - 1u);
        if (EduiColourSliderRepeatTimerPc < 0)
            EduiColourSliderRepeatTimerPc = 0;
        if (!EduiColourSliderRepeatTimerPc || (pad->digital_buttons_pressed & 8)) {
            f32 value = MIN(slider->minimum + slider->range, slider->value + slider->granularity);
            EduiColourSliderRoundPc(slider, menu, item, pad, value);
        }
    } else {
        EduiColourSliderRepeatTimerPc = 30;
        if (pad->analog_l2) {
            if (pad->reserved98[1]) {
                EduiColourSliderDynamicScalePc *= (f64)1.05f;
                slider->normalized_value =
                    MAX(0.0f, slider->normalized_value - (u32)pad->analog_l2 * EduiColourSliderDynamicScalePc);
            } else {
                EduiColourSliderDynamicScalePc = 1.627604291343232e-7f;
                slider->normalized_value =
                    (f32)MAX(0.0f, slider->normalized_value - (u32)pad->analog_l2 * (f64)1.627604251552839e-5f);
            }
            slider->value = slider->normalized_value * slider->range + slider->minimum;
            slider->change_timer = 60;
            if (slider->changed)
                slider->changed(menu, item, pad->digital_buttons);
        } else if (pad->analog_r2) {
            if (pad->reserved98[1]) {
                EduiColourSliderDynamicScalePc *= (f64)1.05f;
                slider->normalized_value =
                    MIN(1.0f, slider->normalized_value + (u32)pad->analog_r2 * EduiColourSliderDynamicScalePc);
            } else {
                EduiColourSliderDynamicScalePc = 1.627604291343232e-7f;
                slider->normalized_value =
                    (f32)MIN(1.0f, slider->normalized_value + (u32)pad->analog_r2 * (f64)1.627604251552839e-5f);
            }
            slider->value = slider->normalized_value * slider->range + slider->minimum;
            slider->change_timer = 60;
            if (slider->changed)
                slider->changed(menu, item, pad->digital_buttons);
        } else {
            EduiColourSliderDynamicScalePc = 1.627604291343232e-7f;
        }
    }
    return 0;
}

typedef struct edui_gradient_stage_pc_s {
    f32 time, hue, saturation, value, red, green, blue;
    u32 colour, metadata;
} EDUI_GRADIENT_STAGE_PC;
STATIC_ASSERT(sizeof(EDUI_GRADIENT_STAGE_PC) == 0x24);
STATIC_ASSERT(offsetof(EDUI_GRADIENT_STAGE_PC, time) == 0);
STATIC_ASSERT(offsetof(EDUI_GRADIENT_STAGE_PC, hue) == 4);
STATIC_ASSERT(offsetof(EDUI_GRADIENT_STAGE_PC, saturation) == 8);
STATIC_ASSERT(offsetof(EDUI_GRADIENT_STAGE_PC, value) == 0xc);
STATIC_ASSERT(offsetof(EDUI_GRADIENT_STAGE_PC, red) == 0x10);
STATIC_ASSERT(offsetof(EDUI_GRADIENT_STAGE_PC, green) == 0x14);
STATIC_ASSERT(offsetof(EDUI_GRADIENT_STAGE_PC, blue) == 0x18);
STATIC_ASSERT(offsetof(EDUI_GRADIENT_STAGE_PC, colour) == 0x1c);
STATIC_ASSERT(offsetof(EDUI_GRADIENT_STAGE_PC, metadata) == 0x20);
STATIC_ASSERT(offsetof(edui_gradient_node_s, hue) == 0x10);
STATIC_ASSERT(offsetof(edui_gradient_node_s, saturation) == 0x14);
STATIC_ASSERT(offsetof(edui_gradient_node_s, value) == 0x18);
STATIC_ASSERT(offsetof(edui_gradient_node_s, metadata) == 0x1c);
STATIC_ASSERT(offsetof(edui_gradient_pick_s, first_stage) == 0x4c);

extern "C" edui_gradient_node_s *__cdecl eduiGradStageAddRGB(edui_gradient_pick_s *item, f32 time, f32 red, f32 green,
                                                             f32 blue) {
    f32 hue, saturation, value;
    EduiColourRgbToHsvPc(red, green, blue, &hue, &saturation, &value);
    return eduiGradStageAdd(item, time, hue, saturation, value);
}

extern "C" i32 __cdecl eduiGradPickRead(eduiitem_s *item, EDUI_GRADIENT_STAGE_PC *stages, i32 capacity) {
    edui_gradient_node_s *node = ((edui_gradient_pick_s *)item)->first_stage;
    f32 red, green, blue;
    i32 count = 0;
    for (; node; node = node->next, ++count) {
        if (count < capacity) {
            EDUI_GRADIENT_STAGE_PC *stage = &stages[count];
            stage->time = node->time;
            stage->hue = node->hue;
            stage->saturation = node->saturation;
            stage->value = node->value;
            eduiHSVToRGB(node->hue, node->saturation, node->value, red, green, blue);
            stage->red = red;
            stage->green = green;
            stage->blue = blue;
            stage->colour =
                (u32)(red * 255.0) - (((((u32)(blue * -255.0) + 0x8000u) << 8) + (u32)(green * -255.0)) << 8);
            stage->metadata = node->metadata;
        }
    }
    return count;
}

extern "C" i32 __cdecl eduicbProcessGreyPick(eduimenu_s *menu, eduiitem_s *item, f32 delta_time, FRAME_PAD_PC *pad) {
    edui_colour_pick_s *pick = (edui_colour_pick_s *)item;
    i32 left = pad->analog_l1 ? pad->analog_l1 : pad->analog_l2;
    i32 right = pad->analog_r1 ? pad->analog_r1 : pad->analog_r2;
    (void)delta_time;
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
    if ((pad->digital_buttons_pressed & 0x40) && pick->changed)
        pick->changed(menu, item, pad->digital_buttons);
    return 0;
}

extern "C" i32 __cdecl eduicbRenderSelWithClipColour(eduimenu_s *menu, eduiitem_s *item, i32 x, i32 y, i32 width,
                                                     i32 selected) {
    i32 height = (i32)(NuQFntHeight((VUFNT *)edui_font) * 1.25) >> 3;
    f64 font_height = NuQFntHeight((VUFNT *)edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline((VUFNT *)edui_font));
    item->x = x;
    item->y = y;
    if (edui_donotdraw == 0) {
        NuRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2 + item->highlighted],
                      (NUMTL *)uimtls[ui_bgmtl]);
        if (edui_donotdraw == 0) {
            NuQFntSet((VUFNT *)edui_font);
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, item->colours[item->highlighted]);
        }
    }
    EduiPrintAlignedTextPc(edui_font, (x * 2 + width) << 3, (y << 3) + baseline, 64, item->text);
    f32 red, green, blue;
    if (item->type == 7)
        eduiHSVToRGB(EduiColourClipboardPc.hue, EduiColourClipboardPc.saturation, EduiColourClipboardPc.value, red,
                     green, blue);
    else
        eduiHSVToRGB(0.0f, 0.0f, EduiColourClipboardPc.gray, red, green, blue);
    i32 colour = ((u32)(blue * 255.0) << 16) + ((u32)(green * 255.0) << 8) + (u32)(red * 255.0) + 0x80000000;
    EduiDrawGradientSpanPc(x + width - 45, y, 40, height, colour, colour, 0.0f, 1.0f);
    return height;
}

extern "C" i32 near_clip_at_cursor;

extern "C" void __cdecl cbNearClipAtCursor(eduimenu_s *menu, eduiitem_s *item, u32 buttons) {
    (void)menu;
    (void)buttons;
    near_clip_at_cursor = item->flags & 1;
}
