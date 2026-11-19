#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nuspline.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif

/* GameCameraMakeMiniCut3 flags: the DynamicCameraCut AI action's keywords that set them. */
#define MINICUT_RANGE 0x1                 /* range= */
#define MINICUT_ROTX 0x2                  /* rotx= */
#define MINICUT_ROTY 0x4                  /* roty= */
#define MINICUT_ROTZ 0x8                  /* rotz= */
#define MINICUT_CONT_ROTX 0x10            /* cont_rotx= */
#define MINICUT_CONT_ROTY 0x20            /* cont_roty= */
#define MINICUT_CONT_ROTZ 0x40            /* cont_rotz= */
#define MINICUT_TARGET 0x80               /* tgt_locator=, tgt_character=, tgt_obj= or tgtpos_* found a target */
#define MINICUT_CAMERA_POSITION 0x100     /* use_current_campos, campos_* or cam_locator= */
#define MINICUT_START_TIME 0x200          /* start_time= */
#define MINICUT_END_TIME 0x400            /* end_time= */
#define MINICUT_BLEND_TIME 0x800          /* blend_time= */
#define MINICUT_HOLD_TIME 0x1000          /* hold_time= */
#define MINICUT_ATTACH_TO_SPLINE 0x2000   /* attach_to_spline= */
#define MINICUT_SPLINE_DIST_OFFSET 0x4000 /* spline_dist_offset= */

/* MINICAM_COMMAND.type: what MiniCam's command loop does with each (Minicam_AddCommand, GameCameraMakeMiniCut3). */
#define MINICAM_COMMAND_START 1 /* blend in */
#define MINICAM_COMMAND_END_TIME 3
#define MINICAM_COMMAND_BLEND 4
#define MINICAM_COMMAND_HOLD 5
#define MINICAM_COMMAND_MODE 6
#define MINICAM_COMMAND_EASING 7
#define MINICAM_COMMAND_FOCUS 8
#define MINICAM_COMMAND_DISTANCE 9
#define MINICAM_COMMAND_PITCH 10
#define MINICAM_COMMAND_YAW 11
#define MINICAM_COMMAND_ROLL 12
#define MINICAM_COMMAND_PITCH_RATE 13
#define MINICAM_COMMAND_YAW_RATE 14
#define MINICAM_COMMAND_ROLL_RATE 15
#define MINICAM_COMMAND_POSITION 16
#define MINICAM_COMMAND_SPLINE 17

/* MINICAM.mode and .easing: the DynamicCameraCut keywords mode=follow, mode=static (a spline camera is mode 2) and
 * linear, slowfast, slowfastslow, fastslow, fastslowfast. */
#define MINICAM_MODE_FOLLOW 0
#define MINICAM_MODE_STATIC 1
#define MINICAM_MODE_SPLINE 2
#define MINICAM_EASING_LINEAR 0
#define MINICAM_EASING_SLOWFAST 1
#define MINICAM_EASING_SLOWFASTSLOW 2
#define MINICAM_EASING_FASTSLOW 3
#define MINICAM_EASING_FASTSLOWFAST 4

    typedef struct MINICAM_COMMAND { // size 0x1c
        i32 type;                    // 0x00
        f32 duration;                // 0x04
        i32 argument;                // 0x08
        NUVEC position;              // 0x0c
        NUVEC *target;               // 0x18
    } MINICAM_COMMAND;

    typedef union MINICAM { // size 0x3fc

        struct {                          // size 0x3fc
            MINICAM_COMMAND commands[32]; // 0x000
            u8 command_count;             // 0x380
            u8 current_command;           // 0x381
            u8 mode;                      // 0x382
            u8 easing;                    // 0x383
            u8 borders;                   // 0x384
            u8 reserved_385[0x3];        // 0x385
            f32 distance;                 // 0x388
            u16 pitch;                    // 0x38c
            u16 yaw;                      // 0x38e
            u16 roll;                     // 0x390
            u8 reserved_392[0x2];          // 0x392
            f32 target_distance;          // 0x394
            u16 target_pitch;             // 0x398
            u16 target_yaw;               // 0x39a
            u16 target_roll;              // 0x39c
            u8 reserved_39e[0x2];          // 0x39e
            NUVEC focus_velocity;         // 0x3a0
            NUVEC position_velocity;      // 0x3ac
            f32 start_distance;           // 0x3b8
            u16 start_pitch;              // 0x3bc
            u16 start_yaw;                // 0x3be
            u16 start_roll;               // 0x3c0
            u8 reserved_3c2[0x2];          // 0x3c2
            NUVEC *focus;                 // 0x3c4
            NUVEC focus_offset;           // 0x3c8
            NUVEC target;                 // 0x3d4
            NUVEC position;               // 0x3e0
            NUGSPLINE *position_spline;   // 0x3ec
            f32 delta_time;               // 0x3f0
            f32 duration;                 // 0x3f4
            f32 elapsed_time;             // 0x3f8
        };

        MINICAM_COMMAND duplicate_search[33];
    } MINICAM;

    extern MINICAM MiniCam;
    extern NUGSPLINE ObstacleCamCutSpline;
    extern i32 ObstacleCamTargetGuid;
    extern u8 ObstacleCamAlwaysSnapAngles;
    void GameCameraMakeMiniCut(NUGSPLINE *, f32, f32, f32, f32, i32, i32);
    void GameCameraMakeMiniCut3(u32, f32, i32, i32, i32, void *, i32, NUVEC *, f32, f32, f32, f32, f32, f32, f32, i32, NUGSPLINE *, i8, i8);
    void Minicam_AddCommand(i32, f32, i32, void *, NUVEC);
    void MiniCam_ChangeMode(i32 mode);
    void Minicam_AddDeltas(f32 duration);
    void Minicam_ClearDeltas(void);
    void Minicam_ResetForNextCommand(void);
    void Minicam_CalcCamPos(void);
    void Minicam_Update(void);
    void Minicam_InitSystem(void);
#ifdef __cplusplus
}
#endif
