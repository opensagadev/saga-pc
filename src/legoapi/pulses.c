#include "saga.h"

typedef struct level_sfx_pulse_prefix_s {
    u8 unknown_00[6];
    i16 sfx_turn_on;
    i16 sfx_turn_off;
    i16 sfx_on_loop;
    i16 sfx_off_loop;
    i16 sfx_hit_player;
} LEVEL_SFX_PULSE_PC;
STATIC_ASSERT(offsetof(LEVEL_SFX_PULSE_PC, sfx_turn_on) == 6);
STATIC_ASSERT(offsetof(LEVEL_SFX_PULSE_PC, sfx_turn_off) == 8);
STATIC_ASSERT(offsetof(LEVEL_SFX_PULSE_PC, sfx_off_loop) == 0xc);
STATIC_ASSERT(offsetof(LEVEL_SFX_PULSE_PC, sfx_on_loop) == 0xa);
STATIC_ASSERT(offsetof(LEVEL_SFX_PULSE_PC, sfx_hit_player) == 0xe);

void __cdecl AddLevelSfxFromId(i32, i32 *, i32 *, i32);
void __cdecl Pulses_AddSfx(LEVEL_SFX_PULSE_PC *system, i32 *ids, i32 *count, i32 capacity) {
    if (system == NULL)
        return;
    if (system->sfx_on_loop != -1)
        AddLevelSfxFromId(system->sfx_on_loop, ids, count, capacity);
    if (system->sfx_off_loop != -1)
        AddLevelSfxFromId(system->sfx_off_loop, ids, count, capacity);
    if (system->sfx_turn_on != -1)
        AddLevelSfxFromId(system->sfx_turn_on, ids, count, capacity);
    if (system->sfx_turn_off != -1)
        AddLevelSfxFromId(system->sfx_turn_off, ids, count, capacity);
    if (system->sfx_hit_player != -1)
        AddLevelSfxFromId(system->sfx_hit_player, ids, count, capacity);
}
