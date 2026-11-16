#pragma once

#include "nu2api/nucore/common.h"

#include "editor/antinode_editor_private.h"
#include "editor/edpath.h"
#include "editor/edpath_types.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/ai/core/aisysall_types.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/nulist.h"
#include "nu2api/numath/nuvec.h"

typedef struct AIEDITOR_RENDER_STATE AIEDITOR_RENDER_STATE;
typedef struct EDLOCATORSET_s EDLOCATORSET_s;
typedef struct EDLOCATOR_s EDLOCATOR_s;

struct EDLOCATORSET_s {               // size 0x118
    NULISTLNK link;                   // 0x000
    char name[16];                    // 0x008
    struct EDLOCATOR_s *locators[64]; // 0x018
};

struct EDLOCATOR_s {                   // size 0x48
    NULISTLNK link;                    // 0x00
    char name[16];                     // 0x08
    NUVEC position;                    // 0x18
    i32 direction;                     // 0x24
    struct EDAIPATHCHECK_s path_check; // 0x28
    u16 runtime_index;                 // 0x44
    u8 drawn;                          // 0x46
    u8 unknown_47;                     // 0x47
};

struct AIEDITOR_RENDER_STATE { // size 0xc3940
    /* Each mode: fixed record pool, free list, live list, then its selection pointers. */
    u8 unknown_00[4];                                    // 0x00000
    struct eduimenu_s *main_menu;                        // 0x00004
    void *enter_context;                                 // 0x00008
    void *enter_owner;                                   // 0x0000c
    NUVEC cursor_position;                               // 0x00010
    NUVEC cursor_movement;                               // 0x0001c
    NUVEC camera_position;                               // 0x00028
    i32 camera_pitch;                                    // 0x00034
    i32 camera_yaw;                                      // 0x00038
    struct nuhspecial_s cursor_platform;                 // 0x0003c
    struct EDAIPATHCHECK_s cursor_check;                 // 0x00048
    struct AISYS *ai_system;                             // 0x00064
    struct EDAIPATH_s path_storage[32];                  // 0x00068
    struct EDAIPATH_s *current_path;                     // 0x04868
    NULISTHDR free_paths;                                // 0x0486c
    NULISTHDR paths;                                     // 0x04874
    struct EDAIPATHNODE_s node_storage[1024];            // 0x0487c
    NULISTHDR free_path_nodes;                           // 0x3187c
    struct EDAISHAREDPATHNODE_s shared_node_storage[64]; // 0x31884
    NULISTHDR free_shared_nodes;                         // 0x31b84

    union { // size 0x8
        NULISTHDR path_walls;
        NULISTHDR shared_path_nodes;
    }; // 0x31b8c
    struct AIPATHSYS *cached_path_system;       // 0x31b94
    struct AIPATH *runtime_path;                // 0x31b98
    struct EDAICREATURE_s creature_pool[128];   // 0x31b9c
    NULISTHDR free_creatures;                   // 0x3719c
    NULISTHDR creatures;                        // 0x371a4
    struct EDAICREATURE_s *nearest_creature;    // 0x371ac
    struct EDAICREATURE_s *selected_creature;   // 0x371b0
    i32 rotation_repeat;                        // 0x371b4
    struct EDAIAREA_s area_pool[64];            // 0x371b8
    NULISTHDR free_areas;                       // 0x383b8
    NULISTHDR areas;                            // 0x383c0
    struct EDAIAREA_s *selected_area;           // 0x383c8
    struct EDAIAREA_s *hovered_area;            // 0x383cc
    struct EDLOCATOR_s locator_pool[256];       // 0x383d0
    NULISTHDR free_locators;                    // 0x3cbd0
    NULISTHDR locators;                         // 0x3cbd8
    struct EDLOCATOR_s *current_locator;        // 0x3cbe0
    struct EDLOCATOR_s *nearest_locator;        // 0x3cbe4
    struct EDLOCATORSET_s locator_set_pool[64]; // 0x3cbe8
    char pending_locator_name[16];              // 0x411e8
    NULISTHDR free_locator_sets;                // 0x411f8
    NULISTHDR locator_sets;                     // 0x41200
    struct EDLOCATORSET_s *current_locator_set; // 0x41208
    struct EDANTINODE_s antinode_pool[128];     // 0x4120c
    NULISTHDR free_antinodes;                   // 0x4380c
    NULISTHDR antinodes;                        // 0x43814
    struct EDANTINODE_s *selected_antinode;     // 0x4381c
    struct EDANTINODE_s *nearest_antinode;      // 0x43820
    f32 preview_width;                          // 0x43824
    u32 flags;                                  // 0x43828
    /* aieditor_Save: "%sLevels\\<level_directory>\\<level_subdirectory>\\<level_file>.ai2" */
    char *level_directory;    // 0x4382c
    char *level_file;         // 0x43830
    char *level_subdirectory; // 0x43834
    u8 save_scratch[524288];  // 0x43838
    char warning[256];        // 0xc3838
    u32 buttons;              // 0xc3938
    u32 buttons_pressed;      // 0xc393c
};

#ifdef __cplusplus
extern "C" {
#endif
    extern struct AIEDITOR_RENDER_STATE *aieditor;
#ifdef __cplusplus
}
#endif

#define AIEDITOR_REGISTER(id, mode_name, enter_fn, process_fn, draw_fn)                                                                    \
    do {                                                                                                                                   \
        AI_EDITOR_MODE_ENTRY *entry_;                                                                                                      \
        i32 index_ = -1;                                                                                                                   \
        if (aieditorsettings.modes.count < 8) {                                                                                            \
            index_ = aieditorsettings.modes.count++;                                                                                       \
            entry_ = &aieditorsettings.modes.entries[index_];                                                                              \
            NuStrCpy(entry_->name, mode_name);                                                                                             \
            entry_->enter = enter_fn;                                                                                                      \
            entry_->process = process_fn;                                                                                                  \
            entry_->draw = draw_fn;                                                                                                        \
            entry_->leave = NULL;                                                                                                          \
        } else {                                                                                                                           \
            AI_SCRIPT_DIAGNOSTIC_BARE(AIScriptMissingStateDiagnostic);                                                                     \
        }                                                                                                                                  \
        (id) = index_;                                                                                                                     \
    } while (0)
