#include "saga.h"

i32 __cdecl GizmoBlowupBlowup(GIZMOBLOWUP_PC *blowup, i32 effects, i32 hit_type, i32 damage, GAMEOBJECT *object,
                              i32 hit_context) {
    NUINSTANIM_TIME_VIEW *animation;
    NUINSTANIM_TIME_VIEW *burst;
    GIZMOBLOWUPTYPE_PC *type;
    f32 frame;
    i32 has_burst;
    i32 destroyed = 0;
    i32 effect_flags = 1;
    i32 player;
    GAMEOBJECT *target;
    f32 radius_squared;
    f32 x, y, z;
    if (blowup == NULL || (blowup->status_flags & 0x800000) == 0 ||
        ((blowup->draw_flags & 0x20) != 0 && ShadowMode == 0))
        return 0;
    animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&blowup->type->animated_special);
    type = blowup->type;
    if (animation != NULL && type->animation_start_frame != type->animation_end_frame) {
        if ((type->animation_flags & 0x2000) == 0 && (blowup->status_flags & 0x400000) == 0) {
            if ((blowup->draw_flags & 0x208) == 0) {
                if (blowup->animation_time < type->animation_end_frame) {
                    blowup->status_flags |= 0x80000;
                    return 0;
                }
                blowup->status_flags &= ~0x80000;
            }
        } else {
            frame = (blowup->draw_flags & 0x800000) != 0 ? animation->ltime : blowup->animation_time;
            if (frame < type->animation_start_frame || type->animation_end_frame < frame)
                return 0;
        }
    }
    switch (hit_type) {
        case 1:
            blowup->status_flags |= 2;
            break;
        case 2:
            blowup->status_flags |= 4;
            break;
        case 3:
            blowup->status_flags |= 8;
            break;
        case 4:
            blowup->status_flags |= 0x20;
            break;
        case 5:
            blowup->status_flags |= 0x10;
            break;
        case 6:
            blowup->status_flags |= 0x40;
            break;
        case 7:
            blowup->status_flags |= 0x80;
            break;
        case 8:
            blowup->status_flags |= 0x100;
            break;
        case 9:
            blowup->status_flags |= 0x200;
            break;
        case 10:
            blowup->status_flags |= 0x400;
            break;
        case 11:
            blowup->status_flags |= 0x800;
            break;
        case 12:
            blowup->status_flags |= 0x1000;
            break;
        case 13:
            blowup->status_flags |= 0x2000;
            break;
    }
    has_burst = NuSpecialExistsFn(&type->burst_special);
    if (has_burst) {
        burst = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&type->burst_special);
        if (burst != NULL)
            burst->flags |= 1;
    }
    if (damage != -1)
        blowup->remaining_health = (u8)(blowup->remaining_health - damage);
    if (damage == -1 || (i8)blowup->remaining_health <= 0) {
        destroyed = 1;
        blowup->remaining_health = 0;
        if ((type->animation_flags & 0x2000) != 0 && (blowup->draw_flags & 0x800000) != 0)
            NuSpecialSetVisibility(&type->animated_special, 0);
        blowup->status_flags = (blowup->status_flags & 0xfe733fff) | 1;
        blowup->animation_time = 1.0f;
        if (object != NULL) {
            if (blowup->damage != 0)
                STUBBED_NAMED("GizmoBlowupBlowup: ObjHitObj PC438230 direct target");
        } else if ((blowup->draw_flags & 0x208) != 0 && blowup->damage != 0 &&
                   (!(blowup->proximity_radius <= 0.0f) || !(blowup->effect_delay > 0.0f))) {
            radius_squared = blowup->proximity_radius * blowup->proximity_radius;
            for (player = 0; player < 8; ++player) {
                target = (GAMEOBJECT *)Player[player];
                if (target != NULL && (target->flags_1fc_word & 0x1001) == 0x1001 && target->dead == 0 &&
                    !(target->field_1010 > 0.0f)) {
                    x = blowup->mid_position.x - target->collision_position.x;
                    y = blowup->mid_position.y - target->collision_position.y;
                    z = blowup->mid_position.z - target->collision_position.z;
                    if (x * x + y * y + z * z < radius_squared)
                        STUBBED_NAMED("GizmoBlowupBlowup: ObjHitObj PC438230 each eligible nearby player");
                }
            }
        }
        if (blowup->platform_id != -1) {
            STUBBED_NAMED("GizmoBlowupBlowup: PlatOnOff PC67c8e0 base platform off");
            if (has_burst && blowup->burst_platform_id != -1)
                STUBBED_NAMED("GizmoBlowupBlowup: PlatOnOff PC67c8e0 burst platform on");
            if ((type->animation_flags & 0x2000) != 0)
                NuSpecialSetVisibility(&type->animated_special, 0);
        }
        if ((blowup->draw_flags & 0x400) == 0 && NuSpecialExistsFn(&type->decal_special)) {
            blowup->animation_time = 0.1f;
            blowup->status_flags |= 0x20000;
        }
        if ((blowup->draw_flags & 0x800) != 0)
            STUBBED_NAMED("GizmoBlowupBlowup: GizmoBlowupCreateStuff PC42d2a0");
        if (GameBlowUpBlownUpFn != NULL)
            GameBlowUpBlownUpFn(blowup);
        if ((blowup->status_flags & 1) != 0 && CheckLostDataFn != NULL)
            CheckLostDataFn(blowup);
        if (blowup->respawn_period > 0.0f)
            blowup->respawn_timer = blowup->respawn_period;
        effect_flags = (blowup->draw_flags & 0x40000) != 0 ? 0x17 : 7;
        if (blowup->anti_node != NULL) {
            STUBBED_NAMED("GizmoBlowupBlowup: GameAntinode_UnregisterAntiNode PC59a6b0");
            blowup->anti_node = NULL;
        }
    } else if ((blowup->status_flags & 0x200000) != 0) {
        effect_flags = 4;
    }
    if (blowup->effect_delay > 0.0f && destroyed)
        effect_flags |= 8;
    if (effects)
        STUBBED_NAMED("GizmoBlowupBlowup: GizmoBlowUp_AddEffects PC5672d0");
    return 1;
}

i32 __cdecl GizmoBlowupCheckProximity(WORLDINFO *world, GAMEOBJECT *object) {
    i32 count = 0;
    i32 special;
    i32 destroy = 0;
    i32 index;
    i32 hit_type;
    GIZMOBLOWUP_PC *blowup;
    void *animation;
    f32 x, y, z;
    if (object == NULL || (object->flags_1fc & 0x80) == 0)
        return 0;
    special = (object->character_data->model_flags & 0x2000) != 0 || object->takeover_source != NULL;
    blowup = world->gizmo_blowups;
    for (index = 0; index < world->gizmo_blowup_count; ++index, ++blowup) {
        if ((blowup->status_flags & 0x804001) != 0x804000 || (special && (blowup->draw_flags & 0x200) == 0) ||
            (!special && (blowup->draw_flags & 8) == 0) || (blowup->status_flags & 0x2040000) != 0)
            continue;
        x = object->collision_position.x - blowup->mid_position.x;
        y = object->collision_position.y - blowup->mid_position.y;
        z = object->collision_position.z - blowup->mid_position.z;
        if (blowup->proximity_radius != 0.0f &&
            !(x * x + y * y + z * z < blowup->proximity_radius * blowup->proximity_radius))
            continue;
        if (special) {
            blowup->status_flags |= 0x10;
            hit_type = 5;
        } else {
            blowup->status_flags |= 0x20;
            hit_type = 4;
        }
        animation = NuSpecialGetInstAnim(&blowup->type->animated_special);
        if (blowup->proximity_delay > 0.0f) {
            blowup->activation_delay = blowup->proximity_delay;
            blowup->status_flags |= 0x40000;
        } else if (animation == NULL || (blowup->status_flags & 0x80000) != 0) {
            destroy = 1;
        } else {
            blowup->status_flags |= 0x2080000;
            blowup->animation_time = 0.0f;
        }
        if (destroy) {
            GizmoBlowupBlowup(blowup, 1, hit_type, 1, object, 1);
            ++count;
        }
    }
    return count;
}
