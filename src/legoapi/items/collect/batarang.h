#ifndef SAGA_LEGOAPI_ITEMS_COLLECT_BATARANG_H
#define SAGA_LEGOAPI_ITEMS_COLLECT_BATARANG_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "legoapi/world/world.h"
#include "legogame/batman_gameai.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 Batarang_StartTargetting(GAMEOBJECT *);
    i32 BatarangGetObjectFromCharIdPc(i32 character);
    void Batarangs_CheckLostData(void *);
    void Batarangs_Reset(void);

    i32 Batarang_FindTarget(WORLDINFO *, GAMEOBJECT *, i32);
    void Batarang_Release(GAMEOBJECT *, i32);
    i32 Batarang_InitRicochet(PLAYER_BATARANG_SLOT_PC *, NUVEC *);
    i32 Batarang_SeekToTarget(PLAYER_BATARANG_SLOT_PC *);
    void Batarang_MoveCode(GAMEOBJECT *);
    void Batarang_StartThrowQuick(GAMEOBJECT *);
    void Batarangs_Update(void);
    void Batarangs_Draw(void);
#ifdef __cplusplus
}
#endif

#endif
