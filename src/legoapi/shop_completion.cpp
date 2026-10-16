#include "saga.h"
#include <string.h>

/* Native Game fields consumed by 42b080. */
typedef struct SHOP_COMPLETION_GAME_PC {
    u8 reserved_0000[0x782c];
    AREASAVE_s area_save[72];
    GAME_EPISODE_START_PC episode_save[6];
    u32 purchased_hint_bits[9];
    u32 purchased_gold_brick_bits[2];
    u32 purchased_extra_bits[2];
    u8 reserved_7c08[0x10];
    u16 completion_points;
    u8 gold_bricks;
    u8 reserved_7c1b[0x79];
    MISSION_SAVE_READER_PC mission_save;
    u8 reserved_7cf8[0x154];
} SHOP_COMPLETION_GAME_PC;
STATIC_ASSERT(sizeof(SHOP_COMPLETION_GAME_PC) == 0x7e4c);
STATIC_ASSERT(offsetof(SHOP_COMPLETION_GAME_PC, area_save) == 0x782c);
STATIC_ASSERT(offsetof(SHOP_COMPLETION_GAME_PC, episode_save) == 0x7b8c);
STATIC_ASSERT(offsetof(SHOP_COMPLETION_GAME_PC, purchased_hint_bits) == 0x7bd4);
STATIC_ASSERT(offsetof(SHOP_COMPLETION_GAME_PC, purchased_gold_brick_bits) == 0x7bf8);
STATIC_ASSERT(offsetof(SHOP_COMPLETION_GAME_PC, purchased_extra_bits) == 0x7c00);
STATIC_ASSERT(offsetof(SHOP_COMPLETION_GAME_PC, completion_points) == 0x7c18);
STATIC_ASSERT(offsetof(SHOP_COMPLETION_GAME_PC, gold_bricks) == 0x7c1a);
STATIC_ASSERT(offsetof(SHOP_COMPLETION_GAME_PC, mission_save) == 0x7c94);
STATIC_ASSERT(offsetof(COLLECTID, field5_0x9) == 9);
STATIC_ASSERT(offsetof(AREA_DATA_PC, flags) == 0x7a);
STATIC_ASSERT(offsetof(AREA_DATA_PC, challenge_trial_time) == 0x84);
STATIC_ASSERT(offsetof(HINT_PC, price) == 4);

extern "C" {
    i32 CompletionPointInfoRecalculatePc[7] = {0};
    i32 __cdecl AllMiniKitsDone(AREASAVE_s *);
    i32 __cdecl AddToCollection(i32);
    void __cdecl AddToCompletionPoints(u32);

    void __cdecl ReCalculateCompletionPoints(void) {
        SHOP_COMPLETION_GAME_PC *save = reinterpret_cast<SHOP_COMPLETION_GAME_PC *>(&Game);
        i32 i;
        save->completion_points = 0;
        save->gold_bricks = 0;
        memset(CompletionPointInfoRecalculatePc, 0, sizeof(CompletionPointInfoRecalculatePc));
        if (AllMiniKitsDone(save->area_save) != 0)
            AddToCollection(VehicleBoltId7f195cPc);
        for (i = 0; i < MasterCollectionPc.count; ++i) {
            COLLECTID *entry = reinterpret_cast<COLLECTID *>(&MasterCollectionPc.list[i]);
            if (entry->field5_0x9 != 0) {
                CompletionPointInfoRecalculatePc[2] += POINTS_PER_CHARACTER;
                if (Collection_Got(entry->id) != 0)
                    AddToCompletionPoints(POINTS_PER_CHARACTER);
            }
        }
        for (i = 0; i < AREACOUNT; ++i) {
            AREA_DATA_PC *area = &ADataList[i];
            AREASAVE_s *area_save = &save->area_save[i];
            if (area == HUB_ADATA || (area->flags & 0x2022) != 0)
                continue;
            if ((area->flags & 0x100) != 0) {
                CompletionPointInfoRecalculatePc[1] += POINTS_PER_SUPERBONUSCOMPLETE;
                if (area_save->area_complete != 0) {
                    AddToCompletionPoints(POINTS_PER_SUPERBONUSCOMPLETE);
                    if (EpisodeGoldBrickForSuperBonusPc != 0)
                        AddToGoldBricks();
                }
                continue;
            }
            if ((area->flags & 4) != 0) {
                CompletionPointInfoRecalculatePc[1] += POINTS_PER_TIMETRIAL;
                if (area_save->challenge_trial_time < static_cast<f32>(area->challenge_trial_time)) {
                    AddToCompletionPoints(POINTS_PER_TIMETRIAL);
                    AddToGoldBricks();
                }
                continue;
            }
            CompletionPointInfoRecalculatePc[1] += POINTS_PER_STORY;
            if (area_save->area_complete != 0) {
                AddToCompletionPoints(POINTS_PER_STORY);
                if ((area->flags & 0x800) == 0)
                    AddToGoldBricks();
            }
            if ((area->flags & 0x10) == 0)
                continue;
            CompletionPointInfoRecalculatePc[1] += POINTS_PER_MINIKIT;
            if (area_save->minikit_complete != 0) {
                AddToCompletionPoints(POINTS_PER_MINIKIT);
                AddToGoldBricks();
            }
            CompletionPointInfoRecalculatePc[1] += POINTS_PER_TRUEJEDI;
            if (HubBothTrueJediGoldBricksPc != 0) {
                if (area_save->true_hero_complete[0] != 0) {
                    AddToCompletionPoints(POINTS_PER_TRUEJEDI);
                    AddToGoldBricks();
                }
                CompletionPointInfoRecalculatePc[1] += POINTS_PER_TRUEJEDI;
                if (area_save->true_hero_complete[1] != 0) {
                    AddToCompletionPoints(POINTS_PER_TRUEJEDI);
                    AddToGoldBricks();
                }
            } else if (area_save->true_hero_complete[0] != 0 || area_save->true_hero_complete[1] != 0) {
                AddToCompletionPoints(POINTS_PER_TRUEJEDI);
                AddToGoldBricks();
            }
            CompletionPointInfoRecalculatePc[1] += POINTS_PER_REDBRICK;
            if (area_save->red_brick_collected != 0)
                AddToCompletionPoints(POINTS_PER_REDBRICK);
            CompletionPointInfoRecalculatePc[1] += POINTS_PER_CHALLENGE;
            if (area_save->reserved_0x7 != 0) {
                AddToCompletionPoints(POINTS_PER_CHALLENGE);
                if (EpisodeGoldBrickForChallengePc != 0)
                    AddToGoldBricks();
            }
        }
        if (MissionSys != NULL) {
            for (i = 0; i < MissionSys->count; ++i) {
                CompletionPointInfoRecalculatePc[3] += POINTS_PER_MISSION;
                if (save->mission_save.completed[i] != 0) {
                    AddToCompletionPoints(POINTS_PER_MISSION);
                    AddToGoldBricks();
                }
            }
        }
        for (i = 0; i < EPISODECOUNT; ++i) {
            CompletionPointInfoRecalculatePc[0] += POINTS_PER_SUPERSTORY;
            if (save->episode_save[i].complete != 0) {
                AddToCompletionPoints(POINTS_PER_SUPERSTORY);
                if (EpisodeGoldBrickForSuperStoryPc != 0)
                    AddToGoldBricks();
            }
        }
        for (i = 0; i < 44; ++i) {
            CompletionPointInfoRecalculatePc[4] += POINTS_PER_CHEAT;
            if ((save->purchased_extra_bits[i >> 5] & (1U << (i & 31))) != 0)
                AddToCompletionPoints(POINTS_PER_CHEAT);
        }
        for (i = 0; i < SHOPHINTCOUNT; ++i) {
            HINT_PC *hint = Hint_FindHint(ShopHintIdsPc[i]);
            if (hint != NULL && hint->price > 0) {
                CompletionPointInfoRecalculatePc[5] += POINTS_PER_HINT;
                if ((save->purchased_hint_bits[i / 32] & (1ULL << (i % 32))) != 0)
                    AddToCompletionPoints(POINTS_PER_HINT);
            }
        }
        for (i = 0; i < SHOPGOLDBRICKS; ++i) {
            CompletionPointInfoRecalculatePc[6] += POINTS_PER_GOLDBRICK;
            if ((save->purchased_gold_brick_bits[i / 32] & (1ULL << (i % 32))) != 0) {
                AddToCompletionPoints(POINTS_PER_GOLDBRICK);
                AddToGoldBricks();
            }
        }
    }
}
