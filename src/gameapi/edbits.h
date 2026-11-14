/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edtools/edui.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct PC_EDBRIDGE { // size 0x44
        i32 instance_id;         // 0x00
        NUVEC position;          // 0x04
        f32 length;              // 0x10
        f32 unknown_14;          // 0x14
        i16 rotation_z;          // 0x18
        i16 rotation_y;          // 0x1a
        i8 connection_index;     // 0x1c
        i8 unknown_1d;           // 0x1d
        i8 unknown_1e;           // 0x1e
        u8 unknown_1f;           // 0x1f
        i32 special_20;          // 0x20
        i32 special_24;          // 0x24
        f32 unknown_28;          // 0x28
        f32 unknown_2c;          // 0x2c
        f32 unknown_30;          // 0x30
        f32 unknown_34;          // 0x34
        f32 unknown_38;          // 0x38
        f32 unknown_3c;          // 0x3c
        u8 red;                  // 0x40
        u8 green;                // 0x41
        u8 blue;                 // 0x42
        u8 unknown_43;           // 0x43
    } PC_EDBRIDGE;

    extern i32 edbri_bridges_used;
    extern i32 edbri_nearest;
    void edbriClearPage(i8 page);
    void edbitsRegisterBaseScene(NUGSCN *scene);
    void edbitsRegisterBaseTerrain(void *terrain);
    extern struct eduimenu_s *edbri_bridgeproperties_menu;
    extern struct eduimenu_s *edbri_dpadmode_menu;
    extern i32 edbri_mode;
    extern struct eduimenu_s *edbri_plankcount_menu;
    extern i32 edbri_planks;
    extern i32 edbri_post_interval;
    void edbricbCancelBridgePropertiesMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edbricbCancelDpadModeMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edbricbCancelPlankCountMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void edbricbSetDpadMode(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edbricbSetPlankCount(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edbricbSetPostInterval(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    extern struct eduimenu_s *edbri_active_menu;
    extern f32 edbri_length;
    extern struct eduimenu_s *edbri_options_menu;
    extern i32 edbri_plank_instance_type;
    extern i32 edbri_post_instance_type;
    extern i32 edbri_roty;
    extern i32 edbri_rotz;
    extern f32 edbri_width;
    extern struct PC_SCENE_MTL_VIEW *edbri_mtl_zoff;
    extern struct eduimenu_s *edbri_ropecolour_menu;
    void edbricbCancelRopeColourMenu(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    extern i32 edbri_cam_ax;
    extern i32 edbri_cam_ay;
    void edbricbFileSave(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edbricbPlankInstanceMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void edbricbPostInstanceMenu(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
#ifdef __cplusplus
}
#endif
