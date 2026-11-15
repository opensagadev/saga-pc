#include "batman.h"
#include "gameapi/edptl.h"
#include "gameapi/edtools/edfile.h"
#include "globals.h"
#include "legoapi/actions/combat/hits.h"
#include "legoapi/ai/core/gameai.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/animlist.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/shard.h"
#include "legoapi/items/collect/thermaldetonators.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/supportall.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/props/objects/tightrope.h"
#include "legoapi/props/objects/zipup.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/render/light/surfaces.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/world.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

void PartImpact_ThermalDetonator(PART_PC *part) {
    i32 stuck = 0;
    i8 surface;
    u32 shadow;
    i32 layer;
    f32 water;
    PART_ACTION stop;
    NUVEC trail;
    if ((part->active & 0x8000) != 0) {
        KillPart(part, 0);
        return;
    }
    if (part->unknown_209 == 0x1c) {
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
                    if ((layer == 1 || layer == 9) && part->position.y < water) {
                        PlaySfx("FS_WaterJump", &part->position);
                        stuck = 1;
                    }
                }
            }
        }
    }
    if (stuck != 0) {
        stop = part->stop_fn;
        part->active |= 2;
        if (stop != NULL) {
            stop(part);
        }
    }
    if (stuck != 2 && brickimpactwait <= 0.0f) {
        PartImpact_Brick(part);
        PlaySfx("ThermalDet_Bnce", &part->position);
    }
    if ((part->active & PART_IN_USE) != 0 && (part->active & 2) == 0) {
        trail.x = part->impact_position.x - part->impact_normal.x * part->unknown_e0;
        trail.y = part->impact_position.y - part->impact_normal.y * part->unknown_e0;
        trail.z = part->impact_position.z - part->impact_normal.z * part->unknown_e0;
        AddGameDebris(WORLD->game_debris_sys, 1, &trail);
    }
}

void PartUpdate_ThermalDetonator(PART_PC *part) {
    f32 height;
    f32 water;
    i32 surface;
    i32 layer;
    if ((part->active & 0x4000) == 0 && (part->active & 2) != 0 && part->time_remaining > 0.0f && part->time_remaining < 1.0f) {
        PlaySfx("ThermalDet_Beep", &part->position);
        part->active |= 0x4000;
        return;
    }
    if ((part->active & 2) != 0) {
        return;
    }
    part->active &= ~0x20000;
    NewTerrPlatformsOff();
    height = GameShadow(NULL, &part->position, 5.0f, -1);
    if ((f64)height == 2000000.0) {
        return;
    }
    if (part->position.y > height) {
        surface = ShadowInfo();
        if ((u32)surface + 1 <= 17 && (TerSurface[surface].flags & 2) != 0) {
            part->reflection_height = height;
            part->active |= 0x20000;
        }
    }
    water = EShadY;
    if ((f64)water == 2000000.0) {
        return;
    }
    layer = EShadowInfo();
    if ((u32)layer > 16) {
        return;
    }
    if ((part->active & 0x8000) == 0 && part->position.y + part->unknown_e0 < water &&
        ((TerLayer[layer].flags & 1) != 0 || layer == 1 || layer == 9)) {
        part->active |= 0x8000;
    }
    if (WORLD->current_level == DEATHSTARRESCUEA_LDATA && GameCam->sock == 2) {
        part->velocity.x = SeekValF(part->velocity.x, 0.0f, 3.0f);
        part->velocity.z = SeekValF(part->velocity.z, 0.0f, 3.0f);
    }
}

i32 PartDraw_ThermalDetonator(PART_PC *part) {
    NUMTX matrix;
    i32 draw = PartDraw_Flickerer(part);
    NUMTX reflection;
    i32 reflected = 0;
    i32 level_index;
    matrix = part->transform;
    if ((part->active & 0x20000) != 0 && (f64)part->reflection_height != 2000000.0 &&
        MatrixReflection(&matrix, 2, part->reflection_height, WORLD->current_level->reflection_y, &reflection) != 0) {
        reflected = 1;
    }
    if (WORLD->level_objects[0xea].active != 0 && WORLD->level_objects[0xeb].active != 0) {
        level_index = draw != 0 ? 0xea : 0xeb;
        NuSpecialDrawAt(&WORLD->level_objects[level_index].special, &matrix);
    } else {
        level_index = -1;
    }
    if (reflected != 0) {
        NuRndrStartReflectionRender(1);
        if (part->source_special != NULL) {
            NuSpecialDrawAt(&part->special, &reflection);
        }
        if (level_index != -1) {
            NuSpecialDrawAt(&WORLD->level_objects[level_index].special, &reflection);
        }
        NuRndrEndReflectionRender();
    }
    return 1;
}

void PartKill_ThermalDetonator(PART_PC *part, i32 unused1) {
    u16 flags;
    EXPLOSION *explosion;
    if (part == NULL) {
        return;
    }

    if (part->owner != NULL) {
        AlertSurroundingCreatures(part->owner, &part->position);
    }

    flags = 0x200;
    if (part->owner != NULL && (part->owner->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && Cheat_IsOn(0x17) != 0) {
        flags = 0x1200;
    }

    explosion = Detonate(&part->position, flags);
    if (explosion != NULL && part->owner != NULL && (part->owner->unknown_280 == 0 || part->owner->unknown_280 == 1)) {
        explosion->player_index = part->owner->unknown_280;
    }
}

PC_SURFACE_MASK GetSurfaceInfoDefaultMask = {0x20};

void ThermalDetonator_Throw(GAMEOBJECT *object) {
    NUVEC movement;
    NUVEC velocity;
    NUVEC origin;
    NUVEC centre;
    ADDPART params = Default_ADDPART;
    NUMTX matrix;
    i32 blocked;
    i32 locator;
    u16 angle;
    PART_PC *part;
    if (WORLD->level_objects[0xe9].active == 0) {
        return;
    }
    if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0) {
        Hint_SetComplete(0x2b8);
    }
    locator = object->character_data->game_character->throw_locator;
    blocked = 0;
    if (locator != -1 && object->character_model->points_of_interest[locator] != NULL) {
        if (object->id == (u16)id_JANGOFETT && object->unknown_7aa == ACTION_THROW2) {
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
        if (NewRayCastScaleYMask(&origin, &movement, 0.0f, 1.0f, 0, GetSurfaceInfoDefaultMask.word | 0x1f) != 0) {
            blocked = 1;
        }
    } else {
        NuMtxSetTranslation(&matrix, &object->collision_position);
    }
    velocity.x = 2.0 * NU_SIN_LUT(object->movement_facing_angle) + object->velocity.x;
    velocity.y = 2.0f;
    velocity.z = 2.0 * NU_COS_LUT(object->movement_facing_angle) + object->velocity.z;
    if (blocked) {
        angle = (u16)NuAtan2D(object->collision_position.x - matrix.m30, object->collision_position.z - matrix.m32);
        velocity.x = 2.0 * NU_SIN_LUT(angle);
        velocity.z = 2.0 * NU_COS_LUT(angle);
        matrix.m30 = object->collision_position.x;
        matrix.m32 = object->collision_position.z;
    }
    params.matrix = &matrix;
    params.velocity = &velocity;
    NuSpecialGetRadius(&WORLD->level_objects[0xe9].special, &centre, &params.unknown_14);
    params.unknown_18 = params.unknown_14;
    params.unknown_14 *= 0.75;
    params.gravity = -5.0f;
    params.special = &WORLD->level_objects[0xe9].special;
    params.time_step = FRAMETIME;
    params.flags = 0x08000292;
    params.unknown_a4 = 10.0f;
    params.owner = object;
    params.unknown_c4 = 1;
    params.stop_fn = PartStop_Flickerer;
    params.draw_fn = PartDraw_ThermalDetonator;
    params.impact_fn = PartImpact_ThermalDetonator;
    params.kill_fn = PartKill_ThermalDetonator;
    params.collision_fn = PartCollide_3D;
    part = AddPart(&params);
    if (part != NULL) {
        part->force_flags = ObjHitObj_Flags(object);
        part->unknown_206 = 0;
        part->update_fn = PartUpdate_ThermalDetonator;
        part->reflection_height = 2000000.0f;
        part->active &= ~0x38000;
    }
    PlaySfx("ThrowDet", &object->collision_position);
    NewBuzzFrames(object->pad->bound_pad, 2, 0);
}

void ThermalDetonator_ThrowMom(GAMEOBJECT *object, NUVEC *velocity) {
    NUVEC movement;
    NUVEC origin;
    NUVEC centre;
    ADDPART params = Default_ADDPART;
    NUMTX matrix;
    i32 locator;
    PART_PC *part;
    if (WORLD->level_objects[0xe9].active == 0) {
        return;
    }
    locator = object->character_data->game_character->throw_locator;
    if (locator != -1 && object->character_model->points_of_interest[locator] != NULL) {
        if (object->id == (u16)id_JANGOFETT && object->unknown_7aa == ACTION_THROW2) {
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
        NewRayCastScaleYMask(&origin, &movement, 0.0f, 1.0f, 0, GetSurfaceInfoDefaultMask.word | 0x1f);
    } else {
        NuMtxSetTranslation(&matrix, &object->collision_position);
    }
    params.matrix = &matrix;
    params.velocity = velocity;
    NuSpecialGetRadius(&WORLD->level_objects[0xe9].special, &centre, &params.unknown_14);
    params.unknown_18 = params.unknown_14;
    params.unknown_14 *= 0.75;
    params.gravity = -5.0f;
    params.special = &WORLD->level_objects[0xe9].special;
    params.time_step = FRAMETIME;
    params.flags = 0x08000292;
    params.unknown_a4 = 10.0f;
    params.owner = object;
    params.unknown_c4 = 1;
    params.stop_fn = PartStop_Flickerer;
    params.draw_fn = PartDraw_ThermalDetonator;
    params.impact_fn = PartImpact_ThermalDetonator;
    params.kill_fn = PartKill_ThermalDetonator;
    params.collision_fn = PartCollide_3D;
    part = AddPart(&params);
    if (part != NULL) {
        part->force_flags = ObjHitObj_Flags(object);
        part->unknown_206 = 0;
        part->update_fn = PartUpdate_ThermalDetonator;
        part->reflection_height = 2000000.0f;
        part->active &= ~0x38000;
    }
    PlaySfx("ThrowDet", &object->collision_position);
    NewBuzzFrames(object->pad->bound_pad, 2, 0);
}

i32 ThermalDetonator_MoveCode(GAMEOBJECT *object) {
    f32 *frame = NULL;
    PART_PC *part;
    f32 release_frame;
    if (object->character_context == CHARACTER_CONTEXT_THROW_DETONATOR) {
        if (object->character_model->action_animations[object->unknown_7aa] != NULL) {
            frame = AnimPlaying(&object->anim_packet, object->unknown_7aa, 1, 0);
            if (frame == NULL) {
                return 0;
            }
        }
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f) {
            object->character_context = CHARACTER_CONTEXT_NONE;
            if ((object->context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0) {
                object->flags_e24 |= 0x4000;
            }
        } else if ((object->context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) == 0) {
            if (object->character_model->action_animations[object->unknown_7aa] != NULL) {
                release_frame = AnimListFrame(object->character_model, object->unknown_7aa, 2);
                if (frame != NULL && *frame > 0.0f && *frame >= release_frame) {
                    object->flags_e24 |= 0x4000;
                }
            } else if (object->context_timer < 0.5f) {
                object->flags_e24 |= 0x4000;
            }
        }
        if ((object->flags_e24 & 0x4000) != 0) {
            object->context_variant_flags |= APIOBJECT_CONTEXT_EVENT_DONE;
        }
        return 0;
    }
    if (WORLD->level_objects[0xe9].active == 0 || (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0 ||
        (object->pad->buttons_pressed & GAMEPAD_SPECIAL) == 0) {
        return 0;
    }
    if (((object->character_data->model_flags & CHARACTER_MODEL_BOUNTY_HUNTER) == 0 && object->helmet != 6 && !SuperWeirdo(object)) ||
        (object->unknown_281 == 0 && object->jump_variant_state != 1)) {
        return 0;
    }
    if (object->character_context != CHARACTER_CONTEXT_NONE && object->character_context != CHARACTER_CONTEXT_WEAPON_IN &&
        object->character_context != CHARACTER_CONTEXT_WEAPON_OUT && (U64_PAIR(&CInfo[object->character_context].flags) & 4) == 0) {
        return 0;
    }
    part = FindPart(NULL, 0, object);
    if (part != NULL) {
        if ((part->active & 2) != 0 && !(part->time_remaining > 1.0f)) {
            return 0;
        }
        KillPart(part, 0);
        return 1;
    }
    object->character_context = CHARACTER_CONTEXT_THROW_DETONATOR;
    if (object->jump_variant_state == 1) {
        object->unknown_7aa = ACTION_THROW3;
    } else if ((object->flags_e20 & APIOBJECT_WEAPON_OUT) != 0) {
        object->unknown_7aa = ACTION_THROW2;
    } else {
        object->unknown_7aa = ACTION_THROW;
    }
    if (object->character_model->action_animations[object->unknown_7aa] == NULL) {
        object->unknown_7aa = ACTION_THROW;
    }
    if (AnimPlaying(&object->anim_packet, object->unknown_7aa, 1, 1) != NULL) {
        ResetAnimPacket(&object->anim_packet, -1);
    }
    object->context_variant_flags &= ~APIOBJECT_CONTEXT_EVENT_DONE;
    object->context_timer = object->character_model->action_animations[object->unknown_7aa] != NULL
                                ? AnimDuration((i16)object->id, object->unknown_7aa, 0.0f, 0.0f, 1)
                                : 1.0f;
    return 0;
}

i32 ShinyMetal_UpdateHint(HINT *hint) {
    (void)hint;
    if (JABBASPALACE_ADATA == NULL || WORLD->current_level != JABBASPALACEB_LDATA ||
        Game.area_save[JABBASPALACE_ADATA->index].area_complete != 0 || Mission_Active(NULL) != NULL) {
        return 0;
    }
    if (LevGizObst[0] != NULL && LevGizObst[0]->anim_set->state == 0) {
        return 0;
    }
    if (LevBlowUp[0] == NULL || (LevBlowUp[0]->status_flags & BLOWUP_STATUS_BLOWN_UP) != 0) {
        return 0;
    }
    if ((player != NULL && (f64)NuVecDistSqr(&player->collision_position, &LevBlowUp[0]->mid_position, NULL) < 25.0) ||
        (player2 != NULL && (f64)NuVecDistSqr(&player2->collision_position, &LevBlowUp[0]->mid_position, NULL) < 25.0)) {
        return 1;
    }
    return 0;
}

static i32 TightRopes_Load(void *world_info, void *unused1) {
    i32 version;
    i32 i;
    WORLDINFO *world = world_info;
    if (world->tightrope_count != 0) {
        return 0;
    }
    version = EdFileReadInt();
    world->tightrope_count = EdFileReadInt();
    for (i = 0; i < world->tightrope_count; ++i) {
        EdFileRead(world->tightropes[i].name, 16);
        EdFileReadNuVec(&world->tightropes[i].start_position);
        EdFileReadNuVec(&world->tightropes[i].end_position);
        if (version > 1) {
            world->tightropes[i].unknown_28 = EdFileReadUnsignedShort();
            world->tightropes[i].unknown_2a = EdFileReadUnsignedShort();
            world->tightropes[i].unknown_2c = EdFileReadUnsignedShort();
            world->tightropes[i].unknown_2e = EdFileReadUnsignedShort();
            world->tightropes[i].unknown_30 = EdFileReadUnsignedChar();
            world->tightropes[i].unknown_31 = EdFileReadUnsignedChar();
            if (version != 2) {
                world->tightropes[i].unknown_32 = EdFileReadChar();
            }
        }
    }
    return 1;
}
