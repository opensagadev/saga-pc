/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern i16 LEGOACT_FALLLAND;
    extern i16 LEGOACT_BACKPACKFALLLAND;
    extern i16 LegoActPunchBehindPc;
    extern i16 LEGOACT_EXTRA_LAND2;
    extern i16 BigJumpLandFallbackPc;
    extern i16 BigJumpLandSecondaryPc;
    extern i16 BigJumpLandPreferredPc;
    extern i16 LEGOACT_SUPERCARRY_WALK;
    extern i16 LEGOACT_BUCK;
    extern i16 LEGOACT_JUMP;
    extern i16 LEGOACT_FALL;
    extern i16 LEGOACT_COMBOJUMP;
    extern i16 LEGOACT_JUMP2;
    extern u16 BoltShootClassId0;
    extern u16 BoltShootClassId1;
    extern u16 BoltShootClassId2;
    extern i16 LEGOACT_HANG_MOVE;
    extern i16 BoltDeactivatedActionPc;
    extern u32 ExBlowupFlagsPc;
    extern i16 BigJumpLandFallbackPc;
    extern i16 BigJumpLandPreferredPc;
    extern i16 BigJumpLandSecondaryPc;
    extern i16 JumpBackFlipActionPc;
    extern i16 JumpSlamActionPc;
    extern i16 JumpBackFlipActionPc;
    extern i16 JumpSlamActionPc;
    extern i16 LEGOACT_BACKPACKFALLLAND;
    extern i16 LEGOACT_COMBATROLL_FALL;
    extern i16 LEGOACT_COMBATROLL_FIRE;
    extern i16 LEGOACT_COMBATROLL_JUMP;
    extern i16 LEGOACT_COMBATROLL_LAND;
    extern i16 LEGOACT_EXTRA_JUMP2;
    extern i16 LEGOACT_EXTRA_JUMP;
    extern i16 LEGOACT_EXTRA_LAND2;
    extern i16 LEGOACT_EXTRA_LAND;
    extern i16 LEGOACT_FALL;
    extern i16 LEGOACT_FALLLAND;
    extern i16 LEGOACT_FLIP;
    extern i16 LEGOACT_FLIPLAND;
    extern i16 LEGOACT_JUMP2;
    extern i16 LEGOACT_JUMP3;
    extern i16 LEGOACT_JUMP;
    extern i16 LEGOACT_LAND3;
    extern i16 LEGOACT_LUNGE;
    extern i16 LEGOACT_LUNGELAND;
    extern i16 LEGOACT_MAGNET_JUMP;
    extern i16 LEGOACT_SLAMLAND;
    extern i16 LEGOACT_IDLE;
    extern i16 LEGOACT_WALK;
    extern i16 LEGOACT_HANG_IDLE;
    extern i16 LEGOACT_CLIMB_IDLE;
    extern i16 LEGOACT_LEDGE_GRAB;
    extern i16 LEGOACT_LEDGE_IDLE;
    extern i16 LEGOACT_LEDGE_LEFT;
    extern i16 LEGOACT_LEDGE_RIGHT;
    extern i16 LEGOACT_GRAPPLE_IDLE;
    extern i16 LEGOACT_GRAPPLE_UP;
    extern i16 LEGOACT_GRAPPLE_DOWN;
    extern i16 LEGOACT_GRAPPLE_HANG;
    extern i16 LEGOACT_GLIDE;
    extern i16 LEGOACT_PUSH;
    extern i16 LEGOACT_SUPERPUSH_IDLE;
    extern i16 LEGOACT_SUPERPUSH_PUSH;
    extern i16 LEGOACT_SUPERPUSH_PULL;
    extern i16 LEGOACT_WALLJUMP_WAIT;

    /* Selected native state declaration inventory, not storage definitions.
     * Place inside canonical guarded C scope; sole-writer overlap/initializer gate applies. */
    extern i16 LEGOACT_BUILD;
    extern i16 LEGOACT_CLIMB_UP;
    extern i16 LEGOACT_CLIMB_DOWN;
    extern i16 LEGOACT_CLIMB_LEFT;
    extern i16 LEGOACT_CLIMB_RIGHT;
    extern i16 LEGOACT_WALLSHUFFLE_IDLE;
    extern i16 LEGOACT_WALLSHUFFLE_LEFT;
    extern i16 LEGOACT_WALLSHUFFLE_RIGHT;
    extern i16 LEGOACT_SUPERCARRY_PICKUP;
    extern i16 LEGOACT_SUPERCARRY_IDLE;
    extern i16 LEGOACT_SUPERCARRY_THROW;
    extern i16 LEGOACT_SUPERCARRY_PUTDOWN;
    extern i16 LEGOACT_SUPERCARRY_BASH;
    extern i16 LEGOACT_SUPERCARRY_JUMP;
    extern i16 LEGOACT_SUPERCARRY_LAND;
    extern i16 LEGOACT_SUPERCARRY_FALLLAND;
    extern i16 LEGOACT_MAGNET_WALK_METAL;
    extern i16 LEGOACT_MAGNET_TIPTOE;
    extern i16 LEGOACT_MAGNET_WALK;
    extern i16 LEGOACT_MAGNET_RUN;
    extern i16 LEGOACT_MAGNET_LAND;
    extern i16 LEGOACT_WHIP_CRACK;
    extern i16 LEGOACT_WHIP_GRAB;
    extern i16 LEGOACT_WHIP_BREAK;
    extern i16 LEGOACT_TEETER;
    extern i16 LEGOACT_WHIP_START;
#ifdef __cplusplus
}
#endif
