#ifndef SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTERS_H
#define SAGA_LEGOAPI_CHARACTERS_CORE_CHARACTERS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/base/apiobject.h, nu2api/nu3d/nu3d_includes.h (not all of it earlier
 * in the shim). */

#include "legoapi/world/world.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/numath/numtx.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "nu2api/nu3d/nu3d_includes.h"
#endif

typedef struct GAME_CUSTOMISER_START_PC {
    u16 pieces[9];
    u8 reserved_12[2];
    char name[32];
    u8 use_saved_name;
    u8 reserved_35[3];
} GAME_CUSTOMISER_START_PC;

STATIC_ASSERT(sizeof(GAME_CUSTOMISER_START_PC) == 56);
STATIC_ASSERT(offsetof(GAME_CUSTOMISER_START_PC, pieces) == 0);
STATIC_ASSERT(offsetof(GAME_CUSTOMISER_START_PC, name) == 0x14);
STATIC_ASSERT(offsetof(GAME_CUSTOMISER_START_PC, use_saved_name) == 0x34);

typedef struct apicharactermodellist_s {
    i16 model_id;
    i16 count;
} APICHARACTERMODELLIST;

STATIC_ASSERT(sizeof(APICHARACTERMODELLIST) == 4);
STATIC_ASSERT(offsetof(APICHARACTERMODELLIST, count) == 2);

typedef struct KILLPARTS_CHARSCENE_PC {
    NUGSCN *scene;
    NUSPECIALHANDLE special_scene;
} KILLPARTS_CHARSCENE_PC;

STATIC_ASSERT(sizeof(KILLPARTS_CHARSCENE_PC) == 0x10);
STATIC_ASSERT(offsetof(KILLPARTS_CHARSCENE_PC, special_scene) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    void CharScene_Draw(WORLDINFO *, i32, NUMTX *, NUMTX *);
    NUSPECIALHANDLE *CharScene_FindHSpecial(WORLDINFO *, i32);
    extern KILLPARTS_CHARSCENE_PC *KillPartsCharSceneAreaPc;
    void HeadMovement(GAMEOBJECT *);
    void HairMovement(GAMEOBJECT *);
    void CharScenes_LevelDump(WORLDINFO *);
    extern const char PCContextName_ZipUp[6];

    typedef struct char_category_record_pc_s {
        char *name;
        u32 model_flags;
        u32 game_flags;
    } CHAR_CATEGORY_RECORD_PC;

    STATIC_ASSERT(sizeof(CHAR_CATEGORY_RECORD_PC) == 12);
    STATIC_ASSERT(offsetof(CHAR_CATEGORY_RECORD_PC, name) == 0);
    STATIC_ASSERT(offsetof(CHAR_CATEGORY_RECORD_PC, model_flags) == 4);
    STATIC_ASSERT(offsetof(CHAR_CATEGORY_RECORD_PC, game_flags) == 8);
    void CharScenes_Init(VARIPTR *, VARIPTR *);
    void CharScenes_AreaLoad(APICHARACTERMODELLIST *, VARIPTR *, VARIPTR);
    extern void *IconScene;
    extern struct KILLPARTS_CHARSCENE_PC *CharScene_Area;
#ifdef __cplusplus
}
#endif

#endif
