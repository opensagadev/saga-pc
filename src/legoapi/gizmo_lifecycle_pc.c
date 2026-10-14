#include "saga.h"

/* These allocation records use the native PC strides, rather than the larger imported views. */
typedef struct registry_buildit_record_pc_s {
    u8 unknown_00[0x10];
    GAMEANIMSET *anim_set;
    u8 unknown_14[0x64];
    i16 blowup_type;
    u16 flags;
} REGISTRY_BUILDIT_RECORD_PC;
STATIC_ASSERT(sizeof(REGISTRY_BUILDIT_RECORD_PC) == 0x7c);
STATIC_ASSERT(offsetof(REGISTRY_BUILDIT_RECORD_PC, anim_set) == 0x10);
STATIC_ASSERT(offsetof(REGISTRY_BUILDIT_RECORD_PC, blowup_type) == 0x78);
STATIC_ASSERT(offsetof(REGISTRY_BUILDIT_RECORD_PC, flags) == 0x7a);

typedef struct registry_force_record_pc_s {
    u8 unknown_00[0x28];
    GAMEANIMSET *anim_set;
    u8 unknown_2c[0x5c];
    i16 blowup_type;
    u8 unknown_8a[0x16];
    u32 runtime_flags;
} REGISTRY_FORCE_RECORD_PC;
STATIC_ASSERT(sizeof(REGISTRY_FORCE_RECORD_PC) == 0xa4);
STATIC_ASSERT(offsetof(REGISTRY_FORCE_RECORD_PC, anim_set) == 0x28);
STATIC_ASSERT(offsetof(REGISTRY_FORCE_RECORD_PC, blowup_type) == 0x88);
STATIC_ASSERT(offsetof(REGISTRY_FORCE_RECORD_PC, runtime_flags) == 0xa0);

typedef struct registry_turret_record_pc_s {
    u8 unknown_00[0x18];
    GAMEANIMSET *anim_set;
    u8 unknown_1c[0x108];
    i16 blowup_type;
    u8 unknown_126[0x14];
    u16 flags;
} REGISTRY_TURRET_RECORD_PC;
STATIC_ASSERT(sizeof(REGISTRY_TURRET_RECORD_PC) == 0x13c);
STATIC_ASSERT(offsetof(REGISTRY_TURRET_RECORD_PC, anim_set) == 0x18);
STATIC_ASSERT(offsetof(REGISTRY_TURRET_RECORD_PC, blowup_type) == 0x124);
STATIC_ASSERT(offsetof(REGISTRY_TURRET_RECORD_PC, flags) == 0x13a);

typedef struct registry_panel_record_pc_s {
    u8 storage[0x90];
} REGISTRY_PANEL_RECORD_PC;
STATIC_ASSERT(sizeof(REGISTRY_PANEL_RECORD_PC) == 0x90);

typedef struct registry_buildit_system_pc_s {
    REGISTRY_BUILDIT_RECORD_PC *buildits;
    u16 count;
    u16 capacity;
    u16 max_animation_objects;
    u16 padding;
    GAMEANIMOBJPOOL *anim_pool;
    GAMEANIMOBJ **anim_objects;
} REGISTRY_BUILDIT_SYSTEM_PC;
STATIC_ASSERT(sizeof(REGISTRY_BUILDIT_SYSTEM_PC) == 0x14);
STATIC_ASSERT(offsetof(REGISTRY_BUILDIT_SYSTEM_PC, count) == 4);
STATIC_ASSERT(offsetof(REGISTRY_BUILDIT_SYSTEM_PC, capacity) == 6);
STATIC_ASSERT(offsetof(REGISTRY_BUILDIT_SYSTEM_PC, max_animation_objects) == 8);
STATIC_ASSERT(offsetof(REGISTRY_BUILDIT_SYSTEM_PC, anim_pool) == 0xc);
STATIC_ASSERT(offsetof(REGISTRY_BUILDIT_SYSTEM_PC, anim_objects) == 0x10);

typedef struct registry_force_system_pc_s {
    REGISTRY_FORCE_RECORD_PC *forces;
    REGISTRY_FORCE_RECORD_PC **visible_forces;
    GIZMO_PC **hit_test_gizmos;
    u16 capacity;
    u16 count;
    u16 visible_force_count;
    u16 hit_test_gizmo_count;
    GAMEANIMOBJPOOL *anim_pool;
    GIZFORCEGROUP_s groups[8];
} REGISTRY_FORCE_SYSTEM_PC;
STATIC_ASSERT(sizeof(REGISTRY_FORCE_SYSTEM_PC) == 0x158);
STATIC_ASSERT(offsetof(REGISTRY_FORCE_SYSTEM_PC, visible_forces) == 4);
STATIC_ASSERT(offsetof(REGISTRY_FORCE_SYSTEM_PC, hit_test_gizmos) == 8);
STATIC_ASSERT(offsetof(REGISTRY_FORCE_SYSTEM_PC, capacity) == 0xc);
STATIC_ASSERT(offsetof(REGISTRY_FORCE_SYSTEM_PC, count) == 0xe);
STATIC_ASSERT(offsetof(REGISTRY_FORCE_SYSTEM_PC, anim_pool) == 0x14);

typedef struct registry_turret_system_pc_s {
    REGISTRY_TURRET_RECORD_PC *turrets;
    GAMEANIMOBJPOOL *anim_pool;
    u16 count;
    u16 capacity;
    u8 unknown_0c[4];
} REGISTRY_TURRET_SYSTEM_PC;
STATIC_ASSERT(sizeof(REGISTRY_TURRET_SYSTEM_PC) == 0x10);
STATIC_ASSERT(offsetof(REGISTRY_TURRET_SYSTEM_PC, anim_pool) == 4);
STATIC_ASSERT(offsetof(REGISTRY_TURRET_SYSTEM_PC, count) == 8);
STATIC_ASSERT(offsetof(REGISTRY_TURRET_SYSTEM_PC, capacity) == 0xa);

typedef struct registry_panel_system_pc_s {
    i32 count;
    REGISTRY_PANEL_RECORD_PC *panels;
} REGISTRY_PANEL_SYSTEM_PC;
STATIC_ASSERT(sizeof(REGISTRY_PANEL_SYSTEM_PC) == 8);
STATIC_ASSERT(offsetof(REGISTRY_PANEL_SYSTEM_PC, panels) == 4);
STATIC_ASSERT(sizeof(GIZSPINNER_s) == 0x304);
STATIC_ASSERT(offsetof(GIZSPINNER_s, anim_set) == 0x68);
STATIC_ASSERT(sizeof(GIZBOMBGEN_s) == 0x2c);
STATIC_ASSERT(offsetof(GIZBOMBGEN_s, anim_set) == 0x1c);
STATIC_ASSERT(sizeof(GIZBOMBGENSYS_s) == 0x14);
STATIC_ASSERT(offsetof(GIZBOMBGENSYS_s, capacity) == 6);
STATIC_ASSERT(offsetof(GIZBOMBGENSYS_s, anim_object_pool) == 8);
STATIC_ASSERT(sizeof(GIZTORPMACHINE_s) == 0x2c);
STATIC_ASSERT(sizeof(GIZTORPMACHINESYS_s) == 0xc);
STATIC_ASSERT(offsetof(GIZTORPMACHINESYS_s, machines) == 0);
STATIC_ASSERT(offsetof(GIZTORPMACHINESYS_s, scale) == 8);
STATIC_ASSERT(sizeof(GRAPPLE_DYNAMIC_PC) == 0xf8);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buffer) == 0x104);
STATIC_ASSERT(offsetof(WORLDINFO, buffer_end) == 0x108);
STATIC_ASSERT(offsetof(WORLDINFO, current_level) == 0x128);
STATIC_ASSERT(offsetof(WORLDINFO, game_anim_sys) == 0x2960);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buildit_sys) == 0x46a8);
STATIC_ASSERT(offsetof(WORLDINFO, giz_force_sys) == 0x46ac);
STATIC_ASSERT(offsetof(WORLDINFO, giz_turret_sys) == 0x46b8);
STATIC_ASSERT(offsetof(WORLDINFO, spinners) == 0x46e0);
STATIC_ASSERT(offsetof(WORLDINFO, spinner_count) == 0x46e4);
STATIC_ASSERT(offsetof(WORLDINFO, spinner_anim_pool) == 0x46e8);
STATIC_ASSERT(offsetof(WORLDINFO, giz_panel_sys) == 0x5070);
STATIC_ASSERT(offsetof(WORLDINFO, grapples) == 0x5080);
STATIC_ASSERT(offsetof(WORLDINFO, grapple_count) == 0x5084);
STATIC_ASSERT(offsetof(WORLDINFO, torpedo_machine_sys) == 0x5100);
STATIC_ASSERT(offsetof(WORLDINFO, giz_bombgen_sys) == 0x5114);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_buildit_objects) == 0xe2);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_force_objects) == 0xe4);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_bombgen_objects) == 0xe6);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_levers) == 0xeb);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_grapples) == 0xee);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_buildits) == 0xf0);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_spinners) == 0xf2);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_gizpanels) == 0xfc);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_force) == 0xfe);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_torpmachines) == 0x104);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_spinneranim_objs) == 0x105);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_turrets) == 0x106);
STATIC_ASSERT(offsetof(LEVELDATA_PC, max_bombgens) == 0x107);

void *__cdecl GizBuildIts_ReserveBufferSpace(void *pointer) {
    WORLDINFO *world = (WORLDINFO *)pointer;
    REGISTRY_BUILDIT_SYSTEM_PC *system;
    i32 i;
    system = (REGISTRY_BUILDIT_SYSTEM_PC *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(*system));
    system->capacity = world->current_level->max_buildits;
    system->max_animation_objects = (u16)world->current_level->max_buildit_objects;
    system->buildits = (REGISTRY_BUILDIT_RECORD_PC *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end,
                                                                     system->capacity * sizeof(*system->buildits));
    system->anim_pool =
        GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 200, system->max_animation_objects);
    system->anim_objects = (GAMEANIMOBJ **)GameBufferAlloc(&world->giz_buffer, &world->buffer_end,
                                                           system->max_animation_objects * sizeof(void *));
    for (i = 0; i < system->capacity; ++i)
        system->buildits[i].anim_set =
            GameAnimSet_Create(&world->giz_buffer, &world->buffer_end, system->anim_pool, world->game_anim_sys);
    world->giz_buildit_sys = (GIZBUILDITSYS_s *)system;
    return system;
}

void *__cdecl GizForces_ReserveBufferSpace(void *pointer) {
    WORLDINFO *world = (WORLDINFO *)pointer;
    REGISTRY_FORCE_SYSTEM_PC *system;
    i32 i;
    system = (REGISTRY_FORCE_SYSTEM_PC *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(*system));
    system->capacity = world->current_level->max_force;
    system->forces = (REGISTRY_FORCE_RECORD_PC *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end,
                                                                 system->capacity * sizeof(*system->forces));
    system->hit_test_gizmos =
        (GIZMO_PC **)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, system->capacity * sizeof(void *));
    system->visible_forces = (REGISTRY_FORCE_RECORD_PC **)GameBufferAlloc(&world->giz_buffer, &world->buffer_end,
                                                                          system->capacity * sizeof(void *));
    system->anim_pool = GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 8,
                                                     (u16)world->current_level->max_force_objects);
    for (i = 0; i < system->capacity; ++i)
        system->forces[i].anim_set =
            GameAnimSet_Create(&world->giz_buffer, &world->buffer_end, system->anim_pool, world->game_anim_sys);
    world->giz_force_sys = (GIZFORCESYS_s *)system;
    return system;
}

void *__cdecl GizSpinner_ReserveBufferSpace(void *pointer) {
    WORLDINFO *world = (WORLDINFO *)pointer;
    i32 i;
    world->spinners = NULL;
    world->spinner_count = 0;
    if (world->current_level->max_spinners == 0)
        return NULL;
    world->spinner_anim_pool = GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 0,
                                                            world->current_level->max_spinneranim_objs);
    world->giz_buffer.addr = (world->giz_buffer.addr + 15) & ~15u;
    world->spinners = (GIZSPINNER_s *)world->giz_buffer.void_ptr;
    world->giz_buffer.addr += world->current_level->max_spinners * sizeof(*world->spinners);
    memset(world->spinners, 0, world->current_level->max_spinners * sizeof(*world->spinners));
    for (i = 0; i < world->current_level->max_spinners; ++i)
        world->spinners[i].anim_set =
            GameAnimSet_Create(&world->giz_buffer, &world->buffer_end, world->spinner_anim_pool, world->game_anim_sys);
    world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
    return world->spinners;
}

void *__cdecl GizTurrets_ReserveBufferSpace(void *pointer) {
    WORLDINFO *world = (WORLDINFO *)pointer;
    REGISTRY_TURRET_SYSTEM_PC *system;
    i32 i;
    system = (REGISTRY_TURRET_SYSTEM_PC *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(*system));
    system->capacity = world->current_level->max_turrets;
    system->turrets = (REGISTRY_TURRET_RECORD_PC *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end,
                                                                   system->capacity * sizeof(*system->turrets));
    system->anim_pool = GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 4, system->capacity * 2);
    for (i = 0; i < system->capacity; ++i)
        system->turrets[i].anim_set =
            GameAnimSet_Create(&world->giz_buffer, &world->buffer_end, system->anim_pool, world->game_anim_sys);
    world->giz_turret_sys = (GIZTURRETSYS_s *)system;
    return system;
}

void *__cdecl GizBombGens_ReserveBufferSpace(void *pointer) {
    WORLDINFO *world = (WORLDINFO *)pointer;
    GIZBOMBGENSYS_s *system;
    i32 i;
    system = (GIZBOMBGENSYS_s *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end, sizeof(*system));
    system->capacity = world->current_level->max_bombgens;
    system->bomb_generators = (GIZBOMBGEN_s *)GameBufferAlloc(&world->giz_buffer, &world->buffer_end,
                                                              system->capacity * sizeof(*system->bomb_generators));
    system->anim_object_pool = GameAnimSet_CreateObjectPool(&world->giz_buffer, &world->buffer_end, 0,
                                                            (u16)world->current_level->max_bombgen_objects);
    for (i = 0; i < system->capacity; ++i)
        system->bomb_generators[i].anim_set = GameAnimSet_Create(
            &world->giz_buffer, &world->buffer_end, (GAMEANIMOBJPOOL *)system->anim_object_pool, world->game_anim_sys);
    world->giz_bombgen_sys = system;
    return system;
}

void *__cdecl GizPanels_ReserveBufferSpace(void *pointer) {
    WORLDINFO *world = (WORLDINFO *)pointer;
    REGISTRY_PANEL_SYSTEM_PC *system;
    world->giz_panel_sys = NULL;
    if (world->current_level->max_levers > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 15) & ~15u;
        world->giz_panel_sys = (GIZPANELSYS_s *)world->giz_buffer.void_ptr;
        world->giz_buffer.addr += sizeof(REGISTRY_PANEL_SYSTEM_PC);
        system = (REGISTRY_PANEL_SYSTEM_PC *)world->giz_panel_sys;
        system->count = 0;
        system->panels = NULL;
        world->giz_buffer.addr = (world->giz_buffer.addr + 15) & ~15u;
        system = (REGISTRY_PANEL_SYSTEM_PC *)world->giz_panel_sys;
        system->panels = (REGISTRY_PANEL_RECORD_PC *)world->giz_buffer.void_ptr;
        world->giz_buffer.addr += world->current_level->max_gizpanels * sizeof(*system->panels);
        memset(system->panels, 0, world->current_level->max_gizpanels * sizeof(*system->panels));
    }
    return world->giz_panel_sys;
}

void *__cdecl GizTorps_ReserveBufferSpace(void *pointer) {
    WORLDINFO *world = (WORLDINFO *)pointer;
    GIZTORPMACHINESYS_s *system;
    world->torpedo_machine_sys = NULL;
    if (world->current_level->max_torpmachines > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        world->torpedo_machine_sys = (TORPEDO_MACHINE_SYS_PC *)world->giz_buffer.void_ptr;
        world->giz_buffer.addr += sizeof(GIZTORPMACHINESYS_s);
        system = (GIZTORPMACHINESYS_s *)world->torpedo_machine_sys;
        system->machines = NULL;
        system->count = 0;
        system->scale = 0.0f;
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        system = (GIZTORPMACHINESYS_s *)world->torpedo_machine_sys;
        system->machines = (GIZTORPMACHINE_s *)world->giz_buffer.void_ptr;
        world->giz_buffer.addr += world->current_level->max_torpmachines * sizeof(*system->machines);
        memset(system->machines, 0, world->current_level->max_torpmachines * sizeof(*system->machines));
        ((GIZTORPMACHINESYS_s *)world->torpedo_machine_sys)->scale = 1.0f;
    }
    return world->torpedo_machine_sys;
}

void *__cdecl Grapples_ReserveBufferSpace(void *pointer) {
    WORLDINFO *world = (WORLDINFO *)pointer;
    GRAPPLE_DYNAMIC_PC *grapples = NULL;
    world->grapples = NULL;
    world->grapple_count = 0;
    if (world->current_level->max_grapples > 0) {
        world->giz_buffer.addr = (world->giz_buffer.addr + 3) & ~3u;
        grapples = (GRAPPLE_DYNAMIC_PC *)world->giz_buffer.void_ptr;
        world->grapples = grapples;
        world->giz_buffer.addr += world->current_level->max_grapples * sizeof(*grapples);
    }
    return grapples;
}

void __cdecl GizBuildIts_PostLoad(void *world, void *pointer) {
    REGISTRY_BUILDIT_SYSTEM_PC *system = (REGISTRY_BUILDIT_SYSTEM_PC *)pointer;
    REGISTRY_BUILDIT_RECORD_PC *buildit;
    i32 i;
    if (system == NULL)
        return;
    buildit = system->buildits;
    for (i = 0; i < system->count; ++i, ++buildit) {
        if (buildit->flags & 0x80) {
            i32 type_id = GizmoBlowupGetTypeFromNameTableId((WORLDINFO *)world, buildit->blowup_type);
            buildit->flags &= ~0x80;
            buildit->blowup_type = (i16)type_id;
        }
    }
}

void __cdecl GizForces_PostLoad(void *world, void *pointer) {
    REGISTRY_FORCE_SYSTEM_PC *system = (REGISTRY_FORCE_SYSTEM_PC *)pointer;
    REGISTRY_FORCE_RECORD_PC *force;
    i32 i;
    if (system == NULL)
        return;
    force = system->forces;
    for (i = 0; i < system->count; ++i, ++force) {
        if (force->runtime_flags & 0x400) {
            i32 type_id = GizmoBlowupGetTypeFromNameTableId((WORLDINFO *)world, force->blowup_type);
            force->runtime_flags &= ~0x400u;
            force->blowup_type = (i16)type_id;
        }
    }
}

void __cdecl GizmoPickups_PostLoad(void *world, void *system) {
    (void)world;
    (void)system;
}

void __cdecl GizTurrets_PostLoad(void *world, void *pointer) {
    REGISTRY_TURRET_SYSTEM_PC *system = (REGISTRY_TURRET_SYSTEM_PC *)pointer;
    REGISTRY_TURRET_RECORD_PC *turret;
    i32 i;
    if (system == NULL)
        return;
    turret = system->turrets;
    for (i = 0; i < system->count; ++i, ++turret) {
        if (turret->flags & 0x100) {
            i32 type_id = GizmoBlowupGetTypeFromNameTableId((WORLDINFO *)world, turret->blowup_type);
            turret->flags &= ~0x100;
            turret->blowup_type = (i16)type_id;
        }
    }
}
