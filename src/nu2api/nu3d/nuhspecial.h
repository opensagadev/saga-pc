/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/numtx.h"
#endif

struct nuhspecial_s;
typedef struct nuhspecial_s nuhspecial_s;

struct nuhspecial_s {                            // size 0xc
    struct NUGSCN_s *scene;                      // 0x0
    NUMTX *special;                              // 0x4
    struct nudisplay_special_s *display_special; // 0x8
};
