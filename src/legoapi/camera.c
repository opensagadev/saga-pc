#include "saga.h"

i32 __cdecl GoingForwardsAlongNarrowSock(GAMEOBJECT *object) {
    i32 difference;
    i32 forwards;
    if (object->in_narrow_socket == 0)
        return 0;
    difference = RotDiff(object->sock_heading, object->field_0x276);
    if (difference < 0)
        difference = -difference;
    forwards = difference < 0x4000;
    if (object->character_context == 0x2a && (f64)object->context_duration * (f64)0.8f > object->context_timer)
        forwards = 1 - forwards;
    return forwards;
}

GAMEPAD_PC *__cdecl ViewCamGetGamePad(void) {
    return ViewCam.gamepad;
}

#pragma data_seg(".data$ViewCam")
VIEWCAM_PC ViewCam = {0,   {1000000000.0f, 0.0f, 0.0f}, -8192, -29500, 2.6600000858306885f, 0.0f, 120.0f, 0x43480000,
                      NULL};
#pragma data_seg()

typedef struct CAMERA_JUDDER_FIELDS_PC {
    u8 unknown_000[0x11c];
    NUVEC position;
    u8 unknown_128[0xa0];
    f32 time;
    f32 duration;
    u8 unknown_1d0[0x58];
    u8 reverse;
    u8 axis;
} CAMERA_JUDDER_FIELDS_PC;
STATIC_ASSERT(offsetof(CAMERA_JUDDER_FIELDS_PC, position) == 0x11c);
STATIC_ASSERT(offsetof(CAMERA_JUDDER_FIELDS_PC, time) == 0x1c8);
STATIC_ASSERT(offsetof(CAMERA_JUDDER_FIELDS_PC, duration) == 0x1cc);
STATIC_ASSERT(offsetof(CAMERA_JUDDER_FIELDS_PC, reverse) == 0x228);
STATIC_ASSERT(offsetof(CAMERA_JUDDER_FIELDS_PC, axis) == 0x229);
typedef struct LEVEL_JUDDER_FIELDS_PC {
    u8 unknown_000[0xcd];
    u8 maximum_distance;
} LEVEL_JUDDER_FIELDS_PC;
STATIC_ASSERT(offsetof(LEVEL_JUDDER_FIELDS_PC, maximum_distance) == 0xcd);

void __cdecl GameCam_Judder(GAMECAMERA *camera, f32 amount, i32 axis, NUVEC *source) {
    union {
        f32 value;
        u32 bits;
    } absolute;
    CAMERA_JUDDER_FIELDS_PC *judder;
    f32 distance;
    f32 maximum_distance;
    if (camera == NULL)
        camera = GameCam;
    judder = (CAMERA_JUDDER_FIELDS_PC *)camera;
    absolute.value = amount;
    absolute.bits &= 0x7fffffff;
    if (absolute.value > judder->time) {
        if (source != NULL) {
            distance = NuVecDist(&judder->position, source, NULL);
            maximum_distance = (f32)((LEVEL_JUDDER_FIELDS_PC *)WORLD->current_level)->maximum_distance;
            if (maximum_distance > distance) {
                absolute.value *= (maximum_distance - distance) / maximum_distance;
                if (absolute.value > judder->time) {
                    judder->duration = absolute.value;
                    judder->time = absolute.value;
                    judder->reverse = amount < 0.0f;
                }
            }
        } else {
            judder->duration = absolute.value;
            judder->time = absolute.value;
            judder->reverse = amount < 0.0f;
        }
        judder->axis = (u8)axis;
    }
}

typedef struct CAMERA_BLEND_FIELDS_PC {
    u8 unknown_000[0x128];
    NUVEC target;
    NUVEC blend_start_position;
    NUVEC desired_position;
    NUVEC blend_end_position;
    u8 unknown_158[0xc];
    NUVEC blend_start_target;
    u8 unknown_170[0xc];
    NUVEC blend_end_target;
    u8 unknown_188[0x34];
    f32 blend_time;
    f32 blend_duration;
    f32 blend_curve;
    u8 unknown_1c8[0x54];
    u16 blend_start_pitch;
    u16 desired_pitch;
    u16 blend_start_yaw;
    u16 desired_yaw;
    u16 blend_start_roll;
    u16 desired_roll;
    u8 unknown_228[2];
    u8 reset_blend;
    u8 blend_mode;
    i8 mode;
    u8 previous_mode;
    u8 previous_camera_mode;
} CAMERA_BLEND_FIELDS_PC;
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, target) == 0x128);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_start_position) == 0x134);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, desired_position) == 0x140);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_end_position) == 0x14c);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_start_target) == 0x164);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_end_target) == 0x17c);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_time) == 0x1bc);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_duration) == 0x1c0);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_curve) == 0x1c4);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_start_pitch) == 0x21c);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, desired_pitch) == 0x21e);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_start_yaw) == 0x220);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, desired_yaw) == 0x222);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_start_roll) == 0x224);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, desired_roll) == 0x226);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, reset_blend) == 0x22a);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, blend_mode) == 0x22b);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, mode) == 0x22c);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, previous_mode) == 0x22d);
STATIC_ASSERT(offsetof(CAMERA_BLEND_FIELDS_PC, previous_camera_mode) == 0x22e);

void __cdecl GameCam_Blend(GAMECAMERA *camera, f32 duration, f32 curve, i32 mode) {
    CAMERA_BLEND_FIELDS_PC *blend;
    if (camera == NULL)
        camera = GameCam;
    blend = (CAMERA_BLEND_FIELDS_PC *)camera;
    if (!(duration > 0.0f) || blend->mode == -1)
        return;
    blend->blend_start_pitch = blend->desired_pitch;
    blend->blend_start_yaw = blend->desired_yaw;
    blend->blend_start_roll = blend->desired_roll;
    blend->blend_mode = mode != 0 ? 2 : 1;
    blend->previous_camera_mode = blend->previous_mode;
    blend->blend_start_position = blend->desired_position;
    blend->blend_end_position = blend->desired_position;
    blend->blend_start_target = blend->target;
    blend->blend_end_target = blend->target;
    blend->reset_blend = 1;
    blend->blend_time = 0.0f;
    blend->blend_duration = duration;
    blend->blend_curve = curve;
}

u16 __cdecl GameCam_GetAdjustedYRot(GAMECAMERA *camera) {
    if (camera == NULL)
        camera = GameCam;
    return (u16)(camera->input_yaw + (i32)camera->adjusted_yaw_offset);
}

void __cdecl GameCam_HitRoll(void) {
    f32 amount = qrand() >= 0x8000 ? 0.25f : -0.25f;
    GameCam_Judder(GameCam, amount, 2, NULL);
}

void __cdecl GameCam_HitJudder(void) {
    f32 amount = VehicleArea != 0 ? -0.3f : -0.2f;
    if (qrand() < 0x8000)
        amount = -amount;
    GameCam_Judder(GameCam, amount, qrand() / 0x5556, NULL);
}

i32 __cdecl ViewCamGetMode(void) {
    return ViewCam.mode;
}
NUVEC *__cdecl ViewCamGetTgt(void) {
    return &ViewCam.target;
}

typedef struct CAMERA_CUT_FIELDS_PC {
    u8 unknown_000[0x1bc];
    f32 blend_time, blend_duration;
    u8 unknown_1c4[0x68];
    i8 mode;
} CAMERA_CUT_FIELDS_PC;
STATIC_ASSERT(offsetof(CAMERA_CUT_FIELDS_PC, blend_time) == 0x1bc);
STATIC_ASSERT(offsetof(CAMERA_CUT_FIELDS_PC, blend_duration) == 0x1c0);
STATIC_ASSERT(offsetof(CAMERA_CUT_FIELDS_PC, mode) == 0x22c);

void GameCameraMakeMiniCut(NUGSPLINE *spline, f32 start, f32 end, f32 blend_in, f32 blend_out, i32 borders,
                           i32 hold_until_players_move) {
    if (spline == NULL)
        return;
    ObstacleCamSpl = spline;
    ObstacleCamStart = start;
    ObstacleCamEnd = end;
    ObstacleCamTime = 0.0f;
    ObstacleCamRotZ = 0;
    ObstacleCamBlendInTime = blend_in;
    ObstacleCamBlendOutTime = blend_out;
    ObstacleCamCutTgtPtr = NULL;
    ObstacleCamCutCamPtr = NULL;
    ObstacleCamBorders = borders;
    if (VehicleArea != 0)
        hold_until_players_move = 0;
    ObstacleCamHoldUntilPlayersMove = hold_until_players_move;
    if (borders != 0) {
        if (start == 0.0f && blend_in <= 0.0f)
            CutBorderScale = 1.0f;
    }
    if (start <= 0.0f && blend_in <= 0.0f) {
        ((CAMERA_CUT_FIELDS_PC *)GameCam)->blend_time = ((CAMERA_CUT_FIELDS_PC *)GameCam)->blend_duration;
        ((CAMERA_CUT_FIELDS_PC *)GameCam)->mode = -1;
    }
    Hint_CancelCurrent();
    ObstacleCamAlwaysSnapAnglesPc = 0;
}

void GameCameraMakeMiniCut2(NUVEC *camera, NUVEC *target, i32 target_guid, float start, float end, float blend_in,
                            float blend_out, i32 follow_target, i32 follow_camera, i32 borders) {
    ObstacleCamCutSpline.length = 2;
    ObstacleCamCutSpline.pt_size = 12;
    ObstacleCamCutSpline.pts = ObstacleCamCutPts;
    ObstacleCamTargetGuid = target_guid;
    if (camera != NULL) {
        ObstacleCamCutPts[0] = *camera;
    }
    if (target != NULL) {
        ObstacleCamCutPts[1] = *target;
    }
    GameCameraMakeMiniCut(&ObstacleCamCutSpline, start, end, blend_in, blend_out, borders, 0);
    if (follow_target != 0 && target != NULL) {
        ObstacleCamCutTgtPtr = target;
    }
    if (follow_camera != 0 && camera != NULL) {
        ObstacleCamCutCamPtr = camera;
    }
}

void GameCameraMakeMiniCut3(u32 flags, float distance, i32 pitch, i32 yaw, i32 roll, void *focus, i32 target_guid,
                            NUVEC *position, float start_time, float blend_in_time, float end_time,
                            float blend_out_time, float blend_time, float hold_time, float max_time, i32 borders,
                            NUGSPLINE *spline, i8 mode, i8 easing) {
    i32 have_remaining_hold;
    f32 remaining_hold;
    f32 elapsed;
    f32 remaining_duration;
    ((CAMERA_CUT_FIELDS_PC *)GameCam)->blend_time = 0.0f;
    if (flags & 0x200) {
        memset(&MiniCam, 0, sizeof(MiniCam));
        Minicam_AddCommand(1, blend_in_time, 0, NULL, v000);
        MiniCam.duration = max_time;
        if ((flags & 0x80) && focus != NULL) {
            MiniCam.focus = (NUVEC *)focus;
            MiniCam.focus_offset = v000;
            MiniCam.target = *(NUVEC *)focus;
            MiniCam.position = *position;
            GameCameraMakeMiniCut2(&MiniCam.position, &MiniCam.target, target_guid, start_time, 1000000000.0f,
                                   blend_in_time, 0.0f, 1, 1, MiniCam.borders);
        } else {
            MiniCam.target = v000;
            MiniCam.focus_offset = v000;
            MiniCam.focus = &MiniCam.target;
            MiniCam.position = *position;
            GameCameraMakeMiniCut2(&MiniCam.position, NULL, target_guid, start_time, 1000000000.0f, blend_in_time, 0.0f,
                                   0, 1, MiniCam.borders);
        }
    }

    if ((flags & 0x1c00) == 0) {
        blend_time = 0.0f;
        flags |= 0x800;
    }
    if (flags & 0x80)
        Minicam_AddCommand(8, 0.0f, 0, focus, v000);
    if (flags & 0x100)
        Minicam_AddCommand(16, 0.0f, 0, NULL, *position);
    else if (flags & 0x2000)
        Minicam_AddCommand(17, 0.0f, 0, spline, v000);
    else if (flags & 1)
        Minicam_AddCommand(9, distance, 0, NULL, v000);
    if (flags & 0x10)
        Minicam_AddCommand(13, 0.0f, pitch, NULL, v000);
    else if (flags & 2)
        Minicam_AddCommand(10, 0.0f, pitch, NULL, v000);
    if (flags & 0x20)
        Minicam_AddCommand(14, 0.0f, yaw, NULL, v000);
    else if (flags & 4)
        Minicam_AddCommand(11, 0.0f, yaw, NULL, v000);
    if (flags & 0x40)
        Minicam_AddCommand(15, 0.0f, roll, NULL, v000);
    if (flags & 8)
        Minicam_AddCommand(12, 0.0f, roll, NULL, v000);
    if (mode != -1)
        Minicam_AddCommand(6, 0.0f, mode, NULL, v000);
    if (easing != -1)
        Minicam_AddCommand(7, 0.0f, easing, NULL, v000);

    if ((flags & 5) == 5 && blend_in_time > 0.0f && blend_time == 0.0f)
        blend_time = 0.01f;
    if ((flags & 0x1800) == 0x1800) {
        have_remaining_hold = hold_time > blend_time;
        remaining_hold = have_remaining_hold ? hold_time - blend_time : 0.0f;
        Minicam_AddCommand(4, blend_time, 0, NULL, v000);
        if (have_remaining_hold)
            Minicam_AddCommand(5, remaining_hold, 0, NULL, v000);
    } else if (flags & 0x800) {
        Minicam_AddCommand(4, blend_time, 0, NULL, v000);
    } else if (flags & 0x1000) {
        Minicam_AddCommand(5, hold_time, 0, NULL, v000);
    }
    if (flags & 0x400) {
        elapsed = 0.0f;
        if ((flags & 0x1800) == 0x1800)
            elapsed = hold_time > blend_time ? hold_time : blend_time;
        else if (flags & 0x800)
            elapsed = blend_time;
        else if (flags & 0x1000)
            elapsed = hold_time;
        remaining_duration = end_time - elapsed;
        if (remaining_duration < 0.0f)
            remaining_duration = 0.0f;
        Minicam_AddCommand(3, remaining_duration, 0, NULL, v000);
        ObstacleCamBlendOutTime = blend_out_time;
    }
    if (borders != 0)
        MiniCam.borders = 1;
}
