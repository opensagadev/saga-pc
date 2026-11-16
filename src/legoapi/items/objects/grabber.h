#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/animpacket.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

/* The grab machine of the Death Star and Jabba levels (grabber.c); WORLDINFO.grabber. */
typedef struct grabber_s {                          // size 0x568
    NUMTX matrix;                                   // 0x000
    NUMTX grab_matrix;                              // 0x040
    u8 unknown_080[0x440 - 0x80];                   // 0x080
    NUMTX carried_matrix;                           // 0x440
    NUVEC initial_position;                         // 0x480
    NUVEC grab_position;                            // 0x48c
    NUVEC velocity;                                 // 0x498
    NUVEC target_velocity;                          // 0x4a4
    struct CHARACTER_MODEL_LAYERS *character_model; // 0x4b0
    ANIMPACKET animation;                           // 0x4b4
    u8 unknown_4f8[4];                              // 0x4f8
    nuhspecial_s special;                           // 0x4fc
    nuhspecial_s shadow_special;                    // 0x508
    struct gameobject_s *victim;                    // 0x514
    struct gizobstacle_s *action_switch;            // 0x518
    struct gizobstacle_s *direction_switches[4];    // 0x51c
    struct GIZMOBLOWUP *carried_blowup;             // 0x52c
    f32 scale;                                      // 0x530
    f32 speed;                                      // 0x534
    f32 radius;                                     // 0x538
    f32 stuck_timer;                                // 0x53c
    f32 shadow_height;                              // 0x540
    f32 carried_height_offset;                      // 0x544
    u16 shadow_z_angle;                             // 0x548
    u16 shadow_x_angle;                             // 0x54a
    f32 floor_height;                               // 0x54c
    f32 animation_timer;                            // 0x550
    f32 platform_contact_timer;                     // 0x554
    u8 state;                                       // 0x558

    union { // size 0x1
        u8 flags;

        struct {                   // size 0x1
            u8 flag_0 : 1;         // 0x559:0
            u8 lifted : 1;         // 0x559:1
            u8 switch_latched : 1; // 0x559:2
            u8 rotate : 1;         // 0x559:3
        } bits;
    }; // 0x559

    u16 yaw;           // 0x55a
    u16 target_yaw;    // 0x55c
    i16 character_id;  // 0x55e
    i16 platform_id;   // 0x560
    u8 move_xy;        // 0x562
    u8 invert_x;       // 0x563
    u8 unknown_564[4]; // 0x564
} GRABBER;

#ifdef __cplusplus
extern "C" {
#endif
    void Grabber_StoreProgress(WORLDINFO *world, LEVEL_PROGRESS *progress);

    extern i16 GrabMachineId;
    extern i16 GrabMagnetId;
    extern i16 RobotBaseId;
    void Grabber_Configure(WORLDINFO *, char *);
    void Grabber_Reset(WORLDINFO *world);
#ifdef __cplusplus
}
#endif

/* grabber_s.flags: the bits of its bit-field; rotate and shadow are the grabbers config keywords. */
#define GRABBER_DRAWN 0x1          /* APIDrawCharacterModel drew the grabber this frame */
#define GRABBER_LIFTED 0x2         /* the load has been lifted (pick-up frame, Grabber_Drop) */
#define GRABBER_SWITCH_LATCHED 0x4 /* released without a controller: wait for the action switch to reset */
#define GRABBER_ROTATE 0x8         /* "rotate" */
#define GRABBER_SHADOW 0x10        /* "shadow" (on unless "shadow off") */
