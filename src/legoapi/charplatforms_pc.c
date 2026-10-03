#include "saga.h"
typedef struct terrain_player_platform_record_pc_s {
    u8 unknown_0[0xe];
    i16 platform_id;
    GAMEOBJECT *object;
} TERRAIN_PLAYER_PLATFORM_RECORD_PC;
typedef struct terrain_player_platform_sys_pc_s {
    u8 unknown_0[4];
    i32 count;
    TERRAIN_PLAYER_PLATFORM_RECORD_PC entries[1];
} TERRAIN_PLAYER_PLATFORM_SYS_PC;
STATIC_ASSERT(sizeof(TERRAIN_PLAYER_PLATFORM_RECORD_PC) == 0x14);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_PLATFORM_RECORD_PC, platform_id) == 0xe);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_PLATFORM_RECORD_PC, object) == 0x10);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_PLATFORM_SYS_PC, count) == 4);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_PLATFORM_SYS_PC, entries) == 8);
GAMEOBJECT *__cdecl CharPlatform_FindObjFromPlatID(void *system, i32 id) {
    TERRAIN_PLAYER_PLATFORM_SYS_PC *platforms = (TERRAIN_PLAYER_PLATFORM_SYS_PC *)system;
    i32 i;
    if (platforms != NULL) {
        for (i = 0; i < platforms->count; ++i) {
            if (platforms->entries[i].platform_id == id && platforms->entries[i].object != NULL)
                return platforms->entries[i].object;
        }
    }
    return NULL;
}
