#ifndef SAGA_LEGOAPI_ITEMS_COLLECT_THERMALDETONATORS_H_PART
#define SAGA_LEGOAPI_ITEMS_COLLECT_THERMALDETONATORS_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/items/base/apiobject.h, legoapi/items/objects/objectsall.h,
 * legoapi/menus/core/hint.h, nu2api/nucore/fixed_width.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/items/base/apiobject.h"
#include "legoapi/items/objects/objectsall.h"
#include "legoapi/menus/core/hint.h"
#include "nu2api/nucore/fixed_width.h"
#endif
/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_ITEMS_COLLECT_THERMALDETONATORS_H_PART == 1 &&                                                        \
    !defined(SAGA_LEGOAPI_ITEMS_COLLECT_THERMALDETONATORS_H_STOP)
typedef void (*PART_ACTION_PC)(PART_PC *);
#ifdef __cplusplus
extern "C" {
#endif
    i32 ShinyMetal_UpdateHint(HINT_PC *);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_COLLECT_THERMALDETONATORS_H_PART
#define SAGA_LEGOAPI_ITEMS_COLLECT_THERMALDETONATORS_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_COLLECT_THERMALDETONATORS_H_STOP
#endif
#endif

#if SAGA_LEGOAPI_ITEMS_COLLECT_THERMALDETONATORS_H_PART == 2 &&                                                        \
    !defined(SAGA_LEGOAPI_ITEMS_COLLECT_THERMALDETONATORS_H_STOP)
#ifdef __cplusplus
extern "C" {
#endif
    i32 ThermalDetonator_MoveCode(GAMEOBJECT *);
    void PartImpact_ThermalDetonator(PART_PC *part);
    void PartUpdate_ThermalDetonator(PART_PC *part);
    i32 PartDraw_ThermalDetonator(PART_PC *part);
    void PartKill_ThermalDetonator(PART_PC *part, i32 unused);
    void ThermalDetonator_Throw(GAMEOBJECT *object);
#ifdef __cplusplus
}
#endif
#undef SAGA_LEGOAPI_ITEMS_COLLECT_THERMALDETONATORS_H_PART
#define SAGA_LEGOAPI_ITEMS_COLLECT_THERMALDETONATORS_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_LEGOAPI_ITEMS_COLLECT_THERMALDETONATORS_H_STOP
#endif
#endif

#undef SAGA_LEGOAPI_ITEMS_COLLECT_THERMALDETONATORS_H_STOP
