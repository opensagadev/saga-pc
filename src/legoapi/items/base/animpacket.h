#pragma once

#include "nu2api/nucore/common.h"

struct CHARACTER_MODEL_LAYERS;

typedef struct animpacket_s {   // size 0x44
    f32 current_time;           // 0x00
    f32 previous_time;          // 0x04
    f32 blend_elapsed;          // 0x08
    f32 blend_duration;         // 0x0c
    f32 blend_source_time;      // 0x10
    f32 blend_target_time;      // 0x14
    u32 unknown_18[6];          // 0x18
    u8 flags;                   // 0x30
    u8 blending;                // 0x31
    i16 blend_source_animation; // 0x32
    i16 blend_target_animation; // 0x34
    i16 animation_index;        // 0x36
    i16 previous_animation;     // 0x38
    i16 requested_animation;    // 0x3a
    u8 reversed_3c;             // 0x3c
    u8 reversed_3d;             // 0x3d
    u8 reversed_3e;             // 0x3e
    u8 unknown_3f;              // 0x3f
    u8 unknown_40;              // 0x40
    u8 unknown_41;              // 0x41
    i16 overlay_animation;      // 0x42
} ANIMPACKET;

typedef struct minianimpacket_s MINIANIMPACKET;
#ifdef __cplusplus
extern "C" {
#endif

    void ResetMiniAnimPacket(MINIANIMPACKET *, i32);
    f32 *AnimPlaying(ANIMPACKET *, i32, i32, i32);
    i32 AnimBlendingFromTo(struct CHARACTER_MODEL_LAYERS *, ANIMPACKET *, i32, i32);
#ifdef __cplusplus
}
#endif
