#ifndef SAGA_LEGOAPI_WORLD_LEVELS_EPISODEII_H
#define SAGA_LEGOAPI_WORLD_LEVELS_EPISODEII_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

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
    extern KAMINO_DISCO_MUSIC_VIEW_PC KaminoDiscoMusicStatePc;
    i32 KaminoInside(void);
    i32 KaminoDiscoOn(void);
    extern AREA_DATA_PC *KAMINO_ADATA;
    extern struct gizmo_pc_s *forcetube;
    extern nuhspecial_s green_light;
    extern char *paint_light_name[3];
    extern char *paint_name[6];
    extern char *paint_panel_name[3];
    extern char *paint_target_colour[3];
    extern nuhspecial_s paintlights[3];
    extern struct gizmo_pc_s *paintobst[6];
    extern struct gizmo_pc_s *paintpanel[3];
    extern nuhspecial_s painttargetcolour[3];
#ifdef __cplusplus
}
#endif

typedef struct BONUS_GUNSHIP_B_NETPACKET_PC {
    u8 progress;
    u8 secondary;
    u8 reserved_2[2];
    f32 time;
    u8 reserved_8[4];
} BONUS_GUNSHIP_B_NETPACKET_PC;

STATIC_ASSERT(sizeof(BONUS_GUNSHIP_B_NETPACKET_PC) == 12);
STATIC_ASSERT(offsetof(BONUS_GUNSHIP_B_NETPACKET_PC, progress) == 0);
STATIC_ASSERT(offsetof(BONUS_GUNSHIP_B_NETPACKET_PC, secondary) == 1);
STATIC_ASSERT(offsetof(BONUS_GUNSHIP_B_NETPACKET_PC, time) == 4);
STATIC_ASSERT(offsetof(BONUS_GUNSHIP_B_NETPACKET_PC, reserved_8) == 8);
#ifdef __cplusplus
extern "C" {
#endif
    extern BONUS_GUNSHIP_B_NETPACKET_PC *BonusGunshipBNetPacketPc;
    extern i32 GunshipPlayerDeadPc;
    void BonusGunshipA_ResetPc(WORLDINFO *);
    void BonusGunshipA_UpdatePc(WORLDINFO *);
    void BonusGunshipB_InitPc(WORLDINFO *);
    void BonusGunshipB_ResetPc(WORLDINFO *);
    void BonusGunshipB_UpdatePc(WORLDINFO *);
    void BonusGunshipB_PanelPc(WORLDINFO *);
#ifdef __cplusplus
}
#endif

/* Original PC Jedi_B arena records; DWORD flags and eight-byte state alignment. */
typedef struct JEDIB_CREATURE_PC {
    NUVEC position;
    i32 angle;
    i32 id;
    GAMEOBJECT *object;
    struct JEDIB_CREATURE_PC *partner;
    AILOCATOR_PC locator;
    i32 field_0x58;
    u32 flags;
} JEDIB_CREATURE_PC;

STATIC_ASSERT(sizeof(JEDIB_CREATURE_PC) == 0x60);
STATIC_ASSERT(offsetof(JEDIB_CREATURE_PC, locator) == 0x1c);
STATIC_ASSERT(offsetof(JEDIB_CREATURE_PC, flags) == 0x5c);

typedef struct JEDIB_PHASE_PC {
    i16 *id;
    const char *script;
} JEDIB_PHASE_PC;

STATIC_ASSERT(sizeof(JEDIB_PHASE_PC) == 8);

typedef struct JEDIB_STATE_PC {
    JEDIB_CREATURE_PC baddies[256];
    JEDIB_CREATURE_PC goodies[8];
    i16 baddie_count;
    i16 goody_count;
    i16 phase;
    i16 state;
    f32 timer;
    u32 seed;
    NUSPECIALHANDLE hero_spawns[3][4];
    GAMEOBJECT *heroes[3];
    GIZAIMESSAGE *msg_phase;
    GIZAIMESSAGE *msg_phase_complete;
    GIZAIMESSAGE *msg_objectives_left;
    GIZAIMESSAGE *msg_restrain[3];
    i16 wave_ids[6];
    u8 wave_spawned[6];
    u8 pad_0x63d6[2];
    GAMEOBJECT *boss;
    f32 wave_timer;

    union {
        u64 exclusion_mask;

        struct {
            u32 exclusion_mask_low;
            u32 exclusion_mask_high;
        };
    };

    u32 unique_spawned;
} JEDIB_STATE_PC;

STATIC_ASSERT(sizeof(JEDIB_STATE_PC) == 0x63f0);
STATIC_ASSERT(offsetof(JEDIB_STATE_PC, goodies) == 0x6000);
STATIC_ASSERT(offsetof(JEDIB_STATE_PC, baddie_count) == 0x6300);
STATIC_ASSERT(offsetof(JEDIB_STATE_PC, goody_count) == 0x6302);
STATIC_ASSERT(offsetof(JEDIB_STATE_PC, phase) == 0x6304);
STATIC_ASSERT(offsetof(JEDIB_STATE_PC, state) == 0x6306);
STATIC_ASSERT(offsetof(JEDIB_STATE_PC, timer) == 0x6308);
STATIC_ASSERT(offsetof(JEDIB_STATE_PC, seed) == 0x630c);
STATIC_ASSERT(offsetof(JEDIB_STATE_PC, hero_spawns) == 0x6310);
STATIC_ASSERT(offsetof(JEDIB_STATE_PC, heroes) == 0x63a0);
STATIC_ASSERT(offsetof(JEDIB_STATE_PC, msg_restrain) == 0x63b8);
STATIC_ASSERT(offsetof(JEDIB_STATE_PC, wave_ids) == 0x63c4);
STATIC_ASSERT(offsetof(JEDIB_STATE_PC, wave_spawned) == 0x63d0);
STATIC_ASSERT(offsetof(JEDIB_STATE_PC, boss) == 0x63d8);
STATIC_ASSERT(offsetof(JEDIB_STATE_PC, wave_timer) == 0x63dc);
STATIC_ASSERT(offsetof(JEDIB_STATE_PC, exclusion_mask) == 0x63e0);
STATIC_ASSERT(offsetof(JEDIB_STATE_PC, unique_spawned) == 0x63e8);

typedef struct JEDIB_NETPACKET_PC {
    i16 state;
    i16 phase;
    i16 count;
    i16 pad_0x06;
    i16 ids[8];
    u8 spawned[8];
} JEDIB_NETPACKET_PC;

STATIC_ASSERT(sizeof(JEDIB_NETPACKET_PC) == 0x20);
STATIC_ASSERT(offsetof(JEDIB_NETPACKET_PC, phase) == 2);
STATIC_ASSERT(offsetof(JEDIB_NETPACKET_PC, count) == 4);
STATIC_ASSERT(offsetof(JEDIB_NETPACKET_PC, ids) == 8);
STATIC_ASSERT(offsetof(JEDIB_NETPACKET_PC, spawned) == 0x18);
#ifdef __cplusplus
extern "C" {
#endif
    extern JEDIB_STATE_PC jedi_b;
    extern i32 JediBActiveCountPc;
    extern i16 CharacterFixupIdPc007f19c8;
    extern i16 CharacterFixupIdPc007f19c4;
    extern i16 CharacterFixupIdPc007f19d0;
    extern i16 CharacterFixupIdPc007f19d4;
    extern i16 CharacterFixupIdPc007f19d8;
    extern JEDIB_NETPACKET_PC *jedib_netpacket;
    extern JEDIB_PHASE_PC jedi_b_phase1[8];
    extern JEDIB_PHASE_PC jedi_b_phase2[8];
    extern JEDIB_PHASE_PC jedi_b_phase3[8];
    extern f32 jedib_safe_r;
    extern NUVEC jedib_offsets[3];
    extern i32 jedib_yrot[3];
    extern i16 *JediB_playerids[3];
    void JediB_DrawPanelPc(WORLDINFO *);
#ifdef __cplusplus
}
#endif

#endif
