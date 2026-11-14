#ifndef SAGA_LEGOAPI_ITEMS_BASE_ANIMPACKET_H
#define SAGA_LEGOAPI_ITEMS_BASE_ANIMPACKET_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

typedef struct animpacket_pc_s {
    f32 current_time;
    f32 previous_time;
    f32 blend_elapsed_pc;
    f32 blend_duration_pc;
    f32 blend_source_time;
    f32 blend_target_time;
    u32 unknown_18[6];
    u8 flags;
    u8 blending;
    i16 blend_source_animation;
    i16 blend_target_animation;
    i16 animation_index;
    i16 previous_animation;
    i16 requested_animation;
    u8 reversed_3c;
    u8 reversed_3d;
    u8 reversed_3e;
    u8 unknown_3f;
    u8 unknown_40;
    u8 unknown_41;
    i16 overlay_animation;
} ANIMPACKET_PC;

STATIC_ASSERT(offsetof(ANIMPACKET_PC, overlay_animation) == 0x42);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, current_time) == 0);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, blend_elapsed_pc) == 8);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, blend_duration_pc) == 0xc);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, blend_source_time) == 0x10);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, blend_target_time) == 0x14);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, blending) == 0x31);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, blend_source_animation) == 0x32);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, blend_target_animation) == 0x34);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, animation_index) == 0x36);

typedef struct minianimpacket_pc_s MINIANIMPACKET_PC;
#ifdef __cplusplus
extern "C" {
#endif

    void ResetMiniAnimPacket(MINIANIMPACKET_PC *, i32);
    STATIC_ASSERT(offsetof(ANIMPACKET_PC, previous_time) == 0x4);
    STATIC_ASSERT(offsetof(ANIMPACKET_PC, flags) == 0x30);
    STATIC_ASSERT(offsetof(ANIMPACKET_PC, previous_animation) == 0x38);
    STATIC_ASSERT(offsetof(ANIMPACKET_PC, requested_animation) == 0x3a);
    STATIC_ASSERT(offsetof(ANIMPACKET_PC, reversed_3c) == 0x3c);
    STATIC_ASSERT(offsetof(ANIMPACKET_PC, reversed_3d) == 0x3d);
    STATIC_ASSERT(offsetof(ANIMPACKET_PC, reversed_3e) == 0x3e);
    STATIC_ASSERT(offsetof(ANIMPACKET_PC, overlay_animation) == 0x42);
    f32 *AnimPlaying(ANIMPACKET_PC *, i32, i32, i32);
    i32 AnimBlendingFromTo(CHARACTER_MODEL_LAYERS_PC *, ANIMPACKET_PC *, i32, i32);
#ifdef __cplusplus
}
#endif

#ifndef TYPES_OFF_22
struct minianimpacket_pc_s {
    f32 current_time;           // 0x00
    f32 previous_time;          // 0x04
    u8 unknown_08[0xc];         // 0x08
    f32 blend_target_time;      // 0x14
    u8 flags;                   // 0x18
    u8 blending;                // 0x19
    u8 unknown_1a[4];           // 0x1a
    i16 current_animation_id;   // 0x1e
    i16 previous_animation_id;  // 0x20
    i16 requested_animation_id; // 0x22
}; // size 0x24

#endif /* TYPES_OFF_22 */
#endif
