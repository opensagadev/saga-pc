#include "saga.h"

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
