#include "saga.h"
struct HATMACHINE_PC {
    NUMTX matrix;
    char name[16];
    NUVEC position;
    u16 yaw;
    u8 configured_hat, displayed_hat;
    i8 animation_state;
    u8 model_letter, model_special_index, flags;
    NUVEC player_position, target_offset;
    u16 terrain_pitch, terrain_roll;
    i16 platform_id;
    u16 reserved_82;
    f32 hat_delay, state_duration, state_elapsed, scale, animation_time, idle_bounce_timer;
    u8 reserved_9c[4];
};
struct HATMACHINE_SYS_PC {
    i32 count, reserved_04, reserved_08;
    HATMACHINE_PC *machines;
};
struct HATMACHINE_WORLD_PC {
    u8 unknown_000[0x5074];
    HATMACHINE_SYS_PC *hat_machine_sys;
};
STATIC_ASSERT(sizeof(HATMACHINE_PC) == 0xa0);
STATIC_ASSERT(sizeof(HATMACHINE_SYS_PC) == 0x10);
STATIC_ASSERT(offsetof(HATMACHINE_SYS_PC, count) == 0);
STATIC_ASSERT(offsetof(HATMACHINE_SYS_PC, machines) == 0xc);
STATIC_ASSERT(offsetof(HATMACHINE_WORLD_PC, hat_machine_sys) == 0x5074);
STATIC_ASSERT(offsetof(HATMACHINE_PC, position) == 0x50);
STATIC_ASSERT(offsetof(HATMACHINE_PC, yaw) == 0x5c);
STATIC_ASSERT(offsetof(HATMACHINE_PC, flags) == 0x63);
STATIC_ASSERT(offsetof(HATMACHINE_PC, player_position) == 0x64);
STATIC_ASSERT(offsetof(HATMACHINE_PC, target_offset) == 0x70);
STATIC_ASSERT(offsetof(HATMACHINE_PC, platform_id) == 0x80);
extern "C" void __cdecl Hat_GetAbsTargetPos(HATMACHINE_PC *machine, NUVEC *position) {
    if (machine != NULL && position != NULL) {
        NUVEC offset = machine->target_offset;
        NuVecRotateY(&offset, &offset, machine->yaw);
        offset.x += machine->position.x;
        offset.z += machine->position.z;
        *position = offset;
    }
}
extern "C" void __cdecl HatMachines_InitTerrain(WORLDINFO *world) {
    if (((HATMACHINE_WORLD_PC *)world)->hat_machine_sys != NULL) {
        for (i32 index = 0; index < ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->count; ++index) {
            HATMACHINE_PC *machine = &((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->machines[index];
            i16 platform = NewPlatPickupInst(machine, 2);
            ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->machines[index].platform_id = platform;
            PlatInstRotate(((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->machines[index].platform_id, 1);
        }
    }
}
extern "C" HATMACHINE_PC *__cdecl HatMachine_FindNearest(WORLDINFO *world, NUVEC *position, GAMEOBJECT *object,
                                                         f32 *distance) {
    f32 nearest_distance = 1.0e9f;
    HATMACHINE_PC *nearest = NULL;
    if (world == NULL || ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys == NULL)
        return NULL;
    for (i32 index = 0; index < ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->count; ++index) {
        HATMACHINE_PC *machine = &((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->machines[index];
        f32 candidate_distance;
        if (object != NULL) {
            if ((machine->flags & 8) == 0 || (machine->flags & 4) == 0 || (machine->flags & 3) != 0 ||
                machine->player_position.y == 2000000.0f)
                continue;
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
    if (distance != NULL)
        *distance = nearest_distance;
    return nearest;
}
extern "C" i32 __cdecl HatMachine_BeingUsed(HATMACHINE_PC *machine) {
    return machine->flags & 1;
}
struct HAT_USE_REQUEST_PC {
    u8 unknown_000[0x79c];
    i32 request;
    void *net_wait_info;
};
STATIC_ASSERT(offsetof(HAT_USE_REQUEST_PC, request) == 0x79c);
STATIC_ASSERT(offsetof(HAT_USE_REQUEST_PC, net_wait_info) == 0x7a0);
STATIC_ASSERT(offsetof(HATMACHINE_PC, configured_hat) == 0x5e);
STATIC_ASSERT(offsetof(HATMACHINE_PC, displayed_hat) == 0x5f);
STATIC_ASSERT(offsetof(HATMACHINE_PC, animation_state) == 0x60);
STATIC_ASSERT(offsetof(HATMACHINE_PC, state_duration) == 0x88);
STATIC_ASSERT(offsetof(HATMACHINE_PC, state_elapsed) == 0x8c);
STATIC_ASSERT(offsetof(HATMACHINE_PC, scale) == 0x90);
STATIC_ASSERT(offsetof(HATMACHINE_PC, idle_bounce_timer) == 0x98);
extern "C" i16 id_PRINCESSLEIABOUSHH;
extern "C" void __cdecl HatMachine_MoveCode(WORLDINFO *world, GAMEOBJECT *object, i32 special_pressed) {
    HATMACHINE_PC *machine = (HATMACHINE_PC *)object->grapple;
    if (object->field_db0 > 0.0f)
        object->field_db0 -= FRAMETIME;
    if (object->character_context != 0x61 || machine == NULL) {
        if (object->character_model->model_data_b[0x5d] == NULL || object->field_281 == 0 || ObjLandReady(object) == 0)
            return;
        f32 distance;
        machine = HatMachine_FindNearest(world, &object->collision_position, object, &distance);
        if (machine == NULL)
            return;
        f32 range = (f64)object->bounds_radius + 0.25;
        range *= machine->scale;
        if (distance < range && (special_pressed != 0 || (((HAT_USE_REQUEST_PC *)object)->request == 2 &&
                                                          ((HAT_USE_REQUEST_PC *)object)->net_wait_info != NULL))) {
            object->grapple = machine;
            object->context_hover_target_y = 0.0f;
            object->delayed_turn_timer = 0.0f;
            object->movement_facing_angle = machine->yaw;
            object->flags_e20 &= ~0x1000;
            object->character_context = 0x61;
            object->field_7b3 = 0;
            FastWeaponIn(object, 0);
            object->flags_e24 |= 0x200;
            object->field_7aa = 0x5d;
            machine->animation_state = 1;
            machine->state_duration =
                StartupGameFlag1 != 0 ? 3.0f : AnimDuration(object->id, object->field_7aa, 0.0f, 0.0f, 1);
            f32 end_frame = AnimListFrame(object->character_model, object->field_7aa, 0);
            machine->flags |= 1;
            machine->state_elapsed = -end_frame;
            machine->state_duration += end_frame;
            machine->idle_bounce_timer = 0.0f;
            LoseHelmet(object, 0, 0);
            NewRumble(object->pad->bound_pad, 0.5f, 0);
            if (object->anim_blending != 0)
                ResetAnimPacket(&object->anim_packet, -1);
            AlertSurroundingCreatures(object, &object->collision_position);
            object->context_timer = AnimDuration(object->id, object->field_7aa, 0.0f, 0.0f, 1);
            ((HATMACHINE_PC *)object->grapple)->state_duration = object->context_timer;
            if (object->context_timer <= 0.0f)
                object->context_timer = 2.0f;
            object->field_db0 = 0.0f;
            return;
        }
        if ((object->flags_1fc & 0x80) != 0 && (object->character_data->model_flags & 0x20) != 0 &&
            object->field_db0 <= 0.0f) {
            PlaySfx("TC14_VLN", &object->collision_position);
            object->field_db0 = 0.5f;
        }
        return;
    }
    if (object->character_model->model_data_b[object->field_7aa] == NULL ||
        AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0) != NULL) {
        object->context_hover_target_y += FRAMETIME;
        if (object->context_hover_target_y > 1.0f)
            object->context_hover_target_y = 1.0f;
        object->context_timer -= FRAMETIME;
        if (object->field_7b3 == 1) {
            if (machine->animation_state != 4)
                return;
            if ((object->character_data->game_character->flags_090 & 0x10) == 0 &&
                (object->id != (u16)id_PRINCESSLEIABOUSHH || FreePlay == 0)) {
                PlaySfx("HatOn", &object->upper_position);
                object->helmet = ((HATMACHINE_PC *)object->grapple)->displayed_hat;
                if (object->helmet == 5)
                    MakeBaddiesForgetAboutParty(1);
            }
            machine->displayed_hat = 0;
            object->field_7b3 = 2;
            if (StartupGameFlag1 != 0)
                machine->animation_state = 6;
        } else if (object->field_7b3 == 0) {
            if (!(object->context_timer <= 0.0f))
                return;
            const bool disguise_blocked = (object->character_data->game_character->flags_090 & 0x10) == 0 &&
                                          (object->id != (u16)id_PRINCESSLEIABOUSHH || FreePlay == 0);
            if (object->character_model->model_data_b[0x6d] != NULL && !disguise_blocked) {
                if (machine->animation_state == 2) {
                    object->field_7aa = 0x6d;
                    object->field_7b3 = 1;
                    object->context_timer = AnimDuration(object->id, 0x6d, 0.0f, 0.0f, 1);
                    machine->state_duration = (f64)object->context_timer * (f64)0.35f;
                    return;
                }
                object->field_7aa = 1;
                return;
            }
            if (object->character_model->model_data_b[0x5e] != NULL) {
                if (machine->animation_state == 2) {
                    object->field_7aa = 0x5e;
                    object->field_7b3 = 1;
                    object->context_timer = AnimDuration(object->id, 0x5e, 0.0f, 0.0f, 1);
                    if (StartupGameFlag1 == 0)
                        machine->state_duration = (f64)object->context_timer * (f64)0.35f;
                    return;
                }
                object->field_7aa = 1;
                return;
            }
            if (machine->animation_state == 2) {
                object->field_7aa = 1;
                object->field_7b3 = 1;
                machine->state_duration = 0.2f;
            }
            return;
        } else if (object->field_7b3 != 2)
            return;
        if (!(object->context_timer <= 0.0f))
            return;
        object->character_context = -1;
        object->context_timer = 0.0f;
        machine = (HATMACHINE_PC *)object->grapple;
        if (machine == NULL)
            return;
        if (machine->configured_hat == 6)
            PlaySfx("Hunter_Granted", &object->collision_position);
        if (((HATMACHINE_PC *)object->grapple)->configured_hat == 5)
            PlaySfx("Trooper_Granted", &object->collision_position);
    } else {
        object->context_timer -= FRAMETIME;
        if (!(object->context_timer <= 0.0f))
            return;
        if (object->grapple == NULL)
            NewRumble(object->pad->bound_pad, 0.5f, 0);
        else {
            NewBuzz(object->pad->bound_pad, 0.1f, 0);
            ((HATMACHINE_PC *)object->grapple)->hat_delay = 0.6f;
            if (((HATMACHINE_PC *)object->grapple)->configured_hat == 6)
                PlaySfx("Hunter_Granted", &object->collision_position);
            if (((HATMACHINE_PC *)object->grapple)->configured_hat == 5)
                PlaySfx("Trooper_Granted", &object->collision_position);
        }
        object->character_context = -1;
    }
    object->grapple = NULL;
}

struct HATMACHINE_PROGRESS_PC {
    i32 preserved_state;
    u32 enabled_mask, visible_mask;
};
STATIC_ASSERT(sizeof(HATMACHINE_PROGRESS_PC) == 12);
STATIC_ASSERT(offsetof(HATMACHINE_PROGRESS_PC, enabled_mask) == 4);
STATIC_ASSERT(offsetof(HATMACHINE_PROGRESS_PC, visible_mask) == 8);
STATIC_ASSERT(offsetof(HATMACHINE_PC, name) == 0x40);
STATIC_ASSERT(offsetof(HATMACHINE_PC, model_letter) == 0x61);
STATIC_ASSERT(offsetof(HATMACHINE_PC, model_special_index) == 0x62);
STATIC_ASSERT(offsetof(HATMACHINE_PC, terrain_pitch) == 0x7c);
STATIC_ASSERT(offsetof(HATMACHINE_PC, terrain_roll) == 0x7e);
STATIC_ASSERT(offsetof(HATMACHINE_PC, hat_delay) == 0x84);
STATIC_ASSERT(offsetof(HATMACHINE_PC, animation_time) == 0x94);
extern "C" ADDGIZMOTYPE_PC Default_ADDGIZMOTYPE;
extern "C" i32 hatmachine_gizmotype_id;
extern "C" void *__cdecl GizmoBufferAlloc(VARIPTR *, VARIPTR *, i32);
// Genuine Draw callback is retained as an unresolved source dependency until its full body is transferred.
extern "C" {
    static void __cdecl HatMachine_Draw(void *, void *, f32);
}
extern "C" {
    static i32 HatMachine_GetMaxGizmos(void *world_ptr) {
        WORLDINFO *world = (WORLDINFO *)world_ptr;
        return world != NULL ? world->current_level->max_hatmachines : 0;
    }
    static char *HatMachine_GetGizmoName(GIZMO_PC *gizmo) {
        if (gizmo == NULL || gizmo->object == NULL)
            return NULL;
        return ((HATMACHINE_PC *)gizmo->object)->name;
    }
    static i32 HatMachine_GetOutput(GIZMO_PC *gizmo, i32, i32) {
        return (((HATMACHINE_PC *)gizmo->object)->flags >> 1) & 1;
    }
    static char *HatMachine_GetOutputName(GIZMO_PC *, i32 output_index) {
        return output_index == 0 ? (char *)"Finished" : NULL;
    }
    static i32 HatMachine_GetNumOutputs(GIZMO_PC *) {
        return 1;
    }
    static i32 HatMachine_Load(void *world_ptr, void *) {
        WORLDINFO *world = (WORLDINFO *)world_ptr;
        if (world == NULL)
            return 0;
        HATMACHINE_SYS_PC *system = ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys;
        if (system == NULL || system->count != 0)
            return 0;
        const i32 version = EdFileReadInt();
        system->count = EdFileReadInt();
        for (i32 index = 0; index < system->count; ++index) {
            EdFileRead(system->machines[index].name, EdFileReadInt());
            EdFileReadNuVec(&system->machines[index].position);
            system->machines[index].yaw = EdFileReadShort();
            system->machines[index].configured_hat = EdFileReadChar();
            if (version >= 3)
                system->machines[index].model_letter = EdFileReadChar();
            else
                system->machines[index].model_letter = 'r';
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
                system->machines[index].flags = (system->machines[index].flags & ~0x20) | ((hidden & 1) << 5);
            }
            system->machines[index].platform_id = -1;
        }
        return 1;
    }
    static void *HatMachines_ReserveBufferSpace(void *world_ptr) {
        WORLDINFO *world = (WORLDINFO *)world_ptr;
        ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys = NULL;
        if (world->current_level->max_levers == 0)
            return NULL;
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys = (HATMACHINE_SYS_PC *)world->giz_buffer.void_ptr;
        world->giz_buffer.addr += sizeof(HATMACHINE_SYS_PC);
        memset(((HATMACHINE_WORLD_PC *)world)->hat_machine_sys, 0, sizeof(HATMACHINE_SYS_PC));
        world->giz_buffer.addr = (world->giz_buffer.addr + 15) & ~15u;
        ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->machines = (HATMACHINE_PC *)world->giz_buffer.void_ptr;
        world->giz_buffer.addr += world->current_level->max_hatmachines * sizeof(HATMACHINE_PC);
        memset(((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->machines, 0,
               world->current_level->max_hatmachines * sizeof(HATMACHINE_PC));
        return ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys;
    }
    static void HatMachines_ClearProgress(void *, void *progress_ptr) {
        HATMACHINE_PROGRESS_PC *progress = (HATMACHINE_PROGRESS_PC *)progress_ptr;
        if (progress != NULL) {
            progress->enabled_mask = ~0u;
            progress->visible_mask = ~0u;
        }
    }
    static void *HatMachines_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
        return GizmoBufferAlloc(buffer, end, sizeof(HATMACHINE_PROGRESS_PC));
    }
    static void HatMachine_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
        if (gizmo == NULL || gizmo->object == NULL)
            return;
        HATMACHINE_PC *machine = (HATMACHINE_PC *)gizmo->object;
        const i32 was_visible = (machine->flags >> 2) & 1;
        machine->flags = (machine->flags & ~4) | ((visible != 0) << 2);
        if ((machine->flags & 4) != 0 && was_visible == 0) {
            machine->platform_id = NewPlatPickupInst(machine, 2);
            PlatInstRotate(machine->platform_id, 1);
        } else if ((machine->flags & 4) == 0 && was_visible != 0)
            DeletePlatinst(machine->platform_id);
    }
    static void HatMachine_AddGizmos(void *gizmo_sys, i32 type_id, void *world_ptr, void *) {
        WORLDINFO *world = (WORLDINFO *)world_ptr;
        if (world == NULL || ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys == NULL)
            return;
        for (i32 index = 0; index < ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->count; ++index) {
            HATMACHINE_PC *machine = &((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->machines[index];
            if (NuStrLen(machine->name) != 0)
                AddGizmo(gizmo_sys, type_id, NULL, machine);
        }
    }
    static void HatMachines_StoreProgress(void *world_ptr, void *, void *progress_ptr) {
        HATMACHINE_PROGRESS_PC *progress = (HATMACHINE_PROGRESS_PC *)progress_ptr;
        if (progress == NULL)
            return;
        progress->enabled_mask = ~0u;
        progress->visible_mask = ~0u;
        WORLDINFO *world = (WORLDINFO *)world_ptr;
        if (world == NULL || ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys == NULL ||
            ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->machines == NULL)
            return;
        for (i32 index = 0; index < ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->count && index < 32; ++index) {
            HATMACHINE_PC *machine = &((HATMACHINE_WORLD_PC *)world)->hat_machine_sys->machines[index];
            const u32 mask = 1u << index;
            if ((machine->flags & 4) == 0)
                progress->visible_mask &= ~mask;
            if ((machine->flags & 8) == 0)
                progress->enabled_mask &= ~mask;
        }
    }
    static void HatMachine_Reset(HATMACHINE_PC *machine) {
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
        machine->flags = (machine->flags & ~3) | 12;
        machine->animation_state = 0;
        machine->hat_delay = 0.0f;
        machine->state_duration = 0.0f;
        machine->animation_time = 0.0f;
        machine->model_special_index = machine->model_letter == 'r' ? 0x22 : 0x20 + (machine->model_letter == 'o');
        if (machine->configured_hat != 0)
            machine->displayed_hat = machine->configured_hat;
        else
            machine->displayed_hat = 1 - (i32)(NuFloatRand((NURAND *)&GAMERAND) * -4.0);
        NuMtxSetRotationY(&machine->matrix, machine->yaw);
        NuMtxTranslate(&machine->matrix, &machine->position);
    }
    static void HatMachines_Reset(void *world_ptr, void *, void *progress_ptr) {
        WORLDINFO *world = (WORLDINFO *)world_ptr;
        if (world == NULL)
            return;
        HATMACHINE_SYS_PC *system = ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys;
        if (system == NULL || system->machines == NULL || system->count <= 0)
            return;
        HATMACHINE_PROGRESS_PC *progress = (HATMACHINE_PROGRESS_PC *)progress_ptr;
        for (i32 index = 0; index < system->count; ++index) {
            HATMACHINE_PC *machine = &system->machines[index];
            HatMachine_Reset(machine);
            if (index < 32 && progress != NULL) {
                const u32 mask = 1u << index;
                machine->flags = (machine->flags & ~4) | (((progress->visible_mask & mask) != 0) << 2);
                machine->flags = (machine->flags & ~8) | (((progress->enabled_mask & mask) != 0) << 3);
            }
        }
    }
    static void HatMachine_Activate(GIZMO_PC *gizmo, i32 enabled) {
        if (gizmo == NULL || gizmo->object == NULL)
            return;
        HATMACHINE_PC *machine = (HATMACHINE_PC *)gizmo->object;
        machine->flags = (machine->flags & ~8) | ((enabled != 0) << 3);
        if (enabled != 0)
            HatMachine_Reset(machine);
    }
    static void HatMachine_Update(void *world_ptr, void *, f32 elapsed) {
        WORLDINFO *world = (WORLDINFO *)world_ptr;
        HATMACHINE_SYS_PC *system = ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys;
        if (system == NULL || system->count <= 0)
            return;
        for (i32 index = 0; index < system->count; ++index) {
            HATMACHINE_PC *machine = &system->machines[index];
            if (machine->hat_delay > 0.0f) {
                machine->hat_delay -= FRAMETIME;
                if (machine->hat_delay <= 0.0f && machine->displayed_hat == 0) {
                    machine->displayed_hat = machine->configured_hat != 0
                                                 ? machine->configured_hat
                                                 : 1 - (i32)(NuFloatRand((NURAND *)&GAMERAND) * -4.0);
                    machine->hat_delay = 1.0f;
                }
            }
            if ((machine->flags & 1) != 0 && machine->animation_state > 0) {
                machine->state_elapsed += elapsed;
                if (machine->state_elapsed > machine->state_duration) {
                    ++machine->animation_state;
                    if (machine->animation_state == 2)
                        PlaySfx("SwLever", &machine->position);
                    machine->state_elapsed = 0.0f;
                    if (machine->animation_state > 6) {
                        machine->flags &= ~1;
                        machine->animation_state = 0;
                        machine->state_duration = 0.0f;
                        machine->hat_delay = 0.6f;
                        machine->animation_time = 0.0f;
                    }
                }
            }
        }
    }
}
extern "C" ADDGIZMOTYPE_PC *HatMachine_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC HatMachineAddTypePc = {0};
    HatMachineAddTypePc = Default_ADDGIZMOTYPE;
    HatMachineAddTypePc.name = (char *)"HatMachine";
    HatMachineAddTypePc.prefix = (char *)"";
    HatMachineAddTypePc.fns.options = 12;
    HatMachineAddTypePc.fns.get_max_gizmos_fn = HatMachine_GetMaxGizmos;
    HatMachineAddTypePc.fns.add_gizmos_fn = (void *)HatMachine_AddGizmos;
    HatMachineAddTypePc.fns.early_update_fn = NULL;
    HatMachineAddTypePc.fns.late_update_fn = HatMachine_Update;
    HatMachineAddTypePc.fns.draw_fn = HatMachine_Draw;
    HatMachineAddTypePc.fns.panel_draw_fn = NULL;
    HatMachineAddTypePc.fns.get_gizmo_name_fn = (char *(__cdecl *)(struct gizmo_pc_s *))HatMachine_GetGizmoName;
    HatMachineAddTypePc.fns.get_output_fn = (i32(__cdecl *)(struct gizmo_pc_s *, i32, i32))HatMachine_GetOutput;
    HatMachineAddTypePc.fns.get_output_name_fn = (char *(__cdecl *)(struct gizmo_pc_s *, i32))HatMachine_GetOutputName;
    HatMachineAddTypePc.fns.get_num_outputs_fn = (void *)HatMachine_GetNumOutputs;
    HatMachineAddTypePc.fns.activate_fn = (void(__cdecl *)(struct gizmo_pc_s *, i32))HatMachine_Activate;
    HatMachineAddTypePc.fns.activate_rev_fn = NULL;
    HatMachineAddTypePc.fns.set_visibility_fn = (void(__cdecl *)(struct gizmo_pc_s *, i32))HatMachine_SetVisibility;
    HatMachineAddTypePc.fns.get_visibility_fn = NULL;
    HatMachineAddTypePc.fns.get_pos_fn = NULL;
    HatMachineAddTypePc.fns.using_special_fn = NULL;
    HatMachineAddTypePc.fns.bolt_hit_plat_fn = NULL;
    HatMachineAddTypePc.fns.get_best_bolt_target_fn = NULL;
    HatMachineAddTypePc.fns.bolt_hit_fn = NULL;
    HatMachineAddTypePc.fns.allocate_progress_data_fn = HatMachines_AllocateProgressData;
    HatMachineAddTypePc.fns.clear_progress_fn = HatMachines_ClearProgress;
    HatMachineAddTypePc.fns.store_progress_fn = HatMachines_StoreProgress;
    HatMachineAddTypePc.fns.reset_fn = HatMachines_Reset;
    HatMachineAddTypePc.fns.reserve_buffer_space_fn = HatMachines_ReserveBufferSpace;
    HatMachineAddTypePc.fns.load_fn = HatMachine_Load;
    HatMachineAddTypePc.fns.post_load_fn = NULL;
    HatMachineAddTypePc.fns.add_level_sfx_fn = NULL;
    hatmachine_gizmotype_id = type_id;
    return &HatMachineAddTypePc;
}

static const NUVEC HatMachine_HatOffset = {0.0f, 0.3f, 0.0f};
static const i32 HatMachineDrawSpecialIndicesPc[4] = {288, 285, 289, 284};
static inline f32 NuFdiv(f32 a, f32 b) {
    if (a == 0.0f || b == 0.0f)
        return 0.0f;
    return a / b;
}
STATIC_ASSERT(offsetof(HATMACHINE_PC, matrix) == 0);
STATIC_ASSERT(offsetof(HATMACHINE_PC, model_special_index) == 0x62);
STATIC_ASSERT(offsetof(HATMACHINE_PC, terrain_pitch) == 0x7c);
STATIC_ASSERT(offsetof(HATMACHINE_PC, terrain_roll) == 0x7e);
STATIC_ASSERT(offsetof(HATMACHINE_PC, hat_delay) == 0x84);
STATIC_ASSERT(offsetof(HATMACHINE_PC, animation_time) == 0x94);
STATIC_ASSERT(sizeof(((HATMACHINE_PC *)0)->model_special_index) == 1);
extern "C" void __cdecl HatMachine_Draw(void *world_ptr, void *, f32) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    HATMACHINE_SYS_PC *system;
    if (world == NULL || (system = ((HATMACHINE_WORLD_PC *)world)->hat_machine_sys) == NULL || system->count == 0)
        return;
    u16 spin_angle = (u16)((f64)NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0 * 65536.0);
    f32 target_pulse =
        (f64)NuTrigTable[((i32)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0) >> 1) & 0x7fff] *
            (f64)0.2f +
        (f64)0.8f;
    f32 ready_alpha =
        (f64)NuTrigTable[((i32)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0) >> 1) & 0x7fff] *
            (f64)0.15f +
        (f64)0.85f;
    EnableShadowMapRendering(0);
    NUSPECIALHANDLE *specials[4];
    NUINSTANIM_TIME_VIEW *animations[4];
    f32 end_frames[4];
    if (StartupGameFlag1 != 0) {
        for (i32 effect = 0; effect < 4; ++effect) {
            specials[effect] = &world->level_objects[HatMachineDrawSpecialIndicesPc[effect]].special;
            animations[effect] = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(specials[effect]);
            if (specials[effect] != NULL && animations[effect] != NULL)
                end_frames[effect] = NuAnimEndFrameOld(
                    ((NUGSCN *)specials[effect]->scene)->instance_animation_data[animations[effect]->animation_index]);
        }
    }
    for (i32 index = 0; index < system->count; ++index) {
        HATMACHINE_PC *machine = &system->machines[index];
        if ((machine->flags & 4) == 0 && editor_active == 0)
            continue;
        f32 frame = machine->state_elapsed;
        if (frame < 0.0f)
            frame = 0.0f;
        if (world->level_objects[283].active != 0)
            NuSpecialDrawAt(&world->level_objects[283].special, &machine->matrix);
        if ((machine->flags & 8) != 0) {
            if (world->level_objects[286].active != 0)
                NuSpecialDrawAt(&world->level_objects[286].special, &machine->matrix);
        } else if (world->level_objects[287].active != 0)
            NuSpecialDrawAt(&world->level_objects[287].special, &machine->matrix);
        NUMTX matrix;
        if ((machine->flags & 0x20) == 0 && machine->target_offset.y != 2000000.0f &&
            world->level_objects[85].active != 0) {
            NUVEC target;
            Hat_GetAbsTargetPos(machine, &target);
            NuMtxSetRotationY(&matrix, spin_angle);
            if (machine->terrain_roll != 0)
                NuMtxRotateZ(&matrix, machine->terrain_roll);
            if (machine->terrain_pitch != 0)
                NuMtxRotateX(&matrix, machine->terrain_pitch);
            NuMtxScaleU(&matrix, 0.8f);
            NuMtxTranslate(&matrix, &target);
            NuMtxPreScaleU(&matrix, machine->scale);
            f32 alpha = (machine->flags & 1) != 0 ? 0.0f : target_pulse;
            NuSpecialDrawAtAlpha(&world->level_objects[85].special, &matrix, alpha);
        }
        if (StartupGameFlag1 != 0) {
            if (specials[0] != NULL && animations[0] != NULL) {
                if (machine->animation_state > 0) {
                    machine->animation_time += FRAMETIME;
                    if (machine->animation_state < 4 && machine->animation_time >= 3.0f) {
                        machine->animation_state = 4;
                        machine->state_elapsed = 0.0f;
                        machine->state_duration = 2.0f;
                    }
                    frame = (f64)animations[0]->tfactor * machine->animation_time * 60.0;
                    if (end_frames[0] <= frame)
                        frame = end_frames[0];
                } else {
                    machine->animation_time = 0.0f;
                    frame = 0.0f;
                    if ((machine->flags & 0xb) == 8) {
                        if (machine->idle_bounce_timer > 0.0f) {
                            machine->idle_bounce_timer -= FRAMETIME;
                            if (machine->idle_bounce_timer < 0.0f)
                                machine->idle_bounce_timer = 0.0f;
                            else {
                                f32 sine = NuTrigTable
                                    [((i32)((f64)machine->idle_bounce_timer / (f64)0.15f * 32768.0 + 16384.0) >> 1) &
                                     0x7fff];
                                frame = (1.0 - fabs((f64)sine)) * 1.5 + 16.0;
                            }
                        } else if (qrand() < 0x800)
                            machine->idle_bounce_timer = 0.15f;
                    }
                }
                EvalAnim(specials[0], frame, &matrix, 0);
                NuMtxMulVU0(&matrix, &matrix, &machine->matrix);
                NuSpecialDrawAt(specials[0], &matrix);
                if (world->level_objects[(i8)machine->model_special_index].active != 0)
                    NuSpecialDrawAt(&world->level_objects[(i8)machine->model_special_index].special, &matrix);
                if ((machine->flags & 0xb) == 8 && world->level_objects[41].active != 0)
                    NuSpecialDrawAtAlpha(&world->level_objects[41].special, &matrix, ready_alpha);
            }
            NUVEC draw_position = machine->position;
            for (i32 effect = 1; effect < 4; ++effect) {
                if (specials[effect] != NULL && animations[effect] != NULL) {
                    frame = (f64)machine->animation_time * animations[0]->tfactor * 60.0;
                    if (end_frames[effect] <= frame)
                        frame = end_frames[effect];
                    EvalAnim(specials[effect], frame, &matrix, 0);
                    NuMtxMulVU0(&matrix, &matrix, &machine->matrix);
                    NuSpecialDrawAt(specials[effect], &matrix);
                    if (effect == 1) {
                        draw_position.x = matrix.m30;
                        draw_position.z = matrix.m32;
                    }
                }
            }
            if ((machine->flags & 8) != 0 && machine->displayed_hat != 0 &&
                world->level_objects[machine->displayed_hat + 249].active != 0 && !(machine->animation_time >= 2.35f)) {
                NUVEC offset = HatMachine_HatOffset;
                f32 hat_scale =
                    1.0 - (1.0 - ((f64)NuTrigTable[((i32)((f64)machine->hat_delay * 32768.0 + 16384.0) >> 1) & 0x7fff] +
                                  1.0) *
                                     0.5);
                offset.y = (1.0 - hat_scale) * (f64)0.1f + HatMachine_HatOffset.y;
                if (machine->animation_state < 3) {
                    f32 ratio = NuFdiv(frame, 50.0f);
                    if (ratio > 1.0f)
                        ratio = 1.0f;
                    f32 bob = (1.0 - ratio) * (f64)0.01f;
                    offset.y += NuTrigTable[((i32)((f64)GameTimer.time_elapsed * 32768.0) >> 1) & 0x7fff] * bob;
                }
                NuVecRotateY(&offset, &offset, machine->yaw);
                NuMtxSetRotationY(&matrix, machine->yaw + 0x8000);
                NuMtxTranslate(&matrix, &draw_position);
                NuMtxTranslate(&matrix, &offset);
                NuMtxPreScaleU(&matrix, hat_scale);
                NuSpecialDrawAt(&world->level_objects[machine->displayed_hat + 249].special, &matrix);
            }
        } else {
            if (world->level_objects[30].active != 0) {
                NuMtxSetRotationY(&matrix, machine->yaw);
                switch (machine->animation_state) {
                    case 1:
                        NuMtxPreRotateX(&matrix, (u16)((f64)frame / machine->state_duration * -32768.0));
                        break;
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                        NuMtxPreRotateX(&matrix, 0x8000);
                        break;
                    case 6:
                        NuMtxPreRotateX(&matrix, (u16)((1.0 - (f64)frame / machine->state_duration) * -32768.0));
                        break;
                }
                NuMtxTranslate(&matrix, &machine->position);
                NuSpecialDrawAt(&world->level_objects[30].special, &matrix);
                if (world->level_objects[(i8)machine->model_special_index].active != 0)
                    NuSpecialDrawAt(&world->level_objects[(i8)machine->model_special_index].special, &matrix);
                if ((machine->flags & 0xb) == 8 && world->level_objects[41].active != 0)
                    NuSpecialDrawAtAlpha(&world->level_objects[41].special, &matrix, ready_alpha);
            }
            for (i32 arm = 0; arm < 2; ++arm) {
                i32 special_index = arm == 0 ? 285 : 284;
                if (world->level_objects[special_index].active == 0)
                    continue;
                NUVEC offset = v000;
                switch (machine->animation_state) {
                    case 2:
                        offset.z =
                            (1.0 +
                             NuTrigTable[((i32)((f64)frame / machine->state_duration * 16384.0 + 32768.0 + 16384.0) >>
                                          1) &
                                         0x7fff]) *
                            (f64)-0.1441f;
                        break;
                    case 3:
                        offset.z = -0.1441f;
                        offset.y =
                            (1.0 +
                             NuTrigTable[((i32)((f64)frame / machine->state_duration * 16384.0 + 32768.0 + 16384.0) >>
                                          1) &
                                         0x7fff]) *
                            (f64)-0.35f;
                        break;
                    case 4:
                        offset.z = -0.1441f;
                        offset.y =
                            (1.0 -
                             (NuTrigTable[((i32)((f64)frame / machine->state_duration * 16384.0 + 32768.0 + 16384.0) >>
                                           1) &
                                          0x7fff] +
                              1.0)) *
                            (f64)-0.35f;
                        break;
                    case 5:
                        offset.z =
                            (1.0 -
                             NuTrigTable[((i32)((f64)frame / machine->state_duration * 16384.0 + 49152.0 + 16384.0) >>
                                          1) &
                                         0x7fff]) *
                            (f64)-0.1441f;
                        break;
                }
                NuVecRotateY(&offset, &offset, machine->yaw);
                NuMtxSetRotationY(&matrix, machine->yaw);
                NuMtxTranslate(&matrix, &machine->position);
                NuMtxTranslate(&matrix, &offset);
                NuSpecialDrawAt(&world->level_objects[special_index].special, &matrix);
            }
            if ((machine->flags & 8) != 0 && machine->displayed_hat != 0 &&
                world->level_objects[machine->displayed_hat + 249].active != 0) {
                NUVEC offset = HatMachine_HatOffset;
                switch (machine->animation_state) {
                    case 2:
                        offset.z =
                            (f64)HatMachine_HatOffset.z -
                            (1.0 +
                             NuTrigTable[((i32)((f64)frame / machine->state_duration * 16384.0 + 32768.0 + 16384.0) >>
                                          1) &
                                         0x7fff]) *
                                (f64)0.1441f;
                        break;
                    case 3:
                        if (!((f64)frame / machine->state_duration < (f64)0.85f))
                            continue;
                        offset.z = (f64)HatMachine_HatOffset.z - (f64)0.1441f;
                        offset.y -=
                            (1.0 +
                             NuTrigTable[((i32)((f64)frame / machine->state_duration * 16384.0 + 32768.0 + 16384.0) >>
                                          1) &
                                         0x7fff]) *
                            (f64)0.35f;
                        offset.y +=
                            (1.0 +
                             NuTrigTable[((i32)((f64)frame / machine->state_duration * 16384.0 + 32768.0 + 16384.0) >>
                                          1) &
                                         0x7fff]) *
                            0.125;
                        break;
                    case 4:
                    case 5:
                        continue;
                }
                f32 hat_scale =
                    1.0 - (1.0 - ((f64)NuTrigTable[((i32)((f64)machine->hat_delay * 32768.0 + 16384.0) >> 1) & 0x7fff] +
                                  1.0) *
                                     0.5);
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
