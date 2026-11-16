#include "batman.h"
#include "decomp.h"
#include "gameapi/edtools/edui.h"
#include "gameapi/gui/apimenu.h"
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/camera.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/cutscenes/gcutscn.h"
#include "legoapi/cutscenes/minicamcut.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/gizmos/object/newblowup.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/camera.h"
#include "legoapi/lib/hub.h"
#include "legoapi/lib/text.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/menus/screens/shop.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/props/objects/techno.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/fx/edsplines.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episodeI.h"
#include "legoapi/world/levels/episodeII.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "legogame/pcbatman.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"

/* Retail unit 0x40a4b0-0x40e6fa: definitions in retail address order. */

extern NUMTX CutCamMtx;
extern LEVELDATA *CLOUDCITYTRAPB_LDATA;
extern struct nuvec_s *shopcamlookat;
extern struct nuvec_s *shopcampos;
extern f32 CamStopBlend;
extern NUVEC CustomisePos[2];
extern NUVEC GunshipAOffset;
extern f32 HUB_MINIKITVIEWER_CAMDY;
extern f32 PodCamDist;
extern i32 movegamecamera_forcesock;
extern i32 netcamera;
extern f32 testmovef;
extern f32 CamTitlesSplineEnd;
extern f32 CamTitlesBonusTime;
extern f32 CamShopBlendTime;
extern f32 CamNewGameDuration;
extern f32 CamPodSprintStartDist;
extern f32 CamPodSprintEndDist;
extern f32 CamPodSprintHeight;
extern f32 CamPodSprintRollScale;
extern f32 CamPodSprintAngleSeek;
extern f32 CamPodShakeScale;
extern f32 CamWalkerRadius;
extern f32 CamWalkerLift;
extern f32 CamDewbackRadius;
extern f32 CamDewbackLift;
void Customiser_GetActiveWeirdoIndex(i32 *index, i32 *count);
void GetShopCamLookPos(struct nuvec_s *position);
i32 SockSysCamera(SOCKSYS *sock_sys, struct nuvec_s *fallback_camera_position, i32 socket_changed, struct nuvec_s *player_camera_positions,
                  struct nuvec_s *player_positions, i32 player_count, struct sockposition_s *camera_socket_position,
                  struct nuvec_s *camera_position, struct nuvec_s *camera_target, f32 *overlap_blend, f32 *position_seek, f32 *angle_seek,
                  f32 *camera_shake, f32 *separation_scale);

i32 Grapple_LookAtPos(GAMEOBJECT *object, NUVEC *position);

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 CameraFabs(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

f32 __fastcall PCDeviceBackend_GetActiveAspect(PC_DEVICE_BACKEND *backend) {
    switch (PcWindowOptions.aspect_mode) {
        case PC_ASPECT_16_9:
            return 1.7777778f;
        case PC_ASPECT_16_10:
            return 1.6f;
        case PC_ASPECT_AUTO:
            return backend->active_aspect;
        case PC_ASPECT_4_3:
            return 1.3333334f;
        default:
            return 1.3333334f;
    }
}

static void PlayerCamPos(GAMEOBJECT *object, NUVEC *position, NUVEC *reference) {
    f32 height = (object->character_data->top + object->character_data->bottom) * object->character_data->model_scale * 0.5f;
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    NUVEC direction;
    NUVEC focus;
    NUVEC look;
    NUVEC *target;
    if ((object->dead != 0 || object->player.character_context == CHARACTER_CONTEXT_DOOMED) &&
        (WORLD->current_level->flags & LEVEL_IN_SPACE) == 0) {
        if (object->dead != 0) {
            *position = object->position;
        } else {
            *position = object->previous_position;
        }
        position->y += height;
        if (VehicleArea != 0 && (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && object->invulnerable_timer > 0.0f) {
            f32 blend = 1.0f - object->invulnerable_timer * 0.5f;
            if (blend < 0.5f) {
                blend = NU_SIN_LUT((blend * 2.0f) * 16384.0f);
            } else {
                blend = 1.0f - (1.0f - (NU_SIN_LUT((blend - 0.5f) * 2.0f * 32768.0f + 16384.0f) + 1.0f) * 0.5f);
            }
            position->x += object->velocity.x * blend;
            position->z += object->velocity.z * blend;
        }
        return;
    }
    target = NULL;
    if (object->player.character_context == CHARACTER_CONTEXT_TELEPORT && object->player.techno != NULL &&
        (((CAMERA_CONTEXT_PATH *)object->player.techno)->flags & 0x10) != 0) {
        NUVEC *points = ((CAMERA_CONTEXT_PATH *)object->player.techno)->path->pts;
        f32 scale;
        NuVecSub(&direction, &points[0], &points[3]);
        NuVecSub(&focus, &points[0], &object->position);
        scale = 1.0f / NuVecMagVU0(&direction);
        direction.x = CameraFabs(points[3].x - points[0].x);
        direction.x *= scale;
        direction.y = CameraFabs(points[3].y - points[0].y);
        direction.y *= scale;
        direction.z = CameraFabs(points[3].z - points[0].z);
        direction.z *= scale;
        position->x = points[0].x - direction.x * focus.x;
        position->y = points[0].y - direction.y * focus.y;
        position->z = points[0].z - direction.z * focus.z;
        return;
    }
    position->x = object->position.x;
    position->y = object->position.y + height;
    position->z = object->position.z;
    if (object->cable != NULL && object->cable->target != NULL) {
        target = &object->cable->target->collision_position;
    }
    switch (object->player.character_context) {
        case CHARACTER_CONTEXT_FORCE:
            if (object->gizforce_target != NULL) {
                target = &object->gizforce_target->position;
            }
            break;
        case CHARACTER_CONTEXT_FORCE_PUSH:
            if (object->player.force_target != NULL) {
                target = &object->player.force_target->collision_position;
            }
            break;
        case CHARACTER_CONTEXT_FORCE_DEFLECT:
            if (object->player.force_part != NULL && (object->player.force_part->active & PART_IN_USE) != 0) {
                target = &object->player.force_part->position;
            }
            break;
        case CHARACTER_CONTEXT_BUILD_IT:
            if (object->player.techno != NULL) {
                target = &((CAMERA_CONTEXT_BUILDIT *)object->player.techno)->position;
            }
            break;
        case CHARACTER_CONTEXT_TECHNO:
            target = Technos_TgtPos(object->player.techno);
            if (target != NULL) {
                NuVecSub(&direction, target, &object->position);
                NuVecScale(&direction, &direction, object->player.techno->scale);
                NuVecAdd(&direction, &direction, &object->position);
                *position = direction;
                target = NULL;
            }
            break;
        case CHARACTER_CONTEXT_PUSH_OBSTACLE:
            if (object->player.techno != NULL) {
                target = &((CAMERA_CONTEXT_FOCUS *)object->player.techno)->focus;
            }
            break;
        case CHARACTER_CONTEXT_GRAPPLE:
            if (Grapple_LookAtPos(object, &look) != 0) {
                target = &look;
            }
            break;
    }
    if ((object->id == (u16)id_GRABCONTROL || object->id == (u16)id_GRABR2CONTROL) && world->grabber != NULL) {
        CAMERA_GRABBER *grabber = world->grabber;
        focus.x = (grabber->grab_position.x + position->x) * 0.5f;
        focus.y = position->y;
        focus.z = (grabber->grab_position.z + position->z) * 0.5f;
        target = &focus;
    }
    if (target != NULL) {
        CentreTwoPlayerCamera(position, position, target, reference);
    }
}

f32 CamStopBlend = 1.0f;
f32 CamTitlesSplineEnd = 0.95f;
NUVEC GunshipAOffset = {12.0f, 22.5f, -12.0f};
f32 CamTitlesBonusTime = 51.0f;
f32 CamShopBlendTime = 1.0f;
f32 CamNewGameDuration = 10.0f;
i32 movegamecamera_forcesock = -1;
f32 testmovef = 0.005f;
f32 CamPodSprintStartDist = 12.0f;
f32 CamPodSprintEndDist = 2.0f;
f32 CamPodSprintHeight = 0.65f;
f32 CamPodSprintRollScale = 0.6f;
f32 CamPodSprintAngleSeek = 7.0f;
f32 CamPodShakeScale = 2.0f;
VIEWCAM ViewCam = {0, {1000000000.0f, 0.0f, 0.0f}, -8192, -29500, 2.66f, 0.0f, 120.0f, 0x43480000, NULL};
f32 CamWalkerRadius = 5.0f;
f32 CamWalkerLift = 4.0f;
f32 CamDewbackRadius = 5.0f;
f32 CamDewbackLift = 1.5f;

void ViewCamSetActive(i32 mode, GAMEPAD *gamepad) {
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

i32 ViewCamGetMode(void) {
    return ViewCam.mode;
}

GAMEPAD *ViewCamGetGamePad(void) {
    return ViewCam.gamepad;
}

NUVEC *ViewCamGetTgt(void) {
    return &ViewCam.target;
}

static void PlayersCamLookPos(NUVEC *target, NUVEC *reference) {
    NUVEC focus;
    i32 count;
    i32 i;
    target->x = target->y = target->z = 0.0f;
    count = 0;
    for (i = 0; i < 2; i++) {
        GAMEOBJECT *object = Player[i];
        if (object != NULL && ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 || LookAtBoth != 0)) {
            PlayerCamPos(object, &focus, reference);
            NuVecAdd(target, target, &focus);
            count++;
        }
    }
    if (count > 0) {
        NuVecScale(target, target, 1.0f / (f32)count);
    }
}

void UpdatePlayerCameraOcclusion(void) {
    if (!VehicleArea) {
        i32 i;
        for (i = 0; i < 2; i++) {
            GAMEOBJECT *object = Player[i];
            if (object == NULL || (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0) {
                continue;
            }
            if (FadeSys.fade != 0.0f || object->dead != 0 || object->player.character_context == CHARACTER_CONTEXT_DOOMED ||
                object->player.hud_icon_timer > 0.0f || GetMenuID() != -1 ||
                object->player.character_context == CHARACTER_CONTEXT_TELEPORT || object->player.rider != NULL ||
                (object->player.unknown_e24 & 8) == 0 ||
                ((object->character_data->model_flags & CHARACTER_MODEL_ASTROMECH) != 0 && object->player.communicate_blend > 0.0f)) {
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
                            object->flags_ef0 = (object->flags_ef0 & ~0x100000u) | (((object->flags_ef0 >> 19) & 1) << 20);
                        }
                    }
                } else {
                    object->tag_reset_timer = 0.0f;
                }
            }
            object->pause_context_state =
                SeekLinearF(object->pause_context_state, (object->flags_ef0 & 0x100000) ? 1.0f : 0.0f, 3.0f * FRAMETIME);
        }
    }
}

__declspec(align(16)) NUMTX CutCamMtx = {0};
i32 MiniCutCam = 0;
f32 PodCamDist = 0;
i32 netcamera = 0;

u32 GAMEPAD_SELECT = PAD_BUTTON_SELECT;

void MoveGameCamera(GAMECAMERA *camera) {
    i32 menu_id;
    i32 previous_mode;
    SOCKPOSITION previous_sock_position;
    NUVEC position;
    NUVEC offset;
    NUVEC direction;
    NUVEC mode_scratch;
    f32 position_seek;
    f32 angle_seek;
    NUVEC target;
    struct GAMEPAD *selected_pad;
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

    NUVEC player_focus[2];
    NUVEC player_positions[2];
    GAMEOBJECT *camera_players[2];
    i32 player_roll[2];
    i32 vehicle_player[2];
    u16 player_yaw[2];
    f32 walker_distance_sq = 0.0f;
    f32 roll_seek_override = 0.0f;
    f32 blend_duration = 0.5f;
    f32 camera_shake = 0.0f;
    f32 landspeeder_lookahead = 0.0f;
    f32 landspeeder_yaw = -1.0f;
    f32 landspeeder_speed = 0.0f;
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
            camera->render_mtx = CutSceneCameraMatrix;
            camera->mtx = camera->render_mtx;
            CutSceneCameraMatrixSet = 0;
            pNuCam->mtx = camera->render_mtx;
            if (CutSceneCameraFocalLength > 0.0f) {
                pNuCam->fov = NuCameraFocalLenToFOV(CutSceneCameraFocalLength);
                pNuCam->fov *= (1.0f / PCDeviceBackend_GetActiveAspect(&PCDeviceBackend)) / 0.75f;
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
    camera->mode = GAMECAM_MODE_NONE;
    if (ViewCam.gamepad != NULL) {
        camera->mode = GAMECAM_MODE_VIEWCAM;
    }
    if (newgamecam) {
        newgamecamtime += FRAMETIME;
        if (newgamecamtime >= CamNewGameDuration || WORLD->camera_splines[6] == NULL || WORLD->camera_splines[7] == NULL) {
            newgamecam = 0;
        }
    }
    if (menu_id == MENU_MINIKIT && HubMiniKitViewerSpline != NULL) {
        camera->mode = GAMECAM_MODE_MINIKIT_VIEWER;
    } else if (menu_id == MENU_MISSIONS && WORLD->camera_splines[24] != NULL) {
        camera->mode = GAMECAM_MODE_MISSION;
    } else if (SHOPACTIVE && shopcampos != NULL && shopcamlookat != NULL) {
        camera->mode = GAMECAM_MODE_SHOP;
    } else if (menu_id == MENU_CUSTOMISER && WORLD->camera_splines[15] != NULL) {
        camera->mode = GAMECAM_MODE_CUSTOMISE;
    } else if (ObstacleCamSpl != NULL && (ObstacleCamTime <= ObstacleCamEnd || ObstacleCamHoldUntilPlayersMove)) {
        f32 old_time = ObstacleCamTime;
        if (ObstacleCamEnd > ObstacleCamTime) {
            ObstacleCamTime += FRAMETIME;
        }
        if (ObstacleCamTime >= ObstacleCamEnd) {
            ObstacleCamTime = ObstacleCamEnd;
            if (ObstacleCamHoldUntilPlayersMove) {
                i32 i;
                camera->mode = GAMECAM_MODE_OBSTACLE;
                for (i = 0; ObstacleCamHoldUntilPlayersMove && i < 2; i++) {
                    GAMEOBJECT *object = Player[i];
                    if (object != NULL && (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 &&
                        (object->model_draw_result == 0 || object->player.pad->input_magnitude > 0.0f ||
                         (object->player.pad->buttons_held & (GAMEPAD_START | GAMEPAD_JUMP | GAMEPAD_ACTION | GAMEPAD_SPECIAL |
                                                              GAMEPAD_TAG | GAMEPAD_TOGGLELEFT | GAMEPAD_TOGGLERIGHT)) != 0)) {
                        ObstacleCamHoldUntilPlayersMove = 0;
                    }
                }
            } else {
                ObstacleCamSpl = NULL;
                if (!(ObstacleCamBlendOutTime <= 0.0f)) {
                    blend_duration = ObstacleCamBlendOutTime;
                } else {
                    GameCam_Reset(camera);
                    previous_mode = camera->mode;
                    camera->previous_mode = previous_mode;
                    previous_sock_position = camera->sock_position;
                    CutBorderScale = 0.0f;
                }
                goto choose_gameplay_camera;
            }
        } else if (ObstacleCamTime >= ObstacleCamStart && ObstacleCamEnd > ObstacleCamTime) {
            camera->mode = GAMECAM_MODE_OBSTACLE;
            if (ObstacleCamStart > 0.0f && ObstacleCamStart > old_time && ObstacleCamBlendInTime <= 0.0f) {
                camera->blend_duration = 0.0f;
                camera->blend_time = 0.0f;
                camera->previous_mode = GAMECAM_MODE_NONE;
                CutBorderScale = 1.0f;
            }
        } else {
            goto choose_gameplay_camera;
        }
    } else if (Door_UseCutCam && Door_CutLookAtPlayers && Door_CutCamWait > 0.0f) {
        Door_CutCamWait -= FRAMETIME;
        camera->mode = GAMECAM_MODE_DOOR_CUT;
    } else {
        goto choose_gameplay_camera;
    }
    goto mode_selected;
choose_gameplay_camera:
    if (camera->mode == GAMECAM_MODE_NONE) {
        camera->mode = GAMECAM_MODE_DEFAULT;
        if (Door_UseCutCam && GameTimer.update_count == 0) {
            camera->mode = GAMECAM_MODE_DOOR_CUT;
        } else if (WORLD->current_level == TITLES_LDATA && WORLD->camera_splines[2] != NULL) {
            camera->mode = GAMECAM_MODE_TITLES;
        } else if (PLAYERCOUNT > 0 && WORLD->sock_sys != NULL) {
            camera->mode = GAMECAM_MODE_SOCKS;
            if (GUNSHIPAHACK && WORLD->current_level == BONUS_GUNSHIPA_LDATA && WORLD->sock_sys->socks[0].valid) {
                camera->mode = GAMECAM_MODE_GUNSHIP;
            }
        }
    }
mode_selected:
    if (camera->mode != previous_mode) {
        if (previous_mode == GAMECAM_MODE_DOOR_CUT) {
            GameCam_Blend(camera, Door_CutCamBlendTime, Door_CutLookAtPlayers ? 0.0f : Door_CutCamWait, 1);
            Door_UseCutCam = 0;
        } else if (previous_mode != GAMECAM_MODE_NONE) {
            f32 duration;
            if (camera->mode == GAMECAM_MODE_OBSTACLE) {
                duration = ObstacleCamBlendInTime;
            } else if (camera->mode == GAMECAM_MODE_MINIKIT_VIEWER || previous_mode == GAMECAM_MODE_MINIKIT_VIEWER ||
                       camera->mode == GAMECAM_MODE_MISSION || previous_mode == GAMECAM_MODE_MISSION) {
                duration = 1.5f;
            } else if (camera->mode == GAMECAM_MODE_SHOP || previous_mode == GAMECAM_MODE_SHOP) {
                duration = CamShopBlendTime;
            } else if (camera->mode == GAMECAM_MODE_CUSTOMISE || previous_mode == GAMECAM_MODE_CUSTOMISE) {
                duration = GAMEDEMO ? 3.0f : 1.5f;
            } else {
                duration = blend_duration;
            }
            if (camera->previous_mode != GAMECAM_MODE_NONE && duration != 0.0f) {
                GameCam_Blend(camera, duration, 0.0f, 1);
            }
        }
    }
    if (camera->mode == GAMECAM_MODE_OBSTACLE || camera->mode == GAMECAM_MODE_DOOR_CUT) {
        MiniCutCam = camera->blend_time < camera->blend_duration ? 1 : (ObstacleCamHoldUntilPlayersMove ? 3 : 2);
    } else if (camera->blend_time < camera->blend_duration && (camera->previous_camera_mode == 2 || camera->previous_camera_mode == 4)) {
        MiniCutCam = 4;
    }
    position = *PlayerStart[0].position;

    position_seek = WORLD->current_level->cam_pos_seek;
    angle_seek = WORLD->current_level->cam_angle_seek;
    target = v000;
    pitch_override = -1, yaw_override = -1, roll_override = 0;
    roll_hint = 0;
    roll_hint_valid = 0;
    // The per-mode camera blend duration starts afresh after transition selection.
    blend_duration = 0.5f;
    selected_pad = camera->mode == GAMECAM_MODE_VIEWCAM ? ViewCam.gamepad : (GAMEPAD *)&GamePadStorage[0];
    left_y = ((f32)(((i32)(selected_pad->bound_pad->analog_left_y)))) - 127.5f;
    magnitude = CameraFabs(left_y);
    if (magnitude < 34.0f) {
        left_y = 0.0f;
    }
    left_y /= 127.5f;
    left_x = ((f32)(((i32)(selected_pad->bound_pad->analog_left_x)))) - 127.5f;
    magnitude = CameraFabs(left_x);
    if (magnitude < 34.0f) {
        left_x = 0.0f;
    }
    left_x /= 127.5f;
    right_y = ((f32)(((i32)(selected_pad->bound_pad->analog_right_y)))) - 127.5f;
    magnitude = CameraFabs(right_y);
    if (magnitude < 34.0f) {
        right_y = 0.0f;
    }
    right_y /= 127.5f;
    right_x = ((f32)(((i32)(selected_pad->bound_pad->analog_right_x)))) - 127.5f;
    magnitude = CameraFabs(right_x);
    if (magnitude < 34.0f) {
        right_x = 0.0f;
    }
    right_x /= 127.5f;
    switch (camera->mode) {
        i32 index;

        case GAMECAM_MODE_SOCKS: {
            i32 i;
            // Case 1 prefix. Original 0x111314..0x1129bf and its child branches.

            i32 player_count = 0;
            i32 vehicle_count = 0;
            for (i = 0; i < 2; i++) {
                struct nuvec_s *source_position;
                i32 counts_as_vehicle;
                GAMEOBJECT *object = Player[i];
                if (object == NULL || ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0 && !LookAtBoth) ||
                    (netcamera && (object->flags_1f8 & 0x40000) != 0) || (BonusWinner != -1 && BonusWinner != i)) {
                    continue;
                }
                vehicle_player[i] = object->character_data->model_flags & CHARACTER_MODEL_VEHICLE;
                counts_as_vehicle = !(SPEEDERCHASEA_LDATA != NULL && WORLD->current_level == SPEEDERCHASEA_LDATA &&
                                      (i16)object->id == id_SPEEDERBIKE && disable_narrow_socks);
                if (vehicle_player[i] && counts_as_vehicle) {
                    vehicle_count++;
                    if ((i16)object->id == id_LANDSPEEDER) {
                        if (landspeeder_lookahead != 0.0f) {
                            landspeeder_lookahead = (landspeeder_lookahead + (object->horizontal_velocity_magnitude * 0.6f)) * 0.5f;
                            landspeeder_yaw = (landspeeder_yaw + object->movement_angle) * 0.5f;
                            landspeeder_speed = (landspeeder_speed + object->horizontal_velocity_magnitude) * 0.5f;
                        } else {
                            landspeeder_speed = object->horizontal_velocity_magnitude;
                            landspeeder_lookahead = object->horizontal_velocity_magnitude * 0.6f;
                            landspeeder_yaw = object->movement_angle;
                        }
                    }
                }
                if (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA) {
                    NUVEC *pod_offset;
                    player_focus[player_count] = Player[i]->collision_position;
                    pod_offset = &mode_scratch;
                    (*pod_offset).x = 0.0f;
                    (*pod_offset).y = 0.0f;
                    (*pod_offset).z = 2.0f;
                    NuVecRotateY(&(*pod_offset), &(*pod_offset), ((i32)(Player[i]->facing_angle + getPodRoll(i) * 5461.0f)));
                    NuVecAdd(&player_focus[player_count], &player_focus[player_count], &(*pod_offset));
                } else {
                    PlayerCamPos(object, &player_focus[player_count], &camera->position);
                }
                source_position = &object->position;
                if (object->player.character_context == CHARACTER_CONTEXT_DOOMED && !object->dead) {
                    source_position = &object->previous_position;
                } else if (object->player.character_context == CHARACTER_CONTEXT_TECHNO && object->player.techno != NULL) {
                    NUVEC *techno_position = Technos_TgtPos(object->player.techno);
                    if (techno_position != NULL) {
                        source_position = techno_position;
                    }
                }
                memcpy(&player_positions[player_count], source_position, sizeof(NUVEC));
                if ((object->character_data->model_flags & CHARACTER_MODEL_VEHICLE) != 0) {
                    if (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA) {
                        player_roll[player_count] = ((i32)(getPodRoll(i) * 8192.0f));
                    } else {
                        player_roll[player_count] = RotDiff(0, object->player.movement_lean_angle);
                    }
                    RotDiff(0, object->player.tertiary_lean_angle);
                } else {
                    player_roll[player_count] = 0;
                }
                camera_players[player_count] = object;
                player_yaw[player_count] = object->movement_angle;
                player_count++;
            }
            if (player_count != 0) {
                f32 separation_scale;
                NUVEC rail_position;
                complexsockposition_forcesock = movegamecamera_forcesock;

                LOG_WARN_ONCE("SockSysCamera before: player_count=%d camera=%p world=%p", player_count, (void *)camera, (void *)WORLD);
                index = SockSysCamera(WORLD->sock_sys, &camera->position, camera->mode != camera->previous_mode, player_focus,
                                      player_positions, player_count, &camera->sock_position, &position, &target, &blend_duration,
                                      &position_seek, &angle_seek, &camera_shake, &separation_scale);
                LOG_WARN_ONCE("SockSysCamera after: index=%d", index);
                rail_position = position;
                blend_duration *= 1.5f;
                if (WORLD->current_level->cam_pullback_dist > 0.0f) {
                    f32 pullback;
                    f32 lateral;
                    i32 angle = NuAtan2D(target.x - position.x, target.z - position.z);
                    NUVEC *pullback_direction = &offset;
                    NUVEC *pullback_offset = &mode_scratch;
                    NuVecSub(&(*pullback_direction), &position, &target);
                    (*pullback_direction).y = 0.0f;
                    NuVecNorm(&(*pullback_direction), &(*pullback_direction));
                    pullback = 0.0f;
                    if (vehicle_count) {
                        u16 heading;
                        if (player_count == 2 && vehicle_player[0] && vehicle_player[1]) {
                            heading = player_yaw[0] + RotDiff(player_yaw[0], player_yaw[1]) / player_count;
                        } else if (vehicle_player[0]) {
                            heading = player_yaw[0];
                        } else {
                            heading = player_yaw[1];
                        }
                        index = RotDiff(angle, heading);
                        if ((camera->sock_position.sock_index == -1 ||
                             (WORLD->sock_sys->socks[camera->sock_position.sock_index].flags & SOCK_FLAG_MISC_2) == 0) &&
                            abs(index) > 0x4000) {
                            pullback = (abs(index) - 0x4000) * (1.0f / 16384.0f);
                        }
                    }
                    camera->pullback_target = SeekLinearF(camera->pullback_target, pullback, FRAMETIME);
                    camera->pullback = SeekValF(camera->pullback, camera->pullback_target, 3.0f);
                    NuVecScale(&(*pullback_offset), &(*pullback_direction), camera->pullback * WORLD->current_level->cam_pullback_dist);
                    NuVecAdd(&position, &position, &(*pullback_offset));
                    NuVecAdd(&target, &target, &(*pullback_offset));
                    lateral = 0.0f;
                    if (vehicle_count && (camera->sock_position.sock_index == -1 ||
                                          (WORLD->sock_sys->socks[camera->sock_position.sock_index].flags & SOCK_FLAG_MISC_2) == 0)) {
                        lateral = 1.0f - abs(0x4000 - abs(index)) * (1.0f / 16384.0f);
                        if (index < 0) {
                            lateral = -lateral;
                        }
                    }
                    camera->lateral_target = SeekLinearF(camera->lateral_target, lateral, FRAMETIME);
                    camera->lateral = SeekValF(camera->lateral, camera->lateral_target, 3.0f);
                    NuVecRotateY(&(*pullback_offset), &(*pullback_direction), 0x4000);
                    NuVecScale(&(*pullback_offset), &(*pullback_offset),
                               (0.5f * WORLD->current_level->cam_lateral_dist) * camera->lateral);
                    NuVecSub(&position, &position, &(*pullback_offset));
                    NuVecSub(&target, &target, &(*pullback_offset));
                }
                // Original PODRACE_ADATA / PODSPRINT_ADATA branches after SockSysCamera.
                if (WORLD->area != NULL && WORLD->area == PODRACE_ADATA) {
                    NUVEC *average;
                    f32 floor;
                    f32 distance;
                    if (avg_currentspeed_mul <= 0.333f) {
                        distance = 3.0f;
                    } else if (avg_currentspeed_mul < 1.0f) {
                        distance = 3.0f - (avg_currentspeed_mul - 0.333f) / 0.667f;
                    } else {
                        distance = 2.0f;
                    }
                    PodCamDist = SeekLinearF(PodCamDist, distance, FRAMETIME);
                    average = &mode_scratch;
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
                    if (player_count != 1) {
                        NuVecScale(&(*average), &(*average), 1.0f / player_count);
                    }
                    position = (*average);
                    position_seek = 10.0f;
                    angle_seek = 10.0f;
                    floor = GameShadow(NULL, &position, 5.0f, -1);
                    if (floor != 2000000.0f) {
                        if (EShadY != 2000000.0f) {
                            floor = EShadY > floor ? EShadY : floor;
                        }
                        floor += 1.0f;
                        if (floor > position.y) {
                            position.y = floor;
                        }
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
                                       ? (CamPodSprintStartDist - CamPodSprintEndDist) * (countdown / 3.0) + CamPodSprintEndDist
                                       : CamPodSprintEndDist;
                    if (player_count == 2) {
                        distance *= 1.5f;
                    }
                    PodCamDist = distance;
                    average = &mode_scratch;
                    (*average).x = 0.0f;
                    (*average).y = 0.0f;
                    (*average).z = 0.0f;
                    index = 0;
                    for (; index < player_count; ++index) {
                        GAMEOBJECT *player = camera_players[index];
                        (*average).x += player->position.x;
                        (*average).y += player->collision_min.y + CamPodSprintHeight;
                        (*average).z += player->position.z;
                    }
                    if (player_count != 1) {
                        NuVecScale(&(*average), &(*average), 1.0f / player_count);
                    }
                    target = (*average);
                    yaw = camera_players[0]->movement_facing_angle;
                    if (player_count == 2) {
                        yaw += RotDiff(yaw, camera_players[1]->movement_facing_angle) / player_count;
                    }
                    position.x = target.x - NU_SIN_LUT(yaw) * PodCamDist;
                    position.y = target.y + CamPodSprintHeight;
                    position.z = target.z - NU_COS_LUT(yaw) * PodCamDist;
                    if (countdown <= 0.0f) {
                        index = player_roll[0];
                        if (player_count == 2) {
                            index += RotDiff((u16)index, (u16)player_roll[1]) / player_count;
                        }
                        yaw += (i32)(index * CamPodSprintRollScale);
                        target.x = position.x + NU_SIN_LUT(yaw) * PodCamDist;
                        target.z = position.z + NU_COS_LUT(yaw) * PodCamDist;
                    }
                    floor_probe = &mode_scratch;
                    (*floor_probe).x = position.x;
                    (*floor_probe).y = target.y - CamPodSprintHeight;
                    (*floor_probe).z = position.z;
                    floor = GameShadow(NULL, &(*floor_probe), 5.0f, -1);
                    if (floor != 2000000.0f) {
                        if (EShadY != 2000000.0f) {
                            floor = EShadY > floor ? EShadY : floor;
                        }
                        floor += CamPodSprintHeight;
                        if (floor > position.y) {
                            position.y = floor;
                        }
                    }
                    position_seek = 10.0f;
                    angle_seek = CamPodSprintAngleSeek;
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
                    if (opposite < 0.0f) {
                        opposite += 65536.0f;
                    }
                    offset = &mode_scratch;
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
                        GAMEOBJECT *walker = FindNearestGameObject(&position, NULL, 0, CamWalkerRadius, 0.0f, -1, GameObjectRotationAtstId,
                                                                   -1, &walker_distance_sq, 0, NULL);
                        if (walker != NULL) {
                            f32 distance;
                            f32 factor;
                            NUVEC *offset;
                            f32 amount;
                            i32 is_player = 0;
                            for (index = 0; index < 8; ++index) {
                                if (Player[index] != NULL && Player[index] == walker) {
                                    is_player = 1;
                                }
                            }
                            landspeeder_lookahead = is_player ? CamWalkerLift : CamWalkerLift * 0.5;
                            distance = NuFsqrt(walker_distance_sq);
                            factor = MAX(0.0f, 1.0f - distance / CamWalkerRadius);
                            factor = (1.0f + NU_SIN_LUT(((i32)(factor * 32768.0f + 16384.0f)))) * 0.5f;
                            offset = &mode_scratch;
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
                        if (FindNearestGameObject(&position, NULL, 0, CamWalkerRadius, 0.0f, -1, id_DEWBACK, -1, &walker_distance_sq, 0,
                                                  NULL) != NULL) {
                            f32 amount;
                            f32 distance = NuFsqrt(walker_distance_sq);
                            NUVEC *offset = &mode_scratch;
                            NuVecSub(&(*offset), &position, &target);
                            NuVecNorm(&(*offset), &(*offset));
                            amount = CamDewbackLift * (1.0f - distance / CamDewbackRadius);
                            NuVecScale(&(*offset), &(*offset), amount);
                            position.y += amount;
                            position.x += (*offset).x;
                            position.z += (*offset).z;
                        }
                    }
                }
                if (TATOOINEE_LDATA != NULL && WORLD->current_level == TATOOINEE_LDATA && landspeeder_lookahead == 0.0f) {
                    f32 magnitude;
                    f32 blend;
                    f32 scale;
                    NUVEC average_position;
                    NUVEC *offset = &mode_scratch;
                    Players_AveragePos(&average_position, NULL);
                    NuVecSub(&(*offset), &rail_position, &average_position);
                    magnitude = NuVecMagSqr(&(*offset));
                    blend = 0.0f;
                    if (player2 == NULL) {
                        blend = magnitude / 125.0f;
                        if (blend > 0.75f) {
                            blend = 0.6f;
                        }
                    }
                    scale = 1.0f + NU_SIN_LUT(((i32)(blend * 16384.0f + 32768.0f + 16384.0f)));
                    NuVecScale(&(*offset), &(*offset), scale);
                    (*offset).y = 0.0f;
                    NuVecSub(&position, &position, &(*offset));
                }
                if (WORLD->area != NULL && WORLD->area == HUB_ADATA && camera->sock_position.sock_index != -1) {
                    position.y = WORLD->sock_sys->socks[camera->sock_position.sock_index].camera_height_above_ground;
                }
                if (WORLD->current_level->cam_tilt != 0.0f && camera->mode == camera->previous_mode) {
                    i32 total;
                    f32 scale;
                    NUVEC forward;
                    NuVecRotateY(&forward, &v001, camera->input_yaw);
                    total = 0;
                    index = 0;
                    do {
                        i32 roll = -RotDiff(0, player_roll[index]);
                        if (camera_players[index] != NULL) {
                            total += ((i32)(roll * (forward.x * camera_players[index]->facing_direction.x +
                                                    forward.z * camera_players[index]->facing_direction.z)));
                        }
                        ++index;
                    } while (index < player_count);
                    scale = WORLD->current_level->cam_tilt;
                    if (player_count != 1) {
                        scale *= 1.0f / player_count;
                    }
                    roll_hint = ((i32)(total * scale));
                    roll_hint_valid = 1;
                    roll_seek_override = 1.0f;
                    roll_override = -1;
                }
                if (BonusWinner != -1) {
                    NUVEC *offset = &mode_scratch;
                    f32 distance = NuVecDist(&position, &target, &(*offset));
                    NuVecNorm(&(*offset), &(*offset));
                    NuVecScale(&(*offset), &(*offset), distance * 0.666f);
                    NuVecAdd(&position, &target, &(*offset));
                }
            }
            movegamecamera_forcesock = -1;

            break;
        }
        case GAMECAM_MODE_OBSTACLE: {
            Minicam_Update();
            if (ObstacleCamCutCamPtr != NULL) {
                ObstacleCamCutPts[0] = *ObstacleCamCutCamPtr;
            }
            if (ObstacleCamCutTgtPtr != NULL) {
                ObstacleCamCutPts[1] = *ObstacleCamCutTgtPtr;
            }
            position = ObstacleCamSpl->pts[0];
            target = ObstacleCamSpl->pts[1];
            if (ObstacleCamAlwaysSnapAngles != 0) {
                position_seek = 10.0f;
                angle_seek = 10.0f;
            }
            if (ObstacleCamRotZ != 0) {
                roll_override = (u16)ObstacleCamRotZ;
            }
            Hint_CancelCurrent();
            break;
        }

        case GAMECAM_MODE_DOOR_CUT: {
            position = DoorCutCameraPosition0;
            target = DoorCutCameraPosition1;
            if (Door_CutLookAtPlayers != 0) {
                PlayersCamLookPos(&target, &position);
            }
            ComplexSockPosition(WORLD->sock_sys, &position, -1, -1, &camera->sock_position);
            break;
        }
        case GAMECAM_MODE_TITLES:
            position = WORLD->camera_splines[2]->pts[0];
            target = WORLD->camera_splines[2]->pts[1];
            if (StartupGameFlag0 != 0) {
                yaw_override = 0;
            }
            break;

        // Case 5 in MoveGameCamera. Inputs have already undergone the original
        // signed-stick conversion and deadzone handling in the common prologue.
        // `selected_pad` is the GAMEPAD_s at the original esp+0x108.
        // left_x/left_y correspond to xmm7/xmm6; right_y/right_x to esp+0xfc/+0x104.
        case GAMECAM_MODE_VIEWCAM: {
            f32 speed;
            f32 distance;
            distance = CameraFabs(ViewCam.distance);
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
                    if (ViewCam.distance < 0.1f) {
                        ViewCam.distance = 0.1f;
                    }
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

        case GAMECAM_MODE_CUSTOMISE: {
            position = WORLD->camera_splines[15]->pts[0];
            target = WORLD->camera_splines[15]->pts[1];
            Customiser_GetActiveWeirdoIndex(&index, &shared_count);
            if (shared_count == 1) {
                NUVEC offset;
                if (MissionMenuPacket.customise_other_player[index] != 0) {
                    index = !index;
                }
                target.x = CustomisePos[index].x;
                target.z = CustomisePos[index].z;

                NuVecSub(&offset, &target, &position);
                NuVecScale(&offset, &offset, 0.25f);
                NuVecAdd(&position, &position, &offset);
                target.y -= 0.075f;
            }
            position.x += NU_SIN_LUT(((u16)((i32)(NuFmod(GameTimer.time_elapsed, 9.321f) / 9.321f * 65536.0f)))) * 0.05f;
            position.y += NU_SIN_LUT(((u16)((i32)(NuFmod(GameTimer.time_elapsed, 5.792f) / 5.792f * 65536.0f)))) * 0.025f;
            position.z += NU_SIN_LUT(((u16)((i32)(NuFmod(GameTimer.time_elapsed, 7.183f) / 7.183f * 65536.0f)))) * 0.025f;
            break;
        }
        case GAMECAM_MODE_SHOP:
            position = *shopcampos;
            GetShopCamLookPos(&target);
            position.x += 0.1f * NU_SIN_LUT(((u16)(((i32)(NuFmod(GameTimer.time_elapsed, 9.321f) / 9.321f * 65536.0f)))));
            position.y += 0.05f * NU_SIN_LUT(((u16)(((i32)(NuFmod(GameTimer.time_elapsed, 5.792f) / 5.792f * 65536.0f)))));
            position.z += 0.05f * NU_SIN_LUT(((u16)(((i32)(NuFmod(GameTimer.time_elapsed, 7.183f) / 7.183f * 65536.0f)))));
            break;
        case GAMECAM_MODE_MINIKIT_VIEWER:
            position = HubMiniKitViewerSpline->pts[0];
            target = HubMiniKitViewerSpline->pts[1];
            target.y += HUB_MINIKITVIEWER_CAMDY;
            position_seek = 10.0f;
            angle_seek = 10.0f;
            break;
        case GAMECAM_MODE_MISSION: {
            NUVEC *points = WORLD->camera_splines[24]->pts;
            memcpy(&position, &points[0], sizeof(position));
            memcpy(&target, &points[1], sizeof(target));
            position.x += NU_SIN_LUT(((u16)((i32)(NuFmod(GameTimer.time_elapsed, 9.321f) / 9.321f * 65536.0f)))) * 0.05f;
            position.y += NU_SIN_LUT(((u16)((i32)(NuFmod(GameTimer.time_elapsed, 5.792f) / 5.792f * 65536.0f)))) * 0.025f;
            position.z += NU_SIN_LUT(((u16)((i32)(NuFmod(GameTimer.time_elapsed, 7.183f) / 7.183f * 65536.0f)))) * 0.025f;
            break;
        }

        case GAMECAM_MODE_GUNSHIP: {
            u16 yaw;
            NUVEC ahead;
            NUVEC lateral;
            NUVEC *offset;
            f32 spline_position =
                (GamePlayTimer.time_elapsed + (LevFlag.gunship.progress == GUNSHIP_PROGRESS_LATE ? CamTitlesBonusTime : 6.0f)) / 100.0f;
            f32 first_position = spline_position;
            if (spline_position > CamTitlesSplineEnd) {
                LevFlag.gunship.secondary = 1;
                first_position = CamTitlesSplineEnd;
            }

            PointAlongSpline(WORLD->sock_sys->socks[0].mid, first_position, &target, &yaw, NULL, 0);

            PointAlongSpline(WORLD->sock_sys->socks[0].mid, spline_position > 1.0f ? 1.0f : spline_position, &ahead, NULL, NULL, 0);

            NuVecRotateY(&lateral, &v001, yaw);
            GunshipANorm = lateral;
            NuVecRotateY(&lateral, &lateral, 0x4000);
            offset = &mode_scratch;
            (*offset).z = 0.0f;
            (*offset).x = 0.0f;
            (*offset).y = 0.0f;
            shared_count = 0;
            for (index = 0; index < 2; ++index) {
                if (Player[index] != NULL) {
                    NUVEC delta;
                    f32 projection;
                    ++shared_count;
                    delta.x = Player[index]->collision_position.x - target.x;
                    delta.z = Player[index]->collision_position.z - target.z;
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
            if (previous_sock_position.sock_index != camera->sock_position.sock_index && WORLD->current_level != DOGFIGHTA_LDATA) {
                GameCam_Blend(camera, 1.0f, 0.0f, 1);
            }
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
        if (camera->blend_curve > 0.0f) {
            camera->blend_curve -= FRAMETIME;
        } else {
            camera->blend_time += FRAMETIME;
            if (camera->blend_time > camera->blend_duration) {
                camera->blend_time = camera->blend_duration;
            }
        }
        if (camera->blend_duration > camera->blend_time) {
            blend = camera->blend_time / camera->blend_duration;
            angle_blend = 2.0f * blend;
            if (angle_blend > 1.0f) {
                angle_blend = 1.0f;
            }
            camera->blend_start_position.x =
                camera->blend_end_position.x + (camera->blend_destination_position.x - camera->blend_end_position.x) * blend;
            camera->blend_start_position.y =
                camera->blend_end_position.y + (camera->blend_destination_position.y - camera->blend_end_position.y) * blend;
            camera->blend_start_position.z =
                camera->blend_end_position.z + (camera->blend_destination_position.z - camera->blend_end_position.z) * blend;
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
    if (camera->mode != previous_mode && camera->blend_time >= camera->blend_duration) {
        camera->position = position;
    } else {
        f32 seek;
        if (camera->blend_duration > camera->blend_time) {
            position.x = camera->blend_start_position.x + (position.x - camera->blend_start_position.x) * blend;
            position.y = camera->blend_start_position.y + (position.y - camera->blend_start_position.y) * blend;
            position.z = camera->blend_start_position.z + (position.z - camera->blend_start_position.z) * blend;
        }
        seek = position_seek * FRAMETIME;
        if (seek > 1.0f) {
            seek = 1.0f;
        }
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
    if (pitch_override == -1) {
        pitch = ((u16)(-NuAtan2D(target_snapshot.y - position_snapshot.y, NuFsqrt(dx * dx + dz * dz))));
    }
    if (camera->blend_duration > camera->blend_time && camera->blend_mode == 2) {
        pitch = camera->blend_start_pitch + ((i32)(RotDiff(camera->blend_start_pitch, pitch) * angle_blend));
    }
    camera->desired_pitch = pitch;
    yaw = yaw_override;
    if (yaw_override == -1) {
        yaw = NuAtan2D(dx, dz);
    }
    if (camera->blend_duration > camera->blend_time && camera->blend_mode == 2) {
        yaw = camera->blend_start_yaw + ((i32)(RotDiff(camera->blend_start_yaw, yaw) * angle_blend));
    }
    camera->desired_yaw = yaw;
    roll = roll_override;
    if (roll_override == -1) {
        roll = roll_hint_valid ? roll_hint : 0;
    }
    if (camera->blend_duration > camera->blend_time && camera->blend_mode == 2) {
        roll = camera->blend_start_roll + ((i32)(RotDiff(camera->blend_start_roll, roll) * angle_blend));
    }
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
                if (CamStopBlend > 1.0f) {
                    CamStopBlend = 1.0f;
                }
            }
            pitch_seek = camera->angle_seek;
            camera->pitch = SeekRot(camera->pitch, pitch, NuFsqrt(CamStopBlend) * pitch_seek);
        } else {
            camera->pitch = pitch_override;
        }
        if (yaw_override == -1) {
            camera->input_yaw = SeekRot(camera->input_yaw, yaw, camera->angle_seek * CamStopBlend);
        } else {
            camera->input_yaw = yaw_override;
        }
        if (roll_override == -1) {
            roll = SeekRot(camera->roll, roll, (roll_seek_override > 0.0f ? roll_seek_override : camera->angle_seek) * CamStopBlend);
        } else {
            roll = roll_override;
        }
        camera->roll = roll;
        pitch = camera->pitch;
        yaw = camera->input_yaw;
    }
    CamStopBlend += 0.1f * FRAMETIME;
    // Preserve NaNs on the quiet unordered path before clamping.
    if (CamStopBlend > 1.0f) {
        CamStopBlend = 1.0f;
    }
    GameCam_UpdateLookRot(camera);

    {
        NUMTX orientation = {0};
        shared_count = 0;
        for (i = 0; i < 2; i++) {
            if (Player[i] != NULL && (Player[i]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && Player[i]->flags_107a == 4) {
                NuVecAdd(NUMTX_GET_ROW_VEC(&orientation, 0), NUMTX_GET_ROW_VEC(&orientation, 0),
                         NUMTX_GET_ROW_VEC(&Player[i]->player.vehicle_orientation, 0));
                NuVecAdd(NUMTX_GET_ROW_VEC(&orientation, 1), NUMTX_GET_ROW_VEC(&orientation, 1),
                         NUMTX_GET_ROW_VEC(&Player[i]->player.vehicle_orientation, 1));
                NuVecAdd(NUMTX_GET_ROW_VEC(&orientation, 2), NUMTX_GET_ROW_VEC(&orientation, 2),
                         NUMTX_GET_ROW_VEC(&Player[i]->player.vehicle_orientation, 2));
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
            NuMtxRotateX(&camera->mtx, pitch + ((u16)(camera->unknown_214)));
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
                i32 phase =
                    ((i32)(((degrees * 0x10000) / 360) * ((camera->judder_duration - camera->judder_time) / camera->judder_duration)));
                f32 amount = (camera->judder_time / camera->judder_duration) * ((546.0f * camera->judder_duration) * NU_SIN_LUT(phase));
                if (camera->judder_reverse) {
                    amount = -amount;
                }
                if (camera->judder_axis == 0) {
                    NuMtxPreRotateX(&camera->render_mtx, ((u16)(((i32)(amount)))));
                } else if (camera->judder_axis == 1) {
                    NuMtxPreRotateY(&camera->render_mtx, ((u16)(amount)));
                } else {
                    NuMtxPreRotateZ(&camera->render_mtx, ((u16)(amount)));
                }
            }
        }
        {
            GIZMO *window = GizmoFindByName(WORLD->gizmo_sys, blowup_gizmotype_id, "window_frame1");
            if (Cheat_PowerUpActive(-1) ||
                (WORLD->current_level == SPEEDERCHASEA_LDATA && !disable_narrow_socks && camera->sock_position.sock_index == 1) ||
                (WORLD->current_level == CLOUDCITYTRAPB_LDATA && window != NULL && window->object != NULL &&
                 (window->blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) != 0 && ((u8)(camera->sock_position.sock_index)) <= 1)) {
                camera_shake = 0.5f;
            } else if (WORLD->current_level == DEATHSTAR2BATTLED_LDATA && LevFlag.bytes[0] > 0) {
                camera_shake = 1.5f;
            } else {
                i32 i;
                for (i = 0; i < 2; i++) {
                    GAMEOBJECT *object = Player[i];
                    if (object != NULL && (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                        if (WORLD->current_level == DEATHSTARBATTLED_LDATA && ObjInNarrowSock(object, WORLD->sock_sys, WORLD->level_idx)) {
                            camera_shake = 0.6f;
                            break;
                        }
                        object = Player[i];
                        if (object->pod_boost_timer > 0.0f && WORLD->area != NULL &&
                            (WORLD->area == PODRACE_ADATA || WORLD->area == PODSPRINT_ADATA)) {
                            camera_shake = object->pod_boost_timer * CamPodShakeScale;
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
            UpdatePlayerCameraOcclusion();
        }
    }
}

void fun_0040e660(void) {
}

void fun_0040e670(void) {
}

void fun_0040e680(void) {
}

i32 GoingForwardsAlongNarrowSock(GAMEOBJECT *object) {
    i32 forwards;
    if (object->player.in_narrow_socket != 0) {
        i32 context;
        u16 heading = object->sock_heading;
        forwards = abs(RotDiff(heading, object->movement_angle)) < 0x4000;
        context = object->player.character_context;
        if (context == CHARACTER_CONTEXT_TURN && object->player.context_timer < object->player.context_duration * (f64)0.8f) {
            forwards = 1 - forwards;
        }
        return forwards;
    }
    return 0;
}
