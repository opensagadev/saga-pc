#pragma once

#include "nu2api/nucore/common.h"

typedef struct EDAIPATHCHECK_s EDAIPATHCHECK_s;

struct EDAIPATHCHECK_s {           // size 0x1c
    i32 on_path;                   // 0x00
    struct EDAIPATH_s *path;       // 0x04
    struct EDAIPATHNODE_s *first;  // 0x08
    struct EDAIPATHNODE_s *second; // 0x0c
    f32 fraction;                  // 0x10
    f32 width;                     // 0x14
    i32 angle;                     // 0x18
};

#ifdef __cplusplus
extern "C" {
#endif
    extern void (*AIPathNodeDeletedFn)(struct EDAIPATHNODE_s *arg0);
#ifdef __cplusplus
}
#endif
