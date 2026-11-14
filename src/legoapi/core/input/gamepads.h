#ifndef SAGA_LEGOAPI_CORE_INPUT_GAMEPADS_H
#define SAGA_LEGOAPI_CORE_INPUT_GAMEPADS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edtools/edui.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/misc/legoapi_status.h"
#include "legoapi/world/world.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edtools/edui.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/misc/legoapi_status.h"
#include "legoapi/world/world.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/edtools/edui.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/misc/legoapi_status.h"
#include "legoapi/world/world.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void KillRumble(GAMEOBJECT *);
    void HitRumble(GAMEOBJECT *);
    void TakeHitRumble(GAMEOBJECT *, f32);
    /* Existing shared extern C scope. No new fields/providers. */

    void FloatRumble(GAMEOBJECT *);
    void NewBuzzFrames(void *, i32, i32);
    void NewBuzzFrames(void *pad, i32 frames, i32 ignored);
    extern i32 readpads_always;
    void GamePads_Init(void);

    void NewRumble(void *, f32, i32);
    void NewBuzz(void *, f32, i32);
    f32 GamePad_Rotate(GAMEOBJECT *);
    i32 GamePad_Waggle(GAMEPAD_PC *);
    extern i32 GAMEPAD_DUP;
    extern i32 GAMEPAD_DDOWN;
    extern i32 GAMEPAD_DLEFT;
    extern i32 GAMEPAD_DRIGHT;
    extern i32 GAMEPAD_LIFT;
    u16 GamePad_InputAngle(GAMEOBJECT *, GAMEPAD_PC *);
    extern u32 GAMEPAD_JUMP;
    extern u32 GAMEPAD_ACTION;
    extern u32 GAMEPAD_SPECIAL;
    extern u32 GAMEPAD_TAG;
    extern void NewRumbleAllPlayers(f32 strength, f32 duration, i32 frames, i32 player_index);
    void ResetRumble(RUMBLE_PACKET_RESOURCE_PC *);
    GAMEPAD_PC *GamePad_Allocate(void);
    extern u32 GAMEPAD_TOGGLELEFT;
    extern u32 GAMEPAD_TOGGLERIGHT;
    extern u32 GAMEPAD_START;
    void ConstantRumble(GAMEOBJECT *, f32, f32);
    void UpdateRumble(RUMBLE_PACKET_RESOURCE_PC *);
    extern u32 GAMEPAD_MENUSELECT;
    extern u32 GAMEPAD_MENUCANCEL;
    extern u32 GAMEPAD_SELECT;

    /* Add within the existing public C linkage cohort. Retain the game-save
     * refinement from game-init-after-MM; it supplies the two existing name fields. */
    extern u32 GAMEPAD_SKIP;
    void GamePad_InitButtons(void);

    i32 ReadPad(i32 port);
    void ReadPads(void);
    extern FRAME_PAD_PC *pActivePad;
    void PadOutPause(i32 port, struct WORLDINFO_s *world);
    void DoInput(WORLDINFO *world);
    void PadOutPause(i32, WORLDINFO *);
#ifdef __cplusplus
}
#endif

#endif
