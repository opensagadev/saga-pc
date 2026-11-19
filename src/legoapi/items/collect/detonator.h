#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"

struct DETONATOR_s;

#ifdef __cplusplus
extern "C" {
#endif
    void Detonator_Detonate(struct DETONATOR_s *arg0);
    extern f32 BatarangDetonatorHitDelay;
    void Detonator_MoveCode(GAMEOBJECT *);
    void Detonators_Reset(void);
    void Detonators_Draw(void);
    void Detonators_Update(void);
#ifdef __cplusplus
}
#endif
