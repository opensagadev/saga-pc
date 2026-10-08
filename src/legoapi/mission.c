#include "saga.h"

struct mission_data_pc_s {
    i16 find_character;
    i16 level;
    i16 name_id;
    i16 text_id;
    i32 bounty;
    i32 alternate_bounty;
    u16 time;
    u8 index;
    u8 unknown_13[5];
};
typedef struct MISSION_SYSTEM_READER_PC {
    MISSIONDATA_PC *missions;
    MISSIONDATA_PC *active_mission;
    TIMER timer;
    MISSION_SAVE_READER_PC *save;
    u8 count;
    u8 state;
    u8 flags;
    u8 character_count;
    i16 character_ids[8];
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
STATIC_ASSERT(sizeof(MISSION_SYSTEM_READER_PC) == 0x30);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, missions) == 0);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, active_mission) == 4);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, timer) == 8);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, save) == 0x18);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, count) == 0x1c);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, state) == 0x1d);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, flags) == 0x1e);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, character_count) == 0x1f);
STATIC_ASSERT(offsetof(MISSION_SYSTEM_READER_PC, character_ids) == 0x20);
STATIC_ASSERT(offsetof(TIMER, time_elapsed) == 0);

MISSIONSYS_PC *__cdecl Missions_Configure(char *file, VARIPTR *buffer_start, VARIPTR *buffer_end,
                                          MISSION_SAVE_READER_PC *save) {
    NUFPAR_PC *parser;
    MISSION_SYSTEM_READER_PC temporary, *destination;
    MISSIONDATA_PC *mission;
    i32 character;
    parser = NuFParCreate(file);
    if (parser == NULL) {
        /* Native diagnostic 58a510 is RET-only: no input read or side effect. */
        return NULL;
    }
    memset(&temporary, 0, sizeof(temporary));
    temporary.flags = 1;
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
                if (NuStrICmp(parser->word_buffer_ptr, "on") == 0)
                    temporary.flags |= 1;
                else if (NuStrICmp(parser->word_buffer_ptr, "off") == 0)
                    temporary.flags &= 0xfe;
            }
        } else if (NuStrICmp(parser->word_buffer_ptr, "mission") == 0) {
            mission->find_character = -1;
            mission->level = -1;
            mission->name_id = -1;
            mission->text_id = -1;
            mission->bounty = mission->alternate_bounty = 100000;
            mission->time = 180;
            mission->index = temporary.count;
            while (NuFParGetWord(parser) != 0) {
                if (NuStrICmp(parser->word_buffer_ptr, "find") == 0) {
                    if (NuFParGetWord(parser) != 0)
                        mission->find_character = (i16)CharIDFromName(parser->word_buffer_ptr);
                } else if (NuStrICmp(parser->word_buffer_ptr, "in_level") == 0) {
                    if (NuFParGetWord(parser) != 0 && Level_FindByName(parser->word_buffer_ptr, &character) != NULL)
                        mission->level = (i16)character;
                } else if (NuStrICmp(parser->word_buffer_ptr, "time") == 0) {
                    u16 time = (u16)NuFParGetInt(parser);
                    mission->time = time < 3 ? 3 : time;
                } else if (NuStrICmp(parser->word_buffer_ptr, "bounty") == 0) {
                    mission->bounty = NuFParGetInt(parser);
                    mission->alternate_bounty = NuFParGetInt(parser);
                } else if (NuStrICmp(parser->word_buffer_ptr, "name_id") == 0)
                    mission->name_id = (i16)NuFParGetInt(parser);
                else if (NuStrICmp(parser->word_buffer_ptr, "text_id") == 0)
                    mission->text_id = (i16)NuFParGetInt(parser);
            }
            if (mission->find_character != -1 && mission->level != -1) {
                ++temporary.count;
                ++mission;
                buffer_start->addr += sizeof(MISSIONDATA_PC);
            }
        }
    }
    NuFParDestroy(parser);
    if (temporary.count == 0)
        return NULL;
    buffer_start->addr = (buffer_start->addr + 3) & ~3u;
    destination = buffer_start->void_ptr;
    PCByteCopy_candidate(destination, &temporary, sizeof(temporary));
    buffer_start->addr = (buffer_start->addr + sizeof(temporary) + 3) & ~3u;
    return (MISSIONSYS_PC *)destination;
}
