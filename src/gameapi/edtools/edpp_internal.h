#ifndef SAGA_GAMEAPI_EDTOOLS_EDPP_INTERNAL_H
#define SAGA_GAMEAPI_EDTOOLS_EDPP_INTERNAL_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 edpp_page_used[8];
    extern i32 edpp_page_on[8];
    extern i32 edpp_instances_used;
    extern i32 edpp_types_used;
    extern EDPP_PARTICLE_PC edpp_ptls[512];
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

#endif
