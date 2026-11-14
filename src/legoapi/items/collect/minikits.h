#ifndef SAGA_LEGOAPI_ITEMS_COLLECT_MINIKITS_H
#define SAGA_LEGOAPI_ITEMS_COLLECT_MINIKITS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h (not all of it earlier in the shim). */

#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/world/world.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 MiniKitModelId_PC;
    extern i32 CharKitModelId_PC;
    void CollectMinikit(NUVEC *, char *, i32);
    void MiniKit_GameMsg_Update(GAME_MESSAGE_PC *);
    void MiniKit_GameMsg_End(GAME_MESSAGE_PC *);
    void CharacterMiniKits_Dump(WORLDINFO *);
    extern i32 *Char_MiniKit;
    extern VARIPTR minikits_savecharacterbufferptr;
    extern const char GizmoConfigPathFormat[7];
    void ResetMinikitCounter(void);
    void CharMiniKit_Draw(i32, NUMTX *, i32, f32, f32);
    extern f32 KITPOS2X;
    extern const char *LEGOASCII_BIGARROW;
    void IncrementMinikitCounter(struct gameobject_s *object);
#ifdef __cplusplus
}
#endif

enum { AREAFLAG_MINIKIT = 16 };

#endif
