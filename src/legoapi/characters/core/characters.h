/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/nu3d/nu3d_includes.h"
#endif

struct NUGSCN_s;
struct WORLDINFO_s;
struct gameobject_s;
struct numtx_s;

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
    struct NUGSCN_s *scene;
    NUSPECIALHANDLE special_scene;
} KILLPARTS_CHARSCENE_PC;

STATIC_ASSERT(sizeof(KILLPARTS_CHARSCENE_PC) == 0x10);
STATIC_ASSERT(offsetof(KILLPARTS_CHARSCENE_PC, special_scene) == 4);
#ifdef __cplusplus
extern "C" {
#endif
    void CharScene_Draw(struct WORLDINFO_s *, i32, struct numtx_s *, struct numtx_s *);
    NUSPECIALHANDLE *CharScene_FindHSpecial(struct WORLDINFO_s *, i32);
    extern KILLPARTS_CHARSCENE_PC *KillPartsCharSceneAreaPc;
    void HeadMovement(struct gameobject_s *);
    void HairMovement(struct gameobject_s *);
    void CharScenes_LevelDump(struct WORLDINFO_s *);

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
