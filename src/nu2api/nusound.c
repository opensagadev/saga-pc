#include "gameapi/edptl.h"
#include "globals.h"
#include "legoapi/audio/audio.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/core/gameliball.h"
#include "nu2api/nucore/nugcutscene.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile/nufpar.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nurand.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numusic/numusic.h"
#include "nu2api/numusic/sfx.h"
#include "nu2api/nusound/nusound.h"
#include "nu2api/nusound/nusound3_include.h"

const char *SfxMusicExtension = ".vag";

/* Sound error report, compiled empty. It lives outside the sound voice code so its callers keep their
 * failure tests and calls; they pass a path, an index, an HRESULT or nothing. */
void PCSoundFailure(const char *format, ...) {
}
