#include "saga.h"
#include <string.h>

struct BATARANG_DETONATOR_HIT_VIEW { // size 0x24
    u8 unknown_00[0xc];              // 0x00
    f32 scale;                       // 0x0c
    u8 unknown_10[0x10];             // 0x10
    f32 delay;                       // 0x20
};

extern "C" const f64 GameObjectNearFloorNoFloorHeight;

extern "C" {

    void Detonators_Reset(void) {
        memset(Detonator, 0, sizeof(Detonator));
    }

    static DETONATOR_s *DetonatorPlacementSlot(GAMEOBJECT *object) {
        i32 count = 0;
        i32 available = -1;
        for (i32 index = 0; index < 10; ++index) {
            if (Detonator[index].active != 0 && Detonator[index].object == object) {
                if (++count == 3) {
                    return NULL;
                }
            } else if (available == -1) {
                available = index;
            }
        }
        if (available == -1) {
            return NULL;
        }
        return &Detonator[available];
    }
}

extern "C" DETONATOR_s *Detonator_FindNearest(NUVEC *position, f32 radius, GAMEOBJECT *owner) {
    DETONATOR_s *detonator = Detonator;
    DETONATOR_s *nearest = NULL;
    f32 nearest_distance = radius != 0.0f ? radius * radius : 1000000000.0;
    for (i32 index = 0; index < 10; ++index, ++detonator) {
        if (detonator->active == 0) {
            continue;
        }
        if (owner != NULL && detonator->object != owner) {
            continue;
        }
        f32 distance = NuVecDistSqr(position, &detonator->position, NULL);
        if (distance < nearest_distance) {
            nearest_distance = distance;
            nearest = detonator;
        }
    }
    return nearest;
}

#define DETONATOR_CONSIDER_OLDEST(detonator)                                                                                               \
    if ((detonator)->active != 0 && (detonator)->timer > oldest_time) {                                                                    \
        oldest_time = (detonator)->timer;                                                                                                  \
        oldest = (detonator);                                                                                                              \
    }

extern "C" {
    static DETONATOR_s *Detonator_FindOldest(void) {
        f32 oldest_time = -1.0f;
        DETONATOR_s *oldest = NULL;
        DETONATOR_CONSIDER_OLDEST(&Detonator[0]);
        DETONATOR_CONSIDER_OLDEST(&Detonator[1]);
        DETONATOR_CONSIDER_OLDEST(&Detonator[2]);
        DETONATOR_CONSIDER_OLDEST(&Detonator[3]);
        DETONATOR_CONSIDER_OLDEST(&Detonator[4]);
        DETONATOR_CONSIDER_OLDEST(&Detonator[5]);
        DETONATOR_CONSIDER_OLDEST(&Detonator[6]);
        DETONATOR_CONSIDER_OLDEST(&Detonator[7]);
        DETONATOR_CONSIDER_OLDEST(&Detonator[8]);
        DETONATOR_CONSIDER_OLDEST(&Detonator[9]);
        return oldest;
    }

    EXPLOSION_PC *Detonate(NUVEC *position, u16 flags) {
        f32 radius;
        f32 duration;
        f32 amount;
        AddGameDebris(WORLD->game_debris_sys, 0x49, position);
        AddGameDebris(WORLD->game_debris_sys, 0x4a, position);
        AddGameDebris(WORLD->game_debris_sys, 0x4b, position);
        AddPartDebris(WORLD->part_debris_sys, 2, position);
        NewRumbleAllPlayers(1.0f, 0.1f, 0, 0);
        PlaySfx("exp_thermalDet", position);
        radius = 0.75f;
        duration = 0.5f;
        if ((flags & 0x1000) != 0) {
            radius = 1.3125f;
            duration = 0.875f;
            amount = qrand() < 0x8000 ? -1.0f : 1.0f;
            GameCam_Judder(GameCam, amount * 2.0f, 2, position);
            GameCam_NewShake(GameCam, 2.0f, 1.0f, 1.0f);
            PlaySfx("exp_thermalDet", position);
        } else {
            GameCam_NewShake(GameCam, 1.0f, 1.0f, 1.0f);
        }
        return AddExplosion(position, radius, duration, NULL, -1, (u32)flags | 0x67);
    }
}

extern "C" {
    f32 DetonatorHitScaleArmed = 0.75f;
    f32 DetonatorHitScaleIdle = 0.08f;
    f32 BatarangDetonatorHitDelay = 2.0f;
}

extern "C" void Detonator_Detonate(DETONATOR_s *detonator) {
    Batarangs_CheckLostData(detonator);
    Detonate(&detonator->unknown_0c, 0);
    BATARANG_DETONATOR_HIT_VIEW *hit_data = static_cast<BATARANG_DETONATOR_HIT_VIEW *>(detonator->unknown_34);
    detonator->active = 0;
    if (hit_data != NULL) {
        hit_data->delay = BatarangDetonatorHitDelay;
        detonator->unknown_34 = NULL;
    }
}

extern "C" void Detonators_Update(void) {
    DETONATOR_s *detonator = Detonator;
    for (i32 index = 0; index < 10; ++index, ++detonator) {
        f32 timer;
        if (detonator->active == 0) {
            continue;
        }
        detonator->timer += FRAMETIME;
        NuCameraTransformScreenClip(&detonator->unknown_18, &detonator->unknown_0c, 1, NULL);
        timer = detonator->timer;
        if (timer >= 10.7f) {
            Detonator_Detonate(detonator);
        } else if (detonator->unknown_34 != NULL) {
            BATARANG_DETONATOR_HIT_VIEW *hit_data = static_cast<BATARANG_DETONATOR_HIT_VIEW *>(detonator->unknown_34);
            if (!(detonator->timer >= 10.0f) &&
                (detonator->object == NULL || detonator->object->dead != 0 || !(detonator->object->unknown_de0 >= 0.3f))) {
                hit_data->scale = DetonatorHitScaleIdle;
            } else {
                hit_data->scale = DetonatorHitScaleArmed;
            }
        }
    }
}

extern "C" void Detonators_Draw(void) {
    if (!WORLD->level_objects[0xec].active) {
        return;
    }
    DETONATOR_s *detonator = Detonator;
    for (i32 index = 0; index < 10; ++index, ++detonator) {
        NUMTX matrix;
        detonator->draw_result = 0;
        if (!detonator->active) {
            continue;
        }
        NuMtxSetRotationY(&matrix, detonator->rotation_y);
        NuMtxRotateZ(&matrix, detonator->rotation_z);
        NuMtxRotateX(&matrix, detonator->rotation_x);
        NuMtxTranslate(&matrix, &detonator->position);
        NuSpecialDrawAt(&WORLD->level_objects[0xec].special, &matrix);
        if (GameTimer.update_count % 6 < 3 && (detonator->timer >= 10.0f || (detonator->object != NULL && detonator->object->dead == 0 &&
                                                                             detonator->object->unknown_de0 >= 0.3f))) {
            if (WORLD->level_objects[0xef].active) {
                detonator->draw_result = (u8)NuSpecialDrawAt(&WORLD->level_objects[0xef].special, &matrix);
            }
        } else {
            if (WORLD->level_objects[0xee].active) {
                detonator->draw_result = (u8)NuSpecialDrawAt(&WORLD->level_objects[0xee].special, &matrix);
            }
        }
    }
}

extern "C" void Detonator_MoveCode(GAMEOBJECT *object) {
    if (object->character_context != CHARACTER_CONTEXT_PICK_UP_DETONATOR &&
        object->character_context != CHARACTER_CONTEXT_PLACE_DETONATOR) {
        if (WORLD->level_objects[0xec].active == 0 || (object->flags_1fc & 0x80) == 0 ||
            (object->unknown_281 == 0 && object->jump_variant_state != 1) ||
            (ObjLandReady(object) == 0 && (*(u64 *)&CInfo[object->character_context].flags & 0x2000) == 0)) {
            object->flags_e20 |= 0x8000;
            object->unknown_de0 = 0.0f;
            return;
        }
        if ((object->pad->buttons_held & GAMEPAD_SPECIAL) != 0) {
            if (!(object->unknown_de0 > 0.0f) && (object->flags_e20 & 0x8000) != 0) {
                return;
            }
            object->flags_e20 |= 0x8000;
            object->unknown_de0 += FRAMETIME;
            if (object->unknown_de0 >= 0.3f && object->unknown_de0 >= 1.0f) {
                DETONATOR_s *oldest = Detonator_FindOldest();
                if (oldest != NULL) {
                    Detonator_Detonate(oldest);
                    object->unknown_de0 = 0.7f;
                } else {
                    object->unknown_de0 = 0.0f;
                    object->flags_e20 |= 0x8000;
                }
            }
            return;
        }
        if (object->unknown_de0 > 0.0f && object->unknown_de0 < 0.3f) {
            DETONATOR_s *nearest = Detonator_FindNearest(&object->lower_position, object->bounds_radius + 0.0775f, object);
            if (nearest != NULL) {
                object->unknown_7b3 = 0;
                object->character_context = CHARACTER_CONTEXT_PICK_UP_DETONATOR;
                FastWeaponIn(object, 0);
                object->unknown_7aa = 0x66;
                object->grapple = nearest;
                object->movement_facing_angle =
                    NuAtan2D(nearest->position.x - object->position.x, nearest->position.z - object->position.z);
                nearest = (DETONATOR_s *)object->grapple;
                object->launch_origin.x = nearest->position.x - NU_SIN_LUT(object->movement_facing_angle) * 0.1684f;
                object->launch_origin.z = nearest->position.z - NU_COS_LUT(object->movement_facing_angle) * 0.1684f;
                object->takeover_start_angle = (u16)RotDiff(object->movement_facing_angle, nearest->rotation_y);
            } else if (DetonatorPlacementSlot(object) != NULL && object->floor_height != GameObjectNearFloorNoFloorHeight &&
                       object->lower_position.y - object->floor_height < 0.155f) {
                object->launch_origin.x = object->lower_position.x;
                object->launch_origin.y = object->floor_height;
                object->launch_origin.z = object->lower_position.z;
                if (Detonator_FindNearest(&object->launch_origin, 0.155f, NULL) == NULL) {
                    object->unknown_7b3 = 0;
                    object->character_context = CHARACTER_CONTEXT_PLACE_DETONATOR;
                    FastWeaponIn(object, 0);
                    object->unknown_7aa = 0x8d;
                    object->unknown_7a4 = object->unknown_1056;
                    object->takeover_start_angle = (u16)qrand();
                    object->unknown_7a8 = object->unknown_1058;
                } else {
                    GameAudio_PlaySfx(0x32, NULL, 0, 0);
                }
            } else {
                GameAudio_PlaySfx(0x32, NULL, 0, 0);
            }
            if (object->character_context == CHARACTER_CONTEXT_PLACE_DETONATOR ||
                object->character_context == CHARACTER_CONTEXT_PICK_UP_DETONATOR) {
                object->velocity.z = 0.0f;
                object->velocity.x = 0.0f;
                if (AnimPlaying(&object->anim_packet, object->unknown_7aa, 1, 1) != NULL) {
                    ResetAnimPacket(&object->anim_packet, -1);
                }
                if (object->character_model->model_data_b[object->unknown_7aa] != NULL) {
                    object->context_timer = AnimDuration((i16)object->id, object->unknown_7aa, 0.0f, 0.0f, 1);
                } else {
                    object->context_timer = 1.0f;
                }
            }
        }
        object->flags_e20 &= ~0x8000;
        object->unknown_de0 = 0.0f;
        return;
    }

    object->flags_e20 |= 0x8000;
    object->unknown_de0 = 0.0f;
    f32 *frame = NULL;
    if (object->character_model->model_data_b[object->unknown_7aa] != NULL) {
        frame = AnimPlaying(&object->anim_packet, object->unknown_7aa, 1, 0);
        if (frame == NULL) {
            return;
        }
    }
    object->context_timer -= FRAMETIME;
    if (object->context_timer <= 0.0f) {
        object->character_context = CHARACTER_CONTEXT_NONE;
        if (object->unknown_7b3 != 0) {
            return;
        }
    } else {
        if (object->unknown_7b3 != 0) {
            return;
        }
        if (object->character_model->model_data_b[object->unknown_7aa] != NULL) {
            f32 marker = AnimListFrame(object->character_model, object->unknown_7aa,
                                       object->character_context == CHARACTER_CONTEXT_PICK_UP_DETONATOR ? 0 : 2);
            if (frame == NULL || !(*frame > 0.0f)) {
                return;
            }
            if (!(*frame >= marker)) {
                return;
            }
        } else if (!(object->context_timer < 0.5f)) {
            return;
        }
    }
    object->unknown_7b3 = 1;
    if (object->character_context == CHARACTER_CONTEXT_PICK_UP_DETONATOR) {
        DETONATOR_s *detonator = (DETONATOR_s *)object->grapple;
        if (detonator != NULL) {
            detonator->active = 0;
            if (((DETONATOR_s *)object->grapple)->unknown_34 != NULL) {
                GameAntinode_UnregisterAntiNode(WORLD->game_antinode_sys, ((DETONATOR_s *)object->grapple)->unknown_34);
                ((DETONATOR_s *)object->grapple)->unknown_34 = NULL;
            }
        }
        return;
    }
    DETONATOR_s *detonator = DetonatorPlacementSlot(object);
    if (detonator != NULL) {
        detonator->position = object->launch_origin;
        detonator->active = 1;
        detonator->object = object;
        detonator->rotation_x = object->unknown_7a4;
        detonator->rotation_y = (u16)(object->takeover_start_angle + object->unknown_276);
        detonator->rotation_z = object->unknown_7a8;
        detonator->unknown_0c.z = 0.0f;
        detonator->unknown_0c.x = 0.0f;
        detonator->unknown_0c.y = 0.0775f;
        NuVecRotateZ(&detonator->unknown_0c, &detonator->unknown_0c, detonator->rotation_z);
        NuVecRotateX(&detonator->unknown_0c, &detonator->unknown_0c, detonator->rotation_x);
        NuVecAdd(&detonator->unknown_0c, &detonator->unknown_0c, &detonator->position);
        detonator->timer = 0.0f;
        detonator->unknown_34 =
            GameAntinode_RegisterAntiNode(WORLD->game_antinode_sys, &detonator->unknown_0c, 1.0f, 1.0f, 1.0f, 0, 0, 0.0f);
        PlaySfx((char *)"imp_thermalDet_attach", &detonator->position);
    } else {
        GameAudio_PlaySfx(0x32, NULL, 0, 0);
    }
}
