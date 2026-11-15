/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/characters/core/character.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/numtl.h"
#endif

struct PC_CONTROL_CONFIG;

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 PermDataLoaded;
    extern volatile i32 LoadPerm_LanguageSelect;
    extern NUMTL *legal_mtl;
    extern i32 legal_tid;
    extern i32 loadlegal_done;
    extern i32 LoadPerm_StringsLoaded;
    extern NUGSCN *save_icon_scene;
    extern LEVELSPLINE_PC LevelSplinesTable[26];
    extern i32 LevelHackDataSize;
    extern i16 ID_DEFAULTCHARACTER_LSW_A;
    extern i16 ID_DEFAULTCHARACTER_LSW_B;
    void LoadPerm(void);
    extern API_EXTRA_ACTION PlayerExtraActionInfo[33];
    extern void *PlayerBlockActionInfo;
    f32 LoadPermFadeAlpha(f32 t, f32 duration);
    extern const f32 PermLegalScale;
    extern const char PermLegalSpanish[11];
    extern const char PermLegalFrench[10];
    extern const char PermLegalUS[4];
    extern const char PermLegalPcPrefix[];
    void LoadPermLoadLegalTexture(void);
    void StartPerm(void);
    void EndPerm(void);
    void LoadPermData(void *process);
    extern const char NuFileExt_at3[5];
    extern const char NuFileExt_psc[5];
    extern const char NuFileExt_phg[5];
    extern const char NuFileExt_nx2[5];
    extern const char NuFileExt_hx2[5];
    extern const char NuFileExt_h4m[5];
#ifdef __cplusplus
    void PcControlRegister(struct PC_CONTROL_CONFIG *, VARIPTR *, VARIPTR *);
    void LoadPermProgressDraw(f32, f32, f32);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern CHARFIXUP_PC CharacterFixupTable[219];

    extern TEXTENTRY Text_TableMenu[167];
    extern TEXTENTRY Text_TableGame[347];
    extern TEXTENTRY Text_TableMain[706];
    extern char *txt;
#ifdef __cplusplus
}
#endif

typedef struct bulk_ai_panel_s {
    u8 unknown_00[0x50];
    NUVEC position;
    u8 unknown_5c[5];
    u8 model_variant;
    u8 flags;
    u8 unknown_63;
    NUVEC floor_position;
} BULK_AI_PANEL;

typedef struct LIGHTING_LOAD_WORLD {
    u8 unknown_000[0x104];
    VARIPTR buffer;
    VARIPTR end_buffer;
    u8 unknown_10c[0x2aac - 0x10c];
    BURN_SET *burnset;
    u8 unknown_2ab0[4];
    RTL_SET *rtl_set;
} LIGHTING_LOAD_WORLD;
