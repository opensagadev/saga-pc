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

i32 __cdecl CharIDFromName(char *name) {
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

        if (resident_count != 0) {
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

LEVELDATA_PC *__cdecl Area_FindStatusLevel(AREA_DATA_PC *area, i32 *index_out) {
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
        STUBBED_NAMED("Areas_ConfigureList: unresolved ret-only PC failure helper at 0x00572ad0");
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
        STUBBED_NAMED("Areas_ConfigureList: unresolved ret-only PC zero-record helper at 0x00572ad0");
        return NULL;
    }

    buffer_end->void_ptr = area;
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
