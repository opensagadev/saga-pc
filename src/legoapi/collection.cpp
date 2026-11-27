#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/save_values.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/screens/gamestructure.h"
#include "legoapi/menus/screens/store.h"
#include "legoapi/world/area.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"
#include "legoapi/items/collect/minikits.h"
#include "legoapi/world/levels/episodes.h"

static i32 CollectCount = 0;
static COLLECTID *CollectList = 0;
i32 COLLECTION_COMPLETIONCOUNT = 0;
COLLECTID *TempCollectID = 0;

i32 InCollectList_Index(i32 id, COLLECTID *list, i32 count) {
    i32 index;
    TempCollectID = NULL;
    if (list == NULL) {
        list = CollectList;
        count = CollectCount;
        if (list == NULL) {
            return id;
        }
    }
    for (index = 0; index < count; ++index, ++list) {
        if (list->id == id) {
            TempCollectID = list;
            return index;
        }
    }
    return -1;
}

i32 Collection_Got(i32 id) {
    i32 area;
    if (InCollectList_Index(id, NULL, 0) == -1) {
        return 0;
    }
    area = AreaFromMiniKitID(id);
    if (area != -1) {
        if (Game_AreaSave != NULL && Game_AreaSave[area].minikit_complete != 0) {
            return 2;
        }
        return 0;
    }
    if (Game_CharacterSave != NULL && (Game_CharacterSave[id] & 1) == 0) {
        return 0;
    }
    return 1;
}

i32 Collection_GotAnyOfType(i32 type, u32 flags) {
    i32 index;
    if (CollectList == NULL) {
        return 0;
    }
    for (index = 0; index < CollectCount; ++index) {
        i32 id = CollectList[index].id;
        if (type != -1 && GCDataList[id].variant != type) {
            continue;
        }
        if (flags != 0 && (CDataList[id].model_flags & flags) != flags) {
            continue;
        }
        if (Collection_Got(id) != 0) {
            return 1;
        }
    }
    return 0;
}

i32 AddToCollection(i32 id) {
    if (id > 0 && id < CHARCOUNT) {
        if (InCollectList_Index(id, NULL, 0) == -1) {
            return 0;
        }
        if (Collection_Got(id) != 0) {
            return 0;
        }
        if (Game_CharacterSave != NULL) {
            Game_CharacterSave[id] |= SAVE_CHARACTER_UNLOCKED;
            Game_CharacterSave[id] |= SAVE_CHARACTER_AVAILABLE;
        }
        return 1;
    }
    return 0;
}

extern "C" void CollectAllCharacters(i32 only_story) {
    i32 i;

    for (i = 0; i < CollectCount; i++) {
        if (only_story == 0 || CollectList[i].type == 1) {
            AddToCollection(CollectList[i].id);
        }
    }
}

void Collection_CreateMaster(char *name, i16 *title_id, COLLECTION_BUILD *collection, i32 columns, f32 scale) {
    collection->list = CollectList;
    collection->columns = (u16)columns;
    collection->count = (u16)CollectCount;
    collection->name = name;
    collection->title_id = title_id;
    collection->scale = scale;
}

void Collection_CreateCustom(char *name, i16 *title_id, COLLECTION_BUILD *collection, u32 required_model_flags, u32 excluded_model_flags,
                             u32 required_game_flags, i32 require_buyable, i32 columns, VARIPTR *buffer, VARIPTR *buffer_end,
                             i32 use_all_characters, f32 scale) {
    i32 index;
    collection->list = (COLLECTID *)((buffer->addr + 3) & ~3u);
    collection->columns = (u16)columns;
    collection->count = 0;
    collection->name = name;
    collection->title_id = title_id;
    collection->scale = scale;
    if (use_all_characters == 0) {
        for (index = 0; index < CollectCount; ++index) {
            COLLECTID *source = &CollectList[index];
            i32 id = source->id;
            if (excluded_model_flags != 0 && (apicharsys->char_data[id].model_flags & excluded_model_flags) != 0) {
                continue;
            }
            if (require_buyable != 0 && source->can_buy == 0) {
                continue;
            }
            if (required_game_flags != 0 && (GCDataList[id].flags_090 & required_game_flags) != required_game_flags) {
                continue;
            }
            if (required_model_flags != 0 && (CDataList[id].model_flags & required_model_flags) != required_model_flags) {
                continue;
            }
            collection->list[collection->count++] = *source;
        }
    } else {
        for (index = 0; index < CHARCOUNT; ++index) {
            if (excluded_model_flags != 0 && (apicharsys->char_data[index].model_flags & excluded_model_flags) != 0) {
                continue;
            }
            if (required_game_flags != 0 && (GCDataList[index].flags_090 & required_game_flags) != required_game_flags) {
                continue;
            }
            if (required_model_flags != 0 && (CDataList[index].model_flags & required_model_flags) != required_model_flags) {
                continue;
            }
            memset(&collection->list[collection->count], 0, sizeof(COLLECTID));
            collection->list[collection->count].id = (i16)index;
            collection->count++;
        }
    }
    buffer->addr = (u32)(collection->list + collection->count);
}

void Collection_GetPos(COLLECTION_s *collection, i32 id, float *x, float *y) {
    i32 i;

    for (i = 0; i < collection->count_y; i++) {
        if (collection->list[i].id == id) {
            *x = ((float *)&collection->list[i].cheat_code[8])[0];
            *y = ((float *)&collection->list[i].cheat_code[8])[1];
            return;
        }
    }
}

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

i32 Collection_GetIDList(COLLECTION_BUILD *collection, u32 mask, u32 required, i16 *out, i32 *first, i32 *second, i32 unused) {
    i32 index;
    i32 count = 0;
    if (first != NULL) {
        *first = -1;
    }
    if (second != NULL) {
        *second = -1;
    }
    for (index = 0; index < collection->count; ++index) {
        i32 id = collection->list[index].id;
        if ((CDataList[id].model_flags & mask) != required || Collection_Got(id) == 0) {
            continue;
        }
        if (out != NULL) {
            out[count] = (i16)id;
        }
        ++count;
        if (first != NULL) {
            if (*first == -1) {
                *first = id;
            } else if (second != NULL && *second == -1) {
                *second = id;
            }
        }
    }
    if (out != NULL) {
        out[count] = -1;
    }
    return count;
}

/* Native 594220 is an actual RET with no argument reads. */
extern "C" void CollectionConfigDiagnostic(char *file, ...) {
}

void Collection_Configure(char *file, VARIPTR *buffer, VARIPTR *end) {
    NUFPAR *parser = NuFParCreate(file);
    COLLECTID *entry;
    i32 area;
    if (parser == NULL) {
        CollectionConfigDiagnostic(file);
        return;
    }

    entry = (COLLECTID *)((buffer->addr + 3) & ~3u);
    buffer->void_ptr = entry;
    CollectList = entry;
    CollectCount = 0;
    COLLECTION_COMPLETIONCOUNT = 0;

    while (NuFParGetLine(parser) != 0) {
        if (NuFParGetWord(parser) == 0 || NuStrICmp(parser->word_buffer_ptr, "collect") != 0 || NuFParGetWord(parser) == 0) {
            continue;
        }
        entry->id = (i16)CharIDFromName(parser->word_buffer_ptr);
        if (entry->id == -1) {
            continue;
        }
        if (InCollectList_Index(entry->id, CollectList, CollectCount) != -1) {
            CollectionConfigDiagnostic(file, parser->word_buffer_ptr);
            continue;
        }

        entry->type = COLLECTION_UNLOCK_NONE;
        entry->area_index = 0xff;
        entry->can_buy = 0;
        entry->price = 0;
        entry->gold_bricks = 0;
        entry->counts_towards_completion = 0;
        entry->cheat_code[0] = '\0';

        while (NuFParGetWord(parser) != 0) {
            if (NuStrICmp(parser->word_buffer_ptr, "story") == 0) {
                entry->type = COLLECTION_UNLOCK_STORY;
            } else if (NuStrICmp(parser->word_buffer_ptr, "area_complete") == 0) {
                if (NuFParGetWord(parser) != 0 && Area_FindByName(parser->word_buffer_ptr, &area) != NULL) {
                    entry->type = COLLECTION_UNLOCK_AREA_COMPLETE;
                    entry->area_index = (u8)area;
                }
            } else if (NuStrICmp(parser->word_buffer_ptr, "all_episodes_complete") == 0) {
                entry->type = COLLECTION_UNLOCK_ALL_EPISODES_COMPLETE;
            } else if (NuStrICmp(parser->word_buffer_ptr, "100_percent") == 0) {
                entry->type = COLLECTION_UNLOCK_100_PERCENT;
            } else if (NuStrICmp(parser->word_buffer_ptr, "gold_bricks") == 0) {
                entry->type = COLLECTION_UNLOCK_GOLD_BRICKS;
                entry->gold_bricks = (u16)abs(NuFParGetInt(parser));
            } else if (NuStrICmp(parser->word_buffer_ptr, "all_minikits_complete") == 0) {
                entry->type = COLLECTION_UNLOCK_ALL_MINIKITS_COMPLETE;
            } else if (NuStrICmp(parser->word_buffer_ptr, "minikit") == 0) {
                entry->type = COLLECTION_UNLOCK_MINIKIT;
            } else if (NuStrICmp(parser->word_buffer_ptr, "buy_in_shop") == 0) {
                entry->can_buy = 1;
                entry->price = NuFParGetInt(parser);
            } else if (NuStrICmp(parser->word_buffer_ptr, "cheat_code") == 0 && NuFParGetWord(parser) != 0 &&
                       NuStrLen(parser->word_buffer_ptr) == 6) {
                NuStrCpy(entry->cheat_code, parser->word_buffer_ptr);
                NuStrUpr(entry->cheat_code, entry->cheat_code);
            }
        }
        if ((entry->type != COLLECTION_UNLOCK_NONE || entry->can_buy != 0) && entry->type != COLLECTION_UNLOCK_100_PERCENT) {
            entry->counts_towards_completion = 1;
            ++COLLECTION_COMPLETIONCOUNT;
        }
        ++entry;
        ++CollectCount;
    }
    NuFParDestroy(parser);
    if (CollectCount > 0) {
        buffer->void_ptr = entry;
    } else {
        CollectList = NULL;
        CollectionConfigDiagnostic(file);
    }
}

/* Append to existing collection.c after its canonical static CollectCount/CollectList. */

COLLECTID *CollectIDUnlocked(i32 id) {
    COLLECTID *entry;
    COLLECTID *list = CollectList;
    i32 index = InCollectList_Index(id, list, CollectCount);
    i32 unlocked = 0;
    i32 area;
    if (index != -1) {
        if (Game_CharacterSave != NULL && (Game_CharacterSave[id] & 2) != 0) {
            unlocked = 1;
        } else {
            entry = &list[index];
            switch (entry->type) {
                case COLLECTION_UNLOCK_NONE:
                    unlocked = 1;
                    break;
                case COLLECTION_UNLOCK_AREA_COMPLETE:
                    area = (i8)entry->area_index;
                    if (area != -1 && Game_AreaSave[area].area_complete != 0) {
                        unlocked = 1;
                    }
                    break;
                case COLLECTION_UNLOCK_ALL_EPISODES_COMPLETE:
                    if (Episodes_Completed() == EPISODECOUNT) {
                        unlocked = 1;
                    }
                    break;
                case COLLECTION_UNLOCK_GOLD_BRICKS:
                    if (CustomiserCompletionSave != NULL && CustomiserCompletionSave->gold_bricks >= entry->gold_bricks) {
                        unlocked = 1;
                    }
                    break;
                case COLLECTION_UNLOCK_100_PERCENT:
                    unlocked = Game_100PercentComplete();
                    break;
                case COLLECTION_UNLOCK_ALL_MINIKITS_COMPLETE:
                    unlocked = AllMiniKitsDone((AREASAVE_s *)Game_AreaSave);
                    break;
            }
        }
        if (unlocked != 0) {
            return &CollectList[index];
        }
    }
    return NULL;
}
