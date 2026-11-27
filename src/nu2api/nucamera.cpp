#include "gameapi/edtools/edui.h"
#include "legoapi/cutscenes/gcutscn.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/fx/game_deb.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nuvport.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/nustencilshadows_PC.h"
#include "externals.h"

#include <string.h>
#include <math.h>
#include <float.h>

PC_GLOBAL_LIGHTING_STATE PCGlobalLightingState = {0};
i32 NuCameraMode = 0;
i32 cam_state_count = 0;
i32 NuCameraClipScissorToViewport = 0;
i32 NuCameraPreviousClipScissorToViewport = 0;
i32 NuCameraOrthographic = 0;
static i32 force_camera_farclip = 0;
NUVEC cam_axes = {1.0f, 1.0f, 1.0f};
NUMTX vmtx = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
NUMTX pmtx = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
NUMTX smtx = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
NUMTX vpc_sci_mtx = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
NUMTX vpc_vport_mtx = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
NUMTX pc_vport_mtx = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
NUMTX vpsmtx = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
NUMTX psmtx = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
NUMTX clip_planes = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
f32 NuCameraProjectionFarClipBias = 0.1f;
__declspec(align(16)) NUMTX NuCameraScissorProjectionScratch = {0};
static f32 NuCameraOrthoScale = 0.1f;
i32 NuCameraScissorWidth = 0x1000;
i32 NuCameraScissorHeight = 0x1000;

NUCAMERA *NuCameraCreate(void) {
    NUCAMERA *camera = (NUCAMERA *)NuAlloc(sizeof(NUCAMERA), "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nucamera_gen.c", 0x11e);

    NuMtxSetIdentity(&camera->mtx);
    camera->fov = 0.75f;
    camera->near_clip = 0.15f;
    camera->portal_nearclip = 0.0f;
    camera->far_clip = 10000.0f;
    camera->aspect = 0.75f;
    camera->projection_scale_x = 1.0f;
    camera->projection_scale_y = 1.0f;
    camera->scale.z = 1.0f;
    camera->scale.y = 1.0f;
    camera->scale.x = 1.0f;
    camera->projection_offset_x = 0.0f;
    camera->projection_offset_y = 0.0f;
    camera->far_clip_override = 0.0f;
    camera->near_clip_override = 0.0f;
    return camera;
}

void NuCameraDestroy(NUCAMERA *camera) {
    if (camera != NULL) {
        NuFree(camera, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\nucamera_gen.c", 0x13e);
    }
}

/* Builds the orthographic projection used when NuCameraOrthographic is set. */
static void NuCameraSetOrthoMtx(NUMTX *mtx, f32 aspect) {
    memset(mtx, 0, sizeof(*mtx));
    mtx->m00 = NuCameraOrthoScale * aspect;
    mtx->m11 = NuCameraOrthoScale;
    mtx->m22 = 1.0f;
    mtx->m33 = 1.0f;
}

extern "C" void NuCameraForceFarclip(i32 enabled) {
    force_camera_farclip = enabled;
}

void NuCameraGet(NUCAMERA *camera) {
    *camera = global_camera;
}

/* Applies the camera with a temporary reflection state, handing back the state NuCameraSet leaves. */
extern "C" void NuCameraSetReflect(NUCAMERA *camera, NUCAMERAREFLECT *reflect) {
    global_reflect = *reflect;
    NuCameraSet(camera);
    *reflect = global_reflect;
}

NUMTX *NuCameraGetMtx(void) {
    return &global_camera.mtx;
}

void NuCameraSetAxes(NUVEC *axes) {
    cam_axes = *axes;
}

void NuCameraGetAxes(NUVEC *axes) {
    *axes = cam_axes;
}

NUCAMERA *NuCameraGetCam(void) {
    return &global_camera;
}

NUMTX *NuCameraGetViewMtx(void) {
    return &vmtx;
}

NUMTX *NuCameraGetProjectionMtx(void) {
    return &pmtx;
}

NUMTX *NuCameraGetClippingMtx(void) {
    return &cmtx;
}

NUMTX *NuCameraGetScalingMtx(void) {
    return &smtx;
}

NUMTX *NuCameraGetVPCSMtx(void) {
    return &vpsmtx;
}

NUMTX *NuCameraGetVPCMtx(void) {
    return &vpc_vport_mtx;
}

NUMTX *NuCameraGetVPMtx(void) {
    return &vpmtx;
}

NUMTX *NuCameraGetPCMtx(void) {
    return &pc_vport_mtx;
}

void NuCameraGetClipMtx(NUMTX *viewport, NUMTX *scissor) {
    if (viewport != NULL) {
        *viewport = vpc_vport_mtx;
    }
    if (scissor != NULL) {
        *scissor = vpc_sci_mtx;
    }
}

NUMTX *NuCameraGetClipPlanes(void) {
    return &clip_planes;
}

extern "C" void NuCameraRayCast(NUVEC *point, f32 x, f32 y) {
    point->x = x * zx;
    point->y = y * zy;
    point->z = 1.0f;
    NuVecMtxRotate(point, point, &global_camera.mtx);
}

void NuMtxCalcCheapFaceOn(NUMTX *m, NUVEC *position) {
    NUMTX *view = NuCameraGetViewMtx();

    m->m00 = -view->m00;
    m->m10 = view->m01;
    m->m20 = -view->m02;
    m->m01 = -view->m10;
    m->m11 = view->m11;
    m->m21 = -view->m12;
    m->m02 = -view->m20;
    m->m12 = view->m21;
    m->m22 = -view->m22;
    m->m03 = m->m13 = m->m23 = 0.0f;
    m->m33 = 1.0f;
    m->m30 = position->x;
    m->m31 = position->y;
    m->m32 = position->z;
}

extern "C" void NuMtxCalcCheapFaceY(NUMTX *m, NUVEC *position) {
    NUMTX *view = NuCameraGetViewMtx();
    NUVEC right;
    NUVEC forward;

    right.x = -view->m00;
    right.y = 0.0f;
    right.z = -view->m20;
    NuVecNorm(&right, &right);
    m->m00 = right.x;
    m->m01 = right.y;
    m->m02 = right.z;
    m->m10 = 0.0f;
    m->m11 = 1.0f;
    m->m12 = 0.0f;
    forward.x = -view->m02;
    forward.y = 0.0f;
    forward.z = -view->m22;
    NuVecNorm(&forward, &forward);
    m->m20 = forward.x;
    m->m21 = forward.y;
    m->m22 = forward.z;
    m->m23 = 0.0f;
    m->m13 = 0.0f;
    m->m03 = 0.0f;
    m->m33 = 1.0f;
    m->m30 = position->x;
    m->m31 = position->y;
    m->m32 = position->z;
}

void NuMtxCalcDebrisFaceOn(NUMTX *m) {
    NUMTX *view = NuCameraGetViewMtx();

    m->m00 = -view->m00;
    m->m10 = view->m01;
    m->m20 = -view->m02;
    m->m01 = -view->m10;
    m->m11 = view->m11;
    m->m21 = -view->m12;
    m->m02 = -view->m20;
    m->m12 = view->m21;
    m->m22 = -view->m22;
    m->m03 = m->m13 = m->m23 = 0.0f;
    m->m33 = 1.0f;
}

/* Reconstructed storage qualifiers express the original F32 checkpoints;
   they do not assert historical declarations. */
void NuCameraSetVPortClipMtx(NUMTX *out, NUMTX *view, NUCAMERA *camera, i32 fast) {
    if (fast == 0) {
        f32 far_clip;
        f32 near_clip;
        f32 half_fov;
        f32 cotangent;
        f32 horizontal;
        i32 angle;
        if (camera->far_clip_override == 0.0f) {
            far_clip = camera->far_clip - NuCameraProjectionFarClipBias;
        } else {
            far_clip = camera->far_clip_override - NuCameraProjectionFarClipBias;
        }
        if (camera->near_clip_override == 0.0f) {
            near_clip = camera->near_clip;
        } else {
            near_clip = camera->near_clip_override;
        }
        half_fov = (f32)((f64)camera->fov * 0.5);
        angle = (i32)((f64)half_fov * AIPathAngularScale);
        cotangent = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] / NuTrigTable[(angle >> 1) & 0x7fff];
        horizontal = (camera->aspect * cotangent) * vpCurrent.projection_scale_x;
        memset(&pc_vport_mtx, 0, sizeof(pc_vport_mtx));
        pc_vport_mtx.m00 = horizontal;
        pc_vport_mtx.m11 = vpCurrent.projection_scale_y * cotangent;
        pc_vport_mtx.m22 = (far_clip + near_clip) / (far_clip - near_clip);
        pc_vport_mtx.m23 = 1.0f;
        pc_vport_mtx.m32 = ((-2.0f * far_clip) * near_clip) / (far_clip - near_clip);
        pc_vport_mtx.m33 = 0.0f;
    }
    NuMtxMulVU0(out, view, &pc_vport_mtx);
}

void NuCameraSetScissorClipMtx(NUMTX *out, NUMTX *view, NUCAMERA *camera, i32 fast) {
    if (fast == 0) {
        f32 far_clip;
        f32 near_clip;
        f32 half_fov;
        f32 cotangent;
        f32 horizontal;
        i32 angle;
        if (camera->far_clip_override == 0.0f) {
            far_clip = camera->far_clip - NuCameraProjectionFarClipBias;
        } else {
            far_clip = camera->far_clip_override - NuCameraProjectionFarClipBias;
        }
        if (camera->near_clip_override == 0.0f) {
            near_clip = camera->near_clip;
        } else {
            near_clip = camera->near_clip_override;
        }
        half_fov = (f32)((f64)camera->fov * 0.5);
        angle = (i32)((f64)half_fov * AIPathAngularScale);
        cotangent = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] / NuTrigTable[(angle >> 1) & 0x7fff];
        horizontal = (camera->aspect * vpCurrent.scissor_scale_x) * cotangent;
        memset(&NuCameraScissorProjectionScratch, 0, sizeof(NuCameraScissorProjectionScratch));
        NuCameraScissorProjectionScratch.m00 = horizontal;
        NuCameraScissorProjectionScratch.m11 = vpCurrent.scissor_scale_y * cotangent;
        NuCameraScissorProjectionScratch.m22 = (far_clip + near_clip) / (far_clip - near_clip);
        NuCameraScissorProjectionScratch.m23 = 1.0f;
        NuCameraScissorProjectionScratch.m32 = ((-2.0f * far_clip) * near_clip) / (far_clip - near_clip);
    }
    NuMtxMulVU0(out, view, &NuCameraScissorProjectionScratch);
}

i32 NuCameraClipTestSphere(NUVEC *point, f32 radius, NUMTX *world) {
    NUVEC transformed;
    NUVEC4 plane_distances;
    if (world != NULL) {
        NuVecMtxTransform(&transformed, point, world);
        NuVecMtxTransform(&transformed, &transformed, &vmtx);
    } else {
        NuVecMtxTransform(&transformed, point, &vmtx);
    }
    if (0.0f > (transformed.z - global_camera.near_clip) + radius) {
        goto rejected;
    }
    if (0.0f > (global_camera.far_clip - transformed.z) + radius) {
        goto rejected;
    }
    NuVec4MtxTransform(&plane_distances, &transformed, &clip_planes);
    if (plane_distances.x < -radius) {
        goto rejected;
    }
    if (-radius > plane_distances.y) {
        goto rejected;
    }
    if (-radius > plane_distances.z) {
        goto rejected;
    }
    if (-radius > plane_distances.w) {
        goto rejected;
    }
    return 0;
rejected:
    return 1;
}

void NuCameraSetEx(NUCAMERA *camera, i32 fast) {
    global_camera = *camera;
    FaceYDirStream(NuAtan2D(-global_camera.mtx.m20, -global_camera.mtx.m22));
    if (fast == 0) {
        NuVpUpdate();
    }
    if (NuCameraMode == 0) {
        NuMtxInvVU0(&vmtx, &global_camera.mtx);
        NuMtxScaleVU0(&vmtx, &global_camera.scale);
    } else {
        NUMTX mirror;
        NUMTX camera_inverse;
        NUMTX reflection_inverse;
        mirror = ANI3_EVAL_IDENTITY_MATRIX;
        mirror.m11 = -1.0f;
        NuMtxInv(&camera_inverse, &global_camera.mtx);
        NuMtxInv(&reflection_inverse, &global_reflect.mtx);
        vmtx = reflection_inverse;
        if (NuCameraMode == 1) {
            NuMtxMul(&vmtx, &vmtx, &mirror);
        }
        NuMtxMul(&vmtx, &vmtx, &global_reflect.mtx);
        NuMtxMul(&vmtx, &vmtx, &camera_inverse);
    }
    if (fast == 0) {
        if (NuCameraOrthographic == 0) {
            NuCameraSetProjectionMtx(&pmtx, global_camera.fov, global_camera.aspect, global_camera.near_clip, global_camera.far_clip);
            pmtx.m00 *= global_camera.projection_scale_x;
            pmtx.m11 *= global_camera.projection_scale_y;
            pmtx.m20 += global_camera.projection_offset_x;
            pmtx.m21 += global_camera.projection_offset_y;
        } else {
            NuCameraSetOrthoMtx(&pmtx, global_camera.aspect);
        }
    }
    NuCameraSetVPortClipMtx(&vpc_vport_mtx, &vmtx, &global_camera, fast);
    if (NuCameraClipScissorToViewport != NuCameraPreviousClipScissorToViewport) {
        fast = 0;
        NuCameraPreviousClipScissorToViewport = NuCameraClipScissorToViewport;
    }
    if (NuCameraClipScissorToViewport != 0) {
        NuCameraSetVPortClipMtx(&vpc_sci_mtx, &vmtx, &global_camera, fast);
    } else {
        NuCameraSetScissorClipMtx(&vpc_sci_mtx, &vmtx, &global_camera, fast);
    }
    smtx = NuCameraScalingTemplate;
    NuMtxMulH(&vpmtx, &vmtx, &pmtx);
    NuMtxMulH(&vpsmtx, &vpmtx, &smtx);
    NuMtxMulH(&psmtx, &pmtx, &smtx);
    if (fast == 0) {
        f32 half_fov = (f32)((f64)global_camera.fov * 0.5);
        i32 angle = (i32)((f64)half_fov * AIPathAngularScale);
        f32 side;
        f32 squared;
        f32 reciprocal;
        zy = NuTrigTable[(angle >> 1) & 0x7fff] / NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
        zx = zy / global_camera.aspect;
        zxs = zx * NuCameraScissorWidth / PS2_REZ_W;
        zys = zy * NuCameraScissorHeight / PS2_REZ_H;
        clip_planes.m02 = 0.0f;
        clip_planes.m12 = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
        clip_planes.m22 = NuTrigTable[(angle >> 1) & 0x7fff];
        clip_planes.m32 = 0.0f;
        clip_planes.m03 = 0.0f;
        clip_planes.m13 = -clip_planes.m12;
        clip_planes.m23 = clip_planes.m22;
        clip_planes.m33 = 0.0f;
        side = (clip_planes.m22 / clip_planes.m12) / camera->aspect;
        squared = (f32)((f64)side * side + 1.0);
        reciprocal = 1.0f / NuFsqrt(squared);
        clip_planes.m00 = -reciprocal;
        clip_planes.m10 = 0.0f;
        clip_planes.m20 = reciprocal * side;
        clip_planes.m30 = 0.0f;
        clip_planes.m11 = 0.0f;
        clip_planes.m01 = reciprocal;
        clip_planes.m21 = clip_planes.m20;
        clip_planes.m31 = 0.0f;
        NuCameraBuildClipPlanes();
    }
    NuRndrSetViewMtx(&vpsmtx, &vpc_vport_mtx, &vpc_sci_mtx);
    PCGlobalLightingState.specular_direction_missing = 1;
    PCGlobalLightingState.specular_intensity_present = 0;
    NuRndrSetSpecularLightPS(NULL, NULL);
    NuRndrStateUpdateCameraState();
    if (g_OcclusionManager.initialized != 0 && g_OcclusionManager.enabled != 0) {
        g_OcclusionManager.current_set->queries_prepared = 0;
    }
}

i32 NuCameraClipTestExtentsAxisAligned(NUVEC *center, NUVEC *extent, f32 far_clip) {
    struct __declspec(align(16)) BoundProjection {
        NUVEC sides;
        f32 fourth;
        f32 far_plane;
        f32 near_plane;
    } distance;
    struct BoundProjection radius;

    f32 saved_far = AxisClipNearFarPlanes.m30;
    if (far_clip != 0.0f) {
        AxisClipNearFarPlanes.m30 -= global_camera.far_clip;
        AxisClipNearFarPlanes.m30 += far_clip;
    }
    NuVecMtxTransform(&distance.sides, center, &AxisClipFrustumPlanes);
    distance.fourth = AxisClipFrustumPlanes.m33 + (center->z * AxisClipFrustumPlanes.m23 +
                                                   (center->x * AxisClipFrustumPlanes.m03 + center->y * AxisClipFrustumPlanes.m13));
    distance.far_plane = AxisClipNearFarPlanes.m30 + (center->z * AxisClipNearFarPlanes.m20 +
                                                      (center->x * AxisClipNearFarPlanes.m00 + center->y * AxisClipNearFarPlanes.m10));
    distance.near_plane = AxisClipNearFarPlanes.m31 + (center->z * AxisClipNearFarPlanes.m21 +
                                                       (center->x * AxisClipNearFarPlanes.m01 + center->y * AxisClipNearFarPlanes.m11));
    NuVecMtxTransform(&radius.sides, extent, &AxisClipAbsFrustumPlanes);
    radius.fourth =
        extent->x * AxisClipAbsFrustumPlanes.m03 + extent->y * AxisClipAbsFrustumPlanes.m13 + extent->z * AxisClipAbsFrustumPlanes.m23;
    radius.far_plane =
        extent->x * AxisClipNearFarPlanes.m02 + extent->y * AxisClipNearFarPlanes.m12 + extent->z * AxisClipNearFarPlanes.m22;
    radius.near_plane =
        extent->x * AxisClipNearFarPlanes.m03 + extent->y * AxisClipNearFarPlanes.m13 + extent->z * AxisClipNearFarPlanes.m23;
    AxisClipNearFarPlanes.m30 = saved_far;

    if (distance.sides.x < -radius.sides.x || distance.sides.y < -radius.sides.y || distance.sides.z < -radius.sides.z ||
        distance.fourth < -radius.fourth || distance.far_plane < -radius.far_plane || distance.near_plane < -radius.near_plane) {
        return 0;
    }
    if (distance.sides.x < radius.sides.x || distance.sides.y < radius.sides.y || distance.sides.z < radius.sides.z ||
        distance.fourth < radius.fourth) {
        NuVecMtxTransform(&distance.sides, center, &AxisClipScissorPlanes);
        distance.fourth = AxisClipScissorPlanes.m33 + (center->z * AxisClipScissorPlanes.m23 +
                                                       (center->x * AxisClipScissorPlanes.m03 + center->y * AxisClipScissorPlanes.m13));
        NuVecMtxTransform(&radius.sides, extent, &AxisClipAbsScissorPlanes);
        radius.fourth =
            AxisClipAbsScissorPlanes.m33 + (extent->z * AxisClipAbsScissorPlanes.m23 +
                                            (extent->y * AxisClipAbsScissorPlanes.m13 + extent->x * AxisClipAbsScissorPlanes.m03));
        if (distance.sides.x < radius.sides.x || distance.sides.y < radius.sides.y || distance.sides.z < radius.sides.z ||
            distance.fourth < radius.fourth) {
            return 2;
        }
    }
    return 1;
}

void NuMtxCalcCheapFaceY_v2(NUMTX *matrix, NUVEC *direction) {
    NUVEC *first = &matrix->rows[0].xyz;
    NUVEC *second = &matrix->rows[1].xyz;
    NUVEC *third = &matrix->rows[2].xyz;
    NuVecCross(first, &global_camera.mtx.rows[2].xyz, direction);
    NuVecNorm(first, first);
    *second = *direction;
    NuVecCross(third, first, second);
    matrix->m03 = matrix->m13 = matrix->m23 = 0.0f;
    matrix->m30 = matrix->m31 = matrix->m32 = 0.0f;
    matrix->m33 = 1.0f;
}

f32 NuCameraFocalLenToFOV(f32 focal_length) {
    f32 ratio = (f32)(18.0 / focal_length);
    f32 result = (f32)(NuAtan2DAF(ratio, 1.0f) * PCAtan2RadianScale * 2.0);
    return result;
}

extern "C" f32 NuCameraFOVToFocalLen(f32 fov) {
    f32 half_fov = (f32)(fov * 0.5);

    return (f32)(18.0 /
                 (NU_SIN_LUT(half_fov * 10430.3779296875) / NuTrigTable[(0x4000 - (i32)(half_fov * -10430.3779296875)) >> 1 & 0x7fff]));
}

extern "C" f32 NuCameraCalcAperture(f32 focal_length, f32 root_fstop) {
    f32 aperture;

    root_fstop = root_fstop * root_fstop;
    if (focal_length == 0.0f || root_fstop == 0.0f) {
        aperture = 0.0f;
    } else {
        aperture = focal_length / root_fstop;
    }
    return aperture;
}

extern "C" f32 NuCameraCalcRootFStop(f32 focal_length, f32 aperture) {
    f32 ratio;

    if (focal_length == 0.0f || aperture == 0.0f) {
        ratio = 0.0f;
    } else {
        ratio = focal_length / aperture;
    }
    return NuFsqrt(ratio);
}

i32 NuCameraClipTestPoints(NUVEC *points, i32 count, NUMTX *world_matrix) {
    NUMTX matrix;
    i32 common;
    i32 i;
    if (world_matrix != NULL) {
        NuMtxMulH(&matrix, world_matrix, &vmtx);
    } else {
        matrix = vmtx;
    }
    common = -1;
    for (i = 0; i < count; ++i) {
        NUVEC t;
        i32 flags;
        f32 negative_z;
        NuVecMtxTransform(&t, &points[i], &matrix);
        flags = 0;
        if (t.z < global_camera.near_clip) {
            flags |= NUCAMERA_CLIP_NEAR;
        }
        if (t.z > global_camera.far_clip) {
            flags |= NUCAMERA_CLIP_FAR;
        }
        negative_z = -t.z;
        if (t.x < negative_z * zx) {
            flags |= NUCAMERA_CLIP_LEFT;
        }
        if (t.z * zx < t.x) {
            flags |= NUCAMERA_CLIP_RIGHT;
        }
        if (t.y < negative_z * zy) {
            flags |= NUCAMERA_CLIP_BOTTOM;
        }
        if (t.y > t.z * zy) {
            flags |= NUCAMERA_CLIP_TOP;
        }
        common &= flags;
    }
    return common;
}

void NuMtxCalcFaceOn(NUMTX *m, NUVEC *v) {
    NUVEC world_up = {0.0f, 1.0f, 0.0f};
    NUVEC right;
    NUVEC up;
    NUVEC forward;
    NUVEC camera_position;
    f32 projection;

    NuMtxGetTranslation(NuCameraGetMtx(), &camera_position);
    NuVecSub(&forward, &camera_position, v);
    NuVecNorm(&forward, &forward);

    projection = NuVecDot(&world_up, &forward);
    up.x = world_up.x - forward.x * projection;
    up.y = world_up.y - forward.y * projection;
    up.z = world_up.z - forward.z * projection;
    NuVecNorm(&up, &up);
    NuVecCross(&right, &up, &forward);

    m->m00 = right.x;
    m->m10 = up.x;
    m->m20 = forward.x;
    m->m01 = right.y;
    m->m11 = up.y;
    m->m21 = forward.y;
    m->m02 = right.z;
    m->m12 = up.z;
    m->m22 = forward.z;
    m->m23 = 0.0f;
    m->m13 = 0.0f;
    m->m03 = 0.0f;
    m->m33 = 1.0f;
    m->m30 = v->x;
    m->m31 = v->y;
    m->m32 = v->z;
}

void NuMtxCalcFaceY(NUMTX *m, NUVEC *v) {
    NUVEC world_up = {0.0f, 1.0f, 0.0f};
    NUVEC right;
    NUVEC up;
    NUVEC forward;
    NUVEC camera_position;

    NuMtxGetTranslation(NuCameraGetMtx(), &camera_position);
    NuVecSub(&forward, &camera_position, v);
    NuVecCross(&right, &world_up, &forward);
    NuVecNorm(&right, &right);
    NuVecCross(&up, &world_up, &right);
    NuVecNorm(&up, &up);

    m->m00 = right.x;
    m->m10 = world_up.x;
    m->m20 = up.x;
    m->m01 = right.y;
    m->m11 = world_up.y;
    m->m21 = up.y;
    m->m02 = right.z;
    m->m12 = world_up.z;
    m->m22 = up.z;
    m->m23 = 0.0f;
    m->m13 = 0.0f;
    m->m03 = 0.0f;
    m->m33 = 1.0f;
    m->m30 = v->x;
    m->m31 = v->y;
    m->m32 = v->z;
}

i32 NuCameraClipTestExtentsGeneric(NUVEC *min, NUVEC *max, NUMTX *world_mtx, f32 far_clip, i32 flags, f32 *min_depth) {
    NUMTX transform;
    NUVEC corners[8];
    NUVEC view[8];
    char results[8];
    i32 i;
    if (far_clip == 0.0f) {
        far_clip = global_camera.far_clip;
    }
    if (world_mtx == NULL || world_mtx == &NuMtxSetIdentityTemplate) {
        transform = vmtx;
    } else {
        NuMtxMul(&transform, world_mtx, &vmtx);
    }
    corners[0].x = min->x;
    corners[0].y = min->y;
    corners[0].z = min->z;
    corners[1].x = max->x;
    corners[1].y = max->y;
    corners[1].z = max->z;
    corners[2].x = min->x;
    corners[2].y = min->y;
    corners[2].z = max->z;
    corners[3].x = max->x;
    corners[3].y = max->y;
    corners[3].z = min->z;
    corners[4].x = min->x;
    corners[4].y = max->y;
    corners[4].z = min->z;
    corners[5].x = min->x;
    corners[5].y = max->y;
    corners[5].z = max->z;
    corners[6].x = max->x;
    corners[6].y = min->y;
    corners[6].z = min->z;
    corners[7].x = max->x;
    corners[7].y = min->y;
    corners[7].z = max->z;
    NuVecMtxTransform(&view[0], &corners[0], &transform);
    NuVecMtxTransform(&view[1], &corners[1], &transform);
    NuVecMtxTransform(&view[2], &corners[2], &transform);
    NuVecMtxTransform(&view[3], &corners[3], &transform);
    NuVecMtxTransform(&view[4], &corners[4], &transform);
    NuVecMtxTransform(&view[5], &corners[5], &transform);
    NuVecMtxTransform(&view[6], &corners[6], &transform);
    NuVecMtxTransform(&view[7], &corners[7], &transform);
    *min_depth = FLT_MAX;
    memset(results, 0, sizeof(results));
    for (i = 0; i < 8; ++i) {
        if (view[i].z < *min_depth) {
            *min_depth = view[i].z;
        }
        NUCAMERA_CLIP_CODE_EX(results[i], view[i], (flags & 2) == 0, far_clip, zx, zy);
    }
    if ((results[0] & results[1] & results[2] & results[3] & results[4] & results[5] & results[6] & results[7]) != 0) {
        return 0;
    }
    if ((results[0] | results[1] | results[2] | results[3] | results[4] | results[5] | results[6] | results[7]) == 0) {
        return 1;
    }
    if ((flags & 1) != 0) {
        return 2;
    }
    memset(results, 0, sizeof(results));
    for (i = 0; i < 8; i += 4) {
        NUCAMERA_CLIP_CODE_EX(results[i], view[i], (flags & 2) == 0, far_clip, VISIBILITY_ALTERNATE_WIDTH, zys);
        NUCAMERA_CLIP_CODE_EX(results[i + 1], view[i + 1], (flags & 2) == 0, far_clip, VISIBILITY_ALTERNATE_WIDTH, zys);
        NUCAMERA_CLIP_CODE_EX(results[i + 2], view[i + 2], (flags & 2) == 0, far_clip, VISIBILITY_ALTERNATE_WIDTH, zys);
        NUCAMERA_CLIP_CODE_EX(results[i + 3], view[i + 3], (flags & 2) == 0, far_clip, VISIBILITY_ALTERNATE_WIDTH, zys);
    }
    if ((results[0] & results[1] & results[2] & results[3] & results[4] & results[5] & results[6] & results[7]) != 0 ||
        (results[0] | results[1] | results[2] | results[3] | results[4] | results[5] | results[6] | results[7]) == 0) {
        return 1;
    }
    return 2;
}

void NuCameraCalcClipMtx(CAMERA_CLIP_PACKET *clip, NUCAMERA *camera, i32 use_cached_projection) {
    static NUMTX clip_inverse = {0};
    if (use_cached_projection == 0 || (clip->flags & 1) == 0) {
        i32 angle;
        f32 half_fov;
        f32 cotangent;
        f32 x;
        f32 y;
        f32 far_clip = clip->far_clip;
        f32 near_clip = clip->near_clip;
        clip->flags |= 1;
        half_fov = (f32)(camera->fov * 0.5);
        angle = (i32)(half_fov * AIPathAngularScale);
        cotangent = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] / NuTrigTable[(angle >> 1) & 0x7fff];
        x = clip->x_scale * camera->aspect * cotangent;
        y = cotangent * clip->y_scale;
        memset(&clip->projection, 0, sizeof(clip->projection));
        clip->projection.m00 = x;
        clip->projection.m11 = y;
        clip->projection.m22 = (f32)(2.0 / ((f64)far_clip - near_clip));
        clip->projection.m23 = 1.0f;
        clip->projection.m32 = (f32)(-2.0 * near_clip / ((f64)far_clip - near_clip));
    }
    NuMtxInvVU0(&clip_inverse, &camera->mtx);
    NuMtxMulVU0(&clip->clip, &clip_inverse, &clip->projection);
}

void NuCameraTransformView(NUVEC *view, NUVEC *world, i32 count, NUMTX *matrix) {
    NUMTX transform;
    NUVEC *end = world + count;
    if (matrix != NULL) {
        NuMtxMul(&transform, matrix, &vmtx);
    } else {
        transform = vmtx;
    }
    while (world < end) {
        NuVecMtxTransform(view++, world++, &transform);
    }
}

void NuCameraTransformScreenClip(NUVEC *screen, NUVEC *world, i32 count, NUMTX *matrix) {
    NUMTX transform;
    NUVEC *end = world + count;
    if (matrix != NULL) {
        NuMtxMulH(&transform, matrix, &vpc_vport_mtx);
    } else {
        transform = vpc_vport_mtx;
    }
    while (world < end) {
        NuVecMtxTransformH(screen++, world++, &transform);
    }
}

void NuCameraTransformScissorClip(NUVEC *screen, NUVEC *world, i32 count, NUMTX *matrix) {
    NUMTX transform;
    NUVEC *end = world + count;
    if (matrix != NULL) {
        NuMtxMulH(&transform, matrix, &vpc_sci_mtx);
    } else {
        transform = vpc_sci_mtx;
    }
    while (world < end) {
        NuVecMtxTransformH(screen++, world++, &transform);
    }
}

void NuCameraTransformScreen(NUVEC *screen, NUVEC *world, i32 count, NUMTX *matrix) {
    NUMTX transform;
    NUVEC *end = world + count;
    if (matrix != NULL) {
        NuMtxMulH(&transform, matrix, &vpsmtx);
    } else {
        transform = vpsmtx;
    }
    while (world < end) {
        NuVecMtxTransformH(screen++, world++, &transform);
    }
}

extern "C" void NuCameraTransformScreenVU0(NUVEC4 *screen, NUVEC4 *world, i32 count, NUMTX *matrix) {
    NUMTX transform;
    NUVEC4 *end = world + count;
    if (matrix != NULL) {
        NuMtxMulVU0(&transform, matrix, &vpsmtx);
    } else {
        transform = vpsmtx;
    }
    while (world < end) {
        NuVec4MtxTransformHVU0(screen++, world++, &transform);
    }
}

i32 NuCameraSaveState(void) {
    i32 index = -1;
    if (cam_state_count < 16) {
        CAMERA_SAVED_STATE *state;
        index = cam_state_count++;
        state = &CameraSavedStates[index];
        state->zx = zx;
        state->camera = global_camera;
        state->zy = zy;
        state->zxs = VISIBILITY_ALTERNATE_WIDTH;
        state->zys = zys;
        state->view = vmtx;
        state->projection = pmtx;
        state->screen = smtx;
        state->view_projection = vpmtx;
        state->view_projection_scissor = vpc_sci_mtx;
        state->view_projection_viewport = vpc_vport_mtx;
        state->viewport = pc_vport_mtx;
        state->view_projection_screen = vpsmtx;
        state->projection_screen = psmtx;
        state->effects = NuCameraMode;
        state->planes = CameraClipPlanes;
    }
    return index + 1;
}

void NuCameraClearStateBuffer(void) {
    cam_state_count = 0;
}

extern "C" i32 NuCameraClipTestPointVport(NUVEC *point) {
    return NuVecClipTestPointVU0(point, &vpc_vport_mtx) == 0;
}

extern "C" i32 NuCameraClipTestPointScissor(NUVEC *point) {
    return NuVecClipTestPointVU0(point, &vpc_sci_mtx) == 0;
}

f32 NuCameraDistSqr(NUVEC *point) {
    return NuVecDistSqr(point, &global_camera.mtx.translation, NULL);
}

extern "C" f32 NuCameraDist(NUVEC *point) {
    return NuVecDist(point, &global_camera.mtx.translation, NULL);
}

extern "C" void NuCameraGetTrans(NUVEC *trans) {
    *trans = global_camera.mtx.translation;
}

extern "C" void NuCameraGetPosition(NUVEC *position) {
    *position = global_camera.mtx.translation;
}

extern "C" void NuCameraGetClippingRatios(f32 *horizontal, f32 *vertical) {
    *horizontal = zx;
    *vertical = zy;
}

void NuCameraCalcRay(f32 screen_x, f32 screen_y, NUVEC *ray_start, NUVEC *ray_end, NUCAMERA *camera) {
    NUVEC near_point;
    NUVEC far_point;
    f32 half_fov;
    f32 tangent;
    f32 x;
    f32 y;
    i32 angle;

    if (camera == NULL) {
        camera = &global_camera;
    }

    half_fov = camera->fov * 0.5;
    angle = (i32)(half_fov * AIPathAngularScale);
    tangent = NuTrigTable[(angle >> 1) & 0x7fff] / NuTrigTable[((i32)((u32)angle + 0x4000u) >> 1) & 0x7fff];
    x = (screen_x * 2.0 - 1.0) * tangent / camera->aspect;
    y = (1.0 - screen_y * 2.0) * tangent;

    near_point.x = x * camera->near_clip;
    near_point.y = y * camera->near_clip;
    near_point.z = camera->near_clip;
    far_point.x = x * camera->far_clip;
    far_point.y = y * camera->far_clip;
    far_point.z = camera->far_clip;

    NuVecMtxTransform(ray_start, &near_point, &camera->mtx);
    NuVecMtxTransform(ray_end, &far_point, &camera->mtx);
}
