#ifndef SAGA_GAMEAPI_EDTOOLS_EDTOOLSALL_H
#define SAGA_GAMEAPI_EDTOOLS_EDTOOLSALL_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edtools/edui.h"
#include "legoapi/legoapi_types.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edtools/edui.h"
#include "legoapi/legoapi_types.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edtools/edui.h"
#include "legoapi/legoapi_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void FS_GetDirList(char *path, char *filter, char *filter_out);
    f32 FS_GetDirTextWidth(void);
    i32 FS_PrevNameLen(char *name);
    void eduiItemColourPickSetHSV(struct edui_colour_pick_s *item, f32 hue, f32 saturation, f32 value);
    void eduicbCancelMessageMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void do_Pad_Standard_camera(struct edcam_s *camera, f32 delta_time, struct FRAME_PAD_PC *pad);
    void do_mouse_flymode_camera(struct edcam_s *camera, f32 delta_time);
    void cbCancelSubMenu(struct eduimenu_s *menu, struct eduimenu_s *arg1);
    void cbSetAutoSpeed(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    i32 eduicbInteractSlider(struct edui_interact_s *arg0);
    void eduicbItemDestroy(struct eduimenu_s *arg0, struct eduiitem_s *arg1);
    void do_maya_mouse_camera(struct edcam_s *camera);
    void eduiItemColourPickSetRGB(struct edui_colour_pick_s *arg0, f32 arg1, f32 arg2, f32 arg3);
    void FS_BuildFilterBlocks(char *arg0);
    void FS_BuildFilterOutBlocks(char *arg0);
    extern i32 ui_bgmtl;
#ifdef __cplusplus
}
#endif

#endif
