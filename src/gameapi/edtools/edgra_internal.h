#ifndef SAGA_GAMEAPI_EDTOOLS_EDGRA_INTERNAL_H
#define SAGA_GAMEAPI_EDTOOLS_EDGRA_INTERNAL_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edptl.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edptl.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edptl.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 edgra_page_used[8];
    extern NUGSCN *edgra_page_scene[8];
    extern void *edgra_page_terrain[8];
    extern NUMTX *edgra_page_matrix_stack[8];
    extern i32 edgra_page_vectors_valid[8];
    extern i32 edgra_nearest;
    extern NUVEC edgra_cam_pos;
    extern NUMTX *edgra_mtxbuffer;
    extern NUVEC *edgra_vecbuffer;
    extern void *edgra_free_vecbuffer;
    extern i32 EDGRA_MAX_CLUMPS;
    extern i32 EDGRA_MAX_INDIVIDUAL_CLUMPS;
    extern i32 EDGRA_MAX_UNITS_PER_INDIVIDUAL_CLUMP;
    extern PC_EDGRACLUMP *GrassClumps;
    extern PC_EDGRAINDIVIDUAL *IndGrassClumps;
    extern i32 *IndGrassClumpsUsed;
    extern i32 edgra_ind_clumps_used;
    extern i32 edgra_clumps_used;
    extern i32 edgra_last_clump_in_buffer;
    extern f32 edgra_global_fadein;
    extern f32 edgra_global_fadeout;
    void edgraDetermineNearestClump(f32 distance);
    extern i32 edgra_page_on[8];
    extern i32 edgra_units_used;
    extern i32 edgra_page_on[8];
    extern i32 edgra_page_calculate_done[8];
    extern i32 edgra_dpadmode;
    extern struct eduimenu_s *edgra_dpadmode_menu;
    extern i32 edgra_filter;
    extern char edgra_filter_string[16];
    extern struct eduimenu_s *edgra_instance_menu;
    extern i32 edgra_instance_type;
    extern struct eduimenu_s *edgra_active_menu;
    extern struct eduimenu_s *edgra_clumparea_menu;
    extern struct eduimenu_s *edgra_clumpfade_menu;
    extern struct eduimenu_s *edgra_clumpmode_menu;
    extern struct eduimenu_s *edgra_clumpproperties_menu;
    extern i32 edgra_editormode;
    extern struct eduimenu_s *edgra_globals_menu;
    extern struct edui_slider_s *fadeinitem;
    extern struct edui_slider_s *fadeoutitem;
    extern struct edui_slider_s *globalfadeinitem;
    extern struct edui_slider_s *globalfadeoutitem;
    extern i32 edgra_cam_ax;
    extern i32 edgra_cam_ay;
    extern i32 edgra_clump_size;
    extern struct eduimenu_s *edgra_clumpdist_menu;
    extern struct eduimenu_s *edgra_clumpterrain_menu;
    extern i32 edgra_nearest_instance;
    extern struct eduimenu_s *edgra_options_menu;
    extern i32 edgra_roty;
    extern i32 edgra_rotz;
    extern f32 edgra_size;
    extern struct eduimenu_s *edgra_clumpsizes_menu;
    extern struct eduimenu_s *edgra_changeinstance_menu;
    extern struct PC_SCENE_MTL_VIEW *edgra_mtl;
#ifdef __cplusplus
}
#endif

#endif
