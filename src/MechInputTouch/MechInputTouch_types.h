#ifndef SAGA_MECHINPUTTOUCH_MECHINPUTTOUCH_TYPES_H_PART
#define SAGA_MECHINPUTTOUCH_MECHINPUTTOUCH_TYPES_H_PART 1
#endif

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* Included via saga.h only for now: legoapi/menus/screens/movies.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/menus/screens/movies.h"
#endif
/* 2 parts: saga.h includes this header once per part, each where its declarations belong; included directly, it gives
 * all parts at once. */

#if SAGA_MECHINPUTTOUCH_MECHINPUTTOUCH_TYPES_H_PART == 1 && !defined(SAGA_MECHINPUTTOUCH_MECHINPUTTOUCH_TYPES_H_STOP)
#ifdef __cplusplus
struct ThingProcessData;
struct ThingRenderData;
struct ThingResetData;
#endif
#undef SAGA_MECHINPUTTOUCH_MECHINPUTTOUCH_TYPES_H_PART
#define SAGA_MECHINPUTTOUCH_MECHINPUTTOUCH_TYPES_H_PART 2
#ifdef SAGA_SHIM
#define SAGA_MECHINPUTTOUCH_MECHINPUTTOUCH_TYPES_H_STOP
#endif
#endif

#if SAGA_MECHINPUTTOUCH_MECHINPUTTOUCH_TYPES_H_PART == 2 && !defined(SAGA_MECHINPUTTOUCH_MECHINPUTTOUCH_TYPES_H_STOP)
#ifdef __cplusplus

/* The packet ProcessThings hands every thing (NuMainUpdateFrame, Movie_CallBack). */
struct ThingProcessData {
    f32 frame_time;
    i32 paused;
    NUPAD_INIT_STATE **pads;
    i32 pad_count;
};
#endif
#undef SAGA_MECHINPUTTOUCH_MECHINPUTTOUCH_TYPES_H_PART
#define SAGA_MECHINPUTTOUCH_MECHINPUTTOUCH_TYPES_H_PART 3
#ifdef SAGA_SHIM
#define SAGA_MECHINPUTTOUCH_MECHINPUTTOUCH_TYPES_H_STOP
#endif
#endif

#undef SAGA_MECHINPUTTOUCH_MECHINPUTTOUCH_TYPES_H_STOP
