/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

struct CAMERA_CLIP_PACKET_PC;
struct NUVEC4BLOCK_PC;

typedef struct nucamera_s {
    NUMTX mtx;
    f32 fov;
    f32 aspect;
    f32 near_clip;
    f32 far_clip;
    f32 unknown_50;
    f32 unknown_54;
    f32 unknown_58;
    f32 unknown_5c;
    f32 unknown_60;
    f32 unknown_64;
    f32 portal_nearclip;
    NUVEC scale;
} NUCAMERA;

STATIC_ASSERT(sizeof(NUCAMERA) == 0x78);

STATIC_ASSERT(offsetof(NUCAMERA, mtx) == 0);
STATIC_ASSERT(offsetof(NUCAMERA, fov) == 0x40);
STATIC_ASSERT(offsetof(NUCAMERA, aspect) == 0x44);
STATIC_ASSERT(offsetof(NUCAMERA, near_clip) == 0x48);
STATIC_ASSERT(offsetof(NUCAMERA, far_clip) == 0x4c);
STATIC_ASSERT(offsetof(NUCAMERA, unknown_50) == 0x50);
STATIC_ASSERT(offsetof(NUCAMERA, unknown_54) == 0x54);
STATIC_ASSERT(offsetof(NUCAMERA, unknown_58) == 0x58);
STATIC_ASSERT(offsetof(NUCAMERA, unknown_5c) == 0x5c);
STATIC_ASSERT(offsetof(NUCAMERA, unknown_60) == 0x60);
STATIC_ASSERT(offsetof(NUCAMERA, unknown_64) == 0x64);
STATIC_ASSERT(offsetof(NUCAMERA, portal_nearclip) == 0x68);
STATIC_ASSERT(offsetof(NUCAMERA, scale) == 0x6c);
#ifdef __cplusplus
extern "C" {
#endif
    f32 NuCameraFocalLenToFOV(f32);
    i32 NuCameraClipTestPoints(NUVEC *, i32, NUMTX *);
    i32 NuCameraClipTestExtentsAxisAligned(NUVEC *, NUVEC *, f32);
    i32 NuCameraClipTestSphere(NUVEC *, f32, NUMTX *);
    extern NUCAMERA global_camera;
    extern NUMTX psmtx;
    void NuCameraSet(NUCAMERA *camera);
    void NuCameraSetEx(NUCAMERA *, i32);
    void NuCameraSetProjectionMtx(NUMTX *, f32, f32, f32, f32);
    extern NUMTX clip_test_mtx;
    extern f32 zx;
    extern f32 zy;
    void PCVecMtxTransformClipBlocks(struct NUVEC4BLOCK_PC *, struct NUVEC4BLOCK_PC *, NUMTX *, i32, u32 *);
    i32 NuCameraClipTestExtents(NUVEC *, NUVEC *, NUMTX *, f32, i32);
    void NuCameraSetVPortClipMtx(NUMTX *out, NUMTX *view, NUCAMERA *camera, i32 fast);
    void NuCameraSetScissorClipMtx(NUMTX *out, NUMTX *view, NUCAMERA *camera, i32 fast);
    extern NUVEC cam_axes;
    void NuCameraGet(NUCAMERA *camera);
    NUCAMERA *NuCameraCreate(void);
    void NuCameraDestroy(NUCAMERA *camera);
    NUMTX *NuCameraGetMtx(void);
    void NuCameraSetAxes(NUVEC *axes);
    void NuCameraGetAxes(NUVEC *axes);
    NUCAMERA *NuCameraGetCam(void);
    NUMTX *NuCameraGetViewMtx(void);
    NUMTX *NuCameraGetProjectionMtx(void);
    NUMTX *NuCameraGetClippingMtx(void);
    NUMTX *NuCameraGetScalingMtx(void);
    NUMTX *NuCameraGetVPCSMtx(void);
    NUMTX *NuCameraGetVPCMtx(void);
    NUMTX *NuCameraGetVPMtx(void);
    NUMTX *NuCameraGetPCMtx(void);
    void NuCameraGetClipMtx(NUMTX *viewport, NUMTX *scissor);
    NUMTX *NuCameraGetClipPlanes(void);
    extern NUMTX vmtx;
    extern NUMTX pmtx;
    extern NUMTX cmtx;
    extern NUMTX smtx;
    extern NUMTX vpmtx;
    extern NUMTX vpc_sci_mtx;
    extern NUMTX vpc_vport_mtx;
    extern NUMTX pc_vport_mtx;
    extern NUMTX vpsmtx;
    extern NUMTX clip_planes;
    extern VARIPTR *NuDisplayListStreamCursor;
    extern i32 TreeInitialised;
    extern f32 zxs;
    extern f32 zys;

    void NuCameraBuildClipPlanes(void);
    i32 NuCameraClipTestExtentsGeneric(NUVEC *, NUVEC *, NUMTX *, f32, i32, f32 *);
    extern i32 cam_state_count;

    void NuCameraCalcClipMtx(struct CAMERA_CLIP_PACKET_PC *, NUCAMERA *, i32);
    void NuCameraTransformView(NUVEC *, NUVEC *, i32, NUMTX *);
    void NuCameraTransformScreenClip(NUVEC *, NUVEC *, i32, NUMTX *);
    void NuCameraTransformScreen(NUVEC *, NUVEC *, i32, NUMTX *);
    i32 NuCameraSaveState(void);
    void NuCameraClearStateBuffer(void);
    f32 NuCameraDistSqr(NUVEC *);
#ifdef __cplusplus
}
#endif
