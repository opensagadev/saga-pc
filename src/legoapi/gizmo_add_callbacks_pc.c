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
    i16 reset : 1;
    i16 reset_saved : 1;
    i16 reset_flags_unused : 14;
    u16 opaque_0e;
    void *progress;
} ADD_BOMB_RESET_SYS_PC;
STATIC_ASSERT(sizeof(ADD_BUILDIT_PC) == 0x7c);
STATIC_ASSERT(sizeof(ADD_FORCE_PC) == 0xa4);
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
STATIC_ASSERT(offsetof(ADD_BOMB_RESET_SYS_PC, opaque_0e) == 0xe);
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
STATIC_ASSERT(offsetof(struct GIZBOMBGENSYS_s, count) == 4);
STATIC_ASSERT(offsetof(struct GIZPANELSYS_s, panels) == 4);

void __cdecl Blowup_AddGizmos(GIZMOSYS_PC *giz, i32 type, void *world_ptr, void *data) {
    WORLDINFO *w = (WORLDINFO *)world_ptr;
    i32 i;
    (void)data;
    for (i = 0; i < w->gizmo_blowup_count; ++i)
        if (NuStrLen(w->gizmo_blowups[i].name) != 0)
            AddGizmo(giz, type, NULL, &w->gizmo_blowups[i]);
}

STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);
STATIC_ASSERT(offsetof(GIZSPINNER_s, name) == 0x40);
STATIC_ASSERT(offsetof(GIZMOBLOWUP_PC, name) == 0xf2);

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
NUVEC *__cdecl GizmoBlowup_GetPos(GIZMO_PC *gizmo) {
    if (gizmo == NULL)
        return NULL;
    return &((FLOW_BLOWUP_POSITION_PC *)gizmo->object)->position;
}

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
