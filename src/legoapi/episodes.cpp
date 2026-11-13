#include "saga.h"

/* Retail unit 0x59b330-0x59bb30 (legoapi/episodes.cpp): definitions in retail address order. */

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
STATIC_ASSERT(offsetof(EPISODEDATA_PC, area_ids) == 4);
STATIC_ASSERT(offsetof(EPISODEDATA_PC, area_count) == 0x18);
STATIC_ASSERT(offsetof(AREA_DATA_PC, flags) == 0x7a);
STATIC_ASSERT(offsetof(AREA_DATA_PC, challenge_trial_time) == 0x84);
STATIC_ASSERT(offsetof(EPISODEDATA_PC, regular_areas) == 0x19);
extern i32 EpCompleteTotal, EpCompleteCount, EpMiniKitTotal, EpMiniKitCount;
extern i32 EpCharKitTotal, EpCharKitCount, EpBuildUpTotal, EpBuildUpCount;
extern i32 EpStoryBuildUpTotal, EpStoryBuildUpCount, EpFreePlayBuildUpTotal, EpFreePlayBuildUpCount;
extern i32 EpRedBrickTotal, EpRedBrickCount, EpGoldBrickTotal, EpGoldBrickCount;
extern i32 EpisodeGoldBrickForSuperStoryPc, EpisodeGoldBrickForSuperBonusPc, EpisodeGoldBrickForChallengePc;
extern i32 HubBothTrueJediGoldBricksPc;
extern "C" {
    void __cdecl EpisodesConfigDiagnosticPc(char *path);

    EPISODEDATA_PC *__cdecl Episodes_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end, i32 max_count,
                                                   i32 *count_out) {
        NUFPAR_PC *parser = NuFParCreate(path);
        EPISODEDATA_PC *episode_base;
        EPISODEDATA_PC *episode;
        i32 episode_count = 0;
        i32 in_episode = 0;

        if (parser == NULL) {
            EpisodesConfigDiagnosticPc(path);
            if (count_out != NULL) {
                *count_out = 0;
            }
            return NULL;
        }

        buffer->addr = (buffer->addr + 3) & ~3u;
        episode_base = (EPISODEDATA_PC *)buffer->void_ptr;
        episode = episode_base;

        while (NuFParGetLine(parser) != 0) {
            char *word;

            NuFParGetWord(parser);
            word = parser->word_buffer_ptr;
            if (word[0] == '\0') {
                continue;
            }

            if (!in_episode) {
                if (episode_count < max_count && NuStrICmp(word, EpisodeConfigKeyword_episode_start) == 0) {
                    episode->name_id = -1;
                    episode->text_id = -1;
                    episode->area_count = 0;
                    episode->index = (u8)episode_count;
                    in_episode = 1;
                }
                continue;
            }

            if (NuStrICmp(word, EpisodeConfigKeyword_episode_end) == 0) {
                in_episode = 0;
                if (episode->area_count > 0) {
                    episode_count++;
                    episode++;
                }
                continue;
            }

            if (NuStrICmp(word, EpisodeConfigKeyword_area) == 0) {
                if (episode->area_count < 10 && NuFParGetWord(parser) != 0) {
                    AREA_DATA_PC *area;
                    i32 area_index;
                    i32 prior_episode_index;
                    i32 area_slot;
                    i32 found = 0;

                    area = Area_FindByName(parser->word_buffer_ptr, &area_index);
                    if (area_index != -1) {
                        for (area_slot = 0; area_slot < episode->area_count; area_slot++) {
                            if (episode->area_ids[area_slot] == area_index) {
                                found = 1;
                                break;
                            }
                        }

                        for (prior_episode_index = 0; found == 0 && prior_episode_index < episode_count;
                             prior_episode_index++) {
                            EPISODEDATA_PC *prior_episode = &episode_base[prior_episode_index];
                            for (area_slot = 0; area_slot < prior_episode->area_count; area_slot++) {
                                if (prior_episode->area_ids[area_slot] == area_index) {
                                    found = 1;
                                    break;
                                }
                            }
                        }

                        if (found == 0) {
                            i32 area_count = episode->area_count;
                            episode->area_ids[area_count] = (i16)area_index;
                            episode->area_count = (u8)(area_count + 1);
                            if ((area->flags & 6) == 0) {
                                episode->regular_areas++;
                            }
                        }
                    }
                }
                continue;
            }

            if (NuStrICmp(word, AreaConfigKeyword_name_id) == 0) {
                episode->name_id = (i16)NuFParGetInt(parser);
            } else if (NuStrICmp(word, AreaConfigKeyword_text_id) == 0) {
                episode->text_id = (i16)NuFParGetInt(parser);
            }
        }

        NuFParDestroy(parser);
        if (episode_count == 0) {
            EpisodesConfigDiagnosticPc(path);
            return NULL;
        }

        buffer->void_ptr = episode;
        if (count_out != NULL) {
            *count_out = episode_count;
        }
        return episode_base;
    }
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
extern "C" {

    u32 __cdecl Episode_FindAreaFromFlags(EPISODEDATA_PC *episode, u32 flags, u32 wanted) {
        i32 index;
        for (index = 0; index < episode->area_count; ++index) {
            AREA_DATA_PC *area = &ADataList[episode->area_ids[index]];
            if ((area->flags & flags) == wanted)
                return area->index;
        }
        return 0xffffffff;
    }
}

/* 59b700/86. Native early NULL save guard does not initialize output count. */
extern "C" i32 __cdecl Episode_IsComplete(EPISODEDATA_PC *episode, i32 *count) {
    COLLECTION_AREA_SAVE_PC *saves = Game_AreaSave;
    i32 completed = 0;
    if (saves == NULL)
        return 0;
    for (i32 i = 0; i < episode->regular_areas; ++i)
        if (saves[episode->area_ids[i]].area_complete != 0)
            ++completed;
    if (count != NULL)
        *count = completed;
    return completed == episode->regular_areas;
}

extern "C" i32 __cdecl Episodes_Completed(void) {
    i32 completed = 0;
    for (i32 i = 0; i < EPISODECOUNT; ++i) {
        EPISODEDATA_PC *episode = &EDataList[i];
        i32 complete = 0;
        if (Game_AreaSave != NULL) {
            for (i32 j = 0; j < episode->regular_areas; ++j)
                if (Game_AreaSave[episode->area_ids[j]].area_complete != 0)
                    ++complete;
            if (complete == episode->regular_areas)
                ++completed;
        }
    }
    return completed;
}

/* Original episode.cpp C++ entry. PC resets every counter even on the -1 path,
 * counts the superstory brick regardless of area filter, and has no Android
 * Store_IsPackUnlocked guard. No backing or capacity is inferred here. */
i32 Episode_CountOpenAreas(i32 episode_index, i32 area_index, COLLECTION_AREA_SAVE_PC *saves) {
    EPISODE_AREA_SAVE_READER_PC *records = (EPISODE_AREA_SAVE_READER_PC *)saves;
    i32 open = 0;
    EpCompleteCount = EpCompleteTotal = 0;
    EpMiniKitCount = EpMiniKitTotal = 0;
    EpCharKitCount = EpCharKitTotal = 0;
    EpBuildUpCount = EpBuildUpTotal = 0;
    EpStoryBuildUpCount = EpStoryBuildUpTotal = 0;
    EpFreePlayBuildUpCount = EpFreePlayBuildUpTotal = 0;
    EpRedBrickCount = EpRedBrickTotal = 0;
    EpGoldBrickCount = EpGoldBrickTotal = 0;
    if (episode_index == -1)
        return 0;
    if (EpisodeGoldBrickForSuperStoryPc != 0) {
        EpGoldBrickTotal = 1;
        if (EpisodeSavePc != NULL && EpisodeSavePc[episode_index].superstory_complete != 0)
            EpGoldBrickCount = 1;
    }
    EPISODEDATA_PC *episode = &EDataList[episode_index];
    for (i32 i = 0; i < episode->area_count; ++i) {
        i32 id = episode->area_ids[i];
        if (area_index != -1 && id != area_index)
            continue;
        if (&ADataList[id] == HUB_ADATA || (ADataList[id].flags & 0x22) != 0)
            continue;
        if ((ADataList[id].flags & 0x100) != 0) {
            if (EpisodeGoldBrickForSuperBonusPc != 0) {
                ++EpGoldBrickTotal;
                if (records[i].area_complete != 0)
                    ++EpGoldBrickCount;
            }
            continue;
        }
        if ((ADataList[id].flags & 4) != 0) {
            ++EpGoldBrickTotal;
            if ((f64)records[id].challenge_time < (f64)ADataList[id].challenge_trial_time)
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
        if ((ADataList[id].flags & 0x10) == 0)
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
