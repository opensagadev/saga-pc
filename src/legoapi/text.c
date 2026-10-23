#include "saga.h"

#include <stdio.h>

#include <string.h>

extern i32 __cdecl NuFParGetLineW(NUFPAR_PC *parser);
extern i32 __cdecl NuFParGetWordW(NUFPAR_PC *parser);
extern i32 __cdecl NuStrLenW(const NUWCHAR *str);
extern const char TextPlatformXbox[5];
extern const char TextPlatformPC[3];
extern const char TextPlatformPSP[4];
extern const char TextPlatformPS3[4];
extern const char TextPlatformPS2[4];
extern const char TextPlatformGameCube[3];
extern const char TextPlatform360[4];
extern const char TextPlatformWii[4];
extern const char TextLoadPermTextDirectory[12];
extern const char TextLoadPermAPIExtension[9];
extern const char TextLoadPermTRCPath[19];
extern i16 tALONGTIMEAGO;
extern i16 TextIdPc880894, TextIdPc880898;

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
    (void)buf_end;

    TTab_Original = table;
    buf->addr = ((u32)table + table_size + 3) & ~3U;
    memset(table, 0, table_size);
    table[0] = Text_ErrorString;
    TTab = table + 1;
    Text_StringBits = (u32 *)buf->void_ptr;
    Text_MaxStrings_Overall = count;
    memset(Text_StringBits, 0, ((count + 31) / 32) * sizeof(u32));
    buf->addr += ((count + 31) / 32) * sizeof(u32);
}

i32 Text_GetMaxOverallStrings(void) {
    return Text_MaxStrings_Overall;
}

void Text_InitTable(TEXTENTRY *entry, i32 first, i32 last) {
    i32 index = 0;
    u32 *bits;
    if (entry == NULL) {
        return;
    }
    bits = Text_StringBits;
    do {
        if (entry->text_id != NULL && entry->value != -1) {
            index = entry->value;
            if (index >= first && index <= last) {
                entry->value = (i16)index;
                *entry->text_id = (i16)index;
            }
            ++index;
            ++entry;
        }
        while (entry->text_id != NULL && entry->value == -1) {
            if (index >= first && index <= last) {
                entry->value = (i16)index;
                *entry->text_id = (i16)index;
                bits[index / 32] |= 1U << (index & 31);
            } else {
                entry->value = 0;
                *entry->text_id = 0;
            }
            ++entry;
            ++index;
        }
    } while (entry->text_id != NULL);
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

static i32 Text_IsOtherPlatformWord(NUFPAR_PC *parser) {
    if (NuStrICmp(parser->word_buffer_ptr, TextPlatform360) == 0)
        return 1;
    if (NuStrICmp(parser->word_buffer_ptr, TextPlatformGameCube) == 0)
        return 1;
    if (NuStrICmp(parser->word_buffer_ptr, TextPlatformPS2) == 0)
        return 1;
    if (NuStrICmp(parser->word_buffer_ptr, TextPlatformPS3) == 0)
        return 1;
    if (NuStrICmp(parser->word_buffer_ptr, TextPlatformPSP) == 0)
        return 1;
    if (NuStrICmp(parser->word_buffer_ptr, TextPlatformPC) == 0) {
        NuFParGetWord(parser);
        return 0;
    }
    if (NuStrICmp(parser->word_buffer_ptr, TextPlatformWii) == 0)
        return 1;
    if (NuStrICmp(parser->word_buffer_ptr, TextPlatformXbox) == 0)
        return 1;
    return 0;
}

void __cdecl Text_LoadAndFixUpStrings(char *path, u8 **buffer, char **table, i32 count) {
    u8 *out = *buffer;
    NUFPAR_PC *parser = NuFParCreate(path);
    i32 index;
    i32 length;

    if (parser != NULL) {
        if (parser->is_utf16 != 0) {
            while (NuFParGetLineW(parser) != 0) {
                index = NuFParGetInt(parser);
                if (index > 0 && index < count) {
                    u16 *word;
                    NuFParGetWordW(parser);
                    word = (u16 *)parser->word_buffer_ptr;
                    length = NuStrLenW(word);
                    if (length > 0) {
                        table[index] = (char *)out;
                        do {
                            out = NuUTF8CharFromUnicode(out, *word++);
                        } while (--length > 0);
                        *out++ = '\0';
                    }
                }
            }
        } else if (parser->is_utf8 != 0) {
            while (NuFParGetLine(parser) != 0) {
                NuFParGetWord(parser);
                if (parser->word_buffer_ptr[0] != '\0') {
                    index = NuAToI(parser->word_buffer_ptr);
                    if (index > 0 && index < count) {
                        NuFParGetWord(parser);
                        if (Text_IsOtherPlatformWord(parser) == 0) {
                            length = NuStrLen(parser->word_buffer_ptr);
                            table[index] = (char *)out;
                            NuStrCpy((char *)out, parser->word_buffer_ptr);
                            out += length + 1;
                        }
                    }
                }
            }
        } else {
            while (NuFParGetLine(parser) != 0) {
                index = NuFParGetInt(parser);
                if (index > 0 && index < count) {
                    NuFParGetWord(parser);
                    if (Text_IsOtherPlatformWord(parser) == 0) {
                        u8 *word = (u8 *)parser->word_buffer_ptr;
                        length = NuStrLen((char *)word);
                        if (length > 0) {
                            table[index] = (char *)out;
                            do {
                                if (*word < 0x80)
                                    *out++ = *word;
                                else
                                    out = NuUTF8CharFromUnicode(out, *word);
                                ++word;
                            } while (--length > 0);
                            *out++ = '\0';
                        }
                    }
                }
            }
        }
        NuFParDestroy(parser);
    }
    *buffer = out;
}

void __cdecl Text_LoadStrings(VARIPTR *buffer, VARIPTR *buffer_end) {
    char language[32];
    char path[256];
    u8 *string_buffer = (u8 *)buffer->void_ptr;

    (void)buffer_end;

    TextRegisterButtonMapFn(Text_DecodeButtons);
    NuStrCpy(language, Text_GetLanguagePath(Text_Language));

    NuStrCpy(path, TextLoadPermTextDirectory);
    NuStrCat(path, language);
    NuStrCat(path, AreaResidentsExtension);
    Text_LoadAndFixUpStrings(path, &string_buffer, TTab, 0x70d);

    if (StartupGameFlag1 != 0) {
        IntroText_SetTextID(tALONGTIMEAGO);
    } else {
        NuStrCpy(path, TextLoadPermTextDirectory);
        NuStrCat(path, language);
        NuStrCat(path, TextLoadPermAPIExtension);
        Text_LoadAndFixUpStrings(path, &string_buffer, TTab, 0x70d);
    }

    buffer->addr = ((u32)string_buffer + 3U) & ~3U;

    if (TTab[TextIdPc880894] != NULL)
        NuStrCpy(Game.customiser_start[0].name, TTab[TextIdPc880894]);
    if (TTab[TextIdPc880898] != NULL)
        NuStrCpy(Game.customiser_start[1].name, TTab[TextIdPc880898]);
    FinishWeirdoNames(-1);
    Customiser_InitNames(CharacterCustomiser);
    MenuLoadTechnicalStrings((char *)TextLoadPermTRCPath, language, buffer, *buffer_end);
}

void PartKill_ThermalDetonator(PART_PC *part, i32 unused1) {
    u16 flags;
    struct EXPLOSION *explosion;
    if (part == NULL) {
        return;
    }

    if (part->owner != NULL) {
        AlertSurroundingCreatures(part->owner, &part->position);
    }

    flags = 0x200;
    if (part->owner != NULL && (part->owner->flags_1fc & 0x80) != 0 && Cheat_IsOn(0x17) != 0) {
        flags = 0x1200;
    }

    explosion = Detonate(&part->position, flags);
    if (explosion != NULL && part->owner != NULL && ((u8)(part->owner->field_280)) <= 1) {
        explosion->field_0x33 = ((u8)(part->owner->field_280));
    }
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

void MatrixTextStringEncode(void *font, unsigned char *source, u16 *destination) {
    u16 character;
    while (*source != '\0') {
        source = NuUnicodeCharFromUTF8(&character, source);
        if (character == '~') {
            if (*source == '\0') {
                break;
            }
            source = NuUnicodeCharFromUTF8(&character, source);
            continue;
        }

        *destination = NuQFntEncodeUnicodeChar(font, character);
        if (*destination == 0xffff) {
            NuUnicodeCharFromUTF8(&character, (unsigned char *)((char *)("\xe2\x96\xa1")));
            *destination = NuQFntEncodeUnicodeChar(font, character);
            if (*destination == 0xffff) {
                *destination = NuQFntEncodeUnicodeChar(font, '?');
            }
        }
        ++destination;
    }
    *destination = 0;
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

void Text_ExpandAllButtonStrings(char *input, char *output) {
    char token[256];
    char expanded[64];

    *output = '\0';
    while (*input != '\0') {
        i32 token_length;
        char literal;
        if (*input != '[') {
            *output = *input;
            output[1] = '\0';
            ++input;
            ++output;
            continue;
        }

        token[0] = '[';
        token_length = 1;
        while (input[token_length] != ']' && input[token_length] != '\0') {
            token[token_length] = input[token_length];
            ++token_length;
        }

        literal = '[';
        if (input[token_length] != '\0') {
            token[token_length] = ']';
            token[token_length + 1] = '\0';
            token_length = NuStrLen(token);
            if (token_length > 0 && Text_ExpandButtonString(token, expanded) != 0) {
                char *character = expanded;
                while (*character != '\0') {
                    *output = *character;
                    output[1] = '\0';
                    ++character;
                    ++output;
                }
                NuStrCat(output, expanded);
                input += token_length;
                continue;
            }
            literal = *input;
        }

        *output = literal;
        output[1] = '\0';
        ++input;
        ++output;
    }
    *output = '\0';
}

void Text_SetLanguage(i32 language) {
    (void)language;
    Text_Language = NuLanguageGet();
    if (Text_GameSetLanguageFn != NULL) {
        Text_GameSetLanguageFn(Text_Language);
    }
}

i32 __cdecl GetMenuID(void) {
    i16 menu = GameMenu[GameMenuLevel].menu;
    return menu == -1 ? -1 : MenuInfo[menu].id;
}

i32 GetParentMenuID(void) {
    i16 menu;
    if (GameMenuLevel <= 1) {
        return -1;
    }
    menu = GameMenu[GameMenuLevel - 1].menu;
    if (menu == -1) {
        return -1;
    }
    return MenuInfo[menu].id;
}

void __cdecl Text3DStringEncodeFont(u8 *source, u16 *destination, VUFNT *font) {
    u16 character;
    while (*source != 0) {
        source = NuUnicodeCharFromUTF8(&character, source);
        if (character == '~') {
            if (*source == 0) {
                break;
            }
            source = NuUnicodeCharFromUTF8(&character, source);
            continue;
        }
        *destination = NuQFntEncodeUnicodeChar(font, character);
        if (*destination == 0xffff) {
            NuUnicodeCharFromUTF8(&character, (u8 *)"\xe2\x96\xa1");
            *destination = NuQFntEncodeUnicodeChar(font, character);
            if (*destination == 0xffff) {
                *destination = NuQFntEncodeUnicodeChar(font, '?');
            }
        }
        ++destination;
    }
    *destination = 0;
}

__declspec(noinline) void __cdecl Text3DStringEncode(char *source, u16 *destination) {
    Text3DStringEncodeFont((u8 *)source, destination, SmartTextFont != NULL ? SmartTextFont : QFont2D);
}

void __cdecl TextDecodeCodeword(char *source, char *destination) {
    i32 button = -1;
    i32 device;
    if (buttonmapfn != NULL) {
        buttonmapfn(source, source);
    }
    if (NuStrICmp(source, "accept") == 0) {
        NuStrCpy(source, "cross");
    }
    if (NuStrICmp(source, "back") == 0) {
        NuStrCpy(source, "circle");
    }
    if (NuStrICmp(source, "wii") == 0) {
        NuStrCpy(source, "logo");
    }
    if (NuStrICmp(source, "playstation") == 0) {
        NuStrCpy(source, "logo");
    }
    NuStrCpy(destination, source);
    if (NuStrICmp(source, "padup") == 0) {
        button = 8;
    } else if (NuStrICmp(source, "padright") == 0) {
        button = 9;
    } else if (NuStrICmp(source, "paddown") == 0) {
        button = 10;
    } else if (NuStrICmp(source, "padleft") == 0) {
        button = 11;
    } else if (NuStrICmp(source, "triangle") == 0) {
        button = 12;
    } else if (NuStrICmp(source, "circle") == 0) {
        button = 13;
    } else if (NuStrICmp(source, "cross") == 0) {
        button = 14;
    } else if (NuStrICmp(source, "square") == 0) {
        button = 15;
    } else if (NuStrICmp(source, "l1") == 0) {
        button = 16;
    } else if (NuStrICmp(source, "l2") == 0) {
        button = 17;
    } else if (NuStrICmp(source, "r1") == 0) {
        button = 18;
    } else if (NuStrICmp(source, "r2") == 0) {
        button = 19;
    } else if (NuStrICmp(source, "l3") == 0) {
        button = 20;
    } else if (NuStrICmp(source, "r3") == 0) {
        button = 21;
    } else if (NuStrICmp(source, "select") == 0) {
        button = 22;
    } else if (NuStrICmp(source, "start") == 0) {
        button = 23;
    }
    device = TextInputLastDevicePc();
    if (device < 0) {
        device = 0;
    }
    TextInputButtonNamePc(device, button, destination);
}

void __cdecl TextDecode(char *text, u8 *destination) {
    u8 *source = (u8 *)text;
    i32 input = 0;
    i32 output = 0;
    char codeword[512];
    char decoded[512];
    if (source[0] != 0) {
        do {
            if (source[input] == '[' && source[input + 1] == '[') {
                i32 length = 0;
                i32 index;
                input += 2;
                while (source[input] != 0 && source[input] != ']') {
                    codeword[length++] = source[input++];
                }
                while (source[input] == ']') {
                    ++input;
                }
                codeword[length] = 0;
                TextDecodeCodeword(codeword, decoded);
                for (index = 0; decoded[index] != 0; ++index) {
                    destination[output++] = decoded[index];
                }
            } else if (source[input] == 0xe3 && source[input + 1] == 0x80 && source[input + 2] == 0x80) {
                destination[output++] = ' ';
                input += 3;
            } else if (source[input] == 0xe3 && source[input + 1] == 0x80 && source[input + 2] == 0x81) {
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = ' ';
            } else if (source[input] == 0xe3 && source[input + 1] == 0x80 && source[input + 2] == 0x82) {
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = ' ';
            } else if (source[input] == 0xe3 && source[input + 1] == 0x80 && source[input + 2] == 0x85) {
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = ' ';
            } else if (source[input] == 0xef && source[input + 1] == 0xbc && source[input + 2] == 0x81) {
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = ' ';
            } else if (source[input] == 0xef && source[input + 1] == 0xbc && source[input + 2] == 0x9f) {
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = ' ';
            } else {
                destination[output++] = source[input++];
                while (source[input] >= 0x80 && source[input] <= 0xbf) {
                    destination[output++] = source[input++];
                }
            }
        } while (output < 480 && source[input] != 0);
    }
    destination[output] = 0;
}

f32 __cdecl TextPrintSubstring(u8 *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, i32 colour, i32 button_font) {
    VUFNT *base_font = SmartTextFont != NULL ? SmartTextFont : QFont2D;
    VUFNT *font;
    f32 relative_scale;
    u16 encoded[512];
    f32 width;
    if (button_font != 0 && QFont2DButtons != NULL) {
        font = QFont2DButtons;
        relative_scale = base_font->height / font->height;
    } else {
        font = base_font;
        relative_scale = 1.0f;
    }
    NuQFntSet(font);
    NuQFntSetScale(font, x_scale * QFONTSCALEX * relative_scale, y_scale * QFONTSCALEY * relative_scale);
    Text3DStringEncodeFont(text, encoded, font);
    width = NuQFntPrintLenW(font, encoded);
    NuQFntSetColour(font, colour);
    NuQFntMove(font, x, y, z);
    NuQFntPrintW(font, encoded);
    return width;
}
#define TEXT3D_FONT() (SmartTextFont != NULL ? SmartTextFont : QFont2D)
void __cdecl Text3DEx(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment,
                      u32 red_slot, u32 green_slot, u32 blue_slot, i32 alpha) {
    u8 red = (u8)red_slot;
    u8 green = (u8)green_slot;
    u8 blue = (u8)blue_slot;
    u8 alpha_byte = (u8)alpha;
    f32 width, height, baseline_y, button_scale, button_width, normal_width, ratio, scaled_width;
    f64 half_height;
    u8 decoded[512], normal[512], buttons[512], fragment[512];
    u16 encoded[512], button_encoded[512], normal_encoded[512];
    i32 button_count, normal_count, count, button_font, preset, i;
    u32 colour;

    text3d_width = text3d_height = 0.0f;
    if (TEXT3D_FONT() == NULL || text == NULL || text[0] == '\0' || MenuStopDraw != 0)
        return;
    x_scale = TextScaleX_candidate * x_scale;
    y_scale = TextScaleY_candidate * y_scale;
    if (x < -2.0f || x > 2.0f || y < -2.0f || y > 2.0f)
        return;
    if (TextPrintModePc != 0)
        NuQFntPushPrintMode(3);
    else
        NuQFntPushPrintMode(2);

    TextDecode(text, decoded);
    if (followon_line == 0)
        NuQFntSet(TEXT3D_FONT());

    Text3DStringEncode((char *)decoded, encoded);
    NuQFntSetScale(TEXT3D_FONT(), QFONTSCALEX * x_scale, QFONTSCALEY * y_scale);
    text3d_width = NuQFntPrintLenW(TEXT3D_FONT(), encoded);
    width = text3d_width;
    text3d_height = NuQFntHeight(TEXT3D_FONT());
    height = text3d_height;
    half_height = (f64)height * 0.5;
    baseline_y = NuQFntBaseline(TEXT3D_FONT()) + y;
    y = (f32)((f64)baseline_y - half_height);
    if (y_scale != x_scale) {
        button_width = 0.0f;
        normal_width = 0.0f;
        switch (smarttext_fwn) {
            case 1:
                button_scale = x_scale;
                break;
            case 2:
                button_scale = y_scale;
                break;
            default:
                button_scale = (f32)((f64)(y_scale + x_scale) * 0.5);
                break;
        }

        button_count = 0;
        normal_count = 0;
        for (i = 0; decoded[i] != 0;) {
            if (decoded[i] == 0xd4 && decoded[i + 1] >= 0xb1 && decoded[i + 1] <= 0xbf) {
                buttons[button_count++] = decoded[i++];
                while (decoded[i] >= 0x80 && decoded[i] <= 0xbf)
                    buttons[button_count++] = decoded[i++];
            } else {
                normal[normal_count++] = decoded[i++];
                while (decoded[i] >= 0x80 && decoded[i] <= 0xbf)
                    normal[normal_count++] = decoded[i++];
            }
        }
        buttons[button_count] = 0;
        normal[normal_count] = 0;

        if (button_count != 0) {
            Text3DStringEncode((char *)buttons, button_encoded);
            NuQFntSetScale(TEXT3D_FONT(), QFONTSCALEX * button_scale, QFONTSCALEY * button_scale);
            button_width = NuQFntPrintLenW(TEXT3D_FONT(), button_encoded);
        }
        if (normal_count != 0) {
            Text3DStringEncode((char *)normal, normal_encoded);
            NuQFntSetScale(TEXT3D_FONT(), QFONTSCALEX * x_scale, QFONTSCALEY * y_scale);
            normal_width = NuQFntPrintLenW(TEXT3D_FONT(), normal_encoded);
        }
        if (normal_width != 0.0f) {
            ratio = (width - button_width) / normal_width;
            x_scale = ratio * x_scale;
        } else {
            button_scale = x_scale;
            if (smarttext_fwn == 2) {
                button_scale = y_scale;
                scaled_width = y_scale * width;
                width = scaled_width / x_scale;
            }
        }
    }
    if ((alignment & 4) != 0)
        y = (f32)((f64)y - half_height);
    else if ((alignment & 1) != 0)
        y = (f32)((f64)y + half_height);
    if ((alignment & 2) == 0) {
        if ((alignment & 8) != 0)
            x -= width;
        else
            x -= width * 0.5;
    }
    if (followon_line == 0) {
        if (apitext_half_rgb != 0)
            current_rgba = ((u32)alpha_byte << 24) | ((u32)(blue >> 1) << 16) | ((u32)(green >> 1) << 8) | (red >> 1);
        else
            current_rgba = ((u32)alpha_byte << 24) | ((u32)blue << 16) | ((u32)green << 8) | red;
    }

    count = 0;
    button_font = 0;
    for (i = 0; count <= 508 && decoded[i] != 0;) {
        if (decoded[i] == '~') {
            ++i;
            if (decoded[i] == '~') {
                if (apitext_half_rgb != 0)
                    colour = ((u32)alpha_byte << 24) | ((u32)(blue >> 1) << 16) | ((u32)(green >> 1) << 8) | (red >> 1);
                else
                    colour = ((u32)alpha_byte << 24) | ((u32)blue << 16) | ((u32)green << 8) | red;
            } else {
                preset = decoded[i] - '0';
                if (red == 0 && green == 0 && blue == 0)
                    colour = (u32)alpha_byte << 24;
                else if (apitext_half_rgb != 0)
                    colour = ((u32)alpha_byte << 24) | ((u32)text_preset_b[preset] << 16) |
                             ((u32)text_preset_g[preset] << 8) | text_preset_r[preset];
                else
                    colour = (u8)(2 * text_preset_r[preset]) | ((u32)((2 * text_preset_b[preset]) & 255) << 16) |
                             ((u32)((2 * text_preset_g[preset]) & 255) << 8) | ((u32)alpha_byte << 24);
            }
            ++i;
            if (count > 0 && colour != current_rgba) {
                fragment[count] = 0;
                if (button_font != 0)
                    x += TextPrintSubstring(fragment, x, y, z, button_scale, button_scale, current_rgba, button_font);
                else
                    x += TextPrintSubstring(fragment, x, y, z, x_scale, y_scale, current_rgba, button_font);
                count = 0;
            }
            current_rgba = colour;
        } else if (decoded[i] == 0xd4 && decoded[i + 1] >= 0xb1 && decoded[i + 1] <= 0xbf) {
            if (count > 0 && button_font == 0) {
                fragment[count] = 0;
                x += TextPrintSubstring(fragment, x, y, z, x_scale, y_scale, current_rgba, button_font);
                count = 0;
            }
            fragment[count++] = decoded[i++];
            button_font = 1;
            while (decoded[i] >= 0x80 && decoded[i] <= 0xbf)
                fragment[count++] = decoded[i++];
        } else {
            if (count > 0 && button_font != 0) {
                fragment[count] = 0;
                x += TextPrintSubstring(fragment, x, y, z, button_scale, button_scale, current_rgba, button_font);
                count = 0;
            }
            fragment[count++] = decoded[i++];
            button_font = 0;
            while (decoded[i] >= 0x80 && decoded[i] <= 0xbf)
                fragment[count++] = decoded[i++];
        }
    }
    if (count > 0) {
        fragment[count] = 0;
        if (button_font != 0)
            TextPrintSubstring(fragment, x, y, z, button_scale, button_scale, current_rgba, button_font);
        else
            TextPrintSubstring(fragment, x, y, z, x_scale, y_scale, current_rgba, button_font);
    }
    NuQFntPopPrintMode();
}

__declspec(noinline) void __cdecl SetQFont2D(void) {
    NuQFntSetCoordinateSystem(3);
    NuQFntSet(QFont2D);
    NuQFntSetSpaceWidth(QFont2D, 12.0f);
}
void __cdecl IntroText_Draw(f32 alpha) {
    u16 encoded[128];
    u32 colour;
    if (INTROTEXT_ID == -1)
        return;
    SetQFont2D();
    NuQFntSetSpaceWidth(QFont2D, 8.0f);
    Text3DStringEncode(TTab[INTROTEXT_ID], encoded);
    NuQFntSetJustifiedTolerances(1.0f, 1.0f);
    colour = ((u32)((f64)alpha * 128.0) << 24) | 0x7f5f00;
    NuQFntPrintJustifiedW(QFont2D, encoded, -0.85f, INTROTEXT_Y, 1.0f, INTROTEXT_SCALE, INTROTEXT_SCALE, 1.7f, 1.0f,
                          colour, NULL);
}

extern VUFNT *QFont2DLower;
void __cdecl Text3DEx2(char *text, f32 x, f32 y, f32 z, f32 sx, f32 sy, f32 sz, u32 alignment, u32 red, u32 green,
                       u32 blue, i32 alpha) {
    if (QFont2DLower != NULL) {
        VUFNT *saved_font = SmartTextFont;
        SmartTextFont = QFont2DLower;
        Text3DEx(text, x, y, z, sx, sy, sz, alignment, red, green, blue, alpha);
        SmartTextFont = saved_font;
    }
}
void __cdecl MenuText3DEx(char *text, f32 x, f32 y, f32 z, f32 sx, f32 sy, f32 sz, u32 alignment, u32 red, u32 green,
                          u32 blue, i32 alpha) {
    if (MenuDrawDropShadows != 0) {
        f32 shadow_x = (f32)((f64)x + (f64)sx * (f64)0.015f);
        f32 shadow_y = (f32)((f64)y - (f64)sy * (f64)0.015f);
        Text3DEx2(text, shadow_x, shadow_y, z, sx, sy, sz, alignment, 0, 0, 0, (u8)alpha >> 2);
    }
    Text3DEx(text, x, y, z, sx, sy, sz, alignment, red, green, blue, alpha);
}

extern f32 STCOORDSCALE, SmartTextDefaultWidthPc, SmartTextLongestWidthPc;
i32 __cdecl FindNextBreak(u8 *, i32);
i32 __cdecl FindNearestBreak(u8 *, i32);
i32 __cdecl NuStrFindPosU(u8 *, i32);
void SmartTextEx(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment, u32 red,
                 u32 green, u32 blue, f32 max_width, i32 max_lines, void *message_box, i32 suppress_draw, i32 alpha) {
    typedef struct box_geometry_pc_s {
        f32 reserved, x, y, z, width, height;
    } BOX_GEOMETRY_PC;
    STATIC_ASSERT(offsetof(BOX_GEOMETRY_PC, x) == 4);
    STATIC_ASSERT(offsetof(BOX_GEOMETRY_PC, y) == 8);
    STATIC_ASSERT(offsetof(BOX_GEOMETRY_PC, width) == 0x10);
    STATIC_ASSERT(offsetof(BOX_GEOMETRY_PC, height) == 0x14);
    BOX_GEOMETRY_PC *box = (BOX_GEOMETRY_PC *)message_box;
    i32 widest_line = -1;
    f32 saved_x_scale;
    f32 saved_y_scale;
    f32 draw_x_scale;
    f32 width;
    f32 longest;
    f32 line_height;
    f32 line_y;
    i32 explicit_breaks;
    i32 last_line;
    i32 constrained;
    i32 characters;
    i32 bytes;
    i32 begin;
    i32 line;
    i32 end;
    i32 length;
    i32 next;
    i32 pos;
    unsigned char decoded[512];
    u16 encoded[512];
    unsigned char candidate[512];
    unsigned char lines[8][512];
    if ((SmartTextFont == NULL && QFont2D == NULL) || text == NULL || text[0] == '\0' || MenuStopDraw != 0)
        return;

    x_scale = TextScaleX_candidate * x_scale;
    y_scale = TextScaleY_candidate * y_scale;
    saved_x_scale = TextScaleX_candidate;
    saved_y_scale = TextScaleY_candidate;

    TextScaleX_candidate = 1.0f;
    TextScaleY_candidate = 1.0f;
    x = STCOORDSCALE * x;
    max_width = STCOORDSCALE * max_width;
    TextDecode(text, decoded);
    Text3DStringEncodeFont(decoded, encoded, TEXT3D_FONT());
    NuQFntSet(TEXT3D_FONT());
    NuQFntSetScale(TEXT3D_FONT(), QFONTSCALEX * x_scale, QFONTSCALEY * y_scale);
    width = NuQFntPrintLenW(TEXT3D_FONT(), encoded);
    line_height = NuQFntHeight(TEXT3D_FONT());
    NuQFntBaseline(TEXT3D_FONT());

    if (max_width <= 0.0f)
        max_width = SmartTextDefaultWidthPc;

    explicit_breaks = 0;
    for (pos = 0; decoded[pos] != '\0';) {
        if (decoded[pos] == '\\') {
            ++pos;
            if (decoded[pos] == 'n')
                ++explicit_breaks;
        }
        ++pos;
        while (decoded[pos] >= 0x80 && decoded[pos] <= 0xbf)
            ++pos;
    }
    last_line = (i32)(width / max_width);
    if (width / (last_line + 1) > max_width * (f64)0.9f)
        ++last_line;
    constrained = 0;
    if (max_lines > 8)
        max_lines = 8;
    if (max_lines > 0 && last_line + 1 > max_lines) {
        last_line = max_lines - 1;
        constrained = 1;
    }
    characters = NuStrLenU(decoded);
    bytes = NuStrLen((char *)(decoded));
    longest = 0.0f;
    if (explicit_breaks > 0) {
        for (line = 0; line <= explicit_breaks; ++line) {
            begin = line == 0 ? 0 : end + 3;
            while (decoded[begin] == ' ')
                ++begin;
            for (end = begin; end < bytes; ++end) {
                if (decoded[end] == '\\' && decoded[end + 1] == 'n')
                    break;
            }
            --end;
            length = end - begin + 1;
            if (length > 511)
                length = 511;
            PCByteCopy_candidate(lines[line], decoded + begin, length);
            lines[line][length] = '\0';
            Text3DStringEncodeFont(lines[line], encoded, TEXT3D_FONT());
            width = NuQFntPrintLenW(TEXT3D_FONT(), encoded);
            if (width > longest) {
                longest = width;
                widest_line = line;
            }
        }
        last_line = explicit_breaks;
    } else if (last_line > 0) {
        if ((alignment & 10) != 0 && constrained == 0) {
            end = 0;
            for (line = 0; line <= last_line; ++line) {
                begin = line == 0 ? 0 : end + 1;
                while (decoded[begin] == ' ')
                    ++begin;
                if (line == last_line) {
                    end = bytes - 1;
                } else if (max_width > 0.0) {
                    next = begin;
                    do {
                        end = next;
                        ++next;
                        while (decoded[next] == ' ')
                            ++next;
                        if (next == bytes) {
                            end = next - 1;
                            break;
                        }
                        next = FindNextBreak(decoded, next);
                        if (decoded[next] != '-' && decoded[next] != '.' && decoded[next] != ',')
                            --next;
                        while (decoded[next] == ' ')
                            --next;
                        length = next - begin + 1;
                        if (length > 511)
                            length = 511;
                        PCByteCopy_candidate(candidate, decoded + begin, length);
                        candidate[length] = '\0';
                        Text3DStringEncodeFont(candidate, encoded, TEXT3D_FONT());
                    } while (NuQFntPrintLenW(TEXT3D_FONT(), encoded) < max_width);
                }
                length = end - begin + 1;
                if (length > 511)
                    length = 511;
                PCByteCopy_candidate(lines[line], decoded + begin, length);
                lines[line][length] = '\0';
                Text3DStringEncode((char *)(lines[line]), encoded);
                width = NuQFntPrintLenW(TEXT3D_FONT(), encoded);
                if (width > longest) {
                    longest = width;
                    widest_line = line;
                }
            }
        } else {
            for (line = 0; line <= last_line; ++line) {
                if (line == 0)
                    begin = 0;
                else
                    begin = FindNearestBreak(decoded, NuStrFindPosU(decoded, characters / (last_line + 1) * line));
                if (line == last_line)
                    end = bytes;
                else
                    end = FindNearestBreak(decoded, NuStrFindPosU(decoded, characters / (last_line + 1) * (line + 1)));
                if (decoded[end] != '-' && decoded[end] != '.' && decoded[end] != ',')
                    --end;
                while (decoded[end] == ' ')
                    --end;
                while (decoded[begin] == ' ' || decoded[begin] == '-' || decoded[begin] == '.' || decoded[begin] == ',')
                    ++begin;
                length = end - begin + 1;
                if (length > 511)
                    length = 511;
                PCByteCopy_candidate(lines[line], decoded + begin, length);
                lines[line][length] = '\0';
                Text3DStringEncode((char *)(lines[line]), encoded);
                width = NuQFntPrintLenW(TEXT3D_FONT(), encoded);
                if (width > longest) {
                    longest = width;
                    widest_line = line;
                }
            }
        }
    } else {
        longest = width;
    }
    if (lines[last_line][0] == '\0')
        --last_line;
    SmartTextLongestWidthPc = longest;
    if (max_width < longest) {
        draw_x_scale = max_width / longest * x_scale;
        SmartTextLongestWidthPc = max_width;
    } else {
        draw_x_scale = x_scale;
    }
    if (box != NULL) {
        NuQFntSetScale(TEXT3D_FONT(), draw_x_scale, y_scale);
        if (widest_line > -1) {
            Text3DStringEncode((char *)(lines[widest_line]), encoded);
            width = NuQFntPrintLenW(TEXT3D_FONT(), encoded);
        } else {
            width = longest;
        }
        box->x = x;
        box->y = y;
        box->width = width;
        box->height = (f32)((last_line + 1) * line_height * -1.0);
    }
    followon_line = 0;
    if (suppress_draw == 0) {
        if (last_line > 0) {
            for (line = 0; line <= last_line; ++line) {
                if ((alignment & 1) != 0)
                    line_y = y + line_height * line;
                else if ((alignment & 4) != 0)
                    line_y = y - last_line * line_height + line_height * line;
                else
                    line_y = y - last_line * 0.5f * line_height + line_height * line;
                if (line != 0)
                    followon_line = 1;
                Text3DEx((char *)(lines[line]), x, line_y, z, draw_x_scale, y_scale, z_scale, alignment, red, green,
                         blue, alpha);
            }
        } else {
            Text3DEx((char *)(decoded), x, y, z, draw_x_scale, y_scale, z_scale, alignment, red, green, blue, alpha);
        }
    }
    TextScaleX_candidate = saved_x_scale;
    followon_line = 0;
    TextScaleY_candidate = saved_y_scale;
}
void __cdecl SmartTextEx(char *, f32, f32, f32, f32, f32, f32, u32, u32, u32, u32, f32, i32, void *, i32, i32);
void __cdecl SmartTextEx2(char *text, f32 x, f32 y, f32 z, f32 sx, f32 sy, f32 sz, u32 alignment, u32 red, u32 green,
                          u32 blue, f32 max_width, i32 max_lines, void *box, i32 suppress, u32 alpha) {
    VUFNT *saved_font = SmartTextFont;
    if (QFont2DLower != NULL)
        SmartTextFont = QFont2DLower;
    SmartTextEx(text, x, y, z, sx, sy, sz, alignment, red, green, blue, max_width, max_lines, box, suppress, alpha);
    SmartTextFont = saved_font;
}

void SmartTextExDrop(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment, u32 red,
                     u32 green, u32 blue, f32 max_width, i32 max_lines, void *message_box, i32 suppress_draw,
                     u32 alpha) {
    SmartTextEx2(text, x - x_scale * 0.007f, y - y_scale * 0.01f, z, x_scale, y_scale, z_scale, alignment, 0, 0, 0,
                 max_width, max_lines, message_box, suppress_draw, alpha);
    SmartTextEx(text, x, y, z, x_scale, y_scale, z_scale, alignment, red, green, blue, max_width, max_lines,
                message_box, suppress_draw, alpha);
}
void __cdecl MenuSmartTextEx(char *text, f32 x, f32 y, f32 z, f32 sx, f32 sy, f32 sz, u32 alignment, u32 red, u32 green,
                             u32 blue, f32 max_width, i32 max_lines, void *box, i32 suppress, i32 alpha) {
    if (MenuDrawDropShadows != 0) {
        f32 shadow_x = (f32)((f64)x + (f64)sx * (f64)0.015f);
        f32 shadow_y = (f32)((f64)y - (f64)sy * (f64)0.015f);
        SmartTextEx2(text, shadow_x, shadow_y, z, sx, sy, sz, alignment, 0, 0, 0, max_width, max_lines, box, suppress,
                     alpha >> 2);
    }
    SmartTextEx(text, x, y, z, sx, sy, sz, alignment, red, green, blue, max_width, max_lines, box, suppress, alpha);
}

typedef struct text_crawl_view_pc_s {
    i16 *heading_text;
    i16 *vehicle_bonus_text;
    i16 *character_bonus_text;
    u8 paragraph_count;
    i8 area;
    u8 unknown_0e[2];
} TEXT_CRAWL_VIEW_PC;
typedef struct text_crawl_mission_view_pc_s {
    u8 unknown_00[4];
    i16 name_id;
    i16 text_id;
} TEXT_CRAWL_MISSION_VIEW_PC;
STATIC_ASSERT(sizeof(TEXT_CRAWL_VIEW_PC) == 0x10);
STATIC_ASSERT(offsetof(TEXT_CRAWL_VIEW_PC, heading_text) == 0);
STATIC_ASSERT(offsetof(TEXT_CRAWL_VIEW_PC, vehicle_bonus_text) == 4);
STATIC_ASSERT(offsetof(TEXT_CRAWL_VIEW_PC, character_bonus_text) == 8);
STATIC_ASSERT(offsetof(TEXT_CRAWL_VIEW_PC, paragraph_count) == 0xc);
STATIC_ASSERT(offsetof(TEXT_CRAWL_VIEW_PC, area) == 0xd);
STATIC_ASSERT(offsetof(TEXT_CRAWL_MISSION_VIEW_PC, name_id) == 4);
STATIC_ASSERT(offsetof(TEXT_CRAWL_MISSION_VIEW_PC, text_id) == 6);
STATIC_ASSERT(offsetof(AREA_DATA_PC, name_id) == 0x78);
STATIC_ASSERT(offsetof(AREA_DATA_PC, episode_index) == 0x86);
STATIC_ASSERT(offsetof(AREA_DATA_PC, area_index) == 0x87);
STATIC_ASSERT(offsetof(AREA_DATA_PC, text_id_value) == 0x96);
STATIC_ASSERT(offsetof(EPISODEDATA_PC, name_id) == 0);
STATIC_ASSERT(offsetof(EPISODEDATA_PC, text_id) == 2);
static TEXT_CRAWL_VIEW_PC *TextCrawlCurrentPc = 0;
static NUMTX TextCrawlMatrixPc = {0};
static NUVEC TextCrawlOffsetPc = {0.0f, -1.0f, 10.0f};
extern u16 TextCrawlRotateXPc, TextCrawlRotateYPc, TextCrawlRotateZPc;
extern f32 TextCrawlTimePc, TextCrawlScaleXPc, TextCrawlScaleYPc, TextCrawlLineWidthPc, TextCrawlLineSpacingPc;
extern u32 TextCrawlRedPc, TextCrawlGreenPc;
extern u8 TextCrawlBluePc;
extern i32 Arcade_TextCrawlID, Arcade_TextCrawlParagraphs;
void __cdecl TextCrawl_Init(TEXT_CRAWL_VIEW_PC *crawl, i32 id, i32 reset) {
    if (crawl == NULL || QFont3DZ == NULL)
        return;
    TextCrawlCurrentPc = crawl;
    crawl->area = id >= 0 && id < AREACOUNT ? (i8)id : -1;
    NuMtxSetRotationX(&TextCrawlMatrixPc, TextCrawlRotateXPc);
    NuMtxRotateY(&TextCrawlMatrixPc, TextCrawlRotateYPc);
    NuMtxRotateZ(&TextCrawlMatrixPc, TextCrawlRotateZPc);
    NuMtxTranslate(&TextCrawlMatrixPc, &TextCrawlOffsetPc);
    if (reset != 0)
        TextCrawlTimePc = 0.0f;
}
void __cdecl TextCrawl_Draw(f32 dt, i32 paragraphs, f32 alpha, char *text) {
    AREA_DATA_PC *area = NULL;
    TEXT_CRAWL_MISSION_VIEW_PC *mission;
    i32 body_text_id = -1, episode_index = -1, area_index = -1, paragraph_count = 0;
    i8 crawl_area;
    char *episode_name = NULL, *episode_text = NULL, *chapter_text = NULL, *area_name = NULL;
    char chapter_buffer[0x40];
    i16 *bonus_text;
    NUMTX matrix;
    f32 colour_scale = 1.0f, alpha_scale = 128.0f, x_scale, y_scale, y;
    u32 red, green, colour;
    u16 encoded[1024];
    i32 i;
    if (TextCrawlCurrentPc == NULL || QFont3DZ == NULL)
        return;
    NuQFntSetJustifiedTolerances(1.2f, 1.2f);
    if (paragraphs == 0 && TextCrawlTimePc >= 60.0)
        return;
    crawl_area = TextCrawlCurrentPc->area;
    if (crawl_area != -1) {
        area = &ADataList[crawl_area];
        body_text_id = area->text_id;
        episode_index = area->episode_index;
        area_index = area->area_index;
    }
    if (text != NULL) {
        episode_name = text;
        body_text_id = -1;
    } else {
        mission = (TEXT_CRAWL_MISSION_VIEW_PC *)Mission_Active(NULL);
        if (mission != NULL) {
            if (mission->name_id != -1 && TTab[mission->name_id] != NULL)
                area_name = TTab[mission->name_id];
            body_text_id = mission->text_id;
            paragraph_count = 3;
        } else if (Arcade != 0) {
            if (area != NULL && area->name_id != -1)
                area_name = TTab[area->name_id];
            body_text_id = Arcade_TextCrawlID;
            paragraph_count = Arcade_TextCrawlParagraphs;
        } else if (episode_index == -1) {
            if (area != NULL && area->name_id != -1)
                area_name = TTab[area->name_id];
        } else {
            if (EDataList[episode_index].name_id != -1)
                episode_name = TTab[EDataList[episode_index].name_id];
            if (EDataList[episode_index].text_id != -1)
                episode_text = TTab[EDataList[episode_index].text_id];
            if (area_index != -1) {
                if (!(area->flags & 4)) {
                    if (TextCrawlCurrentPc->heading_text != NULL) {
                        sprintf(chapter_buffer, "%s %i", TTab[*TextCrawlCurrentPc->heading_text], area_index + 1);
                        chapter_text = chapter_buffer;
                    }
                } else {
                    bonus_text = area->flags & 1 ? TextCrawlCurrentPc->vehicle_bonus_text
                                                 : TextCrawlCurrentPc->character_bonus_text;
                    if (bonus_text != NULL)
                        chapter_text = TTab[*bonus_text];
                }
                if (area->name_id != -1)
                    area_name = TTab[area->name_id];
            }
        }
    }
    TextCrawlTimePc += dt;
    if (TextCrawlTimePc >= 60.0) {
        if (paragraphs == 0)
            return;
        TextCrawlTimePc = 0.0f;
    }
    NuMtxMul(&matrix, &TextCrawlMatrixPc, NuCameraGetMtx());
    NuQFntSet(QFont3DZ);
    NuQFntSetMtx(QFont3DZ, &matrix);
    NuQFntPushPrintMode(4);
    NuQFntSetCoordinateSystem(4);
    red = TextCrawlRedPc;
    green = TextCrawlGreenPc;
    if (paragraphs == 0 && TextCrawlTimePc >= 55.0) {
        colour_scale = (f32)(1.0 - ((f64)TextCrawlTimePc - 55.0) / 5.0);
        alpha_scale = (f32)(colour_scale * 128.0);
        green = (u32)((f64)TextCrawlGreenPc * colour_scale);
        red = (u32)((f64)TextCrawlRedPc * colour_scale);
    }
    colour = ((u32)(alpha_scale * alpha) << 24) | ((red & 0xff) << 16) | ((green & 0xff) << 8) | TextCrawlBluePc;
    NuQFntSetColour(QFont3DZ, colour);
    y = (f32)(TextCrawlTimePc * (f64)0.4f - 4.5);
    x_scale = TextScaleX_candidate * TextCrawlScaleXPc;
    y_scale = TextScaleY_candidate * TextCrawlScaleYPc;
    if (episode_name != NULL) {
        if (text != NULL && NuStrCmp(text, "?") == 0)
            NuQFntSetScale(QFont3DZ, (f32)(x_scale * 2.0), (f32)(y_scale * 3.0));
        else
            NuQFntSetScale(QFont3DZ, x_scale, y_scale);
        Text3DStringEncode(episode_name, encoded);
        NuQFntMove(QFont3DZ, (f32)(-NuQFntPrintLenW(QFont3DZ, encoded) * 0.5), y, 0.0f);
        NuQFntPrintW(QFont3DZ, encoded);
        y = (f32)(y + NuQFntHeight(QFont3DZ) * 1.5);
    }
    if (episode_text != NULL) {
        NuQFntSetScale(QFont3DZ, x_scale, (f32)(y_scale * 2.0));
        Text3DStringEncode(episode_text, encoded);
        NuQFntMove(QFont3DZ, (f32)(-NuQFntPrintLenW(QFont3DZ, encoded) * 0.5), y, 0.0f);
        NuQFntPrintW(QFont3DZ, encoded);
        y = (f32)(y + NuQFntHeight(QFont3DZ) * 1.5);
    }
    if (chapter_text != NULL) {
        NuQFntSetScale(QFont3DZ, x_scale, y_scale);
        Text3DStringEncode(chapter_text, encoded);
        NuQFntMove(QFont3DZ, (f32)(-NuQFntPrintLenW(QFont3DZ, encoded) * 0.5), y, 0.0f);
        NuQFntPrintW(QFont3DZ, encoded);
        y = (f32)(y + NuQFntHeight(QFont3DZ) * 1.5);
    }
    if (area_name != NULL) {
        NuQFntSetScale(QFont3DZ, x_scale, (f32)(y_scale * 2.0));
        Text3DStringEncode(area_name, encoded);
        NuQFntMove(QFont3DZ, (f32)(-NuQFntPrintLenW(QFont3DZ, encoded) * 0.5), y, 0.0f);
        NuQFntPrintW(QFont3DZ, encoded);
        y = (f32)(y + NuQFntHeight(QFont3DZ) * 1.5);
    }
    if (body_text_id != -1) {
        NuQFntSetScale(QFont3DZ, x_scale, y_scale);
        if (paragraph_count == 0)
            paragraph_count = area == NULL ? TextCrawlCurrentPc->paragraph_count : area->text_id_value;
        for (i = 0; i < paragraph_count; ++i) {
            char *paragraph = TTab[body_text_id + i];
            if (paragraph == NULL || *paragraph == '\0')
                break;
            Text3DStringEncode(paragraph, encoded);
            y += NuQFntPrintJustifiedW(QFont3DZ, encoded, (f32)(-TextCrawlLineWidthPc * 0.5), y, 0.0f, x_scale, y_scale,
                                       TextCrawlLineWidthPc, TextCrawlLineSpacingPc, colour, &matrix);
            y += NuQFntHeight(QFont3DZ);
        }
    }
    NuQFntPopPrintMode();
}

extern i32 create_qfont2dlower, create_qfont2dz, create_qfont3d, create_qfont3dz;
extern VUFNT *QFont2DZ, *QFont3D;
void __cdecl NuQFntDestroy(VUFNT *);
VUFNT *__cdecl NuQFntLoadPtr(char *, char *, i32, i32, VARIPTR *, VARIPTR *);
VUFNT *__cdecl NuQFntDuplicate(VUFNT *, i32, i32, VARIPTR *, VARIPTR *);
VUFNT *__cdecl LoadGameFont(char *, char *, VARIPTR *, VARIPTR *, i32);
VUFNT *__cdecl LoadButtonFont(char *, char *, VARIPTR *, VARIPTR *, i32);
VUFNT *__cdecl LoadGameFont(char *path, char *texture_path, VARIPTR *buffer, VARIPTR *end, i32 plane) {
    QFont2D = NuQFntLoadPtr(path, texture_path, 4, plane, buffer, end);
    if (QFont2D != NULL) {
        if (create_qfont2dlower != 0)
            QFont2DLower = NuQFntDuplicate(QFont2D, 4, plane - 1, buffer, end);
        if (create_qfont2dz != 0)
            QFont2DZ = NuQFntDuplicate(QFont2D, 4, 0x40000, buffer, end);
        if (create_qfont3d != 0)
            QFont3D = NuQFntDuplicate(QFont2D, 0x18, plane, buffer, end);
        if (create_qfont3dz != 0)
            QFont3DZ = NuQFntDuplicate(QFont2D, 0x4c, plane, buffer, end);
    }
    return QFont2D;
}

void __cdecl UnloadGameFont(void) {
    if (QFont2DLower != NULL) {
        NuQFntDestroy(QFont2DLower);
        QFont2DLower = NULL;
    }
    if (QFont2DZ != NULL) {
        NuQFntDestroy(QFont2DZ);
        QFont2DZ = NULL;
    }
    if (QFont3D != NULL) {
        NuQFntDestroy(QFont3D);
        QFont3D = NULL;
    }
    if (QFont3DZ != NULL) {
        NuQFntDestroy(QFont3DZ);
        QFont3DZ = NULL;
    }
    NuQFntDestroy(QFont2D);
}

VUFNT *__cdecl LoadButtonFont(char *path, char *texture_path, VARIPTR *buffer, VARIPTR *end, i32 plane) {
    if (QFont2DButtons == NULL) {
        QFont2DButtons = NuQFntLoadPtr(path, texture_path, 4, plane, buffer, end);
        if (QFont2DButtons != NULL) {
            FixUpButtonsInFont(QFont2D, QFont2DButtons);
            QFont2DButtons->baseline = QFont2DButtons->height / QFont2D->height * QFont2D->baseline;
        }
    }
    return QFont2DButtons;
}

void __cdecl Text_LoadFont(char *path, VARIPTR *buffer, VARIPTR *end) {
    create_qfont3dz = 1;
    Text_AppFont = LoadGameFont(path, path, buffer, end, 1);
    LoadButtonFont("stuff\\text\\Buttons", NULL, buffer, end, 0);
}
