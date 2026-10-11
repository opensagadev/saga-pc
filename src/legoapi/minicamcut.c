#include "saga.h"
#include <string.h>
void Minicam_AddCommand(i32 type, f32 duration, i32 argument, void *target, NUVEC position) {
    if (type == 6 || type == 7) {
        i32 i = MiniCam.command_count;
        for (; i >= 0 && MiniCam.duplicate_search[i].type != type; --i) {
        }
        if (i != -1 && MiniCam.duplicate_search[i].argument == argument)
            return;
    }
    if (MiniCam.command_count == 32)
        return;
    switch (type) {
        case 1:
            MiniCam.commands[MiniCam.command_count].type = 1;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 3:
            MiniCam.commands[MiniCam.command_count].type = 3;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 4:
            MiniCam.commands[MiniCam.command_count].type = 4;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 5:
            MiniCam.commands[MiniCam.command_count].type = 5;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 6:
            MiniCam.commands[MiniCam.command_count].type = 6;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 7:
            MiniCam.commands[MiniCam.command_count].type = 7;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 8:
            MiniCam.commands[MiniCam.command_count].type = 8;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = (NUVEC *)target;
            break;
        case 9:
            MiniCam.commands[MiniCam.command_count].type = 9;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].duration = duration;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 10:
            MiniCam.commands[MiniCam.command_count].type = 10;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 11:
            MiniCam.commands[MiniCam.command_count].type = 11;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 12:
            MiniCam.commands[MiniCam.command_count].type = 12;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 13:
            MiniCam.commands[MiniCam.command_count].type = 13;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 14:
            MiniCam.commands[MiniCam.command_count].type = 14;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 15:
            MiniCam.commands[MiniCam.command_count].type = 15;
            MiniCam.commands[MiniCam.command_count].argument = argument;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 16:
            MiniCam.commands[MiniCam.command_count].type = 16;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = position;
            MiniCam.commands[MiniCam.command_count].target = NULL;
            break;
        case 17:
            MiniCam.commands[MiniCam.command_count].type = 17;
            MiniCam.commands[MiniCam.command_count].argument = 0;
            MiniCam.commands[MiniCam.command_count].duration = 0.0f;
            MiniCam.commands[MiniCam.command_count].position = v000;
            MiniCam.commands[MiniCam.command_count].target = (NUVEC *)target;
            break;
    }
    ++MiniCam.command_count;
}

typedef struct MINICAM_STATE_PC {
    MINICAM_COMMAND_PC commands[32];
    u8 command_count, current_command, mode, easing;
    u8 borders, unknown385[3];
    f32 distance;
    u16 pitch, yaw, roll, unknown392;
    f32 target_distance;
    u16 target_pitch, target_yaw, target_roll, unknown39e;
    NUVEC focus_velocity, position_velocity;
    f32 start_distance;
    u16 start_pitch, start_yaw, start_roll, unknown3c2;
    NUVEC *focus;
    NUVEC focus_offset, target, position;
    NUGSPLINE *position_spline;
    f32 delta_time, duration, elapsed_time;
} MINICAM_STATE_PC;
STATIC_ASSERT(sizeof(MINICAM_STATE_PC) == 0x3fc);
typedef struct MINICAM_CAMERA_PC {
    u8 unknown000[0xe8];
    NUVEC render_position;
    u8 unknown0f4[0x28];
    NUVEC position;
    u8 unknown128[0xd0];
    u16 pitch;
    u8 unknown1fa[2];
    u16 yaw;
    u8 unknown1fe[0x2e];
    i8 mode;
} MINICAM_CAMERA_PC;
#define MINI_CAM_STATE (*(MINICAM_STATE_PC *)&MiniCam)
#define MINI_CAM_CAMERA (*(MINICAM_CAMERA_PC *)GameCam)
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, command_count) == 0x380);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, current_command) == 0x381);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, mode) == 0x382);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, easing) == 0x383);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, borders) == 0x384);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, distance) == 0x388);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, pitch) == 0x38c);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, yaw) == 0x38e);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, roll) == 0x390);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, target_distance) == 0x394);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, target_pitch) == 0x398);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, target_yaw) == 0x39a);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, target_roll) == 0x39c);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, focus_velocity) == 0x3a0);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, position_velocity) == 0x3ac);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, start_distance) == 0x3b8);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, start_pitch) == 0x3bc);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, start_yaw) == 0x3be);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, start_roll) == 0x3c0);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, focus) == 0x3c4);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, focus_offset) == 0x3c8);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, target) == 0x3d4);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, position) == 0x3e0);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, position_spline) == 0x3ec);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, delta_time) == 0x3f0);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, duration) == 0x3f4);
STATIC_ASSERT(offsetof(MINICAM_STATE_PC, elapsed_time) == 0x3f8);
STATIC_ASSERT(offsetof(MINICAM_CAMERA_PC, render_position) == 0xe8);
STATIC_ASSERT(offsetof(MINICAM_CAMERA_PC, position) == 0x11c);
STATIC_ASSERT(offsetof(MINICAM_CAMERA_PC, pitch) == 0x1f8);
STATIC_ASSERT(offsetof(MINICAM_CAMERA_PC, yaw) == 0x1fc);
STATIC_ASSERT(offsetof(MINICAM_CAMERA_PC, mode) == 0x22c);

void __cdecl MiniCam_ChangeMode(i32 mode);
void __cdecl Minicam_AddDeltas(f32 duration);
void __cdecl Minicam_ClearDeltas(void);
void __cdecl Minicam_ResetForNextCommand(void);
void __cdecl Minicam_CalcCamPos(void);
void __cdecl Minicam_Update(void);

void MiniCam_ChangeMode(i32 mode) {
    if (mode == 0) {
        NUVEC delta;
        MINI_CAM_STATE.distance = NuVecDist(&MINI_CAM_STATE.target, &MINI_CAM_STATE.position, &delta);
        FindAnglesXY(&delta, &MINI_CAM_STATE.pitch, &MINI_CAM_STATE.yaw);
        MINI_CAM_STATE.yaw += 0x8000;
        MINI_CAM_STATE.pitch = -MINI_CAM_STATE.pitch;
        MINI_CAM_STATE.mode = 0;
    } else if (mode == 1) {
        MINI_CAM_STATE.mode = 1;
    } else if (mode == 2) {
        MINI_CAM_STATE.mode = 2;
    }
}

void Minicam_AddDeltas(float duration) {
    f32 fraction;
    if ((duration == 0.0f) | (MINI_CAM_STATE.delta_time == 0.0f)) {
        fraction = 0.0f;
    } else {
        fraction = MINI_CAM_STATE.delta_time / duration;
        if (fraction < 0.0f)
            fraction = 0.0f;
        else if (fraction > 1.0f)
            fraction = 1.0f;
    }
    switch (MINI_CAM_STATE.easing) {
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
                f32 doubled_fraction = (fraction - 0.5f) + (fraction - 0.5f);
                fraction =
                    (1.0f - NuTrigTable[(((i32)(doubled_fraction * 16384.0f + 16384.0f)) >> 1) & 0x7fff]) * 0.5f + 0.5f;
            } else {
                fraction = 0.5f;
            }
            break;
    }
    if (MINI_CAM_STATE.mode == 0) {
        i32 pitch_delta = MINI_CAM_STATE.target_pitch - MINI_CAM_STATE.start_pitch;
        i32 yaw_delta = MINI_CAM_STATE.target_yaw - MINI_CAM_STATE.start_yaw;
        i32 roll_delta = MINI_CAM_STATE.target_roll - MINI_CAM_STATE.start_roll;
        if (pitch_delta > 32768)
            pitch_delta -= 65536;
        else if (pitch_delta < -65536)
            pitch_delta += 65536;
        if (yaw_delta > 32768)
            yaw_delta -= 65536;
        else if (yaw_delta < -65536)
            yaw_delta += 65536;
        if (roll_delta > 32768)
            roll_delta -= 65536;
        else if (roll_delta < -65536)
            roll_delta += 65536;
        MINI_CAM_STATE.pitch =
            SeekRot(MINI_CAM_STATE.pitch, (u16)(i32)(MINI_CAM_STATE.start_pitch + pitch_delta * fraction), 10.0f);
        MINI_CAM_STATE.yaw =
            SeekRot(MINI_CAM_STATE.yaw, (u16)(i32)(MINI_CAM_STATE.start_yaw + yaw_delta * fraction), 10.0f);
        MINI_CAM_STATE.roll =
            SeekRot(MINI_CAM_STATE.roll, (u16)(i32)(MINI_CAM_STATE.start_roll + roll_delta * fraction), 10.0f);
        MINI_CAM_STATE.distance =
            (MINI_CAM_STATE.target_distance - MINI_CAM_STATE.start_distance) * fraction + MINI_CAM_STATE.start_distance;
    } else if (MINI_CAM_STATE.mode == 1) {
        MINI_CAM_STATE.position.x += MINI_CAM_STATE.position_velocity.x * FRAMETIME;
        MINI_CAM_STATE.position.y += MINI_CAM_STATE.position_velocity.y * FRAMETIME;
        MINI_CAM_STATE.position.z += MINI_CAM_STATE.position_velocity.z * FRAMETIME;
    } else if (MINI_CAM_STATE.mode == 2) {
        PointAlongSpline(MINI_CAM_STATE.position_spline, fraction, &MINI_CAM_STATE.position, NULL, NULL, 0);
    }
    MINI_CAM_STATE.focus_offset.x -= MINI_CAM_STATE.focus_velocity.x * FRAMETIME;
    MINI_CAM_STATE.focus_offset.y -= MINI_CAM_STATE.focus_velocity.y * FRAMETIME;
    MINI_CAM_STATE.focus_offset.z -= MINI_CAM_STATE.focus_velocity.z * FRAMETIME;
    if (MINI_CAM_STATE.focus != NULL) {
        NUVEC target;
        NuVecAdd(&target, MINI_CAM_STATE.focus, &MINI_CAM_STATE.focus_offset);
        MINI_CAM_STATE.target.x = SeekLinearF(MINI_CAM_STATE.target.x, target.x, 10.0f);
        MINI_CAM_STATE.target.y = SeekLinearF(MINI_CAM_STATE.target.y, target.y, 10.0f);
        MINI_CAM_STATE.target.z = SeekLinearF(MINI_CAM_STATE.target.z, target.z, 10.0f);
    }
}

void Minicam_ClearDeltas(void) {
    MINI_CAM_STATE.target_distance = MINI_CAM_STATE.start_distance = MINI_CAM_STATE.distance;
    MINI_CAM_STATE.target_pitch = MINI_CAM_STATE.start_pitch = MINI_CAM_STATE.pitch;
    MINI_CAM_STATE.target_yaw = MINI_CAM_STATE.start_yaw = MINI_CAM_STATE.yaw;
    MINI_CAM_STATE.target_roll = MINI_CAM_STATE.start_roll = MINI_CAM_STATE.roll;
    MINI_CAM_STATE.focus_velocity = MINI_CAM_STATE.focus_offset = v000;
}

void Minicam_ResetForNextCommand(void) {
    i32 current_command, command_count, next_command, remaining_commands;
    Minicam_ClearDeltas();
    current_command = MINI_CAM_STATE.current_command;
    command_count = MINI_CAM_STATE.command_count;
    next_command = current_command + 1;
    if (next_command < command_count) {
        MINICAM_COMMAND_PC *source = &MINI_CAM_STATE.commands[next_command];
        MINICAM_COMMAND_PC *destination = MINI_CAM_STATE.commands;
        MINICAM_COMMAND_PC *const command_end = &MINI_CAM_STATE.commands[command_count];
        while (source != command_end) {
            *destination++ = *source++;
        }
        remaining_commands = command_count - next_command;
    } else {
        remaining_commands = 0;
    }

    MINI_CAM_STATE.command_count = (u8)remaining_commands;
    memset(&MINI_CAM_STATE.commands[remaining_commands], 0,
           (u32)(32 - remaining_commands) * sizeof(MINICAM_COMMAND_PC));
    MINI_CAM_STATE.current_command = 0;
    MINI_CAM_STATE.delta_time = 0.0f;
}

void Minicam_CalcCamPos(void) {
    if (MINI_CAM_STATE.mode == 0) {
        MINI_CAM_STATE.position.x = MINI_CAM_STATE.position.y = 0.0f;
        MINI_CAM_STATE.position.z = MINI_CAM_STATE.distance;
        NuVecRotateX(&MINI_CAM_STATE.position, &MINI_CAM_STATE.position, MINI_CAM_STATE.pitch);
        NuVecRotateY(&MINI_CAM_STATE.position, &MINI_CAM_STATE.position, MINI_CAM_STATE.yaw);
        ObstacleCamRotZ = MINI_CAM_STATE.roll;
        NuVecAdd(&MINI_CAM_STATE.position, &MINI_CAM_STATE.position, &MINI_CAM_STATE.target);
    }
}

void Minicam_Update(void) {
    f32 distance = MINI_CAM_STATE.distance;
    i32 pitch = MINI_CAM_STATE.pitch;
    i32 yaw = MINI_CAM_STATE.yaw;
    i32 roll = MINI_CAM_STATE.roll;
    NUVEC *focus = MINI_CAM_STATE.focus;
    NUVEC position = MINI_CAM_STATE.position;
    i32 pitch_rate = 0, yaw_rate = 0, roll_rate = 0;
    f32 command_duration;
read_command:
    if (MINI_CAM_STATE.current_command < MINI_CAM_STATE.command_count)
        goto dispatch_command;
    command_duration = 0.0f;
execute_command:
    MINI_CAM_STATE.elapsed_time += FRAMETIME;
    if (MINI_CAM_STATE.duration != 0.0f && MINI_CAM_STATE.elapsed_time > MINI_CAM_STATE.duration) {
        Minicam_AddDeltas(command_duration);
        goto end_cut;
    }
    if (MINI_CAM_STATE.current_command >= MINI_CAM_STATE.command_count) {
        Minicam_AddDeltas(1000000000.0f);
        Minicam_CalcCamPos();
        return;
    }
    if (MINI_CAM_STATE.commands[MINI_CAM_STATE.current_command].type == 1) {
        if (command_duration > MINI_CAM_STATE.delta_time) {
            MINI_CAM_STATE.delta_time += FRAMETIME;
        } else {
            NUVEC delta;
            MINI_CAM_STATE.distance = MINI_CAM_STATE.start_distance =
                NuVecDist(&MINI_CAM_CAMERA.render_position, MINI_CAM_STATE.focus, &delta);
            MINI_CAM_STATE.pitch = MINI_CAM_STATE.start_pitch = MINI_CAM_CAMERA.pitch + 0x8000;
            MINI_CAM_STATE.yaw = MINI_CAM_STATE.start_yaw = MINI_CAM_CAMERA.yaw;
            MINI_CAM_STATE.position = MINI_CAM_CAMERA.position;
            Minicam_ResetForNextCommand();
        }
        return;
    }
    if (MINI_CAM_STATE.commands[MINI_CAM_STATE.current_command].type == 3) {
        if (command_duration > MINI_CAM_STATE.delta_time) {
            MINI_CAM_STATE.delta_time += FRAMETIME;
            Minicam_AddDeltas(command_duration);
            Minicam_CalcCamPos();
            return;
        }
        ObstacleCamAlwaysSnapAnglesPc = 0;
        goto end_cut;
    }
    if (MINI_CAM_STATE.commands[MINI_CAM_STATE.current_command].type == 4 && MINI_CAM_STATE.delta_time == 0.0f) {
        if (command_duration == 0.0f) {
            MINI_CAM_STATE.focus = focus;
            MINI_CAM_STATE.distance = distance;
            MINI_CAM_STATE.pitch = pitch;
            MINI_CAM_STATE.yaw = yaw;
            MINI_CAM_STATE.roll = roll;
            MINI_CAM_STATE.focus_offset = v000;
            MINI_CAM_STATE.position = position;
            Minicam_ClearDeltas();
            MINI_CAM_CAMERA.mode = -1;
        } else {
            if (pitch_rate != 0)
                MINI_CAM_STATE.target_pitch = (i32)(pitch_rate * command_duration);
            else
                MINI_CAM_STATE.target_pitch = pitch;
            MINI_CAM_STATE.start_pitch = MINI_CAM_STATE.pitch;
            if (yaw_rate != 0)
                MINI_CAM_STATE.target_yaw = (i32)(yaw_rate * command_duration);
            else
                MINI_CAM_STATE.target_yaw = yaw;
            MINI_CAM_STATE.start_yaw = MINI_CAM_STATE.yaw;
            if (roll_rate != 0)
                MINI_CAM_STATE.target_roll = (i32)(roll_rate * command_duration);
            else
                MINI_CAM_STATE.target_roll = roll;
            MINI_CAM_STATE.target_distance = distance;
            MINI_CAM_STATE.start_roll = MINI_CAM_STATE.roll;
            MINI_CAM_STATE.start_distance = MINI_CAM_STATE.distance;
            if (MINI_CAM_STATE.focus != focus) {
                NuVecSub(&MINI_CAM_STATE.focus_offset, MINI_CAM_STATE.focus, focus);
                NuVecScale(&MINI_CAM_STATE.focus_velocity, &MINI_CAM_STATE.focus_offset, 1.0f / command_duration);
                MINI_CAM_STATE.focus = focus;
            }
            NuVecSub(&MINI_CAM_STATE.position_velocity, &position, &MINI_CAM_STATE.position);
            NuVecScale(&MINI_CAM_STATE.position_velocity, &MINI_CAM_STATE.position_velocity, 1.0f / command_duration);
        }
    }
    MINI_CAM_STATE.delta_time += FRAMETIME;
    if (MINI_CAM_STATE.delta_time >= command_duration) {
        Minicam_AddDeltas(command_duration);
        Minicam_ResetForNextCommand();
    } else {
        Minicam_AddDeltas(command_duration);
    }
    Minicam_CalcCamPos();
    return;
end_cut:
    Minicam_CalcCamPos();
    ObstacleCamEnd = 0.0f;
    if (ObstacleCamBlendOutTime == 0.0f) {
        MINI_CAM_CAMERA.mode = -1;
        Minicam_ResetForNextCommand();
    }
    if (MiniCutCam == 0)
        Minicam_ResetForNextCommand();
    return;
dispatch_command: {
    MINICAM_COMMAND_PC *command = &MINI_CAM_STATE.commands[MINI_CAM_STATE.current_command];
    switch (command->type) {
        case 0:
            return;
        case 1:
        case 3:
        case 4:
            command_duration = command->duration;
            goto execute_command;
        case 2:
            command_duration = 0.0f;
            goto execute_command;
        case 5:
            command_duration = command->duration;
            Minicam_ClearDeltas();
            goto execute_command;
        case 6: {
            u16 target_pitch, old_yaw, old_roll;
            f32 old_distance;
            MiniCam_ChangeMode(command->argument);
            target_pitch = pitch_rate != 0 ? (i32)(pitch_rate * 0.0f) : pitch;
            old_yaw = MINI_CAM_STATE.target_yaw;
            old_roll = MINI_CAM_STATE.target_roll;
            old_distance = MINI_CAM_STATE.target_distance;
            MINI_CAM_STATE.target_pitch = target_pitch;
            MINI_CAM_STATE.start_pitch = MINI_CAM_STATE.pitch;
            Minicam_ClearDeltas();
            if (pitch == target_pitch)
                pitch = MINI_CAM_STATE.target_pitch;
            if (yaw == old_yaw)
                yaw = MINI_CAM_STATE.target_yaw;
            if (roll == old_roll)
                roll = MINI_CAM_STATE.target_roll;
            if (distance == old_distance)
                distance = MINI_CAM_STATE.target_distance;
            break;
        }
        case 7:
            MINI_CAM_STATE.easing = command->argument;
            break;
        case 8:
            focus = command->target;
            break;
        case 9:
            distance = command->duration;
            break;
        case 10:
            pitch = command->argument;
            break;
        case 11:
            yaw = command->argument;
            break;
        case 12:
            roll = command->argument;
            break;
        case 13:
            pitch_rate = command->argument;
            break;
        case 14:
            yaw_rate = command->argument;
            break;
        case 15:
            roll_rate = command->argument;
            break;
        case 16:
            position = command->position;
            break;
        case 17:
            MINI_CAM_STATE.position_spline = (NUGSPLINE *)command->target;
            break;
        default:
            ++MINI_CAM_STATE.current_command;
            return;
    }
    ++MINI_CAM_STATE.current_command;
    goto read_command;
}
}

void __cdecl Minicam_InitSystem(void) {
    memset(&MiniCam, 0, sizeof(MiniCam));
}
