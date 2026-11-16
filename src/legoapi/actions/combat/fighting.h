#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    void StartHold(GAMEOBJECT *);
    void Punch_Hit(GAMEOBJECT *, GAMEOBJECT *, f32, f32);
    extern i32 (*CanStartHoldFn)(GAMEOBJECT *);
    extern i32 (*Punch_GetDamageFn)(GAMEOBJECT *, GAMEOBJECT *);
    extern void (*Punch_HitExtraCodeFn)(GAMEOBJECT *, NUVEC *);
    extern void (*Punch_HitHoldFn)(GAMEOBJECT *, GAMEOBJECT *);
    extern const f64 PlayerStepInnerAction;
    extern const f64 PlayerStepOuterAction;
    void ComboHitFrame(GAMEOBJECT *, i32);
    void HoldCode(GAMEOBJECT *);
    i32 ObjZappedBlue(GAMEOBJECT *);
    void KeepWeaponIn(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif
