#include "saga.h"

extern f32 __cdecl NuAnimEndFrameOld(void *animation_data);

extern i32 nuspecial_draw_state;
extern i32 nuspecial_const_alpha_enabled;
extern i32 nuspecial_const_tint_enabled;
extern f32 nuspecial_const_alpha;
extern NUVEC nuspecial_const_tint;

STATIC_ASSERT(sizeof(NUINSTANCE) == 0x50);
STATIC_ASSERT(offsetof(NUINSTANCE, object_index) == 0x40);
STATIC_ASSERT(offsetof(NUINSTANCE, flags) == 0x44);
STATIC_ASSERT(offsetof(NUINSTANCE, animation) == 0x48);
STATIC_ASSERT(offsetof(NUSPECIAL_LEGACY_PC, instance) == 0x40);
STATIC_ASSERT(offsetof(NUSPECIAL_LEGACY_PC, flags) == 0x48);
STATIC_ASSERT(offsetof(NUINSTANIM, flags) == 0x50);
STATIC_ASSERT(offsetof(NUINSTANIM, animation_index) == 0x5c);
STATIC_ASSERT(offsetof(NUDISPLAY_SPECIAL_PC, draw_matrix) == 0x40);
STATIC_ASSERT(offsetof(NUDISPLAY_SPECIAL_PC, center) == 0xa0);
STATIC_ASSERT(offsetof(NUDISPLAY_SPECIAL_PC, radius) == 0xac);
STATIC_ASSERT(offsetof(NUDISPLAY_SPECIAL_PC, clip_objects) == 0xb0);
STATIC_ASSERT(offsetof(NUDISPLAY_SPECIAL_PC, instance_animation) == 0xc4);
STATIC_ASSERT(sizeof(NUGOBJ) == 0x50);
STATIC_ASSERT(offsetof(NUGOBJ, origin_radius) == 0x14);
STATIC_ASSERT(offsetof(NUGOBJ, bounds_min) == 0x1c);
STATIC_ASSERT(offsetof(NUGOBJ, center) == 0x34);
STATIC_ASSERT(offsetof(NUGOBJ, radius) == 0x40);
STATIC_ASSERT(offsetof(NUGOBJ, next) == 0x4c);
STATIC_ASSERT(offsetof(NUGSCN, num_objects) == 0x14);
STATIC_ASSERT(offsetof(NUGSCN, objects) == 0x18);
STATIC_ASSERT(offsetof(NUGSCN, num_instances) == 0x1c);
STATIC_ASSERT(offsetof(NUGSCN, display_list) == 0x110);
STATIC_ASSERT(offsetof(NUGSCN, end_frame_lookup) == 0x1e0);

#define SPECIAL_LEGACY(handle) ((NUSPECIAL_LEGACY_PC *)(handle)->special)
#define SPECIAL_DISPLAY(handle) ((NUDISPLAY_SPECIAL_PC *)(handle)->display_special)
#define SPECIAL_GOBJ(handle) (((NUGSCN *)(handle)->scene)->objects[SPECIAL_LEGACY(handle)->instance->object_index])

enum {
    NUSPECIAL_COLLISION_FLAG = 1 << 9,
    NUSPECIAL_NO_VISIBILITY_TEST_FLAG = 1 << 7,
    NUINSTANCE_NO_VISIBILITY_TEST_FLAG = 1 << 3
};

f32 __cdecl NuSpecialGetAnimEndFrame(NUSPECIALHANDLE *special) {
    NUSPECIAL_LEGACY_PC *legacy;
    NUINSTANCE *legacy_instance;
    NUDISPLAY_SPECIAL_PC *display;
    NUINSTANIM *animation_instance;
    NUGSCN *fallback_table;
    void *animation;
    void *animation_data;
    u32 animation_index;

    if (special == NULL || special->scene == NULL) {
        return 0.0f;
    }
    legacy = (NUSPECIAL_LEGACY_PC *)special->special;
    if (legacy != NULL) {
        legacy_instance = (NUINSTANCE *)legacy->instance;
        animation = legacy_instance->animation;
    } else {
        display = (NUDISPLAY_SPECIAL_PC *)special->display_special;
        if (display == NULL || display->instance_animation == (void *)-1 || display->instance_animation == NULL) {
            return 0.0f;
        }
        animation = display->instance_animation;
    }
    if (animation == NULL) {
        return 0.0f;
    }

    animation_instance = (NUINSTANIM *)animation;
    animation_data = ((NUGSCN *)special->scene)->instance_animation_data[animation_instance->animation_index];
    if (animation_data != NULL) {
        return NuAnimEndFrameOld(animation_data);
    }

    animation_index = animation_instance->flags >> 22;
    if (animation_index == 0) {
        return 0.0f;
    }
    fallback_table = (NUGSCN *)special->scene;
    if (fallback_table->end_frame_lookup == NULL) {
        return 0.0f;
    }
    return (f32)fallback_table->end_frame_lookup[animation_index - 1].end_frame;
}

void __cdecl NuSpecialGetRadius(NUSPECIALHANDLE *special, NUVEC *position, f32 *radius) {
    if (special->special != NULL) {
        *radius = SPECIAL_GOBJ(special)->radius;
        *position = SPECIAL_GOBJ(special)->center;
    } else {
        *position = SPECIAL_DISPLAY(special)->center;
        *radius = SPECIAL_DISPLAY(special)->radius;
    }
}

f32 __cdecl NuSpecialGetOriginRadius(NUSPECIALHANDLE *special) {
    if (special->special != NULL)
        return SPECIAL_GOBJ(special)->origin_radius;
    return NuVecMag(&SPECIAL_DISPLAY(special)->center) + SPECIAL_DISPLAY(special)->radius;
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
    NUDISPLAY_SPECIAL_PC *display;
    NUSPECIAL_LEGACY_PC *legacy;
    if (special == NULL) {
        return NULL;
    }
    display = (NUDISPLAY_SPECIAL_PC *)special->display_special;
    if (display != NULL) {
        return display->name;
    }
    legacy = (NUSPECIAL_LEGACY_PC *)special->special;
    return legacy != NULL ? legacy->name : NULL;
}

i32 __cdecl NuSpecialTestAnim(NUSPECIALHANDLE *special) {
    if (special->display_special != NULL) {
        if (SPECIAL_DISPLAY(special)->instance_animation != (NUINSTANIM *)-1 &&
            SPECIAL_DISPLAY(special)->instance_animation != NULL)
            return 1;
    } else if (special->special != NULL && SPECIAL_LEGACY(special)->instance->animation != NULL) {
        return 1;
    }
    return 0;
}

NUVEC *__cdecl NuSpecialGetPos(NUSPECIALHANDLE *special) {
    if (special->display_special != NULL)
        return (NUVEC *)&special->display_special->m30;
    if (special->special != NULL)
        return (NUVEC *)&special->special->m30;
    return NULL;
}

NUVEC *__cdecl NuSpecialGetDrawPos(NUSPECIALHANDLE *special) {
    if (special->display_special != NULL) {
        if (SPECIAL_DISPLAY(special)->instance_animation != (NUINSTANIM *)-1 &&
            SPECIAL_DISPLAY(special)->instance_animation != NULL)
            return (NUVEC *)&SPECIAL_DISPLAY(special)->instance_animation->matrix.m30;
        return (NUVEC *)&SPECIAL_DISPLAY(special)->draw_matrix.m30;
    }
    if (special->special != NULL) {
        if (SPECIAL_LEGACY(special)->instance->animation != NULL)
            return (NUVEC *)&SPECIAL_LEGACY(special)->instance->animation->matrix.m30;
        return (NUVEC *)&SPECIAL_LEGACY(special)->instance->matrix.m30;
    }
    return NULL;
}

NUMTX *__cdecl NuSpecialGetDrawMtx(NUSPECIALHANDLE *special) {
    NUSPECIAL_LEGACY_PC *legacy = (NUSPECIAL_LEGACY_PC *)special->special;
    NUINSTANCE *instance_view;
    NUDISPLAY_SPECIAL_PC *display;
    NUMTX *instance;
    NUMTX *draw_matrix;
    if (legacy != NULL) {
        instance = (NUMTX *)legacy->instance;
        instance_view = (NUINSTANCE *)legacy->instance;
        draw_matrix = &instance_view->animation->matrix;
        return draw_matrix != NULL ? draw_matrix : instance;
    }
    display = (NUDISPLAY_SPECIAL_PC *)special->display_special;
    if (display != NULL) {
        if (display->instance_animation == (void *)-1 || display->instance_animation == NULL) {
            return &display->draw_matrix;
        }
        return (NUMTX *)display->instance_animation;
    }
    return NULL;
}

NUMTX *__cdecl NuSpecialGetInstanceMtx(NUSPECIALHANDLE *special) {
    NUSPECIAL_LEGACY_PC *legacy = (NUSPECIAL_LEGACY_PC *)special->special;
    NUDISPLAY_SPECIAL_PC *display;
    if (legacy != NULL) {
        return (NUMTX *)legacy->instance;
    }
    display = (NUDISPLAY_SPECIAL_PC *)special->display_special;
    return display != NULL ? &display->draw_matrix : NULL;
}

i32 __cdecl NuSpecialClipTestExtents(NUSPECIALHANDLE *special, NUMTX *matrix) {
    if (special->special != NULL)
        return NuCameraClipTestExtents(&SPECIAL_GOBJ(special)->bounds_min, &SPECIAL_GOBJ(special)->bounds_max, matrix,
                                       0.0f, 0);
    return NuCameraClipTestExtents((NUVEC *)&SPECIAL_DISPLAY(special)->bounds_min,
                                   (NUVEC *)&SPECIAL_DISPLAY(special)->bounds_max, matrix, 0.0f, 0);
}

void *__cdecl NuSpecialGetInstAnim(NUSPECIALHANDLE *special) {
    if (special->special != NULL)
        return SPECIAL_LEGACY(special)->instance->animation;
    if (special->display_special != NULL && SPECIAL_DISPLAY(special)->instance_animation != (NUINSTANIM *)-1 &&
        SPECIAL_DISPLAY(special)->instance_animation != NULL)
        return SPECIAL_DISPLAY(special)->instance_animation;
    return NULL;
}

void __cdecl NuSpecialSetCollision(NUSPECIALHANDLE *special, i32 enabled) {
    if (special == NULL || special->scene == NULL)
        return;
    if (special->special != NULL) {
        if (enabled != 0)
            SPECIAL_LEGACY(special)->flags |= NUSPECIAL_COLLISION_FLAG;
        else
            SPECIAL_LEGACY(special)->flags &= ~NUSPECIAL_COLLISION_FLAG;
    } else if (special->display_special != NULL) {
        if (enabled != 0)
            SPECIAL_DISPLAY(special)->flags |= NUSPECIAL_COLLISION_FLAG;
        else
            SPECIAL_DISPLAY(special)->flags &= ~NUSPECIAL_COLLISION_FLAG;
    }
}

i32 __cdecl NuSpecialGetCollision(NUSPECIALHANDLE *special) {
    NUSPECIAL_LEGACY_PC *legacy;
    NUDISPLAY_SPECIAL_PC *display;
    if (special == NULL || special->scene == NULL) {
        return 0;
    }
    legacy = (NUSPECIAL_LEGACY_PC *)special->special;
    if (legacy != NULL) {
        return legacy->flags & NUSPECIAL_COLLISION_FLAG;
    }
    display = (NUDISPLAY_SPECIAL_PC *)special->display_special;
    return display != NULL ? display->flags & NUSPECIAL_COLLISION_FLAG : 0;
}

void __cdecl NuSpecialSetNoVisiTest(NUSPECIALHANDLE *special, i32 enabled) {
    if (special->scene == NULL)
        return;
    if (special->special != NULL) {
        SPECIAL_LEGACY(special)->instance->no_visibility_test = enabled;
    } else if (special->display_special != NULL) {
        if (enabled != 0)
            SPECIAL_DISPLAY(special)->flags |= NUSPECIAL_NO_VISIBILITY_TEST_FLAG;
        else
            SPECIAL_DISPLAY(special)->flags &= ~NUSPECIAL_NO_VISIBILITY_TEST_FLAG;
    }
}

i32 __cdecl NuSpecialGetNoVisiTestFn(NUSPECIALHANDLE *special) {
    NUSPECIAL_LEGACY_PC *legacy;
    NUINSTANCE *instance;
    NUDISPLAY_SPECIAL_PC *display;
    if (special->scene == NULL) {
        return 0;
    }
    legacy = (NUSPECIAL_LEGACY_PC *)special->special;
    if (legacy != NULL) {
        instance = (NUINSTANCE *)legacy->instance;
        return (instance->flags >> 3) & 1;
    }
    display = (NUDISPLAY_SPECIAL_PC *)special->display_special;
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
        special->special = (NUMTX *)((NUSPECIAL_LEGACY_PC *)special->special + 1);
    } else if (special->display_special != NULL) {
        special->display_special = (NUMTX *)((NUDISPLAY_SPECIAL_PC *)special->display_special + 1);
    }
}

/* Retail's debug print, compiled empty; varargs keeps the calls out-of-line. */
void __cdecl PCSpecialEmptyHandler_candidate(const char *format, ...) {
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
        PCSpecialEmptyHandler_candidate(NULL); /* retail pushes no argument here */
    }
    return 0;
}

i32 __cdecl NuSpecialGetNumSpecials(NUGSCN *scene) {
    if (scene->special_count != 0)
        return scene->special_count;
    if (scene->display_list != NULL && scene->display_list->special_count != 0)
        return scene->display_list->special_count;
    return 0;
}

i32 __cdecl NuSpecialGetOnScreenFn(NUSPECIALHANDLE *special) {
    NUSPECIAL_LEGACY_PC *legacy;
    NUINSTANCE *instance;
    if (special->scene != NULL && special->special != NULL) {
        legacy = (NUSPECIAL_LEGACY_PC *)special->special;
        instance = (NUINSTANCE *)legacy->instance;
        return (instance->flags >> 1) & 1;
    }
    return 1;
}

i32 __cdecl NuSpecialGetVisibilityFn(NUSPECIALHANDLE *special) {
    NUSPECIAL_LEGACY_PC *legacy;
    NUINSTANCE *instance;
    NUDISPLAY_SPECIAL_PC *display;
    if (special->scene == NULL) {
        return 0;
    }
    legacy = (NUSPECIAL_LEGACY_PC *)special->special;
    if (legacy != NULL) {
        instance = (NUINSTANCE *)legacy->instance;
        return instance->flags & 1;
    }
    display = (NUDISPLAY_SPECIAL_PC *)special->display_special;
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

i32 __cdecl ShadowSetClipStatePc(i32 enabled, i32 state) {
    i32 previous = PCSpecialClipState;
    PCSpecialClipState = enabled == 0 ? -1 : state;
    return previous;
}

void __cdecl ShadowMapEnablePc(struct PC_SPECIAL_SHADOW_LIGHT *light) {
    if (PCSpecialActiveShadowLights < 4 && light->enabled != 0 && light->used_on_specials == 0) {
        light->used_on_specials = 1;
        PCSpecialShadowLights[PCSpecialActiveShadowLights] = light;
        PCSpecialShadowClipResultsPresent = 0;
        ++PCSpecialActiveShadowLights;
    }
}

void __cdecl ShadowMapResetPc(void) {
    i32 i = 0;
    while (i != PCSpecialActiveShadowLights) {
        PCSpecialShadowLights[i]->used_on_specials = 0;
        ++i;
    }
    PCSpecialShadowClipResultsPresent = 0;
    PCSpecialActiveShadowLights = 0;
}

void __cdecl ShadowResetClipResultsPc(void) {
    PCSpecialShadowClipResultsPresent = 0;
}

void __cdecl NuSpecialSetVisibility(NUSPECIALHANDLE *handle, i32 visible) {
    NUDISPLAY_SPECIAL_PC *display;
    NUDISPLAY_LIST_PC *display_list;

    if (handle == NULL || handle->scene == NULL)
        return;
    if (handle->special != NULL) {
        if (SPECIAL_LEGACY(handle)->instance != NULL)
            SPECIAL_LEGACY(handle)->instance->visible = visible;
        if (visible != 0)
            SPECIAL_LEGACY(handle)->flags |= 0x200;
        else
            SPECIAL_LEGACY(handle)->flags &= ~0x200;
        return;
    }
    display = SPECIAL_DISPLAY(handle);
    if (display == NULL)
        return;
    if (visible != 0) {
        display->flags |= 0x202;
        display_list = ((NUGSCN *)handle->scene)->display_list;
        if ((display_list->instance_visibility_enabled & 1) != 0)
            display_list->visibility_flags[display->instance_ix] |= 1;
    } else {
        display->flags &= ~0x2;
        SPECIAL_DISPLAY(handle)->flags &= ~0x200;
        display_list = ((NUGSCN *)handle->scene)->display_list;
        if ((display_list->instance_visibility_enabled & 1) != 0)
            display_list->visibility_flags[display->instance_ix] &= ~1;
    }
}

/* Shared private helper (native EAX source, ECX destination): component-wise |v|. */
static __declspec(noinline) void NuVec4AbsPrivatePc(NUVEC4 *out, const NUVEC4 *in) {
    f32 value;
    u32 bits;
    value = in->x;
    bits = *(u32 *)&value & 0x7fffffff;
    out->x = *(f32 *)&bits;
    value = in->y;
    bits = *(u32 *)&value & 0x7fffffff;
    out->y = *(f32 *)&bits;
    value = in->z;
    bits = *(u32 *)&value & 0x7fffffff;
    out->z = *(f32 *)&bits;
    value = in->w;
    bits = *(u32 *)&value & 0x7fffffff;
    out->w = *(f32 *)&bits;
}

void __cdecl NuDisplayListUpdateSpecial(NUSPECIALHANDLE *special) {
    NUDISPLAY_SPECIAL_PC *display = (NUDISPLAY_SPECIAL_PC *)special->display_special;
    NUDISPLAY_LIST_PC *display_scene = ((NUGSCN *)special->scene)->display_list;
    __declspec(align(16)) NUMTX draw_matrix = *NuSpecialGetDrawMtx(special);
    NUVEC4 half_extent;
    NUVEC4 center;
    NUVEC4 abs_row[3];
    NUVEC4 world_extent;

    half_extent.x = display->bounds_max.x - display->bounds_min.x;
    half_extent.y = display->bounds_max.y - display->bounds_min.y;
    half_extent.z = display->bounds_max.z - display->bounds_min.z;
    half_extent.x *= 0.5;
    half_extent.y *= 0.5;
    half_extent.z *= 0.5;
    center.x = display->bounds_max.x + display->bounds_min.x;
    center.y = display->bounds_max.y + display->bounds_min.y;
    center.z = display->bounds_max.z + display->bounds_min.z;
    center.x *= 0.5;
    center.y *= 0.5;
    center.z *= 0.5;

    {
        f32 value;
        u32 bits;
        value = draw_matrix.m00;
        bits = *(u32 *)&value & 0x7fffffff;
        abs_row[0].x = *(f32 *)&bits;
        value = draw_matrix.m01;
        bits = *(u32 *)&value & 0x7fffffff;
        abs_row[0].y = *(f32 *)&bits;
        value = draw_matrix.m02;
        bits = *(u32 *)&value & 0x7fffffff;
        abs_row[0].z = *(f32 *)&bits;
    }
    NuVec4AbsPrivatePc(&abs_row[1], (NUVEC4 *)&draw_matrix.m10);
    NuVec4AbsPrivatePc(&abs_row[2], (NUVEC4 *)&draw_matrix.m20);
    world_extent.x = abs_row[0].x * half_extent.x + abs_row[1].x * half_extent.y + abs_row[2].x * half_extent.z;
    world_extent.y = abs_row[0].y * half_extent.x + abs_row[1].y * half_extent.y + abs_row[2].y * half_extent.z;
    world_extent.z = abs_row[0].z * half_extent.x + abs_row[1].z * half_extent.y + abs_row[2].z * half_extent.z;

    NuVecMtxTransform((NUVEC *)&center, (NUVEC *)&center, &draw_matrix);
    center.w = NuFsqrt(world_extent.x + world_extent.y + world_extent.z);
    if ((display_scene->render_flags & 0x4000) != 0) {
        display_scene->clip_bounds[display->instance_ix * 2] = center;
        display_scene->clip_bounds[display->instance_ix * 2 + 1] = world_extent;
    } else {
        NUVEC4 minimum;
        NUVEC4 maximum;
        minimum.x = center.x - world_extent.x;
        minimum.y = center.y - world_extent.y;
        minimum.z = center.z - world_extent.z;
        maximum.x = center.x + world_extent.x;
        maximum.y = center.y + world_extent.y;
        maximum.z = center.z + world_extent.z;
        display_scene->clip_bounds[display->instance_ix * 2] = minimum;
        display_scene->clip_bounds[display->instance_ix * 2 + 1] = maximum;
    }

    DisplayListUpdateSpecialTransformPS(special, &draw_matrix);
}

void __cdecl xxxNuDisplayListUpdateSpecial(NUSPECIALHANDLE *special) {
    NUDISPLAY_LIST_PC *scene = ((NUGSCN *)special->scene)->display_list;
    NUDISPLAY_SPECIAL_PC *display = (NUDISPLAY_SPECIAL_PC *)special->display_special;
    __declspec(align(16)) NUMTX matrix = *NuSpecialGetDrawMtx(special);
    __declspec(align(16)) NUVEC4 corners[8];
    __declspec(align(16)) NUVEC4 maximum;
    __declspec(align(16)) NUVEC4 minimum;

    corners[0].x = display->bounds_min.x;
    corners[0].y = display->bounds_min.y;
    corners[0].z = display->bounds_min.z;
    corners[1].x = display->bounds_max.x;
    corners[1].y = display->bounds_max.y;
    corners[1].z = display->bounds_max.z;
    corners[2].x = display->bounds_min.x;
    corners[2].y = display->bounds_min.y;
    corners[2].z = display->bounds_max.z;
    corners[3].x = display->bounds_max.x;
    corners[3].y = display->bounds_max.y;
    corners[3].z = display->bounds_min.z;
    corners[4].x = display->bounds_min.x;
    corners[4].y = display->bounds_max.y;
    corners[4].z = display->bounds_min.z;
    corners[5].x = display->bounds_min.x;
    corners[5].y = display->bounds_max.y;
    corners[5].z = display->bounds_max.z;
    corners[6].x = display->bounds_max.x;
    corners[6].y = display->bounds_min.y;
    corners[6].z = display->bounds_min.z;
    corners[7].x = display->bounds_max.x;
    corners[7].y = display->bounds_min.y;
    corners[7].z = display->bounds_max.z;
    NuVecMtxTransform((NUVEC *)&corners[0], (NUVEC *)&corners[0], &matrix);
    NuVecMtxTransform((NUVEC *)&corners[1], (NUVEC *)&corners[1], &matrix);
    NuVecMtxTransform((NUVEC *)&corners[2], (NUVEC *)&corners[2], &matrix);
    NuVecMtxTransform((NUVEC *)&corners[3], (NUVEC *)&corners[3], &matrix);
    NuVecMtxTransform((NUVEC *)&corners[4], (NUVEC *)&corners[4], &matrix);
    NuVecMtxTransform((NUVEC *)&corners[5], (NUVEC *)&corners[5], &matrix);
    NuVecMtxTransform((NUVEC *)&corners[6], (NUVEC *)&corners[6], &matrix);
    NuVecMtxTransform((NUVEC *)&corners[7], (NUVEC *)&corners[7], &matrix);
    NuVecMin((NUVEC *)&minimum, (NUVEC *)&corners[0], (NUVEC *)&corners[1]);
    NuVecMin((NUVEC *)&minimum, (NUVEC *)&minimum, (NUVEC *)&corners[2]);
    NuVecMin((NUVEC *)&minimum, (NUVEC *)&minimum, (NUVEC *)&corners[3]);
    NuVecMin((NUVEC *)&minimum, (NUVEC *)&minimum, (NUVEC *)&corners[4]);
    NuVecMin((NUVEC *)&minimum, (NUVEC *)&minimum, (NUVEC *)&corners[5]);
    NuVecMin((NUVEC *)&minimum, (NUVEC *)&minimum, (NUVEC *)&corners[6]);
    NuVecMin((NUVEC *)&minimum, (NUVEC *)&minimum, (NUVEC *)&corners[7]);
    NuVecMax((NUVEC *)&maximum, (NUVEC *)&corners[0], (NUVEC *)&corners[1]);
    NuVecMax((NUVEC *)&maximum, (NUVEC *)&maximum, (NUVEC *)&corners[2]);
    NuVecMax((NUVEC *)&maximum, (NUVEC *)&maximum, (NUVEC *)&corners[3]);
    NuVecMax((NUVEC *)&maximum, (NUVEC *)&maximum, (NUVEC *)&corners[4]);
    NuVecMax((NUVEC *)&maximum, (NUVEC *)&maximum, (NUVEC *)&corners[5]);
    NuVecMax((NUVEC *)&maximum, (NUVEC *)&maximum, (NUVEC *)&corners[6]);
    NuVecMax((NUVEC *)&maximum, (NUVEC *)&maximum, (NUVEC *)&corners[7]);
    if ((scene->render_flags & 0x4000) != 0) {
        __declspec(align(16)) NUVEC4 center;
        __declspec(align(16)) NUVEC4 extent;
        center.x = minimum.x + maximum.x;
        center.y = minimum.y + maximum.y;
        center.z = minimum.z + maximum.z;
        center.x *= 0.5f;
        center.y *= 0.5f;
        center.z *= 0.5f;
        extent.x = maximum.x - minimum.x;
        extent.y = maximum.y - minimum.y;
        extent.z = maximum.z - minimum.z;
        extent.x *= 0.5f;
        extent.y *= 0.5f;
        extent.z *= 0.5f;
        center.w = NuFsqrt(extent.y + extent.x + extent.z);
        scene->clip_bounds[display->instance_ix * 2] = center;
        scene->clip_bounds[display->instance_ix * 2 + 1] = extent;
    } else {
        scene->clip_bounds[display->instance_ix * 2] = minimum;
        scene->clip_bounds[display->instance_ix * 2 + 1] = maximum;
    }
    DisplayListUpdateSpecialTransformPS(special, &matrix);
}

void __cdecl NuSpecialUpdate(NUSPECIALHANDLE *special) {
    if (special == NULL || special->display_special == NULL || special->scene == NULL)
        return;
    if (((NUGSCN *)special->scene)->display_list == NULL ||
        (((NUGSCN *)special->scene)->display_list->instance_visibility_enabled & 1) == 0)
        return;
    SPECIAL_DISPLAY(special)->flags |= 0x400;
    NuDisplayListUpdateSpecial(special);
}

i32 __cdecl NuSpecialGetInstanceix(NUSPECIALHANDLE *handle) {
    NUSPECIAL_LEGACY_PC *legacy = (NUSPECIAL_LEGACY_PC *)handle->special;
    i32 i;
    if (legacy != NULL) {
        NUGSCN *scene = (NUGSCN *)handle->scene;
        for (i = 0; i < scene->num_instances; ++i) {
            if (&scene->instances[i] == legacy->instance)
                return i;
        }
        return -1;
    }
    if (handle->display_special != NULL)
        return ((NUDISPLAY_SPECIAL_PC *)handle->display_special)->instance_ix;
    return -1;
}

i32 special_debug = 0;

/* Debug listing of a scene's specials; the retail print call is the empty handler. */
void __cdecl NuSpecialList(NUGSCN *scene) {
    NUSPECIAL_LEGACY_PC *special;
    NUDISPLAY_SPECIAL_PC *display_special;
    i32 index;

    if (scene == NULL)
        return;
    special = scene->specials;
    for (index = 0; index < scene->special_count; ++index, ++special)
        PCSpecialEmptyHandler_candidate((const char *)index, special->name);
    if (scene->display_list != NULL) {
        display_special = scene->display_list->specials;
        for (index = 0; index < scene->display_list->special_count; ++index, ++display_special)
            PCSpecialEmptyHandler_candidate((const char *)index, display_special->name);
    }
}

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
        PCSpecialEmptyHandler_candidate(name);
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
    if (scene == NULL || name == NULL) {
        handle->scene = NULL;
        handle->special = NULL;
        handle->display_special = NULL;
        return 0;
    }
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

void __cdecl NuSpecialSetDrawMtx(NUSPECIALHANDLE *special, NUMTX *matrix) {
    NUSPECIAL_LEGACY_PC *legacy;
    NUDISPLAY_SPECIAL_PC *display;
    NUDISPLAY_LIST_PC *display_list;

    if (special == NULL || special->scene == NULL) {
        return;
    }
    legacy = (NUSPECIAL_LEGACY_PC *)special->special;
    if (legacy != NULL) {
        ((NUINSTANCE *)legacy->instance)->matrix = *matrix;
        return;
    }
    display = (NUDISPLAY_SPECIAL_PC *)special->display_special;
    display->draw_matrix = *matrix;
    display_list = ((NUGSCN *)special->scene)->display_list;
    if ((display_list->instance_visibility_enabled & 1) != 0) {
        NuDisplayListUpdateSpecial(special);
        ((NUDISPLAY_SPECIAL_PC *)special->display_special)->flags |= 0x400;
    }
}

void __cdecl NuSpecialGetBounds(NUSPECIALHANDLE *special, NUVEC *minimum, NUVEC *maximum) {
    NUGOBJ *object;
    if (special->special != NULL) {
        object = SPECIAL_GOBJ(special);
        while (object->next != NULL)
            object = object->next;
        *minimum = object->bounds_min;
        *maximum = object->bounds_max;
    } else if (special->display_special != NULL) {
        *minimum = *(NUVEC *)&SPECIAL_DISPLAY(special)->bounds_min;
        *maximum = *(NUVEC *)&SPECIAL_DISPLAY(special)->bounds_max;
    }
}

void __cdecl NuSpecialSetDrawPos(nuhspecial_s *special_pointer, NUVEC *position) {
    NUSPECIALHANDLE *special = (NUSPECIALHANDLE *)special_pointer;
    NUDISPLAY_LIST_PC *display_list;
    if (special == NULL || special->scene == NULL)
        return;
    if (special->special != NULL) {
        ((NUINSTANCE *)((NUSPECIAL_LEGACY_PC *)special->special)->instance)->matrix.m30 = position->x;
        ((NUINSTANCE *)((NUSPECIAL_LEGACY_PC *)special->special)->instance)->matrix.m31 = position->y;
        ((NUINSTANCE *)((NUSPECIAL_LEGACY_PC *)special->special)->instance)->matrix.m32 = position->z;
    } else {
        ((NUDISPLAY_SPECIAL_PC *)special->display_special)->draw_matrix.m30 = position->x;
        ((NUDISPLAY_SPECIAL_PC *)special->display_special)->draw_matrix.m31 = position->y;
        ((NUDISPLAY_SPECIAL_PC *)special->display_special)->draw_matrix.m32 = position->z;
        display_list = ((NUGSCN *)special->scene)->display_list;
        if ((display_list->instance_visibility_enabled & 1) != 0) {
            NuDisplayListUpdateSpecial(special);
            ((NUDISPLAY_SPECIAL_PC *)special->display_special)->flags |= 0x400;
        }
    }
}
