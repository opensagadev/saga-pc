/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

/* saga.h includes this header once per part (SAGA_SHIM), so `#pragma once` holds for direct includes only; the
 * condition and the _PART selector go away with the shim. */
#ifndef SAGA_SHIM
#pragma once
#endif
#ifndef SAGA_LEGOAPI_ITEMS_COLLECT_THERMALDETONATORS_H_PART
#define SAGA_LEGOAPI_ITEMS_COLLECT_THERMALDETONATORS_H_PART 1
#endif

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/menus/core/hint.h"
#endif

struct PART_PC;
struct gameobject_s;

/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_LEGOAPI_ITEMS_COLLECT_THERMALDETONATORS_H_PART == 1 &&                                                        \
    !defined(SAGA_LEGOAPI_ITEMS_COLLECT_THERMALDETONATORS_H_STOP)
typedef void (*PART_ACTION_PC)(struct PART_PC *);
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
    i32 ThermalDetonator_MoveCode(struct gameobject_s *);
    void PartImpact_ThermalDetonator(struct PART_PC *part);
    void PartUpdate_ThermalDetonator(struct PART_PC *part);
    i32 PartDraw_ThermalDetonator(struct PART_PC *part);
    void PartKill_ThermalDetonator(struct PART_PC *part, i32 unused);
    void ThermalDetonator_Throw(struct gameobject_s *object);
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
