#ifndef SAGA_LEGOAPI_PROPS_OBJECTS_ZIPUP_H
#define SAGA_LEGOAPI_PROPS_OBJECTS_ZIPUP_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/legoapi_types.h (not all of it earlier in
 * the shim). */

#include "legoapi/menus/core/hint.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 ZipUps_UpdateHint(HINT_PC *);
    EXPLOSION_PC *Detonate(NUVEC *, u16);
    struct zipup_progress_pc_s;
    i32 ZipUps_Load(WORLDINFO *world);
    void *ZipUps_ReserveBufferSpace(WORLDINFO *world);
    void ZipUp_MoveCode(GAMEOBJECT *object, i32 special_pressed);
    void ZipUps_Reset(WORLDINFO *world, void *unused, struct zipup_progress_pc_s *progress);
    void ZipUps_Update(WORLDINFO *world, void *unused, f32 delta_time);
    void ZipUps_DrawLines(void);
    extern const NUVEC ZipUpHookOffsetPc;
    extern i32 CanDrawZipUpSwirls;
    extern f32 ZipUpMarkerOffsetYPc;
    extern f32 ZipUpMarkerOffsetZPc;
    extern f32 ZipUpRopeHalfWidthPc;
    extern u32 ZipUpRopeCheatColourPc;
    extern char *ZipUpTypeNamePtrPc;
    extern i32 ZipUpRopeModulationPc;
    extern char GrapAttach_text[11];
    extern char GrapDetach_text[11];
    extern char GrapDetach_text[11];
    extern char GrapAttach_text[11];
    extern const char GameAudioName0075c240Pc[5];
    extern const char GameAudioName0075c248Pc[12];
    extern const char GameAudioName0075c254Pc[13];
    extern const char GameAudioName0075c264Pc[11];
    extern const char GameAudioName0075c270Pc[12];
#ifdef __cplusplus
}
#endif

#endif
