/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

struct nuhspecial_s;
typedef struct nuhspecial_s nuhspecial_s;

struct nuhspecial_s {
    struct NUGSCN_s *scene;
    void *special;
    struct nudisplay_special_pc_s *display_special;
};

STATIC_ASSERT(sizeof(struct nuhspecial_s) == 12);
STATIC_ASSERT(offsetof(struct nuhspecial_s, scene) == 0);
STATIC_ASSERT(offsetof(struct nuhspecial_s, special) == 4);
STATIC_ASSERT(offsetof(struct nuhspecial_s, display_special) == 8);
