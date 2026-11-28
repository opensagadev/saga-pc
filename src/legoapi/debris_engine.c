#include "gameapi/edbits.h"
#include "gameapi/edptl.h"
#include "gameapi/edtools/edanim_internal.h"
#include "gameapi/edtools/edbri_internal.h"
#include "gameapi/edtools/edgra.h"
#include "gameapi/edtools/edpp_internal.h"
#include "gameapi/edtools/edui.h"
#include "legoapi/characters/motion.h"
#include "legoapi/gizmo/base/gizmessage.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/core/terrain.h"
#include "legoapi/render/fx.h"
#include "legoapi/render/fx/game_deb.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuptl.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nugscn_gen.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/sfx.h"
#include "nu2api/nustencilshadows_PC.h"

#include <stdio.h>
#include <string.h>
#include "legoapi/render/fx/parts.h"
#include "nu2api/numath/numath_includes.h"

/* Original inline sound-event loop, kept private to the debris source group. */

/* SetSfxBit_On/SetSfxBit_OnEx are inlined into the emitter sound loop. */

static void edbricbFileLoad(eduimenu_s *menu, eduiitem_s *unused1, u32 unused2) {
    char path[256];
    edbriMakePath(path);
    edbriBridgesReset();
    if (NuFileExists(path)) {
        edbriLoadPage(path, edbits_base_scene);
    }
    edbriStartAllPages();
    edbriDetermineNearest(1.0f);
    eduiCreateMessageMenu(menu, "Loaded OK", 1);
}
