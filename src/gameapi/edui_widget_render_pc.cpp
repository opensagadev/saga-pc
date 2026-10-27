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
        left = x;
        switch (alignment) {
            case 16:
                break;
            case 32:
                left = x - width;
                break;
            case 64:
                left = x - width / 2;
                break;
        }
        NuQFntMove((VUFNT *)font, (f32)left, (f32)y, 0.0f);
        NuQFntPrintU((VUFNT *)font, text);
        NuQFntPopPrintMode();
    }
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
    i8 alignment = item->text_alignment;
    char *text = item->text;
    switch (alignment) {
        case 16:
            EduiPrintAlignedTextPc(edui_font, x << 4, (y << 3) + baseline, alignment, text);
            break;
        case 32:
            EduiPrintAlignedTextPc(edui_font, (x + width) << 4, (y << 3) + baseline, alignment, text);
            break;
        default:
            EduiPrintAlignedTextPc(edui_font, (x * 2 + width) << 3, (y << 3) + baseline, alignment, text);
            break;
    }
    return height;
}

extern "C" void __cdecl NuRndrLineStrip2di(i32 *positions, f32 *uvs, i32 count, i32 colour, NUMTL *material);
extern "C" void __cdecl NuRndrTriStrip2di(i32 *positions, f32 *uvs, i32 count, i32 colour, NUMTL *material);

i32 __cdecl eduicbRenderCheck(eduimenu_s *unused_menu, eduiitem_s *item, i32 x, i32 y, i32 width, i32 unused_selected) {
    i32 height = (i32)(NuQFntHeight((VUFNT *)edui_font) * 1.25) >> 3;
    f64 font_height = NuQFntHeight((VUFNT *)edui_font) * 0.125;
    i32 baseline = (i32)(font_height + NuQFntBaseline((VUFNT *)edui_font));
    i32 skip_draw = edui_donotdraw;
    item->x = x;
    item->y = y;
    if (skip_draw == 0) {
        NuRndrRect2di(x << 4, y << 3, width << 4, height << 3, item->colours[2], (NUMTL *)uimtls[ui_bgmtl]);
        if (edui_donotdraw == 0) {
            NuQFntSet((VUFNT *)edui_font);
            if (edui_donotdraw == 0)
                NuQFntSetColour((VUFNT *)edui_font, item->colours[item->highlighted]);
        }
    }
    EduiPrintAlignedTextPc(edui_font, x << 4, (y << 3) + baseline, 16, item->text);
    i32 top = (y << 3) + 16;
    i32 left = ((x - height + width) << 4) - 16;
    i32 right = left + ((height - 3) << 4);
    i32 bottom = top + ((height - 3) << 3);
    i32 middle_y = (top + bottom) >> 1;
    i32 quarter_x = (left + ((left + right) >> 1)) >> 1;
    i32 lower_y = (bottom + middle_y) >> 1;
    if (item->highlighted) {
        i32 points[8] = {left, middle_y, quarter_x, lower_y, quarter_x, bottom, right, top};
        if (edui_donotdraw == 0)
            NuRndrTriStrip2di(points, NULL, 4, item->colours[0], (NUMTL *)uimtls[0]);
    } else {
        i32 points[10] = {left, middle_y, quarter_x, lower_y, right, top, quarter_x, bottom, left, middle_y};
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
    i32 skip_draw = edui_donotdraw;
    item->x = x;
    item->y = y;
    if (skip_draw == 0) {
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
            if (gradient->selected_stage == stage) {
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
    f32 scaled_width = (f32)(width << 4);
    u32 colours[4] = {first_colour, second_colour, first_colour, second_colour};
    i32 left = (i32)(scaled_width * start) + (x << 4);
    if (edui_donotdraw == 0) {
        i32 right = (i32)(scaled_width * end) + (x << 4);
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
        edui_interact_s *entry = &eduiInteracts[numInteracts];
        entry->x = x;
        entry->y = (f32)(y * 0.5);
        entry->width = width;
        entry->height = (f32)(height * 0.5);
        entry->menu = menu;
        entry->item = item;
        entry->callback = callback;
        entry->buttons = buttons;
        entry->field_20 = field_20;
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
        switch (alignment) {
            case 16:
                break;
            case 32:
                x -= width;
                break;
            case 64:
                x -= width / 2;
                break;
        }
        i32 left = x < clip_x ? clip_x : x;
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
    eduimenu_s *colourmenu = NULL;
    edui_colour_pick_s *cp_item = NULL;
    eduiitem_s *cp_info = NULL;
    eduiitem_s *cp_copy = NULL;
    eduiitem_s *cp_paste = NULL;
    i32(__cdecl *cp_process)(eduimenu_s *, eduiitem_s *, f32, FRAME_PAD_PC *) = NULL;
    f32 *EduiColourTargetRedPc = NULL;
    f32 *EduiColourTargetGreenPc = NULL;
    f32 *EduiColourTargetBluePc = NULL;
    f32 *EduiColourTargetAlphaPc = NULL;
    u32 *EduiColourTargetPackedPc = NULL;
    f32 EduiColourClipboardRedPc = 0.5f;
    f32 EduiColourClipboardGreenPc = 0.5f;
    f32 EduiColourClipboardBluePc = 0.5f;
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

void __cdecl cbPaste(eduimenu_s *, eduiitem_s *, u32) {
    f32 red = EduiColourClipboardRedPc;
    *EduiColourTargetRedPc = red;
    f32 green = EduiColourClipboardGreenPc;
    *EduiColourTargetGreenPc = green;
    f32 blue = EduiColourClipboardBluePc;
    *EduiColourTargetBluePc = blue;
    if (EduiColourTargetPackedPc != NULL) {
        u32 packed = (u32)(*EduiColourTargetAlphaPc * 255.0);
        packed = (packed << 8) | ((u32)(blue * 255.0) & 255);
        packed = (packed << 8) | ((u32)(*EduiColourTargetGreenPc * 255.0) & 255);
        packed = (packed << 8) | ((u32)(*EduiColourTargetRedPc * 255.0) & 255);
        *EduiColourTargetPackedPc = packed;
    }
    eduiItemColourPickSetRGB(cp_item, red, green, blue);
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
