/* Split from saga.h (tools/headersplit.py, docs/header-plan.md). saga.h includes this header at the place of its
 * declarations. */

#pragma once

#include "nu2api/nucore/common.h"

/* The module headers this header uses. saga.h includes every header itself, so the shim skips these. */
#ifndef SAGA_SHIM
#include "nu2api/numath/nuvec.h"
#endif

struct AIPACKET;
struct AISYS_PC;
struct APIOBJECT;
struct gameobject_s;

typedef i32 (*AI_PREPARING_SPECIAL_MOVE_PC)(struct AIPACKET *, struct APIOBJECT *, i32);

#ifdef __cplusplus
extern "C" {
#endif
    extern AI_PREPARING_SPECIAL_MOVE_PC PreparingForSpecialMoveFn;

    typedef struct BATARANG_TARGET_s { // size 0x14
        void *object;                  // 0x00
        u8 type;                       // 0x04
        u8 lost;                       // 0x05
        u16 reserved_06_word;          // 0x06

        union { // size 0xc
            u8 reserved_08[12];
            NUVEC fallback_position;
        }; // 0x08
    } BATARANG_TARGET_s;

#pragma pack(push, 1)

    typedef struct BATARANG_FLIGHT_STATE { // size 0x7
        u8 in_flight;                      // 0x0
        u8 current_target;                 // 0x1
        u8 reserved_7f;                    // 0x2
        f32 flight_time;                   // 0x3
    } BATARANG_FLIGHT_STATE;

    typedef struct BATARANG_RICOCHET_STATE { // size 0x6
        i8 count;                            // 0x0
        u8 flags;                            // 0x1
        f32 timer;                           // 0x2
    } BATARANG_RICOCHET_STATE;

#pragma pack(pop)

    typedef struct player_batarang_slot_s { // size 0xb4

        union { // size 0xb4
            u8 unknown_00[0xb4];

            struct { // size 0xb4

                union { // size 0x7c
                    u8 targeting_reserved_00[0x7c];

                    struct {                          // size 0x7c
                        BATARANG_TARGET_s targets[5]; // 0x00
                        NUVEC position;               // 0x64
                        NUVEC velocity;               // 0x70
                    };
                }; // 0x00

                u8 active; // 0x7c

                union { // size 0x7
                    u8 targeting_reserved_7d[7];
                    BATARANG_FLIGHT_STATE flight_state;
                }; // 0x7d

                struct gameobject_s *owner; // 0x84
                NUVEC sight_position;       // 0x88
                NUVEC sight_velocity;       // 0x94

                union { // size 0xc
                    u8 targeting_reserved_a0[12];
                    NUVEC ricochet_normal;
                }; // 0xa0

                i16 cooldown; // 0xac

                union { // size 0x6
                    u8 targeting_reserved_ae[6];
                    BATARANG_RICOCHET_STATE ricochet_state;
                }; // 0xae
            };
        }; // 0x00
    } PLAYER_BATARANG_SLOT_PC;

    extern PLAYER_BATARANG_SLOT_PC BatarangPackets[8];
    extern i16 PunchRangeUnitId;
    typedef i32 (*AI_MID_SPECIAL_MOVE_PC)(struct AISYS_PC *, struct AIPACKET *, struct APIOBJECT *);

    extern AI_MID_SPECIAL_MOVE_PC MidSpecialMoveFn;
    extern const f64 PCPathArcSineCoefficient3;
    extern const f64 PCPathArcSineCoefficient5;
    extern const f64 PCPathArcSineCoefficient7;
    extern const f64 PCPathArcSineCoefficient9;
    void PlayerInitCapabilitiesDefault(struct gameobject_s *);
    extern f32 BatarangCatwomanStunTime;
#ifdef __cplusplus
}
#endif
