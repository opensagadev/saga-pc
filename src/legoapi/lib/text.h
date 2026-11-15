#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/legoapi_types.h"

#ifdef __cplusplus
extern "C" {
#endif
    void IntroText_Draw(f32);
    void Text_LoadFont(char *, VARIPTR *, VARIPTR *);
    i32 GetParentMenuID(void);
    i32 GetMenuID(void);
    extern char *txt_NULL;
    extern char *apitxt_ENGLISH;
    extern char *apitxt_FRENCH;
    extern char *apitxt_GERMAN;
    extern char *apitxt_ITALIAN;
    extern char *apitxt_SPANISH;
    extern char *apitxt_DANISH;
    extern i16 tNULL;
    extern i16 tENGLISH;
    extern i16 tFRENCH;
    extern i16 tGERMAN;
    extern i16 tITALIAN;
    extern i16 tSPANISH;
    extern i16 tDANISH;
    void *Text_IsFontLoaded(void);
    void Text_InitStringTable(i32 count, VARIPTR *buf, VARIPTR *buf_end);
    void Text_InitTable(TEXTENTRY *entry, i32 first, i32 last);
    void Text_InsertCommasIntoNumber(char *number, char *text, i32 length);
    void IntroText_SetTextID(i32 id);
    void Text_LoadAndFixUpStrings(char *path, u8 **buffer, char **table, i32 count);
    extern const char TextLoadPermAPIExtension[9];
    extern const char TextLoadPermTRCPath[19];
    extern i16 tALONGTIMEAGO;
    extern const char TextPlatformXbox[5];
    extern const char TextPlatformPC[3];
    extern const char TextPlatformPSP[4];
    extern const char TextPlatformPS3[4];
    extern char dat_0075d280[14];
    extern char dat_0075d270[13];
    extern char dat_0075d25c[18];
    extern char dat_0075d24c[13];
    extern char dat_0075d240[12];
    extern char dat_0075d22c[17];
    extern char dat_0075d21c[13];
    extern char dat_0075d20c[14];
    extern i32 textcrawlactive;
#ifdef __cplusplus
}
#endif
