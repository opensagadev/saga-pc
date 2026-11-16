#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nuqfnt.h"

/* The text crawl an area opens with (TextCrawlLsw): the text ids of its heading and bonus lines. */
typedef struct textcrawl_s {   // size 0x10
    i16 *heading_text;         // 0x0
    i16 *vehicle_bonus_text;   // 0x4
    i16 *character_bonus_text; // 0x8
    u8 paragraph_count;        // 0xc
    i8 area;                   // 0xd
    u8 unknown_0e[2];          // 0xe
} TEXTCRAWL;

#ifdef __cplusplus
extern "C" {
#endif
    void Text3DStringEncode(char *source, u16 *destination);
    void Text3DEx(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment, u8 red, u8 green, u8 blue,
                  u8 alpha);
    void Text3D(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment, u8 red, u8 green, u8 blue);
    extern f32 text3d_height;
    extern f32 text3d_width;
    void SetQFont2D(void);

    void Text3DEx2(char *, f32, f32, f32, f32, f32, f32, u32, u32, u32, u32, u8);
    void MenuText3DEx(char *, f32, f32, f32, f32, f32, f32, u32, u32, u32, u32, i32);

    void SmartTextEx(char *, f32, f32, f32, f32, f32, f32, u32, u8, u8, u8, f32, i32, void *, i32, i32);
    void SmartTextEx2(char *, f32, f32, f32, f32, f32, f32, u32, u8, u8, u8, f32, i32, void *, i32, u32);
    void MenuSmartTextEx(char *, f32, f32, f32, f32, f32, f32, u32, u8, u8, u8, f32, i32, void *, i32, i32);
    extern i32 MenuStopDraw;
    extern void (*Text_GameSetLanguageFn)(i32);
    extern const char Text_TimeHMSC[18];
    extern const char Text_TimeHMS[13];
    extern const char Text_TimeMSC[13];
    void Text_SetLanguage(i32 language);
    void Text_SetLanguage_Game(i32 language);
    void Text_MakeTime(f32 time, i32 show_hours, i32 show_minutes, i32 show_centiseconds, char *text);
    extern i16 tUNKNOWN;
    void Text_InitLanguageList(LANGUAGEDATA *language_list);
    char *Text_GetLanguagePath(i32 language);
    i32 Text_GetMaxOverallStrings(void);
    void Text_MakeScore(u32 score, char *text);
    void TextRegisterButtonMapFn(void (*fn)(char *, char *));
    void SmartTextSetFWNMode(i32 mode);
    void SmartTextSetFont(VUFNT *font);
    void SetGameFont(VUFNT *font);
    void Set3DGameFont(VUFNT *font);
    void SmartTextSetWidescreen(f32 font_scale_x, f32 coordinate_scale);
    void SmartTextGetWidescreen(f32 *font_scale_x, f32 *coordinate_scale);
    void TextCrawl_Init(TEXTCRAWL *, i32, i32);
    void TextCrawl_Draw(f32, i32, f32, char *);

    /* Native C ABI. */
    void SmartText(char *, f32, f32, f32, f32, f32, f32, i32, i32, i32, i32, f32, i32);
    i32 Text_ExpandButtonString(char *, char *);
#ifdef __cplusplus
}
#endif
