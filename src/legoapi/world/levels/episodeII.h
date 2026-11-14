#ifndef SAGA_LEGOAPI_WORLD_LEVELS_EPISODEII_H
#define SAGA_LEGOAPI_WORLD_LEVELS_EPISODEII_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */
/* Included via saga.h only for now: decomp.h, legoapi/gizmo/base/gizmo.h, legoapi/items/base/apiobject.h,
 * legoapi/legoapi_types.h, legoapi/lib/hub.h, nu2api/nu3d/nu3d_includes.h (not all of it earlier in the shim). */

#include "gameapi/ai/aisys/aipath.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nuhspecial.h"
#include "nu2api/nucore/common.h"
#include "nu2api/nucore/fixed_width.h"
#include "nu2api/numath/nuvec.h"

#ifndef SAGA_SHIM
#include "decomp.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/items/base/apiobject.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/lib/hub.h"
#include "nu2api/nu3d/nu3d_includes.h"
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

#ifndef TYPES_OFF_31
typedef struct BONUS_GUNSHIP_FLAGS_PC {
    u8 progress;          // 0x0
    u8 secondary;         // 0x1
    u8 reserved[14];      // 0x2
} BONUS_GUNSHIP_FLAGS_PC; // size 0x10

typedef struct dooku_c_pc_s {
    GIZAIMESSAGE *total_message; // 0x00
    GIZAIMESSAGE *hit_message;   // 0x04
    NUSPECIALHANDLE node;        // 0x08
} DOOKU_C_PC;                    // size 0x14

typedef struct kaminodisco_pc_s {
    AIAREA_PC *area;                 // 0x000
    NUSPECIALHANDLE off[16];         // 0x004
    NUSPECIALHANDLE pending[16];     // 0x0c4
    NUSPECIALHANDLE occupied[16];    // 0x184
    NUSPECIALHANDLE complete[16];    // 0x244
    NUSPECIALHANDLE final[16];       // 0x304
    i8 tile_state[16];               // 0x3c4
    i8 tile_count;                   // 0x3d4
    i8 phase;                        // 0x3d5
    i8 tile[2];                      // 0x3d6
    f32 timer;                       // 0x3d8
    u8 completion_sound;             // 0x3dc
    u8 reserved_3dd[3];              // 0x3dd
    GIZAIMESSAGE *next_tile_message; // 0x3e0
    GIZAIMESSAGE *complete_message;  // 0x3e4
} KAMINODISCO_PC;                    // size 0x3e8

typedef struct kaminoc_packet_pc_s {
    i16 off_mask;      // 0x0
    i16 pending_mask;  // 0x2
    i16 occupied_mask; // 0x4
    i16 complete_mask; // 0x6
    i16 final_mask;    // 0x8
    i16 sound_mask;    // 0xa
    u8 complete;       // 0xc
    u8 reserved_0d;    // 0xd
} KAMINOC_PACKET_PC;   // size 0xe

typedef struct kaminoe_turret_reader_pc_s {
    u8 opaque_00_1c[0x1c];                  // 0x000
    struct gameanimobj_s *primary_anim_obj; // 0x01c
    u8 opaque_20_e4[0xc4];                  // 0x020
    NUVEC *target_position;                 // 0x0e4
    u8 opaque_e8_12c[0x44];                 // 0x0e8
    u8 field_12c;                           // 0x12c
    u8 opaque_12d;                          // 0x12d
    u8 field_12e;                           // 0x12e
    u8 opaque_12f_13a[0xb];                 // 0x12f
    u16 flags_13a;                          // 0x13a
    u8 opaque_13c_140[0x4];                 // 0x13c
} KAMINOE_TURRET_READER_PC;                 // size 0x140

typedef struct kamino_e_pc_s {
    GIZAIMESSAGE *jango_fight;               // 0x00
    GIZAIMESSAGE *can_fire;                  // 0x04
    GIZAIMESSAGE *reset_turrets;             // 0x08
    GIZAIMESSAGE *show_hearts;               // 0x0c
    GIZAIMESSAGE *minicut_started;           // 0x10
    CUTINFO *intro_cutscene;                 // 0x14
    AIAREA_PC *landing_area;                 // 0x18
    NUSPECIALHANDLE slave1;                  // 0x1c
    KAMINOE_TURRET_READER_PC *turrets[4];    // 0x28
    KAMINOE_TURRET_READER_PC *active_turret; // 0x38
    void *panels[4];                         // 0x3c
    NUVEC position;                          // 0x4c
    i32 pitch;                               // 0x58
    i32 yaw;                                 // 0x5c
    i32 roll;                                // 0x60
    i32 orbit_pitch;                         // 0x64
    i32 orbit_yaw;                           // 0x68
    f32 departure_timer;                     // 0x6c
    f32 elapsed_time;                        // 0x70
    i8 fire_index;                           // 0x74
    u8 departing;                            // 0x75
    i16 platform;                            // 0x76
} KAMINO_E_PC;                               // size 0x78

typedef struct KAMINO_SHIP_ZERO_ANGLES_PC {
    u16 pitch;                // 0x0
    i32 yaw;                  // 0x4
} KAMINO_SHIP_ZERO_ANGLES_PC; // size 0x8

#endif /* TYPES_OFF_31 */
#endif
