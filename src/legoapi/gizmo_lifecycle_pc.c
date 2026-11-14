#include "saga.h"

/* These allocation records use the native PC strides, rather than the larger imported views. */
typedef struct registry_buildit_record_pc_s {
    u8 unknown_00[0x10];      // 0x00
    GAMEANIMSET *anim_set;    // 0x10
    u8 unknown_14[0x64];      // 0x14
    i16 blowup_type;          // 0x78
    u16 flags;                // 0x7a
} REGISTRY_BUILDIT_RECORD_PC; // size 0x7c

typedef struct registry_force_record_pc_s {
    u8 unknown_00[0x28];    // 0x00
    GAMEANIMSET *anim_set;  // 0x28
    u8 unknown_2c[0x5c];    // 0x2c
    i16 blowup_type;        // 0x88
    u8 unknown_8a[0x16];    // 0x8a
    u32 runtime_flags;      // 0xa0
} REGISTRY_FORCE_RECORD_PC; // size 0xa4

typedef struct registry_panel_record_pc_s {
    u8 storage[0x90];       // 0x00
} REGISTRY_PANEL_RECORD_PC; // size 0x90

typedef struct registry_buildit_system_pc_s {
    REGISTRY_BUILDIT_RECORD_PC *buildits; // 0x00
    u16 count;                            // 0x04
    u16 capacity;                         // 0x06
    u16 max_animation_objects;            // 0x08
    u16 padding;                          // 0x0a
    GAMEANIMOBJPOOL *anim_pool;           // 0x0c
    GAMEANIMOBJ **anim_objects;           // 0x10
} REGISTRY_BUILDIT_SYSTEM_PC;             // size 0x14

typedef struct registry_force_system_pc_s {
    REGISTRY_FORCE_RECORD_PC *forces;          // 0x000
    REGISTRY_FORCE_RECORD_PC **visible_forces; // 0x004
    GIZMO_PC **hit_test_gizmos;                // 0x008
    u16 capacity;                              // 0x00c
    u16 count;                                 // 0x00e
    u16 visible_force_count;                   // 0x010
    u16 hit_test_gizmo_count;                  // 0x012
    GAMEANIMOBJPOOL *anim_pool;                // 0x014
    GIZFORCEGROUP_s groups[8];                 // 0x018
} REGISTRY_FORCE_SYSTEM_PC;                    // size 0x158

typedef struct registry_panel_system_pc_s {
    i32 count;                        // 0x0
    REGISTRY_PANEL_RECORD_PC *panels; // 0x4
} REGISTRY_PANEL_SYSTEM_PC;           // size 0x8

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

void GizmoPickups_PostLoad(void *world, void *system) {
    (void)world;
    (void)system;
}
