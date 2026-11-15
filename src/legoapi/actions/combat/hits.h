#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"

/* ObjHitObj flags (ObjHitObj_Flags, BOLT.hit_flags): who the hit comes from and what hits. */
#define OBJHIT_FROM_AI 0x2 /* the attacker is not an active player */
#define OBJHIT_FROM_PLAYER 0x4
#define OBJHIT_FROM_GOODIE 0x8
#define OBJHIT_FROM_BADDIE 0x10 /* APIOBJECT_SIDE_BADDIE or _GOODIE_BADDIE */
#define OBJHIT_FROM_NEUTRAL 0x20
#define OBJHIT_PUNCH 0x40        /* the fighting hit (Punch_Hit) */
#define OBJHIT_BOLT 0x80         /* every bolt's hit_flags */
#define OBJHIT_SABRE 0x100       /* the lightsabre's hit (blocked from the front, shields) */
#define OBJHIT_EXPLOSION 0x200   /* an explosion's hit */
#define OBJHIT_HITS_GIZMOS 0x800 /* a player's: bolts may hit and aim at gizmos (Bolt_HitPlatPrivate, gizgame) */

#ifdef __cplusplus
extern "C" {
#endif
    i32 ObjHitObj(GAMEOBJECT *, GAMEOBJECT *, i32, u16, i32, i32);
    i32 NoLayerKill(GAMEOBJECT *);
    u16 ObjHitObj_Flags(GAMEOBJECT *);
    i32 CannotKill(GAMEOBJECT *);
    void RegenerateHearts(GAMEOBJECT *);
#ifdef __cplusplus
}
#endif
