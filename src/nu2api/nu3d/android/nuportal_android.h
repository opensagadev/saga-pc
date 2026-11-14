#ifndef SAGA_NU2API_NU3D_ANDROID_NUPORTAL_ANDROID_H
#define SAGA_NU2API_NU3D_ANDROID_NUPORTAL_ANDROID_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

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

#endif
