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

extern i32 NuPrimCoordSystemStack[];
typedef struct qfont_prim_vertex_pc_s {
    f32 x, y, z;
    u32 colour;
    f32 u, v;
} QFONT_PRIM_VERTEX_PC;
STATIC_ASSERT(sizeof(QFONT_PRIM_VERTEX_PC) == 0x18);
STATIC_ASSERT(offsetof(QFONT_PRIM_VERTEX_PC, colour) == 0xc);
STATIC_ASSERT(offsetof(QFONT_PRIM_VERTEX_PC, u) == 0x10);
STATIC_ASSERT(offsetof(QFONT_PRIM_VERTEX_PC, v) == 0x14);
static void NuRndrPrimSetColourPrivate(i32 colour) {
    u32 converted;
    if (NuPrimNeedsOverbrightening != 0)
        converted = ((colour & 0xff) << 16) + ((colour >> 16) & 0xff) + (colour & 0xff00) + (colour & 0xff000000);
    else
        converted =
            ((colour & 0xfe) << 15) + ((colour >> 1) & 0x7f00) + ((colour >> 17) & 0x7f) + (colour & 0xff000000);
    ((QFONT_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr)->colour = converted;
}
#define QFONT_VERTEX(X, Y, Z, U, V)                                                                                    \
    do {                                                                                                               \
        QFONT_PRIM_VERTEX_PC *vertex;                                                                                  \
        NuRndrPrimSetColourPrivate((i32)colour);                                                                       \
        vertex = (QFONT_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;                                          \
        vertex->u = (U);                                                                                               \
        vertex->v = (V);                                                                                               \
        vertex->x = (X);                                                                                               \
        vertex->y = (Y);                                                                                               \
        vertex->z = (Z);                                                                                               \
        NuPrimStreamBufferPointer->addr += sizeof(*vertex);                                                            \
        ++NuPrimVertexCount;                                                                                           \
    } while (0)

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

    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(0);
    if (is_3d) {
        NuPrim3DBegin(12, 7, font->material, &render_data->matrix);
    } else {
        NuPrim2DBegin(16, 7, font->material);
    }

    colour = render_data->colour;
    while (*text != 0) {
        u16 character = *text++;
        VUFNTCHAR *glyph = &font->glyphs[character];
        f32 advance;
        f32 glyph_width;
        f64 precise_left = x;
        f32 left;
        f32 right;

        if (character == 0x20) {
            advance = space_width;
        } else {
            advance = glyph->width * *font->x_scale;
        }

        glyph_width = advance;
        if ((flags & 1) != 0 && character >= 0x30 && character < 0x3a) {
            u16 zero_index = NuQFntEncodeUnicodeChar(font, 0x30);
            VUFNTCHAR *zero = &font->glyphs[(i16)zero_index];
            f32 digit_width = zero->width * *font->x_scale;
            f32 adjustment = digit_width - glyph_width;
            precise_left = (f64)adjustment + x;
            advance = digit_width;
        }

        left = (f32)precise_left;
        right = (f32)(precise_left + glyph_width);
        if (character != 0x20) {
            f32 u0 = glyph->x * inverse_texture_width;
            f32 v0 = glyph->y * inverse_texture_height;
            f32 u1 = (glyph->x + glyph->width) * inverse_texture_width;
            f32 v1 = (glyph->y + font->height) * inverse_texture_height;

            if (is_3d) {
                f32 top = y - height;
                QFONT_VERTEX(left, top, z, u0, v1);
                QFONT_VERTEX(right, top, z, u1, v1);
                QFONT_VERTEX(right, y, z, u1, v0);
                QFONT_VERTEX(right, y, z, u1, v0);
                QFONT_VERTEX(left, y, z, u0, v0);
                QFONT_VERTEX(left, top, z, u0, v1);
            } else {
                QFONT_PRIM_VERTEX_PC *vertex;
                f32 bottom = y + height;
                NuRndrPrimSetColourPrivate((i32)colour);
                vertex = (QFONT_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
                vertex->u = u0;
                vertex->v = v0;
                NuPrim2DAddXYZ(left, y, 0.0f);
                NuRndrPrimSetColourPrivate((i32)colour);
                vertex = (QFONT_PRIM_VERTEX_PC *)NuPrimStreamBufferPointer->void_ptr;
                vertex->u = u1;
                vertex->v = v1;
                NuPrim2DAddXYZ(right, bottom, 0.0f);
            }
        }
        x = (f32)((f64)font->ic_gap * *font->x_scale + advance + x);
    }

    if (is_3d) {
        NuPrim3DEnd();
    } else {
        NuPrim2DEnd();
    }
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}

#undef QFONT_VERTEX

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

void __cdecl NuQFntSetMtx(VUFNT *font, NUMTX *matrix) {
    if (font == NULL)
        font = system_qfont;
    if (font != NULL)
        NuQFntSetMtxRS(NULL, font, matrix);
}

typedef struct font_stream_anchor_pc_s {
    u8 prefix[4];
    u8 stream_anchor;
} FONT_STREAM_ANCHOR_PC;
STATIC_ASSERT(offsetof(FONT_STREAM_ANCHOR_PC, stream_anchor) == 4);

void __cdecl NuQFntSetSpaceWidth(VUFNT *font, f32 width) {
    (void)font;
    nuqfnt_space_width = width;
}
f32 __cdecl NuQFntPrintJustifiedRSW(void *stream, VUFNT *font, u16 *text, f32 x, f32 y, f32 z, f32 sx, f32 sy,
                                    f32 width, f32 line_spacing, u32 colour, NUMTX *matrix) {
    f32 saved_space_width = nuqfnt_space_width;
    f32 space_width = saved_space_width == 0.0f ? font->space_width : saved_space_width;
    f32 saved_ic_gap = font->ic_gap, printed_space_width, line_height;
    u16 encoded_space[2] = {0x20, 0};
    u16 hyphen;
    i32 line_number = 0;
    (void)stream;
    NuQFntSetColourRS(stream, font, colour);
    NuQFntSetScaleRS(stream, font, sx, sy);
    NuQFntSetSpaceWidth(font, space_width);
    printed_space_width = NuQFntPrintLenW(font, encoded_space);
    if (matrix != NULL)
        NuQFntSetMtxRS(stream, font, matrix);
    line_height = font->height * *font->y_scale * qfnt_height_scale;
    hyphen = NuLanguageGet() == 0 ? 0xfffe : UnicodeToIndexFast(font->unicode_map, font->unicode_count, '-');
    do {
        u16 line[256], *next = text, *copy_cursor;
        i32 words = 0, punctuation_spaces = 0, length, spaces;
        f32 words_width = 0.0f, scale, scaled_words_width;
        font->ic_gap = saved_ic_gap;
        NuQFntSetScaleRS(stream, font, sx, sy);
        for (;;) {
            u16 *word_start = next;
            i32 extra_spaces = 0;
            f32 word_width;
            f64 combined_word_width;
            length = 0;
            while (*next != 0 && *next != hyphen) {
                if (*next != 0x20) {
                    line[length] = *next;
                    line[length + 1] = 0;
                    if (NuQFntPrintLenW(font, line) > width) {
                        line[length] = 0;
                        break;
                    }
                    ++length;
                } else {
                    u16 following = next[1];
                    if (following != '!' && following != '?' && following != '.' && following != ',')
                        break;
                    ++extra_spaces;
                }
                ++next;
            }
            if (length > 256)
                AnimationDiagnosticNoOpPc();
            if (*next == hyphen)
                line[length++] = *next++;
            while (*next == 0x20)
                ++next;
            line[length] = 0;
            word_width = NuQFntPrintLenW(font, line);
            combined_word_width = (f64)word_width + words_width;
            if (words != 0 &&
                !((f64)width * qfnt_justify_squash >=
                  (f64)(words + punctuation_spaces + extra_spaces) * printed_space_width + combined_word_width)) {
                next = word_start;
                break;
            }
            words_width = (f32)combined_word_width;
            ++words;
            punctuation_spaces += extra_spaces;
            if (*next == 0)
                break;
        }
        if ((next - text) > 256)
            AnimationDiagnosticNoOpPc();
        length = 0;
        copy_cursor = text;
        while (copy_cursor < next) {
            if (*copy_cursor != 0x20)
                line[length++] = *copy_cursor++;
            else {
                line[length++] = 0x20;
                do {
                    ++copy_cursor;
                } while (*copy_cursor == 0x20);
            }
        }
        line[length] = 0;
        spaces = words - 1 + punctuation_spaces;
        if (*next == 0) {
            f64 ratio = (f64)width / ((f64)printed_space_width * spaces + words_width);
            scale = (f32)(1.0 < ratio ? 1.0 : ratio);
        } else if (words == 1)
            scale = width / words_width;
        else {
            f64 ratio = (f64)width / ((f64)printed_space_width * spaces + words_width);
            scale = (f32)(qfnt_justify_stretch < ratio ? qfnt_justify_stretch : ratio);
        }
        NuQFntSetScaleRS(stream, font, sx * scale, sy);
        scaled_words_width = words_width * scale;
        if (*next == 0)
            NuQFntSetSpaceWidth(font, space_width * scale);
        else if (words == 1)
            NuQFntSetSpaceWidth(font, space_width);
        else
            NuQFntSetSpaceWidth(font, (f32)((f64)space_width * ((f64)width - scaled_words_width) /
                                            ((f64)scale * printed_space_width * (spaces < 0 ? 0 : spaces))));
        NuQFntMoveRS(stream, font, x, (f32)((f64)line_number * line_height * line_spacing + y), z);
        NuQFntPrintCharW(font, line, matrix == NULL ? 0 : 4);
        ++line_number;
        text = next;
    } while (*text != 0);
    NuQFntSetSpaceWidth(font, saved_space_width);
    font->ic_gap = saved_ic_gap;
    return (f32)((f64)line_number * line_height * line_spacing);
}
f32 __cdecl NuQFntPrintJustifiedW(VUFNT *font, u16 *text, f32 x, f32 y, f32 z, f32 sx, f32 sy, f32 width,
                                  f32 line_spacing, u32 colour, NUMTX *matrix) {
    if (font == NULL)
        font = system_qfont;
    if (font == NULL)
        return 0.0f;
    return NuQFntPrintJustifiedRSW(&((FONT_STREAM_ANCHOR_PC *)font->material)->stream_anchor, font, text, x, y, z, sx,
                                   sy, width, line_spacing, colour, matrix);
}

#include <stdarg.h>

void NuQFntPrintV(VUFNT *font, const char *format, va_list arguments) {
    char text[1024];
    vsprintf(text, format, arguments);
    NuQFntPrintU(font, text);
}
