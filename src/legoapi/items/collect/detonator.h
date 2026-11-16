#pragma once

#include "nu2api/nucore/common.h"

struct DETONATOR_s;

#ifdef __cplusplus
extern "C" {
#endif
    void Detonator_Detonate(struct DETONATOR_s *arg0);
    extern f32 BatarangDetonatorHitDelay;
    void Detonator_MoveCode(GAMEOBJECT *);
    void Detonators_Reset(void);
#ifdef __cplusplus
}
#endif
