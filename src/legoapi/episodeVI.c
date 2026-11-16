#include "gameapi/ai/aisys/aipath.h"
#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/ai/aisys/gameaiscript.h"
#include "gameapi/edptl.h"
#include "globals.h"
#include "legoapi/actions/combat/hits.h"
#include "legoapi/audio/audio.h"
#include "legoapi/audio/radio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/base/GizBlowupObjectInterface.h"
#include "legoapi/gizmo/base/gizmessage.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/gizmos/object/gizbuildits.h"
#include "legoapi/gizmos/object/gizobstacles.h"
#include "legoapi/gizmos/object/gizpanel.h"
#include "legoapi/gizmos/object/newblowup.h"
#include "legoapi/gizmos/traps/gizforce.h"
#include "legoapi/gizmos/trigger/gizspecial.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolt.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/rtl.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/edsplines.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/render/light/surfaces.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episodeVI.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/nulgtlaser.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/sfx.h"
#include <stdio.h>

typedef struct SARLACC_DISCO {      // size 0x400
    AIAREA *area;                   // 0x000
    nuhspecial_s off[16];           // 0x004
    nuhspecial_s flash[16];         // 0x0c4
    nuhspecial_s select[16];        // 0x184
    nuhspecial_s on[16];            // 0x244
    nuhspecial_s finish[16];        // 0x304
    GIZOBSTACLE *off_obstacle;      // 0x3c4
    GIZOBSTACLE *on_obstacle;       // 0x3c8
    u8 panel_state[16];             // 0x3cc
    i8 count;                       // 0x3dc
    i8 phase;                       // 0x3dd
    i8 first_panel;                 // 0x3de
    i8 second_panel;                // 0x3df
    f32 timer;                      // 0x3e0
    f32 initial_height;             // 0x3e4
    f32 height;                     // 0x3e8
    u8 reserved_3ec[4];             // 0x3ec
    GIZAIMESSAGE *help_message;     // 0x3f0
    GIZAIMESSAGE *complete_message; // 0x3f4
    GIZAIMESSAGE *state_message;    // 0x3f8
    NUVEC *last_selected_position;  // 0x3fc
} SARLACC_DISCO;

f32 SarlaccDiscoBaseOffset = -0.12f;

static SARLACC_DISCO sarlaccdisco = {0};
nuhspecial_s SarlaccLevSpecial[7] = {0};
GIZOBSTACLE *LevGizObst[8] = {0};
void *SarlaccLevelBuildits[2] = {0};
f32 runningTotalPos = 0;
GIZMO *SarlaccObstMirrorBall = NULL;
u64 routemask_efloor_on = 0;
GIZMO *SarlaccForceMirrorBall = NULL;

i16 dat_007f1a7c = -1;
i16 id_TRACTOR = -1;
i16 id_FIRETRUCK = -1;
i16 dat_007f1a88 = -1;
f32 DeathStarZapSizeWidth = 1.0f;
f32 DeathStarZapSizeLength = 1.0f;
f32 DeathStarZapWidthBias = 0.01f;
f32 DeathStarZapEndWidth = 1.5f;
i32 fire_clip_dist = 1000;
f32 EmperorFloorRadius = 0.35f;
f32 EmperorFloorJitter = 0.2f;
f32 EmperorFloorPush = 0.2f;
f32 EmperorFloorSafeXMinimum = 1.5f;
i32 DeathStarZapLaserType = 0;
nuhspecial_s *hspecial_efloor = 0;
i32 EmperorFloorRouteOn = 0;
f32 eFloor_timer = 0;

/* Private owner of the two player timers. */
static f32 slowDownTimer[2] = {0};
SPLINEPOS fireBackPos = {0};
f32 FireDeltaPosition = 0.0f;

typedef struct emperor_fight_packet_s { // size 0x8
    i32 state;                          // 0x0
    u8 enabled;                         // 0x4
    u8 zap;                             // 0x5
    u8 player_floor_mask;               // 0x6
    u8 reserved;                        // 0x7
} EMPEROR_FIGHT_PACKET;

typedef struct level_script_flags_s { // size 0x10
    u8 first;                         // 0x0
    u8 light_flags[4];                // 0x1
    u8 arrow_flags[4];                // 0x5
    u8 remaining[7];                  // 0x9
} LEVEL_SCRIPT_FLAGS;

typedef struct sarlacc_world_s {  // size 0x50d4
    u8 unknown_0000[0x50cc];      // 0x0000
    i32 blowup_target_count;      // 0x50cc
    GIZMOBLOWUP **blowup_targets; // 0x50d0
} SARLACC_WORLD;

typedef struct sarlacc_panel_s { // size 0x63
    u8 unknown_00[0x62];         // 0x00
    u8 state_flags;              // 0x62
} SARLACC_PANEL;

/* Emperor floor packet and private controls. */
typedef struct emperor_update_packet_s { // size 0x8
    i32 state;                           // 0x0
    u8 enabled;                          // 0x4
    u8 zap;                              // 0x5
    i8 player_floor_mask;                // 0x6
    u8 reserved;                         // 0x7
} EMPEROR_UPDATE_PACKET;

/* Fairground party split locator. */
static AILOCATOR *FairgroundSplitPartyLocator;

typedef struct JABBA_ANTINODE_VIEW { // size 0x4c
    u8 unknown_00[0x40];             // 0x00
    f32 base_radius;                 // 0x40
    f32 base_height;                 // 0x44
    u8 unknown_48[2];                // 0x48
    u8 type;                         // 0x4a
} JABBA_ANTINODE_VIEW;

#define EMPEROR_PACKET ((EMPEROR_UPDATE_PACKET *)emperorfighta_netpacket)

void DeathStar2BattleA_AlwaysUpdate(WORLDINFO *world) {
    if (FreePlay == 0 && dat_0087b284 != NULL) {
        other_level_override = dat_0087b284->index;
    }
}

void DeathStar2BattleD_Init(WORLDINFO *world) {
    char name[256];
    i32 i;
    GIZMO *obstacle;
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "reactor1");
    for (i = 1; i < 7; i++) {
        sprintf(name, "lecnode_%i1", i);
        LevGizmo[i] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, name);
        sprintf(name, "obstacle%d", i);
        obstacle = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, name);
        if (obstacle != NULL && obstacle->object != NULL) {
            LevGizObst[i] = obstacle->object;
        }
    }
    LevGizmo[7] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "shield_inner1");
    ((u8 *)LevFlag)[5] = 0;
}

GIZMOBLOWUP *DeathStar2BattleD_InZapRange(GAMEOBJECT *object) {
    GIZMOBLOWUP *nearest = NULL;
    f32 nearest_distance;
    i32 i;
    if (object == NULL || !(object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) || object->dead) {
        return NULL;
    }
    nearest_distance = 225.0f;
    for (i = 1; i < 7; ++i) {
        GIZMO *gizmo = LevGizmo[i];
        GIZMOBLOWUP *blowup;
        f32 distance;
        if (gizmo == NULL || gizmo->object == NULL) {
            continue;
        }
        blowup = LevGizmo[i]->object;
        if ((blowup->status_flags & BLOWUP_STATUS_ACTIVE) && !(blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) && LevGizObst[i] != NULL &&
            LevGizObst[i]->anim_set->state != GAMEANIMSET_STATE_AT_START) {
            distance = NuVecDistSqr(&object->collision_position, &blowup->mid_position, NULL);
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest = blowup;
            }
        }
    }
    return nearest;
}

TIMER AreaTimer = {0};

void DeathStar2BattleD_Update(WORLDINFO *world) {
    GIZMO *shield_gizmo;
    GIZMOBLOWUP *shield;
    GIZMOBLOWUP *blowup;
    GAMEOBJECT *player;
    NUVEC shield_position;
    NUVEC position;
    NUVEC direction;
    NUVEC colour;
    NUVEC delta;
    NUVEC *mid;
    f32 radius;
    f32 length;
    i32 angle;
    i32 i;
    u8 *flags = (u8 *)LevFlag;
    (void)world;
    if (flags[0] > 0 && qrand() < 0x800) {
        NewRumbleAllPlayers((f32)((f64)qrand() * (f64)0.000015259022f), 0.0f, 0, 0);
    }
    shield_gizmo = LevGizmo[7];
    if (shield_gizmo != NULL) {
        shield = shield_gizmo->object;
        if (shield != NULL && (shield->status_flags & BLOWUP_STATUS_BLOWN_UP)) {
            if ((i32)AreaTimer.time_elapsed % 3 == 0 && (i32)AreaTimer.last_time_elapsed % 3 != 0 && NuFloatRand(&GAMERAND) < 0.5f) {
                angle = qrand();
                shield_position = shield->mid_position;
                direction.x = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
                direction.y = 0.0f;
                direction.z = NuTrigTable[(angle >> 1) & 0x7fff];
                AddPickups(0, 0, 1, 0, &shield_position, &direction, 3.0f, -1, (f32)((f64)NuFloatRand(&GAMERAND) * 15.0 + 10.0), 2000000.0f,
                           NULL, 1, 0);
            }
            return;
        }
    }
    for (i = 0; i < 8; ++i) {
        player = Player[i];
        if (player == NULL || player->player.dynamic_light_id == -1) {
            continue;
        }
        blowup = DeathStar2BattleD_InZapRange(player);
        if (blowup == NULL) {
            continue;
        }
        rtlDynamicEnable(Player[i]->player.dynamic_light_id, 1);
        mid = &blowup->mid_position;
        radius = (f64)NuVecDist(mid, &Player[i]->collision_position, &delta) * 0.5;
        rtlDynamicSetRadii(Player[i]->player.dynamic_light_id, radius, (f32)((f64)radius + 5.0));
        qrand();
        colour.x = 0.0f;
        if (qrand() < 0x8000) {
            colour.y = 2.0f;
        } else {
            colour.y = 0.25f;
        }
        colour.z = colour.y;
        rtlDynamicSetColours(Player[i]->player.dynamic_light_id, &colour, NULL);
        player = Player[i];
        position.x = player->collision_position.x + (f64)delta.x * 0.5;
        position.y = player->collision_position.y + (f64)delta.y * 0.5;
        position.z = player->collision_position.z + (f64)delta.z * 0.5;
        rtlDynamicSetPos(player->player.dynamic_light_id, &position);
        length = NuVecDist(&Player[i]->collision_position, mid, &delta);
        NuLgtLaser(DeathStarZapLaserType, DeathStarZapSizeWidth, DeathStarZapSizeLength, DeathStarZapWidthBias, mid, &delta, 0xff808040,
                   DeathStarZapEndWidth, length);
        NewRumble(Player[i]->player.pad->bound_pad, (f32)((f64)qrand() * (f64)0.000015259022f * (f64)0.3f), 0);
        if (qrand() < 0x1000) {
            NewBuzzFrames(Player[i]->player.pad->bound_pad, 1, 0);
        }
        PlaySfx("ForceLightningLp", &Player[i]->collision_position);
        DisorientateCode(Player[i], mid, 225.0f);
    }
}

void DeathStar2BattleFire_Draw(WORLDINFO *world) {
    u16 yaw = 0;
    u16 pitch = 0;
    NUVEC point = v000;
    NUMTX matrix;
    NUVEC difference;
    NUVEC average;
    SPLINEPOS position;
    f32 distance;
    if (Paused != 0) {
        return;
    }

    position = fireSplinePos;
    while (position.normalized_position > 0.0f) {
        NuMtxSetIdentity(&matrix);
        PointAlongSpline(LevelCodeSpline[0], position.normalized_position, &point, &yaw, &pitch, 0);
        Players_AveragePos(&average, NULL);
        difference.x = average.x - position.position.x;
        difference.y = average.y - position.position.y;
        difference.z = average.z - position.position.z;
        distance = NuVecMagSqr(&difference);
        if (distance < fire_clip_dist) {
            AddVariableShotDebrisEffectTimed1(world->game_debris_sys->entries[121].effect, &position.position, 33, FRAMETIME, pitch, yaw,
                                              NULL);
        }
        MoveSplinePosition(&position, -40.0f);
    }

    position = fireBackPos;
    while (position.normalized_position < fireSplinePos.normalized_position) {
        NuMtxSetIdentity(&matrix);
        PointAlongSpline(LevelCodeSpline[0], position.normalized_position, &point, &yaw, &pitch, 0);
        Players_AveragePos(&average, NULL);
        difference.x = average.x - position.position.x;
        difference.y = average.y - position.position.y;
        difference.z = average.z - position.position.z;
        distance = NuVecMagSqr(&difference);
        if (distance < fire_clip_dist) {
            AddVariableShotDebrisEffectTimed1(world->game_debris_sys->entries[121].effect, &position.position, 67, FRAMETIME, pitch, yaw,
                                              NULL);
        }
        MoveSplinePosition(&position, 40.0f);
    }

    position = fireBackPos;
    while (position.normalized_position > 0.0f) {
        NuMtxSetIdentity(&matrix);
        PointAlongSpline(LevelCodeSpline[0], position.normalized_position, &point, &yaw, &pitch, 0);
        Players_AveragePos(&average, NULL);
        difference.x = average.x - position.position.x;
        difference.y = average.y - position.position.y;
        difference.z = average.z - position.position.z;
        distance = NuVecMagSqr(&difference);
        if (distance < fire_clip_dist) {
            AddVariableShotDebrisEffectTimed1(world->game_debris_sys->entries[121].effect, &position.position, 33, FRAMETIME, pitch, yaw,
                                              NULL);
        }
        MoveSplinePosition(&position, -40.0f);
    }
}

void DeathStar2BattleFire_SetSlowDownMul(GAMEOBJECT *object) {
    i8 index;
    if (WORLD->current_level == DEATHSTAR2BATTLEE_LDATA || WORLD->current_level == DEATHSTAR2BATTLEF_LDATA ||
        WORLD->current_level == DEATHSTAR2BATTLEG_LDATA) {
        if (object != NULL) {
            index = object->player_index;
            if (index == 0 || index == 1) {
                object->velocity = v000;
                slowDownTimer[index] = 3.0f;
            }
        }
    }
}

static void DeathStar2BattleFire_UpdateSlowDownMul(f32 dt) {
    i32 index;
    for (index = 0; index < 2; ++index) {
        if (slowDownTimer[index] > 0.0f) {
            slowDownTimer[index] -= dt;
        }
    }
}

f32 DeathStar2BattleFire_GetSlowDownMul(GAMEOBJECT *object) {
    // MATCH-EXCEPTION: the original reads unknown_280 as a signed char (movsx) at each use; the header declares it u8
    if (object == NULL || (object->unknown_280 != 0 && object->unknown_280 != 1)) {
        return 1.0f;
    }
    if (slowDownTimer[object->player_index] < 0.0f) {
        slowDownTimer[object->player_index] = 0.0f;
    }
    return (f32)((3.0 - (f64)slowDownTimer[object->player_index]) / 3.0);
}

void EmperorFightA_Init(WORLDINFO *world) {
    LEVEL_SCRIPT_FLAGS *flags = (LEVEL_SCRIPT_FLAGS *)&LevFlag;
    i32 i;
    emperorfighta_netpacket = SetLevelHack(8);
    ((EMPEROR_FIGHT_PACKET *)emperorfighta_netpacket)->enabled = 1;
    ((EMPEROR_FIGHT_PACKET *)emperorfighta_netpacket)->enabled = 1;
    NuSpecialFind(world->current_gscn, &LevHSpecial[3], "lift_rlight_bon1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[4], "lift_rlight_bon2", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[5], "lift_rlight_bon3", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[6], "lift_rlight_bon4", 1);
    for (i = 0; i < 4; ++i) {
        flags->light_flags[i] = 0;
    }
    NuSpecialFind(world->current_gscn, &LevHSpecial[7], "lift_rarr_on1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[8], "lift_rarr_on2", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[9], "lift_rarr_on3", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[10], "lift_rarr_on4", 1);
    for (i = 0; i < 4; ++i) {
        flags->arrow_flags[i] = 0;
    }
    NuSpecialFind(world->current_gscn, &LevHSpecial[30], "dot_on_01", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[31], "dot_on_02", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[32], "dot_on_03", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[33], "dot_on_04", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[34], "dot_on_05", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[35], "dot_on_06", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[36], "dot_on_07", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[37], "dot_on_08", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[38], "dot_on_09", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[39], "dot_on_10", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[40], "dot_on_11", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[41], "dot_on_12", 1);
    memset(LevSfxFlag, 0, 3 * sizeof(LevSfxFlag[0]));
}

void EmperorFightA_Reset(WORLDINFO *world) {
    GIZMO *gizmo;
    i32 i;
    char name[32];
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
    LevAIMessage[1] = CheckGizAIMessage(gizaimessagesys, "ElectricFloor", NULL);
    gizmo = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, "obstacle21");
    if (gizmo != NULL && gizmo->object != NULL) {
        LevGizObst[0] = gizmo->object;
    }
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force15");
    LevGizmo[1] = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force16");
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "lift_force_lnull1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], "lift_force_rnull1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], "lift_main1", 1);
    LevArea[0] = AISysFindArea(world->ai_sys, "ElectricFloor");
    eFloor_timer = 0.0f;
    ((EMPEROR_FIGHT_PACKET *)emperorfighta_netpacket)->state = 0;
    routemask_efloor_on = 0;
    routemask_efloor_off = 0;
    for (i = 0; i < world->ai_sys->path_sys->active_path->route_count; ++i) {
        if (NuStrICmp(world->ai_sys->path_sys->active_path->routes[i].name, "electric_on") == 0) {
            routemask_efloor_on = (u64)1 << i;
        } else if (NuStrICmp(world->ai_sys->path_sys->active_path->routes[i].name, "electric_off") == 0) {
            routemask_efloor_off = (u64)1 << i;
        }
        if (routemask_efloor_on != 0 && routemask_efloor_off != 0) {
            break;
        }
    }
    hspecial_efloor = &LevHSpecial[67];
    for (i = 0; i < 20; ++i) {
        if (i < 9) {
            sprintf(name, "elecpad_0%d_on", i + 1);
        } else {
            sprintf(name, "elecpad_%d_on", i + 1);
        }
        NuSpecialFind(world->current_gscn, &hspecial_efloor[i], name, 1);
    }
    gizmo = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, "eFloor_AlwaysOn");
    if (gizmo != NULL && gizmo->object != NULL) {
        LevGizObst[1] = gizmo->object;
    }
    gizmo = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, "eFloor_AlwaysOf");
    if (gizmo != NULL && gizmo->object != NULL) {
        LevGizObst[2] = gizmo->object;
    }
    gizmo = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, "eFloor_OnOff");
    if (gizmo != NULL && gizmo->object != NULL) {
        LevGizObst[3] = gizmo->object;
    }
}

void EmperorFightA_UpdateFloorObstacles(void) {
    EMPEROR_UPDATE_PACKET *packet = emperorfighta_netpacket;
    switch (packet->state) {
        case 1:
            GizObstacle_PlayForwards(LevGizObst[1]);
            GizObstacle_PlayForwards(LevGizObst[2]);
            GizObstacle_PlayForwards(LevGizObst[3]);
            break;
        case 2:
            GizObstacle_PlayForwards(LevGizObst[1]);
            GizObstacle_PlayBackwards(LevGizObst[2]);
            GizObstacle_PlayBackwards(LevGizObst[3]);
            break;
        case 3:
            GizObstacle_PlayForwards(LevGizObst[1]);
            GizObstacle_PlayBackwards(LevGizObst[2]);
            GizObstacle_PlayForwards(LevGizObst[3]);
            break;
        case 0:
        case 4:
            GizObstacle_PlayBackwards(LevGizObst[1]);
            GizObstacle_PlayBackwards(LevGizObst[2]);
            GizObstacle_PlayBackwards(LevGizObst[3]);
            break;
    }
}

void EmperorFightA_Update(WORLDINFO *world) {
    i32 i;
    i32 floor;
    i32 floor_active;
    NUVEC hands[2];
    NUVEC end;
    NUVEC delta;

    if (LevGameObject[0] == NULL) {
        LevGameObject[0] = FindGameObject(id_THEEMPEROR, APIOBJECT_SIDE_BADDIE, 1, 0, 0);
    }
    if (FreePlay) {
        KillBossCompleteLevel(id_THEEMPEROR, 0, 0.0f);
    } else {
        KillBossPlayCutScene(id_THEEMPEROR, 0, 0.0f, "emperorfight_outro");
    }
    if (!netclient) {
        GAMEOBJECT *player;
        GIZAIMESSAGE *message;

        EMPEROR_PACKET->player_floor_mask = 0;
        player = Player[0];
        if (player != NULL && !player->dead && player->unknown_1010 <= 0.0f && LevArea[0] != NULL &&
            (U64_PAIR(player->ai_area_mask) & (1 << ((LevArea[0]) - world->ai_sys->areas)))) {
            EMPEROR_PACKET->player_floor_mask |= 1;
        }
        player = Player[1];
        if (player != NULL && !player->dead && player->unknown_1010 <= 0.0f && LevArea[0] != NULL &&
            (U64_PAIR(player->ai_area_mask) & (1 << ((LevArea[0]) - world->ai_sys->areas)))) {
            EMPEROR_PACKET->player_floor_mask |= 2;
        }
        message = LevAIMessage[1];
        if (message->value == 1.0f && EMPEROR_PACKET->player_floor_mask != 0) {
            if (!EMPEROR_PACKET->zap) {
                EMPEROR_PACKET->zap = 1;
                EMPEROR_PACKET->state = 1;
                eFloor_timer = 0.0f;
            } else {
                eFloor_timer = FRAMETIME + eFloor_timer;
                switch (EMPEROR_PACKET->state) {
                    case 1:
                    case 3:
                        if (eFloor_timer > 2.0f) {
                            EMPEROR_PACKET->state = 2;
                            eFloor_timer = 0.0f;
                        }
                        break;
                    case 2:
                        if (eFloor_timer > 3.0f) {
                            EMPEROR_PACKET->state = 3;
                            eFloor_timer = 0.0f;
                        }
                        break;
                    default:
                        break;
                }
            }
        } else if (message->value == 2.0f) {
            EMPEROR_PACKET->zap = 0;
            EMPEROR_PACKET->state = 4;
        }
    }
    for (i = 0; i < 2; ++i) {
        GAMEOBJECT *player;
        player = Player[i];
        if (player != NULL && player->player.character_context == CHARACTER_CONTEXT_ZAPPED_BY_FLOOR) {
            player->player.character_context = CHARACTER_CONTEXT_NONE;
        }
    }
    if (netclient && EMPEROR_PACKET->zap && EMPEROR_PACKET->player_floor_mask && LevGameObject[0] != NULL) {
        AILOCATOR *locator;
        locator = AIPathFindLocator(world->ai_sys, "Zap");

        ForceLightning_Origin(LevGameObject[0], &hands[0], &hands[1]);
        end = locator->position;
        end.x = (f32)(((f64)0.2f - (f64)NuRandFloat() * (f64)0.4f) + (f64)end.x);
        end.z = (f32)(((f64)0.2f - (f64)NuRandFloat() * (f64)0.4f) + (f64)end.z);
        for (i = 0; i < 2; ++i) {
            f32 distance;
            if ((f64)hands[i].y == 1000000000.0) {
                continue;
            }
            distance = NuVecDist(&end, &hands[i], &delta);
            NuLgtLaser(lightning_type, lightning_sizew[0], lightning_sizel[0], lightning_sizewab[0], &hands[i], &delta, lightning_col[0],
                       lightning_endw[0], distance);
            PlaySfx("ForceLightningLp", &locator->position);
        }
        LevGameObject[0]->ai_packet.movement_look_target = &locator->position;
    }
    floor_active = 0;
    if (hspecial_efloor != NULL && EMPEROR_PACKET->player_floor_mask) {
        for (floor = 0; floor < 20; ++floor) {
            NUINSTANIM *animation;
            NUVEC *floor_position;
            f64 radius;
            animation = NuSpecialGetInstAnim(&hspecial_efloor[floor]);
            if (animation == NULL || !(animation->ltime > 1.0f)) {
                continue;
            }
            floor_position = NuSpecialGetDrawPos(&hspecial_efloor[floor]);
            floor_active = 1;
            radius = EmperorFloorRadius;
            for (i = 0; i < 2; ++i) {
                GAMEOBJECT *player;
                f32 dx;
                f32 negative_radius;
                f32 dz;
                f32 push;
                NUVEC floor_end;
                NUVEC player_end;
                NUVEC floor_delta;
                f64 jitter;
                f64 random;
                f64 clamp_radius;
                f64 upper;
                f64 floor_y;
                GAMEOBJECT *jitter_player;
                f32 height;
                i32 random_integer;
                f32 distance;
                NUVEC *sound_position;
                i32 sound;
                player = Player[i];
                if (player->dead || !(player->unknown_1010 <= 0.0f) || !(EMPEROR_PACKET->player_floor_mask & (1 << i))) {
                    continue;
                }
                if ((i8)player->flags_1fc_word < 0) {
                    i32 invincible;
                    invincible = Player_HasInvincibility(player);
                    radius = EmperorFloorRadius;
                    if (invincible) {
                        continue;
                    }
                }
                player = Player[i];
                if (!(player->position.y > floor_position->y)) {
                    continue;
                }
                dx = player->position.x - floor_position->x;
                if (!(dx < radius)) {
                    continue;
                }
                negative_radius = (f32)-radius;
                if (!(dx > negative_radius)) {
                    continue;
                }
                dz = player->position.z - floor_position->z;
                if (!(dz < radius && dz > negative_radius)) {
                    continue;
                }
                player->player.character_context = CHARACTER_CONTEXT_ZAPPED_BY_FLOOR;
                player = Player[i];
                if (!(player->flags_1f8 & 0x40000)) {
                    ObjHitObj(NULL, player, 1, 0, 0, 1);
                }
                player = Player[i];
                push = EmperorFloorPush;
                if (dx > 0.0f) {
                    player->velocity.x += push;
                } else {
                    player->velocity.x -= push;
                }
                player = Player[i];
                if (dz > 0.0f) {
                    player->velocity.z += push;
                } else {
                    player->velocity.z -= push;
                }

                floor_end.x = player->collision_position.x;
                jitter = EmperorFloorJitter;
                random = NuRandFloat();
                floor_end.x = (f32)((jitter - (random + random) * jitter) + (f64)floor_end.x);
                if (floor_end.x > floor_position->x + EmperorFloorRadius) {
                    floor_end.x = floor_position->x + EmperorFloorRadius;
                } else if (floor_position->x - EmperorFloorRadius > floor_end.x) {
                    floor_end.x = floor_position->x - EmperorFloorRadius;
                }
                floor_y = floor_position->y;
                jitter_player = Player[i];
                floor_end.y = (f32)floor_y;
                floor_end.z = jitter_player->collision_position.z;
                jitter = EmperorFloorJitter;
                random = NuRandFloat();
                floor_end.z = (f32)((jitter - (random + random) * jitter) + (f64)floor_end.z);
                clamp_radius = EmperorFloorRadius;
                upper = clamp_radius + floor_position->z;
                if (floor_end.z > upper) {
                    floor_end.z = (f32)upper;
                } else {
                    f64 lower;
                    lower = floor_position->z - clamp_radius;
                    if (lower > floor_end.z) {
                        floor_end.z = (f32)lower;
                    }
                }
                player = Player[i];
                player_end.x = player->collision_position.x;
                height = player->upper_position.y - player->lower_position.y;
                random_integer = qrand();
                player_end.y = (f32)(((f64)random_integer * 0.000015259021893143654) * ((f64)height * 0.5) +
                                     ((f64)height * 0.25 + player->lower_position.y));
                player_end.z = player->collision_position.z;
                distance = NuVecDist(&player_end, &floor_end, &floor_delta);
                NuLgtLaser(testlaser_type, testlaser_sizew, testlaser_sizel, testlaser_sizewab, &floor_end, &floor_delta, 0xff808040,
                           testlaser_endw, distance);
                sound_position = &Player[i]->collision_position;
                sound = GetSfxId("ForceLightningLp");
                GameAudio_PlaySfxById(sound, sound_position, 0, 1);
                break;
            }
        }
        if (floor_active) {
            GAMEOBJECT *player;
            f64 minimum;
            player = Player[0];
            minimum = EmperorFloorSafeXMinimum;
            if (!player->dead && player->unknown_1010 <= 0.0f && LevArea[0] != NULL &&
                (U64_PAIR(player->ai_area_mask) & (1 << ((LevArea[0]) - world->ai_sys->areas))) && player->collision_position.x < minimum) {
                player->flags_ef4 |= 4;
                Player[0]->last_safe_position.x = (f32)minimum;
                Player[0]->respawn_position.x = (f32)minimum;
            }
            player = Player[1];
            if (!player->dead && player->unknown_1010 <= 0.0f && LevArea[0] != NULL &&
                (U64_PAIR(player->ai_area_mask) & (1 << ((LevArea[0]) - world->ai_sys->areas))) && player->collision_position.x < minimum) {
                player->flags_ef4 |= 4;
                Player[1]->last_safe_position.x = (f32)minimum;
                Player[1]->respawn_position.x = (f32)minimum;
            }
        }
    }

    if (!netclient && routemask_efloor_on && routemask_efloor_off) {
        if (EMPEROR_PACKET->enabled || (floor_active && !EmperorFloorRouteOn)) {
            EMPEROR_PACKET->enabled = 0;
            EmperorFloorRouteOn = 1;
            for (i = 0; i < world->ai_sys->path_sys->active_path->connection_count; ++i) {
                if (world->ai_sys->path_sys->active_path->connections[i].route_mask & routemask_efloor_on) {
                    world->ai_sys->path_sys->active_path->connections[i].traversal_flags[0] &= ~AIPATHCNX_TEMPORARY_BLOCK;
                    world->ai_sys->path_sys->active_path->connections[i].traversal_flags[1] &= ~AIPATHCNX_TEMPORARY_BLOCK;
                } else if (world->ai_sys->path_sys->active_path->connections[i].route_mask & routemask_efloor_off) {
                    world->ai_sys->path_sys->active_path->connections[i].traversal_flags[0] |= AIPATHCNX_TEMPORARY_BLOCK;
                    world->ai_sys->path_sys->active_path->connections[i].traversal_flags[1] |= AIPATHCNX_TEMPORARY_BLOCK;
                }
            }
        } else {
            if (floor_active || !EmperorFloorRouteOn) {
                goto emperor_routes_done;
            }
            EmperorFloorRouteOn = 0;
            for (i = 0; i < world->ai_sys->path_sys->active_path->connection_count; ++i) {
                if (world->ai_sys->path_sys->active_path->connections[i].route_mask & routemask_efloor_on) {
                    world->ai_sys->path_sys->active_path->connections[i].traversal_flags[0] |= AIPATHCNX_TEMPORARY_BLOCK;
                    world->ai_sys->path_sys->active_path->connections[i].traversal_flags[1] |= AIPATHCNX_TEMPORARY_BLOCK;
                } else if (world->ai_sys->path_sys->active_path->connections[i].route_mask & routemask_efloor_off) {
                    world->ai_sys->path_sys->active_path->connections[i].traversal_flags[0] &= ~AIPATHCNX_TEMPORARY_BLOCK;
                    world->ai_sys->path_sys->active_path->connections[i].traversal_flags[1] &= ~AIPATHCNX_TEMPORARY_BLOCK;
                }
            }
        }
        {
            GAMEOBJECT *boss;
            for (i = 0; i < 2; ++i) {
                GAMEOBJECT *player;
                player = Player[i];
                if (player != NULL && (player->flags_1fc_word & (APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_CHARACTER)) ==
                                          (APIOBJECT_FLAG_IN_USE | APIOBJECT_FLAG_CHARACTER)) {
                    AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT *)player, &player->ai_packet, 0xff, 1);
                }
            }
            boss = LevGameObject[0];
            if (boss != NULL) {
                AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT *)boss, &boss->ai_packet, 0xff, 1);
            }
        }
    }
emperor_routes_done:
    EmperorFightA_UpdateFloorObstacles();
    if (!((u8 *)LevFlag)[4]) {
        for (i = 1; i < 5; ++i) {
            if (!((u8 *)LevFlag)[i] && NuSpecialGetVisibilityFn(&LevHSpecial[i + 2])) {
                PlaySfx("env_block_light_on", NuSpecialGetDrawPos(&LevHSpecial[i + 2]));
                ((u8 *)LevFlag)[i] = 1;
            }
        }
    }
    for (i = 0; i < 12; ++i) {
        if (NuSpecialGetVisibilityFn(&LevHSpecial[i + 30]) && !((u8 *)LevSfxFlag)[i]) {
            PlaySfx("env_block_light_on", NuSpecialGetDrawPos(&LevHSpecial[i]));
            ((u8 *)LevSfxFlag)[i] = 1;
        } else if (!NuSpecialGetVisibilityFn(&LevHSpecial[i + 30])) {
            ((u8 *)LevSfxFlag)[i] = 0;
        }
    }
}

void EmperorFightA_Panel(WORLDINFO *world) {
    DrawBossHitPointsTwoRows = 1;
    if (LevGameObject[0] != NULL && LevAIMessage[0] != NULL && LevAIMessage[0]->value == 1.0f) {
        DrawBossHitPoints(LevGameObject[0]);
    }
}

void JabbasPalaceA_Init(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "grill_02", 1);
}

void JabbasPalaceA_Reset(WORLDINFO *world) {
    TerSurface[14].flags = 0xa040;
    TerSurface[9].flags = 0x2040;
}

void JabbasPalaceA_Update(WORLDINFO *world) {
    NUVEC position = {-0.01503f, 0.7414f, 11.5824f};
    JABBA_ANTINODE_VIEW *antinode;
    if (NuSpecialExistsFn(&LevHSpecial[0]) != 0 && NuSpecialGetVisibilityFn(&LevHSpecial[0]) == 0) {
        antinode = (JABBA_ANTINODE_VIEW *)AIAntinodeCreateSingleFrame(&position, 1.0f);
        if (antinode != NULL) {
            antinode->type = 2;
            antinode->base_radius = 0.82443f;
            antinode->base_height = 1.0f;
        }
    }
}

void JabbasPalaceB_Init(WORLDINFO *world) {
    LevGizObst[0] = GizObstacle_FindByName(world->giz_obstacle_sys, "obstacle5");
    LevBlowUp[0] = GizmoBlowUp_FindByName(world, "prison_stone1");
}

void JabbasPalaceB_Reset(WORLDINFO *world) {
    TerSurface[14].flags = 0xa040;
    TerSurface[9].flags = 0x2040;
}

void JabbasPalaceD_Reset(WORLDINFO *world) {
    TerSurface[14].flags = 0xa040;
    TerSurface[9].flags = 0x2040;
}

void JabbasPalaceE_Init(WORLDINFO *world) {
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "prox_explo1");
    LevGizmo[1] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "prox_explo2");
    ((u8 *)LevFlag)[6] = 0;
    ((u8 *)LevFlag)[7] = 0;
}

void JabbasPalaceE_Reset(WORLDINFO *world) {
    LevGameObject[0] = GetNamedGameObject(world->ai_sys, "AI_rancor");
    LevArea[0] = AISysFindArea(world->ai_sys, "Alcove_1");
    LevArea[1] = AISysFindArea(world->ai_sys, "Alcove_2");
    LevArea[2] = AISysFindArea(world->ai_sys, "Back_Room");
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
    TerSurface[14].flags = 0xa040;
    TerSurface[9].flags = 0x2040;
}

void JabbasPalaceE_Update(WORLDINFO *world) {
    i32 i;
    GAMEOBJECT *player;
    GIZMOBLOWUP *blowup;
    u8 *flags = (u8 *)LevFlag;
    if (!netclient) {
        if (FreePlay) {
            KillBossCompleteLevel(id_RANCOR, 0, 0.3f);
        } else {
            KillBossNewLevel(id_RANCOR, 0, 0.3f, dat_0087b2b0->index);
        }
    }
    if (LevGameObject[0] != NULL) {
        LevGameObject[0]->excluded_opponents = 0;
        if (aicreature_sets_alive[0] != 0) {
            for (i = 0; i < 8; ++i) {
                player = Player[i];
                if (player != NULL) {
                    LevGameObject[0]->excluded_opponents |= (u64)1 << player->hostility_group;
                }
            }
        } else {
            for (i = 0; i < 8; ++i) {
                player = Player[i];
                if (player != NULL &&
                    ((LevArea[0] != NULL && (U64_PAIR(player->ai_area_mask) & (1 << (LevArea[0] - world->ai_sys->areas)))) ||
                     (LevArea[1] != NULL && (U64_PAIR(player->ai_area_mask) & (1 << (LevArea[1] - world->ai_sys->areas)))) ||
                     (LevArea[2] != NULL && (U64_PAIR(player->ai_area_mask) & (1 << (LevArea[2] - world->ai_sys->areas)))))) {
                    LevGameObject[0]->excluded_opponents |= (u64)1 << player->hostility_group;
                }
            }
        }
    }
    blowup = LevGizmo[0] != NULL ? (GIZMOBLOWUP *)LevGizmo[0]->object : NULL;
    if (blowup != NULL && (blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) != 0) {
        if (!flags[6]) {
            PlaySfx("exp_minecart", &blowup->position);
            flags[6] = 1;
        }
    } else {
        flags[6] = 0;
    }
    blowup = LevGizmo[1] != NULL ? (GIZMOBLOWUP *)LevGizmo[1]->object : NULL;
    if (blowup != NULL && (blowup->status_flags & BLOWUP_STATUS_BLOWN_UP) != 0) {
        if (!flags[7]) {
            PlaySfx("exp_minecart", &blowup->position);
            flags[7] = 1;
        }
    } else {
        flags[7] = 0;
    }
}

void JabbasPalaceE_Panel(WORLDINFO *world) {
    if (netclient == 0) {
        if (LevGameObject[0] != NULL && LevAIMessage[0] != NULL && LevAIMessage[0]->value == 1.0f) {
            DrawBossHitPoints(LevGameObject[0]);
        } else {
            DrawBossHitPoints(NULL);
        }
    }
}

void SarlaccPitA_Reset(WORLDINFO *world) {
    LevSafePlatID[0] = -1;
    if (NuSpecialFind(world->current_gscn, &LevHSpecial[0], "float_skiff_2", 1) != 0 && world->terrain != NULL) {
        LevSafePlatID[0] = FindPlatInst(NuSpecialGetInstanceix(&LevHSpecial[0]));
    }
}

void SarlaccPitA_Draw(WORLDINFO *unused0) {
    GIZAIMESSAGE *message = NULL;
    if (gizaimessagesys != NULL) {
        message = CheckGizAIMessage(gizaimessagesys, "Boba_Dead", NULL);
    }
    if (message == NULL || message->value != 0.0f) {
        DrawBossHitPoints(NULL);
    } else {
        GAMEOBJECT *boba = FindGameObject(id_BOBAFETT, APIOBJECT_SIDE_BADDIE, 1, 1, 0);
        if (gizaimessagesys != NULL) {
            message = CheckGizAIMessage(gizaimessagesys, "BobaFightStarted", NULL);
        }
        if (boba != NULL && message != NULL && message->value == 1.0f) {
            DrawBossHitPoints(boba);
        }
    }
}

void SarlaccPitB_SpecialUpdate(WORLDINFO *world) {
    i32 index;
    u32 bit;
    NUVEC *position;
    NUINSTANIM *animation;
    (void)world;
    for (index = 0; index < sarlaccdisco.count; ++index) {
        NuSpecialSetVisibility(&sarlaccdisco.off[index], 0);
        NuSpecialSetVisibility(&sarlaccdisco.flash[index], 0);
        NuSpecialSetVisibility(&sarlaccdisco.select[index], 0);
        NuSpecialSetVisibility(&sarlaccdisco.on[index], 0);
        NuSpecialSetVisibility(&sarlaccdisco.finish[index], 0);
        if (netclient) {
            sarlaccdisco.height = SeekValF(sarlaccdisco.height, sarlaccb_netpacket->height, 8.0f);
        } else {
            sarlaccb_netpacket->height = sarlaccdisco.height;
        }
        bit = 1u << index;
        /* Expands the show-and-raise code separately in each branch. */
#define SARLACC_SHOW_PANEL(handle)                                                                                                         \
    do {                                                                                                                                   \
        NuSpecialSetVisibility(handle, 1);                                                                                                 \
        position = NuSpecialGetDrawPos(handle);                                                                                            \
        if (position != NULL) {                                                                                                            \
            position->y = sarlaccdisco.height;                                                                                             \
            NuSpecialSetDrawPos(handle, position);                                                                                         \
        }                                                                                                                                  \
    } while (0)
        if (sarlaccb_netpacket->off_mask & bit) {
            SARLACC_SHOW_PANEL(&sarlaccdisco.off[index]);
        } else if (sarlaccb_netpacket->flash_mask & bit) {
            SARLACC_SHOW_PANEL(&sarlaccdisco.flash[index]);
        } else if (sarlaccb_netpacket->select_mask & bit) {
            SARLACC_SHOW_PANEL(&sarlaccdisco.select[index]);
        } else if (sarlaccb_netpacket->on_mask & bit) {
            SARLACC_SHOW_PANEL(&sarlaccdisco.on[index]);
        } else if (sarlaccb_netpacket->finish_mask & bit) {
            SARLACC_SHOW_PANEL(&sarlaccdisco.finish[index]);
        }
#undef SARLACC_SHOW_PANEL
        position = NuSpecialGetDrawPos(&LevHSpecial[1]);
        if (position != NULL) {
            position->y = SarlaccDiscoBaseOffset + sarlaccdisco.height;
            NuSpecialSetDrawPos(&LevHSpecial[1], position);
        }
        if (sarlaccb_netpacket->sound_mask & bit) {
            PlaySfx("Kam_DiscoFloorPanelOn", &NuSpecialGetDrawMtx(&sarlaccdisco.on[index])->translation);
        }
    }
    if (sarlaccb_netpacket->disco_active != 0) {
        SetGizAIMessage(gizaimessagesys, "DiscoComplete", 1.0f, sarlaccdisco.complete_message);
        PlayRadio("Speaker2", "Speaker21", 1);
        PlayRadio("Speaker1", "Speaker11", 1);
        PlayRadio("decks", "decks", 1);
        if (SarlaccObstMirrorBall != NULL) {
            GizObstacle_PlayForwards(SarlaccObstMirrorBall->object);
        }
        if (NuSpecialExistsFn(&SarlaccLevSpecial[0])) {
            NuSpecialSetVisibility(&SarlaccLevSpecial[0], 1);
        }
        if (NuSpecialExistsFn(&SarlaccLevSpecial[1])) {
            NuSpecialSetVisibility(&SarlaccLevSpecial[1], 1);
        }
        if (NuSpecialExistsFn(&SarlaccLevSpecial[4])) {
            animation = NuSpecialGetInstAnim(&SarlaccLevSpecial[4]);
            if (animation != NULL) {
                animation->flags |= NUINSTANIM_FLAG_PLAYING;
            }
        }
        if (NuSpecialExistsFn(&SarlaccLevSpecial[5]) && NuSpecialExistsFn(&SarlaccLevSpecial[6])) {
            animation = NuSpecialGetInstAnim(&SarlaccLevSpecial[5]);
            if (animation != NULL) {
                animation->flags |= NUINSTANIM_FLAG_PLAYING;
            }
            animation = NuSpecialGetInstAnim(&SarlaccLevSpecial[6]);
            if (animation != NULL) {
                animation->flags |= NUINSTANIM_FLAG_PLAYING;
            }
        }
        if (sarlaccb_netpacket->completion_sound_played == 0) {
            PlaySfx("Kam_DiscoFloorPanelDone", NuSpecialGetDrawPos(&sarlaccdisco.off[0]));
            sarlaccb_netpacket->completion_sound_played = 1;
        }
    } else {
        PlayRadio("Speaker2", "Speaker21", 0);
        PlayRadio("Speaker1", "Speaker11", 0);
        PlayRadio("decks", "decks", 0);
        sarlaccb_netpacket->completion_sound_played = 0;
    }
}

i32 SarlaccPitDiscoActive(WORLDINFO *world) {
    return world->current_level == SARLACCPITB_LDATA && sarlaccb_netpacket->disco_active != 0;
}

void SarlaccPitB_Init(WORLDINFO *world) {
    char name[32];
    i8 *index;
    NUVEC *position;
    (void)world;
    memset(&sarlaccdisco, 0, sizeof(sarlaccdisco));
    sarlaccb_netpacket = SetLevelHack(20);
    index = &sarlaccdisco.count;
    *index = 0;
    for (;;) {
        if (*index < 9) {
            sprintf(name, "dot_off_0%d", *index + 1);
        } else {
            sprintf(name, "dot_off_%d", *index + 1);
        }
        NuSpecialFind(WORLD->current_gscn, &sarlaccdisco.off[*index], name, 1);
        if (*index < 9) {
            sprintf(name, "dot_flash_0%d", *index + 1);
        } else {
            sprintf(name, "dot_flash_%d", *index + 1);
        }
        NuSpecialFind(WORLD->current_gscn, &sarlaccdisco.flash[*index], name, 1);
        if (*index < 9) {
            sprintf(name, "dot_select_0%d", *index + 1);
        } else {
            sprintf(name, "dot_select_%d", *index + 1);
        }
        NuSpecialFind(WORLD->current_gscn, &sarlaccdisco.select[*index], name, 1);
        if (*index < 9) {
            sprintf(name, "dot_on_0%d", *index + 1);
        } else {
            sprintf(name, "dot_on_%d", *index + 1);
        }
        NuSpecialFind(WORLD->current_gscn, &sarlaccdisco.on[*index], name, 1);
        if (*index < 9) {
            sprintf(name, "dot_finish_0%d", *index + 1);
        } else {
            sprintf(name, "dot_finish_%d", *index + 1);
        }
        NuSpecialFind(WORLD->current_gscn, &sarlaccdisco.finish[*index], name, 1);
        if (!NuSpecialExistsFn(&sarlaccdisco.off[*index]) || !NuSpecialExistsFn(&sarlaccdisco.flash[*index]) ||
            !NuSpecialExistsFn(&sarlaccdisco.select[*index]) || !NuSpecialExistsFn(&sarlaccdisco.on[*index]) ||
            !NuSpecialExistsFn(&sarlaccdisco.finish[*index])) {
            break;
        }
        if (*index == 0) {
            position = NuSpecialGetPos(&sarlaccdisco.off[0]);
            if (position != NULL) {
                sarlaccdisco.initial_height = position->y;
                sarlaccdisco.height = position->y;
            }
        }
        ++*index;
        if (*index >= 16) {
            break;
        }
    }
    sarlaccdisco.area = AISysFindArea(WORLD->ai_sys, "DISCO");
    NuSpecialFind(WORLD->current_gscn, &LevHSpecial[0], "force_engine_lump", 1);
    NuSpecialFind(WORLD->current_gscn, &LevHSpecial[1], "disco_base", 1);
}

void SarlaccPitB_Reset(WORLDINFO *world) {
    i32 index;
    sarlaccdisco.timer = 0.0f;
    memset(sarlaccdisco.panel_state, 0, sizeof(sarlaccdisco.panel_state));
    sarlaccdisco.phase = 0;
    sarlaccdisco.first_panel = -1;
    sarlaccdisco.second_panel = -1;
    for (index = 0; index < sarlaccdisco.count; ++index) {
        NuSpecialSetVisibility(&sarlaccdisco.off[index], 1);
        NuSpecialSetVisibility(&sarlaccdisco.flash[index], 0);
        NuSpecialSetVisibility(&sarlaccdisco.on[index], 0);
        NuSpecialSetVisibility(&sarlaccdisco.finish[index], 0);
    }
    sarlaccdisco.help_message = SetGizAIMessage(gizaimessagesys, "HelpWithDisco", 0.0f, NULL);
    sarlaccdisco.complete_message = SetGizAIMessage(gizaimessagesys, "DiscoComplete", 0.0f, NULL);
    sarlaccdisco.state_message = SetGizAIMessage(gizaimessagesys, "DiscoState", 0.0f, NULL);
    sarlaccdisco.off_obstacle = GizObstacle_FindByName(world->giz_obstacle_sys, "disco_off");
    sarlaccdisco.on_obstacle = GizObstacle_FindByName(world->giz_obstacle_sys, "disco_on");
    NuSpecialFind(world->current_gscn, &SarlaccLevSpecial[0], "floor_disco", 1);
    NuSpecialFind(world->current_gscn, &SarlaccLevSpecial[1], "light1_a", 1);
    NuSpecialFind(world->current_gscn, &SarlaccLevSpecial[2], "disco_ball1", 1);
    NuSpecialFind(world->current_gscn, &SarlaccLevSpecial[3], "disco_ball2", 1);
    NuSpecialFind(world->current_gscn, &SarlaccLevSpecial[4], "shutter_1", 1);
    NuSpecialFind(world->current_gscn, &SarlaccLevSpecial[5], "jabba_door1", 1);
    NuSpecialFind(world->current_gscn, &SarlaccLevSpecial[6], "jabba_door2", 1);
    SarlaccForceMirrorBall = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force5");
    SarlaccObstMirrorBall = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, "obstacle5");
    SarlaccLevelBuildits[0] = GizBuildIt_Find(world, "buildit6");
    SarlaccLevelBuildits[1] = GizBuildIt_Find(world, "buildit4");
    LevelLocator = AIPathFindLocator(world->ai_sys, "DiscoHelp");
    if (NuSpecialExistsFn(&SarlaccLevSpecial[3])) {
        NuSpecialSetVisibility(&SarlaccLevSpecial[3], 0);
    }
    if (NuSpecialExistsFn(&SarlaccLevSpecial[0])) {
        NuSpecialSetVisibility(&SarlaccLevSpecial[0], 0);
    }
    if (NuSpecialExistsFn(&SarlaccLevSpecial[1])) {
        NuSpecialSetVisibility(&SarlaccLevSpecial[1], 0);
    }
    if (GizForce_Complete(SarlaccForceMirrorBall->object)) {
        GizObstacle_Stop(SarlaccObstMirrorBall->object);
    }
}

static i32 SarlaccDiscoPickPanel(i32 state, i32 exclude) {
    i32 candidates[16];
    i32 count = 0;
    i32 index;
    i32 panel_count = sarlaccdisco.count;
    for (index = 0; index < panel_count; ++index) {
        if (index != exclude && (i8)sarlaccdisco.panel_state[index] == state) {
            candidates[count++] = index;
        }
    }
    return count != 0 ? candidates[NuRand(NULL) % count] : -1;
}

f32 SarlaccDiscoHeight = 1.27f;
f32 SarlaccDiscoSeek = 2.0f;
f32 SarlaccDiscoTime = 40.0f;
f32 SarlaccDiscoTimeout = 2.5f;
f32 SarlaccDiscoRadius = 0.2f;
static NUVEC sarlacc_lump_position = {0};
static f32 sarlacc_engine_timer = 0.0f;

void SarlaccPitB_Update(WORLDINFO *world) {
    i32 index;
    if (netclient == 0) {
        GIZMO *panel;
        i32 ready;
        sarlaccb_netpacket->sound_mask = 0;
        SetGizAIMessage(gizaimessagesys, "HelpWithDisco", 0.0f, sarlaccdisco.help_message);
        SetGizAIMessage(gizaimessagesys, "DiscoComplete", 0.0f, sarlaccdisco.complete_message);
        SetGizAIMessage(gizaimessagesys, "DiscoState", (f32)sarlaccdisco.phase, sarlaccdisco.state_message);
        panel = GizmoFindByName(world->gizmo_sys, gizspecial_gizmotype_id, "qaz_panel2");
        GizmoGetOutput(world->gizmo_sys, panel, 0, 0);
        ready = GizBuildIt_AtEnd(SarlaccLevelBuildits[0]) && GizBuildIt_AtEnd(SarlaccLevelBuildits[1]) &&
                ((struct GIZSPECIAL_s *)panel->object)->anim_set->state == GAMEANIMSET_STATE_AT_END;
        if (ready && sarlaccdisco.complete_message->value == 0.0f) {
            SetGizAIMessage(gizaimessagesys, "discoflooropen", 1.0f, NULL);
        }
        switch (sarlaccdisco.phase) {
            case 0:
                sarlaccb_netpacket->disco_active = 0;
                if (NuSpecialExistsFn(&SarlaccLevSpecial[0]) && ready &&
                    ((player != NULL && (U64_PAIR(player->ai_area_mask) & (i32)(1u << (sarlaccdisco.area - world->ai_sys->areas)))) ||
                     (player2 != NULL && (U64_PAIR(player2->ai_area_mask) & (i32)(1u << (sarlaccdisco.area - world->ai_sys->areas)))))) {
                    sarlaccdisco.timer = 0.0f;
                    sarlaccdisco.phase = 1;
                    sarlaccdisco.first_panel = (i8)SarlaccDiscoPickPanel(0, -1);
                    sarlaccdisco.second_panel = (i8)SarlaccDiscoPickPanel(0, sarlaccdisco.first_panel);
                    if (sarlaccdisco.first_panel != -1 && sarlaccdisco.second_panel != -1) {
                        sarlaccdisco.panel_state[sarlaccdisco.first_panel] = 1;
                        sarlaccdisco.panel_state[sarlaccdisco.second_panel] = 1;
                    }
                }
                break;
            case 1: {
                i32 controlled_panel;
                i32 occupied[2];
                i32 pass;
                sarlaccb_netpacket->disco_active = 0;
                if (!ready) {
                    SarlaccPitB_Reset(world);
                    break;
                }
                controlled_panel = -1;
                for (pass = 0; pass < 2; ++pass) {
                    i8 *selected = &sarlaccdisco.first_panel + pass;
                    i32 previous = (i8)sarlaccdisco.panel_state[*selected];
                    NUVEC *position;
                    sarlaccdisco.panel_state[*selected] = 1;
                    occupied[pass] = 0;
                    position = NuSpecialGetPos(&sarlaccdisco.flash[*selected]);
                    for (index = 0; index < 8; ++index) {
                        GAMEOBJECT *object = Player[index];
                        if (object != NULL && (object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) &&
                            (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) && object->unknown_281 != 0) {
                            f32 dx = position->x - object->position.x;
                            f32 dz = position->z - object->position.z;
                            f32 distance = dx * dx + dz * dz;
                            if (distance < SarlaccDiscoRadius * SarlaccDiscoRadius) {
                                occupied[pass] = 1;
                                sarlaccdisco.panel_state[*selected] = 2;
                                if (object == player) {
                                    controlled_panel = *selected;
                                }
                                break;
                            }
                        }
                    }
                    if (previous != (i8)sarlaccdisco.panel_state[*selected] && sarlaccdisco.panel_state[*selected] == 2) {
                        sarlaccdisco.last_selected_position = &NuSpecialGetDrawMtx(&sarlaccdisco.on[*selected])->translation;
                        sarlaccb_netpacket->sound_mask |= 1 << *selected;
                    }
                }
                if (occupied[0] && occupied[1]) {
                    sarlaccdisco.timer = 0.0f;
                    sarlaccdisco.panel_state[sarlaccdisco.first_panel] = 3;
                    sarlaccdisco.panel_state[sarlaccdisco.second_panel] = 3;
                    sarlaccdisco.first_panel = (i8)SarlaccDiscoPickPanel(0, -1);
                    sarlaccdisco.second_panel = (i8)SarlaccDiscoPickPanel(0, sarlaccdisco.first_panel);
                    if (sarlaccdisco.first_panel == -1 || sarlaccdisco.second_panel == -1) {
                        sarlaccdisco.phase = 2;
                        for (index = 0; index < sarlaccdisco.count; ++index) {
                            sarlaccdisco.panel_state[index] = 4;
                        }
                    } else {
                        sarlaccdisco.panel_state[sarlaccdisco.first_panel] = 1;
                        sarlaccdisco.panel_state[sarlaccdisco.second_panel] = 1;
                    }
                } else {
                    sarlaccdisco.timer += FRAMETIME;
                    if (sarlaccdisco.timer > SarlaccDiscoTimeout) {
                        sarlaccdisco.timer = 0.0f;
                        for (index = 0; index < 2; ++index) {
                            i32 selected = SarlaccDiscoPickPanel(3, -1);
                            if (selected != -1) {
                                sarlaccdisco.panel_state[selected] = 0;
                            }
                        }
                    }
                    if (player2 == NULL && LevelLocator != NULL) {
                        GAMEOBJECT *partner = Player[0];
                        if (player == partner) {
                            partner = Player[1];
                        }
                        if (partner != NULL) {
                            NUVEC *help_position = NULL;
                            if (controlled_panel == sarlaccdisco.first_panel) {
                                help_position = NuSpecialGetPos(&sarlaccdisco.flash[sarlaccdisco.second_panel]);
                            } else if (controlled_panel == sarlaccdisco.second_panel) {
                                help_position = NuSpecialGetPos(&sarlaccdisco.flash[sarlaccdisco.first_panel]);
                            }
                            if (help_position != NULL) {
                                SetGizAIMessage(gizaimessagesys, "HelpWithDisco", 1.0f, sarlaccdisco.help_message);
                                ((AILOCATOR *)LevelLocator)->position.x = help_position->x;
                                ((AILOCATOR *)LevelLocator)->position.z = help_position->z;
                            }
                        }
                    }
                }
                break;
            }
            case 2:
                sarlaccb_netpacket->disco_active = 1;
                sarlaccdisco.timer += FRAMETIME;
                if (sarlaccdisco.timer > SarlaccDiscoTime || !ready) {
                    sarlaccb_netpacket->disco_active = 0;
                    SarlaccPitB_Reset(world);
                    return;
                }
                break;
        }
        if (sarlaccdisco.phase != 0) {
            f32 amount = (SarlaccDiscoHeight - sarlaccdisco.height) / (SarlaccDiscoHeight - sarlaccdisco.initial_height);
            f32 rate;
            amount = amount < 0.0f ? 0.0f : amount > 1.0f ? 1.0f : amount;
            rate = (f32)(1.0 - ((f64)NuTrigTable[((i32)((f64)amount * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5);
            amount = rate * SarlaccDiscoSeek;
            sarlaccdisco.height = SeekValF(sarlaccdisco.height, SarlaccDiscoHeight, amount);
        }
        sarlaccb_netpacket->off_mask = 0;
        sarlaccb_netpacket->flash_mask = 0;
        sarlaccb_netpacket->select_mask = 0;
        sarlaccb_netpacket->on_mask = 0;
        sarlaccb_netpacket->finish_mask = 0;
        for (index = 0; index < sarlaccdisco.count; ++index) {
            switch ((i8)sarlaccdisco.panel_state[index]) {
                case 0:
                    sarlaccb_netpacket->off_mask |= 1 << index;
                    break;
                case 1:
                    sarlaccb_netpacket->flash_mask |= 1 << index;
                    break;
                case 2:
                    sarlaccb_netpacket->select_mask |= 1 << index;
                    break;
                case 3:
                    sarlaccb_netpacket->on_mask |= 1 << index;
                    break;
                case 4:
                    sarlaccb_netpacket->finish_mask |= 1 << index;
                    break;
            }
        }
    }
    if (((u8 *)LevFlag)[0] == 0 && NuSpecialGetVisibilityFn(&LevHSpecial[0])) {
        NUVEC *position = NuSpecialGetDrawPos(&LevHSpecial[0]);
        sarlacc_engine_timer = 0.05f;
        sarlacc_lump_position = *position;
        ((u8 *)LevFlag)[0] = 1;
    }
    if (((u8 *)LevFlag)[0] != 0) {
        sarlacc_engine_timer -= FRAMETIME;
        if (sarlacc_engine_timer <= 0.0f) {
            sarlacc_engine_timer = 0.1f;
        }
        NuSpecialGetDrawPos(&LevHSpecial[0])->z =
            (f32)((1.0 - ((f64)NuTrigTable[((i32)((f64)sarlacc_engine_timer * 10.0 * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5) *
                      (f64)0.01f -
                  (f64)0.005f + (f64)sarlacc_lump_position.z);
        PlaySfx("env_curtain_lp", NuSpecialGetDrawPos(&LevHSpecial[0]));
    }
    SarlaccPitB_SpecialUpdate(world);
}

void SarlaccPitC_Init(WORLDINFO *world) {
    SarlaccPower = 0;
    SarlaccRecharging = 0;
    SarlaccTargetShield[0] = 0;
    SarlaccTargetShield[1] = 0;
}

static i32 SarlaccBlowUpTargetCount = 0;
static GIZMOBLOWUP *SarlaccBlowUpTargets[4] = {0};
static i32 SarlaccTakenOver = 0;
i32 SarlaccPower = 0;
i32 SarlaccRecharging = 0;

void SarlaccPitC_Reset(WORLDINFO *world) {
    char name[32];
    GIZMO *gizmo;
    i32 i;
    SarlaccTakenOver = 0;
    SarlaccBlowUpTargetCount = 0;
    LevGameObject[0] = GetNamedGameObject(world->ai_sys, "cannon_1");
    for (i = 0; i < 2; ++i) {
        sprintf(name, "cover_%d", i + 1);
        NuSpecialFind(world->current_gscn, &LevHSpecial[i + 16], name, 1);
        sprintf(name, "cover_%d1", i + 1);
        gizmo = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, name);
        if (gizmo != NULL) {
            SarlaccBlowUpTargets[SarlaccBlowUpTargetCount] = gizmo->object;
            SarlaccBlowUpTargets[SarlaccBlowUpTargetCount]->status_flags |= 0x8000000;
            ++SarlaccBlowUpTargetCount;
            UpdateMidPos(gizmo->object);
        }
    }
    for (i = 0; i < 2; ++i) {
        sprintf(name, "spinner%d_null_1", i + 1);
        gizmo = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, name);
        if (gizmo != NULL) {
            SarlaccBlowUpTargets[SarlaccBlowUpTargetCount] = gizmo->object;
            ++SarlaccBlowUpTargetCount;
            UpdateMidPos(gizmo->object);
        }
    }
    for (i = 0; i < 8; ++i) {
        sprintf(name, "shot_0%d", i + 1);
        NuSpecialFind(world->current_gscn, &LevHSpecial[i], name, 1);
    }
    for (i = 0; i < 8; ++i) {
        sprintf(name, "shot_0%da", i + 1);
        NuSpecialFind(world->current_gscn, &LevHSpecial[i + 8], name, 1);
    }
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, gizpanel_gizmotype_id, "panel2");
    for (i = 0; i < 8; ++i) {
        NuSpecialSetVisibility(&LevHSpecial[i], i >= SarlaccPower);
        NuSpecialSetVisibility(&LevHSpecial[i + 8], i < SarlaccPower);
    }
}

static f32 SarlaccRechargeTimer = 0;

void SarlaccPitC_Update(WORLDINFO *world) {
    SARLACC_WORLD *script_world = (SARLACC_WORLD *)world;
    if (LevGameObject[0] != NULL && SarlaccBlowUpTargetCount == 4) {
        if (LevGameObject[0]->player.rider != NULL) {
            script_world->blowup_target_count = 4;
            script_world->blowup_targets = SarlaccBlowUpTargets;
            if (!SarlaccTakenOver) {
                SarlaccTakenOver = 1;
            }
            if ((u32)SarlaccPower > 0) {
                if (LevGameObject[0]->player.quick_shoot_bolt_id >= 0) {
                    SARLACC_PANEL *panel;
                    --SarlaccPower;
                    NuSpecialSetVisibility(&LevHSpecial[SarlaccPower], 1);
                    NuSpecialSetVisibility(&LevHSpecial[SarlaccPower + 8], 0);
                    panel = LevGizmo[0]->object;
                    if (panel->state_flags & 2) {
                        panel->state_flags &= ~2;
                    }
                }
            } else if (LevGameObject[0]->portal_flags & APIOBJECT_CAN_USE_WEAPON) {
                LevGameObject[0]->portal_flags &= ~APIOBJECT_CAN_USE_WEAPON;
            }
        } else if (SarlaccTakenOver) {
            SarlaccTakenOver = 0;
            script_world->blowup_target_count = 0;
            script_world->blowup_targets = NULL;
        }
        if (!SarlaccRecharging) {
            if ((u32)SarlaccPower < 8 && LevGizmo[0] != NULL && GizmoGetOutput(world->gizmo_sys, LevGizmo[0], 0, 0) != 0) {
                SarlaccRecharging = 1;
                SarlaccRechargeTimer = 0.0f;
            }
        } else {
            SarlaccRechargeTimer -= FRAMETIME;
            if (SarlaccRechargeTimer <= 0.0f) {
                LevGameObject[0]->portal_flags |= APIOBJECT_CAN_USE_WEAPON;
                NuSpecialSetVisibility(&LevHSpecial[SarlaccPower], 0);
                NuSpecialSetVisibility(&LevHSpecial[SarlaccPower + 8], 1);
                PlaySfx("imp_c3po_magnet_drop", NuSpecialGetDrawPos(&LevHSpecial[SarlaccPower + 8]));
                ++SarlaccPower;
                if (SarlaccPower == 8) {
                    SarlaccRecharging = 0;
                    PlaySfx("env_magnet_on", NuSpecialGetDrawPos(&LevHSpecial[16]));
                } else {
                    SarlaccRechargeTimer = 0.25f;
                }
            }
        }
    }
    if (!SarlaccTargetShield[0]) {
        GIZMOBLOWUP *target = SarlaccBlowUpTargets[0];
        if (target != NULL && !(target->status_flags & BLOWUP_STATUS_VISIBLE)) {
            PlaySfx("ffieldoff", NuSpecialGetDrawPos(&LevHSpecial[16]));
            SarlaccTargetShield[0] = 1;
        } else {
            PlaySfx("ffield", NuSpecialGetDrawPos(&LevHSpecial[16]));
        }
    }
    if (!SarlaccTargetShield[1]) {
        GIZMOBLOWUP *target = SarlaccBlowUpTargets[1];
        if (target != NULL && !(target->status_flags & BLOWUP_STATUS_VISIBLE)) {
            PlaySfx("ffieldoff", NuSpecialGetDrawPos(&LevHSpecial[17]));
            SarlaccTargetShield[1] = 1;
        } else {
            PlaySfx("ffield", NuSpecialGetDrawPos(&LevHSpecial[17]));
        }
    }
}

static u8 prevOnMoonCar = 0;
static u8 prevOnTractor = 0;
SPLINEPOS fireSplinePos = {0};
void *emperorfighta_netpacket = 0;
f32 fireSpeedScale = 0;
i32 SarlaccTargetShield[2] = {0};
u64 routemask_efloor_off = 0;
SARLACC_NETPACKET *sarlaccb_netpacket = NULL;
static u8 prevOnTaunTaun = 0;
static u8 prevOnTownCar = 0;

void LegoCity_Reset(WORLDINFO *world) {
    GIZMOPICKUP *pickup;
    i32 i;
    prevOnTaunTaun = 0;
    prevOnTractor = 0;
    prevOnMoonCar = 0;
    prevOnTownCar = 0;
    pickup = world->pickup_runtime_sys->pickups;
    for (i = 0; pickup != NULL && i < world->pickup_runtime_sys->pickup_count; i++, pickup++) {
        if ((pickup->flags & GIZMOPICKUP_STATE_COLLECTED) == 0) {
            switch (pickup->activation_group) {
                case 3:
                case 4:
                case 5:
                case 6:
                    pickup->collected = 0;
                    break;
            }
        }
    }
}

void LegoCity_Update(WORLDINFO *world) {
    GIZMOPICKUPRUNTIMESYS_s *pickup_system;
    GIZMOPICKUP *pickup;
    GAMEOBJECT *object;
    i32 index;
    u8 onTractor;
    u8 onTaunTaun;
    u8 onMoonCar;
    u8 onTownCar;
    u8 changedTractor;
    u8 changedTaunTaun;
    u8 changedMoonCar;
    u8 changedTownCar;
    if (world == NULL) {
        return;
    }
    pickup_system = world->pickup_runtime_sys;
    if (pickup_system == NULL) {
        return;
    }
    onTractor = 0;
    onTaunTaun = 0;
    onMoonCar = 0;
    onTownCar = 0;
    changedTractor = 0;
    changedTaunTaun = 0;
    changedMoonCar = 0;
    changedTownCar = 0;
    for (index = 0; index < 8; ++index) {
        object = Player[index];
        if (!(object != NULL && (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && object->dead == 0 &&
              object->player.rider != NULL && object->player.rider->player.character_context == CHARACTER_CONTEXT_BEEN_TAKEN_OVER)) {
            continue;
        }
        if (object->id == (u16)id_TRACTOR) {
            changedTractor = ~prevOnTractor;
            onTractor = 0xff;
        } else if (object->id == (u16)id_TAUNTAUN) {
            changedTaunTaun = ~prevOnTaunTaun;
            onTaunTaun = 0xff;
        } else if (object->id == (u16)id_MOONCAR) {
            changedMoonCar = ~prevOnMoonCar;
            onMoonCar = 0xff;
        } else if (object->id == (u16)dat_007f1a7c) {
            changedTownCar = ~prevOnTownCar;
            onTownCar = 0xff;
        }
    }
    if (onTractor == 0) {
        changedTractor = prevOnTractor;
    }
    if (onTaunTaun == 0) {
        changedTaunTaun = prevOnTaunTaun;
    }
    if (onMoonCar == 0) {
        changedMoonCar = prevOnMoonCar;
    }
    if (onTownCar == 0) {
        changedTownCar = prevOnTownCar;
    }
    prevOnTractor = onTractor;
    prevOnTaunTaun = onTaunTaun;
    prevOnMoonCar = onMoonCar;
    prevOnTownCar = onTownCar;
    if ((changedTractor | changedTaunTaun | changedMoonCar | changedTownCar) == 0) {
        return;
    }
    pickup = pickup_system->pickups;
    for (index = 0; pickup != NULL && index < world->pickup_runtime_sys->pickup_count; ++index, ++pickup) {
        if ((pickup->flags & GIZMOPICKUP_STATE_COLLECTED) == 0) {
            switch (pickup->activation_group) {
                case 3:
                    pickup->collected = onTractor;
                    break;
                case 4:
                    pickup->collected = onTaunTaun;
                    break;
                case 5:
                    pickup->collected = onMoonCar;
                    break;
                case 6:
                    pickup->collected = onTownCar;
                    break;
            }
        }
    }
}

void NewTown_Init(WORLDINFO *world) {
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "dummy_exp8");
}

/* Bonus level vehicle ride flags. */
static u8 prevOnFireTruck = 0;
static u8 prevOnLifeBoat = 0;

void NewTown_Reset(WORLDINFO *world) {
    u32 seed = 17;
    GIZMOPICKUP *pickup;
    i32 i;
    f32 x;
    f32 z;
    prevOnTaunTaun = 0;
    prevOnFireTruck = 0;
    prevOnLifeBoat = 0;
    pickup = world->pickup_runtime_sys->pickups;
    for (i = 0; pickup != NULL && i < world->pickup_runtime_sys->pickup_count; i++, pickup++) {
        if ((pickup->flags & GIZMOPICKUP_STATE_COLLECTED) == 0) {
            switch (pickup->activation_group) {
                case 2:
                case 3:
                case 4:
                    pickup->collected = 0;
                    break;
            }
        }
        if (pickup->activation_group == 4) {
            x = (f32)(NuRandFloatSeeded(&seed) * 4.0 - 7.656 - 1.5);
            z = (f32)(NuRandFloatSeeded(&seed) * 4.0 - 5.871 - 1.5);
            pickup->position.x = x;
            pickup->position.z = z;
        }
    }
}

void NewTown_Update(WORLDINFO *world) {
    GIZMOPICKUPRUNTIMESYS_s *pickup_system;
    GIZMOPICKUP *pickup;
    GAMEOBJECT *object;
    i32 index;
    u8 onTaunTaun;
    u8 onFireTruck;
    u8 onLifeBoat;
    u8 changedTaunTaun;
    u8 changedFireTruck;
    u8 changedLifeBoat;
    if (world == NULL) {
        return;
    }
    pickup_system = world->pickup_runtime_sys;
    if (pickup_system == NULL) {
        return;
    }
    onTaunTaun = 0;
    onFireTruck = 0;
    onLifeBoat = 0;
    changedTaunTaun = 0;
    changedFireTruck = 0;
    changedLifeBoat = 0;
    for (index = 0; index < 8; ++index) {
        object = Player[index];
        if (!(object != NULL && (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && object->dead == 0 &&
              object->player.rider != NULL && object->player.rider->player.character_context == CHARACTER_CONTEXT_BEEN_TAKEN_OVER)) {
            continue;
        }
        if (object->id == (u16)id_TAUNTAUN) {
            changedTaunTaun = ~prevOnTaunTaun;
            onTaunTaun = 0xff;
        } else if (object->id == (u16)id_FIRETRUCK) {
            changedFireTruck = ~prevOnFireTruck;
            onFireTruck = 0xff;
        } else if (object->id == (u16)dat_007f1a88) {
            changedLifeBoat = ~prevOnLifeBoat;
            onLifeBoat = 0xff;
        }
    }
    if (onTaunTaun == 0) {
        changedTaunTaun = prevOnTaunTaun;
    }
    if (onFireTruck == 0) {
        changedFireTruck = prevOnFireTruck;
    }
    if (onLifeBoat == 0) {
        changedLifeBoat = prevOnLifeBoat;
    }
    prevOnTaunTaun = onTaunTaun;
    prevOnFireTruck = onFireTruck;
    prevOnLifeBoat = onLifeBoat;
    if ((changedTaunTaun | changedFireTruck | changedLifeBoat) == 0) {
        return;
    }
    pickup = pickup_system->pickups;
    for (index = 0; pickup != NULL && index < world->pickup_runtime_sys->pickup_count; ++index, ++pickup) {
        if ((pickup->flags & GIZMOPICKUP_STATE_COLLECTED) == 0) {
            switch (pickup->activation_group) {
                case 2:
                    pickup->collected = onTaunTaun;
                    break;
                case 3:
                    pickup->collected = onFireTruck;
                    break;
                case 4:
                    pickup->collected = onLifeBoat;
                    break;
            }
        }
    }
}

void EndorBattleA_Init(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "bbq_popnull", 1);
}

/* Endor barbecue smoke interval and countdown. */
static f32 EndorBattleAPartTime = 0.75f;
static f32 EndorBattleAPartTimer = 0.75f;

void EndorBattleA_Update(WORLDINFO *world) {
    NUINSTANIM *animation = NuSpecialGetInstAnim(&LevHSpecial[0]);
    if (animation != NULL && (animation->flags & NUINSTANIM_FLAG_PLAYING) != 0 && NuSpecialGetVisibilityFn(&LevHSpecial[0]) != 0) {
        if (EndorBattleAPartTimer >= EndorBattleAPartTime) {
            AddPartDebris(WORLD->part_debris_sys, 7, NuSpecialGetDrawPos(&LevHSpecial[0]));
        }
        EndorBattleAPartTimer -= FRAMETIME;
        if (EndorBattleAPartTimer <= 0.0f) {
            EndorBattleAPartTimer = EndorBattleAPartTime;
        }
    }
}

void DeathStar2BattleFire_Init(WORLDINFO *world) {
    VehicleSlowdownCallback = DeathStar2BattleFire_SetSlowDownMul;
    LevelCodeSpline[0] = NuSplineFind(world->current_gscn, "fire");
    NuSpecialFind(WORLD->current_gscn, &LevHSpecial[0], "fire_cube", 0);
    InitSplinePosition(&fireSplinePos, LevelCodeSpline[0], 0.0f, 0);
    InitSplinePosition(&fireBackPos, LevelCodeSpline[0], 0.0f, 0);
    runningTotalPos = 0.0f;
    fireSpeedScale = 1.0f;
}

TIMER LevelTimer = {0};

void DeathStar2BattleFire_Update(WORLDINFO *world) {
    u16 yaw = 0;
    u16 pitch = 0;
    NUVEC point = v000;
    NUMTX matrix;
    NUVEC difference;
    f32 choice;
    f32 nearest_distance = 1000000000.0f;
    GAMEOBJECT *nearest = NULL;
    i32 effect;
    i32 index;
    f32 distance;

    DeathStar2BattleFire_UpdateSlowDownMul(FRAMETIME);
    if (Player[0] != NULL) {
        FireDeltaPosition = (f32)((f64)Player[0]->character_data->game_character->run_speed * 1.25);
    } else {
        FireDeltaPosition = 30.0f;
    }

    choice = (f32)((f64)NuFloatRand(&GAMERAND) * 4.0);
    if (choice < 1.0f) {
        effect = 33;
    } else if (choice < 2.0f) {
        effect = 48;
    } else if (choice < 3.0f) {
        effect = 35;
    } else {
        effect = 36;
    }
    PointAlongSpline(LevelCodeSpline[0], (f32)((f64)NuFloatRand(&GAMERAND) * 0.1f + (f64)fireSplinePos.normalized_position), &point, &yaw,
                     &pitch, 0);
    AddVariableShotDebrisEffectTimed1(world->game_debris_sys->entries[effect].effect, &point, 90, FRAMETIME, (i16)qrand(), (i16)qrand(),
                                      NULL);
    NuMtxSetIdentity(&matrix);
    PointAlongSpline(LevelCodeSpline[0], fireSplinePos.normalized_position, &point, &yaw, &pitch, 0);
    NuMtxPreRotateY(&matrix, yaw);
    NuMtxTranslate(&matrix, &fireSplinePos.position);
    if (fireSplinePos.normalized_position < 1.0f) {
        NuSpecialSetDrawMtx(&LevHSpecial[0], &matrix);
    } else {
        NuSpecialSetVisibility(&LevHSpecial[0], 0);
    }
    if (fireBackPos.normalized_position <= 0.0f) {
        fireBackPos = fireSplinePos;
    }

    for (index = 0; index < 8; ++index) {
        if (Player[index] != NULL && (Player[index]->player.unknown_e20 & 0x20) == 0) {
            NuVecSub(&difference, &Player[index]->position, &fireSplinePos.position);
            NuVecRotateY(&difference, &difference, -yaw);
            if (difference.z < nearest_distance) {
                nearest_distance = difference.z;
                if (difference.z < 0.0f) {
                    nearest = Player[index];
                }
            }
            if (difference.z < 0.0f) {
                i32 no_hurt = objhitobj_nohurtsfx;
                if (Player[index]->dead != 0) {
                    fireSpeedScale = 0.2f;
                }
                AddVariableShotDebrisEffectTimed3(world->game_debris_sys->entries[115].effect, &nearest->position, &nearest->velocity, 40,
                                                  FRAMETIME, NULL, NULL);
                if ((Player[index]->flags_1f8 & 0x40000) == 0) {
                    objhitobj_nohurtsfx = 1;
                    objhitobj_noimpactsfx = 1;
                    ObjHitObj(NULL, Player[index], 1, 0, 0, 1);
                    objhitobj_nohurtsfx = no_hurt;
                }
            }
        }
    }
    if (fireSpeedScale < 1.0f && nearest == NULL) {
        fireSpeedScale = (f32)((f64)fireSpeedScale + (f64)FRAMETIME / 3.0);
    } else if ((f64)fireSpeedScale == (f64)0.2f && nearest != NULL) {
        fireSpeedScale = 0.2f;
    } else if (nearest_distance > 10.0f) {
        fireSpeedScale = 1.75f;
    } else {
        fireSpeedScale = 1.0f;
    }

    if (WORLD->current_level == DEATHSTAR2BATTLEE_LDATA || LevelTimer.time_elapsed > 0.5f) {
        distance = (f32)((f64)FRAMETIME * (f64)FireDeltaPosition * (f64)fireSpeedScale);
        MoveSplinePosition(&fireSplinePos, distance);
    }
    distance = (f32) - ((f64)FRAMETIME * (f64)FireDeltaPosition);
    MoveSplinePosition(&fireBackPos, distance);
    if ((f64)NuFloatRand(&GAMERAND) > (f64)0.95f) {
        switch ((i32)((f64)NuFloatRand(&GAMERAND) * 3.0)) {
            case 0:
                PlaySfx("exp_asteroid", &fireSplinePos.position);
                break;
            case 1:
                PlaySfx("explode_SDest", &fireSplinePos.position);
                break;
            case 2:
                PlaySfx("explode_SDG", &fireSplinePos.position);
                break;
            default:
                PlaySfx("exp_asteroid", &fireSplinePos.position);
                break;
        }
    }
    PlaySfx("env_lantern_lp", &fireSplinePos.position);
}

void FairgroundbadA_Init(WORLDINFO *world) {
    FairgroundSplitPartyLocator = AIPathFindLocator(world->ai_sys, "Split_Party");
}

/* Moves the split-party locator onto the one player standing in the split area; two or more players there
   leave it alone. */
void FairgroundbadA_Update(WORLDINFO *world) {
    i32 count = 0;
    GAMEOBJECT *found = NULL;
    NUVEC *position = NULL;
    GAMEOBJECT *object;
    i32 index;
    if (FairgroundSplitPartyLocator == NULL) {
        return;
    }
    for (index = 0; index < 8; ++index) {
        object = Player[index];
        if (object != NULL && (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) != 0 && object->dead == 0 &&
            (object->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
            position = &object->collision_position;
            if (!(position->x < -57.0 || position->x > -47.0f || position->z < -5.2f || position->z > -4.22f)) {
                if (++count >= 2) {
                    return;
                }
                found = object;
            }
        }
    }
    if (found == NULL) {
        return;
    }
    FairgroundSplitPartyLocator->position = *position;
    if (found->collision_position.y < 1.1f) {
        FairgroundSplitPartyLocator->position.y = 1.5f;
    } else {
        FairgroundSplitPartyLocator->position.y = 0.0f;
    }
    FairgroundSplitPartyLocator->position.y = GameShadow(NULL, &FairgroundSplitPartyLocator->position, 0.2f, -1);
    AISysGetPathPos2(world->ai_sys, &FairgroundSplitPartyLocator->position, &FairgroundSplitPartyLocator->path_info,
                     &FairgroundSplitPartyLocator->position, NULL, 0xff);
}

void FairgroundbadA_Draw(WORLDINFO *world) {
}

#undef EMPEROR_PACKET
