#include "gameapi/edptl.h"
#include "globals.h"
#include "legoapi/actions/movement/climbing.h"
#include "legoapi/ai/game/gameantinode.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/animlist.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/batarang.h"
#include "legoapi/items/collect/detonator.h"
#include "legoapi/items/fx/explosions.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/props/objects/zipup.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include <string.h>

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

extern "C" {
    static DETONATOR_s *Detonator_FindOldest(void) {
        f32 oldest_time = -1.0f;
        DETONATOR_s *oldest = NULL;
        if (Detonator[0].active != 0 && Detonator[0].timer > oldest_time) {
            oldest_time = Detonator[0].timer;
            oldest = &Detonator[0];
        }
        if (Detonator[1].active != 0 && Detonator[1].timer > oldest_time) {
            oldest_time = Detonator[1].timer;
            oldest = &Detonator[1];
        }
        if (Detonator[2].active != 0 && Detonator[2].timer > oldest_time) {
            oldest_time = Detonator[2].timer;
            oldest = &Detonator[2];
        }
        if (Detonator[3].active != 0 && Detonator[3].timer > oldest_time) {
            oldest_time = Detonator[3].timer;
            oldest = &Detonator[3];
        }
        if (Detonator[4].active != 0 && Detonator[4].timer > oldest_time) {
            oldest_time = Detonator[4].timer;
            oldest = &Detonator[4];
        }
        if (Detonator[5].active != 0 && Detonator[5].timer > oldest_time) {
            oldest_time = Detonator[5].timer;
            oldest = &Detonator[5];
        }
        if (Detonator[6].active != 0 && Detonator[6].timer > oldest_time) {
            oldest_time = Detonator[6].timer;
            oldest = &Detonator[6];
        }
        if (Detonator[7].active != 0 && Detonator[7].timer > oldest_time) {
            oldest_time = Detonator[7].timer;
            oldest = &Detonator[7];
        }
        if (Detonator[8].active != 0 && Detonator[8].timer > oldest_time) {
            oldest_time = Detonator[8].timer;
            oldest = &Detonator[8];
        }
        if (Detonator[9].active != 0 && Detonator[9].timer > oldest_time) {
            oldest_time = Detonator[9].timer;
            oldest = &Detonator[9];
        }
        return oldest;
    }

    EXPLOSION *Detonate(NUVEC *position, u16 flags) {
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
    Detonate(&detonator->detonate_position, 0);
    GAMEANTINODE_s *antinode = detonator->antinode;
    detonator->active = 0;
    if (antinode != NULL) {
        antinode->remaining_time = BatarangDetonatorHitDelay;
        detonator->antinode = NULL;
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
        NuCameraTransformScreenClip(&detonator->screen_position, &detonator->detonate_position, 1, NULL);
        timer = detonator->timer;
        if (timer >= 10.7f) {
            Detonator_Detonate(detonator);
        } else if (detonator->antinode != NULL) {
            GAMEANTINODE_s *antinode = detonator->antinode;
            if (!(detonator->timer >= 10.0f) &&
                (detonator->object == NULL || detonator->object->dead != 0 || !(detonator->object->player.detonator_hold_time >= 0.3f))) {
                antinode->radius = DetonatorHitScaleIdle;
            } else {
                antinode->radius = DetonatorHitScaleArmed;
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
                                                                             detonator->object->player.detonator_hold_time >= 0.3f))) {
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
    if (object->player.character_context != CHARACTER_CONTEXT_PICK_UP_DETONATOR &&
        object->player.character_context != CHARACTER_CONTEXT_PLACE_DETONATOR) {
        if (WORLD->level_objects[0xec].active == 0 || (object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0 ||
            (object->ground_flags == 0 && object->player.jump_variant_state != 1) ||
            (ObjLandReady(object) == 0 && (*(u64 *)&CInfo[object->player.character_context].flags & 0x2000) == 0)) {
            object->player.flags_e20 |= APIOBJECT_DETONATOR_WAIT_RELEASE;
            object->player.detonator_hold_time = 0.0f;
            return;
        }
        if ((object->player.pad->buttons_held & GAMEPAD_SPECIAL) != 0) {
            if (!(object->player.detonator_hold_time > 0.0f) && (object->player.flags_e20 & APIOBJECT_DETONATOR_WAIT_RELEASE) != 0) {
                return;
            }
            object->player.flags_e20 |= APIOBJECT_DETONATOR_WAIT_RELEASE;
            object->player.detonator_hold_time += FRAMETIME;
            if (object->player.detonator_hold_time >= 0.3f && object->player.detonator_hold_time >= 1.0f) {
                DETONATOR_s *oldest = Detonator_FindOldest();
                if (oldest != NULL) {
                    Detonator_Detonate(oldest);
                    object->player.detonator_hold_time = 0.7f;
                } else {
                    object->player.detonator_hold_time = 0.0f;
                    object->player.flags_e20 |= APIOBJECT_DETONATOR_WAIT_RELEASE;
                }
            }
            return;
        }
        if (object->player.detonator_hold_time > 0.0f && object->player.detonator_hold_time < 0.3f) {
            DETONATOR_s *nearest = Detonator_FindNearest(&object->lower_position, object->bounds_radius + 0.0775f, object);
            if (nearest != NULL) {
                object->player.context_phase = 0;
                object->player.character_context = CHARACTER_CONTEXT_PICK_UP_DETONATOR;
                FastWeaponIn(object, 0);
                object->player.context_animation = ACTION_PICKUP;
                object->player.grapple = nearest;
                object->movement_facing_angle =
                    NuAtan2D(nearest->position.x - object->position.x, nearest->position.z - object->position.z);
                nearest = object->player.detonator;
                object->player.launch_origin.x = nearest->position.x - NU_SIN_LUT(object->movement_facing_angle) * 0.1684f;
                object->player.launch_origin.z = nearest->position.z - NU_COS_LUT(object->movement_facing_angle) * 0.1684f;
                object->player.takeover_start_angle = (u16)RotDiff(object->movement_facing_angle, nearest->rotation_y);
            } else if (DetonatorPlacementSlot(object) != NULL && object->floor_height != GameObjectNearFloorNoFloorHeight &&
                       object->lower_position.y - object->floor_height < 0.155f) {
                object->player.launch_origin.x = object->lower_position.x;
                object->player.launch_origin.y = object->floor_height;
                object->player.launch_origin.z = object->lower_position.z;
                if (Detonator_FindNearest(&object->player.launch_origin, 0.155f, NULL) == NULL) {
                    object->player.context_phase = 0;
                    object->player.character_context = CHARACTER_CONTEXT_PLACE_DETONATOR;
                    FastWeaponIn(object, 0);
                    object->player.context_animation = ACTION_PUTDOWN;
                    object->player.saved_pitch = object->surface_pitch;
                    object->player.takeover_start_angle = (u16)qrand();
                    object->player.saved_roll = object->surface_roll;
                } else {
                    GameAudio_PlaySfx(0x32, NULL, 0, 0);
                }
            } else {
                GameAudio_PlaySfx(0x32, NULL, 0, 0);
            }
            if (object->player.character_context == CHARACTER_CONTEXT_PLACE_DETONATOR ||
                object->player.character_context == CHARACTER_CONTEXT_PICK_UP_DETONATOR) {
                object->velocity.z = 0.0f;
                object->velocity.x = 0.0f;
                if (AnimPlaying(&object->anim_packet, object->player.context_animation, 1, 1) != NULL) {
                    ResetAnimPacket(&object->anim_packet, -1);
                }
                if (object->character_model->model_data_b[object->player.context_animation] != NULL) {
                    object->player.context_timer = AnimDuration((i16)object->id, object->player.context_animation, 0.0f, 0.0f, 1);
                } else {
                    object->player.context_timer = 1.0f;
                }
            }
        }
        object->player.flags_e20 &= ~APIOBJECT_DETONATOR_WAIT_RELEASE;
        object->player.detonator_hold_time = 0.0f;
        return;
    }

    object->player.flags_e20 |= APIOBJECT_DETONATOR_WAIT_RELEASE;
    object->player.detonator_hold_time = 0.0f;
    f32 *frame = NULL;
    if (object->character_model->model_data_b[object->player.context_animation] != NULL) {
        frame = AnimPlaying(&object->anim_packet, object->player.context_animation, 1, 0);
        if (frame == NULL) {
            return;
        }
    }
    object->player.context_timer -= FRAMETIME;
    if (object->player.context_timer <= 0.0f) {
        object->player.character_context = CHARACTER_CONTEXT_NONE;
        if (object->player.context_phase != 0) {
            return;
        }
    } else {
        if (object->player.context_phase != 0) {
            return;
        }
        if (object->character_model->model_data_b[object->player.context_animation] != NULL) {
            f32 marker = AnimListFrame(object->character_model, object->player.context_animation,
                                       object->player.character_context == CHARACTER_CONTEXT_PICK_UP_DETONATOR ? 0 : 2);
            if (frame == NULL || !(*frame > 0.0f)) {
                return;
            }
            if (!(*frame >= marker)) {
                return;
            }
        } else if (!(object->player.context_timer < 0.5f)) {
            return;
        }
    }
    object->player.context_phase = 1;
    if (object->player.character_context == CHARACTER_CONTEXT_PICK_UP_DETONATOR) {
        DETONATOR_s *detonator = object->player.detonator;
        if (detonator != NULL) {
            detonator->active = 0;
            if (object->player.detonator->antinode != NULL) {
                GameAntinode_UnregisterAntiNode(WORLD->game_antinode_sys, object->player.detonator->antinode);
                object->player.detonator->antinode = NULL;
            }
        }
        return;
    }
    DETONATOR_s *detonator = DetonatorPlacementSlot(object);
    if (detonator != NULL) {
        detonator->position = object->player.launch_origin;
        detonator->active = 1;
        detonator->object = object;
        detonator->rotation_x = object->player.saved_pitch;
        detonator->rotation_y = (u16)(object->player.takeover_start_angle + object->movement_angle);
        detonator->rotation_z = object->player.saved_roll;
        detonator->detonate_position.z = 0.0f;
        detonator->detonate_position.x = 0.0f;
        detonator->detonate_position.y = 0.0775f;
        NuVecRotateZ(&detonator->detonate_position, &detonator->detonate_position, detonator->rotation_z);
        NuVecRotateX(&detonator->detonate_position, &detonator->detonate_position, detonator->rotation_x);
        NuVecAdd(&detonator->detonate_position, &detonator->detonate_position, &detonator->position);
        detonator->timer = 0.0f;
        detonator->antinode = (GAMEANTINODE_s *)GameAntinode_RegisterAntiNode(WORLD->game_antinode_sys, &detonator->detonate_position, 1.0f,
                                                                              1.0f, 1.0f, 0, 0, 0.0f);
        PlaySfx((char *)"imp_thermalDet_attach", &detonator->position);
    } else {
        GameAudio_PlaySfx(0x32, NULL, 0, 0);
    }
}
