#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    typedef void (*TAG_DRAW_ICON)(GAMEOBJECT *);
    extern i32 Tag_Mode;
    extern TAG_DRAW_ICON Tag_DrawIconFn;
    extern f32 TakeOverATATJumpScale;
    extern f32 SpeederChaseATATExitSpeed;
    extern f32 SpeederChaseATATExitHeight;
    extern NUVEC SpeederChaseATATExitLandPos;
    extern i16 id_TROOPERCANNON;
    extern i16 id_HEAVYREPEATINGCANNON;
    extern i16 id_BOMARRMONK;
    extern i16 id_GYROCOPTER;
    extern i32 players_cannot_exit_speeder;
    extern const char TakeOverState_JustBeenTakenOver[18];
    extern const char TakeOverState_MapRunAway[11];
    i32 TagCharacter(GAMEOBJECT *, GAMEOBJECT *, i32);
    void ReleaseTakeOver(GAMEOBJECT *, i32);
    void TakeOverGameObject(struct gameobject_s *rider, struct gameobject_s *vehicle, i32 blend_camera, i32 immediate);
    void Tag_SetMode(i32 mode);
#ifdef __cplusplus
}
#endif
