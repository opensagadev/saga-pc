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

i32 FindNextBreak(u8 *text, i32 index) {
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

i32 FindNearestBreak(u8 *text, i32 index) {
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

void MessageBoxInitMtl(void) {
    MessageMtl = NuMtlCreate(1);
    MessageMtl->attribs.z_mode = 3;
    MessageMtl->attribs.filter_mode = 1;
    MessageMtl->attribs.unknown_2_1_2 = 2;
    MessageMtl->attribs.unknown_1_1_2 = 1;
    MessageMtl->attribs.unknown_1_4_8 = 1;
    MessageMtl->attribs.unknown_2_4 = 1;
    MessageMtl->attribs.alpha_mode = 1;
    NuMtlUpdate(MessageMtl);
    MessageMtlInit = 1;
}

void DrawMessageBox(i32 unused, f32 x, f32 y, f32 width, f32 height) {
    i32 box_width;
    i32 box_height;
    i32 screen_x;
    i32 screen_y;
    u32 colours[4];
    if (MessageMtlInit == 0) {
        MessageBoxInitMtl();
    }
    if (!NuRndrBeginScene(1)) {
        return;
    }
    box_width = (width * 10240.0 * NuQFntHalfScale);
    box_height = (height * 3584.0 * NuQFntHalfScale);
    screen_x = -(box_width / 2) - (i32)((x + 1.0f) * NuQFntHalfScale * -10240.0);
    screen_y = -(box_height / 2) - (i32)((y + 1.0f) * NuQFntHalfScale * -3584.0);
    NuRndrRect2di(screen_x, screen_y, box_width, box_height, 0x80808080, MessageMtl);
    colours[0] = 0x808080;
    colours[1] = 0x80808080;
    colours[2] = 0x808080;
    colours[3] = 0x80808080;
    NuRndrGradRectUV2di(screen_x - 30, screen_y, 60, box_height, 0.0f, 0.0f, 0.0f, 1.0f, colours, MessageMtl);
    colours[0] = 0x80808080;
    colours[1] = 0x808080;
    colours[2] = 0x80808080;
    colours[3] = 0x808080;
    NuRndrGradRectUV2di(screen_x + box_width, screen_y, 60, box_height, 1.0f, 0.0f, 1.0f, 1.0f, colours, MessageMtl);
    colours[0] = 0x808080;
    colours[1] = 0x808080;
    colours[2] = 0x80808080;
    colours[3] = 0x80808080;
    NuRndrGradRectUV2di(screen_x, screen_y - 7, box_width, 30, 0.0f, 0.0f, 1.0f, 0.0f, colours, MessageMtl);
    colours[0] = 0x80808080;
    colours[1] = 0x80808080;
    colours[2] = 0x808080;
    colours[3] = 0x808080;
    NuRndrGradRectUV2di(screen_x, screen_y + box_height - 15, box_width, 30, 0.0f, 1.0f, 1.0f, 1.0f, colours,
                        MessageMtl);
    NuRndrEndScene();
}

STATIC_ASSERT(offsetof(WORLDINFO, equivalent_groups) == 0x46b0);
STATIC_ASSERT(offsetof(WORLDINFO, equivalent_group_count) == 0x46b4);

STATIC_ASSERT(offsetof(GAMEOBJECT, target_velocity) == 0xf18);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef0) == 0xef0);
