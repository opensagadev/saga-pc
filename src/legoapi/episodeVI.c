#include "saga.h"

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
