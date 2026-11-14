#include "saga.h"

#include <string.h>

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. Retail keeps this
 * TU's out-of-line copy at 0x5a2a60. */
static f32 PulsesFabsPc(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

typedef struct PULSE_PC {
    NUSPECIALHANDLE special; // 0x00
    f32 on_time;             // 0x0c
    f32 off_time;            // 0x10
    f32 timer;               // 0x14
    f32 start_wait;          // 0x18
    u16 active;              // 0x1c
    u16 disabled;            // 0x1e
    char gizmo_name[16];     // 0x20
    void *gizmo;             // 0x30
} PULSE_PC;                  // size 0x34

struct level_sfx_pulse_prefix_s {
    PULSE_PC *pulses;                // 0x00
    u16 pulse_count;                 // 0x04
    i16 sfx_turn_on;                 // 0x06
    i16 sfx_turn_off;                // 0x08
    i16 sfx_on_loop;                 // 0x0a
    i16 sfx_off_loop;                // 0x0c
    i16 sfx_hit_player;              // 0x0e
    f32 collide_radius;              // 0x10
    f32 hit_direction_line;          // 0x14
    f32 hit_direction_radius_origin; // 0x18
    i16 debris_hit_player;           // 0x1c
    u8 radial_hit_direction;         // 0x1e
    u8 unknown_1f;                   // 0x1f
}; // size 0x20

typedef struct PULSE_WORLD_VIEW_PC {
    u8 unknown_0[0x5050];           // 0x0000
    LEVEL_SFX_PULSE_PC *pulses_sys; // 0x5050
} PULSE_WORLD_VIEW_PC;              // size 0x5054

void Pulses_AddSfx(LEVEL_SFX_PULSE_PC *system, i32 *ids, i32 *count, i32 capacity) {
    if (system == NULL) {
        return;
    }
    if (system->sfx_on_loop != -1) {
        AddLevelSfxFromId(system->sfx_on_loop, ids, count, capacity);
    }
    if (system->sfx_off_loop != -1) {
        AddLevelSfxFromId(system->sfx_off_loop, ids, count, capacity);
    }
    if (system->sfx_turn_on != -1) {
        AddLevelSfxFromId(system->sfx_turn_on, ids, count, capacity);
    }
    if (system->sfx_turn_off != -1) {
        AddLevelSfxFromId(system->sfx_turn_off, ids, count, capacity);
    }
    if (system->sfx_hit_player != -1) {
        AddLevelSfxFromId(system->sfx_hit_player, ids, count, capacity);
    }
}

void Pulses_Configure(WORLDINFO *world, char *config) {
    NUFPAR_PC *parser;
    LEVEL_SFX_PULSE_PC pulse_sys;
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
    memset(&pulse_sys, 0, sizeof(pulse_sys));

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
        if (NuFParGetWord(parser) == 0) {
            break;
        }
        if (NuStrICmp(parser->word_buffer_ptr, "pulses_start") == 0) {
            for (;;) {
                if (NuFParGetLine(parser) == 0) {
                    goto outer_lines;
                }
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
                                NuStrLen(parser->word_buffer_ptr) < (i32)sizeof(pulse->gizmo_name)) {
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
    if (pulse_sys.pulse_count <= 0) {
        return;
    }

    world->giz_buffer.addr = (u32)pulse;
    ((PULSE_WORLD_VIEW_PC *)world)->pulses_sys = (LEVEL_SFX_PULSE_PC *)world->giz_buffer.addr;
    *((PULSE_WORLD_VIEW_PC *)world)->pulses_sys = pulse_sys;
    world->giz_buffer.addr = ((world->giz_buffer.addr + sizeof(LEVEL_SFX_PULSE_PC) + 3U) & ~3U);
}

void Pulses_Reset(LEVEL_SFX_PULSE_PC *system) {
    i32 i;
    if (system == NULL) {
        return;
    }
    for (i = 0; i < system->pulse_count; ++i) {
        NuSpecialSetVisibility(&system->pulses[i].special, 0);
        system->pulses[i].disabled = 0;
        system->pulses[i].active = 0;
        system->pulses[i].timer = system->pulses[i].start_wait;
        if (system->pulses[i].gizmo_name[0] != '\0') {
            system->pulses[i].gizmo =
                GizmoFindByName(WorldInfo_CurrentlyActive()->gizmo_sys, -1, system->pulses[i].gizmo_name);
        }
    }
}

void Pulses_Update(LEVEL_SFX_PULSE_PC *system) {
    WORLDINFO *world;
    NUVEC hit_direction;
    i32 i;
    i32 handle;
    if (system == NULL) {
        return;
    }
    world = WorldInfo_CurrentlyActive();
    if (system->pulse_count == 0) {
        return;
    }
    hit_direction.x = system->hit_direction_line;
    hit_direction.y = 0.0f;
    hit_direction.z = system->hit_direction_radius_origin;
    NuVecRotateY(&hit_direction, &hit_direction, 0x4000);
    for (i = 0; i < system->pulse_count; ++i) {
        NUVEC *position;
        NUVEC direction;
        f32 collide_radius;
        i32 player_index;
        if (system->pulses[i].gizmo != NULL) {
            i32 visible = GizmoGetVisibility(world->gizmo_sys, system->pulses[i].gizmo);
            if (visible == 0) {
                system->pulses[i].disabled = 1;
                NuSpecialSetVisibility(&system->pulses[i].special, 0);
            }
        }
        if (system->pulses[i].disabled != 0) {
            continue;
        }
        position = NuSpecialGetDrawPos(&system->pulses[i].special);
        system->pulses[i].timer -= FRAMETIME;
        if (system->pulses[i].timer <= 0.0f && netclient == 0) {
            if (system->pulses[i].active != 0) {
                system->pulses[i].active = 0;
                system->pulses[i].timer = system->pulses[i].off_time;
                GameAudio_PlaySfxById(system->sfx_turn_off, position, 0, 0);
            } else {
                system->pulses[i].active = 1;
                system->pulses[i].timer = system->pulses[i].on_time;
                GameAudio_PlaySfxById(system->sfx_turn_on, position, 0, 0);
            }
            NuSpecialSetVisibility(&system->pulses[i].special, system->pulses[i].active);
            if (system->pulses[i].active != 0) {
                GameAudio_PlaySfxById(system->sfx_on_loop, position, 0, 0);
            } else {
                GameAudio_PlaySfxById(system->sfx_off_loop, position, 0, 0);
            }
        }
        if (system->pulses[i].active == 0) {
            continue;
        }
        collide_radius = system->collide_radius;
        if (system->radial_hit_direction == 0) {
            direction.x = hit_direction.x;
            direction.y = 0.0f;
            direction.z = hit_direction.z;
        } else {
            direction.x = position->x - hit_direction.x;
            direction.y = 0.0f;
            direction.z = position->z - hit_direction.z;
            NuVecNorm(&direction, &direction);
        }
        for (player_index = 0; player_index < 2; ++player_index) {
            GAMEOBJECT *player = Player[player_index];
            NUVEC offset;
            f32 distance;
            if (player == NULL || (player->flags_1fc & 0x80) == 0 || player->dead != 0 ||
                (LEGOCONTEXT_DOOMED != -1 && player->character_context == LEGOCONTEXT_DOOMED) ||
                !(player->vehicle_hit_timer <= 0.0f) || player->spawn_protection_timer > 0.0f ||
                (player->flags_ef0 & 0x400000) != 0) {
                continue;
            }
            NuVecSub(&offset, &player->collision_position, position);
            distance = direction.x * offset.x + direction.z * offset.z;
            if (system->radial_hit_direction == 1) {
                if (distance < 0.0f) {
                    continue;
                }
                NuVecRotateY(&offset, &offset, 0x4000);
                distance = direction.x * offset.x + direction.z * offset.z;
            }
            if (!(PulsesFabsPc(distance) < collide_radius)) {
                continue;
            }
            if (system->radial_hit_direction == 0) {
                f32 side = offset.x * system->hit_direction_line + offset.z * system->hit_direction_radius_origin;
                if (side < 0.0f) {
                    continue;
                }
            }
            GameAudio_PlaySfxById(system->sfx_hit_player, &player->collision_position, 0, 0);
            if (ObjHitObj(NULL, player, 1, 0, 0, 1) == 2) {
                KillRumble(player);
                continue;
            }
            TakeHitRumble(player, 0.7f);
            player->hit_side = distance < 0.0f ? 3 : 4;
            player->velocity.x = 0.0f;
            player->velocity.z = 0.0f;
            handle = -1;
            AddFiniteShotDebrisEffect(&handle, world->game_debris_sys->entries[system->debris_hit_player].effect,
                                      &player->collision_position, 1);
        }
    }
}
