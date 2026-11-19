#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 SpecialMove_ReleaseVictim(GAMEOBJECT *);
    i32 SpecialMove_GetAttackerAction(i32);
    i32 SpecialMove_GetVictimAction(i32);
    u32 SpecialMove_GetFlags(i32, u32);
    i32 SpecialMove_Check(GAMEOBJECT *, GAMEOBJECT *);
    extern i32 SpecialMoveCount;
    extern struct specialmove_s *SpecialMove;
    void SpecialMove_Cancel(GAMEOBJECT *);

    void SpecialMoves_Configure(char *, VARIPTR *, VARIPTR *);
    void SpecialMove_VictimCode(GAMEOBJECT *);
    void SpecialMovesConfigMissing(char *filename);
#ifdef __cplusplus
}
#endif
