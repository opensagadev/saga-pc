#include "saga.h"
#include <stdio.h>
#include <string.h>

/* Retail unit 0x4cf2a0-0x4cf653: definitions in retail address order. */

extern i16 TextIdPc880894, TextIdPc880898;
char *Text_GetLanguagePath(i32 language);
__declspec(noinline) void IntroText_SetTextID(i32 id);
void __cdecl Text_LoadAndFixUpStrings(char *path, u8 **buffer, char **table, i32 count);
__declspec(noinline) void TextRegisterButtonMapFn(void (*fn)(char *, char *));

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
    if (NuStrICmp(source, Text_WibbleCodeword) == 0) {
        NuStrCpy(destination, Text_CrossCodeword);
    }
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
