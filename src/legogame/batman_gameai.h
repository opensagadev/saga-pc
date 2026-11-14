#ifndef SAGA_LEGOGAME_BATMAN_GAMEAI_H
#define SAGA_LEGOGAME_BATMAN_GAMEAI_H

/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"
#endif

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"
#endif

struct AIPACKET_PC;
struct AISYS_PC;
struct APIOBJECT_PC;
struct gameobject_s;

typedef i32 (*AI_PREPARING_SPECIAL_MOVE_PC)(struct AIPACKET_PC *, struct APIOBJECT_PC *, i32);
#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)

    typedef struct BATARANG_FLIGHT_STATE_PC {
        u8 in_flight;
        u8 current_target;
        u8 reserved_7f;
        f32 flight_time;
    } BATARANG_FLIGHT_STATE_PC;

    typedef struct BATARANG_RICOCHET_STATE_PC {
        i8 count;
        u8 flags;
        f32 timer;
    } BATARANG_RICOCHET_STATE_PC;

#pragma pack(pop)

    typedef struct player_batarang_slot_pc_s {
        union {
            u8 unknown_00[0xb4];

            struct {
                union {
                    u8 targeting_reserved_00[0x7c];

                    struct {
                        BATARANG_TARGET_s targets[5];
                        NUVEC position;
                        NUVEC velocity;
                    };
                };

                u8 active;

                union {
                    u8 targeting_reserved_7d[7];
                    BATARANG_FLIGHT_STATE_PC flight_state;
                };

                struct gameobject_s *owner;
                NUVEC sight_position;
                NUVEC sight_velocity;

                union {
                    u8 targeting_reserved_a0[12];
                    NUVEC ricochet_normal;
                };

                i16 cooldown;

                union {
                    u8 targeting_reserved_ae[6];
                    BATARANG_RICOCHET_STATE_PC ricochet_state;
                };
            };
        };
    } PLAYER_BATARANG_SLOT_PC;

    STATIC_ASSERT(sizeof(BATARANG_FLIGHT_STATE_PC) == 7);
    STATIC_ASSERT(sizeof(BATARANG_RICOCHET_STATE_PC) == 6);
    STATIC_ASSERT(sizeof(PLAYER_BATARANG_SLOT_PC) == 0xb4);
    STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, position) == 0x64);
    STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, velocity) == 0x70);
    STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, active) == 0x7c);
    STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, targeting_reserved_7d) == 0x7d);
    STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, flight_state.in_flight) == 0x7d);
    STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, flight_state.current_target) == 0x7e);
    STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, flight_state.flight_time) == 0x80);
    STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, owner) == 0x84);
    STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, sight_position) == 0x88);
    STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, sight_velocity) == 0x94);
    STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, ricochet_normal) == 0xa0);
    STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, cooldown) == 0xac);
    STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, targeting_reserved_ae) == 0xae);
    STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, ricochet_state.count) == 0xae);
    STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, ricochet_state.flags) == 0xaf);
    STATIC_ASSERT(offsetof(PLAYER_BATARANG_SLOT_PC, ricochet_state.timer) == 0xb0);

    STATIC_ASSERT(sizeof(PLAYER_BATARANG_SLOT_PC) == 0xb4);
    extern PLAYER_BATARANG_SLOT_PC BatarangPacketsPc[8];
    extern i16 PunchRangeUnitIdPc;
    typedef i32 (*AI_MID_SPECIAL_MOVE_PC)(struct AISYS_PC *, struct AIPACKET_PC *, struct APIOBJECT_PC *);
    extern const f64 PCPathArcSineCoefficient3;
    extern const f64 PCPathArcSineCoefficient5;
    extern const f64 PCPathArcSineCoefficient7;
    extern const f64 PCPathArcSineCoefficient9;
    void PlayerInitCapabilitiesDefaultPc(struct gameobject_s *);
    extern f32 BatarangCatwomanStunTimePc;
#ifdef __cplusplus
}
#endif

#endif
