#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"

struct eduimenu_s;

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 edpp_page_used[8];
    extern struct eduimenu_s *ptlemitvelmenu;
    extern struct eduimenu_s *ptlgravmenu;
    extern struct eduimenu_s *ptlvaremitmenu;
    extern struct eduimenu_s *ptlstartvelmenu;
    extern struct eduimenu_s *ptlrotmenu;
    extern struct eduimenu_s *ptlsizemenu;
    extern struct eduimenu_s *ptlcolmenu;
    extern struct eduimenu_s *emittimemenu;
    extern i32 edpp_page_on[8];
    extern i32 edpp_instances_used;
    extern i32 edpp_types_used;
    extern EDPP_PARTICLE edpp_ptls[512];
    extern i32 edpp_nearest;
    extern NUVEC edpp_cam_pos;
    extern i32 edpp_emitrotx;
    extern i32 edpp_emitroty;
    extern i32 edpp_emitrotz;
    extern i32 edpp_facrotx;
    extern i32 edpp_facroty;
    extern i32 edpp_refroty;
    extern i32 edpp_refrotz;
    extern i32 edpp_roty;
    extern i32 edpp_rotz;
    extern i32 edpp_create_type;
    extern u8 edpp_effect_list;
    extern i32 edpp_num_orphans;
    extern f32 edpp_copy_size;
    extern i32 edpp_dpad_mode;
    extern struct eduimenu_s *edptl_star_menu;
    extern i32 edptl_superscale;
    extern i32 edptl_clipboard_entry;
#ifdef __cplusplus
}
#endif
