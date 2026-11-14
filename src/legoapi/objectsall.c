#include "saga.h"

/* Do not split a "~~" pair, and keep a single '~' with the text after it. */
#define KEEP_TILDE_PAIR(text, index)                                                                                   \
    if ((text)[index] == '~') {                                                                                        \
        if ((index) > 0 && (text)[(index) - 1] == '~')                                                                 \
            --(index);                                                                                                 \
    } else if ((index) > 0 && (text)[(index) - 1] == '~' && (index) > 1 && (text)[(index) - 2] != '~')                 \
    --(index)

#define IS_BREAK_POINT(text, index)                                                                                    \
    (((text)[index] == ' ' && (text)[(index) + 1] != '!' && (text)[(index) + 1] != '?' &&                              \
      (text)[(index) + 1] != ';' && (text)[(index) + 1] != ':') ||                                                     \
     ((text)[index] == '.' && (text)[(index) + 1] != '.') || (text)[index] == '-' || (text)[index] == ',')

i32 __cdecl FindNextBreak(u8 *text, i32 index) {
    i32 japanese = 0;
    i32 ascii_start;
    i32 length;
    i32 count;
    if (NuLanguageGet() == 0) {
        japanese = 1;
    }
    count = 0;
    length = NuStrLen((char *)text);
    while (text[index] >= 0x80 && text[index] <= 0xbf) {
        --index;
    }
    if (IS_BREAK_POINT(text, index)) {
        return index;
    }
    ascii_start = text[index] <= 0x7f;
    for (;;) {
        ++count;
        if ((japanese && count > 10 && !ascii_start) || count > 20) {
            break;
        }
        if (++index >= length) {
            return length;
        }
        while (text[index] >= 0x80 && text[index] <= 0xbf) {
            ++index;
        }
        if (IS_BREAK_POINT(text, index)) {
            return index;
        }
        if (japanese && ((text[index] <= 0x7f && !ascii_start) || (text[index] >= 0x80 && ascii_start))) {
            break;
        }
    }
    KEEP_TILDE_PAIR(text, index);
    return index;
}

i32 __cdecl FindNearestBreak(u8 *text, i32 index) {
    i32 japanese = 0;
    i32 ascii_start;
    i32 length;
    i32 forward;
    i32 backward;
    i32 count;
    if (NuLanguageGet() == 0) {
        japanese = 1;
    }
    while (text[index] >= 0x80 && text[index] <= 0xbf) {
        --index;
    }
    if (IS_BREAK_POINT(text, index)) {
        return index;
    }
    ascii_start = text[index] <= 0x7f;
    forward = backward = index;
    count = 0;
    length = NuStrLen((char *)text);
    for (;;) {
        ++count;
        if ((japanese && count > 10 && !ascii_start) || count > 20) {
            break;
        }
        if (forward < length - 1) {
            ++forward;
        }
        while (text[forward] >= 0x80 && text[forward] <= 0xbf) {
            ++forward;
        }
        if (backward > 0) {
            --backward;
        }
        while (text[backward] >= 0x80 && text[backward] <= 0xbf) {
            --backward;
        }
        if (IS_BREAK_POINT(text, forward)) {
            return forward;
        }
        if (japanese && ((text[forward] <= 0x7f && !ascii_start) || (text[forward] >= 0x80 && ascii_start))) {
            KEEP_TILDE_PAIR(text, forward);
            return forward;
        }
        if (IS_BREAK_POINT(text, backward)) {
            return backward;
        }
        if (japanese && ((text[backward] <= 0x7f && !ascii_start) || (text[backward] >= 0x80 && ascii_start))) {
            ++backward;
            KEEP_TILDE_PAIR(text, backward);
            return backward;
        }
    }
    KEEP_TILDE_PAIR(text, index);
    return index;
}

typedef struct EQUIVALENT_GROUP_PC {
    i16 object_count;           // 0x0
    i16 byte_size;              // 0x2
    NUSPECIALHANDLE objects[1]; // 0x4
} EQUIVALENT_GROUP_PC;          // size 0x10

STATIC_ASSERT(offsetof(WORLDINFO, equivalent_groups) == 0x46b0);
STATIC_ASSERT(offsetof(WORLDINFO, equivalent_group_count) == 0x46b4);

STATIC_ASSERT(offsetof(GAMEOBJECT, target_velocity) == 0xf18);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef0) == 0xef0);
