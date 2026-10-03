#include "saga.h"

#include <string.h>

NUCAMERA *__cdecl NuCameraCreate(void) {
    NUCAMERA *camera = (NUCAMERA *)NuAlloc(sizeof(NUCAMERA), NuCameraAllocSource, 0x11e);

    NuMtxSetIdentity(&camera->mtx);
    camera->fov = 0.75f;
    camera->near_clip = 0.15f;
    camera->portal_nearclip = 0.0f;
    camera->far_clip = 10000.0f;
    camera->aspect = 1.0f;
    camera->unknown_58 = 1.0f;
    camera->unknown_5c = 1.0f;
    camera->scale.y = 1.0f;
    camera->scale.x = 1.0f;
    camera->scale.z = 1.0f;
    camera->unknown_50 = 1.0f;
    camera->unknown_54 = 1.0f;
    camera->unknown_64 = 1.0f;
    camera->unknown_60 = 1.0f;
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

void __cdecl PCClipScreenUnsupported(void) {
}

i32 __cdecl NuCameraClipTestExtents(NUVEC *min, NUVEC *max, NUMTX *world, f32 far_clip, i32 clip_screen) {
    NUVEC corners[8];
    NUVEC transformed[8];
    u32 common;
    u32 combined;
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

    ClipFarLimits[0] = far_clip;
    ClipFarLimits[1] = far_clip;
    ClipFarLimits[2] = far_clip;
    ClipFarLimits[3] = far_clip;
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
    common = 0xffffffffu;
    combined = 0;
    if (PCClipUseSSE != 0) {
        for (i = 0; i < 8; ++i) {
            NUVEC4BLOCK_PC *block = &ClipCornerBlocks[i >> 2];
            block->x[i & 3] = corners[i].x;
            block->y[i & 3] = corners[i].y;
            block->z[i & 3] = corners[i].z;
        }
        PCVecMtxTransformClipBlocks(ClipTransformedBlocks, ClipCornerBlocks, &clip_test_mtx, 8,
                                    clip_outcode_workspace_prefix);
        for (i = 0; i < 8; ++i) {
            common &= clip_outcode_workspace_prefix[i];
            combined |= clip_outcode_workspace_prefix[i];
        }
    } else {
        NuVecMtxTransformBlock(transformed, corners, &clip_test_mtx, 8);
        for (i = 0; i < 8; ++i) {
            u32 bits = 0;
            NUVEC *v = &transformed[i];
            f32 negative_z = -v->z;
            if (v->z < global_camera.near_clip)
                bits |= 0x20;
            if (v->z > far_clip)
                bits |= 0x10;
            if (v->x < negative_z * zx)
                bits |= 1;
            if (v->x > v->z * zx)
                bits |= 2;
            if (v->y < negative_z * zy)
                bits |= 8;
            if (v->y > v->z * zy)
                bits |= 4;
            common &= bits;
            combined |= bits;
        }
    }
    if (common != 0)
        return 0;
    return combined == 0 ? 1 : 2;
}

/* Reconstructed storage qualifiers express the original F32 checkpoints;
   they do not assert historical volatile declarations. */
void __cdecl NuCameraSetVPortClipMtx(NUMTX *out, NUMTX *view, NUCAMERA *camera, i32 fast) {
    if (fast == 0) {
        volatile f32 far_clip =
            (camera->unknown_64 == 0.0f ? camera->far_clip : camera->unknown_64) - NuCameraProjectionFarClipBias;
        volatile f32 near_clip = camera->unknown_60 == 0.0f ? camera->near_clip : camera->unknown_60;
        volatile f32 half_fov;
        volatile f32 cotangent;
        volatile f32 horizontal;
        i32 angle;
        half_fov = (f32)((f64)camera->fov * 0.5);
        angle = (i32)((f64)half_fov * AIPathAngularScale);
        cotangent = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] / NuTrigTable[(angle >> 1) & 0x7fff];
        horizontal = (camera->aspect * cotangent) * NuCameraVPortProjectionScaleX;
        memset(&pc_vport_mtx, 0, sizeof(pc_vport_mtx));
        pc_vport_mtx.m00 = horizontal;
        pc_vport_mtx.m11 = NuCameraVPortProjectionScaleY * cotangent;
        pc_vport_mtx.m22 = (far_clip + near_clip) / (far_clip - near_clip);
        pc_vport_mtx.m23 = 1.0f;
        pc_vport_mtx.m32 = ((-2.0f * far_clip) * near_clip) / (far_clip - near_clip);
        pc_vport_mtx.m33 = 0.0f;
    }
    NuMtxMulVU0(out, view, &pc_vport_mtx);
}

void __cdecl NuCameraSetScissorClipMtx(NUMTX *out, NUMTX *view, NUCAMERA *camera, i32 fast) {
    if (fast == 0) {
        volatile f32 far_clip =
            (camera->unknown_64 == 0.0f ? camera->far_clip : camera->unknown_64) - NuCameraProjectionFarClipBias;
        volatile f32 near_clip = camera->unknown_60 == 0.0f ? camera->near_clip : camera->unknown_60;
        volatile f32 half_fov;
        volatile f32 cotangent;
        volatile f32 horizontal;
        i32 angle;
        half_fov = (f32)((f64)camera->fov * 0.5);
        angle = (i32)((f64)half_fov * AIPathAngularScale);
        cotangent = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] / NuTrigTable[(angle >> 1) & 0x7fff];
        horizontal = (camera->aspect * NuCameraScissorProjectionScaleX) * cotangent;
        memset(&NuCameraScissorProjectionScratch, 0, sizeof(NuCameraScissorProjectionScratch));
        NuCameraScissorProjectionScratch.m00 = horizontal;
        NuCameraScissorProjectionScratch.m11 = NuCameraScissorProjectionScaleY * cotangent;
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
        return 1;
    if (0.0f > (global_camera.far_clip - transformed.z) + radius)
        return 1;
    NuVec4MtxTransform(&plane_distances, &transformed, &clip_planes);
    if (-radius > plane_distances.x)
        return 1;
    if (-radius > plane_distances.y)
        return 1;
    if (-radius > plane_distances.z)
        return 1;
    if (-radius > plane_distances.w)
        return 1;
    return 0;
}

void __cdecl NuCameraSetProjectionMtx(NUMTX *out, f32 fov, f32 aspect, f32 near_clip, f32 far_clip) {
    volatile f32 half_fov = (f32)((f64)fov * 0.5);
    i32 angle = (i32)((f64)half_fov * AIPathAngularScale);
    volatile f32 cotangent = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] / NuTrigTable[(angle >> 1) & 0x7fff];
    volatile f32 depth_factor = far_clip / (far_clip - near_clip);
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

void __cdecl NuCameraSetEx(NUCAMERA *camera, i32 fast) {
    global_camera = *camera;
    NuAtan2D(-global_camera.mtx.m20, -global_camera.mtx.m22);
    STUBBED_NAMED("NuCameraSetEx: FaceYDirStream at 0x005ff830");
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
        STUBBED_NAMED("NuCameraSetEx: scissor frustum ratios");
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
        STUBBED_NAMED("NuCameraSetEx: frustum setup at 0x005d5500");
    }
    STUBBED_NAMED("NuCameraSetEx: viewport/scissor render state submission");
    STUBBED_NAMED("NuCameraSetEx: renderer flags, viewport offsets and clip-mode synchronization");
    STUBBED_NAMED("NuCameraSetEx: conditional renderer cache invalidation");
}

i32 __cdecl NuCameraClipTestExtentsAxisAligned(NUVEC *center, NUVEC *extent, f32 far_clip) {
    struct BoundProjection {
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
