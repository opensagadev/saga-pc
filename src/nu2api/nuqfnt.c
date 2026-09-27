#include "saga.h"

u16 UnicodeToIndexFast(VUCHARIDX *map, i32 count, u16 unicode) {
    i32 middle = count / 2;
    i32 low = 0;
    i32 high = count - 1;
    if (unicode > map[count - 1].unicode) {
        return 0xffff;
    }
    while (map[middle].unicode != unicode) {
        if (high == low + 1) {
            if (map[high].unicode == unicode) {
                return map[high].index;
            }
            if (map[low].unicode == unicode) {
                return map[low].index;
            }
            return 0xffff;
        }
        if (map[middle].unicode > unicode) {
            high = middle;
            middle = (low + high) >> 1;
        } else if (map[middle].unicode < unicode) {
            low = middle;
            middle = (low + high) >> 1;
        } else if (high == low) {
            return 0xffff;
        }
    }
    return map[middle].index;
}

u16 NuQFntEncodeUnicodeChar(VUFNT *font, u16 character) {
    if (font == NULL) {
        font = system_qfont;
    }
    if (font == NULL) {
        return 0xffff;
    }
    return UnicodeToIndexFast(font->unicode_map, font->unicode_count, character);
}

void NuQFntEncodeUnicodeString(VUFNT *font, u16 *text) {
    if (font == NULL) {
        font = system_qfont;
    }
    if (font != NULL) {
        while (*text != 0) {
            *text = UnicodeToIndexFast(font->unicode_map, font->unicode_count, *text);
            if (*text == 0xffff) {
                *text = '?';
            }
            ++text;
        }
    }
}

void NuQFntUTF8toQCode(VUFNT *font, char *text, u16 *encoded) {
    u8 *cursor;
    if (font == NULL) {
        font = system_qfont;
    }
    if (font == NULL) {
        return;
    }
    cursor = (u8 *)text;
    while (*cursor != '\0') {
        u16 character;
        cursor = NuUnicodeCharFromUTF8(&character, cursor);
        *encoded = NuQFntEncodeUnicodeChar(font, character);
        if (*encoded == 0xffff) {
            *encoded = NuQFntEncodeUnicodeChar(font, '?');
        }
        ++encoded;
    }
    *encoded = 0;
}
