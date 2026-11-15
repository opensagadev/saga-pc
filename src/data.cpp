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

    struct text_crawl_view_s TextCrawlLsw = {
        &TextCrawlChapterId, &TextCrawlVehicleBonusId, &TextCrawlCharacterBonusId, 3, 0, {0, 0}
    };
}

extern "C" {
    extern char dat_007516c0[];
}

extern "C" char dat_007516c0[];

extern "C" {
    extern const u16 bad_fluff[26] = {
        '!', '$', '%', '^', '&', '*', '(', ')', '_', '+', '-', '=', '{', '}', '[', ']', '@', '~', '#', '?', '<', '>', ',', '.', ' ', 0,
    };
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

    PC_DEVICE_BACKEND PCDeviceBackend;
    NuKeyboardOwner nu_keyboard_owner;

    LANGUAGEDATA Text_LanguageList_Default[7] = {
        {1,  apitxt_ENGLISH},
        {2,  apitxt_FRENCH },
        {4,  apitxt_GERMAN },
        {5,  apitxt_ITALIAN},
        {3,  apitxt_SPANISH},
        {8,  apitxt_DANISH },
        {-1, NULL          }
    };

    // The 23-record PC table: IDs, callbacks, remembered cursors, and prompt flags.
}

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

// 513 angular samples including both endpoints.

// Movement, object and AI state recovered from the PC image.

// Additional PC movement, targeting, arcade and callback state.

// PC movement tuning, timers, input masks and parser path.

extern "C" {
    i16 id_TIEFIGHTER = -1;
}

extern "C" {

    f32 player_tag_timer = 0;
    i32 do_player_tag = 0;
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
}

extern "C" {
    PC_DEVICE_STATE_CACHE PCDeviceStateCachePc;
}

extern "C" {
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

extern "C" {
    i16 id_GRABCONTROL = -1;
    i16 id_GRABR2CONTROL = -1;
}

extern "C" {

    u32 LswForceUsedHintFlags = 0;
}

/* Verified DWORD working inputs; original owning extents remain unknown. */

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

/* Append inside data.cpp; each original new owner gets its own section. */
