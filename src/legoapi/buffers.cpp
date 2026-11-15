#include "saga.h"

extern "C" {

    VARIPTR permbuffer_base = {0};
    VARIPTR permbuffer_ptr = {0};
    VARIPTR permbuffer_end = {0};
    VARIPTR superbuffer_end = {0};

    void StartPerm(void) {
        permbuffer_ptr.addr = permbuffer_base.addr;
        permbuffer_end.addr = superbuffer_end.addr;
    }

    i32 permbuffer_size = 0;
    VARIPTR characterbuffer_base = {0};
    VARIPTR characterbuffer_ptr = {0};
    VARIPTR characterbuffer_end = {0};
    NUGSCN *vehicle_scene = NULL;
    NUGSCN *area_scene = NULL;
    NUGSCN *big_icon_scene = NULL;
    VARIPTR superbuffer_base = {0};
    VARIPTR superbuffer_ptr = {0};
    VARIPTR editbuffer_end = {0};
    i32 EDITBUFFERENDSIZE = 0;

    void EndPerm(void) {
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

    void ResetCharacterBuffer(i32 force_reset) {
        if (force_reset != 0 || Area == -1 || Area != last_area) {
            characterbuffer_ptr = characterbuffer_base;
            memset(characterbuffer_base.void_ptr, 0, CHARACTERBUFFERSIZE);
            apicharsys->loaded_model_count = apicharsys->permanent_model_count;
            apicharsys->animation_load_attempts = apicharsys->area_animation_count;
            apicharsys->loaded_animation_count = apicharsys->animation_load_attempts;
        }
    }
}
