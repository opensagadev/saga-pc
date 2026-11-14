/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "decomp.h"
#include "gameapi/ai/aisys/aipath.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nu3d_includes.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/numath/nuvec.h"
#endif

typedef struct kaminoeplate_s {
    u8 opaque_00_13a[0x13a];
    u16 flags_13a;
    u8 opaque_13c_140[0x4];
} KAMINOEPLATE;
#ifdef __cplusplus
extern "C" {
#endif
    i32 KaminoE_CheckPlatHit(BOLT *bolt);
    extern NUVEC GunshipANorm;
    extern KAMINO_DISCO_MUSIC_VIEW KaminoDiscoMusicState;
    i32 KaminoInside(void);
    i32 KaminoDiscoOn(void);
    extern AREA_DATA_PC *KAMINO_ADATA;
    extern struct gizmo_s *forcetube;
    extern nuhspecial_s green_light;
    extern char *paint_light_name[3];
    extern char *paint_name[6];
    extern char *paint_panel_name[3];
    extern char *paint_target_colour[3];
    extern nuhspecial_s paintlights[3];
    extern struct gizmo_s *paintobst[6];
    extern struct gizmo_s *paintpanel[3];
    extern nuhspecial_s painttargetcolour[3];
#ifdef __cplusplus
}
#endif

typedef struct BONUS_GUNSHIP_B_NETPACKET { // size 0xc
    u8 progress;                           // 0x0
    u8 secondary;                          // 0x1
    u8 reserved_2[2];                      // 0x2
    f32 time;                              // 0x4
    u8 reserved_8[4];                      // 0x8
} BONUS_GUNSHIP_B_NETPACKET;

#ifdef __cplusplus
extern "C" {
#endif
    extern BONUS_GUNSHIP_B_NETPACKET *BonusGunshipBNetPacket;
    extern i32 GunshipPlayerDead;
    void BonusGunshipA_Reset(WORLDINFO *);
    void BonusGunshipA_Update(WORLDINFO *);
    void BonusGunshipB_Init(WORLDINFO *);
    void BonusGunshipB_Reset(WORLDINFO *);
    void BonusGunshipB_Update(WORLDINFO *);
    void BonusGunshipB_Panel(WORLDINFO *);
#ifdef __cplusplus
}
#endif

/* Original PC Jedi_B arena records; DWORD flags and eight-byte state alignment. */
typedef struct JEDIB_CREATURE {     // size 0x60
    NUVEC position;                 // 0x00
    i32 angle;                      // 0x0c
    i32 id;                         // 0x10
    GAMEOBJECT *object;             // 0x14
    struct JEDIB_CREATURE *partner; // 0x18
    AILOCATOR locator;              // 0x1c
    i32 field_0x58;                 // 0x58
    u32 flags;                      // 0x5c
} JEDIB_CREATURE;

typedef struct JEDIB_PHASE { // size 0x8
    i16 *id;                 // 0x0
    const char *script;      // 0x4
} JEDIB_PHASE;

typedef struct JEDIB_STATE {           // size 0x63f0
    JEDIB_CREATURE baddies[256];       // 0x0000
    JEDIB_CREATURE goodies[8];         // 0x6000
    i16 baddie_count;                  // 0x6300
    i16 goody_count;                   // 0x6302
    i16 phase;                         // 0x6304
    i16 state;                         // 0x6306
    f32 timer;                         // 0x6308
    u32 seed;                          // 0x630c
    NUSPECIALHANDLE hero_spawns[3][4]; // 0x6310
    GAMEOBJECT *heroes[3];             // 0x63a0
    GIZAIMESSAGE *msg_phase;           // 0x63ac
    GIZAIMESSAGE *msg_phase_complete;  // 0x63b0
    GIZAIMESSAGE *msg_objectives_left; // 0x63b4
    GIZAIMESSAGE *msg_restrain[3];     // 0x63b8
    i16 wave_ids[6];                   // 0x63c4
    u8 wave_spawned[6];                // 0x63d0
    u8 pad_0x63d6[2];                  // 0x63d6
    GAMEOBJECT *boss;                  // 0x63d8
    f32 wave_timer;                    // 0x63dc

    union { // size 0x8
        u64 exclusion_mask;

        struct {                     // size 0x8
            u32 exclusion_mask_low;  // 0x63e0
            u32 exclusion_mask_high; // 0x63e4
        };
    }; // 0x63e0

    u32 unique_spawned; // 0x63e8
} JEDIB_STATE;

typedef struct JEDIB_NETPACKET { // size 0x20
    i16 state;                   // 0x00
    i16 phase;                   // 0x02
    i16 count;                   // 0x04
    i16 pad_0x06;                // 0x06
    i16 ids[8];                  // 0x08
    u8 spawned[8];               // 0x18
} JEDIB_NETPACKET;

#ifdef __cplusplus
extern "C" {
#endif
    extern JEDIB_STATE jedi_b;
    extern i32 JediBActiveCount;
    extern i16 CharacterFixupIdPc007f19c8;
    extern i16 CharacterFixupIdPc007f19c4;
    extern i16 CharacterFixupIdPc007f19d0;
    extern i16 CharacterFixupIdPc007f19d4;
    extern i16 CharacterFixupIdPc007f19d8;
    extern JEDIB_NETPACKET *jedib_netpacket;
    extern JEDIB_PHASE jedi_b_phase1[8];
    extern JEDIB_PHASE jedi_b_phase2[8];
    extern JEDIB_PHASE jedi_b_phase3[8];
    extern f32 jedib_safe_r;
    extern NUVEC jedib_offsets[3];
    extern i32 jedib_yrot[3];
    extern i16 *JediB_playerids[3];
    void JediB_DrawPanel(WORLDINFO *);
#ifdef __cplusplus
}
#endif
