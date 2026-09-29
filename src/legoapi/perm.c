#include "saga.h"

void __cdecl LoadPermBuildLegalTextureName(char *name) {
    i32 region;

    /* Retail receives this output pointer in EDI; this C wrapper is structural. */
    region = NuRegionGet();
    NuStrCpy(name, PermLegalBase);
    if (region == 2) {
        if (Text_Language == 1 || Text_Language == 0x12) {
            NuStrCat(name, PermLegalUS);
        } else if (Text_Language == 2) {
            NuStrCat(name, PermLegalFrench);
        } else if (Text_Language == 3) {
            NuStrCat(name, PermLegalSpanish);
        } else {
            NuStrCat(name, PermLegalSeparator);
            NuStrCat(name, Text_GetLanguagePath(Text_Language));
        }
    } else {
        NuStrCat(name, PermLegalSeparator);
        NuStrCat(name, Text_GetLanguagePath(Text_Language));
    }
    if (Text_Language == 0) {
        TextScaleX_candidate = PermLegalScale;
        TextScaleY_candidate = PermLegalScale;
    }
}

void __cdecl SetBackgroundMusic(i32 track) {
    (void)track;
}
