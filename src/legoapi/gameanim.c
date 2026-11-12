#include <stdio.h>
#include <string.h>
#include "saga.h"

extern void __cdecl NuSpecialSetVisibility(NUSPECIALHANDLE *special, i32 visible);
void __cdecl GameAnimMissingAnimDiagnosticPc(char *name);

void __cdecl GameAnimSys_AllocateLevelProgressData(VARIPTR *buf, VARIPTR *end, i32 capacity, i32 level_count) {
    i32 i;

    if (buf == NULL || end == NULL) {
        return;
    }
    gameanimsysprogress.count = level_count;
    gameanimsysprogress.entry_size = capacity;
    gameanimsysprogress.entries = (u8 **)GameBufferAlloc(buf, end, level_count * sizeof(u8 *));
    if (gameanimsysprogress.entries != NULL) {
        for (i = 0; i < level_count; ++i) {
            gameanimsysprogress.entries[i] = (u8 *)GameBufferAlloc(buf, end, capacity);
        }
    }
}

void __cdecl GameAnimSys_StoreProgress(GAMEANIMSYS *system, i32 index) {
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

u8 *__cdecl GameAnimSys_GetProgressData(i32 index) {
    if (index < 0 || index >= gameanimsysprogress.count) {
        return NULL;
    }
    return gameanimsysprogress.entries[index];
}

void __cdecl GameAnimSys_ClearProgress(i32 index) {
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

GAMEANIMSYS *__cdecl GameAnimSys_Create(VARIPTR *buf, VARIPTR *end) {
    GAMEANIMSYS *system = (GAMEANIMSYS *)GameBufferAlloc(buf, end, sizeof(GAMEANIMSYS));

    if (system != NULL && gameanimsysprogress.entry_size != 0) {
        system->sets =
            (GAMEANIMSET **)GameBufferAlloc(buf, end, gameanimsysprogress.entry_size * sizeof(GAMEANIMSET *));
    }
    return system;
}

GAMEANIMSET *__cdecl GameAnimSet_Create(VARIPTR *buf, VARIPTR *end, GAMEANIMOBJPOOL *pool, GAMEANIMSYS *system) {
    GAMEANIMSET *set = NULL;
    i32 index;

    if (pool != NULL) {
        set = (GAMEANIMSET *)GameBufferAlloc(buf, end, sizeof(GAMEANIMSET));
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

void __cdecl GameAnimSet_AddToSystemList(GAMEANIMSET *set) {
    if (set != NULL && set->system != NULL && (set->flags & 4) == 0) {
        NuLinkedListAppend(&set->system->active_sets, &set->links);
        set->flags = (u16)(set->flags | 4);
    }
}

void __cdecl GameAnimSet_EvaluateState(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;
    NUINSTANIM *animation;
    i32 all_at_end = 1;
    i32 all_at_start = 1;
    f32 direction;

    if ((set->flags & 4) != 0) {
        return;
    }
    object = set->objects;
    while (object != NULL) {
        animation = object->instance_animation;
        if (animation != NULL) {
            direction = object->start_frame > object->end_frame ? nu_fsign_minus_one : 1.0f;
            if (animation->ltime * direction < object->end_frame * direction) {
                all_at_end = 0;
            }
            if (object->start_frame * direction < animation->ltime * direction) {
                all_at_start = 0;
            }
        }
        object = object->next;
    }
    set->state = 0;
    if (all_at_end != 0) {
        set->state = 2;
    } else if (all_at_start == 0) {
        set->state = 4;
    }
}

void __cdecl GameAnimSet_RemoveFromSystemList(GAMEANIMSET *set) {
    if (set != NULL && set->system != NULL) {
        if ((set->flags & 4) != 0) {
            NuLinkedListRemove(&set->system->active_sets, &set->links);
            set->flags = (u16)(set->flags & ~4);
        }
        GameAnimSet_EvaluateState(set);
    }
}

void __cdecl GameAnimSys_ReStoreProgress(GAMEANIMSYS *system, i32 index) {
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
        if (system->sets[i]->state == 1 || system->sets[i]->state == 3) {
            if ((system->sets[i]->flags & 4) == 0) {
                GameAnimSet_AddToSystemList(system->sets[i]);
            }
        } else if ((system->sets[i]->flags & 4) != 0) {
            GameAnimSet_RemoveFromSystemList(system->sets[i]);
        }
    }
}

GAMEANIMOBJPOOL *__cdecl GameAnimSet_CreateObjectPool(VARIPTR *buf, VARIPTR *end, i32 object_data_size, i32 capacity) {
    GAMEANIMOBJPOOL *pool = NULL;
    i32 i;

    if (capacity != 0) {
        pool = (GAMEANIMOBJPOOL *)GameBufferAlloc(buf, end, sizeof(GAMEANIMOBJPOOL));
        if (pool != NULL) {
            pool->capacity = (u16)capacity;
            pool->object_data_size = (u16)object_data_size;
            pool->objects = (GAMEANIMOBJ *)GameBufferAlloc(buf, end, (u16)capacity * sizeof(GAMEANIMOBJ));
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

void __cdecl GameAnimSet_RemoveObject(GAMEANIMSET *set, GAMEANIMOBJ *object) {
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

void __cdecl GameAnimSet_RemoveAllObjects(GAMEANIMSET *set) {
    if (set != NULL) {
        while (set->objects != NULL) {
            GameAnimSet_RemoveObject(set, set->objects);
        }
    }
}

void __cdecl GameAnimSet_SetTFactor(GAMEANIMSET *set, f32 factor) {
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

void __cdecl GameAnimSet_ScaleFParam1(GAMEANIMSET *set, f32 scale) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->fparam1 *= scale;
            }
        }
    }
}

i32 __cdecl GameAnimSet_IsAnimationReset(GAMEANIMSET *set) {
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
            if ((animation->flags & 8) != 0) {
                ++repeating_count;
                continue;
            }
            if ((animation->flags & 1) != 0) {
                return 0;
            }
            direction = object->start_frame > object->end_frame ? nu_fsign_minus_one : 1.0f;
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

i32 __cdecl GameAnimSet_Reset(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->flags &= ~1u;
                object->instance_animation->flags &= ~4u;
                object->instance_animation->ltime = object->start_frame;
                object->instance_animation->tfactor = 1.0f;
            }
            GameAnimSet_RemoveFromSystemList(set);
        }
    }
    return 1;
}

i32 __cdecl GameAnimSet_Play(GAMEANIMSET *set, f32 speed, i32 evaluate_state) {
    GAMEANIMOBJ *object;
    f32 direction;

    if (set == NULL) {
        return 1;
    }
    if (evaluate_state != 0) {
        GameAnimSet_EvaluateState(set);
    }
    set->flags = (u16)(set->flags & ~2);
    if (speed >= 0.0f) {
        if (set->state == 2) {
            return 1;
        }
    } else if (speed < 0.0f) {
        if (set->state == 0) {
            return 1;
        }
    }
    for (object = set->objects; object != NULL; object = object->next) {
        if (object->instance_animation != NULL) {
            object->instance_animation->flags |= 1;
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
    if ((set->flags & 4) == 0) {
        GameAnimSet_AddToSystemList(set);
    }
    return 1;
}

i32 __cdecl GameAnimSet_Stop(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set != NULL && (set->state == 1 || set->state == 3)) {
        set->flags = (u16)(set->flags | 2);
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->flags &= ~1u;
            }
        }
    }
    return 1;
}

i32 __cdecl GameAnimSet_SetRepeating(GAMEANIMSET *set, i32 repeating) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->flags =
                    (object->instance_animation->flags & ~8u) | (((u32)repeating & 1) << 3);
            }
        }
    }
    return 1;
}

i32 __cdecl GameAnimSet_JumpToEnd(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->flags &= ~1u;
                object->instance_animation->ltime = object->end_frame;
            }
        }
    }
    return 1;
}

i32 __cdecl GameAnimSet_JumpToStart(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                object->instance_animation->flags &= ~1u;
                object->instance_animation->ltime = object->start_frame;
            }
        }
    }
    return 1;
}

void __cdecl GameAnimSet_JumpToAnimPos(GAMEANIMSET *set, f32 position) {
    GAMEANIMOBJ *object;
    f32 direction;

    if (set == NULL) {
        return;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        if (object->instance_animation != NULL && object->animation != NULL) {
            object->instance_animation->flags &= ~1u;
            object->instance_animation->ltime =
                (object->end_frame - object->start_frame) * position + object->start_frame;
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

i32 __cdecl GameAnimSet_Playing(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL && (object->instance_animation->flags & 1) == 0) {
                return 0;
            }
        }
    }
    return 1;
}

f32 __cdecl GameAnimSet_GetCurrentFrame(GAMEANIMSET *set) {
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

f32 __cdecl GameAnimSet_GetAnimPos(GAMEANIMOBJ *object) {
    f32 position = 0.0f;

    if (object != NULL && object->instance_animation != NULL && object->animation != NULL) {
        if (object->start_frame == object->end_frame) {
            position = 1.0f;
        } else {
            position =
                (object->instance_animation->ltime - object->start_frame) / (object->end_frame - object->start_frame);
            if (position < 0.0f) {
                position = 0.0f;
            } else if (position > 1.0f) {
                position = 1.0f;
            }
        }
    }
    return position;
}

void __cdecl GameAnimSet_SetAnimPos(GAMEANIMOBJ *object, f32 position) {
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

f32 __cdecl GameAnimSet_GetCompletionRatio(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;
    f32 ratio = 0.0f;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            if (object->instance_animation != NULL) {
                if (object->start_frame == object->end_frame) {
                    ratio = 1.0f;
                } else {
                    ratio = (object->instance_animation->ltime - object->start_frame) /
                            (object->end_frame - object->start_frame);
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

GAMEANIMOBJ *__cdecl GameAnimSet_AddObject(GAMEANIMSET *set, NUSPECIALHANDLE *special, f32 start_frame, f32 end_frame,
                                           i32 append) {
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
        object_index = (i32)(object - pool->objects);
        object->object_data = (u8 *)pool->object_data + pool->object_data_size * object_index;
        object->special = *special;
        object->instance_animation = (NUINSTANIM *)NuSpecialGetInstAnim(&object->special);
        if (object->instance_animation != NULL) {
            object->animation =
                ((NUGSCN *)object->special.scene)->instance_animation_data[object->instance_animation->animation_index];
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
                    GameAnimMissingAnimDiagnosticPc(name);
                }
            }
        }
    }
    return object;
}

i32 __cdecl GameAnimSet_GetAveragePos(GAMEANIMSET *set, NUVEC *position, i32 frame_selection, i32 include_animated,
                                      i32 include_static) {
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

i32 __cdecl GameAnimSet_GetVisibility(GAMEANIMSET *set) {
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

void __cdecl GameAnimSet_SetVisibility(GAMEANIMSET *set, i32 visibility) {
    GAMEANIMOBJ *object;

    if (set == NULL) {
        return;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        NuSpecialSetVisibility(&object->special, visibility);
    }
}

void __cdecl GameAnimSet_EvalAnim(GAMEANIMSET *set) {
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

i32 __cdecl GameAnimSet_GetCentreAndRadius(GAMEANIMSET *set, NUVEC *centre, f32 *radius, i32 frame_selection,
                                           i32 include_animated, i32 include_static) {
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
            NuSpecialGetRadius(&object->special, (NUVEC *)&object_centre, &object_radius);
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

void __cdecl GameAnimSys_Update(GAMEANIMSYS *system) {
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
        was_no_visibility_test = set->flags & 1;
        set->flags &= (u16)~1;
        if ((set->flags & 2) != 0) {
            set->flags &= (u16)~2;
            GameAnimSet_RemoveFromSystemList(set);
            set = next_set;
            continue;
        }
        set->state = 0;
        all_playing_forward = 1;
        all_at_start = 1;
        all_at_end = 1;
        any_special_no_visibility_test = 0;
        for (object = set->objects; object != NULL; object = object->next) {
            if (NuSpecialGetNoVisiTestFn(&object->special) != 0) {
                any_special_no_visibility_test = 1;
            }
            if (object->instance_animation != NULL) {
                direction = object->start_frame > object->end_frame ? -1.0f : 1.0f;
                previous_frame = object->instance_animation->ltime;
                if (object->instance_animation->ltime * direction >= object->end_frame * direction) {
                    if (object->instance_animation->tfactor * direction < 0.0f) {
                        object->instance_animation->ltime = object->end_frame;
                    } else if ((object->instance_animation->flags & 8) != 0) {
                        overshoot = object->instance_animation->ltime - object->end_frame;
                        object->instance_animation->ltime = overshoot + object->start_frame;
                    } else {
                        object->instance_animation->ltime = object->end_frame;
                        object->instance_animation->flags &= ~1u;
                    }
                } else if (object->instance_animation->ltime * direction <= object->start_frame * direction) {
                    if (object->instance_animation->tfactor * direction > 0.0f) {
                        object->instance_animation->ltime = object->start_frame;
                    } else if ((object->instance_animation->flags & 8) != 0) {
                        overshoot = object->start_frame - object->instance_animation->ltime;
                        object->instance_animation->ltime = object->end_frame - overshoot;
                    } else {
                        object->instance_animation->ltime = object->start_frame;
                        object->instance_animation->flags &= ~1u;
                    }
                }
                if (previous_frame != object->instance_animation->ltime) {
                    EvalAnim2(&object->special, object->instance_animation->ltime);
                }
                if ((object->instance_animation->flags & 1) != 0) {
                    set->flags |= 1;
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
        if ((set->flags & 1) != any_special_no_visibility_test) {
            for (object = set->objects; object != NULL; object = object->next) {
                NuSpecialSetNoVisiTest(&object->special, set->flags & 1);
            }
        }
        if ((set->flags & 1) != 0) {
            set->state = all_playing_forward != 0 ? 1 : 3;
        } else {
            if (all_at_end != 0) {
                set->state = 2;
            } else if (all_at_start == 0) {
                set->state = 4;
            }
            if (was_no_visibility_test != 0) {
                set->flags |= 2;
            }
        }
        if ((set->flags & (1 | 2)) == 0) {
            GameAnimSet_RemoveFromSystemList(set);
        }
        set = next_set;
    }
}

void __cdecl edanimParamReset(void) {
    i32 i;

    for (i = 0; i < 64; ++i) {
        AnimParams[i].instance_id = -1;
    }
    edanim_next_param = 0;
    edanim_params_used = 0;
    for (i = 0; i < 8; ++i) {
        edanim_page_used[i] = 0;
        edanim_page_on[i] = 0;
    }
}

void __cdecl edanimStartPage(i32 page) {
    if (edanim_page_used[page] != 0 && edanim_page_scene[page] != NULL && edanim_page_on[page] == 0) {
        edanim_page_on[page] = 1;
    }
}

void __cdecl edanimStopPage(i32 page) {
    edanim_page_on[page] = 0;
}

void __cdecl edanimClearPage(i32 page) {
    i32 index;

    if (edanim_page_on[page] != 0) {
        edanimStopPage(page);
    }
    for (index = 0; index < 64; ++index) {
        if (AnimParams[index].page == page) {
            AnimParams[index].instance_id = -1;
            --edanim_params_used;
        }
    }
    edanim_page_used[page] = 0;
    edanim_page_scene[page] = NULL;
}

void __cdecl edanimStartAllPages(void) {
    i32 page;

    for (page = 0; page < 8; ++page) {
        edanimStartPage(page);
    }
}

void EdFileWrite(void *data, i32 len);
void EdFileWriteInt(i32 value);
void EdFileWriteShort(i16 value);
void EdFileWriteNuVec(NUVEC *value);
extern struct eduimenu_s *edanim_switch_menu;

i32 edanimFileSave(char *path) {
    NUSPECIALHANDLE special;
    char name[20];
    i32 count, i, index;
    count = 0;
    for (i = 0; i < 64; ++i) {
        if (AnimParams[i].instance_id != -1)
            ++count;
    }
    EdFileSetMedia(1);
    if (EdFileOpen(path, 1)) {
        EdFileSetReadWrongEndianess(1);
        EdFileWriteInt(6);
        EdFileWriteInt(count);
        for (i = 0; i < 64; ++i) {
            if (AnimParams[i].instance_id == -1)
                continue;
            NuGScnGetSpecial(&special, edbits_base_scene, AnimParams[i].instance_id);
            strncpy(name, NuSpecialGetName(&special), sizeof(name));
            name[sizeof(name) - 1] = 0;
            EdFileWrite(name, sizeof(name));
            EdFileWriteInt(AnimParams[i].effect_count);
            EdFileWriteInt(AnimParams[i].sound_count);
            EdFileWriteInt(AnimParams[i].control_mode);
            EdFileWriteInt(AnimParams[i].control_switch);
            EdFileWriteFloat(AnimParams[i].control_distance);
            EdFileWriteFloat(AnimParams[i].field_018);
            for (index = 0; index < AnimParams[i].effect_count; ++index) {
                EdFileWrite(AnimParams[i].effect_names[index], sizeof(AnimParams[i].effect_names[index]));
                EdFileWriteInt(AnimParams[i].effect_intervals[index]);
                EdFileWriteInt(AnimParams[i].effect_flags[index]);
                EdFileWriteNuVec((NUVEC *)AnimParams[i].effect_positions[index]);
                EdFileWriteShort(AnimParams[i].effect_angles[index]);
                EdFileWriteShort(AnimParams[i].effect_angle_ranges[index]);
            }
            for (index = 0; index < AnimParams[i].sound_count; ++index) {
                EdFileWrite(AnimParams[i].sound_names[index], sizeof(AnimParams[i].sound_names[index]));
                EdFileWriteInt(AnimParams[i].sound_flags[index]);
                EdFileWriteFloat(AnimParams[i].sound_values[index]);
                EdFileWriteNuVec((NUVEC *)AnimParams[i].sound_positions[index]);
            }
            EdFileWriteFloat(AnimParams[i].bounce_impulse);
            EdFileWriteFloat(AnimParams[i].bounce_spring);
            EdFileWriteFloat(AnimParams[i].bounce_damping);
        }
        EdFileClose();
        EdFileSetReadWrongEndianess(0);
        return 1;
    }
    return 0;
}

void __cdecl edanimRegisterBaseScene(NUGSCN *scene) {
    (void)scene;
}

extern struct eduimenu_s *edanim_switchtype_menu;

void edanimcbSetSwitchType(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
    edanim_switchtype_menu = NULL;
    AnimParams[edanim_nearest_param_id].control_mode = item->data;
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

void edanimcbSetSwitchId(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edanim_nearest == -1 || edanim_nearest_param_id == -1)
        return;
    AnimParams[edanim_nearest_param_id].control_switch = (i32)((edui_slider_s *)item)->value;
}

void edanimcbSetSwitchDelay(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edanim_nearest == -1 || edanim_nearest_param_id == -1)
        return;
    AnimParams[edanim_nearest_param_id].field_018 = ((edui_slider_s *)item)->value;
}

void edanimcbSetSwitchVar(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edanim_nearest == -1 || edanim_nearest_param_id == -1)
        return;
    AnimParams[edanim_nearest_param_id].control_distance = ((edui_slider_s *)item)->value;
}

static void edanimcbCancelSwitchTypeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edanim_switchtype_menu);
    edanim_switchtype_menu = NULL;
}

void edanimcbCancelSwitchMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edanim_switch_menu);
    edanim_switch_menu = NULL;
}

void edanimcbSwitchTypeMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    edanim_switchtype_menu =
        eduiMenuCreate(70, 70, 250, 200, ed_fnt, edanimcbCancelSwitchTypeMenu, (char *)("Switch Type"));
    if (!edanim_switchtype_menu)
        return;
    eduiMenuAddItem(edanim_switchtype_menu,
                    eduiItemCheckCreate(0, colours, AnimParams[edanim_nearest_param_id].control_mode == 0, 1,
                                        edanimcbSetSwitchType, (char *)("None")));
    if (edui_last_item->highlighted)
        edanim_switchtype_menu->selected = edui_last_item;
    eduiMenuAddItem(edanim_switchtype_menu,
                    eduiItemCheckCreate(1, colours, AnimParams[edanim_nearest_param_id].control_mode == 1, 1,
                                        edanimcbSetSwitchType, (char *)("Switch")));
    if (edui_last_item->highlighted)
        edanim_switchtype_menu->selected = edui_last_item;
    eduiMenuAddItem(edanim_switchtype_menu,
                    eduiItemCheckCreate(2, colours, AnimParams[edanim_nearest_param_id].control_mode == 2, 1,
                                        edanimcbSetSwitchType, (char *)("Switch One Cycle")));
    if (edui_last_item->highlighted)
        edanim_switchtype_menu->selected = edui_last_item;
    eduiMenuAddItem(edanim_switchtype_menu,
                    eduiItemCheckCreate(3, colours, AnimParams[edanim_nearest_param_id].control_mode == 3, 1,
                                        edanimcbSetSwitchType, (char *)("Switch Continuous")));
    if (edui_last_item->highlighted)
        edanim_switchtype_menu->selected = edui_last_item;
    eduiMenuAddItem(edanim_switchtype_menu,
                    eduiItemCheckCreate(4, colours, AnimParams[edanim_nearest_param_id].control_mode == 4, 1,
                                        edanimcbSetSwitchType, (char *)("Proximity")));
    if (edui_last_item->highlighted)
        edanim_switchtype_menu->selected = edui_last_item;
    eduiMenuAddItem(edanim_switchtype_menu,
                    eduiItemCheckCreate(5, colours, AnimParams[edanim_nearest_param_id].control_mode == 5, 1,
                                        edanimcbSetSwitchType, (char *)("Proximity One Cycle")));
    if (edui_last_item->highlighted)
        edanim_switchtype_menu->selected = edui_last_item;
    eduiMenuAddItem(edanim_switchtype_menu,
                    eduiItemCheckCreate(6, colours, AnimParams[edanim_nearest_param_id].control_mode == 6, 1,
                                        edanimcbSetSwitchType, (char *)("Proximity Continuous")));
    if (edui_last_item->highlighted)
        edanim_switchtype_menu->selected = edui_last_item;
    eduiMenuAddItem(edanim_switchtype_menu,
                    eduiItemCheckCreate(7, colours, AnimParams[edanim_nearest_param_id].control_mode == 7, 1,
                                        edanimcbSetSwitchType, (char *)("Terrain")));
    if (edui_last_item->highlighted)
        edanim_switchtype_menu->selected = edui_last_item;
    eduiMenuAddItem(edanim_switchtype_menu,
                    eduiItemCheckCreate(8, colours, AnimParams[edanim_nearest_param_id].control_mode == 8, 1,
                                        edanimcbSetSwitchType, (char *)("Terrain One Cycle")));
    if (edui_last_item->highlighted)
        edanim_switchtype_menu->selected = edui_last_item;
    eduiMenuAddItem(edanim_switchtype_menu,
                    eduiItemCheckCreate(9, colours, AnimParams[edanim_nearest_param_id].control_mode == 9, 1,
                                        edanimcbSetSwitchType, (char *)("Terrain Continuous")));
    if (edui_last_item->highlighted)
        edanim_switchtype_menu->selected = edui_last_item;
    eduiMenuAddItem(edanim_switchtype_menu,
                    eduiItemCheckCreate(10, colours, AnimParams[edanim_nearest_param_id].control_mode == 10, 1,
                                        edanimcbSetSwitchType, (char *)("Override NoAnim")));
    if (edui_last_item->highlighted)
        edanim_switchtype_menu->selected = edui_last_item;
    eduiMenuAddItem(edanim_switchtype_menu,
                    eduiItemCheckCreate(11, colours, AnimParams[edanim_nearest_param_id].control_mode == 11, 1,
                                        edanimcbSetSwitchType, (char *)("Override Play")));
    if (edui_last_item->highlighted)
        edanim_switchtype_menu->selected = edui_last_item;
    eduiMenuAddItem(edanim_switchtype_menu,
                    eduiItemCheckCreate(12, colours, AnimParams[edanim_nearest_param_id].control_mode == 12, 1,
                                        edanimcbSetSwitchType, (char *)("Override PlayCont")));
    if (edui_last_item->highlighted)
        edanim_switchtype_menu->selected = edui_last_item;
    eduiMenuAttach(parent, edanim_switchtype_menu);
    edanim_switchtype_menu->x = parent->x + 10;
    edanim_switchtype_menu->y = parent->y + 40;
}

void edanimcbSwitchMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    if (edanim_nearest == -1 || edanim_nearest_param_id == -1)
        return;
    edanim_switch_menu = eduiMenuCreate(70, 70, 180, 250, ed_fnt, edanimcbCancelSwitchMenu, (char *)("Switch Menu"));
    if (!edanim_switch_menu)
        return;
    eduiMenuAddItem(edanim_switch_menu,
                    eduiItemSelCreate(1, colours, 0, 0, edanimcbSwitchTypeMenu, (char *)("Switch Type...")));
    eduiMenuAddItem(edanim_switch_menu,
                    eduiItemSliderCreateInt(0, colours, 0, edanimcbSetSwitchId, -1, 129,
                                            AnimParams[edanim_nearest_param_id].control_switch, (char *)("Switch ID")));
    eduiMenuAddItem(edanim_switch_menu,
                    eduiItemSliderCreate(0, colours, 0, edanimcbSetSwitchDelay, 0.0f, 20.0f,
                                         AnimParams[edanim_nearest_param_id].field_018, (char *)("Switch Delay")));
    eduiMenuAddItem(edanim_switch_menu,
                    eduiItemSliderCreate(0, colours, 0, edanimcbSetSwitchVar, 0.0f, 20.0f,
                                         AnimParams[edanim_nearest_param_id].control_distance, (char *)("Switch Var")));
    eduiMenuAttach(parent, edanim_switch_menu);
    edanim_switch_menu->x = parent->x + 10;
    edanim_switch_menu->y = parent->y + 40;
}

void edanimcbToggleSoundType(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
    edanim_soundtype_menu = NULL;
    if (!(item->highlighted & 1)) {
        AnimParams[edanim_nearest_param_id].sound_flags[edanim_nearest_sound] = 0;
        AnimParams[edanim_nearest_param_id].sound_values[edanim_nearest_sound] = 1.0f;
    } else {
        AnimParams[edanim_nearest_param_id].sound_flags[edanim_nearest_sound] = 1;
        AnimParams[edanim_nearest_param_id].sound_values[edanim_nearest_sound] = 50.0f;
    }
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

void edanimcbCancelBouncyMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edanim_bouncy_menu);
    edanim_bouncy_menu = NULL;
}

void edanimcbSetSoundType(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
    edanim_soundtype_menu = NULL;
    edanim_sound_type = item->data == 0x1869f ? -1 : item->data;
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

void edanimcbSetSoundTiming(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    AnimParams[edanim_nearest_param_id].sound_values[edanim_nearest_sound] = ((edui_slider_s *)item)->value;
}

static void edanimcbToggleParticleSwitch(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    if (edanim_nearest_param_id == -1 || edanim_nearest_particle == -1)
        return;
    AnimParams[edanim_nearest_param_id].effect_flags[edanim_nearest_particle] = item->highlighted & 1;
}

void edanimcbSetParticleType(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
    edanim_particletype_menu = NULL;
    edanim_particle_type = item->data == 0 ? -1 : item->data;
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

void edanimcbSetLocalParticleType(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
    edanim_localparticletype_menu = NULL;
    AnimParams[edanim_nearest_param_id].effect_ids[edanim_nearest_particle] = item->data;
    strcpy(AnimParams[edanim_nearest_param_id].effect_names[edanim_nearest_particle],
           debtab[edanim_nearest_particle]->name);
    edanim_particle_type = item->data;
    eduiMenuDetach(menu);
    eduiMenuDestroy(menu);
}

static void edanimcbSetParticleRate(eduimenu_s *unused0, eduiitem_s *item, u32 unused2) {
    AnimParams[edanim_nearest_param_id].effect_intervals[edanim_nearest_particle] =
        ((i32)(((edui_slider_s *)(item))->value));
}

void edanimcbCancelParticleTypeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edanim_particletype_menu);
    edanim_particletype_menu = NULL;
}
void edanimcbLocalParticleTypeMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    i32 index;
    i32 selected;
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    edanim_localparticletype_menu = eduiMenuCreate(70, 70, 250, 200, ed_fnt, edanimcbCancelLocalParticleTypeMenu,
                                                   ((char *)("Highlighted Particle Type")));
    if (!edanim_localparticletype_menu) {
        return;
    }

    for (index = 1; index < debinf_count; ++index) {
        if (!debtab[index]) {
            continue;
        }
        selected = AnimParams[edanim_nearest_param_id].effect_ids[edanim_nearest_particle] == index;
        eduiMenuAddItem(
            edanim_localparticletype_menu,
            eduiItemCheckCreate(index, colours, selected, 1, edanimcbSetLocalParticleType, debtab[index]->name));
        if (selected == 1) {
            edanim_localparticletype_menu->selected = edui_last_item;
        }
    }

    eduiMenuAttach(parent, edanim_localparticletype_menu);
    edanim_localparticletype_menu->x = parent->x + 10;
    edanim_localparticletype_menu->y = parent->y + 40;
}

static void edanimcbCancelLocalParticleMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edanim_localparticle_menu);
    edanim_localparticle_menu = NULL;
}

void edanimcbLocalParticleMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    if (edanim_nearest_particle != -1) {
        edanim_localparticle_menu = eduiMenuCreate(70, 70, 250, 300, ed_fnt, edanimcbCancelLocalParticleMenu,
                                                   ((char *)("Highlighted Particle Settings")));
        if (!edanim_localparticle_menu) {
            return;
        }

        eduiMenuAddItem(edanim_localparticle_menu, eduiItemSelCreate(1, colours, 0, 0, edanimcbLocalParticleTypeMenu,
                                                                     ((char *)("Highlighted Particle Type..."))));
        eduiMenuAddItem(
            edanim_localparticle_menu,
            eduiItemSliderCreateInt(0, colours, 0, edanimcbSetParticleRate, 0, 300,
                                    AnimParams[edanim_nearest_param_id].effect_intervals[edanim_nearest_particle],
                                    ((char *)("Particles Per Sec"))));
        eduiMenuAddItem(edanim_localparticle_menu,
                        eduiItemToggleCreate(0, colours,
                                             AnimParams[edanim_nearest_param_id].effect_flags[edanim_nearest_particle],
                                             1, edanimcbToggleParticleSwitch, ((char *)("Only On Moving"))));

        eduiMenuAttach(parent, edanim_localparticle_menu);
        edanim_localparticle_menu->x = parent->x + 10;
        edanim_localparticle_menu->y = parent->y + 40;
    }
}

void edanimcbCancelParticleMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edanim_particle_menu);
    edanim_particle_menu = NULL;
}

void edanimcbCancelSoundTypeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edanim_soundtype_menu);
    edanim_soundtype_menu = NULL;
}

extern struct eduimenu_s *edanim_localsound_menu;

void edanimcbCancelLocalSoundMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edanim_localsound_menu);
    edanim_localsound_menu = NULL;
}

void edanimcbCancelLocalSoundTypeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edanim_localsoundtype_menu);
    edanim_localsoundtype_menu = NULL;
}

void edanimcbCancelSoundMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edanim_sound_menu);
    edanim_sound_menu = NULL;
}
void edanimcbParticleTypeMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    i32 index;
    i32 selected;
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    edanim_particletype_menu =
        eduiMenuCreate(70, 70, 250, 250, ed_fnt, edanimcbCancelParticleTypeMenu, (char *)("Particle Type"));
    if (!edanim_particletype_menu) {
        return;
    }

    eduiMenuAddItem(edanim_particletype_menu, eduiItemCheckCreate(0, colours, edanim_particle_type == -1, 0,
                                                                  edanimcbSetParticleType, (char *)("NONE")));
    for (index = 1; index < debinf_count; ++index) {
        if (!debtab[index]) {
            continue;
        }
        selected = edanim_particle_type == index;
        eduiMenuAddItem(edanim_particletype_menu,
                        eduiItemCheckCreate(index, colours, selected, 1, edanimcbSetParticleType, debtab[index]->name));
        if (selected == 1) {
            edanim_particletype_menu->selected = edui_last_item;
        }
    }

    eduiMenuAttach(parent, edanim_particletype_menu);
    edanim_particletype_menu->x = parent->x + 10;
    edanim_particletype_menu->y = parent->y + 40;
}

static void edanimcbCancelLocalParticleTypeMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edanim_localparticletype_menu);
    edanim_localparticletype_menu = NULL;
}
void edanimcbParticleMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    if (edanim_nearest_param_id == -1) {
        return;
    }

    edanim_particle_menu =
        eduiMenuCreate(70, 70, 250, 300, ed_fnt, edanimcbCancelParticleMenu, (char *)("Attached Particles"));
    if (!edanim_particle_menu) {
        return;
    }

    eduiMenuAddItem(edanim_particle_menu,
                    eduiItemSelCreate(1, colours, 0, 0, edanimcbParticleTypeMenu, (char *)("Particle Type...")));
    if (edanim_nearest_particle != -1) {
        eduiMenuAddItem(edanim_particle_menu, eduiItemSelCreate(1, colours, 0, 0, edanimcbLocalParticleMenu,
                                                                (char *)("Highlighted Ptl Settings...")));
    }

    eduiMenuAttach(parent, edanim_particle_menu);
    edanim_particle_menu->x = parent->x + 10;
    edanim_particle_menu->y = parent->y + 40;
}
void edanimcbCancelOptMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    edanim_active_menu = NULL;
}

void edanimcbFileSave(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    char name[256];
    char directory[256];
    char extension[256];
    char path[256];
    if (edbits_level_save_directory[0] == 0)
        sprintf(directory, ".");
    else
        strcpy(directory, edbits_level_save_directory);
    if (edbits_level_save_name[0] == 0)
        sprintf(name, "anims");
    else
        strcpy(name, edbits_level_save_name);
    if (edbits_level_save_extension[0] == 0)
        sprintf(extension, "anm");
    else
        strcpy(extension, edbits_level_save_extension);
    sprintf(path, "%s\\%s.%s", directory, name, extension);
    if (edanimFileSave(path))
        eduiCreateMessageMenu(menu, (char *)("Saved OK"), 1);
    else
        eduiCreateMessageMenu(menu, (char *)("File Save Error"), 0);
}

extern const char MenuTechnicalDefaultTextPc75f29c[8];

static void edanimcbMCTBCardPresent(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (saveloadCheckCardPresent()) {
        eduiCreateMessageMenu(menu, (char *)MenuTechnicalDefaultTextPc75f29c, 1);
    } else {
        eduiCreateMessageMenu(menu, (char *)("Not Present"), 0);
    }
}
void edanimcbMCTBCardType(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (saveloadCheckCardType())
        eduiCreateMessageMenu(menu, (char *)("PS2 Card"), 1);
    else
        eduiCreateMessageMenu(menu, (char *)("Not a PS2 Card"), 0);
}
void edanimcbMCTBCardCheckFormat(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (saveloadCheckCardFormatted()) {
        eduiCreateMessageMenu(menu, (char *)("Formatted"), 1);
    } else {
        eduiCreateMessageMenu(menu, (char *)("Not Formatted"), 0);
    }
}
void edanimcbMCTBCardFreeSpace(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    char message[32];
    sprintf(message, "Space = %05d", saveloadCheckCardFreeSpace(0));
    eduiCreateMessageMenu(menu, message, 1);
}
void edanimcbMCTBCardSlotsUsed(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    char message[32];
    sprintf(message, "Slots Used = %02d", saveloadCheckSlotsUsed());
    eduiCreateMessageMenu(menu, message, 1);
}
void edanimcbMCTBCardSaveSlot(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
    char data[32];
    sprintf(data, "This is slot %d", item->data);
    if (saveloadSaveSlot(item->data, data, sizeof(data)))
        eduiCreateMessageMenu(menu, (char *)("Saved OK"), 1);
    else
        eduiCreateMessageMenu(menu, (char *)("Save Error"), 0);
}
void edanimcbMCTBCardLoadSlot(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
    char data[32];
    if (saveloadLoadSlot(item->data, data, sizeof(data))) {
        eduiCreateMessageMenu(menu, data, 1);
    } else {
        eduiCreateMessageMenu(menu, (char *)("Load Error"), 0);
    }
}
void edanimcbMCTBCardDeleteSlot(eduimenu_s *menu, eduiitem_s *item, u32 unused2) {
    if (saveloadDeleteSlot(item->data))
        eduiCreateMessageMenu(menu, (char *)("Delete OK"), 1);
    else
        eduiCreateMessageMenu(menu, (char *)("Delete Error"), 0);
}
void edanimcbMCTBCardUnFormat(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (saveloadUnFormatCard()) {
        eduiCreateMessageMenu(menu, (char *)("Unformat OK"), 1);
    } else {
        eduiCreateMessageMenu(menu, (char *)("Unformat Fail"), 0);
    }
}
void edanimcbMCTBCardFormat(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (saveloadFormatCard()) {
        eduiCreateMessageMenu(menu, (char *)("Format OK"), 1);
    } else {
        eduiCreateMessageMenu(menu, (char *)("Format Fail"), 0);
    }
}

void edanimClose(void) {
    eduiMenuDestroy(edanim_options_menu);
}

void edanimEnter() {
    edanim_nearest = -1;
    edanim_nearest_param_id = -1;
    edanim_nearest_particle = -1;
    edanim_particle_mode = 0;
    edanim_particle_type = -1;
    edanim_sound_type = -1;
}

static void edanimcbMCTBCardWriteKeyCard(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (saveloadWriteKeyCode(id_test, code_test)) {
        eduiCreateMessageMenu(menu, (char *)("KeyCard Write OK"), 1);
    } else {
        eduiCreateMessageMenu(menu, (char *)("KeyCard Write Fail"), 0);
    }
}

i32 __cdecl saveloadCheckKeyCode(char *id, u8 *code);
extern struct eduimenu_s *edanim_mctb_menu;

static void edanimcbMCTBCardCheckKeyCard(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    if (saveloadCheckKeyCode(id_test, code_test)) {
        eduiCreateMessageMenu(menu, (char *)("KeyCard Check OK"), 1);
    } else {
        eduiCreateMessageMenu(menu, (char *)("KeyCard Check Fail"), 0);
    }
}

static void edanimcbCancelMCTBMenu(eduimenu_s *unused0, eduimenu_s *unused1) {
    eduiMenuDestroy(edanim_mctb_menu);
    edanim_mctb_menu = NULL;
}

void edanimcbMCTBMenu(eduimenu_s *parent, eduiitem_s *unused1, u32 unused2) {
    u32 colours[4] = {0x80000000, 0x80ff0000, 0x80808080, 0x80404040};
    edanim_mctb_menu =
        eduiMenuCreate(70, 70, 250, 250, ed_fnt, edanimcbCancelMCTBMenu, (char *)("Memory Card Test Menu"));
    if (!edanim_mctb_menu)
        return;
    eduiMenuAddItem(edanim_mctb_menu,
                    eduiItemSelCreate(1, colours, 0, 0, edanimcbMCTBCardPresent, (char *)("Card Present Test")));
    eduiMenuAddItem(edanim_mctb_menu,
                    eduiItemSelCreate(1, colours, 0, 0, edanimcbMCTBCardType, (char *)("Card Type Test")));
    eduiMenuAddItem(edanim_mctb_menu,
                    eduiItemSelCreate(1, colours, 0, 0, edanimcbMCTBCardCheckFormat, (char *)("Card Format Test")));
    eduiMenuAddItem(edanim_mctb_menu,
                    eduiItemSelCreate(1, colours, 0, 0, edanimcbMCTBCardFreeSpace, (char *)("Card Free Space")));
    eduiMenuAddItem(edanim_mctb_menu,
                    eduiItemSelCreate(1, colours, 0, 0, edanimcbMCTBCardSlotsUsed, (char *)("Card Slots Used")));
    eduiMenuAddItem(edanim_mctb_menu,
                    eduiItemSelCreate(0, colours, 0, 0, edanimcbMCTBCardSaveSlot, (char *)("Card Save Slot 0")));
    eduiMenuAddItem(edanim_mctb_menu,
                    eduiItemSelCreate(0, colours, 0, 0, edanimcbMCTBCardLoadSlot, (char *)("Card Load Slot 0")));
    eduiMenuAddItem(edanim_mctb_menu,
                    eduiItemSelCreate(0, colours, 0, 0, edanimcbMCTBCardDeleteSlot, (char *)("Card Delete Slot 0")));
    eduiMenuAddItem(edanim_mctb_menu,
                    eduiItemSelCreate(1, colours, 0, 0, edanimcbMCTBCardSaveSlot, (char *)("Card Save Slot 1")));
    eduiMenuAddItem(edanim_mctb_menu,
                    eduiItemSelCreate(1, colours, 0, 0, edanimcbMCTBCardLoadSlot, (char *)("Card Load Slot 1")));
    eduiMenuAddItem(edanim_mctb_menu,
                    eduiItemSelCreate(1, colours, 0, 0, edanimcbMCTBCardDeleteSlot, (char *)("Card Delete Slot 1")));
    eduiMenuAddItem(edanim_mctb_menu,
                    eduiItemSelCreate(1, colours, 0, 0, edanimcbMCTBCardFormat, (char *)("Format Card")));
    eduiMenuAddItem(edanim_mctb_menu,
                    eduiItemSelCreate(1, colours, 0, 0, edanimcbMCTBCardUnFormat, (char *)("Unformat Card")));
    eduiMenuAddItem(edanim_mctb_menu,
                    eduiItemSelCreate(1, colours, 0, 0, edanimcbMCTBCardWriteKeyCard, (char *)("Write KeyCard")));
    eduiMenuAddItem(edanim_mctb_menu,
                    eduiItemSelCreate(1, colours, 0, 0, edanimcbMCTBCardCheckKeyCard, (char *)("Check KeyCard")));
    eduiMenuAttach(parent, edanim_mctb_menu);
    edanim_mctb_menu->x = parent->x + 10;
    edanim_mctb_menu->y = parent->y + 40;
}

STATIC_ASSERT(offsetof(GAMEOBJECT, mini_current_animation) == 0x7ee);
STATIC_ASSERT(offsetof(GAMEOBJECT, mini_previous_animation) == 0x7f0);
extern f32 __cdecl GizBuildItMul(GAMEOBJECT *);
extern void __cdecl UpdateAnimPacket(CHARACTER_MODEL_LAYERS_PC *, ANIMPACKET_PC *, f32, f32, f32, f32);
extern void __cdecl AutoWeaponOnOff(GAMEOBJECT *);
extern void __cdecl AddFootSteps(GAMEOBJECT *);

void __cdecl AnimatePlayer(GAMEOBJECT *object) {
    GAMEPAD_PC *pad = object->pad;
    CHARACTER_MODEL_LAYERS_PC *model = object->character_model;
    i16 override_from;
    f32 direction, movement_speed, multiplier;
    f32 *time;
    object->anim_packet.previous_animation = object->anim_packet.animation_index;
    object->mini_previous_animation = object->mini_current_animation;
    if ((object->flags_1f8 & 0x40000) == 0)
        object->character_data->animate_fn(object);
    override_from = object->ai_packet.animation_override_from;
    if (override_from != -1 && ((override_from == 0xe9 && object->character_context != 0x1c) ||
                                object->anim_packet.requested_animation == override_from))
        object->anim_packet.requested_animation = object->ai_packet.animation_override_to;
    if (model == NULL || (object->dead != 0 && object->death_delay == 0.0f) || object->dead == 1)
        return;
    direction = 1.0f;
    if ((object->flags_ef0 & 0x4000) != 0)
        direction = -1.0f;
    if (object->character_model->action_animations[1] == NULL) {
        if ((object->character_data->game_character->flags_090 & 0x100000) != 0) {
            movement_speed =
                object->velocity.x * object->facing_direction.x + object->velocity.z * object->facing_direction.z;
            if (movement_speed < 0.0f)
                movement_speed = 0.0f;
        } else {
            movement_speed = pad->input_magnitude;
        }
    } else {
        movement_speed = pad->animation_input_magnitude;
        if (movement_speed > 0.0f && (u16)object->id == id_GONKDROID && Cheat_IsOn(8) == 0)
            movement_speed = object->character_data->game_character->walk_speed;
    }
    multiplier = (object->character_context == 0x2d && object->grapple != NULL) ? GizBuildItMul(object) : 1.0f;
    UpdateAnimPacket(model, &object->anim_packet, (f32)((f64)FRAMETIME * 30.0 * (f64)multiplier),
                     movement_speed * direction, multiplier * FRAMETIME,
                     object->character_data->game_character->backwards_factor);
    if ((object->anim_packet.flags & 4) != 0) {
        time = NULL;
        if (object->anim_packet.blending != 0) {
            if (object->anim_packet.blend_target_animation == 0x5f)
                time = &object->anim_packet.blend_target_time;
        } else if (object->anim_packet.animation_index == 0x5f) {
            time = &object->anim_packet.current_time;
        }
        if (time != NULL) {
            f32 duration = AnimDuration((i16)object->id, 0x5f, 0.0f, 0.0f, 0);
            i32 count = (i32)((f64)duration / (f64)0.3f);
            i32 phase;
            f32 start;
            if (NuFmod(duration, 3.0f) > 0.15f)
                ++count;
            phase = qrand() / (0xffff / count + 1);
            model = object->character_model;
            start = (f32)((f64)phase * ((f64)0.3f * (f64)model->animation_configs[0x5f]->playback_rate) + 1.0);
            if (start < NuAnimEndFrame(model->action_animations[0x5f]))
                *time = start;
            object->flags_e20 = (object->flags_e20 & ~0x4000u) | ((phase & 1) << 14);
        }
    }
    AutoWeaponOnOff(object);
    AddFootSteps(object);
}

GAMEANIMOBJ *__cdecl GameAnimSet_AddObjectByName(GAMEANIMSET *set, NUGSCN *scene, char *name, f32 start_frame,
                                                 f32 end_frame, i32 append, void *gizmo_sys, char *prefix,
                                                 char *suffix) {
    NUSPECIALHANDLE special;
    if (set && Gizmo_FindNuSpecial(scene, &special, name, 1, gizmo_sys, prefix, suffix))
        return GameAnimSet_AddObject(set, &special, start_frame, end_frame, append);
    return NULL;
}

void GameAnimSet_RemoveSpecial(GAMEANIMSET *set, nuhspecial_s *special) {
    struct gameanimobj_s *object;
    if (set == NULL || special == NULL)
        return;
    object = set->objects;
    while (object != NULL && NuSpecialCompare(&object->special, special) == 0) {
        object = object->next;
    }
    if (object != NULL)
        GameAnimSet_RemoveObject(set, object);
}
typedef struct gameanim_reflection_matrix_pc_s {
    f32 m00, m01, m02, m03;
    f32 m10, m11, m12, m13;
    f32 m20, m21, m22;
    f32 axis_coordinates[5];
} GAMEANIM_REFLECTION_MATRIX_PC;
STATIC_ASSERT(sizeof(GAMEANIM_REFLECTION_MATRIX_PC) == sizeof(NUMTX));
STATIC_ASSERT(offsetof(GAMEANIM_REFLECTION_MATRIX_PC, axis_coordinates) == 0x2c);
STATIC_ASSERT(offsetof(NUMTX, m23) == 0x2c);
STATIC_ASSERT(offsetof(NUMTX, m30) == 0x30);
STATIC_ASSERT(offsetof(NUMTX, m31) == 0x34);
STATIC_ASSERT(offsetof(NUMTX, m32) == 0x38);
STATIC_ASSERT(offsetof(NUMTX, m33) == 0x3c);
i32 __cdecl MatrixReflection(NUMTX *, i32, f32, f32, NUMTX *);
void __cdecl GameAnimSet_DrawReflection(GAMEANIMSET *set, i32 axis, f32 offset, NUMTX *matrix) {
    f32 plane;
    GAMEANIMOBJ *object;
    NUMTX reflection;
    if (!set || !set->objects)
        return;
    if (!matrix)
        matrix = NuSpecialGetMtx(&set->objects->special);
    plane = offset + ((GAMEANIM_REFLECTION_MATRIX_PC *)matrix)->axis_coordinates[axis];
    NuRndrStartReflectionRender(0);
    for (object = set->objects; object; object = object->next) {
        if (!(object->flags & 2) && NuSpecialGetVisibilityFn(&object->special)) {
            NUMTX *draw_matrix = NuSpecialGetDrawMtx(&object->special);
            if (MatrixReflection(draw_matrix, axis, plane, WORLD->current_level->reflection_y, &reflection))
                NuSpecialDrawAt(&object->special, &reflection);
        }
    }
    NuRndrEndReflectionRender();
}
