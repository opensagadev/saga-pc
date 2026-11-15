#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"

typedef struct episode_data_s { // size 0x1c
    i16 name_id;                // 0x00
    i16 text_id;                // 0x02
    i16 area_ids[10];           // 0x04
    u8 area_count;              // 0x18
    u8 regular_areas;           // 0x19
    u8 index;                   // 0x1a
    u8 unknown_001b;            // 0x1b
} EPISODEDATA;

#ifdef __cplusplus
extern "C" {
#endif
    GAMEOBJECT *BossKilled(i32);
    extern EPISODEDATA *EDataList;
    EPISODEDATA *Episodes_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end, i32 max_count, i32 *count_out);
    extern const char LoadPermLevelsPath[18];
    extern const char LoadPermAreasPath[17];
    extern const char LoadPermEpisodesPath[20];
    u32 Episode_FindAreaFromFlags(EPISODEDATA *, u32, u32);
    i32 Episode_ContainsArea(i32, i32 *);
    i32 Episode_IsComplete(EPISODEDATA *, i32 *);
#ifdef __cplusplus
}
#endif
