#ifndef SAGA_EXTERNALS_DSOUND_H
#define SAGA_EXTERNALS_DSOUND_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

STATIC_ASSERT(offsetof(IDirectSound8Vtbl, duplicate_sound_buffer) == 0x14);
STATIC_ASSERT(offsetof(IDirectSound8Vtbl, create_sound_buffer) == 0xc);
STATIC_ASSERT(offsetof(IDirectSoundBufferVtbl, get_current_position) == 0x10);
STATIC_ASSERT(offsetof(IDirectSoundBufferVtbl, lock) == 0x2c);
STATIC_ASSERT(offsetof(IDirectSoundBufferVtbl, set_current_position) == 0x34);
STATIC_ASSERT(offsetof(IDirectSoundBufferVtbl, set_format) == 0x38);
STATIC_ASSERT(offsetof(IDirectSoundBufferVtbl, unlock) == 0x4c);
STATIC_ASSERT(offsetof(IDirectSoundBufferVtbl, restore) == 0x50);
STATIC_ASSERT(offsetof(IDirectSoundBufferVtbl, get_status) == 0x24);
STATIC_ASSERT(offsetof(IDirectSoundBufferVtbl, play) == 0x30);
STATIC_ASSERT(offsetof(IDirectSoundBufferVtbl, set_volume) == 0x3c);
STATIC_ASSERT(offsetof(IDirectSoundBufferVtbl, set_frequency) == 0x44);
STATIC_ASSERT(offsetof(IDirectSoundBufferVtbl, stop) == 0x48);
STATIC_ASSERT(offsetof(IDirectSound3DBufferVtbl, set_max_distance) == 0x40);
STATIC_ASSERT(offsetof(IDirectSound3DBufferVtbl, set_min_distance) == 0x44);
STATIC_ASSERT(offsetof(IDirectSound3DBufferVtbl, set_mode) == 0x48);
STATIC_ASSERT(offsetof(IDirectSound3DBufferVtbl, set_position) == 0x4c);
STATIC_ASSERT(sizeof(PC_DS_BUFFER_DESC_VIEW) == 0x24);
STATIC_ASSERT(offsetof(PC_DS_BUFFER_DESC_VIEW, format) == 0x10);
STATIC_ASSERT(offsetof(PC_DS_BUFFER_DESC_VIEW, algorithm) == 0x14);
STATIC_ASSERT(offsetof(IDirectSoundBufferVtbl, get_volume) == 0x18);
#ifdef __cplusplus
extern "C" {
#endif
    STATIC_ASSERT(sizeof(PC_DS_CAPS_VIEW) == 0x60);
    STATIC_ASSERT(offsetof(IDirectSound8Vtbl, set_cooperative_level) == 0x18);
    STATIC_ASSERT(offsetof(IDirectSound8Vtbl, get_speaker_config) == 0x20);
#ifdef __cplusplus
}
#endif

#endif
