#include "saga.h"

typedef struct CAMERA_JUDDER_FIELDS_PC {
    u8 unknown_000[0x11c];
    NUVEC position;
} CAMERA_JUDDER_FIELDS_PC;
STATIC_ASSERT(offsetof(CAMERA_JUDDER_FIELDS_PC, position) == 0x11c);
typedef struct LEVEL_JUDDER_FIELDS_PC {
    u8 unknown_000[0xcd];
    u8 maximum_distance;
} LEVEL_JUDDER_FIELDS_PC;
STATIC_ASSERT(offsetof(LEVEL_JUDDER_FIELDS_PC, maximum_distance) == 0xcd);

void __cdecl GameCam_Judder(GAMECAMERA *camera, f32 amount, i32 axis, NUVEC *source) {
    union {
        f32 value;
        u32 bits;
    } absolute, magnitude;
    f32 distance;
    f32 maximum_distance;
    if (camera == NULL)
        camera = GameCam;
    absolute.value = amount;
    magnitude.bits = absolute.bits & 0x7fffffff;
    if (magnitude.value > camera->judder_time) {
        if (source != NULL) {
            distance = NuVecDist(&((CAMERA_JUDDER_FIELDS_PC *)camera)->position, source, NULL);
            maximum_distance = (f32)((LEVEL_JUDDER_FIELDS_PC *)WORLD->current_level)->maximum_distance;
            if (distance < maximum_distance) {
                magnitude.value *= (maximum_distance - distance) / maximum_distance;
                if (magnitude.value > camera->judder_time) {
                    camera->judder_duration = magnitude.value;
                    camera->judder_time = magnitude.value;
                    if (amount < 0.0f)
                        camera->judder_reverse = 1;
                    else
                        camera->judder_reverse = 0;
                }
            }
        } else {
            camera->judder_duration = magnitude.value;
            camera->judder_time = magnitude.value;
            if (amount < 0.0f)
                camera->judder_reverse = 1;
            else
                camera->judder_reverse = 0;
        }
        camera->judder_axis = (u8)axis;
    }
}

void __cdecl GameCam_Blend(GAMECAMERA *camera, f32 duration, f32 curve, i32 mode) {
    if (camera == NULL)
        camera = GameCam;
    if (!(duration > 0.0f) || camera->mode == -1)
        return;
    camera->blend_start_pitch = camera->desired_pitch;
    camera->blend_curve = curve;
    camera->blend_time = 0.0f;
    camera->blend_duration = duration;
    camera->blend_start_yaw = camera->desired_yaw;
    camera->blend_start_roll = camera->desired_roll;
    camera->blend_mode = mode != 0 ? 2 : 1;
    camera->previous_camera_mode = camera->previous_mode;
    camera->blend_start_position = camera->desired_position;
    camera->blend_start_target = camera->target;
    camera->blend_end_position = camera->desired_position;
    camera->reset_blend = 1;
    camera->blend_end_target = camera->target;
}

u16 __cdecl GameCam_GetAdjustedYRot(GAMECAMERA *camera) {
    if (camera == NULL)
        camera = GameCam;
    return (u16)(camera->input_yaw + (u16)camera->adjusted_yaw_offset);
}

void __cdecl GameCam_HitRoll(void) {
    f32 amount = qrand() < 0x8000 ? -0.25f : 0.25f;
    GameCam_Judder(GameCam, amount, 2, NULL);
}

void NarrowSockExceptions_Init(NARROWSOCKEXCEPTION_PC *exceptions) {
    i32 level_index;
    if (exceptions == NULL)
        return;
    NarrowSockException = exceptions;
    for (; exceptions->level_name != NULL; ++exceptions) {
        Level_FindByName(exceptions->level_name, &level_index);
        exceptions->level_index = level_index;
    }
}

void __cdecl GameCam_HitJudder(void) {
    f32 amount = VehicleArea != 0 ? -0.3f : -0.2f;
    if (qrand() < 0x8000)
        amount = -amount;
    GameCam_Judder(GameCam, amount, qrand() / 0x5556, NULL);
}

static i32 GameCamLookRotXPc = 10;
static i32 GameCamLookRotYPc = 15;
extern NUVEC PlayPlaneCornersPc[4];
extern f32 PlayPlaneDepthPc;
extern f32 GameCamShakeRadiusPc;
extern f32 GameCamShakeRetargetPc;
extern f32 GameCamShakeDirSeekPc;
extern f32 GameCamShakeOffsetSeekPc;
extern i32 PlayPlanesUseRenderMtxPc;

__declspec(noinline) void __cdecl GameCam_ResetLookRot(GAMECAMERA *camera) {
    if (camera == NULL)
        camera = GameCam;
    camera->field_214 = 0.0f;
    camera->adjusted_yaw_offset = 0.0f;
    camera->field_20c = 0.0f;
    camera->field_210 = 0.0f;
    camera->field_204 = 0.0f;
    camera->field_208 = 0.0f;
}

void __cdecl GameCam_UpdateLookRot(GAMECAMERA *camera) {
    i32 i;
    f32 players;
    if (camera == NULL)
        camera = GameCam;
    camera->field_204 = 0.0f;
    camera->field_208 = 0.0f;
    if (MiniCutCam == 0) {
        players = 0.0f;
        for (i = 0; i < 2; i++) {
            if (Player[i] != NULL && (((GAMEOBJECT *)Player[i])->flags_1fc & 0x80) != 0 &&
                ((GAMEOBJECT *)Player[i])->pad->bound_pad != NULL) {
                i32 look = 1;
                f32 x, y;
                if (AlternateGameCallbackPc != NULL)
                    look = AlternateGameCallbackPc((GAMEOBJECT *)Player[i]);
                if (look == 2) {
                    GAMEPAD_PC *pad = ((GAMEOBJECT *)Player[i])->pad;
                    if (pad->input_mode == 1) {
                        u32 buttons = pad->buttons_held;
                        u32 horizontal = buttons & (GAMEPAD_DLEFT | GAMEPAD_DRIGHT);
                        u32 vertical;
                        if (horizontal == GAMEPAD_DLEFT)
                            x = -1.0f;
                        else if (horizontal == GAMEPAD_DRIGHT)
                            x = 1.0f;
                        else
                            x = 0.0f;
                        vertical = buttons & (GAMEPAD_DUP | GAMEPAD_DDOWN);
                        if (vertical == GAMEPAD_DUP)
                            y = -1.0f;
                        else if (vertical == GAMEPAD_DDOWN)
                            y = 1.0f;
                        else
                            y = 0.0f;
                    } else {
                        x = (f32)((GAMEOBJECT *)Player[i])->pad->bound_pad->analog_left_x - 127.5f;
                        x *= 1.0f / 127.5f;
                        y = (f32)((GAMEOBJECT *)Player[i])->pad->bound_pad->analog_left_y - 127.5f;
                        y *= 1.0f / 127.5f;
                    }
                    camera->field_204 += (f32)((GameCamLookRotXPc << 16) / 360) * y;
                    camera->field_208 += (f32)((GameCamLookRotYPc << 16) / 360) * x;
                    players += 1.0f;
                } else if (look == 1) {
                    FRAME_PAD_PC *bound_pad = ((GAMEOBJECT *)Player[i])->pad->bound_pad;
                    x = (f32)bound_pad->analog_right_x - 127.5f;
                    x *= 1.0f / 127.5f;
                    y = (f32)bound_pad->analog_right_y - 127.5f;
                    y *= 1.0f / 127.5f;
                    camera->field_204 += y * (f32)((GameCamLookRotXPc << 16) / 360);
                    camera->field_208 += (f32)((GameCamLookRotYPc << 16) / 360) * x;
                    players += 1.0f;
                }
            }
            if (players > 1.0f) {
                f32 scale = 1.0f / players;
                camera->field_204 *= scale;
                camera->field_208 *= scale;
            }
        }
    }
    camera->field_20c =
        SeekLinearF(camera->field_20c, camera->field_204, (f32)((GameCamLookRotXPc << 16) / 360) * FRAMETIME * 2.0f);
    camera->field_210 =
        SeekLinearF(camera->field_210, camera->field_208, (f32)((GameCamLookRotYPc << 16) / 360) * FRAMETIME * 2.0f);
    camera->field_214 = SeekValF(camera->field_214, camera->field_20c, 3.0f);
    camera->adjusted_yaw_offset = SeekValF(camera->adjusted_yaw_offset, camera->field_210, 3.0f);
}

typedef struct PLAY_CORNER_PC {
    NUVEC near_point;
    NUVEC far_point;
} PLAY_CORNER_PC;

static void MakePlayPlane(KEEP_SCREEN_PLANE_PC *plane, NUVEC *a, NUVEC *b, NUVEC *c, NUVEC *d) {
    NUVEC first_edge;
    NUVEC second_edge;
    plane->point.x = (a->x + b->x + c->x + d->x) * 0.25f;
    plane->point.y = (a->y + b->y + c->y + d->y) * 0.25f;
    plane->point.z = (a->z + b->z + c->z + d->z) * 0.25f;
    NuVecSub(&first_edge, b, a);
    NuVecSub(&second_edge, c, a);
    NuVecCross(&plane->normal, &first_edge, &second_edge);
    NuVecNorm(&plane->normal, &plane->normal);
}

void __cdecl MakePlayPlanes(GAMECAMERA *camera) {
    NUVEC ray;
    PLAY_CORNER_PC corners[4];
    i32 i;
    for (i = 0; i < 4; i++) {
        ray.x = PlayPlaneCornersPc[i].x * PANEL3DMULX;
        ray.y = PlayPlaneCornersPc[i].y * PANEL3DMULY;
        ray.z = PlayPlaneCornersPc[i].z;
        NuVecMtxRotate(&ray, &ray, PlayPlanesUseRenderMtxPc != 0 ? &camera->render_mtx : &camera->target_mtx);
        NuVecAdd(&corners[i].near_point, &ray, &camera->position);
        NuVecNorm(&ray, &ray);
        corners[i].far_point.x = corners[i].near_point.x + PlayPlaneDepthPc * ray.x;
        corners[i].far_point.y = (PlayPlaneDepthPc * ray.y) + corners[i].near_point.y;
        corners[i].far_point.z = corners[i].near_point.z + PlayPlaneDepthPc * ray.z;
    }
    // Plane order is near, left, right, top, bottom, far.
    MakePlayPlane(&PlayPlanePc[1], &corners[0].near_point, &corners[0].far_point, &corners[2].far_point,
                  &corners[2].near_point);
    MakePlayPlane(&PlayPlanePc[2], &corners[1].far_point, &corners[1].near_point, &corners[3].near_point,
                  &corners[3].far_point);
    MakePlayPlane(&PlayPlanePc[3], &corners[0].near_point, &corners[1].near_point, &corners[1].far_point,
                  &corners[0].far_point);
    MakePlayPlane(&PlayPlanePc[4], &corners[2].near_point, &corners[2].far_point, &corners[3].far_point,
                  &corners[3].near_point);
    MakePlayPlane(&PlayPlanePc[0], &corners[0].near_point, &corners[2].near_point, &corners[3].near_point,
                  &corners[1].near_point);
    MakePlayPlane(&PlayPlanePc[5], &corners[0].far_point, &corners[1].far_point, &corners[3].far_point,
                  &corners[2].far_point);
}

void __cdecl GameCam_UpdateShake(GAMECAMERA *camera, f32 ambient_amount) {
    NUVEC offset;
    f32 amount;
    f32 rate;
    if (camera == NULL)
        camera = GameCam;
    if (camera->shake_decay > 0.0f)
        camera->shake_decay -= FRAMETIME;
    if (camera->shake_decay > 0.0f)
        amount = camera->shake_intensity;
    else
        amount = ambient_amount > 0.0f ? ambient_amount : 0.0f;
    camera->shake_amplitude = SeekLinearF(camera->shake_amplitude, amount, FRAMETIME * 2.0f);
    if (NuVecDistSqr(&camera->shake_direction, &camera->shake_target, NULL) <
        GameCamShakeRetargetPc * GameCamShakeRetargetPc) {
        camera->shake_target.x = 0.0f;
        camera->shake_target.y = qrand() * (1.0f / 65535.0f) * GameCamShakeRadiusPc;
        camera->shake_target.z = 0.0f;
        NuVecRotateZ(&camera->shake_target, &camera->shake_target, qrand());
        camera->shake_target.x *= 4.0f / 3.0f;
    }
    rate = GameCamShakeDirSeekPc;
    if (camera->shake_decay > 0.0f)
        rate = GameCamShakeDirSeekPc * camera->shake_freq;
    SeekVec(&camera->shake_direction, &camera->shake_direction, &camera->shake_target, rate);
    rate = GameCamShakeOffsetSeekPc;
    if (camera->shake_decay > 0.0f)
        rate = GameCamShakeOffsetSeekPc * camera->shake_freq;
    SeekVec(&camera->shake_offset, &camera->shake_offset, &camera->shake_direction, rate);
    NuVecScale(&offset, &camera->shake_offset, camera->shake_amplitude);
    NuMtxPreRotateX(&camera->render_mtx, (i32)(offset.y / GameCamShakeRadiusPc * (camera->shake_amplitude * 910.0f)));
    NuMtxPreRotateY(&camera->render_mtx, (i32)(offset.x / GameCamShakeRadiusPc * (camera->shake_amplitude * 910.0f)));
}

void __cdecl CentreTwoPlayerCamera(NUVEC *center, NUVEC *player_a, NUVEC *player_b, NUVEC *reference) {
    f32 distances[2];
    f32 blend;
    distances[0] = NuVecDist(reference, player_a, NULL);
    distances[1] = NuVecDist(reference, player_b, NULL);
    blend = distances[0] / (distances[1] + distances[0]);
    center->x = (player_b->x - player_a->x) * blend + player_a->x;
    center->y = (player_b->y - player_a->y) * blend + player_a->y;
    center->z = (player_b->z - player_a->z) * blend + player_a->z;
}

void __cdecl ViewCamDraw(void) {
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

void __cdecl GameCam_Reset(GAMECAMERA *camera) {
    if (camera == NULL)
        camera = GameCam;
    camera->previous_mode = -1;
    camera->mode = -1;
    camera->sock_position.segment = -1;
    camera->sock_position.sock_index = -1;
    camera->judder_time = 0.0f;
    camera->shake_amplitude = 0.0f;
    camera->shake_intensity = 0.0f;
    camera->shake_decay = 0.0f;
    camera->shake_freq = 1.0f;
    camera->blend_time = camera->blend_duration = 0.0f;
    ObstacleCamSpl = NULL;
    MiniCutCam = 0;
    camera->position_seek = (f32)(u8)WORLD->current_level->cam_pos_seek;
    camera->angle_seek = (f32)(u8)WORLD->current_level->cam_angle_seek;
    camera->field_1b8 = 0;
    camera->field_1e8 = camera->field_1ec = 0.0f;
    camera->field_1f0 = camera->field_1f4 = 0.0f;
    camera->adjusted_yaw_offset = camera->field_214 = 0.0f;
    camera->field_210 = camera->field_20c = 0.0f;
    camera->field_208 = camera->field_204 = 0.0f;
    Minicam_InitSystem();
}

typedef struct CAMERA_CONTEXT_PATH_PC {
    u8 unknown_00[0x40];
    struct nugspline_s *path;
    u8 unknown_44[8];
    u8 flags;
} CAMERA_CONTEXT_PATH_PC;
STATIC_ASSERT(offsetof(CAMERA_CONTEXT_PATH_PC, path) == 0x40);
STATIC_ASSERT(offsetof(CAMERA_CONTEXT_PATH_PC, flags) == 0x4c);
typedef struct CAMERA_CONTEXT_FOCUS_PC {
    u8 unknown_00[0x28];
    NUVEC focus;
} CAMERA_CONTEXT_FOCUS_PC;
STATIC_ASSERT(offsetof(CAMERA_CONTEXT_FOCUS_PC, focus) == 0x28);
typedef struct CAMERA_CONTEXT_BUILDIT_PC {
    u8 unknown_00[0x34];
    NUVEC position;
} CAMERA_CONTEXT_BUILDIT_PC;
STATIC_ASSERT(offsetof(CAMERA_CONTEXT_BUILDIT_PC, position) == 0x34);
typedef struct CAMERA_GIZFORCE_TARGET_PC {
    u8 unknown_00[0x1c];
    NUVEC position;
} CAMERA_GIZFORCE_TARGET_PC;
STATIC_ASSERT(offsetof(CAMERA_GIZFORCE_TARGET_PC, position) == 0x1c);
typedef struct CAMERA_CABLE_PC {
    GAMEOBJECT *source;
    GAMEOBJECT *target;
} CAMERA_CABLE_PC;
STATIC_ASSERT(offsetof(CAMERA_CABLE_PC, target) == 0x4);
typedef struct CAMERA_GRABBER_PC {
    u8 unknown_000[0x48c];
    NUVEC grab_position;
} CAMERA_GRABBER_PC;
STATIC_ASSERT(offsetof(CAMERA_GRABBER_PC, grab_position) == 0x48c);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, previous_position) == 0x1c0);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1010) == 0x1010);
STATIC_ASSERT(offsetof(GAMEOBJECT, cable) == 0x1040);
STATIC_ASSERT(offsetof(GAMEOBJECT, gizforce_target) == 0x10cc);
STATIC_ASSERT(offsetof(GAMEOBJECT, force_target) == 0x790);
STATIC_ASSERT(offsetof(GAMEOBJECT, force_part) == 0xcb8);
STATIC_ASSERT(offsetof(GAMEOBJECT, techno) == 0x798);
STATIC_ASSERT(offsetof(WORLDINFO, grabber) == 0x46ec);

#define GAME_CAMERA_ABSOLUTE_PC(destination, source)                                                                   \
    do {                                                                                                               \
        union {                                                                                                        \
            f32 value;                                                                                                 \
            u32 bits;                                                                                                  \
        } storage;                                                                                                     \
        u32 bits;                                                                                                      \
        storage.value = (source);                                                                                      \
        bits = storage.bits;                                                                                           \
        bits &= 0x7fffffff;                                                                                            \
        storage.bits = bits;                                                                                           \
        (destination) = storage.value;                                                                                 \
    } while (0)

static void __cdecl SetCameraEffectMatrixPc(void) {
    NUMTX effect_matrix;
    NUVEC scale;
    scale.x = 0.5f;
    scale.y = 0.5f;
    scale.z = 0.5f;
    NuMtxInvR(&effect_matrix, &global_camera.mtx);
    NuMtxScale(&effect_matrix, &scale);
    effect_matrix.m30 = 0.5f;
    effect_matrix.m31 = 0.5f;
    effect_matrix.m32 = 0.0f;
    effect_matrix.m33 = 1.0f;
    effect_matrix.m23 = 0.0f;
    effect_matrix.m13 = 0.0f;
    effect_matrix.m03 = 0.0f;
    NuRndrSetFxMtx(&effect_matrix);
}

void __cdecl SetCameraMatrices(void) {
    PCGlobalLightingState.specular_direction_missing = 1;
    PCGlobalLightingState.specular_intensity_present = 0;
    NuRndrSetSpecularLightPS(NULL, NULL);
    SetCameraEffectMatrixPc();
}

#undef GAME_CAMERA_ABSOLUTE_PC
