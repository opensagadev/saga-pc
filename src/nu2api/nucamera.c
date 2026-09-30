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
    /* The PC camera-state builder at 0x005ed540 is still a dependency. */
    STUBBED_NAMED(0x005ed540);

    NuMtxSetIdentity(&cmtx);
    NuMtxMulVU0(&vpc_vport_mtx, &vpmtx, &cmtx);
    vpc_sci_mtx = vpc_vport_mtx;

    /* The final two calls submit the viewport and projection matrices to D3D. */
    STUBBED_NAMED(0x006e56a0);
    STUBBED_NAMED(0x006e56a0);
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

void __cdecl NuCameraSetVPortClipMtx(NUMTX *out, NUMTX *view, NUCAMERA *camera, i32 fast) {
    if (fast == 0) {
        f32 far_clip = camera->unknown_64 == 0.0f ? camera->far_clip : camera->unknown_64;
        f32 near_clip = camera->unknown_60 == 0.0f ? camera->near_clip : camera->unknown_60;
        f32 half_fov = camera->fov * 0.5f;
        i32 angle = (i32)((f64)half_fov * AIPathAngularScale);
        f32 cotangent = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] / NuTrigTable[(angle >> 1) & 0x7fff];
        f32 horizontal = camera->aspect * cotangent * NuCameraVPortProjectionScaleX;
        far_clip -= NuCameraProjectionFarClipBias;
        memset(&pc_vport_mtx, 0, sizeof(pc_vport_mtx));
        pc_vport_mtx.m00 = horizontal;
        pc_vport_mtx.m11 = NuCameraVPortProjectionScaleY * cotangent;
        pc_vport_mtx.m22 = (far_clip + near_clip) / (far_clip - near_clip);
        pc_vport_mtx.m23 = 1.0f;
        pc_vport_mtx.m32 = ((-2.0f * far_clip) * near_clip) / (far_clip - near_clip);
    }
    NuMtxMulVU0(out, view, &pc_vport_mtx);
}

void __cdecl NuCameraSetScissorClipMtx(NUMTX *out, NUMTX *view, NUCAMERA *camera, i32 fast) {
    if (fast == 0) {
        f32 far_clip = camera->unknown_64 == 0.0f ? camera->far_clip : camera->unknown_64;
        f32 near_clip = camera->unknown_60 == 0.0f ? camera->near_clip : camera->unknown_60;
        f32 half_fov = camera->fov * 0.5f;
        i32 angle = (i32)((f64)half_fov * AIPathAngularScale);
        f32 cotangent = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] / NuTrigTable[(angle >> 1) & 0x7fff];
        f32 horizontal = camera->aspect * cotangent * NuCameraScissorProjectionScaleX;
        far_clip -= NuCameraProjectionFarClipBias;
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
