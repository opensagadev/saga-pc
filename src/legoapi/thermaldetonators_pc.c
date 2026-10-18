#include "saga.h"
STATIC_ASSERT(offsetof(PART_PC, reflection_height) == 0x21c);
STATIC_ASSERT(offsetof(PART_PC, field_e0) == 0xe0);
STATIC_ASSERT(offsetof(PART_PC, unknown_209) == 0x209);
STATIC_ASSERT(offsetof(PART_PC, update_fn) == 0x1c4);
STATIC_ASSERT(offsetof(GAMEOBJECT, locator_matrices) == 0x804);
STATIC_ASSERT(offsetof(PLAYERCHARACTERCONFIG_PC, throw_locator) == 0x112);

void __cdecl PartImpact_ThermalDetonator(PART_PC *part) {
    i32 stuck = 0;
    i8 surface;
    u32 shadow;
    i32 layer;
    f32 water;
    PART_ACTION_PC stop;
    NUVEC trail;
    if ((part->active & 0x8000) != 0 || part->unknown_209 == 0x1c) {
        KillPart(part, 0);
        return;
    }
    surface = (i8)part->unknown_209;
    if (surface >= 0 && surface < 32 && (TerSurface[surface].flags & 0x1000) != 0) {
        PlaySfx("imp_thermalDet_attach", &part->position);
        stuck = 2;
    } else {
        GameShadow(NULL, &part->position, 5.0f, -1);
        shadow = ShadowInfo();
        if (shadow <= 31) {
            if ((TerSurface[shadow].flags & 0x1000) != 0) {
                PlaySfx("imp_thermalDet_attach", &part->position);
                stuck = 2;
            } else {
                water = EShadY;
                if ((f64)water != 2000000.0) {
                    layer = EShadowInfo();
                    if ((layer == 1 || layer == 9) && water > part->position.y) {
                        PlaySfx((char *)TerrainWaterJumpSoundNamePc, &part->position);
                        stuck = 1;
                    }
                }
            }
        }
    }
    if (stuck != 0) {
        stop = part->stop_fn;
        part->active |= 2;
        if (stop != NULL)
            stop(part);
    }
    if (stuck != 2 && KillPartsBrickImpactWaitPc <= 0.0f) {
        PartImpact_Brick(part);
        PlaySfx("ThermalDet_Bnce", &part->position);
    }
    if ((part->active & 3) == 1) {
        trail.x = part->impact_position.x - part->impact_normal.x * part->field_e0;
        trail.y = part->impact_position.y - part->impact_normal.y * part->field_e0;
        trail.z = part->impact_position.z - part->impact_normal.z * part->field_e0;
        AddGameDebris(WORLD->game_debris_sys, 1, &trail);
    }
}

void __cdecl PartUpdate_ThermalDetonator(PART_PC *part) {
    f32 height, water;
    i32 surface, layer;
    if ((part->active & 0x4002) == 2 && part->time_remaining > 0.0f && part->time_remaining < 1.0f) {
        PlaySfx("ThermalDet_Beep", &part->position);
        part->active |= 0x4000;
        return;
    }
    if ((part->active & 2) != 0)
        return;
    part->active &= ~0x20000;
    NewTerrPlatformsOff();
    height = GameShadow(NULL, &part->position, 5.0f, -1);
    if ((f64)height == 2000000.0)
        return;
    if (part->position.y > height) {
        surface = ShadowInfo();
        if ((u32)surface + 1 <= 17 && (TerSurface[surface].flags & 2) != 0) {
            part->reflection_height = height;
            part->active |= 0x20000;
        }
    }
    water = EShadY;
    if ((f64)water == 2000000.0)
        return;
    layer = EShadowInfo();
    if ((u32)layer > 16)
        return;
    if ((part->active & 0x8000) == 0 && water > part->position.y + part->field_e0 &&
        ((TerLayer[layer].flags & 1) != 0 || layer == 1 || layer == 9))
        part->active |= 0x8000;
    if (WORLD->current_level == DEATHSTARRESCUEA_LDATA && GameCam->sock == 2) {
        part->velocity.x = SeekValF(part->velocity.x, 0.0f, 3.0f);
        part->velocity.z = SeekValF(part->velocity.z, 0.0f, 3.0f);
    }
}

i32 __cdecl PartDraw_ThermalDetonator(PART_PC *part) {
    i32 draw = PartDraw_Flickerer(part);
    NUMTX matrix = part->transform;
    NUMTX reflection;
    i32 reflected = 0;
    i32 level_index = -1;
    if ((part->active & 0x20000) != 0 && (f64)part->reflection_height != 2000000.0)
        reflected =
            MatrixReflection(&matrix, 2, part->reflection_height, WORLD->current_level->reflection_y, &reflection) != 0;
    if (WORLD->level_objects[0xea].active != 0 && WORLD->level_objects[0xeb].active != 0) {
        level_index = draw != 0 ? 0xea : 0xeb;
        NuSpecialDrawAt(&WORLD->level_objects[level_index].special, &matrix);
    }
    if (reflected != 0) {
        NuRndrStartReflectionRender(1);
        if (part->source_special != NULL)
            NuSpecialDrawAt(&part->special, &reflection);
        if (level_index != -1)
            NuSpecialDrawAt(&WORLD->level_objects[level_index].special, &reflection);
        NuRndrEndReflectionRender();
    }
    return 1;
}

void __cdecl ThermalDetonator_Throw(GAMEOBJECT *object) {
    NUVEC movement, velocity, origin, centre;
    ADDPART_PC params = Default_ADDPART;
    NUMTX matrix;
    i32 blocked = 0;
    i32 locator;
    u16 angle;
    PART_PC *part;
    if (WORLD->level_objects[0xe9].active == 0)
        return;
    if ((object->flags_1fc & 0x80) != 0)
        Hint_SetComplete(0x2b8);
    locator = object->character_data->game_character->throw_locator;
    if (locator != -1 && object->character_model->points_of_interest[locator] != NULL) {
        if (object->id == id_JANGOFETT && object->field_7aa == 0x6e) {
            NuMtxSetRotationY(&matrix, qrand());
            NuMtxRotateZ(&matrix, qrand());
            NuMtxRotateX(&matrix, qrand());
            NuMtxTranslate(&matrix, &object->locator_matrices[locator].translation);
        } else {
            matrix = object->locator_matrices[locator];
            NuVecNorm((NUVEC *)&matrix.m00, (NUVEC *)&matrix.m00);
            NuVecNorm((NUVEC *)&matrix.m10, (NUVEC *)&matrix.m10);
            NuVecNorm((NUVEC *)&matrix.m20, (NUVEC *)&matrix.m20);
        }
        origin.x = object->collision_position.x;
        origin.y = (object->collision_max.y - object->collision_min.y) * 0.75 + object->collision_min.y;
        origin.z = object->collision_position.z;
        NuVecSub(&movement, &matrix.translation, &origin);
        blocked = NewRayCastScaleYMask(&origin, &movement, 0.0f, 1.0f, 0, GetSurfaceInfoDefaultMaskPc.word | 0x1f) != 0;
    } else {
        NuMtxSetTranslation(&matrix, &object->collision_position);
    }
    angle = object->movement_facing_angle;
    velocity.x = 2.0 * NU_SIN_LUT(angle) + object->velocity.x;
    velocity.y = 2.0f;
    velocity.z = 2.0 * NU_COS_LUT(angle) + object->velocity.z;
    if (blocked) {
        angle = (u16)NuAtan2D(object->collision_position.x - matrix.m30, object->collision_position.z - matrix.m32);
        velocity.x = 2.0 * NU_SIN_LUT(angle);
        velocity.z = 2.0 * NU_COS_LUT(angle);
        matrix.m30 = object->collision_position.x;
        matrix.m32 = object->collision_position.z;
    }
    params.matrix = &matrix;
    params.velocity = &velocity;
    params.owner = object;
    NuSpecialGetRadius(&WORLD->level_objects[0xe9].special, &centre, &params.field_14);
    params.field_18 = params.field_14;
    params.field_14 *= 0.75;
    params.field_c4 = 1;
    params.gravity = -5.0f;
    params.special = &WORLD->level_objects[0xe9].special;
    params.flags = 0x08000292;
    params.collision_fn = PartCollide_3D;
    params.kill_fn = PartKill_ThermalDetonator;
    params.impact_fn = PartImpact_ThermalDetonator;
    params.stop_fn = PartStop_Flickerer;
    params.draw_fn = PartDraw_ThermalDetonator;
    params.time_step = FRAMETIME;
    params.field_a4 = 10.0f;
    part = AddPart(&params);
    if (part != NULL) {
        part->force_flags = (u16)ObjHitObj_Flags(object);
        part->field_206 = 0;
        part->update_fn = PartUpdate_ThermalDetonator;
        part->reflection_height = 2000000.0f;
        part->active &= ~0x38000;
    }
    PlaySfx("ThrowDet", &object->collision_position);
    NewBuzzFrames(object->pad->bound_pad, 2, 0);
}
