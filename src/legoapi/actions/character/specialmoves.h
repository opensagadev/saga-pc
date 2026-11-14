/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 SpecialMove_ReleaseVictim(GAMEOBJECT *);
    i32 SpecialMove_GetAttackerAction(i32);
    i32 SpecialMove_GetVictimAction(i32);
    u32 SpecialMove_GetFlags(i32, u32);
    i32 SpecialMove_Check(GAMEOBJECT *, GAMEOBJECT *);
    extern i32 SpecialMoveCount;
    extern struct specialmove_pc_s *SpecialMove;
    void SpecialMove_Cancel(GAMEOBJECT *);

    void SpecialMoves_Configure(char *, VARIPTR *, VARIPTR *);
    void SpecialMove_VictimCode(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif
