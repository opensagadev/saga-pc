#ifndef SAGA_LEGOAPI_WORLD_LEVELS_EPISODE_H
#define SAGA_LEGOAPI_WORLD_LEVELS_EPISODE_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

typedef struct episode_data_pc_s {
    i16 name_id;
    i16 text_id;
    i16 area_ids[10];
    u8 area_count;
    u8 regular_areas;
    u8 index;
    u8 unknown_001b;
} EPISODEDATA_PC;

STATIC_ASSERT(sizeof(EPISODEDATA_PC) == 0x1c);
STATIC_ASSERT(offsetof(EPISODEDATA_PC, text_id) == 2);
STATIC_ASSERT(offsetof(EPISODEDATA_PC, area_ids) == 0x04);
STATIC_ASSERT(offsetof(EPISODEDATA_PC, area_count) == 0x18);
STATIC_ASSERT(offsetof(EPISODEDATA_PC, index) == 0x1a);
#ifdef __cplusplus
extern "C" {
#endif
    GAMEOBJECT *BossKilled(i32);
    extern EPISODEDATA_PC *EDataList;
    EPISODEDATA_PC *Episodes_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end, i32 max_count,
                                           i32 *count_out);
    extern const char LoadPermLevelsPath[18];
    extern const char LoadPermAreasPath[17];
    extern const char LoadPermEpisodesPath[20];
    u32 Episode_FindAreaFromFlags(EPISODEDATA_PC *, u32, u32);
    i32 Episode_ContainsArea(i32, i32 *);
    i32 Episode_IsComplete(EPISODEDATA_PC *, i32 *);
#ifdef __cplusplus
}
#endif

#endif
