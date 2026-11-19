#pragma once

#include "nu2api/nucore/common.h"

#include "gameapi/gui/apimenu.h"
#include "legogame/game.h"

struct custom_piece_config_s;
struct FRAME_PAD;
struct customiser_config_s;
struct WORLDINFO_s;
struct nufpar_s;

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 PCArcadeUnpairedIconModel;
    void PermanentCustomPieceConfig(struct custom_piece_config_s *, struct nufpar_s *);
    i32 PermanentCustomPieceAvailable(struct custom_piece_config_s *);
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
    extern NUVEC CustomisePos[2];
    void Customiser_GetActiveWeirdoIndex(i32 *index, i32 *count);
#ifdef __cplusplus
}
#endif
