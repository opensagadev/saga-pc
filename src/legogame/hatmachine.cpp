#include "batman.h"
#include "gameapi/edptl.h"
#include "gameapi/edtools/edfile.h"
#include "globals.h"
#include "legoapi/actions/movement/climbing.h"
#include "legoapi/ai/core/gameai.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/animlist.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/object/hatmachine.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/light/shadow.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/NuMotionAccumFilter.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
extern "C" {
    static f32 HatmachineFabs(f32 f) {
        f32 rv;

        *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
        return rv;
    }
}

extern "C" void Hat_GetAbsTargetPos(HATMACHINE *machine, NUVEC *position) {
    if (machine != NULL && position != NULL) {
        NUVEC offset = machine->target_offset;
        NuVecRotateY(&offset, &offset, machine->yaw);
        offset.x += machine->position.x;
        offset.z += machine->position.z;
        *position = offset;
    }
}

extern "C" void HatMachines_InitTerrain(WORLDINFO *world) {
    if (world->hat_machine_sys != NULL) {
        for (i32 index = 0; index < world->hat_machine_sys->count; ++index) {
            HATMACHINE *machine = &world->hat_machine_sys->machines[index];
            world->hat_machine_sys->machines[index].platform_id = NewPlatPickupInst(machine, 2);
            PlatInstRotate(world->hat_machine_sys->machines[index].platform_id, 1);
        }
    }
}

extern "C" HATMACHINE *HatMachine_FindNearest(WORLDINFO *world, NUVEC *position, GAMEOBJECT *object, f32 *distance) {
    f32 nearest_distance = 1.0e9f;
    HATMACHINE *nearest = NULL;
    if (world == NULL || world->hat_machine_sys == NULL) {
        return NULL;
    }
    for (i32 index = 0; index < world->hat_machine_sys->count; ++index) {
        HATMACHINE *machine = &world->hat_machine_sys->machines[index];
        f32 candidate_distance;
        if (object != NULL) {
            if ((machine->flags & HATMACHINE_FLAG_ENABLED) == 0 || (machine->flags & HATMACHINE_FLAG_VISIBLE) == 0 ||
                (machine->flags & (HATMACHINE_FLAG_IN_USE | HATMACHINE_FLAG_OUTPUT)) != 0 || machine->player_position.y == 2000000.0f) {
                continue;
            }
            NUVEC target;
            Hat_GetAbsTargetPos(machine, &target);
            candidate_distance = NuVecDistSqr(position, &target, NULL);
        } else {
            candidate_distance = NuVecDistSqr(position, &machine->position, NULL);
        }
        if (candidate_distance < nearest_distance) {
            nearest_distance = candidate_distance;
            nearest = machine;
        }
    }
    if (distance != NULL) {
        *distance = nearest_distance;
    }
    return nearest;
}

extern "C" i32 HatMachine_BeingUsed(HATMACHINE *machine) {
    return machine->flags & HATMACHINE_FLAG_IN_USE;
}

struct HAT_USE_REQUEST {   // size 0x7a4
    u8 unknown_000[0x79c]; // 0x000
    i32 request;           // 0x79c
    void *net_wait_info;   // 0x7a0
};

extern "C" i16 id_PRINCESSLEIABOUSHH;

extern "C" void HatMachine_MoveCode(WORLDINFO *world, GAMEOBJECT *object, i32 special_pressed) {
    HATMACHINE *machine = object->player.hat_machine;
    if (object->player.use_cooldown > 0.0f) {
        object->player.use_cooldown -= FRAMETIME;
    }
    if (object->player.character_context != CHARACTER_CONTEXT_HAT_MACHINE || machine == NULL) {
        if (object->character_model->model_data_b[0x5d] == NULL || object->ground_flags == 0 || ObjLandReady(object) == 0) {
            return;
        }
        f32 distance;
        machine = HatMachine_FindNearest(world, &object->collision_position, object, &distance);
        if (machine == NULL) {
            return;
        }
        f32 range = (f64)object->bounds_radius + 0.25;
        range *= machine->scale;
        if (distance < range &&
            (special_pressed != 0 || (((HAT_USE_REQUEST *)object)->request == 2 && ((HAT_USE_REQUEST *)object)->net_wait_info != NULL))) {
            object->player.grapple = machine;
            object->movement_facing_angle = machine->yaw;
            object->player.context_hover_target_y = 0.0f;
            object->player.delayed_turn_timer = 0.0f;
            object->player.flags_e20 &= ~0x1000;
            object->player.character_context = CHARACTER_CONTEXT_HAT_MACHINE;
            object->player.context_phase = 0;
            FastWeaponIn(object, 0);
            object->player.flags_e24 |= APIOBJECT_AT_INTERFACE;
            object->player.context_animation = ACTION_PULLLEVER;
            machine->animation_state = HATMACHINE_ANIM_LID_OPENING;
            if (StartupGameFlag1 != 0) {
                machine->state_duration = 3.0f;
            } else {
                machine->state_duration = AnimDuration((i16)object->id, object->player.context_animation, 0.0f, 0.0f, 1);
            }
            f32 end_frame = -AnimListFrame(object->character_model, object->player.context_animation, 0);
            machine->flags |= HATMACHINE_FLAG_IN_USE;
            machine->state_elapsed = end_frame;
            machine->state_duration -= end_frame;
            machine->idle_bounce_timer = 0.0f;
            LoseHelmet(object, 0, 0);
            NewRumble(object->player.pad->bound_pad, 0.5f, 0);
            if (object->anim_blending != 0) {
                ResetAnimPacket(&object->anim_packet, -1);
            }
            AlertSurroundingCreatures(object, &object->collision_position);
            object->player.context_timer = AnimDuration((i16)object->id, object->player.context_animation, 0.0f, 0.0f, 1);
            object->player.hat_machine->state_duration = object->player.context_timer;
            if (object->player.context_timer <= 0.0f) {
                object->player.context_timer = 2.0f;
            }
            object->player.use_cooldown = 0.0f;
            return;
        }
        if ((object->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 &&
            (object->character_data->model_flags & CHARACTER_MODEL_PROTOCOL) != 0 && object->player.use_cooldown <= 0.0f) {
            PlaySfx("TC14_VLN", &object->collision_position);
            object->player.use_cooldown = 0.5f;
        }
        return;
    }
    if (object->character_model->model_data_b[object->player.context_animation] == NULL ||
        AnimPlaying(&object->anim_packet, object->player.context_animation, 1, 0) != NULL) {
        object->player.context_hover_target_y += FRAMETIME;
        if (object->player.context_hover_target_y > 1.0f) {
            object->player.context_hover_target_y = 1.0f;
        }
        object->player.context_timer -= FRAMETIME;
        switch (object->player.context_phase) {
            case 0:
                if (!(object->player.context_timer <= 0.0f)) {
                    return;
                }
                if (object->character_model->model_data_b[0x6d] != NULL &&
                    ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_ALREADY_GOT_HAT) != 0 ||
                     (object->id == (u16)id_PRINCESSLEIABOUSHH && FreePlay != 0))) {
                    if (machine->animation_state == HATMACHINE_ANIM_ARMS_OUT) {
                        object->player.context_animation = ACTION_HELMETON2;
                        object->player.context_phase = 1;
                        object->player.context_timer = AnimDuration((i16)object->id, 0x6d, 0.0f, 0.0f, 1);
                        machine->state_duration = (f64)object->player.context_timer * (f64)0.35f;
                        return;
                    }
                    object->player.context_animation = ACTION_IDLE;
                    return;
                }
                if (object->character_model->model_data_b[0x5e] != NULL) {
                    if (machine->animation_state == HATMACHINE_ANIM_ARMS_OUT) {
                        object->player.context_animation = ACTION_HELMETON;
                        object->player.context_phase = 1;
                        object->player.context_timer = AnimDuration((i16)object->id, 0x5e, 0.0f, 0.0f, 1);
                        if (StartupGameFlag1 == 0) {
                            machine->state_duration = (f64)object->player.context_timer * (f64)0.35f;
                        }
                        return;
                    }
                    object->player.context_animation = ACTION_IDLE;
                    return;
                }
                if (machine->animation_state != HATMACHINE_ANIM_ARMS_OUT) {
                    return;
                }
                object->player.context_animation = ACTION_IDLE;
                object->player.context_phase = 1;
                machine->state_duration = 0.2f;
                return;
            case 1:
                if (machine->animation_state != HATMACHINE_ANIM_ARMS_UP) {
                    return;
                }
                if ((object->character_data->game_character->flags_090 & CHARACTER_RUNTIME_ALREADY_GOT_HAT) == 0 &&
                    (object->id != (u16)id_PRINCESSLEIABOUSHH || FreePlay == 0)) {
                    PlaySfx("HatOn", &object->upper_position);
                    object->helmet = object->player.hat_machine->displayed_hat;
                    if (object->helmet == HELMET_STORMTROOPER) {
                        MakeBaddiesForgetAboutParty(1);
                    }
                }
                machine->displayed_hat = 0;
                object->player.context_phase = 2;
                if (StartupGameFlag1 != 0) {
                    machine->animation_state = HATMACHINE_ANIM_LID_CLOSING;
                }
                break;
            case 2:
                break;
            default:
                return;
        }
        if (!(object->player.context_timer <= 0.0f)) {
            return;
        }
        object->player.character_context = CHARACTER_CONTEXT_NONE;
        object->player.context_timer = 0.0f;
        machine = object->player.hat_machine;
        if (machine == NULL) {
            return;
        }
        if (machine->configured_hat == HELMET_BOUNTY_HUNTER) {
            PlaySfx("Hunter_Granted", &object->collision_position);
        }
        if (object->player.hat_machine->configured_hat == HELMET_STORMTROOPER) {
            PlaySfx("Trooper_Granted", &object->collision_position);
        }
        object->player.grapple = NULL;
    } else {
        object->player.context_timer -= FRAMETIME;
        if (!(object->player.context_timer <= 0.0f)) {
            return;
        }
        if (object->player.grapple != NULL) {
            NewBuzz(object->player.pad->bound_pad, 0.1f, 0);
            object->player.hat_machine->hat_delay = 0.6f;
            if (object->player.hat_machine->configured_hat == HELMET_BOUNTY_HUNTER) {
                PlaySfx("Hunter_Granted", &object->collision_position);
            }
            if (object->player.hat_machine->configured_hat == HELMET_STORMTROOPER) {
                PlaySfx("Trooper_Granted", &object->collision_position);
            }
        } else {
            NewRumble(object->player.pad->bound_pad, 0.5f, 0);
        }
        object->player.grapple = NULL;
        object->player.character_context = CHARACTER_CONTEXT_NONE;
    }
}

struct HATMACHINE_PROGRESS { // size 0xc
    i32 preserved_state;     // 0x0
    u32 enabled_mask[1];     // 0x4
    u32 visible_mask[1];     // 0x8
};

extern "C" ADDGIZMOTYPE Default_ADDGIZMOTYPE;
extern "C" i32 hatmachine_gizmotype_id;
extern "C" {
    static void HatMachine_Draw(void *, void *, f32);
}
extern "C" {
    static i32 HatMachine_GetMaxGizmos(void *world_ptr) {
        WORLDINFO *world = (WORLDINFO *)world_ptr;
        return world != NULL ? world->current_level->max_hatmachines : 0;
    }

    static char *HatMachine_GetGizmoName(GIZMO *gizmo) {
        if (gizmo == NULL || gizmo->object == NULL) {
            return NULL;
        }
        return gizmo->hat_machine->name;
    }

    static i32 HatMachine_GetOutput(GIZMO *gizmo, i32, i32) {
        return (gizmo->hat_machine->flags >> 1) & 1;
    }

    static char *HatMachine_GetOutputName(GIZMO *, i32 output_index) {
        return output_index == 0 ? (char *)"Finished" : NULL;
    }

    static i32 HatMachine_GetNumOutputs(GIZMO *) {
        return 1;
    }

    static i32 HatMachine_Load(void *world_ptr, void *) {
        WORLDINFO *world = (WORLDINFO *)world_ptr;
        if (world == NULL || world->hat_machine_sys == NULL) {
            return 0;
        }
        HATMACHINE_SYS *system = world->hat_machine_sys;
        if (system->count != 0) {
            return 0;
        }
        const i32 version = EdFileReadInt();
        system->count = EdFileReadInt();
        for (i32 index = 0; index < system->count; ++index) {
            EdFileRead(system->machines[index].name, EdFileReadInt());
            EdFileReadNuVec(&system->machines[index].position);
            system->machines[index].yaw = EdFileReadShort();
            system->machines[index].configured_hat = EdFileReadChar();
            if (version >= 3) {
                system->machines[index].model_letter = EdFileReadChar();
            } else {
                system->machines[index].model_letter = 'r';
            }
            if (version >= 4) {
                EdFileReadNuVec(&system->machines[index].target_offset);
                system->machines[index].scale = EdFileReadFloat();
            } else {
                system->machines[index].target_offset.y = 0.0f;
                system->machines[index].target_offset.x = 0.0f;
                system->machines[index].target_offset.z = -0.1441f;
                system->machines[index].scale = 1.0f;
            }
            if (version >= 5) {
                u8 hidden = EdFileReadChar();
                system->machines[index].flags = (system->machines[index].flags & ~HATMACHINE_FLAG_HIDE_TARGET) | ((hidden & 1) << 5);
            }
            system->machines[index].platform_id = -1;
        }
        return 1;
    }

    static void *HatMachines_ReserveBufferSpace(void *world_ptr) {
        WORLDINFO *world = (WORLDINFO *)world_ptr;
        world->hat_machine_sys = NULL;
        if (world->current_level->max_levers > 0) {
            world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
            world->hat_machine_sys = (HATMACHINE_SYS *)world->giz_buffer.void_ptr;
            world->giz_buffer.addr += sizeof(HATMACHINE_SYS);
            memset(world->hat_machine_sys, 0, sizeof(HATMACHINE_SYS));
            world->giz_buffer.addr = (world->giz_buffer.addr + 15) & ~15u;
            world->hat_machine_sys->machines = (HATMACHINE *)world->giz_buffer.void_ptr;
            world->giz_buffer.addr += world->current_level->max_hatmachines * sizeof(HATMACHINE);
            memset(world->hat_machine_sys->machines, 0, world->current_level->max_hatmachines * sizeof(HATMACHINE));
        }
        return world->hat_machine_sys;
    }

    static void HatMachines_ClearProgress(void *, void *progress_ptr) {
        HATMACHINE_PROGRESS *progress = (HATMACHINE_PROGRESS *)progress_ptr;
        if (progress != NULL) {
            progress->enabled_mask[0] = ~0u;
            progress->visible_mask[0] = ~0u;
        }
    }

    static void *HatMachines_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
        return GizmoBufferAlloc(buffer, end, sizeof(HATMACHINE_PROGRESS));
    }

    static void HatMachine_SetVisibility(GIZMO *gizmo, i32 visible) {
        if (gizmo == NULL || gizmo->object == NULL) {
            return;
        }
        HATMACHINE *machine = gizmo->hat_machine;
        const i32 was_visible = (machine->flags >> 2) & 1;
        machine->visible = visible != 0;
        if (machine->visible && was_visible == 0) {
            machine->platform_id = NewPlatPickupInst(machine, 2);
            PlatInstRotate(machine->platform_id, 1);
        } else if (!machine->visible && was_visible != 0) {
            DeletePlatinst(machine->platform_id);
        }
    }

    static void HatMachine_AddGizmos(void *gizmo_sys, i32 type_id, void *world_ptr, void *) {
        WORLDINFO *world = (WORLDINFO *)world_ptr;
        if (world == NULL || world->hat_machine_sys == NULL) {
            return;
        }
        for (i32 index = 0; index < world->hat_machine_sys->count; ++index) {
            if (NuStrLen(world->hat_machine_sys->machines[index].name) != 0) {
                AddGizmo(gizmo_sys, type_id, NULL, &world->hat_machine_sys->machines[index]);
            }
        }
    }

    static void HatMachines_StoreProgress(void *world_ptr, void *, void *progress_ptr) {
        HATMACHINE_PROGRESS *progress = (HATMACHINE_PROGRESS *)progress_ptr;
        if (progress == NULL) {
            return;
        }
        progress->enabled_mask[0] = ~0u;
        progress->visible_mask[0] = ~0u;
        WORLDINFO *world = (WORLDINFO *)world_ptr;
        if (world == NULL || world->hat_machine_sys == NULL || world->hat_machine_sys->machines == NULL) {
            return;
        }
        for (i32 index = 0; index < world->hat_machine_sys->count && index < 32; ++index) {
            HATMACHINE *machine = &world->hat_machine_sys->machines[index];
            const i32 word = index / 32;
            const u32 mask = 1u << (index % 32);
            if ((machine->flags & HATMACHINE_FLAG_VISIBLE) == 0) {
                progress->visible_mask[word] &= ~mask;
            }
            if ((machine->flags & HATMACHINE_FLAG_ENABLED) == 0) {
                progress->enabled_mask[word] &= ~mask;
            }
        }
    }

    static void HatMachine_Reset(HATMACHINE *machine) {
        machine->player_position.y = 0.0f;
        machine->player_position.x = 0.0f;
        machine->player_position.z = 0.1441f;
        NuVecRotateY(&machine->player_position, &machine->player_position, machine->yaw + 0x8000);
        NuVecAdd(&machine->player_position, &machine->player_position, &machine->position);
        NUVEC target;
        Hat_GetAbsTargetPos(machine, &target);
        target.y = machine->position.y;
        machine->player_position.y = GameShadow(NULL, &machine->player_position, 1.0f, -1);
        const f32 height = GameShadow(NULL, &target, 1.0f, -1);
        machine->target_offset.y = height;
        if (height != 2000000.0f) {
            machine->target_offset.y = (f64)height + (f64)0.005f;
            FindAnglesZX(&ShadNorm, &machine->terrain_pitch, &machine->terrain_roll);
        }
        machine->flags =
            (machine->flags & ~(HATMACHINE_FLAG_IN_USE | HATMACHINE_FLAG_OUTPUT)) | HATMACHINE_FLAG_VISIBLE | HATMACHINE_FLAG_ENABLED;
        machine->animation_state = HATMACHINE_ANIM_IDLE;
        machine->hat_delay = 0.0f;
        machine->state_duration = 0.0f;
        machine->animation_time = 0.0f;
        if (machine->model_letter == 'r') {
            machine->model_special_index = 0x22;
        } else {
            machine->model_special_index = 0x20 + (machine->model_letter == 'o');
        }
        if (machine->configured_hat != 0) {
            machine->displayed_hat = machine->configured_hat;
        } else {
            machine->displayed_hat = 1 - (i32)(NuFloatRand(&GAMERAND) * -4.0);
        }
        NuMtxSetRotationY(&machine->matrix, machine->yaw);
        NuMtxTranslate(&machine->matrix, &machine->position);
    }

    static void HatMachines_Reset(void *world_ptr, void *, void *progress_ptr) {
        WORLDINFO *world = (WORLDINFO *)world_ptr;
        if (world == NULL) {
            return;
        }
        HATMACHINE_SYS *system = world->hat_machine_sys;
        if (system == NULL || system->machines == NULL) {
            return;
        }
        HATMACHINE_PROGRESS *progress = (HATMACHINE_PROGRESS *)progress_ptr;
        for (i32 index = 0; index < system->count; ++index) {
            HATMACHINE *machine = &system->machines[index];
            HatMachine_Reset(machine);
            if (progress != NULL && index < 32) {
                const i32 word = index / 32;
                const u32 mask = 1u << (index % 32);
                machine->visible = (progress->visible_mask[word] & mask) != 0;
                machine->enabled = (progress->enabled_mask[word] & mask) != 0;
            }
        }
    }

    static void HatMachine_Activate(GIZMO *gizmo, i32 enabled) {
        if (gizmo == NULL || gizmo->object == NULL) {
            return;
        }
        HATMACHINE *machine = gizmo->hat_machine;
        machine->enabled = enabled != 0;
        if (enabled != 0) {
            HatMachine_Reset(machine);
        }
    }

    static void HatMachine_Update(void *world_ptr, void *, f32 elapsed) {
        WORLDINFO *world = (WORLDINFO *)world_ptr;
        HATMACHINE_SYS *system = world->hat_machine_sys;
        if (system == NULL) {
            return;
        }
        for (i32 index = 0; index < system->count; ++index) {
            HATMACHINE *machine = &system->machines[index];
            if (machine->hat_delay > 0.0f) {
                machine->hat_delay -= FRAMETIME;
                if (machine->hat_delay <= 0.0f && machine->displayed_hat == 0) {
                    if (machine->configured_hat == 0) {
                        machine->displayed_hat = 1 - (u8)(NuFloatRand(&GAMERAND) * -4.0);
                    } else {
                        machine->displayed_hat = machine->configured_hat;
                    }
                    machine->hat_delay = 1.0f;
                }
            }
            if ((machine->flags & HATMACHINE_FLAG_IN_USE) != 0 && machine->animation_state > HATMACHINE_ANIM_IDLE) {
                machine->state_elapsed += elapsed;
                if (machine->state_elapsed > machine->state_duration) {
                    ++machine->animation_state;
                    if (machine->animation_state == HATMACHINE_ANIM_ARMS_OUT) {
                        PlaySfx("SwLever", &machine->position);
                    }
                    machine->state_elapsed = 0.0f;
                    if (machine->animation_state > HATMACHINE_ANIM_LID_CLOSING) {
                        machine->flags &= ~HATMACHINE_FLAG_IN_USE;
                        machine->animation_state = HATMACHINE_ANIM_IDLE;
                        machine->state_duration = 0.0f;
                        machine->hat_delay = 0.6f;
                        machine->animation_time = 0.0f;
                    }
                }
            }
        }
    }
}

extern "C" ADDGIZMOTYPE *HatMachine_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE HatMachineAddType = {0};
    HatMachineAddType = Default_ADDGIZMOTYPE;
    HatMachineAddType.name = (char *)"HatMachine";
    HatMachineAddType.prefix = (char *)"";
    HatMachineAddType.fns.options = sizeof(HATMACHINE_PROGRESS);
    HatMachineAddType.fns.get_max_gizmos_fn = HatMachine_GetMaxGizmos;
    HatMachineAddType.fns.add_gizmos_fn = (void *)HatMachine_AddGizmos;
    HatMachineAddType.fns.early_update_fn = NULL;
    HatMachineAddType.fns.late_update_fn = HatMachine_Update;
    HatMachineAddType.fns.draw_fn = HatMachine_Draw;
    HatMachineAddType.fns.panel_draw_fn = NULL;
    HatMachineAddType.fns.get_gizmo_name_fn = (char *(*)(struct gizmo_s *))HatMachine_GetGizmoName;
    HatMachineAddType.fns.get_output_fn = (i32(*)(struct gizmo_s *, i32, i32))HatMachine_GetOutput;
    HatMachineAddType.fns.get_output_name_fn = (char *(*)(struct gizmo_s *, i32))HatMachine_GetOutputName;
    HatMachineAddType.fns.get_num_outputs_fn = (void *)HatMachine_GetNumOutputs;
    HatMachineAddType.fns.activate_fn = (void (*)(struct gizmo_s *, i32))HatMachine_Activate;
    HatMachineAddType.fns.activate_rev_fn = NULL;
    HatMachineAddType.fns.set_visibility_fn = (void (*)(struct gizmo_s *, i32))HatMachine_SetVisibility;
    HatMachineAddType.fns.get_visibility_fn = NULL;
    HatMachineAddType.fns.get_pos_fn = NULL;
    HatMachineAddType.fns.using_special_fn = NULL;
    HatMachineAddType.fns.bolt_hit_plat_fn = NULL;
    HatMachineAddType.fns.get_best_bolt_target_fn = NULL;
    HatMachineAddType.fns.bolt_hit_fn = NULL;
    HatMachineAddType.fns.allocate_progress_data_fn = HatMachines_AllocateProgressData;
    HatMachineAddType.fns.clear_progress_fn = HatMachines_ClearProgress;
    HatMachineAddType.fns.store_progress_fn = HatMachines_StoreProgress;
    HatMachineAddType.fns.reset_fn = HatMachines_Reset;
    HatMachineAddType.fns.reserve_buffer_space_fn = HatMachines_ReserveBufferSpace;
    HatMachineAddType.fns.load_fn = HatMachine_Load;
    HatMachineAddType.fns.post_load_fn = NULL;
    HatMachineAddType.fns.add_level_sfx_fn = NULL;
    hatmachine_gizmotype_id = type_id;
    return &HatMachineAddType;
}

extern "C" {
    i16 id_PRINCESSLEIABOUSHH = -1;
    i32 hatmachine_gizmotype_id = -1;
}
static const NUVEC HatMachine_HatOffset = {0.0f, 0.3f, 0.0f};
static const i32 HatMachineDrawSpecialIndices[4] = {288, 285, 289, 284};

static inline f32 NuFdiv(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f) {
        return 0.0f;
    }
    return a / b;
}

extern "C" void HatMachine_Draw(void *world_ptr, void *, f32) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    HATMACHINE_SYS *system;
    if (world == NULL || (system = world->hat_machine_sys) == NULL || system->count == 0) {
        return;
    }
    NUVEC draw_position;
    u16 spin_angle = (u16)((f64)NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0 * 65536.0);
    f32 target_pulse =
        (f64)NuTrigTable[((i32)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0) >> 1) & 0x7fff] * (f64)0.2f +
        (f64)0.8f;
    f32 ready_alpha =
        (f64)NuTrigTable[((i32)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0) >> 1) & 0x7fff] * (f64)0.15f +
        (f64)0.85f;
    EnableShadowMapRendering(0);
    nuhspecial_s *specials[4];
    NUINSTANIM *animations[4];
    f32 end_frames[4];
    if (StartupGameFlag1 != 0) {
        for (i32 effect = 0; effect < 4; ++effect) {
            specials[effect] = &world->level_objects[HatMachineDrawSpecialIndices[effect]].special;
            animations[effect] = (NUINSTANIM *)NuSpecialGetInstAnim(specials[effect]);
            if (specials[effect] != NULL && animations[effect] != NULL) {
                end_frames[effect] =
                    NuAnimEndFrameOld(specials[effect]->scene->instance_animation_data[animations[effect]->animation_index]);
            }
        }
    }
    for (i32 index = 0; index < system->count; ++index) {
        HATMACHINE *machine = &system->machines[index];
        if ((machine->flags & HATMACHINE_FLAG_VISIBLE) == 0 && editor_active == 0) {
            continue;
        }
        f32 frame = machine->state_elapsed;
        if (frame < 0.0f) {
            frame = 0.0f;
        }
        if (world->level_objects[283].active != 0) {
            NuSpecialDrawAt(&world->level_objects[283].special, &machine->matrix);
        }
        if ((machine->flags & HATMACHINE_FLAG_ENABLED) != 0) {
            if (world->level_objects[286].active != 0) {
                NuSpecialDrawAt(&world->level_objects[286].special, &machine->matrix);
            }
        } else if (world->level_objects[287].active != 0) {
            NuSpecialDrawAt(&world->level_objects[287].special, &machine->matrix);
        }
        NUMTX matrix;
        NUVEC offset;

        union {
            f32 value;
            u32 bits;
        } magnitude;

        if ((machine->flags & HATMACHINE_FLAG_HIDE_TARGET) == 0 && machine->target_offset.y != 2000000.0f &&
            world->level_objects[85].active != 0) {
            NUVEC target;
            Hat_GetAbsTargetPos(machine, &target);
            NuMtxSetRotationY(&matrix, spin_angle);
            if (machine->terrain_roll != 0) {
                NuMtxRotateZ(&matrix, machine->terrain_roll);
            }
            if (machine->terrain_pitch != 0) {
                NuMtxRotateX(&matrix, machine->terrain_pitch);
            }
            NuMtxScaleU(&matrix, 0.8f);
            NuMtxTranslate(&matrix, &target);
            NuMtxPreScaleU(&matrix, machine->scale);
            NuSpecialDrawAtAlpha(&world->level_objects[85].special, &matrix,
                                 (machine->flags & HATMACHINE_FLAG_IN_USE) != 0 ? 0.0f : target_pulse);
        }
        if (StartupGameFlag1 != 0) {
            if (specials[0] != NULL && animations[0] != NULL) {
                if (machine->animation_state > HATMACHINE_ANIM_IDLE) {
                    machine->animation_time += FRAMETIME;
                    if (machine->animation_state < HATMACHINE_ANIM_ARMS_UP && machine->animation_time >= 3.0f) {
                        machine->animation_state = HATMACHINE_ANIM_ARMS_UP;
                        machine->state_elapsed = 0.0f;
                        machine->state_duration = 2.0f;
                    }
                    frame = (f64)animations[0]->tfactor * machine->animation_time * 60.0;
                    if (end_frames[0] <= frame) {
                        frame = end_frames[0];
                    }
                } else {
                    machine->animation_time = 0.0f;
                    frame = 0.0f;
                    if ((machine->flags & HATMACHINE_FLAG_ENABLED) != 0 &&
                        (machine->flags & (HATMACHINE_FLAG_IN_USE | HATMACHINE_FLAG_OUTPUT)) == 0) {
                        if (machine->idle_bounce_timer > 0.0f) {
                            machine->idle_bounce_timer -= FRAMETIME;
                            if (machine->idle_bounce_timer < 0.0f) {
                                machine->idle_bounce_timer = 0.0f;
                            } else {
                                frame =
                                    NuTrigTable[((i32)((f64)machine->idle_bounce_timer / (f64)0.15f * 32768.0 + 16384.0) >> 1) & 0x7fff];
                                magnitude.value = HatmachineFabs(frame);
                                frame = (1.0 - magnitude.value) * 1.5 + 16.0;
                            }
                        } else if (qrand() < 0x800) {
                            machine->idle_bounce_timer = 0.15f;
                        }
                    }
                }
                EvalAnim(specials[0], frame, &matrix, 0);
                NuMtxMulVU0(&matrix, &matrix, &machine->matrix);
                NuSpecialDrawAt(specials[0], &matrix);
                if (world->level_objects[machine->model_special_index].active != 0) {
                    NuSpecialDrawAt(&world->level_objects[machine->model_special_index].special, &matrix);
                }
                if ((machine->flags & HATMACHINE_FLAG_ENABLED) != 0 &&
                    (machine->flags & (HATMACHINE_FLAG_IN_USE | HATMACHINE_FLAG_OUTPUT)) == 0 && world->level_objects[41].active != 0) {
                    NuSpecialDrawAtAlpha(&world->level_objects[41].special, &matrix, ready_alpha);
                }
            }
            draw_position = machine->position;
            for (i32 effect = 1; effect < 4; ++effect) {
                if (specials[effect] != NULL && animations[effect] != NULL) {
                    frame = (f64)machine->animation_time * animations[0]->tfactor * 60.0;
                    if (end_frames[effect] <= frame) {
                        frame = end_frames[effect];
                    }
                    EvalAnim(specials[effect], frame, &matrix, 0);
                    NuMtxMulVU0(&matrix, &matrix, &machine->matrix);
                    NuSpecialDrawAt(specials[effect], &matrix);
                    if (effect == 1) {
                        draw_position.x = matrix.m30;
                        draw_position.z = matrix.m32;
                    }
                }
            }
            if ((machine->flags & HATMACHINE_FLAG_ENABLED) != 0 && machine->displayed_hat != 0 &&
                world->level_objects[machine->displayed_hat + 249].active != 0) {
                offset = HatMachine_HatOffset;
                if (machine->animation_time < 2.35f) {
                    f32 hat_scale =
                        1.0 - (1.0 - ((f64)NuTrigTable[((i32)((f64)machine->hat_delay * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5);
                    offset.y = (1.0 - hat_scale) * (f64)0.1f + HatMachine_HatOffset.y;
                    if (machine->animation_state < HATMACHINE_ANIM_HAT_DOWN) {
                        frame = NuFdiv(frame, 50.0f);
                        if (frame > 1.0f) {
                            frame = 1.0f;
                        }
                        f32 bob = (1.0 - frame) * (f64)0.01f;
                        offset.y += NuTrigTable[((i32)((f64)GameTimer.time_elapsed * 32768.0) >> 1) & 0x7fff] * bob;
                    }
                    NuVecRotateY(&offset, &offset, machine->yaw);
                    NuMtxSetRotationY(&matrix, machine->yaw + 0x8000);
                    NuMtxTranslate(&matrix, &draw_position);
                    NuMtxTranslate(&matrix, &offset);
                    NuMtxPreScaleU(&matrix, hat_scale);
                    NuSpecialDrawAt(&world->level_objects[machine->displayed_hat + 249].special, &matrix);
                }
            }
        } else {
            if (world->level_objects[30].active != 0) {
                NuMtxSetRotationY(&matrix, machine->yaw);
                switch (machine->animation_state) {
                    case HATMACHINE_ANIM_LID_OPENING:
                        NuMtxPreRotateX(&matrix, (u16)((f64)frame / machine->state_duration * -32768.0));
                        break;
                    case HATMACHINE_ANIM_ARMS_OUT:
                    case HATMACHINE_ANIM_HAT_DOWN:
                    case HATMACHINE_ANIM_ARMS_UP:
                    case HATMACHINE_ANIM_ARMS_IN:
                        NuMtxPreRotateX(&matrix, 0x8000);
                        break;
                    case HATMACHINE_ANIM_LID_CLOSING:
                        NuMtxPreRotateX(&matrix, (u16)((1.0 - (f64)frame / machine->state_duration) * -32768.0));
                        break;
                }
                NuMtxTranslate(&matrix, &machine->position);
                NuSpecialDrawAt(&world->level_objects[30].special, &matrix);
                if (world->level_objects[machine->model_special_index].active != 0) {
                    NuSpecialDrawAt(&world->level_objects[machine->model_special_index].special, &matrix);
                }
                if ((machine->flags & HATMACHINE_FLAG_ENABLED) != 0 &&
                    (machine->flags & (HATMACHINE_FLAG_IN_USE | HATMACHINE_FLAG_OUTPUT)) == 0 && world->level_objects[41].active != 0) {
                    NuSpecialDrawAtAlpha(&world->level_objects[41].special, &matrix, ready_alpha);
                }
            }
            /* Draws the two arm specials with separate copies of the same code. */
            if (world->level_objects[285].active != 0) {
                offset = v000;
                switch (machine->animation_state) {
                    case HATMACHINE_ANIM_ARMS_OUT:
                        offset.z =
                            (1.0 + NuTrigTable[((i32)((f64)frame / machine->state_duration * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff]) *
                            (f64)-0.1441f;
                        break;
                    case HATMACHINE_ANIM_HAT_DOWN:
                        offset.z = -0.1441f;
                        offset.y =
                            (1.0 + NuTrigTable[((i32)((f64)frame / machine->state_duration * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff]) *
                            (f64)-0.35f;
                        break;
                    case HATMACHINE_ANIM_ARMS_UP:
                        offset.z = -0.1441f;
                        offset.y =
                            (1.0 - (NuTrigTable[((i32)((f64)frame / machine->state_duration * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff] +
                                    1.0)) *
                            (f64)-0.35f;
                        break;
                    case HATMACHINE_ANIM_ARMS_IN:
                        offset.z =
                            (1.0 - NuTrigTable[((i32)((f64)frame / machine->state_duration * 16384.0 + 49152.0 + 16384.0) >> 1) & 0x7fff]) *
                            (f64)-0.1441f;
                        break;
                }
                NuVecRotateY(&offset, &offset, machine->yaw);
                NuMtxSetRotationY(&matrix, machine->yaw);
                NuMtxTranslate(&matrix, &machine->position);
                NuMtxTranslate(&matrix, &offset);
                NuSpecialDrawAt(&world->level_objects[285].special, &matrix);
            }
            if (world->level_objects[284].active != 0) {
                offset = v000;
                switch (machine->animation_state) {
                    case HATMACHINE_ANIM_ARMS_OUT:
                        offset.z =
                            (1.0 + NuTrigTable[((i32)((f64)frame / machine->state_duration * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff]) *
                            (f64)-0.1441f;
                        break;
                    case HATMACHINE_ANIM_HAT_DOWN:
                        offset.z = -0.1441f;
                        offset.y =
                            (1.0 + NuTrigTable[((i32)((f64)frame / machine->state_duration * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff]) *
                            (f64)-0.35f;
                        break;
                    case HATMACHINE_ANIM_ARMS_UP:
                        offset.z = -0.1441f;
                        offset.y =
                            (1.0 - (NuTrigTable[((i32)((f64)frame / machine->state_duration * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff] +
                                    1.0)) *
                            (f64)-0.35f;
                        break;
                    case HATMACHINE_ANIM_ARMS_IN:
                        offset.z =
                            (1.0 - NuTrigTable[((i32)((f64)frame / machine->state_duration * 16384.0 + 49152.0 + 16384.0) >> 1) & 0x7fff]) *
                            (f64)-0.1441f;
                        break;
                }
                NuVecRotateY(&offset, &offset, machine->yaw);
                NuMtxSetRotationY(&matrix, machine->yaw);
                NuMtxTranslate(&matrix, &machine->position);
                NuMtxTranslate(&matrix, &offset);
                NuSpecialDrawAt(&world->level_objects[284].special, &matrix);
            }
            if ((machine->flags & HATMACHINE_FLAG_ENABLED) != 0 && machine->displayed_hat != 0 &&
                world->level_objects[machine->displayed_hat + 249].active != 0) {
                offset = HatMachine_HatOffset;
                switch (machine->animation_state) {
                    case HATMACHINE_ANIM_ARMS_OUT:
                        offset.z =
                            (f64)HatMachine_HatOffset.z -
                            (1.0 + NuTrigTable[((i32)((f64)frame / machine->state_duration * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff]) *
                                (f64)0.1441f;
                        break;
                    case HATMACHINE_ANIM_HAT_DOWN:
                        if (!((f64)frame / machine->state_duration < (f64)0.85f)) {
                            continue;
                        }
                        offset.z = (f64)HatMachine_HatOffset.z - (f64)0.1441f;
                        offset.y -=
                            (1.0 + NuTrigTable[((i32)((f64)frame / machine->state_duration * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff]) *
                            (f64)0.35f;
                        offset.y +=
                            (1.0 + NuTrigTable[((i32)((f64)frame / machine->state_duration * 16384.0 + 32768.0 + 16384.0) >> 1) & 0x7fff]) *
                            0.125;
                        break;
                    case HATMACHINE_ANIM_ARMS_UP:
                    case HATMACHINE_ANIM_ARMS_IN:
                        continue;
                }
                f32 hat_scale =
                    1.0 - (1.0 - ((f64)NuTrigTable[((i32)((f64)machine->hat_delay * 32768.0 + 16384.0) >> 1) & 0x7fff] + 1.0) * 0.5);
                offset.y += (1.0 - hat_scale) * (f64)0.1f;
                NuVecRotateY(&offset, &offset, machine->yaw);
                NuMtxSetRotationY(&matrix, machine->yaw + 0x8000);
                NuMtxTranslate(&matrix, &machine->position);
                NuMtxTranslate(&matrix, &offset);
                NuMtxPreScaleU(&matrix, hat_scale);
                NuSpecialDrawAt(&world->level_objects[machine->displayed_hat + 249].special, &matrix);
            }
        }
    }
    ResetShadowMapRendering();
}
