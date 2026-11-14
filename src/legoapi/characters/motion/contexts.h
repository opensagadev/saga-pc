#ifndef SAGA_LEGOAPI_CHARACTERS_MOTION_CONTEXTS_H
#define SAGA_LEGOAPI_CHARACTERS_MOTION_CONTEXTS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: nu2api/nucore/common.h (not all of it earlier in the shim). */

#include "nu2api/nucore/fixed_width.h"

#ifndef SAGA_SHIM
#include "nu2api/nucore/common.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 LEGOCONTEXT_HOLD;
    extern i32 LEGOCONTEXT_LAND_LUNGE;
    extern i32 LEGOCONTEXT_LAND_SLAM;
    extern i32 LEGOCONTEXT_WEAPONIN;
    extern i32 LEGOCONTEXT_WEAPONOUT;
    extern i32 LEGOCONTEXT_PUNCH;
    extern i32 LEGOCONTEXT_LAND_JUMP;
    extern const f32 PlayerPanelVariantOneX;
    extern const f32 PlayerPanelVariantOneZ;
    extern i32 LEGOCONTEXT_COMBO;
    extern i32 LEGOCONTEXT_PUSHSPINNER;
    extern i32 LEGOCONTEXT_DOOMED;
    extern i32 LEGOCONTEXT_BEENTAKENOVER;
    extern i32 LEGOCONTEXT_GETIN;
    extern i32 LEGOCONTEXT_EATEN;
    extern i32 LEGOCONTEXT_BUCK;
    extern i32 LEGOCONTEXT_JUMP;
    extern i32 LEGOCONTEXT_BIGJUMP;
    extern i32 LEGOCONTEXT_TUBE;
    extern i32 LEGOCONTEXT_GLIDE;
    extern i32 ZipUpLandCombatRollContextPc;
    extern i32 ZipUpWallShuffleContextPc;
    extern i32 LEGOCONTEXT_WHIP;
    extern i32 LEGOCONTEXT_SUPERCARRY;
    extern i32 LEGOCONTEXT_SPECIALMOVE_ATTACKER;
    extern i32 LEGOCONTEXT_SPECIALMOVE_VICTIM;
    extern i32 LEGOCONTEXT_BUILDIT;
    extern i32 LEGOCONTEXT_PUSHOBSTACLE;
    extern i32 BoltBlockContextPc;
    extern i32 LEGOCONTEXT_GRAPPLE;
    extern i32 JumpBackFlipContextPc;
    extern i32 LEGOCONTEXT_DOOMED;
    extern i32 LEGOCONTEXT_JUMP;
    extern i32 LEGOCONTEXT_LAND_COMBOJUMP;
    extern i32 LEGOCONTEXT_LAND_FLIP;
    extern i32 LEGOCONTEXT_LAND_JUMP2;
    extern i32 LEGOCONTEXT_LAND_JUMP;
    extern i32 LEGOCONTEXT_LAND_LUNGE;
    extern i32 LEGOCONTEXT_LAND_SLAM;
    extern i32 LEGOCONTEXT_WEAPONIN;
    extern i32 LEGOCONTEXT_WEAPONOUT;
    extern i32 ZipUpLandCombatRollContextPc;
    extern i32 LEGOCONTEXT_CLIMB;
    extern i32 LEGOCONTEXT_HANG;
    extern i32 LEGOCONTEXT_LEDGETERRAIN;

    extern i32 LEGOCONTEXT_PUSH;
    extern i32 LEGOCONTEXT_WALLJUMPWAIT;
    extern i32 LEGOCONTEXT_DROPIN;
    extern i32 LEGOCONTEXT_LAND_SPECIAL;
#ifdef __cplusplus
}
#endif

#endif
