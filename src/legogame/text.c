#include "gameapi/apitext.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/customiser.h"
#include "legoapi/core/startup/text.h"
#include "legoapi/lib/text.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/world/levels.h"
#include "legogame/game.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nustring.h"
#include <stdio.h>
#include <string.h>
#include "legoapi/text_tables.h"

void Text_SetLanguage_Game(i32 language) {
    switch (language) {
        case NULANGUAGE_GERMAN:
            INTROTEXT_Y = 0.13f;
            INTROTEXT_SCALE = 0.575f;
            break;
        case NULANGUAGE_FRENCH:
            INTROTEXT_Y = 0.115f;
            INTROTEXT_SCALE = 0.5f;
            break;
        case NULANGUAGE_SPANISH:
            INTROTEXT_Y = 0.14f;
            INTROTEXT_SCALE = 0.61f;
            break;
        case NULANGUAGE_DANISH:
            INTROTEXT_Y = 0.15f;
            INTROTEXT_SCALE = 0.67f;
            break;
        case NULANGUAGE_ITALIAN:
            INTROTEXT_Y = 0.17f;
            INTROTEXT_SCALE = 0.76f;
            break;
        case NULANGUAGE_JAPANESE:
            INTROTEXT_Y = 0.0f;
            INTROTEXT_SCALE = 1.8f;
            break;
        default:
            INTROTEXT_Y = 0.175f;
            INTROTEXT_SCALE = 0.79f;
            break;
    }
}

char *apitxt_ENGLISH = (char *)"English";
char *apitxt_FRENCH = (char *)("Fran\xc3\xa7"
                               "ais");
char *apitxt_GERMAN = (char *)"Deutsch";
char *apitxt_ITALIAN = (char *)"Italiano";
char *apitxt_SPANISH = (char *)("Espa\xc3\xb1"
                                "ol");
char *apitxt_DANISH = (char *)"Dansk";
char *txt_NULL = (char *)"?";
char *txt_UNKNOWN = (char *)"?";
i16 tNULL = 0;
i16 tUNKNOWN = 0;
i16 tENGLISH = 0;
i16 tFRENCH = 0;
i16 tGERMAN = 0;
i16 tITALIAN = 0;
i16 tSPANISH = 0;
i16 tDANISH = 0;

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

void Text_DecodeButtons(char *source, char *destination) {
    if (NuStrICmp(source, "wibble") == 0) {
        NuStrCpy(destination, "cross");
    }
}

void Text_LoadStrings(VARIPTR *buffer, VARIPTR *buffer_end) {
    char language[32];
    char path[256];
    u8 *string_buffer = buffer->void_ptr;

    TextRegisterButtonMapFn(Text_DecodeButtons);
    NuStrCpy(language, Text_GetLanguagePath(Text_Language));

    NuStrCpy(path, "stuff\\text\\");
    NuStrCat(path, language);
    NuStrCat(path, ".txt");
    Text_LoadAndFixUpStrings(path, &string_buffer, TTab, 0x70d);

    if (StartupGameFlag1 != 0) {
        IntroText_SetTextID(tALONGTIMEAGO);
    } else {
        NuStrCpy(path, "stuff\\text\\");
        NuStrCat(path, language);
        NuStrCat(path, "_api.txt");
        Text_LoadAndFixUpStrings(path, &string_buffer, TTab, 0x70d);
    }

    buffer->addr = ((u32)string_buffer + 3U) & ~3U;

    if (TTab[dat_00880894] != NULL) {
        NuStrCpy(Game.customiser_start[0].name, TTab[dat_00880894]);
    }
    if (TTab[dat_00880898] != NULL) {
        NuStrCpy(Game.customiser_start[1].name, TTab[dat_00880898]);
    }
    FinishWeirdoNames(-1);
    Customiser_InitNames(CharacterCustomiser);
    MenuLoadTechnicalStrings("stuff\\text\\trc.csv", language, buffer, *buffer_end);
}
