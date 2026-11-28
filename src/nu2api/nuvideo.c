#include "legoapi/characters/motion.h"
#include "legoapi/gizmo/base/gizmessage.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/levels/levels.h"
#include "nu2api/PCInput.h"
#include "nu2api/nucore/nuerror.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nucore_pc_includes.h"
#include "nu2api/nuerror_gen.h"
#include <stdarg.h>
#include <stdio.h>

void bgProcDataOversize(i32 size) {
}

/* Empty, next to the other empty error hooks; NuFileReadDir in nufile_pc.c keeps its call. */
void FileDirectoryError(i32 error) {
}

GAMECAMERA GameCamera = {0};
GAMECAMERA *GameCam = &GameCamera;
i32 part_debris_count = 0;
