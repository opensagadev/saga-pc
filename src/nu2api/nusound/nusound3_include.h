#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
#ifdef __cplusplus
extern "C" {
#endif
    void PCStereoUnknownToken(...);
#ifdef __cplusplus
}
#endif
#else
#ifdef __cplusplus
extern "C" {
#endif
    void PCStereoUnknownToken(void);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    void NuSound3AddRumble(void *, f32, i32, f32, f32);
    void NuSound3ClearLoopHold(NUVEC *, i32);
#ifdef __cplusplus
}
#endif
