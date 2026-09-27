#include "saga.h"

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
