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
    NuStrCat(suffix, ".");
    NuStrCat(suffix, ext);
    suffix_length = NuStrLen(suffix);
    source_length = NuStrLen(src);
    if (source_length > suffix_length && NuStrNICmp(src + source_length - suffix_length, suffix, suffix_length) == 0) {
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
        NuStrCat(dst, ".");
        NuStrCat(dst, ext);
    } else {
        *period = '\0';
        NuStrCat(period, "_");
        NuStrCat(period, platform_string != NULL ? platform_string : "PC");
        NuStrCat(period, ".");
        NuStrCat(period, ext);
    }

    if (NuStrLen(dst) > dst_size - 1) {
        NuFileError();
    }
}

void __cdecl NuStrFixExt(char *dst, char *src, char *ext, i32 dst_size) {
    if (src == NULL || ext == NULL || dst == NULL) {
        return;
    }
    NuStrFixExtPlatform(dst, src, ext, dst_size, NULL);
}

void ParseCommandLine() {
    i32 argument_count = argc;
    if (argument_count > 0) {
        char **argument = argv;
        do {
            if (*argument == NULL)
                break;
            if (NuStrICmp(*argument, "PADRECORD") == 0) {
                ++argument;
                --argument_count;
                if (argument_count == 0)
                    NuFileError();
                nuapi.pad_record.filepath = *argument;
                nuapi.pad_record.mode = NUPAD_RECORD;
            } else if (NuStrICmp(*argument, "PADPLAY") == 0) {
                ++argument;
                --argument_count;
                if (argument_count == 0)
                    NuFileError();
                nuapi.pad_record.filepath = *argument;
                nuapi.pad_record.mode = NUPAD_PLAY;
            }
            --argument_count;
            ++argument;
        } while (argument_count > 0);
        argv = argument;
        argc = argument_count;
    }
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

i32 NuStrLenW(const NUWCHAR *str) {
    i32 count = 0;

    while (*str != 0) {
        count++;
        str++;
    }

    return count;
}

i32 __cdecl NuStrICmpW(NUWCHAR *a, NUWCHAR *b) {
    NUWCHAR a_cursor;
    NUWCHAR b_cursor;
    if (a == NULL)
        return -1;
    if (b == NULL)
        return 1;
    do {
        a_cursor = *a;
        if ((u16)(a_cursor - 'a') <= 'z' - 'a' || (u16)(a_cursor - 0xe0) <= 0x1f)
            a_cursor -= 0x20;
        b_cursor = *b;
        if ((u16)(b_cursor - 'a') <= 'z' - 'a' || (u16)(b_cursor - 0xe0) <= 0x1f)
            b_cursor -= 0x20;
        if (a_cursor > b_cursor)
            return 1;
        if (a_cursor < b_cursor)
            return -1;
        ++a;
        ++b;
    } while (a_cursor != 0 && b_cursor != 0);
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
        if ((u16)(value - 'A') <= 'Z' - 'A' || (u16)(value - 0xc0) <= 0x1f) {
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
    i32 upper_value;
    u32 digit;

    for (; *string != '\0'; string++) {
        value <<= 4;
        upper = (u8)*string;
        if (upper >= 'a' && upper <= 'z') {
            upper -= 0x20;
        } else if (upper >= 0xe0) {
            upper -= 0x20;
        }

        upper_value = upper;
        digit = upper_value - '0';
        if (digit <= 9) {
            value |= digit;
        } else if ((u32)(upper_value - 'A') <= 5) {
            value |= upper_value - 0x37;
        } else {
            return 0;
        }
    }
    return value;
}

i32 NuSPrintfW(NUWCHAR *dest, NUWCHAR *format, ...) {
    // The original initializes this local but never reads it.
    i32 length = 0;
    va_list args;
    char ascii_format[256];
    char text[256];
    NuUnicodeToAscii(ascii_format, format);
    va_start(args, format);
    vsprintf(text, ascii_format, args);
    va_end(args);
    NuAsciiToUnicode(dest, text);
    return NuStrLenW(dest);
}

u8 *__cdecl NuUnicodeCharFromUTF8(u16 *dst, u8 *src) {
    u16 character[3];
    character[0] = *src++;
    if (character[0] < 0x80) {
        *dst = character[0];
    } else if ((character[0] & 0xe0) == 0xc0) {
        character[1] = *src++;
        *dst = ((character[0] & 0x1f) << 6) | (character[1] & 0x3f);
    } else if ((character[0] & 0xf0) == 0xe0) {
        character[1] = *src++;
        character[2] = *src++;
        *dst = (character[0] << 12) | ((character[1] & 0x3f) << 6) | (character[2] & 0x3f);
    }
    return src;
}

u8 *__cdecl NuUTF8CharFromUnicode(u8 *dst, u16 character) {
    if (character < 0x80) {
        *dst++ = (u8)character;
    } else if (character < 0x800) {
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
            *dst++ = (u8)(((u32)*src >> 12 & 0xf) | 0xe0);
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
    i32 file_index;

    NuAsciiToUnicode(label_utf16, label);
    NuAsciiToUnicode(lang_utf16, language);
    if (bank->strings != NULL) {
        NuFileError();
    }
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

    mem_file = 0;
    if (bytes_read > 0) {
        for (file_index = 0; file_index < 20; file_index++) {
            if (pc_memfiles[file_index].used == 0) {
                pc_memfiles[file_index].buffer = (u8 *)mem_buf;
                pc_memfiles[file_index].end = (u8 *)mem_buf + bytes_read - 1;
                pc_memfiles[file_index].position = pc_memfiles[file_index].buffer;
                pc_memfiles[file_index].mode = 0;
                pc_memfiles[file_index].used = 1;
                mem_file = file_index + 0x400;
                break;
            }
        }
    }
    if (mem_file != 0) {
        parser = NuFParOpen(mem_file);
        if (parser != NULL) {
            parser->line_buffer_ptr = line_buf;
            parser->word_buffer_ptr = word_buf;
            parser->line_buffer_size = parse_buf_size;
            parser->word_buffer_size = parse_buf_size;
            while (NuFParGetLine(parser) != 0) {
                ++max_strings;
            }
            parser->file_buffer = NULL;
        }
        NuFileClose(mem_file);
    }

    strings = (NUSTRING *)((buf->addr + 3) & ~3U);
    buf->addr = (buf->addr + 3) & ~3U;
    buf->addr += max_strings * sizeof(NUSTRING);
    mem_file = 0;
    if (bytes_read > 0) {
        for (file_index = 0; file_index < 20; file_index++) {
            if (pc_memfiles[file_index].used == 0) {
                pc_memfiles[file_index].buffer = (u8 *)mem_buf;
                pc_memfiles[file_index].end = (u8 *)mem_buf + bytes_read - 1;
                pc_memfiles[file_index].position = pc_memfiles[file_index].buffer;
                pc_memfiles[file_index].mode = 0;
                pc_memfiles[file_index].used = 1;
                mem_file = file_index + 0x400;
                break;
            }
        }
    }
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
                    if (NuStrICmpW((NUWCHAR *)parser->word_buffer_ptr, label_utf16) == 0) {
                        label_column = column;
                    }
                    if (NuStrICmpW((NUWCHAR *)parser->word_buffer_ptr, lang_utf16) == 0) {
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
                                NuStrCpyW(strings[i].str_utf16, (NUWCHAR *)parser->word_buffer_ptr);
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
            parser->file_buffer = NULL;
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
    i32 file_index;

    if (bank->strings != NULL) {
        NuFileError();
    }
    bank->strings = NULL;
    bank->max_strings = 0;
    bank->string_count = 0;
    bytes_read = NuFileLoadBuffer(filepath, mem_buf, 0x100000);
    if ((i32)(0x100000 - bytes_read) < 0x8000) {
        NuFileError();
    }
    tmp_buf = (char *)((u32)mem_buf + bytes_read);
    mem_file = 0;
    if (bytes_read > 0) {
        for (file_index = 0; file_index < 20; file_index++) {
            if (pc_memfiles[file_index].used == 0) {
                pc_memfiles[file_index].buffer = (u8 *)mem_buf;
                pc_memfiles[file_index].end = (u8 *)mem_buf + bytes_read - 1;
                pc_memfiles[file_index].position = pc_memfiles[file_index].buffer;
                pc_memfiles[file_index].mode = 0;
                pc_memfiles[file_index].used = 1;
                mem_file = file_index + 0x400;
                break;
            }
        }
    }
    if (mem_file != 0) {
        parser = NuFParOpen(mem_file);
        if (parser != NULL) {
            while (NuFParGetLine(parser) != 0) {
                ++max_strings;
            }
            parser->file_buffer = NULL;
        }
        NuFileClose(mem_file);
    }

    strings = (NUSTRING *)((buf->addr + 3) & ~3U);
    buf->addr = (buf->addr + 3) & ~3U;
    buf->addr += max_strings * sizeof(NUSTRING);
    mem_file = 0;
    if (bytes_read > 0) {
        for (file_index = 0; file_index < 20; file_index++) {
            if (pc_memfiles[file_index].used == 0) {
                pc_memfiles[file_index].buffer = (u8 *)mem_buf;
                pc_memfiles[file_index].end = (u8 *)mem_buf + bytes_read - 1;
                pc_memfiles[file_index].position = pc_memfiles[file_index].buffer;
                pc_memfiles[file_index].mode = 0;
                pc_memfiles[file_index].used = 1;
                mem_file = file_index + 0x400;
                break;
            }
        }
    }
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
                NuStrCpyW(strings[i].str_utf16, (NUWCHAR *)parser->word_buffer_ptr);

                if (nustring_format == 1) {
                    NU_WIDE_LENGTH(parser->word_buffer_ptr, len);
                    ++len;
                    strings[i].str_utf16 = (u16 *)((buf->addr + 1) & ~1U);
                    buf->addr = (buf->addr + 1) & ~1U;
                    buf->addr += (u32)len * 2;
                    NuStrCpyW(strings[i].str_utf16, (NUWCHAR *)parser->word_buffer_ptr);
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
            parser->file_buffer = NULL;
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
                return StringBank[i].strings[j].str_utf16;
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

i32 __cdecl NuStringTableLoad(char *filepath, VARIPTR *buf, VARIPTR buf_end) {
    char *extension = NuStrRChr(filepath, '.');

    if (extension != NULL && NuStrICmp(extension, ".csv") == 0) {
        return NuStringTableLoadCSV(filepath, buf, buf_end, "LABEL", "ENGLISH");
    }
    return NuStringTableLoadTXT(filepath, buf, buf_end);
}

void __cdecl NuStringFilterLoad(char *path, VARIPTR *buf, VARIPTR buf_end) {
    i32 count;
    i32 i;
    i32 length;
    i32 j;
    void *mem_buf;
    i32 bytes_read;
    NUFILE file;
    NUFPAR_PC *parser;
    u16 word[128];
    i32 file_index;

    if (buf_end.addr - buf->addr < 0x100000) {
        NuFileError();
    }
    mem_buf = (void *)(buf_end.addr - 0x100000);
    bytes_read = NuFileLoadBuffer(path, mem_buf, 0x100000);
    count = 0;

    file = 0;
    if (bytes_read > 0) {
        for (file_index = 0; file_index < 20; file_index++) {
            if (pc_memfiles[file_index].used == 0) {
                pc_memfiles[file_index].buffer = (u8 *)mem_buf;
                pc_memfiles[file_index].end = (u8 *)mem_buf + bytes_read - 1;
                pc_memfiles[file_index].position = pc_memfiles[file_index].buffer;
                pc_memfiles[file_index].mode = 0;
                pc_memfiles[file_index].used = 1;
                file = file_index + 0x400;
                break;
            }
        }
    }
    if (file != 0) {
        parser = NuFParOpen(file);
        if (parser != NULL) {
            do {
                if (NuFParGetWord(parser) > 0) {
                    ++count;
                }
            } while (NuFParGetLine(parser) != 0);
            parser->file_buffer = NULL;
        }
        NuFileClose(file);
    }

    BadWords = (u16 **)((buf->addr + 3) & ~3U);
    buf->addr = (buf->addr + 3) & ~3U;
    buf->addr += (count + 1) * sizeof(u16 *);
    memset(BadWords, 0, (count + 1) * sizeof(u16 *));
    i = 0;

    file = 0;
    if (bytes_read > 0) {
        for (file_index = 0; file_index < 20; file_index++) {
            if (pc_memfiles[file_index].used == 0) {
                pc_memfiles[file_index].buffer = (u8 *)mem_buf;
                pc_memfiles[file_index].end = (u8 *)mem_buf + bytes_read - 1;
                pc_memfiles[file_index].position = pc_memfiles[file_index].buffer;
                pc_memfiles[file_index].mode = 0;
                pc_memfiles[file_index].used = 1;
                file = file_index + 0x400;
                break;
            }
        }
    }
    if (file != 0) {
        parser = NuFParOpen(file);
        if (parser != NULL) {
            do {
                if (NuFParGetWord(parser) > 0) {
                    NuUTF8ToUnicode(word, (u8 *)parser->word_buffer_ptr);
                    length = NuStrLenW(word);
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
            parser->file_buffer = NULL;
        }
        NuFileClose(file);
    }
    NumBadWords = i;
}

static i32 NuStringCharEquivalentPc(u16 a, u16 b) {
    i32 i = 0;
    if (a == b)
        return 1;
    while (bad_equiv[i][0] != 0) {
        if ((bad_equiv[i][1] == a && bad_equiv[i][0] == b) || (bad_equiv[i][1] == b && bad_equiv[i][0] == a))
            return 1;
        ++i;
    }
    return 0;
}

static i32 NuStringIsFluffPc(u16 character) {
    i32 i = 0;
    while (bad_fluff[i] != 0) {
        if (bad_fluff[i] == character)
            return 1;
        ++i;
    }
    return 0;
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
                equivalent = NuStringCharEquivalentPc(*cursor, *match);
                if (!equivalent) {
                    break;
                }
                ++cursor;
                while (*cursor != 0) {
                    is_fluff = NuStringIsFluffPc(*cursor);
                    if (!is_fluff && *cursor != *match) {
                        break;
                    }
                    ++cursor;
                }
                ++match;
            }
            if (*match == 0) {
                if (!whole_word || cursor[1] == 0) {
                    is_fluff = 1;
                } else {
                    is_fluff = NuStringIsFluffPc(cursor[1]);
                }
                if (is_fluff) {
                    *length = (i32)(cursor - text);
                    return (u16 *)text;
                }
            }
        }
        boundary = NuStringIsFluffPc(*text);
        ++text;
    }
    return NULL;
}

i32 NuStrCpyW(u16 *dst, const u16 *src) {
    i32 count = 0;
    if (src == NULL) {
        *dst = 0;
    } else {
        do {
            *dst++ = *src;
            ++count;
        } while (*src++ != 0);
    }
    return count;
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
        NuStrCpyW(dest, source);
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
            NuStrCpyW(dest, replacement);
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

i32 __cdecl NuVSPrintf(char *dest, const char *format, va_list args) {
    return vsprintf(dest, format, args);
}

i32 __cdecl NuSPrintf(char *dest, const char *format, ...) {
    va_list args;
    i32 length;
    va_start(args, format);
    vsprintf(dest, format, args);
    va_end(args);
    length = 0;
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

void __cdecl NuStrToLower(char *text) {
    u8 value;
    while (*text != 0) {
        value = (u8)*text;
        if ((u8)(value - 0x41) <= 0x19 || (u8)(value + 0x40) <= 0x1f)
            value = (u8)(value + 0x20);
        *text++ = (char)value;
    }
}

/* SOURCE proposal: ordinary wide-string counterparts in the existing nustring owner.
 * Work on raw 16-bit units, including Native's broad Latin-1 case conversion.
 * Counts preserve the PC's subtract-then-test behavior and 32-bit wrap; these
 * are not bounded libc replacements and do not add missing-pointer guards.
 */
i32 __cdecl NuStrNCatW(NUWCHAR *str, const NUWCHAR *ext, i32 n) {
    i32 copied;
    while (*str != 0)
        ++str;
    copied = 0;
    if (ext != NULL) {
        do {
            if (n == 0)
                break;
            *str = *ext;
            ++str;
            ++copied;
            n = (i32)((u32)n - 1u);
        } while (*ext++ != 0);
    }
    return copied;
}

i32 __cdecl NuStrNCpyW(NUWCHAR *dst, const NUWCHAR *src, i32 n) {
    i32 copied = 0;
    if (src == NULL) {
        *dst = 0;
        return 0;
    }
    do {
        n = (i32)((u32)n - 1u);
        ++copied;
        if (n <= 0) {
            *dst = 0;
            break;
        }
        *dst = *src;
        ++dst;
    } while (*src++ != 0);
    return copied - 1;
}

NUWCHAR *__cdecl NuStrStrW(NUWCHAR *str, const NUWCHAR *sub) {
    while (*str != 0) {
        NUWCHAR *cursor = str;
        const NUWCHAR *match = sub;
        while (*match != 0) {
            if (*cursor == 0)
                break;
            if (*cursor != *match)
                break;
            ++cursor;
            ++match;
        }
        if (*match == 0)
            return str;
        ++str;
    }
    return NULL;
}

NUWCHAR *__cdecl NuStrIStrW(NUWCHAR *str, const NUWCHAR *sub) {
    while (*str != 0) {
        NUWCHAR *cursor = str;
        const NUWCHAR *match = sub;
        while (*match != 0) {
            NUWCHAR ca;
            NUWCHAR cb;
            if (*cursor == 0)
                break;
            ca = *cursor;
            ca = NU_UTF16_UPPER(ca);
            cb = *match;
            cb = NU_UTF16_UPPER(cb);
            if (ca != cb)
                break;
            ++cursor;
            ++match;
        }
        if (*match == 0)
            return str;
        ++str;
    }
    return NULL;
}

i32 __cdecl NuStrCmpW(const NUWCHAR *a, const NUWCHAR *b) {
    NUWCHAR ca;
    NUWCHAR cb;
    if (a == NULL)
        return -1;
    if (b == NULL)
        return 1;
    do {
        ca = *a;
        cb = *b;
        if (ca > cb)
            return 1;
        if (ca < cb)
            return -1;
        ++a;
        ++b;
    } while (ca != 0 && cb != 0);
    return 0;
}

i32 __cdecl NuStrNCmpW(const NUWCHAR *a, const NUWCHAR *b, i32 n) {
    NUWCHAR ca;
    NUWCHAR cb;
    if (a == NULL)
        return -1;
    if (b == NULL)
        return 1;
    if (n == 0)
        return 0;
    if (n == -1)
        n = NuStrLenW(a);
    else if (n == -2)
        n = NuStrLenW(b);
    do {
        ca = *a;
        cb = *b;
        if (ca > cb)
            return 1;
        if (ca < cb)
            return -1;
        ++a;
        ++b;
        n = (i32)((u32)n - 1u);
    } while (ca != 0 && cb != 0 && n != 0);
    return 0;
}

i32 __cdecl NuStrNICmpW(const NUWCHAR *a, const NUWCHAR *b, i32 n) {
    NUWCHAR ca;
    NUWCHAR cb;
    if (a == NULL)
        return -1;
    if (b == NULL)
        return 1;
    if (n == 0)
        return 0;
    if (n == -1)
        n = NuStrLenW(a);
    else if (n == -2)
        n = NuStrLenW(b);
    do {
        ca = *a;
        ca = NU_UTF16_UPPER(ca);
        cb = *b;
        cb = NU_UTF16_UPPER(cb);
        if (ca > cb)
            return 1;
        if (ca < cb)
            return -1;
        ++a;
        ++b;
        n = (i32)((u32)n - 1u);
    } while (ca != 0 && cb != 0 && n != 0);
    return 0;
}

void __cdecl NuStrUprW(NUWCHAR *dst, const NUWCHAR *src) {
    while (*src != 0) {
        NUWCHAR character = *src;
        *dst = NU_UTF16_UPPER(character);
        ++dst;
        ++src;
    }
    *dst = *src;
}

/* SOURCE-only R69 carry. Raw wide units and PC case ranges, not Unicode/locale
 * conversion. The character functions return NUWCHAR: only AX is the defined
 * return value; incidental high EAX bits are not a 32-bit integer contract.
 */
NUWCHAR __cdecl NuToUpperW(NUWCHAR character) {
    return NU_UTF16_UPPER(character);
}

NUWCHAR __cdecl NuToLowerW(NUWCHAR character) {
    if ((character >= 0x41 && character <= 0x5a) || (character >= 0xc0 && character <= 0xdf))
        character += 0x20;
    return character;
}

void __cdecl NuStrCatW(NUWCHAR *str, const NUWCHAR *ext) {
    while (*str != 0)
        ++str;
    if (ext != NULL) {
        do {
            *str = *ext;
            ++str;
        } while (*ext++ != 0);
    }
}

NUWCHAR *__cdecl NuStrChrW(NUWCHAR *str, NUWCHAR character) {
    while (*str != 0) {
        if (*str == character)
            return str;
        ++str;
    }
    return NULL;
}

NUWCHAR *__cdecl NuStrRChrW(NUWCHAR *str, NUWCHAR character) {
    NUWCHAR *cursor = str;
    while (*cursor != 0)
        ++cursor;
    while (cursor >= str) {
        if (*cursor == character)
            return cursor;
        --cursor;
    }
    return NULL;
}

i32 __cdecl NuStrToL(char *str, char **end, i32 radix) {
    i32 value = 0;
    i32 sign = 0;
    char c = *str++;
    while (c == ' ' || c == '\t')
        c = *str++;
    if (c == '-') {
        sign = -1;
        c = *str++;
    } else if (c == '+')
        c = *str++;
    if (radix == 0) {
        radix = 10;
        if (c == '0' && (*str == 'x' || *str == 'X')) {
            radix = 16;
            ++str;
            c = *str;
        }
    }
    while (1) {
        if (c >= '0' && c <= '9')
            c -= '0';
        else if (c >= 'A' && c <= 'Z')
            c -= 'A' - 10;
        else if (c >= 'a' && c <= 'z')
            c -= 'a' - 10;
        else
            break;
        if (c >= radix)
            break;
        value = (i32)((u32)value * (u32)radix + (u32)c);
        c = *str++;
    }
    if (end != NULL)
        *end = str - 1;
    return sign != 0 ? (i32)((u32)value * (u32)sign) : value;
}

NUWCHAR *__cdecl NuIToAW(i32 value, NUWCHAR *buffer, i32 radix) {
    NUWCHAR reversed[33];
    NUWCHAR *digit = reversed;
    NUWCHAR *out = buffer;
    if (value < 0) {
        value = (i32)(0u - (u32)value);
        *out++ = '-';
    }
    do {
        *digit++ = value % radix + '0';
        value /= radix;
    } while (value != 0);
    while (digit != reversed)
        *out++ = *--digit;
    *out++ = 0;
    return buffer;
}

i32 __cdecl NuAToIW(const NUWCHAR *str) {
    i32 value = 0;
    i32 sign = 0;
    NUWCHAR c = *str++;
    if (c == '-') {
        sign = -1;
        c = *str++;
    }
    while (c >= '0' && c <= '9') {
        value = (i32)((u32)value * 10u + (u32)c - (u32)'0');
        c = *str++;
    }
    return sign != 0 ? (i32)((u32)value * (u32)sign) : value;
}

i32 __cdecl NuStrToLW(NUWCHAR *str, NUWCHAR **end, i32 radix) {
    i32 value = 0;
    i32 sign = 0;
    NUWCHAR c = *str++;
    while (c == ' ' || c == '\t')
        c = *str++;
    if (c == '-') {
        sign = -1;
        c = *str++;
    } else if (c == '+')
        c = *str++;
    if (radix == 0) {
        radix = 10;
        if (c == '0' && (*str == 'x' || *str == 'X')) {
            radix = 16;
            ++str;
            c = *str;
        }
    }
    while (1) {
        if (c >= '0' && c <= '9')
            c -= '0';
        else if (c >= 'A' && c <= 'Z')
            c -= 'A' - 10;
        else if (c >= 'a' && c <= 'z')
            c -= 'a' - 10;
        else
            break;
        if (c >= radix)
            break;
        value = (i32)((u32)value * (u32)radix + (u32)c);
        c = *str++;
    }
    if (end != NULL)
        *end = str - 1;
    return sign != 0 ? (i32)((u32)value * (u32)sign) : value;
}

void __cdecl NuStrGetFilenameNoExt(char *dst, char *src) {
    char *separator;
    char *backslash;
    i32 length;
    char *extension = NuStrRChr(src, '.');
    if (extension == NULL)
        extension = src + (NuStrLen(src) - 1);
    separator = NuStrRChr(src, '/');
    backslash = NuStrRChr(src, '\\');
    if (backslash > separator)
        separator = backslash;
    if (separator == NULL)
        separator = src;
    // Preserve the original's unconditional advance, even for a bare filename.
    ++separator;
    length = extension - separator + 1;
    NuStrNCpy(dst, separator, length);
}

void __cdecl NuStrGetPath(char *dst, char *src) {
    char *separator = NuStrRChr(src, '/');
    char *backslash = NuStrRChr(src, '\\');
    if (backslash > separator)
        separator = backslash;
    if (separator == NULL) {
        *dst = 0;
    } else {
        i32 length = separator - src + 1;
        NuStrNCpy(dst, src, length);
    }
}

void __cdecl NuStrGetExt(char *dst, char *src) {
    char *separator;
    char *backslash;
    char *extension = NuStrRChr(src, '.');
    separator = NuStrRChr(src, '/');
    backslash = NuStrRChr(src, '\\');
    if (backslash > separator)
        separator = backslash;
    if (extension == NULL || separator > extension) {
        *dst = 0;
    } else {
        char *end = src + NuStrLen(src);
        i32 length = end - extension;
        if (length == 0)
            *dst = 0;
        else
            NuStrNCpy(dst, extension + 1, length);
    }
}

void __cdecl NuStrSubstituteString(char *dst, char *src, char *search, char *replacement) {
    i32 matched = 0;
    while (*src != 0) {
        while (search[matched] != 0) {
            u8 source_character = (u8)src[matched];
            u8 search_character = (u8)search[matched];
            if ((u8)(source_character - 'A') <= 0x19 || (u8)(source_character + 0x40) <= 0x1f)
                source_character = (u8)(source_character + 0x20);
            if ((u8)(search_character - 'A') <= 0x19 || (u8)(search_character + 0x40) <= 0x1f)
                search_character = (u8)(search_character + 0x20);
            if (source_character != search_character) {
                matched = 0;
                break;
            }
            ++matched;
        }
        if (matched != 0) {
            src += matched;
            if (replacement != NULL) {
                matched = 0;
                while (replacement[matched] != 0) {
                    *dst = replacement[matched];
                    ++dst;
                    ++matched;
                }
            }
            matched = 0;
        } else {
            *dst = *src;
            ++dst;
            ++src;
        }
    }
    *dst = 0;
}

/* Native unused string utilities; SOURCE-only proposal. */
i32 __cdecl NuStrCpyWC(char *dst, const char *src, const char *replacement) {
    i32 copied = 0;
    if (src == NULL) {
        *dst = '\0';
    } else {
        do {
            if (*src != '*') {
                *dst = *src;
                ++dst;
                ++copied;
            } else {
                const char *cursor = replacement;
                if (cursor != NULL) {
                    while (cursor != NULL && *cursor != '\0') {
                        *dst = *cursor;
                        ++dst;
                        ++cursor;
                        ++copied;
                    }
                }
            }
        } while (*src++ != '\0');
    }
    return copied;
}

i32 __cdecl NuIsAl(char c) {
    switch (c) {
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case '_':
            return 1;
        default:
            return 0;
    }
}

i32 __cdecl NuIsAlW(NUWCHAR c) {
    switch (c) {
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case '_':
            return 1;
        default:
            return 0;
    }
}

u8 __cdecl NuToLower(u8 c) {
    if (c >= 'A' && c <= 'Z') {
        c += 0x20;
    } else if (c > 0xbf && c < 0xe0) {
        c += 0x20;
    }
    return c;
}

f32 __cdecl NuAToFW(const NUWCHAR *str) {
    f32 value = 0.0f;
    f32 divisor = 1.0f;
    NUWCHAR c = *str++;
    if (c == '-') {
        divisor = -1.0f;
        c = *str++;
    }
    while (c >= '0' && c <= '9') {
        value *= 10.0f;
        value += c - '0';
        c = *str++;
    }
    if (c == '.') {
        c = *str++;
        while (c >= '0' && c <= '9') {
            divisor *= 10.0f;
            value *= 10.0f;
            value += c - '0';
            c = *str++;
        }
    }
    return value / divisor;
}

i32 __cdecl NuStringTok(char *str, ...) {
    char *token;
    i32 value;
    va_list args;
    va_start(args, str);
    do {
        token = va_arg(args, char *);
        value = va_arg(args, i32);
        if (token != NULL) {
            if (NuStrICmp(str, token) == 0)
                break;
        }
    } while (token != NULL);
    va_end(args);
    return value;
}

i32 __cdecl NuStrICmpWC(char *pattern, char *text, char *wildcard) {
    i32 index;
    char a, b;
    i32 suffix_length, text_length, wildcard_length;
    if (pattern == NULL)
        return -1;
    if (text == NULL)
        return 1;
    while (*pattern != '\0' && *pattern != '*') {
        a = NuToUpper((u8)*pattern);
        b = NuToUpper((u8)*text);
        if (a > b)
            return 1;
        if (a < b)
            return -1;
        ++pattern;
        ++text;
    }
    if (*pattern == '*') {
        ++pattern;
        suffix_length = NuStrLen(pattern);
        text_length = NuStrLen(text);
        wildcard_length = text_length - suffix_length;
        if (wildcard_length < 0)
            return 1;
        if (wildcard != NULL) {
            for (index = 0; index < wildcard_length; ++index)
                wildcard[index] = text[index];
            wildcard[index] = '\0';
        }
        text += text_length - suffix_length;
    } else if (wildcard != NULL) {
        *wildcard = '\0';
    }
    while (*pattern != '\0') {
        a = NuToUpper((u8)*pattern);
        b = NuToUpper((u8)*text);
        if (a > b)
            return 1;
        if (a < b)
            return -1;
        ++pattern;
        ++text;
    }
    return 0;
}

extern i32 old_line_pos;

#define CLAMP_LINE(position) ((position) & (parser->line_buffer_size - 1))
#define CLAMP_WORD(position) ((position) & (parser->word_buffer_size - 1))
#define CLAMP_WIDE_LINE(position) ((position) & ((parser->line_buffer_size >> 1) - 1))
#define CLAMP_WIDE_WORD(position) ((position) & ((parser->word_buffer_size >> 1) - 1))

void __cdecl NuFParGetPos(NUFPAR_PC *parser, NUFPARPOS_PC *position) {
    position->line_number = parser->line_number;
    position->line_position = parser->line_position;
    position->character_position = parser->character_position;
    position->buffer_start = parser->buffer_start;
    position->buffer_end = parser->buffer_end;
}

void __cdecl NuFParSetPos(NUFPAR_PC *parser, NUFPARPOS_PC *position) {
    parser->character_position = position->character_position;
    parser->line_position = position->line_position;

    if (parser->character_position < parser->buffer_start || parser->character_position > parser->buffer_end) {
        parser->buffer_end = parser->character_position - 1;
    }
}

static NUFPAR_PC *NuAllocFPar(void) {
    i32 index;

    if (NuFParRecordBase == NULL) {
        NuFParRecordBase = NuFParRecords;
        memset(NuFParRecordBase, 0, sizeof(NuFParRecords));
        NuFParBufferBase = &NuFParBuffers[0][0];
    }

    for (index = 0; index < 4; index++) {
        NUFPAR_PC *parser = &NuFParRecordBase[index];

        if (parser->file_buffer == NULL) {
            memset(parser, 0, sizeof(*parser));
            parser->file_buffer = NuFParBufferBase + index * 0x1000;
            memset(parser->file_buffer, 0, 0x1000);
            return parser;
        }
    }

    return NULL;
}

void __cdecl NuFreeFPar(NUFPAR_PC *parser) {
    parser->file_buffer = NULL;
}

NUFPAR_PC *__cdecl NuFParOpen(NUFILE file_handle) {
    NUFPAR_PC *parser = NuAllocFPar();
    u16 first_word;

    if (parser != NULL) {
        parser->file_handle = file_handle;
        parser->buffer_end = -1;
        parser->line_number = -1;
        parser->command_position = -1;
        parser->separator_list = NULL;
        parser->separator_tokens = NULL;
        parser->line_buffer_ptr = parser->line_buffer;
        parser->word_buffer_ptr = parser->word_buffer;
        parser->line_buffer_size = 0x200;
        parser->word_buffer_size = 0x200;

        first_word = 0;
        NuFileRead(file_handle, &first_word, sizeof(first_word));
        parser->is_utf16 = 0;
        parser->is_utf8 = 0;

        if (first_word == 0xfffe) {
            parser->is_utf16 = 2;
        } else if (first_word == 0xfeff) {
            parser->is_utf16 = 1;
        } else if (first_word == 0xbbef) {
            if ((u8)NuFileReadChar(file_handle) == 0xbf) {
                parser->is_utf8 = 1;
            }
        } else if (first_word == 0xefbb) {
            if ((u8)NuFileReadChar(file_handle) == 0xbf) {
                parser->is_utf8 = 2;
            }
        }

        if (parser->is_utf16 == 0 && parser->is_utf8 == 0) {
            NuFileSeek(file_handle, 0, 0);
        }

        parser->file_size = (i32)NuFileOpenSize(file_handle);
    }
    return parser;
}

NUFPAR_PC *__cdecl NuFParCreateGivenFH(char *file_name, NUFILE file_handle) {
    NUFPAR_PC *parser;

    if (file_handle != 0) {
        parser = NuFParOpen(file_handle);
        if (parser != NULL) {
            NuStrCpy(parser->file_name, file_name);
            return parser;
        }
        NuFileClose(file_handle);
    }
    return NULL;
}

NUFPAR_PC *__cdecl NuFParCreate(char *file_name) {
    NUFILE file_handle;
    NUFPAR_PC *parser;

    if (NuFileExists(file_name)) {
        file_handle = NuFileOpenDF(file_name, 0, curr_dat, 0);
        if (file_handle != 0) {
            parser = NuFParOpen(file_handle);
            if (parser != NULL) {
                NuStrCpy(parser->file_name, file_name);
                return parser;
            }
            NuFileClose(file_handle);
        }
    }
    return NULL;
}

void __cdecl NuFParSuspend(NUFPAR_PC *parser) {
    NUFILE file_handle = parser->file_handle;

    if (file_handle >= 0x400 && file_handle < 0x800) {
        return;
    }

    if (parser->file_name[0] == '\0' || file_handle == 0) {
        NuFileError();
    }

    NuFileClose(file_handle);
    parser->file_handle = 0;
}

void __cdecl NuFParResume(NUFPAR_PC *parser) {
    NUFILE file_handle;

    if (parser->file_handle >= 0x400 && parser->file_handle < 0x800) {
        return;
    }

    if (parser->file_name[0] == '\0') {
        NuFileError();
    }

    file_handle = NuFileOpenDF(parser->file_name, 0, curr_dat, 0);
    parser->file_handle = file_handle;
    if (file_handle == 0) {
        NuFileError();
    }

    (void)NuFileSeek(file_handle, parser->character_position, 0);
    parser->buffer_end = parser->character_position - 1;
}

void __cdecl NuFParDestroy(NUFPAR_PC *parser) {
    NUFILE file_handle = parser->file_handle;

    NuFreeFPar(parser);
    NuFileClose(file_handle);
}

char __cdecl NuGetChar(NUFPAR_PC *parser) {
    char c;
    i32 len;

    i32 read_to = parser->buffer_end >= 0 ? parser->buffer_end : 0;

    if (parser->character_position > parser->buffer_end) {
        if (parser->buffer_end + 1 <= parser->file_size) {
            len = NuFileRead(parser->file_handle, parser->file_buffer,
                             parser->file_size - read_to <= 0x1000 ? parser->file_size - read_to : 0x1000);

            parser->buffer_start = parser->buffer_end + 1;
            parser->buffer_end += len;
        } else {
            return '\0';
        }

        if (len == 0) {
            return '\0';
        }
    }

    c = parser->file_buffer[parser->character_position - parser->buffer_start];
    parser->character_position++;

    return c;
}

u16 __cdecl NuGetWChar(NUFPAR_PC *parser) {
    unsigned char lo;
    unsigned char hi;
    u16 c;

    lo = NuGetChar(parser);
    hi = NuGetChar(parser);

    c = lo + (hi << 8);

    return c;
}

i32 __cdecl NuFParGetWordW(NUFPAR_PC *parser) {
    i32 length;
    i32 in_quoted_text;
    u16 *line;
    u16 *word;

    old_line_pos = parser->line_position;
    line = (u16 *)parser->line_buffer_ptr;
    word = (u16 *)parser->word_buffer_ptr;
    length = 0;
    in_quoted_text = 0;

    while (line[parser->line_position & ((parser->line_buffer_size >> 1) - 1)] != 0) {
        u16 character = line[parser->line_position];

        if (parser->separator_tokens != NULL && !in_quoted_text &&
            NuStrChr(parser->separator_tokens, (char)character) != NULL) {
            if (length == 0) {
                word[0] = character;
                length++;
                parser->line_position++;
            }
            word[length & ((parser->word_buffer_size >> 1) - 1)] = 0;
            return length;
        }

        if (parser->separator_list != NULL && !in_quoted_text &&
            NuStrChr(parser->separator_list, (char)character) != NULL) {
            character = ' ';
        }

        switch (character & 0xff) {
            case ' ':
            case ',':
            case '\t':
                if (!in_quoted_text) {
                    if (length != 0) {
                        word[length & ((parser->word_buffer_size >> 1) - 1)] = 0;
                        return length;
                    }
                    break;
                }
                /* fall through */
            default:
                if (character == '"') {
                    if (in_quoted_text && line[parser->line_position + 1] == '"') {
                        word[length & ((parser->word_buffer_size >> 1) - 1)] = '"';
                        length++;
                        parser->line_position++;
                    } else {
                        in_quoted_text = 1 - in_quoted_text;
                    }
                } else {
                    word[length & ((parser->word_buffer_size >> 1) - 1)] = character;
                    length++;
                }
                break;
        }

        parser->line_position++;
    }

    word[length & ((parser->word_buffer_size >> 1) - 1)] = 0;
    return length;
}

i32 __cdecl NuFParGetLineW(NUFPAR_PC *parser) {
    u16 c;
    i32 len;
    u16 *line;
    i32 at_end_of_line;
    i32 is_done;
    i32 in_quoted_text;

    in_quoted_text = 0;
    line = (u16 *)parser->line_buffer_ptr;
    len = 0;
    parser->line_position = 0;
    parser->line_number++;
    is_done = 0;

    do {
        c = NuGetWChar(parser);
        switch (c) {
            case '\r':
                NuGetWChar(parser);
            case '\n':
                if (len == 0 || !is_done) {
                    parser->line_number++;
                    is_done = 0;

                    break;
                }
            case '\0':
                line[len] = '\0';
                return len;
            case '"':
                in_quoted_text = 1 - in_quoted_text;
                is_done = 1;
                line[CLAMP_WIDE_WORD(len)] = c;
                len++;
                break;
            case ';':
                if (!in_quoted_text) {
                    if (len == 0 || !is_done) {
                        at_end_of_line = 0;

                        do {
                            c = NuGetWChar(parser);
                            switch (c) {
                                case '\r':
                                    NuGetWChar(parser);
                                case '\n':
                                case '\0':
                                    at_end_of_line = 1;
                                    break;
                                default:
                                    break;
                            }
                        } while (!at_end_of_line);

                        len = 0;
                        parser->line_position = 0;
                        parser->line_number++;
                        is_done = 0;
                    } else {
                        do {
                            c = NuGetWChar(parser);
                            switch (c) {
                                case '\r':
                                    NuGetWChar(parser);
                                case '\n':
                                case '\0':
                                    line[len] = '\0';
                                    return len;
                                default:
                                    break;
                            }
                        } while (1);

                        return len;
                    }

                    break;
                }
            default:
                is_done = 1;
            case '\t':
            case ' ':
                line[CLAMP_WIDE_WORD(len)] = c;
                len++;
                break;
        }
    } while (1);
}

i32 __cdecl NuFParGetLine(NUFPAR_PC *parser) {
    char c;
    i32 len;
    i32 at_end_of_line;
    i32 is_done;
    i32 in_quoted_text;

    in_quoted_text = 0;

    if (parser->is_utf16) {
        return NuFParGetLineW(parser);
    }

    len = 0;
    parser->line_position = 0;
    parser->line_number++;
    is_done = 0;

    do {
        c = NuGetChar(parser);
        switch (c) {
            case '\r':
                NuGetChar(parser);
            case '\n':
                if (len == 0 || !is_done) {
                    parser->line_number++;
                    is_done = 0;

                    break;
                }
            case '\0':
                parser->line_buffer_ptr[len] = '\0';
                return len;
            case '"':
                in_quoted_text = 1 - in_quoted_text;
                is_done = 1;
                parser->line_buffer_ptr[CLAMP_LINE(len)] = c;
                len++;
                break;
            case ';':
                if (!in_quoted_text) {
                    if (len == 0 || !is_done) {
                        at_end_of_line = 0;

                        do {
                            c = NuGetChar(parser);
                            switch (c) {
                                case '\r':
                                    NuGetChar(parser);
                                case '\n':
                                case '\0':
                                    at_end_of_line = 1;
                                    break;
                                default:
                                    break;
                            }
                        } while (!at_end_of_line);

                        len = 0;
                        parser->line_position = 0;
                        parser->line_number++;
                        is_done = 0;
                    } else {
                        do {
                            c = NuGetChar(parser);
                            switch (c) {
                                case '\r':
                                    NuGetChar(parser);
                                case '\n':
                                case '\0':
                                    parser->line_buffer_ptr[len] = '\0';
                                    return len;
                                default:
                                    break;
                            }
                        } while (1);

                        return len;
                    }

                    break;
                }
            default:
                is_done = 1;
            case '\t':
            case ' ':
                parser->line_buffer_ptr[CLAMP_LINE(len)] = c;
                len++;
                break;
        }
    } while (1);
}

i32 __cdecl NuFParGetWord(NUFPAR_PC *parser) {
    i32 length = 0;
    i32 in_quoted_text = 0;
    i32 found_quotes = 0;

    if (parser->is_utf16 != 0) {
        return NuFParGetWordW(parser);
    }

    old_line_pos = parser->line_position;

    while (parser->line_buffer_ptr[parser->line_position & (parser->line_buffer_size - 1)] != '\0') {
        char character = parser->line_buffer_ptr[parser->line_position];

        if (parser->separator_tokens != NULL && !in_quoted_text &&
            NuStrChr(parser->separator_tokens, character) != NULL) {
            if (length == 0) {
                parser->word_buffer_ptr[0] = character;
                length++;
                parser->line_position++;
            }
            parser->word_buffer_ptr[length & (parser->word_buffer_size - 1)] = '\0';
            return length;
        }

        if (parser->separator_list != NULL && !in_quoted_text && NuStrChr(parser->separator_list, character) != NULL) {
            character = ' ';
        }

        switch (character) {
            case ' ':
            case ',':
            case '\t':
                if (!in_quoted_text) {
                    if (length != 0) {
                        parser->word_buffer_ptr[length & (parser->word_buffer_size - 1)] = '\0';
                        return length;
                    }
                    break;
                }
                /* fall through */
            default:
                if (character == '"') {
                    in_quoted_text = 1 - in_quoted_text;
                    found_quotes = 1;
                } else {
                    parser->word_buffer_ptr[length & (parser->word_buffer_size - 1)] = character;
                    length++;
                }
                break;
        }

        parser->line_position++;
        if (found_quotes && in_quoted_text == 0 && length == 0) {
            break;
        }
    }

    parser->word_buffer_ptr[length & (parser->word_buffer_size - 1)] = '\0';
    return length;
}

f32 __cdecl NuAToF(char *string) {
    f32 dividend = 0.0f;
    f32 divisor = 1.0f;
    char character = *string++;

    if (character == '-') {
        divisor = -1.0f;
        character = *string++;
    }

    while (character >= '0' && character <= '9') {
        dividend *= 10.0;
        dividend += character - '0';
        character = *string++;
    }

    if (character == '.') {
        character = *string++;
        while (character >= '0' && character <= '9') {
            divisor *= 10.0;
            dividend *= 10.0;
            dividend += character - '0';
            character = *string++;
        }
    }

    return dividend / divisor;
}

i32 __cdecl NuAToI(char *string) {
    i32 value = 0;
    i32 sign = 0;
    char character = *string++;

    if (character == '-') {
        sign = -1;
        character = *string++;
    }

    while (character >= '0' && character <= '9') {
        value = (value << 3) + 2 * value;
        value = value + character - '0';
        character = *string++;
    }

    if (sign != 0) {
        return value * sign;
    }
    return value;
}

void __cdecl NuUnicodeToAscii(char *destination, u16 *source) {
    i32 index = 0;

    if (source == NULL || destination == NULL) {
        return;
    }

    *destination = '\0';
    for (; source[index] != 0; index++) {
        if ((source[index] & 0xff00) != 0) {
            switch (source[index]) {
                case 0x2018:
                    destination[index] = (char)0x91;
                    break;
                case 0x2019:
                    destination[index] = (char)0x92;
                    break;
                case 0x2013:
                    destination[index] = (char)0x96;
                    break;
                case 0x2026:
                    destination[index] = (char)0x85;
                    break;
                case 0x2122:
                    destination[index] = (char)0x99;
                    break;
                default:
                    destination[index] = '?';
                    break;
            }
        } else {
            destination[index] = (char)source[index];
        }
    }
    destination[index] = '\0';
}

i32 __cdecl NuFParGetInt(NUFPAR_PC *parser) {
    char buffer[64];

    NuFParGetWord(parser);
    if (parser->is_utf16 != 0) {
        NuUnicodeToAscii(buffer, (u16 *)parser->word_buffer_ptr);
    } else {
        NuStrCpy(buffer, parser->word_buffer_ptr);
    }

    if (buffer[0] == '\0') {
        return 0;
    }
    if (buffer[0] == '$') {
        return NuHexStringToI(buffer + 1);
    }
    if (buffer[0] == '0' && (buffer[1] == 'x' || buffer[1] == 'X')) {
        return NuHexStringToI(buffer + 2);
    }
    return NuAToI(buffer);
}

f32 __cdecl NuFParGetFloat(NUFPAR_PC *parser) {
    char buffer[64];

    NuFParGetWord(parser);
    if (parser->is_utf16 != 0) {
        NuUnicodeToAscii(buffer, (u16 *)parser->word_buffer_ptr);
    } else {
        NuStrCpy(buffer, parser->word_buffer_ptr);
    }

    return buffer[0] != '\0' ? NuAToF(buffer) : 0.0f;
}

i32 __cdecl NuFParPushCom2(NUFPAR_PC *parser, NUFPCOMJMP *commands, NUFPCOMJMP *commands2) {
    if (parser->command_position >= 7) {
        return -1;
    }
    parser->command_position++;
    parser->command_stack[parser->command_position] = commands;
    parser->command_stack2[parser->command_position] = commands2;
    return parser->command_position;
}

i32 __cdecl NuFParInterpretWord(NUFPAR_PC *parser) {
    char buffer[64];
    i32 index;
    NUFPCOMJMP *commands;

    if (parser->is_utf16 != 0) {
        NuUnicodeToAscii(buffer, (u16 *)parser->word_buffer_ptr);
    } else {
        NuStrCpy(buffer, parser->word_buffer_ptr);
    }
    if (buffer[0] == '\0' || buffer[0] == ';') {
        return 0;
    }

    if (parser->command_position > -1) {
        commands = parser->command_stack[parser->command_position];
        for (index = 0; commands[index].keyword != NULL; index++) {
            if (NuStrICmp(commands[index].keyword, buffer) == 0) {
                commands[index].callback(parser);
                return 1;
            }
        }
        commands = parser->command_stack2[parser->command_position];
        if (commands != NULL) {
            for (index = 0; commands[index].keyword != NULL; index++) {
                if (NuStrICmp(commands[index].keyword, buffer) == 0) {
                    commands[index].callback(parser);
                    return 1;
                }
            }
        }
    }

    if (fnInterpreterError != NULL) {
        fnInterpreterError(parser);
    }
    return 0;
}

void __cdecl NuFParUnGetWord(NUFPAR_PC *parser) {
    parser->line_position = old_line_pos;
}

void __cdecl NuFParGetOptionalFloat(NUFPAR_PC *parser, f32 *value) {
    char buffer[64];

    NuFParGetWord(parser);
    if (parser->is_utf16 != 0) {
        NuUnicodeToAscii(buffer, (u16 *)parser->word_buffer_ptr);
    } else {
        NuStrCpy(buffer, parser->word_buffer_ptr);
    }

    if (value != NULL && buffer[0] != '\0') {
        *value = NuAToF(buffer);
        if (*value == 0.0f) {
            NuFParUnGetWord(parser);
        }
    }
}

void __cdecl NuFParGetOptionalInt(NUFPAR_PC *parser, i32 *value) {
    char buffer[64];

    NuFParGetWord(parser);
    if (parser->is_utf16 != 0) {
        NuUnicodeToAscii(buffer, (u16 *)parser->word_buffer_ptr);
    } else {
        NuStrCpy(buffer, parser->word_buffer_ptr);
    }

    if (value != NULL && buffer[0] != '\0') {
        *value = NuAToI(buffer);
        if (*value == 0) {
            NuFParUnGetWord(parser);
        }
    }
}

i32 __cdecl NuFParPushCom(NUFPAR_PC *parser, NUFPCOMJMP *commands) {
    if (parser->command_position >= 7) {
        return -1;
    }

    parser->command_position++;
    parser->command_stack[parser->command_position] = commands;
    parser->command_stack2[parser->command_position] = NULL;
    return parser->command_position;
}

i32 __cdecl NuFParPushComCTX(NUFPAR_PC *parser, NUFPCOMJMPCTX *commands) {
    if (parser->command_position >= 7) {
        return -1;
    }

    parser->command_position++;
    parser->command_stack_ctx[parser->command_position] = commands;
    parser->command_stack2_ctx[parser->command_position] = NULL;
    return parser->command_position;
}

i32 __cdecl NuFParPushComCTX2(NUFPAR_PC *parser, NUFPCOMJMPCTX *commands, NUFPCOMJMPCTX *commands2) {
    if (parser->command_position >= 7) {
        return -1;
    }

    parser->command_position++;
    parser->command_stack_ctx[parser->command_position] = commands;
    parser->command_stack2_ctx[parser->command_position] = commands2;
    return parser->command_position;
}

void __cdecl NuFParPopCom(NUFPAR_PC *parser) {
    if (parser->command_position > -1) {
        parser->command_position--;
    }
}

NUFPCOMFN __cdecl NuFParSetInterpreterErrorHandler(NUFPCOMFN handler) {
    NUFPCOMFN previous = fnInterpreterError;

    fnInterpreterError = handler;
    return previous;
}

i32 __cdecl NuFParInterpretWordCTX(NUFPAR_PC *parser, void *context) {
    char buffer[64];
    i32 index;
    NUFPCOMJMPCTX *commands;

    if (parser->is_utf16 != 0) {
        NuUnicodeToAscii(buffer, (u16 *)parser->word_buffer_ptr);
    } else {
        NuStrCpy(buffer, parser->word_buffer_ptr);
    }
    if (buffer[0] == '\0' || buffer[0] == ';') {
        return 0;
    }

    if (parser->command_position > -1) {
        commands = parser->command_stack_ctx[parser->command_position];
        for (index = 0; commands[index].keyword != NULL; index++) {
            if (NuStrICmp(commands[index].keyword, buffer) == 0) {
                commands[index].callback(parser, context);
                return 1;
            }
        }
        commands = parser->command_stack2_ctx[parser->command_position];
        if (commands != NULL) {
            for (index = 0; commands[index].keyword != NULL; index++) {
                if (NuStrICmp(commands[index].keyword, buffer) == 0) {
                    commands[index].callback(parser, context);
                    return 1;
                }
            }
        }
    }

    if (fnInterpreterError != NULL) {
        fnInterpreterError(parser);
    }
    return 0;
}

typedef struct nurdpfcontext_pc_s {
    i32 token;
    f32 value;
    char *input;
    i32 position;
    i32(__cdecl *get_variable)(char *, f32 *, i32 *);
} NURDPFCTX_PC;

STATIC_ASSERT(sizeof(NURDPFCTX_PC) == 20);
STATIC_ASSERT(offsetof(NURDPFCTX_PC, token) == 0);
STATIC_ASSERT(offsetof(NURDPFCTX_PC, value) == 4);
STATIC_ASSERT(offsetof(NURDPFCTX_PC, input) == 8);
STATIC_ASSERT(offsetof(NURDPFCTX_PC, position) == 12);
STATIC_ASSERT(offsetof(NURDPFCTX_PC, get_variable) == 16);

enum {
    RDPF_ADD,
    RDPF_SUBTRACT,
    RDPF_MULTIPLY,
    RDPF_DIVIDE,
    RDPF_LEFT_PAREN,
    RDPF_RIGHT_PAREN,
    RDPF_END,
    RDPF_NUMBER,
    RDPF_VARIABLE
};

static void NuRDPFGetToken(NURDPFCTX_PC *context) {
    char variable[256];
    i32 length;
    f32 multiplier;

    for (;;) {
        if (context->input[context->position] == '\0') {
            context->token = RDPF_END;
            return;
        }
        if (context->input[context->position] != ' ' && context->input[context->position] != '\t') {
            break;
        }
        /* The PC increments the whitespace byte, not the input position. */
        context->input[context->position]++;
    }

    switch (context->input[context->position]) {
        case ';':
            context->token = RDPF_END;
            return;
        case '+':
            context->token = RDPF_ADD;
            context->position++;
            return;
        case '-':
            context->token = RDPF_SUBTRACT;
            context->position++;
            return;
        case '*':
            context->token = RDPF_MULTIPLY;
            context->position++;
            return;
        case '/':
            context->token = RDPF_DIVIDE;
            context->position++;
            return;
        case '(':
            context->token = RDPF_LEFT_PAREN;
            context->position++;
            return;
        case ')':
            context->token = RDPF_RIGHT_PAREN;
            context->position++;
            return;
        case '.':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            context->token = RDPF_NUMBER;
            context->value = 0.0f;
            multiplier = 0.0f;
            while ((context->input[context->position] >= '0' && context->input[context->position] <= '9') ||
                   context->input[context->position] == '.') {
                if (context->input[context->position] == '.') {
                    if (multiplier != 0.0f) {
                        NuFileError();
                    }
                    multiplier = 0.1f;
                } else if (multiplier != 0.0f) {
                    context->value += (context->input[context->position] - '0') * multiplier;
                    multiplier /= 10.0;
                } else {
                    context->value *= 10.0;
                    context->value += context->input[context->position] - '0';
                }
                context->position++;
            }
            return;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '_':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
            length = 0;
            while (NuIsAlNum(context->input[context->position])) {
                variable[length++] = context->input[context->position++];
            }
            variable[length] = '\0';
            if (context->get_variable == NULL) {
                NuFileError();
            }
            context->value = 0.0f;
            context->token = RDPF_NUMBER;
            context->get_variable(variable, &context->value, NULL);
            return;
        default:
            return;
    }
}

static f32 NuRDPFExpression(NURDPFCTX_PC *context);

static f32 NuRDPFPrimary(NURDPFCTX_PC *context) {
    f32 value;

    switch (context->token) {
        case RDPF_VARIABLE:
            NuFileError();
            /* fall through */
        case RDPF_NUMBER:
            NuRDPFGetToken(context);
            return context->value;
        case RDPF_SUBTRACT:
            NuRDPFGetToken(context);
            return -NuRDPFPrimary(context);
        case RDPF_LEFT_PAREN:
            NuRDPFGetToken(context);
            value = NuRDPFExpression(context);
            if (context->token != RDPF_RIGHT_PAREN) {
                NuFileError();
            }
            NuRDPFGetToken(context);
            return value;
        case RDPF_END:
            return 1.0f;
        default:
            NuFileError();
            return 0.0f;
    }
}

static f32 NuRDPFTerm(NURDPFCTX_PC *context) {
    f32 value = NuRDPFPrimary(context);

    for (;;) {
        switch (context->token) {
            case RDPF_MULTIPLY:
                NuRDPFGetToken(context);
                value *= NuRDPFPrimary(context);
                break;
            case RDPF_DIVIDE:
                NuRDPFGetToken(context);
                value /= NuRDPFPrimary(context);
                break;
            default:
                return value;
        }
    }
}

static f32 NuRDPFExpression(NURDPFCTX_PC *context) {
    f32 value = NuRDPFTerm(context);

    for (;;) {
        switch (context->token) {
            case RDPF_ADD:
                NuRDPFGetToken(context);
                value += NuRDPFTerm(context);
                break;
            case RDPF_SUBTRACT:
                NuRDPFGetToken(context);
                value -= NuRDPFTerm(context);
                break;
            default:
                return value;
        }
    }
}

f32 __cdecl NuRDPFVar(char *input, i32(__cdecl *get_variable)(char *, f32 *, i32 *)) {
    NURDPFCTX_PC context;

    context.input = input;
    context.position = 0;
    context.get_variable = get_variable;
    NuRDPFGetToken(&context);
    return NuRDPFExpression(&context);
}

f32 __cdecl NuFParGetFloatRDP(NUFPAR_PC *parser) {
    char buffer[64];

    NuFParGetWord(parser);
    if (parser->is_utf16 != 0) {
        NuUnicodeToAscii(buffer, (u16 *)parser->word_buffer_ptr);
    } else {
        NuStrCpy(buffer, parser->word_buffer_ptr);
    }
    return buffer[0] != '\0' ? NuRDPFVar(buffer, NULL) : 0.0f;
}

typedef struct nurdpicontext_pc_s {
    i32 token;
    i32 value;
    char *input;
    i32 position;
    NURDPGETVARFN get_variable;
} NURDPICTX_PC;

STATIC_ASSERT(sizeof(NURDPICTX_PC) == 20);
STATIC_ASSERT(offsetof(NURDPICTX_PC, token) == 0);
STATIC_ASSERT(offsetof(NURDPICTX_PC, value) == 4);
STATIC_ASSERT(offsetof(NURDPICTX_PC, input) == 8);
STATIC_ASSERT(offsetof(NURDPICTX_PC, position) == 12);
STATIC_ASSERT(offsetof(NURDPICTX_PC, get_variable) == 16);

enum {
    RDPI_ADD,
    RDPI_SUBTRACT,
    RDPI_MULTIPLY,
    RDPI_DIVIDE,
    RDPI_MODULO,
    RDPI_LEFT_PAREN,
    RDPI_RIGHT_PAREN,
    RDPI_END,
    RDPI_NUMBER,
    RDPI_OR,
    RDPI_AND,
    RDPI_NOT,
    RDPI_XOR,
    RDPI_LOGICAL_OR,
    RDPI_LOGICAL_AND,
    RDPI_LOGICAL_NOT,
    RDPI_EQUAL,
    RDPI_NOT_EQUAL,
    RDPI_LESS,
    RDPI_GREATER,
    RDPI_LESS_EQUAL,
    RDPI_GREATER_EQUAL
};

static void NuRDPIGetToken(NURDPICTX_PC *context) {
    char variable[256];
    i32 length;

    for (;;) {
        if (context->input[context->position] == '\0') {
            context->token = RDPI_END;
            return;
        }
        if (context->input[context->position] != ' ' && context->input[context->position] != '\t') {
            break;
        }
        /* The PC increments the whitespace byte, not the input position. */
        context->input[context->position]++;
    }

    switch (context->input[context->position]) {
        case ';':
            context->token = RDPI_END;
            return;
        case '+':
            context->token = RDPI_ADD;
            context->position++;
            return;
        case '-':
            context->token = RDPI_SUBTRACT;
            context->position++;
            return;
        case '*':
            context->token = RDPI_MULTIPLY;
            context->position++;
            return;
        case '/':
            context->token = RDPI_DIVIDE;
            context->position++;
            return;
        case '%':
            context->token = RDPI_MODULO;
            context->position++;
            return;
        case '(':
            context->token = RDPI_LEFT_PAREN;
            context->position++;
            return;
        case ')':
            context->token = RDPI_RIGHT_PAREN;
            context->position++;
            return;
        case '|':
            context->token = RDPI_OR;
            context->position++;
            if (context->input[context->position] == '|') {
                context->token = RDPI_LOGICAL_OR;
                context->position++;
            }
            return;
        case '&':
            context->token = RDPI_AND;
            context->position++;
            if (context->input[context->position] == '&') {
                context->token = RDPI_LOGICAL_AND;
                context->position++;
            }
            return;
        case '=':
            context->token = RDPI_EQUAL;
            context->position++;
            if (context->input[context->position] == '=') {
                context->position++;
            }
            return;
        case '>':
            context->token = RDPI_GREATER;
            context->position++;
            if (context->input[context->position] == '=') {
                context->position++;
                context->token = RDPI_GREATER_EQUAL;
            }
            return;
        case '<':
            context->token = RDPI_LESS;
            context->position++;
            if (context->input[context->position] == '=') {
                context->position++;
                context->token = RDPI_LESS_EQUAL;
            }
            return;
        case '^':
            context->token = RDPI_XOR;
            context->position++;
            return;
        case '!':
            context->token = RDPI_LOGICAL_NOT;
            context->position++;
            if (context->input[context->position] == '=') {
                context->token = RDPI_NOT_EQUAL;
                context->position++;
            }
            return;
        case '~':
            context->token = RDPI_NOT;
            context->position++;
            return;
        case '$':
            context->position++;
            context->token = RDPI_NUMBER;
            context->value = 0;
            while (ishexnum(context->input[context->position])) {
                context->value <<= 4;
                context->value += hexcodetoint(context->input[context->position]);
                context->position++;
            }
            return;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            context->token = RDPI_NUMBER;
            context->value = 0;
            while ((u8)(context->input[context->position] - '0') <= 9) {
                context->value *= 10;
                context->value += context->input[context->position] - '0';
                context->position++;
            }
            return;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
            length = 0;
            while (NuIsAlNum(context->input[context->position])) {
                variable[length++] = context->input[context->position++];
            }
            variable[length] = '\0';
            if (context->get_variable == NULL) {
                NuFileError();
            }
            context->value = 0;
            context->token = RDPI_NUMBER;
            context->get_variable(variable, NULL, &context->value);
            return;
        default:
            return;
    }
}

static i32 NuRDPIExpression(NURDPICTX_PC *context);

static i32 NuRDPIPrimary(NURDPICTX_PC *context) {
    i32 value;

    switch (context->token) {
        case RDPI_NUMBER:
            NuRDPIGetToken(context);
            return context->value;
        case RDPI_SUBTRACT:
            NuRDPIGetToken(context);
            return -NuRDPIPrimary(context);
        case RDPI_NOT:
            NuRDPIGetToken(context);
            return ~NuRDPIPrimary(context);
        case RDPI_LOGICAL_NOT:
            NuRDPIGetToken(context);
            return !NuRDPIPrimary(context);
        case RDPI_LEFT_PAREN:
            NuRDPIGetToken(context);
            value = NuRDPIExpression(context);
            if (context->token != RDPI_RIGHT_PAREN) {
                NuFileError();
            }
            NuRDPIGetToken(context);
            return value;
        case RDPI_END:
            return 1;
        default:
            NuFileError();
            return 0;
    }
}

static i32 NuRDPITerm(NURDPICTX_PC *context) {
    i32 value = NuRDPIPrimary(context);

    for (;;) {
        switch (context->token) {
            case RDPI_MULTIPLY:
                NuRDPIGetToken(context);
                value *= NuRDPIPrimary(context);
                break;
            case RDPI_DIVIDE:
                NuRDPIGetToken(context);
                value /= NuRDPIPrimary(context);
                break;
            case RDPI_MODULO:
                NuRDPIGetToken(context);
                value %= NuRDPIPrimary(context);
                break;
            case RDPI_EQUAL:
                NuRDPIGetToken(context);
                value = value == NuRDPIPrimary(context);
                break;
            case RDPI_NOT_EQUAL:
                NuRDPIGetToken(context);
                value = value != NuRDPIPrimary(context);
                break;
            case RDPI_LESS:
                NuRDPIGetToken(context);
                value = value < NuRDPIPrimary(context);
                break;
            case RDPI_GREATER:
                NuRDPIGetToken(context);
                value = value > NuRDPIPrimary(context);
                break;
            case RDPI_LESS_EQUAL:
                NuRDPIGetToken(context);
                value = value <= NuRDPIPrimary(context);
                break;
            case RDPI_GREATER_EQUAL:
                NuRDPIGetToken(context);
                value = value >= NuRDPIPrimary(context);
                break;
            default:
                return value;
        }
    }
}

static i32 NuRDPIExpression(NURDPICTX_PC *context) {
    i32 right;
    i32 value = NuRDPITerm(context);

    for (;;) {
        switch (context->token) {
            case RDPI_ADD:
                NuRDPIGetToken(context);
                value += NuRDPITerm(context);
                break;
            case RDPI_SUBTRACT:
                NuRDPIGetToken(context);
                value -= NuRDPITerm(context);
                break;
            case RDPI_AND:
                NuRDPIGetToken(context);
                value &= NuRDPITerm(context);
                break;
            case RDPI_OR:
                NuRDPIGetToken(context);
                value |= NuRDPITerm(context);
                break;
            case RDPI_XOR:
                NuRDPIGetToken(context);
                value ^= NuRDPITerm(context);
                break;
            case RDPI_LOGICAL_AND:
                NuRDPIGetToken(context);
                right = NuRDPITerm(context);
                value = value != 0 && right != 0;
                break;
            case RDPI_LOGICAL_OR:
                NuRDPIGetToken(context);
                right = NuRDPITerm(context);
                value = value != 0 || right != 0;
                break;
            default:
                return value;
        }
    }
}

i32 __cdecl NuRDPIVar(char *input, NURDPGETVARFN get_variable) {
    NURDPICTX_PC context;

    context.input = input;
    context.position = 0;
    context.get_variable = get_variable;
    NuRDPIGetToken(&context);
    return NuRDPIExpression(&context);
}

i32 __cdecl NuRDPI(char *input) {
    return NuRDPIVar(input, NULL);
}

i32 __cdecl NuFParGetIntRDP(NUFPAR_PC *parser) {
    char buffer[64];

    NuFParGetWord(parser);
    if (parser->is_utf16 != 0) {
        NuUnicodeToAscii(buffer, (u16 *)parser->word_buffer_ptr);
    } else {
        NuStrCpy(buffer, parser->word_buffer_ptr);
    }
    return buffer[0] != '\0' ? NuRDPI(buffer) : 0;
}

void __cdecl NuFParInit(i32 max_count, i32 buffer_size) {
    NuFParRecordBase = NuFParRecords;
    memset(NuFParRecordBase, 0, sizeof(NuFParRecords));
    NuFParBufferBase = &NuFParBuffers[0][0];
}

f32 __cdecl NuRDPF(char *input) {
    return NuRDPFVar(input, NULL);
}
