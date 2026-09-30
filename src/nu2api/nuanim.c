#include "saga.h"

typedef struct NuAnimDataPrefixView {
    union {
        i32 magic;
        f32 legacy_end_frame;
    } first_word;
    u8 unknown_04[6];
    u16 first_frame;
    u16 unknown_0c;
    u16 frame_count;
    u8 unknown_10[6];
    u16 declared_end_frame;
} NuAnimDataPrefixView;

STATIC_ASSERT(offsetof(NuAnimDataPrefixView, first_frame) == 0xa);
STATIC_ASSERT(offsetof(NuAnimDataPrefixView, unknown_0c) == 0xc);
STATIC_ASSERT(offsetof(NuAnimDataPrefixView, frame_count) == 0xe);
STATIC_ASSERT(offsetof(NuAnimDataPrefixView, declared_end_frame) == 0x16);
STATIC_ASSERT(sizeof(NuAnimDataPrefixView) == 0x18);

f32 __cdecl NuAnimEndFrameOld(void *animation_data) {
    NuAnimDataPrefixView *animation = (NuAnimDataPrefixView *)animation_data;

    if (animation->first_word.magic < 0x414e4934 || animation->first_word.magic > 0x414e4935) {
        return animation->first_word.legacy_end_frame;
    }
    if (animation->declared_end_frame != 0) {
        return (f32)animation->declared_end_frame;
    }
    return (f32)((i32)animation->frame_count + (i32)animation->first_frame);
}
