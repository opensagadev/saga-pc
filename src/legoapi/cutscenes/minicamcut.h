/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct MINICAM_COMMAND { // size 0x1c
        i32 type;                    // 0x00
        f32 duration;                // 0x04
        i32 argument;                // 0x08
        NUVEC position;              // 0x0c
        NUVEC *target;               // 0x18
    } MINICAM_COMMAND;

    typedef union MINICAM_PC { // size 0x3fc

        struct {                          // size 0x3fc
            MINICAM_COMMAND commands[32]; // 0x000
            u8 command_count;             // 0x380
            u8 current_command;           // 0x381
            u8 mode;                      // 0x382
            u8 easing;                    // 0x383
            u8 borders;                   // 0x384
            u8 unknown_385_b[0x3];        // 0x385
            f32 distance;                 // 0x388
            u16 pitch;                    // 0x38c
            u16 yaw;                      // 0x38e
            u16 roll;                     // 0x390
            u8 unknown_392[0x2];          // 0x392
            f32 target_distance;          // 0x394
            u16 target_pitch;             // 0x398
            u16 target_yaw;               // 0x39a
            u16 target_roll;              // 0x39c
            u8 unknown_39e[0x2];          // 0x39e
            NUVEC focus_velocity;         // 0x3a0
            NUVEC position_velocity;      // 0x3ac
            f32 start_distance;           // 0x3b8
            u16 start_pitch;              // 0x3bc
            u16 start_yaw;                // 0x3be
            u16 start_roll;               // 0x3c0
            u8 unknown_3c2[0x2];          // 0x3c2
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
    } MINICAM_PC;

    extern MINICAM_PC MiniCam;
    extern NUGSPLINE ObstacleCamCutSpline;
    extern i32 ObstacleCamTargetGuid;
    extern u8 ObstacleCamAlwaysSnapAnglesPc;
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
