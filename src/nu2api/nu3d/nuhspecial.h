#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/numtx.h"

struct nuhspecial_s;
typedef struct nuhspecial_s nuhspecial_s;

struct nuhspecial_s {                            // size 0xc
    struct NUGSCN_s *scene;                      // 0x0
    NUMTX *special;                              // 0x4
    struct nudisplay_special_s *display_special; // 0x8
};
