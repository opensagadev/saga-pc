#include "gameapi/edptl.h"
#include "gameapi/edtools/edfile.h"
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/animlist.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/object/lever.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/fx.h"
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
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

extern "C" void LeverNuError(void) {
}

/* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
static f32 LeverFabs(f32 f) {
    f32 rv;

    *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
    return rv;
}

i32 Lever_BeingPulled(GIZMO_LEVER *lever) {
    return (lever->flags >> 1) & 1;
}

void ReleaseLever(GAMEOBJECT *object) {
    if (object->player.character_context == CHARACTER_CONTEXT_PULL_LEVER) {
        if (object->player.grapple != NULL && !object->player.lever->being_pulled) {
            object->player.lever->interacting = 0;
        }
    }
}

void Lever_GetAbsTargetPos(GIZMO_LEVER *lever, NUVEC *position) {
    if (lever != NULL && position != NULL) {
        NUVEC target = lever->target_offset;
        NuVecRotateY(&target, &target, lever->yaw);
        target.x += lever->position.x;
        target.z += lever->position.z;
        *position = target;
    }
}

void Levers_InitTerrain(WORLDINFO *world) {
    if (world->levers != NULL) {
        i32 i;
        for (i = 0; i < world->nlevers; ++i) {
            world->levers[i].platform_id = NewPlatPickupInst(&world->levers[i], 3);
            PlatInstRotate(world->levers[i].platform_id, 1);
        }
    }
}

GIZMO_LEVER *Lever_FindNearest(WORLDINFO *world, NUVEC *position, GAMEOBJECT *object, f32 *distance) {
    GIZMO_LEVER *nearest = NULL;
    GIZMO_LEVER *lever = world->levers;
    f32 nearest_distance = 1.0e9f;
    i32 index;
    for (index = 0; index < world->nlevers; ++index, ++lever) {
        f32 candidate_distance;
        if (object != NULL) {
            NUVEC target;
            if ((lever->flags & LEVER_FLAG_ENABLED) == 0 || (lever->flags & LEVER_FLAG_VISIBLE) == 0 ||
                (lever->flags & (LEVER_FLAG_INTERACTING | LEVER_FLAG_BEING_PULLED)) != 0 || lever->pull_progress != 0.0f ||
                lever->floor_position.y == 2000000.0f) {
                continue;
            }
            Lever_GetAbsTargetPos(lever, &target);
            candidate_distance = NuVecDistSqr(position, &target, NULL);
        } else {
            candidate_distance = NuVecDistSqr(position, &lever->position, NULL);
        }
        if (candidate_distance < nearest_distance) {
            nearest = lever;
            nearest_distance = candidate_distance;
        }
    }
    if (distance != NULL) {
        *distance = nearest_distance;
    }
    return nearest;
}

i32 Lever_FullyPulledDown(GIZMO_LEVER *lever) {
    return (lever->flags & LEVER_FLAG_VISIBLE) != 0 && (lever->flags & LEVER_FLAG_BEING_PULLED) != 0 && lever->pull_progress >= 1.0f;
}

void Lever_MoveCode(WORLDINFO *world, GAMEOBJECT *object) {
    f32 distance = 1.0e9f;
    GIZMO_LEVER *lever;
    if (object->player.character_context == CHARACTER_CONTEXT_PULL_LEVER) {
        object->player.context_hover_target_y += FRAMETIME;
        if (object->player.context_hover_target_y > 1.0f) {
            object->player.context_hover_target_y = 1.0f;
        }
        object->player.context_timer += FRAMETIME;
        if (object->player.context_timer >= object->player.context_duration) {
            object->player.character_context = CHARACTER_CONTEXT_NONE;
            if ((object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) != 0) {
                return;
            }
        } else {
            if ((object->player.context_variant_flags & APIOBJECT_CONTEXT_EVENT_DONE) != 0) {
                return;
            }
            if (object->character_model->model_data_b[object->player.context_animation] != NULL) {
                f32 *playing = AnimPlaying(&object->anim_packet, object->player.context_animation, 1, 0);
                if (playing == NULL) {
                    return;
                }
                if (!(*playing >= AnimListFrame(object->character_model, object->player.context_animation, 0))) {
                    return;
                }
            } else if (!(object->player.context_timer >= 0.25f)) {
                return;
            }
        }
        if ((object->character_data->model_flags & CHARACTER_MODEL_BADDIE) != 0) {
            object->player.lever->flags |= LEVER_FLAG_BADDIE;
        } else if ((object->character_data->model_flags & CHARACTER_MODEL_NEUTRAL) == 0) {
            object->player.lever->flags |= LEVER_FLAG_GOODIE;
        }
        object->player.lever->flags |= LEVER_FLAG_INTERACTION_FINISHED;
        object->player.context_variant_flags |= APIOBJECT_CONTEXT_EVENT_DONE;
        return;
    }
    if (object->character_model->model_data_b[0x5d] == NULL || object->ground_flags == 0) {
        return;
    }
    if (ObjLandReady(object) == 0 && objInNetWaitContext(object, 0x4a) == 0) {
        return;
    }
    lever = Lever_FindNearest(world, &object->lower_position, object, &distance);
    if (objInNetWaitContext(object, 0x4a) != 0) {
        object->player.context_timer -= FRAMETIME;
        if (object->player.context_timer <= 0.0f) {
            object->player.character_context = CHARACTER_CONTEXT_NONE;
            object->player.net_wait_info = NULL;
        }
    }
    if (lever != NULL) {
        f32 range = ((f64)object->bounds_radius + 0.25) * lever->target_indicator_scale;
        f32 range_squared = range * range;
        if (!(distance < range_squared)) {
            return;
        }
        if ((object->player.pad->buttons_pressed & GAMEPAD_SPECIAL) == 0 && objInNetWaitContext(object, 0x4a) == 0) {
            return;
        }
        object->player.context_timer = 0.0f;
        object->player.context_hover_target_y = 0.0f;
        object->player.character_context = CHARACTER_CONTEXT_PULL_LEVER;
        object->player.grapple = lever;
        object->player.context_animation = ACTION_PULLLEVER;
        object->player.context_duration = AnimDuration((i16)object->id, 0x5d, 0.0f, 0.0f, 1);
        if (object->player.context_duration <= 0.0f) {
            object->player.context_duration = 1.0f;
        }
        object->player.context_variant_flags &= ~APIOBJECT_CONTEXT_EVENT_DONE;
        object->movement_facing_angle = object->player.lever->yaw;
        object->player.lever->flags |= LEVER_FLAG_INTERACTING;
        object->player.lever->pull_progress = 0.0f;
        object->player.lever->auto_reset_timer = 0.0f;
    }
}

typedef struct LEVER_PROGRESS { // size 0xc
    u32 pulled_down[1];         // 0x0
    u32 enabled[1];             // 0x4
    u32 visible[1];             // 0x8
} LEVER_PROGRESS;

static void Levers_Draw(void *, void *, f32);

static void Lever_SelectModel(GIZMO_LEVER *lever) {
    if (lever->model_variant == 'r') {
        lever->model_special_index = 0x22;
    } else if (lever->model_variant == 'o') {
        lever->model_special_index = 0x21;
    } else if (lever->model_variant == 'y') {
        lever->model_special_index = 0x20;
    } else if (lever->model_variant == 'b') {
        lever->model_special_index = 0x23;
    } else if (lever->model_variant == 'g') {
        lever->model_special_index = 0x24;
    } else if (lever->model_variant == 'p') {
        lever->model_special_index = 0x25;
    } else if (lever->model_variant == 'w') {
        lever->model_special_index = 0x26;
    } else if (lever->model_variant == 'u') {
        lever->model_special_index = 0x27;
    } else if (lever->model_variant == 'l') {
        lever->model_special_index = 0x28;
    }
}

static void Lever_Reset(GIZMO_LEVER *lever) {
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
    lever->flags |= LEVER_FLAG_VISIBLE | LEVER_FLAG_ENABLED;
    lever->animation_frame = 0;
    if ((lever->flags & LEVER_FLAG_BEING_PULLED) == 0 || (lever->flags & LEVER_FLAG_AUTO_RESET) == 0) {
        lever->pull_progress = 0.0f;
        lever->auto_reset_timer = 0.0f;
        lever->flags &= 0xefb0;
    }
    NuMtxSetRotationY(&lever->matrix, lever->yaw);
    NuMtxTranslate(&lever->matrix, &lever->position);
}

static i32 Levers_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    if (world != NULL) {
        return world->current_level->max_levers;
    }
    return 0;
}

static char *Lever_GetGizmoName(GIZMO *gizmo) {
    if (gizmo != NULL) {
        return gizmo->lever->name;
    }
    return NULL;
}

static i32 Lever_GetOutput(GIZMO *gizmo, i32 output, i32 mode) {
    GIZMO_LEVER *lever;
    lever = (GIZMO_LEVER *)gizmo->object;
    if ((lever->flags & LEVER_FLAG_ENABLED) != 0) {
        switch (output) {
            case LEVER_OUTPUT_DOWN:
                if (Lever_FullyPulledDown(lever) != 0) {
                    return 1;
                }
                break;
            case LEVER_OUTPUT_DOWN_GOODIE:
                if ((lever->flags & LEVER_FLAG_GOODIE) != 0 && Lever_FullyPulledDown(lever) != 0) {
                    return 1;
                }
                break;
            case LEVER_OUTPUT_DOWN_BADDIE:
                if ((lever->flags & LEVER_FLAG_BADDIE) != 0 && Lever_FullyPulledDown(lever) != 0) {
                    return 1;
                }
                break;
        }
    }
    return 0;
}

static char *Lever_GetOutputName(GIZMO *gizmo, i32 output) {
    switch (output) {
        case LEVER_OUTPUT_DOWN:
            return LeverOutputDownNamePtr;
        case LEVER_OUTPUT_DOWN_GOODIE:
            return "Down(Goodie)";
        case LEVER_OUTPUT_DOWN_BADDIE:
            return "Down(Baddie)";
    }
    return NULL;
}

static i32 Lever_GetNumOutputs(GIZMO *gizmo) {
    return 3;
}

static void Lever_Activate(GIZMO *gizmo, i32 enabled) {
    GIZMO_LEVER *lever;
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }
    lever = (GIZMO_LEVER *)gizmo->object;
    lever->enabled = enabled != 0;
    if (enabled != 0) {
        Lever_Reset(lever);
    }
}

static i32 Lever_ActivateRev(GIZMO *gizmo, i32 value, i32 query) {
    GIZMO_LEVER *lever;
    if (gizmo == NULL || gizmo->object == NULL) {
        return 1;
    }
    lever = (GIZMO_LEVER *)gizmo->object;
    if ((query & 1) != 0) {
        return lever->enabled ? value : value == 0;
    }
    lever->enabled = value == 0;
    return 1;
}

static void Lever_SetVisibility(GIZMO *gizmo, i32 visible) {
    GIZMO_LEVER *lever;
    i32 was_visible;
    if (gizmo == NULL || gizmo->object == NULL) {
        return;
    }
    lever = (GIZMO_LEVER *)gizmo->object;
    was_visible = lever->visible;
    lever->visible = visible != 0;
    if (lever->visible && was_visible == 0) {
        lever->platform_id = NewPlatPickupInst(lever, 3);
        PlatInstRotate(lever->platform_id, 1);
    } else if (!lever->visible && was_visible != 0) {
        DeletePlatinst(lever->platform_id);
    }
}

static void *Levers_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
    return GizmoBufferAlloc(buffer, end, sizeof(LEVER_PROGRESS));
}

static void Levers_ClearProgress(void *world, void *progress_ptr) {
    LEVER_PROGRESS *progress = (LEVER_PROGRESS *)progress_ptr;
    if (progress == NULL) {
        return;
    }
    memset(progress->pulled_down, 0, sizeof(progress->pulled_down));
    memset(progress->enabled, 0xff, sizeof(progress->enabled));
    memset(progress->visible, 0xff, sizeof(progress->visible));
}

static void Levers_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    LEVER_PROGRESS *progress = (LEVER_PROGRESS *)progress_ptr;
    GIZMO_LEVER *lever;
    i32 index;
    if (progress == NULL) {
        return;
    }
    Levers_ClearProgress(NULL, progress);
    if (world == NULL) {
        return;
    }
    lever = world->levers;
    if (lever == NULL) {
        return;
    }
    for (index = 0; index < world->nlevers && index < 32; ++index, ++lever) {
        i32 word = index / 32;
        u32 bit = 1 << (index & 31);
        if (lever->being_pulled && lever->pull_progress >= 1.0f) {
            progress->pulled_down[word] |= bit;
        }
        if (!lever->visible) {
            progress->visible[word] &= ~bit;
        }
        if (!lever->enabled) {
            progress->enabled[word] &= ~bit;
        }
    }
}

static void Levers_Reset(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    LEVER_PROGRESS *progress = (LEVER_PROGRESS *)progress_ptr;
    GIZMO_LEVER *lever;
    i32 index;
    if (world == NULL) {
        return;
    }
    lever = world->levers;
    if (lever == NULL) {
        return;
    }
    for (index = 0; index < world->nlevers; ++index, ++lever) {
        Lever_Reset(lever);
        if (progress != NULL && index < 32) {
            i32 word = index / 32;
            u32 bit = 1 << (index & 31);
            if ((progress->pulled_down[word] & bit) != 0) {
                lever->pull_progress = 1.0f;
                lever->being_pulled = 1;
                lever->animation_frame = 0x8000;
            }
            lever->visible = (progress->visible[word] & bit) != 0;
            lever->enabled = (progress->enabled[word] & bit) != 0;
        }
    }
}

static void *Levers_ReserveBufferSpace(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    world->levers = NULL;
    world->nlevers = 0;
    if (world->current_level->max_levers > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 15) & ~15u;
        world->levers = (GIZMO_LEVER *)world->giz_buffer.void_ptr;
        world->giz_buffer.addr += world->current_level->max_levers * sizeof(GIZMO_LEVER);
    }
    return world->levers;
}

static i32 Levers_Load(void *world_ptr, void *data) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    i32 version;
    i32 index;
    if (world->nlevers == 0) {
        version = EdFileReadInt();
        world->nlevers = EdFileReadInt();
        for (index = 0; index < world->nlevers; ++index) {
            EdFileRead(world->levers[index].name, 16);
            EdFileReadNuVec(&world->levers[index].position);
            world->levers[index].yaw = EdFileReadShort();
            world->levers[index].model_variant = EdFileReadChar();
            if (version >= 2) {
                char bit = EdFileReadChar() & 1;
                world->levers[index].flags = (world->levers[index].flags & ~LEVER_FLAG_AUTO_RESET) | (bit << 5);
            }
            if (version >= 3) {
                world->levers[index].auto_reset_delay = EdFileReadFloat();
            } else {
                world->levers[index].auto_reset_delay = 2.0f;
            }
            if (version >= 4) {
                char bit = EdFileReadChar() & 1;
                world->levers[index].flags = (world->levers[index].flags & ~LEVER_FLAG_HIDE_BASE) | (bit << 8);
            } else {
                world->levers[index].flags &= ~LEVER_FLAG_HIDE_BASE;
            }
            if (version >= 5) {
                EdFileReadNuVec(&world->levers[index].target_offset);
                world->levers[index].target_indicator_scale = EdFileReadFloat();
            } else {
                world->levers[index].target_offset.y = 0.0f;
                world->levers[index].target_offset.x = 0.0f;
                world->levers[index].target_offset.z = -0.1441f;
                world->levers[index].target_indicator_scale = 1.0f;
            }
            if (version >= 6) {
                char bit = EdFileReadChar() & 1;
                world->levers[index].flags = (world->levers[index].flags & ~LEVER_FLAG_HIDE_TARGET_INDICATOR) | (bit << 9);
            }
            world->levers[index].platform_id = -1;
        }
        return 1;
    }
    return 0;
}

static void Levers_AddGizmos(void *system, i32 type_id, void *world_ptr, void *data) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    i32 index;
    for (index = 0; index < world->nlevers; ++index) {
        if (NuStrLen(world->levers[index].name) != 0) {
            AddGizmo(system, type_id, NULL, &world->levers[index]);
        }
    }
}

static void Levers_Update(void *world_ptr, void *data, f32 elapsed) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    GIZMO_LEVER *lever = world->levers;
    i32 index;
    if (lever == NULL) {
        return;
    }
    for (index = 0; index < world->nlevers; ++index, ++lever) {
        if (!lever->being_pulled && (lever->flags & (LEVER_FLAG_INTERACTION_FINISHED | LEVER_FLAG_PULLED_DOWN)) == 0) {
            lever->animation_frame = 0;
            continue;
        }
        if (!lever->being_pulled) {
            lever->being_pulled = 1;
        }
        if (lever->auto_reset_timer > 0.0f) {
            lever->auto_reset_timer -= FRAMETIME;
            if (lever->auto_reset_timer <= 0.0f) {
                lever->auto_reset_timer = 0.0f;
                lever->flags = (lever->flags & ~LEVER_FLAG_INTERACTION_FINISHED) | LEVER_FLAG_RETURNING | LEVER_FLAG_PULLED_DOWN;
                lever->pull_progress = 0.6f;
            }
        } else if (!lever->returning && (lever->flags & LEVER_FLAG_PULLED_DOWN) == 0) {
            if (lever->pull_progress < 1.0f) {
                lever->pull_progress += FRAMETIME;
                if (lever->pull_progress > 1.0f) {
                    lever->pull_progress = 1.0f;
                    PlaySfx("SwLever", &lever->position);
                    if ((lever->flags & LEVER_FLAG_AUTO_RESET) != 0) {
                        lever->auto_reset_timer = lever->auto_reset_delay;
                    }
                }
            }
        } else {
            if (!lever->returning) {
                lever->returning = 1;
            }
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

i32 lever_gizmotype_id = -1;
char *LeverOutputDownNamePtr = "Down";
char *LeverTypeNamePtr = "Lever";

ADDGIZMOTYPE *Levers_RegisterGizmo(i32 type_id) {
    static ADDGIZMOTYPE LeversAddType = {0};
    LeversAddType = Default_ADDGIZMOTYPE;
    LeversAddType.name = LeverTypeNamePtr;
    LeversAddType.prefix = "";
    LeversAddType.fns.options = sizeof(LEVER_PROGRESS);
    LeversAddType.fns.get_max_gizmos_fn = Levers_GetMaxGizmos;
    LeversAddType.fns.add_gizmos_fn = (void *)Levers_AddGizmos;
    LeversAddType.fns.early_update_fn = NULL;
    LeversAddType.fns.late_update_fn = Levers_Update;
    LeversAddType.fns.draw_fn = Levers_Draw;
    LeversAddType.fns.panel_draw_fn = NULL;
    LeversAddType.fns.get_gizmo_name_fn = (char *(*)(GIZMO *))Lever_GetGizmoName;
    LeversAddType.fns.get_output_fn = (i32(*)(GIZMO *, i32, i32))Lever_GetOutput;
    LeversAddType.fns.get_output_name_fn = (char *(*)(GIZMO *, i32))Lever_GetOutputName;
    LeversAddType.fns.get_num_outputs_fn = (void *)Lever_GetNumOutputs;
    LeversAddType.fns.activate_fn = (void (*)(GIZMO *, i32))Lever_Activate;
    LeversAddType.fns.activate_rev_fn = (i32(*)(GIZMO *, i32, i32))Lever_ActivateRev;
    LeversAddType.fns.set_visibility_fn = (void (*)(GIZMO *, i32))Lever_SetVisibility;
    LeversAddType.fns.get_visibility_fn = NULL;
    LeversAddType.fns.get_pos_fn = NULL;
    LeversAddType.fns.using_special_fn = NULL;
    LeversAddType.fns.bolt_hit_plat_fn = NULL;
    LeversAddType.fns.get_best_bolt_target_fn = NULL;
    LeversAddType.fns.bolt_hit_fn = NULL;
    LeversAddType.fns.allocate_progress_data_fn = Levers_AllocateProgressData;
    LeversAddType.fns.clear_progress_fn = Levers_ClearProgress;
    LeversAddType.fns.store_progress_fn = Levers_StoreProgress;
    LeversAddType.fns.reset_fn = Levers_Reset;
    LeversAddType.fns.reserve_buffer_space_fn = Levers_ReserveBufferSpace;
    LeversAddType.fns.load_fn = Levers_Load;
    LeversAddType.fns.post_load_fn = NULL;
    LeversAddType.fns.add_level_sfx_fn = NULL;
    lever_gizmotype_id = type_id;
    return &LeversAddType;
}

static void Levers_Draw(void *world_ptr, void *data, f32 delta_time) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    nuhspecial_s *animated_special;
    GIZMO_LEVER *lever;
    u16 target_spin_angle;
    f32 target_pulse;
    f32 ready_alpha;
    i32 index;
    if (world->level_objects[30].active == 0 || world->levers == NULL) {
        return;
    }
    lever = world->levers;
    target_spin_angle = (u16)((f64)NuFmod(GameTimer.time_elapsed, 5.0f) / 5.0 * 65536.0);
    target_pulse =
        (f32)((f64)NuTrigTable[((i32)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0) >> 1) & 0x7fff] * (f64)0.2f +
              (f64)0.8f);
    ready_alpha =
        (f32)((f64)NuTrigTable[((i32)((f64)NuFmod(GameTimer.time_elapsed_mod_seconds, 0.5f) * 2.0 * 65536.0) >> 1) & 0x7fff] * (f64)0.15f +
              (f64)0.85f);
    EnableShadowMapRendering(0);
    if (StartupGameFlag1 != 0 && world->level_objects[30].active != 0) {
        NUINSTANIM *animation;
        animated_special = &world->level_objects[30].special;
        animation = (NUINSTANIM *)NuSpecialGetInstAnim(animated_special);
        if (animated_special != NULL && animation != NULL) {
            NuAnimEndFrameOld(animated_special->scene->instance_animation_data[animation->animation_index]);
        }
    } else {
        animated_special = NULL;
    }
    for (index = 0; index < world->nlevers; ++index, ++lever) {
        NUMTX animated_matrix;
        NUMTX base_matrix;
        NUMTX target_matrix;
        i32 status_special;
        if ((lever->flags & LEVER_FLAG_VISIBLE) == 0) {
            continue;
        }
        if (StartupGameFlag1 != 0 && animated_special != NULL) {
            f32 animation_frame = 0.0f;
            if (lever->pull_progress > 0.0f) {
                if (lever->auto_reset_timer > 0.0f) {
                    animation_frame = 32.0f;
                } else if ((lever->flags & LEVER_FLAG_RETURNING) == 0) {
                    animation_frame = (f32)((f64)lever->pull_progress * 31.0 + 1.0);
                } else {
                    animation_frame = (f32)(((f64)0.6f - (f64)lever->pull_progress) * 32.0 / (f64)0.6f + 32.0);
                }
            } else if ((lever->flags & (LEVER_FLAG_INTERACTING | LEVER_FLAG_BEING_PULLED | LEVER_FLAG_ENABLED)) == LEVER_FLAG_ENABLED) {
                if (lever->idle_animation_timer > 0.0f) {
                    lever->idle_animation_timer = lever->idle_animation_timer - FRAMETIME;
                    if (lever->idle_animation_timer < 0.0f) {
                        lever->idle_animation_timer = 0.0f;
                    } else {
                        f32 sine = NuTrigTable[((i32)((f64)lever->idle_animation_timer / (f64)0.15f * 32768.0 + 16384.0) >> 1) & 0x7fff];
                        animation_frame = (f32)((1.0 - LeverFabs(sine)) * 1.5 + 16.0);
                    }
                } else if (qrand() < 0x800) {
                    lever->idle_animation_timer = 0.15f;
                }
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
        if (world->level_objects[lever->model_special_index].active != 0) {
            NuSpecialDrawAt(&world->level_objects[lever->model_special_index].special, &animated_matrix);
        }
        if (world->level_objects[31].active != 0 && (lever->flags & LEVER_FLAG_HIDE_BASE) == 0) {
            NuSpecialDrawAt(&world->level_objects[31].special, &lever->matrix);
        }
        status_special = 43;
        if ((lever->flags & LEVER_FLAG_ENABLED) != 0) {
            i32 draw_target;
            i32 draw_ready;
            if (lever->pull_progress >= 1.0f) {
                status_special = 42;
            }
            draw_target = (lever->flags & LEVER_FLAG_HIDE_TARGET_INDICATOR) == 0 && lever->target_offset.y != 2000000.0f &&
                          world->level_objects[85].active != 0;
            draw_ready = (lever->flags & (LEVER_FLAG_INTERACTING | LEVER_FLAG_BEING_PULLED)) == 0 && world->level_objects[41].active != 0;
            if (draw_ready) {
                NuSpecialDrawAtAlpha(&world->level_objects[41].special, &target_matrix, ready_alpha);
            }
            if (draw_target || draw_ready) {
                GAMEOBJECT *nearest_player;
                f32 distance_squared;
                if (FindNearestPlayerToVec(&lever->position, &nearest_player, &distance_squared, 0, 0) != 0) {
                    i32 angle =
                        (i32)(((f64)distance_squared / 6.0 < 1.0 ? (f64)distance_squared / 6.0 : 1.0) * 16384.0 + 49152.0 + 16384.0);
                    f32 alpha = (f32)((1.0 - (f64)NuTrigTable[(angle >> 1) & 0x7fff]) * (f64)target_pulse);
                    if (alpha > 0.0f && draw_target) {
                        NUVEC target_position;
                        f32 draw_alpha;
                        Lever_GetAbsTargetPos(lever, &target_position);
                        NuMtxSetRotationY(&animated_matrix, target_spin_angle);
                        if (lever->terrain_roll != 0) {
                            NuMtxRotateZ(&animated_matrix, lever->terrain_roll);
                        }
                        if (lever->terrain_pitch != 0) {
                            NuMtxRotateX(&animated_matrix, lever->terrain_pitch);
                        }
                        NuMtxScaleU(&animated_matrix, 0.8f);
                        NuMtxTranslate(&animated_matrix, &target_position);
                        NuMtxPreScaleU(&animated_matrix, lever->target_indicator_scale);
                        draw_alpha = (f32)((1.0 - (f64)lever->pull_progress) * (f64)alpha);
                        NuSpecialDrawAtAlpha(&world->level_objects[85].special, &animated_matrix, draw_alpha);
                    }
                }
            }
        } else if (lever->pull_progress == 0.0f && ParticlesPerSecond(0.5f, FRAMETIME) > 0) {
            AddGameDebris(WORLD->game_debris_sys, 0x57, &lever->position);
        }
        if (world->level_objects[status_special].active != 0 && (lever->flags & LEVER_FLAG_HIDE_BASE) == 0) {
            NuSpecialDrawAt(&world->level_objects[status_special].special, &base_matrix);
        }
    }
    ResetShadowMapRendering();
}
