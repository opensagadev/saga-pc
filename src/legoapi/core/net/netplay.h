#ifndef SAGA_LEGOAPI_CORE_NET_NETPLAY_H
#define SAGA_LEGOAPI_CORE_NET_NETPLAY_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/characters/core/characters.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 LegoContextNetWaitPc;
    extern f32 LevelHackSendTimer;
    extern APICHARACTERMODELLIST NetFreePlayModelList[49];
    extern u8 net_recievedFreePlayList;
    extern i8 net_FreePlayModelCount;
#ifdef __cplusplus
}
#endif

#endif
