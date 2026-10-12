#include "saga.h"
typedef struct lose_coins_packet_pc_s {
    u32 coins;
} LOSE_COINS_PACKET_PC;
STATIC_ASSERT(offsetof(LOSE_COINS_PACKET_PC, coins) == 0);
STATIC_ASSERT(offsetof(GAMEOBJECT, coin_packet) == 0x7fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, game_character) == 0x24);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);
i32 __cdecl LoseCoins(GAMEOBJECT *object, i32 cause) {
    LOSE_COINS_PACKET_PC *packet;
    u32 lost = 0;
    i32 adjustment;
    if (Player_HasInvincibility(object) || (object->character_data->game_character->flags_090 & 0x8000) ||
        object->coin_packet == NULL)
        return 0;
    if (cause == 1)
        lost = BonusArea != 0 ? 25000 : 2000;
    else if (cause == 2)
        lost = BonusArea != 0 ? 12500 : 1000;
    if (Cheat_IsOn(0x2c)) {
        adjustment = adtab[adaptivedifficulty.level][0];
        if (adjustment == 1)
            lost += lost;
        else if (adjustment == -1)
            lost >>= 1;
    }
    packet = (LOSE_COINS_PACKET_PC *)object->coin_packet;
    if (lost > packet->coins) {
        lost = packet->coins;
        packet->coins = 0;
    } else if (lost > 0)
        packet->coins -= lost;
    if (Cheats_CheckFlags(0x7c) || DoubleScoreTime > 0.0f)
        return 0;
    return (i32)lost;
}

void __cdecl AddCoinsToPanel(i32 coins, NUVEC *position, i32 player, f32 unused, GAMEOBJECT *owner, i32 random_types) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 remainder, remaining, type, score, i, j, base_model, model, main_total;
    u8 counts[10] = {0};
    NUVEC target;
    f32 target_scale;
    GIZMO_PICKUP_TYPE_PC *pickup;
    ADD_GAME_MESSAGE_PC message;
    (void)unused;
    (void)owner;
    if (coins == 0)
        return;
    if (ChallengeMode != 0 || Mission_Active(NULL) != NULL)
        coins = 0;
    remainder = coins % 10;
    if (remainder > 0)
        coins -= remainder;
    if (coins > 512000)
        coins = 512000;
    if (coins > 0)
        PlaySfx((char *)PickupCoinsLandName, position);
    remaining = coins;
    if (random_types)
        while (remaining > 0) {
            type = CoinTab[qrand() / 16384];
            score = GizmoPickupType[type].score;
            if (remaining - score < 0)
                continue;
            ++counts[type];
            remaining -= score;
        }
    else if (remaining > 0)
        for (i = 3; remaining > 0; --i) {
            type = CoinTab[i];
            score = GizmoPickupType[type].score;
            while (remaining - score >= 0) {
                remaining -= score;
                ++counts[type];
            }
        }
    if (player != 0 && player != 1)
        player = -1;
    target.x = player == 1 ? PANEL_COINX : -PANEL_COINX;
    main_total = CoinsGoToMainTotal() != 0;
    if (main_total) {
        target.y = STATSPOSY;
        target_scale = COINTOTAL_COINSIZE;
    } else {
        target.y = STATSPOSY + PANEL_COINY;
        target_scale = PANEL_COINSCALE_END;
    }
    target.z = 1.0f;
    DrawBuildUpTime = (f32)(COINMSGTIME + 1.0);
    for (i = 0; i < 4; ++i) {
        pickup = &GizmoPickupType[CoinTab[i]];
        base_model = pickup->first_model_id;
        for (j = 0; j < counts[CoinTab[i]]; ++j) {
            model = base_model;
            if (pickup->random_model_count > 0)
                model += qrand() / (65535 / pickup->random_model_count + 1);
            if (world->level_objects[model].active == 0 || player == -1)
                continue;
            message = AddGameMsg_Default;
            message.position = position;
            message.target_position = &target;
            message.target_scale = target_scale;
            message.icon = (i16)model;
            message.special = &world->level_objects[message.icon];
            message.score = pickup->score;
            message.end_fn = EndScoreMessage;
            message.player_index = (i8)player;
            message.allow_temporary = 1;
            message.delay = AddCoinDelay[player];
            message.flags = 0x12d;
            message.scale = 1.0f;
            message.duration = COINMSGTIME;
            if (main_total) {
                message.update_fn = GameMsg_DrawAdjustNewPos_CoinToTotal;
                message.target_is_coin_total = 1;
            }
            AddGameMsg(&message);
            AddCoinDelay[player] = (f32)(AddCoinDelay[player] + 0.10000000149011612);
        }
    }
}

void __cdecl AddPickups(i32 coins, i32 hearts, i32 torpedoes, i32 powerups, NUVEC *position, NUVEC *direction,
                        f32 unused, i32 player_id, f32 speed, f32 lifetime, GAMEOBJECT *owner, i32 random_types,
                        i32 panel) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    const f32 scale = AreaPickupScale;
    u32 arcade_flags;
    i32 to_panel, remainder, remaining, i, j, type, score, count, both_players;
    u8 counts[10] = {0};
    NUVEC velocity;
    GAMEOBJECT *recipient;
    GAMEOBJECT *eligible[2];
    (void)unused;
    Arcade_GetMode(&arcade_flags);
    if (arcade_flags & 0x10)
        coins = torpedoes = 0;
    if (world->area != NULL && (world->area->flags & 0x104) == 4)
        random_types = 0;
    if (world->current_level->flags & 0x80000) {
        to_panel = 1;
        if (player_id == -1) {
            if (player2 != NULL)
                player_id = qrand() / 32768;
            else if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc_word & 0x80))
                player_id = 0;
            else
                player_id = 1;
        }
    } else if (world->area != NULL && (world->area->flags & 0x100)) {
        player_id = qrand() / 32768;
        to_panel = 1;
    } else
        to_panel = player_id == -1 ? 0 : panel;
    if (ChallengeMode != 0 || Mission_Active(NULL) != NULL)
        coins = 0;
    if (speed == 1.0f)
        speed *= scale;
    if (netclient != 0)
        return;
    remainder = coins % 10;
    if (remainder > 0)
        coins -= remainder;
    if (coins > 512000)
        coins = 512000;
    if (coins > 0)
        PlaySfx((char *)PickupCoinsLandName, position);
    remaining = coins;
    if (random_types) {
        while (remaining > 0) {
            type = CoinTab[qrand() / 16384];
            score = GizmoPickupType[type].score;
            if (remaining - score < 0)
                continue;
            ++counts[type];
            remaining -= score;
        }
    } else if (remaining > 0) {
        for (i = 3; remaining > 0; --i) {
            type = CoinTab[i];
            score = GizmoPickupType[type].score;
            while (remaining - score >= 0) {
                remaining -= score;
                ++counts[type];
            }
        }
    }
    if (player_id != 0 && player_id != 1)
        player_id = -1;
    if (to_panel) {
        if (coins > 0)
            DrawBuildUpTime = (f32)(COINMSGTIME + 1.0);
        AddCoinsToPanel(coins, position, player_id, speed, owner, random_types);
    } else {
        for (i = 0; i < 4; ++i) {
            type = CoinTab[i];
            for (j = 0; j < counts[type]; ++j) {
                MakePartVector(&velocity, direction, speed);
                AddCoinsAsParts(type, position, &velocity, lifetime, scale);
            }
        }
    }
    both_players = 0;
    if (hearts > 0 && VehicleArea != 0 && Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc_word & 0x80) &&
        Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc_word & 0x80)) {
        both_players = 1;
        hearts = 2;
    }
    for (i = 0; i < hearts; ++i) {
        if (to_panel)
            velocity = v000;
        else
            MakePartVector(&velocity, direction, speed);
        recipient = NULL;
        if (both_players)
            recipient = (GAMEOBJECT *)Player[i == 0 ? 0 : 1];
        else if (owner != NULL)
            recipient = owner;
        else {
            count = 0;
            if (Player[0] != NULL && (((GAMEOBJECT *)Player[0])->flags_1fc_word & 0x80))
                eligible[count++] = (GAMEOBJECT *)Player[0];
            if (Player[1] != NULL && (((GAMEOBJECT *)Player[1])->flags_1fc_word & 0x80))
                eligible[count++] = (GAMEOBJECT *)Player[1];
            if (count > 0)
                recipient = eligible[count == 1 ? 0 : qrand() / 32768];
        }
        AddHeartAsPart(recipient, position, &velocity, scale, lifetime);
    }
    for (i = 0; i < torpedoes; ++i) {
        MakePartVector(&velocity, direction, speed);
        AddTorpedoAsPart(position, &velocity, scale, lifetime);
    }
    if (!(world->current_level->flags & 0x80000))
        for (i = 0; i < powerups; ++i) {
            MakePartVector(&velocity, direction, speed);
            PowerUp_AddPart(position, &velocity, scale, lifetime);
        }
}

void AddMiscPickups(NUVEC *position, i32 player_id, i32 coins, i32 torpedoes) {
    if (torpedoes != 0) {
        AddPickups(coins, 0, torpedoes, 0, position, &v010, 5.0f, player_id, 1.0f, 2000000.0f, NULL, 1, 0);
    } else {
        AddPickups(coins, ReleaseHearts(), 0, 0, position, &v010, 5.0f, player_id, 1.0f, 2000000.0f, NULL, 1, 0);
    }
}

void GizPanel_CreateTerrain(GIZPANEL_s *panel) {
    if (panel == NULL) {
        return;
    }
    if (panel->platform_id != -1) {
        DeletePlatinst(panel->platform_id);
    }

    switch (panel->model_variant) {
        case 0:
            panel->platform_id = NewPlatPickupInst(panel, 4);
            break;
        case 1:
            panel->platform_id = NewPlatPickupInst(panel, 7);
            break;
        case 2:
            panel->platform_id = NewPlatPickupInst(panel, 5);
            break;
        case 3:
            panel->platform_id = NewPlatPickupInst(panel, 6);
            break;
        default:
            break;
    }
    PlatInstRotate(panel->platform_id, 1);
}

static void GizPanel_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    struct GIZPANEL_s *panel;
    i32 was_visible;
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }

    panel = ((GIZPANEL_s *)(gizmo->object));
    was_visible = panel->visible;
    panel->visible = visible != 0;
    if (panel->visible != 0) {
        if (was_visible == 0) {
            GizPanel_CreateTerrain(panel);
        }
    } else if (was_visible != 0) {
        DeletePlatinst(panel->platform_id);
    }
}

typedef struct SPECIAL_MINIKIT_PC {
    NUSPECIALHANDLE special;
    NUINSTANIM_TIME_VIEW *instance_animation;
    void *animation_data;
    f32 end_frame;
    char special_name[16];
    char pickup_name[8];
    u8 unknown_30[8];
    u32 flags;
} SPECIAL_MINIKIT_PC;
typedef struct SPECIAL_MINIKIT_SYSTEM_PC {
    SPECIAL_MINIKIT_PC *items;
    i32 count;
} SPECIAL_MINIKIT_SYSTEM_PC;
typedef struct SPECIAL_MINIKIT_WORLD_PC {
    u8 unknown_0[0x5054];
    SPECIAL_MINIKIT_SYSTEM_PC *special_minikits;
} SPECIAL_MINIKIT_WORLD_PC;
STATIC_ASSERT(sizeof(SPECIAL_MINIKIT_PC) == 0x3c);
STATIC_ASSERT(offsetof(SPECIAL_MINIKIT_PC, special) == 0);
STATIC_ASSERT(offsetof(SPECIAL_MINIKIT_PC, instance_animation) == 0xc);
STATIC_ASSERT(offsetof(SPECIAL_MINIKIT_PC, animation_data) == 0x10);
STATIC_ASSERT(offsetof(SPECIAL_MINIKIT_PC, end_frame) == 0x14);
STATIC_ASSERT(offsetof(SPECIAL_MINIKIT_PC, special_name) == 0x18);
STATIC_ASSERT(offsetof(SPECIAL_MINIKIT_PC, pickup_name) == 0x28);
STATIC_ASSERT(offsetof(SPECIAL_MINIKIT_PC, flags) == 0x38);
STATIC_ASSERT(sizeof(SPECIAL_MINIKIT_SYSTEM_PC) == 8);
STATIC_ASSERT(offsetof(SPECIAL_MINIKIT_SYSTEM_PC, items) == 0);
STATIC_ASSERT(offsetof(SPECIAL_MINIKIT_SYSTEM_PC, count) == 4);
STATIC_ASSERT(offsetof(SPECIAL_MINIKIT_WORLD_PC, special_minikits) == 0x5054);
void __cdecl SpecialMiniKits_Configure(WORLDINFO *world, char *config) {
    SPECIAL_MINIKIT_WORLD_PC *view = (SPECIAL_MINIKIT_WORLD_PC *)world;
    SPECIAL_MINIKIT_PC *items, *cursor;
    SPECIAL_MINIKIT_SYSTEM_PC *system;
    i32 count = 0;
    NUFPAR_PC *parser;
    view->special_minikits = NULL;
    if (world->current_gscn == NULL)
        return;
    parser = NuFParCreateMem("specialminikits", config, 0xffff);
    if (parser == NULL)
        return;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3U) & ~3U;
    items = cursor = (SPECIAL_MINIKIT_PC *)world->giz_buffer.addr;
    while (NuFParGetLine(parser) != 0) {
        if (NuFParGetWord(parser) == 0)
            break;
        if (NuStrICmp(parser->word_buffer_ptr, "specialminikit") != 0)
            continue;
        memset(cursor, 0, sizeof(*cursor));
        cursor->flags |= 1;
        while (NuFParGetWord(parser) != 0) {
            if (NuStrICmp(parser->word_buffer_ptr, "pickup") == 0) {
                if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buffer_ptr) < sizeof(cursor->pickup_name))
                    NuStrCpy(cursor->pickup_name, parser->word_buffer_ptr);
            } else if (NuStrICmp(parser->word_buffer_ptr, "at_object") == 0) {
                if (NuFParGetWord(parser) != 0) {
                    NuSpecialFind(world->current_gscn, &cursor->special, parser->word_buffer_ptr, 1);
                    cursor->flags &= ~0x20U;
                }
            } else if (NuStrICmp(parser->word_buffer_ptr, "at_gizmo") == 0) {
                if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buffer_ptr) < sizeof(cursor->special_name)) {
                    NuStrCpy(cursor->special_name, parser->word_buffer_ptr);
                    cursor->flags |= 0x20;
                }
            } else if (NuStrICmp(parser->word_buffer_ptr, "draw_at_start") == 0) {
                cursor->flags = (cursor->flags & ~1U) | 2;
            } else if (NuStrICmp(parser->word_buffer_ptr, "draw_while_animating") == 0) {
                cursor->flags = (cursor->flags & ~1U) | 4;
            } else if (NuStrICmp(parser->word_buffer_ptr, "draw_at_end") == 0) {
                cursor->flags = (cursor->flags & ~1U) | 8;
            } else if (NuStrICmp(parser->word_buffer_ptr, "orientate") == 0) {
                cursor->flags |= 0x10;
            }
        }
        if (cursor->pickup_name[0] == '\0')
            continue;
        if ((cursor->flags & 0x20) != 0) {
            if (cursor->special_name[0] == '\0')
                continue;
        } else if (NuSpecialExistsFn(&cursor->special) == 0)
            continue;
        if (NuSpecialExistsFn(&cursor->special) != 0) {
            cursor->instance_animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&cursor->special);
            if (cursor->instance_animation != NULL) {
                cursor->animation_data = ((NUGSCN *)cursor->special.scene)
                                             ->instance_animation_data[cursor->instance_animation->animation_index];
                cursor->end_frame = NuAnimEndFrameOld(cursor->animation_data);
            }
        }
        ++count;
        ++cursor;
    }
    NuFParDestroy(parser);
    if (count <= 0)
        return;
    world->giz_buffer.addr = (u32)cursor;
    view->special_minikits = system = (SPECIAL_MINIKIT_SYSTEM_PC *)world->giz_buffer.addr;
    system->items = items;
    system->count = count;
    world->giz_buffer.addr = (world->giz_buffer.addr + sizeof(*system) + 3U) & ~3U;
}

typedef struct PICKUP_COLLISION_RUNTIME_PC {
    GIZMOPICKUP_PC *pickups, *temporary_pickups;
    i32 pickup_count;
} PICKUP_COLLISION_RUNTIME_PC;
STATIC_ASSERT(offsetof(PICKUP_COLLISION_RUNTIME_PC, pickups) == 0);
STATIC_ASSERT(offsetof(PICKUP_COLLISION_RUNTIME_PC, temporary_pickups) == 4);
STATIC_ASSERT(offsetof(PICKUP_COLLISION_RUNTIME_PC, pickup_count) == 8);
typedef struct PICKUP_COLLISION_FLAGS_PC {
    u8 unknown_00[0x15];
    u8 config_flags;
} PICKUP_COLLISION_FLAGS_PC;
STATIC_ASSERT(offsetof(PICKUP_COLLISION_FLAGS_PC, config_flags) == 0x15);
STATIC_ASSERT(offsetof(AREA_GLOBALS_PC, field_18) == 0x18);
static PICKUP_TYPE_SYSTEM_PC *GizmoPickupSys = &GizmoPickupSys_Game;

void __cdecl GizmoPickups_InitSys(PICKUP_TYPE_SYSTEM_PC *system) {
    GizmoPickupSys = system;
}
static GIZMOPICKUP_PC *GizmoPickups_CollideListPc(GAMEOBJECT *object, GIZMOPICKUP_PC *pickups, i32 count) {
    i32 collide_2d, coin_magnet, index;
    f32 collide_scale, scaled_pickup, normal_pickup, scale, radius_x, radius_y, dx, dz, radius;
    GIZMO_PICKUP_TYPE_PC *type;
    GIZMOPICKUP_PC *pickup;
    u8 flags;
    if (pickups == NULL)
        return NULL;
    collide_2d = (object->character_data->model_flags & 0x2000) != 0 && VehicleArea != 0;
    collide_scale = 0.0f;
    if (GizmoPickups_Collide2DFn != NULL) {
        collide_scale = GizmoPickups_Collide2DFn(object);
        if (collide_scale != 0.0f)
            collide_2d = 1;
    }
    coin_magnet = Cheats_CheckFlags(0x8000) != 0 || object->powerup_time > 0.0f;
    normal_pickup = AreaPickupScale;
    scaled_pickup = coin_magnet ? COINMAGNETSCALE * normal_pickup : normal_pickup;
    if (collide_scale != 0.0f)
        scaled_pickup *= collide_scale;
    if (collide_2d && VehicleArea != 0)
        scaled_pickup += scaled_pickup;
    for (index = 0; index < count; ++index) {
        pickup = &pickups[index];
        type = &GizmoPickupSys->types[pickup->type_index];
        flags = pickup->flags;
        if ((type->flags & 0x10) != 0 || (flags & 1) == 0 || (flags & 8) != 0 || (flags & 2) == 0 ||
            ((flags & 0x10) == 0 && (((PICKUP_COLLISION_FLAGS_PC *)pickup)->config_flags & 4) == 0))
            continue;
        if ((flags & 0x40) != 0 && GizmoPickupSys->alternate_type != -1)
            type = &GizmoPickupSys->types[GizmoPickupSys->alternate_type];
        if (type->field_0f != 0)
            continue;
        if ((((PICKUP_COLLISION_FLAGS_PC *)pickup)->config_flags & 4) != 0)
            return pickup;
        scale = type->score == 0 ? normal_pickup : scaled_pickup;
        radius_x = type->shadow_extent_x * scale;
        radius_y = (coin_magnet ? type->shadow_extent_z : type->shadow_radius_z) * scale;
        if ((f64)object->collision_min.x > (f64)pickup->position.x + radius_x ||
            (f64)pickup->position.x - radius_x > (f64)object->collision_max.x ||
            (f64)object->collision_min.z > (f64)pickup->position.z + radius_x ||
            (f64)pickup->position.z - radius_x > (f64)object->collision_max.z)
            continue;
        if (!collide_2d) {
            if ((f64)object->collision_min.y > (f64)pickup->position.y + radius_y ||
                (f64)pickup->position.y - radius_y > (f64)object->collision_max.y)
                continue;
            if (object->rider == NULL) {
                if (SphereSphereOverlapScaleY(&pickup->position, radius_x, radius_y, &object->collision_position,
                                              object->collision_radius, object->bounds_half_height))
                    return pickup;
                continue;
            }
        }
        dx = pickup->position.x - object->collision_position.x;
        dz = pickup->position.z - object->collision_position.z;
        radius = radius_x + object->bounds_radius;
        if (!((f64)dx * dx + (f64)dz * dz >= (f64)radius * radius))
            return pickup;
    }
    return NULL;
}
GIZMOPICKUP_PC *__cdecl GizmoPickups_Collide(WORLDINFO *world, GAMEOBJECT *object, i32 unused) {
    GIZMOPICKUP_PC *pickup;
    i32 temporary, type_index;
    (void)unused;
    if (world == NULL || (object->flags_1f8 & 0x40000) != 0)
        return NULL;
    pickup = GizmoPickups_CollideListPc(
        object, ((PICKUP_COLLISION_RUNTIME_PC *)world->pickup_runtime_sys)->temporary_pickups, 64);
    temporary = pickup != NULL;
    if (pickup == NULL)
        pickup = GizmoPickups_CollideListPc(object, ((PICKUP_COLLISION_RUNTIME_PC *)world->pickup_runtime_sys)->pickups,
                                            ((PICKUP_COLLISION_RUNTIME_PC *)world->pickup_runtime_sys)->pickup_count);
    if (pickup == NULL)
        return NULL;
    pickup->flags |= 8;
    if (temporary)
        pickup->flags &= ~1u;
    type_index = (pickup->flags & 0x40) != 0 && GizmoPickupSys->alternate_type != -1 ? GizmoPickupSys->alternate_type
                                                                                     : pickup->type_index;
    if (GizmoPickupSys->types[type_index].collection_sfx_name != NULL)
        PlaySfx(GizmoPickupSys->types[type_index].collection_sfx_name, &pickup->position);
    if (GizmoPickupSys->types[type_index].collect_fn != NULL)
        GizmoPickupSys->types[type_index].collect_fn(world, pickup, type_index, object, 0);
    if ((GizmoPickupSys->types[type_index].flags & 0x40) != 0)
        ++AreaGlobals_PC.field_18;
    return pickup;
}
