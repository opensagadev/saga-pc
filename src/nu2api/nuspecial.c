#include "saga.h"

extern f32 __cdecl NuAnimEndFrameOld(void *animation_data);

extern i32 nuspecial_draw_state;
extern i32 nuspecial_const_alpha_enabled;
extern i32 nuspecial_const_tint_enabled;
extern f32 nuspecial_const_alpha;
extern NUVEC nuspecial_const_tint;

typedef struct NuSpecialLegacyView {
    u8 unknown_00[0x40];
    void *instance;
    char *name;
    u32 flags;
} NuSpecialLegacyView;

typedef union NuSpecialLegacyInstanceField48 {
    void *animation;
    NUMTX *draw_matrix;
} NuSpecialLegacyInstanceField48;

typedef struct NuSpecialLegacyInstanceView {
    u8 unknown_00[0x44];
    u32 flags;
    NuSpecialLegacyInstanceField48 field_48;
} NuSpecialLegacyInstanceView;

typedef struct NuSpecialDisplayView {
    NUMTX instance_matrix;
    NUMTX draw_matrix;
    u8 unknown_80[0x34];
    char *name;
    u32 flags;
    u8 unknown_bc[8];
    void *instance_animation;
    u8 unknown_c8[8];
} NuSpecialDisplayView;

typedef struct NuSpecialDisplayListView {
    u8 unknown_00[0x6c];
    i32 special_count;
} NuSpecialDisplayListView;

typedef struct NuSpecialSceneView {
    u8 unknown_00[0x24];
    i32 special_count;
    void *specials;
    u8 unknown_2c[0xe4];
    NuSpecialDisplayListView *display_list;
} NuSpecialSceneView;

typedef struct NuSpecialLegacyObjectView {
    u8 unknown_00[0x34];
    NUVEC center;
    f32 radius;
} NuSpecialLegacyObjectView;

typedef struct NuSpecialLegacyInstanceIndexView {
    u8 unknown_00[0x40];
    i16 object_index;
} NuSpecialLegacyInstanceIndexView;

typedef struct NuSpecialAnimationInstanceView {
    u8 unknown_00[0x50];
    u32 flags;
    u8 unknown_54[8];
    u16 animation_index;
} NuSpecialAnimationInstanceView;

typedef struct NuSpecialAnimFallbackRecord {
    u16 unknown_00;
    u16 end_frame;
    u8 unknown_04[8];
} NuSpecialAnimFallbackRecord;

typedef struct NuSpecialAnimFallbackTableView {
    u8 unknown_0000[0x1e0];
    NuSpecialAnimFallbackRecord *records;
} NuSpecialAnimFallbackTableView;

typedef struct NuSpecialBoundsDisplayView {
    u8 unknown_00[0xa0];
    NUVEC center;
    f32 radius;
} NuSpecialBoundsDisplayView;

typedef struct NuSpecialLegacySceneObjectsView {
    u8 unknown_00[0x18];
    NuSpecialLegacyObjectView **objects;
} NuSpecialLegacySceneObjectsView;

enum {
    NUSPECIAL_COLLISION_FLAG = 1 << 9,
    NUSPECIAL_NO_VISIBILITY_TEST_FLAG = 1 << 7,
    NUINSTANCE_NO_VISIBILITY_TEST_FLAG = 1 << 3
};

STATIC_ASSERT(offsetof(NuSpecialLegacyView, instance) == 0x40);
STATIC_ASSERT(offsetof(NuSpecialLegacyView, name) == 0x44);
STATIC_ASSERT(offsetof(NuSpecialLegacyView, flags) == 0x48);
STATIC_ASSERT(sizeof(NuSpecialLegacyView) == 0x4c);
STATIC_ASSERT(offsetof(NuSpecialLegacyInstanceView, field_48) == 0x48);
STATIC_ASSERT(offsetof(NuSpecialLegacyInstanceView, flags) == 0x44);
STATIC_ASSERT(sizeof(NuSpecialLegacyInstanceView) == 0x4c);
STATIC_ASSERT(offsetof(NuSpecialDisplayView, draw_matrix) == 0x40);
STATIC_ASSERT(offsetof(NuSpecialDisplayView, name) == 0xb4);
STATIC_ASSERT(offsetof(NuSpecialDisplayView, flags) == 0xb8);
STATIC_ASSERT(offsetof(NuSpecialDisplayView, instance_animation) == 0xc4);
STATIC_ASSERT(sizeof(NuSpecialDisplayView) == 0xd0);
STATIC_ASSERT(offsetof(NuSpecialDisplayListView, special_count) == 0x6c);
STATIC_ASSERT(offsetof(NuSpecialSceneView, special_count) == 0x24);
STATIC_ASSERT(offsetof(NuSpecialSceneView, display_list) == 0x110);
STATIC_ASSERT(offsetof(NuSpecialLegacyObjectView, center) == 0x34);
STATIC_ASSERT(offsetof(NuSpecialLegacyObjectView, radius) == 0x40);
STATIC_ASSERT(offsetof(NuSpecialLegacyInstanceIndexView, object_index) == 0x40);
STATIC_ASSERT(offsetof(NuSpecialAnimationInstanceView, flags) == 0x50);
STATIC_ASSERT(offsetof(NuSpecialAnimationInstanceView, animation_index) == 0x5c);
STATIC_ASSERT(offsetof(NuSpecialAnimFallbackTableView, records) == 0x1e0);
STATIC_ASSERT(offsetof(NuSpecialAnimFallbackRecord, end_frame) == 2);
STATIC_ASSERT(sizeof(NuSpecialAnimFallbackRecord) == 0xc);
STATIC_ASSERT(offsetof(NuSpecialBoundsDisplayView, center) == 0xa0);
STATIC_ASSERT(offsetof(NuSpecialBoundsDisplayView, radius) == 0xac);
STATIC_ASSERT(offsetof(NuSpecialLegacySceneObjectsView, objects) == 0x18);

f32 __cdecl NuSpecialGetAnimEndFrame(NUSPECIALHANDLE *special) {
    NuSpecialLegacyView *legacy;
    NuSpecialLegacyInstanceView *legacy_instance;
    NuSpecialDisplayView *display;
    NuSpecialAnimationInstanceView *animation_instance;
    NuSpecialAnimFallbackTableView *fallback_table;
    void *animation;
    void **animation_data;
    u32 animation_index;

    if (special == NULL || special->scene == NULL) {
        return 0.0f;
    }
    legacy = (NuSpecialLegacyView *)special->special;
    if (legacy != NULL) {
        legacy_instance = (NuSpecialLegacyInstanceView *)legacy->instance;
        animation = legacy_instance->field_48.animation;
    } else {
        display = (NuSpecialDisplayView *)special->display_special;
        if (display == NULL) {
            return 0.0f;
        }
        animation = display->instance_animation;
        if (animation == (void *)-1) {
            return 0.0f;
        }
    }
    if (animation == NULL) {
        return 0.0f;
    }

    animation_instance = (NuSpecialAnimationInstanceView *)animation;
    animation_index = animation_instance->animation_index;
    animation_data = ((NUGSCN *)special->scene)->instance_animation_data;
    if (animation_data[animation_index] != NULL) {
        return NuAnimEndFrameOld(animation_data[animation_index]);
    }

    animation_index = animation_instance->flags >> 22;
    if (animation_index == 0) {
        return 0.0f;
    }
    fallback_table = (NuSpecialAnimFallbackTableView *)special->scene;
    if (fallback_table->records == NULL) {
        return 0.0f;
    }
    return (f32)fallback_table->records[animation_index - 1].end_frame;
}

void __cdecl NuSpecialGetRadius(NUSPECIALHANDLE *special, NUVEC *position, f32 *radius) {
    NuSpecialLegacyView *legacy;
    NuSpecialLegacyInstanceIndexView *instance;
    NuSpecialLegacySceneObjectsView *scene;
    NuSpecialLegacyObjectView *object;
    NuSpecialBoundsDisplayView *display;

    legacy = (NuSpecialLegacyView *)special->special;
    if (legacy != NULL) {
        instance = (NuSpecialLegacyInstanceIndexView *)legacy->instance;
        scene = (NuSpecialLegacySceneObjectsView *)special->scene;
        object = scene->objects[instance->object_index];
        *radius = object->radius;
        *position = object->center;
        return;
    }
    display = (NuSpecialBoundsDisplayView *)special->display_special;
    *position = display->center;
    *radius = display->radius;
}

NUMTX *__cdecl NuSpecialGetMtx(NUSPECIALHANDLE *special) {
    if (special->display_special != NULL) {
        return special->display_special;
    }
    return special->special;
}

void __cdecl NuSpecialSetMtx(NUSPECIALHANDLE *special, const NUMTX *matrix) {
    if (special->display_special != NULL) {
        *special->display_special = *matrix;
    } else {
        *special->special = *matrix;
    }
}

i32 __cdecl NuSpecialExistsFn(NUSPECIALHANDLE *special) {
    if (special != NULL) {
        if (special->special != NULL || special->display_special != NULL) {
            return 1;
        }
    }
    return 0;
}

char *__cdecl NuSpecialGetName(NUSPECIALHANDLE *special) {
    NuSpecialDisplayView *display;
    NuSpecialLegacyView *legacy;
    if (special == NULL) {
        return NULL;
    }
    display = (NuSpecialDisplayView *)special->display_special;
    if (display != NULL) {
        return display->name;
    }
    legacy = (NuSpecialLegacyView *)special->special;
    return legacy != NULL ? legacy->name : NULL;
}

i32 __cdecl NuSpecialTestAnim(NUSPECIALHANDLE *special) {
    NuSpecialDisplayView *display = (NuSpecialDisplayView *)special->display_special;
    NuSpecialLegacyView *legacy;
    NuSpecialLegacyInstanceView *instance;
    if (display != NULL) {
        return display->instance_animation != NULL && display->instance_animation != (void *)-1;
    }
    legacy = (NuSpecialLegacyView *)special->special;
    if (legacy == NULL) {
        return 0;
    }
    instance = (NuSpecialLegacyInstanceView *)legacy->instance;
    return instance->field_48.animation != NULL;
}

NUVEC *__cdecl NuSpecialGetPos(NUSPECIALHANDLE *special) {
    NUMTX *matrix = special->display_special;
    if (matrix == NULL) {
        matrix = special->special;
    }
    return matrix != NULL ? (NUVEC *)&matrix->m30 : NULL;
}

NUVEC *__cdecl NuSpecialGetDrawPos(NUSPECIALHANDLE *special) {
    NuSpecialDisplayView *display = (NuSpecialDisplayView *)special->display_special;
    NuSpecialLegacyView *legacy;
    NuSpecialLegacyInstanceView *instance_view;
    NUMTX *instance;
    NUMTX *draw_matrix;
    if (display != NULL) {
        if (display->instance_animation != NULL && display->instance_animation != (void *)-1) {
            return (NUVEC *)&((NUMTX *)display->instance_animation)->m30;
        }
        return (NUVEC *)&display->draw_matrix.m30;
    }
    legacy = (NuSpecialLegacyView *)special->special;
    if (legacy == NULL) {
        return NULL;
    }
    instance = (NUMTX *)legacy->instance;
    instance_view = (NuSpecialLegacyInstanceView *)legacy->instance;
    draw_matrix = instance_view->field_48.draw_matrix;
    return draw_matrix != NULL ? (NUVEC *)&draw_matrix->m30 : (NUVEC *)&instance->m30;
}

NUMTX *__cdecl NuSpecialGetDrawMtx(NUSPECIALHANDLE *special) {
    NuSpecialLegacyView *legacy = (NuSpecialLegacyView *)special->special;
    NuSpecialLegacyInstanceView *instance_view;
    NuSpecialDisplayView *display;
    NUMTX *instance;
    NUMTX *draw_matrix;
    if (legacy != NULL) {
        instance = (NUMTX *)legacy->instance;
        instance_view = (NuSpecialLegacyInstanceView *)legacy->instance;
        draw_matrix = instance_view->field_48.draw_matrix;
        return draw_matrix != NULL ? draw_matrix : instance;
    }
    display = (NuSpecialDisplayView *)special->display_special;
    if (display == NULL) {
        return NULL;
    }
    if (display->instance_animation == NULL || display->instance_animation == (void *)-1) {
        return &display->draw_matrix;
    }
    return (NUMTX *)display->instance_animation;
}

NUMTX *__cdecl NuSpecialGetInstanceMtx(NUSPECIALHANDLE *special) {
    NuSpecialLegacyView *legacy = (NuSpecialLegacyView *)special->special;
    NuSpecialDisplayView *display;
    if (legacy != NULL) {
        return (NUMTX *)legacy->instance;
    }
    display = (NuSpecialDisplayView *)special->display_special;
    return display != NULL ? &display->draw_matrix : NULL;
}

void *__cdecl NuSpecialGetInstAnim(NUSPECIALHANDLE *special) {
    NuSpecialLegacyView *legacy = (NuSpecialLegacyView *)special->special;
    NuSpecialLegacyInstanceView *instance;
    NuSpecialDisplayView *display;
    if (legacy != NULL) {
        instance = (NuSpecialLegacyInstanceView *)legacy->instance;
        return instance->field_48.animation;
    }
    display = (NuSpecialDisplayView *)special->display_special;
    if (display != NULL && display->instance_animation != (void *)-1) {
        return display->instance_animation;
    }
    return NULL;
}

void __cdecl NuSpecialSetCollision(NUSPECIALHANDLE *special, i32 enabled) {
    NuSpecialLegacyView *legacy;
    NuSpecialDisplayView *display;
    if (special == NULL || special->scene == NULL) {
        return;
    }
    legacy = (NuSpecialLegacyView *)special->special;
    if (legacy != NULL) {
        if (enabled != 0) {
            legacy->flags |= NUSPECIAL_COLLISION_FLAG;
        } else {
            legacy->flags &= ~NUSPECIAL_COLLISION_FLAG;
        }
        return;
    }
    display = (NuSpecialDisplayView *)special->display_special;
    if (display != NULL) {
        if (enabled != 0) {
            display->flags |= NUSPECIAL_COLLISION_FLAG;
        } else {
            display->flags &= ~NUSPECIAL_COLLISION_FLAG;
        }
    }
}

i32 __cdecl NuSpecialGetCollision(NUSPECIALHANDLE *special) {
    NuSpecialLegacyView *legacy;
    NuSpecialDisplayView *display;
    if (special == NULL || special->scene == NULL) {
        return 0;
    }
    legacy = (NuSpecialLegacyView *)special->special;
    if (legacy != NULL) {
        return legacy->flags & NUSPECIAL_COLLISION_FLAG;
    }
    display = (NuSpecialDisplayView *)special->display_special;
    return display != NULL ? display->flags & NUSPECIAL_COLLISION_FLAG : 0;
}

void __cdecl NuSpecialSetNoVisiTest(NUSPECIALHANDLE *special, i32 enabled) {
    NuSpecialLegacyView *legacy;
    NuSpecialLegacyInstanceView *instance;
    NuSpecialDisplayView *display;
    if (special->scene == NULL) {
        return;
    }
    legacy = (NuSpecialLegacyView *)special->special;
    if (legacy != NULL) {
        instance = (NuSpecialLegacyInstanceView *)legacy->instance;
        if (enabled != 0) {
            instance->flags |= NUINSTANCE_NO_VISIBILITY_TEST_FLAG;
        } else {
            instance->flags &= (u8)~NUINSTANCE_NO_VISIBILITY_TEST_FLAG;
        }
        return;
    }
    display = (NuSpecialDisplayView *)special->display_special;
    if (display == NULL) {
        return;
    }
    if (enabled != 0) {
        display->flags |= NUSPECIAL_NO_VISIBILITY_TEST_FLAG;
    } else {
        display->flags &= ~NUSPECIAL_NO_VISIBILITY_TEST_FLAG;
    }
}

i32 __cdecl NuSpecialGetNoVisiTestFn(NUSPECIALHANDLE *special) {
    NuSpecialLegacyView *legacy;
    NuSpecialLegacyInstanceView *instance;
    NuSpecialDisplayView *display;
    if (special->scene == NULL) {
        return 0;
    }
    legacy = (NuSpecialLegacyView *)special->special;
    if (legacy != NULL) {
        instance = (NuSpecialLegacyInstanceView *)legacy->instance;
        return (instance->flags >> 3) & 1;
    }
    display = (NuSpecialDisplayView *)special->display_special;
    return display != NULL ? display->flags & NUSPECIAL_NO_VISIBILITY_TEST_FLAG : 0;
}

void __cdecl NuSpecialConstAlpha(i32 enabled, f32 alpha) {
    if (enabled != 0) {
        nuspecial_draw_state |= 1;
        nuspecial_const_alpha = alpha;
        nuspecial_const_alpha_enabled = enabled;
    } else {
        nuspecial_draw_state &= ~1;
        nuspecial_const_alpha_enabled = 0;
    }
}

void __cdecl NuSpecialConstTint(i32 enabled, const NUVEC *tint) {
    if (enabled != 0) {
        nuspecial_const_tint_enabled = enabled;
        nuspecial_const_tint = *tint;
        nuspecial_draw_state |= 2;
    } else {
        nuspecial_draw_state &= ~2;
        nuspecial_const_tint_enabled = 0;
    }
}

void __cdecl NuSpecialGetNext(NUSPECIALHANDLE *special) {
    if (special->special != NULL) {
        special->special = (NUMTX *)((NuSpecialLegacyView *)special->special + 1);
    } else if (special->display_special != NULL) {
        special->display_special = (NUMTX *)((NuSpecialDisplayView *)special->display_special + 1);
    }
}

void __cdecl PCSpecialEmptyHandler_candidate(void) {
}

i32 __cdecl NuSpecialGetFirst(NUGSCN *scene, NUSPECIALHANDLE *special, i32 flags) {
    if (scene->specials != NULL) {
        special->scene = scene;
        special->special = (NUMTX *)scene->specials;
        special->display_special = NULL;
        return 1;
    }
    if (scene->display_list->special_count != 0) {
        special->scene = scene;
        special->special = NULL;
        special->display_special = (NUMTX *)scene->display_list->specials;
        return 1;
    }
    special->scene = NULL;
    special->special = NULL;
    special->display_special = NULL;
    if (flags != 0) {
        PCSpecialEmptyHandler_candidate();
    }
    return 0;
}

i32 __cdecl NuSpecialGetNumSpecials(NUGSCN *scene) {
    NuSpecialSceneView *view = (NuSpecialSceneView *)scene;
    i32 count = view->special_count;
    if (count == 0 && view->display_list != NULL) {
        count = view->display_list->special_count;
    }
    return count;
}

i32 __cdecl NuSpecialGetOnScreenFn(NUSPECIALHANDLE *special) {
    NuSpecialLegacyView *legacy;
    NuSpecialLegacyInstanceView *instance;
    if (special->scene != NULL && special->special != NULL) {
        legacy = (NuSpecialLegacyView *)special->special;
        instance = (NuSpecialLegacyInstanceView *)legacy->instance;
        return (instance->flags >> 1) & 1;
    }
    return 1;
}

i32 __cdecl NuSpecialGetVisibilityFn(NUSPECIALHANDLE *special) {
    NuSpecialLegacyView *legacy;
    NuSpecialLegacyInstanceView *instance;
    NuSpecialDisplayView *display;
    if (special->scene == NULL) {
        return 0;
    }
    legacy = (NuSpecialLegacyView *)special->special;
    if (legacy != NULL) {
        instance = (NuSpecialLegacyInstanceView *)legacy->instance;
        return instance->flags & 1;
    }
    display = (NuSpecialDisplayView *)special->display_special;
    return display != NULL ? (display->flags >> 1) & 1 : 0;
}

i32 __cdecl NuSpecialCompare(NUSPECIALHANDLE *first, NUSPECIALHANDLE *second) {
    if (first->special != NULL && first->special == second->special) {
        return 1;
    }
    if (first->display_special != NULL && first->display_special == second->display_special) {
        return 1;
    }
    return 0;
}

void __cdecl NuSpecialClear(NUSPECIALHANDLE *special) {
    special->scene = NULL;
    special->special = NULL;
    special->display_special = NULL;
}

void __cdecl NuSpecialSetVisibility(NUSPECIALHANDLE *handle, i32 visible) {
    NULEGACYSPECIAL *legacy;
    NUDISPLAY_SPECIAL_PC *display;
    NUDISPLAY_LIST_PC *display_list;

    if (handle == NULL || handle->scene == NULL) {
        return;
    }
    legacy = (NULEGACYSPECIAL *)handle->special;
    if (legacy != NULL) {
        if (legacy->instance != NULL) {
            legacy->instance->visible ^= (legacy->instance->visible ^ visible) & 1;
        }
        if (visible != 0) {
            legacy->flags_48 |= 0x200;
        } else {
            legacy->flags_48 &= ~0x200;
        }
        return;
    }
    display = (NUDISPLAY_SPECIAL_PC *)handle->display_special;
    if (display == NULL) {
        return;
    }
    if (visible != 0) {
        display->flags |= 0x202;
        display_list = ((NUGSCN *)handle->scene)->display_list;
        if ((display_list->instance_visibility_enabled & 1) != 0) {
            display_list->visibility_flags[display->instance_ix] |= 1;
        }
    } else {
        display->flags &= ~0x2;
        display->flags &= ~0x200;
        display_list = ((NUGSCN *)handle->scene)->display_list;
        if ((display_list->instance_visibility_enabled & 1) != 0) {
            display_list->visibility_flags[display->instance_ix] &= ~1;
        }
    }
}

/* The PC backend's transform-update leaf is a single ret instruction. */
void __cdecl DisplayListUpdateSpecialTransformPS(NUSPECIALHANDLE *special, NUMTX *matrix) {
    (void)special;
    (void)matrix;
}

void __cdecl NuDisplayListUpdateSpecial(NUSPECIALHANDLE *special) {
    NUDISPLAY_SPECIAL_PC *display;
    NUDISPLAY_LIST_PC *display_scene;
    NUMTX draw_matrix;
    NUVEC center;
    NUVEC half_extent;
    NUVEC world_extent;
    NUVEC4 *clip_bounds;
    i32 bounds_index;
    NUMTX *draw_matrix_ptr;

    display = (NUDISPLAY_SPECIAL_PC *)special->display_special;
    display_scene = ((NUGSCN *)special->scene)->display_list;
    draw_matrix_ptr = NuSpecialGetDrawMtx(special);
    draw_matrix = *draw_matrix_ptr;

    half_extent.x = (display->bounds_max.x - display->bounds_min.x) * 0.5f;
    half_extent.y = (display->bounds_max.y - display->bounds_min.y) * 0.5f;
    half_extent.z = (display->bounds_max.z - display->bounds_min.z) * 0.5f;
    center.x = (display->bounds_max.x + display->bounds_min.x) * 0.5f;
    center.y = (display->bounds_max.y + display->bounds_min.y) * 0.5f;
    center.z = (display->bounds_max.z + display->bounds_min.z) * 0.5f;

#define NUSPECIAL_ABS(value) ((value) < 0.0f ? -(value) : (value))
    world_extent.x = NUSPECIAL_ABS(draw_matrix.m00) * half_extent.x + NUSPECIAL_ABS(draw_matrix.m10) * half_extent.y +
                     NUSPECIAL_ABS(draw_matrix.m20) * half_extent.z;
    world_extent.y = NUSPECIAL_ABS(draw_matrix.m01) * half_extent.x + NUSPECIAL_ABS(draw_matrix.m11) * half_extent.y +
                     NUSPECIAL_ABS(draw_matrix.m21) * half_extent.z;
    world_extent.z = NUSPECIAL_ABS(draw_matrix.m02) * half_extent.x + NUSPECIAL_ABS(draw_matrix.m12) * half_extent.y +
                     NUSPECIAL_ABS(draw_matrix.m22) * half_extent.z;
#undef NUSPECIAL_ABS

    NuVecMtxTransform(&center, &center, &draw_matrix);
    clip_bounds = display_scene->clip_bounds;
    bounds_index = (i32)(display->instance_ix * 2);
    if ((display_scene->render_flags & 0x4000) != 0) {
        clip_bounds[bounds_index].x = center.x;
        clip_bounds[bounds_index].y = center.y;
        clip_bounds[bounds_index].z = center.z;
        clip_bounds[bounds_index].w = NuFsqrt(world_extent.x + world_extent.y + world_extent.z);
        clip_bounds[bounds_index + 1].x = world_extent.x;
        clip_bounds[bounds_index + 1].y = world_extent.y;
        clip_bounds[bounds_index + 1].z = world_extent.z;
        STUBBED_NAMED("NuDisplayListUpdateSpecial: undefined center-extent W store at 0x5df6f1");
    } else {
        clip_bounds[bounds_index].x = center.x - world_extent.x;
        clip_bounds[bounds_index].y = center.y - world_extent.y;
        clip_bounds[bounds_index].z = center.z - world_extent.z;
        STUBBED_NAMED("NuDisplayListUpdateSpecial: undefined axis-bound W store at 0x5df760");
        clip_bounds[bounds_index + 1].x = center.x + world_extent.x;
        clip_bounds[bounds_index + 1].y = center.y + world_extent.y;
        clip_bounds[bounds_index + 1].z = center.z + world_extent.z;
        STUBBED_NAMED("NuDisplayListUpdateSpecial: undefined axis-bound W store at 0x5df788");
    }

    DisplayListUpdateSpecialTransformPS(special, &draw_matrix);
}

void __cdecl NuSpecialUpdate(NUSPECIALHANDLE *special) {
    NUGSCN *scene;
    NUDISPLAY_SPECIAL_PC *display;
    NUDISPLAY_LIST_PC *display_scene;

    if (special == NULL) {
        return;
    }
    display = (NUDISPLAY_SPECIAL_PC *)special->display_special;
    if (display == NULL || special->scene == NULL) {
        return;
    }
    scene = (NUGSCN *)special->scene;
    display_scene = scene->display_list;
    if (display_scene == NULL || (display_scene->instance_visibility_enabled & 1) == 0) {
        return;
    }
    display->flags |= 0x400;
    NuDisplayListUpdateSpecial(special);
}

typedef struct NuSpecialInstanceRecordPC {
    u8 unknown_00[0x50];
} NuSpecialInstanceRecordPC;

typedef struct NuSpecialInstanceScenePC {
    u8 unknown_00[0x1c];
    i32 instance_count;
    NuSpecialInstanceRecordPC *instances;
} NuSpecialInstanceScenePC;

typedef struct NuSpecialInstanceDisplayPC {
    u8 unknown_00[0xc0];
    i32 instance_index;
} NuSpecialInstanceDisplayPC;

STATIC_ASSERT(sizeof(NuSpecialInstanceRecordPC) == 0x50);
STATIC_ASSERT(offsetof(NuSpecialInstanceScenePC, instance_count) == 0x1c);
STATIC_ASSERT(offsetof(NuSpecialInstanceScenePC, instances) == 0x20);
STATIC_ASSERT(offsetof(NuSpecialInstanceDisplayPC, instance_index) == 0xc0);

i32 __cdecl NuSpecialGetInstanceix(NUSPECIALHANDLE *handle) {
    NuSpecialLegacyView *legacy = (NuSpecialLegacyView *)handle->special;
    i32 i;
    if (legacy != NULL) {
        NuSpecialInstanceScenePC *scene = (NuSpecialInstanceScenePC *)handle->scene;
        for (i = 0; i < scene->instance_count; ++i) {
            if (&scene->instances[i] == legacy->instance)
                return i;
        }
        return -1;
    }
    if (handle->display_special != NULL)
        return ((NuSpecialInstanceDisplayPC *)handle->display_special)->instance_index;
    return -1;
}

i32 special_debug;

i32 __cdecl NuSpecialFindDisplay(NUGSCN *scene, NUSPECIALHANDLE *handle, char *name, i32 flags) {
    NUDISPLAY_SPECIAL_PC *special;
    i32 index;
    if (scene == NULL)
        return 0;
    special = scene->display_list->specials;
    for (index = 0; index < scene->display_list->special_count; ++index, ++special) {
        if (NuStrICmp(name, special->name) == 0) {
            handle->scene = scene;
            handle->display_special = (NUMTX *)special;
            handle->special = NULL;
            return 1;
        }
    }
    if (flags != 0 && special_debug != 0) {
        STUBBED_NAMED("NuSpecialFindDisplay: unresolved missing special diagnostic at 0x005c1300");
    }
    handle->scene = NULL;
    handle->special = NULL;
    handle->display_special = NULL;
    return 0;
}

i32 __cdecl NuSpecialFind(NUGSCN *scene, NUSPECIALHANDLE *handle, char *name, i32 flags) {
    NUSPECIAL_LEGACY_PC *special;
    i32 index;
    (void)flags;
    if (scene != NULL && name != NULL) {
        if (scene->display_list != NULL)
            return NuSpecialFindDisplay(scene, handle, name, 1);
        special = scene->specials;
        for (index = 0; index < scene->special_count; ++index, ++special) {
            if (NuStrICmp(name, special->name) == 0) {
                handle->scene = scene;
                handle->special = (NUMTX *)special;
                handle->display_special = NULL;
                return 1;
            }
        }
    }
    handle->scene = NULL;
    handle->special = NULL;
    handle->display_special = NULL;
    return 0;
}

i32 __cdecl NuGScnNumSpecials(NUGSCN *scene) {
    if (scene->display_list != NULL)
        return scene->display_list->special_count;
    return scene->special_count;
}

void __cdecl NuGScnGetSpecial(NUSPECIALHANDLE *special, NUGSCN *scene, i32 index) {
    if (scene != NULL && special != NULL) {
        if (scene->display_list != NULL && index < scene->display_list->special_count) {
            special->scene = scene;
            special->special = NULL;
            special->display_special = (NUMTX *)&scene->display_list->specials[index];
            return;
        }
        if (index < scene->special_count) {
            special->scene = scene;
            special->special = (NUMTX *)&scene->specials[index];
            special->display_special = NULL;
            return;
        }
    }
    special->scene = NULL;
    special->special = NULL;
    special->display_special = NULL;
}

typedef struct NuSpecialLegacyMatrixPc {
    NUMTX matrix;
} NuSpecialLegacyMatrixPc;
STATIC_ASSERT(offsetof(NuSpecialLegacyMatrixPc, matrix) == 0);

void __cdecl NuSpecialSetDrawMtx(NUSPECIALHANDLE *special, NUMTX *matrix) {
    NuSpecialLegacyView *legacy;
    NuSpecialDisplayView *display;
    NUDISPLAY_LIST_PC *display_list;

    if (special == NULL || special->scene == NULL) {
        return;
    }
    legacy = (NuSpecialLegacyView *)special->special;
    if (legacy != NULL) {
        ((NuSpecialLegacyMatrixPc *)legacy->instance)->matrix = *matrix;
        return;
    }
    display = (NuSpecialDisplayView *)special->display_special;
    display->draw_matrix = *matrix;
    display_list = ((NUGSCN *)special->scene)->display_list;
    if ((display_list->instance_visibility_enabled & 1) != 0) {
        NuDisplayListUpdateSpecial(special);
        ((NuSpecialDisplayView *)special->display_special)->flags |= 0x400;
    }
}
