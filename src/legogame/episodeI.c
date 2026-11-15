#include <stdio.h>
#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aiscript_types.h"
#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/ai/aisys/gameaiscript.h"
#include "gameapi/edptl.h"
#include "globals.h"
#include "legoapi/ai/core/gameai.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/chris.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/cutscenes/cutscenes.h"
#include "legoapi/gizmo/base/gizmessage.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolt.h"
#include "legoapi/items/collect/shard.h"
#include "legoapi/items/objects/cable.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/camera.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/core/gamemessage.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/gamestructure.h"
#include "legoapi/misc/buffers.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/render/core/render.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/edsplines.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episodeI.h"
#include "legoapi/world/levels/episodeV.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nugcutscene.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include <math.h>

extern f32 sebulba_goal_ahead_vals[4];
extern f32 sebulba_fallback_lerpf;
extern f32 sebulba_fallback_dist;
extern f32 sebulba_catchup_lerpf;
extern f32 sebulba_catchup_speed_mul;
extern f32 sebulba_catchup_dist;
extern f32 sebulba_fallback_speed_mul;

typedef struct racepod_resource_s {
    NUMTX matrix;
    NUVEC4 previous_axis;
    NUVEC4 previous_position;
    NUVEC4 previous_displacement;
    i32 pitch;
    i32 yaw;
    i32 unknown_0078;
    f32 speed;
    FLIGHT_SPLINE_RESOURCE *spline;
    f32 start;
    i16 model_id;
    i16 unknown_008a;
    f32 distance;
    GAMEOBJECT *object;
    void *next;
    u8 unknown_0098[8];
} RACEPOD_RESOURCE;

typedef struct podrace_resource_s {
    FLIGHT_SPLINE_RESOURCE splines[32];
    RACEPOD_RESOURCE lap_entries[16];
    f32 lap_countdown;
    f32 mushroom_timer;
    f32 lap_display;
    f32 previous_lap_display;
    f32 max_lap_time;
    f32 lap_time_increment;
    i32 attempts_per_increment;
    i32 attempts;
    u8 flags;
    u8 unknown_b021[15];
} PODRACE_RESOURCE;

extern nuhspecial_s SebulbaMineSpecial;
extern f32 SebulbaDistanceBias;
extern f32 SebulbaThrowInterval;
static f32 PodSebulbaThrowSpeed;
static f32 PodSebulbaThrowGravity;

/* Episode I level callbacks installed by FixUpLevels. */

typedef struct retakeg_netpacket_s {
    i16 guard_a;
    i16 guard_b;
} RETAKEG_NETPACKET;

void ChrisAllocLevelStuff(WORLDINFO *world);
void FlightSpline_Init(WORLDINFO *world, FLIGHT_SPLINE_RESOURCE *splines, i32 capacity);
/* PodRace_A mine spawner and renderer. */
i32 QuickNewRayCast(NUVEC *position, NUVEC *movement, f32 radius, i32 scan_flags, f32 max_distance, f32 step);
void PodLoseSpeed(GAMEOBJECT *object, i32 hit, i32 rumble);

typedef struct podrace_netpacket_s {
    f32 start_countdown;
    f32 remaining_time;
    f32 available_time;
    f32 lap_alpha;
    i32 lap;
} PODRACE_NETPACKET;

i32 NuSpecialClipTestExtents(nuhspecial_s *special, NUMTX *matrix);

/* Pod sprint (Episode I bonus) level callbacks. */
typedef struct PODSPRINT_VIEW {   // size 0x94
    NUGSPLINE *finish_line;       // 0x00
    NUGSPLINE *halfway;           // 0x04
    PODSPRINT_AI ai[6];           // 0x08
    GIZAIMESSAGE *lap_msg;        // 0x68
    GIZAIMESSAGE *max_speed_msg;  // 0x6c
    GIZAIMESSAGE *min_speed_msg;  // 0x70
    GIZAIMESSAGE *speed_step_msg; // 0x74
    GAMEOBJECT *pacemaker;        // 0x78
    AIAREA *boulders;             // 0x7c
    f32 arrow_timer;              // 0x80
    f32 countdown;                // 0x84
    f32 lap_time;                 // 0x88
    u8 unknown_8c[2];             // 0x8c
    i8 ai_state;                  // 0x8e
    i8 ai_index;                  // 0x8f
    u8 unknown_90[2];             // 0x90

    union { // size 0x2
        u16 flags;

        struct {                     // size 0x2
            i16 halfway : 1;         // 0x92:0
            i16 unknown_1 : 1;       // 0x92:1
            i16 finished : 2;        // 0x92:2
            i16 reset_requested : 1; // 0x92:4
            i16 unknown_5 : 11;      // 0x92:5
        } bits;
    }; // 0x92
} PODSPRINT_VIEW;

#define PODSPRINT_STATE ((PODSPRINT_VIEW *)&podsprint)
#define PODSPRINT_NET ((PODSPRINT_NETPACKET *)podsprint_netpacket)
/* Last player positions used for the finish line tests. */
static NUVEC pod_old_pos[2];
extern f32 HudBossIconY;
i32 XZLinesIntersect(NUVEC *a, NUVEC *b, NUVEC *c, NUVEC *d, f32 *first, f32 *second);

/* The engine's VuVec helpers; this unit carries its own copies. */
static void EpisodeIVuVecSetOrigin(NUVEC4 *v) {
    v->x = 0.0f;
    v->y = 0.0f;
    v->z = 0.0f;
    v->w = 1.0f;
}

static f32 EpisodeIVuVecDot(const NUVEC4 *a, const NUVEC4 *b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

static f32 EpisodeIVuVecLengthSq(const NUVEC4 *v) {
    return v->x * v->x + v->y * v->y + v->z * v->z;
}

static void PodMatrixCopyPrivate(NUMTX *destination, const NUMTX *source) {
    destination->m00 = source->m00;
    destination->m01 = source->m01;
    destination->m02 = source->m02;
    destination->m03 = source->m03;
    destination->m10 = source->m10;
    destination->m11 = source->m11;
    destination->m12 = source->m12;
    destination->m13 = source->m13;
    destination->m20 = source->m20;
    destination->m21 = source->m21;
    destination->m22 = source->m22;
    destination->m23 = source->m23;
    destination->m30 = source->m30;
    destination->m31 = source->m31;
    destination->m32 = source->m32;
    destination->m33 = source->m33;
}

GUNGAN_A gungan_a = {0};

void GunganA_Init(WORLDINFO *world) {
    char name[32];
    i32 i;
    if (netclient == 0) {
        memset(&gungan_a, 0, sizeof(gungan_a));
        gungan_a.models[0] = dat_007f19ac;
        gungan_a.models[1] = dat_007f19b0;
        gungan_a.models[2] = dat_007f19b4;
        gungan_a.models[3] = dat_007f19b0;
        for (i = 0; i < 32; i++) {
            sprintf(name, "origin_%d", i);
            gungan_a.origins[i] = AIPathFindLocator(world->ai_sys, name);
            sprintf(name, "target_%d", i);
            gungan_a.targets[i] = AIPathFindLocator(world->ai_sys, name);
            if (gungan_a.origins[i] == NULL || gungan_a.targets[i] == NULL) {
                break;
            }
            gungan_a.count++;
        }
    }
}

i16 id_BATTLEDROID = -1;
i16 dat_007f19ac = -1;
i16 dat_007f19b0 = -1;
i16 dat_007f19b4 = -1;
i16 id_ROYALGUARD = -1;
i16 id_ANAKINSPOD = -1;
f32 sockstep = 10.0f;
f32 gungan_a_time = 0.5f;
f32 PodRace_sniper_fire_time = 0.25f;
f32 PodRace_sniper_fire_range_time = 2.0f;
f32 PodRace_sniper_fire_radius = 2.5f;
f32 PodRace_sniper_start_fire_radius = 90.0f;

void GunganA_Update(WORLDINFO *world) {
    GAMEOBJECT *object;
    i32 r;
    if (netclient != 0) {
        return;
    }
    gungan_a.spawn_timer += FRAMETIME;
    if (gungan_a.count != 0 && gungan_a.spawn_timer > gungan_a_time) {
        r = NuRand(NULL) % gungan_a.count;
        if (active_neutral_count < 4) {
            AILOCATOR *origin;
            i32 model;
            gungan_a.model_index = (i16)((gungan_a.model_index + 1) % 4);
            model = gungan_a.models[gungan_a.model_index];
            origin = gungan_a.origins[r];
            object =
                AddDynamicCreature(model, &origin->position, origin->rotation, "Wildlife", &origin->path_info, NULL, 0, NULL, NULL, 0, 0);
        } else {
            i32 pick = NuRand(NULL);
            if ((pick & 1) != 0) {
                AILOCATOR *origin = gungan_a.origins[r];
                object =
                    AddDynamicCreature(id_STAP, &origin->position, origin->rotation, "STAP", &origin->path_info, NULL, 0, NULL, NULL, 0, 0);
            } else {
                AILOCATOR *origin = gungan_a.origins[r];
                object = AddDynamicCreature(id_BATTLEDROID, &origin->position, origin->rotation, "Battledroid", &origin->path_info, NULL, 0,
                                            NULL, NULL, 0, 0);
            }
        }
        if (object != NULL) {
            object->ai_packet.locator = gungan_a.targets[r];
        }
        gungan_a.spawn_timer = 0.0f;
    }
}

void PodRaceSnipersReset(void) {
    PodRace_nsnipers = 0;
    if (Lap != 0) {
        char name[16];
        NUGSPLINE *spline;
        i32 point;
        sprintf(name, "Sniper%d", Lap);
        spline = NuSplineFind(WORLD->current_gscn, name);
        if (spline != NULL) {
            for (point = 0; point < spline->length && PodRace_nsnipers < 10; point += 2) {
                PodRace_snipers[PodRace_nsnipers].prev = spline->pts[point];
                PodRace_snipers[PodRace_nsnipers].pos = spline->pts[point + 1];
                PodRace_snipers[PodRace_nsnipers].fire_timer =
                    NuRandFloat() * PodRace_sniper_fire_time + (f64)PodRace_sniper_fire_time * 0.5;
                PodRace_snipers[PodRace_nsnipers].state = 0.0f;
                ++PodRace_nsnipers;
            }
        }
    }
}

void PodRaceSnipersUpdate(void) {
    i32 i;
    NUVEC direction;
    static i32 player_ix = 0;
    i32 bolttype = (WORLD->current_level == PODSPRINTA_LDATA) ? 0x29 : 0x28;
    BOLTTYPE *type = BoltType_FindByID(bolttype, WORLD);
    for (i = 0; i < PodRace_nsnipers; i++) {
        i32 j;
        i32 in_range = 0;
        for (j = 0; j < 2; ++j) {
            f32 dz;
            f32 dx;
            f32 dist2;
            GAMEOBJECT *target = Player[j];
            if (!(target != NULL && (target->flags_1fc_word & APIOBJECT_FLAG_IN_USE) != 0 &&
                  (target->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0)) {
                continue;
            }
            dx = target->collision_position.x - PodRace_snipers[i].pos.x;
            dz = target->collision_position.z - PodRace_snipers[i].pos.z;
            dist2 = dx * dx + dz * dz;
            if (dist2 < PodRace_sniper_start_fire_radius * PodRace_sniper_start_fire_radius) {
                in_range = 1;
                if (dist2 < PodRace_sniper_fire_radius * PodRace_sniper_fire_radius) {
                    PodRace_snipers[i].state = PodRace_sniper_fire_range_time;
                }
            }
        }
        PodRace_snipers[i].fire_timer += FRAMETIME;
        if (in_range) {
            if (PodRace_snipers[i].fire_timer >= PodRace_sniper_fire_time) {
                NUVEC aim;
                f32 distance;
                NUMTX mtx;
                PodRace_snipers[i].fire_timer = 0.0f;
                if (PodRace_snipers[i].state > 0.0f) {
                    GAMEOBJECT *target = player2;
                    if (target != NULL) {
                        player_ix = player_ix == 0;
                    } else {
                        player_ix = 0;
                    }
                    if (player_ix == 0) {
                        target = player;
                    }
                    CalculateInterceptVector(&PodRace_snipers[i].prev, &target->collision_position, &target->velocity, type->unknown_10,
                                             &aim, NULL);
                    NuVecAdd(&aim, &aim, &PodRace_snipers[i].prev);
                    PodRace_snipers[i].state -= FRAMETIME;
                    if (PodRace_snipers[i].state < 0.0f) {
                        PodRace_snipers[i].state = 0.0f;
                    }
                } else {
                    aim.x = 0.0f;
                    aim.y = 0.0f;
                    aim.z = NuRandFloat() * PodRace_sniper_fire_radius;
                    NuVecRotateY(&aim, &aim, NuRandInt());
                    NuVecAdd(&aim, &aim, &PodRace_snipers[i].pos);
                }

                distance = NuVecDist(&aim, &PodRace_snipers[i].prev, &direction);
                temp_yrot = NuAtan2D(direction.x, direction.z);
                temp_zrot = NuAtan2D(-direction.y, distance);
                // Uses an aligned matrix local in its realigned stack frame.
                NuMtxSetRotationX(&mtx, temp_zrot);
                NuMtxRotateY(&mtx, temp_yrot);
                Bolt_Add(NULL, &PodRace_snipers[i].prev, &mtx, bolttype, 0);
            }
        }
    }
}

MINESYS minesys = {0};
i32 podrace_section = 0;
i32 PodRace_nsnipers = 0;
PODSPRINT podsprint = {0};
CLIENT_MINES client_mines = {0};

void fun_004d8970(NUVEC **positions, u32 **present_words, u32 **exploded_words) {
    *positions = client_mines.positions;
    *present_words = client_mines.present_words;
    *exploded_words = client_mines.exploded_words;
}

static void InitPodRaceMines(void) {
    char name[32];
    i32 i;
    memset(&minesys, 0, sizeof(minesys));
    memset(&client_mines, 0, sizeof(client_mines));
    if (NuSpecialFind(vehicle_scene, &minesys.mine_special, "mine", 1) != 0) {
        minesys.mine_radius = NuSpecialGetOriginRadius(&minesys.mine_special);
        for (i = 0; i < 10; i++) {
            sprintf(name, "nomine_%d", i);
            minesys.nomine_areas[minesys.nomine_count] = AISysFindArea(WORLD->ai_sys, name);
            minesys.nomine_count++;
        }
        minesys.mine_debris = (i16)FindGameDebris(WORLD->game_debris_sys, "MINE_POP");
        minesys.mine_part = PARTLookupType("POD_MINE_PART");
    }
}

i32 pod_mines_bitfield[2] = {0};
i32 clients_mines_bitfield[2] = {0};
GAMEOBJECT *PodPacemaker = 0;
i32 mine_count = 0;

static void PodRaceResetMines(void) {
    clients_mines_bitfield[0] = 0;
    clients_mines_bitfield[1] = 0;
    pod_mines_bitfield[0] = 0;
    pod_mines_bitfield[1] = 0;
    memset(minesys.mines, 0, sizeof(minesys.mines));
    memset(&client_mines, 0, sizeof(client_mines));
    minesys.spawn_timer = AIPositionUnset;
    mine_count = 0;
    minesys.update_timer = 0.0f;
}

static f32 PodMineAreaScale = 1.0f;
static f32 PodMineHeightScale = 0.5f;
static u32 mine_max_tile = 0x2000;

static void *CreatePodRaceMine(NUVEC *pos) {
    NUVEC mine_pos;
    NUVEC ray_pos;
    NUVEC delta;
    MINE_ENTRY *entry;
    f32 distance;
    f32 y;
    i32 i;
    i32 bit;
    y = GameShadow(NULL, pos, 5.0f, -1);
    if (y == 2000000.0) {
        y = pos->y;
    }
    mine_pos.x = pos->x;
    mine_pos.y = PodMineHeightScale * minesys.mine_radius + y;
    mine_pos.z = pos->z;
    for (i = 0; i < minesys.nomine_count; i++) {
        if (CheckPosAIArea(minesys.nomine_areas[i], &mine_pos, PodMineAreaScale * minesys.mine_radius) != 0) {
            return NULL;
        }
    }
    distance = NuVecXZDist(&mine_pos, &player->collision_position, &delta);
    if (distance > 0.0f) {
        NuVecScale(&delta, &delta, 1.0f / distance);
        ray_pos = player->collision_position;
        ray_pos.y = (f32)(ray_pos.y + 100.0);
        if (QuickNewRayCast(&ray_pos, &delta, 0.0f, 0, PodMineAreaScale * minesys.mine_radius + distance, 5.0f) != 0) {
            return NULL;
        }
    }
    entry = minesys.mines;
    for (i = 0; i < 64; i++, entry++) {
        if (entry->active == 0) {
            entry->active = 1;
            entry->pos = mine_pos;
            entry->grow_alpha = 0.0f;
            if (mine_max_tile != 0) {
                entry->rotx = (u16)(NuRandInt() % mine_max_tile);
            }
            entry->roty = (u16)NuRandInt();
            bit = 1 << i;
            *(u64 *)pod_mines_bitfield |= (u64)(i64)bit;
            *(u64 *)client_mines.present_words &= (u64)(i64)~bit;
            mine_count++;
            return entry;
        }
    }
    return NULL;
}

static void PodRaceExplodeMine(MINE_ENTRY *entry, GAMEOBJECT *object) {
    u64 mask;
    if (minesys.mine_debris != -1) {
        AddGameDebris(WORLD->game_debris_sys, minesys.mine_debris, &entry->pos);
    }
    if (minesys.mine_part != -1) {
        AddFiniteShotPART(minesys.mine_part, &entry->pos, 1);
    }
    GameCam_HitJudder();
    GameCam_NewShake(NULL, 0.75f, 1.0f, 1.0f);
    PlaySfx("Explode1", &object->collision_position);
    PodLoseSpeed(object, 1, 1);
    mask = (u64)(i64) ~(1 << (entry - minesys.mines));
    *(u64 *)pod_mines_bitfield &= mask;
    memset(entry, 0, sizeof(*entry));
    mine_count--;
}

static void PodRaceExplodeClientMine(NUVEC *position, GAMEOBJECT *object) {
    if (minesys.mine_debris != -1) {
        AddGameDebris(WORLD->game_debris_sys, minesys.mine_debris, position);
    }
    if (minesys.mine_part != -1) {
        AddFiniteShotPART(minesys.mine_part, position, 1);
    }
    GameCam_HitJudder();
    GameCam_NewShake(NULL, 0.75f, 1.0f, 1.0f);
    PlaySfx("Explode1", &object->collision_position);
}

static f32 PodMineGrowTime = 1.0f;

void UpdatePodRaceMines(void) {
    GAMEOBJECT *objects[64];
    NUVEC delta;
    MINE_ENTRY *entry;
    GAMEOBJECT *object;
    i32 count;
    i32 i;
    i32 j;
    f32 radius;
    f32 reach;
    f32 distance;
    count = 0;
    object = Obj;
    for (i = 0; i < 64; i++, object++) {
        if (object != NULL && (object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) != 0 &&
            (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && object != PodPacemaker) {
            objects[count++] = object;
        }
    }
    if (netclient == 0) {
        entry = minesys.mines;
        for (i = 0; i < 64; i++, entry++) {
            if (entry->active == 0) {
                continue;
            }
            NuVecSub(&delta, &entry->pos, &GameCam->position);
            if (GameCam->dir.x * delta.x + GameCam->dir.y * delta.y + GameCam->dir.z * delta.z < 0.0) {
                *(u64 *)pod_mines_bitfield &= (u64)(i64) ~(1 << (entry - minesys.mines));
                memset(entry, 0, sizeof(*entry));
                mine_count--;
                continue;
            }
            entry->grow_alpha = (entry->grow += FRAMETIME) * PodMineAreaScale / PodMineGrowTime;
            if (entry->grow_alpha > PodMineAreaScale) {
                entry->grow_alpha = PodMineAreaScale;
            }
            radius = entry->grow_alpha * minesys.mine_radius;
            for (j = 0; j < count; j++) {
                object = objects[j];
                if (object == NULL) {
                    continue;
                }
                delta.x = object->collision_position.x - entry->pos.x;
                delta.z = object->collision_position.z - entry->pos.z;
                reach = object->bounds_radius + radius;
                distance = delta.x * delta.x + delta.z * delta.z;
                if (distance < reach * reach) {
                    if (object->player_index == -1) {
                        KillPlayer(object, 2, 1, NULL);
                        objects[j] = NULL;
                    } else {
                        PodRaceExplodeMine(entry, object);
                        if (object->dead != 0) {
                            objects[j] = NULL;
                        }
                    }
                }
            }
        }
    } else {
        for (i = 0; i < 64; i++) {
            if (((u64)(i64)(1 << i) & *(u64 *)client_mines.present_words) == 0 ||
                ((u64)(i64)(1 << i) & *(u64 *)client_mines.exploded_words) != 0) {
                continue;
            }
            radius = minesys.mine_radius * PodMineAreaScale;
            for (j = 0; j < count; j++) {
                object = objects[j];
                if (object == NULL) {
                    continue;
                }
                delta.x = object->collision_position.x - client_mines.positions[i].x;
                delta.z = object->collision_position.z - client_mines.positions[i].z;
                reach = object->bounds_radius + radius;
                distance = delta.x * delta.x + delta.z * delta.z;
                if (distance < reach * reach) {
                    PodRaceExplodeClientMine(&client_mines.positions[i], object);
                    *(u64 *)client_mines.exploded_words |= (u64)(i64)(1 << i);
                }
            }
        }
    }
}

void PodRaceADraw(WORLDINFO *world) {
    NUMTX mtx;
    NUVEC scale;
    i32 i;
    if (netclient == 0) {
        if (NuSpecialExistsFn(&minesys.mine_special) != 0) {
            MINE_ENTRY *entry = minesys.mines;
            for (i = 0; i < 64; i++, entry++) {
                if (entry->active != 0) {
                    scale.x = scale.y = scale.z = entry->grow_alpha;
                    NuMtxSetScale(&mtx, &scale);
                    NuMtxTranslate(&mtx, &entry->pos);
                    NuMtxPreRotateX(&mtx, entry->rotx);
                    NuMtxPreRotateY(&mtx, entry->roty);
                    NuSpecialDrawAt(&minesys.mine_special, &mtx);
                }
            }
        }
    } else {
        for (i = 0; i < 64; i++) {
            if ((*(u64 *)client_mines.present_words & (u64)(i64)(1 << i)) != 0) {
                NuMtxSetIdentity(&mtx);
                NuMtxTranslate(&mtx, &client_mines.positions[i]);
                NuSpecialDrawAt(&minesys.mine_special, &mtx);
            }
        }
    }
}

static f32 PodMineSockAlong[4] = {0.0f, 1.5f, 0.75f, 1.5f};
static f32 PodMineSpread = 30.0f;
static f32 PodMineAhead = 100.0f;
static f32 PodMineMaxSock = 100.0f;

void PodRaceAUpdate(void) {
    NUVEC vec;
    if (Lap < 4 && MiniCutCam == 0) {
        minesys.update_timer += FRAMETIME;
        if (minesys.update_timer > 1.0f) {
            if (minesys.spawn_timer == 1000000000.0) {
                minesys.spawn_timer = GameCam->sock_position.distance;
            }
            if (GameCam->sock_position.distance < PodMineMaxSock && GameCam->sock_position.distance >= minesys.spawn_timer) {
                vec.x = (f32)((0.5 - NuRandFloat()) * (PodMineSpread * 2.0f));
                vec.y = 0.0f;
                vec.z = PodMineAhead;
                NuVecRotateY(&vec, &vec, player->sock_heading);
                NuVecAdd(&vec, &vec, &player->collision_position);
                if (CreatePodRaceMine(&vec) != NULL) {
                    minesys.spawn_timer = PodMineSockAlong[Lap] + GameCam->sock_position.distance;
                }
            }
        }
    }
}

/* Private owner of the active PodRace block. */
static PODRACE_RESOURCE *ActivePodRace = NULL;
i32 Lap = 0;

static RACEPOD_RESOURCE *RacePodAllocatePrivate(void) {
    PODRACE_RESOURCE *race = ActivePodRace;
    i32 slot;
    for (slot = 0; slot < 16; ++slot) {
        if (race->lap_entries[slot].next == NULL) {
            return &race->lap_entries[slot];
        }
    }
    return NULL;
}

static void RacePodAlignPrivate(RACEPOD_RESOURCE *pod, NUVEC4 *direction, f32 blend, i32 alternate) {
    NUVEC4 axis;
    __declspec(align(16)) NUVEC4 local;
    i16 yaw;
    i16 pitch;
    i16 direction_yaw;
    i16 direction_pitch;
    f32 roll;
    i16 angle;
    NuVecInvMtxRotate((NUVEC *)&local, (NUVEC *)direction, &pod->matrix);
    yaw = NuAtan2D(local.x, local.z);
    pod->yaw = yaw;
    NuVecRotateY((NUVEC *)&local, (NUVEC *)&local, -yaw);
    pitch = NuAtan2D(local.y, local.z);
    pod->pitch = pitch;
    NuMtxPreRotateY(&pod->matrix, yaw);
    NuMtxPreRotateX(&pod->matrix, -pitch);
    if (alternate != 0) {
        NuMtxPreRotateZ(&pod->matrix, 0x200);
        return;
    }
    direction_yaw = NuAtan2D(direction->x, direction->z);
    NuVecRotateY((NUVEC *)&axis, (NUVEC *)direction, -direction_yaw);
    direction_pitch = NuAtan2D(axis.y, axis.z);
    axis.x = 1.0f;
    axis.y = 0.0f;
    axis.z = 0.0f;
    axis.w = 0.0f;
    NuVecRotateZ((NUVEC *)&axis, (NUVEC *)&axis, (i16)(i32)pod->previous_position.w + 0x4000);
    NuVecRotateX((NUVEC *)&axis, (NUVEC *)&axis, -direction_pitch);
    NuVecRotateY((NUVEC *)&axis, (NUVEC *)&axis, direction_yaw);
    NuVecInvMtxRotate((NUVEC *)&axis, (NUVEC *)&axis, &pod->matrix);
    angle = NuAtan2D(axis.x, axis.y);
    if (angle > 24000.0 * FRAMETIME) {
        angle = (i16)(24000.0 * FRAMETIME);
    }
    if (angle < FRAMETIME * -24000.0) {
        angle = (i16)(FRAMETIME * -24000.0);
    }
    roll = angle;
    if (blend > 2.0f) {
        roll = 0.0f;
    } else if (blend > 1.0f) {
        roll = (f32)((2.0 - blend) * roll);
    }
    NuMtxPreRotateZ(&pod->matrix, -(i16)roll);
}

static void PodRaceMineKill(PART *part, i32 reason) {
    if (minesys.mine_debris != -1) {
        AddGameDebris(WORLD->game_debris_sys, minesys.mine_debris, &part->position);
    }
    if (minesys.mine_part != -1) {
        AddFiniteShotPART(minesys.mine_part, &part->position, 1);
    }
}

static f32 PodSebulbaThrowTime = 2.0f;
static f32 PodSebulbaThrowJitter = 1.0f;
static f32 PodSebulbaThrowSpeed = 20.0f;
static f32 PodSebulbaThrowGravity = -10.0f;
static f32 PodSebulbaFarThrow = 10.0f;
static f32 PodSebulbaThrowSpread = 3.0f;
static f32 PodSebulbaThrowAhead = 3.0f;
f32 SebulbaThrowInterval = 1.0f;
f32 SebulbaDistanceBias = 25.0f;

static i32 PodRaceUpdatePod(RACEPOD_RESOURCE *pod, WORLDINFO *world) {
    __declspec(align(16)) NUVEC4 direction;
    NUVEC origin;
    NUVEC velocity;
    NUVEC difference;
    NUVEC target;
    NUVEC target_velocity;
    NUMTX matrix;
    ADDPART part;
    GAMEOBJECT *object;
    f32 length_sq;
    f32 dot;
    f32 length;
    f32 ratio;
    f32 speed;
    f32 distance_sq;
    NUVEC4 *point;
    if (pod->spline == NULL) {
        return 0;
    }
    object = pod->object;
    if (object == NULL || (object->flags_1fc & APIOBJECT_FLAG_IN_USE) == 0 || object->dead != 0) {
        memset(pod, 0, sizeof(*pod));
        return 0;
    }
    if (!(ActivePodRace->lap_countdown > 0.0f)) {
        point = &pod->previous_position;
        direction.x = point->x - pod->matrix.m30;
        direction.y = point->y - pod->matrix.m31;
        direction.z = point->z - pod->matrix.m32;
        length_sq = EpisodeIVuVecLengthSq(&direction);
        dot = EpisodeIVuVecDot(&direction, (NUVEC4 *)&pod->matrix.m20);
        if (!(pod->start >= 1.0f)) {
            while (length_sq < 1.0f || dot < 0.0f) {
                pod->start = pod->start + PodRaceSplineIncrement;
                if (pod->start > 1.0f) {
                    pod->start = 1.0f;
                    break;
                }
                CalcSplinePoint(pod->spline, point, pod->start);
                direction.x = point->x - pod->matrix.m30;
                direction.y = point->y - pod->matrix.m31;
                direction.z = point->z - pod->matrix.m32;
                length_sq = direction.x * direction.x + direction.y * direction.y + direction.z * direction.z;
                dot = direction.y * pod->matrix.m21 + direction.x * pod->matrix.m20 + direction.z * pod->matrix.m22;
            }
        }
        length = NuFsqrt(length_sq);
        if (pod->start >= 1.0f) {
            direction.x = pod->previous_axis.x * FRAMETIME;
            direction.y = pod->previous_axis.y * FRAMETIME;
            direction.z = pod->previous_axis.z * FRAMETIME;
            pod->matrix.m30 += direction.x;
            pod->matrix.m31 += direction.y;
            pod->matrix.m32 += direction.z;
            RacePodAlignPrivate(pod, &direction, (f32)(length / 5.0), 0);
            pod->previous_displacement = direction;
        } else if (length != 0.0f) {
            ratio = ActivePodRace->mushroom_timer / ActivePodRace->previous_lap_display;
            speed = pod->speed;
            if (ratio > 1.0f) {
                ratio = 1.0f;
            }
            if (WORLD->current_level == PODRACEB_LDATA && Lap == 1) {
                speed = (f32)(speed / (f64)0.995f);
                if (ratio < 0.01f) {
                    speed = (f32)(speed * ratio / (f64)0.01f);
                } else {
                    speed = (f32)(speed * 1.5 - (ratio - (f64)0.01f) * speed);
                }
            } else {
                speed = (f32)(speed * 1.5 - ratio * speed);
            }
            speed = speed / length;
            pod->previous_axis.x = direction.x * speed;
            pod->previous_axis.y = direction.y * speed;
            pod->previous_axis.z = direction.z * speed;
            direction.x = pod->previous_axis.x * FRAMETIME;
            direction.y = pod->previous_axis.y * FRAMETIME;
            direction.z = pod->previous_axis.z * FRAMETIME;
            pod->matrix.m30 = direction.x + pod->matrix.m30;
            pod->matrix.m31 += direction.y;
            pod->matrix.m32 += direction.z;
            RacePodAlignPrivate(pod, &direction, (f32)(length / 5.0), 0);
            pod->previous_displacement = direction;
        }
        if (world->current_level == PODRACEA_LDATA && pod->object != NULL && pod->model_id == id_SEBULBASPOD &&
            NuSpecialExistsFn(&minesys.mine_special) != 0) {
            pod->object->special_move_timer -= FRAMETIME;
            if (pod->object->special_move_timer < 0.0f) {
                object = pod->object;
                origin.x = object->position.x;
                origin.y = (f32)(object->position.y + 1.0);
                origin.z = object->position.z;
                if (player != NULL) {
                    GAMEOBJECT *heading_player = player;
                    distance_sq = NuVecDistSqr(&player->collision_position, &object->collision_position, &difference);
                    if (distance_sq > 4.0 * PodSebulbaThrowAhead * PodSebulbaThrowAhead) {
                        target.x = PodSebulbaThrowSpread - (PodSebulbaThrowSpread + PodSebulbaThrowSpread) * NuRandFloat();
                        target.y = 0.0f;
                        target.z = PodSebulbaThrowAhead;
                        NuVecRotateY(&target, &target, heading_player->sock_heading);
                        NuVecAdd(&target, &target, &player->collision_position);
                        target_velocity.x = 0.0f;
                        target_velocity.y = 0.0f;
                        target_velocity.z = CurrentSpeed;
                        NuVecRotateY(&target_velocity, &target_velocity, heading_player->sock_heading);
                        MakeThrowVector(&velocity, &origin, &target, &target_velocity, PodSebulbaThrowSpeed, PodSebulbaThrowGravity);
                        if (distance_sq > PodSebulbaFarThrow * PodSebulbaFarThrow) {
                            velocity.y = 0.0f;
                            NuVecNorm(&velocity, &velocity);
                            NuVecScale(&velocity, &velocity, PodSebulbaFarThrow);
                            NuVecAdd(&target, &origin, &velocity);
                            target_velocity.x = 0.0f;
                            target_velocity.y = 0.0f;
                            target_velocity.z = 0.0f;
                            MakeThrowVector(&velocity, &origin, &target, &target_velocity, PodSebulbaThrowSpeed, PodSebulbaThrowGravity);
                        }
                    }
                    NuMtxSetTranslation(&matrix, &origin);
                    part = Default_ADDPART;
                    part.matrix = &matrix;
                    part.velocity = &velocity;
                    part.unknown_14 = NuSpecialGetOriginRadius(&minesys.mine_special);
                    part.unknown_18 = part.unknown_14;
                    part.special = &minesys.mine_special;
                    part.flags = PART_FLAG_COLLIDE_WITH_CHARACTERS | PART_FLAG_DAMAGE_CHARACTERS | PART_FLAG_KEEP_OFF_SCREEN |
                                 PART_FLAG_ROTATE_RANDOMLY | PART_FLAG_SLOT_CANNOT_BE_STOLEN;
                    part.gravity = PodSebulbaThrowGravity;
                    part.collision_fn = PartCollide_3D;
                    part.kill_fn = PodRaceMineKill;
                    part.time_step = FRAMETIME;
                    part.unknown_a4 = 5.0f;
                    AddPart(&part);
                    {
                        const f64 random = NuRandFloat();
                        pod->object->special_move_timer =
                            (PodSebulbaThrowJitter + PodSebulbaThrowJitter) * random + (PodSebulbaThrowTime - PodSebulbaThrowJitter);
                    }
                }
            }
        }
    }
    pod->matrix.m33 = 1.0f;
    if (pod->object != NULL) {
        pod->object->vehicle_orientation = pod->matrix;
        pod->object->position = pod->object->vehicle_orientation.translation;
        pod->object->ai_update_position = pod->object->position;
    }
    return 1;
}

void ResetPodStuff(void) {
    f32 duration;
    i32 i;
    i32 id = apicharsys->playermodelids[id_ANAKINSPOD];
    if (id != -1 && apicharsys->models[id].model_data_b[1] != NULL) {
        duration = *(f32 *)apicharsys->models[id].model_data_b[1];
    } else {
        duration = 0.0f;
    }
    for (i = 0; i < 2; i++) {
        pod_roll_target[i] = 0.0f;
        pod_roll[i] = 0.0f;
        pod_animtime[i] = 1.0f;
        if (duration > 0.0f) {
            pod_animtime[i] = (f32)(qrand() * PlayerRandomScale * (duration - 1.0) + pod_animtime[i]);
        }
    }
}

void PodRace_IncreaseLap(void) {
    Lap = (i32)((u32)Lap + 1u);
}

PODRACE_NETPACKET *podrace_netpacket = NULL;

void PodRaceUpdate(WORLDINFO *world, f32 dt) {
    char reason[128];
    f32 old;
    i32 i;
    // MATCH-EXCEPTION: race caches ActivePodRace for the first countdown test only; it changes the operand order of the
    // later mushroom_timer add (fld dt; fadd timer), which no other rewrite reproduced.
    PODRACE_RESOURCE *race = ActivePodRace;
    if (netclient != 0) {
        ActivePodRace->lap_countdown = podrace_netpacket->start_countdown;
        ActivePodRace->lap_display = podrace_netpacket->remaining_time;
        ActivePodRace->previous_lap_display = podrace_netpacket->available_time;
        Lap = podrace_netpacket->lap;
    }
    if (race->lap_countdown > 0.0f) {
        old = ActivePodRace->lap_countdown;
        ActivePodRace->lap_display = ActivePodRace->previous_lap_display;
        avg_currentspeed_mul = 0.0f;
        if (Player[0] != NULL && (Player[0]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
            Player[0]->current_speed_mul = 0.0f;
        }
        if (Player[1] != NULL && (Player[1]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
            Player[1]->current_speed_mul = 0.0f;
        }
        if (netclient == 0 && FadeSys.fade == 0.0f) {
            ActivePodRace->lap_countdown -= dt;
        }
        if (ActivePodRace->lap_countdown < 2.9f && old >= 2.9f) {
            PlaySfx("PodX_EngStartup1", NULL);
        }
        if (ActivePodRace->lap_countdown < 1.5f && old >= 1.5f) {
            PlaySfx("PodX_EngStartup2", NULL);
        }
        if (ActivePodRace->lap_countdown <= 0.0f) {
            PlaySfx("Pod_Race_Go", NULL);
        } else if ((i32)ActivePodRace->lap_countdown != (i32)old) {
            PlaySfx("Pod_Race_Light", NULL);
        }
    } else if (netclient == 0) {
        ActivePodRace->mushroom_timer = dt + ActivePodRace->mushroom_timer;
        if (ActivePodRace->previous_lap_display > 0.0f) {
            ActivePodRace->lap_display = ActivePodRace->previous_lap_display - ActivePodRace->mushroom_timer;
            if (ActivePodRace->lap_display < 0.0f && FadeSys.fade == 0.0f) {
                ActivePodRace->lap_display = 0.0f;
                if (Player[0] != NULL && (Player[0]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                    LoseCoins(Player[0], 1);
                }
                if (Player[1] != NULL && (Player[1]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
                    LoseCoins(Player[1], 1);
                }
                ActivePodRace->flags |= 2;
                KillPlayer(player, 2, 1, NULL);
                ActivePodRace->attempts++;
                if (ActivePodRace->previous_lap_display < ActivePodRace->max_lap_time && ActivePodRace->attempts_per_increment != 0 &&
                    ActivePodRace->attempts % ActivePodRace->attempts_per_increment == 0) {
                    ActivePodRace->previous_lap_display = ActivePodRace->lap_time_increment + ActivePodRace->previous_lap_display;
                    if (ActivePodRace->previous_lap_display > ActivePodRace->max_lap_time) {
                        ActivePodRace->previous_lap_display = ActivePodRace->max_lap_time;
                    }
                }
            }
        }
    }
    for (i = 0; i < 16; i++) {
        if (ActivePodRace->lap_entries[i].next != NULL) {
            PodRaceUpdatePod(&ActivePodRace->lap_entries[i], world);
        }
    }
    if ((ActivePodRace->flags & 1) == 0 && ((Player[0] != NULL && Player[0]->dead != 0) || (Player[1] != NULL && Player[1]->dead != 0))) {
        ActivePodRace->flags |= 1;
        if ((ActivePodRace->flags & 2) != 0) {
            NuStrCpy(reason, "Ep1_PodRace_OutOfTime");
        } else {
            NuStrCpy(reason, "EP1_PODRACE_PODEXPLODE");
        }
        ResetLevel(world, reason, 1);
    }
    if (netclient == 0) {
        podrace_netpacket->start_countdown = ActivePodRace->lap_countdown;
        podrace_netpacket->lap = Lap;
        podrace_netpacket->remaining_time = ActivePodRace->lap_display;
        podrace_netpacket->available_time = ActivePodRace->previous_lap_display;
    }
}

static f32 podhurryalpha = 0.0f;
static f32 podstartracealpha = 0.0f;
struct PODSPRINTNETPACKET_s *podsprint_netpacket = 0;
static f32 podlapalpha = 0.0f;
/* Pod sprint HUD and boulder tuning. */
static f32 PodSprintLapIconScale = 0.16f;
static f32 PodSprintCountdownY = 0.425f;
static f32 PodSprintCountdownScale = 0.75f;

void PodRacePanel(WORLDINFO *world) {
    char text[256];
    f32 scale;
    i32 count;
    if (netclient != 0) {
        podlapalpha = podrace_netpacket->lap_alpha;
    } else {
        podrace_netpacket->lap_alpha = podlapalpha;
    }
    if (podlapalpha > 0.0f) {
        i32 object_index = Lap + 0x135;
        if (world->level_objects[object_index].active != 0) {
            DrawPanel3DObject(0.0f, HudBossIconY, 1.0f, PodSprintLapIconScale, PodSprintLapIconScale, PodSprintLapIconScale, 0, 0, 0,
                              &world->level_objects[object_index].special, 0, podlapalpha);
        }
    }
    if (podhurryalpha > 0.0f && ActivePodRace->lap_display > 0.0f) {
        count = (i32)ActivePodRace->lap_display + 1;
        sprintf(text, "%i", count);
        scale = NuFmod(ActivePodRace->lap_display, 1.0f);
        if (scale >= 0.7f) {
            scale = (f32)(((f64)scale - (f64)0.7f) / (f64)0.3f + 1.0);
        } else {
            scale = 1.0f;
        }
        scale = PodSprintCountdownScale * scale;
        Text3DEx(text, 0.0f, PodSprintCountdownY, 1.0f, scale, scale, scale, 0, 255, 63, 0, (u8)(i32)(podhurryalpha * 128.0));
    }
    if (podstartracealpha != 0.0f && ActivePodRace->lap_countdown > 0.0f) {
        count = (i32)ActivePodRace->lap_countdown + 1;
        sprintf(text, "%i", count);
        scale = NuFmod(ActivePodRace->lap_countdown, 1.0f);
        if (scale >= 0.7f) {
            scale = (f32)(((f64)scale - (f64)0.7f) / (f64)0.3f + 1.0);
        } else {
            scale = 1.0f;
        }
        scale = PodSprintCountdownScale * scale;
        Text3DEx(text, 0.0f, PodSprintCountdownY, 1.0f, scale, scale, scale, 0, 0, 255, 0, (u8)(i32)(podstartracealpha * 128.0));
    }
}

static i32 podhurry_i = 0;

void UpdatePodRaceLapDisplay(f32 dt) {
    f32 target;
    i32 oldhurry;
    if (FadeSys.fade == 0.0f && MiniCutCam == 0 && CUTSTOPGAME == 0) {
        target =
            (Paused != 0 || (WORLD->current_level == PODRACEB_LDATA && GameTimer.time_elapsed >= 1.0f && GameTimer.time_elapsed < 6.0f))
                ? 1.0f
                : 0.0f;
        podlapalpha = SeekLinearF(podlapalpha, target, dt + dt);
    } else {
        podlapalpha = 0.0f;
        podhurryalpha = 0.0f;
        podstartracealpha = 0.0f;
    }
    oldhurry = podhurry_i;
    if (Paused == 0 && ActivePodRace != NULL && ActivePodRace->lap_display < 10.0f) {
        podhurry_i = (i32)ActivePodRace->lap_display;
        if (podhurryalpha < 1.0f) {
            podhurryalpha += dt + dt;
            if (podhurryalpha > 1.0f) {
                podhurryalpha = 1.0f;
            }
        }
    } else {
        podhurryalpha = 0.0f;
    }
    if (podhurry_i > 0 && podhurry_i != -1 && podhurry_i != oldhurry) {
        TickTockSfx();
    }
    if (Paused != 0) {
        return;
    }
    if (ActivePodRace != NULL && ActivePodRace->lap_countdown > 0.0f && podstartracealpha < 1.0f) {
        podstartracealpha += dt + dt;
        if (podstartracealpha > 1.0f) {
            podstartracealpha = 1.0f;
        }
    }
}

static f32 pod_pacemaker_alpha = 0.0f;
static f32 PodPacemakerTextScale = 0.08f;
f32 mushroom0_along = 65.0f;
f32 mushroom2_along = 85.0f;

void UpdatePacemakerDisplay(WORLDINFO *world) {
    NUVEC position;
    GAME_MESSAGE *message;
    if (PodPacemaker == NULL) {
        return;
    }
    if (FadeSys.fade == 0.0f && pause_rndr_on == 0) {
        pod_pacemaker_alpha += FRAMETIME + FRAMETIME;
        if (pod_pacemaker_alpha > 1.0f) {
            pod_pacemaker_alpha = 1.0f;
        }
        if ((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.2f) < (f64)0.1f) {
            position.x = PodPacemaker->upper_position.x;
            position.y = (f32)(PodPacemaker->upper_position.y + 0.75);
            position.z = PodPacemaker->upper_position.z;
            message = AddGameMessage(" ", &position, PodPacemakerTextScale, NULL, 0.0f, 0xff, 0x3f, 0x3f, GAMEMSG_WORLD_POSITION | GAMEMSG_FOLLOW | GAMEMSG_ONE_FRAME | GAMEMSG_OBJECT_ALPHA, 0.0f);
            if (message != NULL) {
                message->alpha = (u8)(NuTrigTable[(i32)(pod_pacemaker_alpha * 16384.0) >> 1 & 0x7fff] * 128.0);
                message->icon = 0x134;
                if (world->level_objects[0x134].active != 0) {
                    message->special = world->level_objects[0x134].special;
                }
            }
        }
    } else {
        pod_pacemaker_alpha = 0.0f;
    }
}

void fun_004daa50(void) {
    UpdatePodRaceLapDisplay(FRAMETIME);
}

i32 PodRace_InStartCountdown(WORLDINFO *world) {
    POD_RACE_COUNTDOWN_READER *race = (POD_RACE_COUNTDOWN_READER *)ActivePodRace;
    return world->area != NULL && world->area == PODRACE_ADATA && race != NULL && race->lap_countdown > 0.0f;
}

void PodRaceA_Update(WORLDINFO *world) {
    UpdatePacemakerDisplay(world);
    PodRaceUpdate(world, FRAMETIME);
    if (netclient == 0) {
        PodRaceAUpdate();
    }
    UpdatePodRaceMines();
}

void PodRaceA_AlwaysUpdate(WORLDINFO *world) {
    UpdatePodRaceLapDisplay(FRAMETIME);
    if (Lap == 3) {
        if (InStory() != 0) {
            if (dat_0087b0b8 != NULL) {
                other_level_override = dat_0087b0b8->index;
                return;
            }
        }
        if (dat_0087b0bc != NULL) {
            other_level_override = dat_0087b0bc->index;
        }
    }
}

void PodRaceA_Draw(WORLDINFO *world) {
    PodRaceADraw(world);
}

/* Mushroom collapse retry window. */
f32 mushroom_time_available = 15.0f;
f32 mushroom_max_time_available = 15.0f;

i32 Action_MushroomCollapse(AISYS *system, AISCRIPTPROCESS *processor, AIPACKET *packet, char **params, i32 count, i32 first_time,
                            f32 elapsed) {
    if (first_time != 0 && mushroom_collapse == 0) {
        i32 index;
        char *value;
        for (index = 0; index < count; ++index) {
            value = NuStrIStr(params[index], "trigger_crash=");
            if (value != NULL) {
                mushroom0_along = AIParamToFloat((AISCRIPTPROCESS *)packet, value + 14);
            } else if ((value = NuStrIStr(params[index], "trigger_collapse=")) != NULL) {
                mushroom2_along = AIParamToFloat((AISCRIPTPROCESS *)packet, value + 17);
            } else if ((value = NuStrIStr(params[index], "max_time_available=")) != NULL) {
                mushroom_max_time_available = AIParamToFloat((AISCRIPTPROCESS *)packet, value + 19);
            } else if ((value = NuStrIStr(params[index], "time_available=")) != NULL) {
                mushroom_time_available = AIParamToFloat((AISCRIPTPROCESS *)packet, value + 15);
            } else if ((value = NuStrIStr(params[index], "time_increment=")) != NULL) {
                mushroom_time_increment = AIParamToFloat((AISCRIPTPROCESS *)packet, value + 15);
            } else if ((value = NuStrIStr(params[index], "nattempts_per_increment=")) != NULL) {
                mushroom_nattempts_per_increment = (i32)AIParamToFloat((AISCRIPTPROCESS *)packet, value + 24);
            }
            if (mushroom_max_time_available < mushroom_time_available) {
                mushroom_max_time_available = mushroom_time_available;
            }
        }
        mushroom_collapse = 1;
    }
    return 1;
}

i32 PodSeekMushCutSound(void) {
    if (WORLD->current_level == PODRACEB_LDATA && Lap == 2 && ActivePodRace->mushroom_timer > 1.0f && mushroom_collapse != 0) {
        if (GameCam->sock_position.distance > mushroom0_along - 15.0 && GameCam->sock_position.distance <= mushroom0_along + 5.0) {
            return 1;
        }
    }
    return 0;
}

i32 PodSeekSubCutSound(void) {
    if (WORLD->current_level == dat_0087b0b4 && Lap == 3 && GameCam->sock_position.distance > 195.0f) {
        if (ActivePodRace->mushroom_timer < ActivePodRace->previous_lap_display - 2.8f + GameCam->sock_position.distance - 195.0) {
            return 1;
        }
    }
    return 0;
}

i32 PodSeekTuskanCutSound(void) {
    if (WORLD->current_level == PODRACEB_LDATA && Lap == 1 && GameCam->sock_position.distance > 182.0f) {
        if (ActivePodRace->mushroom_timer < ActivePodRace->previous_lap_display - 2.8f + GameCam->sock_position.distance - 182.0) {
            return 1;
        }
    }
    return 0;
}

static i32 mushroom_n_attempts = 0;
i32 mushroom_nattempts_per_increment = 0;
SNIPER_s PodRace_snipers[10] = {0};
static CUTINFO *mushroom0_cut = NULL;
i32 mushroom_collapse = 0;
i32 do_mine = 0;
f32 sebulba_fallback_speed_mul = 0;
static f32 mushroom_countdown = 15.0f;
f32 mushroom_time_increment = 1.0f;
f32 sebulba_lerpf = 0.5f;
f32 test_factor = 10.0f;
f32 test_seek_rate = 0.01f;
f32 max_speed_mul = 1.3f;
f32 min_speed_mul = 0.5f;
i32 PodDustLocatorIndices[2] = {1, 2};
f32 PodDustEmissionRate = 100.0f;
f32 PodDustHeightScale = 2.0f;

void PodRaceBUpdate(WORLDINFO *world) {
    if (Lap == 2 && ActivePodRace->mushroom_timer > 1.0f && mushroom_collapse != 0) {
        switch (((PODRACE_LEVFLAG *)LevFlag)->mushroom_state) {
            case 0:
                if (GameCam->sock_position.distance > mushroom0_along) {
                    NewCutScene(NULL, world->cutscene_sys, "ep1_podrace_mushroom0", 1);
                    ((PODRACE_LEVFLAG *)LevFlag)->mushroom_state = 1;
                }
                break;
            case 1:
                if (mushroom0_cut != NULL && (((CUT_INSTANCE_READER *)mushroom0_cut->instance)->flags_88 & NUGCUTSCENE_INST_ENDED) != 0) {
                    NewCutScene(NULL, world->cutscene_sys, "EP1_PODRACE_MUSHROOM1", 1);
                    mushroom_countdown = mushroom_time_available;
                    ((PODRACE_LEVFLAG *)LevFlag)->mushroom_state = 2;
                }
                break;
            case 2:
                mushroom_countdown -= FRAMETIME;
                if (mushroom_countdown < 0.0f) {
                    NewCutScene(NULL, world->cutscene_sys, "EP1_PODRACE_MUSHROOM2", 1);
                    mushroom_n_attempts++;
                    if (mushroom_nattempts_per_increment > 0 && mushroom_n_attempts % mushroom_nattempts_per_increment == 0) {
                        mushroom_time_available += mushroom_time_increment;
                        if (mushroom_time_available > mushroom_max_time_available) {
                            mushroom_time_available = mushroom_max_time_available;
                        }
                    }
                    ((PODRACE_LEVFLAG *)LevFlag)->mushroom_state = 4;
                } else if (GameCam->sock_position.distance > mushroom2_along) {
                    ((PODRACE_LEVFLAG *)LevFlag)->mushroom_state = 3;
                }
                break;
            case 3:
                if (NuSpecialExistsFn(&LevHSpecial[0]) != 0 &&
                    NuSpecialClipTestExtents(&LevHSpecial[0], NuSpecialGetDrawMtx(&LevHSpecial[0])) == 0) {
                    ((PODRACE_LEVFLAG *)LevFlag)->mushroom_state = 4;
                }
                break;
        }
    }
    UpdatePacemakerDisplay(world);
    UpdatePodRaceLapDisplay(FRAMETIME);
    PodRaceUpdate(world, FRAMETIME);
    if (Lap == 1 && ((PODRACE_LEVFLAG *)LevFlag)->podrace_state == 0 && PODRACE_ADATA != NULL &&
        Game.area_save[PODRACE_ADATA->index].area_complete == 0 && GameTimer.time_elapsed >= 3.0f) {
        Hint_SetComplete(0x27e);
        ((PODRACE_LEVFLAG *)LevFlag)->podrace_state = 1;
    }
}

void PodRaceCUpdate(WORLDINFO *world) {
    i32 i;
    UpdatePacemakerDisplay(world);
    UpdatePodRaceLapDisplay(FRAMETIME);
    PodRaceUpdate(world, FRAMETIME);
    PodRaceSnipersUpdate();
    if (((PODRACE_LEVFLAG *)LevFlag)->podrace_state == 0) {
        CUTINFO *cutscene = CutScene_Find(world->cutscene_sys, "Ep1_Podrace_TuskenRaiders");
        if (cutscene != NULL && (((CUT_INSTANCE_READER *)cutscene->instance)->flags_88 & NUGCUTSCENE_INST_ENDED) != 0) {
            ((PODRACE_LEVFLAG *)LevFlag)->podrace_state = 1;
        }
    }
    if (((PODRACE_LEVFLAG *)LevFlag)->podrace_state == 1) {
        for (i = 0; i < 10; i++) {
            if (NuSpecialExistsFn(&LevHSpecial[i]) != 0) {
                NuSpecialSetVisibility(&LevHSpecial[i], 1);
            }
        }
        ((PODRACE_LEVFLAG *)LevFlag)->podrace_state = 2;
    }
}

char PodRaceIncrementKeyword[11] = "increment=";
char PodRaceAttemptsKeyword[25] = "nattempts_per_increment=";
char PodRaceLapTimeKeyword[9] = "laptime=";
char PodRaceMaxLapTimeKeyword[12] = "maxlaptime=";

i32 Action_SetLapTime(AISYS *system, AISCRIPTPROCESS *process, AIPACKET *packet, char **params, i32 param_count, i32 first_time,
                      f32 ignored) {
    i32 i;
    (void)system;
    (void)process;
    (void)ignored;
    if (first_time != 0 && ActivePodRace->previous_lap_display == 0.0f) {
        for (i = 0; i < param_count; ++i) {
            char *value = NuStrIStr(params[i], PodRaceMaxLapTimeKeyword);
            if (value != NULL) {
                ActivePodRace->max_lap_time = AIParamToFloat((AISCRIPTPROCESS *)packet, value + 11);
            } else if ((value = NuStrIStr(params[i], PodRaceLapTimeKeyword)) != NULL) {
                ActivePodRace->previous_lap_display = AIParamToFloat((AISCRIPTPROCESS *)packet, value + 8);
            } else if ((value = NuStrIStr(params[i], PodRaceAttemptsKeyword)) != NULL) {
                ActivePodRace->attempts_per_increment = (i32)AIParamToFloat((AISCRIPTPROCESS *)packet, value + 24);
            } else if ((value = NuStrIStr(params[i], PodRaceIncrementKeyword)) != NULL) {
                ActivePodRace->lap_time_increment = AIParamToFloat((AISCRIPTPROCESS *)packet, value + 10);
            }
        }
        if (ActivePodRace->max_lap_time == 0.0f) {
            ActivePodRace->max_lap_time = ActivePodRace->previous_lap_display;
        }
    }
    return 1;
}

/* The throw timer is read through the process argument. This is the
   processor AIScriptProcess is running, which resets its action_timer on the action's
   first frame. It is often packet->script_processor but can be an alternate processor,
   so it is not always an alias of the packet's own timer. */
i32 Action_Sebulba(AISYS *unused0, AISCRIPTPROCESS *process, AIPACKET *packet, char **unused3, i32 unused4, i32 first_time, float unused6) {
    struct gameobject_s *object;
    struct gameobject_s *target;
    SOCKSYS *sock;
    SOCKPOSITION ahead;
    NUVEC goal;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL) {
        return 1;
    }

    object = packet->owner->objptr;

    if (first_time != 0) {
        NuSpecialFind(vehicle_scene, &SebulbaMineSpecial, "mine", 1);
        object->portal_flags |= APIOBJECT_UPDATE_SOCK_POS;
        object->flags_ef4 |= APIOBJECT_USE_CURRENT_SPEED;
        object->ai_packet.movement_stopped = 1;
        object->movement_speed_scale = 1.0f;
    }

    target = NULL;
    if (player != NULL) {
        target = player2;
        if (target != NULL) {
            f32 x = object->position.x - player->position.x;
            f32 z = object->position.z - player->position.z;
            f32 distance1 = x * x + z * z;
            f32 distance2;
            x = object->position.x - target->position.x;
            z = object->position.z - target->position.z;
            distance2 = x * x + z * z;
            if (distance1 < distance2) {
                target = player;
            }
        }
    }

    sock = WORLD->sock_sys;
    MoveSockPosition(sock, &object->sock_position, sockstep, &ahead);
    if (target != NULL) {
        f32 wanted;
        f32 speed;
        NUVEC target_offset;
        NUVEC object_offset;
        f64 target_distance = MidDistanceFromSockStart(WORLD->sock_sys, &target->sock_position);
        f64 object_distance = MidDistanceFromSockStart(WORLD->sock_sys, &object->sock_position);
        float difference = target_distance - object_distance;
        difference += SebulbaDistanceBias;
        (void)MidDistanceFromSockStart(WORLD->sock_sys, &object->sock_position);
        (void)MidDistanceFromSockStart(WORLD->sock_sys, &target->sock_position);
        wanted = (difference / test_factor + 1.0f) * avg_currentspeed_mul;
        speed = SeekLinearF(object->movement_speed_scale, wanted, test_seek_rate);
        object->movement_speed_scale = speed;
        if (speed > max_speed_mul) {
            object->movement_speed_scale = max_speed_mul;
        } else if (speed < min_speed_mul) {
            object->movement_speed_scale = min_speed_mul;
        }

        NuVecSub(&target_offset, &target->position, &target->sock_position.midpoint);
        NuVecRotateY(&target_offset, &target_offset, -target->sock_heading);
        NuVecSub(&object_offset, &object->position, &object->sock_position.midpoint);
        NuVecRotateY(&object_offset, &object_offset, -object->sock_heading);
        goal.z = 0.0f;
        goal.y = 0.0f;
        goal.x = object_offset.x * sebulba_lerpf + target_offset.x * (1.0f - sebulba_lerpf);
        NuVecAdd(&goal, &goal, &ahead.midpoint);

        process->action_timer += FRAMETIME;
        if (process->action_timer > SebulbaThrowInterval) {
            if (do_mine != 0) {
                if (CreatePodRaceMine(&object->lower_position) != NULL) {
                    process->action_timer = 0.0f;
                }
            } else if (NuSpecialExistsFn(&SebulbaMineSpecial) != 0) {
                NUVEC target_delta;
                NUVEC throw_velocity;
                NUMTX matrix;
                ADDPART part;
                NUVEC origin;
                origin.x = object->position.x;
                origin.y = object->position.y + 1.0f;
                origin.z = object->position.z;
                NuVecSub(&target_delta, &target->velocity, &object->velocity);
                MakeThrowVector(&throw_velocity, &origin, &target->collision_position, &target_delta, PodSebulbaThrowSpeed,
                                PodSebulbaThrowGravity);
                NuVecAdd(&throw_velocity, &throw_velocity, &object->velocity);
                NuMtxSetTranslation(&matrix, &origin);
                part = Default_ADDPART;
                part.matrix = &matrix;
                part.velocity = &throw_velocity;
                part.special = &SebulbaMineSpecial;
                part.unknown_14 = 0.1f;
                part.unknown_18 = 0.1f;
                part.gravity = PodSebulbaThrowGravity;
                part.flags = PART_FLAG_DIE_WHEN_STOPPED | PART_FLAG_COLLIDE_WITH_CHARACTERS | PART_FLAG_DAMAGE_CHARACTERS |
                             PART_FLAG_KEEP_OFF_SCREEN | PART_FLAG_ROTATE_RANDOMLY | PART_FLAG_SLOT_CANNOT_BE_STOLEN;
                part.collision_fn = PartCollide_3D;
                part.time_step = FRAMETIME;
                AddPart(&part);
                process->action_timer = 0.0f;
            }
        }
    } else {
        goal = ahead.midpoint;
        object->movement_speed_scale = 1.0f;
    }

    AIMoveInstruction(packet, &goal, 0.0f, NULL, 1, 0.0f);
    return 0;
}

i32 PodRaceKey[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

void fun_004db710(void) {
    PodRaceKey[0] = -1;
    PodRaceKey[1] = -1;
    PodRaceKey[2] = -1;
    PodRaceKey[3] = -1;
    PodRaceKey[4] = -1;
    PodRaceKey[5] = -1;
    PodRaceKey[6] = -1;
    PodRaceKey[7] = -1;
}

void PodDust(WORLDINFO *world, GAMEOBJECT *object) {
    NUVEC positions[2];
    NUVEC previous[2];
    NUVEC delta;
    NUVEC position;
    NUMTX matrix;
    f32 rate;
    f32 fraction;
    i32 count;
    i32 i;
    i32 locator;
    i32 key;
    i32 particles;
    if (object->floor_height == 2000000.0f) {
        return;
    }
    if (world->game_debris_sys->entries[25].effect == -1) {
        return;
    }
    count = 0;
    for (i = 0; i < 2; ++i) {
        locator = PodDustLocatorIndices[i];
        if (object->origin_joint_ready == 0 || object->character_model->points_of_interest[locator] == NULL) {
            continue;
        }
        positions[count].x = object->locator_matrices[locator].m30;
        positions[count].y = (f32)((f64)object->floor_height + (f64).1f);
        positions[count].z = object->locator_matrices[locator].m32;
        ++count;
        if (!(object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE)) {
            continue;
        }
        if (object != Player[0] && object != Player[1]) {
            continue;
        }
        key = object == Player[0] ? i : i + 4;
        matrix.m00 = object->locator_matrices[locator].m00;
        matrix.m01 = object->locator_matrices[locator].m01;
        matrix.m02 = object->locator_matrices[locator].m02;
        matrix.m03 = object->locator_matrices[locator].m03;
        matrix.m10 = object->locator_matrices[locator].m20;
        matrix.m11 = object->locator_matrices[locator].m21;
        matrix.m12 = object->locator_matrices[locator].m22;
        matrix.m13 = object->locator_matrices[locator].m23;
        matrix.m20 = -object->locator_matrices[locator].m10;
        matrix.m21 = -object->locator_matrices[locator].m11;
        matrix.m22 = -object->locator_matrices[locator].m12;
        matrix.m23 = -object->locator_matrices[locator].m13;
        matrix.m30 = object->locator_matrices[locator].m30;
        matrix.m31 = object->locator_matrices[locator].m31;
        matrix.m32 = object->locator_matrices[locator].m32;
        matrix.m33 = object->locator_matrices[locator].m33;
        if (PodRaceKey[key] == -1) {
            AddDebrisEffect(&PodRaceKey[key], world->game_debris_sys->entries[53].effect, 0, 0, 0);
        } else {
            DebrisPosOrientationMtx(PodRaceKey[key], &matrix);
        }
        if (PodRaceKey[key + 2] == -1) {
            AddDebrisEffect(&PodRaceKey[key + 2], world->game_debris_sys->entries[54].effect, 0, 0, 0);
        } else {
            matrix.m00 = -NuTrigTable[((object->unknown_276 + 0x4000) >> 1) & 0x7fff];
            matrix.m01 = 0;
            matrix.m02 = NuTrigTable[object->unknown_276 >> 1];
            matrix.m10 = -matrix.m02;
            matrix.m11 = 0;
            matrix.m12 = matrix.m00;
            matrix.m20 = 0;
            matrix.m21 = 1;
            matrix.m22 = 0;
            matrix.m31 = (f32)((f64)object->floor_height + (f64).1f);
            DebrisPosOrientationMtx(PodRaceKey[key + 2], &matrix);
        }
    }
    if (count == 0) {
        positions[0].x = object->collision_position.x;
        positions[0].y = object->floor_height;
        positions[0].z = object->collision_position.z;
        count = 1;
    }
    NuVecSub(&delta, &object->start_position, &object->position);
    rate = (f32)(((f64)PodDustEmissionRate * (f64)avg_currentspeed_mul) *
                 (1.0 - (((f64)object->collision_min.y - (f64)object->floor_height) / (f64)PodDustHeightScale)));
    for (i = 0; i < count; ++i) {
        NuVecAdd(&previous[i], &positions[i], &delta);
        particles = ParticlesPerSecond(rate, FRAMETIME);
        for (; particles > 0; --particles) {
            fraction = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f));
            position.x = (f32)((((f64)positions[i].x - (f64)previous[i].x) * (f64)fraction) + (f64)previous[i].x);
            position.y = (f32)((((f64)positions[i].y - (f64)previous[i].y) * (f64)fraction) + (f64)previous[i].y);
            position.z = (f32)((((f64)positions[i].z - (f64)previous[i].z) * (f64)fraction) + (f64)previous[i].z);
            AddVariableShotDebrisEffect(world->game_debris_sys->entries[25].effect, &position, 1, 0, 0);
        }
    }
}

f32 PodSprint_InStartCountdown(WORLDINFO *world) {
    if (world->current_level == PODSPRINTA_LDATA) {
        return podsprint.speed;
    }
    return 0.0f;
}

float PodSprint_RollMul(GAMEOBJECT *object) {
    i16 id = ((i16)(object->id));
    if (id == dat_007f1a98 || id == dat_007f1a2c || id == dat_007f1a9c) {
        return 0.6f;
    }
    if (id == id_XWING || id == dat_007f1a24 || id == dat_007f19a4 || id == id_NEW_REPUBLIC_GUNSHIP) {
        return 0.8f;
    }
    return 1.0f;
}

static void PodSprint_InitAISpline(WORLDINFO *world, PODSPRINT_AI *ai, char *name) {
    NUGSPLINE *line;
    NUVEC *pts;
    i16 *vals;
    i32 count;
    i32 i = 0;
    if (world == NULL || ai == NULL || name == NULL) {
        return;
    }
    memset(ai, 0, sizeof(*ai));
    ai->spline = NuSplineFind(world->current_gscn, name);
    if (ai->spline == NULL || PODSPRINT_STATE->finish_line == NULL || PODSPRINT_STATE->halfway == NULL) {
        return;
    }
    count = 0;
    vals = ai->vals;
    for (; i < ai->spline->length - 1; i++) {
        if (count >= 6) {
            break;
        }
        pts = &ai->spline->pts[i];
        line = PODSPRINT_STATE->finish_line;
        if ((count & 1) == 0) {
            line = PODSPRINT_STATE->halfway;
        }
        if (XZLinesIntersect(&pts[0], &pts[1], &line->pts[0], &line->pts[1], NULL, NULL) != 0) {
            count++;
            *vals++ = (i16)i;
        }
    }
    if (ai->vals[5] == 0) {
        ai->vals[5] = ai->spline->length;
    }
}

void PodSprintA_Init(WORLDINFO *world) {
    memset(&podsprint, 0, sizeof(podsprint));
    podsprint_netpacket = SetLevelHack(10);
    PODSPRINT_STATE->finish_line = NuSplineFind(world->current_gscn, "finish_line");
    if (PODSPRINT_STATE->finish_line != NULL && PODSPRINT_STATE->finish_line->length < 2) {
        PODSPRINT_STATE->finish_line = NULL;
    }
    PODSPRINT_STATE->halfway = NuSplineFind(world->current_gscn, "halfway");
    if (PODSPRINT_STATE->halfway != NULL && PODSPRINT_STATE->halfway->length < 2) {
        PODSPRINT_STATE->halfway = NULL;
    }
    PODSPRINT_STATE->lap_msg = CheckGizAIMessage(gizaimessagesys, "Lap", NULL);
    PODSPRINT_STATE->lap_msg->value = 0.0f;
    PODSPRINT_STATE->max_speed_msg = CheckGizAIMessage(gizaimessagesys, "sebulba_max_speed", NULL);
    PODSPRINT_STATE->min_speed_msg = CheckGizAIMessage(gizaimessagesys, "sebulba_min_speed", NULL);
    PODSPRINT_STATE->speed_step_msg = CheckGizAIMessage(gizaimessagesys, "sebulba_speed_step", NULL);
    PodSprint_InitAISpline(world, &PODSPRINT_STATE->ai[0], "ai_sebulba");
    PodSprint_InitAISpline(world, &PODSPRINT_STATE->ai[1], "ai_general");
    NuSpecialFind(world->current_gscn, &LevHSpecial[50], "bigrock_five", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[51], "bigrock_eight", 1);
}

void PodSprintA_Reset(WORLDINFO *world) {
    i32 i;
    GAMEOBJECT *object;
    PODSPRINT_STATE->bits.reset_requested = 0;
    pod_old_pos[0].x = pod_old_pos[0].y = pod_old_pos[0].z = -1.0f;
    PODSPRINT_STATE->pacemaker = NULL;
    pod_old_pos[1].x = pod_old_pos[1].y = pod_old_pos[1].z = -1.0f;
    PODSPRINT_STATE->lap_time = 0.0f;
    if ((PODSPRINT_STATE->flags & 0xc) == 0 && (netclient == 0 || PODSPRINT_NET->ai_state <= 2)) {
        PODSPRINT_STATE->bits.halfway = 0;
        PODSPRINT_STATE->countdown = 3.0f;
        PODSPRINT_STATE->pacemaker = NULL;
        PODSPRINT_STATE->ai_state = 1;
        podstartracealpha = 0.0f;
        PODSPRINT_STATE->ai_index = 0;
    } else {
        PODSPRINT_STATE->bits.finished = 0;
        VehicleAreaRememberSpeed = 1.0f;
        PODSPRINT_STATE->ai_state = 3;
        PODSPRINT_STATE->ai_index = 4;
        PODSPRINT_STATE->bits.halfway = 0;
        for (i = 0; i < 2; i++) {
            object = i != 0 ? player2 : player;
            if (object != NULL && (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0) {
                object->unknown_dc8 = 1.0f;
                object->velocity.x = 0.0f;
                object->velocity.y = 0.0f;
                object->velocity.z = object->character_data->game_character->run_speed;
                NuVecRotateY(&object->velocity, &object->velocity, object->unknown_276);
            }
        }
        if (game_cutscenes_cutscene != NULL) {
            CutScene_SnapToEnd(game_cutscenes_cutscene);
            CutScene_StoppedFn_LSW(game_cutscenes_cutscene);
        }
    }
    PodRaceSnipersReset();
    PODSPRINT_STATE->boulders = AISysFindArea(world->ai_sys, "Boulders");
}

static f32 PodSprintArrowSize = 0.05f;
static f32 PodSprintArrowHeight = 0.75f;

static void PodSprint_UpdatePacemaker(WORLDINFO *world) {
    NUVEC position;
    GAME_MESSAGE *message;
    if (PODSPRINT_STATE->pacemaker == NULL) {
        PODSPRINT_STATE->pacemaker = GetNamedGameObject(world->ai_sys, "SebulbasPod");
        if (PODSPRINT_STATE->pacemaker == NULL) {
            return;
        }
    }
    if ((PODSPRINT_STATE->pacemaker->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 || PODSPRINT_STATE->pacemaker->dead != 0) {
        return;
    }
    if (FadeSys.fade != 0.0f || pause_rndr_on != 0) {
        PODSPRINT_STATE->arrow_timer = 0.0f;
        return;
    }
    PODSPRINT_STATE->arrow_timer = (FRAMETIME + FRAMETIME) + PODSPRINT_STATE->arrow_timer;
    if (PODSPRINT_STATE->arrow_timer > 1.0f) {
        PODSPRINT_STATE->arrow_timer = 1.0f;
    }
    if ((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.2f) < (f64)0.1f) {
        position = PODSPRINT_STATE->pacemaker->upper_position;
        position.y += PodSprintArrowHeight;
        message = AddGameMessage(" ", &position, PodSprintArrowSize, NULL, 0.0f, 0xff, 0x3f, 0x3f, GAMEMSG_WORLD_POSITION | GAMEMSG_FOLLOW | GAMEMSG_ONE_FRAME | GAMEMSG_OBJECT_ALPHA, 0.0f);
        if (message != NULL) {
            message->alpha = (u8)(NuTrigTable[(i32)(PODSPRINT_STATE->arrow_timer * 16384.0) >> 1 & 0x7fff] * 128.0);
            message->icon = 0x134;
            if (world->level_objects[0x134].active != 0) {
                message->special = world->level_objects[0x134].special;
            }
        }
    }
}

static i32 PodSprint_UpdateCountdown(void) {
    f32 previous;
    if (PODSPRINT_STATE->countdown > 0.0f) {
        previous = PODSPRINT_STATE->countdown;
        if (FadeSys.fade == 0.0f) {
            PODSPRINT_STATE->countdown -= FRAMETIME;
        }
        if (Player[0] != NULL) {
            if (PODSPRINT_STATE->countdown > 0.0f) {
                Player[0]->unknown_dc8 = 0.0f;
                Player[0]->velocity.z = 0.0f;
                Player[0]->velocity.x = 0.0f;
                Player[0]->run_speed_override = 0.0f;
            } else {
                Player[0]->run_speed_override = 1000000000.0f;
            }
        }
        if (Player[1] != NULL) {
            if (PODSPRINT_STATE->countdown > 0.0f) {
                Player[1]->unknown_dc8 = 0.0f;
                Player[1]->velocity.z = 0.0f;
                Player[1]->velocity.x = 0.0f;
                Player[1]->run_speed_override = 0.0f;
            } else {
                Player[1]->run_speed_override = 1000000000.0f;
            }
        }
        if (PODSPRINT_STATE->countdown < 2.9f && previous >= 2.9f) {
            PlaySfx("PodX_EngStartup1", NULL);
        }
        if (PODSPRINT_STATE->countdown < 1.5f && previous >= 1.5f) {
            PlaySfx("PodX_EngStartup2", NULL);
        }
        if (PODSPRINT_STATE->countdown <= 0.0f) {
            PlaySfx("Pod_Race_Go", NULL);
            return 1;
        }
        if ((i32)PODSPRINT_STATE->countdown != (i32)previous) {
            PlaySfx("Pod_Race_Light", NULL);
        }
        return 1;
    }
    return 0;
}

static void PodSprint_UpdateLaps(WORLDINFO *world) {
    i32 i;
    NUVEC *old;
    NUVEC *line;
    NUVEC *line_end;
    if (PODSPRINT_STATE->finish_line != NULL && PODSPRINT_STATE->halfway != NULL) {
        PODSPRINT_STATE->lap_time += FRAMETIME;
        if ((PODSPRINT_STATE->flags & 1) != 0) {
            line = PODSPRINT_STATE->finish_line->pts;
            line_end = &line[1];
            for (i = 0, old = pod_old_pos; i < 2; i++, old++) {
                if (Player[i] != NULL && old->x != -1.0f && old->y != -1.0f && old->z != -1.0f &&
                    XZLinesIntersect(line, line_end, &Player[i]->position, old, NULL, NULL) != 0) {
                    PODSPRINT_STATE->bits.halfway = 0;
                    PODSPRINT_STATE->lap_time = 0.0f;
                    PODSPRINT_STATE->ai_index++;
                    PODSPRINT_STATE->ai_state++;
                    if (PODSPRINT_STATE->ai_state == 3) {
                        PODSPRINT_STATE->flags |= 8;
                        StoreLevelProgress(world);
                        ResetLevel(world, "ep1_podsprint_sebulba", 1);
                    } else if (PODSPRINT_STATE->ai_state > 3) {
                        if (FreePlay != 0) {
                            CompleteLevel(world);
                        } else if (NewCutScene(NULL, WORLD->cutscene_sys, "Ep1_Podrace_Outro1", 1) == 0 &&
                                   NewCutScene(NULL, WORLD->cutscene_sys, "Ep1_Podsprint_Outro1", 1) == 0) {
                            CompleteLevel(world);
                        }
                    }
                    break;
                }
            }
        } else {
            line = PODSPRINT_STATE->halfway->pts;
            line_end = &line[1];
            for (i = 0, old = pod_old_pos; i < 2; i++, old++) {
                if (Player[i] != NULL && XZLinesIntersect(line, line_end, &Player[i]->position, old, NULL, NULL) != 0) {
                    PODSPRINT_STATE->flags |= 1;
                    PODSPRINT_STATE->ai_index++;
                    break;
                }
            }
        }
    }
    if (Player[0] != NULL) {
        pod_old_pos[0] = Player[0]->position;
    }
    if (Player[1] != NULL) {
        pod_old_pos[1] = Player[1]->position;
    }
    PODSPRINT_STATE->lap_msg->value = (f32)PODSPRINT_STATE->ai_state;
}

static void PodSprint_UpdateAlpha(void) {
    if (FadeSys.fade == 0.0f && MiniCutCam == 0 && CUTSTOPGAME == 0) {
        if (PODSPRINT_STATE->countdown > 0.0f) {
            if (Paused == 0 && podstartracealpha < 1.0f) {
                podstartracealpha += FRAMETIME + FRAMETIME;
                if (podstartracealpha > 1.0f) {
                    podstartracealpha = 1.0f;
                }
            }
            podlapalpha = 0.0f;
        } else {
            podlapalpha = SeekLinearF(podlapalpha, (Paused != 0 || PODSPRINT_STATE->lap_time < 6.0f) ? 1.0f : 0.0f, FRAMETIME + FRAMETIME);
        }
    } else {
        podlapalpha = 0.0f;
        podhurryalpha = 0.0f;
        podstartracealpha = 0.0f;
    }
}

static f32 boulder_offset_y = 1.0f;
static f32 boulder_offset_y_seek = 1.0f;
f32 sebulba_goal_ahead = 10.0f;
f32 sebulba_catchup_dist = -3.0f;
f32 sebulba_catchup_speed_mul = 1.5f;
f32 sebulba_catchup_lerpf = 5.0f;
f32 sebulba_fallback_dist = 150.0f;
f32 sebulba_fallback_lerpf = 5.0f;
f32 sebulba_goal_ahead_vals[4] = {100.0f, 75.0f, 50.0f, 25.0f};

void PodSprintA_Update(WORLDINFO *world) {
    i32 i;
    GAMEOBJECT *object;
    f32 seek;
    VehicleAreaRememberSpeed = 1.0f;
    if (nethost != 0) {
        PODSPRINT_NET->ai_state = PODSPRINT_STATE->ai_state;
        PODSPRINT_NET->countdown = (i16)PODSPRINT_STATE->countdown;
        PODSPRINT_NET->lap_time = (i16)PODSPRINT_STATE->lap_time;
    } else if (netclient != 0) {
        PODSPRINT_STATE->countdown = (f32)PODSPRINT_NET->countdown;
        PODSPRINT_STATE->ai_state = (i8)PODSPRINT_NET->ai_state;
        PODSPRINT_STATE->lap_time = (f32)PODSPRINT_NET->lap_time;
    }
    if (netclient == 0) {
        if (CutScene_PlayingOrRequested(NULL) != 0) {
            return;
        }
        if ((PODSPRINT_STATE->flags & 0xc) != 0) {
            if ((PODSPRINT_STATE->flags & 0x10) == 0) {
                PODSPRINT_STATE->flags |= 0x10;
                ResetLevel(world, NULL, 1);
            }
            return;
        }
    }
    if (PodSprint_UpdateCountdown() == 0) {
        PodSprint_UpdatePacemaker(world);
        if (netclient == 0) {
            PodSprint_UpdateLaps(world);
        }
        PodRaceSnipersUpdate();
    }
    object = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; i++, object++) {
        if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) != 0 && (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 &&
            object->player_index == -1) {
            seek = 0.0f;
            if (PODSPRINT_STATE->boulders != NULL && WORLD->ai_sys != NULL && PODSPRINT_STATE->ai_state > 1 &&
                (*(u64 *)object->ai_area_mask & (u64)(i64)(1 << (PODSPRINT_STATE->boulders - WORLD->ai_sys->areas))) != 0) {
                seek = boulder_offset_y;
            }
            object->movement_spline_offset.y = SeekValF(object->movement_spline_offset.y, seek, boulder_offset_y_seek);
        }
    }
}

void PodSprintA_Panel(WORLDINFO *world) {
    char text[256];
    i32 count;
    f32 scale;
    PodSprint_UpdateAlpha();
    if (Paused == 0 && podstartracealpha != 0.0f && PODSPRINT_STATE->countdown > 0.0f) {
        count = (i32)PODSPRINT_STATE->countdown + 1;
        if (count > 3) {
            count = 3;
        }
        sprintf(text, "%i", count);
        scale = NuFmod(PODSPRINT_STATE->countdown, 1.0f);
        if (scale >= 0.7f) {
            scale = (f32)(((f64)scale - (f64)0.7f) / (f64)0.3f + 1.0);
        } else {
            scale = 1.0f;
        }
        scale = PodSprintCountdownScale * scale;
        Text3DEx(text, 0.0f, PodSprintCountdownY, 1.0f, scale, scale, scale, 0, 0, 0xff, 0, (u8)(podstartracealpha * 128.0));
    }
    if (podlapalpha > 0.0f) {
        i32 object_index = 0x135 + PODSPRINT_STATE->ai_state;
        if (world->level_objects[object_index].active != 0) {
            DrawPanel3DObject(0.0f, HudBossIconY, 1.0f, PodSprintLapIconScale, PodSprintLapIconScale, PodSprintLapIconScale, 0, 0, 0,
                              &world->level_objects[object_index].special, 0, podlapalpha);
        }
    }
}

void PodSprint_GetIAlongVals(NUGSPLINE *spline, i16 *out1, i16 *out2) {
    i32 i;
    for (i = 0; i < 6; i++) {
        if (spline != NULL && podsprint.ai[i].spline == spline) {
            if (podsprint.ai_index == 0) {
                *out1 = 0;
                *out2 = podsprint.ai[i].vals[0];
            } else {
                i32 n = podsprint.ai_index;
                if (n >= 5) {
                    n = 5;
                }
                *out1 = podsprint.ai[i].vals[n - 1];
                *out2 = podsprint.ai[i].vals[n];
            }
            return;
        }
    }
}

i32 Action_NewSebulba(AISYS *sys, AISCRIPTPROCESS *process, AIPACKET *packet, char **params, i32 param_count, i32 first_time,
                      f32 delta_time) {
    GAMEOBJECT *object;
    f32 goal;
    f32 base_speed;
    f32 distance;
    f32 ratio;
    f32 lerp;
    if (packet == NULL || packet->owner == NULL || ((APIOBJECT *)packet->owner)->owner == NULL) {
        return 1;
    }
    object = ((APIOBJECT *)packet->owner)->owner;
    if (first_time != 0) {
        object->run_speed_override = object->character_data->game_character->run_speed;
        object->movement_spline_offset.y = 0.0f;
    }
    if (podsprint.speed > 0.0f) {
        object->run_speed_override = 0.0f;
    } else if ((i8)podsprint.ai_state < 3) {
        goal = podsprint.ai_index < 4 ? sebulba_goal_ahead_vals[podsprint.ai_index] : sebulba_goal_ahead_vals[3];
        base_speed = object->character_data->game_character->run_speed;
        if (object->sock_position.sock_index != -1 && object->sock_position.sock_index == player->sock_position.sock_index) {
            SOCKSYS *sock_sys = WORLD->sock_sys;
            SOCK *sock = &sock_sys->socks[object->sock_position.sock_index];
            if (player2 != NULL) {
                GAMEOBJECT *lead;
                distance = MidDistanceFromSockStart(sock_sys, &player2->sock_position) -
                           MidDistanceFromSockStart(WORLD->sock_sys, &player->sock_position);
                if (distance > sock->unknown_98 * 0.5) {
                    distance -= sock->unknown_98;
                } else if (distance < sock->unknown_98 * -0.5) {
                    distance += sock->unknown_98;
                }
                if (goal < 0.0f) {
                    lead = distance > 0.0f ? player : player2;
                } else {
                    lead = distance > 0.0f ? player2 : player;
                }
                distance = MidDistanceFromSockStart(WORLD->sock_sys, &object->sock_position) -
                           MidDistanceFromSockStart(WORLD->sock_sys, &lead->sock_position);
            } else {
                distance = MidDistanceFromSockStart(sock_sys, &object->sock_position) -
                           MidDistanceFromSockStart(WORLD->sock_sys, &player->sock_position);
            }
            if (distance > sock->unknown_98 * 0.5) {
                distance -= sock->unknown_98;
            } else if (distance < sock->unknown_98 * -0.5) {
                distance += sock->unknown_98;
            }
            ratio = distance - goal;
        } else {
            ratio = 0.0f;
        }
        if (ratio > 0.0f) {
            distance = sebulba_fallback_dist - goal;
            ratio = ratio / distance;
            if (ratio > 1.0f) {
                ratio = 1.0f;
            }
            ratio = sebulba_fallback_speed_mul * ratio + (1.0f - ratio);
            lerp = sebulba_fallback_lerpf;
        } else {
            ratio = ratio / sebulba_catchup_dist;
            if (ratio > 1.0f) {
                ratio = 1.0f;
            }
            ratio = sebulba_catchup_speed_mul * ratio + (1.0f - ratio);
            lerp = sebulba_catchup_lerpf;
        }
        object->run_speed_override = SeekValF(object->run_speed_override, ratio * base_speed, lerp);
    } else {
        base_speed = object->character_data->game_character->run_speed;
        if (podsprint.max_speed_msg != NULL && podsprint.min_speed_msg != NULL && podsprint.speed_step_msg != NULL) {
            base_speed = podsprint.max_speed_msg->value - (f32)(*(i16 *)podsprint.unknown_8c) * podsprint.speed_step_msg->value;
            if (base_speed < podsprint.min_speed_msg->value) {
                base_speed = podsprint.min_speed_msg->value;
            }
        }
        object->run_speed_override = SeekValF(object->run_speed_override, base_speed, 5.0f);
    }
    if (object->movement_spline_position.reached_end != 0 && (podsprint.flags & 4) == 0) {
        if (Player[0] != NULL && (Player[0]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
            LoseCoins(Player[0], 1);
        }
        if (Player[1] != NULL && (Player[1]->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
            LoseCoins(Player[1], 1);
        }
        *(u16 *)&podsprint.flags |= 4;
        ResetLevel(WORLD, "Ep1_Podsprint_OutOfTime", 1);
        ++*(i16 *)podsprint.unknown_8c;
    }
    return 0;
}

RETAKEG_NETPACKET *retakeg_netpacket = NULL;
static GIZAIMESSAGE *RetakeG_TotalGuards_msg = NULL;
static GIZAIMESSAGE *RetakeG_GuardsToRescue_msg = NULL;
struct gameobject_s *Maul_obj = 0;

void RetakeG_Init(WORLDINFO *world) {
    retakeg_netpacket = SetLevelHack(4);
    RetakeG_TotalGuards_msg = CheckGizAIMessage(gizaimessagesys, "TotalGuards", NULL);
    RetakeG_GuardsToRescue_msg = CheckGizAIMessage(gizaimessagesys, "GuardsToRescue", NULL);
}

void RetakeG_Update(WORLDINFO *world) {
    if (netclient == 0) {
        retakeg_netpacket->guard_b = (i16)RetakeG_GuardsToRescue_msg->value;
        retakeg_netpacket->guard_a = (i16)RetakeG_TotalGuards_msg->value;
    } else {
        RetakeG_GuardsToRescue_msg->value = (f32)retakeg_netpacket->guard_b;
        RetakeG_TotalGuards_msg->value = (f32)retakeg_netpacket->guard_a;
    }
}

void RetakeG_Panel(WORLDINFO *world) {
    i16 ids[6] = {id_ROYALGUARD, id_ROYALGUARD, id_ROYALGUARD, id_ROYALGUARD, id_ROYALGUARD, id_ROYALGUARD};
    u8 alive[6] = {1, 1, 1, 1, 1, 1};
    i32 i;
    if (RetakeG_TotalGuards_msg != NULL && RetakeG_TotalGuards_msg->value > 0.0f) {
        i32 total = (i32)RetakeG_TotalGuards_msg->value;
        if (total > 6) {
            total = 6;
        }
        if (RetakeG_GuardsToRescue_msg != NULL && RetakeG_GuardsToRescue_msg->value > 0.0f) {
            i32 rescue = (i32)RetakeG_GuardsToRescue_msg->value;
            if (rescue > 6) {
                rescue = 6;
            }
            for (i = 0; i < rescue; i++) {
                alive[i] = 0;
            }
            DrawMeleeTargetsAlpha(ids, alive, NULL, total);
        }
    }
}
static struct gizaimessage_s *MaulA_ai_message = 0;
static GIZAIMESSAGE *MaulA_hits_message = NULL;
nuhspecial_s SebulbaMineSpecial = {0};

void MaulA_Init(WORLDINFO *world) {
    MaulA_ai_message = CheckGizAIMessage(gizaimessagesys, "MaulOnTheRun", NULL);
    MaulA_hits_message = CheckGizAIMessage(gizaimessagesys, "Hits", NULL);
    NuSpecialFind(world->current_gscn, &LevHSpecial[4], "engine_1c", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[5], "engine_2c", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[6], "engine_1d", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[7], "engine_2d", 1);
}

void MaulA_Reset(WORLDINFO *world) {
    nuhspecial_s *slots = LevHSpecial;
    NuSpecialSetVisibility(&slots[4], 0);
    NuSpecialSetVisibility(&slots[5], 0);
    NuSpecialSetVisibility(&slots[6], 0);
    NuSpecialSetVisibility(&slots[7], 0);
    Maul_obj = FindGameObject(id_DARTHMAUL, APIOBJECT_SIDE_BADDIE, 1, 0, 0);
}

void MaulA_Panel(WORLDINFO *world) {
    if (netclient == 0) {
        if (Maul_obj != NULL && MaulA_ai_message != NULL && MaulA_ai_message->value == 0.0f && MaulA_hits_message != NULL) {
            GAMEOBJECT *maul = Maul_obj;
            maul->current_hp = (u8)(i32)MaulA_hits_message->value;
            maul->unknown_107e = 3;
            DrawBossHitPoints(maul);
        } else {
            DrawBossHitPoints(NULL);
        }
    }
}

void MaulF_Init(WORLDINFO *world) {
    struct nuhspecial_s *slots;
    GIZAIMESSAGE *message;
    NUGSCN *scene;
    message = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
    scene = world->current_gscn;
    MaulA_ai_message = message;
    slots = LevHSpecial;
    NuSpecialFind(scene, &slots[0], "throw_object1", 1);
    NuSpecialFind(world->current_gscn, &slots[1], "throw_object2", 1);
    NuSpecialFind(world->current_gscn, &slots[2], "throw_object3", 1);
}

void MaulF_Reset(WORLDINFO *world) {
    Maul_obj = FindGameObject(id_DARTHMAUL, APIOBJECT_SIDE_BADDIE, 1, 0, 0);
}

void MaulF_Panel(WORLDINFO *world) {
    if (netclient == 0) {
        if (Maul_obj != NULL && MaulA_ai_message != NULL && MaulA_ai_message->value == 1.0f) {
            DrawBossHitPoints(Maul_obj);
        } else {
            DrawBossHitPoints(NULL);
        }
    }
}

void AnakinsFlightB_Init(WORLDINFO *world) {
    i32 count;
    gunship_bolts[0] = (i16)BoltType_FindIDByName("trooper_red", world);
    gunship_flags[0] = 0;
    gunship_flags[1] = 0;
    gunship_flags[2] = 0;
    count = NuSpecialFind(world->current_gscn, &LevHSpecial[0], "minifig_1_1", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[1], "minifig_1_2", 1);
    count += NuSpecialFind(world->current_gscn, &LevHSpecial[2], "minifig_1_3", 1);
    if (count == 3) {
        hothtroopers = LevHSpecial;
    }
}

void PodRaceInit(WORLDINFO *world) {
    PODRACE_RESOURCE *podrace;
    i32 id;
    i32 i;
    podrace_netpacket = SetLevelHack(0x14);
    if (world->level_specific_data == NULL) {
        ChrisAllocLevelStuff(world);
    }
    podrace = world->level_specific_data;
    ActivePodRace = podrace;
    memset(podrace, 0, sizeof(*podrace));
    if (netclient == 0) {
        FlightSpline_Init(world, podrace->splines, 32);
    }
    InitPodRaceMines();
    for (i = 0; i < 8; i++) {
        PodRaceKey[i] = -1;
    }
    ResetPodStuff();
    id = apicharsys->playermodelids[id_ANAKINSPOD];
    if (id != -1) {
        CHARACTER_MODEL_LAYERS *entry = &apicharsys->models[id];
        if (entry->model_data_b[1] != NULL) {
            podanimendframe = AnimEndFrame(entry, 1);
        }
    }
}

void PodRaceReset(void) {
    PODRACE_RESOURCE *podrace = ActivePodRace;
    i32 i;
    podrace->mushroom_timer = 0.0f;
    memset(podrace->lap_entries, 0, sizeof(podrace->lap_entries));
    podrace->flags &= 0xfc;
    podhurry_i = -1;
    for (i = 0; i < 8; i++) {
        PodRaceKey[i] = -1;
    }
}

void PodRaceAInit(WORLDINFO *world) {
    PodRaceInit(world);
}

void PodRaceAReset(WORLDINFO *world) {
    PodRaceReset();
    switch (Lap) {
        case 1:
            podrace_section = 3;
            break;
        case 2:
            podrace_section = 6;
            break;
        case 3:
            podrace_section = 9;
            break;
        default:
            break;
    }
    PodPacemaker = NULL;
    PodRaceResetMines();
    if (Lap == 3 && nethost == 0 && netclient == 0) {
        NewCutScene(NULL, world->cutscene_sys, "ep1_podrace_sebulba", 1);
    }
}

void PodRaceBInit(WORLDINFO *world) {
    PodRaceInit(world);
    mushroom_collapse = 0;
    mushroom_nattempts_per_increment = 1;
    mushroom_n_attempts = 0;
    if (Lap <= 1) {
        ActivePodRace->lap_countdown = 3.0f;
    }
}

void PodRaceBReset(WORLDINFO *world) {
    PodRaceReset();
    switch (Lap) {
        case 1:
            podrace_section = 1;
            break;
        case 2:
            podrace_section = 4;
            break;
        case 3:
            podrace_section = 7;
            break;
        default:
            break;
    }
    PodPacemaker = NULL;
    ((PODRACE_LEVFLAG *)LevFlag)->podrace_state = 0;
    ((PODRACE_LEVFLAG *)LevFlag)->mushroom_state = 0;
    mushroom0_cut = CutScene_Find(world->cutscene_sys, "EP1_PODRACE_MUSHROOM0");
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "collapsing_mush", 1);
}

void PodRaceCInit(WORLDINFO *world) {
    char name[256];
    i32 i;
    PodRaceInit(world);
    ((PODRACE_LEVFLAG *)LevFlag)->podrace_state = 0;
    for (i = 0; i < 10; i++) {
        sprintf(name, "boost0%i", i + 1);
        NuSpecialFind(world->current_gscn, &LevHSpecial[i], name, 1);
    }
    if (FreePlay != 0 || (LevelProgressData[world->current_level->index].unknown_2800[0] & 1) != 0) {
        for (i = 0; i < 10; i++) {
            if (NuSpecialExistsFn(&LevHSpecial[i]) != 0) {
                NuSpecialSetVisibility(&LevHSpecial[i], 1);
            }
        }
        ((PODRACE_LEVFLAG *)LevFlag)->podrace_state = 2;
    } else {
        for (i = 0; i < 17; i++) {
            if (NuSpecialExistsFn(&LevHSpecial[i]) != 0) {
                NuSpecialSetVisibility(&LevHSpecial[i], 0);
            }
        }
    }
}

void PodRaceCReset(WORLDINFO *world) {
    void *cs;
    PodRaceReset();
    switch (Lap) {
        case 1:
            podrace_section = 2;
            break;
        case 2:
            podrace_section = 5;
            break;
        case 3:
            podrace_section = 8;
            break;
        default:
            break;
    }
    PodPacemaker = 0;
    PodRaceSnipersReset();
    cs = CutScene_Find(world->cutscene_sys, "ep1_podrace_avalanche");
    if (cs != NULL) {
        switch (Lap) {
            case 2:
                NewCutScene(cs, world->cutscene_sys, 0, 1);
                break;
            case 3:
                CutScene_SnapToEnd(cs);
                break;
        }
    }
}

char PodCreateDamagedKeyword[8] = "damaged";
char PodCreatePacemakerKeyword[10] = "pacemaker";
char PodCreateTimeKeyword[13] = "time_factor=";
char PodCreateEndKeyword[12] = "spline_end=";
char PodCreateStartKeyword[14] = "spline_start=";
char PodCreateSplineKeyword[11] = "spline_id=";

i32 Action_CreatePod(AISYS *system, AISCRIPTPROCESS *process, AIPACKET *packet, char **params, i32 param_count, i32 first_time,
                     f32 ignored) {
    i32 spline_id = -1;
    i32 pacemaker = 0;
    i32 damaged = 0;
    u16 model_id = 0xffff;
    f32 start = 0.0f;
    f32 end = 1.0f;
    f32 time_factor = 1.0f;
    i32 i;
    i32 spline_index;
    PODRACE_RESOURCE *race;
    FLIGHT_SPLINE_RESOURCE *spline;
    RACEPOD_RESOURCE *pod;
    f64 duration;
    f32 difference;
    NUVEC4 position;
    NUVEC4 next_position;
    __declspec(align(16)) NUVEC4 direction;
    (void)system;
    (void)process;
    (void)ignored;
    if (first_time == 0 || param_count <= 0) {
        return 1;
    }
    for (i = 0; i < param_count; ++i) {
        char *value = NuStrIStr(params[i], PodCreateSplineKeyword);
        if (value != NULL) {
            spline_id = (i32)AIParamToFloat((AISCRIPTPROCESS *)packet, value + 10);
        } else if ((value = NuStrIStr(params[i], "type=")) != NULL) {
            if (LevelCharacterTypeIDFn != NULL && LevelCharacterGlobalIDFn != NULL) {
                model_id = LevelCharacterTypeIDFn(value + 5);
                if (model_id != 0xff) {
                    model_id = LevelCharacterGlobalIDFn(model_id);
                }
            }
        } else if ((value = NuStrIStr(params[i], PodCreateStartKeyword)) != NULL) {
            start = AIParamToFloat((AISCRIPTPROCESS *)packet, value + 13);
        } else if ((value = NuStrIStr(params[i], PodCreateEndKeyword)) != NULL) {
            end = AIParamToFloat((AISCRIPTPROCESS *)packet, value + 11);
        } else if ((value = NuStrIStr(params[i], PodCreateTimeKeyword)) != NULL) {
            time_factor = AIParamToFloat((AISCRIPTPROCESS *)packet, value + 12);
        } else if (NuStrICmp(params[i], PodCreatePacemakerKeyword) == 0) {
            pacemaker = 1;
        } else if (NuStrICmp(params[i], PodCreateDamagedKeyword) == 0) {
            damaged = 1;
        }
    }
    if ((u32)spline_id > 31 || model_id == 0xffff) {
        return 1;
    }
    race = ActivePodRace;
    for (spline_index = 0; spline_index < 32; ++spline_index) {
        spline = &race->splines[spline_index];
        if (race->splines[spline_index].id == spline_id) {
            spline = &race->splines[spline_index];
            break;
        }
    }
    if (start < 0.0f) {
        start = 0.0f;
    } else if (start > 1.0f) {
        start = 1.0f;
    }
    if (end < 0.0f) {
        end = 0.0f;
    } else if (end > 1.0f) {
        end = 1.0f;
    }
    if (spline == NULL || spline->point_count == 0 || start == end) {
        return 1;
    }
    pod = RacePodAllocatePrivate();
    if (pod == NULL) {
        return 1;
    }
    pod->start = start;
    pod->model_id = model_id;
    pod->next = (void *)1;
    pod->spline = spline;
    duration = (f64)time_factor * race->previous_lap_display;
    difference = end - start;
    if (duration > 0.0) {
        pod->speed = (difference * (f64)spline->length) / duration;
    } else {
        pod->speed = 1.0f;
    }
    CalcSplinePointFromDist(spline, &position, start * spline->length);
    CalcSplinePointFromDist(spline, &next_position, (start + PodRaceSplineIncrement) * spline->length);
    PodMatrixCopyPrivate(&pod->matrix, &ANI3_EVAL_IDENTITY_MATRIX);
    *(NUVEC4 *)&pod->matrix.m30 = position;
    direction.x = next_position.x - position.x;
    direction.y = next_position.y - position.y;
    direction.z = next_position.z - position.z;
    RacePodAlignPrivate(pod, &direction, 1.0f, 0);
    EpisodeIVuVecSetOrigin(&pod->previous_axis);
    pod->previous_position = position;
    pod->distance = start;
    pod->object = AddDynamicCreature(pod->model_id, (NUVEC *)spline, 0, NULL, NULL, NULL, 1, NULL, NULL, 0, 0);
    if (pod->object == NULL) {
        return 1;
    }
    pod->object->flags_ef4 |= 0x04000000;
    pod->object->movement_mode = 5;
    if (pacemaker != 0) {
        PodPacemaker = pod->object;
        PodPacemaker->flags_ef4 |= APIOBJECT_NO_TIME_BASED_UPDATE;
    }
    if (damaged != 0) {
        pod->object->flags_ef4 |= 0x08000000;
    }
    return 1;
}
