#ifndef SAGA_LEGOAPI_GIZMOS_OBJECT_HATMACHINE_H
#define SAGA_LEGOAPI_GIZMOS_OBJECT_HATMACHINE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

typedef struct HATMACHINE_PC HATMACHINE_PC;
#ifdef __cplusplus
extern "C" {
#endif
    void Hat_GetAbsTargetPos(HATMACHINE_PC *, NUVEC *);
    void HatMachines_InitTerrain(WORLDINFO *);
    HATMACHINE_PC *HatMachine_FindNearest(WORLDINFO *, NUVEC *, GAMEOBJECT *, f32 *);
    i32 HatMachine_BeingUsed(HATMACHINE_PC *);
    extern i32 hatmachine_gizmotype_id;

    /* Within the existing extern C declaration block; current HATMACHINE_PC is already forward-declared. */
    void HatMachine_MoveCode(WORLDINFO *, GAMEOBJECT *, i32);
    struct addgizmotype_pc_s *HatMachine_RegisterGizmo(i32);
#ifdef __cplusplus
}
#endif

#ifndef TYPES_OFF_18
struct HATMACHINE_PC {
    NUMTX matrix;           // 0x00
    char name[16];          // 0x40
    NUVEC position;         // 0x50
    u16 yaw;                // 0x5c
    u8 configured_hat;      // 0x5e
    u8 displayed_hat;       // 0x5f
    i8 animation_state;     // 0x60
    u8 model_letter;        // 0x61
    u8 model_special_index; // 0x62

    union {       // 0x63
        u8 flags; // 0x63

        struct {                        // 0x63
            u8 state_bit0 : 1;          // 0x63:0
            u8 state_bit1 : 1;          // 0x63:1
            u8 visible : 1;             // 0x63:2
            u8 enabled : 1;             // 0x63:3
            u8 reserved_state_bits : 4; // 0x63:4
        };
    };

    NUVEC player_position; // 0x64
    NUVEC target_offset;   // 0x70
    u16 terrain_pitch;     // 0x7c
    u16 terrain_roll;      // 0x7e
    i16 platform_id;       // 0x80
    u16 reserved_82;       // 0x82
    f32 hat_delay;         // 0x84
    f32 state_duration;    // 0x88
    f32 state_elapsed;     // 0x8c
    f32 scale;             // 0x90
    f32 animation_time;    // 0x94
    f32 idle_bounce_timer; // 0x98
    u8 reserved_9c[4];     // 0x9c
}; // size 0xa0

#endif /* TYPES_OFF_18 */
#endif
