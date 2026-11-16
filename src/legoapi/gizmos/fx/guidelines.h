#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

typedef struct guideline_record_s { // size 0x24
    char name[8];                   // 0x00
    NUVEC start_position;           // 0x08
    NUVEC end_position;             // 0x14

    union { // size 0x1
        u8 flags;

        struct {            // size 0x1
            u8 active : 1;  // 0x20:0
            u8 visible : 1; // 0x20:1
        };
    }; // 0x20

    u8 unknown_21[3]; // 0x21
} GUIDELINE_RECORD;

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
    void GuideLines_Draw(void *world, void *unused, f32 delta_time);
#ifdef __cplusplus
}
#endif
