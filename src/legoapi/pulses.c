#include "saga.h"

#include <string.h>
typedef struct PULSE_PC {
    NUSPECIALHANDLE special;
    f32 on_time, off_time, timer, start_wait;
    u16 active, disabled;
    char gizmo_name[16];
    void *gizmo;
} PULSE_PC;
struct level_sfx_pulse_prefix_s {
    PULSE_PC *pulses;
    u16 pulse_count;
    i16 sfx_turn_on, sfx_turn_off, sfx_on_loop, sfx_off_loop, sfx_hit_player;
    f32 collide_radius, hit_direction_line, hit_direction_radius_origin;
    i16 debris_hit_player;
    u8 radial_hit_direction, unknown_1f;
};
typedef struct PULSE_WORLD_VIEW_PC {
    u8 unknown_0[0x5050];
    LEVEL_SFX_PULSE_PC *pulses_sys;
} PULSE_WORLD_VIEW_PC;
STATIC_ASSERT(sizeof(PULSE_PC) == 0x34);
STATIC_ASSERT(offsetof(PULSE_PC, on_time) == 0xc);
STATIC_ASSERT(offsetof(PULSE_PC, off_time) == 0x10);
STATIC_ASSERT(offsetof(PULSE_PC, timer) == 0x14);
STATIC_ASSERT(offsetof(PULSE_PC, start_wait) == 0x18);
STATIC_ASSERT(offsetof(PULSE_PC, active) == 0x1c);
STATIC_ASSERT(offsetof(PULSE_PC, disabled) == 0x1e);
STATIC_ASSERT(offsetof(PULSE_PC, gizmo_name) == 0x20);
STATIC_ASSERT(offsetof(PULSE_PC, gizmo) == 0x30);
STATIC_ASSERT(sizeof(LEVEL_SFX_PULSE_PC) == 0x20);
STATIC_ASSERT(offsetof(LEVEL_SFX_PULSE_PC, pulse_count) == 4);
STATIC_ASSERT(offsetof(LEVEL_SFX_PULSE_PC, sfx_turn_on) == 6);
STATIC_ASSERT(offsetof(LEVEL_SFX_PULSE_PC, sfx_turn_off) == 8);
STATIC_ASSERT(offsetof(LEVEL_SFX_PULSE_PC, sfx_on_loop) == 0xa);
STATIC_ASSERT(offsetof(LEVEL_SFX_PULSE_PC, sfx_off_loop) == 0xc);
STATIC_ASSERT(offsetof(LEVEL_SFX_PULSE_PC, sfx_hit_player) == 0xe);
STATIC_ASSERT(offsetof(LEVEL_SFX_PULSE_PC, collide_radius) == 0x10);
STATIC_ASSERT(offsetof(LEVEL_SFX_PULSE_PC, hit_direction_line) == 0x14);
STATIC_ASSERT(offsetof(LEVEL_SFX_PULSE_PC, hit_direction_radius_origin) == 0x18);
STATIC_ASSERT(offsetof(LEVEL_SFX_PULSE_PC, debris_hit_player) == 0x1c);
STATIC_ASSERT(offsetof(LEVEL_SFX_PULSE_PC, radial_hit_direction) == 0x1e);
STATIC_ASSERT(offsetof(PULSE_WORLD_VIEW_PC, pulses_sys) == 0x5050);
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

void __cdecl Pulses_Configure(WORLDINFO *world, char *config) {
    NUFPAR_PC *parser;
    LEVEL_SFX_PULSE_PC pulse_sys = {0};
    PULSE_PC *pulse;
    ((PULSE_WORLD_VIEW_PC *)world)->pulses_sys = NULL;
    if (world->current_gscn == NULL) {
        return;
    }

    parser = NuFParCreateMem("pulses", config, 0xffff);
    if (parser == NULL) {
        return;
    }

    world->giz_buffer.addr = ((world->giz_buffer.addr + 3U) & ~3U);

    pulse = (PULSE_PC *)world->giz_buffer.addr;
    pulse_sys.pulses = pulse;
    pulse_sys.sfx_turn_on = -1;
    pulse_sys.sfx_turn_off = -1;
    pulse_sys.sfx_on_loop = -1;
    pulse_sys.sfx_off_loop = -1;
    pulse_sys.sfx_hit_player = -1;
    pulse_sys.collide_radius = 1.0f;
    pulse_sys.debris_hit_player = -1;
    pulse_sys.radial_hit_direction = 1;

outer_lines:
    while (NuFParGetLine(parser) != 0) {
        if (NuFParGetWord(parser) == 0)
            break;
        if (NuStrICmp(parser->word_buffer_ptr, "pulses_start") == 0) {
            for (;;) {
                if (NuFParGetLine(parser) == 0)
                    goto outer_lines;
                memset(pulse, 0, sizeof(*pulse));
                if (NuFParGetWord(parser) == 0) {
                    continue;
                }

                if (NuStrICmp(parser->word_buffer_ptr, "pulses_end") == 0) {
                    break;
                }

                if (NuStrICmp(parser->word_buffer_ptr, "sfx_onloop") == 0) {
                    if (NuFParGetWord(parser) != 0) {
                        pulse_sys.sfx_on_loop = (i16)GetSfxId(parser->word_buffer_ptr);
                    }
                } else if (NuStrICmp(parser->word_buffer_ptr, "sfx_offloop") == 0) {
                    if (NuFParGetWord(parser) != 0) {
                        pulse_sys.sfx_off_loop = (i16)GetSfxId(parser->word_buffer_ptr);
                    }
                } else if (NuStrICmp(parser->word_buffer_ptr, "sfx_turnon") == 0) {
                    if (NuFParGetWord(parser) != 0) {
                        pulse_sys.sfx_turn_on = (i16)GetSfxId(parser->word_buffer_ptr);
                    }
                } else if (NuStrICmp(parser->word_buffer_ptr, "sfx_turnoff") == 0) {
                    if (NuFParGetWord(parser) != 0) {
                        pulse_sys.sfx_turn_off = (i16)GetSfxId(parser->word_buffer_ptr);
                    }
                } else if (NuStrICmp(parser->word_buffer_ptr, "sfx_hitplayer") == 0) {
                    if (NuFParGetWord(parser) != 0) {
                        pulse_sys.sfx_hit_player = (i16)GetSfxId(parser->word_buffer_ptr);
                    }
                } else if (NuStrICmp(parser->word_buffer_ptr, "collide_radius") == 0) {
                    pulse_sys.collide_radius = NuFParGetFloat(parser);
                } else if (NuStrICmp(parser->word_buffer_ptr, "hit_direction_line") == 0) {
                    pulse_sys.hit_direction_line = NuFParGetFloat(parser);
                    pulse_sys.hit_direction_radius_origin = NuFParGetFloat(parser);
                    pulse_sys.radial_hit_direction = 0;
                } else if (NuStrICmp(parser->word_buffer_ptr, "hit_direction_radius_origin") == 0) {
                    pulse_sys.hit_direction_line = NuFParGetFloat(parser);
                    pulse_sys.hit_direction_radius_origin = NuFParGetFloat(parser);
                    pulse_sys.radial_hit_direction = 1;
                } else if (NuStrICmp(parser->word_buffer_ptr, "debris_hitplayer") == 0) {
                    if (NuFParGetWord(parser) != 0) {
                        pulse_sys.debris_hit_player =
                            (i16)FindGameDebris(world->game_debris_sys, parser->word_buffer_ptr);
                    }
                } else if (NuStrICmp(parser->word_buffer_ptr, "pulse") == 0 && NuFParGetWord(parser) != 0 &&
                           NuSpecialFind(world->current_gscn, &pulse->special, parser->word_buffer_ptr, 1) != 0) {
                    while (NuFParGetWord(parser) != 0) {
                        if (NuStrICmp(parser->word_buffer_ptr, "gizmo") == 0) {
                            if (NuFParGetWord(parser) != 0 &&
                                NuStrLen(parser->word_buffer_ptr) < sizeof(pulse->gizmo_name)) {
                                NuStrCpy(pulse->gizmo_name, parser->word_buffer_ptr);
                            }
                        } else if (NuStrICmp(parser->word_buffer_ptr, "on_time") == 0) {
                            pulse->on_time = NuFParGetFloat(parser);
                        } else if (NuStrICmp(parser->word_buffer_ptr, "off_time") == 0) {
                            pulse->off_time = NuFParGetFloat(parser);
                        } else if (NuStrICmp(parser->word_buffer_ptr, "start_wait") == 0) {
                            pulse->start_wait = NuFParGetFloat(parser);
                        }
                    }

                    if (NuSpecialExistsFn(&pulse->special) != 0) {
                        if (pulse->on_time < 0.1f) {
                            pulse->on_time = 0.1f;
                        }
                        if (pulse->off_time < 0.1f) {
                            pulse->off_time = 0.1f;
                        }
                        if (pulse->start_wait < 0.0f) {
                            pulse->start_wait = 0.0f;
                        }
                        if (pulse_sys.collide_radius < 0.1f) {
                            pulse_sys.collide_radius = 0.1f;
                        }
                        ++pulse_sys.pulse_count;
                        ++pulse;
                    }
                }
            }
            break;
        }
    }

    NuFParDestroy(parser);
    if (pulse_sys.pulse_count == 0) {
        return;
    }

    world->giz_buffer.addr = (u32)pulse;
    ((PULSE_WORLD_VIEW_PC *)world)->pulses_sys = (LEVEL_SFX_PULSE_PC *)world->giz_buffer.addr;
    *((PULSE_WORLD_VIEW_PC *)world)->pulses_sys = pulse_sys;
    world->giz_buffer.addr = ((world->giz_buffer.addr + sizeof(LEVEL_SFX_PULSE_PC) + 3U) & ~3U);
}

void __cdecl NuSpecialSetVisibility(NUSPECIALHANDLE *, i32);
i32 __cdecl GizmoGetVisibility(void *, void *);

void __cdecl Pulses_Reset(LEVEL_SFX_PULSE_PC *system) {
    i32 i;
    if (system == NULL)
        return;
    for (i = 0; i < system->pulse_count; ++i) {
        PULSE_PC *pulse;
        NuSpecialSetVisibility(&system->pulses[i].special, 0);
        system->pulses[i].disabled = 0;
        system->pulses[i].active = 0;
        system->pulses[i].timer = system->pulses[i].start_wait;
        pulse = &system->pulses[i];
        if (pulse->gizmo_name[0] != '\0')
            system->pulses[i].gizmo = GizmoFindByName(WorldInfo_CurrentlyActive()->gizmo_sys, -1, pulse->gizmo_name);
    }
}

void __cdecl Pulses_Update(LEVEL_SFX_PULSE_PC *system) {
    WORLDINFO *world;
    NUVEC hit_direction;
    i32 i;
    if (system == NULL)
        return;
    world = WorldInfo_CurrentlyActive();
    if (system->pulse_count == 0)
        return;
    hit_direction.x = system->hit_direction_line;
    hit_direction.y = 0.0f;
    hit_direction.z = system->hit_direction_radius_origin;
    NuVecRotateY(&hit_direction, &hit_direction, 0x4000);
    for (i = 0; i < system->pulse_count; ++i) {
        PULSE_PC *pulse = &system->pulses[i];
        NUVEC *position;
        NUVEC direction;
        f32 collide_radius;
        i32 player_index;
        if (pulse->gizmo != NULL) {
            i32 visible = GizmoGetVisibility(world->gizmo_sys, pulse->gizmo);
            if (visible == 0) {
                system->pulses[i].disabled = 1;
                NuSpecialSetVisibility(&system->pulses[i].special, 0);
            }
        }
        pulse = &system->pulses[i];
        if (pulse->disabled != 0)
            continue;
        position = NuSpecialGetDrawPos(&pulse->special);
        pulse = &system->pulses[i];
        pulse->timer -= FRAMETIME;
        if (pulse->timer <= 0.0f && netclient == 0) {
            if (pulse->active != 0) {
                pulse->active = 0;
                pulse->timer = pulse->off_time;
                GameAudio_PlaySfxById(system->sfx_turn_off, position, 0, 0);
            } else {
                pulse->active = 1;
                pulse->timer = pulse->on_time;
                GameAudio_PlaySfxById(system->sfx_turn_on, position, 0, 0);
            }
            NuSpecialSetVisibility(&system->pulses[i].special, system->pulses[i].active);
            GameAudio_PlaySfxById(system->pulses[i].active != 0 ? system->sfx_on_loop : system->sfx_off_loop, position,
                                  0, 0);
        }
        if (system->pulses[i].active == 0)
            continue;
        collide_radius = system->collide_radius;
        direction.x = hit_direction.x;
        direction.y = 0.0f;
        direction.z = hit_direction.z;
        if (system->radial_hit_direction != 0) {
            direction.x = position->x - hit_direction.x;
            direction.z = position->z - hit_direction.z;
            NuVecNorm(&direction, &direction);
        }
        for (player_index = 0; player_index < 2; ++player_index) {
            GAMEOBJECT *player = Player[player_index];
            NUVEC offset;
            f32 distance;
            union {
                f32 value;
                u32 bits;
            } absolute_distance;
            i32 handle;
            if (player == NULL || (player->flags_1fc & 0x80) == 0 || player->dead != 0 ||
                (LEGOCONTEXT_DOOMED != -1 && player->character_context == LEGOCONTEXT_DOOMED) ||
                !(player->vehicle_hit_timer <= 0.0f) || player->spawn_protection_timer > 0.0f ||
                (player->flags_ef0 & 0x400000) != 0)
                continue;
            NuVecSub(&offset, &player->collision_position, position);
            distance = direction.x * offset.x + direction.z * offset.z;
            if (system->radial_hit_direction == 1) {
                if (distance < 0.0f)
                    continue;
                NuVecRotateY(&offset, &offset, 0x4000);
                distance = direction.x * offset.x + direction.z * offset.z;
            }
            absolute_distance.value = distance;
            absolute_distance.bits &= 0x7fffffffU;
            if (!(absolute_distance.value < collide_radius))
                continue;
            if (system->radial_hit_direction == 0 &&
                (offset.x * system->hit_direction_line + offset.z * system->hit_direction_radius_origin < 0.0f))
                continue;
            GameAudio_PlaySfxById(system->sfx_hit_player, &player->collision_position, 0, 0);
            if (ObjHitObj(NULL, player, 1, 0, 0, 1) == 2) {
                KillRumble(player);
                continue;
            }
            TakeHitRumble(player, 0.7f);
            player->velocity.x = 0.0f;
            player->velocity.z = 0.0f;
            player->flags_e24 = (player->flags_e24 & ~0x70000U) | (distance < 0.0f ? 0x30000 : 0x40000);
            handle = -1;
            AddFiniteShotDebrisEffect(&handle, world->game_debris_sys->entries[system->debris_hit_player].effect,
                                      &player->collision_position, 1);
        }
    }
}
