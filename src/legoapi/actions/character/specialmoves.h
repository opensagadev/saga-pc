#ifndef SAGA_LEGOAPI_ACTIONS_CHARACTER_SPECIALMOVES_H
#define SAGA_LEGOAPI_ACTIONS_CHARACTER_SPECIALMOVES_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "legoapi/actions/character/specialmoves_types.h"

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

#endif
