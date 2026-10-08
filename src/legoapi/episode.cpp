#include "saga.h"

typedef struct episode_area_save_reader_pc_s {
    u8 complete, area_complete, story_complete, freeplay_complete;
    u8 minikit_complete, minikit_count, red_brick_complete, challenge_complete;
    f32 challenge_time;
} EPISODE_AREA_SAVE_READER_PC;
STATIC_ASSERT(sizeof(EPISODE_AREA_SAVE_READER_PC) == 12);
STATIC_ASSERT(offsetof(EPISODE_AREA_SAVE_READER_PC, complete) == 0);
STATIC_ASSERT(offsetof(EPISODE_AREA_SAVE_READER_PC, area_complete) == 1);
STATIC_ASSERT(offsetof(EPISODE_AREA_SAVE_READER_PC, story_complete) == 2);
STATIC_ASSERT(offsetof(EPISODE_AREA_SAVE_READER_PC, freeplay_complete) == 3);
STATIC_ASSERT(offsetof(EPISODE_AREA_SAVE_READER_PC, minikit_complete) == 4);
STATIC_ASSERT(offsetof(EPISODE_AREA_SAVE_READER_PC, minikit_count) == 5);
STATIC_ASSERT(offsetof(EPISODE_AREA_SAVE_READER_PC, red_brick_complete) == 6);
STATIC_ASSERT(offsetof(EPISODE_AREA_SAVE_READER_PC, challenge_complete) == 7);
STATIC_ASSERT(offsetof(EPISODE_AREA_SAVE_READER_PC, challenge_time) == 8);
typedef struct episode_save_reader_pc_s {
    u8 unknown_00[8];
    u8 complete;
    u8 unknown_09[3];
} EPISODE_SAVE_READER_PC;
STATIC_ASSERT(sizeof(EPISODE_SAVE_READER_PC) == 12);
STATIC_ASSERT(offsetof(EPISODE_SAVE_READER_PC, complete) == 8);
STATIC_ASSERT(offsetof(EPISODEDATA_PC, area_ids) == 4);
STATIC_ASSERT(offsetof(EPISODEDATA_PC, area_count) == 0x18);
STATIC_ASSERT(offsetof(AREA_DATA_PC, flags) == 0x7a);
STATIC_ASSERT(offsetof(AREA_DATA_PC, challenge_trial_time) == 0x84);

extern i32 EpCompleteTotal, EpCompleteCount, EpMiniKitTotal, EpMiniKitCount;
extern i32 EpCharKitTotal, EpCharKitCount, EpBuildUpTotal, EpBuildUpCount;
extern i32 EpStoryBuildUpTotal, EpStoryBuildUpCount, EpFreePlayBuildUpTotal, EpFreePlayBuildUpCount;
extern i32 EpRedBrickTotal, EpRedBrickCount, EpGoldBrickTotal, EpGoldBrickCount;
extern i32 EpisodeGoldBrickForSuperStoryPc, EpisodeGoldBrickForSuperBonusPc, EpisodeGoldBrickForChallengePc;
extern i32 HubBothTrueJediGoldBricksPc;
extern EPISODE_SAVE_READER_PC *EpisodeSavePc;

/* Original episode.cpp C++ entry. PC resets every counter even on the -1 path,
 * counts the superstory brick regardless of area filter, and has no Android
 * Store_IsPackUnlocked guard. No backing or capacity is inferred here. */
i32 Episode_CountOpenAreas(i32 episode_index, i32 area_index, COLLECTION_AREA_SAVE_PC *saves) {
    EPISODE_AREA_SAVE_READER_PC *records = (EPISODE_AREA_SAVE_READER_PC *)saves;
    EpCompleteTotal = EpCompleteCount = 0;
    EpMiniKitTotal = EpMiniKitCount = 0;
    EpCharKitTotal = EpCharKitCount = 0;
    EpBuildUpTotal = EpBuildUpCount = 0;
    EpStoryBuildUpTotal = EpStoryBuildUpCount = 0;
    EpFreePlayBuildUpTotal = EpFreePlayBuildUpCount = 0;
    EpRedBrickTotal = EpRedBrickCount = 0;
    EpGoldBrickTotal = EpGoldBrickCount = 0;
    if (episode_index == -1)
        return 0;
    if (EpisodeGoldBrickForSuperStoryPc != 0) {
        EpGoldBrickTotal = 1;
        if (EpisodeSavePc != NULL && EpisodeSavePc[episode_index].complete != 0)
            EpGoldBrickCount = 1;
    }
    i32 open = 0;
    EPISODEDATA_PC *episode = &EDataList[episode_index];
    for (i32 i = 0; i < episode->area_count; ++i) {
        i32 id = episode->area_ids[i];
        if (area_index != -1 && id != area_index)
            continue;
        AREA_DATA_PC *area = &ADataList[id];
        if (area == HUB_ADATA || (area->flags & 0x22) != 0)
            continue;
        if ((area->flags & 0x100) != 0) {
            if (EpisodeGoldBrickForSuperBonusPc != 0) {
                ++EpGoldBrickTotal;
                if (records[i].area_complete != 0)
                    ++EpGoldBrickCount;
            }
            continue;
        }
        if ((area->flags & 4) != 0) {
            ++EpGoldBrickTotal;
            if ((f64)area->challenge_trial_time > (f64)records[id].challenge_time)
                ++EpGoldBrickCount;
            continue;
        }
        EPISODE_AREA_SAVE_READER_PC *save = &records[id];
        ++EpCompleteTotal;
        EpMiniKitTotal += 10;
        if (save->complete != 0)
            ++open;
        ++EpGoldBrickTotal;
        if (save->area_complete != 0) {
            ++EpCompleteCount;
            ++EpGoldBrickCount;
        }
        if ((area->flags & 0x10) == 0)
            continue;
        ++EpGoldBrickTotal;
        if (save->minikit_complete != 0)
            ++EpGoldBrickCount;
        EpMiniKitCount += save->minikit_count;
        ++EpBuildUpTotal;
        ++EpGoldBrickTotal;
        if (HubBothTrueJediGoldBricksPc != 0) {
            if (save->story_complete != 0) {
                ++EpBuildUpCount;
                ++EpGoldBrickCount;
                ++EpStoryBuildUpCount;
            }
            ++EpBuildUpTotal;
            ++EpGoldBrickTotal;
            if (save->freeplay_complete != 0) {
                ++EpBuildUpCount;
                ++EpGoldBrickCount;
                ++EpFreePlayBuildUpCount;
            }
        } else if (save->story_complete != 0 || save->freeplay_complete != 0) {
            ++EpBuildUpCount;
            ++EpGoldBrickCount;
        }
        ++EpRedBrickTotal;
        if (save->red_brick_complete != 0)
            ++EpRedBrickCount;
        ++EpCharKitTotal;
        if (EpisodeGoldBrickForChallengePc != 0)
            ++EpGoldBrickTotal;
        if (save->challenge_complete != 0) {
            ++EpCharKitCount;
            if (EpisodeGoldBrickForChallengePc != 0)
                ++EpGoldBrickCount;
        }
    }
    return open;
}

i32 __cdecl Episode_ContainsArea(i32 area_id, i32 *area_index) {
    i32 i, j;
    for (i = 0; i < EPISODECOUNT; ++i) {
        EPISODEDATA_PC *episode = &EDataList[i];
        for (j = 0; j < episode->area_count; ++j) {
            if (episode->area_ids[j] == area_id) {
                if (area_index != NULL)
                    *area_index = j;
                return i;
            }
        }
    }
    if (area_index != NULL)
        *area_index = -1;
    return -1;
}
