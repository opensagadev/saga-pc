#include "saga.h"

#include <string.h>

extern i32 old_line_pos;

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

NUFPAR_PC *__cdecl NuFParCreateMem(char *name, char *buffer, i32 buffer_size) {
    NUFILE file_handle;
    NUFPAR_PC *parser;
    i32 index;

    if (buffer_size == 0 || buffer == NULL || buffer_size <= 0) {
        return NULL;
    }
    for (index = 0; index < 20; index++) {
        if (pc_memfiles[index].used == 0) {
            break;
        }
    }
    if (index == 20) {
        return NULL;
    }

    pc_memfiles[index].buffer = (u8 *)buffer;
    pc_memfiles[index].end = (u8 *)buffer + buffer_size - 1;
    pc_memfiles[index].position = (u8 *)buffer;
    pc_memfiles[index].mode = 0;
    pc_memfiles[index].used = 1;
    file_handle = index + 0x400;
    parser = NuFParOpen(file_handle);
    if (parser != NULL) {
        NuStrCpy(parser->file_name, name);
        return parser;
    }

    NuFileClose(file_handle);
    return NULL;
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
