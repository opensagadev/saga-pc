#include "saga.h"

/* Retail unit 0x40a4b0-0x40e6fa: definitions in retail address order. */

__declspec(noinline) void __cdecl GameCam_ResetLookRot(GAMECAMERA *camera);
void __cdecl GameCam_UpdateLookRot(GAMECAMERA *camera);
void __cdecl MakePlayPlanes(GAMECAMERA *camera);
void __cdecl GameCam_UpdateShake(GAMECAMERA *camera, f32 ambient_amount);
void __cdecl CentreTwoPlayerCamera(NUVEC *center, NUVEC *player_a, NUVEC *player_b, NUVEC *reference);
extern NUMTX CutCamMtx;
extern LEVELDATA_PC *CLOUDCITYTRAPB_LDATA;
extern struct nuvec_s *shopcamlookat;
extern struct nuvec_s *shopcampos;
extern f32 CamStopBlend;
extern NUVEC CustomisePos[2];
extern NUVEC GunshipAOffset;
extern f32 HUB_MINIKITVIEWER_CAMDY;
extern f32 PodCamDist;
extern i32 movegamecamera_forcesock;
extern i32 complexsockposition_forcesock;
extern i32 netcamera;
extern f32 testmovef;
extern f32 CamTitlesSplineEndPc;
extern f32 CamTitlesBonusTimePc;
extern f32 CamShopBlendTimePc;
extern f32 CamNewGameDurationPc;
extern f32 CamPodSprintStartDistPc;
extern f32 CamPodSprintEndDistPc;
extern f32 CamPodSprintHeightPc;
extern f32 CamPodSprintRollScalePc;
extern f32 CamPodSprintAngleSeekPc;
extern f32 CamPodShakeScalePc;
extern f32 CamWalkerRadiusPc;
extern f32 CamWalkerLiftPc;
extern f32 CamDewbackRadiusPc;
extern f32 CamDewbackLiftPc;
void __cdecl Customiser_GetActiveWeirdoIndex(i32 *index, i32 *count);
void __cdecl GetShopCamLookPos(struct nuvec_s *position);
i32 __cdecl SockSysCamera(SOCKSYS_PC *sock_sys, struct nuvec_s *fallback_camera_position, i32 socket_changed,
                          struct nuvec_s *player_camera_positions, struct nuvec_s *player_positions, i32 player_count,
                          struct sockposition_pc_s *camera_socket_position, struct nuvec_s *camera_position,
                          struct nuvec_s *camera_target, f32 *overlap_blend, f32 *position_seek, f32 *angle_seek,
                          f32 *camera_shake, f32 *separation_scale);
struct nuvec_s *__cdecl Technos_TgtPos(struct TECHNO_s *techno);
f32 __cdecl getPodRoll(i32 arg0);
typedef struct CAMERA_CONTEXT_PATH_PC {
    u8 unknown_00[0x40];
    struct nugspline_s *path;
    u8 unknown_44[8];
    u8 flags;
} CAMERA_CONTEXT_PATH_PC;
typedef struct CAMERA_CONTEXT_FOCUS_PC {
    u8 unknown_00[0x28];
    NUVEC focus;
} CAMERA_CONTEXT_FOCUS_PC;
typedef struct CAMERA_CONTEXT_BUILDIT_PC {
    u8 unknown_00[0x34];
    NUVEC position;
} CAMERA_CONTEXT_BUILDIT_PC;
typedef struct CAMERA_GIZFORCE_TARGET_PC {
    u8 unknown_00[0x1c];
    NUVEC position;
} CAMERA_GIZFORCE_TARGET_PC;
typedef struct CAMERA_CABLE_PC {
    GAMEOBJECT *source;
    GAMEOBJECT *target;
} CAMERA_CABLE_PC;
typedef struct CAMERA_GRABBER_PC {
    u8 unknown_000[0x48c];
    NUVEC grab_position;
} CAMERA_GRABBER_PC;
i32 __cdecl Grapple_LookAtPos(GAMEOBJECT *object, NUVEC *position);
f32 __cdecl NuVecMagVU0(NUVEC *v);
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

f32 __fastcall PCDeviceBackend_GetActiveAspectPc(PC_DEVICE_BACKEND_PC *backend) {
    switch (PcWindowOptions.aspect_mode) {
        case 1:
            return 1.7777777910232544f;
        case 2:
            return 1.600000023841858f;
        case 3:
            return backend->active_aspect;
        case 0:
            return 1.3333333730697632f;
        default:
            return 1.3333333730697632f;
    }
}

static void PlayerCamPos(GAMEOBJECT *object, NUVEC *position, NUVEC *reference) {
    f32 height =
        (object->character_data->top + object->character_data->bottom) * object->character_data->model_scale * 0.5f;
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    NUVEC direction;
    NUVEC focus;
    NUVEC look;
    NUVEC *target;
    if ((object->dead != 0 || object->character_context == 0x2b) && (WORLD->current_level->flags & 0x40000) == 0) {
        if (object->dead != 0)
            *position = object->position;
        else
            *position = object->previous_position;
        position->y += height;
        if (VehicleArea != 0 && (object->flags_1fc & 0x80) != 0 && object->field_1010 > 0.0f) {
            f32 blend = 1.0f - object->field_1010 * 0.5f;
            if (blend < 0.5f)
                blend = NU_SIN_LUT((blend * 2.0f) * 16384.0f);
            else
                blend = 1.0f - (1.0f - (NU_SIN_LUT((blend - 0.5f) * 2.0f * 32768.0f + 16384.0f) + 1.0f) * 0.5f);
            position->x += object->velocity.x * blend;
            position->z += object->velocity.z * blend;
        }
        return;
    }
    target = NULL;
    if (object->character_context == 0x0f && object->techno != NULL &&
        (((CAMERA_CONTEXT_PATH_PC *)object->techno)->flags & 0x10) != 0) {
        NUVEC *points = ((CAMERA_CONTEXT_PATH_PC *)object->techno)->path->pts;
        f32 scale;
        NuVecSub(&direction, &points[0], &points[3]);
        NuVecSub(&focus, &points[0], &object->position);
        scale = 1.0f / NuVecMagVU0(&direction);
        GAME_CAMERA_ABSOLUTE_PC(direction.x, points[3].x - points[0].x);
        direction.x *= scale;
        GAME_CAMERA_ABSOLUTE_PC(direction.y, points[3].y - points[0].y);
        direction.y *= scale;
        GAME_CAMERA_ABSOLUTE_PC(direction.z, points[3].z - points[0].z);
        direction.z *= scale;
        position->x = points[0].x - direction.x * focus.x;
        position->y = points[0].y - direction.y * focus.y;
        position->z = points[0].z - direction.z * focus.z;
        return;
    }
    position->x = object->position.x;
    position->y = object->position.y + height;
    position->z = object->position.z;
    if (object->cable != NULL && ((CAMERA_CABLE_PC *)object->cable)->target != NULL)
        target = &((CAMERA_CABLE_PC *)object->cable)->target->collision_position;
    switch (object->character_context) {
        case 0x08:
            if (object->gizforce_target != NULL)
                target = &((CAMERA_GIZFORCE_TARGET_PC *)object->gizforce_target)->position;
            break;
        case 0x1b:
            if (object->force_target != NULL)
                target = &object->force_target->collision_position;
            break;
        case 0x1d:
            if (object->force_part != NULL && (object->force_part->active & 1) != 0)
                target = &object->force_part->position;
            break;
        case 0x2d:
            if (object->techno != NULL)
                target = &((CAMERA_CONTEXT_BUILDIT_PC *)object->techno)->position;
            break;
        case 0x51:
            target = Technos_TgtPos((TECHNO_s *)object->techno);
            if (target != NULL) {
                NuVecSub(&direction, target, &object->position);
                NuVecScale(&direction, &direction, ((TECHNO_s *)object->techno)->scale);
                NuVecAdd(&direction, &direction, &object->position);
                *position = direction;
                target = NULL;
            }
            break;
        case 0x59:
            if (object->techno != NULL)
                target = &((CAMERA_CONTEXT_FOCUS_PC *)object->techno)->focus;
            break;
        case 0x46:
            if (Grapple_LookAtPos(object, &look) != 0)
                target = &look;
            break;
    }
    if ((object->id == (u16)id_GRABCONTROL || object->id == (u16)id_GRABR2CONTROL) && world->grabber != NULL) {
        CAMERA_GRABBER_PC *grabber = (CAMERA_GRABBER_PC *)world->grabber;
        focus.x = (grabber->grab_position.x + position->x) * 0.5f;
        focus.y = position->y;
        focus.z = (grabber->grab_position.z + position->z) * 0.5f;
        target = &focus;
    }
    if (target != NULL)
        CentreTwoPlayerCamera(position, position, target, reference);
}
#pragma data_seg(".data$ViewCam")
VIEWCAM_PC ViewCam = {0,   {1000000000.0f, 0.0f, 0.0f}, -8192, -29500, 2.6600000858306885f, 0.0f, 120.0f, 0x43480000,
                      NULL};
#pragma data_seg()

void __cdecl ViewCamSetActive(i32 mode, GAMEPAD_PC *gamepad) {
    if (player != NULL) {
        ViewCam.mode = mode;
        if (mode != 0) {
            ViewCam.gamepad = gamepad;
            ViewCam.target = player->collision_position;
        } else {
            ViewCam.gamepad = NULL;
        }
    }
}

i32 __cdecl ViewCamGetMode(void) {
    return ViewCam.mode;
}

GAMEPAD_PC *__cdecl ViewCamGetGamePad(void) {
    return ViewCam.gamepad;
}

NUVEC *__cdecl ViewCamGetTgt(void) {
    return &ViewCam.target;
}

static void PlayersCamLookPos(NUVEC *target, NUVEC *reference) {
    NUVEC focus;
    i32 count;
    i32 i;
    target->x = target->y = target->z = 0.0f;
    count = 0;
    for (i = 0; i < 2; i++) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[i];
        if (object != NULL && ((object->flags_1fc & 0x80) != 0 || LookAtBoth != 0)) {
            PlayerCamPos(object, &focus, reference);
            NuVecAdd(target, target, &focus);
            count++;
        }
    }
    if (count > 0)
        NuVecScale(target, target, 1.0f / (f32)count);
}

void UpdatePlayerCameraOcclusionPc(void) {
    if (!VehicleArea) {
        i32 i;
        for (i = 0; i < 2; i++) {
            GAMEOBJECT *object = Player[i];
            if (object == NULL || (object->flags_1fc & 0x80) == 0)
                continue;
            if (FadeSys.fade != 0.0f || object->dead != 0 || object->character_context == 0x2b ||
                object->hud_icon_timer > 0.0f || GetMenuID() != -1 || object->character_context == 0x0f ||
                object->rider != NULL || (object->field_e24 & 8) == 0 ||
                ((object->character_data->model_flags & 0x40) != 0 && object->communicate_blend > 0.0f)) {
                object->tag_reset_timer = 0.0f;
                object->pause_context_state = 0.0f;
                object->flags_ef0 &= ~0x180000u;
            } else {
                i32 was_blocked = (object->flags_ef0 >> 19) & 1;
                if ((GameTimer.update_count & 1) == i) {
                    NUVEC ray;
                    i32 blocked;
                    NUVEC sample;
                    sample.x = sample.y = 0.0f;
                    sample.z = (qrand() * (1.0f / 65535.0f)) * object->bounds_radius;
                    NuVecRotateX(&sample, &sample, ((i32)((qrand() * (1.0f / 65535.0f)) * -16384.0f)));
                    sample.y *= object->bounds_half_height / object->bounds_radius;
                    NuVecRotateY(&sample, &sample, qrand());
                    NuVecAdd(&sample, &sample, &object->collision_position);

                    NuVecSub(&ray, &sample, &GameCam->position);
                    blocked = NewRayCastScaleYMask(&GameCam->position, &ray, 0.0f, 1.0f, 0, 0x1f);
                    object->flags_ef0 = (object->flags_ef0 & ~0x80000u) | (((u32)blocked & 1) << 19);
                }
                if (((object->flags_ef0 >> 19) & 1) == was_blocked) {
                    if (object->tag_reset_timer < 0.3f) {
                        object->tag_reset_timer += FRAMETIME;
                        if (object->tag_reset_timer >= 0.3f) {
                            object->tag_reset_timer = 0.3f;
                            object->flags_ef0 =
                                (object->flags_ef0 & ~0x100000u) | (((object->flags_ef0 >> 19) & 1) << 20);
                        }
                    }
                } else
                    object->tag_reset_timer = 0.0f;
            }
            object->pause_context_state = SeekLinearF(object->pause_context_state,
                                                      (object->flags_ef0 & 0x100000) ? 1.0f : 0.0f, 3.0f * FRAMETIME);
        }
    }
}

void MoveGameCamera(GAMECAMERA *camera) {
    i32 menu_id;
    i32 previous_mode;
    SOCKPOSITION_PC previous_sock_position;
    NUVEC position;
    NUVEC offset;
    NUVEC direction;
    NUVEC mode_scratch;
    f32 position_seek;
    f32 angle_seek;
    NUVEC target;
    struct GAMEPAD_PC *selected_pad;
    f32 left_y;
    f32 left_x;
    f32 right_y;
    f32 right_x;
    i32 pitch_override;
    i32 yaw_override;
    i32 roll_override;
    u16 roll_hint;
    i32 roll_hint_valid;
    f32 magnitude;
    f32 blend;
    f32 angle_blend;
    NUVEC position_snapshot;
    NUVEC target_snapshot;
    f32 dx;
    f32 dz;
    i32 pitch;
    i32 yaw;
    i32 roll;
    i32 i;
    i32 shared_count = 0;

    NUVEC player_focus[2], player_positions[2];
    GAMEOBJECT *camera_players[2];
    i32 player_roll[2], vehicle_player[2];
    u16 player_yaw[2];
    f32 walker_distance_sq = 0.0f;
    f32 roll_seek_override = 0.0f;
    f32 blend_duration = 0.5f;
    f32 camera_shake = 0.0f;
    f32 landspeeder_lookahead = 0.0f, landspeeder_yaw = -1.0f, landspeeder_speed = 0.0f;
    MiniCutCam = 0;
    NuCameraProjectionFarClipBias = WORLD->current_level->display.farclip_hack;
    menu_id = GetMenuID();
    if (MainRenderTime < 1.0f) {
        camera->render_mtx = NuMtxSetIdentityTemplate;
        camera->mtx = camera->render_mtx;
        pNuCam->mtx = camera->render_mtx;
        NuCameraSet(pNuCam);
        GameCam_ResetLookRot(camera);
        return;
    }
    if (CutSceneCameraCTRL) {
        if (CUTSTOPGAME || CUTCAMONLY) {
            camera->render_mtx = CutSceneCameraMatrixPc;
            camera->mtx = camera->render_mtx;
            CutSceneCameraMatrixSetPc = 0;
            pNuCam->mtx = camera->render_mtx;
            if (CutSceneCameraFocalLengthPc > 0.0f) {
                pNuCam->fov = NuCameraFocalLenToFOV(CutSceneCameraFocalLengthPc);
                pNuCam->fov *= (1.0f / PCDeviceBackend_GetActiveAspectPc(&PCDeviceBackend)) / 0.75f;
            }
            NuCameraSet(pNuCam);
            CutCamMtx = camera->render_mtx;
            CUTCAM = 1;
            return;
        }
    } else if (CUTSTOPGAME) {
        GameCam_ResetLookRot(camera);
        return;
    }
    if (NewMode || NewLData != NULL) {
        GameCam_ResetLookRot(camera);
        return;
    }
    previous_mode = camera->mode;
    camera->previous_mode = camera->mode;
    previous_sock_position = camera->sock_position;
    camera->mode = -1;
    if (ViewCam.gamepad != NULL)
        camera->mode = 5;
    if (newgamecam) {
        newgamecamtime += FRAMETIME;
        if (newgamecamtime >= CamNewGameDurationPc || WORLD->camera_splines[6] == NULL ||
            WORLD->camera_splines[7] == NULL)
            newgamecam = 0;
    }
    if (menu_id == 14 && HubMiniKitViewerSplinePc != NULL)
        camera->mode = 9;
    else if (menu_id == 8 && WORLD->camera_splines[24] != NULL)
        camera->mode = 6;
    else if (SHOPACTIVE && shopcampos != NULL && shopcamlookat != NULL)
        camera->mode = 8;
    else if (menu_id == 12 && WORLD->camera_splines[15] != NULL)
        camera->mode = 7;
    else if (ObstacleCamSpl != NULL && (ObstacleCamTime <= ObstacleCamEnd || ObstacleCamHoldUntilPlayersMove)) {
        f32 old_time = ObstacleCamTime;
        if (ObstacleCamEnd > ObstacleCamTime)
            ObstacleCamTime += FRAMETIME;
        if (ObstacleCamTime >= ObstacleCamEnd) {
            ObstacleCamTime = ObstacleCamEnd;
            if (ObstacleCamHoldUntilPlayersMove) {
                i32 i;
                camera->mode = 2;
                for (i = 0; ObstacleCamHoldUntilPlayersMove && i < 2; i++) {
                    GAMEOBJECT *object = Player[i];
                    if (object != NULL && (object->flags_1fc & 0x80) != 0 &&
                        (object->model_draw_result == 0 || object->pad->input_magnitude > 0.0f ||
                         (object->pad->buttons_held & (GAMEPAD_START | GAMEPAD_JUMP | GAMEPAD_ACTION | GAMEPAD_SPECIAL |
                                                       GAMEPAD_TAG | GAMEPAD_TOGGLELEFT | GAMEPAD_TOGGLERIGHT)) != 0))
                        ObstacleCamHoldUntilPlayersMove = 0;
                }
            } else {
                ObstacleCamSpl = NULL;
                if (!(ObstacleCamBlendOutTime <= 0.0f))
                    blend_duration = ObstacleCamBlendOutTime;
                else {
                    GameCam_Reset(camera);
                    previous_mode = camera->mode;
                    camera->previous_mode = previous_mode;
                    previous_sock_position = camera->sock_position;
                    CutBorderScale = 0.0f;
                }
                goto choose_gameplay_camera;
            }
        } else if (ObstacleCamTime >= ObstacleCamStart && ObstacleCamEnd > ObstacleCamTime) {
            camera->mode = 2;
            if (ObstacleCamStart > 0.0f && ObstacleCamStart > old_time && ObstacleCamBlendInTime <= 0.0f) {
                camera->blend_duration = 0.0f;
                camera->blend_time = 0.0f;
                camera->previous_mode = -1;
                CutBorderScale = 1.0f;
            }
        } else
            goto choose_gameplay_camera;
    } else if (Door_UseCutCam && Door_CutLookAtPlayers && Door_CutCamWait > 0.0f) {
        Door_CutCamWait -= FRAMETIME;
        camera->mode = 4;
    } else
        goto choose_gameplay_camera;
    goto mode_selected;
choose_gameplay_camera:
    if (camera->mode == -1) {
        camera->mode = 0;
        if (Door_UseCutCam && GameTimer.update_count == 0)
            camera->mode = 4;
        else if (WORLD->current_level == TITLES_LDATA && WORLD->camera_splines[2] != NULL)
            camera->mode = 3;
        else if (PLAYERCOUNT > 0 && WORLD->sock_sys != NULL) {
            camera->mode = 1;
            if (GUNSHIPAHACK && WORLD->current_level == BONUS_GUNSHIPA_LDATA && WORLD->sock_sys->socks[0].valid)
                camera->mode = 11;
        }
    }
mode_selected:
    if (camera->mode != previous_mode) {
        if (previous_mode == 4) {
            GameCam_Blend(camera, Door_CutCamBlendTime, Door_CutLookAtPlayers ? 0.0f : Door_CutCamWait, 1);
            Door_UseCutCam = 0;
        } else if (previous_mode != -1) {
            f32 duration;
            if (camera->mode == 2)
                duration = ObstacleCamBlendInTime;
            else if (camera->mode == 9 || previous_mode == 9 || camera->mode == 6 || previous_mode == 6)
                duration = 1.5f;
            else if (camera->mode == 8 || previous_mode == 8)
                duration = CamShopBlendTimePc;
            else if (camera->mode == 7 || previous_mode == 7)
                duration = GAMEDEMO ? 3.0f : 1.5f;
            else
                duration = blend_duration;
            if (camera->previous_mode != -1 && duration != 0.0f)
                GameCam_Blend(camera, duration, 0.0f, 1);
        }
    }
    if (camera->mode == 2 || camera->mode == 4)
        MiniCutCam = camera->blend_time < camera->blend_duration ? 1 : (ObstacleCamHoldUntilPlayersMove ? 3 : 2);
    else if (camera->blend_time < camera->blend_duration &&
             (camera->previous_camera_mode == 2 || camera->previous_camera_mode == 4))
        MiniCutCam = 4;
    position = *PlayerStartRecordInitPc[0].position;

    position_seek = ((u32)(((u8)(WORLD->current_level->cam_pos_seek))));
    angle_seek = ((u32)(((u8)(WORLD->current_level->cam_angle_seek))));
    target = v000;
    pitch_override = -1, yaw_override = -1, roll_override = 0;
    roll_hint = 0;
    roll_hint_valid = 0;
    // The per-mode camera blend duration starts afresh after transition selection.
    blend_duration = 0.5f;
    selected_pad = camera->mode == 5 ? ViewCam.gamepad : (GAMEPAD_PC *)&GamePadStorage[0];
    left_y = ((f32)(((i32)(selected_pad->bound_pad->analog_left_y)))) - 127.5f;
    GAME_CAMERA_ABSOLUTE_PC(magnitude, left_y);
    if (magnitude < 34.0f)
        left_y = 0.0f;
    left_y /= 127.5f;
    left_x = ((f32)(((i32)(selected_pad->bound_pad->analog_left_x)))) - 127.5f;
    GAME_CAMERA_ABSOLUTE_PC(magnitude, left_x);
    if (magnitude < 34.0f)
        left_x = 0.0f;
    left_x /= 127.5f;
    right_y = ((f32)(((i32)(selected_pad->bound_pad->analog_right_y)))) - 127.5f;
    GAME_CAMERA_ABSOLUTE_PC(magnitude, right_y);
    if (magnitude < 34.0f)
        right_y = 0.0f;
    right_y /= 127.5f;
    right_x = ((f32)(((i32)(selected_pad->bound_pad->analog_right_x)))) - 127.5f;
    GAME_CAMERA_ABSOLUTE_PC(magnitude, right_x);
    if (magnitude < 34.0f)
        right_x = 0.0f;
    right_x /= 127.5f;
    switch (camera->mode) {
        i32 index;

        case 1: {
            i32 i;
            // Case 1 prefix. Original 0x111314..0x1129bf and its child branches.

            i32 player_count = 0, vehicle_count = 0;
            for (i = 0; i < 2; i++) {
                struct nuvec_s *source_position;
                i32 counts_as_vehicle;
                GAMEOBJECT *object = Player[i];
                if (object == NULL || ((object->flags_1fc & 0x80) == 0 && !LookAtBoth) ||
                    (netcamera && (object->flags_1f8 & 0x40000) != 0) || (BonusWinner != -1 && BonusWinner != i))
                    continue;
                vehicle_player[i] = object->character_data->model_flags & 0x2000;
                counts_as_vehicle = !(SPEEDERCHASEA_LDATA != NULL && WORLD->current_level == SPEEDERCHASEA_LDATA &&
                                      (i16)object->id == id_SPEEDERBIKE && disable_narrow_socks);
                if (vehicle_player[i] && counts_as_vehicle) {
                    vehicle_count++;
                    if ((i16)object->id == id_LANDSPEEDER) {
                        if (landspeeder_lookahead != 0.0f) {
                            landspeeder_lookahead =
                                (landspeeder_lookahead + (object->horizontal_velocity_magnitude * 0.6f)) * 0.5f;
                            landspeeder_yaw = (landspeeder_yaw + ((u16)(object->field_0x276))) * 0.5f;
                            landspeeder_speed = (landspeeder_speed + object->horizontal_velocity_magnitude) * 0.5f;
                        } else {
                            landspeeder_speed = object->horizontal_velocity_magnitude;
                            landspeeder_lookahead = object->horizontal_velocity_magnitude * 0.6f;
                            landspeeder_yaw = ((u16)(object->field_0x276));
                        }
                    }
                }
                if (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA) {
                    NUVEC *pod_offset;
                    player_focus[player_count] = ((GAMEOBJECT *)Player[i])->collision_position;
                    pod_offset = &(mode_scratch);
                    (*pod_offset).x = 0.0f;
                    (*pod_offset).y = 0.0f;
                    (*pod_offset).z = 2.0f;
                    NuVecRotateY(&(*pod_offset), &(*pod_offset),
                                 ((i32)(((u16)(((GAMEOBJECT *)Player[i])->facing_angle)) + getPodRoll(i) * 5461.0f)));
                    NuVecAdd(&player_focus[player_count], &player_focus[player_count], &(*pod_offset));
                } else
                    PlayerCamPos(object, &player_focus[player_count], &camera->position);
                source_position = &object->position;
                if (object->character_context == 0x2b && !object->dead)
                    source_position = &object->previous_position;
                else if (object->character_context == 0x51 && object->techno != NULL) {
                    NUVEC *techno_position = Technos_TgtPos(((TECHNO_s *)(object->techno)));
                    if (techno_position != NULL)
                        source_position = techno_position;
                }
                memcpy(&player_positions[player_count], source_position, sizeof(NUVEC));
                if ((object->character_data->model_flags & 0x2000) != 0) {
                    if (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA)
                        player_roll[player_count] = ((i32)(getPodRoll(i) * 8192.0f));
                    else
                        player_roll[player_count] = RotDiff(0, object->movement_lean_angle);
                    RotDiff(0, object->tertiary_lean_angle);
                } else
                    player_roll[player_count] = 0;
                camera_players[player_count] = object;
                player_yaw[player_count] = object->field_0x276;
                player_count++;
            }
            if (player_count != 0) {
                f32 separation_scale;
                NUVEC rail_position;
                complexsockposition_forcesock = movegamecamera_forcesock;

                LOG_WARN_ONCE("SockSysCamera before: player_count=%d camera=%p world=%p", player_count, (void *)camera,
                              (void *)WORLD);
                index = SockSysCamera(WORLD->sock_sys, &camera->position, camera->mode != camera->previous_mode,
                                      player_focus, player_positions, player_count, &camera->sock_position, &position,
                                      &target, &blend_duration, &position_seek, &angle_seek, &camera_shake,
                                      &separation_scale);
                LOG_WARN_ONCE("SockSysCamera after: index=%d", index);
                rail_position = position;
                blend_duration *= 1.5f;
                if (WORLD->current_level->cam_pullback_dist > 0.0f) {
                    f32 pullback;
                    f32 lateral;
                    i32 angle = NuAtan2D(target.x - position.x, target.z - position.z);
                    NUVEC *pullback_direction = &(offset);
                    NUVEC *pullback_offset = &(mode_scratch);
                    NuVecSub(&(*pullback_direction), &position, &target);
                    (*pullback_direction).y = 0.0f;
                    NuVecNorm(&(*pullback_direction), &(*pullback_direction));
                    pullback = 0.0f;
                    if (vehicle_count) {
                        u16 heading;
                        if (player_count == 2 && vehicle_player[0] && vehicle_player[1])
                            heading = player_yaw[0] + RotDiff(player_yaw[0], player_yaw[1]) / player_count;
                        else if (vehicle_player[0])
                            heading = player_yaw[0];
                        else
                            heading = player_yaw[1];
                        index = RotDiff(angle, heading);
                        if ((camera->sock_position.sock_index == -1 ||
                             (WORLD->sock_sys->socks[camera->sock_position.sock_index].flags & 0x1000) == 0) &&
                            abs(index) > 0x4000)
                            pullback = (abs(index) - 0x4000) * (1.0f / 16384.0f);
                    }
                    camera->field_1e8 = SeekLinearF(camera->field_1e8, pullback, FRAMETIME);
                    camera->field_1ec = SeekValF(camera->field_1ec, camera->field_1e8, 3.0f);
                    NuVecScale(&(*pullback_offset), &(*pullback_direction),
                               camera->field_1ec * WORLD->current_level->cam_pullback_dist);
                    NuVecAdd(&position, &position, &(*pullback_offset));
                    NuVecAdd(&target, &target, &(*pullback_offset));
                    lateral = 0.0f;
                    if (vehicle_count &&
                        (camera->sock_position.sock_index == -1 ||
                         (WORLD->sock_sys->socks[camera->sock_position.sock_index].flags & 0x1000) == 0)) {
                        lateral = 1.0f - abs(0x4000 - abs(index)) * (1.0f / 16384.0f);
                        if (index < 0)
                            lateral = -lateral;
                    }
                    camera->field_1f0 = SeekLinearF(camera->field_1f0, lateral, FRAMETIME);
                    camera->field_1f4 = SeekValF(camera->field_1f4, camera->field_1f0, 3.0f);
                    NuVecRotateY(&(*pullback_offset), &(*pullback_direction), 0x4000);
                    NuVecScale(&(*pullback_offset), &(*pullback_offset),
                               (0.5f * WORLD->current_level->cam_lateral_dist) * camera->field_1f4);
                    NuVecSub(&position, &position, &(*pullback_offset));
                    NuVecSub(&target, &target, &(*pullback_offset));
                }
                // Original PODRACE_ADATA / PODSPRINT_ADATA branches after SockSysCamera.
                if (WORLD->area != NULL && WORLD->area == PODRACE_ADATA) {
                    NUVEC *average;
                    f32 floor;
                    f32 distance;
                    if (avg_currentspeed_mul <= 0.333f)
                        distance = 3.0f;
                    else if (avg_currentspeed_mul < 1.0f)
                        distance = 3.0f - (avg_currentspeed_mul - 0.333f) / 0.667f;
                    else
                        distance = 2.0f;
                    PodCamDist = SeekLinearF(PodCamDist, distance, FRAMETIME);
                    average = &(mode_scratch);
                    (*average).x = 0.0f;
                    (*average).y = 0.0f;
                    (*average).z = 0.0f;
                    index = 0;
                    for (; index < player_count; ++index) {
                        GAMEOBJECT *player = camera_players[index];
                        (*average).x += player->collision_position.x - NU_SIN_LUT(player->facing_angle) * PodCamDist;
                        (*average).y += player->collision_position.y + 1.0f;
                        (*average).z += player->collision_position.z - NU_COS_LUT(player->facing_angle) * PodCamDist;
                    }
                    if (player_count != 1)
                        NuVecScale(&(*average), &(*average), 1.0f / player_count);
                    position = (*average);
                    position_seek = 10.0f;
                    angle_seek = 10.0f;
                    floor = GameShadow(NULL, &position, 5.0f, -1);
                    if (floor != 2000000.0f) {
                        if (EShadY != 2000000.0f)
                            floor = EShadY > floor ? EShadY : floor;
                        floor += 1.0f;
                        if (floor > position.y)
                            position.y = floor;
                    }
                    target.y = position.y - 0.5f;
                    goto rail_vehicle_focus;
                }
                if (WORLD->area != NULL && WORLD->area == PODSPRINT_ADATA) {
                    NUVEC *average;
                    u16 yaw;
                    NUVEC *floor_probe;
                    f32 floor;
                    f32 countdown = PodSprint_InStartCountdown(WORLD);
                    f32 distance = countdown > 0.0f
                                       ? (CamPodSprintStartDistPc - CamPodSprintEndDistPc) * (countdown / 3.0) +
                                             CamPodSprintEndDistPc
                                       : CamPodSprintEndDistPc;
                    if (player_count == 2)
                        distance *= 1.5f;
                    PodCamDist = distance;
                    average = &(mode_scratch);
                    (*average).x = 0.0f;
                    (*average).y = 0.0f;
                    (*average).z = 0.0f;
                    index = 0;
                    for (; index < player_count; ++index) {
                        GAMEOBJECT *player = camera_players[index];
                        (*average).x += player->position.x;
                        (*average).y += player->collision_min.y + CamPodSprintHeightPc;
                        (*average).z += player->position.z;
                    }
                    if (player_count != 1)
                        NuVecScale(&(*average), &(*average), 1.0f / player_count);
                    target = (*average);
                    yaw = camera_players[0]->movement_facing_angle;
                    if (player_count == 2)
                        yaw += RotDiff(yaw, camera_players[1]->movement_facing_angle) / player_count;
                    position.x = target.x - NU_SIN_LUT(yaw) * PodCamDist;
                    position.y = target.y + CamPodSprintHeightPc;
                    position.z = target.z - NU_COS_LUT(yaw) * PodCamDist;
                    if (countdown <= 0.0f) {
                        index = player_roll[0];
                        if (player_count == 2)
                            index += RotDiff((u16)index, (u16)player_roll[1]) / player_count;
                        yaw += (i32)(index * CamPodSprintRollScalePc);
                        target.x = position.x + NU_SIN_LUT(yaw) * PodCamDist;
                        target.z = position.z + NU_COS_LUT(yaw) * PodCamDist;
                    }
                    floor_probe = &(mode_scratch);
                    (*floor_probe).x = position.x;
                    (*floor_probe).y = target.y - CamPodSprintHeightPc;
                    (*floor_probe).z = position.z;
                    floor = GameShadow(NULL, &(*floor_probe), 5.0f, -1);
                    if (floor != 2000000.0f) {
                        if (EShadY != 2000000.0f)
                            floor = EShadY > floor ? EShadY : floor;
                        floor += CamPodSprintHeightPc;
                        if (floor > position.y)
                            position.y = floor;
                    }
                    position_seek = 10.0f;
                    angle_seek = CamPodSprintAngleSeekPc;
                    goto rail_vehicle_focus;
                }

                if (WORLD->current_level == BONUS_GUNSHIPB_LDATA && camera->sock_position.sock_index != -1) {
                    position.x += target.x * 0.0625f;
                    position.z += target.z * 0.0625f;
                }
            rail_vehicle_focus:
                if (landspeeder_lookahead != 0.0f) {
                    NUVEC *offset;
                    f32 opposite = ((f32)(camera->input_yaw - 0x8000));
                    if (opposite < 0.0f)
                        opposite += 65536.0f;
                    offset = &(mode_scratch);
                    NuVecSub(&(*offset), &position, &target);
                    NuVecNorm(&(*offset), &(*offset));
                    NuVecScale(&(*offset), &(*offset), landspeeder_lookahead);
                    position.y += landspeeder_lookahead;
                    if (landspeeder_yaw > opposite - 4096.0f && landspeeder_yaw < opposite + 4096.0f) {
                        landspeeder_speed *= 0.5f;
                        position.x += NU_SIN_LUT(((i32)(opposite))) * landspeeder_speed + (*offset).x;
                        position.z += NU_SIN_LUT(((i32)(opposite + 16384.0f))) * landspeeder_speed + (*offset).z;
                    } else if (landspeeder_yaw > ((f32)(camera->input_yaw - 0x1000)) &&
                               landspeeder_yaw < ((f32)(camera->input_yaw + 0x1000))) {
                        landspeeder_speed *= 0.75f;
                        position.x += NU_SIN_LUT(camera->input_yaw) * landspeeder_speed + (*offset).x;
                        position.z += NU_SIN_LUT(camera->input_yaw + 0x4000) * landspeeder_speed + (*offset).z;
                    } else {
                        position.x += (*offset).x;
                        position.z += (*offset).z;
                    }
                    GameCam_Blend(GameCam, 0.75f, 0.0f, 0);
                } else {
                    i32 near_walker = 0;
                    if ((SPEEDERCHASEA_LDATA != NULL && WORLD->current_level == SPEEDERCHASEA_LDATA) ||
                        (ENDORBATTLEC_LDATA != NULL && WORLD->current_level == ENDORBATTLEC_LDATA)) {
                        NUVEC average_position;
                        Players_AveragePos(&average_position, NULL);
                    } else if (GameObjectRotationAtstId != -1) {
                        GAMEOBJECT *walker =
                            FindNearestGameObject(&position, NULL, 0, CamWalkerRadiusPc, 0.0f, -1,
                                                  GameObjectRotationAtstId, -1, &walker_distance_sq, 0, NULL);
                        if (walker != NULL) {
                            f32 distance;
                            f32 factor;
                            NUVEC *offset;
                            f32 amount;
                            i32 is_player = 0;
                            for (index = 0; index < 8; ++index) {
                                if (Player[index] != NULL && Player[index] == walker)
                                    is_player = 1;
                            }
                            landspeeder_lookahead = is_player ? CamWalkerLiftPc : CamWalkerLiftPc * 0.5;
                            distance = NuFsqrt(walker_distance_sq);
                            factor = MAX(0.0f, 1.0f - distance / CamWalkerRadiusPc);
                            factor = (1.0f + NU_SIN_LUT(((i32)(factor * 32768.0f + 16384.0f)))) * 0.5f;
                            offset = &(mode_scratch);
                            NuVecSub(&(*offset), &position, &target);
                            NuVecNorm(&(*offset), &(*offset));
                            amount = landspeeder_lookahead * (1.0f - factor);
                            NuVecScale(&(*offset), &(*offset), amount);
                            position.y += amount;
                            position.x += (*offset).x;
                            position.z += (*offset).z;
                            near_walker = landspeeder_lookahead != 0.0f;
                        }
                    }
                    if (!near_walker && id_DEWBACK != -1) {
                        if (FindNearestGameObject(&position, NULL, 0, CamWalkerRadiusPc, 0.0f, -1, id_DEWBACK, -1,
                                                  &walker_distance_sq, 0, NULL) != NULL) {
                            f32 amount;
                            f32 distance = NuFsqrt(walker_distance_sq);
                            NUVEC *offset = &(mode_scratch);
                            NuVecSub(&(*offset), &position, &target);
                            NuVecNorm(&(*offset), &(*offset));
                            amount = CamDewbackLiftPc * (1.0f - distance / CamDewbackRadiusPc);
                            NuVecScale(&(*offset), &(*offset), amount);
                            position.y += amount;
                            position.x += (*offset).x;
                            position.z += (*offset).z;
                        }
                    }
                }
                if (TATOOINEE_LDATA != NULL && WORLD->current_level == TATOOINEE_LDATA &&
                    landspeeder_lookahead == 0.0f) {
                    f32 magnitude;
                    f32 blend;
                    f32 scale;
                    NUVEC average_position;
                    NUVEC *offset = &(mode_scratch);
                    Players_AveragePos(&average_position, NULL);
                    NuVecSub(&(*offset), &rail_position, &average_position);
                    magnitude = NuVecMagSqr(&(*offset));
                    blend = 0.0f;
                    if (player2 == NULL) {
                        blend = magnitude / 125.0f;
                        if (blend > 0.75f)
                            blend = 0.6f;
                    }
                    scale = 1.0f + NU_SIN_LUT(((i32)(blend * 16384.0f + 32768.0f + 16384.0f)));
                    NuVecScale(&(*offset), &(*offset), scale);
                    (*offset).y = 0.0f;
                    NuVecSub(&position, &position, &(*offset));
                }
                if (WORLD->area != NULL && WORLD->area == HUB_ADATA && camera->sock_position.sock_index != -1)
                    position.y = WORLD->sock_sys->socks[camera->sock_position.sock_index].camera_height_above_ground;
                if (WORLD->current_level->cam_tilt != 0.0f && camera->mode == camera->previous_mode) {
                    i32 total;
                    f32 scale;
                    NUVEC forward;
                    NuVecRotateY(&forward, &v001, camera->input_yaw);
                    total = 0;
                    index = 0;
                    do {
                        i32 roll = -RotDiff(0, player_roll[index]);
                        if (camera_players[index] != NULL)
                            total += ((i32)(roll * (forward.x * camera_players[index]->facing_direction.x +
                                                    forward.z * camera_players[index]->facing_direction.z)));
                        ++index;
                    } while (index < player_count);
                    scale = WORLD->current_level->cam_tilt;
                    if (player_count != 1)
                        scale *= 1.0f / player_count;
                    roll_hint = ((i32)(total * scale));
                    roll_hint_valid = 1;
                    roll_seek_override = 1.0f;
                    roll_override = -1;
                }
                if (BonusWinner != -1) {
                    NUVEC *offset = &(mode_scratch);
                    f32 distance = NuVecDist(&position, &target, &(*offset));
                    NuVecNorm(&(*offset), &(*offset));
                    NuVecScale(&(*offset), &(*offset), distance * 0.666f);
                    NuVecAdd(&position, &target, &(*offset));
                }
            }
            movegamecamera_forcesock = -1;

            break;
        }
        case 2: {
            Minicam_Update();
            if (ObstacleCamCutCamPtr != NULL) {
                ObstacleCamCutPts[0] = *ObstacleCamCutCamPtr;
            }
            if (ObstacleCamCutTgtPtr != NULL) {
                ObstacleCamCutPts[1] = *ObstacleCamCutTgtPtr;
            }
            position = ObstacleCamSpl->pts[0];
            target = ObstacleCamSpl->pts[1];
            if (ObstacleCamAlwaysSnapAnglesPc != 0) {
                position_seek = 10.0f;
                angle_seek = 10.0f;
            }
            if (ObstacleCamRotZ != 0)
                roll_override = (u16)ObstacleCamRotZ;
            Hint_CancelCurrent();
            break;
        }

        case 4: {
            position = DoorCutCameraPosition0Pc;
            target = DoorCutCameraPosition1Pc;
            if (Door_CutLookAtPlayers != 0)
                PlayersCamLookPos(&target, &position);
            ComplexSockPosition(WORLD->sock_sys, &position, -1, -1, &camera->sock_position);
            break;
        }
        case 3:
            position = WORLD->camera_splines[2]->pts[0];
            target = WORLD->camera_splines[2]->pts[1];
            if (StartupGameFlag0 != 0)
                yaw_override = 0;
            break;

        // Case 5 in MoveGameCamera. Inputs have already undergone the original
        // signed-stick conversion and deadzone handling in the common prologue.
        // `selected_pad` is the GAMEPAD_s at the original esp+0x108.
        // left_x/left_y correspond to xmm7/xmm6; right_y/right_x to esp+0xfc/+0x104.
        case 5: {
            f32 speed, distance;
            GAME_CAMERA_ABSOLUTE_PC(distance, ViewCam.distance);
            speed = distance * testmovef * FRAMETIME;
            if (ViewCam.gamepad != NULL) {
                offset.x = (left_x * 127.5f) * speed;
                offset.y = ((f32)(((u32)(ViewCam.gamepad->bound_pad->analog_l1)))) * speed -
                           ((f32)(((u32)(ViewCam.gamepad->bound_pad->analog_l2)))) * speed;
                offset.z = (-left_y * 127.5f) * speed;
                NuVecRotateY(&offset, &offset, camera->input_yaw);
                NuVecAdd(&ViewCam.target, &ViewCam.target, &offset);
                ViewCam.pitch = ((i16)(ViewCam.pitch + ((i32)(right_y * 16384.0f * FRAMETIME))));
                ViewCam.pitch = MIN(0x4000, MAX(-0x4000, ViewCam.pitch));
                ViewCam.yaw -= ((i16)(right_x * 16384.0f * FRAMETIME));
                if ((ViewCam.gamepad->bound_pad->digital_buttons & GAMEPAD_SELECT) == 0) {
                    f32 zoom_step = 0.1f / ViewCam.zoom_step;
                    ViewCam.distance -= ((f32)(((u32)(ViewCam.gamepad->bound_pad->analog_r1)))) * zoom_step;
                    ViewCam.distance += ((f32)(((u32)(ViewCam.gamepad->bound_pad->analog_r2)))) * zoom_step;
                    if (ViewCam.distance < 0.1f)
                        ViewCam.distance = 0.1f;
                }
            }
            position = ViewCam.target;
            target = ViewCam.target;
            target.y += ViewCam.height;
            NuVecRotateX(&direction, &v001, ViewCam.pitch);
            NuVecRotateY(&direction, &direction, ViewCam.yaw);
            position.x += ViewCam.distance * direction.x;
            position.y += ViewCam.distance * direction.y;
            position.z += ViewCam.distance * direction.z;
            break;
        }

        case 7: {
            position = WORLD->camera_splines[15]->pts[0];
            target = WORLD->camera_splines[15]->pts[1];
            Customiser_GetActiveWeirdoIndex(&index, &shared_count);
            if (shared_count == 1) {
                NUVEC offset;
                if (MissionMenuPacketPc.customise_other_player[index] != 0)
                    index = !index;
                target.x = CustomisePos[index].x;
                target.z = CustomisePos[index].z;

                NuVecSub(&offset, &target, &position);
                NuVecScale(&offset, &offset, 0.25f);
                NuVecAdd(&position, &position, &offset);
                target.y -= 0.075f;
            }
            position.x +=
                NU_SIN_LUT(((u16)((i32)(NuFmod(GameTimer.time_elapsed, 9.321f) / 9.321f * 65536.0f)))) * 0.05f;
            position.y +=
                NU_SIN_LUT(((u16)((i32)(NuFmod(GameTimer.time_elapsed, 5.792f) / 5.792f * 65536.0f)))) * 0.025f;
            position.z +=
                NU_SIN_LUT(((u16)((i32)(NuFmod(GameTimer.time_elapsed, 7.183f) / 7.183f * 65536.0f)))) * 0.025f;
            break;
        }
        case 8:
            position = *shopcampos;
            GetShopCamLookPos(&target);
            position.x +=
                0.1f * NU_SIN_LUT(((u16)(((i32)(NuFmod(GameTimer.time_elapsed, 9.321f) / 9.321f * 65536.0f)))));
            position.y +=
                0.05f * NU_SIN_LUT(((u16)(((i32)(NuFmod(GameTimer.time_elapsed, 5.792f) / 5.792f * 65536.0f)))));
            position.z +=
                0.05f * NU_SIN_LUT(((u16)(((i32)(NuFmod(GameTimer.time_elapsed, 7.183f) / 7.183f * 65536.0f)))));
            break;
        case 9:
            position = HubMiniKitViewerSplinePc->pts[0];
            target = HubMiniKitViewerSplinePc->pts[1];
            target.y += HUB_MINIKITVIEWER_CAMDY;
            position_seek = 10.0f;
            angle_seek = 10.0f;
            break;
        case 6: {
            NUVEC *points = WORLD->camera_splines[24]->pts;
            memcpy(&position, &points[0], sizeof(position));
            memcpy(&target, &points[1], sizeof(target));
            position.x +=
                NU_SIN_LUT(((u16)((i32)(NuFmod(GameTimer.time_elapsed, 9.321f) / 9.321f * 65536.0f)))) * 0.05f;
            position.y +=
                NU_SIN_LUT(((u16)((i32)(NuFmod(GameTimer.time_elapsed, 5.792f) / 5.792f * 65536.0f)))) * 0.025f;
            position.z +=
                NU_SIN_LUT(((u16)((i32)(NuFmod(GameTimer.time_elapsed, 7.183f) / 7.183f * 65536.0f)))) * 0.025f;
            break;
        }

        case 11: {
            u16 yaw;
            NUVEC ahead;
            NUVEC lateral;
            NUVEC *offset;
            f32 spline_position =
                (GamePlayTimer.time_elapsed + (((u8 *)(LevFlag))[0] == 2 ? CamTitlesBonusTimePc : 6.0f)) / 100.0f;
            f32 first_position = spline_position;
            if (spline_position > CamTitlesSplineEndPc) {
                ((u8 *)(LevFlag))[1] = 1;
                first_position = CamTitlesSplineEndPc;
            }

            PointAlongSpline(WORLD->sock_sys->socks[0].mid, first_position, &target, &yaw, NULL, 0);

            PointAlongSpline(WORLD->sock_sys->socks[0].mid, spline_position > 1.0f ? 1.0f : spline_position, &ahead,
                             NULL, NULL, 0);

            NuVecRotateY(&lateral, &v001, yaw);
            GunshipANorm = lateral;
            NuVecRotateY(&lateral, &lateral, 0x4000);
            offset = &(mode_scratch);
            (*offset).z = 0.0f;
            (*offset).x = 0.0f;
            (*offset).y = 0.0f;
            shared_count = 0;
            for (index = 0; index < 2; ++index) {
                if (Player[index] != NULL) {
                    NUVEC delta;
                    f32 projection;
                    ++shared_count;
                    delta.x = ((GAMEOBJECT *)Player[index])->collision_position.x - target.x;
                    delta.z = ((GAMEOBJECT *)Player[index])->collision_position.z - target.z;
                    projection = delta.z * lateral.z + delta.x * lateral.x;
                    (*offset).x = lateral.x * projection + (*offset).x;
                    (*offset).z = projection * lateral.z + (*offset).z;
                }
            }
            if (shared_count == 2) {
                (*offset).x *= 0.5f;
                (*offset).z *= 0.5f;
            }
            NuVecAdd(&position, &target, &GunshipAOffset);
            position.x += (*offset).x;
            position.z += (*offset).z;
            target.x = ahead.x + (*offset).x;
            target.z = ahead.z + (*offset).z;
            NuVecSub(&(*offset), &target, &position);
            pitch_override = -NuAtan2D((*offset).y, NuFsqrt((*offset).x * (*offset).x + (*offset).z * (*offset).z));
            yaw_override = NuAtan2D((*offset).x, (*offset).z);
            position_seek = 8.0f;
            break;
        }

        default:
            break;
    }
    if (WORLD->current_level == SARLACCPITA_LDATA || WORLD->current_level == SARLACCPITC_LDATA) {
        position.x += 0.1f * NU_SIN_LUT(((u16)(((i32)(NuFmod(GameTimer.time_elapsed, 9.321f) / 9.321f * 65536.0f)))));
        position.y += 0.05f * NU_SIN_LUT(((u16)(((i32)(NuFmod(GameTimer.time_elapsed, 5.792f) / 5.792f * 65536.0f)))));
        position.z += 0.05f * NU_SIN_LUT(((u16)(((i32)(NuFmod(GameTimer.time_elapsed, 7.183f) / 7.183f * 65536.0f)))));
    }
    if (camera->mode == previous_mode) {
        if (previous_sock_position.sock_index == -1 || camera->sock_position.sock_index == -1) {
            if (previous_sock_position.sock_index != camera->sock_position.sock_index &&
                WORLD->current_level != DOGFIGHTA_LDATA)
                GameCam_Blend(camera, 1.0f, 0.0f, 1);
        } else if (camera->sock_position.candidate_count != previous_sock_position.candidate_count ||
                   camera->sock_position.sock_index != previous_sock_position.sock_index) {
            GameCam_Blend(camera, blend_duration, 0.0f, 1);
        }
    }
    if (camera->reset_blend != 0) {
        camera->blend_destination_position = position;
        camera->reset_blend = 0;
        camera->blend_destination_target = target;
    }
    blend = 0.0f, angle_blend = 0.0f;
    if (camera->blend_duration > camera->blend_time) {
        if (camera->blend_curve > 0.0f)
            camera->blend_curve -= FRAMETIME;
        else {
            camera->blend_time += FRAMETIME;
            if (camera->blend_time > camera->blend_duration)
                camera->blend_time = camera->blend_duration;
        }
        if (camera->blend_duration > camera->blend_time) {
            blend = camera->blend_time / camera->blend_duration;
            angle_blend = 2.0f * blend;
            if (angle_blend > 1.0f)
                angle_blend = 1.0f;
            camera->blend_start_position.x =
                camera->blend_end_position.x +
                (camera->blend_destination_position.x - camera->blend_end_position.x) * blend;
            camera->blend_start_position.y =
                camera->blend_end_position.y +
                (camera->blend_destination_position.y - camera->blend_end_position.y) * blend;
            camera->blend_start_position.z =
                camera->blend_end_position.z +
                (camera->blend_destination_position.z - camera->blend_end_position.z) * blend;
            camera->blend_start_target.x =
                camera->blend_end_target.x + (camera->blend_destination_target.x - camera->blend_end_target.x) * blend;
            camera->blend_start_target.y =
                camera->blend_end_target.y + (camera->blend_destination_target.y - camera->blend_end_target.y) * blend;
            camera->blend_start_target.z =
                camera->blend_end_target.z + (camera->blend_destination_target.z - camera->blend_end_target.z) * blend;
        }
    }
    camera->angle_seek = angle_seek;
    camera->position_seek = position_seek;
    if (camera->mode != previous_mode && camera->blend_time >= camera->blend_duration)
        camera->position = position;
    else {
        f32 seek;
        if (camera->blend_duration > camera->blend_time) {
            position.x = camera->blend_start_position.x + (position.x - camera->blend_start_position.x) * blend;
            position.y = camera->blend_start_position.y + (position.y - camera->blend_start_position.y) * blend;
            position.z = camera->blend_start_position.z + (position.z - camera->blend_start_position.z) * blend;
        }
        seek = position_seek * FRAMETIME;
        if (seek > 1.0f)
            seek = 1.0f;
        seek *= CamStopBlend;
        camera->position.x += (position.x - camera->position.x) * seek;
        camera->position.y += (position.y - camera->position.y) * seek;
        camera->position.z += (position.z - camera->position.z) * seek;
    }
    camera->desired_position = position;
    camera->target = target;
    position_snapshot = camera->position;
    target_snapshot = camera->target;
    dx = target_snapshot.x - position_snapshot.x;
    dz = target_snapshot.z - position_snapshot.z;
    pitch = pitch_override;
    if (pitch_override == -1)
        pitch = ((u16)(-NuAtan2D(target_snapshot.y - position_snapshot.y, NuFsqrt(dx * dx + dz * dz))));
    if (camera->blend_duration > camera->blend_time && camera->blend_mode == 2)
        pitch = camera->blend_start_pitch + ((i32)(RotDiff(camera->blend_start_pitch, pitch) * angle_blend));
    camera->desired_pitch = pitch;
    yaw = yaw_override;
    if (yaw_override == -1)
        yaw = NuAtan2D(dx, dz);
    if (camera->blend_duration > camera->blend_time && camera->blend_mode == 2)
        yaw = camera->blend_start_yaw + ((i32)(RotDiff(camera->blend_start_yaw, yaw) * angle_blend));
    camera->desired_yaw = yaw;
    roll = roll_override;
    if (roll_override == -1)
        roll = roll_hint_valid ? roll_hint : 0;
    if (camera->blend_duration > camera->blend_time && camera->blend_mode == 2)
        roll = camera->blend_start_roll + ((i32)(RotDiff(camera->blend_start_roll, roll) * angle_blend));
    camera->desired_roll = roll;
    if (camera->mode != previous_mode && camera->blend_time >= camera->blend_duration) {
        camera->pitch = pitch;
        camera->input_yaw = yaw;
        camera->roll = roll;
    } else {
        if (pitch_override == -1) {
            f32 pitch_seek;
            if (((camera->pitch - pitch) & 0xffff) > 0x7fff) {
                CamStopBlend += FRAMETIME;
                if (CamStopBlend > 1.0f)
                    CamStopBlend = 1.0f;
            }
            pitch_seek = camera->angle_seek;
            camera->pitch = SeekRot(camera->pitch, pitch, NuFsqrt(CamStopBlend) * pitch_seek);
        } else
            camera->pitch = pitch_override;
        if (yaw_override == -1)
            camera->input_yaw = SeekRot(camera->input_yaw, yaw, camera->angle_seek * CamStopBlend);
        else
            camera->input_yaw = yaw_override;
        if (roll_override == -1)
            roll = SeekRot(camera->roll, roll,
                           (roll_seek_override > 0.0f ? roll_seek_override : camera->angle_seek) * CamStopBlend);
        else
            roll = roll_override;
        camera->roll = roll;
        pitch = camera->pitch;
        yaw = camera->input_yaw;
    }
    CamStopBlend += 0.1f * FRAMETIME;
    // Preserve NaNs on the quiet unordered path before clamping.
    if (CamStopBlend > 1.0f)
        CamStopBlend = 1.0f;
    GameCam_UpdateLookRot(camera);

    {
        NUMTX orientation = {0};
        shared_count = 0;
        for (i = 0; i < 2; i++) {
            if (Player[i] != NULL && (((GAMEOBJECT *)Player[i])->flags_1fc & 0x80) != 0 &&
                ((GAMEOBJECT *)Player[i])->flags_107a == 4) {
                NuVecAdd(NUMTX_GET_ROW_VEC(&orientation, 0), NUMTX_GET_ROW_VEC(&orientation, 0),
                         NUMTX_GET_ROW_VEC(&((GAMEOBJECT *)Player[i])->vehicle_orientation, 0));
                NuVecAdd(NUMTX_GET_ROW_VEC(&orientation, 1), NUMTX_GET_ROW_VEC(&orientation, 1),
                         NUMTX_GET_ROW_VEC(&((GAMEOBJECT *)Player[i])->vehicle_orientation, 1));
                NuVecAdd(NUMTX_GET_ROW_VEC(&orientation, 2), NUMTX_GET_ROW_VEC(&orientation, 2),
                         NUMTX_GET_ROW_VEC(&((GAMEOBJECT *)Player[i])->vehicle_orientation, 2));
                shared_count++;
            }
        }
        if (shared_count > 0) {
            NuMtxSetIdentity(&camera->mtx);
            if (shared_count > 1) {
                f32 scale = 1.0f / shared_count;
                NuVecScale(NUMTX_GET_ROW_VEC(&orientation, 0), NUMTX_GET_ROW_VEC(&orientation, 0), scale);
                NuVecScale(NUMTX_GET_ROW_VEC(&orientation, 1), NUMTX_GET_ROW_VEC(&orientation, 1), scale);
                NuVecScale(NUMTX_GET_ROW_VEC(&orientation, 2), NUMTX_GET_ROW_VEC(&orientation, 2), scale);
                NuVecNorm(NUMTX_GET_ROW_VEC(&orientation, 0), NUMTX_GET_ROW_VEC(&orientation, 0));
                NuVecNorm(NUMTX_GET_ROW_VEC(&orientation, 1), NUMTX_GET_ROW_VEC(&orientation, 1));
                NuVecNorm(NUMTX_GET_ROW_VEC(&orientation, 2), NUMTX_GET_ROW_VEC(&orientation, 2));
            }
            *NUMTX_GET_ROW_VEC(&camera->mtx, 0) = *NUMTX_GET_ROW_VEC(&orientation, 0);
            *NUMTX_GET_ROW_VEC(&camera->mtx, 1) = *NUMTX_GET_ROW_VEC(&orientation, 1);
            *NUMTX_GET_ROW_VEC(&camera->mtx, 2) = *NUMTX_GET_ROW_VEC(&orientation, 2);
            NuMtxTranslate(&camera->mtx, &camera->position);
            camera->render_mtx = camera->mtx;
            NuMtxPreRotateZ(&camera->render_mtx, roll);
            NuVecNorm(NUMTX_GET_ROW_VEC(&camera->render_mtx, 0), NUMTX_GET_ROW_VEC(&camera->render_mtx, 0));
            NuVecNorm(NUMTX_GET_ROW_VEC(&camera->render_mtx, 1), NUMTX_GET_ROW_VEC(&camera->render_mtx, 1));
            NuVecNorm(NUMTX_GET_ROW_VEC(&camera->render_mtx, 2), NUMTX_GET_ROW_VEC(&camera->render_mtx, 2));
            camera->target_mtx = camera->render_mtx;
        } else {
            NuMtxSetRotationZ(&camera->mtx, roll);
            NuMtxRotateX(&camera->mtx, pitch + ((u16)(camera->field_214)));
            NuMtxRotateY(&camera->mtx, yaw + ((u16)(camera->adjusted_yaw_offset)));
            NuMtxTranslate(&camera->mtx, &camera->position);
            camera->render_mtx = camera->mtx;
            NuMtxSetRotationZ(&camera->target_mtx, roll);
            NuMtxRotateX(&camera->target_mtx, pitch);
            NuMtxRotateY(&camera->target_mtx, yaw);
            NuMtxTranslate(&camera->target_mtx, &camera->position);
        }
        if (camera->judder_time > 0.0f) {
            camera->judder_time -= FRAMETIME;
            if (camera->judder_time > 0.0f) {
                i32 degrees = (((i32)(camera->judder_duration / 0.333f)) + 1) * 360;
                i32 phase = ((i32)(((degrees * 0x10000) / 360) *
                                   ((camera->judder_duration - camera->judder_time) / camera->judder_duration)));
                f32 amount = (camera->judder_time / camera->judder_duration) *
                             ((546.0f * camera->judder_duration) * NU_SIN_LUT(phase));
                if (camera->judder_reverse)
                    amount = -amount;
                if (camera->judder_axis == 0)
                    NuMtxPreRotateX(&camera->render_mtx, ((u16)(((i32)(amount)))));
                else if (camera->judder_axis == 1)
                    NuMtxPreRotateY(&camera->render_mtx, ((u16)(amount)));
                else
                    NuMtxPreRotateZ(&camera->render_mtx, ((u16)(amount)));
            }
        }
        {
            GIZMO_PC *window = GizmoFindByName(WORLD->gizmo_sys, blowup_gizmotype_id, "window_frame1");
            if (Cheat_PowerUpActive(-1) ||
                (WORLD->current_level == SPEEDERCHASEA_LDATA && !disable_narrow_socks &&
                 camera->sock_position.sock_index == 1) ||
                (WORLD->current_level == CLOUDCITYTRAPB_LDATA && window != NULL && window->object != NULL &&
                 (((GIZMOBLOWUP_PC *)(window->object))->status_flags & 1) != 0 &&
                 ((u8)(camera->sock_position.sock_index)) <= 1))
                camera_shake = 0.5f;
            else if (WORLD->current_level == DEATHSTAR2BATTLED_LDATA && ((u8 *)(LevFlag))[0] > 0)
                camera_shake = 1.5f;
            else {
                i32 i;
                for (i = 0; i < 2; i++) {
                    GAMEOBJECT *object = Player[i];
                    if (object != NULL && (object->flags_1fc & 0x80) != 0) {
                        if (WORLD->current_level == DEATHSTARBATTLED_LDATA &&
                            ObjInNarrowSock(object, WORLD->sock_sys, WORLD->level_idx)) {
                            camera_shake = 0.6f;
                            break;
                        }
                        object = Player[i];
                        if (object->field_1038 > 0.0f && WORLD->area != NULL &&
                            (WORLD->area == PODRACE_ADATA || WORLD->area == PODSPRINT_ADATA)) {
                            camera_shake = object->field_1038 * CamPodShakeScalePc;
                            break;
                        }
                    }
                }
            }
            GameCam_UpdateShake(camera, camera_shake);
            camera->shaken_right = *NUMTX_GET_ROW_VEC(&camera->render_mtx, 0);
            camera->shaken_up = *NUMTX_GET_ROW_VEC(&camera->render_mtx, 1);
            camera->dir = *NUMTX_GET_ROW_VEC(&camera->render_mtx, 2);
            pNuCam->mtx = camera->render_mtx;
            NuCameraSet(pNuCam);
            MakePlayPlanes(camera);
            UpdatePlayerCameraOcclusionPc();
        }
    }
}

i32 __cdecl GoingForwardsAlongNarrowSock(GAMEOBJECT *object) {
    i32 forwards;
    if (object->in_narrow_socket != 0) {
        i32 context;
        u16 heading = object->sock_heading;
        forwards = abs(RotDiff(heading, object->field_0x276)) < 0x4000;
        context = object->character_context;
        if (context == 0x2a && object->context_timer < object->context_duration * (f64)0.8f)
            forwards = 1 - forwards;
        return forwards;
    }
    return 0;
}
