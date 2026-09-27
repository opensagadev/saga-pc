#include "saga.h"

void __cdecl NuStrCat(char *str, const char *ext) {
    while (*str != '\0') {
        str++;
    }

    if (ext != NULL) {
        do {
            *str = *ext;
            str++;
        } while (*(ext++) != '\0');
    }
}

i32 __cdecl NuStrNCat(char *str, const char *ext, i32 n) {
    i32 copied;

    while (*str != '\0') {
        str++;
    }
    copied = 0;

    if (ext != NULL) {
        do {
            if (n == 0) {
                break;
            }
            *str = *ext;
            str++;
            copied++;
            n--;
        } while (*(ext++) != '\0');
    }
    return copied;
}

i32 __cdecl NuStrCpy(char *dst, const char *src) {
    char *dst_start = dst;

    if (src != NULL) {
        while (*src != '\0') {
            *dst = *src;
            dst++;
            src++;
        }
    }

    *dst = '\0';
    return (i32)(dst - dst_start);
}

i32 __cdecl NuStrNCpy(char *dst, const char *src, i32 n) {
    i32 count = 0;

    if (src == NULL) {
        *dst = '\0';
        return 0;
    }

    do {
        n--;
        if (n <= 0) {
            *dst = '\0';
            count++;
            break;
        }

        *dst = *src;
        dst++;
        count++;
    } while (*(src++) != '\0');

    return count - 1;
}

char *__cdecl NuStrChr(char *src, char c) {
    while (*src != '\0') {
        if (*src == c) {
            return src;
        }
        src++;
    }
    return NULL;
}

char *__cdecl NuStrRChr(char *src, char c) {
    char *ptr = src;

    while (*ptr != '\0') {
        ptr++;
    }

    while (ptr >= src) {
        if (*ptr == c) {
            return ptr;
        }
        ptr--;
    }
    return NULL;
}

i32 __cdecl NuStrLen(const char *str) {
    i32 count = 0;

    while (*str != '\0') {
        count++;
        str++;
    }
    return count;
}

i32 __cdecl NuStrLenU(u8 *str) {
    i32 offset = 0;
    i32 characters = 0;

    while (str[offset] != 0) {
        characters++;
        offset++;
        while (str[offset] >= 0x80 && str[offset] <= 0xbf) {
            offset++;
        }
    }
    return characters;
}

i32 __cdecl NuStrCmp(const char *a, const char *b) {
    char a_cursor;
    char b_cursor;

    if (a == NULL) {
        return -1;
    }
    if (b == NULL) {
        return 1;
    }

    do {
        a_cursor = *a;
        b_cursor = *b;
        if (a_cursor > b_cursor) {
            return 1;
        }
        if (a_cursor < b_cursor) {
            return -1;
        }
        a++;
        b++;
    } while (a_cursor != '\0' && b_cursor != '\0');

    return 0;
}

i32 __cdecl NuStrNCmp(const char *a, const char *b, i32 n) {
    char a_cursor;
    char b_cursor;

    if (a == NULL) {
        return -1;
    }
    if (b == NULL) {
        return 1;
    }
    if (n == 0) {
        return 0;
    }
    if (n == -1) {
        n = NuStrLen(a);
    } else if (n == -2) {
        n = NuStrLen(b);
    }

    do {
        a_cursor = *a;
        b_cursor = *b;
        if (a_cursor > b_cursor) {
            return 1;
        }
        if (a_cursor < b_cursor) {
            return -1;
        }
        a++;
        b++;
        n--;
    } while (a_cursor != '\0' && b_cursor != '\0' && n != 0);

    return 0;
}

i32 __cdecl NuStrICmp(const char *a, const char *b) {
    char a_cursor;
    char b_cursor;
    u8 a_upper;
    u8 b_upper;

    if (a == NULL) {
        return -1;
    }
    if (b == NULL) {
        return 1;
    }

    do {
        a_upper = (u8)*a;
        if (a_upper >= 'a' && a_upper <= 'z') {
            a_upper -= 0x20;
        } else if (a_upper >= 0xe0) {
            a_upper -= 0x20;
        }
        a_cursor = (char)a_upper;

        b_upper = (u8)*b;
        if (b_upper >= 'a' && b_upper <= 'z') {
            b_upper -= 0x20;
        } else if (b_upper >= 0xe0) {
            b_upper -= 0x20;
        }
        b_cursor = (char)b_upper;

        if (a_cursor > b_cursor) {
            return 1;
        }
        if (a_cursor < b_cursor) {
            return -1;
        }
        a++;
        b++;
    } while (a_cursor != '\0' && b_cursor != '\0');

    return 0;
}

void __cdecl NuStrUpr(char *dst, const char *src) {
    u8 value;

    for (; *src != '\0'; src++) {
        value = (u8)*src;
        if (value >= 'a' && value <= 'z') {
            value -= 0x20;
        } else if (value >= 0xe0) {
            value -= 0x20;
        }
        *dst = (char)value;
        dst++;
    }
    *dst = *src;
}

void __cdecl NuStrLwr(char *dst, const char *src) {
    u8 value;

    while (*src != '\0') {
        value = (u8)*src;
        if (value >= 'A' && value <= 'Z') {
            value += 0x20;
        } else if (value > 0xbf && value < 0xe0) {
            value += 0x20;
        }
        *dst = (char)value;
        dst++;
        src++;
    }
    *dst = *src;
}

i32 __cdecl NuStrNICmp(const char *a, const char *b, i32 n) {
    char a_cursor;
    char b_cursor;
    u8 a_upper;
    u8 b_upper;

    if (a == NULL) {
        return -1;
    }
    if (b == NULL) {
        return 1;
    }
    if (n == 0) {
        return 0;
    }
    if (n == -1) {
        n = NuStrLen(a);
    } else if (n == -2) {
        n = NuStrLen(b);
    }

    do {
        a_upper = (u8)*a;
        if (a_upper >= 'a' && a_upper <= 'z') {
            a_upper -= 0x20;
        } else if (a_upper >= 0xe0) {
            a_upper -= 0x20;
        }
        a_cursor = (char)a_upper;

        b_upper = (u8)*b;
        if (b_upper >= 'a' && b_upper <= 'z') {
            b_upper -= 0x20;
        } else if (b_upper >= 0xe0) {
            b_upper -= 0x20;
        }
        b_cursor = (char)b_upper;

        if (a_cursor > b_cursor) {
            return 1;
        }
        if (a_cursor < b_cursor) {
            return -1;
        }
        a++;
        b++;
        n--;
    } while (a_cursor != '\0' && b_cursor != '\0' && n != 0);

    return 0;
}

i32 __cdecl NuHexStringToI(char *string) {
    i32 value = 0;
    u8 upper;

    for (; *string != '\0'; string++) {
        value <<= 4;
        upper = (u8)*string;
        if (upper >= 'a' && upper <= 'z') {
            upper -= 0x20;
        } else if (upper >= 0xe0) {
            upper -= 0x20;
        }

        if (upper <= '9' && upper >= '0') {
            value |= upper - 0x30;
        } else if (upper <= 'F' && upper >= 'A') {
            value |= upper - 0x37;
        } else {
            return 0;
        }
    }
    return value;
}

u8 *__cdecl NuUnicodeCharFromUTF8(u16 *dst, u8 *src) {
    u16 c0 = *src++;

    if (c0 <= 0x7f) {
        *dst = c0;
    } else if ((c0 & 0xe0) == 0xc0) {
        u16 c1 = *src++;
        *dst = ((c0 & 0x1f) << 6) | (c1 & 0x3f);
    } else if ((c0 & 0xf0) == 0xe0) {
        u16 c1 = *src++;
        u16 c2 = *src++;
        *dst = (c0 << 12) | ((c1 & 0x3f) << 6) | (c2 & 0x3f);
    }
    return src;
}

u8 *__cdecl NuUTF8CharFromUnicode(u8 *dst, u16 character) {
    if (character <= 0x7f) {
        *dst++ = (u8)character;
    } else if (character <= 0x7ff) {
        *dst++ = (u8)((character >> 6) | 0xc0);
        *dst++ = (u8)((character & 0x3f) | 0x80);
    } else {
        *dst++ = (u8)((character >> 12) | 0xe0);
        *dst++ = (u8)(((character >> 6) & 0x3f) | 0x80);
        *dst++ = (u8)((character & 0x3f) | 0x80);
    }
    return dst;
}

void __cdecl NuUnicodeToUTF8(u8 *dst, u16 *src) {
    if (src == NULL) {
        return;
    }
    if (dst == NULL) {
        return;
    }

    *dst = '\0';
    for (; *src != 0; src++) {
        if (*src < 0x80) {
            *dst++ = (u8)*src;
        } else if (*src < 0x800) {
            *dst++ = (u8)((*src >> 6 & 0x1f) | 0xc0);
            *dst++ = (u8)((*src & 0x3f) | 0x80);
        } else {
            *dst++ = (u8)((*src >> 12 & 0x1f) | 0xe0);
            *dst++ = (u8)((*src >> 6 & 0x3f) | 0x80);
            *dst++ = (u8)((*src & 0x3f) | 0x80);
        }
    }
    *dst = '\0';
}

void __cdecl NuUTF8ToUnicode(u16 *dst, u8 *src) {
    if (src == NULL) {
        return;
    }
    if (dst == NULL) {
        return;
    }

    *dst = 0;
    while (*src != 0) {
        if (*src < 0x80) {
            *dst = *src;
            src++;
        } else if (*src < 0xe0) {
            *dst = (*src & 0x1f) << 6;
            src++;
            *dst |= *src & 0x3f;
            src++;
        } else if (*src < 0xf0) {
            *dst = (*src & 0x0f) << 12;
            src++;
            *dst |= (*src & 0x3f) << 6;
            src++;
            *dst |= *src & 0x3f;
            src++;
        } else {
            *dst = '?';
            src++;
        }
        dst++;
    }
    *dst = 0;
}

void __cdecl NuAsciiToUnicode(u16 *dst, char *src) {
    i32 pos = 0;

    *dst = 0;
    if (src == NULL) {
        return;
    }
    if (dst == NULL) {
        return;
    }

    for (; src[pos] != '\0'; pos++) {
        dst[pos] = (u8)src[pos];
    }
    dst[pos] = '\0';
}
