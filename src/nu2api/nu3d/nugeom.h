#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/numtl.h"

typedef struct nugeom_s {  // size 0x8
    struct nugeom_s *next; // 0x0
    NUMTL *mtl;            // 0x4
} NUGEOM;
