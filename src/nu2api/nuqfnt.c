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

#include <string.h>
#include <stdio.h>
void __cdecl PCSpecialEmptyHandler_candidate(void);
struct PC_MATERIAL_CREATE_SHADER {
    u32 flags;
    i32 diffuse_map_id;
    u8 unknown_08[0xc];
    u32 packed_diffuse_colour;
    u8 unknown_18[0xc];
    f32 default_scale;
    u8 unknown_28[0x80];
    u8 texture_unit_count;
    u8 unknown_a9[0x63];
    i32 animation_ids[4];
    u8 unknown_11c[0x20];
    u32 vertex_format;
    i16 shader_handle, shader_variant_handle;
    u8 unknown_144[0x74];
    u32 material_flags;
    u8 unknown_1bc[0x4c];
};
STATIC_ASSERT(sizeof(PC_MATERIAL_CREATE_SHADER) == 0x208);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, packed_diffuse_colour) == 0x14);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, default_scale) == 0x24);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, texture_unit_count) == 0xa8);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, animation_ids) == 0x10c);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, vertex_format) == 0x13c);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, shader_handle) == 0x140);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, material_flags) == 0x1b8);
typedef struct PC_MATERIAL_CREATE_VIEW PC_MATERIAL_CREATE_VIEW;
struct PC_MATERIAL_CREATE_VIEW {
    u16 flags;
    u8 unknown_002[0x26];
    u32 retained_28, retained_2c;
    NUMTL *next;
    u8 unknown_034[8];
    NUDISPLAYLIST_PC *display_list;
    u32 attributes, attributes_high;
    u8 unknown_048[0xc];
    NUVEC diffuse;
    u8 unknown_060[0x10];
    f32 opacity;
    i16 texture_id;
    i16 sort_priority;
    u8 unknown_078[0x3c];
    PC_MATERIAL_CREATE_SHADER shader;
    PC_TEXTURE_RESOURCE_PC *vertex_declaration;
    u16 version;
    u8 unknown_2c2[2];
};
STATIC_ASSERT(sizeof(PC_MATERIAL_CREATE_VIEW) == 0x2c4);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, retained_28) == 0x28);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, retained_2c) == 0x2c);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, next) == 0x30);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, display_list) == 0x3c);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, attributes) == 0x40);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, attributes_high) == 0x44);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, diffuse) == 0x54);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, opacity) == 0x70);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, texture_id) == 0x74);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, shader) == 0xb4);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, vertex_declaration) == 0x2bc);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, version) == 0x2c0);

STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_VIEW, sort_priority) == 0x76);
typedef struct FONT_LOAD_READER_PC {
    u8 unknown00[6];
    u16 flags;
    i32 size;
    i32 glyph_count, unicode_count;
    f32 height, baseline, space_width;
    i32 mode;
    f32 ic_gap;
    void *legacy_font;
    f32 *x_scale, *y_scale;
    VUFNTCHAR *glyphs;
    VUCHARIDX *unicode_map;
    void *header;
    NUMTL *material;
    u32 *colour;
    NUQFNT_RENDER_DATA_PC *platform_data;
} FONT_LOAD_READER_PC;
STATIC_ASSERT(sizeof(FONT_LOAD_READER_PC) == 0x4c);
STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, flags) == 6);
STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, size) == 8);
STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, glyphs) == 0x34);
STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, unicode_map) == 0x38);
STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, header) == 0x3c);
STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, material) == 0x40);
STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, colour) == 0x44);
STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, platform_data) == 0x48);
typedef struct FONT_RENDER_STORAGE_PC {
    NUQFNT_RENDER_DATA_PC draw;
    f32 x_scale, y_scale;
} FONT_RENDER_STORAGE_PC;
STATIC_ASSERT(sizeof(FONT_RENDER_STORAGE_PC) == 0x58);
STATIC_ASSERT(offsetof(FONT_RENDER_STORAGE_PC, x_scale) == 0x50);
STATIC_ASSERT(offsetof(FONT_RENDER_STORAGE_PC, y_scale) == 0x54);
typedef struct FONT_PTR_BLOCK_PC {
    VUFNT *font;
    void *texture;
} FONT_PTR_BLOCK_PC;
STATIC_ASSERT(sizeof(FONT_PTR_BLOCK_PC) == 8);
void __cdecl NuQFntDestroy(VUFNT *font);
i32 __cdecl NuQFntReadPS(VUFNT *font, i32 texture_id, i32 flags, i32 plane, VARIPTR *buffer, VARIPTR end);
VUFNT *__cdecl NuQFntReadBuffer(VARIPTR *font, VARIPTR *buffer, VARIPTR end);
VUFNT *__cdecl NuQFntRead(char *path, VARIPTR *buffer, VARIPTR end);
VUFNT *__cdecl NuQFntDuplicate(VUFNT *font, i32 flags, i32 plane, VARIPTR *buffer, VARIPTR *end);
VUFNT *__cdecl NuQFntLoadPtr(char *path, char *texture_path, i32 flags, i32 plane, VARIPTR *buffer, VARIPTR *end);

void __cdecl NuQFntDestroy(VUFNT *font) {
    if (font != NULL) {
        i32 texture_id = font->material->texture_id;
        NuMtlDestroy(font->material);
        if ((((FONT_LOAD_READER_PC *)font)->flags & 1) == 0)
            NuTexDestroy(texture_id);
    }
}

i32 __cdecl NuQFntReadPS(VUFNT *font, i32 texture_id, i32 flags, i32 plane, VARIPTR *buffer, VARIPTR end) {
    PC_MATERIAL_CREATE_VIEW *material;
    PC_MATERIAL_CREATE_SHADER description;
    FONT_LOAD_READER_PC *reader = (FONT_LOAD_READER_PC *)font;
    FONT_RENDER_STORAGE_PC *platform;
    font->material = NuMtlCreate(1);
    material = (PC_MATERIAL_CREATE_VIEW *)font->material;
    material->diffuse.x = material->diffuse.y = material->diffuse.z = 1.0f;
    material->attributes = (material->attributes & ~0x1000U) | 0x2000U;
    material->attributes |= 0xc000U;
    material->opacity = 1.0f;
    material->attributes = (material->attributes & ~14U) | 1U;
    material->attributes |= 0x40000U;
    material->texture_id = (i16)texture_id;
    material->attributes = (material->attributes & ~0x200U) | 0x100U;
    material->attributes = (material->attributes & ~0x800U) | 0x400U;
    memset(&description, 0, sizeof(description));
    description.default_scale = 1.0f;
    description.material_flags |= 0x1000;
    description.flags = 0x1000;
    description.diffuse_map_id = texture_id;
    description.packed_diffuse_colour = 0xffffffff;
    description.vertex_format = (description.vertex_format & ~0x3000U) | 0x40901U;
    NuMtlSetShaderDescPS(font->material, (const struct PC_MATERIAL_SHADER_DESC_VIEW *)&description);
    NuMtlUpdate(font->material);
    platform = (FONT_RENDER_STORAGE_PC *)((buffer->addr + 15) & ~15U);
    reader->platform_data = &platform->draw;
    buffer->addr = ((buffer->addr + 15) & ~15U) + sizeof(*platform);
    reader->x_scale = &platform->x_scale;
    reader->colour = &platform->draw.colour;
    reader->y_scale = &platform->y_scale;
    reader->header = NULL;
    return 0;
}

VUFNT *__cdecl NuQFntReadBuffer(VARIPTR *font, VARIPTR *buffer, VARIPTR end) {
    FONT_PTR_BLOCK_PC *block;
    VUFNT *result;
    NUTEXDIM_PC *texture;
    i32 texture_id;
    if (font->void_ptr == NULL)
        return NULL;
    block = (FONT_PTR_BLOCK_PC *)NuPtrBlockFix(font->void_ptr);
    result = block->font;
    texture = (NUTEXDIM_PC *)buffer->void_ptr;
    buffer->addr += sizeof(*texture);
    texture->image_data = block->texture;
    texture->image_size = 0x200000;
    texture_id = NuTexCreateNative(texture);
    NuQFntReadPS(result, texture_id, 4, 0, buffer, end);
    return result;
}

VUFNT *__cdecl NuQFntRead(char *path, VARIPTR *buffer, VARIPTR end) {
    char fixed_path[1204];
    VARIPTR aligned;
    NuStrFixExtPlatform(fixed_path, path, "fnt", 1024, "pc");
    aligned.addr = (buffer->addr + 31) & ~31U;
    buffer->addr = aligned.addr;
    if (NuFileLoadBufferVP(fixed_path, buffer, &end) == 0)
        return NULL;
    return NuQFntReadBuffer(&aligned, buffer, end);
}

VUFNT *__cdecl NuQFntDuplicate(VUFNT *font, i32 flags, i32 plane, VARIPTR *buffer, VARIPTR *end) {
    FONT_LOAD_READER_PC *reader = (FONT_LOAD_READER_PC *)font;
    FONT_LOAD_READER_PC *duplicate;
    PC_MATERIAL_CREATE_VIEW *material, *original;
    u32 previous_plane;
    if (font == NULL)
        AnimationDiagnosticNoOpPc();
    duplicate = (FONT_LOAD_READER_PC *)((buffer->addr + 15) & ~15U);
    buffer->void_ptr = duplicate;
    buffer->addr += reader->size;
    if (buffer->addr < end->addr) {
        PCByteCopy_candidate(duplicate, font, reader->size);
        duplicate->flags |= 1;
        duplicate->glyphs = (VUFNTCHAR *)((u32)duplicate->glyphs + (u32)duplicate - (u32)font);
        duplicate->unicode_map = (VUCHARIDX *)((u32)duplicate->unicode_map + (u32)duplicate - (u32)font);
    }
    if (plane < 0 || plane >= 24)
        AnimationDiagnosticNoOpPc();
    previous_plane = numtl_renderplane;
    numtl_renderplane = plane;
    duplicate->material = (flags & 8) ? NuMtlCreate3D(1) : NuMtlCreate(1);
    material = (PC_MATERIAL_CREATE_VIEW *)duplicate->material;
    original = (PC_MATERIAL_CREATE_VIEW *)font->material;
    material->attributes = original->attributes;
    material->attributes_high = original->attributes_high;
    material->texture_id = original->texture_id;
    material->sort_priority = original->sort_priority;
    material->attributes = (material->attributes & ~0x10000U) | 0x20000U;
    if (flags & 8)
        material->attributes_high &= ~0x800000U;
    if (flags & 0x40)
        material->attributes &= ~0xc000U;
    NuMtlUpdate(duplicate->material);
    if (previous_plane >= 24)
        AnimationDiagnosticNoOpPc();
    numtl_renderplane = previous_plane;
    return (VUFNT *)duplicate;
}

VUFNT *__cdecl NuQFntLoadPtr(char *path, char *texture_path, i32 flags, i32 plane, VARIPTR *buffer, VARIPTR *end) {
    VUFNT *font = NuQFntRead(path, buffer, *end);
    VARIPTR saved;
    char filename[1024];
    FONT_LOAD_READER_PC *reader;
    FONT_RENDER_STORAGE_PC *platform;
    PC_MATERIAL_CREATE_VIEW *material;
    i32 size, texture_id;
    if (font != NULL)
        return font;
    saved = *buffer;
    buffer->addr = (buffer->addr + 15) & ~15U;
    sprintf(filename, "%s.qfn", path);
    size = NuFileLoadBuffer(filename, buffer->void_ptr, end->addr - buffer->addr);
    if (size != 0) {
        font = (VUFNT *)buffer->void_ptr;
        reader = (FONT_LOAD_READER_PC *)font;
        reader->glyphs = (VUFNTCHAR *)((u32)reader->glyphs + (u32)font);
        reader->unicode_map = (VUCHARIDX *)((u32)reader->unicode_map + (u32)font);
        buffer->addr = (buffer->addr + size + 15) & ~15U;
        platform = (FONT_RENDER_STORAGE_PC *)buffer->void_ptr;
        reader->platform_data = &platform->draw;
        buffer->addr += sizeof(*platform);
        reader->colour = &platform->draw.colour;
        reader->x_scale = &platform->x_scale;
        reader->y_scale = &platform->y_scale;
        sprintf(filename, "%s", texture_path != NULL ? texture_path : path);
        texture_id = NuTexReadPc(filename, buffer, *end);
        if (texture_id != 0) {
            font->material = NuMtlCreate(1);
            if (font->material != NULL) {
                material = (PC_MATERIAL_CREATE_VIEW *)font->material;
                material->diffuse.x = material->diffuse.y = material->diffuse.z = 1.0f;
                material->attributes = (material->attributes & ~0x1000U) | 0x2000U;
                material->attributes |= 0xc000U;
                material->opacity = 1.0f;
                material->attributes = (material->attributes & ~14U) | 1U;
                material->attributes |= 0x40000U;
                material->texture_id = (i16)texture_id;
                NuMtlUpdate(font->material);
                return font;
            }
            NuTexDestroy(texture_id);
        }
    }
    PCSpecialEmptyHandler_candidate();
    *buffer = saved;
    return NULL;
}

void __cdecl NuMtlSetRenderPlanePc(NUMTL *material, i32 plane);
VUFNT *__cdecl NuQFntCreate(FONT_LEGACY_READER_PC *font, i32 flags, i32 options, VARIPTR *buffer, VARIPTR *end) {
    f32 width = (f32)NuTextureDimensions[font->material->texture_id - 1]->width;
    f32 height = (f32)NuTextureDimensions[font->material->texture_id - 1]->height;
    FONT_LOAD_READER_PC *result = (FONT_LOAD_READER_PC *)((buffer->addr + 15) & ~15U);
    PC_MATERIAL_CREATE_VIEW *material;
    i32 i, index;
    buffer->addr = (u32)result + sizeof(*result) + sizeof(FONT_RENDER_STORAGE_PC);
    memset(result, 0, sizeof(*result) + sizeof(FONT_RENDER_STORAGE_PC));
    result->platform_data = (NUQFNT_RENDER_DATA_PC *)(result + 1);
    result->glyph_count = font->glyph_count;
    result->legacy_font = font;
    result->height = font->height;
    result->baseline = font->baseline;
    result->ic_gap = font->ic_gap;
    result->space_width = font->glyphs[0].u1 * width - font->glyphs[0].u0 * width;
    result->header = buffer->void_ptr;
    *(f32 *)buffer->void_ptr = result->ic_gap;
    buffer->addr += 4;
    *(f32 *)buffer->void_ptr = result->height;
    buffer->addr += 4;
    *(f32 *)buffer->void_ptr = 1.0f / width;
    buffer->addr += 4;
    *(f32 *)buffer->void_ptr = 1.0f / height;
    buffer->addr += 4;
    *(f32 *)buffer->void_ptr = 0.0f;
    buffer->addr += 4;
    *(f32 *)buffer->void_ptr = 0.0f;
    buffer->addr += 4;
    *(f32 *)buffer->void_ptr = result->space_width;
    buffer->addr += 4;
    *(f32 *)buffer->void_ptr = 0.0f;
    buffer->addr += 4;
    result->x_scale = (f32 *)buffer->void_ptr;
    *(f32 *)buffer->void_ptr = 1.0f;
    buffer->addr += 4;
    result->y_scale = (f32 *)buffer->void_ptr;
    *(f32 *)buffer->void_ptr = 0.5f;
    buffer->addr += 4;
    result->glyphs = (VUFNTCHAR *)buffer->void_ptr;
    for (i = 0; i < font->glyph_count && i < 256; ++i) {
        f32 x = font->glyphs[i].u0 * width;
        *(f32 *)buffer->void_ptr = x;
        buffer->addr += 4;
        *(f32 *)buffer->void_ptr = font->glyphs[i].v0 * height;
        buffer->addr += 4;
        *(f32 *)buffer->void_ptr = font->glyphs[i].u1 * width - font->glyphs[i].u0 * width;
        buffer->addr += 4;
    }
    result->unicode_map = (VUCHARIDX *)buffer->void_ptr;
    for (i = 0; i < 256; ++i) {
        index = font->character_map[i];
        if (index == 255)
            index = 0;
        if (i == 32)
            index = font->glyph_count;
        *buffer->char_ptr++ = (char)index;
    }
    result->material = (NUMTL *)((buffer->addr + 15) & ~15U);
    buffer->addr = (u32)result->material + sizeof(PC_MATERIAL_CREATE_VIEW);
    memcpy(result->material, font->material, sizeof(PC_MATERIAL_CREATE_VIEW));
    NuMtlSetRenderPlanePc(result->material, options & 0xffff);
    material = (PC_MATERIAL_CREATE_VIEW *)result->material;
    if (flags & 8)
        material->attributes_high &= ~4U;
    else
        material->attributes_high |= 4U;
    material->attributes = (material->attributes & ~0x10000U) | 0x20000U;
    material->attributes = (flags & 2) ? material->attributes & ~15U : (material->attributes & ~14U) | 1U;
    if (flags & 0x40)
        material->attributes &= ~0xc000U;
    else if (flags & 0x20)
        material->attributes = (material->attributes & ~0x8000U) | 0x4000U;
    else
        material->attributes |= 0xc000U;
    if (flags & 4)
        material->attributes = (material->attributes & ~0x20U) | 0x10U;
    else if (flags & 0x10)
        material->attributes = (material->attributes & ~0x10U) | 0x20U;
    else
        material->attributes &= ~0x30U;
    material->sort_priority = (i16)(options >> 16);
    NuMtlUpdate(result->material);
    result->platform_data->z = result->platform_data->y = result->platform_data->x = 0.0f;
    return (VUFNT *)result;
}
void NuQFntWrite(char *path, VUFNT *font) {
    char text[256];
    i32 i;
    NUFILE file;
    sprintf(text, "%s.qfn", path);
    file = NuFileOpen(text, 1);
    if (file != 0) {
        font->glyphs = (VUFNTCHAR *)((u32)font->glyphs - (u32)font);
        font->unicode_map = (VUCHARIDX *)((u32)font->unicode_map - (u32)font);
        NuFileWrite(file, font, ((FONT_LOAD_READER_PC *)font)->size);
        font->glyphs = (VUFNTCHAR *)((u32)font->glyphs + (u32)font);
        font->unicode_map = (VUCHARIDX *)((u32)font->unicode_map + (u32)font);
        NuFileClose(file);
    }

    sprintf(text, "%s.htm", path);
    file = NuFileOpen(text, 1);
    if (file != 0) {
        sprintf(text, "<HTML><BODY><font face = courier><H2>Font name: %s</H2>\n", path);
        NuFileWrite(file, text, strlen(text));
        sprintf(text, "<P>Internal char list: (%d entries)\n<P>\n", font->glyph_count);
        NuFileWrite(file, text, strlen(text));
        for (i = 0; i < font->glyph_count; i++) {
            sprintf(text, "%.3d ('%c'): u=%.3d v=%.3d w=%.3d<BR>\n", i, i < 32 ? 1 : i & 255, (i32)(font->glyphs[i].x),
                    (i32)(font->glyphs[i].y), (i32)(font->glyphs[i].width));
            NuFileWrite(file, text, strlen(text));
        }
        sprintf(text, "<P>Lookup table:\n<P>\n");
        NuFileWrite(file, text, strlen(text));
        for (i = 0; i < font->unicode_count; i++) {
            sprintf(text, "%.4d ('%c'): char=%.3d<BR>\n", font->unicode_map[i].unicode,
                    font->unicode_map[i].unicode & 255, font->unicode_map[i].index);
            NuFileWrite(file, text, strlen(text));
        }
        sprintf(text, "<P></BODY></HTML>");
        NuFileWrite(file, text, strlen(text));
        NuFileClose(file);
    }
}

void NuQFntWriteUniversalFont(char *path, VUFNT *font, char *texture_path) {
    char filename[256];
    char buffer[1024];
    NUFILE file;
    NUFILE texture_file;
    i32 remaining;
    i32 size;
    sprintf(filename, "%s.ufn", path);
    file = NuFileOpen(filename, 1);
    texture_file = NuFileOpen(texture_path, 0);
    if (file != 0 && texture_file != 0) {
        font->glyphs = (VUFNTCHAR *)((u32)font->glyphs - (u32)font);
        font->unicode_map = (VUCHARIDX *)((u32)font->unicode_map - (u32)font);
        NuFileWrite(file, font, ((FONT_LOAD_READER_PC *)font)->size);
        font->glyphs = (VUFNTCHAR *)((u32)font->glyphs + (u32)font);
        font->unicode_map = (VUCHARIDX *)((u32)font->unicode_map + (u32)font);
        remaining = NuFileOpenSize(texture_file);
        while (remaining != 0) {
            size = NuFileRead(texture_file, buffer, remaining > 1024 ? 1024 : remaining);
            NuFileWrite(file, buffer, size);
            remaining -= size;
        }
        NuFileClose(texture_file);
        NuFileClose(file);
    }
}

STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, glyph_count) == 0xc);
STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, unicode_count) == 0x10);
STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, height) == 0x14);
STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, baseline) == 0x18);
STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, space_width) == 0x1c);
STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, ic_gap) == 0x24);
STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, legacy_font) == 0x28);
STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, x_scale) == 0x2c);
STATIC_ASSERT(offsetof(FONT_LOAD_READER_PC, y_scale) == 0x30);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, flags) == 0);
STATIC_ASSERT(offsetof(PC_MATERIAL_CREATE_SHADER, diffuse_map_id) == 4);
