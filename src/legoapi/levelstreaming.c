#include "saga.h"

typedef struct streaming_door_reader_pc_s {
    u8 unknown_00[0xd4];
    NUVEC position;
    u8 unknown_e0[0x10];
    i16 level;
    i16 freeplay_level;
    u8 unknown_f4[0x34];
} STREAMING_DOOR_READER_PC;
STATIC_ASSERT(sizeof(STREAMING_DOOR_READER_PC) == 0x128);
STATIC_ASSERT(offsetof(STREAMING_DOOR_READER_PC, position) == 0xd4);
STATIC_ASSERT(offsetof(STREAMING_DOOR_READER_PC, level) == 0xf0);
STATIC_ASSERT(offsetof(STREAMING_DOOR_READER_PC, freeplay_level) == 0xf2);

typedef struct streaming_world_reader_pc_s {
    u8 unknown_00[0x4698];
    STREAMING_DOOR_READER_PC *doors;
    i32 door_count;
} STREAMING_WORLD_READER_PC;
STATIC_ASSERT(offsetof(STREAMING_WORLD_READER_PC, doors) == 0x4698);
STATIC_ASSERT(offsetof(STREAMING_WORLD_READER_PC, door_count) == 0x469c);

typedef struct streaming_cut_reader_pc_s {
    u8 unknown_00[0xec];
    i16 skip_level;
} STREAMING_CUT_READER_PC;
STATIC_ASSERT(offsetof(STREAMING_CUT_READER_PC, skip_level) == 0xec);
STATIC_ASSERT(offsetof(LEVELDATA_PC, flags) == 0x64);
STATIC_ASSERT(offsetof(LEVELDATA_PC, index) == 0x62);
STATIC_ASSERT(offsetof(LEVELDATA_PC, area_index) == 0xa3);
STATIC_ASSERT(offsetof(AREA_LEVEL_LOAD_PC, flags) == 6);
STATIC_ASSERT(offsetof(AREA_LEVEL_LOAD_PC, first_level) == 2);
STATIC_ASSERT(offsetof(AREA_LEVEL_LOAD_PC, second_level) == 4);
STATIC_ASSERT(sizeof(AREA_LEVEL_LOAD_PC) == 8);
STATIC_ASSERT(offsetof(GAMEOBJECT, position) == 0x5c);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);

extern i32 LEGOSPL_SPLIT, other_level_override;
i32 __cdecl Players_AveragePos(NUVEC *, SOCKPOSITION_PC *);
i16 __cdecl StreamingInsideLineXZPc(f32, f32, f32, f32, f32, f32);
BGPROCINFO *__cdecl bgGetProcActive(void);

static i32 LevelStreamingChooseSplitPc(WORLDINFO *world, NUVEC *position, i32 first, i32 second) {
    NUGSPLINE *split;
    NUVEC local;
    NUVEC *points;
    if (LEGOSPL_SPLIT == -1)
        return 0;
    split = world->camera_splines[LEGOSPL_SPLIT];
    if (split == NULL)
        return 0;
    if (position != NULL)
        local = *position;
    else if (Players_AveragePos(&local, NULL) == 0)
        return 0;
    points = split->pts;
    if (StreamingInsideLineXZPc(local.x, local.z, points[0].x, points[0].z, points[1].x, points[1].z))
        other_level = first;
    else
        other_level = second;
    return 1;
}

void __cdecl LevelStreaming_Update(WORLDINFO *world) {
    STREAMING_WORLD_READER_PC *streaming = (STREAMING_WORLD_READER_PC *)world;
    STREAMING_CUT_READER_PC *cut;
    NUVEC position;
    i32 i, level;
    f32 best_distance, distance;
    AREA_LEVEL_LOAD_PC *load;
    LEVELDATA_PC *status;
    if (LOADEROFF != 0 || BGLOAD == 0 || world->area == NULL || (world->area->flags & 8) || NewLData != NULL ||
        Mission_Active(NULL) != NULL)
        return;
    other_level = -1;
    if (no_more_loads == 0) {
        cut = (STREAMING_CUT_READER_PC *)CutStopInfo;
        if (other_level_override != -1) {
            other_level = other_level_override;
        } else if (CUTSTOPGAME != 0 && cut != NULL && cut->skip_level != -1 &&
                   LDataList[cut->skip_level].area_index == LDataList[world->level_idx].area_index) {
            other_level = cut->skip_level;
        } else if (!(world->current_level->flags & 0xe0) && Players_AveragePos(&position, NULL) != 0) {
            best_distance = 1000000000.0f;
            other_level = -1;
            for (i = 0; i < streaming->door_count; ++i) {
                STREAMING_DOOR_READER_PC *door = &streaming->doors[i];
                level = InStory() != 0 || door->freeplay_level == -1 ? door->level : door->freeplay_level;
                if (level != world->level_idx) {
                    distance = NuVecDistSqr(&door->position, &position, NULL);
                    if (distance < best_distance) {
                        best_distance = distance;
                        other_level = door->level;
                    }
                }
            }
            load = LevelLoad;
            for (i = 0; i < LevelLoadCount; ++i, ++load) {
                if (load->level == world->level_idx &&
                    (((load->flags & 1) && InStory() != 0) || ((load->flags & 2) && InStory() == 0)))
                    break;
            }
            if (i < LevelLoadCount) {
                if (load->first_level == load->second_level)
                    other_level = load->first_level;
                else
                    LevelStreamingChooseSplitPc(world, &position, load->first_level, load->second_level);
            }
            if (other_level != -1 && InStory() == 0 && (LDataList[other_level].flags & 0x80)) {
                status = Area_FindStatusLevel(world->area, NULL);
                if (status != NULL)
                    other_level = status->index;
            }
            if (other_level != -1 && InStory() == 0 && (LDataList[other_level].flags & 0xe0))
                other_level = -1;
            if (other_level != -1 && CutScenePlayer_Active() != NULL && (LDataList[other_level].flags & 0x400))
                other_level = -1;
        }
    }
    if (waiting_for_level == -1 && bgGetProcActive() == NULL && other_level != -1 &&
        other_level != WorldInfo_OtherLevel(world) && LDataList[other_level].area_index == world->area_index) {
        waiting_for_level = other_level;
        next_level = other_level;
        abort_load = 0;
        WaitingForLevelTime = 0.0f;
        LOG_INFO("Stream level request: current=%d next=%d area=%d", world->level_idx, next_level, world->area_index);
        bgPostRequest(WorldInfo_StreamLevel, NULL, NULL, 0);
    } else if (waiting_for_level != -1 && bgGetProcActive() != NULL && other_level != -1 &&
               other_level != waiting_for_level && LDataList[other_level].area_index == world->area_index) {
        abort_load = 1;
    }
}

void __cdecl WorldInfo_Activate(void) {
    LOG_INFO("World activate begin: previous=%p next=%p", (void *)WORLD, (void *)LWORLD);
    WORLD = LWORLD;
    WorldInfo_Init(LWORLD);
    LOG_INFO("WorldInfo_Activate complete: world=%p", (void *)WORLD);
}
