#pragma once

#include "nu2api/nucore/common.h"

#include "nu2api/numath/numtx.h"
#include "nu2api/numath/nuvec.h"

struct WORLDINFO_s;
struct gameobject_s;

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct SUIT_INITIALIZER_RECORD { // size 0x18
        char *base_character_name;           // 0x00
        char *suit_character_name;           // 0x04
        i16 *text_id;                        // 0x08
        u16 flags;                           // 0x0c
        u8 group;                            // 0x0e
        char letter;                         // 0x0f
        u32 character_flags;                 // 0x10
        i16 character_id;                    // 0x14
        u8 index;                            // 0x16
        u8 reserved_17;                       // 0x17
    } SUIT_INITIALIZER_RECORD;

#ifdef __cplusplus
}
#endif
typedef struct SIGNALPROGRESS_s SIGNALPROGRESS_s;

struct SIGNALPROGRESS_s {  // size 0x28
    char suit_letters[32]; // 0x00
    u32 active_mask[1];    // 0x20
    u32 visible_mask[1];   // 0x24
};

typedef struct SIGNAL_s SIGNAL_s;

struct SIGNAL_s {          // size 0x94
    char name[16];         // 0x00
    NUMTX matrix;          // 0x10
    NUVEC position;        // 0x50
    u16 yaw;               // 0x5c
    u16 terrain_pitch;     // 0x5e
    u16 terrain_roll;      // 0x60
    i16 animation_0;       // 0x62
    i16 animation_1;       // 0x64
    i16 animation_2;       // 0x66
    char character_letter; // 0x68
    char suit_letter;      // 0x69

    union { // size 0x1
        u8 flags;

        struct {                   // size 0x1
            u8 visible : 1;        // 0x6a:0
            u8 active : 1;         // 0x6a:1
            u8 in_use : 1;         // 0x6a:2
            u8 reserved_flags : 5; // 0x6a:3
        };
    }; // 0x6a

    u8 reserved_6b;                            // 0x6b
    f32 radius;                           // 0x6c
    f32 scale;                            // 0x70
    NUVEC normal;                         // 0x74
    struct SUIT_INITIALIZER_RECORD *suit; // 0x80
    NUVEC target_position;                // 0x84
    i16 platform_id;                      // 0x90
    u16 reserved_92;                           // 0x92
};

/* SIGNAL_s.flags, the bitfields */
#define SIGNAL_VISIBLE 0x1
#define SIGNAL_ACTIVE 0x2
#define SIGNAL_IN_USE 0x4

#ifdef __cplusplus
extern "C" {
#endif
    struct SIGNAL_s *Signal_FindNearest(struct WORLDINFO_s *world, struct nuvec_s *position, struct gameobject_s *object, f32 *distance);
    void Signal_MoveCode(struct WORLDINFO_s *, struct gameobject_s *);
    void SuitBlowupCode(struct WORLDINFO_s *, struct gameobject_s *);
#ifdef __cplusplus
}
#endif
