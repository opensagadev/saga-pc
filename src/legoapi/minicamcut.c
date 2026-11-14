#include "saga.h"

#include <string.h>

typedef struct MINICAM_CAMERA_PC {
    u8 unknown000[0xe8];   // 0x000
    NUVEC render_position; // 0x0e8
    u8 unknown0f4[0x28];   // 0x0f4
    NUVEC position;        // 0x11c
    u8 unknown128[0xd0];   // 0x128
    u16 pitch;             // 0x1f8
    u8 unknown1fa[2];      // 0x1fa
    u16 yaw;               // 0x1fc
    u8 unknown1fe[0x2e];   // 0x1fe
    i8 mode;               // 0x22c
} MINICAM_CAMERA_PC;       // size 0x230

#define MINI_CAM_CAMERA (*(MINICAM_CAMERA_PC *)GameCam)

/* The SDK's inline NuFdiv: a quotient that is 0 when either operand is 0. */
static f32 MinicamcutFdivPc(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f) {
        return 0.0f;
    }
    return a / b;
}

void Minicam_InitSystem(void) {
    memset(&MiniCam, 0, sizeof(MiniCam));
}

void Minicam_AddCommand(i32 type, f32 duration, i32 argument, void *target, NUVEC position) {
    if (type == 6 || type == 7) {
        i32 i;
        for (i = MiniCam.command_count; i >= 0; --i) {
            if (MiniCam.duplicate_search[i].type == type) {
                if (MiniCam.duplicate_search[i].argument == argument) {
                    return;
                }
                i = -1;
            }
        }
    }
    if (MiniCam.command_count == 32) {
        return;
    }
    switch (type) {
        case 1:
            MiniCam.commands[MiniCam.command_count].type = 1;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 3:
            MiniCam.commands[MiniCam.command_count].type = 3;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 4:
            MiniCam.commands[MiniCam.command_count].type = 4;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 5:
            MiniCam.commands[MiniCam.command_count].type = 5;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 6:
            MiniCam.commands[MiniCam.command_count].type = 6;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 7:
            MiniCam.commands[MiniCam.command_count].type = 7;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 8:
            MiniCam.commands[MiniCam.command_count].type = 8;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = target;
            break;
        case 9:
            MiniCam.commands[MiniCam.command_count].type = 9;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 10:
            MiniCam.commands[MiniCam.command_count].type = 10;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 11:
            MiniCam.commands[MiniCam.command_count].type = 11;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 12:
            MiniCam.commands[MiniCam.command_count].type = 12;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 16:
            MiniCam.commands[MiniCam.command_count].type = 16;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].position = position;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 13:
            MiniCam.commands[MiniCam.command_count].type = 13;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 14:
            MiniCam.commands[MiniCam.command_count].type = 14;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 15:
            MiniCam.commands[MiniCam.command_count].type = 15;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 17:
            MiniCam.commands[MiniCam.command_count].type = 17;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = target;
            break;
    }
    ++MiniCam.command_count;
}

void MiniCam_ChangeMode(i32 mode) {
    if (mode == 0) {
        NUVEC delta;
        MiniCam.distance = NuVecDist(&MiniCam.target, &MiniCam.position, &delta);
        FindAnglesXY(&delta, &MiniCam.pitch, &MiniCam.yaw);
        MiniCam.yaw += 0x8000;
        MiniCam.pitch = -MiniCam.pitch;
        MiniCam.mode = 0;
    } else if (mode == 1) {
        MiniCam.mode = 1;
    } else if (mode == 2) {
        MiniCam.mode = 2;
    }
}

void Minicam_AddDeltas(float duration) {
    f32 fraction;
    fraction = MinicamcutFdivPc(MiniCam.delta_time, duration);
    if (fraction < 0.0f) {
        fraction = 0.0f;
    } else if (fraction > 1.0f) {
        fraction = 1.0f;
    }
    switch (MiniCam.easing) {
        case 1:
            fraction = 1.0f + NuTrigTable[(((i32)(fraction * 16384.0f + 32768.0f + 16384.0f)) >> 1) & 0x7fff];
            break;
        case 2:
            fraction = 1.0f - (NuTrigTable[(((i32)(fraction * 32768.0f + 16384.0f)) >> 1) & 0x7fff] + 1.0f) * 0.5f;
            break;
        case 3:
            fraction = NuTrigTable[(((i32)(fraction * 16384.0f + 49152.0f + 16384.0f)) >> 1) & 0x7fff];
            break;
        case 4:
            if (fraction < 0.5f) {
                f32 doubled_fraction = fraction + fraction;
                fraction = NuTrigTable[(((i32)(doubled_fraction * 16384.0f)) >> 1) & 0x7fff] * 0.5f;
            } else if (fraction > 0.5f) {
                f32 doubled_fraction = (fraction - 0.5f) * 2.0f;
                fraction =
                    (1.0f - NuTrigTable[(((i32)(doubled_fraction * 16384.0f + 16384.0f)) >> 1) & 0x7fff]) * 0.5f + 0.5f;
            } else {
                fraction = 0.5f;
            }
            break;
    }
    if (MiniCam.mode == 0) {
        i32 pitch_delta;
        i32 yaw_delta;
        i32 roll_delta;
        pitch_delta = MiniCam.target_pitch - MiniCam.start_pitch;
        if (pitch_delta > 32768) {
            pitch_delta -= 65536;
        } else if (pitch_delta < -65536) {
            pitch_delta += 65536;
        }
        yaw_delta = MiniCam.target_yaw - MiniCam.start_yaw;
        if (yaw_delta > 32768) {
            yaw_delta -= 65536;
        } else if (yaw_delta < -65536) {
            yaw_delta += 65536;
        }
        roll_delta = MiniCam.target_roll - MiniCam.start_roll;
        if (roll_delta > 32768) {
            roll_delta -= 65536;
        } else if (roll_delta < -65536) {
            roll_delta += 65536;
        }
        MiniCam.pitch = SeekRot(MiniCam.pitch, (u16)(i32)(MiniCam.start_pitch + pitch_delta * fraction), 10.0f);
        MiniCam.yaw = SeekRot(MiniCam.yaw, (u16)(i32)(MiniCam.start_yaw + yaw_delta * fraction), 10.0f);
        MiniCam.roll = SeekRot(MiniCam.roll, (u16)(i32)(MiniCam.start_roll + roll_delta * fraction), 10.0f);
        MiniCam.distance = (MiniCam.target_distance - MiniCam.start_distance) * fraction + MiniCam.start_distance;
    } else if (MiniCam.mode == 1) {
        MiniCam.position.x += MiniCam.position_velocity.x * FRAMETIME;
        MiniCam.position.y += MiniCam.position_velocity.y * FRAMETIME;
        MiniCam.position.z += MiniCam.position_velocity.z * FRAMETIME;
    } else if (MiniCam.mode == 2) {
        PointAlongSpline(MiniCam.position_spline, fraction, &MiniCam.position, NULL, NULL, 0);
    }
    MiniCam.focus_offset.x -= MiniCam.focus_velocity.x * FRAMETIME;
    MiniCam.focus_offset.y -= MiniCam.focus_velocity.y * FRAMETIME;
    MiniCam.focus_offset.z -= MiniCam.focus_velocity.z * FRAMETIME;
    if (MiniCam.focus != NULL) {
        NUVEC target;
        NuVecAdd(&target, MiniCam.focus, &MiniCam.focus_offset);
        MiniCam.target.x = SeekLinearF(MiniCam.target.x, target.x, 10.0f);
        MiniCam.target.y = SeekLinearF(MiniCam.target.y, target.y, 10.0f);
        MiniCam.target.z = SeekLinearF(MiniCam.target.z, target.z, 10.0f);
    }
}

void Minicam_ClearDeltas(void) {
    MiniCam.start_distance = MiniCam.target_distance = MiniCam.distance;
    MiniCam.start_pitch = MiniCam.target_pitch = MiniCam.pitch;
    MiniCam.start_yaw = MiniCam.target_yaw = MiniCam.yaw;
    MiniCam.start_roll = MiniCam.target_roll = MiniCam.roll;
    MiniCam.focus_offset = MiniCam.focus_velocity = v000;
}

void Minicam_ResetForNextCommand(void) {
    i32 i;
    Minicam_ClearDeltas();
    for (i = 0; MiniCam.current_command + i + 1 < MiniCam.command_count; i++) {
        MiniCam.commands[i] = MiniCam.commands[MiniCam.current_command + i + 1];
    }
    MiniCam.command_count = (u8)i;
    memset(&MiniCam.commands[MiniCam.command_count], 0, (32 - MiniCam.command_count) * sizeof(MINICAM_COMMAND_PC));
    MiniCam.current_command = 0;
    MiniCam.delta_time = 0.0f;
}

void fun_005776f0(void) {
    memset(&MiniCam, 0, sizeof(MiniCam));
}

void Minicam_CalcCamPos(void) {
    if (MiniCam.mode == 0) {
        MiniCam.position.y = MiniCam.position.x = 0.0f;
        MiniCam.position.z = MiniCam.distance;
        NuVecRotateX(&MiniCam.position, &MiniCam.position, MiniCam.pitch);
        NuVecRotateY(&MiniCam.position, &MiniCam.position, MiniCam.yaw);
        ObstacleCamRotZ = MiniCam.roll;
        NuVecAdd(&MiniCam.position, &MiniCam.position, &MiniCam.target);
    }
}

void GameCameraMakeMiniCut(NUGSPLINE *spline, f32 start, f32 end, f32 blend_in, f32 blend_out, i32 borders,
                           i32 hold_until_players_move) {
    if (spline == NULL) {
        return;
    }
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
    ObstacleCamHoldUntilPlayersMove = VehicleArea == 0 ? hold_until_players_move : 0;
    if (borders != 0) {
        if (start == 0.0f && blend_in <= 0.0f) {
            CutBorderScale = 1.0f;
        }
    }
    if (start <= 0.0f && blend_in <= 0.0f) {
        GameCam->mode = -1;
        GameCam->blend_time = GameCam->blend_duration;
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
    if (target != NULL && follow_target != 0) {
        ObstacleCamCutTgtPtr = target;
    }
    if (camera != NULL && follow_camera != 0) {
        ObstacleCamCutCamPtr = camera;
    }
}

void Minicam_Update(void) {
    f32 distance = MiniCam.distance;
    i32 pitch = MiniCam.pitch;
    i32 yaw = MiniCam.yaw;
    i32 roll = MiniCam.roll;
    NUVEC position = MiniCam.position;
    i32 pitch_rate = 0;
    i32 yaw_rate = 0;
    i32 roll_rate = 0;
    NUVEC *focus = MiniCam.focus;
    f32 command_duration = 0.0f;
    i32 type;
    i32 done = 0;

    while (!done && MiniCam.current_command < MiniCam.command_count) {
        switch (MiniCam.commands[MiniCam.current_command].type) {
            case 0:
                return;
            case 1:
            case 3:
            case 4:
                command_duration = MiniCam.commands[MiniCam.current_command].duration;
                done = 1;
                break;
            case 2:
                done = 1;
                break;
            case 5:
                command_duration = MiniCam.commands[MiniCam.current_command].duration;
                Minicam_ClearDeltas();
                done = 1;
                break;
            case 6: {
                i32 target_pitch;
                i32 old_yaw;
                i32 old_roll;
                f32 old_distance;
                MiniCam_ChangeMode(MiniCam.commands[MiniCam.current_command].argument);
                target_pitch = (u16)(pitch_rate == 0 ? pitch : (i32)(pitch_rate * 0.0f));
                old_yaw = MiniCam.target_yaw;
                old_roll = MiniCam.target_roll;
                old_distance = MiniCam.target_distance;
                MiniCam.target_pitch = target_pitch;
                MiniCam.start_pitch = MiniCam.pitch;
                Minicam_ClearDeltas();
                if (target_pitch == pitch) {
                    pitch = MiniCam.target_pitch;
                }
                if (old_yaw == yaw) {
                    yaw = MiniCam.target_yaw;
                }
                if (old_roll == roll) {
                    roll = MiniCam.target_roll;
                }
                if (old_distance == distance) {
                    distance = MiniCam.target_distance;
                }
                break;
            }
            case 7:
                MiniCam.easing = MiniCam.commands[MiniCam.current_command].argument;
                break;
            case 8:
                focus = MiniCam.commands[MiniCam.current_command].target;
                break;
            case 9:
                distance = MiniCam.commands[MiniCam.current_command].duration;
                break;
            case 10:
                pitch = MiniCam.commands[MiniCam.current_command].argument;
                break;
            case 11:
                yaw = MiniCam.commands[MiniCam.current_command].argument;
                break;
            case 12:
                roll = MiniCam.commands[MiniCam.current_command].argument;
                break;
            case 13:
                pitch_rate = MiniCam.commands[MiniCam.current_command].argument;
                break;
            case 14:
                yaw_rate = MiniCam.commands[MiniCam.current_command].argument;
                break;
            case 15:
                roll_rate = MiniCam.commands[MiniCam.current_command].argument;
                break;
            case 16:
                position = MiniCam.commands[MiniCam.current_command].position;
                break;
            case 17:
                MiniCam.position_spline = (NUGSPLINE *)MiniCam.commands[MiniCam.current_command].target;
                break;
            default:
                ++MiniCam.current_command;
                return;
        }
        if (!done) {
            ++MiniCam.current_command;
        }
    }

    MiniCam.elapsed_time += FRAMETIME;
    if (MiniCam.duration != 0.0f && MiniCam.elapsed_time > MiniCam.duration) {
        Minicam_AddDeltas(command_duration);
        Minicam_CalcCamPos();
        ObstacleCamEnd = 0.0f;
        if (ObstacleCamBlendOutTime == 0.0f) {
            MINI_CAM_CAMERA.mode = -1;
            Minicam_ResetForNextCommand();
        }
        if (MiniCutCam == 0) {
            Minicam_ResetForNextCommand();
        }
        return;
    }
    if (MiniCam.current_command >= MiniCam.command_count) {
        Minicam_AddDeltas(1000000000.0f);
        Minicam_CalcCamPos();
        return;
    }
    type = MiniCam.commands[MiniCam.current_command].type;
    if (type == 1) {
        if (MiniCam.delta_time < command_duration) {
            MiniCam.delta_time += FRAMETIME;
        } else {
            NUVEC delta;
            MiniCam.distance = NuVecDist(&MINI_CAM_CAMERA.render_position, MiniCam.focus, &delta);
            MiniCam.start_distance = MiniCam.distance;
            MiniCam.pitch = MINI_CAM_CAMERA.pitch + 0x8000;
            MiniCam.start_pitch = MiniCam.pitch;
            MiniCam.yaw = MINI_CAM_CAMERA.yaw;
            MiniCam.start_yaw = MiniCam.yaw;
            MiniCam.position = MINI_CAM_CAMERA.position;
            Minicam_ResetForNextCommand();
        }
        return;
    }
    if (type == 3) {
        if (MiniCam.delta_time < command_duration) {
            MiniCam.delta_time += FRAMETIME;
            Minicam_AddDeltas(command_duration);
            Minicam_CalcCamPos();
            return;
        }
        ObstacleCamAlwaysSnapAnglesPc = 0;
        Minicam_CalcCamPos();
        ObstacleCamEnd = 0.0f;
        if (ObstacleCamBlendOutTime == 0.0f) {
            MINI_CAM_CAMERA.mode = -1;
            Minicam_ResetForNextCommand();
        }
        if (MiniCutCam == 0) {
            Minicam_ResetForNextCommand();
        }
        return;
    }
    if (type == 4 && MiniCam.delta_time == 0.0f) {
        if (command_duration == 0.0f) {
            MiniCam.focus = focus;
            MiniCam.distance = distance;
            MiniCam.focus_offset = v000;
            MiniCam.position = position;
            MiniCam.pitch = pitch;
            MiniCam.yaw = yaw;
            MiniCam.roll = roll;
            Minicam_ClearDeltas();
            MINI_CAM_CAMERA.mode = -1;
        } else {
            if (pitch_rate == 0) {
                MiniCam.target_pitch = pitch;
            } else {
                MiniCam.target_pitch = (i32)(pitch_rate * command_duration);
            }
            MiniCam.start_pitch = MiniCam.pitch;
            if (yaw_rate == 0) {
                MiniCam.target_yaw = yaw;
            } else {
                MiniCam.target_yaw = (i32)(yaw_rate * command_duration);
            }
            MiniCam.start_yaw = MiniCam.yaw;
            if (roll_rate == 0) {
                MiniCam.target_roll = roll;
            } else {
                MiniCam.target_roll = (i32)(roll_rate * command_duration);
            }
            MiniCam.target_distance = distance;
            MiniCam.start_roll = MiniCam.roll;
            MiniCam.start_distance = MiniCam.distance;
            if (MiniCam.focus != focus) {
                NuVecSub(&MiniCam.focus_offset, MiniCam.focus, focus);
                NuVecScale(&MiniCam.focus_velocity, &MiniCam.focus_offset, 1.0f / command_duration);
                MiniCam.focus = focus;
            }
            NuVecSub(&MiniCam.position_velocity, &position, &MiniCam.position);
            NuVecScale(&MiniCam.position_velocity, &MiniCam.position_velocity, 1.0f / command_duration);
        }
    }
    MiniCam.delta_time += FRAMETIME;
    if (MiniCam.delta_time >= command_duration) {
        Minicam_AddDeltas(command_duration);
        Minicam_ResetForNextCommand();
    } else {
        Minicam_AddDeltas(command_duration);
    }
    Minicam_CalcCamPos();
}

void GameCameraMakeMiniCut3(u32 flags, float distance, i32 pitch, i32 yaw, i32 roll, void *focus, i32 target_guid,
                            NUVEC *position, float start_time, float blend_in_time, float end_time,
                            float blend_out_time, float blend_time, float hold_time, float max_time, i32 borders,
                            NUGSPLINE *spline, i8 mode, i8 easing) {
    NUVEC command_origin;
    f32 remaining_duration;
    GameCam->blend_time = 0.0f;
    if (flags & 0x200) {
        NUVEC reset_origin;
        memset(&MiniCam, 0, sizeof(MiniCam));
        reset_origin = v000;
        Minicam_AddCommand(1, blend_in_time, 0, NULL, reset_origin);
        MiniCam.duration = max_time;
        if ((flags & 0x80) && focus != NULL) {
            MiniCam.focus = focus;
            MiniCam.focus_offset = reset_origin;
            MiniCam.target = *(NUVEC *)focus;
            MiniCam.position = *position;
            GameCameraMakeMiniCut2(&MiniCam.position, &MiniCam.target, target_guid, start_time, 1000000000.0f,
                                   blend_in_time, 0.0f, 1, 1, MiniCam.borders);
        } else {
            MiniCam.focus = &MiniCam.target;
            MiniCam.focus_offset = reset_origin;
            MiniCam.target = reset_origin;
            MiniCam.position = *position;
            GameCameraMakeMiniCut2(&MiniCam.position, NULL, target_guid, start_time, 1000000000.0f, blend_in_time, 0.0f,
                                   0, 1, MiniCam.borders);
        }
    }

    if ((flags & 0x1c00) == 0) {
        blend_time = 0.0f;
        flags |= 0x800;
    }
    command_origin = v000;
    if (flags & 0x80) {
        Minicam_AddCommand(8, 0.0f, 0, focus, command_origin);
    }
    if (flags & 0x100) {
        Minicam_AddCommand(16, 0.0f, 0, NULL, *position);
    } else if (flags & 0x2000) {
        Minicam_AddCommand(17, 0.0f, 0, spline, command_origin);
    } else if (flags & 1) {
        Minicam_AddCommand(9, distance, 0, NULL, command_origin);
    }
    if (flags & 0x10) {
        Minicam_AddCommand(13, 0.0f, pitch, NULL, command_origin);
    } else if (flags & 2) {
        Minicam_AddCommand(10, 0.0f, pitch, NULL, command_origin);
    }
    if (flags & 0x20) {
        Minicam_AddCommand(14, 0.0f, yaw, NULL, command_origin);
    } else if (flags & 4) {
        Minicam_AddCommand(11, 0.0f, yaw, NULL, command_origin);
    }
    if (flags & 0x40) {
        Minicam_AddCommand(15, 0.0f, roll, NULL, command_origin);
    }
    if (flags & 8) {
        Minicam_AddCommand(12, 0.0f, roll, NULL, command_origin);
    }
    if (mode != -1) {
        Minicam_AddCommand(6, 0.0f, mode, NULL, command_origin);
    }
    if (easing != -1) {
        Minicam_AddCommand(7, 0.0f, easing, NULL, command_origin);
    }

    if ((flags & 1) && (flags & 4) && blend_in_time > 0.0f && blend_time == 0.0f) {
        blend_time = 0.01f;
    }
    if ((flags & 0x800) && (flags & 0x1000)) {
        if (hold_time > blend_time) {
            Minicam_AddCommand(4, blend_time, 0, NULL, command_origin);
            Minicam_AddCommand(5, hold_time - blend_time, 0, NULL, command_origin);
        } else {
            Minicam_AddCommand(4, blend_time, 0, NULL, command_origin);
        }
    } else if (flags & 0x800) {
        Minicam_AddCommand(4, blend_time, 0, NULL, command_origin);
    } else if (flags & 0x1000) {
        Minicam_AddCommand(5, hold_time, 0, NULL, command_origin);
    }
    if (flags & 0x400) {
        remaining_duration = end_time;
        if ((flags & 0x800) && (flags & 0x1000)) {
            if (hold_time > blend_time) {
                remaining_duration = end_time - hold_time;
            } else {
                remaining_duration = end_time - blend_time;
            }
        } else if (flags & 0x800) {
            remaining_duration = end_time - blend_time;
        } else if (flags & 0x1000) {
            remaining_duration = end_time - hold_time;
        }
        Minicam_AddCommand(3, MAX(0.0f, remaining_duration), 0, NULL, command_origin);
        ObstacleCamBlendOutTime = blend_out_time;
    }
    if (borders != 0) {
        MiniCam.borders = 1;
    }
}
