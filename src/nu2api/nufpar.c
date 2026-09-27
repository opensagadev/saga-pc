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

i32 __cdecl GetLineW(u16 *line, NUFILE file) {
    i32 length = 0;
    i32 end_of_line;
    i32 has_text = 0;
    i32 quoted = 0;
    u16 character;

    for (;;) {
        character = NuFileReadWChar(file);
        switch (character) {
            case '\r':
                (void)NuFileReadWChar(file);
                /* fall through */
            case '\n':
                if (length == 0 || has_text == 0) {
                    has_text = 0;
                    break;
                }
                /* fall through */
            case '\0':
            line_complete:
                line[length] = 0;
                return length;
            case '"':
                quoted = 1 - quoted;
                has_text = 1;
                line[length++] = character;
                break;
            case ';':
                if (quoted != 0) {
                    goto append_character;
                }
                if (length == 0 || has_text == 0) {
                    end_of_line = 0;
                    do {
                        character = NuFileReadWChar(file);
                        switch (character) {
                            case '\n':
                            case '\0':
                                end_of_line = 1;
                                break;
                            case '\r':
                                (void)NuFileReadWChar(file);
                                end_of_line = 1;
                                break;
                            default:
                                break;
                        }
                    } while (end_of_line == 0);
                    length = 0;
                    has_text = 0;
                    break;
                }
                for (;;) {
                    character = NuFileReadWChar(file);
                    switch (character) {
                        case '\n':
                        case '\0':
                            goto line_complete;
                        case '\r':
                            (void)NuFileReadWChar(file);
                            goto line_complete;
                        default:
                            break;
                    }
                }
            case ' ':
            case '\t':
                if (has_text != 0) {
                    line[length++] = character;
                }
                break;
            default:
            append_character:
                has_text = 1;
                line[length++] = character;
                break;
        }
    }
}
