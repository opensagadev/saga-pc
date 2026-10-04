
#include "saga.h"
typedef struct menu_mission_reader_pc_s {
    u8 unknown_00[0x12];
    i8 active_count;
} MENU_MISSION_READER_PC;
STATIC_ASSERT(offsetof(MENU_MISSION_READER_PC, active_count) == 0x12);
extern i32 SuperStoryEpisode, hub_from_superstory, hub_from_mission;
extern i32 newlevelfrommenu_newmenuid, newlevelfrommenu_newmenuy;
void __cdecl NewLevelFromMenu(LEVELDATA_PC *level, i32 menu_id, i32 menu_y, i32 ignored) {
    MISSIONDATA_PC *mission;
    (void)ignored;
    if (no_more_loads == 0) {
        abort_load = 1;
        no_more_loads = 1;
    }
    Door_Reset();
    NewLData = level;
    new_level_from_menu = 1;
    newlevelfrommenu_newmenuid = menu_id;
    newlevelfrommenu_newmenuy = menu_y;
    if (HUB_ADATA != NULL && WORLD->area == HUB_ADATA)
        return;
    if (SuperStory != 0) {
        hub_from_superstory = SuperStoryEpisode;
        return;
    }
    mission = Mission_Active(NULL);
    if (mission != NULL && NewLData != NULL && NewLData == HUB_LDATA)
        hub_from_mission = ((MENU_MISSION_READER_PC *)mission)->active_count;
}
