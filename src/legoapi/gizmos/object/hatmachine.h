#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

typedef struct HATMACHINE HATMACHINE;

struct HATMACHINE {         // size 0xa0
    NUMTX matrix;           // 0x00
    char name[16];          // 0x40
    NUVEC position;         // 0x50
    u16 yaw;                // 0x5c
    u8 configured_hat;      // 0x5e
    u8 displayed_hat;       // 0x5f
    i8 animation_state;     // 0x60
    u8 model_letter;        // 0x61
    i8 model_special_index; // 0x62

    union { // size 0x1
        u8 flags;

        struct {                        // size 0x1
            u8 state_bit0 : 1;          // 0x63:0
            u8 state_bit1 : 1;          // 0x63:1
            u8 visible : 1;             // 0x63:2
            u8 enabled : 1;             // 0x63:3
            u8 reserved_state_bits : 4; // 0x63:4
        };
    }; // 0x63

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
};

typedef struct HATMACHINE_SYS { // size 0x10
    i32 count;                  // 0x0
    i32 reserved_04;            // 0x4
    i32 reserved_08;            // 0x8
    HATMACHINE *machines;       // 0xc
} HATMACHINE_SYS;

/* HATMACHINE.flags. */
#define HATMACHINE_FLAG_IN_USE 0x1 /* a character works it; cleared when its animation ends */
#define HATMACHINE_FLAG_OUTPUT 0x2 /* the gizmo output (HatMachine_GetOutput) */
#define HATMACHINE_FLAG_VISIBLE 0x4
#define HATMACHINE_FLAG_ENABLED 0x8
#define HATMACHINE_FLAG_HIDE_TARGET 0x20 /* the level file's hidden byte: no target indicator */

/* HATMACHINE.animation_state: the machine's animation, one step per state_duration (the lid and arm specials
 * the draw moves in each); 0 while the machine is idle. */
#define HATMACHINE_ANIM_IDLE 0
#define HATMACHINE_ANIM_LID_OPENING 1
#define HATMACHINE_ANIM_ARMS_OUT 2
#define HATMACHINE_ANIM_HAT_DOWN 3
#define HATMACHINE_ANIM_ARMS_UP 4
#define HATMACHINE_ANIM_ARMS_IN 5
#define HATMACHINE_ANIM_LID_CLOSING 6

#ifdef __cplusplus
extern "C" {
#endif
    void Hat_GetAbsTargetPos(HATMACHINE *, NUVEC *);
    void HatMachines_InitTerrain(WORLDINFO *);
    HATMACHINE *HatMachine_FindNearest(WORLDINFO *, NUVEC *, GAMEOBJECT *, f32 *);
    i32 HatMachine_BeingUsed(HATMACHINE *);
    extern i32 hatmachine_gizmotype_id;

    /* Within the existing extern C declaration block; current HATMACHINE is already forward-declared. */
    void HatMachine_MoveCode(WORLDINFO *, GAMEOBJECT *, i32);
    struct addgizmotype_s *HatMachine_RegisterGizmo(i32);
#ifdef __cplusplus
}
#endif
