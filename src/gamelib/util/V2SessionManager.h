#ifndef SAGA_GAMELIB_UTIL_V2SESSIONMANAGER_H
#define SAGA_GAMELIB_UTIL_V2SESSIONMANAGER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/items/collect/bolts.h"
#endif

#ifdef __cplusplus

class V2PendingList {
  public:
    V2PendingList();

    V2Peer *head;
    V2Peer *tail;
    i32 count;
};

class V2ClosingList {
  public:
    V2ClosingList();

    V2Peer *head;
    V2Peer *tail;
    i32 count;
};
#endif

#endif
