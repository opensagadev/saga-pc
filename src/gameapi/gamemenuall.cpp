#include "batman.h"
#include "gameapi/apisave.h"
#include "gameapi/apitext.h"
#include "gameapi/edtools/edui.h"
#include "gameapi/gui/apimenu.h"
#include "gameapi/gui/apimenu_internal.h"
#include "gameframework/saveload.h"
#include "globals.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nutime.h"
#include <stdio.h>
#include "legoapi/audio/audio.h"
#include "legoapi/characters/core/players.h"
#include "legogame/game.h"
#include "nu2api/nuwin32/pcapi.h"
#include "legoapi/characters/motion/move.h"
#include "legogame/pcbatman.h"
#include "legogame/startup.h"
#include "nu2api/PCInput.h"
#include "nu2api/nu3d/nuprim.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nucore/nukeyboard.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/nustencilshadows_PC.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/text_tables.h"
#include "legoapi/characters/motion/gameanim.h"
#include "legoapi/cutscenes/gcutscn.h"
#include "legoapi/lib/camera.h"
#include "legoapi/render/fx.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/cutscene_include.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucore/NuMotionAccumFilter.h"
#include "nu2api/nucore/nuanim3.h"
#include "nu2api/nucore/nuanimdata.h"
#include "nu2api/nucore/nugcutscene.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/numath/nuang.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuquat.h"
#include "nu2api/numath/nuvec.h"
#include <float.h>
#include "legoapi/lib/text.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/hub.h"
#include "legoapi/world/world.h"
#include <string.h>
#include "legoapi/menus/core/panel.h"
#include "legoapi/menus/screens/menu.h"
#include "legoapi/render/core/gameliball.h"
#include "nu2api/huffdecode.h"
#include "gameapi/ai/aisys/aisys.h"
#include "nu2api/nuwin32/Stubs.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/nu2api_windows.h"
#include "gameapi/edpart.h"
#include "nu2api/nusound/nusound.h"
#include "legoapi/render/core/rtl.h"
#include "legoapi/items/base/apiobject.h"
#include "gameapi/edptl.h"
#include "nu2api/nucore/nuapi.h"
#include "legoapi/world/levels/levels.h"
#include "nu2api/nu3d/nurndr.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/startup/text.h"
#include "legoapi/gizmos/transport/grapples.h"
#include "legoapi/items/collect/minikits.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/world/area.h"
#include "legoapi/world/levels/episode.h"
#include "legoapi/world/mission.h"
#include "nu2api/nufile/nufpar.h"
#include "decomp.h"
#include "nu2api/NuEffectTex_pc.h"
#include "nu2api/nu3d/nuvport.h"
#include "nu2api/nucamera_dlist.h"
#include "nu2api/nufmv/pc/Bink/dsplay_bink.h"
#include "nu2api/nuhgo_dlist.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/nuwin32/d3dApiCalls.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include <math.h>
#include <stdlib.h>

extern "C" i32 saveload_slotused[];
#ifdef __cplusplus
#endif
/* Draw3D on PC has no sign/episode scene branch. */
struct GIZBUILDIT;
/* Takes action, player0 and player1; the last two are char* values tested
 * for nonzero rather than string contents. */
extern "C" char *TextInputActionLabel(i32 device, i32 action, i32, i32);
extern "C" char HubButtonStringBuffer[];
extern "C" {
    extern u8 MenuAutoSaveCancelFirst;
}

extern "C" void MenuDrawCardWarning(MENU *menu);
extern "C" void MenuDrawDoNotRemoveCard(MENU *menu);
extern "C" void MenuDrawFormatting(MENU *menu);
extern "C" void MenuDrawLoading(MENU *menu);
extern "C" void MenuDrawNoData(MENU *menu);
extern "C" void MenuEnterNoMemoryCard(MENU *menu);
extern "C" void MenuUpdateCardWarning(MENU *menu);
extern "C" void MenuUpdateDoNotRemoveCard(MENU *menu);
extern "C" void MenuUpdateFileCorrupt(MENU *menu);
f32 TextCrawlTime = 0.0f;
extern "C" {
    LEVELFIXUP LevelFixupStartup[2] = {
        {"losttemple_c", &UpdateSockets_LDATA, NULL, NULL, (void *)LostTempleC_Reset, (void *)LostTempleC_Update, NULL, NULL, NULL},
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}};
}
extern "C" {

    void fun_00508f30(void) {
    }
}

/* The release headers' empty NuError hook; the game menu unit keeps its copy. */
extern "C" void GamemenuallNuError(void) {
}

/* The release headers' prim colour and coordinate system stack helpers; this
 * unit keeps its own copies. */
static void MenusPrimSetColour(i32 colour) {
    i32 converted;
    if (NuPrimNeedsOverbrightening != 0) {
        converted = ((colour & 0xff) << 16) + ((colour >> 16) & 0xff) + (colour & 0xff00) + (colour & 0xff000000);
    } else {
        converted = ((colour & 0xfe) << 15) + ((colour >> 1) & 0x7f00) + ((colour >> 17) & 0x7f) + (colour & 0xff000000);
    }
    NuPrimStreamBufferPointer->vertex_ptr->colour = converted;
}

static void MenusPushCoordinateSystem(i32 system) {
    ++NuPrimCSPos;
    NuPrimSetCoordinateSystem(system);
}

static void MenusPopCoordinateSystem(void) {
    --NuPrimCSPos;
    NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[NuPrimCSPos]);
}

u8 PC_WINDOW_OPTIONS::IsBloomEnabled() {
    return bloomenabled;
}

u8 PC_WINDOW_OPTIONS::AreShadowsEnabled() {
    return shadowsenabled;
}

u8 PC_WINDOW_OPTIONS::IsSpeedBlurEnabled() {
    return speedblurenabled;
}

u8 PC_WINDOW_OPTIONS::IsHiresEnabled() {
    return usehires;
}

u8 PC_WINDOW_OPTIONS::IsHiresPending() {
    return usehirespending;
}

u8 PC_WINDOW_OPTIONS::fun_00509080() {
    return option459;
}

GRAPHICS_DISPLAY_MODE *PC_DEVICE_BACKEND::GetDisplayMode(i32 index) {
    return &modes[index];
}

i32 PC_DEVICE_BACKEND::GetMultisampleIndex() {
    return multisample_index;
}

/* Inline: MenuUpdateEffects calls it out of line but keeps its index in a
   callee-saved register across the calls, as for a COMDAT whose register use the
   compiler cannot rely on. */
inline bool PC_DEVICE_BACKEND::SupportsMultisample(i32 index) {
    return multisample_tested != 0 && quality[index] != -1;
}

u8 PC_DEVICE_BACKEND::SupportsAnyMultisample() {
    for (i32 index = 0; index != 5; index++) {
        if (index != 0 && SupportsMultisample(index)) {
            return 1;
        }
    }
    return 0;
}

u32 PC_DEVICE_BACKEND::GetRequestedMode() {
    return requested_mode;
}

i32 PC_DEVICE_BACKEND::GetRequestedMultisample() {
    return requested_multisample;
}

void PC_DEVICE_BACKEND::SetRequestedMultisample(i32 multisample) {
    requested_multisample = multisample;
}

extern "C" {
    i32 MenuAppliedAspect = 0;
}

void PC_DEVICE_BACKEND::ApplyAspectMode(i32 mode) {
    PcWindowOptions.SetAspectMode(mode);
    PcWindowOptions.Save();
    if (PcWindowOptions.aspect_mode == PC_ASPECT_AUTO) {
        MenuAppliedAspect = active_aspect_mode;
    } else {
        MenuAppliedAspect = PcWindowOptions.aspect_mode;
    }
}

u8 MENU_VIDEO_OPTIONS::Changed() {
    if (aspect_mode != PcWindowOptions.aspect_mode) {
        return 1;
    }
    if ((shadows_enabled == 1) != PcWindowOptions.shadowsenabled || (bloom_enabled == 1) != PcWindowOptions.bloomenabled ||
        (speed_blur_enabled == 1) != PcWindowOptions.speedblurenabled) {
        return 1;
    }
    return (hires_enabled == 1) != PcWindowOptions.usehirespending;
}

void MENU_VIDEO_OPTIONS::LoadFromWindowOptions() {
    aspect_mode = PcWindowOptions.aspect_mode;
    shadows_enabled = PcWindowOptions.shadowsenabled != 0;
    bloom_enabled = PcWindowOptions.bloomenabled != 0;
    speed_blur_enabled = PcWindowOptions.speedblurenabled != 0;
    hires_enabled = PcWindowOptions.usehirespending != 0;
}

void MENU_VIDEO_OPTIONS::Apply() {
    PcWindowOptions.SetAspectMode(aspect_mode);
    PcWindowOptions.Save();
    i32 aspect = PcWindowOptions.aspect_mode;
    if (aspect == PC_ASPECT_AUTO) {
        aspect = PCDeviceBackend.active_aspect_mode;
    }
    MenuAppliedAspect = aspect;
    PcWindowOptions.SetShadowsEnabled(shadows_enabled == 1);
    PcWindowOptions.SetBloomEnabled(bloom_enabled == 1);
    PcWindowOptions.SetSpeedBlurEnabled(speed_blur_enabled == 1);
    PcWindowOptions.SetUseHiresPending(hires_enabled == 1);
    PcWindowOptions.Save();
}

extern "C" {
    MENU_VIDEO_OPTIONS MenuVideoOptions = {0};
}

extern "C" void MenuEnterVideo(MENU *) {
    MenuVideoOptions.aspect_mode = PcWindowOptions.aspect_mode;
    MenuVideoOptions.shadows_enabled = PcWindowOptions.shadowsenabled != 0;
    MenuVideoOptions.bloom_enabled = PcWindowOptions.bloomenabled != 0;
    MenuVideoOptions.speed_blur_enabled = PcWindowOptions.speedblurenabled != 0;
    MenuVideoOptions.hires_enabled = PcWindowOptions.usehirespending != 0;
}

extern "C" void MenuExitVideo(MENU *) {
}

extern "C" bool MenuCanChangeMultisample(void) {
    for (i32 i = 0; i != 5; ++i) {
        if (i != 0 && PCDeviceBackend.multisample_tested != 0 && PCDeviceBackend.quality[i] != -1) {
            if (PCDeviceBackend.multisample_index == 6) {
                break;
            }
            return true;
        }
    }
    return false;
}

extern "C" bool MenuCanChangeShadows(void) {
    return PCDeviceBackend.requested_multisample == 0 || PcWindowOptions.shadowmethod == 1;
}

extern "C" bool fun_00509380(void) {
    return true;
}

extern "C" void MenuEnterEffects(MENU *) {
    MenuVideoOptions.aspect_mode = PcWindowOptions.aspect_mode;
    MenuVideoOptions.shadows_enabled = PcWindowOptions.shadowsenabled != 0;
    MenuVideoOptions.bloom_enabled = PcWindowOptions.bloomenabled != 0;
    MenuVideoOptions.speed_blur_enabled = PcWindowOptions.speedblurenabled != 0;
    MenuVideoOptions.hires_enabled = PcWindowOptions.usehirespending != 0;
}

extern "C" void MenuExitEffects(MENU *) {
}

extern "C" {
    f32 MenuVideoConfirmDuration = 0;
    i32 MenuVideoConfirmWait = 0;
}

extern "C" void MenuEnterVideoConfirm(MENU *menu) {
    MenuVideoConfirmDuration = 15.0f;
    MenuVideoConfirmWait = 3;
    menu->selected_row = 1;
}

extern "C" void MenuExitVideoConfirm(MENU *) {
}

extern "C" void MenuEnterHiresNotice(MENU *menu) {
    menu->selected_row = 0;
}

extern "C" void MenuExitHiresNotice(MENU *) {
}

extern "C" void MenuEnterQuit(MENU *menu) {
    menu->selected_row = 0;
}

extern "C" void MenuExitQuit(MENU *) {
}

/* Menu 1030 has empty enter, draw, update and exit steps on PC. */
extern "C" void fun_00509470(MENU *) {
}

extern "C" void fun_00509480(MENU *) {
}

extern "C" void fun_00509490(MENU *) {
}

extern "C" void fun_005094a0(MENU *) {
}

i32 header_g = 0;
i32 header_b = 0;
char MenuHeader[64] = {0};
i32 header_r = 0;
NUMTL *MenuFadeMtl = 0;
u8 MENUHEADERR = 0;
MENUFNINFO MenuInfo[100] = {
    {MENU_SAVE, MenuEnterSave, MenuDrawSave, MenuUpdateSave, MenuExitSave, -1, -1, 0, 1, 1, 0},
    {MENU_DO_NOT_REMOVE_CARD, NULL, MenuDrawDoNotRemoveCard, MenuUpdateDoNotRemoveCard, NULL, -1, -1, 0, 0, 0, 0},
    {MENU_INSERT_CARD, MenuEnterInsertCard, MenuDrawInsertCard, MenuUpdateInsertCard, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_NO_MEMORY_CARD, MenuEnterNoMemoryCard, MenuDrawNoMemoryCard, MenuUpdateNoMemoryCard, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_NOT_ENOUGH_SPACE, NULL, MenuDrawNotEnoughSpace, MenuUpdateNotEnoughSpace, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_NO_DATA, NULL, MenuDrawNoData, MenuUpdateNoData, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_FORMAT_CONFIRM, NULL, MenuDrawFormatConfirm, MenuUpdateFormatConfirm, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_FORMATTING, NULL, MenuDrawFormatting, MenuUpdateFormatting, NULL, -1, -1, 0, 0, 0, 0},
    {MENU_SAVE_CONFIRM, MenuEnterSaveConfirm, MenuDrawSaveConfirm, MenuUpdateSaveConfirm, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_SAVING, NULL, MenuDrawSaving, MenuUpdateSaving, NULL, -1, -1, 0, 0, 0, 0},
    {MENU_AUTO_SAVE_WARNING, MenuEnterAutoSaveWarning, MenuDrawAutoSaveWarning, MenuUpdateAutoSaveWarning, NULL, -1, -1, 0, 1, 0, 0},
    {MENU_AUTO_SAVE_CANCEL, MenuEnterAutoSaveCancel, MenuDrawAutoSaveCancel, MenuUpdateAutoSaveCancel, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_LOAD, MenuEnterLoad, MenuDrawLoad, MenuUpdateLoad, MenuExitLoad, -1, -1, 0, 1, 1, 0},
    {MENU_LOAD_CONFIRM, NULL, MenuDrawLoadConfirm, MenuUpdateLoadConfirm, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_LOADING, NULL, MenuDrawLoading, MenuUpdateLoading, NULL, -1, -1, 0, 0, 0, 0},
    {MENU_DELETING, NULL, MenuDrawDeleting, MenuUpdateDeleting, NULL, -1, -1, 0, 0, 0, 0},
    {MENU_SAVE_CANCEL, NULL, MenuDrawSaveCancel, MenuUpdateSaveCancel, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_LOAD_CANCEL, NULL, MenuDrawLoadCancel, MenuUpdateLoadCancel, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_FORMAT_CANCEL, NULL, MenuDrawFormatCancel, MenuUpdateFormatCancel, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_CARD_WARNING, MenuEnterCardWarning, MenuDrawCardWarning, MenuUpdateCardWarning, MenuExitCardWarning, -1, -1, 0, 1, 0, 0},
    {MENU_FILE_CORRUPT, NULL, MenuDrawFileCorrupt, MenuUpdateFileCorrupt, NULL, -1, -1, 0, 1, 1, 0},
    {MENU_HEADER_SAVE, MenuEnterHeaderSave, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
    {MENU_START_NEW_GAME, MenuEnterStartNewGame, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
    {MENU_DELETE_CONFIRM, NULL, MenuDrawDeleteConfirm, MenuUpdateDeleteConfirm, NULL, -1, -1, 0, 1, 1, 0},
    {1024, NULL, NULL, NULL, NULL, -1, -1, 0, 0, 0, 0},
    {MENU_VIDEO, MenuEnterVideo, MenuDrawVideo, MenuUpdateVideo, MenuExitVideo, -1, -1, 0, 0, 0, 0},
    {MENU_EFFECTS, MenuEnterEffects, MenuDrawEffects, MenuUpdateEffects, MenuExitEffects, -1, -1, 0, 0, 0, 0},
    {MENU_VIDEO_CONFIRM, MenuEnterVideoConfirm, MenuDrawVideoConfirm, MenuUpdateVideoConfirm, MenuExitVideoConfirm, -1, -1, 0, 0, 0, 0},
    {MENU_HIRES_NOTICE, MenuEnterHiresNotice, MenuDrawHiresNotice, MenuUpdateHiresNotice, MenuExitHiresNotice, -1, -1, 0, 0, 0, 0},
    {MENU_QUIT, MenuEnterQuit, MenuDrawQuit, MenuUpdateQuit, MenuExitQuit, -1, -1, 0, 0, 0, 0},
    {1030, fun_00509470, fun_00509480, fun_00509490, fun_005094a0, -1, -1, 0, 0, 0, 0},
};
i32 MenuLanguages = 1;
i32 MenusUsed = 31;
void (*drawslotinfofn)(f32, f32, i32, i32) = APIMenuDrawGameState;
i32 MenuFadeEnabled = 0;
i32 MenuDrawDropShadows = 0;
u8 MENUHEADERG = 0x7f;
u8 MENUHEADERB = 0xff;

void MenuInitialiseEx(MENUFNINFO *menu_info, i32 menu_id_count, i32 language_count, void (*draw_save_slots_info_fn)(f32, f32, i32, i32),
                      i32 is_fade_enabled, i32 is_shadow_enabled) {
    char menus_used_str[64];
    i32 menu_ids_used = menu_id_count;
    i32 i;
    if (menu_ids_used + 31 > 100) {
        menu_ids_used = 69;
    }
    for (i = 0; i < menu_ids_used; ++i) {
        MenuInfo[31 + i] = menu_info[i];
    }
    MenusUsed = menu_ids_used + 31;
    sprintf(menus_used_str, "Menus used: %d", MenusUsed);
    MenuLanguages = language_count;
    MenuHeader[0] = '\0';
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (draw_save_slots_info_fn != NULL) {
        drawslotinfofn = draw_save_slots_info_fn;
    }
    MenuFadeEnabled = is_fade_enabled;
    MenuDrawDropShadows = is_shadow_enabled;
    MenuFadeMtl = NuMtlCreate(1);
    MenuFadeMtl->attribs.z_mode = NUMTL_ZMODE_OFF;
    MenuFadeMtl->attribs.filter_mode = 1;
    MenuFadeMtl->attribs.lighting = NUMTL_LIGHTING_NONE;
    MenuFadeMtl->attribs.u_address = NUMTL_ADDRESS_CLAMP;
    MenuFadeMtl->attribs.v_address = NUMTL_ADDRESS_CLAMP;
    MenuFadeMtl->attribs.vertex_colour = 1;
    MenuFadeMtl->attribs.alpha_mode = NUMTL_ALPHA_BLEND;
    NuMtlUpdate(MenuFadeMtl);
    MenuSetPlatformState(-1);
}

void (*drawslotsfn)(MENU *, f32) = APIMenuDrawMemCardSlots;

void MenuInitialise(MENUFNINFO *menu_info, i32 menu_id_count, i32 language_count, void (*draw_save_slots_fn)(MENU *, f32),
                    i32 is_fade_enabled, i32 is_shadow_enabled) {
    MenuInitialiseEx(menu_info, menu_id_count, language_count, NULL, is_fade_enabled, is_shadow_enabled);
    if (draw_save_slots_fn != NULL) {
        drawslotsfn = draw_save_slots_fn;
    }
}

u8 MENUENTRYG = 127;
u8 MENUENTRYB = 255;
u8 MENUFLASH0G = 255;
u8 MENUFLASH0B = 255;
u8 MENUFLASH1R = 127;
u8 MENUFLASH1B = 255;
u8 MENUNORMALB = 0;
u8 MENUENTRYR = 0;
u8 MENUWARNG = 0;
u8 MENUWARNB = 0;
u8 MENUFLASH0R = 0;
u8 MENUFLASH1G = 0;
u8 MENUNORMALR = 255;
u8 MENUNORMALG = 127;
u8 MENUWARNR = 255;

void MenuSetColours(u8 normal_r, u8 normal_g, u8 normal_b, u8 entry_r, u8 entry_g, u8 entry_b, u8 header_r, u8 header_g, u8 header_b,
                    u8 warn_r, u8 warn_g, u8 warn_b, u8 flash0_r, u8 flash0_g, u8 flash0_b, u8 flash1_r, u8 flash1_g, u8 flash1_b) {
    MENUNORMALR = normal_r;
    MENUNORMALG = normal_g;
    MENUNORMALB = normal_b;
    MENUENTRYR = entry_r;
    MENUENTRYG = entry_g;
    MENUENTRYB = entry_b;
    MENUHEADERR = header_r;
    MENUHEADERG = header_g;
    MENUHEADERB = header_b;
    MENUWARNR = warn_r;
    MENUWARNG = warn_g;
    MENUWARNB = warn_b;
    MENUFLASH0R = flash0_r;
    MENUFLASH0G = flash0_g;
    MENUFLASH0B = flash0_b;
    MENUFLASH1R = flash1_r;
    MENUFLASH1G = flash1_g;
    MENUFLASH1B = flash1_b;
}

f32 menu_pulsate_speed = 0.0f;

extern "C" void MenuSetPulsateSpeed(f32 speed) {
    menu_pulsate_speed = speed;
}

f32 MenuTopY = 0.8f;
f32 MENUBOTY = -0.75f;

extern "C" void MenuSetTopBottom(f32 top, f32 bottom) {
    MenuTopY = top;
    MENUBOTY = bottom;
}

void (*predrawfn)(MENU *) = 0;

extern "C" void MenuSetPreDrawFn(void (*function)(MENU *)) {
    predrawfn = function;
}

void (*headerdrawfn)(void) = DrawMenuHeader;

extern "C" void MenuSetHeaderDrawFn(void (*function)(void)) {
    headerdrawfn = function;
}

i32 MenuSFX = -1;
MENU GameMenu[10] = {0};

void MenuReset(void) {
    memset(GameMenu, 0, sizeof(GameMenu));
    GameMenu[0].menu = -1;
    MenuSFX = -1;
    GameMenuLevel = 0;
    MenuSetPlatformState(-1);
}

i32 MENUSFX_MENUMOVE = -1;
i32 MENUSFX_MENUSELECT = -1;
i32 MENUSFX_MENUBACK = -1;
i32 MENUSFX_MENUNOENTRY = -1;
extern "C" {

    void MenuRegisterSoundFX(i32 move, i32 select, i32 back, i32 no_entry) {
        MENUSFX_MENUMOVE = move;
        MENUSFX_MENUSELECT = select;
        MENUSFX_MENUBACK = back;
        MENUSFX_MENUNOENTRY = no_entry;
    }
}

void MenuRememberCursor(MENU *menu) {
    if (menu->menu == -1) {
        return;
    }
    if (MenuInfo[menu->menu].memory_x != -1) {
        MenuInfo[menu->menu].memory_x = menu->selected_column;
    }
    if (MenuInfo[menu->menu].memory_y != -1) {
        MenuInfo[menu->menu].memory_y = menu->selected_row;
    }
}

i32 MenuStopDraw = 0;
i32 MenuValidated = 0;
f32 MenuAlpha = 0;
i32 MenuA = 0;
i32 CurrentMenuId = 0;

MENU *NewMenu(i32 menu_id, i32 menu_y, i32 unused) {
    MENU *menu = NULL;
    i32 index;
    i32 selected = -1;
    i32 previous_index;
    MenuSetPlatformState(1);
    CurrentMenuId = menu_id;
    for (index = 0; index < MenusUsed; index++) {
        if (MenuInfo[index].id == menu_id) {
            selected = index;
            break;
        }
    }
    previous_index = GameMenu[GameMenuLevel].menu;
    GameMenu[GameMenuLevel].state_bc = 0;

    if (GameMenu[GameMenuLevel].menu != -1) {
        if (MenuInfo[GameMenu[GameMenuLevel].menu].memory_x != -1) {
            MenuInfo[GameMenu[GameMenuLevel].menu].memory_x = GameMenu[GameMenuLevel].selected_column;
        }
        if (MenuInfo[GameMenu[GameMenuLevel].menu].memory_y != -1) {
            MenuInfo[GameMenu[GameMenuLevel].menu].memory_y = GameMenu[GameMenuLevel].selected_row;
        }
    }
    if (selected != -1) {
        GameMenuLevel++;
        if (GameMenuLevel >= 10) {
            GameMenuLevel--;
        }
        menu = &GameMenu[GameMenuLevel];
        menu->menu_time = 0.0f;
        menu->time_04 = 0.0f;
        menu->first_column = 0;
        menu->first_row = 0;
        menu->last_column = 0;
        menu->last_row = 0;
        menu->state = 0;
        menu->draw_item = 0;
        menu->state_b8 = 1;
        menu->state_b4 = 1;
        menu->state_b0 = 0;
        menu->state_c0 = 0;
        menu->enter_state = 0;
        menu->previous_menu = (i8)previous_index;
        menu->menu = (i16)selected;
        MenuStopDraw = 1;
        if (MenuInfo[menu->menu].id != -1 && MenuInfo[menu->menu].draw_fn != NULL) {
            MenuInfo[menu->menu].draw_fn(menu);
        }
        menu->last_row = (i16)((i16)menu->draw_item - 1);
        MenuStopDraw = 0;
        if (menu_y >= menu->first_row && menu_y <= menu->last_row) {
            menu->selected_row = (i16)menu_y;
        } else {
            menu->selected_row = 0;
        }
        menu->selected_column = 0;
        if (MenuInfo[menu->menu].enter_fn != NULL) {
            MenuInfo[menu->menu].enter_fn(menu);
        }
        if (MenuInfo[menu->menu].memory_x != -1) {
            menu->selected_column = MenuInfo[menu->menu].memory_x;
        }
        if (menu->selected_column < menu->first_column) {
            menu->selected_column = menu->first_column;
        } else if (menu->selected_column > menu->last_column) {
            menu->selected_column = menu->last_column;
        }
        if (menu_y >= menu->first_row && menu_y <= menu->last_row) {
            menu->selected_row = (i16)menu_y;
        } else {
            if (MenuInfo[menu->menu].memory_y != -1) {
                menu->selected_row = MenuInfo[menu->menu].memory_y;
            }
            if (menu->selected_row < menu->first_row) {
                menu->selected_row = menu->first_row;
            } else if (menu->selected_row > menu->last_row) {
                menu->selected_row = menu->last_row;
            }
        }
        menu->menu_time = 0.0f;
        menu->time_04 = 0.0f;
        menu->flags_17 = -1;
        menu->transition_duration = 0.0f;
        menu->transition_time = 0.0f;
    } else {
        GameMenu[GameMenuLevel].menu = -1;
    }
    MenuAlpha = 0.0f;
    MenuA = 0;
    MenuValidated = 0;
    return menu;
}

void BackupMenu(void) {
    i32 level = GameMenuLevel;
    if (level != 0) {
        MENU *menu = &GameMenu[level];
        MENU *view = menu;
        MENU *parent;
        if (MenuInfo[menu->menu].exit_fn != NULL) {
            MenuInfo[menu->menu].exit_fn(menu);
            level = GameMenuLevel;
        }
        MenuRememberCursor(menu);
        view->menu = -1;
        view->state_b4 = 0;
        view->state_b8 = 1;
        view->state_bc = 0;
        --level;
        parent = &GameMenu[level];
        parent->state_b8 = 1;
        parent->menu_time = 0.0f;
        parent->state_b4 = 1;
        GameMenuLevel = level;
        parent->state_b0 = 0;
        parent->state_c0 = 0;
        if (parent->menu != -1 && MenuInfo[parent->menu].enter_fn != NULL) {
            MenuInfo[parent->menu].enter_fn(parent);
            level = GameMenuLevel;
        }
        if (MenuSFX == -1) {
            MenuSFX = MENUSFX_MENUBACK;
        }
        MenuAlpha = 0.0f;
        MenuA = 0;
        MenuValidated = 0;
        if (level == 0) {
            MenuSetPlatformState(-1);
        }
    }
}

void BackupMenuNoFn(void) {
    i32 level = GameMenuLevel;
    if (level != 0) {
        MENU *menu = &GameMenu[level];
        MENU *view = menu;
        MENU *parent;
        if (MenuInfo[menu->menu].exit_fn != NULL) {
            MenuInfo[menu->menu].exit_fn(menu);
            level = GameMenuLevel;
        }
        MenuRememberCursor(menu);
        view->state_b4 = 0;
        view->state_b8 = 1;
        view->state_bc = 0;
        --level;
        parent = &GameMenu[level];
        view->menu = -1;
        parent->menu_time = 0.0f;
        GameMenuLevel = level;
        parent->state_b8 = 1;
        parent->state_b4 = 1;
        parent->state_b0 = 0;
        parent->state_c0 = 0;
        if (MenuSFX == -1) {
            MenuSFX = MENUSFX_MENUBACK;
        }
        if (level == 0) {
            MenuSetPlatformState(-1);
        }
        MenuA = 0;
        MenuAlpha = 0.0f;
        MenuValidated = 0;
    }
}

void MenuRepeat(i32 *held, i32 *repeat, f32 *repeat_time, u8 *repeat_count, f32 interval, f32 elapsed) {
    if (*held != 0) {
        if (*repeat_time > 0.0f) {
            *repeat_time -= elapsed;
            if (*repeat_time <= 0.0f) {
                *repeat = 1;
                *repeat_time = interval;
                ++*repeat_count;
            }
        }
    } else {
        *repeat_time = 0.5f;
        *repeat_count = 0;
    }
}

f32 menu_pulsate = 0.0f;
i32 MenuSuppressUpHeld = 0;
i32 MenuSuppressDownHeld = 0;
i32 MenuSuppressLeftHeld = 0;
i32 MenuSuppressRightHeld = 0;
f32 sfx_wait = 0.0f;
i32 MenuResult = 0;
i32 menu_pulsate_angle = 0;

i32 UpdateMenu(u32 primary_held, u32 primary_pressed, u32 alternate_held, u32 alternate_pressed, f32 elapsed, u32 confirm_mask,
               u32 cancel_mask, u32 start_mask, u32 select_mask) {
    i32 previous_item = 0;
    MENU *menu;
    u32 held;
    u32 pressed;
    i32 up_pressed;
    i32 down_pressed;
    i32 left_pressed;
    i32 right_pressed;
    i32 up_held;
    i32 down_held;
    i32 left_held;
    i32 right_held;
    i32 confirm_pressed;
    i32 cancel_pressed;
    u32 start_pressed;
    u32 select_pressed;
    u32 action_04_pressed;
    u32 action_08_pressed;
    i32 output_up_pressed;
    i32 output_down_pressed;
    i32 column_count;
    i32 row_count;
    i32 row_clamped;
    i16 first_column;
    i16 last_column;
    i16 first_row;
    i16 last_row;

    MenuResult = 0;
    if (MenuValidated == 0) {
        return 0;
    }

    menu = &GameMenu[GameMenuLevel];
    if (MenuFadeEnabled != 0) {
        if (MenuAlpha < 1.0f) {
            MenuAlpha += elapsed + elapsed;
            if (MenuAlpha > 1.0f) {
                MenuAlpha = 1.0f;
            }
        }
    } else {
        MenuAlpha = 1.0f;
    }

    MenuA = (i32)((f64)NuTrigTable[((i32)(16384.0 * MenuAlpha) >> 1) & 0x7fff] * 128.0);

    if (menu_pulsate_speed > 0.0f) {
        menu_pulsate_angle += (i32)((f64)elapsed * 65536.0 * menu_pulsate_speed);
        menu_pulsate = ((f64)NuTrigTable[(menu_pulsate_angle >> 1) & 0x7fff] + 1.0) * 0.5;
    } else {
        menu_pulsate = 0.0f;
    }
    MenuSFX = -1;
    if (sfx_wait > 0.0f) {
        sfx_wait -= elapsed;
    }
    if (menu->menu == -1) {
        return 0;
    }

    held = primary_held;
    pressed = primary_pressed;
    if ((held & 0xf000) == 0) {
        held |= alternate_held;
    }
    if ((pressed & 0xf000) == 0) {
        pressed |= alternate_pressed;
    }

    if (MenuAlpha < 0.5f) {
        held = 0;
        pressed = 0;
    }

    up_pressed = pressed & 0x1000;
    down_pressed = pressed & 0x4000;
    left_pressed = pressed & 0x8000;
    right_pressed = pressed & 0x2000;
    up_held = held & 0x1000;
    if (MenuSuppressUpHeld != 0) {
        if (up_held != 0) {
            up_held = 0;
        } else {
            MenuSuppressUpHeld = 0;
        }
    }
    down_held = held & 0x4000;
    if (MenuSuppressDownHeld != 0) {
        if (down_held != 0) {
            down_held = 0;
        } else {
            MenuSuppressDownHeld = 0;
        }
    }
    left_held = held & 0x8000;
    if (MenuSuppressLeftHeld != 0) {
        if (left_held != 0) {
            left_held = 0;
        } else {
            MenuSuppressLeftHeld = 0;
        }
    }
    right_held = held & 0x2000;
    if (MenuSuppressRightHeld != 0) {
        if (right_held != 0) {
            right_held = 0;
        } else {
            MenuSuppressRightHeld = 0;
        }
    }

    action_04_pressed = pressed & 4;
    action_08_pressed = pressed & 8;
    confirm_pressed = pressed & confirm_mask;
    cancel_pressed = pressed & cancel_mask;
    start_pressed = pressed & start_mask;
    select_pressed = pressed & select_mask;
    if (menu->time_04 == 0.0f) {
        confirm_pressed = cancel_pressed = 0;
    }

    if (up_pressed != 0 && down_pressed != 0) {
        up_pressed = down_pressed = 0;
    }
    if (left_pressed != 0 && right_pressed != 0) {
        left_pressed = right_pressed = 0;
    }
    if (up_held != 0 && down_held != 0) {
        up_held = down_held = 0;
    }
    if (left_held != 0 && right_held != 0) {
        left_held = right_held = 0;
    }
    if (action_04_pressed != 0 && action_08_pressed != 0) {
        action_04_pressed = action_08_pressed = 0;
    }

    output_up_pressed = up_pressed;
    output_down_pressed = down_pressed;
    if (menu->input_disabled != 0) {
        up_pressed = 0;
        down_pressed = 0;
        start_pressed = 0;
        confirm_pressed = 0;
        cancel_pressed = 0;
    }

    MenuRepeat(&up_held, &up_pressed, &menu->repeat_up_time, &menu->repeat_up_count, 0.1f, elapsed);
    MenuRepeat(&down_held, &down_pressed, &menu->repeat_down_time, &menu->repeat_down_count, 0.1f, elapsed);
    MenuRepeat(&left_held, &left_pressed, &menu->repeat_left_time, &menu->repeat_left_count, 0.1f, elapsed);
    MenuRepeat(&right_held, &right_pressed, &menu->repeat_right_time, &menu->repeat_right_count, 0.1f, elapsed);

    first_column = menu->first_column;
    last_column = menu->last_column;
    column_count = last_column - first_column + 1;
    first_row = menu->first_row;
    last_row = menu->last_row;
    row_count = last_row - first_row + 1;
    if (menu->transition_time < menu->transition_duration) {
        confirm_pressed = cancel_pressed = 0;
        menu->transition_time += elapsed;
        if (menu->transition_time >= menu->transition_duration) {
            menu->transition_time = menu->transition_duration;
            if (menu->flags_17 != -1) {
                NewMenu(menu->flags_17, -1, -1);
            }
        }
    } else {
        row_clamped = 0;
        previous_item = menu->selected_row;
        if (menu->selected_row < first_row) {
            menu->selected_row = first_row;
            row_clamped = 1;
        } else if (menu->selected_row > last_row) {
            menu->selected_row = last_row;
            row_clamped = 1;
        } else if (MenuInfo[menu->menu].wrap != 0) {
            if (down_pressed != 0) {
                if (++menu->selected_row > last_row) {
                    menu->selected_row = first_row;
                }
            } else if (up_pressed != 0) {
                if (--menu->selected_row < first_row) {
                    menu->selected_row = last_row;
                }
            }
        } else if (down_pressed != 0 && menu->selected_row < last_row) {
            ++menu->selected_row;
        } else if (up_pressed != 0 && menu->selected_row > first_row) {
            --menu->selected_row;
        }
        if (menu->selected_row != previous_item) {
            menu->time_04 = 0.0f;
            if (row_clamped == 0) {
                MenuSFX = MENUSFX_MENUMOVE;
            }
        }
        if (menu->state == 1 || (menu->state == 2 && menu->selected_row == 0) || (menu->state == 3 && menu->selected_row != last_row)) {
            previous_item = menu->selected_column;
            if (menu->selected_column < menu->first_column) {
                menu->selected_column = menu->first_column;
            } else if (menu->selected_column > menu->last_column) {
                menu->selected_column = menu->last_column;
            }
            if (right_pressed != 0 && menu->selected_column < menu->last_column) {
                ++menu->selected_column;
            } else if (left_pressed != 0 && menu->selected_column > menu->first_column) {
                --menu->selected_column;
            }
            if (menu->selected_column != previous_item) {
                MenuSFX = MENUSFX_MENUMOVE;
                menu->time_04 = 0.0f;
            }
        }
    }
    menu->selected_item = menu->selected_row;
    menu->selected_item_column = menu->selected_column;
    menu->previous_item = previous_item;
    menu->buttons_held = held;
    menu->buttons_pressed = pressed;
    menu->column_count = column_count;
    menu->row_count = row_count;
    menu->up_pressed = output_up_pressed;
    menu->down_pressed = output_down_pressed;
    menu->left_pressed = left_pressed;
    menu->right_pressed = right_pressed;
    menu->confirm_pressed = confirm_pressed;
    menu->cancel_pressed = cancel_pressed;
    menu->up_held = up_held;
    menu->down_held = down_held;
    menu->left_held = left_held;
    menu->right_held = right_held;
    menu->start_pressed = start_pressed;
    menu->select_pressed = select_pressed;
    menu->action_04_pressed = action_04_pressed;
    menu->action_08_pressed = action_08_pressed;
    menu->menu_time += elapsed;
    menu->time_04 += elapsed;

    if (MenuInfo[menu->menu].update_fn != NULL) {
        MenuInfo[menu->menu].update_fn(menu);
    }
    if (MenuSFX != -1 && menu->input_disabled == 0) {
        PlaySfxById(MenuSFX, 0);
        MenuSFX = -1;
    }
    return MenuResult;
}

extern "C" i32 GameMenuIsOpen(void) {
    return GameMenu[GameMenuLevel].menu != -1;
}

i32 MenuLoadStarted = 0;
i32 memcard_loadneeded = 0;
i32 memcard_loadstarted = 0;
i32 memcard_loadfailed = 0;
i32 memcard_loadcorrupt = 0;

extern "C" void MenuRequestLoad(void) {
    MenuLoadStarted = 1;
    memcard_loadneeded = 1;
    memcard_loadstarted = 0;
    memcard_loadfailed = 0;
    memcard_loadcorrupt = 0;
}

i32 memcard_saveneeded = 0;
i32 memcard_savestarted = 0;
i32 memcard_savefailed = 0;

extern "C" void MenuRequestSave(void) {
    memcard_saveneeded = 1;
    memcard_savestarted = 0;
    memcard_savefailed = 0;
}

i32 SAVESLOTS = 6;
i32 SAVESIZE_ADDITIONAL = 3;
i32 saveload_cardtype = 2;
i32 saveload_freespace = 2048;
i32 saveload_cardformatted = 1;
i32 MenuSaveOccurred = 0;
i32 Menu_InSaveFlow = 0;
i32 Menu_InHeaderSave = 0;
i32 Menu_LastFlow = 0;
i32 Menu_DisableCancel = 0;
static i32 lastslot = 0;
static i32 slideleft = 0;
static i32 slideright = 0;
i32 memcard_slot = 0;
i32 memcard_cardchanged = 0;
i32 memcard_slotsused = 0;
i32 SAVESIZE = 50;
i32 saveload_filecorrupt = 0;
char *apitxt_SAVEGAME = "Save Game";

void MenuEnterSave(MENU *menu) {
    MenuSaveOccurred = 0;
    memcard_cardchanged = 0;
    Menu_DisableCancel = 0;
    Menu_InSaveFlow = 1;
    Menu_LastFlow = 2;
    if (SAVESLOTS <= 6) {
        menu->last_column = (i16)(SAVESLOTS - 1);
    } else {
        menu->last_column = (i16)memcard_slotsused;
        if (memcard_slotsused == SAVESLOTS || saveload_freespace < SAVESIZE_ADDITIONAL) {
            --menu->last_column;
        }
    }
    menu->state = 2;
    slideright = slideleft = lastslot = 0;
    NuStrCpy(MenuHeader, apitxt_SAVEGAME);
    menu->last_row = 1;
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (saveload_cardtype != 2) {
        NewMenu(MENU_INSERT_CARD, -1, -1);
    } else if (saveload_filecorrupt != 0) {
        NewMenu(MENU_FILE_CORRUPT, -1, -1);
    } else if (saveload_cardformatted == 0) {
        NewMenu(MENU_FORMAT_CONFIRM, 1, -1);
    } else if (saveload_savepresent == 0 && saveload_freespace < SAVESIZE) {
        NewMenu(MENU_NOT_ENOUGH_SPACE, -1, -1);
    } else if (Menu_InHeaderSave != 0) {
        memcard_slot = -1;
        NewMenu(MENU_SAVE_CONFIRM, 1, -1);
    }
}

void MenuEnterInsertCard(MENU *menu) {
}

extern "C" void MenuEnterNoMemoryCard(MENU *menu) {
}

i32 Menu_Overwriteflag = 0;

void MenuEnterSaveConfirm(MENU *menu) {
    i32 slot = Menu_InHeaderSave != 0 ? SAVESLOTS : memcard_slot;
    Menu_Overwriteflag = saveload_slotused[slot] != 0;
}

i32 memcard_autosavedisabled = 0;
i32 memcard_autosaveenabled = 0;

void MenuEnterAutoSaveWarning(MENU *menu) {
    memcard_autosaveenabled = 1;
    memcard_autosavedisabled = 0;
}

i32 MenuASCancelFinished = 0;

void MenuEnterAutoSaveCancel(MENU *menu) {
    if (MenuASCancelFinished == 0) {
        memcard_autosaveenabled = 0;
        memcard_autosavedisabled = 0;
        menu->last_row = 0;
    }
}

i32 MenuLoadOccurred = 0;
i32 Menu_InLoadFlow = 0;

void MenuEnterLoad(MENU *menu) {
    i32 last_column = SAVESLOTS;
    memcard_cardchanged = 0;
    if (last_column > 6) {
        last_column = memcard_slotsused;
    }
    menu->last_column = (i16)(last_column - 1);
    menu->state = 2;
    menu->last_row = 1;
    slideright = slideleft = lastslot = 0;
    Menu_InLoadFlow = 1;
    Menu_LastFlow = 3;
    MenuLoadOccurred = 0;
    MenuLoadStarted = 0;
    if (saveload_filecorrupt != 0) {
        NewMenu(MENU_FILE_CORRUPT, -1, -1);
    } else if (saveload_cardtype != 2) {
        NewMenu(MENU_NO_MEMORY_CARD, -1, -1);
    } else if (saveload_savepresent == 0 || saveload_cardformatted == 0) {
        NewMenu(MENU_NO_DATA, -1, -1);
    }
}

i32 MenuCardWarningState = 0;

void MenuEnterCardWarning(MENU *menu) {
    MenuCardWarningState = 0;
    memcard_cardchanged = 0;
}

void MenuEnterHeaderSave(MENU *menu) {
    Menu_InHeaderSave = 1;
    BackupMenuNoFn();
    NewMenu(MENU_SAVE, -1, -1);
}

i32 saveload_autosave = -1;

void MenuEnterStartNewGame(MENU *menu) {
    memcard_autosaveenabled = 0;
    saveload_autosave = -1;
    BackupMenu();
}

void MenuExitSave(MENU *menu) {
    Menu_InSaveFlow = 0;
}

void MenuExitLoad(MENU *menu) {
    Menu_InLoadFlow = 0;
}

i32 Menu_InWarningFlow = 0;

void MenuExitCardWarning(MENU *menu) {
    Menu_InWarningFlow = 0;
}

extern "C" void fun_0050a7a0(MENU *menu) {
}

extern "C" void fun_0050a7b0(MENU *menu) {
}

extern "C" void fun_0050a7c0(MENU *menu) {
}

extern "C" void fun_0050a7d0(MENU *menu) {
}

extern "C" void fun_0050a7e0(MENU *menu) {
}

extern "C" void MenuDrawDoNotRemoveCard(MENU *menu) {
}

void MenuDrawFormatConfirm(MENU *menu) {
}

void MenuUpdateSave(MENU *menu) {
    i32 slot;
    if (Menu_DisableCancel != 0) {
        MenuInfo[menu->menu].cancel_prompt = 0;
    } else {
        MenuInfo[menu->menu].cancel_prompt = 1;
    }
    if (memcard_cardchanged != 0 || saveload_cardtype != 2) {
        MenuEnterSave(menu);
    }
    slot = menu->selected_column - menu->first_column;
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (menu->selected_row == 0) {
            if (saveload_slotused[slot] == 0 && saveload_freespace < SAVESIZE_ADDITIONAL) {
                MenuSFX = MENUSFX_MENUNOENTRY;
            } else {
                memcard_slot = slot;
                NewMenu(MENU_SAVE_CONFIRM, 1, -1);
            }
        } else if (menu->selected_row == menu->last_row && Menu_DisableCancel == 0) {
            NewMenu(MENU_SAVE_CANCEL, 1, -1);
        }
    } else if (menu->cancel_pressed != 0 && Menu_DisableCancel == 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        NewMenu(MENU_SAVE_CANCEL, 1, -1);
    }
}

extern "C" void MenuUpdateDoNotRemoveCard(MENU *menu) {
    if (menu->menu_time > 2.0f && saveload_status == 1) {
        BackupMenu();
    }
}

void MenuUpdateInsertCard(MENU *menu) {
    if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        NewMenu(MENU_SAVE_CANCEL, 1, -1);
    }
    if (menu->confirm_pressed != 0 && menu->selected_row == menu->last_row) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        NewMenu(MENU_SAVE_CANCEL, 1, -1);
    }
}

void MenuUpdateNoMemoryCard(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (menu->selected_row == 0) {
            BackupMenu();
        } else {
            BackupMenuNoFn();
            NewMenu(MENU_LOAD_CANCEL, 1, -1);
        }
    }
    if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        NewMenu(MENU_LOAD_CANCEL, 1, -1);
    }
}

void MenuUpdateNotEnoughSpace(MENU *menu) {
    if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        NewMenu(MENU_SAVE_CANCEL, 1, -1);
    }
}

void MenuUpdateNoData(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        BackupMenu();
    }
    if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        BackupMenu();
    }
}

void MenuUpdateFormatConfirm(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
    } else if (menu->cancel_pressed != 0 && MenuInfo[GameMenu[GameMenuLevel - 1].menu].id != MENU_CARD_WARNING) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        NewMenu(MENU_FORMAT_CANCEL, 1, -1);
    }
}

extern "C" void MenuUpdateFileCorrupt(MENU *menu) {
    i32 cancel;
    if (MenuInfo[GameMenu[GameMenuLevel - 1].menu].id == MENU_CARD_WARNING) {
        MenuInfo[menu->menu].cancel_prompt = 0;
    } else {
        MenuInfo[menu->menu].cancel_prompt = 1;
    }
    cancel = 0;
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        cancel = 1;
    } else if (menu->cancel_pressed != 0 && MenuInfo[GameMenu[GameMenuLevel - 1].menu].id != MENU_CARD_WARNING) {
        MenuSFX = MENUSFX_MENUSELECT;
        cancel = 1;
    }
    if (cancel != 0) {
        if (MenuInfo[GameMenu[GameMenuLevel - 1].menu].id == MENU_SAVE) {
            BackupMenuNoFn();
            NewMenu(MENU_SAVE_CANCEL, 1, -1);
        } else if (MenuInfo[GameMenu[GameMenuLevel - 1].menu].id == MENU_CARD_WARNING) {
            MenuCardWarningState = 3;
            memcard_cardchanged = 0;
            BackupMenuNoFn();
        } else {
            BackupMenuNoFn();
            NewMenu(MENU_LOAD_CANCEL, 1, -1);
        }
    }
}

i32 memcard_formatme = 0;
i32 memcard_formatting = 0;
i32 memcard_formatfailed = 0;
f32 memcard_formatmessage_delay = 0.0f;
f32 memcard_formatresult_delay = 0.0f;

void MenuUpdateFormatting(MENU *menu) {
    if (memcard_formatme != 0) {
        memcard_formatmessage_delay = 1.5f;
    }
    if (memcard_formatme != 0 || memcard_formatting != 0 || memcard_formatmessage_delay > 0.0f) {
        memcard_formatresult_delay = 1.5f;
        if (memcard_formatme != 0 && memcard_cardchanged != 0) {
            memcard_formatme = 0;
            memcard_formatfailed = 1;
        }
    }
    if (memcard_formatresult_delay <= 0.0f) {
        BackupMenu();
    }
}

i32 memcard_deleteneeded = 0;
i32 memcard_deletestarted = 0;

void MenuUpdateDeleteConfirm(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (menu->selected_row == 1) {
            BackupMenu();
        } else {
            memcard_slot = -1;
            memcard_deleteneeded = 1;
            memcard_deletestarted = 0;
            BackupMenuNoFn();
            BackupMenuNoFn();
            NewMenu(MENU_DELETING, 0, -1);
        }
    } else if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenu();
    }
}

i32 memcard_deletefailed = 0;
f32 memcard_deletemessage_delay = 0.0f;
f32 memcard_deleteresult_delay = 0.0f;

void MenuUpdateDeleting(MENU *menu) {
    if (memcard_deleteneeded != 0 || memcard_deletemessage_delay > 0.0f) {
        memcard_deleteresult_delay = 1.5f;
        if (memcard_deleteneeded != 0 && memcard_cardchanged != 0) {
            memcard_deleteneeded = 0;
            memcard_deletefailed = 1;
        }
    }
    if (memcard_deleteresult_delay <= 0.0f) {
        BackupMenu();
    }
}

void MenuUpdateSaveConfirm(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (menu->selected_row == 1) {
            BackupMenu();
        } else {
            memcard_saveneeded = 1;
            memcard_savestarted = 0;
            memcard_savefailed = 0;
            BackupMenuNoFn();
            NewMenu(MENU_SAVING, 0, -1);
        }
    } else if (menu->cancel_pressed != 0) {
        BackupMenu();
        MenuSFX = MENUSFX_MENUSELECT;
    }
}

f32 memcard_savemessage_delay = 0.0f;
f32 memcard_saveresult_delay = 0.0f;
i32 memcard_autosave = 0;

void MenuUpdateSaving(MENU *menu) {
    if (memcard_saveneeded != 0) {
        memcard_savemessage_delay = 1.5f;
    }
    if (memcard_saveneeded != 0 || memcard_savestarted != 0 || memcard_savemessage_delay > 0.0f) {
        memcard_saveresult_delay = 1.5f;
        if (memcard_saveneeded != 0 && (memcard_cardchanged != 0 || saveload_cardtype == -1)) {
            memcard_saveneeded = 0;
            memcard_savefailed = 1;
        }
    }
    if (memcard_saveresult_delay <= 0.0f) {
        if (memcard_savefailed != 0) {
            BackupMenu();
        } else if (memcard_autosave != 0 && saveload_autosave != -1) {
            BackupMenuNoFn();
            BackupMenuNoFn();
            NewMenu(MENU_AUTO_SAVE_WARNING, 1, -1);
        } else {
            BackupMenuNoFn();
            BackupMenu();
        }
    }
}

void MenuUpdateAutoSaveWarning(MENU *menu) {
    if (memcard_cardchanged != 0) {
        BackupMenu();
    }
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenu();
    }
}

f32 memcard_autosavecanceldelay = 1.5f;
i32 g_enableButtonPrompts = 1;
extern "C" {
    u8 MenuAutoSaveCancelFirst = 1;

    void MenuUpdateAutoSaveCancel(MENU *menu) {
        if (MenuASCancelFinished) {
            MenuASCancelFinished = 0;
            BackupMenu();
            return;
        }
        if (memcard_savefailed) {
            if (MenuAutoSaveCancelFirst) {
                MenuAutoSaveCancelFirst = 0;
                memcard_autosavecanceldelay = 5.0f;
                g_enableButtonPrompts = 0;
                return;
            }
            if (memcard_autosavecanceldelay > 0.0f) {
                return;
            }
            g_enableButtonPrompts = 1;
            NewMenu(MENU_SAVE, -1, -1);
            memcard_autosavecanceldelay = 5.0f;
            MenuASCancelFinished = 1;
            MenuAutoSaveCancelFirst = 1;
        }
        if (menu->confirm_pressed || menu->cancel_pressed) {
            MenuSFX = MENUSFX_MENUSELECT;
            MenuASCancelFinished = 1;
        }
    }
}

void MenuUpdateLoad(MENU *menu) {
    if (memcard_cardchanged != 0 || saveload_cardtype != 2) {
        MenuEnterLoad(menu);
        return;
    }
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (menu->selected_row == 0) {
            memcard_slot = menu->selected_column - menu->first_column;
            if (saveload_slotused[memcard_slot] != 0) {
                NewMenu(MENU_LOAD_CONFIRM, 1, -1);
            } else {
                MenuSFX = MENUSFX_MENUNOENTRY;
            }
        } else if (menu->selected_row == menu->last_row) {
            NewMenu(MENU_LOAD_CANCEL, 1, -1);
        }
    } else if (menu->cancel_pressed != 0) {
        NewMenu(MENU_LOAD_CANCEL, 1, -1);
        MenuSFX = MENUSFX_MENUSELECT;
    }
}

void MenuUpdateLoadConfirm(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        if (menu->selected_row == 1) {
            BackupMenu();
        } else {
            MenuLoadStarted = 1;
            memcard_loadneeded = 1;
            memcard_loadstarted = 0;
            memcard_loadfailed = 0;
            memcard_loadcorrupt = 0;
            BackupMenuNoFn();
            NewMenu(MENU_LOADING, 0, -1);
        }
    } else if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenu();
    }
}

f32 memcard_loadmessage_delay = 0.0f;
f32 memcard_loadresult_delay = 0.0f;

void MenuUpdateLoading(MENU *menu) {
    if (memcard_loadneeded != 0) {
        memcard_loadmessage_delay = 1.5f;
    }
    if (memcard_loadneeded != 0 || memcard_loadstarted != 0 || memcard_loadmessage_delay > 0.0f) {
        memcard_loadresult_delay = 1.5f;
        if (memcard_loadneeded != 0 && (memcard_cardchanged != 0 || saveload_cardtype == -1)) {
            memcard_loadneeded = 0;
            memcard_loadfailed = 1;
        }
    }
    if (memcard_loadresult_delay <= 0.0f) {
        if (memcard_loadfailed == 0 && memcard_loadcorrupt == 0) {
            if (memcard_autosave != 0 && saveload_autosave != -1) {
                BackupMenuNoFn();
                BackupMenuNoFn();
                NewMenu(MENU_AUTO_SAVE_WARNING, 1, -1);
                return;
            }
            BackupMenuNoFn();
        }
        BackupMenu();
    }
}

void MenuUpdateSaveCancel(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        if (menu->selected_row == 0) {
            BackupMenuNoFn();
            BackupMenu();
            MenuSFX = MENUSFX_MENUSELECT;
        } else {
            BackupMenu();
            MenuSFX = MENUSFX_MENUSELECT;
        }
    } else if (menu->cancel_pressed != 0) {
        BackupMenu();
        MenuSFX = MENUSFX_MENUSELECT;
    }
}

void MenuUpdateLoadCancel(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        if (menu->selected_row == 0) {
            BackupMenuNoFn();
            BackupMenu();
            MenuSFX = MENUSFX_MENUSELECT;
        } else {
            BackupMenu();
            MenuSFX = MENUSFX_MENUSELECT;
        }
    } else if (menu->cancel_pressed != 0) {
        BackupMenu();
        MenuSFX = MENUSFX_MENUSELECT;
    }
}

void MenuUpdateFormatCancel(MENU *menu) {
    if (menu->confirm_pressed != 0) {
        if (menu->selected_row == 0) {
            MenuSFX = MENUSFX_MENUSELECT;
            if (MenuInfo[GameMenu[GameMenuLevel - 1].menu].id == MENU_SAVE) {
                BackupMenuNoFn();
                NewMenu(MENU_SAVE_CANCEL, 1, -1);
            } else if (MenuInfo[GameMenu[GameMenuLevel - 1].menu].id == MENU_LOAD) {
                BackupMenuNoFn();
                NewMenu(MENU_LOAD_CANCEL, 1, -1);
            } else {
                BackupMenu();
            }
        } else {
            MenuSFX = MENUSFX_MENUSELECT;
            BackupMenuNoFn();
            NewMenu(MENU_FORMAT_CONFIRM, 1, -1);
        }
    } else if (menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUSELECT;
        BackupMenuNoFn();
        NewMenu(MENU_FORMAT_CONFIRM, 1, -1);
    }
}

extern "C" void MenuUpdateCardWarning(MENU *menu) {
    menu->previous_item = MenuCardWarningState;
    switch (MenuCardWarningState) {
        case 7:
            if (menu->confirm_pressed != 0) {
                MenuCardWarningState = 3;
                MenuSFX = MENUSFX_MENUSELECT;
                BackupMenu();
                return;
            }
            break;
        case 0:
            if (saveload_status == 1) {
                Menu_InWarningFlow = 1;
                Menu_LastFlow = 1;
                MenuCardWarningState = 3;
                BackupMenu();
                return;
            }
            break;
    }
    if (MenuCardWarningState == 3) {
        BackupMenu();
        return;
    }
    if (MenuCardWarningState != menu->previous_item) {
        if (MenuCardWarningState != 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
        }
        menu->time_04 = 0.0f;
    }
}

extern "C" i32 MemCardGetSlot(void) {
    return memcard_slot;
}

/* 50b310/71. Existing menu globals and types, no new owner data. */
extern "C" i32 MenuInMemoryCard(void) {
    if (GameMenuLevel != -1 && MenuValidated != 0 && MenuInfo[GameMenu[GameMenuLevel].menu].id >= 1000 &&
        MenuInfo[GameMenu[GameMenuLevel].menu].id < 1031) {
        return 1;
    }
    return 0;
}

extern "C" i32 Menu_IsMemcardBusy() {
    if (MenuValidated != 0) {
        switch (MenuInfo[GameMenu[GameMenuLevel].menu].id) {
            case MENU_DO_NOT_REMOVE_CARD:
            case MENU_FORMATTING:
            case MENU_SAVING:
            case MENU_LOADING:
            case MENU_DELETING:
                return 1;
        }
    }
    return 0;
}

extern "C" i32 Menu_IsInLoadFlow() {
    return MenuValidated ? Menu_InLoadFlow : 0;
}

extern "C" i32 Menu_IsInWarningFlow() {
    return MenuValidated ? Menu_InWarningFlow : 0;
}

extern "C" i32 Menu_CurrentId() {
    return MenuInfo[GameMenu[GameMenuLevel].menu].id;
}

extern "C" {
    /* Alpha of the dark overlay shown while the memory card is busy. */
    i32 MenuMemCardFade = 0;

    /* Fades a black screen overlay in while the memory card is busy and out
     * afterwards. Nothing in the release calls it. */
    void MenuDrawMemCardFade(void) {
        if (Menu_IsMemcardBusy()) {
            if (MenuMemCardFade < 0x80) {
                MenuMemCardFade += 8;
            }
        } else if (MenuMemCardFade > 0) {
            MenuMemCardFade -= 5;
        }
        if (MenuMemCardFade > 0) {
            MenusPushCoordinateSystem(2);
            NuPrim2DBegin(0x10, 5, MenuFadeMtl);
            MenusPrimSetColour(MenuMemCardFade << 24);
            NuPrim2DAddXYZ(0.0f, 0.0f, 0.0f);
            MenusPrimSetColour(MenuMemCardFade << 24);
            NuPrim2DAddXYZ(1.0f, 1.0f, 0.0f);
            NuPrim2DEnd();
            MenusPopCoordinateSystem();
        }
    }
}
char *apitxt_YES = "Yes";
char *apitxt_NO = "No";
char *apitxt_GAMESAVING = "Saving game...";
char *apitxt_GAMELOADING = "Loading game...";
char *apitxt_FAILEDTOLOAD = "Failed to Load";
char *apitxt_FAILEDTOSAVE = "Failed to Save";
char *apitxt_FAILEDTOOVERWRITE = "Failed to Overwrite";
char *apitxt_NODATAAVAILABLE = "No %s data present";
char *apitxt_AUTOSAVE_ENABLED = "Autosave will now use Game %i";
char *apitxt_FORMATTINGCOMPLETE = "Formatting complete";
char *apitxt_DELETECOMPLETE = "Delete complete";
char *apitxt_NOTENOUGHSPACE = "Insufficient drive space to save games.";
char *apitxt_SELECT = "Select";
char *apitxt_BACK = "Back";
char *apiGameName = "Game";
char *apiGameTitle = "Game";
char *apiGameSubTitle = "";
char *apitxt_CONTINUE = "Continue";
char *apitxt_LOADGAME = "Load Game";
char *apitxt_DELETEGAME = "Delete Game";
char *apitxt_GAMESAVED = "Save Successful";
char *apitxt_GAMEOVERWRITTEN = "Overwrite Successful";
char *apitxt_GAMELOADED = "Game Loaded";
char *apitxt_DELETING = "Deleting data.";
char *apitxt_FORMATTING = "Formatting.";
char *apitxt_GAMEOVERWRITING = "Overwriting data.";
char *apitxt_CORRUPTLOAD = "Load Corrupted";
char *apitxt_CONFIRMLOAD = "Confirm Load?";
char *apitxt_CONFIRMSAVE = "Confirm Save?";
char *apitxt_CONFIRMOVERWRITE = "Are you sure you wish to overwrite this save file?";
char *apitxt_DOYOUWANTTOSTART = "Do you want to start the game?";
char *apitxt_DOYOUWANTTOABORT = "Do you want to cancel the Save?";
char *apitxt_DOYOUWANTTOABORTLOAD = "Do you want to cancel the Load?";
char *apitxt_DOYOUWANTTOABORTFORMAT = "Do you want to cancel the Format?";
char *apitxt_CONTINUEWITHOUTSAVING = "Continue without Saving";
char *apitxt_NEWSAVE = "New Save";
char *apitxt_CONTINUEWITHOUTLOADING = "Continue without loading";
char *apitxt_COMPLETED = "Completed";
char *apitxt_VIBRATION = "Vibration";
char *apitxt_CONFIRMDELETE = "Are you sure you wish to delete this save file?";
char *apitxt_PROGRESS = "Progress: %.1f%%";
char *apitxt_TIMEPLAYED = "Time Played: %s";
char *apitxt_CONTROLLERREMOVED =
    "The controller in controller port %i has been removed. Please insert a controller into controller port %i to "
    "continue with the game.";
char *apitxt_PCPRESSSTARTFE = "FE Start?";
char *apitxt_PCPRESSSTARTP1 = "P1 ?";
char *apitxt_PCPRESSSTARTP2 = "P2 ?";
char *apitxt_RETRY = "Retry";
char *apitxt_SLOT = "Profile";
char *apitxt_PRESENT = "Present";
char *apitxt_EMPTY = "Empty";
char *apitxt_NOSPACE = "No Space";
char *apitxt_OK = "OK";
char *apitxt_CANCEL = "Cancel";

void MenuLoadTechnicalStrings(char *filepath, char *language, VARIPTR *buffer, VARIPTR buffer_end) {
    NuStringTableSetBank(2);
    NuStringTableSetFormat(0);
    NuStringTableLoadCSV(filepath, buffer, buffer_end, "LABEL", language);
    apiGameName = NuStringTableGetByName("GAMENAME");
    apiGameTitle = NuStringTableGetByName("GAMETITLE");
    apiGameSubTitle = NuStringTableGetByName("GAMESUBTITLE");
    apitxt_YES = NuStringTableGetByName("GEN_YES");
    apitxt_NO = NuStringTableGetByName("GEN_NO");
    apitxt_OK = NuStringTableGetByName("GEN_OK");
    apitxt_CANCEL = NuStringTableGetByName("GEN_CANCEL");
    apitxt_RETRY = NuStringTableGetByName("GEN_RETRY");
    apitxt_SLOT = NuStringTableGetByName("GEN_SLOT");
    apitxt_PRESENT = NuStringTableGetByName("GEN_PRESENT");
    apitxt_EMPTY = NuStringTableGetByName("GEN_EMPTY");
    apitxt_NOSPACE = NuStringTableGetByName("GEN_NOSPACE");
    apitxt_CONTINUE = NuStringTableGetByName("GEN_CONTINUE");
    apitxt_SAVEGAME = NuStringTableGetByName("GEN_SAVEGAME");
    apitxt_LOADGAME = NuStringTableGetByName("GEN_LOADGAME");
    apitxt_DELETEGAME = NuStringTableGetByName("GEN_DELETEGAME");
    apitxt_GAMESAVED = NuStringTableGetByName("GEN_GAMESAVED");
    apitxt_GAMEOVERWRITTEN = NuStringTableGetByName("GEN_GAMEOVERWRITTEN");
    apitxt_GAMELOADED = NuStringTableGetByName("GEN_GAMELOADED");
    apitxt_DELETING = NuStringTableGetByName("GEN_DELETING");
    apitxt_FORMATTING = NuStringTableGetByName("GEN_FORMATTING");
    apitxt_CORRUPTLOAD = NuStringTableGetByName("GEN_CORRUPTLOAD");
    apitxt_CONFIRMLOAD = NuStringTableGetByName("GEN_CONFIRMLOAD");
    apitxt_CONFIRMSAVE = NuStringTableGetByName("GEN_CONFIRMSAVE");
    apitxt_CONFIRMOVERWRITE = NuStringTableGetByName("GEN_CONFIRMOVERWRITE");
    apitxt_DOYOUWANTTOSTART = NuStringTableGetByName("GEN_DOYOUWANTTOSTART");
    apitxt_DOYOUWANTTOABORT = NuStringTableGetByName("GEN_DOYOUWANTTOABORT");
    apitxt_DOYOUWANTTOABORTLOAD = NuStringTableGetByName("GEN_DOYOUWANTTOABORTLOAD");
    apitxt_DOYOUWANTTOABORTFORMAT = NuStringTableGetByName("GEN_DOYOUWANTTOABORTFORMAT");
    apitxt_CONTINUEWITHOUTSAVING = NuStringTableGetByName("GEN_CONTINUEWITHOUTSAVING");
    apitxt_GAMEOVERWRITING = NuStringTableGetByName("GEN_GAMEOVERWRITING");
    apitxt_NEWSAVE = NuStringTableGetByName("GEN_NEWSAVE");
    apitxt_CONTINUEWITHOUTLOADING = NuStringTableGetByName("GEN_CONTINUEWITHOUTLOADING");
    apitxt_COMPLETED = NuStringTableGetByName("GEN_COMPLETED");
    apitxt_SELECT = NuStringTableGetByName("GEN_SELECT");
    apitxt_BACK = NuStringTableGetByName("GEN_BACK");
    apitxt_VIBRATION = NuStringTableGetByName("GEN_VIBRATION");
    apitxt_CONFIRMDELETE = NuStringTableGetByName("GEN_CONFIRMDELETE");
    apitxt_PROGRESS = NuStringTableGetByName("GEN_PROGRESS");
    apitxt_TIMEPLAYED = NuStringTableGetByName("GEN_TIMEPLAYED");
    apitxt_PCPRESSSTARTFE = NuStringTableGetByName("GEN_PCPRESSSTARTFE");
    apitxt_PCPRESSSTARTP1 = NuStringTableGetByName("GEN_PCPRESSSTARTP1");
    apitxt_PCPRESSSTARTP2 = NuStringTableGetByName("GEN_PCPRESSSTARTP2");
    apitxt_GAMESAVING = NuStringTableGetByName("PC_GAMESAVING");
    apitxt_GAMELOADING = NuStringTableGetByName("PC_GAMELOADING");
    apitxt_FAILEDTOLOAD = NuStringTableGetByName("PC_FAILEDTOLOAD");
    apitxt_FAILEDTOSAVE = NuStringTableGetByName("PC_FAILEDTOSAVE");
    apitxt_FAILEDTOOVERWRITE = NuStringTableGetByName("PC_FAILEDTOOVERWRITE");
    apitxt_NODATAAVAILABLE = NuStringTableGetByName("PC_NODATAAVAILABLE");
    apitxt_AUTOSAVE_ENABLED = NuStringTableGetByName("PC_AUTOSAVE_ENABLED");
    apitxt_FORMATTINGCOMPLETE = NuStringTableGetByName("PC_FORMATTINGCOMPLETE");
    apitxt_DELETECOMPLETE = NuStringTableGetByName("PC_DELETECOMPLETE");
    apitxt_NOTENOUGHSPACE = NuStringTableGetByName("PC_NOTENOUGHSPACE");
    apitxt_CONTROLLERREMOVED = NuStringTableGetByName("PC_CONTROLLERREMOVED");
}

void *memcard_savedata = NULL;
void *memcard_savedatabuffer = NULL;
i32 memcard_savedatasize = 0;
SAVE_HASH_FN memcard_hashfn = NULL;
void (*memcard_drawasiconfn)(void) = 0;

void SaveSystemInitialise(i32 slots, SAVE_HASH_FN hashfn, void *save, i32 save_size, i32 autosave, SAVE_VOID_FN draw_save_icon) {
    SAVESLOTS = slots;
    if (slots >= 6) {
        SAVESLOTS = 6;
    }
    memcard_hashfn = hashfn;
    memcard_savedata = save;
    memcard_savedatasize = save_size;
    memcard_savedatabuffer = NuAlloc(save_size + 4, "d:\\projects\\game_saga\\gameapi.saga\\gui\\gamemenu\\apisave.c", 0x6d);
    memcard_autosave = autosave;
    memcard_drawasiconfn = draw_save_icon;
}

void *memcard_headerdata = NULL;
void *memcard_headerdatabuffer = NULL;
i32 memcard_headerdatasize = 0;

void SaveSystemInitialiseEx(i32 slots, SAVE_HASH_FN hashfn, void *save, i32 save_size, void *header, i32 header_size, i32 autosave,
                            SAVE_VOID_FN draw_save_icon) {
    memcard_hashfn = hashfn;
    memcard_savedata = save;
    memcard_savedatasize = save_size;
    memcard_savedatabuffer = NuAlloc(save_size + 4, "d:\\projects\\game_saga\\gameapi.saga\\gui\\gamemenu\\apisave.c", 0x7f);
    memcard_autosave = autosave;
    memcard_drawasiconfn = draw_save_icon;
    memcard_headerdata = header;
    memcard_headerdatasize = header_size;
    memcard_headerdatabuffer = NuAlloc(header_size + 4, "d:\\projects\\game_saga\\gameapi.saga\\gui\\gamemenu\\apisave.c", 0x84);
    if (header != NULL) {
        SAVESLOTS = slots < 5 ? slots : 5;
    } else {
        SAVESLOTS = slots < 6 ? slots : 6;
    }
}

SAVE_VOID_FN savesuccessfn = NULL;

void SetSaveSuccessFn(SAVE_VOID_FN callback) {
    savesuccessfn = callback;
}

i32 ChecksumSaveData(void *buffer, i32 size) {
    u32 sum;
    i32 i;
    sum = 0x5c0999;
    for (i = 0; i < size / 4; i++) {
        sum += ((u32 *)buffer)[i];
    }
    return (i32)sum;
}

i32 memcard_autosaveneeded = 0;
i32 memcard_autosavestarted = 0;
f32 memcard_autosavepredelay = 0;
f32 memcard_autosavepostdelay = 0;
i32 memcard_autosaveinprogress = 0;
i32 memcard_justformatted = 0;
f32 memcard_createmessage_delay = 0.0f;
f32 memcard_createresult_delay = 0.0f;
f32 memcard_message_delay = 0.0f;
f32 memcard_result_delay = 0.0f;
i32 saveload_error = 0;
i32 saveload_autosavedisabled = 0;

void UpdateSaveSlots(void) {
    const f32 elapsed = NuTimeGetFrameTime();
    if (memcard_autosavecanceldelay > 0.0f) {
        memcard_autosavecanceldelay -= elapsed;
    }
    if (memcard_loadmessage_delay > 0.0f) {
        memcard_loadmessage_delay -= elapsed;
    } else if (memcard_loadresult_delay > 0.0f) {
        memcard_loadresult_delay -= elapsed;
    }
    if (memcard_savemessage_delay > 0.0f) {
        memcard_savemessage_delay -= elapsed;
    } else if (memcard_saveresult_delay > 0.0f) {
        memcard_saveresult_delay -= elapsed;
    }
    if (memcard_autosavepredelay > 0.0f) {
        memcard_autosavepredelay -= elapsed;
    } else if (memcard_autosavepostdelay > 0.0f) {
        memcard_autosavepostdelay -= elapsed;
        if (memcard_autosavepostdelay <= 0.0f) {
            memcard_autosaveinprogress = 0;
        }
    }
    if (memcard_formatmessage_delay > 0.0f) {
        memcard_formatmessage_delay -= elapsed;
    } else if (memcard_formatresult_delay > 0.0f) {
        memcard_formatresult_delay -= elapsed;
    }
    if (memcard_createmessage_delay > 0.0f) {
        memcard_createmessage_delay -= elapsed;
    } else if (memcard_createresult_delay > 0.0f) {
        memcard_createresult_delay -= elapsed;
    }
    if (memcard_deletemessage_delay > 0.0f) {
        memcard_deletemessage_delay -= elapsed;
    } else if (memcard_deleteresult_delay > 0.0f) {
        memcard_deleteresult_delay -= elapsed;
    }
    if (memcard_message_delay > 0.0f) {
        memcard_message_delay -= elapsed;
    } else if (memcard_result_delay > 0.0f) {
        memcard_result_delay -= elapsed;
    }

    if (saveload_cardtype != 2) {
        memcard_justformatted = 0;
    }
    if (saveload_autosavedisabled != 0) {
        if (memcard_autosavestarted != 0) {
            memcard_savefailed = 1;
            memcard_saveresult_delay = 1.5f;
        }
        memcard_autosavestarted = 0;
        memcard_autosavepredelay = 0.0f;
        memcard_autosaveinprogress = 0;
        memcard_autosaveneeded = 0;
        memcard_autosavepostdelay = 0.0f;
        memcard_autosavedisabled = 1;
        saveload_autosavedisabled = 0;
    }

    if (memcard_loadneeded != 0) {
        if (saveload_status == 1 && saveload_cardtype == 2 && saveload_cardformatted != 0) {
            memcard_loadmessage_delay = 1.5f;
            if (memcard_savedatabuffer == NULL || memcard_savedatasize == 0) {
                memcard_loadneeded = 0;
                memcard_loadstarted = 0;
                memcard_loadfailed = 1;
                return;
            }
            saveloadASLoad(memcard_slot, memcard_savedatabuffer, memcard_savedatasize + 4);
            memcard_loadneeded = 0;
            memcard_loadcorrupt = 0;
            if (saveload_status != 1) {
                memcard_loadstarted = 1;
                memcard_loadfailed = 0;
            } else {
                memcard_loadstarted = 0;
                memcard_loadfailed = 1;
            }
        }
    } else if (memcard_loadstarted != 0 && saveload_status == 1) {
        memcard_loadstarted = 0;
        if (saveload_error == 0) {
            if (ChecksumSaveData(memcard_savedatabuffer, memcard_savedatasize) !=
                *(i32 *)((char *)memcard_savedatabuffer + memcard_savedatasize)) {
                memcard_loadcorrupt = 1;
                saveload_autosave = -1;
            } else {
                PCByteCopy(memcard_savedata, memcard_savedatabuffer, memcard_savedatasize);
                MenuLoadOccurred = 1;
            }
        } else {
            memcard_loadfailed = 1;
            saveload_autosave = -1;
        }
    }

    if (memcard_autosaveneeded != 0) {
        memcard_autosaveneeded = 0;
        if (saveload_autosave != -1) {
            memcard_autosavepredelay = 1.0f;
            memcard_autosavestarted = 1;
            memcard_autosaveinprogress = 1;
        }
    } else if (memcard_autosavestarted != 0 && memcard_saveneeded == 0 && memcard_savestarted == 0 && memcard_autosavepredelay <= 0.0f) {
        memcard_slot = saveload_autosave;
        memcard_saveneeded = 1;
    }

    if (memcard_saveneeded != 0) {
        if (saveload_status == 1 && saveload_cardtype == 2 && saveload_cardformatted != 0) {
            i32 save_hash;
            memcard_savemessage_delay = 1.5f;
            if (memcard_savedatabuffer == NULL || memcard_savedatasize == 0) {
                memcard_saveneeded = 0;
                memcard_savestarted = 0;
                memcard_savefailed = 1;
                return;
            }
            save_hash = memcard_hashfn != NULL ? memcard_hashfn() : -1;
            if (memcard_slot != -1) {
                PCByteCopy(memcard_savedatabuffer, memcard_savedata, memcard_savedatasize);
                *(i32 *)((char *)memcard_savedatabuffer + memcard_savedatasize) =
                    ChecksumSaveData(memcard_savedatabuffer, memcard_savedatasize);
                saveloadASSave(memcard_slot, memcard_savedatabuffer, memcard_savedatasize + 4, (u32)save_hash);
            }
            memcard_saveneeded = 0;
            memcard_savestarted = 1;
            memcard_savefailed = 0;
            memcard_justformatted = 0;
        }
    } else if (memcard_savestarted != 0 && saveload_status == 1) {
        memcard_savestarted = 0;
        if (saveload_error == 0) {
            MenuSaveOccurred = 1;
            if (savesuccessfn != NULL) {
                savesuccessfn();
            }
            if (memcard_autosavestarted != 0) {
                memcard_autosavestarted = 0;
                memcard_autosavepostdelay = 1.0f;
            }
        } else {
            memcard_savefailed = 1;
            if (saveload_autosave != -1) {
                saveload_autosavedisabled = 1;
                memcard_saveresult_delay = 1.5f;
                saveload_autosave = -1;
                memcard_autosavedisabled = 1;
            }
            memcard_autosavestarted = 0;
            memcard_autosaveinprogress = 0;
        }
    }

    if (memcard_deleteneeded != 0) {
        if (saveload_status == 1 && saveload_cardtype == 2 && saveload_cardformatted != 0) {
            saveloadASDelete(memcard_slot);
            memcard_deleteneeded = 0;
            memcard_deletemessage_delay = 1.5f;
            memcard_deletestarted = 1;
            memcard_deletefailed = 0;
        }
    } else if (memcard_deletestarted != 0 && saveload_status == 1) {
        memcard_deletestarted = 0;
        memcard_deletefailed = 0;
        if (saveload_error != 0) {
            memcard_deletefailed = 1;
        }
    }
    if (memcard_formatme != 0) {
        if (saveload_status == 1 && saveload_cardtype == 2) {
            saveloadASFormat();
            memcard_formatme = 0;
            memcard_formatmessage_delay = 1.5f;
            memcard_formatting = 1;
            memcard_formatfailed = 0;
            memcard_justformatted = 0;
        }
    } else if (memcard_formatting != 0 && saveload_status == 3) {
        memcard_formatting = 0;
        if (saveload_error == 0) {
            memcard_justformatted = 1;
        } else {
            memcard_formatfailed = 1;
        }
    }
}

i32 TriggerAutoSave(void) {
    if (memcard_autosave != 0 && memcard_autosaveenabled != 0 && saveload_autosave != -1) {
        memcard_autosaveneeded = 1;
        MenuSaveOccurred = 0;
        return 1;
    }
    return 0;
}

extern "C" i32 SaveloadGetStatus(void) {
    return saveload_status;
}

void Text3DStringEncodeFont(u8 *source, u16 *destination, VUFNT *font) {
    u16 character;
    while (*source != 0) {
        source = NuUnicodeCharFromUTF8(&character, source);
        if (character == '~') {
            if (*source == 0) {
                break;
            }
            source = NuUnicodeCharFromUTF8(&character, source);
            continue;
        }
        *destination = NuQFntEncodeUnicodeChar(font, character);
        if (*destination == 0xffff) {
            NuUnicodeCharFromUTF8(&character, (u8 *)"\xe2\x96\xa1");
            *destination = NuQFntEncodeUnicodeChar(font, character);
            if (*destination == 0xffff) {
                *destination = NuQFntEncodeUnicodeChar(font, '?');
            }
        }
        ++destination;
    }
    *destination = 0;
}

VUFNT *QFont2D = 0;
VUFNT *SmartTextFont = 0;

// noinline: the original calls Text3DStringEncode out of line from IntroText_Draw; VC8 would inline it there.
__declspec(noinline) void Text3DStringEncode(char *source, u16 *destination) {
    Text3DStringEncodeFont((u8 *)source, destination, SmartTextFont != NULL ? SmartTextFont : QFont2D);
}

void (*buttonmapfn)(char *, char *) = 0;

void TextRegisterButtonMapFn(void (*fn)(char *, char *)) {
    if (fn != NULL) {
        buttonmapfn = fn;
    }
}

extern "C" const char *TextInputButtonGlyphs[12] = {
    "\xd4\xb1", "\xd4\xb2", "\xd4\xb3", "\xd4\xb4", "\xd4\xb5", "\xd4\xb6", "\xd4\xbd", "\xd4\xbe", "", "", "\xd4\xb8", "\xd4\xb7",
};
extern "C" const char *TextInputAxisGlyphs[4] = {
    "\xd4\xbc",
    "\xd4\xba",
    "\xd4\xb9",
    "\xd4\xbb",
};

void TextInputGlyphName(i32 code, char *destination) {
    if ((u32)code <= 11U) {
        NuStrCpy(destination, (char *)TextInputButtonGlyphs[code]);
    } else if ((u32)code - 128U <= 3U) {
        NuStrCpy(destination, (char *)TextInputAxisGlyphs[(u32)code - 128U]);
    } else {
        *destination = 0;
    }
}

void TextDecodeCodeword(char *source, char *destination) {
    i32 button;
    i32 device;
    if (buttonmapfn != NULL) {
        buttonmapfn(source, source);
    }
    if (NuStrICmp(source, "accept") == 0) {
        NuStrCpy(source, "cross");
    }
    if (NuStrICmp(source, "back") == 0) {
        NuStrCpy(source, "circle");
    }
    if (NuStrICmp(source, "wii") == 0) {
        NuStrCpy(source, "logo");
    }
    if (NuStrICmp(source, "playstation") == 0) {
        NuStrCpy(source, "logo");
    }
    NuStrCpy(destination, source);
    button = -1;
    if (NuStrICmp(source, "padup") == 0) {
        button = PC_COMMAND_PAD_UP;
    } else if (NuStrICmp(source, "padright") == 0) {
        button = PC_COMMAND_PAD_RIGHT;
    } else if (NuStrICmp(source, "paddown") == 0) {
        button = PC_COMMAND_PAD_DOWN;
    } else if (NuStrICmp(source, "padleft") == 0) {
        button = PC_COMMAND_PAD_LEFT;
    } else if (NuStrICmp(source, "triangle") == 0) {
        button = PC_COMMAND_TRIANGLE;
    } else if (NuStrICmp(source, "circle") == 0) {
        button = PC_COMMAND_CIRCLE;
    } else if (NuStrICmp(source, "cross") == 0) {
        button = PC_COMMAND_CROSS;
    } else if (NuStrICmp(source, "square") == 0) {
        button = PC_COMMAND_SQUARE;
    } else if (NuStrICmp(source, "l1") == 0) {
        button = PC_COMMAND_L1;
    } else if (NuStrICmp(source, "l2") == 0) {
        button = PC_COMMAND_L2;
    } else if (NuStrICmp(source, "r1") == 0) {
        button = PC_COMMAND_R1;
    } else if (NuStrICmp(source, "r2") == 0) {
        button = PC_COMMAND_R2;
    } else if (NuStrICmp(source, "l3") == 0) {
        button = PC_COMMAND_L3;
    } else if (NuStrICmp(source, "r3") == 0) {
        button = PC_COMMAND_R3;
    } else if (NuStrICmp(source, "select") == 0) {
        button = PC_COMMAND_SELECT;
    } else if (NuStrICmp(source, "start") == 0) {
        button = PC_COMMAND_START;
    }
    device = PcInput_GetCtrlStringPlayer();
    if (device < 0) {
        device = 0;
    }
    PcInput_GetCtrlInputString(device, button, destination);
}

void TextDecode(char *text, u8 *destination) {
    u8 *source = (u8 *)text;
    i32 input = 0;
    i32 output = 0;
    char codeword[512];
    char decoded[512];
    if (source[0] != 0) {
        do {
            if (source[input] == '[' && source[input + 1] == '[') {
                i32 length = 0;
                i32 index;
                input += 2;
                while (source[input] != 0 && source[input] != ']') {
                    codeword[length++] = source[input++];
                }
                while (source[input] == ']') {
                    ++input;
                }
                codeword[length] = 0;
                TextDecodeCodeword(codeword, decoded);
                for (index = 0; decoded[index] != 0; ++index) {
                    destination[output++] = decoded[index];
                }
            } else if (source[input] == 0xe3 && source[input + 1] == 0x80 && source[input + 2] == 0x80) {
                destination[output++] = ' ';
                input += 3;
            } else if (source[input] == 0xe3 && source[input + 1] == 0x80 && source[input + 2] == 0x81) {
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = ' ';
            } else if (source[input] == 0xe3 && source[input + 1] == 0x80 && source[input + 2] == 0x82) {
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = ' ';
            } else if (source[input] == 0xe3 && source[input + 1] == 0x80 && source[input + 2] == 0x85) {
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = ' ';
            } else if (source[input] == 0xef && source[input + 1] == 0xbc && source[input + 2] == 0x81) {
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = ' ';
            } else if (source[input] == 0xef && source[input + 1] == 0xbc && source[input + 2] == 0x9f) {
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = source[input++];
                destination[output++] = ' ';
            } else {
                destination[output++] = source[input++];
                while (source[input] >= 0x80 && source[input] <= 0xbf) {
                    destination[output++] = source[input++];
                }
            }
        } while (output < 480 && source[input] != 0);
    }
    destination[output] = 0;
}

VUFNT *QFont2DButtons = 0;
f32 QFONTSCALEX = 1.0f;
f32 QFONTSCALEY = 1.0f;

f32 TextPrintSubstring(u8 *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, i32 colour, i32 button_font) {
    VUFNT *base_font = SmartTextFont != NULL ? SmartTextFont : QFont2D;
    VUFNT *font;
    f32 relative_scale;
    u16 encoded[512];
    f32 width;
    if (button_font != 0 && QFont2DButtons != NULL) {
        relative_scale = base_font->height / QFont2DButtons->height;
        font = QFont2DButtons;
    } else {
        font = SmartTextFont != NULL ? SmartTextFont : QFont2D;
        relative_scale = 1.0f;
    }
    NuQFntSet(font);
    NuQFntSetScale(font, x_scale * QFONTSCALEX * relative_scale, y_scale * QFONTSCALEY * relative_scale);
    Text3DStringEncodeFont(text, encoded, font);
    width = NuQFntPrintLenW(font, encoded);
    NuQFntSetColour(font, colour);
    NuQFntMove(font, x, y, z);
    NuQFntPrintW(font, encoded);
    return width;
}

i32 smarttext_fwn = 0;

void SmartTextSetFWNMode(i32 mode) {
    smarttext_fwn = mode;
}

i32 followon_line = 0;
i32 TextPrintMode = 0;
f32 text3d_height = 0;
f32 text3d_width = 0;
u8 text_preset_r[10] = {128, 128, 0, 0, 0, 128, 128, 128, 0, 0};
u8 text_preset_g[10] = {128, 0, 128, 0, 128, 0, 128, 96, 0, 0};
u8 text_preset_b[10] = {128, 0, 0, 128, 128, 128, 0, 0, 0, 0};
u32 current_rgba = 0x80808080;
i32 apitext_half_rgb = 1;

void Text3DEx(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment, u8 red, u8 green, u8 blue, u8 alpha) {
    u8 alpha_byte = alpha;
    f32 width;
    f32 height;
    f32 baseline_y;
    f32 button_scale;
    f32 button_width;
    f32 normal_width;
    f32 ratio;
    f32 scaled_width;
    f64 half_height;
    u8 decoded[512];
    u8 normal[512];
    u8 buttons[512];
    u8 fragment[512];
    u16 encoded[512];
    u16 button_encoded[512];
    u16 normal_encoded[512];
    i32 button_count;
    i32 normal_count;
    i32 count;
    i32 button_font;
    i32 preset;
    i32 i;
    u32 colour;

    text3d_width = 0.0f;
    text3d_height = 0.0f;
    if ((SmartTextFont != NULL ? SmartTextFont : QFont2D) == NULL || text == NULL || text[0] == '\0' || MenuStopDraw != 0) {
        return;
    }
    x_scale = TextScaleX * x_scale;
    y_scale = TextScaleY * y_scale;
    if (x < -2.0f || x > 2.0f || y < -2.0f || y > 2.0f) {
        return;
    }
    if (TextPrintMode != 0) {
        NuQFntPushPrintMode(NUQFNT_MODE_FIXED_DIGITS | NUQFNT_MODE_2D);
    } else {
        NuQFntPushPrintMode(NUQFNT_MODE_2D);
    }

    TextDecode(text, decoded);
    if (followon_line == 0) {
        NuQFntSet((SmartTextFont != NULL ? SmartTextFont : QFont2D));
    }

    Text3DStringEncode((char *)decoded, encoded);
    NuQFntSetScale((SmartTextFont != NULL ? SmartTextFont : QFont2D), QFONTSCALEX * x_scale, QFONTSCALEY * y_scale);
    text3d_width = NuQFntPrintLenW((SmartTextFont != NULL ? SmartTextFont : QFont2D), encoded);
    width = text3d_width;
    text3d_height = NuQFntHeight((SmartTextFont != NULL ? SmartTextFont : QFont2D));
    height = text3d_height;
    half_height = (f64)height * 0.5;
    baseline_y = NuQFntBaseline((SmartTextFont != NULL ? SmartTextFont : QFont2D)) + y;
    y = (f32)((f64)baseline_y - half_height);
    if (y_scale != x_scale) {
        button_width = 0.0f;
        normal_width = 0.0f;
        switch (smarttext_fwn) {
            case 1:
                button_scale = x_scale;
                break;
            case 2:
                button_scale = y_scale;
                break;
            default:
                button_scale = (f32)((f64)(y_scale + x_scale) * 0.5);
                break;
        }

        button_count = 0;
        normal_count = 0;
        for (i = 0; decoded[i] != 0;) {
            if (decoded[i] == 0xd4 && decoded[i + 1] >= 0xb1 && decoded[i + 1] <= 0xbf) {
                buttons[button_count++] = decoded[i++];
                while (decoded[i] >= 0x80 && decoded[i] <= 0xbf) {
                    buttons[button_count++] = decoded[i++];
                }
            } else {
                normal[normal_count++] = decoded[i++];
                while (decoded[i] >= 0x80 && decoded[i] <= 0xbf) {
                    normal[normal_count++] = decoded[i++];
                }
            }
        }
        buttons[button_count] = 0;
        normal[normal_count] = 0;

        if (button_count != 0) {
            Text3DStringEncode((char *)buttons, button_encoded);
            NuQFntSetScale((SmartTextFont != NULL ? SmartTextFont : QFont2D), QFONTSCALEX * button_scale, QFONTSCALEY * button_scale);
            button_width = NuQFntPrintLenW((SmartTextFont != NULL ? SmartTextFont : QFont2D), button_encoded);
        }
        if (normal_count != 0) {
            Text3DStringEncode((char *)normal, normal_encoded);
            NuQFntSetScale((SmartTextFont != NULL ? SmartTextFont : QFont2D), QFONTSCALEX * x_scale, QFONTSCALEY * y_scale);
            normal_width = NuQFntPrintLenW((SmartTextFont != NULL ? SmartTextFont : QFont2D), normal_encoded);
        }
        if (normal_width != 0.0f) {
            ratio = (width - button_width) / normal_width;
            x_scale = ratio * x_scale;
        } else {
            button_scale = x_scale;
            if (smarttext_fwn == 2) {
                button_scale = y_scale;
                scaled_width = y_scale * width;
                width = scaled_width / x_scale;
            }
        }
    } else {
        button_scale = x_scale;
    }
    if ((alignment & TEXT_ALIGN_BOTTOM) != 0) {
        y = (f32)((f64)y - half_height);
    } else if ((alignment & TEXT_ALIGN_TOP) != 0) {
        y = (f32)((f64)y + half_height);
    }
    if ((alignment & TEXT_ALIGN_LEFT) == 0) {
        if ((alignment & TEXT_ALIGN_RIGHT) != 0) {
            x -= width;
        } else {
            x -= width * 0.5;
        }
    }
    if (followon_line == 0) {
        if (apitext_half_rgb != 0) {
            current_rgba = ((u32)alpha_byte << 24) | ((u32)(blue >> 1) << 16) | ((u32)(green >> 1) << 8) | (red >> 1);
        } else {
            current_rgba = ((u32)alpha_byte << 24) | ((u32)blue << 16) | ((u32)green << 8) | red;
        }
    }

    count = 0;
    button_font = 0;
    i = 0;
    if (decoded[0] != 0) {
        do {
            if (decoded[i] == '~') {
                ++i;
                if (decoded[i] == '~') {
                    if (apitext_half_rgb != 0) {
                        colour = ((u32)alpha_byte << 24) | ((u32)(blue >> 1) << 16) | ((u32)(green >> 1) << 8) | (red >> 1);
                    } else {
                        colour = ((u32)alpha_byte << 24) | ((u32)blue << 16) | ((u32)green << 8) | red;
                    }
                } else {
                    preset = decoded[i] - '0';
                    if (red == 0 && green == 0 && blue == 0) {
                        colour = (u32)alpha_byte << 24;
                    } else if (apitext_half_rgb != 0) {
                        colour = ((u32)alpha_byte << 24) | ((u32)text_preset_b[preset] << 16) | ((u32)text_preset_g[preset] << 8) |
                                 text_preset_r[preset];
                    } else {
                        colour = (u8)(2 * text_preset_r[preset]) | ((u32)((2 * text_preset_b[preset]) & 255) << 16) |
                                 ((u32)((2 * text_preset_g[preset]) & 255) << 8) | ((u32)alpha_byte << 24);
                    }
                }
                ++i;
                if (count > 0 && colour != current_rgba) {
                    fragment[count] = 0;
                    if (button_font != 0) {
                        x += TextPrintSubstring(fragment, x, y, z, button_scale, button_scale, current_rgba, button_font);
                    } else {
                        x += TextPrintSubstring(fragment, x, y, z, x_scale, y_scale, current_rgba, button_font);
                    }
                    count = 0;
                }
                current_rgba = colour;
            } else if (decoded[i] == 0xd4 && decoded[i + 1] >= 0xb1 && decoded[i + 1] <= 0xbf) {
                if (count > 0 && button_font == 0) {
                    fragment[count] = 0;
                    x += TextPrintSubstring(fragment, x, y, z, x_scale, y_scale, current_rgba, button_font);
                    count = 0;
                }
                fragment[count++] = decoded[i++];
                button_font = 1;
                while (decoded[i] >= 0x80 && decoded[i] <= 0xbf) {
                    fragment[count++] = decoded[i++];
                }
            } else {
                if (count > 0 && button_font != 0) {
                    fragment[count] = 0;
                    x += TextPrintSubstring(fragment, x, y, z, button_scale, button_scale, current_rgba, button_font);
                    count = 0;
                }
                fragment[count++] = decoded[i++];
                button_font = 0;
                while (decoded[i] >= 0x80 && decoded[i] <= 0xbf) {
                    fragment[count++] = decoded[i++];
                }
            }
        } while (count <= 508 && decoded[i] != 0);
        if (count > 0) {
            fragment[count] = 0;
            if (button_font != 0) {
                TextPrintSubstring(fragment, x, y, z, button_scale, button_scale, current_rgba, button_font);
            } else {
                TextPrintSubstring(fragment, x, y, z, x_scale, y_scale, current_rgba, button_font);
            }
        }
    }
    NuQFntPopPrintMode();
}

VUFNT *QFont2DLower = 0;

void Text3DEx2(char *text, f32 x, f32 y, f32 z, f32 sx, f32 sy, f32 sz, u32 alignment, u32 red, u32 green, u32 blue, u8 alpha) {
    if (QFont2DLower != NULL) {
        VUFNT *saved_font = SmartTextFont;
        SmartTextFont = QFont2DLower;
        Text3DEx(text, x, y, z, sx, sy, sz, alignment, red, green, blue, alpha);
        SmartTextFont = saved_font;
    }
}

extern "C" {

    i32 FindNextBreak(u8 *text, i32 index) {
        i32 japanese = 0;
        i32 ascii_start;
        i32 length;
        i32 count;
        if (NuLanguageGet() == NULANGUAGE_JAPANESE) {
            japanese = 1;
        }
        count = 0;
        length = NuStrLen((char *)text);
        while (text[index] >= 0x80 && text[index] <= 0xbf) {
            --index;
        }
        if (((text[index] == ' ' && text[index + 1] != '!' && text[index + 1] != '?' && text[index + 1] != ';' && text[index + 1] != ':') ||
             (text[index] == '.' && text[index + 1] != '.') || text[index] == '-' || text[index] == ',')) {
            return index;
        }
        ascii_start = text[index] <= 0x7f;
        for (;;) {
            ++count;
            if ((japanese && count > 10 && !ascii_start) || count > 20) {
                break;
            }
            if (++index >= length) {
                return length;
            }
            while (text[index] >= 0x80 && text[index] <= 0xbf) {
                ++index;
            }
            if (((text[index] == ' ' && text[index + 1] != '!' && text[index + 1] != '?' && text[index + 1] != ';' &&
                  text[index + 1] != ':') ||
                 (text[index] == '.' && text[index + 1] != '.') || text[index] == '-' || text[index] == ',')) {
                return index;
            }
            if (japanese && ((text[index] <= 0x7f && !ascii_start) || (text[index] >= 0x80 && ascii_start))) {
                break;
            }
        }
        if (text[index] == '~') {
            if (index > 0 && text[index - 1] == '~') {
                --index;
            }
        } else if (index > 0 && text[index - 1] == '~' && index > 1 && text[index - 2] != '~') {
            --index;
        }
        return index;
    }

    i32 FindNearestBreak(u8 *text, i32 index) {
        i32 japanese = 0;
        i32 ascii_start;
        i32 length;
        i32 forward;
        i32 backward;
        i32 count;
        if (NuLanguageGet() == NULANGUAGE_JAPANESE) {
            japanese = 1;
        }
        while (text[index] >= 0x80 && text[index] <= 0xbf) {
            --index;
        }
        if (((text[index] == ' ' && text[index + 1] != '!' && text[index + 1] != '?' && text[index + 1] != ';' && text[index + 1] != ':') ||
             (text[index] == '.' && text[index + 1] != '.') || text[index] == '-' || text[index] == ',')) {
            return index;
        }
        ascii_start = text[index] <= 0x7f;
        forward = backward = index;
        count = 0;
        length = NuStrLen((char *)text);
        for (;;) {
            ++count;
            if ((japanese && count > 10 && !ascii_start) || count > 20) {
                break;
            }
            if (forward < length - 1) {
                ++forward;
            }
            while (text[forward] >= 0x80 && text[forward] <= 0xbf) {
                ++forward;
            }
            if (backward > 0) {
                --backward;
            }
            while (text[backward] >= 0x80 && text[backward] <= 0xbf) {
                --backward;
            }
            if (((text[forward] == ' ' && text[forward + 1] != '!' && text[forward + 1] != '?' && text[forward + 1] != ';' &&
                  text[forward + 1] != ':') ||
                 (text[forward] == '.' && text[forward + 1] != '.') || text[forward] == '-' || text[forward] == ',')) {
                return forward;
            }
            if (japanese && ((text[forward] <= 0x7f && !ascii_start) || (text[forward] >= 0x80 && ascii_start))) {
                if (text[forward] == '~') {
                    if (forward > 0 && text[forward - 1] == '~') {
                        --forward;
                    }
                } else if (forward > 0 && text[forward - 1] == '~' && forward > 1 && text[forward - 2] != '~') {
                    --forward;
                }
                return forward;
            }
            if (((text[backward] == ' ' && text[backward + 1] != '!' && text[backward + 1] != '?' && text[backward + 1] != ';' &&
                  text[backward + 1] != ':') ||
                 (text[backward] == '.' && text[backward + 1] != '.') || text[backward] == '-' || text[backward] == ',')) {
                return backward;
            }
            if (japanese && ((text[backward] <= 0x7f && !ascii_start) || (text[backward] >= 0x80 && ascii_start))) {
                ++backward;
                if (text[backward] == '~') {
                    if (backward > 0 && text[backward - 1] == '~') {
                        --backward;
                    }
                } else if (backward > 0 && text[backward - 1] == '~' && backward > 1 && text[backward - 2] != '~') {
                    --backward;
                }
                return backward;
            }
        }
        if (text[index] == '~') {
            if (index > 0 && text[index - 1] == '~') {
                --index;
            }
        } else if (index > 0 && text[index - 1] == '~' && index > 1 && text[index - 2] != '~') {
            --index;
        }
        return index;
    }
}
i32 MessageMtlInit = 0;
extern "C" {
    NUMTL *MessageMtl = 0;

    void MessageBoxInitMtl(void) {
        MessageMtl = NuMtlCreate(1);
        MessageMtl->attribs.z_mode = NUMTL_ZMODE_OFF;
        MessageMtl->attribs.filter_mode = 1;
        MessageMtl->attribs.lighting = NUMTL_LIGHTING_NONE;
        MessageMtl->attribs.u_address = NUMTL_ADDRESS_CLAMP;
        MessageMtl->attribs.v_address = NUMTL_ADDRESS_CLAMP;
        MessageMtl->attribs.vertex_colour = 1;
        MessageMtl->attribs.alpha_mode = NUMTL_ALPHA_BLEND;
        NuMtlUpdate(MessageMtl);
        MessageMtlInit = 1;
    }

    void DrawMessageBox(i32 unused, f32 x, f32 y, f32 width, f32 height) {
        i32 box_width;
        i32 box_height;
        i32 screen_x;
        i32 screen_y;
        u32 colours[4];
        if (MessageMtlInit == 0) {
            MessageBoxInitMtl();
        }
        if (!NuRndrBeginScene(1)) {
            return;
        }
        box_width = (width * 10240.0 * NuQFntHalfScale);
        box_height = (height * 3584.0 * NuQFntHalfScale);
        screen_x = -(box_width / 2) - (i32)((x + 1.0f) * NuQFntHalfScale * -10240.0);
        screen_y = -(box_height / 2) - (i32)((y + 1.0f) * NuQFntHalfScale * -3584.0);
        NuRndrRect2di(screen_x, screen_y, box_width, box_height, 0x80808080, MessageMtl);
        colours[0] = 0x808080;
        colours[1] = 0x80808080;
        colours[2] = 0x808080;
        colours[3] = 0x80808080;
        NuRndrGradRectUV2di(screen_x - 30, screen_y, 60, box_height, 0.0f, 0.0f, 0.0f, 1.0f, colours, MessageMtl);
        colours[0] = 0x80808080;
        colours[1] = 0x808080;
        colours[2] = 0x80808080;
        colours[3] = 0x808080;
        NuRndrGradRectUV2di(screen_x + box_width, screen_y, 60, box_height, 1.0f, 0.0f, 1.0f, 1.0f, colours, MessageMtl);
        colours[0] = 0x808080;
        colours[1] = 0x808080;
        colours[2] = 0x80808080;
        colours[3] = 0x80808080;
        NuRndrGradRectUV2di(screen_x, screen_y - 7, box_width, 30, 0.0f, 0.0f, 1.0f, 0.0f, colours, MessageMtl);
        colours[0] = 0x80808080;
        colours[1] = 0x80808080;
        colours[2] = 0x808080;
        colours[3] = 0x808080;
        NuRndrGradRectUV2di(screen_x, screen_y + box_height - 15, box_width, 30, 0.0f, 1.0f, 1.0f, 1.0f, colours, MessageMtl);
        NuRndrEndScene();
    }
}
f32 SmartTextLongestWidth = 0;
f32 SmartTextDefaultWidth = 1.5f;
f32 STCOORDSCALE = 1.0f;

void SmartTextEx(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment, u8 red, u8 green, u8 blue,
                 f32 max_width, i32 max_lines, void *message_box, i32 suppress_draw, i32 alpha) {
    typedef struct box_geometry_s {
        f32 reserved;
        f32 x; // 0x4
        f32 y; // 0x8
        f32 z;
        f32 width;  // 0x10
        f32 height; // 0x14
    } BOX_GEOMETRY;

    BOX_GEOMETRY *box = (BOX_GEOMETRY *)message_box;
    i32 widest_line = -1;
    f32 saved_x_scale;
    f32 saved_y_scale;
    f32 draw_x_scale;
    f32 width;
    f32 longest;
    f32 line_height;
    f32 line_y;
    i32 explicit_breaks;
    i32 last_line;
    i32 constrained;
    i32 characters;
    i32 bytes;
    i32 begin;
    i32 line;
    i32 end;
    i32 length;
    i32 next;
    i32 pos;
    unsigned char decoded[512];
    u16 encoded[512];
    unsigned char candidate[512];
    unsigned char lines[8][512];
    if ((SmartTextFont != NULL ? SmartTextFont : QFont2D) == NULL || text == NULL || text[0] == '\0' || MenuStopDraw != 0) {
        return;
    }

    x_scale = TextScaleX * x_scale;
    y_scale = TextScaleY * y_scale;
    saved_x_scale = TextScaleX;
    saved_y_scale = TextScaleY;

    TextScaleX = 1.0f;
    TextScaleY = 1.0f;
    x = STCOORDSCALE * x;
    max_width = STCOORDSCALE * max_width;
    TextDecode(text, decoded);
    Text3DStringEncodeFont(decoded, encoded, (SmartTextFont != NULL ? SmartTextFont : QFont2D));
    NuQFntSet((SmartTextFont != NULL ? SmartTextFont : QFont2D));
    NuQFntSetScale((SmartTextFont != NULL ? SmartTextFont : QFont2D), QFONTSCALEX * x_scale, QFONTSCALEY * y_scale);
    width = NuQFntPrintLenW((SmartTextFont != NULL ? SmartTextFont : QFont2D), encoded);
    line_height = NuQFntHeight((SmartTextFont != NULL ? SmartTextFont : QFont2D));
    NuQFntBaseline((SmartTextFont != NULL ? SmartTextFont : QFont2D));

    if (max_width <= 0.0f) {
        max_width = SmartTextDefaultWidth;
    }

    explicit_breaks = 0;
    for (pos = 0; decoded[pos] != '\0';) {
        if (decoded[pos] == '\\') {
            ++pos;
            if (decoded[pos] == 'n') {
                ++explicit_breaks;
            }
        }
        ++pos;
        while (decoded[pos] >= 0x80 && decoded[pos] <= 0xbf) {
            ++pos;
        }
    }
    last_line = (i32)(width / max_width);
    if (width / (last_line + 1) > max_width * (f64)0.9f) {
        ++last_line;
    }
    constrained = 0;
    if (max_lines > 8) {
        max_lines = 8;
    }
    if (max_lines > 0 && last_line + 1 > max_lines) {
        last_line = max_lines - 1;
        constrained = 1;
    }
    characters = NuStrLenU(decoded);
    bytes = NuStrLen((char *)(decoded));
    longest = 0.0f;
    if (explicit_breaks > 0) {
        for (line = 0; line <= explicit_breaks; ++line) {
            begin = line == 0 ? 0 : end + 3;
            while (decoded[begin] == ' ') {
                ++begin;
            }
            for (end = begin; end < bytes; ++end) {
                if (decoded[end] == '\\' && decoded[end + 1] == 'n') {
                    break;
                }
            }
            --end;
            length = end - begin + 1;
            if (length > 511) {
                length = 511;
            }
            PCByteCopy(lines[line], decoded + begin, length);
            lines[line][length] = '\0';
            Text3DStringEncodeFont(lines[line], encoded, (SmartTextFont != NULL ? SmartTextFont : QFont2D));
            width = NuQFntPrintLenW((SmartTextFont != NULL ? SmartTextFont : QFont2D), encoded);
            if (width > longest) {
                longest = width;
                widest_line = line;
            }
        }
        last_line = explicit_breaks;
    } else if (last_line > 0) {
        if ((alignment & 10) != 0 && constrained == 0) {
            end = 0;
            for (line = 0; line <= last_line; ++line) {
                begin = line == 0 ? 0 : end + 1;
                while (decoded[begin] == ' ') {
                    ++begin;
                }
                if (line == last_line) {
                    end = bytes - 1;
                } else {
                    f64 limit = max_width;
                    if (limit > 0.0) {
                        next = begin;
                        do {
                            end = next;
                            ++next;
                            while (decoded[next] == ' ') {
                                ++next;
                            }
                            if (next == bytes) {
                                end = next - 1;
                                break;
                            }
                            next = FindNextBreak(decoded, next);
                            if (decoded[next] != '-' && decoded[next] != '.' && decoded[next] != ',') {
                                --next;
                            }
                            while (decoded[next] == ' ') {
                                --next;
                            }
                            length = next - begin + 1;
                            if (length > 511) {
                                length = 511;
                            }
                            PCByteCopy(candidate, decoded + begin, length);
                            candidate[length] = '\0';
                            Text3DStringEncodeFont(candidate, encoded, (SmartTextFont != NULL ? SmartTextFont : QFont2D));
                        } while (NuQFntPrintLenW((SmartTextFont != NULL ? SmartTextFont : QFont2D), encoded) < limit);
                    }
                }
                length = end - begin + 1;
                if (length > 511) {
                    length = 511;
                }
                PCByteCopy(lines[line], decoded + begin, length);
                lines[line][length] = '\0';
                Text3DStringEncode((char *)(lines[line]), encoded);
                width = NuQFntPrintLenW((SmartTextFont != NULL ? SmartTextFont : QFont2D), encoded);
                if (width > longest) {
                    longest = width;
                    widest_line = line;
                }
            }
        } else {
            for (line = 0; line <= last_line; ++line) {
                if (line == 0) {
                    begin = 0;
                } else {
                    begin = FindNearestBreak(decoded, NuStrFindPosU(decoded, characters / (last_line + 1) * line));
                }
                if (line == last_line) {
                    end = bytes;
                } else {
                    end = FindNearestBreak(decoded, NuStrFindPosU(decoded, characters / (last_line + 1) * (line + 1)));
                }
                if (decoded[end] != '-' && decoded[end] != '.' && decoded[end] != ',') {
                    --end;
                }
                while (decoded[end] == ' ') {
                    --end;
                }
                while (decoded[begin] == ' ' || decoded[begin] == '-' || decoded[begin] == '.' || decoded[begin] == ',') {
                    ++begin;
                }
                length = end - begin + 1;
                if (length > 511) {
                    length = 511;
                }
                PCByteCopy(lines[line], decoded + begin, length);
                lines[line][length] = '\0';
                Text3DStringEncode((char *)(lines[line]), encoded);
                width = NuQFntPrintLenW((SmartTextFont != NULL ? SmartTextFont : QFont2D), encoded);
                if (width > longest) {
                    longest = width;
                    widest_line = line;
                }
            }
        }
    } else {
        longest = width;
    }
    if (lines[last_line][0] == '\0') {
        --last_line;
    }
    SmartTextLongestWidth = longest;
    if (max_width < longest) {
        draw_x_scale = max_width / longest * x_scale;
        SmartTextLongestWidth = max_width;
    } else {
        draw_x_scale = x_scale;
    }
    if (box != NULL) {
        NuQFntSetScale((SmartTextFont != NULL ? SmartTextFont : QFont2D), draw_x_scale, y_scale);
        if (widest_line > -1) {
            Text3DStringEncode((char *)(lines[widest_line]), encoded);
            width = NuQFntPrintLenW((SmartTextFont != NULL ? SmartTextFont : QFont2D), encoded);
        } else {
            width = longest;
        }
        box->x = x;
        box->y = y;
        box->width = width;
        box->height = (f32)((last_line + 1) * line_height * -1.0);
    }
    followon_line = 0;
    if (suppress_draw == 0) {
        if (last_line > 0) {
            for (line = 0; line <= last_line; ++line) {
                if ((alignment & TEXT_ALIGN_TOP) != 0) {
                    line_y = y + line_height * line;
                } else if ((alignment & TEXT_ALIGN_BOTTOM) != 0) {
                    line_y = y - last_line * line_height + line_height * line;
                } else {
                    line_y = y - last_line * 0.5f * line_height + line_height * line;
                }
                if (line != 0) {
                    followon_line = 1;
                }
                Text3DEx((char *)(lines[line]), x, line_y, z, draw_x_scale, y_scale, z_scale, alignment, red, green, blue, alpha);
            }
        } else {
            Text3DEx((char *)(decoded), x, y, z, draw_x_scale, y_scale, z_scale, alignment, red, green, blue, alpha);
        }
    }
    TextScaleX = saved_x_scale;
    followon_line = 0;
    TextScaleY = saved_y_scale;
}

void SmartTextEx2(char *text, f32 x, f32 y, f32 z, f32 sx, f32 sy, f32 sz, u32 alignment, u8 red, u8 green, u8 blue, f32 max_width,
                  i32 max_lines, void *box, i32 suppress, u32 alpha) {
    VUFNT *saved_font = SmartTextFont;
    if (QFont2DLower != NULL) {
        SmartTextFont = QFont2DLower;
    }
    SmartTextEx(text, x, y, z, sx, sy, sz, alignment, red, green, blue, max_width, max_lines, box, suppress, alpha);
    SmartTextFont = saved_font;
}

extern "C" void SmartTextExDrop(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment, u8 red, u8 green,
                                u8 blue, f32 max_width, i32 max_lines, void *message_box, i32 suppress_draw, u32 alpha) {
    SmartTextEx2(text, x - x_scale * 0.007f, y - y_scale * 0.01f, z, x_scale, y_scale, z_scale, alignment, 0, 0, 0, max_width, max_lines,
                 message_box, suppress_draw, alpha);
    SmartTextEx(text, x, y, z, x_scale, y_scale, z_scale, alignment, red, green, blue, max_width, max_lines, message_box, suppress_draw,
                alpha);
}

extern "C" void TextSetPrintMode(i32 mode) {
    TextPrintMode = mode;
}

void SmartTextSetFont(VUFNT *font) {
    SmartTextFont = font;
}

void SetGameFont(VUFNT *font) {
    QFont2D = font;
}

VUFNT *QFont3DZ = 0;

void Set3DGameFont(VUFNT *font) {
    QFont3DZ = font;
}

VUFNT *QFont2DZ = NULL;
VUFNT *QFont3D = NULL;
i32 create_qfont2dlower = 0;
i32 create_qfont2dz = 0;
i32 create_qfont3d = 0;
i32 create_qfont3dz = 0;

VUFNT *LoadGameFont(char *path, char *texture_path, VARIPTR *buffer, VARIPTR *end, i32 plane) {
    QFont2D = NuQFntLoadPtr(path, texture_path, 4, plane, buffer, end);
    if (QFont2D != NULL) {
        if (create_qfont2dlower != 0) {
            QFont2DLower = NuQFntDuplicate(QFont2D, 4, plane - 1, buffer, end);
        }
        if (create_qfont2dz != 0) {
            QFont2DZ = NuQFntDuplicate(QFont2D, 4, 0x40000, buffer, end);
        }
        if (create_qfont3d != 0) {
            QFont3D = NuQFntDuplicate(QFont2D, 0x18, plane, buffer, end);
        }
        if (create_qfont3dz != 0) {
            QFont3DZ = NuQFntDuplicate(QFont2D, 0x4c, plane, buffer, end);
        }
    }
    return QFont2D;
}

void UnloadGameFont(i32 unused) {
    if (QFont2DLower != NULL) {
        NuQFntDestroy(QFont2DLower);
        QFont2DLower = NULL;
    }
    if (QFont2DZ != NULL) {
        NuQFntDestroy(QFont2DZ);
        QFont2DZ = NULL;
    }
    if (QFont3D != NULL) {
        NuQFntDestroy(QFont3D);
        QFont3D = NULL;
    }
    if (QFont3DZ != NULL) {
        NuQFntDestroy(QFont3DZ);
        QFont3DZ = NULL;
    }
    NuQFntDestroy(QFont2D);
}

// noinline: the original calls SetQFont2D out of line from IntroText_Draw; VC8 would inline it there.
__declspec(noinline) void SetQFont2D(void) {
    NuQFntSetCoordinateSystem(NUQFNT_CS_640X224);
    NuQFntSet(QFont2D);
    NuQFntSetSpaceWidth(QFont2D, 12.0f);
}

void SmartTextSetWidescreen(f32 font_scale_x, f32 coordinate_scale) {
    QFONTSCALEX = font_scale_x;
    STCOORDSCALE = coordinate_scale;
}

void SmartTextGetWidescreen(f32 *font_scale_x, f32 *coordinate_scale) {
    if (font_scale_x != NULL) {
        *font_scale_x = QFONTSCALEX;
    }
    if (coordinate_scale != NULL) {
        *coordinate_scale = STCOORDSCALE;
    }
}

extern "C" void fun_0050e720(void) {
}

i32 SplitTextFindNextWS(u8 *text, i32 position) {
    u8 character = text[position];
check_character:
    if (character == ' ') {
        const u8 next = text[position + 1];
        if (next == '!' || next == '?' || next == ';' || next == ':') {
            goto next_character;
        }
        return position;
    }
    if (character == '\0') {
        return position;
    }
    if (character == '\\' && text[position + 1] == 'n') {
        return position;
    }
next_character:
    ++position;
    character = text[position];
    while (character >= 0x80 && character <= 0xbf) {
        ++position;
        character = text[position];
    }
    goto check_character;
}

extern "C" i32 PcControlWrapText(char *string, f32 max_width) {
    u8 *text = (u8 *)string;
    i32 line_start = 0;
    i32 line_count = 1;
    i32 position = 0;
    i32 previous_break = 0;
    u8 substring[512];
    u8 decoded[512];
    u16 encoded[512];
    f32 width;
    if (max_width <= 0.0f) {
        return -1;
    }
    for (;;) {
        i32 stop = 0;
        width = 0.0f;
        while (width < max_width && !stop) {
            previous_break = position;
            position = SplitTextFindNextWS(text, position);
            i32 length = position - line_start;
            if (length > 511) {
                length = 511;
            }
            PCByteCopy(substring, text + line_start, length);
            substring[length] = 0;
            TextDecode((char *)substring, decoded);
            Text3DStringEncodeFont(decoded, encoded, SmartTextFont ? SmartTextFont : QFont2D);
            width = NuQFntPrintLenW(QFont2D, encoded);
            while (text[position] == ' ') {
                ++position;
            }
            if (!text[position]) {
                stop = 1;
            }
            if (text[position] == '\\' && text[position + 1] == 'n') {
                stop = 1;
            }
        }
        if (!text[position]) {
            break;
        }
        if (text[position] == '\\' && text[position + 1] == 'n') {
            text[position + 1] = ' ';
            previous_break = position + 1;
        }
        if (previous_break == line_start) {
            previous_break = position;
        }
        ++line_count;
        text[previous_break - 1] = '\n';
        line_start = previous_break;
    }
    if (width >= max_width) {
        ++line_count;
        text[previous_break - 1] = '\n';
    }
    return line_count;
}

extern "C" void MatrixTextStringEncode(void *font, unsigned char *source, u16 *destination) {
    u16 character;
    while (*source != '\0') {
        source = NuUnicodeCharFromUTF8(&character, source);
        if (character == '~') {
            if (*source == '\0') {
                break;
            }
            source = NuUnicodeCharFromUTF8(&character, source);
            continue;
        }

        *destination = NuQFntEncodeUnicodeChar((VUFNT *)font, character);
        if (*destination == 0xffff) {
            NuUnicodeCharFromUTF8(&character, (unsigned char *)((char *)("\xe2\x96\xa1")));
            *destination = NuQFntEncodeUnicodeChar((VUFNT *)font, character);
            if (*destination == 0xffff) {
                *destination = NuQFntEncodeUnicodeChar((VUFNT *)font, '?');
            }
        }
        ++destination;
    }
    *destination = 0;
}

extern "C" f32 MatrixTextScale = 0.004f;

extern "C" void MatrixText(VUFNT *font, unsigned char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, u32 alignment, u32 colour,
                           NUMTX *matrix, i32 camera_relative) {
    u16 encoded[512];
    f32 width;
    f32 height;
    NUMTX draw_matrix;
    if (text == NULL || text[0] == '\0' || MenuStopDraw != 0) {
        return;
    }
    if (font == NULL) {
        font = QFont3DZ;
    }
    if (font == NULL) {
        return;
    }
    NuQFntPushPrintMode(NUQFNT_MODE_3D);
    NuQFntPushCoordinateSystem(NUQFNT_CS_UNIT_Y_DOWN);
    NuQFntSet(font);
    MatrixTextStringEncode(font, text, encoded);
    NuQFntSetScale(font, x_scale * MatrixTextScale, y_scale * MatrixTextScale);
    width = NuQFntPrintLenW(font, encoded);
    height = NuQFntHeight(font);
    y -= height * 0.5;
    if ((alignment & TEXT_ALIGN_BOTTOM) != 0) {
        y -= height * 0.5;
    } else if ((alignment & TEXT_ALIGN_TOP) != 0) {
        y += height * 0.5;
    }
    if ((alignment & TEXT_ALIGN_LEFT) == 0) {
        if ((alignment & TEXT_ALIGN_RIGHT) != 0) {
            x -= width;
        } else {
            x -= width * 0.5;
        }
    }
    if (camera_relative != 0) {
        NuMtxMul(&draw_matrix, matrix, NuCameraGetMtx());
    } else {
        draw_matrix = *matrix;
    }
    NuQFntSetMtx(font, &draw_matrix);
    NuQFntSetColour(font, colour);
    NuQFntSetICGap(font, 2.0f);
    NuQFntMove(font, x, y, z);
    NuQFntPrintW(font, encoded);
    NuQFntPopCoordinateSystem();
    NuQFntPopPrintMode();
}

void FixUpButtonsInFont(VUFNT *game_font_arg, VUFNT *button_font_arg) {
    VUFNT *game_font = game_font_arg;
    VUFNT *button_font = button_font_arg;

    if (game_font == NULL) {
        game_font = QFont2D;
    }
    if (button_font == NULL) {
        button_font = QFont2DButtons;
    }
    if (game_font != NULL && button_font != NULL) {
        f32 scale = game_font->height / button_font->height;
        i32 i;
        for (i = 0; i < button_font->glyph_count; ++i) {
            i32 j;
            for (j = 0; j < game_font->glyph_count; ++j) {
                if (game_font->unicode_map[j].unicode == button_font->unicode_map[i].unicode) {
                    if (button_font->unicode_map[i].unicode >= 0x531 && button_font->unicode_map[i].unicode <= 0x53f) {
                        game_font->glyphs[game_font->unicode_map[j].index].width =
                            button_font->glyphs[button_font->unicode_map[i].index].width * scale;
                    }
                    break;
                }
            }
        }
    }
}

extern "C" void fun_0050ed10(void) {
}

extern "C" void fun_0050ed20(void) {
}

extern "C" void fun_0050ed30(void) {
}

extern "C" void fun_0050ed40(void) {
}

extern "C" char *TextButtonString(i32 action, const char *player0, const char *player1) {
    char first[32] = "";
    char second[32] = "";
    const i32 device0 = NuPadGetPort(0);
    const i32 device1 = NuPadGetPort(1);
    if (player0 != NULL && device0 >= 0) {
        NuStrCpy(first, TextInputActionLabel(device0, action, 0, 0));
    }
    if (player1 != NULL && device1 >= 0) {
        NuStrCpy(second, TextInputActionLabel(device1, action, 0, 0));
    }
    if (first[0] != 0 && second[0] != 0) {
        NuSPrintf(HubButtonStringBuffer, "~0%s~~ \\ ~0%s~~", first, second);
    } else {
        NuSPrintf(HubButtonStringBuffer, "~0%s%s~~", first, second);
    }
    return HubButtonStringBuffer;
}

extern "C" void MenuApplyVideo(void) {
    if (PCDeviceBackend.HasRequestedModeChanged()) {
        MenuSFX = MENUSFX_MENUSELECT;
        MenuVideoOptions.Apply();
        PCDeviceBackend.CreateOrResetDevice(1);
        BackupMenu();
        NewMenu(MENU_VIDEO_CONFIRM, 0, -1);
        return;
    }
    if (MenuVideoOptions.Changed()) {
        MenuSFX = MENUSFX_MENUSELECT;
        MenuVideoOptions.Apply();
        if (PcWindowOptions.usehirespending != PcWindowOptions.usehires) {
            BackupMenu();
            NewMenu(MENU_HIRES_NOTICE, 0, -1);
            return;
        }
    } else {
        MenuSFX = MENUSFX_MENUBACK;
    }
    BackupMenu();
}

extern "C" void MenuCancelVideo(void) {
    PCDeviceBackend.ResetRequestedMode();
    MenuVideoOptions.aspect_mode = PcWindowOptions.aspect_mode;
    MenuVideoOptions.shadows_enabled = PcWindowOptions.shadowsenabled != 0;
    MenuVideoOptions.bloom_enabled = PcWindowOptions.bloomenabled != 0;
    MenuVideoOptions.speed_blur_enabled = PcWindowOptions.speedblurenabled != 0;
    MenuVideoOptions.hires_enabled = PcWindowOptions.usehirespending != 0;
    MenuSFX = MENUSFX_MENUBACK;
    BackupMenu();
}

extern "C" void MenuUpdateVideo(MENU *menu) {
    if (menu->cancel_pressed) {
        MenuCancelVideo();
        return;
    }
    switch (menu->selected_row) {
        case MENU_VIDEO_ROW_RESOLUTION:
            if (menu->right_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                PCDeviceBackend.NextResolution();
            } else if (menu->left_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                PCDeviceBackend.PreviousResolution();
            }
            break;
        case MENU_VIDEO_ROW_REFRESH:
            if (menu->right_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                PCDeviceBackend.NextRefresh();
            } else if (menu->left_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                PCDeviceBackend.PreviousRefresh();
            }
            break;
        case MENU_VIDEO_ROW_ASPECT:
            if (menu->right_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                if (++MenuVideoOptions.aspect_mode > 3) {
                    MenuVideoOptions.aspect_mode = PC_ASPECT_4_3;
                }
            } else if (menu->left_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                if (--MenuVideoOptions.aspect_mode < 0) {
                    MenuVideoOptions.aspect_mode = PC_ASPECT_AUTO;
                }
            }
            break;
        case MENU_VIDEO_ROW_HIRES:
            if (menu->right_pressed || menu->left_pressed || menu->confirm_pressed) {
                if (!PcWindowOptions.option459) {
                    MenuSFX = MENUSFX_MENUNOENTRY;
                } else {
                    MenuSFX = MENUSFX_MENUMOVE;
                    MenuVideoOptions.hires_enabled = 1 - MenuVideoOptions.hires_enabled;
                }
            }
            break;
        case MENU_VIDEO_ROW_APPLY:
            if (menu->confirm_pressed) {
                MenuApplyVideo();
            }
            break;
    }
}

extern "C" void MenuUpdateEffects(MENU *menu) {
    if (menu->cancel_pressed) {
        MenuCancelVideo();
        return;
    }
    switch (menu->selected_row) {
        case MENU_EFFECTS_ROW_MULTISAMPLE:
            if (menu->right_pressed || menu->left_pressed) {
                if (PcWindowOptions.usehires) {
                    MenuSFX = MENUSFX_MENUNOENTRY;
                    return;
                }
                if (!MenuCanChangeMultisample()) {
                    MenuSFX = MENUSFX_MENUNOENTRY;
                    return;
                }
                MenuSFX = MENUSFX_MENUMOVE;
                i32 index = PCDeviceBackend.requested_multisample;
                if (menu->left_pressed) {
                    --index;
                } else {
                    ++index;
                }
                if (index < 0) {
                    index = 4;
                } else if (index >= 5) {
                    index = 0;
                }
                if (!PCDeviceBackend.SupportsMultisample(index)) {
                    do {
                        if (++index >= 5) {
                            index = 0;
                        }
                    } while (!PCDeviceBackend.SupportsMultisample(index));
                }
                PCDeviceBackend.requested_multisample = index;
            }
            break;
        case MENU_EFFECTS_ROW_SHADOWS:
            if (menu->right_pressed || menu->left_pressed || menu->confirm_pressed) {
                if (!MenuCanChangeShadows()) {
                    MenuSFX = MENUSFX_MENUNOENTRY;
                    return;
                }
                MenuSFX = MENUSFX_MENUMOVE;
                MenuVideoOptions.shadows_enabled = 1 - MenuVideoOptions.shadows_enabled;
            }
            break;
        case MENU_EFFECTS_ROW_BLOOM:
            if (menu->right_pressed || menu->left_pressed || menu->confirm_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                MenuVideoOptions.bloom_enabled = 1 - MenuVideoOptions.bloom_enabled;
            }
            break;
        case MENU_EFFECTS_ROW_SPEED_BLUR:
            if (menu->right_pressed || menu->left_pressed || menu->confirm_pressed) {
                MenuSFX = MENUSFX_MENUMOVE;
                MenuVideoOptions.speed_blur_enabled = 1 - MenuVideoOptions.speed_blur_enabled;
            }
            break;
        case MENU_EFFECTS_ROW_APPLY:
            if (menu->confirm_pressed) {
                MenuApplyVideo();
            }
            break;
    }
}

extern "C" void MenuUpdateVideoConfirm(MENU *menu) {
    if (MenuVideoConfirmWait != 0) {
        --MenuVideoConfirmWait;
        menu->menu_time = 0.0f;
        menu->selected_row = 1;
    }
    if (menu->selected_row == 0 && menu->confirm_pressed) {
        MenuSFX = MENUSFX_MENUBACK;
        BackupMenu();
        if (PcWindowOptions.usehirespending != PcWindowOptions.usehires) {
            NewMenu(MENU_HIRES_NOTICE, 0, -1);
        }
        return;
    }
    if ((menu->selected_row == 1 && menu->confirm_pressed) || menu->menu_time >= MenuVideoConfirmDuration) {
        PCDeviceBackend.RestoreSavedMode();
        MenuVideoOptions.aspect_mode = PcWindowOptions.aspect_mode;
        MenuVideoOptions.shadows_enabled = PcWindowOptions.shadowsenabled != 0;
        MenuVideoOptions.bloom_enabled = PcWindowOptions.bloomenabled != 0;
        MenuVideoOptions.speed_blur_enabled = PcWindowOptions.speedblurenabled != 0;
        MenuVideoOptions.hires_enabled = PcWindowOptions.usehirespending != 0;
        MenuSFX = MENUSFX_MENUBACK;
        BackupMenu();
    }
}

extern "C" void MenuUpdateHiresNotice(MENU *menu) {
    if (menu->confirm_pressed || menu->cancel_pressed) {
        MenuSFX = MENUSFX_MENUBACK;
        BackupMenu();
    }
}

extern "C" void MenuUpdateQuit(MENU *menu) {
    if ((menu->selected_row == 0 && menu->confirm_pressed != 0) || menu->cancel_pressed != 0) {
        MenuSFX = MENUSFX_MENUBACK;
        BackupMenu();
        return;
    }
    if (menu->selected_row == 1 && menu->confirm_pressed != 0) {
        PcQuitToWindows();
    }
}

void MenuSmartTextEx(char *text, f32 x, f32 y, f32 z, f32 sx, f32 sy, f32 sz, u32 alignment, u8 red, u8 green, u8 blue, f32 max_width,
                     i32 max_lines, void *box, i32 suppress, i32 alpha) {
    if (MenuDrawDropShadows != 0) {
        f32 shadow_x = (f32)((f64)x + (f64)sx * (f64)0.015f);
        f32 shadow_y = (f32)((f64)y - (f64)sy * (f64)0.015f);
        SmartTextEx2(text, shadow_x, shadow_y, z, sx, sy, sz, alignment, 0, 0, 0, max_width, max_lines, box, suppress, alpha >> 2);
    }
    SmartTextEx(text, x, y, z, sx, sy, sz, alignment, red, green, blue, max_width, max_lines, box, suppress, alpha);
}

void MenuText3DEx(char *text, f32 x, f32 y, f32 z, f32 sx, f32 sy, f32 sz, u32 alignment, u32 red, u32 green, u32 blue, i32 alpha) {
    if (MenuDrawDropShadows != 0) {
        f32 shadow_x = (f32)((f64)x + (f64)sx * (f64)0.015f);
        f32 shadow_y = (f32)((f64)y - (f64)sy * (f64)0.015f);
        Text3DEx2(text, shadow_x, shadow_y, z, sx, sy, sz, alignment, 0, 0, 0, (u8)alpha >> 2);
    }
    Text3DEx(text, x, y, z, sx, sy, sz, alignment, red, green, blue, alpha);
}

i32 MenuDisableHeaders = 0;

void DrawMenuHeader(void) {
    if (MenuDisableHeaders == 0) {
        MenuSmartTextEx(MenuHeader, 0, MenuTopY, 1, MenuTextScale, MenuTextScale, MenuTextScale, 0, (u8)header_r, (u8)header_g,
                        (u8)header_b, 1.2f, 1, NULL, 0, MenuA);
    }
}

extern "C" void MenuDrawTopText(char *text, u32 red, u32 green, u32 blue) {
    MenuSmartTextEx(text, 0.0f, MenuTopY, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 1, red, green, blue, 1.5f, 3, NULL, 0, MenuA);
}

extern "C" void MenuDrawNotice(char *text, u32 red, u32 green, u32 blue) {
    MenuSmartTextEx(text, 0.0f, 0.15f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, red, green, blue, 1.5f, 4, NULL, 0, MenuA);
}

extern "C" void MenuDrawLowerText(char *text, u32 red, u32 green, u32 blue) {
    MenuSmartTextEx(text, 0.0f, -0.3f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, red, green, blue, 1.5f, 4, NULL, 0, MenuA);
}

i32 menu_flash = 0;
i32 dme_rgb = 0;
i32 dme_align = 0;
u8 dme_b = 0;
u8 dme_r = 0;
u8 dme_g = 0;
f32 dme_sx = 1.0f;
f32 dme_sy = 1.0f;
f32 MENUENTRYEXWIDTH = 1.25f;

void DrawMenuEntryEx(MENU *menu, char *text, u8 alpha) {
    MENU *view = menu;
    f32 scale;
    f32 spacing;

    /* Writes low bytes, then forwards each partially written 32-bit color slot. */
    union {
        u8 byte;
        u32 slot;
    } red, green, blue;

    alpha = alpha > 128 ? 128 : alpha;
    scale = (view->item_scale_x == 0.0f ? MenuTextScale : view->item_scale_x) * dme_sy;
    spacing = (view->item_spacing == 0.0f ? MENUDY : view->item_spacing) * dme_sy;
    if (view->item_offsets != NULL) {
        f32 offset_x = view->item_offsets[0];
        Text3DEx2(text, (f32)(view->draw_x + (f64)offset_x * view->item_offset_scale),
                  (f32)(view->draw_y + (f64)view->item_offsets[1] * view->item_offset_scale), view->draw_z, (f32)((f64)scale * dme_sx),
                  scale, scale, dme_align, 0, 0, 0, alpha >> 2);
    }
    if (view->draw_item >= view->first_row && view->draw_item <= view->last_row) {
        if (view->draw_item == menu->selected_row) {
            if (menu_flash != 0) {
                red.byte = MENUFLASH0R;
                green.byte = MENUFLASH0G;
                blue.byte = MENUFLASH0B;
            } else {
                red.byte = MENUFLASH1R;
                green.byte = MENUFLASH1G;
                blue.byte = MENUFLASH1B;
            }
            if (menu_pulsate > 0.0f) {
                red.byte = (u8)(MENUFLASH0R * (f64)menu_pulsate + MENUFLASH1R * (1.0 - menu_pulsate));
                green.byte = (u8)(MENUFLASH0G * (f64)menu_pulsate + MENUFLASH1G * (1.0 - menu_pulsate));
                blue.byte = (u8)(MENUFLASH0B * (f64)menu_pulsate + MENUFLASH1B * (1.0 - menu_pulsate));
            }
        } else if (dme_rgb != 0) {
            red.byte = dme_r;
            green.byte = dme_g;
            blue.byte = dme_b;
        } else {
            red.byte = MENUENTRYR;
            green.byte = MENUENTRYG;
            blue.byte = MENUENTRYB;
        }
        if (view->item_offsets != NULL) {
            Text3DEx(text, view->draw_x, view->draw_y, view->draw_z, (f32)((f64)scale * dme_sx), scale, scale, dme_align, red.slot,
                     green.slot, blue.slot, alpha);
        } else {
            MenuSmartTextEx(text, view->draw_x, view->draw_y, view->draw_z, (f32)((f64)scale * dme_sx), scale, scale, dme_align, red.slot,
                            green.slot, blue.slot, MENUENTRYEXWIDTH, 1, NULL, 0, alpha);
        }
    }
    ++view->draw_item;
    view->draw_y = (f32)((f64)spacing + view->draw_y);
    dme_rgb = 0;
    dme_align = 0;
    dme_sx = 1.0f;
    dme_sy = 1.0f;
}

void DrawMenuButtonPromptsEx(i32 confirm, i32 cancel, i32 flags, i32 enabled, u32 red, u32 green, u32 blue, u32 alpha) {
    char accept_text[128];
    char cancel_text[128];
    char axes_text[128];
    char *accept = "[[accept]]";
    char *back = "[[back]]";
    char *left = "[[padleft]]";
    char *right = "[[padright]]";
    char **keys;
    i32 device;
    f64 row_delta;
    f32 y;
    f32 scale;
    f64 x;
    device = NuPadGetPort(PcInput_GetCtrlStringPlayer());
    if (PcInput_PortIsKeyboard(device)) {
        keys = PcInput_GetKeyNameTable();
        accept = keys[28];
        back = keys[1];
        left = keys[203];
        right = keys[205];
    }
    if (enabled) {
        sprintf(accept_text, "%s ~0%s", apitxt_SELECT, accept);
        sprintf(cancel_text, "%s ~0%s", apitxt_BACK, back);
    } else {
        sprintf(accept_text, "~0$s~~ %s", accept, apitxt_SELECT);
        sprintf(cancel_text, "~0%s~~ %s", back, apitxt_BACK);
    }
    if (!PcInput_PortIsUC(device)) {
        sprintf(axes_text, "%s ~0%s~~ / ~0$s", apitxt_SELECT, left, right);
    } else {
        sprintf(axes_text, "~0$s~~ %s ~0$s", left, apitxt_SELECT, right);
    }

    row_delta = (f64)MENUDY * (f64)0.8f * MenuPromptPositionScale;
    y = (f32)(row_delta - (f64)0.85f);
    y = (f32)((f64)y + row_delta);
    y = (f32)((f64)y - (f64)0.05f);
    if (confirm) {
        y = (f32)((f64)y - row_delta);
    }
    if (cancel) {
        y = (f32)((f64)y - row_delta);
    }
    if (flags) {
        y = (f32)((f64)y - row_delta);
    }
    if (confirm) {
        x = enabled ? 0.85f : -0.85f;
        scale = (f32)((f64)0.8f * MenuTextScale);
        MenuText3DEx(accept_text, x, y, 1, scale, scale, scale, enabled ? 12 : 6, red, green, blue, alpha);
        row_delta = (f64)MENUDY * (f64)0.8f * MenuPromptPositionScale;
        y = (f32)((f64)y + row_delta);
    }
    if (cancel) {
        x = enabled ? 0.85f : -0.85f;
        scale = (f32)((f64)0.8f * MenuTextScale);
        MenuText3DEx(cancel_text, x, y, 1, scale, scale, scale, enabled ? 12 : 6, red, green, blue, alpha);
        row_delta = (f64)MENUDY * (f64)0.8f * MenuPromptPositionScale;
        y = (f32)((f64)y + row_delta);
    }
    if (flags) {
        x = enabled ? 0.85f : -0.85f;
        scale = (f32)((f64)0.8f * MenuTextScale);
        MenuText3DEx(axes_text, x, y, 1, scale, scale, scale, enabled ? 12 : 6, red, green, blue, alpha);
    }
}

void DrawMenuButtonPrompts(i32 confirm, i32 cancel, i32 enabled, u32 red, u32 green, u32 blue, u32 alpha) {
    DrawMenuButtonPromptsEx(confirm, cancel, 0, enabled, red, green, blue, alpha);
}

void DrawMenu(i32 paused) {
    MENU *menu;
    f32 saved_baseline = 0.0f;
    f32 saved_button_baseline = 0.0f;
    f32 centre;
    if (memcard_autosavestarted != 0 || memcard_autosavepostdelay > 0.0f || memcard_autosavepredelay > 0.0f) {
        if (memcard_drawasiconfn != NULL) {
            memcard_drawasiconfn();
        }
        return;
    }
    menu = &GameMenu[GameMenuLevel];
    if (menu->menu == -1) {
        return;
    }
    MenuValidated = 1;
    if ((SmartTextFont != NULL ? SmartTextFont : QFont2D) != NULL) {
        saved_baseline = (SmartTextFont != NULL ? SmartTextFont : QFont2D)->baseline;
        (SmartTextFont != NULL ? SmartTextFont : QFont2D)->baseline = 0.0f;
    }
    if (QFont2DButtons != NULL) {
        saved_button_baseline = QFont2DButtons->baseline;
        QFont2DButtons->baseline = 0.0f;
    }
    MenuHeader[0] = 0;
    menu->draw_item = menu->first_row;
    menu->item_scale = 1.0f;
    menu->draw_offset = 0.0f;
    centre = (f32)((f64)(menu->last_row - menu->first_row) * (f64)MENUDY * 0.5);
    menu->centre_offset = centre;
    menu->draw_x = 0.0f;
    menu->draw_y = -centre;
    menu->draw_z = 1.0f;

    menu->paused = paused;
    if (predrawfn != NULL) {
        predrawfn(menu);
    }
    if (MenuInfo[menu->menu].draw_fn != NULL) {
        MenuInfo[menu->menu].draw_fn(menu);
    }
    if (headerdrawfn != NULL) {
        headerdrawfn();
    }
    if (MenuAlpha >= 0.5f && g_enableButtonPrompts != 0) {
        DrawMenuButtonPrompts(MenuInfo[menu->menu].confirm_prompt, MenuInfo[menu->menu].cancel_prompt, 1, MENUNORMALR, MENUNORMALG,
                              MENUNORMALB, (u8)MenuA);
    }
    if ((SmartTextFont != NULL ? SmartTextFont : QFont2D) != NULL) {
        (SmartTextFont != NULL ? SmartTextFont : QFont2D)->baseline = saved_baseline;
    }
    if (QFont2DButtons != NULL) {
        QFont2DButtons->baseline = saved_button_baseline;
    }
}

void APIMenuDrawGameState(f32 x, f32 y, i32 highlight, i32 slot) {
    char label[32];
    i32 red;
    i32 green;
    i32 blue;
    if (highlight != 0) {
        if (menu_flash != 0) {
            red = MENUFLASH0R;
            green = MENUFLASH0G;
            blue = MENUFLASH0B;
        } else {
            red = MENUFLASH1R;
            green = MENUFLASH1G;
            blue = MENUFLASH1B;
        }
        if (menu_pulsate > 0.0f) {
            red = (i32)(MENUFLASH0R * (f64)menu_pulsate + MENUFLASH1R * (1.0 - menu_pulsate));
            green = (i32)(MENUFLASH0G * (f64)menu_pulsate + MENUFLASH1G * (1.0 - menu_pulsate));
            blue = (i32)(MENUFLASH0B * (f64)menu_pulsate + MENUFLASH1B * (1.0 - menu_pulsate));
        }
    } else {
        red = MENUFLASH1R;
        green = MENUFLASH1G;
        blue = MENUFLASH1B;
    }
    sprintf(label, "%s %i", apitxt_SLOT, slot + 1);
    MenuText3DEx(label, x, y, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, red, green, blue, (u8)MenuA);
    if (saveload_slotused[slot] != 0) {
        MenuSmartTextEx(apitxt_PRESENT, x, (f32)(y + MenuTextScale * 0.5), 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0,
                        MENUNORMALR, MENUNORMALG, MENUNORMALB, 0.4f, 2, NULL, 0, MenuA);
    } else if (saveload_freespace >= SAVESIZE_ADDITIONAL) {
        MenuSmartTextEx(apitxt_EMPTY, x, (f32)(y + MenuTextScale * 0.5), 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUNORMALR,
                        MENUNORMALG, MENUNORMALB, 0.4f, 2, NULL, 0, MenuA);
    } else {
        MenuSmartTextEx(apitxt_NOSPACE, x, (f32)(y + MenuTextScale * 0.5), 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUWARNR,
                        MENUWARNG, MENUWARNB, 0.4f, 2, NULL, 0, MenuA);
    }
}

void APIMenuDrawMemCardSlots(MENU *menu, f32 y) {
    i32 count;
    i32 first;
    i32 last;
    i32 slot;
    i32 highlight;
    i32 red;
    i32 green;
    i32 blue;
    f32 x;
    f32 scale;
    if (slideright > 0) {
        --slideright;
    }
    if (slideleft > 0) {
        --slideleft;
    }
    if (SAVESLOTS > 3) {
        if (menu->selected_column != lastslot) {
            if (lastslot > menu->selected_column) {
                slideright = 10;
            } else {
                slideleft = 10;
            }
            lastslot = menu->selected_column;
        }
        first = menu->selected_column - 1;
        if (slideleft != 0) {
            --first;
        }
        last = menu->selected_column + 1;
        if (slideright != 0) {
            ++last;
        }
    } else {
        first = 0;
        last = SAVESLOTS - 1;
    }
    if (SAVESLOTS <= 6) {
        count = SAVESLOTS;
    } else {
        count = memcard_slotsused;
    }
    slot = first;
    if (SAVESLOTS <= 3) {
        x = (SAVESLOTS - 1) * -0.25;
    } else {
        x = 0.0 - (menu->selected_column - first) * 0.5 - slideright * 0.5 / 10.0 + slideleft * 0.5 / 10.0;
    }
    for (; slot <= last; ++slot, x += 0.5) {
        highlight = menu->selected_row == menu->first_row && menu->selected_column == slot && slideleft == 0 && slideright == 0;
        if (slot < 0 || slot > menu->last_column) {
            continue;
        }
        if (slot < count) {
            drawslotinfofn(x, y, highlight, slot);
        } else {
            if (menu->selected_column == count && menu->selected_row == 0) {
                if (menu_flash != 0) {
                    red = MENUFLASH0R;
                    green = MENUFLASH0G;
                    blue = MENUFLASH0B;
                } else {
                    red = MENUFLASH1R;
                    green = MENUFLASH1G;
                    blue = MENUFLASH1B;
                }
                if (menu_pulsate > 0.0f) {
                    red = (i32)(MENUFLASH0R * (f64)menu_pulsate + MENUFLASH1R * (1.0 - menu_pulsate));
                    green = (i32)(MENUFLASH0G * (f64)menu_pulsate + MENUFLASH1G * (1.0 - menu_pulsate));
                    blue = (i32)(MENUFLASH0B * (f64)menu_pulsate + MENUFLASH1B * (1.0 - menu_pulsate));
                }
            } else {
                red = MENUENTRYR;
                green = MENUENTRYG;
                blue = MENUENTRYB;
            }
            MenuText3DEx(apitxt_NEWSAVE, x, y, 1.0f, (f32)(MenuTextScale * (f64)0.85f), (f32)(MenuTextScale * (f64)0.85f), MenuTextScale, 0,
                         red, green, blue, (u8)MenuA);
        }
    }
    if (menu->selected_row == 0) {
        if (menu_flash != 0) {
            red = MENUFLASH0R;
            green = MENUFLASH0G;
            blue = MENUFLASH0B;
        } else {
            red = MENUFLASH1R;
            green = MENUFLASH1G;
            blue = MENUFLASH1B;
        }
        if (menu_pulsate > 0.0f) {
            red = (i32)(MENUFLASH0R * (f64)menu_pulsate + MENUFLASH1R * (1.0 - menu_pulsate));
            green = (i32)(MENUFLASH0G * (f64)menu_pulsate + MENUFLASH1G * (1.0 - menu_pulsate));
            blue = (i32)(MENUFLASH0B * (f64)menu_pulsate + MENUFLASH1B * (1.0 - menu_pulsate));
        }
    } else {
        red = MENUENTRYR;
        green = MENUENTRYG;
        blue = MENUENTRYB;
    }
    scale = MenuTextScale + MenuTextScale;
    if (slideleft == 0 && slideright == 0) {
        if (first > 0) {
            MenuText3DEx("<", -0.8f, y, 1.0f, scale, scale, scale, 0, red, green, blue, (u8)MenuA);
        }
        if (last < menu->last_column) {
            MenuText3DEx(">", 0.8f, y, 1.0f, scale, scale, scale, 0, red, green, blue, (u8)MenuA);
        }
    }
}

extern "C" void fun_005108c0(MENU *menu) {
    /* Like MenuDrawCardWarning: the text is not drawn, its buffer keeps the security cookie. */
    char text[8];
    if (0) {
        sprintf(text, "%d", menu->menu);
    }
}

extern "C" void fun_005108e0(MENU *menu) {
}

static void MenuDrawNotEnoughSpaceMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_NOTENOUGHSPACE, 0.0f, 0.3f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3, NULL,
                    0, MenuA);
}

static void Draw_NODATAAVAILABLE(void) {
    char message[1024];
    f32 scale = MenuTextScale;
    sprintf(message, apitxt_NODATAAVAILABLE, apiGameName);
    MenuSmartTextEx(message, 0.0f, -0.4f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3, NULL, 0, MenuA);
}

static void MenuDrawSavingMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_GAMESAVING, 0.0f, -0.2f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0,
                    MenuA);
}

static void MenuDrawSavedMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_GAMESAVED, 0.0f, -0.2f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0,
                    MenuA);
}

static void MenuDrawSaveFailedMessage(void) {
    f32 scale = MenuTextScale;
    if (Menu_Overwriteflag != 0) {
        MenuSmartTextEx(apitxt_FAILEDTOOVERWRITE, 0.0f, 0.0f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3,
                        NULL, 0, MenuA);
    } else {
        MenuSmartTextEx(apitxt_FAILEDTOSAVE, 0.0f, 0.0f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3, NULL,
                        0, MenuA);
    }
}

static void MenuDrawLoadingMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_GAMELOADING, 0.0f, -0.4f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0,
                    MenuA);
}

static void MenuDrawLoadedMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_GAMELOADED, 0.0f, -0.4f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0,
                    MenuA);
}

static void MenuDrawLoadFailedMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_FAILEDTOLOAD, 0.0f, 0.0f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3, NULL, 0,
                    MenuA);
}

static void MenuDrawLoadCorruptMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_CORRUPTLOAD, 0.0f, 0.0f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3, NULL, 0,
                    MenuA);
}

static void MenuDrawFormattingMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_FORMATTING, 0.0f, 0.2f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0,
                    MenuA);
}

static void MenuDrawFormattedMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_FORMATTINGCOMPLETE, 0.0f, 0.0f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3,
                    NULL, 0, MenuA);
}

static void MenuDrawDeletingMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_DELETING, 0.0f, 0.2f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0,
                    MenuA);
}

static void MenuDrawDeletedMessage(void) {
    f32 scale = MenuTextScale;
    MenuSmartTextEx(apitxt_DELETECOMPLETE, 0.0f, 0.0f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3, NULL,
                    0, MenuA);
}

static void Draw_AUTOSAVEWARNING(void) {
    char message[1024];
    f32 scale = MenuTextScale;
    if (memcard_drawasiconfn != NULL) {
        memcard_drawasiconfn();
    }
    sprintf(message, apitxt_AUTOSAVE_ENABLED, saveload_autosave + 1);
    MenuSmartTextEx(message, 0.0f, 0.45f, 1.0f, scale, scale, scale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.5f, 3, NULL, 0, MenuA);
}

void MenuDrawInsertCard(MENU *menu) {
    char message[1024];
    if (0) {
        sprintf(message, "");
    }
}

void MenuDrawNotEnoughSpace(MENU *menu) {
    char message[1024];
    if (0) {
        sprintf(message, "");
    }
    MenuDrawNotEnoughSpaceMessage();
}

static i32 MenuFormattingResultShown = 0;

extern "C" void MenuDrawFormatting(MENU *menu) {
    if (memcard_formatting != 0 || memcard_formatme != 0 || memcard_formatmessage_delay > 0.0f) {
        MenuFormattingResultShown = 0;
        MenuDrawFormattingMessage();
        return;
    }
    if (memcard_formatfailed != 0) {
        if (MenuFormattingResultShown == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuFormattingResultShown = 1;
        }
        return;
    }
    if (MenuFormattingResultShown == 0) {
        MenuAlpha = 0.0f;
        MenuA = 0;
        MenuFormattingResultShown = 1;
    }
    MenuDrawFormattedMessage();
}

static i32 MenuDeleteResultShown = 0;

void MenuDrawDeleting(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_DELETEGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (memcard_deleteneeded != 0 || memcard_deletestarted != 0 || memcard_deletemessage_delay > 0.0f) {
        MenuDeleteResultShown = 0;
        MenuDrawDeletingMessage();
    } else if (memcard_deletefailed != 0) {
        if (MenuDeleteResultShown == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuDeleteResultShown = 1;
        }
    } else {
        if (MenuDeleteResultShown == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuDeleteResultShown = 1;
        }
        MenuDrawDeletedMessage();
    }
}

void MenuDrawFileCorrupt(MENU *menu) {
    char message[1024];
    sprintf(message, apitxt_CORRUPTLOAD);
    MenuSmartTextEx(message, 0.0f, 0.0f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUNORMALR, MENUNORMALG, MENUNORMALB, 1.3f,
                    3, NULL, 0, MenuA);
}

static i32 MenuSaveResultShown = 0;

void MenuDrawSaving(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_SAVEGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (memcard_saveneeded != 0 || memcard_savestarted != 0 || memcard_savemessage_delay > 0.0f) {
        MenuSaveResultShown = 0;
        MenuDrawSavingMessage();
    } else if (memcard_savefailed != 0) {
        if (MenuSaveResultShown == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuSaveResultShown = 1;
        }
        MenuDrawSaveFailedMessage();
    } else {
        if (MenuSaveResultShown == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuSaveResultShown = 1;
        }
        MenuDrawSavedMessage();
    }
}

static i32 MenuLoadResultShown = 0;

extern "C" void MenuDrawLoading(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_LOADGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (memcard_loadneeded != 0 || memcard_loadstarted != 0 || memcard_loadmessage_delay > 0.0f) {
        MenuLoadResultShown = 0;
        MenuDrawLoadingMessage();
    } else if (memcard_loadfailed != 0) {
        if (MenuLoadResultShown == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuLoadResultShown = 1;
        }
        MenuDrawLoadFailedMessage();
    } else if (memcard_loadcorrupt != 0) {
        if (MenuLoadResultShown == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuLoadResultShown = 1;
        }
        MenuDrawLoadCorruptMessage();
    } else {
        if (MenuLoadResultShown == 0) {
            MenuAlpha = 0.0f;
            MenuA = 0;
            MenuLoadResultShown = 1;
        }
        MenuDrawLoadedMessage();
    }
}

i32 saveload_cardchanged = 0;

void loadsaveCallEachFrame(void) {
    saveloadASCallEachFrame();
    if (saveload_cardchanged != 0) {
        memcard_cardchanged = 1;
    }
    UpdateSaveSlots();
    memcard_slotsused = 0;
    if (saveload_savepresent != 0) {
        i32 i;
        for (i = 0; i < SAVESLOTS; ++i) {
            if (saveload_slotused[i] != 0) {
                ++memcard_slotsused;
            }
        }
    }
}

void Text3D(char *text, f32 x, f32 y, f32 z, f32 x_scale, f32 y_scale, f32 z_scale, u32 alignment, u8 red, u8 green, u8 blue) {
    Text3DEx(text, x, y, z, x_scale, y_scale, z_scale, alignment, red, green, blue, 128);
}

/* Lives in the menu unit and is never inlined into hub callers. */
extern "C" void SmartText(char *text, f32 x, f32 y, f32 z, f32 sx, f32 sy, f32 sz, i32 align, i32 red, i32 green, i32 blue, f32 max_width,
                          i32 center) {
    SmartTextEx(text, x, y, z, sx, sy, sz, align, red, green, blue, max_width, center, NULL, 0, 128);
}

VUFNT *LoadButtonFont(char *path, char *texture_path, VARIPTR *buffer, VARIPTR *end, i32 plane) {
    if (QFont2DButtons == NULL) {
        QFont2DButtons = NuQFntLoadPtr(path, texture_path, 4, plane, buffer, end);
        if (QFont2DButtons != NULL) {
            VUFNT *font;
            FixUpButtonsInFont(QFont2D, QFont2DButtons);
            font = QFont2D;
            QFont2DButtons->baseline = QFont2DButtons->height / font->height * font->baseline;
        }
    }
    return QFont2DButtons;
}

extern "C" void DrawMenuEntry(MENU *menu, char *text) {
    DrawMenuEntryEx(menu, text, MenuA);
}

extern "C" void DrawMenuEntryOK(MENU *menu) {
    menu->draw_y = MENUBOTY;
    DrawMenuEntryEx(menu, apitxt_OK, MenuA);
}

extern "C" void DrawMenuEntryCancel(MENU *menu) {
    menu->draw_y = MENUBOTY;
    DrawMenuEntryEx(menu, apitxt_CANCEL, MenuA);
}

void MenuDrawSave(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_SAVEGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (drawslotsfn != NULL && MenuAlpha > (f64)0.2f && MenuStopDraw == 0) {
        drawslotsfn(menu, -0.3f);
    }
    ++menu->draw_item;
    if (Menu_DisableCancel == 0) {
        menu->draw_y = MENUBOTY;
        DrawMenuEntryEx(menu, apitxt_CANCEL, (u8)MenuA);
    }
}

void MenuDrawNoMemoryCard(MENU *menu) {
    menu->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_RETRY, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_CANCEL, (u8)MenuA);
}

extern "C" void MenuDrawNoData(MENU *menu) {
    Draw_NODATAAVAILABLE();
    menu->draw_y = MENUBOTY;
    DrawMenuEntryEx(menu, apitxt_CANCEL, (u8)MenuA);
}

void MenuDrawDeleteConfirm(MENU *menu) {
    MenuSmartTextEx(apitxt_CONFIRMDELETE, 0.0f, 0.0f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUNORMALR, MENUNORMALG,
                    MENUNORMALB, 1.2f, 2, NULL, 0, MenuA);
    menu->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_YES, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_NO, (u8)MenuA);
}

void MenuDrawSaveConfirm(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_SAVEGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (Menu_Overwriteflag != 0) {
        MenuSmartTextEx(apitxt_CONFIRMOVERWRITE, 0.0f, -0.3f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUNORMALR,
                        MENUNORMALG, MENUNORMALB, 1.2f, 2, NULL, 0, MenuA);
    } else {
        MenuSmartTextEx(apitxt_CONFIRMSAVE, 0.0f, -0.3f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUNORMALR, MENUNORMALG,
                        MENUNORMALB, 1.2f, 2, NULL, 0, MenuA);
    }
    menu->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_YES, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_NO, (u8)MenuA);
}

void MenuDrawAutoSaveWarning(MENU *menu) {
    Draw_AUTOSAVEWARNING();
    menu->draw_y = MENUBOTY;
    DrawMenuEntryEx(menu, apitxt_OK, (u8)MenuA);
}

extern "C" {

    void MenuDrawAutoSaveCancel(MENU *menu) {
        if (!MenuASCancelFinished && !memcard_savefailed) {
            menu->draw_y = MENUBOTY;
            DrawMenuEntryEx(menu, apitxt_OK, (u8)MenuA);
        }
    }
}

void MenuDrawLoad(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_LOADGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    if (drawslotsfn != NULL && MenuAlpha > (f64)0.2f && MenuStopDraw == 0) {
        drawslotsfn(menu, -0.2f);
    }
    ++menu->draw_item;
    menu->draw_y = MENUBOTY;
    DrawMenuEntryEx(menu, apitxt_CANCEL, (u8)MenuA);
}

void MenuDrawLoadConfirm(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_LOADGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    MenuText3DEx(apitxt_CONFIRMLOAD, 0.0f, -0.3f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUNORMALR, MENUNORMALG,
                 MENUNORMALB, (u8)MenuA);
    menu->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_YES, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_NO, (u8)MenuA);
}

void MenuDrawSaveCancel(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_SAVEGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    MenuSmartTextEx(apitxt_DOYOUWANTTOABORT, 0.0f, -0.3f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUNORMALR, MENUNORMALG,
                    MENUNORMALB, 1.5f, 2, NULL, 0, MenuA);
    menu->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_YES, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_NO, (u8)MenuA);
}

void MenuDrawLoadCancel(MENU *menu) {
    NuStrCpy(MenuHeader, apitxt_LOADGAME);
    header_r = MENUHEADERR;
    header_g = MENUHEADERG;
    header_b = MENUHEADERB;
    MenuSmartTextEx(apitxt_DOYOUWANTTOABORTLOAD, 0.0f, -0.3f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUNORMALR,
                    MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0, MenuA);
    menu->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_YES, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_NO, (u8)MenuA);
}

void MenuDrawFormatCancel(MENU *menu) {
    MenuSmartTextEx(apitxt_DOYOUWANTTOABORTFORMAT, 0.0f, -0.3f, 1.0f, MenuTextScale, MenuTextScale, MenuTextScale, 0, MENUNORMALR,
                    MENUNORMALG, MENUNORMALB, 1.5f, 2, NULL, 0, MenuA);
    menu->draw_y = MENUBOTY - MENUDY;
    DrawMenuEntryEx(menu, apitxt_YES, (u8)MenuA);
    DrawMenuEntryEx(menu, apitxt_NO, (u8)MenuA);
}

extern "C" void MenuDrawCardWarning(MENU *menu) {
    /* The shipping build draws no warning text; the buffer it was formatted in still gives the
       function its security cookie. */
    char text[8];
    if (0) {
        sprintf(text, "%d", menu->menu);
    }
}

extern "C" void MenuDrawVideo(MENU *menu) {
    char text[256];
    GRAPHICS_DISPLAY_MODE *mode = &PCDeviceBackend.modes[PCDeviceBackend.requested_mode];
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    NuSPrintf(text, "%s: %d x %d", MenuSettingsStrings[19], mode->width, mode->height);
    DrawMenuEntryEx(menu, text, (u8)MenuA);
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    NuSPrintf(text, "%s: %dHz", MenuSettingsStrings[20], mode->refresh_rate);
    DrawMenuEntryEx(menu, text, (u8)MenuA);
    switch (MenuVideoOptions.aspect_mode) {
        case PC_ASPECT_4_3:
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            NuSPrintf(text, "%s: 4:3", MenuSettingsStrings[22]);
            break;
        case PC_ASPECT_16_9:
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            NuSPrintf(text, "%s: 16:9", MenuSettingsStrings[22]);
            break;
        case PC_ASPECT_16_10:
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            NuSPrintf(text, "%s: 16:10", MenuSettingsStrings[22]);
            break;
        case PC_ASPECT_AUTO: {
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            const char *automatic = MenuSettingsStrings[24];
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            NuSPrintf(text, "%s: %s", MenuSettingsStrings[22], automatic);
            break;
        }
        default:
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            NuSPrintf(text, "%s: ERROR", MenuSettingsStrings[22]);
            break;
    }
    DrawMenuEntryEx(menu, text, (u8)MenuA);
    const char *value;
    if (MenuVideoOptions.hires_enabled) {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        value = MenuSettingsStrings[15];
    } else {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        value = MenuSettingsStrings[16];
    }
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    NuSPrintf(text, "%s: %s", MenuSettingsStrings[23], value);
    DrawMenuEntryEx(menu, text, (u8)((PcWindowOptions.option459 ? 255 : 64) * MenuAlpha));
    if (!PCDeviceBackend.HasRequestedModeChanged() && !MenuVideoOptions.Changed()) {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        NuSPrintf(text, "%s", MenuSettingsStrings[10]);
        DrawMenuEntryEx(menu, text, (u8)MenuA);
    } else {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        NuSPrintf(text, "%s", MenuSettingsStrings[8]);
        DrawMenuEntryEx(menu, text, (u8)((menu_flash ? 255 : 0) * MenuAlpha));
    }
}

extern "C" void MenuDrawEffects(MENU *menu) {
    char text[256];
    const char *value;
    if (PcWindowOptions.usehires) {
        if (PCDeviceBackend.requested_multisample == 6) {
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            value = MenuSettingsStrings[31];
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            NuSPrintf(text, "%s: %s", MenuSettingsStrings[30], value);
        } else {
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            value = MenuSettingsStrings[16];
            if (!MenuSettingsStringsInitialized) {
                MenuSettingsStringsInit();
            }
            NuSPrintf(text, "%s: %s", MenuSettingsStrings[30], value);
        }
        DrawMenuEntryEx(menu, text, (u8)(MenuAlpha * 64.0));
    } else {
        switch (PCDeviceBackend.requested_multisample) {
            case 1:
                if (!MenuSettingsStringsInitialized) {
                    MenuSettingsStringsInit();
                }
                NuSPrintf(text, "%s: 2X", MenuSettingsStrings[30]);
                break;
            case 2:
                if (!MenuSettingsStringsInitialized) {
                    MenuSettingsStringsInit();
                }
                NuSPrintf(text, "%s: 4X", MenuSettingsStrings[30]);
                break;
            case 3:
                if (!MenuSettingsStringsInitialized) {
                    MenuSettingsStringsInit();
                }
                NuSPrintf(text, "%s: 8X", MenuSettingsStrings[30]);
                break;
            case 4:
                if (!MenuSettingsStringsInitialized) {
                    MenuSettingsStringsInit();
                }
                NuSPrintf(text, "%s: 16X", MenuSettingsStrings[30]);
                break;
            case 6:
                if (!MenuSettingsStringsInitialized) {
                    MenuSettingsStringsInit();
                }
                value = MenuSettingsStrings[31];
                if (!MenuSettingsStringsInitialized) {
                    MenuSettingsStringsInit();
                }
                NuSPrintf(text, "%s: %s", MenuSettingsStrings[30], value);
                break;
            default:
                if (!MenuSettingsStringsInitialized) {
                    MenuSettingsStringsInit();
                }
                value = MenuSettingsStrings[16];
                if (!MenuSettingsStringsInitialized) {
                    MenuSettingsStringsInit();
                }
                NuSPrintf(text, "%s: %s", MenuSettingsStrings[30], value);
                break;
        }
        DrawMenuEntryEx(menu, text, (u8)((MenuCanChangeMultisample() ? 255 : 64) * MenuAlpha));
    }
    if (MenuCanChangeShadows() && MenuVideoOptions.shadows_enabled) {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        value = MenuSettingsStrings[15];
    } else {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        value = MenuSettingsStrings[16];
    }
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    NuSPrintf(text, "%s: %s", MenuSettingsStrings[32], value);
    DrawMenuEntryEx(menu, text, (u8)((MenuCanChangeShadows() ? 255 : 64) * MenuAlpha));
    if (MenuVideoOptions.bloom_enabled) {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        value = MenuSettingsStrings[15];
    } else {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        value = MenuSettingsStrings[16];
    }
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    NuSPrintf(text, "%s: %s", MenuSettingsStrings[33], value);
    DrawMenuEntryEx(menu, text, 255);
    if (MenuVideoOptions.speed_blur_enabled) {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        value = MenuSettingsStrings[15];
    } else {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        value = MenuSettingsStrings[16];
    }
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    NuSPrintf(text, "%s: %s", MenuSettingsStrings[35], value);
    DrawMenuEntryEx(menu, text, 255);
    if (!PCDeviceBackend.HasRequestedModeChanged() && !MenuVideoOptions.Changed()) {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        NuSPrintf(text, "%s", MenuSettingsStrings[10]);
        DrawMenuEntryEx(menu, text, (u8)MenuA);
    } else {
        if (!MenuSettingsStringsInitialized) {
            MenuSettingsStringsInit();
        }
        NuSPrintf(text, "%s", MenuSettingsStrings[8]);
        DrawMenuEntryEx(menu, text, (u8)((menu_flash ? 255 : 0) * MenuAlpha));
    }
}

extern "C" void MenuDrawVideoConfirm(MENU *menu) {
    char text[256];
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    DrawMenuEntryEx(menu, (char *)MenuSettingsStrings[25], (u8)MenuA);
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    NuSPrintf(text, "%s (%ds)", MenuSettingsStrings[27], (i32)(MenuVideoConfirmDuration - menu->menu_time) + 1);
    DrawMenuEntryEx(menu, text, (u8)MenuA);
}

extern "C" void MenuDrawHiresNotice(MENU *menu) {
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    MenuDrawNotice((char *)MenuSettingsStrings[28], 255, 255, 255);
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    DrawMenuEntryEx(menu, (char *)MenuSettingsStrings[13], (u8)MenuA);
}

extern "C" void MenuDrawQuit(MENU *menu) {
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    DrawMenuEntryEx(menu, (char *)MenuSettingsStrings[3], (u8)MenuA);
    if (!MenuSettingsStringsInitialized) {
        MenuSettingsStringsInit();
    }
    DrawMenuEntryEx(menu, (char *)MenuSettingsStrings[1], (u8)MenuA);
}
