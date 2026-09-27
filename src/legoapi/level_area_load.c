#include "saga.h"

#include <string.h>

extern i32 old_line_pos;

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
    u16 first_word = 0;
    u8 next_byte;

    if (parser == NULL) {
        return NULL;
    }

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

    (void)NuFileRead(file_handle, &first_word, sizeof(first_word));
    parser->is_utf16 = 0;
    parser->is_utf8 = 0;

    if (first_word == 0xfffe) {
        parser->is_utf16 = 2;
    } else if (first_word == 0xfeff) {
        parser->is_utf16 = 1;
    } else if (first_word == 0xbbef || first_word == 0xefbb) {
        next_byte = 0;
        (void)NuFileRead(file_handle, &next_byte, sizeof(next_byte));
        if (next_byte == 0xbf) {
            parser->is_utf8 = first_word == 0xbbef ? 1 : 2;
        }
    }

    if (parser->is_utf16 == 0 && parser->is_utf8 == 0) {
        (void)NuFileSeek(file_handle, 0, 0);
    }

    parser->file_size = (i32)NuFileOpenSize(file_handle);
    return parser;
}

NUFPAR_PC *__cdecl NuFParCreateGivenFH(char *file_name, NUFILE file_handle) {
    NUFPAR_PC *parser;

    if (file_handle == 0) {
        return NULL;
    }

    parser = NuFParOpen(file_handle);
    if (parser == NULL) {
        NuFileClose(file_handle);
        return NULL;
    }

    NuStrCpy(parser->file_name, file_name);
    return parser;
}

NUFPAR_PC *__cdecl NuFParCreate(char *file_name) {
    NUFILE file_handle;
    NUFPAR_PC *parser;

    if (!NuFileExists(file_name)) {
        return NULL;
    }

    file_handle = NuFileOpenDF(file_name, 0, curr_dat, 0);
    if (file_handle == 0) {
        return NULL;
    }

    parser = NuFParOpen(file_handle);
    if (parser == NULL) {
        NuFileClose(file_handle);
        return NULL;
    }

    NuStrCpy(parser->file_name, file_name);
    return parser;
}

void __cdecl NuFParDestroy(NUFPAR_PC *parser) {
    NUFILE file_handle = parser->file_handle;

    NuFreeFPar(parser);
    NuFileClose(file_handle);
}

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

i32 __cdecl NuFParGetWordW(NUFPAR_PC *parser) {
    i32 length = 0;
    i32 in_quoted_text = 0;
    u16 *line = (u16 *)parser->line_buffer_ptr;
    u16 *word = (u16 *)parser->word_buffer_ptr;

    old_line_pos = parser->line_position;

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
                        word[length & ((parser->word_buffer_size >> 1) - 1)] = character;
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
    u16 character;
    i32 length = 0;
    u16 *line = (u16 *)parser->line_buffer_ptr;
    i32 at_end_of_line;
    i32 is_done = 0;
    i32 in_quoted_text = 0;

    parser->line_position = 0;
    parser->line_number++;

    for (;;) {
        character = NuGetWChar(parser);
        switch (character) {
            case '\r':
                (void)NuGetWChar(parser);
                /* fall through */
            case '\n':
                if (length == 0 || !is_done) {
                    parser->line_number++;
                    is_done = 0;
                    break;
                }
                /* fall through */
            case '\0':
                line[length] = 0;
                return length;
            case '"':
                in_quoted_text = 1 - in_quoted_text;
                is_done = 1;
                line[length & ((parser->word_buffer_size >> 1) - 1)] = character;
                length++;
                break;
            case ';':
                if (!in_quoted_text) {
                    if (length == 0 || !is_done) {
                        at_end_of_line = 0;
                        do {
                            character = NuGetWChar(parser);
                            switch (character) {
                                case '\r':
                                    (void)NuGetWChar(parser);
                                    /* fall through */
                                case '\n':
                                case '\0':
                                    at_end_of_line = 1;
                                    break;
                                default:
                                    break;
                            }
                        } while (!at_end_of_line);

                        length = 0;
                        parser->line_position = 0;
                        parser->line_number++;
                        is_done = 0;
                    } else {
                        for (;;) {
                            character = NuGetWChar(parser);
                            switch (character) {
                                case '\r':
                                    (void)NuGetWChar(parser);
                                    /* fall through */
                                case '\n':
                                case '\0':
                                    line[length] = 0;
                                    return length;
                                default:
                                    break;
                            }
                        }
                    }
                    break;
                }
                /* fall through */
            default:
                is_done = 1;
                /* fall through */
            case '\t':
            case ' ':
                line[length & ((parser->word_buffer_size >> 1) - 1)] = character;
                length++;
                break;
        }
    }
}

i32 __cdecl NuFParGetLine(NUFPAR_PC *parser) {
    char character;
    i32 length = 0;
    char *line = parser->line_buffer_ptr;
    i32 is_done = 0;
    i32 in_quoted_text = 0;

    if (parser->is_utf16 != 0) {
        return NuFParGetLineW(parser);
    }

    parser->line_position = 0;
    parser->line_number++;

    for (;;) {
        character = NuGetChar(parser);
        switch (character) {
            case '\r':
                (void)NuGetChar(parser);
                /* fall through */
            case '\n':
                if (length == 0 || !is_done) {
                    parser->line_number++;
                    is_done = 0;
                    break;
                }
                /* fall through */
            case '\0':
                line[length] = '\0';
                return length;
            case '"':
                in_quoted_text = 1 - in_quoted_text;
                is_done = 1;
                line[length & (parser->line_buffer_size - 1)] = character;
                length++;
                break;
            case ';':
                if (!in_quoted_text) {
                    if (length == 0 || !is_done) {
                        i32 at_end_of_line = 0;

                        do {
                            character = NuGetChar(parser);
                            switch (character) {
                                case '\r':
                                    (void)NuGetChar(parser);
                                    /* fall through */
                                case '\n':
                                case '\0':
                                    at_end_of_line = 1;
                                    break;
                                default:
                                    break;
                            }
                        } while (!at_end_of_line);

                        length = 0;
                        parser->line_position = 0;
                        parser->line_number++;
                        is_done = 0;
                    } else {
                        for (;;) {
                            character = NuGetChar(parser);
                            switch (character) {
                                case '\r':
                                    (void)NuGetChar(parser);
                                    /* fall through */
                                case '\n':
                                case '\0':
                                    line[length] = '\0';
                                    return length;
                                default:
                                    break;
                            }
                        }
                    }
                    break;
                }
                /* fall through */
            default:
                is_done = 1;
                /* fall through */
            case '\t':
            case ' ':
                line[length & (parser->line_buffer_size - 1)] = character;
                length++;
                break;
        }
    }
}

i32 __cdecl NuFParGetWord(NUFPAR_PC *parser) {
    i32 length = 0;
    i32 in_quoted_text = 0;
    i32 found_quotes = 0;

    if (parser->is_utf16 != 0) {
        return NuFParGetWordW(parser);
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
