#include "gameapi/edtools/edcam.h"
#include "gameapi/edtools/edtoolsall.h"
#include "gameapi/edtools/edui.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/render/light/lighting.h"
#include "nu2api/PCInput.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nucore/nucore_plain_types.h"
#include "nu2api/nucore/nukeyboard.h"
#include "nu2api/nucore/numouse.h"
#include "nu2api/nucore/nupad.h"
#include "nu2api/nucore/nutime.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/nuwin32/d3dApiCalls.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include <string.h>

/* PC input startup remains structurally incomplete at the marked platform
 * dependencies; these candidates preserve the verified wrapper ABI/order. */
extern HINSTANCE PCInputWindow;

i32 NuKeyboardPlatformStartup(void) {
    PcConfigureInputGraphicsCaps();
    NuKeyboardCreateWindowState(PCInputInstance, PCInputWindow);
    SetCursor(NULL);
    PCDeviceShowCursor(0);
    PCDeviceSetRenderState(D3DRS_CULLMODE, 1);
    return 1;
}

i32 fun_006d3410(void) {
    return 0;
}
