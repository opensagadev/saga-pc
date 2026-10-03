#include "saga.h"

struct KILLPARTS_MINIKIT_PIECE_PC {
    NUSPECIALHANDLE special;
    NUMTX matrix;
    u8 unknown_4c[0x1c];
};
STATIC_ASSERT(sizeof(KILLPARTS_MINIKIT_PIECE_PC) == 0x68);
STATIC_ASSERT(offsetof(KILLPARTS_MINIKIT_PIECE_PC, matrix) == 0xc);

struct KILLPARTS_MINIKIT_PIECES_PC {
    u8 unknown_0[4];
    KILLPARTS_MINIKIT_PIECE_PC *pieces;
    u8 piece_count;
};
STATIC_ASSERT(offsetof(KILLPARTS_MINIKIT_PIECES_PC, pieces) == 4);
STATIC_ASSERT(offsetof(KILLPARTS_MINIKIT_PIECES_PC, piece_count) == 8);

struct KILLPARTS_RENDER_PC {
    const char *name;
    NUSPECIALHANDLE **rigid_specials;
    u8 unknown_8[0xc];
};
STATIC_ASSERT(sizeof(KILLPARTS_RENDER_PC) == 0x14);
STATIC_ASSERT(offsetof(KILLPARTS_RENDER_PC, rigid_specials) == 4);

struct KILLPARTS_OBJECT_LIGHT_PC {
    u8 unknown_0[0x4d8];
    PART_LIGHTING_DATA_PC lighting;
};
STATIC_ASSERT(offsetof(KILLPARTS_OBJECT_LIGHT_PC, lighting) == 0x4d8);

struct KILLPARTS_WORLD_SCENES_PC {
    u8 unknown_0[0x2980];
    KILLPARTS_CHARSCENE_PC *character_scenes;
};
STATIC_ASSERT(offsetof(KILLPARTS_WORLD_SCENES_PC, character_scenes) == 0x2980);

struct KILLPARTS_SAVED_PIECES_PC {
    i16 pieces[9];
    u8 unknown_12[0x26];
};
STATIC_ASSERT(sizeof(KILLPARTS_SAVED_PIECES_PC) == 0x38);

struct KILLPARTS_CUSTOMISER_PC {
    u8 unknown_0[0x24];
    CUSTOMPIECE_LAYERS_PC *head_pieces;
    CUSTOMPIECE_LAYERS_PC *helmet_pieces;
    u8 unknown_2c[0x40];
    i16 character_ids[2];
    u8 unknown_70[0x104];
    KILLPARTS_SAVED_PIECES_PC *save;
};
STATIC_ASSERT(offsetof(KILLPARTS_CUSTOMISER_PC, head_pieces) == 0x24);
STATIC_ASSERT(offsetof(KILLPARTS_CUSTOMISER_PC, helmet_pieces) == 0x28);
STATIC_ASSERT(offsetof(KILLPARTS_CUSTOMISER_PC, character_ids) == 0x6c);
STATIC_ASSERT(offsetof(KILLPARTS_CUSTOMISER_PC, save) == 0x174);

struct KILLPARTS_PART_PC {
    u8 unknown_0[0x20c];
    f32 speeder_index;
};
STATIC_ASSERT(offsetof(KILLPARTS_PART_PC, speeder_index) == 0x20c);

struct KILLPARTS_CAMERA_PC {
    u8 unknown_0[0x68];
    NUMTX mtx;
};
STATIC_ASSERT(offsetof(KILLPARTS_CAMERA_PC, mtx) == 0x68);

struct KILLPARTS_RIDER_LIGHT_PC {
    u8 unknown_0[0x628];
    NUVEC intensity[2];
};
STATIC_ASSERT(offsetof(KILLPARTS_RIDER_LIGHT_PC, intensity) == 0x628);

#define NU_SIN_LUT(angle) NuTrigTable[(static_cast<i32>(angle) >> 1) & 0x7fff]
#define NU_COS_LUT(angle) NuTrigTable[((static_cast<i32>(angle) + 0x4000) >> 1) & 0x7fff]

extern "C" {
    void __cdecl AddPartDebris(PARTDEBRISSYS *, i32, NUVEC *);
    void KillParts(GAMEOBJECT *object, i32 animation, i32 excluded_layer, i32 mode, float vertical_scale,
                   i32 use_vehicle_velocity, u16 *angle_override) {
        CHARACTERDATA_PC *character = object->character_data;
        PLAYERCHARACTERCONFIG_PC *game_character = character->game_character;
        if ((character->model_flags & 0x02000000) != 0 || (game_character->flags_090 & 0x8000) != 0)
            return;
        if (static_cast<i16>(object->id) == id_BOBAFETT && WORLD->current_level == SARLACCPITA_LDATA && FreePlay == 0 &&
            static_cast<u8>(object->field_280) == 0xff)
            return;

        GameAudio_PlaySfx(0x4f, &object->collision_position, 0, 0);

        if ((character->model_flags & 0x04000000) != 0 && static_cast<i16>(object->id) != id_MINIDROIDEKA) {
            KILLPARTS_MINIKIT_PIECES_PC *pieces =
                reinterpret_cast<KILLPARTS_MINIKIT_PIECES_PC **>(Char_MiniKit)[static_cast<i16>(object->id)];
            if (pieces == NULL || pieces->piece_count == 0)
                return;
            for (i32 i = 0; i < pieces->piece_count; ++i) {
                KILLPARTS_MINIKIT_PIECE_PC *piece = &pieces->pieces[i];
                if (!NuSpecialExistsFn(&piece->special))
                    continue;
                NUMTX matrix;
                NuMtxMul(&matrix, &piece->matrix, &object->transform);
                NUVEC momentum;
                SetKillPartMom(&momentum);
                NuVecScale(&momentum, &momentum, 3.0f);
                momentum.x += object->velocity.x * 0.75f;
                momentum.y += 5.0f;
                momentum.y += object->velocity.y * 0.75f;
                momentum.z += object->velocity.z * 0.75f;
                ADDPART_PC params = Default_ADDPART;
                params.matrix = &matrix;
                params.velocity = &momentum;
                params.field_14 = params.field_18 = 0.2f;
                params.gravity = -10.0f;
                params.special = &piece->special;
                params.flags = mode == 0 ? 0x480 : 0x90;
                params.impact_fn = PartImpact_Brick;
                params.stop_fn = PartStop_Flickerer;
                params.draw_fn = PartDraw_Flickerer;
                params.time_step = FRAMETIME;
                params.lighting_data = Cheats_CheckFlags(1)
                                           ? reinterpret_cast<PART_LIGHTING_DATA_PC *>(&KillPartsZeroLightingPc)
                                           : reinterpret_cast<PART_LIGHTING_DATA_PC *>(
                                                 &reinterpret_cast<KILLPARTS_OBJECT_LIGHT_PC *>(object)->lighting);
                params.field_c4 = 1;
                PART_PC *part = AddPart(&params);
                if (part != NULL)
                    part->time_remaining = static_cast<f64>(qrand()) * PlayerRandomScalePc * 2.0f + 7.0f;
            }
            return;
        }

        const u16 random_rotation_a =
            static_cast<u16>(static_cast<i32>(static_cast<f64>(qrand()) * PlayerRandomScalePc * 25486.0f - 12743.0f));
        const u16 random_rotation_b =
            static_cast<u16>(static_cast<i32>(static_cast<f64>(qrand()) * PlayerRandomScalePc * 16384.0f - 8192.0f));

        i32 random_variant = qrand() / 21846;
        for (i32 retry = 0; retry <= 8 && random_variant == KillPartsPreviousVariantPc; ++retry)
            random_variant = qrand() / 21846;
        KillPartsPreviousVariantPc = random_variant;

        u32 layers = (game_character->flags_094_bytes[2] & 4) != 0
                         ? object->layer_mask
                         : AdjustLayerBits(game_character->layer_mask_dead, object);
        if (animation != -1)
            layers = 1u << animation;
        else if (excluded_layer != -1)
            layers &= ~(1u << excluded_layer);

        NUMTX joint_matrices[256];
        i16 render_indices[32];
        CHARACTER_MODEL_LAYERS_PC *model = object->character_model;
        EvalModelAnim(model, &object->anim_packet, &object->transform, joint_matrices, NULL, NULL, NULL, layers);
        const i32 render_count = game_character->make_layer_list(model, render_indices, layers);
        i32 part_index = 0;
        for (i32 render_index = 0; render_index < render_count; ++render_index) {
            const i32 layer = render_indices[render_index];
            if (layer == -1 || (animation != -1 && layer != animation) ||
                (excluded_layer != -1 && layer == excluded_layer))
                continue;
            KILLPARTS_RENDER_PC *render =
                reinterpret_cast<KILLPARTS_RENDER_PC *>(&model->hierarchy->render_parts[layer]);
            if (render->rigid_specials == NULL)
                continue;
            for (i32 joint = 0; joint < model->hierarchy->joint_count; ++joint, ++part_index) {
                NUSPECIALHANDLE *special = static_cast<NUSPECIALHANDLE *>(render->rigid_specials[joint]);
                if (special == NULL)
                    continue;
                NUMTX matrix;
                NUVEC momentum;
                ADDPART_PC params;
                if (VehicleArea != 0 && (character->model_flags & 0x2000) != 0 && game_character->field_028 > 0.0f) {
                    NuMtxMul(&matrix, &joint_matrices[joint], &object->transform);
                    SetKillPartMom(&momentum);
                    NuVecScale(&momentum, &momentum, 4.0f);
                    momentum.x += object->velocity.x * 0.75f;
                    momentum.y += 6.0f;
                    momentum.y += object->velocity.y * 0.75f;
                    momentum.z += object->velocity.z * 0.75f;
                    params = Default_ADDPART;
                    params.field_14 = params.field_18 = 0.4f;
                    params.gravity = -10.0f;
                } else {
                    SetKillPartMom(&momentum);
                    momentum.y += vertical_scale;
                    if (animation != -1)
                        momentum.y += 1.0f;
                    if ((character->model_flags & 0x2000) != 0) {
                        momentum.x += object->velocity.x * 0.75f;
                        momentum.y += object->velocity.y * 0.75f;
                        momentum.z += object->velocity.z * 0.75f;
                    } else if (use_vehicle_velocity != 0) {
                        momentum.x += object->reset_velocity.x * 0.75f;
                        momentum.y += object->reset_velocity.y * 0.75f;
                        momentum.z += object->reset_velocity.z * 0.75f;
                    } else if (angle_override != NULL) {
                        momentum.x += NuTrigTable[(*angle_override >> 1) & 0x7fff] * 2.0f;
                        momentum.z += NuTrigTable[((static_cast<u16>(*angle_override + 0x4000)) >> 1) & 0x7fff] * 2.0f;
                    }
                    NuMtxMul(&matrix, &joint_matrices[joint], &object->transform);
                    params = Default_ADDPART;
                    params.field_14 = params.field_18 = 0.1f;
                    params.gravity = -5.0f;
                }
                params.matrix = &matrix;
                params.velocity = &momentum;
                params.special = special;
                params.lighting_data = Cheats_CheckFlags(1)
                                           ? reinterpret_cast<PART_LIGHTING_DATA_PC *>(&KillPartsZeroLightingPc)
                                           : reinterpret_cast<PART_LIGHTING_DATA_PC *>(
                                                 &reinterpret_cast<KILLPARTS_OBJECT_LIGHT_PC *>(object)->lighting);
                if (static_cast<i16>(object->id) == id_TIEFIGHTER) {
                    KillParts_TIEFIGHTER(&params, part_index, mode, object, random_variant, random_rotation_a,
                                         random_rotation_b, &momentum);
                } else if (static_cast<i16>(object->id) == id_ATAT) {
                    KillParts_ATAT(&params, part_index, mode, object);
                } else if (static_cast<i16>(object->id) == id_SPEEDERBIKE) {
                    KillParts_SpeederBike(&params, part_index, mode, object);
                } else {
                    params.flags = mode == 0 ? 0x480 : 0x90;
                    params.impact_fn = PartImpact_Brick;
                    params.stop_fn = PartStop_Flickerer;
                    params.draw_fn = PartDraw_Flickerer;
                    params.time_step = FRAMETIME;
                    params.field_c4 = 1;
                    PART_PC *part = AddPart(&params);
                    if (part != NULL)
                        part->time_remaining = static_cast<f64>(qrand()) * PlayerRandomScalePc * 2.0f + 7.0f;
                }
            }
        }

        if ((character->flags & 1) != 0 && VehicleArea != 0 && (character->model_flags & 0x2000) != 0 &&
            game_character->field_028 > 0.0f) {
            NUSPECIALHANDLE *special = CharScene_FindHSpecial(WORLD, static_cast<i16>(object->id));
            if (special != NULL) {
                NUMTX matrix = object->transform;
                NUVEC momentum;
                SetKillPartMom(&momentum);
                NuVecScale(&momentum, &momentum, 4.0f);
                momentum.x += object->velocity.x * 0.75f;
                momentum.y += 6.0f;
                momentum.y += object->velocity.y * 0.75f;
                momentum.z += object->velocity.z * 0.75f;
                ADDPART_PC params = Default_ADDPART;
                params.matrix = &matrix;
                params.velocity = &momentum;
                params.field_14 = 0.8f;
                params.field_18 = 0.4f;
                params.gravity = -10.0f;
                params.special = special;
                params.flags = mode == 0 ? 0x480 : 0x90;
                params.impact_fn = PartImpact_Brick;
                params.stop_fn = PartStop_Flickerer;
                params.draw_fn = PartDraw_Flickerer;
                params.time_step = FRAMETIME;
                params.lighting_data = Cheats_CheckFlags(1)
                                           ? reinterpret_cast<PART_LIGHTING_DATA_PC *>(&KillPartsZeroLightingPc)
                                           : reinterpret_cast<PART_LIGHTING_DATA_PC *>(
                                                 &reinterpret_cast<KILLPARTS_OBJECT_LIGHT_PC *>(object)->lighting);
                params.field_c4 = 1;
                PART_PC *part = AddPart(&params);
                if (part != NULL)
                    part->time_remaining = static_cast<f64>(qrand()) * PlayerRandomScalePc * 2.0f + 3.0f;
            }
        }
        if (game_character->weapon_action == 0)
            Customiser_AddPartAccessories(CharacterCustomiser, object, animation, mode, vertical_scale);
    }

    void __cdecl SetKillPartMom(NUVEC *momentum) {
        momentum->x = 0.0f;
        momentum->y = 1.0f;
        momentum->z = 0.0f;
        NuVecRotateZ(momentum, momentum, qrand());
        NuVecRotateX(momentum, momentum, qrand());
    }
    void __cdecl PartImpact_Brick(PART_PC *part) {
        if (KillPartsBrickImpactWaitPc <= 0.0f) {
            f32 speed = NuVecMag(&part->velocity);
            if (speed > 0.0f) {
                if (speed > 1.0f)
                    speed = 1.0f;
                const i32 angle = static_cast<i32>((1.0 - speed) * 16384.0 + 16384.0);
                const f32 volume = NuTrigTable[(angle >> 1) & 0x7fff];
                PlaySfxByIdAndSetVolume(GetSfxId(KillPartsLegoSingleTextPc), &part->position, volume);
                KillPartsBrickImpactWaitPc = 0.1f;
            }
        }
    }

    NUSPECIALHANDLE *__cdecl CharScene_FindHSpecial(WORLDINFO *world, i32 character_id) {
        KILLPARTS_CHARSCENE_PC *scene;
        if (KillPartsCharSceneAreaPc != NULL && KillPartsCharSceneAreaPc[character_id].scene != NULL)
            scene = &KillPartsCharSceneAreaPc[character_id];
        else {
            scene = &reinterpret_cast<KILLPARTS_WORLD_SCENES_PC *>(world)->character_scenes[character_id];
            if (scene->scene == NULL)
                scene = NULL;
        }
        if (scene != NULL && NuSpecialExistsFn(&scene->special_scene) == 0)
            scene = NULL;
        return scene == NULL ? NULL : &scene->special_scene;
    }
    void __cdecl Customiser_AddPartAccessories(CUSTOMISER_LAYERS_PC *customiser, GAMEOBJECT *object, i32 animation,
                                               i32 mode, f32 scale) {
        const i32 joint = object->character_data->game_character->helmet_locator;
        if (customiser == NULL || joint == -1 || object->character_model->points_of_interest[joint] == NULL)
            return;
        KILLPARTS_CUSTOMISER_PC *accessories = reinterpret_cast<KILLPARTS_CUSTOMISER_PC *>(customiser);
        const i32 side = static_cast<i16>(object->id) != accessories->character_ids[0];
        const i16 *pieces = accessories->save[side].pieces;
        for (i32 category = 0; category < 9; ++category) {
            if (category == 2)
                continue;
            if (category == 0 && ((accessories->helmet_pieces[static_cast<u16>(pieces[1])].layer_flags & 1) != 0 ||
                                  (accessories->head_pieces[static_cast<u16>(pieces[0])].layer_flags & 0x20) != 0))
                continue;
            NUSPECIALHANDLE *special = &KillPartsAccessoriesPc[side][category].special;
            if (!NuSpecialExistsFn(special))
                continue;
            NUVEC momentum;
            SetKillPartMom(&momentum);
            momentum.y += scale;
            if (animation != -1)
                momentum.y += 1.0f;
            ADDPART_PC params = Default_ADDPART;
            params.matrix = &object->locator_matrices[joint];
            params.velocity = &momentum;
            params.field_14 = params.field_18 = 0.1f;
            params.gravity = -5.0f;
            params.special = special;
            params.flags = mode == 0 ? 0x480 : 0x90;
            params.impact_fn = PartImpact_Brick;
            params.stop_fn = PartStop_Flickerer;
            params.draw_fn = PartDraw_Flickerer;
            params.time_step = FRAMETIME;
            params.lighting_data = Cheats_CheckFlags(1)
                                       ? &KillPartsZeroLightingPc
                                       : &reinterpret_cast<KILLPARTS_OBJECT_LIGHT_PC *>(object)->lighting;
            AddPart(&params);
        }
    }

    void __cdecl KillParts_TIEFIGHTER(ADDPART_PC *params, i32 part_index, i32 variant, GAMEOBJECT *object, i32 mode,
                                      u16 xrot, u16 yrot, NUVEC *velocity) {
        params->flags = variant == 0 ? 0x400 : 0x10;
        if (static_cast<f64>(object->horizontal_velocity_magnitude) /
                object->character_data->game_character->run_speed <=
            0.25) {
            params->flags = 0x90;
            params->stop_fn = PartStop_Flickerer;
            params->draw_fn = PartDraw_Flickerer;
            params->impact_fn = PartImpact_Brick;
            params->gravity = -8.0f;
            params->velocity = velocity;
            AddPart(params);
            return;
        }
        if (mode == 0) {
            params->kill_fn = TiePart_KillExplode;
            params->flags = 0x111;
            params->field_a4 = FRAMETIME;
            params->velocity = &v000;
            AddPart(params);
            return;
        }
        if (mode == 1) {
            NUVEC spin_velocity = {object->velocity.x * 0.75f, object->velocity.x * 0.75f, object->velocity.z * 0.75f};
            if (part_index == 5) {
                NuVecRotateX(&spin_velocity, &spin_velocity, -static_cast<i32>(xrot));
                NuVecRotateY(&spin_velocity, &spin_velocity, -static_cast<i32>(yrot));
                params->move_fn = TiePart_Move;
            } else {
                NuVecRotateX(&spin_velocity, &spin_velocity, xrot);
                NuVecRotateY(&spin_velocity, &spin_velocity, yrot);
                params->move_fn = TieSpinZPart_Move;
            }
            params->flags = 0x111;
            params->field_a4 = 2.0f;
            params->velocity = &spin_velocity;
            params->kill_fn = TiePart_Kill;
            params->impact_fn = TiePart_Impact;
            PlaySfx(KillPartsTieSpinsTextPc, &object->collision_position);
            AddPart(params);
            return;
        }
        params->field_20 = 0.5f;
        params->kill_fn = TiePart_Kill;
        params->flags = 0x91;
        params->draw_fn = PartDraw_Flickerer;
        params->impact_fn = TiePart_Impact;
        params->velocity = velocity;
        AddPart(params);
    }
    void __cdecl KillParts_ATAT(ADDPART_PC *params, i32, i32 variant, GAMEOBJECT *) {
        params->flags = variant == 0 ? 0x500 : 0x110;
        params->velocity->z = 0.0f;
        params->velocity->y = 0.0f;
        params->velocity->x = 0.0f;
        params->update_fn = AtatPart_Update;
        params->stop_fn = AtatPart_Stop;
        params->draw_fn = PartDraw_Flickerer;
        PART_PC *part = AddPart(params);
        if (part != NULL)
            part->time_remaining = 10.0f;
    }
    void __cdecl KillParts_SpeederBike(ADDPART_PC *params, i32, i32, GAMEOBJECT *object) {
        NUVEC velocity = {object->velocity.x * 1.5f, 0.0f, object->velocity.z * 1.5f};
        params->flags = 0x400;
        params->kill_fn = SpeederPart_Kill;
        params->update_fn = SpeederPart_Update;
        params->draw_fn = SpeederPart_Draw;
        params->velocity = &velocity;
        PART_PC *part = AddPart(params);
        if (part == NULL)
            return;
        part->time_remaining = part->initial_time = 1.5f;
        KILLPARTS_PART_PC *bike_part = reinterpret_cast<KILLPARTS_PART_PC *>(part);
        i32 index;
        for (index = 0; index < 4; ++index) {
            if (KillPartsBikesPc[index].active == 0)
                break;
        }
        bike_part->speeder_index = index == 4 ? -1.0f : static_cast<f32>(index);
        if (bike_part->speeder_index == -1.0f || object->movement_spline_position.spline == NULL) {
            KillPart(part, 0);
            return;
        }
        f32 distance = NuVecMag(&part->velocity) * FRAMETIME;
        KILLPARTS_BIKE_PC *bike = &KillPartsBikesPc[static_cast<i32>(bike_part->speeder_index)];
        bike->spline_position = object->movement_spline_position;
        bike->active = 1;
        bike->rider = object->rider;
        if (bike->rider != NULL) {
            bike->rider->velocity = velocity;
            bike->rider->velocity.y += 1.5f;
            bike->rider_position = object->position;
        }
        NUVEC position;
        u16 yaw, pitch;
        PointAlongSpline(bike->spline_position.spline, bike->spline_position.along, &position, &yaw, &pitch, 1);
        yaw -= 0x8000;
        if (distance < 0.0f) {
            yaw += 0x8000;
            pitch = -pitch;
        }
        part->rotation_y = yaw;
        part->rotation_x = -pitch;
        part->rotation_z = 0;
    }

    void __cdecl TiePart_Kill(PART_PC *part, i32) {
        AddGameDebris(WORLD->game_debris_sys, 0x6a, &part->position);
    }
    void __cdecl TiePart_Impact(PART_PC *part) {
        AddGameDebris(WORLD->game_debris_sys, 0x6a, &part->position);
    }
    void __cdecl TiePart_KillExplode(PART_PC *part, i32) {
        AddGameDebris(WORLD->game_debris_sys, 0x6b, &part->position);
        AddPartDebris(WORLD->part_debris_sys, 3, &part->position);
    }
    /* Native local helper takes the particle in ESI and time on the stack. This
     * logical signature is private to the coherent compiler-generated caller group. */
    static void __cdecl TiePart_MoveCorePC(PART_PC *part, f32 time) {
        NUVEC position;
        position.x = part->position.x + part->velocity.x * time;
        position.y = part->position.y + part->velocity.y * time;
        position.z = part->position.z + part->velocity.z * time;
        NuMtxRotateZ(&part->transform, part->rotation_z);
        part->position = position;
        AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[100].effect, &position, 10, FRAMETIME, 0, 0,
                                          NULL);
    }
    void __cdecl TiePart_Move(PART_PC *part, f32 time) {
        part->field_124[3] = -32768;
        part->rotation_z = static_cast<u16>(static_cast<i32>(-32768.0 * FRAMETIME));
        TiePart_MoveCorePC(part, time);
    }
    void __cdecl TieSpinZPart_Move(PART_PC *part, f32 time) {
        part->field_124[3] = KillPartsTieSpinRatePc;
        part->rotation_z = static_cast<u16>(static_cast<i32>(static_cast<f64>(KillPartsTieSpinRatePc) * FRAMETIME));
        NuMtxPreTranslate(&part->transform, &KillPartsTieSpinTranslationPc);
        TiePart_MoveCorePC(part, time);
        NUVEC trail;
        trail.x = part->velocity.x * KillPartsTieTrailScalePc;
        trail.y = part->velocity.y * KillPartsTieTrailScalePc;
        trail.z = part->velocity.z * KillPartsTieTrailScalePc;
        AddVariableShotDebrisEffectTimed3(WORLD->game_debris_sys->entries[96].effect, &part->position, &trail, 10,
                                          FRAMETIME, NULL, NULL);
    }
    void __cdecl AtatPart_Stop(PART_PC *part) {
        PlaySfx(KillPartsAtatExplodeTextPc, &part->position);
        PartStop_Flickerer(part);
    }
    void AtatPart_Update(PART_PC *part) {
        f32 choice = NuFloatRand(reinterpret_cast<NURAND *>(&GAMERAND)) * 100.0f + 1.0f;
        if (part->scale_time < 1.0f) {
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[118].effect, &part->position,
                                              static_cast<i32>(part->scale_time * 20.0f), FRAMETIME, 0, 0, NULL);
        } else if (part->scale_time < 2.0f) {
            if (choice < 10.0f) {
                i16 y_rotation = qrand();
                i16 z_rotation = qrand();
                AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[120].effect, &part->position,
                                                  static_cast<i32>(part->scale_time * 3.0f), FRAMETIME, z_rotation,
                                                  y_rotation, NULL);
            } else if (choice > 90.0f) {
                i16 y_rotation = qrand();
                i16 z_rotation = qrand();
                AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[120].effect, &part->position,
                                                  static_cast<i32>(part->scale_time * 15.0f), FRAMETIME, z_rotation,
                                                  y_rotation, NULL);
            }
        } else if (part->scale_time < 5.0f) {
            if (choice < 10.0f || choice > 70.0f) {
                i32 count = choice < 10.0f ? 15 : 30;
                i16 y_rotation = qrand();
                i16 z_rotation = qrand();
                AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[120].effect, &part->position, count,
                                                  FRAMETIME, z_rotation, y_rotation, NULL);
            }
            i32 count = choice < 30.0f ? 1 : choice < 70.0f ? 3 : 20;
            i16 y_rotation = qrand();
            i16 z_rotation = qrand();
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[119].effect, &part->position, count,
                                              FRAMETIME, z_rotation, y_rotation, NULL);
        } else if (part->scale_time < 10.0f) {
            i32 count = choice < 30.0f ? 1 : choice < 70.0f ? 3 : 20;
            i16 y_rotation = qrand();
            i16 z_rotation = qrand();
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[119].effect, &part->position, count,
                                              FRAMETIME, z_rotation, y_rotation, NULL);
        }
    }

    i32 SpeederPart_Draw(PART_PC *) {
        return 1;
    }

    void SpeederPart_Kill(PART_PC *part, i32) {
        AddGameDebris(WORLD->game_debris_sys, 0x6a, &part->position);
        if (reinterpret_cast<KILLPARTS_PART_PC *>(part)->speeder_index != -1.0f) {
            GAMEOBJECT *rider =
                KillPartsBikesPc[static_cast<i32>(reinterpret_cast<KILLPARTS_PART_PC *>(part)->speeder_index)].rider;
            if (rider != NULL && (rider->flags_1fc_word & 0x1001) == 0x1001) {
                rider->flags_ef0 &= ~0x400000;
                KillParts(KillPartsBikesPc[static_cast<i32>(reinterpret_cast<KILLPARTS_PART_PC *>(part)->speeder_index)]
                              .rider,
                          -1, -1, 1, 0.0f, 0, NULL);
                KillGameObject(
                    KillPartsBikesPc[static_cast<i32>(reinterpret_cast<KILLPARTS_PART_PC *>(part)->speeder_index)]
                        .rider,
                    2, 0);
            }
            memset(&KillPartsBikesPc[static_cast<i32>(reinterpret_cast<KILLPARTS_PART_PC *>(part)->speeder_index)], 0,
                   sizeof(KILLPARTS_BIKE_PC));
        }
    }

    void SpeederPart_Update(PART_PC *part) {
        f32 time = part->time_remaining / part->initial_time;
        f32 distance = reinterpret_cast<GAMEOBJECT *>(Player[0])->horizontal_velocity_magnitude * FRAMETIME *
                       static_cast<f64>(1.1f);
        KILLPARTS_BIKE_PC *bike =
            &KillPartsBikesPc[static_cast<i32>(reinterpret_cast<KILLPARTS_PART_PC *>(part)->speeder_index)];
        KILLPARTS_CAMERA_PC *camera = reinterpret_cast<KILLPARTS_CAMERA_PC *>(GameCam);
        NUVEC previous_position;
        NUVEC position;
        NUVEC velocity;
        NUMTX matrix;
        if (bike->spline_position.spline != NULL) {
            previous_position = bike->spline_position.position;
            MoveSplinePosition(&bike->spline_position, distance);
            u16 yaw, pitch;
            PointAlongSpline(bike->spline_position.spline, bike->spline_position.along, &position, &yaw, &pitch, 1);
            yaw -= 0x8000;
            if (distance < 0.0f) {
                yaw += 0x8000;
                pitch = -pitch;
            }
            f32 tilt = NU_SIN_LUT(static_cast<i32>(time * 32768.0f + 16384.0f));
            i32 spin = static_cast<i32>((NU_SIN_LUT(static_cast<i32>(time * 16384.0f + 32768.0f + 16384.0f)) + 1.0f) *
                                        196608.0f);
            part->rotation_x =
                SeekRot(part->rotation_x, static_cast<i32>(-((1.0f - (tilt + 1.0f) * 0.5f) * 5461.0f)) - pitch, 10.0f);
            part->rotation_y =
                SeekRot(part->rotation_y,
                        yaw + static_cast<i32>(2730.0f - (1.0f - fabs(static_cast<f64>(tilt))) * 5461.0f), 10.0f);
            part->rotation_z = SeekRot(part->rotation_z, spin + yaw, 10.0f);
            GameShadow(NULL, &bike->spline_position.position, 3.0f, 0);
            NuMtxSetIdentity(&matrix);
            NuMtxPreRotateX(&matrix, part->rotation_x);
            NuMtxPreRotateY(&matrix, part->rotation_y);
            NuMtxPreRotateZ(&matrix, part->rotation_z);
            NuMtxTranslate(&matrix, &bike->spline_position.position);
            part->transform = matrix;
            NuVecSub(&velocity, &bike->spline_position.position, &previous_position);
            NuVecScale(&velocity, &velocity, 1.0f / FRAMETIME);
            part->velocity = velocity;
            AddVariableShotDebrisEffectTimed1(
                WORLD->game_debris_sys->entries[96].effect,
                &KillPartsBikesPc[static_cast<i32>(reinterpret_cast<KILLPARTS_PART_PC *>(part)->speeder_index)]
                     .spline_position.position,
                5, FRAMETIME, pitch, yaw, NULL);
        } else {
            previous_position = part->position;
            f32 tilt = fabs(static_cast<f64>(NU_SIN_LUT(static_cast<i32>(time * 32768.0f + 16384.0f))));
            i32 turn = static_cast<i32>(2730.0f - (1.0f - tilt) * 5461.0f);
            i32 spin = static_cast<i32>((NU_SIN_LUT(static_cast<i32>(time * 16384.0f + 32768.0f + 16384.0f)) + 1.0f) *
                                        196608.0f);
            part->rotation_x = SeekRot(part->rotation_x, 0xd556, 10.0f);
            part->rotation_y =
                SeekRot(part->rotation_y, reinterpret_cast<GAMEOBJECT *>(Player[0])->field_0x276 + 0x4000, 10.0f);
            part->rotation_z = SeekRot(part->rotation_z, spin, 10.0f);
            GameShadow(NULL, &previous_position, 3.0f, 0);
            velocity = v001;
            NuVecRotateY(&velocity, &velocity, turn);
            NuVecScale(&velocity, &velocity,
                       reinterpret_cast<GAMEOBJECT *>(Player[0])->horizontal_velocity_magnitude / FRAMETIME);
            velocity.y = 1.0f / NU_COS_LUT(part->rotation_x);
            position.x = SeekLinearF(previous_position.x, camera->mtx.m30, FRAMETIME * static_cast<f64>(1.8f));
            position.y = SeekLinearF(previous_position.y, camera->mtx.m31, FRAMETIME * static_cast<f64>(1.8f));
            position.z = SeekLinearF(previous_position.z, camera->mtx.m32, FRAMETIME * static_cast<f64>(1.8f));
            NuVecAdd(&position, &position, &velocity);
            NuMtxSetIdentity(&matrix);
            NuMtxPreRotateX(&matrix, part->rotation_x);
            NuMtxPreRotateY(&matrix, part->rotation_y);
            NuMtxPreRotateZ(&matrix, part->rotation_z);
            NuMtxTranslate(&matrix, &position);
            part->transform = matrix;
            AddVariableShotDebrisEffectTimed1(WORLD->game_debris_sys->entries[96].effect, &position, 5, FRAMETIME,
                                              part->rotation_z, part->rotation_x, NULL);
        }
        if (bike->rider != NULL) {
            if ((bike->rider->flags_1fc_word & 0x1001) == 0x1001) {
                bike->rider->flags_ef0 |= 0x400000;
                bike->rider->flags_1fc_word |= 0x20;
                bike->rider->flags_ef4 |= 0x20;
                bike->rider_position.x = SeekLinearF(bike->rider_position.x, camera->mtx.m30, FRAMETIME);
                bike->rider_position.y = SeekLinearF(bike->rider_position.y, camera->mtx.m31 - 2.0f, FRAMETIME * 1.5f);
                bike->rider_position.z = SeekLinearF(bike->rider_position.z, camera->mtx.m32, FRAMETIME);
                bike->rider->position = bike->rider_position;
                bike->rider->ai_update_position = bike->rider_position;
                bike->rider->velocity = velocity;
                bike->rider->pitch_angle +=
                    static_cast<i32>(NuFloatRand(reinterpret_cast<NURAND *>(&GAMERAND)) * 2730.0f);
                bike->rider->field_0x276 +=
                    static_cast<i32>(NuFloatRand(reinterpret_cast<NURAND *>(&GAMERAND)) * 2730.0f);
                bike->rider->roll_angle +=
                    static_cast<i32>(NuFloatRand(reinterpret_cast<NURAND *>(&GAMERAND)) * 2730.0f);
                reinterpret_cast<KILLPARTS_RIDER_LIGHT_PC *>(bike->rider)->intensity[0].x = 1.0f;
                reinterpret_cast<KILLPARTS_RIDER_LIGHT_PC *>(bike->rider)->intensity[0].y = 1.0f;
                reinterpret_cast<KILLPARTS_RIDER_LIGHT_PC *>(bike->rider)->intensity[0].z = 1.0f;
                reinterpret_cast<KILLPARTS_RIDER_LIGHT_PC *>(bike->rider)->intensity[1].x = 1.0f;
                reinterpret_cast<KILLPARTS_RIDER_LIGHT_PC *>(bike->rider)->intensity[1].y = 1.0f;
                reinterpret_cast<KILLPARTS_RIDER_LIGHT_PC *>(bike->rider)->intensity[1].z = 1.0f;
            } else {
                bike->rider = NULL;
            }
        }
    }
}
