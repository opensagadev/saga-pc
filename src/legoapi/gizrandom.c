#include "saga.h"
typedef struct GIZFLOW_LOAD_LEVEL_PC {
    u8 unknown_000[0xe9];
    u8 max_timers;
    u8 unknown_0ea[0xb];
    u8 max_specials;
    u8 unknown_0f6[0xd];
    u8 max_randoms;
} GIZFLOW_LOAD_LEVEL_PC;
typedef struct GIZFLOW_TIMER_PC {
    u8 unknown_000[4];
    f32 start_time;
    u8 unknown_008[2];
    u8 flags;
    u8 unknown_00b;
    char name[16];
} GIZFLOW_TIMER_PC;
typedef struct GIZFLOW_RANDOM_PC {
    char name[16];
    i32 output_count;
    i32 output_weights[8];
    u8 unknown_034[8];
} GIZFLOW_RANDOM_PC;
typedef struct GIZFLOW_RANDOM_SYSTEM_PC {
    GIZFLOW_RANDOM_PC *randoms;
    i32 count;
} GIZFLOW_RANDOM_SYSTEM_PC;
typedef struct GIZFLOW_SPECIAL_PC {
    char name[32];
    GAMEANIMSET *anim_set;
    u8 unknown_024[4];
} GIZFLOW_SPECIAL_PC;
typedef struct GIZFLOW_SPECIAL_SYSTEM_PC {
    GIZFLOW_SPECIAL_PC *specials;
    i32 count;
} GIZFLOW_SPECIAL_SYSTEM_PC;
typedef struct GIZFLOW_LOAD_WORLD_PC {
    u8 unknown_000[0x128];
    GIZFLOW_LOAD_LEVEL_PC *level;
    u8 unknown_12c[0x10];
    NUGSCN *current_gscn;
    u8 unknown_140[0x2988];
    void *gizmo_sys;
    u8 unknown_2acc[0x262c];
    GIZFLOW_TIMER_PC *timers;
    i32 timer_count;
    u8 unknown_5100[4];
    GIZFLOW_RANDOM_SYSTEM_PC *randoms;
    GIZFLOW_SPECIAL_SYSTEM_PC *specials;
} GIZFLOW_LOAD_WORLD_PC;
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_LEVEL_PC, max_timers) == 0xe9);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_LEVEL_PC, max_specials) == 0xf5);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_LEVEL_PC, max_randoms) == 0x103);
STATIC_ASSERT(sizeof(GIZFLOW_TIMER_PC) == 0x1c);
STATIC_ASSERT(offsetof(GIZFLOW_TIMER_PC, start_time) == 4);
STATIC_ASSERT(offsetof(GIZFLOW_TIMER_PC, flags) == 0xa);
STATIC_ASSERT(offsetof(GIZFLOW_TIMER_PC, name) == 0xc);
STATIC_ASSERT(sizeof(GIZFLOW_RANDOM_PC) == 0x3c);
STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_PC, output_count) == 0x10);
STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_PC, output_weights) == 0x14);
STATIC_ASSERT(sizeof(GIZFLOW_SPECIAL_PC) == 0x28);
STATIC_ASSERT(offsetof(GIZFLOW_SPECIAL_PC, anim_set) == 0x20);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, level) == 0x128);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, current_gscn) == 0x13c);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, gizmo_sys) == 0x2ac8);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, timers) == 0x50f8);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, timer_count) == 0x50fc);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, randoms) == 0x5104);
STATIC_ASSERT(offsetof(GIZFLOW_LOAD_WORLD_PC, specials) == 0x5108);

STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_PC, name) == 0);
STATIC_ASSERT(offsetof(GIZFLOW_SPECIAL_PC, name) == 0);
STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_SYSTEM_PC, randoms) == 0);
STATIC_ASSERT(offsetof(GIZFLOW_RANDOM_SYSTEM_PC, count) == 4);
STATIC_ASSERT(offsetof(GIZFLOW_SPECIAL_SYSTEM_PC, specials) == 0);
STATIC_ASSERT(offsetof(GIZFLOW_SPECIAL_SYSTEM_PC, count) == 4);

// NOLINTNEXTLINE(readability-identifier-naming) -- retained genuine subsystem public name.
GIZMO_PC *__cdecl createGizRandom(void *unused, i32 output_count, i32 *weights, char *name) {
    GIZFLOW_LOAD_WORLD_PC *world = (GIZFLOW_LOAD_WORLD_PC *)WorldInfo_CurrentlyLoading();
    GIZFLOW_RANDOM_PC *random;
    i32 i;
    if (world == NULL || world->randoms->count == world->level->max_randoms)
        return NULL;
    random = &world->randoms->randoms[world->randoms->count];
    random->output_count = output_count;
    for (i = 0; i < output_count; ++i)
        random->output_weights[i] = weights[i];
    NuStrNCpy(random->name, name, 16);
    ++world->randoms->count;
    return AddGizmo(world->gizmo_sys, gizrandom_gizmotype_id, NULL, random);
}
