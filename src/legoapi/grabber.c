#include "saga.h"

typedef struct grabber_progress_source_pc_s {
    u8 unknown_0000[0x484];
    f32 stored_tfactor;
    u8 unknown_0488[4];
    f32 stored_frame;
    u8 unknown_0490[4];
    f32 stored_parameter;
} GRABBER_PROGRESS_SOURCE_PC;
STATIC_ASSERT(offsetof(GRABBER_PROGRESS_SOURCE_PC, stored_tfactor) == 0x484);
STATIC_ASSERT(offsetof(GRABBER_PROGRESS_SOURCE_PC, stored_frame) == 0x48c);
STATIC_ASSERT(offsetof(GRABBER_PROGRESS_SOURCE_PC, stored_parameter) == 0x494);

typedef struct world_grabber_progress_pc_s {
    u8 unknown_0000[0x46ec];
    GRABBER_PROGRESS_SOURCE_PC *grabber;
} WORLD_GRABBER_PROGRESS_PC;
STATIC_ASSERT(offsetof(WORLD_GRABBER_PROGRESS_PC, grabber) == 0x46ec);

typedef struct level_grabber_progress_pc_s {
    u8 unknown_0000[0x2810];
    f32 stored_frame;
    f32 stored_tfactor;
    f32 stored_parameter;
} LEVEL_GRABBER_PROGRESS_PC;
STATIC_ASSERT(offsetof(LEVEL_GRABBER_PROGRESS_PC, stored_frame) == 0x2810);
STATIC_ASSERT(offsetof(LEVEL_GRABBER_PROGRESS_PC, stored_tfactor) == 0x2814);
STATIC_ASSERT(offsetof(LEVEL_GRABBER_PROGRESS_PC, stored_parameter) == 0x2818);

void __cdecl Grabber_StoreProgress(WORLDINFO *world, LEVEL_PROGRESS *progress) {
    if (world != NULL && progress != NULL) {
        GRABBER_PROGRESS_SOURCE_PC *grabber = ((WORLD_GRABBER_PROGRESS_PC *)world)->grabber;
        LEVEL_GRABBER_PROGRESS_PC *saved = (LEVEL_GRABBER_PROGRESS_PC *)progress;
        if (grabber != NULL) {
            saved->stored_frame = grabber->stored_frame;
            saved->stored_tfactor = grabber->stored_tfactor;
            saved->stored_parameter = grabber->stored_parameter;
        } else {
            saved->stored_tfactor = 2000000.0f;
        }
    }
}
