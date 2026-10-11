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

extern const char PermLegalDirectoryPc[];
extern const char PermLegalPcPrefixPc[];
extern const char NuFileExt_dds_PC[5];

void __cdecl LoadPermLoadLegalTexture(void) {
    VARIPTR texture_base, texture_end;
    char path[128], name[64];
    if (legal_tid != 0 || loadlegal_done != 0)
        return;
    texture_end = superbuffer_end;
    texture_base.addr = texture_end.addr - 0x400000;
    Text_Language = NuLanguageGet();
    NuStrCpy(path, PermLegalDirectoryPc);
    if (StartupGameFlag1 != 0) {
        LoadPermBuildLegalTextureName(name);
        NuStrCat(path, name);
    } else {
        NuStrCat(path, PermLegalPcPrefixPc);
        NuStrCat(path, PermLegalBase);
        if (PAL != 0 || (Text_Language != 1 && Text_Language != 0x12)) {
            NuStrCat(path, PermLegalSeparator);
            NuStrCat(path, Text_GetLanguagePath(Text_Language));
        }
    }
    NuStrCat(path, NuFileExt_dds_PC);
    legal_tid = NuTexReadPc(path, &texture_base, texture_end);
    loadlegal_done = 1;
}

typedef struct LEGAL_MATERIAL_PC {
    u8 reserved00[0x40];
    u32 attributes;
    u8 reserved44[0x10];
    NUVEC diffuse;
    u8 reserved60[0x10];
    f32 opacity;
    i16 texture_id;
    u8 reserved76[0x3e];
    u32 shader_flags;
} LEGAL_MATERIAL_PC;
STATIC_ASSERT(offsetof(LEGAL_MATERIAL_PC, attributes) == 0x40);
STATIC_ASSERT(offsetof(LEGAL_MATERIAL_PC, diffuse) == 0x54);
STATIC_ASSERT(offsetof(LEGAL_MATERIAL_PC, opacity) == 0x70);
STATIC_ASSERT(offsetof(LEGAL_MATERIAL_PC, texture_id) == 0x74);
STATIC_ASSERT(offsetof(LEGAL_MATERIAL_PC, shader_flags) == 0xb4);

i32 __cdecl LoadPermCreateLegalMtl(NUMTL **output, i32 texture_id) {
    NUMTL *material;
    LEGAL_MATERIAL_PC *view;
    if (*output != NULL)
        return 1;
    material = NuMtlCreate(1);
    if (material == NULL)
        return 0;
    view = (LEGAL_MATERIAL_PC *)material;
    view->diffuse.x = view->diffuse.y = view->diffuse.z = view->opacity = 1.0f;
    view->shader_flags = 0x1000;
    view->attributes = (view->attributes & 0xffffefe2u) | 0xe022;
    view->texture_id = (i16)texture_id;
    NuMtlUpdate(material);
    *output = material;
    return 1;
}
