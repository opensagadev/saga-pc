/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

#ifdef __cplusplus

class V2Peer;

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
