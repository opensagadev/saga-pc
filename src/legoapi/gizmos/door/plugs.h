#ifndef SAGA_LEGOAPI_GIZMOS_DOOR_PLUGS_H
#define SAGA_LEGOAPI_GIZMOS_DOOR_PLUGS_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "legoapi/actions/movement/carrying.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

#ifdef __cplusplus
extern "C" {
#endif
    struct carry_plug_pc_s *Plug_FindNearest(struct carry_plug_sys_pc_s *, NUVEC *, f32 *, i32);
    void Plug_MakeDrawMtx(struct carry_plug_pc_s *, NUMTX *);
    extern const char *PlugGizmoNamePc;
    extern const char *PlugOutputNamePc;
    extern i32 plug_gizmotype_id;
    struct addgizmotype_pc_s *Plugs_RegisterGizmo(i32);
#ifdef __cplusplus
}
#endif

#ifndef TYPES_OFF_15
typedef struct carry_plug_pc_s {
    char name[16];       // 0x00
    NUVEC position;      // 0x10
    u16 rotation_x;      // 0x1c
    u16 rotation_y;      // 0x1e
    u16 rotation_z;      // 0x20
    u8 enabled;          // 0x22
    u8 active : 1;       // 0x23:0
    u8 visible : 1;      // 0x23:1
    u8 plugged : 1;      // 0x23:2
    u8 unknown_23 : 5;   // 0x23:3
    u8 unknown_24[0x10]; // 0x24
} CARRY_PLUG_PC;         // size 0x34

typedef struct carry_plug_sys_pc_s {
    CARRY_PLUG_PC *plugs; // 0x0
    i32 count;            // 0x4
} CARRY_PLUG_SYS_PC;      // size 0x8

#endif /* TYPES_OFF_15 */
#endif
