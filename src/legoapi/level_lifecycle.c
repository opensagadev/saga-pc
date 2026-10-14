#include "saga.h"

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

void __cdecl Levels_FixUp(LEVELFIXUP_PC *fixup) {
    LEVELDATA_PC *level;
    if (fixup != NULL) {
        for (; fixup->name != NULL; ++fixup) {
            if (fixup->level != NULL) {
                level = Level_FindByName(fixup->name, NULL);
                *fixup->level = level;
                if (level != NULL) {
                    if (fixup->load != NULL)
                        level->callbacks[0] = fixup->load;
                    if (fixup->init != NULL)
                        level->callbacks[1] = fixup->init;
                    if (fixup->reset != NULL)
                        level->callbacks[2] = fixup->reset;
                    if (fixup->update != NULL)
                        level->callbacks[3] = fixup->update;
                    if (fixup->always_update != NULL)
                        level->callbacks[4] = fixup->always_update;
                    if (fixup->draw != NULL)
                        level->callbacks[5] = fixup->draw;
                    if (fixup->draw_status != NULL)
                        level->callbacks[6] = fixup->draw_status;
                }
            }
        }
    }
}

void __cdecl Level_Draw(WORLDINFO *world) {
    void(__cdecl * draw)(WORLDINFO *) = (void(__cdecl *)(WORLDINFO *))world->current_level->callbacks[5];
    if (draw != NULL)
        draw(world);
}

void __cdecl Level_Update(WORLDINFO *world) {
    void(__cdecl * update)(WORLDINFO *) = (void(__cdecl *)(WORLDINFO *))world->current_level->callbacks[3];
    if (update != NULL)
        update(world);
    if (DoubleScoreTime > 0.0f &&
        (i32)((f64)GameTimer.time_elapsed * 2.0) != (i32)((f64)GameTimer.last_time_elapsed * 2.0))
        GameAudio_PlaySfxAndSetVolume(0x35, NULL, DoubleScoreTime);
    if (AreaGlobals_PC.field_00 == 0 && Cheats_CheckFlags(0x2000) != 0)
        AreaGlobals_PC.field_00 = 1;
}

/* Native 0x0048a310, 217 bytes. */
typedef struct level_script_progress_pc_s {
    char name[16];
    f32 params[4];
} LEVEL_SCRIPT_PROGRESS_PC;
typedef struct ai_script_progress_view_pc_s {
    u8 unknown_0000[0x2824];
    LEVEL_SCRIPT_PROGRESS_PC scripts[32];
} AI_SCRIPT_PROGRESS_VIEW_PC;
STATIC_ASSERT(offsetof(AI_SCRIPT_PROGRESS_VIEW_PC, scripts) == 0x2824);
STATIC_ASSERT(sizeof(LEVEL_SCRIPT_PROGRESS_PC) == 0x20);
/* Shared LEVEL_SCRIPT_PROCESS_PC is defined in saga.h in the final patch. */
void __cdecl LevelScriptReStoreProgress(WORLDINFO *world, LEVEL_SCRIPT_PROCESS_PC *process) {
    i32 index, parameter;
    AI_SCRIPT_PROGRESS_VIEW_PC *progress;
    if (world->level_progress == NULL || process == NULL || NuStrLen(process->name) == 0)
        return;
    for (index = 0; index < 32; ++index) {
        progress = (AI_SCRIPT_PROGRESS_VIEW_PC *)world->level_progress;
        if (NuStrLen(progress->scripts[index].name) == 0)
            break;
        progress = (AI_SCRIPT_PROGRESS_VIEW_PC *)world->level_progress;
        if (NuStrICmp(progress->scripts[index].name, process->name) == 0) {
            for (parameter = 0; parameter < 4; ++parameter) {
                progress = (AI_SCRIPT_PROGRESS_VIEW_PC *)world->level_progress;
                process->processor.params[parameter] = progress->scripts[index].params[parameter];
            }
            break;
        }
    }
}
