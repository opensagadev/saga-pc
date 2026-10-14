/* Native PC array strides and callback owner views. */
#include "saga.h"

typedef struct {
    char name[16];
    u8 opaque[0x7c - 16];
} ADD_BUILDIT_PC;
typedef struct {
    char name[16];
    u8 opaque[0xa4 - 16];
} ADD_FORCE_PC;
typedef struct {
    u8 opaque_00[8];
    char name[16];
    u8 opaque_18[0x13c - 0x18];
} ADD_TURRET_PC;
typedef struct {
    u8 opaque_00[0x40];
    char name[16];
    u8 opaque_50[0x90 - 0x50];
} ADD_PANEL_PC;
typedef struct {
    char name[16];
    u8 opaque[0xfc - 16];
} ADD_TELEPORT_PC;
typedef struct {
    char name[16];
    u8 opaque[0x2b - 16];
    u8 flags;
} ADD_TORP_PC;
typedef struct {
    char name[16];
    u8 opaque[0xf8 - 16];
} ADD_GRAPPLE_PC;
typedef struct {
    ADD_TORP_PC *items;
    i32 count;
    f32 scale;
} ADD_TORP_SYS_PC;
typedef struct {
    u8 opaque[0x4680];
    ADD_TELEPORT_PC *items;
    i32 count;
} ADD_TELEPORT_WORLD_PC;
typedef struct {
    u8 opaque[0x2aec];
    i32 count;
    struct LEVELSCRIPTPROCESS_s items[32];
} ADD_AI_WORLD_PC;
typedef struct {
    GIZBOMBGEN_s *items;
    u16 count, capacity;
    void *pool;
    u16 reset_flags;
    u16 opaque_0e;
    void *progress;
} ADD_BOMB_RESET_SYS_PC;
extern i32 come_from_an_editor;
STATIC_ASSERT(sizeof(ADD_BUILDIT_PC) == 0x7c);
STATIC_ASSERT(sizeof(ADD_FORCE_PC) == 0xa4);
STATIC_ASSERT(sizeof(ADD_TURRET_PC) == 0x13c);
STATIC_ASSERT(offsetof(ADD_TURRET_PC, name) == 8);
STATIC_ASSERT(sizeof(ADD_PANEL_PC) == 0x90);
STATIC_ASSERT(offsetof(ADD_PANEL_PC, name) == 0x40);
STATIC_ASSERT(sizeof(ADD_TELEPORT_PC) == 0xfc);
STATIC_ASSERT(sizeof(ADD_TORP_PC) == 0x2c);
STATIC_ASSERT(offsetof(ADD_TORP_PC, flags) == 0x2b);
STATIC_ASSERT(sizeof(ADD_GRAPPLE_PC) == 0xf8);
STATIC_ASSERT(offsetof(ADD_TORP_SYS_PC, count) == 4);
STATIC_ASSERT(offsetof(ADD_TELEPORT_WORLD_PC, items) == 0x4680);
STATIC_ASSERT(offsetof(ADD_TELEPORT_WORLD_PC, count) == 0x4684);
STATIC_ASSERT(offsetof(ADD_AI_WORLD_PC, count) == 0x2aec);
STATIC_ASSERT(offsetof(ADD_AI_WORLD_PC, items) == 0x2af0);
STATIC_ASSERT(sizeof(struct LEVELSCRIPTPROCESS_s) == 0xdc);
STATIC_ASSERT(offsetof(ADD_BOMB_RESET_SYS_PC, reset_flags) == 0xc);
STATIC_ASSERT(offsetof(ADD_BOMB_RESET_SYS_PC, progress) == 0x10);
STATIC_ASSERT(sizeof(struct GIZSPINNER_s) == 0x304);
STATIC_ASSERT(offsetof(struct GIZSPINNER_s, name) == 0x40);
STATIC_ASSERT(offsetof(struct GIZSPINNER_s, flags) == 0xac);
STATIC_ASSERT(offsetof(WORLDINFO, spinners) == 0x46e0);
STATIC_ASSERT(offsetof(WORLDINFO, gizmo_blowup_count) == 0x50c0);
STATIC_ASSERT(offsetof(WORLDINFO, gizmo_blowups) == 0x50c8);
STATIC_ASSERT(offsetof(WORLDINFO, giz_panel_sys) == 0x5070);
STATIC_ASSERT(offsetof(WORLDINFO, grapples) == 0x5080);
STATIC_ASSERT(offsetof(WORLDINFO, grapple_count) == 0x5084);
STATIC_ASSERT(offsetof(WORLDINFO, torpedo_machine_sys) == 0x5100);
STATIC_ASSERT(offsetof(struct GIZBUILDITSYS_s, count) == 4);
STATIC_ASSERT(offsetof(struct GIZFORCESYS_s, count) == 0xe);
STATIC_ASSERT(offsetof(struct GIZTURRETSYS_s, count) == 8);
STATIC_ASSERT(offsetof(struct GIZBOMBGENSYS_s, count) == 4);
STATIC_ASSERT(offsetof(struct GIZPANELSYS_s, panels) == 4);

void __cdecl GizBuildIts_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world, void *data) {
    GIZBUILDITSYS_s *s = (GIZBUILDITSYS_s *)data;
    i32 i;
    (void)world;
    if (s == NULL)
        return;
    for (i = 0; i < s->count; ++i)
        if (NuStrLen(((ADD_BUILDIT_PC *)s->buildits)[i].name) != 0)
            AddGizmo(giz, type, NULL, &((ADD_BUILDIT_PC *)s->buildits)[i]);
}
void __cdecl GizForces_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world, void *data) {
    GIZFORCESYS_s *s = (GIZFORCESYS_s *)data;
    i32 i;
    (void)world;
    if (s == NULL)
        return;
    for (i = 0; i < s->count; ++i)
        if (NuStrLen(((ADD_FORCE_PC *)s->forces)[i].name) != 0)
            AddGizmo(giz, type, NULL, &((ADD_FORCE_PC *)s->forces)[i]);
}
void __cdecl Blowup_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world_ptr, void *data) {
    WORLDINFO *w = (WORLDINFO *)world_ptr;
    i32 i;
    (void)data;
    for (i = 0; i < w->gizmo_blowup_count; ++i)
        if (NuStrLen(w->gizmo_blowups[i].name) != 0)
            AddGizmo(giz, type, NULL, &w->gizmo_blowups[i]);
}
void __cdecl GizmoPickups_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world_ptr, void *data) {
    WORLDINFO *w = (WORLDINFO *)world_ptr;
    i32 i;
    (void)data;
    for (i = 0; i < ((struct GIZMOPICKUPRUNTIMESYS_s *)w->pickup_runtime_sys)->pickup_count; ++i)
        if (((struct GIZMOPICKUPRUNTIMESYS_s *)w->pickup_runtime_sys)->pickups[i].unknown_14[1] & 2)
            AddGizmo(giz, type, NULL, &((struct GIZMOPICKUPRUNTIMESYS_s *)w->pickup_runtime_sys)->pickups[i]);
}
void __cdecl GizSpinner_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world_ptr, void *data) {
    WORLDINFO *w = (WORLDINFO *)world_ptr;
    i32 i;
    (void)data;
    if (w->spinners == NULL)
        return;
    for (i = 0; i < w->current_level->max_spinners; ++i)
        if ((w->spinners[i].flags & 1) && NuStrLen(w->spinners[i].name) != 0)
            AddGizmo(giz, type, NULL, &w->spinners[i]);
}
void __cdecl GizTurrets_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world, void *data) {
    GIZTURRETSYS_s *s = (GIZTURRETSYS_s *)data;
    i32 i;
    (void)world;
    if (s == NULL)
        return;
    for (i = 0; i < s->count; ++i)
        if (NuStrLen(((ADD_TURRET_PC *)s->turrets)[i].name) != 0)
            AddGizmo(giz, type, NULL, &((ADD_TURRET_PC *)s->turrets)[i]);
}
void __cdecl GizBombGens_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world, void *data) {
    GIZBOMBGENSYS_s *s = (GIZBOMBGENSYS_s *)data;
    i32 i;
    (void)world;
    if (s == NULL)
        return;
    for (i = 0; i < s->count; ++i)
        if (NuStrLen(s->bomb_generators[i].name) != 0)
            AddGizmo(giz, type, NULL, &s->bomb_generators[i]);
}
void __cdecl AI_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world_ptr, void *data) {
    struct LEVELSCRIPTPROCESS_s *item = ((ADD_AI_WORLD_PC *)world_ptr)->items;
    i32 i;
    (void)data;
    if (item == NULL)
        return;
    for (i = 0; i < ((ADD_AI_WORLD_PC *)WORLD)->count; ++i, ++item)
        AddGizmo(giz, type, NULL, item);
}
void __cdecl GizAIMessage_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world, void *data) {
    GIZAIMESSAGE *message;
    (void)type;
    (void)world;
    (void)data;
    if (gizaimessagesys == NULL)
        return;
    message = (GIZAIMESSAGE *)NuLinkedListGetHead(&gizaimessagesys->active_list);
    while (message != NULL) {
        if (message->flags & 1) {
            AddGizmo(giz, gizaimessage_gizmotype_id, NULL, message);
            message->flags |= 2;
        }
        message = (GIZAIMESSAGE *)NuLinkedListGetNext(&gizaimessagesys->active_list, &message->links);
    }
}
void __cdecl GizPanel_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world_ptr, void *data) {
    WORLDINFO *w = (WORLDINFO *)world_ptr;
    i32 i;
    (void)data;
    if (w == NULL || w->giz_panel_sys == NULL)
        return;
    for (i = 0; i < w->giz_panel_sys->count; ++i)
        if (NuStrLen(((ADD_PANEL_PC *)w->giz_panel_sys->panels)[i].name) != 0)
            AddGizmo(giz, type, NULL, &((ADD_PANEL_PC *)w->giz_panel_sys->panels)[i]);
}
void __cdecl Teleport_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world_ptr, void *data) {
    ADD_TELEPORT_WORLD_PC *w = (ADD_TELEPORT_WORLD_PC *)world_ptr;
    i32 i;
    (void)data;
    if (w == NULL || w->items == NULL)
        return;
    for (i = 0; i < w->count; ++i)
        AddGizmo(giz, type, NULL, &w->items[i]);
}
void __cdecl GizTorp_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world_ptr, void *data) {
    WORLDINFO *w = (WORLDINFO *)world_ptr;
    ADD_TORP_SYS_PC *s;
    i32 i;
    (void)data;
    if (w == NULL || w->torpedo_machine_sys == NULL)
        return;
    s = (ADD_TORP_SYS_PC *)w->torpedo_machine_sys;
    for (i = 0; i < s->count; ++i) {
        if (NuStrLen(s->items[i].name) != 0)
            AddGizmo(giz, type, NULL, &((ADD_TORP_SYS_PC *)w->torpedo_machine_sys)->items[i]);
        s = (ADD_TORP_SYS_PC *)w->torpedo_machine_sys;
    }
}
void __cdecl Grapples_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world_ptr, void *data) {
    WORLDINFO *w = (WORLDINFO *)world_ptr;
    i32 i;
    (void)data;
    for (i = 0; i < w->grapple_count; ++i)
        if (NuStrLen(((ADD_GRAPPLE_PC *)w->grapples)[i].name) != 0)
            AddGizmo(giz, type, NULL, &((ADD_GRAPPLE_PC *)w->grapples)[i]);
}
void __cdecl GizBombGens_SetResetFlag(void *world_ptr, void *data, void *progress) {
    WORLDINFO *w = (WORLDINFO *)world_ptr;
    ADD_BOMB_RESET_SYS_PC *s = (ADD_BOMB_RESET_SYS_PC *)data;
    s->reset_flags |= 1;
    if (come_from_an_editor != 0)
        s->reset_flags &= (u16)~2;
    else
        s->reset_flags = (u16)((s->reset_flags & ~2u) | (((u8)w->unknown_0114[1] & 1u) << 1));
    s->progress = progress;
}
void __cdecl GizTorps_Reset(void *world_ptr, void *data, void *progress) {
    WORLDINFO *w = (WORLDINFO *)world_ptr;
    ADD_TORP_SYS_PC *s;
    i32 i;
    (void)data;
    (void)progress;
    if (w == NULL || w->torpedo_machine_sys == NULL)
        return;
    s = (ADD_TORP_SYS_PC *)w->torpedo_machine_sys;
    if (s->items == NULL)
        return;
    for (i = 0; i < s->count; ++i)
        s->items[i].flags |= 3;
}

STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);
STATIC_ASSERT(offsetof(GIZSPINNER_s, name) == 0x40);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, name) == 0xf2);

char *__cdecl GizmoForce_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? (char *)gizmo->object : NULL;
}
char *__cdecl GizmoBuildit_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? (char *)gizmo->object : NULL;
}
char *__cdecl GizmoPickup_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? (char *)gizmo->object : NULL;
}
char *__cdecl GizSpinner_GetGizmoName(void *gizmo_pointer) {
    GIZMO_PC *gizmo = (GIZMO_PC *)gizmo_pointer;
    return gizmo != NULL ? ((GIZSPINNER_s *)gizmo->object)->name : NULL;
}
char *__cdecl Blowup_GetGizmoName(GIZMO_PC *gizmo) {
    return gizmo != NULL ? ((GIZMOBLOWUP_PC *)gizmo->object)->name : NULL;
}

/* Append gizmo_add_callbacks_pc.c: complete native21+21+15+15+21. */
typedef struct {
    u8 unknown_00[0x1c];
    NUVEC position;
} FLOW_FORCE_POSITION_PC;
typedef struct {
    u8 unknown_00[0x34];
    NUVEC position;
} FLOW_BUILDIT_POSITION_PC;
typedef struct {
    u8 unknown_00[0x30];
    NUVEC position;
} FLOW_BLOWUP_POSITION_PC;
STATIC_ASSERT(offsetof(FLOW_FORCE_POSITION_PC, position) == 0x1c);
STATIC_ASSERT(offsetof(FLOW_BUILDIT_POSITION_PC, position) == 0x34);
STATIC_ASSERT(offsetof(FLOW_BLOWUP_POSITION_PC, position) == 0x30);
STATIC_ASSERT(offsetof(GIZSPINNER_s, position) == 0x5c);
STATIC_ASSERT(offsetof(GIZMOPICKUP_PC, position) == 8);
NUVEC *__cdecl GizmoForce_GetPos(GIZMO_PC *gizmo) {
    if (gizmo == NULL || gizmo->object == NULL)
        return NULL;
    return &((FLOW_FORCE_POSITION_PC *)gizmo->object)->position;
}
NUVEC *__cdecl GizSpinner_GetPos(void *gizmo_pointer) {
    GIZMO_PC *gizmo = (GIZMO_PC *)gizmo_pointer;
    if (gizmo == NULL || gizmo->object == NULL)
        return NULL;
    return &((GIZSPINNER_s *)gizmo->object)->position;
}
NUVEC *__cdecl GizmoBlowup_GetPos(GIZMO_PC *gizmo) {
    if (gizmo == NULL)
        return NULL;
    return &((FLOW_BLOWUP_POSITION_PC *)gizmo->object)->position;
}
NUVEC *__cdecl GizmoPickup_GetPos(GIZMO_PC *gizmo) {
    if (gizmo == NULL)
        return NULL;
    return &((GIZMOPICKUP_PC *)gizmo->object)->position;
}
NUVEC *__cdecl GizmoBuildit_GetPos(GIZMO_PC *gizmo) {
    if (gizmo == NULL || gizmo->object == NULL)
        return NULL;
    return &((FLOW_BUILDIT_POSITION_PC *)gizmo->object)->position;
}

/* Append gizmo_add_callbacks_pc.c. Native4cf150..4cf16017B. */
char *__cdecl Teleport_GetGizmoName(GIZMO_PC *gizmo) {
    if (gizmo == NULL || gizmo->object == NULL)
        return NULL;
    return (char *)gizmo->object;
}

extern char AIGizmoNameBufferPc[16];

typedef struct ai_gizmo_name_process_pc_s {
    char name[16];
    u8 unknown_10[4];
    AISCRIPT_PC *script;
    u8 unknown_18[0x98];
    char *context_name;
    char *locator_name;
    char *locator_set_name;
    u8 unknown_bc[0x20];
} AI_GIZMO_NAME_PROCESS_PC;
STATIC_ASSERT(sizeof(AI_GIZMO_NAME_PROCESS_PC) == 0xdc);
STATIC_ASSERT(offsetof(AI_GIZMO_NAME_PROCESS_PC, name) == 0);
STATIC_ASSERT(offsetof(AI_GIZMO_NAME_PROCESS_PC, script) == 0x14);
STATIC_ASSERT(offsetof(AI_GIZMO_NAME_PROCESS_PC, context_name) == 0xb0);
STATIC_ASSERT(offsetof(AI_GIZMO_NAME_PROCESS_PC, locator_name) == 0xb4);
STATIC_ASSERT(offsetof(AI_GIZMO_NAME_PROCESS_PC, locator_set_name) == 0xb8);
STATIC_ASSERT(offsetof(AISCRIPT_PC, name) == 8);
STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);

char *__cdecl AI_GetGizmoName(void *gizmo_pointer) {
    GIZMO_PC *gizmo = (GIZMO_PC *)gizmo_pointer;
    AI_GIZMO_NAME_PROCESS_PC *process;
    i8 length, total;
    if (gizmo == NULL || gizmo->object == NULL)
        return NULL;
    process = (AI_GIZMO_NAME_PROCESS_PC *)gizmo->object;
    if (NuStrLen(process->name) != 0)
        return process->name;
    if (process->script != NULL && process->script->name != NULL)
        NuStrNCpy(AIGizmoNameBufferPc, process->script->name, 16);
    else
        NuStrCpy(AIGizmoNameBufferPc, "");
    length = (i8)NuStrLen(AIGizmoNameBufferPc);
    if (process->locator_set_name != NULL) {
        total = (i8)((u8)length + (u8)NuStrLen("()"));
        total = (i8)((u8)total + (u8)NuStrLen(process->locator_set_name));
        if (total < 16) {
            NuStrCat(AIGizmoNameBufferPc, "(");
            NuStrCat(AIGizmoNameBufferPc, process->locator_set_name);
            NuStrCat(AIGizmoNameBufferPc, ")");
        }
    } else if (process->locator_name != NULL) {
        total = (i8)((u8)length + (u8)NuStrLen("()"));
        total = (i8)((u8)total + (u8)NuStrLen(process->locator_name));
        if (total < 16) {
            NuStrCat(AIGizmoNameBufferPc, "(");
            NuStrCat(AIGizmoNameBufferPc, process->locator_name);
            NuStrCat(AIGizmoNameBufferPc, ")");
        }
    } else if (process->context_name != NULL) {
        total = (i8)((u8)length + (u8)NuStrLen("()"));
        total = (i8)((u8)total + (u8)NuStrLen(process->context_name));
        if (total < 16) {
            NuStrCat(AIGizmoNameBufferPc, "(");
            NuStrCat(AIGizmoNameBufferPc, process->context_name);
            NuStrCat(AIGizmoNameBufferPc, ")");
        }
    }
    return AIGizmoNameBufferPc;
}
