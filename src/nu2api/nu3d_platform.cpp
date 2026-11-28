// PC platform unit linked after nu3d. The nu3d\pc sources (assert paths name nugscn_dlist.c, numtl_dlist.c,
// nustream.c, nugobj.c, nutex_pc.cpp and nuqfnt_PC.cpp) and the PS/Pc halves of the generic modules form
// one C++ unit; nu3d only ever calls into it, never inlines from it. It holds nugscn_pc, nuportal_pc, numtl
// PS/Pc, the nudlist and nupostfilter platform members, the render-state/stream/scene/gobj/particle backends
// and the geometry backend, plus the platform members of nulgtlaser, nuframe, nuvport, nuprim, nuhgobj,
// nupostdeferred, nurndr, nucamera and nuvec.

#define _CRT_SECURE_NO_DEPRECATE // NOLINT(readability-identifier-naming): VC8 CRT selector.
#include "batman.h"
#include "decomp.h"
#include "editor/edlevelall.h"
#include "gameapi/edtools/edrender.h"
#include "gameapi/edtools/edui.h"
#include "legoapi/characters/motion/move.h"
#include "legoapi/cutscenes/gcutscn.h"
#include "legoapi/items/collect/bolts.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/render/core/gameliball.h"
#include "legoapi/render/fx/game_deb.h"
#include "legoapi/world/levels.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/NuEffectTex_pc.h"
#include "nu2api/PCInput.h"
#include "nu2api/nu3d/ShaderManager.h"
#include "nu2api/nu3d/nu2api_nu3d_types.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nucamera.h"
#include "nu2api/nu3d/nudlist.h"
#include "nu2api/nu3d/nudlist_platform.h"
#include "nu2api/nu3d/nugeom.h"
#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nu3d/nugscn_platform.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nu3d/nulgtlaser.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/nu3d/numtl_platform.h"
#include "nu2api/nu3d/nuportal.h"
#include "nu2api/nu3d/nuportal_platform.h"
#include "nu2api/nu3d/nupostresources.h"
#include "nu2api/nu3d/nuprim.h"
#include "nu2api/nu3d/nuptl.h"
#include "nu2api/nu3d/nuqfnt.h"
#include "nu2api/nu3d/nurndr.h"
#include "nu2api/nu3d/nurndr_platform.h"
#include "nu2api/nu3d/nurndrstat.h"
#include "nu2api/nu3d/nushader.h"
#include "nu2api/nu3d/nushader_plain.h"
#include "nu2api/nu3d/nushadermanager_gen.h"
#include "nu2api/nu3d/nushaderobject.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nu3d/nuvertexformat.h"
#include "nu2api/nu3d/nuvport.h"
#include "nu2api/nu3d/nuwater.h"
#include "nu2api/nu3d_pc_includes.h"
#include "nu2api/nuanimation_gen.h"
#include "nu2api/nucamera_dlist.h"
#include "nu2api/nucore/NuDeferredFilter.h"
#include "nu2api/nucore/NuDeferredFilterGen.h"
#include "nu2api/nucore/NuMainFilter.h"
#include "nu2api/nucore/NuMainFilterGen.h"
#include "nu2api/nucore/NuMotionAccumFilter.h"
#include "nu2api/nucore/NuMotionAccumFilterGen.h"
#include "nu2api/nucore/NuPostFilterGen.h"
#include "nu2api/nucore/NuSpeedBlurFilter.h"
#include "nu2api/nucore/NuSpeedBlurFilterGen.h"
#include "nu2api/nucore/bgproc.h"
#include "nu2api/nucore/nuapi.h"
#include "nu2api/nucore/nuhgobj.h"
#include "nu2api/nucore/numem.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nudir_gen.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nugamma.h"
#include "nu2api/nugobj.h"
#include "nu2api/nugscn_gen.h"
#include "nu2api/nuhgo_dlist.h"
#include "nu2api/nuinstsurfgeom_gen.h"
#include "nu2api/numainfilter_gen.h"
#include "nu2api/numath/nufloat.h"
#include "nu2api/numath/numath_includes.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nutrig.h"
#include "nu2api/numath/nuvec.h"
#include "nu2api/numath/nuvec4.h"
#include "nu2api/numotionaccumfilter_gen.h"
#include "nu2api/nuoctree_gen.h"
#include "nu2api/nuprim_dlist.h"
#include "nu2api/nuptl_dlist.h"
#include "nu2api/nustencilshadows_PC.h"
#include "nu2api/nustream.h"
#include "nu2api/nutex_pc.h"
#include "nu2api/nuwater_PC.h"
#include "nu2api/nuwin32/d3dApiCalls.h"
#include "nu2api/nuwin32/d3dCalls.h"
#include "nu2api/nuwin32/d3dCore.h"
#include "nu2api/nuwin32/pcapi.h"
#include "nu2api/nuwin32/nu2api_windows.h"
#include "nu2api/nucore/nucore_includes.h"
#include "nu2api/pc_fmv_native.h"
#include <float.h>
#include <stdio.h>
#include <string.h>
extern "C" {
    i32 numtl_force_mipmode = 0;
}

extern "C" {

    /* numtl PS half. */

    i32 NuMainRendererControl = 1;

    u8 PCGraphicsRenderTextureActive[32] = {};
    i32 nurndr_tritot_this_frame = 0;
    f32 NuDisplayHandlerOpacity = -1.0f;

    NU_CAMERA_STACK_ENTRY NuCameraStack[64] = {0};

    extern "C" {

        PC_DISPLAY_ITEM_HANDLER PCDisplayItemHandlerPrefix[49] = {
            NuDisplayListMaterial,             /* item type 0x80 */
            NULL,                              /* item type 0x81 */
            NuDisplayListDrawGeometry,         /* item type 0x82 */
            NuDisplayListTransform,            /* item type 0x83 */
            NULL,                              /* item type 0x84 */
            NULL,                              /* item type 0x85 */
            NULL,                              /* item type 0x86 */
            NULL,                              /* item type 0x87 */
            NULL,                              /* item type 0x88 */
            NULL,                              /* item type 0x89 */
            NULL,                              /* item type 0x8a */
            NuDisplayListDrawGeometry,         /* item type 0x8b */
            NuDisplayListTransformTranspose,   /* item type 0x8c */
            NULL,                              /* item type 0x8d */
            NULL,                              /* item type 0x8e */
            NuDisplayListDrawFaceOn,           /* item type 0x8f */
            NuDisplayListFaceOnTransform,      /* item type 0x90 */
            NULL,                              /* item type 0x91 */
            NULL,                              /* item type 0x92 */
            NuDisplayListDrawPrimitive2D,      /* item type 0x93 */
            NuDisplayListLights,               /* item type 0x94 */
            NULL,                              /* item type 0x95 */
            NULL,                              /* item type 0x96 */
            NULL,                              /* item type 0x97 */
            NuDisplayListDrawGeometry,         /* item type 0x98 */
            NuDisplayListSkinPalette,          /* item type 0x99 */
            NuDisplayListCamera,               /* item type 0x9a */
            NULL,                              /* item type 0x9b */
            NULL,                              /* item type 0x9c */
            NULL,                              /* item type 0x9d */
            NULL,                              /* item type 0x9e */
            NULL,                              /* item type 0x9f */
            NULL,                              /* item type 0xa0 */
            NULL,                              /* item type 0xa1 */
            NULL,                              /* item type 0xa2 */
            NULL,                              /* item type 0xa3 */
            NULL,                              /* item type 0xa4 */
            NuDisplayListTint,                 /* item type 0xa5 */
            NuDisplayListFog,                  /* item type 0xa6 */
            NuDisplayListDrawDebris,           /* item type 0xa7 */
            NULL,                              /* item type 0xa8 */
            NuDisplayListVertexGroups,         /* item type 0xa9 */
            NuDisplayListVertexOffsets,        /* item type 0xaa */
            NuDisplayListReflection,           /* item type 0xab */
            NULL,                              /* item type 0xac */
            NuDisplayListPacketAd,             /* item type 0xad */
            NuDisplayListLegacyLightmap,       /* item type 0xae */
            NuDisplayListLegacyLightmapOffset, /* item type 0xaf */
            NuDisplayListLightmapPacket,       /* item type 0xb0 */
        };
    }

    /* PC-only table slot 0xad. */

    /* Font atlas builder: scans a BMP glyph strip for glyph cells and builds a font around it. */

    /* Finds the next glyph cell with the scanner that suits the strip's format. */

    extern "C" {
        PC_EFFECT_SURFACE *PcSurfaceRegistry[256] = {NULL};
        GRAPHICS_RENDER_TEXTURE PCGraphicsRenderTextures[32] = {};
        void *NuDisplayMaterialLast = NULL;
    }

    typedef struct nuprim_transform_packet_s { // size 0x40
        u8 unknown_0000[0x40];                 // 0x00
    } NUPRIM_TRANSFORM_PACKET;

    typedef struct nuprim_camera_packet_s { // size 0x94
        i32 id;                             // 0x00
        NUMTX view;                         // 0x04
        NUMTX projection;                   // 0x44
        f32 viewport[4];                    // 0x84
    } NUPRIM_CAMERA_PACKET;

    // @@FUNCTIONS

    /* Sampler fields packed into GRAPHICS_RENDER_TEXTURE::flags. */
    typedef union PC_SAMPLER_FLAGS { // size 0x4
        u32 value;

        struct {                // size 0x4
            u32 unknown_0 : 8;  // 0x0:0
            u32 address_u : 3;  // 0x0:8
            u32 address_v : 3;  // 0x0:11
            u32 unknown_14 : 3; // 0x0:14
            u32 min_filter : 3; // 0x0:17
            u32 mag_filter : 3; // 0x0:20
            u32 mip_filter : 3; // 0x0:23
            u32 unknown_26 : 6; // 0x0:26
        } bits;
    } PC_SAMPLER_FLAGS;

    void Nu3dPlatformNuError(void) {
    }

    // @@END

} // extern "C"

/* nuhgobj's platform members: the POI lookup and the hierarchy shadow-group packets. */

// extern "C"

/* nurndr's platform members, in nurndr order. */
extern "C" {

    i32 PCFramePresentationState = 0;
    void (*RenderFrameCallback)(void) = NULL;
    u32 RenderFrameSceneFlags = 0;

} // extern "C"

/* nucamera's platform members: NuCameraSet, the extents clip test and the projection matrix. */
// extern "C"

/* nuvec's platform members: the block transforms. */
// extern "C"
