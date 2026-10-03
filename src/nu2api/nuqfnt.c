#include "saga.h"

extern i32 nuqfnt_cs_mode;
extern f32 qfnt_rezscale_w;
extern f32 qfnt_rezscale_h;
extern f32 qfnt_offscale_x;
extern f32 qfnt_offscale_y;

void __cdecl NuQFntPushPrintMode(u32 mode) {
    if (NuQFntModeStackIndex < 16) {
        NuQFntModeStack[NuQFntModeStackIndex++] = NuQFntMode;
    }
    NuQFntMode = mode;
}

void __cdecl NuQFntPopPrintMode(void) {
    if (NuQFntModeStackIndex > 0) {
        NuQFntMode = NuQFntModeStack[--NuQFntModeStackIndex];
    }
}

void __cdecl NuQFntSet(VUFNT *font) {
    if (font == NULL) {
        font = system_qfont;
    }
    if (font != NULL) {
        font->mode = 1;
        nuqfnt_space_width = font->space_width;
    }
}

void __cdecl NuQFntSetScale(VUFNT *font, f32 x_scale, f32 y_scale) {
    if (font == NULL) {
        font = system_qfont;
    }
    if (font != NULL) {
        *font->x_scale = x_scale;
        if ((NuQFntMode & 4) != 0) {
            *font->y_scale = y_scale;
        } else {
            *font->y_scale = (f32)(y_scale * NuQFntHalfScale);
        }
    }
}

void __cdecl NuQFntSetColour(VUFNT *font, u32 colour) {
    if (font == NULL) {
        font = system_qfont;
    }
    if (font != NULL) {
        font->platform_data->colour = colour;
    }
}

void __cdecl NuQFntSetUVSizePc(VUFNT *font, f32 x_scale, f32 y_scale) {
    NuQFntPushPrintMode(2);
    NuQFntSetScale(font, x_scale, y_scale);
    NuQFntPopPrintMode();
}

void __cdecl NuQFntSetRS(void *stream, VUFNT *font) {
    (void)stream;
    font->mode = 1;
}

void __cdecl NuQFntSetScaleRS(void *stream, VUFNT *font, f32 x_scale, f32 y_scale) {
    (void)stream;
    *font->x_scale = x_scale;
    if ((NuQFntMode & 4) != 0) {
        *font->y_scale = y_scale;
        return;
    }
    *font->y_scale = (f32)(y_scale * NuQFntHalfScale);
}

void __cdecl NuQFntSetColourRS(void *stream, VUFNT *font, u32 colour) {
    (void)stream;
    font->platform_data->colour = colour;
}

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

void __cdecl NuQFntMoveRS(void *stream, VUFNT *font, f32 x, f32 y, f32 z) {
    (void)stream;
    if (nuqfnt_cs_mode != 4) {
        x = x * qfnt_rezscale_w + qfnt_offscale_x;
        y = y * qfnt_rezscale_h + qfnt_offscale_y - font->baseline * *font->y_scale;
    }
    font->platform_data->x = x;
    font->platform_data->y = y;
    font->platform_data->z = z;
}

void __cdecl NuQFntSetMtxRS(void *stream, VUFNT *font, NUMTX *matrix) {
    (void)stream;
    font->platform_data->matrix = *matrix;
}

void __cdecl NuQFntPrintCharW(VUFNT *font, u16 *text, u32 flags) {
    NUQFNT_RENDER_DATA_PC *render_data = font->platform_data;
    f32 x = render_data->x;
    f32 y = render_data->y;
    f32 z = render_data->z;
    f32 inverse_texture_width = 1.0f;
    f32 inverse_texture_height = 1.0f;
    f32 height;
    f32 space_width;
    u32 colour;
    i32 is_3d;

    if (text == NULL) {
        return;
    }
    if (font->material->texture_id > 0) {
        NUTEXDIM_PC *texture_dimensions = NuTextureDimensions[font->material->texture_id - 1];
        inverse_texture_width = 1.0f / (f32)texture_dimensions->width;
        inverse_texture_height = 1.0f / (f32)texture_dimensions->height;
    }

    height = font->height * *font->y_scale;
    if (nuqfnt_space_width == 0.0f) {
        space_width = font->space_width * *font->x_scale;
    } else {
        space_width = nuqfnt_space_width * *font->x_scale;
    }
    is_3d = (flags & 4) != 0;

    /* PC coordinate-stack increment and coordinate-system selection. */
    STUBBED_NAMED(0x02854d3c);
    STUBBED_NAMED(0x00600b20);
    if (is_3d) {
        STUBBED_NAMED(0x00607ec0);
    } else {
        STUBBED_NAMED(0x00607d70);
    }

    colour = render_data->colour;
    while (*text != 0) {
        u16 character = *text++;
        VUFNTCHAR *glyph = &font->glyphs[character];
        f32 advance;
        f32 left = x;

        if (character == 0x20) {
            advance = space_width;
        } else {
            advance = glyph->width * *font->x_scale;
        }

        if ((flags & 1) != 0 && character >= 0x30 && character < 0x3a) {
            u16 zero_index = NuQFntEncodeUnicodeChar(font, 0x30);
            VUFNTCHAR *zero = &font->glyphs[zero_index];
            f32 digit_width = zero->width * *font->x_scale;
            left += digit_width - advance;
            advance = digit_width;
        }

        if (character != 0x20) {
            f32 right = left + glyph->width * *font->x_scale;
            f32 u0 = glyph->x * inverse_texture_width;
            f32 v0 = glyph->y * inverse_texture_height;
            f32 u1 = (glyph->x + glyph->width) * inverse_texture_width;
            f32 v1 = (glyph->y + font->height) * inverse_texture_height;

            if (is_3d) {
                f32 top = y - height;
                /* Each marker is one PC NuPrim vertex submission (0x005c1f70). */
                STUBBED_NAMED(0x005c1f70);
                STUBBED_NAMED(0x005c1f70);
                STUBBED_NAMED(0x005c1f70);
                STUBBED_NAMED(0x005c1f70);
                STUBBED_NAMED(0x005c1f70);
                STUBBED_NAMED(0x005c1f70);
            } else {
                /* PC 2D path submits two corner vertices through 0x006009e0. */
                STUBBED_NAMED(0x006009e0);
                STUBBED_NAMED(0x006009e0);
            }
        }
        x += advance + font->ic_gap * *font->x_scale;
    }

    if (is_3d) {
        STUBBED_NAMED(0x006009c0);
    } else {
        STUBBED_NAMED(0x006009a0);
    }
    /* PC decrements the coordinate stack and restores the saved mode here. */
    STUBBED_NAMED(0x02854d3c);
    STUBBED_NAMED(0x00981c6c);
    STUBBED_NAMED(0x00600b20);
}

void __cdecl NuQFntPrintU(VUFNT *font, char *text) {
    u16 encoded[1024];
    if (font == NULL) {
        font = system_qfont;
    }
    if (font != NULL && text != NULL) {
        NuQFntUTF8toQCode(font, text, encoded);
        NuQFntPrintCharW(font, encoded, NuQFntMode);
    }
}

typedef struct qfont_material_stream_view_pc_s {
    u8 unknown_0000[4];
    u8 stream_begin;
} QFONT_MATERIAL_STREAM_VIEW_PC;
STATIC_ASSERT(offsetof(QFONT_MATERIAL_STREAM_VIEW_PC, stream_begin) == 4);

void __cdecl NuQFntMove(VUFNT *font, f32 x, f32 y, f32 z) {
    void *stream;
    if (font == NULL) {
        font = system_qfont;
    }
    if (font != NULL) {
        stream =
            (NuQFntMode & 2) != 0 ? NULL : (void *)&((QFONT_MATERIAL_STREAM_VIEW_PC *)font->material)->stream_begin;
        NuQFntMoveRS(stream, font, x, y, z);
    }
}
