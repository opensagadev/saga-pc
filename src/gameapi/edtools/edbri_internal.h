#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/edbits.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern PC_EDBRIDGE edBridges[64];
    extern i32 edbri_page_used[8];
    void edbriStartPage(i32 page);
    void edbriStartAllPages(void);
#ifdef __cplusplus
}
#endif
