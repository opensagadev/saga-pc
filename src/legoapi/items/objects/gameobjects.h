#ifndef SAGA_LEGOAPI_ITEMS_OBJECTS_GAMEOBJECTS_H
#define SAGA_LEGOAPI_ITEMS_OBJECTS_GAMEOBJECTS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aipath.h"
#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aipath.h"
#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/ai/aisys/aipath.h"
#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    GAMEOBJECT *FindNearestGameObject(NUVEC *, GAMEOBJECT *, u32, f32, f32, i32, i32, i32, f32 *, i32,
                                      i32 (*)(GAMEOBJECT *));
    i32 GameObjectNearFloor(GAMEOBJECT *, f32, f32 *);

    extern GAMEOBJECT *CarWashHack;
    extern f32 player_tag_timer;
    extern i32 do_player_tag;
    void SetToLastSafePos(GAMEOBJECT *);
    void RemoveGameObject(GAMEOBJECT *, i32);
    extern u16 Hub_PadAngle[2];
    void GameObjectDimensions(GAMEOBJECT *);
    i32 UnderPlayerControl(GAMEOBJECT *);
    GAMEOBJECT *FindGameObject(i32, u32, i32, i32, i32);
    void ScaleGameObject(GAMEOBJECT *);
    void GetTopBot(GAMEOBJECT *);
    void SnapCreaturePos(GAMEOBJECT *, NUVEC *, i32, AIPATHINFO_PC *, i32);
    void GameObjectOrigin(GAMEOBJECT *);
    extern i32 timebase_updates;
    extern i32 addcreature_override_id_check;
    extern f32 default_mover_extra;
    extern u8 grapple_attach_frames;
    void GameObjectSetCanUse(GAMEOBJECT *, void *, u8, u8, f32);
#ifdef __cplusplus
}
#endif

#endif
