#include "saga.h"
typedef struct whip_basis_pc_s {
    u8 unknown_000[0x748];
    NUVEC basis[3];
} WHIP_BASIS_PC;
STATIC_ASSERT(offsetof(WHIP_BASIS_PC, basis) == 0x748);
/* Whip action selector (-1 none, 1 crack, 2 break, 3 grab) shares the byte before hit_variant. */
typedef struct whip_action_pc_s {
    u8 unknown_000[0x7ba];
    i8 whip_action;
} WHIP_ACTION_PC;
STATIC_ASSERT(offsetof(WHIP_ACTION_PC, whip_action) == 0x7ba);
void __cdecl SuperCarry_Throw(GAMEOBJECT *, i32);
i32 __cdecl SuperCarry_Possible(GAMEOBJECT *, i32);
void __cdecl SuperCarry_Start(GAMEOBJECT *, GIZMOBLOWUP_PC *, i32);

void __cdecl Whip_MoveCode(GAMEOBJECT *object) {
    WHIP_ACTION_PC *view = (WHIP_ACTION_PC *)object;
    GIZMOBLOWUP_PC *target;
    f32 *frame;
    f32 action_frame;
    i32 activate;
    if (LEGOCONTEXT_WHIP == -1)
        return;
    if (object->character_context == LEGOCONTEXT_WHIP) {
        if (object->field_7b3 == 0) {
            if (AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) == NULL)
                return;
            object->context_timer += FRAMETIME;
            if (object->context_timer >= object->context_duration) {
                switch (view->whip_action) {
                    case 3:
                        target = (GIZMOBLOWUP_PC *)object->grapple;
                        if (target != NULL && (target->status_flags & 0x4000) && (target->status_flags & 0x800000)) {
                            object->context_timer = 0.0f;
                            object->field_7aa = LEGOACT_WHIP_GRAB;
                            object->field_7b3 = 3;
                            object->context_duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
                            object->context_variant_flags &= ~0x40u;
                            target = (GIZMOBLOWUP_PC *)object->grapple;
                            object->movement_facing_angle = NuAtan2D(target->position.x - object->position.x,
                                                                     target->position.z - object->position.z);
                            return;
                        }
                        break;
                    case 2:
                        target = (GIZMOBLOWUP_PC *)object->grapple;
                        if (target != NULL && (target->status_flags & 0x4000) && (target->status_flags & 0x800000)) {
                            object->context_timer = 0.0f;
                            object->field_7aa = LEGOACT_WHIP_BREAK;
                            object->field_7b3 = 2;
                            object->context_duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
                            object->context_variant_flags &= ~0x40u;
                            target = (GIZMOBLOWUP_PC *)object->grapple;
                            object->movement_facing_angle = NuAtan2D(target->position.x - object->position.x,
                                                                     target->position.z - object->position.z);
                            return;
                        }
                        break;
                    default:
                        break;
                }
                view->whip_action = 1;
                object->context_timer = 0.0f;
                object->field_7aa = LEGOACT_WHIP_CRACK;
                object->field_7b3 = 1;
                object->context_duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 1);
                object->context_variant_flags &= ~0x40u;
            }
            return;
        }
        if (object->context_variant_flags & 0x100) {
            NUVEC *target_position = NULL;
            if (object->grapple != NULL)
                target_position = &((GIZMOBLOWUP_PC *)object->grapple)->position;
            else if (object->force_target != NULL)
                target_position = &object->force_target->collision_position;
            if (target_position != NULL)
                object->movement_facing_angle =
                    NuAtan2D(target_position->x - object->position.x, target_position->z - object->position.z);
        }
        activate = 0;
        frame = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
        if (frame != NULL) {
            object->context_timer += FRAMETIME;
            if (!(object->context_variant_flags & 0x40)) {
                action_frame = AnimListFrame(object->character_model, object->field_7aa, 0);
                if (action_frame >= 1.0f && *frame >= action_frame) {
                    object->context_variant_flags |= 0x40;
                    activate = 1;
                }
            }
            if (object->context_timer >= object->context_duration) {
                object->character_context = -1;
                if (!(object->context_variant_flags & 0x40))
                    activate = 1;
            }
            if (activate) {
                switch (object->field_7b3) {
                    case 3:
                        GizmoBlowupBlowup((GIZMOBLOWUP_PC *)object->grapple, 0, 7, -1, NULL, 1);
                        activate = 2;
                        break;
                    case 2:
                        if (GizmoBlowupBlowup((GIZMOBLOWUP_PC *)object->grapple, 1, 1, 1, NULL, 1))
                            NewBuzz(object->pad->bound_pad, 0.1f, 0);
                        NewRumble(object->pad->bound_pad, 0.5f, 0);
                        activate = 2;
                        break;
                    default:
                        break;
                }
                if (activate == 2)
                    GameAudio_PlaySfx(77, &object->collision_position, GameAudio_GetPlrSfxBits(object), 0);
                else
                    GameAudio_PlaySfx(78, &object->collision_position, GameAudio_GetPlrSfxBits(object), 0);
            }
        }
        if (object->character_context == -1 && object->field_7b3 == 3 && object->grapple != NULL)
            SuperCarry_Start(object, (GIZMOBLOWUP_PC *)object->grapple, 1);
    } else {
        i32 action;
        if (!(object->pad->buttons_pressed & GAMEPAD_SPECIAL) || object->field_281 == 0 || !ObjLandReady(object) ||
            LEGOACT_WHIP_START == -1 || object->character_model->model_data_b[LEGOACT_WHIP_START] == NULL)
            return;
        view->whip_action = -1;
        target = NULL;
        if (LEGOACT_WHIP_GRAB != -1 && object->character_model->model_data_b[LEGOACT_WHIP_GRAB] != NULL &&
            SuperCarry_Possible(object, 0) &&
            (target = GizmoBlowUpOpponent(object, 2.0f, 0.75f, 0.25f, 5, 0, 0, 1)) != NULL)
            action = 3;
        else if (LEGOACT_WHIP_BREAK != -1 && object->character_model->model_data_b[LEGOACT_WHIP_BREAK] != NULL &&
                 (target = GizmoBlowUpOpponent(object, 2.0f, 0.75f, 0.25f, 5, 0xc080, 0, 0)) != NULL)
            action = 2;
        else
            action = 1;
        object->character_context = (i8)LEGOCONTEXT_WHIP;
        object->field_7aa = LEGOACT_WHIP_START;
        object->context_timer = 0.0f;
        object->context_variant_flags &= ~0x100u;
        view->whip_action = (i8)action;
        object->field_7b3 = 0;
        object->context_duration = AnimDuration((i16)object->id, object->field_7aa, 0.0f, 0.0f, 0);
        GameAudio_PlaySfx(76, &object->collision_position, GameAudio_GetPlrSfxBits(object), 0);
        SetWeaponIn(object);
        object->grapple = target;
        object->force_target = NULL;
        if (target != NULL)
            object->movement_facing_angle =
                NuAtan2D(target->position.x - object->position.x, target->position.z - object->position.z);
    }
}

void __cdecl Whip_Release(GAMEOBJECT *object) {
    WHIP_BASIS_PC *view = (WHIP_BASIS_PC *)object;
    if (LEGOCONTEXT_WHIP != -1 && object->character_context == LEGOCONTEXT_WHIP && object->grapple != NULL) {
        view->basis[0] = v100;
        view->basis[1] = v010;
        view->basis[2] = v001;
        SuperCarry_Throw(object, 1);
    }
}
