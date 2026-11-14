#ifndef SAGA_GAMEAPI_EDBITS_H
#define SAGA_GAMEAPI_EDBITS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

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

    typedef struct PC_EDBRIDGE {
        i32 instance_id;
        NUVEC position;
        f32 length;
        f32 field_14;
        i16 rotation_z;
        i16 rotation_y;
        i8 connection_index;
        i8 field_1d;
        i8 field_1e;
        u8 field_1f;
        i32 special_20;
        i32 special_24;
        f32 field_28;
        f32 field_2c;
        f32 field_30;
        f32 field_34;
        f32 field_38;
        f32 field_3c;
        u8 red;
        u8 green;
        u8 blue;
        u8 field_43;
    } PC_EDBRIDGE;

    STATIC_ASSERT(sizeof(PC_EDBRIDGE) == 0x44);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, instance_id) == 0x0);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, position) == 0x4);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, length) == 0x10);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, field_14) == 0x14);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, rotation_z) == 0x18);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, rotation_y) == 0x1a);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, connection_index) == 0x1c);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, field_1d) == 0x1d);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, field_1e) == 0x1e);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, special_20) == 0x20);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, special_24) == 0x24);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, field_28) == 0x28);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, field_2c) == 0x2c);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, field_30) == 0x30);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, field_34) == 0x34);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, field_38) == 0x38);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, field_3c) == 0x3c);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, red) == 0x40);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, green) == 0x41);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, blue) == 0x42);
    STATIC_ASSERT(offsetof(PC_EDBRIDGE, field_43) == 0x43);
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

#endif
