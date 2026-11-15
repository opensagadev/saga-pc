#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nu3d_includes.h"

#ifdef __cplusplus
extern "C" {
#endif
    void NuDisplayListRecalculateMaterialStartsPC(NUDISPLAY_LIST *);
    void PCDisplayListCriticalSectionEnter(void);
    void PCDisplayListCriticalSectionLeave(void);
    void NuRndrInitStartupMaterials(void);
    void fun_005de1c0(NUMTL *material);
#ifdef __cplusplus
}
#endif
