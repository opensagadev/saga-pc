#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct WORLDINFO_s;

typedef struct indy_guideline_s { // size 0x24
    char name[8];                 // 0x00
    NUVEC endpoints[2];           // 0x08
    u32 flags;                    // 0x20
} INDY_GUIDELINE;

#ifdef __cplusplus
extern "C" {
#endif
    INDY_GUIDELINE *GuideLine_FindNearest(NUVEC *position, struct WORLDINFO_s *world, i32 *endpoint, f32 *distance);
#ifdef __cplusplus
}
#endif
