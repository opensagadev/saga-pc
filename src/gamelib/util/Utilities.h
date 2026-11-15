#pragma once

#include "nu2api/nucore/common.h"

struct nuhspecial_s;

#ifdef __cplusplus
extern "C" {
#endif
    void *GameBufferAlloc(VARIPTR *buf, VARIPTR *end, i32 size);
    extern u32 LAYER_HOVERIGNORE;
    i32 getqseed(void);
    i32 StreamingInsideLineXZ(f32, f32, f32, f32, f32, f32);
#ifdef __cplusplus
}
#endif
f32 NuSpecialGetAnimPos(struct nuhspecial_s *special);
#ifdef __cplusplus

u32 UtilGetTime(void);
u32 UtilGetFrameStartTime(void);
void UtilFrameStart(void);
#endif
