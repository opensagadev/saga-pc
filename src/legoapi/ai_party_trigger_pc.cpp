#include "saga.h"
typedef struct ai_trigger_world_pc_s {
    u8 unknown_0000[0x4678];
    AITRIGGERSETSYS_s *trigger_system;
} AI_TRIGGER_WORLD_PC;
typedef struct ai_trigger_grapple_pc_s {
    u8 unknown_00[0x24];
    NUVEC ground_position;
} AI_TRIGGER_GRAPPLE_PC;
typedef struct ai_trigger_object_pc_s {
    u8 unknown_0000[0x1072];
    u8 grapple_direction;
} AI_TRIGGER_OBJECT_PC;
STATIC_ASSERT(offsetof(AI_TRIGGER_WORLD_PC, trigger_system) == 0x4678);
STATIC_ASSERT(offsetof(AI_TRIGGER_GRAPPLE_PC, ground_position) == 0x24);
STATIC_ASSERT(offsetof(AI_TRIGGER_OBJECT_PC, grapple_direction) == 0x1072);
typedef struct ai_trigger_force_pc_s {
    u8 unknown_00[0x1c];
    NUVEC position;
    u8 unknown_28[0x4c];
    u32 config_flags;
} AI_TRIGGER_FORCE_PC;
STATIC_ASSERT(offsetof(AI_TRIGGER_FORCE_PC, position) == 0x1c);
STATIC_ASSERT(offsetof(AI_TRIGGER_FORCE_PC, config_flags) == 0x74);
STATIC_ASSERT(offsetof(GAMEOBJECT, field_1088) == 0x1088);
STATIC_ASSERT(offsetof(GAMEOBJECT, flags_ef4) == 0xef4);
STATIC_ASSERT(offsetof(GAMEOBJECT, gizforce_target) == 0x10cc);
STATIC_ASSERT(offsetof(AITRIGGERSET_s, trigger_count) == 0x20c);
STATIC_ASSERT(offsetof(AITRIGGERSET_s, targets) == 0x24);
STATIC_ASSERT(offsetof(AITRIGGERSET_TARGET, position) == 0x10);
STATIC_ASSERT(offsetof(AITRIGGERSET_TARGET, path) == 0x20);
extern "C" {
    void __cdecl ClearSpecialMove(GAMEOBJECT *object);
    f32 __cdecl GizSpinner_GetNearestTargetPoint(GIZSPINNER_s *, NUVEC *, NUVEC *, NUVEC *, i32);
    i32 Action_HelpWithTriggers(AISYS_PC *, AISCRIPTPROCESS_PC *processor, AIPACKET_PC *packet, char **, i32, i32,
                                f32 elapsed) {
        AITRIGGERSETSYS_s *system = ((AI_TRIGGER_WORLD_PC *)WORLD)->trigger_system;
        if (system == NULL || packet == NULL || packet->owner == NULL || packet->owner->objptr == NULL)
            return 0;
        GAMEOBJECT *object = packet->owner->objptr;
        u8 object_index = packet->owner->hostility_group;
        i32 trigger_index = system->field_0x42c0[object_index];
        if (trigger_index == -1)
            return 0;
        AITRIGGERSET_s *set = &system->sets[system->field_0x4280[object_index]];
        AITRIGGERSET_TARGET *target = &set->targets[trigger_index];
        GIZMO_PC *gizmo = set->triggers[trigger_index];
        NUVEC spinner_position, spinner_direction;
        NUVEC *spinner_origin = NULL;
        if (gizmo->type_id == spinner_gizmotype_id) {
            spinner_origin = &packet->owner->collision_position;
            GIZSPINNER_s *spinner = static_cast<GIZSPINNER_s *>(gizmo->object);
            f32 result =
                GizSpinner_GetNearestTargetPoint(spinner, spinner_origin, &spinner_position, &spinner_direction, 1);
            if (result == -1.0f)
                result =
                    GizSpinner_GetNearestTargetPoint(spinner, spinner_origin, &spinner_position, &spinner_direction, 0);
            if (result != -1.0f) {
                NuVecAddScale(&spinner_position, &spinner_position, &spinner_direction, 0.5f);
                AIMoveInstruction(packet, &spinner_position, 0.0f, &target->path, 1, 0.0f);
            }
        } else if (gizmo->type_id == grapple_gizmotype_id) {
            AI_TRIGGER_GRAPPLE_PC *grapple = static_cast<AI_TRIGGER_GRAPPLE_PC *>(gizmo->object);
            if (object->character_context == LEGOCONTEXT_GRAPPLE && object->grapple == grapple) {
                object->field_1088 = 5;
                GameObjectSetCanUse(object, grapple, 1, 50, 0.0f);
                ClearSpecialMove(object);
                for (i32 i = 0; i < 2; ++i) {
                    GAMEOBJECT *other = static_cast<GAMEOBJECT *>(Player[i]);
                    if (other == NULL || other->character_context != 0x46)
                        continue;
                    void *other_grapple = other->grapple;
                    for (i32 j = 0; j < set->trigger_count; ++j) {
                        if (set->triggers[j] != NULL && set->triggers[j]->object == other_grapple) {
                            if (player->position.y > object->position.y + (double)0.05f)
                                ((AI_TRIGGER_OBJECT_PC *)object)->grapple_direction = 1;
                            else if (object->position.y - (double)0.05f > player->position.y)
                                ((AI_TRIGGER_OBJECT_PC *)object)->grapple_direction = 2;
                            return 0;
                        }
                    }
                }
                return 0;
            }
            GameObjectSetCanUse(object, grapple, 1, 50, 0.0f);
            AIMoveInstruction(packet, &target->position, 0.0f, &target->path, 1, 0.0f);
        } else {
            AIMoveInstruction(packet, &target->position, 0.0f, &target->path, 1, 0.0f);
        }
        gizmo = set->triggers[trigger_index];
        if (gizmo == NULL)
            return 0;
        NUVEC offset;
        if (gizmo->type_id == lever_gizmotype_id) {
            if (object->character_model->action_animations[0x5d] == NULL)
                goto change_character;
            GIZMO_LEVER_PC *lever = static_cast<GIZMO_LEVER_PC *>(gizmo->object);
            if (NuVecXZDistSqr(&packet->terrain_origin, &target->position, &offset) < ai_moveradius * ai_moveradius) {
                packet->movement_look_target = &lever->position;
                object->pad->buttons_pressed |= GAMEPAD_SPECIAL;
            }
        } else if (gizmo->type_id == spinner_gizmotype_id) {
            if (object->field_281 != 0 &&
                NuVecXZDistSqr(spinner_origin, &spinner_position, NULL) < packet->mover_height) {
                NuVecAddScale(&spinner_position, &spinner_position, &spinner_direction, -0.5f);
                AIMoveInstruction(packet, &spinner_position, 0.0f, &target->path, 1, 0.0f);
                object->flags_ef4 |= 0x20000;
                object->respawn_timer = 0.0f;
            }
        } else if (gizmo->type_id == force_gizmotype_id) {
            AI_TRIGGER_FORCE_PC *force = static_cast<AI_TRIGGER_FORCE_PC *>(gizmo->object);
            if (CharCategory_IsCategory(object, (force->config_flags & 0x10) != 0 ? 1 : 0) == 0)
                goto change_character;
            if (NuVecXZDistSqr(&packet->terrain_origin, &target->position, &offset) < ai_moveradius * ai_moveradius) {
                packet->movement_look_target = &force->position;
                object->pad->runtime_flags |= 4;
                object->gizforce_target = force;
            }
        } else if (gizmo->type_id == grapple_gizmotype_id) {
            AI_TRIGGER_GRAPPLE_PC *grapple = static_cast<AI_TRIGGER_GRAPPLE_PC *>(gizmo->object);
            if (processor->action_data_1 != 0) {
                if (object->character_context != 0) {
                    processor->action_data_1 = 1;
                    return 0;
                }
                GameObjectSetCanUse(object, grapple, 1, 50, 0.0f);
                AIMoveInstruction(packet, &grapple->ground_position, 0.0f, &target->path, 7,
                                  packet->movement_instruction_parameter);
                if (processor->action_data_1 != 0)
                    return 0;
            }
            if (NuVecXZDistSqr(&packet->terrain_origin, &target->position, &offset) < ai_moveradius * ai_moveradius) {
                processor->action_data_1 = 1;
                GameObjectSetCanUse(object, grapple, 1, 50, 0.0f);
                object->pad->buttons_pressed |= GAMEPAD_JUMP;
            }
        }
        return 0;
    change_character:
        if (FreePlay != 0) {
            processor->action_timer -= elapsed;
            if (processor->action_timer < 0.0f) {
                processor->action_timer = 0.5f;
                object->pad->buttons_pressed |= GAMEPAD_TOGGLERIGHT;
            }
        }
        return 0;
    }
}
