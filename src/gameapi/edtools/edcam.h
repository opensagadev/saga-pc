#pragma once

#include "nu2api/nucore/common.h"

struct FRAME_PAD;
struct edcam_s;
struct edui_interact_s;
struct nucamera_s;
struct nuvec_s;

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 PadFlyMode;
    void edcamSetPos(struct nuvec_s *position);
    void do_Pad_flymode_camera(struct edcam_s *camera, f32 delta_time, struct FRAME_PAD *pad);
    extern struct numtx_s *ed_remap_mtx;
    void edmainSetCamera(struct numtx_s *matrix);
    void edmainExtCamera(struct nucamera_s *camera);
    void edcamSet(void);
    void edcamMoveEx(struct FRAME_PAD *pad, f32 delta_time);
    struct nucamera_s *edmainGetCamera(void);
    f32 edcamGetDist(void);
    i32 edmainGetCursorEnabled(void);
    void edcamGetPosAng(struct nuvec_s *position, i32 *pitch, i32 *yaw);
    void edcamMove(struct FRAME_PAD *pad);
    void edcamSetDist(f32 distance);
    void edcamGetPosAngSnap(struct nuvec_s *position, i32 *angle_x, i32 *angle_y);
    void edcamSetAng(i32 pitch, i32 yaw);
    void edcamSetPosAng(struct nuvec_s *position, i32 pitch, i32 yaw);
    i32 cbInteractMenuCancelChild(struct edui_interact_s *);
    i32 cbInteractMenuScrollDown(struct edui_interact_s *);
    i32 cbInteractMenuScrollUp(struct edui_interact_s *);
    i32 cbInteractMenuTitle(struct edui_interact_s *);
#ifdef __cplusplus
}
#endif
