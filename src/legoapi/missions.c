#include "saga.h"

STATIC_ASSERT(sizeof(MISSIONDATA_PC) == 0x18);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, time) == 0x10);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, index) == 0x12);
STATIC_ASSERT(offsetof(MISSIONSYS_PC, missions) == 0);
STATIC_ASSERT(offsetof(MISSIONSYS_PC, active_mission) == 4);
STATIC_ASSERT(offsetof(MISSIONSYS_PC, state) == 0x1d);
STATIC_ASSERT(offsetof(TIMER, time_elapsed) == 0);

typedef struct MISSION_END_DATA_VIEW_PC { // size 0x2
    i16 find_character;                   // 0x0
} MISSION_END_DATA_VIEW_PC;

typedef struct MISSION_END_SYSTEM_VIEW_PC { // size 0xc
    u8 unknown_00[8];                       // 0x0
    f32 elapsed_time;                       // 0x8
} MISSION_END_SYSTEM_VIEW_PC;

STATIC_ASSERT(offsetof(GAMEOBJECT, collision_identity_mask) == 0x1e8);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_contact_mask) == 0x1f0);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_280) == 0x280);

typedef struct MISSION_SYSTEM_READER_PC { // size 0x30
    MISSIONDATA_PC *missions;             // 0x00
    MISSIONDATA_PC *active_mission;       // 0x04
    TIMER timer;                          // 0x08
    MISSION_SAVE_READER_PC *save;         // 0x18
    u8 count;                             // 0x1c
    u8 state;                             // 0x1d

    union { // size 0x1
        u8 flags;

        struct {                  // size 0x1
            u8 pickups : 1;       // 0x1e:0
            u8 unknown_flags : 7; // 0x1e:1
        };
    }; // 0x1e

    u8 character_count;   // 0x1f
    i16 character_ids[8]; // 0x20
} MISSION_SYSTEM_READER_PC;

STATIC_ASSERT(sizeof(MISSIONDATA_PC) == 0x18);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, find_character) == 0);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, level) == 2);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, name_id) == 4);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, text_id) == 6);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, bounty) == 8);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, alternate_bounty) == 0xc);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, time) == 0x10);
STATIC_ASSERT(offsetof(MISSIONDATA_PC, index) == 0x12);
STATIC_ASSERT(sizeof(MISSION_SAVE_READER_PC) == 0x64);
STATIC_ASSERT(offsetof(MISSION_SAVE_READER_PC, best_times) == 0);
STATIC_ASSERT(offsetof(MISSION_SAVE_READER_PC, completed) == 0x50);
STATIC_ASSERT(offsetof(TIMER, time_elapsed) == 0);

/* Original name unknown. Empty on PC; called when the file cannot be
   opened. VC8 drops calls to an empty body it can see, so the definition lives outside this file
   (see SpecialMovesConfigMissingPc). */
void MissionsConfigMissingPc(char *filename);

MISSIONSYS_PC *Missions_Configure(char *file, VARIPTR *buffer_start, VARIPTR *buffer_end,
                                  MISSION_SAVE_READER_PC *save) {
    NUFPAR_PC *parser;
    MISSION_SYSTEM_READER_PC temporary;
    MISSION_SYSTEM_READER_PC *destination;
    MISSIONDATA_PC *mission;
    i32 character;
    parser = NuFParCreate(file);
    if (parser == NULL) {
        MissionsConfigMissingPc(file);
        return NULL;
    }
    memset(&temporary, 0, sizeof(temporary));
    temporary.pickups = 1;
    buffer_start->addr = (buffer_start->addr + 3) & ~3u;
    mission = buffer_start->void_ptr;
    temporary.missions = mission;
    temporary.save = save;
    while (temporary.count < 20 && NuFParGetLine(parser) != 0) {
        NuFParGetWord(parser);
        if (NuStrICmp(parser->word_buffer_ptr, "party") == 0) {
            while (temporary.character_count < 8 && NuFParGetWord(parser) != 0) {
                character = CharIDFromName(parser->word_buffer_ptr);
                if (character != -1) {
                    temporary.character_ids[temporary.character_count] = (i16)character;
                    ++temporary.character_count;
                }
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "pickups") == 0) {
            if (NuFParGetWord(parser) != 0) {
                if (NuStrICmp(parser->word_buffer_ptr, "on") == 0) {
                    temporary.pickups = 1;
                } else if (NuStrICmp(parser->word_buffer_ptr, "off") == 0) {
                    temporary.pickups = 0;
                }
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "mission") == 0) {
            mission->find_character = -1;
            mission->level = -1;
            mission->name_id = -1;
            mission->bounty = 100000;
            mission->alternate_bounty = 100000;
            mission->text_id = -1;
            mission->time = 180;
            mission->index = temporary.count;
            while (NuFParGetWord(parser) != 0) {
                if (NuStrICmp(parser->word_buffer_ptr, "find") == 0) {
                    if (NuFParGetWord(parser) != 0) {
                        mission->find_character = (i16)CharIDFromName(parser->word_buffer_ptr);
                    }
                } else if (NuStrICmp(parser->word_buffer_ptr, "in_level") == 0) {
                    if (NuFParGetWord(parser) != 0 && Level_FindByName(parser->word_buffer_ptr, &character) != NULL) {
                        mission->level = (i16)character;
                    }
                } else if (NuStrICmp(parser->word_buffer_ptr, "time") == 0) {
                    mission->time = (u16)NuFParGetInt(parser);
                    if (mission->time < 3) {
                        mission->time = 3;
                    }
                } else if (NuStrICmp(parser->word_buffer_ptr, "bounty") == 0) {
                    mission->bounty = NuFParGetInt(parser);
                    mission->alternate_bounty = NuFParGetInt(parser);
                } else if (NuStrICmp(parser->word_buffer_ptr, "name_id") == 0) {
                    mission->name_id = (i16)NuFParGetInt(parser);
                } else if (NuStrICmp(parser->word_buffer_ptr, "text_id") == 0) {
                    mission->text_id = (i16)NuFParGetInt(parser);
                }
            }
            if (mission->find_character != -1 && mission->level != -1) {
                ++temporary.count;
                ++mission;
                buffer_start->addr += sizeof(MISSIONDATA_PC);
            }
        }
    }
    NuFParDestroy(parser);
    if (temporary.count <= 0) {
        return NULL;
    }
    buffer_start->addr = (buffer_start->addr + 3) & ~3u;
    destination = buffer_start->void_ptr;
    PCByteCopy(destination, &temporary, sizeof(temporary));
    buffer_start->addr = (buffer_start->addr + sizeof(temporary) + 3) & ~3u;
    return (MISSIONSYS_PC *)destination;
}

void Mission_Clear(MISSIONSYS_s *ms) {
    if (ms == NULL) {
        ms = MissionSys;
        if (ms == NULL) {
            return;
        }
    }
    ms->mission = NULL;
    ms->field8_0x1d = 0;
    ResetTimer(&ms->timer, 0.0f);
}

MISSIONDATA_PC *Mission_Active(MISSIONSYS_PC *system) {
    if (system == NULL) {
        system = MissionSys;
        if (system == NULL) {
            return NULL;
        }
    }
    if (system->state != 0 && system->active_mission != NULL) {
        return system->active_mission;
    }
    return NULL;
}

i32 Mission_CurrentState(MISSIONSYS_PC *system) {
    MISSION_SYSTEM_READER_PC *reader;
    if (system == NULL) {
        system = MissionSys;
    }
    if (system == NULL) {
        return 0;
    }
    reader = (MISSION_SYSTEM_READER_PC *)system;
    if (reader->state != 0 && reader->active_mission != NULL) {
        return reader->state;
    }
    return 0;
}

i32 Missions_PartyAvailable(MISSIONSYS_PC *system) {
    MISSION_SYSTEM_READER_PC *reader;
    i32 index;
    if (system == NULL) {
        system = MissionSys;
    }
    if (system != NULL) {
        reader = (MISSION_SYSTEM_READER_PC *)system;
        for (index = 0; index < reader->character_count; ++index) {
            if (Collection_Got(reader->character_ids[index]) == 0) {
                return 0;
            }
        }
    }
    return 1;
}

i32 Missions_NumCompleted(MISSIONSYS_PC *system, MISSION_SAVE_READER_PC *save, i32 complete) {
    MISSION_SYSTEM_READER_PC *reader;
    i32 index;
    i32 total = 0;
    if (system == NULL) {
        system = MissionSys;
    }
    if (system != NULL) {
        reader = (MISSION_SYSTEM_READER_PC *)system;
        for (index = 0; index < reader->count; ++index) {
            if (complete != 0) {
                save->completed[index] = 1;
                if (save->best_times[index] == 0.0f) {
                    save->best_times[index] = (f32)reader->missions[index].time - 1.0f;
                }
            }
            if (save->completed[index] != 0) {
                ++total;
            }
        }
    }
    return total;
}

GAMEOBJECT *Mission_FindTarget(MISSIONSYS_PC *system, u64 *identity) {
    MISSION_SYSTEM_READER_PC *reader;
    GAMEOBJECT *target;
    if (system == NULL) {
        system = MissionSys;
    }
    if (system != NULL) {
        reader = (MISSION_SYSTEM_READER_PC *)system;
        target = FindGameObject(reader->active_mission->find_character, 0, 0, 1, 0);
        if (identity != NULL) {
            *identity = target != NULL ? U64_PAIR(target->collision_identity_mask) : 0;
        }
        return target;
    }
    if (identity != NULL) {
        *identity = 0;
    }
    return NULL;
}

i32 Missions_PickupsOff(MISSIONSYS_PC *system) {
    MISSION_SYSTEM_READER_PC *reader;
    if (system == NULL) {
        system = MissionSys;
    }
    if (system == NULL) {
        return 0;
    }
    reader = (MISSION_SYSTEM_READER_PC *)system;
    if (reader->state == 0 || reader->active_mission == NULL || (reader->flags & 1) != 0) {
        return 0;
    }
    return 1;
}
