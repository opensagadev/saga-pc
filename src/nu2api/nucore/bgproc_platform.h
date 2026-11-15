#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    void bgprocFreeze(void);
    void bgprocUnFreeze(void);
    i32 bgprocIsFreezing(void);
#ifdef __cplusplus
}
#endif
