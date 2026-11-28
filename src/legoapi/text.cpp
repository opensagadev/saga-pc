#include "gameapi/apitext.h"
#include "gameapi/gui/apimenu.h"
#include "gameapi/gui/apimenu_internal.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/startup/text.h"
#include "legoapi/gizmos/transport/grapples.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/collect/minikits.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/lib/text.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/episode.h"
#include "legoapi/world/mission.h"
#include "legogame/game.h"
#include "legogame/pcbatman.h"
#include "nu2api/PCInput.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include <stdio.h>
#include <string.h>
#include "nu2api/nucore/nucore_includes.h"

static char **TTab_Original = 0;
char **TTab = 0;
const char *LEGOASCII_UP = NULL;
const char *LEGOASCII_DOWN = NULL;
const char *LEGOASCII_LEFT = NULL;
const char *LEGOASCII_RIGHT = NULL;
const char *LEGOASCII_BIGARROW = NULL;
static char *Text_ErrorString = "Err...";
char *Text_PathEnglish = (char *)"english";
char *Text_PathFrench = (char *)"french";
char *Text_PathGerman = (char *)"german";
char *Text_PathItalian = (char *)"italian";
char *Text_PathSpanish = (char *)"spanish";
char *Text_PathDanish = (char *)"danish";
char *Text_PathJapanese = (char *)"japanese";
i32 Text_Language = 1;
LANGUAGEDATA *Text_LanguageList = Text_LanguageList_Default;
i32 LANGUAGECOUNT = 6;
i32 INTROTEXT_ID = -1;
f32 INTROTEXT_SCALE = 0.79f;
f32 INTROTEXT_Y = 0.175f;
f32 TextCrawlScaleX = 0.01f;
f32 TextCrawlScaleY = 0.01f;
f32 TextCrawlLineWidth = 7.0f;
f32 TextCrawlLineSpacing = 1.3f;
static i32 Text_MaxStrings_Overall = 0;
static u32 *Text_StringBits = 0;
void (*Text_GameSetLanguageFn)(i32) = 0;

extern "C" void fun_0054b510(void) {
}

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
        case NULANGUAGE_FRENCH:
            return Text_PathFrench;
        case NULANGUAGE_GERMAN:
            return Text_PathGerman;
        case NULANGUAGE_ITALIAN:
            return Text_PathItalian;
        case NULANGUAGE_SPANISH:
            return Text_PathSpanish;
        case NULANGUAGE_DANISH:
            return Text_PathDanish;
        case NULANGUAGE_JAPANESE:
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

    TTab_Original = table;
    buf->addr = (buf->addr + table_size + 3) & ~3U;
    memset(table, 0, table_size);
    table[0] = Text_ErrorString;
    TTab = table + 1;
    Text_MaxStrings_Overall = count;
    Text_StringBits = (u32 *)buf->void_ptr;
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
    i32 count;
    i32 output;
    i32 digit;
    if (Text_Language == NULANGUAGE_FRENCH) {
        separator = ' ';
    } else if (Text_Language != NULANGUAGE_ENGLISH && Text_Language != NULANGUAGE_US_ENGLISH && Text_Language != NULANGUAGE_JAPANESE) {
        separator = '.';
    }
    if (length < 0) {
        count = NuStrLen(number);
    } else {
        count = length;
    }
    output = 0;
    for (digit = 0; digit < count; ++digit) {
        i32 remaining = count - digit - 1;
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
    char *first = end - 1;
    *end = '\0';
    *first = (char)('0' + score % 10);
    score /= 10;
    while (score != 0) {
        *--first = (char)('0' + score % 10);
        score /= 10;
    }

    Text_InsertCommasIntoNumber(first, text, end - first);
}

i32 Text_StripComments(char *text, char *dest, i32 separators) {
    i32 count = 0;
    char *end = text + NuStrLen(text);

    while (text <= end) {
        char character = *text;
        if (character == ';' || character == '#' || (character == '/' && *++text == '/')) {
            ++text;
            while ((character = *text) != '\r' && character != '\n' && character != '\0') {
                ++text;
            }
        }
        character = *text++;
        if (separators != 0 && (character == '=' || character == ',')) {
            character = ' ';
        }
        *dest++ = character;
        ++count;
    }
    *dest = '\0';
    return count;
}

void IntroText_SetTextID(i32 id) {
    if (id > 0 && TTab[id] != NULL) {
        INTROTEXT_ID = id;
    }
}

static i32 Text_IsOtherPlatformWord(NUFPAR *parser) {
    if (NuStrICmp(parser->word_buffer_ptr, "360") == 0) {
        return 1;
    }
    if (NuStrICmp(parser->word_buffer_ptr, "gc") == 0) {
        return 1;
    }
    if (NuStrICmp(parser->word_buffer_ptr, "ps2") == 0) {
        return 1;
    }
    if (NuStrICmp(parser->word_buffer_ptr, "ps3") == 0) {
        return 1;
    }
    if (NuStrICmp(parser->word_buffer_ptr, "psp") == 0) {
        return 1;
    }
    if (NuStrICmp(parser->word_buffer_ptr, "pc") == 0) {
        NuFParGetWord(parser);
        return 0;
    }
    if (NuStrICmp(parser->word_buffer_ptr, "wii") == 0) {
        return 1;
    }
    if (NuStrICmp(parser->word_buffer_ptr, "xbox") == 0) {
        return 1;
    }
    return 0;
}

void Text_LoadAndFixUpStrings(char *path, u8 **buffer, char **table, i32 count) {
    u8 *out = *buffer;
    NUFPAR *parser = NuFParCreate(path);
    i32 index;
    i32 length;

    if (parser != NULL) {
        if (parser->is_utf16 != 0) {
            while (NuFParGetLineW(parser) != 0) {
                index = NuFParGetInt(parser);
                if (index > 0 && index < count) {
                    u16 *word;
                    NuFParGetWordW(parser);
                    word = parser->word_buffer_wide;
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
                                if (*word < 0x80) {
                                    *out++ = *word;
                                } else {
                                    out = NuUTF8CharFromUnicode(out, *word);
                                }
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

char HubButtonStringBuffer[72] = "???";
u32 GAMEPAD_MENUSELECT = PAD_BUTTON_CROSS;

void Text_MakeTime(f32 time, i32 show_hours, i32 show_minutes, i32 show_centiseconds, char *text) {
    i32 hours;
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
        hours = 0;
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
            sprintf(text, "%i:%.2i:%.2i.%.2i", hours, minutes, seconds, centiseconds);
        } else {
            sprintf(text, "%i:%.2i:%.2i", hours, minutes, seconds);
        }
    } else if (show_minutes != 0) {
        if (show_centiseconds != 0) {
            sprintf(text, "%i:%.2i.%.2i", minutes, seconds, centiseconds);
        } else {
            sprintf(text, "%i:%.2i", minutes, seconds);
        }
    } else if (show_centiseconds != 0) {
        sprintf(text, "%i.%.2i", seconds, centiseconds);
    } else {
        sprintf(text, "%i", seconds);
    }
}

void Text_ExpandAllButtonStrings(char *input, char *output) {
    char token[256];
    char expanded[64];

    *output = '\0';
    while (*input != '\0') {
        i32 token_length;
        if (*input == '[') {
            token[0] = *input;
            token_length = 1;
            while (input[token_length] != '\0' && input[token_length] != ']') {
                token[token_length] = input[token_length];
                ++token_length;
            }
            if (input[token_length] != '\0') {
                token[token_length++] = ']';
                token[token_length] = '\0';
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
            }
            *output = *input;
            output[1] = '\0';
            ++output;
            ++input;
        } else {
            *output = *input;
            output[1] = '\0';
            ++output;
            ++input;
        }
    }
    *output = '\0';
}

void Text_SetLanguage(i32 language) {
    Text_Language = NuLanguageGet();
    if (Text_GameSetLanguageFn != NULL) {
        Text_GameSetLanguageFn(Text_Language);
    }
}

void IntroText_Draw(f32 alpha) {
    u16 encoded[128];
    u32 colour;
    if (INTROTEXT_ID == -1) {
        return;
    }
    SetQFont2D();
    NuQFntSetSpaceWidth(QFont2D, 8.0f);
    Text3DStringEncode(TTab[INTROTEXT_ID], encoded);
    NuQFntSetJustifiedTolerances(1.0f, 1.0f);
    colour = ((u32)((f64)alpha * 128.0) << 24) | 0x7f5f00;
    NuQFntPrintJustifiedW(QFont2D, encoded, -0.85f, INTROTEXT_Y, 1.0f, INTROTEXT_SCALE, INTROTEXT_SCALE, 1.7f, 1.0f, colour, NULL);
}

static TEXTCRAWL *TextCrawlCurrent = 0;
u16 TextCrawlRotateY = 0;
u16 TextCrawlRotateZ = 0;
u32 TextCrawlRed = 0;
static NUMTX TextCrawlMatrix = {0};
void *Text_AppFont = 0;
static NUVEC TextCrawlOffset = {0.0f, -1.0f, 10.0f};
u8 TextCrawlBlue = 255;
u32 TextCrawlGreen = 111;
u16 TextCrawlRotateX = 13107;

void TextCrawl_Init(TEXTCRAWL *crawl, i32 id, i32 reset) {
    if (crawl == NULL || QFont3DZ == NULL) {
        return;
    }
    TextCrawlCurrent = crawl;
    crawl->area = (i8)(id >= 0 && id < AREACOUNT ? id : -1);
    NuMtxSetRotationX(&TextCrawlMatrix, TextCrawlRotateX);
    NuMtxRotateY(&TextCrawlMatrix, TextCrawlRotateY);
    NuMtxRotateZ(&TextCrawlMatrix, TextCrawlRotateZ);
    NuMtxTranslate(&TextCrawlMatrix, &TextCrawlOffset);
    if (reset != 0) {
        TextCrawlTime = 0.0f;
    }
}

void TextCrawl_Draw(f32 dt, i32 paragraphs, f32 alpha, char *text) {
    AREA_DATA *area;
    MISSIONDATA *mission;
    i32 body_text_id;
    i32 episode_index;
    i32 area_index;
    i32 paragraph_count;
    i8 crawl_area;
    char *episode_name;
    char *episode_text;
    char *chapter_text;
    char *area_name;
    char chapter_buffer[0x40];
    NUMTX matrix;
    f32 colour_scale = 1.0f;
    f32 alpha_scale = 128.0f;
    f32 x_scale;
    f32 y_scale;
    f32 y;
    i32 red;
    i32 green;
    u32 colour;
    u16 encoded[1024];
    i32 i;
    if (TextCrawlCurrent == NULL || QFont3DZ == NULL) {
        return;
    }
    NuQFntSetJustifiedTolerances(1.2f, 1.2f);
    if (paragraphs == 0 && TextCrawlTime >= 60.0) {
        return;
    }
    crawl_area = TextCrawlCurrent->area;
    if (crawl_area != -1) {
        area = &ADataList[crawl_area];
        body_text_id = area->text_id;
        episode_index = area->episode_index;
        area_index = area->area_index;
    } else {
        area = NULL;
        episode_index = -1;
        body_text_id = -1;
        area_index = -1;
    }
    paragraph_count = 0;
    episode_name = NULL;
    episode_text = NULL;
    chapter_text = NULL;
    area_name = NULL;
    if (text != NULL) {
        episode_name = text;
        body_text_id = -1;
    } else {
        mission = Mission_Active(NULL);
        if (mission != NULL) {
            i32 name_id = mission->name_id;
            if (name_id != -1 && TTab[name_id] != NULL) {
                area_name = TTab[name_id];
            }
            body_text_id = mission->text_id;
            paragraph_count = 3;
        } else if (Arcade != 0) {
            if (area != NULL && area->name_id != -1) {
                area_name = TTab[area->name_id];
            }
            body_text_id = Arcade_TextCrawlID;
            paragraph_count = Arcade_TextCrawlParagraphs;
        } else if (episode_index != -1) {
            if (EDataList[episode_index].name_id != -1) {
                episode_name = TTab[EDataList[episode_index].name_id];
            }
            if (EDataList[episode_index].text_id != -1) {
                episode_text = TTab[EDataList[episode_index].text_id];
            }
            if (area_index != -1) {
                if (area->flags & AREAFLAG_BONUS_AREA) {
                    if (area->flags & AREAFLAG_VEHICLE_AREA) {
                        if (TextCrawlCurrent->vehicle_bonus_text != NULL) {
                            chapter_text = TTab[*TextCrawlCurrent->vehicle_bonus_text];
                        }
                    } else {
                        if (TextCrawlCurrent->character_bonus_text != NULL) {
                            chapter_text = TTab[*TextCrawlCurrent->character_bonus_text];
                        }
                    }
                } else {
                    if (TextCrawlCurrent->heading_text != NULL) {
                        sprintf(chapter_buffer, "%s %i", TTab[*TextCrawlCurrent->heading_text], area_index + 1);
                        chapter_text = chapter_buffer;
                    }
                }
                if (area->name_id != -1) {
                    area_name = TTab[area->name_id];
                }
            }
        } else {
            if (area != NULL && area->name_id != -1) {
                area_name = TTab[area->name_id];
            }
        }
    }
    TextCrawlTime += dt;
    if (TextCrawlTime >= 60.0) {
        if (paragraphs == 0) {
            return;
        }
        TextCrawlTime = 0.0f;
    }
    NuMtxMul(&matrix, &TextCrawlMatrix, NuCameraGetMtx());
    NuQFntSet(QFont3DZ);
    NuQFntSetMtx(QFont3DZ, &matrix);
    NuQFntPushPrintMode(NUQFNT_MODE_3D);
    NuQFntSetCoordinateSystem(NUQFNT_CS_UNIT_Y_DOWN);
    alpha_scale = 128.0f;
    red = TextCrawlRed;
    green = TextCrawlGreen;
    if (paragraphs == 0 && TextCrawlTime >= 55.0) {
        colour_scale = (f32)(1.0 - ((f64)TextCrawlTime - 55.0) / 5.0);
        alpha_scale = (f32)(128.0 * colour_scale);
        green = (i32)(TextCrawlGreen * colour_scale);
        red = (i32)(TextCrawlRed * colour_scale);
    }
    colour = ((u32)(alpha_scale * alpha) << 24) | ((red & 0xff) << 16) | ((green & 0xff) << 8) | TextCrawlBlue;
    NuQFntSetColour(QFont3DZ, colour);
    y = (f32)(TextCrawlTime * (f64)0.4f - 4.5);
    x_scale = TextCrawlScaleX * TextScaleX;
    y_scale = TextCrawlScaleY * TextScaleY;
    if (episode_name != NULL) {
        if (text != NULL && NuStrCmp(text, "?") == 0) {
            NuQFntSetScale(QFont3DZ, (f32)(x_scale * 2.0), (f32)(y_scale * 3.0));
        } else {
            NuQFntSetScale(QFont3DZ, x_scale, y_scale);
        }
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
        if (paragraph_count == 0) {
            paragraph_count = area == NULL ? TextCrawlCurrent->paragraph_count : area->text_id_value;
        }
        for (i = 0; i < paragraph_count; ++i) {
            char *paragraph = TTab[body_text_id + i];
            if (paragraph == NULL || *paragraph == '\0') {
                break;
            }
            Text3DStringEncode(paragraph, encoded);
            y += NuQFntPrintJustifiedW(QFont3DZ, encoded, (f32)(-TextCrawlLineWidth * 0.5), y, 0.0f, x_scale, y_scale, TextCrawlLineWidth,
                                       TextCrawlLineSpacing, colour, &matrix);
            y += NuQFntHeight(QFont3DZ);
        }
    }
    NuQFntPopPrintMode();
}

void Text_LoadFont(char *path, VARIPTR *buffer, VARIPTR *end) {
    create_qfont3dz = 1;
    Text_AppFont = LoadGameFont(path, path, buffer, end, 1);
    LoadButtonFont("stuff\\text\\Buttons", NULL, buffer, end, 0);
}

f32 MenuTextScale = 0.5f;
f32 MENUDY = -0.10664f;
f32 TextScaleX = 1.0f;
f32 TextScaleY = 1.0f;
f32 MenuPromptPositionScale = 1.0f;

extern "C" void MenuDrawViewTextStrings(MENU *menu) {
    char text[2048];
    i32 i;
    menu->item_scale = 2.0f;
    menu->centre_offset = MENUDY * 2.0f;
    menu->draw_x = -0.75f;
    menu->draw_y = (f32)-menu->selected_row * menu->centre_offset;
    for (i = 0; i < Text_MaxStrings_Overall; ++i) {
        f32 y;
        i32 red;
        i32 green;
        i32 blue;
        dme_sx = 0.6f;
        dme_sy = menu->item_scale;
        dme_align = 0;
        GameDrawMenuEntry(menu, " ");
        y = menu->draw_y - menu->centre_offset;
        if (MenuStopDraw != 0) {
            continue;
        }
        if (!(y >= -1.25f && y < 1.25f)) {
            continue;
        }
        if ((Text_StringBits[i / 32] & (1U << (i & 31))) != 0) {
            if (i == menu->selected_item) {
                if (menu_flash != 0) {
                    red = MENUFLASH0R;
                    green = MENUFLASH0G;
                    blue = MENUFLASH0B;
                } else {
                    red = MENUFLASH1R;
                    green = MENUFLASH1G;
                    blue = MENUFLASH1B;
                }
                if (menu_pulsate > 0.0f) {
                    red = (i32)(MENUFLASH0R * (f64)menu_pulsate + MENUFLASH1R * (1.0 - menu_pulsate));
                    green = (i32)(MENUFLASH0G * (f64)menu_pulsate + MENUFLASH1G * (1.0 - menu_pulsate));
                    blue = (i32)(MENUFLASH0B * (f64)menu_pulsate + MENUFLASH1B * (1.0 - menu_pulsate));
                }
            } else {
                red = MENUENTRYR;
                green = MENUENTRYG;
                blue = MENUENTRYB;
            }
        } else {
            red = 255;
            if (i == menu->selected_item) {
                if (menu_flash != 0) {
                    green = 255;
                    blue = 255;
                } else {
                    green = 191;
                    blue = 0;
                }
            } else {
                green = 0;
                blue = 0;
            }
        }
        dme_r = (u8)red;
        dme_g = (u8)green;
        dme_b = (u8)blue;
        dme_rgb = 1;
        sprintf(text, "%i", i);
        SmartTextEx(text, -0.76f, y, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 8, red, green, blue, 0.09000003f, 1, NULL, 0,
                    MenuA);
        if (TTab[i] != NULL) {
            dme_rgb = 1;
            Text_ExpandAllButtonStrings(TTab[i], text);
            SmartTextEx(text, -0.74f, y, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 2, red, green, blue, 1.59f, 2, NULL, 0, MenuA);
        }
    }
}

extern "C" void MenuUpdateViewTextStrings(MENU *menu) {
    if (menu->cancel_pressed != 0) {
        BackupMenu();
        return;
    }
    if ((Player[0] == NULL || (Player[0]->player.pad->buttons_held & GAMEPAD_MENUSELECT) == 0) &&
        (Player[1] == NULL || (Player[1]->player.pad->buttons_held & GAMEPAD_MENUSELECT) == 0)) {
        return;
    }
    if (menu->up_held != 0) {
        menu->selected_row -= 5;
        if (menu->selected_row < menu->first_row) {
            menu->selected_row = menu->last_row;
        }
        menu->selected_item = menu->selected_row - menu->first_row;
    } else if (menu->down_held != 0) {
        menu->selected_row += 5;
        if (menu->selected_row > menu->last_row) {
            menu->selected_row = menu->first_row;
        }
        menu->selected_item = menu->selected_row - menu->first_row;
    }
}
