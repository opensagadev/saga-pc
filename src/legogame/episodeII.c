#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/edptl.h"
#include "globals.h"
#include "legoapi/ai/game/creature.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/cutscenes/gcutscn.h"
#include "legoapi/gizmo/base/gizmessage.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmos/object/gizobstacles.h"
#include "legoapi/gizmos/object/gizpanel.h"
#include "legoapi/gizmos/object/newblowup.h"
#include "legoapi/gizmos/traps/gizforce.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/camera.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/render/core/render.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/light/surfaces.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episodeII.h"
#include "legoapi/world/levels/episodeIII.h"
#include "legoapi/world/levels/episodeV.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "legogame/gamestatus.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nugcutscene.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

#define BONUS_GUNSHIP_LEVFLAGS ((BONUS_GUNSHIP_FLAGS *)LevFlag)

/* The Factory B paint puzzle state shared with the network partner (SetLevelHack(4)). */
typedef struct factoryb_netpacket_s { // size 0x4
    u8 unknown_0;                     // 0x0 cleared every update
    i8 paint_try;                     // 0x1 painttry, the paint lights that are lit
    u8 cylinder_up;                   // 0x2 the bonus cylinder rose (stage 4)
    i8 paint_target;                  // 0x3 painttarget, 0 or the target colour + 3
} FACTORYB_NETPACKET;

typedef struct dooku_c_s {
    GIZAIMESSAGE *total_message;
    GIZAIMESSAGE *hit_message;
    nuhspecial_s node;
} DOOKU_C;

typedef struct kaminodisco_s {
    AIAREA *area;
    nuhspecial_s off[16];
    nuhspecial_s pending[16];
    nuhspecial_s occupied[16];
    nuhspecial_s complete[16];
    nuhspecial_s final[16];
    i8 tile_state[16];
    i8 tile_count;
    i8 phase;
    i8 tile[2];
    f32 timer;
    u8 completion_sound;
    u8 reserved_3dd[3];
    GIZAIMESSAGE *next_tile_message;
    GIZAIMESSAGE *complete_message;
} KAMINODISCO;

typedef struct kamino_progress_gizmo_reader_s {
    u8 opaque_00_94[0x94];
    u8 flags;
} KAMINO_PROGRESS_GIZMO_READER;

typedef struct kamino_e_s {
    GIZAIMESSAGE *jango_fight;
    GIZAIMESSAGE *can_fire;
    GIZAIMESSAGE *reset_turrets;
    GIZAIMESSAGE *show_hearts;
    GIZAIMESSAGE *minicut_started;
    CUTINFO *intro_cutscene;
    AIAREA *landing_area;
    nuhspecial_s slave1;
    GIZTURRET_s *turrets[4];
    GIZTURRET_s *active_turret;
    void *panels[4];
    NUVEC position;
    i32 pitch;
    i32 yaw;
    i32 roll;
    i32 orbit_pitch;
    i32 orbit_yaw;
    f32 departure_timer;
    f32 elapsed_time;
    i8 fire_index;
    u8 departing;
    i16 platform;
} KAMINO_E;

extern i32 turret_gizmotype_id;
void *GizmoFindByData(void *gizmo_sys, i32 type_id, void *data);
void KaminoEGizmoMissing(char *name);
#define KAMINO_DISCO_AREA_MASK_SET(world, area)                                                                                            \
    ((*(u64 *)((GAMEOBJECT *)(world)->ai_sys->player_1)->ai_area_mask & (u64)(i64)(1 << (i32)((area) - (world)->ai_sys->areas))) != 0)
#define KAMINO_DISCO_PLAYER_IN_AREA(world, who, area) ((who) != NULL && (area) != NULL && KAMINO_DISCO_AREA_MASK_SET(world, area))

static i32 PaintMixColour(i32 first, i32 second) {
    if ((first == 0 && second == 1) || (first == 1 && second == 0)) {
        return 3;
    }
    if ((first == 2 && second == 1) || (first == 1 && second == 2)) {
        return 4;
    }
    if ((first == 0 && second == 2) || (first == 2 && second == 0)) {
        return 5;
    }
    return -1;
}

i32 randpaints[3] = {0};

static void RandomisePaints(void) {
    i32 i = 0;
    do {
        randpaints[i] = qrand() / 21846;
        if (i == 0) {
            i++;
        } else if (i == 1) {
            if (randpaints[i] != randpaints[0]) {
                i++;
            }
        }
    } while (i < 2);
    randpaints[2] = 3 - randpaints[1] - randpaints[0];
}

nuhspecial_s paintlights[3] = {0};
nuhspecial_s painttargetcolour[3] = {0};

void DrawPaintLights(void) {
    i32 i;
    for (i = 0; i < factoryb_netpacket->paint_try; i++) {
        if (NuSpecialExistsFn(&paintlights[i]) != 0) {
            NuSpecialDrawAt(&green_light, NuSpecialGetDrawMtx(&paintlights[i]));
        }
    }
    for (i = 0; i < 3; i++) {
        i8 colour = factoryb_netpacket->paint_target;
        if (i == colour - 3 || colour == 0) {
            NuSpecialSetVisibility(&painttargetcolour[i], 1);
        } else {
            NuSpecialSetVisibility(&painttargetcolour[i], 0);
        }
    }
}

i32 PAINTPUZZLESTAGE = 0;
i32 JediBActiveCount = 0;
i32 painttry = 0;
AREA_DATA *KAMINO_ADATA = 0;
struct gizmo_s *paintobst[6] = {0};

void ResetPaintPuzzle(WORLDINFO *world) {
    i32 i;
    for (i = 0; i < 6; i++) {
        GizObstacle_JumpToStart(paintobst[i]->object);
    }
    painttry = 0;
    PAINTPUZZLESTAGE = 1;
    RandomisePaints();
}

i32 paintmixed = 0;
i32 painttarget = 0;
i32 paintsused[2] = {0};
i32 played_sound = 0;
struct gizmo_s *paintpanel[3] = {0};
KAMINO_DISCO_MUSIC_VIEW KaminoDiscoMusicState = {0};

void UpdatePaintPuzzle(WORLDINFO *world) {
    i32 i;
    i32 used = 0;
    switch (PAINTPUZZLESTAGE) {
        case 0:
            GizObstacle_PlayBackwards(paintobst[paintmixed]->object);
            for (i = 0; i < 3; i++) {
                GizPanel_Reset(paintpanel[i]->object);
            }
            PAINTPUZZLESTAGE = 1;
            break;
        case 1:
            if (paintmixed != 0 && (((GIZOBSTACLE *)paintobst[paintmixed]->object)->anim_set->flags & GAMEANIMSET_PLAYING) != 0) {
                break;
            }
            paintmixed = 0;
            painttarget = randpaints[painttry] + 3;
            PAINTPUZZLESTAGE = 2;
            break;
        case 2:
            for (i = 0; i < 3; i++) {
                if (((GIZOBSTACLE *)paintobst[i]->object)->anim_set->state == GAMEANIMSET_STATE_AT_END) {
                    paintsused[used++] = i;
                }
            }
            if (used == 2) {
                paintmixed = PaintMixColour(paintsused[0], paintsused[1]);
                PAINTPUZZLESTAGE = 3;
            } else {
                for (i = 0; i < 3; i++) {
                    if (GizmoGetOutput(world->gizmo_sys, paintpanel[i], 0, 0) != 0 &&
                        ((GIZOBSTACLE *)paintobst[i]->object)->anim_set->state == GAMEANIMSET_STATE_AT_START) {
                        GizObstacle_PlayForwards(paintobst[i]->object);
                    }
                }
            }
            break;
        case 3: {
            GIZOBSTACLE *first = paintobst[paintsused[0]]->object;
            if (first->anim_set->state == GAMEANIMSET_STATE_AT_END &&
                ((GIZOBSTACLE *)paintobst[paintsused[1]]->object)->anim_set->state == GAMEANIMSET_STATE_AT_END) {
                GizObstacle_PlayBackwards(first);
                GizObstacle_PlayBackwards(paintobst[paintsused[1]]->object);
                GizObstacle_PlayForwards(paintobst[paintmixed]->object);
            } else if (((GIZOBSTACLE *)paintobst[paintmixed]->object)->anim_set->state == GAMEANIMSET_STATE_AT_END) {
                if (paintmixed == painttarget) {
                    painttry++;
                    if (painttry != 3) {
                        PAINTPUZZLESTAGE = 0;
                    } else {
                        PAINTPUZZLESTAGE = 4;
                    }
                } else {
                    RandomisePaints();
                    painttry = 0;
                    PAINTPUZZLESTAGE = 0;
                }
            }
            break;
        }
        case 4:
            GizObstacle_PlayForwards(forcetube->object);
            factoryb_netpacket->cylinder_up = 1;
            break;
        default:
            break;
    }
    factoryb_netpacket->unknown_0 = 0;
    factoryb_netpacket->paint_try = (i8)painttry;
    factoryb_netpacket->paint_target = (i8)painttarget;
    if (factoryb_netpacket->cylinder_up == 1 && played_sound == 0) {
        PlaySfx("Fac_BonusCylUp", &NuSpecialGetMtx(&((GIZOBSTACLE *)forcetube->object)->anim_set->objects->special)->translation);
        played_sound = 1;
    }
}

static i32 JediBInitLocator(WORLDINFO *world, NUVEC *position, i32 angle, AILOCATOR *locator, f32 radius) {
    f32 floor_y;
    if (netclient == 0) {
        AISYS *ai = world->ai_sys;
        if (ai != NULL) {
            i32 index;
            i32 count = ai->antinode_count;
            AIANTINODE *node = ai->antinodes;
            for (index = 0; index < count; index++, node++) {
                if (node->radius != 0.0f) {
                    f32 dx = position->x - node->position.x;
                    f32 dz = position->z - node->position.z;
                    f32 dist_sqr = dx * dx + dz * dz;
                    f32 r = node->radius + 1.0f;
                    if (dist_sqr < r * r) {
                        f32 scale = r / NuFsqrt(dist_sqr);
                        dx *= scale;
                        position->x = dx + node->position.x;
                        dz *= scale;
                        position->z = dz + node->position.z;
                        break;
                    }
                }
            }
        }
        memset(locator, 0, sizeof(AILOCATOR));
        if (position->x * position->x + position->z * position->z < radius * radius) {
            floor_y = GameShadow(NULL, position, 5.0f, 0);
            if (floor_y != 2000000.0f) {
                position->y = floor_y;
            }
            AISysGetPathPos(world->ai_sys, position, &locator->path_info, NULL, 0xff);
            if ((locator->path_info.flags & AIPATHINFO_FLAG_ON_PATH) != 0) {
                locator->position = *position;
                locator->rotation = angle;
                return 1;
            }
        }
    }
    return 0;
}

static void JediBResetRecord(JEDIB_CREATURE *record) {
    if (record == NULL) {
        return;
    }
    if (record->flags & 2) {
        if (FreePlay == 0 && (jedi_b.unique_spawned & 1) == 0) {
            record->id = dat_007f19d8;
            jedi_b.unique_spawned |= 1;
        } else if (FreePlay == 0 && (jedi_b.unique_spawned & 2) == 0) {
            record->id = dat_007f19d4;
            jedi_b.unique_spawned |= 2;
        } else if (NuRandFloat() < 0.5f) {
            record->id = id_BATTLEDROIDGEONOSIAN;
        } else {
            record->id = id_GEONOSIAN;
        }
    } else {
        record->id = id_BOB;
    }
    record->flags &= ~0x1cu;
    record->unknown_58 = 0;
}

static f32 jedib_create_step_Normal = 2.0f;
NUVEC jedib_offsets[3] = {{0.1f, -0.1f, -0.1f}, {-0.1f, -0.1f, -0.1f}, {-0.1f, -0.1f, 0.1f}};
i32 jedib_yrot[3] = {0x6000, 0xa000, 0xe000};
i16 *JediB_playerids[3] = {&dat_007f19c8, &id_ANAKINPADAWAN, &dat_007f19d0};
static f32 jedib_inner = 3.0f;
static i32 jedib_max_baddies_per_goody = 3;
static i32 jedib_min_baddies_per_goody = 1;
static f32 jedib_offset = 0.5f;
static f32 jedib_outer_r = 8.0f;
f32 jedib_safe_r = 9.0f;
static f32 jedib_proximity = 0.5f;
static i32 jedib_seed = 17;

void JediB_Init(WORLDINFO *world) {
    NUVEC position;
    NUVEC offset;
    AILOCATOR locator;
    JEDIB_CREATURE *goody;
    JEDIB_CREATURE *baddie;
    f32 step;
    f32 x;
    f32 z;
    f32 distance;
    f32 floor_y;
    i32 angle;
    i32 count;
    i32 index;
    if (Mission_Active(MissionSys) != NULL) {
        return;
    }
    memset(&jedi_b, 0, sizeof(jedi_b));
    jedi_b.seed = jedib_seed;
    jedib_netpacket = SetLevelHack(0x20);
    if (netclient != 0) {
        return;
    }
    for (x = -jedib_outer_r; x <= jedib_outer_r; x += step) {
        step = jedib_create_step_Normal;
        for (z = -jedib_outer_r; z <= jedib_outer_r; z += step) {
            position.x = jedib_offset - jedib_offset * 2.0f * NuRandFloatSeeded(&jedi_b.seed) + x;
            position.y = 0.0f;
            position.z = jedib_offset - jedib_offset * 2.0f * NuRandFloatSeeded(&jedi_b.seed) + z;
            distance = position.x * position.x + position.z * position.z;
            if (distance < jedib_outer_r * jedib_outer_r && distance > jedib_inner * jedib_inner) {
                floor_y = GameShadow(NULL, &position, 5.0f, 0);
                if (floor_y != 2000000.0f) {
                    position.y = floor_y;
                }
                angle = NuRandIntSeeded(&jedi_b.seed);
                if (JediBInitLocator(world, &position, angle, &locator, jedib_outer_r) != 0 && jedi_b.baddie_count < 256) {
                    goody = &jedi_b.baddies[jedi_b.baddie_count++];
                    goody->position = position;
                    goody->locator = locator;
                    goody->flags &= ~2u;
                    offset.x = 0.0f;
                    offset.y = 0.0f;
                    offset.z = jedib_proximity;
                    angle = NuRandIntSeeded(&jedi_b.seed);
                    NuVecRotateY(&offset, &offset, NuRandIntSeeded(&jedi_b.seed));
                    count = NuRandIntSeeded(&jedi_b.seed) % (u32)(jedib_max_baddies_per_goody - jedib_min_baddies_per_goody) +
                            jedib_min_baddies_per_goody;
                    for (index = 0; index < count && jedi_b.baddie_count < 256; index++) {
                        if (index != 0) {
                            angle = NuAngAdd(angle, 0x10000 / count);
                        }
                        NuVecRotateY(&position, &offset, angle);
                        NuVecAdd(&position, &position, &goody->position);
                        if (JediBInitLocator(world, &position, angle, &locator, jedib_outer_r) != 0) {
                            baddie = &jedi_b.baddies[jedi_b.baddie_count++];
                            baddie->flags |= 2;
                            baddie->position = position;
                            baddie->locator = locator;
                            baddie->angle = angle;
                            baddie->partner = goody;
                            goody->partner = baddie;
                        }
                    }
                }
            }
            step = jedib_create_step_Normal;
        }
    }
}

void JediB_Reset(WORLDINFO *world) {
    i32 index;
    i32 hero;
    i32 slot;
    char name[32];
    if (Mission_Active(MissionSys) != NULL || netclient != 0) {
        return;
    }
    for (index = 0; index < jedi_b.baddie_count; index++) {
        jedi_b.baddies[index].object = NULL;
        jedi_b.baddies[index].flags &= ~0x19u;
        jedi_b.baddies[index].id = -1;
    }
    for (index = 0; index < jedi_b.goody_count; index++) {
        jedi_b.goodies[index].object = NULL;
        jedi_b.goodies[index].flags &= ~1u;
    }
    ClearAICreatures();
    for (hero = 0; hero < 3; hero++) {
        sprintf(name, "pillar%d_01a", hero + 1);
        NuSpecialFind(world->current_gscn, &jedi_b.hero_spawns[hero][0], name, 1);
        sprintf(name, "pillar%d_01b", hero + 1);
        NuSpecialFind(world->current_gscn, &jedi_b.hero_spawns[hero][1], name, 1);
        sprintf(name, "pillar%d_01c", hero + 1);
        NuSpecialFind(world->current_gscn, &jedi_b.hero_spawns[hero][2], name, 1);
        sprintf(name, "pillar%d_01ba", hero + 1);
        NuSpecialFind(world->current_gscn, &jedi_b.hero_spawns[hero][3], name, 1);
        jedi_b.heroes[hero] = NULL;
        if (FreePlay == 0) {
            for (slot = 0; slot < 8; slot++) {
                if (Player[slot] != NULL && (i16)Player[slot]->id == *JediB_playerids[hero]) {
                    jedi_b.heroes[hero] = Player[slot];
                    break;
                }
            }
        }
    }
    jedi_b.msg_phase = SetGizAIMessage(gizaimessagesys, "Phase", 0.0f, NULL);
    jedi_b.msg_phase_complete = SetGizAIMessage(gizaimessagesys, "PhaseComplete", 0.0f, NULL);
    jedi_b.msg_objectives_left = SetGizAIMessage(gizaimessagesys, "ObjectivesLeft", 0.0f, NULL);
    jedi_b.msg_restrain[0] = SetGizAIMessage(gizaimessagesys, "RestrainPadme", 0.0f, NULL);
    jedi_b.msg_restrain[1] = SetGizAIMessage(gizaimessagesys, "RestrainAnakin", 0.0f, NULL);
    jedi_b.msg_restrain[2] = SetGizAIMessage(gizaimessagesys, "RestrainObiWan", 0.0f, NULL);
    jedi_b.boss = NULL;
    jedi_b.unique_spawned &= ~3u;
}

static void JediBKilledCallback(GAMEOBJECT *object) {
    i32 index;
    i32 other;
    i32 wave;
    i32 alive = 0;
    if (object == NULL) {
        return;
    }
    for (index = 0; index < jedi_b.goody_count; index++) {
        if (jedi_b.goodies[index].object == object) {
            jedi_b.goodies[index].object = NULL;
            for (other = 0; other < jedi_b.goody_count; other++) {
                if (jedi_b.goodies[other].object != NULL) {
                    alive++;
                }
            }
            if (alive == 0) {
                jedi_b.goody_count = 0;
            }
            return;
        }
    }
    for (index = 0; index < jedi_b.baddie_count; index++) {
        if (jedi_b.baddies[index].object == object) {
            for (wave = 0; wave < 6; wave++) {
                if (jedi_b.wave_ids[wave] == jedi_b.baddies[index].id && jedi_b.wave_spawned[wave] == 0) {
                    jedi_b.wave_spawned[wave] = 1;
                    jedi_b.wave_timer = 0.0f;
                    break;
                }
            }
            jedi_b.baddies[index].object = NULL;
            jedi_b.baddies[index].flags = (jedi_b.baddies[index].flags & ~0x18u) | 1;
            JediBResetRecord(&jedi_b.baddies[index]);
            return;
        }
    }
}

static void JediBStartPhase(WORLDINFO *world) {
    JEDIB_PHASE *phase_lists[3] = {jedi_b_phase1, jedi_b_phase2, jedi_b_phase3};
    JEDIB_PHASE *entry;
    AILOCATOR *locator;
    GAMEOBJECT *spawned;
    char name[16];
    i32 i;
    i32 chosen;
    i32 index;
    i32 id;
    if (netclient != 0) {
        return;
    }
    if (jedi_b.phase + 1 >= 4 && jedi_b.timer < 3.0f) {
        jedi_b.timer += FRAMETIME;
        return;
    }
    jedi_b.phase++;
    if (jedi_b.phase == 6) {
        jedi_b.phase = 7;
    }
    switch (jedi_b.phase) {
        case 1:
        case 2:
        case 3:
            for (entry = phase_lists[jedi_b.phase - 1]; entry != NULL && entry->id != NULL && jedi_b.goody_count < 8; entry++) {
                sprintf(name, "phase%d_%d", jedi_b.phase, jedi_b.goody_count + 1);
                locator = AIPathFindLocator(world->ai_sys, name);
                if (locator == NULL) {
                    continue;
                }
                jedi_b.goodies[jedi_b.goody_count].locator = *locator;
                spawned = AddDynamicCreature(*entry->id, &locator->position, locator->rotation, (char *)entry->script, &locator->path_info,
                                             NULL, 1, NULL, NULL, 0, 0);
                jedi_b.goodies[jedi_b.goody_count].object = spawned;
                if (jedi_b.goodies[jedi_b.goody_count].object == NULL) {
                    continue;
                }
                jedi_b.goodies[jedi_b.goody_count].id = *entry->id;
                jedi_b.goodies[jedi_b.goody_count].object->ai_packet.locator = locator;
                jedi_b.goodies[jedi_b.goody_count].object->portal_flags |= 0x1000000;
                jedi_b.goodies[jedi_b.goody_count].object->tag_object_callback = JediBKilledCallback;
                jedi_b.goody_count++;
            }
            break;
        case 4:
        case 5:
        case 6:
            memset(jedi_b.wave_spawned, 0, sizeof(jedi_b.wave_spawned));
            for (i = 0; i < jedi_b.baddie_count; i++) {
                jedi_b.baddies[i].flags &= ~0x18u;
            }
            for (chosen = 0; chosen < 6; chosen++) {
                index = NuRand(NULL) % jedi_b.baddie_count;
                switch (jedi_b.phase) {
                    case 4:
                        id = id_BATTLEDROIDSECURITY;
                        break;
                    case 5:
                        id = chosen < 3 ? id_BATTLEDROIDSECURITY : id_SUPERBATTLEDROID;
                        break;
                    default:
                        id = chosen < 3 ? id_DROIDEKA : id_SUPERBATTLEDROID;
                        break;
                }
                for (;;) {
                    if ((jedi_b.baddies[index].flags & 8) == 0 && (jedi_b.baddies[index].flags & 2) != 0 &&
                        jedi_b.baddies[index].id != id) {
                        break;
                    }
                    index++;
                    if (index >= jedi_b.baddie_count) {
                        index = 0;
                    }
                }
                if (jedi_b.baddies[index].object != NULL) {
                    RemoveGameObject(jedi_b.baddies[index].object, 1);
                    jedi_b.baddies[index].flags &= ~1u;
                    jedi_b.baddies[index].object = NULL;
                }
                jedi_b.baddies[index].id = id;
                jedi_b.wave_ids[chosen] = jedi_b.baddies[index].id;
                jedi_b.baddies[index].flags |= 8;
            }
            jedi_b.wave_timer = 0.0f;
            break;
    }
    if (jedi_b.msg_phase == NULL) {
        jedi_b.msg_phase = CheckGizAIMessage(gizaimessagesys, "Phase", NULL);
    }
    if (jedi_b.msg_phase_complete == NULL) {
        jedi_b.msg_phase_complete = CheckGizAIMessage(gizaimessagesys, "PhaseComplete", NULL);
    }
    if (jedi_b.msg_objectives_left == NULL) {
        jedi_b.msg_objectives_left = CheckGizAIMessage(gizaimessagesys, "ObjectivesLeft", NULL);
    }
    if (jedi_b.msg_restrain[0] == NULL) {
        jedi_b.msg_restrain[0] = CheckGizAIMessage(gizaimessagesys, "RestrainPadme", NULL);
    }
    if (jedi_b.msg_restrain[1] == NULL) {
        jedi_b.msg_restrain[1] = CheckGizAIMessage(gizaimessagesys, "RestrainAnakin", NULL);
    }
    if (jedi_b.msg_restrain[2] == NULL) {
        jedi_b.msg_restrain[2] = CheckGizAIMessage(gizaimessagesys, "RestrainObiWan", NULL);
    }
    jedi_b.msg_phase->value = (f32)jedi_b.phase;
    jedi_b.msg_objectives_left->value = (f32)jedi_b.goody_count;
    jedi_b.msg_phase_complete->value = 0.0f;
    jedi_b.timer = 0.0f;
    jedi_b.state = 1;
}

static void JediBTickPhase(void) {
    i32 done;
    i32 i;
    i32 release;
    GAMEOBJECT *boss;
    jedi_b.timer += FRAMETIME;
    done = 0;
    switch (jedi_b.phase) {
        case 1:
        case 2:
        case 3:
            jedi_b.msg_objectives_left->value = (f32)jedi_b.goody_count;
            if (jedi_b.msg_phase_complete->value == 1.0f) {
                done = 1;
            }
            break;
        case 4:
        case 5:
        case 6:
            done = 1;
            for (i = 0; i < 6; i++) {
                if (jedi_b.wave_spawned[i] == 0) {
                    done = 0;
                    break;
                }
            }
            jedi_b.wave_timer += FRAMETIME;
            if (jedi_b.wave_timer > 5.0f) {
                release = -1;
                for (i = 0; i < jedi_b.baddie_count; i++) {
                    if (jedi_b.baddies[i].flags & 0x10) {
                        release = -1;
                        break;
                    }
                    if (jedi_b.baddies[i].flags & 8) {
                        release = i;
                    }
                }
                if (release != -1) {
                    jedi_b.baddies[release].flags |= 0x10;
                }
                jedi_b.wave_timer = 0.0f;
            }
            break;
        case 7:
            boss = jedi_b.boss;
            if (boss == NULL) {
                boss = FindGameObject(id_JANGOFETT, APIOBJECT_SIDE_BADDIE, 1, 0, 0);
                jedi_b.boss = boss;
                if (boss == NULL) {
                    break;
                }
            }
            if (boss->dead != 0 || boss->current_hp == 0) {
                done = 1;
            }
            break;
    }
    if (done) {
        jedi_b.state = 2;
        jedi_b.timer = 0.0f;
    }
}

static void JediBEndPhase(WORLDINFO *world) {
    if (jedi_b.timer < (f64)0.1f) {
        jedi_b.timer += FRAMETIME;
        return;
    }
    if (jedi_b.phase >= 7) {
        if (FreePlay == 0) {
            NewLData = dat_0087b0f0;
        }
        CompleteLevel(world);
    }
    jedi_b.state = 0;
    jedi_b.timer = 0.0f;
}

void JediB_Update(WORLDINFO *world) {
    i32 hero;
    i32 spawn;
    i32 slot;
    i32 i;
    i32 in_range;
    u32 shift;
    u64 mask;
    GAMEOBJECT *target;
    GAMEOBJECT *object;
    JEDIB_CREATURE *baddie;
    NUVEC spawn_position;
    NUVEC *pos;
    if (Mission_Active(MissionSys) != NULL || netclient != 0) {
        return;
    }
    for (hero = 0; hero < 3; hero++) {
        if (FreePlay != 0 && jedi_b.heroes[hero] == NULL) {
            for (spawn = 0; spawn < 4; spawn++) {
                if (NuSpecialGetVisibilityFn(&jedi_b.hero_spawns[hero][spawn]) != 0) {
                    pos = NuSpecialGetDrawPos(&jedi_b.hero_spawns[hero][spawn]);
                    jedi_b.heroes[hero] = AddDynamicCreature(*JediB_playerids[hero], pos, 0, "Party", NULL, NULL, 1, NULL, NULL, 0, 0);
                }
            }
        }
        if (jedi_b.heroes[hero] == NULL || jedi_b.msg_restrain[hero]->value != 1.0f) {
            continue;
        }
        for (spawn = 0; spawn < 4; spawn++) {
            if (NuSpecialGetVisibilityFn(&jedi_b.hero_spawns[hero][spawn]) == 0) {
                continue;
            }
            jedi_b.heroes[hero]->position = *NuSpecialGetDrawPos(&jedi_b.hero_spawns[hero][spawn]);
            NuVecAdd(&jedi_b.heroes[hero]->position, &jedi_b.heroes[hero]->position, &jedib_offsets[hero]);
            jedi_b.heroes[hero]->ai_update_position = jedi_b.heroes[hero]->position;
            jedi_b.heroes[hero]->unknown_276 = (u16)jedib_yrot[hero];
            jedi_b.heroes[hero]->velocity.x = 0.0f;
            jedi_b.heroes[hero]->velocity.y = 0.0f;
            jedi_b.heroes[hero]->velocity.z = 0.0f;
            jedi_b.heroes[hero]->respawn_timer = 0.0f;
        }
    }
    jedi_b.exclusion_mask = 0;
    if (jedi_b.timer < 4.0f) {
        if (player != NULL) {
            jedi_b.exclusion_mask = (u64)1 << player->hostility_group;
        }
        if (player2 != NULL) {
            jedi_b.exclusion_mask |= (u64)1 << player2->hostility_group;
        }
    } else if (jedi_b.phase == 7) {
        if (jedi_b.boss != NULL) {
            if (player2 != NULL && jedi_b.boss->ai_packet.opponent == (APIOBJECT *)player2) {
                shift = player2->hostility_group;
            } else {
                shift = player->hostility_group;
            }
            jedi_b.exclusion_mask = (u64)1 << shift;
        }
    } else {
        if (player->collision_position.x * player->collision_position.x + player->collision_position.z * player->collision_position.z >
            jedib_safe_r * jedib_safe_r) {
            jedi_b.exclusion_mask = (u64)1 << player->hostility_group;
        }
        if (player2 != NULL &&
            player2->collision_position.x * player2->collision_position.x + player2->collision_position.z * player2->collision_position.z >
                jedib_safe_r * jedib_safe_r) {
            jedi_b.exclusion_mask |= (u64)1 << player2->hostility_group;
        }
    }
    for (slot = 0; slot < 8; slot++) {
        object = Player[slot];
        if (object == NULL) {
            continue;
        }
        object->excluded_opponents = 0;
        object = Player[slot];
        if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0 && jedi_b.boss != NULL && jedi_b.phase == 7) {
            mask = (u64)1 << jedi_b.boss->hostility_group;
            object->excluded_opponents |= mask;
        }
    }
    switch (jedi_b.state) {
        case 0:
            JediBStartPhase(world);
            break;
        case 1:
            JediBTickPhase();
            break;
        case 2:
            JediBEndPhase(world);
            break;
    }
    JediBActiveCount = 0;
    for (i = 0; i < jedi_b.baddie_count; i++) {
        baddie = &jedi_b.baddies[i];
        if (baddie->object != NULL) {
            if ((baddie->flags & 0x10) != 0) {
                baddie->object->ai_packet.locator = NULL;
                target = NULL;
                if ((player->character_data->model_flags & CHARACTER_MODEL_UNTARGETABLE) == 0 &&
                    ((i16)player->objptr->id != dat_007f19c4 || (player->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) &&
                    (player->character_data->game_character->variant != CHARVARIANT_BATTLEDROID ||
                     baddie->object->character_data->game_character->variant != CHARVARIANT_BATTLEDROID)) {
                    target = player;
                } else {
                    if (baddie->object->ai_packet.opponent == (APIOBJECT *)player) {
                        baddie->object->ai_packet.opponent = NULL;
                    }
                    if (player2 != NULL) {
                        if ((player2->character_data->model_flags & CHARACTER_MODEL_UNTARGETABLE) == 0 &&
                            ((i16)player->objptr->id != dat_007f19c4 || (player->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) &&
                            (player->character_data->game_character->variant != CHARVARIANT_BATTLEDROID ||
                             baddie->object->character_data->game_character->variant != CHARVARIANT_BATTLEDROID)) {
                            target = player2;
                        } else if (baddie->object->ai_packet.opponent == (APIOBJECT *)player2) {
                            baddie->object->ai_packet.opponent = NULL;
                        }
                    }
                }
                if (target != NULL) {
                    baddie->object->ai_packet.opponent = (APIOBJECT *)target;
                }
            } else {
                in_range = (OnOrInsidePlane(&baddie->object->position, &PlayPlane[1].point, &PlayPlane[1].normal, NULL, 2.0f, NULL) == 0 &&
                            OnOrInsidePlane(&baddie->object->position, &PlayPlane[2].point, &PlayPlane[2].normal, NULL, 2.0f, NULL) == 0) ||
                           (OnOrInsidePlane(&baddie->locator.position, &PlayPlane[1].point, &PlayPlane[1].normal, NULL, 2.0f, NULL) == 0 &&
                            OnOrInsidePlane(&baddie->locator.position, &PlayPlane[2].point, &PlayPlane[2].normal, NULL, 2.0f, NULL) == 0);
                if (in_range == 0) {
                    baddie->position = baddie->object->position;
                    baddie->angle = baddie->object->unknown_276;
                    RemoveGameObject(baddie->object, 1);
                    baddie->object = NULL;
                    continue;
                }
                if (baddie->object == NULL) {
                    continue;
                }
                if (baddie->object->ai->opponent == NULL && baddie->partner != NULL && baddie->partner->object != NULL) {
                    baddie->object->ai->opponent = (APIOBJECT *)baddie->partner->object;
                }
            }
        } else if ((baddie->flags & 0x10) == 0 &&
                   (OnOrInsidePlane(&baddie->position, &PlayPlane[1].point, &PlayPlane[1].normal, NULL, 1.5f, NULL) != 0 ||
                    OnOrInsidePlane(&baddie->position, &PlayPlane[2].point, &PlayPlane[2].normal, NULL, 1.5f, NULL) != 0)) {
        } else {
            if ((baddie->flags & 1) != 0 &&
                OnOrInsidePlane(&baddie->position, &PlayPlane[1].point, &PlayPlane[1].normal, NULL, 0.5f, NULL) == 0 &&
                OnOrInsidePlane(&baddie->position, &PlayPlane[2].point, &PlayPlane[2].normal, NULL, 0.5f, NULL) == 0) {
                spawn_position = baddie->position;
                NuVecRotateY(&spawn_position, &spawn_position, 0x8000);
            } else {
                spawn_position = baddie->position;
            }
            if (baddie->id == -1) {
                JediBResetRecord(baddie);
            }
            baddie->object = AddDynamicCreature(baddie->id, &spawn_position, baddie->angle, "gen_bdroids", &baddie->locator.path_info, NULL,
                                                1, NULL, NULL, 0, 0);
            if (baddie->object == NULL) {
                continue;
            }
            baddie->object->ai_packet.locator = &baddie->locator;
            baddie->object->tag_object_callback = JediBKilledCallback;
            if (baddie->id == id_BOB) {
                if (baddie->unknown_58 == 0) {
                    baddie->unknown_58 = baddie->object->layer_mask;
                    if (baddie->object->flags_ef0 & 0x200) {
                        baddie->flags |= 4;
                    } else {
                        baddie->flags &= ~4u;
                    }
                } else {
                    baddie->object->layer_mask = baddie->unknown_58;
                    if (baddie->flags & 4) {
                        baddie->object->flags_ef0 |= 0x200;
                    } else {
                        baddie->object->flags_ef0 &= ~0x200u;
                    }
                }
            }
        }
        object = baddie->object;
        if (object == NULL) {
            continue;
        }
        JediBActiveCount++;
        if ((baddie->flags & 0x10) == 0) {
            object->excluded_opponents |= jedi_b.exclusion_mask;
        }
    }
}

void JediB_DrawPanel(WORLDINFO *world) {
    i16 ids[8];
    i32 count;
    i32 index;
    if (Mission_Active(MissionSys) != NULL) {
        return;
    }
    {
        u8 spawned[8] = {0};
        if (netclient == 0) {
            if (nethost != 0) {
                jedib_netpacket->state = jedi_b.state;
                jedib_netpacket->phase = jedi_b.phase;
            }
            if (jedi_b.state != 1) {
                return;
            }
            switch (jedi_b.phase) {
                case 1:
                case 2:
                case 3:
                    if (jedi_b.goody_count != 0) {
                        count = jedi_b.goody_count;
                        if (count > 8) {
                            count = 8;
                        }
                        for (index = 0; index < count; index++) {
                            ids[index] = (i16)jedi_b.goodies[index].id;
                            if (jedi_b.goodies[index].object == NULL) {
                                spawned[index] = 1;
                            }
                        }
                    } else {
                        count = 1;
                        if (jedi_b.phase == 1) {
                            ids[0] = dat_007f19c8;
                        } else if (jedi_b.phase == 2) {
                            ids[0] = id_ANAKINPADAWAN;
                        } else if (jedi_b.phase == 3) {
                            ids[0] = dat_007f19d0;
                        }
                    }
                    DrawMeleeTargetsAlpha(ids, spawned, NULL, count);
                    if (nethost != 0) {
                        PCByteCopy(jedib_netpacket->ids, ids, count * 2);
                        PCByteCopy(jedib_netpacket->spawned, spawned, count);
                        jedib_netpacket->count = (i16)count;
                    }
                    break;
                case 4:
                case 5:
                case 6:
                    DrawMeleeTargetsAlpha(jedi_b.wave_ids, jedi_b.wave_spawned, NULL, 6);
                    if (nethost != 0) {
                        PCByteCopy(jedib_netpacket->ids, jedi_b.wave_ids, 12);
                        PCByteCopy(jedib_netpacket->spawned, jedi_b.wave_spawned, 6);
                    }
                    break;
                case 7:
                    if (jedi_b.boss != NULL) {
                        DrawBossHitPoints(jedi_b.boss);
                    }
                    break;
            }
        } else {
            if (jedib_netpacket->state != 1) {
                return;
            }
            switch (jedib_netpacket->phase) {
                case 1:
                case 2:
                case 3:
                    DrawMeleeTargetsAlpha(jedib_netpacket->ids, jedib_netpacket->spawned, NULL, jedib_netpacket->count);
                    break;
                case 4:
                case 5:
                case 6:
                    DrawMeleeTargetsAlpha(jedib_netpacket->ids, jedib_netpacket->spawned, NULL, 6);
                    break;
                case 7:
                    if (jedi_b.boss == NULL) {
                        jedi_b.boss = FindGameObject(id_JANGOFETT, APIOBJECT_SIDE_BADDIE, 1, 0, 0);
                    }
                    if (jedi_b.boss != NULL) {
                        DrawBossHitPoints(jedi_b.boss);
                    }
                    break;
            }
        }
    }
}

CUTINFO *factoryb_cut = NULL;
GIZAIMESSAGE *factoryb_conveyor_stopped_msg = NULL;

void FactoryB_Reset(WORLDINFO *world) {
    ResetPaintPuzzle(world);
    factoryb_cut = NewCutScene(NULL, world->cutscene_sys, "ep2_droidfactory_conveyor", 0);
    if (factoryb_cut != NULL && factoryb_cut->instance != NULL) {
        ((CUT_INSTANCE_READER *)factoryb_cut->instance)->flags_88 |= NUGCUTSCENE_INST_PLAYING;
        ((CUT_INSTANCE_READER *)factoryb_cut->instance)->flags_88 |= NUGCUTSCENE_INST_LOOP;
    }
    factoryb_conveyor_stopped_msg = CheckGizAIMessage(gizaimessagesys, "conveyor_stop", NULL);
}

f32 FactoryBConveyorStopFrame = 28.0f;
static f32 FactoryBConveyorXSpeed = 0.0f;
static f32 FactoryBConveyorZSpeed = 0.0f;
i32 bonus_gunship_store_progress_flag = 0;

void FactoryB_Update(WORLDINFO *world) {
    GIZAIMESSAGE *message;
    UpdatePaintPuzzle(world);
    message = factoryb_conveyor_stopped_msg;
    if (message != NULL && message->value != 0.0f && ((CUTINFO_READER *)factoryb_cut)->previous_frame > 50.0 &&
        ((CUTINFO_READER *)factoryb_cut)->previous_frame < 54.0) {
        ((CUT_INSTANCE_READER *)factoryb_cut->instance)->current_frame = 0.0f;
        ((CUT_INSTANCE_READER *)factoryb_cut->instance)->flags_88 &= ~NUGCUTSCENE_INST_PLAYING;
        world->current_level->conveyor_x_speed = 0.0f;
        world->current_level->conveyor_z_speed = 0.0f;
    } else if (((CUT_INSTANCE_READER *)factoryb_cut->instance)->current_frame > FactoryBConveyorStopFrame) {
        world->current_level->conveyor_x_speed = FactoryBConveyorXSpeed;
        world->current_level->conveyor_z_speed = FactoryBConveyorZSpeed;
        PlaySfx("FacB_BeltLp", NULL);
    } else {
        world->current_level->conveyor_x_speed = 0.0f;
        world->current_level->conveyor_z_speed = 0.0f;
    }
}

void FactoryB_Draw(WORLDINFO *world) {
    DrawPaintLights();
}

static DOOKU_C dooku_c = {0};

void DookuC_Reset(WORLDINFO *world) {
    memset(&dooku_c, 0, sizeof(dooku_c));
    if (netclient == 0) {
        dooku_c.total_message = SetGizAIMessage(gizaimessagesys, "DookuFight", 0.0f, NULL);
        dooku_c.hit_message = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
    }
    NuSpecialFind(world->current_gscn, &dooku_c.node, "dooku_force", 1);
}

void DookuC_Update(WORLDINFO *world) {
    if (netclient == 0) {
        if (FreePlay != 0) {
            KillBossCompleteLevel(id_DOOKU, 0, 0.0f);
        } else if (dat_0087b140 != NULL) {
            KillBossNewLevel(id_DOOKU, 0, 0.0f, dat_0087b140->index);
        }
    }
    DrawForceBackEffect(&dooku_c.node);
}

void DookuC_DrawPanel(WORLDINFO *world) {
    GAMEOBJECT *dooku;
    if (netclient != 0) {
        return;
    }
    dooku = FindGameObject(id_DOOKU, APIOBJECT_SIDE_BADDIE, 1, 1, 0);
    if (dooku != NULL && dooku_c.hit_message != NULL && dooku_c.hit_message->value == 1.0f) {
        DrawBossHitPoints(dooku);
    }
}

void FactoryG_Init(WORLDINFO *world) {
    struct gizmo_s *g;
    if (netclient != 0) {
        return;
    }
    g = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force5");
    if (g != NULL) {
        force_array[0] = g->object;
    }
    g = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force6");
    if (g != NULL) {
        force_array[1] = g->object;
    }
    g = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force7");
    if (g != NULL) {
        force_array[2] = g->object;
    }
    g = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force8");
    if (g != NULL) {
        force_array[3] = g->object;
    }
}

GAMEOBJECT *ObiWan = NULL;
i32 LevForce = 0;
struct gizmo_s *forcetube = 0;

void FactoryG_Update(WORLDINFO *world) {
    i32 complete = 0;
    i32 i;
    if (netclient != 0) {
        return;
    }
    for (i = 0; i < 4; i++) {
        if (GizForce_Complete(force_array[i]) != 0) {
            complete++;
        }
    }
    if (ObiWan != NULL && complete != 4) {
        f32 lamp;
        ObiWan->position.x = 112.76f;
        ObiWan->position.y = 0.75f;
        ObiWan->position.z = -10.5f;
        lamp =
            (f32)(NuTrigTable[(u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 2.0f) * NuQFntHalfScale * VehicleTurnPhaseScale) >> 1] * 30.0);
        ObiWan->unknown_276 = (u16)(0xc000 - (i16)(i32)((f64)lamp * -182.04444885253906));
        return;
    }
    if (ObiWan == NULL) {
        ObiWan = FindGameObject(dat_007f19d0, APIOBJECT_CREATURE, 0, 1, 0);
        return;
    }
    if (FreePlay != 0) {
        CompleteLevel(WORLD);
    } else {
        CUTINFO *cut = NewCutScene(NULL, world->cutscene_sys, "ep2_factory_outro", 1);
        ((CUT_INSTANCE_READER *)((CUTINFO_READER *)cut)->instance)->flags_88 |= NUGCUTSCENE_INST_PLAYING;
    }
}

void KaminoA_AlwaysUpdate(WORLDINFO *world) {
    debris_object_switches[1] = CUTSTOPGAME == 0 && GameCam->sock_position.sock_index != 5 && GameCam->sock_position.sock_index != 6;
}

KAMINODISCO kaminodisco = {0};
KAMINOC_PACKET *kaminoc_netpacket = NULL;
nuhspecial_s disco_on_spin[3] = {0};
GIZMO *disco_off_spina[3] = {0};
nuhspecial_s walllights[2] = {0};
FACTORYB_NETPACKET *factoryb_netpacket = 0;
nuhspecial_s walllights_disco[2] = {0};
nuhspecial_s green_light = {0};
nuhspecial_s striplights[2] = {0};
JEDIB_STATE jedi_b = {0};
void *kaminoe_netpacket = 0;
struct gizforce_s *force_array[4] = {0};
nuhspecial_s discolights[2] = {0};
JEDIB_NETPACKET *jedib_netpacket = NULL;
nuhspecial_s discorm_wall_on = {0};
nuhspecial_s discorm_wall_off = {0};
GIZMO *gizTurrets[2] = {0};
i32 last = 0;

void KaminoC_Init(WORLDINFO *world) {
    char name[32];
    i32 i;
    memset(&kaminodisco, 0, sizeof(kaminodisco));
    kaminoc_netpacket = SetLevelHack(sizeof(KAMINOC_PACKET));
    kaminodisco.tile_count = 0;
    do {
        if (kaminodisco.tile_count < 9) {
            sprintf(name, "dot_off_0%d", kaminodisco.tile_count + 1);
        } else {
            sprintf(name, "dot_off_%d", kaminodisco.tile_count + 1);
        }
        NuSpecialFind(WORLD->current_gscn, &kaminodisco.off[kaminodisco.tile_count], name, 1);
        if (kaminodisco.tile_count < 9) {
            sprintf(name, "dot_flash_0%d", kaminodisco.tile_count + 1);
        } else {
            sprintf(name, "dot_flash_%d", kaminodisco.tile_count + 1);
        }
        NuSpecialFind(WORLD->current_gscn, &kaminodisco.pending[kaminodisco.tile_count], name, 1);
        if (kaminodisco.tile_count < 9) {
            sprintf(name, "dot_select_0%d", kaminodisco.tile_count + 1);
        } else {
            sprintf(name, "dot_select_%d", kaminodisco.tile_count + 1);
        }
        NuSpecialFind(WORLD->current_gscn, &kaminodisco.occupied[kaminodisco.tile_count], name, 1);
        if (kaminodisco.tile_count < 9) {
            sprintf(name, "dot_finish_0%d", kaminodisco.tile_count + 1);
        } else {
            sprintf(name, "dot_finish_%d", kaminodisco.tile_count + 1);
        }
        NuSpecialFind(WORLD->current_gscn, &kaminodisco.complete[kaminodisco.tile_count], name, 1);
        if (kaminodisco.tile_count < 9) {
            sprintf(name, "dot_on_0%d", kaminodisco.tile_count + 1);
        } else {
            sprintf(name, "dot_on_%d", kaminodisco.tile_count + 1);
        }
        NuSpecialFind(WORLD->current_gscn, &kaminodisco.final[kaminodisco.tile_count], name, 1);
        if (NuSpecialExistsFn(&kaminodisco.off[kaminodisco.tile_count]) == 0 ||
            NuSpecialExistsFn(&kaminodisco.pending[kaminodisco.tile_count]) == 0 ||
            NuSpecialExistsFn(&kaminodisco.occupied[kaminodisco.tile_count]) == 0 ||
            NuSpecialExistsFn(&kaminodisco.complete[kaminodisco.tile_count]) == 0) {
            break;
        }
        kaminodisco.tile_count++;
    } while (kaminodisco.tile_count < 16);
    kaminodisco.area = AISysFindArea(WORLD->ai_sys, "DISCO");
    NuSpecialFind(WORLD->current_gscn, &walllights[0], "walllights1", 1);
    NuSpecialFind(WORLD->current_gscn, &walllights[1], "walllights2", 1);
    NuSpecialFind(WORLD->current_gscn, &walllights_disco[0], "walllights1_disco", 1);
    NuSpecialFind(WORLD->current_gscn, &walllights_disco[1], "walllights2_disco", 1);
    NuSpecialFind(WORLD->current_gscn, &striplights[0], "striplights1", 1);
    NuSpecialFind(WORLD->current_gscn, &striplights[1], "striplights1b", 1);
    NuSpecialFind(WORLD->current_gscn, &discolights[0], "discolight1", 1);
    NuSpecialFind(WORLD->current_gscn, &discolights[1], "discolight2", 1);
    NuSpecialFind(WORLD->current_gscn, &discorm_wall_on, "discorm_wall_on", 1);
    NuSpecialFind(WORLD->current_gscn, &discorm_wall_off, "discorm_wall_off", 1);
    for (i = 0; i < 3; i++) {
        sprintf(name, "disco_on_spin%d", i + 3);
        NuSpecialFind(WORLD->current_gscn, &disco_on_spin[i], name, 1);
        if (netclient == 0) {
            sprintf(name, "disco_off%d", i + 1);
            disco_off_spina[i] = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, name);
        }
    }
    if (netclient == 0) {
        gizTurrets[0] = GizmoFindByName(world->gizmo_sys, turret_gizmotype_id, "turret01");
        gizTurrets[1] = GizmoFindByName(world->gizmo_sys, turret_gizmotype_id, "turret02");
        LevGizmo[0] = GizmoFindByName(world->gizmo_sys, gizaimessage_gizmotype_id, "msg_KaminoCProgress");
        LevGizmo[1] = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, "JANGOFIELD01");
    }
    last = 0;
}

void KaminoC_Reset(WORLDINFO *world) {
    i32 i;
    memset(kaminodisco.tile_state, 0, sizeof(kaminodisco.tile_state));
    kaminodisco.phase = 0;
    kaminodisco.tile[0] = -1;
    kaminodisco.tile[1] = -1;
    kaminodisco.timer = 0.0f;
    kaminodisco.completion_sound = 0;
    kaminoc_netpacket->pending_mask = 0;
    kaminoc_netpacket->complete_mask = 0;
    kaminoc_netpacket->occupied_mask = 0;
    kaminoc_netpacket->final_mask = 0;
    kaminoc_netpacket->off_mask = -1;
    kaminoc_netpacket->complete = 0;
    for (i = 0; i < kaminodisco.tile_count; i++) {
        NuSpecialSetVisibility(&kaminodisco.off[i], 1);
        NuSpecialSetVisibility(&kaminodisco.pending[i], 0);
        NuSpecialSetVisibility(&kaminodisco.complete[i], 0);
    }
    kaminodisco.next_tile_message = SetGizAIMessage(gizaimessagesys, "NextDiscoTile", 0.0f, NULL);
    kaminodisco.complete_message = SetGizAIMessage(gizaimessagesys, "DiscoComplete", 0.0f, NULL);
    for (i = 0; i < 3; i++) {
        NuSpecialSetVisibility(&disco_on_spin[i], 0);
        if (netclient == 0) {
            GizmoSetVisibility(world->gizmo_sys, disco_off_spina[i], 1, 0);
            if (((GIZFORCE *)disco_off_spina[i]->object)->anim_set->state == GAMEANIMSET_STATE_AT_START) {
                GizmoActivate(world->gizmo_sys, disco_off_spina[i], 1, 0);
            }
        }
    }
}

static i32 KaminoDiscoChooseTile(i32 state, i32 excluded) {
    i32 candidates[16];
    i32 count = 0;
    i32 i;
    for (i = 0; i < kaminodisco.tile_count; i++) {
        if (i != excluded && kaminodisco.tile_state[i] == state) {
            candidates[count++] = i;
        }
    }
    return count != 0 ? candidates[NuRand(NULL) % count] : -1;
}

static void KaminoDiscoUpdateVisuals(WORLDINFO *world) {
    i32 i;
    for (i = 0; i < kaminodisco.tile_count; i++) {
        i32 bit = 1 << i;
        NuSpecialSetVisibility(&kaminodisco.off[i], kaminoc_netpacket->off_mask & bit);
        NuSpecialSetVisibility(&kaminodisco.pending[i], kaminoc_netpacket->pending_mask & bit);
        NuSpecialSetVisibility(&kaminodisco.occupied[i], kaminoc_netpacket->occupied_mask & bit);
        NuSpecialSetVisibility(&kaminodisco.final[i], kaminoc_netpacket->final_mask & bit);
        NuSpecialSetVisibility(&kaminodisco.complete[i], kaminoc_netpacket->complete_mask & bit);
        if ((kaminoc_netpacket->sound_mask & bit) != 0) {
            PlaySfx("Kam_DiscoFloorPanelOn", &NuSpecialGetDrawMtx(&kaminodisco.complete[i])->translation);
        }
    }
    if (kaminoc_netpacket->complete != 0) {
        for (i = 0; i < 3; i++) {
            NuSpecialSetVisibility(&disco_on_spin[i], 1);
            GizmoSetVisibility(world->gizmo_sys, disco_off_spina[i], 0, 0);
        }
        NuSpecialSetVisibility(&walllights_disco[0], 1);
        NuSpecialSetVisibility(&walllights_disco[1], 1);
        NuSpecialSetVisibility(&walllights[0], 0);
        NuSpecialSetVisibility(&walllights[1], 0);
        NuSpecialSetVisibility(&striplights[0], 0);
        NuSpecialSetVisibility(&striplights[1], 1);
        NuSpecialSetVisibility(&discolights[0], 1);
        NuSpecialSetVisibility(&discolights[1], 1);
        NuSpecialSetVisibility(&discorm_wall_off, 0);
        NuSpecialSetVisibility(&discorm_wall_on, 1);
        if (kaminodisco.completion_sound == 0) {
            PlaySfx("Kam_DiscoFloorPanelDone", NuSpecialGetDrawPos(&kaminodisco.off[i]));
            kaminodisco.completion_sound = 1;
        }
    } else {
        for (i = 0; i < 3; i++) {
            NuSpecialSetVisibility(&disco_on_spin[i], 0);
            GizmoSetVisibility(world->gizmo_sys, disco_off_spina[i], 1, 0);
        }
        NuSpecialSetVisibility(&walllights_disco[0], 0);
        NuSpecialSetVisibility(&walllights_disco[1], 0);
        NuSpecialSetVisibility(&walllights[0], 1);
        NuSpecialSetVisibility(&walllights[1], 1);
        NuSpecialSetVisibility(&striplights[0], 1);
        NuSpecialSetVisibility(&striplights[1], 0);
        NuSpecialSetVisibility(&discolights[0], 0);
        NuSpecialSetVisibility(&discolights[1], 0);
        NuSpecialSetVisibility(&discorm_wall_on, 0);
        NuSpecialSetVisibility(&discorm_wall_off, 1);
    }
}

static f32 KaminoDiscoTileTime = 2.5f;
static f32 KaminoDiscoTileRadius = 0.2f;

void KaminoC_Update(WORLDINFO *world) {
    KAMINO_PROGRESS_GIZMO_READER *progress = LevGizmo[0]->object;
    i32 i;
    if (GizmoGetOutput(world->gizmo_sys, LevGizmo[0], 0, 0) == 0) {
        if ((progress->flags & 1) == 0) {
            GizmoActivate(world->gizmo_sys, LevGizmo[1], 1, 1);
            if (GizmoGetOutput(world->gizmo_sys, gizTurrets[0], 0, 0) != 0 && GizmoGetOutput(world->gizmo_sys, gizTurrets[1], 0, 0) != 0) {
                GizmoSetVisibility(world->gizmo_sys, LevGizmo[1], 0, 1);
            }
        }
    } else {
        GizmoSetVisibility(world->gizmo_sys, LevGizmo[1], 0, 1);
    }
    kaminoc_netpacket->sound_mask = 0;
    SetGizAIMessage(gizaimessagesys, "NextDiscoTile", 0.0f, kaminodisco.next_tile_message);
    SetGizAIMessage(gizaimessagesys, "DiscoComplete", 0.0f, kaminodisco.complete_message);
    switch (kaminodisco.phase) {
        case 0:
            if (KAMINO_DISCO_PLAYER_IN_AREA(world, world->ai_sys->player_1, kaminodisco.area) ||
                KAMINO_DISCO_PLAYER_IN_AREA(world, world->ai_sys->player_2, kaminodisco.area)) {
                kaminoc_netpacket->complete = 0;
                kaminodisco.timer = 0.0f;
                kaminodisco.phase = 1;
                kaminodisco.tile[0] = (i8)KaminoDiscoChooseTile(0, -1);
                kaminodisco.tile[1] = (i8)KaminoDiscoChooseTile(0, kaminodisco.tile[0]);
                if (kaminodisco.tile[0] != -1 && kaminodisco.tile[1] != -1) {
                    kaminodisco.tile_state[kaminodisco.tile[0]] = 1;
                    kaminodisco.tile_state[kaminodisco.tile[1]] = 1;
                }
            }
            break;
        case 1: {
            i32 occupied_by_player = -1;
            i32 occupied[2];
            i32 k;
            for (k = 0; k < 2; k++) {
                i32 old_state = kaminodisco.tile_state[kaminodisco.tile[k]];
                NUVEC *position;
                kaminodisco.tile_state[kaminodisco.tile[k]] = 1;
                occupied[k] = 0;
                position = NuSpecialGetPos(&kaminodisco.pending[kaminodisco.tile[k]]);
                for (i = 0; i < 8; i++) {
                    GAMEOBJECT *object = Player[i];
                    if (object != NULL && (object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) != 0 &&
                        (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && object->unknown_281 != 0) {
                        f32 dx = position->x - object->position.x;
                        f32 dy = position->y - object->position.y;
                        f32 dz = position->z - object->position.z;
                        f32 dist = dx * dx + dy * dy + dz * dz;
                        if (dist < KaminoDiscoTileRadius * KaminoDiscoTileRadius) {
                            occupied[k] = 1;
                            kaminodisco.tile_state[kaminodisco.tile[k]] = 2;
                            if (object == player) {
                                occupied_by_player = kaminodisco.tile[k];
                            }
                            break;
                        }
                    }
                }
                if (old_state != kaminodisco.tile_state[kaminodisco.tile[k]] && kaminodisco.tile_state[kaminodisco.tile[k]] == 2) {
                    kaminoc_netpacket->sound_mask |= 1 << kaminodisco.tile[k];
                }
            }
            if (occupied[0] != 0 && occupied[1] != 0) {
                kaminodisco.timer = 0.0f;
                kaminodisco.tile_state[kaminodisco.tile[0]] = 3;
                kaminodisco.tile_state[kaminodisco.tile[1]] = 3;
                kaminodisco.tile[0] = (i8)KaminoDiscoChooseTile(0, -1);
                kaminodisco.tile[1] = (i8)KaminoDiscoChooseTile(0, kaminodisco.tile[0]);
                if (kaminodisco.tile[0] != -1 && kaminodisco.tile[1] != -1) {
                    kaminodisco.tile_state[kaminodisco.tile[0]] = 1;
                    kaminodisco.tile_state[kaminodisco.tile[1]] = 1;
                } else {
                    kaminodisco.phase = 2;
                    for (i = 0; i < kaminodisco.tile_count; i++) {
                        kaminodisco.tile_state[i] = 0;
                    }
                }
                break;
            }
            kaminodisco.timer += FRAMETIME;
            if (kaminodisco.timer > KaminoDiscoTileTime) {
                kaminodisco.timer = 0.0f;
                for (i = 0; i < 2; i++) {
                    i32 tile = KaminoDiscoChooseTile(3, -1);
                    if (tile != -1) {
                        kaminodisco.tile_state[tile] = 0;
                    }
                }
            }
            if (player2 == NULL) {
                GAMEOBJECT *companion = Player[0];
                if (player == companion) {
                    companion = Player[1];
                }
                if (companion != NULL) {
                    if (occupied_by_player == kaminodisco.tile[0]) {
                        SetGizAIMessage(gizaimessagesys, "NextDiscoTile", (f32)(kaminodisco.tile[1] + 1), kaminodisco.next_tile_message);
                    } else if (occupied_by_player == kaminodisco.tile[1]) {
                        SetGizAIMessage(gizaimessagesys, "NextDiscoTile", (f32)(kaminodisco.tile[0] + 1), kaminodisco.next_tile_message);
                    }
                }
            }
            break;
        }
        case 2:
            SetGizAIMessage(gizaimessagesys, "DiscoComplete", 1.0f, kaminodisco.complete_message);
            kaminoc_netpacket->complete = 1;
            if (!(KAMINO_DISCO_PLAYER_IN_AREA(world, world->ai_sys->player_1, kaminodisco.area) ||
                  KAMINO_DISCO_PLAYER_IN_AREA(world, world->ai_sys->player_2, kaminodisco.area))) {
                KaminoC_Reset(world);
                return;
            }
            for (i = 0; i < kaminodisco.tile_count; i++) {
                kaminodisco.tile_state[i] = 4;
            }
            kaminodisco.timer += FRAMETIME;
            if (kaminodisco.timer > 20.0f) {
                KaminoC_Reset(world);
                return;
            }
            break;
        default:
            break;
    }
    kaminoc_netpacket->final_mask = 0;
    kaminoc_netpacket->off_mask = 0;
    kaminoc_netpacket->pending_mask = 0;
    kaminoc_netpacket->occupied_mask = 0;
    kaminoc_netpacket->complete_mask = 0;
    for (i = 0; i < kaminodisco.tile_count; i++) {
        switch (kaminodisco.tile_state[i]) {
            case 0:
                kaminoc_netpacket->off_mask |= 1 << i;
                break;
            case 1:
                kaminoc_netpacket->pending_mask |= 1 << i;
                break;
            case 2:
                kaminoc_netpacket->occupied_mask |= 1 << i;
                break;
            case 3:
                kaminoc_netpacket->final_mask |= 1 << i;
                break;
            case 4:
                kaminoc_netpacket->complete_mask |= 1 << i;
                break;
            default:
                break;
        }
    }
    KaminoDiscoUpdateVisuals(world);
}

void KaminoE_Init(WORLDINFO *world) {
    void *g;
    kaminoe_netpacket = SetLevelHack(0x14);
    g = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "endblock_b08");
    if (g != NULL) {
        LevForce = *(i32 *)g;
    }
}

KAMINO_E kamino_e = {0};

void KaminoE_Reset(WORLDINFO *world) {
    char name[16];
    i32 i;
    memset(&kamino_e, 0, sizeof(kamino_e));
    kamino_e.landing_area = AISysFindArea(WORLD->ai_sys, "landing_pad");
    if (NuSpecialFind(world->current_gscn, &kamino_e.slave1, "slave1", 1) != 0) {
        kamino_e.platform = (i16)FindPlatInst(NuSpecialGetInstanceix(&kamino_e.slave1));
    }
    for (i = 0; i < 4; i++) {
        GIZMO *gizmo;
        sprintf(name, "turret%d", i + 1);
        gizmo = GizmoFindByName(world->gizmo_sys, turret_gizmotype_id, name);
        if (gizmo != NULL) {
            kamino_e.turrets[i] = gizmo->object;
            kamino_e.turrets[i]->opponent = &kamino_e.position;
            kamino_e.turrets[i]->opponent_type = 2;
            kamino_e.turrets[i]->flags |= 1;
        } else {
            KaminoEGizmoMissing(name);
        }
        sprintf(name, "R4_t%d", i + 1);
        gizmo = GizmoFindByName(world->gizmo_sys, gizpanel_gizmotype_id, name);
        if (gizmo != NULL) {
            kamino_e.panels[i] = gizmo->object;
        } else {
            KaminoEGizmoMissing(name);
        }
    }
    kamino_e.jango_fight = SetGizAIMessage(gizaimessagesys, "JangoFight", 0.0f, NULL);
    kamino_e.can_fire = SetGizAIMessage(gizaimessagesys, "Slave1CanFire", 0.0f, NULL);
    kamino_e.reset_turrets = SetGizAIMessage(gizaimessagesys, "ResetTurrets", 0.0f, NULL);
    kamino_e.show_hearts = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
    kamino_e.minicut_started = CheckGizAIMessage(gizaimessagesys, "MiniCutStarted", NULL);
    CutScene_Find(world->cutscene_sys, "Ep2_Kamino_Intro2");
    kamino_e.intro_cutscene = CutScene_Find(world->cutscene_sys, "Ep2_Kamino_Intro2");
}

static KAMINOEPLATE *kamino_plate_1 = 0;
static i32 kamino_plate_1_active = 0;
static KAMINOEPLATE *kamino_plate_2 = 0;
static i32 kamino_plate_2_active = 0;
static KAMINOEPLATE *kamino_plate_3 = 0;
static i32 kamino_plate_3_active = 0;
static KAMINOEPLATE *kamino_plate_4 = 0;
static i32 kamino_plate_4_active = 0;
static KAMINOEPLATE *kamino_plate_hit = 0;
static u16 kamino_plate_id = 0;

i32 KaminoE_CheckPlatHit(BOLT *bolt) {
    if (bolt->unknown_fa == kamino_plate_id) {
        if (kamino_plate_1 != NULL && kamino_plate_1_active != 0) {
            if ((kamino_plate_1->flags_13a & 0x30) == 0 && (kamino_plate_1->flags_13a & 0x2) != 0) {
                if (kamino_plate_hit == NULL) {
                    kamino_plate_hit = kamino_plate_1;
                }
            }
        }
        if (kamino_plate_2 != NULL && kamino_plate_2_active != 0) {
            if ((kamino_plate_2->flags_13a & 0x30) == 0 && (kamino_plate_2->flags_13a & 0x2) != 0) {
                if (kamino_plate_hit == NULL) {
                    kamino_plate_hit = kamino_plate_2;
                }
            }
        }
        if (kamino_plate_3 != NULL && kamino_plate_3_active != 0) {
            if ((kamino_plate_3->flags_13a & 0x30) == 0 && (kamino_plate_3->flags_13a & 0x2) != 0) {
                if (kamino_plate_hit == NULL) {
                    kamino_plate_hit = kamino_plate_3;
                }
            }
        }
        if (kamino_plate_4 != NULL && kamino_plate_4_active != 0) {
            if ((kamino_plate_4->flags_13a & 0x30) == 0 && (kamino_plate_4->flags_13a & 0x2) != 0) {
                if (kamino_plate_hit == NULL) {
                    kamino_plate_hit = kamino_plate_4;
                }
            }
        }
        return 1;
    }
    return 0;
}

static void KaminoEResetTurrets(void) {
    i32 i;
    kamino_e.active_turret = NULL;
    for (i = 0; i < 4; i++) {
        GizmoActivate(WORLD->gizmo_sys, GizmoFindByData(WORLD->gizmo_sys, gizpanel_gizmotype_id, kamino_e.panels[i]), 1, 1);
        kamino_e.turrets[i]->state = 1;
        kamino_e.turrets[i]->flags &= ~TURRET_FLAG_AIM_AT_FIXED_ANGLES;
    }
}

NUVEC KaminoShipCentre = {56.5f, -2.89f, 8.0f};
f32 KaminoShipRadius = 6.5f;
i32 KaminoShipOrbitPitch = -3640;
i32 KaminoShipOrbitYaw = 0x4000;
NUVEC KaminoShipGunOffset[2] = {{0.2f, -1.1f, 0.9f}, {-0.2f, -1.1f, 0.9f}};
f32 KaminoShipFireInterval = 0.15f;
f32 KaminoShipSeekRate = 1.0f;
f32 KaminoShipAimScale = 30.0f;
f32 KaminoShipAimLift = 0.5f;
i32 KaminoShipTargetPitch = 0xb60;
i32 KaminoShipTargetYaw = -0x4000;
f32 KaminoShipYawRate = 4.0f;
f32 KaminoShipPitchRate = 4.0f;
f32 KaminoShipLiftTime = 4.0f;
i32 KaminoShipStartPitch = -0x4000;
f32 KaminoShipStartHeight = -2.8f;
f32 KaminoShipZAmplitude = 0.2f;
f32 KaminoShipZPeriod = 4.0f;
f32 KaminoShipYAmplitude = 0.2f;
f32 KaminoShipYPeriod = 3.0f;
KAMINO_SHIP_ZERO_ANGLES KaminoShipZeroAngles = {0};

void KaminoE_UpdateSlave1(WORLDINFO *world) {
    f32 best_distance = 1000000000.0f;
    GAMEOBJECT *nearest = NULL;
    NUMTX *matrix;
    NUVEC desired;
    f64 degrees;
    f32 phase;
    f32 wave;
    i32 angle;
    if (kamino_e.active_turret != NULL && (kamino_e.active_turret->flags & TURRET_FLAG_AIM_AT_FIXED_ANGLES) != 0) {
        kamino_e.active_turret = NULL;
    }
    matrix = NuSpecialGetDrawMtx(&kamino_e.slave1);
    desired.x = 0.0f;
    desired.y = 0.0f;
    desired.z = KaminoShipRadius;
    NuVecRotateX(&desired, &desired, kamino_e.orbit_pitch);
    NuVecRotateY(&desired, &desired, kamino_e.orbit_yaw);
    NuVecAdd(&desired, &desired, &KaminoShipCentre);
    kamino_e.elapsed_time += FRAMETIME;
    degrees = (f64)kamino_e.elapsed_time * 360.0;
    phase = degrees / KaminoShipYPeriod;
    angle = (i32)((f64)phase * 182.04444885253906);
    wave = (f32)(((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5);
    desired.y = wave * KaminoShipYAmplitude + desired.y;
    phase = degrees / KaminoShipZPeriod;
    angle = (i32)((f64)phase * 182.04444885253906);
    wave = (f32)(((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5);
    desired.z = wave * KaminoShipZAmplitude + desired.z;
    if (kamino_e.minicut_started != NULL && kamino_e.minicut_started->value > 0.0f) {
        NuSpecialSetVisibility(&kamino_e.slave1, 1);
    }
    if (kamino_e.departing != 0) {
        PlaySfx("Slave1_EngineLp", &kamino_e.position);
    }
    switch ((i8)kamino_e.departing) {
        case 0:
            if (kamino_e.jango_fight->value > 0.0f) {
                kamino_e.departure_timer = 0.0f;
                kamino_e.departing = 1;
                GizObstacle_FindByName(world->giz_obstacle_sys, "slave1_debris");
            }
            kamino_e.position = KaminoShipCentre;
            kamino_e.position.y = KaminoShipStartHeight;
            kamino_e.pitch = KaminoShipStartPitch;
            kamino_e.yaw = KaminoShipZeroAngles.yaw;
            kamino_e.orbit_pitch = KaminoShipOrbitPitch;
            kamino_e.orbit_yaw = KaminoShipOrbitYaw;
            break;
        case 1: {
            f32 ratio;
            kamino_e.departure_timer += FRAMETIME;
            if (kamino_e.departure_timer < KaminoShipLiftTime) {
                phase = kamino_e.departure_timer / KaminoShipLiftTime;
                phase = (f32)((f64)phase * 180.0 - 90.0);
                angle = (i32)((f64)phase * 182.04444885253906);
                ratio = (f32)(((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5);
            } else {
                kamino_e.departing = 2;
                ratio = 1.0f;
                kamino_e.departure_timer = 0.0f;
            }
            kamino_e.position.y = ratio * desired.y + (1.0f - ratio) * KaminoShipStartHeight;
            break;
        }
        case 2:
            kamino_e.departure_timer += FRAMETIME;
            if (kamino_e.departure_timer < KaminoShipPitchRate) {
                kamino_e.pitch = SeekRot((u16)kamino_e.pitch, KaminoShipZeroAngles.pitch, kamino_e.departure_timer);
            } else {
                kamino_e.pitch = SeekRot((u16)kamino_e.pitch, KaminoShipZeroAngles.pitch, KaminoShipPitchRate);
                kamino_e.departure_timer = 0.0f;
                kamino_e.departing = 3;
            }
            break;
        case 3:
            kamino_e.departure_timer += FRAMETIME;
            if (kamino_e.departure_timer < KaminoShipYawRate) {
                kamino_e.yaw = SeekRot((u16)kamino_e.yaw, (u16)KaminoShipTargetYaw, kamino_e.departure_timer);
            } else {
                kamino_e.yaw = SeekRot((u16)kamino_e.yaw, (u16)KaminoShipTargetYaw, KaminoShipYawRate);
                kamino_e.departure_timer = 0.0f;
                kamino_e.departing = 4;
            }
            kamino_e.pitch = SeekRot((u16)kamino_e.pitch, KaminoShipZeroAngles.pitch, KaminoShipPitchRate);
            break;
        case 4: {
            f32 ratio;
            kamino_e.departure_timer += FRAMETIME;
            if (kamino_e.departure_timer < KaminoShipLiftTime) {
                phase = kamino_e.departure_timer / KaminoShipLiftTime;
                phase = (f32)((f64)phase * 180.0 - 90.0);
                angle = (i32)((f64)phase * 182.04444885253906);
                ratio = (f32)(((f64)NuTrigTable[(angle >> 1) & 0x7fff] + 1.0) * 0.5);
            } else {
                kamino_e.departing = 5;
                ratio = 1.0f;
                kamino_e.departure_timer = 0.0f;
            }
            kamino_e.position.x = desired.x * ratio + KaminoShipCentre.x * (1.0f - ratio);
            kamino_e.position.z = desired.z * ratio + KaminoShipCentre.z * (1.0f - ratio);
            kamino_e.pitch = SeekRot((u16)kamino_e.pitch, (u16)KaminoShipTargetPitch, KaminoShipPitchRate);
            kamino_e.yaw = SeekRot((u16)kamino_e.yaw, (u16)KaminoShipTargetYaw, KaminoShipYawRate);
            break;
        }
        case 5: {
            NUVEC aim;
            NUVEC delta;
            i32 have_target = 0;
            i32 target_pitch;
            i32 target_yaw;
            f32 distance;
            if (kamino_e.active_turret != NULL) {
                aim = *NuSpecialGetDrawPos(&kamino_e.active_turret->primary_anim_obj->special);
                aim.y = KaminoShipAimLift + aim.y;
                have_target = 1;
            } else if (kamino_e.can_fire->value == 1.0f) {
                i32 i;
                for (i = 0; i < 8; ++i) {
                    GAMEOBJECT *object = Player[i];
                    u32 flags;
                    if (object == NULL) {
                        continue;
                    }
                    flags = object->flags_1fc_word;
                    if ((flags & APIOBJECT_FLAG_IN_USE) == 0 || (flags & APIOBJECT_FLAG_CHARACTER) == 0 ||
                        (object->character_data->model_flags & CHARACTER_MODEL_UNTARGETABLE) != 0) {
                        continue;
                    }
                    distance = NuVecDistSqr(&object->collision_position, (NUVEC *)&matrix->m30, &delta);
                    if (distance < best_distance) {
                        nearest = Player[i];
                        best_distance = distance;
                    }
                }
                if (nearest != NULL) {
                    aim.x = nearest->collision_position.x;
                    aim.y = (f32)((f64)nearest->floor_height + (f64)0.1f);
                    aim.z = nearest->collision_position.z;
                    have_target = 1;
                }
            }
            kamino_e.orbit_pitch = KaminoShipOrbitPitch;
            if (have_target != 0) {
                NUVEC offset;
                f32 z = aim.z - KaminoShipCentre.z;
                f32 horizontal;
                if (z > 2.5f) {
                    z = 2.5f;
                } else if (z < -2.5f) {
                    z = -2.5f;
                }
                phase = (f32)((f64)z / 2.5);
                phase = phase * KaminoShipAimScale;
                kamino_e.orbit_yaw = KaminoShipOrbitYaw - (i32)(phase * -182.04445f);
                offset.x = 0.0f;
                offset.y = -KaminoShipGunOffset[0].y;
                offset.z = -KaminoShipGunOffset[0].z;
                NuVecMtxRotate(&offset, &offset, matrix);
                NuVecAdd(&offset, &offset, &aim);
                NuVecSub(&delta, &offset, &kamino_e.position);
                target_yaw = (i32)((f64)NuAtan2(delta.x, delta.z) * 10430.3779296875);
                horizontal = NuFsqrt(delta.z * delta.z + delta.x * delta.x);
                target_pitch = (i32)((f64)NuAtan2(-delta.y, horizontal) * 10430.3779296875);
                kamino_e.departure_timer += FRAMETIME;
                if (kamino_e.departure_timer > KaminoShipFireInterval) {
                    NUMTX bolt_matrix;
                    NUVEC origin;
                    i32 i;
                    kamino_e.departure_timer = 0.0f;
                    for (i = 0; i < 2; ++i) {
                        BOLT *bolt;
                        i32 adjustment;
                        bolt_matrix = *matrix;
                        NuVecMtxTransform(&origin, &KaminoShipGunOffset[kamino_e.fire_index], matrix);
                        if (i == 0) {
                            distance = NuVecDist(&origin, &aim, NULL);
                        }
                        adjustment = (i32)((f64)NuAtan2(-KaminoShipGunOffset[kamino_e.fire_index].x, distance) * 10430.3779296875);
                        NuMtxPreRotateY(&bolt_matrix, adjustment);
                        BoltNoSfx = 1;
                        bolt = Bolt_Add(NULL, &origin, &bolt_matrix, 0x27, 0x800);
                        if (bolt != NULL) {
                            bolt->flags |= BOLT_SHOOTS_OBSTRUCTIONS;
                        }
                        kamino_e.fire_index = !kamino_e.fire_index;
                    }
                    PlaySfx("Kam_Slave1BlasterFire", &origin);
                }
            } else {
                kamino_e.orbit_yaw = KaminoShipOrbitYaw;
                target_yaw = KaminoShipTargetYaw;
                target_pitch = KaminoShipTargetPitch;
            }
            SeekVec(&kamino_e.position, &kamino_e.position, &desired, KaminoShipSeekRate);
            kamino_e.pitch = SeekRot((u16)kamino_e.pitch, (u16)target_pitch, KaminoShipSeekRate);
            kamino_e.yaw = SeekRot((u16)kamino_e.yaw, (u16)target_yaw, KaminoShipSeekRate);
            break;
        }
    }
    NuMtxSetTranslation(matrix, &kamino_e.position);
    NuMtxPreRotateY(matrix, kamino_e.yaw);
    NuMtxPreRotateX(matrix, kamino_e.pitch);
}

void KaminoE_AlwaysUpdate(WORLDINFO *world) {
    debris_object_switches[1] = CUTSTOPGAME != 0 || GameCam->sock_position.sock_index == 0x1e;
}

void KaminoE_Update(WORLDINFO *world) {
    GAMEOBJECT *jango;
    GAMEOBJECT *other;
    GAMEOBJECT *target;
    i32 direction;
    u32 *connection;
    AIAREA *fight_area;
    AILOCATOR *wait;
    AISYS *ai;
    if (kamino_e.reset_turrets != NULL && kamino_e.reset_turrets->value == 1.0f) {
        KaminoEResetTurrets();
        kamino_e.reset_turrets->value = 0.0f;
    }
    if (netclient != 0) {
        return;
    }
    jango = FindGameObject(id_JANGOFETT, APIOBJECT_SIDE_BADDIE, 1, 1, 0);
    other = FindGameObject(id_JANGOFETT, APIOBJECT_SIDE_NEUTRAL, 1, 1, 0);
    target = NULL;
    if (jango != NULL) {
        target = jango;
    } else if (other != NULL) {
        target = other;
    }
    direction = 0;
    if (jango != NULL && jango->current_hp < 1) {
        if (FreePlay != 0) {
            KillBossCompleteLevel(id_JANGOFETT, 0, 0.0f);
        } else {
            KillBossNewLevel(id_JANGOFETT, 0, 0.0f, dat_0087b138->index);
        }
    }
    if (kamino_e.jango_fight->value == 0.0f && FreePlay == 0 && kamino_e.intro_cutscene != NULL &&
        instNuGCutSceneIsFinished(((CUTINFO_READER *)kamino_e.intro_cutscene)->instance) != 0) {
        kamino_e.jango_fight->value = 1.0f;
    }
    KaminoE_UpdateSlave1(world);
    connection = AIPAthFindPathCnx(world->ai_sys, world->ai_sys->path_sys->active_path, "Bridge1_a", "Bridge1_b", &direction);
    fight_area = AISysFindArea(WORLD->ai_sys, "Fight");
    if (target == NULL || connection == NULL || fight_area == NULL) {
        return;
    }
    if ((connection[direction] & 0x80000000) == 0 && (connection[direction == 0] & 0x80000000) == 0) {
        return;
    }
    ai = world->ai_sys;
    if ((*(u64 *)target->ai_area_mask & (u64)(i64)(1 << (fight_area - ai->areas))) != 0) {
        return;
    }
    wait = AIPathFindLocator(ai, "WAIT");
    if (wait == NULL) {
        return;
    }
    target->position = wait->position;
    target->unknown_276 = wait->rotation;
    target->facing_angle = wait->rotation;
    target->movement_facing_angle = wait->rotation;
    target->ai_packet.path_info = wait->path_info;
    target->collision_position = target->position;
    target->initial_position = target->position;
    plr_lastpos = target->collision_position;
    target->start_position = target->collision_position;
    target->respawn_position = target->position;
    target->last_safe_position = target->position;
    target->ai_update_position = target->position;
    target->velocity = v000;
    InitSurfaceInfo(target);
}

/* Reads the Kamino_E state block. */
void KaminoE_Draw(WORLDINFO *world) {
    if (netclient == 0 && kamino_e.show_hearts->value > 0.0f) {
        GAMEOBJECT *jango = FindGameObject(id_JANGOFETT, APIOBJECT_SIDE_BADDIE, 1, 1, 0);
        if (jango != NULL && kamino_e.show_hearts != NULL && kamino_e.show_hearts->value == 1.0f) {
            DrawBossHitPoints(jango);
        }
    }
    NuSpecialSetDrawMtx(&kamino_e.slave1, NuSpecialGetDrawMtx(&kamino_e.slave1));
    NuSpecialSetVisibility(&kamino_e.slave1, 1);
}

i32 KaminoInside(void) {
    if (WORLD->area != NULL && WORLD->area == KAMINO_ADATA) {
        if (WORLD->current_level == dat_0087b12c) {
            if (CUTSTOPGAME != 0 || GameCam->sock_position.sock_index == 5 || GameCam->sock_position.sock_index == 6) {
                return 1;
            }
        } else if (WORLD->current_level == KAMINOE_LDATA) {
            if (CUTSTOPGAME == 0 && GameCam->sock_position.sock_index != 0x1e) {
                return 1;
            }
        }
    }
    return 0;
}

i32 fun_004e2370(void) {
    if (WORLD->current_level == KAMINOC_LDATA && GameCam->sock == 0x15) {
        return 1;
    }
    return 0;
}

i32 KaminoDiscoOn(void) {
    return KaminoDiscoMusicState.mode == 2;
}

void KaminoOutro_Init(WORLDINFO *world) {
    debris_object_switches[1] = CUTSTOPGAME == 0 && GameCam->sock_position.sock_index != 5 && GameCam->sock_position.sock_index != 6;
}

static const char GunshipAGreenBolt[16] = "trooper_green";
static const char GunshipAPowerA11[12] = "power_a11";
static const char GunshipAPowerB11[12] = "power_b11";
static const char GunshipAPowerA21[12] = "power_a21";
static const char GunshipAPowerB21[12] = "power_b21";

void GunshipA_Init(WORLDINFO *world) {
    gunship_bolts[1] = BoltType_FindIDByName(GunshipAGreenBolt, world);
    gunship_bolts[0] = BoltType_FindIDByName("trooper_red", world);
    trooper_side[0] = 0;
    trooper_side[1] = 0;
    trooper_side[2] = 0;
    trooper_side[3] = 0;
    trooper_side[4] = 0;
    trooper_side[5] = 1;
    trooper_side[6] = 1;
    trooper_side[7] = 1;
    trooper_side[8] = 1;
    trooper_side[9] = 1;
    InitMiniSnowTroopers(world, 10, 32, 0);
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, (char *)GunshipAPowerA11);
    LevGizmo[1] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, (char *)GunshipAPowerB11);
    LevGizmo[2] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, (char *)GunshipAPowerA21);
    LevGizmo[3] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, (char *)GunshipAPowerB21);
}

void GunshipA_Update(WORLDINFO *world) {
    UpdateMiniSnowTroopers(world);
}

void GunshipA_Draw(WORLDINFO *world) {
    DrawMiniSnowTroopers(world);
}

void GunshipB_Reset(WORLDINFO *world) {
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gun_a11");
    LevGizmo[1] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gun_a21");
    LevGizmo[2] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gun_b11");
    LevGizmo[3] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gun_b21");
    LevGizmo[4] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gun_c11");
    LevGizmo[5] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gun_c21");
    LevGizmo[6] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gun_d11");
    LevGizmo[7] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "gun_d21");
}

f32 gunshipb_seekrange = 4.0f;
f32 gunshipb_seekmom = 5.0f;
f32 gunshipb_seekmomseek = 5.0f;
i32 GUNSHIPAHACK = 2;

void GunShip_DragBombSeekBlowUp(GAMEOBJECT *object) {
    GIZMOBLOWUP *nearest = NULL;
    f32 nearest_distance;
    NUVEC offset;
    NUVEC nearest_offset;
    NUVEC direction;
    f32 distance;
    f32 factor;
    i32 index;
    if (object->player.character_context != CHARACTER_CONTEXT_BEEN_DRAGGED) {
        return;
    }
    nearest_distance = gunshipb_seekrange * gunshipb_seekrange;
    for (index = 0; index < 8; ++index) {
        GIZMOBLOWUP *blowup;
        if (LevGizmo[index] == NULL || LevGizmo[index]->object == NULL) {
            continue;
        }
        blowup = LevGizmo[index]->object;
        if ((blowup->status_flags & BLOWUP_STATUS_ACTIVE) == 0 || (blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) != 0) {
            continue;
        }
        distance = NuVecDistSqr(&blowup->mid_position, &object->collision_position, &offset);
        if (distance < nearest_distance) {
            nearest_distance = distance;
            nearest = blowup;
            nearest_offset = offset;
        }
    }
    if (nearest == NULL) {
        return;
    }
    NuVecNorm(&direction, &nearest_offset);
    nearest_distance = NuFsqrt(nearest_distance);
    factor = 1.0f - nearest_distance / gunshipb_seekrange;
    direction.x *= gunshipb_seekmom * factor;
    direction.z *= gunshipb_seekmom * factor;
    object->velocity.x = SeekValF(object->velocity.x, direction.x, gunshipb_seekmomseek);
    object->velocity.z = SeekValF(object->velocity.z, direction.z, gunshipb_seekmomseek);
}

i32 GunshipPlayerDead = 0;

void BonusGunshipA_Reset(WORLDINFO *world) {
    GunshipPlayerDead = 0;
    BONUS_GUNSHIP_LEVFLAGS->secondary = 0;
    bonus_gunship_store_progress_flag = 0;
    if (BONUS_GUNSHIP_LEVFLAGS->progress == 1) {
        BONUS_GUNSHIP_LEVFLAGS->progress = 2;
    }
}

void BonusGunshipA_Update(WORLDINFO *world) {
    GAMEOBJECT *first = Player[0];
    GAMEOBJECT *second = Player[1];
    if (BONUS_GUNSHIP_LEVFLAGS->progress == 0) {
        if ((first != NULL && first->sock_position.sock_index == 0 && first->sock_position.distance > 12.1f) ||
            (second != NULL && second->sock_position.sock_index == 0 && second->sock_position.distance > 12.1f)) {
            if (netclient == 0) {
                Doors_SetLastDoor(Door_FindByName(world, "gunshipa_mid"));
                bonus_gunship_store_progress_flag = 1;
                StoreLevelProgress(world);
                second = Player[1];
                first = Player[0];
                bonus_gunship_store_progress_flag = 0;
            }
            BONUS_GUNSHIP_LEVFLAGS->progress = 1;
        }
    }
    if (GunshipPlayerDead == 0 && ((first != NULL && first->dead != 0) || (second != NULL && second->dead != 0))) {
        GunshipPlayerDead = 1;
        ResetLevel(world, "ep2_bonus_gunshipcavalry_explode", 1);
    }
}

BONUS_GUNSHIP_B_NETPACKET *BonusGunshipBNetPacket = NULL;

void BonusGunshipB_Init(WORLDINFO *world) {
    BonusGunshipBNetPacket = SetLevelHack(12);
    LevGizObst[0] = GizObstacle_FindByName(world->giz_obstacle_sys, "obstacle9");
}

void BonusGunshipB_Reset(WORLDINFO *world) {
    MiscTime = 0.0f;
    BONUS_GUNSHIP_LEVFLAGS->progress = 0;
    BONUS_GUNSHIP_LEVFLAGS->secondary = 0;
    GunshipPlayerDead = 0;
    bonus_gunship_store_progress_flag = 0;
}

void BonusGunshipB_Update(WORLDINFO *world) {
    if (netclient == 0) {
        if (GunshipPlayerDead == 0 && ((Player[0] != NULL && Player[0]->dead != 0) || (Player[1] != NULL && Player[1]->dead != 0))) {
            GunshipPlayerDead = 1;
            ResetLevel(world, "ep2_bonus_gunshipcavalry_explode", 1);
        }
        if (BONUS_GUNSHIP_LEVFLAGS->progress == 0) {
            f32 delay = 15.0f;
            if (LevDeaths >= 3) {
                delay = (f32)((f64)LevDeaths + 15.0);
                if (delay > 30.0f) {
                    delay = 30.0f;
                }
            }
            if (GameTimer.time_elapsed >= delay) {
                BONUS_GUNSHIP_LEVFLAGS->progress = 1;
                if (LevGizObst[0] != NULL) {
                    LevGizObst[0]->runtime_flags |= GIZOBSTACLE_LOCKED_OPEN;
                }
                MiscTime = 45.0f;
                TimerScale = 1.0f;
                TimerAlpha = 0.0f;
            }
        } else if (BONUS_GUNSHIP_LEVFLAGS->progress == 1) {
            f32 previous_time;
            NuFmod(MiscTime, 5.0f);
            previous_time = MiscTime;
            MiscTime -= FRAMETIME;
            if (MiscTime <= 0.0f) {
                if (Player[0] != NULL && (Player[0]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                    LoseCoins(Player[0], 1);
                }
                if (Player[1] != NULL && (Player[1]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                    LoseCoins(Player[1], 1);
                }
                if (GunshipPlayerDead == 0) {
                    GunshipPlayerDead = 1;
                    ResetLevel(world, "ep2_bonus_gunshipcavalry_explode", 1);
                }
                BONUS_GUNSHIP_LEVFLAGS->progress = 2;
            } else if (MiscTime > 0.0f && (i32)MiscTime != (i32)previous_time) {
                TimerScale = 1.25f;
                TickTockSfx();
            }
        }
        BonusGunshipBNetPacket->progress = BONUS_GUNSHIP_LEVFLAGS->progress;
        BonusGunshipBNetPacket->secondary = BONUS_GUNSHIP_LEVFLAGS->secondary;
        BonusGunshipBNetPacket->time = MiscTime;
    } else {
        BONUS_GUNSHIP_LEVFLAGS->progress = BonusGunshipBNetPacket->progress;
        BONUS_GUNSHIP_LEVFLAGS->secondary = BonusGunshipBNetPacket->secondary;
        MiscTime = BonusGunshipBNetPacket->time;
    }
}

void BonusGunshipB_Panel(WORLDINFO *world) {
    if (BONUS_GUNSHIP_LEVFLAGS->progress == 1) {
        if (MiscTime > 0.0f) {
            DrawTimer((i32)MiscTime + 1, 0, 0);
        }
    }
}

i16 dat_007f19c4 = -1;
i16 dat_007f19c8 = -1;
i16 id_ANAKINPADAWAN = -1;
i16 dat_007f19d0 = -1;
i16 dat_007f19d4 = -1;
i16 dat_007f19d8 = -1;
i16 id_BOB = -1;
i16 id_JANGOFETT = -1;
char *paint_name[6] = {"red_juice", "yellow_juice", "blue_juice", "orange_juice", "green_juice", "purple_juice"};
char *paint_panel_name[3] = {"redp", "yellp", "bluep"};
char *paint_target_colour[3] = {"but_orange", "but_green", "but_purple"};
char *paint_light_name[3] = {"off_purple", "off_green", "off_orange"};
JEDIB_PHASE jedi_b_phase1[8] = {{&id_DROIDEKA, "phase_droids"},
                                {&id_DROIDEKA, "phase_droids"},
                                {&id_BATTLEDROIDSECURITY, "phase_droids"},
                                {&id_BATTLEDROIDSECURITY, "phase_droids"},
                                {&id_BATTLEDROIDSECURITY, "phase_droids"},
                                {&id_BATTLEDROIDSECURITY, "phase_droids"},
                                {&id_BATTLEDROIDSECURITY, "phase_droids"},
                                {NULL, NULL}};
JEDIB_PHASE jedi_b_phase2[8] = {{&id_DROIDEKA, "phase_droids"},         {&id_DROIDEKA, "phase_droids"},
                                {&id_SUPERBATTLEDROID, "phase_droids"}, {&id_SUPERBATTLEDROID, "phase_droids"},
                                {&id_SUPERBATTLEDROID, "phase_droids"}, {&id_SUPERBATTLEDROID, "phase_droids"},
                                {&id_SUPERBATTLEDROID, "phase_droids"}, {NULL, NULL}};
JEDIB_PHASE jedi_b_phase3[8] = {{&id_SUPERBATTLEDROID, "phase_droids"}, {&id_SUPERBATTLEDROID, "phase_droids"},
                                {&id_DROIDEKA, "phase_droids"},         {&id_DROIDEKA, "phase_droids"},
                                {&id_DROIDEKA, "phase_droids"},         {&id_DROIDEKA, "phase_droids"},
                                {&id_DROIDEKA, "phase_droids"},         {NULL, NULL}};

void InitPaintPuzzle(WORLDINFO *world) {
    i32 i;
    factoryb_netpacket->cylinder_up = 0;
    for (i = 0; i < 6; i++) {
        paintobst[i] = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, paint_name[i]);
    }
    for (i = 0; i < 3; i++) {
        paintpanel[i] = GizmoFindByName(world->gizmo_sys, gizpanel_gizmotype_id, paint_panel_name[i]);
    }
    for (i = 0; i < 3; i++) {
        NuSpecialFind(world->current_gscn, &painttargetcolour[i], paint_target_colour[i], 0);
    }
    for (i = 0; i < 3; i++) {
        NuSpecialFind(world->current_gscn, &paintlights[i], paint_light_name[i], 0);
    }
    forcetube = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, "FORCE_TUBE");
    NuSpecialFind(world->current_gscn, &green_light, "green_light_on1", 1);
    ResetPaintPuzzle(world);
}

void FactoryB_Init(WORLDINFO *world) {
    factoryb_netpacket = SetLevelHack(4);
    InitPaintPuzzle(world);
    if (FactoryBConveyorXSpeed == 0.0f && FactoryBConveyorZSpeed == 0.0f) {
        FactoryBConveyorXSpeed = world->current_level->conveyor_x_speed;
        FactoryBConveyorZSpeed = world->current_level->conveyor_z_speed;
    }
    world->current_level->conveyor_x_speed = 0.0f;
    world->current_level->conveyor_z_speed = 0.0f;
}
