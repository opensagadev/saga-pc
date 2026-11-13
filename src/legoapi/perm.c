#include "saga.h"

static void LoadPermBuildLegalTextureName(char *name) {
    i32 pal = NuRegionGet() != 2;
    NuStrCpy(name, PermLegalBase);
    if (!pal && (Text_Language == 1 || Text_Language == 0x12)) {
        NuStrCat(name, PermLegalUS);
    } else if (!pal && Text_Language == 2) {
        NuStrCat(name, PermLegalFrench);
    } else if (!pal && Text_Language == 3) {
        NuStrCat(name, PermLegalSpanish);
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

void __cdecl LoadPermLoadLegalTexture(void) {
    VARIPTR texture_base, texture_end;
    char path[128], name[64];
    if (legal_tid != 0 || loadlegal_done != 0)
        return;
    texture_end = superbuffer_end;
    texture_base.addr = texture_end.addr - 0x400000;
    Text_Language = NuLanguageGet();
    if (StartupGameFlag1 != 0) {
        NuStrCpy(path, PermLegalDirectoryPc);
        LoadPermBuildLegalTextureName(name);
        NuStrCat(path, name);
    } else {
        NuStrCpy(path, PermLegalDirectoryPc);
        NuStrCat(path, PermLegalPcPrefixPc);
        NuStrCat(path, PermLegalBase);
        if (PAL != 0 || (Text_Language != 1 && Text_Language != 0x12)) {
            NuStrCat(path, PermLegalSeparator);
            NuStrCat(path, Text_GetLanguagePath(Text_Language));
        }
    }
    NuStrCat(path, NuFileExt_dds_PC);
    legal_tid = NuTexRead(path, &texture_base, texture_end);
    loadlegal_done = 1;
}
