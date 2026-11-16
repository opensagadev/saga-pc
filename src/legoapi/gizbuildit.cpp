#include "gameapi/edtools/edfile.h"
#include "gamelib/util/Utilities.h"
#include "globals.h"
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/playeritems.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion.h"
#include "legoapi/characters/motion/animation_ids.h"
#include "legoapi/characters/motion/contexts.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/qrand.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmo/object/gizbuildit.h"
#include "legoapi/gizmo/object/gizmoblowups.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmos/object/gizbuildits.h"
#include "legoapi/items/base/animpacket.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/parts.h"
#include "legoapi/render/light/shadow.h"
#include "legoapi/startup_dats.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/nuportal.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuquat.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"

typedef struct buildit_reset_progress_s { // size 0x60
    u32 completed[2];                     // 0x00
    u32 active[2];                        // 0x08
    u32 visible[2];                       // 0x10
    u32 reward_released[2];               // 0x18
    u8 built_object_count[64];            // 0x20
} BUILDIT_RESET_PROGRESS;

extern "C" {

    /* The SDK's inline NuFabs (nufloat.h): a masked copy of the float bits. */
    static f32 GizbuilditFabs(f32 f) {
        f32 rv;

        *(i32 *)&rv = *(i32 *)&f & 0x7fffffff;
        return rv;
    }

    static f32 BuildItSafeRatio(f32 numerator, f32 denominator) {
        if (numerator != 0.0f && denominator != 0.0f) {
            f32 ratio = numerator / denominator;
            return ratio;
        }
        return 0.0f;
    }
}

extern "C" void *GizBuildIt_Find(WORLDINFO *world, char *name) {
    GIZBUILDITSYS_s *system = world->giz_buildit_sys;
    if (system != NULL) {
        GIZBUILDIT *buildit = system->buildits;
        for (i32 i = 0; i < system->count; ++i, ++buildit) {
            if (NuStrICmp(buildit->name, name) == 0) {
                return buildit;
            }
        }
    }
    return NULL;
}

extern "C" {

    static GAMEOBJECT *BuildItFindActiveBuilder(GIZBUILDIT *buildit) {
        i32 index;
        if (LEGOCONTEXT_BUILDIT == -1) {
            return NULL;
        }
        for (index = 0; index < 8; ++index) {
            GAMEOBJECT *player = Player[index];
            if (player != NULL && (player->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 &&
                player->player.character_context == LEGOCONTEXT_BUILDIT && player->player.grapple == buildit) {
                return player;
            }
        }
        return NULL;
    }

    static f32 BuildItAxisWobbleFactor(NUVEC *axis) {
        NUVEC normal;
        f32 dot;
        f32 vertical;
        f32 product;
        f32 factor;
        f32 sine;
        NuVecNorm(&normal, axis);
        dot = NuVecDot(&normal, &v010);
        vertical = GizbuilditFabs(dot);
        sine = NU_SIN_LUT((f64)vertical * 16384.0 + 16384.0);
        product = (1.0f - sine) * vertical;
        factor = 1.0f - product;
        return factor;
    }
}

extern "C" GIZBUILDIT *GizBuildIt_FindNearest(WORLDINFO *world, GAMEOBJECT *player, i32 mode, i32 shadow) {
    if (world->giz_buildit_sys == NULL || player == NULL) {
        return NULL;
    }
    NUVEC position = player->collision_position;
    f32 lower_y = player->lower_position.y;
    f32 upper_y = player->upper_position.y;
    f32 nearest_distance = 1000000.0f;
    GIZBUILDIT *nearest = NULL;
    GIZBUILDIT *buildit = world->giz_buildit_sys->buildits;
    for (i32 i = 0; i < world->giz_buildit_sys->count; i++, buildit++) {
        if (mode != 2) {
            if ((buildit->availability_flags & 2) == 0 || (buildit->availability_flags & 1) == 0) {
                continue;
            }
            if ((buildit->state_flags & 0x80) != 0 && shadow == 0) {
                continue;
            }
            if (mode == 1) {
                if (buildit->build_state != 0 || LEGOCONTEXT_BUILDIT == -1) {
                    continue;
                }
                i32 p;
                for (p = 0; p < 8; p++) {
                    GAMEOBJECT *object = Player[p];
                    if (object != NULL && object->player.character_context == LEGOCONTEXT_BUILDIT &&
                        object->player.grapple == (void *)buildit) {
                        break;
                    }
                }
                if (p < 8) {
                    continue;
                }
            }
        }
        f32 height = buildit->progress / 100.0 * buildit->bounds_radius;
        if (upper_y < buildit->start_position.y - height || lower_y > buildit->start_position.y + height) {
            continue;
        }
        f32 radius = buildit->bounds_radius + 0.30000001192092896;
        f32 distance = NuVecXZDistSqr(&buildit->start_position, &position, NULL);
        radius = radius * radius;
        if (distance < radius && distance < nearest_distance) {
            nearest_distance = distance;
            nearest = buildit;
        }
    }
    return nearest;
}

extern "C" {

    GIZBUILDIT *GizBuildIt_AnyReacting(WORLDINFO *world) {
        GIZBUILDITSYS_s *system = world->giz_buildit_sys;
        if (system != NULL) {
            GIZBUILDIT *buildit = system->buildits;
            i32 count = system->count;
            i32 index;
            for (index = 0; index < count; ++index, ++buildit) {
                u16 flags = buildit->availability_flags;
                if ((flags & 2) != 0 && (flags & 4) != 0 && buildit->builders_active == 0 && (flags & 1) != 0) {
                    return buildit;
                }
            }
        }
        return NULL;
    }

    i32 fun_0058ecd0(GIZBUILDIT *buildit) {
        if (buildit != NULL && buildit->built_object_count == 0 && buildit->builders_active == 0) {
            return 1;
        }
        return 0;
    }

    i32 GizBuildIt_AtEnd(void *buildit_pointer) {
        GIZBUILDIT *buildit = (GIZBUILDIT *)buildit_pointer;
        if (buildit != NULL && buildit->build_state == 2) {
            return 1;
        }
        return 0;
    }

    void fun_0058ed10(GIZBUILDIT *buildit) {
        if (buildit != NULL) {
            if (buildit->linked_buildit == NULL) {
                GameAnimSet_SetVisibility(buildit->anim_set, 0);
            }
            buildit->availability_flags &= ~2;
        }
    }
}

extern "C" void GizBuildIt_SetHeadTarget(GIZBUILDIT *buildit, GAMEOBJECT *player) {
    if (buildit->anim_object_count != 0) {
        i32 index = buildit->built_object_count;
        if (index >= buildit->anim_object_count) {
            index = buildit->anim_object_count - 1;
        }
        GIZBUILDITANIMDATA_s *data = buildit->anim_objects[index]->buildit_data;
        SetHeadTarget(player, reinterpret_cast<NUVEC *>(&data->start_mtx.m30), 0, 2.0f, 1.0f, 2.0f);
    }
}

extern "C" void GizMoveAttractoBuildItPiece(GIZBUILDIT *buildit, GAMEANIMOBJ *piece) {
    GIZBUILDITANIMDATA_s *data = piece->buildit_data;
    f32 progress = 1.0f - buildit->step_timer / buildit->step_duration;
    NUMTX *draw = NuSpecialGetDrawMtx(&piece->special);
    NUMTX result;
    NUMTX from;
    NUMTX to;
    NUQUAT rotation;
    NUQUAT from_rotation;
    NUQUAT to_rotation;
    NUVEC from_position;
    NUVEC to_position;
    if (progress > 0.0f) {
        from = *draw;
        from.m30 = from.m31 = from.m32 = 0.0f;
        to = data->end_mtx;
        to.m30 = to.m31 = to.m32 = 0.0f;
        NuMtxToQuat(&from, &from_rotation);
        NuMtxToQuat(&to, &to_rotation);
        NuQuatSlerp(&rotation, &from_rotation, &to_rotation, progress);
        NuQuatToMtx(&rotation, &result);
        from_position = *(NUVEC *)&draw->m30;
        to_position = *(NUVEC *)&data->end_mtx.m30;
        result.m30 = (to_position.x - from_position.x) * progress + from_position.x;
        result.m31 = (to_position.y - from_position.y) * progress + from_position.y + NU_SIN_LUT(progress * 32768.0) * 0.1f;
        result.m32 = (to_position.z - from_position.z) * progress + from_position.z;
        *draw = result;
    }
    if (buildit->linked_buildit != NULL) {
        data->draw_mtx = *draw;
    } else {
        NuSpecialSetDrawMtx(&piece->special, draw);
    }
}

extern "C" void GizDrawBuildItPiece(GAMEOBJECT *object, i32 draw_reflection) {
    GIZBUILDIT *buildit = static_cast<GIZBUILDIT *>(object->player.grapple);
    GAMEANIMOBJ *piece;
    GIZBUILDITANIMDATA_s *data;
    f32 progress;
    NUMTX matrix;
    NUMTX from;
    NUMTX to;
    NUMTX result;
    NUMTX reflected;
    NUQUAT from_rotation;
    NUQUAT to_rotation;
    NUQUAT rotation;
    NUVEC to_position;
    if (buildit == NULL || buildit->builders_active == 0 || buildit->build_state != 0 || buildit->anim_object_count == 0) {
        return;
    }
    progress = 1.0f - buildit->step_timer / buildit->step_duration;
    piece = buildit->anim_objects[buildit->built_object_count];
    data = piece->buildit_data;
    matrix = data->start_mtx;
    if (progress > 0.0f) {
        from = matrix;
        from.m30 = from.m31 = from.m32 = 0.0f;
        to = data->end_mtx;
        to.m30 = to.m31 = to.m32 = 0.0f;
        NuMtxToQuat(&from, &from_rotation);
        NuMtxToQuat(&to, &to_rotation);
        NuQuatSlerp(&rotation, &from_rotation, &to_rotation, progress);
        NuQuatToMtx(&rotation, &result);
        to_position = *(NUVEC *)&data->end_mtx.m30;
        result.m30 = (to_position.x - matrix.m30) * progress + matrix.m30;
        result.m31 = (to_position.y - matrix.m31) * progress + matrix.m31 + NU_SIN_LUT(progress * 32768.0) * 0.1f;
        result.m32 = (to_position.z - matrix.m32) * progress + matrix.m32;
        matrix = result;
    }
    if (buildit->linked_buildit != NULL) {
        data->draw_mtx = matrix;
        return;
    }
    NuSpecialDrawAt(&piece->special, &matrix);
    if (draw_reflection != 0) {
        if (MatrixReflection(&matrix, object->reflection_enabled, object->reflection_height, WORLD->current_level->reflection_y,
                             &reflected) != 0) {
            NuRndrStartReflectionRender(0);
            NuSpecialDrawAt(&piece->special, &reflected);
            NuRndrEndReflectionRender();
        }
    }
}

extern "C" {

    void GizGetBuildItPlayerPos(GAMEOBJECT *player, NUVEC *position, NUVEC *target) {
        GIZBUILDITANIMDATA_s *data;
        i32 count;
        i32 index;
        i32 batch_count;
        i32 end;
        NUVEC total;
        NUVEC *end_position;
        f32 dx;
        f32 dz;
        f32 inverse_distance;
        if (player->player.buildit == NULL || player->player.buildit->anim_set == NULL || player->player.buildit->anim_object_count == 0) {
            return;
        }
        count = player->player.buildit->anim_object_count;
        index = player->player.buildit->built_object_count;
        if (index >= count) {
            index = count - 1;
        }
        if (position != NULL) {
            index = (index / player->player.buildit->batch_size) * player->player.buildit->batch_size;
            batch_count = 0;
            end = index;
            while (batch_count < player->player.buildit->batch_size && end < count) {
                ++batch_count;
                ++end;
            }
            total = v000;
            for (; index < end; ++index) {
                data = player->player.buildit->anim_objects[index]->buildit_data;
                NuVecAdd(&total, &total, (NUVEC *)&data->start_mtx.m30);
            }
            if (batch_count > 1) {
                NuVecScale(position, &total, 1.0f / batch_count);
            } else {
                *position = total;
            }
            position->x = (player->player.buildit->start_position.x + position->x) * 0.5;
            position->z = (player->player.buildit->start_position.z + position->z) * 0.5;
            data = player->player.buildit->anim_objects[player->player.buildit->built_object_count]->buildit_data;
            end_position = (NUVEC *)&data->end_mtx.m30;
            dx = position->x - end_position->x;
            dz = position->z - end_position->z;
            inverse_distance = 1.0f / NuFsqrt(dx * dx + dz * dz);
            position->x += inverse_distance * dx * player->bounds_radius;
            position->z += inverse_distance * dz * player->bounds_radius;
        }
        if (target != NULL) {
            data = player->player.buildit->anim_objects[index]->buildit_data;
            *target = *(NUVEC *)&data->end_mtx.m30;
        }
    }

    void GizBuildItPushAwayFromStart(GAMEOBJECT *player, void *context) {
        GIZBUILDIT *buildit = (GIZBUILDIT *)context;
        GIZBUILDITANIMDATA_s *data;
        GAMEANIMOBJ *object;
        NUVEC maximum = v000;
        NUVEC minimum = v000;
        NUVEC start_position;
        NUVEC centre;
        NUVEC radius_centre;
        NUVEC player_position;
        NUVEC position;
        f32 radius;
        f32 nearest_distance;
        f32 dx;
        f32 dz;
        f32 distance;
        i32 nearest = 0;
        i32 i;
        if (player == NULL || buildit == NULL || buildit->anim_set == NULL || buildit->anim_object_count == 0) {
            return;
        }
        minimum.x = 1.0e9f;
        minimum.z = 1.0e9f;
        player_position = player->collision_position;
        maximum.x = -1.0e9f;
        maximum.z = -1.0e9f;
        start_position = buildit->start_position;
        if (buildit->state_flags & 0x10) {
            nearest_distance = 1.0e9f;
            for (i = 0; i < buildit->anim_object_count; ++i) {
                data = buildit->anim_objects[i]->buildit_data;
                position = *(NUVEC *)&data->start_mtx.m30;
                dx = position.x - player_position.x;
                dz = position.z - player_position.z;
                distance = dz * dz + dx * dx;
                if (distance < nearest_distance) {
                    nearest_distance = distance;
                    centre = position;
                    nearest = i;
                }
            }
            object = buildit->anim_objects[nearest];
            NuSpecialGetBounds(&object->special, &minimum, &maximum);
            NuSpecialGetRadius(&object->special, &radius_centre, &radius);
        } else {
            centre = start_position;
            for (i = 0; i < buildit->anim_object_count; ++i) {
                data = buildit->anim_objects[i]->buildit_data;
                position = *(NUVEC *)&data->start_mtx.m30;
                dx = position.x - start_position.x;
                dz = position.z - start_position.z;
                if (!(maximum.x > dx)) {
                    maximum.x = dx;
                }
                if (!(maximum.z > dz)) {
                    maximum.z = dz;
                }
                if (!(minimum.x < dx)) {
                    minimum.x = dx;
                }
                if (!(minimum.z < dz)) {
                    minimum.z = dz;
                }
            }
            radius = NuFsqrt(maximum.z * maximum.z + maximum.x * maximum.x);
        }
        maximum.x += centre.x;
        maximum.z += centre.z;
        minimum.x += centre.x;
        minimum.z += centre.z;
        PushAway(&centre, radius, &minimum, &maximum, player, NULL, 1.0f, 0);
    }

    void GizBuildItPushAwayFromEnd(GAMEOBJECT *player) {
        GIZBUILDIT *buildit;
        GIZBUILDITANIMDATA_s *data;
        NUVEC centre;
        NUVEC end_position;
        NUVEC minimum;
        NUVEC maximum;
        NUVEC radius_centre;
        NUVEC player_position;
        NUVEC position;
        GAMEANIMOBJ *object;
        f32 radius;
        f32 radius_squared = -1.0e9f;
        f32 nearest_distance;
        f32 dx;
        f32 dz;
        f32 distance;
        i32 i;
        i32 j;
        i32 nearest = 0;
        if (WORLD->giz_buildit_sys == NULL) {
            return;
        }
        player_position = player->collision_position;
        buildit = WORLD->giz_buildit_sys->buildits;
        for (i = 0; i < WORLD->giz_buildit_sys->count; ++i, ++buildit) {
            centre = v000;
            if ((buildit->state_flags & 8) == 0 || (buildit->availability_flags & 2) == 0 || buildit->built_object_count <= 0) {
                continue;
            }
            end_position = buildit->position;
            minimum.x = 1.0e9f;
            minimum.z = 1.0e9f;
            maximum.x = -1.0e9f;
            maximum.z = -1.0e9f;
            if ((buildit->state_flags & 0x10) != 0) {
                nearest_distance = 1.0e9f;
                for (j = 0; j < buildit->built_object_count; ++j) {
                    data = buildit->anim_objects[j]->buildit_data;
                    position = *(NUVEC *)&data->end_mtx.m30;
                    dx = position.x - player_position.x;
                    dz = position.z - player_position.z;
                    distance = dz * dz + dx * dx;
                    if (distance < nearest_distance) {
                        nearest_distance = distance;
                        centre = position;
                        nearest = j;
                    }
                }
                object = buildit->anim_objects[nearest];
                NuSpecialGetBounds(&object->special, &minimum, &maximum);
                NuSpecialGetRadius(&object->special, &radius_centre, &radius);
            } else {
                centre.y = end_position.y;
                for (j = 0; j < buildit->built_object_count; ++j) {
                    data = buildit->anim_objects[j]->buildit_data;
                    position = *(NUVEC *)&data->end_mtx.m30;
                    centre.x += position.x;
                    centre.z += position.z;
                    dx = position.x - end_position.x;
                    dz = position.z - end_position.z;
                    if (!(maximum.x > dx)) {
                        maximum.x = dx;
                    }
                    if (!(maximum.z > dz)) {
                        maximum.z = dz;
                    }
                    if (!(minimum.x < dx)) {
                        minimum.x = dx;
                    }
                    if (!(minimum.z < dz)) {
                        minimum.z = dz;
                    }
                    distance = dx * dx + dz * dz;
                    if (distance > radius_squared) {
                        radius_squared = distance;
                    }
                }
                centre.x /= buildit->built_object_count;
                centre.z /= buildit->built_object_count;
                radius = NuFsqrt(radius_squared);
            }
            maximum.x += centre.x;
            maximum.y = centre.y + 0.01f;
            maximum.z += centre.z;
            minimum.x += centre.x;
            minimum.y = centre.y;
            minimum.z += centre.z;
            PushAway(&centre, radius, &minimum, &maximum, player, NULL, 1.0f, 0);
        }
    }

    f32 GizBuildItMul(GAMEOBJECT *player) {
        GIZBUILDIT *buildit;
        i32 amount;
        i32 completed;
        i32 total;
        if ((player->flags_1fc_word & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && Player_HasFastBuild(player)) {
            return 3.0f;
        }
        buildit = player->player.buildit;
        completed = buildit->built_object_count - player->player.big_jump_animation;
        amount = player->player.big_jump_animation;
        total = buildit->anim_object_count;
        if (completed + amount >= (total + completed) / 2) {
            amount = total - completed - amount;
        }
        if (amount > 10) {
            amount = 10;
        } else if (amount < 0) {
            amount = 0;
        }
        return (f32)((f64)amount / 10.0 + 1.0);
    }

    void ReleaseBuildIt(GAMEOBJECT *player, i32 completed) {
        f32 completion_blend;
        if (LEGOCONTEXT_BUILDIT == -1 || player->player.character_context != LEGOCONTEXT_BUILDIT) {
            return;
        }
        player->player.character_context = CHARACTER_CONTEXT_NONE;
        completion_blend = completed != 0 ? 0.6f : 0.0f;
        GameCam_Blend(GameCam, 0.5f, completion_blend, 1);
    }

    void GizBuildIts_PostLoad(void *world, void *pointer) {
        GIZBUILDITSYS_s *system = (GIZBUILDITSYS_s *)pointer;
        GIZBUILDIT *buildit;
        i32 i;
        if (system == NULL) {
            return;
        }
        buildit = system->buildits;
        for (i = 0; i < system->count; ++i, ++buildit) {
            if (buildit->availability_flags & 0x80) {
                i32 type_id = GizmoBlowupGetTypeFromNameTableId((WORLDINFO *)world, buildit->blowup_type);
                buildit->availability_flags &= ~0x80;
                buildit->blowup_type = (i16)type_id;
            }
        }
    }

    void *GizBuildIts_ReserveBufferSpace(void *pointer) {
        WORLDINFO *world = (WORLDINFO *)pointer;
        GIZBUILDITSYS_s *system;
        i32 i;
        system = (GIZBUILDITSYS_s *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(*system));
        system->capacity = world->current_level->max_buildits;
        system->max_animation_objects = (u16)world->current_level->max_buildit_objects;
        system->buildits =
            (GIZBUILDIT *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, system->capacity * sizeof(*system->buildits));
        system->anim_pool = GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 200, system->max_animation_objects);
        system->anim_objects =
            (GAMEANIMOBJ **)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, system->max_animation_objects * sizeof(void *));
        for (i = 0; i < system->capacity; ++i) {
            system->buildits[i].anim_set =
                GameAnimSet_Create(&world->giz_buffer, &world->buffer_end, system->anim_pool, world->game_anim_sys);
        }
        world->giz_buildit_sys = system;
        return system;
    }

    void GizBuildit_SetVisibility(GIZBUILDIT *buildit, i32 visible) {
        if (buildit != NULL) {
            GameAnimSet_SetVisibility(buildit->anim_set, visible);
            buildit->availability.visible = visible != 0;
        }
    }

    void GizmoBuildit_SetVisibility(GIZMO *gizmo, i32 visible) {
        GIZBUILDIT *buildit;
        if (gizmo == NULL) {
            return;
        }
        buildit = gizmo->buildit;
        if (buildit == NULL) {
            return;
        }
        GameAnimSet_SetVisibility(buildit->anim_set, visible);
        buildit->availability.visible = visible != 0;
    }

    NUVEC *GizmoBuildit_GetPos(GIZMO *gizmo) {
        if (gizmo == NULL || gizmo->object == NULL) {
            return NULL;
        }
        return &gizmo->buildit->position;
    }

    void GizBuildit_Reset(GIZBUILDIT *buildit, void *world_ptr) {
        buildit->availability_flags = (u16)((buildit->availability_flags & ~0x200u) | 3u);

        buildit->start_position = buildit->position = buildit->file_position;
        buildit->bounds_radius = NuTrigTable[0x3000];

        if (buildit->anim_set != NULL && buildit->anim_set->objects != NULL) {
            struct gameanimobj_s *object;
            f32 span;
            f32 span_z;
            NUVEC start_max;
            NUVEC start_min;
            NUVEC end_min;
            NUVEC end_max;
            start_min.x = end_min.x = 1000000000.0f;
            start_max.x = end_max.x = -1000000000.0f;
            start_max.y = end_max.y = -1000000000.0f;
            start_max.z = end_max.z = -1000000000.0f;
            start_min.y = end_min.y = 1000000000.0f;
            start_min.z = end_min.z = 1000000000.0f;

            for (object = buildit->anim_set->objects; object != NULL; object = object->next) {
                NUMTX evaluated;
                GIZBUILDITANIMDATA_s *data = object->buildit_data;
                memset(data, 0, sizeof(*data));

                EvalAnim(&object->special, object->start_frame, &evaluated, 1);
                data->start_mtx = evaluated;
                if (buildit->linked_buildit != NULL) {
                    data->start_mtx.m30 -= buildit->linked_buildit->file_position.x;
                    data->start_mtx.m31 -= buildit->linked_buildit->file_position.y;
                    data->start_mtx.m32 -= buildit->linked_buildit->file_position.z;
                    NuMtxRotateY(&data->start_mtx, buildit->linked_rotation);
                    data->start_mtx.m30 = buildit->file_position.x + data->start_mtx.m30;
                    data->start_mtx.m31 = buildit->file_position.y + data->start_mtx.m31;
                    data->start_mtx.m32 = buildit->file_position.z + data->start_mtx.m32;
                    data->draw_mtx = data->start_mtx;
                }

                EvalAnim(&object->special, object->end_frame, &evaluated, 1);
                data->end_mtx = evaluated;
                if (buildit->linked_buildit != NULL) {
                    data->end_mtx.m30 -= buildit->linked_buildit->file_position.x;
                    data->end_mtx.m31 -= buildit->linked_buildit->file_position.y;
                    data->end_mtx.m32 -= buildit->linked_buildit->file_position.z;
                    NuMtxRotateY(&data->end_mtx, buildit->linked_rotation);
                    data->end_mtx.m30 = buildit->file_position.x + data->end_mtx.m30;
                    data->end_mtx.m31 = buildit->file_position.y + data->end_mtx.m31;
                    data->end_mtx.m32 = buildit->file_position.z + data->end_mtx.m32;
                }

                if (data->start_mtx.m30 < start_min.x) {
                    start_min.x = data->start_mtx.m30;
                }
                if (data->start_mtx.m30 > start_max.x) {
                    start_max.x = data->start_mtx.m30;
                }
                if (data->start_mtx.m31 < start_min.y) {
                    start_min.y = data->start_mtx.m31;
                }
                if (data->start_mtx.m31 > start_max.y) {
                    start_max.y = data->start_mtx.m31;
                }
                if (data->start_mtx.m32 < start_min.z) {
                    start_min.z = data->start_mtx.m32;
                }
                if (data->start_mtx.m32 > start_max.z) {
                    start_max.z = data->start_mtx.m32;
                }

                if (data->end_mtx.m30 < end_min.x) {
                    end_min.x = data->end_mtx.m30;
                }
                if (data->end_mtx.m30 > end_max.x) {
                    end_max.x = data->end_mtx.m30;
                }
                if (data->end_mtx.m31 < end_min.y) {
                    end_min.y = data->end_mtx.m31;
                }
                if (data->end_mtx.m31 > end_max.y) {
                    end_max.y = data->end_mtx.m31;
                }
                if (data->end_mtx.m32 < end_min.z) {
                    end_min.z = data->end_mtx.m32;
                }
                if (data->end_mtx.m32 > end_max.z) {
                    end_max.z = data->end_mtx.m32;
                }

                data->wobble_time = 0.2f;
            }

            buildit->start_position.x = (f32)((f64)(start_min.x + start_max.x) * 0.5);
            buildit->start_position.y = (f32)((f64)(start_min.y + start_max.y) * 0.5);
            buildit->start_position.z = (f32)((f64)(start_min.z + start_max.z) * 0.5);
            span = start_max.x - start_min.x;
            span_z = start_max.z - start_min.z;
            if (span > span_z) {
                buildit->bounds_radius = span;
            } else {
                buildit->bounds_radius = span_z;
            }
            buildit->bounds_radius *= NuTrigTable[0x3000];
            buildit->position.x = (f32)((f64)(end_min.x + end_max.x) * 0.5);
            buildit->position.y = (f32)((f64)(end_min.y + end_max.y) * 0.5);
            buildit->position.z = (f32)((f64)(end_min.z + end_max.z) * 0.5);
        }

        if (buildit->wobble_parameter != 0) {
            buildit->state_flags |= GIZBUILDIT_STATE_ROTATING_WOBBLE;
        } else {
            buildit->state_flags &= (u16)(~GIZBUILDIT_STATE_ROTATING_WOBBLE);
        }
        buildit->build_state = GIZBUILDIT_BUILD_IDLE;
        buildit->built_object_count = 0;

        if (world_ptr != NULL) {
            WORLDINFO *world = (WORLDINFO *)(world_ptr);
            buildit->room_index = world->current_gscn != NULL ? NuPortalWhichRoom(world->current_gscn, &buildit->file_position) : -1;
        }
    }

    void GizBuildIts_Reset(void *world, void *system_pointer, void *progress_pointer) {
        GIZBUILDITSYS_s *system = (GIZBUILDITSYS_s *)system_pointer;
        BUILDIT_RESET_PROGRESS *progress = (BUILDIT_RESET_PROGRESS *)progress_pointer;
        GIZBUILDIT *buildit = system->buildits;
        i32 index;
        for (index = 0; index < system->count; ++index, ++buildit) {
            GizBuildit_Reset(buildit, world);
            if (progress != NULL && index < 64) {
                i32 word = index / 32;
                u32 bit = 1u << (index & 31);
                buildit->build_state = (progress->completed[word] & bit) != 0 ? 2 : 0;
                buildit->availability.visible = (progress->visible[word] & bit) != 0;
                buildit->availability.active = (progress->active[word] & bit) != 0;
                buildit->availability.reward_released = (progress->reward_released[word] & bit) != 0;
                buildit->built_object_count = progress->built_object_count[index];
            }
        }
    }

    void GizBuildIts_EarlyUpdate(void *world, void *pointer, f32 delta_time) {
        GIZBUILDITSYS_s *system = (GIZBUILDITSYS_s *)pointer;
        GIZBUILDIT *buildit;
        i32 i;
        if (system == NULL) {
            return;
        }
        buildit = system->buildits;
        for (i = 0; i < system->count; ++i, ++buildit) {
            buildit->availability_flags &= ~4;
            buildit->builders_active = 0;
        }
    }

    i32 GizBuildIts_UpdateHint(HINT *hint) {
        if (WORLD->area != NULL && WORLD->area == HUB_ADATA) {
            return 0;
        }
        if (hint->completed == 0 && GizBuildIt_AnyReacting(WorldInfo_CurrentlyActive()) != NULL) {
            return 1;
        }
        return 0;
    }

    void GizBuildIt_KillParts(GIZBUILDIT *buildit) {
        NUVEC velocity;
        struct gameanimobj_s *object;
        if (buildit == NULL) {
            return;
        }
        velocity = v000;
        if (buildit->anim_set == NULL) {
            return;
        }
        object = buildit->anim_set->objects;
        while (object != NULL) {
            struct PART *part;
            ADDPART params = Default_ADDPART;
            NUMTX matrix;
            GIZBUILDITANIMDATA_s *data = object->buildit_data;
            matrix = data->end_mtx;
            params.matrix = &matrix;
            params.velocity = &velocity;
            params.special = &object->special;
            params.start_radius = 0.2f;
            params.end_radius = 0.2f;
            params.gravity = -4.0f;
            params.flags = PART_FLAG_KEEP_OFF_SCREEN | PART_FLAG_ROTATE_RANDOMLY;
            params.flicker_flags = 1;
            params.stop_fn = PartStop_Flickerer;
            params.draw_fn = PartDraw_Flickerer;
            params.impact_fn = PartImpact_Brick;
            params.time_step = FRAMETIME;
            part = AddPart(&params);
            if (part != NULL) {
                part->time_remaining = qrand() * 1.5259021893143654e-05 * 2.0 + 7.0;
            }
            object = object->next;
        }
    }

    void GizBuildIts_Draw(void *world_pointer, void *system_pointer, f32 unused) {
        WORLDINFO *world = (WORLDINFO *)world_pointer;
        GIZBUILDITSYS_s *system = (GIZBUILDITSYS_s *)system_pointer;
        GIZBUILDIT *buildit;
        GAMEANIMOBJ *object;
        GIZBUILDITANIMDATA_s *data;
        NUMTX matrix;
        f32 reflection_height;
        i32 index;
        if (system == NULL) {
            return;
        }
        buildit = system->buildits;
        for (index = 0; index < system->count; ++index, ++buildit) {
            if (!(buildit->availability_flags & 2) || (buildit->room_index >= 0 && world->rooms_visible_ptr[buildit->room_index] == 0)) {
                continue;
            }
            if (!(buildit->state_flags & 0x400)) {
                ResetShadowMapRendering();
            } else {
                EnableShadowMapRendering(0);
            }
            if (buildit->linked_buildit != NULL) {
                GAMEANIMSET *set = buildit->anim_set;
                if (set == NULL) {
                    continue;
                }
                reflection_height = 0.0f;
                if ((buildit->state_flags & 0x100) && set->objects != NULL && set->objects->object_data != NULL) {
                    data = set->objects->buildit_data;
                    reflection_height = buildit->unknown_50 + data->start_mtx.m31;
                }
                for (object = set->objects; object != NULL; object = object->next) {
                    data = object->buildit_data;
                    matrix = data->draw_mtx;
                    data->was_drawn = (u8)NuSpecialDrawAt(&object->special, &matrix);
                    if ((buildit->state_flags & 0x100) && !(object->flags & 2) &&
                        MatrixReflection(&data->draw_mtx, 2, reflection_height, WORLD->current_level->reflection_y, &matrix)) {
                        NuSpecialDrawAt(&object->special, &matrix);
                    }
                }
            } else {
                ResetShadowMapRendering();
                if (buildit->state_flags & 0x100) {
                    GameAnimSet_DrawReflection(buildit->anim_set, 2, buildit->unknown_50, NULL);
                }
            }
        }
        ResetShadowMapRendering();
    }

    i32 GizBuildIts_GetMaxGizmos(void *world_ptr) {
        WORLDINFO *world = (WORLDINFO *)world_ptr;
        if (world != NULL) {
            return world->current_level->max_buildits;
        }
        return 0;
    }

    void GizBuildIts_AddGizmos(GIZMOSYS *giz, i32 type, void *world, void *data) {
        GIZBUILDITSYS_s *s = (GIZBUILDITSYS_s *)data;
        i32 i;
        if (s == NULL) {
            return;
        }
        for (i = 0; i < s->count; ++i) {
            if (NuStrLen(s->buildits[i].name) != 0) {
                AddGizmo(giz, type, NULL, &s->buildits[i]);
            }
        }
    }

    char *GizmoBuildit_GetGizmoName(GIZMO *gizmo) {
        return gizmo != NULL ? (char *)gizmo->object : NULL;
    }

    i32 GizmoBuildit_GetOutput(GIZMO *gizmo, i32 output_index, i32 force_read) {
        GIZBUILDIT *buildit = gizmo->buildit;
        if (((buildit->availability.visible && buildit->availability.active) || force_read != 0) && buildit->build_state == 2) {
            return 1;
        }
        return 0;
    }

    char *GizmoBuildit_GetOutputName(GIZMO *gizmo, i32 output) {
        return "Finished";
    }

    i32 GizmoBuildit_GetNumOutputs(GIZMO *gizmo) {
        return 1;
    }

    void *GizBuildIts_AllocateProgressData(VARIPTR *buffer, VARIPTR *end) {
        return GizmoBufferAlloc(buffer, end, 0x60);
    }

    void GizBuildIts_ClearProgress(void *owner, void *progress_ptr) {
        BUILD_PROGRESS *progress = (BUILD_PROGRESS *)progress_ptr;
        if (progress == NULL) {
            return;
        }
        memset(progress->built, 0, sizeof(progress->built));
        memset(progress->enabled, -1, sizeof(progress->enabled));
        memset(progress->visible, -1, sizeof(progress->visible));
        memset(progress->completed, 0, sizeof(progress->completed));
        memset(progress->object_count, 0, sizeof(progress->object_count));
    }

    void GizBuildIts_StoreProgress(void *world_ptr, void *data, void *progress_ptr) {
        BUILD_PROGRESS *progress = (BUILD_PROGRESS *)progress_ptr;
        BUILD_PROGRESS_SYSTEM *system = (BUILD_PROGRESS_SYSTEM *)data;
        BUILD_PROGRESS_RECORD *record;
        i32 index;
        i32 word;
        u32 bit;
        if (progress == NULL) {
            return;
        }
        memset(progress->built, 0, sizeof(progress->built));
        memset(progress->enabled, -1, sizeof(progress->enabled));
        memset(progress->visible, -1, sizeof(progress->visible));
        memset(progress->completed, 0, sizeof(progress->completed));
        memset(progress->object_count, 0, sizeof(progress->object_count));

        record = system->records;
        for (index = 0; index < system->count; ++index, ++record) {
            if (index >= 64) {
                break;
            }
            word = index / 32;
            bit = 1u << (index & 31);
            if (record->build_state != 0) {
                progress->built[word] |= bit;
            }
            if (!(record->flags & 2)) {
                progress->visible[word] &= ~bit;
            }
            if (!(record->flags & 1)) {
                progress->enabled[word] &= ~bit;
            }
            if (record->flags & 0x200) {
                progress->completed[word] |= bit;
            }
            progress->object_count[index] = record->built_object_count;
        }
    }

    /* Studs the level's build-its release TotalLevelCoinTally. */
    u32 GizBuildIts_TotalScore(WORLDINFO *world) {
        GIZBUILDITSYS_s *system;
        GIZBUILDIT *buildit;
        u32 total = 0;
        i32 i;
        system = world->giz_buildit_sys;
        if (system != NULL) {
            buildit = system->buildits;
            if (buildit != NULL) {
                for (i = 0; i < system->count; i++, buildit++) {
                    total += (u16)buildit->completion_score;
                }
            }
        }
        return total;
    }

    void GizBuildIt_Finish(GIZBUILDIT *buildit) {
        buildit->build_state = 2;
        if ((buildit->state_flags & 2) != 0) {
            if (buildit->linked_buildit == NULL) {
                GameAnimSet_SetVisibility(buildit->anim_set, 0);
            }
            buildit->availability_flags &= (u16)~2u;
        }
        if (GizBuildIt_FinishFn != NULL) {
            GizBuildIt_FinishFn(buildit);
        }
    }

    void GizBuildIt_SetStepTime(GIZBUILDIT *buildit, GAMEOBJECT *player) {
        buildit->step_duration = 0.3f;
        if (player != NULL) {
            buildit->step_duration = (f64)0.3f / GizBuildItMul(player);
        }
        buildit->step_timer = buildit->step_duration;
    }

    void GizBuildIt_SetToStart(void *buildit_pointer, i32 emit_debris, i32 keep_built_pieces) {
        GIZBUILDIT *buildit = (GIZBUILDIT *)buildit_pointer;
        i32 restored_count;
        i32 i;
        f32 progress;
        if (buildit == NULL || buildit->anim_set == NULL) {
            return;
        }
        if (keep_built_pieces && buildit->anim_object_count != 0) {
            progress = (f32)buildit->built_object_count / buildit->anim_object_count;
            if (progress < 0.5f) {
                restored_count = 0;
            } else if (progress < (f64)0.9f) {
                restored_count = buildit->anim_object_count >> 1;
            } else {
                restored_count = buildit->built_object_count;
            }
        } else {
            restored_count = 0;
        }
        buildit->availability_flags |= 2;
        for (i = 0; i < buildit->anim_object_count; ++i) {
            GIZBUILDITANIMDATA_s *data = buildit->anim_objects[i]->buildit_data;
            if (emit_debris && i < buildit->built_object_count) {
                AddGameDebris(WORLD->game_debris_sys, GizBuilditGDeb[qrand() / 0x2aab], (NUVEC *)&data->end_mtx.m30);
            }
        }
        if (buildit->linked_buildit != NULL) {
            for (i = restored_count; i < buildit->anim_object_count; ++i) {
                GIZBUILDITANIMDATA_s *data = buildit->anim_objects[i]->buildit_data;
                data->draw_mtx = data->start_mtx;
            }
        } else {
            GameAnimSet_SetVisibility(buildit->anim_set, 1);
            for (i = restored_count; i < buildit->anim_object_count; ++i) {
                buildit->anim_objects[i]->instance_animation->flags &= ~NUINSTANIM_FLAG_PLAYING;
                buildit->anim_objects[i]->instance_animation->ltime = buildit->anim_objects[i]->start_frame;
            }
            buildit->step_timer = buildit->step_duration = 0.3f;
        }
        buildit->availability_flags &= ~4;
        buildit->built_object_count = (u8)restored_count;
        buildit->builders_active = 0;
        buildit->build_state = 0;
    }
}

extern "C" void GizBuildIt_SetToEnd(GIZBUILDIT *buildit) {
    if (buildit == NULL) {
        return;
    }
    buildit->availability_flags |= 2;
    if (buildit->linked_buildit != NULL) {
        for (i32 i = 0; i < buildit->anim_object_count; ++i) {
            GIZBUILDITANIMDATA_s *data = buildit->anim_objects[i]->buildit_data;
            data->draw_mtx = data->start_mtx;
        }
    } else {
        GameAnimSet_SetVisibility(buildit->anim_set, 1);
        GameAnimSet_JumpToEnd(buildit->anim_set);
        buildit->step_duration = 0.3f;
        buildit->step_timer = 0.3f;
    }
    buildit->built_object_count = 0;
    if (buildit->anim_set != NULL) {
        buildit->built_object_count = buildit->anim_object_count;
    }
    buildit->availability_flags &= ~4U;
    buildit->builders_active = 0;
    buildit->build_state = 2;
    if ((buildit->state_flags & 2) != 0) {
        if (buildit->linked_buildit == NULL) {
            GameAnimSet_SetVisibility(buildit->anim_set, 0);
        }
        buildit->availability_flags &= ~2U;
    }
    if (GizBuildIt_FinishFn != NULL) {
        GizBuildIt_FinishFn(buildit);
    }
}

extern "C" {

    i32 GizBuildIts_Load(void *world_pointer, void *system_pointer) {
        WORLDINFO *world = (WORLDINFO *)world_pointer;
        GIZBUILDITSYS_s *system = (GIZBUILDITSYS_s *)system_pointer;
        GIZBUILDIT *buildit;
        GAMEANIMOBJ *object;
        u8 version;
        i32 index;
        i32 linked_index;
        i32 animation_index = 0;
        i32 length;
        char linked_name[16];
        char blowup_name[32];
        version = (u8)EdFileReadChar();
        system->count = (u16)EdFileReadShort();
        if (system->count != 0) {
            buildit = system->buildits;
            for (index = 0; index < system->count; ++index, ++buildit) {
                EdFileRead(buildit->name, 16);
                EdFileReadNuVec(&buildit->file_position);
                GizmoFileReadGameAnimSet(buildit->anim_set, world, NULL, version, "GizBuildit", buildit->name);
                buildit->interaction_radius = EdFileReadFloat();
                if (version <= 6) {
                    EdFileReadFloat();
                }
                buildit->completion_score = EdFileReadShort();
                buildit->wobble_parameter = EdFileReadShort();
                buildit->batch_size = (u8)EdFileReadChar();
                buildit->progress = (u8)EdFileReadChar();
                if (version >= 6) {
                    buildit->unknown_50 = EdFileReadFloat();
                }
                buildit->blowup_type = -1;
                if (version >= 7) {
                    if (version >= 8) {
                        length = EdFileReadChar();
                        if (length != 0) {
                            EdFileRead(blowup_name, length);
                            buildit->blowup_type = (i16)GizmoBlowupGetNameTableId(blowup_name);
                            if (buildit->blowup_type != -1) {
                                buildit->availability_flags |= 0x80;
                            }
                        }
                    } else {
                        buildit->blowup_type = EdFileReadShort();
                    }
                    buildit->unknown_58 = EdFileReadShort();
                    buildit->unknown_5a = EdFileReadShort();
                    EdFileReadNuVec(&buildit->effect_position);
                }
                if (version >= 9) {
                    buildit->activation_radius = EdFileReadFloat();
                } else if (world->area != NULL && (world->area->flags & AREAFLAG_VEHICLE_AREA)) {
                    buildit->activation_radius = 12.0f;
                } else {
                    buildit->activation_radius = 1.75f;
                }
                if (version < 4) {
                    buildit->state_flags = EdFileReadChar();
                    if (version >= 2) {
                        if (EdFileReadChar() != 0) {
                            buildit->state_flags |= 0x40;
                        }
                        if (version >= 3 && EdFileReadChar() != 0) {
                            buildit->state_flags |= 0x80;
                        }
                    }
                } else {
                    buildit->state_flags = (u16)EdFileReadShort();
                }
                if (version > 4) {
                    buildit->linked_rotation = (u16)EdFileReadShort();
                    if (EdFileReadChar() != 0) {
                        EdFileRead(linked_name, 16);
                        for (linked_index = 0; linked_index < system->count; ++linked_index) {
                            if (NuStrICmp(system->buildits[linked_index].name, linked_name) == 0) {
                                buildit->linked_buildit = &system->buildits[linked_index];
                                buildit->availability_flags |= 3;
                                break;
                            }
                        }
                    }
                }
                buildit->anim_object_count = 0;
                buildit->anim_objects = &system->anim_objects[animation_index];
                for (object = buildit->anim_set->objects; object != NULL; object = object->next) {
                    if ((object->animation != NULL && object->instance_animation != NULL) || (buildit->state_flags & 0x40)) {
                        buildit->anim_objects[buildit->anim_object_count] = object;
                        ++buildit->anim_object_count;
                        ++animation_index;
                    }
                }
                GizBuildit_Reset(buildit, world);
            }
        }
        return 1;
    }

    void GizmoBuildit_Activate(GIZMO *gizmo, i32 active) {
        GIZBUILDIT *buildit;
        NUVEC scatter_position;
        i32 reward_released;
        i32 index;
        GAMEANIMOBJ *object;
        GIZBUILDITANIMDATA_s *object_data;
        if (gizmo == NULL) {
            return;
        }
        buildit = gizmo->buildit;
        buildit->availability.active = active != 0;
        if (!buildit->availability.active) {
            return;
        }
        if (buildit->state_flags & 0x200) {
            buildit->availability.reward_released = 0;
        }
        if (buildit->linked_buildit == NULL) {
            GameAnimSet_JumpToStart(buildit->anim_set);
        }
        reward_released = buildit->availability.reward_released;
        GizBuildit_Reset(buildit, NULL);
        buildit->availability.reward_released = reward_released != 0;
        if (!(buildit->state_flags & 0x40) || GizBuildit_AutoBuildPosFn == NULL) {
            return;
        }
        if (!GizBuildit_AutoBuildPosFn(NULL, &buildit->start_position, &scatter_position, NULL)) {
            return;
        }
        buildit->step_timer = 5.0f;
        for (index = 0; index < buildit->anim_object_count; ++index) {
            scatter_position.x = (f32)((f64)NuFloatRand(&GAMERAND) / 5.0 - (f64)0.1f + scatter_position.x);
            scatter_position.y = (f32)((f64)NuFloatRand(&GAMERAND) / 5.0 - (f64)0.1f + scatter_position.y);
            scatter_position.z = (f32)((f64)NuFloatRand(&GAMERAND) / 5.0 - (f64)0.1f + scatter_position.z);
            if (buildit->linked_buildit != NULL) {
                object = buildit->anim_objects[index];
                object_data = object->buildit_data;
                *(NUVEC *)&object_data->draw_mtx.m30 = scatter_position;
            } else {
                object = buildit->anim_objects[index];
                NuSpecialSetDrawPos(&object->special, &scatter_position);
            }
            AddGameDebris(WORLD->game_debris_sys, GizBuilditGDeb[qrand() / 0x2aab], &scatter_position);
        }
    }

    void GizBuildIts_LateUpdate(void *world_pointer, void *system_pointer, f32 unused) {
        WORLDINFO *world = (WORLDINFO *)world_pointer;
        GIZBUILDITSYS_s *system = (GIZBUILDITSYS_s *)system_pointer;
        GIZBUILDIT *buildit;
        NUVEC displacement;
        NUVEC reward_direction;
        NUVEC finish_position;
        NUVEC axis;
        NUVEC centre;
        NUVEC target;
        NUMTX matrix;
        i32 index;
        if (system == NULL) {
            return;
        }
        if (BuilditHopSfxWait > 0.0f) {
            BuilditHopSfxWait -= FRAMETIME;
        }
        buildit = system->buildits;
        for (index = 0; index < system->count; ++index, ++buildit) {
            if ((buildit->availability_flags & 2) == 0 || (buildit->availability_flags & 1) == 0 || buildit->anim_set == NULL ||
                buildit->anim_object_count == 0) {
                continue;
            }
            if (buildit->build_state == 1) {
                f32 timer = buildit->step_timer + FRAMETIME;
                f32 progress;
                i32 piece_index;
                buildit->step_timer = timer;
                if (timer >= 0.6f) {
                    i32 hearts;
                    Hint_SetComplete(LEGOHINT_BUILD);
                    GizBuildIt_Finish(buildit);
                    NuVecAdd(&finish_position, &buildit->position, &buildit->effect_position);
                    GameAudio_PlaySfx(0x3a, &finish_position, 0, 0);
                    NewRumbleAllPlayers(0.75f, 0.1f, 0, 0);
                    GameCam_Judder(GameCam, -0.4f, 0, NULL);
                    if ((buildit->availability_flags & 0x200) == 0) {
                        buildit->availability_flags |= 0x200;
                        hearts = ReleaseHearts();
                        if ((u16)buildit->completion_score > 0 || hearts != 0) {
                            NuVecRotateX(&reward_direction, &v010, (u16)buildit->unknown_58);
                            NuVecRotateY(&reward_direction, &reward_direction, (u16)buildit->unknown_5a);
                            AddPickups((u16)buildit->completion_score, hearts, 0, 0, &finish_position, &reward_direction, 2.0f, -1, 1.75f,
                                       2000000.0f, NULL, 1, 0);
                        }
                    }
                    if (buildit->blowup_type != -1) {
                        GizmoBlowUpTypeBlowUp(world, buildit->blowup_type, &buildit->position);
                        GameAnimSet_SetVisibility(buildit->anim_set, 0);
                    }
                    progress = 1.0f;
                } else {
                    f32 ratio = (f64)timer / (f64)0.6f;
                    progress = ratio;
                }
                for (piece_index = 0; piece_index < buildit->anim_object_count; ++piece_index) {
                    GAMEANIMOBJ *piece = buildit->anim_objects[piece_index];
                    GIZBUILDITANIMDATA_s *data = piece->buildit_data;
                    if (buildit->linked_buildit != NULL) {
                        finish_position = *(NUVEC *)&data->end_mtx.m30;
                    } else {
                        finish_position = *NuSpecialGetPos(&piece->special);
                    }
                    f32 sine = NU_SIN_LUT((f64)progress * 32768.0);
                    f32 offset = (f64)sine * (f64)0.2f;
                    if ((buildit->state_flags & GIZBUILDIT_STATE_ROTATING_WOBBLE) != 0) {
                        displacement.x = displacement.y = 0.0f;
                        displacement.z = offset;
                        NuVecRotateY(&displacement, &displacement, (i32)((u32)(u16)buildit->wobble_parameter << 16) / 360);
                    } else {
                        displacement.x = displacement.z = 0.0f;
                        displacement.y = offset;
                    }
                    NuVecAdd(&finish_position, &finish_position, &displacement);
                    if (buildit->linked_buildit != NULL) {
                        *(NUVEC *)&data->draw_mtx.m30 = finish_position;
                    } else {
                        NuSpecialSetDrawPos(&piece->special, &finish_position);
                    }
                }
            } else if (buildit->build_state == 0) {
                f32 buzz_time = 0.0f;
                i32 buzz_frames = 0;
                f32 previous_timer;
                f32 timer;
                if ((buildit->state_flags & 0x40) != 0) {
                    if (GizBuildit_AutoBuildPosFn != NULL) {
                        u16 facing;
                        if (GizBuildit_AutoBuildPosFn(world, &buildit->start_position, &centre, &facing) != 0) {
                            f32 radius;
                            u16 angle;
                            u16 angle_step;
                            i32 piece_index;
                            angle = (u16)(facing - 0x4000);
                            if (buildit->step_timer > 4.5f) {
                                angle_step = (u16)(i32)BuildItSafeRatio(65536.0f, buildit->anim_object_count);
                                radius = 0.5f;
                            } else {
                                f32 phase = (4.5 - (f64)buildit->step_timer) / 4.5;
                                f32 sine = NU_SIN_LUT((f64)phase * 16384.0 + 32768.0 + 16384.0);
                                f64 progress = (f64)sine + 1.0;
                                f32 turns = progress * 5.0;
                                angle = (u16)(i32)((f64)turns * 360.0 * 65536.0 / 360.0 + (facing - 0x4000));
                                angle_step = (u16)(i32)BuildItSafeRatio(65536.0f, buildit->anim_object_count);
                                radius = 0.5 - progress * (f64)0.3f;
                            }
                            for (piece_index = buildit->anim_object_count - 1; piece_index >= buildit->built_object_count; --piece_index) {
                                NUVEC *position;
                                angle = (u16)(angle + angle_step);
                                target = centre;
                                target.x = centre.x + radius * NU_COS_LUT(angle);
                                target.z = centre.z + radius * NU_SIN_LUT(angle);
                                position = NuSpecialGetDrawPos(&buildit->anim_objects[piece_index]->special);
                                target.x = SeekValF(position->x, target.x, 6.0f);
                                target.y = SeekValF(position->y, target.y, 6.0f);
                                target.z = SeekValF(position->z, target.z, 6.0f);
                                if (buildit->linked_buildit != NULL) {
                                    GIZBUILDITANIMDATA_s *data = buildit->anim_objects[piece_index]->buildit_data;
                                    *(NUVEC *)&data->draw_mtx.m30 = target;
                                } else {
                                    NuSpecialSetDrawPos(&buildit->anim_objects[piece_index]->special, &target);
                                    NuSpecialUpdate(&buildit->anim_objects[piece_index]->special);
                                }
                            }
                        }
                    }
                    previous_timer = buildit->step_timer;
                    timer = buildit->step_timer - FRAMETIME;
                    buildit->step_timer = timer;
                    if (timer <= 0.0f) {
                        GAMEANIMOBJ *piece = buildit->anim_objects[buildit->built_object_count];
                        if (buildit->linked_buildit != NULL) {
                            GIZBUILDITANIMDATA_s *data = piece->buildit_data;
                            data->draw_mtx = data->end_mtx;
                            GameAudio_PlaySfx(0x3b, (NUVEC *)&data->draw_mtx.m30, 0, 0);
                        } else {
                            NuSpecialSetVisibility(&piece->special, 1);
                            if (piece->instance_animation != NULL) {
                                piece->instance_animation->flags &= ~NUINSTANIM_FLAG_PLAYING;
                                piece->instance_animation->ltime = piece->end_frame;
                                EvalAnim2(&piece->special, piece->instance_animation->ltime);
                            }
                            GameAudio_PlaySfx(0x3b, NuSpecialGetDrawPos(&piece->special), 0, 0);
                            NuSpecialSetDrawPos(&piece->special, NuSpecialGetPos(&piece->special));
                        }
                        ++buildit->built_object_count;
                        if (buildit->built_object_count == buildit->anim_object_count) {
                            i32 piece_index;
                            buildit->step_timer = 0.0f;
                            buildit->build_state = 1;
                            for (piece_index = 0; piece_index < buildit->anim_object_count; ++piece_index) {
                                GIZBUILDITANIMDATA_s *data = buildit->anim_objects[piece_index]->buildit_data;
                                AddGameDebris(world->game_debris_sys, GizBuilditGDeb[qrand() / 0x2aab], (NUVEC *)&data->end_mtx.m30);
                            }
                        } else {
                            buildit->step_duration = 0.3f;
                            buildit->step_timer = 0.3f;
                        }
                    } else {
                        GAMEANIMOBJ *piece = buildit->anim_objects[buildit->built_object_count];
                        if (previous_timer == buildit->step_duration) {
                            AddGameDebris(world->game_debris_sys, GizBuilditGDeb[qrand() / 0x2aab], NuSpecialGetDrawPos(&piece->special));
                        }
                        GizMoveAttractoBuildItPiece(buildit, piece);
                    }
                } else if (buildit->builders_active != 0) {
                    GAMEOBJECT *builder = BuildItFindActiveBuilder(buildit);
                    previous_timer = buildit->step_timer;
                    timer = buildit->step_timer - FRAMETIME;
                    buildit->step_timer = timer;
                    if (timer <= 0.0f) {
                        GAMEANIMOBJ *piece = buildit->anim_objects[buildit->built_object_count];
                        if (buildit->linked_buildit != NULL) {
                            GIZBUILDITANIMDATA_s *data = piece->buildit_data;
                            data->draw_mtx = data->end_mtx;
                            GameAudio_PlaySfx(0x3b, (NUVEC *)&data->draw_mtx.m30, 0, 0);
                        } else {
                            NuSpecialSetVisibility(&piece->special, 1);
                            piece->instance_animation->flags &= ~NUINSTANIM_FLAG_PLAYING;
                            piece->instance_animation->ltime = piece->end_frame;
                            EvalAnim2(&piece->special, piece->instance_animation->ltime);
                            GameAudio_PlaySfx(0x3b, NuSpecialGetDrawPos(&piece->special), 0, 0);
                        }
                        ++buildit->built_object_count;
                        if (buildit->built_object_count == buildit->anim_object_count) {
                            i32 piece_index;
                            buildit->step_timer = 0.0f;
                            buildit->build_state = 1;
                            for (piece_index = 0; piece_index < buildit->anim_object_count; ++piece_index) {
                                GIZBUILDITANIMDATA_s *data = buildit->anim_objects[piece_index]->buildit_data;
                                AddGameDebris(world->game_debris_sys, GizBuilditGDeb[qrand() / 0x2aab], (NUVEC *)&data->end_mtx.m30);
                            }
                            buzz_time = 0.1f;
                        } else if (buildit->built_object_count <= buildit->anim_object_count) {
                            GizBuildIt_SetStepTime(buildit, builder);
                            buzz_frames = 1;
                        }
                        if (builder != NULL) {
                            if (buzz_time > 0.0f) {
                                NewBuzz(builder->player.pad->bound_pad, buzz_time, 0);
                            } else if (buzz_frames != 0) {
                                NewBuzzFrames(builder->player.pad->bound_pad, buzz_frames, 0);
                            }
                            if (buildit->build_state == 0 &&
                                ((builder->player.pad->buttons_held & GAMEPAD_SPECIAL) != 0 || (builder->flags_1f8 & 0x40000) != 0)) {
                                builder->player.flags_e20 =
                                    ((~builder->player.flags_e20 ^ builder->player.flags_e20) & 0x4000) ^ builder->player.flags_e20;
                                if (builder->player.big_jump_animation < 10) {
                                    ++builder->player.big_jump_animation;
                                }
                            } else {
                                ReleaseBuildIt(builder, buildit->build_state);
                            }
                        }
                    } else {
                        GAMEANIMOBJ *piece = buildit->anim_objects[buildit->built_object_count];
                        if (previous_timer == buildit->step_duration) {
                            AddGameDebris(world->game_debris_sys, GizBuilditGDeb[qrand() / 0x2aab], NuSpecialGetDrawPos(&piece->special));
                        }
                        if (buildit->linked_buildit == NULL) {
                            NuSpecialSetVisibility(&piece->special, 0);
                        }
                    }
                } else {
                    GizBuildIt_SetStepTime(buildit, NULL);
                    if (buildit->linked_buildit == NULL && buildit->built_object_count < buildit->anim_object_count) {
                        NuSpecialSetVisibility(&buildit->anim_objects[buildit->built_object_count]->special, 1);
                    }
                }
            }
            if (buildit->build_state != 0 || (buildit->state_flags & 0x40) != 0 ||
                ((buildit->state_flags & 0x80) != 0 && ShadowMode == 0)) {
                continue;
            }
            {
                f32 probability = (f64)0.1f - (buildit->anim_object_count - buildit->built_object_count) * (f64)0.002f;
                i32 piece_index;
                if (probability < 0.01f) {
                    probability = 0.01f;
                }
                if ((buildit->availability_flags & 4) == 0) {
                    probability = (f64)probability * (f64)0.1f;
                }
                for (piece_index = 0; piece_index < buildit->anim_object_count; ++piece_index) {
                    GAMEANIMOBJ *piece;
                    GIZBUILDITANIMDATA_s *data;
                    if (buildit->linked_buildit != NULL && piece_index < buildit->built_object_count) {
                        continue;
                    }
                    piece = buildit->anim_objects[piece_index];
                    data = piece->buildit_data;
                    if (buildit->linked_buildit != NULL) {
                        matrix = data->start_mtx;
                    } else {
                        matrix = *NuSpecialGetMtx(&piece->special);
                    }
                    if (data->wobble_time > 0.0f) {
                        f32 wobble_time = data->wobble_time - FRAMETIME;
                        data->wobble_time = wobble_time;
                        if (wobble_time <= 0.0f && BuilditHopSfxWait <= 0.0f &&
                            ((buildit->availability_flags & 4) != 0 || qrand() < 32768)) {
                            GameAudio_PlaySfx(0x3c, &buildit->start_position, 0, 0);
                            BuilditHopSfxWait = (f64)qrand() * 1.5259021893143654e-05 * (f64)0.2f + (f64)0.1f;
                        }
                    } else if (piece_index >= buildit->built_object_count &&
                               ((buildit->linked_buildit != NULL && data->was_drawn != 0) ||
                                (buildit->linked_buildit == NULL && NuSpecialGetOnScreenFn(&piece->special) != 0))) {
                        if (qrand() * 1.5259021893143654e-05 < probability) {
                            data->wobble_time = 0.2f;
                            data->wobble_axis = (u8)(qrand() / 0x2aab);
                        }
                    }
                    if (data->wobble_time > 0.0f) {
                        f32 phase = (f64)data->wobble_time / (f64)0.2f;
                        f32 sine = NU_SIN_LUT((1.0f - phase) * 65536.0);
                        i32 angle = (i32)((f64)sine * 3640.0);
                        if ((data->wobble_axis & 1) != 0) {
                            angle = -angle;
                        }
                        if (data->wobble_axis == 0 || data->wobble_axis == 1) {
                            NuMtxGetXAxis(&matrix, &axis);
                            NuMtxPreRotateX(&matrix, (i32)(angle * BuildItAxisWobbleFactor(&axis)));
                        } else if (data->wobble_axis == 2 || data->wobble_axis == 3) {
                            NuMtxGetYAxis(&matrix, &axis);
                            NuMtxPreRotateY(&matrix, (i32)(angle * BuildItAxisWobbleFactor(&axis)));
                        } else {
                            NuMtxGetZAxis(&matrix, &axis);
                            NuMtxPreRotateZ(&matrix, (i32)(angle * BuildItAxisWobbleFactor(&axis)));
                        }
                        matrix.m31 += (f64)NU_SIN_LUT((f64)phase * 32768.0) * GIZBUILDITWOBBLEJUMPHEIGHT * buildit->interaction_radius;
                    }
                    if (buildit->linked_buildit != NULL) {
                        data->draw_mtx = matrix;
                    } else {
                        NuSpecialSetDrawMtx(&piece->special, &matrix);
                        NuSpecialUpdate(&piece->special);
                    }
                }
            }
        }
    }

    i32 LEGOHINT_BUILD = -1;
    f32 GIZBUILDITWOBBLEJUMPHEIGHT = 0.05f;
    i16 GizBuilditGDeb[6] = {-1, -1, -1, -1, -1, -1};
    i32 gizbuildit_gizmotype_id = -1;
    char *GizmoRegistryGizBuildItsNamePtr = "GizBuildit";
    f32 BuilditHopSfxWait = 0;
    i32 (*GizBuildit_AutoBuildPosFn)(void *context, NUVEC *position, NUVEC *result, u16 *angle) = NULL;
    i32 (*GizBuildIt_CanStartBuildingFn)(struct GIZBUILDIT *input, GAMEOBJECT *object) = NULL;
    void (*GizBuildIt_FinishFn)(struct GIZBUILDIT *input) = NULL;
    static ADDGIZMOTYPE GizmoRegistryGizBuildItsAddType = {0};

    ADDGIZMOTYPE *GizBuildIts_RegisterGizmo(i32 type_id) {
        ADDGIZMOTYPE *addtype = &GizmoRegistryGizBuildItsAddType;
        *addtype = Default_ADDGIZMOTYPE;
        addtype->name = GizmoRegistryGizBuildItsNamePtr;
        addtype->fns.panel_draw_fn = NULL;
        addtype->fns.activate_rev_fn = NULL;
        addtype->fns.get_visibility_fn = NULL;
        addtype->fns.using_special_fn = NULL;
        addtype->fns.bolt_hit_plat_fn = NULL;
        addtype->fns.get_best_bolt_target_fn = NULL;
        addtype->fns.bolt_hit_fn = NULL;
        addtype->fns.add_level_sfx_fn = NULL;
        addtype->prefix = "";
        addtype->fns.options = 0x60;
        addtype->fns.get_max_gizmos_fn = (i32(*)(void *))GizBuildIts_GetMaxGizmos;
        addtype->fns.add_gizmos_fn = (void *)GizBuildIts_AddGizmos;
        addtype->fns.early_update_fn = (void (*)(void *, void *, f32))GizBuildIts_EarlyUpdate;
        addtype->fns.late_update_fn = (void (*)(void *, void *, f32))GizBuildIts_LateUpdate;
        addtype->fns.draw_fn = (void (*)(void *, void *, f32))GizBuildIts_Draw;
        addtype->fns.get_gizmo_name_fn = (char *(*)(GIZMO *))GizmoBuildit_GetGizmoName;
        addtype->fns.get_output_fn = (i32(*)(GIZMO *, i32, i32))GizmoBuildit_GetOutput;
        addtype->fns.get_output_name_fn = (char *(*)(GIZMO *, i32))GizmoBuildit_GetOutputName;
        addtype->fns.get_num_outputs_fn = (void *)GizmoBuildit_GetNumOutputs;
        addtype->fns.activate_fn = (void (*)(GIZMO *, i32))GizmoBuildit_Activate;
        addtype->fns.set_visibility_fn = (void (*)(GIZMO *, i32))GizmoBuildit_SetVisibility;
        addtype->fns.get_pos_fn = (void *)GizmoBuildit_GetPos;
        addtype->fns.allocate_progress_data_fn = (void *(*)(VARIPTR *, VARIPTR *))GizBuildIts_AllocateProgressData;
        addtype->fns.clear_progress_fn = (void (*)(void *, void *))GizBuildIts_ClearProgress;
        addtype->fns.store_progress_fn = (void (*)(void *, void *, void *))GizBuildIts_StoreProgress;
        addtype->fns.reset_fn = (void (*)(void *, void *, void *))GizBuildIts_Reset;
        addtype->fns.reserve_buffer_space_fn = (void *(*)(void *))GizBuildIts_ReserveBufferSpace;
        addtype->fns.load_fn = (i32(*)(void *, void *))GizBuildIts_Load;
        addtype->fns.post_load_fn = (void (*)(void *, void *))GizBuildIts_PostLoad;
        gizbuildit_gizmotype_id = type_id;
        return addtype;
    }

    void BuildIt_MoveCode(GAMEOBJECT *player) {
        GIZBUILDIT *buildit;
        i32 animation;
        f32 timer;
        if (LEGOCONTEXT_BUILDIT == -1) {
            return;
        }
        if ((player->character_data->model_flags & (CHARACTER_MODEL_DROID | CHARACTER_MODEL_VEHICLE)) != 0) {
            return;
        }
        if (player->player.character_context == LEGOCONTEXT_BUILDIT) {
            if (BoltSys->stop_targeting != NULL) {
                BoltSys->stop_targeting(player, &player->collision_position);
            }
            buildit = player->player.buildit;
            if (buildit == NULL) {
                return;
            }
            GizBuildIt_SetHeadTarget(buildit, player);
            buildit = player->player.buildit;
            if ((buildit->state_flags & 0x80) != 0 && ShadowMode == 0) {
                player->player.character_context = CHARACTER_CONTEXT_NONE;
                buildit->builders_active = 1;
                (player->player.buildit)->availability_flags |= 4;
            }
            animation = player->player.context_animation;
            if (player->character_model->model_data_b[animation] != NULL && AnimPlaying(&player->anim_packet, animation, 1, 0) == NULL) {
                return;
            }
            buildit = player->player.buildit;
            if (buildit->build_state == 0 && (player->player.pad->buttons_held & GAMEPAD_SPECIAL) != 0 &&
                (f64)player->player.context_timer < (f64)0.2f) {
                player->player.context_timer = 0.2f;
            } else {
                timer = (f64)player->player.context_timer - FRAMETIME;
                player->player.context_timer = timer;
                if (timer <= 0.0f) {
                    player->player.character_context = CHARACTER_CONTEXT_NONE;
                }
            }
            if (player->player.character_context != CHARACTER_CONTEXT_NONE) {
                buildit->builders_active = 1;
                (player->player.buildit)->availability_flags |= 4;
            }
            return;
        }
        if (LEGOACT_BUILD == -1 || player->character_model->model_data_b[LEGOACT_BUILD] == NULL) {
            return;
        }
        if (AnimPlaying(&player->anim_packet, LEGOACT_BUILD, 1, 1) != NULL) {
            return;
        }
        if (player->ground_flags == 0) {
            return;
        }
        if (ObjLandReady(player) == 0 && objInNetWaitContext(player, LEGOCONTEXT_BUILDIT) == 0) {
            return;
        }
        buildit = GizBuildIt_FindNearest(WORLD, player, 1, ShadowMode);
        if (objInNetWaitContext(player, LEGOCONTEXT_BUILDIT) != 0) {
            timer = (f64)player->player.context_timer - FRAMETIME;
            player->player.context_timer = timer;
            if (timer <= 0.0f) {
                player->player.character_context = CHARACTER_CONTEXT_NONE;
                player->player.panel_request_data = NULL;
            }
        }
        if (buildit == NULL || (player->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) == 0) {
            return;
        }
        if (player->ground_flags != 0 && (buildit->state_flags & 4) != 0) {
            GizBuildItPushAwayFromStart(player, buildit);
        }
        GizBuildIt_SetHeadTarget(buildit, player);
        buildit->availability_flags |= 4;
        if ((player->player.pad->buttons_pressed & GAMEPAD_SPECIAL) != 0 ||
            (objInNetWaitContext(player, LEGOCONTEXT_BUILDIT) != 0 && (player->player.pad->buttons_held & GAMEPAD_SPECIAL) != 0)) {
            if (GizBuildIt_CanStartBuildingFn != NULL && GizBuildIt_CanStartBuildingFn(buildit, player) == 0) {
                return;
            }
            player->player.character_context = (i8)LEGOCONTEXT_BUILDIT;
            player->player.context_animation = LEGOACT_BUILD;
            player->player.context_timer = 0.4f;
            player->player.flags_e20 &= ~0x4000;
            player->player.grapple = buildit;
            player->player.big_jump_animation = 0;
            GizBuildIt_SetStepTime(buildit, player);
            GameCam_Blend(GameCam, 0.5f, 0.0f, 1);
            Hint_SetComplete(LEGOHINT_BUILD);
        }
        if ((player->flags_1fc & APIOBJECT_FLAG_PLAYER_ACTIVE) != 0 && player->ground_flags != 0) {
            GizBuildItPushAwayFromEnd(player);
        }
    }
}
