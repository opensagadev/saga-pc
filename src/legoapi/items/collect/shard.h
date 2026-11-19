#pragma once

#include "nu2api/nucore/common.h"

struct SHARD_s;
struct SIGNAL_s;
struct WORLDINFO_s;
struct addgizmotype_s;
struct gameobject_s;

typedef struct AREA_SAVE_PICKUP { // size 0xc

    union { // size 0xc

        struct {                    // size 0xc
            u8 reserved_00[5];        // 0x0
            u8 minikit_count;       // 0x5
            u8 red_brick_collected; // 0x6
            u8 reserved_07[5];       // 0x7
        };

        struct {              // size 0xc
            u8 complete;      // 0x0
            u8 area_complete; // 0x1

            union { // size 0x5
                u8 newgame_reserved_02[5];

                struct {                              // size 0x5
                    u8 true_hero_complete[2];         // 0x2
                    u8 minikit_complete;              // 0x4
                    u8 preserved_minikit_count;       // 0x5
                    u8 preserved_red_brick_collected; // 0x6
                };
            }; // 0x2

            u8 challenge_unlocked;    // 0x7
            f32 challenge_trial_time; // 0x8
        };
    }; // 0x0
} AREA_SAVE_PICKUP;

#ifdef __cplusplus
extern "C" {
#endif
    void Shards_HandleLostObj(struct WORLDINFO_s *, struct gameobject_s *);
    void Signal_SetVisibility(struct SIGNAL_s *signal, i32 visible);
    struct addgizmotype_s *Shards_RegisterGizmo(i32);
    extern void Shard_Collect(struct SHARD_s *, struct gameobject_s *);
#ifdef __cplusplus
}
#endif
