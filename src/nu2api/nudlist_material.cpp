// Display-list material/transform handlers and post-filter members of the platform range; the 7x7
// high/direct blurs and the Gaussian kernel helpers are in nu3d_includes.cpp.
#include "nu2api/NuEffectTex_pc.h"
#include "nu2api/PCInput.h"
#include "nu2api/nu3d/ShaderManager.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nupostfilter.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nurndr_platform.h"
#include "nu2api/nu3d/nushader.h"
#include "nu2api/nu3d/nushader_plain.h"
#include "nu2api/nu3d/nushadermanager_gen.h"
#include "nu2api/nu3d/nushaderobject.h"
#include "nu2api/nu3d/shaderSemantics.h"
#include "nu2api/nu3d_pc_includes.h"
#include "nu2api/nucamera_dlist.h"
#include "nu2api/nucore/NuMainFilter.h"
#include "nu2api/nucore/NuMainFilterGen.h"
#include "nu2api/nucore/NuPostFilterGen.h"
#include "nu2api/nucore/NuSpeedBlurFilter.h"
#include "nu2api/nucore/NuSpeedBlurFilterGen.h"
#include "nu2api/nugobj.h"
#include "nu2api/nuhgo_dlist.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/numotionaccumfilter_gen.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nustream.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include <math.h>

// NuShaderProgramMakeCurrent and the per-stage sampler binds, inlined here.
extern "C" {
    f32 RenderFrameShaderTime = 0.0f;
    f32 PostBlurDirectOutputBias = 0;
    f32 PostBlurDirectInputBias = 0;
}

/* Header-inline: ??_GNuMainFilter (emitted here with the ctor) expands both. */
inline PC_EFFECT_SURFACE::~PC_EFFECT_SURFACE() {
    Reset();
    PcSurfaceRegistry[registry_index] = NULL;
}
