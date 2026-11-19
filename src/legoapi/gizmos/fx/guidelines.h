#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

typedef struct guideline_record_s { // size 0x24
    char name[8];                   // 0x00
    NUVEC endpoints[2];             // 0x08  start and end

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

#ifdef __cplusplus
extern "C" {
#endif
    GUIDELINE_RECORD *GuideLine_FindNearest(NUVEC *position, struct WORLDINFO_s *world, i32 *endpoint, f32 *distance);
    void GuideLines_Draw(void *world, void *unused, f32 delta_time);
    struct addgizmotype_s *GuideLines_RegisterGizmo(i32);
#ifdef __cplusplus
}
#endif
