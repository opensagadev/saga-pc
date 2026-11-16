#include "batman.h"
#include "gameapi/edtools/edfile.h"
#include "globals.h"
#include "legoapi/ai/game/gameantinode.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/action_info.h"
#include "legoapi/characters/motion/contexts.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmos/door/gizspinner.h"
#include "legoapi/gizmos/door/spinner.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/collect/bolt.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/objects/cable.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/light/shadow.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episodeIII.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "legogame/game.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

/* One saved entry per spinner; the gizmo progress block holds eight of them. */
typedef struct SpinnerProgressEntry { // size 0x8
    f32 position;                     // 0x0
    u16 rotation;                     // 0x4
    u8 flags;                         // 0x6
    u8 unused;                        // 0x7
} SpinnerProgressEntry;

/* Takes these absolute values through the float's bit pattern. */
static f32 SpinnerFabs(f32 value) {
    i32 bits = *(i32 *)&value & 0x7fffffff;
    f32 result = *(f32 *)&bits;
    return result;
}

/* Looks the special up in the level scene first, then in the shared things scene. */
static i32 SpinnerFindSpecial(WORLDINFO *world, nuhspecial_s *special, char *name) {
    NuSpecialFind(world->current_gscn, special, name, 0);
    if (NuSpecialExistsFn(special) != 0) {
        return 1;
    }
    NuSpecialFind(things_scene, special, name, 0);
    return NuSpecialExistsFn(special) != 0;
}

// Private helper; receives spinner in EAX, bolt in ESI, and flags on the stack.
// A four-byte aggregate preserves that stack slot under VC8's private argument optimization.
struct SPINNER_HIT_FLAGS_STACK { // size 0x4
    u8 *value;                   // 0x0
};

extern "C" {
    extern const f64 GameObjectNearFloorNoFloorHeight;
    nuhspecial_s thingsSceneArm = {0};
    nuhspecial_s thingsSceneBase = {0};
    static ADDGIZMOTYPE GizmoRegistrySpinnerAddType = {0};
}

extern "C" void SpinnerMissingSpecial(char *format, ...) {
    (void)format;
}

static void SpinnerMatrixSetRotationY(NUMTX *matrix, i32 angle) {
    f32 cosine = NuTrigTable[(((i32)((u32)angle + 0x4000u)) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    matrix->m00 = cosine;
    matrix->m01 = 0.0f;
    matrix->m02 = -sine;
    matrix->m03 = 0.0f;
    matrix->m10 = ANI3_EVAL_IDENTITY_MATRIX.m10;
    matrix->m11 = ANI3_EVAL_IDENTITY_MATRIX.m11;
    matrix->m12 = ANI3_EVAL_IDENTITY_MATRIX.m12;
    matrix->m13 = ANI3_EVAL_IDENTITY_MATRIX.m13;
    matrix->m20 = sine;
    matrix->m21 = 0.0f;
    matrix->m22 = cosine;
    matrix->m23 = 0.0f;
    matrix->m30 = ANI3_EVAL_IDENTITY_MATRIX.m30;
    matrix->m31 = ANI3_EVAL_IDENTITY_MATRIX.m31;
    matrix->m32 = ANI3_EVAL_IDENTITY_MATRIX.m32;
    matrix->m33 = ANI3_EVAL_IDENTITY_MATRIX.m33;
}

static void SpinnerMatrixRotateY(NUMTX *matrix, i32 angle) {
    f32 cosine = NuTrigTable[(((i32)((u32)angle + 0x4000u)) >> 1) & 0x7fff];
    f32 sine = NuTrigTable[(angle >> 1) & 0x7fff];
    f32 m00 = matrix->m00;
    f32 m10 = matrix->m10;
    f32 m20 = matrix->m20;
    f32 m30 = matrix->m30;
    matrix->m00 = (f64)m00 * cosine + (f64)matrix->m02 * sine;
    matrix->m02 = (f64)matrix->m02 * cosine - (f64)m00 * sine;
    matrix->m10 = (f64)matrix->m12 * sine + (f64)m10 * cosine;
    matrix->m12 = (f64)matrix->m12 * cosine - (f64)m10 * sine;
    matrix->m20 = (f64)m20 * cosine + (f64)matrix->m22 * sine;
    matrix->m22 = (f64)matrix->m22 * cosine - (f64)m20 * sine;
    matrix->m30 = (f64)matrix->m32 * sine + (f64)m30 * cosine;
    matrix->m32 = (f64)matrix->m32 * cosine - (f64)m30 * sine;
}

static GAMEOBJECT *SpinnerOwner(GIZSPINNER_s *spinner) {
    if (LEGOCONTEXT_PUSHSPINNER == -1) {
        return NULL;
    }
    for (i32 index = 0; index < 8; ++index) {
        if (Player[index] != NULL && Player[index]->player.character_context == LEGOCONTEXT_PUSHSPINNER &&
            Player[index]->player.grapple == spinner) {
            return Player[index];
        }
    }
    return NULL;
}

extern "C" {
    static void SpinnerBoundsReset(GIZSPINNER_s *spinner) {
        NUVEC minimum;
        NUVEC maximum;
        NUVEC arm_minimum;
        NUVEC arm_maximum;
        f32 x_extent;
        f32 z_extent;
        NuSpecialGetBounds(&spinner->special, &minimum, &maximum);
        if (NuSpecialCompare(&thingsSceneBase, &spinner->special) != 0) {
            NuSpecialGetBounds(&thingsSceneArm, &arm_minimum, &arm_maximum);
            NuVecAdd(&minimum, &minimum, &arm_minimum);
            NuVecAdd(&maximum, &maximum, &arm_maximum);
        }
        x_extent = maximum.x - minimum.x;
        z_extent = maximum.z - minimum.z;
        if (x_extent > z_extent) {
            spinner->unknown_094 = x_extent;
        } else {
            spinner->unknown_094 = z_extent;
        }
        spinner->state_flags |= SPINNER_STATE_PUSHABLE;
        spinner->animation_speed = 1.0f;
        spinner->type = 4;
        spinner->animation_points[0] = 1.0f;
        spinner->unknown_08c = 0;
        spinner->rotation = spinner->initial_rotation;
        spinner->output_count = 1;
        spinner->unknown_09c = (f32)((f64)spinner->unknown_094 * 0.5);
        spinner->unknown_094 = (f32)((f64)spinner->unknown_094 * (f64)0.4f);
        spinner->unknown_098 = 0.0f;
    }
}

extern "C" i32 GizSpinner_Load(void *world_pointer, void *unused) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    GIZSPINNER_s *spinner;
    GAMEANIMSET *anim_set;
    GAMEANIMOBJ *object;
    nuhspecial_s special;
    i32 version;
    i32 index;
    i32 name_length;
    i32 output_points;
    i32 object_count;
    i32 object_index;
    i32 point;
    i32 special_missing;
    i32 spinner_type;
    i32 spinner_flags;
    u32 spinner_state_flags;
    char special_name[32];
    f32 longest_duration = 0.0f;
    f32 animation_speed;
    f32 initial_animation_point;
    f32 spinner_field_98;
    f32 start_frame;
    f32 end_frame;
    f32 frame_duration;
    f32 rounded_point;
    (void)unused;
    if (world->spinners != NULL) {
        NuSpecialFind(things_scene, &thingsSceneBase, "Spinner_default_Base", 0);
        NuSpecialFind(things_scene, &thingsSceneArm, "Spinner_default_Arm", 0);
        if (world->spinner_count == 0) {
            version = EdFileReadInt();
            world->spinner_count = EdFileReadInt();
            for (index = 0; index < world->spinner_count; ++index) {
                animation_speed = 1.0f;
                special_missing = 0;
                spinner_state_flags = 0;
                spinner_flags = 0;
                spinner_type = 4;
                anim_set = world->spinners[index].anim_set;
                memset(&world->spinners[index], 0, sizeof(GIZSPINNER_s));
                world->spinners[index].anim_set = anim_set;
                name_length = EdFileReadChar();
                EdFileRead(world->spinners[index].name, name_length);
                EdFileReadNuVec(&world->spinners[index].position);
                world->spinners[index].initial_rotation = EdFileReadShort();
                name_length = EdFileReadChar();
                EdFileRead(special_name, name_length);
                // MATCH-EXCEPTION: the SpinnerFindSpecial lookup written out; each result is tested in place here.
                NuSpecialFind(world->current_gscn, &special, special_name, 0);
                if (NuSpecialExistsFn(&special) == 0) {
                    NuSpecialFind(things_scene, &special, special_name, 0);
                    if (NuSpecialExistsFn(&special) == 0) {
                        special_missing = 1;
                    }
                }
                world->spinners[index].special = special;
                output_points = EdFileReadChar();
                if (version >= 2) {
                    spinner_type = EdFileReadChar();
                    if (version >= 3) {
                        spinner_state_flags = (u32)EdFileReadInt() & ~0x360u;
                        world->spinners[index].unknown_2d8 = EdFileReadFloat();
                        if (version >= 4) {
                            animation_speed = EdFileReadFloat();
                            if (version >= 6) {
                                spinner_flags = EdFileReadChar();
                            }
                        }
                    }
                }
                GameAnimSet_RemoveAllObjects(world->spinners[index].anim_set);
                if (version >= 5) {
                    GizmoFileReadGameAnimSet(world->spinners[index].anim_set, world, NULL, (u8)version, "Spinner",
                                             world->spinners[index].name);
                    for (object = world->spinners[index].anim_set->objects; object != NULL; object = object->next) {
                        if (world->spinners[index].primary_anim_obj == NULL) {
                            if (object != NULL && object->animation != NULL) {
                                world->spinners[index].primary_anim_obj = object;
                                longest_duration = SpinnerFabs(object->end_frame - object->start_frame);
                            }
                        } else if (object != NULL && object->animation != NULL) {
                            f32 duration = SpinnerFabs(object->end_frame - object->start_frame);
                            if (duration > longest_duration) {
                                longest_duration = duration;
                                world->spinners[index].primary_anim_obj = object;
                            }
                        }
                    }
                } else {
                    object_count = EdFileReadChar();
                    for (object_index = 0; object_index < object_count; ++object_index) {
                        start_frame = 1.0f;
                        end_frame = AIPositionUnset;
                        name_length = EdFileReadChar();
                        EdFileRead(special_name, name_length);
                        SpinnerFindSpecial(world, &special, special_name);
                        if (version >= 3) {
                            start_frame = EdFileReadFloat();
                            end_frame = EdFileReadFloat();
                        }
                        object = GameAnimSet_AddObject(world->spinners[index].anim_set, &special, start_frame, end_frame, 0);
                        if (world->spinners[index].primary_anim_obj == NULL) {
                            if (object != NULL && object->animation != NULL) {
                                world->spinners[index].primary_anim_obj = object;
                                longest_duration = SpinnerFabs(object->end_frame - object->start_frame);
                            }
                        } else if (object != NULL && object->animation != NULL) {
                            f32 duration = SpinnerFabs(object->end_frame - object->start_frame);
                            if (duration > longest_duration) {
                                longest_duration = duration;
                                world->spinners[index].primary_anim_obj = object;
                            }
                        }
                    }
                }
                if (version >= 7) {
                    for (point = 0; point < output_points; ++point) {
                        world->spinners[index].animation_points[point + 1] = EdFileReadFloat();
                    }
                    for (point = output_points; point < 8; ++point) {
                        world->spinners[index].animation_points[point + 1] = -1.0f;
                    }
                } else if (world->spinners[index].anim_set != NULL && world->spinners[index].primary_anim_obj != NULL) {
                    object = world->spinners[index].primary_anim_obj;
                    start_frame = object->start_frame;
                    frame_duration = object->end_frame - start_frame;
                    for (point = 0; point < output_points; ++point) {
                        rounded_point = (i32)(point * (frame_duration / (output_points - 1))) + object->start_frame;
                        world->spinners[index].animation_points[point + 1] = rounded_point;
                    }
                    for (point = output_points; point < 8; ++point) {
                        world->spinners[index].animation_points[point + 1] = -1.0f;
                    }
                }
                spinner_field_98 = version >= 8 ? EdFileReadFloat() : 0.0f;
                initial_animation_point = version >= 9 ? EdFileReadFloat() : animation_speed;
                if (special_missing) {
                    world->spinners[index].flags &= ~GIZSPINNER_FLAG_VALID;
                    SpinnerMissingSpecial(world->spinners[index].name, special_name);
                    --world->spinner_count;
                    --index;
                    continue;
                }
                spinner = &world->spinners[index];
                SpinnerBoundsReset(spinner);
                world->spinners[index].type = (u8)spinner_type;
                world->spinners[index].animation_speed = animation_speed;
                world->spinners[index].animation_points[0] = initial_animation_point;
                world->spinners[index].output_count = (u8)output_points;
                world->spinners[index].state_flags = spinner_state_flags;
                world->spinners[index].flags = spinner_flags;
                world->spinners[index].unknown_098 = spinner_field_98;
                world->spinners[index].flags |= GIZSPINNER_FLAG_VALID;
                world->spinners[index].flags &= 0xddu;
            }
            return 1;
        }
    }
    return 0;
}

extern "C" void *GizSpinner_ReserveBufferSpace(void *pointer) {
    WORLDINFO *world = (WORLDINFO *)pointer;
    i32 i;
    world->spinners = NULL;
    world->spinner_count = 0;
    if (world->current_level->max_spinners != 0) {
        world->spinner_anim_pool =
            GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 0, world->current_level->max_spinneranim_objs);
        world->giz_buffer.addr = (world->giz_buffer.addr + 15) & ~15u;
        world->spinners = (GIZSPINNER_s *)world->giz_buffer.void_ptr;
        world->giz_buffer.addr += world->current_level->max_spinners * sizeof(*world->spinners);
        memset(world->spinners, 0, world->current_level->max_spinners * sizeof(*world->spinners));
        for (i = 0; i < world->current_level->max_spinners; ++i) {
            world->spinners[i].anim_set =
                GameAnimSet_Create(&world->giz_buffer, &world->buffer_end, world->spinner_anim_pool, world->game_anim_sys);
        }
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
    }
    return world->spinners;
}

extern "C" {
    i32 LEGOTHINGSSCENE_TER_SPINBASE = -1;
    i32 LEGOTHINGSSCENE_TER_SPINARM = -1;
}

extern "C" void GizSpinners_InitTerrain(WORLDINFO *world) {
    i32 index;
    i32 use_default_terrain = 0;
    if (world->spinners == NULL) {
        return;
    }
    for (index = 0; index < world->current_level->max_spinners; ++index) {
        if ((world->spinners[index].flags & GIZSPINNER_FLAG_VALID) != 0) {
            NUMTX matrix;
            i32 instance = NuSpecialGetInstanceix(&world->spinners[index].special);
            FindPlatInst(instance);
            NuMtxSetIdentity(&matrix);
            NuMtxSetRotationY(&matrix, 0);
            NuMtxRotateY(&matrix, world->spinners[index].rotation);
            NuMtxTranslate(&matrix, &world->spinners[index].position);
            world->spinners[index].matrix = matrix;

            if (NuSpecialCompare(&world->spinners[index].special, &thingsSceneBase) != 0) {
                use_default_terrain = 1;
            }
            if (use_default_terrain != 0) {
                i32 arm;
                if (LEGOTHINGSSCENE_TER_SPINBASE != -1) {
                    world->spinners[index].platform_id = NewPlatPickupInst(&world->spinners[index].matrix, LEGOTHINGSSCENE_TER_SPINBASE);
                } else {
                    world->spinners[index].platform_id = -1;
                }
                for (arm = 0; arm < world->spinners[index].type; ++arm) {
                    if (LEGOTHINGSSCENE_TER_SPINARM != -1) {
                        world->spinners[index].arms[arm].platform_id =
                            NewPlatPickupInst(&world->spinners[index].arms[arm].matrix, LEGOTHINGSSCENE_TER_SPINARM);
                    } else {
                        world->spinners[index].arms[arm].platform_id = -1;
                    }
                    PlatInstRotate(world->spinners[index].arms[arm].platform_id, 1);
                }
            } else {
                i32 arm;
                world->spinners[index].platform_id = ((i16)(NewPlatInst(&world->spinners[index].matrix, instance)));
                for (arm = 0; arm < world->spinners[index].type; ++arm) {
                    world->spinners[index].arms[arm].platform_id = -1;
                }
            }
        }
    }
}

extern "C" GIZSPINNER_s *GizSpinner_FindBySpecialName(void *world_ptr, char *name) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    if (world == NULL || name == NULL) {
        return NULL;
    }
    for (i32 index = 0; index < world->current_level->max_spinners; ++index) {
        GIZSPINNER_s *spinner = &world->spinners[index];
        if (spinner != NULL && (spinner->flags & GIZSPINNER_FLAG_VALID) != 0 && NuStrCmp(NuSpecialGetName(&spinner->special), name) == 0) {
            return spinner;
        }
    }
    return NULL;
}

extern "C" void GizSpinner_Draw(void *world_pointer, void *data, f32 frame_time) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    GIZSPINNER_s *spinner;
    GIZSPINNERARM_s *arm;
    NUMTX reflection;
    f32 ground_height;
    f32 override_height;
    i32 index;
    i32 arm_index;
    if (world->spinners == NULL || world->current_level->max_spinners == 0) {
        return;
    }
    for (index = 0; index < world->current_level->max_spinners; ++index) {
        EnableShadowMapRendering(0);
        spinner = &world->spinners[index];
        if (!(spinner->flags & GIZSPINNER_FLAG_VALID) || ((spinner->flags & GIZSPINNER_FLAG_HIDDEN) && editor_active == 0)) {
            continue;
        }
        if (NuSpecialExistsFn(&spinner->special)) {
            NuSpecialDrawAt(&spinner->special, &spinner->matrix);
            if (spinner->type > 0 && NuSpecialCompare(&spinner->special, &thingsSceneBase) != 0 &&
                NuSpecialExistsFn(&thingsSceneArm) != 0) {
                arm = spinner->arms;
                for (arm_index = 0; arm_index < spinner->type; ++arm_index, ++arm) {
                    NuSpecialDrawAt(&thingsSceneArm, &arm->matrix);
                }
            }
        }
        ResetShadowMapRendering();
        if (!(spinner->state_flags & SPINNER_STATE_SHADOW) || !NuSpecialExistsFn(&spinner->special) ||
            !NuSpecialGetVisibilityFn(&spinner->special)) {
            continue;
        }
        ground_height = spinner->ground_height;
        override_height = WORLD->current_level->reflection_y;
        if (!MatrixReflectionVU0_AXISY(&spinner->matrix, ground_height, override_height, &reflection)) {
            continue;
        }
        NuRndrStartReflectionRender(0);
        NuSpecialDrawAt(&spinner->special, &reflection);
        if (spinner->type > 0 && NuSpecialCompare(&spinner->special, &thingsSceneBase) != 0 && NuSpecialExistsFn(&thingsSceneArm) != 0) {
            arm = spinner->arms;
            for (arm_index = 0; arm_index < spinner->type; ++arm_index, ++arm) {
                NuSpecialDrawAt(&thingsSceneArm, &arm->matrix);
            }
        }
        NuRndrEndReflectionRender();
    }
}

extern "C" void GizSpinner_PanelDraw(void *world, void *system, f32 frame_time) {
    (void)world;
    (void)system;
    (void)frame_time;
}

extern "C" i32 GizSpinner_GetMaxGizmos(void *world_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    return world->current_level->max_spinners;
}

extern "C" void GizSpinner_AddGizmos(GIZMOSYS *giz, i32 type, void *world_ptr, void *data) {
    WORLDINFO *w = (WORLDINFO *)world_ptr;
    i32 i;
    (void)data;
    if (w->spinners == NULL) {
        return;
    }
    for (i = 0; i < w->current_level->max_spinners; ++i) {
        if ((w->spinners[i].flags & GIZSPINNER_FLAG_VALID) && NuStrLen(w->spinners[i].name) != 0) {
            AddGizmo(giz, type, NULL, &w->spinners[i]);
        }
    }
}

extern "C" char *GizSpinner_GetGizmoName(void *gizmo_pointer) {
    GIZMO *gizmo = (GIZMO *)gizmo_pointer;
    return gizmo != NULL ? gizmo->spinner->name : NULL;
}

extern "C" char *GizSpinner_GetOutputName(void *gizmo, i32 output) {
    GIZSPINNER_s *spinner = (GIZSPINNER_s *)((GIZMO *)gizmo)->object;
    if (output < 0 || output > spinner->output_count) {
        return NULL;
    }
    if (spinner->output_count >= 2) {
        f32 fraction = (f32)output / (spinner->output_count - 1);
        f32 percentage = (f64)fraction * 100.0;
        NuIToA((i32)percentage, SpinnerOutputBuffer, 10);
        NuStrCat(SpinnerOutputBuffer, "%% Complete");
        return SpinnerOutputBuffer;
    }
    if (spinner->output_count != 0) {
        NuStrCpy(SpinnerOutputBuffer, "100%% Complete");
        return SpinnerOutputBuffer;
    }
    NuStrCpy(SpinnerOutputBuffer, "");
    return SpinnerOutputBuffer;
}

extern "C" i32 GizSpinner_GetNumOutputs(void *gizmo) {
    return ((GIZSPINNER_s *)((GIZMO *)gizmo)->object)->output_count;
}

static i32 GizSpinner_WithinOutput(GIZSPINNER_s *spinner) {
    GAMEANIMOBJ *object;
    NUINSTANIM *instance;
    if (spinner == NULL || spinner->anim_set == NULL || (object = spinner->primary_anim_obj) == NULL ||
        (instance = object->instance_animation) == NULL) {
        return -1;
    }
    f32 length = (f64)object->end_frame - object->start_frame + 1.0;
    f32 margin = length / 40.0;
    i32 last = spinner->output_count - 1;
    for (i32 output = 0; output < last; ++output) {
        f32 point = spinner->animation_points[output + 1];
        if (instance->ltime < (f64)point + margin && instance->ltime > (f64)point - margin) {
            return output;
        }
    }
    if (instance->ltime < (f64)length + margin && instance->ltime > (f64)length - margin) {
        return last;
    }
    return -1;
}

static void SpinnerSeekOutput(GIZSPINNER_s *spinner, i32 output) {
    GAMEANIMOBJ *object;
    NUINSTANIM *instance;
    if (spinner == NULL || spinner->anim_set == NULL || (object = spinner->primary_anim_obj) == NULL ||
        (instance = object->instance_animation) == NULL || output == -1 || (spinner->state_flags & SPINNER_STATE_PUSHED) != 0) {
        return;
    }
    f32 rate = instance->fparam1;
    u32 flags = spinner->state_flags;
    i32 direction = ((flags & SPINNER_STATE_POSITIVE) != 0 && (flags & SPINNER_STATE_REVERSED) == 0) ||
                            ((flags & SPINNER_STATE_POSITIVE) == 0 && (flags & SPINNER_STATE_REVERSED) != 0)
                        ? 1
                        : -1;
    f32 length = (f64)object->end_frame - object->start_frame + 1.0;
    f32 target;
    if (spinner->output_count == 1) {
        target = 1.0f;
    } else {
        target = ((f64)spinner->animation_points[output + 1] - 1.0) / ((f64)length - 1.0);
    }
    f32 duration = SpinnerFabs(length / ((f64)spinner->animation_speed * rate * 60.0));
    f32 frequency = SpinnerFabs(60.0 * spinner->animation_speed * rate / length);
    f32 difference = SpinnerFabs((f64)target - spinner->unknown_70);
    if (difference < 0.001f) {
        GameAnimSet_Stop(spinner->anim_set);
        GameAnimSet_JumpToAnimPos(spinner->anim_set, target);
        f32 rotation = (f64)duration * 30.0;
        rotation = rotation / length;
        rotation = rotation * (length / 100.0);
        spinner->rotation = (u16)((i32)(direction * ((f64)target * 100.0 * rotation * 65536.0 / 360.0)) + spinner->initial_rotation);
    } else if (spinner->unknown_70 > target) {
        GameAnimSet_Play(spinner->anim_set, -0.25f, 1);
        spinner->rotation -= (i32)(direction * ((f64)frequency * difference * 5461.0));
    } else if (target > spinner->unknown_70) {
        GameAnimSet_Play(spinner->anim_set, 0.25f, 1);
        spinner->rotation += (i32)(direction * ((f64)frequency * difference * 5461.0));
    }
}

/* 0 while the animation is reset, 2 once it has reached its end, 1 otherwise. */
extern "C" i32 GizSpinner_GetState(GIZSPINNER_s *spinner) {
    if (GameAnimSet_IsAnimationReset(spinner->anim_set) != 0) {
        return 0;
    }
    return (spinner->anim_set->state == GAMEANIMSET_STATE_AT_END) + 1;
}

extern "C" {
    i16 GizSpinnerGDeb_Fail[3] = {-1, -1, -1};
}

extern "C" void GizSpinner_PushFail(GAMEOBJECT *object, GIZSPINNER_s *) {
    NewBuzzFrames(object->player.pad->bound_pad, 1, 0);
    NUVEC position;
    position.x = ((GIZSPINNER_s *)object->player.grapple)->position.x;
    i32 random = qrand();
    position.y = random * (f64)(1.0f / 65535.0f) * (f64)0.25f + (f64)0.15f + ((GIZSPINNER_s *)object->player.grapple)->position.y;
    position.z = ((GIZSPINNER_s *)object->player.grapple)->position.z;
    AddGameDebris(WORLD->game_debris_sys, GizSpinnerGDeb_Fail[1], &position);
    AddGameDebris(WORLD->game_debris_sys, GizSpinnerGDeb_Fail[2], &position);
    GameAudio_PlaySfx(0x39, &position, 0, 0);
}

extern "C" i32 GizSpinner_Push(GIZSPINNER_s *spinner, i32 context) {
    f32 frame_time = FRAMETIME;
    i32 rotation_step = (i32)((f64)frame_time * 5461.0);
    spinner->state_flags &= ~(SPINNER_STATE_NUDGE_POSITIVE | SPINNER_STATE_NUDGE_NEGATIVE);

    i32 result = 0;
    if ((spinner->state_flags & SPINNER_STATE_HALTED) != 0) {
        spinner->state_flags &= ~SPINNER_STATE_HALTED;
        goto finalize;
    }

    {
        i32 at_start = GameAnimSet_IsAnimationReset(spinner->anim_set);
        i32 at_end = spinner->anim_set->state == GAMEANIMSET_STATE_AT_END;
        u32 flags = spinner->state_flags;
        i32 clear_context = 0;
        u32 end_flag = flags & SPINNER_STATE_RELEASE_AT_END;

        if (context == 0x1e) {
            if ((flags & SPINNER_STATE_REVERSED) != 0) {
                if (at_start != 0) {
                    if (spinner->anim_set->object_count != 0) {
                        goto stop_forward;
                    }
                } else if (at_end && end_flag != 0) {
                    clear_context = 1;
                    goto stop_forward;
                }
            } else if (at_end && spinner->anim_set->object_count != 0) {
                goto stop_forward;
            }
            if ((flags & SPINNER_STATE_POSITIVE) != 0) {
                spinner->rotation += rotation_step;
            } else {
                spinner->rotation -= rotation_step;
            }
            goto rotated;

        stop_forward:
            flags &= ~SPINNER_STATE_PUSHED;
            spinner->state_flags = flags;
            if ((flags & SPINNER_STATE_POSITIVE) != 0) {
                flags |= SPINNER_STATE_NUDGE_POSITIVE;
            } else {
                flags |= SPINNER_STATE_NUDGE_NEGATIVE;
            }
        } else {
            if ((flags & SPINNER_STATE_REVERSED) != 0) {
                if (at_end && spinner->anim_set->object_count != 0) {
                    goto stop_reverse;
                }
            } else if (at_start != 0) {
                if (spinner->anim_set->object_count != 0) {
                    goto stop_reverse;
                }
            } else if (at_end && end_flag != 0) {
                clear_context = 1;
                goto stop_reverse;
            }
            if ((flags & SPINNER_STATE_POSITIVE) != 0) {
                spinner->rotation -= rotation_step;
            } else {
                spinner->rotation += rotation_step;
            }
            goto rotated;

        stop_reverse:
            flags &= ~SPINNER_STATE_PUSHED;
            spinner->state_flags = flags;
            if ((flags & SPINNER_STATE_POSITIVE) != 0) {
                flags |= SPINNER_STATE_NUDGE_NEGATIVE;
            } else {
                flags |= SPINNER_STATE_NUDGE_POSITIVE;
            }
        }
        spinner->state_flags = flags;
        if (clear_context != 0) {
            context = -1;
        }
        goto finalize;

    rotated:
        spinner->state_flags = flags | SPINNER_STATE_PUSHED;
        result = 1;
    }

finalize:
    if (RotDiff(spinner->rotation, spinner->previous_rotation) < 0) {
        spinner->state_flags |= SPINNER_STATE_TURNING_POSITIVE;
    } else {
        spinner->state_flags &= ~SPINNER_STATE_TURNING_POSITIVE;
    }
    spinner->room_index = static_cast<i8>(context);
    spinner->target_rotation = spinner->rotation;
    spinner->previous_rotation = spinner->rotation;
    spinner->spin_timer = 0.0f;
    return result;
}

extern "C" i32 GizSpinner_Spin(GIZSPINNER_s *spinner, i32 context) {
    if ((spinner->state_flags & SPINNER_STATE_SIDE_30_ONLY) != 0 && context != 0x1e) {
        return 2;
    }
    if ((spinner->state_flags & SPINNER_STATE_SIDE_31_ONLY) != 0 && context != 0x1f) {
        return 2;
    }

    if (spinner->rotation != spinner->target_rotation || spinner->spin_timer > 0.0f || spinner->anim_set == NULL ||
        spinner->room_index != -1) {
        return 0;
    }
    if (GameAnimSet_GetVisibility(spinner->anim_set) != GAMEANIMSET_VISIBILITY_ALL) {
        return 1;
    }

    f32 speed;
    u32 flags = spinner->state_flags;
    if ((context == 0x1e && (flags & SPINNER_STATE_REVERSED) == 0) || (context != 0x1e && (flags & SPINNER_STATE_REVERSED) != 0)) {
        if (spinner->anim_set->state == GAMEANIMSET_STATE_AT_END) {
            return 2;
        }
        speed = spinner->animation_speed;
        if ((context == 0x1e && (flags & SPINNER_STATE_POSITIVE) != 0) || (context != 0x1e && (flags & SPINNER_STATE_POSITIVE) == 0)) {
            flags |= SPINNER_STATE_TURNING_POSITIVE;
        } else {
            flags &= ~SPINNER_STATE_TURNING_POSITIVE;
        }
    } else {
        if (GameAnimSet_IsAnimationReset(spinner->anim_set) != 0) {
            return 2;
        }
        speed = -spinner->animation_speed;
        if ((context == 0x1f && (flags & SPINNER_STATE_POSITIVE) == 0) || (context != 0x1f && (flags & SPINNER_STATE_POSITIVE) != 0)) {
            flags |= SPINNER_STATE_TURNING_POSITIVE;
        } else {
            flags &= ~SPINNER_STATE_TURNING_POSITIVE;
        }
    }
    spinner->state_flags = flags;

    i32 step = 0x10000 / spinner->type;
    if ((spinner->state_flags & SPINNER_STATE_TURNING_POSITIVE) != 0) {
        spinner->target_rotation = spinner->rotation + step;
    } else {
        spinner->target_rotation = spinner->rotation - step;
    }
    GameAnimSet_Play(spinner->anim_set, speed, 1);
    spinner->previous_rotation = spinner->rotation;
    spinner->spin_timer = 0.25f;
    return 3;
}

extern "C" i32 GizSpinner_GetTargetPoints(GIZSPINNER_s *spinner, NUVEC *positions, NUVEC *directions) {
    if (spinner == NULL) {
        return 0;
    }
    u32 flags = spinner->state_flags & (SPINNER_STATE_POSITIVE | SPINNER_STATE_REVERSED);
    if (spinner->type == 0) {
        return 0;
    }
    i32 step = 65536 / spinner->type;
    f32 y = spinner->position.y + spinner->unknown_098;
    u16 position_angle = spinner->unknown_08c + spinner->initial_rotation + spinner->rotation + 0x8000;
    u16 direction_angle =
        (flags == 0 || flags == (SPINNER_STATE_POSITIVE | SPINNER_STATE_REVERSED)) ? position_angle + 0x4000 : position_angle - 0x4000;
    i32 count = 0;
    for (; count < spinner->type; ++count) {
        if (directions != NULL) {
            directions[count].x = NuTrigTable[direction_angle >> 1];
            directions[count].y = 0.0f;
            directions[count].z = NuTrigTable[((direction_angle + 0x4000) >> 1) & 0x7fff];
        }
        if (positions != NULL) {
            positions[count].x = spinner->unknown_094 * NuTrigTable[position_angle >> 1] + spinner->position.x;
            positions[count].y = y;
            positions[count].z = spinner->unknown_094 * NuTrigTable[((position_angle + 0x4000) >> 1) & 0x7fff] + spinner->position.z;
        }
        direction_angle += step;
        position_angle += step;
    }
    return count;
}

extern "C" void GizSpinner_StoreProgressData(void *world_ptr, void *data, void *progress_ptr) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    SpinnerProgressEntry *progress = (SpinnerProgressEntry *)progress_ptr;
    GIZSPINNER_s *spinner;
    i32 index;
    if (progress == NULL) {
        return;
    }
    if (world->spinners == NULL) {
        return;
    }
    for (index = 0; index < world->current_level->max_spinners; ++index, ++progress) {
        spinner = &world->spinners[index];
        if (spinner->flags & GIZSPINNER_FLAG_VALID) {
            progress->flags |= 1;
            progress->rotation = spinner->target_rotation;
            progress->flags = (u8)((progress->flags & ~4u) | ((~(spinner->flags >> 1) & 1u) << 2));
            progress->flags = (u8)((progress->flags & ~2u) | ((~(spinner->flags >> 2) & 1u) << 1));
            if (GameAnimSet_IsAnimationReset(spinner->anim_set)) {
                progress->position = 0.0f;
            }
            if (spinner->anim_set->state == GAMEANIMSET_STATE_AT_END) {
                progress->position = 1.0f;
            } else if (spinner->anim_set != NULL) {
                progress->position = GameAnimSet_GetAnimPos(spinner->primary_anim_obj);
            }
        }
    }
}

extern "C" void *GizSpinner_AllocateProgressData(void *buffer, void *end) {
    return GizmoBufferAlloc((VARIPTR *)buffer, (VARIPTR *)end, 0x40);
}

extern "C" void GizSpinner_ClearProgress(void *owner, void *progress_ptr) {
    void *progress = progress_ptr;
    if (progress == NULL) {
        return;
    }
    memset(progress, 0, 0x40);
}

static void SpinnerResetEntry(GIZSPINNER_s *spinner, SpinnerProgressEntry *progress) {
    if (progress != NULL && (progress->flags & 1) != 0) {
        spinner->rotation = progress->rotation;
        if (progress->position == 1.0f) {
            GameAnimSet_JumpToEnd(spinner->anim_set);
        } else {
            GameAnimSet_JumpToAnimPos(spinner->anim_set, progress->position);
        }
        if ((progress->flags & 4) != 0) {
            spinner->flags &= ~GIZSPINNER_FLAG_INACTIVE;
        } else {
            spinner->flags |= GIZSPINNER_FLAG_INACTIVE;
        }
        if ((progress->flags & 2) != 0) {
            spinner->flags &= ~GIZSPINNER_FLAG_HIDDEN;
        } else {
            spinner->flags |= GIZSPINNER_FLAG_HIDDEN;
        }
    } else {
        spinner->rotation = spinner->initial_rotation;
        GameAnimSet_JumpToAnimPos(spinner->anim_set, 0.0f);
    }
    spinner->spin_timer = 0.0f;
    spinner->target_rotation = spinner->rotation;
    spinner->previous_rotation = spinner->rotation;
    spinner->room_index = -1;
    spinner->state = GameAnimSet_IsAnimationReset(spinner->anim_set) != 0 ? 0 : (spinner->anim_set->state == GAMEANIMSET_STATE_AT_END) + 1;
    spinner->state_flags =
        (spinner->state_flags & ~(SPINNER_STATE_NUDGE_POSITIVE | SPINNER_STATE_NUDGE_NEGATIVE)) | SPINNER_STATE_NO_JUDDER;
    NuMtxSetRotationY(&spinner->matrix, spinner->initial_rotation);
    NuMtxTranslate(&spinner->matrix, &spinner->position);
    NewTerrPlatformsOff();
    if (NuSpecialGetMtx(&spinner->special) != NULL) {
        spinner->ground_height = GameShadow(NULL, (NUVEC *)&NuSpecialGetMtx(&spinner->special)->m30, 5.0f, -1);
    } else {
        spinner->ground_height = LevelConfigFarDistance;
    }
    if ((f64)spinner->ground_height != GameObjectNearFloorNoFloorHeight && ShadowInfo() == 2) {
        spinner->state_flags |= SPINNER_STATE_SHADOW;
    }
}

extern "C" void GizSpinner_Activate(void *gizmo_pointer, i32 active) {
    if (gizmo_pointer != NULL) {
        GIZSPINNER_s *spinner = (GIZSPINNER_s *)((GIZMO *)gizmo_pointer)->object;
        if (active != 0) {
            spinner->flags &= (u8) ~(GIZSPINNER_FLAG_INACTIVE | GIZSPINNER_FLAG_INVISIBLE);
            spinner->state_flags &= ~SPINNER_STATE_HALTED;
        } else {
            spinner->flags |= GIZSPINNER_FLAG_INACTIVE;
        }
    }
}

extern "C" NUVEC *GizSpinner_GetPos(void *gizmo_pointer) {
    GIZMO *gizmo = (GIZMO *)gizmo_pointer;
    if (gizmo == NULL || gizmo->object == NULL) {
        return NULL;
    }
    return &gizmo->spinner->position;
}

extern "C" void GizSpinner_SetVisibility(void *gizmo_pointer, i32 visible) {
    NUMTX matrix;
    if (gizmo_pointer == NULL) {
        return;
    }
    GIZSPINNER_s *spinner = (GIZSPINNER_s *)((GIZMO *)gizmo_pointer)->object;
    if (visible != 0) {
        spinner->flags &= (u8)~GIZSPINNER_FLAG_HIDDEN;
        if (spinner->platform_id == -1) {
            i32 instance_index = NuSpecialGetInstanceix(&spinner->special);
            FindPlatInst(instance_index);
            NuMtxSetIdentity(&matrix);
            NuMtxSetRotationY(&matrix, 0);
            NuMtxRotateY(&matrix, spinner->rotation);
            NuMtxTranslate(&matrix, &spinner->position);
            spinner->matrix = matrix;
            if (NuSpecialCompare(&spinner->special, &thingsSceneBase) != 0) {
                if (LEGOTHINGSSCENE_TER_SPINBASE != -1) {
                    spinner->platform_id = NewPlatPickupInst(spinner, LEGOTHINGSSCENE_TER_SPINBASE);
                } else {
                    spinner->platform_id = -1;
                }
                for (i32 index = 0; index < spinner->type; ++index) {
                    if (LEGOTHINGSSCENE_TER_SPINARM != -1) {
                        spinner->arms[index].platform_id = NewPlatPickupInst(&spinner->arms[index].matrix, LEGOTHINGSSCENE_TER_SPINARM);
                    } else {
                        spinner->arms[index].platform_id = -1;
                    }
                    PlatInstRotate(spinner->arms[index].platform_id, 1);
                }
            } else {
                spinner->platform_id = (i16)NewPlatInst(spinner, instance_index);
            }
        }
        if (spinner->anti_node == NULL) {
            spinner->anti_node = (GAMEANTINODE_s *)GameAntinode_RegisterAntiNode(WORLD->game_antinode_sys, &spinner->position, 0.66f, 1.0f,
                                                                                 1.0f, spinner->rotation, 0, 0.0f);
        }
    } else {
        spinner->flags |= GIZSPINNER_FLAG_HIDDEN;
        DeletePlatinst(spinner->platform_id);
        spinner->platform_id = -1;
        for (i32 index = 0; index < spinner->type; ++index) {
            if (spinner->arms[index].platform_id != -1) {
                DeletePlatinst(spinner->arms[index].platform_id);
                spinner->arms[index].platform_id = -1;
            }
        }
        if (spinner->anti_node != NULL) {
            GameAntinode_UnregisterAntiNode(WORLD->game_antinode_sys, spinner->anti_node);
            spinner->anti_node = NULL;
        }
    }
}

extern "C" void GizSpinner_GetSpinnerPos(GIZSPINNER_s *spinner, NUVEC *position) {
    if (position != NULL && spinner != NULL) {
        *position = spinner->position;
    }
}

extern "C" i32 GizSpinner_UsingSpecial(void *results_ptr, void *world_ptr, i32 capacity, char *name) {
    GIZMO **results = (GIZMO **)results_ptr;
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    i32 count = 0;
    i32 type_id = -1;
    i32 index;
    nuhspecial_s special;
    GIZSPINNER_s *cached_spinner;
    if (results == NULL || world == NULL || world->spinners == NULL) {
        return 0;
    }
    NuSpecialFind(world->current_gscn, &special, name, 0);
    if (NuSpecialExistsFn(&special) == 0) {
        return 0;
    }
    cached_spinner = world->spinners;
    for (index = 0; index < world->current_level->max_spinners; ++index, ++cached_spinner) {
        GAMEANIMOBJ *object;
        i32 searching;
        if (count >= capacity) {
            break;
        }
        if ((world->spinners[index].flags & GIZSPINNER_FLAG_VALID) == 0) {
            continue;
        }
        object = cached_spinner->anim_set->objects;
        searching = 1;
        for (; object != NULL && searching != 0; object = object->next) {
            if (NuSpecialCompare(&special, &object->special) != 0) {
                GIZMO *gizmo = (GIZMO *)GizmoFindByName(world->gizmo_sys, type_id, name);
                if (gizmo != NULL) {
                    type_id = gizmo->type_id;
                    results[count++] = gizmo;
                    searching = 0;
                }
            }
        }
    }
    return count;
}

extern "C" {
    static void GizSpinner_BoltHitPlat(GIZSPINNER_s *spinner, BOLT *bolt, SPINNER_HIT_FLAGS_STACK argument) {
        u8 *hit_flags = argument.value;
        GAMEOBJECT *owner = bolt->owner;
        if (spinner == NULL || (spinner->state_flags & SPINNER_STATE_PUSHABLE) != 0) {
            return;
        }
        i32 state = GizSpinner_Spin(spinner, bolt->unknown_104);
        if ((state == 2 || state == 1) && hit_flags != NULL) {
            GameAudio_PlaySfx(0x29, &bolt->position, 0, 0);
            Bolt_AddDeflectedBolt(bolt, &bolt->velocity, &bolt->hit_normal, hit_flags);
        }
        NewRumble(owner->player.pad->bound_pad, 0.5f, 0);
        Bolt_PlayHitSfx(bolt);
    }
}

extern "C" i32 GizSpinner_BoltHitPlatAll(WORLDINFO *, void *spinner_ptr, BOLT *bolt, u8 *hit_flags) {
    GIZSPINNER_s *spinner = (GIZSPINNER_s *)spinner_ptr;
    if (spinner == NULL || (spinner->flags & GIZSPINNER_FLAG_VALID) == 0 || (spinner->flags & GIZSPINNER_FLAG_HIDDEN) != 0) {
        return 0;
    }
    if ((spinner->flags & (GIZSPINNER_FLAG_INACTIVE | GIZSPINNER_FLAG_INVISIBLE)) != 0 && spinner->platform_id == bolt->hit_platform) {
        return 1;
    }
    SPINNER_HIT_FLAGS_STACK argument = {hit_flags};
    if (spinner->platform_id == bolt->hit_platform) {
        GizSpinner_BoltHitPlat(spinner, bolt, argument);
        return 1;
    }
    for (i32 arm = 0; arm < spinner->type; ++arm) {
        if (spinner->arms[arm].platform_id == bolt->hit_platform) {
            GizSpinner_BoltHitPlat(spinner, bolt, argument);
            return 1;
        }
    }
    return 0;
}

extern "C" i32 *GizSpinner_GetBestBoltTarget(GIZMOSET *, f32 *, NUVEC *result_position, NUVEC *result_velocity, void *, NUVEC *position,
                                             NUVEC *direction, f32 radius, f32 range_squared, i32 directional, i32, i32) {
    WORLDINFO *world = WorldInfo_CurrentlyActive();
    GIZMO *result = NULL;
    NUVEC target_position = v000;
    NUVEC aim = *direction;
    if (world->spinners == NULL) {
        return NULL;
    }
    f32 best_alignment = 0.0f;
    NUVEC minimum;
    NUVEC maximum;
    minimum.x = position->x - radius;
    minimum.z = position->z - radius;
    maximum.x = position->x + radius;
    maximum.z = position->z + radius;
    GIZMOSET *set = &world->gizmo_sys->sets[spinner_gizmotype_id];
    for (i32 index = 0; index < set->count; ++index, set = &world->gizmo_sys->sets[spinner_gizmotype_id]) {
        GIZMO *gizmo = &set->gizmos[index];
        GIZSPINNER_s *spinner = &world->spinners[index];
        if (spinner->flags == 0 ||
            (spinner->flags & (GIZSPINNER_FLAG_INACTIVE | GIZSPINNER_FLAG_HIDDEN | GIZSPINNER_FLAG_INVISIBLE)) != 0 ||
            (spinner->state_flags & SPINNER_STATE_PUSHABLE) != 0 || spinner->position.x < minimum.x || spinner->position.x > maximum.x ||
            spinner->position.z < minimum.z || spinner->position.z > maximum.z) {
            continue;
        }
        target_position.y = spinner->position.y + spinner->unknown_098;
        u16 step = spinner->type != 0 ? (u16)(0x10000 / spinner->type) : 0x4000;
        u16 angle = (u16)(spinner->rotation + spinner->initial_rotation + spinner->unknown_08c);
        u16 tangent_angle = (u16)(angle + 0x4000);
        for (i32 arm = 0; arm < spinner->type; ++arm, angle += step, tangent_angle += step) {
            target_position.x = spinner->position.x + NuTrigTable[angle >> 1] * spinner->unknown_094;
            target_position.z = spinner->position.z + NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] * spinner->unknown_094;
            NUVEC tangent;
            tangent.x = NuTrigTable[tangent_angle >> 1];
            tangent.z = NuTrigTable[((tangent_angle + 0x4000) >> 1) & 0x7fff];
            NUVEC facing;
            f32 absolute_dot;
            if (directional == 0) {
                NuVecRotateY(&facing, &v001, NuAtan2D(spinner->position.x - position->x, spinner->position.z - position->z));
                absolute_dot = SpinnerFabs(facing.z * tangent.z + facing.x * tangent.x);
            } else {
                absolute_dot = SpinnerFabs(direction->z * tangent.z + direction->x * tangent.x);
            }
            if (!(absolute_dot > NuTrigTable[0x3000])) {
                continue;
            }
            NUVEC delta;
            f32 distance = NuVecDistSqr(&target_position, position, &delta);
            if (!(distance < range_squared)) {
                continue;
            }
            if (directional == 0) {
                NuVecRotateY(&aim, &v001, NuAtan2D(target_position.x - position->x, target_position.z - position->z));
            } else {
                aim = *direction;
            }
            NuVecNorm(&delta, &delta);
            f32 dot = NuVecDot(&delta, &aim);
            u16 target_angle;
            if (distance < TargetDist_Near2 && directional != 0) {
                target_angle = TargetDeg_Near;
            } else if (TargetDist_Mid2 > distance) {
                target_angle = TargetDeg_Mid;
            } else {
                target_angle = TargetDeg_Far;
            }
            if (!(dot > NuTrigTable[((target_angle + 0x4000) >> 1) & 0x7fff])) {
                continue;
            }
            NuVecRotateY(&delta, &v001, NuAtan2D(target_position.x - position->x, target_position.z - position->z));
            f32 alignment = delta.x * aim.x + delta.z * aim.z;
            if (!(alignment > best_alignment)) {
                continue;
            }
            best_alignment = alignment;
            *result_velocity = v000;
            *result_position = target_position;
            result = gizmo;
        }
    }
    return (i32 *)result;
}

extern "C" GIZSPINNER_s *GizSpinner_Find(WORLDINFO *world, NUVEC *position, i32 alternate) {
    f32 nearest_distance = 1.5625f;
    GIZSPINNER_s *nearest = NULL;
    for (i32 index = 0; index < world->current_level->max_spinners; ++index) {
        GIZSPINNER_s *spinner = &world->spinners[index];
        if ((spinner->flags & GIZSPINNER_FLAG_VALID) == 0 ||
            (spinner->flags & (GIZSPINNER_FLAG_INACTIVE | GIZSPINNER_FLAG_HIDDEN | GIZSPINNER_FLAG_INVISIBLE)) != 0) {
            continue;
        }
        if (alternate != 0) {
            if ((spinner->state_flags & SPINNER_STATE_PUSHABLE) == 0) {
                continue;
            }
        } else if ((spinner->state_flags & SPINNER_STATE_PUSHABLE) != 0) {
            continue;
        }
        NUVEC spinner_position = spinner->position;
        f32 distance = NuVecDistSqr(position, &spinner_position, NULL);
        if (distance < nearest_distance) {
            nearest_distance = distance;
            nearest = spinner;
        }
    }
    return nearest;
}

/* The nearest usable spinner; its squared distance goes to distance_out when given. */
extern "C" GIZSPINNER_s *GizSpinner_FindNearest(NUVEC *position, WORLDINFO *world, f32 *distance_out) {
    f32 nearest_distance = 1000000000.0f;
    GIZSPINNER_s *nearest = NULL;
    if (world->spinners == NULL) {
        return NULL;
    }
    for (i32 index = 0; index < world->current_level->max_spinners; ++index) {
        GIZSPINNER_s *spinner = &world->spinners[index];
        if ((spinner->flags & GIZSPINNER_FLAG_VALID) == 0 ||
            (spinner->flags & (GIZSPINNER_FLAG_INACTIVE | GIZSPINNER_FLAG_HIDDEN | GIZSPINNER_FLAG_INVISIBLE)) != 0) {
            continue;
        }
        NUVEC spinner_position = spinner->position;
        f32 distance = NuVecDistSqr(position, &spinner_position, NULL);
        if (distance < nearest_distance) {
            nearest_distance = distance;
            nearest = spinner;
        }
    }
    if (distance_out != NULL) {
        *distance_out = nearest_distance;
    }
    return nearest;
}

extern "C" i32 GizSpinner_GetOutput(void *gizmo_pointer, i32 output_index, i32 unused) {
    GIZSPINNER_s *spinner = (GIZSPINNER_s *)((GIZMO *)gizmo_pointer)->object;
    (void)unused;
    if (spinner != NULL) {
        return GizSpinner_WithinOutput(spinner) == output_index;
    }
    return 0;
}

extern "C" void GizSpinner_Update(GIZSPINNER_s *spinner) {
    u32 flags = spinner->state_flags;
    i32 initial_no_judder = flags & SPINNER_STATE_NO_JUDDER;
    flags &= ~SPINNER_STATE_NO_JUDDER;
    i32 initial_positive_offset = flags & SPINNER_STATE_NUDGE_POSITIVE;
    i32 initial_negative_offset = flags & SPINNER_STATE_NUDGE_NEGATIVE;
    flags &= ~(SPINNER_STATE_NUDGE_POSITIVE | SPINNER_STATE_NUDGE_NEGATIVE);
    spinner->state_flags = flags;
    i32 stopped = 0;
    if ((flags & SPINNER_STATE_HALTED) != 0) {
        GameAnimSet_Stop(spinner->anim_set);
        stopped = 1;
    }
    NUVEC minimum;
    NUVEC maximum;
    minimum.x = spinner->position.x - spinner->unknown_09c;
    maximum.x = spinner->unknown_09c + spinner->position.x;
    minimum.z = spinner->position.z - spinner->unknown_09c;
    maximum.z = spinner->unknown_09c + spinner->position.z;
    if ((spinner->state_flags & (SPINNER_STATE_REVERSED | SPINNER_STATE_PUSHABLE)) == 0) {
        GAMEOBJECT *object = Obj;
        for (i32 index = 0; index < HIGHGAMEOBJECT; ++index, ++object) {
            if ((object->flags_1fc_word & APIOBJECT_FLAG_IN_USE) == 0 || (object->flags_1fc_word & APIOBJECT_FLAG_CHARACTER) == 0 ||
                object->dead != 0 || (object->player.flags_e20 & 0x20) != 0 ||
                (U64_PAIR(&CInfo[object->player.character_context].flags) & 0x40000000) != 0 ||
                (U64_PAIR(&CInfo[object->player.character_context].flags) & 0x8000) != 0 || minimum.x > object->collision_max.x ||
                maximum.x < object->collision_min.x || minimum.z > object->collision_max.z || maximum.z < object->collision_min.z) {
                continue;
            }
            f32 radius = (f64)spinner->unknown_09c + 0.5 + object->bounds_radius;
            f32 dx = (f64)object->position.x - spinner->position.x;
            f32 dz = (f64)object->position.z - spinner->position.z;
            f32 radius_squared = radius * radius;
            if (!(dx * dx + dz * dz < radius_squared)) {
                continue;
            }
            u16 radial = dx != 0.0f || dz != 0.0f ? NuAtan2D(dx, dz) : qrand();
            u16 movement;
            f32 speed;
            if (object->velocity.x == 0.0f && object->velocity.z == 0.0f) {
                movement = object->facing_angle;
                speed = object->character_data->game_character->tiptoe_speed;
            } else {
                movement = NuAtan2D(object->velocity.x, object->velocity.z);
                f32 length_squared = (f64)object->velocity.x * object->velocity.x + (f64)object->velocity.z * object->velocity.z;
                speed = NuFsqrt(length_squared);
                if (speed < object->character_data->game_character->tiptoe_speed) {
                    speed = object->character_data->game_character->tiptoe_speed;
                } else if (speed < object->character_data->game_character->run_speed) {
                    speed = object->character_data->game_character->run_speed;
                }
            }
            if (RotDiff(radial, movement) < 0) {
                radial += 0xcaab;
            } else {
                radial += 0x3555;
            }
            object->velocity.x = NuTrigTable[radial >> 1] * speed;
            object->velocity.z = speed * NuTrigTable[((radial + 0x4000) >> 1) & 0x7fff];
        }
    }
    if (stopped) {
        spinner->room_index = -1;
        spinner->unknown_70 = GameAnimSet_GetCompletionRatio(spinner->anim_set);
        return;
    }
    i32 previous_state = spinner->state;
    f32 play_speed;
    if (spinner->spin_timer > 0.0f) {
        f32 remaining = (f64)spinner->spin_timer - FRAMETIME;
        spinner->spin_timer = remaining;
        if (remaining <= 0.0f) {
            spinner->rotation = spinner->target_rotation;
            GameAnimSet_Stop(spinner->anim_set);
            spinner->spin_timer = 0.0f;
        } else {
            f32 phase = 1.0 - (f64)remaining * 4.0;
            f32 blend = NuTrigTable[((i32)((f64)phase * 16384.0) >> 1) & 0x7fff];
            i32 difference = RotDiff(spinner->previous_rotation, spinner->target_rotation);
            if ((spinner->state_flags & SPINNER_STATE_TURNING_POSITIVE) != 0 && difference < 0) {
                difference += 0x10000;
            } else if ((spinner->state_flags & SPINNER_STATE_TURNING_POSITIVE) == 0 && difference > 0) {
                difference -= 0x10000;
            }
            spinner->rotation = (u16)(i32)((f64)difference * blend + spinner->previous_rotation);
        }
        goto animation_updated;
    }
    if (spinner->room_index != -1) {
        if ((spinner->room_index == 0x1e && (spinner->state_flags & SPINNER_STATE_REVERSED) == 0) ||
            (spinner->room_index != 0x1e && (spinner->state_flags & SPINNER_STATE_REVERSED) != 0)) {
            play_speed = spinner->anim_set->state == GAMEANIMSET_STATE_AT_END ? 0.0f : spinner->animation_speed;
        } else {
            play_speed = GameAnimSet_IsAnimationReset(spinner->anim_set) != 0 ? 0.0f : -spinner->animation_speed;
        }
        if (play_speed != 0.0f) {
            GameAnimSet_Play(spinner->anim_set, play_speed, 1);
        } else {
            GameAnimSet_Stop(spinner->anim_set);
        }
        goto animation_updated;
    }
    if ((spinner->flags & GIZSPINNER_FLAG_SPRING_BACK) == 0 ||
        ((spinner->state_flags & SPINNER_STATE_RELEASE_AT_END) != 0 && spinner->anim_set->state == GAMEANIMSET_STATE_AT_END) ||
        !(spinner->spin_timer <= 0.0f)) {
        GameAnimSet_Stop(spinner->anim_set);
        spinner->state_flags &= ~SPINNER_STATE_PUSHED;
        goto animation_updated;
    }
    if (spinner->spin_timer > -0.0f) { // never true here: the test above already ruled out > 0
        spinner->spin_timer -= FRAMETIME;
        GameAnimSet_Stop(spinner->anim_set);
        goto animation_updated;
    }
    if (GameAnimSet_IsAnimationReset(spinner->anim_set) != 0) {
        goto stop_animation;
    }
    play_speed = -spinner->animation_points[0];
    if ((spinner->state_flags & (SPINNER_STATE_POSITIVE | SPINNER_STATE_REVERSED)) != 0 &&
        (spinner->state_flags & (SPINNER_STATE_POSITIVE | SPINNER_STATE_REVERSED)) != (SPINNER_STATE_POSITIVE | SPINNER_STATE_REVERSED)) {
        spinner->rotation += (i32)(((f64)spinner->animation_points[0] / spinner->animation_speed) * ((f64)FRAMETIME * -5461.0));
    } else {
        spinner->rotation += (i32)(((f64)spinner->animation_points[0] / spinner->animation_speed) * ((f64)FRAMETIME * 5461.0));
    }
play_animation:
    if (play_speed != 0.0f) {
        GameAnimSet_Play(spinner->anim_set, play_speed, 1);
        goto animation_updated;
    }
stop_animation:
    GameAnimSet_Stop(spinner->anim_set);
animation_updated:
    NUMTX matrix;
    SpinnerMatrixSetRotationY(&matrix, spinner->rotation);
    if (initial_positive_offset != 0) {
        SpinnerMatrixRotateY(&matrix, 0x222);
    } else if (initial_negative_offset != 0) {
        SpinnerMatrixRotateY(&matrix, -0x222);
    }
    NuMtxTranslate(&matrix, &spinner->position);
    spinner->state = GameAnimSet_IsAnimationReset(spinner->anim_set) != 0 ? 0 : (spinner->anim_set->state == GAMEANIMSET_STATE_AT_END) + 1;
    if (initial_no_judder == 0 && previous_state == 1 && spinner->state != 1) {
        NewRumbleAllPlayers(0.0f, 0.1f, 0, 0);
        GameCam_Judder(GameCam, -0.2f, 0, NULL);
        GameAudio_PlaySfx(0x39, &spinner->position, 0, 0);
        spinner->state_flags |= SPINNER_STATE_JUDDERED;
    }
    spinner->room_index = -1;
    spinner->unknown_70 = GameAnimSet_GetCompletionRatio(spinner->anim_set);
}

extern "C" void GizSpinners_Update(void *world_pointer, void *unused, f32 unused_time) {
    WORLDINFO *world = (WORLDINFO *)world_pointer;
    (void)unused;
    (void)unused_time;
    if (world == NULL || world->spinners == NULL) {
        return;
    }
    for (i32 index = 0; index < world->current_level->max_spinners; ++index) {
        GIZSPINNER_s *spinner = &world->spinners[index];
        if ((spinner->flags & GIZSPINNER_FLAG_VALID) == 0) {
            continue;
        }
        u16 step = spinner->type != 0 ? (u16)(0x10000 / spinner->type) : 0;
        NUMTX matrix;
        NuMtxSetRotationY(&matrix, 0);
        NuMtxRotateY(&matrix, spinner->initial_rotation + spinner->rotation);
        NuMtxTranslate(&matrix, &spinner->position);
        spinner->matrix = matrix;
        i32 angle = 0;
        for (i32 arm = 0; arm < spinner->type; ++arm) {
            NuMtxSetRotationY(&matrix, 0);
            NuMtxRotateY(&matrix, spinner->initial_rotation + spinner->rotation);
            NuMtxRotateY(&matrix, angle);
            NuMtxTranslate(&matrix, &spinner->position);
            spinner->arms[arm].matrix = matrix;
            angle += step;
        }
        if (GameAnimSet_GetVisibility(spinner->anim_set) == 0) {
            spinner->flags |= GIZSPINNER_FLAG_INVISIBLE;
        } else if ((spinner->flags & GIZSPINNER_FLAG_INVISIBLE) != 0) {
            spinner->flags &= ~GIZSPINNER_FLAG_INVISIBLE;
        }
        if ((spinner->flags & (GIZSPINNER_FLAG_INACTIVE | GIZSPINNER_FLAG_INVISIBLE)) != 0) {
            spinner->state_flags = (spinner->state_flags & ~SPINNER_STATE_PUSHED) | SPINNER_STATE_HALTED;
            if ((spinner->flags & GIZSPINNER_FLAG_HIDDEN) == 0 && qrand() < 0x800) {
                NUVEC position = spinner->position;
                position.y = (f64)position.y + (f64)0.3f;
                if (GizSpinnerGDeb_Fail[0] != -1) {
                    AddVariableShotDebrisEffect(WORLD->game_debris_sys->entries[GizSpinnerGDeb_Fail[0]].effect, &position, 2, 0, 0);
                }
                if (GizSpinnerGDeb_Fail[2] != -1) {
                    AddVariableShotDebrisEffect(WORLD->game_debris_sys->entries[GizSpinnerGDeb_Fail[2]].effect, &position, 2, 0, 0);
                }
            }
        }
        if ((spinner->flags & GIZSPINNER_FLAG_SHADOW_MODE_ONLY) != 0 && ShadowMode == 0) {
            spinner->state_flags = (spinner->state_flags & ~SPINNER_STATE_PUSHED) | SPINNER_STATE_HALTED;
            if (SpinnerOwner(spinner) != NULL) {
                Player[index]->player.character_context = CHARACTER_CONTEXT_NONE;
                Player[index]->player.grapple = NULL;
            }
        }
        if (spinner->output_pause_time > 0.0f) {
            spinner->output_pause_time -= FRAMETIME;
            spinner->state_flags = (spinner->state_flags & ~SPINNER_STATE_PUSHED) | SPINNER_STATE_HALTED;
            if (spinner->output_pause_time <= 0.0f) {
                spinner->output_pause_time = 0.0f;
                spinner->state_flags &= ~SPINNER_STATE_HALTED;
            }
        }
        GizSpinner_Update(spinner);
        i32 output = GizSpinner_WithinOutput(spinner);
        if ((spinner->flags & 0x40) == 0 && output != -1 && spinner->unknown_2d8 > 0.0f) {
            spinner->output_pause_time = spinner->unknown_2d8;
            spinner->flags |= 0x40;
            GAMEOBJECT *owner = SpinnerOwner(spinner);
            if (owner != NULL) {
                NewRumble(owner->player.pad->bound_pad, 0.8f, 0);
            }
        }
        if (output != -1) {
            if (spinner->spin_timer == 0.0f &&
                (spinner->flags & (GIZSPINNER_FLAG_INACTIVE | GIZSPINNER_FLAG_SPRING_BACK | GIZSPINNER_FLAG_INVISIBLE)) == 0 &&
                (spinner->state_flags & SPINNER_STATE_PUSHABLE) != 0 &&
                ((spinner->state_flags & SPINNER_STATE_RELEASE_AT_END) == 0 || spinner->anim_set->state != GAMEANIMSET_STATE_AT_END)) {
                SpinnerSeekOutput(spinner, output);
            }
        } else {
            spinner->flags &= ~0x40u;
        }
    }
}

extern "C" f32 GizSpinner_GetNearestTargetPoint(GIZSPINNER_s *spinner, NUVEC *origin, NUVEC *position, NUVEC *direction,
                                                i32 check_direction) {
    NUVEC positions[8];
    NUVEC directions[8];
    f32 nearest_distance = 1000000000.0f;
    NUVEC *nearest_position = NULL;
    NUVEC *nearest_direction = NULL;
    i32 in_front = 0;
    i32 count = GizSpinner_GetTargetPoints(spinner, positions, directions);
    if (count == 0 || origin == NULL) {
        return -1.0f;
    }
    for (i32 i = 0; i < count; ++i) {
        f32 distance = NuVecDistSqr(origin, &positions[i], NULL);
        if (check_direction != 0) {
            u16 angle = spinner->unknown_08c - (0x10000 / spinner->type) * i - spinner->rotation - spinner->initial_rotation + 0x4000;
            NUVEC offset;
            in_front = 0;
            NuVecSub(&offset, origin, &spinner->position);
            NuVecRotateY(&offset, &offset, angle);
            if (offset.z >= 0.0f) {
                in_front = 1;
            }
        }
        if (distance < nearest_distance) {
            if (check_direction != 0) {
                if (in_front != 0) {
                    nearest_distance = distance;
                    nearest_position = &positions[i];
                    nearest_direction = &directions[i];
                }
            } else {
                nearest_distance = distance;
                nearest_position = &positions[i];
                nearest_direction = &directions[i];
            }
        }
    }
    if (nearest_position == NULL) {
        return -1.0f;
    }
    *position = *nearest_position;
    *direction = *nearest_direction;
    return nearest_distance;
}

extern "C" void GizSpinner_ResetAll(WORLDINFO *world, void *unused, SpinnerProgressEntry *progress) {
    i32 index;
    if (world == NULL || world->spinners == NULL || progress == NULL) {
        return;
    }
    for (index = 0; index < world->current_level->max_spinners; ++index) {
        if ((world->spinners[index].flags & GIZSPINNER_FLAG_VALID) != 0) {
            SpinnerResetEntry(&world->spinners[index], &progress[index]);
        }
    }
    GizSpinners_Update(world, NULL, FRAMETIME);
}

extern "C" i32 GizSpinner_HasBoltHit(void *, void *, void *, NUVEC *, i32, f32, NUVEC *, NUVEC *, BOLT *, u32, u8 *) {
    return 0;
}

extern "C" ADDGIZMOTYPE *Spinner_RegisterGizmo(i32 type_id) {
    ADDGIZMOTYPE *addtype = &GizmoRegistrySpinnerAddType;
    *addtype = Default_ADDGIZMOTYPE;
    addtype->fns.late_update_fn = NULL;
    addtype->fns.activate_rev_fn = NULL;
    addtype->fns.get_visibility_fn = NULL;
    addtype->fns.post_load_fn = NULL;
    addtype->fns.add_level_sfx_fn = NULL;
    spinner_gizmotype_id = type_id;
    addtype->name = "Spinner";
    addtype->prefix = "";
    addtype->fns.options = 0x40;
    addtype->fns.get_max_gizmos_fn = (i32(*)(void *))GizSpinner_GetMaxGizmos;
    addtype->fns.add_gizmos_fn = (void *)GizSpinner_AddGizmos;
    addtype->fns.early_update_fn = (void (*)(void *, void *, f32))GizSpinners_Update;
    addtype->fns.draw_fn = (void (*)(void *, void *, f32))GizSpinner_Draw;
    addtype->fns.panel_draw_fn = (void (*)(void *, void *, f32))GizSpinner_PanelDraw;
    addtype->fns.get_gizmo_name_fn = (char *(*)(GIZMO *))GizSpinner_GetGizmoName;
    addtype->fns.get_output_fn = (i32(*)(GIZMO *, i32, i32))GizSpinner_GetOutput;
    addtype->fns.get_output_name_fn = (char *(*)(GIZMO *, i32))GizSpinner_GetOutputName;
    addtype->fns.get_num_outputs_fn = (void *)GizSpinner_GetNumOutputs;
    addtype->fns.activate_fn = (void (*)(GIZMO *, i32))GizSpinner_Activate;
    addtype->fns.set_visibility_fn = (void (*)(GIZMO *, i32))GizSpinner_SetVisibility;
    addtype->fns.get_pos_fn = (void *)GizSpinner_GetPos;
    addtype->fns.using_special_fn = (void *)GizSpinner_UsingSpecial;
    addtype->fns.bolt_hit_plat_fn = (GIZMO_BOLT_HIT_PLAT_FN)GizSpinner_BoltHitPlatAll;
    addtype->fns.get_best_bolt_target_fn =
        (GIZMO * (*)(GIZMOSET *, f32 *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, f32, f32, i32, i32, i32)) GizSpinner_GetBestBoltTarget;
    addtype->fns.bolt_hit_fn = (GIZMO_BOLT_HIT_FN)GizSpinner_HasBoltHit;
    addtype->fns.allocate_progress_data_fn = (void *(*)(VARIPTR *, VARIPTR *))GizSpinner_AllocateProgressData;
    addtype->fns.clear_progress_fn = (void (*)(void *, void *))GizSpinner_ClearProgress;
    addtype->fns.store_progress_fn = (void (*)(void *, void *, void *))GizSpinner_StoreProgressData;
    addtype->fns.reset_fn = (void (*)(void *, void *, void *))GizSpinner_ResetAll;
    addtype->fns.reserve_buffer_space_fn = (void *(*)(void *))GizSpinner_ReserveBufferSpace;
    addtype->fns.load_fn = (i32(*)(void *, void *))GizSpinner_Load;
    return addtype;
}
