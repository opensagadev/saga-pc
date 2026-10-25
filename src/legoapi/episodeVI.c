#include "saga.h"
#include <stdio.h>

typedef struct SARLACC_DISCO_PC {
    AIAREA_PC *area;
    NUSPECIALHANDLE off[16], flash[16], select[16], on[16], finish[16];
    GIZOBSTACLE *off_obstacle, *on_obstacle;
    u8 panel_state[16];
    i8 count, phase, first_panel, second_panel;
    f32 timer, initial_height, height;
    u8 reserved_3ec[4];
    GIZAIMESSAGE *help_message, *complete_message, *state_message;
    NUVEC *last_selected_position;
} SARLACC_DISCO_PC;
STATIC_ASSERT(sizeof(SARLACC_DISCO_PC) == 0x400);
STATIC_ASSERT(offsetof(SARLACC_DISCO_PC, off) == 4);
STATIC_ASSERT(offsetof(SARLACC_DISCO_PC, flash) == 0xc4);
STATIC_ASSERT(offsetof(SARLACC_DISCO_PC, select) == 0x184);
STATIC_ASSERT(offsetof(SARLACC_DISCO_PC, on) == 0x244);
STATIC_ASSERT(offsetof(SARLACC_DISCO_PC, finish) == 0x304);
STATIC_ASSERT(offsetof(SARLACC_DISCO_PC, panel_state) == 0x3cc);
STATIC_ASSERT(offsetof(SARLACC_DISCO_PC, count) == 0x3dc);
STATIC_ASSERT(offsetof(SARLACC_DISCO_PC, timer) == 0x3e0);
STATIC_ASSERT(offsetof(SARLACC_DISCO_PC, initial_height) == 0x3e4);
STATIC_ASSERT(offsetof(SARLACC_DISCO_PC, height) == 0x3e8);
STATIC_ASSERT(offsetof(SARLACC_DISCO_PC, help_message) == 0x3f0);
STATIC_ASSERT(offsetof(SARLACC_DISCO_PC, complete_message) == 0x3f4);
STATIC_ASSERT(offsetof(SARLACC_DISCO_PC, state_message) == 0x3f8);
typedef struct SARLACC_PACKET_VIEW_PC {
    f32 height;
    i16 off_mask, flash_mask, select_mask, on_mask, finish_mask, sound_mask;
    u8 disco_active, reserved_11;
    i16 completion_sound_played;
} SARLACC_PACKET_VIEW_PC;
STATIC_ASSERT(sizeof(SARLACC_PACKET_VIEW_PC) == 20);
STATIC_ASSERT(offsetof(SARLACC_PACKET_VIEW_PC, off_mask) == 4);
STATIC_ASSERT(offsetof(SARLACC_PACKET_VIEW_PC, flash_mask) == 6);
STATIC_ASSERT(offsetof(SARLACC_PACKET_VIEW_PC, select_mask) == 8);
STATIC_ASSERT(offsetof(SARLACC_PACKET_VIEW_PC, on_mask) == 0xa);
STATIC_ASSERT(offsetof(SARLACC_PACKET_VIEW_PC, finish_mask) == 0xc);
STATIC_ASSERT(offsetof(SARLACC_PACKET_VIEW_PC, sound_mask) == 0xe);
STATIC_ASSERT(offsetof(SARLACC_PACKET_VIEW_PC, disco_active) == 0x10);
STATIC_ASSERT(offsetof(SARLACC_PACKET_VIEW_PC, completion_sound_played) == 0x12);
#define SARLACC_PACKET ((SARLACC_PACKET_VIEW_PC *)sarlaccb_netpacket)
f32 SarlaccDiscoBaseOffsetPc = -0.12f;

static SARLACC_DISCO_PC sarlaccdisco = {0};
NUSPECIALHANDLE SarlaccLevSpecialPc[7] = {0};
void *SarlaccLevelBuilditsPc[2] = {0};
GIZMO_PC *SarlaccObstMirrorBallPc = NULL;
GIZMO_PC *SarlaccForceMirrorBallPc = NULL;

f32 DeathStarZapSizeWidthPc = 1.0f;
f32 DeathStarZapSizeLengthPc = 1.0f;
f32 DeathStarZapWidthBiasPc = 0.01f;
f32 DeathStarZapEndWidthPc = 1.5f;
i32 DeathStarZapLaserTypePc = 0;

/* Original episodeVI.cpp private owner, PC 0x0088a668, two player timers. */
static f32 slowDownTimer[2] = {0};
f32 FireDeltaPositionPc = 0.0f;

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

void __cdecl KillBossNewLevel(i32 id, i32 unused_4, f32 delay, i32 level_index);
STATIC_ASSERT(offsetof(GAMEOBJECT, hostility_group) == 0x28d);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_opponent_exclusion_mask) == 0xeb8);
#define RANCOR_PLAYER_IN_AREA_PC(player, world, n)                                                                     \
    (U64_PAIR((player)->ai_area_mask) &                                                                                \
     (i64)(i32)(1u << (((i32)((u32)LevArea[n] - (u32)(world)->ai_sys->areas) >> 6) & 31)))

void __cdecl JabbasPalaceE_Update(WORLDINFO *world) {
    i32 i;
    GAMEOBJECT *player;
    GIZMOBLOWUP_PC *blowup;
    u8 *flags = (u8 *)LevFlag;
    if (!netclient) {
        if (FreePlay)
            KillBossCompleteLevel(id_RANCOR, 0, 0.3f);
        else
            KillBossNewLevel(id_RANCOR, 0, 0.3f, LevelFixupSlotPc0087b2b0->index);
    }
    if (LevGameObject[0] != NULL) {
        LevGameObject[0]->ai_opponent_exclusion_mask[0] = 0;
        LevGameObject[0]->ai_opponent_exclusion_mask[1] = 0;
        if (aicreature_sets_alive[0] != 0) {
            for (i = 0; i < 8; ++i) {
                player = (GAMEOBJECT *)Player[i];
                if (player != NULL)
                    U64_PAIR(LevGameObject[0]->ai_opponent_exclusion_mask) |= (u64)1 << player->hostility_group;
            }
        } else {
            for (i = 0; i < 8; ++i) {
                player = (GAMEOBJECT *)Player[i];
                if (player != NULL && ((LevArea[0] != NULL && RANCOR_PLAYER_IN_AREA_PC(player, world, 0)) ||
                                       (LevArea[1] != NULL && RANCOR_PLAYER_IN_AREA_PC(player, world, 1)) ||
                                       (LevArea[2] != NULL && RANCOR_PLAYER_IN_AREA_PC(player, world, 2))))
                    U64_PAIR(LevGameObject[0]->ai_opponent_exclusion_mask) |= (u64)1 << player->hostility_group;
            }
        }
    }
    blowup = LevGizmo[0] != NULL ? (GIZMOBLOWUP_PC *)LevGizmo[0]->object : NULL;
    if (blowup != NULL && (blowup->status_flags & 1) != 0) {
        if (!flags[6]) {
            PlaySfx("exp_minecart", &blowup->position);
            flags[6] = 1;
        }
    } else {
        flags[6] = 0;
    }
    blowup = LevGizmo[1] != NULL ? (GIZMOBLOWUP_PC *)LevGizmo[1]->object : NULL;
    if (blowup != NULL && (blowup->status_flags & 1) != 0) {
        if (!flags[7]) {
            PlaySfx("exp_minecart", &blowup->position);
            flags[7] = 1;
        }
    } else {
        flags[7] = 0;
    }
}
#undef RANCOR_PLAYER_IN_AREA_PC

void __cdecl JabbasPalaceE_Panel(WORLDINFO *world) {
    if (netclient == 0) {
        if (LevGameObject[0] != NULL && LevAIMessage[0] != NULL && LevAIMessage[0]->value == 1.0f)
            DrawBossHitPoints(LevGameObject[0]);
        else
            DrawBossHitPoints(NULL);
    }
}

void __cdecl SarlaccPitB_SpecialUpdate(WORLDINFO *world) {
    i32 index;
    u32 bit;
    NUSPECIALHANDLE *shown;
    NUVEC *position;
    NUINSTANIM_TIME_VIEW *animation;
    (void)world;
    for (index = 0; index < sarlaccdisco.count; ++index) {
        NuSpecialSetVisibility(&sarlaccdisco.off[index], 0);
        NuSpecialSetVisibility(&sarlaccdisco.flash[index], 0);
        NuSpecialSetVisibility(&sarlaccdisco.select[index], 0);
        NuSpecialSetVisibility(&sarlaccdisco.on[index], 0);
        NuSpecialSetVisibility(&sarlaccdisco.finish[index], 0);
        if (netclient)
            sarlaccdisco.height = SeekValF(sarlaccdisco.height, SARLACC_PACKET->height, 8.0f);
        else
            SARLACC_PACKET->height = sarlaccdisco.height;
        bit = 1u << index;
        shown = NULL;
        if (SARLACC_PACKET->off_mask & bit)
            shown = &sarlaccdisco.off[index];
        else if (SARLACC_PACKET->flash_mask & bit)
            shown = &sarlaccdisco.flash[index];
        else if (SARLACC_PACKET->select_mask & bit)
            shown = &sarlaccdisco.select[index];
        else if (SARLACC_PACKET->on_mask & bit)
            shown = &sarlaccdisco.on[index];
        else if (SARLACC_PACKET->finish_mask & bit)
            shown = &sarlaccdisco.finish[index];
        if (shown != NULL) {
            NuSpecialSetVisibility(shown, 1);
            position = NuSpecialGetDrawPos(shown);
            if (position != NULL) {
                position->y = sarlaccdisco.height;
                NuSpecialSetDrawPos(shown, position);
            }
        }
        position = NuSpecialGetDrawPos(&LevHSpecial[1]);
        if (position != NULL) {
            position->y = SarlaccDiscoBaseOffsetPc + sarlaccdisco.height;
            NuSpecialSetDrawPos(&LevHSpecial[1], position);
        }
        if (SARLACC_PACKET->sound_mask & bit)
            PlaySfx("Kam_DiscoFloorPanelOn", &NuSpecialGetDrawMtx(&sarlaccdisco.on[index])->translation);
    }
    if (SARLACC_PACKET->disco_active != 0) {
        SetGizAIMessage(gizaimessagesys, "DiscoComplete", 1.0f, sarlaccdisco.complete_message);
        PlayRadio("Speaker2", "Speaker21", 1);
        PlayRadio("Speaker1", "Speaker11", 1);
        PlayRadio("decks", "decks", 1);
        if (SarlaccObstMirrorBallPc != NULL)
            GizObstacle_PlayForwards((GIZOBSTACLE *)SarlaccObstMirrorBallPc->object);
        if (NuSpecialExistsFn(&SarlaccLevSpecialPc[0]))
            NuSpecialSetVisibility(&SarlaccLevSpecialPc[0], 1);
        if (NuSpecialExistsFn(&SarlaccLevSpecialPc[1]))
            NuSpecialSetVisibility(&SarlaccLevSpecialPc[1], 1);
        if (NuSpecialExistsFn(&SarlaccLevSpecialPc[4])) {
            animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&SarlaccLevSpecialPc[4]);
            if (animation != NULL)
                animation->flags |= 1;
        }
        if (NuSpecialExistsFn(&SarlaccLevSpecialPc[5]) && NuSpecialExistsFn(&SarlaccLevSpecialPc[6])) {
            animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&SarlaccLevSpecialPc[5]);
            if (animation != NULL)
                animation->flags |= 1;
            animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&SarlaccLevSpecialPc[6]);
            if (animation != NULL)
                animation->flags |= 1;
        }
        if (SARLACC_PACKET->completion_sound_played == 0) {
            PlaySfx("Kam_DiscoFloorPanelDone", NuSpecialGetDrawPos(&sarlaccdisco.off[0]));
            SARLACC_PACKET->completion_sound_played = 1;
        }
    } else {
        PlayRadio("Speaker2", "Speaker21", 0);
        PlayRadio("Speaker1", "Speaker11", 0);
        PlayRadio("decks", "decks", 0);
        SARLACC_PACKET->completion_sound_played = 0;
    }
}

void __cdecl SarlaccPitB_Init(WORLDINFO *world) {
    char name[32];
    i8 *index;
    NUVEC *position;
    (void)world;
    memset(&sarlaccdisco, 0, sizeof(sarlaccdisco));
    sarlaccb_netpacket = (SARLACC_BATTLE_MUSIC_VIEW_PC *)SetLevelHack(20);
    index = &sarlaccdisco.count;
    *index = 0;
    for (;;) {
        if (*index < 9)
            sprintf(name, "dot_off_0%d", *index + 1);
        else
            sprintf(name, "dot_off_%d", *index + 1);
        NuSpecialFind(WORLD->current_gscn, &sarlaccdisco.off[*index], name, 1);
        if (*index < 9)
            sprintf(name, "dot_flash_0%d", *index + 1);
        else
            sprintf(name, "dot_flash_%d", *index + 1);
        NuSpecialFind(WORLD->current_gscn, &sarlaccdisco.flash[*index], name, 1);
        if (*index < 9)
            sprintf(name, "dot_select_0%d", *index + 1);
        else
            sprintf(name, "dot_select_%d", *index + 1);
        NuSpecialFind(WORLD->current_gscn, &sarlaccdisco.select[*index], name, 1);
        if (*index < 9)
            sprintf(name, "dot_on_0%d", *index + 1);
        else
            sprintf(name, "dot_on_%d", *index + 1);
        NuSpecialFind(WORLD->current_gscn, &sarlaccdisco.on[*index], name, 1);
        if (*index < 9)
            sprintf(name, "dot_finish_0%d", *index + 1);
        else
            sprintf(name, "dot_finish_%d", *index + 1);
        NuSpecialFind(WORLD->current_gscn, &sarlaccdisco.finish[*index], name, 1);
        if (!NuSpecialExistsFn(&sarlaccdisco.off[*index]) || !NuSpecialExistsFn(&sarlaccdisco.flash[*index]) ||
            !NuSpecialExistsFn(&sarlaccdisco.select[*index]) || !NuSpecialExistsFn(&sarlaccdisco.on[*index]) ||
            !NuSpecialExistsFn(&sarlaccdisco.finish[*index]))
            break;
        if (*index == 0) {
            position = NuSpecialGetPos(&sarlaccdisco.off[0]);
            if (position != NULL) {
                sarlaccdisco.initial_height = position->y;
                sarlaccdisco.height = position->y;
            }
        }
        ++*index;
        if (*index >= 16)
            break;
    }
    sarlaccdisco.area = AISysFindArea(WORLD->ai_sys, "DISCO");
    NuSpecialFind(WORLD->current_gscn, &LevHSpecial[0], "force_engine_lump", 1);
    NuSpecialFind(WORLD->current_gscn, &LevHSpecial[1], "disco_base", 1);
}

void __cdecl SarlaccPitB_Reset(WORLDINFO *world) {
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
    NuSpecialFind(world->current_gscn, &SarlaccLevSpecialPc[0], "floor_disco", 1);
    NuSpecialFind(world->current_gscn, &SarlaccLevSpecialPc[1], "light1_a", 1);
    NuSpecialFind(world->current_gscn, &SarlaccLevSpecialPc[2], "disco_ball1", 1);
    NuSpecialFind(world->current_gscn, &SarlaccLevSpecialPc[3], "disco_ball2", 1);
    NuSpecialFind(world->current_gscn, &SarlaccLevSpecialPc[4], "shutter_1", 1);
    NuSpecialFind(world->current_gscn, &SarlaccLevSpecialPc[5], "jabba_door1", 1);
    NuSpecialFind(world->current_gscn, &SarlaccLevSpecialPc[6], "jabba_door2", 1);
    SarlaccForceMirrorBallPc = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, "force5");
    SarlaccObstMirrorBallPc = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, "obstacle5");
    SarlaccLevelBuilditsPc[0] = GizBuildIt_Find(world, "buildit6");
    SarlaccLevelBuilditsPc[1] = GizBuildIt_Find(world, "buildit4");
    LevelLocator = AIPathFindLocator(world->ai_sys, "DiscoHelp");
    if (NuSpecialExistsFn(&SarlaccLevSpecialPc[3]))
        NuSpecialSetVisibility(&SarlaccLevSpecialPc[3], 0);
    if (NuSpecialExistsFn(&SarlaccLevSpecialPc[0]))
        NuSpecialSetVisibility(&SarlaccLevSpecialPc[0], 0);
    if (NuSpecialExistsFn(&SarlaccLevSpecialPc[1]))
        NuSpecialSetVisibility(&SarlaccLevSpecialPc[1], 0);
    if (GizForce_Complete((GIZFORCE_PC *)SarlaccForceMirrorBallPc->object))
        GizObstacle_Stop((GIZOBSTACLE *)SarlaccObstMirrorBallPc->object);
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
static u8 prevOnFireTruck = 0;
static u8 prevOnLifeBoat = 0;
static u8 prevOnMoonCar = 0;
static u8 prevOnTractor = 0;
static u8 prevOnTaunTaun = 0;
static u8 prevOnTownCar = 0;

extern i16 CharacterFixupIdPc007f1a7c;
extern i16 CharacterFixupIdPc007f1a88;

void LegoCity_Update(WORLDINFO *world) {
    LEGOCITY_PICKUPSYS_PC *pickup_system;
    LEGOCITY_PICKUP_PC *pickup;
    GAMEOBJECT *object;
    i32 index;
    u8 onTractor = 0;
    u8 onTaunTaun = 0;
    u8 onMoonCar = 0;
    u8 onTownCar = 0;
    u8 changedTractor = 0;
    u8 changedTaunTaun = 0;
    u8 changedMoonCar = 0;
    u8 changedTownCar = 0;
    if (world == NULL)
        return;
    pickup_system = (LEGOCITY_PICKUPSYS_PC *)world->pickup_runtime_sys;
    if (pickup_system == NULL)
        return;
    for (index = 0; index < 8; ++index) {
        object = (GAMEOBJECT *)Player[index];
        if (!(object != NULL && (object->flags_1fc_word & 0x1000) != 0 && object->dead == 0 && object->rider != NULL &&
              object->rider->character_context == 0x3b)) {
            continue;
        }
        if (object->id == (u16)id_TRACTOR) {
            onTractor = 0xff;
            changedTractor = ~prevOnTractor;
        } else if (object->id == (u16)id_TAUNTAUN) {
            onTaunTaun = 0xff;
            changedTaunTaun = ~prevOnTaunTaun;
        } else if (object->id == (u16)id_MOONCAR) {
            onMoonCar = 0xff;
            changedMoonCar = ~prevOnMoonCar;
        } else if (object->id == (u16)CharacterFixupIdPc007f1a7c) {
            onTownCar = 0xff;
            changedTownCar = ~prevOnTownCar;
        }
    }
    if (onTractor == 0)
        changedTractor = prevOnTractor;
    if (onTaunTaun == 0)
        changedTaunTaun = prevOnTaunTaun;
    if (onMoonCar == 0)
        changedMoonCar = prevOnMoonCar;
    if (onTownCar == 0)
        changedTownCar = prevOnTownCar;
    prevOnTractor = onTractor;
    prevOnTaunTaun = onTaunTaun;
    prevOnMoonCar = onMoonCar;
    prevOnTownCar = onTownCar;
    if ((changedTractor | changedTaunTaun | changedMoonCar | changedTownCar) == 0) {
        return;
    }
    pickup = pickup_system->pickups;
    for (index = 0; pickup != NULL && index < ((LEGOCITY_PICKUPSYS_PC *)world->pickup_runtime_sys)->count;
         ++index, ++pickup) {
        if ((pickup->flags & 8) == 0) {
            switch (pickup->type_id) {
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

void NewTown_Update(WORLDINFO *world) {
    LEGOCITY_PICKUPSYS_PC *pickup_system;
    LEGOCITY_PICKUP_PC *pickup;
    GAMEOBJECT *object;
    i32 index;
    u8 onTaunTaun = 0;
    u8 onFireTruck = 0;
    u8 onLifeBoat = 0;
    u8 changedTaunTaun = 0;
    u8 changedFireTruck = 0;
    u8 changedLifeBoat = 0;
    if (world == NULL)
        return;
    pickup_system = (LEGOCITY_PICKUPSYS_PC *)world->pickup_runtime_sys;
    if (pickup_system == NULL)
        return;
    for (index = 0; index < 8; ++index) {
        object = (GAMEOBJECT *)Player[index];
        if (!(object != NULL && (object->flags_1fc_word & 0x1000) != 0 && object->dead == 0 && object->rider != NULL &&
              object->rider->character_context == 0x3b)) {
            continue;
        }
        if (object->id == (u16)id_TAUNTAUN) {
            onTaunTaun = 0xff;
            changedTaunTaun = ~prevOnTaunTaun;
        } else if (object->id == (u16)id_FIRETRUCK) {
            onFireTruck = 0xff;
            changedFireTruck = ~prevOnFireTruck;
        } else if (object->id == (u16)CharacterFixupIdPc007f1a88) {
            onLifeBoat = 0xff;
            changedLifeBoat = ~prevOnLifeBoat;
        }
    }
    if (onTaunTaun == 0)
        changedTaunTaun = prevOnTaunTaun;
    if (onFireTruck == 0)
        changedFireTruck = prevOnFireTruck;
    if (onLifeBoat == 0)
        changedLifeBoat = prevOnLifeBoat;
    prevOnTaunTaun = onTaunTaun;
    prevOnFireTruck = onFireTruck;
    prevOnLifeBoat = onLifeBoat;
    if ((changedTaunTaun | changedFireTruck | changedLifeBoat) == 0) {
        return;
    }
    pickup = pickup_system->pickups;
    for (index = 0; pickup != NULL && index < ((LEGOCITY_PICKUPSYS_PC *)world->pickup_runtime_sys)->count;
         ++index, ++pickup) {
        if ((pickup->flags & 8) == 0) {
            switch (pickup->type_id) {
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

static void DeathStar2BattleFire_UpdateSlowDownMul(f32 dt) {
    i32 index;
    for (index = 0; index < 2; ++index) {
        if (slowDownTimer[index] > 0.0f)
            slowDownTimer[index] -= dt;
    }
}

void __cdecl DeathStar2BattleFire_Update(WORLDINFO *world) {
    u16 yaw = 0;
    u16 pitch = 0;
    NUVEC point = v000;
    NUMTX matrix;
    NUVEC difference;
    f32 choice;
    f32 along;
    f32 nearest_distance = 1000000000.0f;
    GAMEOBJECT *nearest = NULL;
    i32 effect;
    i32 index;
    i16 y_rotation;
    i16 z_rotation;
    f32 distance;

    DeathStar2BattleFire_UpdateSlowDownMul(FRAMETIME);
    if (Player[0] != NULL)
        FireDeltaPositionPc = (f32)((f64)((GAMEOBJECT *)Player[0])->character_data->game_character->run_speed * 1.25);
    else
        FireDeltaPositionPc = 30.0f;

    choice = (f32)((f64)NuFloatRand((NURAND *)&GAMERAND) * 4.0);
    if (choice < 1.0f)
        effect = 33;
    else if (choice < 2.0f)
        effect = 48;
    else if (choice < 3.0f)
        effect = 35;
    else
        effect = 36;
    along = (f32)((f64)NuFloatRand((NURAND *)&GAMERAND) * 0.1f + (f64)fireSplinePos.normalized_position);
    PointAlongSpline(LevelCodeSpline[0], along, &point, &yaw, &pitch, 0);
    y_rotation = (i16)qrand();
    z_rotation = (i16)qrand();
    AddVariableShotDebrisEffectTimed1(world->game_debris_sys->entries[effect].effect, &point, 90, FRAMETIME, z_rotation,
                                      y_rotation, NULL);
    NuMtxSetIdentity(&matrix);
    PointAlongSpline(LevelCodeSpline[0], fireSplinePos.normalized_position, &point, &yaw, &pitch, 0);
    NuMtxPreRotateY(&matrix, yaw);
    NuMtxTranslate(&matrix, &fireSplinePos.position);
    if (fireSplinePos.normalized_position < 1.0f)
        NuSpecialSetDrawMtx(&LevHSpecial[0], &matrix);
    else
        NuSpecialSetVisibility(&LevHSpecial[0], 0);
    if (fireBackPos.normalized_position <= 0.0f)
        fireBackPos = fireSplinePos;

    for (index = 0; index < 8; ++index) {
        if (Player[index] != NULL && (((GAMEOBJECT *)Player[index])->field_e20 & 0x20) == 0) {
            NuVecSub(&difference, &((GAMEOBJECT *)Player[index])->position, &fireSplinePos.position);
            NuVecRotateY(&difference, &difference, -(i32)yaw);
            if (difference.z < nearest_distance) {
                nearest_distance = difference.z;
                if (difference.z < 0.0f)
                    nearest = (GAMEOBJECT *)Player[index];
            }
            if (difference.z < 0.0f) {
                i32 no_hurt = objhitobj_nohurtsfx;
                if (((GAMEOBJECT *)Player[index])->dead != 0)
                    fireSpeedScale = 0.2f;
                AddVariableShotDebrisEffectTimed3(world->game_debris_sys->entries[115].effect, &nearest->position,
                                                  &nearest->velocity, 40, FRAMETIME, NULL, NULL);
                if ((((GAMEOBJECT *)Player[index])->flags_1f8 & 0x40000) == 0) {
                    objhitobj_nohurtsfx = 1;
                    objhitobj_noimpactsfx = 1;
                    ObjHitObj(NULL, (GAMEOBJECT *)Player[index], 1, 0, 0, 1);
                    objhitobj_nohurtsfx = no_hurt;
                }
            }
        }
    }
    if (fireSpeedScale < 1.0f && nearest == NULL)
        fireSpeedScale = (f32)((f64)fireSpeedScale + (f64)FRAMETIME / 3.0);
    else if ((f64)fireSpeedScale == (f64)0.2f && nearest != NULL)
        fireSpeedScale = 0.2f;
    else if (nearest_distance > 10.0f)
        fireSpeedScale = 1.75f;
    else
        fireSpeedScale = 1.0f;

    if (WORLD->current_level == DEATHSTAR2BATTLEE_LDATA || LevelTimer.time_elapsed > 0.5f) {
        distance = (f32)((f64)FRAMETIME * (f64)FireDeltaPositionPc * (f64)fireSpeedScale);
        MoveSplinePosition(&fireSplinePos, distance);
    }
    distance = (f32) - ((f64)FRAMETIME * (f64)FireDeltaPositionPc);
    MoveSplinePosition(&fireBackPos, distance);
    if ((f64)NuFloatRand((NURAND *)&GAMERAND) > (f64)0.95f) {
        switch ((i32)((f64)NuFloatRand((NURAND *)&GAMERAND) * 3.0)) {
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

GIZMOBLOWUP_PC *__cdecl DeathStar2BattleD_InZapRange(GAMEOBJECT *object) {
    GIZMOBLOWUP_PC *nearest = NULL;
    f32 nearest_distance;
    i32 i;
    if (object == NULL || !(object->flags_1fc_word & 0x80) || object->dead)
        return NULL;
    nearest_distance = 225.0f;
    for (i = 1; i < 7; ++i) {
        GIZMO_PC *gizmo = LevGizmo[i];
        GIZMOBLOWUP_PC *blowup;
        f32 distance;
        if (gizmo == NULL || gizmo->object == NULL)
            continue;
        blowup = (GIZMOBLOWUP_PC *)gizmo->object;
        if ((blowup->status_flags & 0x800000) && !(blowup->status_flags & 1) && LevGizObst[i] != NULL &&
            LevGizObst[i]->anim_set->state != 0) {
            distance = NuVecDistSqr(&object->collision_position, &blowup->mid_position, NULL);
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest = blowup;
            }
        }
    }
    return nearest;
}

void __cdecl DeathStar2BattleD_Update(WORLDINFO *world) {
    GIZMO_PC *shield_gizmo;
    GIZMOBLOWUP_PC *shield, *blowup;
    GAMEOBJECT *player;
    NUVEC shield_position, position, direction, colour, delta;
    NUVEC *mid;
    f32 radius, length;
    i32 angle, i;
    u8 *flags = (u8 *)LevFlag;
    (void)world;
    if (flags[0] != 0 && qrand() < 0x800)
        NewRumbleAllPlayers((f32)((f64)qrand() * (f64)0.000015259021893143654f), 0.0f, 0, 0);
    shield_gizmo = LevGizmo[7];
    if (shield_gizmo != NULL) {
        shield = (GIZMOBLOWUP_PC *)shield_gizmo->object;
        if (shield != NULL && (shield->status_flags & 1)) {
            if ((i32)AreaTimer.time_elapsed % 3 == 0 && (i32)AreaTimer.last_time_elapsed % 3 != 0 &&
                NuFloatRand((NURAND *)&GAMERAND) < 0.5f) {
                angle = qrand();
                shield_position = shield->mid_position;
                direction.x = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
                direction.y = 0.0f;
                direction.z = NuTrigTable[(angle >> 1) & 0x7fff];
                AddPickups(0, 0, 1, 0, &shield_position, &direction, 3.0f, -1,
                           (f32)((f64)NuFloatRand((NURAND *)&GAMERAND) * 15.0 + 10.0), 2000000.0f, NULL, 1, 0);
            }
            return;
        }
    }
    for (i = 0; i < 8; ++i) {
        player = (GAMEOBJECT *)Player[i];
        if (player == NULL || player->dynamic_light_id == -1)
            continue;
        blowup = DeathStar2BattleD_InZapRange(player);
        if (blowup == NULL)
            continue;
        rtlDynamicEnable(((GAMEOBJECT *)Player[i])->dynamic_light_id, 1);
        mid = &blowup->mid_position;
        radius = (f64)NuVecDist(mid, &((GAMEOBJECT *)Player[i])->collision_position, &delta) * 0.5;
        rtlDynamicSetRadii(((GAMEOBJECT *)Player[i])->dynamic_light_id, radius, (f32)((f64)radius + 5.0));
        qrand();
        colour.x = 0.0f;
        if (qrand() < 0x8000)
            colour.y = 2.0f;
        else
            colour.y = 0.25f;
        colour.z = colour.y;
        rtlDynamicSetColours(((GAMEOBJECT *)Player[i])->dynamic_light_id, &colour, NULL);
        player = (GAMEOBJECT *)Player[i];
        position.x = player->collision_position.x + (f64)delta.x * 0.5;
        position.y = player->collision_position.y + (f64)delta.y * 0.5;
        position.z = player->collision_position.z + (f64)delta.z * 0.5;
        rtlDynamicSetPos(player->dynamic_light_id, &position);
        length = NuVecDist(&((GAMEOBJECT *)Player[i])->collision_position, mid, &delta);
        NuLgtLaser(DeathStarZapLaserTypePc, DeathStarZapSizeWidthPc, DeathStarZapSizeLengthPc, DeathStarZapWidthBiasPc,
                   mid, &delta, 0xff808040, DeathStarZapEndWidthPc, length);
        NewRumble(((GAMEOBJECT *)Player[i])->pad->bound_pad,
                  (f32)((f64)qrand() * (f64)0.000015259021893143654f * (f64)0.3f), 0);
        if (qrand() < 0x1000)
            NewBuzzFrames(((GAMEOBJECT *)Player[i])->pad->bound_pad, 1, 0);
        PlaySfx("ForceLightningLp", &((GAMEOBJECT *)Player[i])->collision_position);
        DisorientateCode((GAMEOBJECT *)Player[i], mid, 225.0f);
    }
}
