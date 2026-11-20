#include "globals.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/camera.h"
#include "legoapi/cutscenes/minicamcut.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/fx/edsplines.h"
#include "legoapi/world/levels/levels.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

#include <string.h>

void MinicamcutNuError(void) {
}

/* The SDK's inline NuFdiv: a quotient that is 0 when either operand is 0. */
static f32 MinicamcutFdiv(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f) {
        return 0.0f;
    }
    return a / b;
}

MINICAM MiniCam = {0};

void Minicam_InitSystem(void) {
    memset(&MiniCam, 0, sizeof(MiniCam));
}

NUVEC v000 = {0};

void Minicam_AddCommand(i32 type, f32 duration, i32 argument, void *target, NUVEC position) {
    if (type == MINICAM_COMMAND_MODE || type == MINICAM_COMMAND_EASING) {
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
        case MINICAM_COMMAND_START:
            MiniCam.commands[MiniCam.command_count].type = MINICAM_COMMAND_START;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case MINICAM_COMMAND_END_TIME:
            MiniCam.commands[MiniCam.command_count].type = MINICAM_COMMAND_END_TIME;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case MINICAM_COMMAND_BLEND:
            MiniCam.commands[MiniCam.command_count].type = MINICAM_COMMAND_BLEND;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case MINICAM_COMMAND_HOLD:
            MiniCam.commands[MiniCam.command_count].type = MINICAM_COMMAND_HOLD;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case MINICAM_COMMAND_MODE:
            MiniCam.commands[MiniCam.command_count].type = MINICAM_COMMAND_MODE;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case MINICAM_COMMAND_EASING:
            MiniCam.commands[MiniCam.command_count].type = MINICAM_COMMAND_EASING;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case MINICAM_COMMAND_FOCUS:
            MiniCam.commands[MiniCam.command_count].type = MINICAM_COMMAND_FOCUS;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = target;
            break;
        case MINICAM_COMMAND_DISTANCE:
            MiniCam.commands[MiniCam.command_count].type = MINICAM_COMMAND_DISTANCE;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case MINICAM_COMMAND_PITCH:
            MiniCam.commands[MiniCam.command_count].type = MINICAM_COMMAND_PITCH;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case MINICAM_COMMAND_YAW:
            MiniCam.commands[MiniCam.command_count].type = MINICAM_COMMAND_YAW;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case MINICAM_COMMAND_ROLL:
            MiniCam.commands[MiniCam.command_count].type = MINICAM_COMMAND_ROLL;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case MINICAM_COMMAND_POSITION:
            MiniCam.commands[MiniCam.command_count].type = MINICAM_COMMAND_POSITION;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].position = position;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case MINICAM_COMMAND_PITCH_RATE:
            MiniCam.commands[MiniCam.command_count].type = MINICAM_COMMAND_PITCH_RATE;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case MINICAM_COMMAND_YAW_RATE:
            MiniCam.commands[MiniCam.command_count].type = MINICAM_COMMAND_YAW_RATE;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case MINICAM_COMMAND_ROLL_RATE:
            MiniCam.commands[MiniCam.command_count].type = MINICAM_COMMAND_ROLL_RATE;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case MINICAM_COMMAND_SPLINE:
            MiniCam.commands[MiniCam.command_count].type = MINICAM_COMMAND_SPLINE;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = target;
            break;
    }
    ++MiniCam.command_count;
}

void MiniCam_ChangeMode(i32 mode) {
    if (mode == MINICAM_MODE_FOLLOW) {
        NUVEC delta;
        MiniCam.distance = NuVecDist(&MiniCam.target, &MiniCam.position, &delta);
        FindAnglesXY(&delta, &MiniCam.pitch, &MiniCam.yaw);
        MiniCam.yaw += 0x8000;
        MiniCam.pitch = -MiniCam.pitch;
        MiniCam.mode = MINICAM_MODE_FOLLOW;
    } else if (mode == MINICAM_MODE_STATIC) {
        MiniCam.mode = MINICAM_MODE_STATIC;
    } else if (mode == MINICAM_MODE_SPLINE) {
        MiniCam.mode = MINICAM_MODE_SPLINE;
    }
}

void Minicam_AddDeltas(float duration) {
    f32 fraction;
    fraction = MinicamcutFdiv(MiniCam.delta_time, duration);
    if (fraction < 0.0f) {
        fraction = 0.0f;
    } else if (fraction > 1.0f) {
        fraction = 1.0f;
    }
    switch (MiniCam.easing) {
        case MINICAM_EASING_SLOWFAST:
            fraction = 1.0f + NuTrigTable[(((i32)(fraction * 16384.0f + 32768.0f + 16384.0f)) >> 1) & 0x7fff];
            break;
        case MINICAM_EASING_SLOWFASTSLOW:
            fraction = 1.0f - (NuTrigTable[(((i32)(fraction * 32768.0f + 16384.0f)) >> 1) & 0x7fff] + 1.0f) * 0.5f;
            break;
        case MINICAM_EASING_FASTSLOW:
            fraction = NuTrigTable[(((i32)(fraction * 16384.0f + 49152.0f + 16384.0f)) >> 1) & 0x7fff];
            break;
        case MINICAM_EASING_FASTSLOWFAST:
            if (fraction < 0.5f) {
                f32 doubled_fraction = fraction + fraction;
                fraction = NuTrigTable[(((i32)(doubled_fraction * 16384.0f)) >> 1) & 0x7fff] * 0.5f;
            } else if (fraction > 0.5f) {
                f32 doubled_fraction = (fraction - 0.5f) * 2.0f;
                fraction = (1.0f - NuTrigTable[(((i32)(doubled_fraction * 16384.0f + 16384.0f)) >> 1) & 0x7fff]) * 0.5f + 0.5f;
            } else {
                fraction = 0.5f;
            }
            break;
    }
    if (MiniCam.mode == MINICAM_MODE_FOLLOW) {
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
    } else if (MiniCam.mode == MINICAM_MODE_STATIC) {
        MiniCam.position.x += MiniCam.position_velocity.x * FRAMETIME;
        MiniCam.position.y += MiniCam.position_velocity.y * FRAMETIME;
        MiniCam.position.z += MiniCam.position_velocity.z * FRAMETIME;
    } else if (MiniCam.mode == MINICAM_MODE_SPLINE) {
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
    memset(&MiniCam.commands[MiniCam.command_count], 0, (32 - MiniCam.command_count) * sizeof(MINICAM_COMMAND));
    MiniCam.current_command = 0;
    MiniCam.delta_time = 0.0f;
}

void MiniCam_Clear(void) {
    memset(&MiniCam, 0, sizeof(MiniCam));
}

i16 ObstacleCamRotZ = 0;

void Minicam_CalcCamPos(void) {
    if (MiniCam.mode == MINICAM_MODE_FOLLOW) {
        MiniCam.position.y = MiniCam.position.x = 0.0f;
        MiniCam.position.z = MiniCam.distance;
        NuVecRotateX(&MiniCam.position, &MiniCam.position, MiniCam.pitch);
        NuVecRotateY(&MiniCam.position, &MiniCam.position, MiniCam.yaw);
        ObstacleCamRotZ = MiniCam.roll;
        NuVecAdd(&MiniCam.position, &MiniCam.position, &MiniCam.target);
    }
}

f32 ObstacleCamBlendInTime = 0;
u8 ObstacleCamAlwaysSnapAngles = 0;
i32 ObstacleCamBorders = 0;
NUGSPLINE *ObstacleCamSpl = 0;
f32 ObstacleCamEnd = 0;
f32 ObstacleCamBlendOutTime = 0;
f32 ObstacleCamStart = 0;
i32 ObstacleCamHoldUntilPlayersMove = 0;
f32 ObstacleCamTime = 0;
NUVEC *ObstacleCamCutTgtPtr = 0;
NUVEC *ObstacleCamCutCamPtr = 0;

void GameCameraMakeMiniCut(NUGSPLINE *spline, f32 start, f32 end, f32 blend_in, f32 blend_out, i32 borders, i32 hold_until_players_move) {
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
        GameCam->mode = GAMECAM_MODE_NONE;
        GameCam->blend_time = GameCam->blend_duration;
    }
    Hint_CancelCurrent();
    ObstacleCamAlwaysSnapAngles = 0;
}

NUGSPLINE ObstacleCamCutSpline = {0};
i32 ObstacleCamTargetGuid = 0;
NUVEC ObstacleCamCutPts[2] = {0};

void GameCameraMakeMiniCut2(NUVEC *camera, NUVEC *target, i32 target_guid, float start, float end, float blend_in, float blend_out,
                            i32 follow_target, i32 follow_camera, i32 borders) {
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
            case MINICAM_COMMAND_START:
            case MINICAM_COMMAND_END_TIME:
            case MINICAM_COMMAND_BLEND:
                command_duration = MiniCam.commands[MiniCam.current_command].duration;
                done = 1;
                break;
            case 2:
                done = 1;
                break;
            case MINICAM_COMMAND_HOLD:
                command_duration = MiniCam.commands[MiniCam.current_command].duration;
                Minicam_ClearDeltas();
                done = 1;
                break;
            case MINICAM_COMMAND_MODE: {
                i32 target_pitch;
                i32 old_yaw;
                i32 old_roll;
                f32 old_distance;
                MiniCam_ChangeMode(MiniCam.commands[MiniCam.current_command].argument);
                target_pitch = pitch_rate == 0 ? (u16)pitch : (u16)(pitch_rate * 0.0);
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
            case MINICAM_COMMAND_EASING:
                MiniCam.easing = MiniCam.commands[MiniCam.current_command].argument;
                break;
            case MINICAM_COMMAND_FOCUS:
                focus = MiniCam.commands[MiniCam.current_command].target;
                break;
            case MINICAM_COMMAND_DISTANCE:
                distance = MiniCam.commands[MiniCam.current_command].duration;
                break;
            case MINICAM_COMMAND_PITCH:
                pitch = MiniCam.commands[MiniCam.current_command].argument;
                break;
            case MINICAM_COMMAND_YAW:
                yaw = MiniCam.commands[MiniCam.current_command].argument;
                break;
            case MINICAM_COMMAND_ROLL:
                roll = MiniCam.commands[MiniCam.current_command].argument;
                break;
            case MINICAM_COMMAND_PITCH_RATE:
                pitch_rate = MiniCam.commands[MiniCam.current_command].argument;
                break;
            case MINICAM_COMMAND_YAW_RATE:
                yaw_rate = MiniCam.commands[MiniCam.current_command].argument;
                break;
            case MINICAM_COMMAND_ROLL_RATE:
                roll_rate = MiniCam.commands[MiniCam.current_command].argument;
                break;
            case MINICAM_COMMAND_POSITION:
                position = MiniCam.commands[MiniCam.current_command].position;
                break;
            case MINICAM_COMMAND_SPLINE:
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
            GameCam->mode = GAMECAM_MODE_NONE;
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
    if (type == MINICAM_COMMAND_START) {
        if (MiniCam.delta_time < command_duration) {
            MiniCam.delta_time += FRAMETIME;
        } else {
            NUVEC delta;
            MiniCam.distance = NuVecDist(&GameCam->render_mtx.translation, MiniCam.focus, &delta);
            MiniCam.start_distance = MiniCam.distance;
            MiniCam.pitch = (u16)GameCam->pitch + 0x8000;
            MiniCam.start_pitch = MiniCam.pitch;
            MiniCam.yaw = (u16)GameCam->yaw;
            MiniCam.start_yaw = MiniCam.yaw;
            MiniCam.position = GameCam->position;
            Minicam_ResetForNextCommand();
        }
        return;
    }
    if (type == MINICAM_COMMAND_END_TIME) {
        if (MiniCam.delta_time < command_duration) {
            MiniCam.delta_time += FRAMETIME;
            Minicam_AddDeltas(command_duration);
            Minicam_CalcCamPos();
            return;
        }
        ObstacleCamAlwaysSnapAngles = 0;
        Minicam_CalcCamPos();
        ObstacleCamEnd = 0.0f;
        if (ObstacleCamBlendOutTime == 0.0f) {
            GameCam->mode = GAMECAM_MODE_NONE;
            Minicam_ResetForNextCommand();
        }
        if (MiniCutCam == 0) {
            Minicam_ResetForNextCommand();
        }
        return;
    }
    if (type == MINICAM_COMMAND_BLEND && MiniCam.delta_time == 0.0f) {
        if (command_duration == 0.0f) {
            MiniCam.focus = focus;
            MiniCam.distance = distance;
            MiniCam.focus_offset = v000;
            MiniCam.position = position;
            MiniCam.pitch = pitch;
            MiniCam.yaw = yaw;
            MiniCam.roll = roll;
            Minicam_ClearDeltas();
            GameCam->mode = GAMECAM_MODE_NONE;
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

void GameCameraMakeMiniCut3(u32 flags, float distance, i32 pitch, i32 yaw, i32 roll, void *focus, i32 target_guid, NUVEC *position,
                            float start_time, float blend_in_time, float end_time, float blend_out_time, float blend_time, float hold_time,
                            float max_time, i32 borders, NUGSPLINE *spline, i8 mode, i8 easing) {
    NUVEC command_origin;
    f32 remaining_duration;
    GameCam->blend_time = 0.0f;
    if (flags & MINICUT_START_TIME) {
        NUVEC reset_origin;
        memset(&MiniCam, 0, sizeof(MiniCam));
        reset_origin = v000;
        Minicam_AddCommand(MINICAM_COMMAND_START, blend_in_time, 0, NULL, reset_origin);
        MiniCam.duration = max_time;
        if ((flags & MINICUT_TARGET) && focus != NULL) {
            MiniCam.focus = focus;
            MiniCam.focus_offset = reset_origin;
            MiniCam.target = *(NUVEC *)focus;
            MiniCam.position = *position;
            GameCameraMakeMiniCut2(&MiniCam.position, &MiniCam.target, target_guid, start_time, 1000000000.0f, blend_in_time, 0.0f, 1, 1,
                                   MiniCam.borders);
        } else {
            MiniCam.focus = &MiniCam.target;
            MiniCam.focus_offset = reset_origin;
            MiniCam.target = reset_origin;
            MiniCam.position = *position;
            GameCameraMakeMiniCut2(&MiniCam.position, NULL, target_guid, start_time, 1000000000.0f, blend_in_time, 0.0f, 0, 1,
                                   MiniCam.borders);
        }
    }

    if ((flags & (MINICUT_END_TIME | MINICUT_BLEND_TIME | MINICUT_HOLD_TIME)) == 0) {
        blend_time = 0.0f;
        flags |= MINICUT_BLEND_TIME;
    }
    command_origin = v000;
    if (flags & MINICUT_TARGET) {
        Minicam_AddCommand(MINICAM_COMMAND_FOCUS, 0.0f, 0, focus, command_origin);
    }
    if (flags & MINICUT_CAMERA_POSITION) {
        Minicam_AddCommand(MINICAM_COMMAND_POSITION, 0.0f, 0, NULL, *position);
    } else if (flags & MINICUT_ATTACH_TO_SPLINE) {
        Minicam_AddCommand(MINICAM_COMMAND_SPLINE, 0.0f, 0, spline, command_origin);
    } else if (flags & MINICUT_RANGE) {
        Minicam_AddCommand(MINICAM_COMMAND_DISTANCE, distance, 0, NULL, command_origin);
    }
    if (flags & MINICUT_CONT_ROTX) {
        Minicam_AddCommand(MINICAM_COMMAND_PITCH_RATE, 0.0f, pitch, NULL, command_origin);
    } else if (flags & MINICUT_ROTX) {
        Minicam_AddCommand(MINICAM_COMMAND_PITCH, 0.0f, pitch, NULL, command_origin);
    }
    if (flags & MINICUT_CONT_ROTY) {
        Minicam_AddCommand(MINICAM_COMMAND_YAW_RATE, 0.0f, yaw, NULL, command_origin);
    } else if (flags & MINICUT_ROTY) {
        Minicam_AddCommand(MINICAM_COMMAND_YAW, 0.0f, yaw, NULL, command_origin);
    }
    if (flags & MINICUT_CONT_ROTZ) {
        Minicam_AddCommand(MINICAM_COMMAND_ROLL_RATE, 0.0f, roll, NULL, command_origin);
    }
    if (flags & MINICUT_ROTZ) {
        Minicam_AddCommand(MINICAM_COMMAND_ROLL, 0.0f, roll, NULL, command_origin);
    }
    if (mode != -1) {
        Minicam_AddCommand(MINICAM_COMMAND_MODE, 0.0f, mode, NULL, command_origin);
    }
    if (easing != -1) {
        Minicam_AddCommand(MINICAM_COMMAND_EASING, 0.0f, easing, NULL, command_origin);
    }

    if ((flags & MINICUT_RANGE) && (flags & MINICUT_ROTY) && blend_in_time > 0.0f && blend_time == 0.0f) {
        blend_time = 0.01f;
    }
    if ((flags & MINICUT_BLEND_TIME) && (flags & MINICUT_HOLD_TIME)) {
        if (hold_time > blend_time) {
            Minicam_AddCommand(MINICAM_COMMAND_BLEND, blend_time, 0, NULL, command_origin);
            Minicam_AddCommand(MINICAM_COMMAND_HOLD, hold_time - blend_time, 0, NULL, command_origin);
        } else {
            Minicam_AddCommand(MINICAM_COMMAND_BLEND, blend_time, 0, NULL, command_origin);
        }
    } else if (flags & MINICUT_BLEND_TIME) {
        Minicam_AddCommand(MINICAM_COMMAND_BLEND, blend_time, 0, NULL, command_origin);
    } else if (flags & MINICUT_HOLD_TIME) {
        Minicam_AddCommand(MINICAM_COMMAND_HOLD, hold_time, 0, NULL, command_origin);
    }
    if (flags & MINICUT_END_TIME) {
        remaining_duration = end_time;
        if ((flags & MINICUT_BLEND_TIME) && (flags & MINICUT_HOLD_TIME)) {
            if (hold_time > blend_time) {
                remaining_duration = end_time - hold_time;
            } else {
                remaining_duration = end_time - blend_time;
            }
        } else if (flags & MINICUT_BLEND_TIME) {
            remaining_duration = end_time - blend_time;
        } else if (flags & MINICUT_HOLD_TIME) {
            remaining_duration = end_time - hold_time;
        }
        Minicam_AddCommand(MINICAM_COMMAND_END_TIME, MAX(0.0f, remaining_duration), 0, NULL, command_origin);
        ObstacleCamBlendOutTime = blend_out_time;
    }
    if (borders != 0) {
        MiniCam.borders = 1;
    }
}
