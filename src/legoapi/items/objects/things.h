#ifndef SAGA_LEGOAPI_ITEMS_OBJECTS_THINGS_H
#define SAGA_LEGOAPI_ITEMS_OBJECTS_THINGS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: legoapi/legoapi_types.h (not all of it earlier in the shim). */

#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#endif

#ifdef __cplusplus

extern "C" GameThingManager *theGameThings;
void CreateThingManager();
#endif

#endif
