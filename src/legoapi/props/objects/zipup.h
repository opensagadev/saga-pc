#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/menus/core/hint.h"
#include "legoapi/world/world.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    i32 ZipUps_UpdateHint(HINT *);
    EXPLOSION *Detonate(NUVEC *, u16);
    struct zipup_progress_s;
    i32 ZipUps_Load(WORLDINFO *world);
    void *ZipUps_ReserveBufferSpace(WORLDINFO *world);
    void ZipUp_MoveCode(GAMEOBJECT *object, i32 special_pressed);
    void ZipUps_Reset(WORLDINFO *world, void *unused, struct zipup_progress_s *progress);
    void ZipUps_Update(WORLDINFO *world, void *unused, f32 delta_time);
    void ZipUps_DrawLines(void);
    extern const NUVEC ZipUpHookOffset;
    extern i32 CanDrawZipUpSwirls;
    extern f32 ZipUpMarkerOffsetY;
    extern f32 ZipUpMarkerOffsetZ;
    extern f32 ZipUpRopeHalfWidth;
    extern u32 ZipUpRopeCheatColour;
    extern char *ZipUpTypeNamePtr;
    extern i32 ZipUpRopeModulation;
    extern char GrapAttach_text[11];
    extern char GrapDetach_text[11];
    extern char GrapDetach_text[11];
    extern char GrapAttach_text[11];
#ifdef __cplusplus
}
#endif
