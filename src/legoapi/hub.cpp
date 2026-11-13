#include "saga.h"

/* Retail unit 0x474830-0x481a6f: definitions in retail address order. */
#include <stdio.h>
/* Existing compiled native owners; declarations only, no replacement bodies/data. */
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif

typedef struct hub_minikit_runtime_pc_s {
    NUVEC position;
    f32 scale, scalar_10;
    i32 phase_x, rate_x, phase_y, rate_y, phase_z, rate_z, phase_rotation, rate_rotation;
    u16 rotation, rotation_velocity, tilt, target_rotation;
    u8 displayed_piece_count, target_piece_count, unknown_3e[2];
    NUVEC collision_center, bounds_min, bounds_max;
    f32 radius, height_ratio, offset_x, offset_z, velocity_x, velocity_z;
} HUB_MINIKIT_RUNTIME_PC;
STATIC_ASSERT(sizeof(HUB_MINIKIT_RUNTIME_PC) == 0x7c);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, position) == 0);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, scale) == 0xc);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, scalar_10) == 0x10);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, phase_x) == 0x14);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, rate_x) == 0x18);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, phase_y) == 0x1c);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, rate_y) == 0x20);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, phase_z) == 0x24);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, rate_z) == 0x28);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, phase_rotation) == 0x2c);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, rate_rotation) == 0x30);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, rotation) == 0x34);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, rotation_velocity) == 0x36);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, tilt) == 0x38);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, target_rotation) == 0x3a);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, displayed_piece_count) == 0x3c);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, target_piece_count) == 0x3d);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, offset_x) == 0x6c);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, offset_z) == 0x70);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, velocity_x) == 0x74);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, velocity_z) == 0x78);

typedef struct hub_world_reader_pc_s {
    u8 unknown_0000[0x2984];
    struct hub_minikit_pieces_pc_s **pieces;
    u8 unknown_2988[0x158];
    HUB_MINIKIT_RUNTIME_PC *minikits;
} HUB_WORLD_READER_PC;
STATIC_ASSERT(offsetof(HUB_WORLD_READER_PC, pieces) == 0x2984);
STATIC_ASSERT(offsetof(HUB_WORLD_READER_PC, minikits) == 0x2ae0);

typedef struct hub_area_info_pc_s {
    const char *area_name, *door_name, *lock_name, *bonus_name, *second_bonus_name;
    f32 panel_offset, panel_scale;
    i32 flags;
    AREA_DATA_PC *area;
    void *door;
    NUSPECIALHANDLE lock;
    void *bonus_gizmo, *second_bonus_gizmo;
} HUB_AREA_INFO_PC;
STATIC_ASSERT(sizeof(HUB_AREA_INFO_PC) == 0x3c);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, area_name) == 0);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, area) == 0x20);

STATIC_ASSERT(offsetof(GAMEOBJECT, special_move_look_position_pc) == 0x1098);
extern "C" void __cdecl LostTempleC_ResetPc(WORLDINFO *);
extern "C" void __cdecl LostTempleC_UpdatePc(WORLDINFO *);
/* Original level.cpp FixUpLevels family; PC-specific startup gates verified. */
STATIC_ASSERT(offsetof(LEVELDATA_PC, callbacks) == 0x68);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_index) == 0x94);
STATIC_ASSERT(offsetof(LEVELDATA_PC, music_tracks) == 0x118);
STATIC_ASSERT(sizeof(LEVELDATA_PC) == 0x130);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, sock_index) == 1);
STATIC_ASSERT(offsetof(SOCKPOSITION_PC, segment) == 2);
STATIC_ASSERT(sizeof(SOCKPOSITION_PC) == 0x38);
STATIC_ASSERT(offsetof(GIZOBSTACLE, anim_set) == 0x34);
STATIC_ASSERT(offsetof(GIZOBSTACLE, evaluated_position) == 0x28);
STATIC_ASSERT(offsetof(GIZOBSTACLE, evaluated_radius) == 0x58);
STATIC_ASSERT(offsetof(GAMEANIMSET, flags) == 0xa);
STATIC_ASSERT(offsetof(GAMEOBJECT, sock_position) == 0x670);
STATIC_ASSERT(offsetof(GAMEOBJECT, collision_position) == 0x80);
STATIC_ASSERT(offsetof(WORLDINFO, sock_sys) == 0x2988);
STATIC_ASSERT(offsetof(WORLDINFO, giz_obstacle_sys) == 0x46a4);
STATIC_ASSERT(offsetof(LEVELDATA_PC, callbacks) == 0x68);
typedef struct ARCADE_COIN_PACKET_PC {
    u32 coins;
    f32 scale;
} ARCADE_COIN_PACKET_PC;
typedef struct hub_minikit_piece_pc_s {
    NUSPECIALHANDLE special;
    NUMTX matrix;
    u8 unknown_4c[0x1a];
    u8 enabled;
    u8 direction_index;
} HUB_MINIKIT_PIECE_PC;
STATIC_ASSERT(sizeof(HUB_MINIKIT_PIECE_PC) == 0x68);
STATIC_ASSERT(offsetof(HUB_MINIKIT_PIECE_PC, special) == 0);
STATIC_ASSERT(offsetof(HUB_MINIKIT_PIECE_PC, matrix) == 0xc);
STATIC_ASSERT(offsetof(HUB_MINIKIT_PIECE_PC, enabled) == 0x66);
typedef struct hub_minikit_pieces_pc_s {
    NUGSCN *scene;
    HUB_MINIKIT_PIECE_PC *pieces;
    u8 piece_count;
    i8 area_index;
    i16 model_id;
    NUSPECIALHANDLE base;
} HUB_MINIKIT_PIECES_PC;
STATIC_ASSERT(sizeof(HUB_MINIKIT_PIECES_PC) == 0x18);
STATIC_ASSERT(offsetof(HUB_MINIKIT_PIECES_PC, pieces) == 4);
STATIC_ASSERT(offsetof(HUB_MINIKIT_PIECES_PC, piece_count) == 8);
STATIC_ASSERT(offsetof(HUB_MINIKIT_PIECES_PC, base) == 0xc);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, panel_offset) == 0x14);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, panel_scale) == 0x18);
typedef struct hub_pad_reader_pc_s {
    void *pad;
    u32 held;
    u8 unknown_08[0x58];
} HUB_PAD_READER_PC;
STATIC_ASSERT(sizeof(HUB_PAD_READER_PC) == 0x60);
STATIC_ASSERT(offsetof(HUB_PAD_READER_PC, held) == 4);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, collision_center) == 0x40);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, bounds_min) == 0x4c);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, bounds_max) == 0x58);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, radius) == 0x64);
STATIC_ASSERT(offsetof(HUB_MINIKIT_RUNTIME_PC, height_ratio) == 0x68);
/* Actual PC Draw3D omits the Android sign/episode scene branch. */
typedef struct hub_game_stats_reader_pc_s {
    u8 unknown_0000[0x7c18];
    u16 completion;
    u8 unknown_7c1a[6];
    f32 elapsed;
} HUB_GAME_STATS_READER_PC;
STATIC_ASSERT(offsetof(HUB_GAME_STATS_READER_PC, completion) == 0x7c18);
STATIC_ASSERT(offsetof(HUB_GAME_STATS_READER_PC, elapsed) == 0x7c20);
typedef struct hub_episode_info_pc_s {
    i16 episode;
    u16 flags;
    const char *door_name, *lock_on_name;
    i32 force_open;
    u32 unknown_10;
    EPISODEDATA_PC *data;
    void *door;
    NUSPECIALHANDLE lock_on;
} HUB_EPISODE_INFO_PC;
STATIC_ASSERT(sizeof(HUB_EPISODE_INFO_PC) == 0x28);
STATIC_ASSERT(offsetof(HUB_EPISODE_INFO_PC, episode) == 0);
STATIC_ASSERT(offsetof(HUB_EPISODE_INFO_PC, door_name) == 4);
STATIC_ASSERT(offsetof(HUB_EPISODE_INFO_PC, lock_on_name) == 8);
STATIC_ASSERT(offsetof(HUB_EPISODE_INFO_PC, force_open) == 0xc);
STATIC_ASSERT(offsetof(HUB_EPISODE_INFO_PC, unknown_10) == 0x10);
STATIC_ASSERT(offsetof(HUB_EPISODE_INFO_PC, data) == 0x14);
STATIC_ASSERT(offsetof(HUB_EPISODE_INFO_PC, door) == 0x18);
STATIC_ASSERT(offsetof(HUB_EPISODE_INFO_PC, lock_on) == 0x1c);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, door_name) == 4);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, lock_name) == 8);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, bonus_name) == 0xc);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, second_bonus_name) == 0x10);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, door) == 0x24);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, lock) == 0x28);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, bonus_gizmo) == 0x34);
STATIC_ASSERT(offsetof(HUB_AREA_INFO_PC, second_bonus_gizmo) == 0x38);
typedef struct hub_init_world_reader_pc_s {
    u8 unknown_00[0x13c];
    NUGSCN *scene;
    u8 unknown_140[0x2988];
    void *gizmo_system;
} HUB_INIT_WORLD_READER_PC;
STATIC_ASSERT(offsetof(HUB_INIT_WORLD_READER_PC, scene) == 0x13c);
STATIC_ASSERT(offsetof(HUB_INIT_WORLD_READER_PC, gizmo_system) == 0x2ac8);
typedef struct hub_door_object_reader_pc_s {
    u8 unknown_00[0x9c];
    u32 flags;
} HUB_DOOR_OBJECT_READER_PC;
typedef struct hub_door_reader_pc_s {
    HUB_DOOR_OBJECT_READER_PC *object;
} HUB_DOOR_READER_PC;
STATIC_ASSERT(offsetof(HUB_DOOR_OBJECT_READER_PC, flags) == 0x9c);
STATIC_ASSERT(offsetof(HUB_DOOR_READER_PC, object) == 0);
STATIC_ASSERT(offsetof(AREA_DATA_PC, index) == 0x7c);
STATIC_ASSERT(offsetof(AREA_DATA_PC, episode_index) == 0x86);
// Add only to the genuine Hub owner, after its existing private table definition.
struct GIZBUILDIT_PC;
STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);
/* 424750/92. Can share the fuller Customiser canonical type once integrated. */
typedef struct hub_customiser_menu_reader_pc_s {
    u8 unknown_00[0x6c];
    i16 character_ids[2];
    u8 unknown_70[0x142];
    i16 first_animation;
    u8 unknown_1b4[0x46];
    i16 second_animation;
} HUB_CUSTOMISER_MENU_READER_PC;
STATIC_ASSERT(offsetof(HUB_CUSTOMISER_MENU_READER_PC, first_animation) == 0x1b2);
STATIC_ASSERT(offsetof(HUB_CUSTOMISER_MENU_READER_PC, second_animation) == 0x1fa);
/* 58eaa0/465. WORLDINFO currently carries Android buildit pointee types;
 * cast the pointee to the asserted PC array owner, never index Android84.
 */
typedef struct hub_buildit_sys_pc_s {
    GIZBUILDIT_PC *buildits;
    u16 count;
} HUB_BUILDIT_SYS_PC;
STATIC_ASSERT(offsetof(HUB_BUILDIT_SYS_PC, count) == 4);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buildit_sys) == 0x46a8);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, object_data) == 0x24);
STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, draw_mtx) == 0x80);
STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, start_mtx) == 0);
/* PC50ed50 282 bytes, native three stack parameters: action, player0, player1.
 * Last two are char* values tested for nonzero rather than string contents. */
extern "C" char *TextInputActionLabelPc(i32 device, i32 action, i32, i32);
extern "C" char HubButtonStringBufferPc[]; /*8016a8 native scratch owner size TBD, initial"???"*/
typedef struct hub_freeplay_pad_reader_pc_s {
    u8 unknown_00[0x78];
    u32 digital_buttons_pressed;
} HUB_FREEPLAY_PAD_READER_PC;
STATIC_ASSERT(offsetof(HUB_FREEPLAY_PAD_READER_PC, digital_buttons_pressed) == 0x78);
extern "C" {
    STATIC_ASSERT(offsetof(GAMEOBJECT, portal_flags) == 0xeec);
    STATIC_ASSERT(offsetof(GAMEOBJECT, takeover_source) == 0xcd0);
    STATIC_ASSERT(offsetof(GAMEOBJECT, field_107e) == 0x107e);
    STATIC_ASSERT(offsetof(GAMEOBJECT, current_hp) == 0x107f);
    STATIC_ASSERT(offsetof(GAMEOBJECT, dead) == 0x28b);
    STATIC_ASSERT(offsetof(GAMEOBJECT, field_1010) == 0x1010);
}

extern "C" {
    LEVELFIXUP_PC LevelFixupStartupPc[2] = {{"losttemple_c", &UpdateSockets_LDATA, NULL, NULL,
                                             (void *)LostTempleC_ResetPc, (void *)LostTempleC_UpdatePc, NULL, NULL,
                                             NULL},
                                            {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}};
}

STATIC_ASSERT(offsetof(ARCADE_COIN_PACKET_PC, coins) == 0);
STATIC_ASSERT(offsetof(ARCADE_COIN_PACKET_PC, scale) == 4);

/* Research-only declarations of genuine original Hub state; native initializer
 * and canonical/private ownership verification pending sole writer. */

extern "C" void __cdecl Hub_LockUnlockDoors(WORLDINFO *world) {
    (void)world;
}

/* Additional source-private viewer state/native menu-reader view, genuine
 * original backing/ownership gate pending; no placeholder storage emitted. */

#include <string.h>

/* Application fragment for the existing hub.cpp owner. Native 47cdd0/3846.
 * Private draft: declarations/data ownership listed separately; no activation.
 * Existing misleading Hub* names are deliberately retained by mapped address.
 */

/* PC511560 106 bytes. Cdecl all stack, original 13 args + fixed suffix0,0,128. Retail lives in the menu
   unit and is never inlined into hub callers. */
extern "C" __declspec(noinline) void SmartTextPc(char *text, f32 x, f32 y, f32 z, f32 sx, f32 sy, f32 sz, i32 align,
                                                 i32 red, i32 green, i32 blue, f32 max_width, i32 center) {
    SmartTextEx(text, x, y, z, sx, sy, sz, align, red, green, blue, max_width, center, NULL, 0, 128);
}
extern "C" char *TextButtonStringPc(i32 action, const char *player0, const char *player1) {
    char first[32] = "", second[32] = "";
    const i32 device0 = NuPadGetPort(0);
    const i32 device1 = NuPadGetPort(1);
    if (player0 != NULL && device0 >= 0)
        NuStrCpy(first, TextInputActionLabelPc(device0, action, 0, 0));
    if (player1 != NULL && device1 >= 0)
        NuStrCpy(second, TextInputActionLabelPc(device1, action, 0, 0));
    if (first[0] != 0 && second[0] != 0)
        NuSPrintf(HubButtonStringBufferPc, "~0%s~~ \\ ~0%s~~", first, second);
    else
        NuSPrintf(HubButtonStringBufferPc, "~0%s%s~~", first, second);
    return HubButtonStringBufferPc;
}
/* PC4015e0 462 bytes. Exact command family, no prefix/default substitution. */
extern "C" i32 Text_ExpandButtonString(char *command, char *output) {
    if (NuStrICmp(command, "[TAG]") == 0 || NuStrICmp(command, "[TRIANGLE]") == 0 || NuStrICmp(command, "[T]") == 0) {
        NuStrCpy(output, TextButtonStringPc(12, TTab[816], TTab[817]));
        return 1;
    } else if (NuStrICmp(command, "[ACTION]") == 0 || NuStrICmp(command, "[SQUARE]") == 0 ||
               NuStrICmp(command, "[S]") == 0) {
        NuStrCpy(output, TextButtonStringPc(15, TTab[816], TTab[817]));
        return 1;
    } else if (NuStrICmp(command, "[SPECIAL]") == 0 || NuStrICmp(command, "[CIRCLE]") == 0 ||
               NuStrICmp(command, "[O]") == 0) {
        NuStrCpy(output, TextButtonStringPc(13, TTab[816], TTab[817]));
        return 1;
    } else if (NuStrICmp(command, "[JUMP]") == 0 || NuStrICmp(command, "[CROSS]") == 0 ||
               NuStrICmp(command, "[X]") == 0) {
        NuStrCpy(output, TextButtonStringPc(14, TTab[816], TTab[817]));
        return 1;
    } else if (NuStrICmp(command, "[TOGGLELEFT]") == 0) {
        NuStrCpy(output, TextButtonStringPc(16, TTab[816], TTab[817]));
        return 1;
    } else if (NuStrICmp(command, "[TOGGLERIGHT]") == 0) {
        NuStrCpy(output, TextButtonStringPc(18, TTab[816], TTab[817]));
        return 1;
    }
    return 0;
}

extern "C" void __cdecl MenuInitFreePlay(MENU *) {
} /*47a970/1 genuineRET*/

/* Five reviewed providers: append these four to existing hub.cpp. */

/* Reviewed remaining native HUD helper family, shared total, and sole DrawPanel owner. */
/* Append before moved DrawPanel, in same existing hub.cpp as DrawHitPointsPc. */

/* Complete native player HUD4213B; independent source gate pending. */
