#include "saga.h"
#include <string.h>
typedef struct TERRAIN_PLAYER_IMPACT_PC {
    NUVEC position, normal;
    u8 material[2], flags, normal_flags;
} TERRAIN_PLAYER_IMPACT_PC;
STATIC_ASSERT(sizeof(TERRAIN_PLAYER_IMPACT_PC) == 0x1c);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_IMPACT_PC, normal) == 0xc);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_IMPACT_PC, material) == 0x18);
typedef struct TERRAIN_PLAYER_PLATFORM_RECORD_PC {
    u8 unknown_0[0xe];
    i16 platform_id;
    GAMEOBJECT *object;
} TERRAIN_PLAYER_PLATFORM_RECORD_PC;
typedef struct TERRAIN_PLAYER_PLATFORM_SYS_PC {
    u8 unknown_0[4];
    i32 count;
    TERRAIN_PLAYER_PLATFORM_RECORD_PC entries[1];
} TERRAIN_PLAYER_PLATFORM_SYS_PC;
STATIC_ASSERT(sizeof(TERRAIN_PLAYER_PLATFORM_RECORD_PC) == 0x14);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_PLATFORM_RECORD_PC, platform_id) == 0xe);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_PLATFORM_RECORD_PC, object) == 0x10);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_PLATFORM_SYS_PC, count) == 4);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_PLATFORM_SYS_PC, entries) == 8);
typedef struct TERRAIN_PLAYER_GRABBER_PC {
    u8 unknown_0[0x554];
    f32 platform_contact_timer;
    u8 unknown_558[8];
    i16 platform_id;
} TERRAIN_PLAYER_GRABBER_PC;
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_GRABBER_PC, platform_contact_timer) == 0x554);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_GRABBER_PC, platform_id) == 0x560);
typedef struct TERRAIN_PLAYER_WORLD_PC {
    u8 unknown_0[0x46ec];
    TERRAIN_PLAYER_GRABBER_PC *grabber;
} TERRAIN_PLAYER_WORLD_PC;
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_WORLD_PC, grabber) == 0x46ec);
typedef struct TERRAIN_PLAYER_BLOWUP_PC {
    u8 unknown_0[0x102];
    i16 platform_id;
} TERRAIN_PLAYER_BLOWUP_PC;
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_BLOWUP_PC, platform_id) == 0x102);
typedef struct TERRAIN_PLAYER_COINS_PC {
    u32 coins;
    f32 scale;
} TERRAIN_PLAYER_COINS_PC;
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_COINS_PC, coins) == 0);
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_COINS_PC, scale) == 4);
typedef struct TERRAIN_PLAYER_SET_PC {
    u8 unknown_0[0x1bac];
    i32 minimum_height;
} TERRAIN_PLAYER_SET_PC;
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_SET_PC, minimum_height) == 0x1bac);
typedef struct TERRAIN_PLAYER_CHARACTER_PC {
    u8 unknown_0[0x18];
    void(__cdecl *move_callback)(GAMEOBJECT *);
} TERRAIN_PLAYER_CHARACTER_PC;
STATIC_ASSERT(offsetof(TERRAIN_PLAYER_CHARACTER_PC, move_callback) == 0x18);
extern AREA_DATA_PC *HOTHBATTLE_ADATA, *DAGOBAH_ADATA;
extern LEVELDATA_PC *ENDORBATTLEC_LDATA, *ASTEROIDCHASEC_LDATA, *CRUISERD_LDATA;
void __cdecl Player_ClearContext(GAMEOBJECT *, i32);
extern i32 NOTERRAINSTOP;
extern i16 id_TRACTOR, id_SNAKE;
extern i32 LEGO_AIPATHCNX_BLOCKAGE, LEGO_AIPATHCNX_WALLSHUFFLE, LEGO_AIPATHCNX_FULLTERRAIN;
extern f32 TerrainPlayerPlatConnectTolerancePc, TerrainPlayerBikeMovementScalePc, TerrainPlayerOtherMovementScalePc;
extern i32 TerrainPlayerShadowFixPc;
void __cdecl TakeOverCode(GAMEOBJECT *, i32);
void __cdecl ApplyGravity(GAMEOBJECT *, f32 *, f32, f32, f32 *);
void __cdecl Tag_Check(GAMEOBJECT *);
void __cdecl GameObjectRotation(GAMEOBJECT *, i32);
i32 __cdecl UnderWater(GAMEOBJECT *);
i32 __cdecl IntersectWater(GAMEOBJECT *);
void __cdecl Surface_Deflect(NUVEC *, NUVEC *, NUVEC *, i32);
void __cdecl TerrainSetPlatConnectTol(f32);
void __cdecl TerrainSetImpactData(void *, i32 *, i32);
GAMEOBJECT *__cdecl CharPlatform_FindObjFromPlatID(void *, i32);
void __cdecl StartFlatten(GAMEOBJECT *, GAMEOBJECT *);
void __cdecl InstantKillParts(GAMEOBJECT *, i32, f32);
i32 __cdecl KillPlayer(GAMEOBJECT *, i32, i32, NUVEC *);
i32 __cdecl ObjHitObj(GAMEOBJECT *, GAMEOBJECT *, i32, u16, i32, i32);
i32 __cdecl LoseCoins(GAMEOBJECT *, i32);
void __cdecl AddPickups(i32, i32, i32, i32, NUVEC *, NUVEC *, f32, i32, f32, f32, GAMEOBJECT *, i32, i32);
void __cdecl AddWaterSplash(GAMEOBJECT *, NUVEC *);
void __cdecl PreResetCode(GAMEOBJECT *);
void __cdecl PostResetCode(GAMEOBJECT *);
void __cdecl BigJumpCode(GAMEOBJECT *);
static void TerrainPlayerResetContactPc(GAMEOBJECT *object) {
    object->jump_animation_state = 0;
    object->raw_surface_id_pc = 0;
    object->surface_normal = v010;
    object->water_height = 2000000.0f;
    object->roof_height_pc = 2000000.0f;
    object->field_283 = -1;
    object->roof_surface_id_pc = (u8)-1;
    object->terrain_platform_id = -1;
}
static i32 TerrainKillPlayer(GAMEOBJECT *object, i32 surface, NUVEC *normal) {
    u32 flags;

    flags = TerSurface[surface].flags;
    if ((flags & 1) == 0 &&
        ((flags & 0x4000) == 0 || (object->character_data->model_flags & 0x10) != 0 ||
         (object->character_data->game_character->flags_090 & 0x8000) != 0) &&
        ((flags & 0x8000) == 0 || ((i8)object->field_280 != -1 && !(normal->y > 0.0f)))) {
        return 0;
    }
    InstantKillParts(object, 1, 0.0f);
    if (((object->flags_1fc_word & 0x80) != 0) && BonusWinner == -1) {
        i32 coins;

        coins = LoseCoins(object, 1);
        AddPickups(coins, 0, 0, 0, &object->collision_position, NULL, 2.0f, -1, 1.0f, 2000000.0f, object, 1, 0);
    }
    KillPlayer(object, 2, 1, NULL);
    return 1;
}

void __cdecl TerrainPlayer(GAMEOBJECT *object) {
    NUVEC forward;
    GAMEPAD_PC *entry_pad;
    u8 entry_underwater;
    u8 entry_intersects_water;
    i32 entry_menu_id;
    i16 entry_platform;
    TERRAIN_PLAYER_IMPACT_PC impact_records[8];
    i32 impact_count;

    entry_underwater = object->is_underwater;
    entry_pad = object->pad;
    entry_intersects_water = object->water_intersects;
    entry_menu_id = GetMenuID();
    entry_platform = object->supporting_platform_id;

    impact_count = -1;

    if (object->dead == 0 && object->takeover_source != NULL && object->character_context == 0x3b) {
        TakeOverCode(object, object->pad->buttons_pressed & GAMEPAD_TAG);
    } else if (object->dead == 0) {
        GAMEOBJECT *api;
        i32 terrain_mask;
        f32 entry_vertical_velocity;
        const AIPATHINFO_PC *path_info;
        const AIPATHCNX_PC *path_connection;
        i32 special_path_endpoints;
        i32 shadow_grounding;
        i32 retain_floor;
        f32 lower_bound;
        i32 player_edge_probe;
        f32 movement_threshold;
        f32 speed_squared;
        i32 skip_motion;
        i32 direct_integration;
        i32 left_platform;
        i32 landed_platform;

        api = object;
        terrain_mask = GetSurfaceInfoDefaultMaskPc.word;
        if ((api->character_data->model_flags & 0x10) == 0 &&
            (api->character_data->game_character->flags_090 & 0x8000) == 0) {
            terrain_mask |= GetSurfaceInfoRuntimeMaskPc.word;
        }
        if ((api->flags_1f8 & 0x400) != 0 && object->id != id_DRAGBOMB &&
            (WORLD->area != HOTHBATTLE_ADATA || api->character_data->game_character->field_028 != 0.0f)) {
            terrain_mask |= 0x40;
        }
        object->flags_e20 &= ~8u;
        entry_vertical_velocity = api->velocity.y;
        path_info = &object->ai_packet.path_info;
        path_connection = path_info->connection;
        special_path_endpoints = 0;
        if (path_info->path != NULL && path_connection != NULL) {
            const AIPATHNODE_PC *nodes;

            nodes = path_info->path->nodes;
            special_path_endpoints = ((nodes[path_connection->node_indices[path_info->direction]].runtime_flags |
                                       nodes[path_connection->node_indices[path_info->direction == 0]].runtime_flags) &
                                      0x82) != 0;
        }
        shadow_grounding = 0;
        retain_floor = 0;
        if (object == CarWashHack) {
            shadow_grounding = 1;
        } else if ((api->character_data->game_character->flags_090 & 0x80) != 0) {
            api->velocity.x = 0.0f;
            api->velocity.z = 0.0f;
            shadow_grounding = 1;
        } else if ((api->flags_1fc_word & 0x40) == 0 && (CInfo[object->character_context].flags & 0x8000) == 0) {
            i32 ai_controlled;
            i32 ordinary_ai_path;

            ai_controlled = (api->flags_1f8 & 0x400) != 0;
            ordinary_ai_path =
                ai_controlled && object->movement_spline_position.spline == NULL && object->character_context != 0x1c &&
                object->context_target_position == NULL && !special_path_endpoints &&
                (path_info->flags & (1 | 8)) == 1 && Technos_FindControllingTechno(object) == NULL &&
                (api->flags_1fc_word & 0x40000) == 0 &&
                (path_connection == NULL ||
                 ((path_connection->original_traversal_flags[0] & ((u32)(LEGO_AIPATHCNX_BLOCKAGE))) == 0 &&
                  (path_connection->traversal_flags[0] & ((u32)(LEGO_AIPATHCNX_WALLSHUFFLE))) == 0 &&
                  (path_connection->traversal_flags[0] & ((u32)(LEGO_AIPATHCNX_FULLTERRAIN))) == 0)) &&
                (object->character_context != 0x5a || object->field_7b3 != 0);
            if (ordinary_ai_path) {
                shadow_grounding = 1;
                retain_floor = (api->flags_1fc_word & 0x10) != 0;
            } else {
                retain_floor = !ai_controlled && (0x10 & api->flags_1fc_word) != 0;
                if (0 != (object->flags_ef4 & 0x40)) {
                    object->flags_ef4 &= ~0x40u;
                    shadow_grounding = 1;
                } else {
                    shadow_grounding = (api->flags_1fc_word & 8) != 0;
                }
            }
        }

        lower_bound = object->character_bottom * api->origin_scale;

        player_edge_probe = ((api->flags_1fc_word & 0x80) != 0) && object->spawn_protection_timer > 1.25f;

        if (VehicleArea == 0 && api->dead == 0 &&
            (api->field_281 != 0 ||
             (player_edge_probe && object->character_context == -1 && object->jump_variant_state != 1))) {
            i32 walkable_floor;
            i32 probe_edge;

            walkable_floor = api->floor_height != 2000000.0f && object->surface_normal.y > NuTrigTable[0x238e];
            probe_edge =
                walkable_floor ? (player_edge_probe || 0 != ((CInfo[object->character_context].parameter & 1))) : 0;
            if (probe_edge && api->movement_direction_x == 0.0f && api->movement_direction_z == 0.0f &&
                (api->velocity.x != 0.0f || api->velocity.z != 0.0f)) {
                NUVEC next_position;
                f32 next_floor;

                next_position.x = api->position.x + api->velocity.x * FRAMETIME;
                next_position.y = api->position.y;
                next_position.z = api->position.z + api->velocity.z * FRAMETIME;
                next_floor = GameShadow(object, &next_position, 5.0f, terrain_mask | 0x1f);
                if (next_floor != 2000000.0f && ShadNorm.y > NuTrigTable[0x238e] &&
                    (player_edge_probe ? api->collision_min.y : api->floor_height) - next_floor > 0.2f) {

                    api->velocity.z = 0.0f;
                    api->velocity.x = 0.0f;
                }
            }
        }
        movement_threshold = 0.0f;
        if (!(object->pad->input_magnitude > 0.0f) && object->character_context != 0x33) {
            i32 surface;
            i32 slippery;

            surface = ((i8)(api->jump_animation_state));
            slippery =
                api->field_281 != 0 && surface >= 0 && surface <= 31 && TerSurface[surface].movement_scale < 1.0f;
            if (!slippery && AnimSpeed(api->character_model, CurrentAnim(&api->anim_packet)) == 0.0f &&
                (api->character_data->model_flags & 0x800) == 0) {
                movement_threshold = api->character_data->game_character->stop_speed * api->origin_scale;
            }
        }

        speed_squared =
            (api->velocity.x * api->velocity.x + api->velocity.y * api->velocity.y) + api->velocity.z * api->velocity.z;
        skip_motion =
            (object->flags_ef0 & 0x20) == 0 && (object->flags_ef4 & 0x400000) != 0 && !special_path_endpoints &&
            object->ai_packet.path_target == NULL && !((api->flags_1fc_word & 0x80) != 0) &&
            (object->flags_e20 & 0x10000000) == 0 && object->jump_variant_state == 0 &&
            object->character_context != 0 && object->character_context != 0x1c && api->jump_animation_state != 8 &&
            (((u32)(GameTimer.update_count)) > 1 || (api->flags_1f8 & 5) != 0) && (api->field_281 & 3) != 0 &&
            api->supporting_platform_id == -1 && movement_threshold * movement_threshold >= speed_squared &&
            object->character_context != 0x0f && object->character_context != 0x0b && object->character_context != 0x1e;
        api->flags_1fc_word = (api->flags_1fc_word & ~4u) | (skip_motion ? 4u : 0u);
        object->contact_count = 0;
        direct_integration = (api->flags_1fc_word & 0x20) != 0 || (object->flags_e20 & 0x20) != 0 ||
                             object->movement_spline_position.spline != NULL || object->move_override != NULL ||
                             (object->character_context == 0x0f && object->field_7b3 <= 1) ||
                             (object->character_context == 0x2c && object->field_7b3 == 0) ||
                             (object->character_context == 0x47 && object->action_movement_state == 1);
        if (direct_integration) {

            api->respawn_timer = 0.0f;
            object->flags_e20 |= 2;
            api->field_281 = 0;
            api->supporting_platform_id = -1;
            object->contact_material_word = 0;
            api->position.x += api->velocity.x * FRAMETIME;
            api->position.y += api->velocity.y * FRAMETIME;
            api->position.z += api->velocity.z * FRAMETIME;
        } else if (shadow_grounding || retain_floor || 0 != (WORLD->current_level->flags & 0x10)) {

            api->respawn_timer = 0.0f;
            if (!skip_motion) {
                f32 grounded_lower_bound;

                object->flags_e20 |= 2;
                api->supporting_platform_id = -1;
                api->position.x += api->velocity.x * FRAMETIME;
                api->position.y += api->velocity.y * FRAMETIME;
                api->position.z += api->velocity.z * FRAMETIME;

                if (shadow_grounding) {
                    if (object == CarWashHack) {
                        NewTerrPlatformsOff();
                    }
                    if (TerrainPlayerShadowFixPc != 0) {
                        NUVEC shadow_position;

                        shadow_position = api->collision_position;
                        shadow_position.y = api->initial_position.y > api->collision_position.y
                                                ? api->initial_position.y
                                                : api->collision_position.y;
                        api->floor_height = GameShadow(object, &shadow_position, 5.0f, terrain_mask | 0x1f);
                    } else {
                        api->floor_height = GameShadow(object, &api->collision_position, 5.0f, terrain_mask | 0x1f);
                    }
                    api->supporting_platform_id = ((i16)(NewShadowOnPlatform()));
                    GetSurfaceInfo(object, api->floor_height != 2000000.0f ? 1 : 0, api->floor_height);
                    if (api->floor_height == 2000000.0f)
                        TerrainPlayerResetContactPc(object);
                    api->is_underwater = ((u8)(UnderWater(object)));
                    api->water_intersects = ((u8)(IntersectWater(object)));
                    if (api->supporting_platform_id == -1 && api->field_281 != 0 && object->terrain_platform_id != -1) {
                        api->supporting_platform_id = object->terrain_platform_id;
                    }
                }
                grounded_lower_bound = object->character_bottom * api->origin_scale;
                if (api->floor_height != 2000000.0f && api->position.y + grounded_lower_bound < api->floor_height) {
                    api->velocity.y = 0.0f;
                    api->position.y = api->floor_height - grounded_lower_bound;
                }

                api->field_281 = 0;
                if (GameObjectNearFloor(object, 1.0f, NULL) != 0) {
                    api->field_281 |= 2;
                }
                if (api->collision_min.y <= api->floor_height) {
                    api->field_281 |= 1;
                }
                if ((api->field_281 & 1) != 0) {
                    object->contact_count = 1;
                    object->contact_position.x = api->position.x;
                    object->contact_position.y = api->floor_height;
                    object->contact_position.z = api->position.z;
                    object->contact_material[0] = api->jump_animation_state;
                    object->contact_normal = shadow_grounding ? object->surface_normal : v010;
                    object->terrain_impact_speed = -api->velocity.y;
                } else {
                    object->contact_material_word = 0;
                }
            }
        } else {
            f32 floor_height;
            i32 stopped_without_terrain;

            if (skip_motion) {
                api->velocity.x = 0.0f;
                api->velocity.y = 0.0f;
                api->velocity.z = 0.0f;
            } else {
                NUVEC incoming_velocity;
                NUVEC collision_position;
                NUVEC movement;
                f32 movement_scale;
                i32 object_index;
                i32 excluded_platforms[32];
                i32 excluded_count;
                TERRAIN_PLAYER_PLATFORM_SYS_PC *platforms;
                i32 i;

                incoming_velocity = api->velocity;
                collision_position = api->position;
                collision_position.y += lower_bound;

                NuVecScale(&movement, &api->velocity, FRAMETIME);

                object->contact_material_word = 0;

                if (object->use_model_origin != 0 && api->field_281 != 0 &&
                    (0.0f != movement.x || movement.z != 0.0f) &&
                    !(ENDORBATTLEC_LDATA == WORLD->current_level && object->id == GameObjectRotationAtstId)) {
                    i32 deflection_mode;

                    deflection_mode = 0;
                    if (VehicleArea != 0 && (api->character_data->model_flags & 0x2000) != 0 &&
                        api->character_data->game_character->field_028 != 0.0f) {
                        deflection_mode = 2;
                    } else if (object->character_context == -1 && object->pad->input_magnitude > 0.0f) {
                        deflection_mode = 1;
                    }
                    if (deflection_mode != 0) {
                        NUVEC deflected;

                        Surface_Deflect(&object->surface_normal, &movement, &deflected, deflection_mode);
                        if (deflected.y > movement.y) {
                            movement = deflected;
                            if (deflection_mode == 1) {
                                object->flags_e20 |= 8;
                            }
                        }
                    }
                }
                TerrainSetPlatConnectTol(
                    (api->character_data->model_flags & 0x800) != 0 ? 0.0f : TerrainPlayerPlatConnectTolerancePc);
                if ((object->ai_packet.flags & 0x80) != 0) {
                    IgnoreWallSplines = 1;
                }
                TerrainSetImpactData(impact_records, &impact_count, 8);
                movement_scale = 1.0f;
                if ((object->flags_ef0 & 0x400) != 0) {
                    PLAYERCHARACTERCONFIG_PC *character;

                    character = api->character_data->game_character;
                    if (object->id != id_SPEEDERBIKE) {
                        movement_scale = TerrainPlayerOtherMovementScalePc / character->run_speed;
                    } else {
                        if (object->ai_packet.goal_speed_mode == 1) {
                            movement_scale = TerrainPlayerBikeMovementScalePc / character->walk_speed;
                        } else if (object->ai_packet.goal_speed_mode == 2) {
                            movement_scale = TerrainPlayerBikeMovementScalePc / character->tiptoe_speed;
                        } else {
                            movement_scale = TerrainPlayerBikeMovementScalePc / character->run_speed;
                        }
                    }
                    movement.x *= movement_scale;
                    movement.z *= movement_scale;
                }
                object_index = ((u32)(((u32)(object)) - ((u32)(Obj)))) / sizeof(GAMEOBJECT);

                excluded_count = 0;
                if (object->field_106e != -1) {
                    excluded_platforms[excluded_count++] = object->field_106e;
                }
                if (object->level_object_index != -1) {
                    excluded_platforms[excluded_count++] = object->level_object_index;
                }
                if (object->character_context == 0) {
                    GAMEOBJECT *target;

                    target = object->airborne_collision_target;
                    if (target != NULL && target->level_object_index != -1) {
                        excluded_platforms[excluded_count++] = target->level_object_index;
                    }
                } else if (object->character_context == 0x58 && object->field_7b3 == 1) {
                    TERRAIN_PLAYER_BLOWUP_PC *blowup;

                    blowup = ((TERRAIN_PLAYER_BLOWUP_PC *)(object->grapple));
                    if (blowup->platform_id != -1) {
                        excluded_platforms[excluded_count++] = blowup->platform_id;
                    }
                }
                platforms = ((TERRAIN_PLAYER_PLATFORM_SYS_PC *)(WORLD->shadow_platform_list_pc));
                if (platforms != NULL && VehicleArea == 0) {
                    i32 player;

                    player = ((api->flags_1fc_word & 0x80) != 0);
                    if (!player || object->character_context == 0x3d || object->takeover_source != NULL ||
                        object->character_context == 0x3b ||
                        (api->character_data->game_character->flags_090 & 0x8040) != 0) {
                        i32 i;

                        for (i = 0; platforms->count > i; ++i) {
                            GAMEOBJECT *platform_object;

                            platform_object = platforms->entries[i].object;
                            if (platform_object != NULL && ((object->flags_ef4 & 0x80000) == 0 || player)) {
                                excluded_platforms[excluded_count++] = platform_object->level_object_index;
                            }
                        }
                    }
                }
                for (i = 0; i < excluded_count; ++i) {
                    PlatOnOff(excluded_platforms[i], 0);
                }

                NewTerrainScaleYMask(&collision_position, &movement, ((u8 *)(&object->field_1050)), object_index,
                                     entry_pad->input_magnitude > 0.0f ? 0.0f : movement_threshold * FRAMETIME,
                                     api->collision_radius, object->collision_y_scale, 0, 0, terrain_mask);
                IgnoreWallSplines = 0;
                for (i = 0; i < excluded_count; ++i) {
                    PlatOnOff(excluded_platforms[i], 1);
                }
                if ((object->flags_ef0 & 0x400) != 0) {
                    movement.x /= movement_scale;
                    movement.z /= movement_scale;
                }

                api->supporting_platform_id = ((i16)(NewShadowOnPlatform()));

                object->contact_count = ((u8)(TerrImpact));
                if (object->contact_count != 0 && impact_count > 0) {

                    memcpy(&object->contact_position, &impact_records[impact_count - 1],
                           sizeof(TERRAIN_PLAYER_IMPACT_PC));
                    object->terrain_impact_speed = -NuVecDot(&object->contact_normal, &incoming_velocity);
                } else {
                    object->contact_count = 0;
                }
                NuVecScale(&api->velocity, &movement, 1.0f / FRAMETIME);
                api->position.x = collision_position.x;
                api->position.z = collision_position.z;
                api->position.y = collision_position.y - object->character_bottom * api->origin_scale;

                if (api->dead == 0 && api->supporting_platform_id != -1) {
                    NUMTX *previous;
                    NUMTX *current;

                    previous = NULL;
                    current = NULL;
                    TerrainPlatGetMtx(api->supporting_platform_id, &previous, &current);
                    if (previous != NULL && current != NULL) {

                        NUVEC previous_forward, current_forward;
                        NUANG previous_heading;
                        NUANG current_heading;
                        NUANG rotation;

                        forward.x = 0.0f;
                        forward.y = 0.0f;
                        forward.z = 1.0f;

                        NuVecMtxRotate(&previous_forward, &forward, previous);
                        previous_heading = NuAtan2D(previous_forward.x, previous_forward.z);
                        NuVecMtxRotate(&current_forward, &forward, current);
                        current_heading = NuAtan2D(current_forward.x, current_forward.z);
                        rotation = NuAngSub(current_heading, previous_heading);
                        if (rotation != 0) {
                            api->field_0x276 = NuAngAdd(rotation, api->field_0x276);
                            api->facing_angle = NuAngAdd(rotation, api->facing_angle);
                            api->movement_facing_angle = NuAngAdd(rotation, api->movement_facing_angle);
                        }
                    }
                }
                if (api->dead == 0) {
                    i32 wall_impact;

                    wall_impact = 0;
                    if (object->contact_count != 0 && 0 == (api->flags_1fc_word & 0x84) &&
                        object->character_context != 0 && object->character_context != 0x43 &&
                        object->character_context != 0x45 && (object->flags_ef4 & 0x10000) == 0) {
                        i32 i;

                        for (i = 0; i < impact_count; ++i) {
                            if (impact_records[i].normal.y < 0.707f) {
                                wall_impact = 1;
                                break;
                            }
                        }
                    }
                    if (wall_impact) {
                        api->respawn_timer += FRAMETIME;
                    } else {
                        api->respawn_timer -= FRAMETIME;
                        if (api->respawn_timer < 0.0f) {
                            api->respawn_timer = 0.0f;
                        }
                    }
                }
            }
            floor_height = (api->flags_1fc_word & 4) != 0
                               ? api->floor_height
                               : GameShadow(object, &api->position, 5.0f, terrain_mask | 0x1f);
            stopped_without_terrain = 0;
            if (floor_height != 2000000.0f) {
                f32 bottom_offset;
                f32 top;
                f32 bottom;
                f32 height;

                object->flags_e20 |= 2;
                bottom_offset = api->origin_scale * object->character_bottom;
                top = object->character_top * api->origin_scale + api->position.y;
                bottom = api->position.y + bottom_offset;
                height = top - bottom;
                if (!skip_motion && floor_height > 0.1f * height + bottom &&
                    height + 0.01f >= (floor_height + height) - top) {
                    api->position.y = floor_height - bottom_offset;
                }
            } else {
                object->flags_e20 &= ~2u;
                if (NOTERRAINSTOP != 0) {
                    api->velocity.x = 0.0f;
                    api->velocity.z = 0.0f;
                    api->position.x = api->start_position.x;
                    api->position.z = api->start_position.z;
                    stopped_without_terrain = 1;
                }
            }
            if (!stopped_without_terrain && (api->flags_1fc_word & 4) == 0) {
                api->floor_height = floor_height;
                GetSurfaceInfo(object, floor_height != 2000000.0f ? 1 : 0, floor_height);
                if (api->supporting_platform_id == -1 && api->field_281 != 0 && object->terrain_platform_id != -1) {
                    api->supporting_platform_id = object->terrain_platform_id;
                }
                if ((0x8000 & api->character_data->game_character->flags_090) == 0 &&
                    object->reflection_height == 2000000.0f && object->terrain_platform_id != -1) {
                    f32 reflection_height;

                    reflection_height = FindReflectionNoPlatforms(&api->position);
                    if (reflection_height != 2000000.0f) {
                        object->reflection_height = reflection_height;
                        object->reflection_enabled = 2;
                    }
                }
            }

            api->field_281 = object->field_1050 != 0 ? 1 : 0;
            if (GameObjectNearFloor(object, 1.0f, NULL) != 0) {
                api->field_281 |= 2;
            }
            if (api->field_281 != 0) {
                object->flags_ef0 |= 0x2000000;
            }
        }

        landed_platform = api->field_281 != 0 && entry_platform == -1 && api->supporting_platform_id != -1 &&
                          api->supporting_platform_id == object->terrain_platform_id && entry_vertical_velocity < 0.0f;
        left_platform = api->field_281 == 0 && entry_platform != -1 && api->supporting_platform_id == -1 &&
                        object->terrain_platform_id == entry_platform;
        if (left_platform || landed_platform) {
            i32 platform_id;
            TERRAIN_PLAYER_GRABBER_PC *grabber;

            platform_id = left_platform ? entry_platform : api->supporting_platform_id;
            grabber = ((TERRAIN_PLAYER_WORLD_PC *)(WORLD))->grabber;
            if (grabber != NULL && grabber->platform_id == platform_id) {
                if (grabber->platform_contact_timer <= 0.0f) {
                    grabber->platform_contact_timer = 0.25f;
                }
            } else {
                GAMEOBJECT *platform_owner;

                platform_owner = CharPlatform_FindObjFromPlatID(
                    ((TERRAIN_PLAYER_PLATFORM_SYS_PC *)(WORLD->shadow_platform_list_pc)), platform_id);
                if (platform_owner != NULL && platform_owner->character_data->game_character->field_028 > 0.0f) {
                    if (left_platform) {
                        platform_owner->velocity.y -= entry_vertical_velocity * 0.5f;
                    } else {
                        platform_owner->velocity.y = entry_vertical_velocity * 0.5f + platform_owner->velocity.y;
                    }
                }
            }
        }

        if (VehicleArea == 0 && ((api->flags_1fc_word & 0x80) != 0) && api->supporting_platform_id != -1) {
            GAMEOBJECT *platform_owner;

            platform_owner = CharPlatform_FindObjFromPlatID(
                ((TERRAIN_PLAYER_PLATFORM_SYS_PC *)(WORLD->shadow_platform_list_pc)), api->supporting_platform_id);
            if (platform_owner != NULL) {
                const PLAYERCHARACTERCONFIG_PC *data;

                data = platform_owner->character_data->game_character;
                if ((data->flags_090 & 0x1000) != 0 && api->position.y < platform_owner->position.y &&
                    platform_owner->horizontal_velocity_magnitude > 0.5f * data->run_speed) {
                    StartFlatten(platform_owner, object);
                }
            }
        }

        if (api->dead != 0) {
            if (api->field_281 != 0) {
                api->velocity.y = 0.0f;
                api->flags_1fc_word |= 4;
            }
            if ((api->flags_1fc_word & 4) == 0) {
                ApplyGravity(object, NULL, 0.0f, 0.0f, NULL);
            }
        } else {
            i32 check_terrain_hazards;
            u32 held_buttons;
            u32 pressed_buttons;

            check_terrain_hazards = object->character_context != 0x2b && (api->flags_1fc_word & 4) == 0 &&
                                    gone_through_door_to_new_level == 0;

            if (object->character_context == 0x2b &&
                (api->field_281 != 0 || api->model_draw_result == 0 || object->turn_braking >= 5.0f ||
                 (object->wall_run_state == 3 && object->contact_count != 0))) {
                if (api->model_draw_result != 0) {
                    InstantKillParts(object, 0, 0.0f);
                }
                KillPlayer(object, 3, 1, NULL);
                if (WORLD->current_level == CRUISERD_LDATA && object->wall_run_state == 3) {
                    MiscTime = 0.1f;
                }
            }
            if (check_terrain_hazards && CannotKill(object) == 0 && api->dead == 0 &&
                (VehicleArea == 0 || ((api->flags_1fc_word & 0x80) != 0)) && (object->flags_ef0 & 0x40000) != 0 &&
                (object->portal_flags & 0x40000) == 0 && object->character_context != 0x5d) {
                if (object->contact_count != 0) {
                    i32 count;
                    i32 i;

                    count = impact_count == -1 ? 1 : impact_count;
                    for (i = 0; i < count; ++i) {
                        i32 surface;
                        NUVEC *normal;

                        normal = impact_count == -1 ? &object->contact_normal : &impact_records[i].normal;
                        surface =
                            ((i8)(impact_count == -1 ? object->contact_material_word : impact_records[i].material[0]));
                        if (((u32)(surface)) > 31) {
                            continue;
                        }
                        if (WORLD->current_level == BONUS_GUNSHIPA_LDATA ||
                            (PODRACE_ADATA != NULL && WORLD->area == PODRACE_ADATA && GameTimer.time_elapsed >= 1.0f)) {
                            NewRumble(object->pad->bound_pad, (((f32)(qrand())) * (1.0f / 65535.0f)) * 0.5f, 0);
                            if (api->model_draw_result == 0 && object->character_context != 0x23) {
                                if (((api->flags_1fc_word & 0x80) != 0)) {
                                    LoseCoins(object, 2);
                                }
                                KillPlayer(object, 2, 1, NULL);
                                break;
                            }
                        } else if (((api->character_data->model_flags & 0x2000) != 0 ||
                                    (api->character_data->game_character->flags_090 & 0x04000000) != 0) &&
                                   api->field_283 == 6) {
                            break;
                        }
                        if ((WORLD->area == NULL || WORLD->area != PODSPRINT_ADATA) &&
                            TerrainKillPlayer(object, surface, normal) != 0) {
                            if (WORLD->current_level == ASTEROIDCHASEC_LDATA) {
                                GameCam_Blend(GameCam, 1.0f, 0.0f, 1);
                            }
                            break;
                        }
                        if ((TerSurface[surface].flags & 0x80) != 0 && WORLD->current_level != BONUS_GUNSHIPA_LDATA) {
                            NewRumble(object->pad->bound_pad, 0.6f, 0);
                            NewBuzz(object->pad->bound_pad, 0.1f, 0);
                            if (object->origin_scale_override != 1.0f && CannotKill(object) == 0) {
                                ObjHitObj(NULL, object, 1, 0, 0, 1);
                            }
                            break;
                        }
                    }
                } else if (api->field_281 != 0 && (WORLD->area == NULL || WORLD->area != PODSPRINT_ADATA)) {
                    i32 surface;

                    surface = ((i8)(api->jump_animation_state));
                    if (((u32)(surface)) <= 31) {
                        TerrainKillPlayer(object, surface, &object->surface_normal);
                    }
                }
            }
            if (check_terrain_hazards && CannotKill(object) == 0 && (object->portal_flags & 0x40000) == 0 &&
                api->dead == 0 && api->water_height != 2000000.0f) {
                i32 layer;
                i32 tractor_swamp;

                layer = ((i8)(api->field_283));
                tractor_swamp =
                    layer == 9 && object->id == id_TRACTOR && WORLD->area != NULL && WORLD->area == DAGOBAH_ADATA;
                if ((layer == 3 || tractor_swamp || (TerLayer[layer].flags & 0x20) != 0 ||
                     ((api->character_data->model_flags & 0x2000) == 0 &&
                      (api->character_data->game_character->flags_090 & 0x04000000) == 0)) &&
                    NoLayerKill(object) == 0 && object->character_context != 0x5d) {
                    u32 layer_flags;
                    i32 exempt_context;
                    i32 rescue_below;
                    i32 rescue_above;

                    layer_flags = TerLayer[((i8)(api->field_283))].flags;
                    exempt_context = !((api->flags_1fc_word & 0x80) != 0) &&
                                     (object->character_context == 0x46 || object->character_context == 0x47);
                    rescue_below =
                        ((layer_flags & 1) != 0 || tractor_swamp) && api->position.y < api->water_height &&
                        !(VehicleArea != 0 && BonusArea != 0 && api->character_data->game_character->field_028 > 0.0f);
                    rescue_above = (layer_flags & 0x20) != 0 && api->collision_max.y > api->water_height;
                    if ((rescue_below || rescue_above) && !exempt_context) {
                        if (object->doomed_escape_locator != NULL && !((api->flags_1fc_word & 0x80) != 0)) {
                            if ((object->flags_ef0 & 0x800) != 0) {
                                ObjHitObj(NULL, object, 1, 0, 0, 1);
                            }
                            if (api->dead == 0) {
                                StartBigJump(object, &object->doomed_escape_locator->position, 0, 1.0f, 1.0f, 0);
                            }
                        } else {
                            i32 standing_surface;
                            i32 saved_rescue_surface;

                            standing_surface = (i8)api->jump_animation_state;
                            saved_rescue_surface = StartupGameFlag1 == 0 && StartupGameFlag0 == 0 &&
                                                   api->field_283 == 6 && (u32)standing_surface < 32 &&
                                                   (TerSurface[standing_surface].flags & 1) != 0;
                            if (StartupGameFlag1 == 0 && StartupGameFlag0 == 0 && api->field_283 == 6 &&
                                (u32)standing_surface < 32 && (TerSurface[standing_surface].flags & 1) == 0) {
                                if (object->vehicle_hit_timer <= 0.0f)
                                    ObjHitObj(NULL, object, 1, 0, 0, 1);
                            } else {
                                ClearLastSafeTakeOver(object);
                                Player_ClearContext(object, 1);
                                object->field_7aa = 5;
                                if (object->character_context != -1 &&
                                    api->character_model->model_data_b[api->anim_packet.requested_animation] != NULL) {
                                    object->field_7aa = api->anim_packet.requested_animation;
                                }
                                object->character_context = 0x2b;
                                object->jump_variant_state = 0;
                                if ((TerLayer[((i8)(api->field_283))].flags & 0x20) != 0) {
                                    object->wall_run_state = 3;
                                } else if (saved_rescue_surface || api->field_283 == 6) {
                                    api->velocity.y = -0.25f;
                                    object->wall_run_state = 2;
                                    NewBuzz(object->pad->bound_pad, 0.1f, 0);
                                } else if (WORLD->current_level == VADERC_LDATA) {
                                    api->velocity.y = -0.5f;
                                    object->wall_run_state = 4;
                                    NewBuzz(object->pad->bound_pad, 0.1f, 0);
                                } else {
                                    object->wall_run_state = 1;
                                }
                                object->turn_braking = 0.0f;
                                PlayDieSfx(object);
                                object->current_hp = 0;
                                if (((api->flags_1fc_word & 0x80) != 0) &&
                                    ((TERRAIN_PLAYER_COINS_PC *)(object->coin_packet)) != NULL &&
                                    ((TERRAIN_PLAYER_COINS_PC *)(object->coin_packet))->coins != 0 &&
                                    BonusWinner == -1) {
                                    i32 coins;

                                    coins = LoseCoins(object, 2);
                                    if (coins > 0) {
                                        f32 height;

                                        height = api->water_height;
                                        if (height != 2000000.0f && (TerLayer[((i8)(api->field_283))].flags & 1) == 0) {
                                            height = 2000000.0f;
                                        }
                                        AddPickups(coins, 0, 0, 0, &api->collision_position, NULL, 2.0f,
                                                   ((i8)(api->field_280)), 1.0f, height, NULL, 1, 0);
                                    }
                                    ((TERRAIN_PLAYER_COINS_PC *)(object->coin_packet))->scale = 1.5f;
                                }
                            }
                        }
                    } else if ((api->character_data->model_flags & 0x40) == 0 && object->id != id_SNAKE &&
                               api->field_283 == 9 && api->water_height > api->collision_max.y) {
                        if (BonusWinner == -1) {
                            i32 coins;

                            coins = LoseCoins(object, 2);
                            if (coins > 0) {
                                AddPickups(coins, 0, 0, 0, &api->upper_position, NULL, 2.0f, ((i8)(api->field_280)),
                                           1.0f, 2000000.0f, NULL, 1, 0);
                            }
                        }
                        InstantKillParts(object, 1, 2.0f);
                        KillPlayer(object, 3, 1, NULL);
                    }
                }
            }

            if (check_terrain_hazards) {
                api->is_underwater = ((u8)(UnderWater(object)));
                api->water_intersects = ((u8)(IntersectWater(object)));
            }

            if (api->water_intersects != 0 && (entry_underwater | entry_intersects_water) == 0) {
                i32 splash;

                splash = object->character_context == 0x1f || object->character_context == 0 ||
                         (CInfo[object->character_context].flags & 4) != 0 || object->character_context == 0x0e ||
                         object->character_context == 0x0d;
                if (!splash) {
                    i32 animation;

                    animation = CurrentAnim(&api->anim_packet);
                    splash = animation == 0x28 || animation == 5 || ((u32)(animation - 0x4b)) <= 1;
                }
                if (splash) {
                    AddWaterSplash(object, &api->collision_position);
                }
            }
            if (api->model_draw_result != 0 && object->character_context != 0x40 && object->character_context != 0x2c) {
                extern void GameObjectRotation(GAMEOBJECT *, i32);
                GameObjectRotation(object, 2);
            }

            held_buttons = object->pad->buttons_held;
            pressed_buttons = object->pad->buttons_pressed;
            if ((api->flags_1fc_word & 0x180) == 0x80 && (entry_menu_id != -1 || MiniCutCam == 2)) {
                object->pad->buttons_held = 0;
                object->pad->buttons_pressed = 0;
            }
            Tag_Check(object);
            PreResetCode(object);
            ((TERRAIN_PLAYER_CHARACTER_PC *)(api->character_data))->move_callback(object);
            PostResetCode(object);
            if ((api->character_data->model_flags & 0x00200000) == 0) {
                BigJumpCode(object);
            }
            object->pad->buttons_held = held_buttons;
            object->pad->buttons_pressed = pressed_buttons;
        }

        if ((object->portal_flags & 0x40000) == 0 && ((TERRAIN_PLAYER_SET_PC *)(CurrentTerrain)) != NULL &&
            api->dead == 0 && (WORLD->current_level->flags & 0xe0) == 0 &&
            (((((TERRAIN_PLAYER_SET_PC *)(CurrentTerrain))->minimum_height))) - 5.0f > api->position.y) {
            if (api->model_draw_result != 0) {
                InstantKillParts(object, 0, 0.0f);
            }
            KillPlayer(object, 3, 1, NULL);
        }
    }

    entry_pad->runtime_flags &= ~0x16u;
    object->field_d20 = 0;
    entry_pad->operator_data = NULL;
    NuVecRotateYValZ(&object->facing_direction, 1.0f, object->field_0x276);
}

typedef struct PUSH_SUPPORT_BLOCK_VIEW_PC {
    u8 unknown_000[0x24];
    NUSPECIALHANDLE special;
    GAMEOBJECT *pushing_object;
    u8 unknown_034[0x58 - 0x34];
    NUVEC *position;
    u8 unknown_05c[0xb0 - 0x5c];
    NUVEC bounds_min, bounds_max;
    u32 packed_state_flags;
    u8 unknown_0cc[4];
    u8 completed_snap_positions;
    u8 end_position_count;
    u8 output_count;
    u8 unknown_0d3;
} PUSH_SUPPORT_BLOCK_VIEW_PC;
typedef struct PUSH_SUPPORT_WORLD_VIEW_PC {
    u8 unknown_000[0x46bc];
    PUSH_SUPPORT_BLOCK_VIEW_PC *blocks;
    i32 count;
} PUSH_SUPPORT_WORLD_VIEW_PC;
STATIC_ASSERT(sizeof(PUSH_SUPPORT_BLOCK_VIEW_PC) == 0xd4);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, special) == 0x24);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, pushing_object) == 0x30);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, position) == 0x58);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, bounds_min) == 0xb0);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, bounds_max) == 0xbc);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, packed_state_flags) == 0xc8);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, completed_snap_positions) == 0xd0);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, end_position_count) == 0xd1);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_BLOCK_VIEW_PC, output_count) == 0xd2);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_WORLD_VIEW_PC, blocks) == 0x46bc);
STATIC_ASSERT(offsetof(PUSH_SUPPORT_WORLD_VIEW_PC, count) == 0x46c0);

i32 __cdecl TerrainBlockOnBlock(WORLDINFO *opaque_world, PUSH_BLOCK_PC *block, NUVEC *points, f32 *heights) {
    PUSH_SUPPORT_WORLD_VIEW_PC *world = (PUSH_SUPPORT_WORLD_VIEW_PC *)opaque_world;
    i32 i;
    if (points == NULL || heights == NULL)
        return 0;
    heights[0] = -10000.0f;
    heights[1] = -10000.0f;
    heights[2] = -10000.0f;
    heights[3] = -10000.0f;
    for (i = 0; i < world->count; ++i) {
        PUSH_SUPPORT_BLOCK_VIEW_PC *other = &world->blocks[i];
        f32 xmin, xmax, zmin, zmax, height;
        i32 j;
        if ((PUSH_BLOCK_PC *)other == block)
            continue;
        xmin = other->position->x - (f32)fabs((f64)other->bounds_min.x);
        zmin = other->position->z - (f32)fabs((f64)other->bounds_min.z);
        xmax = other->position->x + (f32)fabs((f64)other->bounds_max.x);
        height = other->position->y + (f32)fabs((f64)other->bounds_max.y);
        zmax = other->position->z + (f32)fabs((f64)other->bounds_max.z);
        for (j = 0; j < 4; ++j) {
            if (points[j].x > xmin && points[j].x < xmax && points[j].z > zmin && points[j].z < zmax)
                heights[j] = height;
        }
    }
    return 1;
}
