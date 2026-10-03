#include "saga.h"
typedef struct carry_plug_pc_s {
    char name[16];
    NUVEC position;
    u16 rotation_x, rotation_y, rotation_z;
    u8 enabled, flags;
    u8 unknown_24[0x10];
} CARRY_PLUG_PC;
typedef struct carry_plug_sys_pc_s {
    CARRY_PLUG_PC *plugs;
    i32 count;
} CARRY_PLUG_SYS_PC;
STATIC_ASSERT(sizeof(CARRY_PLUG_PC) == 0x34);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, position) == 0x10);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, rotation_x) == 0x1c);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, rotation_y) == 0x1e);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, rotation_z) == 0x20);
STATIC_ASSERT(offsetof(CARRY_PLUG_PC, flags) == 0x23);
STATIC_ASSERT(sizeof(CARRY_PLUG_SYS_PC) == 8);
STATIC_ASSERT(offsetof(CARRY_PLUG_SYS_PC, count) == 4);

typedef struct carry_blowup_pc_s {
    NUMTX transform;
    u8 unknown_40[0x9c - 0x40];
    u8 secondary_flags;
    u8 unknown_9d[0xa8 - 0x9d];
    f32 target_scale;
} CARRY_BLOWUP_PC;

typedef struct carry_world_pc_s {
    u8 unknown_0000[0x516c];
    CARRY_PLUG_SYS_PC *plug_sys;
} CARRY_WORLD_PC;
STATIC_ASSERT(offsetof(CARRY_BLOWUP_PC, transform) == 0);
STATIC_ASSERT(offsetof(CARRY_BLOWUP_PC, secondary_flags) == 0x9c);
STATIC_ASSERT(offsetof(CARRY_BLOWUP_PC, target_scale) == 0xa8);
STATIC_ASSERT(offsetof(CARRY_WORLD_PC, plug_sys) == 0x516c);
extern f32 SuperCarryImpactSpeedPc;
extern i32 SuperCarry_KeepObjectLevel, SuperCarry_AlignObject;
CARRY_PLUG_PC *__cdecl Plug_FindNearest(CARRY_PLUG_SYS_PC *, NUVEC *, f32 *, i32);
void __cdecl Plug_MakeDrawMtx(CARRY_PLUG_PC *, NUMTX *);
void __cdecl GizBlowup_Respawn(GIZMOBLOWUP_PC *);
void __cdecl SuperCarry_PartImpact(PART_PC *part) {
    f32 amount = NuVecMag(&part->velocity);
    if (amount >= SuperCarryImpactSpeedPc)
        amount = 1.0f;
    else
        amount /= SuperCarryImpactSpeedPc;
    if (amount > 0.0f)
        GameCam_Judder(NULL, (f32)(amount * 0.30000001192092896), qrand() < 0x8000 ? 0 : 2, &part->position);
}
i32 __cdecl SuperCarry_TurnBlowupBackOn(GIZMOBLOWUP_PC *blowup, NUVEC *position, u16 angle, i32 keep_matrix) {
    CARRY_BLOWUP_PC *view = (CARRY_BLOWUP_PC *)blowup;
    CARRY_PLUG_PC *plug;
    f32 distance;
    blowup->status_flags |= 0x804000;
    GizBlowup_Respawn(blowup);
    blowup->status_flags |= 0x10000;
    plug = Plug_FindNearest(((CARRY_WORLD_PC *)WORLD)->plug_sys, position, &distance, 1);
    if (plug != NULL && distance < 0.5625f) {
        Plug_MakeDrawMtx(plug, &view->transform);
        plug->flags |= 4;
        blowup->status_flags = (blowup->status_flags & ~0x800000) | 0x10000000;
        return 1;
    }
    if (!keep_matrix) {
        if (SuperCarry_AlignObject)
            NuMtxSetRotationY(&view->transform, angle);
        else
            view->transform = numtx_identity;
        NuMtxTranslate(&view->transform, position);
    }
    return 0;
}
