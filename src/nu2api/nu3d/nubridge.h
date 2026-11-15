#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/nuvec.h"

struct nuhspecial_s;

#ifdef __cplusplus
extern "C" {
#endif
    i32 NuBridgeCreate(NUGSCN *, struct nuhspecial_s *, struct nuhspecial_s *, NUVEC *, NUVEC *, f32, i16, f32, f32, f32, f32, i32, f32,
                       f32, i32, u32);
    void NuBridgeInit(void);
    void NuBridgeRemove(i32);
    void NuBrdigeDrawRope(NUMTL *, NUVEC *, NUVEC *, i32, i32 *, i32);
#ifdef __cplusplus
}
#endif
