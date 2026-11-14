#ifndef SAGA_LEGOAPI_RENDER_CORE_RENDER_H
#define SAGA_LEGOAPI_RENDER_CORE_RENDER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/numtl.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void DrawGameMessage_Targets(GAME_MESSAGE_PC *message, NUVEC *position, f32 scale);
    i32 DrawPanel3DObject(f32, f32, f32, f32, f32, f32, u16, u16, u16, NUSPECIALHANDLE *, i32, f32);
    void DrawCharIcon(i32, f32, f32, f32, f32, i32, f32, f32, i32, NUSPECIALHANDLE *);
    extern i32 dco_locatorposonly;
    extern void (*DisguiseAdjustFn)(i32, i32, NUVEC *, NUVEC *);
    void DrawObjectOnCharacter(WORLDINFO *, GAMEOBJECT *, i32, NUSPECIALHANDLE *, i32, i32, NUMTX *, i32, u32, NUMTX *,
                               NUVEC *, f32, f32);
    i32 Draw3DObjectMtx(WORLDINFO *, i32, NUMTX *);
    extern f32 KITPOSX;
    extern f32 KITPOSY;
    extern f32 PANEL_MINIKITSCALE;
    extern f32 PANEL_MINIKITY;
    void RndrOSphere(NUVEC *centre, f32 radius, i32 colour, i32 segments, NUMTL *mtl);
    void RndrOSquare(NUVEC *centre, f32 radius, i32 colour, NUMTL *mtl);
    void BackDrop_ResetColours(void);
    i32 DrawPanel3DObjectNoAlpha(f32 x, f32 y, f32 z, f32 scale_x, f32 scale_y, f32 scale_z, u16 rotate_x, u16 rotate_y,
                                 u16 rotate_z, NUSPECIALHANDLE *special, i32 rotate_order);
    void DrawMiniSnowTroopers(struct WORLDINFO_s *world);
    void DrawMiniKitCount(f32, f32, i32, i32);

    i32 DrawPanel3DObjectMtx(NUSPECIALHANDLE *special, NUMTX *matrix, f32 alpha);
#ifdef __cplusplus
}
#endif

#endif
