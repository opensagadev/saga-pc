#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/characters/core/characters.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/screens/store.h"

/* collection_config_entry_s.type: how a collection entry is unlocked, by its config keyword (0: only bought) */
#define COLLECTION_UNLOCK_NONE 0
#define COLLECTION_UNLOCK_STORY 1                 /* story */
#define COLLECTION_UNLOCK_AREA_COMPLETE 2         /* area_complete */
#define COLLECTION_UNLOCK_ALL_EPISODES_COMPLETE 3 /* all_episodes_complete */
#define COLLECTION_UNLOCK_ALL_MINIKITS_COMPLETE 4 /* all_minikits_complete */
#define COLLECTION_UNLOCK_MINIKIT 5               /* minikit */
#define COLLECTION_UNLOCK_GOLD_BRICKS 6           /* gold_bricks */
#define COLLECTION_UNLOCK_100_PERCENT 7           /* 100_percent */

#ifdef __cplusplus
extern "C" {
#endif
    i32 Collection_Got(i32);
    extern COLLECTID *TempCollectID;
    i32 InCollectList_Index(i32, COLLECTID *, i32);
    i32 Collection_GotAnyOfType(i32, u32);

    void Collection_CreateMaster(char *, i16 *, COLLECTION_BUILD *, i32, f32);
    void Collection_CreateCustom(char *, i16 *, COLLECTION_BUILD *, u32, u32, u32, i32, i32, VARIPTR *, VARIPTR *, i32, f32);
    i32 Collection_GetIDList(COLLECTION_BUILD *, u32, u32, i16 *, i32 *, i32 *, i32);
    extern i32 COLLECTION_COMPLETIONCOUNT;

    void Collection_Configure(char *, VARIPTR *, VARIPTR *);
    extern void (*Collection_GetSelectingPlayerIDsFn)(i16 *ids);
    extern i32 LEGOOBJ_ICON_FRAME_NEUTRAL;
    extern i32 LEGOOBJ_ICON_FRAME_GREEN;
    extern i32 LEGOOBJ_ICON_FRAME_BLUE;
#ifdef __cplusplus
}
#endif
typedef struct COLLECTION_s COLLECTION_s;

struct COLLECTION_s {       // size 0x18
    struct COLLECTID *list; // 0x00
    u16 count_x;            // 0x04
    u16 count_y;            // 0x06
    void *reserved_08;      // 0x08
    void *reserved_0c;      // 0x0c
    f32 reserved_10;        // 0x10
    f32 reserved_14;        // 0x14
};

#ifdef __cplusplus
extern "C" {
#endif
    void ReCalculateCompletionPoints(void);
    i32 AddToCollection(i32);
    void Collection_GetPos(COLLECTION_s *, i32, f32 *, f32 *);
    struct COLLECTID *CollectIDUnlocked(i32 id);
    extern f32 COLLECTION_DX;
    extern f32 COLLECTION_DY;
    extern f32 COLLECTION_ICONSIZE;
    void Collection_Draw(COLLECTION_BUILD *, f32, f32, f32, APICHARACTERMODELLIST *, f32, i32);
#ifdef __cplusplus
}
#endif
