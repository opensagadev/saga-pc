#include "saga.h"

#include <stdio.h>

#include <string.h>

extern i32 __cdecl NuFParGetLineW(NUFPAR_PC *parser);
extern i32 __cdecl NuFParGetWordW(NUFPAR_PC *parser);
extern const char TextPlatformXbox[5];
extern const char TextPlatformPC[3];
extern const char TextPlatformPSP[4];
extern const char TextPlatformPS3[4];
extern const char TextPlatformPS2[4];
extern const char TextPlatformGameCube[3];
extern const char TextPlatform360[4];
extern const char TextPlatformWii[4];

void Text_InitLanguageList(LANGUAGEDATA *language_list) {
    if (language_list != NULL) {
        Text_LanguageList = language_list;
    }
    LANGUAGECOUNT = 0;
    while (Text_LanguageList[LANGUAGECOUNT].language != -1) {
        ++LANGUAGECOUNT;
    }
}

char *Text_GetLanguagePath(i32 language) {
    switch (language) {
        case 2:
            return Text_PathFrench;
        case 4:
            return Text_PathGerman;
        case 5:
            return Text_PathItalian;
        case 3:
            return Text_PathSpanish;
        case 8:
            return Text_PathDanish;
        case 0:
            return Text_PathJapanese;
        default:
            return Text_PathEnglish;
    }
}

void *Text_IsFontLoaded(void) {
    return Text_AppFont;
}

void Text_InitStringTable(i32 count, VARIPTR *buf, VARIPTR *buf_end) {
    char **table = (char **)buf->void_ptr;
    u32 table_size = (count + 1) * sizeof(char *);
    i32 flags_size = ((count + 31) / 32) * sizeof(u32);
    (void)buf_end;

    TTab_Original = table;
    buf->addr = (buf->addr + table_size + 3) & ~3U;
    memset(table, 0, table_size);
    table[0] = Text_ErrorString;
    TTab = table + 1;
    Text_MaxStrings_Overall = count;
    Text_StringBits = (u32 *)buf->void_ptr;
    memset(Text_StringBits, 0, flags_size);
    buf->addr += flags_size;
}

i32 Text_GetMaxOverallStrings(void) {
    return Text_MaxStrings_Overall;
}

void Text_InitTable(TEXTENTRY *entry, i32 first, i32 last) {
    i32 index = 0;
    if (entry == NULL) {
        return;
    }
    while (entry->text_id != NULL) {
        if (entry->value != -1) {
            index = entry->value;
            if (index >= first && index <= last) {
                *entry->text_id = entry->value;
            }
        } else if (index >= first && index <= last) {
            entry->value = (i16)index;
            *entry->text_id = (i16)index;
            Text_StringBits[index / 32] |= 1U << (index & 31);
        } else {
            entry->value = 0;
            *entry->text_id = 0;
        }
        ++index;
        ++entry;
    }
}

void Text_InsertCommasIntoNumber(char *number, char *text, i32 length) {
    char separator = ',';
    i32 output = 0;
    i32 digit;
    if (Text_Language == 2) {
        separator = ' ';
    } else if (Text_Language != 1 && Text_Language != 18 && Text_Language != 0) {
        separator = '.';
    }
    if (length < 0) {
        length = NuStrLen(number);
    }
    for (digit = 0; digit < length; ++digit) {
        i32 remaining = length - digit - 1;
        text[output++] = number[digit];
        if (remaining != 0 && remaining % 3 == 0) {
            text[output++] = separator;
        }
    }
    text[output] = '\0';
}

void Text_MakeScore(u32 score, char *text) {
    char digits[64];
    char *end = &digits[63];
    char *first = end;
    *end = '\0';
    do {
        *--first = (char)('0' + score % 10);
        score /= 10;
    } while (score != 0);
    Text_InsertCommasIntoNumber(first, text, (i32)(end - first));
}

i32 Text_StripComments(char *text, char *dest, i32 separators) {
    char *end = text + NuStrLen(text);
    char *out = dest;
    while (text <= end) {
        char character = *text;
        if (character == ';' || character == '#' || (character == '/' && *++text == '/')) {
            do {
                ++text;
                character = *text;
            } while (character != '\r' && character != '\n' && character != '\0');
        }
        character = *text++;
        if (separators != 0 && (character == '=' || character == ',')) {
            character = ' ';
        }
        *out++ = character;
    }
    *out = '\0';
    return (i32)(out - dest);
}

void IntroText_SetTextID(i32 id) {
    if (id > 0 && TTab[id] != NULL) {
        INTROTEXT_ID = id;
    }
}

void Text_InitDefaultStrings(void) {
    TTab[tNULL] = txt_NULL;
    TTab[tUNKNOWN] = txt_UNKNOWN;
    TTab[tENGLISH] = apitxt_ENGLISH;
    TTab[tFRENCH] = apitxt_FRENCH;
    TTab[tGERMAN] = apitxt_GERMAN;
    TTab[tITALIAN] = apitxt_ITALIAN;
    TTab[tSPANISH] = apitxt_SPANISH;
    TTab[tDANISH] = apitxt_DANISH;
}

void __cdecl Text_LoadAndFixUpStrings(char *path, u8 **buffer, char **table, i32 count) {
    NUFPAR_PC *parser = NuFParCreate(path);
    u8 *out = *buffer;

    if (parser != NULL) {
        if (parser->is_utf16 != 0) {
            while (NuFParGetLineW(parser) != 0) {
                i32 index = NuFParGetInt(parser);
                if (index > 0 && index < count && NuFParGetWordW(parser) > 0) {
                    u16 *word = (u16 *)parser->word_buffer_ptr;
                    table[index] = (char *)out;
                    while (*word != 0) {
                        out = NuUTF8CharFromUnicode(out, *word++);
                    }
                    *out++ = '\0';
                }
            }
        } else {
            while (NuFParGetLine(parser) != 0) {
                i32 index = NuFParGetInt(parser);
                char *word;
                i32 length;
                i32 i;

                if (index <= 0 || index >= count || NuFParGetWord(parser) <= 0) {
                    continue;
                }

                word = parser->word_buffer_ptr;
                if (parser->is_utf8 != 0) {
                    if (NuStrICmp(word, TextPlatform360) == 0 || NuStrICmp(word, TextPlatformGameCube) == 0 ||
                        NuStrICmp(word, TextPlatformPS2) == 0 || NuStrICmp(word, TextPlatformPS3) == 0 ||
                        NuStrICmp(word, TextPlatformPSP) == 0 || NuStrICmp(word, TextPlatformWii) == 0 ||
                        NuStrICmp(word, TextPlatformXbox) == 0) {
                        continue;
                    }
                    if (NuStrICmp(word, TextPlatformPC) == 0) {
                        NuFParGetWord(parser);
                    }
                    word = parser->word_buffer_ptr;
                    length = NuStrLen(word);
                    table[index] = (char *)out;
                    NuStrCpy((char *)out, word);
                    out += length + 1;
                } else {
                    length = NuStrLen(word);
                    table[index] = (char *)out;
                    for (i = 0; i < length; ++i) {
                        u8 character = (u8)word[i];
                        if (character < 0x80) {
                            *out++ = character;
                        } else {
                            out = NuUTF8CharFromUnicode(out, character);
                        }
                    }
                    *out++ = '\0';
                }
            }
        }

        NuFParDestroy(parser);
    }

    *buffer = out;
}

void Text_DecodeButtons(char *source, char *destination) {
    if (NuStrICmp(source, Text_WibbleCodeword) == 0) {
        NuStrCpy(destination, Text_CrossCodeword);
    }
}

void TextRegisterButtonMapFn(void (*fn)(char *, char *)) {
    if (fn != NULL) {
        buttonmapfn = fn;
    }
}

void SmartTextSetFWNMode(i32 mode) {
    smarttext_fwn = mode;
}

void SmartTextSetFont(VUFNT *font) {
    SmartTextFont = font;
}

void SetGameFont(VUFNT *font) {
    QFont2D = font;
}

void Set3DGameFont(VUFNT *font) {
    QFont3DZ = font;
}

void SmartTextSetWidescreen(f32 font_scale_x, f32 coordinate_scale) {
    QFONTSCALEX = font_scale_x;
    STCOORDSCALE = coordinate_scale;
}

void SmartTextGetWidescreen(f32 *font_scale_x, f32 *coordinate_scale) {
    if (font_scale_x != NULL) {
        *font_scale_x = QFONTSCALEX;
    }
    if (coordinate_scale != NULL) {
        *coordinate_scale = STCOORDSCALE;
    }
}

void Text_SetLanguage_Game(i32 language) {
    switch (language) {
        case 4:
            INTROTEXT_Y = 0.13f;
            INTROTEXT_SCALE = 0.575f;
            break;
        case 2:
            INTROTEXT_Y = 0.115f;
            INTROTEXT_SCALE = 0.5f;
            break;
        case 3:
            INTROTEXT_Y = 0.14f;
            INTROTEXT_SCALE = 0.61f;
            break;
        case 8:
            INTROTEXT_Y = 0.15f;
            INTROTEXT_SCALE = 0.67f;
            break;
        case 5:
            INTROTEXT_Y = 0.17f;
            INTROTEXT_SCALE = 0.76f;
            break;
        case 0:
            INTROTEXT_Y = 0.0f;
            INTROTEXT_SCALE = 1.8f;
            break;
        default:
            INTROTEXT_Y = 0.175f;
            INTROTEXT_SCALE = 0.79f;
            break;
    }
}

i32 SplitTextFindNextWS(u8 *text, i32 position) {
    u8 character = text[position];
check_character:
    if (character == ' ') {
        const u8 next = text[position + 1];
        if (next == '!' || next == '?' || next == ';' || next == ':') {
            goto next_character;
        }
        return position;
    }
    if (character == '\0') {
        return position;
    }
    if (character == '\\' && text[position + 1] == 'n') {
        return position;
    }
next_character:
    do {
        ++position;
        character = text[position];
    } while (character >= 0x80 && character <= 0xbf);
    goto check_character;
}

void FixUpButtonsInFont(VUFNT *game_font, VUFNT *button_font) {
    if (game_font == NULL) {
        game_font = QFont2D;
    }
    if (button_font == NULL) {
        button_font = QFont2DButtons;
    }
    if (game_font != NULL && button_font != NULL) {
        f32 scale = game_font->height / button_font->height;
        i32 i;
        for (i = 0; i < button_font->glyph_count; ++i) {
            i32 j;
            for (j = 0; j < game_font->glyph_count; ++j) {
                if (button_font->unicode_map[i].unicode == game_font->unicode_map[j].unicode) {
                    if (button_font->unicode_map[i].unicode >= 0x531 && button_font->unicode_map[i].unicode <= 0x53f) {
                        game_font->glyphs[game_font->unicode_map[j].index].width =
                            button_font->glyphs[button_font->unicode_map[i].index].width * scale;
                    }
                    break;
                }
            }
        }
    }
}

void Text_MakeTime(f32 time, i32 show_hours, i32 show_minutes, i32 show_centiseconds, char *text) {
    i32 hours = 0;
    i32 minutes;
    i32 seconds;
    i32 centiseconds;
    if (time < 0.0f) {
        time = 0.0f;
    }
    if (show_hours != 0) {
        hours = (i32)(time / 3600.0);
        minutes = (i32)NuFmod((f32)(time / 60.0), 60.0f);
    } else {
        minutes = (i32)(time / 60.0);
    }
    if (show_hours != 0 || show_minutes != 0) {
        seconds = (i32)NuFmod(time, 60.0f);
    } else {
        seconds = (i32)time;
    }
    centiseconds = (i32)(NuFmod(time, 1.0f) * 100.0);
    if (text == NULL) {
        return;
    }
    if (show_hours != 0) {
        if (show_centiseconds != 0) {
            sprintf(text, Text_TimeHMSC, hours, minutes, seconds, centiseconds);
        } else {
            sprintf(text, Text_TimeHMS, hours, minutes, seconds);
        }
    } else if (show_minutes != 0) {
        if (show_centiseconds != 0) {
            sprintf(text, Text_TimeMSC, minutes, seconds, centiseconds);
        } else {
            sprintf(text, Text_TimeMS, minutes, seconds);
        }
    } else if (show_centiseconds != 0) {
        sprintf(text, Text_TimeSC, seconds, centiseconds);
    } else {
        sprintf(text, Text_TimeS, seconds);
    }
}

void Text_SetLanguage(i32 language) {
    (void)language;
    Text_Language = NuLanguageGet();
    if (Text_GameSetLanguageFn != NULL) {
        Text_GameSetLanguageFn(Text_Language);
    }
}
