#include "saga.h"

i32 __cdecl GizmoPickup_GetNumOutputs(GIZMO_PC *unused) {
    (void)unused;
    return 1;
}
typedef struct lose_coins_packet_pc_s {
    u32 coins;
} LOSE_COINS_PACKET_PC;
STATIC_ASSERT(offsetof(LOSE_COINS_PACKET_PC, coins) == 0);
STATIC_ASSERT(offsetof(GAMEOBJECT, coin_packet) == 0x7fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_data) == 0x54);
STATIC_ASSERT(offsetof(CHARACTERDATA_PC, game_character) == 0x24);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, flags_090) == 0x90);

typedef struct SPECIAL_MINIKIT_PC {
    NUSPECIALHANDLE special;
    NUINSTANIM *instance_animation;
    void *animation_data;
    f32 end_frame;
    char special_name[16];
    char pickup_name[8];
    GIZMO_PC *at_gizmo;
    GIZMO_PC *pickup;
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
STATIC_ASSERT(offsetof(SPECIAL_MINIKIT_PC, at_gizmo) == 0x30);
STATIC_ASSERT(offsetof(SPECIAL_MINIKIT_PC, pickup) == 0x34);
STATIC_ASSERT(offsetof(SPECIAL_MINIKIT_PC, flags) == 0x38);
STATIC_ASSERT(sizeof(SPECIAL_MINIKIT_SYSTEM_PC) == 8);
STATIC_ASSERT(offsetof(SPECIAL_MINIKIT_SYSTEM_PC, items) == 0);
STATIC_ASSERT(offsetof(SPECIAL_MINIKIT_SYSTEM_PC, count) == 4);
STATIC_ASSERT(offsetof(SPECIAL_MINIKIT_WORLD_PC, special_minikits) == 0x5054);
void __cdecl SpecialMiniKits_Configure(WORLDINFO *world, char *config) {
    SPECIAL_MINIKIT_WORLD_PC *view = (SPECIAL_MINIKIT_WORLD_PC *)world;
    SPECIAL_MINIKIT_PC *items, *cursor;
    SPECIAL_MINIKIT_SYSTEM_PC *system;
    i32 count;
    NUFPAR_PC *parser;
    view->special_minikits = NULL;
    if (world->current_gscn == NULL)
        return;
    parser = NuFParCreateMem("specialminikits", config, 0xffff);
    if (parser == NULL)
        return;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3U) & ~3U;
    items = (SPECIAL_MINIKIT_PC *)world->giz_buffer.addr;
    cursor = items;
    count = 0;
    while (NuFParGetLine(parser) != 0 && NuFParGetWord(parser) != 0) {
        if (NuStrICmp(parser->word_buffer_ptr, "specialminikit") != 0)
            continue;
        memset(cursor, 0, sizeof(*cursor));
        cursor->flags |= 1;
        while (NuFParGetWord(parser) != 0) {
            if (NuStrICmp(parser->word_buffer_ptr, "pickup") == 0) {
                if (NuFParGetWord(parser) != 0 && NuStrLen(parser->word_buffer_ptr) < (i32)sizeof(cursor->pickup_name))
                    NuStrCpy(cursor->pickup_name, parser->word_buffer_ptr);
            } else if (NuStrICmp(parser->word_buffer_ptr, "at_object") == 0) {
                if (NuFParGetWord(parser) != 0) {
                    NuSpecialFind(world->current_gscn, &cursor->special, parser->word_buffer_ptr, 1);
                    cursor->flags &= ~0x20U;
                }
            } else if (NuStrICmp(parser->word_buffer_ptr, "at_gizmo") == 0) {
                if (NuFParGetWord(parser) != 0 &&
                    NuStrLen(parser->word_buffer_ptr) < (i32)sizeof(cursor->special_name)) {
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
        if ((cursor->flags & 0x20) == 0 && NuSpecialExistsFn(&cursor->special) != 0) {
        } else if ((cursor->flags & 0x20) == 0 || cursor->special_name[0] == '\0') {
            continue;
        }
        if (NuSpecialExistsFn(&cursor->special) != 0) {
            cursor->instance_animation = (NUINSTANIM *)NuSpecialGetInstAnim(&cursor->special);
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

STATIC_ASSERT(offsetof(AREA_GLOBALS_PC, field_18) == 0x18);
static PICKUP_TYPE_SYSTEM_PC *GizmoPickupSys = &GizmoPickupSys_Game;

GIZMOPICKUP_PC *__cdecl GizmoPickup_InBox(WORLDINFO *world, i32 type_index, NUVEC *minimum, NUVEC *maximum) {
    GIZMOPICKUPRUNTIMESYS_s *runtime = world->pickup_runtime_sys;
    GIZMOPICKUP_PC *pickup = runtime->pickups;
    GIZMO_PICKUP_TYPE_PC *type;
    f32 radius;
    NUVEC low, high;
    i32 index;
    if (pickup == NULL)
        return NULL;
    type = &GizmoPickupSys->types[type_index];
    if (type->field_0f != 0)
        return NULL;
    radius = type->shadow_extent_x * AreaPickupScale;
    low.x = minimum->x - radius;
    high.x = maximum->x + radius;
    low.y = minimum->y - radius;
    high.y = maximum->y + radius;
    low.z = minimum->z - radius;
    high.z = maximum->z + radius;
    for (index = 0; index < runtime->pickup_count; ++index, ++pickup) {
        if (pickup->type_index != type_index)
            continue;
        if ((pickup->flags & 2) == 0)
            continue;
        if ((pickup->flags & 4) == 0)
            continue;
        if ((pickup->flags & 8) != 0)
            continue;
        if (low.x > pickup->position.x || pickup->position.x > high.x || low.z > pickup->position.z ||
            pickup->position.z > high.z || low.y > pickup->position.y || pickup->position.y > high.y)
            continue;
        return pickup;
    }
    return NULL;
}

void __cdecl SpecialMiniKits_Reset(WORLDINFO *world) {
    SPECIAL_MINIKIT_WORLD_PC *view = (SPECIAL_MINIKIT_WORLD_PC *)world;
    SPECIAL_MINIKIT_PC *item;
    i32 index;
    if (view->special_minikits == NULL || GizmoPickupSys->gizmo_type_id == -1)
        return;
    item = view->special_minikits->items;
    for (index = 0; index < view->special_minikits->count; ++index, ++item) {
        item->pickup = (GIZMO_PC *)GizmoFindByName(world->gizmo_sys, gizmopickup_typeid, item->pickup_name);
        if ((item->flags & 0x20) != 0)
            item->at_gizmo = (GIZMO_PC *)GizmoFindByName(world->gizmo_sys, -1, item->special_name);
    }
}

void __cdecl SpecialMiniKits_Draw(WORLDINFO *world) {
    SPECIAL_MINIKIT_WORLD_PC *view = (SPECIAL_MINIKIT_WORLD_PC *)world;
    SPECIAL_MINIKIT_PC *item;
    GIZMOPICKUP_PC *pickup;
    GIZMO_PICKUP_TYPE_PC *type;
    NUINSTANIM *animation;
    NUVEC scale, *position;
    NUMTX matrix;
    u16 spin, tilt;
    i32 index;
    scale.x = AreaPickupScale;
    scale.y = AreaPickupScale;
    scale.z = AreaPickupScale;
    if (view->special_minikits == NULL || GizmoPickupSys->gizmo_type_id == -1 ||
        GizmoPickupSys->types[GizmoPickupSys->gizmo_type_id].field_0f != 0)
        return;
    spin = (u16)(i32)((f64)NuFmod(GameTimer.time_elapsed, 4.0f) * 0.25 * 65536.0);
    tilt = (u16)(i32)((f64)NuTrigTable[(((i32)spin * 2) >> 1) & 0x7fff] * 1820.0);
    item = view->special_minikits->items;
    for (index = 0; index < view->special_minikits->count; ++index, ++item) {
        if ((item->flags & 0x20) != 0) {
            if (item->at_gizmo == NULL)
                continue;
            if (GizmoGetVisibility(world->gizmo_sys, item->at_gizmo) == 0)
                continue;
            position = GizmoGetPos((GIZMOSYS_PC *)world->gizmo_sys, item->at_gizmo);
            if (position == NULL)
                continue;
            NuMtxSetScale(&matrix, &scale);
            NuMtxTranslate(&matrix, position);
        } else {
            if (NuSpecialGetVisibilityFn(&item->special) == 0)
                continue;
            if ((item->flags & 1) == 0 && item->instance_animation != NULL && item->animation_data != NULL) {
                animation = item->instance_animation;
                if (!(((item->flags & 2) != 0 && (animation->flags & 8) == 0 && animation->ltime <= 1.0f) ||
                      ((item->flags & 8) != 0 && (animation->flags & 8) == 0 && animation->ltime >= item->end_frame) ||
                      ((item->flags & 4) != 0 && (animation->flags & 1) != 0)))
                    continue;
            }
            position = NuSpecialGetDrawPos(&item->special);
            matrix = *NuSpecialGetDrawMtx(&item->special);
            NuVecNorm((NUVEC *)&matrix.m00, (NUVEC *)&matrix.m00);
            NuVecNorm((NUVEC *)&matrix.m10, (NUVEC *)&matrix.m10);
            NuVecNorm((NUVEC *)&matrix.m20, (NUVEC *)&matrix.m20);
            NuMtxPreScale(&matrix, &scale);
        }
        if (item->pickup == NULL)
            continue;
        pickup = (GIZMOPICKUP_PC *)item->pickup->object;
        if ((pickup->flags & 0x40) != 0 && GizmoPickupSys->alternate_type != -1)
            type = &GizmoPickupSys->types[GizmoPickupSys->alternate_type];
        else
            type = &GizmoPickupSys->types[pickup->type_index];
        if (type == NULL)
            continue;
        if ((item->flags & 0x10) == 0) {
            NuMtxSetScale(&matrix, &scale);
            NuMtxRotateY(&matrix, spin);
            NuMtxRotateX(&matrix, tilt);
            NuMtxTranslate(&matrix, position);
        }
        NuSpecialDrawAt(&world->level_objects[type->first_model_id].special, &matrix);
    }
}

void __cdecl GizmoPickups_InitSys(PICKUP_TYPE_SYSTEM_PC *system) {
    GizmoPickupSys = system;
}
static GIZMOPICKUP_PC *GizmoPickups_CollideListPc(GAMEOBJECT *object, GIZMOPICKUP_PC *pickups, i32 count) {
    i32 collide_2d, coin_magnet, index;
    f32 collide_scale, scaled_pickup, normal_pickup, radius_x, radius_y, dx, dz, radius;
    GIZMO_PICKUP_TYPE_PC *type;
    GIZMOPICKUP_PC *pickup;
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
    scaled_pickup = normal_pickup;
    if (coin_magnet)
        scaled_pickup = COINMAGNETSCALE * normal_pickup;
    if (collide_scale != 0.0f)
        scaled_pickup *= collide_scale;
    if (collide_2d && VehicleArea != 0)
        scaled_pickup += scaled_pickup;
    pickup = pickups;
    for (index = 0; index < count; ++index, ++pickup) {
        type = &GizmoPickupSys->types[pickup->type_index];
        if ((type->flags & 0x10) != 0 || (pickup->flags & 1) == 0 || (pickup->flags & 8) != 0 ||
            (pickup->flags & 2) == 0 || ((pickup->flags & 0x10) == 0 && (pickup->unknown_14[1] & 4) == 0))
            continue;
        if ((pickup->flags & 0x40) != 0 && GizmoPickupSys->alternate_type != -1)
            type = &GizmoPickupSys->types[GizmoPickupSys->alternate_type];
        if (type->field_0f != 0)
            continue;
        if ((pickup->unknown_14[1] & 4) != 0)
            return pickup;
        if (type->score > 0) {
            radius_x = type->shadow_extent_x * scaled_pickup;
            radius_y = coin_magnet ? type->shadow_extent_z * scaled_pickup : type->shadow_radius_z * scaled_pickup;
        } else {
            radius_x = type->shadow_extent_x * normal_pickup;
            radius_y = coin_magnet ? type->shadow_extent_z * normal_pickup : type->shadow_radius_z * normal_pickup;
        }
        if (object->collision_min.x > pickup->position.x + radius_x ||
            object->collision_max.x < pickup->position.x - radius_x ||
            object->collision_min.z > pickup->position.z + radius_x ||
            object->collision_max.z < pickup->position.z - radius_x)
            continue;
        if (!collide_2d) {
            if (object->collision_min.y > pickup->position.y + radius_y ||
                object->collision_max.y < pickup->position.y - radius_y)
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
        if (!(dx * dx + dz * dz >= radius * radius))
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
    pickup = GizmoPickups_CollideListPc(object, world->pickup_runtime_sys->temporary_pickups, 64);
    if (pickup != NULL)
        temporary = 1;
    else {
        temporary = 0;
        pickup = GizmoPickups_CollideListPc(object, world->pickup_runtime_sys->pickups,
                                            world->pickup_runtime_sys->pickup_count);
    }
    if (pickup != NULL) {
        pickup->flags |= 8;
        if (temporary)
            pickup->flags &= ~1u;
        type_index = (pickup->flags & 0x40) != 0 && GizmoPickupSys->alternate_type != -1
                         ? GizmoPickupSys->alternate_type
                         : pickup->type_index;
        if (GizmoPickupSys->types[type_index].collection_sfx_name != NULL)
            PlaySfx(GizmoPickupSys->types[type_index].collection_sfx_name, &pickup->position);
        if (GizmoPickupSys->types[type_index].collect_fn != NULL)
            GizmoPickupSys->types[type_index].collect_fn(world, pickup, type_index, object, 0);
        if ((GizmoPickupSys->types[type_index].flags & 0x40) != 0)
            ++AreaGlobals_PC.field_18;
        return pickup;
    }
    return NULL;
}

void __cdecl GameMsg_Draw_MiniKitDetector(GAME_MESSAGE_PC *message, NUVEC *position, f32 scale) {
    f32 excess = 1.0e9f;
    NUVEC screen = *position;
    i32 clipped = 0;
    f32 y;
    f32 distance;
    i32 alpha;
    if (message->parameter_fb != 0) {
        NuVecNeg(&screen, &screen);
        clipped = 1;
        screen.x = screen.x < 0.0f ? -0.825f : 0.825f;
        screen.y = screen.y < 0.0f ? -0.825f : 0.825f;
    }
    if (screen.x >= 0.825f) {
        excess = screen.x - 0.825f;
        screen.x = 0.825f;
        clipped = 1;
        NuStrCpy(message->text_buffer, ">");
    } else if (screen.x <= -0.825f) {
        excess = -0.825f - screen.x;
        screen.x = -0.825f;
        clipped = 1;
        NuStrCpy(message->text_buffer, "<");
    }
    if (screen.y >= 0.825f) {
        y = screen.y - 0.825f;
        screen.y = 0.825f;
        if (clipped == 0 || (excess != 1.0e9f && y > excess)) {
            clipped = 1;
            NuStrCpy(message->text_buffer, LEGOASCII_UP != NULL ? LEGOASCII_UP : "?");
        }
    } else if (screen.y <= -0.825f) {
        y = -0.825f - screen.y;
        screen.y = -0.825f;
        if (clipped == 0 || (excess != 1.0e9f && y > excess)) {
            clipped = 1;
            NuStrCpy(message->text_buffer, LEGOASCII_DOWN != NULL ? LEGOASCII_DOWN : "?");
        }
    }
    distance = NuVecDist(&message->position, &GameCam->position, NULL);
    distance = distance / (f32)WorldInfo_CurrentlyActive()->current_level->reflection_range;
    if (distance > 1.0f)
        distance = 1.0f;
    if (distance < 0.25f)
        alpha = 0x80;
    else if (distance < 0.75f)
        alpha = (i32)((1.0f - (distance - 0.25f) * 2.0f) * 64.0f + 64.0f);
    else
        alpha = 0x40;
    if (clipped != 0) {
        message->red = 0xff;
        if (message->parameter_fb != 0) {
            message->blue = 0;
            message->green = 0;
        } else {
            message->blue = 0xff;
            message->green = 0xff;
        }
        message->parameter_fc = 0;
    } else {
        message->red = 0;
        message->green = 0xff;
        message->blue = 0;
        message->parameter_fc = 4;
    }
    message->alpha = (u8)alpha;
    Text3DEx(message->text != NULL ? (char *)message->text : message->text_buffer, screen.x, screen.y, screen.z, scale,
             scale, scale, message->parameter_fc, message->red, message->green, message->blue, alpha);
}

void MiniKitDetector(NUVEC *position) {
    ADD_GAME_MESSAGE_PC message = AddGameMsg_Default;
    message.text = LEGOASCII_BIGARROW != NULL ? LEGOASCII_BIGARROW : txt_UNKNOWN;
    message.position = position;
    message.flags = 0x40083;
    message.scale = 0.6f;
    message.unknown_4f = 4;
    message.unknown_44 = ((void *)(GameMsg_Draw_MiniKitDetector));
    AddGameMsg(&message);
}

STATIC_ASSERT(sizeof(struct GIZMOPICKUPRUNTIMESYS_s) == 0x1c);
STATIC_ASSERT(offsetof(struct GIZMOPICKUPRUNTIMESYS_s, pickups) == 0);
STATIC_ASSERT(offsetof(struct GIZMOPICKUPRUNTIMESYS_s, temporary_pickups) == 4);
STATIC_ASSERT(offsetof(struct GIZMOPICKUPRUNTIMESYS_s, pickup_count) == 8);
STATIC_ASSERT(offsetof(struct GIZMOPICKUPRUNTIMESYS_s, total_score) == 0x10);
STATIC_ASSERT(offsetof(struct GIZMOPICKUPRUNTIMESYS_s, draw_distance) == 0x14);
STATIC_ASSERT(offsetof(struct GIZMOPICKUPRUNTIMESYS_s, pickup_scale) == 0x18);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLDINFO, level_idx) == 0x11c);
STATIC_ASSERT(offsetof(WORLDINFO, area_index) == 0x120);
STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(AREA_DATA_PC, flags) == 0x7a);
STATIC_ASSERT(offsetof(GIZMOPICKUP_PC, unknown_14) == 0x14);
STATIC_ASSERT(offsetof(GIZMOPICKUP_PC, activation_group) == 0x18);

static __declspec(noinline) void SetAreaPickupScalePc(i32 area) {
    u16 flags;
    AreaPickupScale = 1.0f;
    if (area < 0 || area >= AREACOUNT)
        return;
    flags = ADataList[area].flags;
    if ((flags & 0x200) != 0) {
        AreaPickupScale = 6.0f;
        return;
    }
    if ((flags & 1) != 0)
        AreaPickupScale = BonusArea != 0 ? 3.0f : 5.0f;
}

static __declspec(noinline) void AdjustPickupDrawDistancePc(i32 version, i32 area, f32 *distance) {
    if (version > 6)
        return;
    if (version <= 5 && *distance < 10.0f)
        *distance = 10.0f;
    if (area >= 0 && area < AREACOUNT && (ADataList[area].flags & 1) != 0 && (f64)*distance < 100.0)
        *distance = 100.0f;
}

void *__cdecl GizmoPickups_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    world->giz_buffer.addr = (world->giz_buffer.addr + 3u) & ~3u;
    world->pickup_runtime_sys = (struct GIZMOPICKUPRUNTIMESYS_s *)world->giz_buffer.addr;
    world->giz_buffer.addr += sizeof(struct GIZMOPICKUPRUNTIMESYS_s);
    memset(world->pickup_runtime_sys, 0, sizeof(struct GIZMOPICKUPRUNTIMESYS_s));
    world->pickup_runtime_sys->draw_distance = 10.0f;
    SetAreaPickupScalePc(world->area_index);
    world->pickup_runtime_sys->pickup_scale = AreaPickupScale;
    SetAreaPickupGravity(world->area_index, world->level_idx);
    if ((u16)world->current_level->max_pickups > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3u) & ~3u;
        world->pickup_runtime_sys->pickups = (GIZMOPICKUP_PC *)world->giz_buffer.addr;
        world->giz_buffer.addr += (u16)world->current_level->max_pickups * sizeof(GIZMOPICKUP_PC);
    }
    world->giz_buffer.addr = (world->giz_buffer.addr + 3u) & ~3u;
    world->pickup_runtime_sys->temporary_pickups = (GIZMOPICKUP_PC *)world->giz_buffer.addr;
    world->giz_buffer.addr += 64 * sizeof(GIZMOPICKUP_PC);
    return world->pickup_runtime_sys;
}

i32 __cdecl GizmoPickups_Load(void *world_ptr, void *unused) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZMOPICKUP_PC *pickup;
    i32 version, index;
    (void)unused;
    if (world->pickup_runtime_sys->pickup_count == 0) {
        version = EdFileReadInt();
        world->pickup_runtime_sys->pickup_count = EdFileReadInt();
        if (version >= 3)
            world->pickup_runtime_sys->total_score = EdFileReadInt();
        if (version >= 5) {
            world->pickup_runtime_sys->draw_distance = EdFileReadFloat();
            world->pickup_runtime_sys->pickup_scale = EdFileReadFloat();
            if (version > 5)
                AreaPickupScale = world->pickup_runtime_sys->pickup_scale;
            else {
                SetAreaPickupScalePc(world->area_index);
                world->pickup_runtime_sys->pickup_scale = AreaPickupScale;
            }
        } else {
            AreaPickupScale = 1.0f;
            world->pickup_runtime_sys->pickup_scale = 1.0f;
        }
        AdjustPickupDrawDistancePc(version, world->area_index, &world->pickup_runtime_sys->draw_distance);
        SetAreaPickupGravity(world->area_index, world->level_idx);
        pickup = world->pickup_runtime_sys->pickups;
        for (index = 0; index < world->pickup_runtime_sys->pickup_count; ++index, ++pickup) {
            EdFileRead(pickup->name, sizeof(pickup->name));
            EdFileReadNuVec(&pickup->position);
            pickup->unknown_14[0] = (u8)EdFileReadChar();
            if (version >= 2)
                pickup->unknown_14[1] = (u8)EdFileReadChar();
            if (version >= 4)
                pickup->activation_group = (u8)EdFileReadChar();
        }
        return 1;
    }
    return 0;
}

typedef struct PICKUP_RESET_RECORD_PC {
    char name[8];
    NUVEC position;
    u8 type_code, config_flags, model_variant;
    union {
        u8 flags;
        struct {
            u8 active : 1, enabled : 1, visible : 1, collected : 1, drawn : 1, draw_visible : 1, alternate_type : 1,
                activated : 1;
        };
    };
    u8 unknown_18[4];
    f32 floor;
    u16 shadow_x, shadow_z;
    i8 room;
    u8 type_index;
    u16 rotation;
    f32 timer;
} PICKUP_RESET_RECORD_PC;
typedef struct PICKUP_RESET_PROGRESS_PC {
    u32 collected[16], enabled[16], visible[16], activated[16];
} PICKUP_RESET_PROGRESS_PC;
STATIC_ASSERT(sizeof(PICKUP_RESET_RECORD_PC) == 0x2c);
STATIC_ASSERT(offsetof(PICKUP_RESET_RECORD_PC, position) == 8);
STATIC_ASSERT(offsetof(PICKUP_RESET_RECORD_PC, type_code) == 0x14);
STATIC_ASSERT(offsetof(PICKUP_RESET_RECORD_PC, config_flags) == 0x15);
STATIC_ASSERT(offsetof(PICKUP_RESET_RECORD_PC, model_variant) == 0x16);
STATIC_ASSERT(offsetof(PICKUP_RESET_RECORD_PC, flags) == 0x17);
STATIC_ASSERT(offsetof(PICKUP_RESET_RECORD_PC, floor) == 0x1c);
STATIC_ASSERT(offsetof(PICKUP_RESET_RECORD_PC, shadow_x) == 0x20);
STATIC_ASSERT(offsetof(PICKUP_RESET_RECORD_PC, shadow_z) == 0x22);
STATIC_ASSERT(offsetof(PICKUP_RESET_RECORD_PC, room) == 0x24);
STATIC_ASSERT(offsetof(PICKUP_RESET_RECORD_PC, type_index) == 0x25);
STATIC_ASSERT(offsetof(PICKUP_RESET_RECORD_PC, rotation) == 0x26);
STATIC_ASSERT(offsetof(PICKUP_RESET_RECORD_PC, timer) == 0x28);
STATIC_ASSERT(sizeof(PICKUP_RESET_PROGRESS_PC) == 0x100);
STATIC_ASSERT(offsetof(PICKUP_RESET_PROGRESS_PC, enabled) == 0x40);
STATIC_ASSERT(offsetof(PICKUP_RESET_PROGRESS_PC, visible) == 0x80);
STATIC_ASSERT(offsetof(PICKUP_RESET_PROGRESS_PC, activated) == 0xc0);
STATIC_ASSERT(offsetof(struct GIZMOPICKUPRUNTIMESYS_s, pickups) == 0);
STATIC_ASSERT(offsetof(struct GIZMOPICKUPRUNTIMESYS_s, temporary_pickups) == 4);
STATIC_ASSERT(offsetof(struct GIZMOPICKUPRUNTIMESYS_s, pickup_count) == 8);
STATIC_ASSERT(sizeof(struct GIZMOPICKUPRUNTIMESYS_s) == 0x1c);
STATIC_ASSERT(sizeof(struct LEVELSAVE_s) == 0x54);
STATIC_ASSERT(offsetof(struct LEVELSAVE_s, minikit_count) == 0x50);
STATIC_ASSERT(offsetof(WORLDINFO, pickup_runtime_sys) == 0x50b8);
STATIC_ASSERT(offsetof(WORLDINFO, current_gscn) == 0x13c);
STATIC_ASSERT(offsetof(WORLDINFO, level_idx) == 0x11c);
extern u8 *Game_LevelSave;
extern const f64 GameObjectNearFloorNoFloorHeight;

static i32 FindPickupTypeIndex(u8 type_code) {
    i32 index;
    for (index = 0; index < GizmoPickupSys->type_count; ++index)
        if (GizmoPickupSys->types[index].field_c == type_code)
            return index;
    return 0;
}

static __declspec(noinline) void GizmoPickups_Reset(WORLDINFO *world, PICKUP_RESET_RECORD_PC *pickup) {
    f32 floor;
    pickup->type_index = (u8)FindPickupTypeIndex(pickup->type_code);
    NewTerrPlatformsOff();
    floor = GameShadow(NULL, &pickup->position, 5.0f, -1);
    pickup->floor = floor;
    if ((f64)pickup->floor != GameObjectNearFloorNoFloorHeight) {
        if (pickup->floor >= pickup->position.y)
            pickup->floor = LevelConfigFarDistance;
        else
            FindAnglesZX(&ShadNorm, &pickup->shadow_x, &pickup->shadow_z);
    }
    pickup->flags = (pickup->flags & ~0x18) | 7;
    if (GizmoPickupSys->types[pickup->type_index].random_model_count > 0) {
        i32 random = qrand();
        pickup->model_variant =
            (u8)(random / (65535 / GizmoPickupSys->types[pickup->type_index].random_model_count + 1));
    } else {
        pickup->model_variant = 0;
    }
    pickup->room = world->current_gscn != NULL ? NuPortalWhichRoom(world->current_gscn, &pickup->position) : -1;
    pickup->rotation = (u16)qrand();
    pickup->timer = 0.0f;
    pickup->flags = (pickup->flags & ~0x40) | 0x20;
}

void __cdecl GizmoPickups_ResetPc(void *world_ptr, void *unused, void *progress_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    PICKUP_RESET_PROGRESS_PC *progress = (PICKUP_RESET_PROGRESS_PC *)progress_ptr;
    struct GIZMOPICKUPRUNTIMESYS_s *system;
    PICKUP_RESET_RECORD_PC *pickup;
    i32 index;
    (void)unused;
    for (index = 0; index < GizmoPickupSys->type_count; ++index) {
        GizmoPickupSys->types[index].shadow_extent_x =
            GizmoPickupSys->types[index].draw_distance * GizmoPickupSys->types[index].shadow_radius_x;
        GizmoPickupSys->types[index].shadow_extent_z =
            GizmoPickupSys->types[index].draw_distance * GizmoPickupSys->types[index].shadow_radius_z;
    }
    if (world == NULL)
        return;
    system = world->pickup_runtime_sys;
    pickup = (PICKUP_RESET_RECORD_PC *)system->pickups;
    if (pickup != NULL) {
        for (index = 0; index < world->pickup_runtime_sys->pickup_count; ++index, ++pickup) {
            GizmoPickups_Reset(world, pickup);
            if (progress != NULL && index < 512) {
                i32 word = index / 32;
                u32 bit = 1u << (index & 31);
                pickup->visible = (progress->visible[word] & bit) != 0;
                pickup->enabled = (progress->enabled[word] & bit) != 0;
                pickup->collected = (progress->collected[word] & bit) != 0;
                pickup->activated = (progress->activated[word] & bit) != 0;
                if ((pickup->config_flags & 1) != 0 && !pickup->activated) {
                    pickup->enabled = 0;
                    pickup->visible = 0;
                }
            } else if ((pickup->config_flags & 1) != 0) {
                pickup->enabled = 0;
                pickup->visible = 0;
            }
            if (GizmoPickupSys->gizmo_type_id != -1 && pickup->type_index == GizmoPickupSys->gizmo_type_id &&
                Game_LevelSave != NULL) {
                if (SuperStory != 0) {
                    pickup->flags |= 0x40;
                } else {
                    i32 saved_index;
                    for (saved_index = 0;
                         saved_index < ((struct LEVELSAVE_s *)Game_LevelSave)[world->level_idx].minikit_count;
                         ++saved_index) {
                        if (NuStrICmp(pickup->name, ((struct LEVELSAVE_s *)Game_LevelSave)[world->level_idx]
                                                        .minikit_names[saved_index]) == 0) {
                            pickup->flags |= 0x40;
                            break;
                        }
                    }
                }
            }
        }
    }
    if (world->pickup_runtime_sys->temporary_pickups != NULL)
        memset(world->pickup_runtime_sys->temporary_pickups, 0, 0xb00);
}

extern i32 turret_gizmotype_id, gizbuildit_gizmotype_id;
i32 __cdecl SuperCounter_AnyCollected(AREA_SUPERCOUNTER_PC *, WORLDINFO *);

typedef struct pickup_counter_turret_pc_s {
    u8 unknown_000[0x13a];
    u16 flags;
} PICKUP_COUNTER_TURRET_PC;
typedef struct pickup_counter_blowup_pc_s {
    u8 unknown_00[0x94];
    u8 output_flags;
} PICKUP_COUNTER_BLOWUP_PC;
typedef struct pickup_counter_buildit_pc_s {
    u8 unknown_00[0x6d];
    u8 build_state;
} PICKUP_COUNTER_BUILDIT_PC;
STATIC_ASSERT(offsetof(PICKUP_COUNTER_TURRET_PC, flags) == 0x13a);
STATIC_ASSERT(offsetof(PICKUP_COUNTER_BLOWUP_PC, output_flags) == 0x94);
STATIC_ASSERT(offsetof(PICKUP_COUNTER_BUILDIT_PC, build_state) == 0x6d);
STATIC_ASSERT(offsetof(GAMECAMERA, position) == 0x11c);
STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);

static __declspec(noinline) NUMTX *PickupSetRotationXPc(NUMTX *matrix, i32 angle) {
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    matrix->m00 = ANI3_EVAL_IDENTITY_MATRIX.m00;
    matrix->m01 = ANI3_EVAL_IDENTITY_MATRIX.m01;
    matrix->m02 = ANI3_EVAL_IDENTITY_MATRIX.m02;
    matrix->m03 = ANI3_EVAL_IDENTITY_MATRIX.m03;
    matrix->m10 = 0.0f;
    matrix->m11 = cosine;
    matrix->m12 = sine;
    matrix->m13 = 0.0f;
    matrix->m20 = 0.0f;
    matrix->m21 = -sine;
    matrix->m22 = cosine;
    matrix->m23 = 0.0f;
    matrix->m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
    matrix->m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
    matrix->m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
    matrix->m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
    return matrix;
}

static __declspec(noinline) NUMTX *PickupSetRotationYPc(NUMTX *matrix, i32 angle) {
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
    matrix->m22 = cosine;
    matrix->m23 = 0.0f;
    matrix->m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
    matrix->m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
    matrix->m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
    matrix->m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
    return matrix;
}

static __declspec(noinline) NUMTX *PickupRotateYPc(NUMTX *matrix, i32 angle) {
    f32 cosine = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m00 = matrix->m00, m10 = matrix->m10, m20 = matrix->m20, m30 = matrix->m30;
    matrix->m00 = m00 * cosine + matrix->m02 * sine;
    matrix->m02 = matrix->m02 * cosine - m00 * sine;
    matrix->m10 = m10 * cosine + matrix->m12 * sine;
    matrix->m12 = matrix->m12 * cosine - m10 * sine;
    matrix->m20 = m20 * cosine + matrix->m22 * sine;
    matrix->m22 = matrix->m22 * cosine - m20 * sine;
    matrix->m30 = m30 * cosine + matrix->m32 * sine;
    matrix->m32 = matrix->m32 * cosine - m30 * sine;
    return matrix;
}

static __declspec(noinline) void GizmoPickups_UpdateListPc(WORLDINFO *world, GIZMOPICKUP_PC *pickup, i32 count,
                                                           i32 play_nearby) {
    i32 detectors, index, eligible, complete;
    f32 nearest_distance;
    NUVEC *nearest;
    GIZMO_PICKUP_TYPE_PC *type;
    AREA_SUPERCOUNTER_PC *counter;
    AREA_SUPERCOUNTER_PICKUP_PC *counter_pickup;
    GIZMO_PC *gizmo;
    i32 effect_flags;
    NUVEC position;
    f32 distance, timer;
    if (pickup == NULL || Missions_PickupsOff(MissionSys))
        return;
    nearest_distance = 100000.0f;
    nearest = NULL;
    detectors = 0;
    if (play_nearby && FadeSys.fade == 0.0f && NuFmod(GameTimer.time_elapsed_mod_seconds, 0.2f) < (f64)(f32)0.1f) {
        if (Cheats_CheckFlags(0x200))
            detectors = 1;
        if (Cheats_CheckFlags(0x40000))
            detectors |= 2;
    }
    SuperCounters_ResetProcessed(world);
    for (index = 0; index < count; ++index, ++pickup) {
        if (!(pickup->flags & 1))
            continue;
        pickup->rotation += FRAMETIME * 16384.0;
        pickup->flags |= 0x20;
        if ((pickup->flags & 0x40) && GizmoPickupSys->alternate_type != -1)
            type = &GizmoPickupSys->types[GizmoPickupSys->alternate_type];
        else
            type = &GizmoPickupSys->types[pickup->type_index];
        if (!(pickup->flags & 8) && type->field_0f == 0) {
            effect_flags = type->flags;
            if (effect_flags & 0x2c) {
                eligible = 1;
                counter = SuperCounter_FindFromNameAndLevel(pickup->name, world, &counter_pickup);
                if (counter != NULL) {
                    complete = 0;
                    if ((!pickup->state_enabled || !pickup->state_visible) && !(counter->processed_flags & 1) &&
                        counter_pickup->position_gizmo != NULL) {
                        gizmo = (GIZMO_PC *)counter_pickup->position_gizmo;
                        if (gizmo->type_id == turret_gizmotype_id) {
                            if (((PICKUP_COUNTER_TURRET_PC *)gizmo->object)->flags & 0x20)
                                complete = 1;
                        } else if (gizmo->type_id == blowup_gizmotype_id) {
                            if (((PICKUP_COUNTER_BLOWUP_PC *)gizmo->object)->output_flags & 1)
                                complete = 1;
                        } else if (gizmo->type_id == gizbuildit_gizmotype_id) {
                            if (((PICKUP_COUNTER_BUILDIT_PC *)gizmo->object)->build_state == 2)
                                complete = 1;
                        }
                    }
                    if (complete || SuperCounter_AnyCollected(counter, world) ||
                        ((counter->processed_flags & 1) && (counter->processed_flags & 2)))
                        eligible = 0;
                    counter->processed_flags |= 2;
                }
                if (eligible) {
                    if (((detectors & 1) && (effect_flags & 4)) || ((detectors & 2) && (effect_flags & 8))) {
                        position.x = pickup->position.x;
                        position.y = pickup->position.y + 0.2f;
                        position.z = pickup->position.z;
                        MiniKitDetector(&position);
                    }
                    if (play_nearby) {
                        distance = NuVecDistSqr(&pickup->position, &GameCam->position, NULL);
                        if (distance < nearest_distance) {
                            nearest_distance = distance;
                            nearest = &pickup->position;
                        }
                    }
                }
            }
        }
        if (pickup->timer > 0.0f) {
            if (pickup->timer <= 2.0f) {
                if (pickup->timer <= 0.0f)
                    pickup->flags &= ~2;
                else if ((u32)GameTimer.update_count % 6 >= 3)
                    pickup->flags &= ~0x20;
            }
            if (MiniCutCam != 2) {
                timer = pickup->timer - FRAMETIME;
                pickup->timer = timer;
                if (timer < 0.0f)
                    pickup->flags &= ~1;
            }
        }
        if (type->field_0f == 0 && !(pickup->flags & 8) && (pickup->flags & 2) && (pickup->flags & 4) &&
            type->update_fn != NULL)
            type->update_fn(world, pickup);
    }
    if (nearest != NULL)
        GameAudio_PlaySfx(0x25, nearest, 0, 0);
}

void __cdecl GizmoPickups_Update(void *world_pointer, void *unused_system, f32 unused_time) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    struct GIZMOPICKUPRUNTIMESYS_s *system;
    (void)unused_system;
    (void)unused_time;
    if (world == NULL)
        return;
    system = world->pickup_runtime_sys;
    GizmoPickups_UpdateListPc(world, system->pickups, system->pickup_count, 1);
    system = world->pickup_runtime_sys;
    GizmoPickups_UpdateListPc(world, system->temporary_pickups, 64, 0);
}

static __declspec(noinline) void GizmoPickups_DrawList(WORLDINFO *world, GIZMOPICKUP_PC *pickup, i32 count) {
    i32 index, angle, model_index, opacity;
    GIZMO_PICKUP_TYPE_PC *type;
    NUMTX matrix;
    NUVEC shadow_position;
    f32 maximum_distance_squared, shadow_distance_squared, dx, dz, distance_squared, factor;
    if (pickup == NULL || Missions_PickupsOff(MissionSys))
        return;
    NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f);
    maximum_distance_squared = world->pickup_runtime_sys->draw_distance * world->pickup_runtime_sys->draw_distance;
    shadow_distance_squared =
        (f32)((world->pickup_runtime_sys->draw_distance * 0.5) * (world->pickup_runtime_sys->draw_distance * 0.5));
    for (index = 0; index < count; ++index, ++pickup) {
        if (!(pickup->flags & 1))
            continue;
        pickup->flags &= ~0x10;
        if (!(pickup->flags & 4) || (pickup->flags & 8) || !(pickup->flags & 0x20) ||
            (pickup->room >= 0 && world->rooms_visible_ptr[pickup->room] == 0))
            continue;
        if ((pickup->flags & 0x40) && GizmoPickupSys->alternate_type != -1)
            type = &GizmoPickupSys->types[GizmoPickupSys->alternate_type];
        else
            type = &GizmoPickupSys->types[pickup->type_index];
        if (type->field_0f != 0)
            continue;
        dx = pickup->position.x - GameCam->position.x;
        dz = pickup->position.z - GameCam->position.z;
        distance_squared = dx * dx + dz * dz;
        if (distance_squared > maximum_distance_squared)
            continue;
        model_index = type->first_model_id + pickup->model_variant;
        if (world->level_objects[model_index].active != 0) {
            if (type->flags & 1) {
                angle = (i32)((f64)NU_SIN_LUT(pickup->rotation * 2) * 1820.0);
                PickupSetRotationXPc(&matrix, angle);
                PickupRotateYPc(&matrix, pickup->rotation);
                NuMtxTranslate(&matrix, &pickup->position);
            } else if (type->flags & 2) {
                PickupSetRotationYPc(&matrix, pickup->rotation);
                NuMtxTranslate(&matrix, &pickup->position);
            } else {
                NuMtxSetTranslation(&matrix, &pickup->position);
            }
            if (AreaPickupScale != 1.0f)
                NuMtxPreScaleU(&matrix, AreaPickupScale);
            if (pickup->unknown_14[1] & 8)
                ResetShadowMapRendering();
            pickup->state_drawn = NuSpecialDrawAt(&world->level_objects[model_index].special, &matrix) != 0;
            if (pickup->state_drawn && type->overlay_model_id != -1 &&
                world->level_objects[type->overlay_model_id].active != 0) {
                ResetShadowMapRendering();
                NuSpecialDrawAt(&world->level_objects[type->overlay_model_id].special, &matrix);
                EnableShadowMapRendering(0);
            }
            if (pickup->unknown_14[1] & 8)
                EnableShadowMapRendering(0);
        }
        if (VehicleArea == 0 && (f64)pickup->floor != GameObjectNearFloorNoFloorHeight &&
            distance_squared < shadow_distance_squared) {
            shadow_position.x = pickup->position.x;
            shadow_position.y = (f32)((f64)pickup->floor + (f64)(f32)0.005f);
            shadow_position.z = pickup->position.z;
            factor = (f32)(1.0 - (f64)distance_squared / shadow_distance_squared);
            opacity = (i32)((f64)factor * world->current_level->blob_shadow_alpha);
            NuRndrAddShadow(&shadow_position, type->shadow_radius_x, opacity, pickup->shadow_x, 0, pickup->shadow_z);
        }
    }
}

void __cdecl GizmoPickups_Draw(void *world_pointer, void *unused_system, f32 unused_time) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    struct GIZMOPICKUPRUNTIMESYS_s *system;
    (void)unused_system;
    (void)unused_time;
    if (world == NULL)
        return;
    EnableShadowMapRendering(0);
    system = world->pickup_runtime_sys;
    GizmoPickups_DrawList(world, system->pickups, system->pickup_count);
    system = world->pickup_runtime_sys;
    GizmoPickups_DrawList(world, system->temporary_pickups, 64);
    ResetShadowMapRendering();
}

/* Append to gizmopickups.c: share its actual private GizmoPickupSys pointer. */
STATIC_ASSERT(sizeof(GIZMOPICKUP_PC) == 0x2c);
STATIC_ASSERT(offsetof(WORLDINFO, pickup_runtime_sys) == 0x50b8);
STATIC_ASSERT(offsetof(WORLDINFO, game_debris_sys) == 0x134);
void __cdecl SuperCounter_ActivateGizmoPickup(GIZMO_PC *, GIZMOPICKUP_PC *);
i32 __cdecl GizmoPickup_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    (void)output;
    (void)mode;
    return (((GIZMOPICKUP_PC *)gizmo->object)->flags >> 3) & 1;
}
typedef struct PICKUP_VISIBILITY_VIEW_PC {
    u8 unknown_00[0x17];
    struct {
        u8 unknown_0 : 2;
        u8 visible : 1;
        u8 unknown_3 : 5;
    } flags;
    u8 unknown_18[0x14];
} PICKUP_VISIBILITY_VIEW_PC;
STATIC_ASSERT(sizeof(PICKUP_VISIBILITY_VIEW_PC) == sizeof(GIZMOPICKUP_PC));
STATIC_ASSERT(offsetof(PICKUP_VISIBILITY_VIEW_PC, flags) == offsetof(GIZMOPICKUP_PC, flags));
STATIC_ASSERT(offsetof(PICKUP_VISIBILITY_VIEW_PC, flags) == 0x17);
STATIC_ASSERT(sizeof(((PICKUP_VISIBILITY_VIEW_PC *)0)->flags) == 1);

void __cdecl GizmoPickup_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    PICKUP_VISIBILITY_VIEW_PC *pickup;
    if (gizmo != NULL) {
        pickup = (PICKUP_VISIBILITY_VIEW_PC *)gizmo->object;
        pickup->flags.visible = visible != 0;
    }
}
void __cdecl GizmoPickup_Activate(GIZMO_PC *gizmo, i32 active) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    GIZMOPICKUP_PC *pickup, *group_pickup;
    GIZMO_PICKUP_TYPE_PC *type;
    i32 index;
    if (gizmo == NULL)
        return;
    pickup = (GIZMOPICKUP_PC *)gizmo->object;
    pickup->state_enabled = active != 0;
    if (active != 0) {
        SuperCounter_ActivateGizmoPickup(gizmo, pickup);
        if (pickup->state_enabled) {
            pickup->state_activated = 1;
            if (pickup->state_alternate_type && GizmoPickupSys->alternate_type != -1)
                type = &GizmoPickupSys->types[GizmoPickupSys->alternate_type];
            else
                type = &GizmoPickupSys->types[pickup->type_index];
            if (!(ChallengeMode && !(type->flags & 0x20)) && !(!ChallengeMode && (type->flags & 0x20)) &&
                Mission_Active(NULL) == NULL) {
                if (type->activation_sfx_name != NULL)
                    PlaySfx(type->activation_sfx_name, &pickup->position);
                if (type->debris_id != -1) {
                    if (pickup->state_alternate_type)
                        AddGameDebris(world->game_debris_sys,
                                      GizmoPickupSys->types[GizmoPickupSys->gizmo_type_id].debris_id,
                                      &pickup->position);
                    else
                        AddGameDebris(world->game_debris_sys, type->debris_id, &pickup->position);
                }
            }
        }
    }
    if (pickup->activation_group == 0)
        return;
    group_pickup = world->pickup_runtime_sys->pickups;
    for (index = 0; index < world->pickup_runtime_sys->pickup_count; ++index, ++group_pickup) {
        if (group_pickup->activation_group == pickup->activation_group) {
            group_pickup->state_enabled = active;
            group_pickup->state_visible = active;
        }
    }
}

STATIC_ASSERT(offsetof(GIZMOPICKUP_PC, filter_byte) == 0x14);
/* Complete native00570030..00570082. */
i32 __cdecl GizmoPickup_NumberOfTypePc(WORLDINFO *world, i32 type, u8 filter) {
    GIZMOPICKUPRUNTIMESYS_s *system = world->pickup_runtime_sys;
    GIZMOPICKUP_PC *pickup = system->pickups;
    i32 count = 0;
    i32 i;
    if (filter == 0 && type == -1)
        return 0;
    i = system->pickup_count;
    if (i > 0) {
        do {
            if (filter != 0) {
                if (pickup->filter_byte == filter)
                    ++count;
            } else if (pickup->type_index == type) {
                ++count;
            }
            ++pickup;
        } while (--i != 0);
    }
    return count;
}

GIZMOPICKUP_PC *__cdecl GizmoPickup_FindByName(WORLDINFO *world, char *name) {
    GIZMOPICKUP_PC *pickup;
    i32 index;
    if (world != NULL && name != NULL) {
        pickup = world->pickup_runtime_sys->pickups;
        if (pickup != NULL) {
            for (index = 0; index < world->pickup_runtime_sys->pickup_count; ++index, ++pickup) {
                if (NuStrICmp(pickup->name, name) == 0)
                    return pickup;
            }
        }
    }
    return NULL;
}

void __cdecl GizmoPickup_TurnOnPickup(GIZMOPICKUP_PC *pickup) {
    if (pickup != NULL)
        pickup->flags |= 0x86;
}

i32 __cdecl GizmoPickup_BeenTurnedOn(GIZMOPICKUP_PC *pickup) {
    return pickup != NULL ? pickup->flags >> 7 : 0;
}

/* Studs the level's placed pickups are worth (TotalLevelCoinTally). */
u32 __cdecl GizmoPickups_TotalScore(WORLDINFO *world) {
    struct GIZMOPICKUPRUNTIMESYS_s *system = world->pickup_runtime_sys;
    GIZMOPICKUP_PC *pickup = system->pickups;
    u32 score = 0;
    i32 i;
    if (pickup != NULL) {
        for (i = 0; i < system->pickup_count; i++, pickup++)
            score += GizmoPickupSys->types[pickup->type_index].score;
    }
    return score;
}
