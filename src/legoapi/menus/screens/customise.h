#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/gui/apimenu.h"
#include "legogame/game.h"

struct CUSTOMPIECE_LAYERS;
struct FRAME_PAD;
struct customiser_config_s;
struct WORLDINFO_s;
struct nufpar_s;

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
    void Customiser_Update(struct customiser_config_s *, struct WORLDINFO_s *);
    i32 Customiser_MenuAvailable(struct customiser_config_s *);
    void Customiser_Update(struct customiser_config_s *, struct WORLDINFO_s *);
    extern i32 customiser_save_done;
    extern i32 customiser_changed;
    extern GAMESAVE OldCustomiseGame; /* 85ea98, native0x7e4c owner. */
    void Customiser_Init(struct customiser_config_s *);
    void Customiser_Reset(struct customiser_config_s *);
    void Customiser_Draw3D(struct customiser_config_s *);
    void MenuDrawCustomiser(MENU *);
    void MenuUpdateCustomiser(MENU *);
    i32 CustomiserMenu_Update(void);
    extern i16 dat_00880890;
    extern u8 downrepeatcount[2];
    extern u8 rightrepeatcount[2];
    void Customiser_SetUpCharacterData(struct customiser_config_s *customiser);
#ifdef __cplusplus
}
#endif
