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

typedef void(__cdecl *LEVELSETDEFAULTS_PC)(LEVELDATA_PC *level);

LEVELDATA_PC *__cdecl Levels_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end, i32 max_count,
                                           i32 *count_out, void *set_defaults_fn) {
    NUFPAR_PC *parser;
    LEVELDATA_PC *level_data;
    LEVELDATA_PC *current_level;
    i32 in_level_config = 0;
    i32 level_count = 0;
    u32 aligned_address;

    parser = NuFParCreate(path);
    MAXLDATA = max_count;
    aligned_address = (buffer->addr + 3) & ~3u;
    buffer->addr = aligned_address;
    level_data = (LEVELDATA_PC *)buffer->void_ptr;
    current_level = level_data;

    while (NuFParGetLine(parser) != 0) {
        char *word;

        NuFParGetWord(parser);
        word = parser->word_buffer_ptr;
        if (word[0] == '\0') {
            continue;
        }

        if (in_level_config) {
            if (NuStrICmp(word, LevelConfigKeywordEnd) == 0) {
                in_level_config = 0;
                if (current_level->dir[0] == '\0' || current_level->name[0] == '\0' ||
                    (current_level->flags & 0x200) != 0) {
                    continue;
                }

                if ((current_level->flags & 0x10000) != 0) {
                    NEWGAME_LDATA = current_level;
                }
                if ((current_level->flags & 0x20000) != 0) {
                    LOADGAME_LDATA = current_level;
                }

                level_count++;
                current_level++;
                continue;
            }

            if (NuStrICmp(word, LevelConfigKeywordDir) == 0) {
                if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buffer_ptr) < 0x40) {
                    NuStrCpy(current_level->dir, parser->word_buffer_ptr);
                }
            } else if (NuStrICmp(word, LevelConfigKeywordFile) == 0) {
                if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buffer_ptr) < 0x20) {
                    NuStrCpy(current_level->name, parser->word_buffer_ptr);
                }
            }

            if (NuStrICmp(parser->word_buffer_ptr, LevelConfigKeywordTest) == 0) {
                current_level->flags |= 0x200;
            } else if (NuStrICmp(parser->word_buffer_ptr, LevelConfigKeywordIntro) == 0) {
                current_level->flags |= 0x20;
            } else if (NuStrICmp(parser->word_buffer_ptr, LevelConfigKeywordMidtro) == 0 ||
                       NuStrICmp(parser->word_buffer_ptr, LevelConfigKeywordCutscene) == 0) {
                current_level->flags |= 0x40;
            } else if (NuStrICmp(parser->word_buffer_ptr, LevelConfigKeywordOutro) == 0) {
                current_level->flags |= 0x80;
            } else if (NuStrICmp(parser->word_buffer_ptr, LevelConfigKeywordStatus) == 0) {
                current_level->flags &= ~0x2u;
                current_level->flags &= ~0x8u;
                current_level->flags |= 0x400;
            } else if (NuStrICmp(parser->word_buffer_ptr, LevelConfigKeywordNewGame) == 0) {
                if (NEWGAME_LDATA == NULL) {
                    current_level->flags |= 0x10000;
                }
            } else if (NuStrICmp(parser->word_buffer_ptr, LevelConfigKeywordLoadGame) == 0) {
                if (LOADGAME_LDATA == NULL) {
                    current_level->flags |= 0x20000;
                }
            }
            continue;
        }

        if (NuStrICmp(word, LevelConfigKeywordStart) == 0 && level_count < MAXLDATA) {
            i32 music_index;

            current_level->dir[0] = '\0';
            current_level->name[0] = '\0';
            current_level->unknown_0060 = -1;
            current_level->index = (i16)level_count;
            current_level->flags = 0xe;
            current_level->callbacks[0] = NULL;
            current_level->callbacks[1] = NULL;
            current_level->callbacks[2] = NULL;
            current_level->callbacks[3] = NULL;
            current_level->callbacks[4] = NULL;
            current_level->callbacks[5] = NULL;
            current_level->callbacks[6] = NULL;

            current_level->display.value_00 = PCKeyboardDefaultValue_candidate;
            current_level->display.value_04 = 0.15f;
            current_level->display.value_08 = 20000;
            current_level->display.colors[0] = 0;
            current_level->display.colors[1] = 0;
            current_level->display.colors[2] = 0;
            current_level->display.colors[3] = 0;
            current_level->display.colors[4] = 0;
            current_level->display.colors[5] = 0;

            current_level->music_index = -1;
            current_level->unknown_0096 = -1;
            current_level->max_ter_platforms = 0x80;
            current_level->max_ter_groups = 0x100;
            current_level->unknown_009c = -1;
            current_level->unknown_009e = -1;
            current_level->mode = 3;
            current_level->alpha = 0x7f;
            current_level->unknown_00a2 = 0xff;
            current_level->area_index = -1;
            current_level->camera_distance = 0.0f;
            current_level->hover_distance = 0.0f;

            current_level->unknown_00ac[0] = 0x50;
            current_level->unknown_00ac[1] = 0x50;
            current_level->unknown_00ac[2] = 0x50;
            current_level->unknown_00ac[3] = 0x32;
            current_level->unknown_00ac[4] = 0;
            current_level->unknown_00ac[5] = 0;
            current_level->unknown_00ac[6] = 0;
            current_level->unknown_00ac[7] = 0;

            current_level->unknown_00b4[0] = 0.5f;
            current_level->unknown_00b4[1] = 0.5f;
            current_level->unknown_00b4[2] = 0.5f;
            current_level->unknown_00b4[3] = LevelConfigFarDistance;
            current_level->area_level_index = -1;
            current_level->unknown_00c9[0] = 5;
            current_level->unknown_00c9[1] = 10;
            current_level->unknown_00c9[2] = 5;
            current_level->unknown_00c9[3] = 5;
            current_level->unknown_00c9[4] = 10;
            current_level->unknown_00c9[5] = 5;
            current_level->unknown_00c9[6] = 5;
            current_level->conveyor_speed = LevelConfigFarDistance;
            current_level->conveyor_width = LevelConfigFarDistance;

            for (music_index = 0; music_index < 6; music_index++) {
                current_level->music_tracks[music_index] = -1;
            }

            in_level_config = 1;
            if (set_defaults_fn != NULL) {
                ((LEVELSETDEFAULTS_PC)set_defaults_fn)(current_level);
            }
        }
    }

    NuFParDestroy(parser);
    if (level_count == 0) {
        return NULL;
    }

    buffer_end->void_ptr = current_level;
    if (count_out != NULL) {
        *count_out = level_count;
    }
    return level_data;
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
