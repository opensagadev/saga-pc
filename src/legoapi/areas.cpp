#include "saga.h"
extern "C" {
    void __cdecl MakeFreePlayModelList(i32, i32, i32, i32, i32);
    void __cdecl Customiser_LoadAccessories(CUSTOMISER_LAYERS_PC *, APICHARACTERMODELLIST *);
    extern struct text_crawl_view_pc_s TextCrawlLswPc;
}
extern "C" {
    AREA_DATA_PC *__cdecl Areas_ConfigureList(char *, VARIPTR *, VARIPTR *, i32, i32 *);
    void __cdecl NewAreaMusicChanges(void);
    void __cdecl ClearTakeOverObjectSys(void);
    void __cdecl Door_Reset(void);
    extern void *Door_Last;
}
#include "saga.h"
#include <string.h>

typedef struct area_save_restore_pc_s {
    union {
        u32 values[3];
        struct {
            u8 unknown_0000[5];
            u8 phase;
            u8 unknown_0006[6];
        };
    };
} AREA_SAVE_RESTORE_PC;
STATIC_ASSERT(sizeof(AREA_SAVE_RESTORE_PC) == 0x0c);
STATIC_ASSERT(offsetof(AREA_SAVE_RESTORE_PC, phase) == 5);

typedef struct game_save_restore_pc_s {
    u8 unknown_0000[0x11];
    u8 level_save[0x781b];
    AREA_SAVE_RESTORE_PC area_save[72];
    u8 unknown_7b8c[0x70];
    u32 initial_store_pack_flags;
} GAME_SAVE_RESTORE_PC;
STATIC_ASSERT(offsetof(GAME_SAVE_RESTORE_PC, level_save) == 0x11);
STATIC_ASSERT(offsetof(GAME_SAVE_RESTORE_PC, area_save) == 0x782c);
STATIC_ASSERT(offsetof(GAME_SAVE_RESTORE_PC, initial_store_pack_flags) == 0x7bfc);

extern GAMESAVE BackupGame;
extern u32 areaSuitBits;

void __cdecl ClearAreaProgress(i32 area_index, i32 restore) {
    GAME_SAVE_RESTORE_PC *game = (GAME_SAVE_RESTORE_PC *)&Game;
    GAME_SAVE_RESTORE_PC *backup = (GAME_SAVE_RESTORE_PC *)&BackupGame;
    AREA_DATA_PC *area;
    i32 index;

    for (index = 0; index < 12; ++index) {
        ClearLevelProgress(index, NULL);
    }
    areaSuitBits = game->initial_store_pack_flags;
    if (restore == 0 || area_index == -1) {
        return;
    }
    AREA_GLOBALS_PROGRESS_FLAGS_PC = 0;
    game->area_save[area_index] = backup->area_save[area_index];
    area = &ADataList[area_index];
    for (index = 0; index < area->level_count; ++index) {
        i32 level_index = area->levels[index];
        memcpy(&game->level_save[level_index * 0x54], &backup->level_save[level_index * 0x54], 0x54);
    }
}

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
    AREA_GIZMOSET_PC *sets;

    if (types == NULL || name == NULL || gizmo_sys == NULL) {
        return NULL;
    }

    sets = ((AREA_GIZMOSYS_PREFIX_PC *)gizmo_sys)->sets;
    if (type_id >= 0 && type_id <= types->count) {
        AREA_GIZMOTYPE_PC *type;
        AREA_GIZMOSET_PC *set;
        AREA_GIZMO_PC *gizmo;
        i32 index;

        type = &types->types[type_id];
        set = &sets[type_id];
        if (type->get_gizmo_name == NULL) {
            return NULL;
        }
        gizmo = (AREA_GIZMO_PC *)set->gizmos;
        for (index = 0; index < set->count; ++index, ++gizmo) {
            if (NuStrICmp(type->get_gizmo_name(gizmo), name) == 0) {
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
        set = sets;
        for (type_index = 0; type_index < types->count; ++type_index, ++type, ++set) {
            AREA_GIZMO_PC *gizmo;
            i32 gizmo_index;

            if (type->get_gizmo_name == NULL) {
                continue;
            }
            gizmo = (AREA_GIZMO_PC *)set->gizmos;
            for (gizmo_index = 0; gizmo_index < set->count; ++gizmo_index, ++gizmo) {
                if (NuStrICmp(type->get_gizmo_name(gizmo), name) == 0) {
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
    LEVELDATA_PC *result;
    i32 area_index;
    i32 area_level_index;

    level = &LDataList[level_index];
    area_index = level->area_index;
    area_level_index = level->area_level_index;
    result = level;
    if (area_index != -1) {
        AREA_DATA_PC *area;

        area = &ADataList[area_index];
        if (area_level_index < area->level_count - 1) {
            result = &LDataList[area->levels[area_level_index]];
            if ((result->flags & 0xe0) != 0) {
                i32 index;

                for (index = area_level_index; index != area->level_count - 2; ++index) {
                    LEVELDATA_PC *candidate;

                    candidate = &LDataList[area->levels[index + 1]];
                    if ((candidate->flags & 0xe0) == 0) {
                        return candidate;
                    }
                }
                return level;
            }
        }
    }
    return result;
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

void __cdecl Cheat_SetArea(i32 cheat, i32 area_index) {
    CHEAT_PC *cheats;

    if (cheat < 0 || cheat >= CheatSystemPtr->count || area_index < 0 || area_index >= AREACOUNT) {
        return;
    }

    cheats = CheatSystemPtr->cheats;
    cheats[cheat].area_index = (i8)area_index;
}

AREA_DATA_PC *__cdecl Areas_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end, i32 max_count,
                                          i32 *count_out) {
    NUFPAR_PC *parser;
    AREA_DATA_PC *area_base;
    AREA_DATA_PC *area;
    i32 area_count;
    i32 in_area;
    u32 aligned_address;
    char *word;

    parser = NuFParCreate(path);
    if (parser == NULL) {
        STUBBED_NAMED("Areas_ConfigureList: unresolved ret-only PC failure helper "
                      "at 0x00572ad0");
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
        word = parser->word_buffer_ptr;
        if (word[0] == '\0') {
            continue;
        }

        if (in_area != 0) {
            if (NuStrICmp(word, AreaConfigKeyword_area_end) == 0) {
                in_area = 0;
                if (area->dir[0] != '\0' && area->file[0] != '\0' && area->level_count != 0 &&
                    (area->flags & 0x40) == 0) {
                    ++area;
                    ++area_count;
                }
            } else if (NuStrICmp(word, LevelConfigKeywordDir) == 0) {
                if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buffer_ptr) < 0x40) {
                    NuStrCpy(area->dir, parser->word_buffer_ptr);
                }
            } else if (NuStrICmp(word, LevelConfigKeywordFile) == 0) {
                if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buffer_ptr) < 0x20) {
                    NuStrCpy(area->file, parser->word_buffer_ptr);
                }
            } else if (NuStrICmp(word, AreaConfigKeyword_level) == 0) {
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
            } else if (NuStrICmp(word, AreaConfigKeyword_single_buffer) == 0) {
                area->flags |= 0x0008;
            } else if (NuStrICmp(word, AreaConfigKeyword_minikit) == 0) {
                area->flags |= 0x0010;
                if (NuFParGetWord(parser) != 0) {
                    area->minikit_id = (i16)CharIDFromName(parser->word_buffer_ptr);
                }
            } else if (NuStrICmp(word, AreaConfigKeyword_test_area) == 0) {
                area->flags |= 0x0040;
            } else if (NuStrICmp(word, AreaConfigKeyword_hub_area) == 0) {
                area->flags |= 0x0020;
            } else if (NuStrICmp(word, AreaConfigKeyword_override_things_scene) == 0) {
                area->flags |= 0x0400;
            } else if (NuStrICmp(word, AreaConfigKeyword_vehicle_area) == 0) {
                area->flags |= 0x0001;
            } else if (NuStrICmp(word, AreaConfigKeyword_ending_area) == 0) {
                area->flags |= 0x0002;
            } else if (NuStrICmp(word, AreaConfigKeyword_bonus_area) == 0) {
                area->flags |= 0x0004;
            } else if (NuStrICmp(word, AreaConfigKeyword_super_bonus_area) == 0) {
                area->flags |= 0x0104;
            } else if (NuStrICmp(word, AreaConfigKeyword_nocharactercollision) == 0 ||
                       NuStrICmp(word, AreaConfigKeyword_no_character_collision) == 0 ||
                       NuStrICmp(word, AreaConfigKeyword_nocharactercollisions) == 0 ||
                       NuStrICmp(word, AreaConfigKeyword_no_character_collisions) == 0) {
                area->flags |= 0x0080;
            } else if (NuStrICmp(word, AreaConfigKeyword_nopickupgravity) == 0) {
                area->flags |= 0x0200;
            } else if (NuStrICmp(word, AreaConfigKeyword_no_gold_brick) == 0) {
                area->flags |= 0x0800;
            } else if (NuStrICmp(word, AreaConfigKeyword_no_completion_points) == 0) {
                area->flags |= 0x2000;
            } else if (NuStrICmp(word, AreaConfigKeyword_no_freeplay) == 0) {
                area->flags |= 0x1000;
            } else if (NuStrICmp(word, AreaConfigKeyword_name_id) == 0) {
                area->name_id = (i16)NuFParGetInt(parser);
            } else if (NuStrICmp(word, AreaConfigKeyword_text_id) == 0) {
                area->text_id = (i16)NuFParGetInt(parser);
                if (NuFParGetWord(parser) != 0) {
                    i32 text_id_value;

                    text_id_value = NuAToI(parser->word_buffer_ptr);
                    area->text_id_value = (u8)(text_id_value < 0 ? -text_id_value : text_id_value);
                }
            } else if (NuStrICmp(word, AreaConfigKeyword_timetrial_time) == 0) {
                area->challenge_trial_time = (u16)NuFParGetInt(parser);
            } else if (NuStrICmp(word, AreaConfigKeyword_redbrick_cheat) == 0 ||
                       NuStrICmp(word, AreaConfigKeyword_redbrick_extra) == 0) {
                if (NuFParGetWord(parser) != 0) {
                    area->cheat = (i8)Cheat_FindByName(parser->word_buffer_ptr);
                }
            }
            continue;
        }

        if (NuStrICmp(word, AreaConfigKeyword_area_start) == 0 && area_count < max_count) {
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
    if (area_count == 0) {
        STUBBED_NAMED("Areas_ConfigureList: unresolved ret-only PC zero-record "
                      "helper at 0x00572ad0");
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

typedef struct level_progress_flags_pc_s {
    SCENEPROGRESS scene_progress;
    u32 flags;
} LEVEL_PROGRESS_FLAGS_PC;
STATIC_ASSERT(offsetof(LEVEL_PROGRESS_FLAGS_PC, flags) == 0x2800);

typedef struct player_progress_hp_pc_s {
    u8 unknown_0000[8];
    u8 hitpoints;
    u8 unknown_0009[7];
} PLAYER_PROGRESS_HP_PC;
STATIC_ASSERT(sizeof(PLAYER_PROGRESS_HP_PC) == 0x10);
STATIC_ASSERT(offsetof(PLAYER_PROGRESS_HP_PC, hitpoints) == 8);

extern void *Door_Last;
extern AREA_DATA_PC *HOTHBATTLE_ADATA;
void __cdecl NewAreaMusicChanges(void);
void __cdecl ClearTakeOverObjectSys(void);
void __cdecl Door_Reset(void);

void __cdecl NewArea_ResetGlobalsPC(void) {
    GAME_SAVE_RESTORE_PC *game = (GAME_SAVE_RESTORE_PC *)&Game;
    i32 phase = Area == -1 ? 0 : game->area_save[Area].phase;

    AreaGlobals_PC.new_minikit_count = 0;
    AreaGlobals_PC.area_phase = phase;
    AreaGlobals_PC.progress_flags = phase;
    Door_UseCutCam = 0;
    AreaGlobals_PC.field_00 = 0;
    AreaGlobals_PC.field_1c = 0;
    AreaGlobals_PC.field_08 = 0;
    AreaGlobals_PC.field_18 = 0;
    AreaGlobals_PC.arcade_player_kills[0] = 0;
    AreaGlobals_PC.arcade_player_kills[1] = 0;
    AreaGlobals_PC.arcade_ai_kills[0] = 0;
    AreaGlobals_PC.arcade_ai_kills[1] = 0;
    BuildUpTotal = 0;
    BuildUpDone = 0;
    if (WORLD->area == HOTHBATTLE_ADATA) {
        ResetMinikitCounter();
    }
}

void __cdecl NewArea(void) {
    LEVEL_PROGRESS_FLAGS_PC *progress;
    i32 i;

    LSW1 = 0;
    LSW2 = 0;
    if (StartupGameFlag1 != 0 && WORLD != NULL && WORLD->area != NULL && WORLD->area->episode_index >= 0) {
        if (WORLD->area->episode_index < 3) {
            LSW1 = 1;
            HIGHJUMPHEIGHT = 1.14f;
            goto have_jump;
        }
        LSW2 = 1;
    }
    HIGHJUMPHEIGHT = Arcade != 0 ? 1.14f : 0.75f;
have_jump:
    ClearAreaProgress(Area, 0);
    progress = (LEVEL_PROGRESS_FLAGS_PC *)WORLD->level_progress;
    if (progress != NULL && (progress->flags & 1) == 0) {
        memcpy(&progress->scene_progress, &WORLD->scene_progress, sizeof(progress->scene_progress));
        progress->flags |= 1;
    }
    SuperCounters_Reset(Area);
    NewArea_ResetGlobalsPC();
    for (i = 0; i < 8; ++i) {
        GAMEOBJECT *player = (GAMEOBJECT *)Player[i];
        ((PLAYER_PROGRESS_HP_PC *)PlayerProgress)[i].hitpoints = (u8)DEFAULT_PLAYERHITPOINTS;
        if (player != NULL) {
            player->current_hp = (i8)player->field_107e;
        }
    }
    ResetTimer(&AreaTimer, 0.0f);
    memcpy(&BackupGame, &Game, sizeof(Game));
    NewAreaMusicChanges();
    VehicleAreaRememberSpeed = 0.0f;
    ClearTakeOverObjectSys();
    BonusScore[0] = OldBonusScore[0];
    BonusScore[1] = OldBonusScore[1];
    BonusCoinTotal = 0;
    Door_Last = NULL;
    Door_Reset();
    BombGenerator_PlayerBomb[0] = NULL;
    BombGenerator_PlayerBomb[1] = NULL;
    LevelChange = 1;
    Lap = 1;
}

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
    Area_StoryModelCount = 0;
    AreaPlayerModelsPc[0].model_id = -1;
    AREA_STORY_MODELS_PC[0].model_id = -1;
    Area_FreePlayModelCount = 0;
    Area_FreePlayModelList[0].model_id = -1;
    Area_MissionModelCount = 0;
    Area_MissionModelList[0].model_id = -1;
    LevelLoad[0].level = -1;
    LevelLoadCount = 0;

    AreaMusic = -1;
    if (area != -1) {
        AreaMusic = ADataList[area].area_music;
        ADataList[area].super_counters = NULL;
        ADataList[area].super_counter_count = 0;
    }

    if (Mission_Active(MissionSys) != NULL && ((AREA_MISSION_READER_PC *)MissionSys)->character_count != 0) {
        i16 mission_characters[64];
        const i32 count = ((AREA_MISSION_READER_PC *)MissionSys)->character_count;
        PCByteCopy_candidate(mission_characters, ((AREA_MISSION_READER_PC *)MissionSys)->character_ids,
                             count * sizeof(i16));
        if (count > 2) {
            for (i32 swap = 0; swap < count * 3; ++swap) {
                const i32 first = qrand() / (1 + (0xffff / count));
                const i32 second = (qrand() / (0xffff / (count - 1) + 1) + first) % count;

                mission_characters[first] = mission_characters[second];
                mission_characters[second] = ((const i16)mission_characters[first]);
            }
        }
        APICHARACTERMODELLIST *player_models = reinterpret_cast<APICHARACTERMODELLIST *>(AreaPlayerModelsPc);
        for (i32 index = 0; index < count; ++index) {
            if (Area_MissionModelCount < 0x30) {
                Area_MissionModelList[Area_MissionModelCount].model_id = (i16)(mission_characters[index]);
                Area_MissionModelList[Area_MissionModelCount].count = (i16)(1);
                ++Area_MissionModelCount;
            }
            if (Area_PlayerModelCount <= 7) {
                player_models[Area_PlayerModelCount].model_id = (i16)(mission_characters[index]);
                player_models[Area_PlayerModelCount].count = (i16)(1);
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
        AreaPlayerModelsPc[0].model_id = s[0];
        AreaPlayerModelsPc[0].count = 1;
        AreaPlayerModelsPc[1].model_id = s[1];
        AreaPlayerModelsPc[1].count = 1;
        AreaPlayerModelsPc[2].model_id = -1;
        Area_PlayerIDList[0] = s[0];
        Area_PlayerIDList[1] = s[1];
        Area_PlayerIDList[2] = -1;
        Area_PlayerModelCount = 2;
        AREA_STORY_MODELS_PC[0].model_id = (i16)(s[0]);
        AREA_STORY_MODELS_PC[0].count = (i16)(1);
        AREA_STORY_MODELS_PC[1].model_id = (i16)(s[1]);
        AREA_STORY_MODELS_PC[1].count = (i16)(1);
        AREA_STORY_MODELS_PC[2].model_id = -1;
        Area_StoryModelCount = 2;
        return;
    }

    AREA_SUPERCOUNTER_PC counters[10];
    AREA_SUPERCOUNTER_PC *counter = NULL;
    i32 counter_count = 0;
    bool in_counter = false;
    while (NuFParGetLine(fp) != 0) {
        while (true) {
            if (NuFParGetWord(fp) == 0 || fp->word_buffer_ptr[0] == '\0') {
                break;
            }
            if (in_counter) {

                if (NuStrICmp(fp->word_buffer_ptr, "supercounter_end") == 0) {
                    if (counter->pickup_count != 0) {
                        ++counter_count;
                        in_counter = false;
                    }
                } else if (NuStrICmp(fp->word_buffer_ptr, "pickup") == 0) {
                    if (counter->pickup_count >= 10 || NuFParGetWord(fp) == 0 || NuStrLen(fp->word_buffer_ptr) > 7)
                        break;
                    AREA_SUPERCOUNTER_PICKUP_PC *pickup = &counter->pickups[counter->pickup_count];
                    NuStrCpy(pickup->name, fp->word_buffer_ptr);
                    pickup->level_index = -1;
                    pickup->position_name[0] = '\0';
                    pickup->use_special = 1;
                    while (NuFParGetWord(fp) != 0) {
                        if (NuStrICmp(fp->word_buffer_ptr, "in_level") == 0) {
                            i32 level_index;
                            if (NuFParGetWord(fp) != 0 && Level_FindByName(fp->word_buffer_ptr, &level_index) != NULL)
                                pickup->level_index = static_cast<i16>(level_index);
                        } else if (NuStrICmp(fp->word_buffer_ptr, "draw_at_gizmo") == 0) {
                            if (NuFParGetWord(fp) != 0 && NuStrLen(fp->word_buffer_ptr) < 16) {
                                NuStrCpy(pickup->position_name, fp->word_buffer_ptr);
                                pickup->use_special = 0;
                            }
                        } else if (NuStrICmp(fp->word_buffer_ptr, "draw_at_obj") == 0) {
                            if (NuFParGetWord(fp) != 0 && NuStrLen(fp->word_buffer_ptr) <= 15) {
                                NuStrCpy(pickup->position_name, fp->word_buffer_ptr);
                                pickup->use_special = 1;
                            }
                        }
                    }
                    if (pickup->level_index != -1)
                        ++counter->pickup_count;
                } else if (NuStrICmp(fp->word_buffer_ptr, "colour") == 0) {
                    counter->red = static_cast<u8>(NuFParGetInt(fp));
                    counter->green = static_cast<u8>(NuFParGetInt(fp));
                    counter->blue = static_cast<u8>(NuFParGetInt(fp));
                } else if (NuStrICmp(fp->word_buffer_ptr, "all_together") == 0) {
                    counter->processed_flags |= 1;
                }
                break;
            } else {
                if (NuStrICmp(fp->word_buffer_ptr, "supercounter_start") == 0) {
                    if (counter_count < 10) {
                        counter = &counters[counter_count];
                        counter->processed_flags &= ~1u;
                        counter->pickup_count = 0;
                        counter->red = counter->green = counter->blue = 0xff;
                        in_counter = true;
                    }
                    break;
                }
                if (NuStrICmp(fp->word_buffer_ptr, "character") == 0) {
                    if (NuFParGetWord(fp) == 0) {
                        break;
                    }
                    const i32 character_id = CharIDFromName(fp->word_buffer_ptr);
                    if (character_id == -1 || NuFParGetWord(fp) == 0) {
                        break;
                    }

                    if (NuStrICmp(fp->word_buffer_ptr, "player") == 0) {
                        if (Area_MissionModelCount == 0) {
                            AreaAddModelListPrivatePc(reinterpret_cast<APICHARACTERMODELLIST *>(AreaPlayerModelsPc),
                                                      &Area_PlayerModelCount, 8, character_id, 1, models);
                        }
                        AreaAddModelListPrivatePc(AREA_STORY_MODELS_PC, &Area_StoryModelCount, 0x30, character_id, 1,
                                                  models);
                    } else if (NuStrICmp(fp->word_buffer_ptr, "resident") == 0) {
                        AreaAddModelListPrivatePc(AREA_STORY_MODELS_PC, &Area_StoryModelCount, 0x30, character_id, 1,
                                                  models);
                        AreaAddModelListPrivatePc(reinterpret_cast<APICHARACTERMODELLIST *>(Area_FreePlayModelList),
                                                  &Area_FreePlayModelCount, 0x30, character_id, 1, models);
                        AreaAddModelListPrivatePc(Area_MissionModelList, &Area_MissionModelCount, 0x30, character_id, 1,
                                                  models);
                    } else if (NuStrICmp(fp->word_buffer_ptr, "cutscene") == 0) {
                        AreaAddModelListPrivatePc(AREA_STORY_MODELS_PC, &Area_StoryModelCount, 0x30, character_id, 0,
                                                  models);
                    }
                    break;
                }
                if (NuStrICmp(fp->word_buffer_ptr, "streaming") == 0) {
                    if (LevelLoadCount >= 12 || NuFParGetWord(fp) == 0)
                        break;
                    AREA_LEVEL_LOAD_PC *load = &LevelLoad[LevelLoadCount];
                    bool story_token = false;
                    if (NuStrICmp(fp->word_buffer_ptr, "story_only") == 0) {
                        load->flags = ((~2) & load->flags) | 1;
                        story_token = NuFParGetWord(fp) != 0;
                    }
                    if (NuStrICmp(fp->word_buffer_ptr, "freeplay_only") == 0) {
                        load->flags = (load->flags & ~1) | 2;
                        if (NuFParGetWord(fp) == 0 && !story_token)
                            break;
                    } else {
                        // The reference applies both bits after a lone story_only
                        // token; retain that sequential token-processing behavior.
                        load->flags |= 3;
                    }
                    i32 level_index;
                    Level_FindByName(fp->word_buffer_ptr, &level_index);
                    if (-1 == level_index || LDataList[level_index].area_index != area)
                        break;
                    i32 existing = 0;
                    while (LevelLoadCount > existing && LevelLoad[existing].level != level_index)
                        ++existing;
                    if (existing != LevelLoadCount)
                        break;
                    load->level = static_cast<i16>(level_index);
                    if (NuFParGetWord(fp) == 0)
                        break;
                    Level_FindByName(fp->word_buffer_ptr, &level_index);
                    if (level_index == -1 || LDataList[level_index].area_index != area)
                        break;
                    load->first_level = static_cast<i16>(level_index);
                    load->second_level = -1;
                    if (NuFParGetWord(fp) != 0) {
                        Level_FindByName(fp->word_buffer_ptr, &level_index);
                        if (level_index != -1 && LDataList[level_index].area_index == area)
                            load->second_level = static_cast<i16>(level_index);
                    }
                    if (load->second_level == -1)
                        load->second_level = load->first_level;
                    ++LevelLoadCount;
                    break;
                }
                if (area != -1 && NuStrICmp(fp->word_buffer_ptr, "music") == 0) {
                    if (NuFParGetWord(fp) != 0) {
                        AREA_DATA_PC *area_data = &ADataList[area];
                        area_data->area_music = GetMusicIndex(fp->word_buffer_ptr, MusicInfo, -1);
                        const i32 quiet = music_man.GetTrackHandle(1, fp->word_buffer_ptr);
                        const i32 action = music_man.GetTrackHandle(2, fp->word_buffer_ptr);
                        const i32 silence = music_man.GetTrackHandle(32, fp->word_buffer_ptr);
                        for (i32 index = 0; index < area_data->level_count; ++index) {
                            const i32 level_index = area_data->levels[index];
                            LEVELDATA_PC *level = &LDataList[level_index];
                            if ((level->flags & 0xe2) == 2) {
                                level->music_index = level->music_other_index = area_data->area_music;
                                level->music_tracks[0] = level->music_tracks[1] = quiet;
                                level->music_tracks[2] = level->music_tracks[3] = action;
                                level->music_tracks[4] = level->music_tracks[5] = silence;
                            }
                        }
                    }
                    break;
                }
                if (area != -1 && NuStrICmp(fp->word_buffer_ptr, "story_coins") == 0) {
                    ADataList[area].unknown_008c = NuFParGetInt(fp);
                    break;
                }
                if (area != -1 && NuStrICmp(fp->word_buffer_ptr, "freeplay_coins") == 0) {
                    ADataList[area].unknown_0090 = NuFParGetInt(fp);
                    break;
                }
                if (area != -1 && NuStrICmp(fp->word_buffer_ptr, "timetrial_time") == 0) {
                    ADataList[area].challenge_trial_time = static_cast<u16>(NuFParGetInt(fp));
                    break;
                }
                if (NuStrICmp(fp->word_buffer_ptr, "AIMessage") == 0) {
                    if (NuFParGetWord(fp) != 0) {
                        GIZAIMESSAGE *message = CheckGizAIMessage(gizaimessagesys, fp->word_buffer_ptr, NULL);
                        if (message != NULL) {
                            message->flags |= 1;
                            while (NuFParGetWord(fp) != 0) {
                                char *output = NuStrIStr(fp->word_buffer_ptr, "output");
                                if (output != NULL) {
                                    i32 index = NuAToI(output + 6);
                                    if (NuFParGetWord(fp) != 0 && static_cast<u32>(index) < 8) {
                                        message->output_values[index] = static_cast<i8>(NuAToI(fp->word_buffer_ptr));
                                        if (index >= message->output_count)
                                            message->output_count = static_cast<i8>(index + 1);
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
            }

            break;
        }
    }
    NuFParDestroy(fp);

    APICHARACTERMODELLIST *player_models = reinterpret_cast<APICHARACTERMODELLIST *>(AreaPlayerModelsPc);
    if (Area_PlayerModelCount == 0) {
        i16 player_id = AREA_STORY_MODELS_PC[0].model_id;
        if (player_id == -1) {
            player_id = reinterpret_cast<APICHARACTERMODELLIST *>(Area_FreePlayModelList)[0].model_id;
        }
        if (player_id == -1) {
            player_id = s[0];
        }
        if (player_id != -1) {
            player_models[0].model_id = (i16)(player_id);
            player_models[0].count = (i16)(0);
            player_models[1].model_id = (i16)(player_id);
            player_models[1].count = (i16)(0);
            Area_PlayerModelCount = 2;
        }
    } else if (Area_PlayerModelCount == 1) {
        player_models[1].model_id = (i16)(player_models[0].model_id);
        player_models[1].count = (i16)(1);
        Area_PlayerModelCount = 2;
    }

    player_models[Area_PlayerModelCount].model_id = -1;
    AREA_STORY_MODELS_PC[Area_StoryModelCount].model_id = -1;
    reinterpret_cast<APICHARACTERMODELLIST *>(Area_FreePlayModelList)[Area_FreePlayModelCount].model_id = -1;
    Area_MissionModelList[Area_MissionModelCount].model_id = -1;
    for (i32 i = 0; i < Area_PlayerModelCount; ++i) {
        Area_PlayerIDList[i] = player_models[i].model_id;
    }
    Area_PlayerIDList[Area_PlayerModelCount] = -1;
    if (counter_count >= 1) {
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
    extern APICHARACTERMODELLIST FreePlayModelList[];
    extern APICHARACTERMODELLIST Area_MissionModelList[];
    extern APICHARACTERMODELLIST *CurrentCList, *CurrentStoryCList;
    extern i32 Area_PlayerModelCount;
    extern i16 Area_PlayerModelList[8];
    extern i32 CharacterDataLoad, AreaDataLoaded, loadareadata_loadlevel;
    extern i16 id_DEFAULTCHARACTER[2];
    NUGSCN *__cdecl NuGScnRead(VARIPTR *, VARIPTR, char *);
    void __cdecl WorldInfo_StreamLevel(BGPROCINFO *);

    extern AREA_DATA_PC *ANEWHOPE_ADATA;

    /* Original area.cpp helper: native EDI area, ESI list output, one stack story
     * output. */
    static void AreaSelectModelListsPrivatePc(i32 area, APICHARACTERMODELLIST **list, APICHARACTERMODELLIST **story) {
        if (StartupGameFlag1 != 0 && HUB_ADATA != NULL && area == HUB_ADATA->index)
            *list = Hub_ModelList;
        else if (FreePlay != 0)
            *list = FreePlayModelList;
        else if (Mission_Active(0) != 0)
            *list = Area_MissionModelList;
        else
            *list = Area_StoryModelListPc;
        if (StartupGameFlag1 != 0 && HUB_ADATA != NULL && area == HUB_ADATA->index)
            *story = Hub_ModelList;
        else if (Mission_Active(0) != 0)
            *story = Area_MissionModelList;
        else
            *story = Area_StoryModelListPc;
    }

    static void __cdecl LoadAreaData(BGPROCINFO *bg_info) {
        char path[256];
        i32 area = Area;
        i32 level = Level;
        APICHARACTERMODELLIST *list = NULL;
        APICHARACTERMODELLIST *story = NULL;
        NUGSCN *scene;
        (void)bg_info;
        if (area != -1) {
            if (area != last_area) {
                characterbuffer_ptr.addr = (characterbuffer_ptr.addr + 3) & ~3u;
                NuStrCpy(path, "levels\\");
                NuStrCat(path, ADataList[area].dir);
                NuStrCat(path, "\\");
                NuStrCat(path, ADataList[area].file);
                NuStrCat(path, ".gsc");
                StartupDatsSelectPc((i32)(i8)ADataList[area].episode_index + 1);
                area_scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, path);
                StartupDatsSelectPc(0);
                if (area_scene != NULL && area_scene->display_list != NULL)
                    area_scene->display_list->render_flags |= 0x10;
            }
            if ((ADataList[area].flags & 5) != 0 && area != last_area) {
                characterbuffer_ptr.addr = (characterbuffer_ptr.addr + 3) & ~3u;
                scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, "stuff\\vehicle_things.gsc");
                vehicle_scene = scene;
                if (scene != NULL && scene->display_list != NULL)
                    scene->display_list->render_flags |= 0x10;
            }
        }
        if (HUB_ADATA != NULL && area == HUB_ADATA->index && area != last_area) {
            characterbuffer_ptr.addr = (characterbuffer_ptr.addr + 0x3f) & ~0x3fu;
            scene = NuGScnRead(&characterbuffer_ptr, characterbuffer_end, "stuff\\icons\\starwars_icons_all.gsc");
            big_icon_scene = scene;
            if (scene != NULL && scene->display_list != NULL)
                scene->display_list->render_flags |= 0x10;
        }
        if (makefreeplaymodellist != 0) {
            if ((ADataList[area].flags & 1) != 0 || (Area_PlayerModelCount >= 2 && BonusArea == 0))
                MakeFreePlayModelList(AreaPlayerModelsPc[0].model_id, AreaPlayerModelsPc[1].model_id, area, level, 0);
            else
                MakeFreePlayModelList(id_DEFAULTCHARACTER[0], id_DEFAULTCHARACTER[1], area, level, 0);
            story = Area_StoryModelListPc;
            list = FreePlayModelList;
            makefreeplaymodellist = 0;
        } else if (area != -1 && area == last_area) {
            CharacterDataLoad = 2;
            goto done;
        } else {
            AreaSelectModelListsPrivatePc(area, &list, &story);
            if (list == NULL) {
                CharacterDataLoad = 2;
                goto done;
            }
        }
        CurrentCList = list;
        IconScenes_Load(list, 1, &characterbuffer_ptr, &characterbuffer_end);
        Customiser_SetAnimsToLoad(CharacterCustomiser, HUB_ADATA != NULL && area == HUB_ADATA->index);
        if (LOSTTEMPLE_ADATA != NULL && area == LOSTTEMPLE_ADATA->index) {
            STUBBED_NAMED(Particles_LoadAreaPage); /* Native 48b398: stuff\\char_indy.ptl. */
        } else if (area != -1 && (ADataList[area].flags & 0x62) == 0 &&
                   (((i8)ADataList[area].episode_index >= 0 && (i8)ADataList[area].episode_index < 3) ||
                    (ANEWHOPE_ADATA != NULL && area == ANEWHOPE_ADATA->index) ||
                    (PODSPRINT_ADATA != NULL && area == PODSPRINT_ADATA->index) ||
                    (BONUS_GUNSHIP_ADATA != NULL && area == BONUS_GUNSHIP_ADATA->index))) {
            STUBBED_NAMED(Particles_LoadAreaPage); /* Native 48b398: stuff\\char_lsw1.ptl. */
        }
        nopakfile = CHARPAK == 0;
        APIResetCharacterRemap();
        CharacterDataLoad = 1;
        GameLoadCharacterModels(list, 0, &characterbuffer_ptr, &characterbuffer_end, 1, area);
        CharacterDataLoad = 2;
        CharScenes_AreaLoad(list, &characterbuffer_ptr, characterbuffer_end);
        if (HUB_ADATA == NULL || area != HUB_ADATA->index) {
            Customiser_LoadAccessories(CharacterCustomiser, list);
        }
        Customiser_ResetModelTextureIDs(CharacterCustomiser);
    done:
        if (story != NULL)
            CurrentStoryCList = story;
        if (loadareadata_loadlevel != 0) {
            next_level = Level;
            abort_load = 0;
            WorldInfo_StreamLevel(NULL);
        }
        AreaDataLoaded = 1;
    }
}

extern "C" void ClearUpAreaData() {
    i32 object_index;

    if (HIGHGAMEOBJECT > 0) {
        for (object_index = 0; object_index < HIGHGAMEOBJECT; object_index++) {
            GAMEOBJECT *obj = &Obj[object_index];
            if ((obj->flags_1fc_half & 1) != 0) {
                FreeTorpedoPacket((TORPEDOPACKET_PC **)&obj->torpedo);
                RemoveGameObject(&Obj[object_index], 1);
            }
        }
    }
    if (Area != -1 && Area == last_area) {
        return;
    }
    if (Customiser_AccessoriesLoaded == 2) {
        Customiser_RestoreModelTextureIDs(CharacterCustomiser);
    } else if (Customiser_AccessoriesLoaded == 1) {
        Customiser_DumpAccessories(CharacterCustomiser);
    }
    Customiser_AccessoriesLoaded = 0;
    APIDumpCharacterModels(0);
    IconScenes_Dump();
    CharScenes_AreaDump();
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
    Particles_DumpAreaPage();
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
    extern i32 netnewgame, textcrawlactive;
    static f32 LoadTime;
    static f32 LOADWAITTIME;
    static f32 LoadWait;
    extern f32 AreaCrawlMaximumTimePc, AreaCrawlSoundWaitTimePc;
    extern i32 loadareacharacters_no_backdrop_reset;
    extern i32 AreaCrawlMusicEnabledPc;
    extern f32 RenderBackdropTopRPc, RenderBackdropTopGPc, RenderBackdropTopBPc;
    extern f32 RenderBackdropBottomRPc, RenderBackdropBottomGPc, RenderBackdropBottomBPc;
    extern i16 tTOUCHTOSTART;

    extern f32 AreaCrawlPromptYPc, AreaCrawlPromptScalePc;
    i32 __cdecl AreaGetKeyboardOverridePc(void);
    i32 __cdecl NuSound3LoadingSfx(void);
    i32 __cdecl NuKeyboard_db(i32);
    void __cdecl ReadPads(void);
    void __cdecl MusicClearAll(void);
    void __cdecl SoundKillAll(void);
    void __cdecl NeedScreenGrab(i32);
    void __cdecl GamePlayMusic(LEVELDATA_PC *, i32, OPTIONSSAVE *);
    void __cdecl Area_Configure(i32, i32, AREA_EXTRA_MODEL_PC *, i16 *);
    extern AREA_EXTRA_MODEL_PC ExtraModelList[];
    void __cdecl LoadAreaCharacters(void);
}
void __cdecl legoSetMusicVolume(f32);

void __cdecl LoadAreaCharacters(void) {
    static const char *crawl_music[] = {"Ep1_TextCrawl", "Ep2_TextCrawl", "Ep3_TextCrawl",
                                        "Ep4_TextCrawl", "Ep5_TextCrawl", "Ep6_TextCrawl"};
    i32 load_type = 2;
    i32 reload_same_area = 0;
    i32 saved_override;
    i32 icon_stage;
    i32 skip_text_scroll = 0;
    i32 fade_scene_active = 0;
    f32 icon_time = 0.0f;
    f32 touch_prompt_time = 0.0f;
    f32 music_volume;
    if (Area == -1 || Area != last_area)
        Area_Configure(Area, Level, ExtraModelList, id_DEFAULTCHARACTER);
    saved_override = AreaGetKeyboardOverridePc();
    VehicleArea = Area != -1 && (ADataList[Area].flags & 1) != 0;
    BonusArea = -1 != Area && (ADataList[Area].flags & 4) != 0;
    AreaGlobals_PC.field_04 = VehicleArea != 0 && BonusArea == 0;
    AreaDataLoaded = 0;
    loadareadata_loadlevel = 0;
    loadareacharacters_loadedlevel = 0;
    CharacterDataLoad = 0;
    ResetTimer(&GameTimer, 0.0f);
    if (!(Area != -1 && Area != last_area && (ADataList[Area].flags & 2) == 0 && new_level_from_menu == 0 &&
          ((HUB_ADATA != NULL && last_area == HUB_ADATA->index) || netnewgame != 0 ||
           (GAMEDEMO != 0 && (LastLData == TITLES_LDATA || LastLData == STATUS_LDATA))))) {
        if (0 != SuperStory && (ADataList[Area].flags & 2) != 0)
            load_type = 1;
        else if (last_area != -1 && LastLData != NULL && (LastLData->flags & 0x400) != 0 &&
                 ADataList[Area].flags != 2 && HUB_ADATA != NULL && Area != HUB_ADATA->index)
            load_type = 2;
        else
            load_type = 0;
        reload_same_area = Area != -1 && Area == last_area;
    }
    netnewgame = 0;
    if (reload_same_area != 0 || LOADEROFF != 0 || BGLOAD == 0) {
        LoadAreaData(NULL);
        MainRenderTime = 1.0f;
        return;
    }
    textcrawlactive = 1;
    pNuCam->mtx = NuMtxSetIdentityTemplate;
    NuCameraSet(pNuCam);
    LoadTime = 0.0f;
    LOADWAITTIME = load_type == 2 ? 1.0f : 0.1f;
    LoadWait = LOADWAITTIME;
    FRAMETIME = DEFAULTFRAMETIME;
    CutBorderScale = 1.0f;
    music_volume = GameSetMusicVolume(&Game.options_save);
    if (loadareacharacters_no_backdrop_reset == 0) {
        BackDrop_ResetColours();
    }
    TextCrawl_Init(&TextCrawlLswPc, Area, 1);
    icon_stage = HUB_ADATA != NULL && HUB_ADATA->index == Area ? 0 : 2;
    loadareadata_loadlevel = 1;
    WorldInfo_ReArrangeBuffers(Area, last_area);
    SetBackgroundMusic(1);
    if (load_type == 2 && AreaCrawlMusicEnabledPc != 0 && Game.options_save.music_enabled != 0 && NOSOUND == 0 &&
        0 == NOMUSIC) {
        if (Arcade == 0 && Area != -1 && -1 != ((i8)ADataList[Area].episode_index)) {
            i32 handle = music_man.GetTrackHandle(1, crawl_music[(i8)ADataList[Area].episode_index]);
            music_man.SelectTrackByHandle(1, handle);
            if (music_man.GetTrackHandle(1, NULL) != -1)
                music_man.PlayTrack(1);
        } else {
            GamePlayMusic(TITLES_LDATA, 0, &Game.options_save);
        }
        NuSound3Update();
    }
    bgPostRequest(LoadAreaData, NULL, NULL, 0);
    while (AreaDataLoaded == 0 || LoadWait > 0.0f) {
        i32 draw_touch_prompt = 0;
        TextInputSetKeyboardOverridePc(0);
        NuWindowsProcessMessages();
        STUBBED_NAMED(StartupDrawPumpPc); /* Native48b91c direct6d3420. */
        NuFrameBegin();
        NuCameraSet(&global_camera);
        readpads_always = 1;
        ReadPads();
        if (load_type != 2) {
            if (AreaDataLoaded != 0)
                skip_text_scroll = 1;
        } else if (AreaDataLoaded != 0 && LoadWait == LOADWAITTIME && LoadTime < AreaCrawlMaximumTimePc &&
                   skip_text_scroll == 0 && (NuSound3LoadingSfx() == 0 || !(LoadTime < AreaCrawlSoundWaitTimePc))) {
            AREA_LOADING_PAD_READER_PC *pads = (AREA_LOADING_PAD_READER_PC *)GamePadStorage;
            if ((pads[0].buttons_pressed & GAMEPAD_START) != 0 || (pads[1].buttons_pressed & GAMEPAD_START) != 0 ||
                NuKeyboard_db(1) != 0)
                skip_text_scroll = 1;
            else
                draw_touch_prompt = 1;
            touch_prompt_time += FRAMETIME;
        }
        UpdateTimer(&GlobalTimer);
        ((AREA_LOADING_CLOCK_READER_PC *)&Game)->elapsed_time =
            ((AREA_LOADING_CLOCK_READER_PC *)&Game)->elapsed_time + FRAMETIME;
        LoadTime += FRAMETIME;
        if (icon_stage == 0) {
            if (CharacterDataLoad == 2 || (CharacterDataLoad != 0 && NULL != APICharacterLoaded(PlayerID[0]) &&
                                           APICharacterLoaded(PlayerID[1]) != NULL))
                icon_stage = 1;
        } else if (icon_stage == 1) {
            icon_time += FRAMETIME;
            if (0.6f < icon_time && 2 != CharacterDataLoad)
                icon_time = 0.6f;
            if ((f64)icon_time >= (f64)3.9499998092651367f) {
                icon_stage = 2;
                icon_time = 3.9499998092651367f;
            }
        }
        if (load_type != 2) {
            if (0 != AreaDataLoaded && icon_stage == 2 && LoadWait > 0.0f) {
                LoadWait -= FRAMETIME;
                if (LoadWait < 0.0f)
                    LoadWait = 0.0f;
            }
        } else {
            if (AreaDataLoaded != 0 && LoadWait > 0.0f &&
                (LoadTime >= AreaCrawlMaximumTimePc || skip_text_scroll != 0)) {
                LoadWait -= FRAMETIME;
                if (LoadWait < 0.0f)
                    LoadWait = 0.0f;
            }
            legoSetMusicVolume(music_volume * ((LoadWait / LOADWAITTIME)));
            BackDrop_Update((AreaDataLoaded != 0 ? LoadWait / LOADWAITTIME : 1.0f) * FRAMETIME);
            BackDrop_UpdateColours(1);
        }
        if (NOSOUND == 0) {
            NuSound3Update();
            music_man.Process(FRAMETIME);
        }
        music_man.Process(FRAMETIME);
        NuRndrBeginScene(1);
        if (load_type != 2) {
            NuRndrGradClear(0xf00, (i32)0x80000000u, (i32)0x80000000u, 1.0f);
            if (load_type == 1)
                BackDrop_Draw(1.0f, 1);
            if (LoadWait == LOADWAITTIME) {
                if (HUB_ADATA != NULL && Area == HUB_ADATA->index) {
                    if (CharacterDataLoad != 0 && icon_time > 0.0f && (f64)icon_time < (f64)3.9499998092651367f) {
                        f32 base_x = 0.1425f;
                        f32 icon_scale = 0.4f;
                        if (Game_OptionsSave != NULL && Game_OptionsSave->widescreen != 0) {
                            base_x = 0.106875f;
                            icon_scale = 0.35f;
                        }
                        i32 bob_angle = (i32)((f64)NuFmod(LoadTime, 1.0f) * 65536.0);
                        f32 bob = (f32)((f64)NuTrigTable[(bob_angle >> 1) & 0x7fff] * 0.15000000596046448);
                        for (i32 player = 0; 1 >= player; ++player) {
                            f32 phase = player == 0 ? icon_time : (f32)((f64)icon_time - 0.25);
                            if ((f64)phase > 0.0 && (f64)phase < 3.6999998092651367) {
                                f32 x;
                                f32 alpha;
                                if (phase < 0.6f) {
                                    alpha = (f32)((f64)phase / 0.6000000238418579);
                                    f32 start_x = (f32)((f64)base_x + 0.30000001192092896);
                                    i32 angle = (i32)((f64)alpha * 16384.0);
                                    x = (f32)((f64)start_x +
                                              ((f64)base_x - start_x) * NuTrigTable[(angle >> 1) & 0x7fff]);
                                } else {
                                    x = base_x;
                                    if (phase < 3.1f)
                                        alpha = 1.0f;
                                    else
                                        alpha = (f32)(1.0 - ((f64)phase - 3.0999999046325684) / 0.5999999046325684);
                                }
                                DrawCharIconFindPc = 1;
                                if (player == 0)
                                    DrawCharIcon(PlayerID[0], -x, 0.16624999046325684f, 0.0f, icon_scale, 166, alpha,
                                                 (f32)((0.8500000238418579 + (f64)bob) * alpha), 1, NULL);
                                else
                                    DrawCharIcon(PlayerID[1], x, -0.16624999046325684f, 0.0f, icon_scale, 165, alpha,
                                                 (f32)((0.8500000238418579 - (f64)bob) * alpha), 1, NULL);
                            }
                        }
                    }
                } else {
                    fade_scene_active = 1;
                }
            }
        } else {
            u32 top = 0x80000000u | ((u32)(i32)RenderBackdropTopBPc & 255) << 16 |
                      ((u32)(i32)RenderBackdropTopGPc & 255) << 8 | ((u32)(i32)RenderBackdropTopRPc & 255);
            u32 bottom = 0x80000000u | (255 & ((u32)(i32)RenderBackdropBottomBPc)) << 16 |
                         ((u32)(i32)RenderBackdropBottomGPc & 255) << 8 | ((u32)(i32)RenderBackdropBottomRPc & 255);
            f32 alpha = loadareacharacters_no_backdrop_reset == 0 && LoadTime < 0.5f ? LoadTime * 2.0f : 1.0f;
            NuRndrGradClear(0xf00, (i32)top, (i32)bottom, 1.0f);
            BackDrop_Draw(alpha, 1);
            if (AreaDataLoaded == 0 || (f64)LoadWait >= (f64)0.1f) {
                f32 crawl_alpha =
                    AreaDataLoaded == 0 ? 1.0f : (f32)(((f64)LoadWait - (f64)0.1f) / ((f64)LOADWAITTIME - (f64)0.1f));
                if (AreaDataLoaded == 0 || crawl_alpha > 0.0f) {
                    TextCrawl_Draw(FRAMETIME, 0, crawl_alpha, NULL);
                    SetQFont2D();
                }
            }
            if (draw_touch_prompt != 0) {
                i32 phase = (i32)(NuFmod(touch_prompt_time, 1.0f) * 65536.0);
                f32 wave = NuTrigTable[(((u16)phase + 0x4000) >> 1) & 0x7fff];
                i32 prompt_alpha = (i32)(32.0 - wave * 32.0);
                SetQFont2D();
                Text3DEx(TTab[tTOUCHTOSTART], 0.0f, AreaCrawlPromptYPc, 1.0f, AreaCrawlPromptScalePc,
                         AreaCrawlPromptScalePc, AreaCrawlPromptScalePc, 0, 255, 255, 255, prompt_alpha);
            }
        }
        if (fade_scene_active != 0) {
            NuRndrEndScene();
            FadeSys.Update();
            FadeSys.Draw();
            NuRndrBeginScene(1);
        }
        STUBBED_NAMED(GameThingsRender); /* Native48bf3a: virtual slot28(this,0),
                                            genuine global95e438 unresolved. */
        NuRndrEndScene();
        if (AreaDataLoaded != 0 && (f64)LoadWait < (f64)0.1f &&
            (FadeSys.pending_type == FADE_TYPE_NONE || FadeSys.pending_type == FADE_TYPE_STILL)) {
            FADETYPE wipe = {FADE_TYPE_STILL_WIPE};
            FadeSys.SetFade(wipe, 0);
            NeedScreenGrab(1); /* Native48bf88. */
            GrabStillScreen();
        }
        edGraEnableTerrainSwap();
        FRAMETIME = NuFrameEnd();
        edGraDisableTerrainSwap();
    }
    MainRenderTime = 1.0f;
    CutBorderScale = 0.0f;
    music_man.StopAll(0);
    MusicClearAll();
    SoundKillAll();
    legoSetMusicVolume(music_volume);
    textcrawlactive = 0;
    if (loadareadata_loadlevel != 0) {
        loadareacharacters_loadedlevel = 1;
        loadareadata_loadlevel = 0;
    }
    TextInputSetKeyboardOverridePc(saved_override);
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
                        volatile f32 rounded_distance = NuVecDistSqr(position, draw_position, NULL);
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
