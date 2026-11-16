#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 MosEisleyC_PastBarrier(GAMEOBJECT *);
    extern f32 VehicleLoopTime;
    extern i8 *DeathStarEscapeBNetPacket;
    extern const char DeathStarEscapeBSponge[7];
    extern const char DeathStarEscapeBLiftDown[10];
    extern const char DeathStarEscapeBLiftUp[8];
    i32 DeathStarShieldDown(void);
    void BlockadeRunnerB_Init(WORLDINFO *);
    void BlockadeRunnerB_Update(WORLDINFO *);
    void BlockadeRunnerC_Init(WORLDINFO *);
    void BlockadeRunnerD_Reset(WORLDINFO *);
    void BlockadeRunnerD_Update(WORLDINFO *);
    void DeathStarBattleC_AlwaysUpdate(WORLDINFO *);
    void DeathStarBattleDDraw(WORLDINFO *);
    void DeathStarBattleDInit(WORLDINFO *);
    void DeathStarBattleDReset(WORLDINFO *);
    void DeathStarBattleDUpdate(WORLDINFO *);
    void DeathStarEscapeA_Update(WORLDINFO *);
    void DeathStarEscapeB_AlwaysUpdate(WORLDINFO *);
    void DeathStarEscapeB_Draw(WORLDINFO *);
    void DeathStarEscapeC_Init(WORLDINFO *);
    void DeathStarEscapeC_Reset(WORLDINFO *);
    void DeathStarEscapeC_Update(WORLDINFO *);
    void DeathStarEscapeD_Update(WORLDINFO *);
    void DeathStarRescueB_AlwaysUpdate(WORLDINFO *);
    void DeathStarRescueB_Init(WORLDINFO *);
    void DeathStarRescueB_Update(WORLDINFO *);
    void DeathStarRescueC_AlwaysUpdate(WORLDINFO *);
    void MosEisleyB_Init(WORLDINFO *);
    void MosEisleyD_AlwaysUpdate(WORLDINFO *);
    void MosEisleyE_Init(WORLDINFO *);
    void MosEisleyE_Reset(WORLDINFO *);
    void MosEisleyE_Update(WORLDINFO *);
#ifdef __cplusplus
}
#endif
