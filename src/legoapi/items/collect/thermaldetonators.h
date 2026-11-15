#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/menus/core/hint.h"

struct PART;
struct gameobject_s;

typedef void (*PART_ACTION)(struct PART *);
#ifdef __cplusplus
extern "C" {
#endif
    i32 ShinyMetal_UpdateHint(HINT *);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
    i32 ThermalDetonator_MoveCode(struct gameobject_s *);
    void PartImpact_ThermalDetonator(struct PART *part);
    void PartUpdate_ThermalDetonator(struct PART *part);
    i32 PartDraw_ThermalDetonator(struct PART *part);
    void PartKill_ThermalDetonator(struct PART *part, i32 unused);
    void ThermalDetonator_Throw(struct gameobject_s *object);
#ifdef __cplusplus
}
#endif
