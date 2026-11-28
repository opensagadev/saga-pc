#include <stdio.h>
#include <string.h>
#include "gameapi/edptl.h"
#include "gameapi/edtools/edanim_internal.h"
#include "gameapi/edtools/edfile.h"
#include "gameapi/edtools/edui.h"
#include "gameframework/saveload.h"
#include "gamelib/util/Utilities.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/misc/utilities.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/world/level.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/nugscn_gen.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "legoapi/menus/screens/gamestatus_lsw.h"

GAMEANIMPROGRESS gameanimsysprogress = {0};

void GameAnimSys_AllocateLevelProgressData(VARIPTR *buf, VARIPTR *end, i32 capacity, i32 level_count) {
    i32 i;

    if (buf == NULL || end == NULL) {
        return;
    }
    gameanimsysprogress.count = level_count;
    gameanimsysprogress.entry_size = capacity;
    gameanimsysprogress.entries = GameBufferAlloc(buf, end, level_count * sizeof(u8 *));
    if (gameanimsysprogress.entries != NULL) {
        for (i = 0; i < level_count; ++i) {
            gameanimsysprogress.entries[i] = GameBufferAlloc(buf, end, capacity);
        }
    }
}

void GameAnimSys_StoreProgress(GAMEANIMSYS *system, i32 index) {
    u8 *progress;
    i32 i;

    if (system == NULL || system->sets == NULL || index < 0 || index >= gameanimsysprogress.count) {
        return;
    }
    progress = gameanimsysprogress.entries[index];
    for (i = 0; i < gameanimsysprogress.entry_size && system->sets[i] != NULL; ++i) {
        progress[i] = (u8)system->sets[i]->state;
    }
}

u8 *GameAnimSys_GetProgressData(i32 index) {
    if (index < 0 || index >= gameanimsysprogress.count) {
        return NULL;
    }
    return gameanimsysprogress.entries[index];
}

void GameAnimSys_ClearProgress(i32 index) {
    u8 *progress;
    i32 i;

    if (index < 0 || index >= gameanimsysprogress.count) {
        return;
    }
    progress = gameanimsysprogress.entries[index];
    for (i = 0; i < gameanimsysprogress.entry_size; ++i) {
        progress[i] = 0;
    }
}

GAMEANIMSYS *GameAnimSys_Create(VARIPTR *buf, VARIPTR *end) {
    GAMEANIMSYS *system = GameBufferAlloc(buf, end, sizeof(GAMEANIMSYS));

    if (system != NULL && gameanimsysprogress.entry_size != 0) {
        system->sets = GameBufferAlloc(buf, end, gameanimsysprogress.entry_size * sizeof(GAMEANIMSET *));
    }
    return system;
}

GAMEANIMSET *GameAnimSet_Create(VARIPTR *buf, VARIPTR *end, GAMEANIMOBJPOOL *pool, GAMEANIMSYS *system) {
    GAMEANIMSET *set = NULL;
    i32 index;

    if (pool != NULL) {
        set = GameBufferAlloc(buf, end, sizeof(GAMEANIMSET));
        if (set != NULL) {
            set->object_pool = pool;
            set->system = system;
            index = system->set_count;
            if (index < gameanimsysprogress.entry_size) {
                system->sets[index] = set;
                ++system->set_count;
            }
        }
    }
    return set;
}

void GameAnimSet_AddToSystemList(GAMEANIMSET *set) {
    if (set != NULL && set->system != NULL && (set->flags & GAMEANIMSET_IN_ACTIVE_LIST) == 0) {
        NuLinkedListAppend(&set->system->active_sets, &set->links);
        set->flags = (u16)(set->flags | GAMEANIMSET_IN_ACTIVE_LIST);
    }
}

void GameAnimSet_EvaluateState(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;
    NUINSTANIM *animation;
    i32 all_at_end = 1;
    i32 all_at_start = 1;
    f32 direction;

    if ((set->flags & GAMEANIMSET_IN_ACTIVE_LIST) != 0) {
        return;
    }
    object = set->objects;
    while (object != NULL) {
        animation = object->instance_animation;
        if (animation != NULL) {
            direction = object->start_frame > object->end_frame ? -1.0f : 1.0f;
            if (animation->ltime * direction < object->end_frame * direction) {
                all_at_end = 0;
            }
            if (object->start_frame * direction < animation->ltime * direction) {
                all_at_start = 0;
            }
        }
        object = object->next;
    }
    set->state = GAMEANIMSET_STATE_AT_START;
    if (all_at_end != 0) {
        set->state = GAMEANIMSET_STATE_AT_END;
    } else if (all_at_start == 0) {
        set->state = GAMEANIMSET_STATE_PART_WAY;
    }
}

void GameAnimSet_RemoveFromSystemList(GAMEANIMSET *set) {
    if (set != NULL && set->system != NULL) {
        if ((set->flags & GAMEANIMSET_IN_ACTIVE_LIST) != 0) {
            NuLinkedListRemove(&set->system->active_sets, &set->links);
            set->flags = (u16)(set->flags & ~GAMEANIMSET_IN_ACTIVE_LIST);
        }
        GameAnimSet_EvaluateState(set);
    }
}

void GameAnimSys_ReStoreProgress(GAMEANIMSYS *system, i32 index) {
    u8 *progress;
    i32 i;

    if (system == NULL || system->sets == NULL || index < 0 || index >= gameanimsysprogress.count) {
        return;
    }
    progress = gameanimsysprogress.entries[index];
    for (i = 0; i < gameanimsysprogress.entry_size; ++i) {
        if (system->sets[i] == NULL) {
            break;
        }
        system->sets[i]->state = (i8)progress[i];
        if (system->sets[i]->state == GAMEANIMSET_STATE_FORWARD || system->sets[i]->state == GAMEANIMSET_STATE_BACKWARD) {
            if ((system->sets[i]->flags & GAMEANIMSET_IN_ACTIVE_LIST) == 0) {
                GameAnimSet_AddToSystemList(system->sets[i]);
            }
        } else if ((system->sets[i]->flags & GAMEANIMSET_IN_ACTIVE_LIST) != 0) {
            GameAnimSet_RemoveFromSystemList(system->sets[i]);
        }
    }
}

GAMEANIMOBJPOOL *GameAnimSet_CreateObjectPool(VARIPTR *buf, VARIPTR *end, i32 object_data_size, i32 capacity) {
    GAMEANIMOBJPOOL *pool = NULL;
    i32 i;

    if (capacity != 0) {
        pool = GameBufferAlloc(buf, end, sizeof(GAMEANIMOBJPOOL));
        if (pool != NULL) {
            pool->object_data_size = (u16)object_data_size;
            pool->capacity = (u16)capacity;
            pool->objects = GameBufferAlloc(buf, end, (u16)capacity * sizeof(GAMEANIMOBJ));
            if (object_data_size != 0) {
                pool->object_data = GameBufferAlloc(buf, end, pool->object_data_size * pool->capacity);
            }
            for (i = 0; i < pool->capacity; ++i) {
                pool->objects[i].next = pool->free_objects;
                pool->free_objects = &pool->objects[i];
            }
        }
    }
    return pool;
}

void GameAnimSet_RemoveObject(GAMEANIMSET *set, GAMEANIMOBJ *object) {
    GAMEANIMOBJ *previous;

    if (set == NULL || object == NULL) {
        return;
    }
    if (object == set->objects) {
        set->objects = object->next;
    } else {
        previous = set->objects;
        while (previous != NULL && previous->next != object) {
            previous = previous->next;
        }
        if (previous != NULL) {
            previous->next = object->next;
        }
    }
    object->next = NULL;
    --set->object_count;
    --set->object_pool->active_count;
    object->next = set->object_pool->free_objects;
    set->object_pool->free_objects = object;
}

void GameAnimSet_RemoveAllObjects(GAMEANIMSET *set) {
    if (set != NULL) {
        while (set->objects != NULL) {
            GameAnimSet_RemoveObject(set, set->objects);
        }
    }
}

void GameAnimSet_SetTFactor(GAMEANIMSET *set, f32 factor) {
    GAMEANIMOBJ *object;
    f32 direction;

    if (set == NULL) {
        return;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        if (object->instance_animation != NULL) {
            direction = object->start_frame > object->end_frame ? -1.0f : 1.0f;
            if (object->instance_animation->fparam1 != 0.0f) {
                object->instance_animation->tfactor = object->instance_animation->fparam1 * factor * direction;
            } else {
                object->instance_animation->tfactor = direction * factor;
            }
        }
    }
}

void GameAnimSet_ScaleFParam1(GAMEANIMSET *set, f32 scale) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->fparam1 *= scale;
            }
        }
    }
}

i32 GameAnimSet_IsAnimationReset(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;
    NUINSTANIM *animation;
    i32 animated_count;
    i32 repeating_count;
    f32 direction;

    if (set != NULL) {
        animated_count = 0;
        repeating_count = 0;
        for (object = set->objects; object != NULL; object = object->next) {
            animation = object->instance_animation;
            if (animation == NULL) {
                continue;
            }
            ++animated_count;
            if ((animation->flags & NUINSTANIM_FLAG_REPEATING) != 0) {
                ++repeating_count;
                continue;
            }
            if ((animation->flags & NUINSTANIM_FLAG_PLAYING) != 0) {
                return 0;
            }
            direction = object->start_frame > object->end_frame ? -1.0f : 1.0f;
            if (animation->ltime * direction > object->start_frame * direction) {
                return 0;
            }
        }
        if (animated_count > 0 && repeating_count == animated_count) {
            return 0;
        }
    }
    return 1;
}

i32 GameAnimSet_Reset(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->flags &= ~NUINSTANIM_FLAG_PLAYING;
                object->instance_animation->flags &= ~4u;
                object->instance_animation->ltime = object->start_frame;
                object->instance_animation->tfactor = 1.0f;
            }
            GameAnimSet_RemoveFromSystemList(set);
        }
    }
    return 1;
}

i32 GameAnimSet_Play(GAMEANIMSET *set, f32 speed, i32 evaluate_state) {
    GAMEANIMOBJ *object;
    f32 direction;

    if (set == NULL) {
        return 1;
    }
    if (evaluate_state != 0) {
        GameAnimSet_EvaluateState(set);
    }
    set->flags = (u16)(set->flags & ~GAMEANIMSET_STOPPED);
    if (speed >= 0.0f) {
        if (set->state == GAMEANIMSET_STATE_AT_END) {
            return 1;
        }
    } else if (speed < 0.0f) {
        if (set->state == GAMEANIMSET_STATE_AT_START) {
            return 1;
        }
    }
    for (object = set->objects; object != NULL; object = object->next) {
        if (object->instance_animation != NULL) {
            object->instance_animation->flags |= NUINSTANIM_FLAG_PLAYING;
            object->instance_animation->flags &= ~4u;
            direction = object->start_frame > object->end_frame ? -1.0f : 1.0f;
            if (object->instance_animation->ltime * direction < object->start_frame * direction) {
                object->instance_animation->ltime = object->start_frame;
            } else if (object->end_frame * direction < object->instance_animation->ltime * direction) {
                object->instance_animation->ltime = object->end_frame;
            }
            if (object->instance_animation->fparam1 != 0.0f) {
                object->instance_animation->tfactor = object->instance_animation->fparam1 * speed * direction;
            } else {
                object->instance_animation->tfactor = direction * speed;
            }
        }
    }
    if ((set->flags & GAMEANIMSET_IN_ACTIVE_LIST) == 0) {
        GameAnimSet_AddToSystemList(set);
    }
    return 1;
}

i32 GameAnimSet_Stop(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set != NULL && (set->state == GAMEANIMSET_STATE_FORWARD || set->state == GAMEANIMSET_STATE_BACKWARD)) {
        set->flags = (u16)(set->flags | GAMEANIMSET_STOPPED);
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->flags &= ~NUINSTANIM_FLAG_PLAYING;
            }
        }
    }
    return 1;
}

i32 GameAnimSet_SetRepeating(GAMEANIMSET *set, i32 repeating) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->flags =
                    (object->instance_animation->flags & ~NUINSTANIM_FLAG_REPEATING) | (((u32)repeating & 1) << 3);
            }
        }
    }
    return 1;
}

i32 GameAnimSet_JumpToEnd(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->flags &= ~NUINSTANIM_FLAG_PLAYING;
                object->instance_animation->ltime = object->end_frame;
            }
        }
    }
    return 1;
}

i32 GameAnimSet_JumpToStart(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->flags &= ~NUINSTANIM_FLAG_PLAYING;
                object->instance_animation->ltime = object->start_frame;
            }
        }
    }
    return 1;
}

void GameAnimSet_JumpToAnimPos(GAMEANIMSET *set, f32 position) {
    GAMEANIMOBJ *object;
    f32 direction;

    if (set == NULL) {
        return;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        if (object->instance_animation != NULL && object->animation != NULL) {
            object->instance_animation->flags &= ~NUINSTANIM_FLAG_PLAYING;
            object->instance_animation->ltime = (object->end_frame - object->start_frame) * position + object->start_frame;
            direction = object->start_frame > object->end_frame ? -1.0f : 1.0f;
            if (object->end_frame * direction < object->instance_animation->ltime * direction) {
                object->instance_animation->ltime = object->end_frame;
            }
            if (object->instance_animation->ltime * direction < object->start_frame * direction) {
                object->instance_animation->ltime = object->start_frame;
            }
        }
    }
}

i32 GameAnimSet_Playing(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL && (object->instance_animation->flags & NUINSTANIM_FLAG_PLAYING) == 0) {
                return 0;
            }
        }
    }
    return 1;
}

f32 GameAnimSet_GetCurrentFrame(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                return object->instance_animation->ltime;
            }
        }
    }
    return 0.0f;
}

f32 GameAnimSet_GetAnimPos(GAMEANIMOBJ *object) {
    f32 position = 0.0f;

    if (object != NULL && object->instance_animation != NULL && object->animation != NULL) {
        if (object->start_frame == object->end_frame) {
            position = 1.0f;
        } else {
            position = (object->instance_animation->ltime - object->start_frame) / (object->end_frame - object->start_frame);
            if (position < 0.0f) {
                position = 0.0f;
            } else if (position > 1.0f) {
                position = 1.0f;
            }
        }
    }
    return position;
}

void GameAnimSet_SetAnimPos(GAMEANIMOBJ *object, f32 position) {
    if (object == NULL || object->instance_animation == NULL || object->animation == NULL) {
        return;
    }
    if (position < 0.0f) {
        position = 0.0f;
    }
    if (position > 1.0f) {
        position = 1.0f;
    }
    object->instance_animation->ltime = (object->end_frame - object->start_frame) * position + object->start_frame;
}

void GameAnimSet_SetOffset(GAMEANIMSET *set, NUVEC *offset) {
    struct gameanimobj_s *object;
    if (set == NULL) {
        return;
    }

    for (object = set->objects; object != NULL; object = object->next) {
        struct numtx_s *source;
        NUMTX matrix;
        if ((object->serialized_flags & 4) != 0) {
            continue;
        }
        source = NuSpecialGetMtx(&object->special);
        if (source == NULL) {
            continue;
        }

        matrix = *source;
        matrix.m30 += offset->x;
        matrix.m31 += offset->y;
        matrix.m32 += offset->z;
        NuSpecialSetDrawMtx(&object->special, &matrix);
    }
}

f32 GameAnimSet_GetCompletionRatio(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;
    f32 ratio = 0.0f;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                if (object->start_frame == object->end_frame) {
                    ratio = 1.0f;
                } else {
                    ratio = (object->instance_animation->ltime - object->start_frame) / (object->end_frame - object->start_frame);
                    if (ratio > 1.0f) {
                        ratio = 1.0f;
                    } else if (ratio < 0.0f) {
                        ratio = 0.0f;
                    }
                }
                break;
            }
        }
    }
    return ratio;
}

GAMEANIMOBJ *GameAnimSet_AddObject(GAMEANIMSET *set, nuhspecial_s *special, f32 start_frame, f32 end_frame, i32 append) {
    GAMEANIMOBJPOOL *pool;
    GAMEANIMOBJ *object = NULL;
    GAMEANIMOBJ *tail;
    i32 object_index;
    f32 last_frame;
    char *name;

    if (set != NULL && set->object_pool != NULL && set->object_pool->free_objects != NULL && special != NULL &&
        NuSpecialExistsFn(special) != 0) {
        ++set->object_count;
        ++set->object_pool->active_count;
        object = set->object_pool->free_objects;
        set->object_pool->free_objects = object->next;
        if (append != 0) {
            object->next = NULL;
            if (set->objects == NULL) {
                set->objects = object;
            } else {
                tail = set->objects;
                while (tail->next != NULL) {
                    tail = tail->next;
                }
                tail->next = object;
            }
        } else {
            object->next = set->objects;
            set->objects = object;
        }

        pool = set->object_pool;
        object_index = object - pool->objects;
        object->object_data = (u8 *)pool->object_data + pool->object_data_size * object_index;
        object->special = *special;
        object->instance_animation = NuSpecialGetInstAnim(&object->special);
        if (object->instance_animation != NULL) {
            object->animation = object->special.scene->instance_animation_data[object->instance_animation->animation_index];
            last_frame = NuSpecialGetAnimEndFrame(&object->special);
            if (last_frame > 0.0f) {
                if (end_frame > last_frame) {
                    object->end_frame = last_frame;
                } else if (end_frame < 1.0f) {
                    object->end_frame = 1.0f;
                } else {
                    object->end_frame = end_frame;
                }
                if (start_frame > last_frame) {
                    object->start_frame = last_frame;
                } else if (start_frame < 1.0f) {
                    object->start_frame = 1.0f;
                } else {
                    object->start_frame = start_frame;
                }
                ++set->animated_object_count;
            } else {
                name = NuSpecialGetName(&object->special);
                if (name != NULL) {
                    GameAnimMissingAnimDiagnostic(name);
                }
            }
        }
    }
    return object;
}

i32 GameAnimSet_GetAveragePos(GAMEANIMSET *set, NUVEC *position, i32 frame_selection, i32 include_animated, i32 include_static) {
    GAMEANIMOBJ *object;
    NUVEC sum = {0.0f, 0.0f, 0.0f};
    i32 count = 0;

    if (set == NULL || position == NULL || set->object_count == 0 || set->objects == NULL) {
        return 0;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        if ((object->flags & 1) != 0) {
            continue;
        }
        if (object->instance_animation != NULL) {
            f32 frame;
            NUMTX matrix;

            if (include_animated == 0) {
                continue;
            }
            switch (frame_selection) {
                case 0:
                    frame = object->start_frame;
                    break;
                case 1:
                    frame = object->end_frame;
                    break;
                default:
                    frame = object->instance_animation->ltime;
                    break;
            }
            EvalAnim(&object->special, frame, &matrix, 1);
            NuVecAdd(&sum, &sum, &matrix.translation);
            ++count;
        } else if (include_static != 0) {
            NuVecAdd(&sum, &sum, NuSpecialGetDrawPos(&object->special));
            ++count;
        }
    }
    if (count == 0) {
        return 0;
    }
    NuVecScale(position, &sum, 1.0f / (f32)count);
    return 1;
}

i32 GameAnimSet_GetVisibility(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;
    i32 visible_count;

    if (set != NULL) {
        visible_count = 0;
        for (object = set->objects; object != NULL; object = object->next) {
            if (NuSpecialGetVisibilityFn(&object->special) != 0) {
                ++visible_count;
            }
        }
        if (visible_count == set->object_count) {
            return 2;
        }
        if (visible_count > 0) {
            return 1;
        }
    }
    return 0;
}

void GameAnimSet_SetVisibility(GAMEANIMSET *set, i32 visibility) {
    GAMEANIMOBJ *object;

    if (set == NULL) {
        return;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        NuSpecialSetVisibility(&object->special, visibility);
    }
}

void GameAnimSet_EvalAnim(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set == NULL) {
        return;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        EvalAnim2(&object->special, object->instance_animation->ltime);
    }
}

f32 GameAnimSet_AutoSetReflectY(GAMEANIMSET *set, NUVEC *position, NUMTX *matrix) {
    if (set != NULL && set->objects != NULL) {
        f32 height;
        f32 ground;
        if (matrix == NULL) {
            matrix = NuSpecialGetMtx(&set->objects->special);
        }
        height = matrix->m31;
        ground = GameShadow(NULL, position, 5.0f, -1);
        if (ground != 2000000.0f) {
            return ground - height;
        }
    }
    return 0.0f;
}

i32 GameAnimSet_GetCentreAndRadius(GAMEANIMSET *set, NUVEC *centre, f32 *radius, i32 frame_selection, i32 include_animated,
                                   i32 include_static) {
    GAMEANIMOBJ *object;
    NUVEC minimum;
    NUVEC maximum;
    i32 found_object = 0;
    NUVEC low;
    NUVEC high;
    NUVEC half_size;
    NUVEC4 object_centre;
    f32 object_radius;
    NUMTX matrix;
    f32 frame;

    if (set != NULL && centre != NULL && set->object_count != 0) {
        minimum.x = 1.0e9f;
        minimum.y = 1.0e9f;
        minimum.z = 1.0e9f;
        maximum.x = -1.0e9f;
        maximum.y = -1.0e9f;
        maximum.z = -1.0e9f;
        for (object = set->objects; object != NULL; object = object->next) {
            if ((object->flags & 1) != 0) {
                continue;
            }
            if (object->instance_animation != NULL) {
                if (include_animated == 0) {
                    continue;
                }
                switch (frame_selection) {
                    case 0:
                        frame = object->start_frame;
                        break;
                    case 1:
                        frame = object->end_frame;
                        break;
                    default:
                        frame = object->instance_animation->ltime;
                        break;
                }
                EvalAnim(&object->special, frame, &matrix, 1);
            } else {
                if (include_static == 0) {
                    continue;
                }
                matrix = *NuSpecialGetDrawMtx(&object->special);
            }
            NuSpecialGetRadius(&object->special, &object_centre.xyz, &object_radius);
            object_radius *= 0.75f;
            object_centre.w = 1.0f;
            NuVec4MtxTransformVU0(&object_centre, &object_centre, &matrix);
            low.x = object_centre.x - object_radius;
            low.y = object_centre.y - object_radius;
            low.z = object_centre.z - object_radius;
            if (low.x < minimum.x) {
                minimum.x = low.x;
            }
            if (low.y < minimum.y) {
                minimum.y = low.y;
            }
            if (low.z < minimum.z) {
                minimum.z = low.z;
            }
            high.x = object_centre.x + object_radius;
            high.y = object_centre.y + object_radius;
            high.z = object_centre.z + object_radius;
            if (high.x > maximum.x) {
                maximum.x = high.x;
            }
            if (high.y > maximum.y) {
                maximum.y = high.y;
            }
            if (high.z > maximum.z) {
                maximum.z = high.z;
            }
            found_object = 1;
        }
        if (found_object != 0) {
            centre->x = (minimum.x + maximum.x) * 0.5f;
            centre->y = (minimum.y + maximum.y) * 0.5f;
            centre->z = (minimum.z + maximum.z) * 0.5f;
            if (radius != NULL) {
                half_size.x = (maximum.x - minimum.x) * 0.5f;
                half_size.y = (maximum.y - minimum.y) * 0.5f;
                half_size.z = (maximum.z - minimum.z) * 0.5f;
                *radius = half_size.x * half_size.x + half_size.y * half_size.y + half_size.z * half_size.z;
                *radius = NuFsqrt(*radius);
            }
            return 1;
        }
    }
    return 0;
}

void GameAnimSys_Update(GAMEANIMSYS *system) {
    GAMEANIMSET *set;
    GAMEANIMSET *next_set;
    GAMEANIMOBJ *object;
    i32 was_no_visibility_test;
    i32 all_playing_forward;
    i32 all_at_start;
    i32 all_at_end;
    i32 any_special_no_visibility_test;
    f32 direction;
    f32 previous_frame;
    f32 overshoot;

    if (system == NULL) {
        return;
    }
    set = (GAMEANIMSET *)NuLinkedListGetHead(&system->active_sets);
    while (set != NULL) {
        next_set = (GAMEANIMSET *)NuLinkedListGetNext(&system->active_sets, &set->links);
        was_no_visibility_test = set->flags & GAMEANIMSET_PLAYING;
        set->flags &= (u16)~GAMEANIMSET_PLAYING;
        if ((set->flags & GAMEANIMSET_STOPPED) != 0) {
            set->flags &= (u16)~GAMEANIMSET_STOPPED;
            GameAnimSet_RemoveFromSystemList(set);
            set = next_set;
            continue;
        }
        set->state = GAMEANIMSET_STATE_AT_START;
        all_playing_forward = 1;
        all_at_start = 1;
        all_at_end = 1;
        any_special_no_visibility_test = 0;
        for (object = set->objects; object != NULL; object = object->next) {
            if (NuSpecialGetNoVisiTestFn(&object->special) != 0) {
                any_special_no_visibility_test = GAMEANIMSET_PLAYING;
            }
            if (object->instance_animation != NULL) {
                direction = object->start_frame > object->end_frame ? -1.0f : 1.0f;
                previous_frame = object->instance_animation->ltime;
                if (object->instance_animation->ltime * direction >= object->end_frame * direction) {
                    if (object->instance_animation->tfactor * direction < 0.0f) {
                        object->instance_animation->ltime = object->end_frame;
                    } else if ((object->instance_animation->flags & NUINSTANIM_FLAG_REPEATING) != 0) {
                        overshoot = object->instance_animation->ltime - object->end_frame;
                        object->instance_animation->ltime = overshoot + object->start_frame;
                    } else {
                        object->instance_animation->ltime = object->end_frame;
                        object->instance_animation->flags &= ~NUINSTANIM_FLAG_PLAYING;
                    }
                } else if (object->instance_animation->ltime * direction <= object->start_frame * direction) {
                    if (object->instance_animation->tfactor * direction > 0.0f) {
                        object->instance_animation->ltime = object->start_frame;
                    } else if ((object->instance_animation->flags & NUINSTANIM_FLAG_REPEATING) != 0) {
                        overshoot = object->start_frame - object->instance_animation->ltime;
                        object->instance_animation->ltime = object->end_frame - overshoot;
                    } else {
                        object->instance_animation->ltime = object->start_frame;
                        object->instance_animation->flags &= ~NUINSTANIM_FLAG_PLAYING;
                    }
                }
                if (previous_frame != object->instance_animation->ltime) {
                    EvalAnim2(&object->special, object->instance_animation->ltime);
                }
                if ((object->instance_animation->flags & NUINSTANIM_FLAG_PLAYING) != 0) {
                    set->flags |= GAMEANIMSET_PLAYING;
                    if (object->instance_animation->tfactor * direction < 0.0f) {
                        all_playing_forward = 0;
                    }
                }
                if (object->instance_animation->ltime * direction > object->start_frame * direction) {
                    all_at_start = 0;
                }
                if (object->instance_animation->ltime * direction < object->end_frame * direction) {
                    all_at_end = 0;
                }
            }
        }
        if ((set->flags & GAMEANIMSET_PLAYING) != any_special_no_visibility_test) {
            for (object = set->objects; object != NULL; object = object->next) {
                NuSpecialSetNoVisiTest(&object->special, set->flags & GAMEANIMSET_PLAYING);
            }
        }
        if ((set->flags & GAMEANIMSET_PLAYING) != 0) {
            set->state = all_playing_forward != 0 ? 1 : 3;
        } else {
            if (all_at_end != 0) {
                set->state = GAMEANIMSET_STATE_AT_END;
            } else if (all_at_start == 0) {
                set->state = GAMEANIMSET_STATE_PART_WAY;
            }
            if (was_no_visibility_test != 0) {
                set->flags |= GAMEANIMSET_STOPPED;
            }
        }
        if ((set->flags & (GAMEANIMSET_PLAYING | GAMEANIMSET_STOPPED)) == 0) {
            GameAnimSet_RemoveFromSystemList(set);
        }
        set = next_set;
    }
}

GAMEANIMOBJ *GameAnimSet_AddObjectByName(GAMEANIMSET *set, NUGSCN *scene, char *name, f32 start_frame, f32 end_frame, i32 append,
                                         void *gizmo_sys, char *prefix, char *suffix) {
    nuhspecial_s special;
    if (set && Gizmo_FindNuSpecial(scene, &special, name, 1, gizmo_sys, prefix, suffix)) {
        return GameAnimSet_AddObject(set, &special, start_frame, end_frame, append);
    }
    return NULL;
}

void GameAnimSet_RemoveSpecial(GAMEANIMSET *set, nuhspecial_s *special) {
    struct gameanimobj_s *object;
    if (set == NULL || special == NULL) {
        return;
    }
    object = set->objects;
    while (object != NULL && NuSpecialCompare(&object->special, special) == 0) {
        object = object->next;
    }
    if (object != NULL) {
        GameAnimSet_RemoveObject(set, object);
    }
}

typedef struct gameanim_reflection_matrix_s { // size 0x40
    f32 m00;                                  // 0x00
    f32 m01;                                  // 0x04
    f32 m02;                                  // 0x08
    f32 m03;                                  // 0x0c
    f32 m10;                                  // 0x10
    f32 m11;                                  // 0x14
    f32 m12;                                  // 0x18
    f32 m13;                                  // 0x1c
    f32 m20;                                  // 0x20
    f32 m21;                                  // 0x24
    f32 m22;                                  // 0x28
    f32 axis_coordinates[5];                  // 0x2c
} GAMEANIM_REFLECTION_MATRIX;

void GameAnimSet_DrawReflection(GAMEANIMSET *set, i32 axis, f32 offset, NUMTX *matrix) {
    f32 plane;
    GAMEANIMOBJ *object;
    NUMTX reflection;
    if (!set || !set->objects) {
        return;
    }
    if (!matrix) {
        matrix = NuSpecialGetMtx(&set->objects->special);
    }
    plane = offset + ((GAMEANIM_REFLECTION_MATRIX *)matrix)->axis_coordinates[axis];
    NuRndrStartReflectionRender(0);
    for (object = set->objects; object; object = object->next) {
        if (!(object->flags & 2) && NuSpecialGetVisibilityFn(&object->special)) {
            NUMTX *draw_matrix = NuSpecialGetDrawMtx(&object->special);
            if (MatrixReflection(draw_matrix, axis, plane, WORLD->current_level->reflection_y, &reflection)) {
                NuSpecialDrawAt(&object->special, &reflection);
            }
        }
    }
    NuRndrEndReflectionRender();
}
