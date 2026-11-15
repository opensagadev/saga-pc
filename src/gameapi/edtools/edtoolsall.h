/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

struct eduiitem_s;
struct eduimenu_s;

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
    void eduiAddPropTextPickEnt(struct eduimenu_s *menu, struct eduiitem_s *item);
    void eduiSetCameraEnabled(i32 enabled);
    void eduiSetCursorColour(u32 colour);
    void eduiSetFontScale(f32 x, f32 y);
    extern i32 near_clip_at_cursor;
    void AddColourPick(eduimenu_s *menu, eduiitem_s *item, f32 *red, f32 *green, f32 *blue, u32 *rgba);
    void CreateColourPicker(void);
    void cbCancelSubMenuFromItem(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
    void cbNearClipAtCursor(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
    void cbTriggerSubMenu(eduimenu_s *menu, eduiitem_s *item, u32 buttons);
#ifdef __cplusplus
}
#endif
