#include <stdio.h>
#include <string.h>
#include "saga.h"

i32 __cdecl KaminoInside(void) {
    if (WORLD->area != NULL && WORLD->area == KAMINO_ADATA) {
        if (WORLD->current_level == LevelFixupSlotPc0087b12c) {
            if (CUTSTOPGAME != 0 || GameCam->sock_position.sock_index == 5 || GameCam->sock_position.sock_index == 6)
                return 1;
        } else if (WORLD->current_level == KAMINOE_LDATA) {
            if (CUTSTOPGAME == 0 && GameCam->sock_position.sock_index != 0x1e)
                return 1;
        }
    }
    return 0;
}
i32 __cdecl KaminoDiscoOn(void) {
    return KaminoDiscoMusicStatePc.mode == 2;
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

void GrievousA_Update(WORLDINFO *world) {
    if (netclient != 0)
        return;

    if (Grievous_obj == NULL)
        return;

    if (Grievous_obj->current_hp > 0)
        return;

    if (FreePlay == 0)
        KillBossPlayCutScene((i32)(i16)id_GRIEVOUS, 0, 0.0f, "ep3_GeneralGrievous_Outro");
    else
        KillBossCompleteLevel((i32)(i16)id_GRIEVOUS, 0, 0.0f);
}

void KashyyykA_Reset(WORLDINFO *unused0) {
    KashyyykA_msg_TotalWookies = CheckGizAIMessage(gizaimessagesys, "TotalWookies", NULL);
    KashyyykA_msg_WookiesToRescue = CheckGizAIMessage(gizaimessagesys, "WookiesToRescue", NULL);
}

i32 Action_BoulderSection(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                          i32 param_count, i32 first_time, f32 unused6) {
    struct gameobject_s *object;
    u8 can_attack;
    struct PART_PC *nearest;
    f32 nearest_distance;
    NUVEC difference;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time != 0) {
        i32 index;
        processor->action_data_4 = 1.0f;
        processor->action_data_5 = 1.0f;
        for (index = 0; index < param_count; ++index) {
            char *value = NuStrIStr(params[index], "boulder_range=");
            if (value != NULL) {
                processor->action_data_4 = AIParamToFloat(&packet->script_processor, value + 14);
            } else if ((value = NuStrIStr(params[index], "attack_time=")) != NULL) {
                processor->action_data_5 = AIParamToFloat(&packet->script_processor, value + 12);
            } else {
                packet->movement_instruction_parameter = AIParamToFloat(processor, params[index]);
            }
        }
    }
    can_attack = FreePlay == 0 || CharCategory_IsCategory(object, 0) != 0 || CharCategory_IsCategory(object, 1) != 0 ||
                 CharCategory_IsCategory(object, 8) != 0;
    if (!can_attack) {
        processor->action_timer -= FRAMETIME;
        if (processor->action_timer < 0.0f) {
            processor->action_timer = 0.5f;
            object->pad->buttons_pressed |= GAMEPAD_TOGGLELEFT;
        }
    }
    object->field_ef8 |= GAMEOBJECT_EF8_FLAG_KEEP_WEAPON_OUT;
    nearest = NULL;
    nearest_distance = processor->action_data_4 * processor->action_data_4;
    difference = difference;
    if (boulder_part[0] != NULL) {
        const f32 distance = NuVecDistSqr(&object->collision_position, &boulder_part[0]->position, &difference);
        if (distance < nearest_distance) {
            nearest = boulder_part[0];
            nearest_distance = distance;
        }
    }
    if (boulder_part[1] != NULL) {
        const f32 distance = NuVecDistSqr(&object->collision_position, &boulder_part[1]->position, &difference);
        if (distance < nearest_distance)
            nearest = boulder_part[1];
    }
    // The retail third probe reads padding past this two-entry array. Do not
    // reproduce that out-of-bounds access or enlarge the recovered global.
    if (nearest != NULL) {
        packet->movement_look_target = &nearest->position;
        if (can_attack)
            object->pad->buttons_pressed |= GAMEPAD_ACTION;
    } else {
        if (system != NULL && system->player_1 != NULL && system->player_1->ai != NULL) {
            AIPACKET_PC *player_packet = system->player_1->ai;
            AIMoveInstruction(packet, &player_packet->last_path_position, player_packet->mover_height,
                              &player_packet->path_info, 1, packet->movement_instruction_parameter);
        }
    }
    return 0;
}

void TempleC_AlwaysUpdate(WORLDINFO *unused0) {
    NUVEC position;
    if (LevelFixupSlotPc0087b150 != NULL && Players_AveragePos(&position, NULL) &&
        NuVecXZDistSqr(&position, &TempleC_StreamStatusPos, NULL) < 100.0f)
        other_level_override = LevelFixupSlotPc0087b150->index;
}

void VaderB_Reset(WORLDINFO *unused0) {
    vader_b_complete_msg = SetGizAIMessage(gizaimessagesys, "VaderBComplete", 0.0f, NULL);
    vader_b_playersDead = 0;
}

f32 gunshipb_seekrange = 4.0f;
f32 gunshipb_seekmom = 5.0f;
f32 gunshipb_seekmomseek = 5.0f;
static const char GunshipAGreenBoltPc[16] = "trooper_green";
static const char GunshipARedBoltPc[12] = "trooper_red";
static const char GunshipAPowerA11Pc[12] = "power_a11";
static const char GunshipAPowerB11Pc[12] = "power_b11";
static const char GunshipAPowerA21Pc[12] = "power_a21";
static const char GunshipAPowerB21Pc[12] = "power_b21";

void __cdecl GunshipA_Init(WORLDINFO *world) {
    gunship_bolts[1] = BoltType_FindIDByName(GunshipAGreenBoltPc, world);
    gunship_bolts[0] = BoltType_FindIDByName(GunshipARedBoltPc, world);
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
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, (char *)GunshipAPowerA11Pc);
    LevGizmo[1] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, (char *)GunshipAPowerB11Pc);
    LevGizmo[2] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, (char *)GunshipAPowerA21Pc);
    LevGizmo[3] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, (char *)GunshipAPowerB21Pc);
}

void __cdecl GunshipA_Update(WORLDINFO *world) {
    UpdateMiniSnowTroopers(world);
}

void __cdecl GunshipA_DrawPc(WORLDINFO *world) {
    DrawMiniSnowTroopers(world);
}

void __cdecl GunShip_DragBombSeekBlowUp(GAMEOBJECT *object) {
    GIZMOBLOWUP_PC *nearest = NULL;
    f32 nearest_distance;
    NUVEC offset, nearest_offset, direction;
    f32 distance, factor;
    i32 index;
    if (object->character_context != 0x34)
        return;
    nearest_distance = gunshipb_seekrange * gunshipb_seekrange;
    for (index = 0; index < 8; ++index) {
        GIZMOBLOWUP_PC *blowup;
        if (LevGizmo[index] == NULL || LevGizmo[index]->object == NULL)
            continue;
        blowup = (GIZMOBLOWUP_PC *)LevGizmo[index]->object;
        if ((blowup->status_flags & 0x800001) != 0x800000)
            continue;
        distance = NuVecDistSqr(&blowup->mid_position, &object->collision_position, &offset);
        if (nearest_distance > distance) {
            nearest_distance = distance;
            nearest_offset = offset;
            nearest = blowup;
        }
    }
    if (nearest == NULL)
        return;
    NuVecNorm(&direction, &nearest_offset);
    distance = NuFsqrt(nearest_distance);
    factor = 1.0f - distance / gunshipb_seekrange;
    direction.x *= gunshipb_seekmom * factor;
    direction.z *= gunshipb_seekmom * factor;
    object->velocity.x = SeekValF(object->velocity.x, direction.x, gunshipb_seekmomseek);
    object->velocity.z = SeekValF(object->velocity.z, direction.z, gunshipb_seekmomseek);
}

BONUS_GUNSHIP_B_NETPACKET_PC *BonusGunshipBNetPacketPc = NULL;
i32 GunshipPlayerDeadPc = 0;

typedef struct BONUS_GUNSHIP_FLAGS_PC {
    u8 progress;
    u8 secondary;
    u8 reserved[14];
} BONUS_GUNSHIP_FLAGS_PC;
STATIC_ASSERT(sizeof(BONUS_GUNSHIP_FLAGS_PC) == sizeof(LevFlag));
STATIC_ASSERT(offsetof(BONUS_GUNSHIP_FLAGS_PC, progress) == 0);
STATIC_ASSERT(offsetof(BONUS_GUNSHIP_FLAGS_PC, secondary) == 1);
STATIC_ASSERT(offsetof(GAMEOBJECT, sock_position.sock_index) == 0x671);
STATIC_ASSERT(offsetof(GAMEOBJECT, sock_position.distance) == 0x69c);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc) == 0x1fc);
STATIC_ASSERT(offsetof(GIZOBSTACLE, runtime_flags) == 0x9c);
#define BONUS_GUNSHIP_FLAGS_PC_VIEW ((BONUS_GUNSHIP_FLAGS_PC *)LevFlag)

void __cdecl BonusGunshipA_ResetPc(WORLDINFO *world) {
    GunshipPlayerDeadPc = 0;
    BONUS_GUNSHIP_FLAGS_PC_VIEW->secondary = 0;
    bonus_gunship_store_progress_flag = 0;
    if (BONUS_GUNSHIP_FLAGS_PC_VIEW->progress == 1)
        BONUS_GUNSHIP_FLAGS_PC_VIEW->progress = 2;
}

void __cdecl BonusGunshipA_UpdatePc(WORLDINFO *world) {
    GAMEOBJECT *first = (GAMEOBJECT *)Player[0];
    GAMEOBJECT *second = (GAMEOBJECT *)Player[1];
    if (BONUS_GUNSHIP_FLAGS_PC_VIEW->progress == 0) {
        if ((first != NULL && first->sock_position.sock_index == 0 && first->sock_position.distance > 12.1f) ||
            (second != NULL && second->sock_position.sock_index == 0 && second->sock_position.distance > 12.1f)) {
            if (netclient == 0) {
                Doors_SetLastDoor((DOOR_s *)Door_FindByName(world, "gunshipa_mid"));
                bonus_gunship_store_progress_flag = 1;
                StoreLevelProgress(world);
                second = (GAMEOBJECT *)Player[1];
                first = (GAMEOBJECT *)Player[0];
                bonus_gunship_store_progress_flag = 0;
            }
            BONUS_GUNSHIP_FLAGS_PC_VIEW->progress = 1;
        }
    }
    if (GunshipPlayerDeadPc == 0 && ((first != NULL && first->dead != 0) || (second != NULL && second->dead != 0))) {
        GunshipPlayerDeadPc = 1;
        ResetLevel(world, "ep2_bonus_gunshipcavalry_explode", 1);
    }
}

void __cdecl BonusGunshipB_InitPc(WORLDINFO *world) {
    BonusGunshipBNetPacketPc = (BONUS_GUNSHIP_B_NETPACKET_PC *)SetLevelHack(12);
    LevGizObst[0] = GizObstacle_FindByName((GIZOBSTACLESYS_s *)world->giz_obstacle_sys, "obstacle9");
}

void __cdecl BonusGunshipB_ResetPc(WORLDINFO *world) {
    MiscTime = 0.0f;
    BONUS_GUNSHIP_FLAGS_PC_VIEW->progress = 0;
    BONUS_GUNSHIP_FLAGS_PC_VIEW->secondary = 0;
    GunshipPlayerDeadPc = 0;
    bonus_gunship_store_progress_flag = 0;
}

void __cdecl BonusGunshipB_UpdatePc(WORLDINFO *world) {
    if (netclient == 0) {
        if (GunshipPlayerDeadPc == 0 && ((Player[0] != NULL && ((GAMEOBJECT *)Player[0])->dead != 0) ||
                                         (Player[1] != NULL && ((GAMEOBJECT *)Player[1])->dead != 0))) {
            GunshipPlayerDeadPc = 1;
            ResetLevel(world, "ep2_bonus_gunshipcavalry_explode", 1);
        }
        if (BONUS_GUNSHIP_FLAGS_PC_VIEW->progress == 0) {
            f32 delay = 15.0f;
            if (LevDeaths >= 3) {
                delay = (f32)((f64)LevDeaths + 15.0);
                if (delay > 30.0f)
                    delay = 30.0f;
            }
            if (GameTimer.time_elapsed >= delay) {
                BONUS_GUNSHIP_FLAGS_PC_VIEW->progress = 1;
                if (LevGizObst[0] != NULL)
                    LevGizObst[0]->runtime_flags |= 4;
                MiscTime = 45.0f;
                TimerScalePc = 1.0f;
                TimerAlphaPc = 0.0f;
            }
        } else if (BONUS_GUNSHIP_FLAGS_PC_VIEW->progress == 1) {
            f32 previous_time;
            NuFmod(MiscTime, 5.0f);
            previous_time = MiscTime;
            MiscTime -= FRAMETIME;
            if (MiscTime <= 0.0f) {
                if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0)
                    LoseCoins((GAMEOBJECT *)Player[0], 1);
                if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0)
                    LoseCoins((GAMEOBJECT *)Player[1], 1);
                if (GunshipPlayerDeadPc == 0) {
                    GunshipPlayerDeadPc = 1;
                    ResetLevel(world, "ep2_bonus_gunshipcavalry_explode", 1);
                }
                BONUS_GUNSHIP_FLAGS_PC_VIEW->progress = 2;
            } else if (MiscTime > 0.0f && (i32)MiscTime != (i32)previous_time) {
                TimerScalePc = 1.25f;
                TickTockSfxPc();
            }
        }
        BonusGunshipBNetPacketPc->progress = BONUS_GUNSHIP_FLAGS_PC_VIEW->progress;
        BonusGunshipBNetPacketPc->secondary = BONUS_GUNSHIP_FLAGS_PC_VIEW->secondary;
        BonusGunshipBNetPacketPc->time = MiscTime;
    } else {
        BONUS_GUNSHIP_FLAGS_PC_VIEW->progress = BonusGunshipBNetPacketPc->progress;
        BONUS_GUNSHIP_FLAGS_PC_VIEW->secondary = BonusGunshipBNetPacketPc->secondary;
        MiscTime = BonusGunshipBNetPacketPc->time;
    }
}

void __cdecl BonusGunshipB_PanelPc(WORLDINFO *world) {
    if (BONUS_GUNSHIP_FLAGS_PC_VIEW->progress == 1) {
        if (MiscTime > 0.0f)
            DrawTimerPc((i32)MiscTime + 1, 0, 0);
    }
}

static __declspec(noinline) void JediBResetRecordPc(JEDIB_CREATURE_PC *record) {
    if (record == NULL)
        return;
    if (record->flags & 2) {
        if (FreePlay == 0 && (jedi_b.unique_spawned & 1) == 0) {
            record->id = CharacterFixupIdPc007f19d8;
            jedi_b.unique_spawned |= 1;
        } else if (FreePlay == 0 && (jedi_b.unique_spawned & 2) == 0) {
            record->id = CharacterFixupIdPc007f19d4;
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
    record->field_0x58 = 0;
}

static __declspec(noinline) void __cdecl JediBKilledCallbackPc(GAMEOBJECT *object) {
    i32 index, other, alive, wave;
    JEDIB_CREATURE_PC *record;
    if (object == NULL)
        return;
    for (index = 0; index < jedi_b.goody_count; index++) {
        if (jedi_b.goodies[index].object == object) {
            jedi_b.goodies[index].object = NULL;
            alive = 0;
            for (other = 0; other < jedi_b.goody_count; other++) {
                if (jedi_b.goodies[other].object != NULL)
                    alive++;
            }
            if (alive == 0)
                jedi_b.goody_count = 0;
            return;
        }
    }
    for (index = 0; index < jedi_b.baddie_count; index++) {
        record = &jedi_b.baddies[index];
        if (record->object == object) {
            for (wave = 0; wave < 6; wave++) {
                if (jedi_b.wave_ids[wave] == record->id && jedi_b.wave_spawned[wave] == 0) {
                    jedi_b.wave_spawned[wave] = 1;
                    jedi_b.wave_timer = 0.0f;
                    break;
                }
            }
            record->object = NULL;
            record->flags = (record->flags & ~0x18u) | 1;
            JediBResetRecordPc(record);
            return;
        }
    }
}

void __cdecl JediB_Init(WORLDINFO *world) {
    NUVEC position, offset;
    AILOCATOR_PC locator;
    JEDIB_CREATURE_PC *goody, *baddie;
    f32 step, x, z, distance;
    i32 angle, count, index;
    if (Mission_Active(MissionSys) != NULL)
        return;
    memset(&jedi_b, 0, sizeof(jedi_b));
    jedi_b.seed = jedib_seed;
    jedib_netpacket = (JEDIB_NETPACKET_PC *)SetLevelHack(0x20);
    if (netclient != 0)
        return;
    for (x = -jedib_outer_r; x <= jedib_outer_r; x += step) {
        step = jedib_create_step_Normal;
        for (z = -jedib_outer_r; z <= jedib_outer_r; z += step) {
            position.x = jedib_offset - jedib_offset * 2.0f * NuRandFloatSeeded(&jedi_b.seed) + x;
            position.y = 0.0f;
            position.z = jedib_offset - jedib_offset * 2.0f * NuRandFloatSeeded(&jedi_b.seed) + z;
            distance = position.x * position.x + position.z * position.z;
            if (distance < jedib_outer_r * jedib_outer_r && distance > jedib_inner * jedib_inner) {
                position.y = GameShadow(NULL, &position, 5.0f, 0);
                if (position.y == 2000000.0f)
                    position.y = 0.0f;
                angle = NuRandIntSeeded(&jedi_b.seed);
                if (JediBInitLocator(world, &position, angle, &locator, jedib_outer_r) != 0 &&
                    jedi_b.baddie_count < 256) {
                    goody = &jedi_b.baddies[jedi_b.baddie_count++];
                    goody->position = position;
                    goody->locator = locator;
                    goody->flags &= ~2u;
                    offset.x = 0.0f;
                    offset.y = 0.0f;
                    offset.z = jedib_proximity;
                    angle = NuRandIntSeeded(&jedi_b.seed);
                    NuVecRotateY(&offset, &offset, NuRandIntSeeded(&jedi_b.seed));
                    count = (u32)NuRandIntSeeded(&jedi_b.seed) %
                                (u32)(jedib_max_baddies_per_goody - jedib_min_baddies_per_goody) +
                            jedib_min_baddies_per_goody;
                    for (index = 0; index < count && jedi_b.baddie_count < 256; index++) {
                        if (index != 0)
                            angle = NuAngAdd(angle, 0x10000 / count);
                        NuVecRotateY(&position, &offset, angle);
                        NuVecAdd(&position, &position, &goody->position);
                        if (JediBInitLocator(world, &position, angle, &locator, jedib_outer_r) != 0) {
                            baddie = &jedi_b.baddies[jedi_b.baddie_count++];
                            baddie->flags |= 2;
                            baddie->position = position;
                            baddie->locator = locator;
                            baddie->partner = goody;
                            baddie->angle = angle;
                            goody->partner = baddie;
                        }
                    }
                }
            }
            step = jedib_create_step_Normal;
        }
    }
}

void __cdecl JediB_Reset(WORLDINFO *world) {
    i32 index, hero, slot;
    char name[32];
    if (Mission_Active(MissionSys) != NULL || netclient != 0)
        return;
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
                if (Player[slot] != NULL && (i16)((GAMEOBJECT *)Player[slot])->id == *JediB_playerids[hero]) {
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

static __declspec(noinline) void JediBStartPhasePc(WORLDINFO *world) {
    JEDIB_PHASE_PC *phase_lists[3] = {jedi_b_phase1, jedi_b_phase2, jedi_b_phase3};
    JEDIB_PHASE_PC *entry;
    JEDIB_CREATURE_PC *record;
    AILOCATOR_PC *locator;
    char name[16];
    i32 i, chosen, index, id;
    if (netclient != 0)
        return;
    if (jedi_b.phase > 2 && jedi_b.timer < 3.0f) {
        jedi_b.timer += FRAMETIME;
        return;
    }
    jedi_b.phase++;
    if (jedi_b.phase == 6)
        jedi_b.phase = 7;
    switch (jedi_b.phase) {
        case 1:
        case 2:
        case 3:
            for (entry = phase_lists[jedi_b.phase - 1]; entry != NULL && entry->id != NULL && jedi_b.goody_count < 8;
                 entry++) {
                sprintf(name, "phase%d_%d", jedi_b.phase, jedi_b.goody_count + 1);
                locator = AIPathFindLocator(world->ai_sys, name);
                if (locator == NULL)
                    continue;
                record = &jedi_b.goodies[jedi_b.goody_count];
                record->locator = *locator;
                record->object =
                    AddDynamicCreature(*entry->id, &locator->position, locator->rotation, (char *)entry->script,
                                       &locator->path_info, NULL, 1, NULL, NULL, 0, 0);
                if (jedi_b.goodies[jedi_b.goody_count].object == NULL)
                    continue;
                record = &jedi_b.goodies[jedi_b.goody_count];
                record->id = *entry->id;
                record->object->ai_packet.locator = locator;
                record->object->portal_flags |= 0x1000000;
                record->object->tag_object_callback = JediBKilledCallbackPc;
                jedi_b.goody_count++;
            }
            break;
        case 4:
        case 5:
        case 6:
            memset(jedi_b.wave_spawned, 0, sizeof(jedi_b.wave_spawned));
            for (i = 0; i < jedi_b.baddie_count; i++)
                jedi_b.baddies[i].flags &= ~0x18u;
            for (chosen = 0; chosen < 6; chosen++) {
                index = NuRand(NULL) % jedi_b.baddie_count;
                if (jedi_b.phase == 4)
                    id = id_BATTLEDROIDSECURITY;
                else if (jedi_b.phase == 5)
                    id = chosen < 3 ? id_BATTLEDROIDSECURITY : id_SUPERBATTLEDROID;
                else
                    id = chosen < 3 ? id_DROIDEKA : id_SUPERBATTLEDROID;
                for (;;) {
                    record = &jedi_b.baddies[index];
                    if ((record->flags & 8) == 0 && (record->flags & 2) != 0 && record->id != id)
                        break;
                    index++;
                    if (index >= jedi_b.baddie_count)
                        index = 0;
                }
                if (record->object != NULL) {
                    RemoveGameObject(record->object, 1);
                    record->flags &= ~1u;
                    record->object = NULL;
                }
                record->id = id;
                jedi_b.wave_ids[chosen] = (i16)id;
                record->flags |= 8;
            }
            jedi_b.wave_timer = 0.0f;
            break;
    }
    if (jedi_b.msg_phase == NULL)
        jedi_b.msg_phase = CheckGizAIMessage(gizaimessagesys, "Phase", NULL);
    if (jedi_b.msg_phase_complete == NULL)
        jedi_b.msg_phase_complete = CheckGizAIMessage(gizaimessagesys, "PhaseComplete", NULL);
    if (jedi_b.msg_objectives_left == NULL)
        jedi_b.msg_objectives_left = CheckGizAIMessage(gizaimessagesys, "ObjectivesLeft", NULL);
    if (jedi_b.msg_restrain[0] == NULL)
        jedi_b.msg_restrain[0] = CheckGizAIMessage(gizaimessagesys, "RestrainPadme", NULL);
    if (jedi_b.msg_restrain[1] == NULL)
        jedi_b.msg_restrain[1] = CheckGizAIMessage(gizaimessagesys, "RestrainAnakin", NULL);
    if (jedi_b.msg_restrain[2] == NULL)
        jedi_b.msg_restrain[2] = CheckGizAIMessage(gizaimessagesys, "RestrainObiWan", NULL);
    jedi_b.msg_phase->value = (f32)jedi_b.phase;
    jedi_b.msg_objectives_left->value = (f32)jedi_b.goody_count;
    jedi_b.msg_phase_complete->value = 0.0f;
    jedi_b.timer = 0.0f;
    jedi_b.state = 1;
}

static __declspec(noinline) void JediBTickPhasePc(void) {
    i32 wave_done, i, release;
    GAMEOBJECT *boss;
    jedi_b.timer += FRAMETIME;
    switch (jedi_b.phase) {
        case 1:
        case 2:
        case 3:
            jedi_b.msg_objectives_left->value = (f32)jedi_b.goody_count;
            if (jedi_b.msg_phase_complete->value == 1.0f) {
                jedi_b.state = 2;
                jedi_b.timer = 0.0f;
            }
            break;
        case 4:
        case 5:
        case 6:
            wave_done = 1;
            for (i = 0; i < 6; i++) {
                if (jedi_b.wave_spawned[i] == 0) {
                    wave_done = 0;
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
                    if (jedi_b.baddies[i].flags & 8)
                        release = i;
                }
                if (release != -1)
                    jedi_b.baddies[release].flags |= 0x10;
                jedi_b.wave_timer = 0.0f;
            }
            if (wave_done) {
                jedi_b.state = 2;
                jedi_b.timer = 0.0f;
            }
            break;
        case 7:
            boss = jedi_b.boss;
            if (boss == NULL) {
                boss = FindGameObject(id_JANGOFETT, 1, 1, 0, 0);
                jedi_b.boss = boss;
                if (boss == NULL)
                    break;
            }
            if (boss->dead != 0 || boss->current_hp == 0) {
                jedi_b.state = 2;
                jedi_b.timer = 0.0f;
            }
            break;
    }
}

static __declspec(noinline) void JediBEndPhasePc(WORLDINFO *world) {
    if (jedi_b.timer < (f64)0.1f) {
        jedi_b.timer += FRAMETIME;
        return;
    }
    if (jedi_b.phase >= 7) {
        if (FreePlay == 0)
            NewLData = LevelFixupSlotPc0087b0f0;
        CompleteLevel(world);
    }
    jedi_b.state = 0;
    jedi_b.timer = 0.0f;
}

void __cdecl JediB_DrawPanelPc(WORLDINFO *world) {
    i16 ids[8];
    u8 spawned[8] = {0};
    i32 count, index;
    if (Mission_Active(MissionSys) != NULL)
        return;
    if (netclient == 0) {
        if (nethost != 0) {
            jedib_netpacket->state = jedi_b.state;
            jedib_netpacket->phase = jedi_b.phase;
        }
        if (jedi_b.state != 1)
            return;
        switch (jedi_b.phase) {
            case 1:
            case 2:
            case 3:
                if (jedi_b.goody_count != 0) {
                    count = jedi_b.goody_count;
                    if (count > 8)
                        count = 8;
                    for (index = 0; index < count; index++) {
                        ids[index] = (i16)jedi_b.goodies[index].id;
                        if (jedi_b.goodies[index].object == NULL)
                            spawned[index] = 1;
                    }
                } else {
                    count = 1;
                    if (jedi_b.phase == 1)
                        ids[0] = CharacterFixupIdPc007f19c8;
                    else if (jedi_b.phase == 2)
                        ids[0] = id_ANAKINPADAWAN;
                    else if (jedi_b.phase == 3)
                        ids[0] = CharacterFixupIdPc007f19d0;
                }
                DrawMeleeTargetsAlphaPc(ids, spawned, NULL, count);
                if (nethost != 0) {
                    PCByteCopy_candidate(jedib_netpacket->ids, ids, count * 2);
                    PCByteCopy_candidate(jedib_netpacket->spawned, spawned, count);
                    jedib_netpacket->count = (i16)count;
                }
                break;
            case 4:
            case 5:
            case 6:
                DrawMeleeTargetsAlphaPc(jedi_b.wave_ids, jedi_b.wave_spawned, NULL, 6);
                if (nethost != 0) {
                    PCByteCopy_candidate(jedib_netpacket->ids, jedi_b.wave_ids, 12);
                    PCByteCopy_candidate(jedib_netpacket->spawned, jedi_b.wave_spawned, 6);
                }
                break;
            case 7:
                if (jedi_b.boss != NULL)
                    DrawBossHitPoints(jedi_b.boss);
                break;
        }
    } else {
        if (jedib_netpacket->state != 1)
            return;
        switch (jedib_netpacket->phase) {
            case 1:
            case 2:
            case 3:
                DrawMeleeTargetsAlphaPc(jedib_netpacket->ids, jedib_netpacket->spawned, NULL, jedib_netpacket->count);
                break;
            case 4:
            case 5:
            case 6:
                DrawMeleeTargetsAlphaPc(jedib_netpacket->ids, jedib_netpacket->spawned, NULL, 6);
                break;
            case 7:
                if (jedi_b.boss == NULL)
                    jedi_b.boss = FindGameObject(id_JANGOFETT, 1, 1, 0, 0);
                if (jedi_b.boss != NULL)
                    DrawBossHitPoints(jedi_b.boss);
                break;
        }
    }
}

void __cdecl JediB_Update(WORLDINFO *world) {
    i32 hero, spawn, slot, i, spawn_behind;
    u32 shift;
    u64 mask;
    GAMEOBJECT *boss, *target, *object;
    JEDIB_CREATURE_PC *baddie;
    NUVEC spawn_position;
    if (Mission_Active(MissionSys) != NULL || netclient != 0)
        return;
    for (hero = 0; hero < 3; hero++) {
        if (FreePlay != 0 && jedi_b.heroes[hero] == NULL) {
            for (spawn = 0; spawn < 4; spawn++) {
                if (NuSpecialGetVisibilityFn(&jedi_b.hero_spawns[hero][spawn]) != 0)
                    jedi_b.heroes[hero] = AddDynamicCreature(*JediB_playerids[hero],
                                                             NuSpecialGetDrawPos(&jedi_b.hero_spawns[hero][spawn]), 0,
                                                             "Party", NULL, NULL, 1, NULL, NULL, 0, 0);
            }
        }
        if (jedi_b.heroes[hero] == NULL || jedi_b.msg_restrain[hero]->value != 1.0f)
            continue;
        for (spawn = 0; spawn < 4; spawn++) {
            if (NuSpecialGetVisibilityFn(&jedi_b.hero_spawns[hero][spawn]) == 0)
                continue;
            jedi_b.heroes[hero]->position = *NuSpecialGetDrawPos(&jedi_b.hero_spawns[hero][spawn]);
            NuVecAdd(&jedi_b.heroes[hero]->position, &jedi_b.heroes[hero]->position, &jedib_offsets[hero]);
            jedi_b.heroes[hero]->ai_update_position = jedi_b.heroes[hero]->position;
            jedi_b.heroes[hero]->field_0x276 = (u16)jedib_yrot[hero];
            jedi_b.heroes[hero]->velocity.x = 0.0f;
            jedi_b.heroes[hero]->velocity.y = 0.0f;
            jedi_b.heroes[hero]->velocity.z = 0.0f;
            jedi_b.heroes[hero]->respawn_timer = 0.0f;
        }
    }
    jedi_b.exclusion_mask = 0;
    if (jedi_b.timer < 4.0f) {
        if (player != NULL) {
            shift = player->hostility_group;
            jedi_b.exclusion_mask = shift < 64 ? ((u64)1 << shift) : 0;
        }
        if (player2 != NULL) {
            shift = player2->hostility_group;
            jedi_b.exclusion_mask |= shift < 64 ? ((u64)1 << shift) : 0;
        }
    } else if (jedi_b.phase == 7) {
        if (jedi_b.boss != NULL) {
            target = player2 != NULL && jedi_b.boss->ai_packet.opponent == player2 ? player2 : player;
            shift = target->hostility_group;
            jedi_b.exclusion_mask = shift < 64 ? ((u64)1 << shift) : 0;
        }
    } else {
        if (player->collision_position.x * player->collision_position.x +
                player->collision_position.z * player->collision_position.z >
            jedib_safe_r * jedib_safe_r) {
            shift = player->hostility_group;
            jedi_b.exclusion_mask = shift < 64 ? ((u64)1 << shift) : 0;
        }
        if (player2 != NULL && player2->collision_position.x * player2->collision_position.x +
                                       player2->collision_position.z * player2->collision_position.z >
                                   jedib_safe_r * jedib_safe_r) {
            shift = player2->hostility_group;
            jedi_b.exclusion_mask |= shift < 64 ? ((u64)1 << shift) : 0;
        }
    }
    boss = jedi_b.boss;
    for (slot = 0; slot < 8; slot++) {
        if (Player[slot] == NULL)
            continue;
        ((GAMEOBJECT *)Player[slot])->ai_opponent_exclusion_mask[0] = 0;
        ((GAMEOBJECT *)Player[slot])->ai_opponent_exclusion_mask[1] = 0;
        if (jedi_b.phase == 7 && boss != NULL && (i8)((GAMEOBJECT *)Player[slot])->flags_1fc >= 0) {
            shift = boss->hostility_group;
            mask = shift < 64 ? ((u64)1 << shift) : 0;
            ((GAMEOBJECT *)Player[slot])->ai_opponent_exclusion_mask[0] |= (u32)mask;
            ((GAMEOBJECT *)Player[slot])->ai_opponent_exclusion_mask[1] |= (u32)(mask >> 32);
        }
    }
    switch (jedi_b.state) {
        case 0:
            JediBStartPhasePc(world);
            break;
        case 1:
            JediBTickPhasePc();
            break;
        case 2:
            JediBEndPhasePc(world);
            break;
    }
    JediBActiveCountPc = 0;
    for (i = 0; i < jedi_b.baddie_count; i++) {
        baddie = &jedi_b.baddies[i];
        object = baddie->object;
        if (object != NULL) {
            if ((baddie->flags & 0x10) != 0) {
                object->ai_packet.locator = NULL;
                if ((player->character_data->model_flags & 0x80000) != 0 ||
                    ((i16)player->objptr->id == CharacterFixupIdPc007f19c4 && (i8)player->flags_1fc >= 0) ||
                    (player->character_data->game_character->weapon_action == 4 &&
                     object->character_data->game_character->weapon_action == 4)) {
                    if (object->ai_packet.opponent == player)
                        object->ai_packet.opponent = NULL;
                    if (player2 != NULL) {
                        if ((player2->character_data->model_flags & 0x80000) == 0 &&
                            ((i16)player->objptr->id != CharacterFixupIdPc007f19c4 || (i8)player->flags_1fc < 0) &&
                            (player->character_data->game_character->weapon_action != 4 ||
                             object->character_data->game_character->weapon_action != 4))
                            object->ai_packet.opponent = player2;
                        else if (object->ai_packet.opponent == player2)
                            object->ai_packet.opponent = NULL;
                    }
                } else {
                    object->ai_packet.opponent = player;
                }
            } else {
                if ((OnOrInsidePlane(&object->position, &PlayPlanePc[1].point, &PlayPlanePc[1].normal, NULL, 2.0f,
                                     NULL) == 0 &&
                     OnOrInsidePlane(&baddie->object->position, &PlayPlanePc[2].point, &PlayPlanePc[2].normal, NULL,
                                     2.0f, NULL) == 0) ||
                    (OnOrInsidePlane(&baddie->locator.position, &PlayPlanePc[1].point, &PlayPlanePc[1].normal, NULL,
                                     2.0f, NULL) == 0 &&
                     OnOrInsidePlane(&baddie->locator.position, &PlayPlanePc[2].point, &PlayPlanePc[2].normal, NULL,
                                     2.0f, NULL) == 0)) {
                    object = baddie->object;
                    if (object == NULL)
                        continue;
                    if (((APIOBJECT_PC *)object)->ai->opponent == NULL && baddie->partner != NULL &&
                        baddie->partner->object != NULL)
                        ((APIOBJECT_PC *)object)->ai->opponent = baddie->partner->object;
                } else {
                    baddie->position = baddie->object->position;
                    baddie->angle = baddie->object->field_0x276;
                    RemoveGameObject(baddie->object, 1);
                    baddie->object = NULL;
                    continue;
                }
            }
        } else if ((baddie->flags & 0x10) == 0 && (OnOrInsidePlane(&baddie->position, &PlayPlanePc[1].point,
                                                                   &PlayPlanePc[1].normal, NULL, 1.5f, NULL) != 0 ||
                                                   OnOrInsidePlane(&baddie->position, &PlayPlanePc[2].point,
                                                                   &PlayPlanePc[2].normal, NULL, 1.5f, NULL) != 0)) {
            object = baddie->object;
        } else {
            spawn_behind = (baddie->flags & 1) != 0 &&
                           OnOrInsidePlane(&baddie->position, &PlayPlanePc[1].point, &PlayPlanePc[1].normal, NULL, 0.5f,
                                           NULL) == 0 &&
                           OnOrInsidePlane(&baddie->position, &PlayPlanePc[2].point, &PlayPlanePc[2].normal, NULL, 0.5f,
                                           NULL) == 0;
            spawn_position = baddie->position;
            if (spawn_behind)
                NuVecRotateY(&spawn_position, &spawn_position, 0x8000);
            if (baddie->id == -1)
                JediBResetRecordPc(baddie);
            object = AddDynamicCreature(baddie->id, &spawn_position, baddie->angle, "gen_bdroids",
                                        &baddie->locator.path_info, NULL, 1, NULL, NULL, 0, 0);
            baddie->object = object;
            if (object == NULL)
                continue;
            object->ai_packet.locator = &baddie->locator;
            object->tag_object_callback = JediBKilledCallbackPc;
            if (baddie->id == id_BOB) {
                if (baddie->field_0x58 == 0) {
                    baddie->field_0x58 = object->layer_mask;
                    if (object->flags_ef0 & 0x200)
                        baddie->flags |= 4;
                    else
                        baddie->flags &= ~4u;
                } else {
                    object->layer_mask = baddie->field_0x58;
                    if (baddie->flags & 4)
                        object->flags_ef0 |= 0x200;
                    else
                        object->flags_ef0 &= ~0x200u;
                }
            }
        }
        object = baddie->object;
        if (object == NULL)
            continue;
        JediBActiveCountPc++;
        if ((baddie->flags & 0x10) == 0) {
            object->ai_opponent_exclusion_mask[0] |= jedi_b.exclusion_mask_low;
            object->ai_opponent_exclusion_mask[1] |= jedi_b.exclusion_mask_high;
        }
    }
}
