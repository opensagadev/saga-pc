#include "saga.h"

#include <stdio.h>
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif
STATIC_ASSERT(offsetof(GAMEOBJECT, special_move_look_position_pc) == 0x1098);
extern "C" void LostTempleC_ResetPc(WORLDINFO *);
extern "C" void LostTempleC_UpdatePc(WORLDINFO *);
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
/* Draw3D on PC has no sign/episode scene branch. */
STATIC_ASSERT(offsetof(AREA_DATA_PC, index) == 0x7c);
STATIC_ASSERT(offsetof(AREA_DATA_PC, episode_index) == 0x86);
struct GIZBUILDIT_PC;
STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);
STATIC_ASSERT(offsetof(WORLDINFO, giz_buildit_sys) == 0x46a8);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, object_data) == 0x24);
STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, draw_mtx) == 0x80);
STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, start_mtx) == 0);
/* Takes action, player0 and player1; the last two are char* values tested
 * for nonzero rather than string contents. */
extern "C" char *TextInputActionLabelPc(i32 device, i32 action, i32, i32);
extern "C" char HubButtonStringBufferPc[];
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

extern "C" void Hub_LockUnlockDoors(WORLDINFO *world) {
    (void)world;
}

extern "C" void Hub_DrawStarField(void) {
    GetMenuID();
}

/* Lives in the menu unit and is never inlined into hub callers. */
extern "C" void SmartText(char *text, f32 x, f32 y, f32 z, f32 sx, f32 sy, f32 sz, i32 align, i32 red, i32 green,
                          i32 blue, f32 max_width, i32 center) {
    SmartTextEx(text, x, y, z, sx, sy, sz, align, red, green, blue, max_width, center, NULL, 0, 128);
}

extern "C" char *TextButtonStringPc(i32 action, const char *player0, const char *player1) {
    char first[32] = "";
    char second[32] = "";
    const i32 device0 = NuPadGetPort(0);
    const i32 device1 = NuPadGetPort(1);
    if (player0 != NULL && device0 >= 0) {
        NuStrCpy(first, TextInputActionLabelPc(device0, action, 0, 0));
    }
    if (player1 != NULL && device1 >= 0) {
        NuStrCpy(second, TextInputActionLabelPc(device1, action, 0, 0));
    }
    if (first[0] != 0 && second[0] != 0) {
        NuSPrintf(HubButtonStringBufferPc, "~0%s~~ \\ ~0%s~~", first, second);
    } else {
        NuSPrintf(HubButtonStringBufferPc, "~0%s%s~~", first, second);
    }
    return HubButtonStringBufferPc;
}

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

extern "C" void MenuInitFreePlay(MENU *) {
}
