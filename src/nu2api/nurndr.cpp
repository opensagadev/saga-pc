#include "editor/edlevelall.h"
#include "globals.h"
#include "legoapi/render/fx/game_deb.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/nuprim.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nu3d/nushaderobject.h"
#include "nu2api/nu3d/nuvport.h"
#include "nu2api/nucore/NuSpeedBlurFilter.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nugscn_gen.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numaths.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuplane.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/nushadermtldesc_gen.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nuvport_gen.h"
#include "nu2api/nuwin32/d3dCore.h"

#include <string.h>
#include <float.h>

NUDISPLAYLISTITEM nudlist_2d_static = {0};
NUDISPLAYLISTRENDERSCENE *PCSafeRenderScenes[24] = {0};
struct PC_MATERIAL_ANIM_SET *PCMaterialAnimSets = NULL;
u32 nudlist_field_fe4 = 0;
i32 DisplaySceneRenderDisabled = 0;
i32 noscenespecials = 0;
i32 PCShadowPacketSequence = 0;
f32 PCShadowNearValue = 0;
f32 PCShadowFarValue = 0;

PC_TEXTURE_RESOURCE *RendererVertexDeclaration = 0;
PC_BUFFER_RESOURCE *RendererQuadVertexBuffer = 0;

i32 *NuPortalFrustumCount = 0;
__declspec(align(16)) CAMERA_CLIP_PLANES CameraClipPlanes = {0};
VARIPTR fstack = {0};
__declspec(align(16)) NUPLANE VisibilityDepthPlane = {0};
char *PCDisplayCaptureFlushLimit = NULL;

/* PC 601a30. Minimum reader for this initialization cohort, not a NUMTL extent. */
