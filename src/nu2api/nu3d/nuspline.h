#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct NUGSCN_s;

typedef struct nugspline_s { // size 0xc
    i16 length;              // 0x0
    i16 pt_size;             // 0x2
    char *name;              // 0x4
    NUVEC *pts;              // 0x8
} NUGSPLINE;

#ifdef __cplusplus
extern "C" {
#endif
    NUGSPLINE *NuSplineFind(struct NUGSCN_s *, char *);
    i32 NuSplineFindAllBeg(struct NUGSCN_s *scene, char *name, NUGSPLINE **results, i32 capacity);
#ifdef __cplusplus
}
#endif
