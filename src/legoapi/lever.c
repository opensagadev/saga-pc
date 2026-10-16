#include "saga.h"
typedef struct lever_trigger_flags_pc_s {
    u8 unknown000[0x94];
    u8 flags;
} LEVER_TRIGGER_FLAGS_PC;
STATIC_ASSERT(offsetof(LEVER_TRIGGER_FLAGS_PC, flags) == 0x94);
i32 __cdecl Lever_BeingPulled(void *lever) {
    return (((LEVER_TRIGGER_FLAGS_PC *)lever)->flags >> 1) & 1;
}

typedef struct LEVER_WORLD_PC {
    u8 unknown_000[0x5068];
    GIZMO_LEVER_PC *levers;
    i32 lever_count;
} LEVER_WORLD_PC;
typedef struct LEVER_NET_WAIT_PC {
    u8 unknown_000[0x7a0];
    void *net_wait_info;
} LEVER_NET_WAIT_PC;
STATIC_ASSERT(sizeof(GIZMO_LEVER_PC) == 0xa0);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, floor_position) == 0x40);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, target_offset) == 0x4c);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, position) == 0x68);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, pull_progress) == 0x78);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, auto_reset_timer) == 0x7c);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, target_indicator_scale) == 0x84);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, yaw) == 0x88);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, platform_id) == 0x8c);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, flags) == 0x94);
STATIC_ASSERT(offsetof(LEVER_WORLD_PC, levers) == 0x5068);
STATIC_ASSERT(offsetof(LEVER_WORLD_PC, lever_count) == 0x506c);
STATIC_ASSERT(offsetof(LEVER_NET_WAIT_PC, net_wait_info) == 0x7a0);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_hover_target_y) == 0x778);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_timer) == 0x77c);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_duration) == 0x784);
STATIC_ASSERT(offsetof(GAMEOBJECT, context_variant_flags) == 0x7bc);
STATIC_ASSERT(offsetof(GAMEOBJECT, lower_position) == 0x19c);
void __cdecl ReleaseLever(GAMEOBJECT *object) {
    if (object->character_context == 0x4a) {
        GIZMO_LEVER_PC *lever = (GIZMO_LEVER_PC *)object->grapple;
        if (lever != NULL && (lever->flags & 2) == 0)
            lever->flags &= ~1;
    }
}
void __cdecl Lever_GetAbsTargetPos(GIZMO_LEVER_PC *lever, NUVEC *position) {
    if (lever != NULL && position != NULL) {
        NUVEC target = lever->target_offset;
        NuVecRotateY(&target, &target, lever->yaw);
        target.x += lever->position.x;
        target.z += lever->position.z;
        *position = target;
    }
}
void __cdecl Levers_InitTerrain(WORLDINFO *world) {
    if (((LEVER_WORLD_PC *)world)->levers != NULL) {
        i32 i;
        for (i = 0; i < ((LEVER_WORLD_PC *)world)->lever_count; ++i) {
            i16 platform = NewPlatPickupInst(&((LEVER_WORLD_PC *)world)->levers[i], 3);
            ((LEVER_WORLD_PC *)world)->levers[i].platform_id = platform;
            PlatInstRotate(((LEVER_WORLD_PC *)world)->levers[i].platform_id, 1);
        }
    }
}
GIZMO_LEVER_PC *__cdecl Lever_FindNearest(WORLDINFO *world, NUVEC *position, GAMEOBJECT *object, f32 *distance) {
    GIZMO_LEVER_PC *nearest = NULL;
    f32 nearest_distance = 1.0e9f;
    GIZMO_LEVER_PC *lever = ((LEVER_WORLD_PC *)world)->levers;
    i32 index;
    for (index = 0; index < ((LEVER_WORLD_PC *)world)->lever_count; ++index, ++lever) {
        f32 candidate_distance;
        if (object != NULL) {
            NUVEC target;
            if ((lever->flags & 0x80) == 0 || (lever->flags & 0x10) == 0 || (lever->flags & 3) != 0 ||
                lever->pull_progress != 0.0f || lever->floor_position.y == 2000000.0f)
                continue;
            Lever_GetAbsTargetPos(lever, &target);
            candidate_distance = NuVecDistSqr(position, &target, NULL);
        } else
            candidate_distance = NuVecDistSqr(position, &lever->position, NULL);
        if (candidate_distance < nearest_distance) {
            nearest = lever;
            nearest_distance = candidate_distance;
        }
    }
    if (distance != NULL)
        *distance = nearest_distance;
    return nearest;
}
i32 __cdecl Lever_FullyPulledDown(GIZMO_LEVER_PC *lever) {
    return (lever->flags & 0x10) != 0 && (lever->flags & 2) != 0 && lever->pull_progress >= 1.0f;
}
void __cdecl Lever_MoveCode(WORLDINFO *world, GAMEOBJECT *object) {
    f32 distance = 1.0e9f;
    GIZMO_LEVER_PC *lever;
    if (object->character_context == 0x4a) {
        object->context_hover_target_y += FRAMETIME;
        if (object->context_hover_target_y > 1.0f)
            object->context_hover_target_y = 1.0f;
        object->context_timer += FRAMETIME;
        if (object->context_timer >= object->context_duration) {
            object->character_context = -1;
            if ((object->context_variant_flags & 0x40) != 0)
                return;
        } else {
            if ((object->context_variant_flags & 0x40) != 0)
                return;
            if (object->character_model->model_data_b[object->field_7aa] != NULL) {
                f32 frame;
                f32 *playing = AnimPlaying(&object->anim_packet, object->field_7aa, 1, 0);
                if (playing == NULL)
                    return;
                frame = *playing;
                if (!(frame >= AnimListFrame(object->character_model, object->field_7aa, 0)))
                    return;
            } else if (!(object->context_timer >= 0.25f))
                return;
        }
        if ((object->character_data->model_flags & 4) != 0)
            ((GIZMO_LEVER_PC *)object->grapple)->flags |= 8;
        else if ((object->character_data->model_flags & 0x200) == 0)
            ((GIZMO_LEVER_PC *)object->grapple)->flags |= 4;
        ((GIZMO_LEVER_PC *)object->grapple)->flags |= 0x800;
        object->context_variant_flags |= 0x40;
        return;
    }
    if (object->character_model->model_data_b[0x5d] == NULL || object->field_281 == 0)
        return;
    if (ObjLandReady(object) == 0 && objInNetWaitContext(object, 0x4a) == 0)
        return;
    lever = Lever_FindNearest(world, &object->lower_position, object, &distance);
    if (objInNetWaitContext(object, 0x4a) != 0) {
        object->context_timer -= FRAMETIME;
        if (object->context_timer <= 0.0f) {
            object->character_context = -1;
            ((LEVER_NET_WAIT_PC *)object)->net_wait_info = NULL;
        }
    }
    if (lever != NULL) {
        f32 range = ((f64)object->bounds_radius + 0.25) * lever->target_indicator_scale;
        f32 range_squared = range * range;
        if (!(distance < range_squared))
            return;
        if ((object->pad->buttons_pressed & GAMEPAD_SPECIAL) == 0 && objInNetWaitContext(object, 0x4a) == 0)
            return;
        object->context_timer = object->context_hover_target_y = 0.0f;
        object->character_context = 0x4a;
        object->grapple = lever;
        object->field_7aa = 0x5d;
        object->context_duration = AnimDuration(object->id, 0x5d, 0.0f, 0.0f, 1);
        if (object->context_duration <= 0.0f)
            object->context_duration = 1.0f;
        object->context_variant_flags &= ~0x40;
        object->movement_facing_angle = ((GIZMO_LEVER_PC *)object->grapple)->yaw;
        ((GIZMO_LEVER_PC *)object->grapple)->flags |= 1;
        ((GIZMO_LEVER_PC *)object->grapple)->pull_progress = 0.0f;
        ((GIZMO_LEVER_PC *)object->grapple)->auto_reset_timer = 0.0f;
    }
}

typedef struct LEVER_PROGRESS_PC {
    u32 pulled_down, enabled, visible;
} LEVER_PROGRESS_PC;
STATIC_ASSERT(sizeof(LEVER_PROGRESS_PC) == 12);
STATIC_ASSERT(offsetof(LEVER_PROGRESS_PC, enabled) == 4);
STATIC_ASSERT(offsetof(LEVER_PROGRESS_PC, visible) == 8);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, name) == 0x58);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, idle_animation_timer) == 0x74);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, auto_reset_delay) == 0x80);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, model_special_index) == 0x8a);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, animation_frame) == 0x8e);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, terrain_pitch) == 0x90);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, terrain_roll) == 0x92);
STATIC_ASSERT(offsetof(GIZMO_LEVER_PC, model_variant) == 0x96);
extern i32 lever_gizmotype_id;
extern char *LeverTypeNamePtrPc;
extern char *LeverOutputDownNamePtrPc;
void *__cdecl GizmoBufferAlloc(VARIPTR *, VARIPTR *, i32);
static void __cdecl Levers_Draw(void *, void *, f32);
static void Lever_SelectModel(GIZMO_LEVER_PC *lever) {
    if (lever->model_variant == 'r')
        lever->model_special_index = 0x22;
    else if (lever->model_variant == 'o')
        lever->model_special_index = 0x21;
    else if (lever->model_variant == 'y')
        lever->model_special_index = 0x20;
    else if (lever->model_variant == 'b')
        lever->model_special_index = 0x23;
    else if (lever->model_variant == 'g')
        lever->model_special_index = 0x24;
    else if (lever->model_variant == 'p')
        lever->model_special_index = 0x25;
    else if (lever->model_variant == 'w')
        lever->model_special_index = 0x26;
    else if (lever->model_variant == 'u')
        lever->model_special_index = 0x27;
    else if (lever->model_variant == 'l')
        lever->model_special_index = 0x28;
}
static void Lever_Reset(GIZMO_LEVER_PC *lever) {
    NUVEC target;
    f32 height;
    Lever_SelectModel(lever);
    lever->floor_position.y = 0.0f;
    lever->floor_position.x = 0.0f;
    lever->floor_position.z = 0.1441f;
    NuVecRotateY(&lever->floor_position, &lever->floor_position, lever->yaw + 0x8000);
    NuVecAdd(&lever->floor_position, &lever->floor_position, &lever->position);
    Lever_GetAbsTargetPos(lever, &target);
    target.y = lever->position.y;
    lever->floor_position.y = GameShadow(NULL, &lever->floor_position, 0.2f, -1);
    height = GameShadow(NULL, &target, 0.2f, -1);
    lever->target_offset.y = height;
    if (height != 2000000.0f) {
        lever->target_offset.y = (f64)height + (f64)0.005f;
        FindAnglesZX(&ShadNorm, &lever->terrain_pitch, &lever->terrain_roll);
    }
    lever->flags |= 0x90;
    lever->animation_frame = 0;
    if ((lever->flags & 2) == 0 || (lever->flags & 0x20) == 0) {
        lever->pull_progress = 0.0f;
        lever->auto_reset_timer = 0.0f;
        lever->flags &= 0xefb0;
    }
    NuMtxSetRotationY(&lever->matrix, lever->yaw);
    NuMtxTranslate(&lever->matrix, &lever->position);
}
static i32 Levers_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    return world == NULL ? 0 : world->current_level->max_levers;
}
static char *Lever_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo == NULL ? NULL : ((GIZMO_LEVER_PC *)gizmo->object)->name;
}
static i32 Lever_GetOutput(GIZMO_PC *gizmo, i32 output, i32 mode) {
    GIZMO_LEVER_PC *lever;
    (void)mode;
    lever = (GIZMO_LEVER_PC *)gizmo->object;
    if ((lever->flags & 0x80) == 0)
        return 0;
    if (output == 0)
        return Lever_FullyPulledDown(lever);
    if (output == 1)
        return (lever->flags & 4) != 0 && Lever_FullyPulledDown(lever);
    if (output == 2)
        return (lever->flags & 8) != 0 && Lever_FullyPulledDown(lever);
    return 0;
}
static char *Lever_GetOutputName(GIZMO_PC *gizmo, i32 output) {
    (void)gizmo;
    if (output == 1)
        return "Down(Goodie)";
    if (output == 2)
        return "Down(Baddie)";
    return output == 0 ? LeverOutputDownNamePtrPc : NULL;
}
static i32 Lever_GetNumOutputs(GIZMO_PC *gizmo) {
    (void)gizmo;
    return 3;
}
static void Lever_Activate(GIZMO_PC *gizmo, i32 enabled) {
    GIZMO_LEVER_PC *lever;
    if (gizmo == NULL || gizmo->object == NULL)
        return;
    lever = (GIZMO_LEVER_PC *)gizmo->object;
    lever->flags = (lever->flags & ~0x80) | ((enabled != 0) << 7);
    if (enabled != 0)
        Lever_Reset(lever);
}
static i32 Lever_ActivateRev(GIZMO_PC *gizmo, i32 value, i32 query) {
    GIZMO_LEVER_PC *lever;
    if (gizmo == NULL || gizmo->object == NULL)
        return 1;
    lever = (GIZMO_LEVER_PC *)gizmo->object;
    if ((query & 1) != 0)
        return (lever->flags & 0x80) == 0 ? value == 0 : value;
    lever->flags = (lever->flags & ~0x80) | ((value == 0) << 7);
    return 1;
}
static void Lever_SetVisibility(GIZMO_PC *gizmo, i32 visible) {
    GIZMO_LEVER_PC *lever;
    i32 was_visible;
    if (gizmo == NULL || gizmo->object == NULL)
        return;
    lever = (GIZMO_LEVER_PC *)gizmo->object;
    was_visible = (lever->flags & 0x10) != 0;
    lever->flags = (lever->flags & ~0x10) | ((visible != 0) << 4);
    if ((lever->flags & 0x10) != 0 && was_visible == 0) {
        lever->platform_id = NewPlatPickupInst(lever, 3);
        PlatInstRotate(lever->platform_id, 1);
    } else if ((lever->flags & 0x10) == 0 && was_visible != 0)
        DeletePlatinst(lever->platform_id);
}
static void *Levers_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, sizeof(LEVER_PROGRESS_PC));
}
static void Levers_ClearProgress(void *world, void *progress_ptr) {
    LEVER_PROGRESS_PC *progress = (LEVER_PROGRESS_PC *)progress_ptr;
    (void)world;
    if (progress == NULL)
        return;
    progress->pulled_down = 0;
    progress->enabled = ~0u;
    progress->visible = ~0u;
}
static void Levers_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    LEVER_PROGRESS_PC *progress = (LEVER_PROGRESS_PC *)progress_ptr;
    i32 index;
    (void)data;
    if (progress == NULL)
        return;
    Levers_ClearProgress(NULL, progress);
    if (world == NULL || ((LEVER_WORLD_PC *)world)->levers == NULL)
        return;
    for (index = 0; index < ((LEVER_WORLD_PC *)world)->lever_count && index < 32; ++index) {
        GIZMO_LEVER_PC *lever = &((LEVER_WORLD_PC *)world)->levers[index];
        u32 bit = 1u << index;
        if ((lever->flags & 2) != 0 && lever->pull_progress >= 1.0f)
            progress->pulled_down |= bit;
        if ((lever->flags & 0x80) == 0)
            progress->enabled &= ~bit;
        if ((lever->flags & 0x10) == 0)
            progress->visible &= ~bit;
    }
}
static void Levers_Reset(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    LEVER_PROGRESS_PC *progress = (LEVER_PROGRESS_PC *)progress_ptr;
    i32 index;
    (void)data;
    if (world == NULL || ((LEVER_WORLD_PC *)world)->levers == NULL)
        return;
    for (index = 0; index < ((LEVER_WORLD_PC *)world)->lever_count; ++index) {
        GIZMO_LEVER_PC *lever = &((LEVER_WORLD_PC *)world)->levers[index];
        Lever_Reset(lever);
        if (progress != NULL && index < 32) {
            u32 bit = 1u << index;
            if ((progress->pulled_down & bit) != 0) {
                lever->pull_progress = 1.0f;
                lever->flags |= 2;
                lever->animation_frame = 0x8000;
            }
            lever->flags = (lever->flags & ~0x80) | (((progress->enabled & bit) != 0) << 7);
            lever->flags = (lever->flags & ~0x10) | (((progress->visible & bit) != 0) << 4);
        }
    }
}
static void *Levers_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    ((LEVER_WORLD_PC *)world)->levers = NULL;
    ((LEVER_WORLD_PC *)world)->lever_count = 0;
    if (world->current_level->max_levers != 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 15) & ~15u;
        ((LEVER_WORLD_PC *)world)->levers = (GIZMO_LEVER_PC *)world->giz_buffer.void_ptr;
        world->giz_buffer.addr += world->current_level->max_levers * sizeof(GIZMO_LEVER_PC);
    }
    return ((LEVER_WORLD_PC *)world)->levers;
}
static i32 Levers_Load(void *world_ptr, void *data) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    i32 version, index;
    (void)data;
    if (((LEVER_WORLD_PC *)world)->lever_count != 0)
        return 0;
    version = EdFileReadInt();
    ((LEVER_WORLD_PC *)world)->lever_count = EdFileReadInt();
    for (index = 0; index < ((LEVER_WORLD_PC *)world)->lever_count; ++index) {
        EdFileRead(((LEVER_WORLD_PC *)world)->levers[index].name, 16);
        EdFileReadNuVec(&((LEVER_WORLD_PC *)world)->levers[index].position);
        ((LEVER_WORLD_PC *)world)->levers[index].yaw = EdFileReadShort();
        ((LEVER_WORLD_PC *)world)->levers[index].model_variant = EdFileReadChar();
        if (version >= 2) {
            u16 automatic = EdFileReadChar() & 1;
            ((LEVER_WORLD_PC *)world)->levers[index].flags =
                (((LEVER_WORLD_PC *)world)->levers[index].flags & ~0x20) | (automatic << 5);
        }
        ((LEVER_WORLD_PC *)world)->levers[index].auto_reset_delay = version >= 3 ? EdFileReadFloat() : 2.0f;
        if (version >= 4) {
            u16 hidden = EdFileReadChar() & 1;
            ((LEVER_WORLD_PC *)world)->levers[index].flags =
                (((LEVER_WORLD_PC *)world)->levers[index].flags & ~0x100) | (hidden << 8);
        } else
            ((LEVER_WORLD_PC *)world)->levers[index].flags &= ~0x100;
        if (version >= 5) {
            EdFileReadNuVec(&((LEVER_WORLD_PC *)world)->levers[index].target_offset);
            ((LEVER_WORLD_PC *)world)->levers[index].target_indicator_scale = EdFileReadFloat();
        } else {
            ((LEVER_WORLD_PC *)world)->levers[index].target_offset.y = 0.0f;
            ((LEVER_WORLD_PC *)world)->levers[index].target_offset.x = 0.0f;
            ((LEVER_WORLD_PC *)world)->levers[index].target_offset.z = -0.1441f;
            ((LEVER_WORLD_PC *)world)->levers[index].target_indicator_scale = 1.0f;
        }
        if (version >= 6) {
            u16 hidden = EdFileReadChar() & 1;
            ((LEVER_WORLD_PC *)world)->levers[index].flags =
                (((LEVER_WORLD_PC *)world)->levers[index].flags & ~0x200) | (hidden << 9);
        }
        ((LEVER_WORLD_PC *)world)->levers[index].platform_id = -1;
    }
    return 1;
}
static void Levers_AddGizmos(void *system, i32 type_id, void *world_ptr, void *data) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    i32 index;
    (void)data;
    if (world == NULL || ((LEVER_WORLD_PC *)world)->levers == NULL)
        return;
    for (index = 0; index < ((LEVER_WORLD_PC *)world)->lever_count; ++index) {
        GIZMO_LEVER_PC *lever = &((LEVER_WORLD_PC *)world)->levers[index];
        if (NuStrLen(lever->name) != 0)
            AddGizmo(system, type_id, NULL, lever);
    }
}
static void Levers_Update(void *world_ptr, void *data, f32 elapsed) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZMO_LEVER_PC *lever = ((LEVER_WORLD_PC *)world)->levers;
    i32 index;
    (void)data;
    (void)elapsed;
    if (lever == NULL)
        return;
    for (index = 0; index < ((LEVER_WORLD_PC *)world)->lever_count; ++index, ++lever) {
        if ((lever->flags & 2) == 0 && (lever->flags & 0x1800) == 0) {
            lever->animation_frame = 0;
            continue;
        }
        lever->flags |= 2;
        if (lever->auto_reset_timer > 0.0f) {
            lever->auto_reset_timer -= FRAMETIME;
            if (lever->auto_reset_timer <= 0.0f) {
                lever->auto_reset_timer = 0.0f;
                lever->flags = (lever->flags & ~0x800) | 0x1040;
                lever->pull_progress = 0.6f;
            }
        } else if ((lever->flags & 0x1040) == 0) {
            if (lever->pull_progress < 1.0f) {
                lever->pull_progress += FRAMETIME;
                if (lever->pull_progress > 1.0f) {
                    lever->pull_progress = 1.0f;
                    PlaySfx("SwLever", &lever->position);
                    if ((lever->flags & 0x20) != 0)
                        lever->auto_reset_timer = lever->auto_reset_delay;
                }
            }
        } else {
            lever->flags |= 0x40;
            lever->pull_progress -= FRAMETIME;
            if (lever->pull_progress <= 0.0f) {
                lever->flags &= 0xefb0;
                lever->pull_progress = 0.0f;
                lever->auto_reset_timer = 0.0f;
                lever->idle_animation_timer = 0.0f;
            }
        }
        lever->animation_frame = (u16)(lever->pull_progress * -32768.0);
    }
}
ADDGIZMOTYPE_PC *Levers_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE_PC LeversAddTypePc = {0};
    LeversAddTypePc = Default_ADDGIZMOTYPE;
    LeversAddTypePc.name = LeverTypeNamePtrPc;
    LeversAddTypePc.prefix = "";
    LeversAddTypePc.fns.options = 12;
    LeversAddTypePc.fns.get_max_gizmos_fn = Levers_GetMaxGizmos;
    LeversAddTypePc.fns.add_gizmos_fn = (void *)Levers_AddGizmos;
    LeversAddTypePc.fns.early_update_fn = NULL;
    LeversAddTypePc.fns.late_update_fn = Levers_Update;
    LeversAddTypePc.fns.draw_fn = Levers_Draw;
    LeversAddTypePc.fns.panel_draw_fn = NULL;
    LeversAddTypePc.fns.get_gizmo_name_fn = (void *)Lever_GetGizmoName;
    LeversAddTypePc.fns.get_output_fn = (void *)Lever_GetOutput;
    LeversAddTypePc.fns.get_output_name_fn = (void *)Lever_GetOutputName;
    LeversAddTypePc.fns.get_num_outputs_fn = (void *)Lever_GetNumOutputs;
    LeversAddTypePc.fns.activate_fn = (void *)Lever_Activate;
    LeversAddTypePc.fns.activate_rev_fn = (void *)Lever_ActivateRev;
    LeversAddTypePc.fns.set_visibility_fn = (void *)Lever_SetVisibility;
    LeversAddTypePc.fns.get_visibility_fn = NULL;
    LeversAddTypePc.fns.get_pos_fn = NULL;
    LeversAddTypePc.fns.using_special_fn = NULL;
    LeversAddTypePc.fns.bolt_hit_plat_fn = NULL;
    LeversAddTypePc.fns.get_best_bolt_target_fn = NULL;
    LeversAddTypePc.fns.bolt_hit_fn = NULL;
    LeversAddTypePc.fns.allocate_progress_data_fn = Levers_AllocateProgressData;
    LeversAddTypePc.fns.clear_progress_fn = Levers_ClearProgress;
    LeversAddTypePc.fns.store_progress_fn = Levers_StoreProgress;
    LeversAddTypePc.fns.reset_fn = Levers_Reset;
    LeversAddTypePc.fns.reserve_buffer_space_fn = Levers_ReserveBufferSpace;
    LeversAddTypePc.fns.load_fn = Levers_Load;
    LeversAddTypePc.fns.post_load_fn = NULL;
    LeversAddTypePc.fns.add_level_sfx_fn = NULL;
    lever_gizmotype_id = type_id;
    return &LeversAddTypePc;
}

u8 __cdecl FindNearestPlayerToVec(NUVEC *, GAMEOBJECT **, f32 *, u8, u32);
STATIC_ASSERT(offsetof(WORLDINFO, level_objects) == 0x2ac0);
STATIC_ASSERT(offsetof(WORLDINFO, game_debris_sys) == 0x134);
STATIC_ASSERT(offsetof(TIMER, time_elapsed) == 0);
STATIC_ASSERT(offsetof(TIMER, time_elapsed_mod_seconds) == 8);
STATIC_ASSERT(offsetof(LEVEL_OBJECT_RUNTIME_PC, special) == 0);

static void __cdecl Levers_Draw(void *world_ptr, void *data, f32 delta_time) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    LEVER_WORLD_PC *lever_world = (LEVER_WORLD_PC *)world;
    NUSPECIALHANDLE *animated_special = NULL;
    GIZMO_LEVER_PC *lever;
    u16 target_spin_angle;
    f32 target_pulse, ready_alpha;
    i32 index;
    if (world->level_objects[30].active == 0 || lever_world->levers == NULL)
        return;
    lever = lever_world->levers;
    target_spin_angle = (u16)((f64)NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0 * 65536.0);
    target_pulse =
        (f32)((f64)NuTrigTable[((i32)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0) >> 1) &
                               0x7fff] *
                  (f64)0.2f +
              (f64)0.8f);
    ready_alpha =
        (f32)((f64)NuTrigTable[((i32)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0) >> 1) &
                               0x7fff] *
                  (f64)0.15f +
              (f64)0.85f);
    EnableShadowMapRendering(0);
    if (StartupGameFlag1 != 0 && world->level_objects[30].active != 0) {
        NUINSTANIM_TIME_VIEW *animation;
        animated_special = &world->level_objects[30].special;
        animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(animated_special);
        if (animation != NULL)
            NuAnimEndFrameOld(((NUGSCN *)animated_special->scene)->instance_animation_data[animation->animation_index]);
    }
    for (index = 0; index < lever_world->lever_count; ++index, ++lever) {
        NUMTX animated_matrix, base_matrix, target_matrix;
        i32 status_special;
        if ((lever->flags & 0x10) == 0)
            continue;
        if (StartupGameFlag1 != 0 && animated_special != NULL) {
            f32 animation_frame = 0.0f;
            if (lever->pull_progress > 0.0f) {
                if (lever->auto_reset_timer > 0.0f)
                    animation_frame = 32.0f;
                else if ((lever->flags & 0x40) == 0)
                    animation_frame = (f32)((f64)lever->pull_progress * 31.0 + 1.0);
                else
                    animation_frame = (f32)(((f64)0.6f - (f64)lever->pull_progress) * 32.0 / (f64)0.6f + 32.0);
            } else if ((lever->flags & 0x83) == 0x80) {
                if (lever->idle_animation_timer > 0.0f) {
                    lever->idle_animation_timer = (f32)(lever->idle_animation_timer - FRAMETIME);
                    if (lever->idle_animation_timer < 0.0f)
                        lever->idle_animation_timer = 0.0f;
                    else {
                        f32 sine = NuTrigTable
                            [((i32)((f64)lever->idle_animation_timer / (f64)0.15f * 32768.0 + 16384.0) >> 1) & 0x7fff];
                        animation_frame = (f32)((1.0 - fabs((f64)sine)) * 1.5 + 16.0);
                    }
                } else if (qrand() < 0x800)
                    lever->idle_animation_timer = 0.15f;
            }
            EvalAnim(animated_special, animation_frame, &animated_matrix, 0);
            NuMtxMulVU0(&animated_matrix, &animated_matrix, &lever->matrix);
            NuSpecialDrawAt(animated_special, &animated_matrix);
        } else {
            NuMtxSetRotationX(&animated_matrix, 0);
            NuMtxRotateY(&animated_matrix, lever->yaw);
            NuMtxTranslate(&animated_matrix, &lever->position);
            NuSpecialDrawAt(&world->level_objects[30].special, &animated_matrix);
        }
        target_matrix = animated_matrix;
        NuMtxSetRotationY(&base_matrix, lever->yaw);
        NuMtxTranslate(&base_matrix, &lever->position);
        if (world->level_objects[lever->model_special_index].active != 0)
            NuSpecialDrawAt(&world->level_objects[lever->model_special_index].special, &animated_matrix);
        if (world->level_objects[31].active != 0 && (lever->flags & 0x100) == 0)
            NuSpecialDrawAt(&world->level_objects[31].special, &lever->matrix);
        status_special = 43;
        if ((lever->flags & 0x80) != 0) {
            i32 draw_target;
            i32 draw_ready;
            if (lever->pull_progress >= 1.0f)
                status_special = 42;
            draw_target = (lever->flags & 0x200) == 0 && lever->target_offset.y != 2000000.0f &&
                          world->level_objects[85].active != 0;
            draw_ready = (lever->flags & 3) == 0 && world->level_objects[41].active != 0;
            if (draw_ready)
                NuSpecialDrawAtAlpha(&world->level_objects[41].special, &target_matrix, ready_alpha);
            if (draw_target || draw_ready) {
                GAMEOBJECT *nearest_player;
                f32 distance_squared;
                if (FindNearestPlayerToVec(&lever->position, &nearest_player, &distance_squared, 0, 0) != 0) {
                    i32 angle =
                        (i32)(((f64)distance_squared / 6.0 < 1.0 ? (f64)distance_squared / 6.0 : 1.0) * 16384.0 +
                              49152.0 + 16384.0);
                    f32 alpha = (f32)((1.0 - (f64)NuTrigTable[(angle >> 1) & 0x7fff]) * (f64)target_pulse);
                    if (alpha > 0.0f && draw_target) {
                        NUVEC target_position;
                        f32 draw_alpha;
                        Lever_GetAbsTargetPos(lever, &target_position);
                        NuMtxSetRotationY(&animated_matrix, target_spin_angle);
                        if (lever->terrain_roll != 0)
                            NuMtxRotateZ(&animated_matrix, lever->terrain_roll);
                        if (lever->terrain_pitch != 0)
                            NuMtxRotateX(&animated_matrix, lever->terrain_pitch);
                        NuMtxScaleU(&animated_matrix, 0.8f);
                        NuMtxTranslate(&animated_matrix, &target_position);
                        NuMtxPreScaleU(&animated_matrix, lever->target_indicator_scale);
                        draw_alpha = (f32)((1.0 - (f64)lever->pull_progress) * (f64)alpha);
                        NuSpecialDrawAtAlpha(&world->level_objects[85].special, &animated_matrix, draw_alpha);
                    }
                }
            }
        } else if (lever->pull_progress == 0.0f && ParticlesPerSecond(0.5f, FRAMETIME) > 0)
            AddGameDebris(WORLD->game_debris_sys, 0x57, &lever->position);
        if (world->level_objects[status_special].active != 0 && (lever->flags & 0x100) == 0)
            NuSpecialDrawAt(&world->level_objects[status_special].special, &base_matrix);
    }
    ResetShadowMapRendering();
}
