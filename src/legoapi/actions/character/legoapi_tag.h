#ifndef SAGA_LEGOAPI_ACTIONS_CHARACTER_LEGOAPI_TAG_H
#define SAGA_LEGOAPI_ACTIONS_CHARACTER_LEGOAPI_TAG_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    typedef void (*TAG_DRAW_ICON_PC)(GAMEOBJECT *);
    extern i32 Tag_Mode;
    extern TAG_DRAW_ICON_PC Tag_DrawIconFn;
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
    extern const char TakeOverState_ReleasedTakeOver[17];
    i32 TagCharacter(GAMEOBJECT *, GAMEOBJECT *, i32);
    void ReleaseTakeOver(GAMEOBJECT *, i32);
    void TakeOverGameObject(struct gameobject_s *rider, struct gameobject_s *vehicle, i32 blend_camera, i32 immediate);
#ifdef __cplusplus
}
#endif

#endif
