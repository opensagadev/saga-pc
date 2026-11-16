#pragma once

#include "nu2api/nucore/common.h"

// NOLINTBEGIN(google-runtime-int): These aliases must spell their compiler-provided base types.
typedef u16 NUWCHAR;

// NOLINTEND(google-runtime-int)

typedef struct NUSTRING { // size 0x8
    char *id;             // 0x0

    union { // size 0x4
        u16 *str_utf16;
        u8 *str_utf8;
    }; // 0x4
} NUSTRING;

typedef struct NUSTRINGBANK { // size 0xc
    NUSTRING *strings;        // 0x0
    i32 string_count;         // 0x4
    i32 max_strings;          // 0x8
} NUSTRINGBANK;

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
    i32 NuStrICmpWC(char *pattern, char *name, char *wildcards);
    i32 NuStrLenW(const NUWCHAR *str);
    void NuStrFixExt(char *, char *, char *, i32);
    void NuStrFixExtPlatform(char *dst, char *src, char *ext, i32 dst_size, char *platform_string);
#ifdef __cplusplus
}
#endif

#define NU_WIDE_LENGTH(text, length)                                                                                                       \
    do {                                                                                                                                   \
        const u16 *nu_cursor = (const u16 *)(text);                                                                                        \
        (length) = 0;                                                                                                                      \
        while (*nu_cursor++ != 0) {                                                                                                        \
            ++(length);                                                                                                                    \
        }                                                                                                                                  \
    } while (0)
