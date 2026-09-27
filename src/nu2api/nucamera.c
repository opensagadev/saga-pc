#include "saga.h"

NUCAMERA *__cdecl NuCameraCreate(void) {
    NUCAMERA *camera = (NUCAMERA *)NuAlloc(sizeof(NUCAMERA), NuCameraAllocSource, 0x11e);

    NuMtxSetIdentity(&camera->mtx);
    camera->fov = 0.75f;
    camera->aspect = 1.0f;
    camera->near_clip = 0.15f;
    camera->far_clip = 10000.0f;
    camera->unknown_50 = 1.0f;
    camera->unknown_54 = 1.0f;
    camera->unknown_58 = 1.0f;
    camera->unknown_5c = 1.0f;
    camera->unknown_60 = 1.0f;
    camera->unknown_64 = 1.0f;
    camera->portal_nearclip = 0.0f;
    camera->scale.x = 1.0f;
    camera->scale.y = 1.0f;
    camera->scale.z = 1.0f;
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
