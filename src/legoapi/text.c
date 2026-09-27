#include "saga.h"

#include <string.h>

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
