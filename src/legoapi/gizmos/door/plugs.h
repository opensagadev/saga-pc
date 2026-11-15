#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

struct carry_plug_sys_s;

#ifdef __cplusplus
extern "C" {
#endif
    struct carry_plug_s *Plug_FindNearest(struct carry_plug_sys_s *, NUVEC *, f32 *, i32);
    void Plug_MakeDrawMtx(struct carry_plug_s *, NUMTX *);
    extern const char *PlugGizmoName;
    extern const char *PlugOutputName;
    extern i32 plug_gizmotype_id;
    struct addgizmotype_s *Plugs_RegisterGizmo(i32);
#ifdef __cplusplus
}
#endif
