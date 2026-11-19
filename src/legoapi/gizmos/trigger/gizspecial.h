#pragma once

#include "nu2api/nucore/common.h"

#include "legoapi/gizmo/base/gizmo.h"

struct gameanimobjpool_s;

typedef struct gizspecialsys_s {         // size 0x10
    struct GIZSPECIAL_s *specials;       // 0x0
    i32 count;                           // 0x4
    u32 unknown_08;                      // 0x8
    struct gameanimobjpool_s *anim_pool; // 0xc
} GIZSPECIALSYS;

/* GizSpecial_GetOutput outputs, named after GizSpecial_GetOutputName. */
#define GIZSPECIAL_OUTPUT_AT_END 0
#define GIZSPECIAL_OUTPUT_NOT_AT_START 1

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 gizspecial_gizmotype_id;
    GIZMO *createGizSpecial(void *, char *);
    struct addgizmotype_s *GizSpecial_RegisterGizmo(i32 type_id);
#ifdef __cplusplus
}
#endif
