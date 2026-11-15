#pragma once

#include "nu2api/nucore/common.h"

struct EdClass;

#ifdef __cplusplus

/* Receives the registry's object create/destroy/defunct/revive events. */
class EdObjectNotifier {
  public:
    virtual void NotifyCreateObject(void *, struct EdClass *, void *, i32, i32, i32) {
    }

    virtual void NotifyDestroyObject(void *, struct EdClass *, i32, i32) {
    }

    virtual void NotifyDefunctObject(void *, struct EdClass *, i32) {
    }

    virtual void NotifyReviveObject(void *, struct EdClass *, i32) {
    }
};
#endif
