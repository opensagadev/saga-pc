#include <stdio.h>
#include <string.h>
#include "saga.h"
#define SOCK_BOUNDS_OVERLAP(a, b, c, d, ignore_y)                                                                      \
    ((c)->x <= (b)->x && (a)->x <= (d)->x && (c)->z <= (b)->z && (a)->z <= (d)->z &&                                   \
     ((ignore_y) || ((c)->y <= (b)->y && (a)->y <= (d)->y)))
#define SOCK_BOUNDS_SEPARATE(a, b, c, d, ignore_y)                                                                     \
    ((b)->x < (c)->x || (a)->x > (d)->x || (b)->z < (c)->z || (a)->z > (d)->z ||                                       \
     (!(ignore_y) && ((b)->y < (c)->y || (a)->y > (d)->y)))
#define SOCK_BOUNDS_TOUCH(a, b, c, d, ignore_y)                                                                        \
    ((b)->x >= (c)->x && (a)->x <= (d)->x && (b)->z >= (c)->z && (a)->z <= (d)->z &&                                   \
     ((ignore_y) || ((b)->y >= (c)->y && (a)->y <= (d)->y)))
#define SOCK_ROTATION_DELTA(from, to)                                                                                  \
    (((i32)(to) - (i32)(from) > 0x8000)    ? (i32)(to) - (i32)(from) - 0x10000                                         \
     : ((i32)(to) - (i32)(from) < -0x8000) ? (i32)(to) - (i32)(from) + 0x10000                                         \
                                           : (i32)(to) - (i32)(from))
#define SOCK_FLAG_SCALE_LATERAL_OFFSET 0x200
#define SOCK_FLAG_CLAMP_TARGET_Y 0x20
#define SOCK_FLAG_PROJECT_CAMERA_FROM_PLAYER 0x80
#define SOCK_FLAG_TWO_PLAYER_PLANAR_PULLBACK 0x400
#define SOCK_FLAG_TWO_PLAYER_VERTICAL_SEPARATION 0x4000
#define SOCK_FLAG_CAMERA_DISTANCE_XZ 0x2000
STATIC_ASSERT(offsetof(NUMTX, m00) == 0x0);
STATIC_ASSERT(offsetof(NUMTX, m01) == 0x4);
STATIC_ASSERT(offsetof(NUMTX, m02) == 0x8);
STATIC_ASSERT(offsetof(NUMTX, m10) == 0x10);
STATIC_ASSERT(offsetof(NUMTX, m11) == 0x14);
STATIC_ASSERT(offsetof(NUMTX, m12) == 0x18);
STATIC_ASSERT(offsetof(NUMTX, m20) == 0x20);
STATIC_ASSERT(offsetof(NUMTX, m21) == 0x24);
STATIC_ASSERT(offsetof(NUMTX, m22) == 0x28);
NUGSCN *sockpar_scene = 0;
SOCK_PC *sockpar_sock = 0;
static VARIPTR *sockpar_buffer_ptr = 0;
static VARIPTR *sockpar_buffer_end = 0;
i32 GetSockEdgeEnum(char *name) {
    if (NuStrICmp(name, "SIDE") == 0)
        return 0;
    if (NuStrICmp(name, "END") == 0)
        return 1;
    return -1;
}

static void SockParCircuit(NUFPAR_PC *parser, void *context) {
    sockpar_sock->looping = 1;
}

static void SockParCurSpeed(NUFPAR_PC *parser, void *context) {
    sockpar_sock->current_speed = NuFParGetFloat(parser);
}

static void SockParMidRangeInner(NUFPAR_PC *parser, void *context) {
    sockpar_sock->mid_force_inner_radius = NuFParGetFloat(parser);
}

static void SockParMidRangeOuter(NUFPAR_PC *parser, void *context) {
    f32 radius = NuFParGetFloat(parser);
    if (radius >= 0.0f && radius > sockpar_sock->mid_force_inner_radius) {
        sockpar_sock->mid_force_outer_radius = radius;
    } else {
        sockpar_sock->mid_force_outer_radius = sockpar_sock->mid_force_inner_radius;
    }
}

static void SockParMidforceYscale(NUFPAR_PC *parser, void *context) {
    sockpar_sock->unknown_7c = NuFParGetFloat(parser);
}

static void SockParMidforce(NUFPAR_PC *parser, void *context) {
    if (NuFParGetWord(parser) == 0) {
        return;
    }
    if (NuStrICmp(parser->word_buffer_ptr, (char *)("y")) == 0) {
        sockpar_sock->flags |= 0x02;
    } else if (NuStrICmp(parser->word_buffer_ptr, (char *)("xz")) == 0) {
        sockpar_sock->flags |= 0x04;
    } else if (NuStrICmp(parser->word_buffer_ptr, (char *)("xyz")) == 0) {
        sockpar_sock->flags |= 0x04;
        sockpar_sock->flags ^= 0x04;
    }
}

static void SockParMidForceDown(NUFPAR_PC *parser, void *context) {
    sockpar_sock->flags |= 0x08;
}

static void SockParMidForceYScale(NUFPAR_PC *parser, void *context) {
    sockpar_sock->unknown_7c = NuFParGetFloat(parser);
}

static void SockParCamScaleLateral(NUFPAR_PC *parser, void *context) {
    sockpar_sock->flags |= SOCK_FLAG_SCALE_LATERAL_OFFSET;
}

static void SockParCamLookRatio(NUFPAR_PC *parser, void *context) {
    f32 ratio = NuFParGetFloat(parser);
    if (ratio < 0.0f) {
        ratio = 0.0f;
    }
    sockpar_sock->unknown_80 = ratio;
    sockpar_sock->unknown_84 = ratio;
    sockpar_sock->look_ratio_xz = ratio;
    sockpar_sock->look_ratio_y = ratio;
}

static void SockParCamLockRatioY(NUFPAR_PC *parser, void *context) {
    f32 ratio = NuFParGetFloat(parser);
    if (ratio < 0.0f) {
        ratio = 0.0f;
    }
    sockpar_sock->unknown_84 = ratio;
    sockpar_sock->look_ratio_y = ratio;
}

static void SockParCamPosSeek(NUFPAR_PC *parser, void *context) {
    union {
        f32 value;
        u32 bits;
    } result, absolute;
    result.value = NuFParGetFloat(parser);
    absolute.bits = result.bits & 0x7fffffffu;
    sockpar_sock->camera_position_seek = absolute.value;
}

static void SockParCamAngSeek(NUFPAR_PC *parser, void *context) {
    union {
        f32 value;
        u32 bits;
    } result, absolute;
    result.value = NuFParGetFloat(parser);
    absolute.bits = result.bits & 0x7fffffffu;
    sockpar_sock->camera_angle_seek = absolute.value;
}

static void SockParCamSegAhead(NUFPAR_PC *parser, void *context) {
    sockpar_sock->look_ahead_segments = (u8)(NuFParGetInt(parser));
}

static void SockParCamLateralRatio(NUFPAR_PC *parser, void *context) {
    const f32 ratio = NuFParGetFloat(parser);
    sockpar_sock->camera_local_x_ratio = ratio;
    sockpar_sock->camera_lateral_ratio = ratio;
}

static void SockParCamVerticalRatio(NUFPAR_PC *parser, void *context) {
    sockpar_sock->camera_vertical_ratio = NuFParGetFloat(parser);
}

static void SockParCamOffset(NUFPAR_PC *parser, void *context) {
    sockpar_sock->camera_arena_offset.x = NuFParGetFloat(parser);
    sockpar_sock->camera_arena_offset.y = NuFParGetFloat(parser);
    sockpar_sock->camera_arena_offset.z = NuFParGetFloat(parser);
}

static void SockParCamXOffset(NUFPAR_PC *parser, void *context) {
    sockpar_sock->camera_arena_offset.x = NuFParGetFloat(parser);
}

static void SockParCamYOffset(NUFPAR_PC *parser, void *context) {
    sockpar_sock->camera_arena_offset.y = NuFParGetFloat(parser);
}

static void SockParCamZOffset(NUFPAR_PC *parser, void *context) {
    sockpar_sock->camera_arena_offset.z = NuFParGetFloat(parser);
}

static void SockParCamRailOffset(NUFPAR_PC *parser, void *context) {
    sockpar_sock->camera_rail_offset = NuFParGetFloat(parser);
}

static void SockParCamArenaOffset(NUFPAR_PC *parser, void *context) {
    sockpar_sock->camera_rail_offset = NuFParGetFloat(parser);
}

static void SockParOffsetBlendRatio(NUFPAR_PC *parser, void *context) {
    const f32 ratio = NuFParGetFloat(parser);
    sockpar_sock->camera_arena_blend.z = ratio;
    sockpar_sock->camera_arena_blend.y = ratio;
    sockpar_sock->camera_arena_blend.x = ratio;
}

static void SockParOffsetBlendXRatio(NUFPAR_PC *parser, void *context) {
    sockpar_sock->camera_arena_blend.x = NuFParGetFloat(parser);
}

static void SockParOffsetBlendYRatio(NUFPAR_PC *parser, void *context) {
    sockpar_sock->camera_arena_blend.y = NuFParGetFloat(parser);
}

static void SockParOffsetBlendZRatio(NUFPAR_PC *parser, void *context) {
    sockpar_sock->camera_arena_blend.z = NuFParGetFloat(parser);
}

static void SockParCamDistToTargetXZ(NUFPAR_PC *parser, void *context) {
    sockpar_sock->camera_distance_to_target = NuFParGetFloat(parser);
    sockpar_sock->flags |= SOCK_FLAG_CAMERA_DISTANCE_XZ;
}

static void SockParCamPullBackYRef(NUFPAR_PC *parser, void *context) {
    sockpar_sock->flags |= SOCK_FLAG_TWO_PLAYER_VERTICAL_SEPARATION;
}

static void SockParCamDistToTarget(NUFPAR_PC *parser, void *context) {
    sockpar_sock->camera_distance_to_target = NuFParGetFloat(parser);
}

static void SockParCamDistRatio(NUFPAR_PC *parser, void *context) {
    sockpar_sock->camera_pullback_ratio = NuFParGetFloat(parser);
}

static void SockParIgnore(NUFPAR_PC *parser, void *context) {
    while (NuFParGetWord(parser) != 0) {
        {
            i32 index = NuAToI(parser->word_buffer_ptr);
            sockpar_sock->overlap_exclusion_mask[index / 32] |= 1u << (index & 31);
        }
    }
}

static void SockParYLimits(NUFPAR_PC *parser, void *context) {
    sockpar_sock->flags |= SOCK_FLAG_CLAMP_TARGET_Y;
}

static void SockParTurnOff(NUFPAR_PC *parser, void *context) {
    sockpar_sock->flags |= 0x100;
}

static void SockParName(NUFPAR_PC *parser, void *context) {
    if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buffer_ptr) < 16) {
        NuStrCpy(sockpar_sock->name, parser->word_buffer_ptr);
    }
}

static void SockParMoveAngle(NUFPAR_PC *parser, void *context) {
    i32 angle;
    sockpar_sock->flags |= 0x40;
    angle = NuFParGetInt(parser) % 360;
    sockpar_sock->input_yaw = (u16)((angle << 16) / 360);
}

static void SockParCamBehind(NUFPAR_PC *parser, void *context) {
    const f32 ratio = NuFParGetFloat(parser);
    sockpar_sock->flags |= SOCK_FLAG_PROJECT_CAMERA_FROM_PLAYER;
    sockpar_sock->camera_local_x_ratio = ratio;
    sockpar_sock->camera_vertical_ratio = ratio;
    sockpar_sock->camera_lateral_ratio = ratio;
}

static void SockParCamBlendTime(NUFPAR_PC *parser, void *context) {
    sockpar_sock->overlap_blend_ratio = NuFParGetFloat(parser);
}

static void SockParCam1Zoom(NUFPAR_PC *parser, void *context) {
    sockpar_sock->single_player_pullback = NuFParGetFloat(parser);
}

static void SockParPullbackRatio(NUFPAR_PC *parser, void *context) {
    sockpar_sock->two_player_pullback = NuFParGetFloat(parser);
}

static void SockParPullbackRatioXZ(NUFPAR_PC *parser, void *context) {
    sockpar_sock->two_player_pullback = NuFParGetFloat(parser);
    sockpar_sock->flags |= SOCK_FLAG_TWO_PLAYER_PLANAR_PULLBACK;
}

static void SockParCamHeightAboveTerrain(NUFPAR_PC *parser, void *context) {
    sockpar_sock->camera_height_above_ground = NuFParGetFloat(parser);
}

static void SockParCamShake(NUFPAR_PC *parser, void *context) {
    sockpar_sock->camera_shake = NuFParGetFloat(parser);
}

static void SockParMisc1(NUFPAR_PC *parser, void *context) {
    sockpar_sock->flags |= 0x0800;
}

static void SockParMisc2(NUFPAR_PC *parser, void *context) {
    sockpar_sock->flags |= 0x1000;
}

static void SockParObj(NUFPAR_PC *parser, void *context) {
    if (sockpar_buffer_ptr == NULL)
        return;
    if (sockpar_buffer_end == NULL)
        return;
    if (sockpar_scene == NULL)
        return;
    sockpar_buffer_ptr->addr = ((sockpar_buffer_ptr->addr + 3) & ~3u);
    sockpar_sock->objects = (NUSPECIALHANDLE *)(sockpar_buffer_ptr->void_ptr);
    sockpar_sock->object_count = 0;
    while (NuFParGetWord(parser) != 0) {
        if (NuSpecialFind(sockpar_scene, &sockpar_sock->objects[sockpar_sock->object_count], parser->word_buffer_ptr,
                          1) != 0)
            ++sockpar_sock->object_count;
    }
    if (sockpar_sock->object_count != 0)
        sockpar_buffer_ptr->addr += sockpar_sock->object_count * sizeof(NUSPECIALHANDLE);
    else
        sockpar_sock->objects = NULL;
}

static void SockParBlend(NUFPAR_PC *parser, void *context) {
    i32 edge;
    u32 index;
    i32 value = NuFParGetInt(parser);
    NuFParGetWord(parser);
    edge = GetSockEdgeEnum(parser->word_buffer_ptr);
    index = sockpar_sock->blend_count;
    sockpar_sock->blend_entries[index].edge = (i8)(edge);
    sockpar_sock->blend_entries[index].value = (u8)(value);
    sockpar_sock->blend_count = index + 1;
}

static void SockCamATSTLIFT(NUFPAR_PC *parser, void *context) {
    SOCK_PC *socket = sockpar_sock;
    socket->camera_character_close_lift = NuFParGetFloat(parser);
}

static void SockCamATSTDIST(NUFPAR_PC *parser, void *context) {
    SOCK_PC *socket = sockpar_sock;
    socket->camera_min_distance = NuFParGetFloat(parser);
}

static void SockCamATSTCAMRANGE(NUFPAR_PC *parser, void *context) {
    SOCK_PC *socket = sockpar_sock;
    socket->camera_range_of_effect = NuFParGetFloat(parser);
}

static void SockCamATSTTILT(NUFPAR_PC *parser, void *context) {
    SOCK_PC *socket = sockpar_sock;
    socket->camera_tilt_angle_change = NuFParGetFloat(parser);
}

static void SockCamATSTTILTRATE(NUFPAR_PC *parser, void *context) {
    SOCK_PC *socket = sockpar_sock;
    socket->camera_tilt_angle_rate = NuFParGetFloat(parser);
}

static void SockCamCAMERARAYTILTDIST(NUFPAR_PC *parser, void *context) {
    SOCK_PC *socket = sockpar_sock;
    socket->camera_tilt_distance = NuFParGetFloat(parser);
}

static void SockCamCAMERARAYTILTHEIGHT(NUFPAR_PC *parser, void *context) {
    SOCK_PC *socket = sockpar_sock;
    socket->camera_tilt_height = NuFParGetFloat(parser);
}

static void SockManCam_MAX_X(NUFPAR_PC *parser, void *context) {
    SOCK_PC *socket = sockpar_sock;
    socket->manual_camera_max_x = NuFParGetFloat(parser);
}

static void SockManCam_MAX_Y(NUFPAR_PC *parser, void *context) {
    SOCK_PC *socket = sockpar_sock;
    socket->manual_camera_max_y = NuFParGetFloat(parser);
}

static void SockTerrainCamInActive(NUFPAR_PC *parser, void *context) {
    sockpar_sock->terrain_camera_inactive = 1;
}
NUFPCOMJMPCTX SockSys_ConfigKeywords[] = {{"circuit", SockParCircuit},
                                          {"current_speed", SockParCurSpeed},
                                          {"mid_range_inner", SockParMidRangeInner},
                                          {"mid_range_outer", SockParMidRangeOuter},
                                          {"mid_force_yscale", SockParMidforceYscale},
                                          {"mid_force", SockParMidforce},
                                          {"mid_force_down", SockParMidForceDown},
                                          {"mid_force_yscale", SockParMidForceYScale},
                                          {"cam_scale_lateral", SockParCamScaleLateral},
                                          {"cam_look_ratio", SockParCamLookRatio},
                                          {"cam_look_ratio_y", SockParCamLockRatioY},
                                          {"campos_seek", SockParCamPosSeek},
                                          {"camang_seek", SockParCamAngSeek},
                                          {"cam_segments_ahead", SockParCamSegAhead},
                                          {"cam_lateral_ratio", SockParCamLateralRatio},
                                          {"cam_vertical_ratio", SockParCamVerticalRatio},
                                          {"cam_offset", SockParCamOffset},
                                          {"cam_x_offset", SockParCamXOffset},
                                          {"cam_y_offset", SockParCamYOffset},
                                          {"cam_z_offset", SockParCamZOffset},
                                          {"cam_rail_offset", SockParCamRailOffset},
                                          {"cam_arena_offset", SockParCamArenaOffset},
                                          {"offset_blend_ratio", SockParOffsetBlendRatio},
                                          {"offset_blend_x_ratio", SockParOffsetBlendXRatio},
                                          {"offset_blend_y_ratio", SockParOffsetBlendYRatio},
                                          {"offset_blend_z_ratio", SockParOffsetBlendZRatio},
                                          {"cam_dist_to_target_xz", SockParCamDistToTargetXZ},
                                          {"cam_pullback_yref", SockParCamPullBackYRef},
                                          {"cam_dist_to_target", SockParCamDistToTarget},
                                          {"cam_dist_ratio", SockParCamDistRatio},
                                          {"ignore_socks", SockParIgnore},
                                          {"y_limits", SockParYLimits},
                                          {"turn_off", SockParTurnOff},
                                          {"name", SockParName},
                                          {"move_angle", SockParMoveAngle},
                                          {"cam_behind_setting", SockParCamBehind},
                                          {"cam_blend_time", SockParCamBlendTime},
                                          {"cam_1player_zoom", SockParCam1Zoom},
                                          {"cam_pullback_ratio", SockParPullbackRatio},
                                          {"cam_pullback_ratio_xz", SockParPullbackRatioXZ},
                                          {"cam_height_above_terrain", SockParCamHeightAboveTerrain},
                                          {"cam_shake", SockParCamShake},
                                          {"set_misc_1", SockParMisc1},
                                          {"set_misc_2", SockParMisc2},
                                          {"obj", SockParObj},
                                          {"blend", SockParBlend},
                                          {"CamCharCloseLift", SockCamATSTLIFT},
                                          {"CamMinDist", SockCamATSTDIST},
                                          {"CamRangeOfEffect", SockCamATSTCAMRANGE},
                                          {"CamTiltAngChange", SockCamATSTTILT},
                                          {"CamTiltAngRate", SockCamATSTTILTRATE},
                                          {"CamTiltDist", SockCamCAMERARAYTILTDIST},
                                          {"CamTiltHeight", SockCamCAMERARAYTILTHEIGHT},
                                          {"ManCam_MAX_X", SockManCam_MAX_X},
                                          {"ManCam_MAX_Y", SockManCam_MAX_Y},
                                          {"TerrainCamInActive", SockTerrainCamInActive},
                                          {NULL, NULL}};

static const char *tSockCAM = "sock_cam_";
static const char *tSockA = "sock_a_";
static const char *tSockB = "sock_b_";
static const char *tSockC = "sock_c_";
static const char *tSockD = "sock_d_";
static const char *tSockMID = "sock_mid_";
static const char *tSockLEFT = "sock_left_";
static const char *tSockRIGHT = "sock_right_";
static const char *tSockLOOK = "sock_look_";
static const char *tSockLATERAL = "sock_lateral_";
static const char *tSockTRACKIN = "sock_trackin_";
static const char *tSockLIMIT = "sock_limit_";

static void AdjustMinMaxBox(NUVEC *pnt, NUVEC *min, NUVEC *max) {
    if (pnt->x < min->x) {
        min->x = pnt->x;
    }
    if (max->x < pnt->x) {
        max->x = pnt->x;
    }
    if (pnt->y < min->y) {
        min->y = pnt->y;
    }
    if (max->y < pnt->y) {
        max->y = pnt->y;
    }
    if (pnt->z < min->z) {
        min->z = pnt->z;
    }
    if (max->z < pnt->z) {
        max->z = pnt->z;
    }
}

void SockSysFindInScene(SOCKSYS_PC *sock_sys, NUGSCN *gscn) {
    i32 i;
    i32 j;
    SOCK_PC *sock;
    i32 len;
    float extent_z;

    char name[256];

    if ((gscn != NULL) && (sock_sys != NULL)) {
        sock = sock_sys->socks;
        for (i = 0; i < 0x40; i++, sock++) {
            if (sock->cam != NULL) {
                continue;
            }
            sock->unknown_7c = 1.0f;
            sock->unknown_80 = 1.0f;
            sock->unknown_84 = 1.0f;
            sock->look_ratio_xz = 1.0f;
            sock->look_ratio_y = 1.0f;
            sock->camera_position_seek = 5.0f;
            sock->camera_angle_seek = 5.0f;
            sprintf(sock->name, "%.2i", i);
            sock->overlap_blend_ratio = 0.5f;
            sprintf(name, "%s%.2i", tSockCAM, i);
            sock->cam = NuSplineFind(gscn, name);
            if (sock->cam == NULL) {
                STUBBED_NAMED("SockSysFindInScene: missing camera diagnostic at 0x0053db60");
                continue;
            }
            len = (i32)(i16)sock->cam->length;
            if (1 < len) {
                sock->length = len - 1;
                sprintf(name, "%s%.2i", tSockA, i);
                sock->a = NuSplineFind(gscn, name);
                if (sock->a == NULL) {
                    continue;
                }
                if ((i32)(i16)sock->a->length != len) {
                    continue;
                }
                sprintf(name, "%s%.2i", tSockB, i);
                sock->b = NuSplineFind(gscn, name);
                if (sock->b == NULL) {
                    continue;
                }
                if ((i32)(i16)sock->b->length != len) {
                    continue;
                }
                sprintf(name, "%s%.2i", tSockC, i);
                sock->c = NuSplineFind(gscn, name);
                if (sock->c == NULL) {
                    sock->flags |= 1;
                } else if ((i32)(i16)sock->c->length != len) {
                    sock->flags |= 1;
                }
                if ((sock->flags & 1) == 0) {
                    sprintf(name, "%s%.2i", tSockD, i);
                    sock->d = NuSplineFind(gscn, name);
                    if (sock->d == NULL) {
                        sock->flags |= 1;
                    } else if ((i32)(i16)sock->d->length != len) {
                        sock->flags |= 1;
                    }
                }
                sprintf(name, "%s%.2i", tSockMID, i);
                sock->mid = NuSplineFind(gscn, name);
                if ((sock->mid != NULL) && ((i32)(i16)sock->mid->length != len)) {
                    sock->mid = NULL;
                }
                sprintf(name, "%s%.2i", tSockLEFT, i);
                sock->left = NuSplineFind(gscn, name);
                if ((sock->left != NULL) && ((i32)(i16)sock->left->length != len)) {
                    sock->left = NULL;
                }
                sprintf(name, "%s%.2i", tSockRIGHT, i);
                sock->right = NuSplineFind(gscn, name);
                if ((sock->right != NULL) && ((i32)(i16)sock->right->length != len)) {
                    sock->right = NULL;
                }
                sprintf(name, "%s%.2i", tSockLOOK, i);
                sock->look = NuSplineFind(gscn, name);
                if ((sock->look != NULL) && ((i32)(i16)sock->look->length != len)) {
                    sock->look = NULL;
                }
                sprintf(name, "%s%.2i", tSockLATERAL, i);
                sock->lateral = NuSplineFind(gscn, name);
                if ((sock->lateral != NULL) && ((i32)(i16)sock->lateral->length != len)) {
                    sock->lateral = NULL;
                }
                sprintf(name, "%s%.2i", tSockTRACKIN, i);
                sock->trackin = NuSplineFind(gscn, name);
                if ((sock->trackin != NULL) && ((i32)(i16)sock->trackin->length != len)) {
                    sock->trackin = NULL;
                }
                sprintf(name, "%s%.2i", tSockLIMIT, i);
                sock->limit = NuSplineFind(gscn, name);
                sock->max = sock->a->pts[0];
                sock->min = sock->max;
                for (j = 0; j < len; j++) {
                    AdjustMinMaxBox(&sock->a->pts[j], &sock->min, &sock->max);
                    AdjustMinMaxBox(&sock->b->pts[j], &sock->min, &sock->max);
                    if ((sock->flags & 1) == 0) {
                        AdjustMinMaxBox(&sock->c->pts[j], &sock->min, &sock->max);
                        AdjustMinMaxBox(&sock->d->pts[j], &sock->min, &sock->max);
                    }
                }
                sock->center.x = (sock->min.x + sock->max.x) * 0.5f;
                sock->center.y = (sock->min.y + sock->max.y) * 0.5f;
                sock->center.z = (sock->min.z + sock->max.z) * 0.5f;
                sock->extent = sock->max.x - sock->center.x;
                extent_z = sock->max.z - sock->center.z;
                if (extent_z < sock->extent) {
                    sock->extent = extent_z;
                }
                sock->valid = 1;
                sock_sys->count++;
            }
        }
    }
}

SOCKSYS_PC *SockSysInit(VARIPTR *buf, VARIPTR buf_end, NUGSCN *gscn) {
    SOCKSYS_PC *sys;

    buf->addr = ((buf->addr + 3) & ~3u);
    if (buf->addr + sizeof(SOCKSYS_PC) + 64 * sizeof(SOCK_PC) >= buf_end.addr) {
        return NULL;
    }

    sys = (SOCKSYS_PC *)buf->void_ptr;
    buf->void_ptr = (char *)buf->void_ptr + sizeof(SOCKSYS_PC);
    sys->socks = (SOCK_PC *)buf->void_ptr;
    buf->void_ptr = (char *)buf->void_ptr + 64 * sizeof(SOCK_PC);
    sys->count = 0;
    memset(sys->socks, 0, 64 * sizeof(SOCK_PC));
    SockSysFindInScene(sys, gscn);
    return sys;
}

static void SockEdgeAnglesXY(NUVEC *from, NUVEC *to, u16 *x, u16 *y) {
    f32 dx;
    f32 dz;

    dx = to->x - from->x;
    dz = to->z - from->z;
    *x = (u16)-NuAtan2D(to->y - from->y, NuFsqrt(dx * dx + dz * dz));
    *y = (u16)NuAtan2D(dx, dz);
}

SOCKROT_PC *SockRailAngles(SOCK_PC *sock, NUGSPLINE *spline, SOCKROT_PC *rotations) {
    i32 point_count;

    point_count = sock->cam->length;
    {
        i32 point;

        for (point = 0; point < point_count; ++point) {
            i32 previous;
            i32 next;
            NUVEC previous_pos;
            NUVEC position;
            NUVEC next_pos;
            u16 x0;
            u16 y0;
            u16 x1;
            u16 y1;
            i32 average;

            previous = point - 1;
            if (previous < 0) {
                previous = point_count - 1;
            }
            next = point + 1;
            if (next >= point_count) {
                next = 0;
            }

            if (spline != NULL) {
                previous_pos = spline->pts[previous];
                position = spline->pts[point];
                next_pos = spline->pts[next];
            } else {
                {
                    NUVEC *midpoint = &previous_pos;
                    i32 midpoint_index = previous;
                    if (sock->mid != NULL) {
                        *midpoint = sock->mid->pts[midpoint_index];
                    } else {
                        NuVecAdd(midpoint, &sock->a->pts[midpoint_index], &sock->b->pts[midpoint_index]);
                        if ((sock->flags & 1) == 0) {
                            NuVecAdd(midpoint, midpoint, &sock->c->pts[midpoint_index]);
                            NuVecAdd(midpoint, midpoint, &sock->d->pts[midpoint_index]);
                            NuVecScale(midpoint, midpoint, 0.25f);
                        } else {
                            NuVecScale(midpoint, midpoint, 0.5f);
                        }
                    }
                }
                {
                    NUVEC *midpoint = &position;
                    i32 midpoint_index = point;
                    if (sock->mid != NULL) {
                        *midpoint = sock->mid->pts[midpoint_index];
                    } else {
                        NuVecAdd(midpoint, &sock->a->pts[midpoint_index], &sock->b->pts[midpoint_index]);
                        if ((sock->flags & 1) == 0) {
                            NuVecAdd(midpoint, midpoint, &sock->c->pts[midpoint_index]);
                            NuVecAdd(midpoint, midpoint, &sock->d->pts[midpoint_index]);
                            NuVecScale(midpoint, midpoint, 0.25f);
                        } else {
                            NuVecScale(midpoint, midpoint, 0.5f);
                        }
                    }
                }
                {
                    NUVEC *midpoint = &next_pos;
                    i32 midpoint_index = next;
                    if (sock->mid != NULL) {
                        *midpoint = sock->mid->pts[midpoint_index];
                    } else {
                        NuVecAdd(midpoint, &sock->a->pts[midpoint_index], &sock->b->pts[midpoint_index]);
                        if ((sock->flags & 1) == 0) {
                            NuVecAdd(midpoint, midpoint, &sock->c->pts[midpoint_index]);
                            NuVecAdd(midpoint, midpoint, &sock->d->pts[midpoint_index]);
                            NuVecScale(midpoint, midpoint, 0.25f);
                        } else {
                            NuVecScale(midpoint, midpoint, 0.5f);
                        }
                    }
                }
            }

            x0 = 0;
            y0 = 0;
            x1 = 0;
            y1 = 0;
            average = 1;
            if (point == 0 && sock->unknown_33 == 0) {
                SockEdgeAnglesXY(&position, &next_pos, &x0, &y0);
                average = 0;
            } else if (point == point_count - 1 && sock->unknown_33 == 0) {
                SockEdgeAnglesXY(&previous_pos, &position, &x0, &y0);
                average = 0;
            } else {
                SockEdgeAnglesXY(&previous_pos, &position, &x0, &y0);
                SockEdgeAnglesXY(&position, &next_pos, &x1, &y1);
            }

            rotations[0].x = x0;
            rotations[0].y = y0;
            if (average) {
                rotations[0].x = (u16)(x0 + SOCK_ROTATION_DELTA(x0, x1) / 2);
                rotations[0].y = (u16)(y0 + SOCK_ROTATION_DELTA(y0, y1) / 2);
            }
            ++rotations;
        }
    }

    return rotations;
}

f32 SplineLength(NUGSPLINE *spline, i32 closed) {
    i32 segment_count;
    f32 length;

    if (spline == NULL || spline->length < 2) {
        return 0.0f;
    }
    segment_count = closed == 0 ? spline->length - 1 : spline->length;
    length = 0.0f;
    {
        i32 segment;

        for (segment = 0; segment < segment_count; ++segment) {
            i32 next;

            next = segment == spline->length - 1 ? 0 : segment + 1;
            length += NuVecDist(&spline->pts[segment], &spline->pts[next], NULL);
        }
    }

    return length;
}

void SockSys_Configure(SOCKSYS_PC *sock_sys, char *config, i32 unused, VARIPTR *buf, VARIPTR *buf_end, NUGSCN *gscn) {
    NUFPAR_PC *parser;
    i32 inside_sock;

    if (sock_sys == NULL || config == NULL || gscn == NULL) {
        return;
    }

    parser = NuFParCreateMem("socks", config, 0xffff);
    if (parser == NULL) {
        return;
    }

    sockpar_scene = gscn;
    sockpar_buffer_ptr = buf;
    sockpar_buffer_end = buf_end;
    NuFParPushComCTX(parser, SockSys_ConfigKeywords);

    inside_sock = 0;
    while (NuFParGetLine(parser) != 0) {
        NuFParGetWord(parser);
        if (parser->word_buffer_ptr[0] == '\0') {
            continue;
        }

        if (inside_sock) {
            if (NuStrICmp(parser->word_buffer_ptr, "sock_end") == 0) {
                inside_sock = 0;
            } else {
                NuFParInterpretWordCTX(parser, NULL);
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "sock_start") == 0) {
            i32 sock_index;

            sock_index = NuFParGetInt(parser);
            if (sock_index >= 0 && sock_index < 0x40) {
                inside_sock = 1;
                sockpar_sock = &sock_sys->socks[sock_index];
            }
        }
    }
    NuFParDestroy(parser);
}

void SockSys_GenerateData(SOCKSYS_PC *sock_sys, VARIPTR *buf, VARIPTR buf_end) {
    SOCK_PC *sock;
    i32 sock_index;

    if (sock_sys == NULL) {
        return;
    }

    sock = sock_sys->socks;
    for (sock_index = 0; sock_index < 0x40; ++sock_index, ++sock) {
        SOCKSEGMENT_PC *segment;
        i32 segment_index;
        f32 distance_from_start;

        if (sock->valid == 0) {
            continue;
        }

        if (buf->addr + sock->cam->length * sizeof(SOCKROT_PC) >= buf_end.addr) {
            return;
        }
        sock->cam_rotations = (SOCKROT_PC *)buf->void_ptr;
        buf->void_ptr = SockRailAngles(sock, sock->cam, sock->cam_rotations);

        if (buf->addr + sock->cam->length * sizeof(SOCKROT_PC) >= buf_end.addr) {
            return;
        }
        sock->mid_rotations = (SOCKROT_PC *)buf->void_ptr;
        buf->void_ptr = SockRailAngles(sock, sock->mid, sock->mid_rotations);

        if (buf->addr + sock->cam->length * sizeof(SOCKSEGMENT_PC) >= buf_end.addr) {
            return;
        }
        sock->segments = (SOCKSEGMENT_PC *)buf->void_ptr;
        segment = sock->segments;
        distance_from_start = 0.0f;
        for (segment_index = 0; segment_index < sock->cam->length; ++segment_index, ++segment) {
            i32 next = (segment_index + 1) % sock->cam->length;

            if ((sock->flags & 1) == 0) {
                NuVecSurfaceNormal(&segment->planes[0], &sock->a->pts[segment_index], &sock->b->pts[segment_index],
                                   &sock->d->pts[segment_index]);
                NuVecSurfaceNormal(&segment->planes[1], &sock->a->pts[next], &sock->b->pts[next],
                                   &sock->b->pts[segment_index]);
                NuVecSurfaceNormal(&segment->planes[2], &sock->b->pts[next], &sock->c->pts[next],
                                   &sock->c->pts[segment_index]);
                NuVecSurfaceNormal(&segment->planes[3], &sock->c->pts[next], &sock->d->pts[next],
                                   &sock->d->pts[segment_index]);
                NuVecSurfaceNormal(&segment->planes[4], &sock->d->pts[next], &sock->a->pts[next],
                                   &sock->a->pts[segment_index]);
                NuVecSurfaceNormal(&segment->planes[5], &sock->b->pts[next], &sock->a->pts[next], &sock->c->pts[next]);
                NuVecAdd(&segment->midpoint, &sock->a->pts[segment_index], &sock->b->pts[segment_index]);
                NuVecAdd(&segment->midpoint, &segment->midpoint, &sock->c->pts[segment_index]);
                NuVecAdd(&segment->midpoint, &segment->midpoint, &sock->d->pts[segment_index]);
                NuVecScale(&segment->midpoint, &segment->midpoint, 0.25f);
                NuVecAdd(&segment->next_midpoint, &sock->a->pts[next], &sock->b->pts[next]);
                NuVecAdd(&segment->next_midpoint, &segment->next_midpoint, &sock->c->pts[next]);
                NuVecAdd(&segment->next_midpoint, &segment->next_midpoint, &sock->d->pts[next]);
                NuVecScale(&segment->next_midpoint, &segment->next_midpoint, 0.25f);
                segment->max = sock->a->pts[segment_index];
                segment->min = segment->max;
                AdjustMinMaxBox(&sock->b->pts[segment_index], &segment->min, &segment->max);
                AdjustMinMaxBox(&sock->c->pts[segment_index], &segment->min, &segment->max);
                AdjustMinMaxBox(&sock->d->pts[segment_index], &segment->min, &segment->max);
                AdjustMinMaxBox(&sock->a->pts[next], &segment->min, &segment->max);
                AdjustMinMaxBox(&sock->b->pts[next], &segment->min, &segment->max);
                AdjustMinMaxBox(&sock->c->pts[next], &segment->min, &segment->max);
                AdjustMinMaxBox(&sock->d->pts[next], &segment->min, &segment->max);
            } else {
                NuVecAdd(&segment->midpoint, &sock->a->pts[segment_index], &sock->b->pts[segment_index]);
                NuVecScale(&segment->midpoint, &segment->midpoint, 0.5f);
                NuVecAdd(&segment->next_midpoint, &sock->a->pts[next], &sock->b->pts[next]);
                NuVecScale(&segment->next_midpoint, &segment->next_midpoint, 0.5f);
                segment->max = sock->a->pts[segment_index];
                segment->min = segment->max;
                AdjustMinMaxBox(&sock->b->pts[segment_index], &segment->min, &segment->max);
                AdjustMinMaxBox(&sock->a->pts[next], &segment->min, &segment->max);
                AdjustMinMaxBox(&sock->b->pts[next], &segment->min, &segment->max);
            }

            if (sock->mid != NULL) {
                segment->length = NuVecDist(&sock->mid->pts[segment_index], &sock->mid->pts[next], NULL);
            } else {
                segment->length = NuVecDist(&sock->segments[segment_index].midpoint,
                                            &sock->segments[segment_index].next_midpoint, NULL);
            }
            segment->distance_from_start = distance_from_start;
            distance_from_start += segment->length;
        }

        if (sock->mid != NULL) {
            sock->unknown_98 = SplineLength(sock->mid, sock->unknown_33);
        } else {
            i32 segment_count;

            sock->unknown_98 = 0.0f;
            segment_count = sock->length;
            if (sock->unknown_33 != 0) {
                ++segment_count;
            }
            for (segment_index = 0; segment_index < segment_count; ++segment_index) {
                sock->unknown_98 += NuVecDist(&sock->segments[segment_index].midpoint,
                                              &sock->segments[segment_index].next_midpoint, NULL);
            }
        }
        buf->void_ptr = segment;
    }

    buf->addr = ((buf->addr + 15) & ~15u);
    sock = sock_sys->socks;
    for (sock_index = 0; sock_index < 0x40; ++sock_index, ++sock) {
        SOCK_PC *other;
        i32 other_index;

        if (sock->valid == 0) {
            continue;
        }
        other = sock_sys->socks;
        for (other_index = 0; other_index < 0x40; ++other_index, ++other) {
            i32 ignore_y;
            i32 segment_count;
            i32 other_segment_count;
            i32 segment;

            if (sock_index == other_index || other->valid == 0) {
                continue;
            }
            if ((sock->flags & 1) != 0 || (other->flags & 1) != 0) {
                ignore_y = 1;
            } else {
                ignore_y = 0;
            }
            if (!SOCK_BOUNDS_SEPARATE(&sock->min, &sock->max, &other->min, &other->max, ignore_y)) {
                segment_count = sock->length;
                if (sock->unknown_33 != 0) {
                    ++segment_count;
                }
                other_segment_count = other->length;
                if (other->unknown_33 != 0) {
                    ++other_segment_count;
                }
                for (segment = 0; segment < segment_count; ++segment) {
                    i32 other_segment;

                    for (other_segment = 0; other_segment < other_segment_count; ++other_segment) {
                        if (SOCK_BOUNDS_TOUCH(&sock->segments[segment].min, &sock->segments[segment].max,
                                              &other->segments[other_segment].min, &other->segments[other_segment].max,
                                              ignore_y)) {
                            goto next_other;
                        }
                    }
                }
            }
            sock->overlap_exclusion_mask[other_index / 32] |= 1u << (other_index & 31);
        next_other:;
        }
    }
}
SOCKPOSITION_PC TempSPosList[4] = {0};
SOCKPOSITION_PC *TempBestSPos = 0;
i32 TempSPosCount = 0;
NUVEC temp_pos = {0};
u32 TempSLoc[2] = {0};
i32 complexsockposition_forcesock = -1;
static NUVEC temp_sockmidpos = {0};
static NUVEC temp_sockcampos = {0};

static i32 OnOrOutsidePlane(NUVEC *point, NUVEC *plane_point, NUVEC *normal) {
    return normal->x * (point->x - plane_point->x) + normal->y * (point->y - plane_point->y) +
               normal->z * (point->z - plane_point->z) >=
           0.0f;
}

static i32 InsideLineXZ(f32 x, f32 z, f32 x0, f32 z0, f32 x1, f32 z1) {
    return (x - x0) * (z1 - z0) + (x0 - x1) * (z - z0) >= 0.0f;
}

static f32 DistanceToPlane(NUVEC *point, NUVEC *plane_point, NUVEC *normal) {
    return normal->x * (point->x - plane_point->x) + normal->y * (point->y - plane_point->y) +
           normal->z * (point->z - plane_point->z);
}

f32 RatioBetweenPlanes(NUVEC *point, NUVEC *point_a, NUVEC *normal_a, NUVEC *point_b, NUVEC *normal_b) {
    f32 distance_a;
    f32 distance_b;

    distance_a = DistanceToPlane(point, point_a, normal_a);
    distance_b = DistanceToPlane(point, point_b, normal_b);
    return distance_a / (distance_a + distance_b);
}

static f32 SockDistanceToLineXZ(NUVEC *point, NUVEC *line_a, NUVEC *line_b) {
    u16 angle;
    union {
        f32 value;
        u32 bits;
    } result, absolute;
    angle = (u16)-NuAtan2D(line_b->x - line_a->x, line_b->z - line_a->z);
    result.value = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * (point->x - line_a->x) +
                   NuTrigTable[angle >> 1] * (point->z - line_a->z);
    absolute.bits = result.bits & 0x7fffffffu;
    return absolute.value;
}

static f32 RatioBetweenEdgesXZ(NUVEC *point, NUVEC *edge_a0, NUVEC *edge_a1, NUVEC *edge_b0, NUVEC *edge_b1) {
    f32 distance_a;
    f32 distance_b;

    distance_a = SockDistanceToLineXZ(point, edge_a0, edge_a1);
    distance_b = SockDistanceToLineXZ(point, edge_b0, edge_b1);
    return distance_a / (distance_a + distance_b);
}

void SockSysPointAlongSpline(NUVEC *result, NUGSPLINE *spline, i32 segment, i32 next_segment, f32 ratio) {
    NUVEC *from;
    NUVEC *to;

    from = &spline->pts[segment];
    to = &spline->pts[next_segment];
    result->x = from->x + (to->x - from->x) * ratio;
    result->y = from->y + (to->y - from->y) * ratio;
    result->z = from->z + (to->z - from->z) * ratio;
}

static void SockSysPointAlongMID(SOCK_PC *sock, SOCKPOSITION_PC *position, NUVEC *result) {
    NUVEC *from;
    NUVEC *to;

    if (sock->mid == NULL) {
        from = &sock->segments[position->segment].midpoint;
        to = &sock->segments[position->segment].next_midpoint;
    } else {
        from = &sock->mid->pts[position->segment];
        to = &sock->mid->pts[position->next_segment];
    }
    if (result == NULL) {
        result = &temp_sockmidpos;
    }
    result->x = from->x + (to->x - from->x) * position->ratio;
    result->y = from->y + (to->y - from->y) * position->ratio;
    result->z = from->z + (to->z - from->z) * position->ratio;
}

static void FillSockPosition(SOCKSYS_PC *sock_sys, SOCKPOSITION_PC *position) {
    SOCK_PC *sock;
    i32 segment;
    i32 next;
    SOCKROT_PC *cam_from;
    SOCKROT_PC *cam_to;
    SOCKROT_PC *mid_from;
    SOCKROT_PC *mid_to;
    i32 divisor;

    if (sock_sys == NULL) {
        return;
    }
    sock = &sock_sys->socks[position->sock_index];
    segment = position->segment;
    next = position->next_segment;
    SockSysPointAlongSpline(&temp_sockcampos, sock->cam, segment, next, position->ratio);

    cam_from = &sock->cam_rotations[segment];
    cam_to = &sock->cam_rotations[next];
    position->camera_rotation.x =
        (u16)(cam_from->x + (f32)SOCK_ROTATION_DELTA(cam_from->x, cam_to->x) * position->ratio);
    position->camera_rotation.y =
        (u16)(cam_from->y + (f32)SOCK_ROTATION_DELTA(cam_from->y, cam_to->y) * position->ratio);

    mid_from = &sock->mid_rotations[segment];
    mid_to = &sock->mid_rotations[next];
    position->midpoint_rotation.x =
        (u16)(mid_from->x + (f32)SOCK_ROTATION_DELTA(mid_from->x, mid_to->x) * position->ratio);
    position->midpoint_rotation.y =
        (u16)(mid_from->y + (f32)SOCK_ROTATION_DELTA(mid_from->y, mid_to->y) * position->ratio);

    position->distance = (f32)segment + position->ratio;
    divisor = sock->length + (sock->unknown_33 != 0 ? 1 : 0);
    position->normalized_distance = position->distance / (f32)divisor;
}

static f32 BestSockPosition(SOCKSYS_PC *sock_sys, NUVEC *point, SOCKPOSITION_PC *result, i32 sock_index,
                            i32 prior_segment) {
    SOCK_PC *sock;
    i32 segment_count;
    i32 initial_segment;
    i32 ascending_segment;
    i32 descending_segment;
    i32 direction;
    i32 exhausted_directions;

    sock = &sock_sys->socks[sock_index];
    result->sock_index = -1;
    if (sock->valid == 0) {
        return 0.0f;
    }

    if ((sock->flags & 1) == 0) {
        if (sock->min.x > point->x || point->x > sock->max.x || sock->min.y > point->y || point->y > sock->max.y ||
            sock->min.z > point->z || point->z > sock->max.z)
            return 0.0f;
    } else {
        if (sock->min.x > point->x || point->x > sock->max.x || sock->min.z > point->z || point->z > sock->max.z)
            return 0.0f;
    }

    segment_count = sock->length + (sock->unknown_33 != 0 ? 1 : 0);
    initial_segment = prior_segment == -1 ? segment_count / 2 : prior_segment;
    ascending_segment = initial_segment + 1;
    descending_segment = initial_segment - 1;
    direction = 0;
    exhausted_directions = 0;

    for (;;) {
        i32 segment;

        if (direction == 0) {
            segment = initial_segment;
        } else if (direction == 1) {
            segment = ascending_segment++;
        } else {
            segment = descending_segment--;
        }

        if (segment < 0 || segment >= segment_count) {
            exhausted_directions |= direction;
        } else {
            i32 next;
            NUVEC *a0;
            NUVEC *b0;
            NUVEC *a1;
            NUVEC *b1;
            SOCKSEGMENT_PC *segment_data;
            i32 inside;

            next = segment + 1;
            if (next == segment_count && sock->unknown_33 != 0) {
                next = 0;
            }

            a0 = &sock->a->pts[segment];
            b0 = &sock->b->pts[segment];
            a1 = &sock->a->pts[next];
            b1 = &sock->b->pts[next];
            segment_data = &sock->segments[segment];
            inside = 0;

            if ((sock->flags & 1) == 0) {
                if (point->x >= segment_data->min.x && point->x <= segment_data->max.x &&
                    point->y >= segment_data->min.y && point->y <= segment_data->max.y &&
                    point->z >= segment_data->min.z && point->z <= segment_data->max.z) {
                    NUVEC *c1;
                    NUVEC *d1;

                    c1 = &sock->c->pts[next];
                    d1 = &sock->d->pts[next];
                    inside = OnOrOutsidePlane(point, a0, &segment_data->planes[0]) &&
                             OnOrOutsidePlane(point, a1, &segment_data->planes[1]) &&
                             OnOrOutsidePlane(point, b1, &segment_data->planes[2]) &&
                             OnOrOutsidePlane(point, c1, &segment_data->planes[3]) &&
                             OnOrOutsidePlane(point, d1, &segment_data->planes[4]) &&
                             OnOrOutsidePlane(point, b1, &segment_data->planes[5]);
                }
            } else if (segment_data->min.x <= point->x && point->x <= segment_data->max.x &&
                       segment_data->min.z <= point->z && point->z <= segment_data->max.z) {
                inside = InsideLineXZ(point->x, point->z, b0->x, b0->z, a0->x, a0->z) &&
                         InsideLineXZ(point->x, point->z, a0->x, a0->z, a1->x, a1->z) &&
                         InsideLineXZ(point->x, point->z, a1->x, a1->z, b1->x, b1->z) &&
                         InsideLineXZ(point->x, point->z, b1->x, b1->z, b0->x, b0->z);
            }

            if (inside) {
                f32 distance_squared;

                result->sock_index = (i8)sock_index;
                result->segment = (i16)segment;
                if ((sock->flags & 1) == 0) {
                    result->ratio = RatioBetweenPlanes(point, a0, &segment_data->planes[0], &sock->c->pts[next],
                                                       &segment_data->planes[5]);
                } else {
                    result->ratio = RatioBetweenEdgesXZ(point, b0, a0, b1, a1);
                }
                result->next_segment = (i16)next;
                SockSysPointAlongMID(sock, result, &temp_sockmidpos);
                distance_squared = NuVecDistSqr(point, &temp_sockmidpos, NULL);
                FillSockPosition(sock_sys, result);
                memcpy(TempSLoc, result, sizeof(TempSLoc));
                return distance_squared;
            }
        }

        if (exhausted_directions == 3) {
            return 0.0f;
        }
        direction = direction == 1 ? 2 : 1;
    }
}

void ComplexSockPosition(SOCKSYS_PC *sock_sys, NUVEC *position, i32 prior_sock, i32 prior_segment,
                         SOCKPOSITION_PC *result) {
    i32 original_prior_sock;
    i32 forced_sock;
    i32 sockets_to_check;
    i32 fallback_search;

    if (sock_sys == NULL) {
        result->sock_index = -1;
        result->segment = -1;
        result->flags = 0;
        return;
    }

    original_prior_sock = prior_sock;
    forced_sock = complexsockposition_forcesock;
    complexsockposition_forcesock = -1;
    memset(TempSPosList, 0, sizeof(TempSPosList));
    temp_pos = *position;

    sockets_to_check = 0x40;
    if (forced_sock != -1) {
        prior_sock = forced_sock;
        prior_segment = -1;
        sockets_to_check = 1;
    } else if (prior_sock == -1) {
        prior_sock = 0;
        prior_segment = -1;
    }

    fallback_search = 0;
    for (;;) {
        i32 candidate_count;
        u32 candidate_mask;
        i32 excluded_socket_seen;
        SOCKPOSITION_PC *best;
        f32 best_distance;

        candidate_count = 0;
        candidate_mask = 0;
        excluded_socket_seen = 0;
        best = NULL;
        best_distance = 1000000.0f;

        {
            i32 checked;

            for (checked = 0; checked < sockets_to_check; ++checked) {
                SOCK_PC *sock;

                sock = &sock_sys->socks[prior_sock];
                if ((sock->flags & 0x100) == 0) {
                    i32 excluded;

                    excluded = forced_sock == -1 && !fallback_search && original_prior_sock != -1 &&
                               prior_sock != original_prior_sock &&
                               (sock_sys->socks[original_prior_sock].overlap_exclusion_mask[prior_sock / 32] &
                                (1u << (prior_sock & 31))) != 0;
                    if (excluded) {
                        excluded_socket_seen = 1;
                    } else {
                        f32 distance;
                        SOCKPOSITION_PC *candidate;

                        distance = BestSockPosition(sock_sys, position, &TempSPosList[candidate_count], prior_sock,
                                                    prior_segment);
                        candidate = &TempSPosList[candidate_count];
                        if (candidate->sock_index != -1) {
                            candidate->midpoint = temp_sockmidpos;
                            candidate->camera_position = temp_sockcampos;
                            if (best == NULL || distance < best_distance) {
                                best = candidate;
                                best_distance = distance;
                            }
                            candidate_mask |= 1U << (prior_sock & 0x1f);
                            ++candidate_count;
                            if (candidate_count == 4) {
                                break;
                            }
                        }
                    }
                }
                prior_sock = (prior_sock + 1) % 0x40;
                prior_segment = -1;
            }
        }

        if (best != NULL) {
            *result = *best;
            result->candidate_mask = candidate_mask;
            result->flags = fallback_search ? 1 : 0;
            result->candidate_count = (i8)candidate_count;
            TempBestSPos = best;
            TempSPosCount = candidate_count;
            memcpy(TempSLoc, result, sizeof(TempSLoc));
            return;
        }
        if (!excluded_socket_seen) {
            result->sock_index = -1;
            result->segment = -1;
            result->flags = 0;
            result->candidate_count = 0;
            TempBestSPos = NULL;
            TempSPosCount = 0;
            memcpy(TempSLoc, result, sizeof(TempSLoc));
            return;
        }

        fallback_search = 1;
        prior_sock = 0;
        prior_segment = -1;
        forced_sock = -1;
        sockets_to_check = 0x40;
    }
}

void ComplexSockAngles(SOCKROT_PC *angles) {
    SOCKPOSITION_PC *positions;
    i32 count;

    positions = TempSPosList;
    count = TempSPosCount;
    if (count > 0) {
        angles->x = positions[0].midpoint_rotation.x;
        angles->y = positions[0].midpoint_rotation.y;
        if (count > 1) {
            NUVEC midpoint;

            midpoint = positions[0].midpoint;
            {
                i32 i;

                for (i = 1; i < count; ++i) {
                    NUVEC from;
                    NUVEC to;
                    NUVEC direction;
                    NUVEC offset;
                    f32 distance;
                    f32 ratio;
                    f32 projection;

                    from = midpoint;
                    to = positions[i].midpoint;

                    NuVecSub(&direction, &to, &from);
                    distance =
                        NuFsqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
                    ratio = distance == 0.0f ? 0.0f : 1.0f / distance;
                    direction.x *= ratio;
                    direction.y *= ratio;
                    direction.z *= ratio;
                    NuVecSub(&offset, &temp_pos, &from);
                    projection = offset.x * direction.x + offset.y * direction.y + offset.z * direction.z;
                    if (projection > 0.0f) {
                        if (projection >= 1.0f) {
                            angles->x = positions[i].midpoint_rotation.x;
                            angles->y = positions[i].midpoint_rotation.y;
                            midpoint = to;
                        } else {
                            ratio = projection / distance;
                            angles->x +=
                                (u16)((f32)(SOCK_ROTATION_DELTA(angles->x, positions[i].midpoint_rotation.x)) * ratio);
                            angles->y +=
                                (u16)((f32)(SOCK_ROTATION_DELTA(angles->y, positions[i].midpoint_rotation.y)) * ratio);
                            midpoint.x += (to.x - midpoint.x) * ratio;
                            midpoint.y += (to.y - midpoint.y) * ratio;
                            midpoint.z += (to.z - midpoint.z) * ratio;
                        }
                    }
                }
            }
        }
    }
}

f32 MidDistanceFromSockStart(SOCKSYS_PC *system, SOCKPOSITION_PC *position) {
    if (system != NULL && position->sock_index != -1) {
        SOCKSEGMENT_PC *segment;

        segment = &system->socks[position->sock_index].segments[position->segment];
        return segment->distance_from_start + segment->length * position->ratio;
    }
    return 0.0f;
}

void MoveSockPosition(SOCKSYS_PC *system, SOCKPOSITION_PC *source, f32 distance, SOCKPOSITION_PC *result) {
    i32 direction;
    i32 length;
    NUVEC *from;
    NUVEC *to;
    SOCK_PC *sock;
    NUVEC *point;

    *result = *source;
    if (system == NULL || result->sock_index == -1 || distance == 0.0f)
        return;

    if (distance > 0.0f)
        direction = 1;
    else {
        direction = 2;
        distance = -distance;
    }

    sock = &system->socks[result->sock_index];
    point = &result->midpoint;
    if (sock->unknown_33 != 0)
        length = sock->length + 1;
    else
        length = sock->length;
    for (;;) {
        NUVEC *target;
        f32 remaining;

        result->next_segment = (i16)(result->segment + 1);
        if (result->next_segment == length && sock->unknown_33 != 0)
            result->next_segment = 0;
        if (sock->mid != NULL) {
            from = &sock->mid->pts[result->segment];
            to = &sock->mid->pts[result->next_segment];
        } else {
            from = &sock->segments[result->segment].midpoint;
            to = &sock->segments[result->segment].next_midpoint;
        }
        target = direction == 1 ? to : from;
        remaining = NuVecDist(point, target, NULL);
        if (distance > remaining) {
            *point = *target;
            result->ratio = 0.0f;
            distance -= remaining;
            if (direction == 1) {
                if (sock->unknown_33 != 0) {
                    ++result->segment;
                    if (result->segment == length)
                        result->segment = 0;
                } else {
                    if (result->segment >= length)
                        break;
                    ++result->segment;
                }
            } else {
                if (sock->unknown_33 != 0) {
                    --result->segment;
                    if (result->segment == -1)
                        result->segment = sock->length;
                } else {
                    if (result->segment <= 0)
                        break;
                    --result->segment;
                }
            }
            continue;
        }
        if (remaining > 0.0f && distance > 0.0f) {
            f32 ratio;

            ratio = distance / remaining;
            point->x += (target->x - point->x) * ratio;
            point->y += (target->y - point->y) * ratio;
            point->z += (target->z - point->z) * ratio;
            if (sock->mid != NULL) {
                from = &sock->mid->pts[result->segment];
                to = &sock->mid->pts[result->next_segment];
            } else {
                from = &sock->segments[result->segment].midpoint;
                to = &sock->segments[result->segment].next_midpoint;
            }
            result->ratio = NuVecDist(from, point, NULL) / NuVecDist(from, to, NULL);
        } else {
            result->ratio = 0.0f;
        }
        break;
    }
    FillSockPosition(system, result);
    result->camera_position = temp_sockcampos;
}

i32 SockSysTrackInSplineInfo(SOCKSYS_PC *system, SOCKPOSITION_PC *position, NUVEC *point, f32 *distance) {
    if (position != NULL && position->sock_index != -1 && system != NULL &&
        system->socks[position->sock_index].valid != 0 && system->socks[position->sock_index].trackin != NULL) {
        NUVEC temporary;

        if (point == NULL)
            point = &temporary;
        SockSysPointAlongSpline(point, system->socks[position->sock_index].trackin, position->segment,
                                position->next_segment, position->ratio);
        if (distance != NULL)
            *distance = NuVecDist(point, &position->midpoint, NULL);
        return 1;
    }
    return 0;
}
void SockRotationMatrix(SOCKSYS_PC *system, SOCKPOSITION_PC *position, NUMTX *out, i32 stride, i32 mode) {
    i32 first;
    i32 last;
    SOCK_PC *sock;
    i32 segment;
    i32 samples[4];
    NUMTX matrices[4];
    NUQUAT quats[4];
    NUQUAT result;
    f32 widths[4];
    f32 heights[4];
    NUVEC right;
    NUVEC up;
    NUVEC forward;
    NUVEC *a;
    NUVEC *b;
    NUVEC *c;
    NUVEC *d;
    f32 base_width;
    f32 base_height;
    f32 ratio;

    NuMtxSetIdentity(out);
    if (position->sock_index == -1)
        return;
    first = (mode == 4 || mode == 5) ? 0 : 1;
    last = (mode == 4 || mode == 5) ? 3 : 2;
    sock = &system->socks[position->sock_index];
    segment = position->segment;

    samples[1] = segment - segment % stride;
    samples[2] = samples[1] + stride;
    samples[3] = samples[2] + stride;
    samples[0] = samples[1] - stride;
    if (samples[0] < 0)
        samples[0] = 0;
    if (samples[2] > sock->length)
        samples[2] = sock->length;
    if (samples[3] > sock->length)
        samples[3] = sock->length;

    {
        i32 i;

        for (i = first; i <= last; ++i) {
            NUVEC *a;
            NUVEC *b;
            NUVEC *c;
            NUVEC *d;

            NuMtxSetIdentity(&matrices[i]);
            a = &sock->a->pts[samples[i]];
            b = &sock->b->pts[samples[i]];
            c = &sock->c->pts[samples[i]];
            d = &sock->d->pts[samples[i]];
            right.x = (b->x - a->x) + (c->x - d->x);
            right.y = (b->y - a->y) + (c->y - d->y);
            right.z = (b->z - a->z) + (c->z - d->z);
            up.x = (b->x - c->x) + (a->x - d->x);
            up.y = (b->y - c->y) + (a->y - d->y);
            up.z = (b->z - c->z) + (a->z - d->z);
            widths[i] = NuVecMag(&right);
            heights[i] = NuVecMag(&up);
            NuVecNorm(&right, &right);
            NuVecNorm(&up, &up);
            NuVecCross(&forward, &right, &up);
            NuVecNorm(&forward, &forward);
            NuVecCross(&up, &forward, &right);
            NuVecNorm(&up, &up);
            matrices[i].m00 = right.x;
            matrices[i].m01 = right.y;
            matrices[i].m02 = right.z;
            matrices[i].m10 = up.x;
            matrices[i].m11 = up.y;
            matrices[i].m12 = up.z;
            matrices[i].m20 = forward.x;
            matrices[i].m21 = forward.y;
            matrices[i].m22 = forward.z;
            NuMtxToQuat(&matrices[i], &quats[i]);
        }
    }

    a = &sock->a->pts[0];
    b = &sock->b->pts[0];
    c = &sock->c->pts[0];
    d = &sock->d->pts[0];
    right.x = (b->x - a->x) + (c->x - d->x);
    right.y = (b->y - a->y) + (c->y - d->y);
    right.z = (b->z - a->z) + (c->z - d->z);
    up.x = (b->x - c->x) + (a->x - d->x);
    up.y = (b->y - c->y) + (a->y - d->y);
    up.z = (b->z - c->z) + (a->z - d->z);
    base_width = NuVecMag(&right);
    base_height = NuVecMag(&up);
    ratio = position->ratio;
    ratio += (f32)(segment % stride);
    ratio /= (f32)stride;
    if (sock->flags & 0x200) {
        f32 width;
        f32 height;

        width = (1.0f - ratio) * (widths[1] / base_width) + (widths[2] / base_width) * ratio;
        if (1.0f >= width)
            sock->camera_local_x_ratio = sock->camera_lateral_ratio * width;
        else
            sock->camera_local_x_ratio =
                sock->camera_lateral_ratio + (1.0f - 1.0f / width) * (1.0f - sock->camera_lateral_ratio);
        height = (1.0f - ratio) * (heights[1] / base_height) + (heights[2] / base_height) * ratio;
        if (1.0f >= height)
            sock->camera_vertical_ratio = sock->camera_lateral_ratio * height;
        else
            sock->camera_vertical_ratio =
                sock->camera_lateral_ratio + (1.0f - 1.0f / height) * (1.0f - sock->camera_lateral_ratio);
        if (width > 1.0f)
            sock->look_ratio_xz = sock->unknown_80 / width;
        if (height > 1.0f)
            sock->look_ratio_y = sock->unknown_84 / height;
    } else {
        sock->camera_vertical_ratio = sock->camera_lateral_ratio;
        sock->camera_local_x_ratio = sock->camera_vertical_ratio;
    }
    if (mode == 1) {
        out->m00 = matrices[1].m00 + (matrices[2].m00 - matrices[1].m00) * ratio;
        out->m01 = matrices[1].m01 + (matrices[2].m01 - matrices[1].m01) * ratio;
        out->m02 = matrices[1].m02 + (matrices[2].m02 - matrices[1].m02) * ratio;
        out->m10 = matrices[1].m10 + (matrices[2].m10 - matrices[1].m10) * ratio;
        out->m11 = matrices[1].m11 + (matrices[2].m11 - matrices[1].m11) * ratio;
        out->m12 = matrices[1].m12 + (matrices[2].m12 - matrices[1].m12) * ratio;
        out->m20 = matrices[1].m20 + (matrices[2].m20 - matrices[1].m20) * ratio;
        out->m21 = matrices[1].m21 + (matrices[2].m21 - matrices[1].m21) * ratio;
        out->m22 = matrices[1].m22 + (matrices[2].m22 - matrices[1].m22) * ratio;
        NuVecNorm((NUVEC *)&out->m00, (NUVEC *)&out->m00);
        NuVecNorm((NUVEC *)&out->m10, (NUVEC *)&out->m10);
        NuVecNorm((NUVEC *)&out->m20, (NUVEC *)&out->m20);
    } else if (mode == 2) {
        NuQuatLerp(&result, &quats[1], &quats[2], ratio);
        NuQuatToMtx(&result, out);
    } else if (mode == 3) {
        NuQuatSlerp(&result, &quats[1], &quats[2], ratio);
        NuQuatToMtx(&result, out);
    } else if (mode == 4) {
        NuQuatCubicInt(&result, &quats[0], &quats[1], &quats[2], &quats[3], ratio);
        NuQuatToMtx(&result, out);
    } else if (mode == 5) {
        NuQuatHermiteInt(&result, &quats[0], &quats[1], &quats[2], &quats[3], ratio);
        NuQuatToMtx(&result, out);
    } else {
        *out = matrices[1];
    }
}

void SetSockPostion(SOCKSYS_PC *system, SOCKPOSITION_PC *position, i32 index, i32 segment, f32 fraction) {
    SOCK_PC *sock;
    i32 limit;

    position->sock_index = -1;
    if (system == NULL || (u32)index > 63u) {
        return;
    }
    sock = &system->socks[index];
    limit = sock->length;
    if (sock->unknown_33 != 0) {
        ++limit;
        if (segment > limit) {
            segment = 0;
        }
    } else if (segment > limit) {
        segment = limit;
    }
    if (segment < 0) {
        segment = 0;
    }
    if (fraction < 0.0f) {
        fraction = 0.0f;
    } else if (fraction > 1.0f) {
        fraction = 1.0f;
    }
    position->next_segment = (i16)(u16)((u32)segment + 1u);
    position->ratio = fraction;
    position->sock_index = (i8)index;
    position->segment = (i16)segment;
    if ((i32)position->next_segment == limit && sock->unknown_33 != 0) {
        position->next_segment = 0;
    }
    position->candidate_count = 1;
    position->flags = 0;
    position->candidate_mask = 1u << ((u32)index & 31u);
    FillSockPosition(system, position);
    position->camera_position = temp_sockcampos;
    SockSysPointAlongMID(sock, position, &position->midpoint);
}

STATIC_ASSERT(offsetof(SOCK_PC, length) == 0x30);
STATIC_ASSERT(offsetof(SOCK_PC, looping) == 0x33);
STATIC_ASSERT(offsetof(SOCK_PC, look_ahead_segments) == 0x6d);

void __cdecl PerspectMidPoint(NUVEC *result, NUVEC *first, NUVEC *second, NUVEC *camera_position) {
    f32 first_distance = NuVecDist(camera_position, first, NULL);
    volatile f32 second_distance = NuVecDist(camera_position, second, NULL);
    f32 ratio = (f32)((f64)first_distance / ((f64)second_distance + first_distance));
    result->x = (f32)(((f64)second->x - first->x) * ratio + first->x);
    result->y = (f32)(((f64)second->y - first->y) * ratio + first->y);
    result->z = (f32)(((f64)second->z - first->z) * ratio + first->z);
}

i32 __cdecl SockSegmentsAhead(SOCK_PC *sock, i32 segment, i32 *from_segment, i32 *to_segment) {
    i32 segments_ahead;
    i32 advanced = 0;
    if (sock->look_ahead_segments == 0)
        return 0;
    *from_segment = segment;
    segments_ahead = sock->look_ahead_segments;
    if (sock->looping != 0) {
        while (segments_ahead > 0) {
            ++*from_segment;
            if (*from_segment == sock->length + 1)
                *from_segment = 0;
            --segments_ahead;
        }
        *to_segment = *from_segment + 1;
        if (*to_segment == sock->length + 1)
            *to_segment = 0;
        return 1;
    }
    while (segments_ahead > 0) {
        if (*from_segment >= sock->length - 1)
            break;
        ++*from_segment;
        --segments_ahead;
        *to_segment = *from_segment + 1;
        advanced = 1;
    }
    return advanced;
}

/* Append to canonical socksys.c after reviewed helper290. No shared edits. */
#define SOCK_CAMERA_CLAMP_Y(value, candidate, system)                                                                  \
    do {                                                                                                               \
        if ((value) < (candidate)->camera_position.y)                                                                  \
            (value) = (candidate)->camera_position.y;                                                                  \
        else if ((system)->socks[(candidate)->sock_index].mid != NULL && (candidate)->midpoint.y < (value))            \
            (value) = (candidate)->midpoint.y;                                                                         \
    } while (0)
typedef union SOCK_CAMERA_FLOAT_BITS_PC {
    f32 value;
    u32 bits;
} SOCK_CAMERA_FLOAT_BITS_PC;
void __cdecl PerspectMidPoint(NUVEC *, NUVEC *, NUVEC *, NUVEC *);
i32 __cdecl SockSegmentsAhead(SOCK_PC *, i32, i32 *, i32 *);
i32 __cdecl SockSysCamera(SOCKSYS_PC *sock_sys, NUVEC *fallback_camera_position, i32 socket_changed,
                          NUVEC *player_camera_positions, NUVEC *player_positions, i32 player_count,
                          SOCKPOSITION_PC *camera_socket_position, NUVEC *camera_position, NUVEC *camera_target,
                          f32 *overlap_blend, f32 *position_seek, f32 *angle_seek, f32 *camera_shake,
                          f32 *separation_scale) {
    NUVEC average_camera_position, average_player_position, accumulated_target;
    NUVEC candidate_camera, look_position, displacement, local_right, local_x, local_y;
    i32 i;
    volatile f32 magnitude, side, half_width, height_sum, pair_offset;
    volatile f32 working_scale;
    volatile f32 two_player_pullback;
    volatile f32 single_player_pullback;
    volatile f32 camera_height_above_ground;
    i32 contributing_sockets;
    SOCKPOSITION_PC *candidate;
    i32 include;
    SOCK_PC *sock;
    volatile f32 projection_x;
    volatile f32 projection_y;
    volatile f32 lateral_ratio;
    i32 have_limit;
    i32 look_from;
    i32 look_to;
    u16 from_angle;
    i32 difference;
    u16 pitch;
    u16 yaw;
    NUVEC scratch;
    i32 player;
    volatile f32 rounded_player_count;
    SOCK_CAMERA_FLOAT_BITS_PC absolute_difference;

    scratch.x = scratch.y = scratch.z = 0.0f;
    if (sock_sys == NULL) {
        return 0;
    }

    average_camera_position.x = average_camera_position.y = average_camera_position.z = 0.0f;
    average_player_position.x = average_player_position.y = average_player_position.z = 0.0f;
    for (i = 0; i < player_count; ++i) {
        NuVecAdd(&average_camera_position, &average_camera_position, &player_camera_positions[i]);
        NuVecAdd(&average_player_position, &average_player_position, &player_positions[i]);
    }
    rounded_player_count = (f32)player_count;
    working_scale = 1.0 / rounded_player_count;
    NuVecScale(&average_camera_position, &average_camera_position, working_scale);
    NuVecScale(&average_player_position, &average_player_position, working_scale);

    if (player_count == 2 && socket_changed == 0) {
        PerspectMidPoint(camera_target, &player_camera_positions[0], &player_camera_positions[1],
                         fallback_camera_position);
    } else {
        *camera_target = average_camera_position;
    }

    ComplexSockPosition(sock_sys, &average_camera_position, camera_socket_position->sock_index,
                        camera_socket_position->segment, camera_socket_position);
    if (camera_socket_position->sock_index == -1) {
        *camera_position = *fallback_camera_position;
        return 0;
    }

    camera_position->x = camera_position->y = camera_position->z = 0.0f;
    accumulated_target.x = accumulated_target.y = accumulated_target.z = 0.0f;
    if (overlap_blend != NULL) {
        *overlap_blend = 0.0f;
    }
    if (position_seek != NULL) {
        *position_seek = 0.0f;
    }
    if (angle_seek != NULL) {
        *angle_seek = 0.0f;
    }
    if (camera_shake != NULL) {
        *camera_shake = 0.0f;
    }
    if (separation_scale != NULL) {
        *separation_scale = 0.0f;
    }

    two_player_pullback = 0.0f;
    single_player_pullback = 0.0f;
    camera_height_above_ground = 0.0f;
    contributing_sockets = 0;
    for (i = 0; i < TempSPosCount; ++i) {
        candidate = &TempSPosList[i];
        include =
            socket_changed == 0 || candidate->sock_index == camera_socket_position->sock_index ||
            ((sock_sys->socks[camera_socket_position->sock_index].overlap_exclusion_mask[candidate->sock_index / 32] &
              (1u << (candidate->sock_index & 31))) != 0) == 0;
        if (include) {
            sock = &sock_sys->socks[candidate->sock_index];

            if ((sock->flags & SOCK_FLAG_PROJECT_CAMERA_FROM_PLAYER) != 0) {

                local_y.x = 0.0f;
                local_y.y = 1.0f;
                local_y.z = 0.0f;
                local_x.x = 1.0f;
                local_x.y = 0.0f;
                local_x.z = 0.0f;
                NuVecSub(&scratch, &average_camera_position, &candidate->midpoint);
                NuVecRotateX(&local_x, &local_x, candidate->camera_rotation.x);
                NuVecRotateY(&local_x, &local_x, candidate->camera_rotation.y);
                NuVecRotateX(&local_y, &local_y, candidate->camera_rotation.x);
                NuVecRotateY(&local_y, &local_y, candidate->camera_rotation.y);
                projection_x = NuVecDot(&scratch, &local_x);
                projection_y = NuVecDot(&scratch, &local_y);
                projection_x *= sock->camera_local_x_ratio;
                projection_y *= sock->camera_vertical_ratio;
                NuVecScale(&local_x, &local_x, projection_x);
                NuVecScale(&local_y, &local_y, projection_y);
                NuVecAdd(&candidate_camera, &local_x, &local_y);
                NuVecAdd(&candidate_camera, &candidate_camera, &candidate->camera_position);
            } else {
                candidate_camera = candidate->camera_position;
            }

            if (sock->camera_rail_offset != 0.0f) {
                scratch.x = 0.0f;
                scratch.y = 0.0f;
                scratch.z = 1.0f;
                NuVecRotateX(&scratch, &scratch, candidate->camera_rotation.x);
                NuVecRotateY(&scratch, &scratch, candidate->camera_rotation.y);
                working_scale = sock->camera_rail_offset;
                candidate_camera.x = (f64)scratch.x * working_scale + candidate_camera.x;
                candidate_camera.y = (f64)scratch.y * working_scale + candidate_camera.y;
                candidate_camera.z = (f64)scratch.z * working_scale + candidate_camera.z;
            }

            if (sock->lateral != NULL) {

                SockSysPointAlongSpline(&scratch, sock->lateral, candidate->segment, candidate->next_segment,
                                        candidate->ratio);
                local_right.x = 1.0f;
                local_right.y = 0.0f;
                local_right.z = 0.0f;
                NuVecRotateY(&local_right, &local_right, candidate->midpoint_rotation.y);
                displacement.x = scratch.x - candidate->midpoint.x;
                displacement.z = scratch.z - candidate->midpoint.z;
                side = (f64)local_right.x * displacement.x + (f64)local_right.z * displacement.z;
                magnitude = side;

                SockSysPointAlongSpline(&scratch, sock->a, candidate->segment, candidate->next_segment,
                                        candidate->ratio);
                displacement.x = scratch.x - candidate->midpoint.x;
                displacement.z = scratch.z - candidate->midpoint.z;
                half_width = NuFsqrt((f64)displacement.x * displacement.x + (f64)displacement.z * displacement.z);
                SockSysPointAlongSpline(&scratch, sock->b, candidate->segment, candidate->next_segment,
                                        candidate->ratio);
                displacement.x = scratch.x - candidate->midpoint.x;
                displacement.z = scratch.z - candidate->midpoint.z;
                half_width += NuFsqrt((f64)displacement.x * displacement.x + (f64)displacement.z * displacement.z);
                half_width = (f64)half_width * 0.5;
                if (half_width > 0.0f) {
                    lateral_ratio = (f64)magnitude / half_width * working_scale;
                } else {
                    lateral_ratio = 0.0f;
                }
            } else {
                lateral_ratio = sock->camera_lateral_ratio;
            }

            if (lateral_ratio != 0.0f && (sock->flags & SOCK_FLAG_PROJECT_CAMERA_FROM_PLAYER) == 0) {
                displacement.x = ((f64)average_camera_position.x - candidate->midpoint.x) * lateral_ratio;
                displacement.z = ((f64)average_camera_position.z - candidate->midpoint.z) * lateral_ratio;
                if (sock->left != NULL || sock->right != NULL) {
                    magnitude = NuFsqrt((f64)displacement.x * displacement.x + (f64)displacement.z * displacement.z);
                    local_right.x = 1.0f;
                    local_right.y = 0.0f;
                    local_right.z = 0.0f;
                    NuVecRotateY(&local_right, &local_right, candidate->midpoint_rotation.y);
                    side = (f64)local_right.x * displacement.x + (f64)local_right.z * displacement.z;
                    have_limit = 0;
                    if (side < 0.0f) {
                        if (sock->left != NULL) {
                            SockSysPointAlongSpline(&scratch, sock->left, candidate->segment, candidate->next_segment,
                                                    candidate->ratio);
                            have_limit = 1;
                        }
                    } else if (sock->right != NULL) {
                        SockSysPointAlongSpline(&scratch, sock->right, candidate->segment, candidate->next_segment,
                                                candidate->ratio);
                        have_limit = 1;
                    }
                    if (have_limit) {
                        local_x.x = scratch.x - candidate->camera_position.x;
                        local_x.z = scratch.z - candidate->camera_position.z;
                        half_width = NuFsqrt((f64)local_x.x * local_x.x + (f64)local_x.z * local_x.z);
                        if (half_width < magnitude) {
                            working_scale = (f64)half_width / magnitude;
                            displacement.x = (f64)displacement.x * working_scale;
                            displacement.z = (f64)displacement.z * working_scale;
                        }
                    }
                }
                candidate_camera.x += displacement.x;
                candidate_camera.z += displacement.z;
            }

            if (sock->camera_vertical_ratio != 0.0f && (sock->flags & SOCK_FLAG_PROJECT_CAMERA_FROM_PLAYER) == 0) {
                displacement.y =
                    (f64)sock->camera_vertical_ratio * ((f64)average_camera_position.y - candidate->midpoint.y);
                candidate_camera.y += displacement.y;
            }

            if ((sock->camera_arena_blend.x > 0.0f || sock->camera_arena_blend.y > 0.0f ||
                 sock->camera_arena_blend.z > 0.0f) &&
                (sock->camera_arena_offset.x != 0.0f || sock->camera_arena_offset.y != 0.0f ||
                 sock->camera_arena_offset.z != 0.0f)) {
                NuVecAdd(&scratch, &average_player_position, &sock->camera_arena_offset);
                if (1.0f > sock->camera_arena_blend.x)
                    candidate_camera.x =
                        ((f64)scratch.x - candidate_camera.x) * sock->camera_arena_blend.x + candidate_camera.x;
                else
                    candidate_camera.x = scratch.x;
                if (1.0f > sock->camera_arena_blend.y)
                    candidate_camera.y =
                        ((f64)scratch.y - candidate_camera.y) * sock->camera_arena_blend.y + candidate_camera.y;
                else
                    candidate_camera.y = scratch.y;
                if (1.0f > sock->camera_arena_blend.z)
                    candidate_camera.z =
                        ((f64)scratch.z - candidate_camera.z) * sock->camera_arena_blend.z + candidate_camera.z;
                else
                    candidate_camera.z = scratch.z;
                if ((sock->flags & SOCK_FLAG_CLAMP_TARGET_Y) != 0) {
                    SOCK_CAMERA_CLAMP_Y(candidate_camera.y, candidate, sock_sys);
                }
            } else if (sock->camera_arena_offset.y != 0.0f) {
                height_sum = 0.0f;
                for (player = 0; player < player_count; ++player) {
                    height_sum += player_camera_positions[player].y;
                    height_sum += sock->camera_arena_offset.y;
                }
                candidate_camera.y = (f64)height_sum / rounded_player_count;
                if ((sock->flags & SOCK_FLAG_CLAMP_TARGET_Y) != 0) {
                    SOCK_CAMERA_CLAMP_Y(candidate_camera.y, candidate, sock_sys);
                }
            }

            if (sock->camera_distance_to_target != 0.0f) {

                if ((sock->flags & SOCK_FLAG_CAMERA_DISTANCE_XZ) != 0) {
                    scratch.x = camera_target->x - candidate_camera.x;
                    scratch.y = 0.0f;
                    scratch.z = camera_target->z - candidate_camera.z;
                } else {
                    NuVecSub(&scratch, camera_target, &candidate_camera);
                }
                NuVecNorm(&scratch, &scratch);
                candidate_camera.x = (f64)camera_target->x - (f64)scratch.x * sock->camera_distance_to_target;
                candidate_camera.z = (f64)camera_target->z - (f64)scratch.z * sock->camera_distance_to_target;
                if ((sock->flags & SOCK_FLAG_CAMERA_DISTANCE_XZ) == 0) {
                    candidate_camera.y = (f64)camera_target->y - (f64)scratch.y * sock->camera_distance_to_target;
                }
            } else {
                if (sock->camera_pullback_ratio != 0.0f) {
                    candidate_camera.x =
                        ((f64)camera_target->x - candidate_camera.x) * sock->camera_pullback_ratio + candidate_camera.x;
                    candidate_camera.y =
                        ((f64)camera_target->y - candidate_camera.y) * sock->camera_pullback_ratio + candidate_camera.y;
                    candidate_camera.z =
                        ((f64)camera_target->z - candidate_camera.z) * sock->camera_pullback_ratio + candidate_camera.z;
                }
            }

            if (!(sock->look_ratio_xz == 1.0f && sock->look_ratio_y == 1.0f && sock->look_ahead_segments == 0)) {

                if (sock->look != NULL) {
                    SockSysPointAlongSpline(&look_position, sock->look, candidate->segment, candidate->next_segment,
                                            candidate->ratio);
                } else {
                    look_position.x = 0.0f;
                    look_position.y = 0.0f;
                    look_position.z = NuVecDist(&candidate_camera, camera_target, NULL);
                    if (SockSegmentsAhead(sock, candidate->segment, &look_from, &look_to) != 0) {
                        from_angle = sock->cam_rotations[look_from].x;
                        difference =
                            SOCK_ROTATION_DELTA(sock->cam_rotations[look_from].x, sock->cam_rotations[look_to].x);
                        pitch = (u16)(i32)((f64)from_angle + (f64)difference * candidate->ratio);
                        from_angle = sock->cam_rotations[look_from].y;
                        difference =
                            SOCK_ROTATION_DELTA(sock->cam_rotations[look_from].y, sock->cam_rotations[look_to].y);
                        yaw = (u16)(i32)((f64)from_angle + (f64)difference * candidate->ratio);
                        NuVecRotateX(&look_position, &look_position, pitch);
                        NuVecRotateY(&look_position, &look_position, yaw - 0x1555);
                    } else {
                        NuVecRotateX(&look_position, &look_position, candidate->camera_rotation.x);
                        NuVecRotateY(&look_position, &look_position, candidate->camera_rotation.y);
                    }
                    NuVecAdd(&look_position, &look_position, &candidate_camera);
                }
                accumulated_target.x = ((f64)camera_target->x - look_position.x) * sock->look_ratio_xz +
                                       look_position.x + accumulated_target.x;
                accumulated_target.y = ((f64)camera_target->y - look_position.y) * sock->look_ratio_y +
                                       look_position.y + accumulated_target.y;
                accumulated_target.z = ((f64)camera_target->z - look_position.z) * sock->look_ratio_xz +
                                       look_position.z + accumulated_target.z;
            } else {
                NuVecAdd(&accumulated_target, &accumulated_target, camera_target);
            }

            NuVecAdd(camera_position, camera_position, &candidate_camera);
            if (overlap_blend != NULL) {
                *overlap_blend += sock->overlap_blend_ratio;
            }
            if (position_seek != NULL) {
                *position_seek += sock->camera_position_seek;
            }
            if (angle_seek != NULL) {
                *angle_seek += sock->camera_angle_seek;
            }
            if (camera_shake != NULL) {
                *camera_shake += sock->camera_shake;
            }
            two_player_pullback += sock->two_player_pullback;
            single_player_pullback += sock->single_player_pullback;
            camera_height_above_ground += sock->camera_height_above_ground;
            ++contributing_sockets;
        }
    }

    if (contributing_sockets > 0) {
        working_scale = 1.0 / (f64)contributing_sockets;
        NuVecScale(camera_position, camera_position, working_scale);
        if (overlap_blend != NULL) {
            *overlap_blend *= working_scale;
        }
        if (position_seek != NULL) {
            *position_seek *= working_scale;
        }
        if (angle_seek != NULL) {
            *angle_seek *= working_scale;
        }
        if (camera_shake != NULL) {
            *camera_shake *= working_scale;
        }
        two_player_pullback *= working_scale;
        single_player_pullback *= working_scale;
        if (separation_scale != NULL) {
            *separation_scale = two_player_pullback * working_scale;
        }
        NuVecScale(camera_target, &accumulated_target, working_scale);
        camera_height_above_ground *= working_scale;
    }

    if (camera_height_above_ground > 0.0f) {
        height_sum = NewShadow(camera_position, 0.0f, 5.0f, -1);
        if (height_sum != 2000000.0f && camera_position->y < (f64)height_sum + camera_height_above_ground) {
            camera_position->y = (f64)height_sum + camera_height_above_ground;
        }
    }

    if (player_count == 1 && single_player_pullback != 0.0f) {

        working_scale = NuVecDist(camera_target, camera_position, &scratch);
        if (working_scale > 1.0f) {
            NuVecNorm(&scratch, &scratch);
            if ((f64)working_scale - single_player_pullback < 1.0) {
                single_player_pullback = working_scale - 1.0f;
            }
            camera_position->x = (f64)scratch.x * single_player_pullback + camera_position->x;
            camera_position->y = (f64)scratch.y * single_player_pullback + camera_position->y;
            camera_position->z = (f64)scratch.z * single_player_pullback + camera_position->z;
        }
    }
    if (player_count == 2 && two_player_pullback != 0.0f) {
        // The reference has distinct spatial and planar pullback blocks.
#define SOCK_CAMERA_PAIR_PULLBACK(distance_service, with_y)                                                            \
    do {                                                                                                               \
        if ((sock_sys->socks[camera_socket_position->sock_index].flags & SOCK_FLAG_TWO_PLAYER_VERTICAL_SEPARATION) !=  \
            0) {                                                                                                       \
            absolute_difference.value = (f64)player_camera_positions[0].y - player_camera_positions[1].y;              \
            absolute_difference.bits &= 0x7fffffffu;                                                                   \
            pair_offset = absolute_difference.value;                                                                   \
        } else                                                                                                         \
            pair_offset = distance_service(&player_camera_positions[0], &player_camera_positions[1], NULL);            \
        pair_offset *= -two_player_pullback;                                                                           \
        working_scale = distance_service(camera_target, camera_position, &scratch);                                    \
        NuVecNorm(&scratch, &scratch);                                                                                 \
        if ((f64)working_scale - pair_offset < 1.0)                                                                    \
            pair_offset = working_scale - 1.0f;                                                                        \
        camera_position->x = (f64)scratch.x * pair_offset + camera_position->x;                                        \
        if (with_y)                                                                                                    \
            camera_position->y = (f64)scratch.y * pair_offset + camera_position->y;                                    \
        camera_position->z = (f64)scratch.z * pair_offset + camera_position->z;                                        \
    } while (0)
        if (camera_socket_position->sock_index != -1 &&
            (sock_sys->socks[camera_socket_position->sock_index].flags & SOCK_FLAG_TWO_PLAYER_PLANAR_PULLBACK) != 0) {
            SOCK_CAMERA_PAIR_PULLBACK(NuVecXZDist, 0);
        } else {
            SOCK_CAMERA_PAIR_PULLBACK(NuVecDist, 1);
        }
#undef SOCK_CAMERA_PAIR_PULLBACK
    }
    return 1;
}

#undef SOCK_CAMERA_CLAMP_Y
