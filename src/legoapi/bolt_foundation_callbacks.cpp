#include "saga.h"

STATIC_ASSERT(offsetof(PART_PC, field_206) == 0x206);
STATIC_ASSERT(offsetof(PART_PC, scale_time) == 0xfc);
STATIC_ASSERT(offsetof(BOLT_PC, owner) == 4);
STATIC_ASSERT(offsetof(BOLT_PC, position) == 0x88);
STATIC_ASSERT(offsetof(BOLT_PC, hit_debris) == 0xfc);
STATIC_ASSERT(offsetof(BOLT_PC, hit_part_debris) == 0xfe);
STATIC_ASSERT(offsetof(BOLT_PC, type_id) == 0x101);
STATIC_ASSERT(offsetof(BOLT_PC, field_104) == 0x104);
STATIC_ASSERT(offsetof(BOLT_PC, dogfight_hit_position) == 0x12c);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);
STATIC_ASSERT(offsetof(GAMEOBJECT, transform) == 0xb8);
STATIC_ASSERT(offsetof(GAMEOBJECT, movement_mode) == 0x107a);
STATIC_ASSERT(offsetof(GAMEOBJECT, bounds_radius) == 0x1dc);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_0x276) == 0x27a);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_duration) == 0x784);
STATIC_ASSERT(offsetof(GAMEOBJECT, character_context) == 0x7b5);
STATIC_ASSERT(offsetof(GAMEOBJECT, locator_matrices) == 0x804);
STATIC_ASSERT(offsetof(GAMEOBJECT, torpedo) == 0xcc4);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_1fc_word) == 0x1fc);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_280) == 0x280);
STATIC_ASSERT(offsetof(GAMEOBJECT, pad) == 0xca4);
STATIC_ASSERT(offsetof(GAMEDEBRISENTRY_PC, effect) == 0);
STATIC_ASSERT(85 * sizeof(GAMEDEBRISENTRY_PC) == 0x6a4);

extern "C" i32 __cdecl Bolt_HitPart_LSW(BOLT_PC *, PART_PC *part) {
    if (part->field_206 == 1) {
        if (part->scale_time < 2.0f)
            return 1;
    } else if (part->field_206 == 2) {
        if (part->scale_time < 1.0f)
            return 1;
    }
    return 0;
}

extern "C" void __cdecl GetShootOrigin_LSW(GAMEOBJECT *object, NUVEC *position) {
    *position = object->collision_position;
    if (object->id == (u16)id_ATAT) {
        position->x += NU_SIN_LUT(object->field_0x276) * object->bounds_radius * 1.5;
        position->z += NU_COS_LUT(object->field_0x276) * object->bounds_radius * 1.5;
    }
}

extern "C" void __cdecl UpdateBolt_Geonosian(BOLT_PC *bolt) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 effect = world->game_debris_sys->entries[85].effect;
    if (effect != -1)
        AddVariableShotDebrisEffectTimed1(effect, &bolt->position, 60, FRAMETIME, 0, 0, NULL);
}

extern "C" void __cdecl Torpedo_InitRicochet(BOLT_PC *bolt, NUVEC *position) {
    if (bolt != NULL && bolt->owner != NULL && static_cast<GAMEOBJECT *>(bolt->owner)->torpedo != NULL) {
        TORPEDOPACKET_PC *packet = static_cast<GAMEOBJECT *>(bolt->owner)->torpedo;
        if (packet->target != NULL) {
            if (packet->ricochet_count > 4) {
                packet->flags |= 0x10;
            } else {
                packet->flags |= 8;
                packet->ricochet_position = *position;
                packet->ricochet_time = 0.0f;
                ++packet->ricochet_count;
            }
        }
    }
}

extern "C" f32 __cdecl Torpedo_Scale(BOLT_PC *bolt) {
    if (bolt != NULL && bolt->owner != NULL) {
        volatile f32 scale = 2.0f * static_cast<GAMEOBJECT *>(bolt->owner)->bounds_radius;
        return scale;
    }
    return 3.0f;
}

extern "C" u16 __cdecl GetShootDirection_LSW(GAMEOBJECT *object, NUVEC *direction) {
    NUVEC temporary;
    if (direction == NULL)
        direction = &temporary;
    if (object->movement_mode == 4) {
        NUMTX matrix = object->transform;
        NuMtxPreRotateY(&matrix, 0x8000);
        NuVecMtxRotate(direction, &v001, &matrix);
        return object->facing_angle;
    }
    CHARACTERDATA_PC *model = object->character_data;
    PLAYERCHARACTERCONFIG_PC *data = model->game_character;
    if (data->weapon_shoot_joints[0] != -1 &&
        ((object->id == (u16)id_ATAT && (object->flags_1fc & 0x80) == 0) || object->id == (u16)id_CLONEWALKER ||
         object->id == GameObjectRotationAtstId || object->id == (u16)id_ATST_LOWRES)) {
        direction->x = direction->y = 0.0f;
        direction->z = object->id == (u16)id_ATAT ? 1.0f : -1.0f;
        NuVecMtxRotate(direction, direction,
                       &object->locator_matrices[object->character_data->game_character->weapon_shoot_joints[0]]);
        return NuAtan2D(direction->x, direction->z);
    }
    u16 angle;
    if ((model->model_flags & 0x2000) == 0 && (data->flags_090 & 0x80) == 0) {
        angle = object->movement_facing_angle;
    } else {
        angle = object->facing_angle;
        if (object->character_context == 0x2a) {
            volatile f32 progress = 1.0f - object->context_timer / object->context_duration;
            if (progress >= 0.25f)
                angle += 0x8000;
        }
    }
    if (direction != NULL) {
        direction->x = NU_SIN_LUT(angle);
        direction->y = 0.0f;
        direction->z = NU_COS_LUT(angle);
    }
    return angle;
}

extern "C" u16 __cdecl GetShootDirection_Batman(GAMEOBJECT *object, NUVEC *direction) {
    NUVEC temporary;
    if (direction == NULL)
        direction = &temporary;
    if (object->movement_mode == 4) {
        NUMTX matrix = object->transform;
        NuMtxPreRotateY(&matrix, 0x8000);
        NuVecMtxRotate(direction, &v001, &matrix);
        return object->facing_angle;
    }
    u16 angle;
    if ((object->character_data->model_flags & 0x2000) == 0 &&
        (object->character_data->game_character->flags_090 & 0x80) == 0) {
        angle = object->movement_facing_angle;
    } else {
        angle = object->facing_angle;
        if (object->character_context == 0x2a) {
            volatile f32 progress = 1.0f - object->context_timer / object->context_duration;
            if (progress >= 0.25f)
                angle += 0x8000;
        }
    }
    if (direction != NULL) {
        direction->x = NU_SIN_LUT(angle);
        direction->y = 0.0f;
        direction->z = NU_COS_LUT(angle);
    }
    return angle;
}

extern "C" void __cdecl Bolt_Debris_LSW(BOLT_PC *bolt, NUVEC *points, i32 point, NUVEC *, i32 no_explosion) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    i32 effect;
    if (world->current_level == DOGFIGHTA_LDATA) {
        if (bolt->field_104 != -1)
            points = &bolt->dogfight_hit_position;
        effect = 0x2e;
    } else {
        effect = bolt->hit_debris;
    }
    for (i32 i = 0; i < 3; ++i) {
        if (((bolt->flags & 0x40000200) == 0 && point == -1) ||
            ((bolt->flags & 0x40000200) != 0 && point == -1 && i == 1) || point == i) {
            AddGameDebris(world->game_debris_sys, effect, &points[i]);
            if (!(bolt->flags & 0x10000000) && Player_HasDoubleBoltDamage_FromBolt(bolt))
                AddGameDebris(world->game_debris_sys, effect, &points[i]);
        }
    }
    if (bolt->hit_part_debris != -1)
        AddPartDebris(world->part_debris_sys, bolt->hit_part_debris, &bolt->position);
    if (bolt->type_id == 0x1d) {
        AddGameDebris(world->game_debris_sys, 0x7f, &bolt->position);
        AddPartDebris(world->part_debris_sys, 6, &bolt->position);
        GameCam_NewShake(GameCam, 0.75f, 0.75f, 1.0f);
        NewRumbleAllPlayers(0.5f, 0.0f, 0, 0);
    }
    if (no_explosion == 0 && bolt->owner != NULL && (static_cast<GAMEOBJECT *>(bolt->owner)->flags_1fc & 0x80) != 0 &&
        Cheat_IsOn(0x12) && (world->area == NULL || !(world->area->flags & 1))) {
        AddGameDebris(WORLD->game_debris_sys, 0x49, &bolt->position);
        NewRumble(static_cast<GAMEOBJECT *>(bolt->owner)->pad->bound_pad, 0.3f, 0);
        PlaySfx("exp_thermalDet", points);
        GameCam_HitJudder();
        f32 radius = (f32)(AreaPickupScale * 0.5);
        EXPLOSION_PC *explosion = AddExplosion(&bolt->position, radius, 0.25f, NULL, -1, 0xa067);
        GAMEOBJECT *owner = static_cast<GAMEOBJECT *>(bolt->owner);
        if (explosion != NULL && Arcade != 0 && owner != NULL && (owner == Player[0] || owner == Player[1]) &&
            (owner->flags_1fc_word & 1) != 0 && (owner->flags_1fc_word & 0x1000) != 0 &&
            (owner->field_280 == 0 || owner->field_280 == 1)) {
            explosion->flags |= 0x10000;
            explosion->owner = static_cast<GAMEOBJECT *>(bolt->owner);
        }
    }
}
