#include <stdio.h>
#include <string.h>
#include "saga.h"

STATIC_ASSERT(offsetof(VADERA_PC, in_control_room_message) == 0x00);

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

    if (FreePlay != 0)
        KillBossCompleteLevel((i32)(i16)id_GRIEVOUS, 0, 0.0f);
    else
        KillBossPlayCutScene((i32)(i16)id_GRIEVOUS, 0, 0.0f, "ep3_GeneralGrievous_Outro");
}

void KashyyykA_Reset(WORLDINFO *unused0) {
    KashyyykA_msg_TotalWookies = CheckGizAIMessage(gizaimessagesys, "TotalWookies", NULL);
    KashyyykA_msg_WookiesToRescue = CheckGizAIMessage(gizaimessagesys, "WookiesToRescue", NULL);
}

i32 Action_BoulderSection(AISYS_PC *system, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **params,
                          i32 param_count, i32 first_time, f32 unused6) {
    GAMEOBJECT *object;
    PART_PC *nearest = NULL;
    i32 can_attack = 1;
    i32 index;
    f32 nearest_distance;
    NUVEC difference;
    if (packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
        return 1;
    object = packet->owner->objptr;
    if (first_time != 0) {
        processor->action_data_4 = 1.0f;
        processor->action_data_5 = 1.0f;
        for (index = 0; index < param_count; ++index) {
            char *value = NuStrIStr(params[index], "boulder_range=");
            if (value != NULL)
                processor->action_data_4 = AIParamToFloat(&packet->script_processor, value + 14);
            else if ((value = NuStrIStr(params[index], "attack_time=")) != NULL)
                processor->action_data_5 = AIParamToFloat(&packet->script_processor, value + 12);
            else
                packet->movement_instruction_parameter = AIParamToFloat(processor, params[index]);
        }
    }
    if (FreePlay != 0 && CharCategory_IsCategory(object, 0) == 0 && CharCategory_IsCategory(object, 1) == 0 &&
        CharCategory_IsCategory(object, 8) == 0) {
        can_attack = 0;
        processor->action_timer -= FRAMETIME;
        if (processor->action_timer < 0.0f) {
            processor->action_timer = 0.5f;
            object->pad->buttons_pressed |= GAMEPAD_TOGGLELEFT;
        }
    }
    object->portal_flags |= 0x10;
    nearest_distance = processor->action_data_4 * processor->action_data_4;
    for (index = 0; index < 3; ++index) {
        /* Native third slot aliases the first VaderA field, not array padding. */
        PART_PC *part = index == 2 ? (PART_PC *)vader_a.in_control_room_message : boulder_part[index];
        if (part != NULL) {
            f32 distance = NuVecDistSqr(&object->collision_position, &part->position, &difference);
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest = index == 2 ? (PART_PC *)vader_a.in_control_room_message : boulder_part[index];
            }
        }
    }
    if (nearest != NULL) {
        packet->movement_look_target = &nearest->position;
        if (can_attack != 0)
            object->pad->buttons_pressed |= GAMEPAD_ACTION;
    } else {
        AIPACKET_PC *player_packet = system->player_1->ai;
        AIMoveInstruction(packet, &player_packet->last_path_position, player_packet->mover_height,
                          &player_packet->path_info, 1, packet->movement_instruction_parameter);
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
        if ((blowup->status_flags & 0x800000) == 0 || (blowup->status_flags & 1) != 0)
            continue;
        distance = NuVecDistSqr(&blowup->mid_position, &object->collision_position, &offset);
        if (distance < nearest_distance) {
            nearest_distance = distance;
            nearest = blowup;
            nearest_offset = offset;
        }
    }
    if (nearest == NULL)
        return;
    NuVecNorm(&direction, &nearest_offset);
    nearest_distance = NuFsqrt(nearest_distance);
    factor = 1.0f - nearest_distance / gunshipb_seekrange;
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
    GAMEOBJECT *spawned;
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
                jedi_b.goodies[jedi_b.goody_count].locator = *locator;
                spawned = AddDynamicCreature(*entry->id, &locator->position, locator->rotation, (char *)entry->script,
                                             &locator->path_info, NULL, 1, NULL, NULL, 0, 0);
                jedi_b.goodies[jedi_b.goody_count].object = spawned;
                if (jedi_b.goodies[jedi_b.goody_count].object == NULL)
                    continue;
                jedi_b.goodies[jedi_b.goody_count].id = *entry->id;
                jedi_b.goodies[jedi_b.goody_count].object->ai_packet.locator = locator;
                jedi_b.goodies[jedi_b.goody_count].object->portal_flags |= 0x1000000;
                jedi_b.goodies[jedi_b.goody_count].object->tag_object_callback = JediBKilledCallbackPc;
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
    i32 count, index;
    if (Mission_Active(MissionSys) != NULL)
        return;
    {
        u8 spawned[8] = {0};
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
                    DrawMeleeTargetsAlphaPc(jedib_netpacket->ids, jedib_netpacket->spawned, NULL,
                                            jedib_netpacket->count);
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
}

void __cdecl JediB_Update(WORLDINFO *world) {
    i32 hero, spawn, slot, i, in_range;
    u32 shift;
    u64 mask;
    GAMEOBJECT *target, *object;
    JEDIB_CREATURE_PC *baddie;
    NUVEC spawn_position;
    NUVEC *pos;
    if (Mission_Active(MissionSys) != NULL || netclient != 0)
        return;
    for (hero = 0; hero < 3; hero++) {
        if (FreePlay != 0 && jedi_b.heroes[hero] == NULL) {
            for (spawn = 0; spawn < 4; spawn++) {
                if (NuSpecialGetVisibilityFn(&jedi_b.hero_spawns[hero][spawn]) != 0) {
                    pos = NuSpecialGetDrawPos(&jedi_b.hero_spawns[hero][spawn]);
                    jedi_b.heroes[hero] =
                        AddDynamicCreature(*JediB_playerids[hero], pos, 0, "Party", NULL, NULL, 1, NULL, NULL, 0, 0);
                }
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
        if (player != NULL)
            jedi_b.exclusion_mask = (u64)1 << player->hostility_group;
        if (player2 != NULL)
            jedi_b.exclusion_mask |= (u64)1 << player2->hostility_group;
    } else if (jedi_b.phase == 7) {
        if (jedi_b.boss != NULL) {
            if (player2 != NULL && jedi_b.boss->ai_packet.opponent == player2)
                shift = player2->hostility_group;
            else
                shift = player->hostility_group;
            jedi_b.exclusion_mask = (u64)1 << shift;
        }
    } else {
        if (player->collision_position.x * player->collision_position.x +
                player->collision_position.z * player->collision_position.z >
            jedib_safe_r * jedib_safe_r)
            jedi_b.exclusion_mask = (u64)1 << player->hostility_group;
        if (player2 != NULL && player2->collision_position.x * player2->collision_position.x +
                                       player2->collision_position.z * player2->collision_position.z >
                                   jedib_safe_r * jedib_safe_r)
            jedi_b.exclusion_mask |= (u64)1 << player2->hostility_group;
    }
    for (slot = 0; slot < 8; slot++) {
        object = (GAMEOBJECT *)Player[slot];
        if (object == NULL)
            continue;
        object->ai_opponent_exclusion_mask[0] = 0;
        object->ai_opponent_exclusion_mask[1] = 0;
        object = (GAMEOBJECT *)Player[slot];
        if ((object->flags_1fc & 0x80) == 0 && jedi_b.boss != NULL && jedi_b.phase == 7) {
            mask = (u64)1 << jedi_b.boss->hostility_group;
            object->ai_opponent_exclusion_mask[0] |= (u32)mask;
            object->ai_opponent_exclusion_mask[1] |= (u32)(mask >> 32);
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
        if (baddie->object != NULL) {
            if ((baddie->flags & 0x10) != 0) {
                baddie->object->ai_packet.locator = NULL;
                target = NULL;
                if ((player->character_data->model_flags & 0x80000) == 0 &&
                    ((i16)player->objptr->id != CharacterFixupIdPc007f19c4 || (player->flags_1fc & 0x80) != 0) &&
                    (player->character_data->game_character->weapon_action != 4 ||
                     baddie->object->character_data->game_character->weapon_action != 4)) {
                    target = player;
                } else {
                    if (baddie->object->ai_packet.opponent == player)
                        baddie->object->ai_packet.opponent = NULL;
                    if (player2 != NULL) {
                        if ((player2->character_data->model_flags & 0x80000) == 0 &&
                            ((i16)player->objptr->id != CharacterFixupIdPc007f19c4 ||
                             (player->flags_1fc & 0x80) != 0) &&
                            (player->character_data->game_character->weapon_action != 4 ||
                             baddie->object->character_data->game_character->weapon_action != 4))
                            target = player2;
                        else if (baddie->object->ai_packet.opponent == player2)
                            baddie->object->ai_packet.opponent = NULL;
                    }
                }
                if (target != NULL)
                    baddie->object->ai_packet.opponent = target;
            } else {
                in_range = (OnOrInsidePlane(&baddie->object->position, &PlayPlanePc[1].point, &PlayPlanePc[1].normal,
                                            NULL, 2.0f, NULL) == 0 &&
                            OnOrInsidePlane(&baddie->object->position, &PlayPlanePc[2].point, &PlayPlanePc[2].normal,
                                            NULL, 2.0f, NULL) == 0) ||
                           (OnOrInsidePlane(&baddie->locator.position, &PlayPlanePc[1].point, &PlayPlanePc[1].normal,
                                            NULL, 2.0f, NULL) == 0 &&
                            OnOrInsidePlane(&baddie->locator.position, &PlayPlanePc[2].point, &PlayPlanePc[2].normal,
                                            NULL, 2.0f, NULL) == 0);
                if (in_range == 0) {
                    baddie->position = baddie->object->position;
                    baddie->angle = baddie->object->field_0x276;
                    RemoveGameObject(baddie->object, 1);
                    baddie->object = NULL;
                    continue;
                }
                if (baddie->object == NULL)
                    continue;
                if (((APIOBJECT_PC *)baddie->object)->ai->opponent == NULL && baddie->partner != NULL &&
                    baddie->partner->object != NULL)
                    ((APIOBJECT_PC *)baddie->object)->ai->opponent = baddie->partner->object;
            }
        } else if ((baddie->flags & 0x10) == 0 && (OnOrInsidePlane(&baddie->position, &PlayPlanePc[1].point,
                                                                   &PlayPlanePc[1].normal, NULL, 1.5f, NULL) != 0 ||
                                                   OnOrInsidePlane(&baddie->position, &PlayPlanePc[2].point,
                                                                   &PlayPlanePc[2].normal, NULL, 1.5f, NULL) != 0)) {
        } else {
            if ((baddie->flags & 1) != 0 &&
                OnOrInsidePlane(&baddie->position, &PlayPlanePc[1].point, &PlayPlanePc[1].normal, NULL, 0.5f, NULL) ==
                    0 &&
                OnOrInsidePlane(&baddie->position, &PlayPlanePc[2].point, &PlayPlanePc[2].normal, NULL, 0.5f, NULL) ==
                    0) {
                spawn_position = baddie->position;
                NuVecRotateY(&spawn_position, &spawn_position, 0x8000);
            } else {
                spawn_position = baddie->position;
            }
            if (baddie->id == -1)
                JediBResetRecordPc(baddie);
            baddie->object = AddDynamicCreature(baddie->id, &spawn_position, baddie->angle, "gen_bdroids",
                                                &baddie->locator.path_info, NULL, 1, NULL, NULL, 0, 0);
            if (baddie->object == NULL)
                continue;
            baddie->object->ai_packet.locator = &baddie->locator;
            baddie->object->tag_object_callback = JediBKilledCallbackPc;
            if (baddie->id == id_BOB) {
                if (baddie->field_0x58 == 0) {
                    baddie->field_0x58 = baddie->object->layer_mask;
                    if (baddie->object->flags_ef0 & 0x200)
                        baddie->flags |= 4;
                    else
                        baddie->flags &= ~4u;
                } else {
                    baddie->object->layer_mask = baddie->field_0x58;
                    if (baddie->flags & 4)
                        baddie->object->flags_ef0 |= 0x200;
                    else
                        baddie->object->flags_ef0 &= ~0x200u;
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

/* Episode II FixUpLevels callbacks (Factory_B paint puzzle, Dooku_C, Kamino). */

typedef struct cutinfo_reader_pc_s {
    u8 opaque_00_04[0x4];
    void *instance;
    u8 opaque_08_54[0x4c];
    f32 previous_frame;
} CUTINFO_READER_PC;
STATIC_ASSERT(offsetof(CUTINFO_READER_PC, instance) == 0x4);
STATIC_ASSERT(offsetof(CUTINFO_READER_PC, previous_frame) == 0x54);

typedef struct cut_instance_reader_pc_s {
    u8 opaque_00_88[0x88];
    u32 flags_88;
    u32 unknown_8c;
    f32 current_frame;
} CUT_INSTANCE_READER_PC;
STATIC_ASSERT(offsetof(CUT_INSTANCE_READER_PC, flags_88) == 0x88);
STATIC_ASSERT(offsetof(CUT_INSTANCE_READER_PC, current_frame) == 0x90);

typedef struct dooku_c_pc_s {
    GIZAIMESSAGE *total_message;
    GIZAIMESSAGE *hit_message;
    NUSPECIALHANDLE node;
} DOOKU_C_PC;
STATIC_ASSERT(sizeof(DOOKU_C_PC) == 0x14);

extern f32 FactoryBConveyorStopFrame;
f32 FactoryBConveyorStopFrame = 28.0f;
static f32 FactoryBConveyorXSpeed = 0.0f;
static f32 FactoryBConveyorZSpeed = 0.0f;
CUTINFO *factoryb_cut = NULL;
GIZAIMESSAGE *factoryb_conveyor_stopped_msg = NULL;
static DOOKU_C_PC dooku_c = {0};
i32 PAINTPUZZLESTAGE = 0;
i32 paintmixed = 0;
i32 painttarget = 0;
i32 painttry = 0;
i32 randpaints[3] = {0};
i32 paintsused[2] = {0};
i32 played_sound = 0;
char *paint_name[6] = {"red_juice", "yellow_juice", "blue_juice", "orange_juice", "green_juice", "purple_juice"};
char *paint_panel_name[3] = {"redp", "yellp", "bluep"};
char *paint_target_colour[3] = {"but_orange", "but_green", "but_purple"};
char *paint_light_name[3] = {"off_purple", "off_green", "off_orange"};
nuhspecial_s paintlights[3] = {0};
struct gizmo_pc_s *paintobst[6] = {0};
struct gizmo_pc_s *paintpanel[3] = {0};
nuhspecial_s painttargetcolour[3] = {0};
NUVEC TempleC_StreamStatusPos = {52.57f, 0.76f, -16.54f};

void __cdecl KillBossNewLevel(i32 id, i32 unused_4, f32 delay, i32 level_index);
void __cdecl DrawForceBackEffect(NUSPECIALHANDLE *special);
void __cdecl InitPaintPuzzle(WORLDINFO *world);
void __cdecl UpdatePaintPuzzle(WORLDINFO *world);
void __cdecl DrawPaintLights(void);
void __cdecl GizPanel_Reset(void *panel);

static i32 PaintMixColourPc(i32 first, i32 second) {
    if ((first == 0 && second == 1) || (first == 1 && second == 0))
        return 3;
    if ((first == 2 && second == 1) || (first == 1 && second == 2))
        return 4;
    if ((first == 0 && second == 2) || (first == 2 && second == 0))
        return 5;
    return -1;
}

static void RandomisePaintsPc(void) {
    i32 i = 0;
    do {
        randpaints[i] = qrand() / 21846;
        if (i == 0) {
            i++;
        } else if (i == 1) {
            if (randpaints[1] != randpaints[0])
                i++;
        }
    } while (i < 2);
    randpaints[2] = 3 - randpaints[1] - randpaints[0];
}

void __cdecl DrawPaintLights(void) {
    i32 i;
    for (i = 0; i < ((i8 *)factoryb_netpacket)[1]; i++) {
        if (NuSpecialExistsFn((NUSPECIALHANDLE *)&paintlights[i]) != 0)
            NuSpecialDrawAt((NUSPECIALHANDLE *)&green_light, NuSpecialGetDrawMtx((NUSPECIALHANDLE *)&paintlights[i]));
    }
    for (i = 0; i < 3; i++) {
        i8 colour = ((i8 *)factoryb_netpacket)[3];
        if (i == colour - 3 || colour == 0)
            NuSpecialSetVisibility((NUSPECIALHANDLE *)&painttargetcolour[i], 1);
        else
            NuSpecialSetVisibility((NUSPECIALHANDLE *)&painttargetcolour[i], 0);
    }
}

void __cdecl ResetPaintPuzzle(WORLDINFO *world) {
    i32 i;
    for (i = 0; i < 6; i++)
        GizObstacle_JumpToStart((GIZOBSTACLE *)paintobst[i]->object);
    painttry = 0;
    PAINTPUZZLESTAGE = 1;
    RandomisePaintsPc();
}

void __cdecl UpdatePaintPuzzle(WORLDINFO *world) {
    i32 i;
    i32 used = 0;
    switch (PAINTPUZZLESTAGE) {
        case 0:
            GizObstacle_PlayBackwards((GIZOBSTACLE *)paintobst[paintmixed]->object);
            for (i = 0; i < 3; i++)
                GizPanel_Reset(paintpanel[i]->object);
            PAINTPUZZLESTAGE = 1;
            break;
        case 1:
            if (paintmixed != 0 && (((GIZOBSTACLE *)paintobst[paintmixed]->object)->anim_set->flags & 1) != 0)
                break;
            paintmixed = 0;
            painttarget = randpaints[painttry] + 3;
            PAINTPUZZLESTAGE = 2;
            break;
        case 2:
            for (i = 0; i < 3; i++) {
                if (((GIZOBSTACLE *)paintobst[i]->object)->anim_set->state == GAMEANIMSET_STATE_AT_END)
                    paintsused[used++] = i;
            }
            if (used == 2) {
                paintmixed = PaintMixColourPc(paintsused[0], paintsused[1]);
                PAINTPUZZLESTAGE = 3;
            } else {
                for (i = 0; i < 3; i++) {
                    if (GizmoGetOutput(world->gizmo_sys, paintpanel[i], 0, 0) != 0 &&
                        ((GIZOBSTACLE *)paintobst[i]->object)->anim_set->state == 0)
                        GizObstacle_PlayForwards((GIZOBSTACLE *)paintobst[i]->object);
                }
            }
            break;
        case 3: {
            GIZOBSTACLE *first = (GIZOBSTACLE *)paintobst[paintsused[0]]->object;
            if (first->anim_set->state == GAMEANIMSET_STATE_AT_END &&
                ((GIZOBSTACLE *)paintobst[paintsused[1]]->object)->anim_set->state == GAMEANIMSET_STATE_AT_END) {
                GizObstacle_PlayBackwards(first);
                GizObstacle_PlayBackwards((GIZOBSTACLE *)paintobst[paintsused[1]]->object);
                GizObstacle_PlayForwards((GIZOBSTACLE *)paintobst[paintmixed]->object);
            } else if (((GIZOBSTACLE *)paintobst[paintmixed]->object)->anim_set->state == GAMEANIMSET_STATE_AT_END) {
                if (paintmixed == painttarget) {
                    painttry++;
                    if (painttry != 3)
                        PAINTPUZZLESTAGE = 0;
                    else
                        PAINTPUZZLESTAGE = 4;
                } else {
                    RandomisePaintsPc();
                    painttry = 0;
                    PAINTPUZZLESTAGE = 0;
                }
            }
            break;
        }
        case 4:
            GizObstacle_PlayForwards((GIZOBSTACLE *)forcetube->object);
            ((u8 *)factoryb_netpacket)[2] = 1;
            break;
        default:
            break;
    }
    ((u8 *)factoryb_netpacket)[0] = 0;
    ((u8 *)factoryb_netpacket)[1] = (u8)painttry;
    ((u8 *)factoryb_netpacket)[3] = (u8)painttarget;
    if (((u8 *)factoryb_netpacket)[2] == 1 && played_sound == 0) {
        PlaySfx("Fac_BonusCylUp",
                &NuSpecialGetMtx(&((GIZOBSTACLE *)forcetube->object)->anim_set->objects->special)->translation);
        played_sound = 1;
    }
}

void __cdecl InitPaintPuzzle(WORLDINFO *world) {
    i32 i;
    ((u8 *)factoryb_netpacket)[2] = 0;
    for (i = 0; i < 6; i++)
        paintobst[i] = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, paint_name[i]);
    for (i = 0; i < 3; i++)
        paintpanel[i] = GizmoFindByName(world->gizmo_sys, gizpanel_gizmotype_id, paint_panel_name[i]);
    for (i = 0; i < 3; i++)
        NuSpecialFind(world->current_gscn, (NUSPECIALHANDLE *)&painttargetcolour[i], paint_target_colour[i], 0);
    for (i = 0; i < 3; i++)
        NuSpecialFind(world->current_gscn, (NUSPECIALHANDLE *)&paintlights[i], paint_light_name[i], 0);
    forcetube = GizmoFindByName(world->gizmo_sys, obstacle_gizmotype_id, "FORCE_TUBE");
    NuSpecialFind(world->current_gscn, (NUSPECIALHANDLE *)&green_light, "green_light_on1", 1);
    ResetPaintPuzzle(world);
}

void __cdecl FactoryB_Init(WORLDINFO *world) {
    factoryb_netpacket = SetLevelHack(4);
    InitPaintPuzzle(world);
    if (FactoryBConveyorXSpeed == 0.0f && FactoryBConveyorZSpeed == 0.0f) {
        FactoryBConveyorXSpeed = world->current_level->conveyor_x_speed;
        FactoryBConveyorZSpeed = world->current_level->conveyor_z_speed;
    }
    world->current_level->conveyor_x_speed = 0.0f;
    world->current_level->conveyor_z_speed = 0.0f;
}

void __cdecl FactoryB_Reset(WORLDINFO *world) {
    ResetPaintPuzzle(world);
    factoryb_cut = NewCutScene(NULL, world->cutscene_sys, "ep2_droidfactory_conveyor", 0);
    if (factoryb_cut != NULL && factoryb_cut->instance != NULL) {
        ((CUT_INSTANCE_READER_PC *)factoryb_cut->instance)->flags_88 |= 2;
        ((CUT_INSTANCE_READER_PC *)factoryb_cut->instance)->flags_88 |= 8;
    }
    factoryb_conveyor_stopped_msg = CheckGizAIMessage(gizaimessagesys, "conveyor_stop", NULL);
}

void __cdecl FactoryB_Update(WORLDINFO *world) {
    GIZAIMESSAGE *message;
    UpdatePaintPuzzle(world);
    message = factoryb_conveyor_stopped_msg;
    if (message != NULL && message->value != 0.0f && ((CUTINFO_READER_PC *)factoryb_cut)->previous_frame > 50.0 &&
        ((CUTINFO_READER_PC *)factoryb_cut)->previous_frame < 54.0) {
        ((CUT_INSTANCE_READER_PC *)factoryb_cut->instance)->current_frame = 0.0f;
        ((CUT_INSTANCE_READER_PC *)factoryb_cut->instance)->flags_88 &= ~2u;
        world->current_level->conveyor_x_speed = 0.0f;
        world->current_level->conveyor_z_speed = 0.0f;
    } else if (((CUT_INSTANCE_READER_PC *)factoryb_cut->instance)->current_frame > FactoryBConveyorStopFrame) {
        world->current_level->conveyor_x_speed = FactoryBConveyorXSpeed;
        world->current_level->conveyor_z_speed = FactoryBConveyorZSpeed;
        PlaySfx("FacB_BeltLp", NULL);
    } else {
        world->current_level->conveyor_x_speed = 0.0f;
        world->current_level->conveyor_z_speed = 0.0f;
    }
}

void __cdecl FactoryB_Draw(WORLDINFO *world) {
    DrawPaintLights();
}

void __cdecl DookuC_Reset(WORLDINFO *world) {
    memset(&dooku_c, 0, sizeof(dooku_c));
    if (netclient == 0) {
        dooku_c.total_message = SetGizAIMessage(gizaimessagesys, "DookuFight", 0.0f, NULL);
        dooku_c.hit_message = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
    }
    NuSpecialFind(world->current_gscn, &dooku_c.node, "dooku_force", 1);
}

void __cdecl DookuC_Update(WORLDINFO *world) {
    if (netclient == 0) {
        if (FreePlay != 0) {
            KillBossCompleteLevel(id_DOOKU, 0, 0.0f);
        } else if (LevelFixupSlotPc0087b140 != NULL) {
            KillBossNewLevel(id_DOOKU, 0, 0.0f, LevelFixupSlotPc0087b140->index);
        }
    }
    DrawForceBackEffect(&dooku_c.node);
}

void __cdecl DookuC_DrawPanel(WORLDINFO *world) {
    GAMEOBJECT *dooku;
    if (netclient != 0)
        return;
    dooku = FindGameObject(id_DOOKU, 1, 1, 1, 0);
    if (dooku != NULL && dooku_c.hit_message != NULL && dooku_c.hit_message->value == 1.0f)
        DrawBossHitPoints(dooku);
}

void __cdecl KaminoA_AlwaysUpdate(WORLDINFO *world) {
    debris_object_switches[1] =
        CUTSTOPGAME == 0 && GameCam->sock_position.sock_index != 5 && GameCam->sock_position.sock_index != 6;
}

void __cdecl KaminoE_AlwaysUpdate(WORLDINFO *world) {
    debris_object_switches[1] = CUTSTOPGAME != 0 || GameCam->sock_position.sock_index == 0x1e;
}

void __cdecl KaminoOutro_Init(WORLDINFO *world) {
    debris_object_switches[1] =
        CUTSTOPGAME == 0 && GameCam->sock_position.sock_index != 5 && GameCam->sock_position.sock_index != 6;
}

/* Episode II FixUpLevels callbacks (Factory_G, Kamino_C disco floor, Kamino_E Slave 1). */

typedef struct kaminodisco_pc_s {
    AIAREA_PC *area;
    NUSPECIALHANDLE off[16];
    NUSPECIALHANDLE pending[16];
    NUSPECIALHANDLE occupied[16];
    NUSPECIALHANDLE complete[16];
    NUSPECIALHANDLE final[16];
    i8 tile_state[16];
    i8 tile_count;
    i8 phase;
    i8 tile[2];
    f32 timer;
    u8 completion_sound;
    u8 reserved_3dd[3];
    GIZAIMESSAGE *next_tile_message;
    GIZAIMESSAGE *complete_message;
} KAMINODISCO_PC;
STATIC_ASSERT(offsetof(KAMINODISCO_PC, tile_state) == 0x3c4);
STATIC_ASSERT(offsetof(KAMINODISCO_PC, tile_count) == 0x3d4);
STATIC_ASSERT(offsetof(KAMINODISCO_PC, timer) == 0x3d8);
STATIC_ASSERT(offsetof(KAMINODISCO_PC, next_tile_message) == 0x3e0);
STATIC_ASSERT(sizeof(KAMINODISCO_PC) == 0x3e8);

typedef struct kaminoc_packet_pc_s {
    i16 off_mask;
    i16 pending_mask;
    i16 occupied_mask;
    i16 complete_mask;
    i16 final_mask;
    i16 sound_mask;
    u8 complete;
    u8 reserved_0d;
} KAMINOC_PACKET_PC;
STATIC_ASSERT(sizeof(KAMINOC_PACKET_PC) == 0xe);

typedef struct kamino_progress_gizmo_reader_pc_s {
    u8 opaque_00_94[0x94];
    u8 flags;
} KAMINO_PROGRESS_GIZMO_READER_PC;

typedef struct gizforce_reader_pc_s {
    u8 opaque_00_28[0x28];
    GAMEANIMSET *anim_set;
} GIZFORCE_READER_PC;

typedef struct kaminoe_turret_reader_pc_s {
    u8 opaque_00_e4[0xe4];
    NUVEC *target_position;
    u8 opaque_e8_12c[0x44];
    u8 field_12c;
    u8 opaque_12d;
    u8 field_12e;
    u8 opaque_12f_13a[0xb];
    u16 flags_13a;
    u8 opaque_13c_140[0x4];
} KAMINOE_TURRET_READER_PC;
STATIC_ASSERT(offsetof(KAMINOE_TURRET_READER_PC, field_12e) == 0x12e);
STATIC_ASSERT(offsetof(KAMINOE_TURRET_READER_PC, flags_13a) == 0x13a);
STATIC_ASSERT(sizeof(KAMINOE_TURRET_READER_PC) == 0x140);

typedef struct kamino_e_pc_s {
    GIZAIMESSAGE *jango_fight;
    GIZAIMESSAGE *can_fire;
    GIZAIMESSAGE *reset_turrets;
    GIZAIMESSAGE *show_hearts;
    GIZAIMESSAGE *minicut_started;
    CUTINFO *intro_cutscene;
    AIAREA_PC *landing_area;
    NUSPECIALHANDLE slave1;
    KAMINOE_TURRET_READER_PC *turrets[4];
    KAMINOE_TURRET_READER_PC *active_turret;
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
} KAMINO_E_PC;
STATIC_ASSERT(offsetof(KAMINO_E_PC, turrets) == 0x28);
STATIC_ASSERT(offsetof(KAMINO_E_PC, position) == 0x4c);
STATIC_ASSERT(offsetof(KAMINO_E_PC, platform) == 0x76);
STATIC_ASSERT(sizeof(KAMINO_E_PC) == 0x78);

GAMEOBJECT *ObiWan = NULL;
extern i16 CharacterFixupIdPc007f19d0;
KAMINODISCO_PC kaminodisco = {0};
KAMINOC_PACKET_PC *kaminoc_netpacket = NULL;
NUSPECIALHANDLE disco_on_spin[3] = {0};
GIZMO_PC *disco_off_spina[3] = {0};
NUSPECIALHANDLE walllights[2] = {0};
NUSPECIALHANDLE walllights_disco[2] = {0};
NUSPECIALHANDLE striplights[2] = {0};
NUSPECIALHANDLE discolights[2] = {0};
NUSPECIALHANDLE discorm_wall_on = {0};
NUSPECIALHANDLE discorm_wall_off = {0};
GIZMO_PC *gizTurrets[2] = {0};
i32 last = 0;
static f32 KaminoDiscoTileTimePc = 2.5f;
static f32 KaminoDiscoTileRadiusPc = 0.2f;
KAMINO_E_PC kamino_e = {0};
extern i32 turret_gizmotype_id;

void *__cdecl GizmoFindByData(void *gizmo_sys, i32 type_id, void *data);
void __cdecl KaminoE_Update(WORLDINFO *world);

void __cdecl KaminoEGizmoMissingPc(char *name);

void __cdecl FactoryG_Update(WORLDINFO *world) {
    i32 complete = 0;
    i32 i;
    if (netclient != 0)
        return;
    for (i = 0; i < 4; i++) {
        if (GizForce_Complete((GIZFORCE_PC *)force_array[i]) != 0)
            complete++;
    }
    if (ObiWan != NULL && complete != 4) {
        f32 lamp;
        ObiWan->position.x = 112.76f;
        ObiWan->position.y = 0.75f;
        ObiWan->position.z = -10.5f;
        lamp = (f32)(NuTrigTable[(u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 2.0f) * NuQFntHalfScale *
                                            VehicleTurnPhaseScale) >>
                                 1] *
                     30.0);
        ObiWan->field_0x276 = (u16)(0xc000 - (i16)(i32)((f64)lamp * -182.0444488525390625));
        return;
    }
    if (ObiWan == NULL) {
        ObiWan = FindGameObject(CharacterFixupIdPc007f19d0, 0x400, 0, 1, 0);
        return;
    }
    if (FreePlay != 0) {
        CompleteLevel(WORLD);
    } else {
        CUTINFO *cut = NewCutScene(NULL, world->cutscene_sys, "ep2_factory_outro", 1);
        ((CUT_INSTANCE_READER_PC *)((CUTINFO_READER_PC *)cut)->instance)->flags_88 |= 2;
    }
}

#define KAMINO_DISCO_AREA_MASK_SET_PC(world, area)                                                                     \
    ((*(u64 *)((GAMEOBJECT *)(world)->ai_sys->player_1)->ai_area_mask &                                                \
      (u64)(i64)(1 << (i32)((area) - (world)->ai_sys->areas))) != 0)

#define KAMINO_DISCO_PLAYER_IN_AREA_PC(world, who, area)                                                               \
    ((who) != NULL && (area) != NULL && KAMINO_DISCO_AREA_MASK_SET_PC(world, area))

void __cdecl KaminoC_Init(WORLDINFO *world) {
    char name[32];
    i32 i;
    memset(&kaminodisco, 0, sizeof(kaminodisco));
    kaminoc_netpacket = SetLevelHack(sizeof(KAMINOC_PACKET_PC));
    kaminodisco.tile_count = 0;
    do {
        if (kaminodisco.tile_count < 9)
            sprintf(name, "dot_off_0%d", kaminodisco.tile_count + 1);
        else
            sprintf(name, "dot_off_%d", kaminodisco.tile_count + 1);
        NuSpecialFind(WORLD->current_gscn, &kaminodisco.off[kaminodisco.tile_count], name, 1);
        if (kaminodisco.tile_count < 9)
            sprintf(name, "dot_flash_0%d", kaminodisco.tile_count + 1);
        else
            sprintf(name, "dot_flash_%d", kaminodisco.tile_count + 1);
        NuSpecialFind(WORLD->current_gscn, &kaminodisco.pending[kaminodisco.tile_count], name, 1);
        if (kaminodisco.tile_count < 9)
            sprintf(name, "dot_select_0%d", kaminodisco.tile_count + 1);
        else
            sprintf(name, "dot_select_%d", kaminodisco.tile_count + 1);
        NuSpecialFind(WORLD->current_gscn, &kaminodisco.occupied[kaminodisco.tile_count], name, 1);
        if (kaminodisco.tile_count < 9)
            sprintf(name, "dot_finish_0%d", kaminodisco.tile_count + 1);
        else
            sprintf(name, "dot_finish_%d", kaminodisco.tile_count + 1);
        NuSpecialFind(WORLD->current_gscn, &kaminodisco.complete[kaminodisco.tile_count], name, 1);
        if (kaminodisco.tile_count < 9)
            sprintf(name, "dot_on_0%d", kaminodisco.tile_count + 1);
        else
            sprintf(name, "dot_on_%d", kaminodisco.tile_count + 1);
        NuSpecialFind(WORLD->current_gscn, &kaminodisco.final[kaminodisco.tile_count], name, 1);
        if (NuSpecialExistsFn(&kaminodisco.off[kaminodisco.tile_count]) == 0 ||
            NuSpecialExistsFn(&kaminodisco.pending[kaminodisco.tile_count]) == 0 ||
            NuSpecialExistsFn(&kaminodisco.occupied[kaminodisco.tile_count]) == 0 ||
            NuSpecialExistsFn(&kaminodisco.complete[kaminodisco.tile_count]) == 0)
            break;
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

void __cdecl KaminoC_Reset(WORLDINFO *world) {
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
            if (((GIZFORCE_READER_PC *)disco_off_spina[i]->object)->anim_set->state == 0)
                GizmoActivate(world->gizmo_sys, disco_off_spina[i], 1, 0);
        }
    }
}

static i32 KaminoDiscoChooseTilePc(i32 state, i32 excluded) {
    i32 candidates[16];
    i32 count = 0;
    i32 i;
    for (i = 0; i < kaminodisco.tile_count; i++) {
        if (i != excluded && kaminodisco.tile_state[i] == state)
            candidates[count++] = i;
    }
    return count != 0 ? candidates[NuRand(NULL) % count] : -1;
}

static void KaminoDiscoUpdateVisualsPc(WORLDINFO *world) {
    i32 i;
    for (i = 0; i < kaminodisco.tile_count; i++) {
        i32 bit = 1 << i;
        NuSpecialSetVisibility(&kaminodisco.off[i], kaminoc_netpacket->off_mask & bit);
        NuSpecialSetVisibility(&kaminodisco.pending[i], kaminoc_netpacket->pending_mask & bit);
        NuSpecialSetVisibility(&kaminodisco.occupied[i], kaminoc_netpacket->occupied_mask & bit);
        NuSpecialSetVisibility(&kaminodisco.final[i], kaminoc_netpacket->final_mask & bit);
        NuSpecialSetVisibility(&kaminodisco.complete[i], kaminoc_netpacket->complete_mask & bit);
        if ((kaminoc_netpacket->sound_mask & bit) != 0)
            PlaySfx("Kam_DiscoFloorPanelOn", (NUVEC *)&NuSpecialGetDrawMtx(&kaminodisco.complete[i])->translation);
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

void __cdecl KaminoC_Update(WORLDINFO *world) {
    KAMINO_PROGRESS_GIZMO_READER_PC *progress = (KAMINO_PROGRESS_GIZMO_READER_PC *)LevGizmo[0]->object;
    i32 i;
    if (GizmoGetOutput(world->gizmo_sys, LevGizmo[0], 0, 0) == 0) {
        if ((progress->flags & 1) == 0) {
            GizmoActivate(world->gizmo_sys, LevGizmo[1], 1, 1);
            if (GizmoGetOutput(world->gizmo_sys, gizTurrets[0], 0, 0) != 0 &&
                GizmoGetOutput(world->gizmo_sys, gizTurrets[1], 0, 0) != 0)
                GizmoSetVisibility(world->gizmo_sys, LevGizmo[1], 0, 1);
        }
    } else {
        GizmoSetVisibility(world->gizmo_sys, LevGizmo[1], 0, 1);
    }
    kaminoc_netpacket->sound_mask = 0;
    SetGizAIMessage(gizaimessagesys, "NextDiscoTile", 0.0f, kaminodisco.next_tile_message);
    SetGizAIMessage(gizaimessagesys, "DiscoComplete", 0.0f, kaminodisco.complete_message);
    switch (kaminodisco.phase) {
        case 0:
            if (KAMINO_DISCO_PLAYER_IN_AREA_PC(world, world->ai_sys->player_1, kaminodisco.area) ||
                KAMINO_DISCO_PLAYER_IN_AREA_PC(world, world->ai_sys->player_2, kaminodisco.area)) {
                kaminoc_netpacket->complete = 0;
                kaminodisco.timer = 0.0f;
                kaminodisco.phase = 1;
                kaminodisco.tile[0] = (i8)KaminoDiscoChooseTilePc(0, -1);
                kaminodisco.tile[1] = (i8)KaminoDiscoChooseTilePc(0, kaminodisco.tile[0]);
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
                    GAMEOBJECT *object = (GAMEOBJECT *)Player[i];
                    if (object != NULL && (object->flags_1fc_word & 1) != 0 && (object->flags_1fc_word & 0x1000) != 0 &&
                        object->field_281 != 0) {
                        f32 dx = position->x - object->position.x;
                        f32 dy = position->y - object->position.y;
                        f32 dz = position->z - object->position.z;
                        f32 dist = dx * dx + dy * dy + dz * dz;
                        if (dist < KaminoDiscoTileRadiusPc * KaminoDiscoTileRadiusPc) {
                            occupied[k] = 1;
                            kaminodisco.tile_state[kaminodisco.tile[k]] = 2;
                            if (object == player)
                                occupied_by_player = kaminodisco.tile[k];
                            break;
                        }
                    }
                }
                if (old_state != kaminodisco.tile_state[kaminodisco.tile[k]] &&
                    kaminodisco.tile_state[kaminodisco.tile[k]] == 2)
                    kaminoc_netpacket->sound_mask |= 1 << kaminodisco.tile[k];
            }
            if (occupied[0] != 0 && occupied[1] != 0) {
                kaminodisco.timer = 0.0f;
                kaminodisco.tile_state[kaminodisco.tile[0]] = 3;
                kaminodisco.tile_state[kaminodisco.tile[1]] = 3;
                kaminodisco.tile[0] = (i8)KaminoDiscoChooseTilePc(0, -1);
                kaminodisco.tile[1] = (i8)KaminoDiscoChooseTilePc(0, kaminodisco.tile[0]);
                if (kaminodisco.tile[0] != -1 && kaminodisco.tile[1] != -1) {
                    kaminodisco.tile_state[kaminodisco.tile[0]] = 1;
                    kaminodisco.tile_state[kaminodisco.tile[1]] = 1;
                } else {
                    kaminodisco.phase = 2;
                    for (i = 0; i < kaminodisco.tile_count; i++)
                        kaminodisco.tile_state[i] = 0;
                }
                break;
            }
            kaminodisco.timer += FRAMETIME;
            if (kaminodisco.timer > KaminoDiscoTileTimePc) {
                kaminodisco.timer = 0.0f;
                for (i = 0; i < 2; i++) {
                    i32 tile = KaminoDiscoChooseTilePc(3, -1);
                    if (tile != -1)
                        kaminodisco.tile_state[tile] = 0;
                }
            }
            if (player2 == NULL) {
                GAMEOBJECT *companion = (GAMEOBJECT *)Player[0];
                if (player == companion)
                    companion = (GAMEOBJECT *)Player[1];
                if (companion != NULL) {
                    if (occupied_by_player == kaminodisco.tile[0])
                        SetGizAIMessage(gizaimessagesys, "NextDiscoTile", (f32)(kaminodisco.tile[1] + 1),
                                        kaminodisco.next_tile_message);
                    else if (occupied_by_player == kaminodisco.tile[1])
                        SetGizAIMessage(gizaimessagesys, "NextDiscoTile", (f32)(kaminodisco.tile[0] + 1),
                                        kaminodisco.next_tile_message);
                }
            }
            break;
        }
        case 2:
            SetGizAIMessage(gizaimessagesys, "DiscoComplete", 1.0f, kaminodisco.complete_message);
            kaminoc_netpacket->complete = 1;
            if (!(KAMINO_DISCO_PLAYER_IN_AREA_PC(world, world->ai_sys->player_1, kaminodisco.area) ||
                  KAMINO_DISCO_PLAYER_IN_AREA_PC(world, world->ai_sys->player_2, kaminodisco.area))) {
                KaminoC_Reset(world);
                return;
            }
            for (i = 0; i < kaminodisco.tile_count; i++)
                kaminodisco.tile_state[i] = 4;
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
    KaminoDiscoUpdateVisualsPc(world);
}

void __cdecl KaminoE_Reset(WORLDINFO *world) {
    char name[16];
    i32 i;
    memset(&kamino_e, 0, sizeof(kamino_e));
    kamino_e.landing_area = AISysFindArea(WORLD->ai_sys, "landing_pad");
    if (NuSpecialFind(world->current_gscn, &kamino_e.slave1, "slave1", 1) != 0)
        kamino_e.platform = (i16)FindPlatInst(NuSpecialGetInstanceix(&kamino_e.slave1));
    for (i = 0; i < 4; i++) {
        GIZMO_PC *gizmo;
        sprintf(name, "turret%d", i + 1);
        gizmo = GizmoFindByName(world->gizmo_sys, turret_gizmotype_id, name);
        if (gizmo != NULL) {
            kamino_e.turrets[i] = (KAMINOE_TURRET_READER_PC *)gizmo->object;
            kamino_e.turrets[i]->target_position = &kamino_e.position;
            kamino_e.turrets[i]->field_12c = 2;
            kamino_e.turrets[i]->flags_13a |= 1;
        } else {
            KaminoEGizmoMissingPc(name);
        }
        sprintf(name, "R4_t%d", i + 1);
        gizmo = GizmoFindByName(world->gizmo_sys, gizpanel_gizmotype_id, name);
        if (gizmo != NULL)
            kamino_e.panels[i] = gizmo->object;
        else
            KaminoEGizmoMissingPc(name);
    }
    kamino_e.jango_fight = SetGizAIMessage(gizaimessagesys, "JangoFight", 0.0f, NULL);
    kamino_e.can_fire = SetGizAIMessage(gizaimessagesys, "Slave1CanFire", 0.0f, NULL);
    kamino_e.reset_turrets = SetGizAIMessage(gizaimessagesys, "ResetTurrets", 0.0f, NULL);
    kamino_e.show_hearts = CheckGizAIMessage(gizaimessagesys, "ShowHearts", NULL);
    kamino_e.minicut_started = CheckGizAIMessage(gizaimessagesys, "MiniCutStarted", NULL);
    CutScene_Find(world->cutscene_sys, "Ep2_Kamino_Intro2");
    kamino_e.intro_cutscene = CutScene_Find(world->cutscene_sys, "Ep2_Kamino_Intro2");
}

static void KaminoEResetTurretsPc(void) {
    i32 i;
    kamino_e.active_turret = NULL;
    for (i = 0; i < 4; i++) {
        GizmoActivate(WORLD->gizmo_sys, GizmoFindByData(WORLD->gizmo_sys, gizpanel_gizmotype_id, kamino_e.panels[i]), 1,
                      1);
        kamino_e.turrets[i]->field_12e = 1;
        kamino_e.turrets[i]->flags_13a &= ~0x10;
    }
}

void __cdecl KaminoE_UpdatePc(WORLDINFO *world) {
    GAMEOBJECT *jango;
    GAMEOBJECT *other;
    GAMEOBJECT *target;
    i32 direction;
    u32 *connection;
    AIAREA_PC *fight_area;
    AILOCATOR_PC *wait;
    AISYS_PC *ai;
    if (kamino_e.reset_turrets != NULL && kamino_e.reset_turrets->value == 1.0f) {
        KaminoEResetTurretsPc();
        kamino_e.reset_turrets->value = 0.0f;
    }
    if (netclient != 0)
        return;
    jango = FindGameObject(id_JANGOFETT, 1, 1, 1, 0);
    other = FindGameObject(id_JANGOFETT, 4, 1, 1, 0);
    target = NULL;
    if (jango != NULL)
        target = jango;
    else if (other != NULL)
        target = other;
    direction = 0;
    if (jango != NULL && jango->current_hp < 1) {
        if (FreePlay != 0)
            KillBossCompleteLevel(id_JANGOFETT, 0, 0.0f);
        else
            KillBossNewLevel(id_JANGOFETT, 0, 0.0f, LevelFixupSlotPc0087b138->index);
    }
    if (kamino_e.jango_fight->value == 0.0f && FreePlay == 0 && kamino_e.intro_cutscene != NULL &&
        instNuGCutSceneIsFinished((INSTNUGCUTSCENE *)((CUTINFO_READER_PC *)kamino_e.intro_cutscene)->instance) != 0)
        kamino_e.jango_fight->value = 1.0f;
    STUBBED_NAMED("KaminoE_Update");
    connection =
        AIPAthFindPathCnx(world->ai_sys, world->ai_sys->path_sys->active_path, "Bridge1_a", "Bridge1_b", &direction);
    fight_area = AISysFindArea(WORLD->ai_sys, "Fight");
    if (target == NULL || connection == NULL || fight_area == NULL)
        return;
    if ((connection[direction] & 0x80000000) == 0 && (connection[direction == 0] & 0x80000000) == 0)
        return;
    ai = world->ai_sys;
    if ((*(u64 *)target->ai_area_mask & (u64)(i64)(1 << (i32)(fight_area - ai->areas))) != 0)
        return;
    wait = AIPathFindLocator(ai, "WAIT");
    if (wait == NULL)
        return;
    target->position = wait->position;
    target->field_0x276 = wait->rotation;
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

/* Reads the Kamino_E state block (show_hearts at 0x88903c, slave1 at 0x88904c). */
void __cdecl KaminoE_Draw(WORLDINFO *world) {
    if (netclient == 0 && kamino_e.show_hearts->value > 0.0f) {
        GAMEOBJECT *jango = FindGameObject(id_JANGOFETT, 1, 1, 1, 0);
        if (jango != NULL && kamino_e.show_hearts != NULL && kamino_e.show_hearts->value == 1.0f)
            DrawBossHitPoints(jango);
    }
    NuSpecialSetDrawMtx(&kamino_e.slave1, NuSpecialGetDrawMtx(&kamino_e.slave1));
    NuSpecialSetVisibility(&kamino_e.slave1, 1);
}
