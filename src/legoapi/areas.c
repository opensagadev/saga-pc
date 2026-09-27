#include "saga.h"

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

LEVELDATA_PC *__cdecl Level_FindByName(char *name, i32 *index_out) {
    i32 index;

    for (index = 0; index < LEVELCOUNT; ++index) {
        if (NuStrICmp(LDataList[index].name, name) == 0) {
            if (index_out != NULL) {
                *index_out = index;
            }
            return &LDataList[index];
        }
    }

    if (index_out != NULL) {
        *index_out = -1;
    }
    return NULL;
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
