#include "saga.h"

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
STATIC_ASSERT(offsetof(NuSpecialDisplayView, instance_animation) == 0xc4);
STATIC_ASSERT(sizeof(NuSpecialDisplayView) == 0xd0);
STATIC_ASSERT(offsetof(NuSpecialDisplayListView, special_count) == 0x6c);
STATIC_ASSERT(offsetof(NuSpecialSceneView, special_count) == 0x24);
STATIC_ASSERT(offsetof(NuSpecialSceneView, display_list) == 0x110);

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
