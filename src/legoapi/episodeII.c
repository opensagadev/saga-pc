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
