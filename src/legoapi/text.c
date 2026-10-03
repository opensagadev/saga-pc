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
extern const char TextLoadPermTextDirectory[12];
extern const char TextLoadPermAPIExtension[9];
extern const char TextLoadPermTRCPath[19];
extern i16 tALONGTIMEAGO;

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

    STUBBED_NAMED("Text_LoadStrings: intro-label fixups and trc.csv lookup setup");
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

i32 __cdecl GetMenuID(void) {
    i16 menu = GameMenu[GameMenuLevel].menu;
    return menu == -1 ? -1 : MenuInfo[menu].id;
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

void __cdecl Text3DStringEncode(char *source, u16 *destination) {
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
    while (source[input] != 0 && output < 480) {
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
        } else if ((source[input] == 0xe3 && source[input + 1] == 0x80 &&
                    (source[input + 2] == 0x81 || source[input + 2] == 0x82 || source[input + 2] == 0x85)) ||
                   (source[input] == 0xef && source[input + 1] == 0xbc &&
                    (source[input + 2] == 0x81 || source[input + 2] == 0x9f))) {
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
void __cdecl Text3DEx(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment, u8 red,
                      u8 green, u8 blue, i32 alpha) {
    VUFNT *font;
    f32 draw_x_scale, draw_y_scale, width, height, baseline_y, draw_y;
    f32 button_scale, button_width, normal_width, ratio, scaled_width;
    f64 half_height;
    u8 decoded[512], buttons[512], normal[512], fragment[1024];
    u16 encoded[512], button_encoded[512], normal_encoded[512];
    i32 button_count, normal_count, count, button_font, is_button, next_button, preset, i;
    u32 opacity, base_colour, colour;
    u8 *output;
    i32 *selected_count;

    text3d_width = text3d_height = 0.0f;
    font = SmartTextFont != NULL ? SmartTextFont : QFont2D;
    if (font == NULL || text == NULL || text[0] == '\0' || MenuStopDraw != 0 ||
        (x < -2.0f || x > 2.0f || y < -2.0f || y > 2.0f))
        return;
    draw_x_scale = TextScaleX_candidate * x_scale;
    draw_y_scale = TextScaleY_candidate * y_scale;
    NuQFntPushPrintMode(TextPrintModePc == 0 ? 2 : 3);

    TextDecode(text, decoded);
    font = SmartTextFont != NULL ? SmartTextFont : QFont2D;
    if (followon_line == 0)
        NuQFntSet(font);

    Text3DStringEncode((char *)decoded, encoded);
    NuQFntSetScale(font, draw_x_scale * QFONTSCALEX, draw_y_scale * QFONTSCALEY);
    width = NuQFntPrintLenW(font, encoded);
    text3d_width = width;
    height = NuQFntHeight(font);
    text3d_height = height;
    half_height = (f64)height * 0.5;
    baseline_y = y + NuQFntBaseline(font);
    draw_y = (f32)((f64)baseline_y - half_height);
    button_scale = draw_x_scale;
    if (draw_x_scale != draw_y_scale) {
        if (smarttext_fwn == 1)
            button_scale = draw_x_scale;
        else if (smarttext_fwn == 2)
            button_scale = draw_y_scale;
        else
            button_scale = 0.5f * (draw_x_scale + draw_y_scale);

        button_count = 0;
        normal_count = 0;
        for (i = 0; decoded[i] != 0;) {
            is_button = decoded[i] == 0xd4 && decoded[i + 1] >= 0xb1 && decoded[i + 1] <= 0xbf;
            output = is_button ? buttons : normal;
            selected_count = is_button ? &button_count : &normal_count;
            output[(*selected_count)++] = decoded[i++];
            while ((decoded[i] & 0xc0) == 0x80)
                output[(*selected_count)++] = decoded[i++];
        }
        buttons[button_count] = normal[normal_count] = 0;
        button_width = 0.0f;
        normal_width = 0.0f;

        if (button_count != 0) {
            Text3DStringEncode((char *)buttons, button_encoded);
            NuQFntSetScale(font, button_scale * QFONTSCALEX, button_scale * QFONTSCALEY);
            button_width = NuQFntPrintLenW(font, button_encoded);
        }
        if (normal_count != 0) {
            Text3DStringEncode((char *)normal, normal_encoded);
            NuQFntSetScale(font, draw_x_scale * QFONTSCALEX, draw_y_scale * QFONTSCALEY);
            normal_width = NuQFntPrintLenW(font, normal_encoded);
        }
        if (normal_width != 0.0f) {
            ratio = (width - button_width) / normal_width;
            draw_x_scale = ratio * draw_x_scale;
        } else {
            button_scale = draw_x_scale;
            if (smarttext_fwn == 2) {
                scaled_width = draw_y_scale * width;
                width = scaled_width / draw_x_scale;
                button_scale = draw_y_scale;
            }
        }
    }
    if ((alignment & 4) != 0)
        draw_y -= height * 0.5f;
    else if ((alignment & 1) != 0)
        draw_y += height * 0.5f;
    if ((alignment & 2) == 0)
        x -= (alignment & 8) != 0 ? width : width * 0.5f;
    opacity = (u32)(alpha) << 24;
    base_colour = opacity | red | ((u32)(green) << 8) | ((u32)(blue) << 16);
    if (apitext_half_rgb != 0)
        base_colour = opacity | (red >> 1) | ((u32)(green >> 1) << 8) | ((u32)(blue >> 1) << 16);
    if (followon_line == 0)
        current_rgba = base_colour;

    count = 0;
    button_font = 0;
    for (i = 0; decoded[i] != 0 && count < 509;) {
        if (decoded[i] == '~') {
            colour = base_colour;
            if (decoded[i + 1] != '~') {
                colour = opacity;
                if (red != 0 || green != 0 || blue != 0) {
                    preset = decoded[i + 1] - '0';
                    if (apitext_half_rgb == 0)
                        colour |= (u8)(text_preset_r[preset] * 2) | ((u32)((text_preset_g[preset] * 2) & 255) << 8) |
                                  ((u32)((text_preset_b[preset] * 2) & 255) << 16);
                    else
                        colour |= text_preset_r[preset] | ((u32)(text_preset_g[preset]) << 8) |
                                  ((u32)(text_preset_b[preset]) << 16);
                }
            }
            i += 2;
            if (count != 0 && colour != current_rgba) {
                fragment[count] = 0;
                x += TextPrintSubstring(fragment, x, draw_y, z, button_font ? button_scale : draw_x_scale,
                                        button_font ? button_scale : draw_y_scale, current_rgba, button_font);
                count = 0;
            }
            current_rgba = colour;
            continue;
        }
        next_button = decoded[i] == 0xd4 && decoded[i + 1] >= 0xb1 && decoded[i + 1] <= 0xbf;
        if (count != 0 && next_button != button_font) {
            fragment[count] = 0;
            x += TextPrintSubstring(fragment, x, draw_y, z, button_font ? button_scale : draw_x_scale,
                                    button_font ? button_scale : draw_y_scale, current_rgba, button_font);
            count = 0;
        }
        button_font = next_button;
        fragment[count++] = decoded[i++];
        while ((decoded[i] & 0xc0) == 0x80)
            fragment[count++] = decoded[i++];
    }
    if (count != 0) {
        fragment[count] = 0;
        TextPrintSubstring(fragment, x, draw_y, z, button_font ? button_scale : draw_x_scale,
                           button_font ? button_scale : draw_y_scale, current_rgba, button_font);
    }
    NuQFntPopPrintMode();
}

void __cdecl SetQFont2D(void) {
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
    colour = ((u32)(i64)((f64)alpha * 128.0) << 24) | 0x7f5f00;
    NuQFntPrintJustifiedW(QFont2D, encoded, -0.85f, INTROTEXT_Y, 1.0f, INTROTEXT_SCALE, INTROTEXT_SCALE, 1.7f, 1.0f,
                          colour, NULL);
}
