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

    typedef struct MINICAM_COMMAND_PC {
        i32 type;
        f32 duration;
        i32 argument;
        NUVEC position;
        NUVEC *target;
    } MINICAM_COMMAND_PC;

    STATIC_ASSERT(sizeof(MINICAM_COMMAND_PC) == 0x1c);
    STATIC_ASSERT(offsetof(MINICAM_COMMAND_PC, type) == 0);
    STATIC_ASSERT(offsetof(MINICAM_COMMAND_PC, duration) == 4);
    STATIC_ASSERT(offsetof(MINICAM_COMMAND_PC, argument) == 8);
    STATIC_ASSERT(offsetof(MINICAM_COMMAND_PC, position) == 0xc);
    STATIC_ASSERT(offsetof(MINICAM_COMMAND_PC, target) == 0x18);

    typedef union MINICAM_PC {
        struct {
            MINICAM_COMMAND_PC commands[32];
            u8 command_count;
            u8 current_command;
            u8 mode;
            u8 easing;
            u8 borders;
            u8 unknown_385_b[0x3];
            f32 distance;
            u16 pitch;
            u16 yaw;
            u16 roll;
            u8 unknown_392[0x2];
            f32 target_distance;
            u16 target_pitch;
            u16 target_yaw;
            u16 target_roll;
            u8 unknown_39e[0x2];
            NUVEC focus_velocity;
            NUVEC position_velocity;
            f32 start_distance;
            u16 start_pitch;
            u16 start_yaw;
            u16 start_roll;
            u8 unknown_3c2[0x2];
            NUVEC *focus;
            NUVEC focus_offset;
            NUVEC target;
            NUVEC position;
            NUGSPLINE *position_spline;
            f32 delta_time;
            f32 duration;
            f32 elapsed_time;
        };

        MINICAM_COMMAND_PC duplicate_search[33];
    } MINICAM_PC;

    STATIC_ASSERT(sizeof(MINICAM_PC) == 0x3fc);
    STATIC_ASSERT(offsetof(MINICAM_PC, command_count) == 0x380);
    STATIC_ASSERT(offsetof(MINICAM_PC, current_command) == 0x381);
    STATIC_ASSERT(offsetof(MINICAM_PC, mode) == 0x382);
    STATIC_ASSERT(offsetof(MINICAM_PC, easing) == 0x383);
    STATIC_ASSERT(offsetof(MINICAM_PC, borders) == 0x384);
    STATIC_ASSERT(offsetof(MINICAM_PC, focus) == 0x3c4);
    STATIC_ASSERT(offsetof(MINICAM_PC, focus_offset) == 0x3c8);
    STATIC_ASSERT(offsetof(MINICAM_PC, target) == 0x3d4);
    STATIC_ASSERT(offsetof(MINICAM_PC, position) == 0x3e0);
    STATIC_ASSERT(offsetof(MINICAM_PC, duration) == 0x3f4);
    STATIC_ASSERT(offsetof(MINICAM_PC, elapsed_time) == 0x3f8);
    STATIC_ASSERT(offsetof(MINICAM_PC, duplicate_search) == 0);

    extern MINICAM_PC MiniCam;
    extern NUGSPLINE ObstacleCamCutSpline;
    extern i32 ObstacleCamTargetGuid;
    extern u8 ObstacleCamAlwaysSnapAnglesPc;
    void GameCameraMakeMiniCut(NUGSPLINE *, f32, f32, f32, f32, i32, i32);
    void GameCameraMakeMiniCut3(u32, f32, i32, i32, i32, void *, i32, NUVEC *, f32, f32, f32, f32, f32, f32, f32, i32,
                                NUGSPLINE *, i8, i8);
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
