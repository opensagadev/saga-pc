#ifndef SAGA_EDITOR_AIEDITOR_STATE_H_PART
#define SAGA_EDITOR_AIEDITOR_STATE_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "editor/antinode_editor_private.h"
#include "editor/edpath.h"
#include "editor/edpath_types.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/ai/core/aisysall_types.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "editor/antinode_editor_private.h"
#include "editor/edpath.h"
#include "editor/edpath_types.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/ai/core/aisysall_types.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "editor/antinode_editor_private.h"
#include "editor/edpath.h"
#include "editor/edpath_types.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/ai/core/aisysall_types.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/nuvec.h"
#endif

/* 3 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_EDITOR_AIEDITOR_STATE_H_PART == 1 && !defined(SAGA_EDITOR_AIEDITOR_STATE_H_STOP)
typedef struct AIEDITOR_RENDER_STATE AIEDITOR_RENDER_STATE;
typedef struct EDLOCATORSET_s EDLOCATORSET_s;
typedef struct EDLOCATOR_s EDLOCATOR_s;

struct EDLOCATORSET_s {
    NULISTLNK link;
    char name[16];
    struct EDLOCATOR_s *locators[64];
};

STATIC_ASSERT(sizeof(struct EDLOCATORSET_s) == 280);
#undef SAGA_EDITOR_AIEDITOR_STATE_H_PART
#define SAGA_EDITOR_AIEDITOR_STATE_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_EDITOR_AIEDITOR_STATE_H_STOP
#endif
#endif

#if SAGA_EDITOR_AIEDITOR_STATE_H_PART == 2 && !defined(SAGA_EDITOR_AIEDITOR_STATE_H_STOP)

struct EDLOCATOR_s {
    NULISTLNK link;
    char name[16];
    NUVEC position;
    i32 direction;
    struct EDAIPATHCHECK_s path_check;
    u16 runtime_index;
    u8 drawn;
    u8 unknown_47;
};

STATIC_ASSERT(sizeof(struct EDLOCATOR_s) == 72);
#undef SAGA_EDITOR_AIEDITOR_STATE_H_PART
#define SAGA_EDITOR_AIEDITOR_STATE_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_EDITOR_AIEDITOR_STATE_H_STOP
#endif
#endif

#if SAGA_EDITOR_AIEDITOR_STATE_H_PART == 3 && !defined(SAGA_EDITOR_AIEDITOR_STATE_H_STOP)

struct AIEDITOR_RENDER_STATE { /* Each mode: fixed record pool, free list, live list, then its selection pointers. */
    u8 unknown_00[4];
    struct eduimenu_s *main_menu;
    void *enter_context;
    void *enter_owner;
    NUVEC cursor_position;
    NUVEC cursor_movement;
    NUVEC camera_position;
    i32 camera_pitch;
    i32 camera_yaw;
    struct nuhspecial_s cursor_platform;
    struct EDAIPATHCHECK_s cursor_check;
    struct AISYS_PC *ai_system;
    struct EDAIPATH_s path_storage[32];
    struct EDAIPATH_s *current_path;
    NULISTHDR free_paths;
    NULISTHDR paths;
    struct EDAIPATHNODE_s node_storage[1024];
    NULISTHDR free_path_nodes;
    struct EDAISHAREDPATHNODE_s shared_node_storage[64];
    NULISTHDR free_shared_nodes;

    union {
        NULISTHDR path_walls;
        NULISTHDR shared_path_nodes;
    };
    struct AIPATHSYS_PC *cached_path_system;
    struct AIPATH_PC *runtime_path;
    struct EDAICREATURE_s creature_pool[128];
    NULISTHDR free_creatures;
    NULISTHDR creatures;
    struct EDAICREATURE_s *nearest_creature;
    struct EDAICREATURE_s *selected_creature;
    i32 rotation_repeat;
    struct EDAIAREA_s area_pool[64];
    NULISTHDR free_areas;
    NULISTHDR areas;
    struct EDAIAREA_s *selected_area;
    struct EDAIAREA_s *hovered_area;
    struct EDLOCATOR_s locator_pool[256];
    NULISTHDR free_locators;
    NULISTHDR locators;
    struct EDLOCATOR_s *current_locator;
    struct EDLOCATOR_s *nearest_locator;
    struct EDLOCATORSET_s locator_set_pool[64];
    char pending_locator_name[16];
    NULISTHDR free_locator_sets;
    NULISTHDR locator_sets;
    struct EDLOCATORSET_s *current_locator_set;
    struct EDANTINODE_s antinode_pool[128];
    NULISTHDR free_antinodes;
    NULISTHDR antinodes;
    struct EDANTINODE_s *selected_antinode;
    struct EDANTINODE_s *nearest_antinode;
    f32 preview_width;
    u32 flags;
    char *level_directory; /* aieditor_Save: "%sLevels\\<level_directory>\\<level_subdirectory>\\<level_file>.ai2" */
    char *level_file;
    char *level_subdirectory;
    u8 save_scratch[524288];
    char warning[256];
    u32 pad_buttons;
    u32 pad_pressed;
};

STATIC_ASSERT(sizeof(struct AIEDITOR_RENDER_STATE) == 801088);
STATIC_ASSERT(offsetof(struct AIEDITOR_RENDER_STATE, cursor_check) == 0x48);
STATIC_ASSERT(offsetof(struct AIEDITOR_RENDER_STATE, free_creatures) == 0x3719c);
STATIC_ASSERT(offsetof(struct AIEDITOR_RENDER_STATE, creatures) == 0x371a4);
STATIC_ASSERT(offsetof(struct AIEDITOR_RENDER_STATE, rotation_repeat) == 0x371b4);
STATIC_ASSERT(offsetof(struct AIEDITOR_RENDER_STATE, areas) == 0x383c0);
STATIC_ASSERT(offsetof(struct AIEDITOR_RENDER_STATE, hovered_area) == 0x383cc);
STATIC_ASSERT(offsetof(struct AIEDITOR_RENDER_STATE, locators) == 0x3cbd8);
STATIC_ASSERT(offsetof(struct AIEDITOR_RENDER_STATE, locator_sets) == 0x41200);
STATIC_ASSERT(offsetof(struct AIEDITOR_RENDER_STATE, antinodes) == 0x43814);
STATIC_ASSERT(offsetof(struct AIEDITOR_RENDER_STATE, preview_width) == 0x43824);
STATIC_ASSERT(offsetof(struct AIEDITOR_RENDER_STATE, flags) == 0x43828);
STATIC_ASSERT(offsetof(struct AIEDITOR_RENDER_STATE, level_subdirectory) == 0x43834);
STATIC_ASSERT(offsetof(struct AIEDITOR_RENDER_STATE, pad_pressed) == 0xc393c);
#ifdef __cplusplus
extern "C" {
#endif
    extern struct AIEDITOR_RENDER_STATE *aieditor;
#ifdef __cplusplus
}
#endif
#undef SAGA_EDITOR_AIEDITOR_STATE_H_PART
#define SAGA_EDITOR_AIEDITOR_STATE_H_PART 4
#ifdef SAGA_SHIM
#define SAGA_EDITOR_AIEDITOR_STATE_H_STOP
#endif
#endif

#undef SAGA_EDITOR_AIEDITOR_STATE_H_STOP
