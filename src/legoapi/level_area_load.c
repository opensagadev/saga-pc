#include "saga.h"

typedef struct nufpar_pc_s {
    char *file_buffer;
    NUFILE file_handle;
    char file_name[0x100];
    char line_buffer[0x202];
    char word_buffer[0x202];
    char *line_buffer_ptr;
    char *word_buffer_ptr;
    i32 line_buffer_size;
    i32 word_buffer_size;
    i32 line_number;
    i32 line_position;
    i32 character_position;
    i32 buffer_start;
    i32 buffer_end;
    u8 unknown_0530[0x40];
    i32 command_position;
    i32 file_size;
    u8 is_utf16;
    u8 is_utf8;
    u8 unknown_057a[2];
    char *separator_list;
    char *separator_tokens;
} NUFPAR_PC;

STATIC_ASSERT(offsetof(NUFPAR_PC, file_handle) == 0x4);
STATIC_ASSERT(offsetof(NUFPAR_PC, line_buffer) == 0x108);
STATIC_ASSERT(offsetof(NUFPAR_PC, word_buffer) == 0x30a);
STATIC_ASSERT(offsetof(NUFPAR_PC, line_buffer_ptr) == 0x50c);
STATIC_ASSERT(offsetof(NUFPAR_PC, word_buffer_ptr) == 0x510);
STATIC_ASSERT(offsetof(NUFPAR_PC, line_buffer_size) == 0x514);
STATIC_ASSERT(offsetof(NUFPAR_PC, word_buffer_size) == 0x518);
STATIC_ASSERT(offsetof(NUFPAR_PC, line_number) == 0x51c);
STATIC_ASSERT(offsetof(NUFPAR_PC, line_position) == 0x520);
STATIC_ASSERT(offsetof(NUFPAR_PC, character_position) == 0x524);
STATIC_ASSERT(offsetof(NUFPAR_PC, buffer_start) == 0x528);
STATIC_ASSERT(offsetof(NUFPAR_PC, buffer_end) == 0x52c);
STATIC_ASSERT(offsetof(NUFPAR_PC, is_utf16) == 0x578);
STATIC_ASSERT(offsetof(NUFPAR_PC, is_utf8) == 0x579);
STATIC_ASSERT(offsetof(NUFPAR_PC, separator_list) == 0x57c);
STATIC_ASSERT(offsetof(NUFPAR_PC, separator_tokens) == 0x580);
STATIC_ASSERT(sizeof(NUFPAR_PC) == 0x584);

extern i32 old_line_pos;

char __cdecl NuGetChar(NUFPAR_PC *parser) {
    i32 offset = parser->buffer_end < 0 ? 0 : parser->buffer_end;

    if (parser->character_position > parser->buffer_end) {
        i32 bytes_to_read;
        i32 bytes_read;

        if (parser->buffer_end + 1 > parser->file_size) {
            return '\0';
        }

        bytes_to_read = parser->file_size - offset;
        if (bytes_to_read > 0x1000) {
            bytes_to_read = 0x1000;
        }
        bytes_read = NuFileRead(parser->file_handle, parser->file_buffer, bytes_to_read);
        parser->buffer_start = parser->buffer_end + 1;
        parser->buffer_end += bytes_read;
        if (bytes_read == 0) {
            return '\0';
        }
    }

    {
        char value = parser->file_buffer[parser->character_position - parser->buffer_start];
        parser->character_position++;
        return value;
    }
}

u16 __cdecl NuGetWChar(NUFPAR_PC *parser) {
    u16 low_byte = (u8)NuGetChar(parser);
    u16 high_byte = (u8)NuGetChar(parser);

    return (u16)(low_byte | (u16)(high_byte << 8));
}

i32 __cdecl NuFParGetWord(NUFPAR_PC *parser) {
    i32 length = 0;
    i32 in_quoted_text = 0;
    i32 found_quotes = 0;

    if (parser->is_utf16 != 0) {
        STUBBED_NAMED("NuFParGetWord: UTF-16 word parser at PC 0x004f7df0");
        return 0;
    }

    old_line_pos = parser->line_position;

    while (parser->line_buffer_ptr[parser->line_position] != '\0') {
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

LEVELDATA_PC *__cdecl Levels_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end, i32 max_count,
                                           i32 *count_out, void *set_defaults_fn) {
    (void)path;
    (void)buffer;
    (void)buffer_end;
    (void)max_count;
    (void)count_out;
    (void)set_defaults_fn;

    /* PC parsing depends on NuFPar and the still-unrecovered LEVELDATA_PC fields. */
    STUBBED_NAMED("Levels_ConfigureList: NuFPar parser and full PC level record mapping");
    return NULL;
}

AREA_DATA_PC *__cdecl Areas_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end, i32 max_count,
                                          i32 *count_out) {
    (void)path;
    (void)buffer;
    (void)buffer_end;
    (void)max_count;

    /* PC parsing also needs NuFPar plus name resolution and the full area record mapping. */
    STUBBED_NAMED("Areas_ConfigureList: NuFPar parser, level lookup and full PC area record mapping");
    if (count_out != NULL) {
        *count_out = 0;
    }
    return NULL;
}
