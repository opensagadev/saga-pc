#ifndef SAGA_LEGOAPI_ACTIONS_COMBAT_FIGHTING_H
#define SAGA_LEGOAPI_ACTIONS_COMBAT_FIGHTING_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void StartHold(GAMEOBJECT *);
    void Punch_Hit(GAMEOBJECT *, GAMEOBJECT *, f32, f32);
    extern i32 (*CanStartHoldFn)(GAMEOBJECT *);
    extern i32 (*Punch_GetDamageFn)(GAMEOBJECT *, GAMEOBJECT *);
    extern void (*Punch_HitExtraCodeFn)(GAMEOBJECT *, NUVEC *);
    extern void (*Punch_HitHoldFn)(GAMEOBJECT *, GAMEOBJECT *);
    extern const f64 PlayerStepInnerActionPc;
    extern const f64 PlayerStepOuterActionPc;
    extern const f64 PlayerStepInnerPc;
    extern const f64 PlayerStepOuterPc;
    void ComboHitFrame(GAMEOBJECT *, i32);
    void HoldCode(GAMEOBJECT *);
    i32 ObjZappedBlue(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif

#endif
