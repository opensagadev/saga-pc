#include "saga.h"

#include <string.h>

NUCAMERA *__cdecl NuCameraCreate(void) {
    NUCAMERA *camera = (NUCAMERA *)NuAlloc(sizeof(NUCAMERA), NuCameraAllocSource, 0x11e);

    NuMtxSetIdentity(&camera->mtx);
    camera->fov = 0.75f;
    camera->near_clip = 0.15f;
    camera->portal_nearclip = 0.0f;
    camera->far_clip = 10000.0f;
    camera->aspect = 0.75f;
    camera->unknown_58 = 1.0f;
    camera->unknown_5c = 1.0f;
    camera->scale.z = 1.0f;
    camera->scale.y = 1.0f;
    camera->scale.x = 1.0f;
    camera->unknown_50 = 0.0f;
    camera->unknown_54 = 0.0f;
    camera->unknown_64 = 0.0f;
    camera->unknown_60 = 0.0f;
    return camera;
}

void __cdecl NuCameraGet(NUCAMERA *camera) {
    *camera = global_camera;
}

NUMTX *__cdecl NuCameraGetMtx(void) {
    return &global_camera.mtx;
}

void __cdecl NuCameraSetAxes(NUVEC *axes) {
    cam_axes = *axes;
}

void __cdecl NuCameraSet(NUCAMERA *camera) {
    NuCameraSetEx(camera, 0);

    NuVpSetIdentityPc(&cmtx);
    NuMtxMulH(&vpc_vport_mtx, &vpmtx, &cmtx);
    vpc_sci_mtx = vpc_vport_mtx;

    PCDeviceSetTransform(2, &vmtx);
    PCDeviceSetTransform(3, &pmtx);
}

void __cdecl NuCameraGetAxes(NUVEC *axes) {
    *axes = cam_axes;
}

NUCAMERA *__cdecl NuCameraGetCam(void) {
    return &global_camera;
}

NUMTX *__cdecl NuCameraGetViewMtx(void) {
    return &vmtx;
}

NUMTX *__cdecl NuCameraGetProjectionMtx(void) {
    return &pmtx;
}

NUMTX *__cdecl NuCameraGetClippingMtx(void) {
    return &cmtx;
}

NUMTX *__cdecl NuCameraGetScalingMtx(void) {
    return &smtx;
}

NUMTX *__cdecl NuCameraGetVPCSMtx(void) {
    return &vpsmtx;
}

NUMTX *__cdecl NuCameraGetVPCMtx(void) {
    return &vpc_vport_mtx;
}

NUMTX *__cdecl NuCameraGetVPMtx(void) {
    return &vpmtx;
}

NUMTX *__cdecl NuCameraGetPCMtx(void) {
    return &pc_vport_mtx;
}

void __cdecl NuCameraGetClipMtx(NUMTX *viewport, NUMTX *scissor) {
    if (viewport != NULL) {
        *viewport = vpc_vport_mtx;
    }
    if (scissor != NULL) {
        *scissor = vpc_sci_mtx;
    }
}

NUMTX *__cdecl NuCameraGetClipPlanes(void) {
    return &clip_planes;
}

void __cdecl NuMtxCalcCheapFaceOn(NUMTX *m, NUVEC *position) {
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

void __cdecl NuMtxCalcDebrisFaceOn(NUMTX *m) {
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

#define NUCAMERA_CLIP_CODE_EX(code, v, near_enabled, horizontal, vertical)                                             \
    {                                                                                                                  \
        if ((v).z < global_camera.near_clip && (near_enabled))                                                         \
            (code) |= 0x20;                                                                                            \
        if ((v).z > far_clip)                                                                                          \
            (code) |= 0x10;                                                                                            \
        if ((v).x < -(v).z * (horizontal))                                                                             \
            (code) |= 1;                                                                                               \
        if ((v).x > (v).z * (horizontal))                                                                              \
            (code) |= 2;                                                                                               \
        if ((v).y < -(v).z * (vertical))                                                                               \
            (code) |= 8;                                                                                               \
        if ((v).y > (v).z * (vertical))                                                                                \
            (code) |= 4;                                                                                               \
    }

#define NUCAMERA_CLIP_CODE(code, v) NUCAMERA_CLIP_CODE_EX(code, v, 1, zx, zy)

i32 __cdecl NuCameraClipTestExtents(NUVEC *min, NUVEC *max, NUMTX *world, f32 far_clip, i32 clip_screen) {
    NUVEC corners[8];
    NUVEC transformed[8];
    u8 codes[8];
    i32 i;

    if (PCClipExtentsEnabled == 0)
        return 1;
    if (clip_screen != 0) {
        PCClipScreenUnsupported();
        for (;;) {
        }
    }
    if (far_clip == 0.0f)
        far_clip = global_camera.unknown_64 != 0.0f ? global_camera.unknown_64 : global_camera.far_clip;
    if (world == NULL || world == &NuMtxSetIdentityTemplate)
        clip_test_mtx = vmtx;
    else
        NuMtxMul(&clip_test_mtx, world, &vmtx);

    ClipFarLimits[3] = far_clip;
    ClipFarLimits[2] = far_clip;
    ClipFarLimits[1] = far_clip;
    ClipFarLimits[0] = far_clip;
    if (PCClipUseSSE == 0) {
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
        NuVecMtxTransformBlock(transformed, corners, &clip_test_mtx, 8);
        memset(codes, 0, sizeof(codes));
        for (i = 0; i < 8; i += 4) {
            NUCAMERA_CLIP_CODE(codes[i], transformed[i]);
            NUCAMERA_CLIP_CODE(codes[i + 1], transformed[i + 1]);
            NUCAMERA_CLIP_CODE(codes[i + 2], transformed[i + 2]);
            NUCAMERA_CLIP_CODE(codes[i + 3], transformed[i + 3]);
        }
        if ((codes[0] & codes[1] & codes[2] & codes[3] & codes[4] & codes[5] & codes[6] & codes[7]) != 0)
            return 0;
        return (codes[0] | codes[1] | codes[2] | codes[3] | codes[4] | codes[5] | codes[6] | codes[7]) != 0 ? 2 : 1;
    } else {
        u32 *outcode = clip_outcode_workspace_prefix;
        ClipCornerBlocks[0].x[0] = min->x;
        ClipCornerBlocks[0].y[0] = min->y;
        ClipCornerBlocks[0].z[0] = min->z;
        ClipCornerBlocks[0].x[1] = max->x;
        ClipCornerBlocks[0].y[1] = max->y;
        ClipCornerBlocks[0].z[1] = max->z;
        ClipCornerBlocks[0].x[2] = min->x;
        ClipCornerBlocks[0].y[2] = min->y;
        ClipCornerBlocks[0].z[2] = max->z;
        ClipCornerBlocks[0].x[3] = max->x;
        ClipCornerBlocks[0].y[3] = max->y;
        ClipCornerBlocks[0].z[3] = min->z;
        ClipCornerBlocks[1].x[0] = min->x;
        ClipCornerBlocks[1].y[0] = max->y;
        ClipCornerBlocks[1].z[0] = min->z;
        ClipCornerBlocks[1].x[1] = min->x;
        ClipCornerBlocks[1].y[1] = max->y;
        ClipCornerBlocks[1].z[1] = max->z;
        ClipCornerBlocks[1].x[2] = max->x;
        ClipCornerBlocks[1].y[2] = min->y;
        ClipCornerBlocks[1].z[2] = min->z;
        ClipCornerBlocks[1].x[3] = max->x;
        ClipCornerBlocks[1].y[3] = min->y;
        ClipCornerBlocks[1].z[3] = max->z;
        PCVecMtxTransformClipBlocks(ClipTransformedBlocks, ClipCornerBlocks, &clip_test_mtx, 8, outcode);
        if ((clip_outcode_workspace_prefix[7] & clip_outcode_workspace_prefix[6] & clip_outcode_workspace_prefix[5] &
             clip_outcode_workspace_prefix[4] & clip_outcode_workspace_prefix[3] & clip_outcode_workspace_prefix[2] &
             clip_outcode_workspace_prefix[1] & clip_outcode_workspace_prefix[0]) != 0)
            return 0;
        return (clip_outcode_workspace_prefix[7] | clip_outcode_workspace_prefix[6] | clip_outcode_workspace_prefix[5] |
                clip_outcode_workspace_prefix[4] | clip_outcode_workspace_prefix[3] | clip_outcode_workspace_prefix[2] |
                clip_outcode_workspace_prefix[1] | clip_outcode_workspace_prefix[0]) != 0
                   ? 2
                   : 1;
    }
}

/* Reconstructed storage qualifiers express the original F32 checkpoints;
   they do not assert historical volatile declarations. */
void __cdecl NuCameraSetVPortClipMtx(NUMTX *out, NUMTX *view, NUCAMERA *camera, i32 fast) {
    if (fast == 0) {
        f32 far_clip, near_clip;
        volatile f32 half_fov;
        f32 cotangent;
        f32 horizontal;
        i32 angle;
        if (camera->unknown_64 == 0.0f)
            far_clip = camera->far_clip - NuCameraProjectionFarClipBias;
        else
            far_clip = camera->unknown_64 - NuCameraProjectionFarClipBias;
        if (camera->unknown_60 == 0.0f)
            near_clip = camera->near_clip;
        else
            near_clip = camera->unknown_60;
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

void __cdecl NuCameraSetScissorClipMtx(NUMTX *out, NUMTX *view, NUCAMERA *camera, i32 fast) {
    if (fast == 0) {
        f32 far_clip, near_clip;
        volatile f32 half_fov;
        f32 cotangent;
        f32 horizontal;
        i32 angle;
        if (camera->unknown_64 == 0.0f)
            far_clip = camera->far_clip - NuCameraProjectionFarClipBias;
        else
            far_clip = camera->unknown_64 - NuCameraProjectionFarClipBias;
        if (camera->unknown_60 == 0.0f)
            near_clip = camera->near_clip;
        else
            near_clip = camera->unknown_60;
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

i32 __cdecl NuCameraClipTestSphere(NUVEC *point, f32 radius, NUMTX *world) {
    NUVEC transformed;
    NUVEC4 plane_distances;
    if (world != NULL) {
        NuVecMtxTransform(&transformed, point, world);
        NuVecMtxTransform(&transformed, &transformed, &vmtx);
    } else {
        NuVecMtxTransform(&transformed, point, &vmtx);
    }
    if (0.0f > (transformed.z - global_camera.near_clip) + radius)
        goto rejected;
    if (0.0f > (global_camera.far_clip - transformed.z) + radius)
        goto rejected;
    NuVec4MtxTransform(&plane_distances, &transformed, &clip_planes);
    if (plane_distances.x < -radius)
        goto rejected;
    if (-radius > plane_distances.y)
        goto rejected;
    if (-radius > plane_distances.z)
        goto rejected;
    if (-radius > plane_distances.w)
        goto rejected;
    return 0;
rejected:
    return 1;
}

void __cdecl NuCameraSetProjectionMtx(NUMTX *out, f32 fov, f32 aspect, f32 near_clip, f32 far_clip) {
    volatile f32 half_fov = (f32)((f64)fov * 0.5);
    i32 angle = (i32)((f64)half_fov * AIPathAngularScale);
    f32 cotangent = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] / NuTrigTable[(angle >> 1) & 0x7fff];
    f32 depth_factor = far_clip / (far_clip - near_clip);
    memset(out, 0, sizeof(*out));
    out->m00 = aspect * cotangent;
    out->m11 = cotangent;
    out->m22 = depth_factor;
    out->m23 = 1.0f;
    out->m32 = -depth_factor * near_clip;
    pmtx.m00 *= NuCameraProjectionGlobalScaleX;
    pmtx.m11 *= NuCameraProjectionGlobalScaleY;
    pmtx.m20 = NuCameraProjectionGlobalOffsetX;
    pmtx.m21 = NuCameraProjectionGlobalOffsetY;
}

extern f32 VisibilityAlternateWidthPc;
extern i32 NuCameraScissorWidthPc;
extern i32 NuCameraScissorHeightPc;
i32 __cdecl NuRndrSetViewMtx(NUMTX *view_projection, NUMTX *viewport_clip, NUMTX *scissor_clip);
void __cdecl FaceYDirStream(i32 angle);

void __cdecl NuCameraSetEx(NUCAMERA *camera, i32 fast) {
    global_camera = *camera;
    FaceYDirStream(NuAtan2D(-global_camera.mtx.m20, -global_camera.mtx.m22));
    if (fast == 0)
        NuVpUpdate();
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
        NuMtxInv(&reflection_inverse, &NuCameraReflection);
        vmtx = reflection_inverse;
        if (NuCameraMode == 1)
            NuMtxMul(&vmtx, &vmtx, &mirror);
        NuMtxMul(&vmtx, &vmtx, &NuCameraReflection);
        NuMtxMul(&vmtx, &vmtx, &camera_inverse);
    }
    if (fast == 0) {
        if (NuCameraOrthographic == 0) {
            NuCameraSetProjectionMtx(&pmtx, global_camera.fov, global_camera.aspect, global_camera.near_clip,
                                     global_camera.far_clip);
            pmtx.m00 *= global_camera.unknown_58;
            pmtx.m11 *= global_camera.unknown_5c;
            pmtx.m20 += global_camera.unknown_50;
            pmtx.m21 += global_camera.unknown_54;
        } else {
            memset(&pmtx, 0, sizeof(pmtx));
            pmtx.m00 = NuCameraOrthoScale * global_camera.aspect;
            pmtx.m11 = NuCameraOrthoScale;
            pmtx.m22 = 1.0f;
            pmtx.m33 = 1.0f;
        }
    }
    NuCameraSetVPortClipMtx(&vpc_vport_mtx, &vmtx, &global_camera, fast);
    if (NuCameraClipScissorToViewport != NuCameraPreviousClipScissorToViewport) {
        fast = 0;
        NuCameraPreviousClipScissorToViewport = NuCameraClipScissorToViewport;
    }
    if (NuCameraClipScissorToViewport != 0)
        NuCameraSetVPortClipMtx(&vpc_sci_mtx, &vmtx, &global_camera, fast);
    else
        NuCameraSetScissorClipMtx(&vpc_sci_mtx, &vmtx, &global_camera, fast);
    smtx = NuCameraScalingTemplate;
    NuMtxMulH(&vpmtx, &vmtx, &pmtx);
    NuMtxMulH(&vpsmtx, &vpmtx, &smtx);
    NuMtxMulH(&psmtx, &pmtx, &smtx);
    if (fast == 0) {
        volatile f32 half_fov = (f32)((f64)global_camera.fov * 0.5);
        i32 angle = (i32)((f64)half_fov * AIPathAngularScale);
        volatile f32 side;
        volatile f32 squared;
        volatile f32 reciprocal;
        zy = NuTrigTable[(angle >> 1) & 0x7fff] / NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
        zx = zy / global_camera.aspect;
        VisibilityAlternateWidthPc = zx * NuCameraScissorWidthPc / PS2_REZ_W;
        VisibilityAlternateHeightPc = zy * NuCameraScissorHeightPc / PS2_REZ_H;
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

i32 __cdecl NuCameraClipTestExtentsAxisAligned(NUVEC *center, NUVEC *extent, f32 far_clip) {
    struct __declspec(align(16)) BoundProjection {
        NUVEC sides;
        f32 fourth;
        f32 far_plane;
        f32 near_plane;
    } distance, radius;
    f32 saved_far = AxisClipNearFarPlanesPc.m30;
    if (far_clip != 0.0f) {
        AxisClipNearFarPlanesPc.m30 -= global_camera.far_clip;
        AxisClipNearFarPlanesPc.m30 += far_clip;
    }
    NuVecMtxTransform(&distance.sides, center, &AxisClipFrustumPlanesPc);
    distance.fourth = AxisClipFrustumPlanesPc.m33 +
                      (center->z * AxisClipFrustumPlanesPc.m23 +
                       (center->x * AxisClipFrustumPlanesPc.m03 + center->y * AxisClipFrustumPlanesPc.m13));
    distance.far_plane = AxisClipNearFarPlanesPc.m30 +
                         (center->z * AxisClipNearFarPlanesPc.m20 +
                          (center->x * AxisClipNearFarPlanesPc.m00 + center->y * AxisClipNearFarPlanesPc.m10));
    distance.near_plane = AxisClipNearFarPlanesPc.m31 +
                          (center->z * AxisClipNearFarPlanesPc.m21 +
                           (center->x * AxisClipNearFarPlanesPc.m01 + center->y * AxisClipNearFarPlanesPc.m11));
    NuVecMtxTransform(&radius.sides, extent, &AxisClipAbsFrustumPlanesPc);
    radius.fourth = extent->x * AxisClipAbsFrustumPlanesPc.m03 + extent->y * AxisClipAbsFrustumPlanesPc.m13 +
                    extent->z * AxisClipAbsFrustumPlanesPc.m23;
    radius.far_plane = extent->x * AxisClipNearFarPlanesPc.m02 + extent->y * AxisClipNearFarPlanesPc.m12 +
                       extent->z * AxisClipNearFarPlanesPc.m22;
    radius.near_plane = extent->x * AxisClipNearFarPlanesPc.m03 + extent->y * AxisClipNearFarPlanesPc.m13 +
                        extent->z * AxisClipNearFarPlanesPc.m23;
    AxisClipNearFarPlanesPc.m30 = saved_far;

    if (distance.sides.x < -radius.sides.x || distance.sides.y < -radius.sides.y ||
        distance.sides.z < -radius.sides.z || distance.fourth < -radius.fourth ||
        distance.far_plane < -radius.far_plane || distance.near_plane < -radius.near_plane)
        return 0;
    if (distance.sides.x < radius.sides.x || distance.sides.y < radius.sides.y || distance.sides.z < radius.sides.z ||
        distance.fourth < radius.fourth) {
        NuVecMtxTransform(&distance.sides, center, &AxisClipScissorPlanesPc);
        distance.fourth = AxisClipScissorPlanesPc.m33 +
                          (center->z * AxisClipScissorPlanesPc.m23 +
                           (center->x * AxisClipScissorPlanesPc.m03 + center->y * AxisClipScissorPlanesPc.m13));
        NuVecMtxTransform(&radius.sides, extent, &AxisClipAbsScissorPlanesPc);
        radius.fourth = AxisClipAbsScissorPlanesPc.m33 +
                        (extent->z * AxisClipAbsScissorPlanesPc.m23 +
                         (extent->x * AxisClipAbsScissorPlanesPc.m03 + extent->y * AxisClipAbsScissorPlanesPc.m13));
        if (distance.sides.x < radius.sides.x || distance.sides.y < radius.sides.y ||
            distance.sides.z < radius.sides.z || distance.fourth < radius.fourth)
            return 2;
    }
    return 1;
}

void __cdecl NuMtxCalcCheapFaceY_v2(NUMTX *matrix, NUVEC *direction) {
    NUVEC *first = (NUVEC *)&matrix->m00;
    NUVEC *second = (NUVEC *)&matrix->m10;
    NUVEC *third = (NUVEC *)&matrix->m20;
    NuVecCross(first, (NUVEC *)&global_camera.mtx.m20, direction);
    NuVecNorm(first, first);
    *second = *direction;
    NuVecCross(third, first, second);
    matrix->m03 = matrix->m13 = matrix->m23 = 0.0f;
    matrix->m30 = matrix->m31 = matrix->m32 = 0.0f;
    matrix->m33 = 1.0f;
}

f32 __cdecl NuCameraFocalLenToFOV(f32 focal_length) {
    f32 ratio = (f32)(18.0 / focal_length);
    f32 result = (f32)(NuAtan2DAF(ratio, 1.0f) * PCAtan2RadianScale * 2.0);
    return result;
}

i32 __cdecl NuCameraClipTestPoints(NUVEC *points, i32 count, NUMTX *world_matrix) {
    NUMTX matrix;
    i32 common = -1;
    i32 i;
    if (world_matrix != NULL)
        NuMtxMulH(&matrix, world_matrix, &vmtx);
    else
        matrix = vmtx;
    for (i = 0; i < count; ++i) {
        NUVEC t;
        i32 flags = 0;
        f32 negative_z;
        NuVecMtxTransform(&t, &points[i], &matrix);
        if (t.z < global_camera.near_clip)
            flags |= 0x20;
        if (t.z > global_camera.far_clip)
            flags |= 0x10;
        negative_z = -t.z;
        if (t.x < negative_z * zx)
            flags |= 1;
        if (t.x > t.z * zx)
            flags |= 2;
        if (t.y < negative_z * zy)
            flags |= 8;
        if (t.y > t.z * zy)
            flags |= 4;
        common &= flags;
    }
    return common;
}

#include <math.h>
extern NUMTX VisibilityZeroMatrixTemplatePc;
void __cdecl NuCameraBuildClipPlanes(void);
void __cdecl NuMtxCalcFaceOn(NUMTX *, NUVEC *);
void __cdecl NuMtxCalcFaceY(NUMTX *, NUVEC *);
void __cdecl NuMtxCalcFaceOn(NUMTX *m, NUVEC *v) {
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

void __cdecl NuMtxCalcFaceY(NUMTX *m, NUVEC *v) {
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

#include <float.h>
i32 __cdecl NuCameraClipTestExtentsGeneric(NUVEC *min, NUVEC *max, NUMTX *world_mtx, f32 far_clip, i32 flags,
                                           f32 *min_depth) {
    NUMTX transform;
    NUVEC corners[8];
    NUVEC view[8];
    char results[8];
    i32 i;
    if (far_clip == 0.0f)
        far_clip = global_camera.far_clip;
    if (world_mtx == NULL || world_mtx == &NuMtxSetIdentityTemplate)
        transform = vmtx;
    else
        NuMtxMul(&transform, world_mtx, &vmtx);
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
        if (view[i].z < *min_depth)
            *min_depth = view[i].z;
        NUCAMERA_CLIP_CODE_EX(results[i], view[i], (flags & 2) == 0, zx, zy);
    }
    if ((results[0] & results[1] & results[2] & results[3] & results[4] & results[5] & results[6] & results[7]) != 0)
        return 0;
    if ((results[0] | results[1] | results[2] | results[3] | results[4] | results[5] | results[6] | results[7]) == 0)
        return 1;
    if ((flags & 1) != 0)
        return 2;
    memset(results, 0, sizeof(results));
    for (i = 0; i < 8; i += 4) {
        NUCAMERA_CLIP_CODE_EX(results[i], view[i], (flags & 2) == 0, VISIBILITY_ALTERNATE_WIDTH_PC,
                              VisibilityAlternateHeightPc);
        NUCAMERA_CLIP_CODE_EX(results[i + 1], view[i + 1], (flags & 2) == 0, VISIBILITY_ALTERNATE_WIDTH_PC,
                              VisibilityAlternateHeightPc);
        NUCAMERA_CLIP_CODE_EX(results[i + 2], view[i + 2], (flags & 2) == 0, VISIBILITY_ALTERNATE_WIDTH_PC,
                              VisibilityAlternateHeightPc);
        NUCAMERA_CLIP_CODE_EX(results[i + 3], view[i + 3], (flags & 2) == 0, VISIBILITY_ALTERNATE_WIDTH_PC,
                              VisibilityAlternateHeightPc);
    }
    if ((results[0] & results[1] & results[2] & results[3] & results[4] & results[5] & results[6] & results[7]) != 0)
        return 1;
    if ((results[0] | results[1] | results[2] | results[3] | results[4] | results[5] | results[6] | results[7]) == 0)
        return 1;
    return 2;
}

void __cdecl NuCameraCalcClipMtx(CAMERA_CLIP_PACKET_PC *clip, NUCAMERA *camera, i32 use_cached_projection) {
    static NUMTX clip_inverse = {0};
    if (use_cached_projection == 0 || (clip->flags & 1) == 0) {
        i32 angle;
        f32 half_fov, cotangent, x, y;
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

void __cdecl NuCameraTransformView(NUVEC *view, NUVEC *world, i32 count, NUMTX *matrix) {
    NUMTX transform;
    NUVEC *end = world + count;
    if (matrix != NULL)
        NuMtxMul(&transform, matrix, &vmtx);
    else
        transform = vmtx;
    while (world < end)
        NuVecMtxTransform(view++, world++, &transform);
}

void __cdecl NuCameraTransformScreenClip(NUVEC *screen, NUVEC *world, i32 count, NUMTX *matrix) {
    NUMTX transform;
    NUVEC *end = world + count;
    if (matrix != NULL)
        NuMtxMulH(&transform, matrix, &vpc_vport_mtx);
    else
        transform = vpc_vport_mtx;
    while (world < end)
        NuVecMtxTransformH(screen++, world++, &transform);
}

void __cdecl NuCameraTransformScissorClip(NUVEC *screen, NUVEC *world, i32 count, NUMTX *matrix) {
    NUMTX transform;
    NUVEC *end = world + count;
    if (matrix != NULL)
        NuMtxMulH(&transform, matrix, &vpc_sci_mtx);
    else
        transform = vpc_sci_mtx;
    while (world < end)
        NuVecMtxTransformH(screen++, world++, &transform);
}

void __cdecl NuCameraTransformScreen(NUVEC *screen, NUVEC *world, i32 count, NUMTX *matrix) {
    NUMTX transform;
    NUVEC *end = world + count;
    if (matrix != NULL)
        NuMtxMulH(&transform, matrix, &vpsmtx);
    else
        transform = vpsmtx;
    while (world < end)
        NuVecMtxTransformH(screen++, world++, &transform);
}

void NuCameraTransformScreenVU0(NUVEC4 *screen, NUVEC4 *world, i32 count, NUMTX *matrix) {
    NUMTX transform;
    NUVEC4 *end = world + count;
    if (matrix != NULL)
        NuMtxMulVU0(&transform, matrix, &vpsmtx);
    else
        transform = vpsmtx;
    while (world < end)
        NuVec4MtxTransformHVU0(screen++, world++, &transform);
}

i32 __cdecl NuCameraSaveState(void) {
    i32 index = -1;
    if (CameraSavedStateCountPc < 16) {
        CAMERA_SAVED_STATE_PC *state;
        index = CameraSavedStateCountPc++;
        state = &CameraSavedStatesPc[index];
        state->zx = zx;
        state->camera = global_camera;
        state->zy = zy;
        state->zxs = VISIBILITY_ALTERNATE_WIDTH_PC;
        state->zys = VisibilityAlternateHeightPc;
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
        state->planes = CameraClipPlanesPc;
    }
    return index + 1;
}

void __cdecl NuCameraClearStateBuffer(void) {
    CameraSavedStateCountPc = 0;
}

f32 __cdecl NuCameraDistSqr(NUVEC *point) {
    return NuVecDistSqr(point, (NUVEC *)&global_camera.mtx.m30, NULL);
}

void __cdecl NuCameraCalcRay(f32 screen_x, f32 screen_y, NUVEC *ray_start, NUVEC *ray_end, NUCAMERA *camera) {
    NUVEC near_point;
    NUVEC far_point;
    f32 half_fov;
    f32 tangent;
    i32 angle;

    if (camera == NULL)
        camera = &global_camera;

    half_fov = camera->fov * 0.5;
    angle = (i32)(half_fov * AIPathAngularScale);
    tangent = NuTrigTable[(angle >> 1) & 0x7fff] / NuTrigTable[((i32)((u32)angle + 0x4000u) >> 1) & 0x7fff];
    near_point.x = far_point.x = (screen_x * 2.0 - 1.0) * tangent / camera->aspect;
    near_point.y = far_point.y = (1.0 - screen_y * 2.0) * tangent;
    near_point.z = far_point.z = 1.0f;

    near_point.x *= camera->near_clip;
    near_point.y *= camera->near_clip;
    near_point.z *= camera->near_clip;
    far_point.x *= camera->far_clip;
    far_point.y *= camera->far_clip;
    far_point.z *= camera->far_clip;

    NuVecMtxTransform(ray_start, &near_point, &camera->mtx);
    NuVecMtxTransform(ray_end, &far_point, &camera->mtx);
}
