#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nugscn.h"

#ifdef __cplusplus
extern "C" {
#endif
    void Initialise_PS(NUGSCN *);
    void SetAllInstancesVisible(NUGSCN *);
    void SetAllInstancesHidden(NUGSCN *);
#ifdef __cplusplus
}
#endif
