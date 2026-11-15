#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmo/base/gizmo.h"

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 gizspecial_gizmotype_id;
    GIZMO_PC *createGizSpecial(void *, char *);
    struct addgizmotype_s *GizSpecial_RegisterGizmo(i32 type_id);
#ifdef __cplusplus
}
#endif
