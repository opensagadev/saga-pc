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
