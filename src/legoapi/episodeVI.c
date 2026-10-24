#include "saga.h"
#include <stdio.h>

/* Original episodeVI.cpp private owner, PC 0x0088a668, two player timers. */
static f32 slowDownTimer[2] = {0};

f32 __cdecl DeathStar2BattleFire_GetSlowDownMul(GAMEOBJECT *object) {
    i32 player;
    if (object == NULL || (object->field_280 != 0 && object->field_280 != 1))
        return 1.0f;
    player = object->field_280;
    if (slowDownTimer[player] < 0.0f)
        slowDownTimer[player] = 0.0f;
    return (f32)((3.0 - (f64)slowDownTimer[player]) / 3.0);
}

extern i32 DrawBossHitPointsTwoRowsPc;

void __cdecl EmperorFightA_Panel(WORLDINFO *world) {
    DrawBossHitPointsTwoRowsPc = 1;
    if (LevGameObject[0] != NULL && LevAIMessage[0] != NULL && LevAIMessage[0]->value == 1.0f)
        DrawBossHitPoints(LevGameObject[0]);
}

/* Native Emperor floor packet and private controls. */
typedef struct emperor_update_packet_pc_s {
    i32 state;
    u8 enabled, zap, player_floor_mask, reserved;
} EMPEROR_UPDATE_PACKET_PC;
STATIC_ASSERT(sizeof(EMPEROR_UPDATE_PACKET_PC) == 8);
STATIC_ASSERT(offsetof(EMPEROR_UPDATE_PACKET_PC, player_floor_mask) == 6);
void __cdecl EmperorFightA_UpdateFloorObstaclesPc(void) {
    EMPEROR_UPDATE_PACKET_PC *packet = (EMPEROR_UPDATE_PACKET_PC *)emperorfighta_netpacket;
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

void __cdecl JabbasPalaceA_Init(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "grill_02", 1);
}

void __cdecl JabbasPalaceA_Reset(WORLDINFO *world) {
    TerSurface[14].flags = 0xa040;
    TerSurface[9].flags = 0x2040;
}

void __cdecl JabbasPalaceB_Init(WORLDINFO *world) {
    LevGizObst[0] = GizObstacle_FindByName(world->giz_obstacle_sys, "obstacle5");
    LevBlowUp[0] = GizmoBlowUp_FindByName(world, "prison_stone1");
}

void __cdecl JabbasPalaceB_Reset(WORLDINFO *world) {
    TerSurface[14].flags = 0xa040;
    TerSurface[9].flags = 0x2040;
}

void __cdecl JabbasPalaceD_Reset(WORLDINFO *world) {
    TerSurface[14].flags = 0xa040;
    TerSurface[9].flags = 0x2040;
}

void __cdecl JabbasPalaceE_Panel(WORLDINFO *world) {
    if (netclient == 0) {
        if (LevGameObject[0] != NULL && LevAIMessage[0] != NULL && LevAIMessage[0]->value == 1.0f)
            DrawBossHitPoints(LevGameObject[0]);
        else
            DrawBossHitPoints(NULL);
    }
}

void __cdecl SarlaccPitC_Init(WORLDINFO *world) {
    SarlaccPowerPc = 0;
    SarlaccRechargingPc = 0;
    SarlaccTargetShieldPc[0] = 0;
    SarlaccTargetShieldPc[1] = 0;
}

void __cdecl NewTown_Init(WORLDINFO *world) {
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "dummy_exp8");
}

void __cdecl EndorBattleA_Init(WORLDINFO *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "bbq_popnull", 1);
}

/* PC 0x00800358 / 0x0080035c: Endor barbecue smoke interval and countdown. */
static f32 EndorBattleAPartTimePc = 0.75f;
static f32 EndorBattleAPartTimerPc = 0.75f;

void __cdecl EndorBattleA_Update(WORLDINFO *world) {
    NUINSTANIM_TIME_VIEW *animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&LevHSpecial[0]);
    if (animation != NULL && (animation->flags & 1) != 0 && NuSpecialGetVisibilityFn(&LevHSpecial[0]) != 0) {
        if (EndorBattleAPartTimerPc >= EndorBattleAPartTimePc)
            AddPartDebris(WORLD->part_debris_sys, 7, NuSpecialGetDrawPos(&LevHSpecial[0]));
        EndorBattleAPartTimerPc -= FRAMETIME;
        if (EndorBattleAPartTimerPc <= 0.0f)
            EndorBattleAPartTimerPc = EndorBattleAPartTimePc;
    }
}

void __cdecl DeathStar2BattleA_AlwaysUpdate(WORLDINFO *world) {
    if (FreePlay == 0 && LevelFixupSlotPc0087b284 != NULL)
        other_level_override = LevelFixupSlotPc0087b284->index;
}

/* PC 0x0088ab2c: fairground party split locator. */
static AILOCATOR_PC *FairgroundSplitPartyLocatorPc;

void __cdecl FairgroundbadA_Init(WORLDINFO *world) {
    FairgroundSplitPartyLocatorPc = AIPathFindLocator(world->ai_sys, "Split_Party");
}

void __cdecl FairgroundbadA_Draw(WORLDINFO *world) {
}

typedef struct JABBA_ANTINODE_VIEW_PC {
    u8 unknown_00[0x40];
    f32 base_radius;
    f32 base_height;
    u8 unknown_48[2];
    u8 type;
} JABBA_ANTINODE_VIEW_PC;
STATIC_ASSERT(offsetof(JABBA_ANTINODE_VIEW_PC, type) == offsetof(AIANTINODE_PC, type));

void __cdecl JabbasPalaceA_Update(WORLDINFO *world) {
    NUVEC position = {-0.01503f, 0.7414f, 11.5824f};
    JABBA_ANTINODE_VIEW_PC *antinode;
    if (NuSpecialExistsFn(&LevHSpecial[0]) != 0 && NuSpecialGetVisibilityFn(&LevHSpecial[0]) == 0) {
        antinode = (JABBA_ANTINODE_VIEW_PC *)AIAntinodeCreateSingleFrame(&position, 1.0f);
        if (antinode != NULL) {
            antinode->type = 2;
            antinode->base_radius = 0.82443f;
            antinode->base_height = 1.0f;
        }
    }
}

void __cdecl JabbasPalaceE_Init(WORLDINFO *world) {
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "prox_explo1");
    LevGizmo[1] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "prox_explo2");
    ((u8 *)LevFlag)[6] = 0;
    ((u8 *)LevFlag)[7] = 0;
}

void __cdecl SarlaccPitA_Reset(WORLDINFO *world) {
    LevSafePlatID[0] = -1;
    if (NuSpecialFind(world->current_gscn, &LevHSpecial[0], "float_skiff_2", 1) != 0 && world->terrain != NULL)
        LevSafePlatID[0] = FindPlatInst(NuSpecialGetInstanceix(&LevHSpecial[0]));
}

typedef struct LEGOCITY_PICKUP_PC {
    u8 unknown_00[8];
    NUVEC position;
    u8 unknown_14[3];
    u8 flags;
    u8 type_id;
    u8 unknown_19[0x24 - 0x19];
    u8 collected;
    u8 unknown_25[0x2c - 0x25];
} LEGOCITY_PICKUP_PC;
STATIC_ASSERT(sizeof(LEGOCITY_PICKUP_PC) == 0x2c);

typedef struct LEGOCITY_PICKUPSYS_PC {
    LEGOCITY_PICKUP_PC *pickups;
    u8 unknown_04[4];
    i32 count;
} LEGOCITY_PICKUPSYS_PC;

/* PC 0x0088a58c..0x0088a58f and 0x0088a6ec/0x0088a6ed: bonus level vehicle ride flags. */
static u8 prevOnFireTruck;
static u8 prevOnLifeBoat;
static u8 prevOnMoonCar;
static u8 prevOnTractor;
static u8 prevOnTaunTaun;
static u8 prevOnTownCar;

void __cdecl LegoCity_Reset(WORLDINFO *world) {
    LEGOCITY_PICKUP_PC *pickup;
    i32 i;
    prevOnTaunTaun = 0;
    prevOnTractor = 0;
    prevOnMoonCar = 0;
    prevOnTownCar = 0;
    pickup = ((LEGOCITY_PICKUPSYS_PC *)world->pickup_runtime_sys)->pickups;
    for (i = 0; pickup != NULL && i < ((LEGOCITY_PICKUPSYS_PC *)world->pickup_runtime_sys)->count; i++, pickup++) {
        if ((pickup->flags & 8) == 0) {
            switch (pickup->type_id) {
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

void __cdecl NewTown_Reset(WORLDINFO *world) {
    u32 seed = 17;
    LEGOCITY_PICKUP_PC *pickup;
    i32 i;
    f32 x;
    f32 z;
    prevOnTaunTaun = 0;
    prevOnFireTruck = 0;
    prevOnLifeBoat = 0;
    pickup = ((LEGOCITY_PICKUPSYS_PC *)world->pickup_runtime_sys)->pickups;
    for (i = 0; pickup != NULL && i < ((LEGOCITY_PICKUPSYS_PC *)world->pickup_runtime_sys)->count; i++, pickup++) {
        if ((pickup->flags & 8) == 0) {
            switch (pickup->type_id) {
                case 2:
                case 3:
                case 4:
                    pickup->collected = 0;
                    break;
            }
        }
        if (pickup->type_id == 4) {
            x = (f32)(NuRandFloatSeeded(&seed) * 4.0 - 7.656 - 1.5);
            z = (f32)(NuRandFloatSeeded(&seed) * 4.0 - 5.871 - 1.5);
            pickup->position.x = x;
            pickup->position.z = z;
        }
    }
}

void __cdecl DeathStar2BattleFire_SetSlowDownMul(GAMEOBJECT *object) {
    i8 index;
    if (WORLD->current_level == DEATHSTAR2BATTLEE_LDATA || WORLD->current_level == DEATHSTAR2BATTLEF_LDATA ||
        WORLD->current_level == DEATHSTAR2BATTLEG_LDATA) {
        if (object != NULL) {
            index = (i8)object->field_280;
            if (index == 0 || index == 1) {
                object->velocity = v000;
                slowDownTimer[index] = 3.0f;
            }
        }
    }
}

void __cdecl DeathStar2BattleFire_Init(WORLDINFO *world) {
    VehicleSlowdownCallbackPc = DeathStar2BattleFire_SetSlowDownMul;
    LevelCodeSpline[0] = NuSplineFind(world->current_gscn, "fire");
    NuSpecialFind(WORLD->current_gscn, &LevHSpecial[0], "fire_cube", 0);
    InitSplinePosition(&fireSplinePos, LevelCodeSpline[0], 0.0f, 0);
    InitSplinePosition(&fireBackPos, LevelCodeSpline[0], 0.0f, 0);
    runningTotalPos = 0.0f;
    fireSpeedScale = 1.0f;
}

void __cdecl DeathStar2BattleD_Init(WORLDINFO *world) {
    char name[256];
    i32 i;
    GIZMO_PC *obstacle;
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "reactor1");
    for (i = 1; i < 7; i++) {
        sprintf(name, "lecnode_%i1", i);
        LevGizmo[i] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, name);
        sprintf(name, "obstacle%d", i);
        obstacle = (GIZMO_PC *)GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, name);
        if (obstacle != NULL && obstacle->object != NULL)
            LevGizObst[i] = (GIZOBSTACLE *)obstacle->object;
    }
    LevGizmo[7] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "shield_inner1");
    ((u8 *)LevFlag)[5] = 0;
}
