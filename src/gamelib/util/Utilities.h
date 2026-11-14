#ifndef SAGA_GAMELIB_UTIL_UTILITIES_H
#define SAGA_GAMELIB_UTIL_UTILITIES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nuhspecial.h"

#ifdef __cplusplus
extern "C" {
#endif
    void *GameBufferAlloc(VARIPTR *buf, VARIPTR *end, i32 size);
    extern u32 LAYER_HOVERIGNORE;
    i32 getqseed(void);
    i32 StreamingInsideLineXZPc(f32, f32, f32, f32, f32, f32);
#ifdef __cplusplus
}
#endif
f32 NuSpecialGetAnimPos(struct nuhspecial_s *special);
#ifdef __cplusplus

u32 UtilGetTime(void);
u32 UtilGetFrameStartTime(void);
void UtilFrameStart(void);
#endif

#endif
