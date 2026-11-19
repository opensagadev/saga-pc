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
static NATIVE_PARTICLE_PACKET *NuDebrisCurrentPacket = 0;
extern "C" {
    DEBRIS_PART_HEADER *NuDebrisLastHeader = 0;
    u32 NuDebrisFrameVertexCount = 0;
    VARIPTR *NuPrimStreamBufferPointer = 0;
    i32 NuPrimVertexCount = 0;
    i32 *NuPrimVertexCountPointer = 0;
    i32 NuPrimCSPos = 0;
    f32 NuPrimXBias = 0;
    f32 NuPrimYBias = 0;
    struct POST_SHADER_PROGRAM *NuDisplayMaterialDebrisPrograms = NULL;
    struct POST_SHADER_PROGRAM *NuDisplayMaterialGlassPrograms = NULL;
    struct POST_SHADER_PROGRAM *NuDisplayMaterialFaceOnDecalPrograms = NULL;
    struct POST_SHADER_PROGRAM *NuDisplayMaterialFaceOnPrograms = NULL;
    struct POST_SHADER_PROGRAM *NuDisplayMaterialDefaultPrograms = NULL;
    PC_TEXTURE_RESOURCE *NuDisplayMaterialDebrisDeclaration = NULL;
    PC_TEXTURE_RESOURCE *NuDisplayMaterialFaceOnDeclaration = NULL;
    PC_TEXTURE_RESOURCE *NuDisplayMaterialDefaultDeclaration = NULL;
    i32 NuDisplayMaterialDistortTexture = 0;
    i32 numtl_force_mipmode = 0;
    i32 NuDisplayHandlerReflection = 0;
    i32 NuLgtStateEnabled = 0;
    i32 NuLgtStateUnknown = 0;
    VARIPTR SceneUploadBufferStartPC = {0};
    VARIPTR SceneUploadBufferCursorPC = {0};
    VARIPTR SceneUploadBufferEndPC = {0};
    i32 NuGobjRegistryInitialized = 0;
    NUGSCN *PCRegisteredNativeScenes[100] = {0};
    PC_GOBJ_REGISTRY_ENTRY *NuGobjRegistryFirst = 0;
    i32 dat_02854f54 = 0;
    i32 NuGobjRegistryCount = 0;
    NUVEC4BLOCK ClipCornerBlocks[2] = {0};
    NUVEC4BLOCK ClipTransformedBlocks[2] = {0};
    FRAME_NATIVE_TEXTURE_POOL FrameNativeTexturePool = {0};
}
static u32 NuDebrisCurrentVertexCount = 0;
static u32 NuDebrisVertexBufferSize = 0;
extern "C" {
    u32 NuDebrisMaxVertexCount = 0;
    NULGTVERTEX *NuDebrisVertexData = 0;
}

static f32 Nu3dPcIncludesVuVecLengthSq(const VuVec &v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

extern "C" {

    // @@PREAMBLE
    struct POST_SHADER_PROGRAM;

    typedef struct SCENE_TEXTURE_PACKET_B0 { // size 0x14
        i32 kind;                            // 0x00
        i32 texture_id;                      // 0x04
        i32 extra_texture_ids[3];            // 0x08
    } SCENE_TEXTURE_PACKET_B0;

    typedef struct SCENE_TEXTURE_PACKET_AE { // size 0xc
        i32 texture_ids[3];                  // 0x0
    } SCENE_TEXTURE_PACKET_AE;

    /* numtl PS half. */

    i32 NuMainRendererControl = 1;

    i32 NuRndrControlState = 0;
    NUMTL *NuDisplayHandlerMaterial = NULL;
    GRAPHICS_RENDER_TEXTURE *PCGraphicsPostEffectBackBufferTexture = 0;

    /* The shadow-volume pass reads the arena globals that nurndr.c defines as pointers through array
     * declarations, so it draws from the pointer variables' own addresses (the pass has no caller). */
    extern NUVEC4 RendererArenaData[];
    extern u32 RendererArenaOffsets[];
    extern i32 RendererArenaTypes[];
    extern u32 RendererArenaCount;

    NUMTX cmtx = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    u8 NuUnresolvedTextureDds[0x2080] = {
        0x44, 0x44, 0x53, 0x20, 0x7c, 0x00, 0x00, 0x00, 0x07, 0x10, 0x08, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x20,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x44, 0x58, 0x54, 0x31,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0xf8, 0x9d, 0xff,
        0x00, 0x00, 0x50, 0x50, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8,
        0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55,
        0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00,
        0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff,
        0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8,
        0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55,
        0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00,
        0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff,
        0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8,
        0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55,
        0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00, 0x55, 0x55, 0x41, 0xf8, 0xbe, 0xff, 0x00, 0x00,
        0x55, 0x55, 0x21, 0xf8, 0x9d, 0xff, 0x00, 0x00, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05,
        0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x08, 0xbf, 0xf7,
        0x55, 0x05, 0x05, 0x05, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08,
        0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00,
        0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00,
        0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef,
        0x55, 0x00, 0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08,
        0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00,
        0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00,
        0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef,
        0x55, 0x00, 0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08,
        0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0xbf, 0x08, 0x9f, 0xef, 0x55, 0x00, 0x00, 0x00, 0x5f, 0x08, 0xff, 0xff, 0x55, 0x58, 0x58, 0x58,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50,
        0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdf, 0x08, 0xbf, 0xf7, 0x05, 0x05, 0x05, 0x05, 0x9f, 0x00, 0xbf, 0x00,
        0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00,
        0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa,
        0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa,
        0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00,
        0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00,
        0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa,
        0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa,
        0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00,
        0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00, 0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x9f, 0x00,
        0xbf, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0x3f, 0x00, 0xff, 0xff, 0x58, 0x58, 0x58, 0x58, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0x10, 0xdf, 0xf7, 0x05, 0x05, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7,
        0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0xdf, 0x08,
        0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55,
        0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00,
        0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7,
        0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0xdf, 0x08,
        0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55,
        0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00,
        0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7,
        0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0xdf, 0x08, 0xbf, 0xf7, 0x00, 0x00, 0x55, 0x55, 0x7f, 0x08,
        0xff, 0xff, 0x58, 0x58, 0x55, 0x55, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05,
        0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8,
        0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x61, 0xf8, 0x00, 0x00, 0xa0, 0x50, 0xde, 0xff, 0x00, 0xf8, 0x00, 0x00, 0x0a, 0x35, 0xff, 0xff, 0x20, 0xf8, 0x00, 0x00,
        0xaa, 0x55, 0xff, 0xff, 0xcb, 0xfa, 0x00, 0x00, 0x02, 0x01, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xa3, 0xf8, 0x00, 0xa0, 0x56, 0x7c, 0xff, 0xff, 0x61, 0xf8, 0x00, 0x02, 0x09, 0x09, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xa2, 0xf8, 0x00, 0x80, 0x58, 0x70, 0xff, 0xff, 0xa3, 0xf8,
        0x00, 0x02, 0x01, 0x01, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8,
        0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x08, 0xfa, 0x60, 0x40, 0x40, 0x40,
        0x21, 0xf8, 0xde, 0xff, 0x58, 0x54, 0x54, 0x54, 0xff, 0xff, 0xa2, 0xf8, 0xde, 0xd8, 0xd8, 0xd8, 0xbe, 0xff, 0x00, 0xf8, 0x00, 0xf8,
        0x58, 0x60, 0x5d, 0xff, 0x00, 0xf8, 0x00, 0x5e, 0xf5, 0x03, 0xbe, 0xff, 0x00, 0xf8, 0x00, 0x03, 0x0d, 0x07, 0xde, 0xff, 0x00, 0xf8,
        0x00, 0xf8, 0x58, 0xc0, 0x9e, 0xff, 0x61, 0xf8, 0x00, 0x73, 0x55, 0x25, 0xa3, 0xf8, 0x9e, 0xff, 0x55, 0x58, 0x50, 0x99, 0xbe, 0xff,
        0x00, 0xf8, 0x00, 0x78, 0xd7, 0x0d, 0xbe, 0xff, 0x00, 0xf8, 0x00, 0x2d, 0x97, 0x70, 0xdf, 0xff, 0x24, 0xf9, 0x00, 0x00, 0xc0, 0x40,
        0x9d, 0xff, 0x61, 0xf8, 0x00, 0x56, 0xf5, 0x09, 0xff, 0xff, 0x62, 0xf8, 0x00, 0x2f, 0x35, 0x36, 0xff, 0xff, 0x00, 0xf8, 0x00, 0x80,
        0x70, 0xd8, 0xbe, 0xff, 0x00, 0xf8, 0x00, 0xd7, 0x7d, 0x80, 0xdf, 0xff, 0x61, 0xf8, 0x00, 0x02, 0x09, 0x05, 0x00, 0xf8, 0xff, 0xff,
        0x95, 0x95, 0x95, 0x95, 0xff, 0xff, 0x41, 0xf8, 0x09, 0x09, 0x09, 0x09, 0xff, 0xff, 0x41, 0xf8, 0x00, 0xe0, 0x70, 0x80, 0xbe, 0xff,
        0x00, 0xf8, 0x00, 0x2f, 0x35, 0x0d, 0x9e, 0xff, 0x20, 0xf8, 0x00, 0xff, 0x57, 0x14, 0xbe, 0xff, 0x00, 0xf8, 0x00, 0x82, 0x72, 0xd8,
        0x9d, 0xff, 0x00, 0xf8, 0x00, 0xd7, 0x7d, 0x00, 0xde, 0xff, 0x00, 0xf8, 0x00, 0x02, 0x09, 0x07, 0x9d, 0xff, 0x21, 0xf8, 0x00, 0x60,
        0x5c, 0x36, 0xbe, 0xff, 0x41, 0xf8, 0xc0, 0xf5, 0x57, 0x70, 0x61, 0xf8, 0xde, 0xff, 0x54, 0x54, 0x54, 0x54, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xaa, 0xfa, 0x40, 0x40, 0x40, 0x80, 0xdf, 0xff, 0x61, 0xf8, 0x01, 0x01, 0x09, 0xf5,
        0xff, 0xff, 0x41, 0xf8, 0xd8, 0xd8, 0x9c, 0x37, 0xdf, 0xff, 0x41, 0xf8, 0x60, 0x60, 0x60, 0x58, 0xa6, 0xf9, 0xff, 0xff, 0x56, 0x56,
        0x56, 0x98, 0x3c, 0xff, 0x41, 0xf8, 0x07, 0x07, 0x07, 0x35, 0xdf, 0xff, 0x00, 0xf8, 0xc0, 0xc0, 0xc0, 0x7c, 0xdf, 0xff, 0x00, 0xf8,
        0x09, 0x01, 0x01, 0xfd, 0xff, 0xff, 0x86, 0xf9, 0x40, 0x40, 0xc0, 0x83, 0x9e, 0xff, 0x21, 0xf8, 0xfd, 0x55, 0x09, 0xb5, 0x9e, 0xff,
        0x61, 0xf8, 0x7f, 0x55, 0x80, 0x5f, 0x9d, 0xff, 0x00, 0xf8, 0x82, 0x02, 0xc0, 0x62, 0xff, 0xff, 0x82, 0xf8, 0x55, 0x7e, 0x02, 0xfd,
        0x7d, 0xff, 0x00, 0xf8, 0x0b, 0x25, 0x14, 0x37, 0x9e, 0xff, 0x61, 0xf8, 0x1c, 0x1c, 0xdc, 0x50, 0xdf, 0xff, 0x86, 0xf9, 0x00, 0x00,
        0x00, 0x7d, 0xff, 0xff, 0x00, 0xf8, 0x26, 0x36, 0x27, 0x0d, 0xff, 0xff, 0xe7, 0xf9, 0xc0, 0xc0, 0xc0, 0x57, 0xff, 0xff, 0x00, 0xf8,
        0x09, 0x09, 0x09, 0xfd, 0xff, 0xff, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x02, 0xff, 0xff, 0x21, 0xf8, 0x27, 0x36, 0xdc, 0x58, 0x61, 0xf8,
        0xbe, 0xff, 0x62, 0x50, 0x58, 0x54, 0x9e, 0xff, 0x82, 0xf8, 0xd4, 0x54, 0x9c, 0x50, 0xff, 0xff, 0x62, 0xf8, 0xff, 0x55, 0x00, 0xff,
        0x7d, 0xff, 0x21, 0xf8, 0x27, 0x25, 0x08, 0x25, 0xdf, 0xff, 0xa2, 0xf8, 0x05, 0x05, 0x27, 0xd6, 0xff, 0xff, 0x82, 0xf8, 0x40, 0xc0,
        0x60, 0x5f, 0xdf, 0xff, 0x21, 0xf8, 0x01, 0x01, 0x01, 0x2d, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff,
        0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0xf8, 0x5e, 0xa0, 0x00, 0x00, 0xde, 0xff, 0x00, 0xf8, 0x0d, 0x00, 0x00, 0x00,
        0x62, 0xf8, 0xdf, 0xff, 0x09, 0x55, 0x55, 0x55, 0xdf, 0xff, 0x00, 0xf8, 0xc9, 0x00, 0x00, 0x00, 0xdf, 0xff, 0x82, 0xf8, 0x95, 0x00,
        0x00, 0x00, 0x82, 0xf8, 0xde, 0xff, 0x01, 0x55, 0x55, 0x55, 0x41, 0xf8, 0xde, 0xff, 0x00, 0x55, 0x55, 0x55, 0x8a, 0xfa, 0xff, 0xff,
        0x54, 0x55, 0x55, 0x55, 0xff, 0xff, 0x00, 0xf8, 0x5e, 0x80, 0x00, 0x00, 0xdf, 0xff, 0x41, 0xf8, 0xd5, 0x02, 0x00, 0x00, 0xff, 0xff,
        0xc7, 0xf9, 0x62, 0x00, 0x00, 0x00, 0xff, 0xff, 0x62, 0xf8, 0x55, 0xa8, 0x00, 0x00, 0xde, 0xff, 0x00, 0xf8, 0x0d, 0x00, 0x00, 0x00,
        0xff, 0xff, 0xec, 0xfa, 0x60, 0x00, 0x00, 0x00, 0xdf, 0xff, 0x41, 0xf8, 0x55, 0x28, 0x00, 0x00, 0xff, 0xff, 0xc7, 0xf9, 0x03, 0x00,
        0x00, 0x00, 0xde, 0xff, 0x21, 0xf8, 0x57, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xde, 0xff, 0x00, 0x55, 0x55, 0x55, 0xa3, 0xf8, 0xff, 0xff,
        0x56, 0x55, 0x55, 0x55, 0xdf, 0xff, 0x20, 0xf8, 0x70, 0x00, 0x00, 0x00, 0xff, 0xff, 0xa2, 0xf8, 0x03, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x49, 0xfa, 0x60, 0x00, 0x00, 0x00, 0xde, 0xff, 0x20, 0xf8, 0x55, 0x28, 0x00, 0x00, 0xff, 0xff, 0xc3, 0xf8, 0x2d, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x04, 0xf9, 0x58, 0x80, 0x00, 0x00, 0xff, 0xff, 0x00, 0xf8, 0xf5, 0x0a, 0x00, 0x00, 0xdf, 0xff, 0x61, 0xf8, 0x25, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff,
        0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbe, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x95,
        0xff, 0xff, 0x5c, 0xff, 0x00, 0x00, 0x00, 0xaa, 0xff, 0xff, 0x5c, 0xff, 0x00, 0x00, 0x00, 0xaa, 0xff, 0xff, 0xbe, 0xff, 0x00, 0x00,
        0x00, 0x02, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x3c, 0xff, 0x00, 0x00, 0x00, 0x70, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05,
        0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x08, 0xfa, 0xc0, 0x40, 0x40, 0x40, 0xa3, 0xf8, 0x9d, 0xff, 0x00, 0x00, 0x14, 0x14,
        0xfe, 0xff, 0xa3, 0xf8, 0x55, 0x55, 0x63, 0x63, 0xff, 0xff, 0x8e, 0xfb, 0x03, 0x01, 0x01, 0x41, 0xff, 0xff, 0x21, 0xf8, 0x00, 0x00,
        0xba, 0x55, 0x82, 0xf8, 0xff, 0xff, 0x55, 0x55, 0x55, 0x58, 0xff, 0xff, 0x41, 0xf8, 0x00, 0x00, 0xa8, 0x56, 0xff, 0xff, 0xc3, 0xf8,
        0x00, 0x00, 0xa0, 0x53, 0xff, 0xff, 0xa3, 0xf8, 0x00, 0x00, 0x0a, 0x05, 0xff, 0xff, 0x62, 0xf8, 0x70, 0x70, 0x58, 0x57, 0xff, 0xff,
        0x00, 0xf8, 0x00, 0x00, 0xaa, 0x55, 0xff, 0xff, 0x41, 0xf8, 0x00, 0x00, 0xa0, 0x5a, 0xff, 0xff, 0x65, 0xf9, 0x00, 0x00, 0x82, 0x41,
        0xff, 0xff, 0x61, 0xf8, 0x00, 0x00, 0x2a, 0x35, 0xff, 0xff, 0x41, 0xf8, 0x00, 0x00, 0xa0, 0x50, 0xff, 0xff, 0x00, 0xf8, 0x00, 0x00,
        0x82, 0x7d, 0xff, 0xff, 0x82, 0xf8, 0x00, 0x00, 0x0b, 0x35, 0xff, 0xff, 0x62, 0xf8, 0x00, 0x00, 0x80, 0x58, 0xff, 0xff, 0x00, 0xf8,
        0x00, 0x00, 0x2b, 0xd5, 0xff, 0xff, 0x34, 0xfd, 0x00, 0x00, 0x00, 0x02, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50,
        0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0xf8,
        0xff, 0xff, 0x95, 0x55, 0x55, 0x55, 0xde, 0xff, 0x20, 0xf8, 0x43, 0x40, 0x40, 0x40, 0xff, 0xff, 0x49, 0xfa, 0x61, 0x01, 0x01, 0x01,
        0xff, 0xff, 0x00, 0xf8, 0x72, 0xd8, 0x58, 0xd8, 0xba, 0xfe, 0x00, 0xf8, 0x8b, 0x00, 0x55, 0xaa, 0x7d, 0xff, 0x00, 0xf8, 0x27, 0x36,
        0x15, 0x2a, 0xff, 0xff, 0x04, 0xf9, 0x5c, 0x60, 0x80, 0x60, 0xfb, 0xfe, 0x21, 0xf8, 0x73, 0xd5, 0x15, 0xdd, 0xa6, 0xf9, 0xdf, 0xff,
        0x58, 0x55, 0x55, 0x55, 0xff, 0xff, 0x41, 0xf8, 0x5e, 0x70, 0x70, 0x70, 0xef, 0xfb, 0xff, 0xff, 0x00, 0x55, 0x55, 0x55, 0x04, 0xf9,
        0xbe, 0xff, 0x25, 0x15, 0x15, 0x15, 0xdf, 0xff, 0x24, 0xf9, 0x81, 0x01, 0x01, 0x01, 0x7d, 0xff, 0x41, 0xf8, 0x37, 0x34, 0x34, 0x34,
        0xff, 0xff, 0xcb, 0xfa, 0x70, 0x80, 0x80, 0x80, 0x20, 0xf8, 0xff, 0xff, 0x80, 0x60, 0x58, 0x58, 0xff, 0xff, 0x45, 0xf9, 0x36, 0x00,
        0x00, 0x00, 0x59, 0xfe, 0x20, 0xf8, 0x94, 0x27, 0x57, 0xa7, 0x59, 0xfe, 0x00, 0xf8, 0x60, 0xc0, 0x55, 0xaa, 0xff, 0xff, 0x00, 0xf8,
        0x03, 0x09, 0x0d, 0x0a, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7d, 0xff,
        0x00, 0xf8, 0x68, 0x56, 0xa8, 0x00, 0xff, 0xff, 0xc3, 0xf8, 0x29, 0x95, 0x2f, 0x00, 0xdf, 0xff, 0x04, 0xf9, 0x50, 0x60, 0x00, 0x00,
        0xbe, 0xff, 0x00, 0xf8, 0x02, 0x55, 0xfe, 0x00, 0xff, 0xff, 0xc3, 0xf8, 0x3e, 0x15, 0x0b, 0x00, 0x20, 0xf8, 0xbe, 0xff, 0x09, 0x02,
        0xa9, 0x55, 0xdf, 0xff, 0x62, 0xf8, 0x72, 0x53, 0xe2, 0x00, 0xff, 0xff, 0xe3, 0xf8, 0x09, 0x25, 0x0f, 0x00, 0xff, 0xff, 0x00, 0xf8,
        0x70, 0x60, 0x80, 0x00, 0xbe, 0xff, 0x00, 0xf8, 0x82, 0x55, 0xbf, 0x00, 0x04, 0xf9, 0xde, 0xff, 0x18, 0x98, 0x55, 0x55, 0x7d, 0xff,
        0x00, 0xf8, 0x81, 0x55, 0xbe, 0x00, 0x9e, 0xff, 0x00, 0xf8, 0x37, 0xd5, 0xaa, 0x00, 0x20, 0xf8, 0x9e, 0xff, 0x95, 0x09, 0xa5, 0x55,
        0xff, 0xff, 0x82, 0xf8, 0xad, 0x55, 0xff, 0x00, 0xdf, 0xff, 0x00, 0xf8, 0x02, 0x09, 0x02, 0x00, 0xff, 0xff, 0x41, 0xf8, 0xb6, 0x5c,
        0xe0, 0x00, 0x9e, 0xff, 0x00, 0xf8, 0x80, 0x55, 0xbf, 0x00, 0xff, 0xff, 0x62, 0xf8, 0x0b, 0x09, 0x02, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05,
        0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8,
        0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xba, 0xfe, 0xff, 0xff, 0x55, 0x55, 0x15, 0x15, 0x04, 0xf9, 0xdf, 0xff, 0x55, 0x55,
        0x00, 0x00, 0xdf, 0xff, 0xc3, 0xf8, 0x00, 0x00, 0xb5, 0x55, 0xf3, 0xfc, 0xff, 0xff, 0x55, 0x55, 0x55, 0x54, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbe, 0xff,
        0x41, 0xf8, 0x00, 0xe0, 0x56, 0x27, 0xff, 0xff, 0x82, 0xf8, 0x00, 0x2f, 0x95, 0x3a, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8,
        0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbe, 0xff, 0x00, 0xf8, 0x1c, 0x1c, 0x9c, 0x5c, 0xff, 0xff, 0x61, 0xf8, 0x60, 0x40,
        0x7a, 0xd5, 0xdf, 0xff, 0x82, 0xf8, 0x01, 0x01, 0x83, 0xc0, 0x1b, 0xff, 0x41, 0xf8, 0xa0, 0x56, 0x25, 0x09, 0x7d, 0xff, 0x20, 0xf8,
        0x0a, 0x35, 0xde, 0x70, 0xdf, 0xff, 0x24, 0xf9, 0x80, 0x50, 0xe0, 0x02, 0xff, 0xff, 0x82, 0xf8, 0xa5, 0x55, 0xf5, 0x07, 0xdf, 0xff,
        0xc3, 0xf8, 0x02, 0x05, 0x0b, 0x00, 0xde, 0xff, 0x20, 0xf8, 0x80, 0x70, 0xdc, 0x36, 0xfb, 0xfe, 0x00, 0xf8, 0x2a, 0x55, 0xe2, 0x80,
        0xff, 0xff, 0x00, 0xf8, 0x00, 0x02, 0x09, 0x0d, 0xff, 0xff, 0x82, 0xf8, 0xa8, 0x57, 0x5e, 0x70, 0x7d, 0xff, 0x61, 0xf8, 0xa0, 0x57,
        0xe5, 0x03, 0xbe, 0xff, 0x82, 0xf8, 0x00, 0x81, 0x61, 0x70, 0x9a, 0xfe, 0x61, 0xf8, 0xa8, 0x55, 0x09, 0x02, 0xbe, 0xff, 0x00, 0xf8,
        0x02, 0x0d, 0x37, 0x9c, 0xdf, 0xff, 0x82, 0xf8, 0xa8, 0x54, 0x58, 0x50, 0x5c, 0xff, 0x41, 0xf8, 0xa8, 0x55, 0xe9, 0x00, 0xde, 0xff,
        0x00, 0xf8, 0x00, 0x03, 0x09, 0x0d, 0xbe, 0xff, 0x00, 0xf8, 0x80, 0x70, 0xdc, 0x36, 0x7d, 0xff, 0x00, 0xf8, 0x2a, 0x55, 0x6a, 0x80,
        0xff, 0xff, 0x41, 0xf8, 0x02, 0x09, 0xc9, 0x69, 0xdb, 0xfe, 0x20, 0xf8, 0xa0, 0x57, 0x2d, 0x03, 0x9d, 0xff, 0x00, 0xf8, 0x02, 0x25,
        0x9e, 0xd8, 0xbe, 0xff, 0xff, 0xff, 0x55, 0x55, 0x55, 0x56, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x92, 0xfc, 0xff, 0xff, 0x55, 0x55, 0x55, 0x95,
        0xff, 0xff, 0x00, 0xf8, 0xdc, 0x1c, 0x9e, 0x55, 0x9d, 0xff, 0x04, 0xf9, 0xd7, 0x50, 0x40, 0x81, 0xa2, 0xf8, 0xdf, 0xff, 0x15, 0x16,
        0x98, 0x60, 0x00, 0xf8, 0x5c, 0xff, 0x00, 0xa8, 0x58, 0x02, 0xdb, 0xfe, 0x00, 0xf8, 0x55, 0xaa, 0xe0, 0x55, 0xff, 0xff, 0xa3, 0xf8,
        0x03, 0x02, 0xa2, 0x52, 0xbe, 0xff, 0x00, 0xf8, 0x07, 0x07, 0xa7, 0x55, 0xdf, 0xff, 0x00, 0xf8, 0x00, 0x00, 0x02, 0x0d, 0x9e, 0xff,
        0x41, 0xf8, 0x56, 0xb6, 0x94, 0x58, 0x21, 0xf8, 0xff, 0xff, 0x00, 0xaa, 0x95, 0x00, 0x61, 0xf8, 0xbe, 0xff, 0x50, 0x5a, 0x5a, 0x50,
        0xff, 0xff, 0x00, 0xf8, 0x70, 0x70, 0x7a, 0x55, 0xff, 0xff, 0x00, 0xf8, 0x00, 0x00, 0x2a, 0xd5, 0xdf, 0xff, 0x41, 0xf8, 0x50, 0x70,
        0x60, 0xc0, 0x00, 0xf8, 0x7d, 0xff, 0x00, 0xaa, 0x56, 0x00, 0x79, 0xfe, 0x20, 0xf8, 0x95, 0x2a, 0x38, 0x95, 0xff, 0xff, 0xc3, 0xf8,
        0x50, 0x50, 0x58, 0x54, 0xbe, 0xff, 0x61, 0xf8, 0x00, 0x00, 0x82, 0x61, 0xbe, 0xff, 0x00, 0xf8, 0x0d, 0x0d, 0x0d, 0x35, 0x9e, 0xff,
        0x41, 0xf8, 0x27, 0x26, 0x94, 0x58, 0xbe, 0xff, 0x00, 0xf8, 0x00, 0x00, 0x80, 0x55, 0xde, 0xff, 0x82, 0xf8, 0x62, 0x60, 0x4d, 0x89,
        0x3c, 0xff, 0x20, 0xf8, 0x55, 0xab, 0x09, 0x55, 0x38, 0xfe, 0x00, 0xf8, 0x55, 0xaa, 0xa0, 0xd5, 0xff, 0xff, 0x7d, 0xff, 0x01, 0x02,
        0x02, 0x02, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff,
        0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0xff, 0xff, 0xff, 0x95, 0x55, 0x55, 0x55, 0x41, 0xf8, 0xff, 0xff, 0xaa, 0x55, 0x55, 0x55,
        0xff, 0xff, 0xe3, 0xf8, 0x02, 0x00, 0x00, 0x00, 0xff, 0xff, 0xcf, 0xfb, 0x27, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc7, 0xf9, 0x78, 0x00,
        0x00, 0x00, 0xff, 0xff, 0xab, 0xfa, 0xb5, 0x00, 0x00, 0x00, 0xff, 0xff, 0xae, 0xfb, 0x70, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff,
        0xaa, 0x55, 0x55, 0x55, 0xff, 0xff, 0x10, 0xfc, 0x0d, 0x00, 0x00, 0x00, 0xff, 0xff, 0x51, 0xfc, 0x60, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x49, 0xfa, 0xd5, 0x00, 0x00, 0x00, 0xff, 0xff, 0x75, 0xfd, 0x09, 0x00, 0x00, 0x00, 0xff, 0xff, 0xef, 0xfb, 0x57, 0x00, 0x00, 0x00,
        0xef, 0xfb, 0xff, 0xff, 0x80, 0x55, 0x55, 0x55, 0xff, 0xff, 0xdf, 0xff, 0x02, 0x00, 0x00, 0x00, 0xff, 0xff, 0xe7, 0xf9, 0x5e, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x0c, 0xfb, 0x2d, 0x00, 0x00, 0x00, 0xff, 0xff, 0xef, 0xfb, 0x5c, 0x00, 0x00, 0x00, 0xff, 0xff, 0x92, 0xfc,
        0x61, 0x00, 0x00, 0x00, 0xff, 0xff, 0x10, 0xfc, 0x35, 0x00, 0x00, 0x00, 0xff, 0xff, 0xae, 0xfb, 0x60, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x29, 0xfa, 0xd5, 0x00, 0x00, 0x00, 0x71, 0xfc, 0xff, 0xff, 0x56, 0x55, 0x55, 0x55, 0xff, 0xff, 0x49, 0xfa, 0x5c, 0x00, 0x00, 0x00,
        0x0c, 0xfb, 0xff, 0xff, 0x60, 0x55, 0x55, 0x55, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff,
        0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05,
        0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50,
        0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe1, 0x27, 0xff, 0xff, 0x55, 0x25, 0x25, 0x25, 0xe1, 0x37, 0xfd, 0xef,
        0x55, 0x00, 0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x37,
        0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00, 0x00, 0x00,
        0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00,
        0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef,
        0x55, 0x00, 0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x37,
        0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00, 0x00, 0x00,
        0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00,
        0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef,
        0x55, 0x00, 0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x37, 0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x37,
        0xfd, 0xef, 0x55, 0x00, 0x00, 0x00, 0xe1, 0x27, 0xff, 0xff, 0x55, 0x58, 0x58, 0x58, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xe0, 0x27, 0xff, 0xff, 0x25, 0x25, 0x25, 0x25, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17,
        0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f,
        0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa,
        0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa,
        0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17,
        0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f,
        0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa,
        0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa,
        0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17,
        0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x3f, 0xe0, 0x17, 0xaa, 0xaa, 0xaa, 0xaa, 0xe0, 0x27,
        0xff, 0xff, 0x58, 0x58, 0x58, 0x58, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05,
        0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe1, 0x27, 0xff, 0xff, 0x25, 0x25,
        0x25, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef,
        0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x37,
        0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55,
        0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00,
        0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef,
        0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x37,
        0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55,
        0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00,
        0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef,
        0x00, 0x00, 0x00, 0x55, 0xe1, 0x37, 0xfd, 0xef, 0x00, 0x00, 0x00, 0x55, 0xe1, 0x27, 0xff, 0xff, 0x58, 0x58, 0x58, 0x55, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8,
        0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8, 0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x41, 0xf8,
        0xbe, 0xff, 0x50, 0x50, 0x50, 0x50, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x05, 0x05, 0x05, 0x05, 0x21, 0xf8, 0x9d, 0xff, 0x50, 0x50, 0x00, 0x00, 0x41, 0xf8,
        0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00,
        0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55,
        0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff,
        0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8,
        0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00,
        0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55,
        0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff,
        0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8,
        0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00,
        0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55,
        0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x41, 0xf8, 0xbe, 0xff, 0x55, 0x55, 0x00, 0x00, 0x21, 0xf8, 0x9d, 0xff,
        0x05, 0x05, 0x00, 0x00,
    };
    u32 RendererStencilFunc = 2;
    u32 RendererStencilPass = 1;
    u32 RendererStencilRef = 0x60;
    u32 PCGraphicsPostEffectWidth = 512;
    u32 PCGraphicsPostEffectHeight = 512;

    static NUMTX RndrStateTransform = {0};
    f32 dat_0280d3a0 = 0;
    PC_GRAPHICS_FRAME_SCRATCH PCGraphicsFrameScratchCount = {0};
    NUMTX PcGeometryResetMatrix = {0};
    PC_BUFFER_RESOURCE *NuDebrisVertexBuffers[4] = {0};
    u32 NuRndrFxColour = 0;
    u8 PCGraphicsRenderTextureActive[32] = {};
    NUVEC4 PcGeometryDistanceOrigin = {0};
    u32 PCFogColour = 0;
    NUMTX PcGeometryProjectionMatrix = {0};
    NUMTX PcGeometryInverseViewProjectionMatrix = {0};
    u32 nudlist_field_1598 = 0;
    i32 dat_0097391c = 0;
    static NUMTX RndrStateScaledTransform = {0};
    i32 nurndr_tritot_this_frame = 0;
    VARIPTR nudlist_stream_cursor = {0};
    __declspec(align(16)) u32 clip_outcode_workspace_prefix[8] = {0};
    DWA_WEIGHT_BLOCK *DwaPointerPool[2048] = {0};
    __declspec(align(16)) NUMTX PcGeometryViewProjectionMatrix = {0};
    f32 dat_0097f990 = 0;
    i32 NuLgtLaserOldCnt = 0;
    NUMTX NuRndr_DebrisMtx = {0};
    i32 PCDeviceSamplerInvalidation[16][2] = {0};
    f32 NuCameraProjectionGlobalScaleY = 0;
    i32 NuPrimCoordSystemStack[2] = {0};
    i32 NuLgtArcLaserOldCnt = 0;
    f32 NuCameraProjectionGlobalOffsetY = 0;
    static f32 RndrStateTransformScale = 1.0f;
    i32 PCGraphicsFrameState = 1;
    f32 NuDisplayHandlerOpacity = -1.0f;
    i32 DwaWeightCapacity = 0x4000;
    i32 DwaPointerCapacity = 0x4000;
    i32 DwaBlockCapacity = 0x2000;
    i32 rndr_blend_shape_deformer_wt_cnt = 61184;
    i32 rndr_blend_shape_deformer_wt_ptrs_cnt = 2048;
    i32 DwaEnabled = 1;
    i32 DwaStreamCapacity = 0x2000;
    f32 NuRndrFxHeight = -10000.0f;
    f32 NuRndrFxPreviousHeight = -10000.0f;
    i32 dat_0080e21c = 1;
    RENDER_POST_VIEWPORT_PREFIX RenderPostViewportPrefix = {0.0f, 0.0f, 640.0f, 224.0f, 0.0f, 0.0f, 640.0f, 224.0f, 1.0f, 1.0f, 0.0f, 0.0f};
    PC_SPECIAL_PROJECTION_ADJUST PCSpecialProjectionAdjust = {1.0f, 1.0f, 0.0f, 0.0f};
    u32 NuDebrisIdealDynamicVBSize = 262144u;
    i32 NuPrimCurrentPrimitiveType = 10000;
    __declspec(align(16)) NUMTX numtx_identity = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                                  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    f32 NuPrimXScale = 1.0f;
    f32 NuPrimYScale = 1.0f;

    static VARIPTR rndrstream_b = {0};
    f32 NuCameraProjectionGlobalOffsetX = 0;
    DWA_WEIGHT_BLOCK DwaWeightPool[60669] = {0};
    static VARIPTR rndrstream_a = {0};
    static i32 nurndr_maxstreamsize = 0;
    static i32 rndrstream_buffid = 0;
    NULGTARCMATERIAL NuLgtArcMtl[4] = {0};
    i32 DwaBlockCount = 0;
    static u32 rndrstream_used = 0;
    u8 NuPortalVisibilityBits[625] = {0};
    NU_CAMERA_STACK_ENTRY NuCameraStack[64] = {0};
    __declspec(align(16)) NUMTX clip_test_mtx = {0};
    PC_SHADOW_QUEUE_RECORD NuRndrShadPolDat[128] = {0};
    static u32 rndrstream_max_used = 0;
    i32 NuQFntScanX = 0;

    typedef struct SCENE_FILE_TEXTURE_HEADER { // size 0x18
        i32 width;                             // 0x00
        u32 height;                            // 0x04
        u32 unknown_08;                        // 0x08
        u32 unknown_0c;                        // 0x0c
        u32 flags;                             // 0x10
        u32 size;                              // 0x14
    } SCENE_FILE_TEXTURE_HEADER;

    typedef struct SCENE_FIXUP_GEOMETRY {          // size 0x34
        u8 unknown_00[8];                          // 0x00
        u16 vertex_stride;                         // 0x08
        u8 unknown_0a[0xe];                        // 0x0a
        u32 vertex_count;                          // 0x18
        u8 unknown_1c[4];                          // 0x1c
        PC_BUFFER_RESOURCE *index_buffer;          // 0x20
        PC_BUFFER_RESOURCE *vertex_buffer;         // 0x24
        u32 vertex_format;                         // 0x28
        u8 unknown_2c[4];                          // 0x2c
        PC_BUFFER_RESOURCE *dynamic_vertex_buffer; // 0x30
    } SCENE_FIXUP_GEOMETRY;

    typedef struct SCENE_FIXUP_VERTEX_STREAM { // size 0xc
        u8 unknown_00[8];                      // 0x0
        PC_BUFFER_RESOURCE *vertex_buffer;     // 0x8
    } SCENE_FIXUP_VERTEX_STREAM;

    typedef struct SCENE_FIXUP_NATIVE {             // size 0x20
        u16 vertex_buffer_count;                    // 0x00
        u8 unknown_02[2];                           // 0x02
        PC_BUFFER_RESOURCE **vertex_buffers;        // 0x04
        u16 index_buffer_count;                     // 0x08
        u8 unknown_0a[2];                           // 0x0a
        PC_BUFFER_RESOURCE **index_buffers;         // 0x0c
        SCENE_FIXUP_GEOMETRY **geometries;          // 0x10
        u32 geometry_count;                         // 0x14
        SCENE_FIXUP_VERTEX_STREAM **vertex_streams; // 0x18
        u32 vertex_stream_count;                    // 0x1c
    } SCENE_FIXUP_NATIVE;

    struct __declspec(align(16)) PC_GEOMETRY_BASIS_VECTOR { // size 0x10

        union { // size 0xc

            struct {   // size 0xc
                f32 x; // 0x0
                f32 y; // 0x4
                f32 z; // 0x8
            };

            NUVEC xyz;
        }; // 0x0

        f32 w; // 0xc
    };

    struct PC_GEOMETRY_BASIS_INPUT_RECORD { // size 0x40
        PC_GEOMETRY_BASIS_VECTOR rows[3];   // 0x00
        NUVEC weights;                      // 0x30
        u8 bones[3];                        // 0x3c
        u8 unknown_3f;                      // 0x3f
    };

    struct PC_GEOMETRY_MORPH_DELTA {       // size 0xc
        u32 unknown_00;                    // 0x0
        u32 unknown_04;                    // 0x4
        PC_GEOMETRY_BASIS_VECTOR *vectors; // 0x8
    };

    struct PC_GEOMETRY_BASIS_SOURCE {            // size 0x1c
        u32 unknown_00;                          // 0x00
        i32 record_count;                        // 0x04
        u32 unknown_08;                          // 0x08
        i32 morph_count;                         // 0x0c
        u32 unknown_10;                          // 0x10
        PC_GEOMETRY_BASIS_INPUT_RECORD *records; // 0x14
        PC_GEOMETRY_MORPH_DELTA *morph_deltas;   // 0x18
    };

    struct PC_GEOMETRY_MORPH_WEIGHTS { // size 0x4
        f32 *weight_words;             // 0x0
    };

    extern "C" i32 PcGeometryMatrixCapacity;

    static void PcGeometryCopyMatrix(NUMTX *dst, const NUMTX *src) {
        dst->m00 = src->m00;
        dst->m01 = src->m01;
        dst->m02 = src->m02;
        dst->m03 = src->m03;
        dst->m10 = src->m10;
        dst->m11 = src->m11;
        dst->m12 = src->m12;
        dst->m13 = src->m13;
        dst->m20 = src->m20;
        dst->m21 = src->m21;
        dst->m22 = src->m22;
        dst->m23 = src->m23;
        dst->m30 = src->m30;
        dst->m31 = src->m31;
        dst->m32 = src->m32;
        dst->m33 = src->m33;
    }

    typedef struct PC_GEOMETRY_FORMAT_RECORD { // size 0x78
        u8 unknown_00[0x18];                   // 0x00
        i32 strip_segments;                    // 0x18
        u8 unknown_1c[0x5c];                   // 0x1c
    } PC_GEOMETRY_FORMAT_RECORD;

    typedef struct PC_GEOMETRY_FORMAT {     // size 0x10
        u32 unknown_00;                     // 0x0
        u32 record_count;                   // 0x4
        u32 unknown_08;                     // 0x8
        PC_GEOMETRY_FORMAT_RECORD *records; // 0xc
    } PC_GEOMETRY_FORMAT;

    typedef struct PC_DYNAMIC_GEOMETRY {         // size 0x20
        PC_GEOMETRY_FORMAT *format;              // 0x00
        i32 element_count;                       // 0x04
        u32 unknown_08;                          // 0x08
        PC_GEOMETRY_SORT_ITEM *sort_items;       // 0x0c
        u32 unknown_10;                          // 0x10
        PC_BUFFER_RESOURCE *vertex_buffer;       // 0x14
        PC_BUFFER_RESOURCE *strip_vertex_buffer; // 0x18
        PC_BUFFER_RESOURCE *strip_index_buffer;  // 0x1c
    } PC_DYNAMIC_GEOMETRY;

    typedef struct PC_GEOMETRY_STRIP_VERTEX { // size 0x10
        f32 fraction;                         // 0x0
        f32 side;                             // 0x4
        u8 unknown_08[8];                     // 0x8
    } PC_GEOMETRY_STRIP_VERTEX;

    typedef struct PC_GEOMETRY_CORNER { // size 0x10
        NUVEC position;                 // 0x0
        f32 w;                          // 0xc
    } PC_GEOMETRY_CORNER;

    typedef struct PC_GEOMETRY_OUTPUT { // size 0x50
        PC_GEOMETRY_CORNER corners[4];  // 0x00
        NUVEC basis_axis;               // 0x40
        f32 distance_squared;           // 0x4c
    } PC_GEOMETRY_OUTPUT;

    typedef struct PC_GEOMETRY_SORT_ITEM { // size 0xc
        u32 output_index;                  // 0x0
        f32 distance_squared;              // 0x4
        u32 unknown_08;                    // 0x8
    } PC_GEOMETRY_SORT_ITEM;

    extern "C" NUVEC4 PcGeometryDistanceOrigin;
    extern "C" i32 dat_00806b94;

    typedef struct PC_GEOMETRY_INPUT { // size 0x70
        PC_GEOMETRY_CORNER corners[4]; // 0x00
        NUVEC basis_weights;           // 0x40
        u32 unknown_4c;                // 0x4c
        f32 output_w[4];               // 0x50
        i32 skin_indices[3];           // 0x60
        u8 skin_weights[3];            // 0x6c
        u8 unknown_6f;                 // 0x6f
    } PC_GEOMETRY_INPUT;

    typedef struct PC_GEOMETRY_GROUP { // size 0xc
        u32 element_count;             // 0x0
        u32 first_element;             // 0x4
        u32 transform_index;           // 0x8
    } PC_GEOMETRY_GROUP;

    typedef struct PC_GEOMETRY_MESH { // size 0x14
        u8 unknown_00[8];             // 0x00
        u32 group_count;              // 0x08
        PC_GEOMETRY_INPUT *elements;  // 0x0c
        PC_GEOMETRY_GROUP *groups;    // 0x10
    } PC_GEOMETRY_MESH;

    typedef struct PC_GEOMETRY_SKIN { // size 0x14
        u8 unknown_00[8];             // 0x00
        u32 matrix_group_count;       // 0x08
        u32 unknown_0c;               // 0x0c
        NUMTX *matrices;              // 0x10
    } PC_GEOMETRY_SKIN;

    typedef struct PC_GEOMETRY_BASIS_MATRIX {
        PC_GEOMETRY_BASIS_VECTOR rows[4];
    } PC_GEOMETRY_BASIS_MATRIX;

    typedef struct PC_GEOMETRY_BASIS {        // size 0xc0
        PC_GEOMETRY_BASIS_MATRIX matrices[3]; // 0x00
    } PC_GEOMETRY_BASIS;

    extern "C" PC_GEOMETRY_BASIS_MATRIX *PcGeometryTransforms;
    extern "C" PC_GEOMETRY_OUTPUT *PcGeometryOutput;

    extern "C" {
        extern const DWORD NuDisplayDebrisVertexShader[76] = {
            0xfffe0101u, 0x0022fffeu, 0x42415443u, 0x0000001cu, 0x00000053u, 0xfffe0101u, 0x00000001u, 0x0000001cu, 0x20000120u,
            0x0000004cu, 0x00000030u, 0x00000002u, 0x00020004u, 0x0000003cu, 0x00000000u, 0x77656976u, 0x6a6f7250u, 0xababab00u,
            0x00030003u, 0x00040004u, 0x00000001u, 0x00000000u, 0x315f7376u, 0x4d00315fu, 0x6f726369u, 0x74666f73u, 0x29522820u,
            0x534c4820u, 0x6853204cu, 0x72656461u, 0x6d6f4320u, 0x656c6970u, 0x2e392072u, 0x392e3931u, 0x312e3934u, 0x00343031u,
            0x00000051u, 0xa00f0004u, 0x3f800000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x0000001fu, 0x80000000u, 0x900f0000u,
            0x0000001fu, 0x8000000au, 0x900f0001u, 0x0000001fu, 0x80000005u, 0x900f0002u, 0x00000009u, 0xc0010000u, 0x90e40000u,
            0xa0e40000u, 0x00000009u, 0xc0020000u, 0x90e40000u, 0xa0e40001u, 0x00000009u, 0xc0040000u, 0x90e40000u, 0xa0e40002u,
            0x00000009u, 0xc0080000u, 0x90e40000u, 0xa0e40003u, 0x00000001u, 0xd00f0000u, 0x90e40001u, 0x00000004u, 0xe00f0000u,
            0x90040002u, 0xa0500004u, 0xa0050004u, 0x0000ffffu,
        };

        extern const DWORD NuDisplayDebrisPixelShader[44] = {
            0xffff0101u, 0x0023fffeu, 0x42415443u, 0x0000001cu, 0x00000053u, 0xffff0101u, 0x00000001u, 0x0000001cu, 0x00000120u,
            0x0000004cu, 0x00000030u, 0x00000003u, 0x00000001u, 0x0000003cu, 0x00000000u, 0x66666964u, 0x5f657375u, 0x00786574u,
            0x000c0004u, 0x00010001u, 0x00000001u, 0x00000000u, 0x315f7370u, 0x4d00315fu, 0x6f726369u, 0x74666f73u, 0x29522820u,
            0x44334420u, 0x53203958u, 0x65646168u, 0x6f432072u, 0x6c69706du, 0x39207265u, 0x2e35312eu, 0x2e393737u, 0x30303030u,
            0xababab00u, 0x00000042u, 0xb00f0000u, 0x00000005u, 0x810f0000u, 0xb0e40000u, 0x90e40000u, 0x0000ffffu,
        };

        extern const DWORD NuDisplayGlassVertexShader[89] = {
            0xfffe0200u, 0x0022fffeu, 0x42415443u, 0x0000001cu, 0x00000053u, 0xfffe0200u, 0x00000001u, 0x0000001cu, 0x20000120u,
            0x0000004cu, 0x00000030u, 0x00000002u, 0x00020004u, 0x0000003cu, 0x00000000u, 0x77656976u, 0x6a6f7250u, 0xababab00u,
            0x00030003u, 0x00040004u, 0x00000001u, 0x00000000u, 0x325f7376u, 0x4d00305fu, 0x6f726369u, 0x74666f73u, 0x29522820u,
            0x534c4820u, 0x6853204cu, 0x72656461u, 0x6d6f4320u, 0x656c6970u, 0x2e392072u, 0x392e3931u, 0x312e3934u, 0x00343031u,
            0x05000051u, 0xa00f0004u, 0x3f000000u, 0xbf000000u, 0x00000000u, 0x00000000u, 0x0200001fu, 0x80000000u, 0x900f0000u,
            0x0200001fu, 0x8000000au, 0x900f0001u, 0x0200001fu, 0x80000005u, 0x900f0002u, 0x03000009u, 0xc0040000u, 0x90e40000u,
            0xa0e40002u, 0x03000009u, 0x80040000u, 0x90e40000u, 0xa0e40003u, 0x02000006u, 0x80080000u, 0x80aa0000u, 0x03000009u,
            0x80010000u, 0x90e40000u, 0xa0e40000u, 0x03000009u, 0x80020000u, 0x90e40000u, 0xa0e40001u, 0x03000005u, 0x80030001u,
            0x80ff0000u, 0x80e40000u, 0x02000001u, 0xc00b0000u, 0x80a40000u, 0x04000004u, 0xe0030000u, 0x80e40001u, 0xa0e40004u,
            0xa0000004u, 0x02000001u, 0xd00f0000u, 0x90e40001u, 0x02000001u, 0xe00c0000u, 0x90440002u, 0x0000ffffu,
        };

        extern const DWORD NuDisplayGlassPixelShader[124] = {
            0xffff0200u, 0x002bfffeu, 0x42415443u, 0x0000001cu, 0x00000077u, 0xffff0200u, 0x00000002u, 0x0000001cu, 0x20000120u,
            0x00000070u, 0x00000044u, 0x00000003u, 0x00020001u, 0x00000054u, 0x00000000u, 0x00000064u, 0x00010003u, 0x00060001u,
            0x00000054u, 0x00000000u, 0x6b636162u, 0x66667542u, 0x745f7265u, 0xab007865u, 0x000c0004u, 0x00010001u, 0x00000001u,
            0x00000000u, 0x74736964u, 0x5f74726fu, 0x00786574u, 0x325f7370u, 0x4d00305fu, 0x6f726369u, 0x74666f73u, 0x29522820u,
            0x534c4820u, 0x6853204cu, 0x72656461u, 0x6d6f4320u, 0x656c6970u, 0x2e392072u, 0x392e3931u, 0x312e3934u, 0x00343031u,
            0x05000051u, 0xa00f0000u, 0xbf000000u, 0xbf000000u, 0x80000000u, 0x3f800000u, 0x05000051u, 0xa00f0001u, 0x3c23d70au,
            0x40000000u, 0x00000000u, 0x00000000u, 0x05000051u, 0xa00f0002u, 0x3f800000u, 0x3f800000u, 0x3f800000u, 0x40000000u,
            0x0200001fu, 0x80000000u, 0x900f0000u, 0x0200001fu, 0x80000000u, 0xb02f0000u, 0x0200001fu, 0x90000000u, 0xa00f0800u,
            0x0200001fu, 0x90000000u, 0xa00f0801u, 0x02000001u, 0x80210000u, 0xb0aa0000u, 0x02000001u, 0x80220000u, 0xb0ff0000u,
            0x03000042u, 0x802f0000u, 0x80e40000u, 0xa0e40801u, 0x03000002u, 0x80270000u, 0x80e40000u, 0xa0e40000u, 0x04000004u,
            0x80230000u, 0x80e40000u, 0xa0000001u, 0xb0e40000u, 0x03000005u, 0x80240000u, 0x80aa0000u, 0xa0ff0000u, 0x03000042u,
            0x802f0001u, 0x80e40000u, 0xa0e40800u, 0x03000005u, 0x802f0001u, 0x80e40001u, 0x90e40000u, 0x02000001u, 0x80280000u,
            0x80aa0000u, 0x02000001u, 0x80270000u, 0xa0550001u, 0x03000005u, 0x802f0000u, 0x80e40001u, 0x80e40000u, 0x03000005u,
            0x802f0000u, 0x80e40000u, 0xa0e40002u, 0x02000001u, 0x802f0800u, 0x80e40000u, 0x0000ffffu,
        };

        extern const DWORD NuDisplayGlassVertexShaderSw[87] = {
            0xfffe0101u, 0x0022fffeu, 0x42415443u, 0x0000001cu, 0x00000053u, 0xfffe0101u, 0x00000001u, 0x0000001cu, 0x20000120u,
            0x0000004cu, 0x00000030u, 0x00000002u, 0x00020004u, 0x0000003cu, 0x00000000u, 0x77656976u, 0x6a6f7250u, 0xababab00u,
            0x00030003u, 0x00040004u, 0x00000001u, 0x00000000u, 0x315f7376u, 0x4d00315fu, 0x6f726369u, 0x74666f73u, 0x29522820u,
            0x534c4820u, 0x6853204cu, 0x72656461u, 0x6d6f4320u, 0x656c6970u, 0x2e392072u, 0x392e3931u, 0x312e3934u, 0x00343031u,
            0x00000051u, 0xa00f0004u, 0x3c23d70au, 0x3f000000u, 0xbf000000u, 0x3f800000u, 0x0000001fu, 0x80000000u, 0x900f0000u,
            0x0000001fu, 0x8000000au, 0x900f0001u, 0x00000009u, 0xc0040000u, 0x90e40000u, 0xa0e40002u, 0x00000009u, 0x80040000u,
            0x90e40000u, 0xa0e40003u, 0x00000006u, 0x80080000u, 0x80aa0000u, 0x00000009u, 0x80010000u, 0x90e40000u, 0xa0e40000u,
            0x00000009u, 0x80020000u, 0x90e40000u, 0xa0e40001u, 0x00000004u, 0x80030001u, 0x80e40000u, 0x80ff0000u, 0xa0000004u,
            0x00000001u, 0xc00b0000u, 0x80a40000u, 0x00000004u, 0xe0030000u, 0x80e40001u, 0xa0e90004u, 0xa0550004u, 0x00000001u,
            0xd00f0000u, 0x90e40001u, 0x00000001u, 0xe00c0000u, 0xa0ff0004u, 0x0000ffffu,
        };

        extern const DWORD NuDisplayGlassPixelShaderSw[54] = {
            0xffff0101u, 0x0024fffeu, 0x42415443u, 0x0000001cu, 0x00000057u, 0xffff0101u, 0x00000001u, 0x0000001cu, 0x00000120u,
            0x00000050u, 0x00000030u, 0x00000003u, 0x00000001u, 0x00000040u, 0x00000000u, 0x6b636162u, 0x66667542u, 0x745f7265u,
            0xab007865u, 0x000c0004u, 0x00010001u, 0x00000001u, 0x00000000u, 0x315f7370u, 0x4d00315fu, 0x6f726369u, 0x74666f73u,
            0x29522820u, 0x44334420u, 0x53203958u, 0x65646168u, 0x6f432072u, 0x6c69706du, 0x39207265u, 0x2e35312eu, 0x2e393737u,
            0x30303030u, 0xababab00u, 0x00000051u, 0xa00f0000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x00000042u,
            0xb00f0000u, 0x00000005u, 0x81070000u, 0xb0e40000u, 0x90e40000u, 0x40000001u, 0x80080000u, 0xa0ff0000u, 0x0000ffffu,
        };

        extern const DWORD NuDisplayFaceOnVertexShader[262] = {
            0xfffe0101u, 0x005bfffeu, 0x42415443u, 0x0000001cu, 0x00000134u, 0xfffe0101u, 0x00000008u, 0x0000001cu, 0x20000120u,
            0x0000012du, 0x000000bcu, 0x00540002u, 0x01520001u, 0x000000c8u, 0x00000000u, 0x000000d8u, 0x00500002u, 0x01420003u,
            0x000000e4u, 0x00000000u, 0x000000f4u, 0x00590002u, 0x01660001u, 0x000000c8u, 0x00000000u, 0x00000106u, 0x00300002u,
            0x00c20001u, 0x000000c8u, 0x00000000u, 0x00000111u, 0x00280002u, 0x00a20001u, 0x000000c8u, 0x00000000u, 0x00000117u,
            0x000c0002u, 0x00320004u, 0x000000e4u, 0x00000000u, 0x0000011cu, 0x00550002u, 0x01560004u, 0x000000e4u, 0x00000000u,
            0x00000124u, 0x00000002u, 0x00020004u, 0x000000e4u, 0x00000000u, 0x65636166u, 0x614d6e4fu, 0xabab0067u, 0x00030001u,
            0x00040001u, 0x00000001u, 0x00000000u, 0x65636166u, 0x744d6e4fu, 0xabab0078u, 0x00030003u, 0x00040004u, 0x00000001u,
            0x00000000u, 0x65636166u, 0x72546e4fu, 0x6c736e61u, 0x6f697461u, 0x6f66006eu, 0x61705f67u, 0x736d6172u, 0x69546b00u,
            0x7600746eu, 0x00776569u, 0x77656976u, 0x0078744du, 0x77656976u, 0x6a6f7250u, 0x5f737600u, 0x00315f31u, 0x7263694du,
            0x666f736fu, 0x52282074u, 0x4c482029u, 0x53204c53u, 0x65646168u, 0x6f432072u, 0x6c69706du, 0x39207265u, 0x2e39312eu,
            0x2e393439u, 0x34303131u, 0xababab00u, 0x00000051u, 0xa00f0004u, 0x3f800000u, 0x00000000u, 0x3fb8aa3bu, 0x40000000u,
            0x00000051u, 0xa00f0005u, 0xbf800000u, 0x3f800000u, 0x00000000u, 0x00000000u, 0x0000001fu, 0x80000000u, 0x900f0000u,
            0x0000001fu, 0x8000000au, 0x900f0001u, 0x0000001fu, 0x80000005u, 0x900f0002u, 0x0000001fu, 0x80010005u, 0x900f0003u,
            0x00000004u, 0x800f0000u, 0x90240000u, 0xa0400004u, 0xa0150004u, 0x00000009u, 0x80010001u, 0x80e40000u, 0xa0e40050u,
            0x00000009u, 0x80020001u, 0x80e40000u, 0xa0e40051u, 0x00000009u, 0x80040001u, 0x80e40000u, 0xa0e40052u, 0x00000002u,
            0x80070000u, 0x80e40001u, 0xa0e40059u, 0x00000005u, 0x80030001u, 0x90e40003u, 0xa0000054u, 0x00000001u, 0x80040001u,
            0xa0000004u, 0x00000008u, 0x80010002u, 0x80e40001u, 0xa0f40055u, 0x00000008u, 0x80020002u, 0x80e40001u, 0xa0f40056u,
            0x00000008u, 0x80040002u, 0x80e40001u, 0xa0f40057u, 0x00000008u, 0x80080001u, 0x80e40001u, 0xa0f40058u, 0x00000002u,
            0x80070001u, 0x80e40000u, 0x80e40002u, 0x00000009u, 0x80010000u, 0x80e40001u, 0xa0e4000fu, 0x00000006u, 0x80010000u,
            0x80000000u, 0x00000009u, 0x80020000u, 0x80e40001u, 0xa0e4000cu, 0x00000009u, 0x80040000u, 0x80e40001u, 0xa0e4000du,
            0x00000009u, 0x80080000u, 0x80e40001u, 0xa0e4000eu, 0x00000005u, 0x80070000u, 0x80000000u, 0x80f90000u, 0x00000008u,
            0x80010000u, 0x80e40000u, 0x80e40000u, 0x00000007u, 0x80010000u, 0x80000000u, 0x00000006u, 0x80010000u, 0x80000000u,
            0x00000005u, 0x80010000u, 0x80000000u, 0xa0ff0030u, 0x00000005u, 0x80010000u, 0x80000000u, 0x80000000u, 0x00000005u,
            0x80010000u, 0x80000000u, 0xa0aa0004u, 0x0000004eu, 0x80040002u, 0x80000000u, 0x00000001u, 0x80010000u, 0x80aa0002u,
            0x00000006u, 0xe0010001u, 0x80000000u, 0x00000009u, 0xc0010000u, 0x80e40001u, 0xa0e40000u, 0x00000009u, 0xc0020000u,
            0x80e40001u, 0xa0e40001u, 0x00000009u, 0xc0040000u, 0x80e40001u, 0xa0e40002u, 0x00000009u, 0xc0080000u, 0x80e40001u,
            0xa0e40003u, 0x00000005u, 0x80080000u, 0x90ff0001u, 0xa0ff0028u, 0x00000001u, 0x80070000u, 0x90e40001u, 0x00000001u,
            0x800b0001u, 0xa0e40004u, 0x00000004u, 0x800f0001u, 0xa0240028u, 0x80400001u, 0x80d50001u, 0x00000005u, 0xd00f0000u,
            0x80e40000u, 0x80e40001u, 0x00000005u, 0xe0030000u, 0x90e40002u, 0xa0e40005u, 0x00000001u, 0xe0060001u, 0xa0550004u,
            0x0000ffffu,
        };

        extern const DWORD NuDisplayFaceOnDecalPixelShader[91] = {
            0xffff0200u, 0x002efffeu, 0x42415443u, 0x0000001cu, 0x00000083u, 0xffff0200u, 0x00000002u, 0x0000001cu, 0x20000120u,
            0x0000007cu, 0x00000044u, 0x00000003u, 0x00020001u, 0x00000050u, 0x00000000u, 0x00000060u, 0x00000002u, 0x00020001u,
            0x0000006cu, 0x00000000u, 0x66666964u, 0x5f657375u, 0x00786574u, 0x000c0004u, 0x00010001u, 0x00000001u, 0x00000000u,
            0x5f676f66u, 0x6f6c6f63u, 0xabab0072u, 0x00030001u, 0x00040001u, 0x00000001u, 0x00000000u, 0x325f7370u, 0x4d00305fu,
            0x6f726369u, 0x74666f73u, 0x29522820u, 0x534c4820u, 0x6853204cu, 0x72656461u, 0x6d6f4320u, 0x656c6970u, 0x2e392072u,
            0x392e3931u, 0x312e3934u, 0x00343031u, 0x05000051u, 0xa00f0001u, 0x3f000000u, 0x3f000000u, 0x3f000000u, 0x3f800000u,
            0x0200001fu, 0x80000000u, 0x900f0000u, 0x0200001fu, 0x80000000u, 0xb0230000u, 0x0200001fu, 0x80000000u, 0xb0210001u,
            0x0200001fu, 0x90000000u, 0xa00f0800u, 0x03000042u, 0x802f0000u, 0xb0e40000u, 0xa0e40800u, 0x04000004u, 0x80270000u,
            0x80e40000u, 0x90e40000u, 0xa1e40000u, 0x03000005u, 0x80280001u, 0x80ff0000u, 0x90ff0000u, 0x04000004u, 0x80270001u,
            0xb0000001u, 0x80e40000u, 0xa0e40000u, 0x02000001u, 0x802f0800u, 0x80e40001u, 0x02000001u, 0x802f0801u, 0xa0e40001u,
            0x0000ffffu,
        };

        extern const DWORD NuDisplayFaceOnPixelShader[76] = {
            0xffff0101u, 0x002ffffeu, 0x42415443u, 0x0000001cu, 0x00000083u, 0xffff0101u, 0x00000002u, 0x0000001cu, 0x00000120u,
            0x0000007cu, 0x00000044u, 0x00000003u, 0x00000001u, 0x00000050u, 0x00000000u, 0x00000060u, 0x00000002u, 0x00020001u,
            0x0000006cu, 0x00000000u, 0x66666964u, 0x5f657375u, 0x00786574u, 0x000c0004u, 0x00010001u, 0x00000001u, 0x00000000u,
            0x5f676f66u, 0x6f6c6f63u, 0xabab0072u, 0x00030001u, 0x00040001u, 0x00000001u, 0x00000000u, 0x315f7370u, 0x4d00315fu,
            0x6f726369u, 0x74666f73u, 0x29522820u, 0x44334420u, 0x53203958u, 0x65646168u, 0x6f432072u, 0x6c69706du, 0x39207265u,
            0x2e35312eu, 0x2e393737u, 0x30303030u, 0xababab00u, 0x00000051u, 0xa00f0001u, 0x3f800000u, 0x00000000u, 0x00000000u,
            0x00000000u, 0x00000042u, 0xb00f0000u, 0x00000040u, 0xb00f0001u, 0x00000005u, 0x800f0000u, 0xb0e40000u, 0x90e40000u,
            0x00000001u, 0x80080000u, 0x80ff0000u, 0x40000008u, 0xb0070000u, 0xa0e40001u, 0xb0e40001u, 0x00000012u, 0x80070000u,
            0xb0e40000u, 0x80e40000u, 0xa0e40000u, 0x0000ffffu,
        };

        extern const DWORD NuDisplayDefaultVertexShader[43] = {
            0xfffe0101u, 0x0016fffeu, 0x42415443u, 0x0000001cu, 0x00000023u, 0xfffe0101u, 0x00000000u, 0x00000000u, 0x20000120u,
            0x0000001cu, 0x315f7376u, 0x4d00315fu, 0x6f726369u, 0x74666f73u, 0x29522820u, 0x534c4820u, 0x6853204cu, 0x72656461u,
            0x6d6f4320u, 0x656c6970u, 0x2e392072u, 0x392e3931u, 0x312e3934u, 0x00343031u, 0x0000001fu, 0x80000000u, 0x900f0000u,
            0x0000001fu, 0x8000000au, 0x900f0001u, 0x0000001fu, 0x80000005u, 0x900f0002u, 0x00000001u, 0xc00f0000u, 0x90e40000u,
            0x00000001u, 0xd00f0000u, 0x90e40001u, 0x00000001u, 0xe0030000u, 0x90e40002u, 0x0000ffffu,
        };

        extern const DWORD NuDisplayDefaultPixelShader[44] = {
            0xffff0101u, 0x0023fffeu, 0x42415443u, 0x0000001cu, 0x00000053u, 0xffff0101u, 0x00000001u, 0x0000001cu, 0x00000120u,
            0x0000004cu, 0x00000030u, 0x00000003u, 0x00000001u, 0x0000003cu, 0x00000000u, 0x66666964u, 0x5f657375u, 0x00786574u,
            0x000c0004u, 0x00010001u, 0x00000001u, 0x00000000u, 0x315f7370u, 0x4d00315fu, 0x6f726369u, 0x74666f73u, 0x29522820u,
            0x44334420u, 0x53203958u, 0x65646168u, 0x6f432072u, 0x6c69706du, 0x39207265u, 0x2e35312eu, 0x2e393737u, 0x30303030u,
            0xababab00u, 0x00000042u, 0xb00f0000u, 0x00000005u, 0x800f0000u, 0xb0e40000u, 0x90e40000u, 0x0000ffffu,
        };

        extern const PC_VERTEX_ELEMENT NuMaterialDefaultElements[4] = {
            {0, 0, 2, 0, 0, 0},
            {0, 12, 4, 0, 10, 0},
            {0, 16, 1, 0, 5, 0},
            {255, 0, 17, 0, 0, 0},
        };

        extern const PC_VERTEX_ELEMENT NuMaterialDebrisElements[4] = {
            {0, 0, 2, 0, 0, 0},
            {0, 12, 4, 0, 10, 0},
            {0, 16, 1, 0, 5, 0},
            {255, 0, 17, 0, 0, 0},
        };

        extern const PC_VERTEX_ELEMENT NuMaterialFaceOnElements[5] = {
            {0, 0, 2, 0, 0, 0}, {0, 12, 4, 0, 10, 0}, {0, 16, 1, 0, 5, 0}, {0, 24, 1, 0, 5, 1}, {255, 0, 17, 0, 0, 0},
        };
        i32 PS2_VREZ_W = 640;
        i32 PS2_VREZ_H = 224;
        i32 dat_0080e6a4 = 1;
        i32 NuLgtStateCapacity = 8;
        i32 NuLgtStateActiveCount = -1;
        i32 NuLgtStateFreeHead = -1;
        i32 NuLgtSeed = 12345;
        i32 SceneUploadBufferSizePC = 41943040;
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
        PC_DISPLAY_ITEM_HANDLER *CurrentItemTable = PCDisplayItemHandlerPrefix;
        i32 PCClipExtentsEnabled = 1;
        NU_CAMERA_STACK_ENTRY *NuCameraStackTop = NuCameraStack;
        __declspec(align(16)) f32 ClipFarLimits[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        i32 NuUnresolvedTextureIDPC = -1;

        extern const u8 NuMaterialDistortImage[824] = {
            0x44, 0x44, 0x53, 0x20, 0x7c, 0x00, 0x00, 0x00, 0x07, 0x10, 0x0a, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
            0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
            0x44, 0x58, 0x54, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x08, 0x10, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0xc0, 0xb3, 0xe0, 0x9d, 0xaa, 0xaf, 0xbf, 0x35, 0xc0, 0xdb, 0x41, 0x93, 0x00, 0xa8, 0xfe, 0x55, 0x82, 0xc3, 0xc1,
            0x89, 0x58, 0x5e, 0x5e, 0xfe, 0xa0, 0x71, 0xa7, 0x53, 0xa0, 0xe8, 0x78, 0x5e, 0x00, 0x5b, 0xea, 0x33, 0xa8, 0xae, 0xbf, 0xd5,
            0x43, 0x33, 0xa2, 0x2c, 0xa0, 0xe0, 0x60, 0x68, 0x41, 0x65, 0x00, 0x2c, 0x35, 0x35, 0xbf, 0xfe, 0x40, 0x8e, 0xc0, 0x74, 0x57,
            0x57, 0xaf, 0x09, 0x61, 0x93, 0xa0, 0x8e, 0x2d, 0x2d, 0x2d, 0x2d, 0xb0, 0xa9, 0x41, 0x7b, 0x5f, 0xff, 0x2f, 0x02, 0xd3, 0xab,
            0x11, 0xaa, 0xfe, 0xaf, 0x2d, 0x25, 0xf7, 0xab, 0x54, 0x44, 0x78, 0x78, 0xe0, 0x80, 0xdb, 0x65, 0x53, 0x44, 0x55, 0xff, 0xab,
            0x03, 0x56, 0x56, 0x4a, 0x34, 0xd5, 0xbf, 0xaa, 0x80, 0xc4, 0x43, 0x4b, 0x3d, 0xad, 0xad, 0x2d, 0x35, 0x40, 0x8f, 0xa2, 0x44,
            0x2d, 0x2d, 0xad, 0xad, 0x45, 0x7b, 0xc0, 0x75, 0x25, 0x2f, 0x2f, 0xaa, 0xd6, 0xc1, 0x71, 0x93, 0x02, 0x2f, 0xb5, 0xb5, 0xfb,
            0xc1, 0x9a, 0x93, 0x78, 0x5c, 0x5e, 0x5e, 0x3b, 0x9c, 0x9a, 0x42, 0x80, 0xa8, 0xfe, 0x57, 0xba, 0x65, 0x1a, 0x63, 0x0b, 0xbd,
            0xd5, 0x55, 0x18, 0x56, 0x5b, 0x54, 0x00, 0xaf, 0xb5, 0xb5, 0x4c, 0x54, 0xb5, 0x4d, 0x2d, 0x35, 0xd5, 0x55, 0x66, 0x64, 0x22,
            0x5e, 0x5c, 0xf8, 0x0a, 0x02, 0xc8, 0x74, 0x01, 0x6b, 0x2b, 0x2d, 0xad, 0xff, 0x37, 0xb3, 0xd4, 0x8c, 0x09, 0xab, 0xff, 0x5f,
            0x3b, 0x9c, 0x3a, 0x62, 0xfa, 0x5e, 0x5c, 0x78, 0x59, 0x4a, 0x7d, 0x29, 0xb8, 0xfe, 0xd7, 0xd7, 0x9b, 0x4a, 0x5e, 0x41, 0x2a,
            0xd7, 0xd7, 0xdf, 0xda, 0x44, 0x7c, 0x42, 0x0b, 0x2d, 0x35, 0xb5, 0x58, 0x45, 0x54, 0x3a, 0x00, 0xa0, 0xf8, 0x5e, 0x29, 0x6c,
            0x46, 0x42, 0x80, 0xfa, 0xff, 0xf5, 0x88, 0x5b, 0x80, 0x32, 0x02, 0x0b, 0x09, 0x0d, 0x1a, 0x85, 0x94, 0x7b, 0x09, 0x89, 0xad,
            0xff, 0xfb, 0x8c, 0xdc, 0x5a, 0x78, 0x7a, 0x7e, 0x7e, 0x3e, 0x43, 0x9e, 0x31, 0x57, 0x5f, 0x7a, 0xe0, 0xbe, 0x5a, 0xdf, 0x31,
            0xd5, 0xa5, 0x0d, 0x0b, 0xbd, 0x72, 0xbb, 0x63, 0x5c, 0xde, 0xbe, 0x28, 0x5d, 0x63, 0xf2, 0x59, 0x5c, 0x78, 0x7a, 0x7a, 0x89,
            0x59, 0xa0, 0x3a, 0x5e, 0x78, 0xf8, 0xfe, 0xc0, 0x59, 0xc6, 0x3b, 0x5e, 0x7a, 0xe0, 0x80, 0xf1, 0x6b, 0x0c, 0x4b, 0x20, 0xe0,
            0x7a, 0x57, 0x59, 0x63, 0xb8, 0x5c, 0x2a, 0xb8, 0xdc, 0x5e, 0x3d, 0x7a, 0xdc, 0x53, 0xb7, 0x95, 0xb5, 0x2d, 0xdc, 0x91, 0x3e,
            0x5a, 0x55, 0xfb, 0xe2, 0xa8, 0xdb, 0x92, 0x5a, 0x6a, 0xa9, 0x0b, 0xab, 0x55, 0xb8, 0x9a, 0x8a, 0x81, 0xe0, 0x60, 0x7a, 0x5f,
            0x23, 0x81, 0xe0, 0x41, 0x7e, 0xf8, 0xa0, 0x80, 0x40, 0x73, 0xc0, 0x71, 0xb5, 0x2d, 0x2b, 0xa8, 0x40, 0x64, 0x29, 0x43, 0x5f,
            0x5f, 0xea, 0x83, 0xf4, 0x64, 0xcf, 0x3b, 0x0d, 0x09, 0x01, 0x8b, 0x94, 0x99, 0x36, 0x74, 0xad, 0xad, 0xad, 0xaf, 0xd9, 0xa1,
            0x37, 0x7a, 0xfa, 0x62, 0x7a, 0x55, 0xb4, 0x82, 0x36, 0x62, 0x55, 0x5f, 0xea, 0xc2, 0x83, 0x81, 0x49, 0x6a, 0x2d, 0x2d, 0x35,
            0x35, 0xa0, 0x81, 0x80, 0x7b, 0x00, 0x00, 0xa8, 0x5c, 0x40, 0xac, 0x00, 0x83, 0x5e, 0xbe, 0x2e, 0x2a, 0xc0, 0xd3, 0x41, 0x6c,
            0x55, 0xff, 0xaa, 0x00, 0x61, 0x92, 0x45, 0x84, 0xd5, 0xad, 0x09, 0x0b, 0xe9, 0x92, 0xc2, 0x69, 0xe0, 0x78, 0x5e, 0xd5, 0xe9,
            0x71, 0x85, 0x63, 0xa8, 0x78, 0x58, 0x5e, 0x2c, 0x7b, 0xe1, 0x3a, 0xe0, 0x78, 0x5e, 0x5f, 0x60, 0x54, 0x61, 0x52, 0x55, 0xf5,
            0x2f, 0x0b, 0x80, 0x84, 0x60, 0x62, 0xad, 0xab, 0xa8, 0x00, 0xa0, 0xa5, 0x86, 0xa2, 0xfa, 0xf8, 0x58, 0x5c, 0x03, 0x92, 0x35,
            0x84, 0x82, 0xe2, 0x7a, 0x5c, 0xb7, 0x55, 0xa6, 0x3b, 0x55, 0xd5, 0xbf, 0x02, 0x00, 0x7e, 0x44, 0x3c, 0xad, 0x2d, 0x27, 0x37,
            0x96, 0xaa, 0xc4, 0x6c, 0x0d, 0x2d, 0xbf, 0xff, 0x7a, 0x93, 0xfb, 0x41, 0x02, 0xe0, 0x5c, 0x5c, 0x59, 0x55, 0xfd, 0x49, 0x02,
            0x2f, 0xb5, 0xb5, 0x90, 0x55, 0xea, 0x52, 0x28, 0xa0, 0xf8, 0x57, 0x57, 0x7c, 0xa2, 0x42, 0x0b, 0x2d, 0xad, 0xf5, 0x5a, 0x74,
            0x1d, 0x3a, 0x5c, 0x78, 0xea, 0xe2, 0xfb, 0x7a, 0xfe, 0x51, 0x25, 0x0d, 0x2f, 0xaf, 0x83, 0x69, 0xd7, 0x62, 0xed, 0xad, 0x0b,
            0x03, 0x00, 0xac, 0xe4, 0x52, 0x75, 0xff, 0xaa, 0x02, 0x68, 0x8a, 0x71, 0x64, 0x35, 0x35, 0x2b, 0x02, 0x17, 0x82, 0x23, 0x53,
            0x80, 0xa0, 0xfe, 0x57, 0xc4, 0x71, 0xe0, 0x6b, 0x00, 0xa2, 0x7a, 0x57, 0xa2, 0xa4, 0x18, 0x72, 0xf8, 0xbc, 0xfe, 0x7a, 0xc4,
            0x5d, 0xb9, 0x42, 0x2f, 0x2a, 0xab, 0xfd, 0xea, 0x7b, 0x53, 0x52, 0x62, 0xeb, 0x8b, 0xc0, 0xb2, 0x71, 0x6a, 0x63, 0xbe, 0x2a,
            0xaa, 0x7f, 0xb1, 0x72, 0x09, 0x5d, 0x7a, 0xe2, 0x0a, 0x0a, 0xd0, 0x63, 0xd0, 0x72, 0x02, 0x56, 0x5e, 0x5e, 0x4f, 0x6b, 0x6f,
            0x6b, 0xaa, 0xab, 0xab, 0xab,
        };
    }

    extern "C" void Nu360MtlInit(void) {
        NuDisplayMaterialDebrisPrograms = PostShaderProgramCreate(2, NuDisplayDebrisVertexShader, NuDisplayDebrisPixelShader);
        if (PCDeviceHardwareVertexProcessingPC > 1) {
            NuDisplayMaterialGlassPrograms = PostShaderProgramCreate(2, NuDisplayGlassVertexShader, NuDisplayGlassPixelShader);
        } else {
            NuDisplayMaterialGlassPrograms = PostShaderProgramCreate(2, NuDisplayGlassVertexShaderSw, NuDisplayGlassPixelShaderSw);
        }
        if (PCDeviceHardwareVertexProcessingPC > 1) {
            NuDisplayMaterialFaceOnDecalPrograms = PostShaderProgramCreate(2, NuDisplayFaceOnVertexShader, NuDisplayFaceOnDecalPixelShader);
        }
        NuDisplayMaterialFaceOnPrograms = PostShaderProgramCreate(2, NuDisplayFaceOnVertexShader, NuDisplayFaceOnPixelShader);
        NuDisplayMaterialDefaultPrograms = PostShaderProgramCreate(2, NuDisplayDefaultVertexShader, NuDisplayDefaultPixelShader);
        PCDeviceCreateVertexDeclaration(NuMaterialFaceOnElements, &NuDisplayMaterialFaceOnDeclaration);
        PCDeviceCreateVertexDeclaration(NuMaterialDebrisElements, &NuDisplayMaterialDebrisDeclaration);
        PCDeviceCreateVertexDeclaration(NuMaterialDefaultElements, &NuDisplayMaterialDefaultDeclaration);
        NUTEXDIM *texture = static_cast<NUTEXDIM *>(NuAlloc(0x28, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\numtl_dlist.c", 0x159));
        memset(texture, 0, sizeof(*texture));
        texture->image_data = const_cast<u8 *>(NuMaterialDistortImage);
        texture->image_size = 0x338;
        NuDisplayMaterialDistortTexture = NuTexCreateNative(texture);
    }

    typedef struct PC_SPECIAL_GEOMETRY {   // size 0x38
        u8 unknown_0000[8];                // 0x00
        u16 vertex_stride;                 // 0x08
        u8 joint_indices[8];               // 0x0a
        u8 unknown_0012[2];                // 0x12
        u32 base_vertex;                   // 0x14
        u32 vertex_count;                  // 0x18
        u8 unknown_001c[8];                // 0x1c
        PC_BUFFER_RESOURCE *vertex_buffer; // 0x24
        u32 morph_count;                   // 0x28
        NUVEC **morph_deltas;              // 0x2c
        u32 unknown_0030;                  // 0x30
        void *dynamic_vertices;            // 0x34
    } PC_SPECIAL_GEOMETRY;

    typedef struct PC_SPECIAL_DEFORMER_WEIGHTS { // size 0xc
        u32 unknown_0000[2];                     // 0x0
        f32 weights[1];                          // 0x8
    } PC_SPECIAL_DEFORMER_WEIGHTS;

    typedef struct PC_SPECIAL_FACEON {
        i32 kind;
    } PC_SPECIAL_FACEON;

    typedef struct PC_SPECIAL_TRANSFORM_PACKET { // size 0x40
        NUMTX matrix;                            // 0x00
    } PC_SPECIAL_TRANSFORM_PACKET;

    typedef struct display_primitive_packet_s { // size 0x38
        u32 type;                               // 0x00
        u8 unknown_0004[0x14];                  // 0x04
        u32 vertex_count;                       // 0x18
        u8 unknown_001c[0x1c];                  // 0x1c
    } DISPLAY_PRIMITIVE_PACKET;

    typedef struct PC_DL_SKIN_PALETTE_PACKET { // size 0x8
        i32 count;                             // 0x0
        f32 elements[1];                       // 0x4
    } PC_DL_SKIN_PALETTE_PACKET;

    typedef struct PC_DL_LIGHTS_PACKET { // size 0xb0
        NUVEC4 ambient;                  // 0x00
        NUVEC4 intensity[3];             // 0x10
        NUVEC4 direction[3];             // 0x40
        NUMTX shine_matrix;              // 0x70
    } PC_DL_LIGHTS_PACKET;

    typedef union PC_DL_NORMALIZED_DIRECTION { // size 0x10
        NUVEC4 four;

        struct {         // size 0x10
            NUVEC three; // 0x0
            f32 w;       // 0xc
        } normalized;
    } PC_DL_NORMALIZED_DIRECTION;

    typedef struct PC_DL_FOG_PACKET { // size 0x14
        i32 enabled;                  // 0x00
        u32 colour;                   // 0x04
        f32 near_distance;            // 0x08
        f32 far_distance;             // 0x0c
        f32 density;                  // 0x10
    } PC_DL_FOG_PACKET;

    typedef struct PC_DL_LEGACY_LIGHTMAP_PACKET { // size 0x14
        i32 texture_id[3];                        // 0x00
        f32 x_offset;                             // 0x0c
        f32 y_offset;                             // 0x10
    } PC_DL_LEGACY_LIGHTMAP_PACKET;

    typedef struct PC_DL_CAMERA_PACKET { // size 0x94
        i32 id;                          // 0x00
        NUMTX view;                      // 0x04
        NUMTX projection;                // 0x44
        f32 viewport[4];                 // 0x84
    } PC_DL_CAMERA_PACKET;

    typedef struct PC_DL_CAMERA_PROJECTION_PACKET { // size 0x50
        NUMTX projection;                           // 0x00
        f32 viewport[4];                            // 0x40
    } PC_DL_CAMERA_PROJECTION_PACKET;

    typedef struct PC_DL_VERTEX_GROUPS_PACKET { // size 0x18
        i32 count;                              // 0x00
        u32 flags;                              // 0x04
        NUVEC4 values[1];                       // 0x08
    } PC_DL_VERTEX_GROUPS_PACKET;

    typedef struct PC_DL_VERTEX_OFFSETS_PACKET { // size 0x14
        i32 count;                               // 0x00
        NUVEC4 values[1];                        // 0x04
    } PC_DL_VERTEX_OFFSETS_PACKET;

    /* PC-only table slot 0xad. */
    typedef struct PC_DL_AD_COLOURS { // size 0x3c
        NUVEC colour[5];              // 0x00
    } PC_DL_AD_COLOURS;

    typedef struct PC_DL_AD_RECORD { // size 0x78
        u8 unknown_0000[0x14];       // 0x00
        u8 enabled;                  // 0x14
        u8 unknown_0015[3];          // 0x15
        i32 segment_count;           // 0x18
        f32 pixel_parameter;         // 0x1c
        f32 vertex_parameter_x;      // 0x20
        f32 vertex_parameter_y;      // 0x24
        f32 vertex_parameter_z;      // 0x28
        f32 pixel_parameter_x;       // 0x2c
        f32 pixel_parameter_y;       // 0x30
        f32 pixel_parameter_z;       // 0x34
        f32 pixel_scalar;            // 0x38
        PC_DL_AD_COLOURS colours;    // 0x3c
    } PC_DL_AD_RECORD;

    typedef struct PC_DL_AD_RECORD_LIST { // size 0x10
        u32 unknown_0000;                 // 0x0
        i32 count;                        // 0x4
        u32 unknown_0008;                 // 0x8
        PC_DL_AD_RECORD *records;         // 0xc
    } PC_DL_AD_RECORD_LIST;

    typedef struct PC_DL_AD_PACKET {         // size 0x20
        PC_DL_AD_RECORD_LIST *list;          // 0x00
        u32 instances;                       // 0x04
        u8 unknown_0008[0xc];                // 0x08
        PC_BUFFER_RESOURCE *instance_buffer; // 0x14
        PC_BUFFER_RESOURCE *vertex_buffer;   // 0x18
        PC_BUFFER_RESOURCE *index_buffer;    // 0x1c
    } PC_DL_AD_PACKET;

    typedef struct PC_DL_GEOMETRY_PACKET {  // size 0x38
        u32 kind;                           // 0x00
        u32 primitive_count;                // 0x04
        u16 vertex_stride;                  // 0x08
        u8 unknown_000a[0xa];               // 0x0a
        i32 base_vertex;                    // 0x14
        u32 vertex_count;                   // 0x18
        u32 start_index;                    // 0x1c
        PC_BUFFER_RESOURCE *index_buffer;   // 0x20
        PC_BUFFER_RESOURCE *vertex_buffer;  // 0x24
        u32 dynamic_count;                  // 0x28
        void *morph_deltas;                 // 0x2c
        PC_BUFFER_RESOURCE *dynamic_buffer; // 0x30
        void *dynamic_vertices;             // 0x34
    } PC_DL_GEOMETRY_PACKET;

    typedef struct PC_DL_FACEON_DRAW_PACKET { // size 0x10
        u32 unknown_0000;                     // 0x0
        u32 primitive_count;                  // 0x4
        PC_BUFFER_RESOURCE *vertex_buffer;    // 0x8
        u32 first_vertex;                     // 0xc
    } PC_DL_FACEON_DRAW_PACKET;

    typedef struct PC_DL_DEBRIS_DRAW_PACKET { // size 0x10
        u8 buffer_index;                      // 0x0
        u8 unknown_0001[3];                   // 0x1
        u32 use_system_buffer;                // 0x4
        u32 first_vertex;                     // 0x8
        u32 vertex_count;                     // 0xc
    } PC_DL_DEBRIS_DRAW_PACKET;

    typedef struct PC_DL_LIGHTMAP_UPDATE_PACKET { // size 0x1c
        i32 mode;                                 // 0x00
        i32 texture_id;                           // 0x04
        i32 texture_ids[3];                       // 0x08
        f32 x_offset;                             // 0x14
        f32 y_offset;                             // 0x18
    } PC_DL_LIGHTMAP_UPDATE_PACKET;

    /* The integer slot holds the 32-bit packet identity. */
    extern i32 PCGraphicsBoundShader;

    /* The leading pad keeps the 14-byte file header's 32-bit fields naturally aligned. */
    typedef struct nutexbmp_file_header_s {
        u16 pad_00;
        u16 type;
        u32 size;
        u16 reserved_08;
        u16 reserved_0a;
        u32 data_offset;
    } NUTEXBMP_FILE_HEADER;

    typedef struct nutexbmp_info_header_s {
        u32 size;
        i32 width;
        i32 height;
        u16 planes;
        u16 bit_count;
        u32 compression;
        u32 image_size;
        i32 x_resolution;
        i32 y_resolution;
        u32 colours_used;
        u32 colours_important;
    } NUTEXBMP_INFO_HEADER;

    typedef struct PC_TEXTURE_LEVEL_COUNT_METHODS {             // size 0x38
        void *reserved00[13];                                   // 0x00
        u32(__stdcall *get_level_count)(PC_TEXTURE_RESOURCE *); // 0x34
    } PC_TEXTURE_LEVEL_COUNT_METHODS;

    extern "C" {}

    /* Font atlas builder: scans a BMP glyph strip for glyph cells and builds a font around it. */

    /* Nonzero when any texel of the 32-bit column at (x, y) has a nonzero alpha. */
    // forceinline: the original inlines this into NuQFntColumnOpaque32, which VC8's own heuristics do not.
    static __forceinline i32 NuQFntColumnOpaque32Inline(NUTEXBMP *bitmap, i32 x, i32 y, i32 count) {
        i32 width = bitmap->width;
        u8 *alpha = bitmap->data + (width * y + x) * 4 + 3;
        i32 i;

        for (i = 0; i < count && y + i < bitmap->height; i++) {
            if (*alpha != 0) {
                return 1;
            }
            alpha += width * 4;
        }
        return 0;
    }

    /* Finds the next glyph cell with the scanner that suits the strip's format. */

    extern "C" {}

    extern "C" {
        PC_EFFECT_SURFACE *PcSurfaceRegistry[256] = {NULL};
        SCENE_VIDEO_RESOURCE_HEADER SceneVideoResourcesPC = {0};
        NUVEC4 NuDisplayHandlerTint = {0};
        i32 PCGraphicsBoundShader = 0;
        NUMTX PcGeometryViewMatrix = {0};
        i32 NuLgtLaserCnt = 0;
        GRAPHICS_RENDER_TEXTURE PCGraphicsRenderTextures[32] = {};
        __declspec(align(16)) NUVEC4 NuRndr_DebrisPlane = {0};
        i32 NuQFntScanY = 0;
        void *NuDisplayMaterialLast = NULL;
        PC_LIGHT_STATE_SLOT NuLgtStateSlots[8] = {0};
        u8 PcSurfaceRegistryInitialized = 0;
        u8 NuPrimNeedsOverbrightening = 0;
        PC_FRAMEBUFFER PcFramebufferPool[32];
        char NuTexBmpErrorText[0x80];
        NUTEXBMP_COLOUR NuTexBmpPalette[0x100];
        NUMTX PcGeometryAdjustedProjectionMatrix = {0};
        NUMTX PcGeometryTranslationMatrix = {0};
        NUMTX PcGeometryScaleMatrix = {0};
        f32 NuDisplayHandlerShadow = 0.0f;
        NURND_VERTEX3D NuRndrShadowVertices[4] = {0};
        u8 PcFramebufferActive[32] = {0};
        NUMTL *PCStartupMaterial2 = NULL;
        PC_FRAMEBUFFER *PcBoundFramebuffer = NULL;
        PC_FRAMEBUFFER *PcDefaultFramebuffer = NULL;
        PC_FRAMEBUFFER *PcSecondaryFramebuffer = NULL;
        PC_TEXTURE_RESOURCE *NuTextureEnvironmentCube = NULL;
        PC_TEXTURE_RESOURCE *NuTexturePhongCube = NULL;
        // NuWaterReset writes these standalone four-byte PC globals.
        i32 nu_water_reset_state = 0;
        i32 nu_water_flow_state = 0;
        i32 nu_water_reset_status = 0;
        PC_BOOT_FRAMEBUFFERS PcBootFramebuffers = {NULL, NULL};
        f32 NuCameraProjectionGlobalScaleX = 0;
        i32 NuLgtArcLaserFrame = 0;
    }

    PC_BUFFER_RESOURCE *NuWaterVertexBuffer = 0;

    typedef struct nuprim_packet_s { // size 0x38
        u32 primitive_type;          // 0x00
        u8 unknown_0004[0x14];       // 0x04
        i32 vertex_count;            // 0x18
        u8 unknown_001c[0x1c];       // 0x1c
    } NUPRIM_PACKET;

    typedef struct nuprim_transform_packet_s { // size 0x40
        u8 unknown_0000[0x40];                 // 0x00
    } NUPRIM_TRANSFORM_PACKET;

    typedef struct nuprim_camera_packet_s { // size 0x94
        i32 id;                             // 0x00
        NUMTX view;                         // 0x04
        NUMTX projection;                   // 0x44
        f32 viewport[4];                    // 0x84
    } NUPRIM_CAMERA_PACKET;

    typedef struct nuprim_vec4_s { // size 0x10
        f32 x;                     // 0x0
        f32 y;                     // 0x4
        f32 z;                     // 0x8
        f32 w;                     // 0xc
    } NUPRIM_VEC4;

    typedef struct nuprim_light_packet_s { // size 0xbc
        NUPRIM_VEC4 ambient_intensity;     // 0x00
        NUPRIM_VEC4 light_intensity[3];    // 0x10
        NUPRIM_VEC4 light_direction[3];    // 0x40
        NUMTX specular_mtx;                // 0x70
        NUVEC specular_colour;             // 0xb0
    } NUPRIM_LIGHT_PACKET;

    // @@FUNCTIONS

    /* The PC backend has no special-transform work: the leaf forwards to the shared empty
     * diagnostic. */
    void DisplayListUpdateSpecialTransformPS(nuhspecial_s *special, NUMTX *matrix) {
        DisplayListSpecialTransformNoOp(special, matrix);
    }

    static void PCSpecialTransposeMatrix(NUMTX *out, NUMTX *matrix) {
        f32 saved;
        out->m00 = matrix->m00;
        saved = matrix->m01;
        out->m01 = matrix->m10;
        out->m10 = saved;
        saved = matrix->m02;
        out->m02 = matrix->m20;
        out->m20 = saved;
        saved = matrix->m03;
        out->m03 = matrix->m30;
        out->m30 = saved;
        out->m11 = matrix->m11;
        saved = matrix->m12;
        out->m12 = matrix->m21;
        out->m21 = saved;
        saved = matrix->m13;
        out->m13 = matrix->m31;
        out->m31 = saved;
        out->m22 = matrix->m22;
        saved = matrix->m23;
        out->m23 = matrix->m32;
        out->m32 = saved;
        out->m33 = matrix->m33;
    }

    static void NuLgtSetVertexColor(i32 colour) {
        i32 converted;
        if (NuPrimNeedsOverbrightening) {
            converted = ((colour & 0xff) << 16) + ((colour >> 16) & 0xff) + (colour & 0xff00) + (colour & 0xff000000);
        } else {
            converted = ((colour & 0xfe) << 15) + ((colour >> 1) & 0x7f00) + ((colour >> 17) & 0x7f) + (colour & 0xff000000);
        }
        NuPrimStreamBufferPointer->vertex_ptr->colour = converted;
    }

    void NuPrimSetVertexUV(f32 u, f32 v) {
        NULGTVERTEX *vertex = NuPrimStreamBufferPointer->vertex_ptr;
        vertex->u = u;
        vertex->v = v;
    }

    i32 PC_DEVICE_BACKEND::GetMultisampleQuality() {
        return quality[multisample_index];
    }

    GRAPHICS_CAPS *PC_DEVICE_BACKEND::GetCaps() {
        return &caps;
    }

    GRAPHICS_PRESENT_PARAMETERS *PC_DEVICE_BACKEND::GetPresentParameters() {
        return &parameters;
    }

    u16 PC_DEVICE_BACKEND::GetTargetCount() {
        return target_count;
    }

    i32 PC_DEVICE_BACKEND::RequiresPow2Textures() {
        return (caps.texture_caps & D3DPTEXTURECAPS_POW2) == 0 && (caps.texture_caps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) == 0;
    }

    void PC_DEVICE_BACKEND::SetScreenColourUvDeclaration() {
        PCDeviceSetVertexDeclaration(screen_colour_uv_declaration);
    }

    static i32 PCGraphicsFilterMode(i32 mode) {
        switch (mode) {
            case 0:
                return 0;
            case 1:
                return 1;
            case 2:
                return 2;
            case 3:
                return 3;
            default:
                return -1;
        }
    }

    static i32 PCGraphicsAddressMode(i32 mode) {
        switch (mode) {
            case 0:
                return 1;
            case 1:
                return 2;
            case 2:
                return 3;
            case 3:
                return 4;
            default:
                return -1;
        }
    }

    static i32 PCGraphicsFormatToD3d(i32 format) {
        switch (format) {
            case PC_GRAPHICS_FORMAT_A8R8G8B8:
                return D3DFMT_A8R8G8B8;
            case PC_GRAPHICS_FORMAT_X8R8G8B8:
                return D3DFMT_X8R8G8B8;
            case PC_GRAPHICS_FORMAT_D24S8:
                return D3DFMT_D24S8;
            case PC_GRAPHICS_FORMAT_D24FS8:
                return D3DFMT_D24FS8;
            case PC_GRAPHICS_FORMAT_R32F:
                return D3DFMT_R32F;
            default:
                return -1;
        }
    }

    static i32 PCGraphicsFormatFromD3d(i32 format) {
        switch (format) {
            case D3DFMT_A8R8G8B8:
            case D3DFMT_X8R8G8B8:
                return PC_GRAPHICS_FORMAT_A8R8G8B8;
            case D3DFMT_D24S8:
                return PC_GRAPHICS_FORMAT_D24S8;
            case D3DFMT_D24FS8:
                return PC_GRAPHICS_FORMAT_D24FS8;
            default:
                return 0;
        }
    }

    void NuEffectTexLockVP(VARIPTR *buffer, VARIPTR end) {
    }

    i32 NuEffectTexUnlockVP(void) {
        return 0;
    }

    i32 fun_005f7ab0(void) {
        return 0;
    }

    i32 fun_005f7ac0(void) {
        return 0;
    }

    i32 fun_005f7ad0(void) {
        return 0;
    }

    i32 fun_005f7ae0(void) {
        return 0;
    }

    void PCGraphicsDestroyRenderTexture(GRAPHICS_RENDER_TEXTURE *texture) {
        i32 i;
        i32 memory_delta;
        f32 multiplier;
        if (texture == NULL) {
            return;
        }
        for (i = 0; i != 32; ++i) {
            if (&PCGraphicsRenderTextures[i] == texture) {
                if (texture->external_id == -1) {
                    if (PCGraphicsRenderTextures[i].resource != NULL) {
                        PCGraphicsRenderTextures[i].resource->methods->release(PCGraphicsRenderTextures[i].resource);
                        PCGraphicsRenderTextures[i].resource = NULL;
                    }
                    if (PCGraphicsRenderTextures[i].surface != NULL) {
                        PCGraphicsRenderTextures[i].surface->methods->release(PCGraphicsRenderTextures[i].surface);
                        PCGraphicsRenderTextures[i].surface = NULL;
                    }
                }
                multiplier = texture->mip_count != 1 ? 1.5f : 1.0f;
                memory_delta = (i32)((0u - (u32)texture->width * (u32)texture->height) * 4u);
                PCGraphicsRenderTextureBytes += (u32)(memory_delta * multiplier);
                memset(&PCGraphicsRenderTextures[i], 0, sizeof(GRAPHICS_RENDER_TEXTURE));
                PCGraphicsRenderTextureActive[i] = 0;
                return;
            }
        }
    }

    i32 PCGraphicsEffectTextureWidth(GRAPHICS_RENDER_TEXTURE *texture) {
        return texture->width;
    }

    i32 PCGraphicsEffectTextureHeight(GRAPHICS_RENDER_TEXTURE *texture) {
        return texture->height;
    }

    void PCGraphicsEffectTextureDimensions(GRAPHICS_RENDER_TEXTURE *texture, u16 level, i32 *width, i32 *height) {
        *width = (texture->width >> level) & ~1;
        *height = (texture->height >> level) & ~1;
    }

    i32 PCGraphicsEffectTextureMipCount(GRAPHICS_RENDER_TEXTURE *texture) {
        return texture->mip_count;
    }

    u32 PCGraphicsTextureGetType(GRAPHICS_RENDER_TEXTURE *texture) {
        return texture->texture_type;
    }

    i32 PCGraphicsTextureGetFormat(GRAPHICS_RENDER_TEXTURE *texture) {
        return texture->format;
    }

    void PCGraphicsEffectTextureSetLowNibbles(GRAPHICS_RENDER_TEXTURE *texture, i32 low, i32 high) {
        texture->flags = (texture->flags & ~0xffu) | (((u32)high & 0xfu) << 4) | ((u32)low & 0xfu);
    }

    void PCGraphicsEffectTextureSetSamplerExtra(GRAPHICS_RENDER_TEXTURE *texture, i32 value) {
        texture->sampler_extra = (u32)value;
    }

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

    void PCGraphicsEffectTextureSetFilterFields(GRAPHICS_RENDER_TEXTURE *texture, i32 first, i32 second, i32 third) {
        texture->min_filter = PCGraphicsFilterMode(first);
        texture->mag_filter = PCGraphicsFilterMode(second);
        texture->mip_filter = PCGraphicsFilterMode(third);
    }

    void PCGraphicsTextureSetAddressU(GRAPHICS_RENDER_TEXTURE *texture, i32 mode) {
        texture->address_u = PCGraphicsAddressMode(mode);
    }

    void PCGraphicsEffectTextureSetWrapFields(GRAPHICS_RENDER_TEXTURE *texture, i32 first, i32 second) {
        texture->address_u = PCGraphicsAddressMode(first);
        texture->address_v = PCGraphicsAddressMode(second);
    }

    void PCGraphicsTextureSetAddressModes(GRAPHICS_RENDER_TEXTURE *texture, i32 u, i32 v, i32 w) {
        texture->address_u = PCGraphicsAddressMode(u);
        texture->address_v = PCGraphicsAddressMode(v);
        texture->address_w = PCGraphicsAddressMode(w);
    }

    GRAPHICS_RENDER_TEXTURE *PCGraphicsAllocRenderTextureSlot(void) {
        i32 i;
        for (i = 0; i != 32; ++i) {
            if (!PCGraphicsRenderTextureActive[i]) {
                PCGraphicsRenderTextureActive[i] = 1;
                return &PCGraphicsRenderTextures[i];
            }
        }
        return NULL;
    }

    void PCGraphicsResetRenderTexture(GRAPHICS_RENDER_TEXTURE *texture) {
        i32 memory_delta;
        f32 multiplier;
        multiplier = texture->mip_count != 1 ? 1.5f : 1.0f;
        memory_delta = (i32)((0u - (u32)texture->width * (u32)texture->height) * 4u);
        PCGraphicsRenderTextureBytes += (u32)(memory_delta * multiplier);
        if (texture->external_id == -1 && texture->resource != NULL) {
            texture->resource->methods->release(texture->resource);
            texture->resource = NULL;
        }
        texture->width = 0;
        texture->height = 0;
        texture->depth = 0;
        texture->mip_count = 0;
        texture->original_mip_count = 0;
        texture->texture_type = 1;
        texture->format = 0;
        texture->usage = 0;
        texture->pool = 0;
        texture->resource = NULL;
        texture->external_id = -1;
        texture->sampler_extra = 0;
        texture->flags = 0x012449d0;
        if (texture->surface != NULL) {
            texture->surface->methods->release(texture->surface);
            texture->surface = NULL;
        }
    }

    void fun_005f81e0(void) {
    }

    void fun_005f8380(void) {
    }

    void fun_005f8390(void) {
    }

    void PCGraphicsRenderTextureFromTexture(GRAPHICS_RENDER_TEXTURE *texture, i32 id) {
        PC_TEXTURE_LEVEL_DESC description;
        texture->resource = NuTextureGetDimensions(id)->resource;
        if (texture->resource == NULL) {
            memset(texture, 0, sizeof(*texture));
            return;
        }
        texture->resource->methods->get_level_desc(texture->resource, 0, &description);
        texture->width = description.width;
        texture->height = description.height;
        texture->depth = 0;
        texture->original_mip_count = texture->mip_count = texture->resource->methods->get_level_count(texture->resource);
        texture->texture_type = 1;
        texture->format = PCGraphicsFormatFromD3d(description.format);
        texture->usage = description.usage & 1;
        texture->pool = description.pool;
        texture->external_id = id;
        texture->sampler_extra = 0;
        texture->flags = 0x012449d0;
    }

    i32 PCGraphicsTextureGetExternalId(GRAPHICS_RENDER_TEXTURE *texture) {
        return texture->external_id;
    }

    // noinline: the original calls PCGraphicsInitRenderTexture out of line from PCGraphicsCreatePostEffectTextures; VC8
    // would inline it there.
    __declspec(noinline) void PCGraphicsInitRenderTexture(GRAPHICS_RENDER_TEXTURE *texture, i32 width, i32 height, i32 levels, i32 usage,
                                                          i32 pool, i32 format) {
        u32 d3d_usage;
        i32 memory_delta;
        f32 multiplier;
        texture->width = (u16)width;
        texture->height = (u16)height;
        texture->mip_count = (u16)levels;
        texture->original_mip_count = (u16)levels;
        texture->depth = 0;
        texture->texture_type = 1;
        texture->format = format;
        texture->usage = usage;
        texture->pool = pool;
        texture->external_id = -1;
        texture->surface = NULL;
        texture->sampler_extra = 0;
        texture->flags = 0x012449d0;
        if (format == PC_GRAPHICS_FORMAT_D24S8) {
            texture->pool = 0;
            texture->mip_count = 1;
            PCDeviceCreateDepthSurface(texture->width, texture->height, PCGraphicsFormatToD3d(format), PCDeviceBackend.GetMultisampleType(),
                                       PCDeviceBackend.quality[PCDeviceBackend.multisample_index], FALSE, &texture->surface, NULL);
            PCGraphicsRenderTextureBytes += (i32)((u32)texture->width * texture->height * 4u);
            return;
        }
        d3d_usage = 0;
        if (texture->usage == 1) {
            d3d_usage = format == 5 || format == 6 || format == 7 ? 2 : 1;
        }
        PCDeviceCreateTexture(texture->width, texture->height, texture->mip_count, d3d_usage, PCGraphicsFormatToD3d(format), texture->pool,
                              &texture->resource);
        if (texture->resource != NULL) {
            texture->mip_count =
                (u16)((const PC_TEXTURE_LEVEL_COUNT_METHODS *)texture->resource->methods)->get_level_count(texture->resource);
            multiplier = texture->mip_count != 1 ? 1.5f : 1.0f;
            memory_delta = (i32)((u32)texture->width * texture->height * 4u);
            PCGraphicsRenderTextureBytes += (u32)(memory_delta * multiplier);
        }
    }

    __declspec(align(16)) extern const u32 ClipOutcodeMask32[4] = {32, 32, 32, 32};
    __declspec(align(16)) extern const u32 ClipOutcodeMask16[4] = {16, 16, 16, 16};
    __declspec(align(16)) extern const u32 ClipOutcodeMask1[4] = {1, 1, 1, 1};
    __declspec(align(16)) extern const u32 ClipOutcodeMask2[4] = {2, 2, 2, 2};
    __declspec(align(16)) extern const u32 ClipOutcodeMask8[4] = {8, 8, 8, 8};
    __declspec(align(16)) extern const u32 ClipOutcodeMask4[4] = {4, 4, 4, 4};
    __declspec(align(16)) extern const f32 ClipOutcodeMinusOne[4] = {-1.0f, -1.0f, -1.0f, -1.0f};

    u32 *PCGraphicsEffectTextureSampler(GRAPHICS_RENDER_TEXTURE *texture) {
        return &texture->flags;
    }

    /* Called from NuDeferredFilter::ResizeTextures instead of inlined: its x87 byte-count
     * conversion (an inline fnstcw/fistp for the unsigned cast) keeps it above VC8's inline limit. */
    void PCGraphicsResizeEffectTexture(GRAPHICS_RENDER_TEXTURE *texture, i32 width, i32 height, i32 levels) {
        if (texture == NULL) {
            return;
        }
        f32 multiplier = texture->mip_count != 1 ? 1.5f : 1.0f;
        i32 memory_delta = (i32)((0u - (u32)texture->width * texture->height) * 4u);
        PCGraphicsRenderTextureBytes += (u32)(memory_delta * multiplier);
        if (texture->external_id == -1 && texture->resource != NULL) {
            texture->resource->methods->release(texture->resource);
            texture->resource = NULL;
        }
        if (texture->surface != NULL) {
            texture->surface->methods->release(texture->surface);
            texture->surface = NULL;
        }
        PCGraphicsInitRenderTexture(texture, width > 0 ? width : texture->width, height > 0 ? height : texture->height,
                                    levels > 0 ? levels : texture->original_mip_count, texture->usage, texture->pool, texture->format);
    }

    void PCGraphicsBindEffectTexture(GRAPHICS_RENDER_TEXTURE *texture, u32 stage) {
        if (texture != NULL) {
            PCDeviceSetTexture(stage, texture->resource);
        } else {
            PCDeviceSetTexture(stage, NULL);
        }
    }

    typedef struct PC_EFFECT PC_EFFECT;

    /* ID3DXEffect method table up to ID3DXBaseEffect::SetTexture (slot 52). */
    typedef struct PC_EFFECT_METHODS {                                                     // size 0xd4
        void *unknown_00[52];                                                              // 0x00
        HRESULT(__stdcall *set_texture)(PC_EFFECT *, const char *, PC_TEXTURE_RESOURCE *); // 0xd0
    } PC_EFFECT_METHODS;

    struct PC_EFFECT {                    // size 0x4
        const PC_EFFECT_METHODS *methods; // 0x0
    };

    void PCGraphicsEffectSetTexture(GRAPHICS_RENDER_TEXTURE *texture, PC_EFFECT *effect, const char *parameter) {
        effect->methods->set_texture(effect, parameter, texture->resource);
    }

    void PCGraphicsReleaseEffectTextures(void) {
        for (i32 index = 0; index != 32; ++index) {
            if (PCGraphicsRenderTextureActive[index] && PCGraphicsRenderTextures[index].external_id == -1) {
                if (PCGraphicsRenderTextures[index].resource != NULL) {
                    PCGraphicsRenderTextures[index].resource->methods->release(PCGraphicsRenderTextures[index].resource);
                    PCGraphicsRenderTextures[index].resource = NULL;
                }
                if (PCGraphicsRenderTextures[index].surface != NULL) {
                    PCGraphicsRenderTextures[index].surface->methods->release(PCGraphicsRenderTextures[index].surface);
                    PCGraphicsRenderTextures[index].surface = NULL;
                }
                f32 multiplier = PCGraphicsRenderTextures[index].mip_count != 1 ? 1.5f : 1.0f;
                i32 memory_delta = (i32)((0u - (u32)PCGraphicsRenderTextures[index].width * PCGraphicsRenderTextures[index].height) * 4u);
                PCGraphicsRenderTextureBytes += (u32)(memory_delta * multiplier);
            }
        }
    }

    void PCGraphicsRestoreEffectTextures(i32 unused) {
        for (i32 index = 0; index != 32; ++index) {
            GRAPHICS_RENDER_TEXTURE *texture = &PCGraphicsRenderTextures[index];
            if (PCGraphicsRenderTextureActive[index]) {
                switch (texture->texture_type) {
                    case 1:
                        if (texture->external_id == -1 && texture->resource == NULL && texture->surface == NULL) {
                            PCGraphicsInitRenderTexture(texture, texture->width, texture->height, texture->mip_count, texture->usage,
                                                        texture->pool, texture->format);
                        }
                        break;
                }
            }
        }
    }

    i32 PCDeviceHardwareVertexProcessingGet(void) {
        return PCDeviceHardwareVertexProcessingPC;
    }

    NUMTX *NuRndrGetViewMatrix(void) {
        return &PcGeometryViewMatrix;
    }

    NUMTX *NuRndrGetProjectionMatrix(void) {
        return &PcGeometryProjectionMatrix;
    }

    NUMTX *NuRndrGetViewProjectionMatrix(void) {
        return &PcGeometryViewProjectionMatrix;
    }

    NUVEC4 *NuRndrGetDistanceOrigin(void) {
        return &PcGeometryDistanceOrigin;
    }

    NUVEC4 *NuDisplayGetHandlerTint(void) {
        return &NuDisplayHandlerTint;
    }

    void *NuDisplayHandlerGetMaterial(void) {
        return NuDisplayHandlerMaterial;
    }

    PC_FRAMEBUFFER *PCGraphicsGetBoundFramebuffer(void) {
        return PcBoundFramebuffer;
    }

    PC_FRAMEBUFFER *NuFramebufferGetObject(i32 index) {
        return PcBootFramebuffers.slots[index];
    }

    i32 fun_005f8a70(void) {
        return 0;
    }

    GRAPHICS_RENDER_TEXTURE *NuFramebufferGetAttachedTex(PC_FRAMEBUFFER *framebuffer, i32 index, i32 *level, i32 *field) {
        PC_EFFECT_SURFACE *target = &framebuffer->targets[index];
        GRAPHICS_RENDER_TEXTURE *texture = target->texture;
        if (texture == NULL) {
            if (level != NULL) {
                *level = 0;
            }
            if (field != NULL) {
                *field = 0;
            }
            return NULL;
        }
        if (level != NULL) {
            *level = target->level;
        }
        /* Tests field but writes through level, including when level is null. */
        if (field != NULL) {
            *level = target->face;
        }
        return texture;
    }

    i32 NuFramebufferGetWidth(PC_FRAMEBUFFER *framebuffer) {
        if (framebuffer == PcDefaultFramebuffer || framebuffer == PcSecondaryFramebuffer) {
            return PCDeviceBackend.parameters.width;
        }
        return framebuffer != NULL ? framebuffer->targets[0].width : 0;
    }

    i32 NuFramebufferGetHeight(PC_FRAMEBUFFER *framebuffer) {
        if (framebuffer == PcDefaultFramebuffer || framebuffer == PcSecondaryFramebuffer) {
            return PCDeviceBackend.parameters.height;
        }
        return framebuffer != NULL ? framebuffer->targets[0].height : 0;
    }

    i32 fun_005f8b40(void) {
        return 0;
    }

    void fun_005f8b50(void) {
    }

    void PCFramebufferSetClearColour(PC_FRAMEBUFFER *framebuffer, i32 index, u32 colour) {
        framebuffer->clear_colour[index] = colour;
    }

    void fun_005f8b80(void) {
    }

    void fun_005f8b90(void) {
    }

    void PCGraphicsFramebufferBarrier(i32, i32) {
    }

    void PCGraphicsFramebufferAction(i32) {
    }

    i32 fun_005f8bd0(void) {
        return 1;
    }

    void PCGraphicsEnableFramebufferGuards(PC_FRAMEBUFFER *frame, i32 enabled) {
    }

    void PCGraphicsBindFramebuffer(PC_FRAMEBUFFER *frame) {
        if (PcBoundFramebuffer == frame && (frame == NULL || frame->changed == 0)) {
            return;
        }
        if (frame == PcDefaultFramebuffer) {
            PCDeviceBackend.ConfigureCurrentTargets();
        } else if (frame == PcSecondaryFramebuffer) {
            PCDeviceBackend.ResetTargets();
        } else if (frame != NULL) {
            PCDeviceBackend.SetSurfaceRenderTarget(&frame->targets[0], 0);
            PCDeviceBackend.SetSurfaceRenderTarget(&frame->targets[1], 1);
            PCDeviceBackend.SetSurfaceRenderTarget(&frame->targets[2], 2);
            PCDeviceBackend.SetSurfaceRenderTarget(&frame->targets[3], 3);
            PCDeviceBackend.SetDepthStencilSurface(&frame->targets[4]);
        }
        PcBoundFramebuffer = frame;
        if (frame != NULL) {
            frame->changed = 1;
        }
    }

    void PCGraphicsBindDefaultFramebuffer(void) {
        PCGraphicsBindFramebuffer(PcDefaultFramebuffer);
    }

    void PCGraphicsClearFramebuffer(u32 mask, i32 colour) {
        u32 flags = 0;
        if (PcBoundFramebuffer->targets[0].texture != NULL && (mask & 0x100)) {
            flags = 1;
        }
        if (PcBoundFramebuffer->targets[4].texture != NULL && (mask & 0x200)) {
            flags |= 6;
        }
        if (flags != 0) {
            i32 converted = (((colour >> 24) * 256 + (colour & 0xff)) * 256 + ((colour >> 8) & 0xff)) * 256 + ((colour >> 16) & 0xff);
            PCDeviceClear(0, NULL, flags, converted, 1.0f, 0);
        }
    }

    PC_EFFECT_SURFACE *PCGraphicsFramebufferTarget(PC_FRAMEBUFFER *frame, i32 index) {
        return &frame->targets[index];
    }

    void fun_005f8d40(void) {
    }

    void fun_005f8d50(void) {
    }

    i32 NuTexRead(char *name, VARIPTR *buffer, VARIPTR end) {
        char filename[1024];
        NUFILE file;
        i32 size;
        i32 id;
        void *data;
        NUTEXDIM *texture;
        NuStrFixExt(filename, name, "dds", sizeof(filename));
        file = NuFileOpen(filename, 0);
        if (file == 0) {
            return 0;
        }
        size = (i32)NuFileOpenSize(file);
        data = NuAlloc(size, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nutex_pc.cpp", 0x2f);
        NuFileRead(file, data, size);
        NuFileClose(file);
        texture = (NUTEXDIM *)buffer->addr;
        buffer->addr += sizeof(NUTEXDIM);
        memset(texture, 0, sizeof(*texture));
        texture->image_data = data;
        texture->image_size = size;
        id = NuTexCreateNative(texture);
        NuFree(data, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nutex_pc.cpp", 0x4d);
        return id;
    }

    void NuTexCreatePS(NUTEXDIM *texture) {
        PC_TEXTURE_LEVEL_DESC description;
        if (texture != NULL && texture->image_data != NULL && texture->image_size != 0) {
            NudxFw_D3DBeginCriticalSection();
            PCDeviceCreateTextureFromMemory(texture->image_data, texture->image_size, &texture->resource);
            texture->resource->methods->get_level_desc(texture->resource, 0, &description);
            NudxFw_D3DEndCriticalSection();
            texture->width = (i32)description.width;
            texture->height = (i32)description.height;
            texture->image_data = NULL;
            texture->image_size = 0;
        }
    }

    void fun_005f8ec0(void) {
    }

    void NuTexDestroyPS(NUTEXDIM *texture) {
        if (texture != NULL) {
            NudxFw_D3DBeginCriticalSection();
            PCDeviceReleaseTexture(&texture->resource);
            NudxFw_D3DEndCriticalSection();
        }
    }

    i32 fun_005f8f00(void) {
        return 0;
    }

    i32 NuTexGetReqSize(i32, i32) {
        return 0;
    }

    i32 NuTexReserve(i32) {
        return -1;
    }

    void NuTexUnReserve(void) {
    }

    void fun_005f8f40(void) {
    }

    void PCGraphicsBindTextureDimensions(NUTEXDIM *texture, u32 stage) {
        PCDeviceSetTexture(stage, texture->resource);
    }

    i32 fun_005f8f70(void) {
        return 0;
    }

    void NuTexInitExPS(VARIPTR *buffer) {
        PCDeviceCreateCubeTextureFromFile("pc\\stuff\\LegoCubeMap_PC.dds", &NuTextureEnvironmentCube);
        PCDeviceCreateCubeTextureFromFile("pc\\stuff\\PhongMap_PC.dds", &NuTexturePhongCube);
    }

    /* Empty hooks; the D3D release and restore paths call them for textures. */
    void NuTexReleaseResources(void) {
    }

    void NuTexRestoreResources(void) {
    }

    /* Creates, once, the placeholder texture used for references that fail to resolve. */
    i32 NuTexGetUnresolvedTextureTIDPS() {
        if (NuUnresolvedTextureIDPC == -1) {
            NUTEXDIM *texture = (NUTEXDIM *)NuAlloc(sizeof(NUTEXDIM), "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nutex_pc.cpp", 0xd9);
            memset(texture, 0, sizeof(*texture));
            texture->image_data = NuUnresolvedTextureDds;
            texture->image_size = sizeof(NuUnresolvedTextureDds);
            NuUnresolvedTextureIDPC = NuTexCreateNative(texture);
        }
        return NuUnresolvedTextureIDPC;
    }

    /* Bits per texel of a source image type. */
    i32 NuTexPixelBits(i32 type) {
        switch (type) {
            case 0:
            case 1:
                return 16;
            case 2:
                return 24;
            case 3:
                return 32;
            case 5:
            case 20:
                return 8;
            case 4:
                return 4;
            default:
                return 0;
        }
    }

    i32 NuTexImageSize(i32 type, i32 width, i32 height) {
        return NuTexPixelBits(type) * width * height / 8;
    }

    i32 NuTexImageSizeMips(i32 type, i32 width, i32 height, i32 mips) {
        i32 bits = NuTexPixelBits(type);
        i32 size = (bits * width * height) >> 3;
        i32 level;

        for (level = 0; level < mips;) {
            level++;
            size += ((width >> level) * (height >> level) * bits) >> 3;
        }
        return size;
    }

    /* Loads an uncompressed BMP into a bitmap record, flipping it top to bottom. */
    i32 NuTexLoadBmp(const char *path, i32 mips, NUTEXBMP *bitmap) {
        NUFILE file;
        NUTEXBMP_FILE_HEADER file_header;
        NUTEXBMP_INFO_HEADER info_header;
        i32 palette_size;
        i32 width;
        i32 height;
        i32 levels;
        i32 row_size;
        i32 size;
        u8 *row;
        u8 *top;
        u8 *bottom;
        NUTEXBMP_COLOUR swapped;
        u8 blue;
        i32 i;

        file = NuFileOpen(path, 0);
        if (file != 0) {
            if (bitmap != NULL) {
                memset(bitmap, 0, sizeof(*bitmap));
                NuFilePos(file);
                NuFileRead(file, &file_header.type, 14);
                NuFileRead(file, &info_header, sizeof(info_header));
                switch (info_header.bit_count) {
                    case 32:
                        bitmap->type = 3;
                        palette_size = 0;
                        break;
                    case 24:
                        bitmap->type = 2;
                        palette_size = 0;
                        break;
                    case 4:
                        bitmap->type = 4;
                        palette_size = 0x40;
                        NuFileRead(file, NuTexBmpPalette, 0x40);
                        for (i = 0; i < 0x10; i++) {
                            swapped = NuTexBmpPalette[i];
                            blue = swapped.blue;
                            swapped.blue = swapped.red;
                            swapped.red = blue;
                            NuTexBmpPalette[i] = swapped;
                        }
                        break;
                    case 8:
                        bitmap->type = 5;
                        palette_size = 0x400;
                        NuFileRead(file, NuTexBmpPalette, 0x400);
                        for (i = 0; i < 0x100; i++) {
                            swapped = NuTexBmpPalette[i];
                            blue = swapped.blue;
                            swapped.blue = swapped.red;
                            swapped.red = blue;
                            NuTexBmpPalette[i] = swapped;
                        }
                        break;
                    default:
                        NuTexBmpUnsupportedDepth(info_header.bit_count, path);
                        break;
                }
                bitmap->width = width = info_header.width;
                bitmap->height = height = info_header.height;
                bitmap->mips = mips;
                row_size = (NuTexPixelBits(bitmap->type) * width + 7) / 8;
                levels = mips + 1;
                size = 0;
                do {
                    size += NuTexPixelBits(bitmap->type) * height * width / 8;
                    width >>= 1;
                    height >>= 1;
                } while (--levels > 0);
                bitmap->data = (u8 *)NuAlloc(size, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nutex_pc.cpp", 0x195);
                if (palette_size != 0) {
                    bitmap->palette = (u8 *)NuAlloc(palette_size, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nutex_pc.cpp", 0x198);
                } else {
                    bitmap->palette = NULL;
                }
                _flushall();
                NuFileSeek(file, file_header.data_offset, 0);
                NuFileRead(file, bitmap->data, NuTexImageSize(bitmap->type, bitmap->height, bitmap->width));
                row = (u8 *)NuAlloc(row_size, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nutex_pc.cpp", 0x1a0);
                top = bitmap->data;
                bottom = top + NuTexImageSize(bitmap->type, bitmap->height, bitmap->width) - row_size;
                while (bottom > top) {
                    memcpy(row, top, row_size);
                    memcpy(top, bottom, row_size);
                    memcpy(bottom, row, row_size);
                    top += row_size;
                    bottom -= row_size;
                }
                NuFree(row, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nutex_pc.cpp", 0x1af);
                if (palette_size != 0) {
                    memcpy(bitmap->palette, NuTexBmpPalette, palette_size);
                }
            }
            NuFileClose(file);
            return 1;
        }
        sprintf(NuTexBmpErrorText, "Cannot open file: %s", path);
        return 0;
    }

    NUTEXBMP *NuTexCreateBmp(const char *path) {
        NUTEXBMP *bitmap = (NUTEXBMP *)NuAlloc(sizeof(NUTEXBMP), "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nutex_pc.cpp", 0x1e1);
        if (!NuTexLoadBmp(path, 0, bitmap)) {
            NuFree(bitmap, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nutex_pc.cpp", 0x1e5);
            return NULL;
        }
        return bitmap;
    }

    void NuWaterClear(void) {
        nu_water_flow_state = 0;
        nu_water_reset_status = 0;
    }

    void NuWaterReset(void) {
        nu_water_reset_state = 1;
        nu_water_reset_status = 0;
    }

    /* Releases the water vertex buffer before the D3D device is reset or destroyed. */
    void NuWaterReleaseVertexBuffer(void) {
        if (NuWaterVertexBuffer != NULL) {
            NuWaterVertexBuffer->methods->release(NuWaterVertexBuffer);
            NuWaterVertexBuffer = NULL;
        }
    }

    void fun_005f9930(void) {
        NuPtlDmaSlotsExhausted();
    }

    void NuWaterSpeed(f32 speed) {
        NuShaderObjectBaseSetWaterSpeed(speed);
    }

    void fun_005f9950(void) {
    }

    void NuRndrShadowVolumeFinish(void) {
        PCDeviceSetVertexShader(NULL);
        PCDeviceSetPixelShader(NULL);
        PCDeviceSetRenderState(D3DRS_ALPHABLENDENABLE, 1);
        PCDeviceSetRenderState(D3DRS_ZWRITEENABLE, 0);
        PCDeviceSetRenderState(D3DRS_LIGHTING, 0);
        PCDeviceSetRenderState(D3DRS_SPECULARENABLE, 0);
        PCDeviceSetTexture(0, NULL);
        PCDeviceSetTextureStageState(0, D3DTSS_COLOROP, 2);
        PCDeviceSetTextureStageState(0, D3DTSS_COLORARG1, 0);
        PCDeviceSetTextureStageState(0, D3DTSS_ALPHAOP, 2);
        PCDeviceSetTextureStageState(0, D3DTSS_ALPHAARG1, 0);
        PCDeviceSetTextureStageState(1, D3DTSS_COLOROP, 1);
        PCDeviceSetTextureStageState(1, D3DTSS_ALPHAOP, 1);
        PCDeviceSetRenderState(D3DRS_STENCILFUNC, RendererStencilFunc);
        PCDeviceSetRenderState(D3DRS_STENCILPASS, RendererStencilPass);
        PCDeviceSetRenderState(D3DRS_STENCILREF, RendererStencilRef);
        PCDeviceSetRenderState(D3DRS_ZENABLE, 0);
        PCDeviceSetRenderState(D3DRS_CULLMODE, 1);
        PCDeviceSetRenderState(D3DRS_BLENDOP, 1);
        PCDeviceSetRenderState(D3DRS_SRCBLEND, 5);
        PCDeviceSetRenderState(D3DRS_DESTBLEND, 6);
        PCDeviceSetVertexDeclaration(PCDeviceBackend.screen_colour_uv_declaration);
        PCDeviceSetStreamSource(0, RendererQuadVertexBuffer, 0, 0x1c);
        PCDeviceDrawPrimitive(6, 0, 2);
    }

    void NuRndrShadowVolumeDraw(void) {
        i32 i;
        u32 n;
        PCDeviceSetVertexDeclaration(RendererVertexDeclaration);
        PCDeviceSetRenderState(D3DRS_ZWRITEENABLE, 0);
        PCDeviceSetRenderState(D3DRS_ALPHABLENDENABLE, 1);
        PCDeviceSetRenderState(D3DRS_SRCBLEND, 1);
        PCDeviceSetRenderState(D3DRS_DESTBLEND, 2);
        PCDeviceSetRenderState(D3DRS_ZENABLE, 1);
        PCDeviceSetRenderState(D3DRS_STENCILZFAIL, 1);
        PCDeviceSetRenderState(D3DRS_STENCILENABLE, 1);
        PCDeviceSetRenderState(D3DRS_STENCILFUNC, 8);
        PCDeviceSetRenderState(D3DRS_STENCILPASS, 4);
        PCDeviceSetRenderState(D3DRS_STENCILREF, 1);
        PCDeviceSetRenderState(D3DRS_CULLMODE, 3);
        for (i = 0; i != RendererArenaVertexCount; i += 4) {
            PCDeviceDrawPrimitiveUPPc(6, 2, &RendererArenaData[i], 0x10);
        }
        for (n = 0; n < RendererArenaCount; n++) {
            PCDeviceDrawPrimitiveUPPc(6, RendererArenaTypes[n] - 2, &RendererArenaData[RendererArenaOffsets[n]], 0x40);
        }
        PCDeviceSetRenderState(D3DRS_CULLMODE, 2);
        PCDeviceSetRenderState(D3DRS_STENCILPASS, 5);
        for (i = 0; i != RendererArenaVertexCount; i += 4) {
            PCDeviceDrawPrimitiveUPPc(6, 2, &RendererArenaData[i], 0x10);
        }
        for (n = 0; n < RendererArenaCount; n++) {
            PCDeviceDrawPrimitiveUPPc(6, RendererArenaTypes[n] - 2, &RendererArenaData[RendererArenaOffsets[n]], 0x40);
        }
    }

    /* Renders the queued hierarchy shadow packets as stencil shadow volumes; never called. */
    void NuRndrShadowVolumes(void) {
        if (PcWindowOptions.shadowsenabled != 0 && PcWindowOptions.shadowmethod == 1 && PCShadowPacketCount != 0) {
            PCDeviceSetVertexShader(NULL);
            PCDeviceSetPixelShader(NULL);
            PCDeviceSetRenderState(D3DRS_ALPHATESTENABLE, 0);
            PCDeviceSetRenderState(D3DRS_ZFUNC, 4);
            PCDeviceSetTexture(0, NULL);
            PCDeviceSetWorldIdentity();
            NuRndrArenaBuildShadowVolumes(0x80, 0);
            NuRndrShadowVolumeDraw();
            NuRndrShadowVolumeFinish();
            PCDeviceSetRenderState(D3DRS_ZENABLE, 1);
            PCDeviceSetRenderState(D3DRS_STENCILENABLE, 0);
            PCDeviceSetRenderState(D3DRS_ZWRITEENABLE, 1);
            PCDeviceSetRenderState(D3DRS_ALPHABLENDENABLE, 0);
            PCDeviceSetRenderState(D3DRS_CULLMODE, 3);
            RendererArenaVertexCount = 0;
            RendererArenaCount = 0;
        }
        PCShadowPacketCount = 0;
    }

    void NuRndrShadowOnOff(i32 value) {
        NuRndrControlState = value;
    }

    void fun_005f9ea0(void) {
        PCGraphicsRenderTextureAllocationError();
    }

    void fun_005f9eb0(void) {
        PCGraphicsRenderTextureAllocationError();
    }

    void fun_005f9ec0(void) {
        PCGraphicsRenderTextureAllocationError();
    }

    /* Empty hook; the D3D release paths call it for the geometry origin state. */
    void PcGeometryOriginRelease(void) {
    }

    void fun_005fa230(void) {
    }

    void PcResetGeometryOriginState(void) {
        PcGeometryViewMatrix = NuMtxSetIdentityTemplate;
        PcGeometryProjectionMatrix = NuMtxSetIdentityTemplate;
        PcGeometryViewProjectionMatrix = NuMtxSetIdentityTemplate;
        PcGeometryInverseViewProjectionMatrix = NuMtxSetIdentityTemplate;
        PcGeometryResetMatrix = NuMtxSetIdentityTemplate;
        PcGeometryDistanceOrigin = PcGeometryDefaultOrigin;
        for (i32 i = 0; i != 16; ++i) {
            PCDeviceSamplerInvalidation[i][1] = 0;
            PCDeviceSamplerInvalidation[i][0] = 0x012449d0;
        }
    }

    void PCGraphicsSubmitViewport(i32 x, i32 y, i32 width, i32 height) {
        PC_VIEWPORT viewport;
        viewport.x = x;
        viewport.y = y;
        viewport.width = width;
        viewport.height = height;
        viewport.min_z = 0.0f;
        viewport.max_z = 1.0f;
        NuVpSubmit(&viewport);
    }

    void PcUpdateGeometryCameraOrigin(NUMTX *view, NUMTX *projection) {
        NUVEC scale;
        NUVEC translation;
        scale.z = 1.0f;
        translation.x = 0.0f;
        translation.y = 0.0f;
        translation.z = 0.0f;
        scale.x = PCSpecialProjectionAdjust.x_scale;
        scale.y = PCSpecialProjectionAdjust.y_scale;
        NuMtxSetScale(&PcGeometryScaleMatrix, &scale);
        translation.x = PCSpecialProjectionAdjust.x_offset;
        translation.y = PCSpecialProjectionAdjust.y_offset;
        NuMtxSetTranslation(&PcGeometryTranslationMatrix, &translation);
        NuMtxMulH(&PcGeometryAdjustedProjectionMatrix, projection, &PcGeometryScaleMatrix);
        NuMtxMulH(&PcGeometryAdjustedProjectionMatrix, &PcGeometryAdjustedProjectionMatrix, &PcGeometryTranslationMatrix);
        PcGeometryViewMatrix = *view;
        PcGeometryProjectionMatrix = PcGeometryAdjustedProjectionMatrix;
        NUMTX view_inverse;
        NuMtxInvH(&view_inverse, &PcGeometryViewMatrix);
        PcGeometryDistanceOrigin.x = view_inverse.m30 / view_inverse.m33;
        PcGeometryDistanceOrigin.y = view_inverse.m31 / view_inverse.m33;
        PcGeometryDistanceOrigin.z = view_inverse.m32 / view_inverse.m33;
        PcGeometryDistanceOrigin.w = 1.0f;
        NuMtxMulH(&PcGeometryViewProjectionMatrix, &PcGeometryViewMatrix, &PcGeometryProjectionMatrix);
        NuMtxInvH(&PcGeometryInverseViewProjectionMatrix, &PcGeometryViewProjectionMatrix);
        NuShaderManagerSetfv(SHADER_UNIFORM_VIEW_MATRIX, &PcGeometryViewMatrix.m00);
        NuShaderManagerSetfv(SHADER_UNIFORM_VIEW_PROJ, &PcGeometryViewProjectionMatrix.m00);
        NuShaderManagerSetfv(SHADER_UNIFORM_WORLD_CAM_POS, &PcGeometryDistanceOrigin.x);
        f32 fovy;
        f32 aspect;
        f32 z_near;
        f32 z_far;
        NuMtxGetPerspectiveD3D(&PcGeometryProjectionMatrix, &fovy, &aspect, &z_near, &z_far);
        NUVEC4 depth = {z_near, z_far, z_far - z_near, 1.0f / PcGeometryProjectionMatrix.m22};
        NuShaderManagerSetfv(SHADER_UNIFORM_PROJECTION_PARAMS, &depth.x);
        NUVEC4 frustum;
        NuMtxGetFrustumD3D(&PcGeometryProjectionMatrix, &frustum.x, &frustum.y, &frustum.z, &frustum.w, &z_near, &z_far);
        frustum.y -= frustum.x;
        frustum.w -= frustum.z;
        NuShaderManagerSetfv(SHADER_UNIFORM_FRUSTUM_PARAMS, &frustum.x);
    }

    i32 fun_005fa530(i32 value) {
        return value;
    }

    void PCGraphicsSetSamplerTuple(u32 stage, const u32 *tuple) {
        union {
            f32 value;
            u32 bits;
        } bias;

        PCDeviceSetSamplerState(stage, D3DSAMP_MAXMIPLEVEL, tuple[0] & 15);
        if ((((tuple[0] >> 4) ^ tuple[0]) & 15) == 0) {
            bias.value = -10.0f;
            PCDeviceSetSamplerState(stage, D3DSAMP_MIPMAPLODBIAS, bias.bits);
        } else {
            bias.value = 0.0f;
            PCDeviceSetSamplerState(stage, D3DSAMP_MIPMAPLODBIAS, bias.bits);
        }
        PCDeviceSetSamplerState(stage, D3DSAMP_ADDRESSU, (tuple[0] >> 8) & 7);
        PCDeviceSetSamplerState(stage, D3DSAMP_ADDRESSV, (tuple[0] >> 11) & 7);
        PCDeviceSetSamplerState(stage, D3DSAMP_ADDRESSW, (tuple[0] >> 14) & 7);
        PCDeviceSetSamplerState(stage, D3DSAMP_MAGFILTER, (tuple[0] >> 17) & 7);
        PCDeviceSetSamplerState(stage, D3DSAMP_MINFILTER, (tuple[0] >> 20) & 7);
        PCDeviceSetSamplerState(stage, D3DSAMP_MIPFILTER, (tuple[0] >> 23) & 7);
        PCDeviceSetSamplerState(stage, D3DSAMP_BORDERCOLOR, tuple[1]);
        PCDeviceSamplerInvalidation[stage][0] = tuple[0];
        PCDeviceSamplerInvalidation[stage][1] = tuple[1];
    }

    void fun_005fa630(void) {
        PCGraphicsRenderTextureAllocationError();
    }

    /* Converts a byte string into glyph indices of the font. */
    void NuQFntAsciiToQCode(VUFNT *font, u16 *encoded, const char *text) {
        while (*text != '\0') {
            *encoded = UnicodeToIndexFast(font->unicode_map, font->unicode_count, (u16)(i16)*text);
            text++;
            encoded++;
        }
        *encoded = 0;
    }

    /* Nonzero when any texel of the 8-bit column at (x, y) differs from the key value. */
    static i32 NuQFntColumnDiffers8(NUTEXBMP *bitmap, i32 x, i32 y, i32 count, i32 value) {
        i32 width = bitmap->width;
        u8 *texel = bitmap->data + width * y + x;
        i32 i;

        for (i = 0; i < count; i++) {
            if (*texel != value) {
                return 1;
            }
            texel += width;
        }
        return 0;
    }

    static i32 NuQFntColumnOpaque32(NUTEXBMP *bitmap, i32 x, i32 y, i32 count) {
        return NuQFntColumnOpaque32Inline(bitmap, x, y, count);
    }

    static i32 NuQFntFindGlyphStart8(NUTEXBMP *bitmap, i32 count, i32 *x, i32 *y, i32 value) {
        do {
            do {
                if (NuQFntColumnDiffers8(bitmap, *x, *y, count, value)) {
                    return 1;
                }
                (*x)++;
            } while (*x < bitmap->width);
            *x = 0;
            *y += count;
        } while (*y + count < bitmap->height);
        return 0;
    }

    static i32 NuQFntFindGlyphEnd8(NUTEXBMP *bitmap, i32 count, i32 *x, i32 *y, i32 value) {
        do {
            do {
                if (!NuQFntColumnDiffers8(bitmap, *x, *y, count, value)) {
                    return 1;
                }
                (*x)++;
            } while (*x < bitmap->width);
            *x = 0;
            *y += count;
        } while (*y + count < bitmap->height);
        return 0;
    }

    static i32 NuQFntFindGlyphStart32(NUTEXBMP *bitmap, i32 count, i32 *x, i32 *y) {
        do {
            do {
                if (NuQFntColumnOpaque32(bitmap, *x, *y, count)) {
                    return 1;
                }
                (*x)++;
            } while (*x < bitmap->width);
            *x = 0;
            *y += count;
        } while (*y + count < bitmap->height);
        return 0;
    }

    static i32 NuQFntFindGlyphEnd32(NUTEXBMP *bitmap, i32 count, i32 *x, i32 *y) {
        do {
            do {
                if (!NuQFntColumnOpaque32(bitmap, *x, *y, count)) {
                    return 1;
                }
                (*x)++;
            } while (*x < bitmap->width);
            *x = 0;
            *y += count;
        } while (*y + count < bitmap->height);
        return 0;
    }

    void NuQFntScanReset(void) {
        NuQFntScanY = 0;
        NuQFntScanX = 0;
    }

    /* Finds the next glyph cell in an 8-bit strip; returns nonzero once the strip is exhausted. */
    i32 NuQFntNextGlyph8(NUTEXBMP *bitmap, i32 count, i32 value, f32 *x, f32 *y, f32 *width) {
        i32 exhausted = 0;
        i32 start;

        if (!NuQFntFindGlyphStart8(bitmap, count, &NuQFntScanX, &NuQFntScanY, value)) {
            exhausted = 1;
            NuQFntScanX = 0;
            NuQFntScanY = 0;
        }
        *x = (f32)NuQFntScanX;
        *y = (f32)NuQFntScanY;
        start = NuQFntScanX;
        if (!NuQFntFindGlyphEnd8(bitmap, count, &NuQFntScanX, &NuQFntScanY, value)) {
            exhausted = 1;
            NuQFntScanX = bitmap->width;
            NuQFntScanY = bitmap->height;
        }
        *width = (f32)(NuQFntScanX - start);
        return exhausted;
    }

    /* Finds the next glyph cell in a 32-bit strip; returns nonzero once the strip is exhausted. */
    i32 NuQFntNextGlyph32(NUTEXBMP *bitmap, i32 count, f32 *x, f32 *y, f32 *width) {
        i32 exhausted = 0;
        i32 start;

        if (!NuQFntFindGlyphStart32(bitmap, count, &NuQFntScanX, &NuQFntScanY)) {
            exhausted = 1;
            NuQFntScanY = 0;
            NuQFntScanX = 0;
        }
        *x = (f32)NuQFntScanX;
        *y = (f32)NuQFntScanY;
        start = NuQFntScanX;
        if (!NuQFntFindGlyphEnd32(bitmap, count, &NuQFntScanX, &NuQFntScanY)) {
            exhausted = 1;
            NuQFntScanX = bitmap->width;
            NuQFntScanY = bitmap->height;
        }
        *width = (f32)(NuQFntScanX - start);
        return exhausted;
    }

    static u16 NuQFntMapLookup(VUCHARIDX *map, i32 count, u16 unicode) {
        u16 i;

        for (i = 0; i < count; i++) {
            if (map[i].unicode == unicode) {
                return map[i].index;
            }
        }
        return 0xffff;
    }

    i32 NuQFntMapAdd(VUCHARIDX *map, i32 slot, u16 unicode, u16 index) {
        if (slot > 808 || slot < 0) {
            NuQFntAtlasError();
        }
        map[slot].unicode = unicode;
        map[slot].index = index;
        return slot + 1;
    }

    /* Font material set-up used by the atlas builder; mirrors NuQFntReadPS. */
    static i32 NuQFntAtlasReadPS(VUFNT *font, i32 texture_id, i32 flags, i32 plane, VARIPTR *buffer, VARIPTR end) {
        NUQFNT_RENDER_DATA *platform;
        NUMTL *material;

        NuTexGetWidth(texture_id);
        NuTexGetHeight(texture_id);
        buffer->addr = (buffer->addr + 15) & ~15U;
        font->platform_data = (NUQFNT_RENDER_DATA *)buffer->void_ptr;
        buffer->addr += sizeof(*platform);
        platform = font->platform_data;
        font->color_abgr = &platform->colour;
        font->x_scale = &platform->x_scale;
        font->y_scale = &platform->y_scale;
        font->material = NuMtlCreateEx(1, plane);
        if (font->material != NULL) {
            font->material->diffuse_r = 1.0f;
            font->material->diffuse_g = 1.0f;
            font->material->diffuse_b = 1.0f;
            font->material->attribs.word = (font->material->attribs.word & ~0x1000U) | 0x2000U;
            font->material->attribs.word |= 0xc000U;
            font->material->opacity = 1.0f;
            font->material->attribs.word = (font->material->attribs.word & ~14U) | 1U;
            font->material->attribs.word |= 0x40000U;
            font->material->texture_id = (i16)texture_id;
            material = font->material;
            if (flags & NUQFNT_3D) {
                material->attribs2 &= ~4U;
            } else {
                material->attribs2 |= 4U;
            }
            font->material->attribs.word = (font->material->attribs.word & ~0x10000U) | 0x20000U;
            if (flags & NUQFNT_OPAQUE) {
                font->material->attribs.word &= ~15U;
            } else {
                font->material->attribs.word = (font->material->attribs.word & ~14U) | 1U;
            }
            if (flags & NUQFNT_Z_BUFFERED) {
                font->material->attribs.word &= ~0xc000U;
            } else if (flags & NUQFNT_Z_READ_ONLY) {
                font->material->attribs.word = (font->material->attribs.word & ~0x8000U) | 0x4000U;
            } else {
                font->material->attribs.word |= 0xc000U;
            }
            if (flags & 4) {
                font->material->attribs.word = (font->material->attribs.word & ~0x20U) | 0x10U;
            } else if (flags & 0x10) {
                font->material->attribs.word = (font->material->attribs.word & ~0x10U) | 0x20U;
            } else {
                font->material->attribs.word &= ~0x30U;
            }
            NuMtlUpdate(font->material);
            return 1;
        }
        return 0;
    }

    int NuQFntMapCompare(const void *a, const void *b) {
        u16 left = ((const VUCHARIDX *)a)->unicode;
        u16 right = ((const VUCHARIDX *)b)->unicode;

        if (left < right) {
            return -1;
        }
        return left != right;
    }

    /* Builds a font from a BMP glyph strip, one cell per character of text in strip order. */
    VUFNT *NuQFntCreateFromBmp(const char *path, i32 cell_height, i32 baseline, i32 key_colour, u16 *text, i32 flags, i32 plane,
                               VARIPTR *buffer, VARIPTR end) {
        f32 width;
        f32 y;
        f32 x;
        i32 max_count;
        i32 count;
        NUTEXBMP *bitmap;
        VUCHARIDX map[NUQFNT_ATLAS_MAP_SIZE];
        VUFNTCHAR glyphs[NUQFNT_ATLAS_GLYPH_SIZE];
        PC_LOCKED_RECT locked;
        PC_TEXTURE_RESOURCE *resource;
        NUTEXDIM *texture;
        VUFNT *font;
        u8 *source;
        u8 *destination;
        i32 row_size;
        i32 row;
        i32 index;
        i32 next;
        i32 c;
        i32 size;
        u16 glyph;
        i32 texture_id;

        NuStrLenW(text);
        memset(glyphs, 0, sizeof(glyphs));
        memset(map, 0xff, sizeof(map));
        bitmap = NuTexCreateBmp(path);
        if (bitmap == NULL) {
            return NULL;
        }
        if (bitmap->type != 5) {
            if (bitmap->type != 3) {
                NuQFntAtlasError();
            }
        } else {
            for (c = 0; c < 256; c++) {
                if (c == key_colour) {
                    bitmap->palette[c * 4 + 3] = 0;
                } else {
                    ((u32 *)bitmap->palette)[c] |= 0x7f000000;
                }
            }
        }

        NuQFntScanReset();
        index = 1;
        max_count = 0;
        for (; *text != 0; text++) {
            /* Skip the slots that characters with their own fixed slot occupy. */
            if (index == ' ') {
                index = '!';
            }
            if (index == '!') {
                index = '"';
            }
            if (index == '%') {
                index = '&';
            }
            if (index == '?') {
                index = '@';
            }
            if (index == '.') {
                index = '/';
            }
            if (index == ',') {
                index = '-';
            }
            if (index == '0') {
                index = ':';
            }
            if (index == 'A') {
                index = '[';
            }
            if (index == 'a') {
                index = '{';
            }
            if (index == '[') {
                index = '\\';
            }
            if (index == ']') {
                index = '^';
            }
            if (*text == ' ') {
                continue;
            }
            if ((*text >= 'a' && *text <= 'z') || (*text >= 'A' && *text <= 'Z') || (*text >= '0' && *text <= '9') || *text == '!' ||
                *text == '?' || *text == '.' || *text == ',' || *text == '%' || *text == '[' || *text == ']') {
                if (bitmap->type == 5) {
                    NuQFntNextGlyph8(bitmap, cell_height, key_colour, &x, &y, &width);
                } else {
                    NuQFntNextGlyph32(bitmap, cell_height, &x, &y, &width);
                }
                glyphs[*text].x = x;
                glyphs[*text].y = y;
                glyphs[*text].width = width;
                next = NuQFntMapAdd(map, *text, *text, *text);
            } else {
                if (bitmap->type == 5) {
                    NuQFntNextGlyph8(bitmap, cell_height, key_colour, &x, &y, &width);
                } else {
                    NuQFntNextGlyph32(bitmap, cell_height, &x, &y, &width);
                }
                glyphs[index].x = x;
                glyphs[index].y = y;
                glyphs[index].width = width;
                index = NuQFntMapAdd(map, index, *text, (u16)index);
                next = index + 1;
            }
            if (max_count <= next) {
                max_count = next;
            }
        }

        glyphs[' '].x = 0.0f;
        glyphs[' '].y = 0.0f;
        glyphs[' '].width = (f32)(cell_height >> 2);
        count = max_count;
        map[' '].unicode = ' ';
        map[' '].index = ' ';

        /* Lower case letters without a cell borrow the upper case glyph. */
        for (c = 'a'; c <= 'z'; c++) {
            if (map[c].unicode == 0xffff) {
                glyph = NuQFntMapLookup(map, count, NuToUpper((u8)c));
                if (glyph != 0xffff) {
                    NuQFntMapAdd(map, c, (u16)c, glyph);
                    if (max_count <= c) {
                        max_count = c;
                    }
                }
            }
        }
        /* Accented lower case letters fall back to their upper case forms. */
        for (c = 0xe0; c <= 0xff; c++) {
            if (NuQFntMapLookup(map, index, (u16)c) == 0xffff) {
                glyph = NuQFntMapLookup(map, count, NuToUpper((u8)c));
                if (glyph != 0xffff) {
                    index = NuQFntMapAdd(map, index, (u16)c, glyph);
                }
            }
        }
        /* Curly quotes reuse the apostrophe. */
        if (NuQFntMapLookup(map, index, 0x91) == 0xffff) {
            glyph = NuQFntMapLookup(map, count, '\'');
            if (glyph != 0xffff) {
                index = NuQFntMapAdd(map, index, 0x91, glyph);
            }
        }
        if (NuQFntMapLookup(map, index, 0x92) == 0xffff) {
            glyph = NuQFntMapLookup(map, count, '\'');
            if (glyph != 0xffff) {
                index = NuQFntMapAdd(map, index, 0x92, glyph);
            }
        }
        if (index <= 'z') {
            index = '{';
        }
        qsort(map, index, sizeof(VUCHARIDX), NuQFntMapCompare);

        texture = (NUTEXDIM *)buffer->void_ptr;
        buffer->addr += sizeof(NUTEXDIM);
        memset(texture, 0, sizeof(*texture));
        texture->width = bitmap->width;
        texture->height = bitmap->height;
        PCDeviceCreateTexture(bitmap->width, bitmap->height, bitmap->mips, 0, 0x15, 1, &texture->resource);
        texture_id = NuTexCreateNative(texture);
        if (texture_id == 0) {
            NuQFntAtlasError();
        }
        resource = NuTextureGetDimensions(texture_id)->resource;
        if (resource->methods->lock_rect(resource, 0, &locked, NULL, 0) >= 0) {
            source = bitmap->data;
            row_size = bitmap->width * 4;
            destination = (u8 *)locked.bits;
            for (row = 0; row < bitmap->height; row++) {
                memcpy(destination, source, row_size);
                destination += locked.pitch;
                source += row_size;
            }
            resource = NuTextureGetDimensions(texture_id)->resource;
            resource->methods->unlock_rect(resource, 0);
        }
        NuFree(bitmap->data, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nuqfnt_PC.cpp", 0x264);
        NuFree(bitmap, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nuqfnt_PC.cpp", 0x265);
        NuTexGetWidth(texture_id);
        NuTexGetHeight(texture_id);

        font = (VUFNT *)((buffer->addr + 15) & ~15U);
        buffer->addr = ((buffer->addr + 15) & ~15U) + sizeof(VUFNT);
        memset(font, 0, sizeof(VUFNT));
        font->glyph_count = count;
        font->height = (f32)cell_height;
        font->unicode_count = index;
        font->legacy_font = NULL;
        font->baseline = (f32)baseline;
        font->ic_gap = 0.0f;
        font->space_width = glyphs[' '].width;

        buffer->addr = (buffer->addr + 15) & ~15U;
        font->glyphs = (VUFNTCHAR *)buffer->void_ptr;
        buffer->addr += font->glyph_count * sizeof(VUFNTCHAR);
        memcpy(font->glyphs, glyphs, font->glyph_count * sizeof(VUFNTCHAR));
        for (size = font->glyph_count * sizeof(VUFNTCHAR); size & 15; size += 4) {
            *buffer->u32_ptr = 0;
            buffer->addr += 4;
        }

        buffer->addr = (buffer->addr + 15) & ~15U;
        font->unicode_map = (VUCHARIDX *)buffer->void_ptr;
        buffer->addr += font->unicode_count * sizeof(VUCHARIDX);
        memcpy(font->unicode_map, map, font->unicode_count * sizeof(VUCHARIDX));
        buffer->addr = (buffer->addr + 15) & ~15U;
        font->size = buffer->addr - (u32)font;
        NuQFntAtlasReadPS(font, texture_id, flags, plane, buffer, end);
        return font;
    }

    void PCGraphicsDestroyPostEffectTextures(void) {
        NuPostEffectReleasePorts();
        PCGraphicsDestroyRenderTexture(PCGraphicsPostEffectBackBufferTexture);
    }

    void fun_005fb7f0(void) {
    }

    void fun_005fb800(void) {
    }

    /* After a D3D reset, rebinds each live frame texture's resource and clears it to opaque black. */
    /* The colour+depth copy has no PC implementation; its only call is to the shared empty function. */
    void NuPostFilterGen::copy(GRAPHICS_RENDER_TEXTURE *, GRAPHICS_RENDER_TEXTURE *, PC_FRAMEBUFFER *) {
        NuQFntAtlasError();
    }

    /* Non-virtual post-filter resize members; NuPostEffectResize calls them after a device reset. */
    void NuMainFilter::ResizeTextures(i32, i32) {
    }

    void PC_EFFECT_SURFACE::Reset() {
        if (texture != NULL && texture->surface != NULL) {
            surface = NULL;
        }
        if (surface != NULL) {
            surface->methods->release(surface);
            surface = NULL;
        }
        if (texture != NULL && owns_texture) {
            PCGraphicsDestroyRenderTexture(texture);
        }
        texture = NULL;
        owns_texture = 0;
        level = 0;
        face = 0;
        height = width = 0;
    }

    void PC_EFFECT_SURFACE::Attach(GRAPHICS_RENDER_TEXTURE *new_texture, i32 new_level, bool own) {
        if (texture == new_texture && level == new_level) {
            return;
        }
        Reset();
        texture = new_texture;
        owns_texture = own;
        surface = NULL;
        level = (u16)new_level;
        face = 0;
        height = width = 0;
        if (new_texture == NULL) {
            return;
        }
        if (new_texture->surface != NULL) {
            surface = new_texture->surface;
            width = new_texture->width;
            height = new_texture->height;
        } else {
            PC_TEXTURE_LEVEL_DESC description;
            new_texture->resource->methods->get_surface_level(new_texture->resource, level, &surface);
            texture->resource->methods->get_level_desc(texture->resource, level, &description);
            width = (u16)description.width;
            height = (u16)description.height;
        }
    }

    void PC_EFFECT_SURFACE::CopyFrom(PC_EFFECT_SURFACE *source) {
        bool resize = width != source->width || height != source->height;
        PCDeviceStretchRect(source->surface, NULL, surface, NULL, resize ? 2 : 0);
    }

    void PC_EFFECT_SURFACE::ReleaseResources() {
        for (u32 index = 0; index != sizeof(PcSurfaceRegistry) / sizeof(PcSurfaceRegistry[0]); ++index) {
            PC_EFFECT_SURFACE *item = PcSurfaceRegistry[index];
            if (item != NULL && item->surface != NULL) {
                if (item->texture->surface == NULL) {
                    item->surface->methods->release(item->surface);
                }
                PcSurfaceRegistry[index]->surface = NULL;
            }
        }
    }

    void PC_EFFECT_SURFACE::RestoreResources() {
        for (u32 index = 0; index != sizeof(PcSurfaceRegistry) / sizeof(PcSurfaceRegistry[0]); ++index) {
            if (PcSurfaceRegistry[index] != NULL) {
                if (PcSurfaceRegistry[index]->texture != NULL) {
                    if (PcSurfaceRegistry[index]->texture->surface != NULL) {
                        PcSurfaceRegistry[index]->surface = PcSurfaceRegistry[index]->texture->surface;
                        PcSurfaceRegistry[index]->width = PcSurfaceRegistry[index]->texture->width;
                        PcSurfaceRegistry[index]->height = PcSurfaceRegistry[index]->texture->height;
                    } else {
                        PC_TEXTURE_LEVEL_DESC description;
                        PcSurfaceRegistry[index]->texture->resource->methods->get_surface_level(PcSurfaceRegistry[index]->texture->resource,
                                                                                                PcSurfaceRegistry[index]->level,
                                                                                                &PcSurfaceRegistry[index]->surface);
                        PcSurfaceRegistry[index]->texture->resource->methods->get_level_desc(PcSurfaceRegistry[index]->texture->resource,
                                                                                             PcSurfaceRegistry[index]->level, &description);
                        PcSurfaceRegistry[index]->width = (u16)description.width;
                        PcSurfaceRegistry[index]->height = (u16)description.height;
                    }
                } else {
                    PcSurfaceRegistry[index]->surface = NULL;
                }
            }
        }
    }

    void NuDeferredFilter::initTextureResources(i32 width, i32 height) {
        NuDeferredFilterGen::initTextureResources(width, height);
    }

    void NuDeferredFilter::ResizeTextures(i32, i32) {
        if (textures[2] != NULL) {
            PCGraphicsResizeEffectTexture(textures[2], -1, -1, -1);
        }
        if (textures[0] != NULL) {
            PCGraphicsResizeEffectTexture(textures[0], -1, -1, -1);
        }
        for (i32 i = 0; i < 2; ++i) {
            if (textures[4 + i] != NULL) {
                PCGraphicsResizeEffectTexture(textures[4 + i], textures[2]->width, textures[2]->height, -1);
            }
        }
    }

    void NuMotionAccumFilter::initResources() {
        NuMotionAccumFilterGen::initResources();
        program = NULL;
    }

    void NuSpeedBlurFilter::initTextureResources(i32, i32) {
        texture_surface.Attach(PCGraphicsPostEffectBackBufferTexture, 0, false);
    }

    void NuSpeedBlurFilter::destroyTextureResources() {
        texture_surface.Reset();
    }

    void NuSpeedBlurFilter::ResizeTextures(i32, i32) {
    }

    i32 PcInitializeDynamicGeometryBuffers(PC_DYNAMIC_GEOMETRY *geometry, i32 count) {
        geometry->vertex_buffer = NULL;
        PCDeviceCreateVertexBuffer((u32)count * 0x50, 0x208, 0, 0, &geometry->vertex_buffer);
        i32 segments = geometry->format ? geometry->format->records[geometry->format->record_count - 1].strip_segments : 5;
        i32 vertices = (i32)((u32)segments * 2 + 2);
        PCDeviceCreateVertexBuffer((u32)vertices * sizeof(PC_GEOMETRY_STRIP_VERTEX), 0, 0, 0, &geometry->strip_vertex_buffer);
        PC_GEOMETRY_STRIP_VERTEX *strip = NULL;
        PCDeviceLockVertexBuffer(0, 0, (void **)&strip, 0, geometry->strip_vertex_buffer);
        for (i32 i = 0; i < vertices; ++i) {
            strip[i].fraction = (f32)(i - 1) / (f32)(vertices - 3);
            if (strip[i].fraction < 0.0f) {
                strip[i].fraction = 0.0f;
            }
            if (strip[i].fraction > 1.0f) {
                strip[i].fraction = 1.0f;
            }
            strip[i].side = (i & 1) == 0 ? 0.5f : -0.5f;
        }
        PCDeviceUnlockVertexBuffer(geometry->strip_vertex_buffer);
        PCDeviceCreateIndexBuffer((u32)vertices * sizeof(u16), 0, 0x65, 0, &geometry->strip_index_buffer, NULL);
        u16 *indices;
        geometry->strip_index_buffer->methods->lock(geometry->strip_index_buffer, 0, 0, (void **)&indices, 0);
        for (i32 i = 0; i < vertices; ++i) {
            indices[i] = (u16)i;
        }
        geometry->strip_index_buffer->methods->unlock(geometry->strip_index_buffer);
        return 1;
    }

    i32 PCDynamicGeometryLock(PC_DYNAMIC_GEOMETRY *geometry, void **data) {
        PCDeviceLockVertexBuffer(0, 0, data, 0x2000, geometry->vertex_buffer);
        return 1;
    }

    i32 PCDynamicGeometryUnlock(PC_DYNAMIC_GEOMETRY *geometry) {
        PCDeviceUnlockVertexBuffer(geometry->vertex_buffer);
        return 1;
    }

    i32 PCDynamicGeometryRelease(PC_DYNAMIC_GEOMETRY *geometry) {
        if (geometry->strip_index_buffer != NULL) {
            geometry->strip_index_buffer->methods->release(geometry->strip_index_buffer);
        }
        if (geometry->strip_vertex_buffer != NULL) {
            geometry->strip_vertex_buffer->methods->release(geometry->strip_vertex_buffer);
        }
        if (geometry->vertex_buffer != NULL) {
            geometry->vertex_buffer->methods->release(geometry->vertex_buffer);
        }
        return 1;
    }

    u8 PcTransformDynamicGeometry(PC_GEOMETRY_SKIN *skin, PC_GEOMETRY_MESH *mesh, u32 *output_index) {
        for (u32 group_index = 0; group_index < mesh->group_count; ++group_index) {
            PC_GEOMETRY_GROUP *group = &mesh->groups[group_index];
            PC_GEOMETRY_BASIS *basis = (PC_GEOMETRY_BASIS *)&PcGeometryTransforms[group->transform_index * 3];
            for (u32 element = 0; element < group->element_count; ++element) {
                PC_GEOMETRY_INPUT *input = &mesh->elements[group->first_element + element];
                PC_GEOMETRY_OUTPUT *output = &PcGeometryOutput[*output_index];
                PC_GEOMETRY_BASIS_VECTOR p00;
                PC_GEOMETRY_BASIS_VECTOR p01;
                PC_GEOMETRY_BASIS_VECTOR p02;
                PC_GEOMETRY_BASIS_VECTOR partial0;
                PC_GEOMETRY_BASIS_VECTOR row0;
                {
                    f32 scale = input->basis_weights.z;
                    p02.x = basis->matrices[2].rows[0].x * scale;
                    p02.y = basis->matrices[2].rows[0].y * scale;
                    p02.z = basis->matrices[2].rows[0].z * scale;
                }
                {
                    f32 scale = input->basis_weights.y;
                    p01.x = basis->matrices[1].rows[0].x * scale;
                    p01.y = basis->matrices[1].rows[0].y * scale;
                    p01.z = basis->matrices[1].rows[0].z * scale;
                }
                {
                    f32 scale = input->basis_weights.x;
                    p00.x = basis->matrices[0].rows[0].x * scale;
                    p00.y = basis->matrices[0].rows[0].y * scale;
                    p00.z = basis->matrices[0].rows[0].z * scale;
                }
                partial0.x = p00.x + p01.x;
                partial0.y = p00.y + p01.y;
                partial0.z = p00.z + p01.z;
                row0.x = partial0.x + p02.x;
                row0.y = partial0.y + p02.y;
                row0.z = partial0.z + p02.z;
                PC_GEOMETRY_BASIS_VECTOR p10;
                PC_GEOMETRY_BASIS_VECTOR p11;
                PC_GEOMETRY_BASIS_VECTOR p12;
                PC_GEOMETRY_BASIS_VECTOR partial1;
                PC_GEOMETRY_BASIS_VECTOR row1;
                {
                    f32 scale = input->basis_weights.z;
                    p12.x = basis->matrices[2].rows[1].x * scale;
                    p12.y = basis->matrices[2].rows[1].y * scale;
                    p12.z = basis->matrices[2].rows[1].z * scale;
                }
                {
                    f32 scale = input->basis_weights.y;
                    p11.x = basis->matrices[1].rows[1].x * scale;
                    p11.y = basis->matrices[1].rows[1].y * scale;
                    p11.z = basis->matrices[1].rows[1].z * scale;
                }
                {
                    f32 scale = input->basis_weights.x;
                    p10.x = basis->matrices[0].rows[1].x * scale;
                    p10.y = basis->matrices[0].rows[1].y * scale;
                    p10.z = basis->matrices[0].rows[1].z * scale;
                }
                partial1.x = p10.x + p11.x;
                partial1.y = p10.y + p11.y;
                partial1.z = p10.z + p11.z;
                row1.x = partial1.x + p12.x;
                row1.y = partial1.y + p12.y;
                row1.z = partial1.z + p12.z;
                PC_GEOMETRY_BASIS_VECTOR p20;
                PC_GEOMETRY_BASIS_VECTOR p21;
                PC_GEOMETRY_BASIS_VECTOR p22;
                PC_GEOMETRY_BASIS_VECTOR partial2;
                PC_GEOMETRY_BASIS_VECTOR row2;
                {
                    f32 scale = input->basis_weights.z;
                    p22.x = basis->matrices[2].rows[2].x * scale;
                    p22.y = basis->matrices[2].rows[2].y * scale;
                    p22.z = basis->matrices[2].rows[2].z * scale;
                }
                {
                    f32 scale = input->basis_weights.y;
                    p21.x = basis->matrices[1].rows[2].x * scale;
                    p21.y = basis->matrices[1].rows[2].y * scale;
                    p21.z = basis->matrices[1].rows[2].z * scale;
                }
                {
                    f32 scale = input->basis_weights.x;
                    p20.x = basis->matrices[0].rows[2].x * scale;
                    p20.y = basis->matrices[0].rows[2].y * scale;
                    p20.z = basis->matrices[0].rows[2].z * scale;
                }
                partial2.x = p20.x + p21.x;
                partial2.y = p20.y + p21.y;
                partial2.z = p20.z + p21.z;
                row2.x = partial2.x + p22.x;
                row2.y = partial2.y + p22.y;
                row2.z = partial2.z + p22.z;
                PC_GEOMETRY_BASIS_VECTOR cross;
                NuVecCross(&cross.xyz, &row1.xyz, &row2.xyz);
                PC_GEOMETRY_CORNER *corners = output->corners;
                if (input->skin_indices[0] >= 0 && skin->matrix_group_count > 1) {
                    corners[0] = input->corners[0];
                    for (i32 corner = 1; corner < 4; ++corner) {
                        NUVEC delta;
                        NuVecSub(&delta, &input->corners[corner].position, &input->corners[corner - 1].position);
                        corners[corner] = corners[corner - 1];
                        for (i32 j = 0; j < 3; ++j) {
                            i32 index = input->skin_indices[j];
                            if (index >= 0 && index < (i32)skin->matrix_group_count) {
                                NUVEC transformed;
                                f32 weight = (f32)((f64)input->skin_weights[j] * (f64)(1.0f / 255.0f));
                                NuVecMtxTransform(&transformed, &delta, &skin->matrices[index * 3 + corner - 1]);
                                NuVecScale(&transformed, &transformed, weight);
                                NuVecAdd(&corners[corner].position, &corners[corner].position, &transformed);
                            }
                        }
                    }
                } else {
                    for (i32 corner = 0; corner < 4; ++corner) {
                        corners[corner] = input->corners[corner];
                    }
                }
                NUMTX matrix;
                matrix.m00 = row2.x;
                matrix.m01 = row2.y;
                matrix.m02 = row2.z;
                matrix.m03 = 0;
                matrix.m10 = cross.x;
                matrix.m11 = cross.y;
                matrix.m12 = cross.z;
                matrix.m13 = 0;
                matrix.m20 = row1.x;
                matrix.m21 = row1.y;
                matrix.m22 = row1.z;
                matrix.m23 = 0;
                matrix.m30 = row0.x;
                matrix.m31 = row0.y;
                matrix.m32 = row0.z;
                matrix.m33 = 1;
                for (i32 corner = 0; corner < 4; ++corner) {
                    NuVecMtxTransform(&corners[corner].position, &corners[corner].position, &matrix);
                }
                output->basis_axis.x = row1.x;
                output->basis_axis.y = row1.y;
                output->basis_axis.z = row1.z;
                output->distance_squared = 0;
                for (i32 corner = 0; corner < 4; ++corner) {
                    corners[corner].w = input->output_w[corner];
                }
                ++*output_index;
            }
        }
        return 1;
    }

    i32 PcUpdateGeometryDistances(PC_GEOMETRY_OUTPUT *output, PC_GEOMETRY_SORT_ITEM *items, i32 count) {
        PC_GEOMETRY_BASIS_VECTOR origin;
        origin.x = PcGeometryDistanceOrigin.x;
        origin.y = PcGeometryDistanceOrigin.y;
        origin.z = PcGeometryDistanceOrigin.z;
        for (i32 i = 0; i < count; ++i) {
            PC_GEOMETRY_BASIS_VECTOR delta;
            NUVEC *position = &output[i].corners[2].position;
            delta.x = origin.x - position->x;
            delta.y = origin.y - position->y;
            delta.z = origin.z - position->z;
            output[i].distance_squared = Nu3dPcIncludesVuVecLengthSq(*(VuVec *)&delta);
        }
        for (i32 i = 0; i < count; ++i) {
            items[i].distance_squared = output[items[i].output_index].distance_squared;
        }
        return 1;
    }

    i32 PCGeometrySortItems(PC_GEOMETRY_SORT_ITEM *items, PC_GEOMETRY_OUTPUT *output, i32 left, i32 right) {
        PC_GEOMETRY_SORT_ITEM pivot;
        PC_GEOMETRY_SORT_ITEM swap;
        i32 first = left;
        i32 last = right;
        i32 middle = first + (last - first) / 2;
        swap = items[left];
        items[left] = items[middle];
        items[middle] = swap;
        pivot = items[left];
        while (left < right) {
            while (items[right].distance_squared < pivot.distance_squared && left < right) {
                right--;
            }
            if (left != right) {
                swap = items[left];
                items[left] = items[right];
                items[right] = swap;
                left++;
            }
            while (items[left].distance_squared > pivot.distance_squared && left < right) {
                left++;
            }
            if (left != right) {
                swap = items[right];
                items[right] = items[left];
                items[left] = swap;
                right--;
            }
            items[left] = pivot;
        }
        if (first < left) {
            PCGeometrySortItems(items, output, first, left - 1);
        }
        if (last > left) {
            PCGeometrySortItems(items, output, left + 1, last);
        }
        return 1;
    }

    i32 PCDynamicGeometryCopyOutput(PC_DYNAMIC_GEOMETRY *geometry, PC_GEOMETRY_OUTPUT *output, i32 count) {
        if (dat_00806b94 == 1) {
            for (i32 i = 0; i < count; i++) {
                output[i] = PcGeometryOutput[geometry->sort_items[i].output_index];
            }
        }
        return 1;
    }

    NUDISPLAYLIST *NuDisplayList2dGetPlatform(void) {
        return &PCDisplayList2d;
    }

    void NuDisplayListStreamReset(void) {
        NuDisplayListStreamCursor = &nudlist_stream_cursor;
        nudlist_field_1598 = nuprim_field_ba8;
    }

    VARIPTR *NuDisplayListStreamAlign(void) {
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15;
        return NuDisplayListStreamCursor;
    }

    void NuDisplayListCheckStreamOverflow(void) {
        if (NuDisplayListStreamCursor->addr > nudlist_field_1598) {
            NuPtlDmaSlotsExhausted();
        }
    }

    i32 NuRndrSetAmbientLightPS(const NUVEC *colour) {
        render_state.ambient_intensity.x = colour->x;
        render_state.ambient_intensity.y = colour->y;
        render_state.ambient_intensity.z = colour->z;
        ++render_state.global_id;
        ++render_state.lights_id;
        render_state.light_packet_cache = NULL;
        return 1;
    }

    i32 NuRndrSetDirectionalLightsPS(const NUVEC *d0, const NUVEC *c0, const NUVEC *d1, const NUVEC *c1, const NUVEC *d2, const NUVEC *c2) {
        NUMTX *view = NuCameraGetViewMtx();
        render_state.light_intensity[0] = *c0;
        render_state.light_intensity[1] = *c1;
        render_state.light_intensity[2] = *c2;
        NuVecMtxRotate(&render_state.light_direction[0], (NUVEC *)d0, view);
        NuVecMtxRotate(&render_state.light_direction[1], (NUVEC *)d1, view);
        NuVecMtxRotate(&render_state.light_direction[2], (NUVEC *)d2, view);
        NuVecNorm(&render_state.light_direction[0], &render_state.light_direction[0]);
        NuVecNorm(&render_state.light_direction[1], &render_state.light_direction[1]);
        NuVecNorm(&render_state.light_direction[2], &render_state.light_direction[2]);
        ++render_state.global_id;
        ++render_state.lights_id;
        render_state.light_packet_cache = NULL;
        return 1;
    }

    i32 NuRndrStateSetSpecularLight(const NUMTX *matrix, const NUVEC *colour) {
        render_state.specular_mtx = *matrix;
        render_state.specular_colour = *colour;
        ++render_state.global_id;
        ++render_state.lights_id;
        render_state.light_packet_cache = NULL;
        return 1;
    }

    void NuRndrStateSetSpecularLightEx(const NUVEC *direction, const NUMTX *matrix, const NUVEC *colour) {
        render_state.specular_mtx = *matrix;
        render_state.specular_colour = *colour;
        render_state.specular_direction = *direction;
        ++render_state.global_id;
        ++render_state.lights_id;
        render_state.light_packet_cache = NULL;
    }

    i32 NuRndrStateUpdateCameraState(void) {
        NUMTX *projection = NuCameraGetProjectionMtx();
        NUMTX *view = NuCameraGetViewMtx();
        f64 horizontal;
        f64 vertical;
        render_state.camera_matrix = *view;
        render_state.projection_0154 = projection->m00;
        render_state.projection_0158 = projection->m11;
        render_state.projection_m22 = projection->m22;
        render_state.projection_0160 = projection->m23;
        render_state.projection_0164 = projection->m32;
        NuVpGetPosition2(&render_state.viewport_x, &render_state.viewport_y);
        NuVpGetSize2(&render_state.viewport_width, &render_state.viewport_height);
        horizontal = (f64)PS2_REZ_W / 640.0;
        render_state.viewport_x = (f32)(render_state.viewport_x * horizontal);
        render_state.viewport_width = (f32)(render_state.viewport_width * horizontal);
        vertical = (f64)PS2_REZ_H / 224.0;
        render_state.viewport_y = (f32)(render_state.viewport_y * vertical);
        render_state.viewport_height = (f32)(render_state.viewport_height * vertical);
        render_state.camera_packet_cache = NULL;
        ++render_state.global_id;
        ++render_state.camera_id;
        return 1;
    }

    void NuRndrStateSetFogEnabled(u32 enabled) {
        render_state.fog_state_word = enabled;
        ++render_state.global_id;
        ++render_state.fog_id;
        render_state.fog_packet_cache = NULL;
    }

    void NuRndrStateSetFogState(f32 near_distance, f32 far_distance, u32 colour, f32 density) {
        render_state.fog_value_0170 = near_distance;
        render_state.fog_value_0174 = far_distance;
        render_state.fog_pointer_0178 = colour;
        render_state.fog_density = density;
        render_state.fog_packet_cache = NULL;
        ++render_state.global_id;
        ++render_state.fog_id;
    }

    u32 NuRndrStateGetFogEnabled(void) {
        return render_state.fog_state_word;
    }

    void *DisplayListBuildLightPacket(NUGLOBALRNDRSTATE *state) {
        VARIPTR *cursor;
        NUPRIM_LIGHT_PACKET *packet;
        i32 i;
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
        cursor = NuDisplayListStreamCursor;
        packet = (NUPRIM_LIGHT_PACKET *)cursor->void_ptr;
        cursor->void_ptr = packet + 1;
        packet->ambient_intensity.x = state->ambient_intensity.x;
        packet->ambient_intensity.y = state->ambient_intensity.y;
        packet->ambient_intensity.z = state->ambient_intensity.z;
        packet->ambient_intensity.w = 1.0f;
        for (i = 0; i < 3; ++i) {
            packet->light_intensity[i].x = state->light_intensity[i].x;
            packet->light_intensity[i].y = state->light_intensity[i].y;
            packet->light_intensity[i].z = state->light_intensity[i].z;
            packet->light_intensity[i].w = 1.0f;
            packet->light_direction[i].x = state->light_direction[i].x;
            packet->light_direction[i].y = state->light_direction[i].y;
            packet->light_direction[i].z = state->light_direction[i].z;
            packet->light_direction[i].w = 1.0f;
        }
        packet->specular_mtx = state->specular_mtx;
        packet->specular_colour = state->specular_colour;
        return packet;
    }

    void *DisplayListBuildKonstPacket(NUGLOBALRNDRSTATE *state) {
        VARIPTR *cursor;
        f32 *packet;
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
        cursor = NuDisplayListStreamCursor;
        packet = cursor->f32_ptr;
        if (state->const_alpha_enabled != 0) {
            *packet = state->const_alpha;
        } else {
            *packet = 1.0f;
        }
        cursor->void_ptr = packet + 1;
        return packet;
    }

    void *DisplayListBuildReflectionPacket(NUGLOBALRNDRSTATE *state) {
        VARIPTR *cursor;
        u32 *packet;
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
        cursor = NuDisplayListStreamCursor;
        packet = cursor->u32_ptr;
        *packet = state->reflection_state;
        cursor->addr += sizeof(*packet);
        return packet;
    }

    void *DisplayListBuildFogPacket(NUGLOBALRNDRSTATE *state) {
        VARIPTR *cursor;
        void *packet;
        f32 *field;
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
        cursor = NuDisplayListStreamCursor;
        packet = cursor->void_ptr;
        *cursor->u32_ptr = state->fog_state_word;
        cursor->addr += sizeof(u32);
        if (state->fog_state_word != 0) {
            *cursor->u32_ptr = state->fog_pointer_0178;
            cursor->addr += sizeof(u32);
            field = cursor->f32_ptr;
            *field = state->fog_value_0170;
            cursor->void_ptr = field + 1;
            field = cursor->f32_ptr;
            *field = state->fog_value_0174;
            cursor->void_ptr = field + 1;
            field = cursor->f32_ptr;
            *field = state->fog_density;
            cursor->void_ptr = field + 1;
        }
        return packet;
    }

    void *DisplayListBuildVertexGroupPacket(NUGLOBALRNDRSTATE *unused) {
        VARIPTR *cursor;
        u32 *packet;
        f32 *output;
        i8 *values;
        i32 i;

        if (NuPrimVertexGroupSource != NULL) {
            NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
            cursor = NuDisplayListStreamCursor;
            packet = cursor->u32_ptr;
            *packet = NuPrimVertexGroupSource->count;
            cursor->addr += sizeof(u32);
            *cursor->u32_ptr = NuPrimVertexGroupSource->flags;
            cursor->addr += sizeof(u32);
            output = cursor->f32_ptr;
            cursor->addr += ((NuPrimVertexGroupSource->count + 3) / 4) * 0x10;
            values = NuPrimVertexGroupSource->values;
            for (i = 0; i < NuPrimVertexGroupSource->count; ++i) {
                *output++ = (f32)*values++;
            }
            return packet;
        }
        return NULL;
    }

    i32 *NuDisplayListWriteVertexOffsets(void) {
        VARIPTR *cursor;
        i32 *packet;
        if (nuspecial_vertex_noffsets > 0) {
            NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15;
            cursor = NuDisplayListStreamCursor;
            packet = cursor->i32_ptr;
            *packet = nuspecial_vertex_noffsets;
            cursor->addr += 4;
            PCByteCopy(cursor->void_ptr, nuspecial_vertex_offsets, nuspecial_vertex_noffsets * 16);
            cursor->addr += nuspecial_vertex_noffsets * 16;
            return packet;
        }
        return NULL;
    }

    void RndrStateCopyGlobalState(NUGLOBALRNDRSTATE *state) {
        memcpy(state, &render_state, sizeof(*state));
        state->fog_packet_cache = NULL;
        state->camera_packet_cache = NULL;
        state->light_packet_cache = NULL;
        state->konst_packet_cache = NULL;
    }

    void RndrStateResetGlobalState(NUGLOBALRNDRSTATE *state) {
        state->const_alpha_enabled = 0;
        state->state_0100 = 0;
        state->fog_packet_cache = NULL;
        state->light_packet_cache = NULL;
        state->camera_packet_cache = NULL;
        state->konst_packet_cache = NULL;
        state->reflection_packet_cache = NULL;

        render_state.mtl = 0;
        render_state.tex_id = -1;
        render_state.global_id = -1;
        memset(&render_state.lights_id, 0xff, sizeof(render_state.lights_id));
        memset(&render_state.camera_id, 0xff, sizeof(render_state.camera_id));
        memset(&render_state.fog_id, 0xff, sizeof(render_state.fog_id));
        memset(&render_state.konst_id, 0xff, sizeof(render_state.konst_id));
    }

    void RndrStateResetSharedGlobalState(void) {
        render_state.const_alpha_enabled = 0;
        render_state.state_0100 = 0;
        render_state.fog_packet_cache = NULL;
        render_state.light_packet_cache = NULL;
        render_state.camera_packet_cache = NULL;
        render_state.konst_packet_cache = NULL;
        render_state.reflection_packet_cache = NULL;
        render_state.mtl = 0;
        render_state.tex_id = -1;
        render_state.global_id = -1;
        memset(&render_state.lights_id, 0xff, sizeof(render_state.lights_id));
        memset(&render_state.camera_id, 0xff, sizeof(render_state.camera_id));
        memset(&render_state.fog_id, 0xff, sizeof(render_state.fog_id));
        memset(&render_state.konst_id, 0xff, sizeof(render_state.konst_id));
    }

    NUGLOBALRNDRSTATE *NuRndrGetGlobalState(void) {
        return &render_state;
    }

    void NuRndrStateInit(void) {
        memset(&render_state, 0, sizeof(render_state));
        NuMtxSetIdentity(&RndrStateTransform);
        NuMtxSetIdentity(&RndrStateScaledTransform);
        RndrStateTransformScale = 1.0f;
        ++render_state.global_id;
        ++render_state.fog_id;
        render_state.fog_state_word = 0;
        render_state.fog_packet_cache = NULL;
        NuRndrStateUpdateCameraState();
    }

    void RndrStateSetConstAlphaTint(i32 alpha_enabled, i32 tint_enabled, f32 alpha, const NUVEC *tint, NUMTL *material) {
        render_state.const_alpha = alpha;
        render_state.const_alpha_material = material;
        ++render_state.global_id;
        ++render_state.konst_id;
        render_state.const_alpha_enabled = alpha_enabled;
        render_state.konst_packet_cache = NULL;
    }

    void NuRndrStateGetTransform(NUMTX *output, const NUMTL *material) {
        const NUMTL *selected = material;
        f32 scale;
        NUVEC scaling;
        if (selected->next != NULL) {
            selected = selected->next;
        }
        if (selected->material_kind != 0x81 && selected->material_kind != 0x85) {
            *output = RndrStateTransform;
            return;
        }
        scale = selected->transform_scale != 0.0f ? selected->transform_scale : 1.0f;
        if (scale != RndrStateTransformScale) {
            RndrStateTransformScale = scale;
            RndrStateScaledTransform = RndrStateTransform;
            if (scale != 1.0f) {
                scaling.z = scale;
                scaling.y = scale;
                scaling.x = scale;
                NuMtxPreScale(&RndrStateScaledTransform, &scaling);
            }
        }
        *output = RndrStateScaledTransform;
    }

    void NuRndrStateSetTransform(const NUMTX *matrix) {
        f32 temporary;
        RndrStateTransformScale = 1.0f;
        RndrStateTransform = *matrix;
        temporary = RndrStateTransform.m01;
        RndrStateTransform.m01 = RndrStateTransform.m10;
        RndrStateTransform.m10 = temporary;
        temporary = RndrStateTransform.m02;
        RndrStateTransform.m02 = RndrStateTransform.m20;
        RndrStateTransform.m20 = temporary;
        temporary = RndrStateTransform.m21;
        RndrStateTransform.m21 = RndrStateTransform.m12;
        RndrStateTransform.m12 = temporary;
        RndrStateScaledTransform = RndrStateTransform;
    }

    void RndrStateSetReflection(i32 reflection) {
        render_state.reflection_state = (u32)reflection;
        ++render_state.global_id;
        ++render_state.reflection_id;
        render_state.reflection_packet_cache = NULL;
    }

    static void NuRndrResetStreamBuffer(void) {
        switch (rndrstream_buffid) {
            case 0:
                nudlist_stream_cursor = rndrstream_b;
                nuprim_field_ba8 = rndrstream_b.addr + nurndr_maxstreamsize;
                break;
            case 1:
                nudlist_stream_cursor = rndrstream_a;
                nuprim_field_ba8 = rndrstream_a.addr + nurndr_maxstreamsize;
                break;
        }
        if (nudlist_stream_cursor.addr & 15) {
            nudlist_stream_cursor.addr = (nudlist_stream_cursor.addr + 15) & ~15u;
        }
        SAGA_SKIN_CALL(skin_trace_stream, (unsigned)rndrstream_buffid, nudlist_stream_cursor.addr,
                       nuprim_field_ba8 - nudlist_stream_cursor.addr);
    }

    void NuRndrSwapStreamBuffers(void) {
        switch (rndrstream_buffid) {
            case 0:
                rndrstream_used = nudlist_stream_cursor.addr - rndrstream_b.addr;
                break;
            case 1:
                rndrstream_used = nudlist_stream_cursor.addr - rndrstream_a.addr;
                break;
        }
        if (rndrstream_used > rndrstream_max_used) {
            rndrstream_max_used = rndrstream_used;
        }
        if (rndrstream_used > (u32)nurndr_maxstreamsize) {
            PCGraphicsRenderTextureAllocationError();
            DebugBreak();
        }
        rndrstream_buffid = (rndrstream_buffid + 1) % 2;
        NuRndrResetStreamBuffer();
    }

    void NuRndrStreamInit(i32 size, VARIPTR *buffer) {
        if (buffer == NULL) {
            rndrstream_b.void_ptr = NuAlloc(size, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nustream.c", 126);
            rndrstream_a.void_ptr = NuAlloc(size, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nustream.c", 127);
        } else {
            rndrstream_b.addr = (buffer->addr + 31) & ~31u;
            rndrstream_a.addr = (rndrstream_b.addr + size + 31) & ~31u;
            buffer->addr = rndrstream_a.addr + size;
        }
        nurndr_maxstreamsize = size;
        rndrstream_buffid = 0;
        memset(rndrstream_b.void_ptr, 0, size);
        memset(rndrstream_a.void_ptr, 0, size);
        NuRndrResetStreamBuffer();
    }

    void NuDisplayListDrawPrimitive2D(void *data) {
        DISPLAY_PRIMITIVE_PACKET *packet = (DISPLAY_PRIMITIVE_PACKET *)data;
        u32 count;
        const void *vertices;
        u32 point_size;
        f32 point_size_float;
        if (packet == NULL || PCShaderCurrentObject() == NULL || packet->vertex_count == 0) {
            return;
        }
        count = packet->vertex_count;
        vertices = packet + 1;
        switch (packet->type) {
            case 12:
            case 16:
                if (count > 2) {
                    PCDeviceDrawPrimitiveUPPc(4, count / 3, vertices, 24);
                }
                break;
            case 15:
                if (count > 1) {
                    PCDeviceDrawPrimitiveUPPc(3, count - 1, vertices, 24);
                }
                break;
            case 13:
                if (count > 2) {
                    PCDeviceDrawPrimitiveUPPc(5, count - 2, vertices, 24);
                }
                break;
            case 14:
                if (count > 1) {
                    PCDeviceDrawPrimitiveUPPc(2, count / 2, vertices, 24);
                }
                break;
            case 18:
                point_size_float = 32.0f;
                memcpy(&point_size, &point_size_float, sizeof(point_size));
                PCDeviceSetRenderState(D3DRS_POINTSIZE, point_size);
                PCDeviceDrawPrimitiveUPPc(1, packet->vertex_count, vertices, 24);
                point_size_float = 1.0f;
                memcpy(&point_size, &point_size_float, sizeof(point_size));
                PCDeviceSetRenderState(D3DRS_POINTSIZE, point_size);
                break;
        }
    }

    void NuDisplayHandlerSetShadow(f32 shadow) {
        if (shadow != PCGraphicsFrameState) {
            NuDisplayHandlerShadow = shadow;
            NuShaderManagerSetfv(SHADER_UNIFORM_SHADOW_CASTING_OBJECT, &NuDisplayHandlerShadow);
            PCGraphicsFrameState = shadow;
        }
    }

    void NuDisplayListSkinPalette(void *arg) {
        PC_DL_SKIN_PALETTE_PACKET *packet = (PC_DL_SKIN_PALETTE_PACKET *)arg;
        i32 count = packet->count;
        void *elements = packet->elements;
        NuShaderObject *shader = PCShaderCurrentObject();
        if (shader != NULL) {
            if (count > 8) {
                NuPtlDmaSlotsExhausted();
            }
            NuShaderObjectSetElementsfv(shader, SHADER_UNIFORM_SKIN_MATRIX, 0, count, (const f32 *)elements);
            SAGA_SKIN_CALL(skin_trace_palette, (unsigned)arg, count);
        }
    }

    void NuDisplayListDrawFaceOn(void *arg) {
        PC_DL_FACEON_DRAW_PACKET *packet = (PC_DL_FACEON_DRAW_PACKET *)arg;
        if (packet == NULL) {
            return;
        }
        PCDeviceSetStreamSource(0, packet->vertex_buffer, 0, 32);
        PCDeviceDrawPrimitive(4, packet->first_vertex, packet->primitive_count * 2u);
    }

    void NuDisplayListLights(void *arg) {
        PC_DL_LIGHTS_PACKET *packet = (PC_DL_LIGHTS_PACKET *)arg;
        if (PCDeviceHardwareVertexProcessingPC != PC_SHADER_MODEL_3_0) {
            NuShaderManagerSetfv(SHADER_UNIFORM_SCENE_AMBIENT_COLOR, &packet->ambient.x);
            NuShaderManagerSetfv(SHADER_UNIFORM_LIGHT_POSITION0, &packet->direction[0].x);
            NuShaderManagerSetfv(SHADER_UNIFORM_LIGHT_POSITION1, &packet->direction[1].x);
            NuShaderManagerSetfv(SHADER_UNIFORM_LIGHT_POSITION2, &packet->direction[2].x);
            NuShaderManagerSetfv(SHADER_UNIFORM_LIGHT_COLOR0, &packet->intensity[0].x);
            NuShaderManagerSetfv(SHADER_UNIFORM_LIGHT_COLOR1, &packet->intensity[1].x);
            NuShaderManagerSetfv(SHADER_UNIFORM_LIGHT_COLOR2, &packet->intensity[2].x);
            NuShaderManagerSetfv(SHADER_UNIFORM_PS2_SHINE_MTX, &packet->shine_matrix.m00);
        } else {
            NuShaderManagerSetfv(SHADER_UNIFORM_LIGHT_PARAMS, &packet->ambient.x);
        }
        if (PCDeviceHardwareVertexProcessingPC != PC_SHADER_MODEL_3_0) {
            PC_DL_NORMALIZED_DIRECTION direction;

            NuVec4Add(&direction.four, &packet->direction[0], &packet->direction[1]);
            NuVec4Add(&direction.four, &direction.four, &packet->direction[2]);
            NuVecNorm(&direction.normalized.three, &direction.normalized.three);
            NuShaderManagerSetfv(SHADER_UNIFORM_AVERAGE_LIGHT_DIR, &direction.four.x);
            direction.four.x =
                packet->intensity[0].x > (packet->intensity[1].x > packet->intensity[2].x ? packet->intensity[1].x : packet->intensity[2].x)
                    ? packet->intensity[0].x
                    : (packet->intensity[1].x > packet->intensity[2].x ? packet->intensity[1].x : packet->intensity[2].x);
            direction.four.y =
                packet->intensity[0].y > (packet->intensity[1].y > packet->intensity[2].y ? packet->intensity[1].y : packet->intensity[2].y)
                    ? packet->intensity[0].y
                    : (packet->intensity[1].y > packet->intensity[2].y ? packet->intensity[1].y : packet->intensity[2].y);
            direction.four.z =
                packet->intensity[0].z > (packet->intensity[1].z > packet->intensity[2].z ? packet->intensity[1].z : packet->intensity[2].z)
                    ? packet->intensity[0].z
                    : (packet->intensity[1].z > packet->intensity[2].z ? packet->intensity[1].z : packet->intensity[2].z);
            direction.four.w = 1.0f;
            NuShaderManagerSetfv(SHADER_UNIFORM_AVERAGE_LIGHT_COLOR, &direction.four.x);
        }
    }

    void NuDisplayListTint(void *arg) {
        NUVEC4 tint = {1.0f, 1.0f, 1.0f, *(f32 *)arg};
        NuDisplayHandlerTint = tint;
        NuShaderManagerSetfv(SHADER_UNIFORM_K_TINT, &tint.x);
    }

    void NuDisplayListFog(void *arg) {
        PC_DL_FOG_PACKET *packet = (PC_DL_FOG_PACKET *)arg;
        if (packet->enabled != 0) {
            i32 packed = (i32)packet->colour;
            NUVEC4 colour;
            NUVEC4 params;
            colour.x = (f32)((f32)(packed & 255) / 255.0);
            colour.y = (f32)((f32)((packed >> 8) & 255) / 255.0);
            colour.z = (f32)((f32)((packed >> 16) & 255) / 255.0);
            colour.w = (f32)((f32)((packed >> 24) & 255) / 255.0);
            params.x = packet->near_distance;
            params.y = packet->far_distance;
            params.z = packet->far_distance - packet->near_distance;
            params.w = packet->density;
            NuShaderManagerSetfv(SHADER_UNIFORM_FOG_COLOR, &colour.x);
            NuShaderManagerSetfv(SHADER_UNIFORM_FOG_PARAMS, &params.x);
        } else {
            NUVEC4 params = {0.0f, 0.0f, 0.0f, 0.0f};
            NuShaderManagerSetfv(SHADER_UNIFORM_FOG_PARAMS, &params.x);
        }
    }

    void NuDisplayListLightmapPacket(void *arg) {
        PCGraphicsFrameScratchCount.lightmap_packet = arg;
    }

    void NuDisplayListUpdateLightmap(void) {
        PC_DL_LIGHTMAP_UPDATE_PACKET *packet;
        i32 stage;
        f32 offset[3];
        if (PCGraphicsFrameScratchCount.lightmap_packet == NULL ||
            (u32)PCGraphicsFrameScratchCount.lightmap_packet == (u32)PCGraphicsBoundShader) {
            return;
        }
        packet = (PC_DL_LIGHTMAP_UPDATE_PACKET *)PCGraphicsFrameScratchCount.lightmap_packet;
        PCGraphicsBoundShader = (i32)(u32)PCGraphicsFrameScratchCount.lightmap_packet;
        if (packet->mode == 2 && PcWindowOptions.usehires == 0) {
            packet->mode = 1;
        }
        if (packet->mode == 1) {
            NUTEXDIM *texture;
            stage = 0;
            if (PCDeviceHardwareVertexProcessingPC == PC_SHADER_MODEL_1_1) {
                stage = 1;
            }
            if (packet->texture_id != -1) {
                texture = NuTextureGetDimensions(packet->texture_id);
            } else {
                texture = NuTextureGetDimensions(1);
            }
            PCDeviceSetTexture(stage, texture->resource);
            PCDeviceSetSamplerState(stage, D3DSAMP_ADDRESSU, 1);
            PCDeviceSetSamplerState(stage, D3DSAMP_ADDRESSV, 1);
            PCDeviceSetSamplerState(stage, D3DSAMP_MAGFILTER, 2);
            PCDeviceSetSamplerState(stage, D3DSAMP_MINFILTER, 2);
            PCDeviceSetSamplerState(stage, D3DSAMP_MIPFILTER, 0);
            offset[0] = packet->x_offset;
            offset[1] = -packet->y_offset;
            offset[2] = 0.0f;
            NuShaderManagerSetfv(SHADER_UNIFORM_LIGHTMAP_OFFSET, offset);
        } else if (packet->mode == 2) {
            for (stage = 0; stage < 3; ++stage) {
                NUTEXDIM *texture;
                if (packet->texture_ids[stage] != -1) {
                    texture = NuTextureGetDimensions(packet->texture_ids[stage]);
                } else {
                    texture = NuTextureGetDimensions(1);
                }
                PCDeviceSetTexture(stage, texture->resource);
                PCDeviceSetSamplerState(stage, D3DSAMP_ADDRESSU, 1);
                PCDeviceSetSamplerState(stage, D3DSAMP_ADDRESSV, 1);
                PCDeviceSetSamplerState(stage, D3DSAMP_MAGFILTER, 2);
                PCDeviceSetSamplerState(stage, D3DSAMP_MINFILTER, 2);
                PCDeviceSetSamplerState(stage, D3DSAMP_MIPFILTER, 0);
            }
            offset[0] = packet->x_offset;
            offset[1] = -packet->y_offset;
            offset[2] = 0.0f;
            NuShaderManagerSetfv(SHADER_UNIFORM_LIGHTMAP_OFFSET, offset);
        }
    }

    void NuDisplayListLegacyLightmap(void *arg) {
        i32 *texture_ids = (i32 *)arg;
        f32 offset[3] = {0.0f, 0.0f, 0.0f};
        i32 i;
        for (i = 0; i < 3; ++i) {
            NUTEXDIM *texture;
            if (texture_ids[i] != -1) {
                texture = NuTextureGetDimensions(texture_ids[i]);
            } else {
                texture = NuTextureGetDimensions(1);
            }
            PCDeviceSetTexture(i, texture->resource);
            PCDeviceSetSamplerState(i, D3DSAMP_ADDRESSU, 1);
            PCDeviceSetSamplerState(i, D3DSAMP_ADDRESSV, 1);
        }
        NuShaderManagerSetfv(SHADER_UNIFORM_LIGHTMAP_OFFSET, offset);
    }

    void NuDisplayListLegacyLightmapOffset(void *arg) {
        PC_DL_LEGACY_LIGHTMAP_PACKET *packet = (PC_DL_LEGACY_LIGHTMAP_PACKET *)arg;
        f32 offset[3];
        i32 i;
        for (i = 0; i < 3; ++i) {
            NUTEXDIM *texture;
            if (packet->texture_id[i] != -1) {
                texture = NuTextureGetDimensions(packet->texture_id[i]);
            } else {
                texture = NuTextureGetDimensions(1);
            }
            PCDeviceSetTexture(i, texture->resource);
            PCDeviceSetSamplerState(i, D3DSAMP_ADDRESSU, 1);
            PCDeviceSetSamplerState(i, D3DSAMP_ADDRESSV, 1);
        }
        offset[0] = packet->x_offset;
        offset[1] = -packet->y_offset;
        offset[2] = 0.0f;
        NuShaderManagerSetfv(SHADER_UNIFORM_LIGHTMAP_OFFSET, offset);
    }

    void NuDisplayListPacketAd(void *arg) {
        PC_DL_AD_PACKET *packet = (PC_DL_AD_PACKET *)arg;
        PC_DL_AD_RECORD *record;
        NUVEC4 values;
        NUVEC colour0;
        NUVEC colour1;
        NUVEC colour2;
        NUVEC colour3;
        NUVEC colour4;
        i32 segments;
        u32 vertex_count;
        u32 instances = packet->instances;
        if (packet->vertex_buffer == NULL || packet->instance_buffer == NULL || packet->list == NULL) {
            return;
        }
        record = &packet->list->records[packet->list->count - 1];
        segments = record->segment_count;
        values.x = (f32)segments;
        values.y = record->enabled > 0 ? 1.0f : 0.0f;
        values.z = record->vertex_parameter_x;
        values.w = record->vertex_parameter_y;
        PCDeviceSetVertexShaderConstantsF(0xc8, &values.x, 1);
        values.x = record->vertex_parameter_z;
        values.y = 0.0f;
        values.z = 0.0f;
        values.w = 0.0f;
        PCDeviceSetVertexShaderConstantsF(0xc9, &values.x, 1);
        colour0 = record->colours.colour[0];
        colour1 = record->colours.colour[1];
        colour2 = record->colours.colour[2];
        colour3 = record->colours.colour[3];
        colour4 = record->colours.colour[4];
        values.x = colour0.x;
        values.y = colour0.y;
        values.z = colour0.z;
        values.w = 1.0f;
        PCDeviceSetPixelShaderConstantsF(0xca, &values.x, 1);
        values.x = colour1.x;
        values.y = colour1.y;
        values.z = colour1.z;
        values.w = 1.0f;
        PCDeviceSetPixelShaderConstantsF(0xcb, &values.x, 1);
        values.x = colour2.x;
        values.y = colour2.y;
        values.z = colour2.z;
        values.w = 1.0f;
        PCDeviceSetPixelShaderConstantsF(0xcc, &values.x, 1);
        values.x = colour3.x;
        values.y = colour3.y;
        values.z = colour3.z;
        values.w = 1.0f;
        PCDeviceSetPixelShaderConstantsF(0xcd, &values.x, 1);
        values.x = colour4.x;
        values.y = colour4.y;
        values.z = colour4.z;
        values.w = 1.0f;
        PCDeviceSetPixelShaderConstantsF(0xce, &values.x, 1);
        values.x = record->pixel_parameter;
        values.y = record->pixel_parameter_x;
        values.z = record->pixel_parameter_y;
        values.w = record->pixel_parameter_z;
        PCDeviceSetPixelShaderConstantsF(0xd3, &values.x, 1);
        values.x = record->pixel_scalar;
        values.y = 0.0f;
        values.z = 0.0f;
        values.w = 0.0f;
        PCDeviceSetPixelShaderConstantsF(0xd4, &values.x, 1);
        PCDeviceSetStreamSource(0, packet->vertex_buffer, 0, 0x10);
        PCDeviceSetStreamSourceFreq(0, instances | 0x40000000u);
        PCDeviceSetStreamSource(1, packet->instance_buffer, 0, 0x50);
        PCDeviceSetStreamSourceFreq(1, 0x80000001u);
        PCDeviceSetIndices(packet->index_buffer);
        vertex_count = (u32)segments * 2u + 2u;
        PCDeviceDrawIndexedPrimitive(5, 0, 0, vertex_count, 0, vertex_count - 2u);
        PCDeviceSetStreamSource(0, NULL, 0, 0);
        PCDeviceSetStreamSourceFreq(0, 1);
        PCDeviceSetStreamSourceFreq(1, 1);
    }

    void NuRndrScreenGrabTileInit(SCREENGRAB_TILE_PARAMS *params, i32 tiles, f32 b, f32 a, f32 c) {
    }

    void NuRndrScreenGrabTileDeInit(SCREENGRAB_TILE_PARAMS *params) {
    }

    void NuRndrScreenGrabTileBegin(void **tile) {
        *tile = NULL;
    }

    void NuRndrScreenGrabTileEnd(void **tile) {
    }

    void NuVpRestore(void) {
        PC_VIEWPORT viewport;
        vpCurrent = vpDevice;
        viewport = vpDevice.viewport;
        viewport.width >>= 4;
        viewport.height >>= 4;
        NuVpSubmit(&viewport);
        vport_inval = 1;
    }

    void NuVpInitIdentityMatrix(void) {
        NuMtxSetIdentity(&NuVpIdentityMatrix);
    }

    void NuVpBuildScalingTemplate(void) {
        f32 width = (f32)PS2_REZ_W;
        f32 height = (f32)PS2_REZ_H;
        f32 min_z = vpCurrent.min_z;
        NuCameraScalingTemplate.m00 = (f32)((f64)width * NuQFntHalfScale);
        NuCameraScalingTemplate.m01 = 0.0f;
        NuCameraScalingTemplate.m02 = 0.0f;
        NuCameraScalingTemplate.m03 = 0.0f;
        NuCameraScalingTemplate.m10 = 0.0f;
        NuCameraScalingTemplate.m11 = (f32)(-height * NuQFntHalfScale);
        NuCameraScalingTemplate.m12 = 0.0f;
        NuCameraScalingTemplate.m13 = 0.0f;
        NuCameraScalingTemplate.m20 = 0.0f;
        NuCameraScalingTemplate.m21 = 0.0f;
        NuCameraScalingTemplate.m22 = (f32)(16777215.0 - (f64)min_z);
        NuCameraScalingTemplate.m23 = 0.0f;
        NuCameraScalingTemplate.m30 = (f32)((f64)width * NuQFntHalfScale + 0.0);
        NuCameraScalingTemplate.m31 = (f32)(0.0 + (f64)height * NuQFntHalfScale);
        NuCameraScalingTemplate.m32 = min_z;
        NuCameraScalingTemplate.m33 = 1.0f;
    }

    void NuVpUpdate(void) {
        PC_VIEWPORT viewport;
        i32 x;
        i32 y;
        i32 width;
        i32 height;
        i32 right;
        i32 bottom;
        i32 limit_x;
        i32 limit_y;
        i32 clipped_left;
        i32 clipped_top;
        f32 rounded_width;
        f32 rounded_height;
        f32 reciprocal;
        if (vport_inval == 0) {
            return;
        }
        clipped_left = 0;
        clipped_top = 0;
        NuCameraProjectionGlobalScaleX = 1.0f;
        NuCameraProjectionGlobalOffsetX = 0.0f;
        NuCameraProjectionGlobalOffsetY = 0.0f;
        NuCameraProjectionGlobalScaleY = 1.0f;
        viewport = vpCurrent.viewport;
        x = vpCurrent.x;
        y = vpCurrent.y;
        width = vpCurrent.width;
        height = vpCurrent.height;
        right = (i32)((u32)x + (u32)width);
        bottom = (i32)((u32)y + (u32)height);
        limit_x = (i32)((u32)PS2_REZ_W << 4);
        limit_y = (i32)((u32)PS2_REZ_H << 4);
        if (right < 0 || x >= limit_x || bottom < 0 || y >= limit_y) {
            goto empty;
        }
        if (x < 0) {
            width = right;
            x = 0;
            clipped_left = 1;
        }
        if (right > limit_x) {
            width = (i32)((u32)width + (u32)limit_x - (u32)right);
        }
        if (y < 0) {
            height = bottom;
            y = 0;
            clipped_top = 1;
        }
        if (bottom > limit_y) {
            height = (i32)((u32)height + (u32)limit_y - (u32)bottom);
        }
        if (width <= 0 || height <= 0) {
            goto empty;
        }
        rounded_width = (f32)width;
        NuCameraProjectionGlobalScaleX = (f32)((f64)vpCurrent.width / (f64)rounded_width);
        rounded_height = (f32)height;
        NuCameraProjectionGlobalScaleY = (f32)((f64)vpCurrent.height / (f64)rounded_height);
        NuCameraProjectionGlobalOffsetX = (f32)((f64)(i32)((u32)vpCurrent.width - (u32)width) / (f64)rounded_width);
        NuCameraProjectionGlobalOffsetY = (f32)(-(f64)(i32)((u32)vpCurrent.height - (u32)height) / (f64)rounded_height);
        if (clipped_left != 0) {
            NuCameraProjectionGlobalOffsetX = -NuCameraProjectionGlobalOffsetX;
        }
        if (clipped_top != 0) {
            NuCameraProjectionGlobalOffsetY = -NuCameraProjectionGlobalOffsetY;
        }
        goto submit;
    empty:
        x = y = width = height = 0;
    submit:
        viewport.x = (u32)(x >> 4);
        viewport.y = (u32)(y >> 4);
        viewport.width = (u32)(width >> 4);
        viewport.height = (u32)(height >> 4);
        vport_inval = 0;
        NuVpSubmit(&viewport);
        NuMtxSetIdentity(&NuVpIdentityMatrix);
        NuVpBuildScalingTemplate();
        if (vpCurrent.clip_min_x == 0.0f && vpCurrent.clip_max_x == 1.0f) {
            vpCurrent.projection_scale_x = 1.0f;
            vpCurrent.scissor_scale_x = (f32)((f64)vpCurrent.width * NuVpFixed16Scale);
        } else {
            reciprocal = (f32)(1.0 / ((f64)vpCurrent.clip_max_x - (f64)vpCurrent.clip_min_x));
            vpCurrent.scissor_scale_x = reciprocal;
            vpCurrent.projection_scale_x = vpCurrent.scissor_scale_x;
        }
        if (vpCurrent.clip_min_y == 0.0f && vpCurrent.clip_max_y == 1.0f) {
            vpCurrent.projection_scale_y = 1.0f;
            vpCurrent.scissor_scale_y = (f32)((f64)vpCurrent.height * NuVpFixed16Scale);
        } else {
            reciprocal = (f32)(1.0 / ((f64)vpCurrent.clip_max_y - (f64)vpCurrent.clip_min_y));
            vpCurrent.scissor_scale_y = reciprocal;
            vpCurrent.projection_scale_y = vpCurrent.scissor_scale_y;
        }
    }

    void fun_005ff750(void) {
    }

    i32 PCTimerMilliseconds(void) {
        LARGE_INTEGER frequency;
        LARGE_INTEGER counter;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&counter);
        return (counter.QuadPart / (f64)frequency.QuadPart * 1000.0);
    }

    f32 fun_005ff790(void) {
        return 1.0f;
    }

    void RenderPostViewportSet(f32 source_x, f32 source_y, f32 source_right, f32 source_bottom, f32 dest_x, f32 dest_y, f32 dest_right,
                               f32 dest_bottom) {
        RenderPostViewportPrefix.source_x = source_x;
        RenderPostViewportPrefix.source_y = source_y;
        RenderPostViewportPrefix.source_right = source_right;
        RenderPostViewportPrefix.source_bottom = source_bottom;
        RenderPostViewportPrefix.dest_x = dest_x;
        RenderPostViewportPrefix.dest_y = dest_y;
        RenderPostViewportPrefix.dest_right = dest_right;
        RenderPostViewportPrefix.dest_bottom = dest_bottom;
        RenderPostViewportPrefix.x_offset = dest_x;
        RenderPostViewportPrefix.y_offset = dest_y;
        f32 width = (f32)PS2_VREZ_W;
        RenderPostViewportPrefix.width_scale = (dest_right - dest_x) / width;
        f32 height = (f32)PS2_VREZ_H;
        RenderPostViewportPrefix.height_scale = (dest_bottom - dest_y) / height;
        PCSpecialProjectionAdjust.x_scale = width / (source_right - source_x);
        PCSpecialProjectionAdjust.y_scale = height / (source_bottom - source_y);
        PCSpecialProjectionAdjust.x_offset = ((width - source_right) - source_x) * 2.0f / width;
        PCSpecialProjectionAdjust.y_offset = ((height - source_bottom) - source_y) * -2.0f / height;
    }

    i32 NuSpecialDrawAtAlpha(nuhspecial_s *handle, NUMTX *matrix, f32 alpha) {
        i32 result;
        if (handle->scene == NULL) {
            return 0;
        }
        if (alpha <= 0.0f) {
            return 0;
        }
        if (alpha < 1.0f) {
            NuSpecialConstAlpha(1, alpha);
            result = NuDisplayListRndrSpecial(handle, matrix, 0, NULL, NULL);
            NuSpecialConstAlpha(0, 0.0f);
        } else {
            result = NuDisplayListRndrSpecial(handle, matrix, 0, NULL, NULL);
        }
        return result;
    }

    static u32 NuDebrisCurrentBufferIndex;

    /* PC implementation of the NuInitDebrisRenderer family. */
    void debris_InitializeD3D(void) {
        i32 index;
        NuDebrisCurrentBufferIndex = 0;
        NuDebrisUseSystemMemory = 0;
        NuDebrisCurrentVertexCount = 0;
        NuDebrisLastHeader = NULL;
        for (index = 0; index != 4; ++index) {
            PCDeviceCreateVertexBuffer(NuDebrisVertexBufferSize, 0x208, 0, 0, &NuDebrisVertexBuffers[index]);
        }
        PCDeviceLockVertexBuffer(0, NuDebrisVertexBufferSize, (void **)&NuDebrisVertexData, 0x2000,
                                 NuDebrisVertexBuffers[NuDebrisCurrentBufferIndex]);
    }

    void NuDebrisReleaseVertexBuffers(void) {
        PC_BUFFER_RESOURCE **buffer = NuDebrisVertexBuffers;
        do {
            PCDeviceReleaseTexture((PC_TEXTURE_RESOURCE **)buffer);
            ++buffer;
        } while (buffer != NuDebrisVertexBuffers + 4);
    }

    void NuDebrisRendererFlushBuffers(void) {
        if (NuDebrisUseSystemMemory == 0 && NuDebrisVertexData != NULL) {
            PCDeviceUnlockVertexBuffer(NuDebrisVertexBuffers[NuDebrisCurrentBufferIndex]);
        }
        NuDebrisVertexData = NULL;
        NuDebrisCurrentBufferIndex = 0;
        NuDebrisUseSystemMemory = 0;
        NuDebrisCurrentVertexCount = 0;
        NuDebrisLastHeader = NULL;
        NuDebrisFrameVertexCount = 0;
    }

    i32 NuDebrisRendererNextBuffer(void) {
        if (NuDebrisUseSystemMemory == 0 && NuDebrisVertexData != NULL) {
            PCDeviceUnlockVertexBuffer(NuDebrisVertexBuffers[NuDebrisCurrentBufferIndex]);
        }
        if (NuDebrisUseSystemMemory != 0) {
            if (NuDebrisCurrentBufferIndex + 1 >= 8) {
                PCClipScreenUnsupported();
                return 0;
            }
            NuDebrisCurrentBufferIndex = (NuDebrisCurrentBufferIndex + 1) & 7;
        } else if (NuDebrisCurrentBufferIndex + 1 >= 4) {
            NuDebrisCurrentBufferIndex = 0;
            NuDebrisUseSystemMemory = 1;
        } else {
            NuDebrisCurrentBufferIndex = (NuDebrisCurrentBufferIndex + 1) & 3;
        }
        NuDebrisCurrentVertexCount = 0;
        if (NuDebrisUseSystemMemory != 0) {
            NuDebrisVertexData = NuDebrisSystemBuffers[NuDebrisCurrentBufferIndex];
        } else {
            PCDeviceLockVertexBuffer(0, NuDebrisVertexBufferSize, (void **)&NuDebrisVertexData, 0x2000,
                                     NuDebrisVertexBuffers[NuDebrisCurrentBufferIndex]);
        }
        return 1;
    }

    void NuDisplayListDrawDebris(void *arg) {
        PC_DL_DEBRIS_DRAW_PACKET *packet = (PC_DL_DEBRIS_DRAW_PACKET *)arg;
        if (packet->vertex_count == 0) {
            return;
        }
        if (packet->use_system_buffer != 0) {
            const void *vertices = NuDebrisSystemBuffers[packet->buffer_index] + packet->first_vertex;
            PCDeviceDrawPrimitiveUPPc(4, packet->vertex_count / 3u, vertices, 24);
        } else {
            PCDeviceSetStreamSource(0, NuDebrisVertexBuffers[packet->buffer_index], 0, 24);
            PCDeviceDrawPrimitive(4, packet->first_vertex, packet->vertex_count / 3u);
        }
    }

    DEBRIS_PARTICLE_CHUNK *CreateDmaParticleSet(void *memory, i32 *size) {
        u8 *cursor = (u8 *)memory;
        u8 *start = cursor;
        DEBRIS_PARTICLE_CHUNK *chunk = (DEBRIS_PARTICLE_CHUNK *)cursor;
        DEBRISPARTICLE *particle;
        i32 i;
        chunk->command = 0x52;
        chunk->next = NULL;
        cursor += 0x10;
        ((u32 *)cursor)[1] = 0;
        ((u32 *)cursor)[2] = 0;
        ((u32 *)cursor)[3] = 0;
        ((u32 *)cursor)[4] = 0;
        cursor += 0x10;
        for (i = 0; i < 32; ++i) {
            particle = (DEBRISPARTICLE *)cursor;
            particle->position.x = 1.0f;
            particle->position.y = 2.0f;
            particle->position.z = 3.0f;
            particle->momentum.x = 4.0f;
            particle->momentum.y = 5.0f;
            particle->momentum.z = 6.0f;
            particle->start_time = -1.0f;
            particle->inverse_lifetime = 128.0f;
            cursor += sizeof(DEBRISPARTICLE);
        }
        *(u32 *)cursor = 0;
        cursor += sizeof(u32);
        *size = cursor - start;
        return (DEBRIS_PARTICLE_CHUNK *)start;
    }

    DEBRIS_PARTICLE_CHUNK *CreateDmaParticleSetGlass(void *memory, i32 *size) {
        return CreateDmaParticleSet(memory, size);
    }

    DEBRIS_PART_HEADER *CreateDmaPartEffectList(void *memory, i32 *size) {
        u8 *cursor = (u8 *)(((u32)memory + 0xf) & ~0xf);
        u8 *start = cursor;
        DEBRIS_PART_HEADER *header = (DEBRIS_PART_HEADER *)cursor;
        cursor = (u8 *)&header->frames[64];
        *size = cursor - start;
        return (DEBRIS_PART_HEADER *)start;
    }

    void LinkDmaParticalSets(DEBRIS_PARTICLE_CHUNK **chunks, i32 count) {
        DEBRIS_PARTICLE_CHUNK *chunk = chunks[count - 1];
        i32 i;
        chunk->command = 0x52;
        chunk->next = NULL;
        for (i = count - 2; i >= 0; --i) {
            chunk = chunks[i];
            chunk->command = 0x4e;
            chunk->next = chunks[i + 1];
        }
    }

    void NuRndrParticleSetRepeat(NUVEC *position) {
        NUVEC repeat;
        repeat.x = (position->x - NuRndrDebBase.x) / NuRndrDebRange.x;
        repeat.y = (position->y - NuRndrDebBase.y) / NuRndrDebRange.y;
        repeat.z = (position->z - NuRndrDebBase.z) / NuRndrDebRange.z;
        repeat.x -= (f32)((i32)(repeat.x + 65536.0) - 65536);
        repeat.y -= (f32)((i32)(repeat.y + 65536.0) - 65536);
        repeat.z -= (f32)((i32)(repeat.z + 65536.0) - 65536);
        position->x = NuRndrDebBase.x + repeat.x * NuRndrDebRange.x;
        position->y = NuRndrDebBase.y + repeat.y * NuRndrDebRange.y;
        position->z = NuRndrDebBase.z + repeat.z * NuRndrDebRange.z;
    }

    void GenericDebinfoDmaTypeUpdate(DEBRIS_INFO *effect) {
        if (effect->native_data == NULL) {
            if (freeDmaDebType >= EDPP_MAX_DMADEBTYPES) {
                NuPtlDmaSlotsExhausted();
                return;
            }
            effect->native_data = DmaDebTypes[freeDmaDebType++];
        }

        DEBRIS_PART_HEADER *header = effect->native_data;
        header->gravity = effect->gravity;
        header->texture_u0 = static_cast<f32>(static_cast<u32>(effect->texture_u0) & 0x1ffU) * static_cast<f64>(1.0f / 255.0f);
        header->texture_v0 = static_cast<f32>(static_cast<u32>(effect->texture_v0) & 0x1ffU) * static_cast<f64>(1.0f / 255.0f);
        header->texture_u1 = static_cast<f32>(static_cast<u32>(effect->texture_u1) & 0x1ffU) * static_cast<f64>(1.0f / 255.0f);
        header->texture_v1 = static_cast<f32>(static_cast<u32>(effect->texture_v1) & 0x1ffU) * static_cast<f64>(1.0f / 255.0f);

        f32 width;
        f32 height;
        f32 rotation;
        f32 alpha;
        f32 red;
        f32 green;
        f32 blue;
        f32 time;
        f32 duration;
        f32 elapsed;
        f32 wave_x;
        f32 wave_y;
        f32 position_x;
        f32 position_y;
        f32 texture_x;
        f32 texture_y;
        f32 edge_x;
        f32 edge_y;
        i32 i;
        for (i32 frame_index = 0; frame_index < 64; ++frame_index) {
            width = height = rotation = 0.0f;
            red = green = blue = alpha = 0.0f;
            time = static_cast<f32>(frame_index) / 64.0f;
            for (i = 0; i < 7; ++i) {
                if (effect->width_keys[i].time <= time && time <= effect->width_keys[i + 1].time) {
                    duration = effect->width_keys[i + 1].time - effect->width_keys[i].time;
                    elapsed = time - effect->width_keys[i].time;
                    width = elapsed == 0.0f ? effect->width_keys[i].value
                                            : effect->width_keys[i].value +
                                                  (effect->width_keys[i + 1].value - effect->width_keys[i].value) * (elapsed / duration);
                    break;
                }
            }
            for (i = 0; i < 7; ++i) {
                if (effect->height_keys[i].time <= time && time <= effect->height_keys[i + 1].time) {
                    duration = effect->height_keys[i + 1].time - effect->height_keys[i].time;
                    elapsed = time - effect->height_keys[i].time;
                    height = elapsed == 0.0f ? effect->height_keys[i].value
                                             : effect->height_keys[i].value +
                                                   (effect->height_keys[i + 1].value - effect->height_keys[i].value) * (elapsed / duration);
                    break;
                }
            }
            for (i = 0; i < 7; ++i) {
                if (effect->rotation_keys[i].time <= time && time <= effect->rotation_keys[i + 1].time) {
                    duration = effect->rotation_keys[i + 1].time - effect->rotation_keys[i].time;
                    elapsed = time - effect->rotation_keys[i].time;
                    rotation = elapsed == 0.0f
                                   ? effect->rotation_keys[i].value
                                   : effect->rotation_keys[i].value +
                                         (effect->rotation_keys[i + 1].value - effect->rotation_keys[i].value) * (elapsed / duration);
                    break;
                }
            }
            wave_x = effect->jib_x_amplitude * NU_SIN_LUT(65536.0 * (effect->jib_x_frequency * time));
            wave_y = effect->jib_y_amplitude * NU_SIN_LUT(65536.0 * (effect->jib_y_frequency * time));
            DEBRIS_PARTICLE_FRAME &frame = effect->native_data->frames[frame_index];
            rotation -= 16384.0;
            position_x = -(width / 4.0f) * NU_SIN_LUT(rotation + 16384.0) - (height / 4.0f) * NU_SIN_LUT(rotation) + wave_x;
            position_y = (width / 4.0f) * NU_SIN_LUT(rotation) - (height / 4.0f) * NU_SIN_LUT(rotation + 16384.0) + wave_y;
            texture_x = (width / 4.0f) * NU_SIN_LUT(rotation + 16384.0) - (height / 4.0f) * NU_SIN_LUT(rotation) + wave_x;
            texture_y = -(width / 4.0f) * NU_SIN_LUT(rotation) - (height / 4.0f) * NU_SIN_LUT(rotation + 16384.0) + wave_y;
            edge_x = (height / 2.0f) * NU_SIN_LUT(rotation);
            edge_y = (height / 2.0f) * NU_SIN_LUT(rotation + 16384.0);
            frame.position.x = position_x / 2048.0f;
            frame.position.y = position_y / 2048.0f;
            frame.position.z = 0.0f;
            frame.texture_offset.x = texture_x / 2048.0f;
            frame.texture_offset.y = texture_y / 2048.0f;
            frame.texture_offset.z = 0.0f;
            frame.extent.x = (texture_x + edge_x) / 2048.0f;
            frame.extent.y = (texture_y + edge_y) / 2048.0f;
            frame.extent.z = 0.0f;
            for (i = 0; i < 7; ++i) {
                if (effect->colour_keys[i].time <= time && time <= effect->colour_keys[i + 1].time) {
                    duration = effect->colour_keys[i + 1].time - effect->colour_keys[i].time;
                    elapsed = time - effect->colour_keys[i].time;
                    if (elapsed == 0.0f) {
                        red = effect->colour_keys[i].red;
                        green = effect->colour_keys[i].green;
                        blue = effect->colour_keys[i].blue;
                    } else {
                        red = effect->colour_keys[i].red +
                              (effect->colour_keys[i + 1].red - effect->colour_keys[i].red) * (elapsed / duration);
                        green = effect->colour_keys[i].green +
                                (effect->colour_keys[i + 1].green - effect->colour_keys[i].green) * (elapsed / duration);
                        blue = effect->colour_keys[i].blue +
                               (effect->colour_keys[i + 1].blue - effect->colour_keys[i].blue) * (elapsed / duration);
                    }
                    break;
                }
            }
            for (i = 0; i < 7; ++i) {
                if (effect->alpha_keys[i].time <= time && time <= effect->alpha_keys[i + 1].time) {
                    duration = effect->alpha_keys[i + 1].time - effect->alpha_keys[i].time;
                    elapsed = time - effect->alpha_keys[i].time;
                    alpha = elapsed == 0.0f ? effect->alpha_keys[i].value
                                            : effect->alpha_keys[i].value +
                                                  (effect->alpha_keys[i + 1].value - effect->alpha_keys[i].value) * (elapsed / duration);
                    break;
                }
            }
            alpha += alpha;
            alpha = alpha < 255.0f ? alpha : 255.0f;
            frame.colour =
                (((static_cast<u32>(alpha) << 8) | static_cast<u32>(red)) << 8 | static_cast<u32>(green)) << 8 | static_cast<u32>(blue);
        }
    }

    void NuPrim2DEnd(void) {
        *NuPrimVertexCountPointer = NuPrimVertexCount;
        NuPrimVertexCount = 0;
    }

    void NuPrim3DEnd(void) {
        *NuPrimVertexCountPointer = NuPrimVertexCount;
        NuPrimVertexCount = 0;
    }

    void NuPrim2DAddXYZ(f32 x, f32 y, f32 z) {
        NuPrimStreamBufferPointer->vertex_ptr->x = NuPrimXScale * x + NuPrimXBias;
        NuPrimStreamBufferPointer->vertex_ptr->y = NuPrimYScale * y + NuPrimYBias;
        NuPrimStreamBufferPointer->vertex_ptr->z = z;
        NuPrimStreamBufferPointer->addr += sizeof(NULGTVERTEX);
        NuPrimVertexCount++;

        if (NuPrimCurrentPrimitiveType == 0x10 && (NuPrimVertexCount & 1) == 0) {
            /* Expand the two sprite corners just written into two triangles. */
            NULGTVERTEX *next = NuPrimStreamBufferPointer->vertex_ptr;
            NuPrimStreamBufferPointer->void_ptr = next + 4;
            NuPrimVertexCount += 4;
            next[0] = next[-1];
            next[1] = next[-1];
            next[2].x = next[-2].x;
            next[2].y = next[-1].y;
            next[2].z = next[-1].z;
            next[2].colour = next[-1].colour;
            next[2].u = next[-2].u;
            next[2].v = next[-1].v;
            next[-1].x = next[0].x;
            next[-1].y = next[-2].y;
            next[-1].z = next[-2].z;
            next[-1].colour = next[-2].colour;
            next[-1].u = next[0].u;
            next[-1].v = next[-2].v;
            next[3] = next[-2];
        }
    }

    void NuPrimInit(VARIPTR *buffer, VARIPTR end) {
    }

    void NuPrimSetCoordinateSystem(i32 scale_mode) {
        NuPrimCoordSystemStack[NuPrimCSPos] = scale_mode;

        switch (scale_mode) {
            case 0:
                NuPrimXScale = NuPrimPS2XScale;
                NuPrimYScale = NuPrimPS2YScale;
                NuPrimXBias = -1.0f;
                NuPrimYBias = 1.0f;
                break;
            case 1:
                NuPrimXScale = 1.0f;
                NuPrimYScale = -1.0f;
                NuPrimXBias = 0.0f;
                NuPrimYBias = 0.0f;
                break;
            case 2:
                NuPrimXScale = 2.0f;
                NuPrimYScale = -2.0f;
                NuPrimXBias = -1.0f;
                NuPrimYBias = 1.0f;
                break;
        }
    }

    /* Empty hook; the D3D release path calls it before releasing the geometry buffers. */
    void NuPrimReleaseResources(void) {
    }

    void NuKeyboardWindowStateLeaf(void) {
    }

    void Initialise_PS(NUGSCN *scene) {
        scene->instance_visibility_flags = NuPortalVisibilityBits;
    }

    void PortalFlagRoomVisible(NUGSCNROOM *room, NUGSCN *scene) {
        i32 i;
        if (scene->display_list != NULL) {
            for (i = 0; i < room->instance_count; ++i) {
                i32 index = room->instance_indices[i];
                NuPortalVisibilityBits[index >> 3] |= (u8)(1 << (index & 7));
            }
        } else {
            for (i = 0; i < room->instance_count; ++i) {
                scene->instances[room->instance_indices[i]].flags |= 4;
            }
        }
    }

    void SetAllInstancesVisible(NUGSCN *scene) {
        i32 i;
        if (scene->display_list != NULL) {
            memset(NuPortalVisibilityBits, 255, sizeof(NuPortalVisibilityBits));
        } else {
            for (i = 0; i < scene->num_instances; ++i) {
                scene->instances[i].flags |= 4;
            }
        }
    }

    void SetAllInstancesHidden(NUGSCN *scene) {
        i32 i;
        if (scene->display_list != NULL) {
            memset(NuPortalVisibilityBits, 0, sizeof(NuPortalVisibilityBits));
        } else {
            for (i = 0; i < scene->num_instances; ++i) {
                scene->instances[i].flags &= ~4U;
            }
        }
    }

    void PortalClipRoom(NUGSCN *scene, NUGSCNROOM *room, NUPORTAL_FRUSTUM *frustum) {
        NUDISPLAY_LIST *display = scene->display_list;
        u32 i;
        if (display != NULL && (display->render_flags & 0x4000) != 0) {
            for (i = 0; i < (u32)(i32)room->instance_count; ++i) {
                u32 index = (u32)(i32)room->instance_indices[i];
                u32 byte_index = index >> 3;
                u32 bit = index & 7;
                if (((NuPortalVisibilityBits[byte_index] >> bit) & 1) != 0) {
                    continue;
                }
                display = scene->display_list;
                if (((display->portal_visibility_overrides[byte_index] >> bit) & 1) != 0) {
                    NuPortalVisibilityBits[byte_index] |= (u8)(1 << bit);
                } else {
                    u8 flags = display->visibility_flags[index];
                    if ((flags & 1) != 0) {
                        if ((flags & 8) != 0) {
                            NuPortalVisibilityBits[byte_index] |= (u8)(1 << bit);
                        } else if (NuPortalClipTestBox(&scene->portal_boxes[index].center.xyz, &scene->portal_boxes[index].extent.xyz,
                                                       frustum) != 0) {
                            NuPortalVisibilityBits[byte_index] |= (u8)(1 << bit);
                        }
                    }
                }
            }
        } else {
            for (i = 0; i < (u32)(i32)room->instance_count; ++i) {
                u32 index = (u32)(i32)room->instance_indices[i];
                u32 byte_index = index >> 3;
                u32 bit = index & 7;
                u8 bits = NuPortalVisibilityBits[byte_index];
                if (((bits >> bit) & 1) != 0) {
                    continue;
                }
                /* Dereferences a null display only for an unseen instance. */
                display = scene->display_list;
                if (((display->portal_visibility_overrides[byte_index] >> bit) & 1) != 0) {
                    NuPortalVisibilityBits[byte_index] = bits | (u8)(1 << bit);
                    continue;
                }
                if ((display->visibility_flags[index] & 8) != 0) {
                    NuPortalVisibilityBits[byte_index] = bits | (u8)(1 << bit);
                    continue;
                }
                switch (clipTestSphere(&scene->portal_spheres[index], frustum)) {
                    case 1:
                        NuPortalVisibilityBits[byte_index] |= (u8)(1 << bit);
                        break;
                    case 2:
                        if (clipTestBox(&scene->portal_boxes[index].center.xyz, &scene->portal_boxes[index].extent.xyz, frustum->planes,
                                        frustum->plane_count) != 0) {
                            NuPortalVisibilityBits[byte_index] |= (u8)(1 << bit);
                        }
                        break;
                }
            }
        }
    }

    void NuMtlCreatePS(NUMTL *mtl, i32 is_3d) {
        NuShaderMtlDescInit(&mtl->shader);
        if (mtl->texture_id > 0) {
            mtl->shader.diffuse_map_ids[0] = mtl->texture_id;
            if (mtl->shader.texture_unit_count <= 0) {
                mtl->shader.texture_unit_count = 1;
            }
            if ((mtl->shader.vertex_format & 0x3800) <= 0) {
                mtl->shader.vertex_format = (mtl->shader.vertex_format & 0xffffcfff) | 0x800;
            }
        } else {
            u32 colour = static_cast<u8>(mtl->diffuse_b * 255.0) | 0xffffff00;
            colour = (colour << 8) | static_cast<u8>(mtl->diffuse_g * 255.0);
            colour = (colour << 8) | static_cast<u8>(mtl->diffuse_r * 255.0);
            mtl->shader.diffuse_colours[0] = colour;
        }
        if (is_3d == 0) {
            mtl->shader.vertex_format |= 0x40000;
        }
    }

    void NuMtlUpdatePS(NUMTL *mtl) {
        PC_DEVICE_BACKEND *device = &PCDeviceBackend;
        NuShaderObject *shader;
        NuShaderObject *variant;
        NudxFw_D3DBeginCriticalSection();
        if (mtl->texture_id > 0) {
            mtl->shader.diffuse_map_ids[0] = mtl->texture_id;
            if (mtl->shader.texture_unit_count <= 0) {
                mtl->shader.texture_unit_count = 1;
            }
            if ((mtl->shader.vertex_format & 0x3800) <= 0) {
                mtl->shader.vertex_format = (mtl->shader.vertex_format & 0xffffcfff) | 0x800;
            }
        }
        if ((mtl->attribs2 & 0x800000) != 0 && mtl->display_list == NULL) {
            mtl->shader.vertex_format |= 0x40000;
        }
        if (mtl->shader.shader_handles[0] > 0) {
            NuShaderManagerReleaseShader(PCShaderRecordFromHandle(mtl->shader.shader_handles[0]));
        }
        if (mtl->shader.shader_handles[1] > 0) {
            NuShaderManagerReleaseShader(PCShaderRecordFromHandle(mtl->shader.shader_handles[1]));
        }
        if ((mtl->attribs2 & 0x800000) != 0 && mtl->display_list == NULL) {
            shader = NuShaderManagerRetrieveShader(&mtl->shader, mtl);
            mtl->shader.shader_handles[0] = (i16)shader->marker;
        } else {
            shader = NuShaderManagerRetrieveShaderVariant(&mtl->shader, mtl, device->ShaderVariant16());
            variant = NuShaderManagerRetrieveShaderVariant(&mtl->shader, mtl, device->ShaderVariant1());
            mtl->shader.shader_handles[0] = (i16)shader->marker;
            mtl->shader.shader_handles[1] = (i16)variant->marker;
        }
        mtl->vertex_declaration = NuGetVertexDeclaration(mtl->shader.vertex_format);
        if (mtl->texture_id == 0) {
            u32 colour = static_cast<u8>(mtl->diffuse_b * 255.0) | 0xffffff00;
            colour <<= 8;
            colour |= static_cast<u8>(mtl->diffuse_g * 255.0);
            colour <<= 8;
            colour |= static_cast<u8>(mtl->diffuse_r * 255.0);
            mtl->shader.diffuse_colours[0] = colour;
        }
        NudxFw_D3DEndCriticalSection();
    }

    void NuMtlSetUVOffsetPS(NUMTL *mtl, u32 layer, f32 u, f32 v) {
        mtl->shader.animation_offsets[layer][0] = u;
        mtl->shader.animation_offsets[layer][1] = v;
    }

    void NuMtlSetShaderDescPS(NUMTL *mtl, const NUSHADERMTLDESC *description) {
        i32 colour;
        mtl->shader = *description;
        colour = mtl->shader.diffuse_colours[0];
        mtl->diffuse_r = (colour & 0xff) / 255.0;
        mtl->diffuse_g = ((colour >> 8) & 0xff) / 255.0;
        mtl->diffuse_b = ((colour >> 16) & 0xff) / 255.0;
    }

    void NuDisplayListReflection(void *arg) {
        i32 reflection = *(i32 *)arg;
        NUMTL *material;
        NuDisplayHandlerReflection = reflection;
        material = NuDisplayHandlerMaterial;
        if (material != NULL) {
            switch ((material->attribs.word >> 12) & 3) {
                case 0:
                    if (reflection != 0) {
                        PCDeviceSetRenderState(D3DRS_CULLMODE, 2);
                    } else {
                        PCDeviceSetRenderState(D3DRS_CULLMODE, 3);
                    }
                    break;
                case 1:
                    if (reflection != 0) {
                        PCDeviceSetRenderState(D3DRS_CULLMODE, 3);
                    } else {
                        PCDeviceSetRenderState(D3DRS_CULLMODE, 2);
                    }
                    break;
            }
        }
    }

    void fun_00601770(void) {
    }

    void fun_00601780(void) {
    }

    void fun_00601790(void) {
    }

    void fun_006017a0(NUMTL *material, i32 plane) {
    }

    void fun_006017b0(void) {
    }

    void NuMtlSetRenderPlane(NUMTL *material, i32 plane) {
    }

    void fun_006017d0(void) {
    }

    void fun_006017e0(void) {
    }

    void fun_006017f0(void) {
    }

    i32 fun_00601800(void) {
        return dat_0080e6a4;
    }

    void fun_00601810(i32 value) {
        dat_0080e6a4 = value;
    }

    i32 NuLgtRand(void) {
        NuLgtSeed = (NuLgtSeed * 0x24cd + 1) & 0xffff;
        return NuLgtSeed;
    }

    void NuLightFogX(f32 near_distance, f32 far_distance, u32 colour, f32, f32, i32, f32 density) {
        NuRndrStateSetFogEnabled(1);
        NuRndrStateSetFogState(near_distance, far_distance, colour, density);
    }

    void fun_006018b0(void) {
    }

    void fun_006019e0(void) {
    }

    void fun_006019f0(void) {
    }

    void fun_00601a00(f32 first, f32 second) {
        dat_0097f990 = first;
        dat_0280d3a0 = second;
    }

    void fun_00601a20(void) {
        NuLgtStateUnknown = 0;
        NuLgtStateEnabled = 0;
    }

    NUMTL *PCStartupMaterial1 = NULL;
    i32 NuLgtArcLaserCnt = 0;
    NULGTLASER NuLgtLaserData[64] = {0};

    void NuLgtLaser(i32 type, f32 width, f32 segment_length, f32 width_wobble, NUVEC *start, NUVEC *delta, u32 colour, f32 end_width,
                    f32 length) {
        i32 i;
        if (NuLgtLaserCnt >= 64) {
            return;
        }

        NuLgtLaserData[NuLgtLaserCnt].arc = 0;
        NuLgtLaserData[NuLgtLaserCnt].type = (u8)type;
        NuLgtLaserData[NuLgtLaserCnt].width = width;
        NuLgtLaserData[NuLgtLaserCnt].width_wobble = width_wobble;
        NuLgtLaserData[NuLgtLaserCnt].segment_length = segment_length;
        NuLgtLaserData[NuLgtLaserCnt].end_width_ratio = end_width / width;
        NuLgtLaserData[NuLgtLaserCnt].length = length;
        NuLgtLaserData[NuLgtLaserCnt].start = *start;
        NuLgtLaserData[NuLgtLaserCnt].end.x = delta->x + start->x;
        NuLgtLaserData[NuLgtLaserCnt].end.y = delta->y + start->y;
        NuLgtLaserData[NuLgtLaserCnt].end.z = delta->z + start->z;
        NuLgtLaserData[NuLgtLaserCnt].colour = colour;
        // The original tests the arc cursor here, even for a straight laser.
        if ((NuLgtArcLaserFrame & 1) == 0 || NuLgtLaserData[NuLgtArcLaserCnt].seed == 0) {
            NuLgtLaserData[NuLgtLaserCnt].seed = NU_LGT_RAND();
        }
        for (i = 0; i < 6; ++i) {
            NU_LGT_RAND();
        }
        ++NuLgtLaserCnt;
    }

    void NuLgtSetArcMatEx(i32 type, NUMTL *material, f32 u0, f32 v0, f32 u1, f32 v1) {
        if (type >= 4) {
            return;
        }
        NuLgtArcMtl[type].material = material;
        NuLgtArcMtl[type].u0 = u0;
        NuLgtArcMtl[type].v0 = v0;
        NuLgtArcMtl[type].u1 = u1;
        NuLgtArcMtl[type].v1 = v1;
    }

    i32 fun_00601de0(void) {
        return 0;
    }

    void NuLgtArcLaserEx(i32 type, NUVEC *start, NUVEC *end, NUVEC *bend, f32 width, f32 segment_length, f32 wobble, f32 bend_amount,
                         i32 colour, i32 flags) {
        i32 i;
        i32 index;
        u32 red;
        u32 green;
        u32 blue;
        u32 alpha;

        alpha = ((u32)colour >> 24) * 2;
        if (alpha > 255) {
            alpha = 255;
        }
        alpha <<= 24;
        blue = ((u8)((u32)colour >> 16)) * 2;
        if (blue > 255) {
            blue = 255;
        }
        green = ((u32)colour & 0xff00) * 2;
        if (green > 0xff00) {
            green = 0xff00;
        }
        red = ((u8)colour) * 2;
        if (red > 255) {
            red = 255;
        }

        index = NuLgtArcLaserCnt;
        if (index >= 16) {
            return;
        }

        NuLgtArcLaserData[index].type = (u8)type;
        NuLgtArcLaserData[index].start = *start;
        NuLgtArcLaserData[index].end = *end;
        NuLgtArcLaserData[index].bend = *bend;
        NuLgtArcLaserData[index].width = width;
        NuLgtArcLaserData[index].segment_length = segment_length;
        NuLgtArcLaserData[index].wobble = wobble;
        NuLgtArcLaserData[index].bend_amount = bend_amount;
        NuLgtArcLaserData[index].colour = (red << 16) | green | blue | alpha;
        NuLgtArcLaserData[index].flags = flags;
        if ((NuLgtArcLaserFrame & 1) == 0 || NuLgtArcLaserData[index].seed == 0) {
            NuLgtArcLaserData[index].seed = NU_LGT_RAND();
        }
        for (i = 0; i < 6; ++i) {
            NU_LGT_RAND();
        }
        ++NuLgtArcLaserCnt;
    }

    void NuGScnRndr3(NUGSCN *scene) {
        NuDisplaySceneRndr(scene->display_list);
    }

    void fun_00601e10(void) {
    }

    i32 NuGScnUploadGfxDataFromFilePS(VARIPTR *cursor, VARIPTR end, NUFILE file) {
        i32 section_size;
        i32 bytes_read;
        i32 i;
        void *data;
        i32 skip_texture;
        SCENE_FILE_TEXTURE_HEADER texture;
        section_size = 0;
        if (SceneUploadBufferStartPC.addr == 0) {
            SceneUploadBufferStartPC.void_ptr =
                NuAlloc(SceneUploadBufferSizePC, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nugscn_dlist.c", 0x3f);
            SceneUploadBufferEndPC.addr = SceneUploadBufferStartPC.addr + SceneUploadBufferSizePC;
        }
        SceneUploadBufferCursorPC = SceneUploadBufferStartPC;
        memset(&SceneVideoResourcesPC, 0, sizeof(SceneVideoResourcesPC));
        bytes_read = NuFileRead(file, &section_size, sizeof(section_size));
        bytes_read += NuFileRead(file, &SceneVideoResourcesPC.texture_count, sizeof(SceneVideoResourcesPC.texture_count));
        SceneVideoResourcesPC.textures = (PC_TEXTURE_RESOURCE **)SceneUploadBufferCursorPC.void_ptr;
        SceneUploadBufferCursorPC.addr += SceneVideoResourcesPC.texture_count * sizeof(*SceneVideoResourcesPC.textures);
        memset(SceneVideoResourcesPC.textures, 0, SceneVideoResourcesPC.texture_count * sizeof(*SceneVideoResourcesPC.textures));
        for (i = 0; i != SceneVideoResourcesPC.texture_count; ++i) {
            bytes_read += NuFileRead(file, &texture, sizeof(texture));
            if (texture.size == 0) {
                SceneVideoResourcesPC.textures[i] = NULL;
                if (texture.width < 0) {
                    SceneVideoResourcesPC.textures[i + 1] = NULL;
                    SceneVideoResourcesPC.textures[i + 2] = NULL;
                    SceneVideoResourcesPC.textures[i + 3] = NULL;
                    SceneVideoResourcesPC.textures[i + 4] = NULL;
                    SceneVideoResourcesPC.textures[i + 5] = NULL;
                    i += 5;
                }
            } else if (texture.width == 0) {
                NuPtlDmaSlotsExhausted();
                SceneVideoResourcesPC.textures[i] = NULL;
            } else {
                skip_texture = 0;
                if (PcWindowOptions.usehires == 0 && texture.width > 0 && ((texture.flags & 0xc) != 0 || (texture.flags & 0x3c00) != 0)) {
                    skip_texture = 1;
                }
                data = SceneUploadBufferCursorPC.void_ptr;
                SceneUploadBufferCursorPC.addr += texture.size;
                bytes_read += NuFileRead(file, data, texture.size);
                if (skip_texture == 0) {
                    NudxFw_D3DBeginCriticalSection();
                    if (texture.width < 0) {
                        texture.width = -texture.width;
                        PCDeviceCreateCubeTextureFromMemory(data, texture.size, &SceneVideoResourcesPC.textures[i]);
                        SceneVideoResourcesPC.textures[i + 1] = NULL;
                        SceneVideoResourcesPC.textures[i + 2] = NULL;
                        SceneVideoResourcesPC.textures[i + 3] = NULL;
                        SceneVideoResourcesPC.textures[i + 4] = NULL;
                        SceneVideoResourcesPC.textures[i + 5] = NULL;
                        i += 5;
                    } else {
                        PCDeviceCreateTextureFromMemoryEx(data, texture.size, texture.width, texture.height, -3, 0, 0, 1, -1, -1, 0, NULL,
                                                          NULL, &SceneVideoResourcesPC.textures[i]);
                    }
                    NudxFw_D3DEndCriticalSection();
                    if (bgProcIsBgThread() != 0 && PCFrameLoadingYieldEnabled() != 0) {
                        Sleep(10);
                    }
                }
                SceneUploadBufferCursorPC.addr -= texture.size;
            }
        }
        bytes_read += NuFileRead(file, &SceneVideoResourcesPC.vertex_buffer_count, sizeof(SceneVideoResourcesPC.vertex_buffer_count));
        SceneVideoResourcesPC.vertex_buffers = (PC_BUFFER_RESOURCE **)SceneUploadBufferCursorPC.void_ptr;
        SceneUploadBufferCursorPC.addr += SceneVideoResourcesPC.vertex_buffer_count * sizeof(PC_BUFFER_RESOURCE *);
        for (i = 0; i != SceneVideoResourcesPC.vertex_buffer_count; ++i) {
            u32 size = 0;
            bytes_read += NuFileRead(file, &size, sizeof(size));
            if (size == 0) {
                NuPtlDmaSlotsExhausted();
                SceneVideoResourcesPC.vertex_buffers[i] = NULL;
            } else {
                data = SceneUploadBufferCursorPC.void_ptr;
                SceneUploadBufferCursorPC.addr += size;
                bytes_read += NuFileRead(file, data, size);
                void *locked_data;
                NudxFw_D3DBeginCriticalSection();
                PCDeviceCreateVertexBuffer(size, 0, 0, 1, &SceneVideoResourcesPC.vertex_buffers[i]);
                SceneVideoResourcesPC.vertex_buffers[i]->methods->lock(SceneVideoResourcesPC.vertex_buffers[i], 0, 0, &locked_data, 0);
                memcpy(locked_data, data, size);
                SceneVideoResourcesPC.vertex_buffers[i]->methods->unlock(SceneVideoResourcesPC.vertex_buffers[i]);
                NudxFw_D3DEndCriticalSection();
                SceneUploadBufferCursorPC.addr -= size;
            }
        }
        bytes_read += NuFileRead(file, &SceneVideoResourcesPC.index_buffer_count, sizeof(SceneVideoResourcesPC.index_buffer_count));
        SceneVideoResourcesPC.index_buffers = (PC_BUFFER_RESOURCE **)SceneUploadBufferCursorPC.void_ptr;
        SceneUploadBufferCursorPC.addr += SceneVideoResourcesPC.index_buffer_count * sizeof(PC_BUFFER_RESOURCE *);
        for (i = 0; i != SceneVideoResourcesPC.index_buffer_count; ++i) {
            u32 size = 0;
            bytes_read += NuFileRead(file, &size, sizeof(size));
            if (size == 0) {
                NuPtlDmaSlotsExhausted();
                SceneVideoResourcesPC.index_buffers[i] = NULL;
            } else {
                data = SceneUploadBufferCursorPC.void_ptr;
                SceneUploadBufferCursorPC.addr += size;
                bytes_read += NuFileRead(file, data, size);
                void *locked_data;
                NudxFw_D3DBeginCriticalSection();
                PCDeviceCreateIndexBuffer(size, 0, 0x65, 1, &SceneVideoResourcesPC.index_buffers[i], NULL);
                SceneVideoResourcesPC.index_buffers[i]->methods->lock(SceneVideoResourcesPC.index_buffers[i], 0, 0, &locked_data, 0);
                memcpy(locked_data, data, size);
                SceneVideoResourcesPC.index_buffers[i]->methods->unlock(SceneVideoResourcesPC.index_buffers[i]);
                NudxFw_D3DEndCriticalSection();
                SceneUploadBufferCursorPC.addr -= size;
            }
        }
        section_size += 4;
        for (; bytes_read < section_size; ++bytes_read) {
            u8 padding;
            NuFileRead(file, &padding, 1);
        }
        return section_size;
    }

    void NuGScnCreatePS(NUGSCN *scene, VARIPTR *cursor, VARIPTR *end) {
        i32 i;
        for (i = 0; i != scene->texture_count; ++i) {
            scene->textures[i]->resource = SceneVideoResourcesPC.textures[i];
            scene->textures[i]->image_data = NULL;
            scene->textures[i]->image_size = 0;
        }
    }

    void NuGScnFixupTIDsPS(NUGSCN *scene) {
        i32 i;
        i32 j;
        if (scene->display_list == NULL) {
            return;
        }
        for (i = 0; i < scene->display_list->item_count; ++i) {
            NUDISPLAYLISTITEM *item = &scene->display_list->items[i];
            if (item->type == NUDLIST_LIGHTMAP) {
                SCENE_TEXTURE_PACKET_B0 *packet = (SCENE_TEXTURE_PACKET_B0 *)item->next;
                if (packet->kind == 2) {
                    for (j = 0; j < 3; ++j) {
                        packet->extra_texture_ids[j] = NuGScnFixupTID(scene, packet->extra_texture_ids[j]);
                    }
                }
                packet->texture_id = NuGScnFixupTID(scene, packet->texture_id);
            } else if (item->type == 0xae || item->type == 0xaf) {
                SCENE_TEXTURE_PACKET_AE *packet = (SCENE_TEXTURE_PACKET_AE *)item->next;
                if (packet != NULL) {
                    for (j = 0; j < 3; ++j) {
                        packet->texture_ids[j] = NuGScnFixupTID(scene, packet->texture_ids[j]);
                    }
                }
            }
        }
    }

    void NuGScnRestoreTIDsPS(NUGSCN *scene) {
        i32 i;
        i32 j;
        if (scene->display_list == NULL) {
            return;
        }
        for (i = 0; i < scene->display_list->item_count; ++i) {
            NUDISPLAYLISTITEM *item = &scene->display_list->items[i];
            if (item->type == NUDLIST_LIGHTMAP) {
                SCENE_TEXTURE_PACKET_B0 *packet = (SCENE_TEXTURE_PACKET_B0 *)item->next;
                if (packet->kind == 2) {
                    for (j = 0; j < 3; ++j) {
                        packet->extra_texture_ids[j] = NuGScnRestoreTID(scene, packet->extra_texture_ids[j]);
                    }
                }
                packet->texture_id = NuGScnRestoreTID(scene, packet->texture_id);
            } else if (item->type == 0xae || item->type == 0xaf) {
                SCENE_TEXTURE_PACKET_AE *packet = (SCENE_TEXTURE_PACKET_AE *)item->next;
                if (packet != NULL) {
                    for (j = 0; j < 3; ++j) {
                        packet->texture_ids[j] = NuGScnRestoreTID(scene, packet->texture_ids[j]);
                    }
                }
            }
        }
    }

    void NuGScnFixupPS(NUGSCN *scene) {
        SCENE_FIXUP_NATIVE *native = scene->native_scene;
        SCENE_FIXUP_GEOMETRY *geometry;
        SCENE_FIXUP_VERTEX_STREAM *stream;
        u32 i;
        u8 dynamic = 0;
        i32 material_index;
        i32 registry_index;
        for (i = 0; i != native->geometry_count; ++i) {
            geometry = native->geometries[i];
            geometry->index_buffer = SceneVideoResourcesPC.index_buffers[(u32)geometry->index_buffer];
            geometry->vertex_buffer = SceneVideoResourcesPC.vertex_buffers[(u32)geometry->vertex_buffer];
            if (geometry->vertex_format != 0) {
                dynamic = 1;
                NudxFw_D3DBeginCriticalSection();
                PCDeviceCreateVertexBuffer(geometry->vertex_stride * geometry->vertex_count, 0x208, 0, 0, &geometry->dynamic_vertex_buffer);
                NudxFw_D3DEndCriticalSection();
            }
        }
        if (dynamic != 0) {
            for (registry_index = 0; registry_index != 100; ++registry_index) {
                if (PCRegisteredNativeScenes[registry_index] == NULL) {
                    PCRegisteredNativeScenes[registry_index] = scene;
                    break;
                }
            }
        }
        for (i = 0; i != native->vertex_stream_count; ++i) {
            stream = native->vertex_streams[i];
            stream->vertex_buffer = SceneVideoResourcesPC.vertex_buffers[(u32)stream->vertex_buffer];
        }
        native->vertex_buffer_count = SceneVideoResourcesPC.vertex_buffer_count;
        native->index_buffer_count = SceneVideoResourcesPC.index_buffer_count;
        memcpy(native->vertex_buffers, SceneVideoResourcesPC.vertex_buffers,
               SceneVideoResourcesPC.vertex_buffer_count * sizeof(*native->vertex_buffers));
        memcpy(native->index_buffers, SceneVideoResourcesPC.index_buffers,
               SceneVideoResourcesPC.index_buffer_count * sizeof(*native->index_buffers));
        for (material_index = 0; material_index < scene->material_count; ++material_index) {
            NuMtlUpdate(scene->materials[material_index]);
        }
        NuPortalMaxDepth(scene, scene->max_portals);
    }

    void NuGScnDestroyPS(NUGSCN *scene) {
        SCENE_FIXUP_NATIVE *native = scene->native_scene;
        NUMTL *material;
        i32 i;
        i32 j;
        u8 released;
        for (i = 0; i != native->vertex_buffer_count; ++i) {
            NudxFw_D3DBeginCriticalSection();
            PCDeviceReleaseTexture((PC_TEXTURE_RESOURCE **)&native->vertex_buffers[i]);
            NudxFw_D3DEndCriticalSection();
        }
        for (i = 0; i != native->index_buffer_count; ++i) {
            NudxFw_D3DBeginCriticalSection();
            PCDeviceReleaseTexture((PC_TEXTURE_RESOURCE **)&native->index_buffers[i]);
            NudxFw_D3DEndCriticalSection();
        }
        for (i = 0; i != scene->texture_count; ++i) {
            NudxFw_D3DBeginCriticalSection();
            PCDeviceReleaseTexture(&scene->textures[i]->resource);
            NudxFw_D3DEndCriticalSection();
        }
        released = 0;
        for (i = 0; i != native->geometry_count; ++i) {
            SCENE_FIXUP_GEOMETRY *geometry = native->geometries[i];
            if (geometry->vertex_format != 0 && (geometry->vertex_format & 0xf000) == 0) {
                PC_BUFFER_RESOURCE *resource;
                released = 1;
                NudxFw_D3DBeginCriticalSection();
                resource = geometry->dynamic_vertex_buffer;
                resource->methods->release(resource);
                NudxFw_D3DEndCriticalSection();
            }
        }
        if (released) {
            for (i = 0; i != 100; ++i) {
                if (PCRegisteredNativeScenes[i] == scene) {
                    PCRegisteredNativeScenes[i] = NULL;
                    break;
                }
            }
        }
        material = scene->materials[0];
        for (i = 0; i < scene->material_count; ++i, ++material) {
            for (j = 0; j < 2; ++j) {
                if (material->shader.shader_handles[j] > 0) {
                    PC_SHADER_RECORD_VIEW *record = PCShaderRecordFromHandle(material->shader.shader_handles[j]);
                    record->ref_count--;
                }
            }
        }
    }

    void fun_00602800(void) {
    }

    void PCRebaseNativeScene(NUGSCN *scene, u32 delta) {
        VARIPTR old_scene;
        i32 i;
        old_scene.void_ptr = scene;
        old_scene.addr -= delta;
        for (i = 0; i != 100; ++i) {
            if (PCRegisteredNativeScenes[i] == old_scene.void_ptr) {
                PCRegisteredNativeScenes[i] = scene;
                return;
            }
        }
    }

    /* Releases the dynamic vertex buffers of every registered scene before a D3D reset. */
    void NuGScnReleaseDynamicBuffers(void) {
        i32 i;
        u32 j;
        for (i = 0; i != 100; ++i) {
            NUGSCN *scene = PCRegisteredNativeScenes[i];
            if (scene != NULL) {
                SCENE_FIXUP_NATIVE *native = scene->native_scene;
                for (j = 0; j != native->geometry_count; ++j) {
                    SCENE_FIXUP_GEOMETRY *geometry = native->geometries[j];
                    if (geometry->vertex_format != 0 && (geometry->vertex_format & 0xf000) == 0) {
                        NudxFw_D3DBeginCriticalSection();
                        geometry->dynamic_vertex_buffer->methods->release(geometry->dynamic_vertex_buffer);
                        NudxFw_D3DEndCriticalSection();
                    }
                }
            }
        }
    }

    /* Recreates the dynamic vertex buffers released by NuGScnReleaseDynamicBuffers. */
    void NuGScnRestoreDynamicBuffers(void) {
        i32 i;
        u32 j;
        for (i = 0; i != 100; ++i) {
            NUGSCN *scene = PCRegisteredNativeScenes[i];
            if (scene != NULL) {
                SCENE_FIXUP_NATIVE *native = scene->native_scene;
                for (j = 0; j != native->geometry_count; ++j) {
                    SCENE_FIXUP_GEOMETRY *geometry = native->geometries[j];
                    if (geometry->vertex_format != 0 && (geometry->vertex_format & 0xf000) == 0) {
                        NudxFw_D3DBeginCriticalSection();
                        PCDeviceCreateVertexBuffer(geometry->vertex_stride * geometry->vertex_count, 0x208, 0, 0,
                                                   &geometry->dynamic_vertex_buffer);
                        NudxFw_D3DEndCriticalSection();
                    }
                }
            }
        }
    }

    void NuDisplayListDrawItems(NUDISPLAYLISTITEM *items) {
        NuDisplayListExecute(items, CurrentItemTable);
    }

    i32 DisplayListDebugPS(NUDISPLAYLISTITEM *item, char *text) {
        return 0;
    }

    void NuDisplayListAddClut(NUDISPLAYLISTITEM *item, i32 texture_id) {
        item->type = NUDLIST_DUMMY;
        item->next = NULL;
        item->id = NUDLISTITEM_SKIP;
    }

    void NuDisplayListAddTexture(NUDISPLAYLISTITEM *item, i32 texture_id) {
        item->type = NUDLIST_DUMMY;
        item->next = NULL;
        item->id = NUDLISTITEM_SKIP;
    }

    void NuDisplayListAddMaterialState(NUDISPLAYLISTITEM *item, NUMTL *material) {
        item->type = NUDLIST_MTL;
        item->next = material;
        item->id = NUDLISTITEM_CALL;
    }

    void NuDisplayListAddMicrocode(NUDISPLAYLISTITEM *item, NUMTL *material) {
        item->type = NUDLIST_DUMMY;
        item->next = NULL;
        item->id = NUDLISTITEM_SKIP;
    }

    void NuDisplayListAddLightState(NUDISPLAYLISTITEM *item, NUMTL *material) {
        item->type = NUDLIST_DUMMY;
        item->next = NULL;
        item->id = NUDLISTITEM_SKIP;
    }

    void DisplayListCreatePS(NUDISPLAY_LIST *scene, VARIPTR *buffer, VARIPTR end) {
    }

    VARIPTR *NuDisplayListLinkItem(NUDISPLAYLIST *list, u8 type, void *target) {
        VARIPTR *cursor;
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
        cursor = NuDisplayListStreamCursor;
        list->mtl_last->next = cursor->void_ptr;
        cursor->dlist_ptr->type = type;
        cursor->dlist_ptr->id = NUDLISTITEM_CALL;
        if (target != NULL) {
            cursor->dlist_ptr->next = target;
        } else {
            cursor->dlist_ptr->next = cursor->dlist_ptr + 2;
        }
        cursor->dlist_ptr[1].type = NUDLIST_NEXT;
        cursor->dlist_ptr[1].id = NUDLISTITEM_JUMP;
        cursor->dlist_ptr[1].next = list->dynamic_geometry + 1;
        list->mtl_last = cursor->dlist_ptr + 1;
        cursor->addr += 2 * sizeof(NUDISPLAYLISTITEM);
        return target != NULL ? NULL : cursor;
    }

    VARIPTR *NuDisplayListLinkItems(NUDISPLAYLIST *list, i32 count) {
        VARIPTR *cursor;
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        cursor = NuDisplayListStreamCursor;
        list->mtl_last->next = cursor->void_ptr;
        list->items = cursor->dlist_ptr;
        cursor->addr += count * sizeof(NUDISPLAYLISTITEM);
        cursor->dlist_ptr->type = NUDLIST_NEXT;
        cursor->dlist_ptr->id = NUDLISTITEM_JUMP;
        cursor->dlist_ptr->next = list->dynamic_geometry + 1;
        list->mtl_last = cursor->dlist_ptr;
        cursor->addr += sizeof(NUDISPLAYLISTITEM);
        return cursor;
    }

    void *DisplayListCreateGeomTransformPS(VARIPTR *buffer, NUMTX *matrix, NUMTL *material, void *geometry, void *existing) {
        NUVEC translation;
        NUVEC scale;
        NUMTX *result;
        NuCameraGetProjectionMtx();
        NuCameraGetViewMtx();
        scale.z = 1.0f;
        translation.z = translation.y = translation.x = 0.0f;
        scale.x = PCSpecialProjectionAdjust.x_scale;
        scale.y = PCSpecialProjectionAdjust.y_scale;
        NuMtxSetScale(&PCSpecialProjectionScale, &scale);
        translation.x = PCSpecialProjectionAdjust.x_offset;
        translation.y = PCSpecialProjectionAdjust.y_offset;
        NuMtxSetTranslation(&PCSpecialProjectionTranslate, &translation);
        buffer->addr = (buffer->addr + 3) & ~3u;
        result = buffer->mtx_ptr;
        PCSpecialTransposeMatrix(result, matrix);
        buffer->addr += sizeof(NUMTX);
        return result;
    }

    void *DisplayListCreateFaceonTransformPS(VARIPTR *buffer, NUMTX *matrix, NUMTL *material, void *faceon) {
        NUMTX *result;
        NUMTX *orientation;
        NUVEC direction;
        f32 magnitude;
        buffer->addr = (buffer->addr + 3) & ~3u;
        result = buffer->mtx_ptr;
        *result = *matrix;
        magnitude = NuFsqrt((f32)(((f64)NuVecMagSqr(&buffer->mtx_ptr->rows[1].xyz) + NuVecMagSqr(&buffer->mtx_ptr->rows[0].xyz) +
                                   NuVecMagSqr(&buffer->mtx_ptr->rows[2].xyz)) *
                                  (f64)0.33333334f));
        buffer->addr += sizeof(NUMTX);
        *buffer->f32_ptr = magnitude;
        buffer->addr += sizeof(f32);
        orientation = buffer->mtx_ptr;
        if (((PC_SPECIAL_FACEON *)faceon)->kind == 0) {
            direction.x = direction.y = direction.z = 0.0f;
            buffer->addr += sizeof(NUMTX);
            NuMtxCalcCheapFaceOn(orientation, &direction);
        } else {
            direction.x = matrix->m10;
            direction.y = matrix->m11;
            direction.z = matrix->m12;
            buffer->addr += sizeof(NUMTX);
            NuMtxCalcCheapFaceY_v2(orientation, &direction);
        }
        return result;
    }

    i32 fun_00603700(void) {
        return 0;
    }

    i32 fun_006038f0(void) {
        return 0;
    }

    i32 fun_00603900(void) {
        return 0;
    }

    void *DisplayListCreateSkinTransformPS(VARIPTR *buffer, NUMTX *skin, void *weights, void *geometry_ptr, void **render_geometry,
                                           void *unused) {
        PC_SPECIAL_GEOMETRY *geometry = (PC_SPECIAL_GEOMETRY *)geometry_ptr;
        i32 *matrix_count;
        u32 vertex;
        u32 morph;
        u8 *destination;
        void *locked;
        PC_SPECIAL_GEOMETRY *copy;
        i32 i;
        if (weights != NULL && geometry->morph_count != 0) {
            *render_geometry = (void *)((buffer->addr + 31) & ~31u);
            buffer->addr = ((buffer->addr + 31) & ~31u) + sizeof(*copy);
            copy = (PC_SPECIAL_GEOMETRY *)*render_geometry;
            *copy = *geometry;
            locked = NULL;
            PCDeviceLockVertexBuffer(geometry->base_vertex * geometry->vertex_stride, geometry->vertex_count * geometry->vertex_stride,
                                     &locked, 0x10, geometry->vertex_buffer);
            copy->dynamic_vertices = (u8 *)((buffer->addr + 31) & ~31u);
            buffer->addr = (buffer->addr + 31) & ~31u;
            buffer->addr += geometry->vertex_count * geometry->vertex_stride;
            destination = (u8 *)copy->dynamic_vertices;
            PCByteCopy(destination, locked, geometry->vertex_count * geometry->vertex_stride);
            for (vertex = 0; vertex != geometry->vertex_count; ++vertex) {
                for (morph = 0; morph != geometry->morph_count; ++morph) {
                    f32 weight = ((PC_SPECIAL_DEFORMER_WEIGHTS *)weights)->weights[morph];
                    if (geometry->morph_deltas[morph] != NULL && weight != 0.0f) {
                        ((NUVEC *)destination)->x += geometry->morph_deltas[morph][vertex].x * weight;
                        ((NUVEC *)destination)->y += geometry->morph_deltas[morph][vertex].y * weight;
                        ((NUVEC *)destination)->z += geometry->morph_deltas[morph][vertex].z * weight;
                    }
                }
                locked = (u8 *)locked + geometry->vertex_stride;
                destination += geometry->vertex_stride;
            }
            PCDeviceUnlockVertexBuffer(geometry->vertex_buffer);
        } else {
            geometry->dynamic_vertices = NULL;
            *render_geometry = geometry;
        }
        matrix_count = buffer->i32_ptr;
        buffer->addr += sizeof(*matrix_count);
        *matrix_count = 0;
        for (i = 0; geometry->joint_indices[i] != 255 && i < 8; ++i) {
            *buffer->mtx_ptr = skin[geometry->joint_indices[i]];
            buffer->addr += sizeof(NUMTX);
            ++*matrix_count;
        }
        return matrix_count;
    }

    void DisplayListPrintItemPS(NUDISPLAYLISTITEM *item, i32 file_handle) {
    }

    void DisplayListSwapBuffersPS(void) {
    }

    void fun_00603930(void) {
    }

    void fun_00603940(void) {
    }

    void fun_00603950(void) {
    }

    void DisplayGenerateTransformsNoOp(NUDISPLAY_LIST *scene) {
    }

    void *NuDisplayListPrepareFaceonPS(VARIPTR *buffer, void *faceon, NUMTX *matrix) {
        return faceon;
    }

    void NuDisplaySceneAddPS(NUDISPLAY_LIST *scene) {
        i32 i;
        for (i = 0; i < scene->item_count; ++i) {
            NUDISPLAYLISTITEM *item = &scene->items[i];
            if (item->type == NUDLIST_GEOMCALL) {
                item->type = NUDLIST_GEOMCALL;
            }
        }
    }

    void NuDisplaySceneDestroyPS(NUDISPLAY_LIST *scene) {
    }

    i32 fun_006039c0(void) {
        return 1;
    }

    void NuDisplaySceneClonePS(NUDISPLAY_LIST *source_scene, NUDISPLAY_LIST *clone_scene, VARIPTR *buffer) {
        u32 *word;
        clone_scene->native_word = NULL;
        if (source_scene->native_word != NULL) {
            word = (u32 *)((buffer->addr + 3) & ~3u);
            buffer->addr = ((buffer->addr + 3) & ~3u) + sizeof(*word);
            *word = *source_scene->native_word;
            clone_scene->native_word = word;
        }
    }

    void NuDisplayListVertexGroups(void *arg) {
        PC_DL_VERTEX_GROUPS_PACKET *packet = (PC_DL_VERTEX_GROUPS_PACKET *)arg;
        NUVEC4 *values = packet->values;
        i32 groups = (i32)((u32)packet->count + 3u) / 4;
        i32 i;
        for (i = 0; i < groups && i < 32; ++i, ++values) {
            NuShaderManagerSetElementfv(SHADER_UNIFORM_VERTEX_GROUP_STATES, i, &values->x);
        }
    }

    void NuDisplayListVertexOffsets(void *arg) {
        PC_DL_VERTEX_OFFSETS_PACKET *packet = (PC_DL_VERTEX_OFFSETS_PACKET *)arg;
        i32 count = packet->count;
        i32 i;
        NUVEC4 *values = packet->values;
        for (i = 0; i < count && i < 8; ++i, ++values) {
            NuShaderManagerSetElementfv(SHADER_UNIFORM_OFFSET_TABLE, i, &values->x);
        }
    }

    void PostShadowSelectHandlers(i32 mode) {
        switch (mode) {
            case 0:
                CurrentItemTable = PCDisplayItemHandlerPrefix;
                break;
            case 1:
                CurrentItemTable = PCDisplayShadowItemHandlers;
                break;
            default:
                PCGraphicsRenderTextureAllocationError();
                break;
        }
    }

    void DisplayListSetAlphaPS(NUDISPLAYLISTITEM *item, NUDISPLAYLISTITEM *geometry, f32 alpha) {
        NUMTX *matrix = (NUMTX *)item->next;
        matrix->m33 = alpha < 0.0f ? 0.0f : (alpha > 1.0f ? 1.0f : alpha);
    }

    void DisplayListSetShadowCasterFlagPS(NUDISPLAYLISTITEM *first, NUDISPLAYLISTITEM *last, i32 shadow) {
        PC_SPECIAL_TRANSFORM_PACKET *packet = (PC_SPECIAL_TRANSFORM_PACKET *)first->next;
        packet->matrix.m32 = (f32)shadow;
    }

    void NuDisplayListCamera(void *arg) {
        PC_DL_CAMERA_PACKET *packet = (PC_DL_CAMERA_PACKET *)arg;
        if (packet->id != NuDisplayHandlerCameraId) {
            PC_VIEWPORT viewport;
            NUMTX *view;
            PC_DL_CAMERA_PROJECTION_PACKET *projection;
            i32 viewport_x;
            view = &packet->view;
            projection = (PC_DL_CAMERA_PROJECTION_PACKET *)(view + 1);
            NuDisplayHandlerCameraId = packet->id;
            PcUpdateGeometryCameraOrigin(view, &projection->projection);
            viewport_x = (i32)projection->viewport[0];
            viewport.x = (u32)viewport_x;
            viewport.y = (u32)projection->viewport[1];
            viewport.width = (u32)projection->viewport[2];
            viewport.height = (u32)projection->viewport[3];
            viewport.min_z = 0.0f;
            viewport.max_z = 1.0f;
            NuVpSubmit(&viewport);
        }
    }

    void PCGraphicsInitRenderTexturePool(VARIPTR *buffer, VARIPTR end) {
        i32 index;
        memset(PCGraphicsRenderTextures, 0, sizeof(PCGraphicsRenderTextures));
        for (index = 0; index != 32; ++index) {
            PCGraphicsRenderTextureActive[index] = 0;
            PCGraphicsResetRenderTexture(&PCGraphicsRenderTextures[index]);
        }
    }

    void PCGraphicsDestroyAllRenderTextures(void) {
        i32 i;
        for (i = 0; i != 32; ++i) {
            PCGraphicsRenderTextureActive[i] = 0;
            PCGraphicsDestroyRenderTexture(&PCGraphicsRenderTextures[i]);
        }
    }

    /* Claims a free render-texture slot; inlined into each creator. */
    static GRAPHICS_RENDER_TEXTURE *PCGraphicsAllocRenderTexture(void) {
        for (i32 i = 0; i != 32; ++i) {
            if (PCGraphicsRenderTextureActive[i] == 0) {
                PCGraphicsRenderTextureActive[i] = 1;
                return &PCGraphicsRenderTextures[i];
            }
        }
        PCGraphicsRenderTextureAllocationError();
        return NULL;
    }

    GRAPHICS_RENDER_TEXTURE *PCGraphicsCreateEffectTexture(i32 width, i32 height, i32 levels, i32 usage, i32 format) {
        GRAPHICS_RENDER_TEXTURE *texture = PCGraphicsAllocRenderTexture();
        usage &= ~0x10;
        switch (usage) {
            case 0:
                break;
            case 1:
                break;
            default:
                PCGraphicsRenderTextureAllocationError();
                return NULL;
        }
        PCGraphicsInitRenderTexture(texture, width, height, levels, usage, 0, format);
        return texture;
    }

    GRAPHICS_RENDER_TEXTURE *PCGraphicsRenderTextureFromId(i32 texture_id) {
        GRAPHICS_RENDER_TEXTURE *texture = PCGraphicsAllocRenderTexture();
        PCGraphicsRenderTextureFromTexture(texture, texture_id);
        return texture;
    }

    /* Returns the render texture behind a frame-copy texture id, or NULL for other textures. */
    GRAPHICS_RENDER_TEXTURE *PCGraphicsFrameRenderTexture(i32 texture_id) {
        FRAME_NATIVE_TEXTURE *texture = (FRAME_NATIVE_TEXTURE *)NuTextureGetDimensions(texture_id);
        u32 index = (u32)(texture - FrameNativeTexturePool.records);
        if (index > 15) {
            return NULL;
        }
        return texture->render_texture;
    }

    GRAPHICS_RENDER_TEXTURE *PCGraphicsCreateColourTarget(i32 divider, i32 levels) {
        i32 format = PCGraphicsFormatFromD3d(PCDeviceBackend.parameters.format);
        u32 height = PCDeviceBackend.parameters.height / (u32)divider;
        u32 width = PCDeviceBackend.parameters.width / (u32)divider;
        GRAPHICS_RENDER_TEXTURE *texture = PCGraphicsAllocRenderTexture();
        PCGraphicsInitRenderTexture(texture, width, height, levels, 1, 0, format);
        texture->flags = (texture->flags & 0xffffdbffu) | 0x1b00;
        return texture;
    }

    GRAPHICS_RENDER_TEXTURE *PCGraphicsCreateColourTargetFormat(i32 divider, i32 levels, i32 format) {
        u32 height = PCDeviceBackend.parameters.height / (u32)divider;
        u32 width = PCDeviceBackend.parameters.width / (u32)divider;
        GRAPHICS_RENDER_TEXTURE *texture = PCGraphicsAllocRenderTexture();
        PCGraphicsInitRenderTexture(texture, width, height, levels, 1, 0, format);
        texture->flags = (texture->flags & 0xffffdbffu) | 0x1b00;
        return texture;
    }

    void PCGraphicsResizeEffectTextureToScreen(GRAPHICS_RENDER_TEXTURE *texture, u32 divisor, i32 format) {
        PCGraphicsResizeEffectTexture(texture, PCDeviceBackend.parameters.width / divisor, PCDeviceBackend.parameters.height / divisor,
                                      format);
    }

    void PCGraphicsResizeEffectTextureTo(GRAPHICS_RENDER_TEXTURE *texture, i32 width, i32 height, i32 format) {
        PCGraphicsResizeEffectTexture(texture, width, height, format);
    }

    PC_FRAMEBUFFER *PCGraphicsCreateFramebuffer(void) {
        i32 index;
        for (index = 0; index != 32; ++index) {
            if (PcFramebufferActive[index] == 0) {
                PcFramebufferActive[index] = 1;
                PC_FRAMEBUFFER *frame = &PcFramebufferPool[index];
                if (frame == NULL) {
                    PCGraphicsRenderTextureAllocationError();
                    return NULL;
                }
                for (index = 0; index != 5; ++index) {
                    frame->targets[index].Reset();
                    frame->clear_colour[index] = 0xffffffff;
                }
                frame->changed = 1;
                return frame;
            }
        }
        PCGraphicsRenderTextureAllocationError();
        return NULL;
    }

    void PCGraphicsDestroyFramebuffer(PC_FRAMEBUFFER *frame) {
        for (i32 index = 0; index != 5; ++index) {
            frame->targets[index].Reset();
        }
        for (i32 index = 0; index != 32; ++index) {
            if (&PcFramebufferPool[index] == frame) {
                PcFramebufferActive[index] = 0;
            }
        }
    }

    void PCGraphicsAttachFramebuffer(PC_FRAMEBUFFER *frame, i32 index, GRAPHICS_RENDER_TEXTURE *texture, i32 level) {
        frame->targets[index].Attach(texture, level, false);
        frame->changed = 1;
    }

    void PCGraphicsAttachOwnedFramebuffer(PC_FRAMEBUFFER *frame, i32 index, GRAPHICS_RENDER_TEXTURE *texture, i32 level) {
        frame->targets[index].Attach(texture, level, true);
    }

    void PCGraphicsReleaseFramebuffers(void) {
        PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 0, NULL, 0);
        PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 1, NULL, 0);
        PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 2, NULL, 0);
        PCGraphicsAttachFramebuffer(PcSecondaryFramebuffer, 0, NULL, 0);
    }

    void PCGraphicsRestoreFramebuffers(void) {
        PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 0, PCDeviceBackend.textures[0], 0);
        PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 1, PCDeviceBackend.textures[1], 0);
        PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 2, PCDeviceBackend.textures[2], 0);
        PCGraphicsAttachFramebuffer(PcSecondaryFramebuffer, 0, PCDeviceBackend.textures[0], 0);
    }

    PC_EFFECT_SURFACE::PC_EFFECT_SURFACE() {
        texture = NULL;
        owns_texture = 0;
        surface = NULL;
        level = 0;
        face = 0;
        height = width = 0;
        registry_index = -1;
        Reset();
        if (!PcSurfaceRegistryInitialized) {
            memset(PcSurfaceRegistry, 0, sizeof(PcSurfaceRegistry));
            PcSurfaceRegistryInitialized = 1;
        }
        for (i32 index = 0; index != 256; ++index) {
            if (PcSurfaceRegistry[index] == NULL) {
                PcSurfaceRegistry[index] = this;
                registry_index = (i16)index;
                return;
            }
        }
    }

    inline PC_EFFECT_SURFACE::~PC_EFFECT_SURFACE() {
        Reset();
        PcSurfaceRegistry[registry_index] = NULL;
    }

    u8 PcInitializeGeometryBasis(PC_GEOMETRY_BASIS_SOURCE *source, NUMTX *palette, PC_GEOMETRY_MORPH_WEIGHTS *morph) {
        NUMTX identity;
        i32 morph_count;
        if (source->record_count > PcGeometryMatrixCapacity || PcGeometryTransforms == NULL) {
            PCClipScreenUnsupported();
            return 0;
        }
        PcGeometryCopyMatrix(&identity, &ANI3_EVAL_IDENTITY_MATRIX);
        morph_count = source->morph_count;
        if (morph == NULL) {
            morph_count = 0;
        }
        for (u32 i = 0; i < (u32)source->record_count; ++i) {
            PC_GEOMETRY_BASIS_INPUT_RECORD *input = &source->records[i];
            if (morph_count == 0 && input->bones[0] == 0xff && input->bones[1] == 0xff && input->bones[2] == 0xff) {
                PcGeometryTransforms[i].rows[0] = input->rows[0];
                PcGeometryTransforms[i].rows[1] = input->rows[1];
                PcGeometryTransforms[i].rows[2] = input->rows[2];
                continue;
            }
            PC_GEOMETRY_BASIS_VECTOR point = input->rows[0];
            for (i32 j = 0; j < morph_count; ++j) {
                NUVEC delta;
                NuVecScale(&delta, &source->morph_deltas[j].vectors[i].xyz, morph->weight_words[j + 2]);
                NuVecAdd(&point.xyz, &point.xyz, &delta);
            }
            NUMTX *matrices[3];
            matrices[0] = input->bones[0] < 0xff ? &palette[input->bones[0]] : &identity;
            matrices[1] = input->bones[1] < 0xff ? &palette[input->bones[1]] : &identity;
            matrices[2] = input->bones[2] < 0xff ? &palette[input->bones[2]] : &identity;
            PC_GEOMETRY_BASIS_VECTOR t0;
            PC_GEOMETRY_BASIS_VECTOR t1;
            PC_GEOMETRY_BASIS_VECTOR t2;
            NuVecMtxTransform(&t0.xyz, &point.xyz, matrices[0]);
            NuVecMtxTransform(&t1.xyz, &point.xyz, matrices[1]);
            NuVecMtxTransform(&t2.xyz, &point.xyz, matrices[2]);
            {
                PC_GEOMETRY_BASIS_VECTOR p0;
                PC_GEOMETRY_BASIS_VECTOR p1;
                PC_GEOMETRY_BASIS_VECTOR p2;
                PC_GEOMETRY_BASIS_VECTOR partial;
                PC_GEOMETRY_BASIS_VECTOR sum;
                {
                    f32 scale = input->weights.z;
                    p2.x = t2.x * scale;
                    p2.y = t2.y * scale;
                    p2.z = t2.z * scale;
                }
                {
                    f32 scale = input->weights.y;
                    p1.x = t1.x * scale;
                    p1.y = t1.y * scale;
                    p1.z = t1.z * scale;
                }
                {
                    f32 scale = input->weights.x;
                    p0.x = t0.x * scale;
                    p0.y = t0.y * scale;
                    p0.z = t0.z * scale;
                }
                partial.x = p0.x + p1.x;
                partial.y = p0.y + p1.y;
                partial.z = p0.z + p1.z;
                sum.x = partial.x + p2.x;
                sum.y = partial.y + p2.y;
                sum.z = partial.z + p2.z;
                PcGeometryTransforms[i].rows[0] = sum;
            }
            {
                f32 x = input->rows[1].x * matrices[0]->m00 + input->rows[1].y * matrices[0]->m10 + input->rows[1].z * matrices[0]->m20;
                f32 y = input->rows[1].x * matrices[0]->m01 + input->rows[1].y * matrices[0]->m11 + input->rows[1].z * matrices[0]->m21;
                t0.z = input->rows[1].x * matrices[0]->m02 + input->rows[1].y * matrices[0]->m12 + input->rows[1].z * matrices[0]->m22;
                t0.x = x;
                t0.y = y;
            }
            {
                f32 x = input->rows[1].x * matrices[1]->m00 + input->rows[1].y * matrices[1]->m10 + input->rows[1].z * matrices[1]->m20;
                f32 y = input->rows[1].x * matrices[1]->m01 + input->rows[1].y * matrices[1]->m11 + input->rows[1].z * matrices[1]->m21;
                t1.z = input->rows[1].x * matrices[1]->m02 + input->rows[1].y * matrices[1]->m12 + input->rows[1].z * matrices[1]->m22;
                t1.x = x;
                t1.y = y;
            }
            {
                f32 x = input->rows[1].x * matrices[2]->m00 + input->rows[1].y * matrices[2]->m10 + input->rows[1].z * matrices[2]->m20;
                f32 y = input->rows[1].x * matrices[2]->m01 + input->rows[1].y * matrices[2]->m11 + input->rows[1].z * matrices[2]->m21;
                t2.z = input->rows[1].x * matrices[2]->m02 + input->rows[1].y * matrices[2]->m12 + input->rows[1].z * matrices[2]->m22;
                t2.x = x;
                t2.y = y;
            }
            {
                PC_GEOMETRY_BASIS_VECTOR p0;
                PC_GEOMETRY_BASIS_VECTOR p1;
                PC_GEOMETRY_BASIS_VECTOR p2;
                PC_GEOMETRY_BASIS_VECTOR partial;
                PC_GEOMETRY_BASIS_VECTOR sum;
                {
                    f32 scale = input->weights.z;
                    p2.x = t2.x * scale;
                    p2.y = t2.y * scale;
                    p2.z = t2.z * scale;
                }
                {
                    f32 scale = input->weights.y;
                    p1.x = t1.x * scale;
                    p1.y = t1.y * scale;
                    p1.z = t1.z * scale;
                }
                {
                    f32 scale = input->weights.x;
                    p0.x = t0.x * scale;
                    p0.y = t0.y * scale;
                    p0.z = t0.z * scale;
                }
                partial.x = p0.x + p1.x;
                partial.y = p0.y + p1.y;
                partial.z = p0.z + p1.z;
                sum.x = partial.x + p2.x;
                sum.y = partial.y + p2.y;
                sum.z = partial.z + p2.z;
                PcGeometryTransforms[i].rows[1] = sum;
            }
            {
                f32 x = input->rows[2].x * matrices[0]->m00 + input->rows[2].y * matrices[0]->m10 + input->rows[2].z * matrices[0]->m20;
                f32 y = input->rows[2].x * matrices[0]->m01 + input->rows[2].y * matrices[0]->m11 + input->rows[2].z * matrices[0]->m21;
                t0.z = input->rows[2].x * matrices[0]->m02 + input->rows[2].y * matrices[0]->m12 + input->rows[2].z * matrices[0]->m22;
                t0.x = x;
                t0.y = y;
            }
            {
                f32 x = input->rows[2].x * matrices[1]->m00 + input->rows[2].y * matrices[1]->m10 + input->rows[2].z * matrices[1]->m20;
                f32 y = input->rows[2].x * matrices[1]->m01 + input->rows[2].y * matrices[1]->m11 + input->rows[2].z * matrices[1]->m21;
                t1.z = input->rows[2].x * matrices[1]->m02 + input->rows[2].y * matrices[1]->m12 + input->rows[2].z * matrices[1]->m22;
                t1.x = x;
                t1.y = y;
            }
            {
                f32 x = input->rows[2].x * matrices[2]->m00 + input->rows[2].y * matrices[2]->m10 + input->rows[2].z * matrices[2]->m20;
                f32 y = input->rows[2].x * matrices[2]->m01 + input->rows[2].y * matrices[2]->m11 + input->rows[2].z * matrices[2]->m21;
                t2.z = input->rows[2].x * matrices[2]->m02 + input->rows[2].y * matrices[2]->m12 + input->rows[2].z * matrices[2]->m22;
                t2.x = x;
                t2.y = y;
            }
            {
                PC_GEOMETRY_BASIS_VECTOR p0;
                PC_GEOMETRY_BASIS_VECTOR p1;
                PC_GEOMETRY_BASIS_VECTOR p2;
                PC_GEOMETRY_BASIS_VECTOR partial;
                PC_GEOMETRY_BASIS_VECTOR sum;
                {
                    f32 scale = input->weights.z;
                    p2.x = t2.x * scale;
                    p2.y = t2.y * scale;
                    p2.z = t2.z * scale;
                }
                {
                    f32 scale = input->weights.y;
                    p1.x = t1.x * scale;
                    p1.y = t1.y * scale;
                    p1.z = t1.z * scale;
                }
                {
                    f32 scale = input->weights.x;
                    p0.x = t0.x * scale;
                    p0.y = t0.y * scale;
                    p0.z = t0.z * scale;
                }
                partial.x = p0.x + p1.x;
                partial.y = p0.y + p1.y;
                partial.z = p0.z + p1.z;
                sum.x = partial.x + p2.x;
                sum.y = partial.y + p2.y;
                sum.z = partial.z + p2.z;
                PcGeometryTransforms[i].rows[2] = sum;
            }
        }
        return 1;
    }

    i32 NuRndrSetAmbientLightSpecular(const NUVEC4 *colour) {
        render_state.global_specular = colour->w;
        PCGlobalLightingState.ambient = colour->xyz;
        NuRndrSetAmbientLightPS(&colour->xyz);
        return 0;
    }

    void DisplayListUpdateRenderState(NUDISPLAYLIST *list, NUGLOBALRNDRSTATE *global) {
        void *packet;

        if (global == NULL) {
            return;
        }
        if (list->state->global_id == global->global_id) {
            return;
        }

        if ((u16)list->state->lights_id != global->lights_id) {
            if (global->light_packet_cache == NULL) {
                global->light_packet_cache = DisplayListBuildLightPacket(global);
            }
            NuDisplayListLinkItem(list, NUDLIST_LIGHTS, global->light_packet_cache);
            list->state->lights_id = global->lights_id;
        }

        if ((u16)list->state->camera_id != global->camera_id) {
            if (global->camera_packet_cache == NULL) {
                NUMTX projection;
                VARIPTR *cursor;
                f32 *viewport;
                memset(&projection.m01, 0, sizeof(projection) - sizeof(projection.m00));
                projection.m00 = global->projection_0154;
                projection.m11 = global->projection_0158;
                projection.m22 = global->projection_m22;
                projection.m23 = global->projection_0160;
                projection.m32 = global->projection_0164;
                NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
                cursor = NuDisplayListStreamCursor;
                global->camera_packet_cache = cursor->void_ptr;
                *cursor->i32_ptr = ((i16)global->camera_id + 5) * (global->global_id + 13) + nuapi.frame_count;
                cursor->addr += sizeof(i32);
                *cursor->mtx_ptr = global->camera_matrix;
                cursor->addr += sizeof(NUMTX);
                *cursor->mtx_ptr = projection;
                cursor->addr += sizeof(NUMTX);
                viewport = cursor->f32_ptr;
                *viewport++ = global->viewport_x;
                cursor->void_ptr = viewport;
                *viewport++ = global->viewport_y;
                cursor->void_ptr = viewport;
                *viewport++ = global->viewport_width;
                cursor->void_ptr = viewport;
                *viewport++ = global->viewport_height;
                cursor->void_ptr = viewport;
            }
            NuDisplayListLinkItem(list, NUDLIST_CAMERA, global->camera_packet_cache);
            list->state->camera_id = global->camera_id;
        }

        if ((u16)list->state->fog_id != global->fog_id) {
            if (global->fog_packet_cache == NULL) {
                global->fog_packet_cache = DisplayListBuildFogPacket(global);
            }
            NuDisplayListLinkItem(list, NUDLIST_FOG, global->fog_packet_cache);
            list->state->fog_id = global->fog_id;
        }

        if ((u16)list->state->konst_id != global->konst_id) {
            if (global->konst_packet_cache == NULL) {
                global->konst_packet_cache = DisplayListBuildKonstPacket(global);
            }
            NuDisplayListLinkItem(list, NUDLIST_KONST, global->konst_packet_cache);
            list->state->konst_id = global->konst_id;
        }

        if ((u16)list->state->reflection_id != global->reflection_id) {
            if (global->reflection_packet_cache == NULL) {
                global->reflection_packet_cache = DisplayListBuildReflectionPacket(global);
            }
            NuDisplayListLinkItem(list, NUDLIST_REFLECTION, global->reflection_packet_cache);
            list->state->reflection_id = global->reflection_id;
        }

        if ((u16)list->state->vertex_groups_id != global->vertex_groups_id) {
            packet = DisplayListBuildVertexGroupPacket(global);
            if (packet != NULL) {
                NuDisplayListLinkItem(list, NUDLIST_VERTEX_GROUPS, packet);
            }
            list->state->vertex_groups_id = global->vertex_groups_id;
        }

        list->state->global_id = global->global_id;
    }

    void DisplayListUpdateRenderStateShadow(NUDISPLAYLIST *list, NUGLOBALRNDRSTATE *state) {
        if (state == NULL) {
            return;
        }
        if (list->state->global_id == state->global_id) {
            return;
        }
        if ((u16)list->state->vertex_groups_id != state->vertex_groups_id) {
            void *packet = DisplayListBuildVertexGroupPacket(state);
            if (packet != NULL) {
                NuDisplayListLinkItem(list, NUDLIST_VERTEX_GROUPS, packet);
            }
            list->state->vertex_groups_id = state->vertex_groups_id;
        }
        list->state->global_id = state->global_id;
    }

    void NuDisplayListDrawGeometry(void *arg) {
        PC_DL_GEOMETRY_PACKET *packet = (PC_DL_GEOMETRY_PACKET *)arg;
        NUMTL *material = NuDisplayHandlerMaterial;
        u32 primitive_count;
        const void *vertices;
        if (packet == NULL || PCShaderCurrentObject() == NULL || packet->vertex_count == 0) {
            return;
        }
        primitive_count = packet->primitive_count;
        if (material->shader.layer_flags & 0x4000000) {
            NuDisplayListUpdateLightmap();
        }
        vertices = packet + 1;
        switch (packet->kind) {
            case 6:
                if (packet->dynamic_count != 0 && packet->dynamic_vertices != NULL) {
                    void *locked;
                    PCDeviceLockVertexBuffer(0, 0, &locked, 0x2000, packet->dynamic_buffer);
                    PCByteCopy(locked, packet->dynamic_vertices, packet->vertex_stride * packet->vertex_count);
                    PCDeviceUnlockVertexBuffer(packet->dynamic_buffer);
                    PCDeviceSetStreamSource(0, packet->dynamic_buffer, 0, packet->vertex_stride);
                    PCDeviceSetIndices(packet->index_buffer);
                    SAGA_SKIN_CALL(skin_trace_capture_geometry, (unsigned)packet, (unsigned)NuDisplayHandlerMaterial, PCShaderSlots,
                                   (unsigned)packet->dynamic_buffer, (unsigned)packet->index_buffer, 0, packet->start_index);
                    PCDeviceDrawIndexedPrimitive(5, 0, 0, packet->vertex_count, packet->start_index, primitive_count);
                } else {
                    PCDeviceSetStreamSource(0, packet->vertex_buffer, 0, packet->vertex_stride);
                    PCDeviceSetIndices(packet->index_buffer);
                    SAGA_SKIN_CALL(skin_trace_capture_geometry, (unsigned)packet, (unsigned)NuDisplayHandlerMaterial, PCShaderSlots,
                                   (unsigned)packet->vertex_buffer, (unsigned)packet->index_buffer, packet->base_vertex,
                                   packet->start_index);
                    PCDeviceDrawIndexedPrimitive(5, packet->base_vertex, 0, packet->vertex_count, packet->start_index, primitive_count);
                }
                break;
            case 12:
                PCDeviceDrawPrimitiveUPPc(4, packet->vertex_count / 3u, vertices, 24);
                break;
            case 13:
                if (packet->vertex_count >= 3u) {
                    PCDeviceDrawPrimitiveUPPc(5, packet->vertex_count - 2u, vertices, 24);
                }
                break;
            case 14:
                PCDeviceDrawPrimitiveUPPc(2, packet->vertex_count >> 1, vertices, 24);
                break;
            case 15:
                PCDeviceDrawPrimitiveUPPc(3, packet->vertex_count - 1u, vertices, 24);
                break;
            case 17:
                if (packet->vertex_count >= 3u) {
                    PCDeviceDrawPrimitiveUPPc(6, packet->vertex_count - 2u, vertices, 24);
                }
                break;
            case 18: {
                u32 point_size;
                f32 point_size_float;
                point_size_float = 32.0f;
                memcpy(&point_size, &point_size_float, sizeof(point_size));
                PCDeviceSetRenderState(D3DRS_POINTSIZE, point_size);
                PCDeviceDrawPrimitiveUPPc(1, packet->vertex_count, vertices, 24);
                point_size_float = 1.0f;
                memcpy(&point_size, &point_size_float, sizeof(point_size));
                PCDeviceSetRenderState(D3DRS_POINTSIZE, point_size);
                break;
            }
        }
    }

    void NuVpInit(void) {
        NuPs2GetViewport(&vpDevice);
        vpDevice.center_x = 0.5f;
        vpDevice.center_y = 0.5f;
        vpDevice.clip_min_x = 0.0f;
        vpDevice.clip_min_y = 0.0f;
        vpDevice.clip_max_x = 1.0f;
        vpDevice.clip_max_y = 1.0f;
        vpDevice.height *= 16;
        vpDevice.width *= 16;
        vpDevice.x *= 16;
        vpDevice.y *= 16;
        vpCurrent = vpDevice;
        vport_inval = 1;
        NuVpUpdate();
    }

    void NuInitDebrisRenderer(VARIPTR *buffer, VARIPTR buffer_end) {
        i32 index;
        NuDebrisMaxVertexCount = NuDebrisIdealDynamicVBSize / 24u;
        NuDebrisVertexBufferSize = NuDebrisMaxVertexCount * 24u;
        for (index = 0; index != 8; ++index) {
            NuDebrisSystemBuffers[index] = buffer->vertex_ptr;
            buffer->addr += NuDebrisVertexBufferSize;
        }
        debris_InitializeD3D();
    }

    void AddParticleGroupToDisplayList(NATIVE_PARTICLE_PACKET *group) {
        NUDISPLAYLIST *list = group->material->display_list;
        VARIPTR *buffer;
        list->scene->render_flags |= 2;
        list->scene->material_used[(list->scene->render_flags >> 15) & 1][list->material_id / 8] |= (u8)(1 << (list->material_id % 8));
        DisplayListUpdateRenderState(list, &render_state);
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15;
        buffer = NuDisplayListStreamCursor;
        list->mtl_last->next = buffer->void_ptr;
        list->items = buffer->dlist_ptr;
        buffer->addr += sizeof(NUDISPLAYLISTITEM);
        buffer->dlist_ptr->type = NUDLIST_NEXT;
        buffer->dlist_ptr->id = NUDLISTITEM_JUMP;
        buffer->dlist_ptr->next = list->dynamic_geometry + 1;
        list->mtl_last = buffer->dlist_ptr;
        buffer->addr += sizeof(NUDISPLAYLISTITEM);
        list->items->type = 0xa7;
        list->items->id = NUDLISTITEM_CALL;
        list->items->next = group;
        list->items++;
    }

    void NuPrim2DBegin(u32 primitive_type, u32 vertex_format, NUMTL *material) {
        NUDISPLAYLIST *list;
        VARIPTR *primitive_buffer;
        VARIPTR *cursor;
        NUPRIM_PACKET *packet;

        if (material == NULL) {
            material = numtl_defaultmtl2d;
        }

        NuPrimNeedsOverbrightening = material->texture_id != 0;

        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 0xf) & ~0xfU;
        list = material->display_list;
        primitive_buffer = NuDisplayListStreamCursor;
        if (list != NULL) {
            list->scene->render_flags |= 2;
            list->scene->material_used[(list->scene->render_flags >> 15) & 1][list->material_id / 8] |= (u8)(1 << (list->material_id % 8));
        } else {
            list = &PCDisplayList2d;
            NuDisplayListLinkMtl(list, material);
        }

        render_state.global_id++;
        render_state.konst_id++;
        render_state.const_alpha = 0;
        render_state.const_alpha_material = NULL;
        render_state.const_alpha_enabled = 0;
        render_state.konst_packet_cache = NULL;
        DisplayListUpdateRenderState(list, &render_state);

        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        cursor = NuDisplayListStreamCursor;
        list->mtl_last->next = cursor->void_ptr;
        list->items = cursor->dlist_ptr;
        cursor->addr += sizeof(NUDISPLAYLISTITEM);
        cursor->dlist_ptr->type = NUDLIST_NEXT;
        cursor->dlist_ptr->id = NUDLISTITEM_JUMP;
        cursor->dlist_ptr->next = list->dynamic_geometry + 1;
        list->mtl_last = cursor->dlist_ptr;
        cursor->addr += sizeof(NUDISPLAYLISTITEM);
        NuPrimStreamBufferPointer = primitive_buffer;
        packet = (NUPRIM_PACKET *)primitive_buffer->void_ptr;
        primitive_buffer->void_ptr = packet + 1;
        packet->primitive_type = primitive_type;
        NuPrimVertexCount = 0;
        NuPrimVertexCountPointer = &packet->vertex_count;
        list->items->type = NUDLIST_GEOMCALL2D;
        NuPrimCurrentPrimitiveType = (i32)primitive_type;
        list->items->id = NUDLISTITEM_CALL;
        list->items->next = packet;
        list->items++;
    }

    void NuPrim3DBegin(u32 primitive_type, u32 vertex_format, NUMTL *material, NUMTX *world_matrix) {
        NUDISPLAYLIST *list;
        VARIPTR *cursor;
        VARIPTR *primitive_buffer;
        void *transform;
        NUPRIM_PACKET *geometry;
        if (material == NULL) {
            material = numtl_defaultmtl3d;
        }
        NuPrimNeedsOverbrightening = material->texture_id != 0;
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        list = material->display_list;
        if (list != NULL) {
            list->scene->render_flags |= 2;
            list->scene->material_used[(list->scene->render_flags >> 15) & 1][list->material_id / 8] |= (u8)(1 << (list->material_id % 8));
        } else {
            list = &PCDisplayList2d;
        }
        DisplayListUpdateRenderState(list, &render_state);
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        cursor = NuDisplayListStreamCursor;
        list->mtl_last->next = cursor->void_ptr;
        list->items = cursor->dlist_ptr;
        cursor->addr += 2 * sizeof(NUDISPLAYLISTITEM);
        cursor->dlist_ptr->type = NUDLIST_NEXT;
        cursor->dlist_ptr->id = NUDLISTITEM_JUMP;
        cursor->dlist_ptr->next = list->dynamic_geometry + 1;
        list->mtl_last = cursor->dlist_ptr;
        cursor->addr += sizeof(NUDISPLAYLISTITEM);
        NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15u;
        primitive_buffer = NuDisplayListStreamCursor;
        if (world_matrix == NULL) {
            world_matrix = &numtx_identity;
        }
        transform = DisplayListCreateGeomTransformPS(primitive_buffer, world_matrix, NULL, NULL, NULL);
        geometry = (NUPRIM_PACKET *)primitive_buffer->void_ptr;
        primitive_buffer->void_ptr = geometry + 1;
        geometry->primitive_type = primitive_type;
        NuPrimVertexCountPointer = &geometry->vertex_count;
        list->items->type = NUDLIST_TRANSFORM_REF;
        list->items->id = NUDLISTITEM_CALL;
        list->items->next = transform;
        list->items++;
        list->items->type = NUDLIST_GEOMCALL;
        list->items->id = NUDLISTITEM_CALL;
        list->items->next = geometry;
        list->items++;
    }

    void NuLgtLaserDraw(i32 paused) {
        i32 i;
        i32 k;
        f32 tex_v[4];
        f32 tex_u[4];
        u32 saved_seed;
        NUVEC4 *vertices;
        NUMTX clip_matrix;
        f32 aspect;
        u32 colour;
        i32 red;
        i32 green;
        i32 blue;
        i32 alpha;
        i32 reds[2];
        i32 greens[2];
        i32 blues[2];
        i32 alphas[2];
        f32 dx;
        f32 dy;
        f32 dz;
        f32 remaining;
        NUVEC perpendicular;
        f32 length;
        f32 step;
        f32 width_scale;
        f32 fraction;
        f64 next_fraction;
        f64 wobble_limit;
        f32 offset_x;
        f32 offset_y;
        f32 wobble_x;
        f32 wobble_y;
        f32 neg_y;
        NULGTVERTEX *vertex;
        if (paused) {
            NuLgtLaserCnt = NuLgtLaserOldCnt;
        }
        if (NuLgtArcMtl[0].material == NULL || NuLgtLaserCnt == 0) {
            NuLgtLaserOldCnt = 0;
            return;
        }

        tex_v[0] = NuLgtArcMtl[0].v0;
        tex_v[1] = NuLgtArcMtl[0].v0;
        tex_v[2] = NuLgtArcMtl[0].v1;
        tex_v[3] = NuLgtArcMtl[0].v1;
        saved_seed = NuLgtSeed;
        // Four strip corners, two projected endpoints, and two clipped world endpoints.
        vertices = (NUVEC4 *)NuScratchAlloc32(0x80);
        NuCameraGetClipMtx(&clip_matrix, NULL);
        aspect = (f32)PS2_REZ_W / PS2_REZ_H;

        for (i = 0; i < NuLgtLaserCnt; ++i) {
            vertices[4].x = NuLgtLaserData[i].start.x;
            vertices[4].y = NuLgtLaserData[i].start.y;
            vertices[4].z = NuLgtLaserData[i].start.z;
            vertices[4].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[4], &vertices[4], &clip_matrix);
            vertices[5].x = NuLgtLaserData[i].end.x;
            vertices[5].y = NuLgtLaserData[i].end.y;
            vertices[5].z = NuLgtLaserData[i].end.z;
            vertices[5].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[5], &vertices[5], &clip_matrix);
            if (!(vertices[4].w >= 0.5f) && !(vertices[5].w >= 0.5f)) {
                continue;
            }

            if (vertices[4].w < 0.5f) {
                if (NuLgtLaserData[i].arc == 0) {
                    colour = NuLgtLaserData[i].colour;
                    red = colour & 0xff;
                    green = (colour >> 8) & 0xff;
                    blue = (u8)(colour >> 16);
                    alpha = (u8)(colour >> 24);
                } else {
                    red = (i32)((i32)(NuLgtLaserData[i].colour & 0xff) * (vertices[5].w - 0.5) / (vertices[5].w - vertices[4].w));
                    green = (i32)((((i32)NuLgtLaserData[i].colour >> 8) & 0xff) * (vertices[5].w - 0.5) / (vertices[5].w - vertices[4].w));
                    blue = (i32)((((i32)NuLgtLaserData[i].colour >> 16) & 0xff) * (vertices[5].w - 0.5) / (vertices[5].w - vertices[4].w));
                    alpha = (i32)((((i32)NuLgtLaserData[i].colour >> 24) & 0xff) * (vertices[5].w - 0.5) / (vertices[5].w - vertices[4].w));
                }
                vertices[6].x =
                    (NuLgtLaserData[i].start.x - NuLgtLaserData[i].end.x) * (vertices[5].w - 0.5) / (vertices[5].w - vertices[4].w) +
                    NuLgtLaserData[i].end.x;
                vertices[6].y =
                    (NuLgtLaserData[i].start.y - NuLgtLaserData[i].end.y) * (vertices[5].w - 0.5) / (vertices[5].w - vertices[4].w) +
                    NuLgtLaserData[i].end.y;
                vertices[6].z =
                    (NuLgtLaserData[i].start.z - NuLgtLaserData[i].end.z) * (vertices[5].w - 0.5) / (vertices[5].w - vertices[4].w) +
                    NuLgtLaserData[i].end.z;
            } else {
                vertices[6].x = NuLgtLaserData[i].start.x;
                vertices[6].y = NuLgtLaserData[i].start.y;
                vertices[6].z = NuLgtLaserData[i].start.z;
                colour = NuLgtLaserData[i].colour;
                red = colour & 0xff;
                green = (colour >> 8) & 0xff;
                blue = (u8)(colour >> 16);
                alpha = (u8)(colour >> 24);
            }
            vertices[6].w = 1.0f;

            remaining = (vertices[6].x - NuLgtLaserData[i].end.x) * (vertices[6].x - NuLgtLaserData[i].end.x) +
                        (vertices[6].y - NuLgtLaserData[i].end.y) * (vertices[6].y - NuLgtLaserData[i].end.y) +
                        (vertices[6].z - NuLgtLaserData[i].end.z) * (vertices[6].z - NuLgtLaserData[i].end.z);
            remaining = NuFsqrt(remaining);
            if (vertices[5].w < 0.5f) {
                vertices[7].x =
                    (NuLgtLaserData[i].end.x - NuLgtLaserData[i].start.x) * (vertices[4].w - 0.5) / (vertices[4].w - vertices[5].w) +
                    NuLgtLaserData[i].start.x;
                vertices[7].y =
                    (NuLgtLaserData[i].end.y - NuLgtLaserData[i].start.y) * (vertices[4].w - 0.5) / (vertices[4].w - vertices[5].w) +
                    NuLgtLaserData[i].start.y;
                vertices[7].z =
                    (NuLgtLaserData[i].end.z - NuLgtLaserData[i].start.z) * (vertices[4].w - 0.5) / (vertices[4].w - vertices[5].w) +
                    NuLgtLaserData[i].start.z;
            } else {
                vertices[7].x = NuLgtLaserData[i].end.x;
                vertices[7].y = NuLgtLaserData[i].end.y;
                vertices[7].z = NuLgtLaserData[i].end.z;
            }
            vertices[7].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[4], &vertices[6], &clip_matrix);
            NuVec4ScaleXYZVU0(&vertices[4], &vertices[4], 1.0f / vertices[4].w);
            NuVec4MtxTransformVU0(&vertices[5], &vertices[7], &clip_matrix);
            NuVec4ScaleXYZVU0(&vertices[5], &vertices[5], 1.0f / vertices[5].w);
            if (vertices[4].x < -1.0f && vertices[5].x < -1.0f) {
                continue;
            }
            if (vertices[4].x > 1.0f && vertices[5].x > 1.0f) {
                continue;
            }
            if (vertices[4].y < -1.0f && vertices[5].y < -1.0f) {
                continue;
            }
            if (vertices[4].y > 1.0f && vertices[5].y > 1.0f) {
                continue;
            }

            perpendicular.x = (vertices[5].y - vertices[4].y) * aspect;
            perpendicular.y = vertices[4].x - vertices[5].x;
            perpendicular.z = 0.0f;
            NuVecNorm(&perpendicular, &perpendicular);
            perpendicular.x *= aspect * NuLgtLaserData[i].width;
            perpendicular.y = NuLgtLaserData[i].width * perpendicular.y;
            dx = vertices[7].x - vertices[6].x;
            vertices[7].x = dx;
            dy = vertices[7].y - vertices[6].y;
            vertices[7].y = dy;
            dz = vertices[7].z - vertices[6].z;
            vertices[7].z = dz;
            vertices[7].w -= vertices[6].w;
            length = dx * dx + dy * dy + dz * dz;
            length = NuFsqrt(length);
            if (!(length > 0.0f)) {
                continue;
            }

            step = NuLgtLaserData[i].segment_length / length;
            // Per-laser seeds make repeated and paused rendering deterministic.
            NuLgtSeed = NuLgtLaserData[i].seed;
            if (step > 1.0f) {
                step = 1.0f;
            }

            tex_u[0] = tex_u[2] = NuLgtArcMtl[0].u0;
            tex_u[1] = tex_u[3] = NuLgtArcMtl[0].u1;
            vertices[4].x = vertices[6].x;
            vertices[4].y = vertices[6].y;
            vertices[4].z = vertices[6].z;
            vertices[4].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[4], &vertices[4], &clip_matrix);
            width_scale = 1.0f / vertices[4].w;
            NuVec4ScaleXYZVU0(&vertices[4], &vertices[4], width_scale);
            if (remaining < NuLgtLaserData[i].length) {
                if (remaining < 0.0f) {
                    width_scale *= NuLgtLaserData[i].end_width_ratio;
                } else {
                    width_scale += (NuLgtLaserData[i].end_width_ratio * width_scale - width_scale) *
                                   (NuLgtLaserData[i].length - remaining) / NuLgtLaserData[i].length;
                }
            }
            reds[0] = reds[1] = red;
            greens[0] = greens[1] = green;
            blues[0] = blues[1] = blue;
            alphas[0] = alphas[1] = alpha;
            offset_x = width_scale * perpendicular.x;
            offset_y = width_scale * perpendicular.y;
            vertices[1].x = vertices[4].x - offset_x;
            vertices[1].y = vertices[4].y - offset_y;
            vertices[3].x = vertices[4].x + offset_x;
            vertices[3].y = vertices[4].y + offset_y;
            vertices[1].z = vertices[4].z;
            vertices[3].z = vertices[4].z;
            remaining -= NuLgtLaserData[i].segment_length;

            fraction = 0.0f;
            wobble_limit = 1.0 - 1.5 * step;
            while (fraction < 1.0f) {
                next_fraction = fraction + step;
                if (next_fraction >= 1.0f) {
                    if (NuLgtLaserData[i].arc) {
                        alphas[1] = 0;
                    }
                    vertices[5].x = vertices[7].x + vertices[6].x;
                    vertices[5].y = vertices[6].y + vertices[7].y;
                    vertices[5].z = vertices[7].z + vertices[6].z;
                    tex_u[0] = tex_u[2] = NuLgtArcMtl[0].u1 + (1.0f - fraction) * (NuLgtArcMtl[0].u0 - NuLgtArcMtl[0].u1) / step;
                } else {
                    if (NuLgtLaserData[i].arc) {
                        alphas[1] = (i32)((1.0f - fraction - step) * alpha);
                    }
                    vertices[5].x = vertices[7].x * next_fraction + vertices[6].x;
                    vertices[5].y = vertices[7].y * next_fraction + vertices[6].y;
                    vertices[5].z = vertices[7].z * next_fraction + vertices[6].z;
                }
                vertices[5].w = 1.0f;
                NuVec4MtxTransformVU0(&vertices[5], &vertices[5], &clip_matrix);
                width_scale = 1.0f / vertices[5].w;
                NuVec4ScaleXYZVU0(&vertices[5], &vertices[5], width_scale);
                if (remaining < NuLgtLaserData[i].length) {
                    if (remaining < 0.0f) {
                        width_scale *= NuLgtLaserData[i].end_width_ratio;
                    } else {
                        width_scale += (width_scale * NuLgtLaserData[i].end_width_ratio - width_scale) *
                                       (NuLgtLaserData[i].length - remaining) / NuLgtLaserData[i].length;
                    }
                }

                if (fraction < wobble_limit && NuLgtLaserData[i].seed != 0) {
                    wobble_y = (f32)((NU_LGT_RAND() & 0xff) - 128);
                    wobble_x = wobble_y * perpendicular.x * NuLgtLaserData[i].width_wobble * width_scale;
                    wobble_y = wobble_y * perpendicular.y * NuLgtLaserData[i].width_wobble * width_scale;
                } else {
                    wobble_x = wobble_y = 0.0f;
                }
                offset_x = width_scale * perpendicular.x;
                offset_y = width_scale * perpendicular.y;
                vertices[0].x = vertices[5].x - offset_x + wobble_x;
                vertices[0].y = vertices[5].y - offset_y + wobble_y;
                vertices[2].x = vertices[5].x + offset_x + wobble_x;
                vertices[2].y = vertices[5].y + offset_y + wobble_y;
                vertices[0].z = vertices[5].z;
                vertices[2].z = vertices[5].z;
                // NuPrimSetCoordinateSystem(1), pushed.
                NuPrimCoordSystemStack[NuPrimCSPos++] = 1;
                NuPrimXScale = 1.0f;
                NuPrimYScale = -1.0f;
                NuPrimXBias = 0.0f;
                NuPrimYBias = 0.0f;
                NuPrim2DBegin(13, 7, NuLgtArcMtl[0].material);
                for (k = 0; k < 4; ++k) {
                    NuLgtSetVertexColor((alphas[1 - (k & 1)] << 24) | ((blues[1 - (k & 1)] & 0xff) << 16) |
                                        ((greens[1 - (k & 1)] & 0xff) << 8) | (reds[1 - (k & 1)] & 0xff));
                    vertex = NuPrimStreamBufferPointer->vertex_ptr;
                    vertex->u = tex_u[k];
                    vertex->v = tex_v[k];
                    neg_y = vertices[k].y * -1.0;
                    NuPrim2DAddXYZ(vertices[k].x, neg_y, vertices[k].z);
                }
                // NuPrim2DEnd.
                *NuPrimVertexCountPointer = NuPrimVertexCount;
                NuPrimVertexCount = 0;
                NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[--NuPrimCSPos]);
                vertices[1].x = vertices[0].x;
                vertices[1].y = vertices[0].y;
                vertices[1].z = vertices[0].z;
                vertices[3].x = vertices[2].x;
                vertices[3].y = vertices[2].y;
                vertices[3].z = vertices[2].z;
                alphas[0] = alphas[1];
                fraction = (f32)next_fraction;
                remaining -= NuLgtLaserData[i].segment_length;
            }
        }
        NuLgtSeed = saved_seed;
        NuLgtLaserOldCnt = NuLgtLaserCnt;
        NuLgtLaserCnt = 0;
        NuScratchRelease();
    }

    void NuLgtInitializeState(void) {
        i32 i;
        NuLgtStateUnknown = 0;
        NuLgtStateCapacity = 8;
        memset(NuLgtStateSlots, 0, sizeof(NuLgtStateSlots));
        for (i = 0; i < 8; ++i) {
            NuLgtStateSlots[i].previous = i - 1;
            NuLgtStateSlots[i].next = i + 1;
        }
        NuLgtStateSlots[7].next = -1;
        NuLgtStateActiveCount = 0;
        NuLgtArcLaserCnt = 0;
        NuLgtArcLaserOldCnt = 0;
        NuLgtStateFreeHead = -1;
        NuLgtStateEnabled = 1;
    }

    void NuLgtSetArcMat(NUMTL *material, f32 u0, f32 v0, f32 u1, f32 v1) {
        NuLgtArcMtl[0].material = material;
        NuLgtArcMtl[0].u0 = u0;
        NuLgtArcMtl[0].v0 = v0;
        NuLgtArcMtl[0].u1 = u1;
        NuLgtArcMtl[0].v1 = v1;
    }

    void NuLgtArcLaser(i32 type, NUVEC *start, NUVEC *end, NUVEC *bend, f32 width, f32 segment_length, f32 wobble, f32 bend_amount,
                       i32 colour) {
        NuLgtArcLaserEx(type, start, end, bend, width, segment_length, wobble, bend_amount, colour, 0);
    }

    void NuLgtArcLaserDraw(i32 paused) {
        i32 i;
        f32 tex_v[4];
        NULGTVERTEX *vertex;
        NUVEC4 *vertices;
        NUMTX *clip;
        NUVEC perpendicular;
        f32 length;
        f32 factor;
        f32 step;
        f32 u;
        f32 u1;
        f32 progress;
        f64 next;
        f64 wobble_limit;
        i32 angle;
        f32 *trig;
        f32 reciprocal;
        f32 jitter_x;
        f32 jitter_y;
        f32 offset_x;
        f32 offset_y;
        f32 corner_x;
        f32 corner_y;
        tex_v[0] = NuLgtArcMtl[0].v0;
        tex_v[1] = NuLgtArcMtl[0].v0;
        tex_v[2] = NuLgtArcMtl[0].v1;
        tex_v[3] = NuLgtArcMtl[0].v1;
        if (paused) {
            NuLgtArcLaserCnt = NuLgtArcLaserOldCnt;
        }
        if (!NuLgtArcMtl[0].material || NuLgtArcLaserCnt == 0) {
            NuLgtArcLaserOldCnt = 0;
            return;
        }

        vertices = (NUVEC4 *)NuScratchAlloc32(0xc0);

        clip = NuCameraGetVPCSMtx();
        if (!NuRndrBeginSceneEx(1, 2, 0)) {
            NuScratchRelease();
            return;
        }
        // NuPrimSetCoordinateSystem(0), pushed.
        NuPrimXScale = NuPrimPS2XScale;
        NuPrimYScale = NuPrimPS2YScale;
        NuPrimXBias = -1.0f;
        NuPrimYBias = 1.0f;
        NuPrimCoordSystemStack[NuPrimCSPos++] = 0;
        NuPrim2DBegin(12, 5, NuLgtArcMtl[0].material);
        for (i = 0; i < NuLgtArcLaserCnt; ++i) {
            vertices[4].x = NuLgtArcLaserData[i].start.x;
            vertices[4].y = NuLgtArcLaserData[i].start.y;
            vertices[4].z = NuLgtArcLaserData[i].start.z;
            vertices[4].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[4], &vertices[4], clip);
            vertices[5].x = NuLgtArcLaserData[i].end.x;
            vertices[5].y = NuLgtArcLaserData[i].end.y;
            vertices[5].z = NuLgtArcLaserData[i].end.z;
            vertices[5].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[5], &vertices[5], clip);
            if (!(vertices[4].w >= 0.5f) && !(vertices[5].w >= 0.5f)) {
                continue;
            }
            if (vertices[4].w < 0.5f) {
                vertices[6].x =
                    (NuLgtArcLaserData[i].start.x - NuLgtArcLaserData[i].end.x) * (vertices[5].w - 0.5) / (vertices[5].w - vertices[4].w) +
                    NuLgtArcLaserData[i].end.x;
                vertices[6].y =
                    (NuLgtArcLaserData[i].start.y - NuLgtArcLaserData[i].end.y) * (vertices[5].w - 0.5) / (vertices[5].w - vertices[4].w) +
                    NuLgtArcLaserData[i].end.y;
                vertices[6].z =
                    (NuLgtArcLaserData[i].start.z - NuLgtArcLaserData[i].end.z) * (vertices[5].w - 0.5) / (vertices[5].w - vertices[4].w) +
                    NuLgtArcLaserData[i].end.z;
            } else {
                vertices[6].x = NuLgtArcLaserData[i].start.x;
                vertices[6].y = NuLgtArcLaserData[i].start.y;
                vertices[6].z = NuLgtArcLaserData[i].start.z;
            }
            vertices[6].w = 1.0f;
            if (vertices[5].w < 0.5f) {
                vertices[7].x =
                    (NuLgtArcLaserData[i].end.x - NuLgtArcLaserData[i].start.x) * (vertices[4].w - 0.5) / (vertices[4].w - vertices[5].w) +
                    NuLgtArcLaserData[i].start.x;
                vertices[7].y =
                    (NuLgtArcLaserData[i].end.y - NuLgtArcLaserData[i].start.y) * (vertices[4].w - 0.5) / (vertices[4].w - vertices[5].w) +
                    NuLgtArcLaserData[i].start.y;
                vertices[7].z =
                    (NuLgtArcLaserData[i].end.z - NuLgtArcLaserData[i].start.z) * (vertices[4].w - 0.5) / (vertices[4].w - vertices[5].w) +
                    NuLgtArcLaserData[i].start.z;
            } else {
                vertices[7].x = NuLgtArcLaserData[i].end.x;
                vertices[7].y = NuLgtArcLaserData[i].end.y;
                vertices[7].z = NuLgtArcLaserData[i].end.z;
            }
            vertices[7].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[4], &vertices[6], clip);
            NuVec4ScaleXYZVU0(&vertices[4], &vertices[4], 1.0f / vertices[4].w);
            NuVec4MtxTransformVU0(&vertices[5], &vertices[7], clip);
            NuVec4ScaleXYZVU0(&vertices[5], &vertices[5], 1.0f / vertices[5].w);

            perpendicular.x = (vertices[5].y - vertices[4].y) * ((f32)PS2_REZ_W / 240.0f);
            perpendicular.y = vertices[4].x - vertices[5].x;
            perpendicular.z = 0.0f;
            NuVecNorm(&perpendicular, &perpendicular);
            perpendicular.x *= (((f32)PS2_REZ_W * 150.0f) / 240.0f) * NuLgtArcLaserData[i].width;
            perpendicular.y *= NuLgtArcLaserData[i].width * 150.0f;
            vertices[7].x -= vertices[6].x;
            vertices[7].y -= vertices[6].y;
            vertices[7].z -= vertices[6].z;
            vertices[7].w -= vertices[6].w;

            length = NuFsqrt(vertices[7].x * vertices[7].x + vertices[7].y * vertices[7].y + vertices[7].z * vertices[7].z);
            if (!(length > 0.0f)) {
                continue;
            }
            vertices[10].x = NuLgtArcLaserData[i].bend.x;
            vertices[10].y = NuLgtArcLaserData[i].bend.y;
            vertices[10].z = NuLgtArcLaserData[i].bend.z;

            factor = (vertices[10].y * vertices[10].y) + (vertices[10].x * vertices[10].x) + vertices[10].z * vertices[10].z;
            if (factor > 0.0f) {
                factor = (f64)NuLgtArcLaserData[i].bend_amount / NuFsqrt(factor);
            }
            vertices[10].x *= factor;
            vertices[10].y *= factor;
            vertices[10].z *= factor;
            NuLgtSeed = NuLgtArcLaserData[i].seed;

            step = NuLgtArcLaserData[i].segment_length / length;
            step = step > 1.0f ? 1.0f : step;

            u = NuLgtArcMtl[0].u0;
            u1 = NuLgtArcMtl[0].u1;
            vertices[4].x = vertices[6].x;
            vertices[4].y = vertices[6].y;
            vertices[4].z = vertices[6].z;
            vertices[4].w = 1.0f;
            NuVec4MtxTransformVU0(&vertices[4], &vertices[4], clip);
            NuVec4ScaleXYZVU0(&vertices[4], &vertices[4], 1.0f / vertices[4].w);
            vertices[1].x = vertices[4].x;
            vertices[1].y = vertices[4].y;
            vertices[1].z = vertices[4].z;
            vertices[3].x = vertices[4].x;
            vertices[3].y = vertices[4].y;
            vertices[3].z = vertices[4].z;

            progress = 0.0f;
            wobble_limit = 1.0 - 1.5 * step;
            while (progress < 1.0f) {
                next = progress + step;
                if (next >= 1.0f) {
                    vertices[5].x = vertices[6].x + vertices[7].x;
                    vertices[5].y = vertices[7].y + vertices[6].y;
                    vertices[5].z = vertices[6].z + vertices[7].z;
                    u = NuLgtArcMtl[0].u1 + (1.0f - progress) * (NuLgtArcMtl[0].u0 - NuLgtArcMtl[0].u1) / step;
                } else {
                    angle = ((i32)(next * 32768.0) >> 1) & 0x7fff;
                    trig = &NuTrigTable[angle];
                    vertices[5].x = vertices[7].x * next + vertices[6].x + vertices[10].x * *trig;
                    vertices[5].y = vertices[7].y * next + vertices[6].y + vertices[10].y * *trig;
                    vertices[5].z = vertices[7].z * next + vertices[6].z + *trig * vertices[10].z;
                }
                vertices[5].w = 1.0f;
                NuVec4MtxTransformVU0(&vertices[5], &vertices[5], clip);

                reciprocal = 1.0f / vertices[5].w;
                NuVec4ScaleXYZVU0(&vertices[5], &vertices[5], reciprocal);

                if (progress < wobble_limit && NuLgtSeed != 0) {
                    jitter_y = (f32)((NU_LGT_RAND() & 0xff) - 128);
                    jitter_x = jitter_y * perpendicular.x * NuLgtArcLaserData[i].wobble * reciprocal;
                    jitter_y = jitter_y * perpendicular.y * NuLgtArcLaserData[i].wobble * reciprocal;
                } else {
                    jitter_x = jitter_y = 0.0f;
                }
                offset_x = perpendicular.x * reciprocal;
                offset_y = perpendicular.y * reciprocal;
                corner_x = vertices[5].x - offset_x + jitter_x;
                vertices[0].x = corner_x;
                corner_y = vertices[5].y - offset_y + jitter_y;
                vertices[0].y = corner_y;
                vertices[0].z = vertices[5].z;
                vertices[2].x = vertices[5].x + offset_x + jitter_x;
                vertices[2].y = vertices[5].y + offset_y + jitter_y;
                vertices[2].z = vertices[5].z;
                if (!(corner_x < -50.0 && vertices[1].x < 50.0) && !(corner_x > 700.0f && vertices[1].x > 700.0f) &&
                    !(corner_y < -50.0 && vertices[1].y < -50.0) && !(corner_y > 530.0f && vertices[1].y > 530.0f)) {
                    NU_LGT_ARC_VERTEX(vertex, &vertices[0], NuLgtArcLaserData[i].colour, u, tex_v[0]);
                    NU_LGT_ARC_VERTEX(vertex, &vertices[1], NuLgtArcLaserData[i].colour, u1, tex_v[0]);
                    NU_LGT_ARC_VERTEX(vertex, &vertices[2], NuLgtArcLaserData[i].colour, u, tex_v[2]);
                    NU_LGT_ARC_VERTEX(vertex, &vertices[1], NuLgtArcLaserData[i].colour, u1, tex_v[0]);
                    NU_LGT_ARC_VERTEX(vertex, &vertices[3], NuLgtArcLaserData[i].colour, u1, tex_v[2]);
                    NU_LGT_ARC_VERTEX(vertex, &vertices[2], NuLgtArcLaserData[i].colour, u, tex_v[2]);
                }
                vertices[1].x = vertices[0].x;
                vertices[1].y = vertices[0].y;
                vertices[1].z = vertices[0].z;
                vertices[3].x = vertices[2].x;
                vertices[3].y = vertices[2].y;
                vertices[3].z = vertices[2].z;
                progress = (f32)next;
            }
        }
        // NuPrim2DEnd.
        *NuPrimVertexCountPointer = NuPrimVertexCount;
        NuPrimVertexCount = 0;
        NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[--NuPrimCSPos]);
        NuRndrEndSceneEx(0);
        NuLgtArcLaserOldCnt = NuLgtArcLaserCnt;
        NuLgtArcLaserCnt = 0;
        if (!paused) {
            ++NuLgtArcLaserFrame;
        }
        NuScratchRelease();
    }

    void NuGobjReleaseChild(PC_GOBJ_REGISTRY_CHILD *child) {
        PC_GOBJ_REGISTRY_AUX *aux = child->auxiliary;
        if (child->owned_buffer != NULL) {
            NuFree(child->owned_buffer, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nugobj.c", 605);
            child->owned_buffer = NULL;
        }
        while (aux != NULL) {
            PC_GOBJ_REGISTRY_AUX *next = aux->next;
            if (aux->payload != NULL) {
                aux->payload = NULL;
            }
            aux = next;
        }
        if (child->native_resource != NULL && child->native_resource->resource != NULL) {
            PCDeviceReleaseTexture(&child->native_resource->resource);
        }
    }

    FRAME_NATIVE_TEXTURE *FRAME_NATIVE_TEXTURE_POOL::Allocate() {
        i32 index;
        for (index = cursor; index < 16; ++index) {
            if (((occupied[index >> 3] >> (index & 7)) & 1) == 0) {
                occupied[index >> 3] |= (u8)(1u << (index & 7));
                cursor = (index + 1) % 16;
                return &records[index];
            }
        }
        for (index = 0; index < cursor; ++index) {
            if (((occupied[index >> 3] >> (index & 7)) & 1) == 0) {
                occupied[index >> 3] |= (u8)(1u << (index & 7));
                cursor = (index + 1) % 16;
                return &records[index];
            }
        }
        return NULL;
    }

    void PCGraphicsClearFrameTextures(void) {
        PC_EFFECT_SURFACE surface;
        for (i32 i = 0; i < 16; ++i) {
            FRAME_NATIVE_TEXTURE *record = &FrameNativeTexturePool.records[i];
            if (record->texture_id != 0 && record->render_texture != NULL) {
                record->dimensions.resource = record->render_texture->resource;
                surface.Attach(record->render_texture, 0, false);
                PCDeviceBackend.SetSurfaceRenderTarget(&surface, 0);
                PCDeviceBackend.ClearDepthStencilSurface();
                PCDeviceClear(0, NULL, 1, 0xff000000, 1.0f, 0);
            }
        }
    }

    void FrameNativeTextureRelease(void *, i32 texture_id) {
        FRAME_NATIVE_TEXTURE *texture = (FRAME_NATIVE_TEXTURE *)NuTextureGetDimensions(texture_id);
        if (texture != NULL) {
            i32 index;
            NuTexDestroyId(texture_id);
            index = texture - FrameNativeTexturePool.records;
            FrameNativeTexturePool.occupied[index >> 3] &= ~(1 << (index & 7));
            FrameNativeTexturePool.cursor = index;
        }
    }

    PC_FRAMEBUFFER::PC_FRAMEBUFFER() {
    }

    PC_FRAMEBUFFER::~PC_FRAMEBUFFER() {
    }

    void PCGraphicsInitializeFramebuffers(u32 flags, i32 unused, VARIPTR *buffer, VARIPTR end) {
        memset(PcFramebufferPool, 0, sizeof(PcFramebufferPool));
        memset(PcFramebufferActive, 0, sizeof(PcFramebufferActive));
        PcBoundFramebuffer = NULL;
        PcDefaultFramebuffer = NULL;
        PcSecondaryFramebuffer = NULL;
        bool multi = PCDeviceBackend.caps.simultaneous_render_targets > 1;
        bool second = multi && (flags & 2);
        bool third = multi && (flags & 4);
        PcDefaultFramebuffer = PCGraphicsCreateFramebuffer();
        PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 0, PCDeviceBackend.textures[0], 0);
        PcDefaultFramebuffer->clear_colour[0] = 0;
        PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 2, PCDeviceBackend.textures[2], 0);
        PcDefaultFramebuffer->clear_colour[2] = 0xffffffff;
        if (second && PCDeviceBackend.target_count > 1) {
            PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 1, PCDeviceBackend.textures[1], 0);
            PcDefaultFramebuffer->clear_colour[1] = 0x808080;
        }
        if (third && PCDeviceBackend.target_count > 2) {
            PCGraphicsAttachFramebuffer(PcDefaultFramebuffer, 2, PCDeviceBackend.textures[2], 0);
            PcDefaultFramebuffer->clear_colour[2] = 0x8080;
        }
        PcSecondaryFramebuffer = PCGraphicsCreateFramebuffer();
        PCGraphicsAttachFramebuffer(PcSecondaryFramebuffer, 0, PCDeviceBackend.textures[0], 0);
        PcSecondaryFramebuffer->clear_colour[0] = 0;
        PcBootDefaultFramebuffer = PcBootSecondaryFramebuffer = NULL;
        PcBootDefaultFramebuffer = PcDefaultFramebuffer;
        PcBootSecondaryFramebuffer = PcSecondaryFramebuffer;
        PCGraphicsBindFramebuffer(PcDefaultFramebuffer);
    }

    void BuildDebrisVerts(DEBRIS_PART_HEADER *header, DEBRIS_PARTICLE_CHUNK *chunk, NUMTL *material, f32 time, NUMTX *matrix,
                          i32 particle_type, f32 unused_a, f32 unused_b, f32 unused_c, f32 near_clip) {
        u32 emitted = 0;
        f32 u0;
        f32 v0;
        f32 u1;
        f32 v1;
        i32 i;
        if (material->material_kind == 0x97) {
            u0 = 0.0f;
            u1 = 1.0f;
            v1 = 1.0f;
            v0 = 0.0f;
        } else {
            u0 = header->texture_u0;
            u1 = header->texture_u1;
            v0 = header->texture_v0;
            v1 = header->texture_v1;
        }
        for (i = 0; i != 32; ++i) {
            DEBRISPARTICLE *particle = &chunk->particles[i];
            f32 age = time - particle->start_time;
            u32 frame_index = (u32)(age * particle->inverse_lifetime);
            if (frame_index < 63) {
                f32 fraction = age * particle->inverse_lifetime - (f32)frame_index;
                f32 distance;
                DEBRIS_PARTICLE_FRAME *first = &header->frames[frame_index];
                NUVEC position;
                NUVEC rotated;
                NUVEC corners[4];
                NUVEC *corner;
                i32 count;
                u32 colour;
                u32 index;
                position.x = particle->position.x + particle->momentum.x * age;
                position.y = (f32)(particle->position.y + particle->momentum.y * age + header->gravity * age * age * (f64)0.945f);
                position.z = particle->position.z + particle->momentum.z * age;
                rotated.x = position.x * matrix->m00 + position.y * matrix->m10 + position.z * matrix->m20;
                rotated.y = position.x * matrix->m01 + position.y * matrix->m11 + position.z * matrix->m21;
                rotated.z = position.x * matrix->m02 + position.y * matrix->m12 + position.z * matrix->m22;
                NuRndr_DebrisMtx.m30 = rotated.x + matrix->m30;
                NuRndr_DebrisMtx.m31 = rotated.y + matrix->m31;
                NuRndr_DebrisMtx.m32 = rotated.z + matrix->m32;
                if (particle_type == 6 || particle_type == 7) {
                    NuRndrParticleSetRepeat(&NuRndr_DebrisMtx.translation);
                }
                distance = NuRndr_DebrisPlane.y * NuRndr_DebrisMtx.m31 + NuRndr_DebrisPlane.x * NuRndr_DebrisMtx.m30 +
                           NuRndr_DebrisPlane.z * NuRndr_DebrisMtx.m32 + NuRndr_DebrisPlane.w;
                if (distance < near_clip) {
                    continue;
                }
                if (NuDebrisCurrentVertexCount + emitted + 6 > NuDebrisMaxVertexCount) {
                    VARIPTR *buffer;
                    NATIVE_PARTICLE_PACKET *packet;
                    NuDebrisCurrentPacket->vertex_count += (i32)emitted;
                    NuDebrisFrameVertexCount += emitted;
                    emitted = 0;
                    if (NuDebrisRendererNextBuffer() == 0) {
                        return;
                    }
                    NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15;
                    buffer = NuDisplayListStreamCursor;
                    packet = (NATIVE_PARTICLE_PACKET *)buffer->void_ptr;
                    packet->material = NuDebrisCurrentPacket->material;
                    packet->vertex_buffer_index = (u8)NuDebrisCurrentBufferIndex;
                    packet->use_system_memory_vb = NuDebrisUseSystemMemory;
                    packet->first_vertex = (i32)NuDebrisCurrentVertexCount;
                    packet->vertex_count = 0;
                    buffer->addr += sizeof(*packet);
                    NuDebrisCurrentPacket = packet;
                    AddParticleGroupToDisplayList(packet);
                }
                corners[0].x = first[1].position.x * fraction + first->position.x * (1.0f - fraction);
                corners[0].y = first[1].position.y * fraction + first->position.y * (1.0f - fraction);
                corners[0].z = first[1].position.z * fraction + first->position.z * (1.0f - fraction);
                corners[1].x = first[1].extent.x * fraction + first->extent.x * (1.0f - fraction);
                corners[1].y = first[1].extent.y * fraction + first->extent.y * (1.0f - fraction);
                corners[1].z = first[1].extent.z * fraction + first->extent.z * (1.0f - fraction);
                corners[2].x = first[1].texture_offset.x * fraction + first->texture_offset.x * (1.0f - fraction);
                corners[2].y = first[1].texture_offset.y * fraction + first->texture_offset.y * (1.0f - fraction);
                corners[2].z = first[1].texture_offset.z * fraction + first->texture_offset.z * (1.0f - fraction);
                corners[3].x = (corners[1].x - corners[2].x) + corners[0].x;
                corners[3].y = (corners[1].y - corners[2].y) + corners[0].y;
                corners[3].z = (corners[1].z - corners[2].z) + corners[0].z;
                corner = corners;
                for (count = 4; count != 0; --count) {
                    NuVecMtxTransform(corner, corner, &NuRndr_DebrisMtx);
                    ++corner;
                }
                colour = first->colour;
                index = NuDebrisCurrentVertexCount + emitted;
                NuDebrisVertexData[index].position = corners[0];
                NuDebrisVertexData[index].colour = colour;
                NuDebrisVertexData[index].u = u0;
                NuDebrisVertexData[index].v = v1;
                NuDebrisVertexData[index + 1].position = corners[2];
                NuDebrisVertexData[index + 1].colour = colour;
                NuDebrisVertexData[index + 1].u = u1;
                NuDebrisVertexData[index + 1].v = v1;
                NuDebrisVertexData[index + 2].position = corners[1];
                NuDebrisVertexData[index + 2].colour = colour;
                NuDebrisVertexData[index + 2].u = u1;
                NuDebrisVertexData[index + 2].v = v0;
                NuDebrisVertexData[index + 3].position = corners[0];
                NuDebrisVertexData[index + 3].colour = colour;
                NuDebrisVertexData[index + 3].u = u0;
                NuDebrisVertexData[index + 3].v = v1;
                NuDebrisVertexData[index + 4].position = corners[1];
                NuDebrisVertexData[index + 4].colour = colour;
                NuDebrisVertexData[index + 4].u = u1;
                NuDebrisVertexData[index + 4].v = v0;
                NuDebrisVertexData[index + 5].position = corners[3];
                NuDebrisVertexData[index + 5].colour = colour;
                NuDebrisVertexData[index + 5].u = u0;
                NuDebrisVertexData[index + 5].v = v0;
                emitted += 6;
            }
        }
        NuDebrisCurrentPacket->vertex_count += (i32)emitted;
        NuDebrisCurrentVertexCount += emitted;
        NuDebrisFrameVertexCount += emitted;
    }

    void NuGobjRegistryRelease(PC_GOBJ_REGISTRY_ENTRY *entry) {
        PC_GOBJ_REGISTRY_CHILD *child = entry->children;
        PC_GOBJ_REGISTRY_AUX *aux = entry->auxiliary;
        PC_GOBJ_REGISTRY_ENTRY *next;
        while (child != NULL) {
            PC_GOBJ_REGISTRY_CHILD *next_child = child->next;
            NuGobjReleaseChild(child);
            child = next_child;
        }
        while (aux != NULL) {
            aux = aux->next;
        }
        if (entry->previous != NULL) {
            entry->previous->next = entry->next;
        }
        next = entry->next;
        --NuGobjRegistryCount;
        if (next != NULL) {
            next->previous = entry->previous;
        }
    }

    i32 NuTexRegisterRenderTarget(GRAPHICS_RENDER_TEXTURE *target) {
        FRAME_NATIVE_TEXTURE *texture = FrameNativeTexturePool.Allocate();
        texture->render_texture = target;
        texture->texture_id = NuTexRegisterDimensions(&texture->dimensions);
        if (texture->texture_id == 0) {
            i32 index = (i32)((u32)texture - (u32)&FrameNativeTexturePool.records[0]) / 0x30;
            FrameNativeTexturePool.occupied[index >> 3] &= (u8) ~(1u << (index & 7));
            FrameNativeTexturePool.cursor = index;
            return 0;
        }
        texture->dimensions.width = target->width;
        texture->dimensions.height = target->height;
        texture->dimensions.ref_count = 1;
        memset(texture->dimensions.checksum, 0, sizeof(texture->dimensions.checksum));
        texture->dimensions.image_data = NULL;
        texture->dimensions.image_size = 0;
        texture->dimensions.resource = target->resource;
        return texture->texture_id;
    }

    void NuMainFilter::render() {
        if (bloom_enabled) {
            RenderBloom();
        }
    }

    void NuRndrParticleGroup(DEBRIS_PARTICLE_CHUNK *chunks, DEBRIS_PART_HEADER *header, NUMTL *material, f32 time, NUMTX *matrix,
                             i32 particle_type, f32 a, f32 b, f32 c, f32 near_clip) {
        DEBRIS_PARTICLE_CHUNK *chunk;
        DEBRIS_PARTICLE_CHUNK *data;
        i32 count;
        i32 done;
        i32 command;
        DEBRIS_PARTICLE_CHUNK *next;
        if (header == NULL) {
            NuDebrisLastHeader = NULL;
            return;
        }
        if (material == NULL || (material->material_kind == 0x97 && PCDeviceHardwareVertexProcessingPC < 2)) {
            return;
        }
        if (header != NuDebrisLastHeader) {
            NUCAMERA camera;
            VARIPTR *buffer;
            NATIVE_PARTICLE_PACKET *packet;
            if (material->attribs.lighting != NUMTL_LIGHTING_NONE || material->attribs.vertex_colour == 0) {
                material->attribs.lighting = NUMTL_LIGHTING_NONE;
                material->attribs.vertex_colour = 1;
                NuMtlUpdate(material);
            }
            if (NuRndr_DebrisRotMtxPtr == NULL) {
                NuMtxCalcDebrisFaceOn(&NuRndr_DebrisMtx);
            } else {
                NuRndr_DebrisMtx = *NuRndr_DebrisRotMtxPtr;
            }
            NuCameraGet(&camera);
            NuRndr_DebrisPlane.x = camera.mtx.m20;
            NuRndr_DebrisPlane.y = camera.mtx.m21;
            NuRndr_DebrisPlane.z = camera.mtx.m22;
            NuRndr_DebrisPlane.w = -(camera.mtx.m30 * camera.mtx.m20 + camera.mtx.m31 * camera.mtx.m21 + camera.mtx.m32 * camera.mtx.m22);
            header->last_render_time = time;
            NuDisplayListStreamCursor->addr = (NuDisplayListStreamCursor->addr + 15) & ~15;
            buffer = NuDisplayListStreamCursor;
            packet = (NATIVE_PARTICLE_PACKET *)buffer->void_ptr;
            buffer->addr = (u32)(packet + 1);
            packet->use_system_memory_vb = NuDebrisUseSystemMemory;
            packet->vertex_buffer_index = (u8)NuDebrisCurrentBufferIndex;
            packet->first_vertex = (i32)NuDebrisCurrentVertexCount;
            packet->material = material;
            packet->vertex_count = 0;
            NuDebrisCurrentPacket = packet;
            if (NuDebrisVertexData == NULL) {
                PCDeviceLockVertexBuffer(0, NuDebrisVertexBufferSize, (void **)&NuDebrisVertexData, 0x2000,
                                         NuDebrisVertexBuffers[NuDebrisCurrentBufferIndex]);
            }
            AddParticleGroupToDisplayList(NuDebrisCurrentPacket);
            NuDebrisLastHeader = header;
        }
        done = 0;
        count = 0;
        chunk = chunks;
        data = chunks;
        while (done == 0) {
            command = (i8)chunk->command;
            next = chunk->next;
            switch (command) {
                case 0x4e:
                    if (next != NULL) {
                        BuildDebrisVerts(header, data, material, time, matrix, particle_type, a, b, c, near_clip);
                        data = next;
                        chunk = next;
                    }
                    break;
                case 0x52:
                    BuildDebrisVerts(header, data, material, time, matrix, particle_type, a, b, c, near_clip);
                    done = 1;
                    break;
            }
            count++;
            if (count > 0x100) {
                break;
            }
        }
    }

    void NuGobjRegistryCleanup(void) {
        if (NuGobjRegistryInitialized != 0) {
            PC_GOBJ_REGISTRY_ENTRY *entry = NuGobjRegistryFirst;
            while (entry != NULL) {
                PC_GOBJ_REGISTRY_ENTRY *next = entry->next;
                PC_GOBJ_REGISTRY_CHILD *child = entry->children;
                PC_GOBJ_REGISTRY_AUX *aux = entry->auxiliary;
                while (child != NULL) {
                    PC_GOBJ_REGISTRY_CHILD *next_child = child->next;
                    NuGobjReleaseChild(child);
                    child = next_child;
                }
                while (aux != NULL) {
                    aux = aux->next;
                }
                if (entry->previous != NULL) {
                    entry->previous->next = entry->next;
                }
                if (entry->next != NULL) {
                    entry->next->previous = entry->previous;
                }
                --NuGobjRegistryCount;
                entry = next;
                NuGobjRegistryFirst = entry;
            }
        }
    }

    void NuGobjRegistryInit(void) {
        if (NuGobjRegistryInitialized != 0) {
            NuGobjRegistryCleanup();
        }
        NuGobjRegistryFirst = NULL;
        NuGobjRegistryInitialized = 1;
    }

    void NuMainFilter::initResources() {
        NuMainFilterGen::initResources();
        POST_SHADER_PROGRAM *last_program;
        if (PCDeviceHardwareVertexProcessingPC > 1) {
            programs[6] = PostShaderProgramCreate(2, dat_007662d8, dat_00766578);
            programs[8] = PostShaderProgramCreate(2, dat_00766820, dat_00766578);
            platform_programs[0] = PostShaderProgramCreate(2, dat_007662d8, dat_007666f0);
            last_program = PostShaderProgramCreate(2, dat_00766820, dat_007666f0);
            platform_uniforms[0] = 0x82;
            platform_uniforms[1] = 0x88;
            platform_uniforms[2] = 0x89;
            platform_uniforms[3] = 0x8a;
        } else {
            programs[6] = PostShaderProgramCreate(2, dat_00766c38, dat_00766ed8);
            programs[8] = PostShaderProgramCreate(2, dat_00767180, dat_00766ed8);
            platform_programs[0] = PostShaderProgramCreate(2, dat_00766c38, dat_00767050);
            last_program = PostShaderProgramCreate(2, dat_00767180, dat_00767050);
            platform_uniforms[0] = 0;
            platform_uniforms[1] = 1;
            platform_uniforms[2] = 2;
            platform_uniforms[3] = 3;
        }
        platform_programs[1] = last_program;
        platform_fbo = PCGraphicsCreateFramebuffer();
    }

    void NuDeferredFilter::initResources() {
        NuDeferredFilterGen::initResources();
        programs[0] = PostShaderProgramCreate(2, dat_00767598, dat_007676f0);
        programs[1] = PostShaderProgramCreate(2, dat_00767598, dat_00767fc8);
        programs[2] = PostShaderProgramCreate(2, dat_00768d20, dat_00768870);
        programs[3] = PostShaderProgramCreate(2, dat_00768d20, dat_00768ae0);
    }

    void NuSpeedBlurFilter::initResources() {
        NuPostFilterGen::initResources();
        POST_SHADER_PROGRAM *program;
        if (PCDeviceHardwareVertexProcessingPC > 1) {
            program = PostShaderProgramCreate(2, dat_00768de0, dat_007690b0);
            platform_uniform = 0x80;
        } else {
            program = PostShaderProgramCreate(2, dat_007692c0, dat_00769580);
            platform_uniform = 0;
        }
        programs[0] = program;
    }

    void NuMtlInitExPS(VARIPTR *) {
        Nu360MtlInit();
    }

    void fun_0060dee0(void) {
    }

    // @@END

} // extern "C"

/* nuhgobj's platform members: the POI lookup and the hierarchy shadow-group packets. */

extern "C" {
    NUHGOBJ_POI *NuHGobjGetPOI(NUHGOBJ_EVAL *object, u8 index) {
        if (index < object->point_index_count) {
            index = object->point_index_map[index];
            if (index != 0xff) {
                return &object->points[index];
            }
        }
        return NULL;
    }

    i32 fun_00604c90(void) {
        return 16;
    }

    void NuPrimPopCoordinateSystem2(void) {
        NuPrimSetCoordinateSystem(NuPrimCoordSystemStack[--NuPrimCSPos]);
    }

    void PCEnqueueHierarchyShadowGroup(NUMTX *matrix, void *ellipse, i32 ellipse_count, void *cylinder, i32 cylinder_count, void *third,
                                       i32 third_count) {
        if (PCShadowPacketCount >= 280) {
            return;
        }
        PCShadowPackets[PCShadowPacketCount].near_value = PCShadowNearValue;
        PCShadowPackets[PCShadowPacketCount].far_value = PCShadowFarValue;
        PCShadowPackets[PCShadowPacketCount].state0 = PCShadowState.state0;
        PCShadowPackets[PCShadowPacketCount].state1 = PCShadowState.state1;
        PCShadowPackets[PCShadowPacketCount].state2 = PCShadowState.state2;
        PCShadowPackets[PCShadowPacketCount].state_byte = PCShadowState.state_byte;
        PCShadowPackets[PCShadowPacketCount].matrix = *matrix;
        PCShadowPackets[PCShadowPacketCount].ellipse = ellipse;
        PCShadowPackets[PCShadowPacketCount].ellipse_count = (u8)ellipse_count;
        PCShadowPackets[PCShadowPacketCount].cylinder = cylinder;
        PCShadowPackets[PCShadowPacketCount].cylinder_count = (u8)cylinder_count;
        PCShadowPackets[PCShadowPacketCount].third_shape = third;
        PCShadowPackets[PCShadowPacketCount].third_count = (u8)third_count;
        PCShadowPackets[PCShadowPacketCount].sequence = (u8)PCShadowPacketSequence;
        ++PCShadowPacketCount;
        ++PCShadowPacketSequence;
    }

    void NuHGobjSubmitShadowGroups(NUHGOBJ_EVAL *hierarchy, NUMTX *world, NUMTX *joints) {
        PC_HG_SHADOW_GROUP *group;
        NUHGOBJ_EVAL *object;

        object = hierarchy;
        if (PcWindowOptions.shadowsenabled == 0 || PcWindowOptions.shadowmethod != 1 || object->shadow_groups == NULL ||
            object->suppress_shadow_points != 0) {
            return;
        }
        for (group = object->shadow_groups; group->joint_index != 255; ++group) {
            if (PCShadowPacketCount < 280 && PCShadowSubmissionEnabled != 0) {
                NUMTX combined;

                NuMtxMulVU0(&combined, &joints[group->joint_index], world);
                PCEnqueueHierarchyShadowGroup(&combined, group->ellipses, group->ellipse_count, group->cylinders, group->cylinder_count,
                                              group->third_shape, group->third_count);
                currentScene.clear_flags = 1;
            }
        }
    }
} // extern "C"

/* nupostdeferred's platform members: the empty framebuffer resolve and the shadow display-list handlers. */
extern "C" void PCGraphicsResolveFramebuffer(i32) {
}

GRAPHICS_RENDER_TEXTURE *PC_EFFECT_SURFACE::GetTexture() {
    return texture;
}

u16 PC_EFFECT_SURFACE::GetLevel() {
    return level;
}

u32 PC_EFFECT_SURFACE::GetFace() {
    return face;
}

u16 PC_EFFECT_SURFACE::GetWidth() {
    return width;
}

u16 PC_EFFECT_SURFACE::GetHeight() {
    return height;
}

extern "C" void NuDisplayListShadowTransform(void *data) {
    NUMTX *matrix = (NUMTX *)data;
    f32 saved33 = matrix->m33;
    f32 saved23 = matrix->m23;
    matrix->m33 = 1.0f;
    matrix->m23 = 0.0f;
    NuShaderManagerSetfv(SHADER_UNIFORM_WORLD, &matrix->m00);
    matrix->m33 = saved33;
    matrix->m23 = saved23;
}

extern "C" void NuDisplayListShadowTransformTranspose(void *data) {
    NUMTX *matrix = (NUMTX *)data;
    f32 saved33 = matrix->m33;
    f32 saved32 = matrix->m32;
    matrix->m33 = 1.0f;
    matrix->m32 = 0.0f;
    NuShaderManagerSetElementsfv_transpose(SHADER_UNIFORM_WORLD, 0, 1, &matrix->m00);
    matrix->m33 = saved33;
    matrix->m32 = saved32;
}

extern "C" void NuDisplayListShadowMaterial(void *data) {
    NUMTL *material = (NUMTL *)data;
    NuShaderObject *shader = (NuShaderObject *)PCShaderRecordFromHandle((u32)(i32)material->shader.shader_handles[1]);
    if (shader != NULL) {
        NuDisplayHandlerMaterial = material;
        if (!(material->shader.vertex_format & 0x300000) && material->display_list != NULL) {
            PCShaderSetMaterialParameters(shader, material);
            NuShaderManagerBindShader(shader);
            PCDeviceSetVertexDeclaration(material->vertex_declaration);
        }
        u32 flags = material->attribs.word;
        u32 z_mode = flags & 0xc000;
        if (z_mode == 0x4000) {
            material->attribs.word = (flags & 0xffff7fff) | z_mode;
        }
        NuMtlSetRenderStatesPS(material);
        PCDeviceSetRenderState(D3DRS_ALPHABLENDENABLE, 0);
    }
}

/* nurndr's platform members, in nurndr order. */
extern "C" {

    void NuRndrGradClear(i32 clear_flags, i32 top_colour, i32 bottom_colour, f32 alpha) {
        NuRndrClear(clear_flags, bottom_colour, alpha);
    }

    void fun_005fead0(i32 value) {
        dat_02854c94 = value;
    }

    void fun_005feae0(i32 value) {
        dat_02854c90 = value;
    }

    void fun_005fec80(void) {
    }

    i32 fun_005fec90(void) {
        PCGraphicsRenderTextureAllocationError();
        return 0;
    }

    void fun_005feca0(void) {
        PCGraphicsRenderTextureAllocationError();
    }

    void NuDisplayListResetStateCache(NURNDRLIBSTATE *state) {
        state->mtl = NULL;
        state->tex_id = -1;
        state->global_id = -1;
        state->lights_id = -1;
        state->camera_id = -1;
        state->fog_id = -1;
        state->konst_id = -1;
    }

    void RndrStateUpdate(NURNDRLIBSTATE *state, NUMTL *material, NUDISPLAYLISTITEM *item) {
    }

    void RndrStateUpdateFx(NURNDRLIBSTATE *state, NUDISPLAYLISTITEM *item) {
    }

    void NuDisplayListInit(VARIPTR *cursor, i32 arg2) {
        NuRndrStateInit();
        nudlist_field_9d8 = 0;
        nudlist_field_e80 = 0;
        sort_list = NULL;
        PCMaterialAnimSets = NULL;
        PCDisplayList2d.first = &nudlist_2d_static;
        {
            VARIPTR end;
            end.addr = (u32)arg2;
            NuDisplayListCreateDynMtlList_candidate(cursor, end);
        }
        nudlist_flags |= 0x10;
        nudlist_field_e58 = 0;
        NuDisplayListStreamAlloc(cursor, arg2, 0x40);
        NuDisplayListStreamCursor = &nudlist_stream_cursor;
        nudlist_field_1598 = nuprim_field_ba8;
        nudlist_field_fc4 = PCThreadCriticalSectionCreate_candidate();
        NuDisplayListSwapBuffersEndFrame();
        NuDisplayListBeginFrame();
    }

    u8 DwaBlockTable[0x400] = {0};

    void NuRndrDwaInit(void) {
        DwaWeightCapacity = 0x4000;
        DwaPointerCapacity = 0x4000;
        DwaBlockCapacity = 0x2000;
        DwaEnabled = 1;
        DwaBlockCount = 0;
        rndr_blend_shape_deformer_wt_cnt = 0xef00;
        rndr_blend_shape_deformer_wt_ptrs_cnt = 0x800;
        memset(DwaBlockTable, 0, sizeof(DwaBlockTable));
        DwaStreamCapacity = 0x2000;
    }

    /* Empty hook; PcRendererCleanup calls it while the D3D device is torn down. */
    void NuRndrDwaRelease(void) {
    }

    i32 fun_005fe460(void) {
        return dat_0097391c;
    }

    void PCFogSetColour(u32 colour) {
        PCFogColour = colour & 0xffffff;
        PCDeviceSetRenderState(D3DRS_FOGCOLOR, colour);
        PCFogColourSuppressed = 0;
    }

    void PCFogSetDensity(u32 density) {
        PCDeviceSetRenderState(D3DRS_FOGDENSITY, density);
    }

    void PCFogSuppressColour(void) {
        if (PCFogColourSuppressed == 0) {
            PCDeviceSetRenderState(D3DRS_FOGCOLOR, 0);
        }
        PCFogColourSuppressed = 1;
    }

    void PCFogRestoreColour(void) {
        if (PCFogColourSuppressed != 0) {
            PCDeviceSetRenderState(D3DRS_FOGCOLOR, PCFogColour);
            PCFogColourSuppressed = 0;
        }
    }

    void PCFogSetRange(u32 start, u32 end) {
        PCDeviceSetRenderState(D3DRS_FOGSTART, start);
        PCDeviceSetRenderState(D3DRS_FOGEND, end);
    }

    void PCFogSetDensity2(u32 density) {
        PCDeviceSetRenderState(D3DRS_FOGDENSITY, density);
    }

    i32 PCFogSetEnabled(i32 enable) {
        i32 previous = PCFogEnabled;
        PCFogEnabled = enable != 0;
        PCDeviceSetRenderState(D3DRS_FOGENABLE, PCFogEnabled);
        return previous;
    }

    i32 fun_005fe560(void) {
        PCGraphicsRenderTextureAllocationError();
        return 0;
    }

    void fun_005fe570(void) {
    }

    void fun_005fe580(void) {
    }

    void fun_005fe590(void) {
    }

    void NuRndrConfigureLeaf(f32 a, f32 b, i32 c) {
    }

    void fun_00602940(void) {
    }

    void fun_00602950(void) {
    }

    void fun_00602960(void) {
    }

    void fun_00602970(void) {
    }

    PC_GOBJ_REGISTRY_ENTRY *NuGobjRegistryAlloc(void) {
        PC_GOBJ_REGISTRY_ENTRY *entry =
            (PC_GOBJ_REGISTRY_ENTRY *)NuAlloc(0x70, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nugobj.c", 107);
        PC_GOBJ_REGISTRY_ENTRY *first;
        memset(entry, 0, 0x70);
        first = NuGobjRegistryFirst;
        if (first != NULL) {
            first->previous = entry;
        }
        ++NuGobjRegistryCount;
        entry->next = first;
        entry->previous = NULL;
        NuGobjRegistryFirst = entry;
        return entry;
    }

    void fun_006029d0(i32 value) {
        dat_02854f54 = value;
    }

    void *NuGobjAllocBlock(void) {
        void *block = NuAlloc(0x98, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nugobj.c", 172);
        memset(block, 0, 0x98);
        return block;
    }

    void *NuGobjAllocBlock2(void) {
        void *block = NuAlloc(0x98, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nugobj.c", 368);
        memset(block, 0, 0x98);
        return block;
    }

    void fun_00602de0(void) {
    }

    void NuGobjFreeChildBuffer(PC_GOBJ_REGISTRY_CHILD *child) {
        if (child->owned_buffer != NULL) {
            NuFree(child->owned_buffer, "d:\\projects\\game_saga\\nu2api.saga\\nu3d\\pc\\nugobj.c", 605);
            child->owned_buffer = NULL;
        }
    }

    void *PCGobjRegistryAuxPayload(PC_GOBJ_REGISTRY_AUX *aux) {
        return aux->payload;
    }

    void fun_00602e80(void) {
        PCGraphicsRenderTextureAllocationError();
    }

    void fun_00602e90(void) {
    }

    void fun_00602f30(PC_GOBJ_REGISTRY_AUX *aux) {
        if (aux != NULL && aux->payload != NULL) {
            aux->payload = NULL;
        }
    }

    void fun_006030d0(NUGOBJ *gobj) {
        for (NUGEOM *geom = gobj->geom; geom != NULL; geom = geom->next) {
        }
    }

    void NuGobjSetMaterialObjectFlags(NUGOBJ *gobj, i32 first, i32 second) {
        for (NUGEOM *geom = gobj->geom; geom != NULL; geom = geom->next) {
            NUMTL *mtl = geom->mtl;
            mtl->object_flag_a = first;
            mtl->object_flag_b = second;
            NuMtlUpdate(mtl);
        }
    }

    void PCSetGammaRamp(f32 gamma) {
        u16 ramp[3][256];
        for (i32 i = 0; i < 256; ++i) {
            i32 value = i * gamma * 255.0;
            i32 entry = 0;
            if (value > 0x7fff) {
                value -= 0x7fff;
                entry = 1;
                if (value > 0x7fff) {
                    value = 0x7fff;
                }
            }
            entry = entry * -0x8000 | value;
            ramp[0][i] = entry;
            ramp[1][i] = entry;
            ramp[2][i] = entry;
        }
        PCDeviceSetGammaRamp(ramp);
    }

    void fun_006031f0(void) {
    }

    void PCApplyGammaTable(u8 *colour, const u8 *table, i32 offset) {
        i32 r;
        i32 g;
        i32 b;
        if (colour[0] == 0 && colour[1] == 0 && colour[2] == 0) {
            r = g = b = table[0];
        } else {
            r = table[colour[0]] + offset;
            g = table[colour[1]] + offset;
            b = table[colour[2]] + offset;
        }
        if (r > 255) {
            r = 255;
        }
        if (g > 255) {
            g = 255;
        }
        if (b > 255) {
            b = 255;
        }
        if (r < 0) {
            r = 0;
        }
        if (g < 0) {
            g = 0;
        }
        if (b < 0) {
            b = 0;
        }
        colour[0] = r;
        colour[1] = g;
        colour[2] = b;
    }

    void PCBuildGammaTable(f32 exponent, f32 scale, u8 *table) {
        f32 factor = scale * 255.0f / NuPowFast(255.0f, exponent);
        for (i32 i = 0; i < 256; i++) {
            f32 value = NuPowFast(i, exponent) * factor;
            if (value > 255.0f) {
                value = 255.0f;
            }
            table[i] = value;
        }
    }

    void PermanentGraphicsResourceSetup(void) {
        NuPtlDmaSlotsExhausted();
    }

    void fun_005f9970(void) {
        NuPtlDmaSlotsExhausted();
    }

    void NuRndrShadowInit(u8 *random) {
        NuPtlDmaSlotsExhausted();
    }

    void fun_005fef00(void) {
        PCGraphicsRenderTextureAllocationError();
    }

    void fun_005fef10(void) {
    }

    void fun_005fef20(void) {
    }

    void fun_005fef30(void) {
    }

    void fun_005fef40(void) {
    }

    void fun_005fef50(void) {
    }

    void NuLightSpotFadeSet(u32 fade) {
    }

    void fun_005fef70(void) {
    }

    void fun_005fef80(void) {
    }

    void fun_005fef90(void) {
    }

    i32 fun_005fefa0(void) {
        return 1;
    }

    void NuRndrRectHook(i32, i32 x, i32 y, i32 width, i32 height, f32 u0, f32 v0, f32 u1, f32 v1, i32 colour, i32 z) {
        NuRndrRectUV2diZ(x, y, width, height, u0, v0, u1, v1, colour, NuRndrRectMaterial, z);
    }

    i32 NuRndrRectSetMaterial(i32, NUMTL *material) {
        NuRndrRectMaterial = material;
        return 0;
    }

    void fun_005ff010(void) {
    }

    void fun_005ff020(void) {
    }

    i32 fun_005ff030(void) {
        return 1;
    }

    DWA_WEIGHT_BLOCK *NuRndrCreateBlendShapeDeformerWeightsArray(i32 count) {
        i32 clear_bytes = count * 4 + 0x204;
        DWA_WEIGHT_BLOCK *result;
        rndr_blend_shape_deformer_wt_cnt -= clear_bytes;
        if (rndr_blend_shape_deformer_wt_cnt < 0) {
            PCClipScreenUnsupported();
            return NULL;
        }
        result = &DwaWeightPool[rndr_blend_shape_deformer_wt_cnt];
        memset(result, 0, clear_bytes);
        return result;
    }

    DWA_WEIGHT_BLOCK **NuRndrCreateBlendShapeDWAPointers(i32 count) {
        rndr_blend_shape_deformer_wt_ptrs_cnt -= count;
        if (rndr_blend_shape_deformer_wt_cnt < 0) {
            PCClipScreenUnsupported();
            return NULL;
        }
        return &DwaPointerPool[rndr_blend_shape_deformer_wt_ptrs_cnt];
    }

    void NuRndrShadowDirCol(const NUVEC *direction, u32 colour, f32 near_distance, f32 far_distance) {
        PCShadowState.direction = *direction;
        PCShadowNearValue = near_distance;
        PCShadowFarValue = far_distance;
        PCShadowState.packed_colour = ((colour >> 1) & 0x007f7f7fu) + (colour & 0xff000000u) * 2u;
    }

    void NuRndrAddShadow(NUVEC *position, f32 radius, i32 opacity, i32 unused_x, i32 unused_y, i32 unused_z) {
        if (NuCameraClipTestPoints(position, 1, NULL) == 0 && NuRndrShadowCnt < 128) {
            NuRndrShadPolDat[NuRndrShadowCnt].position = *position;
            NuRndrShadPolDat[NuRndrShadowCnt].radius = radius;
            NuRndrShadPolDat[NuRndrShadowCnt].opacity = (u16)opacity;
            ++NuRndrShadowCnt;
        }
    }

    /* The debug 3D line batch is not drawn on PC. */
    void NuRndrLine3dDbgFlush(void) {
    }

    void fun_005fecc0(void) {
        PCGraphicsRenderTextureAllocationError();
    }

    void fun_005fecd0(void) {
        PCGraphicsRenderTextureAllocationError();
    }

    i32 NuHGobjRndrMtxSimple(NUHGOBJ_EVAL *eval, NUMTX *world, i32 count, i16 *indices, NUMTX *matrices) {
        return NuHGobjRndrMtxDwa(eval, world, count, indices, matrices, NULL, 0);
    }

    void NuRndrShadPolys(NUMTL *material) {
        i32 index;
        PC_SHADOW_QUEUE_RECORD *record;
        NUVEC position;
        f32 radius;
        f32 left;
        f32 right;
        f32 front;
        f32 back;
        u32 colour;
        if (NuRndrShadowCnt == 0) {
            return;
        }
        record = NuRndrShadPolDat;
        for (index = 0; index < NuRndrShadowCnt; ++index, ++record) {
            position = record->position;
            colour = (u32)(i16)record->opacity << 24;
            radius = record->radius;
            NuRndrShadowVertices[0].colour = colour;
            NuRndrShadowVertices[1].colour = colour;
            NuRndrShadowVertices[2].colour = colour;
            NuRndrShadowVertices[3].colour = colour;
            left = position.x - radius;
            NuRndrShadowVertices[0].position.x = left;
            NuRndrShadowVertices[0].position.y = position.y;
            front = position.z - radius;
            NuRndrShadowVertices[0].position.z = front;
            NuRndrShadowVertices[0].u = 0.0f;
            NuRndrShadowVertices[0].v = 0.0f;
            right = position.x + radius;
            NuRndrShadowVertices[1].position.x = right;
            NuRndrShadowVertices[1].position.y = position.y;
            NuRndrShadowVertices[1].position.z = front;
            NuRndrShadowVertices[1].u = 1.0f;
            NuRndrShadowVertices[1].v = 0.0f;
            NuRndrShadowVertices[2].position.x = left;
            NuRndrShadowVertices[2].position.y = position.y;
            back = position.z + radius;
            NuRndrShadowVertices[2].position.z = back;
            NuRndrShadowVertices[2].u = 0.0f;
            NuRndrShadowVertices[2].v = 1.0f;
            NuRndrShadowVertices[3].position.x = right;
            NuRndrShadowVertices[3].position.y = position.y;
            NuRndrShadowVertices[3].position.z = back;
            NuRndrShadowVertices[3].u = 1.0f;
            NuRndrShadowVertices[3].v = 1.0f;
            NuRndrStrip3d(NuRndrShadowVertices, material, NULL, 4);
        }
        NuRndrShadowCnt = 0;
    }

    void NuRndrInitWorld(void) {
    }

    void fun_005fe620(void) {
    }

    void fun_005fe630(void) {
    }

    typedef struct RENDERER_CLEAR_RECORD { // size 0x28
        NUVEC position;                    // 0x00
        f32 start_radius;                  // 0x0c
        f32 end_radius;                    // 0x10
        f32 radius;                        // 0x14
        u32 colour;                        // 0x18
        u32 current_colour;                // 0x1c
        i16 active;                        // 0x20
        i16 duration;                      // 0x22
        u16 extra_bytes;                   // 0x24
        u8 unknown_26[2];                  // 0x26
    } RENDERER_CLEAR_RECORD;

    static RENDERER_CLEAR_RECORD RendererClearRecords[32] = {0};
    NULGTARCLASER NuLgtArcLaserData[16] = {0};
    NUGLOBALRNDRSTATE render_state = {0};

    void RendererClearAdd(NUVEC *position, f32 start_radius, f32 end_radius, i32 duration, u32 colour, i32 extra) {
        for (i32 i = 0; i < 32; i++) {
            if (RendererClearRecords[i].active == 0) {
                position->y += 0.01f;
                RendererClearRecords[i].position = *position;
                RendererClearRecords[i].start_radius = start_radius;
                RendererClearRecords[i].radius = start_radius;
                RendererClearRecords[i].end_radius = end_radius;
                RendererClearRecords[i].active = duration;
                RendererClearRecords[i].duration = duration;
                RendererClearRecords[i].colour = colour;
                RendererClearRecords[i].current_colour = 0;
                RendererClearRecords[i].extra_bytes = extra * 4;
                return;
            }
        }
    }

    void fun_005fe6f0(void) {
    }

    void RendererClearUpdate(i32 frames) {
        for (i32 i = 0; i < 32; i++) {
            RENDERER_CLEAR_RECORD *record = &RendererClearRecords[i];
            if (record->active != 0) {
                record->active -= frames;
                if (record->active <= 0) {
                    record->active = 0;
                } else {
                    f32 t = (f32)record->active / record->duration;
                    f32 fade;
                    record->radius = (record->start_radius - record->end_radius) * t + record->end_radius;
                    if (t > 0.75f) {
                        fade = (1.0f - t) * 4.0f;
                    } else {
                        fade = t * 1.3333334f;
                    }
                    record->current_colour = ((u32)((record->colour >> 24) * fade) << 24) + (record->colour & 0xffffff);
                }
            }
        }
    }

    void fun_005fe800(void) {
    }

    void fun_005fe850(i32 value) {
        dat_0080e21c = value;
    }

    void NuRndrFxApplyHeight(i32 restore) {
        if (NuRndrFxHeightOverride != 0.0f) {
            NuRndrFxHeight = NuRndrFxHeightOverride;
            NuRndrFxColour = 0xc04080c0;
        }
        if (restore) {
            NuRndrFxHeight = NuRndrFxPreviousHeight;
        } else {
            NuRndrFxPreviousHeight = NuRndrFxHeight;
        }
    }

    /* Direct object rendering is not supported on PC: it is reported and nothing is drawn. */
    i32 NuRndrGobj(void *object, NUMTX *matrix) {
        PCClipScreenUnsupported();
        return 0;
    }

    void fun_005fe980(void) {
    }

    void fun_005fe990(void) {
    }

    void fun_005fe9a0(void) {
        NuPtlDmaSlotsExhausted();
    }

    i32 fun_005fe9b0(void) {
        return 1;
    }

    i32 fun_005fe9c0(void) {
        return 0;
    }

    i32 NuRndrSetViewMtx(NUMTX *view_projection, NUMTX *viewport_clip, NUMTX *scissor_clip) {
        return 0;
    }

    void FaceYDirStream(i32 angle) {
    }

    i32 NuRndrSetSpecularLightPS(const NUVEC *direction, const NUVEC4 *intensity) {
        static NUVEC rndrstream_specular_dir = {0.0f, 0.0f, 1.0f};
        static NUVEC4 rndrstream_specular_intensity = {1.0f, 1.0f, 1.0f, 1.0f};
        static NUVEC camvec = {0.0f, 0.0f, -1.0f};
        NUVEC view_direction;
        NUVEC half_direction;
        NUVEC light_direction;
        NUVEC eye_direction;
        NUVEC axis;
        NUMTX matrix;
        f32 horizontal;
        f32 facing;
        f32 weight;
        f32 remainder;
        if (direction != NULL) {
            rndrstream_specular_dir = *direction;
        }
        if (intensity != NULL) {
            rndrstream_specular_intensity = *intensity;
        }
        NuVecInvMtxRotate(&view_direction, &rndrstream_specular_dir, &global_camera.mtx);
        NuVecLerp(&half_direction, &view_direction, &camvec, 0.5f);
        NuVecMtxRotate(&half_direction, &half_direction, &global_camera.mtx);
        NuVecNorm(&half_direction, &half_direction);
        NuVecNorm(&light_direction, &rndrstream_specular_dir);
        eye_direction.x = -global_camera.mtx.m20;
        eye_direction.y = -global_camera.mtx.m21;
        eye_direction.z = -global_camera.mtx.m22;
        facing = NuVecDot(&eye_direction, &light_direction);
        axis = half_direction;
        matrix = NuMtxSetIdentityTemplate;
        horizontal = NuFsqrt(axis.x * axis.x + axis.z * axis.z);
        if (horizontal > FLT_MIN) {
            matrix.m00 = axis.z / horizontal;
            matrix.m10 = (axis.x * axis.y) / horizontal;
            matrix.m20 = -axis.x;
            matrix.m01 = 0.0f;
            matrix.m11 = horizontal;
            matrix.m21 = axis.y;
            matrix.m02 = axis.x / horizontal;
            matrix.m12 = (axis.z * -axis.y) / horizontal;
            matrix.m22 = axis.z;
        } else {
            matrix.m11 = matrix.m22 = 0.0f;
            matrix.m21 = axis.y;
            matrix.m12 = -axis.y;
        }
        matrix.m31 = matrix.m30 = 0.5f;
        if (facing < 0.0f) {
            weight = facing * facing;
            weight *= weight;
            weight *= weight;
            weight *= weight;
            weight *= weight;
            remainder = 1.0f - weight;
            if (matrix.m00 * light_direction.x + matrix.m10 * light_direction.y + matrix.m20 * light_direction.z >= 0.0f) {
                matrix.m00 = matrix.m00 * remainder + light_direction.x * weight;
                matrix.m10 = matrix.m10 * remainder + light_direction.y * weight;
                matrix.m20 = matrix.m20 * remainder + light_direction.z * weight;
            } else {
                matrix.m00 = matrix.m00 * remainder - light_direction.x * weight;
                matrix.m10 = matrix.m10 * remainder - light_direction.y * weight;
                matrix.m20 = matrix.m20 * remainder - light_direction.z * weight;
            }
            if (matrix.m01 * light_direction.x + matrix.m11 * light_direction.y + matrix.m21 * light_direction.z >= 0.0f) {
                matrix.m01 = matrix.m01 * remainder + light_direction.x * weight;
                matrix.m11 = matrix.m11 * remainder + light_direction.y * weight;
                matrix.m21 = matrix.m21 * remainder + light_direction.z * weight;
            } else {
                matrix.m01 = matrix.m01 * remainder - light_direction.x * weight;
                matrix.m11 = matrix.m11 * remainder - light_direction.y * weight;
                matrix.m21 = matrix.m21 * remainder - light_direction.z * weight;
            }
        }
        NuVecNeg(&view_direction, &view_direction);
        NuRndrStateSetSpecularLightEx(&view_direction, &matrix, &rndrstream_specular_intensity.xyz);
        return 1;
    }

    static f32 FrameShaderMaterialAnimationTime = 0;
    NUMTX PCSpecialProjectionTranslate = {0};
    NUMTX PCSpecialProjectionScale = {0};
    static i32 FrameShaderMaterialFirstCall = 1;
    i32 NuDisplayHandlerCameraId = -1;

    void NuMtlAnimateShaderMtlTextures(f32 frame_time) {
        i32 i;
        FrameShaderMaterialAnimationTime += frame_time;
        if (FrameShaderMaterialFirstCall != 0) {
            for (i = 0; i < nudlist_field_9d8; ++i) {
                NUDISPLAY_LIST *scene = PCDisplayScenes[i];
                if (scene->gscene->shader_texture_animation_enabled != 0) {
                    i32 j;
                    for (j = 0; j < (i32)scene->material_count; ++j) {
                        NUMTL *material = scene->display_materials[j];
                        i32 layer;
                        for (layer = 0; layer < 4; ++layer) {
                            material->shader.animation_offsets[layer][0] = 0.0f;
                            material->shader.animation_offsets[layer][1] = 0.0f;
                        }
                    }
                }
            }
            FrameShaderMaterialFirstCall = 0;
        }
    }

    void NuMtlAnimate(f32 frame_time) {
        NuDisplayListAnimateMtls(frame_time * FrameMaterialAnimationSpeed);
        NuMtlAnimateShaderMtlTextures(frame_time);
    }

    void PCFogSetMode(i32 enable, i32 mode) {
        PCFogEnabled = enable != 0;
        PCDeviceSetRenderState(D3DRS_FOGENABLE, PCFogEnabled);
        if (enable) {
            if (mode == 1) {
                PCDeviceSetRenderState(D3DRS_FOGTABLEMODE, 1);
            } else {
                PCDeviceSetRenderState(D3DRS_FOGTABLEMODE, 3);
            }
        } else {
            PCDeviceSetRenderState(D3DRS_FOGTABLEMODE, 0);
        }
    }

    void NuRndrInitEx(i32 size, VARIPTR *buffer, VARIPTR end) {
        NuRndrStreamInit(size, buffer);
        NuRndrInit(buffer, end);
    }

    static i32 RendererConfigEnabled = 0;
    static i32 RendererConfigSecondEnabled = 0;
    i32 dat_02854c90 = 0;
    i32 dat_02854c94 = 0;
    i32 NuRndrShadowCnt = 0;
    i32 PCFogEnabled = 0;
    i32 PCFogColourSuppressed = 0;
    i32 PCFramePresentationState = 0;
    f32 NuRndrFxHeightOverride = 0;
    NUMTL *NuRndrRectMaterial = NULL;
    void (*RenderFrameCallback)(void) = NULL;
    NUMTX *NuRndr_DebrisRotMtxPtr = 0;
    i32 NuDebrisUseSystemMemory = 0;
    static NUMTX RendererConfigMatrix = {0};
    u32 nuprim_field_ba8 = 0;
    NULGTVERTEX *NuDebrisSystemBuffers[8] = {0};
    u32 RenderFrameSceneFlags = 0;
    i32 PCGraphicsRenderTextureBytes = 0;
    static PC_BUFFER_RESOURCE *RendererGeometryVertexBuffer = 0;
    static PC_BUFFER_RESOURCE *RendererGeometryIndexBuffer = 0;

    void NuRndrGeometryBufferRelease(void) {
        PCDeviceReleaseTexture((PC_TEXTURE_RESOURCE **)&RendererGeometryVertexBuffer);
    }

    void NuRndrGeometryBufferRestore(void) {
        PCDeviceReleaseTexture((PC_TEXTURE_RESOURCE **)&RendererGeometryVertexBuffer);
        PCDeviceCreateVertexBuffer(0x40000, 0x208, 0, 0, &RendererGeometryVertexBuffer);
        NuRndrQuadBufferInit();
    }

    typedef struct RENDERER_SHADOW_DEFAULTS { // size 0x10
        f32 state0;                           // 0x0
        f32 state1;                           // 0x4
        f32 state2;                           // 0x8
        u32 packed_colour;                    // 0xc
    } RENDERER_SHADOW_DEFAULTS;

    typedef struct RENDERER_SCREEN_VERTEX { // size 0x1c
        f32 x;                              // 0x00
        f32 y;                              // 0x04
        f32 z;                              // 0x08
        f32 w;                              // 0x0c
        u32 attribute_bits;                 // 0x10
        u8 unknown_14[8];                   // 0x14
    } RENDERER_SCREEN_VERTEX;

    void NuRndrQuadBufferInit(void) {
        RENDERER_SCREEN_VERTEX *quad;
        if (RendererQuadVertexBuffer != NULL) {
            RendererQuadVertexBuffer->methods->release(RendererQuadVertexBuffer);
            RendererQuadVertexBuffer = NULL;
        }
        PCDeviceCreateVertexBuffer(0x70, 0, 0, 1, &RendererQuadVertexBuffer);
        if (RendererQuadVertexBuffer->methods->lock(RendererQuadVertexBuffer, 0, 0, (void **)&quad, 0) < 0) {
            return;
        }
        quad[0].x = 0.0f;
        quad[0].y = 0.0f;
        quad[0].z = 0.5f;
        quad[1].x = (f32)PCDeviceBackend.parameters.width;
        quad[1].y = 0.0f;
        quad[1].z = 0.5f;
        quad[2].x = (f32)PCDeviceBackend.parameters.width;
        quad[2].y = (f32)PCDeviceBackend.parameters.height;
        quad[2].z = 0.5f;
        quad[3].x = 0.0f;
        quad[3].y = (f32)PCDeviceBackend.parameters.height;
        quad[3].z = 0.5f;
        quad[3].attribute_bits = 0x4b000000;
        quad[2].attribute_bits = 0x4b000000;
        quad[1].attribute_bits = 0x4b000000;
        quad[0].attribute_bits = 0x4b000000;
        quad[3].w = 1.0f;
        quad[2].w = 1.0f;
        quad[1].w = 1.0f;
        quad[0].w = 1.0f;
        RendererQuadVertexBuffer->methods->unlock(RendererQuadVertexBuffer);
    }

    void NuRndrGeometryBuffersInit(void) {
        u16 *indices;
        i32 i;
        u32 base;
        if (RendererGeometryVertexBuffer != NULL) {
            RendererGeometryVertexBuffer->methods->release(RendererGeometryVertexBuffer);
            RendererGeometryVertexBuffer = NULL;
        }
        if (RendererGeometryIndexBuffer != NULL) {
            RendererGeometryIndexBuffer->methods->release(RendererGeometryIndexBuffer);
            RendererGeometryIndexBuffer = NULL;
        }
        PCDeviceReleaseTexture((PC_TEXTURE_RESOURCE **)&RendererGeometryVertexBuffer);
        PCDeviceCreateVertexBuffer(0x40000, 0x208, 0, 0, &RendererGeometryVertexBuffer);
        NuRndrQuadBufferInit();
        PCDeviceCreateIndexBuffer(0xc000, 0, 0x65, 1, &RendererGeometryIndexBuffer, NULL);
        if (RendererGeometryIndexBuffer->methods->lock(RendererGeometryIndexBuffer, 0, 0, (void **)&indices, 0) < 0) {
            return;
        }
        base = 0;
        for (i = 0; i < 0x1000; ++i) {
            *indices++ = (u16)base;
            *indices++ = (u16)(base + 1);
            *indices++ = (u16)(base + 2);
            *indices++ = (u16)base;
            *indices++ = (u16)(base + 2);
            *indices++ = (u16)(base + 3);
            base += 4;
        }
        RendererGeometryIndexBuffer->methods->unlock(RendererGeometryIndexBuffer);
    }

    void NuRndrBufferDescriptorsInit(VARIPTR *buffer, VARIPTR end) {
        PC_VERTEX_ELEMENT elements[3];
        elements[0].stream = 0;
        elements[0].offset = 0;
        elements[0].type = 2;
        elements[0].method = 0;
        elements[0].usage = 0;
        elements[0].usage_index = 0;
        elements[1].stream = 0;
        elements[1].offset = 12;
        elements[1].type = 4;
        elements[1].method = 0;
        elements[1].usage = 10;
        elements[1].usage_index = 0;
        elements[2].stream = 0xff;
        elements[2].offset = 0;
        elements[2].type = 17;
        elements[2].method = 0;
        elements[2].usage = 0;
        elements[2].usage_index = 0;
        PCDeviceCreateVertexDeclaration(elements, &RendererVertexDeclaration);
        NuRndrArenaInit(buffer, end);
        NuRndrQuadBufferInit();
        NuRndrGeometryBuffersInit();
    }

    /* Sizes the post-effect targets (power-of-two when the device needs it) and claims the back-buffer copy. */
    void PCGraphicsCreatePostEffectTextures(u32 width, u32 height) {
        GRAPHICS_RENDER_TEXTURE *texture;
        PCGraphicsPostEffectWidth = width;
        if (width >= 512) {
            PCGraphicsPostEffectWidth = 512;
        }
        PCGraphicsPostEffectHeight = height;
        if (height >= 512) {
            PCGraphicsPostEffectHeight = 512;
        }
        if ((PCDeviceBackend.caps.texture_caps & D3DPTEXTURECAPS_POW2) != 0 ||
            (PCDeviceBackend.caps.texture_caps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) != 0) {
            u32 width_power = 1;
            u32 height_power = 1;
            while (width_power <= (width < 512 ? width : 512)) {
                width_power += width_power;
            }
            PCGraphicsPostEffectWidth = width_power >> 1;
            while (height_power <= (height < 512 ? height : 512)) {
                height_power += height_power;
            }
            PCGraphicsPostEffectHeight = height_power >> 1;
        }
        PCGraphicsCreatePostEffectWorkTexture(width, height);
        texture = PCGraphicsAllocRenderTexture();
        PCGraphicsInitRenderTexture(texture, PCGraphicsPostEffectWidth, PCGraphicsPostEffectHeight, 0, 1, 0, 2);
        PCGraphicsPostEffectBackBufferTexture = texture;
    }

    /* Re-sizes the post-effect targets after a mode change, with the same clamps as PCGraphicsCreatePostEffectTextures.
     */
    void PCGraphicsResizePostEffectTextures(u32 width, u32 height) {
        u32 target_width = width < 512 ? width : 512;
        u32 target_height;
        PCGraphicsPostEffectWidth = target_width;
        target_height = height < 512 ? height : 512;
        PCGraphicsPostEffectHeight = target_height;
        if ((PCDeviceBackend.caps.texture_caps & D3DPTEXTURECAPS_POW2) != 0 ||
            (PCDeviceBackend.caps.texture_caps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) != 0) {
            target_width = 1;
            target_height = 1;
            while (target_width <= (width < 512 ? width : 512)) {
                target_width += target_width;
            }
            target_width >>= 1;
            PCGraphicsPostEffectWidth = target_width;
            while (target_height <= (height < 512 ? height : 512)) {
                target_height += target_height;
            }
            target_height >>= 1;
            PCGraphicsPostEffectHeight = target_height;
        }
        PCGraphicsResizeEffectTexture(PCGraphicsPostEffectWorkTexture, target_width, target_height, -1);
        PCGraphicsResizeEffectTexture(PCGraphicsPostEffectBackBufferTexture, PCGraphicsPostEffectWidth, PCGraphicsPostEffectHeight, -1);
        if (PCGraphicsPostEffectAuxTexture != NULL) {
            PCGraphicsResizeEffectTexture(PCGraphicsPostEffectAuxTexture, -1, -1, -1);
        }
    }

    void NuRndrInit(VARIPTR *buffer, VARIPTR end) {
        RENDERER_SHADOW_DEFAULTS *shadow = (RENDERER_SHADOW_DEFAULTS *)&PCShadowState;
        NUVEC defaults;
        i32 i;
        RendererConfigEnabled = 1;
        RendererConfigSecondEnabled = 1;
        NuMtxSetIdentity(&RendererConfigMatrix);
        NuRndrBufferDescriptorsInit(buffer, end);
        defaults.x = 0.0f;
        defaults.y = -0.75f;
        defaults.z = -0.75f;
        *(NUVEC *)&shadow->state0 = defaults;
        PCShadowNearValue = 4.0f;
        PCShadowFarValue = 6.0f;
        shadow->packed_colour = 0x00101010;
        NuRndrShadowCnt = 0;
        for (i = 0; i < 32; ++i) {
            RendererClearRecords[i].active = 0;
        }
        NuRndrInitGeneric();
    }

    void NuRndrInitStartupMaterials(void) {
        PCStartupMaterial1 = NuMtlCreate(1);
        PCStartupMaterial1->diffuse_b = PCStartupMaterial1->diffuse_g = PCStartupMaterial1->diffuse_r = 1.0f;
        PCStartupMaterial1->attribs.word = (PCStartupMaterial1->attribs.word & ~0x1000u) | 0x2000u;
        PCStartupMaterial1->attribs.word = (PCStartupMaterial1->attribs.word & ~0x8000u) | 0x4000u;
        PCStartupMaterial1->opacity = 0.999f;
        PCStartupMaterial1->attribs.word = (PCStartupMaterial1->attribs.word & 0xfffffff1u) | 1u;
        PCStartupMaterial1->attribs2 = (PCStartupMaterial1->attribs2 & ~2u) | 1u;
        PCStartupMaterial1->attribs.word = (PCStartupMaterial1->attribs.word & ~0x200u) | 0x100u;
        PCStartupMaterial1->attribs.word = (PCStartupMaterial1->attribs.word & ~0x800u) | 0x400u;
        PCStartupMaterial1->attribs.word = (PCStartupMaterial1->attribs.word & ~0x10000u) | 0x20000u;
        PCStartupMaterial1->texture_id = 0;
        PCStartupMaterial1->sort_priority = 0x100;
        NuMtlUpdate(PCStartupMaterial1);

        PCStartupMaterial2 = NuMtlCreate(1);
        PCStartupMaterial2->diffuse_b = PCStartupMaterial2->diffuse_g = PCStartupMaterial2->diffuse_r = 1.0f;
        PCStartupMaterial2->attribs.word = (PCStartupMaterial2->attribs.word & ~0x1000u) | 0x2000u;
        PCStartupMaterial2->attribs.word = (PCStartupMaterial2->attribs.word & ~0x8000u) | 0x4000u;
        PCStartupMaterial2->opacity = 0.999f;
        PCStartupMaterial2->attribs.word = (PCStartupMaterial2->attribs.word & 0xfffffff2u) | 2u;
        PCStartupMaterial2->attribs2 = (PCStartupMaterial2->attribs2 & ~2u) | 1u;
        PCStartupMaterial2->attribs.word = (PCStartupMaterial2->attribs.word & ~0x200u) | 0x100u;
        PCStartupMaterial2->attribs.word = (PCStartupMaterial2->attribs.word & ~0x800u) | 0x400u;
        PCStartupMaterial2->attribs.word = (PCStartupMaterial2->attribs.word & ~0x10000u) | 0x20000u;
        PCStartupMaterial2->texture_id = 0;
        PCStartupMaterial2->sort_priority = 0x100;
        NuMtlUpdate(PCStartupMaterial2);
    }

    void NuRndrFx(i32 paused, void *context) {
        NuLgtLaserDraw(paused);
        NuLgtArcLaserDraw(paused);
        if (NuRndrFxHeightOverride != 0.0f) {
            NuRndrFxHeight = NuRndrFxHeightOverride;
            NuRndrFxColour = 0xc04080c0;
        }
        if (paused != 0) {
            NuRndrFxHeight = NuRndrFxPreviousHeight;
        } else {
            NuRndrFxPreviousHeight = NuRndrFxHeight;
        }
        currentScene.clear_flags = 1;
    }

    i32 NuRndrSetFxMtx(NUMTX *matrix) {
        return 1;
    }

    void fun_005fe3e0(void) {
    }
} // extern "C"

/* nucamera's platform members: NuCameraSet, the extents clip test and the projection matrix. */
extern "C" {
    void NuCameraSet(NUCAMERA *camera) {
        NuCameraSetEx(camera, 0);

        NuVpSetIdentity(&cmtx);
        NuMtxMulH(&vpc_vport_mtx, &vpmtx, &cmtx);
        vpc_sci_mtx = vpc_vport_mtx;

        PCDeviceSetTransform(2, &vmtx);
        PCDeviceSetTransform(D3DTS_PROJECTION, &pmtx);
    }

    void NuCameraSetGlobalParameters(NUMTX *mtx, f32 fov, f32 aspect, f32 near_clip, f32 far_clip, f32 portal_nearclip) {
        NUCAMERA camera;
        camera.mtx = *mtx;
        camera.fov = fov;
        camera.aspect = aspect;
        camera.near_clip = near_clip;
        camera.far_clip = far_clip;
        camera.portal_nearclip = portal_nearclip;
        NuCameraSetEx(&camera, 0);
        NuVpSetIdentity(&cmtx);
        NuMtxMulH(&vpc_vport_mtx, &vpmtx, &cmtx);
        vpc_sci_mtx = vpc_vport_mtx;
        PCDeviceSetTransform(2, &vmtx);
        PCDeviceSetTransform(D3DTS_PROJECTION, &pmtx);
    }

    void NuCameraGetGlobalParameters(NUMTX *mtx, f32 *fov, f32 *aspect, f32 *near_clip, f32 *far_clip, f32 *portal_nearclip) {
        *mtx = global_camera.mtx;
        *fov = global_camera.fov;
        *aspect = global_camera.aspect;
        *near_clip = global_camera.near_clip;
        *far_clip = global_camera.far_clip;
        *portal_nearclip = global_camera.portal_nearclip;
    }

    void NuCameraStackReset(void) {
        NuCameraStackTop = NuCameraStack;
    }

    void NuCameraPush(void) {
        if (NuCameraStackTop != NuCameraStack + 64) {
            NuCameraStackTop->camera = global_camera;
            NuCameraStackTop->camera_id = (i16)render_state.camera_id;
            NuCameraStackTop++;
        } else {
            PCGraphicsRenderTextureAllocationError();
        }
    }

    void NuCameraPop(void) {
        if (NuCameraStackTop != NuCameraStack) {
            NuCameraStackTop--;
            if (NuCameraStackTop->camera_id != (i16)render_state.camera_id) {
                NuCameraSetEx(&NuCameraStackTop->camera, 0);
                render_state.camera_id = NuCameraStackTop->camera_id;
            }
        } else {
            PCGraphicsRenderTextureAllocationError();
        }
    }

    NUMTX *NuCameraCopyViewMatrix(NUMTX *matrix) {
        *matrix = vmtx;
        return matrix;
    }

    NUMTX *NuCameraCopyProjectionMatrix(NUMTX *matrix) {
        *matrix = pmtx;
        return matrix;
    }

    i32 NuCameraClipTestExtents(NUVEC *min, NUVEC *max, NUMTX *world, f32 far_clip, i32 clip_screen) {
        NUVEC corners[8];
        NUVEC transformed[8];
        u8 codes[8];
        i32 i;

        if (PCClipExtentsEnabled == 0) {
            return 1;
        }
        if (clip_screen != 0) {
            PCClipScreenUnsupported();
            for (;;) {
            }
        }
        if (far_clip == 0.0f) {
            far_clip = global_camera.far_clip_override != 0.0f ? global_camera.far_clip_override : global_camera.far_clip;
        }
        if (world == NULL || world == &NuMtxSetIdentityTemplate) {
            clip_test_mtx = vmtx;
        } else {
            NuMtxMul(&clip_test_mtx, world, &vmtx);
        }

        ClipFarLimits[3] = far_clip;
        ClipFarLimits[2] = far_clip;
        ClipFarLimits[1] = far_clip;
        ClipFarLimits[0] = far_clip;
        if (PCClipUseSSE == 0) {
            corners[0].x = min->x;
            corners[0].y = min->y;
            corners[0].z = min->z;
            corners[1].x = max->x;
            corners[1].y = max->y;
            corners[1].z = max->z;
            corners[2].x = min->x;
            corners[2].y = min->y;
            corners[2].z = max->z;
            corners[3].x = max->x;
            corners[3].y = max->y;
            corners[3].z = min->z;
            corners[4].x = min->x;
            corners[4].y = max->y;
            corners[4].z = min->z;
            corners[5].x = min->x;
            corners[5].y = max->y;
            corners[5].z = max->z;
            corners[6].x = max->x;
            corners[6].y = min->y;
            corners[6].z = min->z;
            corners[7].x = max->x;
            corners[7].y = min->y;
            corners[7].z = max->z;
            NuVecMtxTransformBlock(transformed, corners, &clip_test_mtx, 8);
            memset(codes, 0, sizeof(codes));
            for (i = 0; i < 8; i += 4) {
                NUCAMERA_CLIP_CODE_EX(codes[i], transformed[i], 1, far_clip, zx, zy);
                NUCAMERA_CLIP_CODE_EX(codes[i + 1], transformed[i + 1], 1, far_clip, zx, zy);
                NUCAMERA_CLIP_CODE_EX(codes[i + 2], transformed[i + 2], 1, far_clip, zx, zy);
                NUCAMERA_CLIP_CODE_EX(codes[i + 3], transformed[i + 3], 1, far_clip, zx, zy);
            }
            if ((codes[0] & codes[1] & codes[2] & codes[3] & codes[4] & codes[5] & codes[6] & codes[7]) != 0) {
                return 0;
            }
            return (codes[0] | codes[1] | codes[2] | codes[3] | codes[4] | codes[5] | codes[6] | codes[7]) != 0 ? 2 : 1;
        } else {
            u32 *outcode = clip_outcode_workspace_prefix;
            ClipCornerBlocks[0].x[0] = min->x;
            ClipCornerBlocks[0].y[0] = min->y;
            ClipCornerBlocks[0].z[0] = min->z;
            ClipCornerBlocks[0].x[1] = max->x;
            ClipCornerBlocks[0].y[1] = max->y;
            ClipCornerBlocks[0].z[1] = max->z;
            ClipCornerBlocks[0].x[2] = min->x;
            ClipCornerBlocks[0].y[2] = min->y;
            ClipCornerBlocks[0].z[2] = max->z;
            ClipCornerBlocks[0].x[3] = max->x;
            ClipCornerBlocks[0].y[3] = max->y;
            ClipCornerBlocks[0].z[3] = min->z;
            ClipCornerBlocks[1].x[0] = min->x;
            ClipCornerBlocks[1].y[0] = max->y;
            ClipCornerBlocks[1].z[0] = min->z;
            ClipCornerBlocks[1].x[1] = min->x;
            ClipCornerBlocks[1].y[1] = max->y;
            ClipCornerBlocks[1].z[1] = max->z;
            ClipCornerBlocks[1].x[2] = max->x;
            ClipCornerBlocks[1].y[2] = min->y;
            ClipCornerBlocks[1].z[2] = min->z;
            ClipCornerBlocks[1].x[3] = max->x;
            ClipCornerBlocks[1].y[3] = min->y;
            ClipCornerBlocks[1].z[3] = max->z;
            PCVecMtxTransformClipBlocks(ClipTransformedBlocks, ClipCornerBlocks, &clip_test_mtx, 8, outcode);
            if ((clip_outcode_workspace_prefix[7] & clip_outcode_workspace_prefix[6] & clip_outcode_workspace_prefix[5] &
                 clip_outcode_workspace_prefix[4] & clip_outcode_workspace_prefix[3] & clip_outcode_workspace_prefix[2] &
                 clip_outcode_workspace_prefix[1] & clip_outcode_workspace_prefix[0]) != 0) {
                return 0;
            }
            return (clip_outcode_workspace_prefix[7] | clip_outcode_workspace_prefix[6] | clip_outcode_workspace_prefix[5] |
                    clip_outcode_workspace_prefix[4] | clip_outcode_workspace_prefix[3] | clip_outcode_workspace_prefix[2] |
                    clip_outcode_workspace_prefix[1] | clip_outcode_workspace_prefix[0]) != 0
                       ? 2
                       : 1;
        }
    }

    void PCClipSetExtentsEnabled(i32 enabled) {
        PCClipExtentsEnabled = enabled;
    }

    NUMTX *NuCameraGetScreenMatrix(void) {
        return &smtx;
    }

    void NuCameraSetProjectionMtx(NUMTX *out, f32 fov, f32 aspect, f32 near_clip, f32 far_clip) {
        f32 half_fov = (f32)((f64)fov * 0.5);
        i32 angle = (i32)((f64)half_fov * AIPathAngularScale);
        f32 cotangent = NuTrigTable[((angle + 0x4000) >> 1) & 0x7fff] / NuTrigTable[(angle >> 1) & 0x7fff];
        f32 depth_factor = far_clip / (far_clip - near_clip);
        memset(out, 0, sizeof(*out));
        out->m00 = aspect * cotangent;
        out->m11 = cotangent;
        out->m22 = depth_factor;
        out->m23 = 1.0f;
        out->m32 = -depth_factor * near_clip;
        pmtx.m00 *= NuCameraProjectionGlobalScaleX;
        pmtx.m11 *= NuCameraProjectionGlobalScaleY;
        pmtx.m20 = NuCameraProjectionGlobalOffsetX;
        pmtx.m21 = NuCameraProjectionGlobalOffsetY;
    }

    i32 NuCameraClipTestExtentsDefault(NUVEC *min, NUVEC *max) {
        return NuCameraClipTestExtents(min, max, NuCameraGetClippingMtx(), 0.0f, 0);
    }

    void fun_00604bf0(void) {
    }
} // extern "C"

/* nuvec's platform members: the block transforms. */
extern "C" {
    void NuVecMtxTransformBlock(NUVEC *out, NUVEC *vectors, NUMTX *matrix, i32 count) {
        i32 index;

        for (index = 0; index < count; ++index) {
            out->x = vectors->x * matrix->m00 + vectors->y * matrix->m10 + vectors->z * matrix->m20 + matrix->m30;
            out->y = vectors->x * matrix->m01 + vectors->y * matrix->m11 + vectors->z * matrix->m21 + matrix->m31;
            out->z = vectors->x * matrix->m02 + vectors->y * matrix->m12 + vectors->z * matrix->m22 + matrix->m32;
            out++;
            vectors++;
        }
    }

    void PCVecMtxTransformClipBlocks(NUVEC4BLOCK *destination, NUVEC4BLOCK *source, NUMTX *matrix, i32 count, u32 *outcodes) {
        count = (count + 3) >> 2;
        if (count <= 0) {
            return;
        }
        __asm {
        push esi
        push edi
        push ebx
        push ecx
        push edx
        mov edi, destination
        mov esi, source
        mov ebx, outcodes
        mov ecx, matrix
        mov edx, count
        movaps xmm4, [ecx]
        movaps xmm5, [ecx + 0x10]
        movaps xmm6, [ecx + 0x20]
        movaps xmm7, [ecx + 0x30]
    next_block:
        movaps xmm0, [esi]
        movaps xmm1, xmm4
        shufps xmm1, xmm1, 0x0
        mulps xmm1, xmm0
        movaps xmm0, [esi + 0x10]
        movaps xmm2, xmm5
        shufps xmm2, xmm2, 0x0
        mulps xmm2, xmm0
        movaps xmm0, [esi + 0x20]
        movaps xmm3, xmm6
        shufps xmm3, xmm3, 0x0
        mulps xmm3, xmm0
        movaps xmm0, xmm7
        shufps xmm0, xmm0, 0x0
        addps xmm0, xmm1
        addps xmm0, xmm2
        addps xmm0, xmm3
        movaps [edi], xmm0
        add edi, 0x10
        movaps xmm0, [esi]
        movaps xmm1, xmm4
        shufps xmm1, xmm1, 0x55
        mulps xmm1, xmm0
        movaps xmm0, [esi + 0x10]
        movaps xmm2, xmm5
        shufps xmm2, xmm2, 0x55
        mulps xmm2, xmm0
        movaps xmm0, [esi + 0x20]
        movaps xmm3, xmm6
        shufps xmm3, xmm3, 0x55
        mulps xmm3, xmm0
        movaps xmm0, xmm7
        shufps xmm0, xmm0, 0x55
        addps xmm0, xmm1
        addps xmm0, xmm2
        addps xmm0, xmm3
        movaps [edi], xmm0
        add edi, 0x10
        movaps xmm0, [esi]
        movaps xmm1, xmm4
        shufps xmm1, xmm1, 0xaa
        mulps xmm1, xmm0
        movaps xmm0, [esi + 0x10]
        movaps xmm2, xmm5
        shufps xmm2, xmm2, 0xaa
        mulps xmm2, xmm0
        movaps xmm0, [esi + 0x20]
        movaps xmm3, xmm6
        shufps xmm3, xmm3, 0xaa
        mulps xmm3, xmm0
        movaps xmm0, xmm7
        shufps xmm0, xmm0, 0xaa
        addps xmm0, xmm1
        addps xmm0, xmm2
        addps xmm0, xmm3
        movaps [edi], xmm0
        add edi, 0x10
        movaps xmm3, xmm0
        movss xmm1, global_camera.near_clip
        shufps xmm1, xmm1, 0x0
        cmpltps xmm0, xmm1
        andps xmm0, ClipOutcodeMask32
        movaps xmm2, xmm0
        movaps xmm0, xmm3
        cmpnleps xmm0, ClipFarLimits
        andps xmm0, ClipOutcodeMask16
        orps xmm2, xmm0
        movss xmm1, zx
        shufps xmm1, xmm1, 0x0
        mulps xmm1, xmm3
        mulps xmm1, ClipOutcodeMinusOne
        movaps xmm0, [edi - 0x30]
        cmpltps xmm0, xmm1
        andps xmm0, ClipOutcodeMask1
        orps xmm2, xmm0
        mulps xmm1, ClipOutcodeMinusOne
        movaps xmm0, [edi - 0x30]
        cmpnleps xmm0, xmm1
        andps xmm0, ClipOutcodeMask2
        orps xmm2, xmm0
        movss xmm1, zy
        shufps xmm1, xmm1, 0x0
        mulps xmm1, xmm3
        mulps xmm1, ClipOutcodeMinusOne
        movaps xmm0, [edi - 0x20]
        cmpltps xmm0, xmm1
        andps xmm0, ClipOutcodeMask8
        orps xmm2, xmm0
        mulps xmm1, ClipOutcodeMinusOne
        movaps xmm0, [edi - 0x20]
        cmpnleps xmm0, xmm1
        andps xmm0, ClipOutcodeMask4
        orps xmm2, xmm0
        movaps [ebx], xmm2
        add ebx, 0x10
        add esi, 0x30
        dec edx
        jne next_block
        pop edx
        pop ecx
        pop ebx
        pop edi
        pop esi
        }
    }
} // extern "C"
