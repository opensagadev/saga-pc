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

STATIC_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);

typedef struct KEEP_SCREEN_CAMERA_FIELDS_PC {
    u8 unknown_000[0x110];
    NUVEC direction;
    u8 unknown_11c[0xdc];
    u16 pitch;
} KEEP_SCREEN_CAMERA_FIELDS_PC;
STATIC_ASSERT(offsetof(KEEP_SCREEN_CAMERA_FIELDS_PC, direction) == 0x110);
STATIC_ASSERT(offsetof(KEEP_SCREEN_CAMERA_FIELDS_PC, pitch) == 0x1f8);
extern i32 newgamecam;
extern i32 MiniCutCam;
extern i32 VehicleArea;
i32 __cdecl OnOrInsidePlane(NUVEC *, NUVEC *, NUVEC *, NUVEC *, f32, f32 *);
void __cdecl SpecialMove_Cancel(GAMEOBJECT *);

static void GunshipKeepOnScreenPc(GAMEOBJECT *object) {
    f32 push_speed = (f32)((f64)object->character_data->game_character->run_speed * 5.0);
    f32 count = 0.0f;
    NUVEC normal, response;
    if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[3].point, &PlayPlanePc[3].normal, NULL, 0.0f, NULL)) {
        NuVecRotateY(&normal, &GunshipANorm, 0x6000);
        Surface_Deflect(&normal, &object->velocity, &response, 0);
        response.x += normal.x * push_speed;
        response.z += normal.z * push_speed;
        count += 1.0f;
    }
    if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[2].point, &PlayPlanePc[2].normal, NULL, 0.0f, NULL)) {
        NuVecRotateY(&normal, &GunshipANorm, 0xa000);
        Surface_Deflect(&normal, &object->velocity, &response, 0);
        response.x += normal.x * push_speed;
        response.z += normal.z * push_speed;
        count += 1.0f;
    }
    if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[1].point, &PlayPlanePc[1].normal, NULL, 0.0f, NULL)) {
        NuVecRotateY(&normal, &GunshipANorm, 0x2000);
        Surface_Deflect(&normal, &object->velocity, &response, 0);
        response.x += normal.x * push_speed;
        response.z += normal.z * push_speed;
        count += 1.0f;
    }
    if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[4].point, &PlayPlanePc[4].normal, NULL, 0.0f, NULL)) {
        NuVecRotateY(&normal, &GunshipANorm, 0xe000);
        Surface_Deflect(&normal, &object->velocity, &response, 0);
        response.x += normal.x * push_speed;
        response.z += normal.z * push_speed;
        count += 1.0f;
    }
    if (count > 0.0f) {
        f32 inverse_count = 1.0f / count;
        object->movement_direction_x = response.x * inverse_count;
        object->movement_direction_z = response.z * inverse_count;
    }
}

void __cdecl KeepOnScreen(GAMEOBJECT *object) {
    f32 previous_keep_time = object->field_da8;
    i32 controlled = 0, index, two_players, constrained = 0;
    GAMEOBJECT *other_player;
    PLAYERCHARACTERCONFIG_PC *character;
    f32 push_distance;
    NUVEC movement;
    KEEP_SCREEN_PLANE_PC *plane;
    object->field_da8 = 0.0f;
    if (newgamecam != 0 || (object->flags_ef4 & 0x10000000) != 0)
        return;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *controller = Player[index];
        if (controller != NULL && controller->character_context == 0x51 && controller->techno != NULL &&
            controller->techno->controlled_object == object)
            controlled = 1;
    }
    if (!(object->flags_1fc & 0x80) && !controlled)
        return;
    if (object->dead != 0 || !(object->flags_ef0 & 0x40000))
        return;
    switch (object->character_context) {
        case 0x2b:
        case 0x0f:
        case 0x47:
        case 0x1f:
        case 0x46:
        case 0x51:
            return;
    }
    if (MiniCutCam != 0 || GetMenuID() != -1)
        return;
    if (WORLD->current_level == BONUS_GUNSHIPA_LDATA && GameCam->mode == 0x0b) {
        GunshipKeepOnScreenPc(object);
        return;
    }
    if (VehicleArea == 0) {
        other_player = object == Player[0] ? Player[1] : object == Player[1] ? Player[0] : NULL;
        if (other_player != NULL && (!(other_player->flags_1fc & 0x80) || other_player->character_context == 0x2b))
            return;
    }
    two_players = Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) && Player[1] != NULL &&
                  (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80);
    character = object->character_data->game_character;
    push_distance = VehicleArea != 0 ? character->walk_speed : character->run_speed;
    movement = object->velocity;
    if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[0].point, &PlayPlanePc[0].normal, NULL, 0.0f, NULL)) {
        Surface_Deflect(&PlayPlanePc[0].normal, &movement, &movement, 0);
        movement.x += PlayPlanePc[0].normal.x * push_distance;
        movement.z += PlayPlanePc[0].normal.z * push_distance;
        constrained = 1;
    }
    plane = NULL;
    if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[2].point, &PlayPlanePc[2].normal, NULL,
                        -object->bounds_radius, NULL)) {
        plane = &PlayPlanePc[2];
    } else if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[1].point, &PlayPlanePc[1].normal, NULL,
                               -object->bounds_radius, NULL)) {
        plane = &PlayPlanePc[1];
    }
    if (plane != NULL) {
        Surface_Deflect(&plane->normal, &object->velocity, &movement, 0);
        movement.x += plane->normal.x * push_distance;
        movement.z += plane->normal.z * push_distance;
        constrained = 1;
    }
    if ((VehicleArea != 0 && (WORLD->area == NULL || WORLD->area != PODSPRINT_ADATA)) || KEEPONSCREEN_SIDESONLY == 0 ||
        (two_players && abs(RotDiff(0, ((KEEP_SCREEN_CAMERA_FIELDS_PC *)GameCam)->pitch)) > 0x2000)) {
        plane = NULL;
        if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[4].point, &PlayPlanePc[4].normal, NULL,
                            -object->bounds_half_height, NULL)) {
            plane =
                !(((KEEP_SCREEN_CAMERA_FIELDS_PC *)GameCam)->direction.y <= 0.0f) ? &PlayPlanePc[5] : &PlayPlanePc[0];
        } else if (OnOrInsidePlane(&object->collision_position, &PlayPlanePc[3].point, &PlayPlanePc[3].normal, NULL,
                                   -object->bounds_half_height, NULL)) {
            plane =
                !(((KEEP_SCREEN_CAMERA_FIELDS_PC *)GameCam)->direction.y <= 0.0f) ? &PlayPlanePc[0] : &PlayPlanePc[5];
        }
        if (plane != NULL) {
            Surface_Deflect(&plane->normal, &object->velocity, &movement, 0);
            movement.x += plane->normal.x * push_distance;
            movement.z += plane->normal.z * push_distance;
            constrained = 1;
        }
    }
    if (constrained) {
        object->movement_direction_x = movement.x;
        object->movement_direction_z = movement.z;
        object->field_da8 = previous_keep_time + FRAMETIME;
        SpecialMove_Cancel(object);
    }
}

void __cdecl KeepPointOnScreen(NUVEC *position, NUVEC *velocity) {
    if ((f64)position->x < (f64)-0.85f) {
        position->x = -0.85f;
        if (velocity != NULL)
            velocity->x = 0.0f;
    } else if ((f64)position->x > (f64)0.85f) {
        position->x = 0.85f;
        if (velocity != NULL)
            velocity->x = 0.0f;
    }
    if ((f64)position->y < (f64)-0.85f) {
        position->y = -0.85f;
        if (velocity != NULL)
            velocity->y = 0.0f;
    } else if ((f64)position->y > (f64)0.85f) {
        position->y = 0.85f;
        if (velocity != NULL)
            velocity->y = 0.0f;
    }
}
