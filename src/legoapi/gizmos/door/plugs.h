#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

typedef struct carry_plug_s { // size 0x34
    char name[16];            // 0x00
    NUVEC position;           // 0x10
    u16 rotation_x;           // 0x1c
    u16 rotation_y;           // 0x1e
    u16 rotation_z;           // 0x20
    u8 enabled;               // 0x22
    u8 active : 1;            // 0x23:0
    u8 visible : 1;           // 0x23:1
    u8 plugged : 1;           // 0x23:2
    u8 reserved_23_3 : 5;        // 0x23:3
    u8 reserved_24[0x10];      // 0x24
} CARRY_PLUG;

typedef struct carry_plug_sys_s { // size 0x8
    CARRY_PLUG *plugs;            // 0x0
    i32 count;                    // 0x4
} CARRY_PLUG_SYS;

#ifdef __cplusplus
extern "C" {
#endif
    CARRY_PLUG *Plug_FindNearest(CARRY_PLUG_SYS *, NUVEC *, f32 *, i32);
    void Plug_MakeDrawMtx(CARRY_PLUG *, NUMTX *);
    extern const char *PlugGizmoName;
    extern const char *PlugOutputName;
    extern i32 plug_gizmotype_id;
    struct addgizmotype_s *Plugs_RegisterGizmo(i32);
#ifdef __cplusplus
}
#endif
