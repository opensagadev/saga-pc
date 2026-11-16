#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/gui/apimenu.h"
#include "legogame/game.h"

struct CUSTOMPIECE_LAYERS;
struct FRAME_PAD;
struct WORLDINFO_s;
struct nufpar_s;

typedef struct CUSTOMISER_LAYERS {     // size 0x70
    u8 unknown_00[0x38];               // 0x00
    struct CUSTOMPIECE_LAYERS *pieces; // 0x38
    u8 unknown_3c[0x30];               // 0x3c
    i16 character_ids[2];              // 0x6c
} CUSTOMISER_LAYERS;

typedef struct GAMEPAD_RUNTIME_INPUT { // size 0x60
    struct FRAME_PAD *bound_pad;       // 0x00
    u32 held;                          // 0x04
    u32 pressed;                       // 0x08
    u32 left_held;                     // 0x0c
    u32 left_pressed;                  // 0x10
    u32 left_previous;                 // 0x14
    u32 right_held;                    // 0x18
    u32 right_pressed;                 // 0x1c
    u32 right_previous;                // 0x20
    u8 input_ready;                    // 0x24
    u8 unknown_25[0x60 - 0x25];        // 0x25
} GAMEPAD_RUNTIME_INPUT;

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 PCArcadeUnpairedIconModel;
    void PermanentCustomPieceConfig(struct CUSTOMPIECE_LAYERS *, struct nufpar_s *);
    i32 PermanentCustomPieceAvailable(struct CUSTOMPIECE_LAYERS *);
    extern f32 MissionPlayerIconSize;
    extern const char *TagAsciiDown;
    extern const char *InitAsciiUp;
    extern const char *InitAsciiLeft;
    extern i32 CustomiseMode[2];
    extern char CustomiseLetter[39];
    void Customiser_Update(CUSTOMISER_LAYERS *, struct WORLDINFO_s *);
    i32 Customiser_MenuAvailable(CUSTOMISER_LAYERS *);
    void Customiser_Update(CUSTOMISER_LAYERS *, struct WORLDINFO_s *);
    extern i32 customiser_save_done;
    extern i32 customiser_changed;
    extern GAMESAVE OldCustomiseGame; /* 85ea98, native0x7e4c owner. */
    void Customiser_Init(CUSTOMISER_LAYERS *);
    void Customiser_Reset(CUSTOMISER_LAYERS *);
    void Customiser_Draw3D(CUSTOMISER_LAYERS *);
    void MenuDrawCustomiser(MENU *);
    void MenuUpdateCustomiser(MENU *);
    i32 CustomiserMenu_Update(void);
    extern i16 dat_00880890;
    extern u8 downrepeatcount[2];
    extern u8 rightrepeatcount[2];
    void Customiser_SetUpCharacterData(CUSTOMISER_LAYERS *customiser);
#ifdef __cplusplus
}
#endif
