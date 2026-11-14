#ifndef SAGA_GAMEAPI_EDTOOLS_EDOBJECTNOTIFIER_H
#define SAGA_GAMEAPI_EDTOOLS_EDOBJECTNOTIFIER_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "editor/edlevelall.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "editor/edlevelall.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "editor/edlevelall.h"
#endif

#ifdef __cplusplus

/* Receives the registry's object create/destroy/defunct/revive events. */
class EdObjectNotifier {
  public:
    virtual void NotifyCreateObject(void *, EdClass *, void *, i32, i32, i32) {
    }

    virtual void NotifyDestroyObject(void *, EdClass *, i32, i32) {
    }

    virtual void NotifyDefunctObject(void *, EdClass *, i32) {
    }

    virtual void NotifyReviveObject(void *, EdClass *, i32) {
    }
};
#endif

#endif
