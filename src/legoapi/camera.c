#include "gameapi/edtools/edui.h"
#include "globals.h"
#include "legoapi/ai/game/creature.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/cutscenes/minicamcut.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/camera.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

typedef struct CAMERA_JUDDER_FIELDS { // size 0x128
    u8 unknown_000[0x11c];            // 0x000
    NUVEC position;                   // 0x11c
} CAMERA_JUDDER_FIELDS;

typedef struct LEVEL_JUDDER_FIELDS { // size 0xce
    u8 unknown_000[0xcd];            // 0x00
    u8 maximum_distance;             // 0xcd
} LEVEL_JUDDER_FIELDS;

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 LegoapiCameraFabs(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

void GameCam_Judder(GAMECAMERA *camera, f32 amount, i32 axis, NUVEC *source) {
    union {
        f32 value;
        u32 bits;
    } magnitude;

    f32 distance;
    f32 maximum_distance;
    if (camera == NULL) {
        camera = GameCam;
    }
    magnitude.value = LegoapiCameraFabs(amount);
    if (magnitude.value > camera->judder_time) {
        if (source != NULL) {
            distance = NuVecDist(&((CAMERA_JUDDER_FIELDS *)camera)->position, source, NULL);
            maximum_distance = (f32)((LEVEL_JUDDER_FIELDS *)WORLD->current_level)->maximum_distance;
            if (distance < maximum_distance) {
                magnitude.value *= (maximum_distance - distance) / maximum_distance;
                if (magnitude.value > camera->judder_time) {
                    camera->judder_duration = magnitude.value;
                    camera->judder_time = magnitude.value;
                    if (amount < 0.0f) {
                        camera->judder_reverse = 1;
                    } else {
                        camera->judder_reverse = 0;
                    }
                }
            }
        } else {
            camera->judder_duration = magnitude.value;
            camera->judder_time = magnitude.value;
            if (amount < 0.0f) {
                camera->judder_reverse = 1;
            } else {
                camera->judder_reverse = 0;
            }
        }
        camera->judder_axis = (u8)axis;
    }
}

void GameCam_Blend(GAMECAMERA *camera, f32 duration, f32 curve, i32 mode) {
    if (camera == NULL) {
        camera = GameCam;
    }
    if (!(duration > 0.0f) || camera->mode == GAMECAM_MODE_NONE) {
        return;
    }
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

u16 GameCam_GetAdjustedYRot(GAMECAMERA *camera) {
    if (camera == NULL) {
        camera = GameCam;
    }
    return (u16)(camera->input_yaw + (u16)camera->adjusted_yaw_offset);
}

void GameCam_HitRoll(void) {
    f32 amount = qrand() < 0x8000 ? -0.25f : 0.25f;
    GameCam_Judder(GameCam, amount, 2, NULL);
}

NARROWSOCKEXCEPTION *NarrowSockException = 0;

void NarrowSockExceptions_Init(NARROWSOCKEXCEPTION *exceptions) {
    i32 level_index;
    if (exceptions == NULL) {
        return;
    }
    NarrowSockException = exceptions;
    for (; exceptions->level_name != NULL; ++exceptions) {
        Level_FindByName(exceptions->level_name, &level_index);
        exceptions->level_index = level_index;
    }
}

void GameCam_HitJudder(void) {
    f32 amount = VehicleArea != 0 ? -0.3f : -0.2f;
    if (qrand() < 0x8000) {
        amount = -amount;
    }
    GameCam_Judder(GameCam, amount, qrand() / 0x5556, NULL);
}

static i32 GameCamLookRotX = 10;
static i32 GameCamLookRotY = 15;
NUVEC PlayPlaneCorners[4] = {
    {-0.85f, 0.85f,  1.0f},
    {0.85f,  0.85f,  1.0f},
    {-0.85f, -0.85f, 1.0f},
    {0.85f,  -0.85f, 1.0f}
};
f32 PP_dist = 5.0f;
f32 GameCamShakeRadius = 0.1f;
f32 GameCamShakeRetarget = 0.025f;
f32 GameCamShakeDirSeek = 10.0f;
f32 GameCamShakeOffsetSeek = 2.0f;
i32 GAMEPAD_DUP = PAD_BUTTON_UP;
i32 GAMEPAD_DDOWN = PAD_BUTTON_DOWN;
i32 GAMEPAD_DLEFT = PAD_BUTTON_LEFT;
i32 GAMEPAD_DRIGHT = PAD_BUTTON_RIGHT;
extern i32 PlayPlanesUseRenderMtx;

void GameCam_ResetLookRot(GAMECAMERA *camera) {
    if (camera == NULL) {
        camera = GameCam;
    }
    camera->unknown_214 = 0.0f;
    camera->adjusted_yaw_offset = 0.0f;
    camera->unknown_20c = 0.0f;
    camera->unknown_210 = 0.0f;
    camera->unknown_204 = 0.0f;
    camera->unknown_208 = 0.0f;
}

void GameCam_UpdateLookRot(GAMECAMERA *camera) {
    i32 i;
    f32 players;
    if (camera == NULL) {
        camera = GameCam;
    }
    camera->unknown_204 = 0.0f;
    camera->unknown_208 = 0.0f;
    if (MiniCutCam == 0) {
        players = 0.0f;
        for (i = 0; i < 2; i++) {
            if (Player[i] != NULL && (Player[i]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && Player[i]->pad->bound_pad != NULL) {
                i32 look = 1;
                f32 x;
                f32 y;
                if (AlternateGameCallback != NULL) {
                    look = AlternateGameCallback(Player[i]);
                }
                if (look == 2) {
                    GAMEPAD *pad = Player[i]->pad;
                    if (pad->input_mode == 1) {
                        u32 buttons = pad->buttons_held;
                        u32 horizontal = buttons & (GAMEPAD_DLEFT | GAMEPAD_DRIGHT);
                        u32 vertical;
                        if (horizontal == GAMEPAD_DLEFT) {
                            x = -1.0f;
                        } else if (horizontal == GAMEPAD_DRIGHT) {
                            x = 1.0f;
                        } else {
                            x = 0.0f;
                        }
                        vertical = buttons & (GAMEPAD_DUP | GAMEPAD_DDOWN);
                        if (vertical == GAMEPAD_DUP) {
                            y = -1.0f;
                        } else if (vertical == GAMEPAD_DDOWN) {
                            y = 1.0f;
                        } else {
                            y = 0.0f;
                        }
                    } else {
                        x = (f32)Player[i]->pad->bound_pad->analog_left_x - 127.5f;
                        x *= 1.0f / 127.5f;
                        y = (f32)Player[i]->pad->bound_pad->analog_left_y - 127.5f;
                        y *= 1.0f / 127.5f;
                    }
                    camera->unknown_204 += (f32)((GameCamLookRotX << 16) / 360) * y;
                    camera->unknown_208 += (f32)((GameCamLookRotY << 16) / 360) * x;
                    players += 1.0f;
                } else if (look == 1) {
                    FRAME_PAD *bound_pad = Player[i]->pad->bound_pad;
                    x = (f32)bound_pad->analog_right_x - 127.5f;
                    x *= 1.0f / 127.5f;
                    y = (f32)bound_pad->analog_right_y - 127.5f;
                    y *= 1.0f / 127.5f;
                    camera->unknown_204 += y * (f32)((GameCamLookRotX << 16) / 360);
                    camera->unknown_208 += (f32)((GameCamLookRotY << 16) / 360) * x;
                    players += 1.0f;
                }
            }
            if (players > 1.0f) {
                f32 scale = 1.0f / players;
                camera->unknown_204 *= scale;
                camera->unknown_208 *= scale;
            }
        }
    }
    camera->unknown_20c = SeekLinearF(camera->unknown_20c, camera->unknown_204, (f32)((GameCamLookRotX << 16) / 360) * FRAMETIME * 2.0f);
    camera->unknown_210 = SeekLinearF(camera->unknown_210, camera->unknown_208, (f32)((GameCamLookRotY << 16) / 360) * FRAMETIME * 2.0f);
    camera->unknown_214 = SeekValF(camera->unknown_214, camera->unknown_20c, 3.0f);
    camera->adjusted_yaw_offset = SeekValF(camera->adjusted_yaw_offset, camera->unknown_210, 3.0f);
}

typedef struct PLAY_CORNER {
    NUVEC near_point;
    NUVEC far_point;
} PLAY_CORNER;

static void MakePlayPlane(KEEP_SCREEN_PLANE *plane, NUVEC *a, NUVEC *b, NUVEC *c, NUVEC *d) {
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

i32 (*AlternateGameCallback)(GAMEOBJECT *object) = NULL;
i32 PlayPlanesUseRenderMtx = 0;

void MakePlayPlanes(GAMECAMERA *camera) {
    NUVEC ray;
    PLAY_CORNER corners[4];
    i32 i;
    for (i = 0; i < 4; i++) {
        ray.x = PlayPlaneCorners[i].x * PANEL3DMULX;
        ray.y = PlayPlaneCorners[i].y * PANEL3DMULY;
        ray.z = PlayPlaneCorners[i].z;
        NuVecMtxRotate(&ray, &ray, PlayPlanesUseRenderMtx != 0 ? &camera->render_mtx : &camera->target_mtx);
        NuVecAdd(&corners[i].near_point, &ray, &camera->position);
        NuVecNorm(&ray, &ray);
        corners[i].far_point.x = corners[i].near_point.x + PP_dist * ray.x;
        corners[i].far_point.y = (PP_dist * ray.y) + corners[i].near_point.y;
        corners[i].far_point.z = corners[i].near_point.z + PP_dist * ray.z;
    }
    // Plane order is near, left, right, top, bottom, far.
    MakePlayPlane(&PlayPlane[1], &corners[0].near_point, &corners[0].far_point, &corners[2].far_point, &corners[2].near_point);
    MakePlayPlane(&PlayPlane[2], &corners[1].far_point, &corners[1].near_point, &corners[3].near_point, &corners[3].far_point);
    MakePlayPlane(&PlayPlane[3], &corners[0].near_point, &corners[1].near_point, &corners[1].far_point, &corners[0].far_point);
    MakePlayPlane(&PlayPlane[4], &corners[2].near_point, &corners[2].far_point, &corners[3].far_point, &corners[3].near_point);
    MakePlayPlane(&PlayPlane[0], &corners[0].near_point, &corners[2].near_point, &corners[3].near_point, &corners[1].near_point);
    MakePlayPlane(&PlayPlane[5], &corners[0].far_point, &corners[1].far_point, &corners[3].far_point, &corners[2].far_point);
}

void GameCam_UpdateShake(GAMECAMERA *camera, f32 ambient_amount) {
    NUVEC offset;
    f32 amount;
    f32 rate;
    if (camera == NULL) {
        camera = GameCam;
    }
    if (camera->shake_decay > 0.0f) {
        camera->shake_decay -= FRAMETIME;
    }
    if (camera->shake_decay > 0.0f) {
        amount = camera->shake_intensity;
    } else {
        amount = ambient_amount > 0.0f ? ambient_amount : 0.0f;
    }
    camera->shake_amplitude = SeekLinearF(camera->shake_amplitude, amount, FRAMETIME * 2.0f);
    if (NuVecDistSqr(&camera->shake_direction, &camera->shake_target, NULL) < GameCamShakeRetarget * GameCamShakeRetarget) {
        camera->shake_target.x = 0.0f;
        camera->shake_target.y = qrand() * (1.0f / 65535.0f) * GameCamShakeRadius;
        camera->shake_target.z = 0.0f;
        NuVecRotateZ(&camera->shake_target, &camera->shake_target, qrand());
        camera->shake_target.x *= 4.0f / 3.0f;
    }
    rate = GameCamShakeDirSeek;
    if (camera->shake_decay > 0.0f) {
        rate = GameCamShakeDirSeek * camera->shake_freq;
    }
    SeekVec(&camera->shake_direction, &camera->shake_direction, &camera->shake_target, rate);
    rate = GameCamShakeOffsetSeek;
    if (camera->shake_decay > 0.0f) {
        rate = GameCamShakeOffsetSeek * camera->shake_freq;
    }
    SeekVec(&camera->shake_offset, &camera->shake_offset, &camera->shake_direction, rate);
    NuVecScale(&offset, &camera->shake_offset, camera->shake_amplitude);
    NuMtxPreRotateX(&camera->render_mtx, (i32)(offset.y / GameCamShakeRadius * (camera->shake_amplitude * 910.0f)));
    NuMtxPreRotateY(&camera->render_mtx, (i32)(offset.x / GameCamShakeRadius * (camera->shake_amplitude * 910.0f)));
}

void CentreTwoPlayerCamera(NUVEC *center, NUVEC *player_a, NUVEC *player_b, NUVEC *reference) {
    f32 distances[2];
    f32 blend;
    distances[0] = NuVecDist(reference, player_a, NULL);
    distances[1] = NuVecDist(reference, player_b, NULL);
    blend = distances[0] / (distances[1] + distances[0]);
    center->x = (player_b->x - player_a->x) * blend + player_a->x;
    center->y = (player_b->y - player_a->y) * blend + player_a->y;
    center->z = (player_b->z - player_a->z) * blend + player_a->z;
}

void ViewCamDraw(void) {
}

void GameCam_Reset(GAMECAMERA *camera) {
    if (camera == NULL) {
        camera = GameCam;
    }
    camera->previous_mode = GAMECAM_MODE_NONE;
    camera->mode = GAMECAM_MODE_NONE;
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
    camera->position_seek = (f32)WORLD->current_level->cam_pos_seek;
    camera->angle_seek = (f32)WORLD->current_level->cam_angle_seek;
    camera->unknown_1b8 = 0;
    camera->unknown_1e8 = camera->unknown_1ec = 0.0f;
    camera->unknown_1f0 = camera->unknown_1f4 = 0.0f;
    camera->adjusted_yaw_offset = camera->unknown_214 = 0.0f;
    camera->unknown_210 = camera->unknown_20c = 0.0f;
    camera->unknown_208 = camera->unknown_204 = 0.0f;
    Minicam_InitSystem();
}

typedef struct CAMERA_CABLE { // size 0x8
    GAMEOBJECT *source;       // 0x0
    GAMEOBJECT *target;       // 0x4
} CAMERA_CABLE;

#define GAME_CAMERA_ABSOLUTE(destination, source)                                                                                          \
    do {                                                                                                                                   \
        union {                                                                                                                            \
            f32 value;                                                                                                                     \
            u32 bits;                                                                                                                      \
        } storage;                                                                                                                         \
        u32 bits;                                                                                                                          \
        storage.value = (source);                                                                                                          \
        bits = storage.bits;                                                                                                               \
        bits &= 0x7fffffff;                                                                                                                \
        storage.bits = bits;                                                                                                               \
        (destination) = storage.value;                                                                                                     \
    } while (0)

static void SetCameraEffectMatrix(void) {
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

void SetCameraMatrices(void) {
    PCGlobalLightingState.specular_direction_missing = 1;
    PCGlobalLightingState.specular_intensity_present = 0;
    NuRndrSetSpecularLightPS(NULL, NULL);
    SetCameraEffectMatrix();
}

#undef GAME_CAMERA_ABSOLUTE

static i32 NarrowSockPositionAllowed(SOCKSYS *system, SOCKPOSITION *position, i32 level_index) {
    NARROWSOCKEXCEPTION *exception;
    NARROWSOCKEXCEPTION *list;
    if (system == NULL || position->sock_index == -1) {
        return 0;
    }
    if ((system->socks[position->sock_index].flags & SOCK_FLAG_MISC_1) == 0) {
        return 0;
    }
    list = NarrowSockException;
    if (list != NULL) {
        for (exception = list; exception->level_name != NULL; ++exception) {
            if (exception->level_index == -1 || exception->level_index != level_index || position->sock_index != exception->sock_index) {
                continue;
            }
            if (position->distance >= exception->start_distance && position->distance <= exception->end_distance) {
                return 0;
            }
        }
    }
    return 1;
}

i32 disable_narrow_socks = 0;

i32 ObjInNarrowSock(GAMEOBJECT *object, SOCKSYS *system, i32 level_index) {
    GAMEOBJECT *other = NULL;
    if ((WORLD->current_level->flags & LEVEL_NARROW_SOCKS) == 0 && VehicleArea == 0) {
        return 0;
    }
    if (disable_narrow_socks != 0) {
        return 0;
    }
    if (object->player_index != -1) {
        if ((object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
            if (object == Player[0]) {
                other = Player[1];
            } else if (object == Player[1]) {
                other = Player[0];
            }
            if (other != NULL && (other->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 &&
                !NarrowSockPositionAllowed(system, &other->sock_position, level_index)) {
                return 0;
            }
        }
        if (NarrowSockPositionAllowed(system, &object->sock_position, level_index)) {
            return 1;
        }
    }
    return 0;
}
