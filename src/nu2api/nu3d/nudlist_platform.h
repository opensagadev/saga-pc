#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/numtx.h"

#ifdef __cplusplus
extern "C" {
#endif
    void *DisplayListCreateFaceonTransformPS(VARIPTR *, NUMTX *, NUMTL *, void *);
#ifdef __cplusplus
}
#endif
