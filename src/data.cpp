#include "batman.h"
#include "debug/log_copier.h"
#include "editor/aieditor_state.h"
#include "editor/edpath.h"
#include "editor/edpath_types.h"
#include "gameapi/ai/aisys/aisys.h"
#include "gameapi/ai/aisys/gameaiscript.h"
#include "gameapi/apisave.h"
#include "gameapi/apitext.h"
#include "gameapi/edbits.h"
#include "gameapi/edptl.h"
#include "gameapi/edtools/edanim_internal.h"
#include "gameapi/edtools/edbri_internal.h"
#include "gameapi/edtools/edgra.h"
#include "gameapi/edtools/edgra_internal.h"
#include "gameapi/edtools/edpp_internal.h"
#include "gameapi/edtools/edrender.h"
#include "gameapi/edtools/edstubs.h"
#include "gameapi/edtools/edui.h"
#include "gameapi/gui/apimenu.h"
#include "gameframework/saveload.h"
#include "gamelib/util/CRC16.h"
#include "gamelib/util/Refpack.h"
#include "globals.h"
#include "legoapi/actions/character/legoapi_tag.h"
#include "legoapi/actions/character/speederchase.h"
#include "legoapi/actions/character/suit.h"
#include "legoapi/actions/combat/fighting.h"
#include "legoapi/actions/movement/carrying.h"
#include "legoapi/actions/movement/dropinout.h"
#include "legoapi/actions/movement/jumping.h"
#include "legoapi/actions/movement/pushblocks.h"
#include "legoapi/ai/core/aisysall.h"
#include "legoapi/ai/core/gameai.h"
#include "legoapi/ai/core/legoai.h"
#include "legoapi/ai/game/creature.h"
#include "legoapi/ai/game/lsw_hub_ai.h"
#include "legoapi/ai/game/starwars_gameai.h"
#include "legoapi/audio/audio.h"
#include "legoapi/audio/sfx.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/characters.h"
#include "legoapi/characters/core/charconfig.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/characters/motion/animation_ids.h"
#include "legoapi/characters/motion/contexts.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/core/config/cheat.h"
#include "legoapi/core/config/cheats.h"
#include "legoapi/core/input/gamepads.h"
#include "legoapi/core/input/timer.h"
#include "legoapi/core/input/timing.h"
#include "legoapi/core/net/netplay.h"
#include "legoapi/core/startup/text.h"
#include "legoapi/cutscenes/gcutscn.h"
#include "legoapi/gizmo/base/gizactions.h"
#include "legoapi/gizmo/base/gizflow.h"
#include "legoapi/gizmo/base/gizmessage.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/gizmo/object/gizmopickup.h"
#include "legoapi/gizmos/door/spinner.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/gizmos/object/gizbuildits.h"
#include "legoapi/gizmos/object/gizobstacles.h"
#include "legoapi/gizmos/object/gizpanel.h"
#include "legoapi/gizmos/object/lever.h"
#include "legoapi/gizmos/transport/grapples.h"
#include "legoapi/gizmos/transport/teleport.h"
#include "legoapi/gizmos/traps/gizbombgen.h"
#include "legoapi/gizmos/traps/gizforce.h"
#include "legoapi/gizmos/traps/gizturret.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/base/collection.h"
#include "legoapi/items/collect/bolt.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/collect/spacelevel.h"
#include "legoapi/items/collect/thermaldetonators.h"
#include "legoapi/items/collect/torpedo.h"
#include "legoapi/items/objects/cable.h"
#include "legoapi/items/objects/game_obj.h"
#include "legoapi/items/objects/gameobjects.h"
#include "legoapi/items/objects/grabber.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/camera.h"
#include "legoapi/lib/hub.h"
#include "legoapi/lib/text.h"
#include "legoapi/menus/core/gamehint.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/menus/core/panel.h"
#include "legoapi/menus/core/text.h"
#include "legoapi/menus/screens/customise.h"
#include "legoapi/menus/screens/gamemenuall.h"
#include "legoapi/menus/screens/gamestatus_lsw.h"
#include "legoapi/menus/screens/gamestructure.h"
#include "legoapi/menus/screens/menu.h"
#include "legoapi/menus/screens/missions.h"
#include "legoapi/menus/screens/shop.h"
#include "legoapi/misc/gamemain.h"
#include "legoapi/misc/legoapi_status.h"
#include "legoapi/props/objects/zipup.h"
#include "legoapi/props/system/socksys.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/rtl.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/render/light/lighting.h"
#include "legoapi/render/light/surfaces.h"
#include "legoapi/text_tables.h"
#include "legoapi/world/area.h"
#include "legoapi/world/areas.h"
#include "legoapi/world/level.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/levels/episodeI.h"
#include "legoapi/world/levels/episodeII.h"
#include "legoapi/world/levels/episodeIII.h"
#include "legoapi/world/levels/episodeIV.h"
#include "legoapi/world/levels/episodeV.h"
#include "legoapi/world/levels/episodeVI.h"
#include "legoapi/world/levels/hub.h"
#include "legoapi/world/levels/levels.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/missions.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "legoapi/world/worldinfo.h"
#include "legogame/batman_gameai.h"
#include "legogame/game.h"
#include "legogame/pcbatman.h"
#include "legogame/startup.h"
#include "nu2api/PCInput.h"
#include "nu2api/huffdecode.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nushadermanager_gen.h"
#include "nu2api/nu3d/nushaderobject.h"
#include "nu2api/nu3d/nuspline.h"
#include "nu2api/nu3d/nutexanm.h"
#include "nu2api/nu3d/nuvport.h"
#include "nu2api/nucore/implode.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore/nuthread.h"
#include "nu2api/nudir_gen.h"
#include "nu2api/nuerror_gen.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/nufile_pc.h"
#include "nu2api/nuhgo_dlist.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nuprim_gen.h"
#include "nu2api/nusound/nusound.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nuwin32/Stubs.h"
#include "nu2api/nuwin32/d3dApiCalls.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/nu2api_windows.h"
#include "nu2api/nuwin32/pcapi.h"
#include "nu2api/platform_files.h"
#include "nu2api/platform_policy.h"

struct text_crawl_view_s {     // size 0x10
    i16 *heading_text;         // 0x0
    i16 *vehicle_bonus_text;   // 0x4
    i16 *character_bonus_text; // 0x8
    u8 paragraph_count;        // 0xc
    i8 area;                   // 0xd
    u8 unknown_0e[2];          // 0xe
};

extern "C" {

    i16 dat_007f1a4c = -1;
    i16 dat_007f1a48 = -1;
    struct text_crawl_view_s TextCrawlLsw = {
        &TextCrawlChapterId, &TextCrawlVehicleBonusId, &TextCrawlCharacterBonusId, 3, 0, {0, 0}
    };
}

extern "C" {
    extern const f64 LightingSurfaceFadeSlope = -0.33399999141693115;
}

extern "C" {
    u32 GAMEPAD_MENUCANCEL = PAD_BUTTON_TRIANGLE;
    u32 GAMEPAD_SELECT = PAD_BUTTON_SELECT;
}

extern "C" {
    LEVELDATA *dat_0087b1c0 = NULL;
    LEVELDATA *dat_0087b1d0 = NULL;
}

extern "C" {
    extern char dat_007516c0[];
    char *apiGameSubTitle = dat_007516c0;
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
}

extern "C" {
    char *AiLevelPathName = dat_007516c0;
}

extern "C" {
    extern const GUID PcPlatformPolicyClassId = {
        0x9a5ea990, 0x3034, 0x4d6f, {0x91, 0x28, 0x1, 0xf3, 0xc6, 0x10, 0x22, 0xbc}
    };
    extern const GUID PcPlatformPolicyInterfaceId = {
        0xe7b2fb72, 0xd728, 0x49b3, {0xa5, 0xf2, 0x18, 0xeb, 0xf5, 0xf1, 0x34, 0x9e}
    };
    const char *NuHwSoundDfsName = dat_007516c0;
    const char *NuHwDfsName = dat_007516c0;
    extern const CPU_VENDOR_WORDS CpuVendorAmd = {0x68747541u, 0x69746e65u, 0x444d4163u, {0}};
    extern const CPU_VENDOR_WORDS CpuVendorCyrix = {0x69727943u, 0x736e4978u, 0x64616574u, {0}};
    extern const CPU_VENDOR_WORDS CpuVendorNoCpuid = {0x6f4e5f5fu, 0x5550435fu, 0x5f5f4449u, {0}};
    extern const CPU_VENDOR_WORDS CpuVendorIntel = {0x756e6547u, 0x49656e69u, 0x6c65746eu, {0}};
    i32 PcWindowQuit = 0;
    i32 NuHwShowCommand = 0;
    i32 PcNoFocusPolicy = 0;
    char PcExecutableFilename[260] = {0};
}

extern const GUID PcWbemClassId = {
    0x4590f811, 0x1d3a, 0x11d0, {0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}
};
extern const GUID PcWbemInterfaceId = {
    0xdc12a687, 0x737f, 0x11cf, {0x88, 0x4d, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24}
};
extern const GUID PcSystemKeyboardId = {
    0x6f1d2b61, 0xd5a0, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID PcSystemMouseId = {
    0x6f1d2b60, 0xd5a0, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID dat_00784dfc = {
    0xa36d02f2, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID dat_00784e0c = {
    0x55728220, 0xd33c, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID dat_00784e2c = {
    0xa36d02e4, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID dat_00784e3c = {
    0xa36d02e3, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID dat_00784e4c = {
    0xa36d02f5, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID dat_00784e5c = {
    0xa36d02f4, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID dat_00784e6c = {
    0xa36d02e2, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID dat_00784e7c = {
    0xa36d02e1, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID dat_00784e8c = {
    0xa36d02e0, 0xc9f3, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}
};
extern const GUID PcDirectInput8AId = {
    0xbf798030, 0x483a, 0x4da2, {0xaa, 0x99, 0x5d, 0x64, 0xed, 0x36, 0x97, 0x00}
};
static const PC_DI_OBJECT_FORMAT PcKeyboardDataFormatObjects[256] = {
    {&dat_00784e0c, 0x0,  0x8000000c, 0x0},
    {&dat_00784e0c, 0x1,  0x8000010c, 0x0},
    {&dat_00784e0c, 0x2,  0x8000020c, 0x0},
    {&dat_00784e0c, 0x3,  0x8000030c, 0x0},
    {&dat_00784e0c, 0x4,  0x8000040c, 0x0},
    {&dat_00784e0c, 0x5,  0x8000050c, 0x0},
    {&dat_00784e0c, 0x6,  0x8000060c, 0x0},
    {&dat_00784e0c, 0x7,  0x8000070c, 0x0},
    {&dat_00784e0c, 0x8,  0x8000080c, 0x0},
    {&dat_00784e0c, 0x9,  0x8000090c, 0x0},
    {&dat_00784e0c, 0xa,  0x80000a0c, 0x0},
    {&dat_00784e0c, 0xb,  0x80000b0c, 0x0},
    {&dat_00784e0c, 0xc,  0x80000c0c, 0x0},
    {&dat_00784e0c, 0xd,  0x80000d0c, 0x0},
    {&dat_00784e0c, 0xe,  0x80000e0c, 0x0},
    {&dat_00784e0c, 0xf,  0x80000f0c, 0x0},
    {&dat_00784e0c, 0x10, 0x8000100c, 0x0},
    {&dat_00784e0c, 0x11, 0x8000110c, 0x0},
    {&dat_00784e0c, 0x12, 0x8000120c, 0x0},
    {&dat_00784e0c, 0x13, 0x8000130c, 0x0},
    {&dat_00784e0c, 0x14, 0x8000140c, 0x0},
    {&dat_00784e0c, 0x15, 0x8000150c, 0x0},
    {&dat_00784e0c, 0x16, 0x8000160c, 0x0},
    {&dat_00784e0c, 0x17, 0x8000170c, 0x0},
    {&dat_00784e0c, 0x18, 0x8000180c, 0x0},
    {&dat_00784e0c, 0x19, 0x8000190c, 0x0},
    {&dat_00784e0c, 0x1a, 0x80001a0c, 0x0},
    {&dat_00784e0c, 0x1b, 0x80001b0c, 0x0},
    {&dat_00784e0c, 0x1c, 0x80001c0c, 0x0},
    {&dat_00784e0c, 0x1d, 0x80001d0c, 0x0},
    {&dat_00784e0c, 0x1e, 0x80001e0c, 0x0},
    {&dat_00784e0c, 0x1f, 0x80001f0c, 0x0},
    {&dat_00784e0c, 0x20, 0x8000200c, 0x0},
    {&dat_00784e0c, 0x21, 0x8000210c, 0x0},
    {&dat_00784e0c, 0x22, 0x8000220c, 0x0},
    {&dat_00784e0c, 0x23, 0x8000230c, 0x0},
    {&dat_00784e0c, 0x24, 0x8000240c, 0x0},
    {&dat_00784e0c, 0x25, 0x8000250c, 0x0},
    {&dat_00784e0c, 0x26, 0x8000260c, 0x0},
    {&dat_00784e0c, 0x27, 0x8000270c, 0x0},
    {&dat_00784e0c, 0x28, 0x8000280c, 0x0},
    {&dat_00784e0c, 0x29, 0x8000290c, 0x0},
    {&dat_00784e0c, 0x2a, 0x80002a0c, 0x0},
    {&dat_00784e0c, 0x2b, 0x80002b0c, 0x0},
    {&dat_00784e0c, 0x2c, 0x80002c0c, 0x0},
    {&dat_00784e0c, 0x2d, 0x80002d0c, 0x0},
    {&dat_00784e0c, 0x2e, 0x80002e0c, 0x0},
    {&dat_00784e0c, 0x2f, 0x80002f0c, 0x0},
    {&dat_00784e0c, 0x30, 0x8000300c, 0x0},
    {&dat_00784e0c, 0x31, 0x8000310c, 0x0},
    {&dat_00784e0c, 0x32, 0x8000320c, 0x0},
    {&dat_00784e0c, 0x33, 0x8000330c, 0x0},
    {&dat_00784e0c, 0x34, 0x8000340c, 0x0},
    {&dat_00784e0c, 0x35, 0x8000350c, 0x0},
    {&dat_00784e0c, 0x36, 0x8000360c, 0x0},
    {&dat_00784e0c, 0x37, 0x8000370c, 0x0},
    {&dat_00784e0c, 0x38, 0x8000380c, 0x0},
    {&dat_00784e0c, 0x39, 0x8000390c, 0x0},
    {&dat_00784e0c, 0x3a, 0x80003a0c, 0x0},
    {&dat_00784e0c, 0x3b, 0x80003b0c, 0x0},
    {&dat_00784e0c, 0x3c, 0x80003c0c, 0x0},
    {&dat_00784e0c, 0x3d, 0x80003d0c, 0x0},
    {&dat_00784e0c, 0x3e, 0x80003e0c, 0x0},
    {&dat_00784e0c, 0x3f, 0x80003f0c, 0x0},
    {&dat_00784e0c, 0x40, 0x8000400c, 0x0},
    {&dat_00784e0c, 0x41, 0x8000410c, 0x0},
    {&dat_00784e0c, 0x42, 0x8000420c, 0x0},
    {&dat_00784e0c, 0x43, 0x8000430c, 0x0},
    {&dat_00784e0c, 0x44, 0x8000440c, 0x0},
    {&dat_00784e0c, 0x45, 0x8000450c, 0x0},
    {&dat_00784e0c, 0x46, 0x8000460c, 0x0},
    {&dat_00784e0c, 0x47, 0x8000470c, 0x0},
    {&dat_00784e0c, 0x48, 0x8000480c, 0x0},
    {&dat_00784e0c, 0x49, 0x8000490c, 0x0},
    {&dat_00784e0c, 0x4a, 0x80004a0c, 0x0},
    {&dat_00784e0c, 0x4b, 0x80004b0c, 0x0},
    {&dat_00784e0c, 0x4c, 0x80004c0c, 0x0},
    {&dat_00784e0c, 0x4d, 0x80004d0c, 0x0},
    {&dat_00784e0c, 0x4e, 0x80004e0c, 0x0},
    {&dat_00784e0c, 0x4f, 0x80004f0c, 0x0},
    {&dat_00784e0c, 0x50, 0x8000500c, 0x0},
    {&dat_00784e0c, 0x51, 0x8000510c, 0x0},
    {&dat_00784e0c, 0x52, 0x8000520c, 0x0},
    {&dat_00784e0c, 0x53, 0x8000530c, 0x0},
    {&dat_00784e0c, 0x54, 0x8000540c, 0x0},
    {&dat_00784e0c, 0x55, 0x8000550c, 0x0},
    {&dat_00784e0c, 0x56, 0x8000560c, 0x0},
    {&dat_00784e0c, 0x57, 0x8000570c, 0x0},
    {&dat_00784e0c, 0x58, 0x8000580c, 0x0},
    {&dat_00784e0c, 0x59, 0x8000590c, 0x0},
    {&dat_00784e0c, 0x5a, 0x80005a0c, 0x0},
    {&dat_00784e0c, 0x5b, 0x80005b0c, 0x0},
    {&dat_00784e0c, 0x5c, 0x80005c0c, 0x0},
    {&dat_00784e0c, 0x5d, 0x80005d0c, 0x0},
    {&dat_00784e0c, 0x5e, 0x80005e0c, 0x0},
    {&dat_00784e0c, 0x5f, 0x80005f0c, 0x0},
    {&dat_00784e0c, 0x60, 0x8000600c, 0x0},
    {&dat_00784e0c, 0x61, 0x8000610c, 0x0},
    {&dat_00784e0c, 0x62, 0x8000620c, 0x0},
    {&dat_00784e0c, 0x63, 0x8000630c, 0x0},
    {&dat_00784e0c, 0x64, 0x8000640c, 0x0},
    {&dat_00784e0c, 0x65, 0x8000650c, 0x0},
    {&dat_00784e0c, 0x66, 0x8000660c, 0x0},
    {&dat_00784e0c, 0x67, 0x8000670c, 0x0},
    {&dat_00784e0c, 0x68, 0x8000680c, 0x0},
    {&dat_00784e0c, 0x69, 0x8000690c, 0x0},
    {&dat_00784e0c, 0x6a, 0x80006a0c, 0x0},
    {&dat_00784e0c, 0x6b, 0x80006b0c, 0x0},
    {&dat_00784e0c, 0x6c, 0x80006c0c, 0x0},
    {&dat_00784e0c, 0x6d, 0x80006d0c, 0x0},
    {&dat_00784e0c, 0x6e, 0x80006e0c, 0x0},
    {&dat_00784e0c, 0x6f, 0x80006f0c, 0x0},
    {&dat_00784e0c, 0x70, 0x8000700c, 0x0},
    {&dat_00784e0c, 0x71, 0x8000710c, 0x0},
    {&dat_00784e0c, 0x72, 0x8000720c, 0x0},
    {&dat_00784e0c, 0x73, 0x8000730c, 0x0},
    {&dat_00784e0c, 0x74, 0x8000740c, 0x0},
    {&dat_00784e0c, 0x75, 0x8000750c, 0x0},
    {&dat_00784e0c, 0x76, 0x8000760c, 0x0},
    {&dat_00784e0c, 0x77, 0x8000770c, 0x0},
    {&dat_00784e0c, 0x78, 0x8000780c, 0x0},
    {&dat_00784e0c, 0x79, 0x8000790c, 0x0},
    {&dat_00784e0c, 0x7a, 0x80007a0c, 0x0},
    {&dat_00784e0c, 0x7b, 0x80007b0c, 0x0},
    {&dat_00784e0c, 0x7c, 0x80007c0c, 0x0},
    {&dat_00784e0c, 0x7d, 0x80007d0c, 0x0},
    {&dat_00784e0c, 0x7e, 0x80007e0c, 0x0},
    {&dat_00784e0c, 0x7f, 0x80007f0c, 0x0},
    {&dat_00784e0c, 0x80, 0x8000800c, 0x0},
    {&dat_00784e0c, 0x81, 0x8000810c, 0x0},
    {&dat_00784e0c, 0x82, 0x8000820c, 0x0},
    {&dat_00784e0c, 0x83, 0x8000830c, 0x0},
    {&dat_00784e0c, 0x84, 0x8000840c, 0x0},
    {&dat_00784e0c, 0x85, 0x8000850c, 0x0},
    {&dat_00784e0c, 0x86, 0x8000860c, 0x0},
    {&dat_00784e0c, 0x87, 0x8000870c, 0x0},
    {&dat_00784e0c, 0x88, 0x8000880c, 0x0},
    {&dat_00784e0c, 0x89, 0x8000890c, 0x0},
    {&dat_00784e0c, 0x8a, 0x80008a0c, 0x0},
    {&dat_00784e0c, 0x8b, 0x80008b0c, 0x0},
    {&dat_00784e0c, 0x8c, 0x80008c0c, 0x0},
    {&dat_00784e0c, 0x8d, 0x80008d0c, 0x0},
    {&dat_00784e0c, 0x8e, 0x80008e0c, 0x0},
    {&dat_00784e0c, 0x8f, 0x80008f0c, 0x0},
    {&dat_00784e0c, 0x90, 0x8000900c, 0x0},
    {&dat_00784e0c, 0x91, 0x8000910c, 0x0},
    {&dat_00784e0c, 0x92, 0x8000920c, 0x0},
    {&dat_00784e0c, 0x93, 0x8000930c, 0x0},
    {&dat_00784e0c, 0x94, 0x8000940c, 0x0},
    {&dat_00784e0c, 0x95, 0x8000950c, 0x0},
    {&dat_00784e0c, 0x96, 0x8000960c, 0x0},
    {&dat_00784e0c, 0x97, 0x8000970c, 0x0},
    {&dat_00784e0c, 0x98, 0x8000980c, 0x0},
    {&dat_00784e0c, 0x99, 0x8000990c, 0x0},
    {&dat_00784e0c, 0x9a, 0x80009a0c, 0x0},
    {&dat_00784e0c, 0x9b, 0x80009b0c, 0x0},
    {&dat_00784e0c, 0x9c, 0x80009c0c, 0x0},
    {&dat_00784e0c, 0x9d, 0x80009d0c, 0x0},
    {&dat_00784e0c, 0x9e, 0x80009e0c, 0x0},
    {&dat_00784e0c, 0x9f, 0x80009f0c, 0x0},
    {&dat_00784e0c, 0xa0, 0x8000a00c, 0x0},
    {&dat_00784e0c, 0xa1, 0x8000a10c, 0x0},
    {&dat_00784e0c, 0xa2, 0x8000a20c, 0x0},
    {&dat_00784e0c, 0xa3, 0x8000a30c, 0x0},
    {&dat_00784e0c, 0xa4, 0x8000a40c, 0x0},
    {&dat_00784e0c, 0xa5, 0x8000a50c, 0x0},
    {&dat_00784e0c, 0xa6, 0x8000a60c, 0x0},
    {&dat_00784e0c, 0xa7, 0x8000a70c, 0x0},
    {&dat_00784e0c, 0xa8, 0x8000a80c, 0x0},
    {&dat_00784e0c, 0xa9, 0x8000a90c, 0x0},
    {&dat_00784e0c, 0xaa, 0x8000aa0c, 0x0},
    {&dat_00784e0c, 0xab, 0x8000ab0c, 0x0},
    {&dat_00784e0c, 0xac, 0x8000ac0c, 0x0},
    {&dat_00784e0c, 0xad, 0x8000ad0c, 0x0},
    {&dat_00784e0c, 0xae, 0x8000ae0c, 0x0},
    {&dat_00784e0c, 0xaf, 0x8000af0c, 0x0},
    {&dat_00784e0c, 0xb0, 0x8000b00c, 0x0},
    {&dat_00784e0c, 0xb1, 0x8000b10c, 0x0},
    {&dat_00784e0c, 0xb2, 0x8000b20c, 0x0},
    {&dat_00784e0c, 0xb3, 0x8000b30c, 0x0},
    {&dat_00784e0c, 0xb4, 0x8000b40c, 0x0},
    {&dat_00784e0c, 0xb5, 0x8000b50c, 0x0},
    {&dat_00784e0c, 0xb6, 0x8000b60c, 0x0},
    {&dat_00784e0c, 0xb7, 0x8000b70c, 0x0},
    {&dat_00784e0c, 0xb8, 0x8000b80c, 0x0},
    {&dat_00784e0c, 0xb9, 0x8000b90c, 0x0},
    {&dat_00784e0c, 0xba, 0x8000ba0c, 0x0},
    {&dat_00784e0c, 0xbb, 0x8000bb0c, 0x0},
    {&dat_00784e0c, 0xbc, 0x8000bc0c, 0x0},
    {&dat_00784e0c, 0xbd, 0x8000bd0c, 0x0},
    {&dat_00784e0c, 0xbe, 0x8000be0c, 0x0},
    {&dat_00784e0c, 0xbf, 0x8000bf0c, 0x0},
    {&dat_00784e0c, 0xc0, 0x8000c00c, 0x0},
    {&dat_00784e0c, 0xc1, 0x8000c10c, 0x0},
    {&dat_00784e0c, 0xc2, 0x8000c20c, 0x0},
    {&dat_00784e0c, 0xc3, 0x8000c30c, 0x0},
    {&dat_00784e0c, 0xc4, 0x8000c40c, 0x0},
    {&dat_00784e0c, 0xc5, 0x8000c50c, 0x0},
    {&dat_00784e0c, 0xc6, 0x8000c60c, 0x0},
    {&dat_00784e0c, 0xc7, 0x8000c70c, 0x0},
    {&dat_00784e0c, 0xc8, 0x8000c80c, 0x0},
    {&dat_00784e0c, 0xc9, 0x8000c90c, 0x0},
    {&dat_00784e0c, 0xca, 0x8000ca0c, 0x0},
    {&dat_00784e0c, 0xcb, 0x8000cb0c, 0x0},
    {&dat_00784e0c, 0xcc, 0x8000cc0c, 0x0},
    {&dat_00784e0c, 0xcd, 0x8000cd0c, 0x0},
    {&dat_00784e0c, 0xce, 0x8000ce0c, 0x0},
    {&dat_00784e0c, 0xcf, 0x8000cf0c, 0x0},
    {&dat_00784e0c, 0xd0, 0x8000d00c, 0x0},
    {&dat_00784e0c, 0xd1, 0x8000d10c, 0x0},
    {&dat_00784e0c, 0xd2, 0x8000d20c, 0x0},
    {&dat_00784e0c, 0xd3, 0x8000d30c, 0x0},
    {&dat_00784e0c, 0xd4, 0x8000d40c, 0x0},
    {&dat_00784e0c, 0xd5, 0x8000d50c, 0x0},
    {&dat_00784e0c, 0xd6, 0x8000d60c, 0x0},
    {&dat_00784e0c, 0xd7, 0x8000d70c, 0x0},
    {&dat_00784e0c, 0xd8, 0x8000d80c, 0x0},
    {&dat_00784e0c, 0xd9, 0x8000d90c, 0x0},
    {&dat_00784e0c, 0xda, 0x8000da0c, 0x0},
    {&dat_00784e0c, 0xdb, 0x8000db0c, 0x0},
    {&dat_00784e0c, 0xdc, 0x8000dc0c, 0x0},
    {&dat_00784e0c, 0xdd, 0x8000dd0c, 0x0},
    {&dat_00784e0c, 0xde, 0x8000de0c, 0x0},
    {&dat_00784e0c, 0xdf, 0x8000df0c, 0x0},
    {&dat_00784e0c, 0xe0, 0x8000e00c, 0x0},
    {&dat_00784e0c, 0xe1, 0x8000e10c, 0x0},
    {&dat_00784e0c, 0xe2, 0x8000e20c, 0x0},
    {&dat_00784e0c, 0xe3, 0x8000e30c, 0x0},
    {&dat_00784e0c, 0xe4, 0x8000e40c, 0x0},
    {&dat_00784e0c, 0xe5, 0x8000e50c, 0x0},
    {&dat_00784e0c, 0xe6, 0x8000e60c, 0x0},
    {&dat_00784e0c, 0xe7, 0x8000e70c, 0x0},
    {&dat_00784e0c, 0xe8, 0x8000e80c, 0x0},
    {&dat_00784e0c, 0xe9, 0x8000e90c, 0x0},
    {&dat_00784e0c, 0xea, 0x8000ea0c, 0x0},
    {&dat_00784e0c, 0xeb, 0x8000eb0c, 0x0},
    {&dat_00784e0c, 0xec, 0x8000ec0c, 0x0},
    {&dat_00784e0c, 0xed, 0x8000ed0c, 0x0},
    {&dat_00784e0c, 0xee, 0x8000ee0c, 0x0},
    {&dat_00784e0c, 0xef, 0x8000ef0c, 0x0},
    {&dat_00784e0c, 0xf0, 0x8000f00c, 0x0},
    {&dat_00784e0c, 0xf1, 0x8000f10c, 0x0},
    {&dat_00784e0c, 0xf2, 0x8000f20c, 0x0},
    {&dat_00784e0c, 0xf3, 0x8000f30c, 0x0},
    {&dat_00784e0c, 0xf4, 0x8000f40c, 0x0},
    {&dat_00784e0c, 0xf5, 0x8000f50c, 0x0},
    {&dat_00784e0c, 0xf6, 0x8000f60c, 0x0},
    {&dat_00784e0c, 0xf7, 0x8000f70c, 0x0},
    {&dat_00784e0c, 0xf8, 0x8000f80c, 0x0},
    {&dat_00784e0c, 0xf9, 0x8000f90c, 0x0},
    {&dat_00784e0c, 0xfa, 0x8000fa0c, 0x0},
    {&dat_00784e0c, 0xfb, 0x8000fb0c, 0x0},
    {&dat_00784e0c, 0xfc, 0x8000fc0c, 0x0},
    {&dat_00784e0c, 0xfd, 0x8000fd0c, 0x0},
    {&dat_00784e0c, 0xfe, 0x8000fe0c, 0x0},
    {&dat_00784e0c, 0xff, 0x8000ff0c, 0x0},
};
extern const PC_DI_DATA_FORMAT PcKeyboardDataFormat = {24, 16, 2, 256, 256, PcKeyboardDataFormatObjects};
static const PC_DI_OBJECT_FORMAT PcJoystickDataFormatObjects[164] = {
    {&dat_00784e8c, 0x0,   0x80ffff03, 0x100},
    {&dat_00784e7c, 0x4,   0x80ffff03, 0x100},
    {&dat_00784e6c, 0x8,   0x80ffff03, 0x100},
    {&dat_00784e5c, 0xc,   0x80ffff03, 0x100},
    {&dat_00784e4c, 0x10,  0x80ffff03, 0x100},
    {&dat_00784e3c, 0x14,  0x80ffff03, 0x100},
    {&dat_00784e2c, 0x18,  0x80ffff03, 0x100},
    {&dat_00784e2c, 0x1c,  0x80ffff03, 0x100},
    {&dat_00784dfc, 0x20,  0x80ffff10, 0x0  },
    {&dat_00784dfc, 0x24,  0x80ffff10, 0x0  },
    {&dat_00784dfc, 0x28,  0x80ffff10, 0x0  },
    {&dat_00784dfc, 0x2c,  0x80ffff10, 0x0  },
    {NULL,          0x30,  0x80ffff0c, 0x0  },
    {NULL,          0x31,  0x80ffff0c, 0x0  },
    {NULL,          0x32,  0x80ffff0c, 0x0  },
    {NULL,          0x33,  0x80ffff0c, 0x0  },
    {NULL,          0x34,  0x80ffff0c, 0x0  },
    {NULL,          0x35,  0x80ffff0c, 0x0  },
    {NULL,          0x36,  0x80ffff0c, 0x0  },
    {NULL,          0x37,  0x80ffff0c, 0x0  },
    {NULL,          0x38,  0x80ffff0c, 0x0  },
    {NULL,          0x39,  0x80ffff0c, 0x0  },
    {NULL,          0x3a,  0x80ffff0c, 0x0  },
    {NULL,          0x3b,  0x80ffff0c, 0x0  },
    {NULL,          0x3c,  0x80ffff0c, 0x0  },
    {NULL,          0x3d,  0x80ffff0c, 0x0  },
    {NULL,          0x3e,  0x80ffff0c, 0x0  },
    {NULL,          0x3f,  0x80ffff0c, 0x0  },
    {NULL,          0x40,  0x80ffff0c, 0x0  },
    {NULL,          0x41,  0x80ffff0c, 0x0  },
    {NULL,          0x42,  0x80ffff0c, 0x0  },
    {NULL,          0x43,  0x80ffff0c, 0x0  },
    {NULL,          0x44,  0x80ffff0c, 0x0  },
    {NULL,          0x45,  0x80ffff0c, 0x0  },
    {NULL,          0x46,  0x80ffff0c, 0x0  },
    {NULL,          0x47,  0x80ffff0c, 0x0  },
    {NULL,          0x48,  0x80ffff0c, 0x0  },
    {NULL,          0x49,  0x80ffff0c, 0x0  },
    {NULL,          0x4a,  0x80ffff0c, 0x0  },
    {NULL,          0x4b,  0x80ffff0c, 0x0  },
    {NULL,          0x4c,  0x80ffff0c, 0x0  },
    {NULL,          0x4d,  0x80ffff0c, 0x0  },
    {NULL,          0x4e,  0x80ffff0c, 0x0  },
    {NULL,          0x4f,  0x80ffff0c, 0x0  },
    {NULL,          0x50,  0x80ffff0c, 0x0  },
    {NULL,          0x51,  0x80ffff0c, 0x0  },
    {NULL,          0x52,  0x80ffff0c, 0x0  },
    {NULL,          0x53,  0x80ffff0c, 0x0  },
    {NULL,          0x54,  0x80ffff0c, 0x0  },
    {NULL,          0x55,  0x80ffff0c, 0x0  },
    {NULL,          0x56,  0x80ffff0c, 0x0  },
    {NULL,          0x57,  0x80ffff0c, 0x0  },
    {NULL,          0x58,  0x80ffff0c, 0x0  },
    {NULL,          0x59,  0x80ffff0c, 0x0  },
    {NULL,          0x5a,  0x80ffff0c, 0x0  },
    {NULL,          0x5b,  0x80ffff0c, 0x0  },
    {NULL,          0x5c,  0x80ffff0c, 0x0  },
    {NULL,          0x5d,  0x80ffff0c, 0x0  },
    {NULL,          0x5e,  0x80ffff0c, 0x0  },
    {NULL,          0x5f,  0x80ffff0c, 0x0  },
    {NULL,          0x60,  0x80ffff0c, 0x0  },
    {NULL,          0x61,  0x80ffff0c, 0x0  },
    {NULL,          0x62,  0x80ffff0c, 0x0  },
    {NULL,          0x63,  0x80ffff0c, 0x0  },
    {NULL,          0x64,  0x80ffff0c, 0x0  },
    {NULL,          0x65,  0x80ffff0c, 0x0  },
    {NULL,          0x66,  0x80ffff0c, 0x0  },
    {NULL,          0x67,  0x80ffff0c, 0x0  },
    {NULL,          0x68,  0x80ffff0c, 0x0  },
    {NULL,          0x69,  0x80ffff0c, 0x0  },
    {NULL,          0x6a,  0x80ffff0c, 0x0  },
    {NULL,          0x6b,  0x80ffff0c, 0x0  },
    {NULL,          0x6c,  0x80ffff0c, 0x0  },
    {NULL,          0x6d,  0x80ffff0c, 0x0  },
    {NULL,          0x6e,  0x80ffff0c, 0x0  },
    {NULL,          0x6f,  0x80ffff0c, 0x0  },
    {NULL,          0x70,  0x80ffff0c, 0x0  },
    {NULL,          0x71,  0x80ffff0c, 0x0  },
    {NULL,          0x72,  0x80ffff0c, 0x0  },
    {NULL,          0x73,  0x80ffff0c, 0x0  },
    {NULL,          0x74,  0x80ffff0c, 0x0  },
    {NULL,          0x75,  0x80ffff0c, 0x0  },
    {NULL,          0x76,  0x80ffff0c, 0x0  },
    {NULL,          0x77,  0x80ffff0c, 0x0  },
    {NULL,          0x78,  0x80ffff0c, 0x0  },
    {NULL,          0x79,  0x80ffff0c, 0x0  },
    {NULL,          0x7a,  0x80ffff0c, 0x0  },
    {NULL,          0x7b,  0x80ffff0c, 0x0  },
    {NULL,          0x7c,  0x80ffff0c, 0x0  },
    {NULL,          0x7d,  0x80ffff0c, 0x0  },
    {NULL,          0x7e,  0x80ffff0c, 0x0  },
    {NULL,          0x7f,  0x80ffff0c, 0x0  },
    {NULL,          0x80,  0x80ffff0c, 0x0  },
    {NULL,          0x81,  0x80ffff0c, 0x0  },
    {NULL,          0x82,  0x80ffff0c, 0x0  },
    {NULL,          0x83,  0x80ffff0c, 0x0  },
    {NULL,          0x84,  0x80ffff0c, 0x0  },
    {NULL,          0x85,  0x80ffff0c, 0x0  },
    {NULL,          0x86,  0x80ffff0c, 0x0  },
    {NULL,          0x87,  0x80ffff0c, 0x0  },
    {NULL,          0x88,  0x80ffff0c, 0x0  },
    {NULL,          0x89,  0x80ffff0c, 0x0  },
    {NULL,          0x8a,  0x80ffff0c, 0x0  },
    {NULL,          0x8b,  0x80ffff0c, 0x0  },
    {NULL,          0x8c,  0x80ffff0c, 0x0  },
    {NULL,          0x8d,  0x80ffff0c, 0x0  },
    {NULL,          0x8e,  0x80ffff0c, 0x0  },
    {NULL,          0x8f,  0x80ffff0c, 0x0  },
    {NULL,          0x90,  0x80ffff0c, 0x0  },
    {NULL,          0x91,  0x80ffff0c, 0x0  },
    {NULL,          0x92,  0x80ffff0c, 0x0  },
    {NULL,          0x93,  0x80ffff0c, 0x0  },
    {NULL,          0x94,  0x80ffff0c, 0x0  },
    {NULL,          0x95,  0x80ffff0c, 0x0  },
    {NULL,          0x96,  0x80ffff0c, 0x0  },
    {NULL,          0x97,  0x80ffff0c, 0x0  },
    {NULL,          0x98,  0x80ffff0c, 0x0  },
    {NULL,          0x99,  0x80ffff0c, 0x0  },
    {NULL,          0x9a,  0x80ffff0c, 0x0  },
    {NULL,          0x9b,  0x80ffff0c, 0x0  },
    {NULL,          0x9c,  0x80ffff0c, 0x0  },
    {NULL,          0x9d,  0x80ffff0c, 0x0  },
    {NULL,          0x9e,  0x80ffff0c, 0x0  },
    {NULL,          0x9f,  0x80ffff0c, 0x0  },
    {NULL,          0xa0,  0x80ffff0c, 0x0  },
    {NULL,          0xa1,  0x80ffff0c, 0x0  },
    {NULL,          0xa2,  0x80ffff0c, 0x0  },
    {NULL,          0xa3,  0x80ffff0c, 0x0  },
    {NULL,          0xa4,  0x80ffff0c, 0x0  },
    {NULL,          0xa5,  0x80ffff0c, 0x0  },
    {NULL,          0xa6,  0x80ffff0c, 0x0  },
    {NULL,          0xa7,  0x80ffff0c, 0x0  },
    {NULL,          0xa8,  0x80ffff0c, 0x0  },
    {NULL,          0xa9,  0x80ffff0c, 0x0  },
    {NULL,          0xaa,  0x80ffff0c, 0x0  },
    {NULL,          0xab,  0x80ffff0c, 0x0  },
    {NULL,          0xac,  0x80ffff0c, 0x0  },
    {NULL,          0xad,  0x80ffff0c, 0x0  },
    {NULL,          0xae,  0x80ffff0c, 0x0  },
    {NULL,          0xaf,  0x80ffff0c, 0x0  },
    {&dat_00784e8c, 0xb0,  0x80ffff03, 0x200},
    {&dat_00784e7c, 0xb4,  0x80ffff03, 0x200},
    {&dat_00784e6c, 0xb8,  0x80ffff03, 0x200},
    {&dat_00784e5c, 0xbc,  0x80ffff03, 0x200},
    {&dat_00784e4c, 0xc0,  0x80ffff03, 0x200},
    {&dat_00784e3c, 0xc4,  0x80ffff03, 0x200},
    {&dat_00784e2c, 0x18,  0x80ffff03, 0x200},
    {&dat_00784e2c, 0x1c,  0x80ffff03, 0x200},
    {&dat_00784e8c, 0xd0,  0x80ffff03, 0x300},
    {&dat_00784e7c, 0xd4,  0x80ffff03, 0x300},
    {&dat_00784e6c, 0xd8,  0x80ffff03, 0x300},
    {&dat_00784e5c, 0xdc,  0x80ffff03, 0x300},
    {&dat_00784e4c, 0xe0,  0x80ffff03, 0x300},
    {&dat_00784e3c, 0xe4,  0x80ffff03, 0x300},
    {&dat_00784e2c, 0x18,  0x80ffff03, 0x300},
    {&dat_00784e2c, 0x1c,  0x80ffff03, 0x300},
    {&dat_00784e8c, 0xf0,  0x80ffff03, 0x400},
    {&dat_00784e7c, 0xf4,  0x80ffff03, 0x400},
    {&dat_00784e6c, 0xf8,  0x80ffff03, 0x400},
    {&dat_00784e5c, 0xfc,  0x80ffff03, 0x400},
    {&dat_00784e4c, 0x100, 0x80ffff03, 0x400},
    {&dat_00784e3c, 0x104, 0x80ffff03, 0x400},
    {&dat_00784e2c, 0x18,  0x80ffff03, 0x400},
    {&dat_00784e2c, 0x1c,  0x80ffff03, 0x400},
};
extern const PC_DI_DATA_FORMAT PcJoystickDataFormat = {24, 16, 1, 272, 164, PcJoystickDataFormatObjects};
static const PC_DI_OBJECT_FORMAT PcMouseDataFormatObjects[11] = {
    {&dat_00784e8c, 0x0,  0xffff03,   0x0},
    {&dat_00784e7c, 0x4,  0xffff03,   0x0},
    {&dat_00784e6c, 0x8,  0x80ffff03, 0x0},
    {NULL,          0xc,  0xffff0c,   0x0},
    {NULL,          0xd,  0xffff0c,   0x0},
    {NULL,          0xe,  0x80ffff0c, 0x0},
    {NULL,          0xf,  0x80ffff0c, 0x0},
    {NULL,          0x10, 0x80ffff0c, 0x0},
    {NULL,          0x11, 0x80ffff0c, 0x0},
    {NULL,          0x12, 0x80ffff0c, 0x0},
    {NULL,          0x13, 0x80ffff0c, 0x0},
};
extern const PC_DI_DATA_FORMAT PcMouseDataFormat = {24, 16, 2, 20, 11, PcMouseDataFormatObjects};
// clang-format off: nested arrays wider than the column limit
PC_INPUT_CONFIG PcStaticInputConfigs[24] = {
    {"USB Joystick     ",
     0x30e8f,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"XBOX 360 For Windows (Controller)",
     0x28e045e,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0a, 0x00, 0x01, 0x00, 0x05, 0x00},
     16,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x03, 0x03}, {0x02, 0x04, 0x04}},
       {{0x00, 0x00, 0x00}, {0x02, 0x06, 0x06}, {0x00, 0x00, 0x00}, {0x02, 0x07, 0x07}}}},
     {{0x00, 0x00},
      {0x00, 0x00},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x02},
      {0x01, 0x06},
      {0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x07},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Controller (Xbox 360 Wireless Receiver for Windows)",
     0x719045e,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0a, 0x00, 0x01, 0x00, 0x05, 0x00},
     16,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x03, 0x03}, {0x02, 0x04, 0x04}},
       {{0x00, 0x00, 0x00}, {0x02, 0x06, 0x06}, {0x00, 0x00, 0x00}, {0x02, 0x07, 0x07}}}},
     {{0x00, 0x00},
      {0x00, 0x00},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x02},
      {0x01, 0x06},
      {0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x07},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"PS Vibration Feedback Converter ",
     0x50c12,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x08},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Rockfire PSX\\USB Bridge",
     0x20500583,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x06, 0x06}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x02},
      {0x01, 0x0a},
      {0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x0b},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Logic3 12 button vibration",
     0x31207b5,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x01},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Interact Gaming Device",
     0x262a05fd,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0a, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x02, 0x05, 0x05}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x01},
      {0x01, 0x04},
      {0x01, 0x03},
      {0x01, 0x00},
      {0x01, 0x0a},
      {0x01, 0x02},
      {0x01, 0x05},
      {0x01, 0x0b},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Keyboard",
     0x1,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
     0,
     0,
     5,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x0a, 0x1e, 0x20}, {0x0a, 0x11, 0x1f}, {0x0a, 0xcb, 0xcd}, {0x0a, 0xd0, 0xc8}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x07, 0x10},
      {0x07, 0x49},
      {0x07, 0x02},
      {0x07, 0x37},
      {0x04, 0x02},
      {0x04, 0x01},
      {0x04, 0x00},
      {0x07, 0x52},
      {0x07, 0x3b},
      {0x07, 0x1d},
      {0x07, 0x9d},
      {0x07, 0x3c},
      {0x07, 0xc8},
      {0x07, 0xcd},
      {0x07, 0xd0},
      {0x07, 0xcb}}},
    {"Keyboard",
     0x2,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
     0,
     0,
     5,
     0,
     1,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x0a, 0x1e, 0x20}, {0x0a, 0x11, 0x1f}, {0x0a, 0xcb, 0xcd}, {0x0a, 0xd0, 0xc8}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x07, 0x10},
      {0x07, 0x49},
      {0x07, 0x02},
      {0x07, 0x37},
      {0x04, 0x02},
      {0x04, 0x01},
      {0x04, 0x00},
      {0x07, 0x52},
      {0x07, 0x3b},
      {0x07, 0x1d},
      {0x07, 0x9d},
      {0x07, 0x3c},
      {0x07, 0xc8},
      {0x07, 0xcd},
      {0x07, 0xd0},
      {0x07, 0xcb}}},
    {"Keyboard",
     0x3,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
     0,
     0,
     5,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x0a, 0x1e, 0x20}, {0x0a, 0x11, 0x1f}, {0x0a, 0xcb, 0xcd}, {0x0a, 0xd0, 0xc8}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x07, 0x10},
      {0x07, 0x12},
      {0x07, 0x02},
      {0x07, 0x04},
      {0x07, 0x48},
      {0x07, 0x4d},
      {0x07, 0x50},
      {0x07, 0x4b},
      {0x07, 0x3b},
      {0x07, 0x1d},
      {0x07, 0x9d},
      {0x07, 0x3c},
      {0x07, 0x14},
      {0x07, 0x23},
      {0x07, 0x22},
      {0x07, 0x21}}},
    {"Keyboard",
     0x4,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
     0,
     0,
     5,
     0,
     1,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x0a, 0x1e, 0x20}, {0x0a, 0x11, 0x1f}, {0x0a, 0xcb, 0xcd}, {0x0a, 0xd0, 0xc8}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x07, 0x10},
      {0x07, 0x49},
      {0x07, 0x02},
      {0x07, 0x37},
      {0x04, 0x02},
      {0x04, 0x01},
      {0x04, 0x00},
      {0x07, 0x52},
      {0x07, 0x3b},
      {0x07, 0x1d},
      {0x07, 0x9d},
      {0x07, 0x3c},
      {0x07, 0xc8},
      {0x07, 0xcd},
      {0x07, 0xd0},
      {0x07, 0xcb}}},
    {"PLAYSTATION(R)3 Controller",
     0x268054c,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x02, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x00, 0x05, 0x05}, {0x00, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x0c},
      {0x01, 0x0d},
      {0x01, 0x0e},
      {0x01, 0x0f},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07}}},
    {"Saitek P2900 Wireless Pad",
     0x40c06a3,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0e, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x08},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"4 axis 16 button joystick",
     0x6676666,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x10, 0x00, 0x00, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x02, 0x05, 0x05}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x08},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x0c},
      {0x01, 0x0d},
      {0x01, 0x0e},
      {0x01, 0x0f}}},
    {"MP-8888 USB Joypad",
     0x88880925,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x02, 0x05, 0x05}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x02},
      {0x01, 0x03},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x08},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"2 in 1 DT",
     0xb320044f,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x06, 0x06}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Logitech RumblePad 2 USB",
     0xc218046d,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     10,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x02, 0x05, 0x05}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Logitech Dual Action",
     0xc216046d,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     8,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x02, 0x02}, {0x02, 0x05, 0x05}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Dual Analog Rumble Pad",
     0x1100f30,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     16,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x01},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Thrustmaster vibrating gamepad",
     0xb312044f,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x06, 0x06}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"4-Axis,12-Button with POV",
     0xa0090583,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x06, 0x06}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Saitek P2600 Rumble Force Pad",
     0x5f0d06a3,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x06},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x05},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x01},
      {0x01, 0x00},
      {0x01, 0x09},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x08},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"T Mini",
     0xd001044f,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x06, 0x06}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x01},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}},
    {"Dual Analog Pad",
     0x1120f30,
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50,
      0x49, 0x44, 0x56, 0x49, 0x44, 0x0c, 0x00, 0x01, 0x00, 0x04, 0x00},
     32,
     0,
     0,
     0,
     0,
     {0x00, 0x00, 0x00, 0x00},
     {{{{0x02, 0x00, 0x00}, {0x02, 0x01, 0x01}, {0x02, 0x05, 0x05}, {0x02, 0x02, 0x02}},
       {{0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}, {0x00, 0x00, 0x00}}}},
     {{0x01, 0x05},
      {0x01, 0x07},
      {0x01, 0x04},
      {0x01, 0x06},
      {0x01, 0x01},
      {0x01, 0x03},
      {0x01, 0x02},
      {0x01, 0x00},
      {0x01, 0x08},
      {0x01, 0x0a},
      {0x01, 0x0b},
      {0x01, 0x09},
      {0x01, 0x80},
      {0x01, 0x81},
      {0x01, 0x82},
      {0x01, 0x83}}}};
PC_DI_CAPS PcKeyboardCapabilities = {};

extern "C" char dat_007516c0[];
extern "C" char dat_00753370[];
extern "C" char dat_00753374[];
extern "C" char dat_00759770[];
extern "C" char dat_0075ac34[];
extern "C" char UiBackPromptLiteral[];
extern "C" char dat_0076fde8[];
extern "C" char dat_007719bc[];
extern "C" char dat_007719c8[];
extern "C" char dat_007719d0[];
extern "C" char dat_007719dc[];
extern "C" char dat_007719e4[];
extern "C" char dat_007719f0[];
extern "C" char dat_007719f8[];
extern "C" char dat_00771a04[];
extern "C" char dat_00771a14[];
extern "C" char dat_00771a20[];
extern "C" char dat_00771a28[];
extern "C" char dat_00771a30[];
extern "C" char dat_00771a38[];
extern "C" char dat_00771a40[];
extern "C" char dat_00771a48[];
extern "C" char dat_00771a50[];
extern "C" char dat_00771a58[];
extern "C" char dat_00771a64[];
extern "C" char dat_00771a68[];
extern "C" char dat_00771a70[];
extern "C" char dat_00771a78[];
extern "C" char dat_00771a80[];
extern "C" char dat_00771a84[];
extern "C" char dat_00771a8c[];
extern "C" char dat_00771a94[];
extern "C" char dat_00771a9c[];
extern "C" char dat_00771aa4[];
extern "C" char dat_00771ab8[];
extern "C" char dat_00771ac0[];
extern "C" char dat_00771ac8[];
extern "C" char dat_00771ad0[];
extern "C" char dat_00771adc[];
extern "C" char dat_00771ae4[];
extern "C" char dat_00771aec[];
extern "C" char dat_00771af4[];
extern "C" char dat_00771afc[];
extern "C" char dat_00771b08[];
extern "C" char dat_00771b14[];
extern "C" char dat_00771b20[];
extern "C" char dat_00771b24[];
extern "C" char dat_00771b2c[];
extern "C" char dat_00771b34[];
extern "C" char dat_00771b40[];
extern "C" char dat_00771b48[];
extern "C" char dat_00771b4c[];
extern "C" char dat_00771b58[];
extern "C" char dat_00771b60[];
extern "C" char dat_00771b68[];
extern "C" char dat_00771b6c[];
extern "C" char dat_00771b78[];
extern "C" char dat_00771b80[];
extern "C" char dat_00771b88[];
extern "C" char dat_00771b90[];
extern "C" char dat_00771b94[];
extern "C" char dat_00771b98[];
extern "C" char dat_00771b9c[];
extern "C" char dat_00771ba0[];
extern "C" char dat_00771bb0[];
extern "C" char dat_00771bb8[];
extern "C" char dat_00771bc0[];
extern "C" char dat_00771bc8[];
extern "C" char dat_00771bd0[];
extern "C" char dat_00771bd8[];
extern "C" char dat_00771be0[];
extern "C" char dat_00771be8[];
extern "C" char dat_00771bf0[];
extern "C" char dat_00771bf8[];
extern "C" char dat_00771c00[];
extern "C" char dat_00771c08[];
extern "C" char dat_00771c10[];
extern "C" char dat_00771c1c[];
extern "C" char dat_00771c28[];
extern "C" char dat_00771c2c[];
extern "C" char dat_00771c30[];
extern "C" char dat_00771c34[];
extern "C" char dat_00771c38[];
extern "C" char dat_00771c3c[];
extern "C" char dat_00771c40[];
extern "C" char dat_00771c44[];
extern "C" char dat_00771c48[];
extern "C" char dat_00771c4c[];
extern "C" char dat_00771c50[];
extern "C" char dat_00771c5c[];
extern "C" char dat_00771c64[];
extern "C" char dat_00771c6c[];
extern "C" char dat_00771c74[];
extern "C" char dat_00771c80[];
extern "C" char dat_00771c8c[];
extern "C" char dat_00771c94[];
extern "C" char dat_00771c9c[];
extern "C" char dat_00771ca0[];
extern "C" char dat_00771ca4[];
extern "C" char dat_00771ca8[];
extern "C" char dat_00771cac[];
extern "C" char dat_00771cb0[];
extern "C" char dat_00771cb4[];
extern "C" char dat_00771cc0[];
extern "C" char dat_00771cc8[];
extern "C" char dat_00771cd4[];
extern "C" char dat_00771ce0[];
extern "C" char dat_00771ce4[];
extern "C" char dat_00771ce8[];
extern "C" char dat_00771cec[];
extern "C" char dat_00771cf0[];
extern "C" char dat_00771cf4[];
extern "C" char dat_00771cf8[];
extern "C" char dat_00771cfc[];
extern "C" char dat_00771d00[];
extern "C" char dat_00771d04[];
extern "C" char dat_00771d0c[];
extern "C" char dat_00771d14[];
extern "C" char dat_00771d18[];
extern "C" char dat_00771d1c[];
extern "C" char dat_00771d20[];
extern "C" char dat_00771d24[];
extern "C" char dat_00771d28[];
extern "C" char dat_00771d2c[];
extern "C" char dat_00771d30[];
extern "C" char dat_00771d34[];
extern "C" char dat_00771d38[];
extern "C" char dat_00771d3c[];
extern "C" char dat_00771d48[];
extern "C" char dat_00771d50[];
extern "C" char dat_00771d58[];
extern "C" char dat_00771d5c[];
extern "C" char dat_00771d60[];
extern "C" char dat_00771d64[];
extern "C" char dat_00771d68[];
extern "C" char dat_00771d6c[];
extern "C" char dat_00771d70[];
extern "C" char dat_00771d74[];
extern "C" char dat_00771d78[];
extern "C" char dat_00771d7c[];
extern "C" char dat_00771d80[];
extern "C" char dat_00774180[];
extern "C" char dat_00774184[];
static char dat_007517a8[3] = "\302\247";
static char dat_00755d70[2] = ")";
static char dat_0075c044[2] = ":";
static char dat_0075eba8[2] = ".";
static char dat_0076fe70[6] = "START";
static char dat_00770634[7] = "O. Win";
static char dat_0077063c[5] = "Alas";
static char dat_00770644[6] = "Oikea";
static char dat_0077064c[6] = "Vasen";
static char dat_00770654[6] = "Yl\303\266s";
static char dat_0077065c[7] = "O. Alt";
static char dat_00770664[8] = "O. Ctrl";
static char dat_0077066c[11] = "Num. Piste";
static char dat_00770678[6] = "V\303\244li";
static char dat_00770680[9] = "O.Vaihto";
static char dat_0077068c[7] = "Miinus";
static char dat_00770694[6] = "Piste";
static char dat_0077069c[7] = "Pilkku";
static char dat_007706a4[8] = "Sarkain";
static char dat_007706ac[14] = "Askelpalautin";
static char dat_007706bc[7] = "H\303\266ger";
static char dat_007706c4[9] = "V\303\244nster";
static char dat_007706d0[4] = "Upp";
static char dat_007706d4[11] = "Print Scrn";
static char dat_007706e0[17] = "Num. Kommatecken";
static char dat_007706f4[11] = "Num. Punkt";
static char dat_00770700[10] = "Blanksteg";
static char dat_0077070c[6] = "Punkt";
static char dat_00770714[12] = "Kommatecken";
static char dat_00770720[7] = "H. Win";
static char dat_00770728[7] = "V. Win";
static char dat_00770730[4] = "Ned";
static char dat_00770734[7] = "H\303\270yre";
static char dat_0077073c[8] = "Venstre";
static char dat_00770744[3] = "UP";
static char dat_00770748[8] = "NUM DIV";
static char dat_00770750[10] = "NUM COMMA";
static char dat_0077075c[9] = "Win Dto.";
static char dat_00770768[9] = "Win Esq.";
static char dat_00770774[16] = "Seta Para Baixo";
static char dat_00770784[18] = "Seta Para Direita";
static char dat_00770798[19] = "Seta Para Esquerda";
static char dat_007707ac[15] = "Seta Para Cima";
static char dat_007707bc[9] = "Alt Dto.";
static char dat_007707c8[9] = "Ctr Dto.";
static char dat_007707d4[11] = "Num. Ponto";
static char dat_007707e0[18] = "Barra De Espa\303\247os";
static char dat_007707f4[9] = "Alt Esq.";
static char dat_00770800[11] = "Shift Dto.";
static char dat_0077080c[21] = "Barra Para A Direita";
static char dat_00770824[6] = "Ponto";
static char dat_0077082c[9] = "V\303\255rgula";
static char dat_00770838[17] = "Par\303\252nteses Dto.";
static char dat_0077084c[11] = "Shift Esq.";
static char dat_00770858[13] = "Acento Grave";
static char dat_00770868[2] = "~";
static char dat_0077086c[10] = "Ctrl Esq.";
static char dat_00770878[17] = "Par\303\252nteses Esq.";
static char dat_0077088c[11] = "Ap\303\263strofo";
static char dat_00770898[11] = "Retroceder";
static char dat_007708a4[8] = "Igual A";
static char dat_007708ac[13] = "\320\237\321\200\320\260\320\262.Win";
static char dat_007708bc[11] = "\320\233\320\265\320\262.Win";
static char dat_007708c8[13] = "\320\237\321\200\320\260\320\262.Alt";
static char dat_007708d8[17] = "/ (\321\206\320\270\321\204.\320\272\320\273.)";
static char dat_007708ec[17] = ", (\321\206\320\270\321\204.\320\272\320\273.)";
static char dat_00770900[14] = "\320\237\321\200\320\260\320\262.Ctrl";
static char dat_00770910[21] = "Enter (\321\206\320\270\321\204.\320\272\320\273.)";
static char dat_00770928[17] = "= (\321\206\320\270\321\204.\320\272\320\273.)";
static char dat_0077093c[17] = ". (\321\206\320\270\321\204.\320\272\320\273.)";
static char dat_00770950[17] = "0 (\321\206\320\270\321\204.\320\272\320\273.)";
static char dat_00770964[17] = "3 (\321\206\320\270\321\204.\320\272\320\273.)";
static char dat_00770978[17] = "2 (\321\206\320\270\321\204.\320\272\320\273.)";
static char dat_0077098c[17] = "1 (\321\206\320\270\321\204.\320\272\320\273.)";
static char dat_007709a0[17] = "+ (\321\206\320\270\321\204.\320\272\320\273.)";
static char dat_007709b4[17] = "6 (\321\206\320\270\321\204.\320\272\320\273.)";
static char dat_007709c8[17] = "5 (\321\206\320\270\321\204.\320\272\320\273.)";
static char dat_007709dc[17] = "4 (\321\206\320\270\321\204.\320\272\320\273.)";
static char dat_007709f0[17] = "- (\321\206\320\270\321\204.\320\272\320\273.)";
static char dat_00770a04[17] = "9 (\321\206\320\270\321\204.\320\272\320\273.)";
static char dat_00770a18[17] = "8 (\321\206\320\270\321\204.\320\272\320\273.)";
static char dat_00770a2c[17] = "7 (\321\206\320\270\321\204.\320\272\320\273.)";
static char dat_00770a40[13] = "\320\237\321\200\320\276\320\261\320\265\320\273";
static char dat_00770a50[11] = "\320\233\320\265\320\262.Alt";
static char dat_00770a5c[17] = "* (\321\206\320\270\321\204.\320\272\320\273.)";
static char dat_00770a70[15] = "\320\237\321\200\320\260\320\262.shift";
static char dat_00770a80[3] = "\320\256";
static char dat_00770a84[3] = "\320\221";
static char dat_00770a88[3] = "\320\254";
static char dat_00770a8c[3] = "\320\242";
static char dat_00770a90[3] = "\320\230";
static char dat_00770a94[3] = "\320\234";
static char dat_00770a98[3] = "\320\241";
static char dat_00770a9c[3] = "\320\247";
static char dat_00770aa0[3] = "\320\257";
static char dat_00770aa4[13] = "\320\233\320\265\320\262.Shift";
static char dat_00770ab4[3] = "\320\255";
static char dat_00770ab8[3] = "\320\226";
static char dat_00770abc[3] = "\320\224";
static char dat_00770ac0[3] = "\320\233";
static char dat_00770ac4[3] = "\320\236";
static char dat_00770ac8[3] = "\320\240";
static char dat_00770acc[3] = "\320\237";
static char dat_00770ad0[3] = "\320\220";
static char dat_00770ad4[3] = "\320\222";
static char dat_00770ad8[3] = "\320\253";
static char dat_00770adc[3] = "\320\244";
static char dat_00770ae0[12] = "\320\233\320\265\320\262.Ctrl";
static char dat_00770aec[3] = "\320\252";
static char dat_00770af0[3] = "\320\245";
static char dat_00770af4[3] = "\320\227";
static char dat_00770af8[3] = "\320\251";
static char dat_00770afc[3] = "\320\250";
static char dat_00770b00[3] = "\320\223";
static char dat_00770b04[3] = "\320\235";
static char dat_00770b08[3] = "\320\225";
static char dat_00770b0c[3] = "\320\232";
static char dat_00770b10[3] = "\320\243";
static char dat_00770b14[3] = "\320\246";
static char dat_00770b18[3] = "\320\231";
static char dat_00770b1c[4] = "ESC";
static char dat_00770b20[7] = "P. Win";
static char dat_00770b28[6] = "D\303\263\305\200";
static char dat_00770b30[6] = "Prawo";
static char dat_00770b38[5] = "Lewo";
static char dat_00770b40[6] = "G\303\263ra";
static char dat_00770b48[7] = "P. Alt";
static char dat_00770b50[8] = "P. Ctrl";
static char dat_00770b58[7] = "Num. .";
static char dat_00770b60[7] = "Spacja";
static char dat_00770b68[9] = "P. Shift";
static char dat_00770b74[6] = "GRAVE";
static char dat_00770b7c[2] = ";";
static char dat_00770b80[6] = "R.Win";
static char dat_00770b88[7] = "Omlaag";
static char dat_00770b90[7] = "Rechts";
static char dat_00770b98[6] = "Links";
static char dat_00770ba0[7] = "Omhoog";
static char dat_00770ba8[10] = "Num. Punt";
static char dat_00770bb4[7] = "Spatie";
static char dat_00770bbc[5] = "Punt";
static char dat_00770bc4[2] = "@";
static char dat_00770bc8[3] = "\302\260";
static char dat_00770bcc[15] = "H. Windowsknap";
static char dat_00770bdc[15] = "V. Windowsknap";
static char dat_00770bec[8] = "Pil Ned";
static char dat_00770bf4[11] = "Pil H\303\270jre";
static char dat_00770c00[12] = "Pil Venstre";
static char dat_00770c0c[7] = "Pil Op";
static char dat_00770c14[7] = "H. Alt";
static char dat_00770c1c[13] = "Print Screen";
static char dat_00770c2c[11] = "Num. Komma";
static char dat_00770c38[6] = "VOLOP";
static char dat_00770c40[7] = "VOLNED";
static char dat_00770c48[5] = "REGN";
static char dat_00770c50[8] = "H. Ctrl";
static char dat_00770c58[11] = "N\303\206STESPOR";
static char dat_00770c64[15] = "UDEN ANGIVELSE";
static char dat_00770c74[17] = "SP\303\230RGSM\303\205LSTEGN";
static char dat_00770c88[9] = "STORT \303\206";
static char dat_00770c94[15] = "ANF\303\230RSELSTEGN";
static char dat_00770ca4[13] = "Num. Lig Med";
static char dat_00770cb4[13] = "Num. Punktum";
static char dat_00770cc4[15] = "Mellemrumstast";
static char dat_00770cd4[7] = "V. Alt";
static char dat_00770cdc[9] = "H. Shift";
static char dat_00770ce8[8] = "Punktum";
static char dat_00770cf0[6] = "Komma";
static char dat_00770cf8[9] = "V. Shift";
static char dat_00770d04[3] = "\302\275";
static char dat_00770d08[3] = "\303\230";
static char dat_00770d0c[3] = "\303\206";
static char dat_00770d10[8] = "V. Ctrl";
static char dat_00770d18[3] = "\302\250";
static char dat_00770d1c[3] = "\303\205";
static char dat_00770d20[7] = "Accent";
static char dat_00770d28[6] = "(TOM)";
static char dat_00770d30[9] = "Win Der.";
static char dat_00770d3c[9] = "Win Izq.";
static char dat_00770d48[5] = "Supr";
static char dat_00770d50[8] = "Av P\303\241g";
static char dat_00770d58[13] = "Flecha Abajo";
static char dat_00770d68[12] = "Flecha Der.";
static char dat_00770d74[12] = "Flecha Izq.";
static char dat_00770d80[8] = "Re P\303\241g";
static char dat_00770d88[11] = "Flecha Arr";
static char dat_00770d94[7] = "Inicio";
static char dat_00770d9c[9] = "Alt Der.";
static char dat_00770da8[7] = "PETSIS";
static char dat_00770db0[7] = "N\303\232M /";
static char dat_00770db8[10] = "N\303\232M Coma";
static char dat_00770dc4[10] = "INICIOWEB";
static char dat_00770dd0[9] = "VOL ARRI";
static char dat_00770ddc[9] = "VOL ABAJ";
static char dat_00770de8[7] = "REPROD";
static char dat_00770df0[9] = "SILENCIO";
static char dat_00770dfc[10] = "Ctrl Der.";
static char dat_00770e08[11] = "N\303\232M Intro";
static char dat_00770e14[10] = "PISTA SIG";
static char dat_00770e20[8] = "DETENER";
static char dat_00770e28[9] = "SUBRAYAR";
static char dat_00770e34[11] = "DOS PUNTOS";
static char dat_00770e40[7] = "ARROBA";
static char dat_00770e48[10] = "PISTA ANT";
static char dat_00770e54[11] = "N\303\232M Igual";
static char dat_00770e60[11] = "N\303\232M Punto";
static char dat_00770e6c[7] = "N\303\232M 0";
static char dat_00770e74[7] = "N\303\232M 3";
static char dat_00770e7c[7] = "N\303\232M 2";
static char dat_00770e84[7] = "N\303\232M 1";
static char dat_00770e8c[7] = "N\303\232M +";
static char dat_00770e94[7] = "N\303\232M 6";
static char dat_00770e9c[7] = "N\303\232M 5";
static char dat_00770ea4[7] = "N\303\232M 4";
static char dat_00770eac[7] = "N\303\232M -";
static char dat_00770eb4[7] = "N\303\232M 9";
static char dat_00770ebc[7] = "N\303\232M 8";
static char dat_00770ec4[7] = "N\303\232M 7";
static char dat_00770ecc[11] = "Bloq Despl";
static char dat_00770ed8[10] = "Bloq N\303\232M";
static char dat_00770ee4[12] = "Bloq May\303\272s";
static char dat_00770ef0[8] = "Espacio";
static char dat_00770ef8[9] = "Alt Izq.";
static char dat_00770f04[7] = "N\303\232M *";
static char dat_00770f0c[11] = "Mayus Der.";
static char dat_00770f18[6] = "Menos";
static char dat_00770f20[5] = "Coma";
static char dat_00770f28[3] = "\303\207";
static char dat_00770f2c[12] = "May\303\272s Izq.";
static char dat_00770f38[3] = "\302\272";
static char dat_00770f3c[3] = "\303\221";
static char dat_00770f40[10] = "Ctrl Izq.";
static char dat_00770f4c[6] = "Intro";
static char dat_00770f54[2] = "`";
static char dat_00770f58[10] = "Retroceso";
static char dat_00770f64[3] = "\302\241";
static char dat_00770f68[2] = "'";
static char dat_00770f6c[9] = "(VAC\303\215O)";
static char dat_00770f78[13] = "Win-Taste R.";
static char dat_00770f88[13] = "Win-Taste L.";
static char dat_00770f98[5] = "Entf";
static char dat_00770fa0[6] = "Einfg";
static char dat_00770fa8[8] = "Bild Ab";
static char dat_00770fb0[12] = "Pfeil unten";
static char dat_00770fbc[5] = "Ende";
static char dat_00770fc4[13] = "Pfeil rechts";
static char dat_00770fd4[12] = "Pfeil links";
static char dat_00770fe0[9] = "Bild Auf";
static char dat_00770fec[11] = "Pfeil oben";
static char dat_00770ff8[5] = "Pos1";
static char dat_00771000[6] = "lt R.";
static char dat_00771008[6] = "S-Abf";
static char dat_00771010[7] = "Num. ,";
static char dat_00771018[8] = "Strg R.";
static char dat_00771020[13] = "Num. Eingabe";
static char dat_00771030[10] = "UNBENANNT";
static char dat_0077103c[2] = "\366";
static char dat_00771040[2] = "\344";
static char dat_00771044[7] = "Rollen";
static char dat_0077104c[10] = "Num-Taste";
static char dat_00771058[15] = "Feststelltaste";
static char dat_00771068[10] = "Leertaste";
static char dat_00771074[7] = "Alt L.";
static char dat_0077107c[10] = "Umsch. R.";
static char dat_00771088[2] = ",";
static char dat_0077108c[10] = "Umsch. L.";
static char dat_00771098[3] = "\303\204";
static char dat_0077109c[3] = "\303\226";
static char dat_007710a0[8] = "Strg L.";
static char dat_007710a8[8] = "Eingabe";
static char dat_007710b0[3] = "\303\234";
static char dat_007710b4[11] = "R\303\274cktaste";
static char dat_007710c0[3] = "\302\264";
static char dat_007710c4[3] = "\303\237";
static char dat_007710c8[7] = "(LEER)";
static char dat_007710d0[9] = "Win Des.";
static char dat_007710dc[9] = "Win Sin.";
static char dat_007710e8[6] = "Canc.";
static char dat_007710f0[10] = "Inserisci";
static char dat_007710fc[10] = "Pag. Gi\303\231";
static char dat_00771108[13] = "Freccia Gi\303\271";
static char dat_00771118[5] = "Fine";
static char dat_00771120[13] = "Freccia Des.";
static char dat_00771130[13] = "Freccia Sin.";
static char dat_00771140[8] = "Pag. Su";
static char dat_00771148[12] = "Freccia S\303\271";
static char dat_00771154[6] = "Pausa";
static char dat_0077115c[9] = "Alt Des.";
static char dat_00771168[6] = "RSIST";
static char dat_00771170[10] = "Tastnum /";
static char dat_0077117c[10] = "Tastnum ,";
static char dat_00771188[7] = "Vol Su";
static char dat_00771190[9] = "Vol Gi\303\271";
static char dat_0077119c[10] = "Ctrl Des.";
static char dat_007711a8[14] = "Tastnum Invio";
static char dat_007711b8[10] = "Tastnum =";
static char dat_007711c4[10] = "Tastnum .";
static char dat_007711d0[10] = "Tastnum 0";
static char dat_007711dc[10] = "Tastnum 3";
static char dat_007711e8[10] = "Tastnum 2";
static char dat_007711f4[10] = "Tastnum 1";
static char dat_00771200[10] = "Tastnum +";
static char dat_0077120c[10] = "Tastnum 6";
static char dat_00771218[10] = "Tastnum 5";
static char dat_00771224[10] = "Tastnum 4";
static char dat_00771230[10] = "Tastnum -";
static char dat_0077123c[10] = "Tastnum 9";
static char dat_00771248[10] = "Tastnum 8";
static char dat_00771254[10] = "Tastnum 7";
static char dat_00771260[13] = "Blocca Scorr";
static char dat_00771270[9] = "Bloc Num";
static char dat_0077127c[12] = "BLOC MAIUSC";
static char dat_00771288[7] = "Spazio";
static char dat_00771290[9] = "Alt Sin.";
static char dat_0077129c[10] = "Tastnum *";
static char dat_007712a8[11] = "Shift Des.";
static char dat_007712b4[2] = "-";
static char dat_007712b8[6] = "Punto";
static char dat_007712c0[8] = "Virgola";
static char dat_007712c8[3] = "\303\271";
static char dat_007712cc[11] = "Shift Sin.";
static char dat_007712d8[3] = "\303\240";
static char dat_007712dc[3] = "\303\262";
static char dat_007712e0[10] = "Ctrl Sin.";
static char dat_007712ec[6] = "Invio";
static char dat_007712f4[2] = "+";
static char dat_007712f8[3] = "\303\250";
static char dat_007712fc[3] = "\303\254";
static char dat_00771300[10] = "APOSTROFO";
static char dat_0077130c[10] = "Windows D";
static char dat_00771318[10] = "Windows G";
static char dat_00771324[6] = "Suppr";
static char dat_0077132c[6] = "Inser";
static char dat_00771334[11] = "Page Suiv.";
static char dat_00771340[4] = "Bas";
static char dat_00771344[4] = "Fin";
static char dat_00771348[7] = "Droite";
static char dat_00771350[7] = "Gauche";
static char dat_00771358[12] = "Page Pr\303\250c.";
static char dat_00771364[5] = "Haut";
static char dat_0077136c[8] = "Origine";
static char dat_00771374[6] = "Alt D";
static char dat_0077137c[17] = "Impr \303\211cran Syst";
static char dat_00771390[12] = "Pav. Num. /";
static char dat_0077139c[18] = "Pav. Num. Virgule";
static char dat_007713b0[4] = "www";
static char dat_007713b4[7] = "Ctrl D";
static char dat_007713bc[18] = "Pav. Num. Entr\303\251e";
static char dat_007713d0[2] = "\260";
static char dat_007713d4[2] = "%";
static char dat_007713d8[12] = "Pav. Num. =";
static char dat_007713e4[2] = "{";
static char dat_007713e8[16] = "Pav. Num. Point";
static char dat_007713f8[12] = "Pav. Num. 0";
static char dat_00771404[12] = "Pav. Num. 3";
static char dat_00771410[12] = "Pav. Num. 2";
static char dat_0077141c[12] = "Pav. Num. 1";
static char dat_00771428[12] = "Pav. Num. +";
static char dat_00771434[12] = "Pav. Num. 6";
static char dat_00771440[12] = "Pav. Num. 5";
static char dat_0077144c[12] = "Pav. Num. 4";
static char dat_00771458[12] = "Pav. Num. -";
static char dat_00771464[12] = "Pav. Num. 9";
static char dat_00771470[12] = "Pav. Num. 8";
static char dat_0077147c[12] = "Pav. Num. 7";
static char dat_00771488[14] = "Arr\303\252t D\303\251fil";
static char dat_00771498[9] = "Verr Num";
static char dat_007714a4[9] = "Verr Maj";
static char dat_007714b0[7] = "Espace";
static char dat_007714b8[6] = "Alt G";
static char dat_007714c0[12] = "Pav. Num. *";
static char dat_007714cc[6] = "Maj D";
static char dat_007714d4[20] = "Point D'Exclamation";
static char dat_007714e8[12] = "Deux Points";
static char dat_007714f4[14] = "Point Virgule";
static char dat_00771504[8] = "Virgule";
static char dat_0077150c[2] = "*";
static char dat_00771510[6] = "Maj G";
static char dat_00771518[3] = "\302\262";
static char dat_0077151c[3] = "\303\231";
static char dat_00771520[7] = "Ctrl G";
static char dat_00771528[8] = "Entr\303\251e";
static char dat_00771530[2] = "|";
static char dat_00771534[2] = "^";
static char dat_00771538[11] = "Tabulation";
static char dat_00771544[5] = "Egal";
static char dat_0077154c[6] = "Echap";
static char dat_00771554[6] = "P.Win";
static char dat_0077155c[6] = "L.Win";
static char dat_00771564[8] = "Pg Down";
static char dat_0077156c[6] = "Dol\305\257";
static char dat_00771574[7] = "Vpravo";
static char dat_0077157c[6] = "Vlevo";
static char dat_00771584[6] = "Pg Up";
static char dat_0077158c[7] = "Nahoru";
static char dat_00771594[6] = "P.Alt";
static char dat_0077159c[6] = "Num./";
static char dat_007715a4[7] = "P.Ctrl";
static char dat_007715ac[10] = "Num.Enter";
static char dat_007715b8[14] = "Num.Rovn\303\241 Se";
static char dat_007715c8[6] = "Num .";
static char dat_007715d0[6] = "Num.0";
static char dat_007715d8[6] = "Num.3";
static char dat_007715e0[6] = "Num.2";
static char dat_007715e8[6] = "Num.1";
static char dat_007715f0[6] = "Num.+";
static char dat_007715f8[6] = "Num.6";
static char dat_00771600[6] = "Num.5";
static char dat_00771608[6] = "Num.4";
static char dat_00771610[6] = "Num.-";
static char dat_00771618[6] = "Num.9";
static char dat_00771620[6] = "Num.8";
static char dat_00771628[6] = "Num.7";
static char dat_00771630[10] = "Mezern\303\255k";
static char dat_0077163c[6] = "L.Alt";
static char dat_00771644[6] = "Kr\303\241t";
static char dat_0077164c[8] = "P.Shift";
static char dat_00771654[9] = "Lom\303\255tko";
static char dat_00771660[7] = "Te\304\215ka";
static char dat_00771668[8] = "\304\214\303\241rka";
static char dat_00771670[9] = "Uvozovky";
static char dat_0077167c[8] = "L.Shift";
static char dat_00771684[8] = "Vlnovka";
static char dat_0077168c[9] = "Apostrof";
static char dat_00771698[3] = "\305\256";
static char dat_0077169c[7] = "L.Ctrl";
static char dat_007716a4[11] = "P.Z\303\241vorka";
static char dat_007716b0[3] = "\303\232";
static char dat_007716b4[11] = "Tabul\303\241tor";
static char dat_007716c0[10] = "Rovn\303\241 Se";
static char dat_007716cc[7] = "M\303\255nus";
static char dat_007716d4[8] = "(EMPTY)";
static char dat_007716dc[25] = "\343\202\242\343\203\227\343\203\252\343\202\261\343\203\274\343\202\267\343\203\247\343\203\263";
static char dat_007716f8[23] = "\345\217\263 \343\202\246\343\202\243\343\203\263\343\203\211\343\202\246\343\202\272";
static char dat_00771710[23] = "\345\267\246 \343\202\246\343\202\243\343\203\263\343\203\211\343\202\246\343\202\272";
static char dat_00771728[10] = "Page Down";
static char dat_00771734[10] = "\344\270\213\347\237\242\345\215\260";
static char dat_00771740[10] = "\345\217\263\347\237\242\345\215\260";
static char dat_0077174c[10] = "\345\267\246\347\237\242\345\215\260";
static char dat_00771758[8] = "Page Up";
static char dat_00771760[10] = "\344\270\212\347\237\242\345\215\260";
static char dat_0077176c[8] = "\345\217\263 Alt";
static char dat_00771774[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 /";
static char dat_00771788[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 ,";
static char dat_0077179c[9] = "\345\217\263 Ctrl";
static char dat_007717a8[22] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 Enter";
static char dat_007717c0[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 .";
static char dat_007717d4[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 0";
static char dat_007717e8[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 3";
static char dat_007717fc[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 2";
static char dat_00771810[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 1";
static char dat_00771824[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 +";
static char dat_00771838[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 6";
static char dat_0077184c[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 5";
static char dat_00771860[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 4";
static char dat_00771874[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 -";
static char dat_00771888[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 9";
static char dat_0077189c[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 8";
static char dat_007718b0[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 7";
static char dat_007718c4[13] = "\343\202\271\343\203\232\343\203\274\343\202\271";
static char dat_007718d4[8] = "\345\267\246 Alt";
static char dat_007718dc[18] = "\343\203\206\343\203\263\343\202\255\343\203\274\343\201\256 *";
static char dat_007718f0[10] = "\345\217\263 Shift";
static char dat_007718fc[34] = "\343\203\225\343\202\251\343\203\257\343\203\274\343\203\211\343\202\271\343\203"
                               "\251\343\203\203\343\202\267\343\203\245(/)";
static char dat_00771920[16] = "\343\203\224\343\203\252\343\202\252\343\203\211(.)";
static char dat_00771930[13] = "\343\202\253\343\203\263\343\203\236(,)";
static char dat_00771940[10] = "\345\267\246 Shift";
static char dat_0077194c[16] = "\343\202\260\343\203\254\343\202\244\343\203\264(`)";
static char dat_0077195c[25] = "\343\202\242\343\203\235\343\202\271\343\203\210\343\203\255\343\203\225\343\202\243(')";
static char dat_00771978[21] = "\343\202\273\343\203\237\343\202\263\343\203\255\343\203\263(\357\274\233)";
static char dat_00771990[9] = "\345\267\246 Ctrl";
static char dat_0077199c[11] = "Num. Comma";
static char dat_007719a8[8] = "Oem_102";
static char dat_007719b0[10] = "Num. Stop";
static char dat_00771e38[15] = "\305\275\303\241dn\303\275 vstup";
static char dat_00771e48[19] = "Tla\304\215\303\255tko my\305\241i A";
static char dat_00771e5c[27] = "St\305\231edn\303\255 tla\304\215\303\255tko my\305\241i";
static char dat_00771e78[24] = "Prav\303\251 tla\304\215\303\255tko my\305\241i";
static char dat_00771e90[23] = "Lev\303\251 tla\304\215\303\255tko my\305\241i";
static char dat_00771ea8[22] = "Kole\304\215ko my\305\241i nahoru";
static char dat_00771ec0[19] = "Pohyb my\305\241\303\255 dol\305\257";
static char dat_00771ed4[20] = "Pohyb my\305\241\303\255 vpravo";
static char dat_00771ee8[21] = "Kole\304\215ko my\305\241i dol\305\257";
static char dat_00771f00[20] = "Pohyb my\305\241\303\255 nahoru";
static char dat_00771f14[19] = "Pohyb my\305\241\303\255 vlevo";
static char dat_00771f28[6] = "Ose X";
static char dat_00771f30[6] = "Zp\304\233t";
static char dat_00771f38[15] = "Prav\303\241 p\303\241\304\215ka";
static char dat_00771f48[14] = "Lev\303\241 p\303\241\304\215ka";
static char dat_00771f58[16] = "Prav\303\241 spou\305\241\305\245";
static char dat_00771f68[15] = "Lev\303\241 spou\305\241\305\245";
static char dat_00771f78[26] = "Prav\303\251 bo\304\215n\303\255 tla\304\215\303\255tko";
static char dat_00771f94[25] = "Lev\303\251 bo\304\215n\303\255 tla\304\215\303\255tko";
static char dat_00771fb0[13] = "Tla\304\215\303\255tko X";
static char dat_00771fc0[13] = "Tla\304\215\303\255tko A";
static char dat_00771fd0[13] = "Tla\304\215\303\255tko B";
static char dat_00771fe0[13] = "Tla\304\215\303\255tko Y";
static char dat_00771ff0[21] = "Prav\303\241 p\303\241\304\215ka dol\305\257";
static char dat_00772008[22] = "Prav\303\241 p\303\241\304\215ka nahoru";
static char dat_00772020[22] = "Prav\303\241 p\303\241\304\215ka vpravo";
static char dat_00772038[21] = "Prav\303\241 p\303\241\304\215ka vlevo";
static char dat_00772050[20] = "Lev\303\241 p\303\241\304\215ka dol\305\257";
static char dat_00772064[21] = "Lev\303\241 p\303\241\304\215ka nahoru";
static char dat_0077207c[21] = "Lev\303\241 p\303\241\304\215ka vpravo";
static char dat_00772094[20] = "Lev\303\241 p\303\241\304\215ka vlevo";
static char dat_007720a8[9] = "Rotace Z";
static char dat_007720b4[9] = "Rotace Y";
static char dat_007720c0[9] = "Rotace X";
static char dat_007720cc[6] = "Osa Z";
static char dat_007720d4[6] = "Osa Y";
static char dat_007720dc[6] = "Osa X";
static char dat_007720e4[9] = "Vy\305\241\305\241\303\255";
static char dat_007720f0[9] = "Ni\305\276\305\241\303\255";
static char dat_007720fc[11] = "Tla\304\215\303\255tko";
static char dat_00772108[21] = "Sm\304\233r. k\305\231\303\255\305\276 vlevo";
static char dat_00772120[21] = "Sm\304\233r. k\305\231\303\255\305\276 dol\305\257";
static char dat_00772138[22] = "Sm\304\233r. k\305\231\303\255\305\276 vpravo";
static char dat_00772150[22] = "Sm\304\233r. k\305\231\303\255\305\276 nahoru";
static char dat_0077229c[39] = "\320\235\320\265\321\202 \321\203\321\201\321\202\321\200\320\276\320\271\321\201\321\202\320\262\320\260 "
                               "\320\262\320\262\320\276\320\264\320\260";
static char dat_007722c4[24] = "\320\232\320\275\320\276\320\277\320\272\320\260 \320\274\321\213\321\210\320\270 N";
static char dat_007722dc[37] = "\320\241\321\200\320\265\320\264\320\275\321\217\321\217 "
                               "\320\272\320\275\320\276\320\277\320\272\320\260 \320\274\321\213\321\210\320\270";
static char dat_00772304[35] = "\320\237\321\200\320\260\320\262\320\260\321\217 "
                               "\320\272\320\275\320\276\320\277\320\272\320\260 \320\274\321\213\321\210\320\270";
static char dat_00772328[33] = "\320\233\320\265\320\262\320\260\321\217 "
                               "\320\272\320\275\320\276\320\277\320\272\320\260 \320\274\321\213\321\210\320\270";
static char dat_0077234c[33] = "\320\232\320\276\320\273\320\265\321\201\320\276 \320\274\321\213\321\210\320\270 "
                               "\320\262\320\262\320\265\321\200\321\205";
static char dat_00772370[18] = "\320\234\321\213\321\210\321\214 \320\262\320\275\320\270\320\267";
static char dat_00772384[22] = "\320\234\321\213\321\210\321\214 \320\262\320\277\321\200\320\260\320\262\320\276";
static char dat_0077239c[31] = "\320\232\320\276\320\273\320\265\321\201\320\276 \320\274\321\213\321\210\320\270 "
                               "\320\262\320\275\320\270\320\267";
static char dat_007723bc[20] = "\320\234\321\213\321\210\321\214 \320\262\320\262\320\265\321\200\321\205";
static char dat_007723d0[20] = "\320\234\321\213\321\210\321\214 \320\262\320\273\320\265\320\262\320\276";
static char dat_007723e4[20] = "\320\234\321\213\321\210\321\214 (\320\276\321\201\321\214 Z)";
static char dat_007723f8[20] = "\320\234\321\213\321\210\321\214 (\320\276\321\201\321\214 Y)";
static char dat_0077240c[20] = "\320\234\321\213\321\210\321\214 (\320\276\321\201\321\214 X)";
static char dat_00772420[11] = "\320\235\320\260\320\267\320\260\320\264";
static char dat_0077242c[11] = "\320\241\321\202\320\260\321\200\321\202";
static char dat_00772438[30] =
    "\320\237\321\200\320\260\320\262\321\213\320\271 \320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272";
static char dat_00772458[28] = "\320\233\320\265\320\262\321\213\320\271 \320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272";
static char dat_00772474[24] = "\320\237\321\200\320\260\320\262\321\213\320\271 \320\272\321\203\321\200\320\276\320\272";
static char dat_0077248c[22] = "\320\233\320\265\320\262\321\213\320\271 \320\272\321\203\321\200\320\276\320\272";
static char dat_007724a4[28] = "\320\237\321\200\320\260\320\262\321\213\320\271 \321\202\321\200\320\270\320\263\320\263\320\265\321\200";
static char dat_007724c0[26] = "\320\233\320\265\320\262\321\213\320\271 \321\202\321\200\320\270\320\263\320\263\320\265\321\200";
static char dat_007724dc[15] = "\320\232\320\275\320\276\320\277\320\272\320\260 X";
static char dat_007724ec[15] = "\320\232\320\275\320\276\320\277\320\272\320\260 A";
static char dat_007724fc[15] = "\320\232\320\275\320\276\320\277\320\272\320\260 B";
static char dat_0077250c[15] = "\320\232\320\275\320\276\320\277\320\272\320\260 Y";
static char dat_0077251c[41] = "\320\222\320\275\320\270\320\267 (\320\277\321\200\320\260\320\262\321\213\320\271 "
                               "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
static char dat_00772548[43] = "\320\222\320\262\320\265\321\200\321\205 (\320\277\321\200\320\260\320\262\321\213\320\271 "
                               "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
static char dat_00772574[45] = "\320\222\320\277\321\200\320\260\320\262\320\276 (\320\277\321\200\320\260\320\262\321\213\320\271 "
                               "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
static char dat_007725a4[43] = "\320\222\320\273\320\265\320\262\320\276 (\320\277\321\200\320\260\320\262\321\213\320\271 "
                               "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
static char dat_007725d0[39] = "\320\222\320\275\320\270\320\267 (\320\273\320\265\320\262\321\213\320\271 "
                               "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
static char dat_007725f8[41] = "\320\222\320\262\320\265\321\200\321\205 (\320\273\320\265\320\262\321\213\320\271 "
                               "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
static char dat_00772624[43] = "\320\222\320\277\321\200\320\260\320\262\320\276 (\320\273\320\265\320\262\321\213\320\271 "
                               "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
static char dat_00772650[41] = "\320\222\320\273\320\265\320\262\320\276 (\320\273\320\265\320\262\321\213\320\271 "
                               "\320\264\320\266\320\276\320\271\321\201\321\202\320\270\320\272)";
static char dat_0077267c[24] = "\320\222\321\200\320\260\321\211\320\265\320\275\320\270\320\265 \320\277\320\276 Z";
static char dat_00772694[24] = "\320\222\321\200\320\260\321\211\320\265\320\275\320\270\320\265 \320\277\320\276 Y";
static char dat_007726ac[24] = "\320\222\321\200\320\260\321\211\320\265\320\275\320\270\320\265 \320\277\320\276 X";
static char dat_007726c4[9] = "\320\236\321\201\321\214 Z";
static char dat_007726d0[9] = "\320\236\321\201\321\214 Y";
static char dat_007726dc[9] = "\320\236\321\201\321\214 X";
static char dat_007726e8[29] = "\320\220\320\275\320\260\320\273\320\276\320\263\320\276\320\262\321\213\320\271 %d (%s)";
static char dat_00772708[15] = "\320\222\321\213\321\201\320\276\320\272\320\276\320\265";
static char dat_00772718[13] = "\320\235\320\270\320\267\320\272\320\276\320\265";
static char dat_00772728[13] = "\320\232\320\275\320\276\320\277\320\272\320\260";
static char dat_00772738[11] = "\320\222\320\273\320\265\320\262\320\276";
static char dat_00772744[9] = "\320\222\320\275\320\270\320\267";
static char dat_00772750[13] = "\320\222\320\277\321\200\320\260\320\262\320\276";
static char dat_00772760[11] = "\320\222\320\262\320\265\321\200\321\205";
static char dat_00772830[14] = "Brak wej\305\233cia";
static char dat_00772840[20] = "Myszka - przycisk N";
static char dat_00772854[26] = "\305\232rodkowy przycisk myszki";
static char dat_00772870[22] = "Prawy przycisk myszki";
static char dat_00772888[21] = "Lewy przycisk myszki";
static char dat_007728a0[24] = "K\303\263\305\202ko myszki w g\303\263r\304\231";
static char dat_007728b8[15] = "Myszka w d\303\263\305\202";
static char dat_007728c8[23] = "K\303\263\305\202ko myszki w d\303\263\305\202";
static char dat_007728e0[16] = "Myszka w g\303\263r\304\231";
static char dat_007728f0[14] = "Myszka w lewo";
static char dat_00772900[9] = "Myszka Z";
static char dat_0077290c[9] = "Myszka Y";
static char dat_00772918[9] = "Myszka X";
static char dat_00772924[7] = "Wr\303\263\304\207";
static char dat_0077292c[15] = "Prawy dr\304\205\305\274ek";
static char dat_0077293c[14] = "Lewy dr\304\205\305\274ek";
static char dat_0077294c[12] = "Prawy spust";
static char dat_00772958[11] = "Lewy spust";
static char dat_00772964[23] = "Prawy przycisk przedni";
static char dat_0077297c[22] = "Lewy przycisk przedni";
static char dat_00772994[11] = "Przycisk X";
static char dat_007729a0[11] = "Przycisk A";
static char dat_007729ac[11] = "Przycisk B";
static char dat_007729b8[11] = "Przycisk Y";
static char dat_007729c4[23] = "Prawy dr\304\205\305\274ek w d\303\263\305\202";
static char dat_007729dc[24] = "Prawy dr\304\205\305\274ek w g\303\263r\304\231";
static char dat_007729f4[23] = "Prawy dr\304\205\305\274ek w prawo";
static char dat_00772a0c[22] = "Prawy dr\304\205\305\274ek w lewo";
static char dat_00772a24[22] = "Lewy dr\304\205\305\274ek w d\303\263\305\202";
static char dat_00772a3c[23] = "Lewy dr\304\205\305\274ek w g\303\263r\304\231";
static char dat_00772a54[22] = "Lewy dr\304\205\305\274ek w prawo";
static char dat_00772a6c[21] = "Lewy dr\304\205\305\274ek w lewo";
static char dat_00772a84[15] = "Obr\303\263t w osi Z";
static char dat_00772a94[15] = "Obr\303\263t w osi Y";
static char dat_00772aa4[15] = "Obr\303\263t w osi X";
static char dat_00772ab4[6] = "O\305\233 Z";
static char dat_00772abc[6] = "O\305\233 Y";
static char dat_00772ac4[6] = "O\305\233 X";
static char dat_00772acc[18] = "Analogowy (%s) %d";
static char dat_00772ae0[7] = "Wysoka";
static char dat_00772ae8[6] = "Niska";
static char dat_00772af0[9] = "Przycisk";
static char dat_00772afc[16] = "Kapturek w lewo";
static char dat_00772b0c[17] = "Kapturek w d\303\263\305\202";
static char dat_00772b20[17] = "Kapturek w prawo";
static char dat_00772b34[18] = "Kapturek w g\303\263r\304\231";
static char dat_00772bd8[11] = "Museknap N";
static char dat_00772be4[19] = "Midterste museknap";
static char dat_00772bf8[16] = "H\303\270jre museknap";
static char dat_00772c08[17] = "Venstre museknap";
static char dat_00772c1c[11] = "Mushjul op";
static char dat_00772c28[11] = "Mus h\303\270jre";
static char dat_00772c34[7] = "Mus op";
static char dat_00772c3c[7] = "\011Mus X";
static char dat_00772c44[8] = "Tilbage";
static char dat_00772c4c[17] = "H\303\270jre styrepind";
static char dat_00772c60[18] = "Venstre styrepind";
static char dat_00772c74[17] = "Venstre udl\303\270ser";
static char dat_00772c88[19] = "H\303\270jre skulderknap";
static char dat_00772c9c[20] = "Venstre skulderknap";
static char dat_00772cb0[7] = "Knap X";
static char dat_00772cb8[7] = "Knap A";
static char dat_00772cc0[7] = "Knap B";
static char dat_00772cc8[7] = "Knap Y";
static char dat_00772cd0[21] = "H\303\270jre Styrepind Ned";
static char dat_00772ce8[20] = "H\303\270jre Styrepind Op";
static char dat_00772cfc[24] = "H\303\270jre Styrepind H\303\270jre";
static char dat_00772d14[25] = "H\303\270jre Styrepind Venstre";
static char dat_00772d30[22] = "Venstre Styrepind Ned";
static char dat_00772d48[21] = "Venstre Styrepind Op";
static char dat_00772d60[25] = "Venstre Styrepind H\303\270jre";
static char dat_00772d7c[26] = "Venstre Styrepind Venstre";
static char dat_00772d98[9] = "Rot\303\251r Z";
static char dat_00772da4[9] = "Rot\303\251r Y";
static char dat_00772db0[10] = "Rot\303\251r X ";
static char dat_00772dbc[8] = "Z-aksen";
static char dat_00772dc4[8] = "Y-aksen";
static char dat_00772dcc[8] = "X-aksen";
static char dat_00772dd4[5] = "H\303\270j";
static char dat_00772ddc[5] = "knap";
static char dat_00772de4[19] = "Hat-switch venstre";
static char dat_00772df8[15] = "Hat-switch ned";
static char dat_00772e08[18] = "Hat-switch h\303\270jre";
static char dat_00772e1c[14] = "Hat-switch Op";
static char dat_00772eec[7] = "Vac\303\255o";
static char dat_00772ef4[16] = "Bot\303\263n rat\303\263n N";
static char dat_00772f04[26] = "Bot\303\263n central del rat\303\263n";
static char dat_00772f20[26] = "Bot\303\263n derecho del rat\303\263n";
static char dat_00772f3c[28] = "Bot\303\263n izquierdo del rat\303\263n";
static char dat_00772f58[30] = "Rueda del rat\303\263n hacia arriba";
static char dat_00772f78[19] = "Rat\303\263n hacia abajo";
static char dat_00772f8c[20] = "Rat\303\263n a la derecha";
static char dat_00772fa0[29] = "Rueda del rat\303\263n hacia abajo";
static char dat_00772fc0[20] = "Rat\303\263n hacia arriba";
static char dat_00772fd4[22] = "Rat\303\263n a la izquierda";
static char dat_00772fec[9] = "Rat\303\263n Z";
static char dat_00772ff8[9] = "Rat\303\263n Y";
static char dat_00773004[9] = "Rat\303\263n X";
static char dat_00773010[7] = "Volver";
static char dat_00773018[13] = "Bot\303\263n START";
static char dat_00773028[14] = "Stick derecho";
static char dat_00773038[16] = "Stick izquierdo";
static char dat_00773048[25] = "Gatillo inferior derecho";
static char dat_00773064[27] = "Gatillo inferior izquierdo";
static char dat_00773080[25] = "Gatillo superior derecho";
static char dat_0077309c[27] = "Gatillo superior izquierdo";
static char dat_007730b8[9] = "Bot\303\263n X";
static char dat_007730c4[9] = "Bot\303\263n A";
static char dat_007730d0[9] = "Bot\303\263n B";
static char dat_007730dc[9] = "Bot\303\263n Y";
static char dat_007730e8[20] = "Stick derecho abajo";
static char dat_007730fc[21] = "Stick derecho arriba";
static char dat_00773114[27] = "Stick derecho a la derecha";
static char dat_00773130[29] = "Stick derecho a la izquierda";
static char dat_00773150[22] = "Stick izquierdo abajo";
static char dat_00773168[23] = "Stick izquierdo arriba";
static char dat_00773180[29] = "Stick izquierdo a la derecha";
static char dat_007731a0[31] = "Stick izquierdo a la izquierda";
static char dat_007731c0[12] = "Rotaci\303\263n Z";
static char dat_007731cc[12] = "Rotaci\303\263n Y";
static char dat_007731d8[12] = "Rotaci\303\263n X";
static char dat_007731e4[6] = "Eje Z";
static char dat_007731ec[6] = "Eje Y";
static char dat_007731f4[6] = "Eje X";
static char dat_007731fc[19] = "%d (%s) anal\303\263gico";
static char dat_00773210[5] = "Baja";
static char dat_00773218[7] = "Bot\303\263n";
static char dat_00773220[30] = "Mando de direcci\303\263n izquierda";
static char dat_00773240[26] = "Mando de direcci\303\263n abajo";
static char dat_0077325c[28] = "Mando de direcci\303\263n derecha";
static char dat_00773278[27] = "Mando de direcci\303\263n arriba";
static char dat_00773334[14] = "Keine Eingabe";
static char dat_00773344[12] = "Maustaste N";
static char dat_00773350[19] = "Mittlere Maustaste";
static char dat_00773364[17] = "Rechte Maustaste";
static char dat_00773378[16] = "Linke Maustaste";
static char dat_00773388[13] = "Mausrad Oben";
static char dat_00773398[11] = "Maus Unten";
static char dat_007733a4[12] = "Maus Rechts";
static char dat_007733b0[14] = "Mausrad Unten";
static char dat_007733c0[10] = "Maus Oben";
static char dat_007733cc[11] = "Maus Links";
static char dat_007733d8[7] = "Maus Z";
static char dat_007733e0[7] = "Maus Y";
static char dat_007733e8[7] = "Maus X";
static char dat_007733f0[8] = "Zur\303\274ck";
static char dat_007733f8[14] = "Rechter Stick";
static char dat_00773408[13] = "Linker Stick";
static char dat_00773418[17] = "Rechter Schalter";
static char dat_0077342c[16] = "Linker Schalter";
static char dat_0077343c[21] = "Rechte Schultertaste";
static char dat_00773454[20] = "Linke Schultertaste";
static char dat_00773468[8] = "X-Taste";
static char dat_00773470[8] = "A-Taste";
static char dat_00773478[8] = "B-Taste";
static char dat_00773480[8] = "Y-Taste";
static char dat_00773488[20] = "Rechter Stick unten";
static char dat_0077349c[19] = "Rechter Stick oben";
static char dat_007734b0[21] = "Rechter Stick rechts";
static char dat_007734c8[20] = "Rechter Stick links";
static char dat_007734dc[19] = "Linker Stick unten";
static char dat_007734f0[18] = "Linker Stick oben";
static char dat_00773504[20] = "Linker Stick rechts";
static char dat_00773518[19] = "Linker Stick links";
static char dat_0077352c[11] = "Z-Rotation";
static char dat_00773538[11] = "Y-Rotation";
static char dat_00773544[11] = "X-Rotation";
static char dat_00773550[8] = "Z-Achse";
static char dat_00773558[8] = "Y-Achse";
static char dat_00773560[8] = "X-Achse";
static char dat_00773568[5] = "Hoch";
static char dat_00773570[8] = "Niedrig";
static char dat_00773578[6] = "Taste";
static char dat_00773580[18] = "Steuerkreuz links";
static char dat_00773594[18] = "Steuerkreuz unten";
static char dat_007735a8[19] = "Steuerkreuz rechts";
static char dat_007735bc[17] = "Steuerkreuz oben";
static char dat_0077367c[13] = "Nessun Input";
static char dat_0077368c[18] = "Tasto N del mouse";
static char dat_007736a0[25] = "Tasto centrale del mouse";
static char dat_007736bc[23] = "Tasto destro del mouse";
static char dat_007736d4[25] = "Tasto sinistro del mouse";
static char dat_007736f0[21] = "Rotella del mouse su";
static char dat_00773708[11] = "Mouse gi\303\271";
static char dat_00773714[15] = "Mouse a destra";
static char dat_00773724[23] = "Rotella del mouse gi\303\271";
static char dat_0077373c[9] = "Mouse su";
static char dat_00773748[17] = "Mouse a sinistra";
static char dat_0077375c[9] = "Indietro";
static char dat_00773768[24] = "Pulsante levetta destro";
static char dat_00773780[26] = "Pulsante levetta sinistro";
static char dat_0077379c[17] = "Grilletto destro";
static char dat_007737b0[19] = "Grilletto sinistro";
static char dat_007737c4[24] = "Pulsante dorsale destro";
static char dat_007737dc[26] = "Pulsante dorsale sinistro";
static char dat_007737f8[11] = "Pulsante X";
static char dat_00773804[11] = "Pulsante A";
static char dat_00773810[11] = "Pulsante B";
static char dat_0077381c[11] = "Pulsante Y";
static char dat_00773828[23] = "Levetta destra in gi\303\271";
static char dat_00773840[21] = "Levetta destra in su";
static char dat_00773858[24] = "Levetta destra a destra";
static char dat_00773870[26] = "Levetta destra a sinistra";
static char dat_0077388c[25] = "Levetta sinistra in gi\303\271";
static char dat_007738a8[23] = "Levetta sinistra in su";
static char dat_007738c0[26] = "Levetta sinistra a destra";
static char dat_007738dc[28] = "Levetta sinistra a sinistra";
static char dat_007738f8[12] = "Rotazione Z";
static char dat_00773904[12] = "Rotazione Y";
static char dat_00773910[12] = "Rotazione X";
static char dat_0077391c[7] = "Asse Z";
static char dat_00773924[7] = "Asse Y";
static char dat_0077392c[7] = "Asse X";
static char dat_00773934[5] = "Alta";
static char dat_0077393c[6] = "Bassa";
static char dat_00773944[9] = "Pulsante";
static char dat_00773950[27] = "Tasto direzionale sinistra";
static char dat_0077396c[23] = "Tasto direzionale gi\303\271";
static char dat_00773984[25] = "Tasto direzionale destra";
static char dat_007739a0[21] = "Tasto direzionale su";
static char dat_00773a78[21] = "Aucune touche saisie";
static char dat_00773a90[16] = "Souris bouton N";
static char dat_00773aa0[21] = "Bouton souris milieu";
static char dat_00773ab8[20] = "Bouton souris droit";
static char dat_00773acc[21] = "Bouton souris gauche";
static char dat_00773ae4[28] = "Molette souris vers le haut";
static char dat_00773b00[19] = "Souris vers le bas";
static char dat_00773b14[22] = "Souris vers la droite";
static char dat_00773b2c[27] = "Molette souris vers le bas";
static char dat_00773b48[20] = "Souris vers le haut";
static char dat_00773b5c[22] = "Souris vers la gauche";
static char dat_00773b74[9] = "Souris Z";
static char dat_00773b80[9] = "Souris Y";
static char dat_00773b8c[9] = "Souris X";
static char dat_00773b98[7] = "Retour";
static char dat_00773ba0[13] = "Touche START";
static char dat_00773bb0[23] = "Stick analogique droit";
static char dat_00773bc8[24] = "Stick analogique gauche";
static char dat_00773be0[17] = "G\303\242chette droite";
static char dat_00773bf4[17] = "G\303\242chette gauche";
static char dat_00773c08[23] = "G\303\242chette haute droite";
static char dat_00773c20[23] = "G\303\242chette haute gauche";
static char dat_00773c38[9] = "Touche X";
static char dat_00773c44[9] = "Touche A";
static char dat_00773c50[9] = "Touche B";
static char dat_00773c5c[9] = "Touche Y";
static char dat_00773c68[29] = "Stick analogique droit - Bas";
static char dat_00773c88[30] = "Stick analogique droit - Haut";
static char dat_00773ca8[32] = "Stick analogique droit - Droite";
static char dat_00773cc8[32] = "Stick analogique droit - Gauche";
static char dat_00773ce8[30] = "Stick analogique gauche - Bas";
static char dat_00773d08[31] = "Stick analogique gauche - Haut";
static char dat_00773d28[33] = "Stick analogique gauche - Droite";
static char dat_00773d4c[33] = "Stick analogique gauche - Gauche";
static char dat_00773d70[11] = "Rotation Z";
static char dat_00773d7c[11] = "Rotation Y";
static char dat_00773d88[11] = "Rotation X";
static char dat_00773d94[6] = "Axe Z";
static char dat_00773d9c[6] = "Axe Y";
static char dat_00773da4[6] = "Axe X";
static char dat_00773dac[19] = "%d (%s) analogique";
static char dat_00773dc0[6] = "Haute";
static char dat_00773dc8[6] = "Basse";
static char dat_00773dd0[7] = "Touche";
static char dat_00773dd8[11] = "BMD Gauche";
static char dat_00773de4[8] = "BMD Bas";
static char dat_00773dec[11] = "BMD Droite";
static char dat_00773df8[9] = "BMD Haut";
static char dat_00773f08[12] = "Sem entrada";
static char dat_00773f14[15] = "Mouse bot\303\243o N";
static char dat_00773f24[21] = "Mouse bot\303\243o do meio";
static char dat_00773f3c[21] = "Mouse bot\303\243o direito";
static char dat_00773f54[22] = "Mouse bot\303\243o esquerdo";
static char dat_00773f6c[21] = "Mouse roda para cima";
static char dat_00773f84[17] = "Mouse para baixo";
static char dat_00773f98[21] = "Mouse para a direita";
static char dat_00773fb0[22] = "Mouse roda para baixo";
static char dat_00773fc8[16] = "Mouse para cima";
static char dat_00773fd8[22] = "Mouse para a esquerda";
static char dat_00773ff0[8] = "Z Mouse";
static char dat_00773ff8[8] = "Y Mouse";
static char dat_00774000[8] = "X Mouse";
static char dat_00774008[7] = "Voltar";
static char dat_00774010[8] = "Iniciar";
static char dat_00774018[19] = "Direcional direito";
static char dat_0077402c[20] = "Direcional esquerdo";
static char dat_00774040[16] = "Gatilho direito";
static char dat_00774050[18] = "Gatilho Inf. Dir.";
static char dat_00774064[18] = "Gatilho Inf. Esq.";
static char dat_00774078[9] = "Bot\303\243o X";
static char dat_00774084[9] = "Bot\303\243o A";
static char dat_00774090[9] = "Bot\303\243o B";
static char dat_0077409c[9] = "Bot\303\243o Y";
static char dat_007740a8[21] = "Direc. dir. p/ baixo";
static char dat_007740c0[20] = "Direc. dir. p/ cima";
static char dat_007740d4[20] = "Direc. dir. p/ dir.";
static char dat_007740e8[19] = "Direc. dir. p/ esq";
static char dat_007740fc[24] = "Deixe o direc. p/ baixo";
static char dat_00774114[23] = "Deixe o direc. p/ cima";
static char dat_0077412c[23] = "Deixe o direc. p/ dir.";
static char dat_00774144[23] = "Deixe o direc. p/ esq.";
static char dat_0077415c[12] = "Rota\303\247\303\243o Z";
static char dat_00774168[12] = "Rota\303\247\303\243o Y";
static char dat_00774174[12] = "Rota\303\247\303\243o X";
static char dat_00774188[19] = "Anal\303\263gico %d (%s)";
static char dat_0077419c[5] = "Alto";
static char dat_007741a4[6] = "Baixa";
static char dat_007741ac[7] = "Bot\303\243o";
static char dat_007741b4[15] = "Direc. p/ esq.";
static char dat_007741c4[16] = "Direc. p/ baixo";
static char dat_007741d4[18] = "Direc. p/ direita";
static char dat_007741e8[15] = "Direc. p/ cima";
static char dat_007742a8[14] = "Ei sy\303\266tett\303\244";
static char dat_007742b8[17] = "Hiiren painike N";
static char dat_007742cc[20] = "Hiiren rullapainike";
static char dat_007742e0[21] = "Hiiren oikea painike";
static char dat_007742f8[21] = "Hiiren vasen painike";
static char dat_00774310[19] = "Hiiren rulla yl\303\266s";
static char dat_00774324[11] = "Hiiri alas";
static char dat_00774330[15] = "Hiiri oikealle";
static char dat_00774340[18] = "Hiiren rulla alas";
static char dat_00774354[12] = "Hiiri yl\303\266s";
static char dat_00774360[17] = "Hiiri vasemmalle";
static char dat_00774374[16] = "Hiiren Z-akseli";
static char dat_00774384[16] = "Hiiren Y-akseli";
static char dat_00774394[16] = "Hiiren X-akseli";
static char dat_007743a4[9] = "Takaisin";
static char dat_007743b0[7] = "Aloita";
static char dat_007743b8[19] = "Oikea peukalosauva";
static char dat_007743cc[19] = "Vasen peukalosauva";
static char dat_007743e0[12] = "Oikea sauva";
static char dat_007743ec[16] = "Vasen liipaisin";
static char dat_007743fc[20] = "Oikea olkan\303\244pp\303\244in";
static char dat_00774410[20] = "Vasen olkan\303\244pp\303\244in";
static char dat_00774424[12] = "N\303\244pp\303\244in X";
static char dat_00774430[12] = "N\303\244pp\303\244in A";
static char dat_0077443c[12] = "N\303\244pp\303\244in B";
static char dat_00774448[12] = "N\303\244pp\303\244in Y";
static char dat_00774454[17] = "Oikea sauva alas";
static char dat_00774468[18] = "Oikea sauva yl\303\266s";
static char dat_0077447c[18] = "Oikea sauva oikea";
static char dat_00774490[18] = "Oikea sauva vasen";
static char dat_007744a4[17] = "Vasen sauva alas";
static char dat_007744b8[18] = "Vasen sauva yl\303\266s";
static char dat_007744cc[18] = "Vasen sauva oikea";
static char dat_007744e0[18] = "Vasen sauva vasen";
static char dat_007744f4[11] = "Z-rotaatio";
static char dat_00774500[11] = "Y-rotaatio";
static char dat_0077450c[11] = "X-rotaatio";
static char dat_00774518[9] = "Z-akseli";
static char dat_00774524[9] = "Y-akseli";
static char dat_00774530[9] = "X-akseli";
static char dat_0077453c[19] = "Analoginen %d (%s)";
static char dat_00774550[7] = "Korkea";
static char dat_00774558[7] = "Matala";
static char dat_00774560[10] = "N\303\244pp\303\244in";
static char dat_0077456c[12] = "Hattu vasen";
static char dat_00774578[11] = "Hattu alas";
static char dat_00774584[12] = "Hattu oikea";
static char dat_00774590[12] = "Hattu yl\303\266s";
static char dat_00774664[16] = "Ingen inmatning";
static char dat_00774674[11] = "Musknapp N";
static char dat_00774680[21] = "Mittersta musknappen";
static char dat_00774698[16] = "H\303\266ger musknapp";
static char dat_007746a8[18] = "V\303\244nster musknapp";
static char dat_007746bc[12] = "Mushjul upp";
static char dat_007746c8[11] = "Mus h\303\266ger";
static char dat_007746d4[12] = "Mushjul ned";
static char dat_007746e0[8] = "Mus upp";
static char dat_007746e8[13] = "Mus v\303\244nster";
static char dat_007746f8[9] = "Tillbaka";
static char dat_00774704[16] = "H\303\266ger styrspak";
static char dat_00774714[18] = "V\303\244nster styrspak";
static char dat_00774728[18] = "H\303\266ger avtryckare";
static char dat_0077473c[3] = "LT";
static char dat_00774740[8] = "R-knapp";
static char dat_00774748[8] = "L-knapp";
static char dat_00774750[8] = "Knapp X";
static char dat_00774758[8] = "Knapp A";
static char dat_00774760[8] = "Knapp B";
static char dat_00774768[8] = "Knapp Y";
static char dat_00774770[16] = "H\303\266ger Spak Ner";
static char dat_00774780[16] = "H\303\266ger Spak Upp";
static char dat_00774790[19] = "H\303\266ger Spak H\303\266ger";
static char dat_007747a4[21] = "H\303\266ger Spak V\303\244nster";
static char dat_007747bc[18] = "V\303\244nster Spak Ner";
static char dat_007747d0[18] = "V\303\244nster Spak Upp";
static char dat_007747e4[21] = "V\303\244nster Spak H\303\266ger";
static char dat_007747fc[23] = "V\303\244nster Spak V\303\244nster";
static char dat_00774814[11] = "Z-rotation";
static char dat_00774820[11] = "Y-rotation";
static char dat_0077482c[11] = "X-rotation";
static char dat_00774838[7] = "Z-axel";
static char dat_00774840[7] = "Y-axel";
static char dat_00774848[7] = "X-axel";
static char dat_00774850[5] = "H\303\266g";
static char dat_00774858[5] = "L\303\245g";
static char dat_00774860[24] = "Riktningsknapp V\303\244nster";
static char dat_00774878[19] = "Riktningsknapp Ner";
static char dat_0077488c[22] = "Riktningsknapp H\303\266ger";
static char dat_007748a4[19] = "Riktningsknapp Upp";
static char dat_00774948[12] = "Ingen input";
static char dat_00774954[12] = "Museknapp N";
static char dat_00774960[17] = "Midtre museknapp";
static char dat_00774974[17] = "H\303\270yre museknapp";
static char dat_00774988[18] = "Venstre museknapp";
static char dat_0077499c[13] = "Musehjul opp";
static char dat_007749ac[8] = "Mus ned";
static char dat_007749b4[11] = "Mus h\303\270yre";
static char dat_007749c0[13] = "Musehjul ned";
static char dat_007749d0[8] = "Mus opp";
static char dat_007749d8[12] = "Mus venstre";
static char dat_007749e4[6] = "Mus Z";
static char dat_007749ec[6] = "Mus Y";
static char dat_007749f4[6] = "Mus X";
static char dat_007749fc[8] = "Tilbake";
static char dat_00774a04[12] = "H\303\270yre spak";
static char dat_00774a10[13] = "Venstre spak";
static char dat_00774a20[17] = "H\303\270yre avtrekker";
static char dat_00774a34[18] = "Venstre avtrekker";
static char dat_00774a48[20] = "H\303\270yre skulderknapp";
static char dat_00774a5c[21] = "Venstre skulderknapp";
static char dat_00774a74[8] = "X-knapp";
static char dat_00774a7c[8] = "A-knapp";
static char dat_00774a84[8] = "B-knapp";
static char dat_00774a8c[8] = "Y-knapp";
static char dat_00774a94[21] = "H\303\270yre styrespak ned";
static char dat_00774aac[21] = "H\303\270yre styrespak opp";
static char dat_00774ac4[24] = "H\303\270yre styrespak h\303\270yre";
static char dat_00774adc[25] = "H\303\270yre styrespak venstre";
static char dat_00774af8[22] = "Venstre styrespak ned";
static char dat_00774b10[22] = "Venstre styrespak opp";
static char dat_00774b28[25] = "Venstre styrespak h\303\270yre";
static char dat_00774b44[26] = "Venstre styrespak venstre";
static char dat_00774b60[11] = "Z-rotasjon";
static char dat_00774b6c[11] = "Y-rotasjon";
static char dat_00774b78[11] = "X-rotasjon";
static char dat_00774b84[7] = "Z-akse";
static char dat_00774b8c[7] = "Y-akse";
static char dat_00774b94[7] = "X-akse";
static char dat_00774b9c[15] = "Analog %d (%s)";
static char dat_00774bac[5] = "H\303\270y";
static char dat_00774bb4[4] = "Lav";
static char dat_00774bb8[6] = "Knapp";
static char dat_00774bc0[19] = "Hatteknapp venstre";
static char dat_00774bd4[15] = "Hatteknapp ned";
static char dat_00774be4[18] = "Hatteknapp h\303\270yre";
static char dat_00774bf8[15] = "Hatteknapp opp";
static char dat_00774cd8[12] = "Geen invoer";
static char dat_00774ce4[11] = "Muisknop N";
static char dat_00774cf0[19] = "Middelste muisknop";
static char dat_00774d04[16] = "Rechtermuisknop";
static char dat_00774d14[15] = "Linkermuisknop";
static char dat_00774d24[16] = "Muiswiel omhoog";
static char dat_00774d34[12] = "Muis omlaag";
static char dat_00774d40[12] = "Muis rechts";
static char dat_00774d4c[16] = "Muiswiel omlaag";
static char dat_00774d5c[12] = "Muis omhoog";
static char dat_00774d68[10] = "Muis Left";
static char dat_00774d74[7] = "Muis Z";
static char dat_00774d7c[7] = "Muis Y";
static char dat_00774d84[7] = "Muis X";
static char dat_00774d8c[6] = "Terug";
static char dat_00774d94[17] = "Rechter joystick";
static char dat_00774da8[16] = "Linker joystick";
static char dat_00774db8[16] = "Rechter trekker";
static char dat_00774dc8[15] = "Linker Trekker";
static char dat_00774dd8[17] = "Rechter Schouder";
static char dat_00774dec[16] = "Linker Schouder";
static char dat_00774dfc[7] = "Knop X";
static char dat_00774e04[7] = "Knop A";
static char dat_00774e0c[7] = "Knop B";
static char dat_00774e14[7] = "Knop Y";
static char dat_00774e1c[21] = "Rechter Stick Omlaag";
static char dat_00774e34[21] = "Rechter Stick Omhoog";
static char dat_00774e4c[21] = "Rechter Stick Rechts";
static char dat_00774e64[20] = "Rechter Stick Links";
static char dat_00774e78[20] = "Linker Stick Omlaag";
static char dat_00774e8c[20] = "Linker Stick Omhoog";
static char dat_00774ea0[20] = "Linker Stick Rechts";
static char dat_00774eb4[19] = "Linker Stick Links";
static char dat_00774ec8[10] = "Z Rotatie";
static char dat_00774ed4[10] = "Y Rotatie";
static char dat_00774ee0[10] = "X Rotatie";
static char dat_00774eec[5] = "Z As";
static char dat_00774ef4[5] = "Y As";
static char dat_00774efc[5] = "X As";
static char dat_00774f04[16] = "Analoog %d (%s)";
static char dat_00774f14[5] = "Hoog";
static char dat_00774f1c[5] = "Laag";
static char dat_00774f24[5] = "Knop";
static char dat_00774f2c[19] = "Navigatiepad Links";
static char dat_00774f40[20] = "Navigatiepad Omlaag";
static char dat_00774f54[20] = "Navigatiepad Rechts";
static char dat_00774f68[20] = "Navigatiepad Omhoog";
static char dat_00775064[13] = "\345\205\245\345\212\233\343\201\252\343\201\227";
static char dat_00775074[23] = "\343\203\236\343\202\246\343\202\271 \343\203\234\343\202\277\343\203\263 %d";
static char dat_0077508c[26] = "\343\203\236\343\202\246\343\202\271 \344\270\255\345\244\256\343\203\234\343\202\277\343\203\263";
static char dat_007750a8[23] = "\343\203\236\343\202\246\343\202\271 \345\217\263\343\203\234\343\202\277\343\203\263";
static char dat_007750c0[23] = "\343\203\236\343\202\246\343\202\271 \345\267\246\343\203\234\343\202\277\343\203\263";
static char dat_007750d8[26] = "\343\203\236\343\202\246\343\202\271 \343\203\233\343\202\244\343\203\274\343\203\253\344\270\212";
static char dat_007750f4[14] = "\343\203\236\343\202\246\343\202\271 \344\270\213";
static char dat_00775104[14] = "\343\203\236\343\202\246\343\202\271 \345\217\263";
static char dat_00775114[26] = "\343\203\236\343\202\246\343\202\271 \343\203\233\343\202\244\343\203\274\343\203\253\344\270\213";
static char dat_00775130[14] = "\343\203\236\343\202\246\343\202\271 \344\270\212";
static char dat_00775140[14] = "\343\203\236\343\202\246\343\202\271 \345\267\246";
static char dat_00775150[12] = "\343\203\236\343\202\246\343\202\271 Z";
static char dat_0077515c[12] = "\343\203\236\343\202\246\343\202\271 Y";
static char dat_00775168[12] = "\343\203\236\343\202\246\343\202\271 X";
static char dat_00775174[5] = "BACK";
static char dat_0077517c[26] = "\345\217\263 \350\246\252\346\214\207\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257";
static char dat_00775198[26] = "\345\267\246 \350\246\252\346\214\207\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257";
static char dat_007751b4[17] = "\345\217\263 \343\203\210\343\203\252\343\202\254\343\203\274";
static char dat_007751c8[17] = "\345\267\246 \343\203\210\343\203\252\343\202\254\343\203\274";
static char dat_007751dc[3] = "RB";
static char dat_007751e0[3] = "LB";
static char dat_007751e4[12] = "X \343\203\234\343\202\277\343\203\263";
static char dat_007751f0[12] = "A \343\203\234\343\202\277\343\203\263";
static char dat_007751fc[12] = "B \343\203\234\343\202\277\343\203\263";
static char dat_00775208[12] = "Y \343\203\234\343\202\277\343\203\263";
static char dat_00775214[23] = "\345\217\263\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \344\270\213";
static char dat_0077522c[23] = "\345\217\263\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \344\270\212";
static char dat_00775244[23] = "\345\217\263\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \345\217\263";
static char dat_0077525c[23] = "\345\217\263\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \345\267\246";
static char dat_00775274[23] = "\345\267\246\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \344\270\213";
static char dat_0077528c[23] = "\345\267\246\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \344\270\212";
static char dat_007752a4[23] = "\345\267\246\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \345\217\263";
static char dat_007752bc[23] = "\345\267\246\343\202\271\343\203\206\343\202\243\343\203\203\343\202\257 \345\267\246";
static char dat_007752d4[9] = "Z \345\233\236\350\273\242";
static char dat_007752e0[9] = "Y \345\233\236\350\273\242";
static char dat_007752ec[9] = "X \345\233\236\350\273\242";
static char dat_007752f8[6] = "Z \350\273\270";
static char dat_00775300[6] = "Y \350\273\270";
static char dat_00775308[6] = "X \350\273\270";
static char dat_00775310[21] = "\343\202\242\343\203\212\343\203\255\343\202\260 %d (%s)";
static char dat_00775328[4] = "\351\253\230";
static char dat_0077532c[4] = "\344\275\216";
static char dat_00775330[10] = "\343\203\234\343\202\277\343\203\263";
static char dat_0077533c[14] = "\343\203\217\343\203\203\343\203\210 \345\267\246";
static char dat_0077534c[14] = "\343\203\217\343\203\203\343\203\210 \344\270\213";
static char dat_0077535c[14] = "\343\203\217\343\203\203\343\203\210 \345\217\263";
static char dat_0077536c[14] = "\343\203\217\343\203\203\343\203\210 \344\270\212";
static char dat_00775418[9] = "No Input";
static char dat_00775424[16] = "Mouse Button %d";
static char dat_00775434[20] = "Mouse Middle Button";
static char dat_00775448[19] = "Mouse Right Button";
static char dat_0077545c[18] = "Mouse Left Button";
static char dat_00775470[15] = "Mouse Wheel Up";
static char dat_00775480[11] = "Mouse Down";
static char dat_0077548c[12] = "Mouse Right";
static char dat_00775498[17] = "Mouse Wheel Down";
static char dat_007754ac[9] = "Mouse Up";
static char dat_007754b8[11] = "Mouse Left";
static char dat_007754c4[8] = "Mouse Z";
static char dat_007754cc[8] = "Mouse Y";
static char dat_007754d4[8] = "Mouse X";
static char dat_007754dc[6] = "Start";
static char dat_007754e4[18] = "Right Thumb Stick";
static char dat_007754f8[17] = "Left Thumb Stick";
static char dat_0077550c[14] = "Right Trigger";
static char dat_0077551c[13] = "Left Trigger";
static char dat_0077552c[15] = "Right Shoulder";
static char dat_0077553c[14] = "Left Shoulder";
static char dat_0077554c[9] = "Button X";
static char dat_00775558[9] = "Button A";
static char dat_00775564[9] = "Button B";
static char dat_00775570[9] = "Button Y";
static char dat_0077557c[17] = "Right Stick Down";
static char dat_00775590[15] = "Right Stick Up";
static char dat_007755a0[18] = "Right Stick Right";
static char dat_007755b4[17] = "Right Stick Left";
static char dat_007755c8[16] = "Left Stick Down";
static char dat_007755d8[14] = "Left Stick Up";
static char dat_007755e8[17] = "Left Stick Right";
static char dat_007755fc[16] = "Left Stick Left";
static char dat_0077560c[11] = "Z Rotation";
static char dat_00775618[11] = "Y Rotation";
static char dat_00775624[11] = "X Rotation";
static char dat_00775630[7] = "Z Axis";
static char dat_00775638[7] = "Y Axis";
static char dat_00775640[7] = "X Axis";
static char dat_00775648[17] = "Analogue %d (%s)";
static char dat_0077565c[5] = "High";
static char dat_00775664[4] = "Low";
static char dat_00775668[7] = "Button";
static char dat_00775670[9] = "Hat Left";
static char dat_0077567c[9] = "Hat Down";
static char dat_00775688[10] = "Hat Right";
static char dat_00775694[7] = "Hat Up";
static char dat_00775eb4[10] = "GameTitle";
static char dat_00775ec0[7] = "TTales";

const char *PcCompanyName = dat_00775ec0;
const char *PcProductName = dat_00775eb4;
TEXT_INPUT_LABELS dat_00820560 = {
    {{dat_0077536c, dat_0077535c, dat_0077534c, dat_0077533c, dat_00775330, dat_0077532c, dat_00775328, dat_00775310,
      dat_00775308, dat_00775300, dat_007752f8, dat_007752ec, dat_007752e0, dat_007752d4, dat_007752bc, dat_007752a4,
      dat_0077528c, dat_00775274, dat_0077525c, dat_00775244, dat_0077522c, dat_00775214, dat_00775208, dat_007751fc,
      dat_007751f0, dat_007751e4, dat_007751e0, dat_007751dc, dat_007751c8, dat_007751b4, dat_00775198, dat_0077517c,
      dat_0076fe70, dat_00775174, dat_00775168, dat_0077515c, dat_00775150}},
    {dat_00775140, dat_00775130, dat_00775114},
    {dat_00775104, dat_007750f4, dat_007750d8},
    {dat_007750c0, dat_007750a8, dat_0077508c},
    dat_00775074,
    dat_00775064
};
TEXT_INPUT_LABELS dat_00820480 = {
    {{dat_00775694, dat_00775688,        dat_0077567c, dat_00775670, dat_00775668, dat_00775664, dat_0077565c, dat_00775648,
      dat_00775640, dat_00775638,        dat_00775630, dat_00775624, dat_00775618, dat_0077560c, dat_007755fc, dat_007755e8,
      dat_007755d8, dat_007755c8,        dat_007755b4, dat_007755a0, dat_00775590, dat_0077557c, dat_00775570, dat_00775564,
      dat_00775558, dat_0077554c,        dat_0077553c, dat_0077552c, dat_0077551c, dat_0077550c, dat_007754f8, dat_007754e4,
      dat_007754dc, UiBackPromptLiteral, dat_007754d4, dat_007754cc, dat_007754c4}},
    {dat_007754b8, dat_007754ac, dat_00775498},
    {dat_0077548c, dat_00775480, dat_00775470},
    {dat_0077545c, dat_00775448, dat_00775434},
    dat_00775424,
    dat_00775418
};
TEXT_INPUT_LABELS dat_00820aa0 = {
    {{dat_00773df8, dat_00773dec, dat_00773de4, dat_00773dd8, dat_00773dd0, dat_00773dc8, dat_00773dc0, dat_00773dac,
      dat_00773da4, dat_00773d9c, dat_00773d94, dat_00773d88, dat_00773d7c, dat_00773d70, dat_00773d4c, dat_00773d28,
      dat_00773d08, dat_00773ce8, dat_00773cc8, dat_00773ca8, dat_00773c88, dat_00773c68, dat_00773c5c, dat_00773c50,
      dat_00773c44, dat_00773c38, dat_00773c20, dat_00773c08, dat_00773bf4, dat_00773be0, dat_00773bc8, dat_00773bb0,
      dat_00773ba0, dat_00773b98, dat_00773b8c, dat_00773b80, dat_00773b74}},
    {dat_00773b5c, dat_00773b48, dat_00773b2c},
    {dat_00773b14, dat_00773b00, dat_00773ae4},
    {dat_00773acc, dat_00773ab8, dat_00773aa0},
    dat_00773a90,
    dat_00773a78
};
TEXT_INPUT_LABELS dat_00820d40 = {
    {{dat_00773278, dat_0077325c, dat_00773240, dat_00773220, dat_00773218, dat_00773210, dat_0077419c, dat_007731fc,
      dat_007731f4, dat_007731ec, dat_007731e4, dat_007731d8, dat_007731cc, dat_007731c0, dat_007731a0, dat_00773180,
      dat_00773168, dat_00773150, dat_00773130, dat_00773114, dat_007730fc, dat_007730e8, dat_007730dc, dat_007730d0,
      dat_007730c4, dat_007730b8, dat_0077309c, dat_00773080, dat_00773064, dat_00773048, dat_00773038, dat_00773028,
      dat_00773018, dat_00773010, dat_00773004, dat_00772ff8, dat_00772fec}},
    {dat_00772fd4, dat_00772fc0, dat_00772fa0},
    {dat_00772f8c, dat_00772f78, dat_00772f58},
    {dat_00772f3c, dat_00772f20, dat_00772f04},
    dat_00772ef4,
    dat_00772eec
};
TEXT_INPUT_LABELS dat_00820c60 = {
    {{dat_007735bc, dat_007735a8, dat_00773594, dat_00773580, dat_00773578, dat_00773570, dat_00773568, dat_00774b9c,
      dat_00773560, dat_00773558, dat_00773550, dat_00773544, dat_00773538, dat_0077352c, dat_00773518, dat_00773504,
      dat_007734f0, dat_007734dc, dat_007734c8, dat_007734b0, dat_0077349c, dat_00773488, dat_00773480, dat_00773478,
      dat_00773470, dat_00773468, dat_00773454, dat_0077343c, dat_0077342c, dat_00773418, dat_00773408, dat_007733f8,
      dat_007754dc, dat_007733f0, dat_007733e8, dat_007733e0, dat_007733d8}},
    {dat_007733cc, dat_007733c0, dat_007733b0},
    {dat_007733a4, dat_00773398, dat_00773388},
    {dat_00773378, dat_00773364, dat_00773350},
    dat_00773344,
    dat_00773334
};
TEXT_INPUT_LABELS dat_00820b80 = {
    {{dat_007739a0, dat_00773984, dat_0077396c, dat_00773950, dat_00773944, dat_0077393c, dat_00773934, dat_00775648,
      dat_0077392c, dat_00773924, dat_0077391c, dat_00773910, dat_00773904, dat_007738f8, dat_007738dc, dat_007738c0,
      dat_007738a8, dat_0077388c, dat_00773870, dat_00773858, dat_00773840, dat_00773828, dat_0077381c, dat_00773810,
      dat_00773804, dat_007737f8, dat_007737dc, dat_007737c4, dat_007737b0, dat_0077379c, dat_00773780, dat_00773768,
      dat_007754dc, dat_0077375c, dat_007754d4, dat_007754cc, dat_007754c4}},
    {dat_00773748, dat_0077373c, dat_00773724},
    {dat_00773714, dat_00773708, dat_007736f0},
    {dat_007736d4, dat_007736bc, dat_007736a0},
    dat_0077368c,
    dat_0077367c
};
TEXT_INPUT_LABELS dat_00820640 = {
    {{dat_00774f68, dat_00774f54, dat_00774f40, dat_00774f2c, dat_00774f24, dat_00774f1c, dat_00774f14, dat_00774f04,
      dat_00774efc, dat_00774ef4, dat_00774eec, dat_00774ee0, dat_00774ed4, dat_00774ec8, dat_00774eb4, dat_00774ea0,
      dat_00774e8c, dat_00774e78, dat_00774e64, dat_00774e4c, dat_00774e34, dat_00774e1c, dat_00774e14, dat_00774e0c,
      dat_00774e04, dat_00774dfc, dat_00774dec, dat_00774dd8, dat_00774dc8, dat_00774db8, dat_00774da8, dat_00774d94,
      dat_007754dc, dat_00774d8c, dat_00774d84, dat_00774d7c, dat_00774d74}},
    {dat_00774d68, dat_00774d5c, dat_00774d4c},
    {dat_00774d40, dat_00774d34, dat_00774d24},
    {dat_00774d14, dat_00774d04, dat_00774cf0},
    dat_00774ce4,
    dat_00774cd8
};
TEXT_INPUT_LABELS dat_008209c0 = {
    {{dat_007741e8, dat_007741d4, dat_007741c4, dat_007741b4, dat_007741ac, dat_007741a4, dat_0077419c, dat_00774188,
      dat_00759770, dat_00774184, dat_00774180, dat_00774174, dat_00774168, dat_0077415c, dat_00774144, dat_0077412c,
      dat_00774114, dat_007740fc, dat_007740e8, dat_007740d4, dat_007740c0, dat_007740a8, dat_0077409c, dat_00774090,
      dat_00774084, dat_00774078, dat_00774064, dat_00774050, dat_00774064, dat_00774040, dat_0077402c, dat_00774018,
      dat_00774010, dat_00774008, dat_00774000, dat_00773ff8, dat_00773ff0}},
    {dat_00773fd8, dat_00773fc8, dat_00773fb0},
    {dat_00773f98, dat_00773f84, dat_00773f6c},
    {dat_00773f54, dat_00773f3c, dat_00773f24},
    dat_00773f14,
    dat_00773f08
};
TEXT_INPUT_LABELS dat_00820e20 = {
    {{dat_00772e1c, dat_00772e08, dat_00772df8, dat_00772de4, dat_00772ddc, dat_00774bb4, dat_00772dd4, dat_00774b9c,
      dat_00772dcc, dat_00772dc4, dat_00772dbc, dat_00772db0, dat_00772da4, dat_00772d98, dat_00772d7c, dat_00772d60,
      dat_00772d48, dat_00772d30, dat_00772d14, dat_00772cfc, dat_00772ce8, dat_00772cd0, dat_00772cc8, dat_00772cc0,
      dat_00772cb8, dat_00772cb0, dat_00772c9c, dat_00772c88, dat_00772c74, dat_00772c88, dat_00772c60, dat_00772c4c,
      dat_007754dc, dat_00772c44, dat_00772c3c, dat_007749ec, dat_007749e4}},
    {dat_007749d8, dat_00772c34, dat_007746d4},
    {dat_00772c28, dat_007749ac, dat_00772c1c},
    {dat_00772c08, dat_00772bf8, dat_00772be4},
    dat_00772bd8,
    dat_00774948
};
TEXT_INPUT_LABELS dat_008210c0 = {
    {{dat_00772150, dat_00772138, dat_00772120, dat_00772108, dat_007720fc, dat_007720f0, dat_007720e4, dat_00774b9c,
      dat_007720dc, dat_007720d4, dat_007720cc, dat_007720c0, dat_007720b4, dat_007720a8, dat_00772094, dat_0077207c,
      dat_00772064, dat_00772050, dat_00772038, dat_00772020, dat_00772008, dat_00771ff0, dat_00771fe0, dat_00771fd0,
      dat_00771fc0, dat_00771fb0, dat_00771f94, dat_00771f78, dat_00771f68, dat_00771f58, dat_00771f48, dat_00771f38,
      dat_007754dc, dat_00771f30, dat_00771f28, dat_007720d4, dat_007720cc}},
    {dat_00771f14, dat_00771f00, dat_00771ee8},
    {dat_00771ed4, dat_00771ec0, dat_00771ea8},
    {dat_00771e90, dat_00771e78, dat_00771e5c},
    dat_00771e48,
    dat_00771e38
};
TEXT_INPUT_LABELS dat_00820f00 = {
    {{dat_00772b34, dat_00772b20, dat_00772b0c, dat_00772afc, dat_00772af0, dat_00772ae8, dat_00772ae0, dat_00772acc,
      dat_00772ac4, dat_00772abc, dat_00772ab4, dat_00772aa4, dat_00772a94, dat_00772a84, dat_00772a6c, dat_00772a54,
      dat_00772a3c, dat_00772a24, dat_00772a0c, dat_007729f4, dat_007729dc, dat_007729c4, dat_007729b8, dat_007729ac,
      dat_007729a0, dat_00772994, dat_0077297c, dat_00772964, dat_00772958, dat_0077294c, dat_0077293c, dat_0077292c,
      dat_007754dc, dat_00772924, dat_00772918, dat_0077290c, dat_00772900}},
    {dat_007728f0, dat_007728e0, dat_007728c8},
    {dat_007728e0, dat_007728b8, dat_007728a0},
    {dat_00772888, dat_00772870, dat_00772854},
    dat_00772840,
    dat_00772830
};
TEXT_INPUT_LABELS dat_00820fe0 = {
    {{dat_00772760, dat_00772750, dat_00772744, dat_00772738, dat_00772728, dat_00772718, dat_00772708, dat_007726e8,
      dat_007726dc, dat_007726d0, dat_007726c4, dat_007726ac, dat_00772694, dat_0077267c, dat_00772650, dat_00772624,
      dat_007725f8, dat_007725d0, dat_007725a4, dat_00772574, dat_00772548, dat_0077251c, dat_0077250c, dat_007724fc,
      dat_007724ec, dat_007724dc, dat_007724c0, dat_007724a4, dat_0077248c, dat_00772474, dat_00772458, dat_00772438,
      dat_0077242c, dat_00772420, dat_0077240c, dat_007723f8, dat_007723e4}},
    {dat_007723d0, dat_007723bc, dat_0077239c},
    {dat_00772384, dat_00772370, dat_0077234c},
    {dat_00772328, dat_00772304, dat_007722dc},
    dat_007722c4,
    dat_0077229c
};
TEXT_INPUT_LABELS dat_00820800 = {
    {{dat_007748a4, dat_0077488c, dat_00774878, dat_00774860, dat_00774bb8, dat_00774858, dat_00774850, dat_00774b9c,
      dat_00774848, dat_00774840, dat_00774838, dat_0077482c, dat_00774820, dat_00774814, dat_007747fc, dat_007747e4,
      dat_007747d0, dat_007747bc, dat_007747a4, dat_00774790, dat_00774780, dat_00774770, dat_00774768, dat_00774760,
      dat_00774758, dat_00774750, dat_00774748, dat_00774740, dat_0077473c, dat_00774728, dat_00774714, dat_00774704,
      dat_007754dc, dat_007746f8, dat_007749f4, dat_007749ec, dat_007749e4}},
    {dat_007746e8, dat_007746e0, dat_007746d4},
    {dat_007746c8, dat_007749ac, dat_007746bc},
    {dat_007746a8, dat_00774698, dat_00774680},
    dat_00774674,
    dat_00774664
};
TEXT_INPUT_LABELS dat_00820720 = {
    {{dat_00774bf8, dat_00774be4, dat_00774bd4, dat_00774bc0, dat_00774bb8, dat_00774bb4, dat_00774bac, dat_00774b9c,
      dat_00774b94, dat_00774b8c, dat_00774b84, dat_00774b78, dat_00774b6c, dat_00774b60, dat_00774b44, dat_00774b28,
      dat_00774b10, dat_00774af8, dat_00774adc, dat_00774ac4, dat_00774aac, dat_00774a94, dat_00774a8c, dat_00774a84,
      dat_00774a7c, dat_00774a74, dat_00774a5c, dat_00774a48, dat_00774a34, dat_00774a20, dat_00774a10, dat_00774a04,
      dat_007754dc, dat_007749fc, dat_007749f4, dat_007749ec, dat_007749e4}},
    {dat_007749d8, dat_007749d0, dat_007749c0},
    {dat_007749b4, dat_007749ac, dat_0077499c},
    {dat_00774988, dat_00774974, dat_00774960},
    dat_00774954,
    dat_00774948
};
TEXT_INPUT_LABELS dat_008208e0 = {
    {{dat_00774590, dat_00774584, dat_00774578, dat_0077456c, dat_00774560, dat_00774558, dat_00774550, dat_0077453c,
      dat_00774530, dat_00774524, dat_00774518, dat_0077450c, dat_00774500, dat_007744f4, dat_007744e0, dat_007744cc,
      dat_007744b8, dat_007744a4, dat_00774490, dat_0077447c, dat_00774468, dat_00774454, dat_00774448, dat_0077443c,
      dat_00774430, dat_00774424, dat_00774410, dat_007743fc, dat_007743ec, dat_007743e0, dat_007743cc, dat_007743b8,
      dat_007743b0, dat_007743a4, dat_00774394, dat_00774384, dat_00774374}},
    {dat_00774360, dat_00774354, dat_00774340},
    {dat_00774330, dat_00774324, dat_00774310},
    {dat_007742f8, dat_007742e0, dat_007742cc},
    dat_007742b8,
    dat_007742a8
};
/* Entries past the end of a 222-entry table read the next table. */
char *dat_00821558[238] = {
    dat_00771d80, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_00771d50, dat_00771d48, dat_00771d3c, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00753374,
    dat_00753370, dat_00771d0c, dat_00771d04, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00771cd4, dat_00771cc8, dat_00771cc0, dat_00771cb4, "\\",         dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00771c94, dat_00771c8c, dat_00771c80,
    dat_00771c74, dat_00771c6c, dat_00771c64, dat_00771c5c, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_007719b0, dat_007516c0, dat_007516c0, dat_007719a8, dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00771afc, dat_00771af4, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_0077199c,
    dat_007516c0, dat_00771aa4, dat_007516c0, dat_00771a9c, dat_00771a94, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_00771a80, dat_00771a78, dat_007516c0, dat_00771a70, dat_007516c0, dat_00771a68, dat_007516c0,
    dat_00771a64, dat_0075ac34, dat_00771a58, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a48, dat_00771a40, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
char *dat_00821910[238] = {
    dat_00771d80, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_00771d50, dat_00771d48, dat_00771d3c, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00753374,
    dat_00753370, dat_00771d0c, dat_00771990, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00771978, dat_0077195c, dat_0077194c, dat_00771940, "\\",         dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00771930, dat_00771920, dat_007718fc,
    dat_007718f0, dat_007718dc, dat_007718d4, dat_007718c4, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_007718b0,
    dat_0077189c, dat_00771888, dat_00771874, dat_00771860, dat_0077184c, dat_00771838, dat_00771824, dat_00771810, dat_007717fc,
    dat_007717e8, dat_007717d4, dat_007717c0, dat_007516c0, dat_007516c0, dat_007719a8, dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_007717a8, dat_0077179c, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_00771788,
    dat_007516c0, dat_00771774, dat_007516c0, dat_00771a9c, dat_0077176c, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_00771760, dat_00771758, dat_007516c0, dat_0077174c, dat_007516c0, dat_00771740, dat_007516c0,
    dat_00771a64, dat_00771734, dat_00771728, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771710, dat_007716f8, dat_007716dc, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
char *dat_00821cc8[238] = {
    dat_007716d4, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_007716cc, dat_007716c0, dat_00771d3c, dat_007716b4, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774180, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_007716b0,
    dat_007716a4, dat_00771d0c, dat_0077169c, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00771698, dat_0077168c, dat_00771684, dat_0077167c, dat_00771670, dat_00774184,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00771668, dat_00771660, dat_00771654,
    dat_0077164c, dat_00771644, dat_0077163c, dat_00771630, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771628,
    dat_00771620, dat_00771618, dat_00771610, dat_00771608, dat_00771600, dat_007715f8, dat_007715f0, dat_007715e8, dat_007715e0,
    dat_007715d8, dat_007715d0, dat_007715c8, dat_007516c0, dat_007516c0, "\\",         dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007715b8, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_007715ac, dat_007715a4, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_0077199c,
    dat_007516c0, dat_0077159c, dat_007516c0, dat_00771a9c, dat_00771594, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_0077158c, dat_00771584, dat_007516c0, dat_0077157c, dat_007516c0, dat_00771574, dat_007516c0,
    dat_00771a64, dat_0077156c, dat_00771564, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_0077155c, dat_00771554, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
char *dat_00822080[222] = {
    dat_00771d80, dat_0077154c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_00755d70, dat_00771544, dat_00773b98, dat_00771538, dat_00771d00, dat_00774180,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00771534,
    dat_00771530, dat_00771528, dat_00771520, dat_00771d34, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00771c9c, dat_0077151c, dat_00771518, dat_00771510, dat_0077150c, dat_00771d30,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771504, dat_007714f4, dat_007714e8, dat_007714d4,
    dat_007714cc, dat_007714c0, dat_007714b8, dat_007714b0, dat_007714a4, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771498, dat_00771488, dat_0077147c,
    dat_00771470, dat_00771464, dat_00771458, dat_0077144c, dat_00771440, dat_00771434, dat_00771428, dat_0077141c, dat_00771410,
    dat_00771404, dat_007713f8, dat_007713e8, dat_007516c0, dat_007516c0, dat_007713e4, dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007713d8, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_007713d4, dat_00771c9c, dat_007713d0, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_007713bc, dat_007713b4, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_007713b0, dat_0077139c,
    dat_007516c0, dat_00771390, dat_007516c0, dat_0077137c, dat_00771374, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_0077136c, dat_00771364, dat_00771358, dat_007516c0, dat_00771350, dat_007516c0, dat_00771348, dat_007516c0,
    dat_00771344, dat_00771340, dat_00771334, dat_0077132c, dat_00771324, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771318, dat_0077130c, dat_00771a38};
char *dat_008223f8[222] = {
    dat_007716d4, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_00771300, dat_007712fc, dat_0077375c, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_007712f8,
    dat_007712f4, dat_007712ec, dat_007712e0, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_007712dc, dat_007712d8, "\\",         dat_007712cc, dat_007712c8, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_007712c0, dat_007712b8, dat_007712b4,
    dat_007712a8, dat_0077129c, dat_00771290, dat_00771288, dat_0077127c, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771270, dat_00771260, dat_00771254,
    dat_00771248, dat_0077123c, dat_00771230, dat_00771224, dat_00771218, dat_0077120c, dat_00771200, dat_007711f4, dat_007711e8,
    dat_007711dc, dat_007711d0, dat_007711c4, dat_007516c0, dat_007516c0, dat_007713e4, dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007711b8, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_0075c044, "_",          dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_007711a8, dat_0077119c, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771190, dat_007516c0, dat_00771188, dat_007516c0, dat_00771ab8, dat_0077117c,
    dat_007516c0, dat_00771170, dat_007516c0, dat_00771168, dat_0077115c, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771154,
    dat_007516c0, dat_00771a84, dat_00771148, dat_00771140, dat_007516c0, dat_00771130, dat_007516c0, dat_00771120, dat_007516c0,
    dat_00771118, dat_00771108, dat_007710fc, dat_007710f0, dat_007710e8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007710dc, dat_007710d0, dat_00771a38};
char *dat_00822770[222] = {
    dat_007710c8, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_007710c4, dat_007710c0, dat_007710b4, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774180, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_007710b0,
    dat_007712f4, dat_007710a8, dat_007710a0, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_0077109c, dat_00771098, dat_00771534, dat_0077108c, dat_00771cb0, dat_00774184,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00771088, dat_0075eba8, dat_007712b4,
    dat_0077107c, dat_00771c6c, dat_00771074, dat_00771068, dat_00771058, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_0077104c, dat_00771044, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_007715c8, dat_007516c0, dat_007516c0, dat_007713e4, dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771040, dat_0077103c, "?",          dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771030, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00771020, dat_00771018, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_00771010,
    dat_007516c0, dat_00771aa4, dat_007516c0, dat_00771008, dat_00771000, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00770ff8, dat_00770fec, dat_00770fe0, dat_007516c0, dat_00770fd4, dat_007516c0, dat_00770fc4, dat_007516c0,
    dat_00770fbc, dat_00770fb0, dat_00770fa8, dat_00770fa0, dat_00770f98, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770f88, dat_00770f78, dat_00771a38};
char *dat_00822ae8[222] = {
    dat_00770f6c, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_00770f68, dat_00770f64, dat_00770f58, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00770f54,
    dat_007712f4, dat_00770f4c, dat_00770f40, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00770f3c, dat_007710c0, dat_00770f38, dat_00770f2c, dat_00770f28, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00770f20, dat_007712b8, dat_00770f18,
    dat_00770f0c, dat_00770f04, dat_00770ef8, dat_00770ef0, dat_00770ee4, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00770ed8, dat_00770ecc, dat_00770ec4,
    dat_00770ebc, dat_00770eb4, dat_00770eac, dat_00770ea4, dat_00770e9c, dat_00770e94, dat_00770e8c, dat_00770e84, dat_00770e7c,
    dat_00770e74, dat_00770e6c, dat_00770e60, dat_007516c0, dat_007516c0, dat_007713e4, dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00770e54, dat_007516c0, dat_007516c0,
    dat_00770e48, dat_00770e40, dat_00770e34, dat_00770e28, dat_00771b2c, dat_00770e20, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00770e14, dat_007516c0, dat_007516c0, dat_00770e08, dat_00770dfc, dat_007516c0, dat_007516c0, dat_00770df0, dat_00771ae4,
    dat_00770de8, dat_007516c0, dat_00770e20, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770ddc, dat_007516c0, dat_00770dd0, dat_007516c0, dat_00770dc4, dat_00770db8,
    dat_007516c0, dat_00770db0, dat_007516c0, dat_00770da8, dat_00770d9c, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771154,
    dat_007516c0, dat_00770d94, dat_00770d88, dat_00770d80, dat_007516c0, dat_00770d74, dat_007516c0, dat_00770d68, dat_007516c0,
    dat_00771344, dat_00770d58, dat_00770d50, dat_00771a50, dat_00770d48, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770d3c, dat_00770d30, dat_00771a38};
char *dat_00822e60[222] = {
    dat_00770d28, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_007712f4, dat_00770d20, dat_00772c44, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00770d1c,
    dat_00770d18, dat_00771d0c, dat_00770d10, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00770d0c, dat_00770d08, dat_00770d04, dat_00770cf8, dat_0077168c, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00770cf0, dat_00770ce8, dat_00771d50,
    dat_00770cdc, dat_00771c6c, dat_00770cd4, dat_00770cc4, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_00770cb4, dat_007516c0, dat_007516c0, dat_007713e4, dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00770ca4, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00770c94, dat_00770c88, dat_00770c74, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00770c64, dat_007516c0,
    dat_00770c58, dat_007516c0, dat_007516c0, dat_00771afc, dat_00770c50, dat_007516c0, dat_007516c0, dat_00771aec, dat_00770c48,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770c40, dat_007516c0, dat_00770c38, dat_007516c0, dat_00771ab8, dat_00770c2c,
    dat_007516c0, dat_00771aa4, dat_007516c0, dat_00770c1c, dat_00770c14, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_00770c0c, dat_00771758, dat_007516c0, dat_00770c00, dat_007516c0, dat_00770bf4, dat_007516c0,
    dat_00771a64, dat_00770bec, dat_00771728, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770bdc, dat_00770bcc, dat_00771a38};
char *dat_008231d8[238] = {
    dat_00771d80, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, "/",          dat_00770bc8, dat_00771d3c, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00770d18,
    dat_0077150c, dat_00771d0c, dat_00771d04, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_007712f4, dat_0077168c, dat_00770bc4, dat_00771cb4, dat_00771cb0, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00770cf0, dat_00770bbc, dat_00771d50,
    dat_00771c74, dat_00771c6c, dat_00771c64, dat_00770bb4, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_00770ba8, dat_007516c0, dat_007516c0, dat_00753374, dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00771afc, dat_00771af4, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_00770c2c,
    dat_007516c0, dat_00771aa4, dat_007516c0, dat_00771a9c, dat_00771a94, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_00770ba0, dat_00771584, dat_007516c0, dat_00770b98, dat_007516c0, dat_00770b90, dat_007516c0,
    dat_00771a64, dat_00770b88, dat_00771564, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_0077155c, dat_00770b80, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
char *dat_00823590[238] = {
    dat_00771d80, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_007712b4, "=",          dat_00771d3c, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00753374,
    dat_00753370, dat_00771d0c, dat_00771d04, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00770b7c, dat_00770f68, dat_00770b74, dat_00771cb4, dat_00771cb0, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00771088, dat_0075eba8, "/",
    dat_00770b68, dat_00771c6c, dat_00771c64, dat_00770b60, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_00770b58, dat_007516c0, dat_007516c0, "\\",         dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00771afc, dat_00770b50, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_0077199c,
    dat_007516c0, dat_00771aa4, dat_007516c0, dat_00771a9c, dat_00770b48, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_00770b40, dat_00771758, dat_007516c0, dat_00770b38, dat_007516c0, dat_00770b30, dat_007516c0,
    dat_00771a64, dat_00770b28, dat_00771728, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a48, dat_00770b20, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
char *dat_00823948[238] = {
    dat_00771d80, dat_00770b1c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_007712b4, "=",          dat_00771d3c, dat_00771d38, dat_00770b18, dat_00770b14,
    dat_00770b10, dat_00770b0c, dat_00770b08, dat_00770b04, dat_00770b00, dat_00770afc, dat_00770af8, dat_00770af4, dat_00770af0,
    dat_00770aec, dat_00771d0c, dat_00770ae0, dat_00770adc, dat_00770ad8, dat_00770ad4, dat_00770ad0, dat_00770acc, dat_00770ac8,
    dat_00770ac4, dat_00770ac0, dat_00770abc, dat_00770ab8, dat_00770ab4, dat_00770f54, dat_00770aa4, "\\",         dat_00770aa0,
    dat_00770a9c, dat_00770a98, dat_00770a94, dat_00770a90, dat_00770a8c, dat_00770a88, dat_00770a84, dat_00770a80, dat_0075eba8,
    dat_00770a70, dat_00770a5c, dat_00770a50, dat_00770a40, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00770a2c,
    dat_00770a18, dat_00770a04, dat_007709f0, dat_007709dc, dat_007709c8, dat_007709b4, dat_007709a0, dat_0077098c, dat_00770978,
    dat_00770964, dat_00770950, dat_0077093c, dat_007516c0, dat_007516c0, dat_00771530, dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00770928, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00770910, dat_00770900, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_007708ec,
    dat_007516c0, dat_007708d8, dat_007516c0, dat_00771a9c, dat_007708c8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_00772760, dat_00771758, dat_007516c0, dat_00772738, dat_007516c0, dat_00772750, dat_007516c0,
    dat_00771a64, dat_00772744, dat_00771728, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007708bc, dat_007708ac, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
char *dat_00823d00[238] = {
    dat_00771d80, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_00770f18, dat_007708a4, dat_00770898, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_0077088c,
    dat_00770878, dat_00771d0c, dat_0077086c, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00770f28, dat_00770868, dat_00770858, dat_0077084c, dat_00770838, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_0077082c, dat_00770824, dat_0077080c,
    dat_00770800, dat_00771c6c, dat_007707f4, dat_007707e0, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_007707d4, dat_007516c0, dat_007516c0, "\\",         dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00771afc, dat_007707c8, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_0077199c,
    dat_007516c0, dat_00771aa4, dat_007516c0, dat_00771a9c, dat_007707bc, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771154,
    dat_007516c0, dat_00771a84, dat_007707ac, dat_00771584, dat_007516c0, dat_00770798, dat_007516c0, dat_00770784, dat_007516c0,
    dat_00771a64, dat_00770774, dat_00771564, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770768, dat_0077075c, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
char *dat_008240b8[238] = {
    dat_00771d80, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_00770f18, dat_007708a4, dat_00770898, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_0077088c,
    dat_00770878, dat_00771d0c, dat_0077086c, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00770f28, dat_00770868, dat_00770858, dat_0077084c, dat_00770838, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_0077082c, dat_00770824, dat_0077080c,
    dat_00770800, dat_00771c6c, dat_007707f4, dat_007707e0, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_007707d4, dat_007516c0, dat_007516c0, "\\",         dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00771afc, dat_007707c8, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_00770750,
    dat_007516c0, dat_00770748, dat_007516c0, dat_00771a9c, dat_007707bc, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771154,
    dat_007516c0, dat_00771a84, dat_007707ac, dat_00771584, dat_007516c0, dat_00770798, dat_007516c0, dat_00770784, dat_007516c0,
    dat_00771a64, dat_00770774, dat_00771564, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770768, dat_0077075c, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
char *dat_00824470[238] = {
    dat_00771d80, dat_00770b1c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_007712f4, "\\",         dat_00771d3c, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00770d1c,
    dat_00770d18, dat_00771d0c, dat_00770d10, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00770d08, dat_00770d0c, dat_00771530, dat_00770cf8, dat_00770f68, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00770cf0, dat_00770ce8, dat_007712b4,
    dat_00770cdc, dat_00771c6c, dat_00770cd4, dat_00771c5c, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_00770cb4, dat_007516c0, dat_007516c0, "<",          dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00771afc, dat_00770c50, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_0077199c,
    dat_007516c0, dat_00771aa4, dat_007516c0, dat_00771a9c, dat_00770c14, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_00770744, dat_00771584, dat_007516c0, dat_0077073c, dat_007516c0, dat_00770734, dat_007516c0,
    dat_00771a64, dat_00770730, dat_00771564, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770728, dat_00770720, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
char *dat_00824828[238] = {
    dat_00771d80, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_007712f4, dat_007710c0, dat_00771d3c, dat_00771d38, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00770d1c,
    dat_00770d18, dat_00771d0c, dat_00770d10, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_0077109c, dat_00771098, dat_007517a8, dat_00770cf8, dat_00770f68, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00770714, dat_0077070c, dat_00771d50,
    dat_00770cdc, dat_00771c6c, dat_00770cd4, dat_00770700, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_007706f4, dat_007516c0, dat_007516c0, "<",          dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00771afc, dat_00770c50, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_007706e0,
    dat_007516c0, dat_00771aa4, dat_007516c0, dat_007706d4, dat_00770c14, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_007706d0, dat_00771584, dat_007516c0, dat_007706c4, dat_007516c0, dat_007706bc, dat_007516c0,
    dat_00771a64, dat_00770730, dat_00771564, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770728, dat_00770720, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
char *dat_00824be0[238] = {
    dat_00771d80, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
    dat_00771d60, dat_00771d5c, dat_00771d58, dat_007712f4, dat_007710c0, dat_007706ac, dat_007706a4, dat_00771d34, dat_00771d30,
    dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00770d1c,
    dat_00770d18, dat_00771d0c, dat_00770d10, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
    dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_0077109c, dat_00771098, dat_007517a8, dat_00770cf8, dat_00770f68, dat_00774180,
    dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_0077069c, dat_00770694, dat_0077068c,
    dat_00770680, dat_00771c6c, dat_00770cd4, dat_00770678, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
    dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
    dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
    dat_00771bb8, dat_00771bb0, dat_0077066c, dat_007516c0, dat_007516c0, "<",          dat_00771ba0, dat_00771b9c, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
    dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
    dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
    dat_00771b08, dat_007516c0, dat_007516c0, dat_00771afc, dat_00770664, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
    dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_007706e0,
    dat_007516c0, dat_00771aa4, dat_007516c0, dat_007706d4, dat_0077065c, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
    dat_007516c0, dat_00771a84, dat_00770654, dat_00771584, dat_007516c0, dat_0077064c, dat_007516c0, dat_00770644, dat_007516c0,
    dat_00771a64, dat_0077063c, dat_00771564, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_007516c0, dat_00770728, dat_00770634, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
    dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
    dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};

extern "C" {

    i32 hub_startoutsidebonusdoor_area = -1;

}
extern "C" {

    i32 hub_from_superstory = -1;

    i32 hub_from_mission = -1;

    i32 newlevelfrommenu_newmenuy = 0;
}
extern "C" {
    LEVEL_INST_ANIM_RESET_VIEW LevInstAnimReset = {0};
}
extern "C" {
    extern const IID PCSoundBuffer8IID = {
        0x6825a449, 0x7524, 0x4d82, {0x92, 0xf, 0x50, 0xe3, 0x6a, 0xb3, 0xab, 0x1e}
    };

    PLAYER_BATARANG_SLOT BatarangPackets[8] = {0};

    i32 Hub_UsePlayerList = 0;

}

struct SoundQueue;
struct PC_SOUND_COMMIT;

// Startup and main-loop state; defaults follow the PC executable.
i32 ChallengeMode = 0;
i32 fade_draw_suppressed = 0;
i32 PLAYERCOUNT = 0;
i32 Area = -1;
i32 ResetOldFStop = 0;
i32 new_level_from_menu = 0;
i32 NewMode = 0;
i32 reset_load = 1;
volatile i32 waiting_for_level = 0;
i32 CUTSTOPGAME = 0;
MAINFRAMECOUNTERS MainFrameCounters = {0};
i32 hub_from_arcade = -1;
i32 UsePlayerList = 0;

extern "C" {
    u32 PCImplodeWordMask = 0xffffffff;
    BOLTSYS GlobalBoltSystem_Default = {
        &GlobalBoltType_Default, 1, NULL, Bolt_Debris_Default, Bolt_GetShootOrigin_Default, Bolt_GetShootDirection_Default, NULL, NULL,
    };
    NUSTRINGBANK StringBank[3] = {0};
    i16 CurrentStringBank = 0;
    u32 nustring_format = 1;
    extern const u16 bad_fluff[26] = {
        '!', '$', '%', '^', '&', '*', '(', ')', '_', '+', '-', '=', '{', '}', '[', ']', '@', '~', '#', '?', '<', '>', ',', '.', ' ', 0,
    };
    extern const f64 VehicleTurnPhaseScale = 65536.0;
    extern const f64 VehicleTurnPhaseOffset = 16384.0;
    extern const f64 MoveSplineRunSpeedSentinel = 1000000000.0;
    extern const f64 BoltTargetRandomDenominator = 65535.0;
    extern const f64 NuAnimDataFloatMax = 3.4028234663852886e38;
    u16 GameObjectRotationAtstId = 0xffff;
    char *NuBlankString = (char *)NuBlankStringValue;
    extern const f64 GameObjectNearFloorNoFloorHeight = 2000000.0;
    extern const f64 AIPathAngularScale = 10430.3779296875;
    extern const f64 PCAtan2RadianScale = 9.58738019107841e-05;
    extern const f64 PCPathArcSineCoefficient3 = -0.166667f;
    extern const f64 PCPathArcSineCoefficient5 = -0.075f;
    extern const f64 PCPathArcSineCoefficient7 = -0.0446429f;
    extern const f64 PCPathArcSineCoefficient9 = -0.0303819f;
    extern const char NuBlankStringValue[2] = " ";
    extern const NUVEC GAMEOBJECT_ROTATION_BUGGY_OFFSETS[4] = {
        {-0.1175f, 0.0f, 0.1575f },
        {0.1175f,  0.0f, 0.1575f },
        {0.1175f,  0.0f, -0.1575f},
        {-0.1175f, 0.0f, -0.1575f},
    };
    extern const NUVEC GAMEOBJECT_ROTATION_RADIOCAR_OFFSETS[4] = {
        {-0.12f, 0.0f, 0.08f },
        {0.12f,  0.0f, 0.08f },
        {0.12f,  0.0f, -0.08f},
        {-0.12f, 0.0f, -0.08f},
    };
    extern const NUVEC GAMEOBJECT_ROTATION_POLICECAR_OFFSETS[4] = {
        {-0.19f, 0.0f, 0.31f },
        {0.19f,  0.0f, 0.31f },
        {0.19f,  0.0f, -0.31f},
        {-0.19f, 0.0f, -0.31f},
    };
    extern const NUVEC GAMEOBJECT_ROTATION_MOWER_OFFSETS[4] = {
        {-0.1175f, 0.0f, 0.155f },
        {0.1175f,  0.0f, 0.155f },
        {0.1175f,  0.0f, -0.155f},
        {-0.1175f, 0.0f, -0.155f},
    };
    extern const NUVEC GAMEOBJECT_ROTATION_DEFAULT_OFFSETS[4] = {
        {-0.2f, 0.0f, 0.2f },
        {0.2f,  0.0f, 0.2f },
        {0.2f,  0.0f, -0.2f},
        {-0.2f, 0.0f, -0.2f},
    };

    extern const __declspec(align(16)) NUMTX ANI3_EVAL_IDENTITY_MATRIX = {
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    };
    TERRAIN_SET *CurrentTerrain = 0;
    TERRAIN_LAYER TerLayer[17] = {
        {0x00000000, 0x0000ffff, 0x40000000},
        {0x00000000, 0x0000ffff, 0x40000000},
        {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000001, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000020, 0x0000ffff, 0x3f800000},
        {0x00000001, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x3f800000},
        {0x00000000, 0x0000ffff, 0x00000000},
    };
    const FLOWBOX_CALLBACKS flowbox_callbacks[3] = {
        {ResetGizmoFlowBox, ProcessGizmoFlowBox,     CheckOutputGizmoFlowBox    },
        {NULL,              ProcessConditionFlowBox, CheckOutputConditionFlowBox},
        {NULL,              ProcessActionFlowBox,    CheckOutputActionFlowBox   },
    };
    extern const char AIScriptFalse[6] = "false";
    i32 nufile_blkcnt = 1;
    PC_NUFILE_READBLOCK nufile_blkinfo[1024] = {0};

    void *debris_cutoff_camera_vec = 0;
    f32 FRAMETIME = 0;
    extern const f32 NuPrimPS2XScale = 0.003125f;
    extern const f32 NuPrimPS2YScale = -1.0f / 112.0f;
    NUFPAR NuFParRecords[4] = {0};
    char NuFParBuffers[4][0x1000] = {0};
    i32 file_criticalsection = 0;
    PC_NUDATFILEINFO *unpack_file_info = 0;
    NUDATOPENFILEINFO *unpack_file_odi = 0;
    i64 unpack_file_pos = 0;
    extern const char PCKeyboardMouse2Name[16] = "KeyboardMouse2";
    extern const f64 NuQFntHalfScale = 0.5;
    GAMESAVE Game = {0};
    extern const f32 PermLegalScale = 2.2f;
    NUGSCN *save_icon_scene = 0;
    i32 nudlist_field_fc4 = 0;
    u32 nudlist_field_1598 = 0;
    LEVELDATA *DEATHSTARBATTLED_LDATA = 0;
    i16 id_ANAKINSPOD = -1;
    i16 id_ANAKINSPODGREEN = -1;
    i16 id_ANAKINSNEWPODGREEN = -1;
    i16 id_REPUBLICGUNSHIP_GREEN = -1;
    i16 id_NEW_REPUBLIC_GUNSHIP = -1;
    i16 id_NEW_REPUBLIC_GUNSHIP_GREEN = -1;
    GAMEOBJECT *Player[8] = {0};
    i16 ID_DEFAULTCHARACTER_A = -1;
    i16 ID_DEFAULTCHARACTER_B = -1;
    f32 DEFAULTFRAMETIME = 0;
    GAMEPAD_INIT_RECORD GamePadStorage[64] = {0};
    HANDLE pc_file_handles[32] = {
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
        INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE,
    };
    FILEINFO file_info[33] = {0};
    FILEBUFF file_buff[4] = {0};
    i32 nufile_buffering_enabled = 0;
    i32 datsys_offset = 0;
    i32 nufile_lasterror = 0;
    i32 nufile_try_packed = 0;
    NUFILE_DEVICE devices[16] = {0};
    i32 numdevices = 0;
    NUFILE_DEVICE host_device = {
        0,
        0,
        0x0e,
        0,
        '\\',
        0x400,
        -1,
        -1,
        -1,
        {0},
        {0},
        4,
        6,
        {0},
        "Host pc filesystem",
        {0},
        {0},
        {0},
        DEV_FormatName,
        DEVHOST_Interrogate,
    };
    char *iop_image_name = "ioprp300.img";
    NUFILE_DEVICE *default_device = &host_device;
    i32 DontDoFileUpperCaseHack = 0;
    HINSTANCE PCInputWindow = 0;
    HWND PCInputInstance = 0;
    PC_DEVICE_BACKEND PCDeviceBackend;
    NuKeyboardOwner nu_keyboard_owner;
    i32 nu_keyboard_device_flags = 0;
    i32 nu_keyboard_repeat_countdown = 0;

    __declspec(align(16)) extern const u32 ClipOutcodeMask32[4] = {32, 32, 32, 32};
    __declspec(align(16)) extern const u32 ClipOutcodeMask16[4] = {16, 16, 16, 16};
    __declspec(align(16)) extern const u32 ClipOutcodeMask1[4] = {1, 1, 1, 1};
    __declspec(align(16)) extern const u32 ClipOutcodeMask2[4] = {2, 2, 2, 2};
    __declspec(align(16)) extern const u32 ClipOutcodeMask8[4] = {8, 8, 8, 8};
    __declspec(align(16)) extern const u32 ClipOutcodeMask4[4] = {4, 4, 4, 4};
    __declspec(align(16)) extern const f32 ClipOutcodeMinusOne[4] = {-1.0f, -1.0f, -1.0f, -1.0f};
    NUCAMERA *pNuCam = 0;
#define IDENTITY_NUMTX 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    NUMTX NuMtxSetIdentityTemplate = {IDENTITY_NUMTX};
    NUMTX vmtx = {IDENTITY_NUMTX};
    NUMTX pmtx = {IDENTITY_NUMTX};
    NUMTX cmtx = {IDENTITY_NUMTX};
    NUMTX smtx = {IDENTITY_NUMTX};
    NUMTX vpmtx = {IDENTITY_NUMTX};
    NUMTX vpc_sci_mtx = {IDENTITY_NUMTX};
    NUMTX vpc_vport_mtx = {IDENTITY_NUMTX};
    NUMTX pc_vport_mtx = {IDENTITY_NUMTX};
    NUMTX vpsmtx = {IDENTITY_NUMTX};
    NUMTX clip_planes = {IDENTITY_NUMTX};
#undef IDENTITY_NUMTX
    f32 NuTrigTable[32768] = {0};

    extern const GUID SoundVoice3DBufferIID = {
        0x279afa86,
        0x4981,
        0x11ce,
        {0xa5, 0x21, 0x00, 0x20, 0xaf, 0x0b, 0xe5, 0x60},
    };
    PC_NUDATFILEINFO pc_dat_file_infos[20] = {0};
    PC_NUMEMFILE pc_memfiles[20] = {0};
    CHEATSYSTEM CheatSystem = {0};

    extern char AreaFixUpName_Jedi[8] = "Jedi";
    extern char AreaFixUpName_Gungan[8] = "Gungan";
    extern char AreaFixUpName_Senate[8] = "senate";
    extern char AreaFixUpName_Losttemple[12] = "losttemple";
    extern char AreaFixUpName_Emperorfight[16] = "emperorfight";
    extern char AreaFixUpName_Moseisley[12] = "moseisley";
    extern char AreaFixUpName_Tatooine[12] = "tatooine";
    extern char AreaFixUpName_Gunship[8] = "gunship";
    extern char AreaFixUpName_Pursuit[8] = "pursuit";
    extern char AreaFixUpName_Podsprint[12] = "podsprint";
    extern char AreaFixUpName_Negotiations[16] = "negotiations";
    extern char AreaFixUpName_Fairground[12] = "Fairground";
    extern char AreaFixUpName_Rooftops[12] = "Rooftops";
    extern char AreaFixUpName_Gothamstreets[16] = "GothamStreets";
    extern char AreaFixUpName_Batcave[8] = "batcave";
    extern char AreaFixUpName_Jimtest[8] = "JimTest";
    extern char AreaFixUpName_NbKashyyyk[12] = "nb_kashyyyk";
    extern char AreaFixUpName_NbKamino[12] = "nb_kamino";
    extern char AreaFixUpName_NbDagobah[12] = "nb_dagobah";
    extern char AreaFixUpName_Hoth[8] = "hoth";
    extern char AreaFixUpName_Utapau[8] = "utapau";
    extern char AreaFixUpName_LostTemple[12] = "LostTemple";
    extern char AreaFixUpName_Bonus[8] = "Bonus";
    extern char AreaFixUpName_Bonus2[8] = "Bonus2";
    extern char AreaFixUpName_Anewhope[12] = "ANewHope";
    extern char AreaFixUpName_BonusGunship[16] = "Bonus_Gunship";
    extern char AreaFixUpName_Anakinsflight[16] = "AnakinsFlight";
    extern char AreaFixUpName_Podrace[8] = "PodRace";
    extern char AreaFixUpName_E6characterbonus[20] = "E6CharacterBonus";
    extern char AreaFixUpName_E5characterbonus[20] = "E5CharacterBonus";
    extern char AreaFixUpName_E4characterbonus[20] = "E4CharacterBonus";
    extern char AreaFixUpName_E3characterbonus[20] = "E3CharacterBonus";
    extern char AreaFixUpName_E2characterbonus[20] = "E2CharacterBonus";
    extern char AreaFixUpName_E1characterbonus[20] = "E1CharacterBonus";
    extern char AreaFixUpName_Deathstar2battle[20] = "DeathStar2Battle";
    extern char AreaFixUpName_Endorbattle[12] = "EndorBattle";
    extern char AreaFixUpName_Speederchase[16] = "SpeederChase";
    extern char AreaFixUpName_Sarlaccpit[12] = "SarlaccPit";
    extern char AreaFixUpName_Jabbaspalace[16] = "JabbasPalace";
    extern char AreaFixUpName_Cloudcityescape[16] = "CloudCityEscape";
    extern char AreaFixUpName_Cloudcitytrap[16] = "CloudCityTrap";
    extern char AreaFixUpName_Dagobah[8] = "Dagobah";
    extern char AreaFixUpName_Asteroidchase[16] = "AsteroidChase";
    extern char AreaFixUpName_Hothescape[12] = "HothEscape";
    extern char AreaFixUpName_Hothbattle[12] = "HothBattle";
    extern char AreaFixUpName_Deathstarbattle[16] = "DeathStarBattle";
    extern char AreaFixUpName_Deathstarescape[16] = "DeathStarEscape";
    extern char AreaFixUpName_Deathstarrescue[16] = "DeathStarRescue";
    extern char AreaFixUpName_Blockaderunner[16] = "BlockadeRunner";
    extern char AreaFixUpName_Vader[8] = "Vader";
    extern char AreaFixUpName_Temple[8] = "Temple";
    extern char AreaFixUpName_Kashyyyk[12] = "Kashyyyk";
    extern char AreaFixUpName_Grievous[12] = "Grievous";
    extern char AreaFixUpName_Cruiser[8] = "Cruiser";
    extern char AreaFixUpName_Dogfight[12] = "Dogfight";
    extern char AreaFixUpName_Dooku[8] = "Dooku";
    extern char AreaFixUpName_Factory[8] = "Factory";
    extern char AreaFixUpName_Kamino[8] = "Kamino";
    extern char AreaFixUpName_Maul[8] = "Maul";
    extern char AreaFixUpName_Retakepalace[16] = "RetakePalace";
    extern char AreaFixUpName_Palacerescue[16] = "PalaceRescue";
    extern char AreaFixUpName_Map[4] = "map";

    AREA_DATA *ROOFTOPS_ADATA = 0;
    AREA_DATA *FAIRGROUND_ADATA = 0;
    AREA_DATA *GUNGAN_ADATA = 0;
    AREA_DATA *RESCUE_ADATA = 0;
    AREA_DATA *RETAKE_ADATA = 0;
    AREA_DATA *MAUL_ADATA = 0;
    AREA_DATA *FACTORY_ADATA = 0;
    AREA_DATA *CRUISER_ADATA = 0;
    AREA_DATA *GRIEVOUS_ADATA = 0;
    AREA_DATA *KASHYYYK_ADATA = 0;
    AREA_DATA *TEMPLE_ADATA = 0;
    AREA_DATA *BLOCKADERUNNER_ADATA = 0;
    AREA_DATA *TATOOINE_ADATA = 0;
    AREA_DATA *MOSEISLEY_ADATA = 0;
    AREA_DATA *DEATHSTARRESCUE_ADATA = 0;
    AREA_DATA *DEATHSTARBATTLE_ADATA = 0;
    AREA_DATA *HOTHBATTLE_ADATA = 0;
    AREA_DATA *CLOUDCITYTRAP_ADATA = 0;
    AREA_DATA *CLOUDCITYESCAPE_ADATA = 0;
    AREA_DATA *SARLACCPIT_ADATA = 0;
    AREA_DATA *ENDORBATTLE_ADATA = 0;
    AREA_DATA *EMPERORFIGHT_ADATA = 0;
    AREA_DATA *E4CHARACTER_ADATA = 0;
    AREA_DATA *E5CHARACTER_ADATA = 0;
    AREA_DATA *E6CHARACTER_ADATA = 0;
    AREA_DATA *E1VEHICLE_ADATA = 0;
    AREA_DATA *E2VEHICLE_ADATA = 0;
    AREA_DATA *E3VEHICLE_ADATA = 0;
    AREA_DATA *E4VEHICLE_ADATA = 0;
    AREA_DATA *E5VEHICLE_ADATA = 0;
    AREA_DATA *E6VEHICLE_ADATA = 0;
    AREA_DATA *PODRACE_ADATA = 0;
    AREA_DATA *ANAKINSFLIGHT_ADATA = 0;
    AREA_DATA *BONUS_GUNSHIP_ADATA = 0;
    AREA_DATA *ANEWHOPE_ADATA = 0;
    AREA_DATA *LEGOCITY_ADATA = 0;

    AREAFIXUP AreaFixUpLSW[65] = {
        {AreaFixUpName_Map,              &HUB_ADATA                },
        {AreaFixUpName_Negotiations,     &NEGOTIATIONS_ADATA       },
        {AreaFixUpName_Gungan,           &GUNGAN_ADATA             },
        {AreaFixUpName_Palacerescue,     &RESCUE_ADATA             },
        {AreaFixUpName_Podsprint,        &PODSPRINT_ADATA          },
        {AreaFixUpName_Retakepalace,     &RETAKE_ADATA             },
        {AreaFixUpName_Maul,             &MAUL_ADATA               },
        {AreaFixUpName_Pursuit,          &BOUNTYHUNTERPURSUIT_ADATA},
        {AreaFixUpName_Kamino,           &KAMINO_ADATA             },
        {AreaFixUpName_Factory,          &FACTORY_ADATA            },
        {AreaFixUpName_Jedi,             &JEDI_ADATA               },
        {AreaFixUpName_Gunship,          &GUNSHIP_ADATA            },
        {AreaFixUpName_Dooku,            &DOOKU_ADATA              },
        {AreaFixUpName_Dogfight,         &DOGFIGHT_ADATA           },
        {AreaFixUpName_Dogfight,         &BATTLEOVERCORUSCANT_ADATA},
        {AreaFixUpName_Cruiser,          &CRUISER_ADATA            },
        {AreaFixUpName_Grievous,         &GRIEVOUS_ADATA           },
        {AreaFixUpName_Kashyyyk,         &KASHYYYK_ADATA           },
        {AreaFixUpName_Temple,           &TEMPLE_ADATA             },
        {AreaFixUpName_Vader,            &VADER_ADATA              },
        {AreaFixUpName_Blockaderunner,   &BLOCKADERUNNER_ADATA     },
        {AreaFixUpName_Tatooine,         &TATOOINE_ADATA           },
        {AreaFixUpName_Moseisley,        &MOSEISLEY_ADATA          },
        {AreaFixUpName_Deathstarrescue,  &DEATHSTARRESCUE_ADATA    },
        {AreaFixUpName_Deathstarescape,  &DEATHSTARESCAPE_ADATA    },
        {AreaFixUpName_Deathstarbattle,  &DEATHSTARBATTLE_ADATA    },
        {AreaFixUpName_Hothbattle,       &HOTHBATTLE_ADATA         },
        {AreaFixUpName_Hothescape,       &HOTHESCAPE_ADATA         },
        {AreaFixUpName_Asteroidchase,    &ASTEROIDCHASE_ADATA      },
        {AreaFixUpName_Dagobah,          &DAGOBAH_ADATA            },
        {AreaFixUpName_Cloudcitytrap,    &CLOUDCITYTRAP_ADATA      },
        {AreaFixUpName_Cloudcityescape,  &CLOUDCITYESCAPE_ADATA    },
        {AreaFixUpName_Jabbaspalace,     &JABBASPALACE_ADATA       },
        {AreaFixUpName_Sarlaccpit,       &SARLACCPIT_ADATA         },
        {AreaFixUpName_Speederchase,     &SPEEDERCHASE_ADATA       },
        {AreaFixUpName_Endorbattle,      &ENDORBATTLE_ADATA        },
        {AreaFixUpName_Deathstar2battle, &DEATHSTARBATTLE2_ADATA   },
        {AreaFixUpName_Emperorfight,     &EMPERORFIGHT_ADATA       },
        {AreaFixUpName_E1characterbonus, &E1CHARACTER_ADATA        },
        {AreaFixUpName_E2characterbonus, &E2CHARACTER_ADATA        },
        {AreaFixUpName_E3characterbonus, &E3CHARACTER_ADATA        },
        {AreaFixUpName_E4characterbonus, &E4CHARACTER_ADATA        },
        {AreaFixUpName_E5characterbonus, &E5CHARACTER_ADATA        },
        {AreaFixUpName_E6characterbonus, &E6CHARACTER_ADATA        },
        {AreaFixUpName_E1characterbonus, &E1VEHICLE_ADATA          },
        {AreaFixUpName_E2characterbonus, &E2VEHICLE_ADATA          },
        {AreaFixUpName_E3characterbonus, &E3VEHICLE_ADATA          },
        {AreaFixUpName_E4characterbonus, &E4VEHICLE_ADATA          },
        {AreaFixUpName_E5characterbonus, &E5VEHICLE_ADATA          },
        {AreaFixUpName_E6characterbonus, &E6VEHICLE_ADATA          },
        {AreaFixUpName_Podrace,          &PODRACE_ADATA            },
        {AreaFixUpName_Anakinsflight,    &ANAKINSFLIGHT_ADATA      },
        {AreaFixUpName_BonusGunship,     &BONUS_GUNSHIP_ADATA      },
        {AreaFixUpName_Anewhope,         &ANEWHOPE_ADATA           },
        {AreaFixUpName_Bonus2,           &LEGOCITY_ADATA           },
        {AreaFixUpName_Bonus,            &NEWTOWN_ADATA            },
        {AreaFixUpName_LostTemple,       &LOSTTEMPLE_ADATA         },
        {AreaFixUpName_Senate,           &SENATE_ADATA             },
        {AreaFixUpName_Utapau,           &UTAPAU_ADATA             },
        {AreaFixUpName_Hoth,             &HOTH_ADATA               },
        {AreaFixUpName_NbDagobah,        &BONUSDAGOBAH_ADATA       },
        {AreaFixUpName_NbKamino,         &BONUSKAMINO_ADATA        },
        {AreaFixUpName_NbKashyyyk,       &BONUSKASHYYYK_ADATA      },
        {AreaFixUpName_Jimtest,          &JIMTEST_ADATA            },
        {NULL,                           NULL                      },
    };

    AREAFIXUP AreaFixUpLostTemple[2] = {
        {AreaFixUpName_Losttemple, &LOSTTEMPLE_ADATA},
        {NULL,                     NULL             },
    };

    AREAFIXUP AreaFixUpGotham[5] = {
        {AreaFixUpName_Batcave,       &HUB_ADATA          },
        {AreaFixUpName_Gothamstreets, &GOTHAMSTREETS_ADATA},
        {AreaFixUpName_Rooftops,      &ROOFTOPS_ADATA     },
        {AreaFixUpName_Fairground,    &FAIRGROUND_ADATA   },
        {NULL,                        NULL                },
    };
    extern const u32 NuTimeBar_EngineColours[7] = {0x7f00007f, 0x7f007f00, 0x7f007f50, 0x7f507f50, 0x7f000050, 0x7f40407f, 0x7f7f0000};
    extern const u32 NuTimeBar_DefaultColours[16] = {
        0x00000040, 0x22222240, 0x00004440, 0x00008840, 0x44000040, 0x88000040, 0x44004440, 0x88008840,
        0x00440040, 0x00880040, 0x00444440, 0x00888840, 0x44440040, 0x88880040, 0x44444440, 0x88888840,
    };
    extern const f32 NuTimeBarQFontSize = 16.0f;
    extern const f32 NuTimeBarQFontScale = 0.0625f;
    extern const f64 NuTimeBarColumnStep = 3.0;
    extern const f64 NuTimeBarSlotWidth = 0.02500000037252903;
    extern const f32 NuTimeBarFontScale = 0.85f;
    extern const f64 NuTimeBarNegativeSlotWidth = -0.02500000037252903;
    extern const f64 NuTimeBarLinesPerSecond = 15734.0;

    i16 PlayerList[8] = {0};
    i32 netclient = 0;
    extern const char GizAIMessage_NameFormat[3] = "%s";
    LANGUAGEDATA Game_LanguageList[7] = {
        {1,  apitxt_ENGLISH},
        {2,  apitxt_FRENCH },
        {4,  apitxt_GERMAN },
        {5,  apitxt_ITALIAN},
        {3,  apitxt_SPANISH},
        {8,  apitxt_DANISH },
        {-1, NULL          }
    };
    LANGUAGEDATA Text_LanguageList_Default[7] = {
        {1,  apitxt_ENGLISH},
        {2,  apitxt_FRENCH },
        {4,  apitxt_GERMAN },
        {5,  apitxt_ITALIAN},
        {3,  apitxt_SPANISH},
        {8,  apitxt_DANISH },
        {-1, NULL          }
    };

    i64 NuTime_TickBase = 0;
    i64 NuTime_PerformanceFrequency = 0;
    extern const f64 NuTime_LowScale = 4294967295.0;
    extern const f64 NuTime_MicroScale = 1000000.0;
    extern const f64 NuTime_RefreshRate = 60.0;
    TERRAIN_SURFACE TerSurface[32] = {
        {1.0f, 0,      0xffff},
        {1.0f, 65,     0xffff},
        {1.0f, 2,      0xffff},
        {1.0f, 8,      0xffff},
        {1.0f, 65536,  0xffff},
        {1.0f, 66560,  0xffff},
        {1.0f, 0,      0xffff},
        {1.0f, 2048,   0xffff},
        {1.0f, 0,      0xffff},
        {1.0f, 8192,   0xffff},
        {1.0f, 10,     0xffff},
        {1.0f, 67,     0xffff},
        {1.0f, 16,     0xffff},
        {1.0f, 16,     0xffff},
        {1.0f, 8192,   0xffff},
        {1.0f, 8192,   0xffff},
        {0.1f, 1088,   0xffff},
        {0.2f, 131074, 0xffff},
        {1.0f, 81,     0xffff},
        {1.0f, 512,    0xffff},
        {1.0f, 8192,   0xffff},
        {1.0f, 514,    0xffff},
        {1.0f, 64,     0xffff},
        {1.0f, 192,    0xffff},
        {1.0f, 8192,   0xffff},
        {1.0f, 4160,   0xffff},
        {1.0f, 0,      0xffff},
        {1.0f, 5184,   0xffff},
        {1.0f, 256,    0xffff},
        {1.0f, 320,    0xffff},
        {1.0f, 64,     0xffff},
        {1.0f, 64,     0xffff},
    };

    extern const f32 LevelConfigFarDistance = 2000000.0f;

    i32 edanim_page_on[8] = {0};
    i32 edanim_page_used[8] = {0};
    NUGSCN *edanim_page_scene[8] = {0};
    i32 edanim_next_param = 0;
    i32 edanim_params_used = 0;
    EDANIM_PARAM AnimParams[64] = {0};

    void MenuEnterNoMemoryCard(MENU *);
    void MenuDrawNoData(MENU *);
    void MenuDrawLoading(MENU *);
    void MenuUpdateFileCorrupt(MENU *);

    void MenuDrawDoNotRemoveCard(MENU *);
    void MenuUpdateDoNotRemoveCard(MENU *);
    void MenuDrawFormatting(MENU *);
    void MenuDrawCardWarning(MENU *);
    void MenuUpdateCardWarning(MENU *);
    void fun_00509470(MENU *);
    void fun_00509480(MENU *);
    void fun_00509490(MENU *);
    void fun_005094a0(MENU *);

    MENUFNINFO MenuInfo[100] = {
        {1000, MenuEnterSave,            MenuDrawSave,            MenuUpdateSave,            MenuExitSave,         -1, -1, 0, 1, 1, 0},
        {1001, NULL,                     MenuDrawDoNotRemoveCard, MenuUpdateDoNotRemoveCard, NULL,                 -1, -1, 0, 0, 0, 0},
        {1002, MenuEnterInsertCard,      MenuDrawInsertCard,      MenuUpdateInsertCard,      NULL,                 -1, -1, 0, 1, 1, 0},
        {1004, MenuEnterNoMemoryCard,    MenuDrawNoMemoryCard,    MenuUpdateNoMemoryCard,    NULL,                 -1, -1, 0, 1, 1, 0},
        {1003, NULL,                     MenuDrawNotEnoughSpace,  MenuUpdateNotEnoughSpace,  NULL,                 -1, -1, 0, 1, 1, 0},
        {1005, NULL,                     MenuDrawNoData,          MenuUpdateNoData,          NULL,                 -1, -1, 0, 1, 1, 0},
        {1006, NULL,                     MenuDrawFormatConfirm,   MenuUpdateFormatConfirm,   NULL,                 -1, -1, 0, 1, 1, 0},
        {1007, NULL,                     MenuDrawFormatting,      MenuUpdateFormatting,      NULL,                 -1, -1, 0, 0, 0, 0},
        {1008, MenuEnterSaveConfirm,     MenuDrawSaveConfirm,     MenuUpdateSaveConfirm,     NULL,                 -1, -1, 0, 1, 1, 0},
        {1009, NULL,                     MenuDrawSaving,          MenuUpdateSaving,          NULL,                 -1, -1, 0, 0, 0, 0},
        {1010, MenuEnterAutoSaveWarning, MenuDrawAutoSaveWarning, MenuUpdateAutoSaveWarning, NULL,                 -1, -1, 0, 1, 0, 0},
        {1011, MenuEnterAutoSaveCancel,  MenuDrawAutoSaveCancel,  MenuUpdateAutoSaveCancel,  NULL,                 -1, -1, 0, 1, 1, 0},
        {1012, MenuEnterLoad,            MenuDrawLoad,            MenuUpdateLoad,            MenuExitLoad,         -1, -1, 0, 1, 1, 0},
        {1013, NULL,                     MenuDrawLoadConfirm,     MenuUpdateLoadConfirm,     NULL,                 -1, -1, 0, 1, 1, 0},
        {1014, NULL,                     MenuDrawLoading,         MenuUpdateLoading,         NULL,                 -1, -1, 0, 0, 0, 0},
        {1015, NULL,                     MenuDrawDeleting,        MenuUpdateDeleting,        NULL,                 -1, -1, 0, 0, 0, 0},
        {1016, NULL,                     MenuDrawSaveCancel,      MenuUpdateSaveCancel,      NULL,                 -1, -1, 0, 1, 1, 0},
        {1017, NULL,                     MenuDrawLoadCancel,      MenuUpdateLoadCancel,      NULL,                 -1, -1, 0, 1, 1, 0},
        {1018, NULL,                     MenuDrawFormatCancel,    MenuUpdateFormatCancel,    NULL,                 -1, -1, 0, 1, 1, 0},
        {1019, MenuEnterCardWarning,     MenuDrawCardWarning,     MenuUpdateCardWarning,     MenuExitCardWarning,  -1, -1, 0, 1, 0, 0},
        {1020, NULL,                     MenuDrawFileCorrupt,     MenuUpdateFileCorrupt,     NULL,                 -1, -1, 0, 1, 1, 0},
        {1021, MenuEnterHeaderSave,      NULL,                    NULL,                      NULL,                 -1, -1, 0, 0, 0, 0},
        {1022, MenuEnterStartNewGame,    NULL,                    NULL,                      NULL,                 -1, -1, 0, 0, 0, 0},
        {1023, NULL,                     MenuDrawDeleteConfirm,   MenuUpdateDeleteConfirm,   NULL,                 -1, -1, 0, 1, 1, 0},
        {1024, fun_00509470,             fun_00509480,            fun_00509490,              fun_005094a0,         -1, -1, 0, 0, 0, 0},
        {1025, MenuEnterVideo,           MenuDrawVideo,           MenuUpdateVideo,           MenuExitVideo,        -1, -1, 0, 0, 0, 0},
        {1026, MenuEnterEffects,         MenuDrawEffects,         MenuUpdateEffects,         MenuExitEffects,      -1, -1, 0, 0, 0, 0},
        {1027, MenuEnterVideoConfirm,    MenuDrawVideoConfirm,    MenuUpdateVideoConfirm,    MenuExitVideoConfirm, -1, -1, 0, 0, 0, 0},
        {1028, MenuEnterHiresNotice,     MenuDrawHiresNotice,     MenuUpdateHiresNotice,     MenuExitHiresNotice,  -1, -1, 0, 0, 0, 0},
        {1029, MenuEnterQuit,            MenuDrawQuit,            MenuUpdateQuit,            MenuExitQuit,         -1, -1, 0, 0, 0, 0},
        {1030, NULL,                     NULL,                    NULL,                      NULL,                 -1, -1, 0, 0, 0, 0},
    };
    i32 MenuSFX = -1;
    extern const f64 nu_wind_phase_scale = 1.0 / 256.0;
    extern const f32 PCKeyboardDefaultValue = 0.1f;
    f32 dme_sy = 1.0f;
    AREA_DATA *GUNSHIP_ADATA = 0;
    OPTIONSSAVE *Game_OptionsSave = 0;

    // The 23-record PC table: IDs, callbacks, remembered cursors, and prompt flags.
    void MenuUpdatePauseRestart(MENU *);
    void MenuDrawEndMission(MENU *);
    void MenuUpdateEndMission(MENU *);
    void MenuDrawEndChallenge(MENU *);
    void MenuUpdateEndChallenge(MENU *);
    void Arcade_DrawEndMenu(MENU *);
    void Arcade_UpdateEndMenu(MENU *);
    void MenuDrawMiniKit(MENU *);
    void MenuUpdateMiniKit(MENU *);
    void Credits_UpdateMenu(MENU *);
    void MenuDrawBonusWin(MENU *);
    void MenuUpdateBonusWin(MENU *);
    void MenuDrawBonusComplete(MENU *);
    void MenuUpdateBonusComplete(MENU *);
    MENUFNINFO GameMenuInfo[23] = {
        {0,  MenuEnterTitles,         MenuDrawTitles,         MenuUpdateTitles,         NULL,                   -1, -1, 0, 0, 0, 0},
        {1,  MenuEnterNewGame,        MenuDrawNewGame,        MenuUpdateNewGame,        NULL,                   -1, -1, 0, 1, 0, 0},
        {2,  NULL,                    MenuDrawPauseMain,      MenuUpdatePauseMain,      NULL,                   -1, -1, 0, 0, 0, 0},
        {3,  NULL,                    MenuDrawPauseRestart,   MenuUpdatePauseRestart,   NULL,                   -1, -1, 0, 0, 0, 0},
        {4,  MenuEnterOptions,        MenuDrawOptions,        MenuUpdateOptions,        MenuExitOptions,        0,  0,  0, 1, 1, 0},
        {5,  NULL,                    MenuDrawExtras,         MenuUpdateExtras,         NULL,                   0,  0,  0, 0, 0, 0},
        {6,  NULL,                    MenuDrawPauseExit,      MenuUpdatePauseExit,      NULL,                   -1, -1, 0, 0, 0, 0},
        {7,  NULL,                    MenuDrawSelectLanguage, MenuUpdateSelectLanguage, NULL,                   0,  0,  0, 0, 0, 0},
        {8,  MenuInitMissions,        MenuDrawMissions,       MenuUpdateMissions,       NULL,                   -1, -1, 0, 0, 0, 0},
        {9,  NULL,                    MenuDrawEndMission,     MenuUpdateEndMission,     NULL,                   -1, -1, 0, 0, 0, 0},
        {10, NULL,                    MenuDrawEndChallenge,   MenuUpdateEndChallenge,   NULL,                   -1, -1, 0, 0, 0, 0},
        {11, NULL,                    Arcade_DrawEndMenu,     Arcade_UpdateEndMenu,     NULL,                   -1, -1, 0, 0, 0, 0},
        {12, NULL,                    MenuDrawCustomiser,     MenuUpdateCustomiser,     NULL,                   -1, -1, 0, 0, 0, 0},
        {13, NULL,                    MenuDrawShop,           MenuUpdateShop,           NULL,                   -1, -1, 0, 0, 0, 0},
        {14, NULL,                    MenuDrawMiniKit,        MenuUpdateMiniKit,        NULL,                   -1, -1, 0, 0, 0, 0},
        {15, MenuInitSelectMode,      MenuDrawSelectMode,     MenuUpdateSelectMode,     NULL,                   -1, -1, 0, 0, 0, 0},
        {16, MenuInitBonusMode,       MenuDrawBonusMode,      MenuUpdateBonusMode,      NULL,                   -1, -1, 0, 0, 0, 0},
        {17, MenuInitFreePlay,        MenuDrawFreePlay,       MenuUpdateFreePlay,       NULL,                   -1, -1, 0, 0, 0, 0},
        {18, NULL,                    NULL,                   Credits_UpdateMenu,       NULL,                   -1, -1, 0, 0, 0, 0},
        {19, NULL,                    MenuDrawBonusWin,       MenuUpdateBonusWin,       NULL,                   -1, -1, 0, 0, 0, 0},
        {20, NULL,                    MenuDrawBonusComplete,  MenuUpdateBonusComplete,  NULL,                   -1, -1, 0, 0, 0, 0},
        {21, MenuEnterControlSetup,   MenuDrawControlSetup,   MenuUpdateControlSetup,   NULL,                   -1, -1, 0, 0, 0, 0},
        {22, MenuEnterSelectControls, MenuDrawControlSetup,   MenuUpdateSelectControls, MenuExitSelectControls, 0,  0,  0, 0, 0, 0},
    };

    i32 PS2_VCNTR_X = 320;
    i32 PS2_VCNTR_Y = 240;
}
NUAPI nuapi = {0};

u32 debrisseed = 0x5c0999;

i32 argc = 0;
char **argv = 0;
NUDATHDR *curr_dat = 0;
MSG win_message = {0};

WORLDINFO *WORLD = &WorldInfo[0];

// PC reads pairs of 16-bit code points, ending with a zero pair.
u16 bad_equiv[8][2] = {
    {'a', '@'},
    {'a', '4'},
    {'e', '3'},
    {'i', '1'},
    {'l', '1'},
    {'o', '0'},
    {'t', '7'},
    {0,   0  }
};

// PC checks each byte index is less than 16.
const u8 CurveGroupMasks[4] = {2, 1, 8, 0};
const u8 KeyStructSizes[16] = {3, 4, 4, 3, 4, 3, 4, 8, 4, 8, 4, 0, 0, 0, 0, 0};

// 513 angular samples including both endpoints.

// Movement, object and AI state recovered from the PC image.
extern "C" {
    i32 HIGHGAMEOBJECT = 0;
    GAMEOBJECT *Obj = 0;
    NUVEC v000 = {0};
}

// Additional PC movement, targeting, arcade and callback state.
extern "C" {
    f32 TargetDist_Near2 = 0;

    extern const f32 ForcePushApproachInnerSquare = 0.010000001f;
    extern const f32 ForcePushApproachOuterSquare = 0.040000003f;
    i16 PCArcadeModeText0 = 0;
    i16 PCArcadeModeText1 = 0;
    i16 PCArcadeModeText2 = 0;
    ARCADE_MODE Arcade_Mode[7] = {
        {&PCArcadeModeText0, 0,      0x19},
        {&PCArcadeModeText1, 10,     0x12},
        {&PCArcadeModeText1, 25,     0x12},
        {&PCArcadeModeText1, 50,     0x12},
        {&PCArcadeModeText2, 50000,  4   },
        {&PCArcadeModeText2, 100000, 4   },
        {&PCArcadeModeText2, 250000, 4   },
    };

}

// PC movement tuning, timers, input masks and parser path.
extern "C" {
    TIMER GamePlayTimer = {0};
    u32 GAMEPAD_JUMP = PAD_BUTTON_CROSS;
    u32 GAMEPAD_ACTION = PAD_BUTTON_SQUARE;
    u32 GAMEPAD_SPECIAL = PAD_BUTTON_CIRCLE;
    u32 GAMEPAD_TAG = PAD_BUTTON_TRIANGLE;
    extern const f64 ApplyGravityVelocityFactor = 0.3330000042915344;
    extern const f64 ApplyGravityFrameScale = 180.0;
    extern const f32 ApplyGravityMinimumFrameTime = 0.016666668f;
}

extern "C" {
    i16 id_GEONOSIAN = -1;
    i16 id_CHEWBACCA = -1;
}

extern "C" {

    i32 part_scene_pageid[32] = {0};
}

extern "C" {

    GIZOBSTACLE *LevGizObst[8] = {0};
}

extern "C" {
    GAMEOBJECT *CarWashHack = 0;
    i16 id_GRIEVOUS = -1;
}

extern "C" {
    APIDEBRISSYS *perm_debrissys = 0;
}

extern "C" {
    i16 id_TIEFIGHTER = -1;
    i16 id_XWING = -1;
}

extern "C" {
    CHECK_LOST_DATA CheckLostDataFn = 0;
// Includes the utility WORD at +0x10000.
    CONFIG_BUFFER ConfigBuffer = {0};
}

extern "C" {

    f32 player_tag_timer = 0;
    i32 do_player_tag = 0;
}

extern "C" {
    i16 id_LUKESKYWALKERDAGOBAH = -1;
    i16 id_ATAT = -1;
}

extern "C" {
    u8 aicreature_sets_alive[16] = {0};
}

extern "C" {
    i32 nethost = 0;
}

extern "C" {
    u16 PlayerPadCurrentAngle[2] = {0};
}

extern "C" {}

extern "C" {
    i32 GAMEPAD_LIFT = PAD_BUTTON_R1;
}

extern "C" {
    f32 DEFAULT_MOVE_RANGE = 0;
}

extern "C" {
    i8 adtabentries[9][4] = {
        {-1, -1, -1, -1},
        {-1, -1, -1, 0 },
        {-1, -1, 0,  0 },
        {-1, 0,  0,  0 },
        {0,  0,  0,  0 },
        {1,  0,  0,  0 },
        {1,  1,  0,  0 },
        {1,  1,  1,  0 },
        {1,  1,  1,  1 }
    };
    ADAPTIVEDIFFICULTY adaptivedifficulty = {0};
    i16 id_YWING = -1;
    i16 id_TIEBOMBER = -1;
    u32 LEGO_AIPATHCNX_FORGOODIES = 0;
    u32 LEGO_AIPATHCNX_FORBADDIES = 0;
    NUVEC ShadNorm = {0};
    PC_SURFACE_MASK GetSurfaceInfoDefaultMask = {0x20};
}

extern "C" {
// Minimum access view for the reviewed 0x948 and nested 0xd0 requests,
// including their two pointer trailers. The full original extent is unknown.
}

extern "C" {
    extern const f64 TerrainSkinSampleCount = 8.0;
    extern const f64 TerrainSkinSampleIndexScale = -8.0;
    extern const f32 TerrainSkinBoundsMax = 123456792.0f;
    extern const f32 TerrainSkinBoundsMin = -123456792.0f;
}

struct TERRAIN_SKIN_INFO;
struct TERRAIN_SKIN_USE_RECORD;

extern "C" {
// Accessed terrain state and literals; full original allocation extents remain unknown.
    struct TERRAIN_SKIN_USE_RECORD *SkinMemInfo = 0;
    extern const f32 TerrainCastLowerSentinel = -2000000.0f;
}

extern "C" {
    extern const f64 ActionSnapWeightedRandomZero = 0.0;
}

extern "C" {

    extern const f64 NuVpFixed16Scale = 1.0 / 65536.0;
}

extern "C" {
    extern const char dat_00757a7c[8] = "looping";
}

extern "C" {
    PC_DEVICE_STATE_CACHE PCDeviceStateCachePc;
    i32 nufile_lsn_allowed = 1;
}

extern "C" {
    PC_FILEEXTINFO extensions[64] = {0};
    i32 num_extensions = 0;
}

extern "C" {}

extern "C" {
    TIMER AreaTimer = {0};
}

extern "C" {
    extern const char GizmoSnapOptionName[5] = "SNAP";
}

extern "C" {
    extern const char dat_00757824[7] = "random";
    extern const char GizObstacleActionNamePC[5] = "Name";
    extern const char GizObstacleActionBackwardPC[9] = "BACKWARD";
    extern const char GizObstacleActionForwardPC[8] = "FORWARD";
    extern const char GizObstacleActionStayOpenPC[9] = "STAYOPEN";
    extern const char GizObstacleActionStayShutPC[9] = "STAYSHUT";

}

extern "C" {
    i32 edgra_page_used[8] = {0};
    NUGSCN *edgra_page_scene[8] = {0};
    NUMTX *edgra_page_matrix_stack[8] = {0};
    i32 edgra_clumps_used = 0;
    void edgraInit(void);
    void edgraClose(void);
    void edgraEnter(void);
    i32 edgraProc(f32 delta_time, FRAME_PAD *pad);
    void edgraRender(void);
    ed_module_s edgradesc = {NULL, NULL, "Grass Editor", edgraInit,  edgraClose, edgraEnter,  NULL,
                             NULL, NULL, NULL,           0x73617267, edgraProc,  edgraRender, NULL};
}

extern "C" {
    i32 edgra_units_used = 0;
    i32 freechunkcontrolsptr = 0;
}

extern "C" {
    extern const f32 AIPositionUnset = 1000000000.0f;
}

extern "C" {
    f32 debris_thinning_level = 1.0f;
    u8 PCFrameLoadingYieldEnabledPC = 0;
}

extern "C" {
    PC_SHADER_SLOT_VIEW *PCShaderSlots = 0;
}

extern "C" {
    i32 edbits_anim_page = 0;
    i32 edanim_particle_mode = 0;
    i32 edanim_nearest_param_id = 0;
}

extern "C" {
    PC_NATIVE_DEBUG_STATE PCNativeWarningDebugState;
    void (*PCWarningHandler)(char *) = 0;
}

extern "C" {
    void *ScaleTerrainT2 = 0;
    void *TempScanStack = 0;
    i32 curSphereter = 0;
    i32 PlatImpactId = 0;
    extern const f32 TerrainUnusedSpatialBlock = 2147483648.0f;
    extern const f32 TerrainBoundsMinimum = -200000000.0f;
    extern const f32 TerrainBoundsMaximum = 200000000.0f;
}

extern "C" {
    extern const f64 PlayerSlideSpeed = 1.2000000476837158;
    extern const f64 PlayerRandomScale = 1.5259021893143654e-05;
    extern const f64 PlayerStepInnerAction = 1.75;
    extern const f64 PlayerStepOuterAction = 2.25;
    extern const f32 PlayerBuildItNegativeLimit = -1000000000.0f;
}

extern "C" {
    i32 curPickInst = 0;
    extern const f64 TerrainScanMargin10 = 0.10000000149011612;
    extern const NUVEC debris_box_corners[8] = {
        {-0.2f, -0.2f, -0.05f},
        {0.2f,  -0.2f, -0.05f},
        {-0.2f, 0.2f,  -0.05f},
        {0.2f,  0.2f,  -0.05f},
        {-0.5f, -0.5f, 0.8f  },
        {0.5f,  -0.5f, 0.8f  },
        {-0.5f, 0.5f,  0.8f  },
        {0.5f,  0.5f,  0.8f  }
    };
}

CRC16 CRC16InstancePC;

extern "C" {
    void *GHGTextureProgramsPC = 0;
    extern const char GHGJointFormatWarningPC[95] =
        "Your Character version is %d, whereas the Engine only accepts Character versions from %d to %d";
    extern const char GHGSceneFormatWarningPC[87] =
        "Your Scene version is %d, whereas the Engine only accepts Scene versions from %d to %d";
    i16 id_GRABCONTROL = -1;
    i16 id_GRABR2CONTROL = -1;
    i16 id_ATST_LOWRES = -1;
    i16 id_DROIDEKA = -1;
}

extern "C" {
    extern const f64 TerrainOpposingContactThreshold = -0.7f;

    TERRAIN_TRACK_SLOT *CurTrackInfo = 0;
    i32 TerImpactDataMax = 0;
    i32 *TerImpactDataCount = 0;
    void *TerImpactData = 0;
}

extern "C" {
    void edanimInit(void);
    void edanimClose(void);
    void edanimEnter(void);
    i32 edanimProc(f32 delta_time, FRAME_PAD *pad);
    void edanimRender(void);
    ed_module_s edanimdesc = {NULL, NULL, "Anim Editor", edanimInit, edanimClose, edanimEnter,  NULL,
                              NULL, NULL, NULL,          0x6d696e61, edanimProc,  edanimRender, NULL};
    i32 edbits_editor_state = 0;
}

extern "C" {
    char ScorePanelName[9] = "MK-Panel";
    ADD_GAME_MESSAGE AddGameMsg_Default = {NULL, &v000, &v000, 1.0f, 1.0f, 255,  255,  255,  128,  0,  1.0f, 0.0f, 0, -1,
                                              NULL, 0,     0.0f,  0.0f, NULL, NULL, NULL, NULL, NULL, -1, 0,    255,  0};
    u32 BonusCoinTarget = 0;
    i32 BonusWinFlag = 0;
    f32 DoubleScoreTime = 0;
    i32 LookAtBoth = 0;
    f32 AddCoinDelay[2] = {0};
}

extern "C" {
    char dat_00758c00[9] = "Power Up";
    char dat_00758c0c[8] = "Torpedo";
}

extern "C" {
    f32 coinimpactwait = 0;
}
extern "C" {}
extern "C" {
    extern const char PickupCoinSfx[11] = "PickupCoin";
}
extern "C" {}
extern "C" {}
extern "C" {}
extern "C" {
    i16 id_BOBAFETT = -1;
}
extern "C" {
    RTL_DATA KillPartsZeroLighting = {0};
}
extern "C" {
    f32 brickimpactwait = 0;
}

extern "C" {}

extern "C" {

    i16 id_SNAKE = -1;

    i16 id_WOOKIEE = -1;

    i16 id_ROYALGUARD = -1;

}

extern "C" {

    TIMER LevelTimer = {0};
}

extern "C" {

    i16 dat_007f17d0 = -1;

    i16 dat_007f18b4 = -1;

    i16 dat_007f195c = -1;

    i16 dat_007f196c = -1;

    i16 id_JAWA = -1;

    i16 dat_007f19a4 = -1;

    i16 dat_007f19a8 = -1;

    i16 id_REPUBLICGUNSHIP = -1;

    i16 id_SENTRYDROID = -1;

    i16 dat_007f1b34 = -1;

    i16 id_ZAMSSPEEDER = -1;

    f32 DEACTIVATEDTIME = 8.0f;
}

extern "C" {
    char dat_007516c0[1] = "";

    char dat_00753370[2] = "]";

    char dat_00753374[2] = "[";

    char dat_0075ac34[5] = "Down";

    char dat_0076fde8[7] = "Delete";

    char dat_007719bc[12] = "MEDIASELECT";

    char dat_007719c8[5] = "MAIL";

    char dat_007719d0[11] = "MYCOMPUTER";

    char dat_007719dc[8] = "WEBBACK";

    char dat_007719e4[11] = "WEBFORWARD";

    char dat_007719f0[8] = "WEBSTOP";

    char dat_007719f8[11] = "WEBREFRESH";

    char dat_00771a04[13] = "WEBFAVORITES";

    char dat_00771a14[10] = "WEBSEARCH";

    char dat_00771a20[5] = "WAKE";

    char dat_00771a28[6] = "SLEEP";

    char dat_00771a30[6] = "POWER";

    char dat_00771a38[5] = "APPS";

    char dat_00771a40[7] = "R. Win";

    char dat_00771a48[7] = "L. Win";

    char dat_00771a50[7] = "Insert";

    char dat_00771a58[9] = "Pg. Down";

    char dat_00771a64[4] = "End";

    char dat_00771a68[6] = "Right";

    char dat_00771a70[5] = "Left";

    char dat_00771a78[7] = "Pg. Up";

    char dat_00771a80[3] = "Up";

    char dat_00771a84[5] = "Home";

    char dat_00771a8c[6] = "Pause";

    char dat_00771a94[7] = "R. Alt";

    char dat_00771a9c[6] = "SysRq";

    char dat_00771aa4[7] = "Num. /";

    char dat_00771aac[11] = "Num. COMMA";

    char dat_00771ab8[8] = "WEBHOME";

    char dat_00771ac0[6] = "VOLUP";

    char dat_00771ac8[8] = "VOLDOWN";

    char dat_00771ad0[10] = "MEDIASTOP";

    char dat_00771adc[5] = "PLAY";

    char dat_00771ae4[5] = "CALC";

    char dat_00771aec[5] = "MUTE";

    char dat_00771af4[8] = "R. Ctrl";

    char dat_00771afc[11] = "Num. Enter";

    char dat_00771b08[10] = "NEXTTRACK";

    char dat_00771b14[10] = "UNLABELED";

    char dat_00771b20[3] = "AX";

    char dat_00771b24[5] = "STOP";

    char dat_00771b2c[6] = "KANJI";

    char dat_00771b34[10] = "UNDERLINE";

    char dat_00771b40[6] = "COLON";

    char dat_00771b48[3] = "AT";

    char dat_00771b4c[10] = "PREVTRACK";

    char dat_00771b58[7] = "Num. =";

    char dat_00771b60[8] = "ABNT_C2";

    char dat_00771b68[4] = "YEN";

    char dat_00771b6c[10] = "NOCONVERT";

    char dat_00771b78[8] = "CONVERT";

    char dat_00771b80[8] = "ABNT_C1";

    char dat_00771b88[5] = "KANA";

    char dat_00771b90[4] = "F15";

    char dat_00771b94[4] = "F14";

    char dat_00771b98[4] = "F13";

    char dat_00771b9c[4] = "F12";

    char dat_00771ba0[4] = "F11";

    char dat_00771ba4[11] = "Num. Point";

    char dat_00771bb0[7] = "Num. 0";

    char dat_00771bb8[7] = "Num. 3";

    char dat_00771bc0[7] = "Num. 2";

    char dat_00771bc8[7] = "Num. 1";

    char dat_00771bd0[7] = "Num. +";

    char dat_00771bd8[7] = "Num. 6";

    char dat_00771be0[7] = "Num. 5";

    char dat_00771be8[7] = "Num. 4";

    char dat_00771bf0[7] = "Num. -";

    char dat_00771bf8[7] = "Num. 9";

    char dat_00771c00[7] = "Num. 8";

    char dat_00771c08[7] = "Num. 7";

    char dat_00771c10[12] = "Scroll Lock";

    char dat_00771c1c[9] = "Num Lock";

    char dat_00771c28[4] = "F10";

    char dat_00771c2c[3] = "F9";

    char dat_00771c30[3] = "F8";

    char dat_00771c34[3] = "F7";

    char dat_00771c38[3] = "F6";

    char dat_00771c3c[3] = "F5";

    char dat_00771c40[3] = "F4";

    char dat_00771c44[3] = "F3";

    char dat_00771c48[3] = "F2";

    char dat_00771c4c[3] = "F1";

    char dat_00771c50[10] = "Caps Lock";

    char dat_00771c5c[6] = "Space";

    char dat_00771c64[7] = "L. Alt";

    char dat_00771c6c[7] = "Num. *";

    char dat_00771c74[9] = "R. Shift";

    char dat_00771c80[11] = "For. Slash";

    char dat_00771c8c[5] = "Stop";

    char dat_00771c94[6] = "Comma";

    char dat_00771c9c[2] = "M";

    char dat_00771ca0[2] = "N";

    char dat_00771ca4[2] = "B";

    char dat_00771ca8[2] = "V";

    char dat_00771cac[2] = "C";

    char dat_00771cb0[2] = "#";

    char dat_00771cb4[9] = "L. Shift";

    char dat_00771cc0[6] = "Grave";

    char dat_00771cc8[11] = "Apostrophe";

    char dat_00771cd4[10] = "Semicolon";

    char dat_00771ce0[2] = "L";

    char dat_00771ce4[2] = "K";

    char dat_00771ce8[2] = "J";

    char dat_00771cec[2] = "H";

    char dat_00771cf0[2] = "G";

    char dat_00771cf4[2] = "F";

    char dat_00771cf8[2] = "D";

    char dat_00771cfc[2] = "S";

    char dat_00771d00[2] = "A";

    char dat_00771d04[8] = "L. Ctrl";

    char dat_00771d0c[6] = "Enter";

    char dat_00771d14[2] = "P";

    char dat_00771d18[2] = "O";

    char dat_00771d1c[2] = "I";

    char dat_00771d20[2] = "U";

    char dat_00771d24[2] = "T";

    char dat_00771d28[2] = "R";

    char dat_00771d2c[2] = "E";

    char dat_00771d30[2] = "W";

    char dat_00771d34[2] = "Q";

    char dat_00771d38[4] = "Tab";

    char dat_00771d3c[10] = "Backspace";

    char dat_00771d48[7] = "Equals";

    char dat_00771d50[6] = "Minus";

    char dat_00771d58[2] = "0";

    char dat_00771d5c[2] = "9";

    char dat_00771d60[2] = "8";

    char dat_00771d64[2] = "7";

    char dat_00771d68[2] = "6";

    char dat_00771d6c[2] = "5";

    char dat_00771d70[2] = "4";

    char dat_00771d74[2] = "3";

    char dat_00771d78[2] = "2";

    char dat_00771d7c[4] = "Esc";

    char dat_00771d80[8] = "(empty)";

    char dat_00774180[2] = "Z";

    char dat_00774184[2] = "Y";

    char *TextInputKeyNameTablePc[238] = {
        dat_00771d80, dat_00771d7c, "1",          dat_00771d78, dat_00771d74, dat_00771d70, dat_00771d6c, dat_00771d68, dat_00771d64,
        dat_00771d60, dat_00771d5c, dat_00771d58, dat_00771d50, dat_00771d48, dat_00771d3c, dat_00771d38, dat_00771d34, dat_00771d30,
        dat_00771d2c, dat_00771d28, dat_00771d24, dat_00774184, dat_00771d20, dat_00771d1c, dat_00771d18, dat_00771d14, dat_00753374,
        dat_00753370, dat_00771d0c, dat_00771d04, dat_00771d00, dat_00771cfc, dat_00771cf8, dat_00771cf4, dat_00771cf0, dat_00771cec,
        dat_00771ce8, dat_00771ce4, dat_00771ce0, dat_00771cd4, dat_00771cc8, dat_00771cc0, dat_00771cb4, dat_00771cb0, dat_00774180,
        dat_00759770, dat_00771cac, dat_00771ca8, dat_00771ca4, dat_00771ca0, dat_00771c9c, dat_00771c94, dat_00771c8c, dat_00771c80,
        dat_00771c74, dat_00771c6c, dat_00771c64, dat_00771c5c, dat_00771c50, dat_00771c4c, dat_00771c48, dat_00771c44, dat_00771c40,
        dat_00771c3c, dat_00771c38, dat_00771c34, dat_00771c30, dat_00771c2c, dat_00771c28, dat_00771c1c, dat_00771c10, dat_00771c08,
        dat_00771c00, dat_00771bf8, dat_00771bf0, dat_00771be8, dat_00771be0, dat_00771bd8, dat_00771bd0, dat_00771bc8, dat_00771bc0,
        dat_00771bb8, dat_00771bb0, dat_00771ba4, dat_007516c0, dat_007516c0, "\\",         dat_00771ba0, dat_00771b9c, dat_007516c0,
        dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
        dat_007516c0, dat_00771b98, dat_00771b94, dat_00771b90, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
        dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b88, dat_007516c0, dat_007516c0, dat_00771b80, dat_007516c0,
        dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b78, dat_007516c0, dat_00771b6c, dat_007516c0, dat_00771b68,
        dat_00771b60, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
        dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771b58, dat_007516c0, dat_007516c0,
        dat_00771b4c, dat_00771b48, dat_00771b40, dat_00771b34, dat_00771b2c, dat_00771b24, dat_00771b20, dat_00771b14, dat_007516c0,
        dat_00771b08, dat_007516c0, dat_007516c0, dat_00771afc, dat_00771af4, dat_007516c0, dat_007516c0, dat_00771aec, dat_00771ae4,
        dat_00771adc, dat_007516c0, dat_00771ad0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
        dat_007516c0, dat_007516c0, dat_007516c0, dat_00771ac8, dat_007516c0, dat_00771ac0, dat_007516c0, dat_00771ab8, dat_00771aac,
        dat_007516c0, dat_00771aa4, dat_007516c0, dat_00771a9c, dat_00771a94, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
        dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a8c,
        dat_007516c0, dat_00771a84, dat_00771a80, dat_00771a78, dat_007516c0, dat_00771a70, dat_007516c0, dat_00771a68, dat_007516c0,
        dat_00771a64, dat_0075ac34, dat_00771a58, dat_00771a50, dat_0076fde8, dat_007516c0, dat_007516c0, dat_007516c0, dat_007516c0,
        dat_007516c0, dat_007516c0, dat_007516c0, dat_00771a48, dat_00771a40, dat_00771a38, dat_00771a30, dat_00771a28, dat_007516c0,
        dat_007516c0, dat_007516c0, dat_00771a20, dat_007516c0, dat_00771a14, dat_00771a04, dat_007719f8, dat_007719f0, dat_007719e4,
        dat_007719dc, dat_007719d0, dat_007719c8, dat_007719bc};
}

extern "C" {

    f32 PUNCHGAP = 0.3f;

    f32 PUNCHCHARGAP = 0.6f;

    i32 CanPunchGirls = 1;

    i32 (*UsingExtraActionsFn)(GAMEOBJECT *) = 0;
}

extern "C" {

    LEVELDATA *DEATHSTARRESCUEE_LDATA = 0;

    LEVELDATA *DEATHSTARESCAPEA_LDATA = 0;

    LEVELDATA *DEATHSTARESCAPEC_LDATA = 0;

}

extern "C" {

    u32 LswForceUsedHintFlags = 0;

    i32 LEGO_AIPATHCNX_BLOCKAGE = 0;

    i32 LEGO_AIPATHCNX_WALLSHUFFLE = 0;
}

extern "C" {

    i32 RenderHubFromCutscenePlayer = 0;
}

extern "C" {
    char UiBackPromptLiteral[5] = "Back";

    char UiSelectPromptLiteral[7] = "Select";
}

extern "C" {
    char dat_0075314c[7] = "batman";

    char dat_0075d280[14] = "batman_shadow";

    char dat_0075d270[13] = "batman_glide";

    char dat_0075d25c[18] = "batman_demolition";

    char dat_0075d24c[13] = "batman_sonar";

    char dat_00753144[6] = "robin";

    char dat_0075d240[12] = "robin_water";

    char dat_0075d22c[17] = "robin_technology";

    char dat_0075d21c[13] = "robin_magnet";

    char dat_0075d20c[14] = "robin_attract";

    /* PodRacePc is defined as a private static in legoapi/episodeI.c. */
    /* (was: void *PodRacePc) */
}

extern "C" {
    i32 gExplodeInitialised = 1;

    u32 GAMEPAD_TOGGLELEFT = PAD_BUTTON_L2 | PAD_BUTTON_L1;

    u32 GAMEPAD_TOGGLERIGHT = PAD_BUTTON_R2 | PAD_BUTTON_R1;

    i32 LEGOHINT_FREEPLAYTOGGLE = -1;

    char *implode_inbuffer = 0;

    char *implode_outbuffer = 0;

    u32 implode_bitbuf = 0;

    u32 implode_compsize = 0;

    u16 implode_left[0x3fb] = {0};

    u16 implode_right[0x3fb] = {0};

    u32 implode_origsize = 0;
}

struct PC_OGG_CACHE_RECORD;
extern "C" {
    extern const IID PCSoundListenerIID = {
        0x279afa84, 0x4981, 0x11ce, {0xa5, 0x21, 0x00, 0x20, 0xaf, 0x0b, 0xe5, 0x60}
    };
    extern const GUID PCSampleBufferAlgorithm = {
        0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
    };
}

extern "C" {
    extern const NUVEC ZipUpHookOffset = {0.0f, -0.1143f, -0.0859f};
    NUMTL *ZipUpLineMaterial = 0;
}

extern "C" {
    const NUVEC NuPortalBoundsOne = {1.0f, 1.0f, 1.0f};
    const NUVEC NuPortalBoundsMinusOne = {-1.0f, -1.0f, -1.0f};
}

extern "C" {
    u32 GAMEPAD_START = PAD_BUTTON_START;
    i32 LEGO_AIPATHCNX_R2D2GLIDE = 0;
}

extern "C" {
    __declspec(align(16)) KEEP_SCREEN_PLANE PlayPlane[6] = {};
    i32 KEEPONSCREEN_SIDESONLY = 0;
    NUVEC GunshipANorm = {};
}

extern "C" {
    f32 (*GizmoPickups_Collide2DFn)(GAMEOBJECT *) = NULL;
}

extern "C" {
    f32 LevelNameTime = 0.0f;
}

extern "C" {
    i16 PermanentWeirdo1Id = -1;
    i16 PermanentWeirdo2Id = -1;
}

extern "C" {
    i32 DefaultMotionLedgeTerrainEnabled = 0;
}

extern "C" {
    i16 DefaultAnimationJabbaId = -1;
    u8 MENUENTRYG = 127;
    u8 MENUENTRYB = 255;
    u8 MENUFLASH0G = 255;
    u8 MENUFLASH0B = 255;
    u8 MENUFLASH1R = 127;
    u8 MENUFLASH1B = 255;
    f32 dme_sx = 1.0f;
    u8 dme_r = 0;
    u8 dme_g = 0;
    u8 dme_b = 0;
    f32 SLAMJUMPSPEED = 3.0f;
}

struct PC_GEOMETRY_BASIS;
struct PC_GEOMETRY_OUTPUT;

struct PC_GEOMETRY_PUBLISH_RECORD;

extern "C" {
    char *LeverOutputDownNamePtr = dat_0075ac34;
    i32 PushBlockHintId = -1;
}

extern "C" {
    i16 id_MOUSEDROID = -1;
    i16 id_BATTLEDROID = -1;
}

extern "C" {
    i16 id_BARMAN = -1;
}

extern "C" {
    i32 Tag_DoneAny = 0;
    u32 GAMEPAD_SKIP = PAD_BUTTON_TRIANGLE | PAD_BUTTON_CROSS | PAD_BUTTON_START;
}

extern "C" {
    i32 (*Tag_NoHiddenIconFn)(GAMEOBJECT *) = NULL;
    TIMER ChallengeTimer = {0};
}

extern "C" {
    const char *TagAsciiDown = "\xC2\xA3";
}

extern "C" {
    i32 (*AlternateGameCallback)(GAMEOBJECT *object) = NULL;
    i32 (*Hub_PanelBusyFn)(void) = NULL;

    i32 LEGOMENU_PAUSEMAIN = -1;
    i32 LEGOMENU_CREDITS = -1;
    i32 LEGOCAMMODE_DOORCUT = -1;
    i32 LEGOCAMMODE_OBSTACLE = -1;
    i32 PlayerHitPointsTwoHearts = 1;
    i32 LEGOHINT_SHOOTCAMERAS = -1;
    i32 LEGOHINT_BUILD = -1;
}

extern "C" {
    i32 NuDisplayHandlerZMode = -1;
}

extern "C" {
    extern const f32 PCSamplerBiasMinimum = -16.0f;
}

/* Verified DWORD working inputs; original owning extents remain unknown. */

extern "C" {
    i32 NuFile_SwapEndianOnWrite = 0;
}

extern "C" {
    extern const u8 PcInputPovBitsPc[8] = {1, 3, 2, 6, 4, 12, 8, 9};
}

extern "C" {}

extern "C" {
    char SaveFolderPathPc[256] = {0};
    i32 saveload_slotcode[6] = {0};
}

extern "C" {
    char SaveProductCode[12] = {0};
}

extern "C" {
    extern const char dat_00758274[] = "reserve";
    extern const char dat_0075827c[] = "set";
    extern const char dat_00758280[] = "locator";
    extern const char dat_00758288[] = "locator=mylocator";
    extern const char dat_0075829c[] = "nearest_either_player";
    extern const char dat_007582b4[] = "furthest_from_either_player";
    extern const char dat_007582d0[] = "furthest_from_opponent";
    extern const char dat_007582e8[] = "ignore_assigned=FALSE";
    extern const char dat_00758300[] = "ignore_assigned=TRUE";
    extern const char dat_00758318[] = "finish_at_end";
    extern const char dat_00758328[] = "first";
    extern const char dat_00758330[] = "next";
    extern const char dat_00758338[] = "off_screen_radius=";
    extern const char dat_0075834c[] = "max_opponent_range=";
    extern const char dat_00758360[] = "max_player_range=";
    extern const char dat_00758374[] = "max_range=";
}

extern "C" {
    extern const char dat_007589f0[] = "NO_TARGET";
    extern const char dat_007589fc[] = "CREATURE";
    extern const char dat_00758a08[] = "HSPECIAL";
    extern const char dat_00758a14[] = "GIZMO";
    extern const char dat_00758a1c[] = "Move_Vertical";
    extern const char dat_00758a2c[] = "Move_Horizontal";
    extern const char dat_00758a3c[] = "Move_Spin";
    extern const char dat_00758a48[] = "Move_Waggle";
    extern const char dat_00758a54[] = "techno=";
}

extern "C" {
    u32 LEGO_AIPATHCNX_BIGJUMP = 0;
}

extern "C" {
    i16 id_WATTO = -1;
}

extern "C" {

    TIMER PauseTimer = {0};

}

extern "C" {}

extern "C" {}

extern "C" {
    i32 g_lowEndLevelBehaviour = 1;
}

extern "C" {

    void edppInit(void);
    void edppClose(void);
    void edppEnter(void);
    void edppApply(void);
    i32 edppProc(f32 delta_time, FRAME_PAD *pad);
    void edppRender(void);
    ed_module_s edptldesc = {
        NULL,       NULL, "Particle Positioner", edppInit, edppClose, edppEnter, NULL, edppApply, NULL, NULL, 0x706c7470, edppProc,
        edppRender, NULL};

    u8 edpp_effect_list = 0;

}

extern "C" {
    f32 iconalphaoverride = 0;
}

extern "C" {
    TIMER SuperStoryTimer = {0};
}

extern "C" {

    STATUSPACKET_s StatusPacket = {0};

    i32 status_prompt = 0;
}

extern "C" {}

extern "C" {}

extern "C" {

    struct PC_SCENE_MTL_VIEW *edpart_mtl = 0;

    struct PC_SCENE_MTL_VIEW *edpart_boxmtl = 0;
}

extern "C" {}

extern "C" {}

extern "C" {

    char edbits_level_save_directory[256] = {0};

    char edbits_level_save_name[256] = {0};

    char edbits_level_save_extension[256] = {0};
}

extern "C" {

    struct eduimenu_s *edgra_options_menu = 0;

    struct eduimenu_s *edgra_active_menu = 0;

}

extern "C" {

    void edbriInit(void);
    void edbriClose(void);
    void edbriEnter(void);
    i32 edbriProc(f32 delta_time, FRAME_PAD *pad);
    void edbriRender(void);
    ed_module_s edbridesc = {NULL, NULL, "Bridge Editor", edbriInit, edbriClose,  edbriEnter, NULL, NULL,
                             NULL, NULL, 0x64697262,      edbriProc, edbriRender, NULL};
}

extern "C" {

    i32 edgra_filter = 0;

}

extern "C" {
    i32 PauseMenus_Align = 0;

    f32 PauseMenus_X = 0;
}

extern "C" {}

extern "C" {
    struct eduimenu_s *active_menu = 0;

    f32 edui_cursor_x = 50.0f;

    char *textrow[4] = {0};

}

extern "C" {
    i32 edgra_nearest_instance = 0;
}

extern "C" {

    i32 edanim_nearest_sound = 0;
}

extern "C" {

    i32 edanim_nearest_particle = 0;
}

extern "C" {
    edcam_s edcam_default = {
        {0.0f, 0.0f, 0.0f},
        0,
        0,
        -2.0f,
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f},
        {1.0f, 1.0f, 1.0f},
        1,
        1,
        {0.0f, 0.0f, 0.0f},
        0,
        0,
        {0.0015625f, 0.0015625f, 0.0015625f},
        2,
        2,
        0.0078125f,
        0.5f,
        0.2f,
        0.2f,
        4.0f,
        0.15f,
        0.2f,
        0.01f,
        0.1f,
        {0x3f}
    };

    edcam_s *gp_cam = &edcam_default;
}

extern "C" {
    f32 edgra_size = 0;
}

extern "C" {}

extern "C" {
    DETONATOR_s Detonator[10] = {0};
}

extern "C" {

    // Frontend strings: language-selected index/string tables.

    extern const char dat_0075f2c0[3] = "No";
    extern const char dat_0075f2c4[4] = "Yes";
    extern const char dat_0076b274[4] = "Off";
    MENU_PC_STRING_ENTRY dat_00827800[40] = {
        {0,  "Quit"                                                       },
        {1,  "Quit to Windows"                                            },
        {2,  "Quit to Demos Menu"                                         },
        {3,  "Return to game"                                             },
        {4,  "Low"                                                        },
        {5,  "Medium"                                                     },
        {6,  "High"                                                       },
        {7,  "Resume"                                                     },
        {8,  "Accept"                                                     },
        {10, "Back"                                                       },
        {9,  "Decline"                                                    },
        {11, dat_0075f2c4                                                 },
        {12, dat_0075f2c0                                                 },
        {13, "OK"                                                         },
        {14, "Cancel"                                                     },
        {15, "On"                                                         },
        {16, dat_0076b274                                                 },
        {17, "Reset To Defaults"                                          },
        {18, "Video Settings"                                             },
        {19, "Screen Resolution"                                          },
        {20, "Refresh Rate"                                               },
        {21, "Vertical Sync"                                              },
        {22, "Aspect Ratio"                                               },
        {23, "Enhanced Graphics"                                          },
        {24, "From Screen-Res"                                            },
        {25, "Use current settings"                                       },
        {26, "Texture Quality"                                            },
        {27, "Revert"                                                     },
        {28, "You must restart the game for these changes to take effect!"},
        {29, "Effects Settings"                                           },
        {30, "FSAA"                                                       },
        {31, "Externally Set"                                             },
        {32, "Shadows"                                                    },
        {33, "Bloom"                                                      },
        {34, "Depth of Field"                                             },
        {35, "Speed Blur"                                                 },
        {36, "Dynamic Light Quality"                                      },
        {37, "Edge AA"                                                    },
        {38, "Show Game Controls"                                         },
        {39, NULL                                                         },
    };
    MENU_PC_STRING_ENTRY dat_00827940[40] = {
        {0,  "Quit"                                                       },
        {1,  "Quit to Windows"                                            },
        {2,  "Quit to Demos Menu"                                         },
        {3,  "Return to game"                                             },
        {4,  "Low"                                                        },
        {5,  "Medium"                                                     },
        {6,  "High"                                                       },
        {7,  "Resume"                                                     },
        {8,  "Accept"                                                     },
        {9,  "Decline"                                                    },
        {10, "Back"                                                       },
        {11, dat_0075f2c4                                                 },
        {12, dat_0075f2c0                                                 },
        {13, "OK"                                                         },
        {14, "Cancel"                                                     },
        {15, "On"                                                         },
        {16, dat_0076b274                                                 },
        {17, "Reset To Defaults"                                          },
        {18, "Video Settings"                                             },
        {19, "Screen Resolution"                                          },
        {20, "Refresh Rate"                                               },
        {21, "Vertical Sync"                                              },
        {22, "Aspect Ratio"                                               },
        {23, "Enhanced Graphics"                                          },
        {24, "From Screen-Res"                                            },
        {25, "Use current settings"                                       },
        {26, "Texture Quality"                                            },
        {27, "Revert"                                                     },
        {28, "You must restart the game for these changes to take effect!"},
        {29, "Effects Settings"                                           },
        {30, "FSAA"                                                       },
        {31, "Externally Set"                                             },
        {32, "Shadows"                                                    },
        {33, "Bloom"                                                      },
        {34, "Depth of Field"                                             },
        {35, "Speed Blur"                                                 },
        {36, "Dynamic Light Quality"                                      },
        {37, "Edge AA"                                                    },
        {38, "Show Game Controls"                                         },
        {39, NULL                                                         },
    };
    MENU_PC_STRING_ENTRY dat_00827d00[40] = {
        {0,  "Esci"                                                                 },
        {1,  "Torna a Windows"                                                      },
        {2,  "Esci al Menu Demo"                                                    },
        {3,  "Torna al gioco"                                                       },
        {4,  "Bassa"                                                                },
        {5,  "Media"                                                                },
        {6,  "Alta"                                                                 },
        {7,  "Riprendi"                                                             },
        {8,  "Accetta"                                                              },
        {9,  "Annulla"                                                              },
        {10, "Indietro"                                                             },
        {11, "S\303\254"                                                            },
        {12, dat_0075f2c0                                                           },
        {13, "OK"                                                                   },
        {14, "Annulla"                                                              },
        {15, "On"                                                                   },
        {16, dat_0076b274                                                           },
        {17, "Impostazioni predefinite"                                             },
        {18, "Video"                                                                },
        {19, "Risoluzione schermo"                                                  },
        {20, "Frequenza di aggiornamento"                                           },
        {21, "Sincronizzazione verticale"                                           },
        {22, "Rapporto"                                                             },
        {23, "Grafica migliorata"                                                   },
        {24, "Da risoluzione"                                                       },
        {25, "Usa impostazioni attuali"                                             },
        {26, "Qualit\303\240 texture"                                               },
        {27, "Ripristina"                                                           },
        {28, "Devi riavviare il gioco affinch\303\251 le modifiche abbiano effetto."},
        {29, "Effetti"                                                              },
        {30, "FSAA"                                                                 },
        {31, "Impostato esternamente"                                               },
        {32, "Ombre"                                                                },
        {33, "Luminosit\303\240"                                                    },
        {34, "Profondit\303\240 di campo"                                           },
        {35, "Sfumatura movimento"                                                  },
        {36, "Qualit\303\240 luci dinamiche"                                        },
        {37, "Edge AA"                                                              },
        {38, "Show Game Controls"                                                   },
        {39, NULL                                                                   },
    };
    MENU_PC_STRING_ENTRY dat_00827e40[40] = {
        {0,  "Salir"                                                        },
        {1,  "Salir a Windows"                                              },
        {2,  "Salir al menu del demo"                                       },
        {3,  "Volver al juego"                                              },
        {4,  "Baja"                                                         },
        {5,  "Media"                                                        },
        {6,  "Alta"                                                         },
        {7,  "Reanudar"                                                     },
        {8,  "Aceptar"                                                      },
        {9,  "Rechazar"                                                     },
        {10, "Atr\303\241s"                                                 },
        {11, "S\303\255"                                                    },
        {12, dat_0075f2c0                                                   },
        {13, "Aceptar"                                                      },
        {14, "Cancelar"                                                     },
        {15, "S\303\255"                                                    },
        {16, dat_0075f2c0                                                   },
        {17, "Restablecer predeterminados"                                  },
        {18, "Ajustes de v\303\255deo"                                      },
        {19, "Resoluci\303\263n"                                            },
        {20, "Tasa de actualizaci\303\263n"                                 },
        {21, "Sincronizaci\303\263n vertical"                               },
        {22, "Relaci\303\263n de aspecto"                                   },
        {23, "Gr\303\241ficos mejorados"                                    },
        {24, "R. pantalla"                                                  },
        {25, "Usar ajustes de actuales"                                     },
        {26, "Calidad de textura"                                           },
        {27, "Revertir"                                                     },
        {28, "Debes reiniciar el juego para que estos cambios tengan efecto"},
        {29, "Ajustes de efectos"                                           },
        {30, "AAPC"                                                         },
        {31, "Ajustado externamente"                                        },
        {32, "Sombras"                                                      },
        {33, "Bloom"                                                        },
        {34, "Profundidad de campo"                                         },
        {35, "Distorsi\303\263n de velocidad"                               },
        {36, "Calidad de luz din\303\241mica"                               },
        {37, "Suavizado de bordes"                                          },
        {38, "Show Game Controls"                                           },
        {39, NULL                                                           },
    };
}

extern "C" {}

extern "C" {}

extern "C" {
    i16 dat_007f1810 = -1;
    i16 dat_007f1820 = -1;
    i16 dat_007f1824 = -1;
    i16 dat_007f1828 = -1;
    i16 dat_007f1844 = -1;
    i16 dat_007f1854 = -1;
    i16 dat_007f1858 = -1;
    i16 dat_007f1864 = -1;
    i16 dat_007f1870 = -1;
    i16 dat_007f18a8 = -1;
    i16 dat_007f18c4 = -1;
    i16 dat_007f19ac = -1;
    i16 dat_007f19b0 = -1;
    i16 dat_007f19b4 = -1;
    i16 dat_007f1a0c = -1;
    i16 dat_007f1a10 = -1;
    i16 dat_007f1a14 = -1;
    i16 dat_007f1a3c = -1;
    i16 dat_007f1a40 = -1;
    i16 dat_007f1a78 = -1;
    i16 dat_007f1a7c = -1;
    i16 dat_007f1a88 = -1;
    i16 dat_007f1ac0 = -1;
    i16 dat_007f1b10 = -1;
    i16 dat_007f1b20 = -1;
    i16 dat_007f1b24 = -1;
    i16 dat_007f1b28 = -1;
    i16 dat_007f1b48 = -1;
    CHARFIXUP CharacterFixupTable[219] = {
        {const_cast<char *>("weirdo1"),                    &PermanentWeirdo1Id                                    },
        {const_cast<char *>("weirdo2"),                    &PermanentWeirdo2Id                                    },
        {const_cast<char *>(dat_0075314c),                 &ID_DEFAULTCHARACTER_A                                 },
        {const_cast<char *>(dat_00753144),                 &ID_DEFAULTCHARACTER_B                                 },
        {"henchman",                                       &dat_007f1810                                          },
        {const_cast<char *>("buggy"),                      reinterpret_cast<i16 *>(&GameObjectRotationBuggyId)    },
        {const_cast<char *>("glide_pack"),                 &dat_007f17bc                                          },
        {const_cast<char *>("radiocar"),                   reinterpret_cast<i16 *>(&GameObjectRotationRadioCarId) },
        {const_cast<char *>("policecar"),                  reinterpret_cast<i16 *>(&GameObjectRotationPoliceCarId)},
        {const_cast<char *>("twoface"),                    &dat_007f1820                                          },
        {const_cast<char *>("harlequinn"),                 &dat_007f1824                                          },
        {const_cast<char *>("catwoman"),                   &PunchRangeUnitId                                      },
        {const_cast<char *>("whip"),                       &PunchRangeCompanionId                                 },
        {const_cast<char *>("PenguinBomb"),                &dat_007f1828                                          },
        {const_cast<char *>("p_goon"),                     &id_PENGUIN_GOON                                       },
        {const_cast<char *>("p_goon_gun"),                 &id_PENGUIN_GOON_GUN                                   },
        {const_cast<char *>("j_goon"),                     &id_JOKER_GOON                                         },
        {const_cast<char *>("j_goon_gun"),                 &id_JOKER_GOON_GUN                                     },
        {const_cast<char *>("r_goon"),                     &id_RIDDLER_GOON                                       },
        {const_cast<char *>("r_goon_gun"),                 &id_RIDDLER_GOON_GUN                                   },
        {const_cast<char *>("jackinabox"),                 &dat_007f1844                                          },
        {const_cast<char *>("mower"),                      reinterpret_cast<i16 *>(&GameObjectRotationMowerId)    },
        {const_cast<char *>("dodgem"),                     &id_DODGEM                                             },
        {const_cast<char *>("gyrocopter"),                 &id_GYROCOPTER                                         },
        {const_cast<char *>("clayface"),                   &dat_007f1854                                          },
        {const_cast<char *>("penguin"),                    &dat_007f17c8                                          },
        {const_cast<char *>("umbrella"),                   &dat_007f17cc                                          },
        {const_cast<char *>("poisonivy"),                  &dat_007f1858                                          },
        {const_cast<char *>("quigonjinn"),                 &ID_DEFAULTCHARACTER_LSW_A                             },
        {"obiwankenobi",                                   &ID_DEFAULTCHARACTER_LSW_B                             },
        {const_cast<char *>("princessleia"),               &dat_007f1864                                          },
        {const_cast<char *>("princessleia_slave"),         &DefaultAnimationLeiaSlaveId                           },
        {const_cast<char *>("princessleia_Boushh"),        &id_PRINCESSLEIABOUSHH                                 },
        {const_cast<char *>("captainantilles"),            &dat_007f1870                                          },
        {const_cast<char *>("captaintarpals"),             &id_CAPTAINTARPALS                                     },
        {const_cast<char *>("imperialguard"),              &id_IMPERIALGUARD                                      },
        {const_cast<char *>("bodyguard"),                  &id_BODYGUARD                                          },
        {const_cast<char *>("darthmaul"),                  &id_DARTHMAUL                                          },
        {const_cast<char *>("gamorreanguard"),             &id_GAMORREANGUARD                                     },
        {"battledroid",                                    &id_BATTLEDROID                                        },
        {const_cast<char *>("battledroid_commander"),      &id_BATTLEDROIDCOMMANDER                               },
        {const_cast<char *>("battledroid_geonosian"),      &id_BATTLEDROIDGEONOSIAN                               },
        {const_cast<char *>("battledroid_security"),       &id_BATTLEDROIDSECURITY                                },
        {const_cast<char *>("Clone_Ep3"),                  &id_CLONEEP3                                           },
        {const_cast<char *>("Clone_Ep3_Sand"),             &id_CLONEEP3SAND                                       },
        {const_cast<char *>("naffdroid1"),                 &id_NAFFDROID1                                         },
        {const_cast<char *>("naffdroid2"),                 &id_NAFFDROID2                                         },
        {const_cast<char *>("naffdroid3"),                 &dat_007f18a8                                          },
        {const_cast<char *>("naffdroid4"),                 &id_NAFFDROID4                                         },
        {const_cast<char *>("mousedroid"),                 &id_MOUSEDROID                                         },
        {const_cast<char *>("probedroid"),                 &dat_007f18b4                                          },
        {const_cast<char *>("pkdroid"),                    &id_PKDROID                                            },
        {const_cast<char *>("snake"),                      &id_SNAKE                                              },
        {const_cast<char *>("bat"),                        &id_BAT                                                },
        {const_cast<char *>("womprat"),                    &dat_007f18c4                                          },
        {const_cast<char *>("yoda"),                       &id_YODA                                               },
        {const_cast<char *>("yoda_ghost"),                 &id_YODAGHOST                                          },
        {const_cast<char *>("moseisleycitizen"),           &id_MOSEISLEYCITIZEN                                   },
        {const_cast<char *>("cantinaaliens"),              &id_CANTINAALIEN                                       },
        {const_cast<char *>("cloudcitycitizen"),           &id_CLOUDCITYCITIZEN                                   },
        {const_cast<char *>("geonosian"),                  &id_GEONOSIAN                                          },
        {const_cast<char *>("watto"),                      &id_WATTO                                              },
        {const_cast<char *>("weirdo1"),                    &PermanentWeirdo1Id                                    },
        {const_cast<char *>("weirdo2"),                    &PermanentWeirdo2Id                                    },
        {const_cast<char *>("chewbacca"),                  &id_CHEWBACCA                                          },
        {const_cast<char *>("wookie"),                     &id_WOOKIEE                                            },
        {"atst",                                           reinterpret_cast<i16 *>(&GameObjectRotationAtstId)     },
        {"atst_lowres",                                    &id_ATST_LOWRES                                        },
        {const_cast<char *>("barman"),                     &id_BARMAN                                             },
        {const_cast<char *>("destroyer"),                  &id_DROIDEKA                                           },
        {const_cast<char *>("superbattledroid"),           &id_SUPERBATTLEDROID                                   },
        {const_cast<char *>("bobafett"),                   &id_BOBAFETT                                           },
        {const_cast<char *>("4lom"),                       &dat_007f190c                                          },
        {const_cast<char *>("trainingremote"),             &dat_007f1910                                          },
        {const_cast<char *>("atat"),                       &id_ATAT                                               },
        {const_cast<char *>("service_car"),                &id_SERVICECAR                                         },
        {const_cast<char *>("dragbomb"),                   reinterpret_cast<i16 *>(&id_DRAGBOMB)                  },
        {"clonewalker",                                    &id_CLONEWALKER                                        },
        {const_cast<char *>("ewok"),                       &dat_007f1924                                          },
        {const_cast<char *>("wicket"),                     &dat_007f1928                                          },
        {const_cast<char *>("catapult"),                   &id_CATAPULT                                           },
        {const_cast<char *>("basketcannon"),               &id_BASKETCANNON                                       },
        {const_cast<char *>("bantha"),                     &id_BANTHA                                             },
        {const_cast<char *>("bomarrmonk"),                 &id_BOMARRMONK                                         },
        {const_cast<char *>("dewback"),                    &id_DEWBACK                                            },
        {const_cast<char *>("speeder_land"),               &id_LANDSPEEDER                                        },
        {const_cast<char *>("tauntaun"),                   &id_TAUNTAUN                                           },
        {const_cast<char *>("speederbike"),                &id_SPEEDERBIKE                                        },
        {const_cast<char *>("heavyrepeatingcannon"),       &id_HEAVYREPEATINGCANNON                               },
        {const_cast<char *>("biggun"),                     &id_BIGGUN                                             },
        {const_cast<char *>("troopercannon"),              &id_TROOPERCANNON                                      },
        {const_cast<char *>("jabba"),                      &DefaultAnimationJabbaId                               },
        {const_cast<char *>("slave1"),                     &dat_007f195c                                          },
        {const_cast<char *>("ywing"),                      &id_YWING                                              },
        {const_cast<char *>("tiebomber"),                  &id_TIEBOMBER                                          },
        {const_cast<char *>("grievous"),                   &id_GRIEVOUS                                           },
        {const_cast<char *>("tiefighterdarth"),            &dat_007f196c                                          },
        {const_cast<char *>("tiefighter"),                 &id_TIEFIGHTER                                         },
        {const_cast<char *>("grabberControl"),             &id_GRABCONTROL                                        },
        {const_cast<char *>("grabberr2control"),           &id_GRABR2CONTROL                                      },
        {"grabber",                                        &GrabMachineId                                         },
        {const_cast<char *>("magnet"),                     &GrabMagnetId                                          },
        {const_cast<char *>("robot_base"),                 &RobotBaseId                                           },
        {const_cast<char *>("jawa"),                       &id_JAWA                                               },
        {const_cast<char *>("lukeskywalker_dagobah"),      &id_LUKESKYWALKERDAGOBAH                               },
        {const_cast<char *>("SnowMob"),                    &id_SNOWMOB                                            },
        {const_cast<char *>("snowtrooper"),                &id_SNOWTROOPER                                        },
        {const_cast<char *>("deathstartrooper"),           &id_DEATHSTARTROOPER                                   },
        {const_cast<char *>("darthvader"),                 &dat_007f199c                                          },
        {const_cast<char *>("theemperor"),                 &id_THEEMPEROR                                         },
        {const_cast<char *>("millenniumfalcon"),           &dat_007f19a4                                          },
        {const_cast<char *>("speederbikesnow"),            &dat_007f19a8                                          },
        {const_cast<char *>("Kaadu"),                      &dat_007f19ac                                          },
        {const_cast<char *>(AreaFixUpName_Gungan),         &dat_007f19b0                                          },
        {const_cast<char *>("Falumpaset"),                 &dat_007f19b4                                          },
        {const_cast<char *>("Stap"),                       &id_STAP                                               },
        {const_cast<char *>("Stap2"),                      &id_STAP2                                              },
        {const_cast<char *>("jumbohomingdroid"),           &id_JUMBOHOMINGDROID                                   },
        {const_cast<char *>("JarJarBinks"),                &dat_007f19c4                                          },
        {const_cast<char *>("PadmeClawed"),                &dat_007f19c8                                          },
        {const_cast<char *>("Anakin_padawan"),             &id_ANAKINPADAWAN                                      },
        {const_cast<char *>("ObiWanKenobi_JediMaster"),    &dat_007f19d0                                          },
        {const_cast<char *>("shaakti"),                    &dat_007f19d4                                          },
        {const_cast<char *>("luminara"),                   &dat_007f19d8                                          },
        {const_cast<char *>("bob"),                        &id_BOB                                                },
        {const_cast<char *>("JangoFett"),                  &id_JANGOFETT                                          },
        {const_cast<char *>("JediStarfighter_Yellow_Ep3"), &id_JEDISTARFIGHTERYELLOWEP3                           },
        {const_cast<char *>("JediStarfighter_Red_Ep3"),    &dat_007f19e8                                          },
        {const_cast<char *>("Rancor"),                     &id_RANCOR                                             },
        {const_cast<char *>("obiwankenobi_ep3"),           &id_OBIWANKENOBIEP3                                    },
        {const_cast<char *>("anakin_jedi"),                &id_ANAKINJEDI                                         },
        {"republicgunship",                                &id_REPUBLICGUNSHIP                                    },
        {const_cast<char *>("republicgunship_green"),      &id_REPUBLICGUNSHIP_GREEN                              },
        {const_cast<char *>("newrepublicgunship"),         &id_NEW_REPUBLIC_GUNSHIP                               },
        {const_cast<char *>("newrepublicgunship_green"),   &id_NEW_REPUBLIC_GUNSHIP_GREEN                         },
        {const_cast<char *>("countdooku"),                 &id_DOOKU                                              },
        {const_cast<char *>("palpatine"),                  &dat_007f1a0c                                          },
        {const_cast<char *>("kitfisto"),                   &dat_007f1a10                                          },
        {const_cast<char *>("r2d2"),                       &dat_007f1a14                                          },
        {const_cast<char *>("r2q5"),                       &id_R2Q5                                               },
        {const_cast<char *>("xwing"),                      &id_XWING                                              },
        {const_cast<char *>("royalguard"),                 &id_ROYALGUARD                                         },
        {const_cast<char *>("snowspeeder"),                &dat_007f1a24                                          },
        {const_cast<char *>("tieinterceptor"),             &dat_007f1a28                                          },
        {const_cast<char *>("imperialshuttle"),            &dat_007f1a2c                                          },
        {const_cast<char *>("ugnaught"),                   &id_UGNAUGHT                                           },
        {const_cast<char *>("sentrydroid"),                &id_SENTRYDROID                                        },
        {const_cast<char *>("kaminoandroid"),              &dat_007f1a38                                          },
        {"stormtrooper",                                   &dat_007f1a3c                                          },
        {const_cast<char *>("beachtrooper"),               &dat_007f1a40                                          },
        {const_cast<char *>("ImperialShuttlePilot"),       &dat_007f1a44                                          },
        {const_cast<char *>("imperialofficer"),            &dat_007f1a48                                          },
        {const_cast<char *>("grandmofftarkin"),            &dat_007f1a4c                                          },
        {const_cast<char *>("gonkdroid"),                  reinterpret_cast<i16 *>(&id_GONKDROID)                 },
        {const_cast<char *>("c3po"),                       &id_C3PO                                               },
        {const_cast<char *>("tc14"),                       &id_TC14                                               },
        {const_cast<char *>("mooncar"),                    &id_MOONCAR                                            },
        {const_cast<char *>("mapcar"),                     &id_MAPCAR                                             },
        {const_cast<char *>("wookieflyer"),                &id_WOOKIEFLYER                                        },
        {const_cast<char *>("imperialspy"),                &id_IMPERIALSPY                                        },
        {const_cast<char *>("greedo"),                     &id_GREEDO                                             },
        {const_cast<char *>("bossk"),                      &id_BOSSK                                              },
        {const_cast<char *>("cantinaband"),                &id_CANTINABAND                                        },
        {const_cast<char *>("pitdroid"),                   &dat_007f1a78                                          },
        {const_cast<char *>("TownCar"),                    &dat_007f1a7c                                          },
        {const_cast<char *>("Tractor"),                    &id_TRACTOR                                            },
        {const_cast<char *>("fireTruck"),                  &id_FIRETRUCK                                          },
        {const_cast<char *>("lifeBoat"),                   &dat_007f1a88                                          },
        {const_cast<char *>("lamasu"),                     &DefaultAnimationLamaSuId                              },
        {const_cast<char *>("taunwe"),                     &DefaultAnimationTaunWeId                              },
        {const_cast<char *>("buzzdroid"),                  &id_BUZZDROID                                          },
        {const_cast<char *>("clonearc"),                   &dat_007f1a98                                          },
        {"naboostarfighter",                               &dat_007f1a9c                                          },
        {const_cast<char *>("naboostarfighter_lime"),      &dat_007f1aa0                                          },
        {const_cast<char *>("hansolo_indy"),               &dat_007f17d4                                          },
        {const_cast<char *>("flashspeeder"),               &id_FLASHSPEEDER                                       },
        {const_cast<char *>("skeleton"),                   &DefaultMotionSkeletonId                               },
        {const_cast<char *>("indianajones"),               &dat_007f17d0                                          },
        {const_cast<char *>("satipo"),                     &dat_007f1aac                                          },
        {const_cast<char *>("chuck"),                      &dat_007f1ab0                                          },
        {const_cast<char *>("belloq"),                     &dat_007f1ab4                                          },
        {const_cast<char *>("hovitostribesman"),           &VehicleSpecialShooterId                               },
        {const_cast<char *>("barranca"),                   &dat_007f1abc                                          },
        {const_cast<char *>("crocodile"),                  &dat_007f1ac0                                          },
        {const_cast<char *>("indyspider"),                 &ZipUpLineCharacterId                                  },
        {const_cast<char *>("indysnake"),                  &DefaultMotionIndySnakeId                              },
        {const_cast<char *>("mini_x_wing"),                &id_MINIXWING                                          },
        {const_cast<char *>("mini_y_wing"),                &id_MINIYWING                                          },
        {const_cast<char *>("mini_tie_interceptor"),       &dat_007f1ad8                                          },
        {const_cast<char *>("mini_tie_bomber"),            &id_MINITIEBOMBER                                      },
        {const_cast<char *>("mini_atat"),                  &id_MINIATAT                                           },
        {const_cast<char *>("mini_star_destroyer"),        &id_MINISTARDESTROYER                                  },
        {const_cast<char *>("mini_royal_starship"),        &id_MINIROYALSTARSHIP                                  },
        {const_cast<char *>("mini_imperial_shuttle"),      &dat_007f1aec                                          },
        {const_cast<char *>("Mini_Millennium_Falcon"),     &dat_007f1af0                                          },
        {"mini_atst",                                      &id_MINIATST                                           },
        {const_cast<char *>("mini_atte"),                  &id_MINIATTE                                           },
        {const_cast<char *>("mini_slave1"),                &id_MINISLAVE1                                         },
        {const_cast<char *>("mini_droideka"),              &id_MINIDROIDEKA                                       },
        {const_cast<char *>("mini_tie_fighter"),           &dat_007f1b00                                          },
        {const_cast<char *>("mini_tie_advanced"),          &dat_007f1b04                                          },
        {const_cast<char *>("mini_sith_infiltrator"),      &dat_007f1b08                                          },
        {const_cast<char *>("mini_solar_sailor"),          &dat_007f1b0c                                          },
        {const_cast<char *>("mini_sand_crawler"),          &dat_007f1b10                                          },
        {const_cast<char *>("anakinspod"),                 &id_ANAKINSPOD                                         },
        {const_cast<char *>("anakinspod_green"),           &id_ANAKINSPODGREEN                                    },
        {const_cast<char *>("sebulbaspod"),                &id_SEBULBASPOD                                        },
        {const_cast<char *>("gasganospod"),                &dat_007f1b20                                          },
        {const_cast<char *>("anothermiscpod"),             &dat_007f1b24                                          },
        {const_cast<char *>("anothermiscpod2"),            &dat_007f1b28                                          },
        {const_cast<char *>("newanakinspod"),              &id_ANAKINSNEWPOD                                      },
        {const_cast<char *>("newanakinspod_green"),        &id_ANAKINSNEWPODGREEN                                 },
        {const_cast<char *>("anakinsspeeder"),             &dat_007f1b34                                          },
        {const_cast<char *>("anakinsspeeder_green"),       &dat_007f1b38                                          },
        {const_cast<char *>("ZamsSpeeder"),                &id_ZAMSSPEEDER                                        },
        {const_cast<char *>("vulturedroid"),               &dat_007f1b40                                          },
        {const_cast<char *>("droidtrifighter"),            &dat_007f1b44                                          },
        {const_cast<char *>("droidstarfighter"),           &dat_007f1b48                                          },
        {NULL,                                             NULL                                                   },
    };
}

extern "C" {}

extern "C" {
    extern const f64 PanelVariantTwoTargetZ = 0.19f;
    extern const f64 PanelVariantTwoTargetY = 0.245f;
    extern const f64 PanelVariantThreeTargetZ = 0.17f;
    extern const f64 PanelVariantThreeTargetY = 0.34f;
}

extern "C" {}

extern "C" {
    extern "C" {
        extern i16 dat_00880f00;
        extern i16 dat_00880f04;
        extern i16 dat_00880f0c;
        extern i16 dat_00880f10;
        extern i16 dat_00880f08;
        extern i16 dat_00880c34;
        extern i16 dat_00880c38;
        extern i16 dat_00880c3c;
        extern i16 dat_00880c40;
        extern i16 dat_00880c44;
        extern i16 dat_00880c48;
        extern i16 dat_00880c4c;
        extern i16 dat_00880c50;
        extern i16 dat_00880c54;
        extern i16 dat_00880c58;
        extern i16 dat_00880c5c;
        extern i16 dat_00880f14;
        extern i16 dat_00880f18;
        extern i16 dat_00880f1c;
        extern i16 dat_00880f20;
        extern i16 dat_00880f44;
        extern i16 dat_00880894;
        extern i16 dat_00880898;
        extern i16 dat_00880f24;
        extern i16 dat_00880f28;
        extern i16 dat_00880f2c;
        extern i16 dat_00880f30;
        extern i16 dat_00880f34;
        extern i16 dat_00880f38;
        extern i16 dat_00880f3c;
        extern i16 dat_00880f40;
        extern i16 dat_00880f48;
        extern i16 dat_00880f4c;
        extern i16 dat_00880f50;
        extern i16 dat_00880c80;
        extern i16 dat_00880c7c;
        extern i16 dat_00880c74;
        extern i16 dat_00880c78;
        extern i16 dat_00880cb0;
        extern i16 dat_008811c0;
        extern i16 dat_008811bc;
        extern i16 PcControlTextId;
        extern i16 dat_008811b0;
    }
    i16 *PcControlTextIds[45] = {&tACCEPT,      &dat_00880890,    &dat_00880f00, &dat_00880f04, &dat_00880f0c, &dat_00880f10, &dat_00880f08,
                                 &dat_00880c34, &dat_00880c38,    &dat_00880c3c, &dat_00880c40, &dat_00880c44, &dat_00880c48, &dat_00880c4c,
                                 &dat_00880c50, &dat_00880c54,    &dat_00880c58, &dat_00880c5c, &dat_00880f14, &dat_00880f18, &dat_00880f1c,
                                 &dat_00880f20, &dat_00880f44,    &dat_00880894, &dat_00880898, &dat_00880f24, &dat_00880f28, &dat_00880f2c,
                                 &dat_00880f30, &dat_00880f34,    &dat_00880f38, &dat_00880f3c, &dat_00880f40, &dat_00880f48, &dat_00880f4c,
                                 &dat_00880f50, &dat_00880c80,    &dat_00880c7c, &dat_00880c74, &dat_00880c78, &dat_00880cb0, &dat_008811c0,
                                 &dat_008811bc, &PcControlTextId, &dat_008811b0};

    i16 PcControlTextId = 0;
}

extern "C" {
    extern i16 dat_00880990;
    extern i16 dat_00880994;
    extern i16 dat_00880998;
    extern i16 dat_0088099c;
    extern i16 dat_008809a0;
    extern i16 dat_008809a4;
    extern i16 dat_008809a8;
    extern i16 dat_008809ac;
    extern i16 dat_008809b0;
    extern i16 dat_008809b4;
    extern i16 dat_008809b8;
    extern i16 dat_008809bc;
    extern i16 dat_008809c0;
    extern i16 dat_008809c4;
    extern i16 dat_008809c8;
    extern i16 dat_008809cc;
    extern i16 dat_008809d0;
    extern i16 dat_00881044;
    extern i16 dat_00881048;
    extern i16 dat_0088104c;
    extern i16 dat_00881058;
    extern i16 dat_0088105c;
    extern i16 dat_00881060;
    extern i16 dat_00881064;
    extern i16 dat_00881068;
    extern i16 dat_0088106c;
    extern i16 dat_00881070;
    extern i16 dat_00881074;
    extern i16 dat_00881078;
    extern i16 dat_0088107c;
    extern i16 dat_00881080;
    extern i16 dat_00881084;
    extern i16 dat_00881088;
    extern i16 dat_0088108c;
    extern i16 dat_00881090;
    extern i16 dat_00881094;
    extern i16 dat_00881098;
    extern i16 dat_0088109c;
    extern i16 dat_008810a0;
    extern i16 dat_008810a4;
    extern i16 dat_008810a8;
    extern i16 dat_008810ac;
    extern i16 dat_008810b0;
    extern i16 dat_008810b4;
    extern i16 dat_008810b8;
    /* Install in extern C data section. */
    char dat_00754f5c[] = "extratoggle";
    char dat_00754f54[] = "MZY419";
    char dat_00754f48[] = "shop_tool1";
    char dat_00754f44[] = "poo";
    char dat_00754f3c[] = "6HUJ56";
    char dat_00754f30[] = "shop_tool2";
    char dat_00754f24[] = "disguises";
    char dat_00754f1c[] = "BRJ437";
    char dat_00754f10[] = "shop_tool3";
    char dat_00754f04[] = "daisychains";
    char dat_00754efc[] = "KAU532";
    char dat_00754ef0[] = "shop_tool4";
    char dat_00754ee4[] = "c3pobits";
    char dat_00754edc[] = "5HD9TY";
    char dat_00754ed0[] = "shop_tool5";
    char dat_00754ec0[] = "towdeathstar";
    char dat_00754eb8[] = "JNK8J4";
    char dat_00754eac[] = "shop_tool6";
    char dat_00754ea0[] = "silhouettes";
    char dat_00754e98[] = "5HD86H";
    char dat_00754e88[] = "shop_silhouette";
    char dat_00754e7c[] = "beepbeep";
    char dat_00754e74[] = "VXZ3K2";
    char dat_00754e64[] = "shop_beepbeep";
    char dat_00754e58[] = "supergonk";
    char dat_00754e50[] = "SPRGNK";
    char dat_00754e40[] = "shop_supergonk";
    char dat_00754e34[] = "poomoney";
    char dat_00754e2c[] = "PMN904";
    char dat_00754e1c[] = "shop_poomoney";
    char dat_00754e08[] = "walkietalkiedisable";
    char dat_00754e00[] = "JKJ589";
    char dat_00754dec[] = "shop_walkietalkie";
    char dat_00754dd8[] = "powerbrickdetector";
    char dat_00754dd0[] = "FBM834";
    char dat_00754dc0[] = "shop_powerbrick";
    char dat_00754db4[] = "superslap";
    char dat_00754dac[] = "294NMB";
    char dat_00754d9c[] = "shop_superslap";
    char dat_00754d90[] = "forcezipup";
    char dat_00754d88[] = "CLZ738";
    char dat_00754d74[] = "shop_forcegrapple";
    char dat_00754d68[] = "coinmagnet";
    char dat_00754d60[] = "VXZ193";
    char dat_00754d50[] = "shop_coinmagnet";
    char dat_00754d40[] = "disarmtroopers";
    char dat_00754d38[] = "HJU848";
    char dat_00754d24[] = "shop_disarmtroopers";
    char dat_00754d14[] = "characterstuds";
    char dat_00754d0c[] = "DHY782";
    char dat_00754cf8[] = "shop_characterstuds";
    char dat_00754ce8[] = "perfectdeflect";
    char dat_00754ce0[] = "PER894";
    char dat_00754ccc[] = "shop_perfectdeflect";
    char dat_00754cb4[] = "explodingblasterbolts";
    char dat_00754cac[] = "EXP912";
    char dat_00754c98[] = "shop_explodingbolts";
    char dat_00754c8c[] = "forcepull";
    char dat_00754c84[] = "PUL966";
    char dat_00754c74[] = "shop_forcepull";
    char dat_00754c60[] = "vehiclesmartbomb";
    char dat_00754c58[] = "SMA293";
    char dat_00754c44[] = "shop_vehiclesmart";
    char dat_00754c34[] = "superastromech";
    char dat_00754c2c[] = "R2D222";
    char dat_00754c18[] = "shop_superastromech";
    char dat_00754c08[] = "superjedislam";
    char dat_00754c00[] = "SLM768";
    char dat_00754bec[] = "shop_superjedislam";
    char dat_00754bd4[] = "superthermaldetonator";
    char dat_00754bcc[] = "THE931";
    char dat_00754bb8[] = "shop_superthermal";
    char dat_00754ba8[] = "deflectbolts";
    char dat_00754ba0[] = "BNL435";
    char dat_00754b8c[] = "shop_deflectbolts";
    char dat_00754b80[] = "darkside";
    char dat_00754b78[] = "DRK328";
    char dat_00754b68[] = "shop_darkside";
    char dat_00754b58[] = "superblasters";
    char dat_00754b50[] = "HHD647";
    char dat_00754b44[] = "shop_tool7";
    char dat_00754b38[] = "fastforce";
    char dat_00754b30[] = "BYY492";
    char dat_00754b24[] = "shop_tool8";
    char dat_00754b18[] = "supersabres";
    char dat_00754b10[] = "GHL978";
    char dat_00754b04[] = "shop_tool9";
    char dat_00754af8[] = "tractorbeam";
    char dat_00754af0[] = "53NKH3";
    char dat_00754ae4[] = "shop_tool10";
    char dat_00754ad4[] = "invincibility";
    char dat_00754acc[] = "HS9K44";
    char dat_00754ac0[] = "shop_tool11";
    char dat_00754ab8[] = "scorex2";
    char dat_00754ab0[] = "HBF899";
    char dat_00754aa4[] = "shop_tool13";
    char dat_00754a94[] = "selfdestruct";
    char dat_00754a8c[] = "DRX444";
    char dat_00754a80[] = "shop_tool12";
    char dat_00754a74[] = "fastbuild";
    char dat_00754a6c[] = "KJU233";
    char dat_00754a60[] = "shop_tool14";
    char dat_00754a58[] = "scorex4";
    char dat_00754a50[] = "DQY857";
    char dat_00754a44[] = "shop_tool15";
    char dat_00754a38[] = "regenerate";
    char dat_00754a30[] = "528HBB";
    char dat_00754a24[] = "shop_tool16";
    char dat_00754a14[] = "minikitdetector";
    char dat_00754a0c[] = "FBM152";
    char dat_00754a00[] = "shop_tool17";
    char dat_007549f8[] = "scorex6";
    char dat_007549f0[] = "NMP499";
    char dat_007549e4[] = "shop_tool18";
    char dat_007549d8[] = "superzapper";
    char dat_007549d0[] = "DFY111";
    char dat_007549c4[] = "shop_tool19";
    char dat_007549bc[] = "rockets";
    char dat_007549b4[] = "BKJ462";
    char dat_007549a8[] = "shop_tool20";
    char dat_007549a0[] = "scorex8";
    char dat_00754998[] = "XXY99G";
    char dat_0075498c[] = "shop_tool21";
    char dat_00754978[] = "superewokcatapult";
    char dat_00754970[] = "ERF893";
    char dat_00754964[] = "shop_tool22";
    char dat_00754950[] = "infinitetorpedos";
    char dat_00754948[] = "NUJ866";
    char dat_0075493c[] = "shop_tool23";
    char dat_00754930[] = "scorex10";
    char dat_00754928[] = "VXZ123";
    char dat_0075491c[] = "shop_tool24";
    char dat_00754908[] = "adaptivedifficulty";
    char dat_00754900[] = "------";
    CHEAT ExtrasCheats[46] = {
        {dat_00754f5c, &dat_00880990, 0, {0, 0}, -1, -1, dat_00754f54, 25000, dat_00754f48, 0x100},
        {dat_00754f44, &dat_0088104c, 0, {0, 0}, -1, -1, dat_00754f3c, 8000, dat_00754f30, 0x0},
        {dat_00754f24, &dat_00880994, 0, {0, 0}, -1, -1, dat_00754f1c, 10000, dat_00754f10, 0x0},
        {dat_00754f04, &dat_00881058, 0, {0, 0}, -1, -1, dat_00754efc, 5000, dat_00754ef0, 0x0},
        {dat_00754ee4, &dat_00881048, 0, {0, 0}, -1, -1, dat_00754edc, 10000, dat_00754ed0, 0x0},
        {dat_00754ec0, &dat_0088105c, 0, {0, 0}, -1, -1, dat_00754eb8, 5000, dat_00754eac, 0x0},
        {dat_00754ea0, &dat_008809d0, 0, {0, 0}, -1, -1, dat_00754e98, 10000, dat_00754e88, 0x1},
        {dat_00754e7c, &dat_00881074, 0, {0, 0}, -1, -1, dat_00754e74, 7500, dat_00754e64, 0x0},
        {dat_00754e58, &dat_00881070, 0, {0, 0}, -1, -1, dat_00754e50, 100000, dat_00754e40, 0x0},
        {dat_00754e34, &dat_0088107c, 0, {0, 0}, -1, -1, dat_00754e2c, 100000, dat_00754e1c, 0x200000},
        {dat_00754e08, &dat_008810a0, 0, {0, 0}, -1, -1, dat_00754e00, 5000, dat_00754dec, 0x0},
        {dat_00754dd8, &dat_00881080, 0, {0, 0}, -1, -1, dat_00754dd0, 125000, dat_00754dc0, 0x40000},
        {dat_00754db4, &dat_00881084, 0, {0, 0}, -1, -1, dat_00754dac, 5000, dat_00754d9c, 0x0},
        {dat_00754d90, &dat_0088108c, 0, {0, 0}, -1, -1, dat_00754d88, 15000, dat_00754d74, 0x0},
        {dat_00754d68, &dat_00881078, 0, {0, 0}, -1, -1, dat_00754d60, 100000, dat_00754d50, 0x238000},
        {dat_00754d40, &dat_00881094, 0, {0, 0}, -1, -1, dat_00754d38, 100000, dat_00754d24, 0x0},
        {dat_00754d14, &dat_008810b8, 0, {0, 0}, -1, -1, dat_00754d0c, 100000, dat_00754cf8, 0x200000},
        {dat_00754ce8, &dat_008810a4, 0, {0, 0}, -1, -1, dat_00754ce0, 20000, dat_00754ccc, 0x100000},
        {dat_00754cb4, &dat_00881098, 0, {0, 0}, -1, -1, dat_00754cac, 20000, dat_00754c98, 0x0},
        {dat_00754c8c, &dat_008810a8, 0, {0, 0}, -1, -1, dat_00754c84, 12000, dat_00754c74, 0x0},
        {dat_00754c60, &dat_0088109c, 0, {0, 0}, -1, -1, dat_00754c58, 15000, dat_00754c44, 0x0},
        {dat_00754c34, &dat_008810ac, 0, {0, 0}, -1, -1, dat_00754c2c, 10000, dat_00754c18, 0x0},
        {dat_00754c08, &dat_008810b4, 0, {0, 0}, -1, -1, dat_00754c00, 11000, dat_00754bec, 0x0},
        {dat_00754bd4, &dat_00881090, 0, {0, 0}, -1, -1, dat_00754bcc, 25000, dat_00754bb8, 0x0},
        {dat_00754ba8, &dat_00881088, 0, {0, 0}, -1, -1, dat_00754ba0, 150000, dat_00754b8c, 0xb0000},
        {dat_00754b80, &dat_008810b0, 0, {0, 0}, -1, -1, dat_00754b78, 25000, dat_00754b68, 0x0},
        {dat_00754b58, &dat_00880998, 0, {0, 0}, -1, -1, dat_00754b50, 15000, dat_00754b44, 0x32002},
        {dat_00754b38, &dat_0088099c, 0, {0, 0}, -1, -1, dat_00754b30, 40000, dat_00754b24, 0x412000},
        {dat_00754b18, &dat_00881044, 0, {0, 0}, -1, -1, dat_00754b10, 40000, dat_00754b04, 0x12400},
        {dat_00754af8, &dat_00881068, 0, {0, 0}, -1, -1, dat_00754af0, 15000, dat_00754ae4, 0x20000},
        {dat_00754ad4, &dat_008809a0, 0, {0, 0}, -1, -1, dat_00754acc, 1000000, dat_00754ac0, 0x32080},
        {dat_00754ab8, &dat_008809a4, 0, {0, 0}, -1, -1, dat_00754ab0, 1250000, dat_00754aa4, 0x230004},
        {dat_00754a94, &dat_00881060, 0, {0, 0}, -1, -1, dat_00754a8c, 25000, dat_00754a80, 0x0},
        {dat_00754a74, &dat_008809a8, 0, {0, 0}, -1, -1, dat_00754a6c, 30000, dat_00754a60, 0x16000},
        {dat_00754a58, &dat_008809ac, 0, {0, 0}, -1, -1, dat_00754a50, 2500000, dat_00754a44, 0x200008},
        {dat_00754a38, &dat_008809b0, 0, {0, 0}, -1, -1, dat_00754a30, 150000, dat_00754a24, 0x33000},
        {dat_00754a14, &dat_008809b4, 0, {0, 0}, -1, -1, dat_00754a0c, 250000, dat_00754a00, 0x200},
        {dat_007549f8, &dat_008809b8, 0, {0, 0}, -1, -1, dat_007549f0, 5000000, dat_007549e4, 0x200010},
        {dat_007549d8, &dat_008809bc, 0, {0, 0}, -1, -1, dat_007549d0, 14000, dat_007549c4, 0x2000},
        {dat_007549bc, &dat_008809c0, 0, {0, 0}, -1, -1, dat_007549b4, 20000, dat_007549a8, 0x2000},
        {dat_007549a0, &dat_008809c4, 0, {0, 0}, -1, -1, dat_00754998, 10000000, dat_0075498c, 0x200020},
        {dat_00754978, &dat_0088106c, 0, {0, 0}, -1, -1, dat_00754970, 25000, dat_00754964, 0x0},
        {dat_00754950, &dat_00881064, 0, {0, 0}, -1, -1, dat_00754948, 25000, dat_0075493c, 0x0},
        {dat_00754930, &dat_008809c8, 0, {0, 0}, -1, -1, dat_00754928, 20000000, dat_0075491c, 0x200040},
        {dat_00754908, &dat_008809cc, 0, {0, 0}, -1, 0, dat_00754900, 0, dat_00754f48, 0x0},
        {0},
    };
}

extern "C" {
    i32 MenuAppliedAspect = 0;
    // {1027, MenuEnterVideoConfirm, MenuDrawVideoConfirm, MenuUpdateVideoConfirm, MenuExitVideoConfirm, -1, -1,
    // 0, 0, 0, 0}, {1028, MenuEnterHiresNotice, MenuDrawHiresNotice, MenuUpdateHiresNotice,
    // MenuExitHiresNotice, -1, -1, 0, 0, 0, 0},

}

extern "C" {
    GAMESAVE TempGame = {0};
}

extern "C" {
    f32 COLLECTION_DX = 0.15f;
    f32 COLLECTION_DY = -0.2f;
}

/* Append inside data.cpp; each original new owner gets its own section. */
extern "C" {
    f32 uprepeattime[2] = {0};
    f32 downrepeattime[2] = {0};
    f32 rightrepeattime[2] = {0};
    u8 rightrepeatcount[2] = {0};
}

extern "C" {
    void (*AIPathCnxHelperSysInitFn)(WORLDINFO *) = 0;
}

extern "C" {
    extern const char dat_00757e34[10] = "pathname=";
    extern const char dat_00757d98[10] = "obstacle=";
    extern const char dat_00757d70[16] = "OBSTACLE_CLOSED";
    extern const char dat_00757db4[9] = "flowbox=";
    extern const char dat_00757de4[9] = "buildit=";
    extern const char dat_00757e18[9] = "bothways";
    extern const char dat_00757e40[10] = "on_frames";
    extern const char dat_00757dc8[14] = "gizmo_output=";
    extern const char dat_00757da4[7] = "force=";
    extern const char dat_00757e10[8] = "on_flag";
    extern const char dat_00757d60[12] = "lastframe-1";
    extern const char dat_00757dac[8] = "blowup=";
    extern const char dat_00757dc0[7] = "gizmo=";
    extern const char dat_00757dfc[5] = "obj=";
    extern const char dat_00757d6c[3] = "to";
    extern const char dat_00757d80[14] = "OBSTACLE_OPEN";
    extern const char dat_00757e04[9] = "off_flag";
    extern const char dat_00757d54[10] = "lastframe";
    extern const char dat_00757dd8[12] = "fakeanimid=";
    extern const char dat_00757d90[7] = "zipup=";
    extern const char dat_00757e24[13] = "CheckVisible";
    extern const char dat_00757df0[10] = "cutscene=";
}

extern "C" {}

extern "C" {}

extern "C" {
    HINT Hints_LSW[65] = {
        {{{600, 0, -1, 0, 7.5f}},     15.0f, {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
        {{{601, 0, -1, 0, 6.0f}},     4.0f,  {GizForce_UpdateHint},     0, {0, 0, 0}, 0.0f},
        {{{602, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
        {{{603, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
        {{{604, 0, -1, 0, 6.0f}},     4.0f,  {GizBuildIts_UpdateHint},  0, {0, 0, 0}, 0.0f},
        {{{605, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
        {{{606, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
        {{{607, 0, -1, 0, 6.0f}},     4.0f,  {GizPanel_UpdateHint},     0, {0, 0, 0}, 0.0f},
        {{{608, 0, -1, 0, 6.0f}},     4.0f,  {GizPanel_UpdateHint},     0, {0, 0, 0}, 0.0f},
        {{{609, 0, -1, 0, 6.0f}},     4.0f,  {GizPanel_UpdateHint},     0, {0, 0, 0}, 0.0f},
        {{{610, 0, -1, 0, 6.0f}},     4.0f,  {ZipUps_UpdateHint},       0, {0, 0, 0}, 0.0f},
        {{{611, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
        {{{612, 0, -1, 0, 6.0f}},     4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{613, 1, -1, 0, 10.0f}},    3.0f,  {Dodge_UpdateHint},        0, {0, 0, 0}, 0.0f},
        {{{614, 1, -1, 0, 10.0f}},    3.0f,  {GizTurrets_UpdateHint},   0, {0, 0, 0}, 0.0f},
        {{{615, 0, -1, 0, 10.0f}},    3.0f,  {Push_UpdateHints},        0, {0, 0, 0}, 0.0f},
        {{{616, 0, -1, 0, 10.0f}},    3.0f,  {Teleport_UpdateHints},    0, {0, 0, 0}, 0.0f},
        {{{617, 0, -1, 0, 10.0f}},    3.0f,  {GizPanel_UpdateHint},     0, {0, 0, 0}, 0.0f},
        {{{618, 0, -1, 0, 10.0f}},    3.0f,  {GizPanel_UpdateHint},     0, {0, 0, 0}, 0.0f},
        {{{620, 4, -1, 0, 6.0f}},     2.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{623, 0, -1, 0, 7.5f}},     0.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{624, 0, -1, 0, 7.5f}},     0.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{625, 0, -1, 0, 7.5f}},     0.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{626, 0, -1, 0, 0.0f}},     0.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{627, 0, -1, 0, 0.0f}},     0.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{628, 0, -1, 0, 0.0f}},     0.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{630, 0, -1, 0, 10.0f}},    3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{631, 0, -1, 0, 10.0f}},    3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{632, 0, -1, 0, 10.0f}},    3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{633, 0, -1, 0, 10.0f}},    3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{634, 0, -1, 0, 10.0f}},    3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{635, 0, -1, 100, 10.0f}},  3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{636, 0, -1, 100, 10.0f}},  3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{637, 0, -1, 100, 10.0f}},  3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{638, 0, -1, 100, 10.0f}},  3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{639, 0, -1, 100, 10.0f}},  3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{640, 0, -1, 100, 10.0f}},  3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{641, 0, -1, 100, 10.0f}},  3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{642, 0, -1, 100, 10.0f}},  3.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{643, 0, -1, 1000, 10.0f}}, 3.0f,  {ShinyMetal_UpdateHint},   0, {0, 0, 0}, 0.0f},
        {{{644, 0, -1, 0, 0.0f}},     0.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{645, 0, -1, 0, 0.0f}},     0.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{646, 0, -1, 0, 0.0f}},     0.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{647, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
        {{{648, 0, -1, 0, 10.0f}},    4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{649, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
        {{{650, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
        {{{651, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
        {{{652, 0, -1, 0, 6.0f}},     4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{653, 0, -1, 0, 5.0f}},     10.0f, {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
        {{{654, 0, -1, 0, 6.0f}},     4.0f,  {Tag_UpdateHint},          0, {0, 0, 0}, 0.0f},
        {{{655, 0, -1, 0, 6.0f}},     4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{656, 0, -1, 0, 6.0f}},     4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{657, 0, -1, 0, 10.0f}},    4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{658, 0, -1, 0, 10.0f}},    4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{659, 0, -1, 0, 10.0f}},    4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{696, 0, -1, 0, 6.0f}},     4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{697, 0, -1, 0, 6.0f}},     4.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
        {{{1675, 1, -1, 0, 8.0f}},    0.0f,  {Shop_UpdateHint},         0, {0, 0, 0}, 0.0f},
        {{{1676, 1, -1, 0, 8.0f}},    0.0f,  {Shop_UpdateHint},         0, {0, 0, 0}, 0.0f},
        {{{1677, 1, -1, 0, 8.0f}},    0.0f,  {Shop_UpdateHint},         0, {0, 0, 0}, 0.0f},
        {{{1678, 1, -1, 0, 8.0f}},    0.0f,  {Shop_UpdateHint},         0, {0, 0, 0}, 0.0f},
        {{{1449, 1, 4, 0, 8.0f}},     0.0f,  {Percent_UpdateHint},      0, {0, 0, 0}, 0.0f},
        {{{1447, 1, 4, 0, 8.0f}},     0.0f,  {IndyUnlocked_UpdateHint}, 0, {0, 0, 0}, 0.0f},
        {{{-1, 0, -1, 1000, 1.0f}},   1.0f,  {NULL},                    0, {0, 0, 0}, 0.0f},
    };
}

extern "C" {}

extern "C" {
    AI_PATH_CONNECTION_TYPE AIPathConnectionTypes[32] = {};
}

extern "C" {
    void xEndState(NUFPAR *parser);
    NUFPCOMJMP AIScriptStateCommands[5] = {
        {"CONDITIONS",      xConditions},
        {"ACTIONS",         xActions   },
        {"REFERENCESCRIPT", xRefScript },
        {"}",               xEndState  },
        {NULL,              NULL       }
    };
}

extern "C" {
    /* DDS image used for textures that fail to resolve (128x128, 0x2080 bytes). */
}

extern "C" {
    NEW_MINI_PIECE NewMiniPiece[10] = {0};
}
