#include "legoapi/actions/combat/whip.h"
#include "legoapi/actions/movement/carrying.h"
#include "legoapi/actions/movement/climbing.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/animation_ids.h"
#include "legoapi/characters/motion/animlist.h"
#include "legoapi/characters/motion/contexts.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/misc/gamemain.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

typedef struct whip_basis_s { // size 0x76c
    u8 unknown_000[0x748];    // 0x000
    NUVEC basis[3];           // 0x748
} WHIP_BASIS;

void Whip_MoveCode(GAMEOBJECT *object) {
    GIZMOBLOWUP *target;
    f32 *frame;
    f32 action_frame;
    i32 activate;
    if (LEGOCONTEXT_WHIP == -1) {
        return;
    }
    if (object->player.character_context == LEGOCONTEXT_WHIP) {
        if (object->player.unknown_7b3 == WHIP_START) {
            if (AnimPlaying(&object->anim_packet, object->player.unknown_7aa, 1, 0) == NULL) {
                return;
            }
            object->player.context_timer += FRAMETIME;
            if (object->player.context_timer >= object->player.context_duration) {
                switch (object->player.whip_action) {
                    case WHIP_GRAB:
                        target = object->player.grapple;
                        if (target != NULL && (target->status_flags & BLOWUP_STATUS_VISIBLE) &&
                            (target->status_flags & BLOWUP_STATUS_ACTIVE)) {
                            object->player.unknown_7aa = LEGOACT_WHIP_GRAB;
                            object->player.context_timer = 0.0f;
                            object->player.unknown_7b3 = WHIP_GRAB;
                            object->player.context_duration = AnimDuration((i16)object->id, object->player.unknown_7aa, 0.0f, 0.0f, 1);
                            object->player.context_variant_flags &= ~APIOBJECT_CONTEXT_EVENT_DONE;
                            target = object->player.grapple;
                            object->movement_facing_angle =
                                NuAtan2D(target->position.x - object->position.x, target->position.z - object->position.z);
                            return;
                        }
                        break;
                    case WHIP_BREAK:
                        target = object->player.grapple;
                        if (target != NULL && (target->status_flags & BLOWUP_STATUS_VISIBLE) &&
                            (target->status_flags & BLOWUP_STATUS_ACTIVE)) {
                            object->player.unknown_7aa = LEGOACT_WHIP_BREAK;
                            object->player.context_timer = 0.0f;
                            object->player.unknown_7b3 = WHIP_BREAK;
                            object->player.context_duration = AnimDuration((i16)object->id, object->player.unknown_7aa, 0.0f, 0.0f, 1);
                            object->player.context_variant_flags &= ~APIOBJECT_CONTEXT_EVENT_DONE;
                            target = object->player.grapple;
                            object->movement_facing_angle =
                                NuAtan2D(target->position.x - object->position.x, target->position.z - object->position.z);
                            return;
                        }
                        break;
                    default:
                        break;
                }
                object->player.whip_action = WHIP_CRACK;
                object->player.unknown_7aa = LEGOACT_WHIP_CRACK;
                object->player.context_timer = 0.0f;
                object->player.unknown_7b3 = WHIP_CRACK;
                object->player.context_duration = AnimDuration((i16)object->id, object->player.unknown_7aa, 0.0f, 0.0f, 1);
                object->player.context_variant_flags &= ~APIOBJECT_CONTEXT_EVENT_DONE;
            }
            return;
        }
        if (object->player.context_variant_flags & 0x100) {
            NUVEC *target_position = NULL;
            if (object->player.grapple != NULL) {
                target_position = &((GIZMOBLOWUP *)object->player.grapple)->position;
            } else if (object->player.force_target != NULL) {
                target_position = &object->player.force_target->collision_position;
            }
            if (target_position != NULL) {
                object->movement_facing_angle = NuAtan2D(target_position->x - object->position.x, target_position->z - object->position.z);
            }
        }
        activate = 0;
        frame = AnimPlaying(&object->anim_packet, object->player.unknown_7aa, 1, 0);
        if (frame != NULL) {
            object->player.context_timer += FRAMETIME;
            if (!(object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE)) {
                action_frame = AnimListFrame(object->character_model, object->player.unknown_7aa, 0);
                if (action_frame >= 1.0f && *frame >= action_frame) {
                    object->player.context_variant_flags |= APIOBJECT_CONTEXT_EVENT_DONE;
                    activate = 1;
                }
            }
            if (object->player.context_timer >= object->player.context_duration) {
                object->player.character_context = CHARACTER_CONTEXT_NONE;
                if (!(object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE)) {
                    activate = 1;
                }
            }
            if (activate) {
                switch (object->player.unknown_7b3) {
                    case WHIP_GRAB:
                        GizmoBlowupBlowup(object->player.grapple, 0, BLOWUP_HIT_CARRIED, -1, NULL, 1);
                        activate = 2;
                        break;
                    case WHIP_BREAK:
                        if (GizmoBlowupBlowup(object->player.grapple, 1, BLOWUP_HIT_PUNCH, 1, NULL, 1)) {
                            NewBuzz(object->player.pad->bound_pad, 0.1f, 0);
                        }
                        NewRumble(object->player.pad->bound_pad, 0.5f, 0);
                        activate = 2;
                        break;
                    default:
                        break;
                }
                if (activate == 2) {
                    GameAudio_PlaySfx(77, &object->collision_position, GameAudio_GetPlrSfxBits(object), 0);
                } else {
                    GameAudio_PlaySfx(78, &object->collision_position, GameAudio_GetPlrSfxBits(object), 0);
                }
            }
        }
        if (object->player.character_context == CHARACTER_CONTEXT_NONE && object->player.unknown_7b3 == WHIP_GRAB &&
            object->player.grapple != NULL) {
            SuperCarry_Start(object, object->player.grapple, 1);
        }
    } else {
        i32 action;
        if (!(object->player.pad->buttons_pressed & GAMEPAD_SPECIAL) || object->unknown_281 == 0 || !ObjLandReady(object) ||
            LEGOACT_WHIP_START == -1 || object->character_model->model_data_b[LEGOACT_WHIP_START] == NULL) {
            return;
        }
        object->player.whip_action = -1;
        target = NULL;
        if (LEGOACT_WHIP_GRAB != -1 && object->character_model->model_data_b[LEGOACT_WHIP_GRAB] != NULL && SuperCarry_Possible(object, 0) &&
            (target = GizmoBlowUpOpponent(object, 2.0f, 0.75f, 0.25f, 5, 0, 0, 1)) != NULL) {
            action = WHIP_GRAB;
        } else if (LEGOACT_WHIP_BREAK != -1 && object->character_model->model_data_b[LEGOACT_WHIP_BREAK] != NULL &&
                   (target = GizmoBlowUpOpponent(object, 2.0f, 0.75f, 0.25f, 5, 0xc080, 0, 0)) != NULL) {
            action = WHIP_BREAK;
        } else {
            action = WHIP_CRACK;
        }
        object->player.character_context = (i8)LEGOCONTEXT_WHIP;
        object->player.unknown_7aa = LEGOACT_WHIP_START;
        object->player.context_timer = 0.0f;
        object->player.context_variant_flags &= ~0x100u;
        object->player.whip_action = (i8)action;
        object->player.unknown_7b3 = WHIP_START;
        object->player.context_duration = AnimDuration((i16)object->id, object->player.unknown_7aa, 0.0f, 0.0f, 0);
        GameAudio_PlaySfx(76, &object->collision_position, GameAudio_GetPlrSfxBits(object), 0);
        SetWeaponIn(object);
        object->player.grapple = target;
        object->player.force_target = NULL;
        if (target != NULL) {
            object->movement_facing_angle = NuAtan2D(target->position.x - object->position.x, target->position.z - object->position.z);
        }
    }
}

NUVEC v100 = {1.0f, 0.0f, 0.0f};
NUVEC v010 = {0.0f, 1.0f, 0.0f};

void Whip_Release(GAMEOBJECT *object) {
    WHIP_BASIS *view = (WHIP_BASIS *)object;
    if (LEGOCONTEXT_WHIP != -1 && object->player.character_context == LEGOCONTEXT_WHIP && object->player.grapple != NULL) {
        view->basis[0] = v100;
        view->basis[1] = v010;
        view->basis[2] = v001;
        SuperCarry_Throw(object, 1);
    }
}

void fun_005b2da0(void) {
}
