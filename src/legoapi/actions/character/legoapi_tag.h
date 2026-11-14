#ifndef SAGA_LEGOAPI_ACTIONS_CHARACTER_LEGOAPI_TAG_H
#define SAGA_LEGOAPI_ACTIONS_CHARACTER_LEGOAPI_TAG_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
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

#ifndef TYPES_OFF_1
typedef union TAGSCRIPTSTATE_PC {
    u32 words[0x33]; // 0x00

    struct {                  // 0x00
        u32 unknown000[0x2c]; // 0x00
        u8 creature_set;      // 0xb0
    } fields;
} TAGSCRIPTSTATE_PC; // size 0xcc

/* Bitfield view of flags_1fc: bit 7 marks a player-controlled object, bit 8 an AI-driven one. */
typedef struct TAGFLAGS1FC_PC {
    u32 bits_0_6 : 7;
    u32 player : 1;
    u32 ai : 1;
    u32 bits_9_31 : 23;
} TAGFLAGS1FC_PC;

#endif /* TYPES_OFF_1 */
#endif
