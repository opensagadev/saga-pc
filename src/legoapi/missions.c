#include "legoapi/characters/core/character.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/world/level.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/missions.h"
#include "legogame/game.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufpar.h"

/* Original name unknown. Empty on PC; called when the file cannot be
   opened. VC8 drops calls to an empty body it can see, so the definition lives outside this file
   (see SpecialMovesConfigMissing). */
void MissionsConfigMissing(char *filename);

MISSIONSYS *Missions_Configure(char *file, VARIPTR *buffer_start, VARIPTR *buffer_end, MISSION_SAVE_READER *save) {
    NUFPAR *parser;
    MISSION_SYSTEM_READER temporary;
    MISSION_SYSTEM_READER *destination;
    MISSIONDATA *mission;
    i32 character;
    parser = NuFParCreate(file);
    if (parser == NULL) {
        MissionsConfigMissing(file);
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
                buffer_start->addr += sizeof(MISSIONDATA);
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
    return (MISSIONSYS *)destination;
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

MISSIONDATA *Mission_Active(MISSIONSYS *system) {
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

i32 Mission_CurrentState(MISSIONSYS *system) {
    MISSION_SYSTEM_READER *reader;
    if (system == NULL) {
        system = MissionSys;
    }
    if (system == NULL) {
        return 0;
    }
    reader = (MISSION_SYSTEM_READER *)system;
    if (reader->state != 0 && reader->active_mission != NULL) {
        return reader->state;
    }
    return 0;
}

i32 Missions_PartyAvailable(MISSIONSYS *system) {
    MISSION_SYSTEM_READER *reader;
    i32 index;
    if (system == NULL) {
        system = MissionSys;
    }
    if (system != NULL) {
        reader = (MISSION_SYSTEM_READER *)system;
        for (index = 0; index < reader->character_count; ++index) {
            if (Collection_Got(reader->character_ids[index]) == 0) {
                return 0;
            }
        }
    }
    return 1;
}

i32 Missions_NumCompleted(MISSIONSYS *system, MISSION_SAVE_READER *save, i32 complete) {
    MISSION_SYSTEM_READER *reader;
    i32 index;
    i32 total = 0;
    if (system == NULL) {
        system = MissionSys;
    }
    if (system != NULL) {
        reader = (MISSION_SYSTEM_READER *)system;
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

GAMEOBJECT *Mission_FindTarget(MISSIONSYS *system, u64 *identity) {
    MISSION_SYSTEM_READER *reader;
    GAMEOBJECT *target;
    if (system == NULL) {
        system = MissionSys;
    }
    if (system != NULL) {
        reader = (MISSION_SYSTEM_READER *)system;
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

i32 Missions_PickupsOff(MISSIONSYS *system) {
    MISSION_SYSTEM_READER *reader;
    if (system == NULL) {
        system = MissionSys;
    }
    if (system == NULL) {
        return 0;
    }
    reader = (MISSION_SYSTEM_READER *)system;
    if (reader->state == 0 || reader->active_mission == NULL || (reader->flags & 1) != 0) {
        return 0;
    }
    return 1;
}
