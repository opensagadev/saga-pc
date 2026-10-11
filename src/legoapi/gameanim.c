#include "saga.h"

extern void __cdecl NuSpecialSetVisibility(NUSPECIALHANDLE *special, i32 visible);

/* PC animation constructors call the shared game allocator out of line. */
__declspec(noinline) void *__cdecl GameBufferAlloc(VARIPTR *buf, VARIPTR *end, i32 size) {
    void *ptr = NULL;

    if (buf != NULL && end != NULL && buf->addr + (u32)size < end->addr) {
        buf->addr = (buf->addr + 3) & ~3u;
        ptr = buf->void_ptr;
        buf->addr += (u32)size;
    }
    if (ptr != NULL) {
        memset(ptr, 0, (u32)size);
    }
    return ptr;
}

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
    NUINSTANIM_TIME_VIEW *animation;
    i32 all_at_end = 1;
    i32 all_at_start = 1;
    f32 direction;
    f32 current_frame;

    if ((set->flags & 4) != 0) {
        return;
    }
    object = set->objects;
    while (object != NULL) {
        animation = object->instance_animation;
        if (animation != NULL) {
            direction = object->end_frame < object->start_frame ? nu_fsign_minus_one : 1.0f;
            current_frame = animation->ltime * direction;
            if (object->end_frame * direction > current_frame) {
                all_at_end = 0;
            }
            if (current_frame > object->start_frame * direction) {
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
    GAMEANIMSET *set;
    u8 *progress;
    i32 i;

    if (system == NULL || system->sets == NULL || index < 0 || index >= gameanimsysprogress.count) {
        return;
    }
    progress = gameanimsysprogress.entries[index];
    for (i = 0; i < gameanimsysprogress.entry_size && system->sets[i] != NULL; ++i) {
        set = system->sets[i];
        set->state = (i8)progress[i];
        if (set->state == 1 || set->state == 3) {
            if ((set->flags & 4) == 0) {
                GameAnimSet_AddToSystemList(set);
            }
        } else if ((set->flags & 4) != 0) {
            GameAnimSet_RemoveFromSystemList(set);
        }
    }
}

GAMEANIMOBJPOOL *__cdecl GameAnimSet_CreateObjectPool(VARIPTR *buf, VARIPTR *end, i32 object_data_size, i32 capacity) {
    GAMEANIMOBJPOOL *pool = NULL;
    GAMEANIMOBJ *object;
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
                object = &pool->objects[i];
                object->next = pool->free_objects;
                pool->free_objects = object;
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
    if (set->objects == object) {
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
    NUINSTANIM_TIME_VIEW *animation;
    f32 direction;

    if (set == NULL) {
        return;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        animation = object->instance_animation;
        if (animation != NULL) {
            direction = object->end_frame < object->start_frame ? nu_fsign_minus_one : 1.0f;
            if (animation->fparam1 != 0.0f) {
                animation->tfactor = animation->fparam1 * factor * direction;
            } else {
                animation->tfactor = direction * factor;
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
    NUINSTANIM_TIME_VIEW *animation;
    i32 animated_count = 0;
    i32 repeating_count = 0;
    f32 direction;

    if (set == NULL || set->objects == NULL) {
        return 1;
    }
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
        direction = object->end_frame < object->start_frame ? nu_fsign_minus_one : 1.0f;
        if (animation->ltime * direction > object->start_frame * direction) {
            return 0;
        }
    }
    if (animated_count == 0) {
        return 1;
    }
    return animated_count != repeating_count;
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
    NUINSTANIM_TIME_VIEW *animation;
    f32 direction;
    f32 current_frame;

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
        animation = object->instance_animation;
        if (animation == NULL) {
            continue;
        }
        animation->flags |= 1;
        animation->flags &= ~4u;
        direction = object->end_frame < object->start_frame ? nu_fsign_minus_one : 1.0f;
        current_frame = animation->ltime * direction;
        if (object->start_frame * direction > current_frame) {
            animation->ltime = object->start_frame;
        } else if (current_frame > object->end_frame * direction) {
            animation->ltime = object->end_frame;
        }
        if (animation->fparam1 != 0.0f) {
            animation->tfactor = animation->fparam1 * speed * direction;
        } else {
            animation->tfactor = direction * speed;
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
    NUINSTANIM_TIME_VIEW *animation;

    if (set != NULL) {
        for (object = set->objects; object != NULL; object = object->next) {
            animation = object->instance_animation;
            if (animation != NULL) {
                animation->flags = (animation->flags & ~8u) | (((u32)repeating & 1) << 3);
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
    NUINSTANIM_TIME_VIEW *animation;
    f32 direction;

    if (set == NULL) {
        return;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        animation = object->instance_animation;
        if (animation == NULL || object->animation == NULL) {
            continue;
        }
        animation->flags &= ~1u;
        animation->ltime = (object->end_frame - object->start_frame) * position + object->start_frame;
        direction = object->end_frame < object->start_frame ? nu_fsign_minus_one : 1.0f;
        if (animation->ltime * direction > object->end_frame * direction) {
            animation->ltime = object->end_frame;
        }
        if (object->start_frame * direction > animation->ltime * direction) {
            animation->ltime = object->start_frame;
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
    f32 position;

    if (object == NULL || object->instance_animation == NULL || object->animation == NULL) {
        return 0.0f;
    }
    if (object->start_frame == object->end_frame) {
        return 1.0f;
    }
    position = (object->instance_animation->ltime - object->start_frame) / (object->end_frame - object->start_frame);
    if (position < 0.0f) {
        return 0.0f;
    }
    if (position > 1.0f) {
        position = 1.0f;
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
    f32 ratio;

    if (set == NULL) {
        return 0.0f;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        if (object->instance_animation == NULL) {
            continue;
        }
        if (object->start_frame == object->end_frame) {
            return 1.0f;
        }
        ratio = (object->instance_animation->ltime - object->start_frame) / (object->end_frame - object->start_frame);
        if (ratio > 1.0f) {
            ratio = 1.0f;
        }
        if (ratio < 0.0f) {
            ratio = 0.0f;
        }
        return ratio;
    }
    return 0.0f;
}

GAMEANIMOBJ *__cdecl GameAnimSet_AddObject(GAMEANIMSET *set, NUSPECIALHANDLE *special, f32 start_frame, f32 end_frame,
                                           i32 append) {
    GAMEANIMOBJPOOL *pool;
    GAMEANIMOBJ *object;
    GAMEANIMOBJ *tail;
    i32 object_index;
    f32 last_frame;

    if (set == NULL || set->object_pool == NULL || set->object_pool->free_objects == NULL || special == NULL ||
        NuSpecialExistsFn(special) == 0) {
        return NULL;
    }

    pool = set->object_pool;
    ++set->object_count;
    object = pool->free_objects;
    ++pool->active_count;
    pool->free_objects = object->next;
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

    object_index = (i32)(object - pool->objects);
    object->object_data = (u8 *)pool->object_data + pool->object_data_size * object_index;
    object->special = *special;
    object->instance_animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(&object->special);
    if (object->instance_animation == NULL) {
        return object;
    }

    object->animation =
        ((NUGSCN *)object->special.scene)->instance_animation_data[object->instance_animation->animation_index];
    last_frame = NuSpecialGetAnimEndFrame(&object->special);
    if (last_frame > 0.0f) {
        if (last_frame < end_frame) {
            object->end_frame = last_frame;
        } else {
            object->end_frame = end_frame;
            if (object->end_frame < 1.0f) {
                object->end_frame = 1.0f;
            }
        }
        if (start_frame > last_frame) {
            object->start_frame = last_frame;
        } else {
            object->start_frame = start_frame;
            if (object->start_frame < 1.0f) {
                object->start_frame = 1.0f;
            }
        }
        ++set->animated_object_count;
    } else {
        (void)NuSpecialGetName(&object->special);
    }
    return object;
}

i32 __cdecl GameAnimSet_GetAveragePos(GAMEANIMSET *set, NUVEC *position, i32 frame_selection, i32 include_animated,
                                      i32 include_static) {
    GAMEANIMOBJ *object;
    NUVEC sum = {0.0f, 0.0f, 0.0f};
    i32 count = 0;

    if (position == NULL || set == NULL || set->object_count == 0 || set->objects == NULL) {
        return 0;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        if ((object->flags & 1) != 0) {
            continue;
        }
        if (object->instance_animation != NULL) {
            f32 frame;
            NUMTX matrix;
            NUVEC translation;

            if (include_animated == 0) {
                continue;
            }
            if (frame_selection == 0) {
                frame = object->start_frame;
            } else if (frame_selection == 1) {
                frame = object->end_frame;
            } else {
                frame = object->instance_animation->ltime;
            }
            EvalAnim(&object->special, frame, &matrix, 1);
            translation.x = matrix.m30;
            translation.y = matrix.m31;
            translation.z = matrix.m32;
            NuVecAdd(&sum, &sum, &translation);
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
    i32 visible_count = 0;

    if (set == NULL) {
        return 0;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        if (NuSpecialGetVisibilityFn(&object->special) != 0) {
            ++visible_count;
        }
    }
    if (visible_count == set->object_count) {
        return 2;
    }
    return visible_count > 0 ? 1 : 0;
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

void __cdecl EvalAnim(NUSPECIALHANDLE *special, f32 frame, NUMTX *matrix, i32 update) {
    NUINSTANIM_TIME_VIEW *instance_animation;
    ANI3_HEADER_PC *animation;
    NUGSCN *scene;
    NUMTX *instance_matrix;

    if (special == NULL || matrix == NULL) {
        return;
    }
    instance_animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(special);
    if (instance_animation == NULL) {
        if (update != 0) {
            instance_matrix = NuSpecialGetInstanceMtx(special);
            if (instance_matrix != NULL) {
                *matrix = *instance_matrix;
            }
        }
        return;
    }

    scene = (NUGSCN *)special->scene;
    animation = (ANI3_HEADER_PC *)scene->instance_animation_data[instance_animation->animation_index];
    if (animation == NULL) {
        return;
    }
    NuAnimData2CalcMatrix(animation, 0, frame, matrix);
    if (update != 0) {
        instance_matrix = NuSpecialGetInstanceMtx(special);
        if (instance_matrix != NULL) {
            matrix->m30 += instance_matrix->m30;
            matrix->m31 += instance_matrix->m31;
            matrix->m32 += instance_matrix->m32;
        }
    }
}

void __cdecl EvalAnim2(NUSPECIALHANDLE *special, f32 frame) {
    NUINSTANIM_TIME_VIEW *instance_animation;
    ANI3_HEADER_PC *animation;
    NUGSCN *scene;
    NUMTX *instance_matrix;
    i32 animation_index;

    instance_animation = (NUINSTANIM_TIME_VIEW *)NuSpecialGetInstAnim(special);
    if (instance_animation == NULL) {
        return;
    }
    scene = (NUGSCN *)special->scene;
    animation = (ANI3_HEADER_PC *)scene->instance_animation_data[instance_animation->animation_index];
    if (animation == NULL || frame == instance_animation->prev_eval_time) {
        return;
    }
    instance_matrix = NuSpecialGetInstanceMtx(special);
    NuAnimData2CalcMatrix(animation, 0, frame, &instance_animation->matrix);
    instance_animation->prev_eval_time = frame;
    animation_index = (i32)(instance_animation - scene->instance_animations);
    scene->instance_animation_matrices[animation_index] = instance_animation->matrix;
    instance_animation->matrix.m30 += instance_matrix->m30;
    instance_animation->matrix.m31 += instance_matrix->m31;
    instance_animation->matrix.m32 += instance_matrix->m32;
    NuSpecialUpdate(special);
}

void __cdecl GameAnimSet_EvalAnim(GAMEANIMSET *set) {
    GAMEANIMOBJ *object;

    if (set == NULL) {
        return;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        if (object->instance_animation != NULL) {
            EvalAnim2(&object->special, object->instance_animation->ltime);
        }
    }
}

i32 __cdecl GameAnimSet_GetCentreAndRadius(GAMEANIMSET *set, NUVEC *centre, f32 *radius, i32 frame_selection,
                                           i32 include_animated, i32 include_static) {
    GAMEANIMOBJ *object;
    NUVEC minimum = {1.0e9f, 1.0e9f, 1.0e9f};
    NUVEC maximum = {-1.0e9f, -1.0e9f, -1.0e9f};
    i32 found_object = 0;

    if (centre == NULL || set == NULL || set->object_count == 0 || set->objects == NULL) {
        return 0;
    }
    for (object = set->objects; object != NULL; object = object->next) {
        NUVEC object_centre;
        f32 object_radius;
        NUMTX *draw_matrix;
        NUMTX evaluated_matrix;
        f32 frame;

        if ((object->flags & 1) != 0) {
            continue;
        }
        if (object->instance_animation != NULL) {
            if (include_animated == 0) {
                continue;
            }
            if (frame_selection == 0) {
                frame = object->start_frame;
            } else if (frame_selection == 1) {
                frame = object->end_frame;
            } else {
                frame = object->instance_animation->ltime;
            }
            EvalAnim(&object->special, frame, &evaluated_matrix, 1);
            draw_matrix = &evaluated_matrix;
        } else {
            if (include_static == 0) {
                continue;
            }
            draw_matrix = NuSpecialGetDrawMtx(&object->special);
            if (draw_matrix == NULL) {
                continue;
            }
        }
        NuSpecialGetRadius(&object->special, &object_centre, &object_radius);
        object_radius *= 0.75f;
        NuVecMtxTransform(&object_centre, &object_centre, draw_matrix);
        if (object_centre.x - object_radius < minimum.x) {
            minimum.x = object_centre.x - object_radius;
        }
        if (object_centre.y - object_radius < minimum.y) {
            minimum.y = object_centre.y - object_radius;
        }
        if (object_centre.z - object_radius < minimum.z) {
            minimum.z = object_centre.z - object_radius;
        }
        if (object_centre.x + object_radius > maximum.x) {
            maximum.x = object_centre.x + object_radius;
        }
        if (object_centre.y + object_radius > maximum.y) {
            maximum.y = object_centre.y + object_radius;
        }
        if (object_centre.z + object_radius > maximum.z) {
            maximum.z = object_centre.z + object_radius;
        }
        found_object = 1;
    }
    if (found_object == 0) {
        return 0;
    }
    centre->x = (minimum.x + maximum.x) * 0.5f;
    centre->y = (minimum.y + maximum.y) * 0.5f;
    centre->z = (minimum.z + maximum.z) * 0.5f;
    if (radius != NULL) {
        f32 half_x = (maximum.x - minimum.x) * 0.5f;
        f32 half_y = (maximum.y - minimum.y) * 0.5f;
        f32 half_z = (maximum.z - minimum.z) * 0.5f;
        *radius = NuFsqrt(half_x * half_x + half_y * half_y + half_z * half_z);
    }
    return 1;
}

void __cdecl GameAnimSys_Update(GAMEANIMSYS *system) {
    GAMEANIMSET *set;
    GAMEANIMSET *next_set;
    GAMEANIMOBJ *object;
    u8 was_no_visibility_test;
    i32 all_playing_forward;
    i32 all_at_start;
    i32 all_at_end;
    i32 any_special_no_visibility_test;
    f32 direction;
    f32 previous_frame;
    f32 directed_frame;

    if (system == NULL) {
        return;
    }
    set = (GAMEANIMSET *)NuLinkedListGetHead(&system->active_sets);
    while (set != NULL) {
        next_set = (GAMEANIMSET *)NuLinkedListGetNext(&system->active_sets, &set->links);
        was_no_visibility_test = (u8)(set->flags & 1);
        set->flags &= (u16)~1;
        if ((set->flags & 2) != 0) {
            set->flags &= (u16) ~(1 | 2);
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
                if (previous_frame * direction >= object->end_frame * direction) {
                    if (object->instance_animation->tfactor * direction < 0.0f) {
                        object->instance_animation->ltime = object->end_frame;
                    } else if ((object->instance_animation->flags & 8) != 0) {
                        object->instance_animation->ltime = previous_frame - object->end_frame + object->start_frame;
                    } else {
                        object->instance_animation->ltime = object->end_frame;
                        object->instance_animation->flags &= ~1u;
                    }
                } else if (previous_frame * direction <= object->start_frame * direction) {
                    if (object->instance_animation->tfactor * direction > 0.0f) {
                        object->instance_animation->ltime = object->start_frame;
                    } else if ((object->instance_animation->flags & 8) != 0) {
                        object->instance_animation->ltime = object->end_frame - (object->start_frame - previous_frame);
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
                directed_frame = object->instance_animation->ltime * direction;
                if (directed_frame > object->start_frame * direction) {
                    all_at_start = 0;
                }
                if (directed_frame < object->end_frame * direction) {
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
        if (edanim_page_used[page] != 0 && edanim_page_scene[page] != NULL && edanim_page_on[page] == 0) {
            edanim_page_on[page] = page * 4 + 1;
        }
    }
}

typedef struct animate_character_reader_pc_s {
    u8 unknown_00[0x1c];
    void(__cdecl *animate_fn)(GAMEOBJECT *);
} ANIMATE_CHARACTER_READER_PC;
STATIC_ASSERT(offsetof(ANIMATE_CHARACTER_READER_PC, animate_fn) == 0x1c);
typedef struct animate_player_reader_pc_s {
    u8 unknown_00[0x3ee];
    i16 animation_override_from;
    i16 animation_override_to;
    u8 unknown_3f2[0x7ee - 0x3f2];
    i16 mini_current_animation;
    i16 mini_previous_animation;
} ANIMATE_PLAYER_READER_PC;
STATIC_ASSERT(offsetof(ANIMATE_PLAYER_READER_PC, animation_override_from) == 0x3ee);
STATIC_ASSERT(offsetof(ANIMATE_PLAYER_READER_PC, animation_override_to) == 0x3f0);
STATIC_ASSERT(offsetof(ANIMATE_PLAYER_READER_PC, mini_current_animation) == 0x7ee);
STATIC_ASSERT(offsetof(ANIMATE_PLAYER_READER_PC, mini_previous_animation) == 0x7f0);
extern f32 __cdecl GizBuildItMul(GAMEOBJECT *);
extern void __cdecl UpdateAnimPacket(CHARACTER_MODEL_LAYERS_PC *, ANIMPACKET_PC *, f32, f32, f32, f32);
extern void __cdecl AutoWeaponOnOff(GAMEOBJECT *);
extern void __cdecl AddFootSteps(GAMEOBJECT *);

void __cdecl AnimatePlayer(GAMEOBJECT *object) {
    ANIMPACKET_PC *packet = &object->anim_packet;
    ANIMATE_PLAYER_READER_PC *state = (ANIMATE_PLAYER_READER_PC *)object;
    GAMEPAD_PC *pad = object->pad;
    CHARACTER_MODEL_LAYERS_PC *model = object->character_model;
    i16 override_from;
    f32 direction, movement_speed, multiplier;
    packet->previous_animation = packet->animation_index;
    state->mini_previous_animation = state->mini_current_animation;
    if ((object->flags_1f8 & 0x40000) == 0)
        ((ANIMATE_CHARACTER_READER_PC *)object->character_data)->animate_fn(object);
    override_from = state->animation_override_from;
    if (override_from != -1 &&
        ((override_from == 0xe9 && object->character_context != 0x1c) || override_from == packet->requested_animation))
        packet->requested_animation = state->animation_override_to;
    if (model == NULL || (object->dead != 0 && (object->death_delay == 0.0f || object->dead == 1)))
        return;
    direction = (object->flags_ef0 & 0x4000) != 0 ? -1.0f : 1.0f;
    if (model->action_animations[1] != NULL) {
        movement_speed = pad->animation_input_magnitude;
        if (movement_speed > 0.0f && (u16)object->id == id_GONKDROID && Cheat_IsOn(8) == 0)
            movement_speed = object->character_data->game_character->walk_speed;
    } else if ((object->character_data->game_character->flags_090 & 0x100000) != 0) {
        movement_speed =
            object->velocity.x * object->facing_direction.x + object->velocity.z * object->facing_direction.z;
        if (movement_speed < 0.0f)
            movement_speed = 0.0f;
    } else {
        movement_speed = pad->input_magnitude;
    }
    multiplier = 1.0f;
    if (object->character_context == 0x2d && object->grapple != NULL)
        multiplier = GizBuildItMul(object);
    UpdateAnimPacket(model, packet, (f32)((f64)FRAMETIME * 30.0 * (f64)multiplier), movement_speed * direction,
                     multiplier * FRAMETIME, object->character_data->game_character->backwards_factor);
    if ((packet->flags & 4) != 0 &&
        (packet->blending != 0 ? packet->blend_target_animation : packet->animation_index) == 0x5f) {
        f32 *time = packet->blending != 0 ? &packet->blend_target_time : &packet->current_time;
        f32 duration = AnimDuration((i16)object->id, 0x5f, 0.0f, 0.0f, 0);
        i32 count = (i32)((f64)duration / (f64)0.3f);
        i32 phase;
        f32 start;
        if (NuFmod(duration, 3.0f) > 0.15f)
            ++count;
        phase = qrand() / (0xffff / count + 1);
        model = object->character_model;
        start = (f32)((f64)phase * ((f64)0.3f * (f64)model->animation_configs[0x5f]->playback_rate) + 1.0);
        if (NuAnimEndFrame(model->action_animations[0x5f]) > start)
            *time = start;
        object->flags_e20 = (object->flags_e20 & ~0x4000u) | ((phase & 1) << 14);
    }
    AutoWeaponOnOff(object);
    AddFootSteps(object);
}

GAMEANIMOBJ *__cdecl GameAnimSet_AddObjectByName(GAMEANIMSET *set, NUGSCN *scene, char *name, f32 start_frame,
                                                 f32 end_frame, i32 append, void *gizmo_sys, char *prefix,
                                                 char *suffix) {
    NUSPECIALHANDLE special;
    if (!set)
        return NULL;
    if (!Gizmo_FindNuSpecial(scene, &special, name, 1, gizmo_sys, prefix, suffix))
        return NULL;
    return GameAnimSet_AddObject(set, &special, start_frame, end_frame, append);
}
i32 __cdecl GizmoFileReadGameAnimSet(GAMEANIMSET *set, void *world_ptr, void(__cdecl *read_data)(GAMEANIMOBJ *, u8),
                                     u8 version, char *prefix, char *suffix) {
    WORLDINFO *world = (WORLDINFO *)world_ptr;
    u8 file_version = EdFileReadChar();
    u8 count = EdFileReadChar();
    i32 success = 1, i;
    for (i = 0; i < count; ++i) {
        char name[64];
        i32 length = (i8)EdFileReadChar();
        f32 start_frame, end_frame;
        u32 flags;
        GAMEANIMOBJ *object, missing;
        if (length)
            EdFileRead(name, length);
        start_frame = EdFileReadFloat();
        end_frame = EdFileReadFloat();
        flags = file_version > 1 ? EdFileReadInt() : 0;
        if (!length)
            continue;
        if (!set->object_pool || !set->object_pool->free_objects)
            success = 0;
        object = GameAnimSet_AddObjectByName(set, world->current_gscn, name, start_frame, end_frame, 0,
                                             world->gizmo_sys, prefix, suffix);
        if (!object) {
            memset(&missing, 0, sizeof(missing));
            object = &missing;
        }
        object->serialized_flags = flags;
        if (read_data)
            read_data(object, version);
        if (file_version < 3)
            object->serialized_flags &= ~2u;
    }
    return success;
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
    if (!set || !set->objects)
        return;
    if (!matrix)
        matrix = NuSpecialGetMtx(&set->objects->special);
    plane = offset + ((GAMEANIM_REFLECTION_MATRIX_PC *)matrix)->axis_coordinates[axis];
    NuRndrStartReflectionRender(0);
    for (object = set->objects; object; object = object->next) {
        if (!(object->flags & 2) && NuSpecialGetVisibilityFn(&object->special)) {
            NUMTX reflection;
            NUMTX *draw_matrix = NuSpecialGetDrawMtx(&object->special);
            if (MatrixReflection(draw_matrix, axis, plane, WORLD->current_level->reflection_y, &reflection))
                NuSpecialDrawAt(&object->special, &reflection);
        }
    }
    NuRndrEndReflectionRender();
}
