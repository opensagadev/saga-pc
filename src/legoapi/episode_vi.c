#include "saga.h"

typedef struct emperor_fight_packet_pc_s {
    i32 state;
    u8 enabled;
    u8 zap;
    u8 player_floor_mask;
    u8 reserved;
} EMPEROR_FIGHT_PACKET_PC;
STATIC_ASSERT(sizeof(EMPEROR_FIGHT_PACKET_PC) == 8);
STATIC_ASSERT(offsetof(EMPEROR_FIGHT_PACKET_PC, enabled) == 4);

typedef struct level_script_flags_pc_s {
    u8 first;
    u8 light_flags[4];
    u8 arrow_flags[4];
    u8 remaining[7];
} LEVEL_SCRIPT_FLAGS_PC;
STATIC_ASSERT(sizeof(LEVEL_SCRIPT_FLAGS_PC) == 16);
STATIC_ASSERT(offsetof(LEVEL_SCRIPT_FLAGS_PC, light_flags) == 1);
STATIC_ASSERT(offsetof(LEVEL_SCRIPT_FLAGS_PC, arrow_flags) == 5);

void __cdecl EmperorFightA_Init(WORLDINFO *world) {
    LEVEL_SCRIPT_FLAGS_PC *flags = (LEVEL_SCRIPT_FLAGS_PC *)&LevFlag;
    i32 i;
    emperorfighta_netpacket = SetLevelHack(8);
    ((EMPEROR_FIGHT_PACKET_PC *)emperorfighta_netpacket)->enabled = 1;
    NuSpecialFind(world->current_gscn, &LevHSpecial[3], (char *)EpisodeVI_liftrlightbon1, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[4], (char *)EpisodeVI_liftrlightbon2, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[5], (char *)EpisodeVI_liftrlightbon3, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[6], (char *)EpisodeVI_liftrlightbon4, 1);
    for (i = 0; i < 4; ++i)
        flags->light_flags[i] = 0;
    NuSpecialFind(world->current_gscn, &LevHSpecial[7], (char *)EpisodeVI_liftrarron1, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[8], (char *)EpisodeVI_liftrarron2, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[9], (char *)EpisodeVI_liftrarron3, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[10], (char *)EpisodeVI_liftrarron4, 1);
    for (i = 0; i < 4; ++i)
        flags->arrow_flags[i] = 0;
    NuSpecialFind(world->current_gscn, &LevHSpecial[30], (char *)EpisodeVI_doton01, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[31], (char *)EpisodeVI_doton02, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[32], (char *)EpisodeVI_doton03, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[33], (char *)EpisodeVI_doton04, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[34], (char *)EpisodeVI_doton05, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[35], (char *)EpisodeVI_doton06, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[36], (char *)EpisodeVI_doton07, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[37], (char *)EpisodeVI_doton08, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[38], (char *)EpisodeVI_doton09, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[39], (char *)EpisodeVI_doton10, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[40], (char *)EpisodeVI_doton11, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[41], (char *)EpisodeVI_doton12, 1);
    LevSfxFlag[0] = 0;
    LevSfxFlag[1] = 0;
    LevSfxFlag[2] = 0;
}
