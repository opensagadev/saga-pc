#include "globals.h"
#include "nu2api/NuEffectTex_pc.h"
#include "nu2api/PCInput.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nupostfilter.h"
#include "nu2api/nu3d/nuptl.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nushader_plain.h"
#include "nu2api/nu3d/nutimebar_plain.h"
#include "nu2api/nucamera_dlist.h"
#include "nu2api/nucore/NuDynamicLight.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nuwin32/d3dApiCalls.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"

extern "C" PC_EFFECT_SURFACE *PcSurfaceRegistry[256];

/* Header-inline: the swap loop's local target expands the destructor. */
inline PC_EFFECT_SURFACE::~PC_EFFECT_SURFACE() {
    Reset();
    PcSurfaceRegistry[registry_index] = NULL;
}
