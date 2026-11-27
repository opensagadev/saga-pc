#include <stdio.h>
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/screens/gamestatus_lsw.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nuprim.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/nushadermtldesc_gen.h"
#include "nu2api/nustencilshadows_PC.h"

/* Original source name unknown. The native entry is exactly RET; ReleaseTakeOver passes it the rider and
 * vehicle character names. */
void TakeOverReleaseDiagnostic(char *rider_name, char *object_name) {
}

/* Original source name unknown. The native entry is exactly RET; GameAnimSet_AddObject passes it the name of a
 * special whose animation has no frames. */
void GameAnimMissingAnimDiagnostic(char *name) {
}

/* Original source name unknown. The native entry is exactly RET; LoadGizmoSys passes it each gizmo type name it
 * reads, loads or skips. */
void GizmoNameDiagnostic(char *name) {
}
