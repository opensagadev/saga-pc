#include "saga.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

extern char *NuBlankString;

#define NU_WIDE_LENGTH(text, length)                                                                                   \
    do {                                                                                                               \
        const u16 *nu_cursor = (const u16 *)(text);                                                                    \
        (length) = 0;                                                                                                  \
        while (*nu_cursor++ != 0) {                                                                                    \
            ++(length);                                                                                                \
        }                                                                                                              \
    } while (0)

#define NU_CHAR_EQUIV(a, b, result)                                                                                    \
    do {                                                                                                               \
        i32 nu_equiv_i;                                                                                                \
        (result) = 0;                                                                                                  \
        if ((a) == (b)) {                                                                                              \
            (result) = 1;                                                                                              \
        } else {                                                                                                       \
            for (nu_equiv_i = 0; bad_equiv[nu_equiv_i][0] != 0; ++nu_equiv_i) {                                        \
                if ((bad_equiv[nu_equiv_i][1] == (a) && bad_equiv[nu_equiv_i][0] == (b)) ||                            \
                    (bad_equiv[nu_equiv_i][1] == (b) && bad_equiv[nu_equiv_i][0] == (a))) {                            \
                    (result) = 1;                                                                                      \
                    break;                                                                                             \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

#define NU_IS_FLUFF(character, result)                                                                                 \
    do {                                                                                                               \
        i32 nu_fluff_i;                                                                                                \
        (result) = 0;                                                                                                  \
        for (nu_fluff_i = 0; bad_fluff[nu_fluff_i] != 0; ++nu_fluff_i) {                                               \
            if (bad_fluff[nu_fluff_i] == (character)) {                                                                \
                (result) = 1;                                                                                          \
                break;                                                                                                 \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

#define NU_UTF16_UPPER(character)                                                                                      \
    (((character) >= 0x0061 && (character) <= 0x007a) || ((character) >= 0x00e0 && (character) <= 0x00ff)              \
         ? (u16)((character) - 0x20)                                                                                   \
         : (u16)(character))

void __cdecl NuStrCat(char *str, const char *ext) {
    while (*str != '\0') {
        str++;
    }

    if (ext != NULL) {
        do {
            *str = *ext;
            str++;
        } while (*(ext++) != '\0');
    }
}

i32 __cdecl NuStrNCat(char *str, const char *ext, i32 n) {
    i32 copied;

    while (*str != '\0') {
        str++;
    }
    copied = 0;

    if (ext != NULL) {
        do {
            if (n == 0) {
                break;
            }
            *str = *ext;
            str++;
            copied++;
            n--;
        } while (*(ext++) != '\0');
    }
    return copied;
}

i32 __cdecl NuStrCpy(char *dst, const char *src) {
    char *dst_start = dst;

    if (src != NULL) {
        while (*src != '\0') {
            *dst = *src;
            dst++;
            src++;
        }
    }

    *dst = '\0';
    return (i32)(dst - dst_start);
}

i32 __cdecl NuStrNCpy(char *dst, const char *src, i32 n) {
    i32 count = 0;

    if (src == NULL) {
        *dst = '\0';
        return 0;
    }

    do {
        n--;
        if (n <= 0) {
            *dst = '\0';
            count++;
            break;
        }

        *dst = *src;
        dst++;
        count++;
    } while (*(src++) != '\0');

    return count - 1;
}

char *__cdecl NuStrChr(char *src, char c) {
    while (*src != '\0') {
        if (*src == c) {
            return src;
        }
        src++;
    }
    return NULL;
}

char *__cdecl NuStrRChr(char *src, char c) {
    char *ptr = src;

    while (*ptr != '\0') {
        ptr++;
    }

    while (ptr >= src) {
        if (*ptr == c) {
            return ptr;
        }
        ptr--;
    }
    return NULL;
}

void __cdecl NuStrFixExtPlatform(char *dst, char *src, char *ext, i32 dst_size, char *platform_string) {
    char *period;
    char *separator;
    char *windows_separator;
    char suffix[48];
    i32 suffix_length;
    i32 source_length;

    if (src == NULL || ext == NULL || dst == NULL) {
        return;
    }

    NuStrCpy(suffix, "PC");
    NuStrCat(suffix, ext);
    suffix_length = NuStrLen(suffix);
    source_length = NuStrLen(src);
    if (source_length >= suffix_length && NuStrNICmp(src + source_length - suffix_length, suffix, suffix_length) == 0) {
        NuStrCpy(dst, src);
        return;
    }

    NuStrCpy(dst, src);
    period = NuStrRChr(dst, '.');
    separator = NuStrRChr(dst, '/');
    windows_separator = NuStrRChr(dst, '\\');
    if (windows_separator > separator) {
        separator = windows_separator;
    }

    if (separator >= period) {
        NuStrCat(dst, "_");
        NuStrCat(dst, platform_string != NULL ? platform_string : "PC");
        NuStrCat(dst, ext);
    } else {
        *period = '\0';
        NuStrCat(period, "_");
        NuStrCat(period, platform_string != NULL ? platform_string : "PC");
        NuStrCat(period, ext);
    }

    if (NuStrLen(dst) > dst_size - 1) {
        STUBBED_NAMED("NuStrFixExtPlatform: retail buffer-size assertion at 0x4ef790");
    }
}

void __cdecl NuStrFixExt(char *dst, char *src, char *ext, i32 dst_size) {
    if (src == NULL || ext == NULL || dst == NULL) {
        return;
    }
    NuStrFixExtPlatform(dst, src, ext, dst_size, NULL);
}

i32 __cdecl NuStrLen(const char *str) {
    i32 count = 0;

    while (*str != '\0') {
        count++;
        str++;
    }
    return count;
}

i32 __cdecl NuStrLenU(u8 *str) {
    i32 offset = 0;
    i32 characters = 0;

    while (str[offset] != 0) {
        characters++;
        offset++;
        while (str[offset] >= 0x80 && str[offset] <= 0xbf) {
            offset++;
        }
    }
    return characters;
}

i32 __cdecl NuStrCmp(const char *a, const char *b) {
    char a_cursor;
    char b_cursor;

    if (a == NULL) {
        return -1;
    }
    if (b == NULL) {
        return 1;
    }

    do {
        a_cursor = *a;
        b_cursor = *b;
        if (a_cursor > b_cursor) {
            return 1;
        }
        if (a_cursor < b_cursor) {
            return -1;
        }
        a++;
        b++;
    } while (a_cursor != '\0' && b_cursor != '\0');

    return 0;
}

i32 __cdecl NuStrNCmp(const char *a, const char *b, i32 n) {
    char a_cursor;
    char b_cursor;

    if (a == NULL) {
        return -1;
    }
    if (b == NULL) {
        return 1;
    }
    if (n == 0) {
        return 0;
    }
    if (n == -1) {
        n = NuStrLen(a);
    } else if (n == -2) {
        n = NuStrLen(b);
    }

    do {
        a_cursor = *a;
        b_cursor = *b;
        if (a_cursor > b_cursor) {
            return 1;
        }
        if (a_cursor < b_cursor) {
            return -1;
        }
        a++;
        b++;
        n--;
    } while (a_cursor != '\0' && b_cursor != '\0' && n != 0);

    return 0;
}

i32 __cdecl NuStrICmp(const char *a, const char *b) {
    char a_cursor;
    char b_cursor;
    u8 a_upper;
    u8 b_upper;

    if (a == NULL) {
        return -1;
    }
    if (b == NULL) {
        return 1;
    }

    do {
        a_upper = (u8)*a;
        if (a_upper >= 'a' && a_upper <= 'z') {
            a_upper -= 0x20;
        } else if (a_upper >= 0xe0) {
            a_upper -= 0x20;
        }
        a_cursor = (char)a_upper;

        b_upper = (u8)*b;
        if (b_upper >= 'a' && b_upper <= 'z') {
            b_upper -= 0x20;
        } else if (b_upper >= 0xe0) {
            b_upper -= 0x20;
        }
        b_cursor = (char)b_upper;

        if (a_cursor > b_cursor) {
            return 1;
        }
        if (a_cursor < b_cursor) {
            return -1;
        }
        a++;
        b++;
    } while (a_cursor != '\0' && b_cursor != '\0');

    return 0;
}

char *__cdecl NuStrIStr(char *text, char *substring) {
    char *start;
    char *candidate;
    char *pattern;
    u8 text_upper;
    u8 pattern_upper;

    while (*text != '\0') {
        start = text;
        candidate = text;
        pattern = substring;
        while (*pattern != '\0') {
            if (*candidate == '\0') {
                break;
            }
            text_upper = (u8)*candidate;
            if (text_upper >= 'a' && text_upper <= 'z') {
                text_upper -= 0x20;
            } else if (text_upper >= 0xe0) {
                text_upper -= 0x20;
            }
            pattern_upper = (u8)*pattern;
            if (pattern_upper >= 'a' && pattern_upper <= 'z') {
                pattern_upper -= 0x20;
            } else if (pattern_upper >= 0xe0) {
                pattern_upper -= 0x20;
            }
            if (text_upper != pattern_upper) {
                break;
            }
            candidate++;
            pattern++;
        }
        if (*pattern == '\0') {
            return start;
        }
        text++;
    }
    return NULL;
}

void __cdecl NuStrUpr(char *dst, const char *src) {
    u8 value;

    for (; *src != '\0'; src++) {
        value = (u8)*src;
        if (value >= 'a' && value <= 'z') {
            value -= 0x20;
        } else if (value >= 0xe0) {
            value -= 0x20;
        }
        *dst = (char)value;
        dst++;
    }
    *dst = *src;
}

void __cdecl NuStrLwr(char *dst, const char *src) {
    u8 value;

    while (*src != '\0') {
        value = (u8)*src;
        if (value >= 'A' && value <= 'Z') {
            value += 0x20;
        } else if (value > 0xbf && value < 0xe0) {
            value += 0x20;
        }
        *dst = (char)value;
        dst++;
        src++;
    }
    *dst = *src;
}

void __cdecl NuStrLwrW(u16 *dst, const u16 *src) {
    while (*src != 0) {
        u16 value = *src;
        if ((value >= 'A' && value <= 'Z') || (value >= 0xc0 && value <= 0xdf)) {
            value += 0x20;
        }
        *dst++ = value;
        ++src;
    }
    *dst = *src;
}

i32 __cdecl NuStrNICmp(const char *a, const char *b, i32 n) {
    char a_cursor;
    char b_cursor;
    u8 a_upper;
    u8 b_upper;

    if (a == NULL) {
        return -1;
    }
    if (b == NULL) {
        return 1;
    }
    if (n == 0) {
        return 0;
    }
    if (n == -1) {
        n = NuStrLen(a);
    } else if (n == -2) {
        n = NuStrLen(b);
    }

    do {
        a_upper = (u8)*a;
        if (a_upper >= 'a' && a_upper <= 'z') {
            a_upper -= 0x20;
        } else if (a_upper >= 0xe0) {
            a_upper -= 0x20;
        }
        a_cursor = (char)a_upper;

        b_upper = (u8)*b;
        if (b_upper >= 'a' && b_upper <= 'z') {
            b_upper -= 0x20;
        } else if (b_upper >= 0xe0) {
            b_upper -= 0x20;
        }
        b_cursor = (char)b_upper;

        if (a_cursor > b_cursor) {
            return 1;
        }
        if (a_cursor < b_cursor) {
            return -1;
        }
        a++;
        b++;
        n--;
    } while (a_cursor != '\0' && b_cursor != '\0' && n != 0);

    return 0;
}

i32 __cdecl NuHexStringToI(char *string) {
    i32 value = 0;
    u8 upper;

    for (; *string != '\0'; string++) {
        value <<= 4;
        upper = (u8)*string;
        if (upper >= 'a' && upper <= 'z') {
            upper -= 0x20;
        } else if (upper >= 0xe0) {
            upper -= 0x20;
        }

        if (upper <= '9' && upper >= '0') {
            value |= upper - 0x30;
        } else if (upper <= 'F' && upper >= 'A') {
            value |= upper - 0x37;
        } else {
            return 0;
        }
    }
    return value;
}

u8 *__cdecl NuUnicodeCharFromUTF8(u16 *dst, u8 *src) {
    u16 c0 = *src++;

    if (c0 <= 0x7f) {
        *dst = c0;
    } else if ((c0 & 0xe0) == 0xc0) {
        u16 c1 = *src++;
        *dst = ((c0 & 0x1f) << 6) | (c1 & 0x3f);
    } else if ((c0 & 0xf0) == 0xe0) {
        u16 c1 = *src++;
        u16 c2 = *src++;
        *dst = (c0 << 12) | ((c1 & 0x3f) << 6) | (c2 & 0x3f);
    }
    return src;
}

u8 *__cdecl NuUTF8CharFromUnicode(u8 *dst, u16 character) {
    if (character <= 0x7f) {
        *dst++ = (u8)character;
    } else if (character <= 0x7ff) {
        *dst++ = (u8)((character >> 6) | 0xc0);
        *dst++ = (u8)((character & 0x3f) | 0x80);
    } else {
        *dst++ = (u8)((character >> 12) | 0xe0);
        *dst++ = (u8)(((character >> 6) & 0x3f) | 0x80);
        *dst++ = (u8)((character & 0x3f) | 0x80);
    }
    return dst;
}

void __cdecl NuUnicodeToUTF8(u8 *dst, u16 *src) {
    if (src == NULL) {
        return;
    }
    if (dst == NULL) {
        return;
    }

    *dst = '\0';
    for (; *src != 0; src++) {
        if (*src < 0x80) {
            *dst++ = (u8)*src;
        } else if (*src < 0x800) {
            *dst++ = (u8)((*src >> 6 & 0x1f) | 0xc0);
            *dst++ = (u8)((*src & 0x3f) | 0x80);
        } else {
            *dst++ = (u8)((*src >> 12 & 0x1f) | 0xe0);
            *dst++ = (u8)((*src >> 6 & 0x3f) | 0x80);
            *dst++ = (u8)((*src & 0x3f) | 0x80);
        }
    }
    *dst = '\0';
}

void __cdecl NuUTF8ToUnicode(u16 *dst, u8 *src) {
    if (src == NULL) {
        return;
    }
    if (dst == NULL) {
        return;
    }

    *dst = 0;
    while (*src != 0) {
        if (*src < 0x80) {
            *dst = *src;
            src++;
        } else if (*src < 0xe0) {
            *dst = (*src & 0x1f) << 6;
            src++;
            *dst |= *src & 0x3f;
            src++;
        } else if (*src < 0xf0) {
            *dst = (*src & 0x0f) << 12;
            src++;
            *dst |= (*src & 0x3f) << 6;
            src++;
            *dst |= *src & 0x3f;
            src++;
        } else {
            *dst = '?';
            src++;
        }
        dst++;
    }
    *dst = 0;
}

void __cdecl NuAsciiToUnicode(u16 *dst, char *src) {
    i32 pos = 0;

    *dst = 0;
    if (src == NULL) {
        return;
    }
    if (dst == NULL) {
        return;
    }

    for (; src[pos] != '\0'; pos++) {
        dst[pos] = (u8)src[pos];
    }
    dst[pos] = '\0';
}

i32 __cdecl NuStringTableLoadCSV(char *filepath, VARIPTR *buf, VARIPTR buf_end, char *label, char *language) {
    i32 parse_buf_size = 0x8000;
    NUSTRINGBANK *bank = &StringBank[CurrentStringBank];
    u16 label_utf16[0x40];
    u16 lang_utf16[0x40];
    void *mem_buf = (void *)(buf_end.addr - 0x100000);
    i32 bytes_read;
    char *word_buf;
    char *line_buf;
    char *tmp_buf;
    NUFILE mem_file;
    NUFPAR_PC *parser;
    NUSTRING *strings;
    i32 column;
    i32 label_column = -1;
    i32 lang_column = -1;
    i32 len;
    i32 i = 0;
    i32 max_strings = 0;

    NuAsciiToUnicode(label_utf16, label);
    NuAsciiToUnicode(lang_utf16, language);
    bank->strings = NULL;
    bank->max_strings = 0;
    bank->string_count = 0;

    if (buf_end.addr - buf->addr < 0x14b000) {
        NuFileError();
    }

    bytes_read = NuFileLoadBuffer(filepath, mem_buf, 0x100000);
    if ((i32)(0x100000 - bytes_read) < 0x18000) {
        NuFileError();
    }
    word_buf = (char *)((u32)mem_buf + bytes_read);
    line_buf = word_buf + parse_buf_size;
    tmp_buf = line_buf + parse_buf_size;

    mem_file = NuMemFileOpen(mem_buf, bytes_read, 1);
    if (mem_file != 0) {
        parser = NuFParOpen(mem_file);
        if (parser != NULL) {
            parser->word_buffer_ptr = word_buf;
            parser->line_buffer_ptr = line_buf;
            parser->line_buffer_size = parse_buf_size;
            parser->word_buffer_size = parse_buf_size;
            while (NuFParGetLine(parser) != 0) {
                ++max_strings;
            }
        }
        NuFileClose(mem_file);
    }

    strings = (NUSTRING *)((buf->addr + 3) & ~3U);
    buf->addr = (buf->addr + 3) & ~3U;
    buf->addr += max_strings * sizeof(NUSTRING);
    mem_file = NuMemFileOpen(mem_buf, bytes_read, 1);
    if (mem_file != 0) {
        parser = NuFParOpen(mem_file);
        if (parser != NULL) {
            parser->word_buffer_ptr = word_buf;
            parser->line_buffer_ptr = line_buf;
            parser->line_buffer_size = parse_buf_size;
            parser->word_buffer_size = parse_buf_size;
            while (NuFParGetLine(parser) != 0) {
                column = 0;
                while (NuFParGetWord(parser) != 0) {
                    const u16 *a;
                    const u16 *b;
                    i32 equal;
                    a = (const u16 *)parser->word_buffer_ptr;
                    b = label_utf16;
                    equal = 1;
                    while (*a != 0 && *b != 0) {
                        u16 ac = NU_UTF16_UPPER(*a);
                        u16 bc = NU_UTF16_UPPER(*b);
                        if (ac != bc) {
                            equal = 0;
                            break;
                        }
                        ++a;
                        ++b;
                    }
                    if (equal && *a == *b) {
                        label_column = column;
                    }

                    a = (const u16 *)parser->word_buffer_ptr;
                    b = lang_utf16;
                    equal = 1;
                    while (*a != 0 && *b != 0) {
                        u16 ac = NU_UTF16_UPPER(*a);
                        u16 bc = NU_UTF16_UPPER(*b);
                        if (ac != bc) {
                            equal = 0;
                            break;
                        }
                        ++a;
                        ++b;
                    }
                    if (equal && *a == *b) {
                        lang_column = column;
                    }
                    ++column;
                }
                if (label_column >= 0 && lang_column >= 0) {
                    break;
                }
            }

            if (label_column >= 0 && lang_column >= 0) {
                while (NuFParGetLine(parser) != 0) {
                    column = 0;
                    while (NuFParGetWord(parser) != 0) {
                        if (column == label_column) {
                            NU_WIDE_LENGTH(parser->word_buffer_ptr, len);
                            ++len;
                            strings[i].id = buf->char_ptr;
                            buf->addr += (u32)len;
                            NuUnicodeToAscii(strings[i].id, (u16 *)parser->word_buffer_ptr);
                        }
                        if (column == lang_column) {
                            if (nustring_format == 1) {
                                NU_WIDE_LENGTH(parser->word_buffer_ptr, len);
                                ++len;
                                strings[i].str_utf16 = (u16 *)((buf->addr + 1) & ~1U);
                                buf->addr = (buf->addr + 1) & ~1U;
                                buf->addr += (u32)len * 2;
                                {
                                    const u16 *src = (const u16 *)parser->word_buffer_ptr;
                                    u16 *dst = strings[i].str_utf16;
                                    do {
                                        *dst++ = *src;
                                    } while (*src++ != 0);
                                }
                            } else {
                                NuUnicodeToUTF8((u8 *)tmp_buf, (u16 *)parser->word_buffer_ptr);
                                len = NuStrLen(tmp_buf) + 1;
                                strings[i].str_utf8 = (u8 *)((buf->addr + 1) & ~1U);
                                buf->addr = (buf->addr + 1) & ~1U;
                                buf->addr += (u32)len;
                                NuStrCpy(strings[i].str_utf8, tmp_buf);
                            }
                        }
                        ++column;
                        if (column > label_column && column > lang_column) {
                            if (strings[i].id != NULL && strings[i].str_utf16 != NULL) {
                                ++i;
                            } else {
                                strings[i].id = NULL;
                                strings[i].str_utf16 = NULL;
                            }
                            break;
                        }
                    }
                }
            }
            bank->strings = strings;
            bank->string_count = i;
            bank->max_strings = max_strings;
        }
        NuFileClose(mem_file);
    }
    return bank->string_count;
}

i32 __cdecl NuStringTableLoadTXT(char *filepath, VARIPTR *buf, VARIPTR buf_end) {
    i32 i = 0;
    i32 max_strings = 0;
    NUSTRING *strings = NULL;
    NUSTRINGBANK *bank = &StringBank[CurrentStringBank];
    void *mem_buf = (void *)(buf_end.addr - 0x100000);
    i32 bytes_read;
    NUFILE mem_file;
    NUFPAR_PC *parser;
    i32 len;
    char *tmp_buf;

    bank->strings = NULL;
    bank->max_strings = 0;
    bank->string_count = 0;
    bytes_read = NuFileLoadBuffer(filepath, mem_buf, 0x100000);
    if ((i32)(0x100000 - bytes_read) < 0x8000) {
        NuFileError();
    }
    tmp_buf = (char *)((u32)mem_buf + bytes_read);
    mem_file = NuMemFileOpen(mem_buf, bytes_read, 1);
    if (mem_file != 0) {
        parser = NuFParOpen(mem_file);
        if (parser != NULL) {
            while (NuFParGetLine(parser) != 0) {
                ++max_strings;
            }
        }
        NuFileClose(mem_file);
    }

    strings = (NUSTRING *)((buf->addr + 3) & ~3U);
    buf->addr = (buf->addr + 3) & ~3U;
    buf->addr += max_strings * sizeof(NUSTRING);
    mem_file = NuMemFileOpen(mem_buf, bytes_read, 1);
    if (mem_file != 0) {
        parser = NuFParOpen(mem_file);
        if (parser != NULL) {
            while (NuFParGetLine(parser) != 0) {
                NuFParGetWord(parser);
                NuFParGetWord(parser);
                NU_WIDE_LENGTH(parser->word_buffer_ptr, len);
                ++len;
                strings[i].id = buf->char_ptr;
                buf->addr += (u32)len;
                NuUnicodeToAscii(strings[i].id, (u16 *)parser->word_buffer_ptr);
                NuFParGetWord(parser);
                NU_WIDE_LENGTH(parser->word_buffer_ptr, len);
                ++len;
                strings[i].str_utf16 = (u16 *)((buf->addr + 1) & ~1U);
                buf->addr = (buf->addr + 1) & ~1U;
                buf->addr += (u32)len * 2;
                {
                    const u16 *src = (const u16 *)parser->word_buffer_ptr;
                    u16 *dst = strings[i].str_utf16;
                    do {
                        *dst++ = *src;
                    } while (*src++ != 0);
                }

                if (nustring_format == 1) {
                    NU_WIDE_LENGTH(parser->word_buffer_ptr, len);
                    ++len;
                    strings[i].str_utf16 = (u16 *)((buf->addr + 1) & ~1U);
                    buf->addr = (buf->addr + 1) & ~1U;
                    buf->addr += (u32)len * 2;
                    {
                        const u16 *src = (const u16 *)parser->word_buffer_ptr;
                        u16 *dst = strings[i].str_utf16;
                        do {
                            *dst++ = *src;
                        } while (*src++ != 0);
                    }
                } else {
                    NuUnicodeToUTF8((u8 *)tmp_buf, (u16 *)parser->word_buffer_ptr);
                    len = NuStrLen(tmp_buf) + 1;
                    strings[i].str_utf8 = (u8 *)((buf->addr + 1) & ~1U);
                    buf->addr = (buf->addr + 1) & ~1U;
                    buf->addr += (u32)len;
                    NuStrCpy(strings[i].str_utf8, tmp_buf);
                }
                ++i;
            }
            bank->strings = strings;
            bank->string_count = i;
            bank->max_strings = max_strings;
        }
        NuFileClose(mem_file);
    }
    return bank->string_count;
}

u16 *__cdecl NuStringTableGetByName(char *name) {
    i32 i;
    i32 j;

    for (i = 0; i < 3; ++i) {
        for (j = 0; j < StringBank[i].string_count; ++j) {
            NUSTRING *string = &StringBank[i].strings[j];
            if (string->id != NULL && string->str_utf16 != NULL && NuStrICmp(string->id, name) == 0) {
                return string->str_utf16;
            }
        }
    }
    return (u16 *)NuBlankString;
}

void __cdecl NuStringTableSetBank(i32 bank) {
    if ((u32)bank <= 2) {
        CurrentStringBank = (i16)bank;
    }
}

void __cdecl NuStringTableSetFormat(i32 format) {
    nustring_format = (u32)format;
}

i32 __cdecl NuStringTableGetFormat(void) {
    return (i32)nustring_format;
}

void __cdecl NuStringTableUnload(void) {
    NUSTRINGBANK *bank = &StringBank[CurrentStringBank];
    bank->strings = NULL;
    bank->max_strings = 0;
    bank->string_count = 0;
}

i32 __cdecl NuStringTableGetIdByName(char *name) {
    i32 i;
    i32 j;

    for (i = 0; i < 3; ++i) {
        for (j = 0; j < StringBank[i].string_count; ++j) {
            NUSTRING *string = &StringBank[i].strings[j];
            if (string->id != NULL && string->str_utf16 != NULL && NuStrICmp(string->id, name) == 0) {
                return j + (i << 24);
            }
        }
    }
    return -1;
}

u16 *__cdecl NuStringTableGetById(i32 id) {
    u32 bank = 0;

    if ((u32)id > 0xffffff && id != -1) {
        bank = (u32)id >> 24;
        id &= 0xffffff;
    }
    if (bank > 2) {
        return NULL;
    }
    if (id < 0 || id >= StringBank[bank].string_count) {
        return NULL;
    }
    return StringBank[bank].strings[id].str_utf16;
}

void __cdecl NuStringFilterLoad(char *path, VARIPTR *buf, VARIPTR buf_end) {
    i32 count = 0;
    i32 i = 0;
    i32 length;
    i32 j;
    void *mem_buf = (void *)(buf_end.addr - 0x100000);
    i32 bytes_read = NuFileLoadBuffer(path, mem_buf, 0x100000);
    NUFILE file;
    NUFPAR_PC *parser;
    u16 word[128];

    file = NuMemFileOpen(mem_buf, bytes_read, 1);
    if (file != 0) {
        parser = NuFParOpen(file);
        if (parser != NULL) {
            do {
                if (NuFParGetWord(parser) > 0) {
                    ++count;
                }
            } while (NuFParGetLine(parser) != 0);
        }
        NuFileClose(file);
    }

    BadWords = (u16 **)((buf->addr + 3) & ~3U);
    buf->addr = (buf->addr + 3) & ~3U;
    buf->addr += (count + 1) * sizeof(u16 *);
    memset(BadWords, 0, (count + 1) * sizeof(u16 *));

    file = NuMemFileOpen(mem_buf, bytes_read, 1);
    if (file != 0) {
        parser = NuFParOpen(file);
        if (parser != NULL) {
            do {
                if (NuFParGetWord(parser) > 0) {
                    NuUTF8ToUnicode(word, (u8 *)parser->word_buffer_ptr);
                    length = 0;
                    while (word[length] != 0) {
                        ++length;
                    }
                    if (length <= 3) {
                        for (j = length; j >= 0; --j) {
                            word[j + 1] = word[j];
                        }
                        word[0] = '!';
                        ++length;
                    }
                    ++length;
                    BadWords[i] = (u16 *)((buf->addr + 1) & ~1U);
                    buf->addr = (buf->addr + 1) & ~1U;
                    buf->addr += (u32)length * 2;
                    NuStrLwrW(BadWords[i], word);
                    ++i;
                }
            } while (NuFParGetLine(parser) != 0 && i < count);
        }
        NuFileClose(file);
    }
    NumBadWords = i;
}

u16 *__cdecl NuStringBadSubString(const u16 *text, const u16 *word, i32 *length, i32 whole_word) {
    const u16 *cursor;
    const u16 *match;
    i32 boundary = 1;
    i32 equivalent;
    i32 is_fluff;

    while (*text != 0) {
        cursor = text;
        match = word;
        if (boundary || !whole_word) {
            while (*match != 0) {
                if (*cursor == 0) {
                    break;
                }
                NU_CHAR_EQUIV(*cursor, *match, equivalent);
                if (!equivalent) {
                    break;
                }
                ++cursor;
                while (*cursor != 0) {
                    NU_IS_FLUFF(*cursor, is_fluff);
                    if (!is_fluff && *cursor != *match) {
                        break;
                    }
                    ++cursor;
                }
                ++match;
            }
            if (*match == 0) {
                NU_IS_FLUFF(cursor[1], is_fluff);
                if (!whole_word || cursor[1] == 0 || is_fluff) {
                    *length = (i32)(cursor - text);
                    return (u16 *)text;
                }
            }
        }
        NU_IS_FLUFF(*text, boundary);
        ++text;
    }
    return NULL;
}

i32 __cdecl NuStringFilterBadWordsW(u16 *dest, u16 *source, u16 *replacement) {
    i32 i = 0;
    i32 changed = 0;
    i32 whole_word;
    u16 *word;
    u16 *match;
    i32 length;
    u16 lower[256];

    if (source != dest) {
        const u16 *src = source;
        u16 *dst = dest;
        do {
            *dst++ = *src;
        } while (*src++ != 0);
    }
    NuStrLwrW(lower, dest);
    while (BadWords[i] != NULL) {
        if (BadWords[i][0] == '!') {
            word = BadWords[i] + 1;
            whole_word = 1;
        } else {
            word = BadWords[i];
            whole_word = 0;
        }
        match = NuStringBadSubString(lower, word, &length, whole_word);
        if (match != NULL) {
            const u16 *src = replacement;
            u16 *dst = dest;
            do {
                *dst++ = *src;
            } while (*src++ != 0);
            changed = 1;
        }
        ++i;
    }
    return changed;
}

i32 __cdecl NuStringFilterBadWords(u8 *dest, u8 *source, u8 *replacement) {
    i32 changed;
    u16 text[128];
    u16 substitute[128];

    NuUTF8ToUnicode(text, source);
    NuUTF8ToUnicode(substitute, replacement);
    changed = NuStringFilterBadWordsW(text, text, substitute);
    NuUnicodeToUTF8(dest, text);
    return changed;
}

#undef NU_WIDE_LENGTH
#undef NU_CHAR_EQUIV
#undef NU_IS_FLUFF

i32 __cdecl NuSPrintf(char *dest, const char *format, ...) {
    va_list args;
    i32 length = 0;
    va_start(args, format);
    vsprintf(dest, format, args);
    va_end(args);
    while (dest[length] != '\0')
        length++;
    return length;
}

i32 __cdecl NuStrFindPosU(u8 *text, i32 character_index) {
    i32 index = 0, count = 0;
    if (character_index == 0)
        return 0;
    if (text[0] == 0)
        return 0;
    do {
        ++index;
        ++count;
        while ((u8)(text[index] - 0x80) <= 0x3f)
            ++index;
    } while (count != character_index && text[index] != 0);
    return index;
}

u8 __cdecl NuToUpper(u8 character) {
    if ((u8)(character - 'a') <= 'z' - 'a' || character >= 0xe0)
        character = (u8)(character - 0x20);
    return character;
}

char *__cdecl NuStrStr(char *text, const char *needle) {
    while (*text != '\0') {
        const char *part = needle;
        char *scan = text;
        while (*part != '\0' && *scan != '\0' && *scan == *part) {
            ++scan;
            ++part;
        }
        if (*part == '\0')
            return text;
        ++text;
    }
    return NULL;
}

PUSH_BLOCK_PC *__cdecl BlockInBlock(WORLDINFO *, PUSH_BLOCK_PC *, i32, PUSH_BLOCK_PC **);
void __cdecl MoveBlocksOverBlock(WORLDINFO *, PUSH_BLOCK_PC *, i32, NUVEC *);
void __cdecl MoveBlocks(WORLDINFO *, PUSH_BLOCK_PC *, i32, NUVEC *);
void __cdecl UpdatePushBlocks(void *, void *, f32);
extern f32 PushVelocitySeekRatePc;
extern f32 PushFallingSpeedPc;
extern f32 PushCornerInsetPc;
extern f32 PushGroundOffsetPc;
extern f32 PushRumblePeriodPc;
extern f32 PushRumbleWindowPc;
extern f32 PushBlockOverlapShrinkPc;
extern i32 PushBlockHintIdPc;
void __cdecl KnockPushBlock(PUSH_BLOCK_PC *, NUVEC *);
PUSH_BLOCK_PC *__cdecl NearestPushBlock(WORLDINFO *, NUVEC *, f32);
char *__cdecl NuIToA(i32, char *, i32);
extern char *PushBlocksNamePc;

char *__cdecl NuIToA(i32 value, char *buffer, i32 radix) {
    char reversed[33];
    char *digit = reversed;
    char *out = buffer;
    if (value < 0) {
        value = (i32)(0u - (u32)value);
        *out++ = '-';
    }
    do {
        *digit++ = (char)(value % radix + '0');
        value /= radix;
    } while (value != 0);
    while (digit != reversed)
        *out++ = *--digit;
    *out = '\0';
    return buffer;
}
