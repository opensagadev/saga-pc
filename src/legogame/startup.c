#include "saga.h"

extern LEVELDATA_PC *__cdecl Levels_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end, i32 max_count,
                                                  i32 *count_out, void *set_defaults_fn);
extern AREA_DATA_PC *__cdecl Areas_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end, i32 max_count,
                                                 i32 *count_out);

void __cdecl Level_SetDefaults(LEVELDATA_PC *level) {
    level->max_gameantinodes = 0x100;
    level->max_gizmoblowups = 0x40;
    level->max_gizmoblowuptypes = 0x40;
    level->max_pickups = 0x100;
    level->max_obstacle_objects = 0x80;
    level->max_buildit_objects = 0x80;
    level->max_force_objects = 0x80;
    level->max_bombgen_objects = 0x10;

    level->max_tightropes = 10;
    level->max_timers = 8;
    level->max_signals = 10;
    level->max_levers = 10;
    level->max_technos = 10;
    level->max_zipups = 10;
    level->max_grapples = 10;
    level->max_obstacles = 0x20;
    level->max_buildits = 0x10;
    level->max_shards = 0x40;
    level->max_spinners = 4;
    level->max_minicuts = 8;
    level->max_minicut_stages = 2;
    level->max_gizspecials = 0x20;
    level->max_attractos = 5;
    level->max_climb_objects = 10;
    level->max_guidelines = 0x20;
    level->max_ledges = 0x60;
    level->max_securitydoors = 5;
    level->max_tubes = 5;
    level->max_gizpanels = 8;
    level->max_hatmachines = 2;
    level->max_force = 0x20;
    level->max_pushblocks = 0x10;
    level->max_pushblock_endpos = 8;
    level->max_doors = 0x10;
    level->max_teleports = 4;
    level->max_gizrandoms = 8;
    level->max_torpmachines = 2;
    level->max_spinneranim_objs = 0x28;
    level->max_turrets = 0x20;
    level->max_bombgens = 8;
    level->max_bridges = 5;
    level->max_plugs = 10;

    level->wind_speed = 3.0f;
    level->wind_size = 512.0f;
}

void __cdecl FixUpAreas(void) {
    if (StartupGameFlag1 != 0) {
        Areas_FixUp(AreaFixUpLSW);
        if (LOSTTEMPLE_ADATA != NULL) {
            LOSTTEMPLE_ADATA->flags |= 0x2000;
        }
    } else if (StartupGameFlag0 != 0) {
        Areas_FixUp(AreaFixUpLostTemple);
    } else {
        Areas_FixUp(AreaFixUpGotham);
    }
}

void __cdecl LoadPermData(void *process) {
    (void)process;

    // The PC worker first initializes shared permanent resources and waits for
    // language selection before reaching the table loaders. Those setup stages
    // still have unresolved dependencies in this source reconstruction.
    STUBBED_NAMED("LoadPermData: font, language/string, audio, and permanent resource setup");

    LDataList = Levels_ConfigureList((char *)LoadPermLevelsPath, &permbuffer_ptr, &permbuffer_end, 0x16d, &LEVELCOUNT,
                                     Level_SetDefaults);
    STUBBED_NAMED("LoadPermData: FixUpLevels");

    ADataList = Areas_ConfigureList((char *)LoadPermAreasPath, &permbuffer_ptr, &permbuffer_end, 0x48, &AREACOUNT);
    FixUpAreas();

    EDataList =
        Episodes_ConfigureList((char *)LoadPermEpisodesPath, &permbuffer_ptr, &permbuffer_end, 6, &EPISODECOUNT);
}

EPISODEDATA_PC *__cdecl Episodes_ConfigureList(char *path, VARIPTR *buffer, VARIPTR *buffer_end, i32 max_count,
                                               i32 *count_out) {
    NUFPAR_PC *parser = NuFParCreate(path);
    EPISODEDATA_PC *episode_base;
    EPISODEDATA_PC *episode;
    i32 episode_count = 0;
    i32 in_episode = 0;

    if (parser == NULL) {
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
            if (episode->area_count != 0) {
                episode_count++;
                episode++;
            }
            continue;
        }

        if (NuStrICmp(word, EpisodeConfigKeyword_area) == 0) {
            if (episode->area_count <= 9 && NuFParGetWord(parser) != 0) {
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
        return NULL;
    }

    buffer_end->void_ptr = episode;
    if (count_out != NULL) {
        *count_out = episode_count;
    }
    return episode_base;
}

void __cdecl StartPerm(void) {
    permbuffer_ptr.addr = permbuffer_base.addr;
    permbuffer_end.addr = superbuffer_end.addr;
}

void __cdecl EndPerm(void) {
    u32 aligned = (permbuffer_ptr.addr + 3U) & ~3U;
    u32 world_start;
    u32 world_mid;

    permbuffer_end.addr = aligned;
    permbuffer_size = (i32)(aligned - permbuffer_base.addr);

    characterbuffer_ptr.addr = aligned;
    characterbuffer_base.addr = aligned;
    world_start = aligned + (u32)CHARACTERBUFFERSIZE;
    characterbuffer_end.addr = world_start;
    superbuffer_ptr.addr = world_start;
    superbuffer_base.addr = world_start;

    world_mid = world_start + ((superbuffer_end.addr - world_start) >> 1);
    WorldInfo[0].giz_buffer.addr = world_start;
    WorldInfo[0].buffer_start = (void *)world_start;
    WorldInfo[0].buffer_end.addr = world_mid;
    WorldInfo[1].giz_buffer.addr = world_mid;
    WorldInfo[1].buffer_start = (void *)world_mid;
    WorldInfo[1].buffer_end.addr = superbuffer_end.addr;
    editbuffer_end.addr = superbuffer_end.addr - (u32)EDITBUFFERENDSIZE;
}
