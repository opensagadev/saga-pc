#include "saga.h"

#include <stdarg.h>
#include <stdio.h>

extern u32 NuQFntModeStack[16];
extern i32 NuQFntModeStackIndex;
extern i32 NuQFntCSModeStack[16];
extern i32 NuQFntCSModeStackIndex;
extern void __cdecl NuQFntMoveRS(void *stream, VUFNT *font, f32 x, f32 y, f32 z);
extern void __cdecl NuQFntPrintU(VUFNT *font, char *text);
extern u16 __cdecl UnicodeToIndexFast(VUCHARIDX *map, i32 count, u16 unicode);

void __cdecl NuQFntSetPointSize(VUFNT *font, f32 width, f32 height) {
    u16 glyph_index;
    f32 x_scale;
    f32 y_scale;
    if (font == NULL) {
        font = system_qfont;
    }
    if (font == NULL) {
        return;
    }
    glyph_index = UnicodeToIndexFast(font->unicode_map, font->unicode_count, 'M');
    if (glyph_index == 0xffff) {
        y_scale = (f32)(height * 18.0 / font->height);
        x_scale = y_scale / height;
    } else {
        x_scale = (f32)(width * 7.0 / font->glyphs[glyph_index].width);
        y_scale = (f32)(height * 18.0 / font->height);
    }
    *font->x_scale = x_scale;
    if ((NuQFntMode & 4) != 0) {
        *font->y_scale = y_scale;
    } else {
        *font->y_scale = (f32)(y_scale * NuQFntHalfScale);
    }
    NuQFntSetUVSizePc(font, x_scale, y_scale);
}

i32 __cdecl NuQFntSetCoordinateSystem(i32 mode) {
    i32 previous = nuqfnt_cs_mode;
    nuqfnt_cs_mode = mode;
    switch (mode) {
        case 2:
            qfnt_rezscale_w = 1.0f;
            qfnt_rezscale_h = 0.5f;
            qfnt_offscale_x = 1.0f;
            qfnt_offscale_y = 1.0f;
            qfnt_len_scale = 1.0f;
            qfnt_height_scale = 1.0f;
            break;
        case 3:
            qfnt_rezscale_w = 320.0f;
            qfnt_rezscale_h = -112.0f;
            qfnt_offscale_x = 320.0f;
            qfnt_offscale_y = 112.0f;
            qfnt_len_scale = 0.003125f;
            qfnt_height_scale = -0.008928572f;
            break;
        case 4:
            qfnt_rezscale_w = 1.0f;
            qfnt_rezscale_h = 1.0f;
            qfnt_offscale_x = 0.0f;
            qfnt_offscale_y = 0.0f;
            qfnt_len_scale = 1.0f;
            qfnt_height_scale = -1.0f;
            break;
        default:
            qfnt_rezscale_w = 0.0625f;
            qfnt_rezscale_h = 0.0625f;
            qfnt_offscale_x = 0.0f;
            qfnt_offscale_y = 0.0f;
            qfnt_len_scale = 16.0f;
            qfnt_height_scale = 16.0f;
            break;
    }
    return previous;
}

void __cdecl NuQFntPushCoordinateSystem(i32 mode) {
    if (NuQFntCSModeStackIndex < 16) {
        NuQFntCSModeStack[NuQFntCSModeStackIndex++] = nuqfnt_cs_mode;
    }
    NuQFntSetCoordinateSystem(mode);
}

void __cdecl NuQFntPopCoordinateSystem(void) {
    if (NuQFntCSModeStackIndex > 0) {
        NuQFntSetCoordinateSystem(NuQFntCSModeStack[--NuQFntCSModeStackIndex]);
    }
}

i32 __cdecl NuQFntGetCoordinateSystem(void) {
    return nuqfnt_cs_mode;
}

f32 __cdecl NuQFntPrintLenW(VUFNT *font, u16 *text) {
    f32 x_scale;
    f32 length = 0.0f;
    u16 character;
    if (font == NULL) {
        font = system_qfont;
    }
    if (font == NULL) {
        return 0.0f;
    }
    x_scale = *font->x_scale;
    while ((character = *text++) != 0) {
        f32 width;
        volatile f32 gap;
        if (character >= '0' && character <= '9' && (NuQFntMode & 1) != 0) {
            u16 zero = NuQFntEncodeUnicodeChar(font, '0');
            width = font->glyphs[zero].width;
        } else if (character == 0xffff) {
            width = 0.0f;
        } else if (character == ' ' && nuqfnt_space_width != 0.0f) {
            width = nuqfnt_space_width;
        } else {
            width = font->glyphs[character].width;
        }
        gap = font->ic_gap;
        length = (gap + width) + length;
    }
    return length * qfnt_len_scale * x_scale;
}

f32 __cdecl NuQFntPrintLenU(VUFNT *font, char *text) {
    u8 *cursor;
    f32 x_scale;
    f32 length = 0.0f;
    if (font == NULL) {
        font = system_qfont;
    }
    if (font == NULL) {
        return 0.0f;
    }
    x_scale = *font->x_scale;
    cursor = (u8 *)text;
    while (*cursor != '\0') {
        u16 character;
        u16 qcode;
        f32 width;
        cursor = NuUnicodeCharFromUTF8(&character, cursor);
        qcode = NuQFntEncodeUnicodeChar(font, character);
        if (qcode == 0xffff) {
            qcode = NuQFntEncodeUnicodeChar(font, '?');
        }
        if (character == ' ' && nuqfnt_space_width != 0.0f) {
            width = nuqfnt_space_width;
        } else {
            if (character >= '0' && character <= '9' && (NuQFntMode & 1) != 0) {
                qcode = NuQFntEncodeUnicodeChar(font, '0');
            }
            width = font->glyphs[qcode].width;
        }
        length += width + font->ic_gap;
    }
    return length * qfnt_len_scale * x_scale;
}

f32 __cdecl NuQFntHeight(VUFNT *font) {
    if (font == NULL) {
        font = system_qfont;
    }
    if (font == NULL) {
        return 0.0f;
    }
    return *font->y_scale * font->height * qfnt_height_scale;
}

f32 __cdecl NuQFntBaseline(VUFNT *font) {
    if (font == NULL) {
        font = system_qfont;
    }
    if (font == NULL) {
        return 0.0f;
    }
    return *font->y_scale * font->baseline * qfnt_height_scale;
}

void __cdecl NuQFntSetJustifiedTolerances(f32 squash, f32 stretch) {
    qfnt_justify_squash = squash < 1.0f ? 1.0f : squash;
    qfnt_justify_stretch = stretch < 1.0f ? 1.0f : stretch;
}

void __cdecl NuQFntPrintEx(VUFNT *font, i32 x, i32 y, i32 alignment, const char *format, ...) {
    char text[1024];
    va_list arguments;
    i32 width;

    if (NuQFntModeStackIndex < 16) {
        NuQFntModeStack[NuQFntModeStackIndex++] = NuQFntMode;
    }
    NuQFntMode = 2;
    va_start(arguments, format);
    vsprintf(text, format, arguments);
    va_end(arguments);
    width = (i32)NuQFntPrintLenU(font, text);
    if (alignment == 0x20) {
        x -= width;
    } else if (alignment == 0x40) {
        x -= width / 2;
    }
    NuQFntMoveRS(NULL, font, (f32)x, (f32)y, 0.0f);
    NuQFntPrintU(font, text);
    if (NuQFntModeStackIndex > 0) {
        NuQFntMode = NuQFntModeStack[--NuQFntModeStackIndex];
    }
}
