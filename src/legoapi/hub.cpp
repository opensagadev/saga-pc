#include "saga.h"

#include <stdio.h>
/* Existing compiled native owners; declarations only, no replacement bodies/data. */
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif
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
/* Actual PC Draw3D omits the Android sign/episode scene branch. */
STATIC_ASSERT(offsetof(AREA_DATA_PC, index) == 0x7c);
STATIC_ASSERT(offsetof(AREA_DATA_PC, episode_index) == 0x86);
// Add only to the genuine Hub owner, after its existing private table definition.
struct GIZBUILDIT_PC;
STATIC_ASSERT(offsetof(GIZMO_PC, object) == 0);
/* 424750/92. Can share the fuller Customiser canonical type once integrated. */
/* 58eaa0/465. WORLDINFO currently carries Android buildit pointee types;
 * cast the pointee to the asserted PC array owner, never index Android84.
 */
STATIC_ASSERT(offsetof(WORLDINFO, giz_buildit_sys) == 0x46a8);
STATIC_ASSERT(offsetof(GAMEANIMOBJ, object_data) == 0x24);
STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, draw_mtx) == 0x80);
STATIC_ASSERT(offsetof(GIZBUILDITANIMDATA_s, start_mtx) == 0);
/* PC50ed50 282 bytes, native three stack parameters: action, player0, player1.
 * Last two are char* values tested for nonzero rather than string contents. */
extern "C" char *TextInputActionLabelPc(i32 device, i32 action, i32, i32);
extern "C" char HubButtonStringBufferPc[]; /*8016a8 native scratch owner size TBD, initial"???"*/
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
