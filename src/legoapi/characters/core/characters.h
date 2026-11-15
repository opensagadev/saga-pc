#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/nu3d/nuhspecial.h"

struct nuhspecial_s;

struct NUGSCN_s;
struct WORLDINFO_s;
struct gameobject_s;
struct numtx_s;

typedef struct apicharactermodellist_s { // size 0x4
    i16 model_id;                        // 0x0
    i16 count;                           // 0x2
} APICHARACTERMODELLIST;

#ifdef __cplusplus
extern "C" {
#endif
    extern APICHARACTERMODELLIST Area_PlayerModelList[9];

    extern APICHARACTERMODELLIST Area_FreePlayModelList[49];

    extern APICHARACTERMODELLIST fplist[341];

    extern APICHARACTERMODELLIST Area_MissionModelList[49];

    extern APICHARACTERMODELLIST FreePlayModelList[49];

    extern APICHARACTERMODELLIST Hub_ModelList[6];

    extern APICHARACTERMODELLIST fplist[341];
#ifdef __cplusplus
}
#endif

typedef struct KILLPARTS_CHARSCENE { // size 0x10
    struct NUGSCN_s *scene;          // 0x0
    nuhspecial_s special_scene;      // 0x4
} KILLPARTS_CHARSCENE;

#ifdef __cplusplus
extern "C" {
#endif
    void CharScene_Draw(struct WORLDINFO_s *, i32, struct numtx_s *, struct numtx_s *);
    struct nuhspecial_s *CharScene_FindHSpecial(struct WORLDINFO_s *, i32);
    extern KILLPARTS_CHARSCENE *KillPartsCharSceneArea;
    void HeadMovement(struct gameobject_s *);
    void HairMovement(struct gameobject_s *);
    void CharScenes_LevelDump(struct WORLDINFO_s *);

    typedef struct char_category_record_s { // size 0xc
        char *name;                         // 0x0
        u32 model_flags;                    // 0x4
        u32 game_flags;                     // 0x8
    } CHAR_CATEGORY_RECORD;

    void CharScenes_Init(VARIPTR *, VARIPTR *);
    void CharScenes_AreaLoad(APICHARACTERMODELLIST *, VARIPTR *, VARIPTR);
    extern struct KILLPARTS_CHARSCENE *CharScene_Area;
#ifdef __cplusplus
}
#endif
