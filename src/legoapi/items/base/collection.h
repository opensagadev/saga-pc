/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/characters/core/characters.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/screens/store.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 Collection_Got(i32);
    extern COLLECTID_PC *TempCollectID;
    i32 InCollectList_Index(i32, COLLECTID_PC *, i32);
    i32 Collection_GotAnyOfType(i32, u32);

    void Collection_CreateMaster(char *, i16 *, COLLECTION_BUILD_PC *, i32, f32);
    void Collection_CreateCustom(char *, i16 *, COLLECTION_BUILD_PC *, u32, u32, u32, i32, i32, VARIPTR *, VARIPTR *,
                                 i32, f32);
    i32 Collection_GetIDList(COLLECTION_BUILD_PC *, u32, u32, i16 *, i32 *, i32 *, i32);
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

struct COLLECTION_s {
    struct COLLECTID *list;
    u16 count_x;
    u16 count_y;
    void *field_8;
    void *field_c;
    f32 field_10;
    f32 field_14;
};

STATIC_ASSERT(sizeof(struct COLLECTION_s) == 24);
STATIC_ASSERT(offsetof(COLLECTION_s, list) == 0);
STATIC_ASSERT(offsetof(COLLECTION_s, count_y) == 6);
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
    void Collection_Draw(COLLECTION_BUILD_PC *, f32, f32, f32, APICHARACTERMODELLIST *, f32, i32);
#ifdef __cplusplus
}
#endif
