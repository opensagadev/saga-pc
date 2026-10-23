#include <string.h>
#include "saga.h"

i32 __cdecl AnakinGreenSabre(GAMEOBJECT *object) {
    if (FreePlay == 0 && object->id == id_ANAKINPADAWAN && WORLD->area != NULL &&
        (WORLD->area == JEDI_ADATA || WORLD->area == DOOKU_ADATA)) {
        return 1;
    }
    return 0;
}

extern NUVEC boulder_old_pos[2];
extern f32 boulder_wait;
extern i32 i_boulder;
extern f32 boulder_radius;
extern i32 boulder_blowup_type;

void __cdecl KashyyykD_Init(WORLDINFO *world) {
    boulder_wait = 0.0f;
    i_boulder = 0;
    boulder_blowup_type = GizmoBlowupGetTypeFromNameTableId(WORLD, GizmoBlowupGetNameTableId("ball_blowup_null"));
}

void __cdecl KashyyykD_Reset(WORLDINFO *world) {
    char name[256];
    i32 i;
    for (i = 0; i < 2; ++i) {
        sprintf(name, "ball%ib", i + 1);
        if (NuSpecialFind(world->current_gscn, &LevHSpecial[i], name, 1))
            NuSpecialSetVisibility(&LevHSpecial[i], 0);
        boulder_part[i] = NULL;
        sprintf(name, "ball%i", i + 1);
        if (NuSpecialFind(world->current_gscn, &LevHSpecial[i], name, 1)) {
            NuSpecialSetVisibility(&LevHSpecial[i], 0);
            LevInstAnimResetPc.animations[i] = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&LevHSpecial[i]);
            LevInstAnimResetPc.animations[i]->ltime = 0.0f;
            LevInstAnimResetPc.animations[i]->flags &= ~1u;
        }
    }
}

void __cdecl Boulder_Move(PART_PC *part, f32 unused) {
    i32 index;
    f32 reciprocal;
    if (netclient != 0)
        return;
    if (part == boulder_part[0])
        index = 0;
    else if (part == boulder_part[1])
        index = 1;
    else {
        KillPart(part, 0);
        return;
    }
    boulder_old_pos[index] = part->position;
    part->transform = *NuSpecialGetDrawMtx(&LevHSpecial[index]);
    if (part->scale_time <= 1.0f) {
        part->velocity = v000;
    } else {
        NuVecSub(&part->velocity, &part->position, &boulder_old_pos[index]);
        reciprocal = 1.0f / FRAMETIME;
        NuVecScale(&part->velocity, &part->velocity, reciprocal);
    }
    if (LevInstAnimResetPc.animations[index]->ltime > 1.0f && !(LevInstAnimResetPc.animations[index]->flags & 1)) {
        KillPart(part, 0);
    } else if (part->active & 1) {
        PlaySfx("Kas_BoulderLoop", &part->position);
    }
}

void __cdecl Boulder_Kill(PART_PC *part, i32 unused) {
    i32 index;
    if (part == boulder_part[0])
        index = 0;
    else if (part == boulder_part[1])
        index = 1;
    else
        return;
    if (part == NULL)
        return;
    if (boulder_blowup_type != -1)
        GizmoBlowUpTypeBlowUp(WORLD, boulder_blowup_type, &part->position);
    NuSpecialSetVisibility(&LevHSpecial[index], 0);
    LevInstAnimResetPc.animations[index]->ltime = 0.0f;
    LevInstAnimResetPc.animations[index]->flags &= ~1u;
    boulder_part[index] = NULL;
}

static void __cdecl BoulderSpawnPc(i32 index) {
    ADDPART_PC params;
    PART_PC *part;
    if (LevInstAnimResetPc.animations[index] != NULL && LevHSpecial[index].display_special != NULL) {
        params = Default_ADDPART;
        params.matrix = &NuMtxSetIdentityTemplate;
        params.velocity = &v000;
        params.owner = NULL;
        params.field_14 = boulder_radius;
        params.field_18 = boulder_radius;
        params.gravity = 0.0f;
        params.special = &LevHSpecial[index];
        params.flags = 0x80861a;
        params.move_fn = Boulder_Move;
        params.collision_fn = PartCollide_3D;
        params.kill_fn = Boulder_Kill;
        params.time_step = FRAMETIME;
        part = AddPart(&params);
        if (part != NULL)
            boulder_part[index] = part;
        LevInstAnimResetPc.animations[index]->ltime = 1.0f;
        LevInstAnimResetPc.animations[index]->flags |= 1;
        LevInstAnimResetPc.animations[index]->matrix = NuMtxSetIdentityTemplate;
        NuSpecialSetVisibility(&LevHSpecial[index], 1);
    }
}

void __cdecl KashyyykD_Update(WORLDINFO *world) {
    if (netclient != 0)
        return;
    if (boulder_wait <= 0.0f) {
        if (boulder_part[i_boulder] != NULL)
            return;
        BoulderSpawnPc(i_boulder);
        ++i_boulder;
        if (i_boulder == 2)
            i_boulder = 0;
        boulder_wait = 8.0f;
    } else {
        boulder_wait -= FRAMETIME;
    }
}

/* Episode III FixUpLevels callbacks (Cruiser_A/C/D, Grievous_A, Kashyyyk_A/B/C, Vader_A/B/C). */

typedef struct cruisera_levflag_pc_s {
    u8 starfighter_state;
} CRUISERA_LEVFLAG_PC;

typedef struct cruiserc_pc_s {
    GIZAIMESSAGE *dooku_fight;
    GAMEOBJECT *count_dooku;
    GIZAIMESSAGE *free_palpatine;
    GAMEOBJECT *palpatine;
    AILOCATOR_PC *palpatine_locator;
} CRUISERC_PC;
STATIC_ASSERT(sizeof(CRUISERC_PC) == 0x14);

typedef struct cruiserc_netpacket_pc_s {
    i16 free_palpatine;
    i16 dooku_fight;
} CRUISERC_NETPACKET_PC;

typedef struct cruiserd_netpacket_pc_s {
    f32 frame;
    f32 speed;
    u32 flags;
} CRUISERD_NETPACKET_PC;

typedef struct cruiserd_lift_anim_pc_s {
    NUMTX matrix;
    f32 tfactor;
    u8 unknown_44[8];
    f32 ltime;
    u32 playing : 1;
    u32 flags_rest : 31;
    f32 fparam1;
    f32 prev_eval_time;
    u16 animation_index;
} CRUISERD_LIFT_ANIM_PC;
STATIC_ASSERT(offsetof(CRUISERD_LIFT_ANIM_PC, tfactor) == 0x40);
STATIC_ASSERT(offsetof(CRUISERD_LIFT_ANIM_PC, ltime) == 0x4c);
STATIC_ASSERT(offsetof(CRUISERD_LIFT_ANIM_PC, fparam1) == 0x54);
STATIC_ASSERT(offsetof(CRUISERD_LIFT_ANIM_PC, animation_index) == 0x5c);

typedef struct vadera_pc_s {
    GIZAIMESSAGE *in_control_room_message;
    GIZAIMESSAGE *ceiling_collapse_message;
    GIZAIMESSAGE *timer_message;
    f32 timer;
    i16 count;
    i16 subtitle;
    GIZFORCE_PC *forces[4];
    AILOCATOR_PC *big_jump_locator;
    i32 collapse_started;
    u8 reset_flag;
    u8 reserved_2d[3];
} VADERA_PC;
STATIC_ASSERT(sizeof(VADERA_PC) == 0x30);
STATIC_ASSERT(offsetof(VADERA_PC, big_jump_locator) == 0x24);

typedef struct vadera_netpacket_pc_s {
    u16 count;
    i16 subtitle;
    f32 timer;
} VADERA_NETPACKET_PC;

typedef struct vaderc_pc_s {
    GIZAIMESSAGE *final_fight_message;
    AILOCATOR_PC *big_jump_locator;
    NUSPECIALHANDLE rocks[10];
    i16 platform_ids[10];
    u8 progress_mask;
    u8 players_dead;
    u8 reserved_96[2];
} VADERC_PC;
STATIC_ASSERT(sizeof(VADERC_PC) == 0x98);
STATIC_ASSERT(offsetof(VADERC_PC, platform_ids) == 0x80);
STATIC_ASSERT(offsetof(VADERC_PC, progress_mask) == 0x94);

typedef struct terrain_platform_reader_pc_s {
    u8 opaque_00_40[0x40];
    NUMTX *scene_object;
    u8 opaque_44_6c[0x28];
} TERRAIN_PLATFORM_READER_PC;
STATIC_ASSERT(sizeof(TERRAIN_PLATFORM_READER_PC) == 0x6c);

typedef struct terrain_set_reader_pc_s {
    u8 opaque_00_68[0x68];
    TERRAIN_PLATFORM_READER_PC *platforms;
} TERRAIN_SET_READER_PC;
STATIC_ASSERT(offsetof(TERRAIN_SET_READER_PC, platforms) == 0x68);

static f32 CruiserD_LiftSpeedPc = 0.1f;
static f32 CruiserD_frame = 1.0f;
static i32 CruiserD_direction = 1;
static f32 CruiserD_ResetTimePc = 1.0f;
static f32 VaderA_TimeBonusPc = 20.0f;
static f32 VaderA_TimeLimitsPc[3] = {30.0f, 15.0f, 0.0f};
static NUVEC vadar_cam_pos = {0.0f, 0.88f, -12.4f};
static NUVEC vadar_cam_tgt = {0.0f, 2.0f, -17.4f};

CRUISERC_PC cruiser_c = {0};
CRUISERC_NETPACKET_PC *crusiserc_netpacket = NULL;
CRUISERD_NETPACKET_PC *cruiserd_netpacket = NULL;
static i32 CruiserE_ix = 0;
static NUSPECIALHANDLE CruiserD_Lift = {NULL, NULL, NULL};
static CRUISERD_LIFT_ANIM_PC *CruiserD_LiftAnim = NULL;
static i32 CruiserD_Lift_plat_id = 0;
static GIZAIMESSAGE *CruiserD_LiftChase_msg = NULL;
void *vadera_netpacket = NULL;
VADERA_PC vader_a = {0};
VADERC_PC vader_c = {0};
void *vaderc_netpacket = NULL;
i32 Vader_ObiWanKilledAnakin = 0;
void *CurTerr = NULL;
extern i16 CharacterFixupIdPc007f1a0c;

GIZFORCE_PC *__cdecl GizForces_FindForce(WORLDINFO *world, char *name);

void __cdecl CruiserAInit(WORLDINFO *world) {
    ((CRUISERA_LEVFLAG_PC *)LevFlag)->starfighter_state = 0;
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], "starfighter1", 1);
    NuSpecialFind(world->current_gscn, &LevHSpecial[1], "starfighter2", 1);
    if (FreePlay != 0)
        ((CRUISERA_LEVFLAG_PC *)LevFlag)->starfighter_state = 2;
}

void __cdecl CruiserAUpdate(WORLDINFO *world) {
    if (((CRUISERA_LEVFLAG_PC *)LevFlag)->starfighter_state == 1) {
        ((CRUISERA_LEVFLAG_PC *)LevFlag)->starfighter_state = 2;
        if (NuSpecialExistsFn(&LevHSpecial[0]) != 0)
            NuSpecialSetVisibility(&LevHSpecial[0], 1);
        if (NuSpecialExistsFn(&LevHSpecial[1]) != 0)
            NuSpecialSetVisibility(&LevHSpecial[1], 1);
    }
}

void __cdecl CruiserCReset(WORLDINFO *world) {
    crusiserc_netpacket = SetLevelHack(4);
    memset(&cruiser_c, 0, sizeof(cruiser_c));
}

void __cdecl CruiserCUpdate(WORLDINFO *world) {
    if (cruiser_c.free_palpatine == NULL)
        cruiser_c.free_palpatine = CheckGizAIMessage(gizaimessagesys, "FreePalpatine", NULL);
    if (cruiser_c.dooku_fight == NULL)
        cruiser_c.dooku_fight = CheckGizAIMessage(gizaimessagesys, "DookuFight", NULL);
    if (netclient != 0) {
        cruiser_c.free_palpatine->value = (f32)crusiserc_netpacket->free_palpatine;
        cruiser_c.dooku_fight->value = (f32)crusiserc_netpacket->dooku_fight;
    }
    if (cruiser_c.palpatine == NULL)
        cruiser_c.palpatine = FindGameObject(CharacterFixupIdPc007f1a0c, 0x400, 1, 1, 0);
    if (cruiser_c.palpatine_locator == NULL)
        cruiser_c.palpatine_locator = AIPathFindLocator(WORLD->ai_sys, "Palpatine");
    if (cruiser_c.count_dooku == NULL)
        cruiser_c.count_dooku = FindGameObject(id_DOOKU, 1, 1, 1, 0);
    if (FreePlay == 0 && cruiser_c.palpatine != NULL && cruiser_c.palpatine_locator != NULL &&
        cruiser_c.free_palpatine != NULL && cruiser_c.free_palpatine->value < 2.0f) {
        cruiser_c.palpatine->position = cruiser_c.palpatine_locator->position;
        cruiser_c.palpatine->field_0x276 = cruiser_c.palpatine_locator->rotation;
    }
}

void __cdecl CruiserCPanel(WORLDINFO *world) {
    if (netclient != 0)
        return;
    if (cruiser_c.count_dooku != NULL && cruiser_c.dooku_fight != NULL && cruiser_c.dooku_fight->value == 1.0f)
        DrawBossHitPoints(cruiser_c.count_dooku);
    else
        DrawBossHitPoints(NULL);
}

void __cdecl CruiserDInit(WORLDINFO *world) {
    i32 i;
    for (i = 0; i < world->area->level_count; i++) {
        if (world->area->levels[i] == LevelFixupSlotPc0087b170->index)
            CruiserE_ix = i;
    }
    if (NuSpecialFind(WORLD->current_gscn, &CruiserD_Lift, "lift", 1) != 0) {
        CruiserD_LiftAnim = NuSpecialGetInstAnim(&CruiserD_Lift);
        CruiserD_Lift_plat_id = FindPlatInst(NuSpecialGetInstanceix(&CruiserD_Lift));
    }
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, gizpanel_gizmotype_id, "panel1");
    cruiserd_netpacket = SetLevelHack(12);
}

void __cdecl CruiserDReset(WORLDINFO *world) {
    CruiserD_LiftChase_msg = CheckGizAIMessage(gizaimessagesys, "LiftChase", NULL);
    MiscTime = 0.0f;
    if (NuSpecialExistsFn(&CruiserD_Lift) != 0 && CruiserD_LiftAnim != NULL) {
        if ((LevelProgressData[CruiserE_ix].unknown_2800[0] & 1) == 0) {
            CruiserD_LiftAnim->playing = 1;
            CruiserD_LiftAnim->ltime = 1.0f;
            CruiserD_direction = 1;
            CruiserD_frame = 1.0f;
            CruiserD_LiftAnim->tfactor = CruiserD_LiftSpeedPc;
        } else if (CruiserD_direction >= 0) {
            f32 *end_frame =
                (f32 *)((NUGSCN *)CruiserD_Lift.scene)->instance_animation_data[CruiserD_LiftAnim->animation_index];
            CruiserD_LiftAnim->ltime = *end_frame;
            CruiserD_LiftAnim->playing = 0;
            CruiserD_LiftAnim->tfactor = CruiserD_LiftSpeedPc;
        } else {
            CruiserD_LiftAnim->ltime = CruiserD_frame;
            CruiserD_LiftAnim->playing = 1;
            CruiserD_LiftAnim->tfactor = -CruiserD_LiftSpeedPc;
        }
    }
    CruiserD_LiftChase = 0;
}

void __cdecl CruiserDUpdate(WORLDINFO *world) {
    CruiserD_LiftChase = 0;
    if (NuSpecialExistsFn(&CruiserD_Lift) == 0 || CruiserD_LiftAnim == NULL || CruiserD_LiftChase_msg == NULL)
        return;
    if (netclient == 0) {
        CruiserD_LiftChase_msg->value = 0.0f;
        if (CruiserD_direction >= 0 && LevGizmo[0] != NULL && LevGizmo[0]->object != NULL &&
            (((GIZPANEL_s *)LevGizmo[0]->object)->field_0x62[0] & 2) != 0) {
            CruiserD_LiftAnim->playing = 1;
            CruiserD_LiftAnim->tfactor = -CruiserD_LiftSpeedPc;
            CruiserD_direction = -1;
        }
        if (CruiserD_LiftAnim->playing && CruiserD_Lift_plat_id != -1) {
            NUVEC *lift_pos;
            i32 i;
            if ((f32)CruiserD_direction >= 0.0f) {
                CruiserD_LiftChase = 1;
                CruiserD_LiftChase_msg->value = 1.0f;
            }
            lift_pos = NuSpecialGetDrawPos(&CruiserD_Lift);
            for (i = 0; i < 8; i++) {
                GAMEOBJECT *victim = (GAMEOBJECT *)Player[i];
                if (victim != NULL && victim->dead == 0 &&
                    (victim->supporting_platform_id == CruiserD_Lift_plat_id ||
                     victim->collision_position.z > lift_pos->z)) {
                    ObjHitObj(NULL, victim, -1, 0, 0, 1);
                    KillGameObject(victim, 2, 0);
                    if (CruiserD_direction >= 0 && MiscTime == 0.0f)
                        MiscTime = CruiserD_ResetTimePc;
                }
            }
            if (MiscTime > 0.0f) {
                MiscTime -= FRAMETIME;
                if (MiscTime <= 0.0f) {
                    MiscTime = 0.0f;
                    if (ChallengeMode != 3)
                        ResetLevel(NULL, NULL, 1);
                }
            }
        }
    }
    if (CruiserD_LiftAnim->playing)
        PlaySfx("Cru_HugeWallMoveLp", NuSpecialGetDrawPos(&CruiserD_Lift));
    CruiserD_frame = CruiserD_LiftAnim->ltime;
    if (netclient != 0) {
        CruiserD_LiftAnim->ltime = cruiserd_netpacket->frame;
        CruiserD_LiftAnim->tfactor = cruiserd_netpacket->speed;
        CruiserD_LiftAnim->playing = cruiserd_netpacket->flags & 1;
    }
}

void __cdecl GrievousA_Init(WORLDINFO *world) {
}

void __cdecl GrievousA_Reset(WORLDINFO *world) {
    if (netclient != 0)
        return;
    Grievous_obj = FindGameObject(id_GRIEVOUS, 1, 1, 0, 0);
    if (Grievous_obj != NULL)
        DrawBossHitPoints(Grievous_obj);
}

void __cdecl KashyyykA_Init(WORLDINFO *world) {
}

void __cdecl KashyyykA_Update(WORLDINFO *world) {
}

void __cdecl KashyyykA_Panel(WORLDINFO *world) {
    if (Mission_Active(MissionSys) == NULL) {
        i16 characters[3] = {id_WOOKIEE, id_WOOKIEE, id_WOOKIEE};
        u8 rescued[3] = {1, 1, 1};
        i32 total;
        i32 remaining;
        i32 i;
        if (KashyyykA_msg_TotalWookies != NULL && KashyyykA_msg_TotalWookies->value > 0.0f) {
            total = (i32)KashyyykA_msg_TotalWookies->value;
            if (total > 3)
                total = 3;
            if (KashyyykA_msg_WookiesToRescue != NULL && KashyyykA_msg_WookiesToRescue->value > 0.0f) {
                remaining = (i32)KashyyykA_msg_WookiesToRescue->value;
                if (remaining > 3)
                    remaining = 3;
                for (i = 0; i < remaining; i++)
                    rescued[i] = 0;
                DrawMeleeTargetsAlphaPc(characters, rescued, NULL, total);
            }
        }
    }
}

void __cdecl KashyyykB_Init(WORLDINFO *world) {
}

void __cdecl KashyyykB_Reset(WORLDINFO *world) {
}

void __cdecl KashyyykB_Update(WORLDINFO *world) {
}

void __cdecl KashyyykC_Init(WORLDINFO *world) {
}

void __cdecl KashyyykC_Update(WORLDINFO *world) {
}

static void VaderA_StartCollapseStage(WORLDINFO *world) {
    i32 path_index;
    NUSPECIALHANDLE specials[100];
    i32 special_count;
    i32 i;
    u32 *path_connection =
        AIPAthFindPathCnx(world->ai_sys, world->ai_sys->path_sys->active_path, "Block1_a", "Block1_b", &path_index);
    if (path_connection != NULL)
        path_connection[path_index] |= 0x80000000;
    vader_a.forces[0] = GizForces_FindForce(world, "force1");
    vader_a.forces[1] = GizForces_FindForce(world, "force2");
    vader_a.forces[2] = GizForces_FindForce(world, "force3");
    vader_a.forces[3] = GizForces_FindForce(world, "force4");
    vader_a.timer = VaderA_TimeLimitsPc[0];
    vader_a.count = 1;
    SetGizAIMessage(gizaimessagesys, "InControlRoom", 1.0f, vader_a.in_control_room_message);
    SetGizAIMessage(gizaimessagesys, "ceiling_collapse", 1.0f, vader_a.ceiling_collapse_message);
    special_count = NuSpecialFindMulti(world->current_gscn, specials, "wobble", 100, 0);
    for (i = 0; i < special_count; i++)
        NuSpecialSetVisibility(&specials[i], 0);
}

void __cdecl VaderA_Init(WORLDINFO *world) {
    memset(&vader_a, 0, sizeof(vader_a));
    vadera_netpacket = SetLevelHack(8);
    vader_a.big_jump_locator = AIPathFindLocator(world->ai_sys, "Bigjump_0");
    vader_a.in_control_room_message = SetGizAIMessage(gizaimessagesys, "InControlRoom", 0.0f, NULL);
    vader_a.ceiling_collapse_message = SetGizAIMessage(gizaimessagesys, "ceiling_collapse", 0.0f, NULL);
    vader_a.timer_message = SetGizAIMessage(gizaimessagesys, "timer", 0.0f, NULL);
}

void __cdecl VaderA_Reset(WORLDINFO *world) {
    DrawTimerPc(0, 0, 1);
    if (netclient == 0 && vader_a.count != 0)
        VaderA_StartCollapseStage(world);
    if (vader_a.timer_message != NULL)
        vader_a.timer_message->value = 0.0f;
    vader_a.collapse_started = 1;
    vader_a.reset_flag = 0;
}

void __cdecl VaderA_Update(WORLDINFO *world) {
    i32 i;
    if (vader_a.timer_message != NULL)
        vader_a.timer_message->value += FRAMETIME;
    if (netclient == 0) {
        if (vader_a.collapse_started != 0) {
            vader_a.collapse_started = 0;
            for (i = 0; i < 8; i++) {
                GAMEOBJECT *object = (GAMEOBJECT *)Player[i];
                if (object != NULL && (object->flags_1fc & 1) != 0)
                    SetObjOnSurface(object, 1);
            }
        }
        if (vader_a.big_jump_locator != NULL && player != NULL && player->supporting_platform_id != -1)
            vader_a.big_jump_locator->position = player->lower_position;
    }
    if (vader_a.count != 0) {
        f32 previous_time = vader_a.timer;
        vader_a.timer -= FRAMETIME;
        if (vader_a.count < 3) {
            for (i = 0; i < 4; i++) {
                if (vader_a.forces[i] != NULL && GizForce_Complete(vader_a.forces[i]) != 0) {
                    vader_a.timer += VaderA_TimeBonusPc;
                    vader_a.subtitle = 1;
                    vader_a.forces[i] = NULL;
                }
            }
            if (vader_a.timer < VaderA_TimeLimitsPc[vader_a.count]) {
                switch (vader_a.count) {
                    case 1:
                        SetGizAIMessage(gizaimessagesys, "ceiling_collapse", 2.0f, vader_a.ceiling_collapse_message);
                        break;
                    case 2:
                        SetGizAIMessage(gizaimessagesys, "ceiling_collapse", 3.0f, vader_a.ceiling_collapse_message);
                        vader_a.timer = 2.0f;
                        NuCameraGetMtx();
                        GameCameraMakeMiniCut2(&vadar_cam_pos, &vadar_cam_tgt, 0, 0.0f, 2.0f, 0.0f, 0.5f, 0, 0, 1);
                        break;
                    default:
                        break;
                }
                vader_a.count++;
            }
            if (vader_a.subtitle != 0 || (vader_a.timer > 0.0f && (i32)previous_time != (i32)vader_a.timer))
                TickTockSfxPc();
        } else if (netclient == 0 && vader_a.timer <= 0.0f) {
            GAMEOBJECT *object = (GAMEOBJECT *)Player[0];
            if (object != NULL && (object->flags_1fc & 0x80) != 0)
                LoseCoins(object, 1);
            object = (GAMEOBJECT *)Player[1];
            if (object != NULL && (object->flags_1fc & 0x80) != 0)
                LoseCoins(object, 1);
            KillGameObject(player, 2, 0);
        }
    }
    if (vader_a.reset_flag == 0 && ((Player[0] != NULL && ((GAMEOBJECT *)Player[0])->dead != 0 &&
                                     (((GAMEOBJECT *)Player[0])->flags_1f8 & 0x40000) == 0) ||
                                    (Player[1] != NULL && ((GAMEOBJECT *)Player[1])->dead != 0 &&
                                     (((GAMEOBJECT *)Player[1])->flags_1f8 & 0x40000) == 0))) {
        vader_a.reset_flag = 1;
        ResetLevel(NULL, NULL, 1);
    }
    if (netclient == 0) {
        ((VADERA_NETPACKET_PC *)vadera_netpacket)->count = vader_a.count;
        ((VADERA_NETPACKET_PC *)vadera_netpacket)->timer = vader_a.timer;
        ((VADERA_NETPACKET_PC *)vadera_netpacket)->subtitle = vader_a.subtitle;
    } else {
        vader_a.count = ((VADERA_NETPACKET_PC *)vadera_netpacket)->count;
        vader_a.timer = ((VADERA_NETPACKET_PC *)vadera_netpacket)->timer;
        vader_a.subtitle = ((VADERA_NETPACKET_PC *)vadera_netpacket)->subtitle;
    }
}

void __cdecl VaderA_DrawPanel(WORLDINFO *world) {
    if (vader_a.count != 0 && (i16)vader_a.count < 3) {
        DrawTimerPc((i32)vader_a.timer + 1, vader_a.subtitle, 0);
        vader_a.subtitle = 0;
    }
}

void __cdecl VaderB_Init(WORLDINFO *world) {
}

void __cdecl VaderB_Update(WORLDINFO *world) {
    if (netclient == 0 && vader_b_complete_msg != NULL && vader_b_complete_msg->value == 1.0f &&
        LevelFixupSlotPc0087b17c != NULL)
        GoToNewLevel(VADERC_LDATA->index);
    if (vader_b_playersDead == 0 && ((Player[0] != NULL && ((GAMEOBJECT *)Player[0])->dead != 0 &&
                                      (((GAMEOBJECT *)Player[0])->flags_1f8 & 0x40000) == 0) ||
                                     (Player[1] != NULL && ((GAMEOBJECT *)Player[1])->dead != 0 &&
                                      (((GAMEOBJECT *)Player[1])->flags_1f8 & 0x40000) == 0))) {
        vader_b_playersDead = 1;
        ResetLevel(NULL, NULL, 1);
    }
}

void __cdecl VaderB_DrawPanel(WORLDINFO *world) {
}

void __cdecl VaderC_Init(WORLDINFO *world) {
    char *names[10] = {"rock1", "rock2", "rock3", "rock4", "rock5", "rock6", "rock7", "rock8", "rock10", "rock11"};
    i32 i;
    memset(&vader_c, 0, sizeof(vader_c));
    vader_c.final_fight_message = CheckGizAIMessage(gizaimessagesys, "FinalFight", NULL);
    vader_c.big_jump_locator = AIPathFindLocator(world->ai_sys, "Bigjump_0");
    for (i = 0; i < 10; i++) {
        if (NuSpecialFind(world->current_gscn, &vader_c.rocks[i], names[i], 1) != 0)
            vader_c.platform_ids[i] = (i16)FindPlatInst(NuSpecialGetInstanceix(&vader_c.rocks[i]));
        else
            vader_c.platform_ids[i] = -1;
    }
    Vader_ObiWanKilledAnakin = 0;
    vaderc_netpacket = SetLevelHack(1);
}

void __cdecl VaderC_Reset(WORLDINFO *world) {
    vader_c.progress_mask = 0;
    vader_c.players_dead = 0;
}

void __cdecl VaderC_Update(WORLDINFO *world) {
    GAMEOBJECT *first;
    GAMEOBJECT *second;
    i32 i;
    if (netclient == 0) {
        GIZAIMESSAGE *final_fight = vader_c.final_fight_message;
        if (final_fight != NULL && ChallengeMode == 0 && final_fight->value == 0.0f &&
            vader_c.big_jump_locator != NULL && player != NULL && player->supporting_platform_id != -1) {
            vader_c.big_jump_locator->position = player->lower_position;
            for (i = 0; i < 10; i++) {
                if (Player[0] != NULL && vader_c.platform_ids[i] != -1 && ((GAMEOBJECT *)Player[0])->field_281 != 0 &&
                    ((GAMEOBJECT *)Player[0])->supporting_platform_id == vader_c.platform_ids[i] &&
                    ((GAMEOBJECT *)Player[0])->position.y >=
                        ((TERRAIN_SET_READER_PC *)CurTerr)
                            ->platforms[((GAMEOBJECT *)Player[0])->supporting_platform_id]
                            .scene_object->translation.y)
                    vader_c.progress_mask |= 1;
                if (Player[1] != NULL && vader_c.platform_ids[i] != -1 && ((GAMEOBJECT *)Player[1])->field_281 != 0 &&
                    ((GAMEOBJECT *)Player[1])->supporting_platform_id == vader_c.platform_ids[i] &&
                    ((GAMEOBJECT *)Player[1])->position.y >=
                        ((TERRAIN_SET_READER_PC *)CurTerr)
                            ->platforms[((GAMEOBJECT *)Player[1])->supporting_platform_id]
                            .scene_object->translation.y)
                    vader_c.progress_mask |= 2;
            }
            if (vader_c.progress_mask == 3) {
                struct DOOR_s *door;
                vader_c.final_fight_message->value = 1.0f;
                door = Door_FindByName(world, "door_fight");
                if (door != NULL)
                    Door_GoThrough(world, door, 1);
            }
        }
    }
    if (netclient != 0 && GameTimer.time_elapsed < 5.0f) {
        struct gizmo_pc_s *gizmo = GizmoFindByName(WORLD->gizmo_sys, obstacle_gizmotype_id, "obstacle20");
        if (gizmo != NULL && gizmo->object != NULL)
            ((GIZOBSTACLE *)gizmo->object)->progress_flags &= ~1u;
        gizmo = GizmoFindByName(WORLD->gizmo_sys, obstacle_gizmotype_id, "obstacle21");
        if (gizmo != NULL && gizmo->object != NULL)
            ((GIZOBSTACLE *)gizmo->object)->progress_flags &= ~1u;
        gizmo = GizmoFindByName(WORLD->gizmo_sys, obstacle_gizmotype_id, "obstacle22");
        if (gizmo != NULL && gizmo->object != NULL)
            ((GIZOBSTACLE *)gizmo->object)->progress_flags &= ~1u;
        gizmo = GizmoFindByName(WORLD->gizmo_sys, obstacle_gizmotype_id, "obstacle23");
        if (gizmo != NULL && gizmo->object != NULL)
            ((GIZOBSTACLE *)gizmo->object)->progress_flags &= ~1u;
    }
    if (vader_c.players_dead == 0) {
        first = (GAMEOBJECT *)Player[0];
        second = (GAMEOBJECT *)Player[1];
        if (((first != NULL && (first->flags_1fc & 0x80) != 0 && first->dead != 0 &&
              (first->flags_1f8 & 0x40000) == 0) ||
             (second != NULL && (second->flags_1fc & 0x80) != 0 && second->dead != 0 &&
              (second->flags_1f8 & 0x40000) == 0)) &&
            (((u8 *)vaderc_netpacket)[0] == 0 || first == NULL || (first->flags_1fc & 0x80) == 0 || second == NULL ||
             (second->flags_1fc & 0x80) == 0) &&
            (ChallengeMode == 0 || AreaGlobals_PC.field_1c < 10)) {
            vader_c.players_dead = 1;
            ResetLevel(NULL, NULL, 1);
        }
    }
}

void __cdecl VaderC_DrawPanel(WORLDINFO *world) {
    if (netclient != 0)
        return;
    if (vader_c.final_fight_message != NULL && vader_c.final_fight_message->value == 1.0f) {
        if (player2 != NULL) {
            DrawBossHitPoints(NULL);
        } else {
            GAMEOBJECT *opponent = (GAMEOBJECT *)Player[0];
            if (player == opponent)
                opponent = (GAMEOBJECT *)Player[1];
            if (opponent != NULL)
                DrawBossHitPoints(opponent);
        }
    }
}
