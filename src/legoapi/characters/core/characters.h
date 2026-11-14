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

typedef struct GAME_CUSTOMISER_START { // size 0x38
    u16 pieces[9];                     // 0x00
    u8 reserved_12[2];                 // 0x12
    char name[32];                     // 0x14
    u8 use_saved_name;                 // 0x34
    u8 reserved_35[3];                 // 0x35
} GAME_CUSTOMISER_START;

typedef struct apicharactermodellist_s { // size 0x4
    i16 model_id;                        // 0x0
    i16 count;                           // 0x2
} APICHARACTERMODELLIST;

typedef struct KILLPARTS_CHARSCENE_PC { // size 0x10
    struct NUGSCN_s *scene;             // 0x0
    NUSPECIALHANDLE special_scene;      // 0x4
} KILLPARTS_CHARSCENE_PC;

#ifdef __cplusplus
extern "C" {
#endif
    void CharScene_Draw(struct WORLDINFO_s *, i32, struct numtx_s *, struct numtx_s *);
    NUSPECIALHANDLE *CharScene_FindHSpecial(struct WORLDINFO_s *, i32);
    extern KILLPARTS_CHARSCENE_PC *KillPartsCharSceneAreaPc;
    void HeadMovement(struct gameobject_s *);
    void HairMovement(struct gameobject_s *);
    void CharScenes_LevelDump(struct WORLDINFO_s *);

    typedef struct char_category_record_s { // size 0xc
        char *name;                         // 0x0
        u32 model_flags;                    // 0x4
        u32 game_flags;                     // 0x8
    } CHAR_CATEGORY_RECORD_PC;

    void CharScenes_Init(VARIPTR *, VARIPTR *);
    void CharScenes_AreaLoad(APICHARACTERMODELLIST *, VARIPTR *, VARIPTR);
    extern void *IconScene;
    extern struct KILLPARTS_CHARSCENE_PC *CharScene_Area;
#ifdef __cplusplus
}
#endif
