#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "legogame/batman_gameai.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 Batarang_StartTargetting(GAMEOBJECT *);
    i32 BatarangGetObjectFromCharId(i32 character);
    void Batarangs_CheckLostData(void *);
    void Batarangs_Reset(void);

    i32 Batarang_FindTarget(WORLDINFO *, GAMEOBJECT *, i32);
    void Batarang_Release(GAMEOBJECT *, i32);
    i32 Batarang_InitRicochet(PLAYER_BATARANG_SLOT *, NUVEC *);
    i32 Batarang_SeekToTarget(PLAYER_BATARANG_SLOT *);
    void Batarang_MoveCode(GAMEOBJECT *);
    void Batarang_StartThrowQuick(GAMEOBJECT *);
    void Batarangs_Update(void);
    void Batarangs_Draw(void);
#ifdef __cplusplus
}
#endif
