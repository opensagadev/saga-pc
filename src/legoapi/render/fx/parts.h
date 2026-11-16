#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

struct debris_key_s;
struct eduiitem_s;
struct eduimenu_s;

#ifdef __cplusplus
extern "C" {
#endif
    PART *FindPart(NUVEC *, i32, GAMEOBJECT *);
    extern char **PDebNameList;
    void AddVariableShotPARTEffect(i32, NUVEC *, f32, f32, NUMTX *);
    void CollectHitPoint(GAMEOBJECT *, NUVEC *, i32);
    void CollectPowerUp(GAMEOBJECT *, NUVEC *, u16, i32);
    void PowerUp_Particles(WORLDINFO *, NUVEC *);
    void KillParts(GAMEOBJECT *, i32, i32, i32, f32, i32, u16 *);
    void SetKillPartMom(NUVEC *);
    void PartImpact_Brick(PART *);
    void MakePartVector(NUVEC *velocity, NUVEC *direction, f32 scale);
    void Parts_Start(WORLDINFO *world);
    void Parts_Stop(WORLDINFO *world);
    void PartStop_Flickerer(PART *);
    i32 PartDraw_Flickerer(PART *);
    void PartCollide_3D(PART *);
    PART *Part_FindFromHSpecial(nuhspecial_s *);
    void PartKill_ForceThrow(PART *, i32);
    PART *AddPart(ADDPART *);
    PART *FindIncomingPart(void *owner, NUVEC *position, f32 radius, u32 flags, f32 range);
    extern ADDPART Default_ADDPART;
    PART *HitParts(GAMEOBJECT *, NUVEC *, i32, f32, NUVEC *, NUVEC *, u32);
    void AddVariableShotDebrisEffectTimed5(i32, NUVEC *, NUVEC *, NUVEC *, i32, f32, NUMTX *, NUMTX *, u16, i8);
    void SetPartRTLSet(RTLSET *rtl_set);
    void LoadPartFile(WORLDINFO *);
    i32 FindPartDebris(PARTDEBRISSYS *, char *);
    void AddPartDebris(PARTDEBRISSYS *, i32, NUVEC *);
    i32 edpartPlace(i32 index, NUVEC *position);
    void AddDebrisEffectToStack(struct debris_key_s *key);
    void DebrisSetup(VARIPTR *, VARIPTR, char *, i32, i32, i32);
    void InitPartTable(char **names);
    void ResetParts(void);
    extern i32 edpart_nearest_orphans;
    void edpartChangeNameMenu(struct eduimenu_s *menu, struct eduiitem_s *item, u32 value);
    void edpartCopyType(struct eduimenu_s *menu, struct eduiitem_s *item, u32 value);
    void edpartDebrisSettingsMenu(struct eduimenu_s *menu, struct eduiitem_s *item, u32 value);
    void edpartDeleteType(struct eduimenu_s *menu, struct eduiitem_s *item, u32 value);
#ifdef __cplusplus
}
#endif
