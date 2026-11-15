#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nufile/nufpar.h"

struct nualloc_manager_block_s;

#ifdef __cplusplus
extern "C" {
#endif
    void NuFreeFPar(NUFPAR *parser);
#ifdef __cplusplus
}
#endif

typedef struct nualloc_manager_trailer_s {          // size 0x4
    struct nualloc_manager_block_s *previous_block; // 0x0
} NUALLOC_MANAGER_TRAILER;
