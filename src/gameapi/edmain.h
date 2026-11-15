#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

struct eduiitem_s;
struct eduimenu_s;

#ifdef __cplusplus
extern "C" {
#endif
    NUVEC *edmainQueryLocVec(void);
    void cbMMCancel(struct eduimenu_s *arg0, struct eduimenu_s *arg1);
    void cbMMRegSel(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
    void cbMMReturnToApp(struct eduimenu_s *arg0, struct eduiitem_s *arg1, u32 arg2);
#ifdef __cplusplus
}
#endif
