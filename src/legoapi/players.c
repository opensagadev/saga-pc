#include "saga.h"

STATIC_ASSERT(offsetof(GAMEOBJECT, field_e22) == 0xe22);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1062) == 0x1062);

i32 __cdecl MakePlayerList(i32 count) {
    i32 player_count = 0;
    i32 i;
    GAMEOBJECT *player;
    PLAYERPROGRESS *progress;
    for (i = 0; i < count; ++i) {
        player = Player[i];
        if (player == NULL)
            continue;
        if (makeplayerlist_freeplay == 1)
            PlayerList[player_count] = FreePlayModelList[i].model_id;
        else if (makeplayerlist_freeplay != 2)
            PlayerList[player_count] = player->id;
        progress = &PlayerProgress[player_count];
        progress->active = player->player_controlled;
        if (player->character_context != 6 && player->field_e33 != 2 &&
            ((player->field_e22 & 1) != 0 || player->field_e33 != 0))
            progress->field_7 = 1;
        else
            progress->field_7 = 0;
        if (player->coin_packet != NULL)
            progress->coins = ((PLAYER_COIN_PACKET_PC *)player->coin_packet)->coins;
        else
            progress->coins = 0;
        progress->field_b = player->jump_variant_state == 1;
        if (player->dead != 0) {
            progress->hitpoints = player->field_107e;
            progress->field_9 = 4;
        } else {
            progress->hitpoints = player->current_hp;
            progress->field_9 = player->saved_hitpoints;
        }
        progress->field_a = player->helmet;
        progress->unknown_0004 = player->field_1062;
        progress->field_c = player->powerup_time;
        PlayerSuit[player_count] = player->suit;
        if (player->torpedo != NULL)
            PlayerTorpedoCount[player_count] = player->torpedo->count;
        else
            PlayerTorpedoCount[player_count] = 0;
        ++player_count;
    }
    makeplayerlist_freeplay = 0;
    for (i = player_count; i < 8; ++i)
        PlayerList[i] = -1;
    return player_count;
}

extern i16 LEGOACT_CLIMB_UP, LEGOACT_CLIMB_DOWN, LEGOACT_CLIMB_LEFT, LEGOACT_CLIMB_RIGHT;
extern i16 LEGOACT_WALLSHUFFLE_IDLE, LEGOACT_WALLSHUFFLE_LEFT, LEGOACT_WALLSHUFFLE_RIGHT;
extern i16 LEGOACT_MAGNET_WALK_METAL;

typedef struct grapple_movement_pc_s {
    u8 unknown_00[0x3c];
    NUVEC shadow_probe_position;
    u8 unknown_48[4];
    f32 swing_scale;
} GRAPPLE_MOVEMENT_PC;
STATIC_ASSERT(offsetof(GRAPPLE_MOVEMENT_PC, shadow_probe_position) == 0x3c);
STATIC_ASSERT(offsetof(GRAPPLE_MOVEMENT_PC, swing_scale) == 0x4c);

i32 __cdecl WallShuffle_SetTargetMom(GAMEOBJECT *object, u16 input_angle) {
    u16 sideways_angle;
    f32 direction, speed;
    object->target_velocity.x = NuTrigTable[object->movement_facing_angle >> 1];
    object->target_velocity.z = NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff];
    sideways_angle = object->movement_facing_angle + 0x4000;
    direction = PushingTowardsAngle(input_angle, sideways_angle);
    if (direction > NuTrigTable[0x3555]) {
        if (LEGOACT_WALLSHUFFLE_RIGHT != -1 &&
            ((CHARACTER_MODEL_LAYERS_PC *)object->character_model)->model_data_b[LEGOACT_WALLSHUFFLE_RIGHT] != NULL) {
            object->field_7aa = LEGOACT_WALLSHUFFLE_RIGHT;
            speed = AnimSpeed((CHARACTER_MODEL_LAYERS_PC *)object->character_model, LEGOACT_WALLSHUFFLE_RIGHT);
        } else {
            speed = 0.5f;
        }
    } else if (direction < -NuTrigTable[0x3555]) {
        if (LEGOACT_WALLSHUFFLE_LEFT != -1 &&
            ((CHARACTER_MODEL_LAYERS_PC *)object->character_model)->model_data_b[LEGOACT_WALLSHUFFLE_LEFT] != NULL) {
            object->field_7aa = LEGOACT_WALLSHUFFLE_LEFT;
            speed = -AnimSpeed((CHARACTER_MODEL_LAYERS_PC *)object->character_model, LEGOACT_WALLSHUFFLE_LEFT);
        } else {
            speed = -0.5f;
        }
    } else {
        object->field_7aa = LEGOACT_WALLSHUFFLE_IDLE;
        speed = 0.0f;
    }
    if (speed != 0.0f) {
        object->target_velocity.x += NuTrigTable[sideways_angle >> 1] * speed;
        object->target_velocity.z += NuTrigTable[((sideways_angle + 0x4000) >> 1) & 0x7fff] * speed;
    }
    return 0;
}

i32 __cdecl Climb_SetTargetMom(GAMEOBJECT *object, u16 input_angle) {
    union {
        f32 value;
        u32 bits;
    } raw_speed, speed;
    i16 animation = object->field_7aa;
    u16 angle, side_angle;
    f32 lateral;
    speed.value = 0.0f;
    if (animation != -1 &&
        (animation == LEGOACT_CLIMB_UP || animation == LEGOACT_CLIMB_DOWN || animation == LEGOACT_CLIMB_LEFT ||
         animation == LEGOACT_CLIMB_RIGHT || animation == LEGOACT_MAGNET_WALK_METAL)) {
        if (((CHARACTER_MODEL_LAYERS_PC *)object->character_model)->model_data_b[animation] != NULL) {
            raw_speed.value = AnimSpeed((CHARACTER_MODEL_LAYERS_PC *)object->character_model, animation);
            speed.bits = raw_speed.bits & 0x7fffffff;
        }
    }
    if (speed.value == 0.0f)
        speed.value = 0.5f;
    angle = object->movement_facing_angle;
    object->target_velocity.x = NuTrigTable[angle >> 1] * speed.value;
    object->target_velocity.y = 0.0f;
    object->target_velocity.z = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * speed.value;
    if (object->pad->input_magnitude > 0.0f) {
        side_angle = angle + 0x4000;
        lateral = PushingTowardsAngle(input_angle, side_angle);
        object->target_velocity.x += NuTrigTable[side_angle >> 1] * lateral * speed.value;
        object->target_velocity.z += lateral * NuTrigTable[((side_angle + 0x4000) >> 1) & 0x7fff] * speed.value;
        object->target_velocity.y += PushingTowardsAngle(input_angle, object->movement_facing_angle) * speed.value;
    }
    return 1;
}

extern i16 LEGOACT_SUPERCARRY_WALK;
extern const f64 TerrainSkinSampleCountPc;
extern const f32 PlayerPanelVariantZeroX, PlayerPanelVariantZeroZ, PlayerPanelVariantOneX, PlayerPanelVariantOneZ;

__declspec(noinline) void __cdecl SpecialMove_Attacker_SetTargetMom(GAMEOBJECT *object) {
    i32 move = object->special_move_pc;
    f32 distance = move != -1 ? PlayerSpecialMoveDataPc[move].distance_apart : 0.0f;
    NUVEC target;
    target.x = object->force_target->position.x - NuTrigTable[object->movement_facing_angle >> 1] * distance;
    target.z = object->force_target->position.z -
               NuTrigTable[((object->movement_facing_angle + 0x4000) >> 1) & 0x7fff] * distance;
    object->velocity.x = object->target_velocity.x = (target.x - object->position.x) * 10.0;
    object->velocity.z = object->target_velocity.z = (target.z - object->position.z) * 10.0;
}

typedef struct blowup_movement_pc_s {
    u8 unknown_00[0x4c];
    NUVEC mid_position;
    u8 unknown_58[0x50];
    f32 target_radius;
} BLOWUP_MOVEMENT_PC;
STATIC_ASSERT(offsetof(BLOWUP_MOVEMENT_PC, mid_position) == 0x4c);
STATIC_ASSERT(offsetof(BLOWUP_MOVEMENT_PC, target_radius) == 0xa8);

typedef struct tightrope_movement_pc_s {
    u8 unknown_00[0x40];
    f32 horizontal_length;
    u16 rotation;
} TIGHTROPE_MOVEMENT_PC;
STATIC_ASSERT(offsetof(TIGHTROPE_MOVEMENT_PC, horizontal_length) == 0x40);
STATIC_ASSERT(offsetof(TIGHTROPE_MOVEMENT_PC, rotation) == 0x44);

__declspec(noinline) i32 __cdecl SuperCarry_SetTargetMom(GAMEOBJECT *object, f32 input_speed) {
    f32 speed = 0.0f;
    object->target_velocity.x = 0.0f;
    object->target_velocity.z = 0.0f;
    if (input_speed > 0.0f) {
        if ((object->field_7b3 == 3 || object->field_7b3 == 1) && object->field_7aa != -1 &&
            object->character_model->model_data_b[object->field_7aa] != NULL) {
            speed = AnimSpeed(object->character_model, object->field_7aa);
        } else if (object->field_7b3 == 3 || object->field_7b3 == 6) {
            if (object->field_7b3 == 6 && LEGOACT_SUPERCARRY_WALK != 0 &&
                object->character_model->model_data_b[LEGOACT_SUPERCARRY_WALK] != NULL)
                speed = AnimSpeed(object->character_model, LEGOACT_SUPERCARRY_WALK);
            else
                speed = NuQFntHalfScale * object->character_data->game_character->run_speed;
        }
        if (speed > 0.0f) {
            object->target_velocity.x = NuTrigTable[object->facing_angle >> 1] * speed;
            object->target_velocity.z = NuTrigTable[((object->facing_angle + 0x4000) >> 1) & 0x7fff] * speed;
        }
    }
    return 0;
}

__declspec(noinline) i32 __cdecl SuperCarry_YRotation(GAMEOBJECT *object, u16 input_angle) {
    if (object->field_7b3 == 0) {
        object->facing_angle = SeekRot(object->facing_angle, object->movement_facing_angle, 12.0f);
        object->field_0x276 = object->facing_angle;
    } else if (object->field_7b3 == 5) {
        if (object->force_target != NULL)
            object->movement_facing_angle =
                NuAtan2D(object->force_target->collision_position.x - object->collision_position.x,
                         object->force_target->collision_position.z - object->collision_position.z);
        object->facing_angle = SeekRot(object->facing_angle, object->movement_facing_angle, 8.0f);
        object->field_0x276 = object->facing_angle;
    } else if (object->field_7b3 == 2 || object->field_7b3 == 3 || object->field_7b3 == 6) {
        if (object->pad->input_magnitude > 0.0f)
            object->facing_angle = TurnRot(
                object->facing_angle, input_angle,
                (i32)(VehicleTurnPhaseOffset * object->context_hover_target_y * TerrainSkinSampleCountPc), NULL);
        object->field_0x276 = SeekRot(object->field_0x276, object->facing_angle, 10.0f);
        object->movement_facing_angle = object->facing_angle;
    }
    return 1;
}

__declspec(noinline) i32 __cdecl SuperCarry_Carrying(GAMEOBJECT *object) {
    if (LEGOCONTEXT_SUPERCARRY != -1 && object->character_context == LEGOCONTEXT_SUPERCARRY) {
        if (object->field_7b3 == 2 || object->field_7b3 == 3 || object->field_7b3 == 6)
            return 1;
    }
    return 0;
}

__declspec(noinline) i32 __cdecl Slide_SetTargetMom(GAMEOBJECT *object, u16 input_angle, f32 input_speed) {
    u16 direction;
    NUVEC lateral;
    union {
        f32 value;
        u32 bits;
    } magnitude, absolute;
    if (object->field_281 != 0 && ((magnitude.value = object->surface_normal.x,
                                    absolute.bits = magnitude.bits & 0x7fffffff, absolute.value > 0.001f) ||
                                   (magnitude.value = object->surface_normal.z,
                                    absolute.bits = magnitude.bits & 0x7fffffff, absolute.value > 0.001f)))
        direction = NuAtan2D(object->surface_normal.x, object->surface_normal.z);
    else
        direction = object->movement_facing_angle;
    object->target_velocity.x = NuTrigTable[direction >> 1] * 1.2f;
    object->target_velocity.z = NuTrigTable[((direction + 0x4000) >> 1) & 0x7fff] * 1.2f;
    if (object->field_281 != 0 && input_speed > 0.0f) {
        lateral.x = NuTrigTable[input_angle >> 1] * input_speed;
        lateral.z = NuTrigTable[((input_angle + 0x4000) >> 1) & 0x7fff] * input_speed;
        NuVecRotateY(&lateral, &lateral, -(i32)direction);
        lateral.z = 0.0f;
        NuVecRotateY(&lateral, &lateral, direction);
        object->target_velocity.x += lateral.x;
        object->target_velocity.z += lateral.z;
    }
    return 0;
}

i32 __cdecl CanClimbSurface(GAMEOBJECT *, i32);
extern i32(__cdecl *CanMagnetClimbFn)(GAMEOBJECT *);
extern i32 LEGOCONTEXT_PUSHSPINNER, PlayerPushSpinnerDegreesPc;
extern i16 LEGOACT_WALLSHUFFLE_LEFT, LEGOACT_WALLSHUFFLE_RIGHT;

typedef struct spinner_movement_pc_s {
    u8 unknown_00[0x84];
    u32 state_flags;
} SPINNER_MOVEMENT_PC;
STATIC_ASSERT(offsetof(SPINNER_MOVEMENT_PC, state_flags) == 0x84);

typedef struct world_conveyor_pc_s {
    u8 unknown_00[0x128];
    LEVELDATA_PC *current_level;
    u8 unknown_12c[0x5044];
    void *conveyor;
} WORLD_CONVEYOR_PC;
STATIC_ASSERT(offsetof(WORLD_CONVEYOR_PC, current_level) == 0x128);
STATIC_ASSERT(offsetof(WORLD_CONVEYOR_PC, conveyor) == 0x5170);

__declspec(noinline) i32 __cdecl CanClimbSurface(GAMEOBJECT *object, i32 surface) {
    if ((u32)surface > 31 || (TerSurface[surface].flags & 0x10000) == 0)
        return 0;
    if (surface == 5 && (CanMagnetClimbFn == NULL || CanMagnetClimbFn(object) == 0))
        return 0;
    return 1;
}

static __inline f32 PlayersFabsPc(f32 value) {
    u32 bits = *(u32 *)&value & 0x7fffffff;
    return *(f32 *)&bits;
}

__declspec(noinline) i32 __cdecl Pushing(GAMEOBJECT *object, u16 *normal_angle, i32 *surface, i32 *angle_difference) {
    i32 pushing_obstacle, difference;
    u16 input_angle, angle;
    if ((LEGOCONTEXT_PUSHOBSTACLE != -1 && object->character_context == LEGOCONTEXT_PUSHOBSTACLE) ||
        (LEGOCONTEXT_JUMP != -1 && object->character_context == LEGOCONTEXT_JUMP && object->action_movement_state == 9))
        pushing_obstacle = 1;
    else
        pushing_obstacle = 0;
    if (((object->flags_1fc & 0x80) || (object->flags_ef4 & 0x30000)) &&
        (object->pad->input_magnitude > 0.0f || pushing_obstacle) && object->contact_count &&
        CanClimbSurface(object, (i8)object->contact_material[0]) == 0 &&
        PlayersFabsPc(object->contact_normal.y) < NuTrigTable[0x3c71]) {
        input_angle = pushing_obstacle ? object->movement_facing_angle : GamePad_InputAngle(object, object->pad);
        angle = NuAtan2D(object->contact_normal.x, object->contact_normal.z);
        difference = RotDiff(input_angle, angle);
        if (angle_difference != NULL)
            *angle_difference = difference;
        difference = abs(difference);
        if (difference > 0x4e38 ||
            (difference > 0x1555 && object->field_7aa != -1 &&
             (object->field_7aa == LEGOACT_WALLSHUFFLE_RIGHT || object->field_7aa == LEGOACT_WALLSHUFFLE_LEFT))) {
            if (normal_angle != NULL)
                *normal_angle = angle;
            if (surface != NULL)
                *surface = (i8)object->contact_material[0];
            return 1;
        }
    }
    if (surface != NULL)
        *surface = -1;
    return 0;
}

void __cdecl SetPushAngle(GAMEOBJECT *object) {
    SPINNER_MOVEMENT_PC *spinner;
    u16 angle;
    i32 adjustment;
    if (Pushing(object, &angle, NULL, NULL)) {
        adjustment = 0;
        spinner = (SPINNER_MOVEMENT_PC *)object->grapple;
        if (spinner != NULL && LEGOCONTEXT_PUSHSPINNER != -1 && object->character_context == LEGOCONTEXT_PUSHSPINNER &&
            (spinner->state_flags & 0x40)) {
            adjustment = PlayerPushSpinnerDegreesPc;
            if (spinner->state_flags & 1)
                adjustment = -adjustment;
        }
        object->movement_facing_angle = angle + ((adjustment + 180) * 65536) / 360;
    }
}

i32 __cdecl Conveyor_AdjustSpeed(NUVEC *velocity) {
    WORLD_CONVEYOR_PC *world = (WORLD_CONVEYOR_PC *)WorldInfo_CurrentlyActive();
    if (world != NULL && world->conveyor != NULL) {
        velocity->x += world->current_level->conveyor_x_speed;
        velocity->z += world->current_level->conveyor_z_speed;
    }
    return 0;
}

__declspec(noinline) void __cdecl PushAway(NUVEC *position, f32 radius, NUVEC *minimum, NUVEC *maximum,
                                           GAMEOBJECT *object, GAMEOBJECT *excluded, f32 speed_multiplier, u32 flags) {
    i32 count, i;
    NUVEC bounds_min, bounds_max;
    NUVEC target;
    f32 dx, dz, combined_radius, speed;
    u16 angle;
    if (object == NULL) {
        object = Obj;
        count = HIGHGAMEOBJECT;
    } else {
        count = 1;
    }
    if (minimum == NULL || maximum == NULL) {
        bounds_min.x = position->x - radius;
        bounds_min.y = position->y - radius;
        bounds_min.z = position->z - radius;
        bounds_max.x = position->x + radius;
        bounds_max.y = position->y + radius;
        bounds_max.z = position->z + radius;
        minimum = &bounds_min;
        maximum = &bounds_max;
    }
    for (i = 0; i < count; ++i, ++object) {
        if (object == excluded || !(object->flags_1fc_word & 1) || !(object->flags_1fc_word & 0x1000) || object->dead ||
            (object->flags_e20 & 0x20))
            continue;
        if ((flags & 2) && !object->field_281)
            continue;
        if (U64_PAIR(&CInfo[object->character_context].flags) & 0x40000000)
            continue;
        if (object->character_data->game_character->flags_090 & 0x8000)
            continue;
        if (minimum->x > object->collision_max.x || maximum->x < object->collision_min.x ||
            minimum->z > object->collision_max.z || maximum->z < object->collision_min.z)
            continue;
        if ((flags & 1) && (minimum->y > object->collision_max.y || maximum->y < object->collision_min.y))
            continue;
        combined_radius = object->bounds_radius + radius;
        dx = object->position.x - position->x;
        dz = object->position.z - position->z;
        combined_radius *= combined_radius;
        if (dx * dx + dz * dz < combined_radius) {
            if (dx == 0.0f && dz == 0.0f)
                angle = qrand();
            else
                angle = NuAtan2D(dx, dz);
            speed = object->character_data->game_character->run_speed * speed_multiplier;
            if (speed < 1.0f)
                speed = 1.0f;
            else if (speed > 3.0f)
                speed = 3.0f;
            target.x = speed * NuTrigTable[angle >> 1];
            target.z = speed * NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
            object->velocity.x = SeekValF(object->velocity.x, target.x, 10.0f);
            object->velocity.z = SeekValF(object->velocity.z, target.z, 10.0f);
        }
    }
}

typedef struct SPECIAL_LAYER_PC_s {
    i16 *character_id;
    const char *name;
    u32 mask;
} SPECIAL_LAYER_PC;
STATIC_ASSERT(sizeof(SPECIAL_LAYER_PC) == 0xc);
STATIC_ASSERT(offsetof(SPECIAL_LAYER_PC, name) == 4);
STATIC_ASSERT(offsetof(SPECIAL_LAYER_PC, mask) == 8);

#pragma const_seg(".rdata$SpecialLayerName_bombbackpack")
static const char SpecialLayerName_bombbackpack[13] = "bombbackpack";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_sonargun")
static const char SpecialLayerName_sonargun[9] = "sonargun";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_infrared_goggles")
static const char SpecialLayerName_infrared_goggles[17] = "infrared_goggles";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_mask_black")
static const char SpecialLayerName_mask_black[11] = "mask_black";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_mask_blue")
static const char SpecialLayerName_mask_blue[10] = "mask_blue";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_mask_red")
static const char SpecialLayerName_mask_red[9] = "mask_red";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_cape_black")
static const char SpecialLayerName_cape_black[11] = "cape_black";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_cape_blue")
static const char SpecialLayerName_cape_blue[10] = "cape_blue";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_grey_nextgen")
static const char SpecialLayerName_body_grey_nextgen[18] = "body_grey_nextgen";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_grey_high")
static const char SpecialLayerName_body_grey_high[15] = "body_grey_high";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_grey_low")
static const char SpecialLayerName_body_grey_low[14] = "body_grey_low";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_black_nextgen")
static const char SpecialLayerName_body_black_nextgen[19] = "body_black_nextgen";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_black_high")
static const char SpecialLayerName_body_black_high[16] = "body_black_high";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_black_low")
static const char SpecialLayerName_body_black_low[15] = "body_black_low";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_blue_nextgen")
static const char SpecialLayerName_body_blue_nextgen[18] = "body_blue_nextgen";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_blue_high")
static const char SpecialLayerName_body_blue_high[15] = "body_blue_high";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_body_blue_low")
static const char SpecialLayerName_body_blue_low[14] = "body_blue_low";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_face_hands_black")
static const char SpecialLayerName_face_hands_black[17] = "face_hands_black";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_face_hands_blue")
static const char SpecialLayerName_face_hands_blue[16] = "face_hands_blue";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_face_hands_red")
static const char SpecialLayerName_face_hands_red[15] = "face_hands_red";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hips_black_nextgen")
static const char SpecialLayerName_hips_black_nextgen[19] = "hips_black_nextgen";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hips_black_high")
static const char SpecialLayerName_hips_black_high[16] = "hips_black_high";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hips_blue_nextgen")
static const char SpecialLayerName_hips_blue_nextgen[18] = "hips_blue_nextgen";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hips_blue_high")
static const char SpecialLayerName_hips_blue_high[15] = "hips_blue_high";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hips_red_nextgen")
static const char SpecialLayerName_hips_red_nextgen[17] = "hips_red_nextgen";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hips_red_high")
static const char SpecialLayerName_hips_red_high[14] = "hips_red_high";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_magnetic_boots")
static const char SpecialLayerName_magnetic_boots[15] = "magnetic_boots";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_scuba_gear")
static const char SpecialLayerName_scuba_gear[11] = "scuba_gear";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hack_pack")
static const char SpecialLayerName_hack_pack[10] = "hack_pack";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_vacuum_gun")
static const char SpecialLayerName_vacuum_gun[11] = "vacuum_gun";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_nextgen_limbs_green")
static const char SpecialLayerName_nextgen_limbs_green[20] = "nextgen_limbs_green";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hires_limbs_green")
static const char SpecialLayerName_hires_limbs_green[18] = "hires_limbs_green";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_lowres_limbs_green")
static const char SpecialLayerName_lowres_limbs_green[19] = "lowres_limbs_green";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_nextgen_limbs_white")
static const char SpecialLayerName_nextgen_limbs_white[20] = "nextgen_limbs_white";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hires_limbs_white")
static const char SpecialLayerName_hires_limbs_white[18] = "hires_limbs_white";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_lowres_limbs_white")
static const char SpecialLayerName_lowres_limbs_white[19] = "lowres_limbs_white";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_nextgen_limbs_grey")
static const char SpecialLayerName_nextgen_limbs_grey[19] = "nextgen_limbs_grey";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hires_limbs_grey")
static const char SpecialLayerName_hires_limbs_grey[17] = "hires_limbs_grey";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_lowres_limbs_grey")
static const char SpecialLayerName_lowres_limbs_grey[18] = "lowres_limbs_grey";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_nextgen_limbs_blue")
static const char SpecialLayerName_nextgen_limbs_blue[19] = "nextgen_limbs_blue";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_hires_limbs_blue")
static const char SpecialLayerName_hires_limbs_blue[17] = "hires_limbs_blue";
#pragma const_seg()
#pragma const_seg(".rdata$SpecialLayerName_lowres_limbs_blue")
static const char SpecialLayerName_lowres_limbs_blue[18] = "lowres_limbs_blue";
#pragma const_seg()
#pragma data_seg(".data$SpecialLayer")
static SPECIAL_LAYER_PC SpecialLayer[42] = {
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_bombbackpack, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_sonargun, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_infrared_goggles, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_mask_black, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_mask_blue, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_mask_red, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_cape_black, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_cape_blue, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_grey_nextgen, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_grey_high, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_grey_low, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_black_nextgen, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_black_high, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_black_low, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_blue_nextgen, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_blue_high, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_body_blue_low, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_face_hands_black, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_face_hands_blue, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_face_hands_red, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_hips_black_nextgen, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_hips_black_high, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_hips_blue_nextgen, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_hips_blue_high, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_hips_red_nextgen, 0},
    {&ID_DEFAULTCHARACTER_A, SpecialLayerName_hips_red_high, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_magnetic_boots, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_scuba_gear, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_hack_pack, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_vacuum_gun, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_nextgen_limbs_green, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_hires_limbs_green, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_lowres_limbs_green, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_nextgen_limbs_white, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_hires_limbs_white, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_lowres_limbs_white, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_nextgen_limbs_grey, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_hires_limbs_grey, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_lowres_limbs_grey, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_nextgen_limbs_blue, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_hires_limbs_blue, 0},
    {&ID_DEFAULTCHARACTER_B, SpecialLayerName_lowres_limbs_blue, 0},
};
#pragma data_seg()

u32 __cdecl AdjustLayerBits(u32 mask, GAMEOBJECT *object) {
    PLAYERCHARACTERCONFIG_PC *data = object->character_data->game_character;
    i32 cape_layer = data->cape_layer;
    u32 cape = cape_layer != -1 ? 1u << cape_layer : 0;
    i32 hair_layer = data->hair_layer;
    u32 hair = hair_layer != -1 ? 1u << hair_layer : 0;
    SUIT_LAYERS_PC *suit;
    if (object->helmet != 0)
        mask &= ~hair;
    suit = (SUIT_LAYERS_PC *)object->suit;
    if (suit != NULL && object->id == (u16)ID_DEFAULTCHARACTER_A) {
        if ((suit->flags & 4) != 0) {
            mask = (mask & ~cape) | SpecialLayer[0].mask;
            mask = (mask & ~SpecialLayer[3].mask) | SpecialLayer[5].mask;
            if ((mask & SpecialLayer[20].mask) != 0)
                mask = (mask & ~SpecialLayer[20].mask) | SpecialLayer[24].mask;
            else if ((mask & SpecialLayer[21].mask) != 0)
                mask = (mask & ~SpecialLayer[21].mask) | SpecialLayer[25].mask;
            mask = (mask & ~SpecialLayer[17].mask) | SpecialLayer[19].mask;
        } else if ((suit->flags & 2) != 0) {
            mask &= ~cape;
        } else if ((suit->flags & 8) != 0) {
            mask = (mask & ~cape) | SpecialLayer[1].mask | SpecialLayer[7].mask;
            mask = (mask & ~SpecialLayer[3].mask) | SpecialLayer[4].mask;
            if ((mask & SpecialLayer[20].mask) != 0)
                mask = (mask & ~SpecialLayer[20].mask) | SpecialLayer[22].mask;
            else if ((mask & SpecialLayer[21].mask) != 0)
                mask = (mask & ~SpecialLayer[21].mask) | SpecialLayer[23].mask;
            mask = (mask & ~SpecialLayer[17].mask) | SpecialLayer[18].mask;
            if ((mask & SpecialLayer[8].mask) != 0)
                mask = (mask & ~SpecialLayer[8].mask) | SpecialLayer[14].mask;
            else if ((mask & SpecialLayer[9].mask) != 0)
                mask = (mask & ~SpecialLayer[9].mask) | SpecialLayer[15].mask;
            else if ((mask & SpecialLayer[10].mask) != 0)
                mask = (mask & ~SpecialLayer[10].mask) | SpecialLayer[16].mask;
        } else if ((suit->flags & 1) != 0) {
            mask |= SpecialLayer[2].mask;
            if ((mask & SpecialLayer[8].mask) != 0)
                mask = (mask & ~SpecialLayer[8].mask) | SpecialLayer[11].mask;
            else if ((mask & SpecialLayer[9].mask) != 0)
                mask = (mask & ~SpecialLayer[9].mask) | SpecialLayer[12].mask;
            else if ((mask & SpecialLayer[10].mask) != 0)
                mask = (mask & ~SpecialLayer[10].mask) | SpecialLayer[13].mask;
        }
    } else if (suit != NULL && object->id == (u16)ID_DEFAULTCHARACTER_B) {
        if ((suit->flags & 0x10) != 0) {
            mask = (mask & ~hair & ~cape) | SpecialLayer[27].mask;
            if ((mask & SpecialLayer[30].mask) != 0)
                mask = (mask & ~SpecialLayer[30].mask) | SpecialLayer[39].mask;
            else if ((mask & SpecialLayer[31].mask) != 0)
                mask = (mask & ~SpecialLayer[31].mask) | SpecialLayer[40].mask;
            else if ((mask & SpecialLayer[32].mask) != 0)
                mask = (mask & ~SpecialLayer[32].mask) | SpecialLayer[41].mask;
        } else if ((suit->flags & 0x40) != 0) {
            mask = (mask & ~cape) | SpecialLayer[26].mask;
            if ((mask & SpecialLayer[30].mask) != 0)
                mask = (mask & ~SpecialLayer[30].mask) | SpecialLayer[36].mask;
            else if ((mask & SpecialLayer[31].mask) != 0)
                mask = (mask & ~SpecialLayer[31].mask) | SpecialLayer[37].mask;
            else if ((mask & SpecialLayer[32].mask) != 0)
                mask = (mask & ~SpecialLayer[32].mask) | SpecialLayer[38].mask;
        } else if ((suit->flags & 0x20) != 0) {
            mask = (mask & ~cape) | SpecialLayer[28].mask;
            if ((mask & SpecialLayer[30].mask) != 0)
                mask = (mask & ~SpecialLayer[30].mask) | SpecialLayer[33].mask;
            else if ((mask & SpecialLayer[31].mask) != 0)
                mask = (mask & ~SpecialLayer[31].mask) | SpecialLayer[34].mask;
            else if ((mask & SpecialLayer[32].mask) != 0)
                mask = (mask & ~SpecialLayer[32].mask) | SpecialLayer[35].mask;
        } else if ((suit->flags & 0x80) != 0) {
            mask = (mask & ~cape) | SpecialLayer[29].mask;
        }
    }
    if (StartupGameFlag1 != 0) {
        if (object->id == (u16)id_BODYGUARD) {
            if (object->current_hp <= 1)
                mask &= ~0x10u;
        } else if (object->id == (u16)id_GEONOSIAN) {
            if ((object->flags_ef0 & 0x200) != 0)
                mask |= 0x20;
            else
                mask |= 0x40;
        } else if (CharacterCustomiser != NULL && object->id == (u16)CharacterCustomiser->character_ids[0]) {
            if ((CharacterCustomiser->pieces[Game.customizer_cape_piece].layer_flags & 0x40) == 0)
                mask |= cape;
        } else if (CharacterCustomiser != NULL && object->id == (u16)CharacterCustomiser->character_ids[1]) {
            if ((CharacterCustomiser->pieces[Game.customizer_secondary_cape_piece].layer_flags & 0x40) == 0)
                mask |= cape;
        } else if (object->id == (u16)id_CHEWBACCA) {
            if (Cheat_IsOn(4) != 0)
                mask |= 0xc0;
        }
    }
    if (object->character_data->game_character->ride_layers_off != 0 && object->takeover_source != NULL &&
        object->character_context == 0x3b)
        mask &= ~object->character_data->game_character->ride_layers_off;
    return mask;
}

void __cdecl FixUpLayers(void) {
    i32 model_index;
    i32 id;
    for (model_index = 0; model_index < apicharsys->loaded_model_count; ++model_index) {
        CHARACTER_MODEL_LAYERS_PC *model = &apicharsys->models[model_index];
        PLAYERCHARACTERCONFIG_PC *character = &GCDataList[model->model_id];
        i32 layer_index;
        for (layer_index = 0; layer_index < character->layer_count; ++layer_index) {
            character->layers[layer_index].hierarchy_layer_index =
                NuHGobjGetLayerIndex(character->layers[layer_index].name, model->hierarchy);
        }
    }

    for (model_index = 0; model_index < 42; ++model_index) {
        SpecialLayer[model_index].mask = 0;
        if (SpecialLayer[model_index].character_id != NULL) {
            id = *SpecialLayer[model_index].character_id;
            if (id != -1)
                SpecialLayer[model_index].mask = 1u << LayerFromName(&GCDataList[id], SpecialLayer[model_index].name);
        }
    }
}

GAMEOBJECT *__cdecl GetOtherActivePlayer(GAMEOBJECT *object) {
    GAMEOBJECT *other;
    if (object == Player[0]) {
        other = (GAMEOBJECT *)Player[1];
    } else if (object == Player[1]) {
        other = (GAMEOBJECT *)Player[0];
    } else {
        return NULL;
    }
    if (other != NULL && (other->flags_1fc_word & 0x80) != 0) {
        return other;
    }
    return NULL;
}

i32 __cdecl AvailableToPlayer(u32 character_flags, i32 weapon_action, i32 helmet, i32 require_all) {
    i32 index;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *object = (GAMEOBJECT *)Player[index];
        if (object == NULL || object->character_data == NULL) {
            continue;
        }
        if (character_flags != 0) {
            if ((object->character_data->model_flags & character_flags) == character_flags) {
                if (require_all == 0) {
                    return 1;
                }
            } else if (require_all != 0) {
                continue;
            }
        }
        if (weapon_action != -1) {
            if (object->character_data->game_character->weapon_action == weapon_action) {
                if (require_all == 0) {
                    return 1;
                }
            } else if (require_all != 0) {
                continue;
            }
        }
        if (helmet != 0) {
            if (object->helmet != helmet) {
                continue;
            }
            if (require_all == 0) {
                return 1;
            }
        }
        return 1;
    }
    if (FreePlay != 0) {
        for (index = 0; index < apicharsys->character_count; ++index) {
            if (apicharsys->playermodelids[index] == -1 || FreePlay == 0 ||
                (GCDataList[index].flags_090 & 0x80000000) == 0 || Cheats_CheckFlags(0x100) == 0) {
                continue;
            }
            if (character_flags != 0) {
                if ((CDataList[index].model_flags & character_flags) == character_flags) {
                    if (require_all == 0) {
                        return 1;
                    }
                } else if (require_all != 0) {
                    continue;
                }
            }
            if (weapon_action != -1) {
                if (GCDataList[index].weapon_action != weapon_action) {
                    continue;
                }
                if (require_all == 0) {
                    return 1;
                }
            }
            return 1;
        }
    }
    return 0;
}

void __cdecl DrawOffsetCode(GAMEOBJECT *object, i32 reset) {
    NUVEC target = v000;
    NUVEC delta;
    f32 distance;
    f32 step;

    if (object->suit != NULL) {
        if ((object->suit->flags & 0x10) != 0) {
            target.y = 0.0076f;
        } else if ((object->suit->flags & 0x40) != 0) {
            target.y = MAGNETOFFSET;
        }
    }
    if ((object->character_context == 0x44 && object->field_7aa != 6 && object->field_7aa != 5) ||
        (object->character_context == 0x43 && object->context_variant == 5)) {
        target = object->context_position_offset;
    }
    if (reset != 0) {
        object->render_offset = target;
        return;
    }
    if (object->render_offset.x == target.x && object->render_offset.y == target.y &&
        object->render_offset.z == target.z) {
        return;
    }
    distance = NuVecDist(&target, &object->render_offset, &delta);
    step = (f32)((f64)FRAMETIME * (f64)0.4f);
    if (step >= distance) {
        object->render_offset = target;
        return;
    }
    NuVecNorm(&delta, &delta);
    NuVecScale(&delta, &delta, step);
    NuVecAdd(&object->render_offset, &object->render_offset, &delta);
}

i32 __cdecl Player_HasPurpleForce(GAMEOBJECT *object) {
    if (Cheat_IsOn(28) != 0) {
        return 1;
    }
    return object != NULL && object->powerup_time > 0.0f;
}

i32 __cdecl Player_HasDoubleWeaponDamage(GAMEOBJECT *object) {
    if (Cheats_CheckFlags(0x400) != 0) {
        return 1;
    }
    return object != NULL && object->powerup_time > 0.0f;
}

i32 __cdecl UnderPlayerControl(GAMEOBJECT *object) {
    if ((object->flags_1fc_word & 0x80) != 0)
        return 1;
    if (object->takeover_source != NULL && (object->takeover_source->flags_1fc_word & 0x80) != 0)
        return 1;
    return 0;
}

void __cdecl SetToLastSafePos(GAMEOBJECT *object) {
    object->start_position = object->last_safe_position;
    object->position = object->last_safe_position;
    object->ai_update_position = object->last_safe_position;
}

typedef struct player_reset_packet_pc_s {
    u8 unknown_000[0xe8];
    i16 context_animation;
    u8 unknown_0ea[0x4];
    u8 context_mode;
    u8 unknown_0ef[0x2];
    i8 build_context;
    u8 unknown_0f2[0x2];
    u8 action_movement_state;
    u8 action_movement_variant;
    u8 unknown_0f6[0x6];
    f32 context_animation_time;
    i8 context_target;
    u8 unknown_101[0x4df];
    GAMEPAD_PC *gamepad;
    u8 unknown_5e4[0x28];
    void *linked_object;
    u8 unknown_610[0x38];
    i32 unknown_648;
    u8 unknown_64c[0x10];
    i32 input_state;
    u8 unknown_660[0x4];
    f32 unknown_664;
    u8 unknown_668[0x4];
    f32 unknown_66c;
    f32 unknown_670;
    f32 unknown_674;
    f32 context_blend;
    f32 context_distance;
    u8 unknown_680[0x14];
    f32 unknown_694;
    f32 unknown_698;
    u8 unknown_69c[0x34];
    f32 unknown_6d0;
    u8 unknown_6d4[0x14];
    f32 unknown_6e8;
    u8 unknown_6ec[0x14];
    f32 unknown_700;
    u8 unknown_704[0x10];
    f32 unknown_714;
    u8 unknown_718[0x8];
    f32 unknown_720;
    f32 unknown_724;
    u8 unknown_728[0x24];
    i16 movement_angle_0;
    u8 unknown_74e[0x2];
    i16 movement_angle_2;
    i16 movement_angle_3;
    u8 unknown_754[0x8];
    u32 secondary_flags;
    u8 unknown_760[0xe];
    u8 unknown_76e;
    u8 unknown_76f;
    u8 unknown_770[0xd];
    i8 random_context;
} PLAYER_RESET_PACKET_PC;
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, context_animation) == 0xe8);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, context_mode) == 0xee);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, build_context) == 0xf1);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, action_movement_state) == 0xf4);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, action_movement_variant) == 0xf5);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, context_animation_time) == 0xfc);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, context_target) == 0x100);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, gamepad) == 0x5e0);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, linked_object) == 0x60c);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_648) == 0x648);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, input_state) == 0x65c);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_664) == 0x664);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_66c) == 0x66c);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_670) == 0x670);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_674) == 0x674);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, context_blend) == 0x678);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, context_distance) == 0x67c);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_694) == 0x694);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_698) == 0x698);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_6d0) == 0x6d0);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_6e8) == 0x6e8);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_700) == 0x700);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_714) == 0x714);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_720) == 0x720);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_724) == 0x724);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, movement_angle_0) == 0x74c);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, movement_angle_2) == 0x750);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, movement_angle_3) == 0x752);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, secondary_flags) == 0x75c);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_76e) == 0x76e);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, unknown_76f) == 0x76f);
STATIC_ASSERT(offsetof(PLAYER_RESET_PACKET_PC, random_context) == 0x77d);
STATIC_ASSERT(offsetof(GAMEPAD_PC, runtime_flags) == 0x5a);
void __cdecl Player_ResetContexts(void *packet_ptr) {
    PLAYER_RESET_PACKET_PC *packet = (PLAYER_RESET_PACKET_PC *)packet_ptr;
    packet->context_animation_time = 0.0f;
    packet->context_target = -1;
    if ((packet->secondary_flags >> 16) & 1)
        packet->context_blend = 1.0f;
    else
        packet->context_blend = 0.0f;
    packet->context_distance = 5.0f;
    packet->unknown_76f = 0;
    packet->unknown_66c = 0.0f;
    packet->build_context = -1;
    packet->unknown_674 = 0.0f;
    packet->action_movement_variant = 0;
    packet->action_movement_state = 0;
    packet->unknown_664 = 0.0f;
    packet->gamepad->runtime_flags &= ~4;
    packet->gamepad->runtime_flags &= ~0x10;
    packet->secondary_flags &= ~0x400;
    packet->input_state = 0;
    packet->unknown_648 = 0;
    packet->random_context = (i8)(qrand() / 0x8000);
    packet->unknown_694 = 0.0f;
    packet->unknown_698 = 0.0f;
    packet->unknown_670 = 0.0f;
    packet->unknown_6d0 = 0.0f;
    packet->unknown_6e8 = 0.0f;
    packet->unknown_700 = 0.0f;
    packet->unknown_714 = 0.0f;
    packet->context_animation = 0;
    packet->unknown_720 = 0.0f;
    packet->context_mode = 0;
    packet->unknown_76e = 0;
    packet->unknown_724 = 0.0f;
    packet->linked_object = NULL;
    packet->movement_angle_0 = -1;
    packet->movement_angle_2 = -1;
    packet->movement_angle_3 = -1;
}
void __cdecl SetFlicker(GAMEOBJECT *object, f32 duration) {
    object->flags_e24 &= ~0x70000;
    object->vehicle_hit_timer = duration;
}

void __cdecl SetProtocolDroidDeactivatedAction(GAMEOBJECT *object) {
    if (object->saved_hitpoints == 3)
        object->field_7aa = 0x42;
    else if (object->saved_hitpoints == 2)
        object->field_7aa = 0x43;
    else if (object->saved_hitpoints == 1)
        object->field_7aa = 0x44;
}

i32 __cdecl Arcade_BothPlayersActive(void) {
    return Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0 && Player[1] != NULL &&
           (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0;
}

i32 __cdecl Player_HasInvincibility(GAMEOBJECT *object) {
    if (Cheats_CheckFlags(0x80) != 0)
        return 1;
    return object != NULL && object->powerup_time > 0.0f;
}
i32 __cdecl Player_HasFastBuild(GAMEOBJECT *object) {
    if (Cheats_CheckFlags(0x4000) != 0)
        return 1;
    return object != NULL && object->powerup_time > 0.0f;
}

typedef struct player_reset_fields_pc_s {
    u8 unknown_0[0xc50];
    NUVEC weapon_trail_offset;
    u8 unknown_c5c[0xa4];
    void *incoming_special;
    u8 unknown_d04[0x14];
    void *force_glow_candidate;
    u8 unknown_d1c[8];
    f32 ground_contact_grace_timer;
    u8 unknown_d28[0x78];
    f32 nearby_floor_distance;
    u8 unknown_da4[0x14];
    f32 pre_reset_hover_height;
    u8 unknown_dbc[0x28];
    f32 hold_timer;
    u8 unknown_de8[0x1c];
    i32 dynamic_light_id;
    u8 unknown_e08[0x275];
    u8 pre_reset_state;
} PLAYER_RESET_FIELDS_PC;
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, weapon_trail_offset) == 0xc50);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, incoming_special) == 0xd00);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, force_glow_candidate) == 0xd18);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, ground_contact_grace_timer) == 0xd24);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, nearby_floor_distance) == 0xda0);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, pre_reset_hover_height) == 0xdb8);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, hold_timer) == 0xde4);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, dynamic_light_id) == 0xe04);
STATIC_ASSERT(offsetof(PLAYER_RESET_FIELDS_PC, pre_reset_state) == 0x107d);
void __cdecl DrawOffsetCode(GAMEOBJECT *, i32);
i32 __cdecl rtlDynamicEnable(i32, i32);
f32 __cdecl GetHoverPosY(GAMEOBJECT *);
i32 __cdecl Player_HasPurpleForce(GAMEOBJECT *);
static __declspec(noinline) i32 PreResetCanAccelerateFallPc(GAMEOBJECT *object) {
    NUVEC direction;
    f32 delta, threshold, delta_x, delta_z;
    if (VehicleArea != 0 || (object->flags_1fc_word & 0x80) == 0)
        return 0;
    if (object->character_context == 0 &&
        object->context_timer < object->character_data->game_character->jump_duration * 0.5)
        return 0;
    if (!(object->pad->input_magnitude > 0.0f) && object->velocity.x == 0.0f && object->velocity.z == 0.0f)
        return 0;
    if ((object->field_281 & 2) != 0 || object->contact_count == 0)
        return 0;
    if (!(object->contact_normal.y > NuTrigTable[0x4000]) || !(object->contact_normal.y < NuTrigTable[0x3000]))
        return 0;
    delta = object->collision_min.y - object->floor_height;
    threshold = (f32)(object->bounds_half_height / (f64)0.21f);
    if (!(delta > threshold * (f64)0.035f))
        return 0;
    if (object->pad->input_magnitude > 0.0f) {
        NuVecRotateY(&direction, &v001, object->movement_facing_angle);
    } else {
        direction.x = object->velocity.x;
        direction.y = 0.0f;
        direction.z = object->velocity.z;
        NuVecNorm(&direction, &direction);
    }
    delta_x = object->contact_position.x - object->position.x;
    delta_z = object->contact_position.z - object->position.z;
    return delta_x * direction.x + delta_z * direction.z > 0.0;
}
void __cdecl PreResetCode(GAMEOBJECT *object) {
    PLAYER_RESET_FIELDS_PC *reset = (PLAYER_RESET_FIELDS_PC *)object;
    PLAYERCHARACTERCONFIG_PC *character;
    object->flags_e24 &= 0xffffafcf;
    object->flags_e20 &= 0xf83fffef;
    object->flags_ef4 &= 0xdfffffff;
    object->quick_shoot_bolt_id = -1;
    object->slam_effect = -1;
    object->batarang_release = 0;
    object->debris_joint = -1;
    DrawOffsetCode(object, 0);
    if ((object->flags_ef4 & 8) == 0)
        return;
    object->flags_e20 &= 0xfff7ffff;
    reset->weapon_trail_offset.x = (f32)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.03f - (f64)0.015f;
    reset->weapon_trail_offset.y = (f32)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.03f - (f64)0.015f;
    reset->weapon_trail_offset.z = (f32)qrand() * (f64)(1.0f / 65535.0f) * (f64)0.03f - (f64)0.015f;
    object->flags_e20 &= ~4u;
    object->incoming_bolt = NULL;
    object->incoming_melee = NULL;
    reset->incoming_special = NULL;
    object->incoming_part = NULL;
    if (object->character_context != 0xa)
        object->flags_e20 &= ~0x800u;
    object->flags_e20 &= 0xfffddfff;
    object->terrain_origin_floor_offset = 0.0f;
    object->sabre_flags = 1;
    reset->force_glow_candidate = NULL;
    object->blade_states[0] = object->blade_states[1] = object->blade_states[2] = object->blade_states[3] = -1;
    object->anim_packet.overlay_animation = -1;
    if ((object->character_data->model_flags & 0x8040) == 0)
        object->jump_variant_state = 0;
    object->flags_e20 &= 0x7fffffff;
    if (reset->dynamic_light_id != -1)
        rtlDynamicEnable(reset->dynamic_light_id, 0);
    object->flags_e24 &= 0xfffffdbf;
    reset->pre_reset_state = 0;
    if (object->field_281 != 0)
        reset->ground_contact_grace_timer = 0.2f;
    else if (reset->ground_contact_grace_timer > 0.0f)
        reset->ground_contact_grace_timer -= FRAMETIME;
    if ((object->flags_1fc_word & 0x80) != 0 && PreResetCanAccelerateFallPc(object))
        object->fall_acceleration_timer = 0.1f;
    else if (object->fall_acceleration_timer > 0.0f)
        object->fall_acceleration_timer -= FRAMETIME;
    GameObjectNearFloor(object, 1.0f, &reset->nearby_floor_distance);
    reset->pre_reset_hover_height = GetHoverPosY(object);
    character = object->character_data->game_character;
    if (character->blade_colour != -1) {
        if (object->field_280 != 0xff && Cheat_IsOn(0x19))
            object->blade_index = 0;
        else if (object->field_280 != 0xff && Player_HasPurpleForce(object))
            object->blade_index = 3;
        else
            object->blade_index = object->character_data->game_character->blade_colour;
    } else {
        object->blade_index = -1;
    }
    if ((object->pad->buttons_held & GAMEPAD_ACTION) != 0) {
        object->flags_e24 &= 0xffff5fff;
        reset->hold_timer += FRAMETIME;
    } else {
        if (reset->hold_timer > 0.0f && reset->hold_timer < 0.25f)
            object->flags_e24 |= 0x40;
        object->flags_e24 &= 0xffff5fff;
        reset->hold_timer = 0.0f;
    }
}

extern const f64 GameObjectNearFloorNoFloorHeight;

f32 __cdecl GetHoverPosY(GAMEOBJECT *object) {
    f32 offset, height;
    if ((object->character_data->model_flags & 0x2000) != 0 &&
        (WorldInfo_CurrentlyActive()->current_level->flags & 0x40000) == 0) {
        offset = VehicleTurnOrLoopOffset(object);
        if (object->field_283 != 7 && object->field_283 != 0x10) {
            height = object->floor_height;
            if (height != GameObjectNearFloorNoFloorHeight) {
                if (object->water_height != GameObjectNearFloorNoFloorHeight && object->water_height > height)
                    height = object->water_height;
                if (WORLD->current_level->hover_height != 0.0f)
                    return height + WORLD->current_level->hover_height + offset;
                return height + object->character_data->game_character->field_028 + offset;
            }
            return LevelConfigFarDistance;
        }
        return object->water_height + offset;
    }
    return LevelConfigFarDistance;
}

i32 __cdecl KillGameObject(GAMEOBJECT *, i32, i32);
void __cdecl Arcade_PlayerKilled(i32, i32);
i32 __cdecl KillPlayer(GAMEOBJECT *object, i32 reason, i32 ignore_spawn_protection, NUVEC *unused) {
    i32 other, active;
    (void)unused;
    if (object->dead != 0)
        return 0;
    if (ignore_spawn_protection != 0)
        object->spawn_protection_timer = 0.0f;
    else if (object->spawn_protection_timer > 0.0f)
        return 0;
    if (Arcade != 0) {
        active = 0;
        other = -1;
        if (object->field_280 == 0)
            other = 1;
        else if (object->field_280 == 1)
            other = 0;
        if (other != -1) {
            if (Player[other] != NULL && (((GAMEOBJECT *)Player[other])->flags_1f8 & 0x40000) != 0)
                active = 1;
            Arcade_PlayerKilled(other, active);
        }
    }
    return KillGameObject(object, reason, 0);
}

extern f32 TerrainChatterWaitPc;
extern i32 TerrainLastChatterSfxPc;
void __cdecl ChatterSfx(GAMEOBJECT *object, i32 sound, f32 interval) {
    if (TerrainChatterWaitPc <= 0.0f && ParticlesPerSecond(2.0f, FRAMETIME) > 0 && object->dead == 0 &&
        object->character_context == -1) {
        if (sound != TerrainLastChatterSfxPc || !(interval > 0.0f)) {
            PlaySfxById(sound, &object->collision_position);
            TerrainChatterWaitPc = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 2.0 + 3.0);
            TerrainLastChatterSfxPc = sound;
        } else {
            TerrainChatterWaitPc = interval;
            TerrainLastChatterSfxPc = -1;
        }
    }
}

STATIC_ASSERT(offsetof(GAMEOBJECT, jump_variant_state) == 0xe32);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_0x276) == 0x27a);
STATIC_ASSERT(offsetof(GAMEOBJECT, force_target) == 0x790);
i32 __cdecl DeactivatePlayer(GAMEOBJECT *object, f32 duration, GAMEOBJECT *source) {
    f32 model_state;
    if (object->character_context != 0x17 || object->context_timer < duration) {
        model_state = object->origin_scale_override;
        Player_ClearContext(object, 1);
        if (object->character_context != 0x3e) {
            Player_ResetContexts(object->ai_player_packet_pc);
            object->character_context = 0x17;
            object->origin_scale_override = model_state;
            if (AIScriptSetBaseScriptStateByName((AISCRIPTPROCESS_PC *)&object->ai_packet,
                                                 (char *)PlayerBeenDeactivatedPc))
                AIScriptProcess(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet,
                                (AISCRIPTPROCESS_PC *)&object->ai_packet, FRAMETIME);
            if (source != NULL && (source->character_data->model_flags & 0x40) &&
                object->character_data->game_character->weapon_action == 1 &&
                object->character_model->model_data_b[0x62] != NULL) {
                duration = 4.0f;
                object->field_7aa = 0x62;
            } else if (object->character_model->model_data_b[0x81] != NULL &&
                       !(object->character_model->animation_configs[0x81]->flags & 2))
                object->field_7aa = 0x81;
            else
                object->field_7aa = object->character_model->model_data_b[0x41] != NULL ? 0x41 : 1;
            if (object->character_data->model_flags & 0x20)
                SetProtocolDroidDeactivatedAction(object);
            object->action_movement_state = 0;
            object->context_timer = duration;
            object->context_duration = 0.0f;
            object->context_hover_target_y = 0.0f;
            NewBuzz(object->pad->bound_pad, 0.1f, 0);
            return 1;
        }
    }
    return 0;
}

__declspec(noinline) void __cdecl Player_ClearContext(GAMEOBJECT *object, i32 mode) {
    if (Player_ClearContextFn != NULL)
        Player_ClearContextFn(object, mode);
    Whip_Release(object);
    SuperCarry_Release(object);
    SpecialMove_ReleaseVictim(object);
    object->flags_e24 &= ~0xc00;
}

void __cdecl ActivatePlayer(GAMEOBJECT *object) {
    object->character_context = -1;
    if (object->character_model->model_data_b[0x80] != NULL) {
        object->character_context = 0x41;
        object->field_7aa = 0x80;
        object->context_timer = AnimDuration((i16)object->id, 0x80, 0.0f, 0.0f, 1);
        object->context_duration = 0.0f;
    }
}

i32 __cdecl PlayerItem_GotAmmo(WEAPON_AMMO_PC *item) {
    if (item == NULL)
        return 1;
    if (item->type == NULL)
        return 1;
    if (item->type->type != 2)
        return 1;
    return item->ammo > 0;
}

void __cdecl Move_VEHICLE(GAMEOBJECT *);
void __cdecl PostResetCode(GAMEOBJECT *object) {
    PLAYERCHARACTERCONFIG_PC *character;
    object->attack_override = 0;
    if (object->character_data->game_character->sfx_chatter != -1)
        ChatterSfx(object, object->character_data->game_character->sfx_chatter,
                   (f32)(u32)object->character_data->game_character->flags_11e);
    if (object->character_data->move_fn != Move_VEHICLE && object->tertiary_lean_angle != 0 &&
        (object->flags_e24 & 0x2000) == 0)
        object->tertiary_lean_angle = 0;
}

extern f32 animduration_blendouttime;
typedef struct player_suit_name_pc_s {
    u8 unknown_000[0x4];
    char *name;
} PLAYER_SUIT_NAME_PC;
STATIC_ASSERT(offsetof(PLAYER_SUIT_NAME_PC, name) == 0x4);
typedef struct player_packet_storage_pc_s {
    u8 unknown_000[0x6c4];
    u8 player_packet_storage[0x788];
} PLAYER_PACKET_STORAGE_PC;
STATIC_ASSERT(offsetof(PLAYER_PACKET_STORAGE_PC, player_packet_storage) == 0x6c4);

typedef struct player_batarang_idle_pc_s {
    u8 unknown_00[0x7d];
    u8 active;
} PLAYER_BATARANG_IDLE_PC;
STATIC_ASSERT(offsetof(PLAYER_BATARANG_IDLE_PC, active) == 0x7d);
typedef struct player_idle_character_pc_s {
    u8 unknown_00[0x116];
    u8 use_standard_idle;
} PLAYER_IDLE_CHARACTER_PC;
STATIC_ASSERT(offsetof(PLAYER_IDLE_CHARACTER_PC, use_standard_idle) == 0x116);
extern void __cdecl ResetPlayerPacket(void *, CHARACTERDATA_PC *);
extern void __cdecl ResetCoinPacket(void *);
extern void __cdecl SetAnimTimeRandom(CHARACTER_MODEL_LAYERS_PC *, ANIMPACKET_PC *);
extern GAMEOBJECT *__cdecl AddGameObject(void);
extern i32 __cdecl InitCreature(GAMEOBJECT *, i32, i32);
extern void __cdecl PlayerInitCapabilitiesVariant0Pc(GAMEOBJECT *);
extern void __cdecl PlayerInitCapabilitiesVariant1Pc(GAMEOBJECT *);
extern void __cdecl PlayerInitCapabilitiesDefaultPc(GAMEOBJECT *);
extern i32 __cdecl CanPullLevers(i32);

void __cdecl SetGameObjectCharacterData(GAMEOBJECT *object) {
    CHARACTERDATA_PC *data = object->character_data;
    object->origin_scale = data->model_scale;
    object->collision_radius_unscaled = data->collision_radius;
    object->character_bottom = data->bottom;
    object->character_top = data->top;
    ScaleGameObject(object);
}

i32 __cdecl GetDefaultIdle(GAMEOBJECT *object) {
    i32 animation;
    PLAYER_BATARANG_IDLE_PC *batarang;
    void **actions;
    void *idle, *standard;
    if (((PLAYER_IDLE_CHARACTER_PC *)object->character_data->game_character)->use_standard_idle == 0 &&
        (object->character_data->model_flags & 0x80) != 0)
        animation = 0x76;
    else
        animation = 0x19;
    batarang = (PLAYER_BATARANG_IDLE_PC *)object->batarang;
    if (batarang != NULL && batarang->active != 0)
        return 0x97;
    actions = object->character_model->action_animations;
    standard = actions[1];
    if (standard != NULL && actions[animation] == NULL)
        return 1;
    idle = actions[animation];
    if (idle != NULL && standard == NULL)
        return animation;
    if (((object->field_e22 & 1) != 0 || object->field_e33 == 1) && idle != NULL)
        return animation;
    return 1;
}

void __cdecl ResetCharacterIdle(GAMEOBJECT *object, i32 mode, i32 animation) {
    CHARACTERANIM_CONFIG_PC *info;
    i32 minimum, maximum;
    if (animation == 0x97)
        return;
    object->idle_animation = (i16)animation;
    object->idle_animation_time = 0.0f;
    minimum = 0;
    if (object->character_model->action_animations[(i16)animation] != NULL) {
        info = object->character_model->animation_configs[(i16)animation];
        minimum = info->minimum_repetitions;
    }
    if (minimum > 0) {
        maximum = info->maximum_repetitions;
        if (maximum > minimum)
            minimum += qrand() / (0xffff / (maximum - minimum) + 1);
        object->idle_animation_limit =
            (f32)((f64)AnimDuration((i16)object->id, object->idle_animation, 0.0f, 0.0f, 0) * (f64)minimum -
                  (f64)animduration_blendouttime);
    } else {
        object->idle_animation_limit = (f32)((f64)qrand() * (f64)(1.0f / 65535.0f) * 7.0 + 8.0);
    }
    if (mode > 0) {
        object->idle_total_time = 0.0f;
        if (mode > 1)
            object->previous_idle_animation = -1;
    }
}

void __cdecl ResetPlayerMoves(GAMEOBJECT *object) {
    ResetPlayerPacket(((PLAYER_PACKET_STORAGE_PC *)object)->player_packet_storage, object->character_data);
    object->fall_acceleration_timer = 0.0f;
    object->movement_direction = v000;
    object->hud_icon_timer = 0.0f;
    object->input_toggle_hold_time = TOGGLEHOLDTIME;
    object->flags_ef0 |= 0x80;
    ResetCharacterIdle(object, 2, GetDefaultIdle(object));
    if (object->character_model->action_animations[1] != NULL) {
        ResetAnimPacket(&object->anim_packet, 1);
        SetAnimTimeRandom(object->character_model, &object->anim_packet);
    } else
        ResetAnimPacket(&object->anim_packet, -1);
    object->field_282 = 0;
    object->field_281 = 0;
    SetGameObjectCharacterData(object);
    object->spawn_protection_timer = 0.0f;
    SetFlicker(object, 0.0f);
    ResetCoinPacket(object->coin_packet);
    object->respawn_timer = 0.0f;
    object->flags_1fc_word &= ~0x2000u;
    DrawOffsetCode(object, 1);
}

void __cdecl ResetPlayerAI(GAMEOBJECT *object) {
    object->special_move_timer_pc = 0.0f;
    object->ai_packet.animation_override_from = -1;
    object->ai_packet.animation_override_to = -1;
    object->flags_ef0 &= ~0x200000u;
    object->ai_packet.flags &= 0xfffffd9f;
    object->ai_packet.intersection_connection = NULL;
    object->ai_packet.intersection_target_connection = NULL;
    object->ai_packet.path_target = NULL;
    object->field_1086 = 0;
    object->field_1087 = 0;
    object->field_108c = 0;
    object->field_1088 = 0;
    object->can_use_object = NULL;
    object->use_action_word = 0;
    object->use_action_parameter_word = 0;
    object->ai_packet.frame_state = 0;
    object->route_frame_state = 0;
    memset(&object->ai_packet.path_info, 0, sizeof(object->ai_packet.path_info));
    object->ai_packet.inside_path_node = -1;
    object->ai_packet.current_route = 0xff;
    object->ai_packet.next_route = 0;
    AISysGetCharacterPathPos(WORLD->ai_sys, (APIOBJECT_PC *)object, &object->ai_packet, 0xff, 1);
}

/* Two-DWORD consumer view of the existing genuine mutable default mask. */
typedef struct PLAYER_ROUTE_MASK_WORDS_PC {
    u32 low;
    u32 high;
} PLAYER_ROUTE_MASK_WORDS_PC;
STATIC_ASSERT(sizeof(PLAYER_ROUTE_MASK_WORDS_PC) == 8);
STATIC_ASSERT(offsetof(PLAYER_ROUTE_MASK_WORDS_PC, low) == 0);
STATIC_ASSERT(offsetof(PLAYER_ROUTE_MASK_WORDS_PC, high) == 4);
extern u64 CharacterMaskAllPc;

void __cdecl InitPlayerAI(GAMEOBJECT *object) {
    CHARACTERDATA_PC *character;
    i32 can_pull_levers;
    u8 type;
    char *name;
    u64 route_mask;
    u32 route_mask_word;
    if (StartupGameFlag0 != 0)
        PlayerInitCapabilitiesVariant0Pc(object);
    else if (StartupGameFlag1 != 0)
        PlayerInitCapabilitiesVariant1Pc(object);
    else
        PlayerInitCapabilitiesDefaultPc(object);
    can_pull_levers = CanPullLevers((i16)object->id);
    object->ai_packet.flags &= ~0x10000u;
    object->flags_ef0 = (object->flags_ef0 & ~0x800000u) | (((u32)can_pull_levers << 23) & 0x800000u);
    object->ai_packet.route_mask_low = 0;
    object->ai_packet.route_mask_high = 0;
    if (FreePlay != 0 && (object->flags_1f8 & 0x400) == 0) {
        memcpy(&route_mask_word, (u8 *)&CharacterMaskAllPc + offsetof(PLAYER_ROUTE_MASK_WORDS_PC, low),
               sizeof(route_mask_word));
        object->ai_packet.route_mask_low = route_mask_word;
        memcpy(&route_mask_word, (u8 *)&CharacterMaskAllPc + offsetof(PLAYER_ROUTE_MASK_WORDS_PC, high),
               sizeof(route_mask_word));
        object->ai_packet.route_mask_high = route_mask_word;
    } else if (SpecialRouteCharacterTypeIDFn != NULL) {
        name = object->suit != NULL ? ((PLAYER_SUIT_NAME_PC *)object->suit)->name : object->character_data->name;
        type = SpecialRouteCharacterTypeIDFn(name);
        if (type != 0xff) {
            if (type < 64) {
                route_mask = (u64)1 << type;
                object->ai_packet.route_mask_low = (u32)route_mask;
                object->ai_packet.route_mask_high = (u32)(route_mask >> 32);
            } else {
                memcpy(&route_mask_word, (u8 *)&CharacterMaskAllPc + offsetof(PLAYER_ROUTE_MASK_WORDS_PC, low),
                       sizeof(route_mask_word));
                object->ai_packet.route_mask_low = route_mask_word;
                memcpy(&route_mask_word, (u8 *)&CharacterMaskAllPc + offsetof(PLAYER_ROUTE_MASK_WORDS_PC, high),
                       sizeof(route_mask_word));
                object->ai_packet.route_mask_high = route_mask_word;
            }
        }
    }
    object->run_speed_override = 1000000000.0f;
    object->walk_speed_override = 1000000000.0f;
    character = object->character_data;
    object->ai_packet.creature_set = 0;
    object->portal_flags |= 2;
    if ((character->model_flags & 0x80) != 0)
        object->portal_flags |= 4;
    else
        object->portal_flags &= ~4u;
    object->flags_ef0 &= 0xc7ffe6c0;
    object->hover_height_override = 1000000000.0f;
    object->flags_ef4 &= 0x31f7e95c;
    object->collision_scale_override = 1000000000.0f;
    object->flags_ef8_word &= 0xfffffffc;
    object->portal_flags = (object->portal_flags & 0x25fffc27) | 8;
    object->doomed_escape_locator = NULL;
    if ((object->flags_1f8 & 0x400) != 0)
        object->flags_ef4 &= ~0x40000u;
    else
        object->flags_ef4 |= 0x40000u;
    object->extra_layer_mask = 0;
    object->ai_packet.movement_target_radius = DEFAULT_MOVE_RANGE;
    object->ai_packet.flags &= ~0x10000000u;
    if (object->ai_packet.movement_target_radius > 0.0f)
        object->ai_packet.flags = (object->ai_packet.flags & ~0x8000000u) | 0x4000000u;
    else
        object->ai_packet.flags &= ~0xc000000u;
    object->flags_ef4 &= 0xfe5fffff;
    object->portal_flags &= 0xfff3ffff;
    if ((character->model_flags & 0x8000000) != 0)
        object->portal_flags |= 0x100000;
    else
        object->portal_flags &= ~0x100000u;
    object->movement_speed_scale_pc = 1.0f;
    object->tag_flags &= ~2u;
    object->field_ec4 = 0.0f;
    object->ai_packet.flags &= 0xffffafff;
    object->ai_packet.target_metric_e0 = 1000000000.0f;
    object->ai_packet.target_metric_f0 = 1000000000.0f;
    object->ai_packet.antinode_timer = 0.0f;
    object->portal_flags &= 0xfadff7fe;
    object->timer_ecc = 0.0f;
    object->flags_ef0 &= ~0x1000000u;
    object->alert_target = NULL;
    object->ai_packet.nearest_opponent = NULL;
    object->ai_packet.target_dc = NULL;
    object->ai_packet.opponent = NULL;
    object->ai_packet.target_ec = NULL;
    object->ai_awareness_mask = 0;
    object->ai_seen_mask = 0;
    object->field_eb0 = 0;
    object->field_eb4 = 0;
    object->opponent = NULL;
    object->last_attacker = NULL;
    object->ai_opponent_exclusion_mask[0] = 0;
    object->ai_opponent_exclusion_mask[1] = 0;
    memset(object->joint_overrides, 0, 2 * sizeof(object->joint_overrides[0]));
    character = object->character_data;
    object->joint_override_count = 0;
    object->head_target = NULL;
    object->ai_packet.dont_avoid_character = NULL;
    object->collision_link = NULL;
    object->collision_exclusion_mask[0] = 0;
    object->collision_exclusion_mask[1] = 0;
    if ((character->model_flags & 0x4000) != 0 && (object->field_280 == 0xff || object->rider != NULL))
        object->flags_1fc_word |= 2;
    else
        object->flags_1fc_word &= ~2u;
    if ((character->game_character->flags_090 & 0x2000) != 0)
        object->flags_ef0 |= 0x80000000u;
    else
        object->flags_ef0 &= ~0x80000000u;
    object->flags_1fc_word &= 0xffe7f7ff;
    object->flags_ef4 &= ~4u;
    object->animation_speed_multiplier = 1.0f;
    object->flags_ef0 &= 0xfbbfffff;
    object->flags_ef8_word &= 0xfffffff3;
    object->field_1038 = 0.0f;
    object->indy_climb = NULL;
    object->field_1083 = 0;
    object->gizforce_target = NULL;
    ResetPlayerAI(object);
}

void SetPlayerGroupPosition(f32 x, f32 y, f32 z) {
    i32 i;
    NUVEC offset = {0.0f, 0.0f, 0.2f};
    for (i = 0; i < PLAYERCOUNT; i++) {
        NUVEC position = {x, y, z};
        NuVecAdd(&position, &position, &offset);
        ResetPlayer(Player[i], 1, &position, 1);
        NuVecRotateY(&offset, &offset, 0x2000);
    }
}

GAMEOBJECT *__cdecl AddCreature(i32 id, i32 parameter) {
    GAMEOBJECT *object;
    if ((u32)id > 0x153 || apicharsys->playermodelids[id] == -1)
        return NULL;
    object = AddGameObject();
    if (object == NULL)
        return NULL;
    InitCreature(object, id, parameter);
    return object;
}

NUVEC *__cdecl Player_StartPos(GAMEOBJECT *object) {
    i32 index;
    NUVEC *position;
    if ((i8)object->field_280 < 0 || (i8)object->field_280 >= 8)
        index = object->hostility_group;
    else
        index = (i8)object->field_280;
    index %= 8;
    position = PlayerStartRecordInitPc[index].position;
    return position != NULL ? position : PlayerStartRecordInitPc[0].position;
}

/* Genuine original call declarations: no placeholder implementations. */
GAMEOBJECT *__cdecl AddCreature(i32 model, i32 mode);
void __cdecl InitPlayerAI(GAMEOBJECT *object);
void __cdecl ResetPlayerMoves(GAMEOBJECT *object);
void __cdecl ResetRumble(RUMBLE_PACKET_RESOURCE_PC *packet);
void __cdecl ResetLights(NUVEC *position, RTL_DATA_PC *packet, void *set);
void __cdecl rtlResetEx(RTL_DATA_PC *packet, i32 clear_cache);
void __cdecl rtlApplySetScale(void *set, RTL_DATA_PC *packet, NUVEC *position, NUMTX *rotation, i32 identity,
                              f32 scale);
extern i32 NOAICREATURES;
extern i32 complexsockposition_forcesock;

#include <string.h>

extern i32 addcreature_override_id_check;
extern f32 default_mover_extra;
extern i16 id_MOSEISLEYCITIZEN, id_CANTINAALIEN, id_CLOUDCITYCITIZEN;
extern NUVEC *__cdecl Player_StartPos(GAMEOBJECT *);
extern GAMEPAD_PC *__cdecl GamePad_Allocate(void);
extern SUIT_LAYERS_PC *__cdecl Suit_GetDefault(i32);

typedef struct CREATURE_RESET_PACKET_VIEW_PC {
    u8 unknown_000[0x6c4];
    u8 packet[0x788];
} CREATURE_RESET_PACKET_VIEW_PC;
STATIC_ASSERT(offsetof(CREATURE_RESET_PACKET_VIEW_PC, packet) == 0x6c4);
STATIC_ASSERT(sizeof(((CREATURE_RESET_PACKET_VIEW_PC *)0)->packet) == 0x788);

static const u8 CreatureMosHatPc[5] = {0, 0, 0, 7, 14};
static const u8 CreatureMosHeadPc[5] = {20, 20, 8, 8, 4};
static const u8 CreatureMosBodyPc[3] = {3, 9, 19};
static const u8 CreatureMosArmsPc[3] = {1, 12, 15};
static const u8 CreatureMosHandsPc[3] = {2, 13, 16};
static const u8 CreatureMosWaistPc[3] = {6, 10, 17};
static const u8 CreatureMosLegsPc[3] = {5, 11, 18};
static const u8 CreatureCantinaHeadPc[3] = {0, 3, 6};
static const u8 CreatureCantinaBodyPc[3] = {1, 4, 7};
static const u8 CreatureCantinaLegsPc[3] = {2, 5, 8};
static const u8 CreatureCloudHeadPc[3] = {1, 5, 7};
static const u8 CreatureCloudBodyPc[3] = {2, 3, 6};
static const u8 CreatureCloudLegsPc[3] = {0, 4, 8};

static void SetLayersMosPc(GAMEOBJECT *object) {
    object->layer_mask = 0;
    object->layer_mask |= 1u << CreatureMosHatPc[qrand() / 0x3334];
    object->layer_mask |= 1u << CreatureMosHeadPc[qrand() / 0x3334];
    object->layer_mask |= 1u << CreatureMosBodyPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureMosArmsPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureMosHandsPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureMosWaistPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureMosLegsPc[qrand() / 0x5556];
}
static void SetLayersCantinaPc(GAMEOBJECT *object) {
    object->layer_mask = 0;
    object->layer_mask |= 1u << CreatureCantinaHeadPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureCantinaBodyPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureCantinaLegsPc[qrand() / 0x5556];
}
static void SetLayersCloudPc(GAMEOBJECT *object) {
    object->layer_mask = 0;
    object->layer_mask |= 1u << CreatureCloudHeadPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureCloudBodyPc[qrand() / 0x5556];
    object->layer_mask |= 1u << CreatureCloudLegsPc[qrand() / 0x5556];
    object->flags_ef4 = (object->flags_ef4 & ~0x800u) | ((object->layer_mask << 4) & 0x800);
}
typedef struct PLAYER_LAYER_FLAGS_PC {
    u32 unknown_0 : 9;
    u32 bob : 1;
    u32 unknown_10 : 22;
} PLAYER_LAYER_FLAGS_PC;
STATIC_ASSERT(sizeof(PLAYER_LAYER_FLAGS_PC) == sizeof(((GAMEOBJECT *)0)->flags_ef0));
static void SetLayersBobPc(GAMEOBJECT *object) {
    i32 head[6] = {1, 2, 3, 21, 22, 23};
    i32 body[8] = {5, 6, 7, 8, 9, 10, 16, 17};
    i32 legs[3] = {11, 12, 15};
    i32 arms[2] = {13, 14};
    i32 hands[2] = {19, 20};
    ((PLAYER_LAYER_FLAGS_PC *)&object->flags_ef0)->bob = qrand() < 0x8000;
    if (qrand() < 0x8000)
        object->layer_mask |= 0x10;
    object->layer_mask = 1;
    object->layer_mask |= 1u << head[qrand() / 0x2aab];
    object->layer_mask |= 1u << body[qrand() / 0x2000];
    object->layer_mask |= 1u << legs[qrand() / 0x5556];
    object->layer_mask |= 1u << arms[qrand() / 0x8000];
    object->layer_mask |= 1u << hands[qrand() / 0x8000];
}
static __declspec(noinline) void InitCreatureModelFlagsPc(GAMEOBJECT *object) {
    if ((object->character_data->game_character->flags_090 & 0x8000) != 0)
        object->flags_1f8 |= 0x20000;
    else
        object->flags_1f8 &= ~0x20000u;
}

void ClearAICreatures() {
    i32 i;
    GAMEOBJECT *object = Obj;
    for (i = 0; i < HIGHGAMEOBJECT; ++i, ++object) {
        if ((object->flags_1fc & 1) != 0 && (object->flags_1f8 & 0x400) != 0) {
            FreeTorpedoPacket(&object->torpedo);
            RemoveGameObject(object, 1);
        }
    }
}

i32 __cdecl InitCreature(GAMEOBJECT *object, i32 id, i32 parameter) {
    NUVEC start;
    void **animations;
    i32 animation;
    addcreature_override_id_check = 0;
    if (id < 0)
        return 0;
    if (id >= 0x154)
        return 0;
    if (apicharsys->playermodelids[id] == -1)
        return 0;
    object->character_data = (CHARACTERDATA_PC *)&apicharsys->char_data[id];
    object->layer_mask = object->character_data->game_character->layer_mask;
    object->flags_1f8 = 0;
    object->level_object_index = -1;
    memset(((CREATURE_RESET_PACKET_VIEW_PC *)object)->packet, 0, 0x788);
    object->field_280 = -1;
    start = *Player_StartPos(object);
    object->flags_1f8 |= 2;
    if (parameter != 0)
        object->flags_1f8 |= 0x4000;
    if ((WORLD->current_level->flags & 0x40000) == 0)
        object->flags_1f8 |= 0x40;
    object->pad = GamePad_Allocate();
    object->pad->input_mode = 1;
    object->current_hp = object->field_107e = object->character_data->game_character->hitpoints;
    ResetPlayerPacket(((CREATURE_RESET_PACKET_VIEW_PC *)object)->packet, object->character_data);
    object->movement_direction = v000;
    object->shield_hitpoints = object->character_data->game_character->shield_hitpoints;
    object->saved_hitpoints = 4;
    object->id = (u16)id;
    object->character_model = &apicharsys->models[apicharsys->playermodelids[id]];
    object->suit = Suit_GetDefault((i16)id);
    object->ai_packet.creature_index = 0xff;
    SetGameObjectCharacterData(object);
    object->death_delay = 0.5f;
    object->start_position = start;
    object->position = start;
    object->initial_position = start;
    object->collision_position = start;
    GetTopBot(object);
    GameObjectDimensions(object);
    object->animation_index = 1;
    if (object->character_model != NULL) {
        animations = object->character_model->action_animations;
        if (animations[1] == NULL) {
            for (animation = 0; animation < 0xe9; ++animation, ++animations)
                if (*animations != NULL)
                    break;
            if (animation < 0xe9)
                object->animation_index = (i16)animation;
        }
    }
    ResetAnimPacket(&object->anim_packet, object->animation_index);
    ResetCharacterIdle(object, 2, GetDefaultIdle(object));
    ResetLights(&object->position, &object->lighting_packet, WORLD->rtl_set);
    object->ai_packet.mover_height = object->bounds_radius + default_mover_extra;
    GameObjectOrigin(object);
    object->last_safe_position = object->position;
    object->ai_update_position = object->position;
    object->flags_ef4 =
        (object->flags_ef4 & ~0x800u) | ((object->character_data->game_character->flags_090 >> 6) & 0x800);
    if (object->id == (u16)id_MOSEISLEYCITIZEN)
        SetLayersMosPc(object);
    else if (object->id == (u16)id_CANTINAALIEN)
        SetLayersCantinaPc(object);
    else if (object->id == (u16)id_CLOUDCITYCITIZEN)
        SetLayersCloudPc(object);
    else if (object->id == (u16)id_BOB)
        SetLayersBobPc(object);
    else if (object->id == (u16)id_GEONOSIAN)
        ((PLAYER_LAYER_FLAGS_PC *)&object->flags_ef0)->bob = qrand() < 0x8000;
    InitCreatureModelFlagsPc(object);
    return 1;
}

static void DrawWeapon_SetSabreObjects(GAMEOBJECT *object, i32 red, i32 green, i32 blue, i32 purple, i32 *models,
                                       i32 *hilt) {
    if (red || green || blue || purple) {
        if (object->id == (u16)id_DARTHMAUL) {
            models[0] = *hilt = 0x12;
        } else if (object->id == (u16)id_DOOKU && WORLD->level_objects[0x13].active) {
            models[0] = *hilt = 0x13;
        } else {
            models[0] = *hilt = 0x11;
        }
    }
    if (red) {
        if (object->id == (u16)id_DARTHMAUL) {
            if (object->dead != 0)
                return;
            models[1] = 0x6d;
            models[2] = 0x6e;
            if (object->flags_e20 & 0x80000)
                models[3] = 0x6e;
        } else {
            if (object->dead != 0)
                return;
            models[1] = 0x65;
            models[2] = 0x66;
            if (object->flags_e20 & 0x80000)
                models[3] = 0x66;
        }
        return;
    }

    if (green) {
        if (object->dead != 0)
            return;
        models[1] = 0x67;
        models[2] = 0x68;
        if (object->flags_e20 & 0x80000)
            models[3] = 0x68;
        return;
    }

    if (blue) {
        if (object->dead == 0) {
            models[1] = 0x69;
            models[2] = 0x6a;
            if (object->flags_e20 & 0x80000) {
                models[3] = 0x6a;
            }
        }
    } else if (purple && object->dead == 0) {
        models[1] = 0x6b;
        models[2] = 0x6c;
        if (object->flags_e20 & 0x80000) {
            models[3] = 0x6c;
        }
    }
}

extern i16 CharacterFixupIdPc007f1810;
STATIC_ASSERT(offsetof(GAMEOBJECT, weapon_model_override_pc) == 0x7f4);

void DrawWeapons(GAMEOBJECT *object, i32 reflection, f32 weapon_scale) {
    PLAYERCHARACTERCONFIG_PC *data;
    NUMTX blade_matrix;
    NUMTX hilt_matrix;
    NUMTX reflected;
    NUMTX saved_matrix;
    NUVEC scale;
    NUVEC hilt_scale;
    NUMTX *matrix;
    i32 hand;
    i32 joint;
    i32 part;
    i32 count;
    i32 side;
    i32 passes;
    i32 model;
    i32 reflected_model;
    i32 red, green, blue, purple;
    u16 rotation = 0;
    i32 models[4] = {-1, -1, -1, -1};
    i32 hilt = -1;
    i32 reflected_models[4] = {-1, -1, -1, -1};
    i32 sabre = 0;
    if (weapon_scale <= 0.0f)
        return;
    if ((object->character_data->game_character->flags_094 & 0x200000) != 0)
        return;
    for (hand = 0; hand < 4; ++hand) {
        joint = object->character_data->game_character->weapon_joints[hand];
        if (joint != -1 && object->character_model->points_of_interest[joint] != NULL) {
            red = 0;
            green = 0;
            blue = 0;
            purple = 0;
            if (Cheat_IsOn(15) && (object->character_data->game_character->weapon_action == 1 ||
                                   object->character_data->game_character->weapon_action == 8)) {
                models[0] = 0x59;
            } else {
                data = object->character_data->game_character;
                models[0] = data->weapon_model;
            }
            if (models[0] != -1) {
                if (object->id == (u16)id_JANGOFETT) {
                    rotation = 0xd1c8;
                } else if (models[0] == 0x65 || models[0] == 0x67 || models[0] == 0x69 || models[0] == 0x6b) {
                    if (object->field_280 != 0xff && Cheat_IsOn(25))
                        red = 1;
                    else if (object->field_280 != 0xff && Player_HasPurpleForce(object))
                        purple = 1;
                    else if (models[0] == 0x65)
                        red = 1;
                    else if (models[0] == 0x67)
                        green = 1;
                    else if (models[0] == 0x69)
                        blue = 1;
                    else
                        purple = 1;
                    DrawWeapon_SetSabreObjects(object, red, green, blue, purple, models, &hilt);
                    sabre = 1;
                }
            } else if (StartupGameFlag0 != 0) {
                if (object->weapon_model_override_pc != NULL)
                    models[0] = *object->weapon_model_override_pc;
                if (models[0] == -1)
                    continue;
            } else if (StartupGameFlag1 != 0) {
                if ((object->character_data->model_flags & 0x90) == 0x80) {
                    models[0] = 0xd;
                } else {
                    if (object->id == (u16)id_BOB) {
                        if ((object->flags_ef0 & 0x200) != 0)
                            green = 1;
                        else
                            blue = 1;
                    } else if (AnakinGreenSabre(object)) {
                        green = 1;
                    } else if (data->streak_color == 0) {
                        red = 1;
                    } else if (data->streak_color == 1) {
                        green = 1;
                    } else if (data->streak_color == 2) {
                        blue = 1;
                    } else if (data->streak_color == 3) {
                        purple = 1;
                    }
                    if (object->field_280 != 0xff && Cheat_IsOn(25)) {
                        red = 1;
                        green = blue = purple = 0;
                    } else if (red || green || blue || purple) {
                        if (object->field_280 != 0xff && Player_HasPurpleForce(object)) {
                            purple = 1;
                            red = green = blue = 0;
                        } else if (green && object->id == (u16)id_GRIEVOUS && (hand == 0 || hand == 3)) {
                            blue = 1;
                            green = 0;
                        }
                    }
                    if (red || green || blue || purple) {
                        DrawWeapon_SetSabreObjects(object, red, green, blue, purple, models, &hilt);
                        sabre = 1;
                    }
                }
            } else if (object->id == (u16)CharacterFixupIdPc007f1810 ||
                       (object->character_data->model_flags & 0x80) != 0) {
                models[0] = 0xc;
            }
            count = 0;
            for (part = 0; part < 4; ++part) {
                if (models[part] != -1) {
                    if (part != 3)
                        ++count;
                    reflected_models[part] = LevelObject_GetReflection(models[part]);
                }
            }
            if (count > 0) {
                blade_matrix = object->locator_matrices[joint];
                scale.z = scale.y = scale.x = weapon_scale;
                if (hilt != -1) {
                    hilt_scale.x = weapon_scale + weapon_scale;
                    hilt_matrix = object->locator_matrices[joint];
                    if (hilt_scale.x > 1.0f)
                        hilt_scale.x = 1.0f;
                    hilt_scale.z = hilt_scale.x;
                    hilt_scale.y = hilt_scale.x;
                    NuMtxPreScale(&hilt_matrix, &hilt_scale);
                }
                side = 0;
                if (rotation != 0)
                    NuMtxPreRotateX(&blade_matrix, rotation);
                NuMtxPreScale(&blade_matrix, &scale);
                saved_matrix = blade_matrix;
                passes = 1;
                do {
                    for (part = 0; part < 4; ++part) {
                        if (side && models[part] == hilt)
                            continue;
                        model = models[part];
                        if (model == -1)
                            continue;
                        if (part == 3) {
                            blade_matrix.m30 += object->weapon_trail_offset.x;
                            blade_matrix.m31 += object->weapon_trail_offset.y;
                            blade_matrix.m32 += object->weapon_trail_offset.z;
                        }
                        matrix = model == hilt ? &hilt_matrix : &blade_matrix;
                        Draw3DObjectMtx(NULL, model, matrix);
                        if (reflection) {
                            reflected_model = reflected_models[part];
                            if (reflected_model == -1 || !WORLD->level_objects[reflected_model].active)
                                reflected_model = model;
                            if (MatrixReflection(matrix, object->reflection_enabled, object->reflection_height,
                                                 WORLD->current_level->reflection_y, &reflected)) {
                                NuRndrStartReflectionRender(0);
                                Draw3DObjectMtx(NULL, reflected_model, &reflected);
                                NuRndrEndReflectionRender();
                            }
                        }
                    }
                    if (!side && object->id == (u16)id_DARTHMAUL &&
                        ((object->field_280 != 0xff && Cheat_IsOn(25)) ||
                         (object->field_280 != 0xff && Player_HasPurpleForce(object)))) {
                        scale.z = scale.x = 1.0f;
                        scale.y = -1.0f;
                        blade_matrix = saved_matrix;
                        side = 1;
                        NuMtxPreScale(&blade_matrix, &scale);
                        ++passes;
                    }
                } while (--passes > 0);
            }
        }
        if (sabre && object->id != (u16)id_GRIEVOUS)
            return;
    }
}

i32 __cdecl GameDrawCharacterModel(CHARACTER_MODEL_LAYERS_PC *, ANIMPACKET_PC *, NUMTX *, NUMTX *, NUMTX *, NUMTX *,
                                   GAMEOBJECT *, u32);
void __cdecl DrawParaphernaliaPc(GAMEOBJECT *object);

typedef struct PARAPHERNALIA_EXTRA_PC {
    u8 prefix[0xc68];
    NUVEC force_position;
    u8 gap_c74[0x10c];
    f32 force_alpha;
    u8 gap_d84[8];
    f32 force_radius;
    u8 gap_d90[0x2ec];
    u8 draw_reflection;
} PARAPHERNALIA_EXTRA_PC;
STATIC_ASSERT(offsetof(PARAPHERNALIA_EXTRA_PC, force_position) == 0xc68);
STATIC_ASSERT(offsetof(PARAPHERNALIA_EXTRA_PC, force_alpha) == 0xd80);
STATIC_ASSERT(offsetof(PARAPHERNALIA_EXTRA_PC, force_radius) == 0xd8c);
STATIC_ASSERT(offsetof(PARAPHERNALIA_EXTRA_PC, draw_reflection) == 0x107c);

void __cdecl DrawForceGlowSpritePc(NUVEC *, f32, i32, f32, GAMEOBJECT *);
void __cdecl DrawSnakeBody(GAMEOBJECT *);

void DrawGameObjectsDraw(i32 pass) {
    GAMEOBJECT *object;
    i32 index;
    u8 drawn;

    EnableShadowMapRendering(0);
    object = Obj;
    for (index = 0; index < HIGHGAMEOBJECT; index++, object++) {
        if ((object->flags_1f8 & 0x800) != 0) {
            if (object->rider != NULL && object->character_context == 0x3b) {
                object->model_draw_result = object->rider->model_draw_result;
            }
            continue;
        }
        drawn = (u8)GameDrawCharacterModel(object->character_model, &object->anim_packet, &object->transform,
                                           (object->flags_ef0 & 0x20000) != 0 ? &object->draw_secondary_mtx : NULL,
                                           (u8)object->field_107c != 0 ? &object->draw_tertiary_mtx : NULL,
                                           object->locator_matrices, object, object->layer_mask);
        object->model_draw_result = drawn;
        object->flags_e24 ^= ((drawcharactermodel_locatorsupdated << 3) ^ object->flags_e24) & 8;
        if (drawn != 0) {
            DrawParaphernaliaPc(object);
        } else if (((object->flags_e20 & 0x400) != 0 || object->character_context == 8) &&
                   object->transform_alpha > 0.0f && object->transform_target_scale > 0.0f &&
                   WORLD->level_objects[object->field_e1e].active != 0 &&
                   (object->character_data->game_character->flags_090 & 0x400) == 0) {
            DrawForceGlowSpritePc(&((PARAPHERNALIA_EXTRA_PC *)object)->force_position, object->transform_target_scale,
                                  object->field_e1e, (f32)(object->transform_alpha / FORCEGLOWTIME), object);
        }
        if (object->snake_body != NULL) {
            DrawSnakeBody(object);
        }
        object->origin_joint_ready = 1;
        if (object->model_draw_result != 0) {
            object->flags_ef0 |= 0x40000;
        }
        if (Paused == 0 && object->character_data->draw_fn != NULL) {
            ((void(__cdecl *)(GAMEOBJECT *))object->character_data->draw_fn)(object);
        }
    }
    ResetShadowMapRendering();
}

extern f32 DrawGameObjectsFarClipPc, DrawGameObjectsVehicleFarClipPc;
void DrawGameObjects() {
    const f32 saved_far_clip = global_camera.unknown_64;
    global_camera.unknown_64 = VehicleArea != 0 ? DrawGameObjectsVehicleFarClipPc : DrawGameObjectsFarClipPc;
    if (global_camera.unknown_64 > saved_far_clip)
        global_camera.unknown_64 = saved_far_clip;
    NuCameraSet(&global_camera);
    DrawGameObjectsDraw(0);
    global_camera.unknown_64 = saved_far_clip;
    NuCameraSet(&global_camera);
}

void __cdecl Shards_HandleLostObj(WORLDINFO *, GAMEOBJECT *);
void __cdecl DestroySnakeBody(GAMEOBJECT *);
void __cdecl GizForce_ResetLOS(GAMEOBJECT *);

i32 __cdecl NewPlayerCharacter(GAMEOBJECT *object, i32 id, i32 old_id, i32 unused) {
    PLAYERCHARACTERCONFIG_PC *config;
    NUVEC velocity;
    AIPATHINFO_PC path;
    AILOCATOR_PC *saved_locator;
    u32 saved_f08, saved_frame_state, saved_ai_flag, lean_enabled, saved_flag;
    i16 lean;
    i8 context;
    u8 health, hitpoints, route, next_route;
    i32 on_surface;
    if (id == old_id || id < 0 || id >= CHARCOUNT || apicharsys->playermodelids[id] == -1)
        return 0;
    Shards_HandleLostObj(WORLD, object);
    if (VehicleArea == 0)
        AddGameDebris(WORLD->game_debris_sys, 0x5c, &object->collision_position);
    context = object->character_context;
    lean = (i16)object->movement_lean_angle;
    lean_enabled = object->character_data->model_flags & 0x2000;
    LoseHelmet(object, 0, 1);
    DestroySnakeBody(object);
    object->id = (u16)id;
    object->character_model = &apicharsys->models[apicharsys->playermodelids[id]];
    object->character_data = (CHARACTERDATA_PC *)&apicharsys->char_data[id];
    velocity = object->velocity;
    health = (u8)object->current_hp;
    hitpoints = object->field_107e;
    saved_flag = object->flags_1fc_word & 0x2000;
    object->takeover_owner = NULL;
    object->suit = Suit_GetDefault(id);
    saved_locator = object->doomed_escape_locator;
    saved_f08 = object->route_frame_state;
    saved_ai_flag = object->flags_ef0 & 0x200000;
    route = object->ai_packet.current_route;
    next_route = object->ai_packet.next_route;
    path = object->ai_packet.path_info;
    saved_frame_state = object->ai_packet.navigation_flags;
    InitPlayerAI(object);
    object->flags_ef0 = (object->flags_ef0 & ~0x200000u) | saved_ai_flag;
    object->doomed_escape_locator = saved_locator;
    object->route_frame_state = saved_f08;
    object->ai_packet.current_route = route;
    object->ai_packet.next_route = next_route;
    object->ai_packet.path_info = path;
    object->ai_packet.navigation_flags = saved_frame_state;
    object->collision_scale = 1.0f;
    config = object->character_data->game_character;
    object->layer_mask = config->layer_mask;
    object->shield_hitpoints = config->shield_hitpoints;
    object->saved_hitpoints = 4;
    Player_ClearContext(object, 1);
    ResetPlayerMoves(object);
    SetGameObjectCharacterData(object);
    ResetAnimPacket(&object->anim_packet, -1);
    Player_ResetContexts(((CREATURE_RESET_PACKET_VIEW_PC *)object)->packet);
    GetTopBot(object);
    GameObjectDimensions(object);
    object->hud_icon_timer = 2.0f;
    object->input_toggle_hold_time = TOGGLEHOLDTIME;
    object->flags_ef0 |= 0x80;
    InitSurfaceInfo(object);
    on_surface = SetObjOnSurface(object, 1);
    GizForce_ResetLOS(object);
    object->tag_reset_timer = 0.0f;
    object->pause_context_state = 0.0f;
    object->flags_ef0 &= ~0x80000u;
    object->current_hp = (i8)health;
    object->field_107e = hitpoints;
    object->flags_1fc_word = (object->flags_1fc_word & ~0x2000u) | saved_flag;
    ResetMiniAnimPacket((MINIANIMPACKET_PC *)object->mini_anim_packet, -1);
    object->weapon_scale = (object->flags_e20 & 0x10000) != 0 ? 1.0f : 0.0f;
    object->field_e33 = 0;
    if (on_surface != 0)
        ((PLAYER_RESET_FIELDS_PC *)object)->ground_contact_grace_timer = 0.2f;
    else
        object->velocity.y = velocity.y;
    if (context == 0x33 && StartSlide(object, 0)) {
        object->velocity.x = velocity.x;
        object->velocity.z = velocity.z;
    }
    if (object->velocity.y == 0.0f)
        object->velocity.y = -0.1f;
    if (lean_enabled != 0 && (object->character_data->model_flags & 0x2000) != 0)
        object->movement_lean_angle = (u16)lean;
    config = apicharsys->char_data[(i16)object->id].game_character;
    object->view_distance = config->viewdistance;
    object->hear_distance = config->heardistance;
    object->max_view_height = config->maxviewheight;
    object->min_view_height = config->minviewheight;
    config = object->character_data->game_character;
    object->flags_ef4 = (object->flags_ef4 & ~0x800u) | ((config->flags_090 >> 6) & 0x800);
    if (object->id == (u16)id_MOSEISLEYCITIZEN)
        SetLayersMosPc(object);
    else if (object->id == (u16)id_CANTINAALIEN)
        SetLayersCantinaPc(object);
    else if (object->id == (u16)id_CLOUDCITYCITIZEN)
        SetLayersCloudPc(object);
    else if (object->id == (u16)id_BOB)
        SetLayersBobPc(object);
    else if (object->id == (u16)id_GEONOSIAN)
        object->flags_ef0 = (object->flags_ef0 & ~0x200u) | ((qrand() < 0x8000) ? 0x200 : 0);
    InitCreatureModelFlagsPc(object);
    return 1;
}

typedef struct player_init_progress_pc_s {
    u32 coins;
    i16 previous_state;
    u8 active, stored_flag;
    i8 hitpoints;
    u8 unknown_09, helmet, unknown_0b;
    f32 powerup_time;
} PLAYER_INIT_PROGRESS_PC;
STATIC_ASSERT(sizeof(PLAYER_INIT_PROGRESS_PC) == 0x10);
STATIC_ASSERT(offsetof(PLAYER_INIT_PROGRESS_PC, previous_state) == 4);
STATIC_ASSERT(offsetof(PLAYER_INIT_PROGRESS_PC, active) == 6);
STATIC_ASSERT(offsetof(PLAYER_INIT_PROGRESS_PC, stored_flag) == 7);
STATIC_ASSERT(offsetof(PLAYER_INIT_PROGRESS_PC, hitpoints) == 8);
STATIC_ASSERT(offsetof(PLAYER_INIT_PROGRESS_PC, helmet) == 0xa);
STATIC_ASSERT(offsetof(PLAYER_INIT_PROGRESS_PC, powerup_time) == 0xc);
typedef struct player_init_extra_pc_s {
    u8 unknown_0000[0x454];
    f32 field_454;
    u8 unknown_0458[0xcc0 - 0x458];
    PLAYER_BATARANG_SLOT_PC *batarang;
    u8 unknown_0cc4[0x1062 - 0xcc4];
    i16 previous_state;
} PLAYER_INIT_EXTRA_PC;
STATIC_ASSERT(offsetof(PLAYER_INIT_EXTRA_PC, batarang) == 0xcc0);
STATIC_ASSERT(offsetof(PLAYER_INIT_EXTRA_PC, field_454) == 0x454);
STATIC_ASSERT(offsetof(PLAYER_INIT_EXTRA_PC, previous_state) == 0x1062);

void __cdecl RememberPlayerIDs(i32 force, i32 first_id, i32 second_id) {
    i32 ids[2], index;
    if (VehicleArea != 0 || GAMEDEMO != 0)
        return;
    if (force == 0 && (!(WORLD->current_level->flags & 2) || (WORLD->current_level->flags & 0x4e0)))
        return;
    ids[0] = first_id;
    ids[1] = second_id;
    for (index = 0; index < 2; ++index) {
        i32 id = ids[index];
        if (id != -1 && (CDataList[id].model_flags & 0x2000) == 0 && id != PlayerID[index] && Collection_Got(id) == 1 &&
            GCDataList[id].hover_fall_mode != 0 && id != PlayerID[(index - 1) & 1])
            PlayerID[index] = id;
    }
    if (first_id != second_id && first_id == PlayerID[1] && second_id == PlayerID[0]) {
        index = PlayerID[0];
        PlayerID[0] = PlayerID[1];
        PlayerID[1] = index;
    }
}

void __cdecl Players_Init(void) {
    GAMEOBJECT **players = (GAMEOBJECT **)Player;
    PLAYER_INIT_PROGRESS_PC *progress = (PLAYER_INIT_PROGRESS_PC *)PlayerProgress;
    i16 *list;
    i32 index;
    for (index = 0; index < 8; ++index)
        players[index] = NULL;
    if (netclient != 0)
        return;
    PLAYERCOUNT = 0;
    if ((WORLD->current_level->flags & 2) != 0) {
        if (FreePlay == 0 && (Hub_UsePlayerList == 0 || HUB_ADATA == NULL || WORLD->area != HUB_ADATA) &&
            (UsePlayerList != 1 ||
             (PlayerList[0] != -1 && (apicharsys->playermodelids[PlayerList[0]] == -1 ||
                                      (PlayerList[1] != -1 && apicharsys->playermodelids[PlayerList[1]] == -1)))))
            list = Area_PlayerIDList;
        else
            list = PlayerList;
        if (LevelChangesInArea == 0 && UsePlayerList != 0 && progress[0].active == 0 && progress[1].active != 0 &&
            FreePlay == 0 && (WORLD->area->flags & 5) == 1 && list[0] != -1 &&
            list[0] == Area_StoryModelListPc[0].model_id && list[1] != -1 &&
            list[1] == Area_StoryModelListPc[1].model_id && list[0] != list[1]) {
            i16 first = list[0];
            list[0] = list[1];
            list[1] = first;
        }
        for (index = 0; list[index] != -1 && index < 8; ++index) {
            i32 id = list[index];
            GAMEOBJECT *object;
            PLAYER_INIT_EXTRA_PC *extra;
            APIOBJECT_PC *api;
            if ((u32)id > 0x153 || apicharsys->playermodelids[id] == -1)
                continue;
            object = AddGameObject();
            if (object == NULL)
                continue;
            InitCreature(object, id, 1);
            PreResetCode(object);
            PostResetCode(object);
            extra = (PLAYER_INIT_EXTRA_PC *)object;
            api = (APIOBJECT_PC *)object;
            players[PLAYERCOUNT] = object;
            object->previous_sock_position = &OldPlrSPos[PLAYERCOUNT];
            players[PLAYERCOUNT]->field_280 = (u8)PLAYERCOUNT;
            ((PLAYER_INIT_EXTRA_PC *)players[PLAYERCOUNT])->batarang = &BatarangPacketsPc[PLAYERCOUNT];
            players[PLAYERCOUNT]->torpedo = GetTorpedoPacket();
            ++PLAYERCOUNT;
            object->flags_1044 |= 1;
            object->field_107e = (u8)DEFAULT_PLAYERHITPOINTS;
            object->helmet = 0;
            if (UsePlayerList != 0) {
                i32 slot = (i8)object->field_280;
                object->player_controlled = progress[slot].active;
                if (Area != last_area) {
                    object->flags_e20 &= ~0x10000u;
                    extra->previous_state = 0;
                } else if (UsePlayerList == 1) {
                    object->flags_e20 ^= ((u32)progress[slot].stored_flag << 16 ^ object->flags_e20) & 0x10000;
                    object->current_hp = progress[slot].hitpoints;
                    if (object->current_hp > object->field_107e)
                        object->current_hp = (i8)object->field_107e;
                    object->helmet = (i8)progress[slot].helmet;
                    extra->previous_state = progress[slot].previous_state;
                    object->powerup_time = progress[slot].powerup_time;
                    object->suit = PlayerSuit[slot];
                    if (object->torpedo != NULL)
                        object->torpedo->count = PlayerTorpedoCount[slot];
                }
            } else {
                if (object->hostility_group == 0)
                    object->flags_1fc_word |= 0x80;
                else
                    object->flags_1fc_word &= ~0x80u;
                progress[object->hostility_group].hitpoints = object->current_hp;
                extra->previous_state = 0;
            }
            api->view_distance = apicharsys->char_data[(i16)object->id].game_character->viewdistance;
            api->hear_distance = apicharsys->char_data[(i16)object->id].game_character->heardistance;
            api->max_view_height = apicharsys->char_data[(i16)object->id].game_character->maxviewheight;
            api->min_view_height = apicharsys->char_data[(i16)object->id].game_character->minviewheight;
            object->ai_packet.flags &= ~0x5000u;
            object->portal_flags &= ~0x801u;
            object->flags_ef4 |= 0x40;
            object->ai_packet.target_metric_e0 = 1.0e9f;
            object->ai_packet.target_metric_f0 = 1.0e9f;
            object->ai_packet.nearest_opponent = NULL;
            object->ai_packet.target_dc = NULL;
            object->ai_packet.opponent = NULL;
            object->ai_packet.target_ec = NULL;
            object->field_ec4 = 0.0f;
            extra->field_454 = 0.0f;
            object->ai_awareness_mask = 0;
            object->ai_seen_mask = 0;
            object->field_eb0 = 0;
            object->field_eb4 = 0;
            object->opponent = NULL;
            object->last_attacker = NULL;
            object->alert_target = NULL;
            object->ai_opponent_exclusion_mask[0] = 0;
            object->ai_opponent_exclusion_mask[1] = 0;
            object->timer_ecc = 0.0f;
        }
    }
    UsePlayerList = 0;
    for (index = 0; index < 2; ++index) {
        if (players[index] != NULL) {
            PLAYER_COIN_PACKET_PC *coins;
            players[index]->coin_packet = &CoinPacket[index];
            coins = (PLAYER_COIN_PACKET_PC *)players[index]->coin_packet;
            coins->last_coin = LEGOOBJ_DEFAULTLASTCOIN.low_word;
            coins->coins = Area == last_area ? progress[index].coins : 0;
            players[index]->gizforce_los_info = &GizForceLOSInfo[index];
            memcpy(&BackUpPlayers[index], players[index], 0x10d8);
        }
    }
    RememberPlayerIDs(0, players[0] != NULL ? (i16)players[0]->id : -1, players[1] != NULL ? (i16)players[1]->id : -1);
}
static i32 Customise_FindLetter(char character) {
    i32 letter = 0;
    for (; letter != 38; ++letter)
        if (character == CustomiseLetter[letter])
            break;
    return letter == 38 ? 0 : letter;
}

typedef struct player_position_record_pc_s {
    NUVEC *position;
    i32 sock_location;
    f32 sock_ratio;
    i16 angle;
    u8 unknown_0e[2];
} PLAYER_POSITION_RECORD_PC;
typedef union player_position_sock_pc_u {
    SOCKPOSITION_PC sock;
    struct {
        i32 location;
        f32 ratio;
    } start;
} PLAYER_POSITION_SOCK_PC;
STATIC_ASSERT(sizeof(PLAYER_POSITION_RECORD_PC) == 16);
STATIC_ASSERT(offsetof(PLAYER_POSITION_RECORD_PC, position) == 0);
STATIC_ASSERT(offsetof(PLAYER_POSITION_RECORD_PC, sock_location) == 4);
STATIC_ASSERT(offsetof(PLAYER_POSITION_RECORD_PC, sock_ratio) == 8);
STATIC_ASSERT(offsetof(PLAYER_POSITION_RECORD_PC, angle) == 12);
STATIC_ASSERT(sizeof(PlayerList) == sizeof(PLAYER_POSITION_RECORD_PC));
STATIC_ASSERT(offsetof(PLAYER_POSITION_SOCK_PC, start.location) == 0);
STATIC_ASSERT(offsetof(PLAYER_POSITION_SOCK_PC, start.ratio) == 4);
extern char DoorExitNamePc[];
void *__cdecl Door_FindByName(WORLDINFO *, char *);
void *__cdecl Door_FindByIndex(WORLDINFO *, i32, i32, NUVEC *);
i32 __cdecl StartDoorPositions(void);
u32 __cdecl Episode_FindAreaFromFlags(EPISODEDATA_PC *, u32, u32);

extern i32 PlayerStartSplineIndexPc;
extern i32 hub_from_superstory;
extern i32 hub_from_mission;
extern i32 hub_startoutsidebonusdoor_area;
extern i32 shop_from_cutsceneplayer;
extern char DoorExitCameraSplineNamePc[32];
extern NUGSPLINE *DoorCutSplinePc;
extern NUVEC DoorCutCameraPosition0Pc;
extern NUVEC DoorCutCameraPosition1Pc;
extern char PlayerMissionStartDoorPc[];
extern char PlayerArcadeStartDoorPc[];
void __cdecl Players_InitPositions(WORLDINFO *world) {
    i32 bonus = world->area != NULL && (world->area->flags & 4) != 0;
    NUGSPLINE *spline = NULL;
    i32 first_pair = 0;
    i32 index;
    for (index = 0; index < 8; ++index) {
        PlayerStartRecordInitPc[index].position = &v000;
        PlayerStartRecordInitPc[index].angle = 0;
    }
    if (world->start_door != NULL && !bonus) {
        spline = world->start_door->spline;
        first_pair = 2;
    } else if (PlayerStartSplineIndexPc != -1) {
        spline = world->camera_splines[PlayerStartSplineIndexPc];
    }
    if (spline != NULL) {
        i32 pair = first_pair;
        for (index = 0; index < 8; ++index) {
            NUVEC direction;
            PlayerStartRecordInitPc[index].position = &spline->pts[pair * 2];
            NuVecSub(&direction, &spline->pts[pair * 2 + 1], PlayerStartRecordInitPc[index].position);
            PlayerStartRecordInitPc[index].angle = NuAtan2D(direction.x, direction.z);
            ++pair;
            if (pair * 2 + 2 > spline->length)
                pair = first_pair;
        }
        if (bonus && spline->length >= 4) {
            i32 count = spline->length / 4;
            i32 start = qrand() / (65535 / count + 1) * 4;
            PlayerStartRecordInitPc[0].position = &spline->pts[start];
            PlayerStartRecordInitPc[0].angle =
                NuAtan2D(spline->pts[start + 1].x - PlayerStartRecordInitPc[0].position->x,
                         spline->pts[start + 1].z - PlayerStartRecordInitPc[0].position->z);
            PlayerStartRecordInitPc[1].position = &spline->pts[start + 2];
            PlayerStartRecordInitPc[1].angle =
                NuAtan2D(spline->pts[start + 3].x - PlayerStartRecordInitPc[0].position->x,
                         spline->pts[start + 3].z - PlayerStartRecordInitPc[1].position->z);
        }
    }
    if (HUB_ADATA != NULL && world->area == HUB_ADATA && last_area != world->area_index &&
        RenderHubFromCutscenePlayerPc == 0) {
        void *door = NULL;
        if (hub_from_superstory != -1) {
            i32 area = Episode_FindAreaFromFlags(&EDataList[hub_from_superstory], 5, 5);
            door = Door_FindByIndex(world, area, -1, NULL);
        } else if (hub_from_mission != -1) {
            door = Door_FindByName(world, PlayerMissionStartDoorPc);
        } else if (hub_from_arcade != -1) {
            door = Door_FindByName(world, PlayerArcadeStartDoorPc);
        } else if (hub_startoutsidebonusdoor_area != -1) {
            door = Door_FindByIndex(world, hub_startoutsidebonusdoor_area, -1, NULL);
        } else if (last_area != -1) {
            i32 area = last_area;
            AREA_DATA_PC *area_data = &ADataList[area];
            if ((area_data->flags & 4) != 0 && area_data->episode_index != -1) {
                area = Episode_FindAreaFromFlags(&EDataList[area_data->episode_index], 5, 5);
                if (area == -1)
                    area = last_area;
            }
            door = Door_FindByIndex(world, area, -1, NULL);
        }
        if (door != NULL)
            NuStrCpy(DoorExitNamePc, (char *)door);
    }
    StartDoorPositions();
    DoorCutSplinePc = NULL;
    if (DoorExitCameraSplineNamePc[0] != '\0') {
        NUGSPLINE *camera = NuSplineFind(world->current_gscn, DoorExitCameraSplineNamePc);
        DoorCutSplinePc = camera;
        if (camera != NULL) {
            DoorCutCameraPosition0Pc = camera->pts[0];
            DoorCutCameraPosition1Pc = camera->pts[1];
        } else {
            Door_UseCutCam = 0;
        }
    }
    DoorExitCameraSplineNamePc[0] = '\0';
    for (index = 0; index < 8; ++index) {
        PLAYER_POSITION_SOCK_PC position;
        ComplexSockPosition(world->sock_sys, PlayerStartRecordInitPc[index].position, -1, -1, &position.sock);
        PlayerStartRecordInitPc[index].sock_location = position.start.location;
        PlayerStartRecordInitPc[index].sock_ratio = position.start.ratio;
    }
    if (HUB_ADATA != NULL && world->area == HUB_ADATA) {
        hub_from_superstory = -1;
        hub_from_mission = -1;
        hub_from_arcade = -1;
        shop_from_cutsceneplayer = RenderHubFromCutscenePlayerPc;
        RenderHubFromCutscenePlayerPc = 0;
    }
}

STATIC_ASSERT(sizeof(PLAYER_ITEM_TYPE_RESET_PC) == 12);
STATIC_ASSERT(offsetof(PLAYER_ITEM_TYPE_RESET_PC, id) == 0);
STATIC_ASSERT(offsetof(PLAYER_ITEM_TYPE_RESET_PC, bolt_name) == 4);
STATIC_ASSERT(offsetof(PLAYER_ITEM_TYPE_RESET_PC, bolt_id) == 9);
static PLAYER_ITEM_TYPE_RESET_PC *PlayerItemTypePc = 0;
static i32 PlayerItemTypeCountPc = 0;
void __cdecl PlayerItemTypes_Init(PLAYER_ITEM_TYPE_RESET_PC *types) {
    PlayerItemTypePc = NULL;
    PlayerItemTypeCountPc = 0;
    if (types == NULL)
        return;
    PlayerItemTypePc = types;
    while (types->id != -1) {
        ++types;
        ++PlayerItemTypeCountPc;
    }
    if (PlayerItemTypeCountPc == 0)
        PlayerItemTypePc = NULL;
}
void __cdecl PlayerItemTypes_Reset(WORLDINFO *world) {
    i32 index;
    if (PlayerItemTypePc != NULL) {
        for (index = 0; index < PlayerItemTypeCountPc; ++index)
            PlayerItemTypePc[index].bolt_id = BoltType_FindIDByName(PlayerItemTypePc[index].bolt_name, world);
    }
}
PLAYER_ITEM_TYPE_RESET_PC *__cdecl PlayerItemType_Find(i32 id) {
    i32 index;
    if (PlayerItemTypePc == NULL)
        return NULL;
    for (index = 0; index < PlayerItemTypeCountPc; ++index)
        if (PlayerItemTypePc[index].id == id)
            return &PlayerItemTypePc[index];
    return NULL;
}

void __cdecl Move_DEFAULT(GAMEOBJECT *);
extern i32 FULLDEBUGTOGGLE;
extern u32 GAMEPAD_TOGGLELEFT;
extern u32 GAMEPAD_TOGGLERIGHT;
extern f32 TOGGLEREPEATTIME;
extern i32 LEGOHINT_FREEPLAYTOGGLE;
extern i32(__cdecl *Player_ToggleSubCharacterFn)(GAMEOBJECT *, i32, i32);
extern void(__cdecl *Player_ToggledCharacterFn)(GAMEOBJECT *, i32);

void __cdecl Player_ToggleCharacter(GAMEOBJECT *object, i32 direction, i32 sound) {
    f32 hold_time = object->input_toggle_hold_time;
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 old_id, left, right, index, attempts, id, collected, area, allowed;
    u32 left_mask, right_mask;
    FULLDEBUGTOGGLE = 0;
    if ((HUB_ADATA != NULL && world->area == HUB_ADATA) || FadeSys.fade != 0.0f ||
        (CInfo[(i8)object->character_context].flags & 0x100) != 0 || object->takeover_source != NULL || FreePlay == 0)
        return;
    if ((MiniCutCam != 0 && (object->flags_1fc_word & 0x180) == 0x80) || object->dead != 0 ||
        object->pad == ViewCamGetGamePad()) {
        object->input_toggle_hold_time = TOGGLEHOLDTIME;
        return;
    }
    old_id = (i16)object->id;
    if (direction != 0) {
        left = direction < 1;
        right = direction >= 1;
    } else {
        left_mask = GAMEPAD_TOGGLELEFT;
        right_mask = GAMEPAD_TOGGLERIGHT;
        if (LIFTPLAYER != 0) {
            left_mask &= ~(u32)GAMEPAD_LIFT;
            right_mask &= ~(u32)GAMEPAD_LIFT;
        }
        left = object->pad->buttons_pressed & left_mask;
        right = object->pad->buttons_pressed & right_mask;
        if (left != 0 && right != 0) {
            left = 0;
            right = 0;
        }
        if ((left | right) == 0 && (object->flags_1fc_word & 0x80) != 0) {
            left = object->pad->buttons_held & left_mask;
            right = object->pad->buttons_held & right_mask;
            if ((left != 0 && right != 0) || (left | right) == 0) {
                left = 0;
                right = 0;
                object->input_toggle_hold_time = TOGGLEHOLDTIME;
            } else {
                object->input_toggle_hold_time -= FRAMETIME;
                if (!(object->input_toggle_hold_time <= 0.0f)) {
                    left = 0;
                    right = 0;
                } else
                    object->input_toggle_hold_time = TOGGLEREPEATTIME;
            }
        }
        hold_time = object->input_toggle_hold_time;
        if ((left | right) == 0)
            return;
    }
    if (Player_ToggleSubCharacterFn != NULL && Player_ToggleSubCharacterFn(object, left, sound) != 0)
        return;
    for (index = 0; index < apicharsys->loaded_model_count; ++index)
        if ((u16)apicharsys->models[index].model_id == object->id)
            break;
    if (index == apicharsys->loaded_model_count)
        return;
    attempts = 0;
    allowed = 0;
    do {
        if (left != 0) {
            if (--index == -1)
                index = apicharsys->loaded_model_count - 1;
        } else if (++index == apicharsys->loaded_model_count)
            index = 0;
        id = apicharsys->models[index].model_id;
        ++attempts;
        collected = InCollectList_Index(id, NULL, 0);
        if ((apicharsys->models[index].flags & 1) == 0)
            goto next_character;
        if (collected == -1 && (i32)GCDataList[id].flags_090 >= 0 && (GCDataList[id].flags_094 & 0x1000000) == 0 &&
            !(VehicleArea != 0 && BonusArea != 0 && Cheats_CheckFlags(0x100) != 0 &&
              (GCDataList[id].flags_094 & 0x2000000) != 0))
            goto next_character;
        if (VehicleArea != 0) {
            if ((CDataList[id].model_flags & 0x2000) == 0 &&
                !(BonusArea != 0 && (CDataList[id].model_flags & 0x4000000) != 0) &&
                (i32)GCDataList[id].flags_090 >= 0 &&
                !(BonusArea != 0 && Cheats_CheckFlags(0x100) != 0 && (GCDataList[id].flags_094 & 0x2000000) != 0))
                goto next_character;
            if (BonusArea != 0) {
                area = AreaFromMiniKitID(id);
                if (area != -1) {
                    if (Game_AreaSave == NULL || Game_AreaSave[area].minikit_complete == 0)
                        goto next_character;
                } else if ((i32)GCDataList[id].flags_090 >= 0 && !(BonusArea != 0 && Cheats_CheckFlags(0x100) != 0 &&
                                                                   (GCDataList[id].flags_094 & 0x2000000) != 0))
                    goto next_character;
                if (object->field_283 >= 0 && object->field_283 < 17 && (TerLayer[object->field_283].flags & 1) != 0 &&
                    GCDataList[id].field_028 <= 0.0f)
                    goto next_character;
            }
        } else if ((CDataList[id].model_flags & 0x2000) != 0 || (GCDataList[id].flags_090 & 0x40) != 0)
            goto next_character;
        if (BonusArea == 0 || VehicleArea == 0) {
            if ((i32)GCDataList[id].flags_090 < 0) {
                if ((object->flags_1fc_word & 0x80) == 0 || Cheats_CheckFlags(0x100) == 0)
                    goto next_character;
            } else {
                if ((object->flags_1fc_word & 0x80) == 0 && collected == -1 &&
                    (GCDataList[id].flags_094 & 0x1000000) == 0)
                    goto next_character;
                if ((GCDataList[id].flags_094 & 0x1000000) == 0 && (collected == -1 || Collection_Got(id) == 0))
                    goto next_character;
            }
        }
        if (object->floor_height != 2000000.0f && object->roof_height_pc != 2000000.0f &&
            object->character_data->move_fn != Move_DEFAULT &&
            CDataList[id].top - CDataList[id].bottom >= (f32)(object->roof_height_pc - object->floor_height)) {
            GameAudio_PlaySfx(0x32, &object->collision_position, 0, 0);
            goto next_character;
        }
        allowed = 1;
    next_character:
        if (attempts > apicharsys->loaded_model_count || allowed != 0 || id == (i16)object->id)
            break;
    } while (1);
    if (attempts >= apicharsys->loaded_model_count || allowed == 0)
        return;
    if ((object->flags_1fc_word & 0x80) != 0) {
        Hint_SetComplete(LEGOHINT_FREEPLAYTOGGLE);
        if (VehicleArea == 0)
            GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
    }
    NewPlayerCharacter(object, id, old_id, 1);
    RememberPlayerIDs(0, Player[0] != NULL ? (i16)((GAMEOBJECT *)Player[0])->id : -1,
                      Player[1] != NULL ? (i16)((GAMEOBJECT *)Player[1])->id : -1);
    if (Player_ToggledCharacterFn != NULL)
        Player_ToggledCharacterFn(object, left);
    object->input_toggle_hold_time = hold_time;
    if (sound != 0) {
        if (left != 0)
            GameAudio_PlaySfx(0x23, &object->collision_position, 0, 0);
        else if (right != 0)
            GameAudio_PlaySfx(0x24, &object->collision_position, 0, 0);
    }
}

STATIC_ASSERT(offsetof(GAMEOBJECT, powerup_time) == 0xdec);
i32 __cdecl Player_HasDeflectBolts(GAMEOBJECT *object) {
    if (Cheats_CheckFlags(0x80000) != 0)
        return 1;
    return object != NULL && object->powerup_time > 0.0f;
}
i32 __cdecl Player_HasDoubleBoltDamage_FromBolt(BOLT_PC *bolt) {
    GAMEOBJECT *object;
    i32 player;
    if (bolt->flags & 1)
        player = 0;
    else if (bolt->flags & 2)
        player = 1;
    else
        return 0;
    object = Player[player];
    if (Cheats_CheckFlags(2) != 0)
        return 1;
    return object != NULL && object->powerup_time > 0.0f;
}

void __cdecl AveragePlayerCurrentSpeedMul(void) {
    i32 count = 0;
    avg_currentspeed_mul = 0.0f;
    if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc_word & 0x80) != 0) {
        avg_currentspeed_mul = (f32)((f64)((GAMEOBJECT *)Player[0])->current_speed_mul + 0.0);
        count = 1;
    }
    if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc_word & 0x80) != 0) {
        avg_currentspeed_mul = (f32)((f64)((GAMEOBJECT *)Player[1])->current_speed_mul + (f64)avg_currentspeed_mul);
        ++count;
    }
    if (count > 1)
        avg_currentspeed_mul = (f32)((1.0 / (f64)count) * (f64)avg_currentspeed_mul);
}
void __cdecl SetPlayer(void) {
    if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc_word & 0x80) != 0) {
        player = Player[0];
        if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc_word & 0x80) != 0) {
            player2 = Player[1];
            return;
        }
    } else if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc_word & 0x80) != 0) {
        player = Player[1];
    } else {
        player = NULL;
        return;
    }
    player2 = NULL;
}

u8 __cdecl FindNearestPlayerToVec(NUVEC *position, GAMEOBJECT **nearest_player, f32 *distance_squared,
                                  u8 require_character_flags, u32 character_flags) {
    i32 index;
    *nearest_player = NULL;
    *distance_squared = 0.0f;
    for (index = 0; index < 8; ++index) {
        GAMEOBJECT *candidate = (GAMEOBJECT *)Player[index];
        if (candidate != NULL && (candidate->flags_1fc & 0x80) != 0) {
            f32 candidate_distance = NuVecDistSqr(&candidate->position, position, NULL);
            if (candidate_distance < *distance_squared || *nearest_player == NULL) {
                if (require_character_flags == 0 ||
                    (((GAMEOBJECT *)Player[index])->character_data->game_character->flags_090 & character_flags) != 0) {
                    *distance_squared = candidate_distance;
                    *nearest_player = (GAMEOBJECT *)Player[index];
                }
            }
        }
    }
    if (*nearest_player != NULL)
        return 1;
    return 0;
}

i32 __cdecl Players_AveragePos(NUVEC *position, SOCKPOSITION_PC *socket_position) {
    NUVEC total = {0.0f, 0.0f, 0.0f};
    f32 count = 0.0f;
    i32 i;
    for (i = 0; i < 2; ++i) {
        GAMEOBJECT *player = (GAMEOBJECT *)Player[i];
        if (player != NULL && (player->flags_1fc_word & 0x80)) {
            NuVecAdd(&total, &total, &player->position);
            count += 1.0f;
        }
    }
    if (count > 0.0f) {
        f32 scale = 1.0f / count;
        NuVecScale(position, &total, scale);
        if (socket_position != NULL)
            ComplexSockPosition(WORLD->sock_sys, position, -1, -1, socket_position);
        return 1;
    }
    if (socket_position != NULL)
        socket_position->sock_index = -1;
    return 0;
}

/* Native ResetPlayer shares the light packet at 4d8 with LightGameObject. */
STATIC_ASSERT(offsetof(GAMEOBJECT, field_dbc) == 0xdbc);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_de0) == 0xde0);
STATIC_ASSERT(offsetof(PLAYER_POSITION_RECORD_PC, angle) == 0xc);
extern i32 movegamecamera_forcesock;

void __cdecl ResetPlayer(GAMEOBJECT *object, i32 reset_moves, NUVEC *position, i32 snap_to_surface) {
    f32 surface_y = 2000000.0f;
    i32 restore_hitpoints;
    i32 player_index;
    u16 facing;
    SOCK_PC *sock;
    PLAYER_POSITION_RECORD_PC *starts = (PLAYER_POSITION_RECORD_PC *)PlayerStartRecordInitPc;
    if (object == NULL)
        return;
    restore_hitpoints = object->field_280 != 0xff && object->character_context == 0x2b;
    if (reset_moves != 0) {
        if (position == NULL)
            position = Player_StartPos(object);
        if (position != NULL)
            object->position = *position;
        if (object->field_280 != 0xff)
            object->flags_ef4 |= 0x40;
        ResetPlayerMoves(object);
        object->collision_position = object->position;
        object->initial_position = object->position;
        plr_lastpos = object->collision_position;
        object->start_position = object->collision_position;
        object->reset_velocity = v000;
        object->velocity = object->reset_velocity;
        GetTopBot(object);
        GameObjectDimensions(object);
        object->flags_e20 &= ~0x08000000u;
        object->use_model_origin = 0;
        object->origin_joint_ready = 0;
        GameObjectOrigin(object);
        ResetRumble(&object->pad->rumble);
        ResetLights(&object->position, &object->lighting_packet, WORLD->rtl_set);
        ResetMiniAnimPacket((MINIANIMPACKET_PC *)object->mini_anim_packet, -1);
        object->sock_position.sock_index = -1;
        object->sock_position.segment = -1;
        if (WORLD->sock_sys != NULL) {
            if (Door_NextSock != -1 && WORLD->sock_sys->socks[Door_NextSock].valid != 0) {
                complexsockposition_forcesock = Door_NextSock;
                movegamecamera_forcesock = Door_NextSock;
            }
            ComplexSockPosition(WORLD->sock_sys, &object->position, object->sock_position.sock_index,
                                object->sock_position.segment, &object->sock_position);
            ComplexSockAngles(&object->sock_angles);
            if ((i8)object->field_280 < 2 && object->sock_position.sock_index != -1) {
                if (BONUS_GUNSHIP_ADATA != NULL && WORLD->area == BONUS_GUNSHIP_ADATA) {
                    object->position = object->sock_position.midpoint;
                    surface_y = FindGunshipHoverHeight(object);
                    snap_to_surface = 0;
                    if (WORLD->current_level == BONUS_GUNSHIPA_LDATA && (LevFlag[0] & 0xff) != 0)
                        object->position.x = (f32)((f64)object->position.x + 12.0);
                } else if (DOGFIGHT_ADATA != NULL && WORLD->area == DOGFIGHT_ADATA) {
                    sock = &WORLD->sock_sys->socks[object->sock_position.sock_index];
                    if ((sock->flags & 1) == 0) {
                        object->position = object->sock_position.midpoint;
                        if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc & 0x80) != 0 &&
                            Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc & 0x80) != 0) {
                            NUVEC offset;
                            f32 side = object == Player[0] ? -1.0f : 1.0f;
                            NuVecNorm(&offset, &sock->segments[object->sock_position.segment].planes[4]);
                            NuVecScale(&offset, &offset, side);
                            NuVecAdd(&object->position, &object->position, &offset);
                        }
                        object->collision_position = object->position;
                        object->initial_position = object->position;
                        plr_lastpos = object->collision_position;
                        object->start_position = object->collision_position;
                    }
                    snap_to_surface = 0;
                }
            }
        }
        player_index = (i8)object->field_280;
        if (player_index < 0 || player_index >= 8)
            player_index = object->hostility_group;
        facing = (u16)starts[player_index].angle;
        object->pitch_angle = 0;
        object->movement_facing_angle = facing;
        object->facing_angle = facing;
        object->field_0x276 = facing;
        object->roll_angle = 0;
        NuVecRotateYValZ(&object->facing_direction, 1.0f, facing);
        object->current_speed_mul = 1.0f;
        object->current_speed_bias = 0.0f;
        CurrentStart(object, 0, 1);
        object->flags_e20 &= ~0x08000000u;
        object->flags_ef0 &= ~0x02040000u;
        object->model_draw_result = 1;
        object->use_model_origin = 0;
        object->origin_joint_ready = 0;
        object->contact_count = 0;
        InitSurfaceInfo(object);
        if ((f64)surface_y == 2000000.0)
            surface_y = GetHoverPosY(object);
        if (snap_to_surface != 0 && (object->character_data->model_flags & 0x2000) == 0) {
            SetObjOnSurface(object, 0);
        } else if ((f64)surface_y != 2000000.0) {
            object->position.y = surface_y;
            object->collision_position = object->position;
            object->initial_position = object->position;
            plr_lastpos = object->collision_position;
            object->start_position = object->collision_position;
        }
        GizForce_ResetLOS(object);
        PortalGameObject(object, 1, 1, -1, WORLD->current_gscn);
        if ((ResetBits & 2) != 0 || object->dead != 0 || restore_hitpoints != 0) {
            object->current_hp = (i8)DEFAULT_PLAYERHITPOINTS;
            if (object->current_hp > object->field_107e)
                object->current_hp = (i8)object->field_107e;
            object->vehicle_secondary_hp = 4;
        }
        if ((ResetBits & 8) != 0) {
            player_index = (i8)object->field_280;
            if (PlayerProgress[player_index].field_b != 0)
                object->jump_variant_state = 1;
            object->vehicle_secondary_hp = PlayerProgress[player_index].field_9;
        }
        ResetPlayerAI(object);
        object->last_safe_position = object->position;
        object->ai_update_position = object->position;
        if ((object->character_data->model_flags & 0x2000) != 0) {
            object->in_narrow_socket = ObjInNarrowSock(object, WORLD->sock_sys, WORLD->level_idx);
            object->field_dc8 = GetVehicleSpeedMul(object, 0.0f);
        } else {
            object->field_dc8 = 0.0f;
        }
        if (object->field_dc8 > 0.0f) {
            object->velocity.x = NuTrigTable[object->facing_angle >> 1] * object->field_dc8;
            object->velocity.z = NuTrigTable[((object->facing_angle + 0x4000) >> 1) & 0x7fff] * object->field_dc8;
        }
    } else {
        object->field_dc8 = 0.0f;
    }
    object->flags_1fc_word &= 0xfffdfa83u;
    object->collision_scale = 1.0f;
    object->flags_ef0 |= 0x80;
    object->field_1010 = 0.0f;
    object->field_da8 = 0.0f;
    object->flags_1f8 &= ~0x100u;
    object->flags_e20 |= 0x8000;
    object->directional_seek_multiplier = 1.0f;
    object->dead = 0;
    object->wall_run_state = 0;
    object->previous_floor_height = 2000000.0f;
    object->effect_flags = 0;
    object->field_dbc = 0.0f;
    object->field_f10 = 0.0f;
    object->camera_screen_position.z = -1.0f;
    object->field_de0 = -1.0f;
}

/* Private source fragment for src/legoapi/players.c; not a standalone TU.
 * Retail private ABI: Copy src=EAX,dst=ECX; other helpers matrix=EAX,angle=ECX.
 * Same-TU compiler convention must be checked with all retail callers.
 */
static void GameDrawMtxCopyPc(NUMTX *destination, const NUMTX *source) {
    destination->m00 = source->m00;
    destination->m01 = source->m01;
    destination->m02 = source->m02;
    destination->m03 = source->m03;
    destination->m10 = source->m10;
    destination->m11 = source->m11;
    destination->m12 = source->m12;
    destination->m13 = source->m13;
    destination->m20 = source->m20;
    destination->m21 = source->m21;
    destination->m22 = source->m22;
    destination->m23 = source->m23;
    destination->m30 = source->m30;
    destination->m31 = source->m31;
    destination->m32 = source->m32;
    destination->m33 = source->m33;
}

static void GameDrawMtxSetYRotationPc(NUMTX *matrix, NUANG angle) {
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    matrix->m00 = cosine;
    matrix->m01 = 0.0f;
    matrix->m02 = -sine;
    matrix->m03 = 0.0f;
    matrix->m10 = ANI3_EVAL_IDENTITY_MATRIX.m10;
    matrix->m11 = ANI3_EVAL_IDENTITY_MATRIX.m11;
    matrix->m12 = ANI3_EVAL_IDENTITY_MATRIX.m12;
    matrix->m13 = ANI3_EVAL_IDENTITY_MATRIX.m13;
    matrix->m20 = sine;
    matrix->m21 = 0.0f;
    matrix->m23 = 0.0f;
    matrix->m22 = cosine;
    matrix->m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
    matrix->m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
    matrix->m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
    matrix->m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
}

static void GameDrawMtxRotateXPc(NUMTX *matrix, NUANG angle) {
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m01 = matrix->m01;
    f32 m11 = matrix->m11;
    f32 m21 = matrix->m21;
    f32 m31 = matrix->m31;

    matrix->m01 = (m01 * cosine) - (matrix->m02 * sine);
    matrix->m02 = (m01 * sine) + (matrix->m02 * cosine);
    matrix->m11 = (m11 * cosine) - (matrix->m12 * sine);
    matrix->m12 = (m11 * sine) + (matrix->m12 * cosine);
    matrix->m21 = (m21 * cosine) - (matrix->m22 * sine);
    matrix->m22 = (m21 * sine) + (matrix->m22 * cosine);
    matrix->m31 = (m31 * cosine) - (matrix->m32 * sine);
    matrix->m32 = (m31 * sine) + (matrix->m32 * cosine);
}

static void GameDrawMtxRotateYPc(NUMTX *matrix, NUANG angle) {
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m00 = matrix->m00;
    f32 m10 = matrix->m10;
    f32 m20 = matrix->m20;
    f32 m30 = matrix->m30;

    matrix->m00 = m00 * cosine + matrix->m02 * sine;
    matrix->m02 = matrix->m02 * cosine - m00 * sine;
    matrix->m10 = m10 * cosine + matrix->m12 * sine;
    matrix->m12 = matrix->m12 * cosine - m10 * sine;
    matrix->m20 = m20 * cosine + matrix->m22 * sine;
    matrix->m22 = matrix->m22 * cosine - m20 * sine;
    matrix->m30 = m30 * cosine + matrix->m32 * sine;
    matrix->m32 = matrix->m32 * cosine - m30 * sine;
}

static void GameDrawMtxRotateZPc(NUMTX *matrix, NUANG angle) {
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m00 = matrix->m00;
    f32 m10 = matrix->m10;
    f32 m20 = matrix->m20;
    f32 m30 = matrix->m30;

    matrix->m00 = (m00 * cosine) - (matrix->m01 * sine);
    matrix->m01 = (m00 * sine) + (matrix->m01 * cosine);
    matrix->m10 = (m10 * cosine) - (matrix->m11 * sine);
    matrix->m11 = (m10 * sine) + (matrix->m11 * cosine);
    matrix->m20 = (m20 * cosine) - (matrix->m21 * sine);
    matrix->m21 = (m20 * sine) + (matrix->m21 * cosine);
    matrix->m30 = (m30 * cosine) - (matrix->m31 * sine);
    matrix->m31 = (m30 * sine) + (matrix->m31 * cosine);
}

#include <string.h>

typedef union DRAW_ATTACHMENTS_PACKET_PC {
    ANIMPACKET_PC packet;
    u32 words[18];
} DRAW_ATTACHMENTS_PACKET_PC;
STATIC_ASSERT(sizeof(DRAW_ATTACHMENTS_MINI_PC) == 0x24);
STATIC_ASSERT(offsetof(DRAW_ATTACHMENTS_MINI_PC, flags) == 0x18);
STATIC_ASSERT(offsetof(DRAW_ATTACHMENTS_MINI_PC, source_animation) == 0x1a);
STATIC_ASSERT(offsetof(DRAW_ATTACHMENTS_MINI_PC, requested_animation) == 0x22);
STATIC_ASSERT(sizeof(DRAW_ATTACHMENTS_PACKET_PC) == 0x48);
STATIC_ASSERT(offsetof(GAMEOBJECT, anim_packet) == 8);
STATIC_ASSERT(offsetof(GAMEOBJECT, suit) == 0xcbc);
STATIC_ASSERT(offsetof(GAMEOBJECT, id) == 0x1064);
STATIC_ASSERT(offsetof(GAMEOBJECT, transform) == 0xb8);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_height) == 0x1014);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_enabled) == 0x107b);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_107c) == 0x107c);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, blending) == 0x31);
STATIC_ASSERT(offsetof(ANIMPACKET_PC, blend_target_animation) == 0x34);
STATIC_ASSERT(sizeof(CHARACTER_MODEL_LAYERS_PC) == 0x54);
STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, model_data_b) == 0xc);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, extra_character_locator) == 0x11d);
i32 __cdecl GameDrawCharacterModel(CHARACTER_MODEL_LAYERS_PC *, ANIMPACKET_PC *, NUMTX *, NUMTX *, NUMTX *, NUMTX *,
                                   GAMEOBJECT *, u32);

void __cdecl AnimPacketMiniToFullPc(DRAW_ATTACHMENTS_MINI_PC *source, ANIMPACKET_PC *destination) {
    destination->current_time = source->current_time;
    destination->previous_time = source->previous_time;
    destination->blend_elapsed_pc = source->blend_elapsed;
    destination->blend_duration_pc = source->blend_duration;
    destination->blend_source_time = source->source_time;
    destination->blend_target_time = source->target_time;
    destination->flags = source->flags;
    destination->blending = source->blending;
    destination->blend_source_animation = source->source_animation;
    destination->blend_target_animation = source->target_animation;
    destination->animation_index = source->animation;
    destination->previous_animation = source->previous_animation;
    destination->requested_animation = source->requested_animation;
    destination->reversed_3c = 0;
    destination->reversed_3d = 0;
    destination->reversed_3e = 0;
    destination->overlay_animation = -1;
}

i32 __cdecl AnimPacketModelsCompatiblePc(ANIMPACKET_PC *packet, i32 source_id, i32 target_id) {
    CHARACTER_MODEL_LAYERS_PC *source, *target;
    i32 source_model, target_model;
    i32 source_animation, target_animation;
    if (source_id == -1)
        return 0;
    source_model = apicharsys->playermodelids[source_id];
    if (source_model == -1)
        return 0;
    source = &apicharsys->models[source_model];
    if (target_id == -1)
        return 0;
    target_model = apicharsys->playermodelids[target_id];
    if (target_model == -1)
        return 0;
    target = &apicharsys->models[target_model];
    if (packet->blending != 0) {
        source_animation = packet->blend_source_animation;
        if (source->model_data_b[source_animation] == NULL)
            return 0;
        target_animation = packet->blend_target_animation;
        if (source->model_data_b[target_animation] == NULL || target->model_data_b[source_animation] == NULL ||
            target->model_data_b[target_animation] == NULL)
            return 0;
    } else {
        source_animation = packet->animation_index;
        if (source->model_data_b[source_animation] == NULL || target->model_data_b[source_animation] == NULL)
            return 0;
    }
    return 1;
}

static void __cdecl DrawCharacterAttachmentModelPc(GAMEOBJECT *object, i32 character_id, NUMTX *joint_matrices,
                                                   ANIMPACKET_PC *animation, i32 locator, NUMTX *override_matrix) {
    i16 model_index;
    CHARACTER_MODEL_LAYERS_PC *model;
    NUMTX matrix, reflected;
    NUMTX *reflection = NULL;
    if (override_matrix == NULL && (locator == -1 || object->character_model->points_of_interest[locator] == NULL))
        return;
    model_index = apicharsys->playermodelids[character_id];
    if (model_index == -1)
        return;
    model = &apicharsys->models[model_index];
    if (override_matrix != NULL)
        matrix = *override_matrix;
    else
        matrix = joint_matrices[locator];
    if ((u8)object->field_107c != 0 && (f64)object->reflection_height != 2000000.0 &&
        MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                         WORLD->current_level->reflection_y, &reflected) != 0)
        reflection = &reflected;
    GameDrawCharacterModel(model, animation, &matrix, NULL, reflection, NULL, NULL, 0xffffffffu);
}

void __cdecl DrawCharacterAttachments(GAMEOBJECT *object, NUMTX *joint_matrices) {
    i32 character_id;
    i32 current_animation;
    DRAW_ATTACHMENTS_PACKET_PC animation;
    NUMTX *override_matrix = NULL;
    if (object->suit != NULL && (object->suit->flags & 2) != 0) {
        if (object->id != (u16)ID_DEFAULTCHARACTER_A)
            return;
        character_id = AreaExtraTargetId7f17bcPc;
        AnimPacketMiniToFullPc((DRAW_ATTACHMENTS_MINI_PC *)object->mini_anim_packet, &animation.packet);
    } else if (object->id == (u16)PunchRangeUnitIdPc && (AnimPlaying(&object->anim_packet, 0x51, 1, 1) != NULL ||
                                                         AnimPlaying(&object->anim_packet, 0x55, 1, 1) != NULL ||
                                                         AnimPlaying(&object->anim_packet, 0x56, 1, 1) != NULL ||
                                                         AnimPlaying(&object->anim_packet, 0x94, 1, 1) != NULL)) {
        character_id = PunchRangeCompanionIdPc;
        memcpy(&animation, (u8 *)object + offsetof(GAMEOBJECT, anim_packet), sizeof(animation));
        if (animation.packet.blending != 0 && animation.packet.blend_target_animation != 0x51 &&
            animation.packet.blend_target_animation != 0x55 && animation.packet.blend_target_animation != 0x56 &&
            animation.packet.blend_target_animation != 0x94)
            return;
    } else if (object->id == (u16)AreaExtraSourceId7f17c8Pc) {
        character_id = AreaExtraTargetId7f17ccPc;
        AnimPacketMiniToFullPc((DRAW_ATTACHMENTS_MINI_PC *)object->mini_anim_packet, &animation.packet);
    } else if (object->id == (u16)VehicleBoltId7f17d0Pc || object->id == (u16)VehicleBoltId7f17d4Pc) {
        if (AnimPacketModelsCompatiblePc(&object->anim_packet, (i16)object->id, PunchRangeCompanionIdPc) == 0)
            return;
        character_id = PunchRangeCompanionIdPc;
        memcpy(&animation, (u8 *)object + offsetof(GAMEOBJECT, anim_packet), sizeof(animation));
        override_matrix = &object->transform;
    } else
        return;
    if (character_id == -1)
        return;
    current_animation = CurrentAnim(&object->anim_packet);
    if (object->character_model->animations[current_animation] != NULL &&
        (object->character_model->animations[current_animation]->misc_flags & 8) != 0)
        return;
    DrawCharacterAttachmentModelPc(object, character_id, joint_matrices, &animation.packet,
                                   object->character_data->game_character->extra_character_locator, override_matrix);
}

/* Private players.c helper. Retail 0x419130, 158 bytes through RET at 0x4191cd.
 * Retail private ABI: object=EAX; f32 result=ST0.
 * The two calculated return paths require a final f32 store/reload.
 */
extern const f32 CharacterWeaponScaleUpperBoundPc;
static f32 ParaphernaliaWeaponScalePc(GAMEOBJECT *object) {
    f32 *frame;
    if (object->character_data->game_character->hover_fall_mode == 9) {
        frame = AnimPlaying(&object->anim_packet, 0x5e, 1, 0);
        if (frame != NULL && *frame >= 18.0f && *frame <= CharacterWeaponScaleUpperBoundPc) {
            if (*frame <= 27.0) {
                return (f32)(((f64)*frame - 18.0) / 9.0);
            }
            if (55.0 > *frame)
                return 1.0f;
            return (f32)(1.0 - ((f64)*frame - 55.0) / 10.0);
        }
    }
    return 0.0f;
}

f32 __cdecl DropInOutScale(GAMEOBJECT *object);

/* Private native PC 41cb50/2038 draft; integrate only after source review. */
typedef struct CHARACTER_DRAW_HIERARCHY_PC {
    u8 unknown_000[0x1a0];
    u8 suppress_shadow_surface_points;
} CHARACTER_DRAW_HIERARCHY_PC;
STATIC_ASSERT(offsetof(CHARACTER_DRAW_HIERARCHY_PC, suppress_shadow_surface_points) == 0x1a0);
STATIC_ASSERT(offsetof(GAMEOBJECT, joint_overrides) == 0xf3c);
STATIC_ASSERT(offsetof(GAMEOBJECT, head_target) == 0xfd8);
STATIC_ASSERT(offsetof(GAMEOBJECT, joint_override_count) == 0x107d);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_link) == 0x298);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_exclusion_mask) == 0x2a0);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1083) == 0x1083);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_108c) == 0x108c);
STATIC_ASSERT(offsetof(GAMEOBJECT, gizforce_target) == 0x10cc);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_direction) == 0x200);
STATIC_ASSERT(offsetof(GAMEOBJECT, view_distance) == 0x230);
STATIC_ASSERT(offsetof(GAMEOBJECT, min_view_height) == 0x23c);
STATIC_ASSERT(offsetof(GAMEOBJECT, mini_anim_packet) == 0x7d0);
STATIC_ASSERT(offsetof(GAMEOBJECT, use_action_word) == 0xf00);
extern i32 GameKeepMergeActionPc;
extern i32 CharacterBlobShadowsOnPc;
extern i32 GameDrawCharacterModelFlagPc;

i32 __cdecl GameDrawCharacterModel(CHARACTER_MODEL_LAYERS_PC *model, ANIMPACKET_PC *animation, NUMTX *matrix,
                                   NUMTX *secondary_matrix, NUMTX *reflection_matrix, NUMTX *auxiliary_matrix,
                                   GAMEOBJECT *object, u32 flags) {
    NUMTX special_matrix;
    NUMTX scaled_matrix;
    NUMTX scaled_reflection;
    NUMTX output_matrices[256];
    i32 drag_bomb;
    i32 complex_shadow;
    i32 result;
    i32 draw_shadow;
    i32 i;
    drag_bomb = 0;
    if (object != NULL && object->id == id_DRAGBOMB && Cheat_IsOn(5) && WORLD->level_objects[0x135].active != 0) {
        NUVEC half_scale;
        ResetShadowMapRendering();
        special_matrix = *matrix;
        drag_bomb = 1;
        NuSpecialDrawAt(&WORLD->level_objects[0x135].special, &special_matrix);
        if (reflection_matrix != NULL) {
            special_matrix = *reflection_matrix;
            NuRndrStartReflectionRender(1);
            NuSpecialDrawAt(&WORLD->level_objects[0x135].special, &special_matrix);
            NuRndrEndReflectionRender();
        }
        half_scale.x = half_scale.y = half_scale.z = 0.5f;
        scaled_matrix = *matrix;
        matrix = &scaled_matrix;
        NuMtxPreScale(matrix, &half_scale);
        if (reflection_matrix != NULL) {
            scaled_reflection = *reflection_matrix;
            reflection_matrix = &scaled_reflection;
            NuMtxPreScale(reflection_matrix, &half_scale);
        }
    }
    complex_shadow = 0;
    if (object != NULL && (object->character_data->model_flags & 0x10000) != 0 &&
        (WORLD->area == NULL || WORLD->area != BATTLEOVERCORUSCANT_ADATA)) {
        ((CHARACTER_DRAW_HIERARCHY_PC *)model->hierarchy)->suppress_shadow_surface_points = 0;
        if (reflection_matrix != NULL) {
            ConfigureComplexShadow(object);
            complex_shadow = 1;
        }
    } else {
        ((CHARACTER_DRAW_HIERARCHY_PC *)model->hierarchy)->suppress_shadow_surface_points = 1;
    }
    drawcharactermodel_keepmergeaction = GameKeepMergeActionPc;
    ModelAnimLayerListDispatchPc = GCDataList[model->model_id].make_layer_list;
    result =
        APIDrawCharacterModel(model, object != NULL ? (APICHARACTERDATA *)object->character_data : NULL, animation,
                              matrix, secondary_matrix, reflection_matrix, NULL, auxiliary_matrix, object, flags,
                              object != NULL && GameDrawCharacterModelFlagPc != 0 ? object->joint_overrides : NULL,
                              object != NULL && GameDrawCharacterModelFlagPc != 0 ? object->joint_override_count : 0,
                              Paused, FRAMETIME, output_matrices, NULL, WORLD->game_debris_sys);
    if (result == 0) {
        if (object != NULL && auxiliary_matrix != NULL) {
            for (i = 0; i < 16; ++i) {
                if (model->points_of_interest[i] != NULL) {
                    auxiliary_matrix[i] = object->transform;
                    memcpy(&auxiliary_matrix[i].m30, &object->collision_position, sizeof(NUVEC));
                    auxiliary_matrix[i].m31 = (f32)((f64)auxiliary_matrix[i].m31 -
                                                    ((f64)object->character_top + (f64)object->character_bottom) *
                                                        (f64)object->origin_scale * 0.5);
                }
            }
        }
    }
    draw_shadow = result != 0 && (object == NULL || ((object->flags_ef0 & 0x4000000) == 0 &&
                                                     (object->character_data->model_flags & 0x10000) == 0));
    if (PcWindowOptions.shadowmethod == 1 && draw_shadow && secondary_matrix != NULL && object != NULL) {
        u8 level_alpha = WORLD->current_level->blob_shadow_alpha;
        if (level_alpha > 0 && object->shadow_opacity > 0.0f && object->surface_normal.y > 0.0f &&
            (object->water_height == -1.0f || (TerLayer[object->field_283].flags & 1) == 0)) {
            if ((i16)object->id == id_MOUSEDROID && WORLD->level_objects[0xa2].active != 0) {
                f32 opacity;
                special_matrix = *secondary_matrix;
                opacity = object->shadow_opacity * object->surface_normal.y;
                NuSpecialDrawAtAlpha(&WORLD->level_objects[0xa2].special, &special_matrix, opacity);
            } else if (CharacterBlobShadowsOnPc != 0) {
                PLAYERCHARACTERCONFIG_PC *config = object->character_data->game_character;
                i32 alpha = config->blob_shadow_alpha < 255 ? config->blob_shadow_alpha : level_alpha;
                f32 radius;
                if (alpha > 0) {
                    radius = config->field_08c < 99.0f ? config->field_08c : object->character_data->collision_radius;
                    if (radius > 0.0f) {
                        radius = DropInOutScale(object) * radius;
                        NuRndrAddShadow(&secondary_matrix->translation, object->origin_scale * radius,
                                        alpha * object->shadow_opacity * object->surface_normal.y, object->field_1052,
                                        0, object->field_1054);
                    }
                }
            }
        }
    }
    if (complex_shadow != 0)
        ConfigureComplexShadow(NULL);
    if (drag_bomb != 0)
        EnableShadowMapRendering(0);
    return result;
}

/* Selective append to players.c after the existing five private matrix helpers. */
typedef struct DRAW_PREPASS_TELEPORT_PC {
    u8 unknown_00[0x4c];
    u8 flags;
} DRAW_PREPASS_TELEPORT_PC;
typedef struct DRAW_PREPASS_TELEPORT_OBJECT_PC {
    u8 unknown_000[0x798];
    DRAW_PREPASS_TELEPORT_PC *teleport;
} DRAW_PREPASS_TELEPORT_OBJECT_PC;
STATIC_ASSERT(sizeof(GAMEOBJECT) == 0x10d8);
STATIC_ASSERT(offsetof(DRAW_PREPASS_TELEPORT_PC, flags) == 0x4c);
STATIC_ASSERT(offsetof(DRAW_PREPASS_TELEPORT_OBJECT_PC, teleport) == 0x798);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1f8) == 0x1f8);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, origin_joint_ready) == 0x28c);
STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
STATIC_ASSERT(offsetof(GAMEOBJECT, death_delay) == 0x100c);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1010) == 0x1010);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_7b3) == 0x7b3);
STATIC_ASSERT(offsetof(GAMEOBJECT, render_offset) == 0xe94);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e20) == 0xe20);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_e24) == 0xe24);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef4) == 0xef4);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef0) == 0xef0);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_mode) == 0x107a);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_enabled) == 0x107b);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_107c) == 0x107c);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_update_distance) == 0x101c);
STATIC_ASSERT(offsetof(GAMEOBJECT, transform) == 0xb8);
STATIC_ASSERT(offsetof(GAMEOBJECT, draw_secondary_mtx) == 0xf8);
STATIC_ASSERT(offsetof(GAMEOBJECT, draw_tertiary_mtx) == 0x138);
STATIC_ASSERT(offsetof(GAMEOBJECT, vehicle_orientation) == 0xc04);
STATIC_ASSERT(offsetof(GAMEOBJECT, pitch_angle) == 0x278);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_0x276) == 0x27a);
STATIC_ASSERT(offsetof(GAMEOBJECT, roll_angle) == 0x27c);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_lean_angle) == 0xe18);
STATIC_ASSERT(offsetof(GAMEOBJECT, secondary_lean_angle) == 0xe1a);
STATIC_ASSERT(offsetof(GAMEOBJECT, tertiary_lean_angle) == 0xe1c);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1052) == 0x1052);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1054) == 0x1054);
STATIC_ASSERT(offsetof(LEVELDATA_PC, reflection_range) == 0xcd);
STATIC_ASSERT(offsetof(LEVELDATA_PC, reflection_y) == 0xc0);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, shadow_locators) == 0xf0);
f32 __cdecl PodSprint_RollMul(GAMEOBJECT *);
void __cdecl ApplyExtraRotation(GAMEOBJECT *, NUMTX *);

i32 __cdecl DrawGameObjectsProcess(void) {
    GAMEOBJECT *object = Obj;
    i32 index;
    for (index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
        u8 draw_pending;
        NUVEC position, scale, displacement;
        f32 drop_scale, distance, offset;
        NUMTX matrix, orientation, reflected, shadow;
        i32 angle;
        object->flags_1f8 |= 0x800;
        if ((object->flags_1fc_word & 0x1001) != 0x1001)
            continue;
        draw_pending = object->origin_joint_ready;
        object->flags_e24 &= ~8u;
        object->origin_joint_ready = 0;
        object->flags_e20 ^= (((u32)draw_pending << 27) ^ object->flags_e20) & 0x8000000u;
        if (object->dead != 0 && (object->field_1010 > 0.0f || object->death_delay == 0.0f))
            continue;
        if ((object->flags_e20 & 0x20) != 0 && object->character_context != 0x23 && object->character_context != 0x24)
            continue;
        if (object->character_context == 0xf && object->field_7b3 == 1 &&
            ((DRAW_PREPASS_TELEPORT_OBJECT_PC *)object)->teleport != NULL &&
            (((DRAW_PREPASS_TELEPORT_OBJECT_PC *)object)->teleport->flags & 4) == 0)
            continue;
        if (object->character_context == 0x3b || object->character_context == 0x39)
            continue;
        position.x = object->position.x;
        position.y = object->position.y;
        if ((object->flags_1f8 & 0x100) != 0)
            position.y = (f32)((f64)object->character_bottom * object->origin_scale + position.y);
        position.z = object->position.z;
        NuVecAdd(&position, &position, &object->render_offset);
        object->flags_ef4 ^= (((u32)object->model_draw_result << 8) ^ object->flags_ef4) & 0x100u;
        object->model_draw_result = 1;
        scale.x = object->origin_scale;
        drop_scale = DropInOutScale(object);
        scale.x = (f32)((f64)drop_scale * scale.x);
        scale.y = scale.z = scale.x;
        NuMtxSetScale(&matrix, &scale);
        switch ((u8)object->movement_mode) {
            case 0:
                GameDrawMtxSetYRotationPc(&orientation, 0x8000);
                if (object->pitch_angle != 0)
                    GameDrawMtxRotateXPc(&orientation, object->pitch_angle);
                if (object->field_0x276 != 0)
                    GameDrawMtxRotateYPc(&orientation, object->field_0x276);
                if (object->roll_angle != 0)
                    GameDrawMtxRotateZPc(&orientation, object->roll_angle);
                NuMtxMulVU0(&matrix, &matrix, &orientation);
                NuMtxTranslate(&matrix, &position);
                break;
            case 1:
                GameDrawMtxSetYRotationPc(&orientation, object->field_0x276 + 0x8000);
                if (object->pitch_angle != 0)
                    GameDrawMtxRotateXPc(&orientation, object->pitch_angle);
                if (object->roll_angle != 0)
                    GameDrawMtxRotateZPc(&orientation, object->roll_angle);
                NuMtxMulVU0(&matrix, &matrix, &orientation);
                NuMtxTranslate(&matrix, &position);
                break;
            case 3:
                GameDrawMtxSetYRotationPc(&orientation, 0x8000);
                if (object->roll_angle != 0)
                    GameDrawMtxRotateZPc(&orientation, object->roll_angle);
                if (object->pitch_angle != 0)
                    GameDrawMtxRotateXPc(&orientation, object->pitch_angle);
                if (object->field_0x276 != 0)
                    GameDrawMtxRotateYPc(&orientation, object->field_0x276);
                NuMtxMulVU0(&matrix, &matrix, &orientation);
                NuMtxTranslate(&matrix, &position);
                break;
            case 4:
                matrix = object->vehicle_orientation;
                NuMtxPreScale(&matrix, &scale);
                NuMtxPreRotateY(&matrix, 0x8000);
                NuMtxTranslate(&matrix, &position);
                break;
            case 5:
                matrix = object->vehicle_orientation;
                NuMtxPreScale(&matrix, &scale);
                NuMtxPreRotateY(&matrix, 0x8000);
                break;
            default:
            case 2:
                GameDrawMtxSetYRotationPc(&orientation, object->field_0x276 + 0x8000);
                if (object->roll_angle != 0)
                    GameDrawMtxRotateZPc(&orientation, object->roll_angle);
                if (object->pitch_angle != 0)
                    GameDrawMtxRotateXPc(&orientation, object->pitch_angle);
                NuMtxMulVU0(&matrix, &matrix, &orientation);
                NuMtxTranslate(&matrix, &position);
                break;
        }
        if (object->movement_lean_angle != 0) {
            angle = (i16)object->movement_lean_angle;
            if (PODSPRINT_ADATA != NULL && WORLD->area == PODSPRINT_ADATA)
                angle = (i32)((f64)PodSprint_RollMul(object) * angle);
            NuMtxPreRotateZ(&matrix, angle);
        }
        if (object->secondary_lean_angle != 0)
            NuMtxPreRotateX(&matrix, (i16)object->secondary_lean_angle);
        if (object->tertiary_lean_angle != 0)
            NuMtxPreRotateY(&matrix, (i16)object->tertiary_lean_angle);
        ApplyExtraRotation(object, &matrix);
        if ((u8)object->movement_mode != 4 &&
            (object->character_context == 0x23 || object->character_context == 0x24)) {
            distance = (f32)((f64)object->character_data->collision_radius * object->character_data->model_scale * 7.5);
            if (object->character_context == 0x23)
                offset = (f32)((1.0 - (f64)drop_scale) * distance);
            else
                offset = (f32)(-((1.0 - (f64)drop_scale) * distance));
            NuVecMtxTransformVU0(&displacement, &v001, &orientation);
            NuVecScale(&displacement, &displacement, offset);
            NuVecAdd(NUMTX_GET_ROW_VEC(&matrix, 3), NUMTX_GET_ROW_VEC(&matrix, 3), &displacement);
        }
        if (object->use_model_origin < 2 &&
            (object->id == GameObjectRotationAtstId || object->id == (u16)id_ATST_LOWRES || object->id == (u16)id_ATAT))
            matrix.m31 = (f32)((f64)object->character_bottom * object->origin_scale + matrix.m31);
        object->transform = matrix;
        if (scale.x == 0.0f && scale.y == 0.0f && scale.z == 0.0f) {
            object->model_draw_result = 0;
            continue;
        }
        object->flags_1f8 ^= 0x800;
        object->flags_ef0 &= ~0x20000u;
        *(u8 *)&object->field_107c = 0;
        if (object->reflection_enabled != 0 && (f64)object->reflection_height != 2000000.0 &&
            (f64)WORLD->current_level->reflection_range > object->ai_update_distance) {
            *(u8 *)&object->field_107c =
                (u8)MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                                     WORLD->current_level->reflection_y, &reflected);
            object->draw_tertiary_mtx = reflected;
        }
        if (object->dead == 0 && (f64)object->floor_height != 2000000.0 && (object->flags_1f8 & 0x40) != 0 &&
            object->character_data->game_character->shadow_locators == 0) {
            GameDrawMtxCopyPc(&shadow, &ANI3_EVAL_IDENTITY_MATRIX);
            GameDrawMtxRotateYPc(&shadow, object->field_0x276 + 0x8000);
            if (object->field_1054 != 0)
                GameDrawMtxRotateZPc(&shadow, object->field_1054);
            if (object->field_1052 != 0)
                GameDrawMtxRotateXPc(&shadow, object->field_1052);
            shadow.m30 = object->position.x;
            object->flags_ef0 |= 0x20000;
            shadow.m31 = (f32)((f64)object->floor_height + (f64)0.005f);
            shadow.m32 = object->position.z;
            object->draw_secondary_mtx = shadow;
        }
    }
    return 0;
}

STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_source) == 0xcd0);
STATIC_ASSERT(offsetof(GAMEOBJECT, force_target) == 0x790);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_0x276) == 0x27a);
STATIC_ASSERT(offsetof(GAMEOBJECT, locator_matrices) == 0x804);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_enabled) == 0x107b);
STATIC_ASSERT(offsetof(GAMEOBJECT, reflection_height) == 0x1014);
STATIC_ASSERT(offsetof(GAMEOBJECT, ai_update_distance) == 0x101c);
STATIC_ASSERT(offsetof(GAMEOBJECT, origin_scale_override) == 0xd30);
STATIC_ASSERT(offsetof(GAMEOBJECT, timer_d34) == 0xd34);
STATIC_ASSERT(offsetof(GAMEOBJECT, communicate_blend) == 0xd38);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_d54) == 0xd54);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_e1e) == 0xe1e);
STATIC_ASSERT(offsetof(GAMEOBJECT, helmet) == 0x1082);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_bottom) == 0xff0);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_top) == 0xff4);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, game_character) == 0x24);
STATIC_ASSERT(offsetof(CHARACTER_MODEL_LAYERS_PC, points_of_interest) == 0x14);
STATIC_ASSERT(offsetof(LEVELDATA_PC, reflection_y) == 0xc0);
STATIC_ASSERT(offsetof(LEVELDATA_PC, reflection_range) == 0xcd);

extern i32 DrawParaphernaliaEnabledPc;
extern i16 CharacterFixupIdPc007f1820;
void __cdecl DrawWeapons(GAMEOBJECT *, i32, f32);
i32 __cdecl GameDrawCharacterModel(CHARACTER_MODEL_LAYERS_PC *, ANIMPACKET_PC *, NUMTX *, NUMTX *, NUMTX *, NUMTX *,
                                   GAMEOBJECT *, u32);
void __cdecl DrawCharacterAttachments(GAMEOBJECT *, NUMTX *);
void __cdecl CustomiserDrawAccessoriesPc(CUSTOMISER_LAYERS_PC *, GAMEOBJECT *, NUMTX *);
static void ParaphernaliaShieldPc(GAMEOBJECT *, i32);
void __cdecl ParaphernaliaCommunicatePc(GAMEOBJECT *, i32);
static void ParaphernaliaThrustPc(GAMEOBJECT *, i32);
void __cdecl DrawForceGlowSpritePc(NUVEC *, f32, i32, f32, GAMEOBJECT *);
static void ParaphernaliaPlacePc(GAMEOBJECT *, i32, i32, i32);
static void ParaphernaliaRemovePc(GAMEOBJECT *, i32, i32, i32);
i32 __cdecl BatarangGetObjectFromCharIdPc(i32);
void __cdecl Grapple_DrawLine(GAMEOBJECT *);
void __cdecl SuperCarry_DrawObject(GAMEOBJECT *);
static void ParaphernaliaHandPc(GAMEOBJECT *, i32);
void __cdecl DrawTorpedos(GAMEOBJECT *);
void __cdecl GizDrawBuildItPiece(GAMEOBJECT *, i32);
void __cdecl TransformDrawTargetPc(NUVEC *, f32, f32);

void __cdecl DrawParaphernaliaPc(GAMEOBJECT *object) {
    i32 i;
    NUMTX matrix, reflected, special_reflected, attachments[16];
    PARAPHERNALIA_EXTRA_PC *extra;
    GAMEOBJECT *carried;
    PARAPHERNALIA_EXTRA_PC *carried_extra;
    i32 ride_locator;
    NUVEC carried_position;
    NUMTX *carried_reflection;
    u32 carried_layers;
    GAMEOBJECT *passenger;
    i32 passenger_locator;
    NUANGVEC passenger_rotation;
    PLAYERCHARACTERCONFIG_PC *config;
    i32 scene_locator;
    NUVEC glow_position;
    f32 glow_radius;
    f32 glow_alpha;
    f32 held_glow_alpha;
    i32 helmet_locator;
    f32 transform_alpha;
    f32 transform_radius;
    f32 *time;
    i32 special_locator;
    f32 distance;
    i32 chosen;
    f32 next;
    NUMTX *special_matrix;
    f32 start;
    f32 finish;
    f32 fraction;
    NUVEC special_scaling;
    i32 special_phase;

    extra = ((PARAPHERNALIA_EXTRA_PC *)(object));
    if (DrawParaphernaliaEnabledPc == 0)
        return;

    carried = object->takeover_source;
    if (carried != NULL && object->character_context != 0x3b &&
        (object->character_data->game_character->flags_098[0] & 0x40) == 0) {
        carried_extra = ((PARAPHERNALIA_EXTRA_PC *)(carried));
        carried_extra->draw_reflection = extra->draw_reflection;
        carried->reflection_enabled = object->reflection_enabled;
        carried->reflection_height = object->reflection_height;
        ride_locator = object->character_data->game_character->ride_locator;
        if (ride_locator != -1 && object->character_model->points_of_interest[ride_locator] != NULL) {
            matrix = object->locator_matrices[ride_locator];
        } else {
            carried_position.y = object->bounds_half_height;
            carried_position.x = 0.0f;
            NuVecMtxRotate(&carried_position, &carried_position, &object->transform);
            NuVecAdd(&carried_position, &carried_position, &object->collision_position);
            matrix = object->transform;
            matrix.translation = carried_position;
        }
        if (carried->reflection_enabled != 0 && carried->reflection_height != 2000000.0 &&
            object->ai_update_distance < ((u8)(WORLD->current_level->reflection_range)) &&
            MatrixReflection(&matrix, carried->reflection_enabled, carried->reflection_height,
                             WORLD->current_level->reflection_y, &reflected))
            carried_reflection = &reflected;
        else
            carried_reflection = NULL;
        carried_layers = AdjustLayerBits(carried->character_data->game_character->layer_mask_medium, carried);
        if (GameDrawCharacterModel(carried->character_model, &carried->anim_packet, &matrix, NULL, carried_reflection,
                                   attachments, NULL, carried_layers)) {
            if (carried->helmet != 0)
                DrawObjectOnCharacter(WORLD, carried, carried->helmet + 0xf9, NULL,
                                      carried->character_data->game_character->helmet_locator, -1, attachments,
                                      carried_extra->draw_reflection, carried_layers, NULL, NULL, 1.0f, 1.0f);
            if (Cheat_IsOn(2))
                DrawObjectOnCharacter(WORLD, carried, 0xe7, NULL, carried->character_data->game_character->head_locator,
                                      -1, attachments, carried_extra->draw_reflection, carried_layers, NULL, NULL, 1.0f,
                                      1.0f);
            DrawCharacterAttachments(carried, attachments);
            if (((i16)(carried->id)) == PermanentWeirdo1IdPc || ((i16)(carried->id)) == PermanentWeirdo2IdPc)
                CustomiserDrawAccessoriesPc(CharacterCustomiser, carried, attachments);
        }
    } else if ((object->flags_e24 & 1) != 0 && object->force_target != NULL) {
        passenger = object->force_target;
        passenger_locator = object->character_data->game_character->weapon_joints[0];
        if (passenger_locator != -1 && object->character_model->points_of_interest[passenger_locator] != NULL) {
            matrix = object->locator_matrices[passenger_locator];
        } else {
            passenger_rotation.x = -0x4000;
            passenger_rotation.y = object->field_0x276;
            NuMtxSetRotationXYVU0(&matrix, &passenger_rotation);
            matrix.m30 = object->position.x + NuTrigTable[object->field_0x276 >> 1] * object->bounds_radius * 1.75;
            matrix.m31 =
                object->collision_min.y + (object->collision_max.y - object->collision_min.y) * 0.60000002384185791;
            matrix.m32 = object->position.z +
                         NuTrigTable[((object->field_0x276 + 0x4000) >> 1) & 0x7fff] * object->bounds_radius * 1.75;
        }
        GameDrawCharacterModel(passenger->character_model, &passenger->anim_packet, &matrix, NULL, NULL, NULL, NULL,
                               passenger->character_data->game_character->layer_mask_medium);
    }
    config = object->character_data->game_character;
    if ((config->flags_094 & 1) != 0) {
        if (BonusArea != 0 && VehicleArea != 0)
            CharMiniKit_Draw(((i16)(object->id)), &object->transform, object->reflection_enabled,
                             object->reflection_height, WORLD->current_level->reflection_y);
    } else if ((object->character_data->flags & 1) != 0) {
        scene_locator = config->thingy_locator;
        matrix = scene_locator != -1 && object->character_model->points_of_interest[scene_locator] != NULL &&
                         object->model_draw_result != 0
                     ? object->locator_matrices[scene_locator]
                     : object->transform;
        reflected = object->draw_tertiary_mtx;
        CharScene_Draw(WORLD, ((i16)(object->id)), &matrix, extra->draw_reflection ? &reflected : NULL);
    } else if (((i16)(object->id)) == id_CATAPULT &&
               (object->character_context != 0x0a || (object->context_variant_flags & 0x40) == 0 ||
                object->quick_shoot_bolt_id != -1 ||
                (object->context_timer < 0.5f && ((u32)(GameTimer.update_count)) % 6 < 3))) {
        dco_prerotatez = 0xc000;
        DrawObjectOnCharacter(WORLD, object, 0xf0, NULL, object->character_data->game_character->weapon_shoot_joints[0],
                              -1, object->locator_matrices, 0, object->layer_mask, NULL, NULL, 1.0f, 1.0f);
    }
    if (object->character_data->game_character->weapon_action == 1 &&
        AnimPlaying(&object->anim_packet, 0x60, 1, 1) != NULL) {
        DrawObjectOnCharacter(WORLD, object, 9, NULL, object->character_data->game_character->weapon_joints[0], -1,
                              object->locator_matrices, extra->draw_reflection, object->layer_mask, NULL, NULL, 1.0f,
                              1.0f);
    } else if (object->weapon_scale > 0.0f) {
        DrawWeapons(object, extra->draw_reflection, object->weapon_scale);
        if (object->field_d54 > 0.0f) {
            dco_locatorposonly = 1;
            DrawObjectOnCharacter(
                WORLD, object, 0x35, NULL, object->character_data->game_character->weapon_shoot_joints[0], -1,
                object->locator_matrices, extra->draw_reflection, object->layer_mask, NULL, NULL, 1.0f, 1.0f);
        }
    } else {
        DrawWeapons(object, extra->draw_reflection, ParaphernaliaWeaponScalePc(object));
    }
    if (object->origin_scale_override > 0.0f || object->timer_d34 > 0.0f) {
        ResetShadowMapRendering();
        ParaphernaliaShieldPc(object, extra->draw_reflection);
        EnableShadowMapRendering(0);
    }
    if (object->communicate_blend > 0.0f)
        ParaphernaliaCommunicatePc(object, extra->draw_reflection);
    if (object->character_context == 0x22) {
        if ((WORLD->area != NULL && WORLD->area == HUB_ADATA) || WORLD->current_level == LevelFixupSlotPc0087b194 ||
            WORLD->current_level == DOOKUC_LDATA) {
            glow_position.x = object->collision_position.x;
            glow_position.y = object->character_bottom * object->origin_scale + object->position.y +
                              (object->character_top - object->character_bottom) * 0.5 * object->origin_scale;
            glow_position.z = object->collision_position.z;
            glow_radius =
                object->bounds_radius > object->bounds_half_height ? object->bounds_radius : object->bounds_half_height;
            glow_radius = glow_radius * 1.75;

            if (object->context_hover_target_y < 0.20000000298023224)
                glow_alpha = object->context_hover_target_y / 0.20000000298023224;
            else if (object->context_timer < 0.20000000298023224)
                glow_alpha = object->context_timer / 0.20000000298023224;
            else
                glow_alpha = 1.0f;
            DrawForceGlowSpritePc(&glow_position, glow_radius, 0xdf, glow_alpha, object);
        }
    } else if (extra->force_alpha > 0.0f && extra->force_radius > 0.0f &&
               WORLD->level_objects[object->field_e1e].active != 0 &&
               (object->character_data->game_character->flags_090 & 0x400) == 0) {
        held_glow_alpha = ((f32)(extra->force_alpha / FORCEGLOWTIME));
        DrawForceGlowSpritePc(&extra->force_position, extra->force_radius, object->field_e1e, held_glow_alpha, object);
    }
    if (Cheat_IsOn(2))
        DrawObjectOnCharacter(WORLD, object, 0xe7, NULL, object->character_data->game_character->head_locator, -1,
                              object->locator_matrices, extra->draw_reflection, object->layer_mask, NULL, NULL, 1.0f,
                              1.0f);
    if (object->helmet != 0) {
        if ((((i16)(object->id)) == id_CHEWBACCA || ((i16)(object->id)) == id_WOOKIEE) && object->helmet != 5 &&
            object->helmet != 6)
            helmet_locator = 9;
        else
            helmet_locator = object->character_data->game_character->helmet_locator;
        DrawObjectOnCharacter(WORLD, object, object->helmet + 0xf9, NULL, helmet_locator, -1, object->locator_matrices,
                              extra->draw_reflection, object->layer_mask, NULL, NULL, 1.0f, 1.0f);
    }
    if (object->model_draw_result != 0 && object->character_data->game_character->thrust_locators != 0 &&
        object->thrust_effect_scale > 0.0f)
        ParaphernaliaThrustPc(object, extra->draw_reflection);
    if (object->character_context == 0x2d)
        GizDrawBuildItPiece(object, extra->draw_reflection);
    if (object->character_context == 0x48 && object->character_model->model_data_b[object->field_7aa] != NULL &&
        object->field_7b3 == 0) {
        ParaphernaliaPlacePc(object, 0xec, extra->draw_reflection,
                             object->character_data->game_character->place_locator);
    } else if (object->character_context == 0x49 && object->character_model->model_data_b[object->field_7aa] != NULL &&
               object->field_7b3 != 0) {
        ParaphernaliaRemovePc(object, 0xec, extra->draw_reflection,
                              object->character_data->game_character->place_locator);
    }
    if ((object->flags_e20 & 0x400000) != 0) {
        dco_prerotatez = 0x4000;
        DrawObjectOnCharacter(WORLD, object, BatarangGetObjectFromCharIdPc(((i16)(object->id))), NULL,
                              object->character_data->game_character->throw_locator, -1, object->locator_matrices,
                              extra->draw_reflection, object->layer_mask, NULL, NULL, 1.0f, 1.0f);
    }
    if (object->character_context == 0x46)
        Grapple_DrawLine(object);
    if ((object->flags_e20 & 0x800000) != 0 || object->character_context == 0x58)
        SuperCarry_DrawObject(object);
    DrawCharacterAttachments(object, object->locator_matrices);
    if (object->character_context == 0x5d)
        ParaphernaliaHandPc(object, extra->draw_reflection);
    if (object->torpedo != NULL && object->torpedo->count > 0)
        DrawTorpedos(object);
    if (object->character_data->game_character->weapon_action == 0)
        CustomiserDrawAccessoriesPc(CharacterCustomiser, object, NULL);
    if ((object->character_data->game_character->flags_090 & 0x1000000) != 0 && extra->force_alpha > 0.0f &&
        extra->force_radius > 0.0f) {
        transform_alpha = ((f32)(extra->force_alpha * 0.60000002384185791 + 0.40000000596046448));
        transform_radius =
            ((f32)((extra->force_alpha * 0.20000004768371582 + 1.3999999761581421) * extra->force_radius));
        TransformDrawTargetPc(&extra->force_position, transform_radius, transform_alpha);
    }
    config = object->character_data->game_character;

    if (config->weapon_action == 1) {
        special_locator = config->thingy_locator;
        if (special_locator == -1 || object->character_model->points_of_interest[special_locator] == NULL ||
            (time = AnimPlaying(&object->anim_packet, 0x6b, 1, 1)) == NULL)
            return;
        if ((*time >= 70.0f && *time <= 210.0) || (*time >= 310.0f && *time <= 433.0)) {
            matrix = object->locator_matrices[special_locator];
            NuMtxPreRotateZ(&matrix, -0x4000);
            NuSpecialDrawAt(&WORLD->level_objects[10].special, &matrix);
            if (extra->draw_reflection != 0 &&
                MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                                 WORLD->current_level->reflection_y, &special_reflected))
                NuSpecialDrawAt(&WORLD->level_objects[10].special, &special_reflected);
        } else {
            distance = 1.0f;
            chosen = -1;
            for (i = 10; i < 18; ++i) {
                if (NuSpecialExistsFn(&LevHSpecial[i])) {
                    next = NuVecDistSqr(&object->collision_position, NuSpecialGetDrawPos(&LevHSpecial[i]), NULL);
                    if (next < distance) {
                        distance = next;
                        chosen = i;
                    }
                }
            }
            if (chosen != -1) {
                special_matrix = NuSpecialGetDrawMtx(&LevHSpecial[chosen]);
                NuSpecialDrawAt(&WORLD->level_objects[10].special, special_matrix);
            }
        }
    } else if (((i16)(object->id)) == CharacterFixupIdPc007f1820 &&
               (time = AnimPlaying(&object->anim_packet, 1, 0, 0)) != NULL) {
        start = AnimListFrame(object->character_model, 1, 0);
        if (start >= 1.0f) {
            finish = AnimListFrame(object->character_model, 1, 1);
            if (finish >= start && *time > start && *time < finish) {
                fraction = (*time - start) / (finish - start);

                if (fraction < 0.10000000149011612)
                    special_scaling.x = fraction / 0.10000000149011612;
                else if (fraction < 0.89999997615814209)
                    special_scaling.x = 1.0f;
                else
                    special_scaling.x = 1.0 - (fraction - 0.89999997615814209) / 0.10000000149011612;
                special_scaling.y = special_scaling.z = special_scaling.x;
                NuMtxSetScale(&matrix, &special_scaling);
                NuMtxRotateX(&matrix, ((u16)(((i32)(fraction * -65536.0)))));
                NuMtxRotateY(&matrix, object->field_0x276);
                NuMtxTranslate(&matrix, &object->locator_matrices[0].translation);
                special_phase = ((i32)(fraction * 32768.0));
                matrix.m31 = matrix.m31 + NuTrigTable[(special_phase >> 1) & 0x7fff] * 0.20000000298023224;
                DrawObjectOnCharacter(WORLD, object, 0xf9, NULL, 0, -1, &matrix, extra->draw_reflection,
                                      object->layer_mask, NULL, NULL, 1.0f, 1.0f);
            }
        }
    }
}

/* Native 0x0041fe90, 526 bytes. */
typedef struct ai_creature_init_packet_pc_s {
    u8 unknown_000[0xa0];
    void *area;
    void *locator;
} AI_CREATURE_INIT_PACKET_PC;
STATIC_ASSERT(offsetof(AI_CREATURE_INIT_PACKET_PC, area) == 0xa0);
STATIC_ASSERT(offsetof(AI_CREATURE_INIT_PACKET_PC, locator) == 0xa4);
STATIC_ASSERT(offsetof(AIPACKET_PC, creature_index) + offsetof(GAMEOBJECT, ai_packet) == 0x400);

void __cdecl InitAICreatures(AISYS_PC *system) {
    i32 creature_index, member, type;
    AICREATURE_PC *creature;
    GAMEOBJECT *object;
    AIGROUP_PC *group;
    u32 model_flags;
    if (NOAICREATURES != 0 || Mission_Active(NULL) != NULL || netclient != 0 || system == NULL)
        return;
    for (creature_index = 0; creature_index < system->creature_count; ++creature_index) {
        creature = &system->creatures[creature_index];
        group = NULL;
        for (member = 0; member < creature->count; ++member) {
            if (((u64)creature->active_mask & ((u64)1 << member)) == 0)
                continue;
            type = creature->type;
            if ((u32)type > 339 || apicharsys->playermodelids[type] == -1)
                continue;
            object = AddGameObject();
            if (object == NULL)
                continue;
            InitCreature(object, type, 1);
            object->flags_1f8 |= 0x400;
            object->flags_1fc_word &= ~0x1000u;
            model_flags = apicharsys->char_data[creature->type].model_flags;
            if ((model_flags & 0x200) != 0)
                object->flags_1f8 |= 4;
            else if ((model_flags & 4) != 0)
                object->flags_1f8 |= 1;
            object->flags_1044 |= 1;
            if ((apicharsys->char_data[creature->type].model_flags & 0x1000) != 0)
                object->flags_1044 |= 4;
            object->ai_packet.creature_index = (u8)creature_index;
            if (member == 0 && creature->count > 1 && creature->start_stagger == 0.0f)
                group = CreateAIGroup(system, creature->count_across, creature->x_spacing, creature->z_spacing,
                                      object->character_data->game_character->run_speed);
            if (group != NULL)
                AddToAIGroup(group, (APIOBJECT_PC *)object);
            ((AI_CREATURE_INIT_PACKET_PC *)&object->ai_packet)->area = creature->area;
            ((AI_CREATURE_INIT_PACKET_PC *)&object->ai_packet)->locator = creature->locator;
            object->ai_packet.respawn_locator = (AILOCATOR_PC *)creature->respawn_locator;
            object->ai_packet.creature_set = creature->set;
        }
    }
    system->reset_flags &= ~1u;
}

static void ParaphernaliaShieldPc(GAMEOBJECT *object, i32 reflection) {
    PLAYERCHARACTERCONFIG_PC *config = object->character_data->game_character;
    i32 locator = config->shield_locator;
    NUVEC scaling;
    NUMTX matrix, reflected;
    if (object->shield_hitpoints == 0 && config->shield_hitpoints > 0 && object->timer_d34 > 0.0f) {
        if ((GameTimer.update_count & 3u) >= 2)
            return;
        scaling.x = 1.0f;
    } else {
        if (object->origin_scale_override <= 0.0f)
            return;
        scaling.x = object->origin_scale_override;
    }
    scaling.y = scaling.z = scaling.x;
    NuMtxSetScale(&matrix, &scaling);
    if (locator != -1 && object->character_model->points_of_interest[locator] != NULL)
        NuMtxTranslate(&matrix, (NUVEC *)&object->locator_matrices[locator].m30);
    else
        NuMtxTranslate(&matrix, &object->collision_position);
    Draw3DObjectMtx(NULL, object->timer_d34 > 0.0f ? 0x70 : 0x6f, &matrix);
    if (reflection != 0 && MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                                            WORLD->current_level->reflection_y, &reflected)) {
        NuRndrStartReflectionRender(0);
        Draw3DObjectMtx(NULL, object->timer_d34 > 0.0f ? 0x93 : 0x92, &reflected);
        NuRndrEndReflectionRender();
    }
}

static void ParaphernaliaThrustPc(GAMEOBJECT *object, i32 reflection) {
    i32 locator, effect_index;
    if (WORLD->level_objects[0x77].active == 0 || WORLD->level_objects[0x78].active == 0)
        return;
    effect_index = 0;
    for (locator = 0; locator < 16; ++locator) {
        f32 ratio;
        NUVEC scaling;
        NUMTX matrix, reflected;
        i32 first, second;
        if ((object->character_data->game_character->thrust_locators & (1u << locator)) == 0 ||
            object->character_model->points_of_interest[locator] == NULL)
            continue;
        ratio = (f32)((f64)((u8 *)object)[0xe28 + effect_index++] / 255.0);
        scaling.x = (f32)((((f64)ratio - 0.5) * 0.5) * object->thrust_effect_scale + object->thrust_effect_scale);
        if (scaling.x <= 0.0f)
            continue;
        scaling.y = scaling.z = scaling.x;
        matrix = object->locator_matrices[locator];
        NuMtxPreScale(&matrix, &scaling);
        Draw3DObjectMtx(NULL, 0x77, &matrix);
        Draw3DObjectMtx(NULL, 0x78, &matrix);
        if (reflection != 0 && MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                                                WORLD->current_level->reflection_y, &reflected)) {
            first = LevelObject_GetReflection(0x77);
            if (WORLD->level_objects[first].active == 0)
                first = 0x77;
            second = LevelObject_GetReflection(0x78);
            if (WORLD->level_objects[second].active == 0)
                second = 0x77;
            NuRndrStartReflectionRender(0);
            /* Retail draws the original matrix in this reflection pass. */
            Draw3DObjectMtx(NULL, first, &matrix);
            Draw3DObjectMtx(NULL, second, &matrix);
            NuRndrEndReflectionRender();
        }
    }
}

void __cdecl ParaphernaliaCommunicatePc(GAMEOBJECT *object, i32 reflection) {
    PLAYERCHARACTERCONFIG_PC *config = object->character_data->game_character;
    i32 locator = config->thingy_locator;
    i32 model, reflection_model;
    NUMTX matrix, reflected;
    NUVEC scaling, blend_scaling;
    if (locator == -1 || object->character_model->points_of_interest[locator] == NULL)
        return;
    model = -1;
    if ((object->character_data->model_flags & 0x40) != 0) {
        scaling.z = object->communicate_blend * object->origin_scale;
        scaling.x = scaling.y = scaling.z;
        NuMtxSetScale(&matrix, &scaling);
        GameDrawMtxRotateYPc(&matrix, (NUANG)(object->field_0x276 + 0x8000));
        matrix.m30 = object->locator_matrices[locator].m30;
        matrix.m31 = object->locator_matrices[locator].m31;
        matrix.m32 = object->locator_matrices[locator].m32;
        if (object->field_283 == 9 && matrix.m31 < object->water_height)
            matrix.m31 = object->water_height;
        model = 0x19;
    } else {
        if ((config->flags_094 & 0x80) != 0)
            model = 0xe;
        matrix = object->locator_matrices[locator];
        if (object->communicate_blend != 1.0f) {
            blend_scaling.z = object->communicate_blend;
            blend_scaling.x = blend_scaling.y = blend_scaling.z;
            NuMtxPreScale(&matrix, &blend_scaling);
        }
        if (model == -1)
            return;
    }
    if (WORLD->level_objects[model].active == 0)
        return;
    NuSpecialDrawAt(&WORLD->level_objects[model].special, &matrix);
    reflection_model = LevelObject_GetReflection(model);
    if (reflection_model != -1 && WORLD->level_objects[reflection_model].active != 0 && reflection != 0 &&
        MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                         WORLD->current_level->reflection_y, &reflected)) {
        NuRndrStartReflectionRender(0);
        NuSpecialDrawAt(&WORLD->level_objects[reflection_model].special, &reflected);
        NuRndrEndReflectionRender();
    }
}

static void ParaphernaliaPlacePc(GAMEOBJECT *object, i32 model, i32 reflection, i32 locator) {
    NUVEC scaling;
    NUMTX matrix;
    NUMTX reflected;
    f32 *frame;
    f32 end;
    f32 start;
    f32 marker;
    i32 extra_model;
    if (WORLD->level_objects[model].active == 0 || locator == -1 ||
        object->character_model->points_of_interest[locator] == NULL)
        return;
    frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
    if (frame == NULL)
        return;
    end = NuAnimEndFrame(object->character_model->model_data_b[object->field_7aa]);
    start = AnimListFrame(object->character_model, object->field_7aa, 0);
    if (start >= end)
        return;
    if (start < 1.0f)
        start = 1.0f;
    if (*frame < start)
        return;
    marker = AnimListFrame(object->character_model, object->field_7aa, 1);
    if (marker <= start)
        return;
    if (end < marker)
        marker = end;
    if (*frame < marker)
        scaling.x = (*frame - start) / (marker - start);
    else
        scaling.x = 1.0f;
    scaling.y = scaling.z = scaling.x;
    matrix = object->locator_matrices[locator];
    if (model == 0xec) {
        extra_model = 0xed;
        NuMtxPreRotateY(&matrix, object->takeover_start_angle);
    } else {
        extra_model = -1;
    }
    NuMtxPreScale(&matrix, &scaling);
    NuSpecialDrawAt(&WORLD->level_objects[model].special, &matrix);
    if (extra_model != -1 && WORLD->level_objects[extra_model].active != 0)
        NuSpecialDrawAt(&WORLD->level_objects[extra_model].special, &matrix);
    if (reflection != 0 && MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                                            WORLD->current_level->reflection_y, &reflected)) {
        NuSpecialDrawAt(&WORLD->level_objects[model].special, &reflected);
        /* Retail draws the attachment with the unreflected matrix here. */
        if (extra_model != -1 && WORLD->level_objects[extra_model].active != 0)
            NuSpecialDrawAt(&WORLD->level_objects[extra_model].special, &matrix);
    }
}

static void ParaphernaliaRemovePc(GAMEOBJECT *object, i32 model, i32 reflection, i32 locator) {
    NUVEC scaling;
    NUMTX matrix;
    NUMTX reflected;
    f32 *frame;
    f32 end;
    f32 start;
    f32 fade_start;
    f32 fade_end;
    i32 extra_model;
    if (WORLD->level_objects[model].active == 0 || locator == -1 ||
        object->character_model->points_of_interest[locator] == NULL)
        return;
    frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
    if (frame == NULL)
        return;
    end = NuAnimEndFrame(object->character_model->model_data_b[object->field_7aa]);
    start = AnimListFrame(object->character_model, object->field_7aa, 0);
    if (start >= end)
        return;
    if (start < 1.0f)
        start = 1.0f;
    if (*frame < start)
        return;
    fade_start = AnimListFrame(object->character_model, object->field_7aa, 1);
    if (fade_start <= start)
        return;
    fade_end = AnimListFrame(object->character_model, object->field_7aa, 2);
    if (fade_end < fade_start)
        fade_end = fade_start;
    if (end < fade_start)
        fade_start = end;
    if (end < fade_end)
        fade_end = end;
    if (*frame <= fade_start) {
        scaling.x = 1.0f;
    } else {
        if (!(*frame < fade_end) || !(fade_start < fade_end))
            return;
        scaling.x = 1.0f - (*frame - fade_start) / (fade_end - fade_start);
    }
    scaling.y = scaling.z = scaling.x;
    matrix = object->locator_matrices[locator];
    if (model == 0xec) {
        extra_model = 0xed;
        NuMtxPreRotateY(&matrix, object->takeover_start_angle);
    } else {
        extra_model = -1;
    }
    NuMtxPreScale(&matrix, &scaling);
    NuSpecialDrawAt(&WORLD->level_objects[model].special, &matrix);
    if (extra_model != -1 && WORLD->level_objects[extra_model].active != 0)
        NuSpecialDrawAt(&WORLD->level_objects[extra_model].special, &matrix);
    if (reflection != 0 && MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                                            WORLD->current_level->reflection_y, &reflected)) {
        NuSpecialDrawAt(&WORLD->level_objects[model].special, &reflected);
        /* Retail draws the attachment with the unreflected matrix here. */
        if (extra_model != -1 && WORLD->level_objects[extra_model].active != 0)
            NuSpecialDrawAt(&WORLD->level_objects[extra_model].special, &matrix);
    }
}

static void ParaphernaliaHandPc(GAMEOBJECT *object, i32 reflection) {
    NUMTX matrix;
    NUMTX reflected;
    i32 locator;
    if (object->character_context != 0x5d || WORLD->level_objects[0xf7].active == 0)
        return;
    locator = object->character_data->game_character->hand_locators[0];
    if (locator == -1 || object->character_model->points_of_interest[locator] == NULL)
        return;
    NuMtxSetTranslation(&matrix, (NUVEC *)&object->locator_matrices[locator].m30);
    Draw3DObjectMtx(NULL, 0xf7, &matrix);
    if (reflection != 0 && MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height,
                                            WORLD->current_level->reflection_y, &reflected)) {
        NuRndrStartReflectionRender(0);
        Draw3DObjectMtx(NULL, 0xf7, &reflected);
        NuRndrEndReflectionRender();
    }
}

void __cdecl ConfigureComplexShadow(GAMEOBJECT *object) {
    NUVEC direction;
    f32 distance;
    if (object != NULL) {
        direction.x = 65536.0f;
        direction.y = object->collision_position.y;
        direction.z = (f32)((f64)object->floor_height - (f64)0.1f);
    } else {
        direction.x = 0.0f;
        direction.y = -ComplexShadowBasePc * 0.25;
        direction.z = 0.0f;
    }
    distance = VehicleArea != 0 ? 150.0f : 30.0f;
    NuRndrShadowDirCol(&direction, 0x17000000u, distance, distance);
}
