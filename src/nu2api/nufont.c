#include "saga.h"

#include <stdarg.h>
#include <stdio.h>

extern u32 NuQFntModeStack[16];
extern i32 NuQFntModeStackIndex;
extern void __cdecl NuQFntMoveRS(void *stream, VUFNT *font, f32 x, f32 y, f32 z);
extern void __cdecl NuQFntPrintU(VUFNT *font, char *text);

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
        length += width + font->ic_gap;
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
