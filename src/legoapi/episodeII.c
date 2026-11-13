#include <stdio.h>
#include <string.h>
#include "saga.h"

STATIC_ASSERT(offsetof(VADERA_PC, in_control_room_message) == 0x00);

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
    /* MATCH-EXCEPTION: retail scans three entries of the two-entry boulder_part array; the third read is
       the first word of the next global (vader_a), exactly as the original binary does. */
    for (index = 0; index < 3; ++index) {
        if (boulder_part[index] != NULL) {
            f32 distance = NuVecDistSqr(&object->collision_position, &boulder_part[index]->position, &difference);
            if (distance < nearest_distance) {
                nearest_distance = distance;
                nearest = boulder_part[index];
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

NUVEC TempleC_StreamStatusPos = {52.57f, 0.76f, -16.54f};

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

typedef struct kaminoe_turret_reader_pc_s {
    u8 opaque_00_1c[0x1c];
    GAMEANIMOBJ *primary_anim_obj;
    u8 opaque_20_e4[0xc4];
    NUVEC *target_position;
    u8 opaque_e8_12c[0x44];
    u8 field_12c;
    u8 opaque_12d;
    u8 field_12e;
    u8 opaque_12f_13a[0xb];
    u16 flags_13a;
    u8 opaque_13c_140[0x4];
} KAMINOE_TURRET_READER_PC;
STATIC_ASSERT(offsetof(KAMINOE_TURRET_READER_PC, primary_anim_obj) == 0x1c);
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

#define KAMINO_DISCO_AREA_MASK_SET_PC(world, area)                                                                     \
    ((*(u64 *)((GAMEOBJECT *)(world)->ai_sys->player_1)->ai_area_mask &                                                \
      (u64)(i64)(1 << (i32)((area) - (world)->ai_sys->areas))) != 0)

typedef struct KAMINO_SHIP_ZERO_ANGLES_PC {
    u16 pitch;
    i32 yaw;
} KAMINO_SHIP_ZERO_ANGLES_PC;
STATIC_ASSERT(offsetof(KAMINO_SHIP_ZERO_ANGLES_PC, pitch) == 0);
STATIC_ASSERT(offsetof(KAMINO_SHIP_ZERO_ANGLES_PC, yaw) == 4);
STATIC_ASSERT(sizeof(KAMINO_SHIP_ZERO_ANGLES_PC) == 8);
