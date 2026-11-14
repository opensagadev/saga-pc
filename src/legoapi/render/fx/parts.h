#ifndef SAGA_LEGOAPI_RENDER_FX_PARTS_H
#define SAGA_LEGOAPI_RENDER_FX_PARTS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    PART_PC *FindPart(NUVEC *, i32, GAMEOBJECT *);
    extern char **PDebNameList;
    void AddVariableShotPARTEffect(i32, NUVEC *, f32, f32, NUMTX *);
    void CollectHitPoint(GAMEOBJECT *, NUVEC *, i32);
    void CollectPowerUp(GAMEOBJECT *, NUVEC *, u16, i32);
    void PowerUp_Particles(WORLDINFO *, NUVEC *);
    void KillParts(GAMEOBJECT *, i32, i32, i32, f32, i32, u16 *);
    void SetKillPartMom(NUVEC *);
    void PartImpact_Brick(PART_PC *);
    void MakePartVector(NUVEC *velocity, NUVEC *direction, f32 scale);
    void Parts_Start(WORLDINFO *world);
    void Parts_Stop(WORLDINFO *world);
    void PartStop_Flickerer(PART_PC *);
    i32 PartDraw_Flickerer(PART_PC *);
    void PartCollide_3D(PART_PC *);
    PART_PC *Part_FindFromHSpecial(NUSPECIALHANDLE *);
    void PartKill_ForceThrow(PART_PC *, i32);
    PART_PC *AddPart(ADDPART_PC *);
    PART_PC *FindIncomingPart(void *owner, NUVEC *position, f32 radius, u32 flags, f32 range);
    extern ADDPART_PC Default_ADDPART;
    PART_PC *HitParts(GAMEOBJECT *, NUVEC *, i32, f32, NUVEC *, NUVEC *, u32);
    void AddVariableShotDebrisEffectTimed5(i32, NUVEC *, NUVEC *, NUVEC *, i32, f32, NUMTX *, NUMTX *, u16, i8);
    void SetPartRTLSet(RTLSET_PC *rtl_set);
    void LoadPartFile(WORLDINFO *);
    i32 FindPartDebris(PARTDEBRISSYS *, char *);
    void AddPartDebris(PARTDEBRISSYS *, i32, NUVEC *);
#ifdef __cplusplus
}
#endif

#endif
