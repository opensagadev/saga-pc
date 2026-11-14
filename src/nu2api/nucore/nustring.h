#ifndef SAGA_NU2API_NUCORE_NUSTRING_H
#define SAGA_NU2API_NUCORE_NUSTRING_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

// NOLINTBEGIN(google-runtime-int): These aliases must spell their compiler-provided base types.
typedef u16 NUWCHAR;

// NOLINTEND(google-runtime-int)

typedef struct NUSTRING {
    char *id;

    union {
        u16 *str_utf16;
        u8 *str_utf8;
    };
} NUSTRING;

STATIC_ASSERT(sizeof(NUSTRING) == 8);
STATIC_ASSERT(offsetof(NUSTRING, str_utf16) == 4);

typedef struct NUSTRINGBANK {
    NUSTRING *strings;
    i32 string_count;
    i32 max_strings;
} NUSTRINGBANK;

STATIC_ASSERT(sizeof(NUSTRINGBANK) == 12);
STATIC_ASSERT(offsetof(NUSTRINGBANK, string_count) == 4);
STATIC_ASSERT(offsetof(NUSTRINGBANK, max_strings) == 8);
#ifdef __cplusplus
extern "C" {
#endif
    i32 NuStrFindPosU(u8 *, i32);
    void NuUnicodeToUTF8(u8 *output, u16 *input);
    void NuUTF8ToUnicode(u16 *output, u8 *input);
    void NuAsciiToUnicode(u16 *output, char *input);
    i32 NuIsAlNum(char character);
    f32 NuAToF(char *text);
    i32 NuAToI(char *text);
    void NuUnicodeToAscii(char *out, u16 *text);
    i32 NuStringTableLoadCSV(char *, VARIPTR *, VARIPTR, char *, char *);
    i32 NuStringTableLoadTXT(char *, VARIPTR *, VARIPTR);
    u16 *NuStringTableGetByName(char *);
    void NuStringTableSetBank(i32);
    void NuStringTableSetFormat(i32);
    i32 NuStringTableGetFormat(void);
    void NuStrLwrW(u16 *, const u16 *);
    void NuStringTableUnload(void);
    u16 *NuStringTableGetById(i32);
    i32 NuStringTableGetIdByName(char *);
    i32 NuStringFilterBadWordsW(u16 *, u16 *, u16 *);
    i32 NuStringFilterBadWords(u8 *, u8 *, u8 *);
    i32 NuStrLen(const char *text);
    i32 NuStrICmp(const char *a, const char *b);
    i32 NuStrICmpW(NUWCHAR *a, NUWCHAR *b);
    char *NuStrIStr(char *text, char *substring);
    i32 NuStrNICmp(const char *a, const char *b, i32 n);
    void NuStrUpr(char *dst, const char *src);
    void NuStrLwr(char *dst, const char *src);
    i32 NuHexStringToI(char *string);
    u8 *NuUnicodeCharFromUTF8(u16 *out, u8 *text);
    u8 *NuUTF8CharFromUnicode(u8 *out, u16 codepoint);
    void NuStrCat(char *str, const char *ext);
    i32 NuStrNCat(char *str, const char *ext, i32 n);
    i32 NuStrCpy(char *dst, const char *src);
    i32 NuStrNCpy(char *dst, const char *src, i32 n);
    char *NuStrChr(char *src, char c);
    char *NuStrRChr(char *src, char c);
    i32 NuStrLenU(u8 *str);
    i32 NuStrCmp(const char *a, const char *b);
    i32 NuStrNCmp(const char *a, const char *b, i32 n);

    u8 NuToUpper(u8);
    char *NuIToA(i32, char *, i32);
    u16 NuToUpperW(u16 c);
#ifdef __cplusplus
}
#endif

#endif
