#include "saga.h"

typedef struct emperor_fight_packet_pc_s {
    i32 state;
    u8 enabled;
    u8 zap;
    u8 player_floor_mask;
    u8 reserved;
} EMPEROR_FIGHT_PACKET_PC;
STATIC_ASSERT(sizeof(EMPEROR_FIGHT_PACKET_PC) == 8);
STATIC_ASSERT(offsetof(EMPEROR_FIGHT_PACKET_PC, enabled) == 4);

typedef struct level_script_flags_pc_s {
    u8 first;
    u8 light_flags[4];
    u8 arrow_flags[4];
    u8 remaining[7];
} LEVEL_SCRIPT_FLAGS_PC;
STATIC_ASSERT(sizeof(LEVEL_SCRIPT_FLAGS_PC) == 16);
STATIC_ASSERT(offsetof(LEVEL_SCRIPT_FLAGS_PC, light_flags) == 1);
STATIC_ASSERT(offsetof(LEVEL_SCRIPT_FLAGS_PC, arrow_flags) == 5);

void __cdecl EmperorFightA_Init(WORLDINFO *world) {
    LEVEL_SCRIPT_FLAGS_PC *flags = (LEVEL_SCRIPT_FLAGS_PC *)&LevFlag;
    i32 i;
    emperorfighta_netpacket = SetLevelHack(8);
    ((EMPEROR_FIGHT_PACKET_PC *)emperorfighta_netpacket)->enabled = 1;
    NuSpecialFind(world->current_gscn, &LevHSpecial[3], (char *)EpisodeVI_liftrlightbon1, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[4], (char *)EpisodeVI_liftrlightbon2, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[5], (char *)EpisodeVI_liftrlightbon3, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[6], (char *)EpisodeVI_liftrlightbon4, 1);
    for (i = 0; i < 4; ++i)
        flags->light_flags[i] = 0;
    NuSpecialFind(world->current_gscn, &LevHSpecial[7], (char *)EpisodeVI_liftrarron1, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[8], (char *)EpisodeVI_liftrarron2, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[9], (char *)EpisodeVI_liftrarron3, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[10], (char *)EpisodeVI_liftrarron4, 1);
    for (i = 0; i < 4; ++i)
        flags->arrow_flags[i] = 0;
    NuSpecialFind(world->current_gscn, &LevHSpecial[30], (char *)EpisodeVI_doton01, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[31], (char *)EpisodeVI_doton02, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[32], (char *)EpisodeVI_doton03, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[33], (char *)EpisodeVI_doton04, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[34], (char *)EpisodeVI_doton05, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[35], (char *)EpisodeVI_doton06, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[36], (char *)EpisodeVI_doton07, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[37], (char *)EpisodeVI_doton08, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[38], (char *)EpisodeVI_doton09, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[39], (char *)EpisodeVI_doton10, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[40], (char *)EpisodeVI_doton11, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[41], (char *)EpisodeVI_doton12, 1);
    LevSfxFlag[0] = 0;
    LevSfxFlag[1] = 0;
    LevSfxFlag[2] = 0;
}

void __cdecl EmperorFightA_Reset(WORLDINFO *world) {
    GIZMO_PC *gizmo;
    i32 i;
    char name[32];
    LevAIMessage[0] = CheckGizAIMessage(gizaimessagesys, EpisodeVI_ShowHearts, NULL);
    LevAIMessage[1] = CheckGizAIMessage(gizaimessagesys, EpisodeVI_ElectricFloor, NULL);
    gizmo = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, (char *)EpisodeVI_obstacle21);
    if (gizmo != NULL && gizmo->object != NULL)
        LevGizObst[0] = (GIZOBSTACLE *)gizmo->object;
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, (char *)EpisodeVI_force15);
    LevGizmo[1] = GizmoFindByName(world->gizmo_sys, force_gizmotype_id, (char *)EpisodeVI_force16);
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], (char *)EpisodeVI_lift_force_lnull1, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], (char *)EpisodeVI_lift_force_rnull1, 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[2], (char *)EpisodeVI_lift_main1, 1);
    LevArea[0] = AISysFindArea(world->ai_sys, (char *)EpisodeVI_ElectricFloor);
    eFloor_timer = 0.0f;
    ((EMPEROR_FIGHT_PACKET_PC *)emperorfighta_netpacket)->state = 0;
    routemask_efloor_on = 0;
    routemask_efloor_off = 0;
    for (i = 0; i < world->ai_sys->path_sys->active_path->route_count; ++i) {
        if (NuStrICmp(world->ai_sys->path_sys->active_path->routes[i].name, (char *)EpisodeVI_electric_on) == 0)
            routemask_efloor_on = (u64)1 << i;
        else if (NuStrICmp(world->ai_sys->path_sys->active_path->routes[i].name, (char *)EpisodeVI_electric_off) == 0)
            routemask_efloor_off = (u64)1 << i;
        if (routemask_efloor_on != 0 && routemask_efloor_off != 0)
            break;
    }
    hspecial_efloor = &LevHSpecial[67];
    for (i = 0; i < 20; ++i) {
        if (i < 9)
            sprintf(name, EpisodeVI_elecpad_zero_format, i + 1);
        else
            sprintf(name, EpisodeVI_elecpad_format, i + 1);
        NuSpecialFind(world->current_gscn, &hspecial_efloor[i], name, 1);
    }
    gizmo = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, (char *)EpisodeVI_eFloor_AlwaysOn);
    if (gizmo != NULL && gizmo->object != NULL)
        LevGizObst[1] = (GIZOBSTACLE *)gizmo->object;
    gizmo = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, (char *)EpisodeVI_eFloor_AlwaysOf);
    if (gizmo != NULL && gizmo->object != NULL)
        LevGizObst[2] = (GIZOBSTACLE *)gizmo->object;
    gizmo = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, (char *)EpisodeVI_eFloor_OnOff);
    if (gizmo != NULL && gizmo->object != NULL)
        LevGizObst[3] = (GIZOBSTACLE *)gizmo->object;
}

void SarlaccPitA_Draw(WORLDINFO *unused0) {
    GIZAIMESSAGE *message = NULL;
    if (gizaimessagesys != NULL)
        message = CheckGizAIMessage(gizaimessagesys, "Boba_Dead", NULL);
    if (message == NULL || message->value != 0.0f) {
        DrawBossHitPoints(NULL);
    } else {
        GAMEOBJECT *boba = FindGameObject(id_BOBAFETT, 1, 1, 1, 0);
        if (gizaimessagesys != NULL)
            message = CheckGizAIMessage(gizaimessagesys, "BobaFightStarted", NULL);
        if (message != NULL && boba != NULL && message->value == 1.0f)
            DrawBossHitPoints(boba);
    }
}

typedef struct sarlacc_world_pc_s {
    u8 unknown_0000[0x50cc];
    i32 blowup_target_count;
    GIZMOBLOWUP_PC **blowup_targets;
} SARLACC_WORLD_PC;
STATIC_ASSERT(offsetof(SARLACC_WORLD_PC, blowup_target_count) == 0x50cc);
STATIC_ASSERT(offsetof(SARLACC_WORLD_PC, blowup_targets) == 0x50d0);
typedef struct sarlacc_panel_pc_s {
    u8 unknown_00[0x62];
    u8 state_flags;
} SARLACC_PANEL_PC;
STATIC_ASSERT(offsetof(SARLACC_PANEL_PC, state_flags) == 0x62);
void __cdecl SarlaccPitC_Update(WORLDINFO *world) {
    SARLACC_WORLD_PC *script_world = (SARLACC_WORLD_PC *)world;
    if (LevGameObject[0] != NULL && SarlaccBlowUpTargetCountPc == 4) {
        if (LevGameObject[0]->rider != NULL) {
            script_world->blowup_target_count = 4;
            script_world->blowup_targets = SarlaccBlowUpTargetsPc;
            if (!SarlaccTakenOverPc)
                SarlaccTakenOverPc = 1;
            if (SarlaccPowerPc != 0) {
                if (LevGameObject[0]->quick_shoot_bolt_id >= 0) {
                    SARLACC_PANEL_PC *panel;
                    --SarlaccPowerPc;
                    NuSpecialSetVisibility(&LevHSpecial[SarlaccPowerPc], 1);
                    NuSpecialSetVisibility(&LevHSpecial[SarlaccPowerPc + 8], 0);
                    panel = (SARLACC_PANEL_PC *)LevGizmo[0]->object;
                    if (panel->state_flags & 2)
                        panel->state_flags &= ~2;
                }
            } else if (LevGameObject[0]->portal_flags & 8)
                LevGameObject[0]->portal_flags &= ~8;
        } else if (SarlaccTakenOverPc) {
            SarlaccTakenOverPc = 0;
            script_world->blowup_target_count = 0;
            script_world->blowup_targets = NULL;
        }
        if (!SarlaccRechargingPc) {
            if ((u32)SarlaccPowerPc < 8 && LevGizmo[0] != NULL &&
                GizmoGetOutput(world->gizmo_sys, LevGizmo[0], 0, 0) != 0) {
                SarlaccRechargingPc = 1;
                SarlaccRechargeTimerPc = 0.0f;
            }
        } else {
            SarlaccRechargeTimerPc -= FRAMETIME;
            if (SarlaccRechargeTimerPc <= 0.0f) {
                LevGameObject[0]->portal_flags |= 8;
                NuSpecialSetVisibility(&LevHSpecial[SarlaccPowerPc], 0);
                NuSpecialSetVisibility(&LevHSpecial[SarlaccPowerPc + 8], 1);
                PlaySfx((char *)EpisodeVI_magnet_drop, NuSpecialGetDrawPos(&LevHSpecial[SarlaccPowerPc + 8]));
                ++SarlaccPowerPc;
                if (SarlaccPowerPc == 8) {
                    SarlaccRechargingPc = 0;
                    PlaySfx((char *)EpisodeVI_magnet_on, NuSpecialGetDrawPos(&LevHSpecial[16]));
                } else
                    SarlaccRechargeTimerPc = 0.25f;
            }
        }
    }
    if (!SarlaccTargetShieldPc[0]) {
        GIZMOBLOWUP_PC *target = SarlaccBlowUpTargetsPc[0];
        if (target != NULL && !(target->status_flags & 0x4000)) {
            PlaySfx((char *)EpisodeVI_ffieldoff, NuSpecialGetDrawPos(&LevHSpecial[16]));
            SarlaccTargetShieldPc[0] = 1;
        } else
            PlaySfx((char *)EpisodeVI_ffield, NuSpecialGetDrawPos(&LevHSpecial[16]));
    }
    if (!SarlaccTargetShieldPc[1]) {
        GIZMOBLOWUP_PC *target = SarlaccBlowUpTargetsPc[1];
        if (target != NULL && !(target->status_flags & 0x4000)) {
            PlaySfx((char *)EpisodeVI_ffieldoff, NuSpecialGetDrawPos(&LevHSpecial[17]));
            SarlaccTargetShieldPc[1] = 1;
        } else
            PlaySfx((char *)EpisodeVI_ffield, NuSpecialGetDrawPos(&LevHSpecial[17]));
    }
}

void __cdecl SarlaccPitC_Reset(WORLDINFO *world) {
    char name[32];
    GIZMO_PC *gizmo;
    GIZMOBLOWUP_PC *target;
    i32 i;
    SarlaccTakenOverPc = 0;
    SarlaccBlowUpTargetCountPc = 0;
    LevGameObject[0] = GetNamedGameObject(world->ai_sys, (char *)EpisodeVI_cannon_1);
    for (i = 1; i <= 2; ++i) {
        sprintf(name, EpisodeVI_cover_format, i);
        NuSpecialFind(world->current_gscn, &LevHSpecial[i + 15], name, 1);
        sprintf(name, EpisodeVI_cover_gizmo_format, i);
        gizmo = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, name);
        if (gizmo != NULL) {
            target = (GIZMOBLOWUP_PC *)gizmo->object;
            SarlaccBlowUpTargetsPc[SarlaccBlowUpTargetCountPc] = target;
            target->status_flags |= 0x8000000;
            ++SarlaccBlowUpTargetCountPc;
            UpdateMidPos(target);
        }
    }
    for (i = 1; i <= 2; ++i) {
        sprintf(name, EpisodeVI_spinner_format, i);
        gizmo = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, name);
        if (gizmo != NULL) {
            target = (GIZMOBLOWUP_PC *)gizmo->object;
            SarlaccBlowUpTargetsPc[SarlaccBlowUpTargetCountPc] = target;
            ++SarlaccBlowUpTargetCountPc;
            UpdateMidPos(target);
        }
    }
    for (i = 1; i <= 8; ++i) {
        sprintf(name, EpisodeVI_shot_format, i);
        NuSpecialFind(world->current_gscn, &LevHSpecial[i - 1], name, 1);
    }
    for (i = 1; i <= 8; ++i) {
        sprintf(name, EpisodeVI_shot_a_format, i);
        NuSpecialFind(world->current_gscn, &LevHSpecial[i + 7], name, 1);
    }
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, gizpanel_gizmotype_id, (char *)EpisodeVI_panel2);
    for (i = 0; i < 8; ++i) {
        NuSpecialSetVisibility(&LevHSpecial[i], SarlaccPowerPc <= i);
        NuSpecialSetVisibility(&LevHSpecial[i + 8], SarlaccPowerPc > i);
    }
}

struct SARLACC_BATTLE_MUSIC_VIEW_PC {
    u8 unknown_00[0x10];
    u8 disco_active;
};
STATIC_ASSERT(offsetof(SARLACC_BATTLE_MUSIC_VIEW_PC, disco_active) == 0x10);

i32 __cdecl SarlaccPitDiscoActive(WORLDINFO *world) {
    return world->current_level == SARLACCPITB_LDATA && sarlaccb_netpacket->disco_active != 0;
}
