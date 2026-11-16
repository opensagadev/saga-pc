#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "legogame/batman_gameai.h"
#include "nu2api/numath/nuvec.h"

struct AISYS;

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
    extern AI_MID_SPECIAL_MOVE batman_midspecialmovefns[8];
    f32 NuFdiv(f32 a, f32 b);
    i32 Batman_MidSpecialMove_BigJump(struct AISYS *system, AIPACKET *packet, APIOBJECT *object);
    i32 Batman_MidSpecialMove_ClimbDown(struct AISYS *system, AIPACKET *packet, APIOBJECT *object);
    i32 Batman_MidSpecialMove_ClimbUp(struct AISYS *system, AIPACKET *packet, APIOBJECT *object);
    i32 Batman_MidSpecialMove_Default(struct AISYS *system, AIPACKET *packet, APIOBJECT *object);
    i32 Batman_MidSpecialMove_Glide(struct AISYS *system, AIPACKET *packet, APIOBJECT *object);
    i32 Batman_MidSpecialMove_Grapple(struct AISYS *system, AIPACKET *packet, APIOBJECT *object);
    i32 Batman_MidSpecialMove_SpecialUse(struct AISYS *system, AIPACKET *packet, APIOBJECT *object);
    i32 Batman_MidSpecialMove_StartJump(struct AISYS *system, AIPACKET *packet, APIOBJECT *object);
    i32 Batman_PrepareBigJump(AIPACKET *packet, APIOBJECT *apiobject, i32 checks_unused);
    i32 Batman_PrepareClimb(AIPACKET *packet, APIOBJECT *object, i32 checks);
    i32 Batman_PrepareGlide(AIPACKET *packet, APIOBJECT *object, i32 checks_unused);
    i32 Batman_PrepareGrapple(AIPACKET *packet, APIOBJECT *object, i32 checks_unused);
    i32 Batman_PrepareHoverTube(AIPACKET *packet, APIOBJECT *apiobject, i32 checks_unused);
    i32 Batman_PrepareJump(AIPACKET *packet, APIOBJECT *object, i32 checks);
    i32 Batman_PrepareObstacle(AIPACKET *packet, APIOBJECT *object, i32 checks_unused);
    i32 Batman_PrepareSpecialUse(AIPACKET *packet, APIOBJECT *object, i32 checks_unused);
    i32 Batman_PrepareTakeOverJump(AIPACKET *packet, APIOBJECT *object, i32 checks_unused);
    extern void Batarang_GetSightInfo(i32, i32 *, i32 *, i32 *, char *);
#ifdef __cplusplus
}
#endif
