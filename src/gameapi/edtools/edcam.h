#ifndef SAGA_GAMEAPI_EDTOOLS_EDCAM_H
#define SAGA_GAMEAPI_EDTOOLS_EDCAM_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edtools/edui.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 PadFlyMode;
    void edcamSetPos(struct nuvec_s *position);
    void do_Pad_flymode_camera(struct edcam_s *camera, f32 delta_time, struct FRAME_PAD_PC *pad);
    extern struct numtx_s *ed_remap_mtx;
    void edmainSetCamera(struct numtx_s *matrix);
    void edmainExtCamera(struct nucamera_s *camera);
    void edcamSet(void);
    void edcamMoveEx(struct FRAME_PAD_PC *pad, f32 delta_time);
#ifdef __cplusplus
}
#endif

#endif
