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
