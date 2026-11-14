#ifndef SAGA_LEGOGAME_STARTUP_H
#define SAGA_LEGOGAME_STARTUP_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/apitext.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/numtl.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/apitext.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/numtl.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "gameapi/apitext.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/numtl.h"
#endif

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
    extern API_EXTRA_ACTION_PC PlayerExtraActionInfoPc[33];
    extern void *PlayerBlockActionInfoPc;
    f32 LoadPermFadeAlpha(f32 t, f32 duration);
    extern const f32 PermLegalScale;
    extern const char PermLegalBase[6];
    extern const char PermLegalSeparator[2];
    extern const char PermLegalSpanish[11];
    extern const char PermLegalFrench[10];
    extern const char PermLegalUS[4];
    extern const char PermLegalDirectoryPc[];
    extern const char PermLegalPcPrefixPc[];
    void LoadPermLoadLegalTexture(void);
    void StartPerm(void);
    void EndPerm(void);
    void LoadPermData(void *process);
    extern const char NuFileExt_dds_PC[5];
    extern const char NuFileExt_at3_PC[5];
    extern const char NuFileExt_psc_PC[5];
    extern const char NuFileExt_phg_PC[5];
    extern const char NuFileExt_nx2_PC[5];
    extern const char NuFileExt_hx2_PC[5];
    extern const char NuFileExt_h4m_PC[5];
#ifdef __cplusplus
    void PcControlRegisterPc(PC_CONTROL_CONFIG_PC *, VARIPTR *, VARIPTR *);
    void LoadPermProgressDraw(f32, f32, f32);
#ifdef __cplusplus
}
#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif
    extern CHARFIXUP_PC CharacterFixupTablePc[219];

    extern TEXTENTRY Text_TableMenu[167];
    extern TEXTENTRY Text_TableGame[347];
    extern TEXTENTRY Text_TableMain[706];
    extern char *txt;
#ifdef __cplusplus
}
#endif

#endif
