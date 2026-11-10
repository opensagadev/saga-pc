#include "saga.h"

extern "C" {
    AREA_DATA_PC *__cdecl Areas_ConfigureList(char *, VARIPTR *, VARIPTR *, i32, i32 *);
    void __cdecl AreasConfigDiagnosticPc(char *message);
}
#include "saga.h"
#include <string.h>

typedef struct area_gizmotype_pc_s {
    u8 unknown_0000[0x44];
    char *(__cdecl *get_gizmo_name)(void *gizmo);
    u8 unknown_0048[0x54];
} AREA_GIZMOTYPE_PC;
STATIC_ASSERT(sizeof(AREA_GIZMOTYPE_PC) == 0x9c);
STATIC_ASSERT(offsetof(AREA_GIZMOTYPE_PC, get_gizmo_name) == 0x44);

typedef struct area_gizmotypes_pc_s {
    i32 count;
    i32 unknown_0004;
    AREA_GIZMOTYPE_PC *types;
} AREA_GIZMOTYPES_PC;
STATIC_ASSERT(sizeof(AREA_GIZMOTYPES_PC) == 0x0c);
STATIC_ASSERT(offsetof(AREA_GIZMOTYPES_PC, types) == 0x08);

typedef struct area_gizmoset_pc_s {
    void *type;
    i32 count;
    i32 max_count;
    void *gizmos;
    void *unknown_0010;
} AREA_GIZMOSET_PC;
STATIC_ASSERT(sizeof(AREA_GIZMOSET_PC) == 0x14);
STATIC_ASSERT(offsetof(AREA_GIZMOSET_PC, count) == 0x04);
STATIC_ASSERT(offsetof(AREA_GIZMOSET_PC, gizmos) == 0x0c);

typedef struct area_gizmo_pc_s {
    void *object;
    i16 unknown_0004;
    u8 type_id;
    u8 unknown_0007;
} AREA_GIZMO_PC;
STATIC_ASSERT(sizeof(AREA_GIZMO_PC) == 8);
STATIC_ASSERT(offsetof(AREA_GIZMO_PC, type_id) == 0x06);

typedef struct area_gizmosys_prefix_pc_s {
    AREA_GIZMOSET_PC *sets;
} AREA_GIZMOSYS_PREFIX_PC;
STATIC_ASSERT(offsetof(AREA_GIZMOSYS_PREFIX_PC, sets) == 0x00);

void *__cdecl GizmoFindByName(void *gizmo_sys, i32 type_id, char *name) {
    AREA_GIZMOTYPES_PC *types = (AREA_GIZMOTYPES_PC *)gizmotypes;

    if (types == NULL || gizmo_sys == NULL || name == NULL) {
        return NULL;
    }

    if (type_id >= 0 && type_id <= types->count) {
        AREA_GIZMOTYPE_PC *type;
        AREA_GIZMOSET_PC *set;
        AREA_GIZMO_PC *gizmo;
        i32 index;

        type = &types->types[type_id];
        set = &((AREA_GIZMOSYS_PREFIX_PC *)gizmo_sys)->sets[type_id];
        if (type->get_gizmo_name == NULL) {
            return NULL;
        }
        gizmo = (AREA_GIZMO_PC *)set->gizmos;
        for (index = 0; index < set->count; ++index, ++gizmo) {
            char *gizmo_name = type->get_gizmo_name(gizmo);
            if (NuStrICmp(gizmo_name, name) == 0) {
                return gizmo;
            }
        }
        return NULL;
    }

    {
        AREA_GIZMOTYPE_PC *type;
        AREA_GIZMOSET_PC *set;
        i32 type_index;

        type = types->types;
        set = ((AREA_GIZMOSYS_PREFIX_PC *)gizmo_sys)->sets;
        for (type_index = 0; type_index < ((AREA_GIZMOTYPES_PC *)gizmotypes)->count; ++type_index, ++type, ++set) {
            AREA_GIZMO_PC *gizmo;
            i32 gizmo_index;

            if (type->get_gizmo_name == NULL) {
                continue;
            }
            gizmo = (AREA_GIZMO_PC *)set->gizmos;
            for (gizmo_index = 0; gizmo_index < set->count; ++gizmo_index, ++gizmo) {
                char *gizmo_name = type->get_gizmo_name(gizmo);
                if (NuStrICmp(gizmo_name, name) == 0) {
                    return gizmo;
                }
            }
        }
    }
    return NULL;
}

void __cdecl Cheats_Init(CHEAT_PC *cheats) {
    CHEATSYSTEM_PC *system;

    system = CheatSystemPtr;
    system->cheats = cheats;
    system->count = 0;
    if (cheats == NULL || cheats[0].name == NULL) {
        return;
    }

    do {
        ++system->count;
    } while ((++cheats)->name != NULL);
}

i32 __cdecl CharIDFromName(const char *name) {
    i32 index;

    for (index = 0; index < CHARCOUNT; ++index) {
        if (NuStrICmp(CDataList[index].name, name) == 0) {
            return index;
        }
    }
    return -1;
}

i32 __cdecl Cheat_FindByName(char *name) {
    CHEAT_PC *cheats;
    i32 index;

    cheats = CheatSystemPtr->cheats;
    for (index = 0; index < CheatSystemPtr->count; ++index) {
        if (NuStrICmp(cheats[index].name, name) == 0) {
            return index;
        }
    }
    return -1;
}

AREA_DATA_PC *__cdecl Area_FindByName(char *name, i32 *index_out) {
    i32 index;

    for (index = 0; index < AREACOUNT; ++index) {
        if (NuStrICmp(ADataList[index].file, name) == 0) {
            if (index_out != NULL) {
                *index_out = index;
            }
            return &ADataList[index];
        }
    }

    if (index_out != NULL) {
        *index_out = -1;
    }
    return NULL;
}

void __cdecl Areas_ConfigureResidents(VARIPTR *buffer, VARIPTR *buffer_end) {
    AREA_DATA_PC *area;
    i32 area_index;

    (void)buffer_end;
    if (ADataList == NULL) {
        return;
    }

    buffer->addr = (buffer->addr + 3) & ~3u;
    area = ADataList;
    for (area_index = 0; area_index < AREACOUNT; ++area_index, ++area) {
        NUFPAR_PC *parser;
        char path[0x100];
        i32 resident_count;

        area->hub_player_ids = NULL;
        if ((area->flags & 0x0002) != 0) {
            continue;
        }

        NuStrCpy(path, AreaResidentsDirectory);
        NuStrCat(path, area->dir);
        NuStrCat(path, "\\");
        NuStrCat(path, area->file);
        NuStrCat(path, AreaResidentsExtension);
        parser = NuFParCreate(path);
        if (parser == NULL) {
            continue;
        }

        resident_count = 0;
        while (NuFParGetLine(parser) != 0) {
            i32 character_id;

            if (NuFParGetWord(parser) == 0 || NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_character) != 0) {
                continue;
            }
            if (NuFParGetWord(parser) == 0) {
                continue;
            }
            character_id = CharIDFromName(parser->word_buffer_ptr);
            if (character_id == -1 || NuFParGetWord(parser) == 0 ||
                NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_resident) != 0) {
                continue;
            }

            if (area->hub_player_ids == NULL) {
                area->hub_player_ids = (i16 *)buffer->void_ptr;
            }
            area->hub_player_ids[resident_count] = (i16)character_id;
            ++resident_count;
        }
        NuFParDestroy(parser);

        if (resident_count > 0) {
            area->hub_player_ids[resident_count] = -1;
            buffer->void_ptr = &area->hub_player_ids[resident_count + 1];
        }
    }
    buffer->addr = (buffer->addr + 3) & ~3u;
}

void __cdecl Areas_FixUp(AREAFIXUP_PC *fixup) {
    if (fixup != NULL) {
        while (fixup->name != NULL) {
            if (fixup->area != NULL) {
                *fixup->area = Area_FindByName(fixup->name, NULL);
            }
            ++fixup;
        }
    }
}

__declspec(noinline) LEVELDATA_PC *__cdecl Area_FindStatusLevel(AREA_DATA_PC *area, i32 *index_out) {
    i32 index;

    if (index_out != NULL) {
        *index_out = -1;
    }
    if (area == NULL) {
        return NULL;
    }

    for (index = 0; index < area->level_count; ++index) {
        if ((LDataList[area->levels[index]].flags & 0x400) != 0) {
            if (index_out != NULL) {
                *index_out = area->levels[index];
            }
            return &LDataList[area->levels[index]];
        }
    }
    return NULL;
}

LEVELDATA_PC *__cdecl Area_FindNextPlayLevel(i32 level_index) {
    LEVELDATA_PC *level;
    i32 area_index;
    i32 area_level_index;

    level = &LDataList[level_index];
    area_index = level->area_index;
    area_level_index = level->area_level_index;
    if (area_index != -1) {
        const i32 last = ADataList[area_index].level_count - 1;

        for (; area_level_index < last; ++area_level_index) {
            const i32 next = ADataList[area_index].levels[area_level_index];

            if ((LDataList[next].flags & 0xe0) == 0) {
                return &LDataList[next];
            }
        }
    }
    return level;
}

void __cdecl SuperCounters_Reset(i32 area_index) {
    if (area_index != -1) {
        AREA_DATA_PC *area;
        AREA_SUPERCOUNTER_PC *super_counters;
        i32 index;

        area = &ADataList[area_index];
        super_counters = area->super_counters;
        if (super_counters != NULL && area->super_counter_count != 0) {
            for (index = 0; index < area->super_counter_count; ++index) {
                super_counters[index].collected_count = 0;
            }
        }
    }
}

AREA_SUPERCOUNTER_PC *__cdecl SuperCounter_FindFromNameAndLevel(char *name, WORLDINFO *world,
                                                                AREA_SUPERCOUNTER_PICKUP_PC **pickup_out) {
    AREA_SUPERCOUNTER_PC *counter;
    i32 counter_index;

    if (world->area != NULL && world->area->super_counters != NULL) {
        counter = world->area->super_counters;
        for (counter_index = 0; counter_index < world->area->super_counter_count; ++counter_index, ++counter) {
            i32 pickup_index;

            for (pickup_index = 0; pickup_index < counter->pickup_count; ++pickup_index) {
                AREA_SUPERCOUNTER_PICKUP_PC *pickup;

                pickup = &counter->pickups[pickup_index];
                if (pickup->level_index == world->level_idx && NuStrICmp(pickup->name, name) == 0) {
                    if (pickup_out != NULL) {
                        *pickup_out = pickup;
                    }
                    return counter;
                }
            }
        }
    }

    if (pickup_out != NULL) {
        *pickup_out = NULL;
    }
    return NULL;
}

void __cdecl SuperCounters_ResetProcessed(WORLDINFO *world) {
    AREA_SUPERCOUNTER_PC *super_counters;
    i32 index;

    if (world->area != NULL && world->area->super_counters != NULL && world->area->super_counter_count != 0) {
        super_counters = world->area->super_counters;
        for (index = 0; index < world->area->super_counter_count; ++index) {
            super_counters[index].processed_flags &= (u8)~2;
        }
    }
}

AREA_DATA_PC *__cdecl Areas_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end, i32 max_count,
                                          i32 *count_out) {
    NUFPAR_PC *parser;
    AREA_DATA_PC *area_base;
    AREA_DATA_PC *area;
    i32 area_count;
    i32 in_area;
    u32 aligned_address;

    parser = NuFParCreate(path);
    if (parser == NULL) {
        AreasConfigDiagnosticPc(path);
        if (count_out != NULL) {
            *count_out = 0;
        }
        return NULL;
    }

    area_count = 0;
    in_area = 0;
    aligned_address = (buffer->addr + 3) & ~3u;
    buffer->addr = aligned_address;
    area = (AREA_DATA_PC *)buffer->void_ptr;
    area_base = area;

    while (NuFParGetLine(parser) != 0) {
        NuFParGetWord(parser);
        if (parser->word_buffer_ptr[0] == '\0') {
            continue;
        }

        if (in_area != 0) {
            if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_area_end) == 0) {
                in_area = 0;
                if (area->dir[0] != '\0' && area->file[0] != '\0' && area->level_count != 0 &&
                    (area->flags & 0x40) == 0) {
                    ++area;
                    ++area_count;
                }
            } else if (NuStrICmp(parser->word_buffer_ptr, LevelConfigKeywordDir) == 0) {
                if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buffer_ptr) < 0x40) {
                    NuStrCpy(area->dir, parser->word_buffer_ptr);
                }
            } else if (NuStrICmp(parser->word_buffer_ptr, LevelConfigKeywordFile) == 0) {
                if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buffer_ptr) < 0x20) {
                    NuStrCpy(area->file, parser->word_buffer_ptr);
                }
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_level) == 0) {
                if (area->level_count >= 12 || NuFParGetWord(parser) == 0) {
                    continue;
                }
                {
                    i32 level_index;
                    i32 level_slot;
                    i32 duplicate;

                    Level_FindByName(parser->word_buffer_ptr, &level_index);
                    if (level_index < 0) {
                        continue;
                    }
                    level_slot = area->level_count;
                    duplicate = 0;
                    for (level_slot = 0; level_slot < area->level_count; ++level_slot) {
                        if (area->levels[level_slot] == level_index) {
                            duplicate = 1;
                            break;
                        }
                    }
                    if (duplicate == 0) {
                        area->levels[area->level_count] = (i16)level_index;
                        ++area->level_count;
                    }
                }
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_single_buffer) == 0) {
                area->flags |= 0x0008;
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_minikit) == 0) {
                area->flags |= 0x0010;
                if (NuFParGetWord(parser) != 0) {
                    area->minikit_id = (i16)CharIDFromName(parser->word_buffer_ptr);
                }
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_test_area) == 0) {
                area->flags |= 0x0040;
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_hub_area) == 0) {
                area->flags |= 0x0020;
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_override_things_scene) == 0) {
                area->flags |= 0x0400;
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_vehicle_area) == 0) {
                area->flags |= 0x0001;
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_ending_area) == 0) {
                area->flags |= 0x0002;
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_bonus_area) == 0) {
                area->flags |= 0x0004;
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_super_bonus_area) == 0) {
                area->flags |= 0x0104;
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_nocharactercollision) == 0 ||
                       NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_no_character_collision) == 0 ||
                       NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_nocharactercollisions) == 0 ||
                       NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_no_character_collisions) == 0) {
                area->flags |= 0x0080;
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_nopickupgravity) == 0) {
                area->flags |= 0x0200;
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_no_gold_brick) == 0) {
                area->flags |= 0x0800;
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_no_completion_points) == 0) {
                area->flags |= 0x2000;
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_no_freeplay) == 0) {
                area->flags |= 0x1000;
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_name_id) == 0) {
                area->name_id = (i16)NuFParGetInt(parser);
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_text_id) == 0) {
                area->text_id = (i16)NuFParGetInt(parser);
                if (NuFParGetWord(parser) != 0) {
                    i32 text_id_value;

                    text_id_value = NuAToI(parser->word_buffer_ptr);
                    area->text_id_value = (u8)(text_id_value < 0 ? -text_id_value : text_id_value);
                }
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_timetrial_time) == 0) {
                area->challenge_trial_time = (u16)NuFParGetInt(parser);
            } else if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_redbrick_cheat) == 0 ||
                       NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_redbrick_extra) == 0) {
                if (NuFParGetWord(parser) != 0) {
                    area->cheat = (i8)Cheat_FindByName(parser->word_buffer_ptr);
                }
            }
            continue;
        }

        if (NuStrICmp(parser->word_buffer_ptr, AreaConfigKeyword_area_start) == 0 && area_count < max_count) {
            area->dir[0] = '\0';
            area->file[0] = '\0';
            area->levels[0] = -1;
            area->name_id = -1;
            area->flags = 0;
            area->index = (u8)area_count;
            area->level_count = 0;
            area->cheat = -1;
            area->super_counter_count = 0;
            area->super_counters = NULL;
            area->challenge_trial_time = 0;
            area->episode_index = -1;
            area->area_index = -1;
            area->area_music = -1;
            area->minikit_id = -1;
            area->unknown_008c = 0;
            area->unknown_0090 = 0;
            area->text_id = -1;
            area->text_id_value = 1;
            area->hub_player_ids = NULL;
            in_area = 1;
        }
    }

    NuFParDestroy(parser);
    if (area_count <= 0) {
        AreasConfigDiagnosticPc(path);
        return NULL;
    }

    buffer->void_ptr = area;
    if (count_out != NULL) {
        *count_out = area_count;
    }

    {
        i32 index;

        for (index = 0; index < area_count; ++index) {
            AREA_DATA_PC *current;

            current = &area_base[index];
            if (current->challenge_trial_time == 0) {
                if ((current->flags & 0x0104) == 0x0004) {
                    current->challenge_trial_time = AREA_DEFAULTBONUSTIMETRIALTIME;
                } else if ((current->flags & 0x0010) != 0) {
                    current->challenge_trial_time = AREA_DEFAULTCHALLENGETIME;
                }
            }
            if (current->cheat != -1) {
                Cheat_SetArea(current->cheat, index);
            }
            if (current->challenge_trial_time != 0 && (current->flags & 0x0114) == 0x0010) {
                current->challenge_trial_time = 1200;
            }
        }
    }
    return area_base;
}

void __cdecl GoToNewLevel(i32 level_index) {
    i32 waiting;

    waiting = waiting_for_level;
    NewLData = &LDataList[level_index];
    if (waiting != -1)
        waiting_for_new_level = 1;
}

void __cdecl CompleteLevel(WORLDINFO *world) {
    LEVELDATA_PC *level;

    level = NewLData;
    LevelCompletionFallbackFlagPC = 1u;
    if (level == NULL || ((level->flags & 0xe0u) != 0 && FreePlay != 0)) {
        level = Area_FindStatusLevel(world->area, NULL);
        NewLData = level;
    }
    if (level == NULL)
        NewLData = HUB_LDATA;
    if (waiting_for_level != -1)
        waiting_for_new_level = 1;
}

typedef struct player_progress_hp_pc_s {
    u8 unknown_0000[8];
    u8 hitpoints;
    u8 unknown_0009[7];
} PLAYER_PROGRESS_HP_PC;
STATIC_ASSERT(sizeof(PLAYER_PROGRESS_HP_PC) == 0x10);
STATIC_ASSERT(offsetof(PLAYER_PROGRESS_HP_PC, hitpoints) == 8);

i32 __cdecl AreaFromMiniKitID(i32 id) {
    i32 index;
    for (index = 0; index < AREACOUNT; ++index) {
        if (ADataList[index].minikit_id == id)
            return index;
    }
    return -1;
}

extern "C" i8 __cdecl Area_CharIDInCurrentList(i32 character_id) {
    i32 index;
    if (CurrentCList != NULL) {
        for (index = 0; CurrentStoryCList[index].model_id != -1; ++index) {
            if (index >= 64)
                break;
            if (CurrentStoryCList[index].model_id == character_id)
                return (i8)index;
        }
    }
    return -1;
}

#include "saga.h"
#include <string.h>
typedef struct AREA_MISSION_READER_PC {
    u8 unknown_00[0x1f];
    u8 character_count;
    i16 character_ids[8];
} AREA_MISSION_READER_PC;
STATIC_ASSERT(offsetof(AREA_MISSION_READER_PC, character_count) == 0x1f);
STATIC_ASSERT(offsetof(AREA_MISSION_READER_PC, character_ids) == 0x20);
typedef struct AREA_MISSION_DATA_READER_PC {
    i16 find_char;
} AREA_MISSION_DATA_READER_PC;
STATIC_ASSERT(offsetof(AREA_MISSION_DATA_READER_PC, find_char) == 0);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_other_index) == 0x9c);
STATIC_ASSERT(offsetof(AREA_SUPERCOUNTER_PC, red) == 0x1e4);
STATIC_ASSERT(offsetof(AREA_SUPERCOUNTER_PC, green) == 0x1e5);
STATIC_ASSERT(offsetof(AREA_SUPERCOUNTER_PC, blue) == 0x1e6);
extern "C" {
    extern i32 Area_PlayerModelCount, Area_StoryModelCount, Area_FreePlayModelCount, Area_MissionModelCount,
        LevelLoadCount, AreaMusic;
    extern APICHARACTERMODELLIST AreaPlayerModelsPc[], Area_FreePlayModelList[], Area_MissionModelList[];
    extern AREA_LEVEL_LOAD_PC LevelLoad[];
    i32 __cdecl CharIDFromName(const char *);
}
#define AREA_STORY_MODELS_PC Area_StoryModelListPc
static void AreaAddModelListPrivatePc(APICHARACTERMODELLIST *list, i32 *count, i32 capacity, i32 character_id,
                                      i32 load_model, AREA_EXTRA_MODEL_PC *extras) {
    if (*count < capacity && InModelList(list, character_id, NULL) == 0) {
        list[*count].model_id = (i16)character_id;
        list[*count].count = (i16)load_model;
        ++*count;
        list[*count].model_id = -1;
    }
    if (extras == NULL || list == AreaPlayerModelsPc || load_model == 0)
        return;
    for (; extras->model_list != NULL; ++extras) {
        if (*extras->model_list != character_id)
            continue;
        const i32 extra_id = *extras->extra_id;
        if (extra_id != -1 && *count < capacity && InModelList(list, extra_id, NULL) == 0) {
            list[*count].model_id = (i16)extra_id;
            list[*count].count = (i16)load_model;
            ++*count;
            list[*count].model_id = -1;
        }
        return;
    }
}
extern "C" void __cdecl Area_Configure(i32 area, i32 param, AREA_EXTRA_MODEL_PC *models, i16 *s) {
    Area_PlayerModelCount = 0;
    AreaPlayerModelsPc[0].model_id = -1;
    Area_StoryModelCount = 0;
    AREA_STORY_MODELS_PC[0].model_id = -1;
    Area_FreePlayModelCount = 0;
    Area_FreePlayModelList[0].model_id = -1;
    Area_MissionModelCount = 0;
    Area_MissionModelList[0].model_id = -1;
    LevelLoad[0].level = -1;
    LevelLoadCount = 0;

    if (area != -1) {
        AreaMusic = ADataList[area].area_music;
        ADataList[area].super_counters = NULL;
        ADataList[area].super_counter_count = 0;
    } else {
        AreaMusic = -1;
    }

    if (Mission_Active(MissionSys) != NULL && ((AREA_MISSION_READER_PC *)MissionSys)->character_count > 0) {
        i16 mission_characters[64];
        PCByteCopy_candidate(mission_characters, ((AREA_MISSION_READER_PC *)MissionSys)->character_ids,
                             ((AREA_MISSION_READER_PC *)MissionSys)->character_count * sizeof(i16));
        const u8 shuffle_count = ((AREA_MISSION_READER_PC *)MissionSys)->character_count;
        if (shuffle_count > 2) {
            for (i32 swap = 0; swap < shuffle_count * 3; ++swap) {
                const i32 first = qrand() / (0xffff / ((AREA_MISSION_READER_PC *)MissionSys)->character_count + 1);
                const i32 second =
                    (qrand() / (0xffff / (((AREA_MISSION_READER_PC *)MissionSys)->character_count - 1) + 1) + first) %
                    ((AREA_MISSION_READER_PC *)MissionSys)->character_count;
                const i16 held = mission_characters[first];
                mission_characters[first] = mission_characters[second];
                mission_characters[second] = held;
            }
        }
        for (i32 index = 0; index < ((AREA_MISSION_READER_PC *)MissionSys)->character_count; ++index) {
            if (Area_MissionModelCount < 0x30) {
                Area_MissionModelList[Area_MissionModelCount].model_id = (i16)(mission_characters[index]);
                Area_MissionModelList[Area_MissionModelCount].count = (i16)(1);
                ++Area_MissionModelCount;
            }
            if (Area_PlayerModelCount < 8) {
                AreaPlayerModelsPc[Area_PlayerModelCount].model_id = (i16)(mission_characters[index]);
                AreaPlayerModelsPc[Area_PlayerModelCount].count = (i16)(1);
                ++Area_PlayerModelCount;
            }
        }
        if (Area_MissionModelCount < 0x30) {
            Area_MissionModelList[Area_MissionModelCount].model_id =
                (i16)(((AREA_MISSION_DATA_READER_PC *)MissionSys->active_mission)->find_char);
            Area_MissionModelList[Area_MissionModelCount].count = (i16)(1);
            ++Area_MissionModelCount;
        }
        Area_MissionModelList[Area_MissionModelCount].model_id = -1;
    }

    char path[128];
    NuStrCpy(path, "levels\\");
    if (area != -1) {
        NuStrCat(path, ADataList[area].dir);
        NuStrCat(path, "\\");
        NuStrCat(path, ADataList[area].file);
    } else {
        NuStrCat(path, LDataList[param].dir);
        NuStrCat(path, "\\");
        NuStrCat(path, LDataList[param].name);
    }
    NuStrCat(path, ".txt");

    NUFPAR_PC *fp = NuFParCreate(path);
    if (fp == NULL) {
        const i16 first = s[0];
        AreaPlayerModelsPc[0].model_id = first;
        AreaPlayerModelsPc[0].count = 1;
        const i16 second = s[1];
        AreaPlayerModelsPc[1].model_id = second;
        AreaPlayerModelsPc[1].count = 1;
        AreaPlayerModelsPc[2].model_id = -1;
        Area_PlayerModelCount = 2;
        AREA_STORY_MODELS_PC[0].model_id = first;
        AREA_STORY_MODELS_PC[0].count = 1;
        AREA_STORY_MODELS_PC[1].model_id = second;
        AREA_STORY_MODELS_PC[1].count = 1;
        AREA_STORY_MODELS_PC[2].model_id = -1;
        Area_StoryModelCount = 2;
        Area_PlayerIDList[0] = first;
        Area_PlayerIDList[1] = second;
        Area_PlayerIDList[2] = -1;
        return;
    }

    AREA_SUPERCOUNTER_PC counters[10];
    AREA_SUPERCOUNTER_PC *counter = NULL;
    i32 counter_count = 0;
    i32 in_counter = 0;
    i32 level_index;
    i32 index;
    while (NuFParGetLine(fp) != 0) {
        NuFParGetWord(fp);
        if (fp->word_buffer_ptr[0] == '\0')
            continue;
        if (in_counter != 0) {
            if (NuStrICmp(fp->word_buffer_ptr, "supercounter_end") == 0) {
                if (counter->pickup_count > 0) {
                    ++counter_count;
                    in_counter = 0;
                }
            } else if (NuStrICmp(fp->word_buffer_ptr, "pickup") == 0) {
                if (NuFParGetWord(fp) == 0 || NuStrLen(fp->word_buffer_ptr) >= 8)
                    continue;
                NuStrCpy(counter->pickups[counter->pickup_count].name, fp->word_buffer_ptr);
                counter->pickups[counter->pickup_count].level_index = -1;
                counter->pickups[counter->pickup_count].position_name[0] = '\0';
                counter->pickups[counter->pickup_count].use_special = 1;
                while (NuFParGetWord(fp) != 0) {
                    if (NuStrICmp(fp->word_buffer_ptr, "in_level") == 0) {
                        if (NuFParGetWord(fp) != 0 && Level_FindByName(fp->word_buffer_ptr, &level_index) != NULL)
                            counter->pickups[counter->pickup_count].level_index = static_cast<i16>(level_index);
                    } else if (NuStrICmp(fp->word_buffer_ptr, "draw_at_gizmo") == 0) {
                        if (NuFParGetWord(fp) != 0 && NuStrLen(fp->word_buffer_ptr) < 16) {
                            NuStrCpy(counter->pickups[counter->pickup_count].position_name, fp->word_buffer_ptr);
                            counter->pickups[counter->pickup_count].use_special = 0;
                        }
                    } else if (NuStrICmp(fp->word_buffer_ptr, "draw_at_obj") == 0) {
                        if (NuFParGetWord(fp) != 0 && NuStrLen(fp->word_buffer_ptr) < 16) {
                            NuStrCpy(counter->pickups[counter->pickup_count].position_name, fp->word_buffer_ptr);
                            counter->pickups[counter->pickup_count].use_special = 1;
                        }
                    }
                }
                if (counter->pickups[counter->pickup_count].level_index != -1)
                    ++counter->pickup_count;
            } else if (NuStrICmp(fp->word_buffer_ptr, "colour") == 0) {
                counter->red = static_cast<u8>(NuFParGetInt(fp));
                counter->green = static_cast<u8>(NuFParGetInt(fp));
                counter->blue = static_cast<u8>(NuFParGetInt(fp));
            } else if (NuStrICmp(fp->word_buffer_ptr, "all_together") == 0) {
                counter->processed_flags |= 1;
            }
        } else if (NuStrICmp(fp->word_buffer_ptr, "supercounter_start") == 0) {
            if (counter_count < 10) {
                counter = &counters[counter_count];
                counter->processed_flags &= ~1u;
                counter->pickup_count = 0;
                counter->red = 0xff;
                counter->green = 0xff;
                counter->blue = 0xff;
                in_counter = 1;
            }
        } else if (NuStrICmp(fp->word_buffer_ptr, "character") == 0) {
            if (NuFParGetWord(fp) == 0)
                continue;
            const i32 character_id = CharIDFromName(fp->word_buffer_ptr);
            if (character_id == -1 || NuFParGetWord(fp) == 0)
                continue;
            if (NuStrICmp(fp->word_buffer_ptr, "player") == 0) {
                if (Area_MissionModelCount == 0) {
                    AreaAddModelListPrivatePc(reinterpret_cast<APICHARACTERMODELLIST *>(AreaPlayerModelsPc),
                                              &Area_PlayerModelCount, 8, character_id, 1, models);
                }
                AreaAddModelListPrivatePc(AREA_STORY_MODELS_PC, &Area_StoryModelCount, 0x30, character_id, 1, models);
            } else if (NuStrICmp(fp->word_buffer_ptr, "resident") == 0) {
                AreaAddModelListPrivatePc(AREA_STORY_MODELS_PC, &Area_StoryModelCount, 0x30, character_id, 1, models);
                AreaAddModelListPrivatePc(reinterpret_cast<APICHARACTERMODELLIST *>(Area_FreePlayModelList),
                                          &Area_FreePlayModelCount, 0x30, character_id, 1, models);
                AreaAddModelListPrivatePc(Area_MissionModelList, &Area_MissionModelCount, 0x30, character_id, 1,
                                          models);
            } else if (NuStrICmp(fp->word_buffer_ptr, "cutscene") == 0) {
                if (Area_StoryModelCount < 0x30 && InModelList(AREA_STORY_MODELS_PC, character_id, NULL) == 0) {
                    AREA_STORY_MODELS_PC[Area_StoryModelCount].model_id = static_cast<i16>(character_id);
                    AREA_STORY_MODELS_PC[Area_StoryModelCount].count = 0;
                    ++Area_StoryModelCount;
                    AREA_STORY_MODELS_PC[Area_StoryModelCount].model_id = -1;
                }
            }
        } else if (NuStrICmp(fp->word_buffer_ptr, "streaming") == 0) {
            i32 story_token;
            if (LevelLoadCount >= 12 || NuFParGetWord(fp) == 0)
                continue;
            story_token = 0;
            if (NuStrICmp(fp->word_buffer_ptr, "story_only") == 0) {
                LevelLoad[LevelLoadCount].flags = (LevelLoad[LevelLoadCount].flags & ~2) | 1;
                if (NuFParGetWord(fp) != 0)
                    story_token = 1;
            }
            if (NuStrICmp(fp->word_buffer_ptr, "freeplay_only") == 0) {
                LevelLoad[LevelLoadCount].flags = (LevelLoad[LevelLoadCount].flags & ~1) | 2;
                if (NuFParGetWord(fp) == 0 && story_token == 0)
                    continue;
            } else {
                // The reference applies both bits after a lone story_only
                // token; retain that sequential token-processing behavior.
                LevelLoad[LevelLoadCount].flags |= 3;
            }
            Level_FindByName(fp->word_buffer_ptr, &level_index);
            if (level_index == -1 || LDataList[level_index].area_index != area)
                continue;
            for (index = 0; index < LevelLoadCount; ++index) {
                if (LevelLoad[index].level == level_index)
                    break;
            }
            if (index != LevelLoadCount)
                continue;
            LevelLoad[LevelLoadCount].level = static_cast<i16>(level_index);
            if (NuFParGetWord(fp) == 0)
                continue;
            Level_FindByName(fp->word_buffer_ptr, &level_index);
            if (level_index == -1 || LDataList[level_index].area_index != area)
                continue;
            LevelLoad[LevelLoadCount].first_level = static_cast<i16>(level_index);
            LevelLoad[LevelLoadCount].second_level = -1;
            if (NuFParGetWord(fp) != 0) {
                Level_FindByName(fp->word_buffer_ptr, &level_index);
                if (level_index != -1 && LDataList[level_index].area_index == area)
                    LevelLoad[LevelLoadCount].second_level = static_cast<i16>(level_index);
            }
            if (LevelLoad[LevelLoadCount].second_level == -1)
                LevelLoad[LevelLoadCount].second_level = LevelLoad[LevelLoadCount].first_level;
            ++LevelLoadCount;
        } else if (area != -1 && NuStrICmp(fp->word_buffer_ptr, "music") == 0) {
            if (NuFParGetWord(fp) != 0) {
                AREA_DATA_PC *area_data = &ADataList[area];
                area_data->area_music = GetMusicIndex(fp->word_buffer_ptr, MusicInfo, -1);
                const i32 quiet = music_man.GetTrackHandle(1, fp->word_buffer_ptr);
                const i32 action = music_man.GetTrackHandle(2, fp->word_buffer_ptr);
                const i32 silence = music_man.GetTrackHandle(32, fp->word_buffer_ptr);
                for (index = 0; index < area_data->level_count; ++index) {
                    if ((LDataList[area_data->levels[index]].flags & 0xe2) == 2) {
                        LDataList[area_data->levels[index]].music_index = area_data->area_music;
                        LDataList[area_data->levels[index]].music_other_index = area_data->area_music;
                        LDataList[area_data->levels[index]].music_tracks[0] = quiet;
                        LDataList[area_data->levels[index]].music_tracks[2] = action;
                        LDataList[area_data->levels[index]].music_tracks[4] = silence;
                        LDataList[area_data->levels[index]].music_tracks[1] = quiet;
                        LDataList[area_data->levels[index]].music_tracks[3] = action;
                        LDataList[area_data->levels[index]].music_tracks[5] = silence;
                    }
                }
            }
        } else if (NuStrICmp(fp->word_buffer_ptr, "AIMessage") == 0) {
            if (NuFParGetWord(fp) != 0) {
                GIZAIMESSAGE *message = CheckGizAIMessage(gizaimessagesys, fp->word_buffer_ptr, NULL);
                if (message != NULL) {
                    message->flags |= 1;
                    while (NuFParGetWord(fp) != 0) {
                        char *output = NuStrIStr(fp->word_buffer_ptr, "output");
                        if (output != NULL) {
                            i32 slot = NuAToI(output + 6);
                            if (NuFParGetWord(fp) != 0 && static_cast<u32>(slot) <= 7) {
                                message->output_values[slot] = static_cast<i8>(NuAToI(fp->word_buffer_ptr));
                                if (slot + 1 > message->output_count)
                                    message->output_count = static_cast<i8>(slot + 1);
                            }
                        }
                    }
                }
            }
        } else if (area != -1) {
            if (NuStrICmp(fp->word_buffer_ptr, "story_coins") == 0) {
                ADataList[area].unknown_008c = NuFParGetInt(fp);
            } else if (NuStrICmp(fp->word_buffer_ptr, "freeplay_coins") == 0) {
                ADataList[area].unknown_0090 = NuFParGetInt(fp);
            } else if (NuStrICmp(fp->word_buffer_ptr, "timetrial_time") == 0) {
                ADataList[area].challenge_trial_time = static_cast<u16>(NuFParGetInt(fp));
            }
        }
    }
    NuFParDestroy(fp);

    if (Area_PlayerModelCount == 0) {
        i32 player_id;
        if (AREA_STORY_MODELS_PC[0].model_id != -1)
            player_id = AREA_STORY_MODELS_PC[0].model_id;
        else if (Area_FreePlayModelList[0].model_id != -1)
            player_id = Area_FreePlayModelList[0].model_id;
        else
            player_id = s[0];
        if (player_id != -1) {
            AreaPlayerModelsPc[0].model_id = static_cast<i16>(player_id);
            AreaPlayerModelsPc[0].count = 0;
            AreaPlayerModelsPc[1].model_id = static_cast<i16>(player_id);
            AreaPlayerModelsPc[1].count = 0;
            AreaPlayerModelsPc[2].model_id = -1;
            Area_PlayerModelCount = 2;
        }
    } else if (Area_PlayerModelCount == 1) {
        AreaPlayerModelsPc[1].model_id = AreaPlayerModelsPc[0].model_id;
        AreaPlayerModelsPc[1].count = 1;
        Area_PlayerModelCount = 2;
    }

    AREA_STORY_MODELS_PC[Area_StoryModelCount].model_id = -1;
    Area_FreePlayModelList[Area_FreePlayModelCount].model_id = -1;
    Area_MissionModelList[Area_MissionModelCount].model_id = -1;
    AreaPlayerModelsPc[Area_PlayerModelCount].model_id = -1;
    for (index = 0; index < Area_PlayerModelCount; ++index) {
        Area_PlayerIDList[index] = AreaPlayerModelsPc[index].model_id;
    }
    Area_PlayerIDList[index] = -1;
    if (counter_count > 0) {
        characterbuffer_ptr.addr = (characterbuffer_ptr.addr + 3) & ~3u;
        ADataList[area].super_counters = (AREA_SUPERCOUNTER_PC *)characterbuffer_ptr.addr;
        PCByteCopy_candidate(ADataList[area].super_counters, counters, counter_count * sizeof(AREA_SUPERCOUNTER_PC));
        ADataList[area].super_counter_count = (u8)counter_count;
        characterbuffer_ptr.addr += counter_count * sizeof(AREA_SUPERCOUNTER_PC);
    }
}

extern "C" void DumpAreaData(i32 mode, i32) {
    if (big_icon_scene != NULL) {
        NuGScnRemove(big_icon_scene);
    }
    big_icon_scene = NULL;
    if (area_scene != NULL) {
        NuGScnRemove(area_scene);
    }
    area_scene = NULL;
    if (vehicle_scene != NULL) {
        NuGScnRemove(vehicle_scene);
    }
    vehicle_scene = NULL;
    if (Customiser_AccessoriesLoaded == 2) {
        Customiser_RestoreModelTextureIDs(Game_Customiser);
    } else if (Customiser_AccessoriesLoaded == 1) {
        Customiser_DumpAccessories(Game_Customiser);
    }
    Customiser_AccessoriesLoaded = 0;
    IconScenes_Dump();
    CharScenes_AreaDump();
    APIDumpCharacterModels(mode);
    Particles_DumpAreaPage();
}

extern "C" {
    extern APICHARACTERMODELLIST Area_MissionModelList[];
    extern APICHARACTERMODELLIST *CurrentCList, *CurrentStoryCList;
    extern i32 Area_PlayerModelCount;
    extern i16 Area_PlayerModelList[8];
}

typedef struct area_loading_clock_reader_pc_s {
    u8 unknown_0000[0x7c20];
    f32 elapsed_time;
} AREA_LOADING_CLOCK_READER_PC;
STATIC_ASSERT(offsetof(AREA_LOADING_CLOCK_READER_PC, elapsed_time) == 0x7c20);
typedef struct area_loading_pad_reader_pc_s {
    u8 unknown_0000[8];
    u32 buttons_pressed;
    u8 unknown_000c[0x54];
} AREA_LOADING_PAD_READER_PC;
STATIC_ASSERT(sizeof(AREA_LOADING_PAD_READER_PC) == 0x60);
STATIC_ASSERT(offsetof(AREA_LOADING_PAD_READER_PC, buttons_pressed) == 8);
STATIC_ASSERT(offsetof(AREA_GLOBALS_PC, field_04) == 4);
extern "C" {

    void __cdecl LoadAreaCharacters(void);
}

extern "C" u8 *Game_LevelSave;

extern "C" i32 __cdecl SuperCounter_AnyCollected(AREA_SUPERCOUNTER_PC *counter, WORLDINFO *world) {
    AREA_SUPERCOUNTER_PICKUP_PC *pickup = counter->pickups;
    i32 index, saved;
    LEVELSAVE_s *save;
    for (index = 0; index < counter->pickup_count; ++index, ++pickup) {
        if (world->area_index != -1) {
            for (saved = 0; saved < AreaGlobals_PC.new_minikit_count; ++saved)
                if (NewMiniPiece[saved].level == pickup->level_index &&
                    NuStrICmp(NewMiniPiece[saved].name, pickup->name) == 0)
                    return 1;
        }
        if (Game_LevelSave != NULL) {
            save = &((LEVELSAVE_s *)Game_LevelSave)[pickup->level_index];
            for (saved = 0; saved < save->minikit_count; ++saved)
                if (NuStrICmp(save->minikit_names[saved], pickup->name) == 0)
                    return 1;
        }
    }
    return 0;
}

/* Append to areas.cpp; native573780316 +5738c0295. */
STATIC_ASSERT(offsetof(WORLDINFO, area) == 0x12c);
STATIC_ASSERT(offsetof(WORLDINFO, level_idx) == 0x11c);
STATIC_ASSERT(offsetof(WORLDINFO, gizmo_sys) == 0x2ac8);
STATIC_ASSERT(offsetof(AREA_DATA_PC, super_counter_count) == 0x7f);
STATIC_ASSERT(offsetof(AREA_DATA_PC, super_counters) == 0x80);
STATIC_ASSERT(offsetof(AREA_SUPERCOUNTER_PICKUP_PC, gizmo) == 0);
STATIC_ASSERT(offsetof(AREA_SUPERCOUNTER_PICKUP_PC, position_gizmo) == 4);
STATIC_ASSERT(offsetof(AREA_SUPERCOUNTER_PICKUP_PC, position_special) == 8);
STATIC_ASSERT(sizeof(AREA_SUPERCOUNTER_PICKUP_PC) == 0x30);
STATIC_ASSERT(sizeof(AREA_SUPERCOUNTER_PC) == 0x1e8);
STATIC_ASSERT(offsetof(AREA_SUPERCOUNTER_PC, red) == 0x1e4);
STATIC_ASSERT(offsetof(AREA_SUPERCOUNTER_PC, green) == 0x1e5);
STATIC_ASSERT(offsetof(AREA_SUPERCOUNTER_PC, blue) == 0x1e6);
STATIC_ASSERT(offsetof(GAMECAMERA, position) == 0x11c);
extern "C" AREA_SUPERCOUNTER_PC *__cdecl SuperCounters_FindPickup(WORLDINFO *world, void *gizmo, NUVEC *position,
                                                                  void *output_pointer) {
    AREA_SUPERCOUNTER_PICKUP_PC **output = (AREA_SUPERCOUNTER_PICKUP_PC **)output_pointer;
    AREA_SUPERCOUNTER_PC *found_counter = NULL;
    AREA_SUPERCOUNTER_PICKUP_PC *found_pickup = NULL;
    if (world->area != NULL && world->area->super_counters != NULL) {
        AREA_SUPERCOUNTER_PC *counter = world->area->super_counters;
        f32 closest = 1000000000.0f;
        for (i32 counter_index = 0; counter_index < world->area->super_counter_count; ++counter_index, ++counter) {
            AREA_SUPERCOUNTER_PICKUP_PC *pickup = counter->pickups;
            for (i32 index = 0; index < counter->pickup_count; ++index, ++pickup) {
                if (pickup->level_index == world->level_idx && pickup->gizmo == gizmo) {
                    if (pickup->position_gizmo == NULL) {
                        if (output != NULL)
                            *output = pickup;
                        return counter;
                    }
                    NUVEC *draw_position = GizmoGetPos((GIZMOSYS_PC *)world->gizmo_sys, pickup->position_gizmo);
                    if (draw_position != NULL) {
                        f32 rounded_distance = NuVecDistSqr(position, draw_position, NULL);
                        f32 distance = rounded_distance;
                        if (closest > distance) {
                            closest = distance;
                            found_counter = counter;
                            found_pickup = pickup;
                        }
                    }
                }
            }
        }
        if (output != NULL)
            *output = found_pickup;
        return found_counter;
    }
    if (output != NULL)
        *output = NULL;
    return NULL;
}
extern "C" void __cdecl SuperCounter_ActivateGizmoPickup(GIZMO_PC *gizmo, GIZMOPICKUP_PC *gizmo_pickup) {
    NUVEC position;
    if (Players_AveragePos(&position, NULL) == 0)
        position = GameCam->position;
    AREA_SUPERCOUNTER_PICKUP_PC *pickup;
    AREA_SUPERCOUNTER_PC *counter = SuperCounters_FindPickup(WORLD, gizmo, &position, &pickup);
    if (counter != NULL) {
        gizmo_pickup->flags &= (u8)~6;
        if (counter->collected_count < counter->pickup_count) {
            ++counter->collected_count;
            if (counter->collected_count == counter->pickup_count)
                gizmo_pickup->flags |= 6;
            NUVEC *message_position;
            if (pickup->position_gizmo != NULL)
                message_position = GizmoGetPos((GIZMOSYS_PC *)WORLD->gizmo_sys, pickup->position_gizmo);
            else if (NuSpecialExistsFn((NUSPECIALHANDLE *)&pickup->position_special) != 0)
                message_position = NuSpecialGetDrawPos((NUSPECIALHANDLE *)&pickup->position_special);
            else
                message_position = &gizmo_pickup->position;
            AddGameMsgCount(message_position, counter->collected_count, counter->pickup_count, counter->red,
                            counter->green, counter->blue, 0.75f);
            GameAudio_PlaySfx(0x53, NULL, 0, 0);
        }
    }
}

STATIC_ASSERT(offsetof(AREA_DATA_PC, super_counter_count) == 0x7f);
STATIC_ASSERT(offsetof(AREA_DATA_PC, super_counters) == 0x80);
STATIC_ASSERT(offsetof(AREA_SUPERCOUNTER_PICKUP_PC, position_special) == 8);
STATIC_ASSERT(sizeof(((AREA_SUPERCOUNTER_PICKUP_PC *)0)->position_special) == 12);
extern "C" void __cdecl SuperCounters_FixUpGizmos(WORLDINFO *world) {
    if (world->area == NULL || world->area->super_counters == NULL)
        return;
    for (i32 counter_index = 0; counter_index < world->area->super_counter_count; ++counter_index) {
        AREA_SUPERCOUNTER_PICKUP_PC *pickup = world->area->super_counters[counter_index].pickups;
        for (i32 index = 0; index < world->area->super_counters[counter_index].pickup_count; ++index, ++pickup) {
            pickup->gizmo = GizmoFindByName(world->gizmo_sys, -1, pickup->name);
            pickup->position_gizmo = NULL;
            memset(pickup->position_special, 0, sizeof(pickup->position_special));
            if (pickup->position_name[0] != '\0') {
                if (pickup->use_special == 0)
                    pickup->position_gizmo = GizmoFindByName(world->gizmo_sys, -1, pickup->position_name);
                else
                    NuSpecialFind(world->current_gscn, reinterpret_cast<NUSPECIALHANDLE *>(pickup->position_special),
                                  pickup->position_name, 1);
            }
        }
    }
}
