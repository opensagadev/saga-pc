#ifndef SAGA_LEGOAPI_MENUS_SCREENS_CUSTOMISE_H
#define SAGA_LEGOAPI_MENUS_SCREENS_CUSTOMISE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: gameapi/edtools/edui.h, legoapi/characters/core/customiser.h (not all of it earlier
 * in the shim). */

#include "gameapi/gui/apimenu.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/nufile/nufpar.h"

#ifndef SAGA_SHIM
#include "gameapi/edtools/edui.h"
#include "legoapi/characters/core/customiser.h"
#endif

typedef struct CUSTOMISER_LAYERS_PC {
    u8 unknown_00[0x38];
    CUSTOMPIECE_LAYERS_PC *pieces;
    u8 unknown_3c[0x30];
    i16 character_ids[2];
} CUSTOMISER_LAYERS_PC;

STATIC_ASSERT(offsetof(CUSTOMISER_LAYERS_PC, pieces) == 0x38);
STATIC_ASSERT(offsetof(CUSTOMISER_LAYERS_PC, character_ids) == 0x6c);

typedef struct GAMEPAD_RUNTIME_INPUT_PC {
    FRAME_PAD_PC *bound_pad;
    u32 held;
    u32 pressed;
    u32 left_held;
    u32 left_pressed;
    u32 left_previous;
    u32 right_held;
    u32 right_pressed;
    u32 right_previous;
    u8 input_ready;
    u8 unknown25[0x60 - 0x25];
} GAMEPAD_RUNTIME_INPUT_PC;

STATIC_ASSERT(sizeof(GAMEPAD_RUNTIME_INPUT_PC) == 0x60);
STATIC_ASSERT(offsetof(GAMEPAD_RUNTIME_INPUT_PC, bound_pad) == 0);
STATIC_ASSERT(offsetof(GAMEPAD_RUNTIME_INPUT_PC, held) == 4);
STATIC_ASSERT(offsetof(GAMEPAD_RUNTIME_INPUT_PC, pressed) == 8);
STATIC_ASSERT(offsetof(GAMEPAD_RUNTIME_INPUT_PC, left_held) == 0xc);
STATIC_ASSERT(offsetof(GAMEPAD_RUNTIME_INPUT_PC, left_pressed) == 0x10);
STATIC_ASSERT(offsetof(GAMEPAD_RUNTIME_INPUT_PC, left_previous) == 0x14);
STATIC_ASSERT(offsetof(GAMEPAD_RUNTIME_INPUT_PC, right_held) == 0x18);
STATIC_ASSERT(offsetof(GAMEPAD_RUNTIME_INPUT_PC, right_pressed) == 0x1c);
STATIC_ASSERT(offsetof(GAMEPAD_RUNTIME_INPUT_PC, right_previous) == 0x20);
STATIC_ASSERT(offsetof(GAMEPAD_RUNTIME_INPUT_PC, input_ready) == 0x24);
#ifdef __cplusplus
extern "C" {
#endif
    extern i32 PCArcadeUnpairedIconModel;
    void PermanentCustomPieceConfigPc(CUSTOMPIECE_LAYERS_PC *, NUFPAR_PC *);
    i32 PermanentCustomPieceAvailablePc(CUSTOMPIECE_LAYERS_PC *);
    extern f32 MissionPlayerIconSizePc;
    extern const char *TagAsciiDownPc;
    extern const char *InitAsciiUpPc;
    extern const char *InitAsciiLeftPc;
    extern i32 CustomiseMode[2];
    extern char CustomiseLetter[39];
    extern const char GameAudioName00753354Pc[11];
    void Customiser_Update(CUSTOMISER_LAYERS_PC *, WORLDINFO *);
    i32 Customiser_MenuAvailable(CUSTOMISER_LAYERS_PC *);
    void Customiser_Update(CUSTOMISER_LAYERS_PC *, WORLDINFO *);
    extern i32 customiser_save_done;
    extern i32 customiser_changed;
    extern GAMESAVE OldCustomiseGame; /* 85ea98, native0x7e4c owner. */
    void Customiser_Init(CUSTOMISER_LAYERS_PC *);
    void Customiser_Reset(CUSTOMISER_LAYERS_PC *);
    void Customiser_Draw3D(CUSTOMISER_LAYERS_PC *);
    void MenuDrawCustomiserPc(MENU *);
    void MenuUpdateCustomiserPc(MENU *);
    i32 CustomiserMenu_Update(void);
    extern i16 TextIdPc880890;
    extern u8 downrepeatcount[2];
    extern u8 rightrepeatcount[2];
#ifdef __cplusplus
}
#endif

#endif
